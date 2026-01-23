#include "pch.h"  
#include "Pack.h"
#include <cstring>
#include <iostream>
#include <stdexcept>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

// ------------------ 构造函数 ------------------

CPack::CPack()
    : m_nPackID(0), m_nPackLen(0)
{
    std::memset(m_szPackData, 0, sizeof(m_szPackData));
}

CPack::CPack(int id, const char* data, int len) {
    setPackID(id);
    setPackData(data, len);
}

// ------------------ 序列化（组包） ------------------

std::vector<char> CPack::serialize(const CPack& pack) {
    int totalSize = HEADER_SIZE + pack.m_nPackLen;
    std::vector<char> buffer(totalSize);

    // 包ID（网络字节序）
    uint32_t netID = htonl(static_cast<uint32_t>(pack.m_nPackID));
    std::memcpy(buffer.data(), &netID, sizeof(netID));

    // 包长度（网络字节序）
    uint32_t netLen = htonl(static_cast<uint32_t>(pack.m_nPackLen));
    std::memcpy(buffer.data() + sizeof(netID), &netLen, sizeof(netLen));

    // 包数据
    if (pack.m_nPackLen > 0) {
        std::memcpy(buffer.data() + HEADER_SIZE, pack.m_szPackData, pack.m_nPackLen);
    }

    return buffer;
}

// ------------------ 反序列化（解包） ------------------

std::vector<CPack> CPack::deserializeAll(const char* data, size_t dataSize) {
    std::vector<CPack> packs;
    size_t offset = 0;

    while (offset + HEADER_SIZE <= dataSize) {
        // 读取 body 长度（跳过 ID，从第4字节开始）
        uint32_t netLen;
        std::memcpy(&netLen, data + offset + 4, sizeof(netLen));
        uint32_t bodyLen = ntohl(netLen);

        // 安全检查：防止非法长度
        if (bodyLen > static_cast<uint32_t>(MAX_PACK_DATA_SIZE)) {
            break; // 视为非法数据，停止解析
        }

        size_t totalPackSize = HEADER_SIZE + bodyLen;
        if (offset + totalPackSize > dataSize) {
            break; // 包不完整，等待更多数据
        }

        // 构造新包
        CPack pack;
        uint32_t netID;
        std::memcpy(&netID, data + offset, sizeof(netID));
        pack.m_nPackID = static_cast<int>(ntohl(netID));
        pack.m_nPackLen = static_cast<int>(bodyLen);
        if (bodyLen > 0) {
            std::memcpy(pack.m_szPackData, data + offset + HEADER_SIZE, bodyLen);
        }
        else {
            std::memset(pack.m_szPackData, 0, sizeof(pack.m_szPackData));
        }

        packs.push_back(std::move(pack));
        offset += totalPackSize;
    }
   
    return packs;
}

// ------------------ Getter / Setter ------------------

void CPack::setPackID(int id) {
    m_nPackID = id;
}

int CPack::getPackID() const {
    return m_nPackID;
}

bool CPack::setPackData(const char* data, int len) {
    if (!data || len <= 0) {
        m_nPackLen = 0;
        return false;
    }
    m_nPackLen = (len > MAX_PACK_DATA_SIZE) ? MAX_PACK_DATA_SIZE : len;
    std::memcpy(m_szPackData, data, m_nPackLen);
    return true;
}

const char* CPack::getData() const {
    return m_szPackData;
}

int CPack::getDataLen() const {
    return m_nPackLen;
}

// ------------------ 工具函数 ------------------

void CPack::print() const {
    std::cout << "[CPack] ID=" << m_nPackID
        << ", Len=" << m_nPackLen
        << ", Data='" << std::string(m_szPackData, m_nPackLen) << "'\n";
}
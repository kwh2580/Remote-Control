#include "Pack.h"
#include <stdexcept>
#include <string>

// ------------------ 构造函数 ------------------

CPack::CPack()
    : m_nPackID(0), m_nPackLen(0)
{
    // std::vector 初始为空，无需 memset
}

CPack::CPack(int id, const char* data, int len) {
    setPackID(id);
    setPackData(data, len);
}

// ------------------ 序列化（组包） ------------------

std::vector<char> CPack::serialize(const CPack& pack) {
    int totalSize = HEADER_SIZE + pack.m_nPackLen;
    std::vector<char> buffer(totalSize);

    // 包ID（转为网络字节序）
    uint32_t netID = htonl(static_cast<uint32_t>(pack.m_nPackID));
    std::memcpy(buffer.data(), &netID, sizeof(netID));

    // 包数据长度（转为网络字节序）
    uint32_t netLen = htonl(static_cast<uint32_t>(pack.m_nPackLen));
    std::memcpy(buffer.data() + sizeof(netID), &netLen, sizeof(netLen));

    // 拷贝实际数据（如果有的话）
    if (pack.m_nPackLen > 0) {
        std::memcpy(buffer.data() + HEADER_SIZE, pack.m_szPackData.data(), pack.m_nPackLen);
    }

    return buffer;
}

// ------------------ 反序列化（解包） ------------------

std::vector<CPack> CPack::deserializeAll(const char* data, size_t dataSize, size_t& bytesConsumed) {
    std::vector<CPack> packs;
    size_t offset = 0;
    bytesConsumed = 0;

    while (offset + HEADER_SIZE <= dataSize) {
        // 读取 PackID（4字节）
        uint32_t netID;
        std::memcpy(&netID, data + offset, sizeof(uint32_t));
        uint32_t packID = ntohl(netID);

        // 读取 Body 长度（4字节）
        uint32_t netLen;
        std::memcpy(&netLen, data + offset + sizeof(uint32_t), sizeof(uint32_t));
        uint32_t bodyLen = ntohl(netLen);

        // 安全检查：防止超大包攻击
        if (bodyLen > static_cast<uint32_t>(MAX_PACK_DATA_SIZE)) {
            packs.clear();
            return packs; // 或可根据需求改为 break / 抛异常
        }

        // 检查是否收齐完整包：header(8) + body(bodyLen)
        size_t totalSize = HEADER_SIZE + static_cast<size_t>(bodyLen);
        if (offset + totalSize > dataSize) {
            break; // 数据不完整，等待下一次接收
        }

        // 构造新包
        CPack pack;
        pack.m_nPackID = static_cast<int>(packID);
        pack.m_nPackLen = static_cast<int>(bodyLen);

        // 拷贝 body 数据到 vector
        if (bodyLen > 0) {
            pack.m_szPackData.assign(
                data + offset + HEADER_SIZE,
                data + offset + HEADER_SIZE + bodyLen
            );
        }
        else {
            pack.m_szPackData.clear();
        }

        packs.push_back(std::move(pack));
        offset += totalSize;
    }

    bytesConsumed = offset;
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
    if (len < 0 || len > MAX_PACK_DATA_SIZE) {
        return false;
    }
    if (data == nullptr && len > 0) {
        return false;
    }
    m_szPackData.assign(data, data + len);
    m_nPackLen = len;
    return true;
}

void CPack::clear() {
    m_nPackID = 0;
    m_nPackLen = 0;
    m_szPackData.clear();
}

bool CPack::Packsend(SOCKET sock) const {
    std::vector<char> buffer = serialize(*this);
    int total = static_cast<int>(buffer.size());
    int sent = 0;
    while (sent < total) {
        int ret = send(sock, buffer.data() + sent, total - sent, 0);
        if (ret <= 0) {
            return false; // 发送失败
        }
        sent += ret;
    }
    return true;
}

const char* CPack::getData() const {
    if (m_szPackData.empty()) {
        return nullptr;
    }
    return m_szPackData.data(); 
}

int CPack::getDataLen() const {
    return m_nPackLen; 
}

// ------------------ 工具函数 ------------------

void CPack::print() const {
    std::cout << "[CPack] ID=" << m_nPackID
        << ", Len=" << m_nPackLen;
    if (m_nPackLen > 0 && m_nPackLen < 100) { // 避免打印大二进制数据
        std::string preview(m_szPackData.data(), m_nPackLen);
        std::cout << ", Data='" << preview << "'";
    }
    std::cout << "\n";
}
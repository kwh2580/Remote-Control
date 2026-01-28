// CPack.h
#pragma once

#include "pch.h"
#include <vector>
#include <cstddef> // for size_t
#include <cstring>
#include <iostream>

//// 跨平台网络字节序支持
//#ifdef _WIN32
//#include <winsock2.h>
//#pragma comment(lib, "ws2_32.lib")
//#else
//#include <arpa/inet.h>
//#endif

class CPack {
public:
    static const int MAX_PACK_DATA_SIZE = 1024 * 1024; // 1MB
    static const int HEADER_SIZE = 8; // 4 (ID) + 4 (Len)

    // 构造函数
    CPack();
    CPack(int id, const char* data, int len);

    // 序列化 / 反序列化
    static std::vector<char> serialize(const CPack& pack);
    static std::vector<CPack> deserializeAll(const char* data, size_t dataSize, size_t& bytesConsumed);

    // Getter / Setter
    void setPackID(int id);
    int getPackID() const;
    bool setPackData(const char* data, int len);
    void clear();
    bool Packsend(SOCKET sock) const;
    const char* getData() const;
    int getDataLen() const;

    // 工具
    void print() const;

private:
    int m_nPackID;
    int m_nPackLen;
    std::vector<char> m_szPackData; // 使用 vector 存储数据
};
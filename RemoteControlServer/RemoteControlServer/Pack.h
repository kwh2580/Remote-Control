// CPack.h
#pragma once


#include <iostream>
#include <vector>
#include <cstddef> // for size_t

// 跨平台网络字节序支持
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif



/*
    @包含结构定义
    前面4个字节为id
    4-8 字节为数据长度
	9-1024字节为数据字节为包体长
*/



class CPack {
public:
    static const int MAX_PACK_DATA_SIZE = 1024 * 1024;
    static const int HEADER_SIZE = 8; // sizeof(int) * 2

    // 构造函数
    CPack();
    CPack(int id, const char* data, int len);

    // 组包：单个 CPack → 二进制 buffer
    static std::vector<char> serialize(const CPack& pack);

    // 解包：从连续数据流中提取所有完整包
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
    std::vector<char> m_szPackData;
    //char m_szPackData[MAX_PACK_DATA_SIZE];
};
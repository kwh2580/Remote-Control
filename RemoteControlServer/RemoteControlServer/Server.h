// CServer.h
#pragma once

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include "Pack.h"

// 前向声明（如果需要可加，此处非必需）
class CServer;

class CServer {
public:
    // 禁止拷贝和赋值
    CServer(const CServer&) = delete;
    CServer& operator=(const CServer&) = delete;

    // 单例访问点
    static CServer& getInstance();

    // 启动服务器（监听指定端口）
    bool start(u_short port);

    // 停止服务器
    void stop();

    // 是否正在运行
    bool isRunning() const;

private:
    SOCKET server_fd_ = INVALID_SOCKET;
    std::atomic<bool> isRunning_{ false };

    // 私有构造 / 析构
    CServer();
    ~CServer();

    // 内部方法
    void acceptLoop();
    void handleClient(SOCKET client_sock);
    void handleError(const char* msg);
    void cleanup();
};
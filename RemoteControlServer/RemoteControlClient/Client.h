#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <stdexcept>
#include <functional>
#include <thread>
#include <unordered_map>
#include <mutex>
#include "Pack.h"
#include "MfcLogger.h"

// 客户端连接类
class CClient {
public:
    // 定义回调函数类型：参数为接收到的数据包，返回void
    using PackHandler = std::function<void(const CPack&)>;

    // 构造函数：初始化Winsock
    CClient() : client_sock_(INVALID_SOCKET), is_connected_(false), is_async_running_(false) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw std::runtime_error("WSAStartup failed: " + std::to_string(result));
        }
    }

    // 析构函数：清理资源
    ~CClient() {
        stopAsyncReceive();
        disconnect();
        WSACleanup();
    }

    // 禁用拷贝（每个客户端连接唯一）
    CClient(const CClient&) = delete;
    CClient& operator=(const CClient&) = delete;

    // 连接到服务器
    bool connect(const std::string& server_ip, u_short server_port) {
        if (is_connected_) {
            std::cerr << "Client is already connected!\n";
            return false;
        }

        // 创建套接字
        client_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (client_sock_ == INVALID_SOCKET) {
            handleError("socket creation failed");
            return false;
        }

        // 配置服务器地址
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);

        // 将IP字符串转换为网络地址
        if (InetPtonA(AF_INET, server_ip.c_str(), &server_addr.sin_addr) != 1) {
            handleError("invalid server IP address");
            cleanup();
            return false;
        }

        // 连接服务器
        if (::connect(client_sock_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            handleError("connect to server failed");
            cleanup();
            return false;
        }

        is_connected_ = true;
        server_ip_ = server_ip;
        server_port_ = server_port;
        std::cout << "Successfully connected to server: " << server_ip << ":" << server_port << std::endl;
        return true;
    }

    // 断开连接
    void disconnect() {
        if (is_connected_.exchange(false) && client_sock_ != INVALID_SOCKET) {
            closesocket(client_sock_);
            client_sock_ = INVALID_SOCKET;
            std::cout << "Disconnected from server: " << server_ip_ << ":" << server_port_ << std::endl;
        }
    }

    // 发送单个数据包到服务器
    bool sendPack(const CPack& pack) {
        if (!is_connected_) {
            std::cerr << "Not connected to server!\n";
            return false;
        }

        // 序列化数据包
        std::vector<char> send_data = CPack::serialize(pack);
        if (send_data.empty()) {
            std::cerr << "Empty pack data!\n";
            return false;
        }

        // 发送数据
        int bytes_sent = send(client_sock_, send_data.data(), static_cast<int>(send_data.size()), 0);
        if (bytes_sent == SOCKET_ERROR) {
            handleError("send pack failed");
            disconnect();
            return false;
        }

        std::cout << "Sent pack (ID: " << pack.getPackID() << ") to server, "
            << bytes_sent << " bytes sent\n";
        return true;
    }

    // 【原有】接收服务器响应（阻塞式）
    std::string receiveResponse(int buffer_size = 1024) {
        if (!is_connected_) {
            std::cerr << "Not connected to server!\n";
            return "";
        }

        char buffer[1024];
        int bytes_received = recv(client_sock_, buffer, buffer_size - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cerr << "Server closed the connection!\n";
            }
            else {
                handleError("receive response failed");
            }
            disconnect();
            return "";
        }

        // 处理接收到的数据
        buffer[bytes_received] = '\0';
        std::string response(buffer);
        std::cout << "Received " << bytes_received << " bytes from server: " << response << std::endl;
        return response;
    }

    // 【新增】绑定packID对应的处理函数
    void bindPackHandler(int pack_id, const PackHandler& handler) {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        handlers_[pack_id] = handler;
        std::cout << "Bound handler for pack ID: " << pack_id << std::endl;
    }


    bool CClient::startAsyncReceive(int buffer_size /*= 4096*/) {
        if (!is_connected_) {
            std::cerr << "Not connected to server, cannot start async receive!\n";
            return false;
        }

        if (is_async_running_.exchange(true)) {
            std::cerr << "Async receive is already running!\n";
            return false;
        }

        // 启动异步接收线程
        async_thread_ = std::thread([this, buffer_size]() {
            std::vector<char> recvBuffer; // 👈 关键：累积缓冲区
            std::vector<char> tempBuf(buffer_size);

            while (is_async_running_ && is_connected_) {
                int n = recv(client_sock_, tempBuf.data(), static_cast<int>(tempBuf.size()), 0);
                if (n <= 0) {
                    if (n == 0) {
                        std::cout << "[Async] Server closed the connection.\n";
                    }
                    else {
                        int err = WSAGetLastError();
                        if (err != WSAEINTR) {
                            std::cerr << "[Async] recv failed: " << err << "\n";
                        }
                    }
                    break;
                }
                LOG_INFO(CString(_T("Raw recv: ")) + CString(std::to_string(n).c_str()) + _T(" bytes"));
                // 将新数据追加到累积缓冲区
                recvBuffer.insert(recvBuffer.end(), tempBuf.begin(), tempBuf.begin() + n);

                // 👇 循环解析所有完整包
                size_t bytesConsumed = 0;
                auto packs = CPack::deserializeAll(recvBuffer.data(), recvBuffer.size(), bytesConsumed);


                if (!packs.empty()) {
                    handleReceivedPacks(packs);
                }

                // ✅ 移除已处理的数据
                if (bytesConsumed > 0) {
                    recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + bytesConsumed);
                }

                // 可选：防止缓冲区无限增长（如遇到非法数据）
                if (recvBuffer.size() > 10 * 1024 * 1024) { // 10MB
                    std::cerr << "[Async] Receive buffer too large, possible protocol error!\n";
                    break;
                }
            }

            // 清理
            is_async_running_ = false;
            disconnect(); // 确保 socket 关闭
            std::cout << "[Async] Receive thread stopped.\n";
            });

        async_thread_.detach();
        std::cout << "[Async] Receive thread started.\n";
        return true;
    }
    // 【新增】停止异步接收
    void stopAsyncReceive() {
        if (is_async_running_.exchange(false)) {
            // 唤醒可能阻塞在recv的套接字（Windows下可关闭套接字触发退出）
            if (client_sock_ != INVALID_SOCKET) {
                shutdown(client_sock_, SD_RECEIVE);
            }
            std::cout << "[Async] Stopping receive thread...\n";
        }
    }

    // 获取连接状态
    bool isConnected() const { return is_connected_; }
    bool isAsyncRunning() const { return is_async_running_; }
    const std::string& getServerIP() const { return server_ip_; }
    u_short getServerPort() const { return server_port_; }

private:
    // 处理接收到的数据包（调用绑定的回调）
    void handleReceivedPacks(const std::vector<CPack>& packs) {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        for (const auto& pack : packs) {

            // 查找对应的处理函数
            auto it = handlers_.find(pack.getPackID());
            if (it != handlers_.end()) {
                try {
                    // 执行自定义处理逻辑
                    it->second(pack);
                }
                catch (const std::exception& e) {
                    std::cerr << "[Async] Handler for pack ID " << pack.getPackID()
                        << " threw exception: " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "[Async] No handler bound for pack ID: " << pack.getPackID() << std::endl;
            }
        }
    }

    // 错误处理
    void handleError(const char* msg) {
        int err = WSAGetLastError();
        std::cerr << "[CLIENT ERROR] " << msg << ", WSAGLE=" << err << std::endl;
    }

    // 清理资源
    void cleanup() {
        if (client_sock_ != INVALID_SOCKET) {
            closesocket(client_sock_);
            client_sock_ = INVALID_SOCKET;
        }
        is_connected_ = false;
    }

    // 成员变量
    SOCKET client_sock_;                          // 客户端套接字
    std::atomic<bool> is_connected_;              // 连接状态（线程安全）
    std::string server_ip_;                       // 服务器IP
    u_short server_port_;                         // 服务器端口

    // 异步接收相关
    std::atomic<bool> is_async_running_;          // 异步接收线程运行状态
    std::thread async_thread_;                    // 异步接收线程
    std::unordered_map<int, PackHandler> handlers_; // packID -> 处理函数映射
    std::mutex handler_mutex_;                    // 保护handlers_的线程安全
};
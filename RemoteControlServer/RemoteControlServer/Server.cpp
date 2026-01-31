// CServer.cpp
#include "Server.h"
#include <WS2tcpip.h>   // 用于 InetNtopA
#include <stdexcept>
#include <string>
#include "handle.h"

#pragma comment(lib, "ws2_32.lib")

// ==================== 公共接口 ====================

CServer& CServer::getInstance() {
    static CServer instance;
    return instance;
}

bool CServer::start(u_short port) {
    if (isRunning_.exchange(true)) {
        std::cerr << "Server is already running!\n";
        return false;
    }

    server_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd_ == INVALID_SOCKET) {
        handleError("socket creation failed");
        isRunning_ = false;
        return false;
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        handleError("setsockopt(SO_REUSEADDR) failed");
        cleanup();
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        handleError("bind failed");
        cleanup();
        return false;
    }

    if (listen(server_fd_, 5) == SOCKET_ERROR) {
        handleError("listen failed");
        cleanup();
        return false;
    }

    std::cout << "TCP Server listening on port " << port << std::endl;
    acceptLoop();
    return true;
}

void CServer::stop() {
    if (isRunning_) {
        isRunning_ = false;
        if (server_fd_ != INVALID_SOCKET) {
            closesocket(server_fd_);
            server_fd_ = INVALID_SOCKET;
        }
    }
}

bool CServer::isRunning() const {
    return isRunning_;
}

// ==================== 私有实现 ====================

CServer::CServer() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        wchar_t msg[256];
        wsprintfW(msg, L"WSAStartup failed with error: %d", result);
        MessageBoxW(NULL, msg, L"Error", MB_OK | MB_ICONERROR);
        throw std::runtime_error("Failed to initialize Winsock");
    }
}

CServer::~CServer() {
    stop();
    WSACleanup();
}

void CServer::acceptLoop() {
    while (isRunning_) {
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);

        SOCKET client_sock = accept(server_fd_, (sockaddr*)&client_addr, &client_len);
        if (client_sock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (err == WSAEINTR || err == WSAECONNABORTED) {
                continue;
            }
            handleError("accept failed");
            break;
        }

        char ip_str[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        std::cout << "New connection from " << ip_str << ":"
            << ntohs(client_addr.sin_port) << std::endl;

        handleClient(client_sock);
        //closesocket(client_sock);
    }
}

void CServer::handleClient(SOCKET client_sock) {
    std::vector<char> recvBuffer; 

    while (isRunning_) {
        char tempBuf[4096];
        int n = recv(client_sock, tempBuf, sizeof(tempBuf), 0);
        if (n <= 0) {
            if (n == 0) {
                std::cout << "Client disconnected.\n";
            }
            else {
                std::cerr << "recv failed: " << WSAGetLastError() << "\n";
            }
            break;
        }


        recvBuffer.insert(recvBuffer.end(), tempBuf, tempBuf + n);


        size_t bytesConsumed = 0;
        auto packs = CPack::deserializeAll(recvBuffer.data(), recvBuffer.size(), bytesConsumed);

        if (!packs.empty()) {
            for (auto& pack : packs) {
                pack.print();
                switch (pack.getPackID()) {
                case 1: handle::HandleGetDiskInformation(client_sock, pack); break; // 获取磁盘信息
                case 2: handle::HandleGetFileList(client_sock, pack); break; // 获取文件列表
                case 3: handle::HandleOpenFile(client_sock, pack); break; // 打开文件
                case 4: handle::HandleSendFile(client_sock, pack); break;// 发送文件
                case 5: handle::HandleMouseMove(client_sock, pack); break;// 移动鼠标
                case 6: handle::HandleScreenMonitor(client_sock, pack); break;// 屏幕监控
                case 7: handle::HandleLock(client_sock, pack); break;// 锁屏
                case 8: handle::HandleStopLock(client_sock, pack); break;
                case 9: handle::HandleStopScreenMonitor(client_sock, pack); break;
                default:
                    std::cout << "Received unknown pack ID: " << pack.getPackID() << "\n";
                    break;
                }
            }

            recvBuffer.erase(recvBuffer.begin(), recvBuffer.begin() + bytesConsumed);
        }


    }

    closesocket(client_sock);
}

void CServer::handleError(const char* msg) {
    int err = WSAGetLastError();
    std::cerr << "[ERROR] " << msg << ", WSAGLE=" << err << std::endl;
}

void CServer::cleanup() {
    if (server_fd_ != INVALID_SOCKET) {
        closesocket(server_fd_);
        server_fd_ = INVALID_SOCKET;
    }
    isRunning_ = false;
}
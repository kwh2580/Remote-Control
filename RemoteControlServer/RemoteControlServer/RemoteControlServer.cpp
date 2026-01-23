// tcp_server_win.cpp
#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "TCP Server listening on port " << PORT << std::endl;

    sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    while (true) {
        SOCKET client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        // 使用 InetNtopA 替代已弃用的 inet_ntoa，避免 C4996 警告
        char ipStr[INET_ADDRSTRLEN];
        const char* ipDisplay = nullptr;
        if (InetNtopA(AF_INET, &client_addr.sin_addr, ipStr, INET_ADDRSTRLEN) != NULL) {
            ipDisplay = ipStr;
        }
        else {
            ipDisplay = "unknown";
        }

        std::cout << "New connection from "
            << ipDisplay << ":"
            << ntohs(client_addr.sin_port) << std::endl;

        char buffer[BUFFER_SIZE];
        int bytes = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::cout << "Received: " << buffer << std::endl;
            const char* reply = "Hello from Windows TCP server\n";
            send(client_socket, reply, (int)strlen(reply), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
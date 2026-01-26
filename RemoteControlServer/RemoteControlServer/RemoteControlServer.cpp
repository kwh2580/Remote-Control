// tcp_server_win.cpp
#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Server.h"
#include "ServerFun.h"
#include "handle.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    //std::cout << CServerFun::GetDiskInformation() << std::endl;
   // std::cout << CServerFun::GetFileList("C:\\Users\\32404\\Desktop\\aaa") << std::endl;
    handle::HandleGetFileList(0, CPack());
    return 0;
    try {
        auto& server = CServer::getInstance();
        server.start(PORT);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return -1;
    }


}
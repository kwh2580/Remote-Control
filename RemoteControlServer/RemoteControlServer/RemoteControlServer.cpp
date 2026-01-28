// tcp_server_win.cpp
#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Server.h"
#include "ServerFun.h"
#include "handle.h"
#include <thread>   
#include <chrono>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    //std::cout << CServerFun::GetDiskInformation() << std::endl;
   // std::cout << CServerFun::GetFileList("C:\\Users\\32404\\Desktop\\aaa") << std::endl;
    //handle::HandleGetFileList(0, CPack());
   // CServerFun::CaptureScreenToJPG("min_full_screen.bmp",0,0,0,0,75);

    try {
        auto& server = CServer::getInstance();
        server.start(PORT);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return -1;
    }


}
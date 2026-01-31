#pragma once
#include "Server.h"
#include "Pack.h"
#include "ServerFun.h"
#include "json.hpp"
#include "stb_image_write.h"


// 前置声明（避免包含不必要的头文件，减少编译依赖）
class CPack;

// handle类声明
class handle
{
public:
    // 鼠标/文件相关结构体声明
    struct MyFileInfo
    {
        // 文件名
        char name[256];
        // 文件类型 true:目录 false:文件
        bool type;
        // 是否停止
        bool stop;
    };

    // 处理获取磁盘信息的请求
    static void HandleGetDiskInformation(SOCKET client_sock, CPack pack);

    // 处理获取文件列表的请求
    static void HandleGetFileList(SOCKET client_sock, CPack pack);

    // 处理打开文件的请求
    static void HandleOpenFile(SOCKET client_sock, CPack pack);

    // 处理发送文件的请求
    static void HandleSendFile(SOCKET client_sock, CPack pack);

    // 处理鼠标移动/按键操作的请求
    static void HandleMouseMove(SOCKET client_sock, CPack pack);

    // 处理屏幕监控的请求
    static void HandleScreenMonitor(SOCKET client_sock, CPack pack);
    // 处理停止屏幕监控的请求
    static void HandleStopScreenMonitor(SOCKET client_sock, CPack pack);
    

    // 处理锁屏的请求
    static void HandleLock(SOCKET client_sock, CPack pack);
    
    // 处理停止锁屏的请求
    static void HandleStopLock(SOCKET client_sock, CPack pack);
};

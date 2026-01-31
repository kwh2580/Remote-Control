#include "handle.h"
#include "json.hpp"

// 链接ShellExecute库
#pragma comment(lib, "shlwapi.lib")

// ===================== 函数实现 =====================
void handle::HandleGetDiskInformation(SOCKET client_sock, CPack pack) {
    // 调用 CServerFun::GetDiskInformation() 获取磁盘信息
    std::string diskInfo = CServerFun::GetDiskInformation();
    // 构造响应包
    CPack responsePack;
    responsePack.setPackID(1); // 假设1是获取磁盘信息的响应ID
    responsePack.setPackData(diskInfo.c_str(), static_cast<int>(diskInfo.size()));
    responsePack.Packsend(client_sock);
}

void handle::HandleGetFileList(SOCKET client_sock, CPack pack) {
    CPack responsePack;
    nlohmann::json sendjson;

    const char* dirData = pack.getData();
    int dirSize = pack.getDataLen();

    // 检查空路径
    if (!dirData || dirSize <= 0) {
        sendjson["name"] = "传入路径为空";
        sendjson["type"] = false;
        sendjson["stop"] = true;
        std::string errInfo = sendjson.dump();
        responsePack.setPackID(2);
        responsePack.setPackData(errInfo.c_str(), static_cast<int>(errInfo.size()));
        responsePack.Packsend(client_sock);
        return;
    }

    // UTF-8 char* -> std::wstring
    std::wstring wstrDir;
    int wstrSize = MultiByteToWideChar(CP_UTF8, 0, dirData, dirSize, nullptr, 0);
    if (wstrSize > 0) {
        wstrDir.resize(wstrSize);
        MultiByteToWideChar(CP_UTF8, 0, dirData, dirSize, &wstrDir[0], wstrSize);
    }
    else {
        wstrDir = L"C:\\Users\\32404\\Desktop\\v2rayN-v7.10.5"; // 默认路径
    }

    // 确保路径结尾有反斜杠
    if (!wstrDir.empty() && wstrDir.back() != L'\\' && wstrDir.back() != L'/') {
        wstrDir += L"\\";
    }
    std::wstring wstrSearchPath = wstrDir + L"*";

    WIN32_FIND_DATAW findData = { 0 };
    HANDLE hFind = FindFirstFileW(wstrSearchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD dwError = GetLastError();
        sendjson["name"] = "Send fail";
        sendjson["type"] = false;
        sendjson["stop"] = true;
        sendjson["error_code"] = dwError;
        std::string diskInfo = sendjson.dump();
        responsePack.setPackID(2);
        responsePack.setPackData(diskInfo.c_str(), static_cast<int>(diskInfo.size()));
        responsePack.Packsend(client_sock);
        return;
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
            continue;

        // 宽字符 -> UTF-8 std::string
        int len = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, nullptr, 0, nullptr, nullptr);
        std::string filename;
        if (len > 1) {
            filename.resize(len - 1);
            WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, &filename[0], len, nullptr, nullptr);
        }
        else {
            filename = "未知文件名";
        }

        // 构造 JSON 并发送
        sendjson.clear();
        sendjson["name"] = filename;
        sendjson["type"] = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        sendjson["stop"] = false;

        std::string diskInfo = sendjson.dump();
        std::cout << diskInfo << std::endl;

        responsePack.clear();
        responsePack.setPackID(2);
        responsePack.setPackData(diskInfo.c_str(), static_cast<int>(diskInfo.size()));
        responsePack.Packsend(client_sock); 

    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);

    // 发送结束标志
    sendjson.clear();
    sendjson["name"] = "Send completed";
    sendjson["type"] = false;
    sendjson["stop"] = true;
    std::string diskInfo = sendjson.dump();
    std::cout << diskInfo << std::endl;

    responsePack.clear();
    responsePack.setPackID(2);
    responsePack.setPackData(diskInfo.c_str(), static_cast<int>(diskInfo.size()));
    responsePack.Packsend(client_sock);
}

void handle::HandleOpenFile(SOCKET client_sock, CPack pack) {
    std::cout << "HandleOpenFile" << std::endl;
    std::string filePath(pack.getData(), pack.getDataLen());
    // 运行文件
    ShellExecuteA(NULL, NULL, filePath.c_str(), NULL, NULL, SW_SHOW);

    CPack responsePack;
    responsePack.clear();
    responsePack.setPackID(3);
    responsePack.Packsend(client_sock);
}

void handle::HandleSendFile(SOCKET client_sock, CPack pack) {
    CPack responsePack;
    std::cout << "HandleSendFile" << std::endl;

    // 1. 读取传入的文件路径
    const char* pathData = pack.getData();
    int pathLen = pack.getDataLen();
    if (pathData == nullptr || pathLen <= 0) {
        std::cerr << "File path is empty!" << std::endl;
        responsePack.clear();
        responsePack.setPackID(4);
        std::string errorMsg = "File path is empty";
        responsePack.setPackData(errorMsg.c_str(), static_cast<int>(errorMsg.size()));
        responsePack.Packsend(client_sock);
        return;
    }

    // 2. 转换UTF-8路径为宽字符
    std::wstring wFilePath;
    int wPathLen = MultiByteToWideChar(CP_UTF8, 0, pathData, pathLen, nullptr, 0);
    if (wPathLen == 0) {
        std::cerr << "Convert path failed! Error: " << GetLastError() << std::endl;
        responsePack.clear();
        responsePack.setPackID(4);
        std::string errorMsg = "Convert path failed";
        responsePack.setPackData(errorMsg.c_str(), static_cast<int>(errorMsg.size()));
        responsePack.Packsend(client_sock);
        return;
    }
    wFilePath.resize(wPathLen);
    MultiByteToWideChar(CP_UTF8, 0, pathData, pathLen, &wFilePath[0], wPathLen);

    // 3. 打开文件
    HANDLE hFile = CreateFileW(
        wFilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        std::cerr << "Error opening file! Path: " << std::string(pathData, pathLen)
            << ", Error: " << err << std::endl;
        responsePack.clear();
        responsePack.setPackID(4);
        std::string errorMsg = "Error opening file, err: " + std::to_string(err);
        responsePack.setPackData(errorMsg.c_str(), static_cast<int>(errorMsg.size()));
        responsePack.Packsend(client_sock);
        return;
    }

    // 👇 发送 "filebegin"
    responsePack.clear();
    responsePack.setPackID(4);
    std::string beginMsg = "filebegin";
    responsePack.setPackData(beginMsg.c_str(), static_cast<int>(beginMsg.size()));
    responsePack.Packsend(client_sock);

    // 4. 分块读取并发送文件 + 字节统计
    // 将buffer从栈上移到堆上，避免大栈分配
    std::unique_ptr<char[]> buffer(new char[1024]);
    DWORD bytesRead = 0;
    bool sendSuccess = true;
    ULONGLONG totalSentBytes = 0; // 👈 新增：统计发送的文件数据字节数

    while (true) {
        BOOL ret = ReadFile(hFile, buffer.get(), 1024, &bytesRead, NULL);

        if (!ret) {
            DWORD err = GetLastError();
            if (err != ERROR_HANDLE_EOF) {
                std::cerr << "Read file failed! Error: " << err << std::endl;
                sendSuccess = false;
                break;
            }
        }

        if (bytesRead > 0) {
            responsePack.clear();
            responsePack.setPackID(4);
            responsePack.setPackData(buffer.get(), static_cast<int>(bytesRead));
            if (!responsePack.Packsend(client_sock)) {
                sendSuccess = false;
                break;
            }
            totalSentBytes += bytesRead; // 👈 累加成功发送的字节数
        }

        if (bytesRead < 1024) {
            break; // EOF
        }
    }

    // 5. 关闭文件句柄
    CloseHandle(hFile);

    // 6. 发送完成/失败标识
    responsePack.clear();
    responsePack.setPackID(4);
    std::string endMsg = sendSuccess ? "File send completed" : "File send failed";
    responsePack.setPackData(endMsg.c_str(), static_cast<int>(endMsg.size()));
    responsePack.Packsend(client_sock);

    // 👇 打印统计信息
    std::cout << "File send " << (sendSuccess ? "completed" : "failed")
        << ", Path: " << std::string(pathData, pathLen)
        << ", Total data sent: " << totalSentBytes << " bytes" << std::endl;
}


void handle::HandleMouseMove(SOCKET client_sock, CPack pack) {
    std::cout << "HandleMouseMove" << std::endl;
    std::string mouseData(pack.getData(), pack.getDataLen());
    nlohmann::json jsonData = nlohmann::json::parse(mouseData);
    int type = jsonData["type"];
    if (type == 1) {
        int buttonType = jsonData["buttonType"];
        int eventType = jsonData["eventType"];
        CServerFun::HandleMouseEventByNum(buttonType, eventType);
    }
    if (type == 2) {
        int x = jsonData["x"];
        int y = jsonData["y"];
         CServerFun::MoveMouseTo(x, y);
    }


}
void handle::HandleScreenMonitor(SOCKET client_sock, CPack pack) {
    std::vector<BYTE> jpgData;
    CServerFun::CaptureScreenToJPGMemory(jpgData, 0, 0, 0, 0, 100);
    // 发送图片数据
    CPack responsePack;
    responsePack.clear();
    responsePack.setPackID(6);

    responsePack.setPackData(reinterpret_cast<char*>(jpgData.data()), static_cast<int>(jpgData.size()));
    responsePack.Packsend(client_sock);

}
//停止屏幕监控
void handle::HandleStopScreenMonitor(SOCKET client_sock, CPack pack) {
    //CServerFun::StopScreenMonitor();
}


//锁机
void handle::HandleLock(SOCKET client_sock, CPack pack) {
    CServerFun::DisableMouseKeyboard();
    BlockInput(TRUE);
    std::thread fullScreenThread(CServerFun::RunFullScreenWindow);
    fullScreenThread.detach();


}
//停止锁机
void handle::HandleStopLock(SOCKET client_sock, CPack pack) {
    BlockInput(FALSE);
    CServerFun::RestoreMouseKeyboard();
    CServerFun::CloseFullScreenWindow();
}
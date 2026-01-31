#pragma once
#include <string>
#include <windows.h>
#include <gdiplus.h>
#include <fileapi.h>
#include <sstream>
#include <iostream>
#include <vector>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

enum class MouseButton {
    LEFT = 1,    // 左键
    RIGHT = 2,   // 右键
    MIDDLE = 3   // 中键
};

// 鼠标事件类型枚举
enum class MouseEvent {
    CLICK = 1,    // 单击
    PRESS = 2,    // 按下
    RELEASE = 3,  // 抬起
    DOUBLE_CLICK1 = 4 // 双击
};

class CServerFun
{
    public:
        static std::string GetDiskInformation();
        static std::string GetFileList(const std::string& strDir);
		static std::string WstringToUtf8(const std::wstring& wstr); // utf8转wstring辅助函数：将宽字符串转换为UTF8字符串
        static void RecursiveTraverse(const std::string& strDir, std::ostringstream& oss);

        static bool HandleMouseEventByNum(int buttonType, int eventType);

        static bool HandleMouseEvent(MouseButton button, MouseEvent event);

        static bool MoveMouseTo(int x, int y);
        static bool CaptureScreenMinSize(const std::string& savePath, int x, int y, int width, int height);

        static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

        static bool CaptureScreenToJPG(const std::string& savePath, int x, int y, int width, int height, int quality);

        static bool CaptureScreenToJPGMemory(std::vector<BYTE>& jpgData, int x, int y, int width, int height, int quality);

        static bool SendJPGData(SOCKET clientSock, const std::vector<BYTE>& jpgData);

        static bool DisableMouseKeyboard();

        static bool RestoreMouseKeyboard();


        static LRESULT FullScreenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


        static void RunFullScreenWindow();

        static void CloseFullScreenWindow();


};


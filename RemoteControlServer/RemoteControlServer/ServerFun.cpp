#include "ServerFun.h"

std::string CServerFun::GetDiskInformation()
{
    std::string str;
    DWORD drives = GetLogicalDrives();  // 返回一个位掩码，bit0=A:, bit1=B:, ..., bit25=Z:

    for (char c = 'A'; c <= 'Z'; ++c) {
        if (drives & 1) {               // 检查当前位是否为1
            str += c;
            str += ',';
        }
        drives >>= 1;                   // 右移一位，检查下一个盘符
    }
	str = str.empty() ? "No drives found." : str.substr(0, str.length() - 1); // 去掉最后一个逗号

    return str;
}

std::string CServerFun::GetFileList(const std::string& strRootDir) {

    std::ostringstream oss;
    // 表头
    oss << "===== 所有文件/目录列表（根目录：" << strRootDir << "）=====\n";
    oss << "目录路径\t\t文件名/目录名\n";
    oss << "--------------------------------------------\n";

    // 调用递归函数遍历所有层级
    RecursiveTraverse(strRootDir, oss);

    oss << "--------------------------------------------\n";
    oss << "遍历完成\n";
    return oss.str();

}


std::string CServerFun::WstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}



void CServerFun::RecursiveTraverse(const std::string& strDir, std::ostringstream& oss) {
    std::wstring wstrDir = std::wstring(strDir.begin(), strDir.end());
    if (!wstrDir.empty() && wstrDir.back() != L'\\' && wstrDir.back() != L'/') {
        wstrDir += L"\\";
    }
    std::wstring wstrSearchPath = wstrDir + L"*";

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(wstrSearchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD dwError = GetLastError();
        oss << "[错误] 目录[" << strDir << "]遍历失败，错误码：" << dwError << "\n";
        return;
    }

    // 遍历当前目录下的所有项
    do {
        // 跳过 . 和 ..
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }

        // 转换文件/目录名到UTF8
        std::string strName = WstringToUtf8(findData.cFileName);
        // 拼接当前项的完整路径
        std::string strFullPath = strDir + "\\" + strName;

        // 判断是否是目录：如果是，递归遍历；如果是文件，直接记录
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 先记录当前子目录
            oss << strDir << "\t" << strName << " [子目录]\n";
            // 递归遍历该子目录
            RecursiveTraverse(strFullPath, oss);
        }
        else {
            // 记录文件
            oss << strDir << "\t" << strName << "\n";
        }

    } while (FindNextFileW(hFind, &findData));

    // 关闭句柄，释放资源
    FindClose(hFind);
}


bool CServerFun::HandleMouseEventByNum(int buttonType, int eventType) {
    // 转换按键类型
    MouseButton button;
    switch (buttonType) {
    case 1: button = MouseButton::LEFT; break; // 左键
    case 2: button = MouseButton::RIGHT; break; // 右键
    case 3: button = MouseButton::MIDDLE; break; // 中键
    default: return false;
    }

    // 转换事件类型
    MouseEvent event;
    switch (eventType) {
    case 1: event = MouseEvent::CLICK; break; // 单击
    case 2: event = MouseEvent::PRESS; break;// 按下
    case 3: event = MouseEvent::RELEASE; break;// 抬起
    case 4: event = MouseEvent::DOUBLE_CLICK1; break; // 双击
    default: return false;
    }

    // 调用核心函数
    return CServerFun::HandleMouseEvent(button, event);
}
bool CServerFun::HandleMouseEvent(MouseButton button, MouseEvent event) {
    // 定义鼠标事件标志位
    DWORD mouseEventFlag = 0;

    // 根据按键+事件组合设置对应的Windows API标志
    switch (button) {
    case MouseButton::LEFT: {
        switch (event) {
        case MouseEvent::PRESS:
            mouseEventFlag = MOUSEEVENTF_LEFTDOWN;
            break;
        case MouseEvent::RELEASE:
            mouseEventFlag = MOUSEEVENTF_LEFTUP;
            break;
        case MouseEvent::CLICK:
            // 单击 = 按下 + 抬起
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            return true;
        case MouseEvent::DOUBLE_CLICK1:
            // 双击 = 两次单击（系统自动识别双击，这里模拟两次单击）
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            Sleep(50); // 模拟双击间隔（系统默认500ms内两次单击为双击）
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            return true;
        default:
            return false;
        }
        break;
    }
    case MouseButton::RIGHT: {
        switch (event) {
        case MouseEvent::PRESS:
            mouseEventFlag = MOUSEEVENTF_RIGHTDOWN;
            break;
        case MouseEvent::RELEASE:
            mouseEventFlag = MOUSEEVENTF_RIGHTUP;
            break;
        case MouseEvent::CLICK:
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
            return true;
        case MouseEvent::DOUBLE_CLICK1:
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
            Sleep(50);
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
            return true;
        default:
            return false;
        }
        break;
    }
    case MouseButton::MIDDLE: {
        switch (event) {
        case MouseEvent::PRESS:
            mouseEventFlag = MOUSEEVENTF_MIDDLEDOWN;
            break;
        case MouseEvent::RELEASE:
            mouseEventFlag = MOUSEEVENTF_MIDDLEUP;
            break;
        case MouseEvent::CLICK:
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
            return true;
        case MouseEvent::DOUBLE_CLICK1:
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
            Sleep(50);
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0);
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
            return true;
        default:
            return false;
        }
        break;
    }
    default:
        return false;
    }

    // 执行按下/抬起操作
    if (mouseEventFlag != 0) {
        mouse_event(mouseEventFlag, 0, 0, 0, 0);
        return true;
    }

    return false;
}


bool CServerFun::MoveMouseTo(int x, int y) {
    // Windows API：设置鼠标绝对位置（需结合屏幕分辨率计算）
    // 方式1：SetCursorPos（简单直接，相对屏幕坐标）
    BOOL ret = SetCursorPos(x, y);
    if (!ret) {
       
        return false;
    }
    return true;
    // （可选）方式2：mouse_event（支持相对移动/绝对移动，更灵活）
    // mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 
    //             (x * 65535) / GetSystemMetrics(SM_CXSCREEN),  // 转换为0-65535的绝对坐标
    //             (y * 65535) / GetSystemMetrics(SM_CYSCREEN),
    //             0, 0);
}


bool CServerFun::CaptureScreenMinSize(const std::string& savePath, int x = 0, int y = 0, int width = 0, int height = 0)
{
    // 1. 获取屏幕DC和基础信息
    HDC hScreenDC = GetDC(NULL);
    if (hScreenDC == NULL)
    {
        std::cerr << "获取屏幕DC失败！错误码：" << GetLastError() << std::endl;
        return false;
    }

    // 获取屏幕分辨率
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 适配全屏/指定区域
    if (width == 0) width = screenWidth;
    if (height == 0) height = screenHeight;

    // 2. 创建兼容DC
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (hMemDC == NULL)
    {
        std::cerr << "创建兼容DC失败！错误码：" << GetLastError() << std::endl;
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 3. 定义8位位图信息（仅使用BI_RGB，放弃RLE压缩）
    BITMAPINFOHEADER bih = { 0 };
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 8;          // 8位索引色（核心体积优化）
    bih.biCompression = BI_RGB;  // 仅用无压缩，避免RLE兼容性问题
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 256;
    bih.biClrImportant = 256;

    // 分配连续内存：BITMAPINFOHEADER + 256个RGBQUAD调色板
    DWORD dwInfoSize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    BITMAPINFO* pbmi = (BITMAPINFO*)LocalAlloc(LPTR, dwInfoSize);
    if (pbmi == NULL)
    {
        std::cerr << "分配位图信息内存失败！错误码：" << GetLastError() << std::endl;
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 复制位图头信息
    memcpy(&pbmi->bmiHeader, &bih, sizeof(BITMAPINFOHEADER));

    // 初始化256色调色板（灰度调色板，体积最优）
    RGBQUAD* pPal = pbmi->bmiColors;
    for (int i = 0; i < 256; i++)
    {
        pPal[i].rgbBlue = (BYTE)i;
        pPal[i].rgbGreen = (BYTE)i;
        pPal[i].rgbRed = (BYTE)i;
        pPal[i].rgbReserved = 0;
    }

    // 4. 创建DIB位图
    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hMemDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hBitmap == NULL)
    {
        std::cerr << "创建8位位图失败！错误码：" << GetLastError() << std::endl;
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 5. 复制屏幕内容到位图
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

    // 6. 计算位图数据大小（处理行字节对齐）
    // 8位位图行字节数必须是4的倍数
    DWORD dwRowSize = ((width * 8 + 31) / 32) * 4;
    DWORD dwBmpSize = dwRowSize * height;

    // 分配内存存储位图数据
    char* lpbitmap = new char[dwBmpSize];
    if (!lpbitmap)
    {
        std::cerr << "分配位图数据内存失败！" << std::endl;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 7. 读取位图数据（关键修复：仅用BI_RGB格式读取）
    BOOL bRet = GetDIBits(
        hMemDC,          // 设备上下文
        hBitmap,         // 位图句柄
        0,               // 起始扫描行
        height,          // 扫描行数
        lpbitmap,        // 输出缓冲区
        pbmi,            // 位图信息
        DIB_RGB_COLORS   // 颜色格式
    );

    if (bRet == 0)
    {
        std::cerr << "读取位图数据失败！错误码：" << GetLastError() << std::endl;
        delete[] lpbitmap;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 8. 写入BMP文件
    HANDLE hFile = CreateFileA(
        savePath.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,  // 优化写入性能
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "创建文件失败！错误码：" << GetLastError() << std::endl;
        delete[] lpbitmap;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // 构造BMP文件头
    BITMAPFILEHEADER bmfHeader = { 0 };
    bmfHeader.bfType = 0x4D42;  // BM标识
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + dwInfoSize + dwBmpSize;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + dwInfoSize;  // 数据偏移量

    // 写入文件内容
    DWORD dwWritten;
    WriteFile(hFile, &bmfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, pbmi, dwInfoSize, &dwWritten, NULL);  // 写入Header+调色板
    WriteFile(hFile, lpbitmap, dwBmpSize, &dwWritten, NULL);

    // 9. 释放所有资源
    CloseHandle(hFile);
    delete[] lpbitmap;
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    LocalFree(pbmi);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    std::cout << "最小体积截图成功！保存路径：" << savePath << std::endl;
    return true;
}






// 最小体积截图函数（直接保存为 JPG）
// 参数说明：
// - savePath: 保存路径（必须是.jpg/.jpeg）
// - x, y: 截图左上角坐标
// - width, height: 截图宽高（0表示全屏）
// - quality: JPG压缩质量（0-100，0=体积最小/质量最低，100=体积最大/质量最高）
bool CServerFun::CaptureScreenToJPG(const std::string& savePath, int x = 0, int y = 0, int width = 0, int height = 0, int quality = 75)
{
    // 1. 初始化 GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 2. 获取屏幕DC和基础信息
    HDC hScreenDC = GetDC(NULL);
    if (hScreenDC == NULL)
    {
        std::cerr << "获取屏幕DC失败！错误码：" << GetLastError() << std::endl;
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 获取屏幕分辨率
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // 适配全屏/指定区域
    if (width == 0) width = screenWidth;
    if (height == 0) height = screenHeight;

    // 3. 创建兼容DC
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (hMemDC == NULL)
    {
        std::cerr << "创建兼容DC失败！错误码：" << GetLastError() << std::endl;
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 4. 定义8位位图信息（体积优化核心）
    BITMAPINFOHEADER bih = { 0 };
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 8;          // 8位索引色
    bih.biCompression = BI_RGB;  // 无压缩，保证兼容性
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 256;
    bih.biClrImportant = 256;

    // 分配连续内存：BITMAPINFOHEADER + 256个RGBQUAD调色板
    DWORD dwInfoSize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    BITMAPINFO* pbmi = (BITMAPINFO*)LocalAlloc(LPTR, dwInfoSize);
    if (pbmi == NULL)
    {
        std::cerr << "分配位图信息内存失败！错误码：" << GetLastError() << std::endl;
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 复制位图头信息
    memcpy(&pbmi->bmiHeader, &bih, sizeof(BITMAPINFOHEADER));

    // 初始化256色调色板（灰度调色板，体积最优）
    RGBQUAD* pPal = pbmi->bmiColors;
    for (int i = 0; i < 256; i++)
    {
        pPal[i].rgbBlue = (BYTE)i;
        pPal[i].rgbGreen = (BYTE)i;
        pPal[i].rgbRed = (BYTE)i;
        pPal[i].rgbReserved = 0;
    }

    // 5. 创建DIB位图
    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hMemDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hBitmap == NULL)
    {
        std::cerr << "创建8位位图失败！错误码：" << GetLastError() << std::endl;
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 6. 复制屏幕内容到位图
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY);

    // 7. 将HBITMAP转换为GDI+的Bitmap对象（关键：实现JPG编码）
    Bitmap* pBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
    if (pBitmap == NULL)
    {
        std::cerr << "转换为GDI+ Bitmap失败！" << std::endl;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 8. 设置JPG压缩质量
    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    encoderParams.Parameter[0].Value = &quality;

    // 9. 获取JPG编码器CLSID
    CLSID jpgClsid;
    if (CServerFun::GetEncoderClsid(L"image/jpeg", &jpgClsid) == -1)
    {
        std::cerr << "获取JPG编码器失败！" << std::endl;
        delete pBitmap;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 10. 转换保存路径为宽字符（GDI+要求）
    int wcharLen = MultiByteToWideChar(CP_ACP, 0, savePath.c_str(), -1, NULL, 0);
    WCHAR* wSavePath = new WCHAR[wcharLen];
    MultiByteToWideChar(CP_ACP, 0, savePath.c_str(), -1, wSavePath, wcharLen);

    // 11. 保存为JPG文件
    Status status = pBitmap->Save(wSavePath, &jpgClsid, &encoderParams);
    if (status != Ok)
    {
        std::cerr << "保存JPG失败！GDI+错误码：" << status << std::endl;
        delete[] wSavePath;
        delete pBitmap;
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 12. 释放所有资源
    delete[] wSavePath;
    delete pBitmap;
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    LocalFree(pbmi);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    GdiplusShutdown(gdiplusToken);

    std::cout << "JPG截图成功！保存路径：" << savePath
        << " | 压缩质量：" << quality << "%" << std::endl;
    return true;
}





// 核心：彩色截图转内存JPG实现
bool CServerFun::CaptureScreenToJPGMemory(std::vector<BYTE>& jpgData, int x, int y, int width, int height, int quality)
{
    jpgData.clear(); // 清空输出缓冲区，避免残留旧数据
    // 校验压缩质量：限制在0-100之间，避免GDI+报错
    if (quality < 0) quality = 0;
    if (quality > 100) quality = 100;

    // 1. 初始化GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartupOutput gdiplusOutput;
    Status gdiStatus = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiplusOutput);
    if (gdiStatus != Ok)
    {
        std::cerr << "GDI+初始化失败！错误码：" << gdiStatus << std::endl;
        return false;
    }

    // 2. 获取屏幕DC和全屏信息
    HDC hScreenDC = GetDC(NULL);
    if (hScreenDC == NULL)
    {
        std::cerr << "获取屏幕DC失败！Windows错误码：" << GetLastError() << std::endl;
        GdiplusShutdown(gdiplusToken);
        return false;
    }
    // 获取全屏宽高（入参为0时使用全屏）
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (width <= 0) width = screenWidth;
    if (height <= 0) height = screenHeight;
    // 校验截图区域：避免超出屏幕范围
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > screenWidth) width = screenWidth - x;
    if (y + height > screenHeight) height = screenHeight - y;
    if (width <= 0 || height <= 0)
    {
        std::cerr << "截图区域无效！宽高需大于0" << std::endl;
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 3. 创建兼容内存DC（用于绘制截图）
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (hMemDC == NULL)
    {
        std::cerr << "创建兼容DC失败！Windows错误码：" << GetLastError() << std::endl;
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 4. 配置24位真彩色位图信息头（核心：解决灰度问题，无需调色板）
    BITMAPINFOHEADER bih = { 0 };
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = -height; // 负号：按屏幕正序存储（从上到下），避免截图颠倒
    bih.biPlanes = 1;
    bih.biBitCount = 24; // 24位真彩色（RGB各8位），彻底抛弃8位灰度
    bih.biCompression = BI_RGB; // 无压缩
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0; // 24位真彩色无需调色板，设为0
    bih.biClrImportant = 0;

    // 分配位图信息内存（仅信息头，无调色板）
    DWORD dwInfoSize = sizeof(BITMAPINFOHEADER);
    BITMAPINFO* pbmi = (BITMAPINFO*)LocalAlloc(LPTR, dwInfoSize);
    if (pbmi == NULL)
    {
        std::cerr << "分配位图信息内存失败！Windows错误码：" << GetLastError() << std::endl;
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }
    memcpy(&pbmi->bmiHeader, &bih, sizeof(BITMAPINFOHEADER));

    // 5. 创建24位DIB位图（设备无关位图，兼容GDI+）
    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hMemDC, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hBitmap == NULL)
    {
        std::cerr << "创建24位真彩色位图失败！Windows错误码：" << GetLastError() << std::endl;
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 6. 将位图选入内存DC，复制屏幕内容到位图（核心截图操作）
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, x, y, SRCCOPY | CAPTUREBLT); // 加CAPTUREBLT：捕获分层窗口（如QQ、浏览器悬浮窗）
    SelectObject(hMemDC, hOldBitmap); // 恢复原位图，避免资源泄漏

    // 7. 将GDI位图转换为GDI+ Bitmap（用于压缩为JPG）
    Bitmap* pGdiBitmap = Bitmap::FromHBITMAP(hBitmap, NULL);
    if (pGdiBitmap == NULL || pGdiBitmap->GetLastStatus() != Ok)
    {
        std::cerr << "GDI位图转GDI+ Bitmap失败！GDI+错误码：" << (pGdiBitmap ? pGdiBitmap->GetLastStatus() : Status::GenericError) << std::endl;
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        if (pGdiBitmap) delete pGdiBitmap;
        return false;
    }

    // 8. 配置JPG压缩参数（自定义质量）
    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    encoderParams.Parameter[0].Value = &quality;

    // 9. 获取JPG编码器CLSID（GDI+压缩必须）
    CLSID jpgClsid;
    if (GetEncoderClsid(L"image/jpeg", &jpgClsid) == -1)
    {
        std::cerr << "获取JPG编码器CLSID失败！未安装JPG编码器" << std::endl;
        delete pGdiBitmap;
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 10. 创建内存流，将GDI+ Bitmap保存为JPG（核心：写入内存而非本地文件）
    IStream* pMemStream = NULL;
    HRESULT hr = CreateStreamOnHGlobal(NULL, TRUE, &pMemStream);
    if (FAILED(hr) || pMemStream == NULL)
    {
        std::cerr << "创建内存流失败！HRESULT：" << std::hex << hr << std::dec << std::endl;
        delete pGdiBitmap;
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }
    // 将Bitmap保存到内存流（JPG格式）
    Status saveStatus = pGdiBitmap->Save(pMemStream, &jpgClsid, &encoderParams);
    if (saveStatus != Ok)
    {
        std::cerr << "Bitmap保存为JPG内存流失败！GDI+错误码：" << saveStatus << std::endl;
        pMemStream->Release();
        delete pGdiBitmap;
        DeleteObject(hBitmap);
        LocalFree(pbmi);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);
        GdiplusShutdown(gdiplusToken);
        return false;
    }

    // 11. 从内存流读取JPG二进制数据到vector<BYTE>（输出参数）
    HGLOBAL hGlobal = NULL;
    GetHGlobalFromStream(pMemStream, &hGlobal);
    DWORD dwJpgSize = (DWORD)GlobalSize(hGlobal); // 获取JPG数据大小
    BYTE* pJpgBuffer = (BYTE*)GlobalLock(hGlobal); // 锁定内存，获取数据指针
    if (pJpgBuffer != NULL && dwJpgSize > 0)
    {
        jpgData.assign(pJpgBuffer, pJpgBuffer + dwJpgSize); // 复制到输出vector
    }
    // 解锁内存+释放内存流
    GlobalUnlock(hGlobal);
    pMemStream->Release();

    // 12. 释放所有资源（按创建逆序释放，避免内存泄漏/句柄泄漏）
    delete pGdiBitmap;
    DeleteObject(hBitmap);
    LocalFree(pbmi);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    GdiplusShutdown(gdiplusToken);

    // 最终校验：JPG数据是否有效
    if (jpgData.empty())
    {
        std::cerr << "生成的JPG内存数据为空！" << std::endl;
        return false;
    }

    // 打印成功信息
    std::cout << "【彩色截图成功】JPG内存数据大小：" << jpgData.size() << " 字节 | 截图区域："
        << x << "," << y << " " << width << "x" << height
        << " | 压缩质量：" << quality << "%" << std::endl;
    return true;
}

// 辅助函数：获取GDI+编码器CLSID（如JPG/PNG/BMP）
int CServerFun::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0; // 编码器数量
    UINT size = 0; // 编码器信息缓冲区大小

    ImageCodecInfo* pImageCodecInfo = NULL;

    // 获取编码器数量和缓冲区大小
    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1; // 无编码器

    // 分配编码器信息内存
    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;

    // 获取所有编码器信息
    GetImageEncoders(num, size, pImageCodecInfo);

    // 遍历查找指定格式的编码器
    for (UINT i = 0; i < num; i++)
    {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i; // 找到，返回索引
        }
    }

    // 未找到指定编码器
    free(pImageCodecInfo);
    return -1;
}


bool CServerFun::SendJPGData(SOCKET clientSock, const std::vector<BYTE>& jpgData)
{
    if (clientSock == INVALID_SOCKET || jpgData.empty())
    {
        std::cerr << "发送失败：无效Socket或空数据！" << std::endl;
        return false;
    }

    // 第一步：发送数据长度（4字节，网络字节序），让客户端知道要接收多少数据
    DWORD dataLen = (DWORD)jpgData.size();
    DWORD netLen = htonl(dataLen); // 转换为网络字节序（大端）
    int sendRet = send(clientSock, (const char*)&netLen, sizeof(DWORD), 0);
    if (sendRet == SOCKET_ERROR)
    {
        std::cerr << "发送数据长度失败！WSA错误码：" << WSAGetLastError() << std::endl;
        return false;
    }

    // 第二步：分块发送JPG二进制数据（避免单次发送过大）
    const int BUF_SIZE = 4096; // 每次发送4KB
    DWORD sentTotal = 0;
    while (sentTotal < dataLen)
    {
        DWORD sendSize = min((DWORD)BUF_SIZE, dataLen - sentTotal);
        sendRet = send(clientSock, (const char*)&jpgData[sentTotal], sendSize, 0);

        if (sendRet == SOCKET_ERROR)
        {
            std::cerr << "发送JPG数据失败！WSA错误码：" << WSAGetLastError() << std::endl;
            return false;
        }

        sentTotal += sendRet;
    }

    std::cout << "JPG数据发送完成！总发送字节：" << sentTotal << std::endl;
    return true;
}




// 全局变量：全屏窗口句柄和屏幕尺寸
HWND g_hFullScreenWnd = NULL;
const wchar_t* g_szClassName = L"TCPServerFullScreenClass";
int g_screenWidth, g_screenHeight;

// 全局变量定义（源文件中，头文件需加extern声明）
bool g_isInputDisabled = false;
int g_originalMouseSpeed = 0;
bool g_originalMouseVisible = true;
bool g_isWndClassRegistered = false;
// 【核心函数】禁用系统所有鼠标+键盘功能（无Hook，系统级屏蔽）
bool CServerFun::DisableMouseKeyboard()
{
    // 避免重复禁用
    if (g_isInputDisabled)
    {
        return true;
    }

    try
    {
        // 1. 保存鼠标原始配置（关键：恢复时精准还原，避免鼠标参数异常）
        // 获取原始鼠标移动速度
        SystemParametersInfo(SPI_GETMOUSESPEED, 0, &g_originalMouseSpeed, 0);
        // 获取原始鼠标指针显示状态
        g_originalMouseVisible = ShowCursor(TRUE) >= 0; // ShowCursor返回值判断可见性

        // 2. 禁用鼠标：设置移动速度为0（无法移动/点击）+ 隐藏鼠标指针
        SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)0, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
        while (ShowCursor(FALSE) >= 0); // 循环隐藏，确保指针完全消失

        // 3. 禁用键盘：系统原生API，屏蔽所有键盘输入（包括系统快捷键）
        if (!BlockInput(TRUE))
        {
            // 键盘禁用失败，兜底恢复鼠标配置
            RestoreMouseKeyboard();
            return false;
        }

        // 4. 标记禁用状态
        g_isInputDisabled = true;
        return true;
    }
    catch (...)
    {
        // 异常时兜底恢复
        RestoreMouseKeyboard();
        return false;
    }
}

// 【核心函数】恢复系统鼠标+键盘功能（精准还原原始配置）
bool CServerFun::RestoreMouseKeyboard()
{
    // 未禁用则直接返回
    if (!g_isInputDisabled)
    {
        return true;
    }

    try
    {
        // 1. 恢复键盘：解除系统输入屏蔽
        BlockInput(FALSE);

        // 2. 恢复鼠标：还原原始移动速度
        SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID)g_originalMouseSpeed, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

        // 3. 恢复鼠标：还原原始显示状态
        if (g_originalMouseVisible)
        {
            while (ShowCursor(TRUE) < 0); // 循环显示，确保指针正常出现
        }
        else
        {
            while (ShowCursor(FALSE) >= 0);
        }

        // 4. 重置禁用状态和全局变量
        g_isInputDisabled = false;
        g_originalMouseSpeed = 0;
        g_originalMouseVisible = true;

        return true;
    }
    catch (...)
    {
        // 强制重置状态
        g_isInputDisabled = false;
        return false;
    }
}


// 窗口过程函数（处理全屏窗口消息）
LRESULT CALLBACK CServerFun::FullScreenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        // 获取屏幕分辨率
        g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
        g_screenHeight = GetSystemMetrics(SM_CYSCREEN);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 设置深蓝色背景
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 50, 100));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        // 绘制提示文字（显示TCP服务端信息）
        std::wstring text = L"TCP服务端已启动（端口：8080）\n全屏模式运行中\n按下 ESC 键关闭全屏窗口";
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawText(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

        EndPaint(hwnd, &ps);
        break;
    }

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)  // ESC键关闭全屏窗口
        {
            DestroyWindow(hwnd);
            CloseFullScreenWindow();
            g_hFullScreenWnd = NULL;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


// 全屏窗口创建和运行函数（供子线程调用）
void CServerFun::RunFullScreenWindow()
{
    // 1. 注册窗口类：仅首次创建时注册，后续复用（核心修复！）
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = FullScreenWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = g_szClassName;

    if (!g_isWndClassRegistered) // 仅未注册时执行注册
    {
        if (!RegisterClassEx(&wc))
        {
            MessageBox(NULL, L"全屏窗口类注册失败！", L"错误", MB_ICONEXCLAMATION | MB_OK);
            return;
        }
        g_isWndClassRegistered = true; // 注册成功，标记状态
    }

    // 2. 先检查是否已有窗口运行，避免重复创建
    if (g_hFullScreenWnd != NULL && IsWindow(g_hFullScreenWnd))
    {
        MessageBox(NULL, L"全屏窗口已在运行中！", L"提示", MB_ICONINFORMATION | MB_OK);
        return;
    }

    // 3. 创建无边框全屏窗口（原有逻辑不变）
    g_hFullScreenWnd = CreateWindowEx(
        0,
        g_szClassName,
        L"TCP服务端全屏窗口",
        WS_POPUP | WS_VISIBLE,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (g_hFullScreenWnd == NULL)
    {
        MessageBox(NULL, L"全屏窗口创建失败！", L"错误", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // 4. 窗口消息循环（原有逻辑不变，窗口关闭时PostQuitMessage会让GetMessage返回0，退出循环）
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void CServerFun::CloseFullScreenWindow()
{
    // 1. 检查窗口句柄是否有效，无效则直接清理状态返回
    if (g_hFullScreenWnd == NULL || !IsWindow(g_hFullScreenWnd))
    {
        g_hFullScreenWnd = NULL;
        g_isWndClassRegistered = false; // 清理注册标记，避免下次创建判断错误
        return;
    }

    // 2. 异步发送关闭消息，让窗口过程处理WM_DESTROY（优雅关闭）
    PostMessage(g_hFullScreenWnd, WM_CLOSE, 0, 0);

    // 3. 等待窗口销毁完成（最多1秒，处理剩余消息）
    DWORD startTime = GetTickCount();
    while (IsWindow(g_hFullScreenWnd) && (GetTickCount() - startTime) < 1000)
    {
        MSG msg;
        while (PeekMessage(&msg, g_hFullScreenWnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(5); // 缩短休眠，降低CPU占用
    }

    // 4. 强制销毁（异步关闭失败时兜底）
    if (IsWindow(g_hFullScreenWnd))
    {
        DestroyWindow(g_hFullScreenWnd);
    }

    // 5. 注销窗口类（关键！让系统释放类注册信息，支持下次重新注册）
    if (g_isWndClassRegistered)
    {
        UnregisterClass(g_szClassName, GetModuleHandle(NULL));
        g_isWndClassRegistered = false; // 重置注册标记
    }

    // 6. 彻底清理全局句柄
    g_hFullScreenWnd = NULL;

    // 可选：移除弹窗（服务端建议静默运行，避免多次关闭弹框）
    // MessageBox(NULL, L"全屏窗口已成功关闭！", L"提示", MB_ICONINFORMATION | MB_OK);
}
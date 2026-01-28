#include "pch.h"
#include "MfcLogger.h"

CMfcLogger::CMfcLogger()
    : m_logLevel(LogLevel::LOG_DEBUG), m_pEditCtrl(nullptr), m_pParentWnd(nullptr), m_nEditCtrlID(0)
{
}

// 绑定编辑框（传入父窗口指针和编辑框ID）
void CMfcLogger::BindEditCtrl(CWnd* pParentWnd, UINT nID)
{
    if (pParentWnd == nullptr)
        return;

    std::lock_guard<std::mutex> lock(m_mutex); // 加锁保证线程安全
    m_pParentWnd = pParentWnd;
    m_nEditCtrlID = nID;
    m_pEditCtrl = (CEdit*)pParentWnd->GetDlgItem(nID);

    // 设置编辑框为只读、允许多行、自动换行
    if (m_pEditCtrl != nullptr)
    {
        m_pEditCtrl->SetReadOnly(TRUE);
        m_pEditCtrl->SetWindowText(_T("")); // 清空初始内容
    }
}

// 解绑编辑框
void CMfcLogger::UnbindEditCtrl()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pEditCtrl = nullptr;
    m_pParentWnd = nullptr;
    m_nEditCtrlID = 0;
}

// 清空日志编辑框
void CMfcLogger::ClearLog()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_pEditCtrl != nullptr && ::IsWindow(m_pEditCtrl->GetSafeHwnd()))
    {
        m_pEditCtrl->SetWindowText(_T(""));
    }
}

// 调试日志
void CMfcLogger::Debug(const CString& strMsg)
{
    Log(LogLevel::LOG_DEBUG, strMsg);
}

// 普通信息日志
void CMfcLogger::Info(const CString& strMsg)
{
    Log(LogLevel::LOG_INFO, strMsg);
}

// 警告日志
void CMfcLogger::Warning(const CString& strMsg)
{
    Log(LogLevel::LOG_WARNING, strMsg);
}

// 错误日志
void CMfcLogger::Error(const CString& strMsg)
{
    Log(LogLevel::LOG_ERROR, strMsg);
}

// 致命错误日志
void CMfcLogger::Fatal(const CString& strMsg)
{
    Log(LogLevel::LOG_FATAL, strMsg);
}

// 核心日志输出逻辑
void CMfcLogger::Log(LogLevel level, const CString& strMsg)
{
    // 如果当前日志级别低于设置的级别，直接返回
    if (level < m_logLevel)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);

    // 拼接日志内容：时间戳 + 日志级别 + 日志信息
    CString strLog = GetTimeStamp() + _T(" [") + LogLevelToString(level) + _T("] ") + strMsg + _T("\r\n");

    // 如果绑定了编辑框，输出到编辑框
    if (m_pEditCtrl != nullptr && ::IsWindow(m_pEditCtrl->GetSafeHwnd()))
    {
        // 获取原有内容，追加新日志
        CString strOld;
        m_pEditCtrl->GetWindowText(strOld);
        strOld += strLog;

        // 设置新内容并滚动到最后一行
        m_pEditCtrl->SetWindowText(strOld);
        m_pEditCtrl->LineScroll(m_pEditCtrl->GetLineCount());
    }

    // 额外：也可以输出到控制台（可选）
    OutputDebugString(strLog);
}

// 获取格式化的时间戳（格式：YYYY-MM-DD HH:MM:SS）
CString CMfcLogger::GetTimeStamp()
{
    time_t now = time(nullptr);
    tm tmNow;
    localtime_s(&tmNow, &now); // 线程安全的时间转换函数

    CString strTime;
    strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec);

    return strTime;
}

// 日志级别转字符串
CString CMfcLogger::LogLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::LOG_DEBUG:    return _T("DEBUG");
    case LogLevel::LOG_INFO:     return _T("INFO");
    case LogLevel::LOG_WARNING:  return _T("WARNING");
    case LogLevel::LOG_ERROR:    return _T("ERROR");
    case LogLevel::LOG_FATAL:    return _T("FATAL");
    default:                     return _T("UNKNOWN");
    }
}
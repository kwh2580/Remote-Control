#pragma once
#pragma once
#include <afxwin.h>
#include <string>
#include <mutex>
#include <ctime>

// 日志级别枚举
enum class LogLevel
{
    LOG_DEBUG,   // 调试信息
    LOG_INFO,    // 普通信息
    LOG_WARNING, // 警告
    LOG_ERROR,   // 错误
    LOG_FATAL    // 致命错误
};

class CMfcLogger
{
public:
    // 单例模式，保证全局只有一个日志实例
    static CMfcLogger& GetInstance()
    {
        static CMfcLogger instance;
        return instance;
    }

    // 禁止拷贝和赋值
    CMfcLogger(const CMfcLogger&) = delete;
    CMfcLogger& operator=(const CMfcLogger&) = delete;

    // 绑定/解绑编辑框控件
    void BindEditCtrl(CWnd* pParentWnd, UINT nID);
    void UnbindEditCtrl();

    // 设置日志输出级别（低于该级别的日志不输出）
    void SetLogLevel(LogLevel level) { m_logLevel = level; }

    // 各种级别的日志输出接口
    void Debug(const CString& strMsg);
    void Info(const CString& strMsg);
    void Warning(const CString& strMsg);
    void Error(const CString& strMsg);
    void Fatal(const CString& strMsg);

    // 清空编辑框中的日志
    void ClearLog();

private:
    CMfcLogger();
    ~CMfcLogger() = default;

    // 核心日志输出函数
    void Log(LogLevel level, const CString& strMsg);

    // 获取格式化的时间戳
    CString GetTimeStamp();

    // 将日志级别转为字符串
    CString LogLevelToString(LogLevel level);

private:
    std::mutex m_mutex;          // 线程锁，保证多线程下日志输出安全
    LogLevel m_logLevel;         // 当前日志级别
    CEdit* m_pEditCtrl;          // 绑定的编辑框指针
    CWnd* m_pParentWnd;          // 父窗口指针
    UINT m_nEditCtrlID;          // 编辑框ID
};

// 便捷的日志宏定义（简化调用）
#define LOG_DEBUG(msg) CMfcLogger::GetInstance().Debug(msg)
#define LOG_INFO(msg) CMfcLogger::GetInstance().Info(msg)
#define LOG_WARNING(msg) CMfcLogger::GetInstance().Warning(msg)
#define LOG_ERROR(msg) CMfcLogger::GetInstance().Error(msg)
#define LOG_FATAL(msg) CMfcLogger::GetInstance().Fatal(msg)

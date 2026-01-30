
// RemoteControlClientDlg.h: 头文件
//

#pragma once
#include "Client.h"
#include "ClientFun.h"
#include <fstream>
#include "CRemoteScreenDlg.h"
#include <atlimage.h> 
#pragma comment(lib, "gdiplus.lib")

// CRemoteControlClientDlg 对话框
class CRemoteControlClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteControlClientDlg(CWnd* pParent = nullptr);	// 标准构造函数
//析构函数
    ~CRemoteControlClientDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECONTROLCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWinThread* m_pScreenThread; // 屏幕数据线程句柄，用于管理线程
	volatile bool m_bThreadRunning; // 线程退出标志（volatile防止编译器优化，保证多线程可见）
	afx_msg void OnBnClickedButton1();
	CEdit m_EdiLog;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButtonconnect();
	CClient client;
	afx_msg void OnBnClickedButtonfile();
	CTreeCtrl m_treeCtrl;
	afx_msg void OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void Onopenfile();
	afx_msg void Ondowloadfile();
private:
	HANDLE m_hCurrentFile = INVALID_HANDLE_VALUE;
	std::string   m_currentFileName;  // 当前保存的路径（用于日志）
	size_t m_nReceivedBytes = 0;
	std::vector<BYTE> m_cachedJpgImage;
public:
	afx_msg void Ondeletfile();
	// 线程入口函数：静态成员函数（必须），接收对话框this指针作为参数
	static UINT GetScreenDataThread(LPVOID pParam);
	// 原有的获取屏幕数据函数（修改为非循环，由线程入口调用循环）
	void GetScreenData();
	// 线程控制函数：启动/停止线程（可选，封装成按钮响应）
	void StartScreenThread();
	void StopScreenThread();
	void ShowRemoteScreen();
	afx_msg void OnBnClickedButtonremotescreen();
	const std::vector<BYTE>& GetCachedJpgImage() const { return m_cachedJpgImage; }
	void ClearCachedImage() { m_cachedJpgImage.clear(); }
	CRemoteScreenDlg* p_RemoteScreen;
};

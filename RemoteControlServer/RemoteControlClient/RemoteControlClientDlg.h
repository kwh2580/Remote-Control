
// RemoteControlClientDlg.h: 头文件
//

#pragma once
#include "Client.h"
#include "ClientFun.h"
#include <fstream>

// CRemoteControlClientDlg 对话框
class CRemoteControlClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteControlClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

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
	void GetScreenData(LPVOID pParam);
	afx_msg void OnBnClickedButtonremotescreen();
	const std::vector<BYTE>& GetCachedJpgImage() const { return m_cachedJpgImage; }
	void ClearCachedImage() { m_cachedJpgImage.clear(); }
};

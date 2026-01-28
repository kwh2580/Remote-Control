#pragma once
#include "afxdialogex.h"


// CRemoteScreenDlg 对话框

class CRemoteScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoteScreenDlg)

public:
	CRemoteScreenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRemoteScreenDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTE_SCREEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

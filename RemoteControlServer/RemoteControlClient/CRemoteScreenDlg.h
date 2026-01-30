#pragma once
#include "afxdialogex.h"

// 前置声明
class CRemoteControlClientDlg;

// CRemoteScreenDlg 对话框

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
public:
	CStatic RemoteControlScreen;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();
	CRemoteControlClientDlg* p_RemoteControlClientDlg;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	MouseButton m_MouseButton;
    MouseEvent m_MouseEvent;
};

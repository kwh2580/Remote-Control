// CRemoteScreenDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteControlClient.h"
#include "afxdialogex.h"
#include "CRemoteScreenDlg.h"
#include "RemoteControlClientDlg.h"


// CRemoteScreenDlg 对话框

IMPLEMENT_DYNAMIC(CRemoteScreenDlg, CDialogEx)

CRemoteScreenDlg::CRemoteScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTE_SCREEN, pParent)
{
	
	p_RemoteControlClientDlg = (CRemoteControlClientDlg*)pParent;
}

CRemoteScreenDlg::~CRemoteScreenDlg()
{
}

void CRemoteScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATICAA, RemoteControlScreen);
}


BEGIN_MESSAGE_MAP(CRemoteScreenDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteScreenDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRemoteScreenDlg 消息处理程序

void CRemoteScreenDlg::OnDestroy()
{
	CDialogEx::OnDestroy();


}

void CRemoteScreenDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CRemoteScreenDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
	p_RemoteControlClientDlg->StopScreenThread();
}

// CRemoteScreenDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteControlClient.h"
#include "afxdialogex.h"
#include "CRemoteScreenDlg.h"


// CRemoteScreenDlg 对话框

IMPLEMENT_DYNAMIC(CRemoteScreenDlg, CDialogEx)

CRemoteScreenDlg::CRemoteScreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTE_SCREEN, pParent)
{

}

CRemoteScreenDlg::~CRemoteScreenDlg()
{
}

void CRemoteScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoteScreenDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteScreenDlg 消息处理程序

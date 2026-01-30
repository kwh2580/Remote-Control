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
	ON_WM_LBUTTONDBLCLK()
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

 

void CRemoteScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point) // 双击
{

    // ========== 核心新增：对话框相对坐标 → 图片控件相对坐标 ==========
    CRect ctrlRect; // 存储图片控件的「对话框相对位置+尺寸」
    // 关键：GetWindowRect获取屏幕坐标，再转对话框相对坐标，精准拿到控件在对话框的位置
    RemoteControlScreen.GetWindowRect(ctrlRect);
    ScreenToClient(&ctrlRect); // 控件的屏幕坐标 → 对话框相对坐标
    // 控件在对话框内的左上角偏移（x=左边界偏移，y=上边界偏移）
    int ctrlLeft = ctrlRect.left;
    int ctrlTop = ctrlRect.top;
    // 图片控件的实际显示宽高（用于后续比例转换）
    int ctrlW = ctrlRect.Width();
    int ctrlH = ctrlRect.Height();

    // 修正：对话框坐标 - 控件偏移 = 图片控件的相对坐标
    CPoint ctrlPoint;
    ctrlPoint.x = point.x - ctrlLeft;
    ctrlPoint.y = point.y - ctrlTop;

    // ========== 关键校验：确保点击的是图片控件内的区域 ==========
    // 若点击的是对话框空白处，直接返回，避免无效操作
    if (ctrlPoint.x < 0 || ctrlPoint.x >= ctrlW || ctrlPoint.y < 0 || ctrlPoint.y >= ctrlH)
    {
        CDialogEx::OnLButtonDblClk(nFlags, point);
        return;
    }
    // ========== 坐标修正结束，后续逻辑和之前完全一致 ==========

    // 1. 获取服务端屏幕尺寸（客户端已保存）
    int serverW = 1920;
    int serverH = 1080;
    if (serverW <= 0 || serverH <= 0)
    {
        AfxMessageBox(_T("未获取服务端屏幕尺寸！"));
        CDialogEx::OnLButtonDblClk(nFlags, point);
        return;
    }

    // 2. 按比例转换为服务端真实屏幕坐标（用修正后的ctrlPoint）
    int serverX = (int)((double)ctrlPoint.x / ctrlW * serverW);
    int serverY = (int)((double)ctrlPoint.y / ctrlH * serverH);
    // 边界校验：确保坐标在服务端屏幕范围内
    serverX = max(0, min(serverX, serverW - 1));
    serverY = max(0, min(serverY, serverH - 1));

    // ========== 3. 先发移动包，再发双击包（你的原有逻辑，不变） ==========
    CPack pack;
    // 移动包：type=2，传修正后的服务端坐标
    nlohmann::json jsonMove;
    jsonMove["type"] = 2;
    jsonMove["x"] = serverX;
    jsonMove["y"] = serverY;
    std::string strMove = jsonMove.dump();
    pack.setPackID(5);
    pack.setPackData(strMove.data(), strMove.size());
    p_RemoteControlClientDlg->client.sendPack(pack);
    Sleep(100); // 防包乱序

    // 双击包：type=1，左键双击
    pack.clear();
    nlohmann::json jsonDbl;
    jsonDbl["type"] = 1;
    jsonDbl["buttonType"] = MouseButton::LEFT;
    jsonDbl["eventType"] = MouseEvent::DOUBLE_CLICK1;
    std::string strDbl = jsonDbl.dump();
    pack.setPackID(5);
    pack.setPackData(strDbl.data(), strDbl.size());
    p_RemoteControlClientDlg->client.sendPack(pack);

    CDialogEx::OnLButtonDblClk(nFlags, point);


}

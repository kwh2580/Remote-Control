// RemoteControlClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RemoteControlClient.h"
#include "RemoteControlClientDlg.h"
#include "afxdialogex.h"
#include <ws2tcpip.h>
#include "json.hpp"
#include <atlconv.h> // 添加到文件头部

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Pack.h"
#include "MfcLogger.h"
#include "Client.h"
#include "CRemoteScreenDlg.h"


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CAboutDlg::OnTreeDblClick)
END_MESSAGE_MAP()


// CRemoteControlClientDlg 对话框



CRemoteControlClientDlg::CRemoteControlClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECONTROLCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRemoteControlClientDlg::~CRemoteControlClientDlg()
{
	if(p_RemoteScreen != nullptr)
	{
		
		p_RemoteScreen->DestroyWindow(); // 销毁窗口
        delete p_RemoteScreen;

	}
}

void CRemoteControlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_EdiLog);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
}

BEGIN_MESSAGE_MAP(CRemoteControlClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteControlClientDlg::OnBnClickedButton1)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT2, &CRemoteControlClientDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTONConnect, &CRemoteControlClientDlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTONFile, &CRemoteControlClientDlg::OnBnClickedButtonfile)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CRemoteControlClientDlg::OnNMDblclkTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CRemoteControlClientDlg::OnNMClickTree1)

	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CRemoteControlClientDlg::OnNMRClickTree1)
	ON_COMMAND(ID_open_file, &CRemoteControlClientDlg::Onopenfile)
	ON_COMMAND(ID_dowload_file, &CRemoteControlClientDlg::Ondowloadfile)
	ON_COMMAND(ID_delet_file, &CRemoteControlClientDlg::Ondeletfile)
	ON_BN_CLICKED(IDC_BUTTONremoteScreen, &CRemoteControlClientDlg::OnBnClickedButtonremotescreen)
END_MESSAGE_MAP()


// CRemoteControlClientDlg 消息处理程序

BOOL CRemoteControlClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CMfcLogger::GetInstance().BindEditCtrl(this, IDC_EDIT2);
	LOG_INFO(_T("程序启动成功"));
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteControlClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteControlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteControlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRemoteControlClientDlg::OnBnClickedButton1()
{
	ShowRemoteScreen();
	return;
	char* str = "D:\\ChangZhi\\dnplayer2\\AdbWinApi.dll";
	CPack pack(6, str,strlen(str));
	client.sendPack(pack);
	return;
	// 插入根节点
	HTREEITEM hRoot = m_treeCtrl.InsertItem(_T("我的电脑"), TVI_ROOT, TVI_LAST);

	// 保存 C 盘节点的句柄
	HTREEITEM hC_Drive = m_treeCtrl.InsertItem(_T("C:\\"), hRoot);

	// 在 C 盘下插入子文件夹
	m_treeCtrl.InsertItem(_T("Windows"), hC_Drive);
	m_treeCtrl.InsertItem(_T("Program Files"), hC_Drive);
	m_treeCtrl.InsertItem(_T("Users"), hC_Drive);

	// D 盘（无子项）
	m_treeCtrl.InsertItem(_T("D:\\"), hRoot);
	return;



	
}

int CRemoteControlClientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CRemoteControlClientDlg::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRemoteControlClientDlg::OnBnClickedButtonconnect()
{
	if (client.isConnected())
	{
		LOG_INFO(_T("已连接"));
		return;
	}


    // 获取根节点句柄
    HTREEITEM hRoot = m_treeCtrl.GetRootItem();
    // 捕获this和hRoot
    client.bindPackHandler(1, [this, hRoot](const CPack& pack) {

        CString str = CA2W(pack.getData());
        //分割字符
        std::vector<CString> result;

        // 分隔符：逗号（可替换为空格、分号等单字符）
        TCHAR sep = _T(',');
        int nPos = 0;
        // Tokenize(分隔符, 起始位置)：返回分割后的子串，同时更新nPos
        CString token = str.Tokenize(CString(sep), nPos);

        while (!token.IsEmpty())
        {
            result.push_back(token);
            token = str.Tokenize(CString(sep), nPos);
        }
        for (const auto& s : result)
        {
            LOG_INFO(s);
            HTREEITEM hDrive = m_treeCtrl.InsertItem(s + ":", hRoot, TVI_LAST);
        }

        LOG_INFO(str);

    });
    client.bindPackHandler(2, [this, hRoot](const CPack& pack) {
		nlohmann::json packjson;
		std::string json1(pack.getData(), pack.getDataLen());
		packjson = nlohmann::json::parse(json1);
		//获取表示
        bool stop = packjson["stop"];
		CString str;
		//获取字符串
		if (packjson.contains("name") && packjson["name"].is_string())
		{
			std::string utf8Name = packjson["name"].get<std::string>();
			 str = CA2W(utf8Name.c_str(), CP_UTF8);
		}
		//获取文件类型
		bool isDir = packjson["type"];
		if (stop)
        {
            LOG_INFO(_T("已停止"));
            return;
        }
		//获取当前树节点
        HTREEITEM hParent = m_treeCtrl.GetSelectedItem();
		//根据类型插入树节点
        if (isDir)
        {
            HTREEITEM hChild = m_treeCtrl.InsertItem(str, hParent, TVI_LAST);
            m_treeCtrl.InsertItem(_T(""), hChild, TVI_LAST);
            //m_treeCtrl.SetItemImage(hChild, 1, 1);
        }
        else
        {
            HTREEITEM hChild = m_treeCtrl.InsertItem(str, hParent, TVI_LAST);
        }


		CString str1 = CA2W(pack.getData());
        LOG_INFO(str1);

    });
    client.bindPackHandler(3, [this, hRoot](const CPack& pack) {
        LOG_INFO(_T("3"));

    });
	client.bindPackHandler(4, [this](const CPack& pack) {
		//LOG_INFO(_T("Received pack ID=4"));

		std::string data = pack.getData();
		LOG_INFO(CString(_T("Received data size: ")) + CString(std::to_string(pack.getDataLen()).c_str()));
		if (data == "filebegin") {
			LOG_INFO(_T("开始接收文件"));

			CHAR exePath[MAX_PATH];
			GetModuleFileNameA(NULL, exePath, MAX_PATH);
			PathRemoveFileSpecA(exePath);
			//获取当前树选择
            HTREEITEM hParent = m_treeCtrl.GetSelectedItem();
			//获取当前节点名称
            CString str = m_treeCtrl.GetItemText(hParent); 
			//转化为string
			USES_CONVERSION;
			std::string pathstr = CW2A(str, CP_ACP); // 或 CP_UTF8 视你的编码需求

			std::string savePath = std::string(exePath) + "\\" + pathstr;
			//追加字符串
       
	


			// （可选）弹窗覆盖提示，此处省略以简化

			m_hCurrentFile = CreateFileA(
				savePath.c_str(),
				GENERIC_WRITE,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);

			if (m_hCurrentFile == INVALID_HANDLE_VALUE) {
				LOG_ERROR(_T("无法创建文件"));
				return;
			}

			m_currentFileName = savePath;
			m_nReceivedBytes = 0; // 👈 重置计数器！
			LOG_INFO(_T("开始写入文件"));
			return;
		}

		if (data == "File send completed") {
			if (m_hCurrentFile != INVALID_HANDLE_VALUE) {
				CloseHandle(m_hCurrentFile);
				m_hCurrentFile = INVALID_HANDLE_VALUE;
			}

			// 👇 打印总接收大小
			CString msg;
			msg.Format(_T("✅ 文件接收完成！\n保存路径: %hs\n传输大小: %I64u 字节"),
				m_currentFileName.c_str(),
				static_cast<ULONGLONG>(m_nReceivedBytes));
			LOG_INFO(msg);
			return;
		}
		

		if (m_hCurrentFile != INVALID_HANDLE_VALUE) {
			DWORD bytesWritten = 0;
			BOOL success = WriteFile(
				m_hCurrentFile,
				pack.getData(),
				pack.getDataLen(),
				&bytesWritten,
				NULL
			);

			if (success && bytesWritten == pack.getDataLen()) {
				m_nReceivedBytes += pack.getDataLen();
			}
			else {
				LOG_ERROR(_T("写入文件失败！"));
				if (m_hCurrentFile != INVALID_HANDLE_VALUE) {
					CloseHandle(m_hCurrentFile);
					m_hCurrentFile = INVALID_HANDLE_VALUE;
				}
			}
		}
		});
    
	client.bindPackHandler(6, [this](const CPack& pack) {

		LOG_INFO(_T("Received pack ID=6"));

		const char* pData = pack.getData();
		size_t dataLen = pack.getDataLen();

		if (pData == nullptr || dataLen == 0) {
			LOG_ERROR(_T("图像数据为空"));
			return;
		}
		m_cachedJpgImage.assign(pData, pData + dataLen);
		//显示图片
		
        ShowRemoteScreen();

	
	

	});
    
    
	if (!client.connect("127.0.0.1", 8080)) {
		LOG_INFO(_T("连接失败"));
        return ;
    }

    if (!client.startAsyncReceive(1024 * 1024)) {
		LOG_INFO(_T("接收消息失败"));
        return;
    }
	LOG_INFO(_T("连接成功"));
}

void CRemoteControlClientDlg::OnBnClickedButtonfile()
{
	CPack pack(1, "PPPPPPPP", 8);
	client.sendPack(pack);
}

void CAboutDlg::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBoxA(NULL,"点击了树节点2",NULL,NULL);
	//messageBox("点击了树节点");
	*pResult = 0;
}

//获取文件路径
CString GetFilePath()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		return dlg.GetPathName();
	}
	return _T("");
}
std::vector<CString> GetFullPathFromTreeItem(CTreeCtrl& tree, HTREEITEM hItem)
{
	std::vector<CString> path;

	// 从当前节点向上遍历到根
	HTREEITEM hCurrent = hItem;
	while (hCurrent != NULL)
	{
		CString text = tree.GetItemText(hCurrent);
		path.push_back(text);
		hCurrent = tree.GetParentItem(hCurrent); // 获取父节点
	}

	// 此时 path 是 [当前节点, 父, 祖父, ..., 根]
	// 我们通常需要从根到当前，所以反转
	std::reverse(path.begin(), path.end());

	return path;
}

void CRemoteControlClientDlg::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
{


	// 窗口有效性检查
	HTREEITEM hSel = m_treeCtrl.GetSelectedItem();
	if (hSel)
	{
		auto fullPath = GetFullPathFromTreeItem(m_treeCtrl, hSel);

		// 打印完整路径
		CString fullStr;
		for (size_t i = 0; i < fullPath.size(); ++i)
		{
			if (i > 0) fullStr += _T("\\");
			fullStr += fullPath[i];
		}
		fullStr += "\\";
        CT2A pszAnsi(fullStr);
		CPack pack(2, pszAnsi, strlen(pszAnsi));
		client.sendPack(pack);
		AfxMessageBox(_T("完整路径：\n") + fullStr);
	}
	
}



void CRemoteControlClientDlg::OnNMClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	return;
	// 窗口有效性检查
	if (!::IsWindow(m_treeCtrl.m_hWnd))
	{
		*pResult = 0;
		return;
	}

	// 用鼠标位置 HitTest 获取项（不要 reinterpret_cast 为 NMTREEVIEW）
	CPoint pt;
	::GetCursorPos(&pt);
	m_treeCtrl.ScreenToClient(&pt);

	UINT uFlags = 0;
	HTREEITEM hItem = m_treeCtrl.HitTest(pt, &uFlags);

	// 若 HitTest 返回有效项，则读取文本；uFlags 可用于进一步判断是否在标签/图标等
	if (hItem != NULL)
	{
		CString strNodeText = m_treeCtrl.GetItemText(hItem);
		AfxMessageBox(strNodeText);
	}

	*pResult = 0;
}


void CRemoteControlClientDlg::OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CPoint ptMouse;
	GetCursorPos(&ptMouse);

	CMenu menu; menu.LoadMenu(IDR_MENURCLCK);
	CMenu* pPopup = menu.GetSubMenu(0); 
	if (pPopup != NULL) 
	{ 
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this);
	}

}

// 打开文件
void CRemoteControlClientDlg::Onopenfile()
{
	HTREEITEM hSel = m_treeCtrl.GetSelectedItem();
	auto fullPath = GetFullPathFromTreeItem(m_treeCtrl, hSel);
	CString fullStr;
	for (size_t i = 0; i < fullPath.size(); ++i)
	{
		if (i > 0) fullStr += _T("\\");
		fullStr += fullPath[i];
	}
	CT2A pszAnsi(fullStr);
	CPack pack(3, pszAnsi, strlen(pszAnsi));
	client.sendPack(pack);
	AfxMessageBox(_T("完整路径：\n") + fullStr);
	// TODO: 在此添加命令处理程序代码
}

// 下载文件

void CRemoteControlClientDlg::Ondowloadfile()
{
	HTREEITEM hSel = m_treeCtrl.GetSelectedItem();
	if (hSel)
	{
		auto fullPath = GetFullPathFromTreeItem(m_treeCtrl, hSel);

		// 打印完整路径
		CString fullStr;
		for (size_t i = 0; i < fullPath.size(); ++i)
		{
			if (i > 0) fullStr += _T("\\");
			fullStr += fullPath[i];
		}
		CT2A pszAnsi(fullStr);
		CPack pack(4, pszAnsi, strlen(pszAnsi));
		client.sendPack(pack);
		AfxMessageBox(_T("完整路径：\n") + fullStr);
	}


}

// 删除文件
void CRemoteControlClientDlg::Ondeletfile()
{
	// TODO: 在此添加命令处理程序代码
}
//定时发送数据获取屏幕数据
UINT CRemoteControlClientDlg::GetScreenDataThread(LPVOID pParam)
{
	// 将传入的this指针转为对话框对象指针
	CRemoteControlClientDlg* pDlg = (CRemoteControlClientDlg*)pParam;
	if (pDlg == nullptr)
	{
		AfxMessageBox(_T("线程初始化失败：对话框指针为空！"));
		return 1; // 线程异常退出，返回非0
	}

	// 设置线程退出标志为true，开始循环
	pDlg->m_bThreadRunning = true;
	while (pDlg->m_bThreadRunning) // 由退出标志控制循环
	{
		// 调用业务函数：单次发送屏幕请求
		pDlg->GetScreenData();
		// 休眠20ms，控制循环频率（原逻辑的Sleep）
		Sleep(100);
	}

	// 循环结束，线程正常退出，返回0
	pDlg->m_pScreenThread = nullptr; // 清空线程句柄
	pDlg->m_bThreadRunning = false;  // 重置退出标志
	return 0;
}
void CRemoteControlClientDlg::GetScreenData()
{
    CPack pack(6, "PPPPPPPP", 8);
    client.sendPack(pack);
}


// 启动屏幕数据线程
void CRemoteControlClientDlg::StartScreenThread()
{
	// 防止重复启动线程：线程句柄非空 或 线程正在运行，直接返回
	if (m_pScreenThread != nullptr || m_bThreadRunning)
	{
		AfxMessageBox(_T("屏幕数据线程已在运行！"));
		return;
	}

	// 创建并启动MFC工作线程（CREATE_SUSPENDED：先创建挂起，再启动，更安全）
	m_pScreenThread = AfxBeginThread(
		GetScreenDataThread,  // 线程入口函数（静态成员函数）
		this,                 // 传递给线程的参数（对话框this指针）
		THREAD_PRIORITY_NORMAL, // 线程优先级（默认正常即可）
		0,                    // 线程栈大小（0表示使用系统默认）
		CREATE_SUSPENDED      // 挂起创建，后续手动ResumeThread启动
	);

	if (m_pScreenThread == nullptr)
	{
		AfxMessageBox(_T("创建屏幕数据线程失败！"));
		return;
	}

	// 设置线程为“后台线程”：主程序退出时，后台线程会自动退出（避免阻塞程序退出）
	m_pScreenThread->m_bAutoDelete = TRUE; // MFC默认TRUE，可显式设置
	// 启动挂起的线程
	m_pScreenThread->ResumeThread();
	AfxMessageBox(_T("屏幕数据线程启动成功！"));
}


// 安全停止屏幕数据线程
void CRemoteControlClientDlg::StopScreenThread()
{
	// 线程未运行，直接返回
	if (!m_bThreadRunning || m_pScreenThread == nullptr)
	{
		AfxMessageBox(_T("屏幕数据线程未运行！"));
		return;
	}

	// 第一步：设置退出标志，让线程的循环自然结束（核心：安全退出）
	m_bThreadRunning = false;
	// 第二步：等待线程退出（最多等待1秒，避免无限阻塞）
	DWORD dwRet = WaitForSingleObject(m_pScreenThread->m_hThread, 1000);
	switch (dwRet)
	{
	case WAIT_OBJECT_0:
		// 线程正常退出
		AfxMessageBox(_T("屏幕数据线程已正常停止！"));
		break;
	case WAIT_TIMEOUT:
		// 线程超时未退出，强制终止（万不得已才用，尽量避免）
		TerminateThread(m_pScreenThread->m_hThread, -1);
		AfxMessageBox(_T("屏幕数据线程超时，已强制终止！"));
		break;
	default:
		// 等待失败
		AfxMessageBox(_T("停止屏幕数据线程失败！"));
		break;
	}

	// 清空线程句柄和退出标志
	m_pScreenThread = nullptr;
	m_bThreadRunning = false;
}

//显示远程屏幕
void CRemoteControlClientDlg::ShowRemoteScreen()
{
	// 1. 空数据/控件空判断：双重校验，避免崩溃
	if (m_cachedJpgImage.empty() || p_RemoteScreen == nullptr || !p_RemoteScreen->RemoteControlScreen.IsWindowVisible())
	{
		return;
	}

	CImage img;
	HGLOBAL hGlobal = NULL;
	IStream* pStream = NULL;
	HBITMAP hOldBmp = NULL;
	// 新增：保存控件DC，用于自适应绘制
	CDC* pCtrlDC = nullptr;
	HDC hMemDC = NULL;
	HBITMAP hMemBmp = NULL, hOldMemBmp = NULL;

	// 统一资源释放：无论是否成功，都释放所有申请的资源
	auto ReleaseAll = [&]() {
		if (hOldMemBmp) SelectObject(hMemDC, hOldMemBmp);
		if (hMemBmp) DeleteObject(hMemBmp);
		if (hMemDC) DeleteDC(hMemDC);
		if (pCtrlDC) p_RemoteScreen->RemoteControlScreen.ReleaseDC(pCtrlDC);
		if (pStream) pStream->Release();
		if (hOldBmp) DeleteObject(hOldBmp);
		img.Destroy(); // 主动销毁CImage，避免内存泄漏
		};

	try
	{
		// 2. 分配全局内存并拷贝JPG数据（原逻辑不变，加错误提示）
		hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, m_cachedJpgImage.size());
		if (hGlobal == NULL)
		{
			AfxMessageBox(_T("分配全局内存失败！错误码："));
			ReleaseAll();
			return;
		}

		LPVOID pData = GlobalLock(hGlobal);
		memcpy(pData, m_cachedJpgImage.data(), m_cachedJpgImage.size());
		GlobalUnlock(hGlobal);

		// 3. 创建内存流（原逻辑不变，加错误提示）
		HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
		if (hr != S_OK)
		{
			AfxMessageBox(_T("创建IStream流失败！HRESULT："));
			ReleaseAll();
			return;
		}

		// 4. 加载JPG到CImage（关键修复：加GDI+依赖、校验加载结果）
		HRESULT loadHr = img.Load(pStream);
		if (loadHr != S_OK)
		{
			//AfxMessageBox(_T("CImage解析JPG失败！\n错误码)));
			ReleaseAll();
			return;
		}

		// 5. 核心修复：获取控件客户区，创建适配的内存DC绘制（替代直接SetBitmap）
		CRect ctrlRect;
		p_RemoteScreen->RemoteControlScreen.GetClientRect(&ctrlRect); // 控件有效绘制区域
		if (ctrlRect == nullptr)
		{
			AfxMessageBox(_T("图片控件区域为空！"));
			ReleaseAll();
			return;
		}

		// 获取控件DC，用于最终绘制
		pCtrlDC = p_RemoteScreen->RemoteControlScreen.GetDC();
		// 创建内存DC，用于离线绘制（避免闪屏）
		hMemDC = CreateCompatibleDC(pCtrlDC->m_hDC);
		// 创建与控件同尺寸的位图，作为绘制画布
		hMemBmp = CreateCompatibleBitmap(pCtrlDC->m_hDC, ctrlRect.Width(), ctrlRect.Height());
		hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

		// 清空白布（避免残留旧图）
		RECT memRect = { 0,0,ctrlRect.Width(),ctrlRect.Height() };
		FillRect(hMemDC, &memRect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		// 自适应缩放绘制图片（保持宽高比，无拉伸）
		int imgW = img.GetWidth();
		int imgH = img.GetHeight();
		float scaleX = (float)ctrlRect.Width() / imgW;
		float scaleY = (float)ctrlRect.Height() / imgH;
		float scale = min(scaleX, scaleY); // 取最小缩放比，保证图片完整
		int drawW = (int)(imgW * scale);
		int drawH = (int)(imgH * scale);
		int drawX = (ctrlRect.Width() - drawW) / 2;  // 水平居中
		int drawY = (ctrlRect.Height() - drawH) / 2; // 垂直居中

		// 将CImage绘制到内存DC的画布上
		img.Draw(hMemDC, drawX, drawY, drawW, drawH, 0, 0, imgW, imgH);

		// 6. 将内存DC的画布绘制到控件DC，完成显示（核心替代SetBitmap）
		BitBlt(pCtrlDC->m_hDC, 0, 0, ctrlRect.Width(), ctrlRect.Height(),
			hMemDC, 0, 0, SRCCOPY);

		// 7. 刷新控件，立即显示
		p_RemoteScreen->RemoteControlScreen.Invalidate(FALSE); // FALSE：不擦除背景，避免闪屏
		p_RemoteScreen->RemoteControlScreen.UpdateWindow();
	}
	catch (CException* e)
	{
		CString strErr;
		e->GetErrorMessage(strErr.GetBuffer(256), 256);
		AfxMessageBox(_T("显示远程屏幕失败：") + strErr);
		e->Delete();
		ReleaseAll();
	}
	catch (...)
	{
		AfxMessageBox(_T("显示远程屏幕时发生未知错误！"));
		ReleaseAll();
	}

	// 最终资源释放兜底
	ReleaseAll();
}

void CRemoteControlClientDlg::OnBnClickedButtonremotescreen()
{
	//启动线程
	StartScreenThread();
	if (p_RemoteScreen == nullptr || !(p_RemoteScreen->IsWindowVisible()))
	{
		p_RemoteScreen = new CRemoteScreenDlg(this);
		p_RemoteScreen->Create(IDD_REMOTE_SCREEN,this);
        p_RemoteScreen->ShowWindow(SW_SHOW);	
	}

	

	//创建对话框
	//RemoteScreen dlg;
 //   dlg.DoModal();
	//创建线程
   // CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetScreenData, NULL, 0, NULL);
}

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
		packjson = nlohmann::json::parse(pack.getData());
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

		LOG_INFO(_T("图像缓存已刷新，大小: %zu 字节"), dataLen);

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
void CRemoteControlClientDlg::GetScreenData(LPVOID pParam)
{
	while (true)
	{
		// 创建一个CPack对象，并设置pack_id为1，data为"PPPPPPPP"，data_len为8
		CPack pack(6, "hello", 5);
		// 调用client对象的sendPack方法，将pack对象发送给服务器
		client.sendPack(pack);
		// 休眠1000毫秒
		Sleep(20);
	}
}

//显示远程屏幕
void CRemoteControlClientDlg::ShowRemoteScreen()
{
	// 1. 空数据判断：未接收到JPG数据时直接返回，避免崩溃
	if (m_cachedJpgImage.empty())
	{
		return;
	}

	// 2. 定义CImage对象，用于解析JPG二进制数据
	CImage img;
	HGLOBAL hGlobal = NULL;  // 全局内存句柄，用于创建IStream流
	IStream* pStream = NULL; // 流对象，CImage从流中加载JPG
	HBITMAP hOldBmp = NULL;  // 控件原有位图句柄，用于释放

	try
	{
		// 3. 将std::vector<BYTE>的JPG数据转为IStream流（CImage要求的加载方式）
		// 分配全局内存，大小为JPG数据长度
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, m_cachedJpgImage.size());
		if (hGlobal == NULL)
		{
			AfxMessageBox(_T("分配内存失败！"));
			return;
		}
		// 将vector中的JPG数据拷贝到全局内存
		LPVOID pData = GlobalLock(hGlobal);
		memcpy(pData, m_cachedJpgImage.data(), m_cachedJpgImage.size());
		GlobalUnlock(hGlobal);

		// 从全局内存创建IStream流
		if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) != S_OK)
		{
			AfxMessageBox(_T("创建流对象失败！"));
			GlobalFree(hGlobal); // 释放已分配的内存
			return;
		}

		// 4. 从IStream流加载JPG数据到CImage（自动解析JPG格式）
		if (img.Load(pStream) != S_OK)
		{
			AfxMessageBox(_T("解析JPG图片失败！（数据可能损坏/非JPG格式）"));
			return;
		}

		// 5. 将CImage中的位图设置到图片控件CStatic
		// 先获取控件原有位图句柄，避免内存泄漏
		hOldBmp = m_EdiLog.GetBitmap();
		// 设置新位图到控件（CStatic的SetBitmap会返回原有句柄）
		m_EdiLog.SetBitmap(img.Detach()); // Detach：将CImage的HBITMAP分离出来，交给控件管理

		// 6. 释放控件原有位图（关键：避免内存泄漏）
		if (hOldBmp != NULL)
		{
			DeleteObject(hOldBmp);
			hOldBmp = NULL;
		}

		// 7. 刷新控件，让图片立即显示（避免卡顿/不刷新）
		m_EdiLog.Invalidate();    // 标记控件为需要重绘
		m_EdiLog.UpdateWindow(); // 立即执行重绘，显示新图片
	}
	catch (CException* e)
	{
		// 异常捕获：打印错误信息
		CString strErr;
		e->GetErrorMessage(strErr.GetBuffer(256), 256);
		AfxMessageBox(_T("显示远程屏幕失败：") + strErr);
		e->Delete();
	}
	catch (...)
	{
		// 捕获未知异常，防止程序崩溃
		AfxMessageBox(_T("显示远程屏幕时发生未知错误！"));
	}

	// 8. 释放资源（无论是否成功，都要释放）
	if (pStream != NULL)
	{
		pStream->Release(); // 释放IStream流
	}
	// 注意：hGlobal不需要手动释放，CreateStreamOnHGlobal第二个参数为TRUE时，流释放会自动释放hGlobal
	if (hOldBmp != NULL)
	{
		DeleteObject(hOldBmp); // 二次释放保护
	}
}


void CRemoteControlClientDlg::OnBnClickedButtonremotescreen()
{
	//创建对话框
	//RemoteScreen dlg;
 //   dlg.DoModal();
	//创建线程
   // CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetScreenData, NULL, 0, NULL);
}

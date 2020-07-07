
// crawler-clientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "crawler-client.h"
#include "crawler-clientDlg.h"
#include "afxdialogex.h"
#include "logdlg.h"
#include "config.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CcrawlerclientDlg 对话框



CcrawlerclientDlg::CcrawlerclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CRAWLERCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcrawlerclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, logList);
	DDX_Control(pDX, IDC_BUTTON1, refreshButton);
}

BEGIN_MESSAGE_MAP(CcrawlerclientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_NOTIFYMENU, &CcrawlerclientDlg::OnNotifyMenu)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_Exit, &CcrawlerclientDlg::OnExit)
	ON_COMMAND(ID_Start, &CcrawlerclientDlg::OnStart)
	ON_COMMAND(ID_Log, &CcrawlerclientDlg::OnLog)
	ON_COMMAND(ID_Stop, &CcrawlerclientDlg::OnStop)
	ON_BN_CLICKED(IDC_BUTTON1, &CcrawlerclientDlg::OnClickedRefreshButton)
	ON_WM_TIMER()
	ON_COMMAND(ID_Config, &CcrawlerclientDlg::OnConfig)
END_MESSAGE_MAP()


// CcrawlerclientDlg 消息处理程序

BOOL CcrawlerclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// log回调函数
	//theApp.log->Callback = Callback_OnSelchangeDatecombo;


	// 任务栏图标
	this->nid.cbSize = sizeof(this->nid);
	this->nid.hWnd = AfxGetMainWnd()->m_hWnd;
	this->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	this->nid.uCallbackMessage = WM_NOTIFYMENU;
	this->nid.hIcon = theApp.LoadIconW(IDR_MAINFRAME);
	this->nid.uID = IDR_MAINFRAME;
	StringCchCopy(this->nid.szTip, ARRAYSIZE(this->nid.szTip), _T("client"));
	Shell_NotifyIcon(NIM_ADD, &this->nid);

	this->menu.LoadMenuW(IDR_MENU1);
	this->pSubmenu = this->menu.GetSubMenu(0);


	logList.InsertColumn(0, _T("QQ"), LVCFMT_LEFT, 80, 0);
	logList.InsertColumn(1, _T("进程"), LVCFMT_LEFT, 60, 0);
	logList.InsertColumn(2, _T("状态"), LVCFMT_LEFT, 55, 0);

	_injection = new injection();

	this->OnStart();

	SetTimer(1, 1000 * 10, NULL);

	config dlg;
	dlg.DoModal();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CcrawlerclientDlg::OnPaint()
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
HCURSOR CcrawlerclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CcrawlerclientDlg::OnNotifyMenu(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDR_MAINFRAME)
		return -1;
	switch (lParam)
	{
	case WM_LBUTTONUP:
		this->ShowWindow(SW_SHOW);
		break;
	case WM_RBUTTONUP:
		if (_injection->IsStarted())
		{
			this->pSubmenu->ModifyMenuW(ID_Status, MF_BYCOMMAND | MF_STRING | MF_DISABLED, ID_Status, L"状态：开启");
			this->pSubmenu->ModifyMenuW(ID_Start, MF_DISABLED, ID_Start, L"开启");
			this->pSubmenu->ModifyMenuW(ID_Stop, MF_ENABLED, ID_Stop, L"停止");
		}
		else
		{
			this->pSubmenu->ModifyMenuW(ID_Status, MF_BYCOMMAND | MF_STRING | MF_DISABLED, ID_Status, L"状态：停止");
			this->pSubmenu->ModifyMenuW(ID_Start, MF_ENABLED, ID_Start, L"开启");
			this->pSubmenu->ModifyMenuW(ID_Stop, MF_DISABLED, ID_Stop, L"停止");
		}
		this->SetForegroundWindow();
		GetCursorPos(&this->point);
		this->pSubmenu->TrackPopupMenu(TPM_LEFTALIGN, this->point.x, this->point.y, AfxGetMainWnd());
		break;
	default:
		break;
	}
	return 0;
}


void CcrawlerclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((nID & 0x0000FFF0) == SC_MINIMIZE)
		AfxGetMainWnd()->ShowWindow(SW_HIDE);
	else
		CDialogEx::OnSysCommand(nID, lParam);
}


void CcrawlerclientDlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	this->OnOK();
}


void CcrawlerclientDlg::OnStart()
{
	// TODO: 在此添加命令处理程序代码
	if (_injection->IsStarted())
	{
		troycrawler::log::error(WS2U8(L"启动失败，监控已启动。"));
	}
	else
	{
		_injection->start();
		troycrawler::log::info(WS2U8(L"监控启动。"));
	}
}



void CcrawlerclientDlg::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	if (_injection->IsStarted())
	{
		_injection->stop();
		troycrawler::log::info(WS2U8(L"监控停止。"));
	}
	else
	{
		troycrawler::log::error(WS2U8(L"停止失败，监控未启动。"));
	}
}

void CcrawlerclientDlg::OnLog()
{
	logdlg dlg;
	dlg.DoModal();
}

void CcrawlerclientDlg::OnClickedRefreshButton()
{
	// TODO: 在此添加控件通知处理程序代码
	logList.DeleteAllItems();

	std::map<int, std::string> mPid = troycrawler::config::get("pid");
	for (std::map<int, std::string>::iterator it = mPid.begin(); it != mPid.end(); ++it)
	{
		std::string sPid = it->second;
		CString module = U82WS(sPid.substr(0, sPid.find('/'))).c_str();
		CString pid = U82WS(sPid.substr(sPid.find('/')+1)).c_str();
		logList.InsertItem(0, module);
		logList.SetItemText(0, 1, pid);
		if (pid.Compare(L"0"))
			logList.SetItemText(0, 2, L"监控中");
		else
			logList.SetItemText(0, 2, L"停止");
	}
	return;
}


void CcrawlerclientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case 1:
		this->OnClickedRefreshButton();
	default:
		break;
	}
}


void CcrawlerclientDlg::OnConfig()
{
	// TODO: 在此添加命令处理程序代码
	config dlg;
	dlg.DoModal();
}

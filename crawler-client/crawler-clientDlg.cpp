
// crawler-clientDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "crawler-client.h"
#include "crawler-clientDlg.h"
#include "afxdialogex.h"

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
	ON_COMMAND(ID_Log, &CcrawlerclientDlg::OnLog)
	ON_COMMAND(ID_Start, &CcrawlerclientDlg::OnStart)
	ON_COMMAND(ID_Stop, &CcrawlerclientDlg::OnStop)
	ON_CBN_DROPDOWN(IDC_ModuleCombo, &CcrawlerclientDlg::OnDropdownModulecombo)
	ON_CBN_DROPDOWN(IDC_DateCombo, &CcrawlerclientDlg::OnDropdownDatecombo)
	ON_CBN_SELCHANGE(IDC_DateCombo, &CcrawlerclientDlg::OnSelchangeDatecombo)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CcrawlerclientDlg::OnLvnGetdispinfoList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CcrawlerclientDlg::OnClickedRefreshButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void Callback_OnSelchangeDatecombo()
{
	CcrawlerclientDlg* dlg = (CcrawlerclientDlg*)AfxGetApp()->m_pMainWnd;
	dlg->OnSelchangeDatecombo();
}

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


	//// combo初始化
	//moduleCombo.AddString(L"主程序");
	//moduleCombo.SetCurSel(0);
	//for (const auto & entry : fs::directory_iterator(theApp.pLogpath))
	//{
	//	if (entry.status().type() == fs::file_type::regular)
	//	{
	//		dateCombo.InsertString(0, entry.path().filename().wstring().c_str());
	//	}
	//}
	//dateCombo.SetCurSel(0);

	//// listctrl初始化
	//logList.InsertColumn(0, _T("时间"), LVCFMT_LEFT, 150, 0);
	//logList.InsertColumn(1, _T("级别"), LVCFMT_LEFT, 100, 1);
	//logList.InsertColumn(2, _T("内容"), LVCFMT_LEFT, 1024, 2);

	logList.InsertColumn(0, _T("QQ"), LVCFMT_LEFT, 80, 0);
	logList.InsertColumn(1, _T("进程"), LVCFMT_LEFT, 60, 0);
	logList.InsertColumn(2, _T("状态"), LVCFMT_LEFT, 55, 0);
	//this->OnSelchangeDatecombo();
	/*logList.InsertItem(0, L"297307007");
	logList.SetItemText(0,1, L"65535");
	logList.SetItemText(0,2, L"监控");*/

	this->OnStart();

	SetTimer(1, 1000 * 10, NULL);

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
		if (theApp.hInjectModule)
		{
			this->pSubmenu->ModifyMenuW(ID_Status, MF_BYCOMMAND | MF_STRING, ID_Status, L"状态：开启");
		}
		else
		{
			this->pSubmenu->ModifyMenuW(ID_Status, MF_BYCOMMAND | MF_STRING, ID_Status, L"状态：关闭");
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


void CcrawlerclientDlg::OnLog()
{
	// TODO: 在此添加命令处理程序代码
	this->ShowWindow(SW_SHOW);
}


int update()
{
	std::string ret = theApp.http->Post("http://114.67.180.124/anonymousApi", "{\"type\":\"0\"}");
	if (ret == "")return 0;
	json resp = json::parse(ret.c_str());
	if (resp["error"] == "0")
	{
		json data = resp["data"];
		for (json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string filename = (*it)["filename"].get<std::string>();
			std::string url = (*it)["url"].get<std::string>();
			std::string md5 = (*it)["md5"].get<std::string>();


			if (md5 != Getmd5(filename))
			{
				theApp.log->doLog("[debug]", Getmd5(filename).c_str());
				if (theApp.http->Download(url, filename)) {
					continue;
				}
				return 0;
			}
		}
		return 1;
	}
	return 0;
}


void CcrawlerclientDlg::OnStart()
{
	// TODO: 在此添加命令处理程序代码
	if (update())
	{
		theApp.log->doLog("[INFO]", u8"更新成功");

		if (theApp.hInjectModule)
		{
			theApp.log->doLog("[INFO]", u8"监控已启动");
		}
		else
		{
			theApp.hInjectModule = LoadLibrary(L"inject.dll");
			if (theApp.hInjectModule)
			{
				ResetEvent(theApp.hInjectStop);
				((int(*)())GetProcAddress(theApp.hInjectModule, "start"))();

				theApp.log->doLog("[INFO]", u8"监控启动");
			}
			else
			{
				theApp.log->doLog("[ERROR]", u8"监控启动失败");
			}
		}
	}
	else
	{
		theApp.log->doLog("[ERROR]", u8"更新失败");
	}
}



void CcrawlerclientDlg::OnStop()
{
	// TODO: 在此添加命令处理程序代码
	if (theApp.hInjectModule)
	{
		SetEvent(theApp.hInjectStop);
		((int(*)())GetProcAddress(theApp.hInjectModule, "stop"))();
		BOOL bRes = FreeLibrary(theApp.hInjectModule);
		if (bRes)
		{
			theApp.log->doLog("[INFO]", u8"监控停止");
			theApp.hInjectModule = NULL;
		}
		else theApp.log->doLog("[ERROR]", u8"监控停止错误");
	}
}



void CcrawlerclientDlg::OnDropdownModulecombo()
{
	// TODO: 在此添加控件通知处理程序代码
	moduleCombo.ResetContent();
	moduleCombo.AddString(L"主程序");
	for (const auto & entry : fs::directory_iterator(theApp.pLogpath))
	{
		if (entry.status().type() == fs::file_type::directory)
		{
			moduleCombo.InsertString(1, entry.path().filename().wstring().c_str());
		}
	}
}


void CcrawlerclientDlg::OnDropdownDatecombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString module;
	moduleCombo.GetLBText(moduleCombo.GetCurSel(), module);
	dateCombo.ResetContent();
	if (module.Compare(CString(L"主程序")) == 0)
	{
		for (const auto & entry : fs::directory_iterator(theApp.pLogpath))
		{
			if (entry.status().type() == fs::file_type::regular)
			{
				dateCombo.InsertString(0, entry.path().filename().wstring().c_str());
			}
		}
	}
	else
	{
		for (const auto & entry : fs::directory_iterator(theApp.pLogpath / std::wstring(module)))
		{
			if (entry.status().type() == fs::file_type::regular)
			{
				dateCombo.InsertString(0, entry.path().filename().wstring().c_str());
			}
		}
	}
}


void CcrawlerclientDlg::OnSelchangeDatecombo()
{
	// TODO: 在此添加控件通知处理程序代码

	CString csModule, csDate;
	moduleCombo.GetLBText(moduleCombo.GetCurSel(), csModule);
	dateCombo.GetLBText(dateCombo.GetCurSel(), csDate);

	std::wstring wsModule(csModule);
	std::wstring wsDate(csDate);

	std::filesystem::path pLogFilename = theApp.pLogpath;
	if (wsModule != L"主程序")
	{
		pLogFilename /= wsModule;
	}
	pLogFilename /= wsDate;

	if (theApp.cr)delete theApp.cr;
	theApp.cr = new csv::Reader;
	theApp.cr->configure_dialect("my fancy dialect")
		.header(false)
		.skip_empty_rows(true)
		.column_names("time", "level", "content")
		.quote_character('"')
		.delimiter(",")
		.double_quote(true)
		.trim_characters(' ', '\t', '"');

	theApp.cr->read(WS2U8(pLogFilename.c_str()));
	rows = theApp.cr->rows();
	logList.SetItemCount(rows.size());
	logList.EnsureVisible(rows.size() - 1, false);
}


void CcrawlerclientDlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	LV_ITEM* pItem = &(pDispInfo)->item;

	int iItemIndx = pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //字符串缓冲区有效
	{
		switch (pItem->iSubItem) {
		case 0: //填充数据项的名字
			lstrcpyW(pItem->pszText, U82WS(rows[iItemIndx]["time"].c_str()).c_str());
			break;
		case 1: //填充子项1
			lstrcpyW(pItem->pszText, U82WS(rows[iItemIndx]["level"].c_str()).c_str());
			break;
		case 2: //填充子项2
			lstrcpyW(pItem->pszText, U82WS(rows[iItemIndx]["content"].c_str()).c_str());
			break;
		}
	}

	*pResult = 0;
}


void CcrawlerclientDlg::OnClickedRefreshButton()
{
	// TODO: 在此添加控件通知处理程序代码
	logList.DeleteAllItems();
	FILE* fp = NULL;
	CHAR strBuf[1024];
	std::string sPid;
	for (const auto & entry : fs::directory_iterator(theApp.pLogpath))
	{
		sPid = "";
		if (entry.status().type() == fs::file_type::directory)
		{
			if (!_wfopen_s(&fp, (entry.path() / "pid").c_str(), L"rb"))
			{
				while (!feof(fp))
				{
					memset(strBuf, 0, sizeof(strBuf));
					fread(strBuf, 1, 1024, fp);
					sPid += strBuf;
				}
				logList.InsertItem(0, entry.path().filename().c_str());
				logList.SetItemText(0, 1, U82WS(sPid).c_str());
				logList.SetItemText(0, 2, L"监控");
				fclose(fp);
				fp = NULL;
			}
			else
			{

				logList.InsertItem(0, entry.path().filename().c_str());
				logList.SetItemText(0, 1, L"N/A");
				logList.SetItemText(0, 2, L"停止");
			}
		}
	}
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

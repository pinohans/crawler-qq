// logdlg.cpp: 实现文件
//

#include "stdafx.h"
#include "crawler-client.h"
#include "logdlg.h"
#include "afxdialogex.h"


// logdlg 对话框

IMPLEMENT_DYNAMIC(logdlg, CDialogEx)

logdlg::logdlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_logdlg1, pParent)
{

}

logdlg::~logdlg()
{
}

void logdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, moduleCombo);
	DDX_Control(pDX, IDC_LIST1, logList);
	DDX_Control(pDX, IDC_COMBO3, dateCombo);
}


BEGIN_MESSAGE_MAP(logdlg, CDialogEx)
ON_CBN_DROPDOWN(IDC_COMBO1, &logdlg::OnDropdownCombo1)
ON_CBN_DROPDOWN(IDC_COMBO3, &logdlg::OnDropdownCombo3)
ON_CBN_SELCHANGE(IDC_COMBO3, &logdlg::OnSelchangeCombo3)
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &logdlg::OnGetdispinfoList1)
ON_BN_CLICKED(IDC_BUTTON1, &logdlg::OnBnClickedButton1)
ON_WM_TIMER()
END_MESSAGE_MAP()


// logdlg 消息处理程序


BOOL logdlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//// combo初始化

	this->OnDropdownCombo1();
	this->OnDropdownCombo3();
	dateCombo.SetCurSel(0);

	//// listctrl初始化
	logList.InsertColumn(0, _T("时间"), LVCFMT_LEFT, 150, 0);
	logList.InsertColumn(1, _T("级别"), LVCFMT_LEFT, 100, 1);
	logList.InsertColumn(2, _T("内容"), LVCFMT_LEFT, 200, 2);

	this->OnBnClickedButton1();
	SetTimer(2, 1000 * 10, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void logdlg::OnDropdownCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString module = L"main";
	int nIndex = moduleCombo.GetCurSel();
	if(nIndex != CB_ERR)
		moduleCombo.GetLBText(moduleCombo.GetCurSel(), module);
	moduleCombo.ResetContent();
	int cnt = 0;
	int last = 0;
	for (const auto & entry : fs::directory_iterator(troycrawler::pFilepath))
	{
		if (entry.status().type() == fs::file_type::directory)
		{
			std::wstring sModule = entry.path().filename().wstring();
			moduleCombo.InsertString(cnt++, sModule.c_str());
			if (!sModule.compare(std::wstring(module)))
			{
				last = cnt - 1;
			}
		}
	}
	moduleCombo.SetCurSel(last);
}


void logdlg::OnDropdownCombo3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString module = L"main", date = L"";
	int nIndex = moduleCombo.GetCurSel();
	if (nIndex != CB_ERR)
		moduleCombo.GetLBText(moduleCombo.GetCurSel(), module);
	nIndex = dateCombo.GetCurSel();
	if (nIndex != CB_ERR)
		dateCombo.GetLBText(dateCombo.GetCurSel(), date);
	dateCombo.ResetContent();

	BOOL bLastFlag = FALSE;
	int last = 0;
	for (const auto & entry : fs::directory_iterator(troycrawler::pFilepath / std::wstring(module)))
	{
		if (entry.status().type() == fs::file_type::regular)
		{
			std::wstring sDate = entry.path().filename().wstring();
			dateCombo.InsertString(0, sDate.c_str());
			if (bLastFlag)
			{
				last++;
			}
			else if (!sDate.compare(std::wstring(date)))
			{
				bLastFlag = TRUE;
			}
		}
	}
	dateCombo.SetCurSel(last);
}


void logdlg::OnSelchangeCombo3()
{
	// TODO: 在此添加控件通知处理程序代码
	this->OnBnClickedButton1();
	
}

void logdlg::OnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	LV_ITEM* pItem = &(pDispInfo)->item;

	int iItemIndx = pItem->iItem;

	if (pItem->mask & LVIF_TEXT) //字符串缓冲区有效
	{
		switch (pItem->iSubItem) {
		case 0: //填充数据项的名字
			lstrcpyW(pItem->pszText, U82WS(v[iItemIndx].sTime).substr(0, 125).c_str());
			break;
		case 1: //填充子项1
			lstrcpyW(pItem->pszText, U82WS(v[iItemIndx].sLevel).substr(0, 125).c_str());
			break;
		case 2: //填充子项2
			lstrcpyW(pItem->pszText, U82WS(v[iItemIndx].sMessage).substr(0, 125).c_str());
			break;
		}
	}

	*pResult = 0;
}


void logdlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString module = L"", date = L"";
	int nIndex = moduleCombo.GetCurSel();
	if (nIndex != CB_ERR)
		moduleCombo.GetLBText(moduleCombo.GetCurSel(), module);
	nIndex = dateCombo.GetCurSel();
	if (nIndex != CB_ERR)
		dateCombo.GetLBText(dateCombo.GetCurSel(), date);

	std::string sModule = WS2U8(std::wstring(module));
	std::string sDate = WS2U8(std::wstring(date));

	v = troycrawler::log::get(sModule, sDate);

	logList.SetItemCount(v.size());
	logList.EnsureVisible(0, false);
}


void logdlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	switch (nIDEvent)
	{
	case 2:
		this->OnBnClickedButton1();
	default:
		break;
	}
}

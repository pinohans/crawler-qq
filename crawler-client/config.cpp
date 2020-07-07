// config.cpp: 实现文件
//

#include "stdafx.h"
#include "crawler-client.h"
#include "config.h"
#include "afxdialogex.h"


// config 对话框

IMPLEMENT_DYNAMIC(config, CDialogEx)

config::config(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_config, pParent)
{

}

config::~config()
{
}

void config::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, configlist);
	DDX_Control(pDX, IDC_EDIT1, urlEdit);
}


BEGIN_MESSAGE_MAP(config, CDialogEx)
	ON_BN_CLICKED(ID_Add, &config::OnBnClickedAdd)
	ON_BN_CLICKED(ID_Delete, &config::OnBnClickedDelete)
END_MESSAGE_MAP()


// config 消息处理程序


BOOL config::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	configlist.SetExtendedStyle(configlist.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	RECT rect;
	configlist.GetWindowRect(&rect);
	configlist.InsertColumn(0, _T("id"), LVCFMT_LEFT, 50, 0);
	configlist.InsertColumn(1, _T("URL"), LVCFMT_LEFT, rect.right - rect.left - 51, 1);
	this->Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void config::OnBnClickedAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CString url;
	urlEdit.GetWindowText(url);
	troycrawler::config::add("url", std::string(WS2U8(std::wstring(url))));
	this->Refresh();
}


void config::Refresh()
{
	// TODO: 在此处添加实现代码.

	configlist.DeleteAllItems();

	std::map<int,std::string> mUrl = troycrawler::config::get("url");
	for(std::map<int, std::string>::iterator it = mUrl.begin(); it != mUrl.end(); ++it)
	{
		CString id = U82WS(std::to_string(it->first)).c_str();
		CString url = U82WS(it->second).c_str();

		configlist.InsertItem(0, id);
		configlist.SetItemText(0, 1, url);
	}
}


void config::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = configlist.GetFirstSelectedItemPosition();
	int id = atoi(WS2U8(std::wstring(configlist.GetItemText(configlist.GetNextSelectedItem(pos), 0))).c_str());

	troycrawler::config::del(id);
	this->Refresh();
}

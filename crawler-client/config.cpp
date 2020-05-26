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
	theApp.log->doConfig("url", std::string(WS2U8(std::wstring(url))), FALSE);
	this->Refresh();
}


void config::Refresh()
{
	// TODO: 在此处添加实现代码.

	configlist.DeleteAllItems();

	std::filesystem::path pLogFilename = theApp.log->path / "config.db";
	if (this->sql) {}
	else
	{
		int result = sqlite3_open_v2(pLogFilename.string().c_str(), &this->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
		if (result != SQLITE_OK)
		{
			this->sql = NULL;
			return;
		}
	}
	sqlite3_stmt *stmt = NULL;

	int result = sqlite3_prepare_v2(this->sql, "CREATE TABLE IF NOT EXISTS config( id integer PRIMARY KEY AUTOINCREMENT, module text NOT NULL, key text NOT NULL, value text NOT NULL ); ", -1, &stmt, NULL);

	if (result == SQLITE_OK) {
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		result = sqlite3_prepare_v2(this->sql, "SELECT id, value FROM config WHERE key='url'; ", -1, &stmt, NULL);

		if (result == SQLITE_OK) {

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				CString id = U82WS(std::to_string(sqlite3_column_int(stmt, 0))).c_str();
				CString url = U82WS(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))).c_str();

				configlist.InsertItem(0, id);
				configlist.SetItemText(0, 1, url);
			}
			sqlite3_finalize(stmt);
		}
	}
}


void config::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION pos = configlist.GetFirstSelectedItemPosition();
	int id = atoi(WS2U8(std::wstring(configlist.GetItemText(configlist.GetNextSelectedItem(pos), 0))).c_str());

	theApp.log->deleteConfig("url", id);
	this->Refresh();
}

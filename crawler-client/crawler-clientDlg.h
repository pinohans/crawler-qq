
// crawler-clientDlg.h: 头文件
//

#pragma once
#define WM_NOTIFYMENU WM_USER

// CcrawlerclientDlg 对话框
class CcrawlerclientDlg : public CDialogEx
{
// 构造
public:
	CcrawlerclientDlg(CWnd* pParent = nullptr);	// 标准构造函数
	injection* _injection;
	sqlite3 *sql_log = NULL;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CRAWLERCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CMenu menu, *pSubmenu;
	CPoint point;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNotifyMenu(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnExit();
	NOTIFYICONDATA nid;
	afx_msg void OnLog();
	afx_msg void OnStart();
	afx_msg void OnStop();
	CListCtrl logList;
	CButton refreshButton;
	afx_msg void OnClickedRefreshButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnConfig();
};

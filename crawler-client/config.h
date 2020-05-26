#pragma once


// config 对话框

class config : public CDialogEx
{
	DECLARE_DYNAMIC(config)
	
public:
	config(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~config();
	sqlite3 * sql = NULL;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_config };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl configlist;
	afx_msg void OnBnClickedAdd();
	void Refresh();
	CEdit urlEdit;
	afx_msg void OnBnClickedDelete();
};

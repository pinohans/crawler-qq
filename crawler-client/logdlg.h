#pragma once


// logdlg 对话框

struct Message
{
	std::string sTime;
	std::string sLevel;
	std::string sMessage;

	Message(){}
	Message(
		const unsigned char* _sTime,
		const unsigned char* _sLevel,
		const unsigned char* _sMessage) {
		this->sTime = std::string(reinterpret_cast<const char*>(_sTime));
		this->sLevel = std::string(reinterpret_cast<const char*>(_sLevel));
		this->sMessage = std::string(reinterpret_cast<const char*>(_sMessage));
	}
};

class logdlg : public CDialogEx
{
	DECLARE_DYNAMIC(logdlg)

public:
	logdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~logdlg();
	sqlite3 *sql = NULL; // 一个打开的数据库实例
	std::vector<Message> v;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_logdlg1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox moduleCombo;
	CListCtrl logList;
	CComboBox dateCombo;
	afx_msg void OnDropdownCombo1();
	afx_msg void OnDropdownCombo3();
	afx_msg void OnSelchangeCombo3();
	afx_msg void OnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

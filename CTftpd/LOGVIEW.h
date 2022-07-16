#pragma once


// LOGVIEW 对话框

class LOGVIEW : public CDialogEx
{
	DECLARE_DYNAMIC(LOGVIEW)

public:
	LOGVIEW(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~LOGVIEW();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString LOG;
	afx_msg void OnEnChangeEdit1();
};

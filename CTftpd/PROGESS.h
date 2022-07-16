#pragma once


// PROGESS 对话框

class PROGESS : public CDialogEx
{
	DECLARE_DYNAMIC(PROGESS)

public:
	PROGESS(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PROGESS();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult);
	long TOTBYTES;
	CProgressCtrl pbar;
	afx_msg void OnEnChangeEdit2();
	virtual BOOL OnInitDialog();
	long throughput;
};

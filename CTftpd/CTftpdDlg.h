
// CTftpdDlg.h: 头文件
//
#include "PROGESS.h"
#pragma once


// CCTftpdDlg 对话框
class CCTftpdDlg : public CDialogEx
{
// 构造
public:
	CCTftpdDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTFTPD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeDirectory();
	CString CD;
	CString HOST;
	CString PORT;
	PROGESS* progess;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	CString UpName;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox mode;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	int upload(char* ip, char* port, char* file_name, char* remotefilename, int mode);
	
};

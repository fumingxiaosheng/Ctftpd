#pragma once


// PROGESS2 对话框

class PROGESS2 : public CDialogEx
{
	DECLARE_DYNAMIC(PROGESS2)

public:
	PROGESS2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PROGESS2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGESS2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	long ReceiveBytes;
	long tuntu;
};

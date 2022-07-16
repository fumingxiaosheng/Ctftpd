// PROGESS.cpp: 实现文件
//

#include "pch.h"
#include "CTftpd.h"
#include "PROGESS.h"
#include "afxdialogex.h"


// PROGESS 对话框

IMPLEMENT_DYNAMIC(PROGESS, CDialogEx)

PROGESS::PROGESS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGESS, pParent)
	, TOTBYTES(0)
	, throughput(0)
{

}

PROGESS::~PROGESS()
{
}

void PROGESS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, TOTBYTES);
	DDX_Control(pDX, IDC_PROGRESS1, pbar);
	DDX_Text(pDX, IDC_EDIT1, throughput);
}


BEGIN_MESSAGE_MAP(PROGESS, CDialogEx)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &PROGESS::OnNMCustomdrawProgress1)
	ON_EN_CHANGE(IDC_EDIT2, &PROGESS::OnEnChangeEdit2)
END_MESSAGE_MAP()


// PROGESS 消息处理程序


void PROGESS::OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void PROGESS::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


BOOL PROGESS::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_STATIC)->ShowWindow(TRUE);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// LOGVIEW.cpp: 实现文件
//

#include "pch.h"
#include "CTftpd.h"
#include "LOGVIEW.h"
#include "afxdialogex.h"


// LOGVIEW 对话框

IMPLEMENT_DYNAMIC(LOGVIEW, CDialogEx)

LOGVIEW::LOGVIEW(CWnd* pParent /*=nullptr*/)
	: CDialogEx(ID_LOG, pParent)
	, LOG(_T(""))
{

}

LOGVIEW::~LOGVIEW()
{
}

void LOGVIEW::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, LOG);
}


BEGIN_MESSAGE_MAP(LOGVIEW, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &LOGVIEW::OnEnChangeEdit1)
END_MESSAGE_MAP()


// LOGVIEW 消息处理程序


void LOGVIEW::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

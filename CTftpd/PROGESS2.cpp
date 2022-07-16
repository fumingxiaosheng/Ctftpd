// PROGESS2.cpp: 实现文件
//

#include "pch.h"
#include "CTftpd.h"
#include "PROGESS2.h"
#include "afxdialogex.h"


// PROGESS2 对话框

IMPLEMENT_DYNAMIC(PROGESS2, CDialogEx)

PROGESS2::PROGESS2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGESS2, pParent)
	, ReceiveBytes(0)
	, tuntu(0)
{

}

PROGESS2::~PROGESS2()
{
}

void PROGESS2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, ReceiveBytes);
	DDX_Text(pDX, IDC_EDIT2, tuntu);
}


BEGIN_MESSAGE_MAP(PROGESS2, CDialogEx)
END_MESSAGE_MAP()


// PROGESS2 消息处理程序


// CTftpdDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CTftpd.h"
#include "CTftpdDlg.h"
#include "afxdialogex.h"
#include "LOGVIEW.h"
#include "PROGESS2.h"

//应用程序需要的头文件
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<iostream>
#include<cstring>
#include <time.h>
using namespace std;
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dl

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//与程序有关的宏定义
#define FILE_NAME_MAX_SIZE 512
#define BUF_SIZE 1024
#define DATA_MAX_SIZE 512

//全局变量区
long totbytes=0;
long curbytes = 0;
int open = 1;
//PROGESS* progess;
//需要调用的函数
//调用的函数区
int upload(char* ip, char* port, char* file_name, char* remotefilename, int mode);
int download(char* ip, char* port, char* file_name, char* remotefilename, int mode);
DWORD WINAPI Dprogress(LPVOID lpParamter);
DWORD WINAPI Dprogress2(LPVOID lpParamter);
int makereq(char type, int mode, char* filename, char* buffer, int size)
{
	int pos = 0; // 位置
	int i = 0;
	char s[32] = "";
	if (mode == 0) // 传输模式
		strcpy_s(s, "netascii");
	else
		strcpy_s(s, "octet");
	buffer[pos] = 0;
	pos++;
	buffer[pos] = type; // Opcode = 01(RRQ) or 02(WRQ)
	pos++;

	for (i = 0; i < strlen(filename); i++) // &buffer[3] = Filename
	{
		buffer[pos] = filename[i];
		pos++;
	}
	buffer[pos] = 0; // 0
	pos++;
	for (i = 0; i < strlen(s); i++) // Mode
	{
		buffer[pos] = s[i];
		pos++;
	}
	buffer[pos] = 0; // 0
	pos++;
	return pos; // 返回请求包头长度
}

int makedata(int num, char* data, int datasize, char* buffer, int bufsize)
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = 3; // Opcode = 03
	pos++;
	buffer[pos] = (char)(num >> 8); // Block # 块号
	pos++;
	buffer[pos] = (char)num;
	pos++;
	memcpy(&buffer[pos], data, datasize); // Data 数据
	pos = pos + datasize;

	return pos;
}
//int makeack(unsigned short num, char* buffer, int size)
int makeack(char num1, char num2, char* buffer, int size)
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = 4; // Opcode = 04
	pos++;
	//buffer[pos] = (char)(num >> 8); // 块号2个字节
	buffer[pos] = num1;
	pos++;
	//buffer[pos] = (char)num;
	buffer[pos] = num2;
	pos++;
	return pos;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCTftpdDlg 对话框



CCTftpdDlg::CCTftpdDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTFTPD_DIALOG, pParent)
	, CD(_T(""))
	, HOST(_T(""))
	, PORT(_T(""))
	, UpName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	progess = NULL;
}

void CCTftpdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE2, CD);
	DDX_Text(pDX, IDC_EDIT1, HOST);
	DDX_Text(pDX, IDC_EDIT2, PORT);
	DDX_Text(pDX, IDC_EDIT3, UpName);
	DDX_Control(pDX, IDC_COMBO1, mode);
}

BEGIN_MESSAGE_MAP(CCTftpdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_EN_CHANGE(IDC_Directory, &CCTftpdDlg::OnEnChangeDirectory)
	ON_BN_CLICKED(IDC_BUTTON1, &CCTftpdDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CCTftpdDlg::OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CCTftpdDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCTftpdDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCTftpdDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CCTftpdDlg 消息处理程序

BOOL CCTftpdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	mode.AddString(_T("Netascii"));
	mode.AddString(_T("Octet"));
	mode.SetCurSel(0);//默认选择第一个
	//清空日志文件
	FILE* stream;
	freopen_s(&stream, "D:\\计网实验\\out.out", "w", stdout);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCTftpdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCTftpdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCTftpdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCTftpdDlg::OnEnChangeDirectory()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CCTftpdDlg::OnBnClickedButton1()
{

	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	USES_CONVERSION;
	//upload需要的参数为:IP地址，端口号，本地文件路径，还有上传的路径
	//错误显示需要一个提示框，包括请求失败，以及一些error的包的原因等等
	//最后需要记录重传的包的个数，总的传输时间显示吞吐量
	char* ip = T2A(HOST);
	char* port = T2A(PORT);
	char* localfile = T2A(CD);
	char* remotefilename = T2A(UpName);
	int m = mode.GetCurSel();
	//设置输出的缓冲区
	/*progess = new PROGESS;
	//(*progess).OnInitDialog();
	progess->Create(IDD_PROGESS, this);
	(*progess).TOTBYTES = 10;
	(*progess).UpdateData(FALSE);
	progess->ShowWindow(SW_SHOW);
	UpdateWindow();*/
	//progess->ShowWindow(SW_SHOW);*/
	FILE* stream;
	freopen_s(&stream, "D:\\计网实验\\out.out", "a", stdout);
	upload(ip, port, localfile, remotefilename, m);
	if (stream) fclose(stream);
}



int CCTftpdDlg::upload(char *ip,char* port,char* file_name,char* remotefilename,int mode)
{
	open = 1;
	curbytes = 0;
	long tot = 0, repeat = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	/*服务端地址*/
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;//定义使用TCP、UDP地址族
	//server_addr.sin_addr.s_addr = inet_addr("10.11.160.190");
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(atoi(port));

	/*创建socket*/
	int client_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_socket_fd == INVALID_SOCKET) {
		//printf("%d\n", WSAGetLastError());
		//perror("Create Socket Failed:");
		return 0;
	}
	//超时设置
	int nNetTimeout = 3000;
	//struct timeval timer;
	//timer.tv_usec = 1000;
	setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
	/*输入文件名到缓冲区*/

	/*日志信息记录*/
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	//printf("\007The current date/time is: %s", asctime(timeinfo));
	if (mode) printf("upload file <%s> mode octet :%s",file_name, asctime(timeinfo));
	else printf("upload file <%s> mode asc :%s \n",file_name, asctime(timeinfo));
	printf("<destination> %s:%s\n", ip, port);
	/*吞吐量计算操作*/
	double start = 0, finish = 0;

	/*打开文件*/
	FILE* fp;
	if (mode == 0) fopen_s(&fp, file_name, "r");
	else fopen_s(&fp, file_name, "rb");
	if (fp == NULL) {
		printf("<error>:fail to open local file");
		//MessageBox(NULL, _TEXT("fail to open file"), _TEXT("ERROR"), MB_OK);
		MessageBoxA(NULL, (LPCSTR)("<error>:fail to open local file"), (LPCSTR)("ERROR"), MB_OK);
		//fclose(fp);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	totbytes = ftell(fp);
	//printf("文件大小为%ld", totbytes);
	fseek(fp, 0, SEEK_SET);

	char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(buf));
	int size = makereq(2, mode, remotefilename, buf, BUF_SIZE);
	int key = 0;
resendr:
	int a = clock();
	//cout << "resend" << endl;
	if (++key && sendto(client_socket_fd, buf, size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("Send file name failed:");
		return 0;
	}

	/*接收服务器返回的新的协商端口*/
	struct sockaddr_in server_addr_new;
	int new_length = sizeof(server_addr_new);
	char recv[BUF_SIZE + 1];
	int datalength = recvfrom(client_socket_fd, recv, BUF_SIZE, 0, (struct sockaddr*)(&server_addr_new), &new_length);
	if (datalength == SOCKET_ERROR) {
		int b = clock();
		//printf("%d 超时时间为:%d\n", datalength, b - a);
		//perror("time limit for");
		if (key <= 3) goto resendr;
		printf("<error> WRQ Time out\n");
		MessageBoxA(NULL, (LPCSTR)("<error>:WRQ Time Out"), (LPCSTR)("ERROR"), MB_OK);
		return 0;
	}
	server_addr.sin_port = server_addr_new.sin_port;
	//printf("请求%x %x %x %x\n", recv[0], recv[1], recv[2], recv[3]);
	/*逐块发送数据*/
	//打开进度条窗口
	//设置ACK时间
	//nNetTimeout = 3000;
	//setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
	HANDLE hThread = CreateThread(NULL, 0, Dprogress, NULL, 0, NULL);
	Sleep(1);
	char data[DATA_MAX_SIZE + 1];
	memset(data, 0, sizeof(data));
	
	int i, num;
	double startu, finishu;
	startu = clock();
	for (i = 0, num = 1; !feof(fp);) {
		data[i++] = fgetc(fp);
		if (i % 512 == 0) {
			int key = 0;
			memset(buf, 0, sizeof(buf));
			size = makedata(num++, data, i, buf, BUF_SIZE);
		reup:
			int a = clock();
			//printf("send data %d:%d\n", num - 1, key);
			if (++key && sendto(client_socket_fd, buf, size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
				printf("send data %d fail", num - 1);
				return 0;
			}
			//接收ACK报文，并验证，只有当ACK时才会继续
			datalength = recvfrom(client_socket_fd, recv, BUF_SIZE, 0, (struct sockaddr*)(&server_addr_new), &new_length);
			//cout << "外部的长度" <<datalength << endl;
			if (datalength == SOCKET_ERROR) {
				int d = clock();
				//printf("接收超时为:%d\n", d - a);
				//未在规定时间内接收ACK包
				//int b = clock();
				//cout << "data " << datalength << "time" << b-a << endl;// << "type" << recv[0] << recv[1] << endl;
				//perror("recvfrom error");
				if (key >= 10) {
					open = 0;
					char error[1000] = { 0 };
					sprintf(error, "<error>:Time out for waiting ACK %x %x", buf[2], buf[3]);
					printf("%s\n",error);
					MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
					return 0;
				}
				//printf("从1处跳转\n");
				repeat++;
				goto reup;
				return 0;
			}
			//当收到了error包时，应该直接退出即可
			if (recv[0] == 0 && recv[1] == 5) {
				open = 0;
				char error[1000] = { 0 };
				sprintf(error, "<error>: %s ",recv+4);
				printf("%s\n", error);
				MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
				return 0;
			}
			//printf("ack recv %x %x\n", recv[2], recv[3]);
			if (recv[2] != buf[2] || recv[3] != buf[3]) {
				//没有收到正确的ACK包
				//if()
				if (key >= 10) {
					open = 0;
					char error[1000] = { 0 };
					sprintf(error, "<error>:Time out for waiting ACK %x %x", buf[2], buf[3]);
					printf("%s\n", error);
					MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
					return 0;
				}
				//printf("从判断语句跳转:\n");
				repeat++;
				goto reup;
			}
			memset(data, 0, sizeof(data));
			curbytes += i;
			i %= 512;
			//printf("已经传输的字节数为%ld\n", curbytes);
			tot++;
		}
		//tot++;
	}
	//将剩余的内容发送过去
	if (i) {
		int key = 0;
		memset(buf, 0, sizeof(buf));
		size = makedata(num++, data, i, buf, BUF_SIZE);
	reup1:
		//printf("send data %d %d\n", num - 1, key);
		if (++key && sendto(client_socket_fd, buf, size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			printf("send data %d fail", num - 1);
			return 0;
		}
		if (recvfrom(client_socket_fd, recv, BUF_SIZE, 0, (struct sockaddr*)(&server_addr_new), &new_length) == SOCKET_ERROR) {
			//未在规定时间内接收ACK包
			if (key >= 10) {
				open = 0;
				char error[1000] = { 0 };
				sprintf(error, "<error>:Time out for waiting ACK %x %x", buf[2], buf[3]);
				printf("%s\n", error);
				MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
				return 0;
			}
			repeat++;
			goto reup1;
			return 0;
		}
		if (recv[2] != buf[2] || recv[3] != buf[3]) {
			//没有收到正确的ACK包
			if (key >= 10) {
				open = 0;
				char error[1000] = { 0 };
				sprintf(error, "<error>:Time out for waiting ACK %x %x", buf[2], buf[3]);
				printf("%s\n", error);
				MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
				return 0;
			}
			repeat++;
			goto reup1;
		}
	}
	tot++;
	curbytes += i;
	finishu = clock();
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("<%s>:%ld bytes ,%ld blks sent in %g ms, %ld resend :%s", file_name,curbytes,tot,finishu-startu, repeat, asctime(timeinfo));
	open = 0;
	return 0;
}

DWORD WINAPI Dprogress(LPVOID lpParamter) {
	double start = clock();
	int x = 1;
	//展示进度的窗口
	PROGESS *progess = new PROGESS();
	//(*progess).OnInitDialog();
	progess->Create(IDD_PROGESS, NULL);
	//cout << "在进度条中设置的长度" << (*progess).TOTBYTES << endl;
	(*progess).pbar.SetRange(0, totbytes);
	(*progess).TOTBYTES=totbytes;
	(*progess).throughput = 100;
	(*progess).UpdateData(FALSE);
	progess->ShowWindow(SW_SHOW);
	progess->UpdateWindow();

	while (open) {
		double finish = clock();
		(*progess).throughput = curbytes / (finish - start);
		if (finish - start >= 1000*x){
			(*progess).throughput =  (curbytes) / (((long)(finish - start))/1000);
			//cout << "吞吐量为" << (*progess).throughput << endl;
			(*progess).pbar.SetPos(curbytes);
			(*progess).UpdateData(FALSE);
			//(*progess).pbar.SetPos(curbytes);
			x++;
		}
		//(*progess).pbar.SetPos(curbytes);
		progess->UpdateWindow();
	}
	/*double finish = clock();
	(*progess).throughput = curbytes / (finish - start);
	progess->UpdateWindow();*/
	/*模态对话框是不行的*/
	/*
	PROGESS newD;
	newD.DoModal();
	newD.TOTBYTES = totbytes;
	newD.pbar.SetRange(0, totbytes);
	while (open) {
		newD.pbar.SetPos(curbytes);
	}*/
	return 0L;
}

void CCTftpdDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CCTftpdDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CCTftpdDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//download需要的参数为待下载的路径file_name,服务器上的文件路径remotefilename
	//传输的模式，端口号和IP
	UpdateData(TRUE);
	USES_CONVERSION;
	char* ip = T2A(HOST);
	char* port = T2A(PORT);
	char* localfile = T2A(CD);
	char* remotefilename = T2A(UpName);
	int m = mode.GetCurSel();
	//设置输出的缓冲区
	FILE* stream;
	freopen_s(&stream, "D:\\计网实验\\out.out", "a", stdout);
	download(ip, port, localfile, remotefilename, m);
	if(stream) fclose(stream);
}

int download(char* ip, char* port, char* file_name, char* remotefilename, int mode) {
	open = 1;
	curbytes = 0;
	long tot = 0, repeat = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	/*服务端地址*/
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;//定义使用TCP、UDP地址族
	//server_addr.sin_addr.s_addr = inet_addr("10.11.160.190");
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(atoi(port));

	/*创建socket*/
	int client_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_socket_fd == INVALID_SOCKET) {
		//printf("%d\n", WSAGetLastError());
		//perror("Create Socket Failed:");
		MessageBox(NULL, _TEXT("Create Socket Failed:"), _TEXT("ERROR"), MB_OK);
		return 0;
	}
	//超时设置
	int nNetTimeout = 10000;
	//struct timeval timer;
	//timer.tv_usec = 1000;
	setsockopt(client_socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	if (mode) printf("download file <%s> mode octet :%s", remotefilename, asctime(timeinfo));
	else printf("download file <%s> mode asc :%s", remotefilename, asctime(timeinfo));
	printf("<source> %s:%s\n", ip, port);
	FILE* fp;
	if (mode == 0) fopen_s(&fp, file_name, "w");
	else fopen_s(&fp, file_name, "wb");
	if (fp == NULL) {
		printf("<error>:fail to open local file");
		MessageBoxA(NULL, (LPCSTR)("<error>:fail to open local file\n"), (LPCSTR)("ERROR"), MB_OK);

		return 0;
	}
	/*发送RRQ*/
	int key = 0;//代表rrq请求没有成功
	int retime = 0;//代表重新发送的次数


	char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(buf));
	int size = makereq(1, mode, remotefilename, buf, BUF_SIZE);
resent:
	double start, end;
	start = clock();
	if (sendto(client_socket_fd, buf, size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("Send requested file name failed:");
		//MessageBox(NULL, _TEXT("ERROR"), _TEXT("FAIL TO OPEN LOCAL FILE"), MB_YESNO | MB_ICONQUESTION);
		return 0;
	}
	/*接收服务器返回的新的协商端口*/
	struct sockaddr_in server_addr_new;
	int new_length = sizeof(server_addr_new);
	char recv[BUF_SIZE + 1];
	//数据包调试区域
	//用于避免重复
	char pre1 = 0;
	char pre2 = 0;
	int bytes = 0;
	int blocknum = 0;
	int i = 0;
	double startu = clock();
	double finishu = 0;
	int k2 = 1;
	while (1) {
		if (key && k2) {
			HANDLE hThread = CreateThread(NULL, 0, Dprogress2, NULL, 0, NULL);
			k2 = 0;
		}
		Sleep(1);
		int datalength = recvfrom(client_socket_fd, recv, BUF_SIZE, 0, (struct sockaddr*)(&server_addr_new), &new_length);
		//cout << "length"<<datalength << endl;
		if (datalength == SOCKET_ERROR) {
			end = clock();
			if (!key) {
				//对于请求的重发机制
				//cout << "resent:" << end - start << endl;
				retime++;
				if (retime >= 3) {
					printf("<error> RRQ Time out\n");
					MessageBoxA(NULL, (LPCSTR)("<error>:RRQ Time Out"), (LPCSTR)("ERROR"), MB_OK);
					//cout << "Can not connect Server,request fail!" << endl;
					//MessageBox(NULL, _TEXT("CAN NOT CONNECT SERVER"), _TEXT("ERROR"), MB_OK);
					return 0;
				}
				goto resent;
			}
			//printf("接收完成，共需接收时间为%.2f\n", (end - start)/1000);
			//最后超时了就标识传输完成
			//printf("receive %d blks,%d bytes in %.2f s", blocknum, bytes, (end - start) / 1000);
			finishu = clock();
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			printf("<%s>:%ld bytes ,%ld blks receive in %g ms, %ld blks resend <%s> \n", file_name, curbytes, tot, finishu - startu - nNetTimeout, repeat, asctime(timeinfo));
			open = 0;
			return 1;
		}
		key = 1;//请求重发机制
		//如果收到了error包，直接退出即可
		if (recv[0] == 0 && recv[1] == 5) {
			open = 0;
			char error[1000] = { 0 };
			sprintf(error, "<error>: %s ", recv + 4);
			printf("%s\n", error);
			MessageBoxA(NULL, (LPCSTR)(error), (LPCSTR)("ERROR"), MB_OK);
			return 0;
		}
		if (((unsigned char)(pre1) == (unsigned char)(recv[2]) && (unsigned char)(pre2) >= (unsigned char)(recv[3])) || (unsigned char)(pre1) > (unsigned char)(recv[2])) {//避免重复接收包机制
			//printf("%x%x repeat!\n", recv[2], recv[3]);
			repeat++;
			//continue;
			goto sendack;//注意收到重复的包时，应该再次发送ACK
		}
		pre1 = recv[2];
		pre2 = recv[3];
		blocknum++;
		tot++;
		server_addr.sin_port = server_addr_new.sin_port;

		//将recv中的字符拷贝到文件中
		//int i = 4;
		//recv[i] >= -128 && recv[i]<=127 && i<=515 && 
		/*while (i<datalength) {
			if (fputc(recv[i], fp) == EOF)  cout << "fputc question" << recv[i] << endl;
			//if(recv[i]=='\n') printf("%d\n",++u );
			//if ((j + 1) % 70 == 0) cout << endl;
			//printf("%d %d %d\n", i, recv[i],recv[i+1]);
			i++;
			bytes++;
		}*/
		//将数据递交
		::fwrite(recv + 4, datalength - 4, 1, fp);
		//printf("%x %x %x %x %x %x\n", recv[4], recv[5], recv[6], recv[7], recv[8], recv[9]);
		i += datalength - 4;
		curbytes+= datalength - 4;
		//发送ACK报文
	sendack:
		memset(buf, 0, sizeof(buf));
		size = makeack(recv[2], recv[3], buf, BUF_SIZE);
		if (sendto(client_socket_fd, buf, size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
			perror("Send ack failed:");
			return 0;
		}
		//printf("send ack:%x %x total length = %d \n", recv[2], recv[3], i);
	}
}

DWORD WINAPI Dprogress2(LPVOID lpParamter) {
	double start = clock();
	int x = 1;
	//展示进度的窗口
	PROGESS2* progess2 = new PROGESS2();
	progess2->Create(IDD_PROGESS2, NULL);
	progess2->ShowWindow(SW_SHOW);
	progess2->UpdateWindow();

	while (open) {
		double finish = clock();
		//(*progess).throughput = curbytes / (finish - start);
		if (finish - start >= 1000 * x) {
			(*progess2).tuntu = (curbytes) / (((long)(finish - start)) / 1000);
			(*progess2).ReceiveBytes = curbytes;
			(*progess2).UpdateData(FALSE);
			x++;
		}
		progess2->UpdateWindow();
	}

	return 0L;
}
void CCTftpdDlg::OnBnClickedButton3()
{

	// TODO: 在此添加控件通知处理程序代码
	LOGVIEW logviewer;
	FILE* fp;
	//fopen_s(&fp, "D:\\计网实验\\CTftpd\\LOGVIEW.txt", "r");
	fopen_s(&fp, "D:\\计网实验\\out.out", "r");
	USES_CONVERSION;
	char logview[10000];//注意设置这个缓冲区的大小
	memset(logview, 0, 10000);
	char cur[1000];
	memset(cur, 0, 1000);
	int length = 0;
	char c;
	//fread(logview,1000,1,fp);
	//logviewer.LOG = CA2CT(logview);
	if (fp == NULL) return;
	while (fgets(cur,1000,fp) != NULL) {
		strcat_s(cur, "\r\n");
		strcat_s(logview, cur);
	}
	logviewer.LOG = CA2CT(logview);
	UpdateData(FALSE);
	//fwitre();
	logviewer.DoModal();
	fclose(fp);
}

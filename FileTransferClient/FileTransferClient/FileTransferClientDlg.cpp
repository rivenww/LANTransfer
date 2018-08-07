
// FileTransferClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileTransferClient.h"
#include "FileTransferClientDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileTransferClientDlg 对话框




CFileTransferClientDlg::CFileTransferClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileTransferClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileTransferClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SAVE_DIR, m_editSaveDir);
	DDX_Control(pDX, IDC_RICHEDIT_RECORD, m_richeditRecord);
	DDX_Control(pDX, IDC_BUTTON_SEND_FILE, m_btnSendFile);
}

BEGIN_MESSAGE_MAP(CFileTransferClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND_FILE, &CFileTransferClientDlg::OnBnClickedButtonSendFile)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DIR, &CFileTransferClientDlg::OnBnClickedButtonSelectDir)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIR, &CFileTransferClientDlg::OnBnClickedButtonOpenDir)
	ON_EN_CHANGE(IDC_RICHEDIT_RECORD, &CFileTransferClientDlg::OnEnChangeRicheditRecord)
	ON_EN_CHANGE(IDC_EDIT_SAVE_DIR, &CFileTransferClientDlg::OnEnChangeEditSaveDir)
END_MESSAGE_MAP()


// CFileTransferClientDlg 消息处理程序

BOOL CFileTransferClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
#pragma region 基本初始化
	char szDeskPath[256] = {0};
	SHGetSpecialFolderPath(0,szDeskPath,CSIDL_DESKTOPDIRECTORY,0);
	CString strDeskPath = _T("");
	strDeskPath.Format(_T("%s\\%s%d"), szDeskPath, _T("RecvFileDirClient"), GetTickCount());
	m_editSaveDir.SetWindowText(strDeskPath);
	if(!PathFileExists(strDeskPath))
	{
		CreateDirectory(strDeskPath, NULL);
	}

	m_pWriteFileFp = NULL;
#pragma endregion

	AddOneRecord(&m_richeditRecord, _T("初始化Socket"));

#pragma region socket init
	WSADATA wsaData;
	int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if (err != 0 ) 
	{               
		MessageBox(_T("Error: WSAStartup失败"));
		exit(0);
	}

	m_socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_socketClient)
	{
		MessageBox(_T("Error： 连接服务器失败,socket失败"));
		exit(0);
	}

	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	m_remoteAddr.sin_port = htons(SERVER_LISTEN_PORT);

	AddOneRecord(&m_richeditRecord, _T("开始连接服务端"));

	if (connect(m_socketClient, (struct sockaddr *)&m_remoteAddr, 
		sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		int n = GetLastError();
		m_socketClient = INVALID_SOCKET;
		MessageBox(_T("Error： 连接服务器失败,请先启动服务端"));
		exit(0);
	}

	AddOneRecord(&m_richeditRecord, _T("服务端连接成功"));
	AddOneRecord(&m_richeditRecord, _T("创建线程监听服务端的消息"));

	//创建一个线程，用于接收银行返回的数据
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadFuncRecvMsg, this, 0, &dwThreadID);
	if (!hThread)
	{
		return FALSE;
	}
#pragma  endregion

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileTransferClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileTransferClientDlg::OnPaint()
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
HCURSOR CFileTransferClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



DWORD WINAPI CFileTransferClientDlg::ThreadFuncRecvMsg(LPVOID param)
{
	t_MsgInfo tMsgInfo;
	CFileTransferClientDlg *pThis = (CFileTransferClientDlg*)param;
	while(1)
	{	
		//第一步，需要收到发送文件的请求以及该请求中包含了发送端生成的公钥
		memset(&tMsgInfo, 0x00, sizeof(tMsgInfo));
		int nByte = recv(pThis->m_socketClient,(char*)&tMsgInfo,
			sizeof(tMsgInfo),0);
		if(nByte != sizeof(tMsgInfo))
		{
			continue;
		}

		switch(tMsgInfo.eMsgType)
		{
		case MSG_TYPE_NEW_FILE:
			{
				pThis->AddOneRecord(&pThis->m_richeditRecord, _T("收到服务端发送文件的消息"));
			}
			break;

		case MSG_TYPE_DATA_FILE:
			{
				pThis->RecvMsgFileData(&tMsgInfo, pThis->m_socketClient);
			}
			break;

		case MSG_TYPE_END_FILE:
			{
				pThis->AddOneRecord(&pThis->m_richeditRecord, _T("文件发送完成"));
			}
			break;

		default:
			break;
		}
	}

	

	return 0;
}

void CFileTransferClientDlg::OnBnClickedButtonSendFile()
{
	BOOL isOpen = TRUE;     //是否打开(否则为保存)  
	CString fileName = "";         //默认打开的文件名  
	CString filter = "文件 (*.*)|*.*||";   //文件过虑的类型  
	CFileDialog openFileDlg(isOpen, NULL, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
	INT_PTR result = openFileDlg.DoModal();  
	if(result != IDOK)
	{
		return;
	}

	m_strSendFilePath = openFileDlg.GetPathName();
	m_strFileName = openFileDlg.GetFileName();

	AddOneRecord(&m_richeditRecord, _T("创建线程发送文件开始"));
	m_btnSendFile.EnableWindow(FALSE);

	//创建线程发送文件
	DWORD dwThreadID = 0;
	CreateThread(NULL, 0, ThreadFuncSendFile, this, 0, &dwThreadID);
}

DWORD WINAPI CFileTransferClientDlg::ThreadFuncSendFile(LPVOID param)
{
	CFileTransferClientDlg *pThis = (CFileTransferClientDlg*)param;

	WIN32_FIND_DATA             FindFileData;
	UINT dwRead=0;
	CFile myFile;
	if(!myFile.Open(pThis->m_strSendFilePath, CFile::modeRead | CFile::typeBinary))
	{
		pThis->AddOneRecord(&pThis->m_richeditRecord, _T("文件不存在，发送失败"));
	}
	else
	{
		CString strShow = _T("");
		FindClose(FindFirstFile(pThis->m_strSendFilePath,&FindFileData));
		t_MsgInfo tMsgInfo;
		while(dwRead<FindFileData.nFileSizeLow)
		{
			
			////循环读取文件数据，然后send
			memset(&tMsgInfo, 0x00, sizeof(tMsgInfo));
			tMsgInfo.nSendDataLen = myFile.Read(tMsgInfo.szFileData, sizeof(tMsgInfo.szFileData));
			dwRead += tMsgInfo.nSendDataLen;
			tMsgInfo.lTotalSendDataLen = dwRead;
			tMsgInfo.lFileDataLen = FindFileData.nFileSizeLow;
			strcpy(tMsgInfo.szFileName, CT2A(pThis->m_strFileName));
			tMsgInfo.eMsgType = MSG_TYPE_DATA_FILE;
			//Sleep(100);
			send(pThis->m_socketClient,(char*)&tMsgInfo,sizeof(t_MsgInfo),0);

			
			float fSpeed = (100.0* tMsgInfo.lTotalSendDataLen)/(1.0 * tMsgInfo.lFileDataLen); 
			strShow.Format(_T("%s%s%s%f%s"),_T("正在发送文件‘"),pThis->m_strFileName, _T("’，进度："), fSpeed, _T("\%"));
			pThis->AddOneRecord(&pThis->m_richeditRecord,strShow);
		}
		myFile.Close();

		strShow.Format(_T("%s%s%s"), _T("文件‘"), pThis->m_strFileName, _T("’发送完成"));
		pThis->AddOneRecord(&pThis->m_richeditRecord,strShow);
	}
	
	pThis->m_btnSendFile.EnableWindow(TRUE);

	return 0;
}

#pragma region 选择保存文件的目录
void CFileTransferClientDlg::OnBnClickedButtonSelectDir()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("请选择文件保存路径");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			m_editSaveDir.SetWindowText(szFolderPath);
		}  
	}  
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}
}


void CFileTransferClientDlg::OnBnClickedButtonOpenDir()
{
	CString strDir = _T("");
	m_editSaveDir.GetWindowText(strDir);
	if(!PathIsDirectory(strDir))
	{
		MessageBox(_T("目录不存在"));
		return;
	}
	ShellExecute(NULL, _T("open"), strDir, NULL, NULL, SW_SHOWNORMAL);
}
#pragma endregion


void CFileTransferClientDlg::AddOneRecord(CRichEditCtrl *pRicheditCtr, CString strMsg)
{
	pRicheditCtr->SetSel(-1, -1);
	pRicheditCtr->ReplaceSel(strMsg + "\r\n");
	pRicheditCtr->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CFileTransferClientDlg::RecvMsgFileData(t_MsgInfo *ptMsgInfo, SOCKET sock)
{
	if(m_pWriteFileFp == NULL)
	{
		CString strMsg = _T("");
		strMsg.Format(_T("%s%s"), _T("有新文件上传，文件："),ptMsgInfo->szFileName);
		//AddOneRecord(&m_richeditRecord, strMsg);

		CString strSaveDir = _T("");
		m_editSaveDir.GetWindowText(strSaveDir);
		char szPath[256] = {0};
		strcpy(szPath, CT2A(strSaveDir));
		strcat(szPath,"\\");
		strcat(szPath,ptMsgInfo->szFileName);

		/*char szTick[20] = {0};
		itoa(GetTickCount(),szTick,10);
		strcat(szPath,szTick);*/
		m_pWriteFileFp = fopen(szPath,"wb");
		if(!m_pWriteFileFp)
		{
			m_pWriteFileFp = NULL;
			return;
		}
	}
	fwrite(ptMsgInfo->szFileData,1,ptMsgInfo->nSendDataLen,m_pWriteFileFp);

	CString strMsg = _T("");
	float fSpeed = (100.0 * ptMsgInfo->lTotalSendDataLen)/(1.0 * ptMsgInfo->lFileDataLen);
	fSpeed = fSpeed > 100 ? 100 : fSpeed;
	strMsg.Format(_T("%s%s%s%f%s"), _T("正在接收文件‘"),ptMsgInfo->szFileName, _T("’。  进度：  "), fSpeed, _T("\%"));
	AddOneRecord(&m_richeditRecord, strMsg);

	if(ptMsgInfo->lTotalSendDataLen >= ptMsgInfo->lFileDataLen)
	{
		//发送结束
		fclose(m_pWriteFileFp);
		m_pWriteFileFp = NULL;

		CString strMsg = _T("");
		strMsg.Format(_T("%s%s%s"), _T("文件‘"),ptMsgInfo->szFileName, _T("’接收完成"));
		AddOneRecord(&m_richeditRecord, strMsg);
	}

	return;
}

void CFileTransferClientDlg::OnEnChangeRicheditRecord()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CFileTransferClientDlg::OnEnChangeEditSaveDir()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

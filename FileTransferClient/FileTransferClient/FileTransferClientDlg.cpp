
// FileTransferClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileTransferClient.h"
#include "FileTransferClientDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CFileTransferClientDlg �Ի���




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


// CFileTransferClientDlg ��Ϣ�������

BOOL CFileTransferClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
#pragma region ������ʼ��
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

	AddOneRecord(&m_richeditRecord, _T("��ʼ��Socket"));

#pragma region socket init
	WSADATA wsaData;
	int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if (err != 0 ) 
	{               
		MessageBox(_T("Error: WSAStartupʧ��"));
		exit(0);
	}

	m_socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == m_socketClient)
	{
		MessageBox(_T("Error�� ���ӷ�����ʧ��,socketʧ��"));
		exit(0);
	}

	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	m_remoteAddr.sin_port = htons(SERVER_LISTEN_PORT);

	AddOneRecord(&m_richeditRecord, _T("��ʼ���ӷ����"));

	if (connect(m_socketClient, (struct sockaddr *)&m_remoteAddr, 
		sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		int n = GetLastError();
		m_socketClient = INVALID_SOCKET;
		MessageBox(_T("Error�� ���ӷ�����ʧ��,�������������"));
		exit(0);
	}

	AddOneRecord(&m_richeditRecord, _T("��������ӳɹ�"));
	AddOneRecord(&m_richeditRecord, _T("�����̼߳�������˵���Ϣ"));

	//����һ���̣߳����ڽ������з��ص�����
	DWORD dwThreadID = 0;
	HANDLE hThread = CreateThread(NULL, 0, ThreadFuncRecvMsg, this, 0, &dwThreadID);
	if (!hThread)
	{
		return FALSE;
	}
#pragma  endregion

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileTransferClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		//��һ������Ҫ�յ������ļ��������Լ��������а����˷��Ͷ����ɵĹ�Կ
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
				pThis->AddOneRecord(&pThis->m_richeditRecord, _T("�յ�����˷����ļ�����Ϣ"));
			}
			break;

		case MSG_TYPE_DATA_FILE:
			{
				pThis->RecvMsgFileData(&tMsgInfo, pThis->m_socketClient);
			}
			break;

		case MSG_TYPE_END_FILE:
			{
				pThis->AddOneRecord(&pThis->m_richeditRecord, _T("�ļ��������"));
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
	BOOL isOpen = TRUE;     //�Ƿ��(����Ϊ����)  
	CString fileName = "";         //Ĭ�ϴ򿪵��ļ���  
	CString filter = "�ļ� (*.*)|*.*||";   //�ļ����ǵ�����  
	CFileDialog openFileDlg(isOpen, NULL, fileName, OFN_HIDEREADONLY|OFN_READONLY, filter, NULL);  
	INT_PTR result = openFileDlg.DoModal();  
	if(result != IDOK)
	{
		return;
	}

	m_strSendFilePath = openFileDlg.GetPathName();
	m_strFileName = openFileDlg.GetFileName();

	AddOneRecord(&m_richeditRecord, _T("�����̷߳����ļ���ʼ"));
	m_btnSendFile.EnableWindow(FALSE);

	//�����̷߳����ļ�
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
		pThis->AddOneRecord(&pThis->m_richeditRecord, _T("�ļ������ڣ�����ʧ��"));
	}
	else
	{
		CString strShow = _T("");
		FindClose(FindFirstFile(pThis->m_strSendFilePath,&FindFileData));
		t_MsgInfo tMsgInfo;
		while(dwRead<FindFileData.nFileSizeLow)
		{
			
			////ѭ����ȡ�ļ����ݣ�Ȼ��send
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
			strShow.Format(_T("%s%s%s%f%s"),_T("���ڷ����ļ���"),pThis->m_strFileName, _T("�������ȣ�"), fSpeed, _T("\%"));
			pThis->AddOneRecord(&pThis->m_richeditRecord,strShow);
		}
		myFile.Close();

		strShow.Format(_T("%s%s%s"), _T("�ļ���"), pThis->m_strFileName, _T("���������"));
		pThis->AddOneRecord(&pThis->m_richeditRecord,strShow);
	}
	
	pThis->m_btnSendFile.EnableWindow(TRUE);

	return 0;
}

#pragma region ѡ�񱣴��ļ���Ŀ¼
void CFileTransferClientDlg::OnBnClickedButtonSelectDir()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  

	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("��ѡ���ļ�����·��");  
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;  
	sInfo.lpfn     = NULL;  

	// ��ʾ�ļ���ѡ��Ի���  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// ȡ���ļ�����  
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
		MessageBox(_T("Ŀ¼������"));
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
		strMsg.Format(_T("%s%s"), _T("�����ļ��ϴ����ļ���"),ptMsgInfo->szFileName);
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
	strMsg.Format(_T("%s%s%s%f%s"), _T("���ڽ����ļ���"),ptMsgInfo->szFileName, _T("����  ���ȣ�  "), fSpeed, _T("\%"));
	AddOneRecord(&m_richeditRecord, strMsg);

	if(ptMsgInfo->lTotalSendDataLen >= ptMsgInfo->lFileDataLen)
	{
		//���ͽ���
		fclose(m_pWriteFileFp);
		m_pWriteFileFp = NULL;

		CString strMsg = _T("");
		strMsg.Format(_T("%s%s%s"), _T("�ļ���"),ptMsgInfo->szFileName, _T("���������"));
		AddOneRecord(&m_richeditRecord, strMsg);
	}

	return;
}

void CFileTransferClientDlg::OnEnChangeRicheditRecord()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CFileTransferClientDlg::OnEnChangeEditSaveDir()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

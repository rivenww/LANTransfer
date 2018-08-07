
// FileTransferServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileTransferServer.h"
#include "FileTransferServerDlg.h"
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


// CFileTransferServerDlg �Ի���




CFileTransferServerDlg::CFileTransferServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileTransferServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileTransferServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_RECORD, m_richeditRecord);
	DDX_Control(pDX, IDC_EDIT_SAVE_DIR, m_editSaveDir);
	DDX_Control(pDX, IDC_LIST_CLIENT_LIST, m_listCtrlClient);
	DDX_Control(pDX, IDC_BUTTON_SEND_FILE, m_btnSendFile);
}

BEGIN_MESSAGE_MAP(CFileTransferServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MY_MSG, &CFileTransferServerDlg::OnMyMsg)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DIR, &CFileTransferServerDlg::OnBnClickedButtonSelectDir)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIR, &CFileTransferServerDlg::OnBnClickedButtonOpenDir)
	ON_BN_CLICKED(IDC_BUTTON_SEND_FILE, &CFileTransferServerDlg::OnBnClickedButtonSendFile)
	ON_EN_CHANGE(IDC_EDIT_SAVE_DIR, &CFileTransferServerDlg::OnEnChangeEditSaveDir)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CLIENT_LIST, &CFileTransferServerDlg::OnLvnItemchangedListClientList)
	ON_BN_CLICKED(IDC_BUTTON2, &CFileTransferServerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_ALLCHECK, &CFileTransferServerDlg::OnBnClickedAllcheck)
END_MESSAGE_MAP()


// CFileTransferServerDlg ��Ϣ�������

BOOL CFileTransferServerDlg::OnInitDialog()
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
	strDeskPath.Format(_T("%s\\%s"), szDeskPath, _T("RecvFileDirServer"));
	m_editSaveDir.SetWindowText(strDeskPath);
	if(!PathFileExists(strDeskPath))
	{
		CreateDirectory(strDeskPath, NULL);
	}

	//ListView_SetExtendedListViewStyle(m_listCtrlClient.GetSafeHwnd(), m_listCtrlClient.GetExStyle() | LVS_EX_CHECKBOXES); 

	DWORD dwStyle = m_listCtrlClient.GetExtendedStyle();
	dwStyle |= LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT| LVS_NOSCROLL;
	m_listCtrlClient.SetExtendedStyle(dwStyle);
	m_listCtrlClient.InsertColumn(0,_T("���"));//�����
	m_listCtrlClient.InsertColumn(1,_T("Socket"));

	CRect rect;
	m_listCtrlClient.GetClientRect(&rect);
	int nWidth = rect.Width()/2;
	m_listCtrlClient.SetColumnWidth(0, nWidth);
	m_listCtrlClient.SetColumnWidth(1, nWidth);
#pragma endregion

#pragma region socket init
	WSADATA wsaData;
	int err = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if (err != 0 ) 
	{               
		MessageBox(_T("Error: WSAStartupʧ��"));
		exit(0);
	}
	StartRunRecv();
#pragma  endregion

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileTransferServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileTransferServerDlg::OnPaint()
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
HCURSOR CFileTransferServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CFileTransferServerDlg::OnMyMsg(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case E_WIN_MSG_REFRESH_USER_LIST:
		{
			m_listCtrlClient.DeleteAllItems();
			for (int nIndex = m_vClientInfo.size() - 1; nIndex >= 0; nIndex--)
			{
				CString strNewClientSocket = _T("");
				strNewClientSocket.Format(_T("%d"),m_vClientInfo[nIndex].newClientSocket);
				CString strIndex = _T("");
				strIndex.Format(_T("%d"),nIndex + 1);
				int nNewItem = m_listCtrlClient.InsertItem(0, strIndex); 
				m_listCtrlClient.SetItemText(nNewItem, 1, strNewClientSocket);
			}

			
		}
		break;

	default:
		break;
	}

	return 0;
}




void CFileTransferServerDlg::StartRunRecv()
{
	DWORD dwThreadID = 0;
	CreateThread(NULL, 0, ThreadFuncRecv, this, 0, &dwThreadID);
}

DWORD WINAPI CFileTransferServerDlg::ThreadFuncRecv(LPVOID param)
{
	CFileTransferServerDlg *pThis = (CFileTransferServerDlg*)param;

	pThis->AddOneRecord(&pThis->m_richeditRecord, _T("��ʼ��socket"));

	pThis->m_recvSocket=socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET==pThis->m_recvSocket)
	{
		pThis->AddOneRecord(&pThis->m_richeditRecord, _T("ThreadFuncClientRecvFile ����socketʧ��"));
		WSACleanup();
		return -1;
	}

	
	pThis->m_remoteAddr.sin_family=AF_INET;
	pThis->m_remoteAddr.sin_port=htons(SERVER_LISTEN_PORT);
	pThis->m_remoteAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	pThis->m_nRemoteLen = sizeof(pThis->m_remoteAddr);

	int err=bind(pThis->m_recvSocket,(sockaddr*)&pThis->m_remoteAddr,pThis->m_nRemoteLen);
	if(SOCKET_ERROR==err)
	{
		pThis->AddOneRecord(&pThis->m_richeditRecord, _T("ThreadFuncRecv bind socketʧ��"));
		closesocket(pThis->m_recvSocket);
		WSACleanup();
		return -1;
	}

	err=listen(pThis->m_recvSocket, 50);

	pThis->AddOneRecord(&pThis->m_richeditRecord, _T("��ʼ�����ͻ��˵�����"));

	SOCKET newClientSocket = INVALID_SOCKET;
	while(1)
	{
		newClientSocket = accept(pThis->m_recvSocket, (struct sockaddr *)&pThis->m_remoteAddr, &pThis->m_nRemoteLen);
		if (newClientSocket == INVALID_SOCKET)
		{
			continue;
		}

		pThis->AddOneRecord(&pThis->m_richeditRecord, _T("���µĿͻ�������"));

		//����������ӵ�vector��
		t_ClientInfo tClientInfo;
		tClientInfo.newClientSocket = newClientSocket;
		pThis->m_vClientInfo.push_back(tClientInfo);

		//ˢ���û��б�
		::SendMessage(pThis->m_hWnd,WM_MY_MSG, E_WIN_MSG_REFRESH_USER_LIST,0);
		
		//���ӳɹ��������̴߳��������
		t_NewClientThreadParam *ptThreadParam = new t_NewClientThreadParam();
		ptThreadParam->newClientSocket = newClientSocket;
		ptThreadParam->pThis = param;

		DWORD dwThreadID = 0;
		CreateThread(NULL, 0, ThreadFuncRecvClientMsg, ptThreadParam, 0, &dwThreadID);
	}

	closesocket(pThis->m_recvSocket);
	WSACleanup();


	return 0;
}

DWORD WINAPI CFileTransferServerDlg::ThreadFuncRecvClientMsg(LPVOID param)
{
	t_MsgInfo tMsgInfo;
	t_NewClientThreadParam *ptThreadParam = (t_NewClientThreadParam*)param;
	CFileTransferServerDlg *pThis = (CFileTransferServerDlg*)ptThreadParam->pThis;
	while(1)
	{	
		//��һ������Ҫ�յ������ļ��������Լ��������а����˷��Ͷ����ɵĹ�Կ
		memset(&tMsgInfo, 0x00, sizeof(tMsgInfo));
		int nByte = recv(ptThreadParam->newClientSocket,(char*)&tMsgInfo,
			sizeof(tMsgInfo),0);
		if(nByte != sizeof(tMsgInfo))
		{
			continue;
		}

		switch(tMsgInfo.eMsgType)
		{
		case MSG_TYPE_NEW_FILE:
			{
				pThis->RecvMsgFileNew(&tMsgInfo, ptThreadParam->newClientSocket);
			}
			break;

		case MSG_TYPE_DATA_FILE:
			{
				pThis->RecvMsgFileData(&tMsgInfo, ptThreadParam->newClientSocket);
			}
			break;

		default:
			break;
		}
	}

	return 0;
}

void CFileTransferServerDlg::RecvMsgFileNew(t_MsgInfo *ptMsgInfo, SOCKET sock)
{

}


void CFileTransferServerDlg::RecvMsgFileData(t_MsgInfo *ptMsgInfo, SOCKET sock)
{
	//����sock������vector�б���ĸö���
	int nClientIndex = 0; 
	bool bFindClient = false;
	for (nClientIndex = 0; nClientIndex < m_vClientInfo.size(); nClientIndex++)
	{
		if(m_vClientInfo[nClientIndex].newClientSocket == sock)
		{
			bFindClient = true;
			break;
		}
	}
	if(!bFindClient)
	{
		AddOneRecord(&m_richeditRecord, _T("û���ҵ�����Ŀͻ��˱���"));
		return;
	}
	if(m_vClientInfo[nClientIndex].pWriteFileFp == NULL)
	{
		CString strMsg = _T("");
		strMsg.Format(_T("%s%s"), _T("�����ļ��ϴ����ļ���"),ptMsgInfo->szFileName);
		AddOneRecord(&m_richeditRecord, strMsg);

		CString strSaveDir = _T("");
		m_editSaveDir.GetWindowText(strSaveDir);
		char szPath[256] = {0};
		strcpy(szPath, CT2A(strSaveDir));
		strcat(szPath,"\\");
		strcat(szPath,ptMsgInfo->szFileName);

		/*char szTick[20] = {0};
		itoa(GetTickCount(),szTick,10);
		strcat(szPath,szTick);*/
		m_vClientInfo[nClientIndex].pWriteFileFp = fopen(szPath,"wb");
		if(!m_vClientInfo[nClientIndex].pWriteFileFp)
		{
			m_vClientInfo[nClientIndex].pWriteFileFp = NULL;
			return;
		}
	}
	fwrite(ptMsgInfo->szFileData,1,ptMsgInfo->nSendDataLen,m_vClientInfo[nClientIndex].pWriteFileFp);

	CString strMsg = _T("");
	float fSpeed = (100.0 * ptMsgInfo->lTotalSendDataLen)/(1.0 * ptMsgInfo->lFileDataLen);
	fSpeed = fSpeed > 100 ? 100 : fSpeed;
	strMsg.Format(_T("%s%s%s%f%s"), _T("���ڽ����ļ���"),ptMsgInfo->szFileName, _T("����  ���ȣ�  "), fSpeed, _T("\%"));
	AddOneRecord(&m_richeditRecord, strMsg);

	if(ptMsgInfo->lTotalSendDataLen >= ptMsgInfo->lFileDataLen)
	{
		//���ͽ���
		fclose(m_vClientInfo[nClientIndex].pWriteFileFp);
		m_vClientInfo[nClientIndex].pWriteFileFp = NULL;

		CString strMsg = _T("");
		strMsg.Format(_T("%s%s%s"), _T("�ļ���"),ptMsgInfo->szFileName, _T("���������"));
		AddOneRecord(&m_richeditRecord, strMsg);
	}

	return;
}



#pragma region ���һ�м�¼��richedit�ؼ���
void CFileTransferServerDlg::AddOneRecord(CRichEditCtrl *pRicheditCtr, CString strMsg)
{
	pRicheditCtr->SetSel(-1, -1);
	pRicheditCtr->ReplaceSel(strMsg + "\r\n");
	pRicheditCtr->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CFileTransferServerDlg::CString2Char(CString str, char ch[])
{
	//int i;
	//char *tmpch;
	//int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//�õ�Char�ĳ���
	//tmpch = new char[wLen + 1];                                             //��������ĵ�ַ��С
	//WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //��CStringת����char*

	//for(i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	//ch[i] = '\0';
}

#pragma endregion


#pragma region ѡ�񱣴��ļ���Ŀ¼
void CFileTransferServerDlg::OnBnClickedButtonSelectDir()
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
#pragma endregion

#pragma region ��Ŀ¼
void CFileTransferServerDlg::OnBnClickedButtonOpenDir()
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


void CFileTransferServerDlg::OnBnClickedButtonSendFile()
{
	m_vSendFileClient.clear();
	for (int nIndex = 0; nIndex < m_listCtrlClient.GetItemCount(); nIndex++)
	{
		if( m_listCtrlClient.GetItemState(nIndex, LVIS_SELECTED) == LVIS_SELECTED 
			|| m_listCtrlClient.GetCheck(nIndex))
		{
			m_vSendFileClient.push_back(m_listCtrlClient.GetItemText(nIndex,1));
		}
	}
	if(m_vSendFileClient.size()<=0)
	{
		MessageBox(_T("����ѡ��Ҫ�����ļ��Ŀͻ���"));
		return;
	}

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

	DWORD dwThreadID = 0;
	CreateThread(NULL, 0, ThreadFuncSendFile, this, 0, &dwThreadID);
}

DWORD WINAPI CFileTransferServerDlg::ThreadFuncSendFile(LPVOID param)
{
	CFileTransferServerDlg *pThis = (CFileTransferServerDlg*)param;

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
			for (int nIndex = 0; nIndex < pThis->m_vSendFileClient.size(); nIndex++)
			{
				int nSocket = atoi(pThis->m_vSendFileClient[nIndex]);
				send((SOCKET)nSocket,(char*)&tMsgInfo,sizeof(t_MsgInfo),0);
			}

			
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


void CFileTransferServerDlg::OnLvnItemchangedListClientList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}



//void CFileTransferServerDlg::OnBnClickedAllcheck()
//{
//	m_listCtrlClient.SetItemState(-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
//}

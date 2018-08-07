
// FileTransferServerDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include "../../Common/Common.h"
#include "afxwin.h"
#include "afxcmn.h"

using namespace std;

#pragma region �Զ�����Ϣ
#define WM_MY_MSG (WM_APP + 1000)

typedef enum e_WindowsMsgType
{
	E_WIN_MSG_REFRESH_USER_LIST,
	E_WIN_MSG_END
};
#pragma endregion

typedef struct t_NewClientThreadParam
{
	SOCKET newClientSocket;
	void *pThis;
	t_NewClientThreadParam()
	{
		newClientSocket = INVALID_SOCKET;
		pThis = NULL;
	}
};

typedef struct t_ClientInfo
{
	SOCKET newClientSocket;
	FILE *pWriteFileFp;
	t_ClientInfo()
	{
		newClientSocket = INVALID_SOCKET;
		pWriteFileFp = NULL;
	}
};




// CFileTransferServerDlg �Ի���
class CFileTransferServerDlg : public CDialogEx
{
// ����
public:
	CFileTransferServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILETRANSFERSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	afx_msg LRESULT OnMyMsg(WPARAM, LPARAM);
	void StartRunRecv();
	static DWORD WINAPI ThreadFuncRecv(LPVOID param);
	static DWORD WINAPI ThreadFuncRecvClientMsg(LPVOID param);
	static DWORD WINAPI ThreadFuncSendFile(LPVOID param);

	void AddOneRecord(CRichEditCtrl *pRicheditCtr, CString strMsg);
	void CString2Char(CString str, char ch[]);

	void RecvMsgFileData(t_MsgInfo *ptMsgInfo, SOCKET sock);
	void RecvMsgFileNew(t_MsgInfo *ptMsgInfo, SOCKET sock);

private:
	SOCKET m_recvSocket;
	sockaddr_in m_remoteAddr;
	int m_nRemoteLen;

	CString m_strSendFilePath;
	CString m_strFileName;

	vector<t_ClientInfo> m_vClientInfo;
	vector<CString> m_vSendFileClient;

public:
	CRichEditCtrl m_richeditRecord;
	afx_msg void OnBnClickedButtonSelectDir();
	CEdit m_editSaveDir;
	afx_msg void OnBnClickedButtonOpenDir();
	CListCtrl m_listCtrlClient;
	afx_msg void OnBnClickedButtonSendFile();
	CButton m_btnSendFile;
	afx_msg void OnEnChangeEditSaveDir();
	afx_msg void OnLvnItemchangedListClientList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedAllcheck();
};

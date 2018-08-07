
// FileTransferClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../../Common/Common.h"

// CFileTransferClientDlg �Ի���
class CFileTransferClientDlg : public CDialogEx
{
// ����
public:
	CFileTransferClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILETRANSFERCLIENT_DIALOG };

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
	static DWORD WINAPI ThreadFuncRecvMsg(LPVOID param);
	static DWORD WINAPI ThreadFuncSendFile(LPVOID param);
	void RecvMsgFileData(t_MsgInfo *ptMsgInfo, SOCKET sock);
	void AddOneRecord(CRichEditCtrl *pRicheditCtr, CString strMsg);

private:
	SOCKET m_socketClient;
	CString m_strSendFilePath;
	CString m_strFileName;
	sockaddr_in m_remoteAddr;
	FILE *m_pWriteFileFp;
public:
	afx_msg void OnBnClickedButtonSendFile();
	CEdit m_editSaveDir;
	CRichEditCtrl m_richeditRecord;
	afx_msg void OnBnClickedButtonSelectDir();
	afx_msg void OnBnClickedButtonOpenDir();
	CButton m_btnSendFile;
	afx_msg void OnEnChangeRicheditRecord();
	afx_msg void OnEnChangeEditSaveDir();
};

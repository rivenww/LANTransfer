
// FileTransferClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../../Common/Common.h"

// CFileTransferClientDlg 对话框
class CFileTransferClientDlg : public CDialogEx
{
// 构造
public:
	CFileTransferClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FILETRANSFERCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

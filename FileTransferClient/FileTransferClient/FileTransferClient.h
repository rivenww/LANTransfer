
// FileTransferClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileTransferClientApp:
// �йش����ʵ�֣������ FileTransferClient.cpp
//

class CFileTransferClientApp : public CWinApp
{
public:
	CFileTransferClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileTransferClientApp theApp;
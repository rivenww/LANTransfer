
// FileTransferServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileTransferServerApp:
// �йش����ʵ�֣������ FileTransferServer.cpp
//

class CFileTransferServerApp : public CWinApp
{
public:
	CFileTransferServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileTransferServerApp theApp;
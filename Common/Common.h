#pragma once
#include <iostream>
#include <string>
using namespace std;


#define SERVER_LISTEN_PORT  9999	//����˼����˿�


//����֮�䷢����Ϣʱ����������
typedef enum
{
	MSG_TYPE_NEW_FILE,		//��ʼ�����ļ�	
	MSG_TYPE_DATA_FILE,		//�����ļ�����	
	MSG_TYPE_END_FILE,		//�����ļ�����
	MSG_TYPE_END
}e_MsgType;


#define RECV_FILE_SIZE_ONE_TIME 102400

typedef struct t_MsgInfo
{
	e_MsgType	eMsgType;		//��������
	char szFileName[256];
	char szFileData[RECV_FILE_SIZE_ONE_TIME];
	int nSendDataLen;			//�˴η��͵��ļ����ݳ���
	long lTotalSendDataLen;		//��ǰ�ܹ��Ѿ����͵����ݳ��ȣ�������ǰ��һ��
	long lFileDataLen;			//��ǰ���͵��ļ����ܴ�С


	t_MsgInfo()
	{
		eMsgType = MSG_TYPE_END;
		memset(szFileName, 0x00, sizeof(szFileName));
		memset(szFileData, 0x00, sizeof(szFileData));
		lFileDataLen = 0;
		lTotalSendDataLen = 0;
		lFileDataLen = 0;
	}
};


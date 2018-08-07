#pragma once
#include <iostream>
#include <string>
using namespace std;


#define SERVER_LISTEN_PORT  9999	//服务端监听端口


//三者之间发送消息时的请求类型
typedef enum
{
	MSG_TYPE_NEW_FILE,		//开始发送文件	
	MSG_TYPE_DATA_FILE,		//发送文件数据	
	MSG_TYPE_END_FILE,		//发送文件结束
	MSG_TYPE_END
}e_MsgType;


#define RECV_FILE_SIZE_ONE_TIME 102400

typedef struct t_MsgInfo
{
	e_MsgType	eMsgType;		//请求类型
	char szFileName[256];
	char szFileData[RECV_FILE_SIZE_ONE_TIME];
	int nSendDataLen;			//此次发送的文件数据长度
	long lTotalSendDataLen;		//当前总共已经发送的数据长度，包括当前这一次
	long lFileDataLen;			//当前发送的文件的总大小


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


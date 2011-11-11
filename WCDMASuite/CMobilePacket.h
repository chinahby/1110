#pragma once

enum EATReturnCode
{
	RETURN_OK = 1,		//返回命令应答OK
	RETURN_TIMEOUT,		//在指定时间内命令应答没有返回
	RETURN_BUSY,		//前一个命令正在执行，还未完成
	RETURN_ERROR = 256,	//返回命令应答Error
	RETURN_ERROR_REPORT,//返回命令应答Error,且带有错误码,错误码为ErrorCode,则命令返回值为RETURN_ERROR_REPORT+ErrorCode
};


class CMobilePacket
{
	int m_nReturn;	//命令返回码或错误代码
	int m_nLen;		//消息长度(字节数)
	char m_szMessage[1024];//消息内容
public: 
	CMobilePacket(const char* strCmd, size_t strSize);
	virtual ~CMobilePacket();

	virtual void SetReturnCode(int code);

	int ReturnCode()const{return m_nReturn;}

	int MessageSize()const{return m_nLen;}
	
	const char* Message()const{return m_szMessage;}
protected:
	CMobilePacket();
	void SetMessage(const char* strCmd, size_t strSize);
};


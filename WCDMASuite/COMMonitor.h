#ifndef AFX_COMMONITOR_H__8BD28A9B_01E8_4A44_9E02_550987167D91__INCLUDED_
#define AFX_COMMONITOR_H__8BD28A9B_01E8_4A44_9E02_550987167D91__INCLUDED_

#include "MobileATProto.h"
#include "SerialPort.h"

//#define USE_SIMULATOR


#define SERIAL_PORT CSerialPort


class CMobileManager;
class CCOMRecvThread;
class CCOMSendThread;
class CCOMMonitor  
{
public:
	CCOMMonitor(CMobileManager* pPortOwner);
	~CCOMMonitor();

	//启动COM口发送/接收线程
	bool StartMonitoring(const CString& COMPort, UINT baud);

	//停止COM口发送/接收线程
	void StopMonitoring();

	//判断COM口发送/接收线程是否启动
	bool IsThreadActive();

	//设置当前待发送的AT命令
	void PushCmd(CATCmd& at);

	//设置当前AT命令的返回码
	void SetReturnCode(int errorCode);

	//查询信号强度命令特殊处理
	void QuerySignal();

	//上网时要清空队列
	void ClearCmdQ();

	bool m_commonitorstate;			// add by lab686	2009/10/28  17:39:00
private:
	bool InitSerial(const CString& COMPort, UINT baud);
	void StartThreads();
private:
	
	CEvent m_EventExitThead;//线程退出事件
	SERIAL_PORT		m_COM;	//串口对象

private:
	CCOMSendThread* m_pSendThread;
	CCOMRecvThread* m_pRecvThread;
};

#endif

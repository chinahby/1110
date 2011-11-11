#ifndef AFX_MOBILEATPROTO_H__5C1DED7A_16CB_4916_8D36_D736B04B32EE__INCLUDED_
#define AFX_MOBILEATPROTO_H__5C1DED7A_16CB_4916_8D36_D736B04B32EE__INCLUDED_

#include <afxmt.h>
//#include "GLobalConst.h"
#include "CMobilePacket.h"
#include "CriticalSecMon.h"

class CCallManager;
class CMobileATProto  
{
public:
	enum
	{ 
		SOUND_TYPE_DIAL = 0, 
		SOUND_TYPE_DATA = 1, 
		SOUND_TYPE_FAX = 3, 
		SOUND_TYPE_SMS = 4 
	};
private:
	static CMobilePacket* GeneratePacket(const char* strCmd, size_t strSize);
public:
	///模拟键盘控制
	static CMobilePacket* KeyInput(char cKey);

	///得到音量设置
	static CMobilePacket* GetVolumn();

	///查询SIM卡是否存在
	static CMobilePacket* CheckSimExist();
	
	///设置回显
	static CMobilePacket* SetEcho(bool bOn);	// ok

	///测试SIM卡锁定
	static CMobilePacket* TestSimLock();		// ok
	///测试SIM卡锁定
	static CMobilePacket* SETSimLock(int mode, const CString& strPinCode);		// ok
	//设置ME错误报告
	static CMobilePacket* SetCmee();

	//设置网络登记状态报告
	static CMobilePacket* SetCreg();

	//查询网络登记状态
	static CMobilePacket* QueryCreg();

	//查询GPRS附着
	static CMobilePacket*  QueryGprsAttach();

	//查询EDGE附着
	static CMobilePacket*  QueryEDGEAttach();
	
	///设置音量 
	static CMobilePacket* SetVolumn(int nVolumn);

	//
	static CMobilePacket* RegisterNet(const CString& strNetName);
	static CMobilePacket* QueryCurNet();
	static CMobilePacket* QueryAllNet();
	///查询本机电话
//	static CMobilePacket* LocalPhoneNum();

	///设置本机电话
//	static CMobilePacket* SetLocalPhone(const CString& strLocalPhone);

														//////////SMS//////////
	//设置短信格式为text格式
	static CMobilePacket* CMobileATProto::SetCMGF();			// ok set 1

	//设置字符集
	static CMobilePacket* CMobileATProto::SetCharset();

	///查询短消息服务中心
	static CMobilePacket* CscaPhoneNum();

	static CMobilePacket* SMSSave(const CString& strPhoneNum, const CString& strContent);		// modify 

	static CMobilePacket* SMSRead(int nID);					// ok

	static CMobilePacket* SMSDelete(int nID);					// ok
	static CMobilePacket* SMSDeleteAll();						// ok

	static CMobilePacket* SMSBoxSelect(const CString& strType);// ok

	static CMobilePacket* SetSMSPrompt();					// ok

	static CMobilePacket* SetSMSPraCmd(int fmt);					// ok
														//////////SMS//////////

	///查询版本
	static CMobilePacket* LocalVersion();
	
	static CMobilePacket* CheckCPIN();

	static CMobilePacket* SetMode(int mode);		// add by lab686

	static CMobilePacket* QueryMode();		// add by lab686
	
	static CMobilePacket* CheckSignal();

	static CMobilePacket* CheckHDRSignal();						// ok lab686

	static CMobilePacket* CheckSYSINFO();						// ok lab686
	
	static CMobilePacket* QueryBaud();
	
	static CMobilePacket* QueryIMEI();

	static CMobilePacket* CheckMIC();

	static CMobilePacket* SetMICMute(bool bSet);
	
	static CMobilePacket* SoundPlay(int nType, int nVolumn, int nSound);

	static CMobilePacket* SetBaudrate(int iRate);

	static CMobilePacket* GetBaudrate();

	static CMobilePacket* SetDialShow(bool bShow);
	
	static CMobilePacket* SetPhoneBookEnable();
	
	static CMobilePacket* DialHangUp();

	static CMobilePacket* DialCallOut(const CString& strPhoneNum, CCallManager* pCallMgr);
	
	static CMobilePacket* DialAccept(CCallManager* pCallMgr);

	static CMobilePacket* SendDtmf(const CString& strKey, size_t callnumber);

	static CMobilePacket* SendDDSETEX(size_t comnumber);	// add by lab686 20091030

	static CMobilePacket* SetCVOICE(size_t comnumber);			// add by lab686 20091103 // OK
	static CMobilePacket* PhoneBookType();

	//static CMobilePacket* PhoneBookParam();

	static CMobilePacket* PhoneBookLoad(int nMinID, int nMaxID);

	static CMobilePacket* PhoneBookDelete(int nID);

	static CMobilePacket* PhoneBookSelect(const CString& strType);

	static CMobilePacket* PhoneBookModify(int nID, const CString& strPhoneNum, const CString& strName);

	static CMobilePacket* CheckRUIMSurport();

	static CMobilePacket* ReadAOLock();

	static CMobilePacket* SetAOLock(bool bLock);

	static CMobilePacket* ReadAILock();

	static CMobilePacket* SetAILock(bool bLock);

	static CMobilePacket* SelectRing(BYTE nMusicIndex, BYTE deviceNo);

	static CMobilePacket* InputPIN1(const CString& strPIN);

	static CMobilePacket* InputPUK1(const CString& strPUK, const CString& strNewPIN);

	static CMobilePacket* QueryLockTag();

	static CMobilePacket* SendFlash(const char* strNum = NULL);

	static CMobilePacket* SendPDUSMS(const CString& SMSCenter, const CString& strPhoneNum, const CString& strText, CMobilePacket*& cmd2);

	static CMobilePacket* CallHold(int code);
	static CMobilePacket* CallForwardSet(int reason, int mode, const char* strNum);
	static CMobilePacket* CallForwardCancel(int reason, int mode);
	static CMobilePacket* CallForwardQuery(int reason, int mode);
	static CMobilePacket* QueryCallEnd();

	static CMobilePacket* TestComATOK();

	static CMobilePacket* QuerySWRV();
	static CMobilePacket* QueryIDStatus();
	static CMobilePacket* QueryID();
	static CMobilePacket* QueryMN();
	static CMobilePacket* QueryState();

	static CMobilePacket* SetDevToUDisk();

	static CMobilePacket* QueryPPP();

	static CMobilePacket* SetPCMSWitch(int nMode);
	static CMobilePacket* SetApn(const CString& strApn);
	static CMobilePacket* RSPDevice();
};

/**
*	AT command 类.
*	处理AT 命令的同步问题。内部使用引用计数
*	来保持对一个AT命令对象的追踪。
*	该类的实例是一个轻量级对象，所以可以简单
*	地按值(By value)传送
*/
static CCriticalSection g_CSATCmd;
class CATCmd
{
public:
	//构造函数
	explicit CATCmd(CMobilePacket* pPacket = NULL)
	{
		InitializeCriticalSection(&m_CS);
		m_pObj = new TATPacket(pPacket);
	}

	//拷贝构造
	CATCmd(const CATCmd& atcmd)
	{
		InitializeCriticalSection(&m_CS);
		atcmd.m_pObj->AddRef();
		m_pObj = atcmd.m_pObj;
	}

	//operator=
	CATCmd& operator=(const CATCmd& atcmd)
	{
		//CCriticalMonitor lock(m_CS);
		g_CSATCmd.Lock();

		if(&atcmd != this)
		{
			m_pObj->Release();
			atcmd.m_pObj->AddRef();
			m_pObj = atcmd.m_pObj;
		}
		g_CSATCmd.Unlock();

		return *this;
	}

	//析构
	~CATCmd()
	{
		m_pObj->Release();
		m_pObj = NULL;
		DeleteCriticalSection(&m_CS);
	}

	//访问内部的Packet对象
	CMobilePacket* GetPacket()
	{
		return m_pObj->m_pPacket;
	}

	CMobilePacket* GetPacket()const//const 版本
	{
		return m_pObj->m_pPacket;
	}

	//等待命令的完成事件
	DWORD WaitEvent(DWORD dwMillisec)
	{
		return ::WaitForSingleObject(m_pObj->m_Event, dwMillisec);
	}

	//触发命令的完成事件
	void SetEvent()const
	{
		//CCriticalMonitor lock(m_CS);
		g_CSATCmd.Lock();
		m_pObj->m_Event.SetEvent();
		g_CSATCmd.Unlock();
	}

	//清除命令的完成事件
	void ResetEvent()
	{
		//CCriticalMonitor lock(m_CS);
		g_CSATCmd.Lock();
		m_pObj->m_Event.ResetEvent();
		g_CSATCmd.Unlock();
	}

	//设置命令的完成码并触发完成事件
	void SetErrorCode(int code)const
	{
		//CCriticalMonitor lock(m_CS);
		g_CSATCmd.Lock();
		if(m_pObj->m_pPacket)
			m_pObj->m_pPacket->SetReturnCode(code);
		g_CSATCmd.Unlock();
	}
private:
	//内部的Packet包装类
	struct TATPacket
	{
		CEvent m_Event;	//命令完成事件
		long   m_nRef;	//引用计数

		CMobilePacket* m_pPacket;//Packet对象，必须是用过new操作符产生的,或者为NULL
	public:
		//构造
		TATPacket(CMobilePacket* pPacket = NULL)
			:m_Event(FALSE, TRUE),m_nRef(1),m_pPacket(pPacket)
		{
		}

		//增加应用计数
		void AddRef()
		{
			::InterlockedIncrement(&m_nRef);
		}

		//减少应用计数
		void Release()
		{
			::InterlockedDecrement(&m_nRef);
			if(! m_nRef)
			{
				delete this;
			}
		}
	private://禁止访问
		TATPacket(const TATPacket&);
		TATPacket& operator=(const TATPacket&);
	private:
		~TATPacket()
		{
			delete m_pPacket;//释放packet对象
			m_pPacket = NULL;
		}
	};

private:
	TATPacket* m_pObj;//内部的Packet包装类对象指针，此指针始终有效
	mutable CRITICAL_SECTION m_CS;
};

#endif

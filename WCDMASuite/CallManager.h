// CallManager.h: interface for the CCallManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLMANAGER_H__7E9C04C0_69FD_47D7_8CC1_70903529DA20__INCLUDED_)
#define AFX_CALLMANAGER_H__7E9C04C0_69FD_47D7_8CC1_70903529DA20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordManager.h"
#include "mobileatproto.h"
#include "mobilemanager.h"
#include <afxmt.h>
#include "data\CallRecord.h"


//呼叫状态
enum CallStatus
{
	CALLSTATE_IDLE,			//空闲状态
	CALLSTATE_INCOMING,		//来电振玲状态
	CALLSTATE_IN_TALKING,	//呼入接听状态
	CALLSTATE_OUT_TALKING,	//呼出接听状态
	CALLSTATE_OUTGONING,	//正在呼出状态
};

//呼叫状态监听接口
struct ICallStatusListener
{
	virtual void OnCallEnd(CCallManager* pSender) = 0;
	virtual void OnCallComing(CCallManager* pSender) = 0;
	virtual void OnCallGoing(CCallManager* pSender) = 0;
	virtual void OnTalking(CCallManager* pSender) = 0;
	virtual void OnCallForward(const CString& strForwardNum) = 0;
	virtual void OnFlashCallConnected() = 0;	
	virtual void OnCallWaiting(const CString& strNum) = 0;	
};

class CMobileManager;
class CCallManager  
{
public:	
	CMobileManager* m_pMgr;
	ICallStatusListener* m_pListener;
	//呼叫
	void Call(const CString& strNumber);
	//挂断
	void EndCall(bool selfHangup = false);
	//接听电话
	void AcceptIncomingCall();
	void OnCallWating(const CString& strNum);
	void SetListener(ICallStatusListener* pListener);
	//二次拨号
	void SendDTMF(const CString& str);
	const CString& GetDTMFString()const;
	//设置呼叫的状态
	void SetCallStatus(CallStatus status);
	//获取呼叫的状态
	CallStatus GetCallStatus(void);
	//呼叫记时
	void CallTime(void);
	void SetMobileManager(CMobileManager* pMgr);
	void OnCallEnd(int nReason, bool selfHangup = false);		//响应+CCEND
	void OnCallContinue();
	void OnOutgoingCall();	//响应+ZCORG
	void OnIncomingCall(const CString& strPhone);	//响应+CLIP
	//设置呼叫的状态
	CallStatus  m_setCallStatus;
	CString		m_strDTMF;		//本次呼出电话的DTMF串
//	CMobileATProto* m_mobileAT;
	CCallManager();
	virtual ~CCallManager();

	const TCallInfo& GetCallInfo()const;
	void RecordCallInfo(TCallInfo* callInfo);

	DWORD GetCurCallTime()const;

	int m_callID;				// add by lab686 2009/10/29  used annal the  uniquely number of the calling value between 1 to 6

	void SetVoiceCom();	// add by lab686 20091030 	set AT^DDSETEX=2

private:
	TCallInfo	m_CallInfo;		//当前呼叫信息

	BOOL	m_bRing;		//控制振铃一次
	BOOL	m_bIncoming;	//控制来电显示一次
	bool    flagSelfHangup;

};

#endif // !defined(AFX_CALLMANAGER_H__7E9C04C0_69FD_47D7_8CC1_70903529DA20__INCLUDED_)

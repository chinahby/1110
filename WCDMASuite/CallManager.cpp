// CallManager.cpp: implementation of the CCallManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MMSYSTEM.h"
#include "CallManager.h"
#include "GlobalDataCenter.h"
#include "mobileatproto.h"
#include "Resource.h"
#include "SunnyCatDlg.h"
#include "CDMATime.h"
#include "Util.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallManager::CCallManager()
:m_pMgr(NULL)
{	
	m_setCallStatus = CALLSTATE_IDLE;

	m_bRing = FALSE;
	m_bIncoming = FALSE;
	flagSelfHangup = false;

	m_callID = 1;				// add by lab686 2009/10/29
//	m_pMgr->m_callnum = 1;
}

CCallManager::~CCallManager()
{

}

void CCallManager::SetMobileManager(CMobileManager* pMgr)
{
	m_pMgr = pMgr;
}

void CCallManager::Call(const CString &strNumber)
{		
	if(m_setCallStatus == CALLSTATE_IDLE && m_pMgr != NULL)
	{
		m_strDTMF = "";
		m_setCallStatus = CALLSTATE_OUTGONING;

		m_CallInfo.strTelephone = strNumber;
		m_CallInfo.eRecordType = CALLRECORD_DAIL;
		m_CallInfo.type = CALL_OUTGOING;
		m_CallInfo.begin = CCDMATime::Instance().GetCDMATime();
		CString strName;
		int nID;
		((CSunnyCatDlg*)AfxGetMainWnd())->FindNameInPhonebook(strNumber,strName,nID);
		m_CallInfo.strName = strName;

		SetVoiceCom();	// 设置语音通讯端口add by lab686 20091030
		
		CATCmd cmd = m_pMgr->SendATPacket(CMobileATProto::SetVolumn(3));
		m_pMgr->WaitForATCompleted(cmd,1000);

		Sleep(1000);

		TRACE(TEXT("CCallManager::Cal:> SetVoiceCom()"));
		cmd = m_pMgr->SendATPacket(CMobileATProto::DialCallOut(strNumber, this));
		//bool bOK = RETURN_OK == m_pMgr->WaitForATCompleted(cmd);
		//if(bOK)
		//	m_setCallStatus = CALLSTATE_OUT_TALKING;
	}
	
}

void CCallManager::EndCall(bool selfHangup)
{
	TRACE(TEXT("CCallManager::EndCall:> "));
	if(selfHangup)
	{
		flagSelfHangup = true;
	}
	//m_pMgr->SendATPacket(CMobileATProto::DialHangUp());
	m_pMgr->WaitForATCompleted(m_pMgr->SendATPacket(CMobileATProto::DialHangUp()),1000);
	OnCallEnd(0, selfHangup);

	/*
	m_setCallStatus = CALLSTATE_IDLE;

	m_bRing = FALSE;
	m_bIncoming = FALSE;

	PlaySound(NULL, NULL, SND_ASYNC|SND_NODEFAULT );
	if(m_CallInfo.period != 0)
	{
		m_CallInfo.period = (GetTickCount()-m_CallInfo.period)/1000; //转换为秒
		if(m_CallInfo.type == CALL_INCOMING)
			m_CallInfo.eRecordType = CALLRECORD_RECV;
		else if(m_CallInfo.type == CALL_OUTGOING)
			m_CallInfo.eRecordType = CALLRECORD_DAIL;
	}
	if(m_CallInfo.period == 0)
	{
		if(m_CallInfo.type == CALL_INCOMING)
			m_CallInfo.eRecordType = CALLRECORD_MISS;
	}

	RecordCallInfo(&m_CallInfo);
*/
}

void CCallManager::SetCallStatus(CallStatus status)
{
	m_setCallStatus = status;
}

CallStatus CCallManager::GetCallStatus()
{
	return m_setCallStatus;
}

void CCallManager::CallTime()
{

}

void CCallManager::RecordCallInfo(TCallInfo* callInfo)
{
	if(!callInfo)return;

	CCallRecord call;
	//m_pMgr->m_pOwner->m_recordDlg.m_callRecord.Add(* callInfo);
	call.Add(* callInfo);
}

void CCallManager::OnCallEnd(int nReason, bool selfHangup)		//响应+CCEND
{
	m_pMgr->m_pOwner->KillTimer(TIMER_TALKING);
//	m_pMgr->m_pOwner->KillTimer(TIMER_CALL_STATUS);
	m_pMgr->m_pOwner->KillTimer(TIMER_CALL_RING);
	
	m_pMgr->m_pOwner->m_bSetQueryRingTimer = false;

	m_pMgr->m_pOwner->StopRecord();
	m_pMgr->m_pOwner->StopVoice();

	PlaySound(NULL, NULL, SND_ASYNC|SND_NODEFAULT );
	
	
	if((!selfHangup)&&(!flagSelfHangup))
	{
		//((CSunnyCatDlg *)AfxGetMainWnd())->m_callDlg.ShowCallInImag(0);
		m_pMgr->m_pOwner->PostMessage(WM_SHOW_CALLIMAG,0,0);
	}
	else
	{
		if(flagSelfHangup)
		{
			flagSelfHangup = false;
		}
	}
	
	

	if(m_setCallStatus == CALLSTATE_IDLE)
		return;
	m_bRing = FALSE;
	m_bIncoming = FALSE;
	m_pMgr->m_callnum = 1;			// add by lab686 	20091029

	if(m_CallInfo.period != 0)
	{
		m_CallInfo.period = (GetTickCount()-m_CallInfo.period)/1000; //转换为秒
		if(m_CallInfo.type == CALL_INCOMING)
			m_CallInfo.eRecordType = CALLRECORD_RECV;
		else if(m_CallInfo.type == CALL_OUTGOING)
			m_CallInfo.eRecordType = CALLRECORD_DAIL;
	}
	if(m_CallInfo.period == 0)
	{
		if(m_CallInfo.type == CALL_INCOMING)
			m_CallInfo.eRecordType = CALLRECORD_MISS;
		if(m_CallInfo.type == CALL_OUTGOING)
		{
			CString strMsg = LoadStringEx(IDS_CALL_FAILED);
			//m_pMgr->m_pOwner->m_ctrlInfoBar.SetWindowText(strMsg);
			m_pMgr->PostMessageToClient(WM_REFRESH_FAILINFOBAR,0,0);
			Sleep(1000);
		}
	}

	m_setCallStatus = CALLSTATE_IDLE;
	//m_pMgr->m_pOwner->FlashInfoBar("");
	m_pMgr->PostMessageToClient(WM_REFRESH_INFOBAR,0,0);
	RecordCallInfo(&m_CallInfo);

//	m_pMgr->m_pOwner->PostMessage(WM_REFRESH_CALLRECORD,m_CallInfo.eRecordType,0);

	m_CallInfo.strTelephone = "";//清除本次呼叫的信息
	m_CallInfo.period = 0;
	
	
//	if((m_pMgr->m_pOwner->m_recordDlg.currItemData == CALL_RECORD
//		&& m_CallInfo.eRecordType == CALLRECORD_RECV)||
//		(m_pMgr->m_pOwner->m_recordDlg.currItemData == CALL_RECORD_RECV
//		&& m_CallInfo.eRecordType == CALLRECORD_RECV) ||
//		(m_pMgr->m_pOwner->m_recordDlg.currItemData == CALL_RECORD_OUTCALL
//		&& m_CallInfo.eRecordType == CALLRECORD_DAIL) ||
//		(m_pMgr->m_pOwner->m_recordDlg.currItemData == CALL_RECORD_NOTGET
// 		&& m_CallInfo.eRecordType == CALLRECORD_MISS))
// 	{

	
	//m_pMgr->m_pOwner->m_recordDlg.m_ctrlRecordList.DeleteAllCol();
	//m_pMgr->m_pOwner->m_recordDlg.LoadListHead(CALLRECORD);
	//m_pMgr->m_pOwner->m_recordDlg.ShowList(m_CallInfo.eRecordType);
// 	}


/*	{
		CCriticalMonitor FlashDoor(m_CSFlash);
		if(m_pFlashCallInfo)
		{
			OnSecondCallEnd();
			return;
		}
	}

	if(	m_setCallStatus == CALLSTATE_OUTGONING || 
		m_setCallStatus == CALLSTATE_OUT_TALKING)
	{
		m_RecentNumber = m_CallInfo.number;
		m_RecentName = m_CallInfo.name;
	}

	
	if(m_CallInfo.period)
	{
		m_CallInfo.period = (GetTickCount()-m_CallInfo.period)/1000; //转换为秒
	}

	RecordCallInfo(&m_CallInfo);

	m_setCallStatus = CALLSTATE_IDLE;
	if(m_pListener)
	{
		m_pListener->OnCallEnd(this);
	}

	m_CallInfo.number = "";//清除本次呼叫的信息
	m_CallInfo.period = 0;

	m_strDTMF = "";

	if(!m_CallInfo.IsMissingCall())
	{
		m_LastCall = m_CallInfo;
	}
	*/
}

void CCallManager::OnCallContinue()	//响应^CONN
{
	if(	m_setCallStatus != CALLSTATE_INCOMING && 
		m_setCallStatus != CALLSTATE_OUTGONING)
		return;

	SetVoiceCom();
	
	m_pMgr->m_pOwner->OnTalking(this);
	
	PlaySound(NULL, NULL, SND_ASYNC|SND_NODEFAULT );
	
	//似乎AT的指示次序有问题，会出现重复的时间纪录,
	//所以在这里再次纪录通话开始时间
	m_CallInfo.begin = CCDMATime::Instance().GetCDMATime();

	m_setCallStatus = (m_setCallStatus==CALLSTATE_INCOMING)? CALLSTATE_IN_TALKING: CALLSTATE_OUT_TALKING;
	
	//m_pMgr->m_pOwner->FlashInfoBar(m_CallInfo.strTelephone);
	m_pMgr->PostMessageToClient(WM_REFRSH_TELINFOBAR,0,0);
	
	m_CallInfo.period = GetTickCount();//纪录开始通话时间
	
}

void CCallManager::OnOutgoingCall()	//响应+ZCORG
{
	if(m_setCallStatus != CALLSTATE_OUTGONING)
		return;

//	m_CallInfo.begin = CCDMATime::Instance().GetCDMATime();
//	m_CallInfo.period = 0;
}


void CCallManager::OnIncomingCall(const CString& strPhone)	//响应+CLIP, RING
{
	//呼叫等待
	if(	m_setCallStatus == CALLSTATE_IN_TALKING || m_setCallStatus == CALLSTATE_OUT_TALKING)
	{
		OnCallWating(strPhone);
		return;
	}

	if(m_setCallStatus != CALLSTATE_IDLE && strPhone.IsEmpty())
		return;

	
	
	m_setCallStatus = CALLSTATE_INCOMING;
	m_CallInfo.strTelephone = strPhone;
	m_CallInfo.strName = "";
	m_CallInfo.period = 0;
	m_CallInfo.type = CALL_INCOMING;
	m_CallInfo.begin = CCDMATime::Instance().GetCDMATime();

	//((CSunnyCatDlg*)AfxGetMainWnd())->m_callDlg.PlayImagThread();
	m_pMgr->m_pOwner->m_callDlg.PlayImagThread();
	
	CString strName;
	int nID;
	//((CSunnyCatDlg*)AfxGetMainWnd())->FindNameInPhonebook(strPhone,strName,nID);
	m_pMgr->m_pOwner->FindNameInPhonebook(strPhone,strName,nID);
	m_CallInfo.strName = strName;


	if(!m_bRing || !m_bIncoming)
	{
		m_pMgr->m_pOwner->OnCallComing(this);
		if(strPhone.IsEmpty())
			m_bRing = TRUE;
		else
			m_bIncoming = TRUE;
	}

}

void CCallManager::SetListener(ICallStatusListener* pListener)
{
	m_pListener = pListener;
}

void CCallManager::AcceptIncomingCall()
{
	if(m_pMgr)
	{
		CATCmd cmd = m_pMgr->SendATPacket(CMobileATProto::DialAccept(this));
		bool bOK = RETURN_OK == m_pMgr->WaitForATCompleted(cmd,2000);
		if(bOK)
		{
//			m_pMgr->m_pOwner->StartVoice();			//接电话时打开端口
			m_setCallStatus = CALLSTATE_IN_TALKING;
		}
		
	}
	SetVoiceCom();
}

void CCallManager::OnCallWating(const CString& strNum)
{
/*	CCriticalMonitor FlashDoor(m_CSFlash);
	
	if(!m_pFlashCallInfo)
	{
		m_pFlashCallInfo = new TCallInfo;
	}
	
	m_pFlashCallInfo->number = strNum;
	m_pFlashCallInfo->type = CALL_INCOMING;

	if(m_pListener)
		m_pListener->OnCallWaiting(strNum);
		*/
}

//二次拨号
void CCallManager::SendDTMF(const CString& str)
{
	if(m_pMgr)
	{
		m_pMgr->SendATPacket(CMobileATProto::SendDtmf(str, m_pMgr->m_callnum));
		m_strDTMF += str;
		TRACE(TEXT("Read:%s\r\n"), m_strDTMF);
	}
}

void CCallManager::SetVoiceCom()		// add by lab686 20091030
{
	if(m_pMgr)
	{
//		CATCmd cmd;
//		cmd = m_pMgr->SendATPacket(CMobileATProto::SendDDSETEX(2));
//		m_pMgr->WaitForATCompleted(cmd,2000);
	}
}

const CString& CCallManager::GetDTMFString()const
{
	return m_strDTMF;
}

const TCallInfo& CCallManager::GetCallInfo()const
{
	return m_CallInfo;
}

DWORD CCallManager::GetCurCallTime()const
{
	if(m_setCallStatus != CALLSTATE_IDLE)
	{
		if(m_CallInfo.period)
			return (GetTickCount()-m_CallInfo.period)/1000; //转换为秒
		else
			return 0;
	}

	return m_CallInfo.period;
}

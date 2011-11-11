#include "stdafx.h"
#include "TheadSendSMS.h"
#include "MobileManager.h"
#include "CDMATime.h"
#include "data/SMSEmbed.h"
#include "SunnyCatDlg.h"
#include "SMSDialog.h"

CThreadSendSMS::CThreadSendSMS(	CMobileManager& mgr, 
					const vector<CString>& numbers, 
					const vector<CString>& sendStrings,
					int   priority,
					int   saveDays,
					EStringType type)
		:m_Mgr(mgr), 
		m_Numbers(numbers), 
		m_Contents(sendStrings),
		m_nPriority(priority),
		m_nSaveDays(saveDays),
		m_StringType(type)
{
//	m_bSendState = false;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail = 0;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc = 0;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_bsmssendsucc = false;
	m_iSendmaxrec = m_Numbers.size() > 20 ? 20 : m_Numbers.size();
	sendFail = m_iSendmaxrec;
}

void CThreadSendSMS::DoWork()//发送函数
{
	m_Mgr.m_bIsSendSMS = true;	// add by lab 200912081655
	for(size_t i=0; i< m_iSendmaxrec ; ++i) 	//m_Numbers.size(); ++i)//逐个发送
	{
		if(m_bCancel)
		{
			break;
		}

		if(!SendTo(i))
		{
//			ShowMessageBox(IDS_SMS_SEND_FAILED);
			GMessageBox(LoadStringEx(IDS_SMS_SEND_FAILED));
			break;
		}
	}

	::Sleep(2000);
//	((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail = (m_iSendmaxrec == (((((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc) + (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail))/2) ? (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail) : (sendFail * 2));
	if (m_iSendmaxrec != (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc) + (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail))
	{
//		(((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc) = (m_iSendmaxrec - sendFail);
//		(((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail) =  sendFail;
	}
	
	m_Mgr.m_pOwner->PostMessage(WM_SMSSEND_SHOW, (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc), (((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail));

	((CSunnyCatDlg *)AfxGetMainWnd())->m_bsmssendsucc = false;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendfail = 0;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc = 0;

	m_Mgr.ProcessNewSMSPostMsg( -1);		// add by lab 200912081655
	m_Mgr.m_bIsSendSMS = false;		// add by lab 200912081655
}

bool CThreadSendSMS::SendTo(size_t nNumberIndex)
{
	CString strProgress;
	CString strName ;//= m_Mgr.NameInPhonebook(m_Numbers[nNumberIndex],true);
	if(strName == LoadStringEx(IDS_UNKOWN))
		strName = m_Numbers[nNumberIndex];

	bool bOK;

	int nMaxProgress = m_Numbers.size()*m_Contents.size();

	for(size_t i=0; i<m_Contents.size(); ++i)
	{
		if(m_bCancel)return true;


		//报告进度
		strProgress.Format(TEXT("%s"), LoadStringEx(IDS_SMS_SENDING));
		int nCurProgress = nNumberIndex*m_Contents.size()+i;
		if(m_Contents.size()>=1)
		{
			CString strSending = "";
			strSending = m_Numbers[nNumberIndex];
			//strProgress = "";
			strName.Format(TEXT("(%s)"), strSending);
			strProgress += strName;
		}
		m_pUI->SetProgress(nCurProgress+1, nMaxProgress, strProgress);

		bOK = SendOneSMS(m_Numbers[nNumberIndex], m_Contents[i]);

		CATCmd cmd;
		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度 // ok
		m_Mgr.WaitForATCompleted(cmd,1000);						

//		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度 // ok
//		m_Mgr.WaitForATCompleted(cmd,1000);
		
		if( !(((CSunnyCatDlg *)AfxGetMainWnd())->m_bsmssendsucc) || !bOK )	// send unsuccess
		{
			//&&

			TSMSItem item;
			item.iSMSState = SMS_UNSEND;
			item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_UNSEND;
			item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_UNSEND;
			item.strPhoneNum = m_Numbers[nNumberIndex];
			item.nPSMSState = (PSMSType)2;
			item.strText = m_Contents[i];
			item.tmSMSTime = CTime::GetCurrentTime();

			m_Mgr.m_pOwner->m_smsDlg.InsertUnsendSms(item);
			m_Mgr.PostMessageToClient(WM_SMS_REFRESHLIST,SMS_PC_UNSEND,0);
		}		
		else
		{
			TSMSItem item;
			item.iSMSState = SMS_SEND;
			item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
			item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
			item.strPhoneNum = m_Numbers[nNumberIndex];
			item.nPSMSState = (PSMSType)1;
			item.strText = m_Contents[i];
			item.tmSMSTime = CTime::GetCurrentTime();

			m_Mgr.m_pOwner->m_smsDlg.InsertSendSms(item);
			m_Mgr.PostMessageToClient(WM_SMS_REFRESHLIST,SMS_PC_SEND,0);

//			sendOK++;
			if(sendFail > 0)
				--sendFail;
		}
		
		((CSunnyCatDlg *)AfxGetMainWnd())->m_bsmssendsucc = false;
//		m_iSendState = ((CSunnyCatDlg *)AfxGetMainWnd())->m_smssendsucc;
		
	}
	

	return true;
}

bool CThreadSendSMS::SendOneSMS(const CString& number, const CString& content)
{
	typedef bool (CMobileManager::*SendSMS_func)(const CString& strPhoneNum, 
								int iPriority, 
								const CString& strText);

	SendSMS_func func;
	int strtype = 6;
	switch(m_StringType)
	{
	case STR_DOUBLE_BYTE:
		func = CMobileManager::SendSMS_MultiByte;
		strtype = 6;
		break;
	case STR_ASCII:
		func = CMobileManager::SendSMS_ASCII;
		strtype = 1;
		break;
	case STR_SINGLE_BYTE:
		func = CMobileManager::SendSMS_SingleByte;
		strtype = 0;
		break;
	default:
		ASSERT(0);
	}

//	CATCmd cmd;
//	cmd = m_Mgr.SendATPacket(CMobileATProto::SetSMSPraCmd(strtype));//废黜短信发送格式 // ok
//	m_Mgr.WaitForATCompleted(cmd,2000);
	TRACE(TEXT("SMS content = %s\n"), content);
	
	bool bOK = false;
	for(int nTry=0; nTry<3 && !m_bCancel; ++nTry)//尝试多次发送
	{
		bOK = (m_Mgr.*func)(number, m_nPriority, content);
		if(bOK)
		{
			SaveSentSMS(number, content);
			break;
		}

		if(!m_bCancel)
		{
			Sleep(500*(nTry+1));
		}
		else
		{
			break;
		}
	}
	
	return bOK;
}

void CThreadSendSMS::SaveSentSMS(const CString& number, const CString& content)
{
	CSMSEmbed& smsList = m_Mgr.m_pOwner->m_smsDlg.m_EmbedSend;
	TSMSItem& item = smsList.AddFront();
	item.iSMSState		= SMS_SEND;
	item.strPhoneNum	= number;
	item.strText		= content;
	//item.iPriority		= m_nPriority;
	item.iSaveDays		= m_nSaveDays;
	item.tmSMSTime		= CCDMATime::Instance().GetCDMATime();

//	smsList.SortByTime();
}


// InitAT.cpp: implementation of the CInitAT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sunnycat.h"
#include "InitAT.h"
#include "Porting.h"
#include "SunnyCatDlg.h"
#include "devenum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInitAT::CInitAT(CMobileManager& mgr)
:m_Mgr(mgr),m_bIsResetCom(false),m_bRunning(false)
{

}

CInitAT::~CInitAT()
{

}

void CInitAT::Go()
{
	TRACE(TEXT(" CInitAT::Go start \n"));
	if(m_bRunning)
		return;
	m_bRunning = true;
	m_Mgr.PostMessageToClient(WM_INIT_START);
	SetWorkMode(m_Mgr,USETYPE_USBMODEM,true);
	bool bOK = m_Mgr.Init();
	if(!bOK)
	{
		m_Mgr.ShutDown();
		CString strModemN;
		strModemN.Format(TEXT(""));

//		CString strModemName;
		
//		strModemName = GetModemName();
		
		if(m_Mgr.IsModemExisting(strModemN)&&m_Mgr.m_bSimExist)					
		{
			m_bIsResetCom = true;
			m_Mgr.m_pOwner->m_bIsResetCom = true;
		}
		else
		{
			m_bIsResetCom = false;
			m_Mgr.m_pOwner->m_bIsResetCom = false;
		}
		
		m_Mgr.m_SerialPort.StopMonitoring();
		if(m_bIsResetCom)
		{
			m_Mgr.m_pOwner->ResetCom(TEXT("4D36E96D-E325-11CE-BFC1-08002BE10318"), m_Mgr.GetPortDevName());
			::Sleep(2000);
		}
		
		if(m_Mgr.IsModemExisting(strModemN))
		{
			m_bIsResetCom = false;
			m_Mgr.m_pOwner->m_bIsResetCom = false;
		}
		if(m_bIsResetCom)
			bOK = m_Mgr.Init();
			
		
		if(!bOK)
		{
			m_Mgr.isAbleCom = false;

			bool bDeviceExit = false;
	
			bDeviceExit = m_Mgr.IsModemExisting(strModemN);
			
			if(!bDeviceExit)
			{
				m_bIsResetCom = false;
			}
			m_Mgr.PostMessageToClient(WM_INIT_ERROR,1,0);
			m_Mgr.ShutDown();	//20070514
		}
		else
		{
			m_bIsResetCom = false;
			m_Mgr.m_pOwner->m_bIsResetCom = false;
		}
	}
	
	
	if(bOK)
	{
		m_Mgr.isAbleCom = true;
		CATCmd cmd;
	
		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckCPIN());
		m_Mgr.WaitForATCompleted(cmd,1000);
		m_Mgr.m_strPINResponse.MakeLower();
		
		cmd = m_Mgr.SendATPacket(CMobileATProto::QueryCurNet());//查询当前网络
				m_Mgr.WaitForATCompleted(cmd,1000); 					
		
		if(m_Mgr.m_strPINResponse == "ready")
		{
			m_Mgr.m_simLocked = false;
		}
		else if ((m_Mgr.m_strPINResponse == "sim pin")||
				(m_Mgr.m_strPINResponse == "sim puk"))
		{			
			m_Mgr.m_simLocked = true;
			m_Mgr.PostMessageToClient(WM_SIGNAL_SIMLOCK, 0, 0);
			m_Mgr.PostMessageToClient(WM_INIT_END);
			return;
		}
		else if (m_Mgr.m_strPINResponse == "sim busy");
		else
		{
			m_Mgr.PostMessageToClient(WM_INIT_ERROR,1,0);
			m_Mgr.ShutDown();
			m_Mgr.isAbleCom = false;
			
			m_bRunning = false;
			return;
		}
//		cmd = m_Mgr.SendATPacket(CMobileATProto::TestSimLock());//关闭Echo // ok
//		m_Mgr.WaitForATCompleted(cmd,1000);

		cmd = m_Mgr.SendATPacket(CMobileATProto::SetEcho(false));//关闭Echo // ok
		m_Mgr.WaitForATCompleted(cmd,1000);

		//设置CMEE
		cmd = m_Mgr.SendATPacket(CMobileATProto::SetCmee());				// OK LAB686
		m_Mgr.WaitForATCompleted(cmd,1000);
		
		cmd = m_Mgr.SendATPacket(CMobileATProto::QueryIMEI());				// ok lab686
		m_Mgr.WaitForATCompleted(cmd,1000);
		
//		cmd = m_Mgr.SendATPacket(CMobileATProto::SetCreg());//使能网络登记状态报告
//		m_Mgr.WaitForATCompleted(cmd,1000);		

		/************************SMS*********************************/
		// 设置新SMS 提示信息 
 		cmd = m_Mgr.SendATPacket(CMobileATProto::SetSMSPrompt());			// ok
		m_Mgr.WaitForATCompleted(cmd,1000);

		// 设置SMS 存储位置
		cmd = m_Mgr.SendATPacket(CMobileATProto::SMSBoxSelect("SM"));			// OK 
		m_Mgr.WaitForATCompleted(cmd,1000);

		// 设置SMS 格式
		cmd = m_Mgr.SendATPacket(CMobileATProto::SetCMGF());//设置短信格式  // ok
		m_Mgr.WaitForATCompleted(cmd,1000);	

		// 设置网络模式为混合模式
//		cmd = m_Mgr.SendATPacket(CMobileATProto::SetMode(8));//设置短信格式  // ok
//		m_Mgr.WaitForATCompleted(cmd,1000);	
		/************************************************************/

		/************************VOICE**********************************/
//		int nPCMVoice = 2;
//		cmd = m_Mgr.SendATPacket(CMobileATProto::SendDDSETEX(nPCMVoice));	// OK // CMobileATProto::SetPCMSWitch(nPCMVoice));
//		m_Mgr.WaitForATCompleted(cmd,1000);

//		cmd = m_Mgr.SendATPacket(CMobileATProto::SetCVOICE(0));			// OK  ADD BY LAB686 20091103
//		m_Mgr.WaitForATCompleted(cmd,1000);
		
		//允许来电显示
		cmd = m_Mgr.SendATPacket(CMobileATProto::SetDialShow(true));			// 没有这个命令格式，先加上
		m_Mgr.WaitForATCompleted(cmd,1000);	

		cmd = m_Mgr.SendATPacket(CMobileATProto::PhoneBookSelect("SM"));
		m_Mgr.WaitForATCompleted(cmd,1000);
		/************************************************************/
		
		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度 // ok
		m_Mgr.WaitForATCompleted(cmd,1000);								

//		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度 // ok
//		m_Mgr.WaitForATCompleted(cmd,1000);	

		DWORD dwTime = GetTickCount();
		while(m_Mgr.m_nSignal <=5 )
		{
			cmd = m_Mgr.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度// ok
			m_Mgr.WaitForATCompleted(cmd,1000);
//			cmd = m_Mgr.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度// ok
//			m_Mgr.WaitForATCompleted(cmd,1000);
			if((GetTickCount()-dwTime)>15000)
			{
				break;                   
			}
			::Sleep(1000);
		}		
		cmd = m_Mgr.SendATPacket(CMobileATProto::SetCharset());//设置字符集
		m_Mgr.WaitForATCompleted(cmd,1000);	
		
		cmd = m_Mgr.SendATPacket(CMobileATProto::CscaPhoneNum());//短信中心号码
		m_Mgr.WaitForATCompleted(cmd,1000);	
		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckSignal());//查询信号强度
		m_Mgr.WaitForATCompleted(cmd,1000);
//		cmd = m_Mgr.SendATPacket(CMobileATProto::CheckHDRSignal());//查询信号强度
//		m_Mgr.WaitForATCompleted(cmd,1000);

		cmd = m_Mgr.SendATPacket(CMobileATProto::TestSimLock());//测试SIM锁定// ok
		m_Mgr.WaitForATCompleted(cmd,1000);		
		
	}
	m_Mgr.PostMessageToClient(WM_INIT_END);

	m_bRunning = false;
	TRACE(TEXT(" CInitAT::Go end \n"));
}

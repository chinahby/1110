// InfoDialup.h: interface for the CInfoDialup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFODIALUP_H__D3EAC210_5415_45E4_9C2F_C52C5CD80F1B__INCLUDED_)
#define AFX_INFODIALUP_H__D3EAC210_5415_45E4_9C2F_C52C5CD80F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GlobalDataCenter.h"
#include "InfoConnect.h"


struct TMTUSetting
{
	DWORD m_dwPPPMTU;
	DWORD m_dwVPNMTU;
public:
	TMTUSetting();
	bool Save()const;
	void Load();
	void ResetToDefault();
	bool operator==(const TMTUSetting& an)const;
	bool operator!=(const TMTUSetting& an)const
	{
		return !operator==(an);
	}
};

class CInfoOptions
{
public:
	CInfoOptions();
	
	bool SaveToReg();
	bool LoadFromReg();
	void ResetToDefault();

	CString m_strConnection;///通讯连接名称(entry Name)
	CString m_strPhoneNum; 	///拨号电话号码
	CString m_strUserName; 	///用户名
	CString m_strPassword; 	///密码
	CString m_strApn;///接入点
	CString m_initialAT;///modem初始化AT
	
	CString m_strDevName; 	///设备名称(modem)
	CString m_strDNSMaster;	///主DNS
	CString m_strDNSSlave;	///从DNS
	CString m_strStaticIP;
	bool m_bDNSEnable;
	bool m_bUseStaticIP;
	bool	m_bEnableAutoConnection;//启动时自动连接
	bool	m_bAutoReconnect;//断开时自动重新连接

	CString m_ComPort;		///串口名


	CString m_strIncomeRing;//来电铃声
	CString m_strSMSRing;	//短信铃声
	CString m_strCallConnectRing;//电话接通铃声
	CString m_strCallDisconnectRing;//电话断开铃声


//	int m_TDelay;//T键延时jiangguochao 20070119 deleted for P/T delaying
//	int m_PDelay;//T键延时jiangguochao 20070119 deleted for P/T delaying

//	CString m_strLocalPhone;//本机号码
//	CString m_strLocalName;	//本机姓名
	//CString m_strCscaPhone;//短信中心

	TMTUSetting m_MTUSetting; //MTU(Max Transmit Unit)设置
};

#endif // !defined(AFX_INFODIALUP_H__D3EAC210_5415_45E4_9C2F_C52C5CD80F1B__INCLUDED_)

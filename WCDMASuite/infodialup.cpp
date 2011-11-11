#include "stdafx.h"
#include "InfoDialup.h"
#include <atlbase.h>
#include "Util.h"
#include "Porting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CInfoOptions::CInfoOptions()
{
	ResetToDefault();
}

static void QueryString
(
	CRegKey& key,
	CString& value, 
	const CString& entry
)
{
    TCHAR szValue[1024] = {0};
	DWORD nCount = sizeof(szValue)-1;;
	if(key.QueryValue(szValue, entry, & nCount) == ERROR_SUCCESS)
	{
		value = szValue;
	}
}

static CString GetConfigString
(
	const CString& strSection,
	const CString& strEntry,
	const CString& strDefault
)
{
	CString configFile = GetAppPath()+"default_conf.txt";
	TCHAR value[1024]={0};
	::GetPrivateProfileString(
		strSection, 
		strEntry, 
		strDefault, 
		value, 
		sizeof(value)-1,
		configFile);

	return value;
}

void CInfoOptions::ResetToDefault()
{
	m_strPhoneNum = GetConfigString("network", "number", "*99#");
	m_strUserName = GetConfigString("network", "user", "Card");
	m_strPassword = GetConfigString("network", "password", "card");
	m_strApn = GetConfigString("network", "apn", "Telecom");

	m_strConnection = GetConnectionName();
	//m_strDevName 不变
	m_strDNSMaster = "";
	m_strDNSSlave  = "";
	m_bDNSEnable   = false;

	m_bEnableAutoConnection = false;
	m_bAutoReconnect = false;

	m_strIncomeRing = GetAppPath() + "music\\ring.wav";
	m_strSMSRing = GetAppPath() + "music\\sms.wav";
	m_strCallConnectRing = GetAppPath() + "music\\call-connect.wav";
	m_strCallDisconnectRing = GetAppPath() + "music\\call-Disconnect.wav";
	//m_initialAT = "AT+CGDCONT=1,\"IP\",\"CDMA Wireless\"";

//	m_strLocalPhone = "";
//	m_strLocalName = "";
	//m_strCscaPhone ="";

	//m_TDelay = 3000;jiangguochao 20070119 deleted for P/T delaying
	//m_PDelay = 1500;jiangguochao 20070119 deleted for P/T delaying

	m_MTUSetting.ResetToDefault();
}

bool CInfoOptions::LoadFromReg()
{
	CRegKey reg;
    
	DWORD dwValue;
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	
	//拨号上网：电话号码
	QueryString(reg, m_strPhoneNum, "phone");

	//拨号上网：用户名
	QueryString(reg, m_strUserName, "UserId");

	//拨号上网：密码
	QueryString(reg, m_strPassword, "PassWord");

	//拨号上网：设备(Modem)名称
	m_strDevName = GetModemName();

	//DNS main
	QueryString(reg, m_strDNSMaster, "DNS1");
	
	//DNS Minor
	QueryString(reg, m_strDNSSlave, "DNS2");

	//Static IP
	QueryString(reg, m_strStaticIP, "StaticIP");

	//来电铃声
	QueryString(reg, m_strIncomeRing, "IncomeRing");

	//短信铃声
	QueryString(reg, m_strSMSRing, "SMSRing");

	//呼叫接通铃声
	QueryString(reg, m_strCallConnectRing, "CallConnRing");

	//呼叫断开铃声
	QueryString(reg, m_strCallDisconnectRing, "CallDisconnRing");

	//modem初始化AT设定
	//QueryString(reg, m_initialAT, "InitialAT");

 	//本机号码
//	QueryString(reg, m_strLocalPhone, "LocalPhoneNum");

 	//本机号码
//	QueryString(reg, m_strLocalName, "LocalName");

	 //短信中心地址
	//QueryString(reg, m_strCscaPhone, "CscaPhone");

    if(reg.QueryValue(dwValue,TEXT("DNSEnable")) == ERROR_SUCCESS)
	{
		if(dwValue)
			m_bDNSEnable = 1;
		else
			m_bDNSEnable = 0;
    }
	else
	{
		m_bDNSEnable = 0;
	}

	m_bUseStaticIP=0;
	if(reg.QueryValue(dwValue,TEXT("UseStaticIP")) == ERROR_SUCCESS)
	{
		m_bUseStaticIP = dwValue!=0;
	}

/*jiangguochao 20070119 deleted for P/T delaying
	if(reg.QueryValue(dwValue,"TDelay") == ERROR_SUCCESS)
	{
		m_TDelay = dwValue;
	}

	if(reg.QueryValue(dwValue,"PDelay") == ERROR_SUCCESS)
	{
		m_PDelay = dwValue;
	}
*/

	if(reg.QueryValue(dwValue,TEXT("ConnectOnStartup")) == ERROR_SUCCESS)
	{
		m_bEnableAutoConnection = dwValue!=0;
	}

	if(reg.QueryValue(dwValue,TEXT("AutoReconnect")) == ERROR_SUCCESS)
	{
		m_bAutoReconnect = dwValue!=0;
	}

	reg.Close(); 

	return true;
}

bool CInfoOptions::SaveToReg()
{
	CRegKey reg;
    LPCTSTR lpszKey =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,lpszKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,lpszKey) != ERROR_SUCCESS)
		{
			return false;
		}
	}			
	
	reg.SetValue(m_strPhoneNum, TEXT("Phone"));
	reg.SetValue(m_strUserName, TEXT("UserId"));
	reg.SetValue(m_strPassword, TEXT("PassWord"));
	reg.SetValue(m_strDevName, TEXT("DevName"));
    reg.SetValue(m_strDNSMaster, TEXT("DNS1"));
	reg.SetValue(m_strDNSSlave, TEXT("DNS2"));
	reg.SetValue(m_strStaticIP, TEXT("StaticIP"));

    reg.SetValue(m_strIncomeRing, TEXT("IncomeRing"));
	reg.SetValue(m_strSMSRing, TEXT("SMSRing"));
	reg.SetValue(m_strCallConnectRing, TEXT("CallConnRing"));
	reg.SetValue(m_strCallDisconnectRing, TEXT("CallDisconnRing"));
		//reg.SetValue(m_initialAT, "InitialAT");
//	reg.SetValue(m_strLocalPhone, "LocalPhoneNum");	
// 	reg.SetValue(m_strLocalName, "LocalName");
	//reg.SetValue(m_strCscaPhone, "CscaPhone");

/*
	reg.SetValue(m_TDelay, "TDelay");
	reg.SetValue(m_PDelay, "PDelay");
*/

	reg.SetValue((DWORD)m_bDNSEnable, TEXT("DNSEnable"));
	reg.SetValue((DWORD)m_bUseStaticIP, TEXT("UseStaticIP"));
	reg.SetValue((DWORD)m_bEnableAutoConnection, TEXT("ConnectOnStartup"));
	reg.SetValue((DWORD)m_bAutoReconnect, TEXT("AutoReconnect"));

	reg.Close(); 


	//将自动运行信息写入注册表
	const CString RegRunPath = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	bool bOK = true;
	if(reg.Open(HKEY_LOCAL_MACHINE,RegRunPath) != ERROR_SUCCESS && m_bEnableAutoConnection)
	{
		reg.Create(HKEY_LOCAL_MACHINE,RegRunPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE,RegRunPath) != ERROR_SUCCESS)
		{
			bOK = false;
		}
	}		
	if(bOK)
	{
		if(!m_bEnableAutoConnection)
			reg.DeleteValue(TEXT("SUNNYCAT card"));
		else
		{
			TCHAR buf[1024] = {0};
			GetModuleFileName(NULL, buf, sizeof(buf));
			CString strApp;
			strApp.Format(TEXT("\"%s\""), buf);
			reg.SetValue(strApp, TEXT("SUNNYCAT card"));
		}
	}


	return true;
}

TMTUSetting::TMTUSetting()
{
	Load();
}

bool TMTUSetting::Save()const
{
	CRegKey reg;
	CString strKey = TEXT("SYSTEM\\CurrentControlSet\\Services\\Ndiswan\\Parameters\\Protocols\\0");

	if(reg.Open(HKEY_LOCAL_MACHINE,strKey) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,strKey);		
		if(reg.Open(HKEY_LOCAL_MACHINE,strKey) != ERROR_SUCCESS)
		{
			return false;
		}
	}	
	
	bool bOK = true;
	if(bOK)
	{
		bOK = ERROR_SUCCESS == reg.SetValue((DWORD)0X800, TEXT("ProtocolType"));
	}
	if(bOK)
	{
		bOK = ERROR_SUCCESS == reg.SetValue((DWORD)0X21, TEXT("PPPProtocolType"));
	}
	if(bOK)
	{
		bOK = ERROR_SUCCESS == reg.SetValue(m_dwPPPMTU, TEXT("ProtocolMTU"));
	}
	if(bOK)
	{
		bOK = ERROR_SUCCESS == reg.SetValue(m_dwVPNMTU, TEXT("TunnelMTU"));
	}

	return bOK;
}

void TMTUSetting::Load()
{
	CRegKey reg;
	CString strKey = "SYSTEM\\CurrentControlSet\\Ndiswan\\Parameters\\Protocols\\0";

	if(reg.Open(HKEY_LOCAL_MACHINE,strKey) != ERROR_SUCCESS)
	{
		ResetToDefault();
	}
	else
	{
		reg.QueryValue(m_dwPPPMTU, TEXT("ProtocolMTU"));
		reg.QueryValue(m_dwVPNMTU, TEXT("TunnelMTU"));
	}
	reg.Close();
}

void TMTUSetting::ResetToDefault()
{
	//Win2000/XP/Server2003的默认值
	m_dwPPPMTU = 1500;
	m_dwVPNMTU = 1400;
}

bool TMTUSetting::operator==(const TMTUSetting& an)const
{
	return m_dwPPPMTU == an.m_dwPPPMTU && m_dwVPNMTU == an.m_dwVPNMTU;
}
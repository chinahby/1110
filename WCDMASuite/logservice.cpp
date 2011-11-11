#include "stdafx.h"
#include "LogService.h"
#include "CriticalSecMon.h"
#include "Porting.h"
#include <atlbase.h>
#include "Util.h"

static void SaveEnableValue(bool bEnable)
{
	CRegKey reg;
    CString RegPath =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,RegPath) != ERROR_SUCCESS)
	{
		reg.Create(HKEY_LOCAL_MACHINE,RegPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE,RegPath) != ERROR_SUCCESS)
		{
			return ;
		}
	}			

	reg.SetValue((DWORD)bEnable, TEXT("EnableLog"));
	reg.Close();
}

static bool LoadEnableValue()
{
	CRegKey reg;
    CString RegPath =  GetConfigString(); 
    
	if(reg.Open(HKEY_LOCAL_MACHINE,RegPath) != ERROR_SUCCESS)
	{
		return false;
	}			

	DWORD dwEnable = 0; 
	reg.QueryValue(dwEnable, TEXT("EnableLog"));
	reg.Close();
	return dwEnable !=0;
}


std::string GetModulePath()
{
	char path_buffer[MAX_PATH];	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	GetModuleFileNameA(NULL, path_buffer, MAX_PATH);
	_splitpath(path_buffer, drive, dir, 0, 0 );

	std::string sPath = drive;
	sPath += dir;

	return sPath;
}

CLogService& CLogService::Instance()
{
	static CLogService s_Inst;

	return s_Inst;
}

CLogService::CLogService()
{
	m_bEnable = LoadEnableValue();

	std::string filename = GetModulePath();
	filename += "log.txt";
	SetFileName(filename.c_str());
}

CLogService::~CLogService()
{
	m_LogStream.close();
}

void CLogService::Enable(bool bEnabled)
{
	CCriticalMonitor lock(m_CS);
	m_bEnable = bEnabled;
	SaveEnableValue(bEnabled);
}

static void LogTime(std::ofstream& file)
{
	SYSTEMTIME sysTm;
	GetLocalTime(&sysTm);
	file<<sysTm.wYear<<'-'<<sysTm.wMonth<<'-'<<sysTm.wDay<<' '<<sysTm.wHour<<':'<<sysTm.wMinute<<':'<<sysTm.wSecond<<':'<<sysTm.wMilliseconds<<' ';
}

std::string HexStr(const char* str, size_t size)
{
	std::string strHex;
	const char* pEnd = str+size;
	for(const char* p = str; p<pEnd; ++p)
	{
		BYTE b= (0XF0 & (*p))>>4;
		if(b>=0 && b<=9)
			strHex += '0'+b;
		else
			strHex += 'A'+(b-10);

		b= 0X0F & (*p);
		if(b>=0 && b<=9)
			strHex += '0'+b;
		else
			strHex += 'A'+(b-10);

		strHex += ' ';
	}

	return strHex;
}

static bool NeedHexOut(const  char* str, size_t size)
{
	bool bNeedHexOut = false;
	for(size_t i=0; i<size; ++i)
	{
		if(!str[i])
		{
			bNeedHexOut = true;
			break;
		}
		bool bASCII = (str[i]>=0X20 && str[i]<=0X7E)|| (str[i] == 0X0D || str[i] == 0X0A);
		if(!bASCII)
		{
			bNeedHexOut = true;
			break;
		}
	}

	return bNeedHexOut;
}

static std::string StrSub(const char* str, size_t size)
{
	std::string strOut;
	for(size_t i=0; i<size; ++i) strOut += str[i];

	return strOut;
}

void CLogService::LogString(const char* str, size_t size)
{
	if(!m_bEnable || !str)return;
	CCriticalMonitor lock(m_CS);
	
	size_t len = size? size : strlen(str);
	std::string strOut = StrSub(str, len);
	if(strOut.find("+CSQ") == std::string::npos)// +CSQ ²»Êä³ö
	{
		LogTime(m_LogStream);
		m_LogStream<<strOut.c_str()<<std::endl;
		if(size && NeedHexOut(str, size))
		{
			m_LogStream<<"HEX:"<<HexStr(str, size).c_str() <<std::endl;
		}

		m_LogStream.flush();
	}
}

void CLogService::SetFileName(const char* filename)
{
	CCriticalMonitor lock(m_CS);
	m_LogStream.close();
	m_LogStream.open(filename);
	m_LogStream.clear();
}

void LogString(const char* str,size_t size)
{
	CLogService::Instance().LogString(str, size);
}

void LogString(const wchar_t* str,size_t size)
{
	std::string stra = ToMultiBytes(str,size);
	CLogService::Instance().LogString(stra.c_str(), size);
}

void LogString(CString strLog)
{
	std::string stra = ToMultiBytes(LPCTSTR(strLog),strLog.GetLength());
	CLogService::Instance().LogString(stra.c_str(), strLog.GetLength());
}

void EnableLog(bool bEnable)
{
	CLogService::Instance().Enable(bEnable);
}

bool LogEnabled()
{
	return CLogService::Instance().Enabled();
}

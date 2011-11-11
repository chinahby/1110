#include "StdAfx.h"
#include <stdlib.h>
#include <stdio.h>
#include "Util.h"
#include <fstream>
#include <map>
#include <vector>
#include "porting.h"
#include <atlbase.h>
#include "GlobalDataCenter.h"
#include <Setupapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetAppPath()
{
	TCHAR path_buffer[MAX_PATH];	
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	GetModuleFileName(NULL, path_buffer, MAX_PATH);
	_tsplitpath(path_buffer, drive, dir, 0, 0 );

	CString sPath = drive;
	sPath += dir;

	return sPath;
}


/*
秒变成"分：秒"形式的字符串
*/
CString SecondToTime(int iSecond)
{
	CString strTime;

	strTime.Format(TEXT("%02d:%02d:%02d"), iSecond/3600, (iSecond%3600)/60, iSecond % 60);

	return strTime;
}

int ShowMessageBox(UINT strid, UINT nType /*= MB_OK*/)
{
	return AfxMessageBox(LoadStringEx(strid), nType);
//	return GMessageBox(LoadStringEx(strid), NULL, nType);
}

void DumpStringTable()
{
	CString sFile = GetAppPath()+"string_table.txt";
	std::ofstream file(ToMultiBytes(LPCTSTR(sFile)).c_str());
	if(!file.is_open())return;

	for(int i=0;i<1000;++i)
	{
		CString str;
		str.LoadString(i);
		if(!str.GetLength())continue;
		str.Replace(TEXT("\r\n"),TEXT("\\n"));
		str.Replace(TEXT("\n"),TEXT("\\n"));
		file<<i<<";;;"<<(LPCTSTR)str<<std::endl;
	}
}

//*****************************************
//	class stringtable
//*****************************************
class CStringTable
{
public:
	static CStringTable& Instance();
	~CStringTable();
	bool GetString(UINT id, CString& str)const;
private:
	CStringTable();

	void Load();
private:
	CStringTable& operator=(const CStringTable&);
	CStringTable(const CStringTable&);
private:
	std::map<UINT, CString> m_Map;

	typedef std::map<UINT, CString>::iterator iterator;
	typedef std::map<UINT, CString>::const_iterator const_iterator;
};

CStringTable& CStringTable::Instance()
{
	static CStringTable s_Instance;

	return s_Instance;
}
CStringTable::~CStringTable()
{
}
bool CStringTable::GetString(UINT id, CString& str)const
{
	const_iterator it = m_Map.find(id);
	if(it != m_Map.end())
	{
		str = it->second;
		return true;
	}
	
	return false;
}
CStringTable::CStringTable()
{
	
	Load();
}
void CStringTable::Load()
{
	CString strLanguage = GetLanguage();
	CString strProduct = GetProduct();
	CString sFile = _T("");
	
	if(strLanguage == "-Ru")
	{
//		if(strProduct == "-p") //PCMCIA
//			sFile = GetAppPath()+"resource\\resource_cn.txt";
//		else if(strProduct == "-u") // USB_MODEM
			sFile = GetAppPath()+"resource\\resource_ru_usb.txt";
//		else if(strProduct == "-c") //COM
//			sFile = GetAppPath()+"resource\\resource_cn_com.txt";
	}
	else if(strLanguage == "-En")
	{
//		if(strProduct == "-p") //PCMCIA
//			sFile = GetAppPath()+"resource\\resource_en.txt";
//		else if(strProduct == "-u") // USB_MODEM
			sFile = GetAppPath()+"resource\\resource_en_usb.txt";
//		else if(strProduct == "-c") //COM
//			sFile = GetAppPath()+"resource\\resource_en_com.txt";
	}

	
	std::ifstream file(ToMultiBytes(LPCTSTR(sFile)).c_str());
	if(!file.is_open())return;

	char buf[1024];
	while(!file.eof())
	{
		memset(buf,0, sizeof(buf));
		file.getline(buf,sizeof(buf)-1);
		
		CString str = ToUnicode(buf).c_str();
		if(!str.GetLength())continue;
		int nFind = str.Find(TEXT(";;;"));//使用;;;作为ID和字符串的分割标志
		if(nFind == -1)continue;
		CString strNum = str.Left(nFind);
		if(!strNum.GetLength())continue;
		UINT nID = _ttoi(strNum);
		CString strV = str.Right(str.GetLength()-nFind-3);
		nFind = strV.Find('\r');
		if(nFind!=-1)strV.Delete(nFind);
		if(!strV.GetLength())continue;
		strV.Replace(TEXT("\\n"),TEXT("\n"));
		m_Map[nID] = strV;
	}
}

CString LoadStringEx(UINT id)
{
	CString str;
	if(!CStringTable::Instance().GetString(id, str))
	{
		str.Format(TEXT("%d: Message not defined"), id);
	}
	return str;
}

HANDLE CreateThreadEx(LPTHREAD_START_ROUTINE func, void* param)
{
	DWORD threadID;
	return ::CreateThread(
						NULL,			// SD
						0,				// initial stack size
						func,		// thread function
						param,			// thread argument
						0,				// start immediately
						&threadID);	// thread identifier
}


//计算字符串中的字符个数，中英文都算一个字符
size_t GetCharNum(const CString& strText)
{
	if(!strText.GetLength())return 0;
	std::string stdstr = ToMultiBytes(LPCTSTR(strText));

	const char* pStrBegin = stdstr.c_str(); 
	const char* pStrEnd = pStrBegin+stdstr.size();
	size_t iCharNum = 0;
	while(pStrBegin<pStrEnd)
	{		
		if(IsDBCSLeadByteEx(CP_ACP,*pStrBegin))
			pStrBegin += 2;
		else
			pStrBegin += 1;			

		iCharNum++;
	}

	return iCharNum;
}

//短信长度限制
int MaxSMSLength(EStringType strType)
{
	switch(strType)
	{
	case STR_DOUBLE_BYTE://多字节
		return 70;
	case STR_SINGLE_BYTE://单字节
		return 140;
	default:			 //ASCII码
		return 160;
	}
}

//获取字符串的类型
EStringType GetStringType(const CString& strText)
{
	return GetStringType(ToMultiBytes(LPCTSTR(strText)).c_str(), ToMultiBytes(LPCTSTR(strText)).size());
}

EStringType GetStringType(const char* str, size_t strLen)
{
	const char* pStrBegin = str; 
	const char* pStrEnd = pStrBegin+strLen;

	bool bIsMutiBytesString = false;
	bool bIsSngleByteString = false;
	while(pStrBegin<pStrEnd)
	{		
		if(IsDBCSLeadByteEx(CP_ACP,*pStrBegin)||((BYTE)*pStrBegin) == 96) //`强制按UNICODE编码
		{
			bIsMutiBytesString = true;
			break;
		}
		else if(((BYTE)*pStrBegin)>=128)
		{
			bIsSngleByteString = true;
		}
		++pStrBegin;
	}

	if(bIsMutiBytesString)return STR_DOUBLE_BYTE;
	if(bIsSngleByteString)return STR_SINGLE_BYTE;

	return STR_ASCII;


}

//获取字符串的左边若干个字符组成的子串
CString GetLeftString(const CString& strText,size_t iCount)
{
	const char* pStrBegin = ToMultiBytes((LPCTSTR)strText).c_str(); 
	const char* pStrEnd = pStrBegin+ToMultiBytes((LPCTSTR)strText).size();
	size_t iCharNum = 0;
	while(pStrBegin<pStrEnd && iCharNum<iCount)
	{		
	//	if(IsDBCSLeadByteEx(CP_ACP,*pStrBegin))
	//		pStrBegin += 2;
	//	else
			pStrBegin += 1;			

		iCharNum++;
	}
	int nPos = pStrBegin-(LPCSTR)ToMultiBytes((LPCTSTR)strText).c_str();

	return strText.Left(nPos);
}

bool IsUnicode(const char *p, int len)
{
	return 0 != IsTextUnicode((void*)p, len, NULL);
}

std::wstring ToUnicode(const std::string& str, UINT codepage)
{
	if(codepage == 0)
		codepage = GetACP();
	int n = ::MultiByteToWideChar(codepage, MB_PRECOMPOSED,
		str.c_str(), (int)str.size(), NULL, 0);
	wchar_t* wstr = new wchar_t[n+1];
	::MultiByteToWideChar(codepage, MB_PRECOMPOSED,
		str.c_str(), (int)str.size(), wstr, n);
	wstr[n] = 0;
	std::wstring w = wstr;
	delete[] wstr;
	return w;
}
//转换字节序
static void SwapUnicodeBytes(BYTE* bytes, size_t size)
{
	for(size_t i=0; i+1 < size; i+=2)
	{
		char chTmp;
		chTmp = bytes[i];
		bytes[i] = bytes[i+1];
		bytes[i+1] = chTmp;
	}
}


std::string BytesToStringA(BYTE* bytes, size_t size, bool bUnicode)
{
	if(!bUnicode)
	{
		std::string str;
		str.reserve(size+1);
		for(size_t i=0; i<size; ++i)str += (char)bytes[i];
		return str;
	}
	else
	{
		SwapUnicodeBytes(bytes, size);
		return ToMultiBytes((wchar_t*)bytes, size/2);
	}
}


std::wstring BytesToStringW(BYTE* bytes, size_t size, bool bUnicode)
{
	if(!bUnicode)
	{
		std::string str;
		str.reserve(size+1);
		for(size_t i=0; i<size; ++i)str += (char)bytes[i];
		return ToUnicode(str);
	}
	else
	{
		SwapUnicodeBytes(bytes, size);
		size_t charcount = size/2;
		wchar_t* unicode = (wchar_t*)bytes;

		std::wstring str;
		str.reserve(charcount);

		for(size_t i=0; i<size; ++i)
			str += unicode[i];

		return str;
	}
}

std::string BytesToString(BYTE* bytes, size_t size, bool bUnicode)
{
	return BytesToStringA(bytes, size, bUnicode);
}

std::string ToMultiBytes(const wchar_t* str, size_t charcount, UINT codepage)
{
	if(codepage == 0)
		codepage = GetACP();
	int n = ::WideCharToMultiByte(codepage, 0, 
		str, (int)charcount, NULL, 0, NULL, NULL);
	char* astr = new char[n+1];
	n = ::WideCharToMultiByte(codepage, 0, 
		str, (int)charcount, astr, n, NULL, NULL);
	astr[n] = 0;
	std::string a = astr;
	delete[] astr;
	return a;
}

std::string ToMultiBytes(const std::wstring& str, UINT codepage)
{
	return ToMultiBytes(str.c_str(), str.size(), codepage);
}

void UnicodeToASCII(LPCWSTR SourceStr, DWORD dwWCharCount, LPSTR pASCII, DWORD dwASCIISize)
{
	int nConvert = ::WideCharToMultiByte(
		GetACP(),
		CP_ACP,
		(LPWSTR)SourceStr, 
		dwWCharCount,
		pASCII, 
		dwASCIISize, 
		0, 
		0);
	
	if(nConvert>0)
	{
		pASCII[nConvert] = 0;
	}

}

//将ASCII格式的字符串转换为UNICODE格式
int ASCIIToUnicode(LPCSTR ASC, DWORD ASCCount,	LPWSTR WCharBuf, DWORD dwWCharSize)
{
	int nConverted = ::MultiByteToWideChar(
		GetACP(), 
		MB_PRECOMPOSED,
		ASC, 
		ASCCount,
		WCharBuf, dwWCharSize);
	if(nConverted<dwWCharSize)
	{
		WCharBuf[nConverted] = 0;
	}

	return nConverted;
}

/*
将Uncide的高字节与低字节反转
*/
void ReverseUnicode(BYTE *pUnicodem, int iUnicodeNum)
{
	char chTmp;
	for(int i = 0; i < iUnicodeNum; i ++)
	{
		chTmp = pUnicodem[i*2];
		pUnicodem[i*2] = pUnicodem[i*2 + 1];
		pUnicodem[i*2 + 1] = chTmp;
	}
}

//运行一个程序
HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	memset (&startupInfo, 0, sizeof(STARTUPINFO));
	memset (&processInfo, 0, sizeof(PROCESS_INFORMATION));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = bShowWindow? SW_MINIMIZE: SW_HIDE;

	TCHAR szCmdLine[256] ={0};
	memcpy(szCmdLine,strCmdLine, strCmdLine.GetLength());
	if(!CreateProcess (NULL, szCmdLine, NULL, NULL, FALSE, 
	   NULL, //use DETACHED_PROCESS if try to create a console process
	   NULL, 
	   NULL, &startupInfo, &processInfo))
	{
		return NULL;
	}
	CloseHandle(processInfo.hThread);

	return 	processInfo.hProcess;
}

DWORD OptionsShellType()
{
	DWORD ShellType;
	DWORD winVer;
	
	
	winVer = GetVersion();
	if(winVer < 0x80000000){/*NT */
		ShellType = Win_NT3;
		OSVERSIONINFO osvi;
		memset(&osvi, 0, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		if(osvi.dwMajorVersion==4L)
			ShellType = Win_NT4;
		else if(osvi.dwMajorVersion==5L && osvi.dwMinorVersion==0L)
			ShellType=Win_2000;
		else if(osvi.dwMajorVersion==5L && osvi.dwMinorVersion==1L)
			ShellType=Win_XP;
		else if(osvi.dwMajorVersion==5L && osvi.dwMinorVersion==2L)
			ShellType=Win_2003;
		else if(osvi.dwMajorVersion > 5)
			ShellType = Win_Vista;
	}
	else if  (LOBYTE(LOWORD(winVer))<4)
		ShellType = Win_32s;
	else{
		ShellType = Win_95;
		OSVERSIONINFO osvi;
		memset(&osvi,0,sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		if(osvi.dwMajorVersion==4L && osvi.dwMinorVersion==10L)
			ShellType = Win_98;
		else if(osvi.dwMajorVersion==4L && osvi.dwMinorVersion==90L)
			ShellType = Win_ME;
	}

	return ShellType;
}

class CPBMathingTable
{
public:
	static CPBMathingTable& Instance();
	bool MathNum(const CString& strNum1, const CString& strNum2);
private:
	CPBMathingTable();
	void Load();

	static bool Mating(	const CString& strNum1, 
						const CString& strNum2, 
						const CString& strRule);
private:
	CPBMathingTable(const CPBMathingTable&);
	CPBMathingTable& operator=(const CPBMathingTable&);
private:
	std::vector<CString> m_Data;
};

CPBMathingTable::CPBMathingTable()
{
	Load();
}

void CPBMathingTable::Load()
{
	CString strFile = GetAppPath()+"PBMatchRules.txt";
	
	std::ifstream file(ToMultiBytes(LPCTSTR(strFile)).c_str());
	if(!file.is_open())return;

	while(!file.eof())
	{
		char buf[512]={0};
		file.getline(buf,sizeof(buf)-1);
		for(int i=0;i<sizeof(buf) && buf[i]; ++i)
		{
			if(buf[i]=='X')buf[i]='x';
		}
		CString str=ToUnicode(buf).c_str();
		str.TrimLeft();
		if(str.IsEmpty())continue;
		if(str[0]=='#')continue;
		
		m_Data.push_back(str);
	}
}

CPBMathingTable& CPBMathingTable::Instance()
{
	static CPBMathingTable s_Inst;

	return s_Inst;
}

bool CPBMathingTable::MathNum(const CString& strNum1, const CString& strNum2)
{
	for(size_t i=0; i<m_Data.size(); ++i)
	{
		if(Mating(strNum1, strNum2, m_Data[i]))return true;
	}

	return false;
}

bool CPBMathingTable::Mating(	
							 const CString& strNum1, 
							 const CString& strNum2, 
							 const CString& strRule)
{
	const char* str = ToMultiBytes(LPCTSTR(strRule)).c_str();
	for(;*str && *str!='x';++str);
	if(!*str)return false;
	int nTagLen = str-(LPCSTR)ToMultiBytes(LPCTSTR(strRule)).c_str();
	CString strTag = strRule.Left(nTagLen);

	CString strA = strNum1;
	if(strNum1.Find(strTag)==0)
		strA.Delete(0, strTag.GetLength());
	CString strB = strNum2;
	if(strNum2.Find(strTag)==0)
		strB.Delete(0, strTag.GetLength());

	if(strB.GetLength() != strA.GetLength())return false;
	if(strB.GetLength() != strRule.GetLength()-nTagLen)return false;

	const char* num1 = ToMultiBytes(LPCTSTR(strA)).c_str();
	const char* num2 = ToMultiBytes(LPCTSTR(strB)).c_str();
	for(;*str; ++num1, ++num2, ++str)
	{
		if(*num1 != *num2)return false;
		if(*str != 'x')
		{
			if(*str != *num1)return false;
		}
	}

	return true;
}


bool MathingNumByRules(const CString& strNum1, const CString& strNum2)
{
	return CPBMathingTable::Instance().MathNum(strNum1, strNum2);
}

void TimeFromString(const CString& strCDMA, CTime& tm)
{
	if(strCDMA.IsEmpty())
	{
		tm = CTime::GetCurrentTime();
		return;
	}

	int iYear,iMonth, iDay, iHour, iMinute, iSecond;

	if(strCDMA.Find(TEXT("-"),0) != -1)
		_stscanf(strCDMA, TEXT("%d-%d-%d %d:%d:%d"), &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
	else
		_stscanf(strCDMA, TEXT("%d/%d/%d %d:%d:%d"), &iMonth, &iDay, &iYear, &iHour, &iMinute, &iSecond);
				
	if(iYear >= 70 && iYear<100)
	{
		iYear += 1900;
	}
	else if(iYear < 70)
	{
		iYear += 2000;	
	}
	if(iMonth<=0)iMonth=1;
	if(iMonth>12)iMonth=12;
	if(iDay<=0)iDay=1;
	if(iDay>31)iDay=31;
	if(iHour<=0)iHour=0;
	if(iHour>=24)iHour=23;
	if(iMinute<=0)iMinute=0;
	if(iMinute>=60)iMinute=59;
	if(iSecond<=0)iSecond=0;
	if(iSecond>=60)iSecond=59;

	CTime today(iYear,iMonth, iDay, iHour, iMinute, iSecond);

	tm = today;
}


CString TimeToString(CTime tm, ETimeFormat format)
{
	
	SYSTEMTIME  SystemTime;
	TCHAR	cDate[100]={0};
	TCHAR	cTime[100]={0};
	TCHAR	cTemp[100]={0};
	TCHAR	cReturn[100]={0};
	CString strTemp = _T("");
	int		i=0;

	tm.GetAsSystemTime(SystemTime);
	switch(format)
	{
		case TIMEF_YMD_HM:
			if(!GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cDate, sizeof(cDate)))
			{
				break;
			}

			
			if(!GetTimeFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cTemp, sizeof(cTemp)))
			{
				break;
			}

			strTemp = cTemp;
			i = strTemp.ReverseFind(':');
			
			if(i<0) break;
			cTime[0] = ' ';
			memcpy(&cTime[1],&cTemp,i);

			//HH:MM AM
			if(lstrlen(cTemp) != 8)
			{
				memcpy(&cTime[1],&cTemp,i);
				lstrcat(cTime,&cTemp[i+3]);
			}
			break;

		case TIMEF_YMD: //date
			if(!GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cDate, sizeof(cDate)))
			{
				break;
			}

		
			break;
		case TIMEF_HM: //time HH:MM AM
			if(!GetTimeFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cTemp, sizeof(cTemp)))
			{
				break;
			}

			if(lstrlen(cTemp) == 8)
			{
				lstrcpy(cTime,cTemp);
			}
			else
			{
				memcpy(cTime,cTemp,5);
				lstrcat(cTime,&cTemp[8]);
			}
			break;
		case TIMEF_HMS: //time HH:MM:SS AM
			if(!GetTimeFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cTime, sizeof(cTime)))
			{
				break;
			}

			break;

		case TIMEF_YM: //date Year/Month
			if(!GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cTemp, sizeof(cTemp)))
			{
				break;
			}

			strTemp = cTemp;

			i = strTemp.ReverseFind('-');

			if(i >= 0)
			{
				memcpy(cDate, cTemp,i);
			}
			else 
			{
				i = strTemp.Find('/');

				if(i >= 0)
				{
					memcpy(cDate, cTemp, i+1);
				}

				int j = strTemp.ReverseFind('/');

				if(j >= 0)
				{

				//	strcat(cDate, &cTemp[j+1]);
					memcpy(cDate,cTemp,j);
				}

			}


			break;

		 case TIMEF_YMD_HMS:
		 default:
			if(!GetDateFormat(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cDate, sizeof(cDate)))
			{
				break;
			}

			
			if(!GetTimeFormat(LOCALE_USER_DEFAULT, LOCALE_NOUSEROVERRIDE , &SystemTime, NULL, cTemp, sizeof(cTemp)))
			{
				break;
			}
			_stprintf(cTime,TEXT(" %s"), cTemp);
			break;

	}

	_stprintf(cReturn,TEXT("%s%s"), cDate, cTime);

	return cReturn;
}

///转换联通时间
CTime TimeFromCDMA(const CString& strCDMA)
{
	// CDMA时间格式 YY/MM/DD,hh:mm:sec
	int iYear=1, iMonth=1, iDay=1, iHour=0, iMinute=0,	iSecond=0;
	const char* p = ToMultiBytes(LPCTSTR(strCDMA)).c_str();
	const char* pEnd = p+strCDMA.GetLength();

	std::string str;
	for(;p<pEnd && *p!='/'; ++p) str += *p;
	iYear = atoi(str.c_str());

	str = "";
	for(++p;p<pEnd && *p!='/'; ++p) str += *p;
	iMonth = atoi(str.c_str());

	str = "";
	for(++p;p<pEnd && *p!=','; ++p) str += *p;
	iDay = atoi(str.c_str());

	str = "";
	for(++p;p<pEnd && *p!=':' && *p!=' '; ++p) str += *p;
	iHour = atoi(str.c_str());

	str = "";
	for(++p;p<pEnd && *p!=':'; ++p) str += *p;
	iMinute = atoi(str.c_str());
	
	str = "";
	for(++p;p<pEnd && *p!='.'; ++p) str += *p;
	iSecond = atoi(str.c_str());

	if(iYear < 70)
		iYear += 2000;
	else if(iYear >= 70 && iYear<100)
		iYear += 1900;

	if(iMonth<=0)iMonth=1;
	if(iMonth>12)iMonth=12;
	if(iDay<=0)iDay=1;
	if(iDay>31)iDay=31;
	if(iHour<=0)iHour=0;
	if(iHour>=24)iHour=23;
	if(iMinute<=0)iMinute=0;
	if(iMinute>=60)iMinute=59;
	if(iSecond<=0)iSecond=0;
	if(iSecond>=60)iSecond=59;

	return CTime(iYear,iMonth, iDay, iHour, iMinute, iSecond);
}

void TimeFromCDMA(const CString& strCDMA, CTime& tm)
{
	int iYear,iMonth, iDay, iHour, iMinute, iSecond;
	_stscanf(strCDMA, TEXT("%d/%d/%d, %d:%d:%d"), &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
				
	if(iYear >= 70 && iYear<100)
	{
		iYear += 1900;
	}
	else if(iYear < 70)
	{
		iYear += 2000;	
	}
	if(iMonth<=0)iMonth=1;
	if(iMonth>12)iMonth=12;
	if(iDay<=0)iDay=1;
	if(iDay>31)iDay=31;
	if(iHour<=0)iHour=0;
	if(iHour>=24)iHour=23;
	if(iMinute<=0)iMinute=0;
	if(iMinute>=60)iMinute=59;
	if(iSecond<=0)iSecond=0;
	if(iSecond>=60)iSecond=59;

	CTime today(iYear,iMonth, iDay, iHour, iMinute, iSecond);

	tm = today;
}

bool FindString(const CString & str_info,const CString & str_find)
{
	int i = 0;
	int j = 0;
	int len_info = 0;
	int len_find = 0;

	len_info = str_info.GetLength();
	len_find = str_find.GetLength();

	for  (i = 0; i<len_info; i++)
	{
		if (str_info.GetAt(i) == str_find.GetAt(j))
		{
			if ( len_find - 1 == j )
			{
				return true;
			}
			else
			{
				j ++;
			}
		}
		else
		{
			j = 0;
		}
	}
	return false;
}


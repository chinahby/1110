#ifndef PCMCIA_UTIL_CE562D0D5A624c3686FF65F98493055C
#define PCMCIA_UTIL_CE562D0D5A624c3686FF65F98493055C

#include <vector>
//#define CN_SUNNYCAT


std::string ToMultiBytes(const std::wstring& str, UINT codepage = 0 );
std::string ToMultiBytes(const wchar_t* str, size_t charcount, UINT codepage = 0 );
std::string BytesToString(BYTE* bytes, size_t size, bool bUnicode);
std::wstring ToUnicode(const std::string& str, UINT codepage = 0 );

CTime TimeFromCDMA(const CString& strCDMA);

void TimeFromCDMA(const CString& strCDMA, CTime& tm);

//获取应用程序所在的目录，包含目录后的'\'
CString GetAppPath();

//秒格式化成"分:秒"形式的字符串
CString SecondToTime(int iSecond);

//装载字符串资源
CString LoadStringEx(UINT id);

//显示对话框，并设置其消息字符串
int ShowMessageBox(UINT strid, UINT nType = MB_OK);

//创建一个线程
HANDLE CreateThreadEx(LPTHREAD_START_ROUTINE func, void* param);


//计算字符串中的字符个数，中英文都算一个字符
size_t GetCharNum(const CString& str);

//寻找对应的字串
bool FindString(const CString& str_info, const CString& str_find);

/**
*	字符串类型。
*/
enum EStringType
{
	STR_DOUBLE_BYTE = 0,//双字节字符
	STR_ASCII,			//单字节字符,且编码范围在[0, 127]之间
	STR_SINGLE_BYTE		//单字节字符,且含编码范围在[0, 127]之外
};

int MaxSMSLength(EStringType strType);

//获取字符串的类型
EStringType GetStringType(const CString& str);
EStringType GetStringType(const char* str, size_t strLen);
void TimeFromString(const CString& strCDMA, CTime& tm);
//获取字符串的左边若干个字符组成的子串
CString GetLeftString(const CString& str, size_t iCount);

//判断是否为UNICODE
bool IsUnicode(const char *p, int len);

void UnicodeToASCII(LPCWSTR SourceStr, DWORD dwWCharCount, LPSTR pASCII, DWORD dwASCIISize);

int ASCIIToUnicode(LPCSTR ASC, DWORD ASCCount,	LPWSTR WCharBuf, DWORD dwWCharSize);

//
//将Uncide的高字节与低字节反转
//
void ReverseUnicode(BYTE *pUnicodem, int iUnicodeNum);

//
//	运行一个命令行
//
HANDLE RunProgram(const CString& strCmdLine, bool bShowWindow);

enum Win32Type{ Win_32s = 1,
		Win_NT3,
		Win_NT4,
		Win_95,
		Win_98,
		Win_ME,
		Win_2000 = 0xF,
		Win_XP,
		Win_2003,
		Win_Vista	};
DWORD OptionsShellType();

//根据规则匹配
bool MathingNumByRules(const CString& strNum1, const CString& strNum2);


enum ETimeFormat
{
	TIMEF_YMD_HM,
	TIMEF_YMD_HMS,
	TIMEF_YMD,
	TIMEF_HM,
	TIMEF_HMS,
	TIMEF_YM,
};
CString TimeToString(CTime tm, ETimeFormat format);
#endif

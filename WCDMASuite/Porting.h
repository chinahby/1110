#ifndef PORTING_H_F1A2F2A4BC8441bdA0B5B71F6B76034D
#define PORTING_H_F1A2F2A4BC8441bdA0B5B71F6B76034D

#ifdef PORTING_EXPORTS
#define PORTING_API __declspec(dllexport)
#else
#define PORTING_API __declspec(dllimport)
#endif

//本文件包含了所有需要移植的函数或变量

PORTING_API
const CString& GetConfigString();

PORTING_API
const CString& GetModemConfigString();

PORTING_API
const CString& GetModemName();

PORTING_API
const CString& GetModemComName();

PORTING_API
const CString& GetConnectionName();

PORTING_API
const CString& GetAppID();

PORTING_API
const CString& GetSplashBMPFileName();

PORTING_API
const CString& GetEncryptFile();


PORTING_API
const CString& GetAppName();

//PORTING_API
//DWORD OptionsDetectPortOn2K();

//PORTING_API
//void ChangePortSetting(DWORD nBaud);

PORTING_API 
const CString& GetHardwareID(int index);

#endif

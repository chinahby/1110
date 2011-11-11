#include "StdAfx.h"
#include "Wininet.h"
#include "Mmsystem.h"
#include "Update.h"
#include "GlobalDataCenter.h"

CUpdateDownload::CUpdateDownload(HWND hWnd):
m_hWnd(hWnd)
{

}

CUpdateDownload::~CUpdateDownload()
{

}

/*
*	原作者：依星(airen3339@163.com)
*/
BOOL CUpdateDownload::InternetGetFile(CString strUrl, CString strFileName,CEvent& EventStopUpdate)
{
	DWORD dwFlags;
	InternetGetConnectedState(&dwFlags, 0);
	CHAR strAgent[64];
	memset(strAgent,0,sizeof(strAgent));
	sprintf(strAgent, "Agent%ld", timeGetTime());
	HINTERNET hOpen;
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hOpen)
	{
		return FALSE;
	}

	//DWORD dwSize;
	TCHAR   szHead[] = TEXT("Accept: */*\r\n\r\n");
	VOID* szTemp[4096];//16384
	HINTERNET  hConnect;
	CFile file;

	INTERNET_BUFFERS InternetBuffer = {0};
	BYTE buffer[4096] = {0};

	if ( !(hConnect = InternetOpenUrl ( hOpen, strUrl, szHead,
		 lstrlen (szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		InternetCloseHandle(hOpen);
	    return FALSE;
	}

	if  (file.Open(strFileName,CFile::modeWrite|CFile::modeCreate)==FALSE )
	{
	  return FALSE;
	}

	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;
	DWORD dwTotalBytes = 0;
	DWORD dwTotalDlBytes = 0;
	size_t nPercent = 0;
	DWORD lBytesReaded=0;  

    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                  (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}

	memset(szTemp,0,sizeof(szTemp));
	do
	{
		if(WaitForSingleObject(EventStopUpdate,0) == WAIT_OBJECT_0)	//用户要求停止更新
		{
			file.Close();
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hOpen);

			return FALSE;
		}
		try
		{			
			InternetBuffer.dwStructSize = sizeof(INTERNET_BUFFERS);
			InternetBuffer.lpvBuffer = buffer;
			InternetBuffer.dwBufferLength = 4096;
			
			::InternetReadFileEx(hConnect,&InternetBuffer,IRF_ASYNC,0);
		}
		catch(...)
		{
			file.Close();
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hOpen);

			return FALSE;
		}
		
		if(InternetBuffer.dwBufferLength == 0)
			break;
		else
		{

			file.Write(buffer,InternetBuffer.dwBufferLength);
			memset(buffer,0,4096);   
			lBytesReaded   +=   InternetBuffer.dwBufferLength;   
			InternetBuffer.dwOffsetLow   =   lBytesReaded;   


			dwTotalBytes += InternetBuffer.dwBufferLength;
			dwTotalDlBytes += InternetBuffer.dwBufferLength;

			if(dwTotalBytes >dwByteToRead/100)
			{
				nPercent++;
				::PostMessage(m_hWnd,WM_UPDATE_SETPROGRESS,nPercent,100);
				dwTotalBytes = dwTotalBytes%(dwByteToRead/100);
			}


		}
		
	}while (TRUE);

	file.Close();
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hOpen);

	return (dwTotalDlBytes >= dwByteToRead);
}
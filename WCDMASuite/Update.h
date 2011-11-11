#pragma once

#include <Afxmt.h>

class CUpdateDownload  
{
public:
	CUpdateDownload(HWND hWnd);
	~CUpdateDownload();
	BOOL InternetGetFile(CString strUrl, CString strFileName,CEvent& EventStopUpdate);	
private:
	HWND m_hWnd;
};
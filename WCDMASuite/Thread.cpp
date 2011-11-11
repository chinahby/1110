//////////////////////////////////////////////////////////////////////
//
// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include "Thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
CThread::CThread()
:m_hThread(NULL)
{

}

//--------------------------------------------------------------------
CThread::~CThread()
{
	if(m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

//--------------------------------------------------------------------
bool CThread::Start()
{
	if(IsRunning())	
		return true;

	if(m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_hThread = ::CreateThread(
						NULL,			// SD
						0,				// initial stack size
						ThreadProc,		// thread function
						this,			// thread argument
						0,				// start immediately
						&m_dwThreadID);	// thread identifier

	return m_hThread != NULL;
}

//--------------------------------------------------------------------
DWORD WINAPI CThread::ThreadProc(LPVOID lpParameter)
{
	CThread* pThread=reinterpret_cast<CThread*>(lpParameter);
	if(pThread)
	{
		pThread->Go();
	}

	return 0;
}

//--------------------------------------------------------------------
bool CThread::IsRunning()const
{
	if(!m_hThread)return false;

	return WAIT_TIMEOUT == ::WaitForSingleObject(m_hThread,0);
}

//--------------------------------------------------------------------
HANDLE CThread::handle()const
{
	return m_hThread;
}

//--------------------------------------------------------------------
DWORD CThread::threadID()const
{
	return m_dwThreadID;
}

//--------------------------------------------------------------------
DWORD CThread::WaitForComplete(DWORD dwMilliseconds)
{
	return ::WaitForSingleObject(m_hThread,dwMilliseconds);
}

//////////////////////////////////////////////////////////////////////
//
// CThread class:
//		This is a WIN32 thread class
//
//		BY GuanXP@Dragontec.com.cn,2002.10
//--------------------------------------------------------------------
//NOTE:
//		The destructor is not thread safe.If the thread is running,
//		the client must call ::WaitForSingleObject(handle()) to wait
//		the thread end	before the CThread object go out of its life
//		scope .
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_THREAD_H__6C519E86_53EA_42A5_BF64_F613899980A9__INCLUDED_
#define AFX_THREAD_H__6C519E86_53EA_42A5_BF64_F613899980A9__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CThread  
{
public:
	/**
	*
	*	constructor
	*
	*/
	CThread();

	/**
	*
	*	destructor
	*
	*/
	virtual ~CThread();

	/**
	*
	*	Start this thread
	*
	*@return 
	*	if the thread is running or thread 
	*	creation success,return true,return
	*	false otherwise.
	*
	*/
	bool Start();

	/**
	*
	*	Check if this thread is running
	*
	*@return 
	*	if the thread is running,return true,
	*	otherwise return false.
	*
	*/
	bool IsRunning()const;

	/**
	*
	*	get the thread handle,client can call 
	*	::WaitForSingleObject(handle()) to 
	*	wait the thread to end.
	*NOTE:
	*	You can NOT call CloseHandle() with this 
	*	function's return value.
	*
	*/
	HANDLE handle()const;
	/**
	*
	*	Get teh current thread ID
	*
	*/
	DWORD threadID()const;

	/**
	*	Wait the thread complete
	*/
	DWORD WaitForComplete(DWORD dwMilliseconds = INFINITE);
	
protected:
	/**
	*
	*	thread routine,subclass should override it
	*
	*/
	virtual void Go()=0;

private:
	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

	//
	//	Forbid access to copy constructor or operator=
	//
	CThread(const CThread&);
	CThread& operator=(const CThread&);
private:
	HANDLE m_hThread;	///< Thread handle
	DWORD  m_dwThreadID;///< Thread ID
};

#endif 

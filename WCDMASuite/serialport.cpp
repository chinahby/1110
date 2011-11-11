/*
Module : SERIALPORT.CPP
Purpose: Implementation for an MFC wrapper class for serial ports
Created: PJN / 31-05-1999
History: PJN / 03-06-1999 1. Fixed problem with code using CancelIo which does not exist on 95.
                          2. Fixed leaks which can occur in sample app when an exception is thrown
         PJN / 16-06-1999 1. Fixed a bug whereby CString::ReleaseBuffer was not being called in 
                             CSerialException::GetErrorMessage
         PJN / 29-09-1999 1. Fixed a simple copy and paste bug in CSerialPort::SetDTR

Copyright (c) 1999 by PJ Naughter.  
All rights reserved.

*/

/////////////////////////////////  Includes  //////////////////////////////////
#include "StdAfx.h"
#include "serialport.h"
#include "winerror.h"
#include <sstream>
#include "GlobalDataCenter.h"

#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif

//////////////////////////////// Implementation ///////////////////////////////



//Class which handles CancelIo function which must be constructed at run time
//since it is not imeplemented on NT 3.51 or Windows 95. To avoid the loader
//bringing up a message such as "Failed to load due to missing export...", the
//function is constructed using GetProcAddress. The CSerialPort::CancelIo 
//function then checks to see if the function pointer is NULL and if it is it 
//throws an exception using the error code ERROR_CALL_NOT_IMPLEMENTED which
//is what 95 would have done if it had implemented a stub for it in the first
//place !!

class _SERIAL_PORT_DATA
{
public:
//Constructors /Destructors
  _SERIAL_PORT_DATA();
  ~_SERIAL_PORT_DATA();

  HINSTANCE m_hKernel32;
  typedef BOOL (CANCELIO)(HANDLE);
  typedef CANCELIO* LPCANCELIO;
  LPCANCELIO m_lpfnCancelIo;
};

_SERIAL_PORT_DATA::_SERIAL_PORT_DATA()
{
  m_hKernel32 = LoadLibrary(TEXT("KERNEL32.DLL"));
  m_lpfnCancelIo = (LPCANCELIO) GetProcAddress(m_hKernel32, "CancelIo");
}

_SERIAL_PORT_DATA::~_SERIAL_PORT_DATA()
{
  FreeLibrary(m_hKernel32);
  m_hKernel32 = NULL;
}


//The local variable which handle the function pointers

_SERIAL_PORT_DATA _SerialPortData;




////////// Exception handling code

void AfxThrowSerialException(DWORD dwError /* = 0 */)
{
	if (dwError == 0)
		dwError = ::GetLastError();

	throw CSerialException(dwError);
}

static BOOL GetErrorMessage(
							DWORD dwError, 
							LPSTR pstrError, 
							UINT nMaxError, 
							PUINT pnHelpContext)
{
	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	LPSTR lpBuffer;
	BOOL bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                      NULL,  dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                      (LPTSTR) &lpBuffer, 0, NULL);

	if (bRet == FALSE)
		*pstrError = '\0';
	else
	{
		strncpy(pstrError, lpBuffer, nMaxError);
		bRet = TRUE;

		LocalFree(lpBuffer);
	}

	return bRet;
}



CSerialException::CSerialException(DWORD dwError)
{
	char buffer[1024];
	GetErrorMessage(dwError, buffer, sizeof(buffer), NULL);
	m_ErrMsg = buffer;
}

CSerialException::~CSerialException()throw()
{
}

const char *CSerialException::what() const throw()
{
	return m_ErrMsg.c_str();
}

////////// The actual serial port code

CSerialPort::CSerialPort()
{
  m_hComm = INVALID_HANDLE_VALUE;
  m_bOverlapped = FALSE;
}

CSerialPort::~CSerialPort()
{
  Close();
}

bool CSerialPort::Open(const char* portname, DWORD dwBaud, Parity parity, BYTE DataBits, StopBits stopbits, FlowControl fc, BOOL bOverlapped)
{
	ASSERT(portname);
	//Call CreateFile to open up the comms port
	HANDLE hCom;//20070517
	hCom = CreateFileA(portname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, bOverlapped ? FILE_FLAG_OVERLAPPED : 0, NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		//DWORD errorCode = GetLastError();
		//AfxThrowSerialException();
		if(m_hComm == INVALID_HANDLE_VALUE)
		{
			//GMessageBox("´ò¿ª´®¿ÚÊ§°Ü£¡");
			return false;
		}		
	}
	else
	{
		m_hComm = hCom;
	}
	m_bOverlapped = bOverlapped;
	
	//Get the current state prior to changing it
	DCB dcb;
	GetState(dcb);
	
	//Setup the baud rate
	dcb.BaudRate = dwBaud; 
	
	//Setup the Parity
	switch (parity)
	{
    case EvenParity:  dcb.Parity = EVENPARITY;  break;
    case MarkParity:  dcb.Parity = MARKPARITY;  break;
    case NoParity:    dcb.Parity = NOPARITY;    break;
    case OddParity:   dcb.Parity = ODDPARITY;   break;
    case SpaceParity: dcb.Parity = SPACEPARITY; break;
    default:          ASSERT(FALSE);            break;
	}
	
	//Setup the data bits
	dcb.ByteSize = DataBits;
	
	//Setup the stop bits
	switch (stopbits)
	{
    case OneStopBit:           dcb.StopBits = ONESTOPBIT;   break;
    case OnePointFiveStopBits: dcb.StopBits = ONE5STOPBITS; break;
    case TwoStopBits:          dcb.StopBits = TWOSTOPBITS;  break;
    default:                   ASSERT(FALSE);               break;
	}
	
	//Setup the flow control 
	dcb.fDsrSensitivity = FALSE;
	switch (fc)
	{
    case NoFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
    case CtsRtsFlowControl:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
    case CtsDtrFlowControl:
		{
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
    case DsrRtsFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
    case DsrDtrFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = TRUE;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			break;
		}
    case XonXoffFlowControl:
		{
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fOutX = TRUE;
			dcb.fInX = TRUE;
			dcb.XonChar = 0x11;
			dcb.XoffChar = 0x13;
			dcb.XoffLim = 100;
			dcb.XonLim = 100;
			break;
		}
    default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	
	//Now that we have all the settings in place, make the changes
	SetState(dcb);
	return true;
}

void CSerialPort::Close()
{
  if (IsOpen())
  {
	  if(m_hComm == INVALID_HANDLE_VALUE)
	  {
		return;
	  }
	  else
	  {
		BOOL bSuccess = CloseHandle(m_hComm);
		if(bSuccess)
		{
			m_hComm = INVALID_HANDLE_VALUE; 
			m_bOverlapped = FALSE;
		}
		else
		{
			Sleep(1000);
			BOOL bSecondTestOK = CloseHandle(m_hComm);
			if(bSecondTestOK)
			{
				m_hComm = INVALID_HANDLE_VALUE; 
				m_bOverlapped = FALSE;
			}
		}

	  }	 
  }
}

void CSerialPort::Attach(HANDLE hComm)
{
  Close();
  m_hComm = hComm;  
}

HANDLE CSerialPort::Detach()
{
  HANDLE hrVal = m_hComm;
  m_hComm = INVALID_HANDLE_VALUE;
  return hrVal;
}

DWORD CSerialPort::Read(void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());
  ASSERT(!m_bOverlapped);

  DWORD dwBytesRead = 0;
  if (!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, NULL))
  {
    AfxThrowSerialException();
  }

  return dwBytesRead;
}

BOOL CSerialPort::Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
  if(!IsOpen() || !m_bOverlapped || !overlapped.hEvent)
  {
	  AfxThrowSerialException();
  }

  DWORD dwBytesRead = 0;
  BOOL bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &overlapped);
  if (!bSuccess)
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      AfxThrowSerialException();
    }
  }
  return bSuccess;
}

DWORD CSerialPort::Write(const void* lpBuf, DWORD dwCount)
{
  if(!IsOpen() || m_bOverlapped)
  {
	  AfxThrowSerialException(ERROR_WRITE_FAULT);
  }

  DWORD dwBytesWritten = 0;
  if (!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, NULL))
  {
    AfxThrowSerialException();
  }

  return dwBytesWritten;
}

BOOL CSerialPort::Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped)
{
  if(!IsOpen() || !m_bOverlapped || !overlapped.hEvent)
  {
	  AfxThrowSerialException(ERROR_WRITE_FAULT);
  }

  DWORD dwBytesWritten = 0;
  BOOL bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &overlapped);
  if (!bSuccess)
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      AfxThrowSerialException();
    }
  }
  return bSuccess;
}

void CSerialPort::GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait)
{
  if(!m_bOverlapped || !overlapped.hEvent ||!IsOpen())
  {
	  dwBytesTransferred = 0;
	  return;
  }
	
  DWORD dwBytesWritten = 0;
  if (!::GetOverlappedResult(m_hComm, &overlapped, &dwBytesTransferred, bWait))
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      AfxThrowSerialException();
    }
  }
}

void CSerialPort::_OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped)
{
  //Validate our parameters
  ASSERT(lpOverlapped);

  //Convert back to the C++ world
  CSerialPort* pSerialPort = (CSerialPort*) lpOverlapped->hEvent;

  //Call the C++ function
  pSerialPort->OnCompletion(dwErrorCode, dwCount, lpOverlapped);
}

void CSerialPort::OnCompletion(DWORD /*dwErrorCode*/, DWORD /*dwCount*/, LPOVERLAPPED lpOverlapped)
{
  //Just free up the memory which was previously allocated for the OVERLAPPED structure
  delete lpOverlapped;

  //Your derived classes can do something useful in OnCompletion, but don't forget to
  //call CSerialPort::OnCompletion to ensure the memory is freed up
}

void CSerialPort::CancelIo()
{
  ASSERT(IsOpen());

  if (_SerialPortData.m_lpfnCancelIo == NULL)
  {
    AfxThrowSerialException(ERROR_CALL_NOT_IMPLEMENTED);  
  }

  if (!::_SerialPortData.m_lpfnCancelIo(m_hComm))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::WriteEx(const void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());

  OVERLAPPED* pOverlapped = new OVERLAPPED;
  ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
  pOverlapped->hEvent = (HANDLE) this;
  if (!WriteFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
  {
    delete pOverlapped;
    AfxThrowSerialException();
  }
}

void CSerialPort::ReadEx(void* lpBuf, DWORD dwCount)
{
  ASSERT(IsOpen());

  OVERLAPPED* pOverlapped = new OVERLAPPED;
  ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
  pOverlapped->hEvent = (HANDLE) this;
  if (!ReadFileEx(m_hComm, lpBuf, dwCount, pOverlapped, _OnCompletion))
  {
    delete pOverlapped;
    AfxThrowSerialException();
  }
}

void CSerialPort::TransmitChar(char cChar)
{
  ASSERT(IsOpen());

  if (!TransmitCommChar(m_hComm, cChar))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetConfig(COMMCONFIG& config)
{
  ASSERT(IsOpen());

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!GetCommConfig(m_hComm, &config, &dwSize))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetConfig(COMMCONFIG& config)
{
  ASSERT(IsOpen());

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!SetCommConfig(m_hComm, &config, dwSize))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetBreak()
{
  ASSERT(IsOpen());

  if (!SetCommBreak(m_hComm))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearBreak()
{
  ASSERT(IsOpen());

  if (!ClearCommBreak(m_hComm))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearError(DWORD& dwErrors)
{
  ASSERT(IsOpen());

  if (!ClearCommError(m_hComm, &dwErrors, NULL))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetDefaultConfig(int nPort, COMMCONFIG& config)
{
  //Validate our parameters
  ASSERT(nPort>0 && nPort<=255);

  //Create the device name as a string

  std::stringstream sPort;
  sPort<<"COM"<<nPort;

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!GetDefaultCommConfigA(sPort.str().c_str(), &config, &dwSize))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetDefaultConfig(int nPort, COMMCONFIG& config)
{
  //Validate our parameters
  ASSERT(nPort>0 && nPort<=255);

  //Create the device name as a string
  std::stringstream sPort;
  sPort<<"COM"<<nPort;

  DWORD dwSize = sizeof(COMMCONFIG);
  if (!SetDefaultCommConfigA(sPort.str().c_str(), &config, dwSize))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetStatus(COMSTAT& stat)
{
  ASSERT(IsOpen());

  DWORD dwErrors;
  if (!ClearCommError(m_hComm, &dwErrors, &stat))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetState(DCB& dcb)
{
  ASSERT(IsOpen());

  if (!GetCommState(m_hComm, &dcb))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetState(DCB& dcb)
{
  ASSERT(IsOpen());

  if (!SetCommState(m_hComm, &dcb))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::Escape(DWORD dwFunc)
{
  ASSERT(IsOpen());

  if (!EscapeCommFunction(m_hComm, dwFunc))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::ClearDTR()
{
  Escape(CLRDTR);
}

void CSerialPort::ClearRTS()
{
  Escape(CLRRTS);
}

void CSerialPort::SetDTR()
{
  Escape(SETDTR);
}

void CSerialPort::SetRTS()
{
  Escape(SETRTS);
}

void CSerialPort::SetXOFF()
{
  Escape(SETXOFF);
}

void CSerialPort::SetXON()
{
  Escape(SETXON);
}

void CSerialPort::GetProperties(COMMPROP& properties)
{
  ASSERT(IsOpen());

  if (!GetCommProperties(m_hComm, &properties))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetModemStatus(DWORD& dwModemStatus)
{
  ASSERT(IsOpen());

  if (!GetCommModemStatus(m_hComm, &dwModemStatus))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetMask(DWORD dwMask)
{
  ASSERT(IsOpen());

  if (!SetCommMask(m_hComm, dwMask))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetMask(DWORD& dwMask)
{
  ASSERT(IsOpen());

  if (!GetCommMask(m_hComm, &dwMask))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::Flush()
{
  ASSERT(IsOpen());

  if (!FlushFileBuffers(m_hComm))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::Purge(DWORD dwFlags)
{
  ASSERT(IsOpen());

  if (!PurgeComm(m_hComm, dwFlags))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::TerminateOutstandingWrites()
{
  Purge(PURGE_TXABORT);
}

void CSerialPort::TerminateOutstandingReads()
{
  Purge(PURGE_RXABORT);
}

void CSerialPort::ClearWriteBuffer()
{
  Purge(PURGE_TXCLEAR);
}

void CSerialPort::ClearReadBuffer()
{
  Purge(PURGE_RXCLEAR);
}

void CSerialPort::Setup(DWORD dwInQueue, DWORD dwOutQueue)
{
  ASSERT(IsOpen());

  if (!SetupComm(m_hComm, dwInQueue, dwOutQueue))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::SetTimeouts(COMMTIMEOUTS& timeouts)
{
  ASSERT(IsOpen());

  if (!SetCommTimeouts(m_hComm, &timeouts))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::GetTimeouts(COMMTIMEOUTS& timeouts)
{
  ASSERT(IsOpen());

  if (!GetCommTimeouts(m_hComm, &timeouts))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::Set0Timeout()
{
  COMMTIMEOUTS Timeouts;
  ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::Set0WriteTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.WriteTotalTimeoutMultiplier = 0;
  Timeouts.WriteTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::Set0ReadTimeout()
{
  COMMTIMEOUTS Timeouts;
  GetTimeouts(Timeouts);
  Timeouts.ReadIntervalTimeout = MAXDWORD;
  Timeouts.ReadTotalTimeoutMultiplier = 0;
  Timeouts.ReadTotalTimeoutConstant = 0;
  SetTimeouts(Timeouts);
}

void CSerialPort::WaitEvent(DWORD& dwMask)
{
  ASSERT(IsOpen());
  ASSERT(!m_bOverlapped);

  if (!WaitCommEvent(m_hComm, &dwMask, NULL))
  {
    AfxThrowSerialException();
  }
}

void CSerialPort::WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped)
{
  ASSERT(IsOpen());
  ASSERT(m_bOverlapped);
  ASSERT(overlapped.hEvent);

  if (!WaitCommEvent(m_hComm, &dwMask, &overlapped))
  {
    if (GetLastError() != ERROR_IO_PENDING)
    {
      AfxThrowSerialException();
    }
  }
}

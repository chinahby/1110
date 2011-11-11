// RasSpdM.cpp: implementation of the CRasSpdM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RasSpdM.h"

#ifdef _DEBUG
#undef THIS_FILE
char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TOTALBYTES    8192
#define BYTEINCREMENT 1024

LPTSTR lpNameStrings = NULL;
LPTSTR *lpNamesArray = NULL;
BOOL glo_fFromDynReg = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static DWORD dwTotalXmit = 0;
static DWORD dwTotalRecv = 0;


/*
在9x下通过dyn_data注册表得到速度
*/
DWORD CRasSpdM::GetTotalRecv(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,TEXT("PerfStats\\StatData"),&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,TEXT("Dial-Up Adapter\\TotalBytesRecvd"),NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}

DWORD CRasSpdM::GetTotalXmit(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,TEXT("PerfStats\\StatData"),&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,TEXT("Dial-Up Adapter\\TotalBytesXmit"),NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}

DWORD CRasSpdM::GetPerSecondRecv(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,TEXT("PerfStats\\StatData"),&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,TEXT("Dial-Up Adapter\\BytesRecvd"),NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}

DWORD CRasSpdM::GetPerSecondXmit(void)
{
	HKEY hKey;
	DWORD dwValue=(DWORD)-1;
	if(0==RegOpenKey(HKEY_DYN_DATA,TEXT("PerfStats\\StatData"),&hKey))
	{
		DWORD dwType,dwLen=sizeof(DWORD);
		RegQueryValueEx(hKey,TEXT("Dial-Up Adapter\\BytesXmit"),NULL,&dwType,(BYTE*)&dwValue,&dwLen);
		RegCloseKey(hKey);
	}
	return dwValue;
}


void CRasSpdM::ResetOn9X()
{
	dwTotalXmit = GetTotalXmit() ;
	dwTotalRecv = GetTotalRecv() ;
}

/*****************************************************************
在NT下通过per Data得到
*****************************************************************/

PPERF_OBJECT_TYPE CRasSpdM::FirstObject( PPERF_DATA_BLOCK PerfData )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + 
        PerfData->HeaderLength) );
}

PPERF_OBJECT_TYPE CRasSpdM::NextObject( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + 
        PerfObj->TotalByteLength) );
}

PPERF_INSTANCE_DEFINITION CRasSpdM::FirstInstance( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + 
        PerfObj->DefinitionLength) );
}

PPERF_INSTANCE_DEFINITION CRasSpdM::NextInstance( 
    PPERF_INSTANCE_DEFINITION PerfInst )
{
    PPERF_COUNTER_BLOCK PerfCntrBlk;

    PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + 
        PerfInst->ByteLength);

    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + 
        PerfCntrBlk->ByteLength) );
}

PPERF_COUNTER_DEFINITION CRasSpdM::FirstCounter( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + 
        PerfObj->HeaderLength) );
}

PPERF_COUNTER_DEFINITION CRasSpdM::NextCounter( 
    PPERF_COUNTER_DEFINITION PerfCntr )
{
    return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + 
        PerfCntr->ByteLength) );
}

/*****************************************************************
 *                                                               *
 * Load the counter and object names from the registry to the    *
 * global variable lpNamesArray.                                 *
 *                                                               *
 *****************************************************************/

void CRasSpdM::GetNameStrings( )
{
    HKEY hKeyPerflib;      // handle to registry key
    HKEY hKeyPerflib009;   // handle to registry key
    DWORD dwMaxValueLen;   // maximum size of key values
    DWORD dwBuffer;        // bytes to allocate for buffers
    DWORD dwBufferSize;    // size of dwBuffer
    LPTSTR lpCurrentString; // pointer for enumerating data strings
    DWORD dwCounter;       // current counter index

// Get the number of Counter items.

    RegOpenKeyEx( HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib"),
        0,
        KEY_READ,
        &hKeyPerflib);

    dwBufferSize = sizeof(dwBuffer);

    RegQueryValueEx( hKeyPerflib,
        TEXT("Last Counter"),
        NULL,
        NULL,
        (LPBYTE) &dwBuffer,
        &dwBufferSize );

    RegCloseKey( hKeyPerflib );

// Allocate memory for the names array.

    lpNamesArray = (TCHAR**)malloc( (dwBuffer+1) * sizeof(LPTSTR) );
// Open key containing counter and object names.

    RegOpenKeyEx( HKEY_LOCAL_MACHINE,
    TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009"),
        0,
        KEY_READ,
        &hKeyPerflib009);

// Get the size of the largest value in the key (Counter or Help).

    RegQueryInfoKey( hKeyPerflib009,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &dwMaxValueLen,
        NULL,
        NULL);

// Allocate memory for the counter and object names.

    dwBuffer = dwMaxValueLen + 1;

    lpNameStrings = (TCHAR*)malloc( dwBuffer * sizeof(TCHAR) );

// Read Counter value.

    RegQueryValueEx( hKeyPerflib009,
        TEXT("Counter"),
        NULL,
        NULL,
        (BYTE*)lpNameStrings, &dwBuffer );

// Load names into an array, by index.

    //int iC=1;
	for( lpCurrentString = lpNameStrings; *lpCurrentString;
         lpCurrentString += (lstrlen(lpCurrentString)+1) )
    {
        dwCounter = _ttol( lpCurrentString );
        lpCurrentString += (lstrlen(lpCurrentString)+1);

		//printf("%d length=%d data=%s\n",iC++,dwCounter,lpCurrentString);
        lpNamesArray[dwCounter] = (LPTSTR) lpCurrentString;
    }
}

/*****************************************************************
得到下RAS速度
 *****************************************************************/
void CRasSpdM::GetRASSpeedOn9X(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT)
{
	// under 9x
	*pTPS=GetPerSecondXmit();
	*pRPS=GetPerSecondRecv();
	*pTT=GetTotalXmit();
	*pRT=GetTotalRecv();

	*pTT -= dwTotalXmit;
	*pRT -= dwTotalRecv;

}

void CRasSpdM::GetRASSpeedOnNT(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT)
{// under NT
////参数：每秒发送，每秒接收，总发送，总接受
    PPERF_DATA_BLOCK PerfData = NULL;
    PPERF_OBJECT_TYPE PerfObj;
    PPERF_COUNTER_DEFINITION PerfCntr;
    PPERF_COUNTER_BLOCK PtrToCntr;
    DWORD BufferSize = TOTALBYTES;
    DWORD i, j;

// Get the name strings through the registry.

	//return;
// Allocate the buffer for the performance data.

    PerfData = (PPERF_DATA_BLOCK) malloc( BufferSize );

    while( RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                               TEXT("906"),//"Global",
                               NULL,
                               NULL,
                               (LPBYTE) PerfData,
                               &BufferSize ) == ERROR_MORE_DATA )
    {
    // Get a buffer that is big enough.

        BufferSize += BYTEINCREMENT;
        PerfData = (PPERF_DATA_BLOCK) realloc( PerfData, BufferSize );
    }

// Get the first object type.

    PerfObj = FirstObject( PerfData );

	// Process all objects.
	for( i=0; i < PerfData->NumObjectTypes; i++ )
    {
    // Display the object by index and name.

    // Get the first counter.

        PerfCntr = FirstCounter( PerfObj );

        if( PerfObj->NumInstances > 0 )
        {
			//exit(0);
        }
        else
        {
        // Get the counter block.
            PtrToCntr = (PPERF_COUNTER_BLOCK) ((PBYTE)PerfObj +
                            PerfObj->DefinitionLength );

        // Retrieve all counters.

            for( j=0; j < PerfObj->NumCounters; j++ )
            {
            // Display the counter by index and name.

				if(_tcscmp(TEXT("Bytes Transmitted/Sec"),lpNamesArray[PerfCntr->CounterNameTitleIndex])==0)
				{
					*pTPS=*((DWORD*)((BYTE*)PtrToCntr+PerfCntr->CounterOffset));
				}
				if(_tcscmp(TEXT("Bytes Received/Sec"),lpNamesArray[PerfCntr->CounterNameTitleIndex])==0)
				{
					*pRPS=*((DWORD*)((BYTE*)PtrToCntr+PerfCntr->CounterOffset));
				}
                if(_tcscmp(TEXT("Bytes Transmitted"),lpNamesArray[PerfCntr->CounterNameTitleIndex])==0)
				{
					*pTT=*((DWORD*)((BYTE*)PtrToCntr+PerfCntr->CounterOffset));
				}
                if(_tcscmp(TEXT("Bytes Received"),lpNamesArray[PerfCntr->CounterNameTitleIndex])==0)
				{
					*pRT=*((DWORD*)((BYTE*)PtrToCntr+PerfCntr->CounterOffset));
				}
				// Get the next counter.
				PerfCntr = NextCounter( PerfCntr );
			}
		}

		// Get the next object type.
		PerfObj = NextObject( PerfObj );
    }

	free( PerfData );
}

void CRasSpdM::GetRASSpeed(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT)
{////参数：每秒发送，每秒接收，总发送，总接受
	if(glo_fFromDynReg==FALSE)
	{
		GetRASSpeedOnNT(pTPS,pRPS, pTT,pRT);
	}
	else
	{
		GetRASSpeedOn9X(pTPS,pRPS, pTT,pRT);
	}
}

void CRasSpdM::ResetStatistic()
{
	if(glo_fFromDynReg==FALSE)
	{
	}
	else
	{
		ResetOn9X();
	}
}

void CRasSpdM::InitSpeedMonitor(void)
{
	DWORD dwVersion = GetVersion();
	// Get major and minor version numbers of Windows

	//DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	// Get build numbers for Windows NT or Win32s
	if (dwVersion>>31 ==0)                // Windows NT
	{
		glo_fFromDynReg=FALSE;
		GetNameStrings( );
	}
	else         // Windows 95 - 98
		glo_fFromDynReg=TRUE;
}

void CRasSpdM::ClearSpeedMonitor(void)
{
	if(lpNameStrings)
		free(lpNameStrings);
	if(lpNamesArray)
		free(lpNamesArray);
}

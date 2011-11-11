// RasSpdM.h: interface for the CRasSpdM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASSPDM_H__5909FEC1_D65A_4A95_A446_0A2336A5FF0D__INCLUDED_)
#define AFX_RASSPDM_H__5909FEC1_D65A_4A95_A446_0A2336A5FF0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "winperf.h"

class CRasSpdM  
{
	static void ResetOn9X();
	static DWORD GetTotalRecv(void);
	static DWORD GetTotalXmit(void);
	static DWORD GetPerSecondRecv(void);
	static DWORD GetPerSecondXmit(void);
	static PPERF_OBJECT_TYPE FirstObject( PPERF_DATA_BLOCK PerfData );
	static PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj );
	static PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj );
	static PPERF_INSTANCE_DEFINITION NextInstance( PPERF_INSTANCE_DEFINITION PerfInst );
	static PPERF_COUNTER_DEFINITION FirstCounter( PPERF_OBJECT_TYPE PerfObj );
	static PPERF_COUNTER_DEFINITION NextCounter( PPERF_COUNTER_DEFINITION PerfCntr );
	static void GetNameStrings( );
	static void GetRASSpeedOn9X(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT);
	static void GetRASSpeedOnNT(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT);
public:
	static void ResetStatistic();
	static void GetRASSpeed(DWORD* pTPS,DWORD* pRPS,DWORD* pTT,DWORD* pRT);
	static void InitSpeedMonitor(void);
	static void ClearSpeedMonitor(void);

};

#endif // !defined(AFX_RASSPDM_H__5909FEC1_D65A_4A95_A446_0A2336A5FF0D__INCLUDED_)

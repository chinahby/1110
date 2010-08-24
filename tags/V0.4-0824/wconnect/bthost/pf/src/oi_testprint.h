#ifdef FEATURE_BT_EXTPF_AV

/** @file   
This header file provides the OI_TestPrint macros
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#ifndef _OI_TESTPRINT_H
#define _OI_TESTPRINT_H


#ifdef OI_TEST /*only for test*/
#ifdef WIN32 /*only for windows*/
#include <windows.h> 
#include <stdio.h>
#include "oi_debug.h"
#include "oi_avdtp.h"

/** \addtogroup Debugging Debugging APIs */
/**@{*/

/* Definition for bits to set to enable testprints in a specific module */
#define TEST_BIT(module) (1 << OI_MODULE_##module)

/* The list of bits for configuring the testprint modules */
#define TEST_CONFIG (TEST_BIT(AVDTP) | TEST_BIT(AVCTP))

void OI_AVDTP_PrintHandles(OI_UINT16 *handles, OI_UINT8 numHandles);
void OI_AVDTP_PrintCaps(OI_AVDTP_CAP *caps, OI_UINT8 numCaps);
void OI_AVDTP_PrintSeidData(OI_AVDTP_DISCOVER_RSP_INFO *dataArray, OI_UINT16 arrayLen); 


#define OI_AVDTP_TestPrintHandles(handles, numHandles) OI_AVDTP_PrintHandles((handles), (numHandles))
#define OI_AVDTP_TestPrintCaps(caps, numCaps) OI_AVDTP_PrintCaps((caps), (numCaps))
#define OI_AVDTP_TestPrintSeidData(dataArray, arrayLen) OI_AVDTP_PrintSeidData((dataArray), (arrayLen))

#define OI_TestPrint(msg) \
if (TEST_CONFIG & (1 << __OI_MODULE__)) \
{ \
    SYSTEMTIME time; \
    GetLocalTime(&time); \
    OI_Printf("%02d:%02d:%02d:%d ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds); \
    OI_Printf msg; \
} 
/*#define OI_TestPrint(msg) * #define OI_AVDTP_TestPrint(msg) only print for avdtp, not l2cap */
#define OI_AVDTP_TestPrint(msg) \
{ \
    SYSTEMTIME time; \
    GetLocalTime(&time); \
    printf("%02d:%02d:%02d:%d ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds); \
    printf msg; \
}

#else /* WIN32 */
#define OI_AVDTP_TestPrintHandles(handles, numHandles) 
#define OI_AVDTP_TestPrintCaps(caps, numCaps) 
#define OI_AVDTP_TestPrintSeidData(dataArray, arrayLen)
#define OI_AVDTP_TestPrint(msg)
#define OI_TestPrint(msg)
#endif

#else  /* OI_TEST */

#define OI_AVDTP_TestPrintHandles(handles, numHandles) 
#define OI_AVDTP_TestPrintCaps(caps, numCaps) 
#define OI_AVDTP_TestPrintSeidData(dataArray, arrayLen)
#define OI_AVDTP_TestPrint(msg)
#define OI_TestPrint(msg)
#endif

#endif /* _OI_TESTPRINT_H */

/**@}*/

#endif /* FEATURE_BT_EXTPF_AV */


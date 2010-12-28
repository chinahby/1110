#ifndef AEEBTUTILS_H
#define AEEBTUTILS_H

/*=============================================================================
FILE:  AEEBTUtils.h

SERVICES:  AEE BT Utility APIs
GENERAL DESCRIPTION:

===============================================================================
           Copyright (c) 2007-2009 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTUtils.h#3 $ 
$DateTime: 2009/06/02 13:35:41 $
$Author: c_bhatg $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-01-07  NM   Fix the issue of not setting FTP/OPP/BIP service security 
                 through IBTMgr Interface
2008-11-25  gb   Replaced usage of FEATURE_IBT_DYNAMIC_BMP_1_0 flag with
                 FEATURE_BMP.
2008-10-06  rs   Adding the feature flag to avoid inclucion of features not 
                 yet a part of BREW IBT release.
=============================================================================*/
#include "AEEISignalCtl.h"
#include "AEEBTDef.h"
#ifdef FEATURE_BMP
#include "AEEISysTimer.h"
#endif /* FEATURE_BMP */

#include "AEEBTDef.h"

#define RELEASEIF(x) if (x) { IQI_Release((IQI *)(x)); (x) = 0; }

#ifdef MSG_ERROR 
   #undef MSG_ERROR
#endif
#ifdef MSG_DEBUG 
   #undef MSG_DEBUG
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif

#define IBT_ERROR 1
#define IBT_HIGH  1
// Uncomment these to print
 

#if defined IBT_DEBUG
#define MSG_DEBUG( sf, a, b, c )    DBGPRINTF_HIGH(sf, a, b, c)
#else
#define MSG_DEBUG( sf, a, b, c )
#endif //IBT_DEBUG


#if defined IBT_ERROR
#define MSG_ERROR( sf, a, b, c )    DBGPRINTF_ERROR(sf, a, b, c)
#else
#define MSG_ERROR( sf, a, b, c )
#endif //IBT_ERROR

#if defined IBT_HIGH
#define MSG_HIGH( sf, a, b, c )     DBGPRINTF_HIGH(sf, a, b, c)
#else
#define MSG_HIGH( sf, a, b, c )  
#endif //IBT_HIGH

#if defined IBT_MED
#define MSG_MED( sf, a, b, c )      DBGPRINTF_MED(sf, a, b, c)
#else
#define MSG_MED( sf, a, b, c ) 
#endif ////IBT_MED

#if defined IBT_LOW
#define MSG_LOW( sf, a, b, c )      DBGPRINTF_LOW(sf, a, b, c)
#else
#define MSG_LOW( sf, a, b, c )  
#endif //IBT_LOW

#define AEEBT_MSG_LOW 0
#define AEEBT_MSG_MED 1
#define AEEBT_MSG_HIGH 2
#define AEEBT_MSG_ERROR 3

#define BT_AEE_MGR "BT AEE MGR "
#define BT_MW_MGR "BT MW MGR "
#define BT_OEM_MGR "BT OEM MGR "

#define BT_AEE_SD "BT AEE SD "
#define BT_MW_SD "BT MW SD "
#define BT_OEM_SD "BT OEM SD "


#define AEEBT_PRINT(msg,arg1,arg2,arg3,level) DBGPRINTF(msg,arg1,arg2,arg3)
#ifndef USE_CS
//#define USE_CS
#endif

#ifdef USE_CS
#error code not present
#else
#include "AEEShell.h"
#define IBTEnv IShell
#define AEEBT_CREATEINSTANCE(env,clsid,ppo) ISHELL_CreateInstance(env,clsid,ppo)
#define AEEBT_MALLOC(env,nSize,ppo)         ERR_MALLOC(nSize,ppo)
#define AEEBT_MALLOCREC(env,t,ppo)          ERR_MALLOCREC(t,ppo)
#define AEEBT_REALLOC(env,nSize,ppo)        ERR_REALLOC(nSize,ppo)
#define AEEBT_FREEIF(env,p)                 FREEIF(p)
#define AEEBT_ENVADDREF(env)                ISHELL_AddRef(env)
#define AEEBT_ENVRELEASE(env)               ISHELL_Release(env)
#endif


//=============================================================================
// AEE BT A2DP event queue
//=============================================================================
typedef struct 
{
   uint8       *pBuf;
   int         nIndRead;
   int         nIndWrite;
   int         nMaxCnt;
   int         nMaxEvtSize;
}AEEBTEvtQueue;

int AEEBTEvtQ_Free(AEEBTEvtQueue *pEvtQ);
int AEEBTEvtQ_Get(AEEBTEvtQueue *pEvtQ, uint8 *pEvt );
int AEEBTEvtQ_Put(AEEBTEvtQueue *pEvtQ, uint8 *pEvt);
int AEEBTEvtQ_Init(AEEBTEvtQueue *pEvtQ, int nMaxCnt, int nMaxEvtSize);

static __inline boolean AEEBT_BD_ADDRESS_EQUAL (const BDAddress* p1, 
                                                const BDAddress* p2)
{
   int i = sizeof (BDAddress);
   while (i-- > 0) { if (p1->uAddr[i] != p2->uAddr[i]) return FALSE; }
   return TRUE;
}
//=============================================================================
// Other utility functions
//=============================================================================
AEEResult AEEBT_SetSvcSecurity(
    IBTEnv* pEnv, BTSecurityLevel cSec, BTServiceClassUUID svcClsUUID);

#ifdef FEATURE_BMP
char* AEEBT_WChar2Char(const _wchar_t*, char**, int*);
_wchar_t* AEEBT_Char2WChar(const char*, _wchar_t**, int*);
AEEResult AEEBT_WChar2UTF8(IBTEnv*, const _wchar_t*, int, uint8**, int*);

AEEResult AEEBT_CreateTimer(
   IBTEnv*        pEnv, 
   ISysTimer**    ppTimer,
   ISignalCtl**   ppiSigCtl, 
   void (*pfn)(void *pUser),
   void*          pUser);

void AEEBT_ReleaseTimer(ISysTimer** ppTimer, ISignalCtl** ppiSigCtl);

void AEEBT_StartTimer(ISysTimer* pTimer, ISignalCtl* piSigCtl, int nSeconds);

void AEEBT_CancelTimer(ISysTimer* pTimer);
#endif /* FEATURE_BMP */

#endif /* AEEBTUTILS_H */


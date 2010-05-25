/*=============================================================================
FILE:  AEEBTUtils.C

SERVICES:  AEE BT Utility APIs
GENERAL DESCRIPTION:

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEBTUtils.c#2 $ 
$DateTime: 2009/01/21 20:37:23 $
$Author: c_novinm $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-01-07  NM   Fix the issue of not setting FTP/OPP/BIP service security
                 through IBTMgr Interface
2008-11-25  gb   Replaced usage of FEATURE_IBT_DYNAMIC_BMP_1_0 flag with
                 FEATURE_BMP.
2008-10-06  rs   Adding the feature flag to avoid inclucion of features not 
                 yet a part of BREW IBT release.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "AEE_OEMDispatch.h"
#include "OEMHeap.h"
#include "AEEStd.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEBTUtils.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"

#include "AEE_OEM.h"
#include "AEEBT.h"
#include "AEEBT.bid"
#include "AEEBTMgr.bid"
#include "AEEIBTMgr.h"

#ifdef FEATURE_BMP
#include "AEESysTimer.bid"
#endif /* FEATURE_BMP */

#if defined (BT_SIMULATION)
#include "btmsg.h"
#endif
//=============================================================================
// 
//
//=============================================================================
AEEResult AEEBT_SetSvcSecurity(
             IBTEnv* pEnv,
             BTSecurityLevel cSec, 
             BTServiceClassUUID svcClsUUID)
{    
    BTServiceIdentifier svc2protect;
    IBT*      piBT = NULL;  // IBT interface 
    IBTMgr*   piMgr = NULL; // IBT Manager interface
    AEEResult result = AEE_EFAILED;

    if ( AEEBT_CREATEINSTANCE( pEnv, AEECLSID_BT, (void**)&piBT) != 
         AEE_SUCCESS )
    {
       MSG_ERROR("AEEBTUTIL Set security, failed to create IBT", 0, 0, 0);
    }
    else if ( IBT_GetObject( piBT, AEECLSID_BTMgr, NULL, 0,
                              (void**)&piMgr) != AEE_SUCCESS )
    {
       MSG_ERROR("AEEBTUTIL Set security, failed to create IBTMgr", 0, 0, 0);
    }
    else
    {
       svc2protect.dIDMethod  =  AEEBT_SEC_BY_SERVICE_CLASS;
       svc2protect.nNumItems  =  1;
       svc2protect.dSvcCls[0] =  svcClsUUID;                     

       if ( (result = IBTMgr_SetSvcSecurity( piMgr, cSec, &svc2protect) )
            != AEE_SUCCESS )
       {
          MSG_ERROR("AEEBTUTIL Initiator Set security, failed to set sec level"
                    ,0, 0, 0);
       }
    }

    // Release IBTMgr related pointers
   if (NULL != piBT)
   {
      IBT_Release(piBT);
      piBT = NULL;
   }
   if ( piMgr != NULL )
   {
      IBTMgr_Release (piMgr);
      piMgr = NULL; 
   }
 
   return result;
}

//=============================================================================
// 
//
//=============================================================================

int AEEBTEvtQ_Init(AEEBTEvtQueue *pEvtQ, int nMaxCnt, int nMaxEvtSize)
{
   int ret = AEE_SUCCESS;

   pEvtQ->nIndRead  = 0;
   pEvtQ->nIndWrite = 0;
   pEvtQ->nMaxCnt   = nMaxCnt;
   pEvtQ->nMaxEvtSize = nMaxEvtSize;

   if (0 != nMaxCnt)
   {
      pEvtQ->pBuf = OEM_Malloc(nMaxEvtSize * nMaxCnt); //check can be malloc?
      if (NULL == pEvtQ->pBuf)
      {
         ret = AEE_ENOMEMORY;
      }
   }
   else
   {
      ret = AEE_EFAILED;
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int AEEBTEvtQ_Put(AEEBTEvtQueue *pEvtQ, uint8 *pEvt)
{
   int nIndNextFree = 0;
   int ret = AEE_SUCCESS;

   uint8 *pBuf;

   // copy the contents
   pBuf = pEvtQ->pBuf + (pEvtQ->nIndWrite * pEvtQ->nMaxEvtSize);

   MEMCPY((void*)pBuf, pEvt, pEvtQ->nMaxEvtSize);

   // check if there is space in the queue before incrementing the index
   nIndNextFree = (pEvtQ->nIndWrite + 1) % pEvtQ->nMaxCnt;

   if (nIndNextFree == pEvtQ->nIndRead)
   {
      MSG_ERROR(" AEE  EVT Queue full: Dropping events ",0,0,0); 
      ret = AEE_EFAILED;
   }
   else
   {
      pEvtQ->nIndWrite = nIndNextFree;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEBTEvtQ_Get(AEEBTEvtQueue *pEvtQ, uint8 *pEvt )
{
   int ret = AEE_ENOMORE;
   uint8 *pBuf = NULL;

   if ( pEvtQ->nIndWrite != pEvtQ->nIndRead)
   {
      // copy the contents
      pBuf = pEvtQ->pBuf + (pEvtQ->nIndRead * pEvtQ->nMaxEvtSize);

      MEMCPY((void*)pEvt, (void*)pBuf, pEvtQ->nMaxEvtSize);

      MEMSET((void*)pBuf, 0, pEvtQ->nMaxEvtSize);

      pEvtQ->nIndRead = (pEvtQ->nIndRead + 1) % pEvtQ->nMaxCnt;
      ret = AEE_SUCCESS;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEBTEvtQ_Free(AEEBTEvtQueue *pEvtQ)
{
   if (NULL != pEvtQ->pBuf)
   {
      OEM_Free(pEvtQ->pBuf);
      pEvtQ->pBuf      = NULL;
      pEvtQ->nIndRead  = 0;
      pEvtQ->nIndWrite = 0;
      pEvtQ->nMaxCnt   = 0;
   }
   return AEE_SUCCESS;
}

#ifdef FEATURE_BMP

/*======================================================================= 
   Allocates memory if necessary and converts given wide string to string.
   Caller must free allocated memory when done.
=======================================================================*/
char* AEEBT_WChar2Char(const _wchar_t* pwStr, char** ppszDest, int* pnDestSize)
{
   AEEResult result = AEE_SUCCESS;
   int nLen = std_wstrlen(pwStr) + 1; // room for NULL

   if (*pnDestSize < nLen)
   {
      if (AEE_SUCCESS == (result = ERR_REALLOC(nLen, ppszDest)))
      {
         *pnDestSize = nLen;
      }
   }

   if (AEE_SUCCESS == result)
   {
#if !defined(USE_ICHARSETCONV)
      WSTRTOSTR(pwStr, *ppszDest, nLen);
#else
#error code not present
#endif
   }
   return *ppszDest;

   // caller must free the allocated buffer
}

/*======================================================================= 
   Allocates memory if necessary and converts given string to wide string.
   Caller must free allocated memory when done.
=======================================================================*/
_wchar_t* AEEBT_Char2WChar(
   const char* pszStr, 
   _wchar_t**  ppwDest, 
   int*        pnDestSize)
{
   AEEResult result = AEE_SUCCESS;
   int nwLen = (std_strlen(pszStr) + 1) * sizeof(_wchar_t);

   if (*pnDestSize < nwLen)
   {
      if (AEE_SUCCESS == (result = ERR_REALLOC(nwLen, ppwDest)))
      {
         *pnDestSize = nwLen;
      }
   }

   if (AEE_SUCCESS == result)
   {
#if !defined(USE_ICHARSETCONV)
      STRTOWSTR(pszStr, *ppwDest, nwLen);
#else
#error code not present
#endif
   }
   return *ppwDest;
   // caller must free the allocated buffer
}

/*======================================================================= 
   Allocates memory if necessary and converts given wide string to UTF-8.
   Caller must free allocated memory when done.
=======================================================================*/
AEEResult AEEBT_WChar2UTF8(
   IBTEnv*         pEnv, 
   const _wchar_t* pwStr,
   int             nStrLen,
   uint8**         ppDest, 
   int*            pnDestSize)
{
   AEEResult result = AEE_EBADPARM;

   if ((NULL != pwStr) && (0 < nStrLen))
   {
      nStrLen++; // account for NULL
      if (*pnDestSize < nStrLen)
      {
         if (AEE_SUCCESS == (result = AEEBT_REALLOC(pEnv, nStrLen, ppDest)))
         {
            *pnDestSize = nStrLen;
         }
      }
      if (*pnDestSize >= nStrLen)
      {
         WSTRTOUTF8(pwStr, nStrLen, *ppDest, *pnDestSize);
         (*ppDest)[nStrLen-1] = 0;
         result = AEE_SUCCESS;
      }
   }
   return result;

   // caller must free the allocated buffer
}

//=============================================================================
// Creates an object of type ISysTimer if the interface is available
//
//=============================================================================
AEEResult AEEBT_CreateTimer(
   IBTEnv*        pEnv,
   ISysTimer**    ppTimer,
   ISignalCtl**   ppiSigCtl,
   void (*pfn)(void *pUser),
   void*          pUser
)
{
   AEEResult         result = AEE_ENOMEMORY;
   ISysTimer*        piSysTimer;
   ISignal*          piSig = NULL;
   ISignalCBFactory* piSigCBFactory;

   if ((AEE_SUCCESS == AEEBT_CREATEINSTANCE(pEnv, AEECLSID_SysTimer, 
                                            (void**)&piSysTimer)) &&
       (AEE_SUCCESS == AEEBT_CREATEINSTANCE(pEnv, AEECLSID_SignalCBFactory, 
                                            (void**)&piSigCBFactory)))
   {
      if (AEE_SUCCESS == ISignalCBFactory_CreateSignal(
                            piSigCBFactory, 
                            pfn, 
                            pUser, 
                            &piSig, 
                            ppiSigCtl))
      {
         if (AEE_SUCCESS == ISysTimer_AssociateSignal(piSysTimer, piSig))
         {
            *ppTimer = piSysTimer;
            result = AEE_SUCCESS;
         }
         else
         {
            DBGPRINTF_ERROR("CreateTimer - failed to associate signal with timer");
         }
         ISignal_Release(piSig);
      }
      ISignalCBFactory_Release(piSigCBFactory);
   }

   return result;
}

//=============================================================================
// Releases the timer created earlier
//
//=============================================================================
void AEEBT_ReleaseTimer(ISysTimer** ppTimer, ISignalCtl** ppiSigCtl)
{
   if (NULL != *ppTimer)
   {
      ISysTimer_Cancel(*ppTimer);
      ISysTimer_Release(*ppTimer);
      *ppTimer = NULL;
   }
   if (NULL != *ppiSigCtl)
   {
      ISignalCtl_Release(*ppiSigCtl);
      *ppiSigCtl = NULL;
   }
}

//=============================================================================
// Starts the timer created earlier
//
//=============================================================================
void AEEBT_StartTimer(ISysTimer* pTimer, ISignalCtl* piSigCtl, int nSeconds)
{
   if (NULL != pTimer)
   {
      ISysTimer_SetDelay(pTimer, nSeconds*1000);
      if (NULL != piSigCtl)
      {
         ISignalCtl_Enable(piSigCtl);
         DBGPRINTF("StartTimer - timer set, signal enabled");
      }
   }
}

//=============================================================================
// Stops the timer started earlier
//
//=============================================================================
void AEEBT_CancelTimer(ISysTimer* pTimer)
{
   if (NULL != pTimer)
   {
      ISysTimer_Cancel(pTimer);
   }
}
#endif /* FEATURE_BMP */

#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


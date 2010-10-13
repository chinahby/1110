/*======================================================
FILE:  AEEOBEXCli.c

SERVICES:  Implementation of IOBEXCli interface

GENERAL DESCRIPTION:
   Implements IOBEXCli

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEOBEXCli.c#1 $
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-09-18   pr  Klocwork fixes
  
=============================================================================*/

#include "OEMFeatures.h"
#include "AEEStdLib.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "AEEStdErr.h"
#include "AEEShell.h"
#include "AEEISignalBus.h"
#include "AEEISignalCtl.h"
#include "AEESignalBus.bid"
#include "AEE_OEMDispatch.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#include "OEMHeap.h"
#include "AEEIOBEXHeader.h"

#include "AEEIOBEXCli.h"
#include "AEEOBEXDefs_priv.h"
#include "AEEBTUtils.h"

//=============================================================================
//  Type definitions
//============================================================================= 

//=============================================================================
//  OBEX client states
//=============================================================================
 
#define AEEOBEX_CLI_STATE_CREATED         1
#define AEEOBEX_CLI_STATE_INIT            2
#define AEEOBEX_CLI_STATE_CONN_PEND       3
#define AEEOBEX_CLI_STATE_CONNECTED       4
#define AEEOBEX_CLI_STATE_PUT_PEND        5
#define AEEOBEX_CLI_STATE_GET_PEND        6
#define AEEOBEX_CLI_STATE_SET_PATH_PEND   7
#define AEEOBEX_CLI_STATE_DISC_PEND       8

//=============================================================================
// AEE OBEX client Object created in system memory
//=============================================================================
typedef struct {
   uint32           uRef;
   int              state;
   IOBEXCli         *pCli;
   boolean          bConnOriented;
   boolean          bAbort;
   AEECallback      evSignalCb;
   AEEOBEXEvtQueue  sysEvtQueue;
   uint32           uMultiPacketOp;
   OEMOBEXHdr*      aSendHdr[AEEOBEX_MAX_SEND_BUFF];
   OEMOBEXCli       *pOemObj;
}AEEOBEXCliSys;

//=============================================================================
// AEE OBEX client Object
//=============================================================================
struct IOBEXCli {
   const IOBEXCliVtbl    *pvt;
   uint32                uRef;
   IShell                *pIShell;
   AEECallback           cbAppClose;    // callback that is called by BREW when 
                                        // app terminates
   OEMOBEXAuthInfo       authChal;      // auth chal info received from remote
   boolean               bAuthChal;
   OEMOBEXAuthResp       authResp;      // auth resp info received from remote 
   boolean               bAuthResp;
   uint32                suppTransport; // supported transports
   ISignalBus            *piSigAppEvt;  // signal bus to put the app's iSignal
   AEEOBEXCliSys         *pCliSys;      // pointer to sys memory allocated for
                                        // this object
   OEMOBEXAuthInfo       authReq;       // auth info, in case client initiates auth
   boolean               bAuthReq;
   AEEOBEXEvtQueue       appEvtQueue;   // queue of events to be delivered to app
   uint32                uMaxPacket;
};

//=============================================================================
// OBEX client event to be sent to app
//=============================================================================
typedef struct {
   uint8       event;
   uint32      status;
   OEMOBEXHdr  *pOemHdr;
}AEEOBEXCliEvt;



//=============================================================================
// Interface function definitions
//=============================================================================
static uint32 AEEOBEXCli_AddRef( IOBEXCli *me );
static uint32 AEEOBEXCli_Release( IOBEXCli *me );
static int AEEOBEXCli_QueryInterface( IOBEXCli *me,
                                      AEECLSID iid, void **ppo );
static int AEEOBEXCli_GetSupportedTransports( IOBEXCli *me,
                                              AEEOBEXTransport *pTransport,
                                              int nLen,
                                              int *pnLenReq);
static int AEEOBEXCli_SetTransport( IOBEXCli         *me,
                                    AEEOBEXTransport transport,
                                    const char       *pszTransParams);
static int AEEOBEXCli_Authenticate( IOBEXCli        *me,
                                    AEEOBEXCharSet  charSet,
                                    uint8           *pRealm,
                                    int             nRealmLen,
                                    boolean         uid,
                                    boolean         acs);
static int AEEOBEXCli_SendCmd( IOBEXCli           *me,
                               AEEOBEXRequestType obexReq,
                               IOBEXHeader        *piObexHdr,
                               AEEOBEXFlags       flags,
                               boolean            bFinal);

static int AEEOBEXCli_OnEventNotify(IOBEXCli     *me,
                                    ISignal      *piSignal );
static int AEEOBEXCli_GetEvent(IOBEXCli           *me,
                               AEEOBEXEventType   *pEvent,
                               IOBEXHeader        **ppiHdr,
                               AEEOBEXCmdStatus   *pCmdStatus,
                               AEEOBEXFlags       *pFlags);

static int AEEOBEXCli_SetUidPassword(IOBEXCli         *me,
                                     const uint8      *pUid,
                                     int              nUidLen,
                                     const uint8      *pPswd,
                                     int              nPswdLen);

static int AEEOBEXCli_GetAuthChallenge(IOBEXCli           *me,
                                       boolean            *pbUserIdReq,
                                       boolean            *pbFullAccess,
                                       AEEOBEXCharSet     *pCharSet,
                                       uint8              *pRealm,
                                       int                nRealmLen,
                                       int                *pnRealmLenReq );
static int AEEOBEXCli_GetAuthResponse(IOBEXCli        *me,
                                      AEEOBEXAuthResp *pAuthResp);
static int AEEOBEXCli_GetParams( IOBEXCli          *me,
                                 AEEOBEXParamType  param,
                                 uint32            *pU32,
                                 uint8             *pParam,
                                 int               nParamLen,
                                 int               *pnParamLenReq);

/*=============================================================================
 Client static function definitions
=============================================================================*/

static int AEEOBEXCli_Connect( IOBEXCli             *me,
                               IOBEXHeader          *piHdr,
                               OEMOBEXAuthInfo      *pAuth);
static int AEEOBEXCli_AccessReq( IOBEXCli        *me,
                                 AEEOBEXRequestType obexReq,
                                 IOBEXHeader     *piHdr,
                                 AEEOBEXFlags    flags,
                                 boolean         bFinal,
                                 OEMOBEXAuthInfo *pAuth);

static int AEEOBEXCli_Disconnect(IOBEXCli     *p,
                                 IOBEXHeader  *piHdr );

static uint32 AEEOBEXCli_Delete(IOBEXCli *me);
static int AEEOBEXCli_SysInit(IOBEXCli *me);
static int AEEOBEXCli_ParseBtConnStr(const char *pszTransParams,
                                     OEMOBEX_TransportConnInfo *connInfo);
static boolean AEEOBEXCli_SendHdrValid(AEEOBEXCliSys *pCliSys, OEMOBEXHdr *pOemHdr);
static boolean AEEOBEXCli_RecvHdrAvailable(AEEOBEXCliSys *pCliSys);

static void AEEOBEXCli_ReleaseSendHdrs(AEEOBEXCliSys *pSysObj);
static int AEEOBEXCli_Abort( AEEOBEXCliSys  *pCliSys, IOBEXHeader  *piHdr);
static int AEEOBEXCli_CreateHeaderSet(IOBEXCli *me, IOBEXHeader **ppif);
/*=============================================================================
 Client function definitions that process events from the OEM layer
=============================================================================*/
static void AEEOBEXCli_OEMEvtCallback(void* pCliSys, OEMOBEXEvent *pEvt,
                                      OEMOBEXAuthInfo * pAuthChal,
                                      OEMOBEXAuthResp *pAuthResp);
static void AEEOBEXCli_SysEvtCallback(void* pCliSys);
static void AEEOBEXCli_EvtConnComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtPutComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtGetComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtSetPathComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtAbortComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtDiscComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtAuthChallenge(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtAuthResponse(AEEOBEXCliSys *pCliSys);
static void AEEOBEXCli_EvtAuthComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);
static void AEEOBEXCli_EvtError(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt);

/*=============================================================================
 IOBEXCli vector table
=============================================================================*/
static const IOBEXCliVtbl g_Vtbl_OBEXCli = {
   AEEOBEXCli_AddRef,
   AEEOBEXCli_Release,
   AEEOBEXCli_QueryInterface,
   AEEOBEXCli_GetSupportedTransports,
   AEEOBEXCli_SetTransport,
   AEEOBEXCli_CreateHeaderSet,
   AEEOBEXCli_Authenticate,
   AEEOBEXCli_SendCmd,
   AEEOBEXCli_OnEventNotify,
   AEEOBEXCli_GetEvent,
   AEEOBEXCli_SetUidPassword,
   AEEOBEXCli_GetAuthChallenge,
   AEEOBEXCli_GetAuthResponse,
   AEEOBEXCli_GetParams
};

//=============================================================================
// 
//
//=============================================================================

static int AEEOBEXCli_CreateHeaderSet(IOBEXCli *me, IOBEXHeader **ppif)
{
   AEEOBEXCliSys *pSysObj = me->pCliSys;
   int ret = AEE_ENOMEMORY;
   boolean  bFound = FALSE;
   int i = 0;
   OEMOBEXHdr* pHdr = NULL;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_CreateHeaderSet ->",0,0,0);

   // see if there is free header in the list 
   for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
   {
      if (pSysObj->aSendHdr[i] != NULL)
      {  // there is an oem header, check if it is free
         bFound = OEMOBEXHdr_IsFree(pSysObj->aSendHdr[i]);

         if (TRUE == bFound)
         {
            ret = AEE_SUCCESS;
            pHdr = pSysObj->aSendHdr[i];
            break;
         }
      }
   }

   if (AEE_SUCCESS != ret)
   {
      // no oem hdr in the list we have is free, try to allocate one
      for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if (NULL == pSysObj->aSendHdr[i])
         {  // there is place for an oem header, see if we can allocate one
            ret = OEMOBEXHeader_Init(&pSysObj->aSendHdr[i], me->uMaxPacket);
            pHdr = pSysObj->aSendHdr[i];
            break;
         }
      }
   }

   if ((AEE_SUCCESS == ret) && (NULL != pHdr))
   {
      // get a corresponding AEE object for this oem object
      ret = AEEOBEXHeader_GetObject(me->pIShell, (void**)ppif, pHdr, TRUE);
   }
   MSG_DEBUG("AEE OBEX AEEOBEXCli_CreateHeaderSet ret %x <-",ret,0,0);
   return ret;
}


//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_SysInit(IOBEXCli *me)
{
   AEEOBEXCliSys *pSysObj = NULL;
   int ret             = AEE_SUCCESS;
   
   MSG_DEBUG("AEE OBEX AEEOBEXCli_SysInit ->",0,0,0);

   pSysObj = OEM_Malloc(sizeof(AEEOBEXCliSys));

   if (NULL == pSysObj)
   {
      MSG_ERROR("AEE OBEX could not alloc sys obj",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)pSysObj, 0, sizeof(AEEOBEXCliSys));

   pSysObj->state         = AEEOBEX_CLI_STATE_CREATED;
   pSysObj->uRef          = 1;
   pSysObj->pCli          = me;
   me->pCliSys            = pSysObj;

   ret = AEEOBEXEvtQ_Init(&pSysObj->sysEvtQueue,
                          AEEOBEX_MAX_EVENTS,
                          sizeof(OEMOBEXEvent));

   if (AEE_SUCCESS == ret)
   {
      // initialize the oem layer and register a callback that 
      // the oem layer calls when it needs to deliver an event to the
      // AEE layer concerning this object
      MSG_DEBUG("AEE OBEX cli sys evt Q init compl...",0,0,0);
      ret = OEMOBEXCli_Init(&pSysObj->pOemObj,
                            pSysObj,
                            AEEOBEXCli_OEMEvtCallback);

      if (AEE_SUCCESS == ret)
      {
         // initially get one send header list
         MSG_DEBUG("AEE OBEX cli oem init compl...",0,0,0);
         OEMOBEXCli_GetPacketSize(me->pCliSys->pOemObj, &me->uMaxPacket);

         ret = OEMOBEXHeader_Init(&pSysObj->aSendHdr[0], me->uMaxPacket);
      }
   }

   if (AEE_SUCCESS != ret)
   {  // clean up
      MSG_ERROR("AEE OBEX cli sys obj creation failed ret %x",ret,0,0);
      if (NULL != pSysObj->pOemObj)
      {
         OEMOBEXCli_Release(pSysObj->pOemObj);
         pSysObj->pOemObj = NULL;
      }
      AEEOBEXCli_ReleaseSendHdrs(pSysObj);
      AEEOBEXEvtQ_Free(&pSysObj->sysEvtQueue);
      me->pCliSys    = NULL;
      OEM_Free(pSysObj);
   }

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SysInit ret %x <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXCli_ReleaseSendHdrs(AEEOBEXCliSys *pSysObj)
{
   int i = 0;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_ReleaseSendHdrs -> ",0,0,0);

   for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
   {
      if (pSysObj->aSendHdr[i] != NULL)
      {
         MSG_DEBUG("AEE OBEX rel send hdrs index i %x ",i,0,0);
         OEMOBEXHdr_Release(pSysObj->aSendHdr[i]);
         pSysObj->aSendHdr[i] = NULL;
      }
   }
   MSG_DEBUG("AEE OBEX AEEOBEXCli_ReleaseSendHdrs <- ",0,0,0);
   return ;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXCli_AddRef( IOBEXCli *me )
{
   return ++me->uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXCli_Release( IOBEXCli *me )
{
   uint32 uRef = --me->uRef;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_Release -> ",0,0,0);

   if (0 == uRef)
   {
      MSG_HIGH("AEE OBEX cli uRef 0.. calling delete ",0,0,0);
      AEEOBEXCli_Delete(me);
   }
   MSG_DEBUG("AEE OBEX AEEOBEXCli_Release uRef <-",uRef,0,0);
   return uRef;
}
//=============================================================================
// 
//
//=============================================================================

static uint32 AEEOBEXCli_SysRelease(AEEOBEXCliSys *pCliSys)
{
   boolean bRelease = FALSE;
   pCliSys->pCli = NULL;

#ifdef BT_SIMULATION
    // In case of simulation while closing the simulator,
    // all the memory allocated by interfaces has to be freed
    // immediately. This is ensured by setting the state 
    // to AEEOBEX_CLI_STATE_CREATED
    pCliSys->state = AEEOBEX_CLI_STATE_CREATED ; 
#endif
   MSG_DEBUG("AEE OBEX AEEOBEXCli_SysRelease ->",0,0,0);

   if (0 == pCliSys->uRef)
   { // the app wants to free the object

      if ((AEEOBEX_CLI_STATE_CREATED == pCliSys->state) ||
          (AEEOBEX_CLI_STATE_INIT == pCliSys->state))
      {
         MSG_HIGH("AEE OBEX cli state init ...",0,0,0);
         bRelease = TRUE;
      }
      else
      {
         // try to abort the current op if there is one
         // the evt complete for the op will free this object
         MSG_HIGH("AEE OBEX cli sending abort ...",0,0,0);
         if (pCliSys->uMultiPacketOp)
         {
         AEEOBEXCli_Abort(pCliSys, NULL);
      }
         else if (AEEOBEX_CLI_STATE_CONNECTED == pCliSys->state)
         {
            int ret = AEE_SUCCESS;
            // call disconnect and wait for the response
            ret = OEMOBEXCli_Disconnect(pCliSys->pOemObj,NULL);
            if (AEE_SUCCESS == ret) 
            {
               pCliSys->state = AEEOBEX_CLI_STATE_DISC_PEND;
            }
            else 
            {
               MSG_HIGH("AEE OBEX cli unable to disconnect ...",0,0,0);
               bRelease = TRUE;
            }
         }
      }
   }

   if (bRelease)
   {
      MSG_DEBUG("AEE OBEX Cli cleaning up sys object",0,0,0);
      CALLBACK_Cancel(&pCliSys->evSignalCb);
      if (NULL != pCliSys->pOemObj)
      {
         MSG_HIGH("AEE OBEX calling oem cli release... ",0,0,0);
         OEMOBEXCli_Release(pCliSys->pOemObj);
         pCliSys->pOemObj = NULL;
      }
      MSG_DEBUG("AEE OBEX releasing send headers... ",0,0,0);

      AEEOBEXCli_ReleaseSendHdrs(pCliSys);
      AEEOBEXEvtQ_Free(&pCliSys->sysEvtQueue);
      OEM_Free(pCliSys);

      MSG_HIGH("AEE OBEX cli sys object released... ",0,0,0);
   }

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SysRelease <- ",0,0,0);

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXCli_Delete( IOBEXCli *me )
{
   AEEOBEXCliEvt evt;
   int ret = AEE_SUCCESS;
   // this function is called when the client object
   // refernce count goes to zero Or the app
   // unloads without freeing the object.
   // decrement the reference count of the sys object so that
   // it can clean up.
   // release the AEE object 

   MSG_DEBUG("AEE OBEX AEEOBEXCli_Delete -> ",0,0,0);

   me->pCliSys->uRef--;
   AEEOBEXCli_SysRelease(me->pCliSys);
   // this queue is always accessed within the UI task.so
   // it can be freed without using a critic section
   ISignalBus_Release(me->piSigAppEvt);
   
   while (AEE_SUCCESS == ret)
   {
      MEMSET((void*)&evt, 0, sizeof(AEEOBEXCliEvt));

      MSG_DEBUG("AEE OBEX clearing app evt Q ",0,0,0);

      ret = AEEOBEXEvtQ_Get(&me->appEvtQueue, (uint8*)&evt);

      if (evt.pOemHdr != NULL)
      {
         OEMOBEXHdr_Release(evt.pOemHdr);
      }
   }

   AEEOBEXEvtQ_Free(&me->appEvtQueue);
   CALLBACK_Cancel(&me->cbAppClose);
   RELEASEIF(me->pIShell);
   FREEIF(me);

   MSG_DEBUG("AEE OBEX AEEOBEXCli_Delete <- ",0,0,0);
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_QueryInterface( IOBEXCli *me,
                                      AEECLSID iid, void **ppo )
{
   switch (iid)
   {
   case AEEIID_IQI:
   case AEEIID_IOBEXCli:
      *ppo = me;
      break;

   default:
      *ppo = 0;
      return AEE_ECLASSNOTSUPPORT;
   }
   IQI_AddRef(*ppo);
   return AEE_SUCCESS;
}


//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_GetSupportedTransports( IOBEXCli *me,
                                              AEEOBEXTransport *pTransport,
                                              int nLen,
                                              int *pnLenReq)
{
   return AEEOBEX_GetSupportedTransports(me->suppTransport,
                                         pTransport,
                                         nLen,
                                         pnLenReq);
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_SetTransport( IOBEXCli         *me,
                                    AEEOBEXTransport transport,
                                    const char       *pszTransParams)
{
   OEMOBEX_TransportConnInfo  connInfo;
   AEEOBEXCliSys *pSysObj = me->pCliSys;
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SetTransport -> ",0,0,0);

   if ((NULL == pszTransParams) || (NULL == pSysObj))
   {
      MSG_DEBUG("AEE OBEX bad params param %x , pSysObj %x",
                 pszTransParams,pSysObj,0);
      return AEE_EBADPARM;
   }
      
   ret = AEEOBEXCli_ParseBtConnStr(pszTransParams, &connInfo);

   if ( AEE_SUCCESS == ret)
   {
      MSG_DEBUG("AEE OBEX parsing complete..setting params ",0,0,0);
      ret = OEMOBEXCli_SetTransport(pSysObj->pOemObj, &connInfo);
      pSysObj->state = AEEOBEX_CLI_STATE_INIT;
   }

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SetTransport ret %x <- ",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_Authenticate( IOBEXCli        *me,
                                    AEEOBEXCharSet  charSet,
                                    uint8           *pRealm,
                                    int             nRealmLen,
                                    boolean         uid,
                                    boolean         acs )
{
   // saves authentication params to be sent with the subsequent command
   // auth params are cleared once sent
   MSG_DEBUG("AEE OBEX AEEOBEXCli_Authenticate -> ",0,0,0);

   MSG_MED("AEE OBEX cli authnticate uid %x, acs %x, nRealmLen %x ",uid,acs,nRealmLen);

   if ((0 != nRealmLen) && 
      ((NULL == pRealm) || (!AEE_CHECK_PTR("AEEOBEXCli_Authenticate",
                        pRealm, nRealmLen, TRUE)) ))
   {
      MSG_DEBUG("AEE OBEX cli nRealmLen %x pRealm %x ",nRealmLen,pRealm,0);
      return AEE_EBADPARM;
   }

   me->authReq.bFullAccess = acs;
   me->authReq.bUserIdReq  = uid;
   me->authReq.nRealmLen   = 0;
   me->authReq.charSet     = 0;

   MEMSET((void*)&me->authReq.aRealm[0], 0, sizeof(me->authReq.aRealm));

   if (0 != nRealmLen)
   {
      int len = 0;

      me->authReq.charSet   = charSet;
      // verify that the nRealmLen does not exceed the buffer size
      len = (nRealmLen < sizeof(me->authReq.aRealm)) ?
                        nRealmLen :
                        sizeof(me->authReq.aRealm);

      me->authReq.nRealmLen = len;
      MEMCPY((void*)&me->authReq.aRealm[0], pRealm, len);
   }
   me->bAuthReq = TRUE;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_Authenticate <- ",0,0,0);

   return AEE_SUCCESS;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_SendCmd( IOBEXCli           *me,
                               AEEOBEXRequestType obexReq,
                               IOBEXHeader        *piObexHdr,
                               AEEOBEXFlags       flags,
                               boolean            bFinal )
{
   OEMOBEXAuthInfo  *pAuthRequest = NULL;
   OEMOBEXHdr *pOemHdr = NULL;
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SendCmd -> ",0,0,0);

   MSG_MED("AEE OBEX SendCmd cmd %x, flags %x, bfinal %x ",obexReq,flags,bFinal);

   pAuthRequest = (me->bAuthReq) ? &me->authReq : NULL;
   me->bAuthReq = FALSE;

   // Are there any headers being sent out?
   if (NULL != piObexHdr)
   {
      MSG_MED("AEE OBEX Send cmd header being sent ",0,0,0);
      pOemHdr = AEEOBEXHdr_GetOemHdr(piObexHdr);
      // verify that the header belongs to the pool for this 
      // client.
      if ( !AEEOBEXCli_SendHdrValid(me->pCliSys, pOemHdr))
      {
         MSG_ERROR("AEE OBEX Send cmd header invalid ",0,0,0);
         return AEE_EBADPARM;
      }
   }

   // verify that there is a buffer available to 
   // receive packets from the remote device.
   if ((AEEOBEX_REQ_ABORT != obexReq) &&
      (!AEEOBEXCli_RecvHdrAvailable(me->pCliSys)))
   {
      MSG_ERROR("AEE OBEX recv hdrs not available",0,0,0);
      return AEE_ENOMEMORY;
   }

   switch (obexReq)
   {
      case AEEOBEX_REQ_CONNECT:
         ret = AEEOBEXCli_Connect(me, piObexHdr, pAuthRequest);
         break;
      case AEEOBEX_REQ_PUT:
      case AEEOBEX_REQ_GET:
      case AEEOBEX_REQ_SETPATH:
         ret = AEEOBEXCli_AccessReq(me, obexReq, piObexHdr,
                                    flags, bFinal, pAuthRequest);
         break;
      case AEEOBEX_REQ_DISCONNECT:
         ret = AEEOBEXCli_Disconnect(me, piObexHdr);
         break;
      case AEEOBEX_REQ_ABORT:
         ret = AEEOBEXCli_Abort(me->pCliSys, piObexHdr);
         break;
      default :
         ret = AEE_EUNSUPPORTED;
         break;
   }

   MEMSET((void*)&me->authReq, 0, sizeof(OEMOBEXAuthInfo));

   MSG_DEBUG("AEE OBEX AEEOBEXCli_SendCmd ret %x <- ",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_OnEventNotify(IOBEXCli     *me,
                                    ISignal      *piSignal )
{
   int ret = AEE_SUCCESS;
   MSG_DEBUG("AEE OBEX AEEOBEXCli_OnEventNotify -> ",0,0,0);

   if (NULL == piSignal)
   {
      MSG_ERROR("AEE OBEX on evt notify, bad sig %x ",piSignal,0,0);
      return AEE_EBADPARM;
   }

   ret = ISignalBus_Add(me->piSigAppEvt, piSignal);

   MSG_DEBUG("AEE OBEX AEEOBEXCli_OnEventNotify ret %x <- ",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_GetEvent(IOBEXCli           *me,
                               AEEOBEXEventType   *pEvent,
                               IOBEXHeader        **ppiHdr,
                               AEEOBEXCmdStatus   *pCmdStatus,
                               AEEOBEXFlags       *pFlags)
{
   int ret = AEE_ENOMORE;
   AEEOBEXCliEvt evt;

   if ((NULL == pEvent) || (NULL == pCmdStatus) || (NULL == pFlags) ||
       (NULL == ppiHdr) ||
       (!AEE_CHECK_PTR("AEEOBEXCli_GetEvent",
                        pEvent, sizeof(AEEOBEXEventType), TRUE)) ||
       (!AEE_CHECK_PTR("AEEOBEXCli_GetEvent",
                        ppiHdr, sizeof(IOBEXHeader*), TRUE)) ||
       (!AEE_CHECK_PTR("AEEOBEXCli_GetEvent",
                        pCmdStatus, sizeof(AEEOBEXCmdStatus), TRUE))||
       (!AEE_CHECK_PTR("AEEOBEXCli_GetEvent",
                        pFlags, sizeof(AEEOBEXFlags), TRUE)))
   {
      MSG_ERROR("AEE OBEX GetEvent bad param pEvent %x,ppiHdr %x, pCmdStatus %x"
         ,pEvent,ppiHdr,pCmdStatus);
      MSG_ERROR("pFlags %x",pFlags,0,0);
      return AEE_EBADPARM;
   }
   MSG_MED("AEE OBEX AEEOBEXCli_GetEvent -> ",0,0,0);

   MEMSET((void*)&evt, 0, sizeof(AEEOBEXCliEvt));
   // pop the first event on the queue.
   ret = AEEOBEXEvtQ_Get(&me->appEvtQueue, (uint8*)&evt);

   if (AEE_SUCCESS == ret)
   {
      *pEvent     = evt.event;
      *pCmdStatus = evt.status;
      *pFlags     = 0;
      *ppiHdr     = NULL;

      if (NULL != evt.pOemHdr)
      {  // get an AEE object for this header
         ret = AEEOBEXHeader_GetObject(me->pIShell,
                                 (void**)ppiHdr,
                                 evt.pOemHdr,
                                 FALSE);
      }
      MSG_MED("AEE OBEX event present evt %x, stat %x hdr %x"
               ,evt.event,evt.status,*ppiHdr);
   }
   MSG_MED("AEE OBEX AEEOBEXCli_GetEvent ret %x ",ret,0,0);
   return ret;
}


//=============================================================================
// 
//
//=============================================================================

static int AEEOBEXCli_SetUidPassword(IOBEXCli         *me,
                                     const uint8      *pUid,
                                     int              nUidLen,
                                     const uint8      *pPswd,
                                     int              nPswdLen)
{
   int ret = AEE_SUCCESS;
   // verify params
   if ((nUidLen > 0) &&
       ((NULL == pUid)||
       (!AEE_CHECK_PTR("AEEOBEXCli_SetUidPassword",
                    pUid, nUidLen, TRUE))))
   {
      MSG_ERROR("AEE OBEX cli set uid/pswd  uid %x, len %x ",pUid,nUidLen,0);
      return AEE_EBADPARM;
   }
   if ((nPswdLen > 0) &&
       ((NULL == pPswd)||(!AEE_CHECK_PTR("AEEOBEXCli_SetUidPassword",
                    pPswd, nPswdLen, TRUE))))
   {
      MSG_ERROR("AEE OBEX cli set uid/pswd  pswd %x ",pPswd,0,0);
      return AEE_EBADPARM;
   }

   if (me->bAuthChal || me->bAuthResp)
   {
      MSG_DEBUG("AEE OBEX calling oem auth response",0,0,0);

      ret = OEMOBEXCli_AuthResponse(me->pCliSys->pOemObj,
                                    pUid,
                                    nUidLen,
                                    pPswd,
                                    nPswdLen);
      if (AEE_SUCCESS == ret)
      {
         if (me->bAuthChal)
         {
            me->bAuthChal = FALSE;
         }
         else if (me->bAuthResp)
         {
            me->bAuthResp = FALSE;
         }
      }
   }
   MSG_MED("AEE OBEX auth response ret %x ",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_GetAuthChallenge(IOBEXCli           *me,
                                       boolean            *pbUserIdReq,
                                       boolean            *pbFullAccess,
                                       AEEOBEXCharSet     *pCharSet,
                                       uint8              *pRealm,
                                       int                nRealmLen,
                                       int                *pnRealmLenReq )
{
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_GetAuthChallenge -> ",0,0,0);

   if (FALSE == me->bAuthChal)
   {
      MSG_ERROR("AEE OBEX no auth chal info",0,0,0);
      return AEE_ENOSUCH;
   }

   if ((NULL == pbUserIdReq) ||
       (NULL == pbFullAccess)||
       (nRealmLen < 0))
   {
      MSG_ERROR("AEE OBEX auth chal bad param pusrid %x, pbAcs ",pbUserIdReq,pbFullAccess,0);
      return AEE_EBADPARM;
   }

   *pbUserIdReq  = me->authChal.bUserIdReq;
   *pbFullAccess = me->authChal.bFullAccess;

   if (NULL != pCharSet)
   {
      *pCharSet = me->authChal.charSet;
   }

   if (NULL != pnRealmLenReq)
   {
      *pnRealmLenReq = me->authChal.nRealmLen;
   }

   if (me->authChal.nRealmLen > 0)
   {
      if (nRealmLen >= me->authChal.nRealmLen)
   {
      if ((NULL == pRealm) ||
         (!AEE_CHECK_PTR("AEEOBEXCli_GetAuthChallenge",
                    pRealm, nRealmLen, TRUE)))
      {
         ret = AEE_EBADPARM;
      }
      else
      {
         int i= 0;
         for (i = 0; (i < nRealmLen) && (i < me->authChal.nRealmLen); i++)
         {
            pRealm[i] = me->authChal.aRealm[i];
         }
      }
   }
      else
      {
         ret = AEE_EBUFFERTOOSMALL;
      }
   }
   MSG_DEBUG("AEE OBEX AEEOBEXCli_GetAuthChallenge <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_GetAuthResponse(IOBEXCli        *me,
                                      AEEOBEXAuthResp *pAuthResp)
{
   int i =0;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_GetAuthResponse-> ",0,0,0);

   if ((NULL == pAuthResp) ||
      (!AEE_CHECK_PTR("AEEOBEXCli_GetAuthResponse",
                        pAuthResp, sizeof(AEEOBEXAuthResp), TRUE)))
   {
      MSG_ERROR("AEE OBEX bad param: pAuthResp %x ",pAuthResp,0,0);
      return AEE_EBADPARM;
   }

   if (FALSE == me->bAuthResp )
   {
      MSG_ERROR("AEE OBEX cli no auth response to read ",0,0,0);
      return AEE_ENOSUCH;
   }

   pAuthResp->nUidLen = me->authResp.nUidLen;
   
   for ( i = 0 ; (i < AEEOBEX_MAX_UID_LEN) && (i < OEMOBEX_MAX_UID_LEN); i++)
   {
      pAuthResp->aUserId[i] = me->authResp.aUserId[i];
   }

   MSG_DEBUG("AEE OBEX AEEOBEXCli_GetAuthResponse <- ",0,0,0);
   return AEE_SUCCESS;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_GetParams( IOBEXCli          *me,
                                 AEEOBEXParamType  param,
                                 uint32            *pU32,
                                 uint8             *pParam,
                                 int               nParamLen,
                                 int               *pnParamLenReq)
{ 
   int ret = AEE_EUNSUPPORTED;

   MSG_MED("AEE OBEX cli get param %x",param,0,0);

   switch(param)
   {
      case AEEOBEXPARAM_BT_BDA:
      {
         if ((nParamLen >= AEEOBEX_MAX_BDA_LEN) &&
             (NULL == pParam))
         {
            ret = AEE_EBADPARM;
         }
         else if (nParamLen < AEEOBEX_MAX_BDA_LEN) 
         {
            ret = AEE_EBUFFERTOOSMALL;
         }
         else
         {
            MSG_MED("AEE OBEX cli get bda",0,0,0);
            ret = OEMOBEXCli_GetBda(me->pCliSys->pOemObj, pParam);
         }
         if (NULL != pnParamLenReq)
         {
            *pnParamLenReq = AEEOBEX_MAX_BDA_LEN;
         }
         break;
      }
      case AEEOBEXPARAM_BT_RFCOMM_SCN:
      {
         MSG_MED("AEE OBEX cli get scn",0,0,0);
         if (NULL != pU32)
         {
            ret = OEMOBEXCli_GetScn(me->pCliSys->pOemObj, pU32);
         }
         else
         {
            ret = AEE_EBADPARM;
         }
         break;
      }
      case AEEOBEXPARAM_MAX_PACKET_SIZE:
      {
         MSG_MED("AEE OBEX cli get packet size",0,0,0);
         if (NULL != pU32)
         {
            ret = OEMOBEXCli_GetPacketSize(me->pCliSys->pOemObj, pU32);
         }
         else
         {
            ret = AEE_EBADPARM;
         }
         break;
      }
      default:
         break;
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_Connect( IOBEXCli             *me,
                               IOBEXHeader          *pHdr,
                               OEMOBEXAuthInfo      *pAuth)
{
   int ret = AEE_SUCCESS;
   OEMOBEXHdr *pOemHdr = NULL;

   MSG_MED("AEE OBEX cli connect state %x , bAbort %x "
      ,me->pCliSys->state,me->pCliSys->bAbort,0);

   // check if transport is set and the client is not already connected
   if ((AEEOBEX_CLI_STATE_CREATED == me->pCliSys->state) ||
       (AEEOBEX_CLI_STATE_CONNECTED == me->pCliSys->state))
   {
      MSG_ERROR("AEE OBEX cli connect bad state %x"
            ,me->pCliSys->state,0,0);
      return AEE_EBADSTATE;
   }

   // there should be no operation pending.( including abort )
   if ((AEEOBEX_CLI_STATE_INIT != me->pCliSys->state) ||
       (TRUE == me->pCliSys->bAbort))
   {
      MSG_ERROR("AEE OBEX cli connect bad state %x"
            ,me->pCliSys->state,0,0);
      return AEE_EALREADY;
   }

   if (NULL != pHdr)
   {
      // the command packet should not exced 255 bytes
      pOemHdr = AEEOBEXHdr_GetOemHdr(pHdr);

      if (FALSE == AEEOBEX_VerifyPacketSize(me->uMaxPacket, pOemHdr, TRUE)) // 3rdparam indicates single packet
      {
         MSG_ERROR("AEE OBEX cli connect bad packet size "
            ,0,0,0);
         return AEE_EBADPARM;
      }
   }
   ret = OEMOBEXCli_Connect(me->pCliSys->pOemObj, pOemHdr, pAuth);

   if (AEE_SUCCESS == ret)
   {
      // set state to connect pending
      me->pCliSys->state = AEEOBEX_CLI_STATE_CONN_PEND;
      if (pHdr)
      {
         // the oem layer will mark the object as not busy
         // when it is done.
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }

   MSG_MED("AEE OBEX cli connect state %x , ret %x "
            ,me->pCliSys->state,ret,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_AccessReq( IOBEXCli           *me,
                                 AEEOBEXRequestType obexReq,
                                 IOBEXHeader        *pHdr,
                                 AEEOBEXFlags       flags,
                                 boolean            bFinal,
                                 OEMOBEXAuthInfo    *pAuth)
{
   int ret      = AEE_SUCCESS;
   uint32 state = me->pCliSys->state;
   OEMOBEXHdr *pOemHdr = NULL;

   MSG_MED("AEE OBEX cli access req state  %x , bAbort %x "
            ,me->pCliSys->state,me->pCliSys->bAbort,0);

   // check if transport is set
   if (AEEOBEX_CLI_STATE_CREATED == me->pCliSys->state)
   {
      MSG_ERROR("AEE OBEX cli access req bad state",0,0,0);
      return AEE_EBADSTATE;
   }

   // there should be no operation pending
   if (((AEEOBEX_CLI_STATE_INIT != me->pCliSys->state) && 
       (AEEOBEX_CLI_STATE_CONNECTED != me->pCliSys->state)) ||
       (TRUE == me->pCliSys->bAbort))
   {
      MSG_ERROR("AEE OBEX cli access req bad state",0,0,0);
      return AEE_EALREADY;
   }
   
   if (NULL != pHdr)
   {
      // the command packet should not exced maxpacket bytes
      pOemHdr = AEEOBEXHdr_GetOemHdr(pHdr);
      // see if connection less + auth enabld #check
      if (FALSE == AEEOBEX_VerifyPacketSize(me->uMaxPacket, pOemHdr, FALSE)) // 3rdparam indicates single packet
      {
         MSG_ERROR("AEE OBEX cli access req invalid header size",0,0,0);
         return AEE_EBADPARM;
      }
   }

   switch (obexReq)
   {
      case AEEOBEX_REQ_PUT:
      {
         ret = OEMOBEXCli_Put(me->pCliSys->pOemObj, pOemHdr, bFinal, pAuth);
         state = AEEOBEX_CLI_STATE_PUT_PEND;
         break;
      }
      case AEEOBEX_REQ_GET:
      {
         ret = OEMOBEXCli_Get(me->pCliSys->pOemObj, pOemHdr, bFinal, pAuth);
         state = AEEOBEX_CLI_STATE_GET_PEND;
         break;
      }
      case AEEOBEX_REQ_SETPATH:
      {
         boolean bLevel = (flags & AEEOBEX_FLAG_BACKUP) ? TRUE :FALSE;
         boolean bDontCreate = (flags & AEEOBEX_FLAG_DONT_CREATE_DIR) ? TRUE :FALSE;
         ret = OEMOBEXCli_SetPath(me->pCliSys->pOemObj, pOemHdr,bLevel, bDontCreate, pAuth);
         state = AEEOBEX_CLI_STATE_SET_PATH_PEND;
         break;
      }
      default :
         MSG_ERROR(" AEE OBEX Cli Invalid request = %x", obexReq,0,0);
         // should not be reaching here 
         break;
   }

   if (AEE_SUCCESS == ret)
   {
      me->pCliSys->state = state;
      // mark multipacket operations
      if ((AEEOBEX_REQ_GET == obexReq) ||
         (AEEOBEX_REQ_PUT  == obexReq))
      {
         me->pCliSys->uMultiPacketOp = (bFinal) ? AEEOBEX_PACKET_FINAL : AEEOBEX_PACKET_INTERMEDIATE;
      }
      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }

   MSG_MED("AEE OBEX cli access req state %x ret %x",me->pCliSys->state,ret,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_Abort( AEEOBEXCliSys *pCliSys ,IOBEXHeader  *pHdr)
{
   OEMOBEXHdr *pOemHdr = NULL;
   int ret = AEE_SUCCESS;
  
   MSG_MED("AEE OBEX cli abort state %x, bAbort %x ",pCliSys->state,pCliSys->bAbort,0);

   // there should be a PUT or GET Pending 
   if ((AEEOBEX_CLI_STATE_PUT_PEND != pCliSys->state) &&
       (AEEOBEX_CLI_STATE_GET_PEND != pCliSys->state) &&
       (0 == pCliSys->uMultiPacketOp))
   {
      MSG_HIGH("AEE OBEX cli no multi packet op in progress state %x, bAbort %x, multi op %x ",
               pCliSys->state,pCliSys->bAbort,pCliSys->uMultiPacketOp);
      return AEE_EBADSTATE;
   }

   if (pCliSys->bAbort)
   {
      MSG_ERROR(" AEE OBEX cli abort already in progress",0,0,0);
      return AEE_EALREADY;
   }
   // the header size should not not exced 255 bytes
   if (NULL != pHdr)
   {
      // the command packet should not exced 255 bytes
      pOemHdr = AEEOBEXHdr_GetOemHdr(pHdr);

      if (FALSE == AEEOBEX_VerifyPacketSize(0, pOemHdr, TRUE)) // 3rdparam indicates single packet
      {
         return AEE_EBADPARM;
      }
   }

   ret = OEMOBEXCli_Abort(pCliSys->pOemObj, pOemHdr);

   if (AEE_SUCCESS == ret) 
   {
      pCliSys->bAbort = TRUE;
      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }

   MSG_MED("AEE OBEX cli abort state %x, bAbort %x ret %x ",
            pCliSys->state,pCliSys->bAbort,ret);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_Disconnect(IOBEXCli     *me,
                                 IOBEXHeader  *pHdr )
{
   int ret = AEE_SUCCESS;
   OEMOBEXHdr *pOemHdr = NULL;
   
   MSG_MED("AEE OBEX cli disconnect state %x, bAbort %x ",
            me->pCliSys->state,me->pCliSys->bAbort,0);

   // check if there is a connection
   if ((AEEOBEX_CLI_STATE_CONNECTED != me->pCliSys->state) ||
       (TRUE == me->pCliSys->bAbort))
   {
      MSG_ERROR("AEE OBEX cli disconnect bad state ",0,0,0);
      return AEE_EBADSTATE;
   }

   // the header size should not not exced 255 bytes
   if (NULL != pHdr)
   {
      // the command packet should not exced 255 bytes
      pOemHdr = AEEOBEXHdr_GetOemHdr(pHdr);

      if (FALSE == AEEOBEX_VerifyPacketSize(0, pOemHdr, TRUE)) // 3rdparam indicates single packet
      {
         return AEE_EBADPARM;
      }
   }

   ret = OEMOBEXCli_Disconnect(me->pCliSys->pOemObj,pOemHdr);

   if (AEE_SUCCESS == ret) 
   {
      // set op to connect
      me->pCliSys->state = AEEOBEX_CLI_STATE_DISC_PEND;

      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }
   MSG_MED("AEE OBEX cli disconnect ret %x",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_OEMEvtCallback(void            *cbData,
                               OEMOBEXEvent    *pEvt,
                               OEMOBEXAuthInfo *pAuthChal,
                               OEMOBEXAuthResp *pAuthResp)
{
   AEEOBEXCliSys *pCliSys = (AEEOBEXCliSys*)cbData;

   /*
   MSG_MED("AEE OBEX AEEOBEXCli_OEMEvtCallback evt %x, authchal %x, authReap %x",
            pEvt->event,pAuthChal,pAuthResp);

   MSG_MED("AEE OBEX AEEOBEXCli_OEMEvtCallback cli sys %x",
            cbData,0,0);
   */

   if (NULL != pAuthResp) 
   {
      if ((NULL != pCliSys) && (NULL != pCliSys->pCli))
      {
         pCliSys->pCli->authResp = *pAuthResp;
      }
   }
   if (NULL != pAuthChal) 
   {
      if ((NULL != pCliSys) && (NULL != pCliSys->pCli))
      {
         pCliSys->pCli->authChal = *pAuthChal;
      }
   }
   if (pCliSys)
   {
      AEEOBEXEvtQ_Put(&pCliSys->sysEvtQueue, (uint8*)pEvt);
      CALLBACK_Cancel(&pCliSys->evSignalCb);
      CALLBACK_Init(&pCliSys->evSignalCb, AEEOBEXCli_SysEvtCallback, pCliSys);
      AEE_SYS_RESUME(&pCliSys->evSignalCb);
   }
}
//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_SysEvtCallback(void* pme)
{
   AEEOBEXCliSys *pCliSys = (AEEOBEXCliSys*)pme;
   OEMOBEXEvent  evt;
   int ret = AEE_EFAILED;

   MSG_MED("AEE OBEX AEEOBEXCli_SysEvtCallback", 0,0,0);
   // pop the first event on the queue.
   ret = AEEOBEXEvtQ_Get(&pCliSys->sysEvtQueue, (uint8*)&evt);

   while (AEE_SUCCESS == ret)
   {
      MSG_MED("AEE OBEX AEEOBEXCli_SysEvtCallback event %x",evt.event,0,0);

      switch (evt.event)
      {
         case AEEOBEX_EVT_CLI_CONN_RESP:
            AEEOBEXCli_EvtConnComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_CLI_DISC_RESP:
            AEEOBEXCli_EvtDiscComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_CLI_PUT_RESP:
            AEEOBEXCli_EvtPutComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_CLI_GET_RESP:
            AEEOBEXCli_EvtGetComplete(pCliSys,&evt);
           break;
         case AEEOBEX_EVT_CLI_SETPATH_RESP:
            AEEOBEXCli_EvtSetPathComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_CLI_ABORT_COMPL:
            AEEOBEXCli_EvtAbortComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_AUTH_CHALLENGE:
            AEEOBEXCli_EvtAuthChallenge(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_AUTH_RESPONSE:
            AEEOBEXCli_EvtAuthResponse(pCliSys);
            break;
         case AEEOBEX_EVT_AUTH_COMPL:
            AEEOBEXCli_EvtAuthComplete(pCliSys,&evt);
            break;
         case AEEOBEX_EVT_ERROR:
            AEEOBEXCli_EvtError(pCliSys,&evt);
            break;
         default:
            break;
      }
      if (pCliSys->pCli)
      {
        ISignalBus_Strobe(pCliSys->pCli->piSigAppEvt);
      }
      else
      {
         if (evt.u.cli.pOemHdr != NULL)
         { 
            // the receive header needs to be marked free
            // as it will not be sent to the app
            MSG_MED("AEE OBEX cli sys cb. no cli obj , releasing hdr",0,0,0);
            OEMOBEXHdr_Release(evt.u.cli.pOemHdr);
         }

      }
      ret = AEEOBEXEvtQ_Get(&pCliSys->sysEvtQueue, (uint8*)&evt);
   }
   
   if (pCliSys->uRef == 0)
   {
      // note : sysRelease deletes the object so pCliSys should not
      // be referenced after this point
      MSG_HIGH("AEE OBEX cli sys cb.calling sys release...",0,0,0);
      AEEOBEXCli_SysRelease(pCliSys);
   }
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtConnComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt conn compl stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (AEEOBEX_CLI_STATE_CONN_PEND == pCliSys->state)
   {
      if (AEEOBEX_STAT_SUCCESS == pEvt->u.cli.status)
      {
         pCliSys->state = AEEOBEX_CLI_STATE_CONNECTED;
         pCliSys->bConnOriented = TRUE;
      }
      else
      {
         pCliSys->state = AEEOBEX_CLI_STATE_INIT;
      }

      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_CONN_RESP;
         evt.status  = pEvt->u.cli.status;
         evt.pOemHdr = pEvt->u.cli.pOemHdr;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtPutComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt put compl stat %x, sysobj %x state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (AEEOBEX_CLI_STATE_PUT_PEND == pCliSys->state)
   {
       pCliSys->state = ( pCliSys->bConnOriented ) ? AEEOBEX_CLI_STATE_CONNECTED 
                                         :AEEOBEX_CLI_STATE_INIT;
      // if the fianl put packet was sent, the PUT sequence is over
      pCliSys->uMultiPacketOp = (pCliSys->uMultiPacketOp == AEEOBEX_PACKET_FINAL) ?
                                 0 :
                                 pCliSys->uMultiPacketOp;
      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_PUT_RESP;
         evt.status  = pEvt->u.cli.status;
         evt.pOemHdr = pEvt->u.cli.pOemHdr;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
         if (pCliSys->bAbort)
         {
            pCliSys->bAbort = FALSE;
      pCliSys->uMultiPacketOp = 0;
      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
            evt.event   = AEEOBEX_EVT_CLI_ABORT_COMPL;
            evt.status  = AEE_SUCCESS;
            evt.pOemHdr = NULL;
         
            AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
         }
      }
   return;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtGetComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt get compl stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (AEEOBEX_CLI_STATE_GET_PEND == pCliSys->state)
   {
      pCliSys->state = ( pCliSys->bConnOriented ) ? AEEOBEX_CLI_STATE_CONNECTED
                                          : AEEOBEX_CLI_STATE_INIT;

      pCliSys->uMultiPacketOp = (pEvt->u.cli.status == AEEOBEX_STAT_CONTINUE) ?
                                 AEEOBEX_PACKET_INTERMEDIATE :
                                 0;
      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_GET_RESP;
         evt.status  = pEvt->u.cli.status;
         evt.pOemHdr = pEvt->u.cli.pOemHdr;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
   if (pCliSys->bAbort)
   {
      pCliSys->bAbort = FALSE;
      pCliSys->uMultiPacketOp = 0;
      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_ABORT_COMPL;
         evt.status  = AEE_SUCCESS;
         evt.pOemHdr = NULL;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtSetPathComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt set path compl stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (AEEOBEX_CLI_STATE_SET_PATH_PEND == pCliSys->state)
   {
      pCliSys->state = ( pCliSys->bConnOriented ) ? AEEOBEX_CLI_STATE_CONNECTED
                                        : AEEOBEX_CLI_STATE_INIT;

      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_SETPATH_RESP;
         evt.status  = pEvt->u.cli.status;
         evt.pOemHdr = pEvt->u.cli.pOemHdr;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtAbortComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt abort compl stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (TRUE == pCliSys->bAbort)
   {
      pCliSys->bAbort = FALSE;

      if (pCliSys->pCli)
      {
         AEEOBEXCliEvt evt;
         evt.event   = AEEOBEX_EVT_CLI_ABORT_COMPL;
         evt.status  = AEE_SUCCESS;
         evt.pOemHdr = NULL;
         AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
      }
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtDiscComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXCliEvt evt;

   MSG_MED("AEE OBEX cli evt disc compl stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   pCliSys->bConnOriented = FALSE;
   pCliSys->state = AEEOBEX_CLI_STATE_INIT;
   pCliSys->uMultiPacketOp = 0;
   
   if (pCliSys->pCli)
   {
      evt.event   = AEEOBEX_EVT_CLI_DISC_RESP;
      evt.status  = pEvt->u.cli.status;
      evt.pOemHdr = pEvt->u.cli.pOemHdr;
      AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtAuthChallenge(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt auth chal evt stat %x, sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,pCliSys->state);

   if (pCliSys->pCli)
   {
      AEEOBEXCliEvt evt;
      evt.event     = AEEOBEX_EVT_AUTH_CHALLENGE;
      evt.status    = AEE_SUCCESS;
      evt.pOemHdr   = NULL;
      pCliSys->pCli->bAuthChal = TRUE;
      AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtAuthResponse(AEEOBEXCliSys *pCliSys)
{
   MSG_MED("AEE OBEX cli evt auth response evt sysobj %x, state %x"
            ,pCliSys,pCliSys->state,0);

   if (pCliSys->pCli)
   {
      AEEOBEXCliEvt evt;

      pCliSys->pCli->bAuthResp = TRUE;
      evt.event   = AEEOBEX_EVT_AUTH_RESPONSE;
      evt.status  = AEE_SUCCESS;
      evt.pOemHdr = NULL;

      AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
void AEEOBEXCli_EvtAuthComplete(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   MSG_MED("AEE OBEX cli evt auth complete evt sysobj %x, state %x"
            ,pEvt->u.cli.status,pCliSys,0);

   if (pCliSys->pCli)
   {
      AEEOBEXCliEvt evt;

      evt.event   = AEEOBEX_EVT_AUTH_COMPL;
      evt.status  = pEvt->u.cli.status;
      evt.pOemHdr = NULL;

      AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXCli_ParseBdAddr(const char* tok, void* data, void* dat2)
{
   OEMOBEX_BtRfcommConn *pConnInfo = (OEMOBEX_BtRfcommConn*)data;
   int ret = AEE_SUCCESS;

   MSG_MED("AEE OBEX cli parse bd addr %s",tok,0,0);

   ret = AEEOBEXTok_GetBdAddr(tok, &pConnInfo->bdAddr[0]);

   MSG_DEBUG("AEE OBEX cli parse bd addr read %x %x %x ",pConnInfo->bdAddr[0],
      pConnInfo->bdAddr[1],pConnInfo->bdAddr[2]);
   MSG_DEBUG("%x %x %x ",pConnInfo->bdAddr[3],
      pConnInfo->bdAddr[4],pConnInfo->bdAddr[5]);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXCli_ParseScn(const char* tok, void* data, void* dat2)
{
   OEMOBEX_BtRfcommConn *pConnInfo = (OEMOBEX_BtRfcommConn*)data;
   int ret = AEE_SUCCESS;

   ret = AEEOBEXTok_GetScn(tok, &pConnInfo->channel);
   MSG_MED("AEE OBEX cli parse scn %s , parsed scn, ret %x  ",tok,pConnInfo->channel,ret);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================

int AEEOBEXCli_ParseSecurity(const char* tok, void* data, void* dat2)
{
   int ret = AEE_SUCCESS;
   OEMOBEX_BtRfcommConn *pConnInfo = (OEMOBEX_BtRfcommConn*)data;
   ret = AEEOBEXTok_GetSecurity(tok, &pConnInfo->security);
   MSG_MED("AEE OBEX cli parse scn %s, parsed security ret %x ",tok,pConnInfo->security, ret);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXCli_ParseUuid(const char* tok, void* data, void* dat2)
{
   OEMOBEX_BtRfcommConn *pConnInfo = (OEMOBEX_BtRfcommConn*)data;
   uint32 uuid;
   int ret = AEE_SUCCESS;

   ret = AEEOBEXTok_GetUuid(tok, &uuid);
   pConnInfo->uuid = (uint16)uuid;
   MSG_MED("AEE OBEX cli parse uuid %s, parsed uuid",tok,pConnInfo->uuid,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXCli_ParseBtConnStr(const char *pszTransParams,
                                     OEMOBEX_TransportConnInfo *pConnInfo)
{
   AEEOBEXStrToken strParser;
   char *pNextTok = NULL;
   int ret = AEE_SUCCESS;
   int i;
   boolean bBdAddr = FALSE;
   OEMOBEX_BtRfcommConn *pBtCon = &pConnInfo->transportConn.btConn;
   AEEOBEXTokTable tokTable[] = {{"bd_address", AEEOBEXCli_ParseBdAddr},
                              {"server_channel",AEEOBEXCli_ParseScn},
                              {"bt_security",AEEOBEXCli_ParseSecurity},
                              {"uuid16", AEEOBEXCli_ParseUuid}};

   int tableLen = sizeof(tokTable)/sizeof(AEEOBEXTokTable);

   MSG_MED("AEE OBEX cli parse bt conn str %s",pszTransParams,0,0);

   // init the  conn params
   MEMSET(&pConnInfo->transportConn.btConn, 0, sizeof(OEMOBEX_BtRfcommConn));

   pConnInfo->transport = OEMOBEX_TRANS_BT_RFCOMM;
   pBtCon->uuid = OEMOBEXSDUUID_INVALID;

   // initialize the tokenizer
   AEEOBEXStrTok_Init(&strParser, pszTransParams, ';');

   // get the first token
   pNextTok = AEEOBEXStrTok_GetNext(&strParser);
  
   while ((NULL != pNextTok) && (AEE_SUCCESS == ret))
   {
      MSG_MED("AEE OBEX cli read token str %s",pNextTok,0,0);

      for (i = 0; i < tableLen ;i++)
      {
         if (STRBEGINS(tokTable[i].tok, pNextTok))
         {
            MSG_MED("AEE OBEX cli found token handler",0,0,0);

            if (STRBEGINS(tokTable[i].tok,"bd_address"))
            {
               bBdAddr = TRUE;
            }
            // found the attribute. get the value.
            ret = tokTable[i].pfnParse(pNextTok,(void*)pBtCon,NULL);
            break;
         }
      } 
      if (tableLen == i)
      {
         // this token was not found in our table.
         MSG_ERROR("AEE OBEX cli token not found",0,0,0);
         ret = AEE_EINVALIDFORMAT;   
      }
      pNextTok = AEEOBEXStrTok_GetNext(&strParser);
   }

   // verify the conn params.
   if ((AEE_SUCCESS == ret)&&
       (((0 == pBtCon->channel) && (OEMOBEXSDUUID_INVALID == pBtCon->uuid)) ||
        (FALSE == bBdAddr)))
   {
      MSG_ERROR("AEE OBEX mandatory params not present channel = %x uuid %x invalid ret %x <- "
                        ,pBtCon->channel,pBtCon->uuid,ret);
      ret = AEE_EINCOMPLETEITEM;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static boolean AEEOBEXCli_SendHdrValid(AEEOBEXCliSys *pCliSys, OEMOBEXHdr *pOemHdr)
{
   int i = 0;
   boolean bFound = FALSE;

   if ( NULL != pOemHdr)
   {
      for ( i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if ( pCliSys->aSendHdr[i] == pOemHdr)
         {
            bFound = TRUE;
            break;
         }
      }
   }
   return bFound;
}
//=============================================================================
// 
//
//=============================================================================
static boolean AEEOBEXCli_RecvHdrAvailable(AEEOBEXCliSys *pCliSys)
{
   return OEMOBEXCli_RecvHdrAvailable(pCliSys->pOemObj);
}

//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXCli_EvtError(AEEOBEXCliSys *pCliSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXCliEvt evt;

   MSG_MED("AEE OBEX cli Evt Error stat %x",pEvt->u.cli.status,0,0);

   pCliSys->bConnOriented = FALSE;
   evt.event   = AEEOBEX_EVT_ERROR;
   evt.status  = pEvt->u.cli.status;
   evt.pOemHdr = pEvt->u.cli.pOemHdr;
   pCliSys->state  = AEEOBEX_CLI_STATE_INIT;
   pCliSys->bAbort = FALSE;
   pCliSys->bConnOriented = FALSE;
   pCliSys->uMultiPacketOp = 0;

   if (pCliSys->pCli)
   {
      AEEOBEXEvtQ_Put(&pCliSys->pCli->appEvtQueue, (uint8*)&evt);
   }
   return;
}
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXCli_New(IShell* pIShell, AEECLSID cls, void **ppif)
{
#if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT_EXTPF_GOEP)
   return ECLASSNOTSUPPORT;
#else
   IOBEXCli *me = NULL;
   int ret      = AEE_SUCCESS;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_New ->",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE OBEX AEEOBEXCli_New ppif = NULL ",0,0,0);
      return AEE_EBADPARM;
   }
   // allocate AEE object
   me = MALLOCREC(IOBEXCli);

   if (NULL == me)
   {
      MSG_ERROR("AEE OBEX cannot allocate me",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(IOBEXCli));
   // initialize the object
   me->pvt             = &g_Vtbl_OBEXCli;
   me->uRef            = 1;
   me->pIShell         = pIShell;
   
   me->suppTransport   = 0;
   me->pCliSys         = NULL;
   me->uMaxPacket      = AEE_OBEX_DEFAULT_MAX_PACKET;

   ret = ISHELL_CreateInstance( pIShell, AEECLSID_SignalBus,
                               (void**) &me->piSigAppEvt );
   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE OBEX cannot create sig bus ret %x", ret,0,0);
      FREEIF(me);
      return AEE_EFAILED;
   }

   // fetch the supported transports from the OEM layer
   OEMOBEX_GetSupTransport(&me->suppTransport);

   if (0 == me->suppTransport)
   {
      // no transports supported?
      MSG_ERROR("AEE OBEX no transport support",0,0,0);
      RELEASEIF(me->piSigAppEvt);
      FREEIF(me);
      return AEE_EFAILED;
   }

   ret = AEEOBEXEvtQ_Init(&me->appEvtQueue,
                          AEEOBEX_MAX_EVENTS,
                          sizeof(AEEOBEXCliEvt));

   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE OBEX cannot alloc evt Q ret %x", ret,0,0);
      RELEASEIF(me->piSigAppEvt);
      FREEIF(me);
      return AEE_EFAILED;
   }
   // allocate and initialize any system memory including OEM layer
   ret = AEEOBEXCli_SysInit(me);
   
   if (AEE_SUCCESS != ret)
   {
      // couldnt allocate system memory 
      AEEOBEXEvtQ_Free(&me->appEvtQueue);
      RELEASEIF(me->piSigAppEvt);
      FREEIF(me);
      return AEE_ENOMEMORY;
   }

   // Initialize the cb that will be called in case the app closes
   CALLBACK_Init(&me->cbAppClose, AEEOBEXCli_Delete, me);
   AEE_LinkSysObject(&me->cbAppClose);

   ISHELL_AddRef(pIShell);
   // return the interface
   *ppif = (void *)me;

   MSG_DEBUG("AEE OBEX AEEOBEXCli_New <-",0,0,0);

   return AEE_SUCCESS;
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */
}


/*===========================================================================
FILE:  AEEOBEXSrv.c

SERVICES:  Implementation of IOBEXSrv interface

GENERAL DESCRIPTION:
   Implements IOBEXSrv and IOBEXSrvSession interface

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEOBEXSrv.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

  when         who  what, where, why
  --------   ---  ---------------------------------------------------------------
  2008-10-02   rs  Removing compiler and Lint warnings.
  2008-09-18   pr  Klocwork fixes
  2008-09-15   sp  Compiler warning fix

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
#include "AEEOBEXDefs_priv.h"
#include "AEEIOBEXSrv.h"
#include "AEEIOBEXSrvSession.h"
#include "AEEOBEXComDefs.h"
#include "OEMOBEXDefs.h"
#include "AEE_OEM.h"
#include "AEEBTDef.h"
#include "AEEBT.bid"
#include "AEEBT.h"
#include "AEEBTMgr.bid"
#include "AEEIBTMgr.h"
#include "AEEIBTServiceDiscovery.h"
#include "AEEBTServiceDiscovery.bid"
#include "AEEBTUtils.h"
//=============================================================================
//  Type definitions
//=============================================================================

//=============================================================================
//  OBEX Server states
//=============================================================================
#define AEEOBEX_SRV_STATE_CREATED           0
#define AEEOBEX_SRV_STATE_REG_PEND          1
#define AEEOBEX_SRV_STATE_REGISTERED        2
#define AEEOBEX_SRV_STATE_DEREG_PEND        3

//=============================================================================
//  SD UUID types
//=============================================================================
#define AEEOBEX_SRV_UUID_NONE 0
#define AEEOBEX_SRV_UUID128   1
#define AEEOBEX_SRV_UUID32    2
#define AEEOBEX_SRV_UUID16    4
//=============================================================================
//  Maximum sessions per server object
//=============================================================================
#define AEEOBEX_MAX_SESSION_PER_SERVER      2

//=============================================================================
//  OBEX server session states
//=============================================================================
#define AEEOBEX_SSN_STATE_CREATED            1
#define AEEOBEX_SSN_STATE_CONN_REQ_PEND      2
#define AEEOBEX_SSN_STATE_CONN_STARTED       3
#define AEEOBEX_SSN_STATE_CONNECTED          4
#define AEEOBEX_SSN_STATE_PUT_REQ_PEND       5
#define AEEOBEX_SSN_STATE_PUT_STARTED        6
#define AEEOBEX_SSN_STATE_GET_REQ_PEND       7
#define AEEOBEX_SSN_STATE_GET_STARTED        8
#define AEEOBEX_SSN_STATE_SETPATH_REQ_PEND   9
#define AEEOBEX_SSN_STATE_SETPATH_STARTED    10
#define AEEOBEX_SSN_STATE_DISC_PEND          11
#define AEEOBEX_SSN_STATE_DISC_STARTED       12

typedef struct AEEOBEXSsn AEEOBEXSsn;
typedef struct AEEOBEXSrvSys AEEOBEXSrvSys;

//=============================================================================
// AEE OBEX Srv Session object
//=============================================================================
struct IOBEXSrvSession {
   const IOBEXSrvSessionVtbl    *pvt;
   uint32                uRef;
   IShell                *pIShell;
   AEECallback           cbAppClose;
   OEMOBEXAuthInfo       authChal;      // auth chal info received from remote
   boolean               bAuthChal;
   OEMOBEXAuthResp       authResp;      // auth resp info received from remote 
   boolean               bAuthResp;
   AEEOBEXSsn            *pSsnSys;      // pointer to sys memory allocated for
                                        // this object
   uint32                nMaxPacket;
};
//=============================================================================
// AEE OBEX Srv session object in system memory
//=============================================================================
struct AEEOBEXSsn{
   uint32                uRef;
   int                   state;
   uint16                targetId;
   uint16                connId;
   boolean               bConnOriented;
   AEEOBEXEvtQueue       appEvtQueue;  // app.event queue needs to
                                       // be in the sys memory as the aee object
                                       // is not created until app reads event
   ISignalBus            *piSigBusAppEvt; // signal to register for OEM events to
                                          // be processed in the ui task context
   boolean               bAvailable;   // for a new session
   AEEOBEXSrvSys         *pSrvSys;     // pointer to server system object
   IOBEXSrvSession       *pSsn;        // pointer to the AEE session object
};

//=============================================================================
// AEE OBEX Srv session event to be delivered to the application
//=============================================================================
typedef struct {
   uint8      event;
   uint32     status;
   uint32     flags;
   uint32     packet;
   OEMOBEXHdr *pOemHdr;
}AEEOBEXSsnEvt;

//=============================================================================
// AEE OBEX Srv Object
//=============================================================================
struct IOBEXSrv {
   const IOBEXSrvVtbl    *pvt;
   uint32                uRef;
   IShell                *pIShell;
   AEECallback           cbAppClose;
   uint32                suppTransport;  // supported transports
   ISignalBus            *piSigBusAppEvt;// signal bus to put the app's Signal
   AEEOBEXSrvSys         *pSrvSys;      // pointer to sys memory allocated for
                                        // this object
   AEEOBEXEvtQueue       appEvtQueue;   // queue of events to be delivered to app
};

//=============================================================================
// AEE OBEX Srv Object in system memory
//=============================================================================
struct AEEOBEXSrvSys {
   uint32             uRef;
   int                state;
   AEEOBEXSsn         aSsnObj[AEEOBEX_MAX_SESSION_PER_SERVER];
   OEMOBEXHdr*        aRecvHdr[OEMOBEX_MAX_RECEIVE_BUFF];
   OEMOBEXHdr*        aSendHdr[AEEOBEX_MAX_SEND_BUFF];
   IOBEXSrv           *pSrv;           // reference to the AEE object 
   AEECallback        evSignalCb;
   AEEOBEXEvtQueue    sysEvtQueue;     // event queue for the OEM layer events
   OEMOBEXSrv         *pSrvOem;        // pointer to the oem object for server
   IBT                *piBT;
   IBTMgr             *piMgr;
   IBTServiceDiscovery *piSd;
   uint32              uSDReg;
   ISignalCtl         *piSigCtlSecEvt;
   ISignalCtl         *piSigCtlSdEvt;
   uint32              scn; 
   char                uuid128[33]; // ASCII rep of 128 bit uuid 16 *2 bytes + 1
   uint32              uuid32;
   OEMOBEX_TransportConfigInfo config;
   uint32              sdHandle;
   uint16              uuid16;
};
//=============================================================================
// OBEX Srv event to be delivered to the application
//=============================================================================
typedef struct {
   uint8       event;
   uint32      status;
   AEEOBEXSsn  *pSsn;
}AEEOBEXSrvEvt;

//=============================================================================
// Interface function definitions for IOBEXSrvSession
//=============================================================================
static uint32 AEEOBEXSsn_AddRef(IOBEXSrvSession *me);
static uint32 AEEOBEXSsn_Release(IOBEXSrvSession *me);
static int AEEOBEXSsn_QueryInterface(IOBEXSrvSession *me,
                                     AEECLSID iid, void **ppo);
static int AEEOBEXSsn_Authenticate(IOBEXSrvSession  *me,
                                   AEEOBEXCharSet  charSet,
                                   uint8           *pRealm,
                                   int             nRealmLen,
                                   boolean         uid,
                                   boolean         acs);
static int AEEOBEXSsn_CreateHeaderSet(IOBEXSrvSession *me, IOBEXHeader **ppif);

static int AEEOBEXSsn_OnEventNotify(IOBEXSrvSession *me,
                                    ISignal      *piSignal);

static int AEEOBEXSsn_GetEvent(IOBEXSrvSession   *me,
                               AEEOBEXEventType  *pEvent,
                               IOBEXHeader       **ppiOBEXHdr,
                               AEEOBEXEvent      *pEventData);

static int AEEOBEXSsn_SendResponse(IOBEXSrvSession  *me,
                             AEEOBEXRequestType     request,
                             IOBEXHeader            *piOBEXHdr,
                             AEEOBEXCmdStatus       cmdStatus,
                             AEEOBEXFlags           flags);

static int AEEOBEXSsn_Disconnect(IOBEXSrvSession      *me);
static int AEEOBEXSsn_SetUidPassword(IOBEXSrvSession  *me,
                                     const uint8      *pUid,
                                     int              nUidLen,
                                     const uint8      *pPswd,
                                     int              nPswdLen);

static int AEEOBEXSsn_GetAuthChallenge(IOBEXSrvSession    *me,
                                       boolean            *pbUserIdReq,
                                       boolean            *pbFullAccess,
                                       AEEOBEXCharSet     *pCharSet,
                                       uint8              *pRealm,
                                       int                nRealmLen,
                                       int                *pnRealmLenReq );
static int AEEOBEXSsn_GetAuthResponse(IOBEXSrvSession        *me,
                                      AEEOBEXAuthResp *pAuthResp);

static int AEEOBEXSsn_GetParams(IOBEXSrvSession   *me,
                                AEEOBEXParamType  param,
                                uint32            *pU32,
                                uint8             *pParam,
                                int               nParamLen,
                                int               *pnParamLenReq);

//=============================================================================
//  IOBEXSrvSession vector table
//=============================================================================
static const IOBEXSrvSessionVtbl g_Vtbl_OBEXSsn = {
   AEEOBEXSsn_AddRef,
   AEEOBEXSsn_Release,
   AEEOBEXSsn_QueryInterface,
   AEEOBEXSsn_CreateHeaderSet,
   AEEOBEXSsn_Authenticate,
   AEEOBEXSsn_OnEventNotify,
   AEEOBEXSsn_GetEvent,
   AEEOBEXSsn_SendResponse,
   AEEOBEXSsn_Disconnect,
   AEEOBEXSsn_SetUidPassword,
   AEEOBEXSsn_GetAuthChallenge,
   AEEOBEXSsn_GetAuthResponse,
   AEEOBEXSsn_GetParams
};

//=============================================================================
//  Interface function definitions for IOBEXSrv 
//=============================================================================
static uint32 AEEOBEXSrv_AddRef(IOBEXSrv *me);
static uint32 AEEOBEXSrv_Release(IOBEXSrv *me) ;
static int AEEOBEXSrv_QueryInterface(IOBEXSrv *me,
                                     AEECLSID iid, void **ppo);
static int AEEOBEXSrv_GetSupportedTransports(IOBEXSrv *me,
                                             AEEOBEXTransport *pTransport,
                                             int nLen,
                                             int *pnLenReq);
static int AEEOBEXSrv_Register(IOBEXSrv          *me,
                               AEEOBEXTransport  transType,
                               uint8             *pTarget,
                               int               nTargetLen,
                               const char              *pszTransport);
static int AEEOBEXSrv_DeRegister(IOBEXSrv *me);

static int AEEOBEXSrv_OnEventNotify(IOBEXSrv     *me,
                                    ISignal      *piSignal);
static int AEEOBEXSrv_GetEvent(IOBEXSrv           *me,
                               AEEOBEXEventType   *pEvent,
                               AEEOBEXCmdStatus   *pCmdStatus,
                               IOBEXSrvSession    **ppiSrvSession);
static int AEEOBEXSrv_GetParams(IOBEXSrv          *me,
                                AEEOBEXParamType  param,
                                uint32            *pU32,
                                uint8             *pParam,
                                int               nParamLen,
                                int               *pnParamLenReq);

//=============================================================================
// IOBEXSrv vector table
//=============================================================================
static const IOBEXSrvVtbl g_Vtbl_OBEXSrv = {
   AEEOBEXSrv_AddRef,
   AEEOBEXSrv_Release,
   AEEOBEXSrv_QueryInterface,
   AEEOBEXSrv_GetSupportedTransports,
   AEEOBEXSrv_Register,
   AEEOBEXSrv_DeRegister,
   AEEOBEXSrv_OnEventNotify,
   AEEOBEXSrv_GetEvent,
   AEEOBEXSrv_GetParams
};

//=============================================================================
// static functions 
//=============================================================================
static int AEEOBEXSrv_SysInit(IOBEXSrv *me);
static uint32 AEEOBEXSrv_Delete(IOBEXSrv *me);
static int AEEOBEXSrv_SsnRelease(AEEOBEXSsn *pSsn);
static int AEEOBEXSrv_SsnInit(AEEOBEXSsn *pSsn);

static int AEEOBEXSsn_ConnectResponse(IOBEXSrvSession  *me,
                               IOBEXHeader      *piOBEXHdr,
                               AEEOBEXCmdStatus cmdStatus);
static int AEEOBEXSsn_PutResponse(IOBEXSrvSession *me,
                                  IOBEXHeader *piOBEXHdr,
                                  AEEOBEXCmdStatus cmdStatus);
static int AEEOBEXSsn_GetResponse(IOBEXSrvSession *me,
                                  IOBEXHeader *piOBEXHdr,
                                  AEEOBEXCmdStatus cmdStatus);
static int AEEOBEXSsn_SetPathResponse(IOBEXSrvSession *me,
                                      IOBEXHeader *piOBEXHdr,
                                      AEEOBEXCmdStatus cmdStatus);
static int AEEOBEXSsn_DisconnectResponse(IOBEXSrvSession *me,
                                         IOBEXHeader *piOBEXHdr,
                                         AEEOBEXCmdStatus cmdStatus);

static void AEEOBEXSrv_EvtError(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtAuthComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtAuthResp(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtAuthChallenge(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtDeRegComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtRegComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtAbort(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt) ;//#check
static void AEEOBEXSrv_EvtSetPathReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtGetReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtPutReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtNewSession(AEEOBEXSrvSys *pSrvSys, AEEOBEXSsn *pSsn);
static void AEEOBEXSrv_EvtConnReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtDiscReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static void AEEOBEXSrv_EvtConnComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt);
static int  AEEOBEX_ParseBtConfigStr(OEMOBEX_TransportConfigInfo *pConfig,
                              const char *pszTransport,AEEOBEXSrvSys *pSrvSys);
static int AEEOBEXSrvSession_GetObject(IShell* pIShell,
                                       IOBEXSrvSession **ppif,
                                       AEEOBEXSsn *pSsnSys);
static AEEOBEXSsn* AEEOBEXSrv_GetSsnFromConnId(AEEOBEXSrvSys *pSrvSys,
                                               uint16 connId,
                                               boolean bNewSsn);
static uint32 AEEOBEXSsn_Delete(IOBEXSrvSession *me) ;
static boolean AEEOBEXSrv_RecvHdrAvailable(AEEOBEXSsn *pSsnSys);
static int AEEOBEXSrv_CreateSsn(AEEOBEXSsn *pSsnSys);
static int AEEOBEXSrv_CleanUp(AEEOBEXSrvSys *pSrvSys);
static boolean AEEOBEXSrv_SendHdrValid(AEEOBEXSsn *pSsnSys, OEMOBEXHdr *pOemHdr);
static int AEEOBEX_UpdateSecurity(AEEOBEXSrvSys *pSrvSys,BTSecurityLevel dSec);
static void AEEOBEXSrv_BtMgrCallback(void* dat);
static void AEEOBEXSrv_SdCallback(void* dat);
static int  AEEOBEXSrv_RegBtMgrCallback(AEEOBEXSrvSys *pSrvSys);
static int  AEEOBEXSrv_RegBtSdCallback(AEEOBEXSrvSys *pSrvSys);
static int AEEOBEXSrv_RegisterService(AEEOBEXSrvSys *pSrvSys);
static int AEEOBEXSrv_SendEvent(AEEOBEXSrvSys *pSrvSys,
                                AEEOBEXEventType evt,
                                AEEOBEXCmdStatus stat,
                                AEEOBEXSsn  *pSsn);
static void AEEOBEXSrv_RegCleanUp(AEEOBEXSrvSys *pSrvSys);
static int AEEOBEXSrv_FormatSdString(char *pBuf, int maxLen, AEEOBEXSrvSys *pSrvSys);
//=============================================================================
// OEM callback 
//=============================================================================

void AEEOBEXSrv_OEMEventCb( void            *cbData,
                            OEMOBEXEvent    *pEvt,
                            OEMOBEXAuthInfo *pAuthChal,
                            OEMOBEXAuthResp *pAuthResp);
//=============================================================================
// Callback called in UI task context to process OEM events
//=============================================================================
void AEEOBEXSrv_SysEvtCallback(void* pme);


//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_SysInit(IOBEXSrv *me)
{
   AEEOBEXSrvSys    *pSysObj           = NULL;
   int  ret = AEE_SUCCESS;
   int  i    = 0;

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_SysInit ->",0,0,0);

   pSysObj = (AEEOBEXSrvSys*)OEM_Malloc(sizeof(AEEOBEXSrvSys));

   if (NULL == pSysObj)
   {
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)pSysObj, 0, sizeof(AEEOBEXSrvSys));

   pSysObj->state         = AEEOBEX_SRV_STATE_CREATED;
   pSysObj->uRef          = 1;
   pSysObj->pSrv          = me;
   me->pSrvSys            = pSysObj;
   pSysObj->piBT          = NULL;
   pSysObj->piMgr         = NULL;

   // to hold events from oem for the server
   ret = AEEOBEXEvtQ_Init(&pSysObj->sysEvtQueue,
                          AEEOBEX_MAX_EVENTS,
                          sizeof(OEMOBEXEvent));

   if (AEE_SUCCESS != ret)
   {
      // cannot allocate event queue
      MSG_ERROR("AEE OBEX cannot alloc sys evt Q",0,0,0);
      FREEIF(me);
      return AEE_EFAILED;
   }

   // initialize the oem layer and register a callback that 
   // the oem layer calls when it needs to deliver an event to the
   // AEE layer.
   for (i = 0; i < AEEOBEX_MAX_SESSION_PER_SERVER; i++)
   {
      AEEOBEXSrv_SsnInit(&pSysObj->aSsnObj[i]);
   }

   ret = OEMOBEXSrv_init(&pSysObj->pSrvOem, pSysObj, AEEOBEXSrv_OEMEventCb);

   if (AEE_SUCCESS != ret)
   {  // clean up
      int i = 0;

      MSG_DEBUG("AEE OBEX srv sys init failed cleaning up...",0,0,0);
      if (NULL != pSysObj->pSrvOem)
      {
         OEMOBEXSrv_Release(pSysObj->pSrvOem);
         pSysObj->pSrvOem = NULL;
      }
      // clean up any send headers that may have been acquired
      for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if (NULL != pSysObj->aSendHdr[i])
         {
            OEMOBEXHdr_Release(pSysObj->aSendHdr[i]);
            pSysObj->aSendHdr[i] = NULL;
         }
      }

      me->pSrvSys    = NULL;
      AEEOBEXEvtQ_Free(&pSysObj->sysEvtQueue);
      OEM_Free(pSysObj);
   }

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_SysInit ret <-",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSrv_AddRef( IOBEXSrv *me )
{
   return ++me->uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSrv_Release( IOBEXSrv *me )
{
   uint32 uRef = --me->uRef;

   MSG_MED("AEE OBEX Srv rel uRef ",uRef,0,0);

   if (0 == uRef)
   {
      AEEOBEXSrv_Delete(me);
   }
   return uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSrv_SysRelease(AEEOBEXSrvSys *pSrvSys)
{
   boolean bRelease = FALSE;

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_SysRelease ->",0,0,0);

   MSG_MED("AEE OBEX AEEOBEXSrv_SysRelease state %x, uref %x "
           ,pSrvSys->state,pSrvSys->uRef,0);

   if (0 == pSrvSys->uRef)
   {
#ifdef BT_SIMULATION
      pSrvSys->state =     AEEOBEX_SRV_STATE_CREATED ;//#
#endif
      // the app wants to free the object
      if (AEEOBEX_SRV_STATE_CREATED == pSrvSys->state) 
      {
         bRelease = TRUE;
      }
      else
      {
         MSG_HIGH("AEE OBEX srv sys rel calling clean up...  state %x, ",pSrvSys->state,0,0);
         AEEOBEXSrv_CleanUp(pSrvSys);
      }
   }
   else if (AEEOBEX_SRV_STATE_DEREG_PEND == pSrvSys->state)
   {
      // the server has choosen to de register with open sessions
      MSG_HIGH("AEE OBEX srv sys rel state de reg pending ...%x, ",pSrvSys->state,0,0);
      AEEOBEXSrv_CleanUp(pSrvSys);
   }

   if (bRelease)
   {
      int i = 0;

      MSG_MED("AEE OBEX Srv releasing sys object...",0,0,0);

      if (NULL != pSrvSys->pSrvOem)
      {
         MSG_MED("AEE OBEX calling oem release...",0,0,0);
         OEMOBEXSrv_Release(pSrvSys->pSrvOem);
         pSrvSys->pSrvOem = NULL;
      }
      CALLBACK_Cancel(&pSrvSys->evSignalCb);

      AEEOBEXSrv_RegCleanUp(pSrvSys);
     
      // release the send headers
      MSG_DEBUG("AEE OBEX releasing send hdrs",0,0,0);
      for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if (NULL != pSrvSys->aSendHdr[i])
         {
            MSG_DEBUG("AEE OBEX releasing send hdrs ind %x",i,0,0);
            OEMOBEXHdr_Release(pSrvSys->aSendHdr[i]);
            pSrvSys->aSendHdr[i] = NULL;
         }
      }

      MSG_DEBUG("AEE OBEX freeing sys evt Q",0,0,0);
      AEEOBEXEvtQ_Free(&pSrvSys->sysEvtQueue);
      
      MSG_DEBUG("AEE OBEX freeing sys object",0,0,0);
      OEM_Free(pSrvSys);
   }

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_SysRelease <-",0,0,0);
   return AEE_SUCCESS; //#check
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSrv_Delete( IOBEXSrv *me )
{
   MSG_DEBUG("AEE OBEX AEEOBEXSrv_Delete ->",0,0,0);

   // this function is called when the server object
   // refernce count goes to zero Or the app
   // unloads without freeing the object.
   // decrement the reference count of the sys object so that
   // it can clean up.
   // release the AEE object 
   --me->pSrvSys->uRef;
   me->pSrvSys->pSrv = NULL;
   
   AEEOBEXSrv_SysRelease(me->pSrvSys);
   me->pSrvSys = NULL;
   
   // this queue is always accessed within the UI task.so
   // it can be freed without using a critic section
   AEEOBEXEvtQ_Free(&me->appEvtQueue);
   RELEASEIF(me->piSigBusAppEvt);
   CALLBACK_Cancel(&me->cbAppClose);
   RELEASEIF(me->pIShell);
   FREEIF(me);

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_Delete <-",0,0,0);
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_QueryInterface( IOBEXSrv *me,
                                      AEECLSID iid, void **ppo )
{
   switch (iid)
   {
   case AEEIID_IQI:
   case AEEIID_IOBEXSrv:
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
static int AEEOBEXSrv_GetSupportedTransports( IOBEXSrv *me,
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
static int AEEOBEXSrv_Register(IOBEXSrv          *me,
                               AEEOBEXTransport  transType,
                               uint8             *pTarget,
                               int               nTargetLen,
                               const char        *pszTransport)
{
   int ret = AEE_EUNSUPPORTED;

   MSG_MED("AEE OBEX srv reg trans %x",transType,0,0);

   if (AEEOBEX_SRV_STATE_CREATED != me->pSrvSys->state)
   {
      MSG_ERROR("AEE OBEX srv reg bad state %x",me->pSrvSys->state,0,0);
      return AEE_EBADSTATE;
   }

   if ((0 != nTargetLen) &&
      ((NULL == pTarget) ||
       (!AEE_CHECK_PTR("AEEOBEXSrv_Register",
                        pTarget, nTargetLen, FALSE))))
   {
      MSG_ERROR("AEE OBEX srv bad param tgt len %x, pTarget %x"
         ,nTargetLen,pTarget,0);
      return AEE_EBADPARM;
   }

      // initialize the config structure
   MEMSET((void*)&me->pSrvSys->config, 0, sizeof(OEMOBEX_TransportConfigInfo));
   me->pSrvSys->scn = 0;
   me->pSrvSys->uSDReg = 0;
   me->pSrvSys->uuid32 = 0xFFFFFFFF;
   MEMSET((void*)&me->pSrvSys->uuid128, 0, sizeof(me->pSrvSys->uuid128));
 

   if (0 != nTargetLen)
   {
      me->pSrvSys->config.pTargetId  = pTarget;
      me->pSrvSys->config.nTargetLen = nTargetLen;
   }
   // parse the string
   switch(transType)
   {
      case AEEOBEX_TRANS_BT_RFCOMM:
      {
         if (me->suppTransport & OEMOBEX_TRANS_BT_RFCOMM)
         {
            me->pSrvSys->config.transport = OEMOBEX_TRANS_BT_RFCOMM;
            ret = AEEOBEX_ParseBtConfigStr(&me->pSrvSys->config,pszTransport,me->pSrvSys);
         } // else ret is already AEE_EUNSUPPORTED
         break;
      }
      default:
         // ret is already AEE_EUNSUPPORTED
         break;
   }

   if (AEE_SUCCESS == ret)
   {
      MSG_MED("AEE OBEX srv parsed config str. calling oem reg...",0,0,0);
      // we were able to parse the string 
      ret = OEMOBEXSrv_Register(me->pSrvSys->pSrvOem, &me->pSrvSys->config);

      if (AEE_SUCCESS == ret)
      {
         me->pSrvSys->state = AEEOBEX_SRV_STATE_REG_PEND;
      }
   }

   MSG_MED("AEE OBEX srv reg compl ret %x",ret,0,0);
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_CleanUp(AEEOBEXSrvSys *pSrvSys)
{
   int i = 0;
   int ret = AEE_SUCCESS;

   MSG_HIGH("AEE OBEX srv Clean up state %x,",pSrvSys->state,0,0);

   for (i = 0; i < AEEOBEX_MAX_SESSION_PER_SERVER; i++)
   {
      if (AEEOBEX_SSN_STATE_CREATED != pSrvSys->aSsnObj[i].state)
      {
         MSG_HIGH("AEE OBEX srv ssn state not init state %x, ind %x",pSrvSys->state,i,0);
         // de link the app session object from the sys object
         if (pSrvSys->aSsnObj[i].pSsn != NULL)
         {
            pSrvSys->aSsnObj[i].pSsn->pSsnSys = NULL;
         }
         RELEASEIF(pSrvSys->aSsnObj[i].piSigBusAppEvt);
         pSrvSys->aSsnObj[i].piSigBusAppEvt = NULL;

         pSrvSys->aSsnObj[i].uRef = 0;
         if (AEEOBEX_SSN_STATE_DISC_STARTED != pSrvSys->aSsnObj[i].state)// disconnect started already,
         {
            MSG_HIGH("AEE OBEX srv calling force disconnect state %x, ind %x",pSrvSys->aSsnObj[i].state,i,0);

            ret = OEMOBEXSrv_Disconnect(pSrvSys->pSrvOem, pSrvSys->aSsnObj[i].connId);
            if (AEE_SUCCESS == ret)
            {
               pSrvSys->aSsnObj[i].state = AEEOBEX_SSN_STATE_DISC_STARTED;
            }
            else
            {
               MSG_ERROR("AEE OBEX srv cannot disconnect state %x",pSrvSys->aSsnObj[i].state,0,0);
            }
         }
         break;
      }
   }

   if (i == AEEOBEX_MAX_SESSION_PER_SERVER)
   {
      // all sessions were closed.
      // we can now de register
      MSG_HIGH("AEE OBEX srv clean up. no sessions. calling de register. state %x, ",pSrvSys->state,0,0);
      AEEOBEXSrv_RegCleanUp(pSrvSys);
      ret = OEMOBEXSrv_DeRegister(pSrvSys->pSrvOem);
      if (AEE_SUCCESS == ret)
      {
         pSrvSys->state = AEEOBEX_SRV_STATE_DEREG_PEND;
      }
      else
      {
         MSG_ERROR("AEE OBEX srv cannot de reg!!! state %x",pSrvSys->state,0,0);
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_DeRegister(IOBEXSrv *me)
{
   int ret = AEE_SUCCESS;

   MSG_MED("AEE OBEX srv de reg state %x",me->pSrvSys->state,0,0);

   if (AEEOBEX_SRV_STATE_REGISTERED != me->pSrvSys->state)
   {
      MSG_ERROR("AEE OBEX srv de reg bad state",0,0,0);
      ret = AEE_EBADSTATE;
   }
   else
   {
      me->pSrvSys->state = AEEOBEX_SRV_STATE_DEREG_PEND;
      AEEOBEXSrv_CleanUp(me->pSrvSys);
   }
   MSG_MED("AEE OBEX srv de reg ret ",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_OnEventNotify(IOBEXSrv     *me,
                                    ISignal      *piSignal)
{
   int ret = AEE_SUCCESS;

   if (NULL == piSignal)
   {
      return AEE_EBADPARM;
   }
   ret = ISignalBus_Add(me->piSigBusAppEvt, piSignal);

   MSG_MED("AEE OBEX app reg for signal srvObj %x, sig %x , ret %x "
            ,me,piSignal,ret);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_GetEvent(IOBEXSrv           *me,
                               AEEOBEXEventType   *pEvent,
                               AEEOBEXCmdStatus   *pCmdStatus,
                               IOBEXSrvSession    **ppiSrvSession)
{
   int ret = AEE_ENOMORE;
   AEEOBEXSrvEvt evt;

   MSG_MED("AEE OBEX srv get event",0,0,0);

   if ((NULL == pEvent) || (NULL == pCmdStatus) || (NULL == ppiSrvSession) ||
       (!AEE_CHECK_PTR("AEEOBEXSrv_GetEvent",
                        pEvent, sizeof(AEEOBEXEventType), TRUE)) ||
       (!AEE_CHECK_PTR("AEEOBEXSrv_GetEvent",
                        pCmdStatus, sizeof(AEEOBEXCmdStatus), TRUE))||
       (!AEE_CHECK_PTR("AEEOBEXSrv_GetEvent",
                        ppiSrvSession, sizeof(IOBEXSrvSession*), TRUE)))
   {
      MSG_ERROR("AEE OBEX get evt bad params pEvent %x, pCmdStatus %x , ppiSrvSession %x "
               ,pEvent,pCmdStatus,ppiSrvSession);
      return AEE_EBADPARM;
   }

   // pop the first event on the queue.
   ret = AEEOBEXEvtQ_Get(&me->appEvtQueue, (uint8*)&evt);

   if (AEE_SUCCESS == ret)
   {
      MSG_MED("AEE OBEX srv got event %x, stat %x, ssn %x",
               evt.event,evt.status,evt.pSsn);

      *pEvent     = evt.event;
      *pCmdStatus = evt.status;
      *ppiSrvSession = NULL;

      if (NULL != evt.pSsn)
      { 
         ret = AEEOBEXSrvSession_GetObject(me->pIShell, ppiSrvSession, evt.pSsn);
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

static int AEEOBEXSrv_GetParams(IOBEXSrv          *me,
                                AEEOBEXParamType  param,
                                uint32            *pU32,
                                uint8             *pParam,
                                int               nParamLen,
                                int               *pnParamLenReq)
{
   int ret = AEE_EUNSUPPORTED;

   MSG_MED("AEE OBEX srv get param %x",param,0,0);

   switch(param)
   {
      case AEEOBEXPARAM_BT_BDA:
      {
         if ((nParamLen >= AEEOBEX_MAX_BDA_LEN) &&
             (NULL != pParam))
         {
            MSG_MED("AEE OBEX srv get bda",0,0,0);
            ret = OEMOBEXSrv_GetBda(me->pSrvSys->pSrvOem, pParam);
         }
         else
         {
            ret = AEE_EBUFFERTOOSMALL;
         }

         if (NULL != pnParamLenReq)
         {
            *pnParamLenReq = AEEOBEX_MAX_BDA_LEN;
         }
         break;
      }
      case AEEOBEXPARAM_BT_RFCOMM_SCN:
      {
         if (NULL != pU32)
         {
            ret = OEMOBEXSrv_GetScn(me->pSrvSys->pSrvOem, pU32);
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
void AEEOBEXSrv_OEMEventCb( void            *cbData,
                            OEMOBEXEvent    *pEvt,
                            OEMOBEXAuthInfo *pAuthChal,
                            OEMOBEXAuthResp *pAuthResp)
{
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)cbData;
   AEEOBEXSsn  *pSsnSys = NULL;
   int i = 0;
   /*
   MSG_MED("AEE OBEX AEEOBEXSrv_OEMEventCb evt %x, authchal %x, authReap %x",
            pEvt->event,pAuthChal,pAuthResp);

   MSG_MED("AEE OBEX AEEOBEXSrv_OEMEventCb srv sys %x",
            cbData,0,0);
   */

   for (i = 0; i < AEEOBEX_MAX_SESSION_PER_SERVER; i++)
   {
      if (pSrvSys->aSsnObj[i].connId == pEvt->u.srv.connId)
      {
         MSG_MED("AEE OBEX session found %x",i,0,0);
         pSsnSys = &pSrvSys->aSsnObj[i];
         break;
      }
   }

   if (NULL != pAuthResp) 
   {
      if ((NULL != pSsnSys) && (NULL != pSsnSys->pSsn))
      {
         pSsnSys->pSsn->authResp = *pAuthResp;
      }
   }
   if ( NULL != pAuthChal ) 
   {
      if ((NULL != pSsnSys) && (NULL != pSsnSys->pSsn))
      {
         pSsnSys->pSsn->authChal = *pAuthChal; 
         //bAuthchal is set to true when the event is handed in ui task
      }
   }
   AEEOBEXEvtQ_Put(&pSrvSys->sysEvtQueue, (uint8*)pEvt);
   CALLBACK_Cancel(&pSrvSys->evSignalCb);
   CALLBACK_Init(&pSrvSys->evSignalCb, AEEOBEXSrv_SysEvtCallback, pSrvSys);
   AEE_SYS_RESUME(&pSrvSys->evSignalCb);
}



//=============================================================================
// 
//
//=============================================================================
void AEEOBEXSrv_SysEvtCallback(void* pme)
{
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)pme;
   OEMOBEXEvent  evt;
   int ret   = AEE_EFAILED;

   MSG_MED("AEE OBEX Srv sys evt cb srv sys %x",
            pSrvSys,0,0);
   // pop the first event on the queue.
   ret = AEEOBEXEvtQ_Get(&pSrvSys->sysEvtQueue, (uint8*)&evt);

   while (AEE_SUCCESS == ret)
   {
      MSG_MED("AEE OBEX Srv Sys Evt %x" ,evt.event, 0, 0);

      switch (evt.event)
      {
         case AEEOBEX_EVT_SRV_CONN_REQ:
            AEEOBEXSrv_EvtConnReq(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_DISC_REQ:
         case OEMOBEX_EVT_SRV_DISC_COMPL:
            AEEOBEXSrv_EvtDiscReq(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_PUT_REQ:
            AEEOBEXSrv_EvtPutReq(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_GET_REQ:
            AEEOBEXSrv_EvtGetReq(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_SETPATH_REQ:
            AEEOBEXSrv_EvtSetPathReq(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_ABORTED:
            AEEOBEXSrv_EvtAbort(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_REG_CMPL:
            AEEOBEXSrv_EvtRegComplete(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_DEREG_CMPL:
            AEEOBEXSrv_EvtDeRegComplete(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_AUTH_CHALLENGE:
            AEEOBEXSrv_EvtAuthChallenge(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_AUTH_RESPONSE:
            AEEOBEXSrv_EvtAuthResp(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_AUTH_COMPL:
            AEEOBEXSrv_EvtAuthComplete(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_ERROR:
            AEEOBEXSrv_EvtError(pSrvSys, &evt);
            break;
         case AEEOBEX_EVT_SRV_CONN_COMPL:
            AEEOBEXSrv_EvtConnComplete(pSrvSys, &evt);
         break;
         default :
            break;
      }
      ret = AEEOBEXEvtQ_Get(&pSrvSys->sysEvtQueue, (uint8*)&evt);
   }
   if ((pSrvSys->uRef == 0) ||
      (AEEOBEX_SRV_STATE_DEREG_PEND == pSrvSys->state))
   {
      // note : sysRelease deletes the object so pCliSys should not
      // be referenced after this point
      MSG_HIGH("AEE OBEX srv sys cb.calling sys release...",0,0,0);
      AEEOBEXSrv_SysRelease(pSrvSys);
   }
}

//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXSrv_EvtConnReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsn *pSsn = NULL;

   MSG_MED("AEE OBEX srv conn req, state %x" ,pSrvSys->state, 0, 0);

   if (AEEOBEX_SRV_STATE_REGISTERED == pSrvSys->state)
   {
      // see if there is a session object available
      pSsn = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, TRUE);

      if (NULL != pSsn)
      {
         AEEOBEXSsnEvt   event;

         MSG_HIGH("AEE OBEX Srv New Session",0,0,0);
         event.event   = AEEOBEX_EVT_SRV_CONN_REQ;
         event.flags   = 0;
         event.pOemHdr = pEvt->u.srv.pOemHdr;
         event.status  = AEE_SUCCESS;

         pSsn->state    = AEEOBEX_SSN_STATE_CONN_REQ_PEND;
         pSsn->targetId = pEvt->u.srv.targetId;
         pSsn->connId   = pEvt->u.srv.connId;
         AEEOBEXEvtQ_Put(&pSsn->appEvtQueue, (uint8*)&event);
         if (pSsn->piSigBusAppEvt)
         {
            ISignalBus_Strobe(pSsn->piSigBusAppEvt);
         }
      }
      else
      {
         MSG_ERROR( "AEE OBEX Srv Rec conn req, no available ssn",0,0,0);
         if (pEvt->u.srv.pOemHdr)
         {
            OEMOBEXHdr_Release(pEvt->u.srv.pOemHdr);
         }
      }

   }// else we could be de registering. in which case ignore the event
   return;
}
//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXSrv_EvtDiscReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsn = NULL;
   int newState;

   // the event is sent when the client disconnects, or if the 
   // transport connection is lost, or if authentication fails
   MSG_MED("AEE OBEX srv disc req, state %x" ,pSrvSys->state, 0, 0);

   pSsn = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsn)
   {
      MSG_MED(" AEE OBEX Srv ssn state %x ",pSsn->state,0,0);

      if ((pSsn->state >= AEEOBEX_SSN_STATE_CONNECTED) &&
          (pSsn->state != AEEOBEX_SSN_STATE_DISC_PEND) &&
          (pSsn->state != AEEOBEX_SSN_STATE_DISC_STARTED))
      {
         newState    = AEEOBEX_SSN_STATE_DISC_PEND;
         evt.event   = AEEOBEX_EVT_SRV_DISC_REQ;
         evt.status  = pEvt->u.srv.status;
         evt.flags   = 0;
         evt.pOemHdr = pEvt->u.srv.pOemHdr;
         evt.packet  = 0;
      }
      else
      {
         newState    = AEEOBEX_SSN_STATE_CREATED;
         evt.event   = AEEOBEX_EVT_ERROR;
         evt.status  = pEvt->u.srv.status;
         evt.flags   = 0;
         evt.pOemHdr = NULL;
         if (pEvt->u.srv.pOemHdr)
         {
            OEMOBEXHdr_Release(pEvt->u.srv.pOemHdr);
         }
         evt.packet  = 0;
         // some error occured.Make this session unavailable for 
         // the next connection till the app frees it.
         pSsn->connId = 0xFFFF;
         pSsn->bConnOriented = FALSE;
      
      }
      if ((pSsn->pSsn) && (pSsn->state != AEEOBEX_SSN_STATE_DISC_STARTED) &&
          (pSsn->piSigBusAppEvt))
      {
         AEEOBEXEvtQ_Put(&pSsn->appEvtQueue, (uint8*)&evt);
         ISignalBus_Strobe(pSsn->piSigBusAppEvt);
      }

      pSsn->state = newState;

      // we could reach here when the app has released the
      // object
      if (0 == pSsn->uRef)
      {
         AEEOBEXSrv_SsnRelease(pSsn);
      }
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================

static AEEOBEXSsn* AEEOBEXSrv_GetSsnFromConnId(AEEOBEXSrvSys *pSrvSys,
                                               uint16        connId,
                                               boolean       bNewSsn)
{
   int i = 0;
   AEEOBEXSsn *pSsn = NULL;
   int ret = AEE_SUCCESS;

   //#check, if the server always assigns a conn id
   for (i = 0; i < AEEOBEX_MAX_SESSION_PER_SERVER; i++)
   {
      if (pSrvSys->aSsnObj[i].connId == connId)
      {
         pSsn = &pSrvSys->aSsnObj[i];
         pSsn->bAvailable = FALSE;
         break;
      }
   }

   if ((TRUE == bNewSsn) && (AEEOBEX_MAX_SESSION_PER_SERVER == i))
   {
      // session not found. see if there is a session available.
      for (i = 0; i < AEEOBEX_MAX_SESSION_PER_SERVER; i++)
      {
         if (pSrvSys->aSsnObj[i].bAvailable == TRUE)
         {
            ret = AEEOBEXSrv_CreateSsn(&pSrvSys->aSsnObj[i]);

            if (AEE_SUCCESS == ret)
            {
               pSrvSys->aSsnObj[i].bAvailable = FALSE;
               pSsn = &pSrvSys->aSsnObj[i];
               pSsn->pSrvSys = pSrvSys;
               AEEOBEXSrv_EvtNewSession(pSrvSys, pSsn);
            }
            break;
         }
      }
   }

   return pSsn;
}
//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXSrv_EvtNewSession(AEEOBEXSrvSys *pSrvSys, AEEOBEXSsn *pSsn)
{
   MSG_DEBUG(" AEE OBEX AEEOBEXSrv_EvtNewSession -> ",0,0,0);

   if (NULL != pSrvSys->pSrv)
   {
      AEEOBEXSrvEvt event;


      MEMSET((void*)&event, 0, sizeof(event));

      event.event   = AEEOBEX_EVT_SRV_NEW_CLI;
      event.status  = AEE_SUCCESS;
      event.pSsn    = pSsn;

      AEEOBEXEvtQ_Put(&pSrvSys->pSrv->appEvtQueue, (uint8*)&event);
      ISignalBus_Strobe(pSrvSys->pSrv->piSigBusAppEvt);
   }

   MSG_DEBUG(" AEE OBEX AEEOBEXSrv_EvtNewSession <- ",0,0,0);
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtPutReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsn = NULL;

   MSG_MED("AEE OBEX srv put req, state %x" ,pSrvSys->state, 0, 0);

   pSsn = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, TRUE);

   if (NULL != pSsn)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSsn->state,0,0);

      MEMSET((void*)&evt, 0, sizeof(AEEOBEXSsnEvt));

      pSsn->state = AEEOBEX_SSN_STATE_PUT_REQ_PEND;
      evt.event   = AEEOBEX_EVT_SRV_PUT_REQ;
      evt.status  = pEvt->u.srv.status;
      evt.flags   = 0;
      evt.pOemHdr = pEvt->u.srv.pOemHdr;

      evt.packet  = (pEvt->u.srv.bFinal) ?
                    (evt.packet | AEEOBEX_PACKET_FINAL) :
                    (evt.packet | AEEOBEX_PACKET_INTERMEDIATE);

      AEEOBEXEvtQ_Put(&pSsn->appEvtQueue, (uint8*)&evt);
      if (pSsn->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsn->piSigBusAppEvt);
      }
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtGetReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsn = NULL;

   MSG_MED("AEE OBEX srv get req, state %x" ,pSrvSys->state, 0, 0);

   pSsn = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, TRUE);

   if (NULL != pSsn)
   {
      MEMSET((void*)&evt, 0, sizeof(AEEOBEXSsnEvt));

      MSG_MED(" AEE OBEX srv ssn state %x ",pSsn->state,0,0);

      pSsn->state = AEEOBEX_SSN_STATE_GET_REQ_PEND;
      evt.event   = AEEOBEX_EVT_SRV_GET_REQ;
      evt.status  = pEvt->u.srv.status;
      evt.flags   = 0;
      evt.pOemHdr = pEvt->u.srv.pOemHdr;
      evt.packet  = (pEvt->u.srv.bFinal) ?
                    (evt.packet | AEEOBEX_PACKET_FINAL) :
                    (evt.packet | AEEOBEX_PACKET_INTERMEDIATE);

      AEEOBEXEvtQ_Put(&pSsn->appEvtQueue, (uint8*)&evt);
      if (pSsn->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsn->piSigBusAppEvt);
      }
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtSetPathReq(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsn = NULL;

   MSG_MED("AEE OBEX srv set path req, state %x" ,pSrvSys->state, 0, 0);

   pSsn = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, TRUE);

   if (NULL != pSsn)
   {
      MEMSET((void*)&evt, 0, sizeof(AEEOBEXSsnEvt));

      MSG_MED(" AEE OBEX srv ssn state %x ",pSsn->state,0,0);

      pSsn->state = AEEOBEX_SSN_STATE_SETPATH_REQ_PEND;
      evt.event   = AEEOBEX_EVT_SRV_SETPATH_REQ;
      evt.status  = pEvt->u.srv.status;
      evt.flags   = 0;
      evt.pOemHdr = pEvt->u.srv.pOemHdr;
      evt.packet  = 0;
      AEEOBEXEvtQ_Put(&pSsn->appEvtQueue, (uint8*)&evt);
      if (pSsn->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsn->piSigBusAppEvt);
      }
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtAbort(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsnSys = NULL;

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsnSys)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSsnSys->state,0,0);

      if (pSsnSys->bConnOriented)
      {
         pSsnSys->state = AEEOBEX_SSN_STATE_CONNECTED;
      }
      else
      {
         pSsnSys->state = AEEOBEX_SSN_STATE_CREATED;
      }
      evt.event   = AEEOBEX_EVT_SRV_ABORTED;
      evt.status  = pEvt->u.srv.status;
      evt.flags   = 0;
      evt.pOemHdr = pEvt->u.srv.pOemHdr;
      evt.packet  = 0;

      AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);
      if (pSsnSys->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
   }

   return;
}
//=============================================================================
// 
//
//=============================================================================


static void AEEOBEXSrv_DoRegComplete(AEEOBEXSrvSys *pSrvSys,
                                     boolean *pbNotify,
                                     AEEOBEXSrvEvt *pEvt)
{
   uint32 btSec = pSrvSys->config.transportConfig.btConfig.security;
   
   if ((AEEBT_SEC_NONE == btSec) &&
      ((0 == pSrvSys->uSDReg) || (AEEOBEX_SRV_UUID16 == pSrvSys->uSDReg)))
   {
      // dont have to set security or reg with SDP
      *pbNotify = TRUE;
       pSrvSys->state = AEEOBEX_SRV_STATE_REGISTERED;
   }
   else if (btSec != AEEBT_SEC_NONE)
   {
      if (AEE_SUCCESS != (AEEOBEX_UpdateSecurity(pSrvSys, btSec ))) // set the service security
      {
         // Couldnt update the security , do de register
         MSG_ERROR(" AEE OBEX Srv error updating service security",0,0,0);
         
         // cleanup
         AEEOBEXSrv_RegCleanUp(pSrvSys);

         if (AEE_SUCCESS != OEMOBEXSrv_DeRegister(pSrvSys->pSrvOem))
         {
            MSG_ERROR("AEE OBEX srv failed to de register server",0,0,0);
            pSrvSys->state = AEEOBEX_SRV_STATE_CREATED;
            pEvt->status = AEEOBEX_STAT_ERROR;
            *pbNotify = TRUE;
         }// else the de reg complete event will notify the app.
      }
   }// else the update security complete evt will notify the app
   else if ((0 != pSrvSys->uSDReg) && (AEEOBEX_SRV_UUID16 != pSrvSys->uSDReg))
   {
      // dont set the security,reg the 128 bit uuid directly
      AEEOBEXSrv_RegisterService(pSrvSys);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtRegComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSrvEvt event;
   int ret = AEE_SUCCESS;
   boolean bNotify = FALSE;

   MSG_MED("AEE OBEX srv reg compl, state %x" ,pSrvSys->state, 0, 0);

   MEMSET((void*)&event, 0, sizeof(event));

   event.event   = AEEOBEX_EVT_SRV_REG_CMPL;
   event.status  = pEvt->u.srv.status;
   event.pSsn    = NULL;

   if (AEE_SUCCESS == pEvt->u.srv.status)
   {
      if ((ret = OEMOBEXSrv_GetScn(pSrvSys->pSrvOem, &pSrvSys->scn)) != AEE_SUCCESS)
      {
         MSG_ERROR(" AEE OBEX Srv connot get server channel ret %x",ret,0,0);
      }
      AEEOBEXSrv_DoRegComplete(pSrvSys, &bNotify, &event);
   }
   else
   {
      pSrvSys->state = AEEOBEX_SRV_STATE_CREATED;
      bNotify = TRUE;
   }

   MSG_MED("AEE OBEX srv reg compl notify app state %x, bnotif %x" ,pSrvSys->state, bNotify, 0);
   if ((pSrvSys->pSrv) && (bNotify))
   {
      MSG_MED("AEE OBEX srv reg compl notify app state %x" ,pSrvSys->state, 0, 0);
      AEEOBEXEvtQ_Put(&pSrvSys->pSrv->appEvtQueue, (uint8*)&event);
      ISignalBus_Strobe(pSrvSys->pSrv->piSigBusAppEvt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtDeRegComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSrvEvt event;

   MSG_MED("AEE OBEX srv de reg complete state %x" ,pSrvSys->state, 0, 0);

   MEMSET((void*)&event, 0, sizeof(event));

   if (AEEOBEX_SRV_STATE_DEREG_PEND == pSrvSys->state)
   {
      event.event   = AEEOBEX_EVT_SRV_DEREG_CMPL;
      event.status  = AEE_SUCCESS;
      pSrvSys->uSDReg = 0;
      pSrvSys->scn = 0;
   }
   else if (AEEOBEX_SRV_STATE_REG_PEND == pSrvSys->state)
   {
      // this could happen if the server was registered
      // but the setSecurity or SDP reg procudure failed
      event.event   = AEEOBEX_EVT_SRV_REG_CMPL;
      event.status  = AEEOBEX_STAT_ERROR;
   }
   event.pSsn    = NULL;
   pSrvSys->state = AEEOBEX_SRV_STATE_CREATED;

   if (NULL != pSrvSys->pSrv)
   {
      AEEOBEXEvtQ_Put(&pSrvSys->pSrv->appEvtQueue, (uint8*)&event);
      ISignalBus_Strobe(pSrvSys->pSrv->piSigBusAppEvt);
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtAuthChallenge(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsnSys = NULL;

   MSG_MED("AEE OBEX srv auth chal req, state %x" ,pSrvSys->state, 0, 0);

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsnSys)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSrvSys->state,0,0);
      evt.event     = AEEOBEX_EVT_AUTH_CHALLENGE;
      evt.status    = AEE_SUCCESS;
      evt.flags     = 0;
      evt.pOemHdr   = NULL;

      if ((NULL != pSsnSys->pSsn) &&
         (pSsnSys->piSigBusAppEvt))
      {
         pSsnSys->pSsn->bAuthChal = TRUE;
         AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtAuthResp(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsnSys = NULL;

   MSG_MED("AEE OBEX srv auth resp, state %x" ,pSrvSys->state, 0, 0);

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsnSys)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSrvSys->state,0,0);
      evt.event     = AEEOBEX_EVT_AUTH_RESPONSE;
      evt.status    = AEE_SUCCESS;
      evt.flags     = 0;
      evt.pOemHdr   = NULL;
      if ((NULL != pSsnSys->pSsn) && (pSsnSys->piSigBusAppEvt))
      {
         pSsnSys->pSsn->bAuthResp = TRUE;
         AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtConnComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsnSys = NULL;

   MSG_MED("AEE OBEX srv evt conn compl, state %x" ,pSrvSys->state, 0, 0);

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if ((NULL != pSsnSys) && (AEEOBEX_SSN_STATE_CONN_STARTED == pSsnSys->state))
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSrvSys->state,0,0);

      if (AEE_SUCCESS == pEvt->u.srv.status)
      {
         pSsnSys->state = AEEOBEX_SSN_STATE_CONNECTED;
         pSsnSys->bConnOriented = TRUE;
      }
      else
      {
         pSsnSys->state = AEEOBEX_SSN_STATE_CREATED;
         pSsnSys->connId = 0xFFFF;
      }
      evt.event     = AEEOBEX_EVT_SRV_CONN_COMPL;
      evt.status    = pEvt->u.srv.status;
      evt.flags     = 0;
      evt.pOemHdr   = NULL;
      AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);
      if (pSsnSys->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
   }
   return;
}

//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtAuthComplete(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn   *pSsnSys = NULL;

   MSG_MED("AEE OBEX srv auth compl, state %x" ,pSrvSys->state, 0, 0);

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsnSys)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSrvSys->state,0,0);
      evt.event     = AEEOBEX_EVT_AUTH_COMPL;
      evt.status    = pEvt->u.srv.status;
      evt.flags     = 0;
      evt.pOemHdr   = NULL;
      AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);

      if (pSsnSys->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_EvtError(AEEOBEXSrvSys *pSrvSys, OEMOBEXEvent *pEvt)
{
   AEEOBEXSsn   *pSsnSys = NULL;
   AEEOBEXSsnEvt evt;

   MSG_MED("AEE OBEX srv evt error state %x" ,pSrvSys->state, 0, 0);

   pSsnSys = AEEOBEXSrv_GetSsnFromConnId(pSrvSys, pEvt->u.srv.connId, FALSE);

   if (NULL != pSsnSys)
   {
      MSG_MED(" AEE OBEX srv ssn state %x ",pSrvSys->state,0,0);
      evt.event     = AEEOBEX_EVT_ERROR;
      evt.status    = pEvt->u.srv.status;
      evt.flags     = 0;
      evt.pOemHdr   = NULL;
 
      AEEOBEXEvtQ_Put(&pSsnSys->appEvtQueue, (uint8*)&evt);
      if (pSsnSys->piSigBusAppEvt)
      {
         ISignalBus_Strobe(pSsnSys->piSigBusAppEvt);
      }
      pSsnSys->connId = 0xFFFF;
      pSsnSys->bConnOriented = FALSE;
   }
   return;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEXSrvSession_GetObject(IShell* pIShell, IOBEXSrvSession **ppif, AEEOBEXSsn *pSsnSys )
{
   IOBEXSrvSession *me = NULL;
   int ret = AEE_SUCCESS;

   me = MALLOCREC(IOBEXSrvSession);

   if (NULL == me)
   {
      AEEOBEXSrv_SsnRelease(pSsnSys);
      MSG_ERROR("AEE OBEX Srv, cannot alloc session object",0,0,0);
      return AEE_ENOMEMORY;
   }
   me->pvt                = &g_Vtbl_OBEXSsn;
   me->pIShell            = pIShell;
   me->uRef               = 1;
   me->pSsnSys            = pSsnSys;
   pSsnSys->uRef          = 1;
   pSsnSys->pSsn          = me;
   OEMOBEXSrv_GetPacketSize(pSsnSys->pSrvSys->pSrvOem,&me->nMaxPacket);
   
   if ((ret =  ISHELL_CreateInstance(pIShell, AEECLSID_SignalBus,
                                    (void**)&pSsnSys->piSigBusAppEvt)) != AEE_SUCCESS)
   {
      FREE(me);
      MSG_ERROR("AEE OBEX Srv, cannot alloc session signal %x",ret,0,0);
   }
   else
   {
      ISHELL_AddRef(pIShell);
      // Initialize the cb that will be called in case the app closes
      CALLBACK_Init(&me->cbAppClose, AEEOBEXSsn_Delete, me);
      AEE_LinkSysObject(&me->cbAppClose);

      *ppif = (void *)me;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSsn_AddRef(IOBEXSrvSession *me)
{
   return ++me->uRef;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSsn_Release(IOBEXSrvSession *me) 
{
   uint32 uRef = --me->uRef;

   MSG_MED("AEE OBEX srv ssn release uref %x",uRef,0,0);

   if (0 == uRef)
   {
      AEEOBEXSsn_Delete(me);
   }
   return uRef;

}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXSsn_Delete(IOBEXSrvSession *me) 
{
   MSG_MED("AEE OBEX srv ssn delete",0,0,0);

   if (me->pSsnSys != NULL)
   {
      MSG_MED("AEE OBEX srv calling ssn release",0,0,0);
      --me->pSsnSys->uRef;
      me->pSsnSys->pSsn = NULL;
      
      RELEASEIF(me->pSsnSys->piSigBusAppEvt);
      me->pSsnSys->piSigBusAppEvt = NULL;
      AEEOBEXSrv_SsnRelease(me->pSsnSys);
   }

   me->bAuthChal = FALSE;
   me->bAuthResp = FALSE;
   
   CALLBACK_Cancel(&me->cbAppClose);
   RELEASEIF(me->pIShell);
   FREEIF(me);

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_QueryInterface(IOBEXSrvSession *me,
                                     AEECLSID iid, void **ppo)
{
   
   switch (iid)
   {
   case AEEIID_IQI:
   case AEEIID_IOBEXSrvSession:
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
static int AEEOBEXSsn_Authenticate( IOBEXSrvSession *me,
                                    AEEOBEXCharSet  charSet,
                                    uint8           *pRealm,
                                    int             nRealmLen,
                                    boolean         uid,
                                    boolean         acs )
{
   AEEOBEXSsn *pSsn = NULL;
   OEMOBEXAuthInfo authReq;
   int ret = AEE_SUCCESS;
   pSsn = me->pSsnSys;

   MSG_MED("AEE OBEX srv ssn authenticate",0,0,0);

   if (NULL == pSsn)
   {
      MSG_ERROR("AEE OBEX srv ssn , no ssn object",0,0,0);
      return AEE_EBADSTATE;
   }
   if ((AEEOBEX_SSN_STATE_CONN_REQ_PEND != pSsn->state) &&
       (AEEOBEX_SSN_STATE_GET_REQ_PEND != pSsn->state) &&
       (AEEOBEX_SSN_STATE_PUT_REQ_PEND != pSsn->state)&&
       (AEEOBEX_SSN_STATE_SETPATH_REQ_PEND != pSsn->state))
   {
      MSG_ERROR("AEE OBEX srv ssn authenticate bad state %x",pSsn->state,0,0);
      return AEE_EBADSTATE;
   }

   if ((0 != nRealmLen) && 
      ((NULL == pRealm) || (!AEE_CHECK_PTR("AEEOBEXSsn_Authenticate",
                        pRealm, nRealmLen, TRUE)) ))
   {
      return AEE_EBADPARM;
   }

   authReq.bFullAccess = acs;
   authReq.bUserIdReq  = uid;
   authReq.nRealmLen   = 0;
   authReq.charSet     = 0;

   MEMSET((void*)&authReq.aRealm[0], 0, sizeof(authReq.aRealm));

   if (0 != nRealmLen)
   {
      int len = 0;

      authReq.charSet   = charSet;
      // verify that the nRealmLen does not exceed the buffer size
      len = (nRealmLen < sizeof(authReq.aRealm)) ?
                        nRealmLen :
                        sizeof(authReq.aRealm);

      authReq.nRealmLen = len;
      MEMCPY((void*)&authReq.aRealm[0], pRealm, len);
   }

   ret = OEMOBEXSrv_Authenticate(pSsn->pSrvSys->pSrvOem, &authReq, pSsn->connId);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================

static int AEEOBEXSsn_OnEventNotify(IOBEXSrvSession     *me,
                                    ISignal      *piSignal)
{
   int ret = AEE_SUCCESS;

   if (NULL == piSignal)
   {
      return AEE_EBADPARM;
   }

   if ((NULL == me->pSsnSys) || (NULL == me->pSsnSys->piSigBusAppEvt))
   {
      return AEE_EBADSTATE;
   }

   ret = ISignalBus_Add(me->pSsnSys->piSigBusAppEvt, piSignal);

   // we need to do this as there will always be an event available for 
   // the app to read when it first gets a session object
   ISignalBus_Strobe(me->pSsnSys->piSigBusAppEvt);
   MSG_MED("AEE OBEX srv ssn on evt notify me %x, pisignal %x ret %x"
            ,me,piSignal,ret);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_GetEvent(IOBEXSrvSession   *me,
                               AEEOBEXEventType  *pEvent,
                               IOBEXHeader       **ppiHdr,
                               AEEOBEXEvent      *pEventData)
{
   int ret = AEE_ENOMORE;
   AEEOBEXSsnEvt evt;
   AEEOBEXSsn *pSsnSys = NULL;

   pSsnSys = me->pSsnSys;


   if ((NULL == pEvent) || (NULL == ppiHdr) || (NULL == pEventData) ||
       (!AEE_CHECK_PTR("AEEOBEXSsn_GetEvent",
                        pEvent, sizeof(AEEOBEXEventType), TRUE)) ||
       (!AEE_CHECK_PTR("AEEOBEXSsn_GetEvent",
                        ppiHdr, sizeof(IOBEXHeader*), TRUE)) ||
       (!AEE_CHECK_PTR("AEEOBEXSsn_GetEvent",
                        pEventData, sizeof(AEEOBEXEvent), TRUE)))
   {
      return AEE_EBADPARM;
   }

   if (NULL == pSsnSys)
   {
      return AEE_EBADSTATE;
   }
   // pop the first event on the queue.
   ret = AEEOBEXEvtQ_Get(&pSsnSys->appEvtQueue, (uint8*)&evt);

   if (AEE_SUCCESS == ret)
   {
      *pEvent               = evt.event;
      pEventData->packet    = evt.packet;
      pEventData->cmdStatus = evt.status;
      pEventData->flags     = evt.flags;
      *ppiHdr               = NULL;

      if (NULL != evt.pOemHdr)
      {
         // if this object cannot be created, 
         // AEEOBEXHeader_GetObject() releases the 
         // oem object
         AEEOBEXHeader_GetObject(me->pIShell,
                                 (void**)ppiHdr,
                                 evt.pOemHdr,
                                 FALSE); // not writable
      }
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_SendResponse(IOBEXSrvSession     *me,
                                   AEEOBEXRequestType  request,
                                   IOBEXHeader         *piOBEXHdr,
                                   AEEOBEXCmdStatus    cmdStatus,
                                   AEEOBEXFlags        flags)
{
   int ret = AEE_SUCCESS;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }
   if (NULL != piOBEXHdr)
   {
      OEMOBEXHdr *pOemHdr = NULL;
      MSG_MED("AEE OBEX Send cmd header being sent ",0,0,0);

      pOemHdr = AEEOBEXHdr_GetOemHdr(piOBEXHdr);
      // verify that the header belongs to the pool for this 
      // client.
      if ( !AEEOBEXSrv_SendHdrValid(me->pSsnSys, pOemHdr))
      {
         MSG_ERROR("AEE OBEX Send cmd header invalid ",0,0,0);
         return AEE_EBADPARM;
      }
   }
   switch (request)
   {
      case AEEOBEX_REQ_CONNECT:
      {
         ret = AEEOBEXSsn_ConnectResponse(me, piOBEXHdr, cmdStatus);
         break;
      }
      case  AEEOBEX_REQ_PUT:
      {
         ret = AEEOBEXSsn_PutResponse(me, piOBEXHdr, cmdStatus);
         break;
      }
      case AEEOBEX_REQ_GET:
      {
         ret = AEEOBEXSsn_GetResponse(me, piOBEXHdr, cmdStatus);
         break;
      }
      case AEEOBEX_REQ_SETPATH:
      {
         ret = AEEOBEXSsn_SetPathResponse(me, piOBEXHdr, cmdStatus);
         break;
      }
      case AEEOBEX_REQ_DISCONNECT:
      {
         ret = AEEOBEXSsn_DisconnectResponse(me, piOBEXHdr, cmdStatus);
         break;
      }
      default :
      {
         ret = AEE_EUNSUPPORTED;
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_ConnectResponse(IOBEXSrvSession  *me,
                                      IOBEXHeader      *pHdr,
                                      AEEOBEXCmdStatus cmdStatus)
{
   OEMOBEXHdr* pOemHdr = NULL;
   int ret    = AEE_SUCCESS;
   AEEOBEXSsn *pSsnSys = me->pSsnSys;

   if (AEEOBEX_SSN_STATE_CONN_REQ_PEND != pSsnSys->state)
   {
      return AEE_EBADSTATE;
   }

   // verify that there is a buffer available to 
   // receive packets from the remote device.
   if (!AEEOBEXSrv_RecvHdrAvailable(pSsnSys))
   {
      return AEE_ENOMEMORY;
   }

   if (NULL != pHdr)
   {
      // the command packet should not exced 255 bytes
      pOemHdr = AEEOBEXHdr_GetOemHdr(pHdr);

      AEEOBEXHdr_MarkReadOnly(pHdr);

      if (FALSE == AEEOBEX_VerifyPacketSize(0, pOemHdr, TRUE)) // 3rdparam indicates single packet
      {
         return AEE_EBADPARM;
      }
   }
   ret = OEMOBEXSrv_ConnectResp(pSsnSys->pSrvSys->pSrvOem,
                                pSsnSys->connId,
                                pOemHdr,
                                cmdStatus);
   if (AEE_SUCCESS == ret)
   {
      pSsnSys->state = AEEOBEX_SSN_STATE_CONN_STARTED;
   }

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_PutResponse(IOBEXSrvSession *me,
                                  IOBEXHeader *pHdr,
                                  AEEOBEXCmdStatus cmdStatus)
{
   OEMOBEXHdr *pOemHdr = NULL;
   int   ret = AEE_SUCCESS;
   AEEOBEXSsn *pSsnSys = me->pSsnSys;

   pOemHdr = (pHdr != NULL) ?
              AEEOBEXHdr_GetOemHdr(pHdr):
              NULL;

   if (AEEOBEX_SSN_STATE_PUT_REQ_PEND != pSsnSys->state)
   {
      return AEE_EBADSTATE;
   }
   // verify that there is a buffer available to 
   // receive packets from the remote device.
   if (!AEEOBEXSrv_RecvHdrAvailable(pSsnSys))
   {
      return AEE_ENOMEMORY;
   }

   ret = OEMOBEXSrv_PutResp(pSsnSys->pSrvSys->pSrvOem,
                            pSsnSys->connId,
                            pOemHdr,
                            cmdStatus);
   if (AEE_SUCCESS == ret)
   {
      pSsnSys->state = AEEOBEX_SSN_STATE_PUT_STARTED;

      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_GetResponse(IOBEXSrvSession *me,
                                  IOBEXHeader *pHdr,
                                  AEEOBEXCmdStatus cmdStatus)
{
   void* pOemHdr = NULL;
   int   ret = AEE_SUCCESS;
   AEEOBEXSsn *pSsnSys = me->pSsnSys;


   pOemHdr = (pHdr != NULL) ?
             AEEOBEXHdr_GetOemHdr(pHdr) : NULL;

   if (AEEOBEX_SSN_STATE_GET_REQ_PEND != pSsnSys->state)
   {
      return AEE_EBADSTATE;
   }
   // verify that there is a buffer available to 
   // receive packets from the remote device.
   if (!AEEOBEXSrv_RecvHdrAvailable(pSsnSys))
   {
      return AEE_ENOMEMORY;
   }
   ret = OEMOBEXSrv_GetResp(pSsnSys->pSrvSys->pSrvOem,
                            pSsnSys->connId,
                            pOemHdr,
                            cmdStatus);
   if (AEE_SUCCESS == ret)
   {
      pSsnSys->state = AEEOBEX_SSN_STATE_GET_STARTED;
      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_SetPathResponse(IOBEXSrvSession *me,
                                      IOBEXHeader *pHdr,
                                      AEEOBEXCmdStatus cmdStatus)
{
   void* pOemHdr = NULL;
   int   ret = AEE_SUCCESS;
   AEEOBEXSsn *pSsnSys = me->pSsnSys;


   pOemHdr = (pHdr != NULL) ?
             AEEOBEXHdr_GetOemHdr(pHdr) :
             NULL;

   if (AEEOBEX_SSN_STATE_SETPATH_REQ_PEND != pSsnSys->state)
   {
      return AEE_EBADSTATE;
   }
   // verify that there is a buffer available to 
   // receive packets from the remote device.
   if (!AEEOBEXSrv_RecvHdrAvailable(pSsnSys))
   {
      return AEE_ENOMEMORY;
   }
   ret = OEMOBEXSrv_SetPathResp(pSsnSys->pSrvSys->pSrvOem,
                                pSsnSys->connId,
                                pOemHdr,
                                cmdStatus);
   if (AEE_SUCCESS == ret)
   {
      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
      pSsnSys->state = AEEOBEX_SSN_STATE_SETPATH_STARTED;
   }

   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_DisconnectResponse(IOBEXSrvSession  *me,
                                        IOBEXHeader       *pHdr,
                                        AEEOBEXCmdStatus cmdStatus)
{
   void* pOemHdr = NULL;
   int   ret = AEE_SUCCESS;

   pOemHdr = (pHdr != NULL) ?
             AEEOBEXHdr_GetOemHdr(pHdr) :
             NULL;

   if (AEEOBEX_SSN_STATE_DISC_PEND != me->pSsnSys->state)
   {
      return AEE_EBADSTATE;
   }

   ret = OEMOBEXSrv_DiscResp(me->pSsnSys->pSrvSys->pSrvOem,
                             me->pSsnSys->connId,
                             pOemHdr,
                             cmdStatus);
   if (AEE_SUCCESS == ret)
   {
      if (pHdr)
      {
         AEEOBEXHdr_MarkReadOnly(pHdr);
      }
      me->pSsnSys->state = AEEOBEX_SSN_STATE_DISC_STARTED;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_Disconnect(IOBEXSrvSession *me)
{
   int   ret = AEE_SUCCESS;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }

   //if (AEEOBEX_SSN_STATE_CONNECTED != me->pSsnSys->state)
   if ((AEEOBEX_SSN_STATE_CONNECTED > me->pSsnSys->state) ||
       (AEEOBEX_SSN_STATE_DISC_STARTED == me->pSsnSys->state))
   {
      return  AEE_EBADSTATE;
   }
   ret = OEMOBEXSrv_Disconnect(me->pSsnSys->pSrvSys->pSrvOem,
                               me->pSsnSys->connId);
   if (AEE_SUCCESS == ret)
   {
      me->pSsnSys->state = AEEOBEX_SSN_STATE_DISC_STARTED;
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_SetUidPassword(IOBEXSrvSession  *me,
                                     const uint8      *pUid,
                                     int              nUidLen,
                                     const uint8      *pPswd,
                                     int              nPswdLen)
{
   int ret = AEE_SUCCESS;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }
   // verify params
   if ((nUidLen > 0) &&
       ((NULL == pUid)||
       (!AEE_CHECK_PTR("AEEOBEXSrv_SetUidPassword",
                    pUid, nUidLen, TRUE))))
   {
      MSG_ERROR("AEE OBEX srv set uid/pswd  uid %x, len %x ",pUid,nUidLen,0);
      return AEE_EBADPARM;
   }
   if ((nPswdLen > 0) &&
       ((NULL == pPswd)||(!AEE_CHECK_PTR("AEEOBEXSrv_SetUidPassword",
                    pPswd, nPswdLen, TRUE))))
   {
      MSG_ERROR("AEE OBEX srv set uid/pswd  pswd %x ",pPswd,0,0);
      return AEE_EBADPARM;
   }

   if (me->bAuthChal || me->bAuthResp)
   {
      MSG_DEBUG("AEE OBEX calling oem auth response",0,0,0);

      ret = OEMOBEXSrv_AuthResponse(me->pSsnSys->pSrvSys->pSrvOem,
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
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_GetAuthChallenge(IOBEXSrvSession    *me,
                                       boolean            *pbUserIdReq,
                                       boolean            *pbFullAccess,
                                       AEEOBEXCharSet     *pCharSet,
                                       uint8              *pRealm,
                                       int                nRealmLen,
                                       int                *pnRealmLenReq )
{
   int ret = AEE_SUCCESS;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }

   if (FALSE == me->bAuthChal)
   {
      return AEE_ENOSUCH;
   }

   if ((NULL == pbUserIdReq) ||
       (NULL == pbFullAccess)||
       (NULL == pCharSet) ||
       (NULL == pnRealmLenReq))
   {
      return AEE_EBADPARM;
   }

   *pbUserIdReq  = me->authChal.bUserIdReq;
   *pbFullAccess = me->authChal.bFullAccess;
   *pCharSet     = me->authChal.charSet;
 
   *pnRealmLenReq = me->authChal.nRealmLen;

   // the input buffer should not be lesser than the 
   // required length
   if ((nRealmLen < me->authChal.nRealmLen) &&
       (0 != me->authChal.nRealmLen))
   {
      ret = AEE_EBUFFERTOOSMALL;
   }
   else
   {
      if (0 != me->authChal.nRealmLen)
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
            for ( i = 0; i < me->authChal.nRealmLen; i++ )
            {
               pRealm[i] = me->authChal.aRealm[i];
            }
         }
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_GetAuthResponse(IOBEXSrvSession      *me,
                                      AEEOBEXAuthResp      *pAuthResp)
{
   int i = 0;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }

   if ((NULL == pAuthResp) ||
      (!AEE_CHECK_PTR("AEEOBEXCli_GetAuthResponse",
                        pAuthResp, sizeof(AEEOBEXAuthResp), TRUE)))
   {
      return AEE_EBADPARM;
   }

   if (FALSE == me->bAuthResp )
   {
      return AEE_ENOSUCH;
   }

   pAuthResp->nUidLen = me->authResp.nUidLen;
   
   for ( i = 0 ; (i < AEEOBEX_MAX_UID_LEN) && (i < OEMOBEX_MAX_UID_LEN); i++)
   {
      pAuthResp->aUserId[i] = me->authResp.aUserId[i];
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_GetParams(IOBEXSrvSession    *me,
                                AEEOBEXParamType  param,
                                uint32            *pU32,
                                uint8             *pParam,
                                int               nParamLen,
                                int               *pnParamLenReq)
{
   int ret = AEE_EUNSUPPORTED;

   MSG_MED("AEE OBEX cli get param %x",param,0,0);

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADSTATE;
   }

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
            MSG_MED("AEE OBEX srv get bda",0,0,0);
            ret = OEMOBEXSrv_GetBda(me->pSsnSys->pSrvSys->pSrvOem, pParam);
         }
         if (NULL != pnParamLenReq)
         {
            *pnParamLenReq = AEEOBEX_MAX_BDA_LEN;
         }
         break;
      }
      case AEEOBEXPARAM_BT_RFCOMM_SCN:
      {
         MSG_MED("AEE OBEX srv get scn",0,0,0);
         if (NULL != pU32)
         {
            ret = OEMOBEXSrv_GetScn(me->pSsnSys->pSrvSys->pSrvOem, pU32);
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
            *pU32 = me->nMaxPacket;
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
static int AEEOBEXSrv_SsnRelease(AEEOBEXSsn *pSsnSys)
{
   AEEOBEXSsnEvt evt;
   boolean bRelease = FALSE;
   int ret = AEE_SUCCESS;

   if (0 == pSsnSys->uRef)
   {
#ifdef BT_SIMULATION
      pSsnSys->state = AEEOBEX_SSN_STATE_CREATED;
#endif
      // means there was atleast a transport level connection
      if ((AEEOBEX_SSN_STATE_CREATED != pSsnSys->state) &&
         (AEEOBEX_SSN_STATE_DISC_STARTED != pSsnSys->state))// disconnect started already,
      {
         ret = OEMOBEXSrv_Disconnect(pSsnSys->pSrvSys->pSrvOem,
                                  pSsnSys->connId);

         if (AEE_SUCCESS == ret)
         {
            pSsnSys->state = AEEOBEX_SSN_STATE_DISC_STARTED;
         }
         else
         {
            MSG_ERROR("AEE OBEX srv ssn cannot disconnect state %x",pSsnSys->state,0,0);
         }
      }
      else
      {
         if (AEEOBEX_SSN_STATE_CREATED == pSsnSys->state)
         {
            MSG_HIGH("AEE OBEX srv ssn releasing obj state %x",pSsnSys->state,0,0);
            bRelease = TRUE;
         }
      }
   }

   if (bRelease)
   {
      MEMSET((void*)&evt, 0, sizeof(AEEOBEXSsnEvt));
      while (AEE_SUCCESS == AEEOBEXEvtQ_Get(&pSsnSys->appEvtQueue, (uint8*)&evt))
      {
         // free all the events in the queue
      if (evt.pOemHdr != NULL)
      {
         OEMOBEXHdr_Release(evt.pOemHdr);
      }
         MEMSET((void*)&evt, 0, sizeof(AEEOBEXSsnEvt));
   }
   AEEOBEXSrv_SsnInit(pSsnSys);
   AEEOBEXEvtQ_Free(&pSsnSys->appEvtQueue);
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

static int AEEOBEXSrv_SsnInit(AEEOBEXSsn *pSsnSys)
{
   pSsnSys->bConnOriented = FALSE;
   pSsnSys->connId = 0xffff;
   pSsnSys->pSsn = NULL;
   pSsnSys->state = AEEOBEX_SSN_STATE_CREATED;
   pSsnSys->bAvailable = TRUE;

   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

static boolean AEEOBEXSrv_RecvHdrAvailable(AEEOBEXSsn *pSsnSys)
{
   return OEMOBEXSrv_RecvHdrAvailable(pSsnSys->pSrvSys->pSrvOem);
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSsn_CreateHeaderSet(IOBEXSrvSession *me, IOBEXHeader **ppif)
{
   AEEOBEXSrvSys *pSrvSys = NULL; 
   int ret = AEE_ENOMEMORY;
   boolean  bFound = FALSE;
   int i = 0;
   OEMOBEXHdr* pHdr = NULL;

   if (NULL == me->pSsnSys)
   {
      return AEE_EBADPARM;
   }
   pSrvSys = me->pSsnSys->pSrvSys;

   // see if there is free header in the list 
   for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
   {
      if (pSrvSys->aSendHdr[i] != NULL)
      {  // there is an oem header, check if it is free
         bFound = OEMOBEXHdr_IsFree(pSrvSys->aSendHdr[i]);

         if (TRUE == bFound)
         {
            ret = AEE_SUCCESS;
            pHdr = pSrvSys->aSendHdr[i];
            break;
         }
      }
   }

   if (AEE_SUCCESS != ret)
   {
      // no oem hdr in the list we have is free, try to allocate one
      for (i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if (NULL == pSrvSys->aSendHdr[i])
         {  // there is place for an oem header, see if we can allocate one
            ret = OEMOBEXHeader_Init(&pSrvSys->aSendHdr[i], me->nMaxPacket);
            pHdr = pSrvSys->aSendHdr[i];
            break;
         }
      }
   }

   if ((AEE_SUCCESS == ret) && (NULL != pHdr))
   {
      // get a corresponding AEE object for this oem object
      ret = AEEOBEXHeader_GetObject(me->pIShell, (void**)ppif, pHdr,
                            TRUE);
   }
   return ret;
}

//=============================================================================

int AEEOBEXSrv_ParseSecurity(const char* tok, void* data, void* dat2)
{
   OEMOBEX_BtRfcommConfig *pConnInfo = (OEMOBEX_BtRfcommConfig*)data;
   return AEEOBEXTok_GetSecurity(tok, &pConnInfo->security);
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXSrv_ParseUuid(const char* tok, void* data, void* dat2)
{
   OEMOBEX_BtRfcommConfig *pConnInfo = (OEMOBEX_BtRfcommConfig*)data;
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)dat2;
   uint32 uuid = 0;
   int ret = AEE_SUCCESS;

   ret = AEEOBEXTok_GetUuid(tok, &uuid);

   if (AEE_SUCCESS == ret)
   {
      pConnInfo->uuid = (uint16)uuid;
      pSrvSys->uSDReg |= AEEOBEX_SRV_UUID16;
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXSrv_ParseUuid32(const char* tok, void* data, void* dat2)
{
   char *pStr = NULL;
   int ret = AEE_EBADPARM;
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)dat2;

   pStr = STRCHR(tok, '=');

   if (pStr)
   {
      pStr++;
      ret = AEEOBEX_IsDigitStr(pStr,16);
      if (AEE_SUCCESS == ret)
      {
         pSrvSys->uuid32 = (uint32)STRTOUL(pStr,NULL,16); 
         pSrvSys->uSDReg |= AEEOBEX_SRV_UUID32;
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEXSrv_ParseUuid128(const char* pTok, void* data, void* dat2)
{
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)dat2;
   int ret = AEE_EBADPARM;
   char *pStr = NULL;
   int len = 0;

   pStr = STRCHR(pTok, '=');

   if (pStr)
   {
      pStr++;
      // check if all bytes rep valid hex digits, len > 0 and len <= 32 ( 16 byte) 
      ret  = AEEOBEX_IsDigitStr(pStr,32);
      if (AEE_SUCCESS == ret)
      {
         len = STRLEN(pStr);
         MEMSET(pSrvSys->uuid128,0, sizeof(pSrvSys->uuid128));
         MEMCPY(pSrvSys->uuid128, pStr, len);
         pSrvSys->uSDReg |= AEEOBEX_SRV_UUID128; 
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEXSrv_ParseServiceName(const char* tok, void* data, void *dat2)
{
   OEMOBEX_BtRfcommConfig *pConnInfo = (OEMOBEX_BtRfcommConfig*)data;
   char *pName = NULL;
   int ret = AEE_EBADPARM;

   pName = STRCHR(tok, '=');

   if (pName)
   {
      int len = 0;
      // move beyond "="
      pName++;
      len = (STRLEN(pName) > OEMOBEX_MAX_SERVICE_NAME_LEN) ?
             OEMOBEX_MAX_SERVICE_NAME_LEN :
             STRLEN(pName);
      MEMCPY(&pConnInfo->serviceName[0], pName, len);
      pConnInfo->serviceName[len] = 0;
      ret = AEE_SUCCESS;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int  AEEOBEX_ParseBtConfigStr(OEMOBEX_TransportConfigInfo *pConfig,
                                     const char *pszTransport,
                                     AEEOBEXSrvSys *pSrvSys)
{
   AEEOBEXStrToken strParser;
   char *pNextTok = NULL;
   int ret = AEE_SUCCESS;
   int i;
   OEMOBEX_BtRfcommConfig *pBtCon = &pConfig->transportConfig.btConfig;
   AEEOBEXTokTable tokTable[] = {{"bt_security",AEEOBEXSrv_ParseSecurity},
                                 {"uuid16", AEEOBEXSrv_ParseUuid},
                                 {"service_name", AEEOBEXSrv_ParseServiceName},
                                 {"uuid128",AEEOBEXSrv_ParseUuid128},
                                 {"uuid32",AEEOBEXSrv_ParseUuid32}};

   int tableLen = sizeof(tokTable)/sizeof(AEEOBEXTokTable);
   pBtCon->uuid = OEMOBEXSDUUID_INVALID;

   if ((NULL != pszTransport) &&
       ((ret = AEEOBEXStrTok_Init(&strParser, pszTransport, ';')) == AEE_SUCCESS))
   {
      // get the first token
      pNextTok = AEEOBEXStrTok_GetNext(&strParser);
  
      while ((NULL != pNextTok) && (AEE_SUCCESS == ret))
      {
         for (i = 0; i < tableLen ; i++)
         {
            if (STRBEGINS(tokTable[i].tok, pNextTok))
            {
               ret = tokTable[i].pfnParse(pNextTok,(void*)pBtCon,(void*)pSrvSys);
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
   }
   // note: if only 16 bit then send to oem else do registration by ourself
   if ((pSrvSys->uSDReg != 0) && (pSrvSys->uSDReg != AEEOBEX_SRV_UUID16))
   {
      // there is a 128 bit or 32 bit uuid.. so do reg our self
      pSrvSys->uuid16 = pSrvSys->config.transportConfig.btConfig.uuid;
      pSrvSys->config.transportConfig.btConfig.uuid = OEMOBEXSDUUID_INVALID;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_CreateSsn(AEEOBEXSsn *pSsnSys)
{
   int ret = AEE_SUCCESS;

   if (AEE_SUCCESS == ret)
   {
      ret = AEEOBEXEvtQ_Init(&pSsnSys->appEvtQueue,
                             AEEOBEX_MAX_EVENTS,
                             sizeof(AEEOBEXSsnEvt));
      AEEOBEXSrv_SsnInit(pSsnSys);
   }

   // clean up if failed
   if (AEE_SUCCESS != ret)
   {
      AEEOBEXEvtQ_Free(&pSsnSys->appEvtQueue);
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static boolean AEEOBEXSrv_SendHdrValid(AEEOBEXSsn *pSsnSys, OEMOBEXHdr *pOemHdr)
{
   int i = 0;
   boolean bFound = FALSE;

   if ( NULL != pOemHdr)
   {
      for ( i = 0; i < AEEOBEX_MAX_SEND_BUFF; i++)
      {
         if ( pSsnSys->pSrvSys->aSendHdr[i] == pOemHdr)
         {
            bFound = TRUE;
            break;
         }
      }
   }
   return bFound;
}//=============================================================================
// 
//
//=============================================================================
static int AEEOBEX_UpdateSecurity(AEEOBEXSrvSys *pSrvSys, BTSecurityLevel dSec)
{
   int ret = AEE_EFAILED;
   IShell* pIShell = AEE_GetShell();


   if ((NULL == pSrvSys->piBT) && 
           ((ret = ISHELL_CreateInstance (pIShell, AEECLSID_BT, 
                              (void**)&pSrvSys->piBT)) != AEE_SUCCESS))
   {
      MSG_ERROR("AEE OBEX SRV cannot create ibt ret %x", ret,0,0);
   }
   else if ((NULL == pSrvSys->piMgr) &&
               ((ret = IBT_GetObject (pSrvSys->piBT, AEECLSID_BTMgr, NULL, 0,
                              (void**)&pSrvSys->piMgr)) != AEE_SUCCESS))
   {
      MSG_ERROR ("AEE OBEX Srv get ibt mgr failed ret %x", ret, 0,0);
   }
   else if ((NULL == pSrvSys->piSigCtlSecEvt) &&
            ((ret = AEEOBEXSrv_RegBtMgrCallback(pSrvSys)) != AEE_SUCCESS))
   {
      MSG_ERROR ("AEE OBEX Srv reg bt mgr failed ret %x", ret, 0,0);
   }
   else
   {
      BTServiceIdentifier  svc2protect = {
      AEEBT_SEC_BY_SERVER_CHANNEL,
      1,               // number of protocols /services
      {0, 0, 0, 0, 0}, //protocols
      {0,0,0,0,0},
      {0,0,0,0,0}, //scns
      {0,0,0,0,0}}; //psm

      svc2protect.uSvrChnl[0] = pSrvSys->scn;
      if ((ret = IBTMgr_SetSvcSecurity( pSrvSys->piMgr, dSec, 
                                       &svc2protect)) != AEE_SUCCESS)
      {
         MSG_ERROR("AEE OBEX Srv set security failed r=%x", ret,0,0);
      }
   }
   
   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE OBEX Srv set security failed, clean up r=%x", ret,0,0);

      RELEASEIF(pSrvSys->piMgr);
      pSrvSys->piMgr = NULL;

      RELEASEIF(pSrvSys->piBT);
      pSrvSys->piBT = NULL;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int  AEEOBEXSrv_RegBtMgrCallback(AEEOBEXSrvSys *pSrvSys)
{
   ISignalCBFactory *piSignalCbFactory = NULL;
   ISignal          *piSignal          = NULL;
   IShell* pIShell = AEE_GetShell();
   int ret = AEE_SUCCESS;

   // get a signal factory object
   if ((ret = ISHELL_CreateInstance(pIShell, AEECLSID_SignalCBFactory,
                           (void**)&piSignalCbFactory)) == AEE_SUCCESS)
   {
      MSG_DEBUG("AEE OBEX Srv sig cb factory created...",0,0,0);
     
      if ((ret  = ISignalCBFactory_CreateSignal(piSignalCbFactory,
                                    AEEOBEXSrv_BtMgrCallback, // function cb
                                    pSrvSys,                   // data to cb
                                    &piSignal,
                                    &pSrvSys->piSigCtlSecEvt)) == AEE_SUCCESS)
      {
         ret = IBTMgr_OnEventNotify(pSrvSys->piMgr, piSignal);
         
      }
   }
   RELEASEIF(piSignal);
   RELEASEIF(piSignalCbFactory);
   if (AEE_SUCCESS != ret)
   {
      RELEASEIF(pSrvSys->piSigCtlSecEvt);
      pSrvSys->piSigCtlSecEvt = NULL; 
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXSrv_BtMgrCallback(void* dat)
{
   BTMgrEventType  pEventID;
   BTResult result;
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)dat;

   ISignalCtl_Enable(pSrvSys->piSigCtlSecEvt);

   while ( IBTMgr_GetEvent(pSrvSys->piMgr,
                                  &pEventID,
                                  &result) != AEE_ENOMORE)
   {
      if (AEEOBEX_SRV_STATE_REG_PEND == pSrvSys->state)
      {
         if (AEEBTMGR_EV_SET_SVC_SEC_SUCCESS == pEventID)
         {
            if (( 0 != pSrvSys->uSDReg) && (AEEOBEX_SRV_UUID16 != pSrvSys->uSDReg))
            {
               AEEOBEXSrv_RegisterService(pSrvSys);
            }
            else
            {
               // set the state to regisered
               pSrvSys->state = AEEOBEX_SRV_STATE_REGISTERED;
               if (pSrvSys->pSrv)
               {
                  AEEOBEXSrv_SendEvent(pSrvSys, AEEOBEX_EVT_SRV_REG_CMPL,
                                       AEEOBEX_STAT_SUCCESS,NULL);
               }
               
               MSG_MED("AEE OBEX srv reg compl, state %x" ,pSrvSys->state, 0, 0);
            }
         }
         else if (AEEBTMGR_EV_SET_SVC_SEC_FAILED == pEventID)
         {
            // de register
            AEEOBEXSrv_RegCleanUp(pSrvSys);
            if (AEE_SUCCESS != OEMOBEXSrv_DeRegister(pSrvSys->pSrvOem))
            {
               MSG_ERROR("AEE OBEX srv failed to de register server",0,0,0);
               pSrvSys->state = AEEOBEX_SRV_STATE_CREATED;
            }
         }
      }// else ignore the event
   }

   if ((pSrvSys->uRef == 0) ||
      (AEEOBEX_SRV_STATE_DEREG_PEND == pSrvSys->state))
   {
      // note : sysRelease deletes the object so pCliSys should not
      // be referenced after this point
      MSG_HIGH("AEE OBEX srv sys cb.calling sys release...",0,0,0);
      AEEOBEXSrv_SysRelease(pSrvSys);
   }
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_SendEvent(AEEOBEXSrvSys *pSrvSys,
                                AEEOBEXEventType evt,
                                AEEOBEXCmdStatus stat,
                                AEEOBEXSsn  *pSsn)
{
   AEEOBEXSrvEvt event;
   MEMSET((void*)&event, 0, sizeof(event));

   event.event   = (uint8)evt;
   event.status  = stat;
   event.pSsn    = pSsn;

   if (pSrvSys->pSrv)
   {
      AEEOBEXEvtQ_Put(&pSrvSys->pSrv->appEvtQueue, (uint8*)&event);
      ISignalBus_Strobe(pSrvSys->pSrv->piSigBusAppEvt);
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_RegisterService(AEEOBEXSrvSys *pSrvSys)
{
   int ret = AEE_EFAILED;
   IShell* pIShell = AEE_GetShell();


   if ((NULL == pSrvSys->piBT) && 
           ((ret = ISHELL_CreateInstance (pIShell, AEECLSID_BT, 
                              (void**)&pSrvSys->piBT)) != AEE_SUCCESS))
   {
      MSG_ERROR("AEE OBEX SRV cannot create ibt ret %x", ret,0,0);
   }
   else if ((NULL == pSrvSys->piSd) &&
               ((ret = IBT_GetObject(pSrvSys->piBT, AEECLSID_BTServiceDiscovery, NULL, 0,
                              (void**)&pSrvSys->piSd)) != AEE_SUCCESS))
   {
      MSG_ERROR ("AEE OBEX Srv get IBTSD failed ret %x", ret, 0,0);
   }
   else if ((NULL == pSrvSys->piSigCtlSdEvt) &&
            ((ret = AEEOBEXSrv_RegBtSdCallback(pSrvSys)) != AEE_SUCCESS))
   {
      MSG_ERROR ("AEE OBEX Srv reg bt SD failed ret %x", ret, 0,0);
   }
   else
   {
      // note: here we allocate ~500 bytes
      // taking into account max service name len is 80
      // and only 1 128 bit uuid,1 16 bit uuid and 1 32 bit uuid is 
      // supported
      char *pBuf = (char*)MALLOC(500); 

      if (NULL != pBuf)
      {
         AEEOBEXSrv_FormatSdString(pBuf, 500, pSrvSys);
         if ((ret = IBTServiceDiscovery_RegisterService(pSrvSys->piSd, pBuf, 
                                          TRUE)) != AEE_SUCCESS)
         {
            MSG_ERROR("AEE OBEX Srv reg SD failedr=%x", ret,0,0);
         }
         // done with the buffer
         FREEIF(pBuf);
      }
   }
   
   if (AEE_SUCCESS != ret)
   {
      MSG_ERROR("AEE OBEX Srv register with SD failed, clean up r=%x", ret,0,0);

      RELEASEIF(pSrvSys->piSd);
      pSrvSys->piSd = NULL;

      RELEASEIF(pSrvSys->piBT);
      pSrvSys->piBT = NULL;
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
static int  AEEOBEXSrv_RegBtSdCallback(AEEOBEXSrvSys *pSrvSys)
{
   ISignalCBFactory *piSignalCbFactory = NULL;
   ISignal          *piSignal          = NULL;
   IShell* pIShell = AEE_GetShell();
   int ret = AEE_SUCCESS;

   // get a signal factory object
   if ((ret = ISHELL_CreateInstance(pIShell, AEECLSID_SignalCBFactory,
                           (void**)&piSignalCbFactory)) == AEE_SUCCESS)
   {
      MSG_DEBUG("AEE OBEX Srv sig cb factory created for SD...",0,0,0);
     
      if ((ret  = ISignalCBFactory_CreateSignal(piSignalCbFactory,
                                    AEEOBEXSrv_SdCallback, // function cb
                                    pSrvSys,                   // data to cb
                                    &piSignal,
                                    &pSrvSys->piSigCtlSdEvt)) == AEE_SUCCESS)
      {
         ret = IBTServiceDiscovery_OnEventNotify(pSrvSys->piSd, piSignal);
      }
   }
   RELEASEIF(piSignal);
   RELEASEIF(piSignalCbFactory);
   if (AEE_SUCCESS != ret)
   {
      RELEASEIF(pSrvSys->piSigCtlSdEvt);
      pSrvSys->piSigCtlSdEvt = NULL; 
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static void AEEOBEXSrv_SdCallback(void* dat)
{
   BTSDEventType eventID;
   BTResult result;
   AEEOBEXSrvSys *pSrvSys = (AEEOBEXSrvSys*)dat;

   ISignalCtl_Enable(pSrvSys->piSigCtlSdEvt);

   while ((pSrvSys->piSd != NULL)&&
          ( IBTServiceDiscovery_GetEvent(pSrvSys->piSd,
                                  &eventID,
                                  &result) != AEE_ENOMORE))
   {
      if (AEEOBEX_SRV_STATE_REG_PEND == pSrvSys->state)
      {
         if (AEEBTSD_EV_SVC_REG_COMPLETE == eventID)
         {
            int num = 0;
            // set the state to regisered
            IBTServiceDiscovery_GetMyRecordHandles(pSrvSys->piSd,&pSrvSys->sdHandle,1,&num);
            pSrvSys->state = AEEOBEX_SRV_STATE_REGISTERED;
            
            if (pSrvSys->pSrv)
            {
               AEEOBEXSrv_SendEvent(pSrvSys, AEEOBEX_EVT_SRV_REG_CMPL,
                                    AEEOBEX_STAT_SUCCESS,NULL);
            }
            MSG_MED("AEE OBEX srv reg compl, state %x" ,pSrvSys->state, 0, 0);
         }
         else if (AEEBTSD_EV_SVC_REG_FAILED == eventID)
         {
            AEEOBEXSrv_RegCleanUp(pSrvSys);
            if (AEE_SUCCESS != OEMOBEXSrv_DeRegister(pSrvSys->pSrvOem))
            {
               MSG_ERROR("AEE OBEX srv failed to de register server",0,0,0);
               pSrvSys->state = AEEOBEX_SRV_STATE_CREATED;
            }
         }
      }// else ignore the event
   }

   if ((pSrvSys->uRef == 0) ||
      (AEEOBEX_SRV_STATE_DEREG_PEND == pSrvSys->state))
   {
      // note : sysRelease deletes the object so pCliSys should not
      // be referenced after this point
      MSG_HIGH("AEE OBEX srv sys cb.calling sys release...",0,0,0);
      AEEOBEXSrv_SysRelease(pSrvSys);
   }
}

//=============================================================================
// 
//
//=============================================================================

static void AEEOBEXSrv_RegCleanUp(AEEOBEXSrvSys *pSrvSys)
{
   uint32 *pBtSec = &pSrvSys->config.transportConfig.btConfig.security;
   

   if (pSrvSys->piSigCtlSecEvt)
   {
      ISignalCtl_Detach(pSrvSys->piSigCtlSecEvt);
      ISignalCtl_Release(pSrvSys->piSigCtlSecEvt);
      pSrvSys->piSigCtlSecEvt = NULL;
   }
   if (*pBtSec != AEEBT_SEC_NONE)// && 
      //((AEEOBEX_UpdateSecurity(pSrvSys, AEEBT_SEC_NONE)) == AEE_SUCCESS))
   {
      if (pSrvSys->piMgr)
      {
         int ret = 0;
         BTServiceIdentifier  svc2protect = {
         AEEBT_SEC_BY_SERVER_CHANNEL,
         1,               // number of protocols /services
         {0, 0, 0, 0, 0}, //protocols
         {0,0,0,0,0},
         {0,0,0,0,0}, //scns
         {0,0,0,0,0}}; //psm

         svc2protect.uSvrChnl[0] = pSrvSys->scn;
         if ((ret = IBTMgr_SetSvcSecurity( pSrvSys->piMgr, AEEBT_SEC_NONE, 
                                          &svc2protect)) != AEE_SUCCESS)
         {
            MSG_ERROR("AEE OBEX Srv reset security failed r=%x", ret,0,0);
         }
      }
      else
      {
         MSG_ERROR(" AEE OBEX Srv canot reset service security",0,0,0);
      }
      *pBtSec = AEEBT_SEC_NONE;
   }
      
   if (pSrvSys->piSigCtlSdEvt)
   {
      ISignalCtl_Detach(pSrvSys->piSigCtlSdEvt);
      ISignalCtl_Release(pSrvSys->piSigCtlSdEvt);
      pSrvSys->piSigCtlSdEvt = NULL;
   }
   if (pSrvSys->sdHandle)
   {
      if (pSrvSys->piSd)
      {
         IBTServiceDiscovery_DeregisterService(pSrvSys->piSd,pSrvSys->sdHandle);
      }
      pSrvSys->sdHandle = (uint32)NULL;
   }
   RELEASEIF(pSrvSys->piSd);
   pSrvSys->piSd = NULL;

   RELEASEIF(pSrvSys->piMgr);
   pSrvSys->piMgr = NULL;

   return;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXSrv_FormatSdString(char *pBuf, int maxLen, AEEOBEXSrvSys *pSrvSys)
{
   char temp[150];

   MEMSET((void*)pBuf, 0, maxLen);
   SNPRINTF(pBuf,maxLen,"<srec><attr id=\"0x0001\"><list>");

   if (pSrvSys->uSDReg & AEEOBEX_SRV_UUID16)
   {
      MEMSET(temp,0, sizeof(temp));
      SNPRINTF(temp,sizeof(temp),"<uuid16>0x%04x</uuid16>",pSrvSys->uuid16);
      STRLCAT(pBuf,temp,maxLen);
   }
   if (pSrvSys->uSDReg & AEEOBEX_SRV_UUID128)
   {
      MEMSET(temp,0, sizeof(temp));
      SNPRINTF(temp,sizeof(temp),"<uuid128>0x%s</uuid128>",pSrvSys->uuid128);
      STRLCAT(pBuf,temp,maxLen);
   }
   if(pSrvSys->uSDReg & AEEOBEX_SRV_UUID32)
   {
      MEMSET(temp,0, sizeof(temp));
      SNPRINTF(temp,sizeof(temp),"<uuid32>0x%08x</uuid32>",pSrvSys->uuid32);
      STRLCAT(pBuf,temp,maxLen);
   }
   // Protocol desc list, L2CAP and RFComm
   STRLCAT(pBuf,"</list></attr><attr id=\"0x0004\"><list><list><uuid16>0x0100</uuid16></list><list><uuid16>0x0003</uuid16>",maxLen);
   MEMSET(temp,0, sizeof(temp));
   SNPRINTF(temp,sizeof(temp),"<uint8>0x%02x</uint8></list>",   
           pSrvSys->scn); // RFComm scn
   STRLCAT(pBuf,temp,maxLen);
   // Protocol desc list contd. OBEX 
   STRLCAT(pBuf,"<list><uuid16>0x0008</uuid16></list></attr><attr id=\"0x0100\">",maxLen); 

   MEMSET(temp,0, sizeof(temp));
   SNPRINTF(temp,sizeof(temp),"<string>%s</string></attr></srec>",pSrvSys->config.transportConfig.btConfig.serviceName);
   STRLCAT(pBuf,temp,maxLen);
   
   return AEE_SUCCESS ;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXSrv_New(IShell* pIShell, AEECLSID cls,void **ppif)
{
#if !defined(FEATURE_IBT_DYNAMIC) || !defined(FEATURE_BT_EXTPF_GOEP)
   return ECLASSNOTSUPPORT;
#else
   IOBEXSrv *me = NULL;
   int ret      = AEE_SUCCESS;

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_New -> ",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE OBEX srv bad param ppif %x ",ppif,0,0);
      return AEE_EBADPARM;
   }
   // allocate AEE object
   me = MALLOCREC(IOBEXSrv);

   if (NULL == me)
   {
      MSG_ERROR("AEE OBEX srv cannot allocate memory",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(IOBEXSrv));
   // initialize the object
   me->pvt             = &g_Vtbl_OBEXSrv;
   me->pIShell         = pIShell;
   me->uRef            = 1;
   me->suppTransport   = 0;
   me->pSrvSys         = NULL;

   ret = ISHELL_CreateInstance(pIShell, AEECLSID_SignalBus,
                               (void**) &me->piSigBusAppEvt);

   if (AEE_SUCCESS != ret)
   {
      FREEIF(me);
      MSG_ERROR("AEE OBEX srv cannot create sig bus",0,0,0);
      return AEE_EFAILED;
   }

   // fetch the supported transports from the OEM layer
   OEMOBEX_GetSupTransport(&me->suppTransport);

   if (0 == me->suppTransport)
   {
      // no transports supported?
      MSG_ERROR("AEE OBEX srv no transport supported",0,0,0);
      FREEIF(me);
      return AEE_EFAILED;
   }

   ret = AEEOBEXEvtQ_Init(&me->appEvtQueue,
                          AEEOBEX_MAX_EVENTS,
                          sizeof(AEEOBEXSrvEvt));
   if (AEE_SUCCESS != ret)
   {
      // cannot allocate event queue
      MSG_ERROR("AEE OBEX srv cannot alloc evt queue",0,0,0);
      FREEIF(me);
      return AEE_EFAILED;
   }

   // allocate and initialize any system memory including OEM layer
   ret = AEEOBEXSrv_SysInit(me);
   
   if (AEE_SUCCESS != ret)
   {
      // couldnt allocate memory 
      MSG_ERROR("AEE OBEX srv sys obj creation failed...",0,0,0);
      FREEIF(me);
      return AEE_ENOMEMORY;
   }
   
   // Initialize the cb that will be called in case the app closes
   CALLBACK_Init(&me->cbAppClose, AEEOBEXSrv_Delete, me);
   AEE_LinkSysObject(&me->cbAppClose);

   ISHELL_AddRef(pIShell);
   // return the interface
   *ppif = (void *)me;

   MSG_DEBUG("AEE OBEX AEEOBEXSrv_New <-",0,0,0);

   return AEE_SUCCESS;
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */
}


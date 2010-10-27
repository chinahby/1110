/*===========================================================================

FILE:      OEMOBEX.c

SERVICES:  OBEX BREW extension

GENERAL DESCRIPTION: BREW OEM interface to the MT-OBEX profile.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


        Copyright ?2004-2008  QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMOBEX.c#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------  
10/07/08   GB      Replaced the depricated string API with the safer version.
09/15/08   PR      Fix compiler warnings
03/26/07   MH     Fixed OEM OBEX cleanup to free OBEX handle 
03/21/07   MH     Added realm string type.
03/12/07   MH     Removing unused variables to remove compiler warnings
02/16/07   MH     Added support for disconnect functionality.
02/13/07   MH     Set path indication to support optional headers  
02/06/07   MH     Added support for additional OBEX response codes
1/29/07    MH     Added support for 128Bit UUID and service name registration 
01/03/07   MH     Added functions to Free notification and model events header memroy. 
12/05/06   MH     Deregistration return error when connection is active. 
11/16/06   MH     Modified auth response user id length calculation  
11/14/06   MH     Provided a check to control null header pointer access. 
10/26/06   MH     Modified confirm set path to support all headers
9/25/06    MH     New response codes and sending client BD address with server
                  notification event added.
9/19/06    MH     New Server Authentication notification event is added.
8/25/06    bh     compile errors fixed
7/28/06    bh     modified for obex client authentication

6/23/06    jh     Modified definition of IOBEXClient_Disconnect, SetPath,
                  IOBEXServer_AcceptConnect and Register to support JSR82.  
                  Added call to bt_cmd_pf_goep_free_header_list() in 
                  OEMOBEX_CopyToAppHeaders() to free header list allocated in
                  the btpf layer.
2/24/05    sivap  Do not free headers received from btpf. they will now be freed
                  by the btpf layer.
2/7/05     sivap  Fixed some cases where we may receive NULL pointer header from
                  btpf. So handle this correctly.  Allow GetResponse to send
                  NULL header.
                  Ensure that the model events are queued and the model callback
                  is invoked correctly in the app context.
1/24/05    sivap  fixed issues with Initialize(). Also make sure to pass in
                  serverConnId for ForceDisconnect(). Also added IOBEXClient
                  support
11/13/04   sivap  initial checkin. unit tested for IOBEXServer API

=============================================================================*/



#include "OEMFeatures.h"
#ifdef FEATURE_IOBEX

  #include "bt.h"
  #include "assert.h"

  #include "BREWVersion.h"
  #include "AEEOBEX.h"
  #include "AEEOBEX_priv.h"
  #include "OEMOBEX.h"
  #include "AEEOBEXNotifier.h"
  #include "OEMOBEXNotifier.h"
  #include "AEEModel.h"
  #include "AEEStdLib.h"
  #include "OEMHeap.h"

  #include "btpf.h"
  #include "btpfcmdi.h"

  #include "mt_obex.h"
  #include "bt.h"

//==========================================================================
//   Macro definitions
//==========================================================================


//==========================================================================
//   Type definitions
//==========================================================================
enum _AEEOBEXState
{
  AEE_OBEX_STATE_INIT,
  AEE_OBEX_STATE_REGISTERED,
  AEE_OBEX_STATE_CONNECTED,
  AEE_OBEX_STATE_GET_STARTED,
  AEE_OBEX_STATE_PUT_STARTED,
  AEE_OBEX_STATE_SET_PATH_STARTED,
  AEE_OBEX_STATE_GET_REQ_PENDING,
  AEE_OBEX_STATE_PUT_REQ_PENDING,
  AEE_OBEX_STATE_SET_PATH_REQ_PENDING, 
  AEE_OBEX_STATE_DISCONNECT_PENDING
};
typedef uint8 AEEOBEXState;

typedef struct OEMOBEXobj_struct
{
  ACONTEXT*                   pac;
  AEECLSID                    classID; //class ID of app that created us
  IOBEXNotifier              *pNotifier;
  AEEOBEXState                state;
  boolean                     bDoDeregister;
  IOBEX                      *pParent;
  uint32                      uRefs;     //reference count
  uint32                      myHandle; //to be sent in the CON_REQ notification

  mt_obex_app_id_type         appId;
  mt_obex_srv_auth_type       auth;
  boolean                     bReadOnly;
  uint8                       channelNum;
  uint8                       targetId;
  uint16                      serverConnID;
  uint16                      clientConnID;

  //memory allocated by the OEM layer (in system memory) for the duration of an OBEX session
  mt_obex_hdr_list_type       oemHeaderList;
  boolean                     bIsServer;
  struct OEMOBEXobj_struct   *pNextObject;  // linked into mgr

  // The size of array notifications is somewhat arbitrary.There is a chance that
  // the obex callback may be called several times before function
  // AEEOBEX_DeliverModelEvent() gets a chance to do its thing, so
  // than more than OBEX_MAX_PENDING_EVENTS will occur--in this case,
  // some events will be lost, and the obex callback will save the number of
  // overrun events in eventsOverrun.

  // event buffer
  AEEOBEXModelNotificationDataType notifications[OBEX_MAX_PENDING_EVENTS];
  uint32                           numEvents;     // number of queued events
  uint32                           eventsOverrun; // number of lost events

   // We initialize this structure in the constructor with a pointer to
   // function OEMOBEX_DeliverQueuedUpModelEvents() and a pointer to this
   // interface.  Function OEMOBEX_EventCallback() calls ISHELL_Resume() with
   // &cbDeliverModelEvents as a parameter.

   AEECallback cbDeliverModelEvents;

} OEMOBEXobj_t;


// OBEX object manager
typedef struct
{
  OEMOBEXobj_t* pHeadObject;    // singly linked list of OBEX objects
} OEMOBEX_Mgr;



//==========================================================================
//   Function prototypes
//==========================================================================

// static helper functions
static void OEMOBEX_Cleanup( OEMOBEXobj_t* pMe );
static boolean OEMOBEX_CopyHeaders(OEMOBEXobj_t *pMe, AEEOBEXHdrType *hdrPtr, uint8 count);
static void OEMOBEX_CopyToAppHeaders(bt_app_id_type app_id,
                                     uint8 *appHdrCntPtr,
                                     AEEOBEXHdrType **appHdrPtrPtr,
                                     uint8 hdrCnt,
                                     mt_obex_hdr_type *hdrPtr);

static void OEMOBEXDisconect_CopyToAppHeaders(bt_app_id_type app_id,
                                     uint8 *appHdrCntPtr,
                                     AEEOBEXHdrType **appHdrPtrPtr,
                                     uint8 hdrCnt,
                                     mt_obex_hdr_type *hdrPtr);
static void OEMOBEX_FreeAppHeaders(AEEOBEXModelNotificationDataType *appHdrPtr);
static void OEMOBEX_FreeHeaders(mt_obex_hdr_list_type *pHeaderList);
static mt_obex_status_type OEMOBEX_MapCmdStatus(AEEOBEXCmdStatusType status);
static AEEOBEXStatusType OEMOBEX_MapEvtStatus(bt_cmd_status_type status);

static boolean OEMOBEX_HandleCmdDone( bt_ev_gn_cmd_done_type *pCmdDn,
                                      AEEOBEXModelNotificationDataType *pN,
                                      OEMOBEXobj_t *pMe);

static OEMOBEXobj_t* OEMOBEX_FindMe(mt_obex_app_id_type appId);

static int  OEMOBEX_CheckCmdStatus( mt_obex_status_type stat );

static void OEMOBEX_QueueUpModelEvent( OEMOBEXobj_t *pMe,
                                       AEEOBEXModelNotificationDataType notif);
static void OEMOBEX_DeliverQueuedUpModelEvents(OEMOBEXobj_t *pMe);
STATIC void OEMOBEX_EventCallback(bt_ev_msg_type *ev_msg_ptr);


//==========================================================================
//   Static data
//==========================================================================

static OEMOBEX_Mgr        gMgr;               // manager object

//==========================================================================
//   Public Funtions
//==========================================================================


//Init will always AddRef
int OEMOBEX_Initialize( IOBEX* pParent )
{
  OEMOBEXobj_t *pMe;

  if (pParent->m_hOBEX != 0)
  {
    if (AEEHandle_From( &gOEMOBEXHandleList, pParent->m_hOBEX,
                        (OEMINSTANCE*)&pMe ) != TRUE)
    {
      return EFAILED;
    }
    pMe->pParent = pParent; //set the new parent.
    //update the app context
    pMe->pac = AEE_GetAppContext();
    return SUCCESS;
  }

  //if m_hOBEX is 0, create a new OEMOBEXobj_t

  //allocate pMe
  pMe = sys_malloc(sizeof(OEMOBEXobj_t));
  if (pMe == NULL)
  {
    return ENOMEMORY;
  }
  //since sys_malloc does not zero out the memory, lets do it ourselves
  MEMSET(pMe, 0, sizeof(pMe));
  //get a handle to the Notifier
  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_OBEX_NOTIFIER,
                              (void**) &pMe->pNotifier ) != SUCCESS )
  {
    sys_free( pMe );
    return EFAILED;
  }



  pMe->pac = AEE_GetAppContext();

  if ( AEEHandle_To( &gOEMOBEXHandleList,
                     (OEMINSTANCE*)pMe,
                     &pMe->myHandle ) == FALSE )
  {
    pParent->m_hOBEX = 0;
    OEMOBEX_Cleanup( pMe );
    return EFAILED;
  }
  pMe->pParent = pParent;
  pParent->m_hOBEX = pMe->myHandle;
  // init me
  pMe->clientConnID = AEEOBEX_NO_CONN_ID;
  pMe->serverConnID = AEEOBEX_NO_CONN_ID;
  pMe->state        = AEE_OBEX_STATE_INIT;
  pMe->uRefs        = 0; //we will addref later
  TASKLOCK();
  pMe->pNextObject = gMgr.pHeadObject;
  gMgr.pHeadObject = pMe;
  TASKFREE();

  CALLBACK_Init(&(pMe->cbDeliverModelEvents),
                OEMOBEX_DeliverQueuedUpModelEvents, // function pointer
                pMe); // data

  OEMOBEX_AddRef(pParent);
  return SUCCESS;
}


uint32 OEMOBEX_AddRef(IOBEX *pParent)
{
  OEMOBEXobj_t *pMe;
  if (pParent != NULL)
  {
    if (AEEHandle_From(&gOEMOBEXHandleList, pParent->m_hOBEX,
                       (OEMINSTANCE*)&pMe ) != FALSE)
    {
      pMe->uRefs++;
      return pMe->uRefs;
    }
  }
  return 0;
}
//IBase (see AEE.h)
uint32 OEMOBEX_Release(IOBEX *pParent)
{
  OEMOBEXobj_t *pMe;

  if (pParent->m_hOBEX == 0)
  {
    //nothing to do here. no corresponding OEM object exists.
    //this may happen if an app created an instance of IOBEXServer and did not initialize it before
    //calling the release function
    return SUCCESS;
  }
  if (AEEHandle_From(&gOEMOBEXHandleList, pParent->m_hOBEX,
                     (OEMINSTANCE*)&pMe ) == TRUE)
  {
    pMe->uRefs--;
  }
  else
     return EFAILED;  //unable to get handle via AEEHandle_From
  if (pMe->uRefs == 0) //time to remove this object from manager and delete it
  {
    //cancel the callback that may have been set
    if(pMe->cbDeliverModelEvents.pfnCancel)
      pMe->cbDeliverModelEvents.pfnCancel(&pMe->cbDeliverModelEvents);

    //TODO - Should we Fail the Release function if the OEM OBEX
    //object state is  >=AEE_OBEX_STATE_REGISTERED
    TASKLOCK();
    {

      OEMOBEXobj_t **ppT;
      //unlink from LL mgr
      for (ppT = &gMgr.pHeadObject; *ppT != NULL; ppT = &(*ppT)->pNextObject)
      {
        if ((*ppT)->appId == pMe->appId) //look for obex object matching the appId
        {
          *ppT = pMe->pNextObject;
          pMe->pNextObject = NULL; //remove the link into the LL mgr
          break;
        }
      }
    }
    TASKFREE();
    pMe->pac = NULL;
    OEMOBEX_Cleanup( pMe );
    AEEHandle_Clear( &gOEMOBEXHandleList, pParent->m_hOBEX );
    pParent->m_hOBEX = 0;
  }
  pMe->pParent = NULL;
  return SUCCESS;

}

//IOBEX_ServerRegister (see AEEOBEX.h)
int OEMOBEX_ServerRegister(IOBEX *po, const AEEOBEXRegistrationInfoType *pReg,
                           uint8 *pSrvChNum)
{
  //register with MT-OBEX/GOEP

  mt_obex_status_type status;
  OEMOBEXobj_t *pMe;

  mt_obex_tp_config_struct_type tpCParams;
  mt_obex_byte_seq_type target;
  int retStat;

  if (po->m_hOBEX == 0) //if we havent already created the OEMOBEXobj_t, then create it first
  {
    //need to create our object
    if (OEMOBEX_Initialize(po) != SUCCESS) //this automatically AddRefs the OEM Object
    {
      return EFAILED;
    }
  }
  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }


  if (pReg == NULL)
  {
    return EBADPARM;
  }

  // Get the app id from BT driver (we need to do this even for non BT transports)
  pMe->appId =  bt_cmd_ec_get_app_id_and_register(OEMOBEX_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    OEMOBEX_Release( po );
    return EFAILED;
  }

  //convert to the correct auth type
  switch (pReg->auth)
  {
  case AEE_OBEX_AUTH_NONE:
    pMe->auth = MT_OBEX_SRV_AUTH_NONE;
    break;
  case AEE_OBEX_AUTH_PASSWORD:
    pMe->auth = MT_OBEX_SRV_AUTH_PASSWORD;
    break;
  case AEE_OBEX_AUTH_USERID_AND_PASSWORD:
    pMe->auth = MT_OBEX_SRV_AUTH_USERID_AND_PASSWORD;
    break;
  default:
    pMe->auth = MT_OBEX_SRV_AUTH_USERID_AND_PASSWORD;
    break;
  }
  pMe->bReadOnly = pReg->bReadOnly;
  pMe->classID = AEE_GetAppContextCls(pMe->pac);

  //create the mt-obex params that are needed for registration
  tpCParams.transports = pReg->transportConfig.transports;

  if (tpCParams.transports & AEE_OBEX_BLUETOOTH_TRANSPORT)
  {
    if( pReg->transportConfig.bt_config.is_uuid128 == TRUE) 
    {
        /*
        App to set this value to Invalid 16 bit UUID, this is 
        to make sure the same has been rechecked and updated. 
        This value is set to make sure SD registration will not happen 
        with 16bit UUID.
        */
        MSG_HIGH( "OEMOBEX Server register - 128BitUUID",0,0,0);
        tpCParams.bt_config.uuid = (mt_obex_sd_uuid_type)AEEOBEX_INVALID_16BitUUID; 
    }
    else
    {
        /* 16Bit UUID registration is handle by BT SIO open in GOEP layer */
    tpCParams.bt_config.uuid = (mt_obex_sd_uuid_type)pReg->transportConfig.bt_config.uuid;
    }

    switch (pReg->transportConfig.bt_config.security)
    {
    case AEEOBEX_BT_SEC_NONE:
      tpCParams.bt_config.security = MT_OBEX_SEC_NONE;
      break;
    case AEEOBEX_BT_SEC_AUTHENTICATE:
      tpCParams.bt_config.security = MT_OBEX_SEC_AUTHENTICATE;
      break;
    case AEEOBEX_BT_SEC_AUTHENTICATE_AND_ENCRYPT:
      tpCParams.bt_config.security = MT_OBEX_AUTHENTICATE_AND_ENCRYPT;
      break;
    default:
      tpCParams.bt_config.security = MT_OBEX_SEC_AUTHENTICATE;
      break;
    }
  }

  target.len = pReg->targetPtr->len;
  target.data = pReg->targetPtr->data;
  pMe->bIsServer = TRUE;
   /* This is needed to indicate that the APP needs a different event type
    * for AUTH_REQ. The same should not be combined with
    * the connection indication.This is a modification for the Java APPs.
    */        
  tpCParams.bt_config.authRequired = 0;
  tpCParams.bt_config.authRequired = pReg->authRequired;
  if (pMe->pNextObject != NULL) //this is not the first entry. so use secondary target
  {
     pMe->channelNum = pMe->pNextObject->channelNum;  //the channel num is the same
     status = mt_obex_srv_register_secondary_target(pMe->appId, &target,
                                                    pMe->auth, pMe->channelNum,
                                                    &pMe->targetId);
  }
  else
  {
     status = mt_obex_srv_register(pMe->appId, &target, pMe->auth, &tpCParams, &pMe->channelNum);
  }
  retStat = OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    /* Register 128Bit UUID */
    if( pReg->transportConfig.bt_config.is_uuid128 == TRUE) 
    {
        MSG_HIGH("OEMOBEX Server registered - Registering 128BitUUID with SD", 0, 0, 0);
        status = mt_obex_sd_register_custom_service(
                pMe->appId,
                (mt_obex_uuid128_type*) &(pReg->transportConfig.bt_config.uuid128),
                pMe->channelNum,
                (char *)&(pReg->transportConfig.bt_config.serv_name_str[0]));

        if((retStat = OEMOBEX_CheckCmdStatus(status)) != SUCCESS)
        {
            MSG_HIGH("OEMOBEX custom 128bit SD registration fails. Status: %x", status, 0, 0);
            OEMOBEX_Release( po );
            return retStat;
        }
    }
    pMe->state = AEE_OBEX_STATE_REGISTERED;
    //add ref - now OEM object is at refcount == 2. deregister will bring it to 1 and Release
    //will bring it to 0
    OEMOBEX_AddRef(po);

    // return the BT server channel number
    if ((tpCParams.transports & AEE_OBEX_BLUETOOTH_TRANSPORT) && 
        (pSrvChNum != NULL))
    {
      *pSrvChNum = pMe->channelNum;
    }
  }
  else
  {
    //register failed. Lets cleanup
    MSG_ERROR("IOBEXServer registration failed with status: %d", status, 0, 0);
    OEMOBEX_Release( po );
  }
  return retStat;
}

//IOBEX_ServerDeregister (see AEEOBEX.h)
int OEMOBEX_ServerDeregister(IOBEX *po)
{
  mt_obex_status_type status;
  int retStat;
  OEMOBEXobj_t *pMe;
//    mt_obex_status_type mtCmdStatus;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }
  
  switch (pMe->state)
  {
  case AEE_OBEX_STATE_INIT:
    return EBADSTATE;

  case AEE_OBEX_STATE_REGISTERED:
    //remove registration with obex (remove sddb entries etc etc)
    status = mt_obex_srv_deregister_server(pMe->appId, pMe->channelNum);
    retStat = OEMOBEX_CheckCmdStatus(status);
    if ( retStat == SUCCESS)
    {
      if ( pMe->appId != BT_APP_ID_NULL )
      {
        //remove registration with bt driver
        (void)bt_cmd_ec_free_application_id( pMe->appId );
        pMe->appId = BT_APP_ID_NULL;
      }
    }
    //Release (this corresponds to the addref made in the ServerRegister()
    //app will still need to call Release again to cleanup this object.
    //This will happen when app calls IOBEXServer_Release()
    OEMOBEX_Release(po);
    pMe->state = AEE_OBEX_STATE_INIT;
    return retStat;
  default: 
    //App should disconnect server using ForceDisconnect API and call deregister. 
    //Return error.    
    return EBADSTATE;
  }
}

int OEMOBEX_ServerAcceptConnection(IOBEX *po,
                                   boolean bAccept,  
                                   AEEOBEXHdrType *headersPtr,
                                   uint8 headersCnt,
                                   AEEOBEXCmdStatusType cmdStatus )
{
  mt_obex_status_type status;
  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }
  if (pMe->state >= AEE_OBEX_STATE_CONNECTED)
  {
    return EBADSTATE;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;
  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_srv_accept_connect(pMe->appId, pMe->serverConnID,
                                      bAccept, &pMe->oemHeaderList, 
                                      mtCmdStatus);
  return  OEMOBEX_CheckCmdStatus(status);
}

int OEMOBEX_ServerConfirmSetPath(IOBEX *po, AEEOBEXHdrType *headersPtr,
                                 uint8   headersCnt, 
                                 AEEOBEXCmdStatusType cmdStatus)
{
  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;
  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_SET_PATH_REQ_PENDING )
  {
    return EBADSTATE;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
  {
      return EBADPARM;
  }

  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_srv_confirm_set_path(pMe->appId, pMe->serverConnID,
                                        &pMe->oemHeaderList, 
                                        mtCmdStatus);
  retStat = OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_SET_PATH_STARTED;
  }
  return retStat;
}

int OEMOBEX_ServerPutResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                              uint8   headersCnt,
                              AEEOBEXCmdStatusType cmdStatus)
{
  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }
  if (pMe->state != AEE_OBEX_STATE_PUT_REQ_PENDING )
  {
    return EBADSTATE;
  }

  if (headersPtr == NULL && headersCnt != 0)
  {
    return EBADPARM;
  }


  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;
  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_srv_put_response(pMe->appId, pMe->serverConnID, &pMe->oemHeaderList, mtCmdStatus);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_PUT_STARTED;
  }
  return retStat;

}

int  OEMOBEX_ServerGetResponse(IOBEX *po, AEEOBEXHdrType    *headersPtr,
                               uint8   headersCnt,
                               AEEOBEXCmdStatusType cmdStatus)
{
  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_GET_REQ_PENDING )
  {
    return EBADSTATE;
  }

  if ((headersCnt != 0) && (headersPtr == NULL))
  {
    return EBADPARM;
  }
  //make a copy of the header into oemHeaderList
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;

  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_srv_get_response(pMe->appId, pMe->serverConnID, &pMe->oemHeaderList, mtCmdStatus);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_GET_STARTED;
  }
  return retStat;

}

int OEMOBEX_ServerDisconnectResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                                     uint8   headersCnt,
                                     AEEOBEXCmdStatusType cmdStatus)
{
  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;

  MSG_HIGH("IOBEX : Server Disconnect Response HdrCnt=%x cmdStatus=%x",
                                            headersCnt,cmdStatus,0);

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_DISCONNECT_PENDING )
  {
    return EBADSTATE;
  }

  if (headersPtr == NULL && headersCnt != 0)
  {
    return EBADPARM;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;
  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_srv_disconnect_response(pMe->appId, pMe->serverConnID, &pMe->oemHeaderList, mtCmdStatus);

  retStat =  OEMOBEX_CheckCmdStatus(status);

  if (retStat != SUCCESS)
  {
      MSG_HIGH("IOBEX : Disconnect response failed retStatus=%x",retStat, 0,0);                                               
  }

  pMe->state = AEE_OBEX_STATE_REGISTERED;
  pMe->serverConnID = AEEOBEX_NO_CONN_ID;

  return retStat;
}

int OEMOBEX_ServerForceDisconnect(IOBEX *po)
{

  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
//    mt_obex_status_type mtCmdStatus;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }
  if (pMe->state < AEE_OBEX_STATE_CONNECTED ) //we may disconnect even if a request is pending
  {
    return EBADSTATE;
  }

  status = mt_obex_srv_force_disconnect(pMe->appId, pMe->serverConnID);

  //free oemHeaderList if its not empty
  //
  if (pMe->oemHeaderList.list_ptr != NULL)
  {

    OEMOBEX_FreeHeaders(&pMe->oemHeaderList);
  }


  return  OEMOBEX_CheckCmdStatus(status);
}



int OEMOBEX_ClientConnect(IOBEX *po, const AEEOBEXDeviceAddressType *pAddr,
                          AEEOBEXHdrType  *headersPtr,
                          uint8            headersCnt)
{
  mt_obex_status_type status;
  int retStat;

  OEMOBEXobj_t *pMe;
  mt_obex_tp_conn_struct_type connStruct;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state >= AEE_OBEX_STATE_CONNECTED )
  {
    return EBADSTATE;
  }

  if (headersPtr == NULL)
  {
    return EBADPARM;
  }

  switch (pAddr->transport)
  {
  case AEE_OBEX_BLUETOOTH_TRANSPORT:
    connStruct.transport = MT_OBEX_BLUETOOTH_TRANSPORT;
    connStruct.transport_conn.bt_conn.bd_addr_ptr = (bt_bd_addr_type*)pAddr->transport_conn.bt_conn.bd_addr_ptr;
    connStruct.transport_conn.bt_conn.channel = pAddr->transport_conn.bt_conn.channel;
    connStruct.transport_conn.bt_conn.uuid = (mt_obex_sd_uuid_type)pAddr->transport_conn.bt_conn.uuid;
    connStruct.transport_conn.bt_conn.security = (mt_obex_security_type)pAddr->transport_conn.bt_conn.security; //the enums values are identical
    break;
  case AEE_OBEX_IRDA_TRANSPORT:
    connStruct.transport = MT_OBEX_IRDA_TRANSPORT;
    connStruct.transport_conn.irda_conn.irda_device_addr = pAddr->transport_conn.irda_conn.irda_device_addr;
    connStruct.transport_conn.irda_conn.ias_query = pAddr->transport_conn.irda_conn.ias_query;
    connStruct.transport_conn.irda_conn.ias_query_len = pAddr->transport_conn.irda_conn.ias_query_len;
    break;
  case AEE_OBEX_USB_TRANSPORT:
    connStruct.transport = MT_OBEX_USB_TRANSPORT;
    break;
  case AEE_OBEX_RS232_TRANSPORT:
    connStruct.transport = MT_OBEX_RS232_TRANSPORT;
    break;
  default:
    break;
  }

  pMe->clientConnID = AEEOBEX_NO_CONN_ID;
  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;

  // Get the app id from BT driver (we need to do this even for non BT transports)
  pMe->appId =  bt_cmd_ec_get_app_id_and_register(OEMOBEX_EventCallback);
  if ( pMe->appId == BT_APP_ID_NULL )
  {
    return EFAILED;
  }

  status = mt_obex_cli_connect(&connStruct, pMe->appId, &pMe->oemHeaderList);
  //state will be set to CONNECTED when the cmd finishes
  retStat =  OEMOBEX_CheckCmdStatus(status);

  return retStat;
}

int OEMOBEX_ClientDisconnect(IOBEX *po,
                             AEEOBEXHdrType  *headersPtr,
                             uint8            headersCnt)
{
  OEMOBEXobj_t *pMe;
  mt_obex_status_type status;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if ((headersCnt != 0) && (headersPtr == NULL))
  {
    return EBADPARM;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;

  status = mt_obex_cli_disconnect(pMe->appId, pMe->clientConnID, 
                                  &pMe->oemHeaderList);
  retStat =  OEMOBEX_CheckCmdStatus(status);

  return retStat;
}

int OEMOBEX_ClientPut(IOBEX *po, AEEOBEXHdrType  *headersPtr,
                      uint8  headersCnt, AEEOBEXCmdStatusType cmdStatus)
{

  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_CONNECTED )
  {
    return EBADSTATE;
  }

  if (headersPtr == NULL)
  {
    return EBADPARM;
  }

  if ((headersCnt != 0) && (headersPtr == NULL))
  {
    return EBADPARM;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;
  mtCmdStatus = OEMOBEX_MapCmdStatus(cmdStatus);

  status = mt_obex_cli_put(pMe->appId, pMe->clientConnID, &pMe->oemHeaderList, mtCmdStatus);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_PUT_STARTED;
  }
  return retStat;
}

int OEMOBEX_ClientGet(IOBEX *po, AEEOBEXHdrType  *headersPtr,
                      uint8  headersCnt,
                      boolean final)
{
  mt_obex_status_type status;

  OEMOBEXobj_t *pMe;

  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_CONNECTED )
  {
    return EBADSTATE;
  }

  if (headersPtr == NULL)
  {
    return EBADPARM;
  }

  if ((headersCnt != 0) && (headersPtr == NULL))
  {
    return EBADPARM;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;



  status = mt_obex_cli_get(pMe->appId, pMe->clientConnID, &pMe->oemHeaderList, final);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_GET_STARTED;
  }
  return retStat;
}

int OEMOBEX_ClientSetPath(  IOBEX *po,  const AECHAR *pwPathName,
                            boolean upLevel, boolean dontCreate,
                            AEEOBEXHdrType  *headersPtr,
                            uint8            headersCnt)
{
  mt_obex_status_type status;
  OEMOBEXobj_t *pMe;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state != AEE_OBEX_STATE_CONNECTED )
  {
    return EBADSTATE;
  }

  if ((headersCnt != 0) && (headersPtr == NULL))
  {
    return EBADPARM;
  }

  //make a copy of the header into OEM memory
  if ( ! OEMOBEX_CopyHeaders(pMe, headersPtr, headersCnt) )
    return EBADPARM;

  status = mt_obex_cli_set_path(pMe->appId, pMe->clientConnID, 
                                (uint16 *)pwPathName, upLevel, dontCreate,
                                &pMe->oemHeaderList);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  if (retStat == SUCCESS)
  {
    pMe->state = AEE_OBEX_STATE_SET_PATH_STARTED;
  }
  return retStat;
}

int OEMOBEX_ClientAbortOperation(IOBEX *po)
{
  mt_obex_status_type status;
  OEMOBEXobj_t *pMe;
  int retStat;

  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  if (pMe->state < AEE_OBEX_STATE_CONNECTED) //cant abort if we are not even connected
  {
    return EBADSTATE;
  }
  pMe->state = AEE_OBEX_STATE_CONNECTED; //reset state
  status = mt_obex_cli_abort_operation(pMe->appId, pMe->clientConnID);
  retStat =  OEMOBEX_CheckCmdStatus(status);
  return retStat;
}

int OEMOBEX_AuthResponse(IOBEX *po , const char *pwUserID, const char *pwPassword)
{
//    bt_cmd_status_type status;

  OEMOBEXobj_t *pMe;
  mt_obex_status_type mtCmdStatus;
  int retStat;
  uint8 userIdLen = 0; 


  if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                      (OEMINSTANCE*)&pMe ) != TRUE)
  {
    return EFAILED;
  }

  //auth can theoretically happen before accepting a connection or even for each get/put operation
  if (pMe->bIsServer)
  {
    mtCmdStatus = mt_obex_srv_auth_response(pMe->appId, pMe->serverConnID, pwPassword, pMe->bReadOnly);
  }
  else //client
  {
    if(pwUserID)
    {
        userIdLen = STRLEN(pwUserID) + 1 ;
    }
    mtCmdStatus = mt_obex_cli_auth_response(pMe->appId, pMe->clientConnID, (byte*)pwUserID, userIdLen, (char *)pwPassword);
  }
  retStat =  OEMOBEX_CheckCmdStatus(mtCmdStatus);

  return retStat;
}

int OEMOBEX_GetSupportedTransports(IOBEX *po)
{
  AEEOBEXTransportConfigType tct = (AEEOBEXTransportConfigType) mt_obex_get_transports_supported();

  return tct;
}


//==========================================================================
//   static helper functions
//==========================================================================
//cleanup memory allocated to hold the OEM headers
//also remove the app id from bt driver
//and release the Notifier object
static void OEMOBEX_Cleanup( OEMOBEXobj_t * pMe )
{
  //free oemHeaderList if its not empty
  //
  if (pMe->oemHeaderList.list_ptr != NULL)
  {
    OEMOBEX_FreeHeaders(&pMe->oemHeaderList);
  }



  if ( pMe->pNotifier != NULL )
  {
    IOBEXNotifier_Release( pMe->pNotifier );
    pMe->pNotifier = NULL;
  }
  sys_free( pMe );
}

static AEEOBEXStatusType OEMOBEX_MapEvtStatus(bt_cmd_status_type status)
{
  switch ( status )
  {
  case BT_CS_GN_SUCCESS:
      return AEE_OBEX_ERR_NONE;
#ifndef FEATURE_BT_EXTPF
  case BT_CS_PF_INVALID_PARAMETERS:
      return AEE_OBEX_ERR_BAD_PARAM;
  case BT_CS_PF_NO_RESOURCES:
  case BT_CS_PF_OBEX_NO_MORE_CONNECTIONS:
      return AEE_OBEX_ERR_RESOURCE;
  case BT_CS_PF_OBEX_METHOD_NOT_ALLOWED:
  case BT_CS_PF_OBEX_ACCESS_DENIED:
      return AEE_OBEX_ERR_NOT_ALLOWED;
  case BT_CS_PF_OBEX_REQUIRED_HEADER_NOT_FOUND:
      return AEE_OBEX_MISSING_HEADER;
  case BT_CS_PF_OBEX_CONNECT_FAILED:
      return AEE_OBEX_CONNECT_FAILED;
  case BT_CS_PF_OBEX_CONNECTION_TIMEOUT:
      return AEE_OBEX_TIMEOUT;
  case BT_CS_PF_OBEX_NO_SUCH_FOLDER:
      return AEE_OBEX_NO_SUCH_FOLDER;
  case BT_CS_PF_OBEX_INCOMPLETE_PACKET:
      return AEE_OBEX_INCOMPLETE_PACKET;
  case BT_CS_PF_OBEX_CONTINUE:
      return AEE_OBEX_ERR_CONTINUE;
  case BT_CS_PF_OBEX_LENGTH_REQUIRED:
      return AEE_OBEX_ERR_LENGTH_REQUIRED;
  case BT_CS_PF_OBEX_DATABASE_LOCKED:
      return AEE_OBEX_DATABASE_LOCKED;
  case BT_CS_PF_OBEX_NOT_MODIFIED:
      return AEE_OBEX_NOT_MODIFIED;
  case BT_CS_PF_OBEX_GONE:
      return AEE_OBEX_GONE;
  case BT_CS_PF_OBEX_REQ_TIMEOUT:
      return AEE_OBEX_REQ_TIMEOUT;
  case BT_CS_PF_OBEX_PRECONDITION_FAILED:
      return AEE_OBEX_PRECONDITION_FAILED;
  case BT_CS_PF_OBEX_SERVICE_UNAVAILABLE :
      return AEE_OBEX_SERVICE_UNAVAILABLE;
  case BT_CS_PF_OBEX_NOT_FOUND:
      return AEE_OBEX_NOT_FOUND;
  case BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE:
      return AEE_OBEX_VALUE_NOT_ACCEPTABLE;
  case BT_CS_PF_OBEX_DATABASE_FULL:
      return AEE_OBEX_DATABASE_FULL;
  case BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR:
      return AEE_OBEX_INTERNAL_SERVER_ERROR;
  case BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE:
      return AEE_OBEX_UNSUPPORTED_MEDIA_TYPE;
  case BT_CS_PF_OBEX_UNAUTHORIZED:
      return AEE_OBEX_UNAUTHORIZED;
  case BT_CS_PF_OBEX_PARTIAL_CONTENT:
      return AEE_OBEX_PARTIAL_CONTENT;
#endif
  default:
    return AEE_OBEX_ERR_FAILED;
  }
}
static mt_obex_status_type OEMOBEX_MapCmdStatus(AEEOBEXCmdStatusType status)
{
  switch (status)
  {

  case AEE_OBEX_STAT_SUCCESS: return MT_OBEX_SUCCESS;
  #ifndef FEATURE_BT_EXTPF
  case AEE_OBEX_STAT_CONTINUE: return MT_OBEX_CONTINUE;
  case AEE_OBEX_STAT_COMMAND_ERROR: return MT_OBEX_COMMAND_ERROR;
  case AEE_OBEX_STAT_OBEX_CONNECTION_TIMEOUT: return MT_OBEX_CONNECTION_TIMEOUT;
  case AEE_OBEX_STAT_OBEX_CONNECT_FAILED: return MT_OBEX_CONNECT_FAILED;
  case AEE_OBEX_STAT_OBEX_DISCONNECT_FAILED: return MT_OBEX_DISCONNECT_FAILED;
  case AEE_OBEX_STAT_ERROR: return MT_OBEX_ERROR ;
  case AEE_OBEX_STAT_INCOMPLETE_PACKET: return MT_OBEX_INCOMPLETE_PACKET ;
  case AEE_OBEX_STAT_LENGTH_REQUIRED: return MT_OBEX_LENGTH_REQUIRED ;
  case AEE_OBEX_STAT_NOT_CONNECTED: return MT_OBEX_NOT_CONNECTED ;
  case AEE_OBEX_STAT_NO_MORE_CONNECTIONS: return MT_OBEX_NO_MORE_CONNECTIONS ;
  case AEE_OBEX_STAT_OPERATION_IN_PROGRESS: return MT_OBEX_OPERATION_IN_PROGRESS ;

  case AEE_OBEX_STAT_PUT_RESPONSE_ERROR: return MT_OBEX_PUT_RESPONSE_ERROR ;
  case AEE_OBEX_STAT_GET_RESPONSE_ERROR: return MT_OBEX_GET_RESPONSE_ERROR ;
  case AEE_OBEX_STAT_ERROR_REQUIRED_HEADER_NOT_FOUND: return MT_OBEX_REQUIRED_HEADER_NOT_FOUND ;
  case AEE_OBEX_STAT_SERVICE_UNAVAILABLE: return MT_OBEX_SERVICE_UNAVAILABLE ;

  case AEE_OBEX_STAT_TOO_MANY_HEADER_BYTES: return MT_OBEX_TOO_MANY_HEADER_BYTES ;
  case AEE_OBEX_STAT_UNKNOWN_COMMAND: return MT_OBEX_UNKNOWN_COMMAND ;
  case AEE_OBEX_STAT_UNSUPPORTED_VERSION: return MT_OBEX_UNSUPPORTED_VERSION ;
  case AEE_OBEX_STAT_CLIENT_ABORTED_COMMAND: return MT_OBEX_CLIENT_ABORTED_COMMAND ;
  case AEE_OBEX_STAT_BAD_PACKET: return MT_OBEX_BAD_PACKET;
  case AEE_OBEX_STAT_BAD_REQUEST: return MT_OBEX_BAD_REQUEST ;
  case AEE_OBEX_STAT_OBJECT_OVERFLOW: return MT_OBEX_OBJECT_OVERFLOW ;
  case AEE_OBEX_STAT_NOT_FOUND: return MT_OBEX_NOT_FOUND;
  case AEE_OBEX_STAT_ACCESS_DENIED: return MT_OBEX_ACCESS_DENIED ;
  case AEE_OBEX_STAT_VALUE_NOT_ACCEPTABLE: return MT_OBEX_VALUE_NOT_ACCEPTABLE ;
  case AEE_OBEX_STAT_PACKET_OVERFLOW: return MT_OBEX_PACKET_OVERFLOW ;
  case AEE_OBEX_STAT_NO_SUCH_FOLDER: return MT_OBEX_NO_SUCH_FOLDER ;
  case AEE_OBEX_STAT_NAME_REQUIRED: return MT_OBEX_NAME_REQUIRED ;
  case AEE_OBEX_STAT_PASSWORD_TOO_LONG: return MT_OBEX_PASSWORD_TOO_LONG ;
  case AEE_OBEX_STAT_PRECONDITION_FAILED: return MT_OBEX_PRECONDITION_FAILED ;
  case AEE_OBEX_STAT_UNAUTHORIZED: return MT_OBEX_UNAUTHORIZED ;
  case AEE_OBEX_STAT_NOT_IMPLEMENTED: return MT_OBEX_NOT_IMPLEMENTED ;
  case AEE_OBEX_STAT_INVALID_AUTH_DIGEST: return MT_OBEX_INVALID_AUTH_DIGEST ;
  case AEE_OBEX_STAT_INVALID_OPERATION: return MT_OBEX_INVALID_OPERATION ;
  case AEE_OBEX_STAT_DATABASE_FULL: return MT_OBEX_DATABASE_FULL ;
  case AEE_OBEX_STAT_DATABASE_LOCKED: return MT_OBEX_DATABASE_LOCKED ;
  case AEE_OBEX_STAT_INTERNAL_SERVER_ERROR: return MT_OBEX_INTERNAL_SERVER_ERROR  ;
  case AEE_OBEX_STAT_UNSUPPORTED_MEDIA_TYPE: return MT_OBEX_UNSUPPORTED_MEDIA_TYPE ;
  case AEE_OBEX_STAT_PARTIAL_CONTENT: return MT_OBEX_PARTIAL_CONTENT ;
  case AEE_OBEX_STAT_METHOD_NOT_ALLOWED: return MT_OBEX_METHOD_NOT_ALLOWED ;
  case AEE_OBEX_STATSRV_INCOMPLETE_GET: return MT_OBEX_OBEXSRV_INCOMPLETE_GET ;
  case AEE_OBEX_STAT_FOLDER_BROWSING_NOT_ALLOWED: return MT_OBEX_FOLDER_BROWSING_NOT_ALLOWED ;
  case AEE_OBEX_STAT_SERVER_FORCED_DISCONNECT: return MT_OBEX_SERVER_FORCED_DISCONNECT ;
  case AEE_OBEX_STAT_CONNECTION_NOT_FOUND: return  MT_OBEX_CONNECTION_NOT_FOUND ;
  case AEE_OBEX_STAT_NOT_MODIFIED: return MT_OBEX_NOT_MODIFIED ;
  case AEE_OBEX_STAT_GONE : return MT_OBEX_GONE;
  case AEE_OBEX_STAT_REQ_TIMEOUT : return MT_OBEX_REQ_TIMEOUT;
  default: return MT_OBEX_COMMAND_ERROR;
   #endif

  }
}


static int  OEMOBEX_CheckCmdStatus( mt_obex_status_type status )
{

  switch (status)
  {
  case MT_OBEX_SUCCESS:
  case MT_OBEX_PENDING:
    return SUCCESS;
  case MT_OBEX_CMD_Q_FULL:
    return ENOMEMORY;
#ifndef FEATURE_BT_EXTPF
  case MT_OBEX_OUT_OF_MEMORY:
    return ENOMEMORY;
  case MT_OBEX_INVALID_PARAMETERS:
    return EBADPARM;
#endif
  default:
    return EFAILED;
  }
}

static boolean OEMOBEX_HandleCmdDone( bt_ev_gn_cmd_done_type  *pCmdDn,
                                      AEEOBEXModelNotificationDataType *pN,
                                      OEMOBEXobj_t   *pMe)
{
  boolean doSendNotif = FALSE;
  MSG_HIGH("IOBEX HandleCmdDone - status=%x, cmd=%x", pCmdDn->cmd_status, pCmdDn->cmd_type, 0);
  switch (pCmdDn->cmd_type )
  {

  case BT_PF_CMD_GOEP_SRV_ACCEPT:
    {
      if ( pCmdDn->cmd_status != BT_CS_GN_SUCCESS )
      {
        pMe->serverConnID = AEEOBEX_NO_CONN_ID;
        pN->base.evCode = AEE_OBEX_EVT_CONNECTED;
        pN->base.dwParam = 1; //failure
        doSendNotif = TRUE; //send out a notification if connection setup fails

      }
      else
      {
        pMe->state = AEE_OBEX_STATE_CONNECTED;
        pN->base.evCode = AEE_OBEX_EVT_CONNECTED;
        pN->base.dwParam = 0; //success
        doSendNotif = TRUE;
      }
      break;
    }
  case BT_PF_CMD_GOEP_SRV_FORCE_DCN:
    {
      pMe->state = AEE_OBEX_STATE_REGISTERED; //connected -> registered
      break;
    }
  case BT_PF_CMD_GOEP_SRV_GET_RSP:
  case BT_PF_CMD_GOEP_SRV_PUT_RSP:
  case BT_PF_CMD_GOEP_SRV_CFM_SET_PATH:
    pMe->state = AEE_OBEX_STATE_CONNECTED; //done with either get/put,setpath
    break;
  default:
    break;
  }

  return doSendNotif;

}


static void OEMOBEX_FreeHeaders(mt_obex_hdr_list_type *pHeaderList)
{
  uint8 count ;
  int i;

  //iterate over the headers and FREE it
  if (pHeaderList == NULL)
  {
    return;
  }

  count = pHeaderList->count;


  for (i = 0; i < count; i++)
  {
    switch (OBEX_HDR_KIND(pHeaderList->list_ptr[i].id))
    {
    case OBEX_HDR_ID_UNICODE:
      sys_free(pHeaderList->list_ptr[i].val.unicode.str);
      break;
    case OBEX_HDR_ID_BYTESEQ:
      sys_free(pHeaderList->list_ptr[i].val.byteseq.data);
      break;
    case OBEX_HDR_ID_UINT8:
    case OBEX_HDR_ID_UINT32:
      break;
    default:
      MSG_ERROR ("Unknown header type seen in OEMOBEX_FreeHeaders function: %d at hdr index: %d\n", pHeaderList->list_ptr[i].id, i, 0);

      break;
    }

  }
  sys_free(pHeaderList->list_ptr);
  pHeaderList->list_ptr = NULL;
  pHeaderList->count = 0;
}

//Copy Hdrs supplied by the app into OEM allocated memory to pass to MT-OBEX
static boolean OEMOBEX_CopyHeaders(OEMOBEXobj_t *pMe, AEEOBEXHdrType *hdrPtr, uint8 count)
{
  int i = 0;
  boolean retVal = TRUE;
  //free oemHeaderList if its not empty
  //
  if (pMe->oemHeaderList.list_ptr != NULL)
  {
    OEMOBEX_FreeHeaders(&pMe->oemHeaderList);
    pMe->oemHeaderList.list_ptr = NULL;
  }

  //now allocate the memory and make the copy
  pMe->oemHeaderList.count = count;
  if ((count == 0) || (hdrPtr == NULL))
  {
    pMe->oemHeaderList.list_ptr = NULL;
    return TRUE;
  }

  pMe->oemHeaderList.list_ptr =
    (mt_obex_hdr_type*) sys_malloc(sizeof(mt_obex_hdr_type)* count);


  for (i = 0; i < count; i++)
  {
    pMe->oemHeaderList.list_ptr[i].id = hdrPtr[i].id;

    MSG_HIGH("OEMOBEX - Receivied headers from App : HDRcnt=%x : HDRId=%x", count, hdrPtr[i].id, 0);

    switch (OBEX_HDR_KIND(hdrPtr[i].id))
    {
    case OBEX_HDR_ID_UNICODE:
      //allocate a unicode block and copy it

      pMe->oemHeaderList.list_ptr[i].val.unicode.len = hdrPtr[i].val.unicode.len;
      pMe->oemHeaderList.list_ptr[i].val.unicode.str = sys_malloc( (hdrPtr[i].val.unicode.len + 1) * sizeof(uint16));
      // WSTRCPY(pMe->oemHeaderList.list_ptr[i].val.unicode.str, hdrPtr[i].val.unicode.str);
      WSTRNCOPYN(pMe->oemHeaderList.list_ptr[i].val.unicode.str, 
                 hdrPtr[i].val.unicode.len + 1, hdrPtr[i].val.unicode.str, hdrPtr[i].val.unicode.len + 1);
      pMe->oemHeaderList.list_ptr[i].val.unicode.str[hdrPtr[i].val.unicode.len] = 0;
      break;
    case OBEX_HDR_ID_BYTESEQ:

      //allocate a byteseq and copy it
      pMe->oemHeaderList.list_ptr[i].val.byteseq.len = hdrPtr[i].val.byteseq.len;
      pMe->oemHeaderList.list_ptr[i].val.byteseq.data = sys_malloc( (hdrPtr[i].val.byteseq.len + 1) * sizeof(byte));
      MEMCPY(pMe->oemHeaderList.list_ptr[i].val.byteseq.data, hdrPtr[i].val.byteseq.data, hdrPtr[i].val.byteseq.len);
      pMe->oemHeaderList.list_ptr[i].val.byteseq.data[hdrPtr[i].val.byteseq.len] = 0;
      break;
    case OBEX_HDR_ID_UINT8:

      //copy the uint8
      pMe->oemHeaderList.list_ptr[i].val.u_int_8 = hdrPtr[i].val.uInt8;;
      break;
    case OBEX_HDR_ID_UINT32:

      //copy the uint32...
      pMe->oemHeaderList.list_ptr[i].val.u_int_32 = hdrPtr[i].val.uInt32;
      break;
    default:
      MSG_ERROR ("Unknown header type seen in OEMOBEX_CopyHeaders function: %d at hdr index: %d\n" , hdrPtr[i].id, i, 0);
      retVal = FALSE;
      break;
    }
  }
  return retVal;
}

//copy the hdrs received from mt-obex into the application headers
//note that this function should always be called in the correct app
//context.
static void OEMOBEX_CopyToAppHeaders(bt_app_id_type app_id,
                                     uint8 *appHdrCntPtr,
                                     AEEOBEXHdrType **appHdrPtrPtr,
                                     uint8 hdrCnt,
                                     mt_obex_hdr_type *hdrPtr)
{
  int i = 0;
  AEEOBEXHdrType *pAppHdr = NULL;

  *appHdrCntPtr = 0;
  *appHdrPtrPtr = NULL;

  //allocate the number of required headers.
  //this memory will be owned by app that rx it via the notification
  if ((hdrPtr != NULL) && (hdrCnt != 0))
  {
    *appHdrCntPtr = hdrCnt;
    pAppHdr = (AEEOBEXHdrType*) MALLOC(sizeof(AEEOBEXHdrType) * (*appHdrCntPtr));
  MSG_HIGH("IOBEX-OEMOBEX-Mem Alloc RX OBEX hdrs : HDRcnt=%x : HDRptr=%x", hdrCnt, pAppHdr, 0);
    if (pAppHdr == NULL)
    {
      MSG_HIGH("Rx OBEX hdr malloc failed!", 0, 0, 0);
      return; //MALLOC failed. so app gets empty headers
    }
  }
  else
  {
    MSG_HIGH("Rx empty OBEX hdr cnt %x ptr %x", hdrCnt, hdrPtr, 0);
    return;
  }

  //lint -e613
  *appHdrPtrPtr = pAppHdr;

  for (i = 0; i < *appHdrCntPtr; i++)
  {
    pAppHdr[i].id = hdrPtr[i].id;
  MSG_HIGH("IOBEX-OEMOBEX-Allocating memory for each header : HDRId=%x", hdrPtr[i].id, 0, 0);
    switch (hdrPtr[i].id)
    {
    case MT_OBEX_HDR_COUNT: /**< number of objects (used by Connect) */
      pAppHdr[i].val.count = hdrPtr[i].val.count;
      break;
    case MT_OBEX_HDR_NAME:  /**< name of the object (often a file name) */
      pAppHdr[i].val.name.len = hdrPtr[i].val.name.len;
      pAppHdr[i].val.name.str = MALLOC((hdrPtr[i].val.name.len + 1) * sizeof(uint16));
      WSTRNCOPYN(pAppHdr[i].val.name.str, pAppHdr[i].val.name.len+1, hdrPtr[i].val.name.str, pAppHdr[i].val.name.len+1);
      break;
    case MT_OBEX_HDR_TYPE:  /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
      pAppHdr[i].val.type.len = hdrPtr[i].val.type.len;
      pAppHdr[i].val.type.data = MALLOC(hdrPtr[i].val.type.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.type.data, hdrPtr[i].val.type.data, hdrPtr[i].val.type.len);
      break;
    case MT_OBEX_HDR_LENGTH: /**< the length of the object in bytes */
      pAppHdr[i].val.length = hdrPtr[i].val.length;
      break;

    case MT_OBEX_HDR_TIME:   /**< date/time stamp (ISO 8601) */
      pAppHdr[i].val.time.len = hdrPtr[i].val.time.len;
      pAppHdr[i].val.time.data = MALLOC(hdrPtr[i].val.time.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.time.data, hdrPtr[i].val.time.data, hdrPtr[i].val.time.len);
      break;
    case MT_OBEX_HDR_DESCRIPTION: /**< text description of the object */
      pAppHdr[i].val.description.len = hdrPtr[i].val.description.len;
      pAppHdr[i].val.description.str = MALLOC( (hdrPtr[i].val.description.len + 1) * sizeof(uint16));
      WSTRNCOPYN(pAppHdr[i].val.description.str, pAppHdr[i].val.description.len + 1, hdrPtr[i].val.description.str, pAppHdr[i].val.description.len + 1);
      break;
    case MT_OBEX_HDR_TARGET:  /**< name of service to which operation is targeted */
      pAppHdr[i].val.target.len = hdrPtr[i].val.target.len;
      pAppHdr[i].val.target.data = MALLOC(hdrPtr[i].val.target.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.target.data, hdrPtr[i].val.target.data, hdrPtr[i].val.target.len);
      break;
    case MT_OBEX_HDR_HTTP:    /**< HTTP 1.x header */
      pAppHdr[i].val.http.len = hdrPtr[i].val.http.len;
      pAppHdr[i].val.http.data = MALLOC(hdrPtr[i].val.http.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.http.data, hdrPtr[i].val.http.data, hdrPtr[i].val.http.len);
      break;
    case MT_OBEX_HDR_BODY:    /**< a chunk of the object body. */
      pAppHdr[i].val.body.len = hdrPtr[i].val.body.len;
      pAppHdr[i].val.body.data = MALLOC(hdrPtr[i].val.body.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.body.data, hdrPtr[i].val.body.data, hdrPtr[i].val.body.len);
      break;
    case MT_OBEX_HDR_END_OF_BODY: /**< the final chunk of the object body */
      pAppHdr[i].val.endOfBody.len = hdrPtr[i].val.end_of_body.len;
      pAppHdr[i].val.endOfBody.data = MALLOC(hdrPtr[i].val.end_of_body.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.endOfBody.data, hdrPtr[i].val.end_of_body.data,
             hdrPtr[i].val.end_of_body.len);
      break;
    case MT_OBEX_HDR_WHO:           /**< identifies the OBEX application; used to tell if talking to a peer */
      pAppHdr[i].val.who.len = hdrPtr[i].val.who.len;
      pAppHdr[i].val.who.data = MALLOC(hdrPtr[i].val.who.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.who.data, hdrPtr[i].val.who.data, hdrPtr[i].val.who.len);
      break;
    case MT_OBEX_HDR_CONNECTION_ID: /**< an identifier used for OBEX connection multiplexing */
      pAppHdr[i].val.connectionId = hdrPtr[i].val.connectionId;
      break;

    case MT_OBEX_HDR_APPLICATION_PARAMS:        /**< extended application request and response information */
      pAppHdr[i].val.applicationParams.len = hdrPtr[i].val.application_params.len;
      pAppHdr[i].val.applicationParams.data =
      MALLOC(hdrPtr[i].val.application_params.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.applicationParams.data, hdrPtr[i].val.application_params.data,
             hdrPtr[i].val.application_params.len);
      break;
    case MT_OBEX_HDR_AUTHENTICATION_CHALLENGE:  /**< authentication digest-challenge */
      pAppHdr[i].val.authChallenge.len = hdrPtr[i].val.auth_challenge.len;
      pAppHdr[i].val.authChallenge.data = MALLOC(hdrPtr[i].val.auth_challenge.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.authChallenge.data, hdrPtr[i].val.auth_challenge.data,
             hdrPtr[i].val.auth_challenge.len);
      break;
    case MT_OBEX_HDR_AUTHENTICATION_RESPONSE:   /**< authentication digest-response */
      pAppHdr[i].val.authResponse.len = hdrPtr[i].val.auth_response.len;
      pAppHdr[i].val.authResponse.data = MALLOC(hdrPtr[i].val.auth_response.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.authResponse.data, hdrPtr[i].val.auth_response.data,
             hdrPtr[i].val.auth_response.len);
      break;
    case MT_OBEX_HDR_OBJECT_CLASS:   /* object class */
      pAppHdr[i].val.objectClass.len = hdrPtr[i].val.object_class.len;
      pAppHdr[i].val.objectClass.data = MALLOC(hdrPtr[i].val.object_class.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.objectClass.data, hdrPtr[i].val.object_class.data, hdrPtr[i].val.object_class.len);
      break;
    default:
      //should never happen
      MSG_ERROR ("Unknown header type seen in OEMOBEX_CopyToAppHeaders function: %d at hdr index: %d\n" , hdrPtr[i].id, i, 0);

      (*appHdrCntPtr)--; //for every unknown header, we decrement the count
      break;
    }
  }
  //lint +e613

  // Free the header list in the profile layer
  bt_cmd_pf_goep_free_headers( app_id, hdrPtr );
}


// In case of disconect memory for each header string is being allocated seperately
//free memory for each header
static void OEMOBEXDisconect_CopyToAppHeaders(bt_app_id_type app_id,
                                     uint8 *appHdrCntPtr,
                                     AEEOBEXHdrType **appHdrPtrPtr,
                                     uint8 hdrCnt,
                                     mt_obex_hdr_type *hdrPtr)
{
  int i = 0;
  AEEOBEXHdrType *pAppHdr = NULL;

  *appHdrCntPtr = 0;
  *appHdrPtrPtr = NULL;

  //allocate the number of required headers.
  //this memory will be owned by app that rx it via the notification
  if ((hdrPtr != NULL) && (hdrCnt != 0))
  {
    *appHdrCntPtr = hdrCnt;
    pAppHdr = (AEEOBEXHdrType*) MALLOC(sizeof(AEEOBEXHdrType) * (*appHdrCntPtr));
  MSG_HIGH("IOBEX-OEMOBEX-Disconnect-Mem Alloc RX OBEX hdrs : HDRcnt=%x : HDRptr=%x", hdrCnt, pAppHdr, 0);
    if (pAppHdr == NULL)
    {
      MSG_HIGH("Rx OBEX hdr malloc failed!", 0, 0, 0);
      return; //MALLOC failed. so app gets empty headers
    }
  }
  else
  {
    MSG_HIGH("Rx empty OBEX hdr cnt %x ptr %x", hdrCnt, hdrPtr, 0);
    return;
  }

  //lint -e613
  *appHdrPtrPtr = pAppHdr;

  for (i = 0; i < *appHdrCntPtr; i++)
  {
    pAppHdr[i].id = hdrPtr[i].id;
  MSG_HIGH("IOBEX-OEMOBEX-DisconnectAllocating memory for each header : HDRId=%x", hdrPtr[i].id, 0, 0);
    switch (hdrPtr[i].id)
    {
    case MT_OBEX_HDR_COUNT: /**< number of objects (used by Connect) */
      pAppHdr[i].val.count = hdrPtr[i].val.count;
      break;
    case MT_OBEX_HDR_NAME:  /**< name of the object (often a file name) */
      pAppHdr[i].val.name.len = hdrPtr[i].val.name.len;
      pAppHdr[i].val.name.str = MALLOC((hdrPtr[i].val.name.len + 1) * sizeof(uint16));
      WSTRNCOPYN(pAppHdr[i].val.name.str, pAppHdr[i].val.name.len+1, hdrPtr[i].val.name.str, pAppHdr[i].val.name.len+1);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.name.str );
      break;
    case MT_OBEX_HDR_TYPE:  /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
      pAppHdr[i].val.type.len = hdrPtr[i].val.type.len;
      pAppHdr[i].val.type.data = MALLOC(hdrPtr[i].val.type.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.type.data, hdrPtr[i].val.type.data, hdrPtr[i].val.type.len);
       bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.type.data );
      break;
    case MT_OBEX_HDR_LENGTH: /**< the length of the object in bytes */
      pAppHdr[i].val.length = hdrPtr[i].val.length;
      break;

    case MT_OBEX_HDR_TIME:   /**< date/time stamp (ISO 8601) */
      pAppHdr[i].val.time.len = hdrPtr[i].val.time.len;
      pAppHdr[i].val.time.data = MALLOC(hdrPtr[i].val.time.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.time.data, hdrPtr[i].val.time.data, hdrPtr[i].val.time.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.time.data );
      break;
    case MT_OBEX_HDR_DESCRIPTION: /**< text description of the object */
      pAppHdr[i].val.description.len = hdrPtr[i].val.description.len;
      pAppHdr[i].val.description.str = MALLOC( (hdrPtr[i].val.description.len + 1) * sizeof(uint16));
      WSTRNCOPYN(pAppHdr[i].val.description.str, pAppHdr[i].val.description.len + 1, hdrPtr[i].val.description.str, pAppHdr[i].val.description.len + 1);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.description.str );
      break;
    case MT_OBEX_HDR_TARGET:  /**< name of service to which operation is targeted */
      pAppHdr[i].val.target.len = hdrPtr[i].val.target.len;
      pAppHdr[i].val.target.data = MALLOC(hdrPtr[i].val.target.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.target.data, hdrPtr[i].val.target.data, hdrPtr[i].val.target.len);
       bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.target.data );
      break;
    case MT_OBEX_HDR_HTTP:    /**< HTTP 1.x header */
      pAppHdr[i].val.http.len = hdrPtr[i].val.http.len;
      pAppHdr[i].val.http.data = MALLOC(hdrPtr[i].val.http.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.http.data, hdrPtr[i].val.http.data, hdrPtr[i].val.http.len);
       bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.http.data );
      break;
    case MT_OBEX_HDR_BODY:    /**< a chunk of the object body. */
      pAppHdr[i].val.body.len = hdrPtr[i].val.body.len;
      pAppHdr[i].val.body.data = MALLOC(hdrPtr[i].val.body.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.body.data, hdrPtr[i].val.body.data, hdrPtr[i].val.body.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.body.data );
      break;
    case MT_OBEX_HDR_END_OF_BODY: /**< the final chunk of the object body */
      pAppHdr[i].val.endOfBody.len = hdrPtr[i].val.end_of_body.len;
      pAppHdr[i].val.endOfBody.data = MALLOC(hdrPtr[i].val.end_of_body.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.endOfBody.data, hdrPtr[i].val.end_of_body.data,
             hdrPtr[i].val.end_of_body.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.end_of_body.data );
      break;
    case MT_OBEX_HDR_WHO:           /**< identifies the OBEX application; used to tell if talking to a peer */
      pAppHdr[i].val.who.len = hdrPtr[i].val.who.len;
      pAppHdr[i].val.who.data = MALLOC(hdrPtr[i].val.who.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.who.data, hdrPtr[i].val.who.data, hdrPtr[i].val.who.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.who.data );
      break;
    case MT_OBEX_HDR_CONNECTION_ID: /**< an identifier used for OBEX connection multiplexing */
      pAppHdr[i].val.connectionId = hdrPtr[i].val.connectionId;
      break;

    case MT_OBEX_HDR_APPLICATION_PARAMS:        /**< extended application request and response information */
      pAppHdr[i].val.applicationParams.len = hdrPtr[i].val.application_params.len;
      pAppHdr[i].val.applicationParams.data =
      MALLOC(hdrPtr[i].val.application_params.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.applicationParams.data, hdrPtr[i].val.application_params.data,
             hdrPtr[i].val.application_params.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.application_params.data );
      break;
    case MT_OBEX_HDR_AUTHENTICATION_CHALLENGE:  /**< authentication digest-challenge */
      pAppHdr[i].val.authChallenge.len = hdrPtr[i].val.auth_challenge.len;
      pAppHdr[i].val.authChallenge.data = MALLOC(hdrPtr[i].val.auth_challenge.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.authChallenge.data, hdrPtr[i].val.auth_challenge.data,
             hdrPtr[i].val.auth_challenge.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.auth_challenge.data );
      break;
    case MT_OBEX_HDR_AUTHENTICATION_RESPONSE:   /**< authentication digest-response */
      pAppHdr[i].val.authResponse.len = hdrPtr[i].val.auth_response.len;
      pAppHdr[i].val.authResponse.data = MALLOC(hdrPtr[i].val.auth_response.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.authResponse.data, hdrPtr[i].val.auth_response.data,
             hdrPtr[i].val.auth_response.len);
      bt_cmd_pf_goep_free_headers( app_id, (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.auth_response.data );
      break;
    case MT_OBEX_HDR_OBJECT_CLASS:   /* object class */
      pAppHdr[i].val.objectClass.len = hdrPtr[i].val.object_class.len;
      pAppHdr[i].val.objectClass.data = MALLOC(hdrPtr[i].val.object_class.len * sizeof(byte));
      MEMCPY(pAppHdr[i].val.objectClass.data, hdrPtr[i].val.object_class.data, hdrPtr[i].val.object_class.len);
      bt_cmd_pf_goep_free_headers( app_id,  (bt_pf_goep_obex_hdr_type*)hdrPtr[i].val.object_class.data);
      break;
    default:
      //should never happen
      MSG_ERROR ("Unknown header type seen in OEMOBEX_CopyToAppHeaders function: %d at hdr index: %d\n" , hdrPtr[i].id, i, 0);

      (*appHdrCntPtr)--; //for every unknown header, we decrement the count
      break;
    }
  }
  //lint +e613

  // Free the header list in the profile layer
  bt_cmd_pf_goep_free_headers( app_id, hdrPtr );
}


//find pMe matching the given appId
static OEMOBEXobj_t* OEMOBEX_FindMe(mt_obex_app_id_type appId)
{
  OEMOBEXobj_t *pMe;
  for (pMe = gMgr.pHeadObject; pMe != NULL; pMe = pMe->pNextObject)
  {
    if (pMe->appId == appId)
    {
      return pMe;
    }
  }
  return NULL;
}

//This function is invoked by the EventCallback and executes in the context
//of the AMSS task that calls the EventCallback function. It adds an entry to
//the notifications array. If the array is already full, it increments
//pMe->eventsOverrun and prints it to debug log so that we atleast
//know that this overrun occurred.
static void OEMOBEX_QueueUpModelEvent( OEMOBEXobj_t *pMe,
                                       AEEOBEXModelNotificationDataType notif)
{
  if (pMe->numEvents < OBEX_MAX_PENDING_EVENTS)
  {
    pMe->notifications[ pMe->numEvents ] = notif;
    pMe->numEvents++;
  }
  else
  {
    pMe->eventsOverrun++;

  }
}

//This function delivers te queued up model notifications via  a call to
//IMODEL_Notify(). This function is invoked in a BREW task context
//and sets the appication context to that of the process that needs to handle
//the notification and then notifies it.
//It leaves the app context before returning from the function
static void OEMOBEX_DeliverQueuedUpModelEvents(OEMOBEXobj_t *pMe)
{
  IModel *piModel;
  ACONTEXT *pc;
  uint32 saved_num_events;
  uint8 count = 0; // Must initialize outside the do-while loop!
  boolean done = FALSE;

  ASSERT(pMe);
  if (pMe == NULL)
  {
     MSG_ERROR("pMe is NULL while in event dequeue callback!", 0, 0, 0);
     return;
  }
  if (pMe->pParent == NULL)
  {
     MSG_ERROR("pMe->pParent is NULL while in event dequeue callback!", 0, 0, 0);
     return;
  }

  //It is possible to have this call back invoked even though numEvents
  //is set to zero when the previous instance of this callback ran to
  //process all events. Just to play it safe, we just return if numEvents
  //is zero.
  if(pMe->numEvents == 0)
    return;

  //set the app context
  pc = AEE_EnterAppContext(pMe->pac);
  AEEOBEX_QueryInterface(pMe->pParent, AEEIID_MODEL, (void**)&piModel);
  // There is a slim chance that me->num_events may get incremented by
  // OEMOBEX_QueueUpModelEvent()--which is invoked by the BT task--
  // while we are delivering events already.  Since we need to clear
  // me->num_events after we deliver all events, we have a race condition
  // where OEMOBEX_QueueUpModelEvent() increments num_events and then
  // we clear them, resulting in a loss of one or more events.  For this
  // reason, we need to make sure that we clear numEvents ONLY when no
  // other events have been enqueued while we were delivering the
  // old ones.  It is for this reason that we execute the for loop below
  // in a do-while loop, which repeats the for loop if there have been
  // new events enqueued into the notifications[] array while the for loop was
  // executing.

  do
  {
    // Note that we do not initialize variable count here.  The value
    // of count must remain untouched from one iteration of the for
    // loop to the next--each time the for loop executes, count will
    // index the first event from the "batch" of events that arrived
    // when the for loop last executed.


    for(saved_num_events = pMe->numEvents;
         count < saved_num_events;
         count++)
    {
      pMe->notifications[count].base.pModel = piModel;
      IMODEL_Notify(piModel, (ModelEvent*)&(pMe->notifications[count]));
      MSG_HIGH("IOBEX : IModelNotify count=%d:piModel=%d,&(pMe->notifications[count]=%d",
                                            count,piModel,&(pMe->notifications[count]));

      //IModel Notify is synchronous call, which inturn calls app callbacks, 
      //App has to use the pointer by this time, should not use after 
      // model event call back is returned.
      // Free header memory.
      OEMOBEX_FreeAppHeaders(&(pMe->notifications[count]));
    } // for

    // The block of code below needs to execute atomically with respect
    // to the act of incrementing numEvents in method
    // OEMOBEX_QueueUpModelEvent().  We terminate the loop only if
    // the number of queued up events has not changed since the last
    // iteration of the for loop.  Note that AFTER we perform the check,
    // zero out me->numEvents and decide to terminate the do-while loop
    // but BEFORE the loop actually exits, OEMOBEX_QueueUpModelEvent()
    // may increment num_events, but that will be OK--the new events
    // will be delivered starting at index zero, and the next time we
    // run this method, the new events will be delivered.

    TASKLOCK();
    done = saved_num_events == pMe->numEvents;
    if (done) {
      pMe->numEvents = 0;
    }
    TASKFREE();

  } while (!done);


  IMODEL_Release(piModel);

  // Check for an overrun.
  {
    // We do not simply clear eventsOverrun because this creates a
    // race condition with the act of incrementing it in method
    // OEMOBEX_QueueUpModelEvent(), in a similar fashion to the race
    // condition between incrementing and clearing numEvents.  Unlike
    // numEvents, however, eventsOverrun is not associated with an
    // array, it is simply a counter of the number of lost events, so
    // instead of locking task preemption and then clearing it, we
    // simply save it away and decrement the saved value from the
    // current value, then use the saved value to determine whether
    // to report an overrun error.

    uint32 saved_events_overrun = pMe->eventsOverrun;
    pMe->eventsOverrun -=  saved_events_overrun;

    if ( saved_events_overrun )
    {
      MSG_ERROR("Severe Error. Lost %d events due to overrun!", saved_events_overrun, 0, 0);
    }
  }

  //back to the app context we started with
  AEE_LeaveAppContext(pc);

}


STATIC void OEMOBEX_EventCallback(bt_ev_msg_type* ev_msg_ptr)
{
  OEMOBEXobj_t *pMe = OEMOBEX_FindMe(ev_msg_ptr->ev_hdr.bt_app_id);
  AEEOBEXNotificationType notification;
  AEEOBEXNotificationType *pNot = &notification;

  mt_obex_ev_srv_get_ind_type getEvt;
  mt_obex_ev_srv_put_ind_type putEvt;
  mt_obex_ev_srv_set_path_ind_type setPathEvt;
  mt_obex_ev_srv_dcn_ind_type srvDcnIndEvt;
  mt_obex_ev_cli_get_rcv_data_type getRcvEvt;
  mt_obex_ev_cli_set_path_cfm_type setPathCfm;
  mt_obex_ev_cli_put_cfm_type putCfm;

  mt_obex_ev_cli_con_cfm_type conCfmEvt;
  mt_obex_ev_cli_dcn_ind_type dcnIndEvt;
  mt_obex_ev_cli_auth_ind_type cliAuthEvt;
  mt_obex_ev_cli_auth_res_ind_type cliAuthResEvt;

  AEEOBEXModelNotificationDataType modelNotif;
  ACONTEXT *pc;
  
  if (pMe == NULL)
  {
    return; //no matching appid found. what happened here?
  }


  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
  case  BT_EV_GN_CMD_DONE:
    {
      if ( ! OEMOBEX_HandleCmdDone( &ev_msg_ptr->ev_msg.ev_gn_cmd_done,
                                    &modelNotif, pMe ) )
      {

        return; // no need to send notification to app
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE

    //server events - this is the only one sent as a notification. all others are sent via IMODEL_Notify
  case MT_OBEX_EV_SRV_CON_IND:
    MSG_HIGH("IOBEX Evt Handler: S con ind: %x", ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.conn_id, 0,0);

    pMe->serverConnID = ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.conn_id;

    pNot->uID = AEE_OBEX_EVT_SRV_CON_REQ;
    pNot->data.obexHandle = pMe->myHandle;
    pNot->data.classID = pMe->classID;
    pNot->data.ConnReq.srvChNum = ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.ch_num;
    pNot->data.ConnReq.targetId = ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.target_id;
    pNot->data.ConnReq.bUnauthorized = ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.unauthorized;
    if (ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.user_id_len > 0)
    {
      STRTOWSTR( (char*)ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.user_id,
                 pNot->data.ConnReq.wUserID, sizeof(pNot->data.ConnReq.wUserID) );
    }
    else
    {
      pNot->data.ConnReq.wUserID[ 0 ] = 0;  // NULL string
    }

    mt_obex_srv_get_client_addr(ev_msg_ptr->ev_hdr.bt_app_id,
                                ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.conn_id,
                                (bt_bd_addr_type* )&pNot->data.bdAddr );

    //Enter the apps context before we allocate memory.
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(pNot->data.ConnReq.headerCount),
                             &(pNot->data.ConnReq.headerPtr),
                             ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.cmd_headers_cnt,
                             ev_msg_ptr->ev_msg.ev_goep_srv_con_ind.cmd_headers_ptr);
    //now leave the app context
    AEE_LeaveAppContext(pc);

    //now queue the event into the notifier
    pNot->uID = (pNot->uID << 16) | (GET_NOTIFIER_MASK(NMASK_OBEX_GOEP));
    IOBEXNotifier_DoNotify(pMe->pNotifier, pNot, NULL);
    return;

   //server events - this is the authenticate indication notification event to server app.
  case MT_OBEX_EV_SRV_AUTH_IND:
      
    MSG_HIGH("IOBEX Evt Handler: S Auth Req ind: %x", ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.conn_id, 0,0);

    pMe->serverConnID = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.conn_id;

    pNot->uID =     AEE_OBEX_EVT_SRV_AUTH_REQ;
    pNot->data.obexHandle = pMe->myHandle;
    pNot->data.classID = pMe->classID;
    pNot->data.AuthReq.srvChNum = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.ch_num;
    pNot->data.AuthReq.targetId = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.target_id;

    pNot->data.AuthReq.bUidReqd = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.user_id_rqd;
    pNot->data.AuthReq.bFullAccess = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.full_access;

    //Copy the Reaalm Info 
    pNot->data.AuthReq.realmInfo.len = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.realm_len;
    pNot->data.AuthReq.realmInfo.realmStrType = ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.realm_str_type;

    if (ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.realm_len > 0)
    {

      STRTOWSTR( (char*)ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.realm_str,
                 pNot->data.AuthReq.realmInfo.realmStr, sizeof(pNot->data.AuthReq.realmInfo.realmStr));
    }
    else
    {
      pNot->data.AuthReq.realmInfo.realmStr[0] = 0;  // NULL string
    }

    mt_obex_srv_get_client_addr(ev_msg_ptr->ev_hdr.bt_app_id,
                                ev_msg_ptr->ev_msg.ev_goep_srv_auth_req_ind.conn_id,
                                (bt_bd_addr_type* )&pNot->data.bdAddr );

    //now queue the event into the notifier
    pNot->uID = (pNot->uID << 16) | (GET_NOTIFIER_MASK(NMASK_OBEX_GOEP));
    IOBEXNotifier_DoNotify(pMe->pNotifier, pNot, NULL);
    return;

  case  MT_OBEX_EV_SRV_DCN_IND:
    if ( pMe->serverConnID != ev_msg_ptr->ev_msg.ev_goep_srv_dcn_ind.conn_id )
    {
      MSG_ERROR( "IOBEX Evt Handler: S dcn ind, wrong conn id=%x", ev_msg_ptr->ev_msg.ev_goep_srv_dcn_ind.conn_id, 0, 0 );
      return;
    }

    MSG_HIGH( "IOBEX Evt Handler: S dcn ind, ServconnId=%x: State=%x",
               ev_msg_ptr->ev_msg.ev_goep_srv_dcn_ind.conn_id, pMe->state, 0 );

    srvDcnIndEvt =  ev_msg_ptr->ev_msg.ev_goep_srv_dcn_ind; 

    if( pMe->state == AEE_OBEX_STATE_REGISTERED)
    {
        if ( pMe->bDoDeregister )
        {
            pMe->bDoDeregister = FALSE;
            if ( OEMOBEX_ServerDeregister(pMe->pParent) != SUCCESS )
            {
                MSG_ERROR("Unable to deregister after Disconnect was rx. Conn Id: %x", pMe->serverConnID, 0, 0);
                modelNotif.base.dwParam = AEE_OBEX_ERR_FAILED; //indicate that something bad happened
            }
        }
        /* Already Disconnect Indication is sent to app, and app would issue disconnect response
        API, return from here. */
        return;
    }
    else
    {
        pMe->state = AEE_OBEX_STATE_DISCONNECT_PENDING;
        modelNotif.base.evCode = AEE_OBEX_EVT_SRV_DCN_IND;
        modelNotif.base.dwParam = AEE_OBEX_ERR_NONE;

        //Enter the apps context before we allocate memory.
        pc = AEE_EnterAppContext( pMe->pac );
        OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                                 &(modelNotif.u.discReq.headerCount),
                                 &(modelNotif.u.discReq.headerPtr),
                                 srvDcnIndEvt.cmd_headers_cnt,
                                 srvDcnIndEvt.cmd_headers_ptr);
        //Now leave the app context
        AEE_LeaveAppContext(pc);
    }
    break;

  case  MT_OBEX_EV_SRV_GET_IND:
    //no need to check if conn_id is correct...assuming GOEP behaves itself
    if (pMe->state < AEE_OBEX_STATE_CONNECTED)
    {
      MSG_ERROR("IOBEX - Not yet connected. Cannot accept GET Request", 0, 0, 0);
      return;
    }

    getEvt = ev_msg_ptr->ev_msg.ev_goep_srv_get_ind;

    /* Use MSG_LOW() here crashes in case of pulling out USB cable while copying file to PC. Use MSG_HIGH() seems to be fine but not 100% convinced */
    MSG_HIGH("IOBEX Evt Handler S GET Ind, id=%x,status=%x", getEvt.conn_id, getEvt.status, 0);

    pMe->state = AEE_OBEX_STATE_GET_REQ_PENDING; //connected to get req pending

    //pNot->uID = AEE_OBEX_EVT_SRV_GET_REQ;
    modelNotif.base.evCode = AEE_OBEX_EVT_SRV_GET_REQ;

    modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(getEvt.status);
    //Enter the apps context before we allocate memory.
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.accessReq.headerCount),
                             &(modelNotif.u.accessReq.headerPtr),
                             getEvt.cmd_headers_cnt,
                             getEvt.cmd_headers_ptr);
    //now leave the app context
    AEE_LeaveAppContext(pc);
    break;
  case  MT_OBEX_EV_SRV_PUT_IND:
    if (pMe->state < AEE_OBEX_STATE_CONNECTED)
    {
      MSG_ERROR("IOBEX - Not yet connected. Cannot accept PUT Request", 0, 0, 0);
      return;
    }

    putEvt = ev_msg_ptr->ev_msg.ev_goep_srv_put_ind;

    /* Use MSG_LOW() here crashes in case of pulling out USB cable while copying file to PC. Use MSG_HIGH() seems to be fine but not 100% convinced */
    MSG_HIGH("IOBEX Evt Handler S Put Ind, id=%x,status=%x", getEvt.conn_id, getEvt.status, 0);

    pMe->state = AEE_OBEX_STATE_PUT_REQ_PENDING; //connected to put req pending

    modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(putEvt.status);
    modelNotif.base.evCode = AEE_OBEX_EVT_SRV_PUT_REQ;
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.accessReq.headerCount),
                             &(modelNotif.u.accessReq.headerPtr),
                             putEvt.cmd_headers_cnt,
                             putEvt.cmd_headers_ptr);
    AEE_LeaveAppContext(pc);
    break;

  case  MT_OBEX_EV_SRV_SET_PATH_IND:
    if (pMe->state < AEE_OBEX_STATE_CONNECTED)
    {
      MSG_ERROR("IOBEX - Not yet connected. Cannot accept SETPATH Request", 0, 0, 0);
      return;
    }

    setPathEvt = ev_msg_ptr->ev_msg.ev_goep_srv_set_path_ind;

    MSG_HIGH("IOBEX Evt Handler S SETPATH Ind, folder_ptr=%x", setPathEvt.folder_ptr, 0, 0);
    pMe->state = AEE_OBEX_STATE_SET_PATH_REQ_PENDING; //connected to setpath req pending
    modelNotif.base.evCode = AEE_OBEX_EVT_SRV_SET_PATH_REQ;
    modelNotif.u.setPathReq.bCreate = setPathEvt.create;
    modelNotif.u.setPathReq.upLevel = setPathEvt.up_level;
    if (setPathEvt.folder_ptr != NULL)
      WSTRLCPY(modelNotif.u.setPathReq.folderName, 
               setPathEvt.folder_ptr,
               AEE_OBEX_MAX_FILE_NAME);
    else
      modelNotif.u.setPathReq.folderName[0] = 0;

    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.setPathReq.headerCount),
                             &(modelNotif.u.setPathReq.headerPtr),
                             setPathEvt.cmd_headers_cnt,
                             setPathEvt.cmd_headers_ptr);
    AEE_LeaveAppContext(pc);
    break;
    //client events
  case MT_OBEX_EV_CLI_CON_CFM:
    conCfmEvt = ev_msg_ptr->ev_msg.ev_goep_cli_con_cfm;

    if (conCfmEvt.status != MT_OBEX_SUCCESS) //then send conn failed notification to app
    {
      modelNotif.base.evCode = AEE_OBEX_EVT_CLI_CONN_FAILED;
      modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(conCfmEvt.status);
      (void)bt_cmd_ec_free_application_id( pMe->appId );
      break;
    }
    else
    {
      //else send connected notification to app and set our state to connected
      pMe->state = AEE_OBEX_STATE_CONNECTED;
      pMe->clientConnID = conCfmEvt.conn_id;
  
      modelNotif.base.evCode = AEE_OBEX_EVT_CLI_CON_CFM;
      modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(conCfmEvt.status);
      modelNotif.u.connectCfm.readOnly = conCfmEvt.read_only;
    }
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.connectCfm.headerCount),
                             &(modelNotif.u.connectCfm.headerPtr),
                             conCfmEvt.rsp_headers_cnt,
                             conCfmEvt.rsp_headers_ptr); 
    AEE_LeaveAppContext(pc);
    break;
  case MT_OBEX_EV_CLI_DCN_IND:
    dcnIndEvt = ev_msg_ptr->ev_msg.ev_goep_cli_dcn_ind;

    if (pMe->clientConnID != dcnIndEvt.conn_id)
    {
      MSG_ERROR("IOBEX Evt Handler Client Disconnect ind - wrong conn id: %x", dcnIndEvt.conn_id, 0, 0);
      return;
    }
    pMe->state = AEE_OBEX_STATE_INIT;
    pMe->clientConnID = AEEOBEX_NO_CONN_ID;
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_DCN_IND;
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEXDisconect_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.discReq.headerCount),
                             &(modelNotif.u.discReq.headerPtr),
                             dcnIndEvt.rsp_headers_cnt,
                             dcnIndEvt.rsp_headers_ptr); 
    AEE_LeaveAppContext(pc);
    (void)bt_cmd_ec_free_application_id( pMe->appId );
    break;

  case MT_OBEX_EV_CLI_AUTH_IND:
    cliAuthEvt = ev_msg_ptr->ev_msg.ev_goep_cli_auth_ind;
    MSG_HIGH("IOBEX Client auth ind. conn id:%x UIDReqd:%x", cliAuthEvt.conn_id, 
                                                cliAuthEvt.user_id_required, 0);
    pMe->clientConnID = cliAuthEvt.conn_id;
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_AUTH_REQ;
    modelNotif.u.authReq.bUserIDRequired = cliAuthEvt.user_id_required;
    modelNotif.u.authReq.bFullAccess = cliAuthEvt.full_access;

    break;

  case MT_OBEX_EV_CLI_AUTH_RESP_IND:
    cliAuthResEvt = ev_msg_ptr->ev_msg.ev_goep_cli_auth_res_ind;
    MSG_HIGH("IOBEX Client auth response ind. conn id : %x", cliAuthResEvt.conn_id, 0, 0);
    pMe->clientConnID = cliAuthResEvt.conn_id;
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_AUTH_RESP;
     // Not used. If the client requies UID from Server, it can be used.
    //modelNotif.u.authResp.wUserID 
    break;

  case MT_OBEX_EV_CLI_GET_RCV_DATA:
    getRcvEvt =
    ev_msg_ptr->ev_msg.ev_goep_cli_get_rcv_data;
    MSG_HIGH("IOBEX Client Evt Handler GET Rcv Data, id=%x, status=%x", getRcvEvt.conn_id, getRcvEvt.status, 0);
    pMe->state = AEE_OBEX_STATE_CONNECTED;
    modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(getRcvEvt.status);
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_GET_RCV_DATA;
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.accessReq.headerCount),
                             &(modelNotif.u.accessReq.headerPtr), 
                             getRcvEvt.rsp_headers_cnt,
                             getRcvEvt.rsp_headers_ptr);
    AEE_LeaveAppContext(pc);
    break;

  case MT_OBEX_EV_CLI_PUT_CFM:
    putCfm = ev_msg_ptr->ev_msg.ev_goep_cli_put_cfm;
    MSG_HIGH("IOBEX Client Evt Handler Put Cfm, id=%x, status=%x", putCfm.conn_id, putCfm.status, 0);
    pMe->state = AEE_OBEX_STATE_CONNECTED;
    modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(putCfm.status);
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_PUT_CFM;
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.accessReq.headerCount),
                             &(modelNotif.u.accessReq.headerPtr), 
                             putCfm.rsp_headers_cnt,
                             putCfm.rsp_headers_ptr);
    AEE_LeaveAppContext(pc);
    break;

  case MT_OBEX_EV_CLI_SET_PATH_CFM:
    setPathCfm = ev_msg_ptr->ev_msg.ev_goep_cli_set_path_cfm;
    MSG_HIGH("IOBEX Client Evt Handler set path Cfm, id=%x, status=%x", setPathCfm.conn_id, setPathCfm.status, 0);
    pMe->state = AEE_OBEX_STATE_CONNECTED;
    modelNotif.base.dwParam = OEMOBEX_MapEvtStatus(setPathCfm.status);
    modelNotif.base.evCode = AEE_OBEX_EVT_CLI_SET_PATH_CFM;
    pc = AEE_EnterAppContext( pMe->pac );
    OEMOBEX_CopyToAppHeaders(ev_msg_ptr->ev_hdr.bt_app_id,
                             &(modelNotif.u.setPathReq.headerCount),
                             &(modelNotif.u.setPathReq.headerPtr), 
                             setPathCfm.rsp_headers_cnt,
                             setPathCfm.rsp_headers_ptr); 
    AEE_LeaveAppContext(pc);
    break;

  default:
    MSG_ERROR("IOBEX Evt Handler: unknown event: %x", ev_msg_ptr->ev_hdr.ev_type, 0, 0);
    return;
  }
  //now queue the notification
  if (pMe->pParent)
  {

     OEMOBEX_QueueUpModelEvent(pMe,modelNotif);
     ISHELL_Resume(pMe->pParent->m_pIShell,
                   &(pMe->cbDeliverModelEvents));
  }

  return;
}

/*
//mt_obex stubs for testing


#if defined(WIN32) && defined(_DEBUG) && !defined(srHOST) //if debug bld and not stride bld, then stub out the foll. funcs




mt_obex_status_type mt_obex_cli_auth_response(
    mt_obex_app_id_type                  app_id,
    mt_obex_cli_conn_id_type             conn_id,
    byte*                                user_id_ptr,
    uint8                                user_id_len,
    char*                                password_ptr )
{
  return MT_OBEX_SUCCESS;
}



mt_obex_tp_support_type mt_obex_get_transports_supported(void)
{
  return MT_OBEX_BLUETOOTH_TP;
}

mt_obex_status_type mt_obex_srv_register(
    mt_obex_app_id_type                  app_id,
    mt_obex_byte_seq_type*               target_ptr,
    mt_obex_srv_auth_type                auth,
    mt_obex_tp_config_struct_type        *tp_config_params,
    uint8*                               ch_num )
{
  return MT_OBEX_SUCCESS;
}

mt_obex_status_type mt_obex_srv_accept_connect(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    boolean                              accept,
    mt_obex_status_type                  status )
{
  return MT_OBEX_SUCCESS;

}

mt_obex_status_type mt_obex_srv_auth_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    const char*                          password_ptr,
    boolean                              read_only )
{
  return MT_OBEX_SUCCESS;
}

mt_obex_status_type mt_obex_srv_confirm_set_path(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_status_type                  status )
{
  return MT_OBEX_SUCCESS;
}


bt_app_id_type bt_cmd_ec_get_app_id_and_register(
                        bt_ev_cb_func_type* ev_cb_ptr )
{
  return 1;
}

bt_cmd_status_type bt_cmd_ec_free_application_id(
                            bt_app_id_type bt_app_id )
{
  return BT_CS_GN_SUCCESS;
}

mt_obex_status_type mt_obex_srv_get_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status )
{
  return MT_OBEX_SUCCESS;
}

mt_obex_status_type mt_obex_srv_put_response(
    mt_obex_app_id_type                  app_id,
    uint16                               conn_id,
    mt_obex_hdr_list_type*               headers_ptr,
    mt_obex_status_type                  status )
{
  return MT_OBEX_SUCCESS;
}

mt_obex_status_type mt_obex_srv_force_disconnect(
    mt_obex_app_id_type                  app_id,
  uint16                               conn_id )
{
  return MT_OBEX_SUCCESS;
}

mt_obex_status_type mt_obex_srv_deregister_server(
    uint16                               app_id,
    uint8                                ch_num )
{
  return MT_OBEX_SUCCESS;
}


#endif
*/

/* Presently this API is not used. 
 * The required initialization is handled internaly.
 * Can be extented, if needed.
 * This needs to be removed after JSR Testing. 
 */  
void OEMOBEX_ClientInitData(IOBEX *po, uint16 index)
{
    mt_obex_client_init_data(index);
    return;
}

// Command to Initiate Client Authentication.
int OEMOBEX_ClientAuthenticate(IOBEX *po, 
                               AEEOBEXDeviceAddressType *pAddr,
                               AEEOBEXRealm *realmInfo,
                               boolean bUidReqd,
                               boolean bFullAccess)
{

    mt_obex_tp_conn_struct_type connStruct;
    mt_realm_info mtRealm;
    mt_obex_status_type status;
    int retStatus;
    OEMOBEXobj_t *pMe;

    if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                            (OEMINSTANCE*)&pMe ) != TRUE)
    {
        return EFAILED;
    }

    if (pMe->state >= AEE_OBEX_STATE_CONNECTED )
    {
        return EBADSTATE;
    }
    
    if((pAddr != NULL) && (realmInfo != NULL))
    {
        connStruct.transport_conn.bt_conn.bd_addr_ptr =
            (bt_bd_addr_type*)pAddr->transport_conn.bt_conn.bd_addr_ptr;
        connStruct.transport_conn.bt_conn.channel = 
            pAddr->transport_conn.bt_conn.channel;

        mtRealm.len = realmInfo->len;
        mtRealm.realmStrType = realmInfo->realmStrType;
        if(realmInfo->len)
        {
            memcpy(&mtRealm.realmStr[0], 
                   realmInfo->realmStr,
                   realmInfo->len);
        }

        status = mt_obex_client_authenticate(&connStruct,
                                             &mtRealm,
                                             bUidReqd,
                                             bFullAccess);

        retStatus = OEMOBEX_CheckCmdStatus(status);
        return retStatus;
    }
    else
    {
        return EBADPARM;
    }
}

// Command to Initiate Server Authentication.
int OEMOBEX_ServerAuthenticate(IOBEX *po,
                               AEEOBEXDeviceAddressType *pAddr,
                               AEEOBEXRealm *realmInfo,
                               boolean bUidReqd,
                               boolean bFullAccess)
{
    mt_obex_tp_conn_struct_type connStruct;
    mt_realm_info mtRealm;
    OEMOBEXobj_t *pMe;
    mt_obex_status_type status;
    int retStatus;

    if (AEEHandle_From( &gOEMOBEXHandleList, po->m_hOBEX,
                  (OEMINSTANCE*)&pMe ) != TRUE)
    {
        return EFAILED;
    }

    if (pMe->state >= AEE_OBEX_STATE_CONNECTED )
    {
        return EBADSTATE;
    }

    if((pAddr != NULL) && (realmInfo != NULL))
    {

        connStruct.transport_conn.bt_conn.bd_addr_ptr =
            (bt_bd_addr_type*)pAddr->transport_conn.bt_conn.bd_addr_ptr;
        connStruct.transport_conn.bt_conn.channel = 
                pAddr->transport_conn.bt_conn.channel;

        mtRealm.len = realmInfo->len;
        if(realmInfo->len)
        {
            STRTOWSTR( (char*)realmInfo->realmStr, (AECHAR *)mtRealm.realmStr, sizeof(mtRealm.realmStr));
        }

        status = mt_obex_server_authenticate(
                                           &connStruct,
                                           pMe->serverConnID,
                                           &mtRealm,
                                           bUidReqd,
                                           bFullAccess);

        retStatus = OEMOBEX_CheckCmdStatus(status);
        return retStatus;
    }
    else
    {
        return EBADPARM;
    }
}

/*
This function check and free header memory. 
It should be called only in the application context and the memory being free 
is allocated in the application context.
*/

static void OEMOBEX_FreeAppHeaders(AEEOBEXModelNotificationDataType *appHdrPtr)
{
    int i = 0;
    AEEOBEXHdrType *pAppHdr = NULL;
    uint8 appHdrCnt = 0;
    uint32 modelEvCode;

    if( appHdrPtr == NULL)
    {
        return;
    }
    //Get model event
    modelEvCode = appHdrPtr->base.evCode;

    switch(modelEvCode)
    {
        case AEE_OBEX_EVT_CONNECTED :
        case AEE_OBEX_EVT_SRV_DCN_IND :
        case AEE_OBEX_EVT_SRV_SET_PATH_REQ :
         return;

        case  AEE_OBEX_EVT_CLI_CON_CFM :
            //Get the corresponding header pointer 
            pAppHdr = appHdrPtr->u.connectCfm.headerPtr;
            //Get number of headers 
            appHdrCnt = appHdrPtr->u.connectCfm.headerCount;
            break;

        case  AEE_OBEX_EVT_SRV_GET_REQ :
        case  AEE_OBEX_EVT_SRV_PUT_REQ :
        case AEE_OBEX_EVT_CLI_GET_RCV_DATA:
        case AEE_OBEX_EVT_CLI_DCN_IND :
        case AEE_OBEX_EVT_CLI_PUT_CFM :
            //Get the corresponding header pointer 
            pAppHdr = appHdrPtr->u.accessReq.headerPtr;
            //Get number of headers 
            appHdrCnt = appHdrPtr->u.accessReq.headerCount;
            break;

        case AEE_OBEX_EVT_CLI_SET_PATH_CFM :
            //Get the corresponding header pointer 
            pAppHdr = appHdrPtr->u.setPathReq.headerPtr;
            //Get number of headers 
            appHdrCnt = appHdrPtr->u.setPathReq.headerCount;
            break;
        default :
            break;
    }

    if(pAppHdr == NULL)
    {
        //Memory not allocated, just return.
        return;
    }

    MSG_HIGH("IOBEX : Free Model event Hdrs. HdrCount=%d", appHdrCnt, 0, 0);
    for (i = 0; i < appHdrCnt; i++)
    {
      switch (pAppHdr[i].id )
      {
      case MT_OBEX_HDR_COUNT: /**< number of objects (used by Connect) */
        break;
      case MT_OBEX_HDR_NAME:  /**< name of the object (often a file name) */
        FREE(pAppHdr[i].val.name.str);
        break;
      case MT_OBEX_HDR_TYPE:  /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
        FREE(pAppHdr[i].val.type.data);
        break;
      case MT_OBEX_HDR_LENGTH: /**< the length of the object in bytes */
        break;
      case MT_OBEX_HDR_TIME:   /**< date/time stamp (ISO 8601) */
        FREE(pAppHdr[i].val.time.data); 
        break;
      case MT_OBEX_HDR_DESCRIPTION: /**< text description of the object */
        FREE(pAppHdr[i].val.description.str); 
        break;
      case MT_OBEX_HDR_TARGET:  /**< name of service to which operation is targeted */
        FREE(pAppHdr[i].val.target.data); 
        break;
      case MT_OBEX_HDR_HTTP:    /**< HTTP 1.x header */
        FREE(pAppHdr[i].val.http.data); 
        break;
      case MT_OBEX_HDR_BODY:    /**< a chunk of the object body. */
        FREE(pAppHdr[i].val.body.data); 
        break;
      case MT_OBEX_HDR_END_OF_BODY: /**< the final chunk of the object body */
        FREE(pAppHdr[i].val.endOfBody.data);         
        break;
      case MT_OBEX_HDR_WHO:           /**< identifies the OBEX application; used to tell if talking to a peer */
        FREE(pAppHdr[i].val.who.data); 
        break;
      case MT_OBEX_HDR_CONNECTION_ID: /**< an identifier used for OBEX connection multiplexing */
        break;
      case MT_OBEX_HDR_APPLICATION_PARAMS:        /**< extended application request and response information */
        FREE(pAppHdr[i].val.applicationParams.data);
        break;
      case MT_OBEX_HDR_AUTHENTICATION_CHALLENGE:  /**< authentication digest-challenge */
        FREE(pAppHdr[i].val.authChallenge.data); 
        break;
      case MT_OBEX_HDR_AUTHENTICATION_RESPONSE:   /**< authentication digest-response */
        FREE(pAppHdr[i].val.authResponse.data); 
        break;
      case MT_OBEX_HDR_OBJECT_CLASS:   /* object class */
        FREE(pAppHdr[i].val.objectClass.data); 
        break;
      default:
        //should never happen
        MSG_ERROR ("Unknown header type seen in OEMOBEX_CopyToAppHeaders function: %d at hdr index: %d\n" , pAppHdr[i].id, i, 0);

        appHdrCnt--; //for every unknown header, we decrement the count
        break;
      }
    }
    //Free the main header pointer....
    FREE(pAppHdr); 
}

/*
This function check and free notification events header memory.
It should always be called from the application context or by the application task.
*/

void OEMOBEX_FreeAppNotificationEventHeaders(uint32 uID, OBEXNotificationDataType *notifyData)
{
    int i = 0;
    AEEOBEXHdrType *pAppHdr = NULL;
    uint8 appHdrCnt = 0;

    /*
    Connection request notification event sends headers to app and need to free 
    headers and header pointer only for connection request event, for any other
    notification event, just return.
    */
    if(uID != AEE_OBEX_EVT_SRV_CON_REQ)
    {
        MSG_HIGH("IOBEX : Not connection request notification event ", 0, 0, 0);
        return;
    }
    
    if(notifyData == NULL)
    {   MSG_ERROR ("IOBEX : FreeNotify event recd NULL Notification data",0,0,0);
        return;
    }

    pAppHdr = notifyData->ConnReq.headerPtr;
    appHdrCnt = notifyData->ConnReq.headerCount;

    MSG_HIGH("IOBEX : Freeing Notification Event Headers HdrCount=%d", appHdrCnt, 0, 0);

    if(pAppHdr == NULL)
    {
     //The notication event is sent to app with empty headers.
        MSG_HIGH("IOBEX : Notification event has empty event headers to free", 0, 0, 0);
        return;
    }

    for (i = 0; i < appHdrCnt; i++)
    {
      switch (pAppHdr[i].id)
      {
      case MT_OBEX_HDR_COUNT: /**< number of objects (used by Connect) */
        break;
      case MT_OBEX_HDR_NAME:  /**< name of the object (often a file name) */
        FREE(pAppHdr[i].val.name.str);
        break;
      case MT_OBEX_HDR_TYPE:  /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
        FREE(pAppHdr[i].val.type.data);
        break;
      case MT_OBEX_HDR_LENGTH: /**< the length of the object in bytes */
        break;
      case MT_OBEX_HDR_TIME:   /**< date/time stamp (ISO 8601) */
        FREE(pAppHdr[i].val.time.data); 
        break;
      case MT_OBEX_HDR_DESCRIPTION: /**< text description of the object */
        FREE(pAppHdr[i].val.description.str); 
        break;
      case MT_OBEX_HDR_TARGET:  /**< name of service to which operation is targeted */
        FREE(pAppHdr[i].val.target.data); 
        break;
      case MT_OBEX_HDR_HTTP:    /**< HTTP 1.x header */
        FREE(pAppHdr[i].val.http.data); 
        break;
      case MT_OBEX_HDR_BODY:    /**< a chunk of the object body. */
        FREE(pAppHdr[i].val.body.data); 
        break;
      case MT_OBEX_HDR_END_OF_BODY: /**< the final chunk of the object body */
        FREE(pAppHdr[i].val.endOfBody.data);         
        break;
      case MT_OBEX_HDR_WHO:           /**< identifies the OBEX application; used to tell if talking to a peer */
        FREE(pAppHdr[i].val.who.data); 
        break;
      case MT_OBEX_HDR_CONNECTION_ID: /**< an identifier used for OBEX connection multiplexing */
        break;
      case MT_OBEX_HDR_APPLICATION_PARAMS:        /**< extended application request and response information */
        FREE(pAppHdr[i].val.applicationParams.data);
        break;
      case MT_OBEX_HDR_AUTHENTICATION_CHALLENGE:  /**< authentication digest-challenge */
        FREE(pAppHdr[i].val.authChallenge.data); 
        break;
      case MT_OBEX_HDR_AUTHENTICATION_RESPONSE:   /**< authentication digest-response */
        FREE(pAppHdr[i].val.authResponse.data); 
        break;
      case MT_OBEX_HDR_OBJECT_CLASS:   /* object class */
        FREE(pAppHdr[i].val.objectClass.data); 
        break;
      default:
        //should never happen
        MSG_ERROR ("Unknown header type seen in FreeAppNotification event headers function: %d at hdr index: %d\n" , pAppHdr[i].id, i, 0);
        appHdrCnt--; //for every unknown header, we decrement the count
        break;
      }
    }
    //Free the main header pointer....
    FREE(pAppHdr); 
}

#endif /*FEATURE_IOBEX */

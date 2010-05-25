/*=============================================================================
FILE:         OEMBTSPP.c

SERVICES:     OEMBTSPP implementation

DESCRIPTION:  This file handles the OEM implemenation for the 
              IBTSPPServer and IBTSPPClient interface 

===============================================================================
           Copyright (c) 2007-2009 QUALCOMM Incorporated.
                All Rights Reserved.
           Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTSPP.c#2 $ 
$DateTime: 2009/04/14 15:01:41 $
$Author: phuongn $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2008-10-06   pr  Removing deprecated function calls
  2008-10-02   rs  Removing compiler and Lint warnings.
  2008-09-17   rs  Having a relatively small watermark for the BT simulation.
  2008-09-18   pr  Klocwork fixes
=============================================================================*/
#include "AEE_OEM.h"
#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)

#include "OEMHeap.h"
#include "bt.h"

#define  FEATURE_DSM_WM_CB
#include "btsio.h"
#include "btsd.h"

#ifndef BT_SIMULATION
#include "msg.h"
#else
#include "bt_sim_interface.h"
#endif
#include "btmsg.h"

#include "OEMOS.h"
#include "OEMBTSPP.h"
#include "OEMBT_priv.h"

#ifdef   FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

#include "AEEStdLib.h"
#include "AEEHandle.h"
#include "AEEStd.h"


#define BT_MSG_LAYER BT_MSG_SP

//==========================================================================
//   Macro definitions
//==========================================================================

#ifndef BT_SIMULATION
#define WATERMARK_HI           9000
#define WATERMARK_LO           5000
#define WATERMARK_MAX         10000
#else
#define WATERMARK_HI           200
#define WATERMARK_LO           20
#define WATERMARK_MAX          240
#endif

//NOTE : If this macro changes, all function pointer tables
//       will have to be updated as well. 
#define MAX_SPP_STREAMS           5

#define MAX_SPP_SERVERS           BT_SD_MAX_NUM_OF_DISCOVERED_DEVICE

#define OEMBTSPP_NUM_EVENTS   50

#define EV_RANGE(x, y) (( ((x) >> 12) & ((y) >> 12)) == ((y) >> 12 ) ) 


/*============================================================================
  Allows only one SPP client object to perform device discovery at a time
============================================================================*/
#define OEMBTSPP_SRCH_OFF    0 // no inquiry, no service search
#define OEMBTSPP_INQ_ON      1 // inquiry on
#define OEMBTSPP_SRCH_ON     2 // service search on
#define OEMBTSPP_CANCL_ON    3 // user cancelld search, or timed out, initiated
// cancellation. 

//==========================================================================
//   Type definitions
//==========================================================================

typedef enum
{
   OEMBTSPP_NO_OP,
   OEMBTSPP_OP_CONNECT,
   OEMBTSPP_OP_SRCH,
   OEMBTSPP_OP_CANCEL,
   OEMBTSPP_OP_ENABLE,
   OEMBTSPP_OP_DISABLE,
   OEMBTSPP_OP_AUTHORIZE,
   OEMBTSPP_OP_DISCONNECT,
   OEMBTSPP_OP_IOCTL,
   OEMBTSPP_OP_CONFIGURE,
   OEMBTSPP_OP_OPENED, // for the server when the client disconnects 
   OEMBTSPP_OP_RELEASE
}oembtspp_op_type;

// list of SPP devices for Client APPs to connect to
typedef struct OEMBTSPP_DEVINFO
{
   q_link_type          link;
   BTDeviceInfo         bdi;
   uint32               uNumSPPServices;
   BTSPPServiceRecord*  pRecs;
}OEMBTSPP_DEVINFO;


typedef struct OEMBTSPP_SERVER
{
   AEECallback*        pcbAuth;
   BTEventData*        pEvData;
   BTSecurityLevel     secLevel;
   char                svcName[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ]; 
   uint16              version;
   BTServiceClassUUID  uuid;
   boolean             bUnRegSdp;
   boolean             bSecSet;
}OEMBTSPP_SERVER;


typedef struct OEMBTSPP_CLIENT
{
   uint32              uSrchSt; // to allow reading of servers independent
                                // of other SPP objs.
                                // gSppDevDiscState is used to prevent device
                                // discovery from being initiated by two
                                // ports simultaneously. 
   uint32              uMaxNumDevReq;
   AEECallback*        pcbDevDisc;
   uint32*             pDevInfoSize;  
   q_type              servQueue;   // only SPP servers 
   q_type              discDevQueue;  // all discovered devices
   uint32              uNumDevFound;
   uint32              uSearchTime;

}OEMBTSPP_CLIENT;


typedef union OEMBTSPP_ROLE_DATA
{
   OEMBTSPP_SERVER  serv;
   OEMBTSPP_CLIENT  cli;

}OEMBTSPP_ROLE_DATA;



typedef struct OEMBTSPP
{
   // index into callback function tables
   uint8               uIndex; 
   int                 lastErr; 
   // App id that is given by the Bt driver
   bt_app_id_type      appId; 

   // Application context required for callbacks.
   ACONTEXT*           pac; 
   AEECallback         BREWEvCb;    

   // The stream ID; valid from Open to Close
   sio_stream_id_type  streamID;

   // Tx and Rx queues to handle data on the stream
   dsm_watermark_type  txWaterMark;
   dsm_watermark_type  rxWaterMark;
   q_type              txQueue;
   q_type              rxQueue;
   dsm_item_type*      pDsmItem;

   // Configuration of the stream and Ioctls
   BTSPPConfig         config; 
   BTSPPConfig*        pConfig; 
   AEECallback*        pcbIoctlResult;

   // keeps track of the result of the last operation.
   BTResult            result; 
   BTResult*           pResult;
    // for Authorize, connect, disconnect, disable, enable operations.
   AEECallback*         pcbResult;

   // to handle writes
   boolean             bOKToSend;
   
   AEECallback*        pcbWrite;

   // Reads
   AEECallback*        pcbRead;

   // keeps a track of changes in modem, line and state change
   OEMBTSPP_STATUS     statusChanged;

   // Handle Status change: closed, opened, connected, ...
   AEEBTSPPState       state;
   AEECallback*        pcbStateChange;

   // Track modem status lines
   BTSPPModemStatus    modemStatus;
   AEECallback*        pcbModemChange;

   // Track line errors
   AEECallback*        pcbLineError;
   BTSPPLineError      lineError;

   boolean             bClientApp;
   
   // BD address of remote device
   BDAddress           bdAddr;
   uint8               uChannelNumber;

   uint32              uRef;
   uint8               currOp;
   BTResult            closeReason; 

   boolean             bDCE;  
   AEECallback*        pcbFlushTx;
   OEMBTSPP_ROLE_DATA  role;

   // linked into mgr
   struct OEMBTSPP*    pNextSPP;
} OEMBTSPP;

// singly linked list of SPPs
typedef struct
{
   OEMBTSPP*         pNextSPP;
   boolean           bIdxTaken[ MAX_SPP_STREAMS ];
} OEMBTSPP_Mgr;



typedef void (*OEMBTSPP_ev_hndlr_type) (bt_ev_msg_type* bt_ev_ptr);

//==========================================================================
//   Function prototypes
//==========================================================================


// static helper functions
static void OEMBTSPP_BREWCallEventCB( OEMBTSPP* pMe );
static void OEMBTSPP_Cancel( AEECallback* pcb );
static void OEMBTSPP_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr);
static void OEMBTSPP_store_bt_event( bt_ev_msg_type* bt_ev_ptr );

static uint16 OEMBTSPP_GetMaxEventBytes( void );
static void OEMBTSPP_UpdateServSDPRec( OEMBTSPP* pMe, boolean enable );
static void OEMBTSPP_InquiryTimeout(void* pData);
static void OEMBTSPP_SearchTimeout(void* pData);

static OEMBTSPP* OEMBTSPP_GetObj( BTHandle hBT );
static OEMBTSPP* OEMBTSPP_FindMe( sio_stream_id_type streamID);
static OEMBTSPP* OEMBTSPP_FindMeFromAppId( bt_app_id_type bt_app_id);
static OEMBTSPP* OEMBTSPP_FindMeFromIndex( uint32 index );

static BTResult OEMBTSPP_ConvertBTReason( bt_event_reason_type bt_reason );
static BTResult OEMBTSPP_CmdStatusToAeeResult( bt_cmd_status_type stat );
static int  OEMBTSPP_CheckCmdStatus( bt_cmd_status_type stat );

static void OEMBTSPP_InitStream( OEMBTSPP* pMe,
                                 sio_open_type* so,
                                 bt_spp_open_type* bso );
static void OEMBTSPP_InitTxRxBuffer( OEMBTSPP* pMe );
static void OEMBTSPP_StatusChangeCB( bt_spp_status_type* pStatus, uint32 index );
static void OEMBTSPP_ConfigChangeCB( bt_spp_cfg_rpt_type* pCfg );
static void OEMBTSPP_ModemStatusCB( bt_spp_ms_type* pModemStatus );
static void OEMBTSPP_LineErrorCB( bt_spp_le_type* pLineError );

static void OEMBTSPP_DataReceived ( dsm_watermark_type* rx_wm ,void* pCbData );
static void OEMBTSPP_TxLowWater( dsm_watermark_type* ptx_wm, void* pCbData );
static void OEMBTSPP_RxLoWater( dsm_watermark_type* prx_wm, void* pCbData );
static void OEMBTSPP_RxHiWater( dsm_watermark_type* prx_wm, void* pCbData );
static void OEMBTSPP_TxHiWater( dsm_watermark_type* ptx_wm, void* pCbData );
static void OEMBTSPP_EnableDTR( int8 index );
static void OEMBTSPP_FlushTx( int8 index );

static void OEMBTSPP_DevNameResp( OEMBTSPP* pMe, bt_ev_msg_type *bt_ev_ptr );
static void OEMBTSPP_ServiceSearchResp( OEMBTSPP* pMe, 
                                        bt_ev_msg_type* ev_ptr );
static void OEMBTSPP_TimeoutResp( OEMBTSPP* pMe, bt_ev_msg_type *bt_ev_ptr );
static void OEMBTSPP_ErrorResp( OEMBTSPP* pMe, bt_ev_msg_type *bt_ev_ptr );

static void OEMBTSPP_RmEvHndlr( OEMBTSPP* pMe, bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_CmdResp( OEMBTSPP* pMe, bt_ev_gn_cmd_done_type* ev_ptr );

static void OEMBTSPP_EnableDTR0( void );
static void OEMBTSPP_EnableDTR1( void );
static void OEMBTSPP_EnableDTR2( void );
static void OEMBTSPP_EnableDTR3( void );
static void OEMBTSPP_EnableDTR4( void );

static void OEMBTSPP_FlushTx0( void );
static void OEMBTSPP_FlushTx1( void );
static void OEMBTSPP_FlushTx2( void );
static void OEMBTSPP_FlushTx3( void );
static void OEMBTSPP_FlushTx4( void );


static void OEMBTSPP_StatusChangeCB0( bt_spp_status_type* pStatus );
static void OEMBTSPP_StatusChangeCB1( bt_spp_status_type* pStatus );
static void OEMBTSPP_StatusChangeCB2( bt_spp_status_type* pStatus );
static void OEMBTSPP_StatusChangeCB3( bt_spp_status_type* pStatus );
static void OEMBTSPP_StatusChangeCB4( bt_spp_status_type* pStatus );


// Event handler functions:
static void OEMBTSPP_EvFlushTtx( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvEnableDtr( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvTxLoWm( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvRxGne( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvLineError( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvModemStatus( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvConfigChange( bt_ev_msg_type* bt_ev_ptr );
static void OEMBTSPP_EvStatusChange( bt_ev_msg_type* bt_ev_ptr );

// The main event processor:
static void OEMBTSPP_ProcessEvQueue( void );
static void OEMBTSPP_CleanUp(OEMBTSPP* pMe);

//==========================================================================
//   Static data
//==========================================================================

// allow the application to have more than one handle to 
// the same OEM object.
static AEEInstance gpInstances[MAX_SPP_STREAMS*2];

static AEEInstanceList gOEMBTSPPHandleList 
= { gpInstances, sizeof(gpInstances)/sizeof(gpInstances[0]), 0};

static OEMBTSPP_Mgr          gSppMgr;               // manager object
static uint8                 gSppDevDiscState;
static bt_event_q_info_type  gSppEventQueueInfo;
static q_type                gSppEventQueue;


static rex_crit_sect_type    gSppCritSect; 


static sio_vv_func_ptr_type enable_dtr_func_table[ ] =
{
   OEMBTSPP_EnableDTR0,
   OEMBTSPP_EnableDTR1,
   OEMBTSPP_EnableDTR2,
   OEMBTSPP_EnableDTR3,
   OEMBTSPP_EnableDTR4

};

static sio_vv_func_ptr_type flush_tx_func_table[ ] =
{
   OEMBTSPP_FlushTx0,
   OEMBTSPP_FlushTx1,
   OEMBTSPP_FlushTx2,
   OEMBTSPP_FlushTx3,
   OEMBTSPP_FlushTx4

};

static bt_spp_status_fptr_type status_change_func_table[ ] =
{
   OEMBTSPP_StatusChangeCB0,
   OEMBTSPP_StatusChangeCB1,
   OEMBTSPP_StatusChangeCB2,
   OEMBTSPP_StatusChangeCB3,
   OEMBTSPP_StatusChangeCB4

};

int OEMBTSPP_ServClone( BTHandle* phBT,
                        int16     streamID 
                      )
{
   OEMBTSPP* pMe = NULL;

   if ( streamID != SIO_NO_STREAM_ID )
   {
      for ( pMe = gSppMgr.pNextSPP; pMe != NULL; pMe = pMe->pNextSPP )
      {
         if ( pMe->streamID == streamID )
         {
            break;
         }
      }
   }
      
   if ( pMe == NULL )
   {
      MSG_ERROR("IBT SPP Cannot clone SPP object \
                 No matching information for st ID = %d ",streamID,0,0);   

      return EFAILED;
   }

   if ( AEEHandle_To( &gOEMBTSPPHandleList,
                      (OEMINSTANCE *)pMe, phBT ) == FALSE )
   {
      MSG_ERROR("IBT SPP Failed to get a handle for SPP object \n",0, 0, 0);
      return EFAILED;
   }

   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult);
   }
   while ( pMe->pcbIoctlResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbIoctlResult);
   }

   pMe->pConfig = NULL;
   pMe->pResult = NULL;

   while ( pMe->pcbRead != NULL )
   {
      CALLBACK_Cancel( pMe->pcbRead );
   }
   while ( pMe->pcbWrite != NULL )
   {
      CALLBACK_Cancel( pMe->pcbWrite );
   }
   while ( pMe->pcbStateChange != NULL )
   {
      CALLBACK_Cancel( pMe->pcbStateChange );
   }
   while ( pMe->pcbModemChange != NULL )
   {
      CALLBACK_Cancel( pMe->pcbModemChange );
   }
   while ( pMe->pcbLineError != NULL )
   {
      CALLBACK_Cancel( pMe->pcbLineError );
   }

   while ( pMe->pcbFlushTx != NULL )
   {
      pMe->pcbFlushTx->pmc = NULL;
      CALLBACK_Cancel( pMe->pcbFlushTx );
   }
   
   if ( pMe->bClientApp == FALSE )
   {
      while ( pMe->role.serv.pcbAuth != NULL )
      {
         CALLBACK_Cancel( pMe->role.serv.pcbAuth );
         pMe->role.serv.pEvData = NULL;
      }
   }
   else
   {
      while ( pMe->role.cli.pcbDevDisc != NULL )
      {
         CALLBACK_Cancel( pMe->role.cli.pcbDevDisc );
         pMe->role.cli.pDevInfoSize = NULL;
      }
   }

   // get the new app context
   pMe->pac = AEE_GetAppContext();

   ++pMe->uRef;

   return SUCCESS;
}


//==========================================================================
//   Public Funtions
//==========================================================================

int OEMBTSPP_New( BTHandle* phBT,
                  const uint8* pszServiceName,
                  uint16 version,
                  BTServiceClassUUID uuid,
                  boolean bServer)
{
   OEMBTSPP *pMe;
   uint8 i;

   for ( i = 0; i < MAX_SPP_STREAMS; i++ )
   {
      if ( gSppMgr.bIdxTaken[ i ] == FALSE )
      {
         break;
      }
   }

   if ( MAX_SPP_STREAMS == i )
   {
      MSG_ERROR("IBT SPP Cannot allocate a free SPP stream. \
                 Max allowed = %d ",MAX_SPP_STREAMS,0,0);   

      return EFAILED;
   }

   pMe = OEM_Malloc( sizeof (OEMBTSPP));
  
   if ( pMe == NULL )
   {
      MSG_ERROR("IBT SPP Out of memory .Cannot allocate a free SPP stream ",0, 0, 0);  
      return ENOMEMORY;
   }

   MEMSET( pMe, 0, sizeof (OEMBTSPP));

   if ( AEEHandle_To( &gOEMBTSPPHandleList,
                      (OEMINSTANCE *)pMe, phBT ) == FALSE )
   {
      OEM_Free (pMe);
      MSG_ERROR("IBT SPP Failed to get a handle for SPP object \n",0, 0, 0);
      return EFAILED;
   }

   gSppMgr.bIdxTaken[ i ] = TRUE;

   // save the stream index that the stream manager uses
   pMe->uIndex         = i;
   pMe->streamID       = SIO_NO_STREAM_ID;
   pMe->pac            = AEE_GetAppContext();
   pMe->state          = AEEBTSPP_ST_CLOSED;
   pMe->result         = AEEBT_RESULT_SUCCESS;
   pMe->closeReason    = AEEBT_RESULT_SUCCESS;
   pMe->lastErr        = SUCCESS;


   pMe->pcbResult      = NULL; 
   pMe->bOKToSend      = FALSE;
   pMe->pcbWrite       = NULL;
   pMe->pcbRead        = NULL;
   pMe->statusChanged  = 0;
   pMe->pcbStateChange = NULL;


   pMe->pcbModemChange = NULL;
   pMe->pcbLineError   = NULL;
   pMe->uChannelNumber = 0;

   pMe->pConfig        = NULL;
   pMe->pcbIoctlResult = NULL;
   
   pMe->uRef           = 1;
   pMe->currOp         = OEMBTSPP_NO_OP;
   pMe->bDCE           = TRUE;
   pMe->pcbFlushTx     = NULL;
   
   OEMBTSPP_InitTxRxBuffer( pMe );
   
   if ( bServer )
   {
      pMe->bClientApp        = FALSE;
      pMe->role.serv.pcbAuth = NULL;
      pMe->role.serv.pEvData = NULL;
      pMe->role.serv.uuid    = uuid;
      pMe->role.serv.version = version;
      pMe->role.serv.bUnRegSdp  = FALSE;
      pMe->role.serv.bSecSet    = FALSE;

      MEMSET( pMe->role.serv.svcName, 0, (BT_SD_MAX_SERVICE_NAME_LEN + 1) );

      std_strlcpy ((char*)(pMe->role.serv.svcName),
                   (const char*)(pszServiceName),
                    sizeof(pMe->role.serv.svcName));
   }
   else
   {
      pMe->bClientApp = TRUE;
      pMe->role.cli.uMaxNumDevReq  = 0;
      pMe->role.cli.pcbDevDisc      = NULL;
      pMe->role.cli.pDevInfoSize    = NULL;  
      pMe->role.cli.uSrchSt      = OEMBTSPP_SRCH_OFF;
      q_init( &pMe->role.cli.servQueue );
      q_init( &pMe->role.cli.discDevQueue );
      pMe->role.cli.uNumDevFound = 0;
   }

   if ( gSppMgr.pNextSPP == NULL )
   {
      q_init( &gSppEventQueue );
      gSppEventQueueInfo.event_q_ptr = &gSppEventQueue;
      gSppEventQueueInfo.max_q_depth = OEMBTSPP_NUM_EVENTS;

      gSppEventQueueInfo.event_q_bit_mask = BT_EVQ_BREW_SPP_B;
      gSppEventQueueInfo.max_event_bytes = OEMBTSPP_GetMaxEventBytes();
      // Register a callback function with BT
      bt_ec_register_brew_spp( OEMBTSPP_ProcessEvQueue );

      gSppDevDiscState = OEMBTSPP_SRCH_OFF;
   }

   // get the BT app id for this SPP Application
   pMe->appId = bt_cmd_ec_get_app_id_and_register( OEMBTSPP_EventCallback );

   if ( BT_APP_ID_NULL == pMe->appId )
   {
      OEM_Free(pMe);
      MSG_ERROR("IBT SPP  Cannot reg app with BT ",0, 0, 0);
      return EFAILED;
   }
   
   // link into mgr
   pMe->pNextSPP = gSppMgr.pNextSPP;
   gSppMgr.pNextSPP = pMe;

   // Increment the reference count of IBT so that it 
   // is not deleted while the SPP object exists.
   OEMBT_IncRefCnt();

#ifdef BT_SIMULATION   
   if ( pMe->bClientApp == TRUE)
   {
      bt_sim_start(PROFILE_SPP_CLIENT, TRUE);
   }
   else
   {
      bt_sim_start(PROFILE_SPP_SERVER, TRUE);
   }
#endif

   return SUCCESS;
}

//==========================================================================
//   IBTSPPSRV VTABLE (see header files for documentation)
//==========================================================================
int OEMBTSPP_Release( BTHandle hBT )
{
   OEMBTSPP** ppc;
   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   if ( pMe != NULL )
   {
      for ( ppc = &gSppMgr.pNextSPP; *ppc != NULL; ppc = &(*ppc)->pNextSPP )
      {
         if ( (*ppc)->uIndex == pMe->uIndex )
         {
            --pMe->uRef;
            AEEHandle_Clear( &gOEMBTSPPHandleList, hBT );

            if ( pMe->uRef == 0 )
            {   
               MSG_MED( "IBT SPP Release called SPP obj; streamID = "
                        "0x%x ,st= 0x%x",pMe->streamID,pMe->state,0);
               // cancel all the callbacks as the 
               // application is unloading
               while ( pMe->pcbResult != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbResult);
               }
               while ( pMe->pcbIoctlResult != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbIoctlResult);
               }
               pMe->pConfig = NULL;
               pMe->pResult = NULL;
               while ( pMe->pcbRead != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbRead );
               }
               while ( pMe->pcbWrite != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbWrite );
               }
               while ( pMe->pcbStateChange != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbStateChange );
               }
               while ( pMe->pcbModemChange != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbModemChange );
               }
               while ( pMe->pcbLineError != NULL )
               {
                  CALLBACK_Cancel( pMe->pcbLineError );
               }

               while ( pMe->pcbFlushTx != NULL )
               {
                  pMe->pcbFlushTx->pmc = NULL;
                  CALLBACK_Cancel( pMe->pcbFlushTx );
               }

               if ( pMe->bClientApp == FALSE )
               {
                  while ( pMe->role.serv.pcbAuth != NULL )
                  {
                     CALLBACK_Cancel( pMe->role.serv.pcbAuth );
                     pMe->role.serv.pEvData = NULL;
                  }
               }
               else
               {
                  while ( pMe->role.cli.pcbDevDisc != NULL )
                  {
                     CALLBACK_Cancel( pMe->role.cli.pcbDevDisc );
                     pMe->role.cli.pDevInfoSize = NULL;
                  }
               }

               pMe->pac = NULL;

               OEMBTSPP_CleanUp(pMe);
               return SUCCESS;
            }
         }
      } /* End of for */
   }
   return EFAILED;
}

// A malicious app can choose to unload at any instance.
// It can do so while it has initiated an operation but dosent wait for 
// it to complete. It can also unload while there is a stream is open. 
// The OEM layer object will be freed only when the ref count is zero and
// there is no operation that is currently in progress and the stream is
//  properly closed.
// This will also prevent a new SPP object from getting spurious events that were
// left behind by the previous Object as the stream ID and application IDs are
// reused.

static void OEMBTSPP_CleanUp(OEMBTSPP* pMe)
{
   OEMBTSPP** ppc;
   OEMBTEvent* pEv;

   if ( pMe->uRef == 0 )
   {
      MSG_MED( "IBT SPP CleanUp Ref cnt = 0x%x, stID= 0x%x "
               "currOp = 0x%x",pMe->uRef,pMe->streamID, \
               pMe->currOp);

      // The application has decided to unload
#ifdef BT_SIMULATION
     pMe->currOp = OEMBTSPP_NO_OP;
     pMe->streamID = SIO_NO_STREAM_ID;
#endif      
      // there is no operation and the stream is closed and the app
      // decided to unload. So clean up and free the object
      if ( pMe->currOp == OEMBTSPP_NO_OP )
      {
         if ( pMe->streamID == SIO_NO_STREAM_ID)
         {
            // there is no operation and the stream is closed and the app
            // decided to unload. So clean up and free the object
            for ( ppc = &gSppMgr.pNextSPP;
                *ppc != NULL; ppc = &(*ppc)->pNextSPP)
            {
               if ( (*ppc)->uIndex == pMe->uIndex )
               {
                  gSppMgr.bIdxTaken[ pMe->uIndex ] = FALSE;
                  *ppc = pMe->pNextSPP;
                  pMe->pNextSPP = NULL;

                  if ( pMe->bClientApp != FALSE )
                  {
                     OEMBTSPP_DEVINFO *pDevInfo = NULL;
                     
                     while ( q_cnt( &pMe->role.cli.servQueue ) > 0 )
                     {
                        pDevInfo = 
                        (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.servQueue );

                        if (pDevInfo != NULL )
                        {
                           if ( NULL != pDevInfo->pRecs )
                           {
                              OEM_Free ( pDevInfo->pRecs );
                           }
                           OEM_Free ( pDevInfo );
                           pDevInfo = NULL;
                        }
                     }
                     while ( q_cnt( &pMe->role.cli.discDevQueue ) > 0 )
                     {
                        pDevInfo = 
                        (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.discDevQueue );

                        if ( pDevInfo != NULL )
                        {
                           if ( NULL != pDevInfo->pRecs )
                           {
                              OEM_Free(pDevInfo->pRecs);
                              
                           }
                           OEM_Free ( pDevInfo );
                           pDevInfo = NULL;
                        }
                     }
                     
                  }
                  
                  bt_cmd_ec_free_application_id( pMe->appId );
                  // free all DSM items
                  dsm_empty_queue( &pMe->txWaterMark );
                  dsm_empty_queue( &pMe->rxWaterMark );
                  dsm_free_packet( &pMe->pDsmItem );
                  
                  if ( NULL == gSppMgr.pNextSPP )
                  {
                     bt_ec_deregister_brew_spp( OEMBTSPP_ProcessEvQueue );
                  }
                  // clear any events that we may have in the event cache.
                  while ( (pEv = OEMBT_GetReadyEv(pMe->appId)) != NULL )
                  {
                     OEMBT_FreeEv( pEv ); // free the entry
                  }

                  CALLBACK_Cancel( &pMe->BREWEvCb );

                  MSG_HIGH( "IBT SPP CleanUp Freeing OEM object",0,0,0);

                  OEMBT_DecRefCnt();
                  OEM_Free (pMe );
                  return;
               }
            }
         }
         else
         {
            // the stream is still open, initiate close
            pMe->currOp = OEMBTSPP_OP_RELEASE;
            sio_close( pMe->streamID, NULL );
         }

      }
      else
      {
         // there is some operation thats currently in progress
         // and the app unloaded without waiting for it to complete.
         // we just return.The routine that completes
         // the operation will clean up.
         // But if we are the client, and search is on, cancel the 
         // search.
         IShell* pIShell = AEE_GetShell();

         if ( ( pMe->bClientApp != FALSE ) &&
              ( pMe->currOp == OEMBTSPP_OP_SRCH ) )
         {
            if ( pMe->role.cli.uSrchSt == OEMBTSPP_INQ_ON )
            {
               bt_cmd_sd_stop_device_discovery( pMe->appId );
               if ( pMe->role.cli.uSearchTime != 0 )
               {
                  ISHELL_CancelTimer(pIShell,
                                     OEMBTSPP_InquiryTimeout,
                                     (void*)pMe);
               }
            }
            if ( pMe->role.cli.uSearchTime != 0 )
            {
               ISHELL_CancelTimer(pIShell,
                                  OEMBTSPP_SearchTimeout,
                                  (void*)pMe);
            }
            pMe->currOp = OEMBTSPP_OP_CANCEL;
            pMe->role.cli.uSrchSt = OEMBTSPP_CANCL_ON;
         }
      }
   }
   return;
}


/*======================================================================= 
=======================================================================*/

int OEMBTSPP_GetLastError( BTHandle hBT )
{
   OEMBTSPP            *pMe;

   pMe = OEMBTSPP_GetObj( hBT );

   if ( NULL == pMe )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }
   return pMe->lastErr;  
}

/*======================================================================= 
=======================================================================*/
int32 OEMBTSPP_Read( BTHandle   hBT, 
                     char*      pDst, 
                     int32      uMaxBytes
                   )
{
   int32     bytesRead = 0;
   int32     totalBytesRead = 0;
   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );

   if ( (NULL == pMe) || (0 == uMaxBytes) ||
        ((pDst != NULL) &&
        (!AEE_CHECK_PTR( "OEMBTSPP_Read", pDst, uMaxBytes, TRUE))) ||
        (pMe->pac != AEE_GetAppContext()))
   {
      return AEEPORT_ERROR;
   }

   MSG_LOW( "IBT SPP Rd str id 0x%x, ind  0x%x ",
            pMe->streamID,pMe->uIndex,0 );

   if ( uMaxBytes > WATERMARK_MAX )
   {
      uMaxBytes = WATERMARK_MAX;
   }

   while (TRUE)
   {
#ifndef BT_SIMULATION
      if ( NULL != pMe->pDsmItem )
      {
         if ( pDst != NULL )
         {
            bytesRead = dsm_pullup( &pMe->pDsmItem, ( pDst+totalBytesRead ),
                                  ( uMaxBytes-totalBytesRead ) );
         }
         else
         {
            bytesRead = dsm_pullup( &pMe->pDsmItem, pDst,
                                  ( uMaxBytes-totalBytesRead ) );
         }
         if ( bytesRead == 0 )
         {
            MSG_ERROR( "IBT SPP SPP_Read - dsm_pullup() returned 0", 0, 0, 0 );
            break;
         }

         totalBytesRead += bytesRead;
         
         if ( totalBytesRead >= uMaxBytes )
         {
            // destination full?
            break;
         }
         /* dsm_pullup will free the dsm_item after pulling off the last byte */
      }

      if ( NULL == pMe->pDsmItem )
      {
         // DSM item has been freed?
         pMe->pDsmItem = dsm_dequeue( &pMe->rxWaterMark ); // get next one
         if ( NULL == pMe->pDsmItem )
         {
            // rx queue is empty?
            break;
         }
      }
#else
       //Go read from Temp SIO buffer for BT Simulation
      totalBytesRead = read_sio_buffer(pDst, uMaxBytes);
      break;
#endif
   }
   if ( totalBytesRead == 0 )
   {
      return AEEPORT_WAIT;
   }
   else
   {
      MSG_HIGH( "IBT SPP Rd tot bytes 0x%x", totalBytesRead, 0, 0 );
      return totalBytesRead;
   }
}

/*======================================================================= 
=======================================================================*/
void OEMBTSPP_Readable( BTHandle      hBT,
                        AEECallback*  pCb,
                        IShell*       pIShell )
{

   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pCb == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_Readable", pCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pCb->pfnNotify == NULL) ||
        (pMe->pac != AEE_GetAppContext() ))
   {
      return;
   }

   CALLBACK_Cancel( pCb );

   MSG_LOW( "IBT SPP Rdble str id 0x%x, ind 0x%x ", 
            pMe->streamID,pMe->uIndex,0 );


   // readable?
   if ( q_cnt( pMe->rxWaterMark.q_ptr ) > 0 )
   {
      ISHELL_Resume( pIShell, pCb );
   }
   else
   {
      MSG_LOW( "IBT SPP Q Rdble cb str id 0x%x, ind = %x , curr  cnt = %x",
               pMe->streamID,pMe->uIndex,pMe->rxWaterMark.current_cnt);

      // only one callback for reads
      while ( pMe->pcbRead != NULL )
      {
         CALLBACK_Cancel( pMe->pcbRead );
      }
      // queue callback
      pCb->pCancelData  = &pMe->pcbRead;
      pCb->pfnCancel    = OEMBTSPP_Cancel;
      pCb->pNext        = pMe->pcbRead;
      pMe->pcbRead      = pCb;
   }
}

/*======================================================================= 
=======================================================================*/
int32 OEMBTSPP_Write( BTHandle hBT, 
                      const char* pSrc, 
                      int32 uNumBytes )

{
   int32          bytesWritten  = 0;
   int32          count;
   OEMBTSPP*      pMe = OEMBTSPP_GetObj( hBT );
   uint32         qCnt = 0;

#ifdef BT_SIMULATION
   char *sio_buf = NULL;
#else
   dsm_item_type* dsm_ptr;
#endif

   if ( (pMe == NULL) || (pSrc == NULL) || ( uNumBytes == 0 ) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return AEEPORT_ERROR;
   }
   // pointer validation
   if ( !AEE_CHECK_PTR( "OEMBTSPP_Write", pSrc, uNumBytes, FALSE ) )
   {
      return AEEPORT_ERROR;
   }

   if ( SIO_NO_STREAM_ID == pMe->streamID )
   {
      pMe->lastErr = AEEPORT_ENOTOPEN;
      return AEEPORT_ERROR;
   }

   if (uNumBytes > WATERMARK_MAX )
   {
      uNumBytes = WATERMARK_MAX;
   }

   while ((pMe->state == AEEBTSPP_ST_CONNECTED) &&
          pMe->bOKToSend && (uNumBytes > 0) )
   {
#ifndef BT_SIMULATION

      qCnt = pMe->txWaterMark.current_cnt; 
      if ( qCnt >= WATERMARK_HI )
      {
         break;
      }
      dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM,
                                     MIN( uNumBytes,
                                          WATERMARK_HI + 1 - qCnt ));
      if ( dsm_ptr == NULL )
      {
         break;
      }
      count = dsm_pushdown_tail( &dsm_ptr, pSrc, 
                                 MIN( uNumBytes, WATERMARK_HI + 1- qCnt), 
                                 (dsm_mempool_id_enum_type)dsm_ptr->pool_id );

      sio_transmit( pMe->streamID, dsm_ptr );
#else
      /*For Simulation copy the entire data to a BYTE array.*/
      sio_buf = (char*) MALLOC(uNumBytes);
      memcpy(sio_buf, pSrc, uNumBytes);
     if (sio_buf != NULL)
     {
        // Send the Data to SIO. 
        sio_transmit(pMe->streamID,sio_buf, uNumBytes);
     }
     count = uNumBytes;
#endif

      pSrc          += count;
      bytesWritten  += count;
      uNumBytes     -= count;
   }

   MSG_LOW( "IBT SPP Wr str id 0x%x, ind 0x%x ", \
            pMe->streamID,pMe->uIndex,0 );


   if ( bytesWritten == 0 )
   {
#ifndef BT_SIMULATION
       MSG_HIGH( "IBT SPP Wr buf full; okToSend 0x%x dsm 0x%x qcnt 0x%x",
                 pMe->bOKToSend, dsm_ptr, qCnt );
#endif
      return AEEPORT_WAIT;
   }
   else
   {
      MSG_LOW( "IBT SPP Wr tot bytes 0x%x", bytesWritten, 0, 0 );
      return bytesWritten;
   }

}

/*======================================================================= 
=======================================================================*/
void OEMBTSPP_Writable( BTHandle hBT, AEECallback* pCb, IShell* pIShell )
{

   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pCb == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_Writable", pCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pCb->pfnNotify == NULL) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return;
   }

   CALLBACK_Cancel( pCb );

   if ( (pMe->bOKToSend) &&
        (pMe->state == AEEBTSPP_ST_CONNECTED) )
   {
      MSG_LOW( "IBT SPP Wtble Tx strm id 0x%x index 0x%x ",
               pMe->streamID,pMe->uIndex,0);

      ISHELL_Resume( pIShell, pCb );
   }
   else
   {
      MSG_LOW( "IBT SPP Q wtble cb str id 0x%x index 0x%x ",
               pMe->streamID,pMe->uIndex,0);

      while ( pMe->pcbWrite != NULL )
      {
         CALLBACK_Cancel( pMe->pcbWrite );
      }

      // queue callback
      pCb->pCancelData  = &pMe->pcbWrite;
      pCb->pfnCancel    = OEMBTSPP_Cancel;
      pCb->pNext        = pMe->pcbWrite;
      pMe->pcbWrite     = pCb;
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_InitTxRxBuffer( OEMBTSPP* pMe )
{
   /* set up TX watermark */
   dsm_queue_init( &pMe->txWaterMark, WATERMARK_MAX, &pMe->txQueue );
   pMe->txWaterMark.lo_watermark         = WATERMARK_LO;
   pMe->txWaterMark.hi_watermark         = WATERMARK_HI;
   pMe->txWaterMark.lowater_func_ptr     = (wm_cb_type)OEMBTSPP_TxLowWater;
   pMe->txWaterMark.lowater_func_data    = (void*)pMe->appId;
   pMe->txWaterMark.hiwater_func_ptr     = (wm_cb_type)OEMBTSPP_TxHiWater;
   pMe->txWaterMark.hiwater_func_data    = (void*)pMe->appId;

   /* set up RX watermark */
   dsm_queue_init( &pMe->rxWaterMark, WATERMARK_MAX, &pMe->rxQueue );
   pMe->rxWaterMark.lo_watermark         = WATERMARK_LO;
   pMe->rxWaterMark.hi_watermark         = WATERMARK_HI;
   pMe->rxWaterMark.lowater_func_ptr     = (wm_cb_type)OEMBTSPP_RxLoWater;
   pMe->rxWaterMark.lowater_func_data    = (void*)pMe->appId;
   pMe->rxWaterMark.non_empty_func_ptr   = (wm_cb_type)OEMBTSPP_DataReceived;
   pMe->rxWaterMark.non_empty_func_data  = (void*)pMe->appId;
   pMe->rxWaterMark.hiwater_func_ptr     = (wm_cb_type)OEMBTSPP_RxHiWater;
   pMe->rxWaterMark.hiwater_func_data    = (void*)pMe->appId;

   pMe->pDsmItem                         = NULL;

}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_InitStream( OEMBTSPP* pMe,
                                 sio_open_type* so,
                                 bt_spp_open_type* bso )
{

   OEMBTSPP_InitTxRxBuffer( pMe );

   so->port_id                = SIO_PORT_BT_SPP;
   so->stream_mode            = SIO_GENERIC_MODE;
   so->tx_flow                = SIO_FCTL_OFF;
   so->rx_flow                = SIO_FCTL_OFF;
   so->tx_queue               = &pMe->txWaterMark;
   so->rx_queue               = &pMe->rxWaterMark;
   so->rx_func_ptr            = NULL;
   so->bt_open_ptr            = bso;

   bso->max_frame_size        = BT_SPP_MFS_AUTOMATIC; 
   bso->status_change_fptr    = status_change_func_table[pMe->uIndex] ;
   bso->config_change_fptr    = OEMBTSPP_ConfigChangeCB;
   bso->modem_status_fptr     = OEMBTSPP_ModemStatusCB;
   bso->line_error_fptr       = OEMBTSPP_LineErrorCB;

   return;
}


/*======================================================================= 
=======================================================================*/
int OEMBTSPPSRV_EnableService( BTHandle         hBT,
                               BTSecurityLevel  secLevel,
                               uint32           uServerChannel,
                               boolean          bUnRegSdp,
                               BTResult*        pResult,
                               AEECallback*     pResultCb)
{

   sio_open_type     so;
   bt_spp_open_type  bso;
   OEMBTSPP*         pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pResultCb == NULL) || (pResult == NULL)||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_EnableService", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL) ||
        (secLevel > AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_EnableService", pResult, 
                         sizeof(BTResult), FALSE )) ||
        (pMe->pac != AEE_GetAppContext()) ||
        ((pMe->role.serv.uuid == BT_SD_INVALID_UUID) && ( bUnRegSdp == TRUE )))
   {
      return EBADPARM;
   }

   if ( pMe->streamID != SIO_NO_STREAM_ID )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   pMe->role.serv.secLevel    = secLevel;
   pMe->role.serv.bUnRegSdp   = bUnRegSdp;

   CALLBACK_Cancel(pResultCb);

   // only one callback registered at any time
   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult );
   }
   // queue callback
   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;
   pMe->pResult            = pResult;

   // free all DSM items if there were any from the
   // previous connection
   dsm_empty_queue( &pMe->txWaterMark );
   dsm_empty_queue( &pMe->rxWaterMark );
   dsm_free_packet( &pMe->pDsmItem );

   OEMBTSPP_InitStream(pMe,&so,&bso);

   bso.client_app            = FALSE;// false for server
   bso.bd_addr_ptr           = NULL;  

   //BT_SD_SERVICE_CLASS_SERIAL_PORT;
   bso.service_uuid          = pMe->role.serv.uuid ;
   bso.service_version       = pMe->role.serv.version;
   if ( uServerChannel != 0 )
   {
      bso.rc_server_channel     = (bt_spp_scn_enum_type)uServerChannel;
   }
   else
   {
      bso.rc_server_channel     = BT_SPP_SCN_UNSPECIFIED;
   }
   bso.service_name_str_ptr  = pMe->role.serv.svcName ;

   MSG_LOW( "IBT SPP enabling SPP service",0,0,0);

   pMe->currOp = OEMBTSPP_OP_ENABLE;

   rex_enter_crit_sect( &gSppCritSect );
   pMe->streamID = SIO_MAX_STREAM; // indicates opening
   pMe->streamID = sio_open( &so );
   rex_leave_crit_sect( &gSppCritSect );

   if ( pMe->streamID == SIO_NO_STREAM_ID )
   {
      pMe->currOp = OEMBTSPP_NO_OP;
   }

   return( pMe->streamID == SIO_NO_STREAM_ID ) ? EFAILED : SUCCESS;
}


/*======================================================================= 
=======================================================================*/
int OEMBTSPPSRV_Authorize( BTHandle          hBT,
                           const BDAddress*  pBDAddr,
                           boolean           bAccept,
                           BTResult*         pResult,
                           AEECallback*      pResultCb)
{

   bt_cmd_status_type  stat;
   bt_service_id_type  service_ident;
   OEMBTSPP*           pMe = OEMBTSPP_GetObj( hBT );
   int retStat;

   if ( (pMe == NULL) || (pBDAddr == NULL) ||
        (pResultCb == NULL) || (pResult == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_Authorize", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL)||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_Authorize", pResult, 
                         sizeof(BTResult), FALSE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( pMe->streamID == SIO_NO_STREAM_ID )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   CALLBACK_Cancel(pResultCb);

   // only one callback registered at any time
   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult );
   }
   // queue callback
   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;
   pMe->pResult            = pResult;

   service_ident.service_id_method = BT_SIM_RFCOMM_SCN;
   service_ident.rfcomm_scn        = pMe->uChannelNumber;

   MSG_MED( "IBT SPP Authorizing client.Accept = 0x%x",bAccept,0,0);

   BT_BDA( MSG_API, "IBT SPP client", (bt_bd_addr_type*)pBDAddr );

   pMe->currOp = OEMBTSPP_OP_AUTHORIZE;

   stat = bt_cmd_rm_authorize_reply( pMe->appId, &service_ident,
                                     (bt_bd_addr_type*)pBDAddr, bAccept );

   retStat = OEMBTSPP_CheckCmdStatus( stat );

   if ( retStat != SUCCESS )
   {
      pMe->currOp = OEMBTSPP_NO_OP;
   }

   return retStat;
}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPSRV_OnConnRequest( BTHandle       hBT,
                               AEECallback*   pAuthCb,
                               BTEventData*   pEvData )
{

   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pAuthCb == NULL) || (pEvData == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_OnConnRequest", pAuthCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pAuthCb->pfnNotify == NULL ) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_OnConnRequest", pEvData, 
                         sizeof(BTEventData), TRUE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   CALLBACK_Cancel( pAuthCb );

   // only one callback registered at any time
   while ( pMe->role.serv.pcbAuth != NULL )
   {
      CALLBACK_Cancel( pMe->role.serv.pcbAuth );
   }
   // queue Authorisation callback
   pAuthCb->pCancelData    = &pMe->role.serv.pcbAuth;
   pAuthCb->pfnCancel      = OEMBTSPP_Cancel;
   pAuthCb->pNext          = pMe->role.serv.pcbAuth;
   pMe->role.serv.pcbAuth  = pAuthCb;
   pMe->role.serv.pEvData  = pEvData;

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPSRV_DisableService( BTHandle          hBT,
                                BTResult*         pResult,
                                AEECallback*      pResultCb)
{

   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pResultCb == NULL) || (pResult == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_DisableService", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL ) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_DisableService", pResult, 
                         sizeof(BTResult), TRUE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( pMe->streamID == SIO_NO_STREAM_ID )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   MSG_MED( "IBT SPP Disabling Service; strID 0x%x ind 0x%x",
             pMe->streamID,pMe->uIndex,0);

   while ( pMe->role.serv.pcbAuth != NULL )
   {
      CALLBACK_Cancel( pMe->role.serv.pcbAuth );
   }

   // only one callback registered at any time
   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult );
   }
   // queue callback
   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;
   pMe->pResult            = pResult;


   // free all DSM items
   dsm_empty_queue( &pMe->txWaterMark );
   dsm_empty_queue( &pMe->rxWaterMark );
   dsm_free_packet( &pMe->pDsmItem );


   pMe->currOp = OEMBTSPP_OP_DISABLE;
   pMe->closeReason = AEEBT_RESULT_NORMAL_DISCONNECT;
   sio_close( pMe->streamID, NULL );
   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
int OEMBTSPP_OnStatusChange( BTHandle        hBT,
                             AEECallback*    pStatusCb,
                             OEMBTSPP_STATUS statusType,
                             IShell*         pIShell)
{
   AEECallback**  pCb = NULL; 
   OEMBTSPP*      pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pStatusCb == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_OnStatusChange", pStatusCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pStatusCb->pfnNotify == NULL ) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   CALLBACK_Cancel( pStatusCb );

   if ( pMe->statusChanged & statusType )
   {
      ISHELL_Resume( pIShell, pStatusCb );
   }
   else
   {

      switch (statusType)
      {
      case OEMBTSPP_STATE_CHANGE :
         pCb = &pMe->pcbStateChange;
         break;
      case OEMBTSPP_MODEM_CHANGE :
         pCb = &pMe->pcbModemChange;
         break;
      case OEMBTSPP_LINE_ERROR :
         pCb = &pMe->pcbLineError;
         break;
      }
      while ( *pCb != NULL )
      {
         CALLBACK_Cancel( *pCb );
      }
      // queue callback
      pStatusCb->pCancelData  = pCb;
      pStatusCb->pfnCancel    = OEMBTSPP_Cancel;
      pStatusCb->pNext        = *pCb;
      *pCb                    = pStatusCb;
   }
   return SUCCESS;
}


/*======================================================================= 
=======================================================================*/
int OEMBTSPP_Disconnect( BTHandle      hBT,
                         BTResult*     pResult,
                         AEECallback*  pResultCb
                       )
{

   sio_ioctl_param_type  param;
   OEMBTSPP*             pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pResultCb == NULL) || (pResult == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_Disconnect", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL )||
        (!AEE_CHECK_PTR( "OEMBTSPP_Disconnect", pResult, 
                         sizeof(BTResult), FALSE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( (pMe->streamID == SIO_NO_STREAM_ID) ||
        (AEEBTSPP_ST_CONNECTED != pMe->state) )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   CALLBACK_Cancel( pResultCb );

   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult );
   }

   // queue callback
   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;
   pMe->pResult            = pResult;

   MSG_MED( "IBT SPP disconnecting Stream; str ID 0x%x",pMe->streamID,0,0);

   pMe->currOp = OEMBTSPP_OP_DISCONNECT;

   sio_ioctl( pMe->streamID, SIO_IOCTL_BT_DISCONNECT, &param );

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPCLI_FindServersInit( BTHandle      hBT,
                                 uint32        uDuration,
                                 uint32        uMaxServers
                               )
{

   bt_cmd_status_type   stat;
   OEMBTSPP_DEVINFO*    pDiscDev;
   OEMBTSPP*            pMe = OEMBTSPP_GetObj( hBT );
   int                  retStat;

   if ( ( NULL == pMe ) || (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   // we check for OFF as it can be in any of the other two
   // states.We dont allow another client to do a service search
   // in case one is already on.
   if ( OEMBTSPP_SRCH_OFF != gSppDevDiscState )
   {
      MSG_ERROR( " Device Discovery still on \n",0, 0, 0);
      return EALREADY;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   pMe->role.cli.uNumDevFound = 0;

   // free the earlier Servers list
   while ( q_cnt( &pMe->role.cli.servQueue ) > 0 )
   {
      pDiscDev = (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.servQueue );

      if ( pDiscDev != NULL )
      {
         if ( NULL != pDiscDev->pRecs )
         {
            OEM_Free ( pDiscDev->pRecs );
         }
         OEM_Free ( pDiscDev );
      }
   }

   // clean up the earlier temp list if it is present
   while ( q_cnt( &pMe->role.cli.discDevQueue ) > 0 )
   {
      pDiscDev = (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.discDevQueue );

      if ( pDiscDev != NULL )
      {
         if ( NULL != pDiscDev->pRecs )
         {
            OEM_Free ( pDiscDev->pRecs );
         }
         OEM_Free ( pDiscDev );
      }
   }

   pMe->role.cli.uMaxNumDevReq  = uMaxServers;
   pMe->role.cli.uSrchSt        = OEMBTSPP_INQ_ON;
   gSppDevDiscState        = OEMBTSPP_INQ_ON;

   pMe->role.cli.uSearchTime    = uDuration;

   MSG_MED( "IBT SPP start inq timeout %x",uDuration/3,0,0);

   if ( uDuration != 0 )
   {
      IShell* pIShell = AEE_GetShell();
      // start the inquiry timer.
      ISHELL_SetTimer( pIShell,
                       (uDuration/3*1000),
                       OEMBTSPP_InquiryTimeout,
                       (void*)pMe
                     );
   }

   pMe->currOp = OEMBTSPP_OP_SRCH;

   stat = bt_cmd_sd_discover_devices( pMe->appId, BT_SERVICE_CLASS_ALL,
                                      NULL,
                                      MAX_SPP_SERVERS );

   retStat = OEMBTSPP_CheckCmdStatus( stat );

   if ( retStat != SUCCESS )
   {
      pMe->role.cli.uSrchSt   = OEMBTSPP_SRCH_OFF;
      gSppDevDiscState        = OEMBTSPP_SRCH_OFF;
      pMe->currOp             = OEMBTSPP_NO_OP;
   }

   return retStat;
}
/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_InquiryTimeout(void* pData)
{

   OEMBTSPP* pMe = (OEMBTSPP*)pData;

   MSG_MED( "IBT SPP start inq timeout occured st %x",
             pMe->role.cli.uSrchSt,0,0);

   if ( pMe->role.cli.uSrchSt == OEMBTSPP_INQ_ON )
   {
      bt_cmd_sd_stop_device_discovery( pMe->appId );
      pMe->role.cli.uSrchSt = OEMBTSPP_SRCH_ON;
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_SearchTimeout(void* pData)
{
   OEMBTSPP* pMe = (OEMBTSPP*)pData;

   MSG_MED( "IBT SPP start srch timeout... st %x",pMe->role.cli.uSrchSt,0,0);

   if ( pMe->role.cli.uSrchSt == OEMBTSPP_SRCH_OFF )
   {
      return ;
   }

   pMe->role.cli.uSrchSt = OEMBTSPP_CANCL_ON;

}
// once cancelled, the app should wait for the find serversnext api to 
// return size as zero.
/*======================================================================= 
=======================================================================*/
int OEMBTSPPCLI_CancelServerSearch( BTHandle      hBT,
                                    AEECallback*  pCb)
{
   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );
   IShell* pIShell = AEE_GetShell();

   if ( (NULL == pMe) ||
        (NULL == pCb) ||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_CancelServerSearch", pCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pCb->pfnNotify == NULL ) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( pMe->role.cli.uSrchSt == OEMBTSPP_SRCH_OFF )
   {
      // Free any servers that are in the list.
      OEMBTSPP_DEVINFO *pDevInfo;
      OEMBTSPP_CLIENT* cli =  &pMe->role.cli;
      
      while ( q_cnt( &pMe->role.cli.servQueue ) > 0 )
      {
         pDevInfo = 
         (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.servQueue );

         if ( NULL != pDevInfo->pRecs )
         {
            OEM_Free ( pDevInfo->pRecs );
         }
         OEM_Free ( pDevInfo );
      }

      while ( q_cnt( &pMe->role.cli.discDevQueue ) > 0 )
      {
         pDevInfo = 
         (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.discDevQueue );
         if ( NULL != pDevInfo->pRecs )
         {
            OEM_Free ( pDevInfo->pRecs );
         }
         OEM_Free ( pDevInfo );
      }

      
      // invoke app's callback, if any
      while ( cli->pcbDevDisc != NULL )
      {
         AEECallback* pCbDisc = cli->pcbDevDisc;

         if (cli->pDevInfoSize != NULL)
         {
            *cli->pDevInfoSize = 0;
            cli->pDevInfoSize  =  NULL;
         }
         
         cli->pcbDevDisc = cli->pcbDevDisc->pNext;
         AEE_APP_RESUME( pCbDisc, pMe->pac );
      }

      ISHELL_Resume( pIShell, pCb );
   }
   else
   {
      if ( pMe->role.cli.uSrchSt == OEMBTSPP_INQ_ON )
      {
         bt_cmd_sd_stop_device_discovery( pMe->appId );

         if ( pMe->role.cli.uSearchTime != 0 )
         {
            ISHELL_CancelTimer(pIShell,
                               OEMBTSPP_InquiryTimeout,
                               (void*)pMe);
         }
      }

      CALLBACK_Cancel( pCb );

      while ( pMe->pcbResult != NULL )
      {
         CALLBACK_Cancel( pMe->pcbResult );
      }
      // queue callback
      pCb->pCancelData  = &pMe->pcbResult;
      pCb->pfnCancel    = OEMBTSPP_Cancel;
      pCb->pNext        = pMe->pcbResult;
      pMe->pcbResult    = pCb;

      // use the same flag which will be checkd at various 
      // points during the server search.
      if ( pMe->role.cli.uSearchTime != 0 )
      {
         ISHELL_CancelTimer(pIShell,
                            OEMBTSPP_SearchTimeout,
                            (void*)pMe);
      }

      pMe->role.cli.uSrchSt = OEMBTSPP_CANCL_ON;
      pMe->currOp = OEMBTSPP_OP_CANCEL;
   }
   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPCLI_FindServersNext( BTHandle      hBT,
                                 uint32*       pSize,
                                 AEECallback*  pCb,
                                 IShell*       pIShell )
{

   OEMBTSPP* pMe = OEMBTSPP_GetObj( hBT );

   if ( (NULL == pMe) || (NULL == pCb) || (NULL == pSize) ||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_FindServersNext", pCb, 
                         sizeof(AEECallback), TRUE )) ||
        (NULL == pCb->pfnNotify )||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_FindServersNext", pSize, 
                         sizeof(uint32), FALSE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }


   CALLBACK_Cancel( pCb );

   if ( q_cnt( &pMe->role.cli.servQueue ) > 0 )
   {
      OEMBTSPP_DEVINFO* pServerInfo = 
      (OEMBTSPP_DEVINFO*) q_check( &pMe->role.cli.servQueue );

      *pSize = sizeof( BTSPPDeviceInfo ) + 
               ( (pServerInfo->uNumSPPServices - 1) 
                 * sizeof( BTSPPServiceRecord) );

      ISHELL_Resume( pIShell, pCb );
   }
   else if ( pMe->role.cli.uSrchSt == OEMBTSPP_SRCH_OFF )
   {
      // not expecting to find any more servers

      *pSize = 0;
      ISHELL_Resume( pIShell, pCb );
   }
   else
   {
      while ( pMe->role.cli.pcbDevDisc != NULL )
      {
         CALLBACK_Cancel( pMe->role.cli.pcbDevDisc );
      }
      // still searching for devices. will call the cb once completed
      pCb->pCancelData              = &pMe->role.cli.pcbDevDisc;
      pCb->pfnCancel                = OEMBTSPP_Cancel;
      pCb->pNext                    = pMe->role.cli.pcbDevDisc;
      pMe->role.cli.pcbDevDisc      = pCb;
      pMe->role.cli.pDevInfoSize    = pSize;
   }

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPCLI_GetServerInfo( BTHandle hBT, BTSPPDeviceInfo* pServerInfo )
{

   uint32            size = 0;
   OEMBTSPP*         pMe  = OEMBTSPP_GetObj( hBT );
   OEMBTSPP_DEVINFO* pSrc = NULL;

   if ( NULL == pMe )
   {
      return EBADPARM;
   }

   pSrc = (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.servQueue );

   if (NULL == pSrc)
   {
      return EBADITEM;
   }

   size = sizeof( BTSPPDeviceInfo ) + 
          ( (pSrc->uNumSPPServices - 1) * sizeof( BTSPPServiceRecord) );

   if ( (NULL == pServerInfo) || 
        !AEE_CHECK_PTR( "OEMBTSPPCLI_GetServerInfo", pServerInfo, size, TRUE ) )
   {
      return EBADPARM;
   }

   // copy data
   pServerInfo->bdi             = pSrc->bdi;
   pServerInfo->uNumSPPServices = pSrc->uNumSPPServices;
   MEMCPY( pServerInfo->serviceRecords, pSrc->pRecs,
           pSrc->uNumSPPServices * sizeof(BTSPPServiceRecord) );

   // free source
   OEM_Free ( pSrc->pRecs );
   OEM_Free ( pSrc );

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPPCLI_Connect( BTHandle            hBT,
                         const BDAddress*    pBDAddr,
                         uint32              uServerChannel,
                         BTResult*           pResult,
                         AEECallback*        pResultCb)
{

   sio_open_type     so;
   bt_spp_open_type  bso;
   OEMBTSPP*         pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (pResultCb == NULL) || (pResult == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_Connect", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_Connect", pResult, 
                         sizeof(BTResult), TRUE )) ||
        (!AEE_CHECK_PTR( "OEMBTSPPCLI_Connect", pBDAddr, 
                         sizeof(BDAddress), FALSE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( pMe->streamID != SIO_NO_STREAM_ID )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   pMe->bdAddr = *pBDAddr;

   CALLBACK_Cancel( pResultCb );

   while ( pMe->pcbResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbResult );
   }

   // queue callback
   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;
   pMe->pResult            = pResult;

   
   // The app may use the same object to connect to the 
   // second server.Since we do not clear the Rx buffer
   // when disconnected, we clear the water mark if the app
   // has not read the previous data.
   dsm_empty_queue( &pMe->txWaterMark );
   dsm_empty_queue( &pMe->rxWaterMark );
   dsm_free_packet( &pMe->pDsmItem );

   OEMBTSPP_InitStream(pMe,&so,&bso);

   bso.bd_addr_ptr           = (bt_bd_addr_type*)pBDAddr;
   bso.client_app            = TRUE ;

   if (0 ==  uServerChannel)
   {
      // as we do not specify the rcn 
      bso.service_uuid          = BT_SD_SERVICE_CLASS_SERIAL_PORT;
      bso.rc_server_channel     = BT_SPP_SCN_UNSPECIFIED;

   }
   else
   {
      bso.service_uuid          = BT_SD_INVALID_UUID;// as we specify the rcn 
      bso.rc_server_channel     = (bt_spp_scn_enum_type)uServerChannel;
   }

   bso.service_version       = 0;
   bso.service_name_str_ptr  = NULL;

   MSG_MED( "IBT SPP connecting to server. SCN 0x%x",uServerChannel,0,0);
   BT_BDA( MSG_API, "IBT SPP Server", (bt_bd_addr_type*)pBDAddr );

   pMe->currOp  = OEMBTSPP_OP_CONNECT;

   rex_enter_crit_sect( &gSppCritSect );
   pMe->streamID = SIO_MAX_STREAM; // indicates opening
   pMe->streamID = sio_open( &so );
   rex_leave_crit_sect( &gSppCritSect );

   if ( pMe->streamID == SIO_NO_STREAM_ID )
   {
      pMe->currOp = OEMBTSPP_NO_OP;
   }
   return( pMe->streamID == SIO_NO_STREAM_ID ) ? EFAILED : SUCCESS;
}



/*======================================================================= 
=======================================================================*/
int OEMBTSPP_GetStatus( BTHandle hBT, BTSPPStatus* pStatus )
{
   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (NULL == pStatus) ||
        (!AEE_CHECK_PTR( "OEMBTSPPSRV_GetStatus", pStatus, 
                         sizeof(BTSPPStatus), TRUE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   MSG_MED( " IBT SPP Get St 0x%x, str id 0x%x \n",
              pMe->state, pMe->streamID, 0);

   pStatus->BDAddr   = pMe->bdAddr; 
   pStatus->streamID = pMe->streamID;
   pStatus->state    = pMe->state; 
   pStatus->uChannel = pMe->uChannelNumber;

   if ( pMe->state != AEEBTSPP_ST_CLOSED )
   {
      pStatus->result   = pMe->result;
   }
   else
   {
      pStatus->result   = pMe->closeReason;
   }

   pMe->statusChanged &= ~OEMBTSPP_STATE_CHANGE;


   MSG_MED( "IBT SPP Reading SPP State. st = 0x%x,"
            "changed = 0x%x",pStatus->state,pMe->statusChanged,0);

   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
int OEMBTSPP_GetModemStatus( BTHandle hBT, BTSPPModemStatus* pStatus)
{

   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (NULL == pStatus) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_GetModemStatus", pStatus, 
                         sizeof(BTSPPModemStatus), TRUE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   *pStatus                      = pMe->modemStatus;

   // clear the changed mask but keep the current modem status
   pMe->modemStatus.changedMask  = 0;
   pMe->modemStatus.breaks       = 0;
   pMe->modemStatus.breakLength  = 0;
   pMe->modemStatus.breakPresent = 0;
   pMe->modemStatus.status      &= ~AEEBTSPP_MS_TX_FLUSHED_B;
   pMe->statusChanged           &= ~OEMBTSPP_MODEM_CHANGE; 

   MSG_MED( "IBT SPP Get Mod  CM 0x%x, MS 0x%x , stscg %x",
            pStatus->changedMask,pStatus->status,pMe->statusChanged);

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPP_GetLineError( BTHandle hBT, BTSPPLineError* pLineError)
{

   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   if ( (pMe == NULL) || (NULL == pLineError) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_GetLineError", pLineError, 
                         sizeof(BTSPPLineError), TRUE )) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   *pLineError         = pMe->lineError;
   pMe->statusChanged &= ~OEMBTSPP_LINE_ERROR; 

   // once the line error status has been read clear the contents.
   pMe->lineError.lineError      = 0;
   pMe->lineError.uFramingErrors = 0;
   pMe->lineError.uOverRuns      = 0;
   pMe->lineError.uParityErrors  = 0;

   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
int OEMBTSPP_Configure( BTHandle hBT, const BTSPPConfig* pConfig,
                        BTResult* pResult, AEECallback* pResultCb )
{

   sio_ioctl_param_type  param;
   bt_spp_config_type    sppCfg;

   OEMBTSPP*  pMe = OEMBTSPP_GetObj( hBT );

   MSG_MED( "IBT SPP Send Config cmd ",0,0,0);

   if ( (pMe == NULL) || (NULL == pConfig) || (NULL == pResult) ||
        (NULL == pResultCb) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_Configure", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_Configure", pConfig, 
                         sizeof(BTSPPConfig), FALSE)) ||
        (pMe->pac != AEE_GetAppContext()) )
   {
      return EBADPARM;
   }

   if ( pMe->streamID == SIO_NO_STREAM_ID )
   {
      return EBADSTATE;
   }

   if ( pMe->currOp != OEMBTSPP_NO_OP )
   {
      return EALREADY;
   }

   CALLBACK_Cancel( pResultCb );

   while ( pMe->pcbIoctlResult != NULL )
   {
      CALLBACK_Cancel( pMe->pcbIoctlResult );
   }

   // queue callback
   pResultCb->pCancelData  = &pMe->pcbIoctlResult;
   pResultCb->pfnCancel    = OEMBTSPP_Cancel;
   pResultCb->pNext        = pMe->pcbIoctlResult;
   pMe->pcbIoctlResult     = pResultCb;

   pMe->pResult            = pResult;
   pMe->pConfig            = ( BTSPPConfig* ) pConfig;

   sppCfg.baudrate       = pConfig->baudRate; 
   sppCfg.format         = pConfig->format;
   sppCfg.flow_ctrl      = pConfig->flowCtrl;
   sppCfg.xon_char       = pConfig->uXonChar;
   sppCfg.xoff_char      = pConfig->uXoffChar;
   sppCfg.max_frame_size = BT_SPP_MFS_AUTOMATIC;

   param.bt_spp_config_ptr = &sppCfg;

   pMe->currOp = OEMBTSPP_OP_CONFIGURE;

   sio_ioctl( pMe->streamID, SIO_IOCTL_BT_CONFIGURE , &param );

   return SUCCESS;

}

/*======================================================================= 
=======================================================================*/
int OEMBTSPP_IOCtlEx( BTHandle            hBT,
                      BTSPPIOCtlCommand   cmd,
                      BTSPPModemStatus*   pModemStatus, 
                      AEECallback*        pResultCb,
                      IShell*             pIShell )
{
   
   sio_ioctl_cmd_type   ioctl_cmd = SIO_IOCTL_BT_GET_STATUS;// init to avoid compiler warning
   sio_ioctl_param_type param;
   OEMBTSPP*            pMe = OEMBTSPP_GetObj( hBT );
   int                  retVal = SUCCESS;
   boolean              bSendIoctl = TRUE; 

   if ( (pMe == NULL) || (NULL == pModemStatus) || (NULL == pResultCb) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_IOCtlEx", pModemStatus, 
                         sizeof(BTSPPModemStatus), TRUE )) ||
        (!AEE_CHECK_PTR( "OEMBTSPP_IOCtlEx", pResultCb, 
                         sizeof(AEECallback), TRUE )) ||
        (pResultCb->pfnNotify == NULL) ||
        (pMe->pac != AEE_GetAppContext()))
   {
      return EBADPARM;
   }

   if ( (pMe->streamID == SIO_NO_STREAM_ID ) &&
        ( cmd != AEEBTSPP_IOCTL_SET_DTE ) && 
        ( cmd != AEEBTSPP_IOCTL_SET_DCE) )
   {
      return EBADSTATE;
   }

   CALLBACK_Cancel( pResultCb );

   switch ( cmd )
   {
   case AEEBTSPP_IOCTL_INBOUND_FLOW_ENABLE:
      ioctl_cmd = SIO_IOCTL_INBOUND_FLOW_ENABLE; 
      break;

   case AEEBTSPP_IOCTL_INBOUND_FLOW_DISABLE:
      ioctl_cmd = SIO_IOCTL_INBOUND_FLOW_DISABLE; 
      break;

   case AEEBTSPP_IOCTL_FLUSH_TX:
      ioctl_cmd = SIO_IOCTL_FLUSH_TX;
      param.record_flush_func_ptr = flush_tx_func_table[ pMe->uIndex ]; 

      while ( pMe->pcbFlushTx != NULL )
      {
         CALLBACK_Cancel( pMe->pcbFlushTx );
      }

      // queue callback
      pResultCb->pCancelData  = &pMe->pcbFlushTx;
      pResultCb->pfnCancel    = OEMBTSPP_Cancel;
      pResultCb->pNext        = pMe->pcbFlushTx;
      pMe->pcbFlushTx         = pResultCb;
      pResultCb->pmc          = pModemStatus;

      break;

   case AEEBTSPP_IOCTL_CD_ASSERT:
      if ( pMe->bDCE == FALSE )
      {
         retVal = EBADPARM;
      }
      else
      {
         pMe->modemStatus.status |= AEEBTSPP_MS_CD_ASSERTED_B;
         ioctl_cmd = SIO_IOCTL_CD_ASSERT;
      }
      break;

   case AEEBTSPP_IOCTL_CD_DEASSERT:
      if ( pMe->bDCE == FALSE )
      {
         retVal = EBADPARM;
      }
      else
      {
         ioctl_cmd = SIO_IOCTL_CD_DEASSERT;
         pMe->modemStatus.status &= ~AEEBTSPP_MS_CD_ASSERTED_B;
      }
      break;

   case AEEBTSPP_IOCTL_RI_ASSERT:
      if ( pMe->bDCE == FALSE )
      {
         retVal = EBADPARM;
      }
      else
      {
         pMe->modemStatus.status |= AEEBTSPP_MS_RI_ASSERTED_B;
         ioctl_cmd = SIO_IOCTL_RI_ASSERT;
      }
      break;

   case AEEBTSPP_IOCTL_RI_DEASSERT:
      if ( pMe->bDCE == FALSE )
      {
         retVal = EBADPARM;
      }
      else
      {
         ioctl_cmd = SIO_IOCTL_RI_DEASSERT;
         pMe->modemStatus.status &= ~AEEBTSPP_MS_RI_ASSERTED_B;
      }
      break;

   case AEEBTSPP_IOCTL_DSR_ASSERT:
      pMe->modemStatus.status = ( pMe->bDCE != FALSE) ?
      ( pMe->modemStatus.status | AEEBTSPP_MS_DSR_ASSERTED_B ) :
      ( pMe->modemStatus.status | AEEBTSPP_MS_DTR_ASSERTED_B );
      
      ioctl_cmd = SIO_IOCTL_DSR_ASSERT;
      break;

   case AEEBTSPP_IOCTL_DSR_DEASSERT:
      pMe->modemStatus.status = ( pMe->bDCE != FALSE) ?
      ( pMe->modemStatus.status & ~AEEBTSPP_MS_DSR_ASSERTED_B ) :
      ( pMe->modemStatus.status & ~AEEBTSPP_MS_DTR_ASSERTED_B );
      ioctl_cmd = SIO_IOCTL_DSR_DEASSERT;
      break;

   case AEEBTSPP_IOCTL_ENABLE_DTR_EVENT:
      ioctl_cmd = SIO_IOCTL_ENABLE_DTR_EVENT;
      param.enable_dte_ready_event = enable_dtr_func_table[ pMe->uIndex ]; 
      break;

   case AEEBTSPP_IOCTL_DISABLE_DTR_EVENT:
      ioctl_cmd = SIO_IOCTL_DISABLE_DTR_EVENT;
      break;

   case AEEBTSPP_IOCTL_GET_DTR:
   case AEEBTSPP_IOCTL_GET_RTS:
      bSendIoctl = FALSE;

      break;
   case AEEBTSPP_IOCTL_SET_DTE:
      if ( pMe->bDCE != FALSE )
      {
         BTSPPModemStatus prevStat = pMe->modemStatus;

         //DCE->DTE
         pMe->modemStatus.changedMask = 0;
         pMe->modemStatus.status = 0;

         pMe->modemStatus.status =  (prevStat.status &
                                     AEEBTSPP_MS_DTR_ASSERTED_B )?
                                    (pMe->modemStatus.status |
                                     AEEBTSPP_MS_DSR_ASSERTED_B ):
                                     pMe->modemStatus.status;

         pMe->modemStatus.status =  (prevStat.status &
                                     AEEBTSPP_MS_RTS_ASSERTED_B )?
                                    (pMe->modemStatus.status |
                                     AEEBTSPP_MS_CTS_ASSERTED_B ):
                                     pMe->modemStatus.status;

         pMe->modemStatus.changedMask =  (prevStat.changedMask &
                                     AEEBTSPP_MS_DTR_ASSERTED_B )?
                                    (pMe->modemStatus.changedMask |
                                     AEEBTSPP_MS_DSR_ASSERTED_B ):
                                     pMe->modemStatus.changedMask;

         pMe->modemStatus.status =  (prevStat.changedMask &
                                     AEEBTSPP_MS_RTS_ASSERTED_B )?
                                    (pMe->modemStatus.changedMask |
                                     AEEBTSPP_MS_CTS_ASSERTED_B ):
                                     pMe->modemStatus.changedMask;

         pMe->bDCE = FALSE;
      }

      bSendIoctl = FALSE;
      break;
   case AEEBTSPP_IOCTL_SET_DCE:
      if ( pMe->bDCE != TRUE )
      {
         BTSPPModemStatus prevStat = pMe->modemStatus;

         pMe->modemStatus.changedMask = 0;
         pMe->modemStatus.status = 0;
         //DTE->DCE
         pMe->modemStatus.status =  (prevStat.status &
                                     AEEBTSPP_MS_DSR_ASSERTED_B )?
                                    (pMe->modemStatus.status |
                                     AEEBTSPP_MS_DTR_ASSERTED_B ):
                                     pMe->modemStatus.status;

         pMe->modemStatus.status =  (prevStat.status &
                                     AEEBTSPP_MS_CTS_ASSERTED_B )?
                                    (pMe->modemStatus.status |
                                     AEEBTSPP_MS_RTS_ASSERTED_B ):
                                     pMe->modemStatus.status;

         pMe->modemStatus.changedMask =  (prevStat.changedMask &
                                     AEEBTSPP_MS_DSR_ASSERTED_B )?
                                    (pMe->modemStatus.changedMask |
                                     AEEBTSPP_MS_DTR_ASSERTED_B ):
                                     pMe->modemStatus.changedMask;

         pMe->modemStatus.status =  (prevStat.changedMask &
                                     AEEBTSPP_MS_CTS_ASSERTED_B )?
                                    (pMe->modemStatus.changedMask |
                                     AEEBTSPP_MS_RTS_ASSERTED_B ):
                                     pMe->modemStatus.changedMask;

         pMe->bDCE = TRUE;
      }
      bSendIoctl = FALSE;
      break;

   default:
      return EBADPARM;
   }

   MSG_MED( "IBT SPP Send Ioctl cmd 0x%x ", cmd,0,0);

   if ( ( bSendIoctl != FALSE ) && (retVal == SUCCESS ))
   {
      sio_ioctl( pMe->streamID, ioctl_cmd, &param );
   }

   if (retVal == SUCCESS)
   {
      if ( AEEBTSPP_IOCTL_FLUSH_TX != cmd )
      {
         *pModemStatus = pMe->modemStatus;
         ISHELL_Resume( pIShell, pResultCb );
      }
   }
   return retVal;
}

/*======================================================================= 
=======================================================================*/



///////////////////////////////////////////////////////////////////////
// Event handler functions:
///////////////////////////////////////////////////////////////////////
static void OEMBTSPP_EvFlushTtx( bt_ev_msg_type* bt_ev_ptr )
{

   OEMBTSPP* pMe = 
   OEMBTSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_flush_tx.stream_id );


   MSG_MED( "IBT SPP Ev Flush Ttx ", 0,0,0);


   if ( NULL != pMe )
   {
      pMe->modemStatus.streamID     = pMe->streamID;

      while ( pMe->pcbFlushTx != NULL )
      {
         AEECallback* pCb = pMe->pcbFlushTx;
         
         pMe->pcbFlushTx = pMe->pcbFlushTx->pNext;

         if ( pCb->pmc != NULL )
         {
            BTSPPModemStatus  modStat;

            modStat = pMe->modemStatus;
            modStat.status      |= AEEBTSPP_MS_TX_FLUSHED_B;
            modStat.changedMask |= AEEBTSPP_MS_TX_FLUSHED_B;
            * ((BTSPPModemStatus*)pCb->pmc) = modStat;
         }
         AEE_APP_RESUME( pCb, pMe->pac );
      }
   }
}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvEnableDtr( bt_ev_msg_type* bt_ev_ptr )
{

   boolean               dtr_asserted;
   sio_ioctl_param_type  param;
   OEMBTSPP*             pMe = 
   OEMBTSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_enable_dtr.stream_id );
   BTSPPModemStatusBitmap prevStat;

   if ( NULL != pMe )
   {
      MSG_MED( "IBT SPP Ev Enab Dtr ", 0,0,0);

      prevStat = pMe->modemStatus.status;
      dtr_asserted = (pMe->modemStatus.status & AEEBTSPP_MS_DTR_ASSERTED_B)
                      ? TRUE : FALSE;

      param.dte_ready_asserted = &dtr_asserted;
      sio_ioctl( pMe->streamID, SIO_IOCTL_DTE_READY_ASSERTED, &param );

      if ( dtr_asserted )
      {
         pMe->modemStatus.status |= AEEBTSPP_MS_DTR_ASSERTED_B;
      }
      else
      {
         pMe->modemStatus.status &= ~AEEBTSPP_MS_DTR_ASSERTED_B;
      }

      pMe->modemStatus.streamID   = pMe->streamID;

      if ( prevStat != pMe->modemStatus.status )
      {
         pMe->statusChanged         |= OEMBTSPP_MODEM_CHANGE;

         while ( pMe->pcbModemChange != NULL )
         {
            AEECallback* pCb = pMe->pcbModemChange;
            pMe->pcbModemChange = pMe->pcbModemChange->pNext;
            AEE_APP_RESUME( pCb, pMe->pac );
         }
      }
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvTxLoWm( bt_ev_msg_type* bt_ev_ptr )
{

   OEMBTSPP  *pMe = 
   OEMBTSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_tx_lo_wm.stream_id );

   MSG_MED( "IBT SPP Ev TxLoWm str id 0x%x indx 0x%x ",
            pMe->streamID,pMe->uIndex,0);

   if ( (NULL != pMe) && (pMe->state == BT_SPP_ST_CONNECTED) )
   {
      pMe->bOKToSend = TRUE;

      while ( pMe->pcbWrite != NULL )
      {
         AEECallback* pCb = pMe->pcbWrite;

         MSG_MED( "IBT SPP  Call wrtble cb str id 0x%x indx 0x%x ",
                  pMe->streamID,pMe->uIndex,0);
         pMe->pcbWrite = pMe->pcbWrite->pNext;
         AEE_APP_RESUME( pCb, pMe->pac );

      }
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvRxGne( bt_ev_msg_type* bt_ev_ptr )
{

   OEMBTSPP *pMe = OEMBTSPP_FindMe( bt_ev_ptr->ev_msg.ev_spp_rx_gne.stream_id );

   if ( pMe != NULL )
   {
      MSG_LOW( "IBT SPP Ev RxGne strm id "
                "0x%x indx 0x%x ",pMe->streamID,pMe->uIndex,0);

      while ( pMe->pcbRead != NULL )
      {

         AEECallback* pCb = pMe->pcbRead;

         MSG_MED( "IBT SPP Ev RxGne Call rdble cb strm id "
                  "0x%x indx 0x%x ",pMe->streamID,pMe->uIndex,0);

         pMe->pcbRead = pMe->pcbRead->pNext;

         AEE_APP_RESUME( pCb, pMe->pac );
      }
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvLineError( bt_ev_msg_type* bt_ev_ptr )
{

   bt_spp_le_type* line_error_ptr =
   &bt_ev_ptr->ev_msg.ev_spp_line_error.line_error;
   OEMBTSPP* pMe = OEMBTSPP_FindMe( line_error_ptr->stream_id );

   if ( NULL == pMe )
   {
      return;
   }

   pMe->lineError.streamID = pMe->streamID;

   switch ( line_error_ptr->cur_line_error )
   {
   case BT_SPP_LE_NONE:
      pMe->lineError.lineError      = AEEBTSPP_LE_NONE;
      break;
   case BT_SPP_LE_OVERRUN_ERROR:
      pMe->lineError.lineError      = AEEBTSPP_LE_OVERRUN_ERROR;
      pMe->lineError.uOverRuns      = line_error_ptr->overruns;
      break;
   case BT_SPP_LE_PARITY_ERROR:
      pMe->lineError.lineError      = AEEBTSPP_LE_PARITY_ERROR;
      pMe->lineError.uParityErrors  = line_error_ptr->parity_errors;
      break;
   case BT_SPP_LE_FRAMING_ERROR:
      pMe->lineError.lineError      = AEEBTSPP_LE_FRAMING_ERROR;
      pMe->lineError.uFramingErrors = line_error_ptr->framing_errors;
      break;
   default:
      return;
   }

   pMe->statusChanged |= OEMBTSPP_LINE_ERROR;

   while ( pMe->pcbLineError != NULL )
   {
      AEECallback* pCb = pMe->pcbLineError;
      pMe->pcbLineError = pMe->pcbLineError->pNext;
      AEE_APP_RESUME( pCb, pMe->pac );
   }

}



////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

//  AEEBTSPP_MS_DSR_ASSERTED_B         0x01 BT_SPP_MS_RTC_ASSERTED_B (0x04) (DTE)
//  AEEBTSPP_MS_CTS_ASSERTED_B         0x02 BT_SPP_MS_RTR_ASSERTED_B (0x08) (DTE)

//  AEEBTSPP_MS_RI_ASSERTED_B   (IC)   0x04  BT_SPP_MS_IC_ASSERTED_B  (0x40)
//  AEEBTSPP_MS_CD_ASSERTED_B   (DV)   0x08  BT_SPP_MS_DV_ASSERTED_B  (0x80) 

//  AEEBTSPP_MS_RTS_ASSERTED_B         0x10  BT_SPP_MS_RTR_ASSERTED_B (0x08) (DCE)
//  AEEBTSPP_MS_DTR_ASSERTED_B         0x20  BT_SPP_MS_RTC_ASSERTED_B (0x04) (DCE)
//  AEEBTSPP_MS_TX_FLUSHED_B           0x40  

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
static void OEMBTSPP_EvModemStatus( bt_ev_msg_type* bt_ev_ptr )
{

   bt_spp_ms_type*  pNewModemStatus = 
   &bt_ev_ptr->ev_msg.ev_spp_modem_status.modem_status;
   BTSPPModemStatus* pModemStat = NULL;

   OEMBTSPP* pMe = OEMBTSPP_FindMe( pNewModemStatus->stream_id );
   
   if ( NULL == pMe )
   {
      return;
   }

   MSG_HIGH( "IBT SPP Ev new Mod stat - str id 0x%x cm 0x%x ms 0x%x", \
             pMe->streamID,pNewModemStatus->ms_changed_mask,pNewModemStatus->modem_status);


   pModemStat = &pMe->modemStatus;
   pMe->modemStatus.streamID    = pMe->streamID;

   //  Map to the bit mask in bt.h
   if ( pNewModemStatus->ms_changed_mask & BT_SPP_MS_RTC_ASSERTED_B )
   {
      if ( pMe->bDCE )
      {
         pModemStat->changedMask |= AEEBTSPP_MS_DTR_ASSERTED_B;

         pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_RTC_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_DTR_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_DTR_ASSERTED_B );
      }
      else
      {
         pModemStat->changedMask |= AEEBTSPP_MS_DSR_ASSERTED_B;
         pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_RTC_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_DSR_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_DSR_ASSERTED_B );
      }
   }

   if ( pNewModemStatus->ms_changed_mask & BT_SPP_MS_RTR_ASSERTED_B )
   {
      if ( pMe->bDCE )
      {
           pModemStat->changedMask |= AEEBTSPP_MS_RTS_ASSERTED_B;

           pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_RTR_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_RTS_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_RTS_ASSERTED_B );
      }
      else
      {
         pModemStat->changedMask |= AEEBTSPP_MS_CTS_ASSERTED_B;
         pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_RTR_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_CTS_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_CTS_ASSERTED_B );

      }
   }

   if ( pNewModemStatus->ms_changed_mask & BT_SPP_MS_IC_ASSERTED_B )
   {
      if( !pMe->bDCE )
      {
         pModemStat->changedMask |= AEEBTSPP_MS_DSR_ASSERTED_B;
         pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_IC_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_RI_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_RI_ASSERTED_B );
      }
      
   }
   if ( pNewModemStatus->ms_changed_mask & BT_SPP_MS_DV_ASSERTED_B )
   {
      if( !pMe->bDCE )
      {
         pModemStat->changedMask |= AEEBTSPP_MS_CD_ASSERTED_B;
         pModemStat->status = 
            ( pNewModemStatus->modem_status & BT_SPP_MS_DV_ASSERTED_B ) ? 
            ( pModemStat->status | AEEBTSPP_MS_CD_ASSERTED_B ) :
            ( pModemStat->status & ~AEEBTSPP_MS_CD_ASSERTED_B );
      }
      
   }
   pMe->modemStatus.breakPresent = pNewModemStatus->break_present;
   pMe->modemStatus.breakLength  = pNewModemStatus->break_length;
   pMe->modemStatus.breaks       = pNewModemStatus->breaks;


   if ( pModemStat->changedMask ||
        pModemStat->breakPresent )
   {
      pMe->statusChanged |= OEMBTSPP_MODEM_CHANGE;

      while ( pMe->pcbModemChange != NULL )
      {
         AEECallback* pCb = pMe->pcbModemChange;
         pMe->pcbModemChange = pMe->pcbModemChange->pNext;
         AEE_APP_RESUME( pCb, pMe->pac );
      }
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvConfigChange( bt_ev_msg_type* bt_ev_ptr )
{

   bt_spp_cfg_rpt_type* config_ptr =
   &bt_ev_ptr->ev_msg.ev_spp_config_change.config_change;
   OEMBTSPP* pMe = OEMBTSPP_FindMe( config_ptr->stream_id );

   MSG_MED( "IBT SPP Ev config chgd - strm id 0x%x ", 
            pMe->streamID,0,0);

   if ( NULL != pMe )
   {
      pMe->config.baudRate      = config_ptr->spp_config.baudrate;
      pMe->config.format        = config_ptr->spp_config.format;
      pMe->config.flowCtrl      = config_ptr->spp_config.flow_ctrl;
      pMe->config.uXonChar      = config_ptr->spp_config.xon_char;
      pMe->config.uXoffChar     = config_ptr->spp_config.xoff_char;

      if ( pMe->currOp == OEMBTSPP_OP_CONFIGURE )
      {
         while ( (pMe->pcbIoctlResult != NULL) &&
                 (pMe->pConfig != NULL) &&
                 (pMe->pResult != NULL) )
         {
            // we had initiated the config change.
            AEECallback* pCb = pMe->pcbIoctlResult;

            if ( (pMe->config.baudRate == pMe->pConfig->baudRate )&&
                 (pMe->config.format   == pMe->pConfig->format )&&
                 (pMe->config.flowCtrl == pMe->pConfig->flowCtrl )&&
                 (pMe->config.uXonChar == pMe->pConfig->uXonChar )&&
                 (pMe->config.uXoffChar == pMe->pConfig->uXoffChar ))
            {
               *pMe->pResult = AEEBT_RESULT_SUCCESS;
            }
            else
            {
               // copy the actul configuration 
               *pMe->pResult = AEEBT_RESULT_NEGOTIATION_FAILURE;
               *pMe->pConfig = pMe->config;
            }

            pMe->pcbIoctlResult = NULL;
            pMe->pConfig   = NULL;
            pMe->pResult   = NULL;
            AEE_APP_RESUME( pCb, pMe->pac );
         }
         pMe->currOp = OEMBTSPP_NO_OP;
      }

      MSG_MED( "IBT SPP Config br 0x%x, fr 0x%x, fc 0x%x ", 
               pMe->config.baudRate,pMe->config.format,pMe->config.flowCtrl);

      MSG_MED( "IBT SPP Config xon 0x%x, xof 0x%x ", 
               pMe->config.uXoffChar,pMe->config.uXonChar,0);
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_RmEvHndlr( OEMBTSPP* pMe, bt_ev_msg_type* msg_ptr )
{

   bt_ev_rm_authorize_request_type* ev_ptr = &msg_ptr->ev_msg.ev_rm_atzrq;
   int retStat;

   if ( pMe->bClientApp != FALSE )
   {
      return;
   }

   if ( ( BT_SIM_RFCOMM_SCN == ev_ptr->service_id.service_id_method ) &&
        ( ev_ptr->service_id.rfcomm_scn == pMe->uChannelNumber)
      )
   {
      if ( ( NULL == pMe->role.serv.pEvData ) || 
           ( NULL == pMe->role.serv.pcbAuth ) )
      {
         // accept the connection by default.
         bt_service_id_type  service_ident;

         service_ident.service_id_method = BT_SIM_RFCOMM_SCN;
         service_ident.rfcomm_scn        = pMe->uChannelNumber;

         if ( pMe->currOp == OEMBTSPP_NO_OP )
         {
            bt_cmd_status_type  stat;

            stat = bt_cmd_rm_authorize_reply( pMe->appId, &service_ident,
                                              &ev_ptr->bd_addr, TRUE );

            retStat = OEMBTSPP_CheckCmdStatus( stat );

            if ( retStat == SUCCESS )
            {
               pMe->currOp = OEMBTSPP_OP_AUTHORIZE;
            }
         }
      }
      else
      {
         pMe->role.serv.pEvData->connEv.bdAddr = *((BDAddress*)&ev_ptr->bd_addr);
         pMe->role.serv.pEvData->connEv.uuid =   pMe->role.serv.uuid;

         AEE_APP_RESUME( pMe->role.serv.pcbAuth, pMe->pac );
         pMe->role.serv.pcbAuth   = NULL;
         pMe->role.serv.pEvData   = NULL; 
      }
   }
}




/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EvStatusChange( bt_ev_msg_type* bt_ev_ptr )
{

   bt_spp_status_type* pStatus =
   &bt_ev_ptr->ev_msg.ev_spp_status_change.status_change;
   OEMBTSPP*  pMe = OEMBTSPP_FindMe( pStatus->stream_id );

   if ( NULL != pMe )
   {
      MSG_MED( "IBT SPP Ev St change ind 0x%x st 0x%x r 0x%x", 
               pMe->uIndex, pStatus->spp_state, pStatus->spp_reason );


      MSG_MED( "IBT SPP Ev St Change str id 0x%x ", 
               pMe->streamID,0,0 );

      pMe->bdAddr = *((BDAddress*)&pStatus->bd_addr);

      pMe->streamID           = pStatus->stream_id;
      pMe->bClientApp         = pStatus->client_app;

      pMe->uChannelNumber     = pStatus->rc_server_channel;

      pMe->result
      = OEMBTSPP_ConvertBTReason( pStatus->spp_reason );

      switch ( pStatus->spp_state )
      {
      case BT_SPP_ST_CLOSED:
         pMe->streamID = SIO_NO_STREAM_ID;
         pMe->bOKToSend = FALSE;
         dsm_empty_queue( &pMe->txWaterMark );
         // notify the result of failure 
         // of connect or enable
         // if disable is called, then return success

         if ( (pMe->currOp == OEMBTSPP_OP_DISABLE ) ||
              ( ( pMe->currOp == OEMBTSPP_OP_DISCONNECT ) ))
         {
            if (( NULL != pMe->pcbResult ) && ( NULL != pMe->pResult ))
            {
               *pMe->pResult    = AEEBT_RESULT_SUCCESS;
               AEE_ResumeCallback( pMe->pcbResult, pMe->pac );
               pMe->pcbResult = NULL;
               pMe->pResult   = NULL;
            }
         }
         else if ( (pMe->currOp == OEMBTSPP_OP_CONNECT ) ||
                   (pMe->currOp == OEMBTSPP_OP_ENABLE) )
         {
            if (( NULL != pMe->pcbResult ) && ( NULL != pMe->pResult ))
            {
               *pMe->pResult    = pMe->closeReason;
               AEE_ResumeCallback( pMe->pcbResult, pMe->pac );
               pMe->pcbResult = NULL;
               pMe->pResult   = NULL;
            }
         }

         while ( pMe->pcbFlushTx != NULL )
         {
            AEECallback* pCb = pMe->pcbFlushTx;
            pMe->pcbFlushTx = pMe->pcbFlushTx->pNext;

            if ( pCb->pmc != NULL )
            {
               BTSPPModemStatus* pModStat = pCb->pmc;
               *pModStat = pMe->modemStatus;
               pModStat->status |= AEEBTSPP_MS_TX_FLUSHED_B;
            }
            AEE_APP_RESUME( pCb, pMe->pac );
         }
         // we could also reach here because of an error 
         // in connect, enable, or update security
         if ( ( pMe->currOp != OEMBTSPP_OP_SRCH ) &&
              ( pMe->currOp != OEMBTSPP_OP_CANCEL) )
         {
            // if we are the server, unregister with RM
            if ( (pMe->bClientApp == FALSE ) &&
                 ( pMe->role.serv.bSecSet  == TRUE ))
            {
               bt_service_id_type  service_ident;
               bt_cmd_status_type  stat;

               service_ident.service_id_method = BT_SIM_RFCOMM_SCN;
               service_ident.rfcomm_scn        = pMe->uChannelNumber;

#ifndef FEATURE_BT_2_1
               stat = bt_cmd_rm_set_service_security( pMe->appId,
                                                      &service_ident,
                                                      BT_SEC_NONE,
                                                      FALSE, 
                                                      FALSE );
#else
               stat = bt_cmd_rm_set_sm4( pMe->appId,
                                         &service_ident,
                                         BT_RM_SL_1_LOW,
                                         FALSE,
                                         FALSE );
#endif 
               if ((stat != BT_CS_GN_SUCCESS) && (stat != BT_CS_GN_PENDING))
               {
                  MSG_ERROR( "IBT SPP Server de register with RM failed ",stat,0,0);
                  pMe->currOp = OEMBTSPP_NO_OP;
               }
               pMe->role.serv.bSecSet  = FALSE;
            }
            else
            {
               pMe->currOp = OEMBTSPP_NO_OP;
            }
         }
         break;

      case BT_SPP_ST_OPENING:
         break;

      case BT_SPP_ST_OPEN_ERROR:
         // if enable or connect fails, the 
         // result will be sent after the stream is closed
         pMe->closeReason = pMe->result;
         sio_close(pMe->streamID, NULL);
         break;

      case BT_SPP_ST_OPEN:

         pMe->bOKToSend = FALSE;
         dsm_empty_queue( &pMe->txWaterMark );

         // We have to be the server, but we still check.
         // we need to set the security level here and enable the SDP rec
         if ( pMe->bClientApp == FALSE )
         {
            if ( ( pMe->currOp != OEMBTSPP_OP_DISCONNECT ) &&
                 ( pMe->currOp != OEMBTSPP_OP_ENABLE) &&
                 ( pMe->currOp != OEMBTSPP_OP_DISABLE) )
            {
               // will reach here if the client disconnects
               pMe->currOp = OEMBTSPP_OP_OPENED;
            }

            OEMBTSPP_UpdateServSDPRec(pMe, TRUE);
         }
         // if the remote side has disconnected while we 
         // were doing a configure
         while ( pMe->pcbIoctlResult != NULL )
         {
            // we had initiated the config change.
            AEECallback* pCb = pMe->pcbIoctlResult;

            if ( (pMe->pConfig != NULL) &&
                 (pMe->pResult != NULL ) )
            {
               *pMe->pResult = AEEBT_RESULT_REMOTE_DISCONNECT;
               *pMe->pConfig = pMe->config;
            }
            AEE_APP_RESUME( pCb, pMe->pac );
            pMe->pcbIoctlResult = NULL;
            pMe->pConfig        = NULL;
            pMe->pResult        = NULL;
         }
         while ( pMe->pcbFlushTx != NULL )
         {
            AEECallback* pCb = pMe->pcbFlushTx;
            pMe->pcbFlushTx = pMe->pcbFlushTx->pNext;

            if ( pCb->pmc != NULL )
            {
               BTSPPModemStatus* pModStat = pCb->pmc;
               *pModStat = pMe->modemStatus;
               pModStat->status |= AEEBTSPP_MS_TX_FLUSHED_B;
            }
            AEE_APP_RESUME( pCb, pMe->pac );
         }

         break;

      case BT_SPP_ST_CONNECTED:

         pMe->bOKToSend = TRUE;

         if ( pMe->bClientApp == TRUE )
         {
            // report status of the IBTSPPCLI_Connect the client 
            if ( pMe->currOp == OEMBTSPP_OP_CONNECT )
            {
               if (( NULL != pMe->pcbResult ) && ( NULL != pMe->pResult ))
               {
                  *pMe->pResult    =
                  OEMBTSPP_ConvertBTReason( pStatus->spp_reason );
                  AEE_ResumeCallback( pMe->pcbResult, pMe->pac );
                  pMe->pcbResult   = NULL;
                  pMe->pResult     = NULL;
               }
               pMe->currOp = OEMBTSPP_NO_OP;
            }
         }
         else
         {
            // disable SDP record if app has requested it
#ifndef BT_SIMULATION
            OEMBTSPP_UpdateServSDPRec(pMe, FALSE);
#else
            if (AEEBT_SEC_NONE == pMe->role.serv.secLevel)
            {
                if (pMe->role.serv.pcbAuth != NULL)
                {
                    AEE_ResumeCallback (pMe->role.serv.pcbAuth, pMe->pac);
                }
            }
            else
            {
                OEMBTSPP_UpdateServSDPRec(pMe, FALSE);
            }
#endif
         }
         // If there was a writable callback 
         // registered before the stream was connected
         // we call it .
         while ( pMe->pcbWrite != NULL )
         {
            AEECallback* pCb = pMe->pcbWrite;

            MSG_MED( "IBT SPP  Call wrtble cb str id 0x%x indx 0x%x ",
                     pMe->streamID,pMe->uIndex,0);
            pMe->pcbWrite = pMe->pcbWrite->pNext;
            AEE_APP_RESUME( pCb, pMe->pac );
         }
         break;

      case BT_SPP_ST_DISCONNECTING:
         pMe->bOKToSend = FALSE;

         break;

      case BT_SPP_ST_DISCONNECTED: 

         pMe->bOKToSend = FALSE;
         dsm_empty_queue( &pMe->txWaterMark );
       

         // called only for the client.
         if ( pMe->bClientApp )
         {
            if ( pMe->currOp == OEMBTSPP_OP_DISCONNECT)
            {
               pMe->closeReason = AEEBT_RESULT_NORMAL_DISCONNECT;
            }
            else
            {
               if ( pMe->currOp != OEMBTSPP_OP_CONNECT )
               {
                  pMe->closeReason = AEEBT_RESULT_REMOTE_DISCONNECT;
               }
            }
            sio_close( pMe->streamID, NULL);
            while ( pMe->pcbIoctlResult != NULL )
            {
               // we had initiated the config change, but server 
               // disconnected.
               AEECallback* pCb = pMe->pcbIoctlResult;

               if ( (pMe->pConfig != NULL) &&
                    (pMe->pResult != NULL ) )
               {
                  *pMe->pResult = AEEBT_RESULT_REMOTE_DISCONNECT;
                  *pMe->pConfig = pMe->config;
               }
               AEE_APP_RESUME( pCb, pMe->pac );
               pMe->pcbIoctlResult = NULL;
               pMe->pConfig        = NULL;
               pMe->pResult     = NULL;
            }
            while ( pMe->pcbFlushTx != NULL )
            {
               AEECallback* pCb = pMe->pcbFlushTx;
               pMe->pcbFlushTx = pMe->pcbFlushTx->pNext;

               if ( pCb->pmc != NULL )
               {
                  BTSPPModemStatus* pModStat = pCb->pmc;
                  *pModStat = pMe->modemStatus;
                  pModStat->status |= AEEBTSPP_MS_TX_FLUSHED_B;
               }
               AEE_APP_RESUME( pCb, pMe->pac );
            }
         }

         break;
      default:
         // should not be reaching here
         MSG_ERROR( "IBT SPP Unhandled state change - \
                       App indx = 0x%x st = 0x%x r = 0x%x", \
                    pMe->uIndex, pStatus->spp_state, pStatus->spp_reason );

         return;
      }

      if (pMe->state != (AEEBTSPPState) pStatus->spp_state)
      {
         // now update the state
         pMe->state          = (AEEBTSPPState) pStatus->spp_state;
         pMe->statusChanged |= OEMBTSPP_STATE_CHANGE;

         while ( pMe->pcbStateChange != NULL )
         {
            AEECallback* pCb = pMe->pcbStateChange;
            pMe->pcbStateChange = pMe->pcbStateChange->pNext;
            AEE_APP_RESUME( pCb, pMe->pac );
         }
      }
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_UpdateServSDPRec( OEMBTSPP* pMe, boolean enable )
{
   bt_service_id_type  service_ident;
   bt_cmd_status_type stat;

   if ( pMe->bClientApp != FALSE )
   {
      return;
   }
   if ( enable )
   {
      if ( pMe->currOp == OEMBTSPP_OP_ENABLE )
      {
         if ( ( AEEBT_SEC_NONE != pMe->role.serv.secLevel ) ||
              ( NULL != pMe->role.serv.pcbAuth) )
         {
            // set the security level for the service
            // the command done event for set security
            // will complete the result callback.
            service_ident.service_id_method = BT_SIM_RFCOMM_SCN;
            service_ident.rfcomm_scn        = pMe->uChannelNumber;
            pMe->role.serv.bSecSet           = TRUE;
#ifndef FEATURE_BT_2_1
            stat = bt_cmd_rm_set_service_security(
                                                 pMe->appId, 
                                                 &service_ident,
                                                 (bt_security_type) pMe->role.serv.secLevel,
                                                 (NULL == pMe->role.serv.pcbAuth ) ? FALSE : TRUE,
                                                 TRUE);
#else
            stat = bt_cmd_rm_set_sm4(
                                    pMe->appId, 
                                    &service_ident,
                                    BT_RM_SL_1_LOW,
                                    (NULL == pMe->role.serv.pcbAuth ) ? FALSE : TRUE,
                                    TRUE);
#endif 
            if ((stat != BT_CS_GN_SUCCESS) && (stat != BT_CS_GN_PENDING))
            {
               MSG_ERROR( "IBT SPP Set Sec for server failed res 0x%x",stat,0,0);
               pMe->role.serv.bSecSet = FALSE;
               pMe->closeReason = OEMBTSPP_CmdStatusToAeeResult(stat);
               sio_close(pMe->streamID, NULL);
            }
         }
         else
         {
            // complete the EnableService. result cb.
            while ( pMe->pcbResult != NULL )
            {
               AEECallback* pCb = pMe->pcbResult;
               pMe->pcbResult = NULL;
               if ( pMe->pResult != NULL)
               {
                  *(pMe->pResult) = AEEBT_RESULT_SUCCESS;
                  pMe->result     = AEEBT_RESULT_SUCCESS;
               }
               AEE_APP_RESUME( pCb, pMe->pac );
            }
            pMe->currOp = OEMBTSPP_NO_OP;
         }
        
      }
      else
      {
         // we are here because of a self or client 
         // initiated disconnect
         // see if the service was unregistered.
         // But if the server has initiated a 
         // disable, then we should not re-register with SDP
         if ( (pMe->role.serv.bUnRegSdp) &&
              (pMe->currOp != OEMBTSPP_OP_DISABLE))
         {
            stat = bt_cmd_sd_register_service
                   (
                   pMe->appId,
                   pMe->role.serv.uuid,
                   pMe->role.serv.version,
                   pMe->uChannelNumber,
                   pMe->role.serv.svcName
                   );

            if ((stat != BT_CS_GN_SUCCESS) && (stat != BT_CS_GN_PENDING))
            {
               MSG_ERROR( "IBT SPP Register SPP Server with SDP failed",0,0,0);
               pMe->closeReason = OEMBTSPP_CmdStatusToAeeResult(stat);
               sio_close(pMe->streamID, NULL);
            }
         }
         else
         {
            // either we disconnected or the client initiated a 
            // disconnect
            if ( pMe->currOp == OEMBTSPP_OP_DISCONNECT )
            {
               // complete the disconnect callback.
               if (( NULL != pMe->pcbResult ) && ( NULL != pMe->pResult ))
               {
                  *pMe->pResult    = AEEBT_RESULT_SUCCESS;
                  AEE_ResumeCallback( pMe->pcbResult, pMe->pac );
                  pMe->pcbResult   = NULL;
                  pMe->pResult     = NULL;
               }
            }
            if ( pMe->currOp != OEMBTSPP_OP_DISABLE )
            {
               pMe->currOp = OEMBTSPP_NO_OP;
            }
         }
      }
   }
   else
   {
      // App wants us to disable this service when connected 
      if ( pMe->role.serv.bUnRegSdp)
      {

         MSG_MED( "IBT SPP Unregister SPP Server - "
                  "App indx = 0x%x uuic = 0x%x scn = 0x%x", 
                  pMe->uIndex, pMe->role.serv.uuid, pMe->uChannelNumber );

         bt_cmd_sd_unregister_srv_by_scn(pMe->appId,
                                         pMe->role.serv.uuid,
                                         pMe->uChannelNumber);
      }
   }

}
////////////////////////////////////////////////////////
// This function is called from the BT task's signal
// processing loop.
////////////////////////////////////////////////////////
/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_ProcessEvQueue( void )
{

   bt_ev_msg_type *bt_ev_ptr;
   OEMBTEvent *pEv;
   int cnt = q_cnt( &gSppEventQueue );

   OEMBTSPP* pMe = NULL ;

   BT_VSTAT_MAX( cnt, OEMBTEXTSPP_MAX_EV_Q_DEPTH );

   while ( ( bt_ev_ptr = (bt_ev_msg_type *) q_get( &gSppEventQueue ) ) != NULL )
   {
      if ( EV_RANGE(bt_ev_ptr->ev_hdr.ev_type , BT_CMD_EV_SP_BASE ))
      {

         pMe = OEMBTSPP_FindMeFromAppId( bt_ev_ptr->ev_hdr.bt_app_id );

         if ( (pMe != NULL) && (( pEv = OEMBT_GetFreeEv()) != NULL) )
         {
            pEv->bt_event = *bt_ev_ptr;
            pEv->uState   = BT_ECS_READY;
            CALLBACK_Init( &pMe->BREWEvCb, OEMBTSPP_BREWCallEventCB, pMe );
            AEE_SYS_RESUME( &pMe->BREWEvCb );
         }

      }
      q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
   }

}


///////////////////////////////////////////////////////////////////////////////
// Callbacks from either BT or DSM watermarks
///////////////////////////////////////////////////////////////////////////////
/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_StatusChangeCB( bt_spp_status_type* pStatus, uint32 index )
{

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromIndex( index );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_STATUS_CHANGE;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_status_change.status_change = *pStatus;
      OEMBTSPP_store_bt_event( &bt_ev );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_ConfigChangeCB( bt_spp_cfg_rpt_type* config_ptr )
{

   OEMBTSPP* pMe = NULL;
   
   // the Config change callback may arrive before 
   // sio_open() returns for the client. The stream ID would not 
   // have been updated at that time.
   rex_enter_crit_sect( &gSppCritSect );

   pMe = OEMBTSPP_FindMe( config_ptr->stream_id );

   rex_leave_crit_sect( &gSppCritSect );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_CONFIG_CHANGE;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_config_change.config_change = *config_ptr;
      OEMBTSPP_store_bt_event( &bt_ev );
   }

}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_ModemStatusCB( bt_spp_ms_type* modem_status_ptr )
{

   OEMBTSPP* pMe = NULL ;

   // the modem change callback may arrive before 
   // sio_open() returns. The stream ID would not 
   // have been updated at that time.
   rex_enter_crit_sect( &gSppCritSect );

   pMe = OEMBTSPP_FindMe( modem_status_ptr->stream_id );

   rex_leave_crit_sect( &gSppCritSect );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_MODEM_STATUS;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_modem_status.modem_status = *modem_status_ptr;
      OEMBTSPP_store_bt_event( &bt_ev );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_LineErrorCB( bt_spp_le_type* line_error_ptr )
{

   OEMBTSPP* pMe = OEMBTSPP_FindMe( line_error_ptr->stream_id );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_LINE_ERROR;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_line_error.line_error = *line_error_ptr;
      OEMBTSPP_store_bt_event( &bt_ev );
   }
}

/*======================================================================= 
=======================================================================*/
/////////////////////////////////////////////////////////
// NOTE: This DSM callback is called with tasks locked //
/////////////////////////////////////////////////////////
static void OEMBTSPP_DataReceived( dsm_watermark_type* rx_wm, void* pCbData )
{

   bt_app_id_type app_id = (bt_app_id_type) ((uint32) pCbData);

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( app_id );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_RX_GNE;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_rx_gne.stream_id = pMe->streamID;
      OEMBTSPP_store_bt_event( &bt_ev );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_RxLoWater( dsm_watermark_type* prx_wm, void* pCbData )
{

   /////////////////////////////////////////////////////////
   // NOTE: This DSM callback is called with tasks locked //
   /////////////////////////////////////////////////////////
   bt_app_id_type app_id = (bt_app_id_type) ((uint32) pCbData);

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( app_id );

   if ( pMe != NULL )
   {
      sio_ioctl( pMe->streamID,
              SIO_IOCTL_INBOUND_FLOW_ENABLE, NULL );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_RxHiWater( dsm_watermark_type* prx_wm, void* pCbData )
{

   /////////////////////////////////////////////////////////
   // NOTE: This DSM callback is called with tasks locked //
   /////////////////////////////////////////////////////////
   bt_app_id_type app_id = (bt_app_id_type) ((uint32) pCbData);

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( app_id );

   if ( pMe != NULL )
   {
      sio_ioctl( pMe->streamID,
              SIO_IOCTL_INBOUND_FLOW_DISABLE, NULL );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_TxLowWater( dsm_watermark_type* ptx_wm, void* pCbData )
{

   /////////////////////////////////////////////////////////
   // NOTE: This DSM callback is called with tasks locked //
   /////////////////////////////////////////////////////////
   bt_app_id_type app_id = (bt_app_id_type) ((uint32) pCbData);

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( app_id );

   if ( (pMe != NULL) && (pMe->bOKToSend != TRUE) )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_TX_LO_WM;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_rx_gne.stream_id = pMe->streamID;
      OEMBTSPP_store_bt_event( &bt_ev );
   }

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_TxHiWater( dsm_watermark_type* ptx_wm, void* pCbData )
{
   /////////////////////////////////////////////////////////
   // NOTE: This DSM callback is called with tasks locked //
   /////////////////////////////////////////////////////////
   bt_app_id_type app_id = (bt_app_id_type) ((uint32) pCbData);

   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( app_id );

   if ( pMe != NULL )
   {
      pMe->bOKToSend = FALSE;
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EnableDTR( int8 index )
{
   OEMBTSPP* pMe = OEMBTSPP_FindMeFromIndex( index );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;

      bt_ev.ev_hdr.ev_type = BT_EV_SPP_ENABLE_DTR;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_enable_dtr.stream_id = pMe->streamID;
      OEMBTSPP_store_bt_event( &bt_ev );
   }
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_EnableDTR0( void )
{
   OEMBTSPP_EnableDTR( 0 );
}
static void OEMBTSPP_EnableDTR1( void )
{
   OEMBTSPP_EnableDTR( 1 );
}
static void OEMBTSPP_EnableDTR2( void )
{
   OEMBTSPP_EnableDTR( 2 );
}
static void OEMBTSPP_EnableDTR3( void )
{
   OEMBTSPP_EnableDTR( 3 );
}
static void OEMBTSPP_EnableDTR4( void )
{
   OEMBTSPP_EnableDTR( 4 );
}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_StatusChangeCB0( bt_spp_status_type* pStatus)
{
   OEMBTSPP_StatusChangeCB( pStatus, 0 );
}
static void OEMBTSPP_StatusChangeCB1( bt_spp_status_type* pStatus )
{
   OEMBTSPP_StatusChangeCB( pStatus, 1 );
}
static void OEMBTSPP_StatusChangeCB2( bt_spp_status_type* pStatus )
{
   OEMBTSPP_StatusChangeCB( pStatus, 2 );
}
static void OEMBTSPP_StatusChangeCB3( bt_spp_status_type* pStatus )
{
   OEMBTSPP_StatusChangeCB( pStatus, 3 );
}
static void OEMBTSPP_StatusChangeCB4( bt_spp_status_type* pStatus )
{
   OEMBTSPP_StatusChangeCB( pStatus, 4 );
}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_FlushTx( int8 index )
{
   OEMBTSPP* pMe = OEMBTSPP_FindMeFromIndex( index );

   if ( pMe != NULL )
   {
      bt_ev_msg_type bt_ev;
      bt_ev.ev_hdr.ev_type = BT_EV_SPP_FLUSH_TX;
      bt_ev.ev_hdr.bt_app_id = pMe->appId;
      bt_ev.ev_msg.ev_spp_flush_tx.stream_id = pMe->streamID;
      OEMBTSPP_store_bt_event( &bt_ev );
   }
}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_FlushTx0( void )
{
   OEMBTSPP_FlushTx( 0 );
}
static void OEMBTSPP_FlushTx1( void )
{
   OEMBTSPP_FlushTx( 1 );
}
static void OEMBTSPP_FlushTx2( void )
{
   OEMBTSPP_FlushTx( 2 );
}
static void OEMBTSPP_FlushTx3( void )
{
   OEMBTSPP_FlushTx( 3 );
}
static void OEMBTSPP_FlushTx4( void )
{
   OEMBTSPP_FlushTx( 4 );
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_DevDiscResponse( OEMBTSPP* pMe, 
                                      bt_ev_msg_type* bt_ev_msg_ptr )
{

   bt_ev_sd_device_discovery_resp_type* pEvt =
   &bt_ev_msg_ptr->ev_msg.ev_sd_dev_discv_resp;

   OEMBTSPP_DEVINFO*  pDevInfo = OEM_Malloc( sizeof ( OEMBTSPP_DEVINFO ) );

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   if ( pDevInfo == NULL )
   {
      MSG_ERROR( "IBT SPP Out of memory. Cannot allocate mem for device disc ",0, 0, 0);
      // no point continuing
      bt_cmd_sd_stop_device_discovery( pMe->appId );
      return;
   }

   BT_BDA( MSG_API, "IBT SPP Dev dis resp",
           &pEvt->bd_addr );

   MSG_MED( " IBT SPP Dev dis resp. bd Addr = %x : %x : %x :",
            pEvt->bd_addr.bd_addr_bytes[0],
            pEvt->bd_addr.bd_addr_bytes[1],
            pEvt->bd_addr.bd_addr_bytes[2]);

   MSG_MED( "%x : %x : %x",
            pEvt->bd_addr.bd_addr_bytes[3],
            pEvt->bd_addr.bd_addr_bytes[4],
            pEvt->bd_addr.bd_addr_bytes[5]);

   // initialize it 
   q_link( pDevInfo, &pDevInfo->link );
   pDevInfo->uNumSPPServices = 0;

   pDevInfo->bdi.bdAddr = *((BDAddress*)&pEvt->bd_addr);

   pDevInfo->bdi.COD.COS     = pEvt->service_class;
   pDevInfo->bdi.COD.DevCls  = pEvt->major_device_class;
   pDevInfo->pRecs           = NULL;
   // append to the temp queue
   q_put( &pMe->role.cli.discDevQueue, &pDevInfo->link );

}

/*======================================================================= 
=======================================================================*/
static int OEMBTSPP_do_service_search( OEMBTSPP* pMe, 
                                       OEMBTSPP_DEVINFO* pDevInfo )
{

   bt_sd_srch_pat_uuid_list_type     uuid_list;
   bt_sd_srch_pat_attr_id_list_type  attr_id_list;
   bt_cmd_status_type                stat;

   uuid_list.num_uuid128 = 0;
   uuid_list.num_uuid = 1;
   uuid_list.uuid[0]  = BT_SD_SERVICE_CLASS_SERIAL_PORT;

   attr_id_list.num_attr_id         = 3;
   attr_id_list.attr_id[0].is_range = FALSE;
   attr_id_list.attr_id[0].value    = BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST;
   attr_id_list.attr_id[1].is_range = FALSE;
   attr_id_list.attr_id[1].value    = BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST;
   attr_id_list.attr_id[2].is_range = FALSE;
   attr_id_list.attr_id[2].value    = BT_SD_ATTR_ID_SERVICE_NAME;

   stat = bt_cmd_sd_search_service( pMe->appId,
                                    (bt_bd_addr_type*)&pDevInfo->bdi.bdAddr,
                                    &uuid_list,
                                    &attr_id_list,
                                    BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT );

   return OEMBTSPP_CheckCmdStatus( stat );
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_CompleteServerSearch( OEMBTSPP* pMe )
{
   OEMBTSPP_CLIENT*  cli = &pMe->role.cli;
   IShell* pIShell       = AEE_GetShell();

   // in case we are not cancelled and not timed out,
   // and there are no more devices OR the req num of
   // devices are found, search timer is still running.
   if (pMe->role.cli.uSearchTime != 0 )
   {
      ISHELL_CancelTimer(pIShell,
                         OEMBTSPP_SearchTimeout,
                         (void*)pMe);
   }

   //  search is cancelled
   if ( cli->uSrchSt == OEMBTSPP_CANCL_ON )
   {
      // search has been cancelled.Free any servers that are in the list.
      OEMBTSPP_DEVINFO *pDevInfo;
      MSG_MED( "IBT SPP search cancelled",0,0,0);

      while ( q_cnt( &pMe->role.cli.servQueue ) > 0 )
      {
         pDevInfo = 
         (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.servQueue );

         if ( NULL != pDevInfo->pRecs )
         {
            OEM_Free ( pDevInfo->pRecs );   
         }
         OEM_Free ( pDevInfo );
      }
      while ( q_cnt( &pMe->role.cli.discDevQueue ) > 0 )
      {
         pDevInfo = 
         (OEMBTSPP_DEVINFO*) q_get( &pMe->role.cli.discDevQueue );
         if ( NULL != pDevInfo->pRecs )
         {
            OEM_Free ( pDevInfo->pRecs );   
         }
         OEM_Free ( pDevInfo );
      }
      // cancel API callback.
      if (NULL != pMe->pcbResult)
      {
         AEE_ResumeCallback( pMe->pcbResult, pMe->pac );
         pMe->pcbResult = NULL;
      }
   }

   cli->uSrchSt = OEMBTSPP_SRCH_OFF;
   gSppDevDiscState = OEMBTSPP_SRCH_OFF;
   pMe->currOp = OEMBTSPP_NO_OP;
   return;
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_SearchNextDev( OEMBTSPP* pMe )
{

   OEMBTSPP_DEVINFO*   pDevInfo;
   OEMBTSPP_CLIENT*    cli = NULL;
   int                 stat;

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   cli = &pMe->role.cli;

   pDevInfo = q_check( &pMe->role.cli.discDevQueue );

   MSG_MED( " IBT SPP disc dev Q pDevInfo %x dev found  %x ",
            pDevInfo,cli->uNumDevFound ,0  );


   if ( (NULL != pDevInfo) && // there is a device 
        ( (cli->uMaxNumDevReq == 0) || // app req for all dev 
          (cli->uNumDevFound < cli->uMaxNumDevReq) ) && // 
        (cli->uSrchSt != OEMBTSPP_CANCL_ON))
   { // if search is not cancelled.
      // if there are devices in the vicinity and the required num of servers
      // are not yet found, query the next device.
      MSG_MED( "IBT SPP searching next dev",0,0,0);

      stat = OEMBTSPP_do_service_search( pMe, pDevInfo );

      if ( stat != SUCCESS )
      {
         OEMBTSPP_CompleteServerSearch( pMe );
      }
   }
   else
   {
      OEMBTSPP_CompleteServerSearch( pMe );

   }

   // invoke app's callback, if we have finished search (or cancelled)
   // or there is a dev found
   if ( (cli->uSrchSt == OEMBTSPP_SRCH_OFF) ||
        (q_cnt (&pMe->role.cli.servQueue) > 0) )
   {
      OEMBTSPP_DEVINFO* pSrc = 
      (OEMBTSPP_DEVINFO*) q_check( &pMe->role.cli.servQueue );

      MSG_MED( " IBT SPP Return Serv info to app; cnt 0x%x",
               q_cnt (&pMe->role.cli.servQueue),0,0);

      // invoke app's callback, if any
      while ( cli->pcbDevDisc != NULL )
      {
         AEECallback* pCb = cli->pcbDevDisc;
      
         if (cli->pDevInfoSize != NULL)
         {
            *cli->pDevInfoSize = 
            ( q_cnt(&pMe->role.cli.servQueue) > 0 )? 
            sizeof( BTSPPDeviceInfo ) + 
            ( (pSrc->uNumSPPServices - 1) * sizeof( BTSPPServiceRecord) ):
            0;
            cli->pDevInfoSize  =  NULL;

         }
         cli->pcbDevDisc = cli->pcbDevDisc->pNext;
         AEE_APP_RESUME( pCb, pMe->pac );
      }
   }
}


/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_DevNameResp( OEMBTSPP* pMe, bt_ev_msg_type* bt_ev_ptr )
{

   bt_ev_sd_dev_name_resp_type* pEvt = &bt_ev_ptr->ev_msg.ev_sd_dname;
   OEMBTSPP_DEVINFO*        pDevInfo ;

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   pDevInfo = q_last_check( &pMe->role.cli.servQueue );

   if ( pDevInfo == NULL)
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   if ( pEvt->reason == BT_EVR_GN_SUCCESS )
   {
      // sanity check
      if ( BT_BD_ADDRS_EQUAL( &pDevInfo->bdi.bdAddr, &pEvt->bd_addr ) )
      {
         std_strlcpy((char*)pDevInfo->bdi.szName,
                     (char*) pEvt->device_name_str,
                      sizeof(pDevInfo->bdi.szName));
      }
   }

   // start service search for next device
   OEMBTSPP_SearchNextDev( pMe );

}

/*======================================================================= 
Iterates through Search Result DB couting up all SPP service records
=======================================================================*/
static uint8 OEMBTSPP_CountSPPRecs( void )
{

   bt_sd_srv_rec_type*  srvRecPtr;
   uint8 numRecs = 0;
   int i,j;
   bt_sd_srv_attr_rec_type* attr_ptr;

   bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
                            BT_SD_ITER_ALL );
   for ( srvRecPtr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.first();
       srvRecPtr != NULL;
       srvRecPtr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.next()
       )
   {
      // get uuid list.
      for ( i = 0; i < srvRecPtr->num_srv_attr; i++)
      {
         attr_ptr = &srvRecPtr->srv_attr[i];
         switch ( attr_ptr->attr_id )
         {
         case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
            for ( j =0; j < attr_ptr->attr_value.uuid_list.num_uuid; j++)
            {
               if ( BT_SD_SERVICE_CLASS_SERIAL_PORT == 
                    attr_ptr->attr_value.uuid_list.uuid[ j ] )
               {
                  numRecs++;
                  break;
               }
            }

            break;
         } 
      }
   }
   return numRecs;

}

/*======================================================================= 
Iterates through Search Result DB extracting data for all relevant 
attributes from each and every SPP service record.
=======================================================================*/
static void OEMBTSPP_ExtractAttribs( OEMBTSPP_DEVINFO* pSrvInfo )
{

   uint32                    i,j;
   bt_sd_srv_rec_type*      srvRecPtr;
   bt_sd_srv_attr_rec_type* attr_ptr;
   uint32                   len;

   bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
                            BT_SD_ITER_ALL );
   for ( srvRecPtr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.first();
       srvRecPtr != NULL;
       srvRecPtr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.next()
       )
   {
      if ( srvRecPtr->num_srv_attr == 0 )
      {
         continue;
      }

      j = pSrvInfo->uNumSPPServices++;
      for ( i=0; i<srvRecPtr->num_srv_attr; i++ )
      {
         attr_ptr = &srvRecPtr->srv_attr[i];
         switch ( attr_ptr->attr_id )
         {
         case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
            pSrvInfo->pRecs[ j ].uuid = 
            attr_ptr->attr_value.uuid_list.uuid[ 0 ];
            break;
         case BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST:
            pSrvInfo->pRecs[ j ].uServerChannel = 
            bt_sd_extract_scn( attr_ptr );
            break;
         case BT_SD_ATTR_ID_SERVICE_NAME:

            len = STRLEN(attr_ptr->attr_value.str) >
                  AEEBT_MAX_LEN_SERVICE_NAME ?
                  AEEBT_MAX_LEN_SERVICE_NAME :
                  STRLEN (attr_ptr->attr_value.str);

            MEMCPY( pSrvInfo->pRecs[ j ].szServiceName,
                    attr_ptr->attr_value.str,
                    len );

            pSrvInfo->pRecs[ j ].szServiceName[len] = 0;

            break;
         default:
            MSG_ERROR( "unexp attrib %x", attr_ptr->attr_id, 0, 0 );
            break;
         }
      }
   }
}

/*======================================================================= 
Handles the BT_EV_SD_TIMEOUT_RESP event when doing service search
=======================================================================*/
static void OEMBTSPP_TimeoutResp( OEMBTSPP* pMe, 
                                  bt_ev_msg_type* msg_ptr )
{

   bt_ev_sd_timeout_resp_type*         ev_ptr;

   OEMBTSPP_DEVINFO*                   pDevInfo;

   ev_ptr = &msg_ptr->ev_msg.ev_sd_timeout_resp;

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   pDevInfo = q_get( &pMe->role.cli.discDevQueue );

   if ( NULL == pDevInfo )
   {
      MSG_ERROR( "IBT SPP timeout resp for wrong device ", 0, 0, 0 );
      return;
   }

   if ( BT_BD_ADDRS_EQUAL( &ev_ptr->bd_addr, &pDevInfo->bdi.bdAddr )  )
   {
      BT_BDA( MSG_API, "IBT SPP TimeOut resp for service srch",
              &ev_ptr->bd_addr );
      OEM_Free ( pDevInfo );
      OEMBTSPP_SearchNextDev( pMe);
   }
}

/*======================================================================= 
Handles the BT_EV_SD_ERROR_RESP event when doing service search
=======================================================================*/
static void OEMBTSPP_ErrorResp( OEMBTSPP* pMe, 
                                bt_ev_msg_type* msg_ptr )
{
   bt_ev_sd_error_resp_type* pEvt = &msg_ptr->ev_msg.ev_sd_error_resp;
   OEMBTSPP_DEVINFO*        pDevInfo;

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }
   //  check if the error was during service search
   // if the error occured during device name, we simply start service search 
   // for the next device 
   pDevInfo = q_check( &pMe->role.cli.discDevQueue );

   if ( pDevInfo == NULL )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   if ( BT_BD_ADDRS_EQUAL( &pDevInfo->bdi.bdAddr, &pEvt->bd_addr ) )
   {
      BT_BDA( MSG_API, "IBT SPP Error resp for service srch",
              &pEvt->bd_addr );

      pDevInfo = q_get(&pMe->role.cli.discDevQueue);

      if ( pDevInfo )
      {
         if ( pDevInfo->pRecs )
         {
            OEM_Free ( pDevInfo->pRecs );
         }
        OEM_Free ( pDevInfo);
      }
      OEMBTSPP_SearchNextDev( pMe);
   }

   // check if the error occured during get device name 
   pDevInfo = q_last_check( &pMe->role.cli.servQueue );

   if ( (pDevInfo != NULL) &&
        (BT_BD_ADDRS_EQUAL( &pDevInfo->bdi.bdAddr, &pEvt->bd_addr)) )
   {
      // we keep the device in the server list and proceed to the next.
      OEMBTSPP_SearchNextDev( pMe);
   }
}

/*======================================================================= 
Handles the BT_EV_SD_SERVICE_SEARCH_RESP event
=======================================================================*/
static void OEMBTSPP_ServiceSearchResp( OEMBTSPP* pMe, 
                                        bt_ev_msg_type* msg_ptr )
{

   bt_ev_sd_service_search_resp_type*  ev_ptr;
   uint8                               numRecs = 0;
   OEMBTSPP_DEVINFO*                   pDevInfo;
   bt_cmd_status_type                  stat;

   if ( FALSE == pMe->bClientApp )
   {
      MSG_ERROR( "IBT SPP getting client events for server ",0, 0, 0);
      return;
   }

   ev_ptr = &msg_ptr->ev_msg.ev_sd_service_search_resp;
   pDevInfo = q_get( &pMe->role.cli.discDevQueue );

   MSG_MED( "IBT SPP SD SS Resp - recCnt=%d", ev_ptr->srv_rec_count, 0, 0 );


   if ( NULL == pDevInfo )
   {
      MSG_ERROR( "IBT SPP service search resp for wrong device ",0,0,0 );
      return;
   }


   if ( BT_BD_ADDRS_EQUAL( &ev_ptr->bd_addr, &pDevInfo->bdi.bdAddr ) &&
        (ev_ptr->srv_rec_count > 0) )
   {

      numRecs = OEMBTSPP_CountSPPRecs();

      MSG_MED( "IBT SPP SPP service found, numRecs %d", numRecs, 0, 0 );

      BT_BDA( MSG_API, "IBT SPP Server BDA", &ev_ptr->bd_addr );

      if ( numRecs > 0 )
      {
         // allocate memory for services info
         pDevInfo->pRecs = OEM_Malloc ( numRecs * sizeof(BTSPPServiceRecord) );

         if ( NULL != pDevInfo->pRecs )
         {
            OEMBTSPP_ExtractAttribs( pDevInfo );

            // put this item in the servers queue
            q_put( &pMe->role.cli.servQueue, &pDevInfo->link );

            pMe->role.cli.uNumDevFound++;
            // now, get remote device's name
            stat  = bt_cmd_sd_get_device_name( pMe->appId, &ev_ptr->bd_addr );

            if ( (stat != BT_CS_GN_SUCCESS) && (stat != BT_CS_GN_PENDING) )
            {
               // although we do not get the name of the prev device
               // we still have other information.
               OEMBTSPP_SearchNextDev( pMe);
            }
            return;
         }
      }
   }

   // if the device addr does not match OR the device does not have SPP service
   OEM_Free( pDevInfo );
   OEMBTSPP_SearchNextDev( pMe);
}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_CmdResp( OEMBTSPP *pMe, bt_ev_gn_cmd_done_type* ev_ptr )
{
   switch ( ev_ptr->cmd_type )
   {
   case BT_CMD_SD_DISCOVER_DEVICES: 
      {
         if ( (pMe->bClientApp == TRUE) &&
              (BT_CS_GN_SUCCESS != ev_ptr->cmd_status) &&
              ((pMe->currOp == OEMBTSPP_OP_SRCH)||
               (pMe->currOp == OEMBTSPP_OP_CANCEL)))
         {
            while ( pMe->role.cli.pcbDevDisc != NULL )
            {
               AEECallback* pCb = pMe->role.cli.pcbDevDisc;
               if ( NULL != pMe->role.cli.pDevInfoSize )
               {
                  *pMe->role.cli.pDevInfoSize = 0;
                  pMe->role.cli.pDevInfoSize = NULL;
               }
               pMe->role.cli.pcbDevDisc = NULL;

               AEE_APP_RESUME( pCb, pMe->pac );
            }
            while ( pMe->pcbResult != NULL )
            {
               // we will reach here only if a cancel has
               // been initiated.
               AEECallback* pCb = pMe->pcbResult;
               pMe->pcbResult = NULL;
               *(pMe->pResult) = SUCCESS;
               AEE_APP_RESUME( pCb, pMe->pac );
            }

            pMe->role.cli.uSrchSt = OEMBTSPP_SRCH_OFF;
            gSppDevDiscState = OEMBTSPP_SRCH_OFF;
            pMe->currOp = OEMBTSPP_NO_OP;
         }
         break;
      }
   case BT_CMD_SD_GET_DEVICE_NAME: 
      {
         if ((pMe->bClientApp == TRUE) &&
             (BT_CS_GN_SUCCESS != ev_ptr->cmd_status))
         {
            OEMBTSPP_SearchNextDev( pMe );
         }
         break;
      }

   case BT_CMD_RM_AUTHORIZE_REPLY:
      {
         if ( pMe->currOp == OEMBTSPP_OP_AUTHORIZE )
         {
            while ( pMe->pcbResult != NULL )
            {
               AEECallback* pCb = pMe->pcbResult;
               pMe->pcbResult = NULL;
               if ( pMe->pResult != NULL)
               {
                  *(pMe->pResult) =
                  OEMBTSPP_CmdStatusToAeeResult( ev_ptr->cmd_status);
               }
               AEE_APP_RESUME( pCb, pMe->pac );
            }
            pMe->currOp = OEMBTSPP_NO_OP;
         }
         break;
      }
#ifdef FEATURE_BT_2_1
   case BT_CMD_RM_SET_SM4:
#else
   case BT_CMD_RM_SET_SERVICE_SECURITY:
#endif
      {
         if ( BT_CS_GN_SUCCESS != ev_ptr->cmd_status )
         {
            // this has been called when either 
            // we are enabling the service
            // or when de registering the server.
            MSG_ERROR( "IBT SPP set service security failed cmd stat =0x%x"
                        ,ev_ptr->cmd_status,0,0 );

            if ( pMe->state != BT_SPP_ST_CLOSED )
            {
               pMe->closeReason = 
                  OEMBTSPP_CmdStatusToAeeResult( ev_ptr->cmd_status);
               sio_close ( pMe->streamID, NULL);
            }
            else
            {
               pMe->currOp = OEMBTSPP_NO_OP;
            }
            pMe->role.serv.bSecSet = FALSE;
         }
         else
         {
            // complete the EnableService result cb.
            while ( pMe->pcbResult != NULL )
            {
               AEECallback* pCb = pMe->pcbResult;
               pMe->pcbResult = NULL;
               if ( pMe->pResult != NULL)
               {
                  *(pMe->pResult) = AEEBT_RESULT_SUCCESS;
                  pMe->result     = AEEBT_RESULT_SUCCESS;
               }
               AEE_APP_RESUME( pCb, pMe->pac );
            }
            pMe->currOp = OEMBTSPP_NO_OP;
            if ( pMe->state != BT_SPP_ST_CLOSED )
            {
               pMe->role.serv.bSecSet = TRUE;
            }
         }
         break;
      }
   case BT_CMD_SD_REGISTER_SERVICE:
      {
         if ( ( BT_CS_GN_SUCCESS != ev_ptr->cmd_status ) &&
              ( BT_CS_SD_SERVICE_RECORD_EXISTS != ev_ptr->cmd_status))
         {
            // this has been called when  
            // we bUnregSDP flag was true and server 
            // is back to opened state
            MSG_ERROR( "IBT SPP SDP register failed cmd stat =0x%x ",
                        ev_ptr->cmd_status,0,0 );
            pMe->closeReason = 
               OEMBTSPP_CmdStatusToAeeResult( ev_ptr->cmd_status);
            sio_close ( pMe->streamID, NULL);
         }
         else
         {
            // complete the Server initiated disconnect cb
            if ( pMe->currOp == OEMBTSPP_OP_DISCONNECT )
            {
               while ( pMe->pcbResult != NULL )
               {
                  AEECallback* pCb = pMe->pcbResult;
                  pMe->pcbResult = NULL;
                  if ( pMe->pResult != NULL)
                  {
                     *(pMe->pResult) = AEEBT_RESULT_SUCCESS;
                     pMe->result     = AEEBT_RESULT_SUCCESS;
                  }
                  AEE_APP_RESUME( pCb, pMe->pac );
               }
            }
            if ( ( pMe->currOp == OEMBTSPP_OP_DISCONNECT ) ||
                 ( pMe->currOp == OEMBTSPP_OP_OPENED ) )
            {
               pMe->currOp = OEMBTSPP_NO_OP;
            }
         }
         break;
      }
   case BT_CMD_SD_UNREGISTER_SRV_BY_SCN:
      {
         if ( BT_CS_GN_SUCCESS != ev_ptr->cmd_status )
         {
            MSG_ERROR( "IBT SPP Unregister service failed cmd stat = 0x%x",
                        ev_ptr->cmd_status,0,0);
         }
         break;
      }
   }
}

//==========================================================================
//   static helper functions
//==========================================================================
static void OEMBTSPP_Cancel( AEECallback* pcb )
{

   AEECallback** ppcb;

   ppcb = pcb->pCancelData;
   // Verify that it is me set as the cancel function.
   // Cross-task cancellations need this check.
   if ( pcb->pfnCancel == OEMBTSPP_Cancel )
   {
      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;

      for ( ; *ppcb != NULL; ppcb = &(*ppcb)->pNext)
      {
         if ( pcb == *ppcb )
         {
            *ppcb = pcb->pNext;
            pcb->pNext = NULL;
            break;
         }
      }
   }
}


/*======================================================================= 
=======================================================================*/
static OEMBTSPP* OEMBTSPP_FindMe( sio_stream_id_type streamID)
{

   OEMBTSPP* pSPP = NULL;

   if ( streamID != SIO_NO_STREAM_ID )
   {
      for ( pSPP = gSppMgr.pNextSPP; pSPP != NULL; pSPP = pSPP->pNextSPP )
      {
         if ( pSPP->streamID == streamID )
         {
            break;
         }
      }
   }
   return pSPP;

}


/*======================================================================= 
=======================================================================*/
static OEMBTSPP* OEMBTSPP_FindMeFromAppId( bt_app_id_type bt_app_id)
{

   OEMBTSPP* pSPP = NULL;

   if ( bt_app_id != BT_APP_ID_NULL )
   {
      for ( pSPP = gSppMgr.pNextSPP; pSPP != NULL; pSPP = pSPP->pNextSPP )
      {
         if ( pSPP->appId == bt_app_id )
         {
            break;
         }
      }
   }
   return pSPP;

}

/*======================================================================= 
=======================================================================*/
static OEMBTSPP* OEMBTSPP_FindMeFromIndex( uint32 index )
{

   OEMBTSPP* pSPP = NULL;

   for ( pSPP = gSppMgr.pNextSPP; pSPP != NULL; pSPP = pSPP->pNextSPP )
   {
      if ( pSPP->uIndex == index )
      {
         break;
      }
   }

   return pSPP;

}

/*======================================================================= 
=======================================================================*/
static OEMBTSPP* OEMBTSPP_GetObj( BTHandle hBT )
{

   OEMBTSPP* pOemSpp = NULL;
   AEEHandle_From( &gOEMBTSPPHandleList, hBT, (OEMINSTANCE*)&pOemSpp );
   return pOemSpp;

}

/*======================================================================= 
=======================================================================*/
static void OEMBTSPP_store_bt_event( bt_ev_msg_type* bt_ev_ptr )
{
   bt_store_bt_event( bt_ev_ptr, &gSppEventQueueInfo );
}

/*======================================================================= 
=======================================================================*/
static uint16 OEMBTSPP_GetMaxEventBytes( void )
{
   uint16 max_eb;

   max_eb =              sizeof( bt_ev_spp_flush_tx_type      )  ;
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_enable_dtr_type    ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_hi_wm_type      ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_lo_wm_type      ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_tx_gne_type        ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_hi_wm_type      ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_lo_wm_type      ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_rx_gne_type        ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_line_error_type    ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_modem_status_type  ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_config_change_type ) );
   max_eb = MAX( max_eb, sizeof( bt_ev_spp_status_change_type ) );

   max_eb = MAX( max_eb, sizeof( bt_cmd_status_type           ) );

   max_eb += sizeof( bt_ev_hdr_type );

   return max_eb;
}

/*======================================================================= 
=======================================================================*/
static BTResult OEMBTSPP_ConvertBTReason( bt_event_reason_type bt_reason )
{
   switch ( bt_reason )
   {
   case BT_EVR_GN_SUCCESS:
      return AEEBT_RESULT_SUCCESS;
   case BT_EVR_RC_NORMAL_DISCONNECT:
      return AEEBT_RESULT_NORMAL_DISCONNECT;
   case BT_EVR_SPP_OUT_OF_SERVER_CHANNELS:
   case BT_EVR_SPP_SDP_REGISTRATION_FAILED:
   case BT_EVR_SPP_RFCOMM_REGISTRATION_FAILED:
      return AEEBT_RESULT_NO_RESOURCE;
   case BT_EVR_SPP_SDP_TIMEOUT:
      return AEEBT_RESULT_REMOTE_TIMEOUT;
   case BT_EVR_RC_SETUP_NEGOTIATIONS_FAILED: 
      return AEEBT_RESULT_NEGOTIATION_FAILURE;

   case  BT_EVR_SPP_BAD_SERVER_CHANNEL:
   case BT_EVR_SPP_BAD_OPEN_PARAMS :
      return AEEBT_RESULT_BAD_PARAM;
   case BT_EVR_SPP_SERVER_CHANNEL_IN_USE:
      return AEEBT_RESULT_SERVER_CHANNEL_IN_USE;
   case BT_EVR_RC_REMOTE_REJECTS_CONNECTION:
      return AEEBT_RESULT_CONNECTION_REJECTED;

   default:
      return AEEBT_RESULT_UNKNOWN_ERR;
   }
}

/*======================================================================= 
=======================================================================*/
static BTResult OEMBTSPP_CmdStatusToAeeResult( bt_cmd_status_type stat )
{
   switch (stat)
   {
   case BT_CS_GN_SUCCESS:
   case BT_CS_GN_PENDING:
      return AEEBT_RESULT_SUCCESS;
   case BT_CS_GN_CMD_Q_FULL:
   case BT_CS_DC_SIO_PORT_ALREADY_OPENED:
   case BT_CS_DC_NO_SIO_PORT_ID:
      return AEEBT_RESULT_NO_RESOURCE;
   case BT_CS_RM_UNRECOGNIZED_CMD:
   default:
      return AEEBT_RESULT_UNKNOWN_ERR;
   }
}


/*======================================================================= 
=======================================================================*/

// This API is to be used to convert the cmd status result
// to an appropriate error that 
static int OEMBTSPP_CheckCmdStatus( bt_cmd_status_type stat )
{
   switch (stat)
   {
   case BT_CS_GN_SUCCESS:
   case BT_CS_GN_PENDING:
      return SUCCESS;
   case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
   case BT_CS_DC_SIO_PORT_ALREADY_OPENED:
   case BT_CS_DC_NO_SIO_PORT_ID:
      return AEEBT_EPORT;
   case BT_CS_RM_UNRECOGNIZED_CMD:
      return EUNSUPPORTED;
   default:
      return EFAILED;
   }
}

/*======================================================================= 
Executes in BREW context
=======================================================================*/
static void OEMBTSPP_BREWCallEventCB( OEMBTSPP* pMe )
{

   OEMBTEvent* pEv;
   bt_app_id_type  appId;

   MSG_MED( "IBT SPP --> BREWCallEventCB:", 0, 0, 0);

   if ( pMe == NULL )
   {
      MSG_ERROR( "IBT SPP --> OEMBTSPP_BREWCallEventCB: \
                  Unexpected NULL pointer", 0, 0, 0);
   }
   else
   {
      appId = pMe->appId; 
      while ( (pEv = OEMBT_GetReadyEv(appId)) != NULL )
      {
         switch ( pEv->bt_event.ev_hdr.ev_type )
         {
         case BT_EV_GN_CMD_DONE:
            OEMBTSPP_CmdResp( pMe, &pEv->bt_event.ev_msg.ev_gn_cmd_done );
            break;

         case BT_EV_RM_AUTHORIZE_REQUEST:
            OEMBTSPP_RmEvHndlr( pMe, &pEv->bt_event );
            break;

         case BT_EV_SD_DEVICE_DISCOVERY_RESP:
            OEMBTSPP_DevDiscResponse( pMe, &pEv->bt_event );
            break;
         case BT_EV_SD_DEVICE_DISCOVERY_COMPLETE:
         {
            IShell* pIShell = AEE_GetShell();
            // cancel the inquiry timer
            // this event could occur if the timer fired and the 
            // inquiry was cancelled, or the search was cancelled
            // by user or the inquiry was complete.
            // In all case it is OK to cancel the timer.

            MSG_MED( "IBT SPP Disc Complete st %x",pMe->role.cli.uSrchSt,0,0);

            if (pMe->role.cli.uSearchTime != 0 )
            {
               ISHELL_CancelTimer(pIShell,
                                  OEMBTSPP_InquiryTimeout,
                                  (void*)pMe);
            }
            // if user has not cancelled the search
            // start the search timer
            if ( pMe->role.cli.uSrchSt != OEMBTSPP_CANCL_ON )
            {
               MSG_MED( "IBT SPP starting srch st %x timeout  %x" 
                        ,pMe->role.cli.uSrchSt,pMe->role.cli.uSearchTime*2/3,0);

               pMe->role.cli.uSrchSt = OEMBTSPP_SRCH_ON;

               if (pMe->role.cli.uSearchTime != 0 )
               {
                  ISHELL_SetTimer( pIShell,
                                   (pMe->role.cli.uSearchTime*2/3*1000),
                                   OEMBTSPP_SearchTimeout,
                                   (void*)pMe
                                 );
               }

            }

            gSppDevDiscState = OEMBTSPP_SRCH_ON;
            OEMBTSPP_SearchNextDev( pMe );
            break;
         }
         case BT_EV_SD_DEVICE_NAME_RESP:
            OEMBTSPP_DevNameResp( pMe, &pEv->bt_event );
            break;
         case BT_EV_SD_SERVICE_SEARCH_RESP:
            OEMBTSPP_ServiceSearchResp( pMe, &pEv->bt_event );
            break;
         case BT_EV_SD_TIMEOUT_RESP:
            OEMBTSPP_TimeoutResp(pMe, &pEv->bt_event);
            break;
         case BT_EV_SD_ERROR_RESP:
            OEMBTSPP_ErrorResp( pMe, &pEv->bt_event );
            break;

         case BT_EV_SPP_STATUS_CHANGE:

            OEMBTSPP_EvStatusChange( &pEv->bt_event );
            break;

         case BT_EV_SPP_CONFIG_CHANGE:
            OEMBTSPP_EvConfigChange( &pEv->bt_event );
            break;

         case BT_EV_SPP_MODEM_STATUS:
            OEMBTSPP_EvModemStatus( &pEv->bt_event );
            break;
         case BT_EV_SPP_LINE_ERROR:
            OEMBTSPP_EvLineError( &pEv->bt_event );
            break;
         case BT_EV_SPP_RX_GNE:
            OEMBTSPP_EvRxGne( &pEv->bt_event );
            break;
         
         case BT_EV_SPP_TX_LO_WM:
            OEMBTSPP_EvTxLoWm( &pEv->bt_event );

            break;
         case BT_EV_SPP_ENABLE_DTR:
            OEMBTSPP_EvEnableDtr(&pEv->bt_event);
            break;
         case BT_EV_SPP_FLUSH_TX:
            OEMBTSPP_EvFlushTtx(&pEv->bt_event);
            break;
         default:
            MSG_MED( "Ignoring BT ev %x", pEv->bt_event.ev_hdr.ev_type, 0, 0 );
            break;
         }
         OEMBT_FreeEv( pEv ); // free the entry
      }
      // Note pMe should not be used after calling
      // cleanup as it could be the last event that 
      // was to be serviced before pMe is freed.
      OEMBTSPP_CleanUp(pMe);
   }
}

/*======================================================================= 
Executes in BT context, so save the event and schedule BREW callback
=======================================================================*/
static void OEMBTSPP_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr)
{

   OEMBTEvent* pEv;
   OEMBTSPP* pMe = OEMBTSPP_FindMeFromAppId( bt_ev_msg_ptr->ev_hdr.bt_app_id );
   
   if ( (pMe != NULL) && ((pEv = OEMBT_GetFreeEv()) != NULL) )
   {
      pEv->bt_event = *bt_ev_msg_ptr;
      pEv->uState   = BT_ECS_READY;
      CALLBACK_Init( &pMe->BREWEvCb, OEMBTSPP_BREWCallEventCB, pMe );
      AEE_SYS_RESUME( &pMe->BREWEvCb );
   }
}

#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


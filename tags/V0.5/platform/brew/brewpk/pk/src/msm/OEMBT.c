/*=============================================================================
FILE:  OEMBT.c

SERVICES:  Implementation of IBT,
  This file contains a reference implementation of BT support.

GENERAL DESCRIPTION:
   Implements IBT, IBTNotifier.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "OEMFeatures.h"

#include "OEMHeap.h"
#include "OEMCriticalSection.h"
#include "AEEBT.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "OEMBT.h"
#include "OEMBT_priv.h"
#include "btsd.h"
#include "msg.h"
#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }
#define OEMBT_MAX_EV_CACHE 30


/*=============================================================================
  OEMBT Definition
=============================================================================*/
struct _OEMBT
{
   uint32            uRefs;
   bt_app_id_type    appId;
   OEMBTEvent*       pEvCache;
   uint32            uReadyIndex;
   uint32            uFreeIndex;
   // callback of BTNotifier
   AEECallback*      pcbOnBTEv;
   boolean           bNotifierReg;
   boolean           bCallNotifierCb;
   uint32            uTimeStamp;
};

//==========================================================================
//   Static data
//==========================================================================
static OEMBT* pgOEMBT;
static OEMCriticalSection gOEMBTCritSect;

//==========================================================================
//   Function prototypes
//==========================================================================
static void OEMBT_EvCb( bt_ev_msg_type* ev_ptr );
static void OEMBT_EvCbRm( bt_ev_msg_type* ev_ptr );

static void OEMBT_InitMe( void );
static BTEvent OEMBT_BT2BREWEvent( bt_event_type bt_ev );
static void OEMBT_CancelCallback( AEECallback *pcb );
static BTModDevField OEMBT_BT2BREWStatus( bt_rm_dev_update_status_type stat );

//==========================================================================
//   Public Funtions
//==========================================================================
int OEMBT_New( OEMBT** ppif )
{
   uint8 i;
   OEMBTEvent* pEv;

   if ( NULL == pgOEMBT ) 
   {
      pgOEMBT = (OEMBT*)OEM_Malloc( sizeof(OEMBT) + 
                                    (sizeof(OEMBTEvent) * OEMBT_MAX_EV_CACHE) );
      
      if ( NULL != pgOEMBT )
      {
      
         pgOEMBT->uRefs     = 0;
         pgOEMBT->pcbOnBTEv = NULL;
         pgOEMBT->bNotifierReg    = FALSE; 
         pgOEMBT->bCallNotifierCb = FALSE;
         pgOEMBT->uTimeStamp  = 0;
        
         // Initialize the cache
         pEv = pgOEMBT->pEvCache  = (OEMBTEvent*)( pgOEMBT + 1 );
       
         // Mark all the cache entries as free
         for ( i = 0; i < OEMBT_MAX_EV_CACHE; i++, pEv++ )
         {
            pEv->uState = BT_ECS_FREE;
         }
         OEMBT_InitMe();

         pgOEMBT->appId     = bt_cmd_ec_get_application_id(  );

         if ( BT_APP_ID_NULL == pgOEMBT->appId )
         {
            OEM_Free ( pgOEMBT );
            pgOEMBT = NULL;
            
            MSG_ERROR("IBT Cannot reg app with BT ",0, 0, 0);  
            return EFAILED;
         }

         bt_cmd_ec_register_event_set( pgOEMBT->appId,
                                       OEMBT_EvCbRm,
                                       BT_EC_ES_RADIO_MANAGER,
                                       BT_EV_NULL,
                                       BT_EV_NULL );

         bt_cmd_ec_register_event_set( pgOEMBT->appId,
                                          OEMBT_EvCb,
                                          BT_EC_ES_CUSTOM,
                                          BT_EV_GN_CMD_DONE, 
                                          BT_EV_GN_CMD_DONE );
         
         bt_cmd_ec_reg_event_set_any_app_id( pgOEMBT->appId,
                                                OEMBT_EvCb,
                                                BT_EC_ES_CUSTOM,
                                                BT_EV_RM_DEVICE_UPDATED,
                                                BT_EV_RM_DEVICE_UPDATED );

         bt_cmd_rm_set_bondable( pgOEMBT->appId, TRUE );

         bt_cmd_rm_config_application( pgOEMBT->appId,
                                             FALSE,
                                             BT_RM_AVP_AUTOMATIC,
                                             BT_RM_AVP_AUTOMATIC,
                                             TRUE);
      }
      else
      {
         *ppif = 0;
         
         MSG_ERROR( "IBT Cannot allocate Mem for IBT ",0,0,0 );
         return ENOMEMORY;
      }
   }

   ++pgOEMBT->uRefs;

   MSG_MED( "IBT Returning IBT object. Ref count = 0x%x ", pgOEMBT->uRefs,0,0);
   *ppif = pgOEMBT;
   
   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
void OEMBT_NotifReg( OEMBT* pme )
{
   if ( pme != NULL )
   {
      pme->bNotifierReg = TRUE;
   }

   return ;
}

/*======================================================================= 
=======================================================================*/
void OEMBT_NotifUnReg( OEMBT* pme )
{
   OEMBTEvent* pEv = NULL;

   if ( pme != NULL )
   {
      pme->bNotifierReg = FALSE;

      while ( ( pEv = OEMBT_GetReadyEv( pme->appId )) != NULL)
      {
         OEMBT_FreeEv( pEv );
      }
   }
   return ;
}

/*======================================================================= 
=======================================================================*/
void OEMBT_IncRefCnt(void)
{
   if ( pgOEMBT != NULL )
   {
      ++pgOEMBT->uRefs;
   }
   return ;
}


/*======================================================================= 
=======================================================================*/
void OEMBT_DecRefCnt(void)
{
   if ( pgOEMBT != NULL )
   {
      OEMBT_Release( pgOEMBT );
   }
   return ;
}

/*======================================================================= 
=======================================================================*/
uint32 OEMBT_Release( OEMBT* pme )
{
   uint32 uRefs;

   if ( 0 == pme->uRefs )
   {
      return pme->uRefs;
   }

   uRefs = --pme->uRefs;

   MSG_MED( "IBT Decrementing ref count : ref count = 0x%x ", pme->uRefs,0,0);

   if ( uRefs == 0 )
   {
      pgOEMBT = NULL;

      if ( pme->pcbOnBTEv != NULL )
      {
         CALLBACK_Cancel( pme->pcbOnBTEv );
      }

      bt_cmd_rm_set_bondable( pme->appId, FALSE );

      bt_cmd_rm_config_application( pme->appId,
                                    TRUE,
                                    BT_RM_AVP_AUTOMATIC,
                                    BT_RM_AVP_AUTOMATIC,
                                    FALSE);


      bt_cmd_ec_unregister_event_set( pme->appId,
                                      BT_EC_ES_RADIO_MANAGER,
                                      BT_EV_NULL,
                                      BT_EV_NULL );

      bt_cmd_ec_unregister_event_set( pme->appId,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_GN_CMD_DONE,
                                           BT_EV_GN_CMD_DONE );

      bt_cmd_ec_unreg_event_set_any_app_id( pme->appId,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_RM_DEVICE_UPDATED,
                                           BT_EV_RM_DEVICE_UPDATED );

      bt_cmd_ec_free_application_id( pme->appId );
      OEM_Free( pme );
   }
   return uRefs;
}

/*======================================================================= 
=======================================================================*/
int OEMBT_GetNotificationData( BTNotifyInfo* pNotif, BTEventData* pED )
{

   int nErr = EUNSUPPORTED;
   uint8  i = OEMBT_MAX_EV_CACHE;

   if ( (pNotif == NULL) || (pED == NULL) )
   {
      nErr = EBADPARM;
   }
   else if ( pgOEMBT != NULL )
   {
      OEMBTEvent* pEv = pgOEMBT->pEvCache;

      OEMCriticalSection_Enter( &gOEMBTCritSect );

      for ( i = 0; i < OEMBT_MAX_EV_CACHE; i++, pEv++ )
      {
         if ( (BT_ECS_DISPATCHING == pEv->uState) &&
              (pNotif->event == OEMBT_BT2BREWEvent( pEv->bt_event.
                                                    ev_hdr.ev_type )) && 
              (pNotif->uTimeStamp == pEv->uTimeStamp) )
         {
            nErr = SUCCESS;
            break;
         }
      }
      
      if ( SUCCESS == nErr )
      {
         switch ( pNotif->event )
         {
            case AEEBT_EV_PIN_REQ:
               pED->bdAddr = *((BDAddress*)&pEv->bt_event.ev_msg.
                              ev_rm_pinrq.bd_addr);
               break;
            case AEEBT_EV_DEV_UPDATE:
            {
               BTDevUpdateEvData* pDevUpdateData = &pED->devUpdateEv;
               
               pDevUpdateData->bdAddr = *((BDAddress*)&pEv->bt_event.ev_msg.
                                          ev_rm_devud.bd_addr);

               pDevUpdateData->modField = OEMBT_BT2BREWStatus(
                             pEv->bt_event.ev_msg.ev_rm_devud.update_status);


               break;
            }
            default:
               // how is this possible?!
               break;
         }
       
      }
      else
      {
         // The app requested for an event which no longer exists in cache
         MSG_ERROR(" IBT Get Notif Data, event 0x%x not found ",pNotif->event,0,0);
      }
      OEMCriticalSection_Leave(&gOEMBTCritSect);
   }
   return nErr;

}
/*======================================================================= 
=======================================================================*/
int OEMBT_GetLocalInfo( BTLocalInfo* pLocalInfo )
{

   int nErr = EFAILED;

   if ( pgOEMBT != NULL )
   {
      bt_version_type vers;
      bt_cod_type cod;
      BTDeviceInfo* pInfo = &pLocalInfo->myInfo;
      bt_security_type security = BT_SEC_NONE;

      bt_cmd_rm_get_local_info( 
         pgOEMBT->appId, ((bt_bd_addr_type*)(&pInfo->bdAddr)), 
         &vers, (bt_lmp_features_type*)(&pInfo->LMPFeatures), 
         &cod, (char*) pInfo->szName,
         &security );

      pLocalInfo->mySecLevel       = (uint32)security;
      pInfo->LMPVersion.CompID     = (BTCompanyID) vers.manufacturer_name;
      pInfo->LMPVersion.VersNum    = (uint32) vers.lmp_version;
      pInfo->LMPVersion.SubVersNum = (uint32) vers.lmp_subversion;
      
      bt_sd_parse_class_of_device( &cod, 
                                   (uint16*)&pInfo->COD.COS, 
                                   (uint16*)&pInfo->COD.DevCls,
                                   (uint16*)&pInfo->COD.SubDevCls );
      pInfo->COD.SubDevCls -= pInfo->COD.DevCls;

      nErr = SUCCESS;
   }
   return nErr;
}

/*======================================================================= 
=======================================================================*/
int OEMBT_GetSPPServer( 
   OEMBT*             pme, 
   BTServiceClassUUID uuid, 
   const uint8*       pszServiceName, 
   const uint16       version, 
   void**             ppo
)
{

   int AEEBTSPPServer_New( IShell *pIShell, void **ppif,  
                           const uint8* pszServiceName, 
                           const uint16 version,
                           BTServiceClassUUID uuid);
   return AEEBTSPPServer_New( AEE_GetShell(), ppo, pszServiceName, 
                              version, uuid);

}

/*======================================================================= 
=======================================================================*/
int OEMBT_GetSPPServerClone
( 
   OEMBT*             pme, 
   int16              streamID,  // the stream ID
   void**             ppo
)
{

   int AEEBTSPPServerClone( IShell *pIShell, void **ppif,  
                            int16              streamID);
   return AEEBTSPPServerClone( AEE_GetShell(), ppo, streamID
                             );
}

/*======================================================================= 
=======================================================================*/
int OEMBT_GetSPPClient( OEMBT* pme, void** ppo )
{

   int AEEBTSPPClient_New(IShell *pIShell, void **ppif );
   return AEEBTSPPClient_New( AEE_GetShell(), ppo );

}
/*======================================================================= 
=======================================================================*/
int OEMBT_GetDevDB( OEMBT* pme, void** ppo )
{
   int AEEBTDEVDB_New(IShell *pIShell, void **ppif );
   return AEEBTDEVDB_New(AEE_GetShell(), ppo );
}
/*======================================================================= 
=======================================================================*/
void OEMBT_OnBTEv( OEMBT* pme, AEECallback *pcb )
{
   if ( NULL != pme )
   {
      OEMCriticalSection_Enter( &gOEMBTCritSect );

      if ( pme->bCallNotifierCb != FALSE )
      {
         pme->bCallNotifierCb =  FALSE;
         AEE_SYS_RESUME( pcb );
      }
      else
      {
         // Make sure that its not there in any other list
         CALLBACK_Cancel(pcb);
  
         pcb->pfnCancel   = OEMBT_CancelCallback;
         pcb->pCancelData = (void *)pme;
         pcb->pmc         = NULL;
  
         if( pme->pcbOnBTEv ) 
         {
            pme->pcbOnBTEv->pmc = (void *)pcb;
         }
         pcb->pNext       = pme->pcbOnBTEv;
         pme->pcbOnBTEv   = pcb;
      }

      OEMCriticalSection_Leave(&gOEMBTCritSect);
   }
}


/*=============================================================================
 Get the next READY event for a particular App id : either IBT or IBTSPP
=============================================================================*/
OEMBTEvent* OEMBT_GetReadyEv( bt_app_id_type appId)
{
   OEMBTEvent* pEvt = NULL;
   uint32 tempIndex = 0;

   OEMCriticalSection_Enter( &gOEMBTCritSect );

   if ( (NULL != pgOEMBT) && (pgOEMBT->uReadyIndex != pgOEMBT->uFreeIndex) )
   {   

      tempIndex = pgOEMBT->uReadyIndex;

      while ( tempIndex != pgOEMBT->uFreeIndex )
      {
         OEMBTEvent* pCurEvt = &pgOEMBT->pEvCache[tempIndex];

         if ( (appId == pCurEvt->bt_event.ev_hdr.bt_app_id) &&
              (pCurEvt->uState == BT_ECS_READY) )
         {
            break;
         }
         else
         {
            tempIndex = ( tempIndex + 1 ) % OEMBT_MAX_EV_CACHE;
         }
      }
      
      if ( tempIndex != pgOEMBT->uFreeIndex )
      {
         // found an event for this app id
         pEvt = &pgOEMBT->pEvCache[tempIndex];
         pEvt->uState = BT_ECS_PARSING;
      
         // Increment the Ready index only if the oldest Ready event 
         // has been serviced.
         if ( tempIndex == pgOEMBT->uReadyIndex )
         {
            // Oldest ready event. So increment the ready pointer to the 
            // next ready event
            while ( tempIndex != pgOEMBT->uFreeIndex )
            {
               if ( pgOEMBT->pEvCache[tempIndex].uState == BT_ECS_READY  )
               {
                  // found the next ready event;
                  break;
               }
               else
               {
                  tempIndex = ( tempIndex + 1 ) % OEMBT_MAX_EV_CACHE;
               }
            }

            pgOEMBT->uReadyIndex = tempIndex;

         }
      }
      
   }

   OEMCriticalSection_Leave(&gOEMBTCritSect);

   return pEvt;
}

/*=============================================================================
 Get a free event pointer from the cache.Used by IBT and IBTSPP
=============================================================================*/
OEMBTEvent* OEMBT_GetFreeEv( void )
{

   OEMBTEvent* pEvt =  NULL;
   uint32 nextFreeIndx = 0;

   OEMCriticalSection_Enter( &gOEMBTCritSect );
   
   if ( NULL != pgOEMBT )
   {

      nextFreeIndx = pgOEMBT->uFreeIndex;
   
      MSG_MED( "IBT OEMBT_GetFreeEv rd idx 0x%x, fr idx 0x%x",
         pgOEMBT->uReadyIndex, pgOEMBT->uFreeIndex, 0 );

      pEvt =  &pgOEMBT->pEvCache[pgOEMBT->uFreeIndex];
      pEvt->uTimeStamp = pgOEMBT->uTimeStamp;
      pgOEMBT->uTimeStamp++;
  
      nextFreeIndx = (pgOEMBT->uFreeIndex + 1) % OEMBT_MAX_EV_CACHE;

      if ( ( nextFreeIndx == pgOEMBT->uReadyIndex ) || 
           ( pgOEMBT->pEvCache[nextFreeIndx].uState == BT_ECS_PARSING))
      {
         // the next event should either be in FREE or in DISPATCHING state
         MSG_ERROR(" IBT Event cache full: Dropping events ",0,0,0);
      }
      else
      {
         pgOEMBT->uFreeIndex = nextFreeIndx;
      }

      // if it was in the dispatching state.It will be the 
      // oldest event that was not consumed.
      pEvt->uState = BT_ECS_PARSING;
   }
   
   OEMCriticalSection_Leave(&gOEMBTCritSect);
 
   return pEvt;
}

/*======================================================================= 
 Free the event in the cache
=======================================================================*/
void OEMBT_FreeEv( OEMBTEvent* pEvt )
{
   OEMCriticalSection_Enter( &gOEMBTCritSect );

   pEvt->uState = BT_ECS_FREE;

   OEMCriticalSection_Leave(&gOEMBTCritSect);
}


/*======================================================================= 
 Callback called by BT task to deliver RM events.Since we register for
 DB events for all app ids, we would get duplicate events.
=======================================================================*/
static void OEMBT_EvCbRm( bt_ev_msg_type* ev_ptr )
{

   MSG_MED( "IBT OEMBT_EvCbRm ev 0x%x ,app id 0x%x",
             ev_ptr->ev_hdr.ev_type, ev_ptr->ev_hdr.bt_app_id, 0 );
   
   if ( ev_ptr->ev_hdr.ev_type != BT_EV_RM_DEVICE_UPDATED )
   {
      OEMBT_EvCb ( ev_ptr );
   }

}

/*======================================================================= 
 Callback called by BT task
=======================================================================*/
static void OEMBT_EvCb( bt_ev_msg_type* ev_ptr )
{

   OEMBTEvent* pEv;
  

   if ( ( NULL == pgOEMBT ) || // no BT object 
        ( FALSE == pgOEMBT->bNotifierReg ))
   {

      MSG_HIGH( "IBT OEMBT_EvCb pgOEMBT = 0x%x ", pgOEMBT, 0, 0 );
      return; // nothing to do
   }

   MSG_MED( "IBT OEMBT_EvCb called.ev = 0x%x ,app id 0x%x, IBT app id 0x%x",
             ev_ptr->ev_hdr.ev_type, ev_ptr->ev_hdr.bt_app_id, pgOEMBT->appId );

   switch ( ev_ptr->ev_hdr.ev_type)
   {
      case BT_EV_RM_PIN_REQUEST:
      case BT_EV_RM_DEVICE_UPDATED:
      {
         if ( (pEv = OEMBT_GetFreeEv()) != NULL )
         {
            pEv->bt_event = *ev_ptr;
            
            // In case of the BT_EV_RM_DEVICE_UPDATED event,
            // This event could have been generated because some other app 
            // initiated the change. But when servicing the event from cache,
            // this event belongs to IBT. 
            pEv->bt_event.ev_hdr.bt_app_id = pgOEMBT->appId;
            pEv->uState   = BT_ECS_READY;

            OEMCriticalSection_Enter( &gOEMBTCritSect );

            if ( pgOEMBT->pcbOnBTEv )
            { 
               AEE_SYS_RESUME( pgOEMBT->pcbOnBTEv );
            }
            else
            {
               pgOEMBT->bCallNotifierCb = TRUE;
            }

            OEMCriticalSection_Leave(&gOEMBTCritSect);
         }
         break;
      }
      case BT_EV_GN_CMD_DONE:
      default:
         break;
   }

}

//==========================================================================
//   static Funtions
//==========================================================================
static void OEMBT_InitMe( void )
{
   pgOEMBT->uReadyIndex = 0;
   pgOEMBT->uFreeIndex = 0;
   
}

/*======================================================================= 
=======================================================================*/
static void OEMBT_CancelCallback( AEECallback *pcb )
{
   OEMBT *pme = (OEMBT *)pcb->pCancelData;
   AEECallback *pPrev = (AEECallback *)pcb->pmc;
   
   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMBT_CancelCallback)
      return;
   
   if ( pPrev )
   {
      pPrev->pNext = pcb->pNext;
   }
   else
   {
      pme->pcbOnBTEv = pcb->pNext;
   }
   if ( pcb->pNext )
   {
      pcb->pNext->pmc = (void *)pPrev;
   }

   pcb->pCancelData = NULL;
   pcb->pfnCancel   = NULL;
}


/*======================================================================= 
=======================================================================*/
static BTEvent OEMBT_BT2BREWEvent( bt_event_type bt_ev )
{
   switch ( bt_ev )
   {
      case BT_EV_RM_PIN_REQUEST:
         return AEEBT_EV_PIN_REQ;
      case BT_EV_RM_DEVICE_UPDATED:
         return AEEBT_EV_DEV_UPDATE;
      
      default:
         return AEEBT_EV_UNSUPPORTED;
   }
}

/*======================================================================= 
=======================================================================*/
static BTModDevField OEMBT_BT2BREWStatus( bt_rm_dev_update_status_type stat )
{
   switch(stat)
   {
      case BT_RM_DUS_UNBONDED_B:
      case BT_RM_DUS_BONDED_B:
         return AEEBT_MDF_BONDED_B;
      case BT_RM_DUS_SECURITY_B:
         return AEEBT_MDF_SEC_B;
      case BT_RM_DUS_NICK_NAME_B:
         return AEEBT_MDF_NICKNAME_B;
      case BT_RM_DUS_VALUE_1_B:      
         return AEEBT_MDF_VAL1_B;
      case BT_RM_DUS_VALUE_2_B:
         return AEEBT_MDF_VAL2_B;
      case BT_RM_DUS_VALUE_3_B:
         return AEEBT_MDF_VAL3_B;
      case BT_RM_DUS_VALUE_4_B:
         return AEEBT_MDF_VAL4_B;
      case BT_RM_DUS_SERVICES_B:
         return AEEBT_MDF_SVCS_B;
      case BT_RM_DUS_ADDED_B:
      case BT_RM_DUS_REMOVED_B:      
         return AEEBT_MDF_VALID_B;
      case BT_RM_DUS_NAME_B:
         return AEEBT_MDF_NAME_B;
      default :
         return 0;
   }
}



/*=============================================================================
   Notifier implementation
=============================================================================*/
int OEMBTNotifier_GetBTEv( BTNotifyInfo* pNotif )
{

   int nErr = EFAILED;

   OEMBTEvent* pEv = NULL;

   if ( pgOEMBT != NULL )
   {
      pEv = OEMBT_GetReadyEv( pgOEMBT->appId );

      if ( pEv != NULL )
      {
         pNotif->event = 
                  OEMBT_BT2BREWEvent( pEv->bt_event.ev_hdr.ev_type );
         if ( pNotif->event != AEEBT_EV_UNSUPPORTED )
         {
            pEv->uState        = BT_ECS_DISPATCHING;
            pNotif->uTimeStamp = pEv->uTimeStamp;
            nErr = SUCCESS;
         }
      }
   }

   return nErr;
}


/*=============================================================================
FILE:  OEMBT.c

SERVICES:  Implementation of IBT,
  This file contains a reference implementation of BT support.

GENERAL DESCRIPTION:
   Implements IBT, IBTNotifier.

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBT.c#5 $ 
$DateTime: 2009/05/12 17:41:25 $
$Author: c_bhatg $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2009-05-11  gb   Properly fixing the crash during the OEMBT object 
                 cleanup.
2009-05-11  gb   Fixed crash during the release of OEMBT object 
                 by not invoking SetBondable() API after pgOEMBT is set to NULL 
2009-04-16  pn   Fixed compiler error.
2009-04-09  NM   Tear down SCO if BT Headset is not in use
2009-03-31  NM   Custom interface to set/clear bondable
2009-03-06  vk   Featurizing AV profiles
2008-10-02  rs   Removing compiler and Lint warnings.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "bt.h"
#include "OEMHeap.h"
#include "OEMCriticalSection.h"
#include "AEEStdErr.h"
#include "AEEBT.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "OEMBT.h"
#include "OEMBT_priv.h"
#include "btsd.h"
#ifndef BT_SIMULATION
#include "voc.h"
#include "msg.h"
#include "OEMNV.h"
#else
#include "btmsg.h"
#include "bt_sim_interface.h"
#endif
#include "OEMBTAudio.h"
#include "AEETelephone.h"
#include "uixsnd.h"

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }
#define OEMBT_MAX_EV_CACHE 30

#define OEMBT_INVALID_CONN_HANDLE   0xFFFF
#define OEMBT_MAX_NUM_CONN          3

typedef struct
{
   bt_bd_addr_type   bd_addr;
   bt_rm_handle_type handle;
}  OEMBTConnection;

typedef struct
{
   SoundChangeCallbackFn*  pAGCbFunc;
   SoundChangeCallbackFn*  pA2DPCbFunc;
   boolean                 bAGDevReady;
   boolean                 bA2DPDevReady;
   OEMBTSoundType          dCurrSoundType;
   boolean                 bDTMFPlaying;
   boolean                 bDevChange;
   boolean                 bSndDevNotifyReg;
   uint32                  uFlag;
#ifdef FEATURE_AVS_BT_SCO_REWORK
   uint32                  audioType;
#endif

}  OEMBTAudio;

/*=============================================================================
  OEMBT Definition
=============================================================================*/
struct _OEMBT
{
   uint32            uRefs;
   bt_app_id_type    appId;
   OEMBTConnection   connectionList[ OEMBT_MAX_NUM_CONN ];
   OEMBTAudio        audioMgr;

   OEMBTEvent*       pEvCache;
   uint32            uReadyIndex;
   uint32            uFreeIndex;
   // callback of BTNotifier
   AEECallback*      pcbOnBTEv;
   boolean           bNotifierReg;
   boolean           bCallNotifierCb;
   boolean           bondable;
   uint32            uTimeStamp;
#ifdef FEATURE_AVS_BT_SCO_REWORK
   AEECallback       sndChangedCb; 
#endif

};

//==========================================================================
//   Static data
//==========================================================================
static OEMBT* pgOEMBT;
static OEMCriticalSection gOEMBTCritSect;


/************************************************************************ 
                       Private functions
************************************************************************/
/*======================================================================= 
 Executes in BT task context
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
      case BT_EV_RM_CONNECTED_ACL:
      case BT_EV_RM_DISCONNECTED_ACL:
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
      case  BT_EV_AG_AUDIO_CONNECTED:
      {
         OEMCriticalSection_Enter( &gOEMBTCritSect );
         pgOEMBT->audioMgr.uFlag |= OEMBT_STATE_AUDIO_CONNECTED;
         if ( ( pgOEMBT->audioMgr.uFlag & OEMBT_STATE_BRINGING_UP_AUDIO ) != 0 )
         {
            pgOEMBT->audioMgr.uFlag &= ~OEMBT_STATE_BRINGING_UP_AUDIO;
         }
         OEMCriticalSection_Leave(&gOEMBTCritSect);
         break;
      }
      case BT_EV_AG_AUDIO_CONNECT_FAILED:
      {
         MSG_HIGH( "IBT OEMBT_EvCb: Connect Failed!!", 0, 0, 0 );
      }
      case BT_EV_AG_AUDIO_DISCONNECTED:
      {
         OEMCriticalSection_Enter( &gOEMBTCritSect );         
         if ( ( pgOEMBT->audioMgr.uFlag & OEMBT_STATE_AUDIO_CONNECTED ) != 0 )
         {
            pgOEMBT->audioMgr.uFlag &= ~OEMBT_STATE_AUDIO_CONNECTED;
         }
         if ( ( pgOEMBT->audioMgr.uFlag & OEMBT_STATE_BRINGING_DOWN_AUDIO ) != 0 )
         {
            pgOEMBT->audioMgr.uFlag &= ~OEMBT_STATE_BRINGING_DOWN_AUDIO;
         }
         if ( ( pgOEMBT->audioMgr.uFlag & OEMBT_STATE_BRINGING_UP_AUDIO ) != 0 )
         {
            pgOEMBT->audioMgr.uFlag &= ~OEMBT_STATE_BRINGING_UP_AUDIO;
         }
         OEMCriticalSection_Leave(&gOEMBTCritSect);
         break;
      }
      case BT_EV_AG_CONNECTED:
      {
         OEMCriticalSection_Enter( &gOEMBTCritSect );
          pgOEMBT->audioMgr.uFlag |= OEMBT_STATE_AG_CONNECTED;
         OEMCriticalSection_Leave(&gOEMBTCritSect);
         break;
      }

      case BT_EV_AG_CONNECTION_FAILED:
      case BT_EV_AG_DISCONNECTED:
      {
         OEMCriticalSection_Enter( &gOEMBTCritSect );
         if ( ( pgOEMBT->audioMgr.uFlag & OEMBT_STATE_AG_CONNECTED ) != 0 )
         {
            pgOEMBT->audioMgr.uFlag &= ~OEMBT_STATE_AG_CONNECTED;
         }
         OEMCriticalSection_Leave(&gOEMBTCritSect);
         break;
      }
      case BT_EV_GN_CMD_DONE:
      default:
         break;
   }

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

static void OEMBT_InitMe( void )
{
   pgOEMBT->uReadyIndex = 0;
   pgOEMBT->uFreeIndex  = 0;
}

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
      case BT_EV_RM_CONNECTED_ACL:
         return AEEBT_EV_CONN_UP;
      case BT_EV_RM_DISCONNECTED_ACL:
         return AEEBT_EV_CONN_DOWN;
      default:
         return AEEBT_EV_UNSUPPORTED;
   }
}

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

static AEETCallState OEMBT_GetCallState (void)
{
   IShell* piShell = AEE_GetShell();
   ICall* piCall = NULL;
   ICallMgr* piCallMgr = NULL;
   AEETCallState callState = AEET_CALL_STATE_NONE;

   if (ISHELL_CreateInstance (piShell, AEECLSID_CALLMGR,
                              (void**)&piCallMgr) != SUCCESS)
   {
     DBGPRINTF_ERROR ("GetCallState - ICallMgr create instance failed");
   }
   else
   {
      if (ICALLMGR_GetActiveVoiceCall (piCallMgr, &piCall) == SUCCESS)
      {
         callState = ICALL_GetState (piCall);
         ICALL_Release (piCall);
      }
      ICALLMGR_Release (piCallMgr);
   }
   return callState;
}

static void OEMBTAudio_ClearPCMPath (snd_device_type snd_dev)
{
#ifndef BT_SIMULATION
    boolean bIsAttached = uisnd_is_device_attached (snd_dev);

   DBGPRINTF ("OEMBTAudio_ClearPCMPath - dev=%d attached=%d", 
              snd_dev, bIsAttached);

   if (bIsAttached != FALSE)
   {
      uisnd_set_device_status (snd_dev, UISND_DEV_UNATTACHED);
   }

   if (ISHELL_SendURL (AEE_GetShell(), 
                       (SND_DEVICE_BT_HEADSET == snd_dev) ? 
                       "soundman:deselect?dev=BTHeadset" :
                       "soundman:deselect?dev=BTStereoHeadset") == FALSE)
   {
      uisnd_set_device_auto (NULL,NULL);
      DBGPRINTF ("OEMBTAudio_ClearPCMPath - via uisnd");
   }
#endif
}

static void OEMBTAudio_SetPCMPath (snd_device_type snd_dev)
{
#ifndef BT_SIMULATION
   boolean bIsEnabled = uisnd_is_device_enabled (snd_dev);

   DBGPRINTF ("OEMBTAudio_SetPCMPath - dev=%d enabled=%d", snd_dev, bIsEnabled);

   if (bIsEnabled == FALSE)
   {
      uisnd_set_device_status (snd_dev, UISND_DEV_ENABLED);
   }

   if (ISHELL_SendURL (AEE_GetShell(),
                       (SND_DEVICE_BT_HEADSET == snd_dev) ? 
                       "soundman:select?dev=BTHeadset" :
                       "soundman:select?dev=BTStereoHeadset") == FALSE)
   {
      uisnd_set_device_auto (NULL,NULL);
      DBGPRINTF ("OEMBTAudio_SetPCMPath - via uisnd");
   }
#endif
}

#ifdef FEATURE_AVS_BT_SCO_REWORK
static void OEMBT_SoundTypeChangedBrewCb (void *pData)
{
   OEMBTSoundType newSoundType = OEMBT_SND_TYPE_NONE;
   AEETCallState callState;
   boolean       oldDTMFPlaying;

   if ( pgOEMBT == NULL )
   {
      return;  // no one to handle this
   }

   callState = OEMBT_GetCallState ();
   switch (pgOEMBT->audioMgr.audioType)
   {
      case VOC_PLAYBACK_STOP:
         newSoundType = OEMBT_SND_TYPE_NONE;
#ifdef FEATURE_BT_EXTPF_AV
         if (((pgOEMBT->audioMgr.dCurrSoundType == OEMBT_SND_TYPE_BT_OTHER) ||
              (pgOEMBT->audioMgr.dCurrSoundType == OEMBT_SND_TYPE_BT_ALL)) &&
             (pgOEMBT->audioMgr.bA2DPDevReady))
         {
            (void) bt_cmd_pf_a2dp_suspend (pgOEMBT->appId);
         }
#endif /* FEATURE_BT_EXTPF_AV */
         break;
      case VOC_PLAYBACK_OTHER:
         newSoundType = OEMBT_SND_TYPE_OTHER_START;
         break;
      case VOC_PLAYBACK_OTHER_STOP:
         newSoundType = OEMBT_SND_TYPE_OTHER_STOP;
         break;
      case VOC_PLAYBACK_SCO:
         newSoundType = OEMBT_SND_TYPE_BT_SCO;
         break;
      case VOC_PLAYBACK_A2DP:
         newSoundType = OEMBT_SND_TYPE_BT_OTHER;
         break;
      case (VOC_PLAYBACK_SCO | VOC_PLAYBACK_A2DP):
         newSoundType = OEMBT_SND_TYPE_BT_ALL;
         break;
      case VOC_PLAYBACK_DTMF_START:
         newSoundType = OEMBT_SND_TYPE_DTMF_START;
         break;
      case VOC_PLAYBACK_DTMF_STOP:
         newSoundType = OEMBT_SND_TYPE_DTMF_STOP;
         break;
      case VOC_PLAYBACK_DTMF_RINGER_START:
         newSoundType = OEMBT_SND_TYPE_DTMF_RING_START;
         break;
      case VOC_PLAYBACK_DTMF_RINGER_STOP:
         newSoundType = OEMBT_SND_TYPE_DTMF_RING_STOP;
         break;
      default:
         DBGPRINTF_ERROR ("SndTypeChg - unexp type 0x%04x",
               pgOEMBT->audioMgr.audioType);
         return;
   }

   oldDTMFPlaying = pgOEMBT->audioMgr.bDTMFPlaying;
   if (OEMBT_SND_TYPE_DTMF_START == newSoundType)
   {
      DBGPRINTF ("SndTypeChg - DTMF Start" );
      pgOEMBT->audioMgr.bDTMFPlaying = TRUE;
   }
   else if (OEMBT_SND_TYPE_DTMF_STOP  == newSoundType)
   {
      if( oldDTMFPlaying == FALSE )
      {
         DBGPRINTF ("SndTypeChg - DTMF Stop, but not playing");
      }
      else
      {
         DBGPRINTF ("SndTypeChg - DTMF Stop" );
      }
      pgOEMBT->audioMgr.bDTMFPlaying = FALSE;
   }
   else
   {
      DBGPRINTF ("SndTypeChg - then=%d now=%d callSt=0x%08x", 
                 pgOEMBT->audioMgr.dCurrSoundType, newSoundType, callState);
      pgOEMBT->audioMgr.dCurrSoundType = newSoundType;
   }

   if ((callState & AEET_CALL_STATE_INCOM) != 0)
   {
      OEMBTAudio_ClearPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
      if ( pgOEMBT->audioMgr.pAGCbFunc != NULL )
      {
         (pgOEMBT->audioMgr.pAGCbFunc)(newSoundType);
      }
   }
#ifdef FEATURE_BT_EXTPF_AV
   else if ((pgOEMBT->audioMgr.bA2DPDevReady)             &&
            (((newSoundType == OEMBT_SND_TYPE_BT_OTHER)||
              (newSoundType == OEMBT_SND_TYPE_BT_ALL))     ||
             ((newSoundType == OEMBT_SND_TYPE_DTMF_STOP)&&
              (oldDTMFPlaying == TRUE)  &&
              ((pgOEMBT->audioMgr.dCurrSoundType == OEMBT_SND_TYPE_BT_OTHER)||
               (pgOEMBT->audioMgr.dCurrSoundType == OEMBT_SND_TYPE_BT_ALL)))))
   {
      (void) bt_cmd_pf_a2dp_start (pgOEMBT->appId, NULL);
      OEMBTAudio_SetPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
   }
#endif /* FEATURE_BT_EXTPF_AV */
   else if ( pgOEMBT->audioMgr.pAGCbFunc != NULL )
   {
     (pgOEMBT->audioMgr.pAGCbFunc)(newSoundType);
   }

   if ( pgOEMBT->audioMgr.pA2DPCbFunc != NULL )
   {
      (pgOEMBT->audioMgr.pA2DPCbFunc)(newSoundType);
   }
}
static void OEMBT_SoundTypeChanged (uint32 audio_type)
{
   if ( pgOEMBT != NULL )
   {
      pgOEMBT->audioMgr.audioType = audio_type;
      CALLBACK_Cancel(&pgOEMBT->sndChangedCb);
      CALLBACK_Init(&pgOEMBT->sndChangedCb, OEMBT_SoundTypeChangedBrewCb, pgOEMBT);
      AEE_SYS_RESUME(&pgOEMBT->sndChangedCb);
   }
   return;
}
#endif


/************************************************************************ 
                       Exposed to AEE layer
************************************************************************/
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
         for ( i = 0; i < ARR_SIZE(pgOEMBT->connectionList); i++ )
         {
            pgOEMBT->connectionList[ i ].handle = OEMBT_INVALID_CONN_HANDLE;
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

         pgOEMBT->audioMgr.bSndDevNotifyReg = FALSE;
#ifdef BT_SIMULATION
         bt_sim_start(PROFILE_GAP, FALSE);
#endif

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

         bt_cmd_ec_reg_event_set_any_app_id( pgOEMBT->appId,
                                             OEMBT_EvCb,
                                             BT_EC_ES_CUSTOM,
                                             BT_EV_RM_CONNECTED_ACL,
                                             BT_EV_RM_CONNECTED_ACL );

         bt_cmd_ec_reg_event_set_any_app_id( pgOEMBT->appId,
                                             OEMBT_EvCb,
                                             BT_EC_ES_CUSTOM,
                                             BT_EV_RM_DISCONNECTED_ACL,
                                             BT_EV_RM_DISCONNECTED_ACL );
         bt_cmd_ec_reg_event_set_any_app_id ( pgOEMBT->appId,
                                           OEMBT_EvCb,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_AG_AUDIO_CONNECTED, 
                                           BT_EV_AG_AUDIO_CONNECT_FAILED );

         bt_cmd_ec_reg_event_set_any_app_id ( pgOEMBT->appId,
                                           OEMBT_EvCb,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_AG_CONNECTED, 
                                           BT_EV_AG_CONNECTION_FAILED );

         bt_cmd_ec_reg_event_set_any_app_id ( pgOEMBT->appId,
                                           OEMBT_EvCb,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_AG_DISCONNECTED, 
                                           BT_EV_AG_DISCONNECTED );
         
	     OEMBT_SetBondable( TRUE );

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
      OEMBT_SetBondable( FALSE ); 
      pgOEMBT = NULL;

      if ( pme->pcbOnBTEv != NULL )
      {
         CALLBACK_Cancel( pme->pcbOnBTEv );
      }
#ifdef FEATURE_AVS_BT_SCO_REWORK
      CALLBACK_Cancel( &pme->sndChangedCb );
#endif
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

      bt_cmd_ec_unreg_event_set_any_app_id( pme->appId,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_RM_CONNECTED_ACL,
                                           BT_EV_RM_CONNECTED_ACL );

      bt_cmd_ec_unreg_event_set_any_app_id( pme->appId,
                                           BT_EC_ES_CUSTOM,
                                           BT_EV_RM_DISCONNECTED_ACL,
                                           BT_EV_RM_DISCONNECTED_ACL );

      bt_cmd_ec_unreg_event_set_any_app_id ( pme->appId,                  
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_AG_AUDIO_CONNECTED, 
                                         BT_EV_AG_AUDIO_CONNECT_FAILED );

      bt_cmd_ec_unreg_event_set_any_app_id ( pme->appId,                  
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_AG_CONNECTED, 
                                         BT_EV_AG_CONNECTION_FAILED );

      bt_cmd_ec_unreg_event_set_any_app_id ( pme->appId,
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_AG_DISCONNECTED, 
                                         BT_EV_AG_DISCONNECTED );

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
            case AEEBT_EV_CONN_UP:
            {
               pED->bdAddr = *((BDAddress*)&pEv->bt_event.ev_msg.
                               ev_rm_conna.bd_addr);
               for ( i = 0; i < ARR_SIZE (pgOEMBT->connectionList); i++ )
               {
                  if ( pgOEMBT->connectionList[ i ].handle == 
                       OEMBT_INVALID_CONN_HANDLE )
                  {
                     pgOEMBT->connectionList[ i ].handle =
                        pEv->bt_event.ev_msg.ev_rm_conna.handle;
                     pgOEMBT->connectionList[ i ].bd_addr = 
                        pEv->bt_event.ev_msg.ev_rm_conna.bd_addr;
                     break;
                  }
               }
               break;
            }
            case AEEBT_EV_CONN_DOWN:
            {
               for ( i = 0; i < ARR_SIZE (pgOEMBT->connectionList); i++ )
               {
                  if ( pgOEMBT->connectionList[ i ].handle == 
                       pEv->bt_event.ev_msg.ev_rm_conna.handle )
                  {
                     pED->bdAddr = 
                        *((BDAddress*)&pgOEMBT->connectionList[ i ].bd_addr);
                     pgOEMBT->connectionList[ i ].handle =
                        OEMBT_INVALID_CONN_HANDLE;
                     break;
                  }
               }
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
int OEMBT_GetRadioState( BTRadioState* pdRadioState )
{
   int nErr = EFAILED;

   if ( pgOEMBT != NULL )
   {
      bt_cmd_status_type      stat;
      bt_radio_activity_type  radio_activity;
      stat = bt_cmd_rm_get_radio_activity( pgOEMBT->appId, 
                                           &radio_activity );
      if (BT_CS_GN_SUCCESS == stat)
      {
         nErr = SUCCESS;
         *pdRadioState = 0;
         if ( radio_activity & BT_RA_RADIO_OFF )
         {
           *pdRadioState |= AEEBT_RADIO_OFF;
         }
         if ( radio_activity & BT_RA_INQUIRING )
         {
           *pdRadioState |= AEEBT_RADIO_INQUIRING;
         }
         if ( radio_activity & BT_RA_PAGING )
         {
           *pdRadioState |= AEEBT_RADIO_PAGING;
         }
         if ( radio_activity & BT_RA_INQ_SCANNING )
         {
           *pdRadioState |= AEEBT_RADIO_INQ_SCANNING;
         }
         if ( radio_activity & BT_RA_PAGE_SCANNING )
         {
           *pdRadioState |= AEEBT_RADIO_PAGE_SCANNING;
         }
      }
   }
   return nErr;
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

/************************************************************************ 
                       Exposed to other OEMBT
************************************************************************/
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
 Registers Sound Change callback function with AVS
=======================================================================*/
int OEMBTAudio_RegisterSoundChangeCb (
   SoundChangeCallbackFn* pCbFunc, 
   OEMBTSoundDevType sndDevType)
{
#ifdef BT_SIMULATION
   int result = AEE_SUCCESS;
#else
   int result = AEE_EUNSUPPORTED;

#ifdef FEATURE_AVS_BT_SCO_REWORK
   boolean bAreadyReg = FALSE;
   result = AEE_EALREADY;

   OEMCriticalSection_Enter( &gOEMBTCritSect );
   if ((NULL != pgOEMBT->audioMgr.pAGCbFunc) || 
       (NULL != pgOEMBT->audioMgr.pA2DPCbFunc))
   {
      bAreadyReg = TRUE;
   }
   if (OEMBT_SND_DEV_TYPE_AG == sndDevType)
   {
      pgOEMBT->audioMgr.pAGCbFunc   = pCbFunc;
      result = AEE_SUCCESS;
   }
   else
   {
      pgOEMBT->audioMgr.pA2DPCbFunc = pCbFunc;
      result = AEE_SUCCESS;
   }
   if ((FALSE == bAreadyReg) && (AEE_SUCCESS == result))
   {
      snd_set_bt_cb_func (OEMBT_SoundTypeChanged);
   }
   OEMCriticalSection_Leave(&gOEMBTCritSect);
#endif // FEATURE_AVS_BT_SCO_REWORK
#endif // BT_SIMULATION
   return result;
}

/*======================================================================= 
 Deregisters Sound Change callback function
=======================================================================*/
int OEMBTAudio_DeregisterSoundChangeCb (SoundChangeCallbackFn* pCbFunc)
{
#ifdef BT_SIMULATION
   int result = AEE_SUCCESS;
#else
   int result = AEE_EUNSUPPORTED;

#ifdef FEATURE_AVS_BT_SCO_REWORK
   OEMCriticalSection_Enter( &gOEMBTCritSect );
   if (pCbFunc == pgOEMBT->audioMgr.pAGCbFunc)
   {
      pgOEMBT->audioMgr.pAGCbFunc = NULL;
      result = AEE_SUCCESS;
   }
   else if (pCbFunc == pgOEMBT->audioMgr.pA2DPCbFunc)
   {
      pgOEMBT->audioMgr.pA2DPCbFunc = NULL;
      result = AEE_SUCCESS;
   }
   else
   {
      result = AEE_EBADSTATE;
   }

   if (AEE_SUCCESS == result)
   {
      if ((NULL == pgOEMBT->audioMgr.pAGCbFunc) && 
          (NULL == pgOEMBT->audioMgr.pA2DPCbFunc))
      {
         snd_set_bt_cb_func (NULL);
      }
   }
   OEMCriticalSection_Leave(&gOEMBTCritSect);
#endif // FEATURE_AVS_BT_SCO_REWORK
#endif // BT_SIMULATION
   return result;
}

/*======================================================================= 
 Initializes volume settings for a particular sound device type
=======================================================================*/
void OEMBTAudio_InitializeVolumes (OEMBTSoundDevType sndDevType)
{
#ifndef BT_SIMULATION
   nv_item_type nvi;
   snd_device_type snd_dev = 
      (sndDevType == OEMBT_SND_DEV_TYPE_AG) ?
      SND_DEVICE_BT_HEADSET : SND_DEVICE_BT_A2DP_HEADSET;

   // beep volume
   if (NV_NOTACTIVE_S == OEMNV_Get (NV_BEEP_LVL_SHADOW_I, &nvi))
   {
      nvi.beep_lvl_shadow = UISND_2ND_VOL;
      (void) OEMNV_Put (NV_BEEP_LVL_SHADOW_I, &nvi);
   }
   snd_set_volume (snd_dev, SND_METHOD_KEY_BEEP, 
                   nvi.beep_lvl_shadow, NULL, NULL);

   // voice volume
   if (NV_NOTACTIVE_S == OEMNV_Get (NV_EAR_LVL_SHADOW_I, &nvi))
   {
      nvi.ear_lvl_shadow = UISND_2ND_VOL;
      (void) OEMNV_Put (NV_EAR_LVL_SHADOW_I, &nvi);
   }
   snd_set_volume (snd_dev, SND_METHOD_VOICE, 
                   nvi.ear_lvl_shadow, NULL, NULL);

   // message volume
   if (NV_NOTACTIVE_S == OEMNV_Get (NV_ALERTS_LVL_SHADOW_I, &nvi))
   {
      nvi.alerts_lvl_shadow = UISND_2ND_VOL;
      (void) OEMNV_Put (NV_ALERTS_LVL_SHADOW_I, &nvi);
   }
   snd_set_volume (snd_dev, SND_METHOD_MESSAGE, 
                   nvi.alerts_lvl_shadow, NULL, NULL);

   // ring volume
   if (NV_NOTACTIVE_S == OEMNV_Get (NV_RINGER_LVL_I, &nvi))
   {
      nvi.ringer_lvl_shadow = UISND_2ND_VOL;
      (void) OEMNV_Put (NV_RINGER_LVL_I, &nvi);
   }
   snd_set_volume (snd_dev, SND_METHOD_RING, 
                   nvi.ringer_lvl_shadow, NULL, NULL);

#ifdef FEATURE_MULTIMEDIA
   // midi volume
   if (NV_NOTACTIVE_S == OEMNV_Get (NV_MM_LVL_SHADOW_I, &nvi))
   {
      nvi.mm_lvl_shadow = UISND_2ND_VOL;
      (void) OEMNV_Put (NV_MM_LVL_SHADOW_I, &nvi);
   }
   snd_set_volume (snd_dev, SND_METHOD_MIDI, 
                   nvi.mm_lvl_shadow, NULL, NULL);
#endif // FEATURE_MULTIMEDIA

#endif // BT_SIMULATION
}

/*======================================================================= 
 Enables the given sound device type
=======================================================================*/
int OEMBTAudio_EnableDevice (OEMBTSoundDevType sndDevType)
{
   int result = AEE_SUCCESS;
#ifndef BT_SIMULATION
   snd_device_type snd_dev = 
      (sndDevType == OEMBT_SND_DEV_TYPE_AG) ?
      SND_DEVICE_BT_HEADSET : SND_DEVICE_BT_A2DP_HEADSET;
   DBGPRINTF ("OEMBTAudio_EnableDevice - sndDev=%d", sndDevType);
   if (pgOEMBT == NULL)
   {
      result = AEE_EBADSTATE;
   }
#ifdef FEATURE_BT_EXTPF_AV
   else if (SND_DEVICE_BT_A2DP_HEADSET == snd_dev)
   {
      pgOEMBT->audioMgr.bA2DPDevReady = TRUE;
      OEMBTAudio_SetPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
      if ((OEMBT_SND_TYPE_BT_OTHER == pgOEMBT->audioMgr.dCurrSoundType) ||
          (OEMBT_SND_TYPE_BT_ALL == pgOEMBT->audioMgr.dCurrSoundType))
      {
         // Generate a sound stop event for AG in case the SCO link is up
         // to play the current audio.
        
         if ( pgOEMBT->audioMgr.pAGCbFunc != NULL )
         {
            (pgOEMBT->audioMgr.pAGCbFunc)(OEMBT_SND_TYPE_NONE);
         }
         if ( pgOEMBT->audioMgr.pA2DPCbFunc != NULL )
         {
            (pgOEMBT->audioMgr.pA2DPCbFunc)(pgOEMBT->audioMgr.dCurrSoundType);
         }
         
         (void) bt_cmd_pf_a2dp_start (pgOEMBT->appId, NULL);
      }
   }
#endif /* FEATURE_BT_EXTPF_AV */
   else if (SND_DEVICE_BT_HEADSET == snd_dev)
   {
      pgOEMBT->audioMgr.bAGDevReady = TRUE;
      OEMBTAudio_SetPCMPath (SND_DEVICE_BT_HEADSET);
   }
   else
   {
      result = AEE_EBADPARM;
   }

   if (AEE_SUCCESS == result)
   {
      // disable the other one, if any
      if ((FALSE != pgOEMBT->audioMgr.bAGDevReady) &&
          (SND_DEVICE_BT_HEADSET != snd_dev) )
      {
         OEMBTAudio_ClearPCMPath (SND_DEVICE_BT_HEADSET);
      }
      if ((FALSE != pgOEMBT->audioMgr.bA2DPDevReady) &&
          (SND_DEVICE_BT_A2DP_HEADSET != snd_dev) )
      {
         OEMBTAudio_ClearPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
      }
   }
#endif
   return result;
}

/*======================================================================= 
 Disables the given sound device type
=======================================================================*/
int OEMBTAudio_DisableDevice (OEMBTSoundDevType sndDevType)
{
   int result = AEE_SUCCESS;
#ifndef BT_SIMULATION
   snd_device_type snd_dev = 
      (sndDevType == OEMBT_SND_DEV_TYPE_AG) ?
      SND_DEVICE_BT_HEADSET : SND_DEVICE_BT_A2DP_HEADSET;
   DBGPRINTF ("OEMBTAudio_DisableDevice - sndDev=%d", sndDevType);
   if (pgOEMBT == NULL)
   {
      result = AEE_EBADSTATE;
   }
   else if (SND_DEVICE_BT_A2DP_HEADSET == snd_dev)
   {
      pgOEMBT->audioMgr.bA2DPDevReady = FALSE;
      OEMBTAudio_ClearPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
   }
   else if (SND_DEVICE_BT_HEADSET == snd_dev)
   {
      pgOEMBT->audioMgr.bAGDevReady = FALSE;
      OEMBTAudio_ClearPCMPath (SND_DEVICE_BT_HEADSET);
   }
   else
   {
      result = AEE_EBADPARM;
   }

   if (AEE_SUCCESS == result)
   {
      // restore the other one, if any
      if (FALSE != pgOEMBT->audioMgr.bAGDevReady)
      {
         OEMBTAudio_SetPCMPath (SND_DEVICE_BT_HEADSET);
      }
      if (FALSE != pgOEMBT->audioMgr.bA2DPDevReady)
      {
         OEMBTAudio_SetPCMPath (SND_DEVICE_BT_A2DP_HEADSET);
      }
   }
#endif // BT_SIMULATION
   return result;
}

/*======================================================================= 
 Sets to bondable mode
=======================================================================*/
int OEMBT_SetBondable( boolean bondable )
{
   int nErr = EFAILED;

   bt_cmd_status_type  stat;
      
   stat = bt_cmd_rm_set_bondable( pgOEMBT->appId, bondable );
   if ( ( BT_CS_GN_SUCCESS == stat ) || ( BT_CS_GN_PENDING == stat ) )
   {
      pgOEMBT->bondable = bondable;
      nErr = SUCCESS;
   }
   DBGPRINTF ("OEMBT_SetBondable - %d, stat = %d ", bondable, nErr);

   return nErr;
}

/*======================================================================= 
 Get bondable mode
=======================================================================*/
int OEMBT_GetBondable( boolean* bondable )
{
   int nErr = EFAILED;

   if ( bondable != NULL )
   {
      nErr = SUCCESS;
	  *bondable = pgOEMBT->bondable;
   }
   DBGPRINTF ("OEMBT_GetBondable - %d, stat = %d ", pgOEMBT->bondable, nErr);

   return nErr;
}

#ifndef BT_SIMULATION
/*======================================================================= 
 Device change callback
=======================================================================*/
static void OEMBTAudio_DeviceChangeCb( OEMBT* pMe,
                                  const uisnd_notify_data_s_type* pNotifyData )
{
   snd_device_type snd_dev;

   snd_dev = pNotifyData->out_device;

   DBGPRINTF( "OEMBT DeviceChangeCb - Device or Mute setting Changed!!!" );
   DBGPRINTF( "OEMBT DeviceChangeCb - Current Dev type %x", snd_dev );
   DBGPRINTF( "OEMBT DeviceChangeCb - uFlag = %d", pMe->audioMgr.uFlag);
   DBGPRINTF( "OEMBT DeviceChangeCb - AG Ready = %d A2DP Ready = %d",
               pMe->audioMgr.bAGDevReady, pMe->audioMgr.bA2DPDevReady);

   if ( snd_dev != SND_DEVICE_BT_HEADSET )
   {
      if ( ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AUDIO_CONNECTED ) != 0 )  &&
           ( ( pMe->audioMgr.uFlag & OEMBT_STATE_BRINGING_DOWN_AUDIO ) == 0 ) )           
      {
         /* not a sco device, disconnect sco */
         DBGPRINTF( "OEMBT DeviceChangeCb - Stop SCO!" );
         //disconnected audio as a result of device change
         pMe->audioMgr.bDevChange = TRUE;
         pMe->audioMgr.uFlag |= OEMBT_STATE_BRINGING_DOWN_AUDIO ;
         (void) bt_cmd_ag_audio_disconnect (pMe->appId);
      }
      else
      {
         if ( ( pMe->audioMgr.bDevChange == FALSE ) &&
              ( pMe->audioMgr.bAGDevReady == FALSE ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AG_CONNECTED ) != 0 ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AUDIO_CONNECTED ) == 0 ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_BRINGING_UP_AUDIO ) == 0 ) )
         {
            DBGPRINTF( "OEMBT DeviceChangeCb - Set PCM Path for SCO!" );
            /* enable BT Headset unless it is due to disable AG during audio
               disconnect, on device change
            */
            pMe->audioMgr.bAGDevReady = TRUE;
            OEMBTAudio_SetPCMPath (SND_DEVICE_BT_HEADSET);
         }
         if ( ( pMe->audioMgr.bDevChange == TRUE ) &&
              ( pMe->audioMgr.bAGDevReady == FALSE ) )
         {
            /* PCM path is cleared so reset */
            pMe->audioMgr.bDevChange = FALSE;
         }
      }
   }
   else
   {
      /* BT Headset */
      if ( ( pMe->audioMgr.bAGDevReady == FALSE ) &&
           ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AG_CONNECTED ) != 0 ) &&
           ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AUDIO_CONNECTED ) == 0 ) &&
           ( ( pMe->audioMgr.uFlag & OEMBT_STATE_BRINGING_UP_AUDIO ) == 0 ) )
      {
         DBGPRINTF( "OEMBT DeviceChangeCb - Set PCM Path for SCO!" );
         pMe->audioMgr.bAGDevReady = TRUE;
         OEMBTAudio_SetPCMPath (SND_DEVICE_BT_HEADSET);
      }
      else
      {
         if ( ( pMe->audioMgr.bAGDevReady == TRUE ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AG_CONNECTED ) != 0 ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_AUDIO_CONNECTED ) == 0 ) &&
              ( ( pMe->audioMgr.uFlag & OEMBT_STATE_BRINGING_UP_AUDIO ) == 0 ) )
         {
            DBGPRINTF( "OEMBT DeviceChangeCb - Bringing up audio !" );
            pMe->audioMgr.uFlag |= OEMBT_STATE_BRINGING_UP_AUDIO;
            (void) bt_cmd_ag_audio_connect (pMe->appId, 0);
         }
      }
   }
}
#endif /* BT_SIMULATION */

/*======================================================================= 
 Register device change callback
=======================================================================*/
boolean  OEMBTAudio_RegisterDeviceChangeCb (void)
{
#ifndef BT_SIMULATION
   /* Register callback for sound device changes */
   DBGPRINTF ( "AG - Register Dev Chg CB" );
   if ( ( FALSE == pgOEMBT->audioMgr.bSndDevNotifyReg ) &&
      ( ( pgOEMBT->audioMgr.bSndDevNotifyReg =
          uisnd_notify_register( (UISND_NOTIFY_FUNC)OEMBTAudio_DeviceChangeCb,
                                 (void*)pgOEMBT ) ) != TRUE ) )
   {
      DBGPRINTF ( "AG - Failed to register Dev Chg CB " );
   }
   else
   {
      DBGPRINTF ( "AG - registered Dev Chg CB " );
   }
#endif
   return ( pgOEMBT->audioMgr.bSndDevNotifyReg );
}

/*======================================================================= 
 Deregister device change callback
=======================================================================*/
boolean  OEMBTAudio_DeregisterDeviceChangeCb (void)
{
#ifndef BT_SIMULATION
   /* Deregister Device Change Callback */
   DBGPRINTF ( "AG - Unregister Dev Chg CB" );
   if ( ( TRUE == pgOEMBT->audioMgr.bSndDevNotifyReg ) &&
        ( uisnd_notify_unregister( ( UISND_NOTIFY_FUNC)OEMBTAudio_DeviceChangeCb,
        (void*)pgOEMBT ) == TRUE ) )
   {
      pgOEMBT->audioMgr.bSndDevNotifyReg = FALSE;
      DBGPRINTF ( "AG - Unregistered Dev Chg CB" );
   }
#endif
   return ( pgOEMBT->audioMgr.bSndDevNotifyReg );
}

/*======================================================================= 
 Set Audio State
=======================================================================*/
void OEMBTAudio_SetState (OEMBTAudioState state)
{
   if ( pgOEMBT )
   {
      pgOEMBT->audioMgr.uFlag |= state;
   }
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */

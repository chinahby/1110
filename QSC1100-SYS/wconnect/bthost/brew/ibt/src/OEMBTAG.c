/*=============================================================================
FILE:         OEMBTAG.c

SERVICES:     OEMBTAG implementation

DESCRIPTION:  This file handles the OEM implementation for IBTAudioGateway

===============================================================================
        Copyright (c) 2007-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTAG.c#6 $ 
$DateTime: 2009/04/15 11:34:13 $
$Author: cou $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2009-04-15   co  Removed mute settings for A2DP.
  2009-04-09   NM  Tear down SCO if BT Headset is not in use
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2009-03-10   co  Now try HSP if HFP connection attempt fails.
  2009-01-13   rs  Moved the emptying of external I/O watermark queue functions 
                   from bt to OEM.
  2008-09-19   pn  Klocwork fixes

=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "dsm.h"
#include "bt.h"
#include "btsd.h"
#include "btsio.h"
#ifndef BT_SIMULATION
#include "vbatt.h"
#include "cm.h"
#include "voc.h"
#endif
#include "OEMBT_Priv.h"
#include "OEMBTUtils.h"
#include "OEMBTAG.h"
#include "OEMBTAudio.h"
#include "OEMCriticalSection.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEISignal.h"
#include "AEESound.h"
#ifdef  FEATURE_QVPHONE
#include "AEEBitmap.h"
#include "AEEVideoPhone.h"
#endif
#include "btmsg.h"

#ifdef BT_SIMULATION
#include "bt_sim_interface.h"
#endif
typedef struct
{
   dsm_item_type*       pDSMItem;
   dsm_watermark_type*  pTxWm;
   dsm_watermark_type*  pRxWm;
   q_type               txQ;
   q_type               rxQ;
   ISignal*             piATCmdSignal;
}  AGExternalIO;

typedef struct _AGCoreEvent AGCoreEvent;

struct _AGCoreEvent
{
   AGCoreEvent*         pNext;
   bt_event_type        ev_type;
   bt_ev_params_union   ev_msg;
};

struct _OEMAG
{
  IShell*              piShell;

   // driver interface
   bt_app_id_type       appId;
   OEMAGEvent           eventPool[10];
   OEMAGEvent*          pReadyEvent;
   AGExternalIO         extIO;
   int                  nRMCmdExpected;
   uint32               uFlags;
   uint8                uBatteryLevel;
   uint32               uSndType;
   ISignal*             piEventSignal;
   AEECallback          evSignalCb; 
#ifdef  FEATURE_QVPHONE
   // Video Phone call handling info
   IVideoPhone*         pIVP;
#endif

};

static OEMAG* pgTheOEMAG = NULL;
static OEMCriticalSection gOEMAGCriticalSection;

// set to try HSP if HFP connection attempt fails
#define OEMAG_ATTEMPT_HSP_B     0x00000001

#define OEMAG_LOCK()    OEMCriticalSection_Enter(&gOEMAGCriticalSection)
#define OEMAG_UNLOCK()  OEMCriticalSection_Leave(&gOEMAGCriticalSection)

static BTAGOperationType OEMBTAG_Cmd2Op (bt_cmd_type cmd)
{
   switch (cmd)
   {
      case BT_CMD_AG_ENABLE:
         return AEEBTAG_OP_ENABLE;
      case BT_CMD_AG_DISABLE:
         return AEEBTAG_OP_DISABLE;
      case BT_CMD_AG_CONNECT:
         return AEEBTAG_OP_CONNECT;
      case BT_CMD_AG_DISCONNECT:
         return AEEBTAG_OP_DISCONNECT;
      case BT_CMD_AG_AUDIO_CONNECT:
         return AEEBTAG_OP_CONNECT_AUDIO;
      case BT_CMD_AG_AUDIO_DISCONNECT:
         return AEEBTAG_OP_DISCONNECT_AUDIO;
      case BT_CMD_RM_SET_SERVICE_SECURITY:
         return AEEBTAG_OP_CONFIG;
      default:
         return AEEBTAG_OP_NONE;
   }
}

static void OEMBTAG_CoreEv2OemEv (bt_ev_msg_type* pEv, OEMAGEvent* pOemEv)
{
   switch (pEv->ev_hdr.ev_type)
   {
      case BT_EV_GN_CMD_DONE:
         pOemEv->op = OEMBTAG_Cmd2Op (pEv->ev_msg.ev_gn_cmd_done.cmd_type);
         if (AEEBTAG_OP_NONE != pOemEv->op)
         {
            if (BT_CS_GN_SUCCESS == pEv->ev_msg.ev_gn_cmd_done.cmd_status)
            {
               pOemEv->type = OEMBTAG_EV_OP_CMPLT_SUCCESS;
            }
            else
            {
               pOemEv->type = OEMBTAG_EV_OP_CMPLT_FAILED;
            }
         }
         break;
      case BT_EV_AG_ENABLED:
         pOemEv->type = OEMBTAG_EV_ENABLED;
         break;
      case BT_EV_AG_CONNECTED:
         pgTheOEMAG->uFlags &= ~OEMAG_ATTEMPT_HSP_B;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_connected.bd_addr);
         pOemEv->svcClsID = 
            (pEv->ev_msg.ev_ag_connected.audio_device == BT_AD_HEADSET) ?
            AEEBT_SCU_HEADSET : AEEBT_SCU_HANDSFREE;
         pOemEv->type = OEMBTAG_EV_CONNECTED;
         break;
      case BT_EV_AG_CONNECTION_FAILED:
      {
         bt_cmd_status_type stat = BT_CS_GN_BAD_CMD_STATE;

         if (pgTheOEMAG->uFlags & OEMAG_ATTEMPT_HSP_B)
         {
            // HFP connection attempt failed
            // now try HSP
            DBGPRINTF ("OEMBTAG_CoreEv2OemEv - attempt HSP connection");

            pgTheOEMAG->uFlags &= ~OEMAG_ATTEMPT_HSP_B;
            stat = bt_cmd_ag_connect (
                      pgTheOEMAG->appId,
                      &pEv->ev_msg.ev_ag_connection_failed.bd_addr,
                      BT_AD_HEADSET);
         }

         if ((BT_CS_GN_SUCCESS != stat) &&
             (BT_CS_GN_PENDING != stat))
         {
            pOemEv->type = OEMBTAG_EV_CONNECT_FAILED;
            pOemEv->bdAddr = 
               *((BDAddress*)&pEv->ev_msg.ev_ag_connection_failed.bd_addr);
         }
         break;
      }
      case BT_EV_AG_BUTTON_PRESSED:
         pOemEv->type = OEMBTAG_EV_BUTTON_PRESS;
         break;
      case BT_EV_AG_AUDIO_CONNECTED:
         pOemEv->type = OEMBTAG_EV_AUDIO_CONNECTED;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_audio_connected.bd_addr);
         break;
      case BT_EV_AG_AUDIO_DISCONNECTED:
         pOemEv->type = OEMBTAG_EV_AUDIO_DISCONNECTED;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_audio_disconnected.bd_addr);
         break;
      case BT_EV_AG_AUDIO_CONNECT_FAILED:
         pOemEv->type = OEMBTAG_EV_AUDIO_CONNECT_FAILED;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_audio_connect_failed.bd_addr);
         break;
      case BT_EV_AG_DISCONNECTED:
         pOemEv->type = OEMBTAG_EV_DISCONNECTED;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_disconnected.bd_addr);
         break;
      case BT_EV_AG_DISABLED:
         pOemEv->type = OEMBTAG_EV_DISABLED;
         break;
      case BT_EV_AG_SLC_CHANGED:
         pOemEv->type = pEv->ev_msg.ev_ag_slc_changed.slc_up ? 
            OEMBTAG_EV_SLC_UP : OEMBTAG_EV_SLC_DOWN;
         break;
      case BT_EV_AG_DEV_PICKUP:
         pOemEv->type = OEMBTAG_EV_DEV_PICKUP;
         break;
      case BT_EV_AG_DEV_HANGUP:
         pOemEv->type = OEMBTAG_EV_DEV_HANGUP;
         break;
      case BT_EV_AG_VR_TOGGLE:
         pOemEv->type = pEv->ev_msg.ev_ag_vr_state.vr_enabled ?
            OEMBTAG_EV_VR_ACT : OEMBTAG_EV_VR_DEACT;
         break;
      case BT_EV_AG_DEV_DIAL:
      {
         uint8* src = (uint8*)pEv->ev_msg.ev_ag_dev_dial.phone_num;
         uint8* dst = pOemEv->u8Buf;
         STRLCPY( (char*)dst, (char*)src, sizeof(pOemEv->u8Buf) );
         pOemEv->type = OEMBTAG_EV_DEV_DIAL;
         break;
      }
      case BT_EV_AG_DEV_REDIAL:
         pOemEv->type = OEMBTAG_EV_DEV_REDIAL;
         break;
      case BT_EV_AG_SPKR_GAIN_REPORT:
         pOemEv->type = OEMBTAG_EV_SPKR_GAIN;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_ad_spkr_gain_report.bd_addr);
         pOemEv->u8Buf[0] = 
            pEv->ev_msg.ev_ag_ad_spkr_gain_report.ad_speaker_gain;
         break;
      case BT_EV_AG_MIC_GAIN_REPORT:
         pOemEv->type = OEMBTAG_EV_MIC_GAIN;
         pOemEv->bdAddr = 
            *((BDAddress*)&pEv->ev_msg.ev_ag_ad_mic_gain_report.bd_addr);
         pOemEv->u8Buf[0] = 
            pEv->ev_msg.ev_ag_ad_mic_gain_report.ad_microphone_gain;
         break;
      case BT_EV_PF_A2DP_CON:
         pOemEv->type = OEMBTAG_EV_A2DP_CONNECTED;
         break;
      case BT_EV_PF_A2DP_DISCON:
         pOemEv->type = OEMBTAG_EV_A2DP_DISCONNECTED;
         break;
      case BT_EV_PF_A2DP_START:
         pOemEv->type = OEMBTAG_EV_A2DP_START;
         break;
      case BT_EV_PF_A2DP_SUSPEND:
         pOemEv->type = OEMBTAG_EV_A2DP_SUSPEND;
         break;
   }
}

static OEMAGEvent* OEMBTAG_GetFreeEvent (OEMAG* pMe)
{
   int i;
   OEMAGEvent* pFreeEv = NULL;

   OEMAG_LOCK();
   for (i = 0; i < ARR_SIZE(pMe->eventPool); i++)
   {
      // found an event from free pool
      if (OEMBTAG_EV_NONE == pMe->eventPool[i].type)
      {
         pFreeEv = &pMe->eventPool[i];
         pFreeEv->type  = OEMBTAG_EV_PREPPING; // mark it as not free
         pFreeEv->op    = AEEBTAG_OP_NONE;
         pFreeEv->pNext = NULL;
         break;
      }
   }
   OEMAG_UNLOCK();
   return pFreeEv;
}

static void OEMBTAG_EvSignalCallback(void *pData)
{
   OEMAG* pMe = (OEMAG*)pData;
   if (NULL != pMe->piEventSignal)
   {
      ISignal_Set (pMe->piEventSignal);
   }
   return;
}
static void OEMBTAG_QueueReadyEvent (OEMAG* pMe, OEMAGEvent* pEv)
{
   OEMAGEvent** ppEv = NULL;

   if ((NULL == pMe) || (pMe != pgTheOEMAG) || 
       (NULL == pMe->piEventSignal))
   {
      MSG_LOW ("OEMBTAG_QReadyEv - obj not ready",0,0,0);
      return;
   }

   OEMAG_LOCK();
   {
      // find the end of the Ready event list
      ppEv = &pMe->pReadyEvent;
      while ((NULL != ppEv) && (NULL != *ppEv))
      {
         ppEv = &(*ppEv)->pNext;
      }

      // append the new event to Ready event list
      if (NULL != ppEv)
      {
         *ppEv = pEv;
      }
   }
   if (NULL != ppEv)
   {
      CALLBACK_Cancel(&pMe->evSignalCb);
      CALLBACK_Init(&pMe->evSignalCb, OEMBTAG_EvSignalCallback, pMe);
      AEE_SYS_RESUME(&pMe->evSignalCb);
   }
   OEMAG_UNLOCK();
}

// executes in BREW system context
static void OEMBTAG_SetATCmdSignal (void* pUser)
{
   OEMAG* pMe = pgTheOEMAG;

   if ((NULL == pMe) || (pMe != pgTheOEMAG) || 
       (NULL == pMe->extIO.piATCmdSignal))
   {
      DBGPRINTF ("OEMBTAG_SetATCmdSignal - obj not ready");
      return;
   }

   if ((OEMAG*)pUser == pMe)
   {
      ISignal_Set( pMe->extIO.piATCmdSignal );
   }
}

// This executes in BT driver context
static void OEMAG_ATCmdRxCb (void)
{
   OEMAG* pMe = pgTheOEMAG;

   if ((NULL != pMe) && 
       (NULL != pMe->extIO.piATCmdSignal) && (NULL != pMe->extIO.pRxWm))
   {
      // ***** doing this causes phone to crash due to incompatibility *****
      // ***** between TASKLOCK and critical sections                  *****
      // ISignal_Set( pMe->extIO.piATCmdSignal );

      (void) AEE_SetSysTimer (5, OEMBTAG_SetATCmdSignal, pMe);
   }
}

static void OEMBTAG_SoundTypeChanged (OEMBTSoundType soundType)
{
   OEMAG* pMe = pgTheOEMAG;
   OEMAGEvent* pEv = NULL;
   if ((NULL == pMe) || (NULL == pMe->piEventSignal))
   {
      return;  // maybe should deregister callback from AVS...
   }

   if ((pEv = OEMBTAG_GetFreeEvent (pMe)) == NULL)
   {
      DBGPRINTF_ERROR ("SndTypeChgd - no free OEM event");
      return; // drop event as event queue is full
   }
   pEv->soundType = soundType;
   pEv->type = OEMBTAG_EV_SND_TYPE;

   OEMBTAG_QueueReadyEvent (pMe, pEv);
}

static void OEMBTAG_PeriodicBatteryCheck (void* pUser)
{
   OEMAG* pMe = (OEMAG*)pUser;
   if (pMe == pgTheOEMAG)
   {
      uint8  uNewBattLevel = vbatt_read (); // value ranges 0 to 255
      if (pMe->uBatteryLevel != uNewBattLevel)
      {
         bt_ag_battchg_level_type uScaledBatteryLevel;
         pMe->uBatteryLevel = uNewBattLevel;

         if (uNewBattLevel == 0)
         {
            uScaledBatteryLevel = BT_AG_BATTCHG_LEVEL_0;
         }
         else
         {
            uScaledBatteryLevel = (bt_ag_battchg_level_type) 
               ((uNewBattLevel * BT_AG_BATTCHG_LEVEL_5) / 255);
         }
         (void) bt_cmd_ag_set_battchg_ind (pMe->appId, uScaledBatteryLevel);
      }
      (void) AEE_SetSysTimer (10000, OEMBTAG_PeriodicBatteryCheck, pMe);
   }
}

// called from BT task context; so just convert and queue the event
static void OEMBTAG_EventCallback (bt_ev_msg_type* bt_ev_msg_ptr)
{
   OEMAG* pMe = pgTheOEMAG;
   OEMAGEvent* pEv = NULL;

   if ((NULL == pMe) || (NULL == pMe->piEventSignal))
   {
      return;
   }

   if ((pEv = OEMBTAG_GetFreeEvent (pMe)) == NULL)
   {
      MSG_ERROR ("EventCb - no free OEM event",0,0,0);
      return; // drop event as event queue is full
   }

   OEMBTAG_CoreEv2OemEv (bt_ev_msg_ptr, pEv);
   if (AEEBTAG_OP_CONFIG == pEv->op)
   {
      if (--pMe->nRMCmdExpected > 0)
      {
         pEv->type = OEMBTAG_EV_PREPPING;
      }
   }
   if ((OEMBTAG_EV_PREPPING != pEv->type) && (OEMBTAG_EV_NONE != pEv->type))
   {
      MSG_LOW ("AG_EvCb - queueing ev=%d op=%d", pEv->type, pEv->op, 0);
      OEMBTAG_QueueReadyEvent (pMe, pEv);
   }
   else
   {
      OEMAG_LOCK();
      pEv->type = OEMBTAG_EV_NONE; // mark it as free
      OEMAG_UNLOCK();
   }
}

/************************************************************************ 
                       Exposed to AEE layer
************************************************************************/
int OEMBTAG_Enable (OEMAG* pMe)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      bt_cmd_status_type stat = bt_cmd_ag_enable (pMe->appId, NULL, 
                                                  BT_AD_HEADSET);
      result = OEMBT_CmdStatus2AEEResult (stat);
   }
   return result;
}

int OEMBTAG_Disable (OEMAG* pMe)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      bt_cmd_status_type stat = bt_cmd_ag_disable (pMe->appId);
      result = OEMBT_CmdStatus2AEEResult (stat);
   }
   return result;
}

int OEMBTAG_Connect (OEMAG* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe) && (NULL != pAddr))
   {
      bt_cmd_status_type   stat;
      bt_audio_device_type dev_type = BT_AD_MAX;
      bt_device_type       device;
      bt_service_type      svcs[BT_SD_MAX_NUM_OF_SRV_REC];
      uint8                uNumSvcs = BT_SD_MAX_NUM_OF_SRV_REC;

      // search RM device DB for given device
      device.bd_addr = *((bt_bd_addr_type*)pAddr);
      stat = bt_cmd_rm_device_read (pMe->appId, &device, &uNumSvcs, svcs );
      if ((AEE_SUCCESS == OEMBT_CmdStatus2AEEResult (stat)) &&
          (uNumSvcs > 0))
      {
         int i;
         // search for Handsfree first
         for (i = 0; i< uNumSvcs; i++)
         {
            if (svcs[i].service_class == BT_SD_SERVICE_CLASS_HANDSFREE)
            {
               dev_type = BT_AD_HANDSFREE;
               break;
            }
         }
         if (BT_AD_MAX == dev_type)
         {
            // Handsfree not supported, how about Headset?
            for (i = 0; i< uNumSvcs; i++)
            {
               if (svcs[i].service_class == BT_SD_SERVICE_CLASS_HEADSET)
               {
                  dev_type = BT_AD_HEADSET;
                  break;
               }
            }
         }
      }
      else // no service info available in RM DB
      {
         // assume remote supports Handsfree profile
         dev_type = BT_AD_HANDSFREE;
      }

      if (BT_AD_MAX == dev_type)
      {
         // device does not support either Headset or Handsfree profile
         result = AEE_ENOTALLOWED;
      }
      else
      {
         // will try HSP if HFP connection attempt fails
         // set the bit before calling bt_cmd_ag_connect() in case
         // events generated due to bt_cmd_ag_connect() received
         // before it returns.
         pMe->uFlags |= OEMAG_ATTEMPT_HSP_B;

         stat = bt_cmd_ag_connect (pMe->appId, (bt_bd_addr_type*)pAddr, 
                                   dev_type);
         result = OEMBT_CmdStatus2AEEResult (stat);

         if ((BT_AD_HEADSET == dev_type) ||
             (AEE_SUCCESS != result))
         {
            pMe->uFlags &= ~OEMAG_ATTEMPT_HSP_B;
         }
      }
   }
   return result;
}

int OEMBTAG_Disconnect (OEMAG* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      // currently only support one ACL connection
      // so ignoring the given BD address
      bt_cmd_status_type stat = bt_cmd_ag_disconnect (pMe->appId);
      result = OEMBT_CmdStatus2AEEResult(stat);
   }
   return result;
}

int OEMBTAG_ConnectAudio (OEMAG* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      bt_cmd_status_type stat;

      (void) OEMBTAudio_RegisterDeviceChangeCb();
      OEMBTAudio_SetState (OEMBT_STATE_BRINGING_UP_AUDIO);
      // currently only support one ACL connection
      // so ignoring the given BD address
      stat = bt_cmd_ag_audio_connect (pMe->appId, 0);
      
      result = OEMBT_CmdStatus2AEEResult(stat);
   }
   return result;
}

int OEMBTAG_DisconnectAudio (OEMAG* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      bt_cmd_status_type stat;

      OEMBTAudio_SetState (OEMBT_STATE_BRINGING_DOWN_AUDIO);
      (void) OEMBTAudio_DeregisterDeviceChangeCb();      
      // currently only support one ACL connection
      // so ignoring the given BD address
      stat = bt_cmd_ag_audio_disconnect (pMe->appId);
      result = OEMBT_CmdStatus2AEEResult (stat);
   }
   return result;
}

int OEMBTAG_ReportVRState (OEMAG* pMe, boolean bVRActive)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      bt_cmd_status_type stat = bt_cmd_ag_update_vr_state (pMe->appId, 
                                                           bVRActive);
      result = OEMBT_CmdStatus2AEEResult (stat);
   }
   return result;
}

void OEMBTAG_SetAudioMuting (OEMAG* pMe, boolean bMute)
{
   ISound* piSound = NULL;
   DBGPRINTF ("SetAudioMuting - bMute=%d", bMute);
   if (ISHELL_CreateInstance (pMe->piShell, AEECLSID_SOUND, 
                              (void**)&piSound) == SUCCESS)
   {
      AEESoundInfo si;
      ISOUND_Get (piSound, &si);
      if (AEE_SOUND_DEVICE_BT_HEADSET == si.eDevice)
      {
         uint16 uVol = bMute ? 0 : (AEE_MAX_VOLUME);
         DBGPRINTF ("SetAudioMuting - vol=%d", uVol);
         ISOUND_SetVolume(piSound, uVol);
      }
   }
   if (NULL != piSound)
   {
      ISOUND_Release (piSound);
   }
}


void OEMBTAG_SuspendA2DP (OEMAG* pMe)
{
   DBGPRINTF ("SuspendA2DP - entered");
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      OEMBTAudio_DisableDevice (OEMBT_SND_DEV_TYPE_A2DP);
   }
}

void OEMBTAG_ResumeA2DP (OEMAG* pMe)
{
   DBGPRINTF ("ResumeA2DP - entered");

   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      OEMBTAudio_EnableDevice (OEMBT_SND_DEV_TYPE_A2DP);
   }
}


void OEMBTAG_SetInband (OEMAG* pMe, boolean bInbandOn)
{
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      (void) bt_cmd_ag_set_inband_ring (pMe->appId, bInbandOn);
   }
}

void OEMBTAG_SendRingAlert (OEMAG* pMe)
{
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      (void) bt_cmd_ag_ring_audio_device (pMe->appId, 1, 0);
      ISHELL_SetTimer( pMe->piShell, OEMBTAG_RING_ALERT_PERIOD_MS,
                       (PFNNOTIFY) OEMBTAG_SendRingAlert, pMe );
   }
}

void OEMBTAG_StopRingAlert (OEMAG* pMe)
{
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      if (ISHELL_GetTimerExpiration (pMe->piShell, 
                                     (PFNNOTIFY) OEMBTAG_SendRingAlert, 
                                     pMe ) != 0)
      {
         ISHELL_CancelTimer (pMe->piShell, 
                             (PFNNOTIFY) OEMBTAG_SendRingAlert, pMe);
         (void) bt_cmd_ag_ring_audio_device (pMe->appId, 0, 0);
      }
   }
}

void OEMBTAG_SendResponse (OEMAG* pMe, const char* pszRsp)
{
#ifndef BT_SIMULATION
   if ((NULL != pMe) && (pgTheOEMAG == pMe) && (NULL != pszRsp))
   {
      uint16 uNumBytes = STRLEN (pszRsp) + 4;
      dsm_item_type* dsm_item_ptr = bt_get_free_dsm_ptr (BT_TL_RFCOMM, 
                                                         uNumBytes);
      if (NULL == dsm_item_ptr)
      {
         DBGPRINTF_ERROR ("SendResponse - failed to get free dsm item");
      }
      else if ((dsm_pushdown_tail (&dsm_item_ptr, "\r\n", 2, 
                                   dsm_item_ptr->pool_id) != 2) || 
               (dsm_pushdown_tail (&dsm_item_ptr, pszRsp, STRLEN (pszRsp), 
                                   dsm_item_ptr->pool_id) != STRLEN (pszRsp)) || 
               (dsm_pushdown_tail (&dsm_item_ptr, "\r\n", 2, 
                                   dsm_item_ptr->pool_id) != 2))
      {
         DBGPRINTF_ERROR ("SendResponse - failed to push data onto dsm item");
         dsm_free_packet (&dsm_item_ptr);
      }
      else
      {
         dsm_enqueue (pMe->extIO.pTxWm, &dsm_item_ptr);
      }
   }
#endif
}

int OEMBTAG_GetNextEvent (OEMAG* pMe, OEMAGEvent* pOemEv)
{
   int result = AEE_ENOMORE;
   OEMAGEvent* pEv = NULL;

   if ((NULL != pMe) && (pgTheOEMAG == pMe) && (NULL != pOemEv))
   {
      DBGPRINTF ("AG_GetNextEv - pReadyEv=0x%x", pMe->pReadyEvent, 0, 0 );
      while ((NULL == pEv) && 
             (NULL != pMe->pReadyEvent) && 
             (AEE_SUCCESS != result))
      {
         // extract first event from list
         OEMAG_LOCK();
         pEv = pMe->pReadyEvent;
         pMe->pReadyEvent = pMe->pReadyEvent->pNext;
         OEMAG_UNLOCK();

         switch (pEv->type)
         {
            case OEMBTAG_EV_ENABLED:
               (void) AEE_SetSysTimer (10000, OEMBTAG_PeriodicBatteryCheck, pMe);
               break;
            case OEMBTAG_EV_DISABLED:
               (void) AEE_CancelTimer (OEMBTAG_PeriodicBatteryCheck, pMe);
               break;
            case OEMBTAG_EV_AUDIO_CONNECTED:
               OEMBTAudio_EnableDevice (OEMBT_SND_DEV_TYPE_AG);
               break;
            case OEMBTAG_EV_AUDIO_CONNECT_FAILED:
            case OEMBTAG_EV_AUDIO_DISCONNECTED:
               OEMBTAudio_DisableDevice (OEMBT_SND_DEV_TYPE_AG);
               break;
         }

         *pOemEv = *pEv; // copy event to AEE destination
         result = AEE_SUCCESS;

         OEMAG_LOCK();
         pEv->type = OEMBTAG_EV_NONE; // free the local event
         pEv = NULL;
         OEMAG_UNLOCK();
      }
   }
   return result;
}

void OEMBTAG_EventCbDeregister (OEMAG* pMe)
{
   DBGPRINTF ("AG_EvCbDereg - pMe=0x%x pgObj=0x%x piSig=0x%x", 
              pMe, pgTheOEMAG, pMe->piEventSignal);
   CALLBACK_Cancel(&pMe->evSignalCb);
   if ((NULL != pMe) && (pgTheOEMAG == pMe) && (NULL != pMe->piEventSignal))
   {
      ISignal_Release (pMe->piEventSignal);
      pMe->piEventSignal = NULL;
   }
}

int OEMBTAG_EventCbRegister (OEMAG* pMe, ISignal* piSignal)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("AG_EvCbReg - piSig=0x%x", piSignal, 0, 0 );
   if ((NULL != pMe) && (pgTheOEMAG == pMe))
   {
      pMe->piEventSignal = piSignal;
      ISignal_AddRef (pMe->piEventSignal);
      result = AEE_SUCCESS;
   }
   return result;
}

int OEMBTAG_RetrieveATCmd (
   OEMAG* pMe,
   char* pszATCmd,
   uint16* puNumChars)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMAG == pMe) && 
       (NULL != pszATCmd) && (NULL != puNumChars))
   {
      AGExternalIO* pEIO = &pMe->extIO;
      uint16 bytesRead   = 0;
      uint16 bytesToRead = *puNumChars;
      *puNumChars = 0;

      while ( TRUE )
      {
         if ( pEIO->pDSMItem != NULL )
         {
            bytesRead = dsm_pullup (&pEIO->pDSMItem,
                                    (pszATCmd + *puNumChars), 
                                    (bytesToRead - *puNumChars) );
            if ( bytesRead == 0 )
            {
               DBGPRINTF_ERROR ("ATCmdRxCb - dsm_pullup() returned 0");
            }

            *puNumChars += bytesRead;

            if (*puNumChars >= bytesToRead)  // destination full?
            {
               break;
            }
            // dsm_pullup will free the dsm_item after pulling off the last byte
         }
         if ( NULL == pEIO->pDSMItem )  // DSM item has been freed?
         {
            pEIO->pDSMItem = dsm_dequeue (pEIO->pRxWm); // get next one
            if ( pEIO->pDSMItem == NULL )  // rx queue is empty?
            {
               break;
            }
         }
      }
      result = (*puNumChars != 0) ? AEE_SUCCESS : AEE_EFAILED;
   }
   return result;
}

void OEMBTAG_ATCmdParserCleanup (OEMAG* pMe)
{
   if ((NULL != pMe) && (pgTheOEMAG == pMe) && (BT_APP_ID_NULL != pMe->appId))
   {
      OEMAG_LOCK();
      if ( pMe->extIO.pTxWm != NULL ) 
      {
         dsm_empty_queue( pMe->extIO.pTxWm );
      }
      if ( pMe->extIO.pRxWm != NULL ) 
      {
         dsm_empty_queue( pMe->extIO.pRxWm );
      }
      OEMAG_UNLOCK();

      bt_cmd_ag_external_io_config (pMe->appId, FALSE, NULL, NULL, 
                                    BT_AG_IC_NONE_M);
      FREEIF (pMe->extIO.pTxWm);
      FREEIF (pMe->extIO.pRxWm);

      if (NULL != pMe->extIO.piATCmdSignal)
      {
         ISignal_Release (pMe->extIO.piATCmdSignal);
         pMe->extIO.piATCmdSignal = NULL;
      }
   }
}

int OEMBTAG_ATCmdParserInit (OEMAG* pMe, ISignal* piSignal)
{
   int result = AEE_EFAILED;

   DBGPRINTF ("AG_ATCmdParserInit - piSig=0x%x", piSignal, 0, 0 );

   if ((NULL != pMe) && (pgTheOEMAG == pMe) && 
       (BT_APP_ID_NULL != pMe->appId) && (NULL != piSignal))
   {
      bt_cmd_status_type stat;

      if (((pMe->extIO.pTxWm = MALLOCREC (dsm_watermark_type)) == NULL) ||
          ((pMe->extIO.pRxWm = MALLOCREC (dsm_watermark_type)) == NULL))
      {
         DBGPRINTF_ERROR ("ATCmdParserInit - memory allocation failed");
         result = AEE_ENOMEMORY;
      }
      else
      {
         dsm_queue_init( pMe->extIO.pRxWm, 1000, &pMe->extIO.rxQ );
         pMe->extIO.pRxWm->each_enqueue_func_ptr = OEMAG_ATCmdRxCb;

         dsm_queue_init( pMe->extIO.pTxWm, 1000, &pMe->extIO.txQ );

         stat = bt_cmd_ag_external_io_config (
                  pMe->appId, TRUE, pMe->extIO.pRxWm, pMe->extIO.pTxWm, 
                  BT_AG_IC_NONE_M);
         result = OEMBT_CmdStatus2AEEResult (stat);
      }

      if (AEE_SUCCESS == result)
      {
         pMe->extIO.piATCmdSignal = piSignal;
         ISignal_AddRef (piSignal);
      }
   }

   return result;
}

void OEMBTAG_AudioHandlerCleanup (OEMAG* pMe)
{
   (void) OEMBTAudio_DeregisterSoundChangeCb (OEMBTAG_SoundTypeChanged);
}

int OEMBTAG_AudioHandlerInit (OEMAG* pMe)
{
   int result;
   OEMBTAudio_InitializeVolumes (OEMBT_SND_DEV_TYPE_AG);
   result = OEMBTAudio_RegisterSoundChangeCb (OEMBTAG_SoundTypeChanged, 
                                              OEMBT_SND_DEV_TYPE_AG);
   return result;
}

void OEMBTAG_Release (OEMAG* pMe)
{
   if ((NULL == pMe) || (pgTheOEMAG != pMe))
   {
      DBGPRINTF_ERROR ("OEMBTAG_Release - pMe=0x%x pgTheOEMAG=%0x%x",
                       pMe, pgTheOEMAG);
   }
   else
   {
      if (BT_APP_ID_NULL != pMe->appId)
      {
         bt_cmd_ec_free_application_id (pMe->appId);
         pMe->appId = BT_APP_ID_NULL;
      }
      if (NULL != pMe->piShell)
      {
         ISHELL_Release (pMe->piShell);
         pMe->piShell = NULL;
      }
      FREEIF (pMe);
      pgTheOEMAG = NULL;
   }
#ifdef BT_SIMULATION
   sim_btag_cleanup();
#endif
}

int OEMBTAG_New (IShell* piShell, OEMAG** ppo)
{
   int result = AEE_EFAILED;

   if (NULL == pgTheOEMAG)
   {
      pgTheOEMAG = MALLOCREC (OEMAG);
      if (NULL == pgTheOEMAG)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - no mem for obj");
      }
      else if ((pgTheOEMAG->appId = 
                bt_cmd_ec_get_application_id ()) == BT_APP_ID_NULL)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - failed to get appID");
      }
      else if (bt_cmd_ec_reg_event_set_any_app_id (
                 pgTheOEMAG->appId, OEMBTAG_EventCallback, 
                 BT_EC_ES_AUDIO_GATEWAY,
                 BT_EV_NULL, BT_EV_NULL) != BT_CS_GN_SUCCESS)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - failed to register for AG events");
      }
      else if (bt_cmd_ec_register_event_set (
                 pgTheOEMAG->appId, OEMBTAG_EventCallback, BT_EC_ES_CUSTOM,
                 BT_EV_GN_CMD_DONE, BT_EV_GN_CMD_DONE) != BT_CS_GN_SUCCESS)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - failed to register for CMD_DONE ev");
      }
      else if (bt_cmd_ec_reg_event_set_any_app_id (
                 pgTheOEMAG->appId, OEMBTAG_EventCallback, BT_EC_ES_CUSTOM,
                 BT_EV_PF_A2DP_CON,BT_EV_PF_A2DP_SUSPEND) != BT_CS_GN_SUCCESS)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - failed to register for A2DP events");
      }
      else if (bt_cmd_ag_config (
                 pgTheOEMAG->appId, BT_AGIM_AUTOMATIC, 0) != BT_CS_GN_PENDING)
      {
         DBGPRINTF_ERROR ("OEMBTAG_New - failed to config AG");
      }
      else
      {
         pgTheOEMAG->pReadyEvent = NULL;

         pgTheOEMAG->piShell = piShell;
         ISHELL_AddRef (piShell);

         *ppo = pgTheOEMAG;
         result = AEE_SUCCESS;
      }
   }

#ifdef BT_SIMULATION
  bt_sim_start(PROFILE_AG, TRUE);
#endif
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


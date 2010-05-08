/*=============================================================================
FILE:         OEMBTA2DP.c

SERVICES:     Oem implementation of Bluetooth Advanced Audio Distribution Profile 

DESCRIPTION:  This file contains the OEM implementation for the
              Bluetooth A2DP interface.
              

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTA2DP.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.

 =========================================================================== */

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)
#include "OEMHeap.h"
#include "AEEStdErr.h"
#include "AEE_OEMHeap.h"
#include "bt.h"
#include "btpf.h"
#include "bti.h"
#include "btpfcmdi.h"
#include "OEMBTA2DP.h"
#include "AEEIBTA2DPNotifier.h"
#include "AEEIBTA2DP.h"
#include "OEMBTUtils.h"
#include "OEMBTAudio.h"


#include "uixsnd.h"
#include "AEE_OEM.h"
#ifdef BT_SIMULATION
#include "btmsg.h"
#include "bt_sim_interface.h"
#else
#include "snddev.h"
#include "msg.h"
#endif

#define OEMBTA2DP_CTL_INVALID 0xFF

//=============================================================================
// 
//
//=============================================================================
struct OEMBTA2DP
{
  bt_app_id_type          appId;
  void                    *cbData;
  PFNA2DPEVTCB            pAeeCb;
  BDAddress               bdAddr;

  // these are the values to be set system wide
  boolean                 bSuspendOnEnd;
  bt_pf_a2dp_ctl_type     reCfg;
};
//=============================================================================
// 
//
//=============================================================================
static void OEMBTA2DP_EventCallback(bt_ev_msg_type* ev_msg_ptr);
static int OEMA2DPBtToAeeBdAddr(BDAddress *pAeeAddr,
                                bt_bd_addr_type *pBtAddr); 

static int OEMA2DPAeeToBtBdAddr(bt_bd_addr_type *pBtAddr,
                                const  BDAddress *pAeeAddr); 
static uint32 OEMBTA2DP_MapErrorCode(bt_cmd_status_type reason);
static uint8 OEMBTA2DP_AeeToBtCtl(OEMBTA2DP *me, BTA2DPConfig config, boolean bUpdate );
void OEMBTA2DP_UpdateConfig(OEMBTA2DP *me);
//=============================================================================

// 
//
//=============================================================================
OEMBTA2DP *gOEMBTA2DP;


//=============================================================================
//
//
//=============================================================================
int OEMBTA2DP_Init(OEMBTA2DP **pOemobj, PFNA2DPEVTCB aeeEvtCb, void* cbData)
{
   OEMBTA2DP *me;

   MSG_HIGH("OEM a2dp init ->",0,0,0);

   me = (OEMBTA2DP*)OEM_Malloc(sizeof(OEMBTA2DP));

   if (NULL == me)
   {
      MSG_ERROR("OEM a2dp could not alloc memory",0,0,0);
      return AEE_ENOMEMORY;
   }
   //since sys_malloc does not zero out the memory, need to initialize it.
   MEMSET(me, 0, sizeof(OEMBTA2DP));
   me->cbData         = cbData;
   me->pAeeCb         = aeeEvtCb;
   me->bSuspendOnEnd  = TRUE;
   me->reCfg          = BT_PF_A2DP_CTL_RECFG_BY_CLOSE; 

   me->appId =  bt_cmd_ec_get_app_id_and_register(OEMBTA2DP_EventCallback);

   if (BT_APP_ID_NULL == me->appId)
   {
      OEMBTA2DP_Release(me);
      MSG_ERROR("OEM a2dp could not alloc app id",0,0,0);
      return AEE_EFAILED;
   }
   *pOemobj = me;
   gOEMBTA2DP = me;

   MSG_HIGH("OEM a2dp init <-",0,0,0);

#ifdef BT_SIMULATION
   bt_sim_start(PROFILE_A2DP_SINK, TRUE);
#endif

   return AEE_SUCCESS;
}
//=============================================================================
//
//
//=============================================================================
void OEMBTA2DP_StateConnectFailed(OEMBTA2DP *me)
{
    OEMBTAudio_DisableDevice (OEMBT_SND_DEV_TYPE_A2DP);
    return;
}
//=============================================================================
//
//
//=============================================================================
void OEMBTA2DP_StateEnabled(OEMBTA2DP *me)
{
   (void)OEMBTAudio_RegisterSoundChangeCb (NULL, OEMBT_SND_DEV_TYPE_A2DP);
    return;
}
//=============================================================================
//
//
//=============================================================================
void OEMBTA2DP_StateConnected(OEMBTA2DP *me)
{
   OEMBTAudio_InitializeVolumes (OEMBT_SND_DEV_TYPE_A2DP);
   OEMBTAudio_EnableDevice (OEMBT_SND_DEV_TYPE_A2DP);
   (void)bt_cmd_rm_set_connectable(me->appId, FALSE, BT_RM_AVP_AUTOMATIC );
   return;
}  
//=============================================================================
//
//
//=============================================================================
void OEMBTA2DP_StateDisconnected(OEMBTA2DP *me)
{
   OEMBTAudio_DisableDevice (OEMBT_SND_DEV_TYPE_A2DP);
   OEMBTA2DP_UpdateConfig(me);
   (void)bt_cmd_rm_set_connectable(me->appId, TRUE, BT_RM_AVP_AUTOMATIC);
   return;
}
//=============================================================================
//
//
//=============================================================================
static void OEMBTA2DP_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr)
{
   OEMBTA2DPEvent evt;
   OEMBTA2DP *me = gOEMBTA2DP;
   boolean bFlagNotify = TRUE;
   
   if (NULL == me)
   {
      MSG_HIGH("OEM a2dp cb, me NUll",0,0,0);
      return;

   }

   MSG_HIGH( "OEM a2dp ev cb - ev=%x", bt_ev_msg_ptr->ev_hdr.ev_type,
            0, 0 );

   MEMSET((void*)&evt, 0, sizeof(OEMBTA2DPEvent));

   switch (bt_ev_msg_ptr->ev_hdr.ev_type)
   {
      case BT_EV_GN_CMD_DONE:
      {
         bt_ev_gn_cmd_done_type* pm =
                           (bt_ev_gn_cmd_done_type*)&bt_ev_msg_ptr->ev_msg;

         MSG_HIGH( "OEM a2dp cmd dne cmd %x, stat %x", pm->cmd_type,
            pm->cmd_status, 0 );
         switch (pm->cmd_type)
         {
            case BT_PF_CMD_A2DP_CONNECT:
            {
               if(pm->cmd_status != BT_CS_GN_SUCCESS)
               {
                  evt.event = AEEBTA2DP_EVT_CON_FAILED;
                  evt.u.evtEnbl.result = OEMBTA2DP_MapErrorCode(pm->cmd_status);
               }
               else
               {
                  bFlagNotify = FALSE;
               }
               break;
            }
            case BT_PF_CMD_A2DP_DISCONNECT:
            {
               bFlagNotify = FALSE;
               break;
            }
            case BT_PF_CMD_A2DP_ENABLE:
            {
               if(pm->cmd_status != BT_CS_GN_SUCCESS)
               {
                  evt.event = AEEBTA2DP_EVT_ENABLE_FAILED;
                  evt.u.evtEnbl.result = OEMBTA2DP_MapErrorCode(pm->cmd_status);
               }
               else
               {
                  evt.event = AEEBTA2DP_EVT_ENABLED;
                  evt.u.evtEnbl.result = AEE_SUCCESS;
               }
               break;
            }
            case BT_PF_CMD_A2DP_DISABLE:
            {
               evt.event = AEEBTA2DP_EVT_DISABLED;
               evt.u.evtDsbl.result = AEE_SUCCESS;
               MEMSET(&me->bdAddr,0,sizeof(BDAddress));
               break;
            }
            case BT_PF_CMD_A2DP_SET_DEVICE:
            case BT_PF_CMD_A2DP_CTL:
            case BT_PF_CMD_A2DP_START:
            case BT_PF_CMD_A2DP_SUSPEND:
            case BT_CMD_RM_SET_CONNECTABLE:
            {
               bFlagNotify = FALSE;
               if(pm->cmd_status != BT_CS_GN_SUCCESS)
               {
                  MSG_ERROR( "oem a2dp ev cb - Bad Cmd Done %x Status %x.",
                          pm->cmd_type, pm->cmd_status, 0 );
               }
              break;
            }
            default:
            {
               MSG_ERROR( "oem a2dp unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
               bFlagNotify = FALSE;
               break;
            }
         }
         break;
      } // end of case BT_EV_GN_CMD_DONE

      case BT_EV_PF_A2DP_CON:
      {
         evt.event = AEEBTA2DP_EVT_CONNECTED;
         OEMA2DPBtToAeeBdAddr(&evt.u.evtConn.bdAddr,
                              &bt_ev_msg_ptr->ev_msg.ev_a2dp_con.bd_addr); 
         break;
      }
      case BT_EV_PF_A2DP_CON_FAILED:
      {
         evt.event = AEEBTA2DP_EVT_CON_FAILED;
         OEMA2DPBtToAeeBdAddr(&evt.u.evtConFail.bdAddr,
                 &bt_ev_msg_ptr->ev_msg.ev_a2dp_con_failed.bd_addr); 
         evt.u.evtConFail.result =
                 OEMBTA2DP_MapErrorCode(bt_ev_msg_ptr->ev_msg.ev_a2dp_con_failed.reason);
         break;
      }
      case BT_EV_PF_A2DP_DISCON:
      {
         evt.event = AEEBTA2DP_EVT_DISCONNECTED;
         evt.u.evtDisc.result = OEMBTA2DP_MapErrorCode(
                         bt_ev_msg_ptr->ev_msg.ev_a2dp_discon.reason);
         OEMA2DPBtToAeeBdAddr(&evt.u.evtDisc.bdAddr,
                        &bt_ev_msg_ptr->ev_msg.ev_a2dp_discon.bd_addr); 
         MEMSET(&me->bdAddr,0,sizeof(BDAddress));
         break;
      }
      case BT_EV_PF_A2DP_START:
      {
		   OEMA2DPBtToAeeBdAddr(&evt.u.evtStart.bdAddr,
            &bt_ev_msg_ptr->ev_msg.ev_a2dp_start.bd_addr);
         evt.event = AEEBTA2DP_EVT_START;
         evt.u.evtStart.bLocalInit =
            (bt_ev_msg_ptr->ev_msg.ev_a2dp_start.origin == BT_PF_A2DP_EVENT_LOCAL_INIT)
             ? TRUE : FALSE ;
         break;
      }
      case BT_EV_PF_A2DP_SUSPEND:
      {
		   OEMA2DPBtToAeeBdAddr(&evt.u.evtSuspend.bdAddr,
            &bt_ev_msg_ptr->ev_msg.ev_a2dp_suspend.bd_addr);
         evt.event = AEEBTA2DP_EVT_SUSPEND;
         evt.u.evtSuspend.bLocalInit = 
            (bt_ev_msg_ptr->ev_msg.ev_a2dp_suspend.origin == BT_PF_A2DP_EVENT_LOCAL_INIT) ? TRUE
                                       : FALSE ;
       break;
      }
      case BT_EV_PF_A2DP_OPEN:
      {
		   OEMA2DPBtToAeeBdAddr(&evt.u.evtOpen.bdAddr,
            &bt_ev_msg_ptr->ev_msg.ev_a2dp_open.bd_addr);
         evt.event = AEEBTA2DP_EVT_OPEN;
         evt.u.evtOpen.bLocalInit =
            (bt_ev_msg_ptr->ev_msg.ev_a2dp_open.origin == BT_PF_A2DP_EVENT_LOCAL_INIT) ? TRUE 
                                    : FALSE;
         break;
      }
      case BT_EV_PF_A2DP_CLOSE:
      {
		   OEMA2DPBtToAeeBdAddr(&evt.u.evtClose.bdAddr,
            &bt_ev_msg_ptr->ev_msg.ev_a2dp_close.bd_addr);
         evt.event = AEEBTA2DP_EVT_CLOSE;
         evt.u.evtClose.bLocalInit =
            (bt_ev_msg_ptr->ev_msg.ev_a2dp_close.origin == BT_PF_A2DP_EVENT_LOCAL_INIT) ? TRUE : FALSE;
         break;
      }
      default:
      {
         MSG_ERROR( "OEM a2dp ev cb - unexpect event %x",
                bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
         bFlagNotify = FALSE;
         break;
      }
   }
   if (bFlagNotify)
   {
      me->pAeeCb(me->cbData,&evt);
   }
   return;
}


//=============================================================================
//
//
//=============================================================================
int OEMBTA2DP_Release(OEMBTA2DP *me)
{
   gOEMBTA2DP = NULL;

   if (BT_APP_ID_NULL != me->appId)
   {
      bt_cmd_pf_a2dp_disable(me->appId);
      (void)bt_cmd_rm_set_connectable(me->appId, FALSE, BT_RM_AVP_AUTOMATIC);
      bt_cmd_ec_free_application_id(me->appId);
      me->appId = 0;
   }
   OEM_Free(me);
   return AEE_SUCCESS;
}

//=============================================================================
//
//
//=============================================================================
int OEMBTA2DP_Enable(OEMBTA2DP *me)
{
   int ret = AEE_SUCCESS;
   bt_cmd_status_type status;

   MSG_MED("OEM a2dp enable ->",0,0,0);

   // Register with the A2DP layer
   status = bt_cmd_pf_a2dp_enable(me->appId);

   ret = OEMBT_CmdStatus2AEEResult (status);

   if (AEE_SUCCESS == ret)
   {
      (void)bt_cmd_rm_set_connectable(me->appId, TRUE, BT_RM_AVP_AUTOMATIC);
      (void)bt_cmd_pf_a2dp_ctl(me->appId, BT_PF_A2DP_CTL_APP_START, 0);
   }

   MSG_MED("OEM a2dp enable ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
//
//
//=============================================================================
int OEMBTA2DP_Disable(OEMBTA2DP *me)
{
   int ret = AEE_SUCCESS;
   bt_cmd_status_type status;

   MSG_MED("OEM a2dp disable ->",0,0,0);

   bt_cmd_rm_set_connectable(me->appId, FALSE, BT_RM_AVP_AUTOMATIC);

   status  = bt_cmd_pf_a2dp_disable(me->appId);

   ret = OEMBT_CmdStatus2AEEResult (status);

   MSG_MED("OEM a2dp disable ret %x <-",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int OEMBTA2DP_Connect(OEMBTA2DP *me, const BDAddress *pBDAddr)
{
   int ret = AEE_SUCCESS;
   bt_cmd_status_type status;
   bt_bd_addr_type bda;

   MSG_MED("OEM a2dp connect ->",0,0,0);

   OEMA2DPAeeToBtBdAddr(&bda,pBDAddr);

   status = bt_cmd_pf_a2dp_connect(me->appId,
                                  (bt_bd_addr_type*)&bda);

   ret = OEMBT_CmdStatus2AEEResult (status);


   MSG_MED("OEM a2dp connect ret %x <-",ret,0,0);

   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int OEMBTA2DP_Disconnect(OEMBTA2DP *me, const BDAddress *pBDAddr)
{
   int ret = AEE_SUCCESS;
   bt_cmd_status_type status;

   MSG_MED("OEM a2dp disconnect ->",0,0,0);

   status = bt_cmd_pf_a2dp_disconnect(me->appId);

   ret = OEMBT_CmdStatus2AEEResult (status);

   MSG_MED("OEM a2dp disconnect ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int OEMBTA2DP_SetDevice(OEMBTA2DP *me, const BDAddress *pBDAddr)
{
   int ret = AEE_SUCCESS;
   bt_cmd_status_type status;
   bt_bd_addr_type btAddr;
   
   MSG_MED("OEM a2dp set device ->",0,0,0);

   OEMA2DPAeeToBtBdAddr(&btAddr,pBDAddr);

   status = bt_cmd_pf_a2dp_set_device(me->appId,(bt_bd_addr_type*)&btAddr);

   ret = OEMBT_CmdStatus2AEEResult (status);

   if (AEE_SUCCESS == ret)
   {
      OEMBTAudio_InitializeVolumes (OEMBT_SND_DEV_TYPE_A2DP);
      OEMBTAudio_EnableDevice (OEMBT_SND_DEV_TYPE_A2DP);
   }
  
   MSG_MED("OEM a2dp set device ret %x <-",ret,0,0);

   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static uint32 OEMBTA2DP_MapErrorCode(bt_cmd_status_type reason)
{
  uint16 result = 0;

  if((reason >= (BT_CMD_EV_HCI_BASE+BT_BE_PAGE_TIMEOUT)) && 
      reason < ((BT_CMD_EV_HCI_BASE+BT_BE_LAST)))
  {
    result = (reason & 0x00FF);
    switch(result)
    {
      case BT_BE_PAGE_TIMEOUT:
      case BT_BE_CMD_DISALLOWED:
           reason = (bt_cmd_status_type)result;
           break;
      default:
          reason = BT_CS_PF_DISCONNECT_LOWER_LAYER;
    }
  }

   switch (reason)
   {
      case BT_CS_PF_A2DP_ERR_INCOMPATIABLE_DEVICES :
        return AEEA2DP_RESULT_INCOMPATIBLE_DEVICES;
      case BT_CS_PF_A2DP_ERR_ENDPOINT_DISCOVERY:
         return AEEA2DP_RESULT_ENDPOINT_DISCOVERY;
      case BT_CS_PF_A2DP_ERR_FAILED_TO_START_STREAM:
         return AEEA2DP_RESULT_FAILED_TO_START_STREAM;
      case BT_CS_PF_A2DP_ERR_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM:
         return AEEA2DP_RESULT_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM;
      case BT_CS_PF_A2DP_ERR_ABORT_FAILED:
         return AEEA2DP_RESULT_ABORT_FAILED;
      case BT_CS_PF_A2DP_ERR_FAILED_TO_PROCESS_CMD:
         return AEEA2DP_RESULT_FAILED_TO_PROCESS_CMD;
      case BT_CS_PF_A2DP_ERR_SET_CONFIG_FAILED:
         return AEEA2DP_RESULT_SET_CONFIG_FAILED;
      case BT_CS_PF_CONFIG_FAIL_INVALID_PARAMETERS:
         return AEEA2DP_RESULT_CONFIG_PARAMS_NOT_AGREEABLE;
      case BT_CS_PF_CONNECT_REFUSED_NO_RESOURCES:
         return AEEA2DP_RESULT_CONNECT_REFUSED_NO_RESOURCES;
      case BT_CS_PF_CONNECT_TIMEOUT:
         return AEEA2DP_RESULT_PAGE_FAILED;
      case BT_CS_PF_CONNECT_REFUSED_SECURITY:
         return AEEA2DP_RESULT_AUTHENTICATION_FAILED;
      case BT_CS_PF_DISCONNECT_REMOTE_REQUEST:
         return AEEA2DP_RESULT_NORMAL_DISCONNECT;
      case BT_CS_PF_A2DP_NORMAL_DISCONNECT:
         return AEEA2DP_RESULT_NORMAL_DISCONNECT;
      case BT_CS_PF_DISCONNECT_LOWER_LAYER:
         return AEEA2DP_RESULT_LINK_LOST;
      case BT_BE_PAGE_TIMEOUT:
         return AEEA2DP_RESULT_PAGE_FAILED;
      case BT_BE_CMD_DISALLOWED:
         return AEEA2DP_RESULT_WRONG_COMMAND;
      case BT_CS_PF_NO_RESOURCES:
         return AEEA2DP_RESULT_RESOURCES_NOT_AVAILABLE;
      case BT_CS_PF_RESET_IN_PROGRESS:
         return AEEA2DP_RESULT_RESET_IN_PROGRESS;
      case BT_CS_PF_A2DP_ERR_A2DP_NONE:
         return AEEA2DP_RESULT_SUCCESS;
      default :
        return AEEA2DP_RESULT_FAILED_TO_PROCESS_CMD;
   }
}
//=============================================================================
// 
//
//=============================================================================

static int OEMA2DPAeeToBtBdAddr(bt_bd_addr_type *pBtAddr,
                                const  BDAddress *pAeeAddr)
{
   int i = 0;
   for (i = 0; i < OEMBTA2DP_MAX_BDA_LEN; i++)
   {
      pBtAddr->bd_addr_bytes[i] = pAeeAddr->uAddr[i];
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

static OEMA2DPBtToAeeBdAddr(BDAddress *pAeeAddr,
                            bt_bd_addr_type *pBtAddr)
{
   int i = 0;
   for ( i = 0; i < OEMBTA2DP_MAX_BDA_LEN; i++)
   {
      pAeeAddr->uAddr[i] = pBtAddr->bd_addr_bytes[i];
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================
int OEMBTA2DP_Config(OEMBTA2DP *me, const BDAddress *pBDAddr, BTA2DPConfig config)
{
   uint32 data = 0;
   int ret = AEE_EFAILED;
   bt_cmd_status_type status;
   uint8 ctl = 0;
   boolean bUpdate = (pBDAddr != NULL) ? FALSE: TRUE;

   MSG_MED("OEM a2dp config ->",0,0,0);


   ctl = OEMBTA2DP_AeeToBtCtl(me,config, bUpdate);

   if (OEMBTA2DP_CTL_INVALID != ctl)
   {
      status  = bt_cmd_pf_a2dp_ctl(me->appId, (bt_pf_a2dp_ctl_type)ctl, data);
      ret = OEMBT_CmdStatus2AEEResult (status);
   }

   MSG_MED("OEM a2dp config ret %x <-",ret,0,0);
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static uint8 OEMBTA2DP_AeeToBtCtl(OEMBTA2DP *me, BTA2DPConfig config, boolean bUpdate )
{
   uint8 ret = OEMBTA2DP_CTL_INVALID;

   switch(config)
   {
      case AEEA2DP_CFG_SUSPEND_ON_END:
      {
         if (bUpdate)
         {
            me->bSuspendOnEnd = TRUE;
         }
         ret = (uint8)BT_PF_A2DP_CTL_SUSPEND_ON_END;
         break;
      }
   case AEEA2DP_CFG_CLOSE_ON_END:
      {
         if (bUpdate)
         {
            me->bSuspendOnEnd = FALSE;
         }
         ret = (uint8)BT_PF_A2DP_CTL_CLOSE_ON_END;
         break;
      }
   case AEEA2DP_CFG_RECFG_BY_RECFG:
      {
         if (bUpdate)
         {
            me->reCfg = BT_PF_A2DP_CTL_RECFG_BY_RECFG;
         }
         ret = (uint8)BT_PF_A2DP_CTL_RECFG_BY_RECFG;
         break;
      }
   case AEEA2DP_CFG_RECFG_BY_CLOSE:
      {
         if (bUpdate)
         {
            me->reCfg = BT_PF_A2DP_CTL_RECFG_BY_CLOSE;
         }
         ret = (uint8)BT_PF_A2DP_CTL_RECFG_BY_CLOSE;
         break;
      }
   case AEEA2DP_CFG_AUTO_BITRATE:
      {
         ret = (uint8)BT_PF_A2DP_CTL_AUTO_BITRATE;
         break;
      }
   case AEEA2DP_CFG_INCR_BITRATE:
      {
         ret = (uint8)BT_PF_A2DP_CTL_INCR_BITRATE;
         break;
      }
   case  AEEA2DP_CFG_DECR_BITRATE:
      {
         ret = (uint8)BT_PF_A2DP_CTL_DECR_BITRATE;
         break;
      }
   case  AEEA2DP_CFG_LOWEST_BITRATE :
      {
         ret = (uint8)BT_PF_A2DP_CTL_LOWEST_BITRATE;
         break;
      }
   default :
      {
         ret = OEMBTA2DP_CTL_INVALID;
         break;
      }
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================

void OEMBTA2DP_UpdateConfig(OEMBTA2DP *me)
{
   if (me->bSuspendOnEnd)
   {
      bt_cmd_pf_a2dp_ctl(me->appId, BT_PF_A2DP_CTL_SUSPEND_ON_END, 0);
   }
   else
   {
      bt_cmd_pf_a2dp_ctl(me->appId, BT_PF_A2DP_CTL_CLOSE_ON_END, 0);
   }
   bt_cmd_pf_a2dp_ctl(me->appId, me->reCfg, 0);
   return;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */


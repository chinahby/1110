/*=============================================================================
FILE:         OEMBTSD.c

SERVICES:     OEMBTSD implementation

DESCRIPTION:  This file handles the OEM implementation for IBTServiceDiscovery

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMBTSD.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-13  pn   Processes BT events in BREW task.
2008-10-02  rs   Removing compiler and Lint warnings.
2008-09-17  rs   Changes for BT Simulation: Including the btsdxml.h for BT 
                 simulation build.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT)
#include "bt.h"
#include "btsd.h"
#include "btsdxml.h"
#include "btsio.h"
#include "OEMHeap.h"
#include "OEMBTUtils.h"
#include "OEMBTSD.h"
#include "OEMCriticalSection.h"
#include "AEEStd.h"
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEE_OEMDispatch.h"
#include "btmsg.h"
#ifdef BT_SIMULATION
#include "bt_sim_interface.h"
#endif /* BT_SIMULATION */

typedef struct _BTSDEvCb BTSDEvCb;
struct _BTSDEvCb
{
   BTSDEvCb*               pNext;
   SDEvCallbackFn*         pEvCbFn;
   IBTServiceDiscovery*    piSD;
};

typedef struct
{
   bt_sd_srv_rec_type      svc_rec; // holds all data extracted from reg string
   bt_sd_uuid_list_type    uuid_list;
   uint16                  ver;
   uint8                   scn; // 0 = unspecified channel
   uint16                  psm; // what's a valid PSM?
   char service_name[BT_SD_MAX_SERVICE_NAME_LEN];
}  BTRegData;

typedef struct
{
   uint32                  uHandle;
}  BTDeregData;

struct _OEMBTSD
{
   IShell*                 piShell;
   uint32                  uRefs;

   bt_app_id_type          appId;
   uint32                  uFlags;
   BTSDEvCb*               pSDEvCbFnList;
   BTRegData               regData;
   BTDeregData             deregData;
   OEMBTSDEvent            oemEvents[10];
};

static OEMBTSD* pgTheOEMBTSD = NULL;
static OEMCriticalSection gOEMBTSDCriticalSection;

#define OEMBTSD_LOCK()    OEMCriticalSection_Enter(&gOEMBTSDCriticalSection)
#define OEMBTSD_UNLOCK()  OEMCriticalSection_Leave(&gOEMBTSDCriticalSection)

#define OEMBTSD_FLAG_OP_BY_ME                   (1 << 0)
#define OEMBTSD_FLAG_DEV_SRCH_PENDING           (1 << 1)
#define OEMBTSD_FLAG_DEV_SRCH_STOPPING          (1 << 2)
#define OEMBTSD_FLAG_NAME_REQ_PENDING           (1 << 3)
#define OEMBTSD_FLAG_NAME_REQ_STOPPING          (1 << 4)
#define OEMBTSD_FLAG_SVC_SRCH_PENDING           (1 << 5)
#define OEMBTSD_FLAG_SVC_SRCH_STOPPING          (1 << 6)
#define OEMBTSD_FLAG_SVC_SET_DISC_MODE_PENDING  (1 << 7)
#define OEMBTSD_FLAG_SVC_REG_PENDING            (1 << 8)
#define OEMBTSD_FLAG_SVC_DEREG_PENDING          (1 << 8)
#define OEMBTSD_FLAG_SVC_RELEASING              (1 << 9)

/************************************************************************ 
                           PRIVATE FUNCTIONS
************************************************************************/
static bt_sd_service_discoverable_mode_type OEMBT_DiscModeBrew2Core (
   BTSvcDiscoverableMode dMode
)
{
   switch (dMode)
   {
      case AEEBTSD_DISCOVERABLE_MODE_NONE:
         return BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;
      case AEEBTSD_DISCOVERABLE_MODE_GENERAL:
         return BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL;
      case AEEBTSD_DISCOVERABLE_MODE_LIMITED:
         return BT_SD_SERVICE_DISCOVERABLE_MODE_LIMITED;
      default: // should never happen
         return BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;
   }
}


static BTSvcDiscoverableMode OEMBT_DiscModeCore2Brew (
   bt_sd_service_discoverable_mode_type mode
)
{
   switch (mode)
   {
      case BT_SD_SERVICE_DISCOVERABLE_MODE_NONE:
         return AEEBTSD_DISCOVERABLE_MODE_NONE;
      case BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL:
         return AEEBTSD_DISCOVERABLE_MODE_GENERAL;
      case BT_SD_SERVICE_DISCOVERABLE_MODE_LIMITED:
         return AEEBTSD_DISCOVERABLE_MODE_LIMITED;
      default: // should never happen
         return AEEBTSD_DISCOVERABLE_MODE_NONE;
   }
}


static int OEMBTSD_WriteAttribute (
   OEMBTSD*                 pMe, 
   bt_sd_srv_rec_type*      dst_rec_ptr,
   bt_sd_srv_attr_rec_type* svc_attr_ptr
)
{
   int result = AEE_EFAILED;
   bt_sd_srv_attr_rec_type* dst_attr_ptr =
      bt_sd_find_service_attribute (dst_rec_ptr, svc_attr_ptr->attr_id);

   if (NULL == dst_attr_ptr)
   {
      dst_attr_ptr = bt_sd_service_record_add_attribute (
                        dst_rec_ptr, svc_attr_ptr->attr_id, 
                        svc_attr_ptr->attr_type);
   }
   else
   {
      bt_sd_service_record_init_attribute (dst_attr_ptr, svc_attr_ptr->attr_id, 
                                           svc_attr_ptr->attr_type);
   }
   if (NULL != dst_attr_ptr)
   {
      memcpy(dst_attr_ptr,svc_attr_ptr, sizeof(bt_sd_srv_attr_rec_type));
      result = AEE_SUCCESS;
   }
   return result;
}


static int OEMBTSD_WriteRecord (
   OEMBTSD*             pMe, 
   bt_sd_srv_rec_type*  dst_rec_ptr,
   bt_sd_srv_rec_type*  src_rec_ptr
)
{
   int result = AEE_SUCCESS;
   int i;
   for (i = 0; 
        (i < src_rec_ptr->num_srv_attr) && (AEE_SUCCESS == result); 
        i++)
   {
      if (BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE ==
          src_rec_ptr->srv_attr[i].attr_id)
      {
         result = AEE_ENOTALLOWED;
      }
      else
      {
         result = OEMBTSD_WriteAttribute (pMe, dst_rec_ptr, 
                                          &src_rec_ptr->srv_attr[i]);
      }
   }
   return result;
}


static void OEMBTSD_UpdateRegServiceRec (
   OEMBTSD*            pMe,
   bt_sd_srv_rec_type* rec_ptr, 
   BTRegData*          pRegData
)
{
   int i = 1;

   // add the remaining 16-bit UUIDs
   if ((pRegData->uuid_list.num_uuid128 == 0) &&
       (pRegData->uuid_list.num_uuid > 1))
   {
      bt_sd_srv_attr_rec_type* dst_attr_ptr =
         bt_sd_find_service_attribute (
            rec_ptr, BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST);
      if (NULL == dst_attr_ptr)
      {
         // how is this even possible???
         MSG_ERROR ("SD_UpdateServiceRec - Cls ID list not found!",0,0,0);
      }
      else
      {
         for (i = 0; i < pRegData->uuid_list.num_uuid; i++)
         {
            bt_sd_service_attribute_add_uuid (dst_attr_ptr,
                                              pRegData->uuid_list.uuid[i]);
         }
      }
   }

   // add/update the attributes
   for (i = 0; i < pRegData->svc_rec.num_srv_attr; i++)
   {
      switch (pRegData->svc_rec.srv_attr[i].attr_id)
      {
         case BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE:
         case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
            // skip these
            break;
         default:
            OEMBTSD_WriteAttribute (pMe, rec_ptr, 
                                    &pRegData->svc_rec.srv_attr[i]);
            break;

      }
   }
}

static void OEMBTSD_ProcessCmdDone (
   OEMBTSD*                pMe, 
   bt_ev_gn_cmd_done_type* ev_ptr,
   OEMBTSDEvent*           pOemEv
)
{
   MSG_LOW ("SD_ProcessCmdDone - cmd=0x%x stat=0x%x", 
              ev_ptr->cmd_type, ev_ptr->cmd_status, 0 );

   pOemEv->dResult = OEMBT_CmdStatus2Result (ev_ptr->cmd_status);
   switch (ev_ptr->cmd_type)
   {
      case BT_CMD_SD_DISCOVER_DEVICES_EXT:
         if (AEEBT_RESULT_SUCCESS == pOemEv->dResult)
         {
            pOemEv->dType = OEMBTSD_EV_DEV_DISCOVERY_STARTED;
         }
         else
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_DEV_SRCH_PENDING;
         }
         break;
      case BT_CMD_SD_STOP_DEVICE_DISCOVERY:
         if (AEEBT_RESULT_SUCCESS != pOemEv->dResult)
         {
            pOemEv->dType = OEMBTSD_EV_DEV_DISCOVERY_COMPLETED;
            pMe->uFlags &= ~OEMBTSD_FLAG_DEV_SRCH_STOPPING;
         }
         // if success, wait to handle DEVICE_DISCOVERY_COMPLETE ev
         break;
      case BT_CMD_SD_GET_DEVICE_NAME:
         if (AEEBT_RESULT_SUCCESS == pOemEv->dResult)
         {
            pOemEv->dType = OEMBTSD_EV_NAME_REQ_STARTED;
         }
         else
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_NAME_REQ_PENDING;
         }
         break;
      case BT_CMD_SD_GET_DEVICE_NAME_CANCEL:
         // don't send this up; wait to handle GetName ev
         break;
      case BT_CMD_SD_SEARCH_SERVICE_EXT:
         if (AEEBT_RESULT_SUCCESS == pOemEv->dResult)
         {
            pOemEv->dType = OEMBTSD_EV_SVC_DISCOVERY_STARTED;
         }
         else
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_SVC_SRCH_PENDING;
         }
         break;
      case BT_CMD_SD_STOP_SERVICE_SEARCH:
         // no other events expected
         if ( pMe->uFlags & OEMBTSD_FLAG_SVC_SRCH_STOPPING)
         {
            pOemEv->dType   = OEMBTSD_EV_SVC_DISCOVERY_COMPLETED;
            pOemEv->dResult = AEEBT_RESULT_UNKNOWN_ERR;
            pOemEv->pszXML  = NULL;
            pMe->uFlags &= ~(OEMBTSD_FLAG_SVC_SRCH_PENDING | 
                             OEMBTSD_FLAG_SVC_SRCH_STOPPING);
         }
         break;
      case BT_CMD_SD_SET_SERVICE_DISCOVERABLE_MODE:
         // no other events expected
         pMe->uFlags &= ~OEMBTSD_FLAG_SVC_SET_DISC_MODE_PENDING;
         pOemEv->dType = OEMBTSD_EV_SET_DISC_MODE_DONE;
         break;
      case BT_CMD_SD_REGISTER_SERV_EXT:
      case BT_CMD_SD_REGISTER_CUSTOM_SERVICE_EXT:
         if (pOemEv->dResult == AEEBT_RESULT_SUCCESS)
         {
            bt_sd_srv_rec_type* rec_ptr = NULL;
            if (pMe->regData.uuid_list.num_uuid128 > 0)
            {
               rec_ptr = bt_sd_find_service_record_by_uuid128 (
                            BT_SD_DB_TYPE_LOCAL_DB,
                            &pMe->regData.uuid_list.uuid128[0],
                            pMe->regData.scn,
                            pMe->regData.psm);
            }
            else
            {
               rec_ptr = bt_sd_find_service_record_by_uuid (
                            BT_SD_DB_TYPE_LOCAL_DB,
                            pMe->regData.uuid_list.uuid[0],
                            pMe->regData.scn,
                            pMe->regData.psm);
            }
            if (NULL == rec_ptr)
            {
               // consider registration failed
               pOemEv->dResult = AEEBT_RESULT_UNKNOWN_ERR;
            }
            else
            {
               // add any additional UUIDs and/or attributes that
               // were in the registration string
               OEMBTSD_UpdateRegServiceRec (pMe, rec_ptr, &pMe->regData);

               // extract record handle
               pOemEv->uVal = rec_ptr->srv_rec_id;
            }
         }
         pMe->uFlags &= ~OEMBTSD_FLAG_SVC_REG_PENDING;
         pOemEv->dType = OEMBTSD_EV_SVC_REG_DONE;
         break;
      case BT_CMD_SD_UNREGISTER_SERVICE:
      case BT_CMD_SD_UNREGISTER_CUSTOM_SERVICE:
         pMe->uFlags  &= ~OEMBTSD_FLAG_SVC_DEREG_PENDING;
         pOemEv->dType = OEMBTSD_EV_SVC_DEREG_DONE;
         pOemEv->uVal  = pMe->deregData.uHandle;
         pMe->deregData.uHandle = 0;
         break;
      case BT_CMD_SD_SEARCH_SERVICE_UUID128:
      //case BT_CMD_SD_UPDATE_SERVICE_RECORD:
      //case BT_CMD_SD_UPDATE_OBEX_PROFILE_SERVICE_RECORD:
      //case BT_CMD_SD_CUSTOM_UPDATE_OBEX_PROFILE_SERVICE_RECORD:
      //case BT_CMD_SD_REGISTER_SYNCML_SERVICE:
      //case BT_CMD_SD_UNREGISTER_SYNCML_SERVICE:
      case BT_CMD_SD_REGISTER_SERVICE:
      case BT_CMD_SD_UPDATE_SRV_REC_BY_SCN:
      case BT_CMD_SD_UPDATE_SRV_REC_BY_PSM:
      case BT_CMD_SD_UPDATE_SRV_NAME:
      case BT_CMD_SD_UNREGISTER_SRV_BY_SCN:
      case BT_CMD_SD_UNREGISTER_SRV_BY_PSM:
      //case BT_CMD_SD_GET_SECONDARY_SERVER_CHANNEL_NUMBER:
      default:
         MSG_ERROR ("SD_ProcessCmdDone - unexp cmd",0,0,0);
         pOemEv->dType = OEMBTSD_EV_NONE;
         break;
   }
   MSG_LOW ("SD_ProcessCmdDone - cmd=0x%x flags=0x%x", 
              pOemEv->dType, pOemEv->dResult,0 );
}

static void OEMBTSD_CoreEv2OemEv (
   OEMBTSD* pMe, 
   bt_ev_msg_type* pEv, 
   OEMBTSDEvent* pOemEv
)
{
   switch (pEv->ev_hdr.ev_type)
   {
      case BT_EV_GN_CMD_DONE:
         OEMBTSD_ProcessCmdDone (pMe, &pEv->ev_msg.ev_gn_cmd_done, pOemEv);
         break;
      case BT_EV_SD_DEVICE_DISCOVERY_RESP:
         pOemEv->dResult = AEE_SUCCESS;
         pOemEv->dType = OEMBTSD_EV_DEV_DISCOVERY_RESPONSE;
         pOemEv->devInfo.bdAddr = 
            *((BDAddress*)(&pEv->ev_msg.ev_sd_dev_discv_resp.bd_addr));
         pOemEv->devInfo.COD.COS = 
            pEv->ev_msg.ev_sd_dev_discv_resp.service_class;
         pOemEv->devInfo.COD.DevCls = 
            pEv->ev_msg.ev_sd_dev_discv_resp.major_device_class;
         pOemEv->devInfo.COD.SubDevCls = 
            pEv->ev_msg.ev_sd_dev_discv_resp.minor_device_class;
         pOemEv->devInfo.COD.SubDevCls -= pOemEv->devInfo.COD.DevCls;
         break;
      case BT_EV_SD_DEVICE_DISCOVERY_COMPLETE:
         pMe->uFlags &=
            ~(OEMBTSD_FLAG_DEV_SRCH_PENDING | OEMBTSD_FLAG_DEV_SRCH_STOPPING);
         pOemEv->dResult = AEE_SUCCESS;
         pOemEv->dType = OEMBTSD_EV_DEV_DISCOVERY_COMPLETED;
         pOemEv->uVal = pEv->ev_msg.ev_sd_dev_discv_cmplt.num_discvd_dev;
         break;
      case BT_EV_SD_DEVICE_NAME_RESP:
         pMe->uFlags &=
            ~(OEMBTSD_FLAG_NAME_REQ_PENDING | OEMBTSD_FLAG_NAME_REQ_STOPPING);
         pOemEv->dType = OEMBTSD_EV_NAME_REQ_COMPLETED;
         if (pEv->ev_msg.ev_sd_dname.reason == BT_EVR_GN_SUCCESS)
         {
            pOemEv->dResult = AEEBT_RESULT_SUCCESS;
            pOemEv->devInfo.bdAddr =
               *((BDAddress*)&pEv->ev_msg.ev_sd_dname.bd_addr);
            std_memmove (pOemEv->devInfo.szName, 
                         (void*)pEv->ev_msg.ev_sd_dname.device_name_str,
                         sizeof (pOemEv->devInfo.szName));
         }
         else
         {
            pOemEv->dResult = AEEBT_RESULT_UNKNOWN_ERR;
            MSG_ERROR ("SD_CoreEv2OemEv - get name err=0x%x",
                             pEv->ev_msg.ev_sd_dname.reason,0,0);
         }
         break;
      case BT_EV_SD_SERVICE_SEARCH_XML_RESP:
      {
         int nBytes = 0;
#ifndef BT_SIMULATION
         dsm_item_type*  dsm_ptr = 
            pEv->ev_msg.ev_sd_service_search_xml_resp.dsm_ptr;
#endif /* BT_SIMULATION */

         pMe->uFlags &=
            ~(OEMBTSD_FLAG_SVC_SRCH_PENDING | OEMBTSD_FLAG_SVC_SRCH_STOPPING);
         pOemEv->dType = OEMBTSD_EV_SVC_DISCOVERY_COMPLETED;
         pOemEv->devInfo.bdAddr = 
            *((BDAddress*)(&pEv->ev_msg.ev_sd_service_search_xml_resp.bd_addr));

#ifndef BT_SIMULATION
         nBytes = dsm_length_packet (dsm_ptr);
#else
         nBytes = strlen (pEv->ev_msg.ev_sd_service_search_xml_resp.xml_str);
         if (0 == nBytes) nBytes = -1;
#endif
         MSG_LOW ("SD_CoreEv2OemEv - dsm_len=%d", nBytes,0,0);
         pOemEv->pszXML = MALLOC (nBytes + 1);
         if (NULL == pOemEv->pszXML)
         {
            MSG_ERROR ("SD_CoreEv2OemEv - MALLOC failed for xml buf",0,0,0);
         }
         else
         {
#ifndef BT_SIMULATION
            int nRead = 0;
            char* psz = pOemEv->pszXML;
            while (nBytes > 0)
            {
               nRead = dsm_pullup (&dsm_ptr, psz, nBytes);
               MSG_LOW ("SD_CoreEv2OemEv - read %d bytes", nRead,0,0);
               nBytes -= nRead;
               psz += nRead;
            }
#else
            memcpy (pOemEv->pszXML,
                    pEv->ev_msg.ev_sd_service_search_xml_resp.xml_str, 
                    nBytes);
#endif
         }
         pOemEv->uVal =
            bt_sd_get_num_service_record (BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB);
         pOemEv->dResult = AEEBT_RESULT_SUCCESS;
         break;
      }
      case BT_EV_SD_TIMEOUT_RESP:
      {
         pMe->uFlags &=
            ~(OEMBTSD_FLAG_SVC_SRCH_PENDING | OEMBTSD_FLAG_SVC_SRCH_STOPPING);
         pOemEv->dType = OEMBTSD_EV_SVC_DISCOVERY_COMPLETED;
         pOemEv->devInfo.bdAddr = 
            *((BDAddress*)(&pEv->ev_msg.ev_sd_timeout_resp.bd_addr));
         pOemEv->pszXML  = NULL;
         pOemEv->dResult = AEEBT_RESULT_REMOTE_TIMEOUT;
         break;
      }
      case BT_EV_SD_ERROR_RESP:
      {
         pMe->uFlags &=
            ~(OEMBTSD_FLAG_SVC_SRCH_PENDING | OEMBTSD_FLAG_SVC_SRCH_STOPPING);
         pOemEv->dType = OEMBTSD_EV_SVC_DISCOVERY_COMPLETED;
         pOemEv->devInfo.bdAddr = 
            *((BDAddress*)(&pEv->ev_msg.ev_sd_error_resp.bd_addr));
         pOemEv->pszXML  = NULL;
         pOemEv->dResult = AEEBT_RESULT_UNKNOWN_ERR; // no appropriate err code to use...
         break;
      }
      default:
         MSG_ERROR ("SD_CoreEv2OemEv - unexp core ev 0x%x",
                          pEv->ev_hdr.ev_type,0,0);
         pOemEv->dType = OEMBTSD_EV_MAX;
         break;
   }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static void OEMBTSD_ReleaseOEMEvent(OEMBTSDEvent* pEv)
{
   OEMBTSD_LOCK();
   pEv->bInUse = FALSE;
   OEMBTSD_UNLOCK();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static OEMBTSDEvent* OEMBTSD_GetFreeOEMEvent(OEMBTSD* pMe)
{
   OEMBTSDEvent* pEv = NULL;
   int nNumEvents = ARR_SIZE(pMe->oemEvents);

   OEMBTSD_LOCK();
   while (0 < nNumEvents--)
   {
      if (FALSE == pMe->oemEvents[nNumEvents].bInUse)
      {
         pEv = &(pMe->oemEvents[nNumEvents]);
         pEv->bInUse = TRUE;
         break;
      }
   }
   OEMBTSD_UNLOCK();

   return pEv;
}

/*-----------------------------------------------------------------------------
// in BREW context
-----------------------------------------------------------------------------*/
static void OEMBTSD_BREWEventCb(void* pUser)
{
   OEMBTSDEvent* pEv  = (OEMBTSDEvent*) pUser;
   if (NULL != pgTheOEMBTSD)
   {
      BTSDEvCb* pEvCb = pgTheOEMBTSD->pSDEvCbFnList;

      while (NULL != pEvCb)
      {
         (pEvCb->pEvCbFn) (pEvCb->piSD, pEv);
         pEvCb = pEvCb->pNext;
      }
   }
   OEMBTSD_ReleaseOEMEvent(pEv);
}

/*-----------------------------------------------------------------------------
// called from BT task context; so just convert and queue the event
-----------------------------------------------------------------------------*/
static void OEMBTSD_EventCallback (bt_ev_msg_type* bt_ev_msg_ptr)
{
   OEMBTSD* pMe = pgTheOEMBTSD;
   OEMBTSDEvent* pEv = NULL;
   
   if ((NULL == pMe) || (NULL == pMe->pSDEvCbFnList) ||
       (bt_ev_msg_ptr->ev_hdr.bt_app_id != pMe->appId))
   {
      MSG_ERROR ("OEMBTSD_EventCallback - dropping ev=%x for app %d", 
                 bt_ev_msg_ptr->ev_hdr.ev_type, 
                 bt_ev_msg_ptr->ev_hdr.bt_app_id, 0);
   }
   else if (NULL == (pEv = OEMBTSD_GetFreeOEMEvent(pMe)))
   {
      MSG_ERROR ("OEMBTSD_EventCallback - failed to get free oem ev for %x", 
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0);
   }
   else
   {
      OEMBTSD_CoreEv2OemEv (pMe, bt_ev_msg_ptr, pEv);
      if ((OEMBTSD_EV_NONE != pEv->dType) && (OEMBTSD_EV_MAX != pEv->dType))
      {
         // will process event in BREW task
         CALLBACK_Init(&pEv->cb, OEMBTSD_BREWEventCb, pEv);
         AEE_SYS_RESUME(&pEv->cb);
      }
      else
      {
         OEMBTSD_ReleaseOEMEvent(pEv);
      }
   }
}


/************************************************************************ 
                       Exposed to AEE layer
************************************************************************/
void OEMBTSD_Release (OEMBTSD* pMe, uint32* puHandle, int nNumHandles)
{
   if ((NULL == pMe) || (pMe != pgTheOEMBTSD))
   {
      DBGPRINTF_ERROR ("OEMBTSD_Release - pMe=0x%x pgTheOEMBTSD=%0x%x",
                       pMe, pgTheOEMBTSD);
   }
   else if (pMe->uRefs > 0)
   {
      int i;
      pMe->uFlags |= OEMBTSD_FLAG_SVC_RELEASING;
      for (i = 0; i < nNumHandles; i++)
      {
         OEMBTSD_DeregisterService (pMe, puHandle[i]);
      }
      pMe->uFlags &= ~OEMBTSD_FLAG_SVC_RELEASING;

      pMe->uRefs--;
      if (0 == pMe->uRefs)
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
         OEM_Free (pMe);
         pgTheOEMBTSD = NULL;
      }
   }
}

int OEMBTSD_New (IShell* piShell, OEMBTSD** ppo)
{
   int result = AEE_EFAILED;

   if (NULL == pgTheOEMBTSD)
   {
      pgTheOEMBTSD = (OEMBTSD*) OEM_Malloc (sizeof(OEMBTSD));
      if (NULL == pgTheOEMBTSD)
      {
         DBGPRINTF_ERROR ("OEMBTSD_New - no mem for obj");
      }
      else if ((pgTheOEMBTSD->appId = 
                bt_cmd_ec_get_app_id_and_register (
                   OEMBTSD_EventCallback)) == BT_APP_ID_NULL)
      {
         DBGPRINTF_ERROR ("OEMBTSD_New - failed to get appID");
      }
      else
      {
         ISHELL_AddRef (piShell);
         pgTheOEMBTSD->piShell = piShell;
         pgTheOEMBTSD->uRefs = 1;
         *ppo = pgTheOEMBTSD;
         result = AEE_SUCCESS;
      }
   }
   else
   {
      pgTheOEMBTSD->uRefs++;
      *ppo = pgTheOEMBTSD;
      result = AEE_SUCCESS;
   }

   if (NULL != pgTheOEMBTSD)
   {
      DBGPRINTF_MED ("OEMBTSD_New - ref count = %d", pgTheOEMBTSD->uRefs);
   }

#ifdef BT_SIMULATION
   //starting remote device emulator
   bt_sim_start(PROFILE_SD, TRUE);
#endif

   return result;
}

void OEMBTSD_DeregisterEvCb (OEMBTSD* pMe, IBTServiceDiscovery* piSD)
{
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && (NULL != piSD))
   {
      BTSDEvCb* pEvCb = pMe->pSDEvCbFnList;
      BTSDEvCb* pPrevEvCb = NULL;

      OEMBTSD_LOCK();
      while (NULL != pEvCb)
      {
         if (pEvCb->piSD == piSD)
         {
            if (NULL != pPrevEvCb)
            {
               pPrevEvCb->pNext = pEvCb->pNext;
            }
            else
            {
               pMe->pSDEvCbFnList = pEvCb->pNext;
            }
            FREE (pEvCb);
            break;
         }
         pPrevEvCb = pEvCb;
         pEvCb = pEvCb->pNext;
      }
      OEMBTSD_UNLOCK();
   }
}

int OEMBTSD_RegisterEvCb (
   OEMBTSD*             pMe, 
   SDEvCallbackFn*      pCbFn, 
   IBTServiceDiscovery* piSD
)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && 
       (NULL != pCbFn) && (NULL != piSD))
   {
      BTSDEvCb** ppEvCb = &pMe->pSDEvCbFnList;
      OEMBTSD_LOCK();
      // find the end of the callback function list
      while ((NULL != ppEvCb) && (NULL != *ppEvCb))
      {
         ppEvCb = &((*ppEvCb)->pNext);
      }
      DBGPRINTF ("SD_RegEvCb - ppEvCb=0x%x", ppEvCb);
      if (NULL != ppEvCb)
      {
         *ppEvCb = MALLOCREC (BTSDEvCb);
         if (NULL != *ppEvCb)
         {
            (*ppEvCb)->pEvCbFn = pCbFn;
            (*ppEvCb)->piSD    = piSD;
            (*ppEvCb)->pNext   = NULL;
            result = AEE_SUCCESS;
         }
      }
      OEMBTSD_UNLOCK();
   }
   return result;
}

int OEMBTSD_StartDevDiscovery (
   OEMBTSD* pMe, 
   BTClassOfService dCoS,
   int nMaxDevs,
   int nDurationSec
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StartDevDisc - f=0x%x", pMe->uFlags);

   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && (pMe->uFlags == 0))
   {
      bt_cmd_status_type stat;
      bt_bd_addr_type    addr = {0,0,0,0,0,0};
      bt_service_class_enum_type bitmap = OEMBT_SvcClsBrew2Core (dCoS);
      pMe->uFlags |= OEMBTSD_FLAG_DEV_SRCH_PENDING;
      stat = bt_cmd_sd_discover_devices_ext (pMe->appId, bitmap, 
                                             &addr, nMaxDevs, nDurationSec);
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_DEV_SRCH_PENDING;
      }
   }
   return result;
}

int OEMBTSD_StopDevDiscovery (OEMBTSD* pMe)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StopDevDisc - f=0x%x", pMe->uFlags);
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) &&
       (pMe->uFlags & OEMBTSD_FLAG_DEV_SRCH_PENDING))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTSD_FLAG_DEV_SRCH_STOPPING;
      stat = bt_cmd_sd_stop_device_discovery (pMe->appId);
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_DEV_SRCH_STOPPING;
      }
   }
   return result;
}

int OEMBTSD_GetName (OEMBTSD* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && (NULL != pAddr) &&
       (pMe->uFlags == 0))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTSD_FLAG_NAME_REQ_PENDING;
      stat = bt_cmd_sd_get_device_name (pMe->appId, (bt_bd_addr_type*)pAddr);
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_NAME_REQ_PENDING;
      }
   }
   return result;
}

int OEMBTSD_GetNameCancel (OEMBTSD* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) &&
       (pMe->uFlags & OEMBTSD_FLAG_NAME_REQ_PENDING))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTSD_FLAG_NAME_REQ_STOPPING;
      stat = bt_cmd_sd_get_device_name_cancel (pMe->appId,
                                               (bt_bd_addr_type*)pAddr);
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_NAME_REQ_STOPPING;
      }
   }
   return result;
}

int OEMBTSD_GetLocalDBRecCount (OEMBTSD* pMe)
{
   // pMe unused
   return (int)bt_sd_get_num_service_record (BT_SD_DB_TYPE_LOCAL_DB);
}

int OEMBTSD_GetLocalDBSummary (
   OEMBTSD* pMe, 
   BTServiceRecordSummary* pRecSummary,
   int nNumRecs
)
{
   // pMe unused
   int result = AEE_EFAILED;
   const bt_sd_srv_rec_type* rec_ptr;
   int idx;
   if (pRecSummary != NULL)
   {
      bt_sd_srv_rec_iter.init (BT_SD_DB_TYPE_LOCAL_DB, BT_SD_ITER_ALL);
      for (rec_ptr = bt_sd_srv_rec_iter.first(), idx = 0;
           (rec_ptr != NULL) && (nNumRecs > idx);
           rec_ptr = bt_sd_srv_rec_iter.next(), idx++)
      {
         boolean bHandleFound = FALSE;
         boolean bUUIDFound = FALSE;
         const bt_sd_srv_attr_rec_type* attr_ptr;

         bt_sd_srv_attr_iter.init( rec_ptr, BT_SD_ITER_ALL );
         for (attr_ptr = bt_sd_srv_attr_iter.first();
              attr_ptr != NULL && !(bHandleFound && bUUIDFound);
              attr_ptr = bt_sd_srv_attr_iter.next())
         {
            if (BT_SD_ATTR_ID_SERVICE_RECORD_HANDLE == attr_ptr->attr_id)
            {
               pRecSummary[ idx ].uHandle = 
                  (uint32) attr_ptr->attr_value.primitive_value;
               bHandleFound = TRUE;
            }
            else if (BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST ==
                     attr_ptr->attr_id)
            {
               pRecSummary[ idx ].profileUUID =
                  attr_ptr->attr_value.proto_desc_list.proto_desc[0].uuid;
               bUUIDFound = TRUE;
            }
         }
      }
      result = AEE_SUCCESS;
   }
   return result;
}

int OEMBTSD_ReadRec (
   OEMBTSD* pMe, 
   uint32   uHandle, 
   char**   ppszRecXML
)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && (NULL != ppszRecXML))
   {
      int nLen = 0;
      bt_sd_srv_rec_type* rec_ptr = 
         bt_sd_find_service_record_by_handle (uHandle);
      if (NULL == rec_ptr)
      {
         result = AEE_ENOSUCH;
      }
      else
      {
         dsm_item_type* dsm_ptr = dsm_new_buffer (DSM_DS_SMALL_ITEM_POOL);
         result = AEE_ENOMEMORY;

         if ((NULL != dsm_ptr) &&
             ((bt_sd_create_xml_service_record (rec_ptr, &dsm_ptr) != FALSE)))
         {
            nLen = dsm_length_packet (dsm_ptr);
            *ppszRecXML = MALLOC (nLen + 1); // for NULL
            if (NULL != *ppszRecXML)
            {
               dsm_pullup (&dsm_ptr, *ppszRecXML, nLen);
               result = AEE_SUCCESS;
            }
            dsm_free_packet (&dsm_ptr);
         }
      }
   }
   return result;
}


int OEMBTSD_WriteRec (
   OEMBTSD*    pMe, 
   uint32      uHandle, 
   const char* pszRecXML
)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && (NULL != pszRecXML))
   {
      bt_sd_srv_rec_type      svc_rec;
      bt_sd_srv_attr_rec_type svc_attr;
      bt_sd_srv_rec_type*     rec_ptr = 
         bt_sd_find_service_record_by_handle (uHandle);

      if (NULL == rec_ptr)
      {
         result = AEE_ENOSUCH;
      }
      else if (bt_sd_xml_parse_service_record ((char*)pszRecXML, 
                                               std_strlen (pszRecXML),
                                               &svc_rec) != FALSE)
      {
         result = OEMBTSD_WriteRecord (pMe, rec_ptr, &svc_rec);
      }
      else if (bt_sd_xml_parse_attribute ((char*)pszRecXML, 
                                          std_strlen (pszRecXML),
                                          &svc_attr) != FALSE)
      {
         result = OEMBTSD_WriteAttribute (pMe, rec_ptr, &svc_attr);
      }
      else
      {
         return AEE_EBADPARM; // can't parse the given string
      }
   }
   return result;
}

int OEMBTSD_GetDiscoverableMode (OEMBTSD* pMe, BTSvcDiscoverableMode* pdMode)
{
   // pMe unused
   int result = AEE_EFAILED;
   if (NULL != pdMode)
   {
      bt_sd_service_discoverable_mode_type mode = 
         bt_sd_get_service_discoverable_mode();

      *pdMode = OEMBT_DiscModeCore2Brew (mode);
      result = AEE_SUCCESS;
   }
   return result;
}

int OEMBTSD_SetDiscoverableMode (OEMBTSD* pMe, BTSvcDiscoverableMode dMode)
{
   int result = AEE_EFAILED;
   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) &&
       ((pMe->uFlags & OEMBTSD_FLAG_SVC_SET_DISC_MODE_PENDING) == 0))
   {
      bt_cmd_status_type stat;

      pMe->uFlags |= OEMBTSD_FLAG_SVC_SET_DISC_MODE_PENDING;
      stat = bt_cmd_sd_set_service_discoverable_mode (
            pMe->appId, OEMBT_DiscModeBrew2Core (dMode));
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_SVC_SET_DISC_MODE_PENDING;
      }
   }
   return result;
}

int OEMBTSD_StartSvcDiscovery (
   OEMBTSD* pMe, 
   const BDAddress* pAddr,
   const BTSvcSearchPattern* pWhat2Search
)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StartSvcDisc - f=0x%x", pMe->uFlags);

   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) &&
       (NULL != pAddr) && (NULL != pWhat2Search) && (pMe->uFlags == 0))
   {
      bt_sd_srch_pat_uuid_list_type    uuid_list;
      bt_sd_srch_pat_attr_id_list_type attr_id_list;

      if ((ARR_SIZE (uuid_list.uuid) < pWhat2Search->nNumUUID16s) ||
          (ARR_SIZE (attr_id_list.attr_id) < pWhat2Search->nNumAttributes))
      {
         result = AEE_EBADPARM;
      }
      else
      {
         bt_cmd_status_type stat;
         int i;

         uuid_list.num_uuid = pWhat2Search->nNumUUID16s;
         std_memmove (uuid_list.uuid, pWhat2Search->UUID16,
                      (uuid_list.num_uuid * sizeof (bt_sd_uuid_type)));
         if (FALSE == pWhat2Search->bUUID128Valid)
         {
            uuid_list.num_uuid128 = 0;
         }
         else
         {
            uuid_list.num_uuid128 = 1;
            std_memmove ((void*)&uuid_list.uuid128, &pWhat2Search->UUID128,
                         sizeof(bt_sd_uuid128_type));
         }
         attr_id_list.num_attr_id = pWhat2Search->nNumAttributes;
         for (i = 0; i < attr_id_list.num_attr_id; i++)
         {
            attr_id_list.attr_id[i].value = pWhat2Search->attribValue[i];
            attr_id_list.attr_id[i].is_range = 
               ((pWhat2Search->attribValue[i] >> 16) > 0) ? TRUE : FALSE;
         }
         pMe->uFlags |= OEMBTSD_FLAG_SVC_SRCH_PENDING;
         stat = bt_cmd_sd_search_service_ext (
                   pMe->appId, 
                   (bt_bd_addr_type*)pAddr,
                   &uuid_list, 
                   &attr_id_list,
                   BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT,
                   TRUE);
         if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_SVC_SRCH_PENDING;
         }
      }
   }
   return result;
}

int OEMBTSD_StopSvcDiscovery (OEMBTSD* pMe, const BDAddress* pAddr)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_StopSvcDisc - f=0x%x", pMe->uFlags);

   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) &&
       (pMe->uFlags & OEMBTSD_FLAG_SVC_SRCH_PENDING))
   {
      bt_cmd_status_type stat;
      pMe->uFlags |= OEMBTSD_FLAG_SVC_SRCH_STOPPING;
      stat = bt_cmd_sd_stop_service_search (pMe->appId,
                                            (bt_bd_addr_type*)pAddr);
      if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
      {
         pMe->uFlags &= ~OEMBTSD_FLAG_SVC_SRCH_STOPPING;
      }
   }
   return result;
}

int OEMBTSD_RegisterService (OEMBTSD* pMe, const char* pszRegXML, boolean bConn)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_RegSvc - f=0x%x", pMe->uFlags);

   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && 
       (0 == pMe->uFlags) && (NULL != pszRegXML))
   {
      bt_cmd_status_type stat;
      std_memset(&pMe->regData, 0, sizeof(pMe->regData));

      pMe->uFlags |= OEMBTSD_FLAG_SVC_REG_PENDING;
      if (bt_sd_xml_parse_service_record ((char*)pszRegXML, 
                                          std_strlen (pszRegXML),
                                          &pMe->regData.svc_rec) == FALSE)
      {
         DBGPRINTF_ERROR ("SD_RegSvc - failed to parse svc rec");
      }
      else
      {
         int i;

         for (i = 0; i < pMe->regData.svc_rec.num_srv_attr; i++)
         {
            switch (pMe->regData.svc_rec.srv_attr[i].attr_id)
            {
               case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
                  pMe->regData.uuid_list = 
                     pMe->regData.svc_rec.srv_attr[i].attr_value.uuid_list;
                  break;
               case BT_SD_ATTR_ID_SERVICE_NAME:
                  std_strlcpy (pMe->regData.service_name,
                               pMe->regData.svc_rec.srv_attr[i].attr_value.str,
                               sizeof (pMe->regData.service_name));
                  break;
               case BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST:
               {
                 int j;
                 bt_sd_proto_desc_list_type* pd_list =
                    &pMe->regData.svc_rec.srv_attr[i].attr_value.proto_desc_list;
                 for (j = 0; j < pd_list->num_proto_desc; j++)
                 {
                   /* See both L2CAP PSM and RFCOMM SCN */
                   if ((pd_list->proto_desc[j].uuid == BT_SD_PROTOCOL_L2CAP) &&
                       (pd_list->proto_desc[j].num_param > 0))
                   {
                      pMe->regData.psm = 
                         (uint16) pd_list->proto_desc[j].param[0].value;
                   }
                   if ((pd_list->proto_desc[j].uuid == BT_SD_PROTOCOL_RFCOMM) &&
                       (pd_list->proto_desc[j].num_param > 0))
                   {
                      pMe->regData.scn =
                         (uint8) pd_list->proto_desc[j].param[0].value;
                   }
                 }
                 break;
               }
               case BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST:
               {
                  int j;
                  bt_sd_proto_desc_list_type* pd_list =
                     &pMe->regData.svc_rec.srv_attr[i].attr_value.proto_desc_list;
                  for (j = 0; j < pd_list->num_proto_desc; j++)
                  {
                     /* Profile version */
                     if ((pd_list->proto_desc[j].num_param > 0) &&
                          (2 == pd_list->proto_desc[j].param[0].size))
                     {
                        pMe->regData.ver = 
                          (uint16) pd_list->proto_desc[j].param[0].value;             
                        break;
                     }
                  }
                  break;
               }
            }
         }

         if (pMe->regData.uuid_list.num_uuid128 > 0)
         {
            stat = bt_cmd_sd_register_custom_service_ext (
                      pMe->appId, &pMe->regData.uuid_list, 
                      pMe->regData.ver, pMe->regData.scn, pMe->regData.psm, 
                      bConn, pMe->regData.service_name);
         }
         else
         {
            stat = bt_cmd_sd_register_serv_ext (
                      pMe->appId, pMe->regData.uuid_list.uuid[0], 
                      pMe->regData.ver, pMe->regData.scn, pMe->regData.psm, 
                      bConn, pMe->regData.service_name);
         }
         if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_SVC_REG_PENDING;
         }
      }
   }
   return result;
}

int OEMBTSD_DeregisterService (OEMBTSD* pMe, uint32 uHandle)
{
   int result = AEE_EFAILED;
   DBGPRINTF ("SD_DeregSvc - f=0x%x", pMe->uFlags);

   if ((NULL != pMe) && (pgTheOEMBTSD == pMe) && 
       ((0 == pMe->uFlags) || (pMe->uFlags & OEMBTSD_FLAG_SVC_RELEASING)))
   {
      const bt_sd_srv_rec_type* rec_ptr = 
         bt_sd_find_service_record_by_handle (uHandle);

      if (NULL == rec_ptr)
      {
         result = AEE_ENOSUCH;
      }
      else
      {
         bt_cmd_status_type stat;
         pMe->uFlags |= OEMBTSD_FLAG_SVC_DEREG_PENDING;
         pMe->deregData.uHandle = uHandle;
         if (rec_ptr->is_custom_srv == FALSE)
         {
            stat = bt_cmd_sd_unregister_service (pMe->appId, rec_ptr->srv_uuid);
         }
         else
         {
            stat = bt_cmd_sd_unregister_custom_service (pMe->appId, 
                                                        &rec_ptr->srv_uuid128);
         }
         if ((result = OEMBT_CmdStatus2AEEResult (stat)) != AEE_SUCCESS)
         {
            pMe->uFlags &= ~OEMBTSD_FLAG_SVC_DEREG_PENDING;
         }
      }
   }
   return result;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT) */


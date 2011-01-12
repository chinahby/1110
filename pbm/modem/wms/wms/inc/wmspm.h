#ifndef WMSPM_H
#define WMSPM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W i r e l e s s     M e s s a g i n g  S e r v i c e s
              -- API definitions for Power Management

DESCRIPTION
  This module defines the data types and functions for the Short Message
  Services (SMS) including EMS and Broadcast SMS in CDMA 1x and GSM/GPRS/UMTS
  systems which are used by the client software and are part of the
  UAPI (User Application Programming Interface).

Copyright (c) 1998,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008
              by QUALCOMM, Incorporated.
All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wms.h_v   1.22   19 Jul 2002 08:59:16   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc/wmspm.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
01/31/08   PMD     Updated Copyright Information to include 2008
04/18/07   HQ      Added guards against C++ style linking.
04/09/07   HQ      Re-org API header files.
03/29/07   HQ      Initial version for supporting Power Management
                   for Dual CPU chipsets.
===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "wms.h"


#ifdef __cplusplus
extern "C" {
#endif

/* <EJECT> */
/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/* ========================================================================= */
/* ========================== Common data types ============================ */
/* ========================================================================= */



typedef enum
{
  WMS_PM_NOTIFY_NONE   = 0,      /* Don't notify any events */
  WMS_PM_NOTIFY_ALL,             /* Notify based on event masks */
  WMS_PM_NOTIFY_DEFAULT_FILTER,  /* Notify based on default filtering */
  WMS_PM_NOTIFY_CUSTOM_FILTER,   /* Notify based on customized filtering */
  WMS_PM_NOTIFY_MAX
} wms_pm_notify_e_type;

typedef enum
{
  WMS_CLIENT_PROCESSOR_NONE   = 0,
  WMS_CLIENT_PROCESSOR_MODEM,
  WMS_CLIENT_PROCESSOR_APP1,
  WMS_CLIENT_PROCESSOR_APP2,
  WMS_CLIENT_PROCESSOR_MAX
} wms_client_processor_e_type;


/* CFG Event Masks
*/
#define WMS_CFG_EVENT_MASK_GW_READY          (1 << WMS_CFG_EVENT_GW_READY)
#define WMS_CFG_EVENT_MASK_CDMA_READY        (1 << WMS_CFG_EVENT_CDMA_READY)
#define WMS_CFG_EVENT_MASK_ROUTES            (1 << WMS_CFG_EVENT_ROUTES)
#define WMS_CFG_EVENT_MASK_MEMORY_STATUS     (1 << WMS_CFG_EVENT_MEMORY_STATUS)
#define WMS_CFG_EVENT_MASK_MESSAGE_LIST      (1 << WMS_CFG_EVENT_MESSAGE_LIST)
#define WMS_CFG_EVENT_MASK_MEMORY_FULL       (1 << WMS_CFG_EVENT_MEMORY_FULL)
#define WMS_CFG_EVENT_MASK_GW_DOMAN_PREF     (1 << WMS_CFG_EVENT_GW_DOMAIN_PREF)
#define WMS_CFG_EVENT_MASK_CELL_CHANGE       (1 << WMS_CFG_EVENT_CELL_CHANGE)
#define WMS_CFG_EVENT_MASK_PRIMARY_CLIENT_SET (1 << WMS_CFG_EVENT_PRIMARY_CLIENT_SET)
#define WMS_CFG_EVENT_MASK_MEMORY_STATUS_SET (1 << WMS_CFG_EVENT_MEMORY_STATUS_SET)
#define WMS_CFG_EVENT_MASK_LINK_CONTROL      (1 << WMS_CFG_EVENT_LINK_CONTROL)


/* MSG Event Masks
*/
#define WMS_MSG_EVENT_MASK_SEND                 (1 << WMS_MSG_EVENT_SEND)
#define WMS_MSG_EVENT_MASK_ACK                  (1 << WMS_MSG_EVENT_ACK)
#define WMS_MSG_EVENT_MASK_READ                 (1 << WMS_MSG_EVENT_READ)
#define WMS_MSG_EVENT_MASK_WRITE                (1 << WMS_MSG_EVENT_WRITE)
#define WMS_MSG_EVENT_MASK_DELETE               (1 << WMS_MSG_EVENT_DELETE)
#define WMS_MSG_EVENT_MASK_DELETE_ALL           (1 << WMS_MSG_EVENT_DELETE_ALL)
#define WMS_MSG_EVENT_MASK_MODIFY_TAG           (1 << WMS_MSG_EVENT_MODIFY_TAG)
#define WMS_MSG_EVENT_MASK_READ_TEMPLATE        (1 << WMS_MSG_EVENT_READ_TEMPLATE)
#define WMS_MSG_EVENT_MASK_WRITE_TEMPLATE       (1 << WMS_MSG_EVENT_WRITE_TEMPLATE)
#define WMS_MSG_EVENT_MASK_DELETE_TEMPLATE      (1 << WMS_MSG_EVENT_DELETE_TEMPLATE)
#define WMS_MSG_EVENT_MASK_DELETE_TEMPLATE_ALL  (1 << WMS_MSG_EVENT_DELETE_TEMPLATE_ALL)
#define WMS_MSG_EVENT_MASK_READ_STS_REPORT      (1 << WMS_MSG_EVENT_READ_STS_REPORT)
#define WMS_MSG_EVENT_MASK_WRITE_STS_REPORT     (1 << WMS_MSG_EVENT_WRITE_STS_REPORT)
#define WMS_MSG_EVENT_MASK_DELETE_STS_REPORT    (1 << WMS_MSG_EVENT_DELETE_STS_REPORT)
#define WMS_MSG_EVENT_MASK_DELETE_STS_REPORT_ALL (1 << WMS_MSG_EVENT_DELETE_STS_REPORT_ALL)
#define WMS_MSG_EVENT_MASK_RECEIVED_MESSAGE     (1 << WMS_MSG_EVENT_RECEIVED_MESSAGE)
#define WMS_MSG_EVENT_MASK_SUBMIT_REPORT        (1 << WMS_MSG_EVENT_SUBMIT_REPORT)
#define WMS_MSG_EVENT_MASK_STATUS_REPORT        (1 << WMS_MSG_EVENT_STATUS_REPORT)
#define WMS_MSG_EVENT_MASK_MT_MESSAGE_ERROR     (1 << WMS_MSG_EVENT_MT_MESSAGE_ERROR)
#define WMS_MSG_EVENT_MASK_ACK_REPORT           (1 << WMS_MSG_EVENT_ACK_REPORT)
#define WMS_MSG_EVENT_MASK_DUPLICATE_CB_PAGE    (1 << WMS_MSG_EVENT_DUPLICATE_CB_PAGE)


/* DC Event Masks
*/
#define WMS_DC_EVENT_MASK_INCOMING             (1 << WMS_DC_EVENT_INCOMING)
#define WMS_DC_EVENT_MASK_CONNECTED            (1 << WMS_DC_EVENT_CONNECTED)
#define WMS_DC_EVENT_MASK_ABORTED              (1 << WMS_DC_EVENT_ABORTED)
#define WMS_DC_EVENT_MASK_DISCONNECTED         (1 << WMS_DC_EVENT_DISCONNECTED)
#define WMS_DC_EVENT_MASK_CONNECTING           (1 << WMS_DC_EVENT_CONNECTING)
#define WMS_DC_EVENT_MASK_ENABLE_AUTO_DISCONNECT  (1 << WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT)
#define WMS_DC_EVENT_MASK_DISABLE_AUTO_DISCONNECT (1 << WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT)


/* BC_MM Event Masks
*/
#define WMS_BC_MM_EVENT_MASK_CONFIG          (1 << WMS_BC_MM_EVENT_CONFIG)
#define WMS_BC_MM_EVENT_MASK_PREF            (1 << WMS_BC_MM_EVENT_PREF)
#define WMS_BC_MM_EVENT_MASK_TABLE           (1 << WMS_BC_MM_EVENT_TABLE)
#define WMS_BC_MM_EVENT_MASK_SRV_IDS         (1 << WMS_BC_MM_EVENT_SRV_IDS)
#define WMS_BC_MM_EVENT_MASK_SRV_INFO        (1 << WMS_BC_MM_EVENT_SRV_INFO)
#define WMS_BC_MM_EVENT_MASK_SRV_UPDATED     (1 << WMS_BC_MM_EVENT_SRV_UPDATED)
#define WMS_BC_MM_EVENT_MASK_ADD_SRVS        (1 << WMS_BC_MM_EVENT_ADD_SRVS)
#define WMS_BC_MM_EVENT_MASK_DELETE_SRVS     (1 << WMS_BC_MM_EVENT_DELETE_SRVS)
#define WMS_BC_MM_EVENT_MASK_SELECT_ALL_SRVS (1 << WMS_BC_MM_EVENT_SELECT_ALL_SRVS)
#define WMS_BC_MM_EVENT_MASK_SET_PRIORITY_ALL_SRVS (1 << WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS)
#define WMS_BC_MM_EVENT_MASK_READING_PREF    (1 << WMS_BC_MM_EVENT_READING_PREF)
#define WMS_BC_MM_EVENT_MASK_DELETE_ALL_SRVS (1 << WMS_BC_MM_EVENT_DELETE_ALL_SRVS)
#define WMS_BC_MM_EVENT_MASK_ENABLE_FAILURE  (1 << WMS_BC_MM_EVENT_ENABLE_FAILURE)
#define WMS_BC_MM_EVENT_MASK_DISABLE_FAILURE (1 << WMS_BC_MM_EVENT_DISABLE_FAILURE)


/* DBG Event Masks
*/
#define WMS_DBG_EVENT_MASK_RESET_TL_SEQ_NUM (1 << WMS_DBG_EVENT_RESET_TL_SEQ_NUM)
#define WMS_DBG_EVENT_MASK_SET_SIM_MESSAGE_NUMBER  (1 << WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER)
#define WMS_DBG_EVENT_MASK_CLEAR_SMMA_FLAG  (1 << WMS_DBG_EVENT_CLEAR_SMMA_FLAG)
#define WMS_DBG_EVENT_MASK_RETRY_INTERVAL   (1 << WMS_DBG_EVENT_RETRY_INTERVAL)



/*=========================================================================
FUNCTION
  wms_client_init_for_pm

DESCRIPTION
  Allow the client to register itself with the API for the purpose of
  power management.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init_for_pm
(
  wms_client_type_e_type       client_type,
  wms_client_id_type           * client_id_ptr,
  wms_client_processor_e_type  processor
);
/*~ FUNCTION wms_client_init_for_pm */
/*~ PARAM INOUT client_id_ptr POINTER */


/*=========================================================================
FUNCTION
  wms_client_reg_cfg_cb_for_pm

DESCRIPTION
  Allow the client to register its configuration event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_cfg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         cfg_notify_type,
  uint32                       cfg_event_mask,
  wms_cfg_event_cb_type        cfg_event_cb
);
/*~ FUNCTION wms_client_reg_cfg_cb_for_pm */


/*=========================================================================
FUNCTION
  wms_client_reg_msg_cb_for_pm

DESCRIPTION
  Allow the client to register its  event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         msg_notify_type,
  uint32                       msg_event_mask,
  wms_msg_event_cb_type        msg_event_cb
);
/*~ FUNCTION wms_client_reg_msg_cb_for_pm */


/*=========================================================================
FUNCTION
  wms_client_reg_dc_cb_for_pm

DESCRIPTION
  Allow the client to register its  event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dc_notify_type,
  uint32                       dc_event_mask,
  wms_dc_event_cb_type         dc_event_cb
);
/*~ FUNCTION wms_client_reg_dc_cb_for_pm */

/*=========================================================================
FUNCTION
  wms_client_reg_bc_mm_cb_for_pm

DESCRIPTION
  Allow the client to register its  event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         bc_mm_notify_type,
  uint32                       bc_mm_event_mask,
  wms_bc_mm_event_cb_type      bc_mm_event_cb
);
/*~ FUNCTION wms_client_reg_bc_mm_cb_for_pm */


/*=========================================================================
FUNCTION
  wms_client_reg_dbg_cb_for_pm

DESCRIPTION
  Allow the client to register its  event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dbg_notify_type,
  uint32                       dbg_event_mask,
  wms_dbg_event_cb_type        dbg_event_cb
);
/*~ FUNCTION wms_client_reg_dbg_cb_for_pm */

#ifdef __cplusplus
}
#endif


#endif /* WMSPM_H */

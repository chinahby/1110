#ifndef WMSMSG_H
#define WMSMSG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           -- Message Handling Group

DESCRIPTION
  This module defines the data types and functions for the Short Message
  Services (SMS) which are used by the client software and are part of the
  UAPI (User Application Programming Interface).

  Copyright (c) 2001,2002,2003,2004,2005,2006,2007,2008
                by QUALCOMM, Incorporated.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsmsg.h_v   1.8   17 Jun 2002 15:47:58   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc_deprecated/wmsmsg.h#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/08   YC      Disabled MO msg retry if DC can't be established
06/24/08   PMD     Removed Tabs from code
06/04/08   PMD     Removed support for Auto / Transparent SMS 
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
02/16/08   PMD     Fixed compile error on 1x targets
02/14/08   PMD     Added support for common type wms_cm_conn_id_type
01/31/08   PMD     Updated Copyright Information to include 2008
07/20/07   PMD     Added support for correct Ack_Report Event
06/27/07   PMD     Moved defines to wmspriv.h
06/20/07   PMD     Cleaned up file for internal functions - moved to wmsts
05/03/07   HQ      Mainlined some IWMSC fields.
04/26/07   HQ      Added support for IWMSC.
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
11/13/06   HQ      Added WMS_BD_ESN.
08/26/06   PMD     Added support for EVENT_MO_SMS_RETRY_ATTEMPT
                   Added Function Headers
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
05/11/06   PMD     Changed T_QSC6010 feature to FEATURE_LOW_MEMORY_USAGE
05/03/06   HQ      Added auto-dc send feature.
04/20/06   PMD     Eliminated MO Ack Timer, Pending Ack Timer Logic for CDSMS
                   Updated MO SMS and ACK Retry Logic and Record Type.
                   Mainlined FEATURE_SMS_RETRY.
                   Added Function Headers.
04/11/06   HQ      Updated the comment headers.
04/07/06   HQ      Added wms_msg_cdma_mt_busy() and wms_msg_cdma_mo_busy().
01/25/06   VK      Updated Copyright Info
11/18/05   VK      Cleaned up Lint warnings. Removed some redudant code. 
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/18/05   VK      Added feature FEATURE_ACP for awi.
                   Decreased the MO MT records for sc1x targets.
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Support for New Retry Logic in WMS
10/17/05   PMD     Added msg_s_ptr->mo_retry_interval
07/09/05   PMD     Exposed wms_msg_total_memory_full() prototype
05/05/05   Rex     Added function prototype wms_msg_offline
01/17/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
01/15/05   VK      Added support for 36 digit string
11/16/04   PMD     Changed Prototype for wms_msg_SMMA_conf_proc
11/03/04   PMD     Increased MO and MT Record Queues to 20.
07/15/04   PMD     Added prototype - wms_msg_gw_cb_duplicate_ind_proc
05/29/04   Rex     Added new structure wms_msg_gw_cb_receive_log_type
04/13/04   HQ      Changed smi in mo_rec from byte to uint16.
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
01/16/03   PMD     Featurization Fixes.
                   Added prototypes for retry
01/16/04   AP/PMD  Added GW and CDMA Retry
01/13/04   HQ      Added prototype of wms_msg_cdma_check_dups().
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
08/28/03   AP      Added mt_rec_user_data to hold user_data when acking.
08/21/03   AP      Moved prototype for wms_msg_evt_mo_sms_status outside of
                   Feature flag.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/26/03   PMD     Changes to suppress / remove Lint Errors
07/10/03   Rex     Added New Prototype for wms_msg_evt_mo_sms_status
07/01/03   PMD     Added New Prototype for wms_msg_check_in_use_records
04/16/03   HQ      Added support of FEATURE_CDSMS_IS637B:
                     - Deposit Index in Deliver and User Ack Messages
                     - Delivery Status in Delivery Ack Message
02/27/03   HQ      Added support for FEATURE_GWSMS_MO_CONTROL.
02/14/03   AP      Added support for JCDMA.
02/07/03   SUN     Added Support for Status Reports and Commands
01/24/03   AP      Added client_id to record type.
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
11/13/02   AP      Added support for duplicate detection and simultaneou MO SMS
                   w/o requesting a tl ack, changed value of DUMMY_SEQ_NUM.
10/23/02   HQ      Added support of Brew SMS prefix.
07/31/02   SUN     Changes Prototype for wms_msg_SMMA_conf_proc
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
06/10/02   HQ      Added WMS_MSG_SMMA_REQUEST_ID.
06/03/02   SUN     Removed wms_msg_s_ptr().
05/14/02   SUN     Fixed SMMA Issue
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
04/24/02   HQ      Added wms_msg_s_ptr().
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
06/29/01   HQ      Initial version for WCDMA & GSM SMS.

===========================================================================*/

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


#include "wms.h"
#include "wmsi.h"
#include "wmsll.h"

#ifdef FEATURE_GSTK
#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */
#endif /* FEATURE_GSTK */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

/* forward declaration
*/
typedef struct wms_cfg_struct  cfg_s_type;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

/* Parameter lengths
*/
enum{ WMS_OTA_ADDRESS_MAX      = ( WMS_ADDRESS_MAX + 5 ) };
enum{ WMS_OTA_SUBADDRESS_MAX   = ( WMS_SUBADDRESS_MAX + 4 ) };
enum{ WMS_OTA_CAUSE_CODES_MAX  = 4 };

enum{ WMS_OTA_TL_ACK_MAX       = ( 1 + WMS_OTA_ADDRESS_MAX +
                                         WMS_OTA_SUBADDRESS_MAX +
                                         WMS_OTA_CAUSE_CODES_MAX ) };

enum{ WMS_OTA_PARM_MAX        =  WMS_MAX_LEN };

enum{ WMS_EP_MSG_LEN_BITS     =  5 };
enum{ WMS_EP_MST_BITS         =  8 };
enum{ WMS_EP_CB_DIGITS        =  36 };

enum{ WMS_A_CLI_LEN           = 40 };
enum{ WMS_A_VOICE_MAIL_LEN    = 30 };
enum{ WMS_A_SHORT_MSG_LEN     = 30 };

enum{ WMS_MWI_LEN             = 1 };

/* SMS Transport Layer sequence number is 6 bits (0-63).
*/
enum{ WMS_TL_SEQ_NUM_MAX        = 64 };

#define WMS_BREW_SMS_PREFIX       "//BREW:"
#define WMS_BREW_SMS_PREFIX_LEN   7

/* Analog AWI SMS flag
*/
typedef enum
{
  WMS_AWI_INTERMEDIATE = 0,
  WMS_AWI_FINAL        = 1,
  WMS_AWI_FIRST        = 2,
  WMS_AWI_UNSEGMENTED  = 3

} wms_awi_flag_e_type;



/* Struct for MC task to send sms
*/
typedef struct
{
  wms_OTA_message_type     ota;      /* the raw SMS data to be sent */
  wms_address_s_type       address;  /* SMS destination addr for auth. */

} wms_cdma_sms_type;

#endif /* FEATURE_CDSMS */

/* Message record state
*/
/* NOTE:
   For MO msg: IDLE->QUEUED->LAERY2_IN_PROGRESS->WAIT_FOR_ACK->IDLE
   For MT msg: IDLE->WAIT_FOR_ACK->QUEUED->LAYER2_IN_PROGRESS->IDLE
*/
typedef enum
{
  WMS_MSG_STATE_IDLE   = 0,
  WMS_MSG_STATE_QUEUED,
  WMS_MSG_STATE_LAYER2_IN_PROGRESS,
  WMS_MSG_STATE_WAIT_FOR_ACK

} wms_msg_state_e_type;

#ifdef FEATURE_CDSMS

#define WMS_RUIM_SMS_MAX   255

typedef PACKED struct
{
  uint8                     status;
  wms_OTA_message_type      ota;
} wms_msg_ruim_sms_s_type;


#endif /* FEATURE_CDSMS */

#ifdef FEATURE_CDSMS
/* Duplicate Detection info
*/
typedef struct
{
  wms_message_index_type          index;
  wms_memory_store_e_type         mem_store;
  /* Mask for tl parameters */
  uint16                          tl_mask;
  /* Mask for client bd optional parameters */
  uint32                          cl_mask;
  /* Parameters to compare for uniqueness  IS_637A: 4.3.1.6 */
  wms_message_id_s_type           msg_id;
  wms_address_s_type              address;
  wms_subaddress_s_type           sub_address;
  wms_timestamp_s_type            mc_time;
} wms_msg_dup_detect_info_s_type;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

void wms_msg_offline
(
  void
);


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


void wms_msg_init
(
  void
);

void wms_msg_process_cmd
(
  wms_cmd_type  *cmd_ptr
);

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_msg

DESCRIPTION
  This function receives an SMS message and sends a command to the SMS task
  for processing.
  For Analog AWI SMS messages, segments are assembled into a complete
  message before being sent to the SMS task and an ack order is returned
  for intermediate segments.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the command queue. AWI SMS info is updated for AWI SMS
  segments.

=========================================================================*/
extern void  wms_msg_cdma_mt_msg
(
  wms_format_e_type      format,

  /* length of sms message in bytes */
  uint16                   sms_len,

  /* Pointer to received (mobile-terminated) SMS message */
  uint8                   * sms_ptr,
  wms_bearer_e_type       bearer,
  boolean                   ack_pending,
  wms_transaction_id_type   mt_iwmsc_tid
);

/*=========================================================================
FUNCTION
  wms_msg_cdma_mc_mo_status

DESCRIPTION
  This function reports the status of SMS data burst origination to the SMS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the SMS command queue.

=========================================================================*/
extern void wms_msg_cdma_mc_mo_status
(
  wms_error_class_e_type          error_class,
  wms_status_e_type               status
);

/*=========================================================================
FUNCTION
  wms_msg_cdma_mc_state_changed

DESCRIPTION
  This function notifies the SMS task that the MC's state has changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the SMS command queue.

=========================================================================*/
extern void wms_msg_cdma_mc_state_changed
(
  void
);

#ifndef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_ack_timer_proc

DESCRIPTION
  This function processed the MT SMS Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_cdma_mt_ack_timer_proc
(
  void
);
#endif

/*=========================================================================
FUNCTION
  wms_msg_awi_ack_timer_proc

DESCRIPTION
  This function processes the AWI Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_awi_ack_timer_proc
(
  void
);

/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status_proc

DESCRIPTION
  This function processed the MO SMS Status WMS Command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_mc_mo_status_proc
(
  wms_cmd_type    *cmd_ptr
);

/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status

DESCRIPTION
  This function processed the MO SMS Status recieved from MC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Posts a command to WMS Command Queue

=========================================================================*/
void wms_msg_mc_mo_status
(
  wms_error_class_e_type     error_class,
  wms_status_e_type          status,
  wms_bearer_e_type          bearer
);


void wms_msg_send_scpt_result
(
  wms_cmd_type          * cmd_ptr,
  wms_tl_message_type   * tl_ptr
);

#endif /* FEATURE_CDSMS */

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_status

DESCRIPTION
  Event Notification function for MO SMS Status to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_status
(
  wms_msg_event_info_s_type  * msg_evt_ptr,
  wms_client_msg_hdr_s_type  * msg_hdr_ptr
);

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_retry_attempt

DESCRIPTION
  Event Notification function for MO SMS Retry Attempt to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_retry_attempt
(
  uint32 transaction_id,
  uint32 record_id,
  uint8  prev_status_report,
  uint8  prev_cause_value,
  uint32 retry_time_left
);

wms_transaction_id_type wms_msg_get_next_tid
(
  void
);

boolean wms_msg_check_in_use_records
(
  void
);

#ifdef FEATURE_CDSMS
boolean wms_msg_cdma_check_dups
(
  const wms_tl_message_type     *new_tl_msg
);


/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_busy

DESCRIPTION
  Check whether CDMA MT records are in use or not.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mt_busy
(
  void
);

/*=========================================================================
FUNCTION
  wms_msg_cdma_mo_busy

DESCRIPTION
  Check whether CDMA MO records are in use or not.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mo_busy
(
  void
);


/*=========================================================================
FUNCTION
  wms_msg_auto_dc_send

DESCRIPTION
  Automatically send large messages when DC is connected.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_dc_send
(
  void
);

/*=========================================================================
FUNCTION
  wms_msg_stop_dc_resend

DESCRIPTION
  Stop resending large messages over DC when it cannot be established.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_stop_dc_resend
(
  void
);

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#ifndef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_set_retry_timer

DESCRIPTION
  This function Sets the Global Retry Timer to tick every second.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_set_retry_timer
(
  void
);

/*=========================================================================
FUNCTION
  wms_msg_process_retry_sig

DESCRIPTION
  This function processed the Retry Signal

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_process_retry_sig
(
  void
);
#endif
#ifdef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_mo_timer

DESCRIPTION
  MO Message Timer callback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_mo_timer(int4 ms_interval, void *user_data_ptr);

/*=========================================================================
FUNCTION
  wms_mt_timer

DESCRIPTION
  MT Message Timer callback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_mt_timer(int4 ms_interval, void *user_data_ptr);
#endif
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSMSG_H */


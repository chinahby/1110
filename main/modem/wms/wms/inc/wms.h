#ifndef WMS_H
#define WMS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W i r e l e s s     M e s s a g i n g  S e r v i c e s

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
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc/wms.h#5 $ $DateTime: 2009/11/06 09:53:03 $ $Author: yongzec $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/08   HN      Limited EMS support to reduce memory footprint
10/23/08   HKM     Added missing "*" in HTORPC Meta comment of wms_client_init() 
                   to indicate dereferenced client_id_ptr
10/22/08   HKM     Updated HTORPC Meta comment of wms_client_init() 
                   to include parameter client_id_ptr for wms_client_release.
07/28/08   PMD     Removed Featurizations
07/10/08   PMD     Added RPC Metacomments - ONERROR, RELEASE_FUNC, CALLBACK
06/26/08   PMD     Removed more featurizations in code
06/23/08   PMD     Removed FEATURE_SMS_500_MESSAGES from code
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/07/08   PMD     Added Client ID for RIL
04/22/08   PMD     Added WMS_CMD_ERR_CS_ERROR for Component Service Errors
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
01/31/08   PMD     Updated Copyright Information to include 2008
01/14/07   PMD     Added new error codes in wms_cmd_err_e_type
10/15/07   PMD     Fixed typo in WMS RPC Meta Comment
09/04/07   PMD     Reverted back adding MMGSDI as client
08/30/07   PMD     Added MMGSDI as WMS Client
07/20/07   PMD     Updated WMS Version to 0x500
07/20/07   PMD     Moved TS functions from wmsts.h
07/03/07   PMD     Fix incorrect value of WMS_TP_CAUSE_UNSPECIFIED_PID_ERROR
05/15/07   PMD     Changes to not export WMS TS Utilities to Apps Processor
                   updated API version to 0x04A3
04/26/07   HQ      Added WMS_TELESERVICE_IMSST and IMS Client Type;
                   updated API version to 0x04A1.
04/09/07   HQ      Re-org API header files.
03/29/07   HQ      Added support for Power Collapse/Power Management
                   for Dual CPU chipsets;
                   API version updated to 0x04A0.
03/16/07   HQ      Added support for LPM; API version updated to 0x0494.
03/06/07   HQ      Added support for IWMSC.
02/28/07   HQ      Added support for FEATURE_SMS_OVER_IP;
                   added new SIP errors;
                   updated API version to 0x0493.
02/14/07   PMD     Added Support for NO_RESPONSE_FROM_NETWORK
02/13/07   HQ      Added support for Extended/Long SMS.
02/08/07   HQ      Updated WMS_API_VERSION to 0x0491.
01/31/07   PMD     Added Command Error - WMS_CMD_ERR_NO_NETWORK
01/23/07   HQ      Added additional WMS status codes for Blocked and Busy.
01/05/07   HQ      Updated WMS_API_VERSION to 0x0490;
                   Updated copyright info.
11/17/06   PMD     Updated RPC Meta Comments for scpt_data_ptr and scpt_result_ptr
                   Added comments for certain functions for use of pointers
11/15/06   PMD     Updated WMS RPC Comments; Removed Feature define
11/13/06   HQ      Updated WMS_API_VERSION to 0x0481;
                   added WMS_ESN_MISMATCH_S;
                   added wms_ts_decode_cdma_bd_with_other().
11/06/06   HQ      Added generic CDMA BD parameters as 'other' to support
                   proprietary CDMA parameters.
10/25/06   PMD     Fixed WMS RPC Comments and indentation
                   Added Function Comment Headers; Updated WMS API Version
10/05/06   PMD     Added new cmd_err -  WMS_CMD_ERR_DC_BAD_PARM
10/04/06   NP      Changed some HTORPC Meta Comments to be INOUT
                   for translation utilities provided by WMS service
09/18/06   PMD     Fixed HTORPC Meta Comments and Functionality
08/22/06   PMD     Modified incorrect Enums for wms_pid_e_type
08/17/06   PMD     Remoted wms_client_reg_dbg_cb()
                   Remoted wms_cfg_locate_mo_msg_for_sts_report()
                   Remoted wms_cfg_get_sts_rpt_index()
08/17/06   PMD     Remoted wms_cfg_get/set_link_control() APIs
                   Updated WMS Version to 4.7.1
08/15/06   PMD     Added Documentation for Status Report Storage Behavior.
08/14/06   PMD     Mainlined FEATURE_CDSMS_UDH_EMS in code
08/14/06   PMD     Mainlined FEATURE_CDSMS_UDH in code
08/14/06   PMD     Mainlined FEATURE_GWSMS_UDH_EMS in code
08/14/06   PMD     Mainlined FEATURE_GWSMS_UDH in code
08/14/06   PMD     Mainlined FEATURE_SMS_UDH_EMS in code
08/14/06   PMD     Mainlined FEATURE_GWSMS_UDH_WAP in code.
08/14/06   PMD     Mainlined FEATURE_CDSMS_UDH_WAP in code.
07/03/06   NP      Set WMS_DELIVERY_STATUS_MESSAGE_EXPIRED 
                   value to be 13 as per TIA/EIA-637B, page 4-55.
06/14/06   PMD     Added Status - WMS_RPT_MO_RETRY_PERIOD_EXPIRED.
05/03/06   PMD     Added support for SMS CCAT Feature
05/02/06   HQ      Updated API version to 0x0460.
05/02/06   HQ      Added WMS_CLIENT_ERR_TASK_NOT_READY.
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/25/06   HQ      Put parentheses for parameters of macros to prevent wrong
                   precedences of operations.
04/20/06   PMD     Removed Pending Queue Internal Commands
03/24/06   PMD     Modified Meta Comments for Compilation purposes
                   Added event in bc_mm_event_info structure
01/25/06   VK      Updated Copyright Info
01/24/06   VK      Lint Fixes
01/20/06   PMD     Added Support for 500 SMS Messages under Feature Flag
12/20/05   PMD     Cleaned up FEATURE_SIRIUS from Code Base.
12/20/05   PMD     Added Support for CS Only and PS Only GW Domain Preference.
11/21/05   VK      Added a few more meta comments & moved feature define.  
11/21/05   VK      Added Meta comments.
11/18/05   HQ      Added client type for PBM; updated API version to 0x0412.
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
                   Updated WMS version Number
10/20/05   VK      Lint error/warning fixes
10/19/05   VK      Added Featurization fixes for low tier phones
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Added Get / Set Retry Interval APIs
                   Updated Debug Event Information
                   Updated WMS version Number
10/06/05   PMD     Increased WMS_MAX_UD_HEADERS to 15 under F_SIRIUS
09/06/05   PMD     Added wms_msg_get_retry_period API
                   Updated WMS API Version
08/29/05   PMD     Changed len to uint32 in message_list_s_type
                   Updated WMS API Version
08/05/05   Rex     Code cleanup
07/29/05   Rex     Removed featurization, added names for typedef struct
07/28/05   HQ      Added typedef struct wms_ack_info_s.
07/11/05   PMD     Added raw_dcs_data byte in wms_gw_dcs_s_type
06/30/05   VK      Added Meta-comments support for Linux
06/15/05   Rex     Added wms_gw_ack_info_s define for Linux
03/31/05   PMD     Added Cell Change Type Event Information.
03/10/05   PMD     Added support for wms_bc_mm_msg_delete_all_indication.
                   Updated WMS API version to 0x0402.
02/25/05   HQ      Updated module description and license information.
02/23/05   VK      Added an extra octet - text color - udh_text_formatting
02/18/05   Rex     Change the max size for udh_other type
02/03/05   HQ      Updated API version info to 0x0400.
02/01/05   PMD     Added wms_bc_mm_set/get_reading_preferences API; Updated Version
01/17/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
01/15/05   VK      Added support for 36 digit string
01/07/05   PMD     Added Debug Event Information for GSM. Updated Version
01/07/05   PMD     Changed wms_ts_get_udh_length function prototype.
11/03/04   PMD     Updated WMS_API_VERSION to 0x302
11/03/04   PMD     Added wms_cfg_check_wap_push_message function
10/06/04   PMD     Clarified the Width Parameter in var pic udh structure.
10/06/04   PMD     Changed Var Picture Max Size. Updated WMS Version.
09/21/04   Rex     Added support for CPHS
09/01/04   HQ      Update for UDH User Prompt and Extended Object;
                   updated API version to 0x0300.
08/30/04   Rex     Fix struct to enum type and add comments for link control
08/25/04   Rex     Added support for SMS link control
08/03/04   Rex     Added new msg cmd err for no sc address
07/15/04   PMD     Added new Message Event for Duplicate CB Page
                   Added new API and Cmd for CB Page Deletion Indication
06/30/04   PMD     Made Java as the First Client Type for Correct Parsing
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
05/26/04   HQ      Added WMS_CLIENT_TYPE_WINCE.
05/03/04   Rex     Added new enum for checking gw address max
04/01/04   HQ      Added definitions of EMS 4.0, vCard and vCalendar.
03/31/04   HQ      International address clarifications;
                   SMS API version update.
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
03/19/04   HQ      Added comments about primary client APIs which are still
                   under development.
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
                   Updated API Version - Added new APIs in WMS
01/29/04   HQ      Changed WMS_NUMBER_RESERVED_5 to WMS_NUMBER_ALPHANUMERIC.
01/27/04   HQ      Updated API version to 0x0204.
01/26/04   PMD     Added function wms_ts_get_udh_length()
01/23/04   PMD     Exposed APIs: wms_ts_encode_dcs and wms_ts_decode_dcs
01/16/04   AP/PMD  Added GW and CDMA Retry
01/13/04   HQ      Added utility wms_cfg_check_cdma_duplicate().
01/12/04   HQ      Removed wms_cm_start_full_service().
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
11/06/03   AP      Changed value of WMS_AUTO_USER_DATA_MAX to 160, added
                   comments regarding auto_message_s, use wms_client_id_type
                   instead of wms_client_type_e_type for auto API's.
11/06/03   PMD     Added new command error WMS_CMD_ERR_CFG_DOMAIN_PREF
10/31/03   HQ      Updated API version to 0x0203.
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
10/28/03   HQ      Added rp_cause in gw_ack_info_s_type.
10/23/03   PMD     Added Macro WMS_IS_VALID_TAG
10/23/03   HQ      Added more comments;
                   added client type JAVA;
                   removed tid from wms_message_status_info_s_type;
                   use wms_rp_cause_e_type for cause_value in ack_info;
                   moved some functions to their corresponding groups.
10/22/03   HQ      Updated WMS API version to 0x0202.
10/03/03   AP      Added check for voicemail contents.
10/06/03   HQ      Added WMS API version number #define.
09/22/03   PMD     Added Command and Prototype to Clear SMMA Flag
09/02/03   HQ      Added support for WMS_CFG_EVENT_CELL_CHANGE.
08/28/03   AP      Added support for new msg event ACK_REPORT.
08/20/03   HQ      Added Read Ack in message_type and in reply_option.
08/20/03   PMD     Added more comments for wms_msg_event_info_u
07/29/03   HQ      Grouped enums into different value groups.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/08/03   HQ      Fixed WMS_IS_MO macro for checking MO_SENT.
06/27/03   PMD     Added New Command for GSDI Refresh (File Change Notification)
                   Added New Error code for BUSY (while in Refresh)
06/08/03   PMD     Support for MO SMS Control
06/05/03   PMD     Make alpha_id.data dynamically allocated
05/30/03   SUN     Added new command to set msg ref num
05/28/03   AP      Added client_id to mt_message_info struct.
05/28/03   HQ      Added comments to GW ack info structure.
05/22/03   HQ      Make scpt_data and scpt_result to be pointers;
                   changed WMS_BC_SCPT_MAX_SERVICES from 32 to 5.
05/12/03   HQ      Removed WMS_BC_EVENT_SRV_ADDED_SCPT and
                   WMS_BC_EVENT_SRV_DELETED_SCPT.
04/23/03   AP/HQ   Fixed compile problem with WMS_RSP_....
04/17/03   AP      Added support for FEATURE_UIM_TOOLKIT_UTK && FEATURE_GSTK.
04/16/03   HQ      Added support of FEATURE_CDSMS_IS637B:
                     - Deposit Index in Deliver and User Ack Messages
                     - Delivery Status in Delivery Ack Message
04/02/03   HQ      Removed many feature flags for RPC support;
                   added names to unions;
                   consolidated alert_mode and alert_on_delivery;
                   added error status and cmd err UNSUPPORTED.
03/14/03   HQ      Removed wms_status_report_info_s_type.
02/27/03   HQ      Added support of FEATURE_GWSMS_MO_CONTROL.
02/21/03   SUN     Added new functions for Status Reports and CatApp
02/14/03   HQ      Fixed some typos.
02/14/03   AP      Added support for JCDMA and client parsing.
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/07/03   SUN     Added Support for Status Reports and Commands
01/29/03   HQ      Added support for MO SMS PS/CS domain preferences.
01/24/03   AP      Re-arranged client_type_e_type order.  Added client_id for
                   submit report structure.
01/22/03   HQ      Corrected the SCPT Delete operation codes.
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
11/13/02   AP      Mainlined message list support.
11/09/02   HQ      Added new BC SMS commands: Delete All Services and
                   Set Priority For All.
11/05/02   HQ      Added macro WMS_IS_MO_CDMA_MSG_TYPE and WMS_IS_MO_TPDU_TYPE.
11/01/02   HQ      Added WAP/MMS client.
10/30/02   HQ      Fixed compile error when EMS is not defined.
10/30/02   SUN     Added new enum for MSG waiting DCS,
                   Handled Non Supported UDH
10/23/02   HQ      Added definitions for FEATURE_GWSMS_UDH_WAP and
                   FEATURE_CDSMS_UDH_WAP for enabling MMS/WAP Browser Push.
10/11/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
10/06/02   HQ      Added new clients: WMS_APP and CAT.
09/24/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE and
                   FEATURE_GWSMS_MSG_INFO_CACHE.
09/09/02   SUN     Added Support for Concatenation
08/09/02   HQ      Added support of FEATURE_CDSMS_637A:
                     - Alert mode in Bearer Data
                     - Email address support
                     - Unicode user data
                     - Display mode
08/08/02   HQ      Removed feature flags for wms_message_class_e_type.
08/03/02   HQ      Added more client types: BREW, GPS, GSM1x;
                   added route in wms_mt_message_info_s_type.
07/31/02   HQ      Added support of the following:
                     - 'udh_present' in CDMA message_id structure.
                     - FEATURE_GWSMS_UDH_EMS
                     - FEATURE_CDSMS_UDH_EMS
                     - FEATURE_GWSMS_UDH
                     - FEATURE_CDSMS_UDH
07/31/02   SUN     Added WMS_CMD_ERR_RAM_FULL,WMS_CMD_ERR_SIM_FULL,WMS_CMD_ERR_NV_FULL
07/24/02   SUN     Fixed Large Animation, Small Animation Dimensions
07/19/02   HQ      Added API definitions for EMS and GSM1x:
                   FEATURE_SMS_UDH_EMS and FEATURE_GSM1x.
07/10/02   HQ      Added wms_cm_start_full_service() for 1x.
06/22/02   HQ      Added more CDMA template masks.
06/22/02   HQ      Added command wms_msg_delete_all() and
                   wms_msg_delete_template_all().
06/15/02   HQ      Added dtmf and ascii conversion functions.
06/10/02   HQ      Added WMS_MSG_EVENT_MT_MESSAGE_ERROR.
06/03/02   HQ      Added 1x Broadcast SMS support and ASCII packing/unpacking.
05/09/02   SUN     Added WMS_TPDU_NONE to fix tpdu length issue
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode;
                   Feature-ized CLASS_CDMA under FEATURE_CDSMS.
05/06/02   SUN     Added support for User Data Headers
04/16/02   HQ      Completed definition for wms_rp_cause_e_type.
03/07/02   HQ      Added Multimode support - added FEATURE_CDSMS.
03/09/02   HQ      Code clean-ups.
03/06/02   HQ      Fixed compiler warnings.
02/08/02   SUN     Added new error WMS_CMD_ERR_MSG_TPDU_TYPE
02/07/02   HQ      Added CFG_EVENT_READY and CFG_EVENT_MEMORY_FULL.
01/25/02   HQ      Fixed a compile warning. Removed CFG_EVENT_WRITE_ERROR.
01/23/02   HQ      Changed validity format enums.
01/21/02   SUN     Added Error Notification event
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/28/01   SUN     Added new error type for validity
11/27/01   HQ      Added report status to submit report information.
06/29/01   HQ      Initial version for WCDMA & GSM SMS.

===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#ifdef CUST_EDITION
#ifdef WIN32
#include "custwms.h"
#endif
#endif /*CUST_EDITION*/

#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

/* <EJECT> */
/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/* ========================================================================= */
/* ========================== Common data types ============================ */
/* ========================================================================= */
#define WMS_MO_ONLY_TRAFFIC     0
#define WMS_MO_ALL_CHANNEL      1


typedef enum _wms_box_e_type
{
    WMS_MB_NONE,         // 无邮箱
    WMS_MB_VOICEMAIL,    // 语音箱
    WMS_MB_INBOX,        // 收件箱
    WMS_MB_OUTBOX,       // 发件箱
    WMS_MB_DRAFT,        // 草稿箱
    WMS_MB_TEMPLATE,     // 消息模板
    WMS_MB_RESERVE,      // 预约
    WMS_MB_RSVFAILED,    // 预约失败
#ifdef FEATURE_USES_MMS
    WMS_MB_OUTBOX_MMS,   // 彩信发件箱
    WMS_MB_INBOX_MMS,
    WMS_MB_DRAFTBOX_MMS,
#endif
    WMS_MB_MAX
} wms_box_e_type;

typedef enum
{
    // 用于收件箱
    WMS_INBOX_DEL_ALL       = 0x00,
    WMS_INBOX_DEL_RUIM,
    WMS_INBOX_DEL_PHONE,
    
    // 用于发件箱
    WMS_OUTBOX_DEL_ALL,
    
    // 用于草稿箱
    WMS_DRAFT_DEL_ALL,
    
    // 用于预约短信
    WMS_RESERVE_DEL_ALL,
    
    // 用于预约失败短信
    WMS_RSVFAILED_DEL_ALL,

    //用于删除预约以及预约失败短信
    WMS_RSVANDRSVFAILED_DEL_ALL,

    //删除常用短信息
    WMS_TEMPLATE_DEL_ALL,
    
    // 删除全部
    WMS_BOXES_DEL_ALL,

    
    
    WMS_BOX_MESSAGE_MAX
} wms_deletebox_e_type;
#endif /*CUST_EDITION*/
/* <EJECT> */
/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/


/* ========================================================================= */
/* ========================== Common data types ============================ */
/* ========================================================================= */


#define WMS_API_VERSION           0x0500


enum { WMS_MAX_LEN                = 255 };

enum { WMS_ADDRESS_MAX            = 36  };
enum { WMS_SUBADDRESS_MAX         = 36  };
enum { WMS_GW_ADDRESS_MAX         = 20  };
enum { WMS_CDMA_USER_DATA_MAX     = 229 };
#ifdef CUST_EDITION
enum { WMS_AUTO_USER_DATA_MAX     = 160 };
#endif /*CUST_EDITION*/
enum { WMS_GW_USER_DATA_MAX     = 160 };
#ifdef CUST_EDITION
#ifdef FEATURE_SMS_500_MESSAGES
#error code not present
#else

// 255->500 值需大于 WMS_MAX_NV_CDMA_MESSAGES 
enum { WMS_MESSAGE_LIST_MAX     = 500 };
#endif /* FEATURE_SMS_500_MESSAGES */
#endif /*CUST_EDITION*/

enum { WMS_MAX_TEMPLATES        = 10 };
enum { WMS_GW_COMMAND_DATA_MAX  = 157 };                                                   
// enum { WMS_MAX_LONG_LEN         = 39015 /* 255*153 */ };
enum { WMS_ALPHA_ID_MAX         = 255 };

/* For Extended/Long SMS support, FEATURE_SMS_UDH will be enabled too.
*/
#ifdef CUST_EDITION
#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
enum { WMS_MAX_UD_HEADERS       = 7};

/* UDH MAX SIZES */
enum { WMS_UDH_MAX_8BIT              = 134};
enum { WMS_UDH_MAX_7BIT              = 153};
enum { WMS_UDH_OTHER_SIZE        = 226 };
/* 229 (CDMA MAX USER DATA LEN) - 1(UDHL) - 1(UDH ID) - 1(UDH Len) */


/* Sound definitions
*/
enum { WMS_UDH_MAX_SND_SIZE          = 128 };

/* Picture definitions
*/
enum { WMS_UDH_LARGE_PIC_SIZE    = 128 };
enum { WMS_UDH_SMALL_PIC_SIZE    = 32  };
enum { WMS_UDH_VAR_PIC_SIZE      = 134 };  /* 140 - 1(UDHL) - 5(UDH) */

/* Animation definitions
*/
enum { WMS_UDH_ANIM_NUM_BITMAPS  = 4 };
enum { WMS_UDH_LARGE_BITMAP_SIZE = 32 };
enum { WMS_UDH_SMALL_BITMAP_SIZE = 8 };

#else /* FEATURE_SMS_UDH && ! FEATURE_SMS_EXTENDED */

/* decreasing the EMS header size and the all the rest because this reduces the structure
   of the user_data_s_type and hence the RAM memory decreases when EMS is not required. 
*/
enum { WMS_MAX_UD_HEADERS       = 1};

/* UDH MAX SIZES */
#ifdef FEATURE_SMS_EXTENDED
enum { WMS_UDH_MAX_8BIT              = 4};
enum { WMS_UDH_MAX_7BIT              = 5};
#endif /* FEATURE_SMS_EXTENDED */

enum { WMS_UDH_OTHER_SIZE        = 1 };
/* 229 (CDMA MAX USER DATA LEN) - 1(UDHL) - 1(UDH ID) - 1(UDH Len) */


/* Sound definitions
*/
enum { WMS_UDH_MAX_SND_SIZE          = 1 };

/* Picture definitions
*/
enum { WMS_UDH_LARGE_PIC_SIZE    = 1 };
enum { WMS_UDH_SMALL_PIC_SIZE    = 1  };
enum { WMS_UDH_VAR_PIC_SIZE      = 1 };  /* 140 - 1(UDHL) - 5(UDH) */

/* Animation definitions
*/
enum { WMS_UDH_ANIM_NUM_BITMAPS  = 1 };
enum { WMS_UDH_LARGE_BITMAP_SIZE = 1 };
enum { WMS_UDH_SMALL_BITMAP_SIZE = 1 };
//miaoxiaoming

#endif /* FEATURE_SMS_UDH */
#endif /*CUST_EDITION*/
/* -------------------- */
/* ---- Digit mode ---- */
/* -------------------- */
typedef enum
{
  WMS_DIGIT_MODE_4_BIT     = 0,  /* DTMF digits */
  WMS_DIGIT_MODE_8_BIT     = 1,
  WMS_DIGIT_MODE_MAX32     = 0x10000000

} wms_digit_mode_e_type;


/* --------------------- */
/* ---- Number type ---- */
/* --------------------- */
typedef enum
{
  WMS_NUMBER_UNKNOWN        = 0,

  WMS_NUMBER_INTERNATIONAL  = 1,
  WMS_NUMBER_NATIONAL       = 2,
  WMS_NUMBER_NETWORK        = 3,
  WMS_NUMBER_SUBSCRIBER     = 4,
  WMS_NUMBER_ALPHANUMERIC   = 5, /* GSM SMS: addr value is GSM 7-bit chars */
  WMS_NUMBER_ABBREVIATED    = 6,
  WMS_NUMBER_RESERVED_7     = 7,

  /* The above are used when number mode is not data network address.
     The following are used only when number mode is data network address
     mode.
  */
  WMS_NUMBER_DATA_IP        = 1

  , WMS_NUMBER_INTERNET_EMAIL = 2
    /* In the address data "digits", each byte contains an ASCII character.
    ** Examples are "x@y.com,a@b.com" - ref TIA/EIA-637A 3.4.3.3.
    */

  , WMS_NUMBER_MAX32        = 0x10000000

} wms_number_type_e_type;


/* --------------------- */
/* ---- Number plan ---- */
/* --------------------- */
typedef enum
{
  WMS_NUMBER_PLAN_UNKNOWN     = 0,
  WMS_NUMBER_PLAN_TELEPHONY   = 1,  /* CCITT E.164 and E.163,
                                       including ISDN plan */
  WMS_NUMBER_PLAN_RESERVED_2  = 2,
  WMS_NUMBER_PLAN_DATA        = 3,  /* CCITT X.121 */
  WMS_NUMBER_PLAN_TELEX       = 4,  /* CCITT F.69 */
  WMS_NUMBER_PLAN_RESERVED_5  = 5,
  WMS_NUMBER_PLAN_RESERVED_6  = 6,
  WMS_NUMBER_PLAN_RESERVED_7  = 7,
  WMS_NUMBER_PLAN_RESERVED_8  = 8,
  WMS_NUMBER_PLAN_PRIVATE     = 9,
  WMS_NUMBER_PLAN_RESERVED_10 = 10,
  WMS_NUMBER_PLAN_RESERVED_11 = 11,
  WMS_NUMBER_PLAN_RESERVED_12 = 12,
  WMS_NUMBER_PLAN_RESERVED_13 = 13,
  WMS_NUMBER_PLAN_RESERVED_14 = 14,
  WMS_NUMBER_PLAN_RESERVED_15 = 15,
  WMS_NUMBER_PLAN_MAX32       = 0x10000000

} wms_number_plan_e_type;


/* --------------------- */
/* ---- Number mode ---- */
/* --------------------- */
typedef enum
{
  WMS_NUMBER_MODE_NONE_DATA_NETWORK      = 0,
  WMS_NUMBER_MODE_DATA_NETWORK           = 1,
  WMS_NUMBER_MODE_DATA_NETWORK_MAX32     = 0x10000000

} wms_number_mode_e_type;


/* ----------------- */
/* ---- Address ---- */
/* ----------------- */
typedef struct wms_address_s
{
  wms_digit_mode_e_type          digit_mode;
    /* Indicates 4-bit or 8-bit
    */

  wms_number_mode_e_type         number_mode;
    /* Used in CDMA only: only meaningful when digit_mode is 8-bit
    */

  wms_number_type_e_type         number_type;
    /* In case of CDMA address, this is used only when digit_mode is 8-bit.
    ** To specify an international address for CDMA, use the following:
    **   digit_mode  = 8-bit
    **   number_mode = NONE_DATA_NETWORK
    **   number_type = INTERNATIONAL
    **   number_plan = TELEPHONY
    **   number_of_digits = number of digits
    **   digits = ASCII digits, e.g. '1', '2', '3', '4' and '5'.
    */

  wms_number_plan_e_type         number_plan;
    /* In case of CDMA address, this is used only when digit_mode is 8-bit.
    */

  uint8                          number_of_digits;

  uint8                          digits[ WMS_ADDRESS_MAX ];
    /* Each byte in this array represents a 4-bit or 8-bit digit of
    ** address data.
    */

} wms_address_s_type;


/* ------------------------- */
/* ---- Subaddress type ---- */
/* ------------------------- */
typedef enum
{
  WMS_SUBADDRESS_NSAP           = 0,  /* CCITT X.213 or ISO 8348 AD2 */
  WMS_SUBADDRESS_USER_SPECIFIED = 1,  /* e.g. X.25 */
  WMS_SUBADDRESS_MAX32          = 0x10000000

} wms_subaddress_type_e_type;


/* -------------------- */
/* ---- Subaddress ---- */
/* -------------------- */
typedef struct wms_subaddress_s
{
  wms_subaddress_type_e_type           type;
  boolean                              odd;   /* TRUE means the last byte's
                                                 lower 4 bits should be
                                                 ignored */
  uint8                                number_of_digits;

  /* Each byte in the following array represents a 8-bit digit of
     subaddress data:
  */
  uint8                                digits[ WMS_SUBADDRESS_MAX ];

} wms_subaddress_s_type;



/* --------------------- */
/* -- Message Tag    --- */
/* --------------------- */
/* Message tag:
** The message status in the SIM card
*/
typedef enum
{
  /* The values assigned here match those in the SIM/RUIM card
  */
  WMS_TAG_NONE        = 0x00,
  WMS_TAG_MT_READ     = 0x01,
  WMS_TAG_MT_NOT_READ = 0x03,
  WMS_TAG_MO_SENT     = 0x05, /* No status report requested */
  WMS_TAG_MO_NOT_SENT = 0x07,

  /* GSM/WCDMA only */
  WMS_TAG_MO_SENT_ST_NOT_RECEIVED = 0x0D,
  WMS_TAG_MO_SENT_ST_NOT_STORED   = 0x15,
  WMS_TAG_MO_SENT_ST_STORED       = 0x35,

  // 在本平台许多地方将 wms_message_tag_e_type 大小视为一个字节，下面的
  // 定义不是一个字节所能表示的
  //WMS_TAG_MO_TEMPLATE = 0x100, /* SMS parameters */
  //WMS_TAG_STATUS_RPT  = 0x200  /* SMSR parameters*/
  WMS_TAG_MO_TEMPLATE = 0x10, /* SMS parameters */
  WMS_TAG_STATUS_RPT  = 0x20,  /* SMSR parameters*/
  
  // 添加一个标记用于草稿箱
#ifdef CUST_EDITION
  WMS_TAG_MO_DRAFT  = 0x30,     /* 草稿 */
  WMS_TAG_PHRASE    = 0x31,     /* 常用语 */
  WMS_TAG_RESERVE   = 0x32,     /* 预约 */
  WMS_TAG_RSVFAILED = 0x33      /* 预约失败 */
#endif /*CUST_EDITION*/
} wms_message_tag_e_type;


#define WMS_IS_MO( tag )           \
  ( ( (tag) & 0x7 ) == WMS_TAG_MO_SENT ||  \
    (tag) == WMS_TAG_MO_NOT_SENT ||  \
    (tag) == WMS_TAG_MO_TEMPLATE )

#define WMS_IS_MT( tag )           \
  ( (tag) == WMS_TAG_MT_READ ||      \
    (tag) == WMS_TAG_MT_NOT_READ )

#define WMS_IS_VALID_TAG( tag )    \
  ( (tag) == WMS_TAG_NONE ||         \
    (tag) == WMS_TAG_MT_READ ||      \
    (tag) == WMS_TAG_MT_NOT_READ ||  \
    (tag) == WMS_TAG_MO_SENT ||      \
    (tag) == WMS_TAG_MO_NOT_SENT ||  \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_RECEIVED || \
    (tag) == WMS_TAG_MO_SENT_ST_NOT_STORED ||   \
    (tag) == WMS_TAG_MO_SENT_ST_STORED ||       \
    (tag) == WMS_TAG_MO_TEMPLATE ||  \
    (tag) == WMS_TAG_STATUS_RPT )



/* ------------------- */
/* ---- Timestamp ---- */
/* ------------------- */
typedef struct
{
  /* If 'year' is between 96 and 99, the actual year is 1900 + 'year';
     if 'year' is between 00 and 95, the actual year is 2000 + 'year'.
     NOTE: Each field has two BCD digits and byte arrangement is <MSB, ... ,LSB>
  */
  uint8      year;        /* 0x00-0x99 */
  uint8      month;       /* 0x01-0x12 */
  uint8      day;         /* 0x01-0x31 */
  uint8      hour;        /* 0x00-0x23 */
  uint8      minute;      /* 0x00-0x59 */
  uint8      second;      /* 0x00-0x59 */
  sint7      timezone;    /* +/-, [-48,+48] number of 15 minutes - GW only */
} wms_timestamp_s_type;


/* ------------------ */
/* ---- Language ---- */
/* ------------------ */
typedef enum
{
  WMS_LANGUAGE_UNSPECIFIED = 0,
  WMS_LANGUAGE_ENGLISH,
  WMS_LANGUAGE_FRENCH,
  WMS_LANGUAGE_SPANISH,
  WMS_LANGUAGE_JAPANESE,
  WMS_LANGUAGE_KOREAN,
  WMS_LANGUAGE_CHINESE,
  WMS_LANGUAGE_HEBREW,
  WMS_LANGUAGE_MAX32       = 0x10000000

} wms_language_e_type;


/* Message mode:
** determines which system mode is used for a message
*/
typedef enum
{
  WMS_MESSAGE_MODE_CDMA = 0,
  WMS_MESSAGE_MODE_GW,
#ifdef CUST_EDITION  
  WMS_MESSAGE_MODE_AUTO,
#endif /*CUST_EDITION*/  
  WMS_MESSAGE_MODE_MAX,
  WMS_MESSAGE_MODE_MAX32 = 0x100000
} wms_message_mode_e_type;


/* TS data formats
*/
typedef enum
{
  WMS_FORMAT_CDMA         = 0,  /* IS-95 */
  WMS_FORMAT_ANALOG_CLI,        /* IS-91 */
  WMS_FORMAT_ANALOG_VOICE_MAIL, /* IS-91 */
  WMS_FORMAT_ANALOG_SMS,        /* IS-91 */
  WMS_FORMAT_ANALOG_AWISMS,     /* IS-95 Alert With Information SMS */
  WMS_FORMAT_MWI,               /* Message Waiting Indication as voice mail */
  WMS_FORMAT_GW_PP,             /* GW Point-to-Point SMS */
  WMS_FORMAT_GW_CB,             /* GW CB SMS */
  WMS_FORMAT_MAX,
  WMS_FORMAT_MAX32        = 0x10000000

} wms_format_e_type;



/* Send mode:
** indicates the specific requirements for sending a message
*/
typedef enum
{
  WMS_SEND_MODE_CLIENT_MESSAGE = 0,
  WMS_SEND_MODE_MEMORY_STORE,
  WMS_SEND_MODE_MAX,
  WMS_SEND_MODE_MAX32 = 0x10000000  /* pad to 32 bit int */
} wms_send_mode_e_type;


/* Write mode:
** indicates the specific requirements for storing a message
*/
typedef enum
{
  WMS_WRITE_MODE_INSERT = 0,
  WMS_WRITE_MODE_REPLACE,
  WMS_WRITE_MODE_MAX,
  WMS_WRITE_MODE_MAX32  = 0x10000000
} wms_write_mode_e_type;


/* Message class:
** The message class in the Data Coding Scheme of a TPDU or CB message
*/
typedef enum
{
  /* the following are GW message classes
  */
  WMS_MESSAGE_CLASS_0 = 0,
  WMS_MESSAGE_CLASS_1,
  WMS_MESSAGE_CLASS_2,
  WMS_MESSAGE_CLASS_3,
  WMS_MESSAGE_CLASS_NONE,

  /* The following class is for CDMA only
  */
  WMS_MESSAGE_CLASS_CDMA,

  WMS_MESSAGE_CLASS_MAX,
  WMS_MESSAGE_CLASS_MAX32 = 0x10000000
} wms_message_class_e_type;


/* Message routing types
*/
typedef enum
{
  WMS_ROUTE_DISCARD = 0,      /* Discard message */
  WMS_ROUTE_STORE_AND_NOTIFY, /* Store message and notify clients */
  WMS_ROUTE_TRANSFER_ONLY,    /* Don't store msg but send it to clients */
  WMS_ROUTE_TRANSFER_AND_ACK, /* Send the message to clients and ack it */
  WMS_ROUTE_NO_CHANGE,        /* Don't change the current setting */
  WMS_ROUTE_INTERNAL_PROC,    /* INTERANL USE ONLY */
  WMS_ROUTE_MAX,
  WMS_ROUTE_MAX32   = 0x10000000
} wms_route_e_type;

typedef enum
{
    MMS_OUTBOX,
    MMS_INBOX,
    MMS_DRAFTBOX
}WMS_MMS_SAVETYPE;


/* Memory stores:
** Different types of memory stores supported
*/
typedef enum
{
  WMS_MEMORY_STORE_NONE = 0,

  /* GSM/WCDMA memory stores
  */
  WMS_MEMORY_STORE_RAM_GW,
  WMS_MEMORY_STORE_SIM,
  WMS_MEMORY_STORE_NV_GW,

  /* CDMA memory stores
  */
  WMS_MEMORY_STORE_RAM_CDMA,  /* NOT SUPPORTED */
  WMS_MEMORY_STORE_RUIM,
  WMS_MEMORY_STORE_NV_CDMA,

  WMS_MEMORY_STORE_MAX,
  WMS_MEMORY_STORE_MAX32  = 0x10000000
} wms_memory_store_e_type;


typedef enum
{
  WMS_GW_DOMAIN_PREF_CS_PREFERRED  = 0,
  WMS_GW_DOMAIN_PREF_PS_PREFERRED,
  WMS_GW_DOMAIN_PREF_CS_ONLY, 
  WMS_GW_DOMAIN_PREF_PS_ONLY, 
  WMS_GW_DOMAIN_PREF_MAX,
  WMS_GW_DOMAIN_PREF_MAX32  = 0x10000000
} wms_gw_domain_pref_e_type;

typedef struct
{
  boolean      set_primary;
  boolean      use_client_memory;
}wms_cfg_set_primary_client_s_type;

typedef enum
{
  /* mode preferences [0,1,2] for link control */
  WMS_LINK_CONTROL_DISABLED = 0,
  WMS_LINK_CONTROL_ENABLED_ONE,
  WMS_LINK_CONTROL_ENABLE_ALL,
  WMS_LINK_CONTROL_MAX,
  WMS_LINK_CONTROL_MAX32 = 0x10000000

}wms_cfg_link_control_mode_e_type;

typedef enum
{
  /* status of the link control */
  WMS_LINK_CONTROL_INACTIVE = 0,
  WMS_LINK_CONTROL_ACTIVE,
  WMS_LINK_CONTROL_TIMEOUT,
  WMS_LINK_CONTROL_CONNECTION_RELEASED
} wms_cfg_link_control_status_e_type;

typedef struct wms_cfg_link_control_s
{
  wms_cfg_link_control_mode_e_type  link_control_mode;
  wms_cfg_link_control_status_e_type    link_control_status;
} wms_cfg_link_control_s_type;



/* Message number:
** In CDMA mode, it is the Message ID of the Bearer Data;
** In GW mode, it is the Message Reference number in the TPDU.
*/
typedef uint32  wms_message_number_type;


/* Message index:
** The message index in a message list
*/
typedef uint32  wms_message_index_type;

#define WMS_DUMMY_MESSAGE_INDEX  0xFFFFFFFF
#ifdef CUST_EDITION
#define WMS_VM_MESSAGE_INDEX     0x00000000
#endif /*CUST_EDITION*/

/* Transaction ID
*/
typedef uint32  wms_transaction_id_type;

#define WMS_DUMMY_TRANSACTION_ID  0xFFFFFFFF


/* WMS Command IDs
*/
typedef enum
{
  /* Configuration group commands
  */
  WMS_CMD_CFG_SET_ROUTES = 0x0000,
  WMS_CMD_CFG_GET_ROUTES,
  WMS_CMD_CFG_GET_MEMORY_STATUS,
  WMS_CMD_CFG_GET_MESSAGE_LIST,
  WMS_CMD_CFG_SET_GW_DOMAIN_PREF,
  WMS_CMD_CFG_GET_GW_DOMAIN_PREF,
  WMS_CMD_CFG_SET_PRIMARY_CLIENT,
  WMS_CMD_CFG_SET_MEMORY_FULL,
  WMS_CMD_CFG_SET_LINK_CONTROL,
  WMS_CMD_CFG_GET_LINK_CONTROL,
#ifdef CUST_EDITION  
  WMS_CMD_CFG_SET_MULTISEND,
#ifdef FEATURE_AUTOREPLACE
  WMS_CMD_CFG_SET_AUTOREPLACE,
#endif  
#endif /*CUST_EDITION*/

  /* Message group commands
  */
  WMS_CMD_MSG_SEND       = 100,
  WMS_CMD_MSG_ACK,
  WMS_CMD_MSG_READ,
  WMS_CMD_MSG_WRITE,
  WMS_CMD_MSG_DELETE,
#ifdef CUST_EDITION  
  WMS_CMD_MSG_DELETE_BOX,
  WMS_CMD_MSG_COPY,
  WMS_CMD_REFRESH_IND,
#endif /*CUST_EDITION*/  
  WMS_CMD_MSG_DELETE_ALL,
  WMS_CMD_MSG_MODIFY_TAG,
  WMS_CMD_MSG_READ_TEMPLATE,
  WMS_CMD_MSG_WRITE_TEMPLATE,
  WMS_CMD_MSG_DELETE_TEMPLATE,
  WMS_CMD_MSG_DELETE_TEMPLATE_ALL,

  WMS_CMD_MSG_READ_STS_REPORT,
  WMS_CMD_MSG_WRITE_STS_REPORT,
  WMS_CMD_MSG_DELETE_STS_REPORT,
  WMS_CMD_MSG_DELETE_STS_REPORT_ALL,

  /* DC commands
  */
  WMS_CMD_DC_CONNECT     = 200,
  WMS_CMD_DC_DISCONNECT,
  WMS_CMD_DC_ENABLE_AUTO_DISCONNECT,
  WMS_CMD_DC_DISABLE_AUTO_DISCONNECT,

#ifdef CUST_EDITION
  /* BC commands
  */
  WMS_CMD_BC_GET_CONFIG    = 300,
  WMS_CMD_BC_GET_PREF,
  WMS_CMD_BC_SET_PREF,
  WMS_CMD_BC_GET_TABLE,
  WMS_CMD_BC_SELECT_SRV,
  WMS_CMD_BC_GET_ALL_SRV_IDS,
  WMS_CMD_BC_GET_SRV_INFO,
  WMS_CMD_BC_ADD_SRV,
  WMS_CMD_BC_DELETE_SRV,
  WMS_CMD_BC_CHANGE_LABEL,
  WMS_CMD_BC_DELETE_ALL_SERVICES,
  WMS_CMD_BC_SET_PRIORITY_FOR_ALL,
#endif /*CUST_EDITION*/
  /* BC_MM commands
  */
  WMS_CMD_BC_MM_GET_CONFIG   = 400,
  WMS_CMD_BC_MM_GET_PREF,
  WMS_CMD_BC_MM_SET_PREF,
  WMS_CMD_BC_MM_GET_TABLE,
  WMS_CMD_BC_MM_SELECT_SRV,
  WMS_CMD_BC_MM_GET_ALL_SRV_IDS,
  WMS_CMD_BC_MM_GET_SRV_INFO,
  WMS_CMD_BC_MM_ADD_SRV,
  WMS_CMD_BC_MM_DELETE_SRV,
  WMS_CMD_BC_MM_CHANGE_SRV_INFO,
  WMS_CMD_BC_MM_DELETE_ALL_SERVICES,
  WMS_CMD_BC_MM_SELECT_ALL_SERVICES,
  WMS_CMD_BC_MM_SET_PRIORITY_ALL_SERVICES,
  WMS_CMD_BC_MM_MSG_DELETE_INDICATION,
  WMS_CMD_BC_MM_MSG_DELETE_ALL_INDICATION,
  WMS_CMD_BC_MM_GET_READING_PREF,
  WMS_CMD_BC_MM_SET_READING_PREF,

  /* DBG commands
  */
  WMS_CMD_DBG_RESET_TL_SEQ_NUM  = 500,
  WMS_CMD_DBG_SET_MSG_REF_NUM,
  WMS_CMD_DBG_CLEAR_SMMA,
  WMS_CMD_DBG_GET_RETRY_INTERVAL,
  WMS_CMD_DBG_SET_RETRY_INTERVAL,

  /* -- NOTE: the following are for internal use only --- */

  /* Internal: events from MC
  */
  WMS_CMD_MSG_MC_MT_MSG_E     = 600,
  WMS_CMD_MSG_MC_MO_STATUS_E,
  WMS_CMD_MSG_MC_STATE_CHANGED_E,
  WMS_CMD_MSG_MC_MWI_E,
  WMS_CMD_MSG_GSTK_EVT,

  /* Internal: events from CM
  */
  WMS_CMD_DC_CALL_INCOM_E,
  WMS_CMD_DC_CALL_CONNECT_E,
  WMS_CMD_DC_CALL_END_E,
  WMS_CMD_DC_CALL_ERROR_E,  /* in case of CM cmd errors */

  WMS_CMD_CM_BC_ENABLE_FAILURE_E,
  WMS_CMD_CM_START_FULL_SERVICE_E,
  WMS_CMD_CM_ONLINE_E,
#ifdef CUST_EDITION  
  WMS_CMD_CM_NAS_SM_DATA,
  WMS_CMD_CM_NAS_SM_REPORT,
  WMS_CMD_CM_NAS_SM_LINK_CONTROL,
  WMS_CMD_CM_NAS_CB_DATA,
  WMS_CMD_CM_NAS_CB_CELL_CHANGE,
  WMS_CMD_CM_NAS_CB_DUPLICATION_DETECTION,
#endif /*CUST_EDITION*/  
  WMS_CMD_CM_LPM_E,

  WMS_CMD_GSDI_ASYNC_CB,
  WMS_CMD_GSDI_CARD_STATUS_CB,
  WMS_CMD_GSDI_REFRESH_FCN_IND,
  WMS_CMD_BC_SCPT_DATA,

  WMS_CMD_SS_CHANGE_INFO,
  WMS_CMD_IPAPP_CHANGE_INFO,

  /* Internal events from DEM
  */
  WMS_CMD_DEM_APPS_POWERDOWN,
  WMS_CMD_DEM_APPS_SUSPEND,
  WMS_CMD_DEM_APPS_POWER_RESTORED,

  WMS_CMD_MAX,
  WMS_CMD_DUMMY = WMS_CMD_MAX,
  WMS_CMD_MAX32 = 0x10000000  /* pad to 32 bit int */
} wms_cmd_id_e_type;


/* Command error:
** the command status reported in the command callback
*/
typedef enum
{
  WMS_CMD_ERR_NONE       = 0,

  WMS_CMD_ERR_CLIENT_ID,
  WMS_CMD_ERR_NO_RESOURCE,
  WMS_CMD_ERR_UNSUPPORTED,
  WMS_CMD_ERR_BUSY,
  WMS_CMD_ERR_NULL_PTR,
  WMS_CMD_ERR_NO_NETWORK,
  WMS_CMD_ERR_BAD_PARM,
  WMS_CMD_ERR_CS_ERROR, /* Component Services Error */

  WMS_CMD_ERR_CFG_NV_WRITE  = 100,
  WMS_CMD_ERR_CFG_NV_READ,
  WMS_CMD_ERR_CFG_ROUTE,
  WMS_CMD_ERR_CFG_MEM_STORE,
  WMS_CMD_ERR_CFG_MSG_CLASS,
  WMS_CMD_ERR_CFG_DOMAIN_PREF,
  WMS_CMD_ERR_CFG_UNPRIVILEGED_CLIENT,
  WMS_CMD_ERR_CFG_LINK_CONTROL,

  WMS_CMD_ERR_MSG_MEMORY_STORE = 200,
  WMS_CMD_ERR_MSG_INDEX,
  WMS_CMD_ERR_MSG_EMPTY_MESSAGE,
  WMS_CMD_ERR_MSG_TAG,
  WMS_CMD_ERR_MSG_ENCODE,
  WMS_CMD_ERR_MSG_DECODE,
  WMS_CMD_ERR_MSG_SEND_MODE,
  WMS_CMD_ERR_MSG_WRITE_MODE,
  WMS_CMD_ERR_MSG_MEMORY_FULL,
  WMS_CMD_ERR_MSG_RAM_FULL,
  WMS_CMD_ERR_MSG_SIM_FULL,
  WMS_CMD_ERR_MSG_NV_FULL,
  WMS_CMD_ERR_MSG_TRANSACTION_ID,
  WMS_CMD_ERR_MSG_SIM_WRITE,
  WMS_CMD_ERR_MSG_SIM_READ,
  WMS_CMD_ERR_MSG_SIM_DELETE,
  WMS_CMD_ERR_MSG_RUIM_WRITE,
  WMS_CMD_ERR_MSG_RUIM_READ,
  WMS_CMD_ERR_MSG_RUIM_DELETE,
  WMS_CMD_ERR_MSG_NV_WRITE,
  WMS_CMD_ERR_MSG_NV_READ,
  WMS_CMD_ERR_MSG_NV_DELETE,
  WMS_CMD_ERR_MSG_TPDU_TYPE,
  WMS_CMD_ERR_MSG_FORMAT,
  WMS_CMD_ERR_MSG_NO_MO_MSG,
  WMS_CMD_ERR_MSG_NO_SC_ADDRESS,
  WMS_CMD_ERR_MSG_LPM,
  WMS_CMD_ERR_MSG_BLOCKED,

  WMS_CMD_ERR_BC_BAD_PREF    = 300,
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE,
  WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL,
  WMS_CMD_ERR_BC_NV_WRITE,
  WMS_CMD_ERR_BC_BAD_SRV_ID,
  WMS_CMD_ERR_BC_TABLE_FULL,
  WMS_CMD_ERR_BC_DUPLICATE_SRV,
  WMS_CMD_ERR_BC_SIM_WRITE,
  WMS_CMD_ERR_BC_SIM_READ,

  WMS_CMD_ERR_DC_BAD_STATE   = 400,
  WMS_CMD_ERR_DC_BAD_CALL_ID, /* Internal use only */
  WMS_CMD_ERR_DC_BAD_PARM,
  WMS_CMD_ERR_DC_LPM,

  WMS_CMD_ERR_DBG_BAD_PARAM  = 500,

  //WMS_CMD_ERR_DBG_,
  WMS_CMD_ERR_MAX,
  WMS_CMD_ERR_MAX32 = 0x10000000    /* pad to 32 bit int */

} wms_cmd_err_e_type;


/* Command callback:
** Callback function for reporting command execution status of the API
*/
typedef void (* wms_cmd_cb_type )
(
  wms_cmd_id_e_type        cmd,
    /* which command's status being reported
    */
  void                     *user_data,
  wms_cmd_err_e_type       cmd_err
    /* the status of the command
    */
);
/*~ CALLBACK wms_cmd_cb_type */
/*~ ONERROR return  */

/* Client request errors
*/
typedef enum
{
  WMS_CLIENT_ERR_NONE,
  WMS_CLIENT_ERR_CLIENT_ID_PTR,     /* Invalid client ID pointer*/
  WMS_CLIENT_ERR_CLIENT_TYPE,       /* Invalid client type */
  WMS_CLIENT_ERR_CLIENT_ID,         /* Invalid client ID */
  WMS_CLIENT_ERR_TASK_NOT_READY,    /* WMS task is not ready for clients */
  WMS_CLIENT_ERR_INVALID_PROCESSOR, /* Invalid processor id */
  WMS_CLIENT_ERR_INVALID_NOTIFY_TYPE, /* Invalid notify type */
  WMS_CLIENT_ERR_UNSUPPORTED,       /* Unsupported request */
  WMS_CLIENT_ERR_GENERAL,           /* Any General Error */
  WMS_CLIENT_ERR_RPC,               /* Any ONCRPC Error */   
  WMS_CLIENT_ERR_MAX,               /* FOR INTERNAL USE OF CM ONLY! */
  WMS_CLIENT_ERR_MAX32 = 0x10000000    /* pad to 32 bit int */
} wms_client_err_e_type;


/* Status:
*/
typedef enum
{
  WMS_OK_S                  = 0,
  WMS_OUT_OF_RESOURCES_S,   /* e.g. out of memory buffer */
  WMS_TERMINAL_BLOCKED_S,
  WMS_TERMINAL_BUSY_S,
  WMS_INVALID_TRANSACTION_ID_S,
  WMS_INVALID_FORMAT_S,
  WMS_GENERAL_ERROR_S,
  WMS_UNSUPPORTED_S,
  WMS_NULL_PTR_S,

  /* CDMA only
  */
  WMS_INVALID_PARM_SIZE_S   = 100,
  WMS_INVALID_USER_DATA_SIZE_S,
  WMS_INVALID_PARM_VALUE_S,
  WMS_MISSING_PARM_S,
  WMS_NETWORK_NOT_READY_S,
  WMS_PHONE_NOT_READY_S,
  WMS_NOT_ALLOWED_IN_AMPS_S,
  WMS_NETWORK_FAILURE_S,
  WMS_ACCESS_TOO_LARGE_S,
  WMS_DTC_TOO_LARGE_S,
  WMS_ACCESS_BLOCK_S,
  WMS_ESN_MISMATCH_S, /* for JCDMA2 feature only */

  /* GSM/WCDMA only
  */
  WMS_INVALID_TPDU_TYPE_S  = 200,
  WMS_INVALID_VALIDITY_FORMAT_S,
  WMS_INVALID_CB_DATA_S,
  WMS_MT_MSG_FAILED_S, /* internal use */

  /* SIP errors
  */
  WMS_SIP_PERM_ERROR_S     = 300,
  WMS_SIP_TEMP_ERROR_S,

  /* WMSC, CS and RPC errors
  */
  WMS_WMSC_ERROR_S         = 400,
  WMS_CS_ERROR_S,
  WMS_RPC_ERROR_S,

  WMS_STATUS_MAX,
  WMS_STATUS_MAX32 = 0x10000000    /* pad to 32 bit int */
} wms_status_e_type;


/* Information of a message report (SUBMIT-REPORT)
** received from the network
*/
typedef enum
{
  WMS_RPT_OK        = 0,
    /* Success: submit_report_ack info is available for GSM/WCDMA
    */

  WMS_RPT_LL_ERROR  = 1,
    /* Lower layer errors;
    ** For GSM/WCDMA: cause_value = LL Cause, not available to clients.
    */

  WMS_RPT_GENERAL_ERROR = 2,
    /* Any other error */


  /* ------- CDMA specific failures -------- */

  WMS_RPT_OUT_OF_RESOURCES = 3,
    /* e.g. out of memory buffer */

  WMS_RPT_NETWORK_NOT_READY = 4,

  WMS_RPT_PHONE_NOT_READY   = 5,

  WMS_RPT_NO_ACK            = 6,
    /* CDMA TL Ack is not received
    */

  WMS_RPT_CDMA_TL_ERROR     = 7,
    /* CDMA Transport Layer error from the network;
    ** error_class and status in cause_info are available
    */

  WMS_RPT_ACCESS_TOO_LARGE  = 8,
    /* CDMA Message is too large to be sent over access channel
    */

  WMS_RPT_DC_TOO_LARGE      = 9,
    /* CDMA message is too large to be sent over Dedicated channel
    */

  WMS_RPT_NOT_ALLOWED_IN_AMPS = 10,
    /* CDMA: cannot sent msg in analog mode
    */

  WMS_RPT_CANNOT_SEND_BROADCAST = 11,
    /* CDMA: sending broadcast message is invalid
    */

  WMS_RPT_ACCESS_BLOCK       = 12,
    /* CDMA: HDR access block
    */

  /* ------- GSM/WCDMA specific failures -------- */

  WMS_RPT_RP_ERROR         = 100,
    /* GSM/WCDMA Failure: submit_report_error TPDU is available;
    ** cause_value = RP Cause as in section 8.2.5.4 TS 24.011;
    ** diagnostics might be available as in section 8.2.5.4 TS 24.011
    */

  WMS_RPT_CP_ERROR         = 101,
    /* GSM/WCDMA Failure: no submit_report TPDU available;
    ** cause_value = CP Cause as in section 8.1.4.2 TS 24.011
    */

  WMS_RPT_SMR_TR1M_TIMEOUT = 102,
  WMS_RPT_SMR_TR2M_TIMEOUT = 103,
  WMS_RPT_SMR_NO_RESOURCES = 104,
    /* GSM/WCDMA Failures
    */

  WMS_RPT_MO_CONTROL_DISALLOW = 105,
    /* MO SMS Control disallows the MO message */

  WMS_RPT_MO_CONTROL_ERROR = 106,
    /* MO SMS Control problem */

  WMS_RPT_MO_RETRY_PERIOD_EXPIRED = 107,
    /* MO Retry Period Expired */

  WMS_RPT_NO_RESPONSE_FROM_NETWORK = 108,
    /* No Response from GSM / WCDMA Network */

  /* ------ SIP specific errors ----- */
  WMS_RPT_SIP_PERM_ERROR   = 200,
  WMS_RPT_SIP_TEMP_ERROR   = 201,

  WMS_RPT_MAX,
  WMS_RPT_MAX32 = 0x10000000

} wms_report_status_e_type;

typedef struct
{
    wms_memory_store_e_type         memStore_src;
    wms_message_index_type          index_src;
    wms_memory_store_e_type         memStore_dest;
    wms_message_index_type          index_dest;
    boolean                         delete_src;
} wms_msg_copy_type;


/* <EJECT> */
/* ========================================================================= */
/* ===================== CDMA specific type definitions ==================== */
/* ========================================================================= */

/* ------------------------ */
/* ---- Teleservice Id ---- */
/* ------------------------ */
typedef enum
{
  /*----------------------------------------------------------------
     The following values 0 - 0xFFFF are used in CDMA mode and Analog
     AWI SMS as defined in IS-637/IS-41.
  -----------------------------------------------------------------*/

  /* NOTE: In case of teleservice CM_91, the encoding type of the user data
     indicates whether the teleservice is actually CPT, VMN or CMT and the
     user data is extracted into the corresponding bearer data fields.
  */
  WMS_TELESERVICE_CMT_91             = 4096,  /* embedded IS91 SMS */
  WMS_TELESERVICE_CPT_95             = 4097,  /* page */
  WMS_TELESERVICE_CMT_95             = 4098,  /* short message */
  WMS_TELESERVICE_VMN_95             = 4099,  /* voice mail notification */
  WMS_TELESERVICE_WAP                = 4100,  /* WAP */
  WMS_TELESERVICE_WEMT               = 4101,  /* Enhanced Messaging/EMS */
  WMS_TELESERVICE_SCPT               = 4102,  /* Srv Category Programming */
  WMS_TELESERVICE_CATPT              = 4103,  /* Card App Toolkit Protocol */

  WMS_TELESERVICE_GSM1x_01           = 4104,  /* GSM1x signalling message */
  WMS_TELESERVICE_GSM1x_02           = 4105,  /* GSM1x short message */
  WMS_TELESERVICE_GSM1x_03           = 4106,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_04           = 4107,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_05           = 4108,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_06           = 4109,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_07           = 4110,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_08           = 4111,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_09           = 4112,  /* Reserved for now */
  WMS_TELESERVICE_GSM1x_10           = 4113,  /* Reserved for now */

  WMS_TELESERVICE_IMSST              = 4242,  /* IMS Services Teleservice */
#ifdef CUST_EDITION  
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
  WMS_TELESERVICE_ISRAEL_SILENT      = 49858,  /* silent sms      */
#endif
  
#if defined(FEATURE_POWERUP_REGISTER_CHINAUNICOM)||defined(FEATURE_VERSION_K212)||defined(FEATURE_VERSION_K212_BH)
  WMS_TELESERVICE_CHINAUNICOMREG     = 65005, /* 联通开机注册 */
#endif  

#if defined(FEATURE_QMA)
  WMS_TELESERVICE_QMA_EMAIL          = 65490,  /* UMS for QMA */
  WMS_TELESERVICE_QMA_WPUSH          = 65002,  /* QMA Wap push message service ID */
  WMS_TELESERVICE_QMA_PMAIL          = 65497,  /* Photo Mail for QMA */
#endif//#elif defined (FEATURE_CARRIER_CHINA_TELCOM)
  WMS_TELESERVICE_WPUSH              = 65002,  /* China telecom Wap push message service ID  */
//#endif
#endif /*CUST_EDITION*/
  /*---------------------------------------------------------------------
     The following are defined as extensions to the standard teleservices
  -----------------------------------------------------------------------*/

  /* teleservice types in Analog mode.
  */
  WMS_TELESERVICE_IS91_PAGE          = 0x00010000,
  WMS_TELESERVICE_IS91_VOICE_MAIL    = 0x00020000,
  WMS_TELESERVICE_IS91_SHORT_MESSAGE = 0x00030000,

  /* Voice mail notification through Message Waiting Indication in
     CDMA mode or Analog mode
  */
  WMS_TELESERVICE_MWI                = 0x00040000,

  /* Broadcast SMS messages
  */
  WMS_TELESERVICE_BROADCAST          = 0x00050000,

  WMS_TELESERVICE_UNKNOWN            = 0x0FFFFFFF

} wms_teleservice_e_type;


/* ------------------------ */
/* -- Service category ---- */
/* ------------------------ */
typedef enum
{
  WMS_SRV_UNKOWN          = 0,
  WMS_SRV_EMERGENCY,      // 1
  WMS_SRV_ADMIN,          // 2
  WMS_SRV_MAINTENANCE,    // 3
  WMS_SRV_GEN_NEWS_LOC,   // 4
  WMS_SRV_GEN_NEWS_REG,   // 5
  WMS_SRV_GEN_NEWS_NAT,   // 6
  WMS_SRV_GEN_NEWS_INT,   // 7
  WMS_SRV_FIN_NEWS_LOC,   // 8
  WMS_SRV_FIN_NEWS_REG,   // 9
  WMS_SRV_FIN_NEWS_NAT,   // 0xA
  WMS_SRV_FIN_NEWS_INT,   // 0xB
  WMS_SRV_SPT_NEWS_LOC,   // 0xC
  WMS_SRV_SPT_NEWS_REG,   // 0xD
  WMS_SRV_SPT_NEWS_NAT,   // 0xE
  WMS_SRV_SPT_NEWS_INT,   // 0xF
  WMS_SRV_ENT_NEWS_LOC,   // 0x10
  WMS_SRV_ENT_NEWS_REG,   // 0x11
  WMS_SRV_ENT_NEWS_NAT,   // 0x12
  WMS_SRV_ENT_NEWS_INT,   // 0x13
  WMS_SRV_LOC_WEATHER,    // 0x14
  WMS_SRV_AREA_TRAFFIC,   // 0x15
  WMS_SRV_AIRPORT_SCHED,  // 0x16
  WMS_SRV_RESTAURANTS,    // 0x17
  WMS_SRV_LODGINGS,       // 0x18
  WMS_SRV_RETAILS,        // 0x19
  WMS_SRV_ADS,            // 0x1A
  WMS_SRV_STOCK_QUOTES,   // 0x1B
  WMS_SRV_JOBS,           // 0x1C
  WMS_SRV_MEDICAL,        // 0x1D
  WMS_SRV_TECH_NEWS,      // 0x1E
  WMS_SRV_MULTI,          // 0x1F

  WMS_SRV_MAX32           = 0x10000000

} wms_service_e_type;


/* --------------------- */
/* ---- Error class ---- */
/* --------------------- */
typedef enum
{
  WMS_ERROR_NONE        = 0,
  WMS_ERROR_RESERVED_1  = 1,
  WMS_ERROR_TEMP        = 2,
  WMS_ERROR_PERM        = 3,
  WMS_ERROR_MAX32       = 0x10000000

} wms_error_class_e_type;


/* ---------------------- */
/* -- CDMA status code -- */
/* ---------------------- */
typedef enum
{
  /* The first half of the enums are from IS-41D SMS cause codes
     with the exact binary values as in IS-41D. They are in the range
     of 0x00 to 0xFF.
  */

  /* A. Network Problems:
  */
  WMS_TL_ADDRESS_VACANT_S                     = 0,
  WMS_TL_ADDRESS_TRANSLATION_FAILURE_S,
  WMS_TL_NETWORK_RESOURCE_SHORTAGE_S,
  WMS_TL_NETWORK_FAILURE_S,
  WMS_TL_INVALID_TELESERVICE_ID_S,
  WMS_TL_OTHER_NETWORK_PROBLEM_S,
  WMS_TL_OTHER_NETWORK_PROBLEM_MORE_FIRST_S   = 6,
  /* all values within this range are treated as
     WMS_TL_OTHER_NETWORK_PROBLEM_S
  */
  WMS_TL_OTHER_NETWORK_PROBLEM_MORE_LAST_S    = 31,

  /* B. Terminal Problems:
  */
  WMS_TL_NO_PAGE_RESPONSE_S                   = 32,
  WMS_TL_DESTINATION_BUSY_S,
  WMS_TL_NO_ACK_S,
  WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S,
  WMS_TL_SMS_DELIVERY_POSTPONED_S,
  WMS_TL_DESTINATION_OUT_OF_SERVICE_S,
  WMS_TL_DESTINATION_NO_LONGER_AT_THIS_ADDRESS_S,
  WMS_TL_OTHER_TERMINAL_PROBLEM_S,
  WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_FIRST_S  = 40,
  /* all values within this range are treated as
     WMS_TL_OTHER_TERMINAL_PROBLEM_S
  */
  WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_LAST_S   = 47,
  WMS_TL_SMS_DELIVERY_POSTPONED_MORE_FIRST_S  = 48,
  WMS_TL_SMS_DELIVERY_POSTPONED_MORE_LAST_S   = 63,

  /* C. Radio Interface Problems:
  */
  WMS_TL_RADIO_IF_RESOURCE_SHORTAGE_S         = 64,
  WMS_TL_RADIO_IF_INCOMPATIBLE_S,
  WMS_TL_OTHER_RADIO_IF_PROBLEM_S,
  WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_FIRST_S  = 67,
  /* all values within this range are treated as
     WMS_TL_OTHER_RADIO_IF_PROBLEM_S
  */
  WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_LAST_S   = 95,

  /* D. General Problems:
  */
  WMS_TL_UNEXPECTED_PARM_SIZE_S               = 96,
  WMS_TL_SMS_ORIGINATION_DENIED_S,
  WMS_TL_SMS_TERMINATION_DENIED_S,
  WMS_TL_SUPPL_SERVICE_NOT_SUPPORTED,
  WMS_TL_SMS_NOT_SUPPORTED_S,
  WMS_TL_RESERVED_101_S,
  WMS_TL_MISSING_EXPECTED_PARM_S,
  WMS_TL_MISSING_MANDATORY_PARM_S,
  WMS_TL_UNRECOGNIZED_PARM_VALUE_S,
  WMS_TL_UNEXPECTED_PARM_VALUE_S,
  WMS_TL_USER_DATA_SIZE_ERROR_S,
  WMS_TL_OTHER_GENERAL_PROBLEMS_S,
  WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_FIRST_S  = 108,
  /* all values within this range are treated as
     WMS_TL_OTHER_GENERAL_PROBLEMS_S
  */
  WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_LAST_S   = 255,
  WMS_TL_MAX32                                = 0x10000000

} wms_cdma_tl_status_e_type;



/* ----------------------- */
/* ---- Message types ---- */
/* ----------------------- */
typedef enum
{
  WMS_BD_TYPE_RESERVED_0     = 0,
  WMS_BD_TYPE_DELIVER,       /* MT only */
  WMS_BD_TYPE_SUBMIT,        /* MO only */
  WMS_BD_TYPE_CANCELLATION,  /* MO only */
  WMS_BD_TYPE_DELIVERY_ACK,  /* MT only */
  WMS_BD_TYPE_USER_ACK,      /* MT & MO */
  WMS_BD_TYPE_READ_ACK,      /* MT & MO */
  WMS_BD_TYPE_MAX32          = 0x10000000

} wms_bd_message_type_e_type;


#define WMS_IS_MO_CDMA_MSG_TYPE( type )         \
          ( (type) == WMS_BD_TYPE_SUBMIT ||       \
            (type) == WMS_BD_TYPE_CANCELLATION || \
            (type) == WMS_BD_TYPE_USER_ACK ||     \
            (type) == WMS_BD_TYPE_READ_ACK )


/* -------------------- */
/* ---- Message Id ---- */
/* -------------------- */
typedef struct wms_message_id_s
{
  wms_bd_message_type_e_type   type;
  wms_message_number_type      id_number;
  boolean                      udh_present;
    /* NOTE: if FEATURE_SMS_UDH is not defined,
    ** udh_present should be ignored.
    */
} wms_message_id_s_type;


/* ----------------------------- */
/* -- User data encoding type -- */
/* ----------------------------- */
typedef enum
{
  WMS_ENCODING_OCTET        = 0,    /* 8-bit */
  WMS_ENCODING_IS91EP,              /* varies */
  WMS_ENCODING_ASCII,               /* 7-bit */
  WMS_ENCODING_IA5,                 /* 7-bit */
  WMS_ENCODING_UNICODE,             /* 16-bit */
  WMS_ENCODING_SHIFT_JIS,           /* 8 or 16-bit */
  WMS_ENCODING_KOREAN,              /* 8 or 16-bit */
  WMS_ENCODING_LATIN_HEBREW,        /* 8-bit */
  WMS_ENCODING_LATIN,               /* 8-bit */
  WMS_ENCODING_GSM_7_BIT_DEFAULT,    /* 7-bit */
  WMS_ENCODING_MAX32        = 0x10000000

} wms_user_data_encoding_e_type;


/* ------------------------ */
/* -- IS-91 EP data type -- */
/* ------------------------ */
typedef enum
{
  WMS_IS91EP_VOICE_MAIL         = 0x82,
  WMS_IS91EP_SHORT_MESSAGE_FULL = 0x83,
  WMS_IS91EP_CLI_ORDER          = 0x84,
  WMS_IS91EP_SHORT_MESSAGE      = 0x85,
  WMS_IS91EP_MAX32              = 0x10000000

} wms_IS91EP_type_e_type;


/* ------------------ */
/* ---- Priority ---- */
/* ------------------ */
typedef enum
{
  WMS_PRIORITY_NORMAL      = 0,
  WMS_PRIORITY_INTERACTIVE,
  WMS_PRIORITY_URGENT,
  WMS_PRIORITY_EMERGENCY,
  WMS_PRIORITY_MAX32       = 0x10000000

} wms_priority_e_type;


/* ----------------- */
/* ---- Privacy ---- */
/* ----------------- */
typedef enum
{
  WMS_PRIVACY_NORMAL      = 0,
  WMS_PRIVACY_RESTRICTED,
  WMS_PRIVACY_CONFIDENTIAL,
  WMS_PRIVACY_SECRET,
  WMS_PRIVACY_MAX32       = 0x10000000

} wms_privacy_e_type;


/* ---------------------------------- */
/* ---------- Display Mode ---------- */
/* ---------------------------------- */
typedef enum
{
  WMS_DISPLAY_MODE_IMMEDIATE   = 0,
  WMS_DISPLAY_MODE_DEFAULT     = 1,
  WMS_DISPLAY_MODE_USER_INVOKE = 2,
  WMS_DISPLAY_MODE_RESERVED    = 3
} wms_display_mode_e_type;

typedef enum
{
  WMS_ALERT_MODE_DEFAULT         = 0,
  WMS_ALERT_MODE_LOW_PRIORITY    = 1,
  WMS_ALERT_MODE_MEDIUM_PRIORITY = 2,
  WMS_ALERT_MODE_HIGH_PRIORITY   = 3,

  /* For pre-IS637A implementations, alert_mode only has values of True/False:
  */
  WMS_ALERT_MODE_OFF   = 0,
  WMS_ALERT_MODE_ON    = 1

} wms_alert_mode_e_type;


/* ---------------------------------- */
/* ---------- Download Mode --------- */
/* ---------------------------------- */
typedef enum
{
  WMS_DOWNLOAD_MODE_NONE         = 0,
  WMS_DOWNLOAD_MODE_PP_VAS       = 0x20,
  WMS_DOWNLOAD_MODE_PP_PRL       = 0x30,
  WMS_DOWNLOAD_MODE_UPDATE_PRL   = 0x10
} wms_download_mode_e_type;


/* IS-637B parameters/fields
*/
/* ---------------------------------- */
/* ---------- Delivery Status ------- */
/* ---------------------------------- */
typedef enum
{
  WMS_DELIVERY_STATUS_ACCEPTED              = 0,    /* ERROR_CLASS_NONE */
  WMS_DELIVERY_STATUS_DEPOSITED_TO_INTERNET = 1,    /* ERROR_CLASS_NONE */
  WMS_DELIVERY_STATUS_DELIVERED             = 2,    /* ERROR_CLASS_NONE */
  WMS_DELIVERY_STATUS_CANCELLED             = 3,    /* ERROR_CLASS_NONE */

  WMS_DELIVERY_STATUS_NETWORK_CONGESTION  = 4,    /* ERROR_CLASS_TEMP & PERM */
  WMS_DELIVERY_STATUS_NETWORK_ERROR       = 5,    /* ERROR_CLASS_TEMP & PERM */
  WMS_DELIVERY_STATUS_CANCEL_FAILED       = 6,    /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_BLOCKED_DESTINATION = 7,    /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_TEXT_TOO_LONG       = 8,    /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_DUPLICATE_MESSAGE   = 9,    /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_INVALID_DESTINATION = 10,   /* ERROR_CLASS_PERM */
  WMS_DELIVERY_STATUS_MESSAGE_EXPIRED     = 13,   /* ERROR_CLASS_PERM */

  WMS_DELIVERY_STATUS_UNKNOWN_ERROR       = 0x1F  /* ERROR_CLASS_PERM */

  /* All the other values are reserved */

} wms_delivery_status_e_type;

typedef struct wms_delivery_status_s
{
  wms_error_class_e_type       error_class;
  wms_delivery_status_e_type   status;
} wms_delivery_status_s_type;


/* Service options of dedicated channel
*/
typedef enum
{
  WMS_DC_SO_AUTO = 0,
  WMS_DC_SO_6    = 6,
  WMS_DC_SO_14   = 14,
  WMS_DC_SO_MAX32   = 0x10000000      /* pad to 32 bit int*/

} wms_dc_so_e_type;



/* <EJECT> */
/* ========================================================================= */
/* ===================== GSM/WCDMA specific type definitions =============== */
/* ========================================================================= */

/* GW TPDU types
*/
typedef enum
{
  WMS_TPDU_DELIVER = 0,
  WMS_TPDU_DELIVER_REPORT_ACK,
  WMS_TPDU_DELIVER_REPORT_ERROR,
  WMS_TPDU_SUBMIT,
  WMS_TPDU_SUBMIT_REPORT_ACK,
  WMS_TPDU_SUBMIT_REPORT_ERROR,
  WMS_TPDU_STATUS_REPORT,
  WMS_TPDU_COMMAND,
  WMS_TPDU_MAX,
  WMS_TPDU_NONE,
  WMS_TPDU_MAX32 = 0x10000000
} wms_gw_tpdu_type_e_type;


#define WMS_IS_MO_TPDU_TYPE( type )      \
          ( (type) == WMS_TPDU_SUBMIT ||   \
            (type) == WMS_TPDU_COMMAND )


/* GW Command types - ref. 3GPP TS 23.040 section 9.2.3.19
*/
typedef enum
{
  WMS_GW_COMMAND_ENQUIRY                = 0x00,
  WMS_GW_COMMAND_CANCEL_STATUS_REPORT   = 0x01,
  WMS_GW_COMMAND_DELETE_SM              = 0x02,
  WMS_GW_COMMAND_ENABLE_STATUS_REPORT   = 0x03,
  WMS_GW_COMMAND_MAX32 = 0x10000000   /* pas to 32 bit int */
  /* reserved: 0x04 - 0x1f */
  /* specific to each SC: 0xe0 - 0xff */
} wms_gw_command_e_type;


/* TP-Failure-Cause - ref 3GPP TS 23.040 section 9.2.3.22
*/
typedef enum
{
  /* values from 0x00 to 0x7f are reserved */

  /* TP-PID errors:
  */
  WMS_TP_CAUSE_TELEMATIC_NOT_SUPPORTED    = 0x80,   // MO
  WMS_TP_CAUSE_SM_TYPE0_NOT_SUPPORTED     = 0x81,   // MO & MT
  WMS_TP_CAUSE_CANNOT_REPLACE_SM          = 0x82,   // MO & MT
  /* ... values from 0x83 to 0x8e are reserved ... */
  WMS_TP_CAUSE_UNSPECIFIED_PID_ERROR      = 0x8f,   // MO & MT

  /* TP-DCS errors:
  */
  WMS_TP_CAUSE_ALPHABET_NOT_SUPPORTED     = 0x90,   // MO
  WMS_TP_CAUSE_MSG_CLASS_NOT_SUPPORTED    = 0x91,   //      MT
  /* ... values from 0x92 to 0x9e are reserved ... */
  WMS_TP_CAUSE_UNSPECIFIED_DCS_ERROR      = 0x9f,   // MO & MT

  /* TP-Command errors:
  */
  WMS_TP_CAUSE_CANNOT_PERFORM_COMMAND     = 0xa0,   // MO
  WMS_TP_CAUSE_COMMAND_NOT_SUPPORTED      = 0xa1,   // MO
  /* ... values from 0xa2 to 0xae are reserved ... */
  WMS_TP_CAUSE_UNSPECIFIED_CMD_ERROR      = 0xaf,   // MO

  WMS_TP_CAUSE_TPDU_NOT_SUPPORTED         = 0xb0,   // MO & MT
  /* ... values from 0xb1 to 0xbf are reserved ... */

  WMS_TP_CAUSE_SC_BUSY                    = 0xc0,   // MO
  WMS_TP_CAUSE_NO_SC_SUBSCRIPTION         = 0xc1,   // MO
  WMS_TP_CAUSE_SC_FAILURE                 = 0xc2,   // MO
  WMS_TP_CAUSE_INVALID_SME_ADDRESS        = 0xc3,   // MO
  WMS_TP_CAUSE_DESTINATION_SME_BARRED     = 0xc4,   // MO
  WMS_TP_CAUSE_REJECTED_DUPLICATE_SM      = 0xc5,   // MO
  WMS_TP_CAUSE_VPF_NOT_SUPPORTED          = 0xc6,   // MO
  WMS_TP_CAUSE_VP_NOT_SUPPORTED           = 0xc7,   // MO
  /* values from 0xc8 to 0xcf are reserved */

  WMS_TP_CAUSE_SIM_FULL                   = 0xd0,   //      MT
  WMS_TP_CAUSE_NO_SM_STORAGE_IN_SIM       = 0xd1,   //      MT
  WMS_TP_CAUSE_ERROR_IN_MS                = 0xd2,   //      MT
  WMS_TP_CAUSE_MEMORY_FULL                = 0xd3,   //      MT
  WMS_TP_CAUSE_SAT_BUSY                   = 0xd4,   //      MT
  WMS_TP_CAUSE_SIM_DATA_DOWNLOAD_ERROR    = 0xd5,   //      MT
  /* values from 0xd6 to 0xdf are reserved */

  /* values from 0xe0 to 0xfe are for errors specific to an application */

  WMS_TP_CAUSE_UNSPECIFIED_ERROR          = 0xff    // MO & MT
} wms_tp_cause_e_type;


/* RP-Cause - ref TS 24.011 section 8.2.5.4
*/
typedef enum
{
  WMS_RP_CAUSE_UNASSIGNED_NUMBER                  = 1, /* MO error */
  WMS_RP_CAUSE_OPERATOR_DETERMINED_BARRING        = 8, /* MO error */
  WMS_RP_CAUSE_CALL_BARRED                        = 10, /* MO error */
  WMS_RP_CAUSE_RESERVED_11                        = 11, /* MO error */
  WMS_RP_CAUSE_NETWORK_FAILURE                    = 17, /* CP error */
  WMS_RP_CAUSE_SM_REJECTED                        = 21, /* MO error */
  WMS_RP_CAUSE_MEMORY_EXCEEDED                    = 22, /* MT & CP error */
  WMS_RP_CAUSE_DESTINATION_OUT_OF_ORDER           = 27, /* MO error */
  WMS_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER            = 28, /* MO error */
  WMS_RP_CAUSE_FACILITY_REJECTED                  = 29, /* MO error */
  WMS_RP_CAUSE_UNKNOWN_SUBSCRIBER                 = 30, /* MO & SMMA error */
  WMS_RP_CAUSE_NETWORK_OUT_OF_ORDER               = 38, /* MO & SMMA error */
  WMS_RP_CAUSE_TEMPORARY_FAILURE                  = 41, /* MO & SMMA error */
  WMS_RP_CAUSE_CONGESTION                         = 42, /* MO & SMMA error */
  WMS_RP_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED  = 47, /* MO & SMMA error */
  WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED  = 50, /* MO error */
  WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED = 69, /* MO & SMMA error */
  WMS_RP_CAUSE_INVALID_SM_TRANSFER_REF_VALUE      = 81, /* MO & MT & CP error */
  WMS_RP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE     = 95, /* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION      = 96, /* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT          = 97, /* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE        = 98, /* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99, /* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED         = 111,/* MO/MT/SMMA/CP error */
  WMS_RP_CAUSE_INTERWORKING_UNSPECIFIED           = 127 /* MO & SMMA error */

  /* All other MO errors shall be treated as cause number 41 "Temporary Failure"
  */

  /* All other MT errors shall be treated as cause number 111 "Protocol
  ** error, unspecified
  */

  /* All other SMMA errors shall be treated as cause number 41
  */

  /* All other CP errors shall be treated as cause number 111
  */
} wms_rp_cause_e_type;


/* TP-Status - ref. 3GPP TS 23.040 section 9.2.3.15
*/
typedef enum
{
  /* Short message transaction completed:
  */
  WMS_TP_STATUS_RECEIVED_OK                   = 0x00,
  WMS_TP_STATUS_UNABLE_TO_CONFIRM_DELIVERY    = 0x01,
  WMS_TP_STATUS_REPLACED                      = 0x02,
  /* reserved: 0x03 - 0x0f */
  /* specific to each SC: 0x10 - 0x1f */

  /* Temporary error, SC still trying to transfer SM:
  */
  WMS_TP_STATUS_TRYING_CONGESTION             = 0x20,
  WMS_TP_STATUS_TRYING_SME_BUSY               = 0x21,
  WMS_TP_STATUS_TRYING_NO_RESPONSE_FROM_SME   = 0x22,
  WMS_TP_STATUS_TRYING_SERVICE_REJECTED       = 0x23,
  WMS_TP_STATUS_TRYING_QOS_NOT_AVAILABLE      = 0x24,
  WMS_TP_STATUS_TRYING_SME_ERROR              = 0x25,
  /* reserved: 0x26 - 0x2f */
  /* specific to each SC: 0x30 - 0x3f */

  /* Permanent error, SC is not making any more attempts:
  */
  WMS_TP_STATUS_PERM_REMOTE_PROCEDURE_ERROR   = 0x40,
  WMS_TP_STATUS_PERM_INCOMPATIBLE_DEST        = 0x41,
  WMS_TP_STATUS_PERM_REJECTED_BY_SME          = 0x42,
  WMS_TP_STATUS_PERM_NOT_OBTAINABLE           = 0x43,
  WMS_TP_STATUS_PERM_QOS_NOT_AVAILABLE        = 0x44,
  WMS_TP_STATUS_PERM_NO_INTERWORKING          = 0x45,
  WMS_TP_STATUS_PERM_VP_EXPIRED               = 0x46,
  WMS_TP_STATUS_PERM_DELETED_BY_ORIG_SME      = 0x47,
  WMS_TP_STATUS_PERM_DELETED_BY_SC_ADMIN      = 0x48,
  WMS_TP_STATUS_PERM_SM_NO_EXISTING           = 0x49,
  /* reserved: 0x4a - 0x4f */
  /* specific to each SC: 0x50 - 0x5f */

  /* Temporary error, SC is not making any more attempts:
  */
  WMS_TP_STATUS_TEMP_CONGESTION               = 0x60,
  WMS_TP_STATUS_TEMP_SME_BUSY                 = 0x61,
  WMS_TP_STATUS_TEMP_NO_RESPONSE_FROM_SME     = 0x62,
  WMS_TP_STATUS_TEMP_SERVICE_REJECTED         = 0x63,
  WMS_TP_STATUS_TEMP_QOS_NOT_AVAILABLE        = 0x64,
  WMS_TP_STATUS_TEMP_SME_ERROR                = 0x65,
  /* reserved: 0x66 - 0x6f */
  /* specific to each SC: 0x70 - 0x7f */

  /* reserved: 0x80 - 0xff */

  WMS_TP_STATUS_LAST = 0xFF,
  WMS_TP_STATUS_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_tp_status_e_type;


/* GW Protocol ID - ref 3GPP TS 23.040, section 9.2.3.9
*/
typedef enum
{
  /* values from 0x00 to 0x1f are for SM-AL protocols:
  */
  WMS_PID_DEFAULT           = 0x00,  /* default value to use */

  /* values from 0x20 to 0x3f are for telematic interworking:
  */
  WMS_PID_IMPLICIT          = 0x20,
  WMS_PID_TELEX             = 0x21,
  WMS_PID_G3_FAX            = 0x22,
  WMS_PID_G4_FAX            = 0x23,
  WMS_PID_VOICE_PHONE       = 0x24,
  WMS_PID_ERMES             = 0x25,
  WMS_PID_NAT_PAGING        = 0x26,
  WMS_PID_VIDEOTEX          = 0x27,
  WMS_PID_TELETEX_UNSPEC    = 0x28,
  WMS_PID_TELETEX_PSPDN     = 0x29,
  WMS_PID_TELETEX_CSPDN     = 0x2a,
  WMS_PID_TELETEX_PSTN      = 0x2b,
  WMS_PID_TELETEX_ISDN      = 0x2c,
  WMS_PID_UCI               = 0x2d,
  WMS_PID_RESERVED_0x2e     = 0x2e,
  WMS_PID_RESERVED_0x2f     = 0x2f,
  WMS_PID_MSG_HANDLING      = 0x30,
  WMS_PID_X400              = 0x31,
  WMS_PID_INTERNET_EMAIL    = 0x32,
  WMS_PID_RESERVED_0x33     = 0x33,
  WMS_PID_RESERVED_0x34     = 0x34,
  WMS_PID_RESERVED_0x35     = 0x35,
  WMS_PID_RESERVED_0x36     = 0x36,
  WMS_PID_RESERVED_0x37     = 0x37,
  WMS_PID_SC_SPECIFIC_1     = 0x38,
  WMS_PID_SC_SPECIFIC_2     = 0x39,
  WMS_PID_SC_SPECIFIC_3     = 0x3a,
  WMS_PID_SC_SPECIFIC_4     = 0x3b,
  WMS_PID_SC_SPECIFIC_5     = 0x3c,
  WMS_PID_SC_SPECIFIC_6     = 0x3d,
  WMS_PID_SC_SPECIFIC_7     = 0x3e,
  WMS_PID_GSM_UMTS          = 0x3f,

  /* values from 0x40 to 0x7f: */
  WMS_PID_SM_TYPE_0         = 0x40,
  WMS_PID_REPLACE_SM_1      = 0x41,
  WMS_PID_REPLACE_SM_2      = 0x42,
  WMS_PID_REPLACE_SM_3      = 0x43,
  WMS_PID_REPLACE_SM_4      = 0x44,
  WMS_PID_REPLACE_SM_5      = 0x45,
  WMS_PID_REPLACE_SM_6      = 0x46,
  WMS_PID_REPLACE_SM_7      = 0x47,
  /* ... values reserved not listed ... */
  WMS_PID_EMS               = 0x5e,
  WMS_PID_RETURN_CALL       = 0x5f,
  /* ... values reserved not listed ... */
  WMS_PID_ANSI136_R_DATA    = 0x7c,
  WMS_PID_ME_DATA_DOWNLOAD  = 0x7d,
  WMS_PID_ME_DEPERSONALIZE  = 0x7e,
  WMS_PID_SIM_DATA_DOWNLOAD = 0x7f,
  WMS_PID_E_MAX32 = 0x10000000   /* pas to 32 bit int */

  /* values from 0x80 to 0xbf are reserved */

  /* values from 0xc0 to 0xff are for SC specific use */

} wms_pid_e_type;


/* GW Alphabet sets
*/
typedef enum
{
  WMS_GW_ALPHABET_7_BIT_DEFAULT,
  WMS_GW_ALPHABET_8_BIT,
  WMS_GW_ALPHABET_UCS2,
  WMS_GW_ALPHABET_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_gw_alphabet_e_type;


/* GW message waiting actions
*/
typedef enum
{
  WMS_GW_MSG_WAITING_NONE,
  WMS_GW_MSG_WAITING_DISCARD,
  WMS_GW_MSG_WAITING_STORE,
  WMS_GW_MSG_WAITING_NONE_1111,
  WMS_GW_MSG_WAITING_MAX32 = 0x10000000 /* pas to 32 bit int */
} wms_gw_msg_waiting_e_type;


/* GW message waiting types
*/
typedef enum
{
  WMS_GW_MSG_WAITING_VOICEMAIL,
  WMS_GW_MSG_WAITING_FAX,
  WMS_GW_MSG_WAITING_EMAIL,
  WMS_GW_MSG_WAITING_OTHER,
  WMS_GW_MSG_WAITING_KIND_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_gw_msg_waiting_kind_e_type;


/* GW Validity Formats
*/
typedef enum
{
  WMS_GW_VALIDITY_NONE = 0,
  WMS_GW_VALIDITY_RELATIVE = 2,
  WMS_GW_VALIDITY_ABSOLUTE = 3,
  WMS_GW_VALIDITY_ENHANCED = 1,
  WMS_GW_VALIDITY_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_gw_validity_format_e_type;


/* SMS User Data Header IDs
*/
typedef enum
{
  WMS_UDH_CONCAT_8         = 0x00,
  WMS_UDH_SPECIAL_SM,
  /* 02 - 03    Reserved */
  WMS_UDH_PORT_8           = 0x04,
  WMS_UDH_PORT_16,
  WMS_UDH_SMSC_CONTROL,
  WMS_UDH_SOURCE,
  WMS_UDH_CONCAT_16,
  WMS_UDH_WCMP,
  WMS_UDH_TEXT_FORMATING,
  WMS_UDH_PRE_DEF_SOUND,
  WMS_UDH_USER_DEF_SOUND,
  WMS_UDH_PRE_DEF_ANIM,
  WMS_UDH_LARGE_ANIM,
  WMS_UDH_SMALL_ANIM,
  WMS_UDH_LARGE_PICTURE,
  WMS_UDH_SMALL_PICTURE,
  WMS_UDH_VAR_PICTURE,

  WMS_UDH_USER_PROMPT      = 0x13,
  WMS_UDH_EXTENDED_OBJECT  = 0x14,

  /* 15 - 1F    Reserved for future EMS */

  WMS_UDH_RFC822           = 0x20,

  /*  21 - 6F    Reserved for future use */
  /*  70 - 7f    Reserved for (U)SIM Toolkit Security Headers */
  /*  80 - 9F    SME to SME specific use */
  /*  A0 - BF    Reserved for future use */
  /*  C0 - DF    SC specific use */
  /*  E0 - FF    Reserved for future use */

  WMS_UDH_OTHER            = 0xFFFF, /* For unsupported or proprietary headers */
  WMS_UDH_ID_MAX32 = 0x10000000   /* pas to 32 bit int */

} wms_udh_id_e_type;


typedef struct wms_udh_concat_8_s
{
  uint8       msg_ref;
  /*indicates the reference number for a particular concatenated short message. */
  /*it is constant for every short message which makes up a particular concatenated short message*/
  uint8       total_sm;
  /*indicates the total number of short messages within the concatenated short message.
   The value shall start at 1 and remain constant for every short message which makes up the concatenated short message.
   if it is 0 then the receiving entity shall ignore the whole Information Element*/
   uint8      seq_num;
   /*it indicates the sequence number of a particular short message within the concatenated short */
   /* message. The value shall start at 1 and increment by one for every short message sent within the concatenated short message. If the value is zero or the value is greater than the value in octet 2 then the receiving entity shall ignore the whole Information Element.*/

} wms_udh_concat_8_s_type;


typedef struct wms_udh_special_sm_s
{
  wms_gw_msg_waiting_e_type                  msg_waiting;
  wms_gw_msg_waiting_kind_e_type             msg_waiting_kind;
  uint8                                      message_count;
  /*it indicates the number of messages of the type specified in Octet 1 waiting.*/
} wms_udh_special_sm_s_type;



typedef struct wms_udh_wap_8_s
{
  uint8  dest_port;
  uint8  orig_port;
} wms_udh_wap_8_s_type;

typedef struct wms_udh_wap_16_s
{
  uint16  dest_port;
  uint16  orig_port;
} wms_udh_wap_16_s_type;



typedef struct wms_udh_concat_16_s
{
  uint16      msg_ref;
  uint8       total_sm;
  uint8       seq_num;

} wms_udh_concat_16_s_type;

/* EMS Headers */

/* --------------------------- */
/* Text Formatting Definitions */
/* --------------------------- */
typedef enum
{
  WMS_UDH_LEFT_ALIGNMENT = 0,
  WMS_UDH_CENTER_ALIGNMENT,
  WMS_UDH_RIGHT_ALIGNMENT,
  WMS_UDH_DEFAULT_ALIGNMENT,
  WMS_UDH_MAX_ALIGNMENT,
  WMS_UDH_ALIGNMENT_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_udh_alignment_e_type;

typedef enum
{
  WMS_UDH_FONT_NORMAL = 0,
  WMS_UDH_FONT_LARGE,
  WMS_UDH_FONT_SMALL,
  WMS_UDH_FONT_RESERVED,
  WMS_UDH_FONT_MAX,
  WMS_UDH_FONT_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_udh_font_size_e_type;

typedef enum
{
  WMS_UDH_TEXT_COLOR_BLACK          = 0x0,
  WMS_UDH_TEXT_COLOR_DARK_GREY      = 0x1,
  WMS_UDH_TEXT_COLOR_DARK_RED       = 0x2,
  WMS_UDH_TEXT_COLOR_DARK_YELLOW    = 0x3,
  WMS_UDH_TEXT_COLOR_DARK_GREEN     = 0x4,
  WMS_UDH_TEXT_COLOR_DARK_CYAN      = 0x5,
  WMS_UDH_TEXT_COLOR_DARK_BLUE      = 0x6,
  WMS_UDH_TEXT_COLOR_DARK_MAGENTA   = 0x7,
  WMS_UDH_TEXT_COLOR_GREY           = 0x8,
  WMS_UDH_TEXT_COLOR_WHITE          = 0x9,
  WMS_UDH_TEXT_COLOR_BRIGHT_RED     = 0xA,
  WMS_UDH_TEXT_COLOR_BRIGHT_YELLOW  = 0xB,
  WMS_UDH_TEXT_COLOR_BRIGHT_GREEN   = 0xC,
  WMS_UDH_TEXT_COLOR_BRIGHT_CYAN    = 0xD,
  WMS_UDH_TEXT_COLOR_BRIGHT_BLUE    = 0xE,
  WMS_UDH_TEXT_COLOR_BRIGHT_MAGENTA = 0xF,
  WMS_UDH_TEXT_COLOR_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_udh_text_color_e_type;

typedef struct wms_udh_text_formating_s
{
  uint8                     start_position;
  uint8                     text_formatting_length;
  wms_udh_alignment_e_type  alignment_type ;       /*bit 0 and  bit 1*/
  wms_udh_font_size_e_type  font_size ;            /*bit 3 and  bit 2*/
  boolean                   style_bold;            /*bit 4 */
  boolean                   style_italic;          /*bit 5  */
  boolean                   style_underlined;      /*bit 6 */
  boolean                   style_strikethrough;   /*bit 7 */

  boolean                   is_color_present;
    /* if FALSE, ignore the following color information */
  wms_udh_text_color_e_type text_color_foreground;
  wms_udh_text_color_e_type text_color_background;

} wms_udh_text_formating_s_type;


/* --------------------------- */
/* Sound Related Definitions   */
/* --------------------------- */

/* Predefined sound
*/
typedef struct wms_udh_pre_def_sound_s
{
  uint8       position;
  uint8       snd_number;
} wms_udh_pre_def_sound_s_type;

/* User Defined sound
*/
typedef struct wms_udh_user_def_sound_s
{
  uint8       data_length;
  uint8       position;
  uint8       user_def_sound[WMS_UDH_MAX_SND_SIZE];
} wms_udh_user_def_sound_s_type;


/* --------------------------- */
/* Picture Related Definitions */
/* --------------------------- */

/* Large picture
*/
typedef struct wms_udh_large_picture_data_s
{
  uint8                      position;
  uint8                      data[WMS_UDH_LARGE_PIC_SIZE];
} wms_udh_large_picture_data_s_type;

/* Small picture
*/
typedef struct wms_udh_small_picture_data_s
{
  uint8                     position;
  uint8                     data[WMS_UDH_SMALL_PIC_SIZE];
} wms_udh_small_picture_data_s_type;

/* Variable length picture
*/
typedef struct wms_udh_var_picture_s
{
  uint8       position;
  uint8       width;    /* Number of pixels - Should be a mutliple of 8 */
  uint8       height;
  uint8       data[WMS_UDH_VAR_PIC_SIZE];
} wms_udh_var_picture_s_type;


/* ----------------------------- */
/* Animation Related Definitions */
/* ----------------------------- */

/* Predefined animation
*/
typedef struct wms_udh_pre_def_anim_s
{
  uint8       position;
  uint8       animation_number;
} wms_udh_pre_def_anim_s_type;


/* Large animation
*/
typedef struct wms_udh_large_anim_s
{
  uint8        position;
  uint8        data[WMS_UDH_ANIM_NUM_BITMAPS][WMS_UDH_LARGE_BITMAP_SIZE];
} wms_udh_large_anim_s_type;


/* Small animation
*/
typedef struct wms_udh_small_anim_s
{
  uint8         position;
  uint8         data[WMS_UDH_ANIM_NUM_BITMAPS][WMS_UDH_SMALL_BITMAP_SIZE];
} wms_udh_small_anim_s_type;



typedef struct wms_udh_rfc822_s
{
  uint8        header_length;
} wms_udh_rfc822_s_type;


/* User Prompt Indicator UDH
*/
typedef struct wms_udh_user_prompt_s
{
  uint8       number_of_objects;
    /* Number of objects of the same kind that follow this header which will
    ** be stitched together by the applications. For example, 5 small pictures
    ** are to be stitched together horizontally, or 6 iMelody tones are to be
    ** connected together with intermediate iMelody header and footer ignored.
    ** Allowed objects to be stitched:
    **   - Images (small, large, variable)
    **   - User defined sounds
    */
} wms_udh_user_prompt_s_type;


#define WMS_UDH_EO_CONTROL_FORWARD        0x01
#define WMS_UDH_EO_CONTROL_USER_PROMPT    0x02

/* Extended Object IDs/types
*/
typedef enum
{
  WMS_UDH_EO_VCARD                   = 0x09,
  WMS_UDH_EO_VCALENDAR               = 0x0A,
  WMS_UDH_EO_MAX32 = 0x10000000   /* pas to 32 bit int */
} wms_udh_eo_id_e_type;

/* Contents of Extended Objects
*/
/* 3 bytes for UDHL, EO UDH_ID, EO UDH_LEN; 6 bytes for the whole CONCAT16 */
#define WMS_UDH_EO_DATA_SEGMENT_MAX    131 /* 140 - 3 - 6 */
typedef struct wms_udh_eo_content_s
{
  uint8         length;

  uint8         data[WMS_UDH_EO_DATA_SEGMENT_MAX];
    /* WMS_UDH_EO_VCARD: See http://www.imc.org/pdi/vcard-21.doc for payload */
    /* WMS_UDH_EO_VCALENDAR: See http://www.imc.org/pdi/vcal-10.doc */
    /* Or: Unsupported/proprietary extended objects */

} wms_udh_eo_content_s_type;


/* Extended Object UDH
*/
typedef struct wms_udh_eo_s
{
  /* Extended objects are to be used together with 16-bit concatenation
  ** UDH. The max number of segments supported for E.O. is 8 at least.
  */
  wms_udh_eo_content_s_type    content;

  boolean                                 first_segment;
    /* The following fields are only present in the first segment of a
    ** concatenated SMS message.
    */
  uint8                                   reference;
    /* Identify those extended object segments which should be linked together
    */
  uint16                                  length;
    /* Length of the whole extended object data
    */
  uint8                                   control;
  wms_udh_eo_id_e_type                    type;
  uint16                                  position;
    /* Absolute position of the E.O. in the whole text after concatenation,
    ** starting from 1.
    */
} wms_udh_eo_s_type;


typedef struct wms_udh_other_s
{
   wms_udh_id_e_type  header_id;
   uint8               header_length;
   uint8              data[WMS_UDH_OTHER_SIZE];
} wms_udh_other_s_type;

typedef struct wms_udh_s
{
  wms_udh_id_e_type               header_id;

  union wms_udh_u
  {
    wms_udh_concat_8_s_type             concat_8;       // 00
 /*~ CASE WMS_UDH_CONCAT_8 wms_udh_u.concat_8 */

    wms_udh_special_sm_s_type           special_sm;     // 01
 /*~ CASE WMS_UDH_SPECIAL_SM wms_udh_u.special_sm */
    wms_udh_wap_8_s_type                wap_8;          //04
 /*~ CASE WMS_UDH_PORT_8 wms_udh_u.wap_8 */
    wms_udh_wap_16_s_type               wap_16;         //05
 /*~ CASE WMS_UDH_PORT_16 wms_udh_u.wap_16 */
    wms_udh_concat_16_s_type            concat_16;      // 08
 /*~ CASE WMS_UDH_CONCAT_16 wms_udh_u.concat_16 */
    wms_udh_text_formating_s_type       text_formating; // 0a
 /*~ CASE WMS_UDH_TEXT_FORMATING wms_udh_u.text_formating */
    wms_udh_pre_def_sound_s_type        pre_def_sound;  // 0b
 /*~ CASE WMS_UDH_PRE_DEF_SOUND wms_udh_u.pre_def_sound */
    wms_udh_user_def_sound_s_type       user_def_sound; // 0c
 /*~ CASE WMS_UDH_USER_DEF_SOUND wms_udh_u.user_def_sound */
    wms_udh_pre_def_anim_s_type         pre_def_anim;   // 0d
 /*~ CASE WMS_UDH_PRE_DEF_ANIM wms_udh_u.pre_def_anim */
    wms_udh_large_anim_s_type           large_anim;     // 0e
 /*~ CASE WMS_UDH_LARGE_ANIM wms_udh_u.large_anim */
    wms_udh_small_anim_s_type           small_anim;     // 0f
 /*~ CASE WMS_UDH_SMALL_ANIM wms_udh_u.small_anim */ 
    wms_udh_large_picture_data_s_type   large_picture;  // 10
 /*~ CASE WMS_UDH_LARGE_PICTURE wms_udh_u.large_picture */
    wms_udh_small_picture_data_s_type   small_picture;  // 11
 /*~ CASE WMS_UDH_SMALL_PICTURE wms_udh_u.small_picture */
    wms_udh_var_picture_s_type          var_picture;    // 12
 /*~ CASE WMS_UDH_VAR_PICTURE wms_udh_u.var_picture */

    wms_udh_user_prompt_s_type          user_prompt;    // 13
 /*~ CASE WMS_UDH_USER_PROMPT wms_udh_u.user_prompt */
    wms_udh_eo_s_type                   eo;             // 14
 /*~ CASE WMS_UDH_EXTENDED_OBJECT wms_udh_u.eo */

    wms_udh_rfc822_s_type               rfc822;         // 20
 /*~ CASE WMS_UDH_RFC822 wms_udh_u.rfc822 */
    wms_udh_other_s_type                other;
 /*~ DEFAULT wms_udh_u.other */

  }u;   /*~ FIELD wms_udh_s.u DISC _OBJ_.header_id */
} wms_udh_s_type;




/* <EJECT> */
/* ========================================================================= */
/* ========================== Client group data types ====================== */
/* ========================================================================= */

/* Client types
*/
typedef enum
{
  WMS_CLIENT_TYPE_JAVA = 0,   /* Java */
  WMS_CLIENT_TYPE_BREW,       /* Brew TAPI */
  WMS_CLIENT_TYPE_WAP_MMS,    /* WAP/MMS browser */
  WMS_CLIENT_TYPE_WMS_APP,    /* Messaging App */
  WMS_CLIENT_TYPE_UI,         /* Softkey UI & Core App */
  WMS_CLIENT_TYPE_CAT,        /* Card Application Toolkit module, e.g. GSTK */
  WMS_CLIENT_TYPE_ATCOP,      /* AT command processor */
  WMS_CLIENT_TYPE_DIAG,       /* Diagnostic task */
  WMS_CLIENT_TYPE_GPS,        /* GPS application */
  WMS_CLIENT_TYPE_GSM1x,      /* GSM1x application */
  WMS_CLIENT_TYPE_WINCE,      /* WinCE applications */
  WMS_CLIENT_TYPE_PBM,        /* Phonebook Manager */
  WMS_CLIENT_TYPE_IMS,        /* IMS/MMD/AIMS */
  WMS_CLIENT_TYPE_RIL,        /* RIL Layer */
  WMS_CLIENT_TYPE_BREW_MP,    /* Brew Mobile Platform */
  WMS_CLIENT_FLOATING1,
  WMS_CLIENT_FLOATING2,
  WMS_CLIENT_TYPE_INTERNAL,   /* Internal use only! */
  WMS_CLIENT_TYPE_MAX,
  WMS_CLIENT_TYPE_MAX32 = 0x10000000   /* pad to 32 bit */
} wms_client_type_e_type;


/* Client ID
*/
typedef uint8 wms_client_id_type;

enum{ WMS_DUMMY_CLIENT_ID = 0xFF };



/* <EJECT> */
/* ========================================================================= */
/* ======================= Configuration group data types ================== */
/* ========================================================================= */

/* Routing information
*/
typedef struct wms_routing_s
{
  wms_route_e_type            route;
  wms_memory_store_e_type     mem_store;
} wms_routing_s_type;


/* Message route configuration
*/
typedef struct wms_routes_s
{
  wms_routing_s_type     pp_routes[WMS_MESSAGE_CLASS_MAX];

  wms_routing_s_type     bc_routes[WMS_MESSAGE_CLASS_MAX];
    /* Only make sense if FEATURE_CDSMS_BROADCAST is defined */

  boolean                transfer_status_report;
    /* GW only: If TRUE, don't store status reports but send them to clients */

} wms_routes_s_type;



/* Memory store usage status
*/
typedef struct wms_memory_status_s
{
  wms_memory_store_e_type     mem_store;  /* status of which memory store */
  uint32                      max_slots;  /* maximum number of slots */
  uint32                      free_slots; /* number of free slots */
  uint32                      used_tag_slots;
    /* Number of slots used by messages of the following tag. If the tag is
    ** WMS_TAG_NONE, this is the number of all used/occupied slots.
    */
  wms_message_tag_e_type      tag;
} wms_memory_status_s_type;


/* Message list for messages with a specific tag in a memory store
*/
typedef struct wms_message_list_s
{
  wms_memory_store_e_type     mem_store; /* which memory store */
  wms_message_tag_e_type      tag;       /* what msgs are included in list */
  uint32                      len;       /* how many indices returned */
  wms_message_index_type      voice_mail_index;
  boolean                     voice_mail_active;
  wms_message_index_type      indices[WMS_MESSAGE_LIST_MAX];
  wms_message_tag_e_type      tags[WMS_MESSAGE_LIST_MAX];
} wms_message_list_s_type;

/* Cell Change Causes
*/
typedef enum
{
  WMS_GW_CB_NO_SERVICE,
  WMS_GW_CB_SAME_CELL,
  WMS_GW_CB_CELL_CHANGE,
  WMS_GW_CB_LAC_CHANGE,
  WMS_GW_CB_PLMN_CHANGE,
  WMS_GW_CB_MAX32
}wms_gw_cb_cell_change_e_type;

/* Configuration event types
*/
typedef enum
{
  WMS_CFG_EVENT_GW_READY = 0,
  WMS_CFG_EVENT_CDMA_READY,
  WMS_CFG_EVENT_ROUTES,
  WMS_CFG_EVENT_MEMORY_STATUS,
  WMS_CFG_EVENT_MESSAGE_LIST,
  WMS_CFG_EVENT_MEMORY_FULL,
  WMS_CFG_EVENT_GW_DOMAIN_PREF,
  WMS_CFG_EVENT_CELL_CHANGE,
  WMS_CFG_EVENT_PRIMARY_CLIENT_SET,
  WMS_CFG_EVENT_MEMORY_STATUS_SET,
  WMS_CFG_EVENT_LINK_CONTROL,
#ifdef CUST_EDITION  
  WMS_CFG_EVENT_REFRESH_DONE,
#endif /*CUST_EDITION*/  
  WMS_CFG_EVENT_MAX,
  WMS_CFG_EVENT_MAX32 = 0x10000000
} wms_cfg_event_e_type;

#define WMS_CFG_EVENT_SIM_READY WMS_CFG_EVENT_GW_READY



/* Configuration event information for each event
*/
typedef union wms_cfg_event_info_u
{
  wms_routes_s_type           routes;
    /* WMS_CFG_EVENT_ROUTES
    */
 /*~ CASE WMS_CFG_EVENT_ROUTES wms_cfg_event_info_u.routes */
  wms_memory_status_s_type    memory_status;
    /* WMS_CFG_EVENT_MEMORY_STATUS */
 /*~ CASE WMS_CFG_EVENT_MEMORY_STATUS wms_cfg_event_info_u.memory_status */

  wms_message_list_s_type     message_list;
    /* WMS_CFG_EVENT_MESSAGE_LIST
    */
 /*~ CASE WMS_CFG_EVENT_MESSAGE_LIST wms_cfg_event_info_u.message_list */

  wms_memory_store_e_type      mem_store;
    /* WMS_CFG_EVENT_MEMORY_FULL
    */
 /*~ CASE WMS_CFG_EVENT_MEMORY_FULL wms_cfg_event_info_u.mem_store */

  wms_gw_domain_pref_e_type    gw_domain_pref;
    /* WMS_CFG_EVENT_GW_DOMAIN_PREF
    */
 /*~ CASE WMS_CFG_EVENT_GW_DOMAIN_PREF wms_cfg_event_info_u.gw_domain_pref */

  wms_gw_cb_cell_change_e_type cell_change_type;
    /* WMS_CFG_EVENT_CELL_CHANGE 
    */
/*~ CASE WMS_CFG_EVENT_CELL_CHANGE wms_cfg_event_info_u.cell_change_type */

  wms_cfg_set_primary_client_s_type  set_primary;
    /* WMS_CFG_PRIMARY_CLIENT_SET
    */
 /*~ CASE WMS_CFG_EVENT_PRIMARY_CLIENT_SET wms_cfg_event_info_u.set_primary */

  boolean                      memory_full;
    /* WMS_CFG_EVENT_MEMORY_STATUS_SET
    */
 /*~ CASE WMS_CFG_EVENT_MEMORY_STATUS_SET wms_cfg_event_info_u.memory_full */

  wms_cfg_link_control_s_type  link_control;
    /* WMS_CFG_EVENT_LINK_CONTROL */
 /*~ CASE WMS_CFG_EVENT_LINK_CONTROL wms_cfg_event_info_u.link_control */

 /*~ DEFAULT wms_cfg_event_info_u.void */
 /*  used as dummy data */

} wms_cfg_event_info_s_type;

/* Configuration event callback
*/
typedef void (*wms_cfg_event_cb_type)
(
  wms_cfg_event_e_type         event,
  wms_cfg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK wms_cfg_event_cb_type */
/*~ ONERROR return */


/* <EJECT> */
/* ========================================================================= */
/* ====================== Message group data types ========================= */
/* ========================================================================= */


/* Over-the-air raw TS data structure
*/
typedef struct wms_raw_ts_data_s
{
  wms_format_e_type           format;
  wms_gw_tpdu_type_e_type     tpdu_type;
    /* only meaningful if format is GSM/WCDMA messages */
  uint32                      len;
  uint8                       data[ WMS_MAX_LEN ];
} wms_raw_ts_data_s_type;



/* ====================== CDMA message definitions ========================= */


/* ------------------- */
/* ---- User Data ---- */
/* ------------------- */
typedef struct wms_cdma_user_data_s
{
  /* NOTE: If message_id.udh_present == TRUE:
  **       'num_headers' is the number of User Data Headers (UDHs),
  **       and 'headers' include all those headers.
  */
  uint8                              num_headers;
  wms_udh_s_type                     headers[WMS_MAX_UD_HEADERS];

  wms_user_data_encoding_e_type      encoding;
  wms_IS91EP_type_e_type             is91ep_type;

  /*----------------------------------------------------------------------
     'data_len' indicates the valid number of bytes in the 'data' array.

     'padding_bits' (0-7) indicates how many bits in the last byte of 'data'
     are invalid bits. This parameter is only used for Mobile-Originated
     messages. There is no way for the API to tell how many padding bits
     exist in the received message. Instead, the application can find out how
     many padding bits exist in the user data when decoding the user data.

     'data' has the raw bits of the user data field of the SMS message.
     The client software should decode the raw user data according to its
     supported encoding types and languages.

     EXCEPTION 1: CMT-91 user data raw bits are first translated into BD fields
     (e.g. num_messages, callback, etc.) The translated user data field in
     VMN and Short Message is in the form of ASCII characters, each occupying
     a byte in the resulted 'data'.

     EXCEPTION 2: GSM 7-bit Default characters are decoded so that each byte
     has one 7-bit GSM character.

     'number_of_digits' is the number of digits/characters (7, 8, 16, or
     whatever bits) in the raw user data, which can be used by the client
     when decoding the user data according to the encoding type and language.
  -------------------------------------------------------------------------*/
  uint8                                data_len;
  uint8                                padding_bits;
  uint8                                data[ WMS_CDMA_USER_DATA_MAX ];
  uint8                                number_of_digits;

} wms_cdma_user_data_s_type;


/* ----------------------- */
/* ---- User response ---- */
/* ----------------------- */
typedef uint8           wms_user_response_type;

#define WMS_RSP_APPLICATION_TOOLKIT_BUSY   WMS_TL_DESTINATION_BUSY_S
#define WMS_RSP_DATA_DOWNLOAD_ERROR        WMS_TL_OTHER_TERMINAL_PROBLEM_S

/* ---------------------- */
/* ---- Reply option ---- */
/* ---------------------- */
typedef struct wms_reply_option_s
{
  /* whether user ack is requested
  */
  boolean          user_ack_requested;

  /* whether delivery ack is requested.
     Should be FALSE for incoming messages.
  */
  boolean          delivery_ack_requested;

  /* Message originator requests the receiving phone to send back a READ_ACK
  ** message automatically when the user reads the received message.
  */
  boolean          read_ack_requested;

} wms_reply_option_s_type;


#ifdef CUST_EDITION
#define WMS_BC_SRV_LABEL_SIZE    10
#endif /*CUST_EDITION*/
#define WMS_BC_TABLE_MAX         ( 32 * 4 )

/* Service ID, which is a service/language pair
*/
typedef struct wms_bc_service_id_s
{
  wms_service_e_type   service;
  wms_language_e_type  language;
} wms_bc_service_id_s_type;

/* Alert options for receiving broadcast SMS
*/
typedef enum
{
  WMS_BC_ALERT_NONE  = 0,
  WMS_BC_ALERT_DEFAULT,
  WMS_BC_ALERT_VIBRATE_ONCE,
  WMS_BC_ALERT_VIBRATE_REPEAT,
  WMS_BC_ALERT_VISUAL_ONCE,
  WMS_BC_ALERT_VISUAL_REPEAT,
  WMS_BC_ALERT_LOW_PRIORITY_ONCE,
  WMS_BC_ALERT_LOW_PRIORITY_REPEAT,
  WMS_BC_ALERT_MEDIUM_PRIORITY_ONCE,
  WMS_BC_ALERT_MEDIUM_PRIORITY_REPEAT,
  WMS_BC_ALERT_HIGH_PRIORITY_ONCE,
  WMS_BC_ALERT_HIGH_PRIORITY_REPEAT
  /* other values are reserved */
} wms_bc_alert_e_type;


/* NOTE: All SCPT related definitions are for internal use only!
*/

/* Max number of services that can be included in a SCPT message
*/
#define WMS_BC_MM_SRV_LABEL_SIZE   30
#define WMS_BC_SCPT_MAX_SERVICES   5

/* Operation code for SCPT
*/
typedef enum
{
  WMS_BC_SCPT_OP_DELETE      = 0,
  WMS_BC_SCPT_OP_ADD         = 1,
  WMS_BC_SCPT_OP_DELETE_ALL  = 2,
  WMS_BC_SCPT_OP_MAX32       = 0x10000000
  /* other values are reserved */
} wms_bc_scpt_op_code_e_type;


/* Information for an entry in the SCPT Program Data
*/
typedef struct wms_bc_scpt_data_entry_s
{
  wms_bc_scpt_op_code_e_type         op_code;

  wms_bc_service_id_s_type           srv_id;

  uint8                              max_messages;
    /* max msgs to be stored for this service */
  wms_bc_alert_e_type                bc_alert;
    /* alert options for this srv */

  uint8                              label_len;
    /* Number of characters of the label.
    ** NOTE: For Unicode, each character is 2 bytes.
    */
  uint8                              label[WMS_BC_MM_SRV_LABEL_SIZE];
    /* Name/label for this service */

} wms_bc_scpt_data_entry_s_type;


/* The SCPT Program Data from the network
*/
typedef struct wms_bc_scpt_data_s
{
  wms_user_data_encoding_e_type      encoding; /* encoding of service name */
  uint8                              num_entries;
  wms_bc_scpt_data_entry_s_type      entry[WMS_BC_SCPT_MAX_SERVICES];

} wms_bc_scpt_data_s_type;


/* SCPT program result status
*/
typedef enum
{
  WMS_BC_SCPT_STATUS_OK   = 0,
  WMS_BC_SCPT_STATUS_SERVICE_MEMORY_FULL,
  WMS_BC_SCPT_STATUS_SERVICE_LIMIT_REACHED,
  WMS_BC_SCPT_STATUS_ALREADY_PROGRAMMED,
  WMS_BC_SCPT_STATUS_NOT_YET_PROGRAMMED,
  WMS_BC_SCPT_STATUS_INVALID_MAX_MESSAGES,
  WMS_BC_SCPT_STATUS_INVALID_ALERT_OPTION,
  WMS_BC_SCPT_STATUS_INVALID_SERVICE_NAME,
  WMS_BC_SCPT_STATUS_UNSPECIFIED_ERROR
  /* other values are reserved */
} wms_bc_scpt_status_e_type;


/* Information for an entry in the SCPT Program Result
*/
typedef struct wms_bc_scpt_result_entry_s
{
  wms_bc_service_id_s_type            srv_id;
  wms_bc_scpt_status_e_type           status;

} wms_bc_scpt_result_entry_s_type;


/* The SCPT Program Result to the network
*/
typedef struct wms_bc_scpt_result_s
{
  uint8                                    num_entries;
  wms_bc_scpt_result_entry_s_type          entry[WMS_BC_SCPT_MAX_SERVICES];
} wms_bc_scpt_result_s_type;

/* Multimode Broadcast Definitions */

typedef uint16     wms_gw_cb_srv_id_type;

/* GW CB Geographic scope
*/
typedef enum
{
  WMS_GW_CB_GEO_SCOPE_CELL_IMMEDIATE = 0,
  WMS_GW_CB_GEO_SCOPE_PLMN           = 1,
  WMS_GW_CB_GEO_SCOPE_LOCATION_AREA  = 2,
  WMS_GW_CB_GEO_SCOPE_CELL_NORMAL    = 3
} wms_gw_cb_geo_scope_e_type;


/* GW CB languages as defined in 3GPP TS 23.038
*/
typedef enum
{
  WMS_GW_CB_LANGUAGE_GERMAN      = 0x00,
  WMS_GW_CB_LANGUAGE_ENGLISH     = 0x01,
  WMS_GW_CB_LANGUAGE_ITALIAN     = 0x02,
  WMS_GW_CB_LANGUAGE_FRENCH      = 0x03,
  WMS_GW_CB_LANGUAGE_SPANISH     = 0x04,
  WMS_GW_CB_LANGUAGE_DUTCH       = 0x05,
  WMS_GW_CB_LANGUAGE_SWEDISH     = 0x06,
  WMS_GW_CB_LANGUAGE_DANISH      = 0x07,
  WMS_GW_CB_LANGUAGE_PORTUGUESE  = 0x08,
  WMS_GW_CB_LANGUAGE_FINNISH     = 0x09,
  WMS_GW_CB_LANGUAGE_NORWEGIAN   = 0x0a,
  WMS_GW_CB_LANGUAGE_GREEK       = 0x0b,
  WMS_GW_CB_LANGUAGE_TURKISH     = 0x0c,
  WMS_GW_CB_LANGUAGE_HUNGARIAN   = 0x0d,
  WMS_GW_CB_LANGUAGE_POLISH      = 0x0e,

  WMS_GW_CB_LANGUAGE_UNSPECIFIED = 0x0f,
    /* If language indication is not in the CB message, this is used */

  WMS_GW_CB_LANGUAGE_CZECH       = 0x20,
  WMS_GW_CB_LANGUAGE_RESERVED_21 = 0x21,
  WMS_GW_CB_LANGUAGE_RESERVED_22 = 0x22,
  WMS_GW_CB_LANGUAGE_RESERVED_23 = 0x23,
  WMS_GW_CB_LANGUAGE_RESERVED_24 = 0x24,
  WMS_GW_CB_LANGUAGE_RESERVED_25 = 0x25,
  WMS_GW_CB_LANGUAGE_RESERVED_26 = 0x26,
  WMS_GW_CB_LANGUAGE_RESERVED_27 = 0x27,
  WMS_GW_CB_LANGUAGE_RESERVED_28 = 0x28,
  WMS_GW_CB_LANGUAGE_RESERVED_29 = 0x29,
  WMS_GW_CB_LANGUAGE_RESERVED_2A = 0x2a,
  WMS_GW_CB_LANGUAGE_RESERVED_2B = 0x2b,
  WMS_GW_CB_LANGUAGE_RESERVED_2C = 0x2c,
  WMS_GW_CB_LANGUAGE_RESERVED_2D = 0x2d,
  WMS_GW_CB_LANGUAGE_RESERVED_2E = 0x2e,
  WMS_GW_CB_LANGUAGE_RESERVED_2F = 0x2f,

  WMS_GW_CB_LANGUAGE_ISO_639     = 0x10,
    /* ISO639 2-letter languages code is in the first 2 GSM 7-bit characters
    ** of the CB user data.
    */

  WMS_GW_CB_LANGUAGE_DUMMY       = 255

} wms_gw_cb_language_e_type;


/* GW CB Data Coding Scheme groups
*/
typedef enum
{
  WMS_GW_CB_DCS_GROUP_DEFINED   = 0,
  WMS_GW_CB_DCS_GROUP_WAP,
  WMS_GW_CB_DCS_GROUP_RESERVED
} wms_gw_cb_dcs_group_e_type;


/* GW CB Data Coding Schemes
*/
typedef struct wms_gw_cb_dcs_s
{
  wms_gw_cb_dcs_group_e_type  group;

  /* If group==DEFINED:
  */
  wms_message_class_e_type    msg_class;
  boolean                     is_compressed;
  wms_gw_alphabet_e_type      alphabet;
  wms_gw_cb_language_e_type   language;
  uint8                       iso_639_lang[3]; /* 2 GSM chars and a CR char */
  boolean                     is_udh_present;

  /* If group==WAP or RESERVED: (This is available for other DCS groups too)
  */
  uint8                       raw_dcs_data;
} wms_gw_cb_dcs_s_type;


/* GW CB Decoded Page Header
*/
typedef struct wms_gw_cb_page_header_s
{
  wms_gw_cb_srv_id_type        cb_srv_id;
  wms_gw_cb_dcs_s_type         cb_dcs;
  uint8                        total_pages; /* [1, 15] */
  uint8                        page_number; /* [1, 15] */

  /* The following are fields in Serial Number as defined in 3GPP 23.041 */
  wms_gw_cb_geo_scope_e_type   geo_scope;
  uint16                       message_code;  /* [0, 1023] */
  uint8                        update_number; /* [0, 15] */

  /* The original serial number raw data */
  uint16                       raw_serial_number;
} wms_gw_cb_page_header_s_type;


/* IP Address
*/
#define WMS_IP_ADDRESS_SIZE     4

typedef struct wms_ip_address_s
{
  uint8               address[WMS_IP_ADDRESS_SIZE];
  boolean             is_valid;
} wms_ip_address_s_type;


/* This special parameter captures any unrecognized/proprietary parameters
*/
typedef struct wms_other_parm_s
{
  uint8                         input_other_len;
  uint8                         desired_other_len; /* used during decoding */
  uint8                         * other_data;
  /*~ FIELD wms_other_parm_s.other_data VARRAY LENGTH wms_other_parm_s.input_other_len*/
} wms_other_parm_s_type;


/* CDMA client message structure
*/
typedef struct wms_cdma_message_s
{
  boolean                      is_mo; /* TRUE: MO msg; FALSE: MT msg */
  wms_teleservice_e_type       teleservice;
    /* If not available, will be set to "Unknown"
    */

  /* For incoming messages, the address is the origination address;
     otherwise, it is the destination address.
     NOTE: Mandatory in both incoming and outgoing messages.
  */
  wms_address_s_type           address;

  /* Optional: If subaddress.number_of_digits == 0, this field is not present.
  */
  wms_subaddress_s_type        subaddress;

  /* Indicates if the client needs to confirm whether the message is received
     successfully or not.
  */
  boolean                      is_tl_ack_requested;

  boolean                      is_service_present;
  wms_service_e_type           service;

  /* Raw (un-decoded) bearer data:
  */
  wms_raw_ts_data_s_type       raw_ts;
  
  // 添加下列字段以便非正常长消息的串接
#ifdef CUST_EDITION  
  wms_udh_concat_8_s_type      concat_8;
#endif /*CUST_EDITION*/
} wms_cdma_message_s_type;



/* Bearer data subparameter mask values:
*/
enum{ WMS_MASK_BD_NULL             =   0x00000000 };
enum{ WMS_MASK_BD_MSG_ID           =   0x00000001 };
enum{ WMS_MASK_BD_USER_DATA        =   0x00000002 };
enum{ WMS_MASK_BD_USER_RESP        =   0x00000004 };
enum{ WMS_MASK_BD_MC_TIME          =   0x00000008 };
enum{ WMS_MASK_BD_VALID_ABS        =   0x00000010 };
enum{ WMS_MASK_BD_VALID_REL        =   0x00000020 };
enum{ WMS_MASK_BD_DEFER_ABS        =   0x00000040 };
enum{ WMS_MASK_BD_DEFER_REL        =   0x00000080 };
enum{ WMS_MASK_BD_PRIORITY         =   0x00000100 };
enum{ WMS_MASK_BD_PRIVACY          =   0x00000200 };
enum{ WMS_MASK_BD_REPLY_OPTION     =   0x00000400 };
enum{ WMS_MASK_BD_NUM_OF_MSGS      =   0x00000800 };
enum{ WMS_MASK_BD_ALERT            =   0x00001000 };
enum{ WMS_MASK_BD_LANGUAGE         =   0x00002000 };
enum{ WMS_MASK_BD_CALLBACK         =   0x00004000 };
enum{ WMS_MASK_BD_DISPLAY_MODE     =   0x00008000 };
enum{ WMS_MASK_BD_SCPT_DATA        =   0x00010000 };
enum{ WMS_MASK_BD_SCPT_RESULT      =   0x00020000 };
enum{ WMS_MASK_BD_DEPOSIT_INDEX    =   0x00040000 };
enum{ WMS_MASK_BD_DELIVERY_STATUS  =   0x00080000 };
enum{ WMS_MASK_BD_IP_ADDRESS       =   0x10000000 };
enum{ WMS_MASK_BD_RSN_NO_NOTIFY    =   0x20000000 };
enum{ WMS_MASK_BD_OTHER            =   0x40000000 };



/* ---------------------------------- */
/* ---------- Bearer Data struct ---- */
/* ---------------------------------- */
typedef struct wms_client_bd_s
{
  /* the mask indicates which fields are present in this message */
  uint32                        mask;

  wms_message_id_s_type         message_id;
  wms_cdma_user_data_s_type     user_data;
  wms_user_response_type        user_response;
  wms_timestamp_s_type          mc_time;
  wms_timestamp_s_type          validity_absolute;
  wms_timestamp_s_type          validity_relative;
  wms_timestamp_s_type          deferred_absolute;
  wms_timestamp_s_type          deferred_relative;
  wms_priority_e_type           priority;
  wms_privacy_e_type            privacy;
  wms_reply_option_s_type       reply_option;
  uint8                         num_messages;  /* the actual value; not BCDs */
  wms_alert_mode_e_type         alert_mode;
     /* For pre-IS-637A implementations, alert_mode is either Off or On. */
  wms_language_e_type           language;
  wms_address_s_type            callback;
  wms_display_mode_e_type       display_mode;

  wms_download_mode_e_type      download_mode;

  wms_delivery_status_s_type    delivery_status;
  uint32                        deposit_index;

  /* NOTE: All SCPT related definitions are for internal use only!
  */
  wms_bc_scpt_data_s_type       * scpt_data_ptr;
  /*~ FIELD wms_client_bd_s.scpt_data_ptr VARRAY LENGTH 0 */

  wms_bc_scpt_result_s_type     * scpt_result_ptr;
  /*~ FIELD wms_client_bd_s.scpt_result_ptr VARRAY LENGTH 0 */

  wms_ip_address_s_type         ip_address;
  uint8                         rsn_no_notify;

  /* See function comments of wms_ts_decode() and
  ** wms_ts_decode_cdma_bd_with_other() for details regarding 'other' parameters
  */
  wms_other_parm_s_type         other;

} wms_client_bd_s_type;


/* CDMA template masks (Parameter Indicators byte 5 and 6) as in 3GPP2 C.S0023-D
*/
enum
{
  WMS_CDMA_TEMPLATE_MASK_ORIG_ADDR             = 0x0100, /* Not used */
  WMS_CDMA_TEMPLATE_MASK_DEST_ADDR             = 0x0200,
  WMS_CDMA_TEMPLATE_MASK_RESERVED_4            = 0x0400, /* Not used */
  WMS_CDMA_TEMPLATE_MASK_DCS                   = 0x0800,
  WMS_CDMA_TEMPLATE_MASK_VALIDITY              = 0x1000,
  WMS_CDMA_TEMPLATE_MASK_SERVICE               = 0x2000,
  WMS_CDMA_TEMPLATE_MASK_ORIG_SUBADDR          = 0x4000, /* Not used */
  WMS_CDMA_TEMPLATE_MASK_DEST_SUBADDR          = 0x8000,

  WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION   = 0x0001,
  WMS_CDMA_TEMPLATE_MASK_BEARER_DATA           = 0x0002
};


/* CDMA template structure
*/
typedef struct wms_cdma_template_s
{
  wms_teleservice_e_type          teleservice;

  uint16                          mask;
    /* indicates which of the following fields are present */

  wms_address_s_type              address;
  wms_subaddress_s_type           subaddress;
  wms_user_data_encoding_e_type   dcs;
  wms_timestamp_s_type            relative_validity;
  wms_service_e_type              service;
  wms_client_bd_s_type            client_bd;

  /* NOTE: If WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION is set in the mask,
  ** there is no corresponding field for it in this structure.
  */
} wms_cdma_template_s_type;


/* CDMA Ack info
*/
typedef struct wms_cdma_ack_info_s
{
  wms_error_class_e_type      error_class;
  wms_cdma_tl_status_e_type   tl_status;
} wms_cdma_ack_info_s_type;




/* ====================== GSM/WCDMA message definitions ==================== */


/* GW Data Coding Scheme
*/
typedef struct wms_gw_dcs_s
{
  wms_message_class_e_type          msg_class;
  boolean                           is_compressed;
  wms_gw_alphabet_e_type            alphabet;
  wms_gw_msg_waiting_e_type         msg_waiting;
  boolean                           msg_waiting_active;
  wms_gw_msg_waiting_kind_e_type    msg_waiting_kind;
  /* Raw DCS Byte */
  uint8                             raw_dcs_data;
} wms_gw_dcs_s_type;



/* GW Validity info
*/
typedef struct wms_gw_validity_s
{
  wms_gw_validity_format_e_type  format;
  union wms_gw_validity_u
  {
    wms_timestamp_s_type    time;
    /*~ IF (_DISC_ == WMS_GW_VALIDITY_RELATIVE || _DISC_ == WMS_GW_VALIDITY_ABSOLUTE) wms_gw_validity_u.time */
      /* used by both absolute and relative formats */
    // TBD: enhanced format

    /*~ DEFAULT wms_gw_validity_u.void */
  } u;
  /*~ FIELD wms_gw_validity_s.u DISC _OBJ_.format */
} wms_gw_validity_s_type;



/* GW user data type
*/
typedef struct wms_gw_user_data_s
{
  uint8                          num_headers;
  wms_udh_s_type                 headers[WMS_MAX_UD_HEADERS];

  uint16                         sm_len;
  uint8                          sm_data[WMS_MAX_LEN];
    /* If DCS indicates the default 7-bit alphabet, each byte holds one character
    ** with bit 7 equal to 0 and sm_len indicates the number of characters;
    ** If DCS indicates otherwise, each character may occupy multiple bytes
    ** and sm_len indicates the total number of bytes.
    */
} wms_gw_user_data_s_type;



/* Deliver TPDU
*/
typedef struct wms_gw_deliver_s
{
  boolean                          more;                     /* TP-MMS */
  boolean                          reply_path_present;       /* TP-RP */
  boolean                          user_data_header_present; /* TP-UDHI */
  boolean                          status_report_enabled;    /* TP-SRI */
  wms_address_s_type               address;                  /* TP-OA */
  wms_pid_e_type                   pid;                      /* TP-PID */
  wms_gw_dcs_s_type                dcs;                      /* TP-DCS */
  wms_timestamp_s_type             timestamp;                /* TP-SCTS */
  wms_gw_user_data_s_type          user_data;                /* TP-UD */
} wms_gw_deliver_s_type;


/* Deliver report with ack TPDU
*/
typedef struct wms_gw_deliver_report_ack_s
{
  boolean                         user_data_header_present; /* TP-UDHI */

  /* the mask indicates which of the optional fields are present
  */
  uint32                          mask;                     /* TP-PI */
  wms_pid_e_type                  pid;                      /* TP-PID */
  wms_gw_dcs_s_type               dcs;                      /* TP-DCS */
  wms_gw_user_data_s_type         user_data;                /* TP-UD */
} wms_gw_deliver_report_ack_s_type;


/* Deliver report with error TPDU
*/
typedef struct wms_gw_deliver_report_error_s
{
  boolean                         user_data_header_present; /* TP-UDHI */
  wms_tp_cause_e_type             tp_cause;                 /* TP-FCS */

  /* the mask indicates which of the optional fields are present
  */
  uint32                          mask;                     /* TP-PI */
  wms_pid_e_type                  pid;                      /* TP-PID */
  wms_gw_dcs_s_type               dcs;                      /* TP-DCS */
  wms_gw_user_data_s_type         user_data;                /* TP-UD */
} wms_gw_deliver_report_error_s_type;


/* Submit TPDU
*/
typedef struct wms_gw_submit_s
{
  boolean                           reject_duplicates;        /* TP-RD */
  boolean                           reply_path_present;       /* TP-RP */
  boolean                           user_data_header_present; /* TP-UDHI */
  boolean                           status_report_enabled;    /* TP-SRR */
  wms_message_number_type           message_reference;        /* TP-MR */
  wms_address_s_type                address;                  /* TP-DA */
  wms_pid_e_type                    pid;                      /* TP-PID */
  wms_gw_dcs_s_type                 dcs;                      /* TP-DCS */
  wms_gw_validity_s_type            validity;                 /* TP-VPF & TP-VP */
  wms_gw_user_data_s_type           user_data;                /* TP-UD */
} wms_gw_submit_s_type;


/* TPDU parameter bit masks
*/
enum
{
  WMS_TPDU_MASK_PID         = 0x0001,
  WMS_TPDU_MASK_DCS         = 0x0002,
  WMS_TPDU_MASK_USER_DATA   = 0x0004
};

/* Submit report with ack TPDU
*/
typedef struct wms_gw_submit_report_ack_s
{
  boolean                        user_data_header_present; /* TP-UDHI */
  wms_timestamp_s_type           timestamp;                /* TP-SCTS */

  /* the mask indicates which of the optional fields are present
  */
  uint32                         mask;                     /* TP-PI */
  wms_pid_e_type                 pid;                      /* TP-PID */
  wms_gw_dcs_s_type              dcs;                      /* TP-DCS */
  wms_gw_user_data_s_type        user_data;                /* TP-UD */
} wms_gw_submit_report_ack_s_type;


/* Submit report with error TPDU
*/
typedef struct wms_gw_submit_report_error_s
{
  boolean                       user_data_header_present; /* TP-UDHI */
  wms_tp_cause_e_type           tp_cause;                 /* TP-FCS */
  wms_timestamp_s_type          timestamp;                /* TP-SCTS */

  /* the mask indicates which of the optional fields are present
  */
  uint32                        mask;                     /* TP-PI */
  wms_pid_e_type                pid;                      /* TP-PID */
  wms_gw_dcs_s_type             dcs;                      /* TP-DCS */
  wms_gw_user_data_s_type       user_data;                /* TP-UD */
} wms_gw_submit_report_error_s_type;


/* Status report TPDU
*/
typedef struct wms_gw_status_report_s
{
  boolean                       user_data_header_present; /* TP-UDHI */
  boolean                       more;                     /* TP-MMS */
  boolean                       status_report_qualifier;  /* TP-SRQ */
  wms_message_number_type       message_reference;        /* TP-MR */
  wms_address_s_type            address;                  /* TP-RA */
  wms_timestamp_s_type          timestamp;                /* TP-SCTS */
  wms_timestamp_s_type          discharge_time;           /* TP-DT */
  wms_tp_status_e_type          tp_status;                /* TP-ST */

  /* the mask indicates which of the optional fields are present
  */
  uint32                        mask;                     /* TP-PI */
  wms_pid_e_type                pid;                      /* TP-PID */
  wms_gw_dcs_s_type             dcs;                      /* TP-DCS */
  wms_gw_user_data_s_type       user_data;                /* TP-UD */
} wms_gw_status_report_s_type;


/* Command TPDU
*/
typedef struct wms_gw_command_s
{
  boolean                      user_data_header_present; /* TP-UDHI */
  boolean                      status_report_enabled;    /* TP-SRR */
  wms_message_number_type      message_reference;        /* TP-MR */
  wms_pid_e_type               pid;                      /* TP-PID */
  wms_gw_command_e_type        command;                  /* TP-CT */
  wms_message_number_type      message_number;           /* TP-MN */
  wms_address_s_type           address;                  /* TP-DA */
  uint8                        command_data_len;         /* TP-CDL */
  uint8                        command_data[WMS_GW_COMMAND_DATA_MAX]; /* TP-CD */
} wms_gw_command_s_type;



/* GW template masks as in TS 31.102 (SMS parameters)
*/
enum
{
  WMS_GW_TEMPLATE_MASK_DEST_ADDR = 0x01,
  WMS_GW_TEMPLATE_MASK_SC_ADDR   = 0x02,
  WMS_GW_TEMPLATE_MASK_PID       = 0x04,
  WMS_GW_TEMPLATE_MASK_DCS       = 0x08,
  WMS_GW_TEMPLATE_MASK_VALIDITY  = 0x10
};

/* Alpha ID
*/
typedef struct wms_alpha_id_s
{
  uint8   len;
  uint8   *data;
} wms_alpha_id_s_type;
/*~ FIELD wms_alpha_id_s.data VARRAY LENGTH wms_alpha_id_s.len */


typedef struct wms_gw_template_s
{
  wms_alpha_id_s_type          alpha_id;

  uint8                        mask;
    /* indicates which of the following fields are present */

  wms_address_s_type           dest_addr;
  wms_address_s_type           sc_addr; /* service center address */
  wms_pid_e_type               pid; /* protocol ID */
  wms_gw_dcs_s_type            dcs; /* data coding scheme */
  wms_timestamp_s_type         relative_validity;

} wms_gw_template_s_type;


/* GW message
*/
typedef struct wms_gw_message_s
{
  boolean                       is_broadcast;
  wms_address_s_type            sc_address;
  wms_raw_ts_data_s_type        raw_ts_data;
} wms_gw_message_s_type;


/* GW ack information
*/
typedef struct wms_gw_ack_info_s
{
  boolean                              success;
  wms_rp_cause_e_type                  rp_cause; /* used when success==FALSE */

  wms_gw_tpdu_type_e_type              tpdu_type;
    /* Use WMS_TPDU_NONE if deliver_report TPDUs are not included */

  union wms_gw_ack_info_u
  {
    wms_gw_deliver_report_ack_s_type    ack;    /* if success==TRUE  */
    /*~ IF (_DISC_ == TRUE) wms_gw_ack_info_u.ack */
    wms_gw_deliver_report_error_s_type  error;  /* if success==FALSE */
    /*~ IF (_DISC_ == FALSE) wms_gw_ack_info_u.error */

    /*~ DEFAULT wms_gw_ack_info_u.void */
  } u;
   /*~ FIELD wms_gw_ack_info_s.u DISC _OBJ_.success */
} wms_gw_ack_info_s_type;

#ifdef CUST_EDITION
/* AUTOMATIC memory stores:
** (1) distinguishes which mem_store to store the message when sending.
** (2) to be used in conjunction with the delivered message mode
**     to distinguish which memory store the message resides.
**     ie. WMS_AUTO_MEMORY_STORE_CARD && WMS_MESSAGE_MODE_CDMA ==
**         WMS_MEMORY_STORE_RUIM
** (3) WMS_AUTO_MEMORY_STORE_NONE: WMS will not attempt to store the message
**     after delivery.
** NOTE: Heirarchy of memory stores. ->CARD->PHONE->RAM->
**       ie. If attempt to store to the CARD fails, an additional attempt to
**           store to the PHONE memory will be made, and so on.
*/

typedef enum
{
  WMS_AUTO_MEMORY_STORE_NONE = 0, /* No storage is required */
  WMS_AUTO_MEMORY_STORE_PHONE,    /* NV */
  WMS_AUTO_MEMORY_STORE_CARD,     /* SIM, RUIM */
  WMS_AUTO_MEMORY_STORE_TEMP,     /* RAM */
  WMS_AUTO_MEMORY_STORE_MAX,
  WMS_AUTO_MEMORY_STORE_MAX32 = 0x10000000

} wms_auto_memory_store_e_type;

/* AUTOMATIC address
*/
typedef struct wms_auto_address_s
{
  wms_address_s_type            address;          /* Mandatory */
    /* Note: address.digits will be 8-bit Ascii representation.
    */

  uint8                         email_length;
  uint16                        email_address[WMS_ADDRESS_MAX*2];  /* Unicode */
    /* Email address is OPTIONAL.  This allows for email addresses to be
    ** specified if it should not standardly appear in the above address struct.
    */

  wms_address_s_type            reply_sc_address;
    /* reply_sc_address_ptr should be specified if replying to a GW MT message
    ** where the reply_sc_address will be provided.
    */

} wms_auto_address_s_type;

/* AUTOMATIC message
*/
typedef struct wms_auto_message_s
{
  wms_auto_address_s_type         dest_addr;        /* Destination Address */
  wms_auto_memory_store_e_type    mem_store;
  uint8                           num_of_headers;
  wms_udh_s_type                  headers[WMS_MAX_UD_HEADERS];
  uint16                          user_data[WMS_AUTO_USER_DATA_MAX];/* Unicode*/
  uint8                           user_data_length; /* num of uint16 chars */
  boolean                         reply_request;
  boolean                         status_request;

} wms_auto_message_s_type;

/* wms_auto_submit_report_info_s_type will be part of the submit report info and
** should be used when the message_mode in submit_report_info_s is AUTOMATIC.
*/
typedef struct wms_auto_submit_report_info_s
{
  wms_message_mode_e_type         submit_message_mode;
    /* The mode of which the MO message was delivered.
    */

  wms_message_number_type         message_id;
    /* CDMA = id_number
    ** GW   = reference_number
    */

  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
    /* Storage details after the message is sent.
    */

  wms_message_tag_e_type          tag;
    /* SENT     = sent OK
    ** NOT_SENT = send failure
    */

} wms_auto_submit_report_info_s_type;
#endif /*CUST_EDITION*/
/* ============ Client message definitions ========== */

/* Header
*/
typedef struct wms_client_msg_hdr_s
{
  wms_message_mode_e_type   message_mode;

  wms_message_tag_e_type    tag;
    /* Set to WMS_TAG_NONE if not applicable
    */

  wms_memory_store_e_type   mem_store;
    /* indicates the memory store used for storing this message;
    ** WMS_MEMORY_STORE_NONE means this message is not stored
    */

  wms_message_index_type    index;
    /* indicates the index for a stored message;
    ** set to WMS_DUMMY_MESSAGE_INDEX when mem_store==WMS_MEMORY_STORE_NONE
    */

} wms_client_msg_hdr_s_type;


/* Information about a message of all types: MT, MO, regular message,
** report, template, CB message, etc.
*/
typedef struct wms_client_message_s
{
  wms_client_msg_hdr_s_type     msg_hdr;

  /* message_mode and tag together decides which structure to use in the
  ** union below.
  */
  union wms_client_message_u
  {
    wms_cdma_message_s_type      cdma_message;
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_CDMA &&
          _DISC_.tag != WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.cdma_message*/

    wms_cdma_template_s_type     cdma_template;
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_CDMA &&
          _DISC_.tag == WMS_TAG_MO_TEMPLATE)
       wms_client_message_u.cdma_template*/

    wms_gw_message_s_type        gw_message;
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_GW &&
          _DISC_.tag != WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.gw_message*/

    wms_gw_template_s_type       gw_template;
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_GW &&
          _DISC_.tag == WMS_TAG_MO_TEMPLATE)
        wms_client_message_u.gw_template*/
#ifdef CUST_EDITION
    wms_auto_message_s_type      auto_message;
  /*~ IF ( _DISC_.message_mode == WMS_MESSAGE_MODE_AUTO)
        wms_client_message_u.auto_message*/
#endif /*CUST_EDITION*/
    /*~ DEFAULT wms_client_message_u.void */

   } u; /*~ FIELD wms_client_message_s.u DISC _OBJ_.msg_hdr */

} wms_client_message_s_type;




/* ============ Message event definitions =========== */

/* Status information of a message that was sent/stored
*/
typedef struct wms_message_status_info_s
{
  void                        *user_data;
    /* the user data pointer passed in the original command to WMS
    */

  wms_client_type_e_type      client_id;
    /* Indicates which client issued the original command to WMS
    */

  wms_send_mode_e_type        send_mode;
    /* valid if event is WMS_MSG_EVENT_SEND
    */

  wms_write_mode_e_type       write_mode;
    /* valid if event is WMS_MSG_EVENT_WRITE
    */

  wms_client_message_s_type   message;
    /* The message/template/status-report that is operated on
    */

  wms_alpha_id_s_type         alpha_id;
    /* valid if event is WMS_MSG_EVENT_SEND
    */

} wms_message_status_info_s_type;


/* Information of a message received from the network
*/
typedef struct wms_mt_message_info_s
{
  wms_client_type_e_type         client_id;
    /* If the client id is set to WMS_CLIENT_TYPE_MAX, then this message
    ** was broadcasted to all clients, otherwise the message was delivered to
    ** a single client of this particular client_id.
    */

  wms_route_e_type               route;
    /* Only in the case of TRANSFER_ONLY, the client needs to ack the message.
    */

  wms_transaction_id_type        transaction_id;
    /* to differentiate among multiple incoming messages
    */

  wms_client_message_s_type      message;

} wms_mt_message_info_s_type;


/* Information of error of a message received from the network
*/
typedef struct wms_mt_message_error_s
{
  wms_transaction_id_type        transaction_id;
  wms_report_status_e_type       report_status;

} wms_mt_message_error_s_type;


/* Cause info in the submit report
*/
typedef struct wms_cause_info_s
{
  /* Applicable to CDMA only:
  */
  wms_error_class_e_type        error_class;
  wms_cdma_tl_status_e_type     tl_status;

  /* Applicable to GW only:
  */
  wms_rp_cause_e_type           cause_value;
  boolean                       diagnostics_present;
  uint8                         diagnostics;

} wms_cause_info_s_type;


/* Submit report
*/
typedef struct wms_submit_report_info_s
{
  void                        *user_data;
  wms_client_type_e_type      client_id;
  wms_report_status_e_type    report_status;
  wms_message_mode_e_type     message_mode;
  wms_cause_info_s_type       cause_info;
  wms_gw_tpdu_type_e_type     tpdu_type;
  wms_alpha_id_s_type         alpha_id;
#ifdef CUST_EDITION  
  wms_auto_submit_report_info_s_type  auto_rpt_info;
#endif /*CUST_EDITION*/  
  union wms_submit_report_info_u
  {
    /* Applicable to GW only:
    */
    wms_gw_submit_report_ack_s_type    ack;
    /*~ CASE WMS_RPT_OK wms_submit_report_info_u.ack */
    wms_gw_submit_report_error_s_type  error;
    /*~ DEFAULT wms_submit_report_info_u.error */

  } u; /*~ FIELD wms_submit_report_info_s.u DISC _OBJ_.report_status */

} wms_submit_report_info_s_type;



/* Ack information
*/
typedef struct wms_ack_info_s
{
  wms_transaction_id_type          transaction_id;
  wms_message_mode_e_type          message_mode;

  union wms_ack_info_u
  {
    wms_cdma_ack_info_s_type       cdma;
     /*~ CASE WMS_MESSAGE_MODE_CDMA wms_ack_info_u.cdma */
    wms_gw_ack_info_s_type         gw;
    /*~ CASE WMS_MESSAGE_MODE_GW wms_ack_info_u.gw */
    /* TBD: check default mode */

    /*~ DEFAULT wms_ack_info_u.void */

  } u; /*~ FIELD wms_ack_info_s.u DISC _OBJ_.message_mode */

} wms_ack_info_s_type;


/* Ack Report information
*/
typedef struct
{
  wms_client_type_e_type           client_id;
  wms_transaction_id_type          transaction_id;
  wms_message_mode_e_type          message_mode;
  void                             *user_data;
  boolean                          is_success;
} wms_ack_report_info_s_type;

/* Message event types
*/
typedef enum
{
  /* Command related events
  */
  WMS_MSG_EVENT_SEND = 0,
  WMS_MSG_EVENT_ACK,
  WMS_MSG_EVENT_READ,
  WMS_MSG_EVENT_WRITE,
  WMS_MSG_EVENT_DELETE,
  WMS_MSG_EVENT_DELETE_ALL,
  WMS_MSG_EVENT_MODIFY_TAG,
  WMS_MSG_EVENT_READ_TEMPLATE,
  WMS_MSG_EVENT_WRITE_TEMPLATE,
  WMS_MSG_EVENT_DELETE_TEMPLATE,
  WMS_MSG_EVENT_DELETE_TEMPLATE_ALL,
  WMS_MSG_EVENT_READ_STS_REPORT,
  WMS_MSG_EVENT_WRITE_STS_REPORT,
  WMS_MSG_EVENT_DELETE_STS_REPORT,
  WMS_MSG_EVENT_DELETE_STS_REPORT_ALL,

  /* Events from lower layer
  */
  WMS_MSG_EVENT_RECEIVED_MESSAGE,
  WMS_MSG_EVENT_SUBMIT_REPORT,
  WMS_MSG_EVENT_STATUS_REPORT,
  WMS_MSG_EVENT_MT_MESSAGE_ERROR, /* for example, MT msg ack timeout */
  WMS_MSG_EVENT_ACK_REPORT,

  WMS_MSG_EVENT_DUPLICATE_CB_PAGE,

  WMS_MSG_EVENT_MAX,
  WMS_MSG_EVENT_MAX32 = 0x10000000
} wms_msg_event_e_type;



/* Message event information for each event
*/
typedef union wms_msg_event_info_u
{
  wms_message_status_info_s_type     status_info;
  /*~ DEFAULT wms_msg_event_info_u.status_info */
    /* WMS_MSG_EVENT_SEND
    ** WMS_MSG_EVENT_READ
    ** WMS_MSG_EVENT_WRITE
    ** WMS_MSG_EVENT_DELETE
    ** WMS_MSG_EVENT_DELETE_ALL
    ** WMS_MSG_EVENT_MODIFY_TAG
    ** WMS_MSG_EVENT_READ_TEMPLATE
    ** WMS_MSG_EVENT_WRITE_TEMPLATE
    ** WMS_MSG_EVENT_DELETE_TEMPLATE
    ** WMS_MSG_EVENT_DELETE_TEMPLATE_ALL
    ** WMS_MSG_EVENT_READ_STS_REPORT
    ** WMS_MSG_EVENT_WRITE_STS_REPORT
    ** WMS_MSG_EVENT_DELETE_STS_REPORT
    ** WMS_MSG_EVENT_DELETE_STS_REPORT_ALL
    */

  wms_ack_info_s_type                ack_info;
    /* WMS_MSG_EVENT_ACK
    */
 /*~ CASE WMS_MSG_EVENT_ACK wms_msg_event_info_u.ack_info */

  wms_mt_message_info_s_type         mt_message_info;
    /* WMS_MSG_EVENT_RECEIVED_MESSAGE
    ** WMS_MSG_EVENT_STATUS_REPORT
    */
 /*~ IF (_DISC_ == WMS_MSG_EVENT_RECEIVED_MESSAGE || _DISC_ == WMS_MSG_EVENT_STATUS_REPORT) 
     wms_msg_event_info_u.mt_message_info */
 
  wms_submit_report_info_s_type      submit_report_info;
    /* WMS_MSG_EVENT_SUBMIT_REPORT
    */
 /*~ CASE WMS_MSG_EVENT_SUBMIT_REPORT wms_msg_event_info_u.submit_report_info */

  wms_mt_message_error_s_type        mt_message_error;
    /* WMS_MSG_EVENT_MT_MESSAGE_ERROR - Applicable to GW only
    */
 /*~ CASE WMS_MSG_EVENT_MT_MESSAGE_ERROR wms_msg_event_info_u.mt_message_error */

  wms_ack_report_info_s_type         ack_report_info;
    /* WMS_MSG_EVENT_ACK_REPORT
    */
 /*~ CASE WMS_MSG_EVENT_ACK_REPORT wms_msg_event_info_u.ack_report_info */

  wms_gw_cb_page_header_s_type       cb_page_header;
    /* WMS_MSG_EVENT_DUPLICATE_CB_PAGE
    */
 /*~ CASE WMS_MSG_EVENT_DUPLICATE_CB_PAGE wms_msg_event_info_u.cb_page_header */

} wms_msg_event_info_s_type;


/* Message event callback
*/
typedef void (*wms_msg_event_cb_type)
(
  wms_msg_event_e_type         event,
    /* event type */
  wms_msg_event_info_s_type    *info_ptr,
    /* event information */
  boolean                      *shared
    /* whether the msg can be passed to other clients or not */
);
/*~ PARAM IN info_ptr POINTER DISC event*/
/*~ PARAM INOUT shared   POINTER */
/*~ CALLBACK wms_msg_event_cb_type */
/*~ ONERROR return  */


/* Message info cache callback
*/

#define WMS_MSG_INFO_CACHE_SIZE    30
#ifdef CUST_EDITION
// Add compile condition FEATURE_CDSMS_CACHE_USELIST
#ifdef FEATURE_CDSMS_CACHE_USELIST
typedef void (*wms_msg_info_cache_cb_type)(const wms_client_message_s_type * msg_ptr, wms_message_tag_e_type delcachetag);
#else
typedef void (*wms_msg_info_cache_cb_type)
(
  const wms_client_message_s_type         * msg_ptr,  /* INPUT from WMS */
  uint8                                   * cache_ptr /* OUTPUT from client */
);
/*~ PARAM IN msg_ptr     POINTER */
/*~ PARAM INOUT cache_ptr ARRAY WMS_MSG_INFO_CACHE_SIZE */ 
/*~ CALLBACK wms_msg_info_cache_cb_type */
/*~ ONERROR return  */
#endif
#endif /*CUST_EDITION*/

/* Specific parsing criteria provided by the client.
 * If a client registers this callback to WMS, this callback will be called
 * each time there is a MT message. The client should examine the message
 * pointed to by msg_ptr, and return TRUE or FALSE in 'shared'.
 *
 * TRUE: The client does not want to consume the message. WMS will continue with
 *       normal processing.
 * FALSE: The client wants to consume the message. WMS will deliver the message
 *        only to this client, using route TRANSFER_ONLY. The client is
 *        responsible for SMS Ack.
*/
typedef void (*wms_cl_parse_msg_cb_type)
(
  const wms_client_message_s_type         *msg_ptr,
  boolean                                 *shared
);
/*~ PARAM IN msg_ptr  POINTER */
/*~ PARAM INOUT shared   POINTER */
/*~ CALLBACK wms_cl_parse_msg_cb_type */
/*~ ONERROR return  */



/* <EJECT> */
/* ========================================================================= */
/* =========================== DC group data types ========================= */
/* ========================================================================= */


typedef enum
{
  WMS_DC_EVENT_INCOMING   = 0,
  WMS_DC_EVENT_CONNECTED,
  WMS_DC_EVENT_ABORTED,
  WMS_DC_EVENT_DISCONNECTED,
  WMS_DC_EVENT_CONNECTING,
  WMS_DC_EVENT_ENABLE_AUTO_DISCONNECT,
  WMS_DC_EVENT_DISABLE_AUTO_DISCONNECT,
  WMS_DC_EVENT_MAX,
  WMS_DC_EVENT_MAX32 = 0x10000000

} wms_dc_event_e_type;



typedef struct wms_dc_event_info_s
{
  wms_dc_so_e_type    so;
  uint32              idle_timeout;
} wms_dc_event_info_s_type;


/* DC event callback
*/
typedef void (*wms_dc_event_cb_type)
(
  wms_dc_event_e_type         event,
  wms_dc_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER */
/*~ CALLBACK wms_dc_event_cb_type */
/*~ ONERROR return  */


/* ====================================================================== */
/* ---------------------------------------------------------------------- */
/* ----------------- Multimode Broadcast definitions: ------------------- */
/* ---------------------------------------------------------------------- */
/* ====================================================================== */
#define WMS_BC_MM_TABLE_MAX        190

/* ---------- data definitions ------------ */

/* Carrier's configuration
*/
typedef enum
{
  WMS_BC_CONFIG_DISALLOW,
  WMS_BC_CONFIG_ALLOW_TABLE,
  WMS_BC_CONFIG_ALLOW_ALL,   /* CDMA only */
  WMS_BC_CONFIG_MAX32 = 0x10000000

} wms_bc_config_e_type;


/* User preference
*/
typedef enum
{
  WMS_BC_PREF_DEACTIVATE,
  WMS_BC_PREF_ACTIVATE_TABLE, /* For GW CB, use language prefs */
  WMS_BC_PREF_ACTIVATE_ALL,   /* For GW CB, ignore language prefs */
  WMS_BC_PREF_MAX,
  WMS_BC_PREF_MAX32 = 0x10000000

} wms_bc_pref_e_type;
#ifdef CUST_EDITION


/* Service info, which is a entry in the service table
*/
typedef struct wms_bc_service_info_s
{
  wms_bc_service_id_s_type    srv_id;
  boolean                     selected;
  wms_priority_e_type         priority;
  char                        label[WMS_BC_SRV_LABEL_SIZE];

  wms_user_data_encoding_e_type   label_encoding;
    /* Encoding type for the service label.
    ** If it is ASCII or IA5, each byte of 'label' has one character;
    ** otherwise, 'label' has the raw bytes to be decoded by the clients.
    */
  wms_bc_alert_e_type             alert;
    /* Alert options for this broadcast service.
    */
  uint8                            max_messages;
    /* Max number of messages that can be stored for this service.
    */

} wms_bc_service_info_s_type;


/* the whole service table
*/
typedef struct wms_bc_table_s
{
  uint16                        size;
  wms_bc_service_info_s_type    entries[WMS_BC_TABLE_MAX];
} wms_bc_table_s_type;


/* all service IDs in the service table
*/
typedef struct wms_bc_service_ids_s
{
  uint16                       size;
  wms_bc_service_id_s_type     entries[WMS_BC_TABLE_MAX];
} wms_bc_service_ids_s_type;




/* ---------------- */
/* ---- Events ---- */
/* ---------------- */
typedef enum
{
  WMS_BC_EVENT_CONFIG,
  WMS_BC_EVENT_PREF,
  WMS_BC_EVENT_TABLE,
  WMS_BC_EVENT_SRV_IDS,
  WMS_BC_EVENT_SRV_INFO,
  WMS_BC_EVENT_SRV_ADDED,
  WMS_BC_EVENT_SRV_DELETED,
  WMS_BC_EVENT_SRV_UPDATED,
  WMS_BC_EVENT_ALL_SRV_DELETED, /* no event data needed */
  WMS_BC_EVENT_ENABLE_FAILURE,  /* lower layer (CM & CP) failure */
  WMS_BC_EVENT_DISABLE_FAILURE, /* lower layer (CM & CP) failure */

  WMS_BC_EVENT_MAX,
  WMS_BC_EVENT_MAX32 = 0x10000000
} wms_bc_event_e_type;


/* event info
*/
typedef union wms_bc_event_info_u
{
  wms_bc_config_e_type         bc_config;
    /* WMS_BC_EVENT_CONFIG */
  /*~ IF (_DISC_ == WMS_BC_EVENT_CONFIG) wms_bc_event_info_u.bc_config */

  wms_bc_pref_e_type           bc_pref;
    /* WMS_BC_EVENT_PREF */
  /*~ IF (_DISC_ == WMS_BC_EVENT_PREF) wms_bc_event_info_u.bc_pref */

  wms_bc_table_s_type          bc_table;
    /* WMS_BC_EVENT_TABLE */
  /*~ IF (_DISC_ == WMS_BC_EVENT_TABLE) wms_bc_event_info_u.bc_table */

  wms_bc_service_ids_s_type    bc_srv_ids;
    /* WMS_BC_EVENT_SRV_IDS */
  /*~ IF (_DISC_ == WMS_BC_EVENT_SRV_IDS) wms_bc_event_info_u.bc_srv_ids */

  wms_bc_service_info_s_type   bc_srv_info;
    /* WMS_BC_EVENT_SRV_INFO */
    /* WMS_BC_EVENT_SRV_ADDED */
    /* WMS_BC_EVENT_SRV_UPDATED */
  /*~ IF (_DISC_ == WMS_BC_EVENT_SRV_INFO || _DISC_ == WMS_BC_EVENT_SRV_ADDED
         || _DISC_ == WMS_BC_EVENT_SRV_UPDATED) wms_bc_event_info_u.bc_srv_info */

  wms_bc_service_id_s_type     bc_deleted_srv;
    /* WMS_BC_EVENT_SRV_DELETED */
  /*~ IF (_DISC_ == WMS_BC_EVENT_SRV_DELETED) wms_bc_event_info_u.bc_deleted_srv */

  /* no event data for WMS_BC_EVENT_ALL_SRV_DELETED */

  wms_status_e_type            bc_enable_error;
    /* WMS_BC_EVENT_ENABLE_FAILURE */
    /* WMS_BC_EVENT_DISABLE_FAILURE */
  /*~ IF (_DISC_ == WMS_BC_EVENT_ENABLE_FAILURE || _DISC_ == WMS_BC_EVENT_DISABLE_FAILURE)
      wms_bc_event_info_u.bc_enable_error */

  /*~ DEFAULT wms_bc_event_event_info_u.void */

} wms_bc_event_info_s_type;


/* Broadcast event callback
*/
typedef void (*wms_bc_event_cb_type)
(
  wms_bc_event_e_type         event,
  wms_bc_event_info_s_type    *info_ptr
);
 /*~ PARAM IN info_ptr POINTER DISC event */


/* ====================================================================== */
/* ---------------------------------------------------------------------- */
/* ----------------- Multimode Broadcast definitions: ------------------- */
/* ---------------------------------------------------------------------- */
/* ====================================================================== */

#define WMS_BC_MM_SRV_LABEL_SIZE   30
#define WMS_BC_MM_TABLE_MAX        190
#endif /*CUST_EDITION*/

typedef struct wms_gw_cb_srv_range_s
{
  /* 'from' is <= 'to' */
  wms_gw_cb_srv_id_type    from;
  wms_gw_cb_srv_id_type    to;
} wms_gw_cb_srv_range_s_type;


/* Service ID
*/
typedef struct wms_bc_mm_srv_id_u
{
  wms_bc_service_id_s_type       bc_srv_id;
  wms_gw_cb_srv_range_s_type     gw_cb_range;
} wms_bc_mm_srv_id_type;

/* GW CB Decoded Page Data:
*/
typedef struct wms_gw_cb_ts_data_s
{
  wms_gw_cb_page_header_s_type     cb_page_hdr;
  wms_gw_user_data_s_type          user_data;
} wms_gw_cb_ts_data_s_type;


/* Multimode Broadcast Service Table Entry:
*/
typedef struct wms_bc_mm_service_info_s
{
  wms_bc_mm_srv_id_type           srv_id;

  boolean                         selected;
  wms_priority_e_type             priority; /* For GW CB, it is always Normal */
  char                            label[WMS_BC_MM_SRV_LABEL_SIZE];

  wms_user_data_encoding_e_type   label_encoding;
    /* Encoding type for the service label.
    ** If it is ASCII or IA5, each byte of 'label' has one character;
    ** otherwise, 'label' has the raw bytes to be decoded by the clients.
    */
  wms_bc_alert_e_type             alert;
    /* Alert options for this broadcast service.
    */
  uint8                            max_messages;
    /* Max number of messages that can be stored for this service.
    */

} wms_bc_mm_service_info_s_type;


/* Multimode Broadcast Service Table:
*/
typedef struct wms_bc_mm_table_s
{
  uint16                           size;
  wms_bc_mm_service_info_s_type    * entries;
} wms_bc_mm_table_s_type;
/*~ FIELD wms_bc_mm_table_s.entries VARRAY LENGTH wms_bc_mm_table_s.size */

/* Multimode Broadcast Service IDs:
*/
typedef struct wms_bc_mm_service_ids_s
{
  uint16                       size;
  wms_bc_mm_srv_id_type        * entries;
} wms_bc_mm_service_ids_s_type;
/*~ FIELD wms_bc_mm_service_ids_s.entries VARRAY LENGTH wms_bc_mm_service_ids_s.size */

typedef struct wms_bc_mm_reading_pref_s
{
  boolean                      reading_advised;
  boolean                      reading_optional;
} wms_bc_mm_reading_pref_s_type;

/* ---------------- */
/* ---- Events ---- */
/* ---------------- */
typedef enum
{
  WMS_BC_MM_EVENT_CONFIG,
  WMS_BC_MM_EVENT_PREF,
  WMS_BC_MM_EVENT_TABLE,
  WMS_BC_MM_EVENT_SRV_IDS,
  WMS_BC_MM_EVENT_SRV_INFO,
  WMS_BC_MM_EVENT_SRV_UPDATED,
  WMS_BC_MM_EVENT_ADD_SRVS,
  WMS_BC_MM_EVENT_DELETE_SRVS,
  WMS_BC_MM_EVENT_SELECT_ALL_SRVS,
  WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS,
  WMS_BC_MM_EVENT_READING_PREF,
  WMS_BC_MM_EVENT_DELETE_ALL_SRVS, /* no event data needed */
  WMS_BC_MM_EVENT_ENABLE_FAILURE,  /* lower layer (CM & CP) failure */
  WMS_BC_MM_EVENT_DISABLE_FAILURE, /* lower layer (CM & CP) failure */

  WMS_BC_MM_EVENT_MAX,
  WMS_BC_MM_EVENT_MAX32 = 0x10000000

} wms_bc_mm_event_e_type;



/* Multimode Broadcast Event Info:
*/
typedef struct wms_bc_mm_event_info_s
{
 wms_message_mode_e_type         message_mode;
 wms_bc_mm_event_e_type          event;
 union wms_bc_mm_event_info_u
 {
  wms_bc_config_e_type            bc_config;
    /* WMS_BC_MM_EVENT_CONFIG */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_CONFIG) wms_bc_mm_event_info_u.bc_config */

  wms_bc_pref_e_type              bc_pref;
    /* WMS_BC_MM_EVENT_PREF */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_PREF) wms_bc_mm_event_info_u.bc_pref */

  wms_bc_mm_table_s_type          bc_mm_table;
    /* WMS_BC_MM_EVENT_TABLE */
    /* WMS_BC_MM_EVENT_ADD_SRVS */
    /* WMS_BC_MM_EVENT_SELECT_ALL_SRVS */
    /* WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS */
  /*~ IF ( _DISC_ == WMS_BC_MM_EVENT_TABLE || _DISC_ == WMS_BC_MM_EVENT_ADD_SRVS 
          || _DISC_ == WMS_BC_MM_EVENT_SELECT_ALL_SRVS || _DISC_ == WMS_BC_MM_EVENT_SET_PRIORITY_ALL_SRVS ) 
      wms_bc_mm_event_info_u.bc_mm_table */

  wms_bc_mm_service_ids_s_type    bc_mm_srv_ids;
    /* WMS_BC_MM_EVENT_SRV_IDS */
    /* WMS_BC_MM_EVENT_DELETE_SRVS */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_SRV_IDS || _DISC_ == WMS_BC_MM_EVENT_DELETE_SRVS)
      wms_bc_mm_event_info_u.bc_mm_srv_ids */

  wms_bc_mm_service_info_s_type   bc_mm_srv_info;
    /* WMS_BC_MM_EVENT_SRV_INFO */
    /* WMS_BC_MM_EVENT_SRV_UPDATED */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_SRV_INFO || _DISC_ == WMS_BC_MM_EVENT_SRV_UPDATED)
      wms_bc_mm_event_info_u.bc_mm_srv_info */

  /* no event data for WMS_BC_MM_EVENT_ALL_SRV_DELETED */

  wms_bc_mm_reading_pref_s_type   bc_mm_reading_pref;
    /* WMS_BC_MM_EVENT_READING_PREF */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_READING_PREF) wms_bc_mm_event_info_u.bc_mm_reading_pref */

  wms_status_e_type               bc_enable_error;
    /* WMS_BC_MM_EVENT_ENABLE_FAILURE */
    /* WMS_BC_MM_EVENT_DISABLE_FAILURE */
  /*~ IF (_DISC_ == WMS_BC_MM_EVENT_ENABLE_FAILURE || _DISC_ == WMS_BC_MM_EVENT_DISABLE_FAILURE)
      wms_bc_mm_event_info_u.bc_enable_error */

  /*~ DEFAULT wms_bc_mm_event_info_u.void */

 } u;  /*~ FIELD wms_bc_mm_event_info_s.u DISC _OBJ_.event */

} wms_bc_mm_event_info_s_type;

/* Multimode Broadcast Event Callback:
*/
typedef void (*wms_bc_mm_event_cb_type)
(
  wms_bc_mm_event_e_type         event,
  wms_bc_mm_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER */
/*~ CALLBACK wms_bc_mm_event_cb_type */
/*~ ONERROR return  */


/* <EJECT> */
/* ========================================================================= */
/* ============================= Debug group data types ==================== */
/* ========================================================================= */

/* ---------------- */
/* ---- Events ---- */
/* ---------------- */
typedef enum
{
  WMS_DBG_EVENT_RESET_TL_SEQ_NUM,
  WMS_DBG_EVENT_SET_SIM_MESSAGE_NUMBER,
  WMS_DBG_EVENT_CLEAR_SMMA_FLAG,
  WMS_DBG_EVENT_RETRY_INTERVAL,

  WMS_DBG_EVENT_MAX,
  WMS_DBG_EVENT_MAX32 = 0x10000000

} wms_dbg_event_e_type;



/* event info
*/
typedef union wms_dbg_event_info_u
{
  uint32         retry_interval;
  /* WMS_DBG_EVENT_RETRY_INTERVAL
  */
/*~ CASE WMS_DBG_EVENT_RETRY_INTERVAL wms_dbg_event_info_u.retry_interval */

/*~ DEFAULT wms_dbg_event_info_u.void */
} wms_dbg_event_info_s_type;

/* Debug event callback
*/
typedef void (*wms_dbg_event_cb_type)
(
  wms_dbg_event_e_type         event,
  wms_dbg_event_info_s_type    *info_ptr
);
/*~ PARAM IN info_ptr POINTER DISC event */
/*~ CALLBACK wms_dbg_event_cb_type */
/*~ ONERROR return  */

/* <EJECT> */
/* ========================================================================= */
/* ======================= Translation group data types ==================== */
/* ========================================================================= */


/* GW Point-to-Point TS data decoded from the raw TS data
*/
typedef struct wms_gw_pp_ts_data_s
{
  wms_gw_tpdu_type_e_type                tpdu_type;
    /* Note: this is not equivalent to TP-MTI */
  union wms_gw_pp_ts_data_u
  {
    wms_gw_deliver_s_type                deliver;
    /*~ CASE WMS_TPDU_DELIVER wms_gw_pp_ts_data_u.deliver */
    wms_gw_deliver_report_ack_s_type     deliver_report_ack;
    /*~ CASE WMS_TPDU_DELIVER_REPORT_ACK wms_gw_pp_ts_data_u.deliver_report_ack */
    wms_gw_deliver_report_error_s_type   deliver_report_error;
    /*~ CASE WMS_TPDU_DELIVER_REPORT_ERROR wms_gw_pp_ts_data_u.deliver_report_error */
    wms_gw_submit_s_type                 submit;
    /*~ CASE WMS_TPDU_SUBMIT wms_gw_pp_ts_data_u.submit */
    wms_gw_submit_report_ack_s_type      submit_report_ack;
    /*~ CASE WMS_TPDU_SUBMIT_REPORT_ACK wms_gw_pp_ts_data_u.submit_report_ack */
    wms_gw_submit_report_error_s_type    submit_report_error;
    /*~ CASE WMS_TPDU_SUBMIT_REPORT_ERROR wms_gw_pp_ts_data_u.submit_report_error */
    wms_gw_status_report_s_type          status_report;
    /*~ CASE WMS_TPDU_STATUS_REPORT wms_gw_pp_ts_data_u.status_report*/
    wms_gw_command_s_type                command;
    /*~ CASE WMS_TPDU_COMMAND wms_gw_pp_ts_data_u.command */

    /*~ DEFAULT wms_gw_pp_ts_data_u.void */

  } u; /*~ FIELD wms_gw_pp_ts_data_s.u DISC _OBJ_.tpdu_type */

} wms_gw_pp_ts_data_s_type;

/* Client TS data decoded from the raw TS data
*/
typedef struct wms_client_ts_data_s
{
  wms_format_e_type             format;

  union wms_client_ts_data_u
  {
    wms_client_bd_s_type        cdma;
      /* for both Point-to-Point and Broadcast */
    /*~ CASE WMS_FORMAT_CDMA wms_client_ts_data_u.cdma */

    wms_gw_pp_ts_data_s_type    gw_pp;
      /* Point-to-Point */
    /*~ CASE WMS_FORMAT_GW_PP wms_client_ts_data_u.gw_pp */

    wms_gw_cb_ts_data_s_type    gw_cb;
      /* Cell Broadcast */
    /*~ CASE WMS_FORMAT_GW_CB wms_client_ts_data_u.gw_cb */

    /*~ DEFAULT wms_client_ts_data_u.void */

  } u; /*~ FIELD wms_client_ts_data_s.u DISC _OBJ_.format */

} wms_client_ts_data_s_type;


#ifdef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_msg_set_auto_cdma_params

DESCRIPTION
  Sets particular CDMA parameters based on cdma_set_params_ptr, to be used for
  MO messaging on the CDMA network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Internal data set for MO processing.

=========================================================================*/
wms_status_e_type wms_msg_set_auto_cdma_params
(
  wms_client_id_type              client_id,
  wms_cdma_template_s_type        *cdma_set_params_ptr  /* IN */
);
/*~ FUNCTION wms_msg_set_auto_cdma_params */
/*~ PARAM IN cdma_set_params_ptr POINTER */

/*=========================================================================
FUNCTION
  wms_msg_set_auto_gw_params

DESCRIPTION
  Sets particular GW parameters based on gw_set_params_ptr, to be used for
  MO messaging on the GW network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Internal data set for MO processing.

COMMENTS
  Set gw_set_params_ptr->alpha_id.data to a Valid Value or NULL
  Set gw_set_params_ptr->alpha_id.len to a Valid Value or 0

=========================================================================*/
wms_status_e_type wms_msg_set_auto_gw_params
(
  wms_client_id_type              client_id,
  wms_gw_template_s_type          *gw_set_params_ptr    /* IN */
);
/*~ FUNCTION wms_msg_set_auto_gw_params */
/*~ PARAM IN gw_set_params_ptr POINTER */

/*=========================================================================
FUNCTION
  wms_msg_get_auto_cdma_params

DESCRIPTION
  Gets particular CDMA parameters, to be used for MO messaging on the CDMA
  network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Modifies cdma_get_params_ptr directly.

=========================================================================*/
wms_status_e_type wms_msg_get_auto_cdma_params
(
  wms_client_id_type              client_id,
  wms_cdma_template_s_type        *cdma_get_params_ptr  /* OUT */
);
/*~ FUNCTION wms_msg_get_auto_cdma_params */
/*~ PARAM INOUT cdma_get_params_ptr POINTER */

/*=========================================================================
FUNCTION
  wms_msg_get_auto_gw_params

DESCRIPTION
  Gets particular GW parameters, to be used for MO messaging on the GW
  network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Modifies gw_get_params_ptr directly.

COMMENTS
  Set gw_set_params_ptr->alpha_id.data to a Valid Value or NULL
  Set gw_set_params_ptr->alpha_id.len to a Valid Value or 0

=========================================================================*/
wms_status_e_type wms_msg_get_auto_gw_params
(
  wms_client_id_type              client_id,
  wms_gw_template_s_type          *gw_get_params_ptr    /* INOUT */
);
/*~ FUNCTION wms_msg_get_auto_gw_params */
/*~ PARAM INOUT gw_get_params_ptr POINTER */

/*=========================================================================
FUNCTION
  wms_ts_convert_cdma_gw_msg_to_auto

DESCRIPTION
  Converts a CDMA or GW msg_ptr, to a message in AUTOMATIC format auto_msg_ptr.
  email_len represents the number of characters in the standard e-mail
  provided. For CDMA, if using the standard e-mail implementation, email_len
  will be disregarded. Non-standard implementation will take email_len into
  consideration. For GW, email_len will always be taken into consideration and
  should be the total characters for the email address.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies auto_msg_ptr directly.

=========================================================================*/
wms_status_e_type wms_ts_convert_cdma_gw_msg_to_auto
(
  const wms_client_message_s_type     *msg_ptr,       /* IN */
  uint8                               email_len,
  wms_auto_message_s_type             *auto_msg_ptr   /* OUT */
);
/*~ FUNCTION wms_ts_convert_cdma_gw_msg_to_auto */
/*~ PARAM IN msg_ptr POINTER */
/*~ PARAM INOUT auto_msg_ptr POINTER */

/*=========================================================================
FUNCTION
  wms_convert_auto_to_cdma_gw_msg

DESCRIPTION
  Converts an automatic formatted message, auto_msg_ptr, to a CDMA or GW msg,
  msg_ptr, based on:
  (1) msg_mode desired, CDMA format or GW format.
  (2) the set auto params declared by client_id.
  NOTE: mem_store and index will not be assigned for the client message hdr,
        the tag will be set to WMS_TAG_NONE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies msg_ptr directly.

=========================================================================*/
wms_status_e_type wms_ts_convert_auto_to_cdma_gw_msg
(
  wms_client_id_type                  client_id,
  wms_message_mode_e_type             msg_mode,
  const wms_auto_message_s_type       *auto_msg_ptr,  /* IN */
  wms_client_message_s_type           *msg_ptr        /* INOUT */
);
/*~ FUNCTION wms_ts_convert_auto_to_cdma_gw_msg */
/*~ PARAM IN auto_msg_ptr POINTER */
/*~ PARAM INOUT msg_ptr POINTER */
#endif /*CUST_EDITION*/


/* <EJECT> */
/*===========================================================================

                          Client Group

                    API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_client_init

DESCRIPTION
  Allow the client to register itself with the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init
(
  wms_client_type_e_type   client_type,
  wms_client_id_type       *client_id_ptr
);
/*~ FUNCTION wms_client_init */
/*~ PARAM INOUT client_id_ptr POINTER */
/*~ RELEASE_FUNC wms_client_release(*client_id_ptr) */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_release

DESCRIPTION
  Allow the client to release itself from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data reset for this client.

=========================================================================*/
wms_client_err_e_type wms_client_release
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_release */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_activate

DESCRIPTION
  The client tells the API that it is ready to use the services from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_activate
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_activate */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_deactivate

DESCRIPTION
  The client tells the API that it is not ready to use the services.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_deactivate
(
  wms_client_id_type       client_id
);
/*~ FUNCTION wms_client_deactivate */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_reg_cfg_cb

DESCRIPTION
  Allow the client to register its configuration event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_cfg_cb
(
  wms_client_id_type       client_id,
  wms_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION wms_client_reg_cfg_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_reg_msg_cb

DESCRIPTION
  Allow the client to register its message event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb
(
  wms_client_id_type       client_id,
  wms_msg_event_cb_type    msg_event_cb
);
/*~ FUNCTION wms_client_reg_msg_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_reg_msg_info_cache_cb

DESCRIPTION
  Allow the client to register its callback function for telling WMS how to
  cache the message information for the messages from the memory.

  If a null pointer is passed, the callback is de-registered. No message
  information will be cached.

  NOTE: Only one cache callback is allowed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
void wms_reg_msg_info_cache_cb
(
  wms_msg_info_cache_cb_type    msg_info_cache_cb
);
/*~ FUNCTION wms_reg_msg_info_cache_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_reg_cl_parse_msg_cb

DESCRIPTION
  Allow the client to register its callback function for additional parsing
  of a message.

  If a null pointer is passed, the callback is de-registered. No parsing of the
  message will be administered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The message may not be shared amoung other clients, depending on the criteria
  of the parsing being administered to the message.
=========================================================================*/
wms_client_err_e_type wms_client_reg_parse_msg_cb
(
  wms_client_id_type               client_id,
  wms_cl_parse_msg_cb_type         msg_parsing_cb
);
/*~ FUNCTION wms_client_reg_parse_msg_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_reg_dc_cb

DESCRIPTION
  Allow the client to register its DC event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb
(
  wms_client_id_type       client_id,
  wms_dc_event_cb_type     dc_event_cb
);
/*~ FUNCTION wms_client_reg_dc_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */
#ifdef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_client_reg_bc_cb

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_cb
(
  wms_client_id_type       client_id,
  wms_bc_event_cb_type     bc_event_cb
);
/*~ FUNCTION wms_client_reg_bc_cb */
#endif /*CUST_EDITION*/
/*=========================================================================
FUNCTION
  wms_client_reg_bc_mm_cb

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb
(
  wms_client_id_type          client_id,
  wms_bc_mm_event_cb_type     bc_event_cb
);
/*~ FUNCTION wms_client_reg_bc_mm_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */

/*=========================================================================
FUNCTION
  wms_client_reg_dbg_cb

DESCRIPTION
  Allow the client to register its Debug event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb
(
  wms_client_id_type       client_id,
  wms_dbg_event_cb_type    dbg_event_cb
);
/*~ FUNCTION wms_client_reg_dbg_cb */
/*~ ONERROR return WMS_CLIENT_ERR_RPC */


/* <EJECT> */
/*===========================================================================

                          Configuration Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_cfg_set_routes

DESCRIPTION
  Allow the client to change the message routing configuration.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_routes
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  const wms_routes_s_type          * routes_ptr
);
/*~ FUNCTION wms_cfg_set_routes */
/*~ PARAM IN routes_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_get_routes

DESCRIPTION
  Allow the client to retrieve the current message routing configuration.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_routes
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_cfg_get_routes */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_get_memory_status

DESCRIPTION
  Allow the client to retrieve the status for a memory store.
  If tag==WMS_TAG_NONE: all used slots are calculated;
  otherwise, only messages having this tag value are counted as used_tag_slots.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_memory_status
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
);
/*~ FUNCTION wms_cfg_get_memory_status */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_get_message_list

DESCRIPTION
  Allow the client to retrieve the message list for a memory store.
  If tag==WMS_TAG_NONE: all types of messages are included in the list;
  otherwise, only messages having this tag value are included.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_message_list
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_memory_store_e_type          mem_store,
  wms_message_tag_e_type           tag
);
/*~ FUNCTION wms_cfg_get_message_list */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_set_gw_domain_pref

DESCRIPTION
  Allow the client to set the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_set_gw_domain_pref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_gw_domain_pref_e_type         gw_domain_pref
);
/*~ FUNCTION wms_cfg_set_gw_domain_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_get_gw_domain_pref

DESCRIPTION
  Allow the client to get the GW Domain Preference

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_cfg_get_gw_domain_pref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_cfg_get_gw_domain_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_set_primary_client

DESCRIPTION
  This Allow the client to make itself the primary client. The advantage of
  being a  primary client is that your memory status is checked for memory
  full and SMMA Request sent to the network.

USAGE
  Once a primary client is set, no other client can become the primary client.
  If the set_primay == FALSE; the primary client is deregistered

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_set_memory_full()

=========================================================================*/
wms_status_e_type wms_cfg_set_primary_client
(
  wms_client_id_type      client_id,
  wms_cmd_cb_type         cmd_cb,
  const void            * user_data,
  boolean                 set_primary,
  boolean                 use_client_memory
);
/*~ FUNCTION wms_cfg_set_primary_client */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_cfg_set_memory_full

DESCRIPTION
  This allows the primary client to specify its Memory Status.

USAGE
  Only the primary client will be allowed to set/reset the Memory Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO
  wms_cfg_set_primary()
=========================================================================*/
wms_status_e_type wms_cfg_set_memory_full
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          memory_full
);
/*~ FUNCTION wms_cfg_set_memory_full */
/*~ ONERROR return WMS_RPC_ERROR_S */
#ifdef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_cfg_set_multisend

DESCRIPTION
  This allows the primary client to specify its multisend Status.

USAGE
  Only the primary client will be allowed to set/reset the multisend Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO

=========================================================================*/
wms_status_e_type wms_cfg_set_multisend
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          multisend
);
/*~ wms_cfg_set_multisend */

#ifdef FEATURE_AUTOREPLACE
/*=========================================================================
FUNCTION
  wms_cfg_set_autoreplace

DESCRIPTION
  This allows the primary client to specify its autoreplace Status.

USAGE
  Only the primary client will be allowed to set/reset the autoreplace Status.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

SEE ALSO

=========================================================================*/
wms_status_e_type wms_cfg_set_autoreplace
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                     * user_data,
  boolean                          autoreplace
);
/*~ wms_cfg_set_autoreplace */
#endif
#endif /*CUST_EDITION*/

/*=========================================================================
FUNCTION
wms_cfg_locate_mo_msg_for_sts_report

DESCRIPTION
  Locates the origina MO message's index on the SIM given the message reference
  number in the received Status Report message.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_locate_mo_msg_for_sts_report
(
  wms_message_index_type  *rec_id,      /* OUTPUT */
  wms_message_number_type  message_ref  /* INPUT */
);
/*~ FUNCTION wms_cfg_locate_mo_msg_for_sts_report */
/*~ PARAM INOUT rec_id POINTER */
/*~ ONERROR return FALSE */

/*=========================================================================
FUNCTION
wms_cfg_get_sts_rpt_index

DESCRIPTION
  Gets the Status Report's index on the SIM given the MO SMS's index on the SIM.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_get_sts_rpt_index
(
  wms_message_index_type sms_index,   /* INPUT */
  wms_message_index_type *smsr_index  /* OUTPUT */
);
/*~ FUNCTION wms_cfg_get_sts_rpt_index */
/*~ PARAM INOUT smsr_index POINTER */
/*~ ONERROR return FALSE */

/*=========================================================================
FUNCTION
wms_cfg_check_voicemail_contents

DESCRIPTION
  Evaluates message for voicemail contents.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: voicemail message, FALSE: not a voicemail message

SIDE EFFECTS
  Updates is_voicemail_active and count.

=========================================================================*/

#ifndef CUST_EDITION
boolean wms_cfg_check_voicemail_contents
(
  wms_message_mode_e_type         msg_mode,             /* INPUT */
  const wms_raw_ts_data_s_type    *message,             /* INPUT */
  boolean                         *is_voicemail_active, /* OUTPUT */
  uint8                           *count  /* OUTPUT: how many voice mails */
);
#else
boolean wms_cfg_check_voicemail_contents
(
  wms_message_mode_e_type         msg_mode,             /* INPUT */
  const wms_client_message_s_type *pCltMsg,             /* INPUT */
  boolean                         *is_voicemail_active, /* OUTPUT */
  uint8                           *count  /* OUTPUT: how many voice mails */
);
#endif

/*~ FUNCTION wms_cfg_check_voicemail_contents */
/*~ PARAM IN message POINTER */
/*~ PARAM INOUT is_voicemail_active POINTER */
/*~ PARAM INOUT count POINTER */
/*~ ONERROR return FALSE */

/*===========================================================================
FUNCTION wms_cfg_check_cphs_msg

DESCRIPTION
  Determine if a message is CPHS specific, using TP-OA for voice message waiting indicator

DEPENDENCIES
  none

RETURN VALUE
  The routing.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_check_cphs_msg
(
   const wms_address_s_type     *addr
);
/*~ FUNCTION wms_cfg_check_cphs_msg */
/*~ PARAM IN addr POINTER */
/*~ ONERROR return FALSE */

/*=========================================================================
FUNCTION
wms_cfg_check_cdma_duplicate

DESCRIPTION
  Check if a CDMA message is a duplicate in comparison to the messages
  stored in RUIM and NV.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  duplicate
  FALSE: not a duplicate

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_cdma_duplicate
(
  const wms_client_message_s_type     *msg_ptr
);
/*~ FUNCTION wms_cfg_check_cdma_duplicate */
/*~ PARAM IN msg_ptr POINTER */
/*~ ONERROR return FALSE */

/*===========================================================================
FUNCTION wms_cfg_set_link_control

DESCRIPTION
  Send a request to the lower layers to disable/enable SMS link control for
  connection establishment and timer duration

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_cfg_set_link_control
(
  wms_client_id_type                client_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                       *user_data,
  wms_cfg_link_control_mode_e_type  control_option,
  uint8                             idle_timer /* in seconds */
);
/*~ FUNCTION wms_cfg_set_link_control */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*===========================================================================
FUNCTION wms_cfg_get_link_control

DESCRIPTION
  Retrieves the current link control setting.  WMS will send a cfg event to
  all clients to infom if the link is up or down and mode

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_cfg_get_link_control
(
  wms_client_id_type                client_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                       *user_data
);
/*~ FUNCTION wms_cfg_get_link_control */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*===========================================================================
FUNCTION wms_cfg_locate_voicemail_line2

DESCRIPTION
  Retrieves the msg index for voicemail line 2.

DEPENDENCIES
  none

RETURN VALUE
  FALSE - no active voicemail in line 2
  TRUE - there is active voicemail in line 2

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cfg_locate_voicemail_line2
(
  wms_message_index_type        *sms_index   /* OUTPUT */
);
/*~ FUNCTION wms_cfg_locate_voicemail_line2 */
/*~ PARAM INOUT sms_index POINTER */
/*~ ONERROR return FALSE */

/*=========================================================================
FUNCTION
wms_cfg_check_wap_push_message

DESCRIPTION
  Check if a CDMA / GSM / WCDMA message is a WAP Push Message.

  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is WAP Push Message
  FALSE: Is Not a WAP Push Message

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_cfg_check_wap_push_message
(
  const wms_client_message_s_type     *msg_ptr
);
/*~ FUNCTION wms_cfg_check_wap_push_message */
/*~ PARAM IN msg_ptr POINTER */
/*~ ONERROR return FALSE */

/* <EJECT> */
/*===========================================================================

                              Message Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_msg_send

DESCRIPTION
  Allow the client to send a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_send
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                         * user_data,
  wms_send_mode_e_type               send_mode,
  const wms_client_message_s_type    * message_ptr
);
/*~ FUNCTION  wms_msg_send */
/*~ PARAM IN message_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_ack

DESCRIPTION
  Allow the client to acknowledge a message if the message has not been
  acknowledged.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_ack
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_ack_info_s_type       *ack_info_ptr
);
/*~ FUNCTION  wms_msg_ack */
/*~ PARAM IN ack_info_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_read

DESCRIPTION
  Allow the client to read a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION wms_msg_read */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_read_sts_report

DESCRIPTION
  Allow the client to read a Status Report from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read_sts_report
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_read_sts_report */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_get_cache_info

DESCRIPTION
  Allow the client to retrieve the message cache info for a message stored in
  a memory.

  NOTE: This is a synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  Cmd error.

SIDE EFFECTS
  None.

=========================================================================*/
wms_status_e_type wms_msg_get_cache_info
(
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag,
  uint8                           * cache_ptr
);
/*~ FUNCTION  wms_msg_get_cache_info */
/*~ PARAM INOUT cache_ptr ARRAY WMS_MSG_INFO_CACHE_SIZE */ 
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_write

DESCRIPTION
  Allow the client to write a message to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_write
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                         * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type    * message_ptr
);
/*~ FUNCTION wms_msg_write */
/*~ PARAM IN message_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_delete

DESCRIPTION
  Allow the client to delete a message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The corresponding Status Report for the Message if stored is also deleted.

=========================================================================*/
wms_status_e_type wms_msg_delete
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION wms_msg_delete */
#ifdef CUST_EDITION
/*==============================================================================
FUNCTION
    wms_msg_delete_box

DESCRIPTION
    Allow the client to delete all message in a message box.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note
    A command is sent to WMS task.
==============================================================================*/
wms_status_e_type wms_msg_delete_box
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_deletebox_e_type            box_deltype
);
/*~ FUNCTION wms_msg_delete_box */

/*==============================================================================
FUNCTION
    wms_msg_copy

DESCRIPTION
    Allow the client to copy a message from one Storage to another Storage.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note
    A command is sent to WMS task. Only use for MT message copy between UIM
    and NV. If index_dest not equal to WMS_DUMMY_MESSAGE_INDEX, the dest message
    will be replaced.
==============================================================================*/
wms_status_e_type wms_msg_copy
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_msg_copy_type               *pmsg_copy
);
/*~ FUNCTION wms_msg_copy */

#if defined(FEATURE_UIM_TOOLKIT)
/*==============================================================================
FUNCTION
    wms_msg_refresh_ruimmsg

DESCRIPTION
    Allow the client to rebuild ruim sms cache.

RETURN VALUE
    WMS_OK_S
    WMS_OUT_OF_RESOURCES_S

note

==============================================================================*/
wms_status_e_type wms_msg_refresh_ruimmsg
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_msg_refresh_ruimmsg */
#endif
#endif /*CUST_EDITION*/

/*=========================================================================
FUNCTION
  wms_msg_delete_all

DESCRIPTION
  Allow the client to delete all message from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_tag_e_type          tag
);
/*~ FUNCTION wms_msg_delete_all */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_modify_tag

DESCRIPTION
  Allow the client to modify the tag of a message.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_modify_tag
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag
);
/*~ FUNCTION  wms_msg_modify_tag */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_read_template

DESCRIPTION
  Allow the client to read a template from a memory store. If memory store
  is NV, the message index is ignored since only one template is in NV.
  Otherwise, the memory store can only be SIM.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_read_template
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_read_template */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_write_template

DESCRIPTION
  Allow the client to write a template from a memory store. If memory store
  is NV, the message index is ignored since only one template is in NV.
  Otherwise, only SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.data
                              to a Valid Value or NULL
  If referenced in the union, set msg_ptr->u.gw_template.alpha_id.len
                              to a Valid Value or 0

=========================================================================*/
wms_status_e_type wms_msg_write_template
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  wms_write_mode_e_type            write_mode,
  const wms_client_message_s_type  * message_ptr
);
/*~ FUNCTION  wms_msg_write_template */
/*~ PARAM IN message_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_delete_template

DESCRIPTION
  Allow the client to delete a template from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_template
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_delete_template */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_delete_template_all

DESCRIPTION
  Allow the client to delete all templates from a memory store.
  Only NV or SIM memory store is allowed.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_template_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store
);
/*~ FUNCTION  wms_msg_delete_template_all */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_write_sts_report

DESCRIPTION
  Allow the client to write a status report to a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

COMMENTS
  The Status Report will only be stored if the corresponding SMS Message 
  is also stored in the same Memory. If the SMS Message is not found, 
  WMS_CMD_ERR_MSG_NO_MO_MSG will be returned in Command Callback.

=========================================================================*/
wms_status_e_type wms_msg_write_sts_report
(
  wms_client_id_type                 client_id,
  wms_cmd_cb_type                    cmd_cb,
  const void                         * user_data,
  wms_write_mode_e_type              write_mode,
  const wms_client_message_s_type    * message_ptr
);
/*~ FUNCTION  wms_msg_write_sts_report */
/*~ PARAM IN message_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_delete_sts_report

DESCRIPTION
  Allow the client to delete a status report from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_sts_report
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index
);
/*~ FUNCTION  wms_msg_delete_sts_report */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_delete_sts_report_all

DESCRIPTION
  Allow the client to delete all status reports from a memory store.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_delete_sts_report_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_memory_store_e_type         mem_store
);
/*~ FUNCTION  wms_msg_delete_sts_report_all */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_set_retry_period

DESCRIPTION
  Allow a client to specify the period of allowing retries of sms messages.
  NOTE: if period = 0 seconds, retry will not be attempted.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_set_retry_period
(
  wms_client_id_type              client_id,
  uint32                          period     /* in seconds */
);
/*~ FUNCTION  wms_msg_set_retry_period */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_msg_get_retry_period

DESCRIPTION
  Allow a client to retrieve the period of allowing retries of sms messages.
  
DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_get_retry_period
(
  wms_client_id_type              client_id,
  uint32                          * period     /* in seconds */
);
/*~ FUNCTION  wms_msg_get_retry_period */
/*~ PARAM INOUT period POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/* <EJECT> */
/*===========================================================================

                         Dedicated Channel Group

                        API FUNCTION DEFINITIONS

===========================================================================*/


/*=========================================================================
FUNCTION
  wms_dc_enable_auto_disconnect

DESCRIPTION
  This function enables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period. The client will receive an event if the connection is disconnected.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_enable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  uint32                          timeout    /* in seconds */
);
/*~ FUNCTION  wms_dc_enable_auto_disconnect */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_dc_disable_auto_disconnect

DESCRIPTION
  This function disables the feature of auto-disconnecting the DC connection
  when there is no message sent or received during the specified time out
  period.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disable_auto_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION  wms_dc_disable_auto_disconnect */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_dc_connect

DESCRIPTION
  This function requests to establish a DC connection. It will queue
  the request and signal the SMS task to process it. Once the DC connection
  is established or fails, an event will be sent to the client.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_connect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_dc_so_e_type                so
);
/*~ FUNCTION  wms_dc_connect */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_dc_disconnect

DESCRIPTION
  This function is used to disconnect the DC connection when there is a
  request from the client or from the base station. It will send CDMA
  messages to the base station to disconnect the DC connection.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_dc_disconnect
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_dc_disconnect */
#ifdef CUST_EDITION



/* <EJECT> */
/*===========================================================================

                            Broadcast Group

                        API FUNCTION DEFINITIONS

===========================================================================*/


/*=========================================================================
FUNCTION
  wms_bc_get_config

DESCRIPTION
  This function is used to retrieve the configuration for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_config
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_bc_get_config */

/*=========================================================================
FUNCTION
  wms_bc_get_pref

DESCRIPTION
  This function is used to retrieve the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_bc_get_pref */

/*=========================================================================
FUNCTION
  wms_bc_set_pref

DESCRIPTION
  This function is used to set the user preference for broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_set_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_bc_pref_e_type     pref
);
/*~ FUNCTION wms_bc_set_pref */

/*=========================================================================
FUNCTION
  wms_bc_get_table

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_table
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_bc_get_table */

/*=========================================================================
FUNCTION
  wms_bc_select_service

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_select_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type  *srv_id,
  boolean                         selected,
  wms_priority_e_type             priority
);
/*~ FUNCTION wms_bc_select_service */
/*~ PARAM IN srv_id POINTER */

/*=========================================================================
FUNCTION
  wms_bc_get_all_service_ids

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_all_service_ids
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_bc_get_all_service_ids */

/*=========================================================================
FUNCTION
  wms_bc_get_service_info

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_get_service_info
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type  *srv_id
);
/*~ FUNCTION wms_bc_get_service_info */
/*~ PARAM IN srv_id POINTER */

/*=========================================================================
FUNCTION
  wms_bc_add_service

DESCRIPTION
  This function is used to add an entry to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_add_service
(
  wms_client_id_type                client_id,
  wms_cmd_cb_type                   cmd_cb,
  const void                        *user_data,
  const wms_bc_service_info_s_type  *srv_info
);
/*~ FUNCTION wms_bc_add_service */
/*~ PARAM IN srv_info POINTER */

/*=========================================================================
FUNCTION
  wms_bc_delete_service

DESCRIPTION
  This function is used to delete an entry to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_delete_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type   *srv_id
);
/*~ FUNCTION wms_bc_delete_service */
/*~ PARAM IN srv_id POINTER */

/*=========================================================================
FUNCTION
  wms_bc_change_label

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_change_label
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  const wms_bc_service_id_s_type  *srv_id,
  const char                      *label_ptr
);
/*~ FUNCTION wms_bc_change_label */
/*~ PARAM IN srv_id POINTER */
/*~ PARAM IN label_ptr STRING */ 


/*=========================================================================
FUNCTION
  wms_bc_delete_all_services

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_delete_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_bc_delete_all_services */

/*=========================================================================
FUNCTION
  wms_bc_set_priority_for_all

DESCRIPTION
  This function is used to change priority levels for all services in the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_set_priority_for_all
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_priority_e_type             priority
);
/*~ FUNCTION wms_bc_set_priority_for_all */
#endif /*CUST_EDITION*/
/* <EJECT> */
/*===========================================================================

                        Multimode Broadcast Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_bc_mm_get_config

DESCRIPTION
  This function is used to retrieve the configuration for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_config
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_get_config */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_pref

DESCRIPTION
  This function is used to retrieve the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_get_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_pref

DESCRIPTION
  This function is used to set the user preference for
  Multimode Broadcast SMS.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_set_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_bc_pref_e_type              pref
);
/*~ FUNCTION wms_bc_mm_set_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_table

DESCRIPTION
  This function is used to retrieve the service table for broadcast SMS.

  NOTE: To prevent retrieving a large service table, the function
  wms_bc_mm_get_all_service_ids() can be called to retrieve all service
  IDs, where each ID is much smaller than a complete table entry, and then
  the function wms_bc_mm_get_service_info() can be called for each service ID
  in order to retrieve the table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_table
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_get_table */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_select_service

DESCRIPTION
  This function is used to select a broadcast SMS service

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_select_service
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type     *srv_id_ptr,
  boolean                         selected
);
/*~ FUNCTION wms_bc_mm_select_service */
/*~ PARAM IN srv_id_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_all_service_ids

DESCRIPTION
  This function is used to retrieve all the service IDs in the broadcast
  SMS service table. After the retrieval of the service IDs, the function
  wms_bc_get_service_info() can be called to retrieve the complete
  table entries one by one.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_all_service_ids
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_get_all_service_ids */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_service_info

DESCRIPTION
  This function is used to retrieve a table entry in the broadcast SMS
  service table using a service ID.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_get_service_info
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  const wms_bc_mm_srv_id_type     *srv_id_ptr
);
/*~ FUNCTION wms_bc_mm_get_service_info */
/*~ PARAM IN srv_id_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_add_services

DESCRIPTION
  This function is used to add entries to the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_add_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_service_info_s_type   *entries
);
/*~ FUNCTION wms_bc_mm_add_services */
/*~ PARAM IN entries VARRAY LENGTH num_entries */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_delete_services

DESCRIPTION
  This function is used to delete entries from the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_delete_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  uint8                           num_entries,
  const wms_bc_mm_srv_id_type     *srv_ids
);
/*~ FUNCTION wms_bc_mm_delete_services */
/*~ PARAM IN srv_ids VARRAY LENGTH num_entries */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_change_service_info

DESCRIPTION
  This function is used to change an entry in the broadcast SMS service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_change_service_info
(
  wms_client_id_type                  client_id,
  wms_cmd_cb_type                     cmd_cb,
  const void                          *user_data,
  wms_message_mode_e_type             message_mode,
  const wms_bc_mm_service_info_s_type *srv_info_ptr
);
/*~ FUNCTION wms_bc_mm_change_service_info */
/*~ PARAM IN srv_info_ptr POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_delete_all_services

DESCRIPTION
  This function is used to delete all entries from the broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_delete_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_delete_all_services */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_select_all_services

DESCRIPTION
  This function is used to select/deselect all entries from the
  broadcast service table.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.

=========================================================================*/
wms_status_e_type wms_bc_mm_select_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         selected
);
/*~ FUNCTION wms_bc_mm_select_all_services */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_priority_for_all_services

DESCRIPTION
  This function is used to set the priorities for all entries from the
  broadcast service table. (Specific to CDMA, Does not apply to GW)

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_set_priority_for_all_services
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data,
  wms_message_mode_e_type         message_mode,
  wms_priority_e_type             priority
);
/*~ FUNCTION wms_bc_mm_set_priority_for_all_services */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Clients to
  indicate the deletion of a GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_msg_delete_indication
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_gw_cb_page_header_s_type   *cb_page_header
);
/*~ FUNCTION wms_bc_mm_msg_delete_indication */
/*~ PARAM IN cb_page_header POINTER */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_msg_delete_all_indication

DESCRIPTION
  This function is used to notify the lower layer of Message Deletion.
  This is applicable to GW mode only and is used by the WMS Client to
  indicate the deletion of ALL GW CB page.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_msg_delete_all_indication
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data
);
/*~ FUNCTION wms_bc_mm_msg_delete_all_indication */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_get_reading_pref

DESCRIPTION
  This function is used to Get the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_get_reading_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode
);
/*~ FUNCTION wms_bc_mm_get_reading_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
  wms_bc_mm_set_reading_pref

DESCRIPTION
  This function is used to Set the Reading Preferences - Reading Advised
  and Reading Optional for UMTS CBS Messages
  This is applicable to GW mode only.

DEPENDENCIES
  None

RETURN VALUE
  Status of the request.

SIDE EFFECTS
  Request is added to the request queue.
=========================================================================*/
wms_status_e_type wms_bc_mm_set_reading_pref
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                     *user_data,
  wms_message_mode_e_type         message_mode,
  boolean                         reading_advised,
  boolean                         reading_optional
);
/*~ FUNCTION wms_bc_mm_set_reading_pref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/* <EJECT> */
/*===========================================================================

                            Translation Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_ts_encode

DESCRIPTION
  Allow the client to encode a Transport Service message.

DEPENDENCIES
  None

RETURN VALUE
  Encoding status

SIDE EFFECTS
  None

COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.

=========================================================================*/
wms_status_e_type wms_ts_encode
(
  const wms_client_ts_data_s_type         * client_ts_data_ptr,
  wms_raw_ts_data_s_type                  * raw_ts_data_ptr /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode

DESCRIPTION
  Allow the client to decode a Transport Service message.

  Note: If the CDMA bearer data has 'other' parameters, upon return of this
  API function, the 'mask' will have WMS_MASK_BD_OTHER set, and
  'desired_other_len' will indicate how many bytes are needed to properly
  decode the 'other' parameters by calling wms_ts_decode_cdma_bd_with_other().

DEPENDENCIES
  None

RETURN VALUE
  Decoding status

SIDE EFFECTS
  None

COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.
=========================================================================*/
wms_status_e_type wms_ts_decode
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr /* OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_cdma_bd_with_other

DESCRIPTION
  Allow the client to decode CDMA BD with other parameters. See also function 
  comments for wms_ts_decode().

  The client shall make sure 'input_other_len' reflects the buffer size of
  'other_data' which should have enough room for decoding the other parameters.
  The size of the memory allocation in 'other_data' can be determined
  from the 'desired_other_len' after calling wms_ts_decode().

DEPENDENCIES
  None

RETURN VALUE
  Decoding status

SIDE EFFECTS
  None

COMMENTS
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.input_other_len
                              to a Valid Value or 0.
  If referenced in the union, Set client_ts_data_ptr->u.cdma.other.other_data
                              to a Valid Value or NULL.

=========================================================================*/
wms_status_e_type wms_ts_decode_cdma_bd_with_other
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr
);

/*=========================================================================
FUNCTION
  wms_ts_get_udh_length

DESCRIPTION
  Allow the client to get the header length for the given user data header.

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_get_udh_length
(
  const wms_udh_s_type                    *udh
);

/*=========================================================================
FUNCTION
  wms_ts_bcd_to_ascii

DESCRIPTION
  Convert BCD digits to an ASCII number string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_bcd_to_ascii
(
  const uint8         * addr,
  uint8               len,
  uint8               * out  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_ascii_to_bcd

DESCRIPTION
  Convert an ASCII number string to BCD digits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_bcd
(
  const uint8     * ascii,
  uint8           len,
  uint8           * out  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_ascii_to_default

DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_default
(
  const uint8   * asc,
  uint32        len,
  uint8         * def  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_default_to_ascii

DESCRIPTION
  Convert 7-bit GSM Default alphabet string to an ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_default_to_ascii
(
  const uint8   * def,
  uint32        len,
  uint8         * asc  /* IN/OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_relative_time

DESCRIPTION
  Decode the Relative Timestamp raw byte to a WMS Timestamp structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_decode_relative_time
(
  uint8                     v,
  wms_timestamp_s_type    * timestamp
);

/*=========================================================================
FUNCTION
  wms_ts_encode_relative_time

DESCRIPTION
  Encode the Relative Time to a raw byte.

DEPENDENCIES
  None

RETURN VALUE
  Encoded Byte

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_relative_time
(
  const wms_timestamp_s_type  *timestamp
);

/*=========================================================================
FUNCTION
  wms_ts_encode_dcs

DESCRIPTION
  Encode Data Coding Scheme into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_dcs
(
  const wms_gw_dcs_s_type   *dcs,   /* INPUT */
  uint8                     *data   /* OUTPUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_dcs

DESCRIPTION
  Decode Data Coding Scheme from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_dcs
(
  const uint8           *data,     /* INPUT */
  wms_gw_dcs_s_type     *dcs       /* INPUT/OUTPUT */
);

/*===========================================================================

FUNCTION    wms_ts_pack_ascii

DESCRIPTION
  Pack the 7 bits in each character of a null-terminated ASCII string into a
  a byte array.

DEPENDENCIES
  None

RETURN VALUE
  The string length.

SIDE EFFECTS
  None

===========================================================================*/
uint8 wms_ts_pack_ascii
(
  const char         * ascii_ptr,       /* IN */
  uint8              * data,            /* IN/OUT */
  uint8              * data_len_ptr,    /* IN/OUT */
  uint8              * padding_bits_ptr /* IN/OUT */
);

/*===========================================================================

FUNCTION    wms_ts_unpack_ascii

DESCRIPTION
  Unpack the 7-bit ASCII string from the packed format in a byte array.

DEPENDENCIES
  None

RETURN VALUE
  The string length.

SIDE EFFECTS
  None

===========================================================================*/
uint8 wms_ts_unpack_ascii
(
  const wms_cdma_user_data_s_type    *ud,     /* IN */
  uint8                              buf_len, /* IN */
  uint8                              *buf     /* INOUT */
);

/*===========================================================================

FUNCTION    wms_ts_dtmf2ascii

DESCRIPTION
  Convert the DTMF digits to ASCII digits.

DEPENDENCIES
  None

RETURN VALUE
  Number of digits converted

SIDE EFFECTS
  None

===========================================================================*/
uint8 wms_ts_dtmf2ascii
(
  uint8            len,
  const uint8     *dtmf,
  uint8           *ascii
);

/*===========================================================================

FUNCTION    wms_ts_ascii2dtmf

DESCRIPTION
  Convert the ASCII digit string to DTMF digits

DEPENDENCIES
  None

RETURN VALUE
  Number of digits converted

SIDE EFFECTS
  None

===========================================================================*/
uint8 wms_ts_ascii2dtmf
(
  const char     *ascii,
  uint8          *dtmf
);

/*=========================================================================
FUNCTION
wms_ts_pack_gw_user_data

DESCRIPTION
  Packs only the user data for a GW Message

DEPENDENCIES
  None

RETURN VALUE
  status of the conversion

SIDE EFFECTS
  Modifies raw_ts_data_ptr directly

=========================================================================*/
wms_status_e_type wms_ts_pack_gw_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
);

/*=========================================================================
FUNCTION
wms_ts_pack_cdma_user_data

DESCRIPTION
  Packs only the user data for a cdma messge given the encoding.

DEPENDENCIES
  None

RETURN VALUE
  status of the conversion

SIDE EFFECTS
  Modifies raw_ts_data_ptr directly

=========================================================================*/
wms_status_e_type wms_ts_pack_cdma_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
);

/*=========================================================================
FUNCTION
wms_ts_cdma_OTA2cl

DESCRIPTION
  Converts OTA data provided from the card to client message format that
  can be processed normally by WMS for delivery.

DEPENDENCIES
  None

RETURN VALUE
  status of the conversion

SIDE EFFECTS
  Modifies cl_msg directly.
=========================================================================*/
wms_status_e_type wms_ts_cdma_OTA2cl
(
  const uint8                 *data,
  uint16                      length,
  wms_client_message_s_type   *cl_msg
);

/*=========================================================================
FUNCTION
  wms_ts_pack_gw_7_bit_chars

DESCRIPTION
  Pack GSM 7-bit characters into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint16 wms_ts_pack_gw_7_bit_chars
(
  const uint8     * in,
  uint16          in_len, //number of chars
  uint16          shift,
  uint16          out_len_max,
  uint8           * out
);

/*=========================================================================
FUNCTION
  wms_ts_unpack_gw_7_bit_chars

DESCRIPTION
  Unpack raw data to GSM 7-bit characters.

DEPENDENCIES
  None

RETURN VALUE
  Number of 7-bit charactes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_unpack_gw_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,
  uint8             out_len_max,
  uint16            shift,
  uint8             * out
);

/*=========================================================================
FUNCTION
  wms_ts_encode_user_data_header

DESCRIPTION
  Encode User Data structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_user_data_header
(
  uint8                           num_headers, /* IN */
  const wms_udh_s_type            * headers,   /* IN */
  uint8                           *data        /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_decode_user_data_header

DESCRIPTION
  Decode User Data from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_user_data_header
(
  const uint8               len, /* user_data_length*/
  const uint8               *data,
  uint8                     * num_headers_ptr, /* OUT */
  wms_udh_s_type            * udh_ptr          /* OUT */
);

/*=========================================================================
FUNCTION
  wms_ts_bcd_to_int

DESCRIPTION
  Convert a BCD to Integer.
  Valid BCD values altogether are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  Failure if either of the BCD digit is invaild (greater than 9, e.g. 11111010 = 15 and 10)
  Success if both BCD digits are valid

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_bcd_to_int
(
  const uint8 bcd, /*IN*/
  uint8 *result    /*OUT*/
);

/* <EJECT> */
/*===========================================================================

                            Debugging Group

                        API FUNCTION DEFINITIONS

===========================================================================*/

/*=========================================================================
FUNCTION
  wms_dbg_reset_tl_seq_num

DESCRIPTION
  Reset internal count for the CDMA SMS TL sequence number.
  Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  None.

=========================================================================*/
wms_status_e_type wms_dbg_reset_tl_seq_num
(
  wms_client_id_type              client_id,
  wms_cmd_cb_type                 cmd_cb,
  const void                      *user_data
);
/*~ FUNCTION wms_dbg_reset_tl_seq_num */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
wms_dbg_set_msg_ref

DESCRIPTION
  Sets THE TP-MR to 0. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS


=========================================================================*/
wms_status_e_type wms_dbg_set_msg_ref
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_dbg_set_msg_ref */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
wms_dbg_clear_smma_flag

DESCRIPTION
  Resets the SMMA Flag. Command is sent to WMS Task.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_clear_smma_flag
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
);
/*~ FUNCTION wms_dbg_clear_smma_flag */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
wms_dbg_get_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_get_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data
); 
/*~ FUNCTION wms_dbg_get_retry_interval */
/*~ ONERROR return WMS_RPC_ERROR_S */

/*=========================================================================
FUNCTION
wms_dbg_set_retry_interval

DESCRIPTION
  Retreive the SMS Retry Interval. i.e. the interval between the SMS Retry Attempts
  Value is specified in seconds. Command is sent to WMS Task.
  A Debug Event will be posted by WMS its clients with the Retry Interval.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
=========================================================================*/
wms_status_e_type wms_dbg_set_retry_interval
(
  wms_client_id_type               client_id,
  wms_cmd_cb_type                  cmd_cb,
  const void                       * user_data,
  uint32                           retry_interval /* seconds */
);
/*~ FUNCTION wms_dbg_set_retry_interval */
/*~ ONERROR return WMS_RPC_ERROR_S */

#ifdef __cplusplus
}
#endif

void wms_msg_setmochannel(byte usechl);

#endif /* defined(FEATURE_CDSMS) || defined(GWSMS) */


#endif /* WMS_H */

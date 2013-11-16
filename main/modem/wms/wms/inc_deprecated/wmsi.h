#ifndef WMSI_H
#define WMSI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S   M E S S A G I N G   S E R V I C E S

                       ------ Internal Type Definitions

DESCRIPTION
  This module defines the internal data types and functions that support the
  Short Message Services (SMS) UAPI implementation.

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

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsi.h_v   1.11   22 Jun 2002 12:05:46   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc_deprecated/wmsi.h#5 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/08   HN      Move wmsi_get_card_presence() out of FEATURE_GSTK guard
06/23/08   PMD     Removed FEATURE_SMS_500_MESSAGES from code
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
01/31/08   PMD     Updated Copyright Information to include 2008
07/23/07   PMD     Added support for plus code dialing
07/23/07   NP      LPM broken, WMS does not go or wake up from LPM
06/27/07   PMD     Moved certain defines to wmspriv.h
06/26/07   PMD     Moved certain defines to wmsts.h
05/03/07   HQ      Mainlined certain IWMSC fields.
04/19/07   HQ      Added FEATURE_IWMSC.
04/09/07   HQ      Re-org API header files.
04/06/07   HQ      Added detection of PLMN change.
02/23/07   HQ      Added wms_init() and wms_process_signals().
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
11/13/06   HQ      Ack timer change for JCDMA2.
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
09/29/06   NP      Add new define WMS_GW_TEMPLATE_MIN - minimal record length
                   for the file EF-SMSP
08/31/06   PMD     Increased the Max Commands to 30 to solve WMS being out
                   of Resources on Power Up.
08/26/06   PMD     Added support for EVENT_MO_SMS_RETRY_ATTEMPT
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
08/01/06   PMD     Increased WMS_MAX_RPTS from 10 to 20
07/31/06   PMD     Removed unused feature - FEATURE_GWSMS_INIT_DELAY
06/27/06   PMD     Changes to Maxmimum number of Messages.
06/26/06   PMD     Updated Max SMS Message Slots Documentation.
06/20/06   PMD     Removed wms_task_init_done variable.
05/05/06   PMD     Reduced Max Commands for Low Memory Usage
05/02/06   HQ      Added check for wms_task_init_done.
04/28/06   PMD     Added support for SIM_INIT_COMPLETED Event for Game Mode
04/20/06   PMD     Eliminated MO Ack Timer, Pending Msg and Pending Ack 
                   timer Logic for CDSMS. Added Max_Ack_Retry_Timer.
                   Mainlined FEATURE_SMS_RETRY
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
12/20/05   PMD     Cleaned up FEATURE_SIRIUS from Code Base.
11/04/05   PMD     Increased Max NV Messages to 500 under F_SIRIUS
10/18/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Updated Timer Signals Definitions.
10/17/05   PMD     Changed WMS_TIMER_RETRY_INTERVAL to 5 seconds
                   Added support for Retry Interval Commands
09/08/05   PMD     Increased Max Retry Period and Clear Timer to 240 seconds.
09/06/05   PMD     Added WMS_CLEAR_TIMER_PERIOD Enum
08/29/05   PMD     Added support for 255 NV SMS Messages.
08/16/05   Rex     Added TL max length checking enum 
07/26/05   PMD     Added support for 100 NV Messages under Sirius Feature Flag.
05/14/05   PMD     Added RAM Storage Support for Status Reports
02/18/05   Rex     Added GW max size for udh_other_type
02/01/05   PMD     Added support for wms_bc_mm_get/set_reading_preferences API
01/06/05   Rex     Featurize nas info type under FEATURE_APIONE
09/22/04   Rex     Featurize SMS link control
08/30/04   Rex     Fix struct to enum type for link control
08/25/04   Rex     Added support for SMS link control
07/15/04   PMD     Added Support for CB Message Delete Indication
06/22/04   PMD     Fix for WMS Dog Timeout
05/27/04   Rex     Added support for event WMS_SEARCH_REQUEST
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
02/05/04   HQ      Use FEATURE_MMGSDI for gsdi header files.
02/03/04   HQ      Fixed compile errors when UTK is defined on 1x targets.
01/27/04   AP      Modified signal values for retry.
01/16/04   AP/PMD  Added GW and CDMA Retry
01/12/04   HQ      Added check of CDMA and GW subscription available status
                   to kick start WMS CDMA and GW initialization.
12/15/03   PMD     Added Ix Broadcast Support in Multimode Broadcast API
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
06/27/03   PMD     Added new constant WMS_CARD_NUM_REFRESH_FILES
                   Added New Structure for Handling GSDI Refresh Command
                   Added new prototypes for wms_get_cmd_count and wms_get_rpt_count
                   Added new prototypes for wms_card_register_for_refresh,
                   wms_card_refresh_fcn_ind_proc and wms_card_refresh_complete
06/02/03   AP/HQ   Shortened LAYER2 and LAYER2_LONG timers to 30 and 50
                   seconds, respectively.
04/26/03   HQ/AP   Added WMS_MAX_CDMA_MESSAGES_IN_MEM and
                   WMS_MAX_GW_MESSAGES_IN_MEM.
04/21/03   HQ      Use WMS_MAX_NV_CDMA_MESSAGES and WMS_MAX_NV_GW_MESSAGES to
                   replace WMS_MAX_NV_MESSAGES.
03/26/03   HQ      Decreased MAX RAM messages from 99 to 50.
03/21/03   SUN     Increased WMS_MAX_RPTS  to 10
03/14/03   HQ      Use FEATURE_GSTK for gstk events.
02/27/03   HQ      Added support of FEATURE_GWSMS_MO_CONTROL.
02/03/03   HQ      Added support for MO SMS PS/CS domain preferences.
02/07/03   HQ      Increased CDMA NV SMS slots to 99.
02/07/03   SUN     Added Support for Status Reports and Commands
                   Mainlined FEATURE_GWSMS_ASYNC_INIT
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
12/13/02   SUN     Increased WMS_MAX_CMDS to 20
11/21/02   HQ      Added event reporting to Diag:
                   EVENT_MT_SMS_NOTIFY and EVENT_MO_SMS_STATUS.
11/13/02   AP      Added extern wms_msg_info_cache_cb_type to be used by other
                   modules.
11/09/02   HQ      Added new BC SMS commands: Delete All Services and
                   Set Priority For All.
10/25/02   AP      Fix compile error.  Feature-ize include "gsdi_exp.h".
10/12/02   HQ      Added support of FEATURE_GWSMS_ASYNC_SIM_INIT.
08/06/02   HQ      Added support of FEATURE_MULTIMODE_ARCH.
07/31/02   AP      Changed value of WMS_RUIM_ACCESS_SIG to fix conflict with
                   TASK_OFFLINE_SIG.
06/22/02   HQ      Added command wms_msg_delete_all() and
                   wms_msg_delete_template_all();
                   Added WMS_RUIM_ACCESS_SIG.
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
05/09/02   HQ      Added FEATURE_GWSMS_INIT_DELAY.
05/07/02   HQ      Added more support for FEATURE_CDSMS and Multimode.
03/09/02   HQ      Added support of FEATURE_CDSMS for Multimode.
02/25/02   SUN     Added UDL_MAX enum
01/25/02   HQ      Added WMS_MAX_NV_MESSAGES.
01/23/02   HQ      Changed max RAM messages to 99.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/9/01    HQ      Cleaned up compiler warnings.
06/14/01   HQ      Initial version for FEATURE_GWSMS.

===========================================================================*/


#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "wms.h"
#include "wmspm.h"
#include "wmspriv.h"
#ifndef WIN32
#include "rex.h"
#else
typedef int rex_timer_type;
rex_timer_type wms_rpt_timer;
typedef int rex_tcb_type;
typedef int rex_sigs_type;
#endif
#include "queue.h"
#include "cm.h"

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#endif /* FEATURE_GSTK */


/*===========================================================================
=============================================================================
========================  DATA TYPES  =======================================
=============================================================================
===========================================================================*/
/* Timer for kicking the WatchDog */
extern rex_timer_type  wms_rpt_timer;

#ifdef FEATURE_LOW_MEMORY_USAGE
enum { WMS_MAX_CMDS  = 10 };
enum { WMS_MAX_RPTS  = 10 };
#else
enum { WMS_MAX_CMDS  = 30 };
enum { WMS_MAX_RPTS  = 20 };
#endif

enum { WMS_MAX_RAM_MESSAGES = 50 };
enum { WMS_MAX_RAM_STATUS_REPORTS = 10 };

#ifdef CUST_EDITION
//  以前是提前5条提示，现在是提前10条提示 
#ifdef FEATURE_ONEMSG_USE_ONEFILE

#if defined(FEATURE_VERSION_EC99)||defined(FEATURE_VERSION_K212_HUALU)
    enum 
    { 
        IN_WATERMARK = 70,
        IN_MAX = 80  
    };
    enum 
    { 
        OUT_WATERMARK = 40,
        OUT_MAX = 50
    };

    enum { MAX_OEMTEMPLATES = 10};// OEM 消息常用语条数
    enum { PHRASE_MAX = 20 };
    enum { DRAFT_MAX = 50 };
    enum { BC_MAX = 50 };
    enum { RESERVE_MAX = 30 };
#else
    enum 
    { 
        IN_WATERMARK = 190,
        IN_MAX = 200  
    };
    enum 
    { 
        OUT_WATERMARK = 90,
        OUT_MAX = 100
    };

    enum { MAX_OEMTEMPLATES = 10};// OEM 消息常用语条数
    enum { PHRASE_MAX = 20 };
    enum { DRAFT_MAX = 50 };
    enum { BC_MAX = 50 };
    enum { RESERVE_MAX = 30 };
#endif




enum { WMS_MAX_NV_CDMA_MESSAGES     = (1+IN_MAX+OUT_MAX+PHRASE_MAX+DRAFT_MAX+BC_MAX+RESERVE_MAX) };
enum 
{ 
    VOICE_INDEX    = 0,                         // 语音短信索引值
    IN_START       = 1,                         // 收件信息索引开始值
    IN_END         = IN_MAX,                    // 收件信息索引结束值
    OUT_START      ,                            // 发件信息索引开始值
    OUT_END        = (IN_END+OUT_MAX),          // 发件信息索引结束值
    PHRASE_START   ,                            // 常用语信息索引开始值
    PHRASE_END     = (OUT_END+PHRASE_MAX),      // 常用语信息索引结束值
    DRAFT_START    ,                            // 草稿信息索引开始值
    DRAFT_END      = (PHRASE_END+DRAFT_MAX),    // 草稿信息索引结束值
    BC_START       ,                            // 广播信息索引开始值
    BC_END         = (DRAFT_END+BC_MAX),        // 广播信息索引结束值
    RESERVE_START  ,                            // 预约信息索引开始值
    RESERVE_END    = (BC_END+RESERVE_MAX),      // 预约信息索引结束值
};
    
#else
enum { WMS_MAX_NV_CDMA_MESSAGES = 99 }; /* slots 0..98 */
#endif
#endif /*CUST_EDITION*/

enum { WMS_MAX_GCF_NV_GW_MESSAGES = 23 }; /* slots 0..22 */
#ifdef FEATURE_SMS_500_MESSAGES
#error code not present
#else 
enum { WMS_MAX_NV_GW_MESSAGES   = 255 }; /* slots 0..254 */
#endif /* FEATURE_SMS_500_MESSAGES */
#ifdef CUST_EDITION // Gemsea Add for memory optimize
enum { WMS_MAX_CDMA_MESSAGES_IN_MEM =
         WMS_MAX_NV_CDMA_MESSAGES /*+ WMS_MESSAGE_LIST_MAX */};
enum { WMS_MAX_GW_MESSAGES_IN_MEM =
         WMS_MAX_NV_GW_MESSAGES /*+ WMS_MESSAGE_LIST_MAX */+ WMS_MAX_RAM_MESSAGES };
#else
enum { WMS_MAX_CDMA_MESSAGES_IN_MEM =
         WMS_MAX_NV_CDMA_MESSAGES + WMS_MESSAGE_LIST_MAX };
enum { WMS_MAX_GW_MESSAGES_IN_MEM =
         WMS_MAX_NV_GW_MESSAGES + WMS_MESSAGE_LIST_MAX + WMS_MAX_RAM_MESSAGES };
#endif
enum { WMS_SIM_CBMI_LIST_MAX  = 127 };
enum { WMS_SIM_CBMIR_LIST_MAX = 63 };
enum { WMS_SIM_CBMID_LIST_MAX = 127 };

enum { WMS_GW_CB_MAX_LANGUAGES = 255 };

enum { WMS_UDH_OTHER_SIZE_GW = 137 }; /* 140 - 1(UDHL) - 1(UDH ID) - 1(UDH Len) */

//enum { WMS_TL_MAX_LEN        = 246 };
#ifdef CUST_EDITION
#define WMS_GW_TEMPLATE_MIN         28
#endif /*CUST_EDITION*/
#define WMS_CARD_NUM_REFRESH_FILES 14

/* SMS task signals
*/
enum
{
  WMS_CMD_Q_SIG               =  0x001,
  WMS_MT_ACK_TIMER_SIG        =  0x002,
  WMS_DC_CALL_IN_TIMER_SIG    =  0x008,
  WMS_DC_CALL_OUT_TIMER_SIG   =  0x010,
  WMS_DC_IDLE_TIMER_SIG       =  0x020,
  WMS_AWISMS_ACK_TIMER_SIG    =  0x040,
  WMS_NV_SIG                  =  0x080,
  WMS_RPT_Q_SIG               =  0x100,
  WMS_RPT_TIMER_SIG           =  0x200,
  WMS_DELAY_TIMER_SIG         =  0x400,
  WMS_RESERVED_SIG            =  0x1000, /* reserved for SIM server */
  WMS_RUIM_ACCESS_SIG                 =  0x800,
  WMS_MO_RETRY_TIMER_SIG              =  0x10000
};


#define WMS_SIGNALS ( WMS_CMD_Q_SIG \
                        | WMS_MT_ACK_TIMER_SIG \
                        | WMS_DC_CALL_OUT_TIMER_SIG \
                        | WMS_DC_IDLE_TIMER_SIG \
                        | WMS_AWISMS_ACK_TIMER_SIG \
                        | WMS_NV_SIG \
                        | WMS_RPT_Q_SIG \
                        | WMS_RPT_TIMER_SIG \
                        | WMS_MO_RETRY_TIMER_SIG )


enum{ WMS_TIMER_RETRY_INTERVAL  = (5) /* seconds */ };
enum{ WMS_MAX_RETRY_PERIOD = (240) /* seconds */ };

#ifdef FEATURE_JCDMA_2
enum{ WMS_MAX_ACK_RETRY_PERIOD = (18) /* seconds */ };
#else
enum{ WMS_MAX_ACK_RETRY_PERIOD = (30) /* seconds */ };
#endif /* FEATURE_JCDMA_2 */


#ifdef FEATURE_CDSMS

/* Timer values
*/
enum{ WMS_TIMER_DC_CALL_OUT    = ( 63 * 1000 )  /* ms */ };

enum{ WMS_TIMER_MT_ACK          = 2000  /* ms */ };

enum{ WMS_TIMER_AWISMS_ACK      = 700  /* ms */ };

enum{ WMS_TIMER_MO_ACK          = ( 20 * 1000 )  /* ms */ };

enum{ WMS_TIMER_LAYER2          = ( 30 * 1000 )  /* ms */ };
enum{ WMS_TIMER_LAYER2_LONG     = ( 50 * 1000 )  /* ms */ };

#endif /* FEATURE_CDSMS */

/* Define the list of SMS bearers
*/
typedef enum
{
  WMS_BEARER_CDMA_1X,
  WMS_BEARER_CDMA_EVDO,
  WMS_BEARER_CDMA_WLAN,

  //GSM/UMTS bearers to be supported later:
  WMS_BEARER_GW_CS,
  WMS_BEARER_GW_PS,
  WMS_BEARER_GW_PS_SIP,
  WMS_BEARER_GW_WLAN,

  WMS_BEARER_MAX,
  WMS_BEARER_MAX32   = 0x1FFFFFFF
} wms_bearer_e_type; 

/* Define the usage preference for SMS bearers
*/
typedef enum
{
  WMS_BEARER_USAGE_PREFERRED,
  WMS_BEARER_USAGE_ALLOWED,
  WMS_BEARER_USAGE_BLOCKED,
  WMS_BEARER_USAGE_MAX,
  WMS_BEARER_USAGE_MAX32       = 0x1FFFFFFF
} wms_bearer_usage_e_type;

/* Define the SMS Bearer Preference array
*/
typedef wms_bearer_usage_e_type wms_bearer_prefs_arr_type[WMS_BEARER_MAX];



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Generic report header type - all reportss to WMS must start with this */
typedef struct
{
  q_link_type          link;         /* Queue link */
  rex_tcb_type         *task_ptr;    /* Pointer to requesting task TCB */
  rex_sigs_type        sigs;         /* Signal to set upon cmd completion */
  q_type               *done_q_ptr;  /* Queue to place this cmd on when done */

  wms_cmd_id_e_type         cmd_id;
  wms_client_id_type        client_id;
  wms_cmd_cb_type           cmd_cb;
  void                      * user_data;
} wms_cmd_hdr_type;



/*-------------------------------------------------------------------------*/

/* Configuration group commands
*/
typedef struct
{
  wms_routes_s_type                routes;
} wms_cmd_cfg_set_routes_type;


typedef struct
{
  int dummy;
} wms_cmd_cfg_get_routes_type;


typedef struct
{
  wms_memory_store_e_type          mem_store;
  wms_message_tag_e_type           tag;
} wms_cmd_cfg_get_mem_status_type;


typedef struct
{
  wms_memory_store_e_type          mem_store;
  wms_message_tag_e_type           tag;
} wms_cmd_cfg_get_msg_list_type;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

typedef struct
{
  boolean                          set_primary;
  boolean                          use_client_memory;
} wms_cmd_cfg_set_primary_client_type;

typedef struct
{
  boolean                          memory_full;
} wms_cmd_cfg_set_memory_full_type;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/* Message group commands
*/
typedef struct
{
  wms_send_mode_e_type               send_mode;
  wms_client_message_s_type          message;
} wms_cmd_msg_send_type;


typedef struct
{
  wms_ack_info_s_type             ack_info;
} wms_cmd_msg_ack_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_type;


typedef struct
{
  wms_write_mode_e_type              write_mode;
  wms_client_message_s_type          message;
} wms_cmd_msg_write_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_tag_e_type          tag;
} wms_cmd_msg_delete_all_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
  wms_message_tag_e_type          tag;
} wms_cmd_msg_modify_tag_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_template_type;


typedef struct
{
  wms_write_mode_e_type            write_mode;
  wms_client_message_s_type        message;
} wms_cmd_msg_write_template_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_template_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
} wms_cmd_msg_delete_template_all_type;

typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_read_sts_report_type;


typedef struct
{
  wms_client_message_s_type        message;
  wms_write_mode_e_type            write_mode;
} wms_cmd_msg_write_sts_report_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
  wms_message_index_type          index;
} wms_cmd_msg_delete_sts_report_type;


typedef struct
{
  wms_memory_store_e_type         mem_store;
} wms_cmd_msg_delete_sts_report_all_type;

#ifdef FEATURE_GSTK
typedef struct
{
  gstk_cmd_from_card_type   gstk_evt;
} wms_cmd_msg_gstk_evt_type;
#endif /* FEATURE_GSTK */


#ifdef FEATURE_CDSMS
/* DC commands
*/
typedef struct
{
  wms_dc_so_e_type     so;
} wms_cmd_dc_connect_type;

typedef struct
{
  uint32                 timeout;
} wms_cmd_dc_enable_auto_disc_type;
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_CDSMS_BROADCAST
typedef struct
{
  cm_sms_cmd_err_e_type       sms_cmd_err;
} wms_cmd_bc_enable_failure_type;
#endif /* FEATURE_CDSMS_BROADCAST */


#ifdef FEATURE_BROADCAST_SMS_MULTIMODE

/* BC MM commands
*/
typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_config_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_pref_e_type              bc_pref;
} wms_cmd_bc_mm_set_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_table_type;


typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_srv_id_type           srv_id;
  boolean                         selected;
} wms_cmd_bc_mm_select_srv_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_all_srv_ids_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_srv_id_type           srv_id;
} wms_cmd_bc_mm_get_srv_info_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  uint8                           num_entries;
  wms_bc_mm_service_info_s_type   * entries;
} wms_cmd_bc_mm_add_srv_type;


typedef struct
{
  wms_message_mode_e_type         message_mode;
  uint8                           num_entries;
  wms_bc_mm_srv_id_type           * srv_ids;
} wms_cmd_bc_mm_delete_srv_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_bc_mm_service_info_s_type   srv_info;
} wms_cmd_bc_mm_change_srv_info_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_delete_all_srvs_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  boolean                         selected;
} wms_cmd_bc_mm_select_all_srvs_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  wms_priority_e_type             priority;
} wms_cmd_bc_mm_set_priority_all_srvs_type;

typedef struct
{
  wms_gw_cb_page_header_s_type    page_hdr;
} wms_cmd_bc_mm_msg_delete_indication_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
} wms_cmd_bc_mm_get_reading_pref_type;

typedef struct
{
  wms_message_mode_e_type         message_mode;
  boolean                         reading_advised;
  boolean                         reading_optional;
} wms_cmd_bc_mm_set_reading_pref_type;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

typedef struct
{
  uint32                          retry_interval;
} wms_cmd_dbg_set_retry_interval_type;

#ifdef FEATURE_CDSMS

typedef struct
{
  wms_OTA_message_type      ota;
  wms_bearer_e_type         bearer;
  boolean                   ack_pending;
  wms_transaction_id_type   mt_iwmsc_tid;
} wms_cmd_mc_mt_msg_type;


typedef struct
{
  wms_error_class_e_type        error_class;
  wms_status_e_type             status;
  wms_bearer_e_type             bearer;
} wms_cmd_mc_mo_status_type;

typedef struct
{
  uint8                        msg_count;
} wms_cmd_mc_mwi;

typedef struct
{
  cm_call_cmd_e_type          call_cmd;
  cm_call_cmd_err_e_type      call_cmd_err;
} wms_cmd_call_error_type;
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS*/

#ifdef FEATURE_MMGSDI
typedef struct
{
  gsdi_sim_events_T   gsdi_event;
} wms_cmd_gsdi_card_status_cb_type;
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_CDSMS_BROADCAST
typedef struct
{
  wms_tl_message_type   cdma_tl;
} wms_cmd_bc_scpt_data_type;
#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_GSTK
typedef struct
{
  uint8               num_files;
  uim_items_enum_type file_list[WMS_CARD_NUM_REFRESH_FILES];
}wms_cmd_gsdi_refresh_fcn_ind_type;
#endif /* FEATURE_GSTK */

typedef struct
{
  boolean       cdma_ready;
  boolean       gw_ready;
} wms_cmd_cm_start_full_service_type;

typedef struct
{
  boolean       in_lpm;
} wms_cmd_cm_oprt_mode;

typedef struct
{
  sys_sys_mode_e_type          sys_mode;
  sys_srv_domain_e_type        srv_domain;
  sys_srv_status_e_type        srv_status;
  boolean                      umts_plmn_changed;
#ifdef CM_API_PLUS_DIALING
  cm_hs_based_plus_dial_e_type hs_based_plus_dial_setting;
  cm_country_code_type         home_mobile_country_code;
  cm_country_code_type         current_mobile_country_code;
#endif /* CM_API_PLUS_DIALING */
} wms_cmd_ss_change_info_s_type;

/* Union of all command types.  The header is always present and           */
/* specifies the command type and attributes.  If the command has          */
/* arguments, they follow in the union.                                    */

#ifdef CUST_EDITION
typedef struct
{
  boolean                          multisend;
} wms_cmd_cfg_set_multisend_type;

typedef struct
{
    wms_deletebox_e_type box_deltype;
} wms_cmd_msg_delete_box_type;

typedef struct
{
    wms_msg_copy_type             copypram;
} wms_cmd_msg_copy_type;


#endif


typedef struct
{
  wms_cmd_hdr_type                    hdr;       /* header */

  union
  {
    /* Configuration group commands
    */
    wms_cmd_cfg_set_routes_type       cfg_set_routes;
    wms_cmd_cfg_get_routes_type       cfg_get_routes;
    wms_cmd_cfg_get_mem_status_type   cfg_get_mem_status;
    wms_cmd_cfg_get_msg_list_type     cfg_get_msg_list;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    wms_cmd_cfg_set_primary_client_type cfg_set_primary_client;
    wms_cmd_cfg_set_memory_full_type  cfg_set_memory_full;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
#ifdef CUST_EDITION
    wms_cmd_cfg_set_multisend_type    cfg_set_multisend;
#endif

    /* Message group commands
    */
    wms_cmd_msg_send_type             msg_send;
    wms_cmd_msg_ack_type              msg_ack;
    wms_cmd_msg_read_type             msg_read;
    wms_cmd_msg_write_type            msg_write;
    wms_cmd_msg_delete_type           msg_delete;
    wms_cmd_msg_delete_all_type       msg_delete_all;
#ifdef CUST_EDITION	
    wms_cmd_msg_delete_box_type       msg_delete_box;
    wms_cmd_msg_copy_type             msg_copy;
#endif /*CUST_EDITION*/	
    wms_cmd_msg_modify_tag_type       msg_modify_tag;
    wms_cmd_msg_read_template_type    msg_read_template;
    wms_cmd_msg_write_template_type   msg_write_template;
    wms_cmd_msg_delete_template_type  msg_delete_template;
    wms_cmd_msg_delete_template_all_type msg_delete_template_all;
    wms_cmd_msg_read_sts_report_type       msg_read_sts_report;
    wms_cmd_msg_write_sts_report_type      msg_write_sts_report;
    wms_cmd_msg_delete_sts_report_type     msg_delete_sts_report;
    wms_cmd_msg_delete_sts_report_all_type msg_delete_sts_report_all;

#ifdef FEATURE_GSTK
    wms_cmd_msg_gstk_evt_type              msg_gstk_evt;
#endif


    /* DC commands
    */
#ifdef FEATURE_CDSMS
    wms_cmd_dc_connect_type           dc_connect;
    wms_cmd_dc_enable_auto_disc_type  dc_enable_auto_disc;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_CDSMS_BROADCAST
    wms_cmd_bc_enable_failure_type    bc_enable_failure; /* from CM */
    wms_cmd_bc_scpt_data_type         bc_scpt_data; /* from WMS itself */
#endif /* FEATURE_CDSMS_BROADCAST */


#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
    wms_cmd_bc_mm_get_config_type       bc_mm_get_config;
    wms_cmd_bc_mm_get_pref_type         bc_mm_get_pref;
    wms_cmd_bc_mm_set_pref_type         bc_mm_set_pref;
    wms_cmd_bc_mm_get_table_type        bc_mm_get_table;
    wms_cmd_bc_mm_select_srv_type       bc_mm_select_srv;
    wms_cmd_bc_mm_get_all_srv_ids_type  bc_mm_get_all_srv_ids;
    wms_cmd_bc_mm_get_srv_info_type     bc_mm_get_srv_info;
    wms_cmd_bc_mm_add_srv_type          bc_mm_add_srv;
    wms_cmd_bc_mm_delete_srv_type       bc_mm_delete_srv;
    wms_cmd_bc_mm_change_srv_info_type  bc_mm_change_srv_info;
    wms_cmd_bc_mm_delete_all_srvs_type  bc_mm_delete_all_srvs;
    wms_cmd_bc_mm_select_all_srvs_type  bc_mm_select_all_srvs;
    wms_cmd_bc_mm_set_priority_all_srvs_type bc_mm_set_priority_all_srvs;
    wms_cmd_bc_mm_msg_delete_indication_type bc_mm_msg_delete_indication;
    wms_cmd_bc_mm_get_reading_pref_type bc_mm_get_reading_pref;
    wms_cmd_bc_mm_set_reading_pref_type bc_mm_set_reading_pref;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */


    /* DBG commands
    */
    wms_cmd_dbg_set_retry_interval_type dbg_set_retry_interval;

#ifdef FEATURE_CDSMS
    /* events from MC through CM
    */
    wms_cmd_mc_mt_msg_type                  mt_msg;
    wms_cmd_mc_mo_status_type               mc_mo_status;
    wms_cmd_mc_mwi                          mwi;

    /* Call events from CM
    */
    wms_cmd_call_error_type                 call_error;
#ifdef FEATURE_MULTIMODE_ARCH
    cm_mm_call_info_s_type                  call_info;
#else /* FEATURE_MULTIMODE_ARCH */
    cm_call_info_s_type                     call_info;
#endif /* FEATURE_MULTIMODE_ARCH */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_MMGSDI
    wms_cmd_gsdi_card_status_cb_type        gsdi_card_status_cb;
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GSTK
    wms_cmd_gsdi_refresh_fcn_ind_type       gsdi_refresh_fcn_ind;
#endif /* FEATURE_GSTK */

    wms_cmd_cm_start_full_service_type      start_full_service;

    wms_cmd_cm_oprt_mode                    oprt_mode;

    wms_cmd_ss_change_info_s_type           ss_change_info;
    cm_ipapp_info_s_type                    ipapp_change_info;

  } cmd;

}  wms_cmd_type;

/* Message info cache: This callback will stand by itself, not part of normal
** registration with WMS.  The purpose of this is to relieve any timing
** constraints between client registration and WMS initialization.
*/
extern wms_msg_info_cache_cb_type    wms_msg_info_cache_cb;

/* Event payload to Diag
*/
typedef PACKED struct
{
  uint8    mem_type;
  uint32   msg_index;
} wms_evt_mt_sms_notify_s_type;

typedef PACKED struct
{
  uint32   transaction_id;
  uint8    mem_type;
  uint32   msg_index;
  uint8    report_status;
  uint8    cause_code;
} wms_evt_mo_sms_status_s_type;

typedef PACKED struct
{
  uint32   transaction_id;
  uint32   record_id;
  uint8    prev_report_status;
  uint8    prev_cause_code;
  uint32   retry_time_left;
} wms_evt_mo_sms_retry_attempt_s_type;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

void wms_task( uint32 dummy );


/*=========================================================================
FUNCTION
  wms_client_cmd_status

DESCRIPTION
  Issue client command status (error or no-error) to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_client_cmd_status
(
  wms_cmd_type        *cmd_ptr,
  wms_cmd_err_e_type  cmd_err
);



/*===========================================================================

FUNCTION wms_get_cmd_buf

DESCRIPTION
  Allocate a WMS Task command buffer.

DEPENDENCIES
  wms_init() was done.

RETURN VALUE
  Pointer to allocated WMS command buffer.
  NULL pointer is returned if no more buffer is available.

SIDE EFFECTS
  Remove a command from the free queue.

===========================================================================*/
wms_cmd_type *wms_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION wms_put_cmd

DESCRIPTION
  The API functions calls uses this function to put commands to the WMS.

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Add a command to the command queue.

===========================================================================*/
void wms_put_cmd
(
  wms_cmd_type  *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION wms_get_cmd_count

DESCRIPTION
  The API function call returns the number of commands to WMS

DEPENDENCIES
  wms_cmd_q already initialized.

RETURN VALUE
  Count of the number of commands

SIDE EFFECTS
  None.

===========================================================================*/
int wms_get_cmd_count
(
  void
);

/* <EJECT> */
/*===========================================================================
FUNCTION WMS_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void wms_kick_dog
(
  void
);

/*===========================================================================
FUNCTION WMSI_GET_CARD_PRESENCE

DESCRIPTION
  Get the presence information for RUIM and SIM Cards.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wmsi_get_card_presence(boolean* pbRUIM, boolean* pbSIM);

#ifdef FEATURE_GSTK

/*==============================================================================
FUNCTION NAME WMS_CARD_REGISTER_FOR_REFRESH

DESCRIPTION
  Will Register WMS for File Change Notifications and Voting Privledges.

RETURN VALUE
  void

DEPENDENCIES
  None
==============================================================================*/
void wms_card_register_for_refresh
(
  void
);

/*===========================================================================
FUNCTION WMS_CARD_REFRESH_FCN_IND_PROC

DESCRIPTION
  Called during WMS Command Buffer processing to refresh the File List
  returned to it by GSDI_REFRESH

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_card_refresh_fcn_ind_proc
(
  wms_cmd_type   * cmd_ptr
);

/*===========================================================================
FUNCTION WMS_CARD_REFRESH_COMPLETE

DESCRIPTION
  Called within WMS to indicate that it has completed reading the files it
  was notified to read.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the WMS task.
===========================================================================*/
void wms_card_refresh_complete(
  boolean               pass_fail
);

#endif /* FEATURE_GSTK */

/*===========================================================================

FUNCTION wms_init

DESCRIPTION
  Initialize the WMS module.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Initialization to internal data is done.

===========================================================================*/
void wms_init
(
  void
);

/*=========================================================================
FUNCTION
  wms_process_signals

DESCRIPTION
  Process the signals for SMS related commands and timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_process_signals
(
  rex_sigs_type              sigs
);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSI_H */


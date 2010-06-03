#ifndef DSTASK_H
#define DSTASK_H
/*===========================================================================

                     D A T A   S E R V I C E S   T A S K

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the Data Services (DS) Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the Data Services Task.

  Signal and command definitions for the Data Services Task, and functions
  that should be used by other tasks to send commands to the Data Services
  Task are defined here.


   Copyright (c) 2001 - 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dstask.h_v   1.17   28 Feb 2003 18:56:06   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/dstask.h#5 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands.
01/02/09   bs      Added support for 1X AT Phonebook commands.
09/07/08   cs      Fixed featurization compilation warnings..
07/09/08   bs      Added support for CDMA NAARefresh.
01/28/08   sn      Featurized WCDMA/GSM code.
10/15/07 sq/mga/yz Merged changes to support FEATURE_UIM_SUPPORT_3GPD
                   and made changes to solve compilation error.
11/04/04   gr      Added PZID_HYS_SDB_DATA_CMD to notify the hysteresis 
                   engine whenever SDB packet data is exchanged
10/12/04   vr      Added PZID_HYS_DATA_READY_CMD to notify the hysteresis 
                   engine whenever PPP data is exchanged
05/12/04   sy      Added pzid hysteresis timer expired command.
10/01/03   ak      Added PZID Delay timer command.
08/24/03   ak      New cmds for IFACE_UP, _DOWN and _ROUTEABLE indications
                   for 1X packet.
05/21/03   vas     Added HDR commands.
04/18/03   sb      Partition CM supserv event info to conserve storage.
04/18/03   vsk     updated to use new UMTS RMSM cmd and names. 
                   added info_ptr to dsumtsps_call_info_type
04/11/03   ak      Updated to use new 707 RMSM CMD names.
02/26/03   rc      Added field ps_data_suspend to ds_ss_info_type.Renamed 
                   DS_CM_SS_SYS_MODE_CHG_CMD as DS_CM_SS_SRV_CHG_CMD.
01/10/03   sb      Event handler declarations for +CGATT command
11/14/02   jd      Removed DS_RMSM_UM_PPP_UP_CMD (not used)
11/13/02   rc      Added multimode support for GPRS.
11/12/02   ar      Add support for supplementary services events
11/11/02   sb      Call Related CM events now apply in all modes
10/30/02   rsl     Made call_cmd and call_event common.
10/14/02   ak      Added client_id to call_info_type.  This helps support
                   JCDMA.
10/14/02   ak      Removed JCDMA commands - no longer needed.
10/14/02   sb      Merge from ETSI. PBM event declarations and state machine.
10/14/02   vsk     Added support to get statistics at end of call
10/10/02   ak      Updated for JCDMA 1X.
10/04/02   usb     Added Generic RM State Machine specific commands
10/03/02   rc      Added UMTS specific command handling.
08/28/02   usb     Added/renamed RMSM cmd names
08/14/02   atp     Added DS_707_ASYNC_ATZ_CMD.
08/13/02   dgy     Added return value to ds_get_call_information() and
                   ds_get_current_session_data_counters().
08/02/02   dgy     Added support for UI data call screen.
08/01/02   rc      Removed FEATURE_DS_SOCKETS wrap around socket commands.
                   Added DS_FLOW_CTRL_DSMGR_MASK to sockets flow control mask.
07/29/02   ak      Added use_hdown_timer field to ds707_pkt_call_info_type.
07/24/02   atp     Added DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD. 
07/22/02   ak      Added DS_707_PKT_IFACE_DOWN_CBACK_CMD.
07/19/02   aku     Moved flow control masks to ds_flow_control.h
07/16/02   aku     Removed SOCKET featurization for dssnet flow mask.
07/13/02   atp     Added commands for 707 async.
07/01/02   ak      Added signal for re-origing a 707 pkt call.
06/18/02   tmr     Added DS_COMPLETE_LL_DISCONNECT_CMD, DS_GCSD_RX_DCD_ON_CMD and
                   DS_GCSD_TX_DATA_FLUSHED_CMD 
05/21/02   smp     Added DS_CM_SS_SYS_MODE_CHG_CMD.
05/21/02   ak      #ifdef'd some RAB stuff since CM does it.  Should soon
                   go away.  But this helps compile.
05/20/02   usb     Added new command ids for RMSM (IS707 pkt call Rm handler)
05/16/02   smp     Updates for Multimode CM.
05/08/02   ak      Added end_status to call_info type.
05/08/02    wx     Remove FEATURE_DSAT_TIMER
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added DS_INITIATE_CALL_CMD, DS_ENTER_SOCKET_MODE_CMD,
                   DS_EXIT_SOCKET_MODE_CMD, DS_ORIG_SOCKET_NET_CMD,
                   DS_RELEASE_SOCKET_NET_CMD. Added ds_call_type_e_type and 
                   ds_call_stack_e_type and ds_orig_call_type.
04/04/02   tmr     Modified definitions of DS_TASK_START_SIG, 
                   DS_TASK_STOP_SIG and DS_TASK_OFFLINE_SIG
02/26/02   tmr     Added DS_COMPLETE_LL_CONNECT_CMD, DS_RELEASE_CALL_CMD 
                   and DS_GCSD_XID_NEG_COMPL_CMD. 
02/05/02   rlp     Added online-command mode support.
01/11/02   smp     Merged from packet data branch. Added call_type parameter
                   for commands received from CM.
11/21/01   tmr     Added DS_GCSD_DISCONNECT_CMD command ID
10/31/01   smp     Removed extra comma from ds_cmd_enum_type.
08/31/01   smp     Updates due to CM name changes.
05/24/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "queue.h"
#include "sio.h"
#include "cm.h"
#include "nv.h"
#include "task.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI)
#include "gsdi_exp.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI) */

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#endif

/* this file should be included only in GSM or WCDMA mode */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "wms.h"
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "uiupbm.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
extern nv_esn_type     ds_esn;  /* Mobile's ESN value.  Used by an AT cmd  */

/*---------------------------------------------------------------------------
  Data Services Task Signal Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Task top-level signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_TASK_START_SIG       TASK_START_SIG         /* 0x8000           */
#define DS_TASK_STOP_SIG        TASK_STOP_SIG          /* 0x4000           */
#define DS_TASK_OFFLINE_SIG     TASK_OFFLINE_SIG       /* 0x2000           */
#define DS_CMD_Q_SIG            0x1000   /* Command Queue signal           */
#define DS_DOG_RPT_TIMER_SIG    0x0800   /* Watchdog Report timeout signal */
#define DS_NV_CMD_SIG           0x0400   /* NV item retrieval signal       */
#define DS_TIMER_SIG            0x0200   /* Timer signal                   */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  IS-707 Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_PKT_REORIG_SIG       0x0100   /* called when want to reorig     */ 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Serial Port signals 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define DS_1ST_SIO_RX_SIG       0x0008   /* 1st byte into serial rx buffer */
#define DS_SIO_RX_PKT_SIG       0x0004   /* Protocol packet rx'ed          */
#define DS_SIO_ESCAPE_SIG       0x0002   /* AT escape sequence detected    */
#define DS_DTR_CHANGED_SIG      0x0001   /* DTR pin changed state          */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  ATCOP Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_AT_MT_MSG_SIG        0x10000  /* MT message received            */ 
#define DS_AT_SMS_SIG           0x20000  /* Other, non-"MT msg" SMS event  */ 
#define DS_AT_CM_MSG_SIG        0x40000  /* CM message received            */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  UCSD Sub-task signals
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define DS_UCSD_GSDI_MSG_SIG    0x00010  /* GSDI message received          */ 
#define DS_UIM_CMD_SIG          0x00020  /* UIM Command Signal             */

/*---------------------------------------------------------------------------
  Data call types: circuit-switched or packet. In each mode, circuit-switched
  and packet data calls may be handled by different call control modules.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_CALL_TYPE_CKT,                          /* Circuit-switched data call */
  DS_CALL_TYPE_PKT,                                    /* Packet data call */
  DS_NUM_CALL_TYPES             /* !! For internal use of 3G Dsmgr only !! */
} ds_call_type_e_type;

/*---------------------------------------------------------------------------
  Data statistics types are used by Data Servies to indicate to UI if the
  data statistics UI receives is valid or not. If the data stats is not
  valid, UI should properly handle such case. The data stats types are:
  
  DS_STATS_VALID            - Valid data statistics
  DS_STATS_INVALID_CALL_ID  - Invalid call ID
  DS_STATS_UNAVAILABLE      - Data statistics are unavailable
---------------------------------------------------------------------------*/
typedef enum
{
  DS_STATS_VALID,                                 /* Valid data statistics */
  DS_STATS_INVALID_CALL_ID,                             /* Invalid call ID */
  DS_STATS_UNAVAILABLE                  /* Data statistics are unavailable */
} ds_stats_e_type;

/*---------------------------------------------------------------------------
  Data Services Task Command Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Commands that may be sent to the Data Services Task.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
  DS_MIN_CMD = -1,
  
  /* 3G Dsmgr sub-task commands */

  DS_RDM_OPEN_CMD,           /* Command from RDM to open a serial port     */
  DS_RDM_CLOSE_CMD,          /* Command from RDM to close a serial port    */
  DS_CM_CALL_END_CMD,        /* Notification of call end                   */
  DS_CM_CALL_INCOM_CMD,      /* Notification of incoming call              */
  DS_CM_CALL_CONNECTED_CMD,  /* Notification that call is connected        */
  DS_CM_CALL_SETUP_CMD,      /* Notification of incoming setup indication  */
  DS_CM_CALL_CONF_CMD,       /* Confirmation that MO call is proceeding    */
  DS_CM_SS_SRV_CHG_CMD,      /* Notification of srv change                 */
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
  DS_TIMER_EXPIRED_CMD,      /* Notification of timer expiry               */

  DS_INITIATE_CALL_CMD,      /* Command to Initiate a call                 */
  DS_RELEASE_CALL_CMD,       /* Command to Release a call                  */

  /*-------------------------------------------------------------------------
    ATCoP online mode commands
  -------------------------------------------------------------------------*/
  DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD,    /* Goto online-cmd flush TX         */
  DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD, /* Goto online-cmd, don't flush TX  */
  DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD,   /* Goto online-data, flush TX       */
  DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD,/* Goto online-data, don't flush TX */
  DS_COMPLETE_ONLINE_CMD_SWITCH_CMD,   /* OK to finish online-cmd switch   */
  DS_COMPLETE_ONLINE_DATA_SWITCH_CMD,  /* OK to finish online-data switch  */
  DS_COMPLETE_LL_CONNECT_CMD,     /* Complete low-level call connection    */
  DS_COMPLETE_LL_DISCONNECT_CMD,  /* Complete low-level call disconnection */

/*===========================================================================
  START of ATCoP asynchronous events
===========================================================================*/
  DS_AT_ASYNC_EVENT_START,           /* Start of ATCoP asynchronous events */

  /*-------------------------------------------------------------------------
    WCDMA and GSM asynchronous AT events/commands
  -------------------------------------------------------------------------*/
  DS_AT_STATUS_CMD,        /* used to report the excution status of an     */
                           /* async func call.  The callback function of   */
                           /* the  async command submit this cmd.          */
  DS_AT_CM_CALL_CMD,       /* Call feedback from CM                        */
  DS_AT_CM_CALL_INFO_CMD,  /* Call information from CM                     */
  DS_AT_CM_SS_CMD,         /* Serving System feedback from CM              */
  DS_AT_CM_SS_INFO_CMD,    /* Serving System information from CM           */
#ifdef FEATURE_ETSI_ATTACH
  DS_AT_CM_PH_CMD,       /* Attach/Detach Feedback from CM               */
#endif /* FEATURE_ETSI_ATTACH */
  DS_AT_GSDI_INFO_CMD,     /* SIM Event from GSDI                          */
#ifdef FEATURE_MMGSDI
  DS_AT_MMGSDI_INFO_CMD,     /* RUIM response from MMGSDI */
#endif /* FEATURE_MMGSDI */
  DS_AT_TIMER_EXPIRED_CMD, /* a atcop timer expire                         */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  DS_AT_PBM_CB_CMD,                /* Pbm async function's call back
                                      function submits this command to send
                                      the results to ATCOP
                                   */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */


#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  DS_AT_SMS_ERR_CMD,    /* Error Event from UASMS                          */
  DS_AT_SMS_MSG_CMD,    /* Message Event from UASMS                        */
  DS_AT_SMS_CFG_CMD,    /* Configuration Event from UASMS                  */
  DS_AT_SMS_ABT_CMD,    /* Event to tell ATCOP to abort command            */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

  /*-------------------------------------------------------------------------
    GSM FAX asynchronous AT events/commands
  -------------------------------------------------------------------------*/
  DS_AT_FPS_T31_CMD_STATUS,     /* GSM Fax command status                  */

#ifdef FEATURE_DSAT_FLOW_CONTROL
  DS_AT_TO_SIO_LOW_CMD,                    /* ds-to-sio watermark hits low */
#endif /* FEATURE_DSAT_FLOW_CONTROL */

#ifdef FEATURE_DATA_ETSI_SUPSERV 
  DS_AT_CM_SUPS_CMD,               /* Supplemental Services status from CM */
  DS_AT_CM_SUPS_INFO_CMD,        /* Supplemental Services response from CM */
#endif /* FEATURE_DATA_ETSI_SUPSERV */

  DS_AT_ASYNC_EVENT_END,        /* End of ATCoP asynchronous events        */
/*===========================================================================
  END of ATCoP asynchronous events
===========================================================================*/

#if ((defined(FEATURE_WCDMA) && defined(FEATURE_DATA_WCDMA_CS)) || \
     (defined(FEATURE_GSM) && defined(FEATURE_DATA_GCSD)))
#error code not present
#endif
  
  /*-------------------------------------------------------------------------
    IS707 Packet Data sub-task Commands
  -------------------------------------------------------------------------*/
  DS_707_PKT_PZID_CHANGE_CMD,             /* PZID has changed              */
  DS_707_PKT_SID_CHANGE_CMD,              /* SID has changed               */
  DS_707_PKT_NID_CHANGE_CMD,              /* NID has changed               */

  DS_707_PKT_CTA_TIMER_EXPIRED_CMD,       /* AT+CTA (idle timeout) timer   */
  DS_707_PKT_HOLDDOWN_TIMER_EXPIRED_CMD,  /* holddown timer expired        */
  DS_707_PKT_PZID_DELAY_TIMER_EXPIRED_CMD,/* pzid delay timer expired      */
  DS_707_PKT_PZID_HYSTERESIS_TIMER_EXPIRED_CMD, 
                                          /* pzid hysteresis timer expired */
  DS_707_PKT_PZID_HYS_DATA_READY_CMD,     /* to notify pzid hyst engine    */
  DS_707_PKT_PZID_HYS_SDB_DATA_CMD,       /* about packet data exchange    */

  DS_707_PKT_PHYS_LINK_UP_CMD,            /* bring up phys link            */
  DS_707_PKT_PHYS_LINK_DOWN_CMD,          /* tear down phys link           */
  DS_707_PKT_IFACE_UP_CMD,                /* bring up 1x pkt iface         */
  DS_707_PKT_IFACE_DOWN_CMD,              /* tear down 1x pkt iface        */
  DS_707_PKT_IFACE_DOWN_IND_CBACK_CMD,    /* 1x pkt iface is down ind cback*/
  DS_707_PKT_IFACE_UP_IND_CBACK_CMD,      /* 1x pkt iface is up ind cback  */
  DS_707_PKT_IFACE_ROUTEABLE_IND_CBACK_CMD,/*1x pkt iface routeable cback  */

  DS_707_TOGGLE_QNC_ENABLE_CMD,           /* toggle QNC setting - UI       */

  /*-------------------------------------------------------------------------
    IS707 Packet Data Rm State Machine sub-task Commands
  -------------------------------------------------------------------------*/
  DS_707_RMSM_RM_WANTS_PKT_CALL_CMD,
  DS_707_RMSM_RM_IFACE_DOWN_CMD,
  DS_707_RMSM_UM_IFACE_DOWN_CMD,
  DS_707_RMSM_UM_PHYS_LINK_UP_CMD,
  DS_707_RMSM_UM_PHYS_LINK_DOWN_CMD,
  DS_707_RMSM_UM_PPP_DOWN_CMD,
  DS_707_RMSM_RM_PPP_UP_CMD,
  DS_707_RMSM_UM_MIP_UP_CMD,
  DS_707_RMSM_UM_MIP_DOWN_CMD,

  /*-------------------------------------------------------------------------
    IS707 Async Data sub-task Commands
  -------------------------------------------------------------------------*/
  DS_707_ASYNC_IFACE_BRING_UP_CMD,              /* bring up 1x async iface */
  DS_707_ASYNC_IFACE_TEAR_DOWN_CMD,             /* tear down 1x async iface*/
  DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD, /*tear down physlink 1x async iface*/
  DS_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD,  /* Open protocols timer    */
  DS_707_ASYNC_PTCL_OPENED_CMD,              /* PS informs protocols opened*/
  DS_707_ASYNC_PTCL_CLOSED_CMD,              /* PS informs protocols closed*/
  DS_707_ASYNC_ATZ_CMD,                      /* ATZ reflected cmd          */

#if (defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS))
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

  /*-------------------------------------------------------------------------
    HDR Commands
  -------------------------------------------------------------------------*/
  DS_HDR_EVENT_CMD,

  /*-------------------------------------------------------------------------
    HDR AN Authentication Module Commands
  -------------------------------------------------------------------------*/
  DS_HDR_AN_CALL_END_CMD,
  DS_HDR_AN_CALL_CONNECTED_CMD,

  DS_MAX_CMDS
} ds_cmd_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  q_link_type       link;                               /* Queue link type */
  ds_cmd_enum_type  cmd_id;                      /* Identifies the command */
} ds_cmd_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Data types
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
  Structure is used when calling function in 3G DSMGR.  These structures
  are then passed right back to this interface.  Contains info that is
  important in configuring the response to orig or end a call.  Specific
  to IS-707 data
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                drs_bit;          /* contains DRS bit value       */
  boolean                use_hdown_timer;  /* use the holddown timer?      */
  uint32                 pkt_instance;     /* pkt instance to work on      */
} ds707_pkt_call_info_type;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */


/* Type for DS3G_INITIATE_CALL */

typedef struct
{
  ds_call_type_e_type    call_type;        /* Call type, circuit or packet */
  const byte            *dial_string;      /* Null-terminated dial string  */
  boolean                digit_mode;       /* 1 = dial str contains non-   */
                                           /*     digit chars              */
                                           /* 0 = dial str contains only   */
                                           /*     digits                   */
} ds_orig_call_type;


/* Type for DS_RDM_OPEN_CMD */

typedef struct
{
  sio_port_id_type  port_id;           /* Port id of the port to be opened */
} ds_rdm_open_type;


/* Type for: DS_CM_CALL_END_CMD,
             DS_CM_CALL_INCOM_CMD,
             DS_CM_CALL_CONNECTED_CMD,
             DS_CM_CALL_SETUP_CMD,
             DS_CM_CALL_CONF_CMD       */

typedef struct
{
  byte                       call_id;    /* Call id of the call            */
  cm_client_id_type          client_id;  /* who issued this cmd            */
  cm_call_type_e_type        call_type;  /* Call type, circuit or packet   */
  cm_call_mode_info_s_type   mode_info;  /* Call info, depends on the mode */
  cm_call_end_e_type         end_status; /* why call ended                 */
} ds_call_info_type;

/* Type for DS_CM_SS_SRV_CHG_CMD */

typedef struct
{
  sys_sys_mode_e_type         sys_mode;             /* Current system mode */
  boolean                     ps_data_suspend;      /* Suspend State - TRUE
                                                       or FALSE            */
} ds_ss_info_type;

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

/* Type for DS_TIMER_EXPIRED_CMD */

typedef struct
{
  unsigned long  timer_id;           /* Timer id of the timer that expired */
} ds_timer_type;

/* DS_AT_CM_CALL_CMD data type,
   report call command execution status */
typedef struct
{
  void                   *data_ptr;
  cm_call_cmd_e_type      cmd;
  cm_call_cmd_err_e_type  cmd_err;
} ds_at_cm_call_cmd_type;

/* DS_AT_CM_CALL_INFO_CMD data type,
   convery call event info to atcop */
typedef struct
{
  cm_call_event_e_type      event;
  cm_mm_call_info_s_type    event_info;
} ds_at_cm_call_event_type;

/* Type for DS_AT_STATUS_CMD */
typedef struct
{
  int8          cmd_id;  /* identify which cpb callback function
                             submitted this cmd */
  int16         cmd_status; /* contains the data for the command */
  byte          cmd_data;   /* contains the data for the command */
} ds_at_cmd_status_type;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* DS_AT_PBM_CB_CMD data type,
   return pbm function cb results from pbm to atcop */
typedef struct
{
  pbm_device_type pbm_device; /* the storage type */
  int used;                   /* number of used entries for 
                                 the storage type */
  int total;                  /* number of total entries for 
                                 the storage type */
  int text_len;               /* Length of text field. */
} ds_at_pbm_file_info_type;

typedef struct
{
  int8          cmd;          /* Pbm call back command identifier.
                                 It determines what type of cmd_info 
                                 is contained in cmd_info union */
  pbm_return_T status;        /* Command status of the pbm cb function */
  union
  {
    ds_at_pbm_file_info_type file_info; /* used for pbm_file_info_cb 
                                           function */
    pbm_record_s_type        record;    /* used for pbm read or find
                                           call back function */
  } cmd_info;                 /* pbm call back command results */
} ds_at_pbm_info_type;

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI)
/* DS_AT_GSDI_INFO_CMD data type,
   convery SIM Event from GSDI to atcop */
typedef struct
{
  gsdi_sim_events_T      event;
} ds_at_gsdi_event_type;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI) */
/* SMS Event handler data structures */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
typedef struct
{
  wms_cmd_id_e_type cmd;
  wms_cmd_err_e_type cmd_info;
}ds_at_sms_cmd_type;

typedef struct
{
  wms_cfg_event_e_type event;
  wms_cfg_event_info_s_type event_info;
}ds_at_sms_cfg_type;

typedef struct
{
  wms_msg_event_e_type event;
  wms_msg_event_info_s_type event_info;
}ds_at_sms_msg_type;

/* SMS/ATCOP Abort command handler */
typedef struct
{
  int16         cmd_status;
  int           cms_error;
} ds_at_sms_abt_type;

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_DATA_GCSD_FAX
#error code not present
#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_MMGSDI
/* DS_AT_MMGSDI_INFO_CMD data type */
#ifdef FEATURE_RUIM_CDMA_REFRESH
/* dsat_fcn_ind_type holds the FCN details returned
   by GSDI_REFRESH_FCN call back */
#define DSAT_MAX_NUM_RUIM_REFRESH_FILES  2

typedef struct 
{
  uint32                num_files; /* number of files in FCN ind */
  mmgsdi_file_enum_type  file_list[DSAT_MAX_NUM_RUIM_REFRESH_FILES]; /* file list */
} dsat_fcn_ind_type;
#endif /* FEATURE_RUIM_CDMA_REFRESH */

typedef struct
{
  mmgsdi_events_enum_type mmgsdi_event;  /* client event */
  int16           cmd_status;  /* contains the status for the command */
#ifdef FEATURE_RUIM_CDMA_REFRESH
    union 
    {
    dsat_fcn_ind_type   fcn_ind;/* FCN details */
    }u;
#endif /*FEATURE_RUIM_CDMA_REFRESH */
} ds_at_mmgsdi_event_type;
#endif /* FEATURE_MMGSDI */

/* Type for DS_RELEASE_CALL_CMD */
typedef struct
{
   cm_end_params_s_type end_params;
} ds_release_call_ind_type; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type: The command header identifies the command. If the command has
  any parameters, they are specified in the union.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  ds_cmd_hdr_type    hdr;                                /* Command header */
  
  union
  {
    ds_rdm_open_type          rdm_open;
    ds_call_info_type         call_info;
    ds_ss_info_type           ss_info;
    ds_timer_type             timer;
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
    ds_at_cm_call_cmd_type    call_cmd;
    ds_at_cm_call_event_type  call_event;
    ds_at_cmd_status_type     status_cmd;   /* used to report the excution */
                                            /* status of a async function  */
                                            /* call.  The callback function*/
                                            /* of the async command submit */
                                            /* this cmd.                   */

    unsigned int                dsat_timer_id;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI)
	ds_at_gsdi_event_type       gsdi_event;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_MMGSDI) */
#ifdef FEATURE_MMGSDI
    ds_at_mmgsdi_event_type   mmgsdi_event_info;
#endif /* FEATURE_MMGSDI */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
    ds_at_pbm_info_type         pbm_info;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
    ds_at_sms_cmd_type        sms_cmd;
    ds_at_sms_cfg_type        sms_cfg;
    ds_at_sms_msg_type        sms_msg;
    ds_at_sms_abt_type        sms_abt;
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

#ifdef FEATURE_DATA_GCSD_FAX
#error code not present
#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_DATA_IS707
    ds707_pkt_call_info_type  ds707_data;
#endif /* FEATURE_DATA_IS707 */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */

    ds_release_call_ind_type  end_params;

    ds_orig_call_type         orig_params;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif
    uint32               *user_data_ptr; /* Pointer to any user  data      */

  } cmd;

} ds_cmd_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS_TASK

DESCRIPTION
  This is the entry point for the Data Services Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds_task
(
  dword ignored
    /* lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);


/*===========================================================================

FUNCTION DS_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the Data Services Task. The
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

extern ds_cmd_type  *ds_get_cmd_buf( void );


/*===========================================================================

FUNCTION DS_PUT_CMD

DESCRIPTION
  This function puts a command on the Data Services Task's command queue, and
  sets the DS_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling ds_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds_put_cmd
(
  ds_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
);


/*===========================================================================

FUNCTION DS_GET_CALL_INFORMATION

DESCRIPTION
  This function should be invoked by UI for displaying data call information.
  The function returns by reference the following information:
  
  *system_information - A NULL-terminated string indicating the system or
                        service option, e.g., "WCDMA", for the call.
                        
  This function checks the size of the buffer for *system_information before
  it populates the buffer.
  
  Currently, UI retrieves the TX/RX maximum bit rates of the call from the
  QoS IE that comes with the CM CALL_CONNECTED event.

DEPENDENCIES
  None

RETURN VALUE
  This function returns the data statistics type, indicating if the information
  passed by reference, i.e., *system_information, is valid.

SIDE EFFECTS
  None

===========================================================================*/

extern ds_stats_e_type  ds_get_call_information
(
  cm_call_id_type     call_id,                                  /* Call ID */
  byte               *system_information,            /* System information */
  byte                max_system_information_length  /* Maximum length of
                                                        system information */
);


/*===========================================================================

FUNCTION DS_GET_CURRENT_SESSION_DATA_COUNTERS

DESCRIPTION
  This function should be invoked by UI for displaying data call statistics.
  It returns by reference the total count of bytes transmitted and received 
  at RLC for the current call:
  
  *rx_from_network_byte_cnt - Total count of bytes received for the call
  *tx_to_network_byte_cnt   - Total count of bytes transmitted for the call.
  
  The total byte count is accumulative for the call. If RAB is 
  re-established after PDP context is preserved during the call, the total
  byte count includes bytes transmitted and received after RAB is 
  re-established.

DEPENDENCIES
  None

RETURN VALUE
  This function returns the data statistics type, indicating if the information
  passed by reference, e.g., *rx_from_network_byte_cnt, is valid.

SIDE EFFECTS
  None

===========================================================================*/

extern ds_stats_e_type  ds_get_current_session_data_counters 
(
  cm_call_id_type     call_id,                                  /* Call ID */
  uint32             *rx_from_network_byte_cnt,       /* RX byte count for 
                                                                  the call */
  uint32             *tx_to_network_byte_cnt          /* TX byte count for
                                                                  the call */
);


/*===========================================================================

FUNCTION DS_GET_LAST_SESSION_DATA_COUNTERS

DESCRIPTION
  This function should be invoked by UI for retrieving the data call
  statistics at the end of the call. It returns the total count of bytes 
  transmitted and received at link layer for the call which terminated:
  
  *rx_from_network_byte_cnt - Total count of bytes received for the call
  *tx_to_network_byte_cnt   - Total count of bytes transmitted for the call.
  
  The total byte count is accumulative for the call. If RAB is 
  re-established after PDP context is preserved during the call, the total
  byte count includes bytes transmitted and received after RAB is 
  re-established.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A valid set of counters are returned
  FALSE - An error occured and the counters returned are not valid

SIDE EFFECTS
  None

===========================================================================*/

extern boolean  ds_get_last_session_data_counters 
(
  uint32             *rx_from_network_byte_cnt,       /* RX byte count for 
                                                                  the call */
  uint32             *tx_to_network_byte_cnt          /* TX byte count for
                                                                  the call */
);


#endif /* FEATURE_DATA */

#endif /* DSTASK_H */


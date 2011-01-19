/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            CM Logging Facilities

GENERAL DESCRIPTION
   This module contains functions for logging CM packets.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 - 2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmlog.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   aj      Adding support for updating clients with cdma local time
05/27/09   aj      Adding support for passing otasp status to clients
05/15/09   sg      Update SS EVENT log packet with new SS events added in CM
02/11/09   sv      Updated cmlog_get_max to cmlog_get_max_buff_size
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
11/26/08   ks      Reuse static phone info to avoid overflow of CM stack
11/24/08   cl      Limit maximum payload of the event to 32 bytes.
09/26/08   aj      Fix compiler warning
09/18/08   aj      support for UMA GAN
08/20/08   st      Resolve Klockwork defects
06/12/08   sv      Resolve Klocwork defects
03/17/08   am      Added two diag commands to support 
                   commands for CM_CALL_CMD_MODIFY_REQ / RSP
01/30/08   vs      Removing unnecessary code that was added under
                   FEATURE_HDR_HYBRID_DISABLE
01/03/08   ka      Adding handset based plus code dialing feature.
01/02/08   jqi/vs  Added support to temporary change the system preference
                   when FEATURE_HDR_HYBRID_DISABLE is enabled.
08/07/07   ic      Lint clean up
06/05/07   ic/cl   Change MSG_HIGH() to CM_MSG_HIGH() in the effort of merging
                   CL# 436262. This CL however has been previously merged.
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
02/23/07   ic      In cmlog_wcdma_call_origination() do not send as many
                   origination commands as there are digits in called number
                   (fixed bug introduced on CM mainline in CL 271665 )
11/17/06   jqi     Added support for HDR active protocol revision information.
11/09/06   rp      Added support for EVENT_CM_CELL_SRV_IND and
                   EVENT_CM_CELL_SRV_IND
07/10/06   ic      Fixed Klockworks errors (added missing assertions)
07/03/06   ic      Cleaned up leftovers from mainlining FEATURE_CM
                   Cleaned up leftovers from mainlining FEATURE_SUPS
04/20/06   jqi     Lint fixes.
04/19/06   jqi     Added diag subsystem command(select system preference
                   command2) to process 64-bit band preference.
04/10/06   pk      Lint fixes
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ic      Lint cleanup
12/13/05   ic      Fixed compiler errors "..\..\services\cm\cmlog.c",
                   Error: C3028E: '=': implicit cast of pointer to non-equal
                   pointer
12/10/05   ic      Lint cleanup
11/21/05   ic      Send EVENT_CM_OPERATIONAL_MODE on transition from
                   SYS_OPRT_MODE_OFFLINE_CDMA to SYS_OPRT_MODE_RESET.
                   Fix the functionality that postpones sending of
                   EVENT_CM_OPERATIONAL_MODE for SYS_OPRT_MODE_OFFLINE_CDMA
                   until after TASK_OFFLINE_SIG from TMC comes (CR73050).
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/28/05   pk      Modified cm_mm_client_ss_reg to register for all SS events
10/27/05   pk      Added support for SS log packet and
                   EVENT_CM_CALL_EVENT_ORIG_THR
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/02/05   ic      Print value that caused error in default switch case
07/27/05   ic      Log call events sent via cmxcall_event_generic()
07/25/05   sk      Replaced (for fixing CR 67893) the following:
                   - cm_util_def_alphabet_to_ascii()
                   - cm_util_ascii_to_def_alphabet()
                   with:
                   - cm_util_gsm_alphabet_to_ascii()
                   - cm_util_ascii_to_gsm_alphabet()
07/11/05   pk      Modified cm_ss_cmd_orig_thr_update_tbl() added packet_zone
                   as a additional parameter
06/21/05   ic      Renamed DIAG_MAX_GSM_DIGITS to CM_DIAG_MAX_GSM_DIGITS and
06/08/05   sk      Rearranged the featurization for
                   cmlog_cm_get_call_information_list() to be minimal.
                   Added cmlog_cm_send_call_information_log().
                   Moved user defined data types from here to cmlog.h.
06/06/05   pk      Added diag command to reset the call throttle table
                   CM_ORIG_THR_TBL_RESET -- SUBCOMMAND 15
05/25/05   ic      Mainlined FEATURE_CM
05/05/05   pk      Corrected copying of ddtm_srv_opt_list in the log packet
02/09/05   ic      Lint cleanup
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Fixed copying of arrays for call/phone event logging
01/06/05   ic      Mainlined FEATURE_CM_CALL_EVENT_LOGGING and
                   FEATURE_CM_PH_EVENT_LOGGING
01/05/05   ic      Log packet version for call and phone event packets
12/20/04   ic      Fixed ADS 1.2 compiler errors
12/19/04   ic      Added support for sending LOG_CM_CALL_EVENT_C under
                   FEATURE_CM_CALL_EVENT_LOGGING
12/10/04   ic      Refinments for CR: 48569 - HDR-KDDI: Request for new EVENTs
                   for DDTM mode (LOG_CM_PH_EVENT_C)
12/05/04   ic      Added support for sending LOG_CM_PH_EVENT_C under
                   FEATURE_CM_PH_EVENT_LOGGING
11/15/04   ka      Fixed copying calling number from diag request in
                   cmlog_cm_call_orig().
11/11/04   ic      Lint cleanup
10/28/04   ka      get_call_information_list modified to return call info
                   list syncronously, added ps call dormant status to list of
                   call details.
10/01/04   ic      Removed unnecessary lint -e787 comment (switch statement
                   now has default case and -e788 is globally suppressed)
09/30/04   ka      Changed CM_MSG_ERROR to MSG_LOW
09/24/04   ka      Adding support for Location based service request, service
                   complete response and event for conf from lower layers.
09/22/04   ka      Fixed a issue where allocated log buffer size was being
                   over run.
08/30/04   ka      Added support for srv_status in CM_STATE_INFO response
08/27/04   jqi     Added ps dormant optimization.
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/18/04   ic      Fixed RVCT 2.1 compiler warning thrown on Saber build
                   (M6250CTMQG4106):
                   Warning:  #144-D: a value of type "__packed void *" cannot
                   be used to initialize an entity of type
                   "DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *"
                   DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *req_ptr =
                   req_pkt;
                   Warning:  #144-D: a value of type "__packed void *" cannot
                   be used to initialize an entity of type
                   "DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *"
                   DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *req_ptr =
                   req_pkt;
08/12/04   ws      Remove cm20 support.
08/10/04   ic      Removed cmlog_get_operating_mode() and cmlog_system_mode()
                   as they became obsolete.
                   In cmlog_ph_event_handler() and cmlog_ss_event_handler()
                   call event_report_payload() with sizeof(value) instead of
                   sizeof(int)
08/03/04   ic      CM Lint clean up - cmlog.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   ws      Add redirecting num call event as a valid event with
                   unassigned call id.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/07/04   ka      Fixed Lint errors
07/01/04   ka      Increased array length for call_state_list, corrected length
                   checking for CM_SEND_USSD_REPLY. log_ptr allocation failure
                   no more stops USSD events from being reported.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/30/04   dk      Added GET_CCM_INFO command for CR 44830.
06/23/04   ic      Fixed RVCT 2.1 compiler error #158: (expression must be an
                   lvalue or a function designator) / Lint error #50:
                   Attempted to take the address of a non-lvalue
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
06/14/04   ws      Fixed featurization problem with CM_CALL_END.
05/14/04   ka      Fixed Lint errors.
05/14/04   ka      Changing system selection preference to accept 0xFFFFFFFF for
                   no change
04/05/04   ka      Changing system selection preference to be written with CM_PREF_TERM_PERMANENT.
03/23/04   ws      Merged code from 6300.
03/19/04   ka      Added Fix to change phone to OFFLINE_CDMA only after
                   cmlog_cm_offline_signal_recvd is set to true.
03/18/04   sj      Fixed featurization problems on CDMA only target.
03/16/04   ka      Merged code from 6250.
                   Added new events for PS Data continuity.
                   Fixed Lint errors.
03/02/04   ic      Fixed featurization compile errors
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/26/04   ka      Added routine cmlog_check_request_length () to check request length.
                   Swapped Id's for get and set closed user group preference.
01/21/04   ka      Corrected CM_CALL_ID_MAX to CM_DIAG_CALL_ID_MAX
01/13/04   ka      Removed include file uissups.h. Defined constants locally.
12/15/03   ka      Added function for getting call info list, entering password and
                   sending reply to USSD events. Added events for password prompting,
                   USSD request and notify indication
10/17/03   ka      Diag commands for getting/setting closed user group
                   preferences. CUG parameters used for call origination.
10/13/03   ka      Changed wcdma, gsm call origination function names.
09/29/03   vt      Added orig & end commands.
07/10/03   prk     Modified cmlog_cm_system_selection to reverse the byte
                   order of the PLMN id.  Previously, the user was being
                   forced to enter the PLMN id in little endian notation.
05/21/03   vt      Added new call events.
04/28/03   AT      Lint cleanup.
03/21/03   ATM     Added event logging for svc_cnf and nw_list_cnf
                   And commands for initiating service/nw_list
                   And mainlined DEBUG_CM_EVENT_REPORTING
02/26/03   prk     Added include of diagcmd.h.
02/12/03   atm     Added pref logging and moved subsys command from UMTS to
                   CM.
01/21/03   ws      Updated copyright information for 2003
09/03/02   atm     Added mapping from CM enum to logging enum to sync up to
                   existing event database.
05/06/02   PRK     Renamed DEBUG_CM_NO_EVENT_REPORTING to
                   DEBUG_CM_EVENT_REPORTING.  The flag needs to be defined
                   to perform event reporting.
04/30/02   atm     Added flag to turn off functionality:
                    DEBUG_CM_NO_EVENT_REPORTING (by default, code is on)
04/30/02   atm     Event codes now defined in event_defs.h, -r1.14.
04/26/02   atm     Initial Revision.

===========================================================================*/

#if defined __cplusplus
  extern "C" {
#endif

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"
#include "cmlog.h"          /* This module's header file */
#include "event.h"          /* For logging STATE events and event codes */
#include "cmcall.h"         /* For cmcall_overall_call_state() */
#include "cmss.h"           /* For pointer to SS object */
#include "sys.h"            /* For SYSMODE and OPMODE enums */
#include "diagpkt.h"        /* for subsystem commands      */
#include "diagcmd.h"        /* for subsystem commands      */
#include "cmclient.h"       /* for event notifications     */

#include "cmdbg.h"
#include <string.h>
#include "msg.h"            /* Interface to F3 messages services */
#include "cmutil.h"         /* Declaring cm_util_ascii_to_def_alphabet () */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif

static cmlog_client_s_type diag_client;

static cmlog_call_state_s_type call_state_list[CM_CALL_ID_MAX];

/*===========================================================================
                         FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION cmlog_call_orig_start

DESCRIPTION
   This function logs the call orig start event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_start
(
  const cmcall_s_type  *call_ptr
    /*  pointer for the call object */
)
{
  uint8 payload[6];
  uint8 *payload_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* report EVENT_CM_CALL_ORIG_START_P1 */
  payload_ptr = payload;/* set payload pointer = &payload[0] */
  *payload_ptr = (uint8)call_ptr->call_type;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)call_ptr->srv_type;
  payload_ptr += sizeof(uint8);
  #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
  *((uint16 *)payload_ptr)
       = (uint16)call_ptr->cmcall_mode_info.info.cdma_info.srv_opt;
  payload_ptr += sizeof(uint16);
  *payload_ptr = (uint8)call_ptr->cmcall_mode_info.info.cdma_info.drs_bit;
  #endif
  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #else
  event_report_payload(EVENT_CM_CALL_ORIG_START_P1,5,payload);
  #endif

  /* report EVENT_CM_CALL_ORIG_START_P2 */
  payload_ptr = payload; /* set payload pointer = &payload[0] */
  *payload_ptr = (uint8)call_ptr->favored_mode_pref;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)call_ptr->favored_hybr_pref;
  event_report_payload(EVENT_CM_CALL_ORIG_START_P2,2,payload);

  /* report EVENT_CM_CALL_ORIG_START_P3 */
  payload_ptr = payload; /* set payload pointer = &payload[0] */
  *payload_ptr = (uint8)call_ptr->ss;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)cmss_ptr()->info.sys_mode;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)cmss_ptr()->is_colloc;
  event_report_payload(EVENT_CM_CALL_ORIG_START_P3,3,payload);

}


/*===========================================================================
FUNCTION  cmlog_call_orig_redial

DESCRIPTION
   This function logs the call orig redial event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_redial(
  cm_call_end_e_type    end_status,
    /* the call end status */
  cmcall_act_e_type     redial_act,
    /* the redial action */
  dword                 sr_left
    /* number of silent redial left */
)
{

  uint8 payload[4];
  uint8 *payload_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** 571: Suspicious cast
  ** To be investigated why we cast it as uint16 vs uint8
  */
  /*lint -save -e571 */

  /* Report EVENT_CM_CALL_ORIG_REDIAL
  */

  /* Set payload pointer = &payload[0]
  */
  payload_ptr = payload;
  *((uint16 *)payload_ptr) = (uint16)end_status;
  payload_ptr += sizeof(uint16);
  *payload_ptr = (uint8)redial_act;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)sr_left;
  event_report_payload(EVENT_CM_CALL_ORIG_REDIAL,4, payload);
  /*lint -restore */
}


/*===========================================================================
FUNCTION  cmlog_call_orig_send_hdr_orig

DESCRIPTION
   This function logs the orig sent to HDR event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_hdr_orig( void )
{
  event_report_payload(EVENT_CM_CALL_ORIG_SEND_HDR_ORIG,0,NULL);
}


/*===========================================================================
FUNCTION  cmlog_call_orig_send_mc_orig

DESCRIPTION
   This function logs the orig sent to MC event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_send_mc_orig( void )
{
  event_report_payload(EVENT_CM_CALL_ORIG_SEND_MC_ORIG,0,NULL);
}


/*===========================================================================
FUNCTION  cmlog_call_orig_end

DESCRIPTION
   This function logs the call orig end event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_end(
  cm_call_end_e_type         end_status
    /* call end status */
)
{
  uint8 payload[2];
  uint8 *payload_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** 571: Suspicious cast
  ** It's OK here, want to supress it
  */
  /*lint -save -e571 */
  /* report EVENT_CM_CALL_ORIG_END */
  payload_ptr = payload;/* set payload pointer = &payload[0] */
  *(uint16 *)payload_ptr = (uint16)end_status;
  event_report_payload(EVENT_CM_CALL_ORIG_END,2,payload);
  /*lint -restore */
}


/*===========================================================================
FUNCTION  cmlog_call_orig_connected

DESCRIPTION
   This function logs the call orig connected event

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_call_orig_connected(
  cm_call_type_e_type         call_type,
    /* origination call type */
  cm_srv_type_e_type          srv_type,
    /* origination srv type */
  word                        srv_opt
    /* origination srv opt */
)
{
   uint8 payload[4];
   uint8 *payload_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* report EVENT_CM_CALL_ORIG_CONNECTED */
  payload_ptr = payload;/* set payload pointer = &payload[0] */
  *payload_ptr = (uint8)call_type;
  payload_ptr += sizeof(uint8);
  *payload_ptr = (uint8)srv_type;
  payload_ptr += sizeof(uint8);
  *(uint16 *)payload_ptr =(uint16)srv_opt;
  event_report_payload(EVENT_CM_CALL_ORIG_CONNECTED,4,payload);
}


/*===========================================================================
FUNCTION  cmlog_enter_emergency_cb
===========================================================================*/
void cmlog_enter_emergency_cb ( void )
{
    event_report_payload( EVENT_CM_ENTER_EMERGENCY_CB, 0, NULL );
}

/*===========================================================================
FUNCTION  cmlog_enter_emergency_cb
===========================================================================*/
void cmlog_exit_emergency_cb ( void )
{
    event_report_payload( EVENT_CM_EXIT_EMERGENCY_CB, 0, NULL );
}

/*===========================================================================
FUNCTION  cmlog_call_state
===========================================================================*/
void cmlog_call_state ( void )
{
    byte STATE = cmlog_get_call_state();

    event_report_payload( EVENT_CM_CALL_STATE, 1, &STATE );
}

/*===========================================================================
FUNCTION  cmlog_get_call_state
===========================================================================*/
byte cmlog_get_call_state ( )
{
    return (byte)cmcall_overall_call_state();
}


/*===========================================================================
FUNCTION  cmlog_get_operating_mode
===========================================================================*/
byte cmlog_get_operating_mode ( )
{
    return (byte)diag_client.oprt_mode;
}

/*===========================================================================
FUNCTION  cmlog_get_system_mode
===========================================================================*/
byte cmlog_get_system_mode ( )
{
    return (byte)diag_client.sys_mode;
}

/*===========================================================================
FUNCTION  cmlog_get_call_state_list_index

Description:

       Loops through call_state_list to return index of the array entry
       holding requested call_id
===========================================================================*/
static int cmlog_get_call_state_list_index (cm_call_id_type call_id)
{
  int loop = 0;

  for (loop = 0;loop < CM_CALL_ID_MAX; ++loop)
  {
    if (call_state_list[loop].call_id == call_id)
      return loop;
  }

  return CM_CALL_ID_MAX;
}


/*===========================================================================
FUNCTION  cmlog_set_call_state_list_entry

Description:

       Loops through call_state_list to find an entry for storing call_id
       and returns the index
===========================================================================*/
static int cmlog_set_call_state_list_entry (cm_call_id_type call_id)
{
  int loop = 0;

  for (loop = 0;loop < CM_CALL_ID_MAX; ++loop)
  {
    if (call_state_list[loop].call_id == CM_CALL_ID_INVALID)
    {
      call_state_list[loop].call_id = call_id;
      return loop;
    }
  }

  return CM_CALL_ID_MAX;
}

/*===========================================================================
FUNCTION  cmph_log_handle_oprt_mode_event

Description:

  This function handles CM_PH_EVENT_OPRT_MODE
  We want to send DIAG EVENT_CM_OPERATIONAL_MODE only after TASK_OFFLINE_SIG
  was sent by TMC and received by CM

===========================================================================*/
static void cmph_log_handle_oprt_mode_event(

     cm_ph_event_e_type         ph_event,
       /* phone event to log */

     const cm_ph_info_s_type   *ph_info_ptr
       /* phone info structure with event info */
)
/*lint -esym(715, ph_event) */
{
  boolean send_oprt_mode_event;
  int     value;

  if (diag_client.oprt_mode != ph_info_ptr->oprt_mode)
  {
    /* Most of the time we'll send the DIAG event
    */
    send_oprt_mode_event = TRUE;

    /* Field diag_client.offline_signal_recvd records TASK_OFFLINE_SIG
    ** sent by TMC to CM.
    ** This signal is the closest indicator about all the tasks in the system
    ** going offline.
    ** Send EVENT_CM_OPERATIONAL_MODE for SYS_OPRT_MODE_OFFLINE_CDMA
    ** only after CM got the signal from TMC
    */
    if(ph_info_ptr->oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA)
    {
      if(diag_client.offline_signal_recvd == TRUE)
      {
        /* Reset the flag, send the event
        */
        diag_client.offline_signal_recvd = FALSE;
      }
      else
      {
        /* Defer sending the event until after TASK_OFFLINE_SIG comes
        ** It will be sent from cmlog_set_cm_offline_signal_recvd()
        */
        send_oprt_mode_event = FALSE;
        CM_MSG_MED("No TASK_OFFLINE_SIG, defer EVENT_CM_OPERATIONAL_MODE DIAG event",0,0,0);
      }
    }

    if(send_oprt_mode_event)
    {
     value = (int)(diag_client.oprt_mode = ph_info_ptr->oprt_mode);
     event_report_payload( EVENT_CM_OPERATIONAL_MODE, sizeof(value), &value );
    }
  }
}
/*lint +esym(715, ph_event) */

/*===========================================================================
FUNCTION  cmlog_ph_event_handler

Description:

       This function checks for changes to preferences and logs
       them accordingly.

===========================================================================*/
/*lint -esym(765, cmlog_ph_event_handler)
** Can't be static, registered to handle events for DIAG client
*/
void cmlog_ph_event_handler(

     cm_ph_event_e_type         ph_event,
       /* phone event to log */

     const cm_ph_info_s_type   *ph_info_ptr
       /* phone info structure with event info */
)
{

 int                     value;

 unsigned int            i;
   /* Loop counter
   */

 unsigned int            log_pkt_len = 0;
   /* Length of packet to be logged.
   */

 LOG_CM_PH_EVENT_C_type *log_ptr     = NULL;
   /* Log pointer to the log record type.
   */

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 CM_ASSERT(!BETWEEN(CM_PH_EVENT_NONE, ph_event, CM_PH_EVENT_MAX));
 CM_ASSERT(ph_info_ptr != NULL);

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 switch ( ph_event )
 {
   case CM_PH_EVENT_INFO_AVAIL:
     /* Phone information is now available. Automatically generated
     ** after a client does activation */
   case CM_PH_EVENT_SYS_SEL_PREF:
   /* System Selection pref was changed - SD20*/

     if ( diag_client.oprt_mode != ph_info_ptr->oprt_mode )
     {
         value = (int)(diag_client.oprt_mode = ph_info_ptr->oprt_mode);
         event_report_payload( EVENT_CM_OPERATIONAL_MODE, sizeof(value), &value );
     }
     if ( diag_client.mode_pref != ph_info_ptr->mode_pref )
     {
         value = (int)(diag_client.mode_pref = ph_info_ptr->mode_pref);
         event_report_payload( EVENT_CM_MODE_PREF, sizeof(value), &value );
     }
     if ( diag_client.acq_order_pref != ph_info_ptr->acq_order_pref )
     {
         value = (int)(diag_client.acq_order_pref = ph_info_ptr->acq_order_pref);
         event_report_payload( EVENT_CM_GW_ACQ_ORDER_PREF, sizeof(value), &value );
     }
     if ( diag_client.srv_domain_pref != ph_info_ptr->srv_domain_pref )
     {
         value = (int)(diag_client.srv_domain_pref = ph_info_ptr->srv_domain_pref);
         event_report_payload( EVENT_CM_SRV_DOMAIN_PREF, sizeof(value), &value );
     }
     if ( diag_client.band_pref != ph_info_ptr->band_pref )
     {
         value = (int)(diag_client.band_pref = ph_info_ptr->band_pref);
         event_report_payload( EVENT_CM_BAND_PREF, sizeof(value), &value );
     }
     if ( diag_client.roam_pref != ph_info_ptr->roam_pref )
     {
         value = (int)(diag_client.roam_pref = ph_info_ptr->roam_pref);
         event_report_payload( EVENT_CM_ROAM_PREF, sizeof(value), &value );
     }
     if ( diag_client.hybr_pref != ph_info_ptr->hybr_pref )
     {
         value = (int)(diag_client.hybr_pref = ph_info_ptr->hybr_pref);
         event_report_payload( EVENT_CM_HYBRID_PREF, sizeof(value), &value );
     }
     if ( diag_client.network_sel_mode_pref != ph_info_ptr->network_sel_mode_pref )
     {
         value = (int)(diag_client.network_sel_mode_pref = ph_info_ptr->network_sel_mode_pref);
         event_report_payload( EVENT_CM_NETWORK_SEL_MODE_PREF, sizeof(value), &value );
     }
     break;

   case CM_PH_EVENT_OPRT_MODE:
     /* Operating mode was changed */

     cmph_log_handle_oprt_mode_event(ph_event, ph_info_ptr);
     break;

   case CM_PH_EVENT_NAM_SEL:
     /* NAM dependent variables changed, such as
     ** the mode preference and the system preference
     ** in accordance with the new NAM selection.
     */
     if ( diag_client.mode_pref != ph_info_ptr->mode_pref )
     {
         value = (int)(diag_client.mode_pref = ph_info_ptr->mode_pref);
         event_report_payload( EVENT_CM_MODE_PREF, sizeof(value), &value );
     }
     if ( diag_client.band_pref != ph_info_ptr->band_pref )
     {
         value = (int)(diag_client.band_pref = ph_info_ptr->band_pref);
         event_report_payload( EVENT_CM_BAND_PREF, sizeof(value), &value );
     }
     if ( diag_client.roam_pref != ph_info_ptr->roam_pref )
     {
         value = (int)(diag_client.roam_pref = ph_info_ptr->roam_pref);
         event_report_payload( EVENT_CM_ROAM_PREF, sizeof(value), &value );
     }
     break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   default:
     break;
 } /* switch */

 /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 /*
 ** The below are log packets for all PH events
 */

 /*
 ** Compute log packet length
 */
 log_pkt_len = sizeof(LOG_CM_PH_EVENT_C_type);

 /* Allocate a log packet.
 */
 log_ptr = (LOG_CM_PH_EVENT_C_type *) log_alloc_ex( LOG_CM_PH_EVENT_C,
                                                    log_pkt_len );
 /*
 ** If allocation failed, just return.
 ** It means that either logging of LOG_CM_PH_EVENT_C is off
 ** or DIAG is truly out of memory (rare)
 */
 if ( log_ptr == NULL )
 {
   CM_MSG_LOW("Log packet buffer allocation failed!",0,0,0);
   return;
 }

 /*lint -save -e641 Converting enum to int */

 /*
 ** Write log packet version
 ** This is always set to CM_LOG_PH_EVENT_LOG_PACKET_VERSION
 */
 log_ptr->version               = CM_LOG_PH_EVENT_LOG_PACKET_VERSION;

 /* Copy field by field from ph_info_ptr
 */
 log_ptr->ph_event              = ph_event;
 log_ptr->is_in_use             = ph_info_ptr->is_in_use;
 log_ptr->oprt_mode             = ph_info_ptr->oprt_mode;
 log_ptr->test_control_type     = ph_info_ptr->test_control_type;
 log_ptr->cdma_lock_mode        = ph_info_ptr->cdma_lock_mode;
 log_ptr->answer_voice          = ph_info_ptr->answer_voice;
 log_ptr->mode_pref             = ph_info_ptr->mode_pref;
 log_ptr->pref_term             = ph_info_ptr->pref_term;
 log_ptr->acq_order_pref        = ph_info_ptr->acq_order_pref;
 log_ptr->srv_domain_pref       = ph_info_ptr->srv_domain_pref;
 log_ptr->band_pref             = ph_info_ptr->band_pref; /*lint !e713 */
 log_ptr->roam_pref             = ph_info_ptr->roam_pref;
 log_ptr->nam_sel               = ph_info_ptr->nam_sel;
 log_ptr->curr_nam              = ph_info_ptr->curr_nam;
 log_ptr->packet_state          = ph_info_ptr->packet_state;
 log_ptr->hybr_pref             = ph_info_ptr->hybr_pref;
 log_ptr->network_sel_mode_pref = ph_info_ptr->network_sel_mode_pref;

 log_ptr->plmn.identity[0] = ph_info_ptr->plmn.identity[0];
 log_ptr->plmn.identity[1] = ph_info_ptr->plmn.identity[1];
 log_ptr->plmn.identity[2] = ph_info_ptr->plmn.identity[2];

 /* Copy MIN() to not overflow log packet buffer
 */
 log_ptr->user_pref_networks.length = MIN(ph_info_ptr->user_pref_networks.length,
                                          ARR_SIZE(log_ptr->user_pref_networks.info));


 for(i = 0; i < log_ptr->user_pref_networks.length; i++)
 {
   log_ptr->user_pref_networks.info[i].plmn.identity[0] =
                    ph_info_ptr->user_pref_networks.info[i].plmn.identity[0];
   log_ptr->user_pref_networks.info[i].plmn.identity[1] =
                    ph_info_ptr->user_pref_networks.info[i].plmn.identity[1];
   log_ptr->user_pref_networks.info[i].plmn.identity[2] =
                    ph_info_ptr->user_pref_networks.info[i].plmn.identity[2];

   log_ptr->user_pref_networks.info[i].access_tech =
                    ph_info_ptr->user_pref_networks.info[i].access_tech;
 }

 /* Copy MIN() to not overflow log packet buffer
 */
 log_ptr->available_networks.length = MIN(ph_info_ptr->available_networks.length,
                                          ARR_SIZE(log_ptr->available_networks.info));

 for(i = 0; i < log_ptr->available_networks.length; i++)
 {

   log_ptr->available_networks.info[i].plmn.identity[0] =
            ph_info_ptr->available_networks.info[i].plmn.identity[0];
   log_ptr->available_networks.info[i].plmn.identity[1] =
            ph_info_ptr->available_networks.info[i].plmn.identity[1];
   log_ptr->available_networks.info[i].plmn.identity[2] =
            ph_info_ptr->available_networks.info[i].plmn.identity[2];

   log_ptr->available_networks.info[i].rat =
            ph_info_ptr->available_networks.info[i].rat;

   log_ptr->available_networks.info[i].plmn_service_capability =
            ph_info_ptr->available_networks.info[i].plmn_service_capability;

   log_ptr->available_networks.info[i].plmn_forbidden =
            ph_info_ptr->available_networks.info[i].plmn_forbidden;

   log_ptr->available_networks.info[i].list_category =
            ph_info_ptr->available_networks.info[i].list_category;

   log_ptr->available_networks.info[i].signal_quality =
            ph_info_ptr->available_networks.info[i].signal_quality;

   log_ptr->available_networks.info[i].signal_strength =
            ph_info_ptr->available_networks.info[i].signal_strength;

 }

 log_ptr->is_cdma_subscription_available =
                                 ph_info_ptr->is_cdma_subscription_available;
 log_ptr->is_gw_subscription_available   =
                                 ph_info_ptr->is_gw_subscription_available;
 log_ptr->rtre_config                    = ph_info_ptr->rtre_config;
 log_ptr->rtre_control                   = ph_info_ptr->rtre_control;
 log_ptr->orig_srv_domain_pref           =
                            cmtask_compute_srv_domain_pref(CM_SS_MAIN);
 log_ptr->ddtm_pref                      = ph_info_ptr->ddtm_pref;
 log_ptr->cur_ddtm_status                = ph_info_ptr->cur_ddtm_status;
 log_ptr->ddtm_act_mask                  = ph_info_ptr->ddtm_act_mask;
 log_ptr->orig_ddtm_act_mask             = ph_info_ptr->orig_ddtm_act_mask;


 /* Copy MIN() to not overflow log packet buffer
 */
 log_ptr->ddtm_num_srv_opt               = MIN(ph_info_ptr->ddtm_num_srv_opt,
                                               ARR_SIZE(log_ptr->ddtm_srv_opt_list));
 for ( i = 0; i < log_ptr->ddtm_num_srv_opt; i++ )
 {
   log_ptr->ddtm_srv_opt_list[i] = ph_info_ptr->ddtm_srv_opt_list[i];
 }

 log_ptr->cphs_allowed         = ph_info_ptr->cphs_allowed;
 log_ptr->als_allowed          = ph_info_ptr->als_allowed;
 log_ptr->line                 = ph_info_ptr->line;
 log_ptr->line_switching       = ph_info_ptr->line_switching;

 log_ptr->requesting_client_id = ph_info_ptr->requesting_client_id;


 #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
 #else

 /* Fill with N/A if those features are off
 */

 log_ptr->cdma_capability.type        = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
 log_ptr->cdma_capability.payload_len = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;

 for ( i = 0; i < ARR_SIZE(log_ptr->cdma_capability.payload); i++ )
 {
   log_ptr->cdma_capability.payload[i] = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
 }

 log_ptr->msm_id = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;

 #endif

 /* Submit the log to DIAG.
 */
 log_commit( (void *) log_ptr );

 /*lint -restore */

}

/*===========================================================================
FUNCTION  cmlog_set_cm_offline_signal_recvd

Description:

  CM calls this after receiving TASK_OFFLINE_SIG signal from TMC.
  This happens some time after cm_ph_cmd_oprt_mode() with
  SYS_OPRT_MODE_OFFLINE_CDMA had been processed by CM.
  Call on log object oprt mode event handler to force sending of DIAG
  EVENT_CM_OPERATIONAL_MODE event now that we got TASK_OFFLINE_SIG
  from TMC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmlog_set_cm_offline_signal_recvd(void)
{
  /* get a buffer for phone info */
  cm_ph_info_s_type *ph_info_ptr = cmph_info_alloc();;

  /* Set the flag to indicate that we got TASK_OFFLINE_SIG
  */
  diag_client.offline_signal_recvd = TRUE;

  /* Copy phone state information into phone info buffer.
  */
  (void)cmph_info_get( ph_info_ptr );

  /* ... it'd better be SYS_OPRT_MODE_OFFLINE_CDMA
  */
  CM_ASSERT(ph_info_ptr->oprt_mode == SYS_OPRT_MODE_OFFLINE_CDMA);

  /* Call on log object oprt mode event handler to force sending of DIAG
  ** EVENT_CM_OPERATIONAL_MODE event now that we got TASK_OFFLINE_SIG
  ** from TMC
  */
  cmph_log_handle_oprt_mode_event(CM_PH_EVENT_OPRT_MODE,
                                  ph_info_ptr);
}

/*===========================================================================
FUNCTION  cmlog_ss_event_handler

Description:

       This function checks for changes to preferences and logs
       them accordingly.

===========================================================================*/
/*lint -esym(765, cmlog_ss_event_handler)
** Can't be static, registered to handle SS events for DIAG client
*/
void cmlog_ss_event_handler( cm_ss_event_e_type           ss_event,
                             const cm_mm_ss_info_s_type  *ss_info_ptr )
{
  int value,i;
  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
  uint8 sys_mode;
  #endif /* FEATURE_PS_DORMANT_PWR_SAVE */

  unsigned int            log_pkt_len = 0;
   /* Length of packet to be logged.
   */

  LOG_CM_SS_EVENT_C_type *log_ptr     =  NULL;
    /* Log pointer to the log record type.
    */

  cm_country_code_type     current_mcc = cmss_ptr()->info.current_mcc;
    /* Current country code
    */

  switch ( ss_event )
  {
    case CM_SS_EVENT_SRV_CHANGED:
      /* Serving system information changed */
      if ( diag_client.sys_mode != ss_info_ptr->sys_mode )
      {
          value = (int)(diag_client.sys_mode = ss_info_ptr->sys_mode);
          event_report_payload( EVENT_CM_SYSTEM_MODE, sizeof(value), &value );
      }

      /* Event when data suspend mask gets changed */
      if ((ss_info_ptr->changed_fields & CM_SS_PS_DATA_SUSPEND_MASK) != 0)
      {
          uint8 result[2];
          result[0] = (uint8)ss_info_ptr->ps_data_suspend;
          result[1] = (uint8)ss_info_ptr->sys_mode;

          event_report_payload( EVENT_CM_DS_INTERRAT_STATE, sizeof(result), result);
      }
      if ( ( ( (ss_info_ptr->changed_fields & CM_SS_SRV_IND_MASK) !=0 ) &&
             ((diag_client.cell_srv_ind.hs_ind != ss_info_ptr->cell_srv_ind.hs_ind) ||
             (diag_client.cell_srv_ind.dtm_supp != ss_info_ptr->cell_srv_ind.dtm_supp))
           )
         )
      {
          uint8 result[5];
          diag_client.cell_srv_ind.hs_ind = ss_info_ptr->cell_srv_ind.hs_ind;
          diag_client.cell_srv_ind.dtm_supp = ss_info_ptr->cell_srv_ind.dtm_supp;
          result[0] = (uint8)ss_info_ptr->cell_srv_ind.hs_ind;
          result[1] = (uint8)ss_info_ptr->cell_srv_ind.dtm_supp;

          /* Setting unused bytes to zero */
          result[2]=result[3]=result[4]= (uint8)0;

          event_report_payload( EVENT_CM_CELL_SRV_IND, sizeof(result), result);
      }

      /* CM_SS_EVENT_COUNTRY_SELECTED gets sent only when user selects a country.
    ** When CM can automatically decide CM_SS_EVENT_SRV_CHANGED carries
    ** selected country. Mapping CM_SS_EVENT_COUNTRY_SELECTED and
    ** CM_SS_EVENT_SRV_CHANGED to EVENT_CM_COUNTRY_SELECTED
      */
      {
        uint8 result[CMLOG_MAX_EVENT_PAYLOAD];

        //if (ss_info_ptr->current_mcc != CM_INVALID_MOBILE_COUNTRY_CODE)
        {

          /*lint -save -e545 */
          memcpy (&result, &current_mcc,
                                      sizeof(current_mcc));
          /*lint -restore */

          event_report_payload( EVENT_CM_COUNTRY_SELECTED, sizeof(current_mcc), result);

          CM_MSG_HIGH("Sending EVENT_CM_COUNTRY_SELECTED with country %d",
                      cmss_ptr()->info.current_mcc, 0, 0);
        }

      }
      break;
  #ifdef FEATURE_PS_DORMANT_PWR_SAVE
    case CM_SS_EVENT_PS_DATA_AVAIL:
      sys_mode = (uint8) cmss_ptr()->info.sys_mode;
      event_report_payload( EVENT_CM_DATA_AVAILABLE,
        sizeof(sys_mode), &sys_mode );
      break;
  #endif /* FEATURE_PWR_DORMANT_PWR_SAVE */

    case CM_SS_EVENT_SELECT_COUNTRY:

      /* Not storing it in diag_client */
      {
        uint8 result[CMLOG_MAX_EVENT_PAYLOAD];

        result[0] = (uint8)(cmss_ptr()->info.country_list.country_code_len *
                 sizeof(cmss_ptr()->info.country_list.country_code_list[0]));

        if (result[0] > CMLOG_MAX_EVENT_PAYLOAD - 1)
        {
          CM_ERR ("Reducing select country event payload", 0, 0, 0);
          result[0] = CMLOG_MAX_EVENT_PAYLOAD - 1;
        }

        memcpy (&result[1], cmss_ptr()->info.country_list.country_code_list,
                 result[0]);

        event_report_payload( EVENT_CM_SELECT_COUNTRY, sizeof(result), result);

        CM_MSG_HIGH("Sending EVENT_CM_SELECT_COUNTRY with country list len %d",
                      cmss_ptr()->info.country_list.country_code_len, 0, 0);
      }

      break;


    case CM_SS_EVENT_COUNTRY_SELECTED:

      /* Not storing it in diag_client */
      {
        uint8 result[CMLOG_MAX_EVENT_PAYLOAD];

        /*lint -save -e545 */
        memcpy (&result, &current_mcc,
                                      sizeof(current_mcc));
        /*lint -restore */

        event_report_payload( EVENT_CM_COUNTRY_SELECTED, sizeof(result), result);

        CM_MSG_HIGH("Sending EVENT_CM_COUNTRY_SELECTED with country %d",
                      current_mcc, 0, 0);

      }

      break;

    default:
      break;
  } /* switch */

  /*
  ** Generate log packet for SS event
  */

  /*
  ** Compute log packet length
  */
  log_pkt_len = sizeof(LOG_CM_SS_EVENT_C_type);

  /* Allocate a log packet.
  */
  log_ptr = (LOG_CM_SS_EVENT_C_type *) log_alloc_ex( LOG_CM_SS_EVENT_C,
                                                       log_pkt_len );
  /*
  ** If allocation failed, just return.
  ** It means that either logging of LOG_CM_SS_EVENT_C is off
  ** or DIAG is truly out of memory (rare)
  */
  if ( log_ptr == NULL )
  {
    CM_MSG_LOW("Log packet buffer allocation failed!",0,0,0);
    return;
  }

  /*lint -save -e641 Converting enum to int */

  /*
  ** Write log packet version
  ** This is always set to CM_LOG_SS_EVENT_LOG_PACKET_VERSION
  */

  log_ptr->version                = CM_LOG_SS_EVENT_LOG_PACKET_VERSION;
  log_ptr->ss_event               = ss_event;

  log_ptr->changed_fields         = ss_info_ptr->changed_fields;
  log_ptr->srv_status             = ss_info_ptr->srv_status;
  log_ptr->srv_domain             = ss_info_ptr->srv_domain;
  log_ptr->srv_capability         = ss_info_ptr->srv_capability;
  log_ptr->sys_mode               = ss_info_ptr->sys_mode;
  log_ptr->roam_status            = ss_info_ptr->roam_status;

  log_ptr->sys_id.id_type         = ss_info_ptr->sys_id.id_type;

  switch( ss_info_ptr->sys_id.id_type )
  {

    case SYS_SYS_ID_TYPE_IS95:

      log_ptr->sys_id.id.is95.sid = ss_info_ptr->sys_id.id.is95.sid;
      log_ptr->sys_id.id.is95.nid = ss_info_ptr->sys_id.id.is95.nid;
      break;

    case SYS_SYS_ID_TYPE_IS856:

      for( i=0; i<16; i++ )
      {
        log_ptr->sys_id.id.is856[i] = ss_info_ptr->sys_id.id.is856[i];
      }
      break;

    case SYS_SYS_ID_TYPE_WLAN:

      log_ptr->sys_id.id.ssid.len = ss_info_ptr->sys_id.id.ssid.len;
      for( i=0 ; i<ss_info_ptr->sys_id.id.ssid.len; i++ )
      {
        log_ptr->sys_id.id.ssid.ssid[i] = ss_info_ptr->sys_id.id.ssid.ssid[i];
      }
      break;


    case SYS_SYS_ID_TYPE_UMTS:

      log_ptr->sys_id.id.plmn_lac.lac = ss_info_ptr->sys_id.id.plmn_lac.lac;
      for( i=0; i<3; i++ )
      {
        log_ptr->sys_id.id.plmn_lac.plmn.identity[i] =
                            ss_info_ptr->sys_id.id.plmn_lac.plmn.identity[i];
      }
      break;

    case SYS_SYS_ID_TYPE_UNDEFINED:

      memset((void *) &log_ptr->sys_id.id, CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
              sizeof(log_ptr->sys_id.id));
      break;

    default:

      memset((void *) &log_ptr->sys_id, CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
              sizeof(log_ptr->sys_id));
      break;
  } /* switch( ss_info_ptr->sys_id.id_type ) */

  log_ptr->is_sys_forbidden       = ss_info_ptr->is_sys_forbidden;
  log_ptr->is_colloc              = ss_info_ptr->is_colloc;
  log_ptr->rssi                   = ss_info_ptr->rssi;
  log_ptr->ecio                   = ss_info_ptr->ecio;
  log_ptr->io                     = ss_info_ptr->io;
  log_ptr->sinr                   = ss_info_ptr->sinr;
  log_ptr->psist                  = ss_info_ptr->psist;
  log_ptr->ps_data_suspend        = ss_info_ptr->ps_data_suspend;
  log_ptr->hdr_hybrid             = ss_info_ptr->hdr_hybrid;
  log_ptr->hdr_srv_status         = ss_info_ptr->hdr_srv_status;
  log_ptr->hdr_roam_status        = ss_info_ptr->hdr_roam_status;
  log_ptr->hdr_rssi               = ss_info_ptr->hdr_rssi;
  log_ptr->hdr_ecio               = ss_info_ptr->hdr_ecio;
  log_ptr->hdr_io                 = ss_info_ptr->hdr_io;
  log_ptr->hdr_sinr               = ss_info_ptr->hdr_sinr;
  log_ptr->hdr_active_prot        = ss_info_ptr->hdr_active_prot;
  log_ptr->prev_idle_digital_mode = ss_info_ptr->prev_idle_digital_mode;
  log_ptr->cur_idle_digital_mode  = ss_info_ptr->cur_idle_digital_mode;

  switch ( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_NONE:
    case SYS_SYS_MODE_GPS:
    case SYS_SYS_MODE_NO_SRV:

      memset((void *) &log_ptr->mode_info, CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
              sizeof(log_ptr->mode_info));
      break;


    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:

      /* Copy GW info here
      */
      log_ptr->mode_info.gw_info.bit_err_rate   = ss_info_ptr->mode_info.gw_info.bit_err_rate;
      log_ptr->mode_info.gw_info.sim_state      = ss_info_ptr->mode_info.gw_info.sim_state;
      log_ptr->mode_info.gw_info.plmn_forbidden = ss_info_ptr->mode_info.gw_info.plmn_forbidden;

      log_ptr->mode_info.gw_info.mm_information.plmn_avail                =
                    ss_info_ptr->mode_info.gw_info.mm_information.plmn_avail;
      log_ptr->mode_info.gw_info.mm_information.full_name_avail           =
               ss_info_ptr->mode_info.gw_info.mm_information.full_name_avail;
      log_ptr->mode_info.gw_info.mm_information.short_name_avail          =
              ss_info_ptr->mode_info.gw_info.mm_information.short_name_avail;
      log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone_avail =
        ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone_avail;
      log_ptr->mode_info.gw_info.mm_information.time_zone_avail           =
               ss_info_ptr->mode_info.gw_info.mm_information.time_zone_avail;
      log_ptr->mode_info.gw_info.mm_information.lsa_identity_avail        =
            ss_info_ptr->mode_info.gw_info.mm_information.lsa_identity_avail;
      log_ptr->mode_info.gw_info.mm_information.daylight_saving_adj_avail =
        ss_info_ptr->mode_info.gw_info.mm_information.daylight_saving_adj_avail;

      if( log_ptr->mode_info.gw_info.mm_information.plmn_avail )
      {
        for( i=0; i<3; i++ )
        {
          log_ptr->mode_info.gw_info.mm_information.plmn.identity[i] =
              ss_info_ptr->mode_info.gw_info.mm_information.plmn.identity[i];
        }

      }
      else
      {

        memset((void *) &log_ptr->mode_info.gw_info.mm_information.plmn,
               CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
               sizeof(log_ptr->mode_info.gw_info.mm_information.plmn));
      }


      if( log_ptr->mode_info.gw_info.mm_information.full_name_avail )
      {
        log_ptr->mode_info.gw_info.mm_information.full_name.length        =
              ss_info_ptr->mode_info.gw_info.mm_information.full_name.length;
        log_ptr->mode_info.gw_info.mm_information.full_name.coding_scheme =
          ss_info_ptr->mode_info.gw_info.mm_information.full_name.coding_scheme;
        log_ptr->mode_info.gw_info.mm_information.full_name.add_country_initials =
          ss_info_ptr->mode_info.gw_info.mm_information.full_name.add_country_initials;

        for( i=0; i<log_ptr->mode_info.gw_info.mm_information.full_name.length; i++)
        {
          log_ptr->mode_info.gw_info.mm_information.full_name.name[i] =
             ss_info_ptr->mode_info.gw_info.mm_information.full_name.name[i];
        }
      }
      else
      {

        memset((void *) &log_ptr->mode_info.gw_info.mm_information.full_name,
               CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
               sizeof(log_ptr->mode_info.gw_info.mm_information.full_name));
      }


      if( log_ptr->mode_info.gw_info.mm_information.short_name_avail )
      {
        log_ptr->mode_info.gw_info.mm_information.short_name.length        =
          ss_info_ptr->mode_info.gw_info.mm_information.short_name.length;
        log_ptr->mode_info.gw_info.mm_information.short_name.coding_scheme =
          ss_info_ptr->mode_info.gw_info.mm_information.short_name.coding_scheme;
        log_ptr->mode_info.gw_info.mm_information.short_name.add_country_initials =
          ss_info_ptr->mode_info.gw_info.mm_information.short_name.add_country_initials;

        for( i=0; i<log_ptr->mode_info.gw_info.mm_information.short_name.length; i++)
        {
          log_ptr->mode_info.gw_info.mm_information.short_name.name[i] =
            ss_info_ptr->mode_info.gw_info.mm_information.short_name.name[i];
        }
      }
      else
      {

        memset((void *) &log_ptr->mode_info.gw_info.mm_information.short_name,
               CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
               sizeof(log_ptr->mode_info.gw_info.mm_information.short_name));
      }

      if( log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone_avail )
      {
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.year      =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.year;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.month     =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.month;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.day       =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.day;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.hour      =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.hour;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.minute    =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.minute;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.second    =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.second;
        log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.time_zone =
          ss_info_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone.time_zone;

      }
      else
      {

        memset((void *) &log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone,
               CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
               sizeof(log_ptr->mode_info.gw_info.mm_information.univ_time_and_time_zone));
      }

      if( log_ptr->mode_info.gw_info.mm_information.lsa_identity_avail )
      {
        log_ptr->mode_info.gw_info.mm_information.lsa_identity.type =
             ss_info_ptr->mode_info.gw_info.mm_information.lsa_identity.type;

        for(i=0; i<3; i++ )
        {
          log_ptr->mode_info.gw_info.mm_information.lsa_identity.identity[i] =
            ss_info_ptr->mode_info.gw_info.mm_information.lsa_identity.identity[i];
        }
      }
      else
      {

        memset((void *) &log_ptr->mode_info.gw_info.mm_information.lsa_identity,
               CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
               sizeof(log_ptr->mode_info.gw_info.mm_information.lsa_identity));
      }

      log_ptr->mode_info.gw_info.mm_information.time_zone           =
                     ss_info_ptr->mode_info.gw_info.mm_information.time_zone;
      log_ptr->mode_info.gw_info.mm_information.daylight_saving_adj =
           ss_info_ptr->mode_info.gw_info.mm_information.daylight_saving_adj;
      break;

    default:

      /* Copy CDMA info here
      */

      log_ptr->mode_info.cdma_info.band_class      =
                                 ss_info_ptr->mode_info.cdma_info.band_class;
      log_ptr->mode_info.cdma_info.block_or_system =
                            ss_info_ptr->mode_info.cdma_info.block_or_system;
      log_ptr->mode_info.cdma_info.reg_zone        =
                                   ss_info_ptr->mode_info.cdma_info.reg_zone;
      log_ptr->mode_info.cdma_info.packet_zone     =
                                ss_info_ptr->mode_info.cdma_info.packet_zone;
      log_ptr->mode_info.cdma_info.bs_p_rev        =
                                   ss_info_ptr->mode_info.cdma_info.bs_p_rev;
      log_ptr->mode_info.cdma_info.p_rev_in_use    =
                               ss_info_ptr->mode_info.cdma_info.p_rev_in_use;
      log_ptr->mode_info.cdma_info.is_registered   =
                              ss_info_ptr->mode_info.cdma_info.is_registered;
      log_ptr->mode_info.cdma_info.ccs_supported   =
                              ss_info_ptr->mode_info.cdma_info.ccs_supported;
      log_ptr->mode_info.cdma_info.uz_id           =
                                      ss_info_ptr->mode_info.cdma_info.uz_id;

      log_ptr->mode_info.cdma_info.uz_name.uz_name_encoding =
                   ss_info_ptr->mode_info.cdma_info.uz_name.uz_name_encoding;
      log_ptr->mode_info.cdma_info.uz_name.uz_name_n_octets =
                   ss_info_ptr->mode_info.cdma_info.uz_name.uz_name_n_octets;

      for( i=0; i<SYS_PUZL_MAX_UZ_NAME_LEN; i++)
      {
        log_ptr->mode_info.cdma_info.uz_name.uz_name[i] =
                         ss_info_ptr->mode_info.cdma_info.uz_name.uz_name[i];
      }

      log_ptr->mode_info.cdma_info.srch_win_n =
                                 ss_info_ptr->mode_info.cdma_info.srch_win_n;
      log_ptr->mode_info.cdma_info.base_lat   =
                                   ss_info_ptr->mode_info.cdma_info.base_lat;
      log_ptr->mode_info.cdma_info.base_long  =
                                  ss_info_ptr->mode_info.cdma_info.base_long;
      log_ptr->mode_info.cdma_info.base_id    =
                                    ss_info_ptr->mode_info.cdma_info.base_id;

      /*Copy CDMA time info into log packet*/
      log_ptr->mode_info.cdma_info.time_info.sys_mode = 
                         ss_info_ptr->mode_info.cdma_info.time_info.sys_mode; 
      if(log_ptr->mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_CDMA)
      {
        log_ptr->mode_info.cdma_info.time_info.time.cdma_time.lp_sec = 
            ss_info_ptr->mode_info.cdma_info.time_info.time.cdma_time.lp_sec; 
        log_ptr->mode_info.cdma_info.time_info.time.cdma_time.ltm_offset = 
         ss_info_ptr->mode_info.cdma_info.time_info.time.cdma_time.ltm_offset; 
        log_ptr->mode_info.cdma_info.time_info.time.cdma_time.daylt_savings = 
         ss_info_ptr->mode_info.cdma_info.time_info.time.cdma_time.daylt_savings; 
      }
      if(log_ptr->mode_info.cdma_info.time_info.sys_mode == SYS_SYS_MODE_HDR)
      {
        log_ptr->mode_info.cdma_info.time_info.time.hdr_time.lp_sec = 
              ss_info_ptr->mode_info.cdma_info.time_info.time.hdr_time.lp_sec; 
        log_ptr->mode_info.cdma_info.time_info.time.hdr_time.ltm_offset = 
          ss_info_ptr->mode_info.cdma_info.time_info.time.hdr_time.ltm_offset; 
      }
      break;

  } /* switch (ss_info_ptr->sys_mode) */


  log_ptr->hybr_hdr_bcmcs_srv_supported = ss_info_ptr->hybr_hdr_bcmcs_srv_supported;
  log_ptr->ss_main_bcmcs_srv_supported  = ss_info_ptr->ss_main_bcmcs_srv_supported;
  log_ptr->requesting_client_id         = ss_info_ptr->requesting_client_id;

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
  #else
  log_ptr->msm_id = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
  #endif

  log_ptr->wlan_rssi              = ss_info_ptr->wlan_rssi;
  log_ptr->wlan_srv_status        = ss_info_ptr->wlan_srv_status;
  log_ptr->wlan_bss_info.chan     = ss_info_ptr->wlan_bss_info.chan;
  log_ptr->wlan_bss_info.band     = ss_info_ptr->wlan_bss_info.band;
  log_ptr->wlan_bss_info.bss_type = ss_info_ptr->wlan_bss_info.bss_type;
  log_ptr->wlan_bss_info.bssid    = ss_info_ptr->wlan_bss_info.bssid;
  log_ptr->wlan_bss_info.rssi     = ss_info_ptr->wlan_bss_info.rssi;
  log_ptr->wlan_bss_info.tech     = ss_info_ptr->wlan_bss_info.tech;

  log_ptr->wlan_bss_info.ssid.len = ss_info_ptr->wlan_bss_info.ssid.len;

  for( i=0; i<log_ptr->wlan_bss_info.ssid.len; i++ )
  {
    log_ptr->wlan_bss_info.ssid.ssid[i] = ss_info_ptr->wlan_bss_info.ssid.ssid[i];
  }

  log_ptr->wlan_stats.current_xmit_rate = ss_info_ptr->wlan_stats.current_xmit_rate;
  log_ptr->wlan_stats.total_tx_bytes    = ss_info_ptr->wlan_stats.total_tx_bytes;
  log_ptr->wlan_stats.total_rx_bytes    = ss_info_ptr->wlan_stats.total_rx_bytes;

  log_ptr->cipher_domain = ss_info_ptr->cipher_domain;

  log_ptr->main_thr_status.ss_thr_status = ss_info_ptr->main_thr_status.ss_thr_status;
  log_ptr->main_thr_status.thr_uptime    = ss_info_ptr->main_thr_status.thr_uptime;
  log_ptr->main_thr_status.idx           = ss_info_ptr->main_thr_status.idx;
  log_ptr->main_thr_status.call_status   = ss_info_ptr->main_thr_status.call_status;

  log_ptr->hdr_thr_status.ss_thr_status  = ss_info_ptr->hdr_thr_status.ss_thr_status;
  log_ptr->hdr_thr_status.thr_uptime     = ss_info_ptr->hdr_thr_status.thr_uptime;
  log_ptr->hdr_thr_status.idx            = ss_info_ptr->hdr_thr_status.idx;
  log_ptr->hdr_thr_status.call_status    = ss_info_ptr->hdr_thr_status.call_status;

  log_ptr->hdr_sys_id.id_type = ss_info_ptr->hdr_sys_id.id_type;

  switch( ss_info_ptr->hdr_sys_id.id_type )
  {

    case SYS_SYS_ID_TYPE_IS95:

      log_ptr->hdr_sys_id.id.is95.sid = ss_info_ptr->hdr_sys_id.id.is95.sid;
      log_ptr->hdr_sys_id.id.is95.nid = ss_info_ptr->hdr_sys_id.id.is95.nid;
      break;

    case SYS_SYS_ID_TYPE_IS856:

      for( i=0; i<16; i++ )
      {
        log_ptr->hdr_sys_id.id.is856[i] = ss_info_ptr->hdr_sys_id.id.is856[i];
      }
      break;

    case SYS_SYS_ID_TYPE_WLAN:

      log_ptr->hdr_sys_id.id.ssid.len   = ss_info_ptr->hdr_sys_id.id.ssid.len;
      for( i=0 ; i<ss_info_ptr->hdr_sys_id.id.ssid.len; i++ )
      {
        log_ptr->hdr_sys_id.id.ssid.ssid[i] = ss_info_ptr->hdr_sys_id.id.ssid.ssid[i];
      }
      break;


    case SYS_SYS_ID_TYPE_UMTS:

      log_ptr->hdr_sys_id.id.plmn_lac.lac = ss_info_ptr->hdr_sys_id.id.plmn_lac.lac;
      for( i=0; i<3; i++ )
      {
        log_ptr->hdr_sys_id.id.plmn_lac.plmn.identity[i] =
                        ss_info_ptr->hdr_sys_id.id.plmn_lac.plmn.identity[i];
      }
      break;

    case SYS_SYS_ID_TYPE_UNDEFINED:

      memset((void *) &log_ptr->hdr_sys_id.id, CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
              sizeof(log_ptr->hdr_sys_id.id));
      break;

    default:

      memset((void *) &log_ptr->hdr_sys_id, CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
              sizeof(log_ptr->hdr_sys_id));
      break;
  } /* switch( ss_info_ptr->hdr_sys_id.id_type ) */

  log_ptr->curr_uptime        = ss_info_ptr->curr_uptime;


  /* Below fields added for version = 5 and upwards */

  log_ptr->true_srv_status                 = ss_info_ptr->true_srv_status;
  log_ptr->signal_strength_changed_fields 
                            = ss_info_ptr->signal_strength_changed_fields;
  log_ptr->pathloss                        = ss_info_ptr->pathloss;
  log_ptr->sir                             = ss_info_ptr->sir;
  log_ptr->rssi2                           = ss_info_ptr->rssi2;
  log_ptr->rscp                            = ss_info_ptr->rscp;
  log_ptr->ecio_for_display                = ss_info_ptr->ecio_for_display;
  
  log_ptr->gw_ecio            = ss_info_ptr->gw_ecio;

  log_ptr->cell_info.cell_id = ss_info_ptr->cell_info.cell_id;
	
  log_ptr->reg_reject_info.reject_srv_domain 
                         = ss_info_ptr->reg_reject_info.reject_srv_domain;
  log_ptr->reg_reject_info.reject_cause      
                         = ss_info_ptr->reg_reject_info.reject_cause;

  log_ptr->gan_info.gan_state       = ss_info_ptr->gan_info.gan_state;
  log_ptr->gan_info.gan_conn_status = ss_info_ptr->gan_info.gan_conn_status;
  log_ptr->gan_info.gan_sig_chan_status = 
                                   ss_info_ptr->gan_info.gan_sig_chan_status;
  log_ptr->gan_info.gan_mode_display.is_mode_display_on     =
                  ss_info_ptr->gan_info.gan_mode_display.is_mode_display_on;
  log_ptr->gan_info.gan_mode_display.gan_usz_icon_indicator =
              ss_info_ptr->gan_info.gan_mode_display.gan_usz_icon_indicator;
  log_ptr->gan_info.gan_mode_display.gan_ms_location_status =
              ss_info_ptr->gan_info.gan_mode_display.gan_ms_location_status;

  memcpy((void*)log_ptr->gan_info.gan_mode_display.usz_str,
         ss_info_ptr->gan_info.gan_mode_display.usz_str,
         sizeof(log_ptr->gan_info.gan_mode_display.usz_str));

  log_ptr->gan_info.gan_error.gan_error_category =
                          ss_info_ptr->gan_info.gan_error.gan_error_category;
  log_ptr->gan_info.gan_error.gan_error_codepoint =
                         ss_info_ptr->gan_info.gan_error.gan_error_codepoint;


  /* Below fields added for version = 6 and upwards */
    
  log_ptr->cell_srv_ind.hs_ind     = ss_info_ptr->cell_srv_ind.hs_ind;
  log_ptr->cell_srv_ind.dtm_supp   = ss_info_ptr->cell_srv_ind.dtm_supp;
  log_ptr->cell_srv_ind.egprs_supp = ss_info_ptr->cell_srv_ind.egprs_supp;
  log_ptr->cell_srv_ind.hs_call_status 
                                 = ss_info_ptr->cell_srv_ind.hs_call_status;
  
  log_ptr->hdr_active_prot           = ss_info_ptr->hdr_active_prot;
  log_ptr->hybr_hdr_bcmcs_srv_status = ss_info_ptr->hybr_hdr_bcmcs_srv_status;
  log_ptr->ss_main_bcmcs_srv_status  = ss_info_ptr->ss_main_bcmcs_srv_status;

  log_ptr->ipapp_info.reg_status       = ss_info_ptr->ipapp_info.reg_status;
  log_ptr->ipapp_info.cause            = ss_info_ptr->ipapp_info.cause;

  log_ptr->ipapp_info.warn_info.present 
                              = ss_info_ptr->ipapp_info.warn_info.present;
  log_ptr->ipapp_info.warn_info.cause   
                              = ss_info_ptr->ipapp_info.warn_info.cause;
  
  memcpy((void*)log_ptr->ipapp_info.warn_info.hostname,
         ss_info_ptr->ipapp_info.warn_info.hostname,
         sizeof(log_ptr->ipapp_info.warn_info.hostname));

  memcpy((void*)log_ptr->ipapp_info.warn_info.description,
         ss_info_ptr->ipapp_info.warn_info.description,
         sizeof(log_ptr->ipapp_info.warn_info.description));
  
  log_ptr->ipapp_info.call_type_mask = ss_info_ptr->ipapp_info.call_type_mask;
  log_ptr->ipapp_info.sys_mode       = ss_info_ptr->ipapp_info.sys_mode; 

  log_ptr->active_band               = ss_info_ptr->active_band; 
  log_ptr->active_channel            = ss_info_ptr->active_channel;
  log_ptr->hybrid_active_band        = ss_info_ptr->hybrid_active_band;
  log_ptr->hybrid_active_channel     = ss_info_ptr->hybrid_active_channel;
  
  log_ptr->hdr_ln_status = ss_info_ptr->hdr_ln_status;
  
  log_ptr->country_list.present = ss_info_ptr->country_list.present;
  log_ptr->country_list.sid     = ss_info_ptr->country_list.sid;
  log_ptr->country_list.country_code_len 
                               = ss_info_ptr->country_list.country_code_len;
  
  memcpy((void*)log_ptr->country_list.country_code_list,
         ss_info_ptr->country_list.country_code_list,
         sizeof(log_ptr->country_list.country_code_list));
  
  log_ptr->current_mcc = ss_info_ptr->current_mcc;  
  log_ptr->home_mcc    = ss_info_ptr->home_mcc;
  
  log_ptr->plus_dial_setting = ss_info_ptr->plus_dial_setting;
  log_ptr->def_roam_ind      = ss_info_ptr->def_roam_ind;



  /* Submit the log to DIAG.
  */
  log_commit( (void *) log_ptr );
} /* cmlog_ss_event_handler */


/*===========================================================================
FUNCTION  cmlog_call_event_report_payload

Description:

       Generate DIAG events corresponding to given call event

===========================================================================*/
static void cmlog_call_event_report_payload(cm_call_event_e_type          call_event,
                                            const cm_mm_call_info_s_type *call_info_ptr
)
{
  cm_call_id_type         call_id;
    /* call id
    */

  int                     call_state_list_index;
    /* Code to log call id, call state and call type of all calls
    */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_info_ptr != NULL);
  CM_ASSERT(!BETWEEN(CM_CALL_EVENT_NONE, call_event, CM_CALL_EVENT_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
  /*
  **  Ignore these events if they arrived with CM_CALL_ID_UNASSIGNED
  **  ie. were sent via cmxcall_event_generic()
  */
  if ((call_event == CM_CALL_EVENT_SIGNAL)             ||
      (call_event == CM_CALL_EVENT_ABRV_ALERT)         ||
      (call_event == CM_CALL_EVENT_DISPLAY)            ||
      (call_event == CM_CALL_EVENT_EXT_DISP)           ||
      (call_event == CM_CALL_EVENT_EXIT_TC)            ||
      (call_event == CM_CALL_EVENT_REDIRECTING_NUMBER) ||
      (call_event == CM_CALL_EVENT_CONNECTED_NUM)      ||
      (call_event == CM_CALL_EVENT_CALLER_ID)          ||
      (call_event == CM_CALL_EVENT_CALLED_PARTY)       ||
      (call_event == CM_CALL_EVENT_EXT_BRST_INTL)
    )
  {
    if(call_info_ptr->call_id == CM_CALL_ID_UNASSIGNED)
    {
      CM_MSG_MED("CM_CALL_ID_UNASSIGNED for call event %d",call_event,0,0);
      return;
    }
  }
  #endif   /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


  call_id = call_info_ptr->call_id;


  if (call_id >= CM_CALL_ID_MAX)
  {
    MSG_ERROR ("Not enough buff to update call_state_list[] \n", 0, 0, 0);
    return;
  }

  /* Check if we have previous information about call_id provided by CM */
  call_state_list_index = cmlog_get_call_state_list_index (call_id);

  if (call_state_list_index >= CM_CALL_ID_MAX)
  {
    /* Need to create an entry for new call */
    call_state_list_index = cmlog_set_call_state_list_entry (call_id);

    if (call_state_list_index < CM_CALL_ID_MAX)
    {
      call_state_list[call_state_list_index].call_state =
                                             call_info_ptr->call_state;

      call_state_list[call_state_list_index].call_type  =
                                             call_info_ptr->call_type;
    }
    else
    {
      MSG_ERROR ("Not enough buff to update call_state_list[] \n", 0, 0, 0);
    }

  }
  else /* Already existing call id */
  {
    if (call_event == CM_CALL_EVENT_END)
    {
      call_state_list[call_state_list_index].call_id = CM_CALL_ID_INVALID;
    }
    else
    {
      call_state_list[call_state_list_index].call_state =
                                                  call_info_ptr->call_state;

      call_state_list[call_state_list_index].call_type  =
                                                  call_info_ptr->call_type;
    }
  } /* if (call_state_list_index >= CM_CALL_ID_MAX) */

  /* Code to log calls ends */

  switch ( call_event )
  {
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case CM_CALL_EVENT_ORIG:
      {
        cmlog_call_orig_s_type call_orig;
        cmcall_s_type *call_ptr = cmcall_ptr(call_info_ptr->call_id);

        if(call_ptr == NULL)
        {
            CM_ERR( "Invalid Callid: %d", call_info_ptr->call_id,0,0);
            break;
        }

        call_orig.call_id = (uint8) call_info_ptr->call_id;
        call_orig.call_type = (uint8) call_info_ptr->call_type;
        call_orig.srv_type = (uint8) call_ptr->srv_type;
        event_report_payload( EVENT_CM_CALL_EVENT_ORIG,
          sizeof(cmlog_call_orig_s_type), &call_orig );
      }
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case CM_CALL_EVENT_CONNECT:
      {
        cmlog_call_connect_s_type call_connect;

        call_connect.call_id = (uint8) call_info_ptr->call_id;
        call_connect.call_type = (uint8) call_info_ptr->call_type;
        call_connect.sys_mode = (uint8) cmss_ptr()->info.sys_mode;
        event_report_payload( EVENT_CM_CALL_EVENT_CONNECT,
          sizeof(cmlog_call_connect_s_type), &call_connect );
      }
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case CM_CALL_EVENT_END:
      {
        cmlog_call_end_s_type call_end;

        call_end.num_calls = 1;
        call_end.call_id = (uint8) call_info_ptr->call_id;
        event_report_payload( EVENT_CM_CALL_EVENT_END,
          sizeof(cmlog_call_end_s_type), &call_end );
      }
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    #ifndef FEATURE_PS_DORMANT_PWR_SAVE
    case CM_CALL_EVENT_PS_DATA_AVAILABLE:
      {
        uint8 sys_mode;

        sys_mode = (uint8) cmss_ptr()->info.sys_mode;
        event_report_payload( EVENT_CM_DATA_AVAILABLE,
          sizeof(sys_mode), &sys_mode );
      }
      break;
    #endif /* FEATURE_PS_DORMANT_PWR_SAVE */

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    #if defined(FEATURE_WCDMA)
#error code not present
    #endif /* FEATURE_WCDMA */

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    case CM_CALL_EVENT_CALL_ORIG_THR:
      {
        cmlog_call_orig_s_type call_orig;
        cmcall_s_type *call_ptr = cmcall_ptr(call_info_ptr->call_id);
        if(call_ptr == NULL)
        {
            CM_MSG_HIGH( "call_ptr_NULL",0,0,0);
            break;
        }
        call_orig.call_id = (uint8) call_info_ptr->call_id;
        call_orig.call_type = (uint8) call_info_ptr->call_type;
        call_orig.srv_type = (uint8) call_ptr->srv_type;
        event_report_payload( EVENT_CM_CALL_EVENT_ORIG_THR,
          sizeof(cmlog_call_orig_s_type), &call_orig );
      }
      break;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      break;
  } /* switch */
}

/*===========================================================================
FUNCTION  cmlog_call_event_handler

Description:

       Generate log packet for given call event.

===========================================================================*/
/*lint -esym(765, cmlog_call_event_handler)
** Can't be static, registered to be called by CM for call event handling
*/
void cmlog_call_event_handler( cm_call_event_e_type          call_event,
                                      const cm_mm_call_info_s_type *call_info_ptr )
{

  int                      i            = 0;
    /* Loop index
    */

  unsigned int             log_pkt_len  = 0;
    /* Length of packet to be logged.
    */

  LOG_CM_CALL_EVENT_C_type *log_ptr     = NULL;
    /* Log pointer to the log record type.
    */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_info_ptr != NULL);
  CM_ASSERT(!BETWEEN(CM_CALL_EVENT_NONE, call_event, CM_CALL_EVENT_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Generate DIAG events for call event as appropriate
  */
  cmlog_call_event_report_payload(call_event, call_info_ptr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Generate log packet for call event
  */

  /*
  ** Compute log packet length
  */
  log_pkt_len = sizeof(LOG_CM_CALL_EVENT_C_type);

  /* Allocate a log packet.
  */
  log_ptr = (LOG_CM_CALL_EVENT_C_type *) log_alloc_ex( LOG_CM_CALL_EVENT_C,
                                                       log_pkt_len );
  /*
  ** If allocation failed, just return.
  ** It means that either logging of LOG_CM_CALL_EVENT_C is off
  ** or DIAG is truly out of memory (rare)
  */
  if ( log_ptr == NULL )
  {
    CM_MSG_LOW("Log packet buffer allocation failed!",0,0,0);
    return;
  }

  /*lint -save -e641 Converting enum to int */

  /*
  ** Write log packet version
  ** This is always set to CM_LOG_CALL_EVENT_LOG_PACKET_VERSION
  */
  log_ptr->version        = CM_LOG_CALL_EVENT_LOG_PACKET_VERSION;

  /*
  ** Copy field by field from call_info_ptr
  ** Cannot do memcpy() etc because we're copying from something that
  ** is padded into something that is packed
  */

  log_ptr->call_event     = call_event;
  log_ptr->call_client_id = call_info_ptr->call_client_id;
  log_ptr->call_id        = call_info_ptr->call_id;
  log_ptr->call_state     = call_info_ptr->call_state;
  log_ptr->call_type      = call_info_ptr->call_type;


  /* Copy MIN() to not overflow log packet buffer
  */
  log_ptr->num.len        = MIN(call_info_ptr->num.len,
                                ARR_SIZE(log_ptr->num.buf));

  for(i = 0; i < log_ptr->num.len; i++)
  {
    log_ptr->num.buf[i]   = call_info_ptr->num.buf[i];
  }

  log_ptr->num.digit_mode              = call_info_ptr->num.digit_mode;
  log_ptr->num.number_type             = call_info_ptr->num.number_type;


  log_ptr->num.number_plan             = call_info_ptr->num.number_plan;
  log_ptr->num.pi                      = call_info_ptr->num.pi;
  log_ptr->num.si                      = call_info_ptr->num.si;


  log_ptr->num.subaddr.extension_bit
                                  = call_info_ptr->num.subaddr.extension_bit;
  log_ptr->num.subaddr.subadd_type  = call_info_ptr->num.subaddr.subadd_type;
  log_ptr->num.subaddr.odd_even_ind
                                   = call_info_ptr->num.subaddr.odd_even_ind;

  /* Copy MIN() to not overflow log packet buffer
  */
  log_ptr->num.subaddr.num_char  = MIN(call_info_ptr->num.subaddr.num_char,
                                       ARR_SIZE(log_ptr->num.subaddr.chari));

  for(i = 0; i < log_ptr->num.subaddr.num_char; i++)
  {
    log_ptr->num.subaddr.chari[i]      = call_info_ptr->num.subaddr.chari[i];
  }

  /* Copy MIN() to not overflow log packet buffer
  */
  log_ptr->redirecting_number.num.len =
                          MIN(call_info_ptr->redirecting_number.num.len,
                              ARR_SIZE(log_ptr->redirecting_number.num.buf));

  for(i = 0; i < log_ptr->redirecting_number.num.len; i++)
  {
    log_ptr->redirecting_number.num.buf[i]
                              = call_info_ptr->redirecting_number.num.buf[i];
  }

  log_ptr->redirecting_number.num.digit_mode
                          = call_info_ptr->redirecting_number.num.digit_mode;
  log_ptr->redirecting_number.num.number_type
                         = call_info_ptr->redirecting_number.num.number_type;
  log_ptr->redirecting_number.num.number_plan
                         = call_info_ptr->redirecting_number.num.number_plan;
  log_ptr->redirecting_number.num.pi
                                  = call_info_ptr->redirecting_number.num.pi;
  log_ptr->redirecting_number.num.si
                                  = call_info_ptr->redirecting_number.num.si;
  log_ptr->redirecting_number.num.subaddr.extension_bit
               = call_info_ptr->redirecting_number.num.subaddr.extension_bit;


  log_ptr->redirecting_number.num.subaddr.subadd_type
                 = call_info_ptr->redirecting_number.num.subaddr.subadd_type;
  log_ptr->redirecting_number.num.subaddr.odd_even_ind
                = call_info_ptr->redirecting_number.num.subaddr.odd_even_ind;

  /* Copy MIN() to not overflow log packet buffer
  */
  log_ptr->redirecting_number.num.subaddr.num_char
                = MIN(call_info_ptr->redirecting_number.num.subaddr.num_char,
                      ARR_SIZE(log_ptr->redirecting_number.num.subaddr.chari));

  for(i = 0; i < log_ptr->redirecting_number.num.subaddr.num_char; i++)
  {
   log_ptr->redirecting_number.num.subaddr.chari[i]
                    = call_info_ptr->redirecting_number.num.subaddr.chari[i];
  }
  log_ptr->redirecting_number.redirecting_reason =
                        call_info_ptr->redirecting_number.redirecting_reason;

  /* Copy MIN() to not overflow log packet buffer
  */
  log_ptr->alpha.len        = MIN(call_info_ptr->alpha.len,
                                  ARR_SIZE(log_ptr->alpha.buf));

  for(i = 0; i < log_ptr->alpha.len; i++)
  {
   log_ptr->alpha.buf[i]        = call_info_ptr->alpha.buf[i];
  }

  log_ptr->signal.is_signal_info_avail =
                                  call_info_ptr->signal.is_signal_info_avail;

  if(log_ptr->signal.is_signal_info_avail)
  {
    log_ptr->signal.signal_type = call_info_ptr->signal.signal_type;
    log_ptr->signal.alert_pitch = call_info_ptr->signal.alert_pitch;
    log_ptr->signal.signal = call_info_ptr->signal.signal;
  }
  else
  {
    log_ptr->signal.signal_type = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
    log_ptr->signal.alert_pitch = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
    log_ptr->signal.signal      = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
  }

  log_ptr->end_status = call_info_ptr->end_status;
  log_ptr->num_call_ids_ended = call_info_ptr->num_call_ids_ended;

  log_ptr->end_params.call_id = call_info_ptr->end_params.call_id;
  log_ptr->end_params.info_type = call_info_ptr->end_params.info_type;

  if(log_ptr->end_params.info_type == CM_CALL_MODE_INFO_GW_CS)
  {
    log_ptr->end_params.end_params.gw_cs_end.reserved =
                     call_info_ptr->end_params.end_params.gw_cs_end.reserved;
  }
  else if(log_ptr->end_params.info_type == CM_CALL_MODE_INFO_GW_PS)
  {
    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
    #endif
  }
  else if(log_ptr->end_params.info_type == CM_CALL_MODE_INFO_CDMA)
  {

    #if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900))
    log_ptr->end_params.end_params.cdma_end.end_reason_included =
           call_info_ptr->end_params.end_params.cdma_end.end_reason_included;

    if(log_ptr->end_params.end_params.cdma_end.end_reason_included)
    {
      log_ptr->end_params.end_params.cdma_end.end_reason =
                    call_info_ptr->end_params.end_params.cdma_end.end_reason;
    }
    else
    {
      log_ptr->end_params.end_params.cdma_end.end_reason =
                                          CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
    }
    #endif

  }
  else if(log_ptr->end_params.info_type == CM_CALL_MODE_INFO_NONE)
  {
   /* Fill with N/A
   */
   memset((void *) &log_ptr->end_params.end_params,
           CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
           sizeof(log_ptr->end_params.end_params));
  }
  else
  {
    CM_MSG_MED("info_type not set!",0,0,0);

    /* Fill with N/A
    */
    memset((void *) &log_ptr->end_params.end_params,
           CM_LOG_PACKET_CONTENTS_NOT_DEFINED,
           sizeof(log_ptr->end_params.end_params));
  }

  log_ptr->sys_mode            = call_info_ptr->sys_mode;
  log_ptr->call_connect_time   = call_info_ptr->call_connect_time;
  log_ptr->call_end_time       = call_info_ptr->call_end_time;
  log_ptr->bytes_received      = call_info_ptr->bytes_received;
  log_ptr->bytes_transmitted   = call_info_ptr->bytes_transmitted;

  log_ptr->line   = call_info_ptr->line;
  log_ptr->requesting_client_id   = call_info_ptr->requesting_client_id;

  #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
  #else
  log_ptr->msm_id   = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
  #endif

  log_ptr->direction   = call_info_ptr->direction;
  if(call_event == CM_CALL_EVENT_OTASP_STATUS)
  {
    log_ptr->otasp_status = call_info_ptr->mode_info.info.cdma_call.otasp_status;
  }
  else
  {
    log_ptr->otasp_status = CM_LOG_PACKET_CONTENTS_NOT_DEFINED;
  }
  /* Submit the log to DIAG.
  */
  log_commit( (void *) log_ptr );

  /*lint -restore 641 Converting enum to int */

}


/*===========================================================================
Function: cmlog_cm_state_info
Description:

       This function processes the umts specific call manager state
       information request packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_state_info)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_state_info
(
  PACKED void* req_pkt, uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_STATE_INFO_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_STATE_INFO,
                                 rsp_len );

  if (rsp_ptr != NULL)
  {
    /* Fill the response packet */
    /* Query Call Manager for State information */
    rsp_ptr->call_state            = cmlog_get_call_state();
    rsp_ptr->oprt_mode             = diag_client.oprt_mode;
    rsp_ptr->system_mode           = cmlog_get_system_mode();
    rsp_ptr->mode_pref             = diag_client.mode_pref;
    rsp_ptr->acq_order_pref        = diag_client.acq_order_pref;
    rsp_ptr->srv_domain_pref       = diag_client.srv_domain_pref;
    rsp_ptr->band_pref             = (int)diag_client.band_pref;
    rsp_ptr->roam_pref             = diag_client.roam_pref;
    rsp_ptr->hybr_pref             = diag_client.hybr_pref;
    rsp_ptr->network_sel_mode_pref = diag_client.network_sel_mode_pref;
    rsp_ptr->srv_status            = diag_client.srv_status;
  }
  /*lint -restore */

  return rsp_ptr ;
}
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) */

/*===========================================================================
Function: cmlog_cm_plmn_list
Description:

       This function processes the call manager plmn list request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_get_plmn_list)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_get_plmn_list
(
  PACKED void* req_pkt, uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt) */
/*lint -esym(715,pkt_len) */
{
    DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type *rsp_ptr ;

    /*lint -save -e641*/
    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_PLMN_SELECTION_MODE,
                                   sizeof(DIAG_SUBSYS_CM_CM_PLMN_SELECTION_MODE_rsp_type) );
    /*lint -restore */

    if (rsp_ptr != NULL)
    {   /* Fill the response packet */
        rsp_ptr->status = cm_ph_cmd_get_networks(
            NULL,
            NULL,
            diag_client.client,
            CM_NETWORK_LIST_AVAILABLE,
            CM_MODE_PREF_GSM_WCDMA_ONLY
            );
    }

    return rsp_ptr ;
}
/*lint +esym(715,req_pkt) */
/*lint +esym(715,pkt_len) */
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

/*===========================================================================
Function: cmlog_plmn_list_event
Description:

       This function sends a diag event when the PLMN NETWORK LIST arrives.

============================================================================*/
void  cmlog_plmn_list_event
(
        sys_detailed_plmn_list_s_type *plmn_list_ptr
            /* Pointer to a structure containing the list of available PLMNs.
            */
)
{
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(plmn_list_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for( i=0; i< plmn_list_ptr->length; ++i)
  {
    event_report_payload( EVENT_CM_PLMN_FOUND,
                          sizeof(sys_detailed_plmn_list_info_s_type),
                          &plmn_list_ptr->info[i] );
  }
}

/*===========================================================================
Function: cmlog_cm_system_selection
Description:

       This function processes the call manager system selection request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_set_system_selection)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_set_system_selection (PACKED void* req_pkt, uint16 pkt_len)
{
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type*)req_pkt;
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type *rsp_ptr;

    /*lint -save -e641*/

    cm_network_sel_mode_pref_e_type  nw_sel_mode_pref;
    sys_plmn_id_s_type               plmn_struct;

    cm_mode_pref_e_type               mode_pref;
    cm_gw_acq_order_pref_e_type       acq_order_pref;
    cm_band_pref_e_type               band_pref;
    cm_roam_pref_e_type               roam_pref;
    cm_hybr_pref_e_type               hybr_pref;
    cm_srv_domain_pref_e_type         srv_domain_pref;

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_SYSTEM_SELECTION_PREFERENCE,
                                   sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_rsp_type) );

    /* Ensure response packet allocation */
    if (!rsp_ptr)
    {
        return NULL;
    }
    /* Sanity check request packet */
    if (!req_ptr || pkt_len < sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE_req_type) )
    {
        rsp_ptr->status = FALSE;

    } else {
        /* Get params from Request Packet */
        /* Convert from little endian notation */
        plmn_struct.identity[0] = req_ptr->PLMN[2];
        plmn_struct.identity[1] = req_ptr->PLMN[1];
        plmn_struct.identity[2] = req_ptr->PLMN[0];

        /*lint -save -e650 -e737
        **            Loss of sign in promotion from int to unsigned int
        **            Constant out of range for operator '!='
        **            AST picked 0xFFFFFFFF to mean "no change"
        **            so we just disable those two here
        */
        if (((req_ptr->mode_pref >= CM_MODE_PREF_MAX) &&
              (req_ptr->mode_pref != 0xFFFFFFFF))                         ||

            ((req_ptr->acq_order_pref >= CM_GW_ACQ_ORDER_PREF_MAX) &&
             (req_ptr->acq_order_pref != 0xFFFFFFFF ))                     ||

            ((req_ptr->band_pref >= CM_BAND_PREF_MAX) &&
             (req_ptr->band_pref != 0xFFFFFFFF))                          ||

            ((req_ptr->roam_pref >= CM_ROAM_PREF_MAX) &&
             (req_ptr->roam_pref != 0xFFFFFFFF))                          ||

            ((req_ptr->hybr_pref >= CM_HYBR_PREF_MAX) &&
             (req_ptr->hybr_pref != 0xFFFFFFFF))                          ||

            ((req_ptr->srv_domain_pref >= CM_SRV_DOMAIN_PREF_MAX) &&
             (req_ptr->srv_domain_pref != 0xFFFFFFFF))                    ||

            ((req_ptr->network_sel_mode_pref >= CM_NETWORK_SEL_MODE_PREF_MAX) &&
             (req_ptr->network_sel_mode_pref != 0xFF)))
        {
          MSG_ERROR ("One or more request parameters had illegal values \n", 0, 0, 0);
        }
        /*lint -restore */

        if( req_ptr->network_sel_mode_pref < CM_NETWORK_SEL_MODE_PREF_MAX )
        {
          nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
        }
        else
        {
          nw_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;
        }

        if( req_ptr->mode_pref < CM_MODE_PREF_MAX )
        {
            mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
        }
        else
        {
            mode_pref = CM_MODE_PREF_NO_CHANGE;
        }

        if( req_ptr->acq_order_pref < CM_GW_ACQ_ORDER_PREF_MAX )
        {
            acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
        }
        else
        {
            acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
        }

        /*lint -save -e650 Constant '-2147483647' out of range for operator '<'
        ** Band pref went to 64 bits.
        ** Can't modify req packet so just suppress it and accept that we'll
        ** have garbage there
        */
        /* This CM ss pref dispatch command/resp uses 32bit band pref.
        ** The value of CM_BAND_PREF_NO_CHANGE is unchanged when
        ** band pref is increased from 32bit to 64bit.
        */
        if( req_ptr->band_pref < CM_BAND_PREF_NO_CHANGE )
        {
            band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
        }
        else
        {
            band_pref = CM_BAND_PREF_NO_CHANGE;
        }
        /*lint -restore */

        if( req_ptr->roam_pref < CM_ROAM_PREF_MAX )
        {
            roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
        }
        else
        {
            roam_pref = CM_ROAM_PREF_NO_CHANGE;
        }

        if( req_ptr->hybr_pref < CM_HYBR_PREF_MAX )
        {
            hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
        }
        else
        {
            hybr_pref = CM_HYBR_PREF_NO_CHANGE;
        }

        if( req_ptr->srv_domain_pref < CM_SRV_DOMAIN_PREF_MAX )
        {
            srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
        }
        else
        {
            srv_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
        }


        /* Issue SYS_SEL_PREF & Fill the response packet */
        rsp_ptr->status = cm_ph_cmd_sys_sel_pref
              (
                NULL,
                NULL,
                diag_client.client,
                mode_pref,
                CM_PREF_TERM_PERMANENT, /* Store in NV */
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct,
                NULL
             );
    }
    /*lint -restore */

    return rsp_ptr ;
}

/*===========================================================================
Function: cmlog_get_max_buff_size
Description:

       This function returns the max allowed buffer size based on the size of
       the payload (which is passed as argument)

============================================================================*/
static byte cmlog_get_max_buff_size(byte a)
{
  return ( (a > 32) ? (32) : (a) );
}

/*===========================================================================
Function: cmlog_service_cnf_event
Description:

       This function sends a diag event when a SERVICE CONFIRMATION arrives.

============================================================================*/
void  cmlog_service_cnf_event
(
    sys_plmn_service_state_s_type   *srv_info_ptr
       /* Pointer to the service state info received in the report.
       */
)
{
  byte payload_size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(srv_info_ptr != NULL);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the current service information in diag client */
  diag_client.srv_status = srv_info_ptr->service_status;

  payload_size = cmlog_get_max_buff_size(sizeof(sys_plmn_service_state_s_type));

  event_report_payload( EVENT_CM_SERVICE_CONFIRMED,
                        payload_size,
                        srv_info_ptr );

}

/*===========================================================================
Function: cmlog_check_request_length
Description:

       Function checks the length of packet with its associated type and returns TRUE/FALSE.
       It is important to modify this function depending on changes made to packet structures
       of WCDMA/GSM call origination, call end, set closed user group, set password and send
       USSD reply.
============================================================================*/
static boolean cmlog_check_request_length
(  int pkt_len,
   PACKED const void* req_ptr,
   int packet_type
)
{
  int minimum_length;
  int expected_length;

  if (req_ptr == NULL)
  {
    return FALSE;
  }

  if (packet_type == CM_WCDMA_CALL_ORIGINATION)
  {

    /* num_digits and amr_rate and at least one digit */
    minimum_length = sizeof(diagpkt_subsys_header_type)
                     + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                     + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->amr_rate)
                     + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

    if (pkt_len < minimum_length)
      return FALSE;

    /* digits[] should have as many digits indicated by num_digits */
    expected_length = sizeof(diagpkt_subsys_header_type)
                     + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                     + sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->amr_rate)
                     + ((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->num_digits *
                       sizeof (((DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

    if ((pkt_len < expected_length) ||
         (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_WCDMA_CALL_ORIGINATION_req_type)))

    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_GSM_CALL_ORIGINATION)
  {
    /* num_digits and at least one digit */
    minimum_length = sizeof(diagpkt_subsys_header_type)
                     + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                     + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

    if (pkt_len < minimum_length)
      return FALSE;

    /* digits[] should have as many digits indicated by num_digits */
    expected_length = sizeof(diagpkt_subsys_header_type)
                      + sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits)
                      + ((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->num_digits *
                        sizeof(((DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type *)req_ptr)->digits[0]);

    if ((pkt_len < expected_length) ||
         (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_GSM_CALL_ORIGINATION_req_type)))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_CALL_END)
  {
    /* call_id_count and at one parameter value*/
    minimum_length = sizeof(diagpkt_subsys_header_type) +
                      + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count)
                      + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->end_params[0]);

    if (pkt_len < minimum_length)
    {
      return FALSE;
    }

    /* digits[] should have as many digits indicated by num_digits */
    expected_length = sizeof(diagpkt_subsys_header_type)
                      + sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count)
                      + ((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->call_id_count *
                        sizeof(((DIAG_SUBSYS_CM_CM_CALL_END_req_type *)req_ptr)->end_params[0]);

    if ((pkt_len < expected_length) ||
          (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_CALL_END_req_type)))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_SET_CLOSED_USER_GROUP_PREFERENCE)
  {
    if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SET_CLOSED_USER_GROUP_PREFERENCE_req_type))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_SET_PASSWORD)
  {
    if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SET_PASSWORD_req_type))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_SEND_USSD_REPLY)
  {

    /* call_id_count and at one parameter value */
    minimum_length = sizeof(diagpkt_subsys_header_type)
                     + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->invoke_id)
                     + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->ussd_type)
                     + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size)
                     + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data[0]);

    if (pkt_len < minimum_length)
      return FALSE;

     /* reply_data[] should have as many bytes indictaed by reply_data_size */
    expected_length = sizeof(diagpkt_subsys_header_type)
                      + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->invoke_id)
                      + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->ussd_type)
                      + sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size)
                      + ((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data_size *
                        sizeof(((DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type *)req_ptr)->reply_data[0]);

    if ((pkt_len < expected_length) ||
         (pkt_len > (int) sizeof(DIAG_SUBSYS_CM_CM_SEND_USSD_REPLY_req_type)))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_SYSTEM_SELECTION_PREFERENCE2)
  {
    if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_GET_CELL_SERVICE_IND)
  {
    if ( pkt_len != (int)sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_req_type))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else if (packet_type == CM_COUNTRY_SELECTED_BY_USER)
  {
    if (pkt_len != (int) sizeof(DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_req_type))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    return FALSE;
  }

} /* cmlog_check_request_length (int pkt_len, PACKED void* req_pkt, int packet_type) */



#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */


/*===========================================================================
Function: cmlog_cm_get_call_information_list
Description:

       This function returns call_id, call_state and call_type for all allocated
       call ids.
============================================================================*/
/*lint -esym(765, cmlog_cm_get_call_information_list)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_get_call_information_list
(
  PACKED void* req_pkt,
  uint16 pkt_len
)
/*lint -esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */
/*lint -esym(715,req_pkt)*/
/*lint -esym(715,pkt_len)*/
{
  uint8 loop = 0;

  DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type *rsp_ptr ;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type);

  /* Stops Lint complain DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_req_type */
  if (((DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_req_type *) req_pkt) == NULL)
  {
    MSG_LOW ("cmlog_cm_get_call_information_list, req_pkt is NULL \n", 0, 0, 0);
  }

  /*lint -save -e641*/
  /* Allocate the memory for response */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CALL_INFORMATION_LIST_rsp_type *)
                diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                 CM_GET_CALL_INFORMATION_LIST, rsp_len );
  /*lint -restore */

  if (rsp_ptr != NULL)
  {
    rsp_ptr->status = TRUE;
  }
  else
  {
    return rsp_ptr;
  }

  /* Initialize number of calls to 0 */
  rsp_ptr->num_calls = 0;

  /* Fill in the call information */
  for (loop=0;loop<CM_CALL_ID_MAX;++loop)
  {
    if (call_state_list[loop].call_id != CM_CALL_ID_INVALID)
    {

      rsp_ptr->num_calls = rsp_ptr->num_calls + 1;

      /* Update rsp_ptr */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_id            =
        (byte) call_state_list[loop].call_id;

      /* call type */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_type          =
        (byte) call_state_list[loop].call_type;

      /* call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].call_state         =
        (byte) call_state_list[loop].call_state;

      /* ps call state */
      rsp_ptr->call_info[rsp_ptr->num_calls - 1].is_ps_call_dormant =
        (byte) call_state_list[loop].is_ps_call_dormant;

    }


  }/* for () */

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif

  return rsp_ptr;

} /* cmlog_cm_get_call_information_list () */
/*lint +esym(715,req_pkt)*/
/*lint +esym(715,pkt_len)*/
/*lint +esym(818, req_pkt) diagpkt_user_table_entry_type prohibits that */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================
Function: cmlog_cm_call_orig
Description:

       This function processes the call manager call origination request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_call_orig)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_call_orig (PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(715,pkt_len) */
{
  DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_req_type *req_ptr
          = (DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_req_type*)req_pkt;
  DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type *rsp_ptr;

  cm_num_s_type               called_number;
  cm_num_s_type               calling_number;
  cm_orig_alpha_s_type        alpha;
  cm_cdma_orig_params_s_type  cdma_orig_params;
  cm_call_id_type             call_id;

  /*-----------------------------------------------------------------------*/

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_CDMA_CALL_ORIGINATION,
    sizeof(DIAG_SUBSYS_CM_CM_CDMA_CALL_ORIGINATION_rsp_type) );
  /*lint-restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
      return NULL;
  }

  /* Sanity check request packet */
  if ( !req_ptr )
  {
      rsp_ptr->status = FALSE;
  }
  else
  {
    /* Set the called number */
    cm_num_init( &called_number );
    if ( req_ptr->called_calling_num_count > 0 )
    {
      /*
      called_number.len = MIN( req_ptr->number[0].digits_count,
        CM_MAX_NUMBER_CHARS );
      memcpy( called_number.buf, (const byte *) req_ptr->number[0].digits,
        called_number.len );
      called_number.digit_mode = (cm_digit_mode_e_type)(req_ptr->number[0].digit_mode);

      */

      /* Sets the digit mode logic right */
      cm_num_fill(&called_number, (const byte *) req_ptr->number[0].digits,
          MIN( req_ptr->number[0].digits_count, CM_MAX_NUMBER_CHARS ),
          (cm_digit_mode_e_type)(req_ptr->number[0].digit_mode));

      called_number.number_type = req_ptr->number[0].number_type;
      called_number.number_plan = req_ptr->number[0].number_plan;
      called_number.pi = req_ptr->number[0].presentation_ind;
      called_number.si = req_ptr->number[0].screening_ind;
      called_number.subaddr.extension_bit = req_ptr->number[0].subaddr_extension;
      called_number.subaddr.subadd_type = req_ptr->number[0].subaddr_type;
      called_number.subaddr.odd_even_ind = req_ptr->number[0].subaddr_odd_even_ind;
      called_number.subaddr.num_char = MIN( req_ptr->number[0].subaddr_count,
        CM_MAX_SUBADDRESS_DIGITS );
      memcpy( called_number.subaddr.chari, (const byte *) req_ptr->number[0].subaddr,
        called_number.subaddr.num_char );
    }

    /* Set the calling number */
    cm_num_init( &calling_number );
    if ( req_ptr->called_calling_num_count > 1 )
    {
      calling_number.len = MIN( req_ptr->number[1].digits_count,
        CM_MAX_NUMBER_CHARS );
      memcpy( calling_number.buf, (const byte *) req_ptr->number[1].digits,
        calling_number.len );
      calling_number.digit_mode = (cm_digit_mode_e_type)(req_ptr->number[1].digit_mode);
      calling_number.number_type = req_ptr->number[1].number_type;
      calling_number.number_plan = req_ptr->number[1].number_plan;
      calling_number.pi = req_ptr->number[1].presentation_ind;
      calling_number.si = req_ptr->number[1].screening_ind;
      calling_number.subaddr.extension_bit = req_ptr->number[1].subaddr_extension;
      calling_number.subaddr.subadd_type = req_ptr->number[1].subaddr_type;
      calling_number.subaddr.odd_even_ind = req_ptr->number[1].subaddr_odd_even_ind;
      calling_number.subaddr.num_char = MIN( req_ptr->number[1].subaddr_count,
        CM_MAX_SUBADDRESS_DIGITS );
      memcpy( calling_number.subaddr.chari, (const byte *) req_ptr->number[1].subaddr,
        calling_number.subaddr.num_char );
    }

    /* Set alpha buffer */
    alpha.len = MIN( req_ptr->alpha_digits_count,
      CMLOG_CM_MAX_ALPHA_DIGITS );
    memcpy( alpha.buf, (char*) req_ptr->alpha_digits, alpha.len );

    /* Set the cdma specific orig parameters */
    cdma_orig_params.srv_opt = req_ptr->service_option;
    cdma_orig_params.activate_code = (cm_activate_code_e_type)(req_ptr->otasp_activate_code);
    cdma_orig_params.drs_bit = req_ptr->drs_bit;
    cdma_orig_params.sr_id_included = req_ptr->sr_id_included;
    cdma_orig_params.sr_id = req_ptr->sr_id;
    cdma_orig_params.qos_parms_incl = req_ptr->qos_params_included;
    if ( cdma_orig_params.qos_parms_incl )
    {
      cdma_orig_params.qos_parms_len = MIN(req_ptr->qos_params_count,
        CM_ORIG_QOS_LEN_MAX);
      memcpy( cdma_orig_params.qos_parms, (char*) req_ptr->qos_params,
        cdma_orig_params.qos_parms_len );
    }
    cdma_orig_params.last_act_data_net = (sys_sys_mode_e_type)(req_ptr->hdr_last_act_data_net);

    /* Issue SYS_SEL_PREF & Fill the response packet */
    rsp_ptr->status =
           cm_mm_call_cmd_orig( NULL,
                                NULL,
                                diag_client.client,
                                (cm_call_type_e_type)(req_ptr->call_type),
                                (cm_srv_type_e_type)(req_ptr->srv_type),
                                &calling_number,
                                &called_number,
                                &alpha,
                                &cdma_orig_params,
                                NULL,
                                NULL,
                                &call_id );
    rsp_ptr->call_id = call_id;
  }

  return rsp_ptr;

} /* cmlog_cm_call_orig() */
/*lint +esym(715,pkt_len) */

#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

/*===========================================================================
Function: cmlog_cm_call_end
Description:

       This function processes the call manager call end request
       packet from QXDM and sends the response back.

============================================================================*/
/*lint -esym(765, cmlog_cm_call_end)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_call_end (PACKED void* req_pkt, uint16 pkt_len)
{
  DIAG_SUBSYS_CM_CM_CALL_END_req_type  *req_ptr =
    (DIAG_SUBSYS_CM_CM_CALL_END_req_type*) req_pkt;
  DIAG_SUBSYS_CM_CM_CALL_END_rsp_type  *rsp_ptr;

  uint8                 num_call_ids;
  cm_end_params_s_type  end_params[CM_DIAG_CALL_ID_MAX];
  int i;


  /*-----------------------------------------------------------------------*/

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_CALL_END_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_CM, CM_CALL_END,
    sizeof(DIAG_SUBSYS_CM_CM_CALL_END_rsp_type) );
  /*lint -restore */

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
    return NULL;
  }

  /* Check for packet length */
  if (cmlog_check_request_length (pkt_len, req_pkt, CM_CALL_END) == FALSE)
  {
    MSG_ERROR ("Packet length check failed \n", 0, 0, 0);
    return rsp_ptr;
  }

  /* Sanity check request packet */
  /* if (!req_ptr || pkt_len < sizeof(DIAG_SUBSYS_CM_CM_CALL_END_req_type) ) */
  /* Checking for packet size to conform to min length required*/
  if ( !req_ptr )
  {
    rsp_ptr->status = FALSE;
  }
  else
  {
    /* Get params from Request Packet */
    num_call_ids = MIN( req_ptr->call_id_count, CM_DIAG_CALL_ID_MAX );
    for ( i = 0; i < num_call_ids; i++ )
    {

      MSG_HIGH (" cmlog call end call id = %d \n", req_ptr->end_params[i].call_id, 0, 0);

      end_params[i].call_id = req_ptr->end_params[i].call_id;
      if ( req_ptr->end_params[i].call_mode_type == 0 )
      {
        end_params[i].info_type = CM_CALL_MODE_INFO_CDMA;
        end_params[i].end_params.cdma_end.end_reason_included =
          req_ptr->end_params[i].cdma_end_reason_included;
        end_params[i].end_params.cdma_end.end_reason =
          req_ptr->end_params[i].cdma_end_reason;
      }
      else if ( req_ptr->end_params[i].call_mode_type == 1 )
      {
        end_params[i].info_type = CM_CALL_MODE_INFO_GW_CS;
      }
      else /* It is PS */
      {
        end_params[i].info_type = CM_CALL_MODE_INFO_GW_PS;

       #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
       #endif

        end_params[i].end_params.gw_ps_end.cause =
          (sm_status_T) (req_ptr->end_params[i].gw_cause);
      }
    } /* for ( i = 0; i < num_call_ids; i++ ) */

    /* Issue the command & Fill the response packet */
    /*lint -esym(772, end_params) if not initialized then num_call_ids is 0
    ** so we don't care what's in end_params anyway
    */
    rsp_ptr->status = cm_mm_call_cmd_end( NULL, NULL, diag_client.client,
                                          num_call_ids, end_params );
    /*lint +esym(772, end_params) */
  }


  return rsp_ptr ;

} /* cmlog_cm_call_end() */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* #if ((FEATURE_WCDMA) || (FEATURE_GSM) || (FEATURE_MMGPS)) */



#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


#if (defined (FEATURE_CALL_THROTTLE))
/*===========================================================================
Function: cmlog_orig_thr_tbl_reset
Description:

       This function processes the Throttle table reset request packet
       from QXDM and sends the response back.

============================================================================*/

/*lint -esym(765, cmlog_orig_thr_tbl_reset)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_orig_thr_tbl_reset (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_req_type *req_ptr =
                    (DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_req_type *)req_pkt;
  DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type);

  /*lint -save -e641*/
  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_CM_CM_ORIG_THR_TBL_RESET_rsp_type *)
       diagpkt_subsys_alloc (DIAG_SUBSYS_CM, CM_ORIG_THR_TBL_RESET, rsp_len);
  /*lint -restore */

  SYS_ARG_NOT_USED( pkt_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {

    MSG_HIGH (" orig thr tbl reset memory alloc failed for response packet \
               \n" , 0, 0, 0);
    return NULL;
  }

  /* Fill the memory returned, with 0s */
  rsp_ptr->status = 0;

  /* Sanity check */
  if (!req_ptr)
  {
    rsp_ptr->status = FALSE;
  }
  else
  {

    rsp_ptr->status =  cm_ss_cmd_orig_thr_update_tbl(
                                            NULL,
                                            NULL,
                                            cmclient_get_client_id_for_client_type(CM_CLIENT_TYPE_DIAG),
                                            CM_SS_ORIG_THR_RESET_TBL,
                                            CM_SS_ORIG_THR_DS_MIP_FA_FAILURE,
                                            CM_CALL_TYPE_PS_DATA,
                                            SYS_SYS_MODE_NONE,
                                            NULL,
                                            SYS_BAND_BC0,
                                            0 );
  }
  return rsp_ptr ;

} /* cmlog_orig_thr_tbl_reset */

#endif /* (defined(FEATURE_CALL_THROTTLE)) */


/*===========================================================================
Function: cmlog_cm_system_selection2
Description:

       This function processes the call manager system selection request2
       packet from QXDM and sends the response back.

       Note: It expands the band preference from 32-bit to 64-bit. The rest
       fields are same as those of call manager system selection request.

============================================================================*/
/*lint -esym(765, cmlog_cm_set_system_selection2)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_set_system_selection2 (PACKED void* req_pkt, uint16 pkt_len)
{
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type *req_ptr
            = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_req_type*)req_pkt;
    DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type *rsp_ptr;
    const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type);

    /*lint -save -e641*/

    cm_network_sel_mode_pref_e_type  nw_sel_mode_pref =
                                          CM_NETWORK_SEL_MODE_PREF_NO_CHANGE;

    sys_plmn_id_s_type               plmn_struct;

    cm_mode_pref_e_type               mode_pref = CM_MODE_PREF_NO_CHANGE;
    cm_gw_acq_order_pref_e_type       acq_order_pref =
                                              CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
    cm_band_pref_e_type               band_pref = CM_BAND_PREF_NO_CHANGE;
    cm_roam_pref_e_type               roam_pref = CM_ROAM_PREF_NO_CHANGE;

    cm_hybr_pref_e_type               hybr_pref = CM_HYBR_PREF_NO_CHANGE;
    cm_srv_domain_pref_e_type         srv_domain_pref =
                                                CM_SRV_DOMAIN_PREF_NO_CHANGE;

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_SYSTEM_SELECTION_PREFERENCE2_rsp_type *)
    diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                         CM_SYSTEM_SELECTION_PREFERENCE2,
                         rsp_len );

    /* Ensure response packet allocation */
    if (!rsp_ptr)
    {
        MSG_ERROR(" System reference2 mem alloc fail for response pkt ", 0, 0, 0 );
        return NULL;
    }

    /* Sanity check request packet */
    if (!cmlog_check_request_length (
                                  pkt_len,
                                  req_pkt,
                                  CM_SYSTEM_SELECTION_PREFERENCE2))
    {
        rsp_ptr->status = FALSE;

        MSG_ERROR("System reference2 Packet length check failed \n", 0, 0, 0);
        /* Return status now.*/
        return rsp_ptr;

    }

    /* Get params from Request Packet */
    /* Convert from little endian notation */
    plmn_struct.identity[0] = req_ptr->PLMN[2];
    plmn_struct.identity[1] = req_ptr->PLMN[1];
    plmn_struct.identity[2] = req_ptr->PLMN[0];


    /* Update the system prefernce*/
    if( req_ptr->network_sel_mode_pref < (unsigned int)CM_NETWORK_SEL_MODE_PREF_MAX )
    {
      nw_sel_mode_pref = (cm_network_sel_mode_pref_e_type) req_ptr->network_sel_mode_pref;
    }
    else
    {
      MSG_ERROR ("Bad nw_sel_mode_pref %d, using no change",
                 req_ptr->network_sel_mode_pref, 0, 0);
    }

    if( req_ptr->mode_pref < (unsigned int)CM_MODE_PREF_MAX )
    {
      mode_pref = (cm_mode_pref_e_type) req_ptr->mode_pref;
    }
    else
    {
      MSG_ERROR ("Bad mode_pref %d, using no change",
                 req_ptr->mode_pref, 0, 0);
    }

    if( req_ptr->acq_order_pref < (unsigned int)CM_GW_ACQ_ORDER_PREF_MAX )
    {
      acq_order_pref = (cm_gw_acq_order_pref_e_type) req_ptr->acq_order_pref;
    }
    else
    {
      MSG_ERROR ("Bad acq_order_pref %d, using no change",
                 req_ptr->acq_order_pref, 0, 0);
    }

    if( req_ptr->band_pref < CM_BAND_PREF_MAX )
    {
      band_pref = (cm_band_pref_e_type) req_ptr->band_pref;
    }
    else
    {
      MSG_ERROR ("Bad band_pref %d, using no change",
                 req_ptr->band_pref, 0, 0);
    }

    if( req_ptr->roam_pref < (unsigned int)CM_ROAM_PREF_MAX )
    {
      roam_pref = (cm_roam_pref_e_type) req_ptr->roam_pref;
    }
    else
    {
      MSG_ERROR ("Bad roam_pref %d, using no change",
                 req_ptr->roam_pref, 0, 0);
    }

    if( req_ptr->hybr_pref < (unsigned int)CM_HYBR_PREF_MAX )
    {
      hybr_pref = (cm_hybr_pref_e_type) req_ptr->hybr_pref;
    }
    else
    {
      MSG_ERROR ("Bad hybr_pref %d, using no change",
                 req_ptr->hybr_pref, 0, 0);
    }

    if( req_ptr->srv_domain_pref < (unsigned int)CM_SRV_DOMAIN_PREF_MAX )
    {
      srv_domain_pref = (cm_srv_domain_pref_e_type) req_ptr->srv_domain_pref;
    }
    else
    {
      MSG_ERROR ("Bad srv_domain_pref %d, using no change",
                 req_ptr->srv_domain_pref, 0, 0);
    }

    /* Issue SYS_SEL_PREF & Fill the response packet */
    rsp_ptr->status = cm_ph_cmd_sys_sel_pref
              (
                NULL,
                NULL,
                diag_client.client,
                mode_pref,
                CM_PREF_TERM_PERMANENT,
                0,
                acq_order_pref,
                band_pref,
                CM_PRL_PREF_NO_CHANGE,
                roam_pref,
                hybr_pref,
                srv_domain_pref,
                nw_sel_mode_pref,
                &plmn_struct,
                NULL
             );


    CM_MSG_MED( "Status %d", rsp_ptr->status, 0, 0 );

    /* Return status.*/
    return rsp_ptr ;
}


/*===========================================================================
Function: cmlog_cm_get_cell_service_ind
Description:

       This function processes the call manager cell service indication
       request from QXDM and sends the response back.
       Note : Currently supports hsdpa_hsupa and dtm

============================================================================*/
/*lint -esym(765, cmlog_cm_get_cell_service_ind)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
PACKED void *cmlog_cm_get_cell_service_ind(PACKED void* req_pkt, uint16 pkt_len)
/*lint -esym(818, req_pkt)*/
{

  DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rsp_ptr = (DIAG_SUBSYS_CM_CM_GET_CELL_SERVICE_IND_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                                 CM_GET_CELL_SERVICE_IND,
                                 rsp_len );

  /* Ensure response packet allocation */
  if (!rsp_ptr)
  {
      MSG_ERROR(" CELL Service IND mem alloc fail for response pkt ", 0, 0, 0 );
      return NULL;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check request packet */
  if (!cmlog_check_request_length (
                                pkt_len,
                                req_pkt,
                                CM_GET_CELL_SERVICE_IND))
  {

      MSG_ERROR("CELL SERVICE IND Packet length check failed \n", 0, 0, 0);
      /* Return status now.*/
      return rsp_ptr;

  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Filling cell service indication */


  rsp_ptr->hsdpa_hsupa_support = (uint8)diag_client.cell_srv_ind.hs_ind;
  rsp_ptr->dtm_support = (uint8)diag_client.cell_srv_ind.dtm_supp;
  rsp_ptr->reserved1 = 0;
  rsp_ptr->reserved2 = 0;
  rsp_ptr->reserved3 = 0;


  return rsp_ptr;

}
/*lint +esym(818, req_pkt)*/

/*===========================================================================
Function: cmlog_cm_country_selected_by_user
Description:

       This function takes user selection for resolving a sid to a particular
       country.

============================================================================*/
/*lint -esym(765, cmlog_cm_country_selected_by_user)
** Can't be static, to be called by DIAG via COMMAND DISPATCH TABLE
*/
/*lint -esym(818, req_ptr) */
PACKED void *cmlog_cm_country_selected_by_user (PACKED void* req_ptr, uint16 pkt_len)
{
    DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_rsp_type *rsp_ptr;
    const int rsp_len = sizeof(DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_rsp_type);

    uint16 mcc_selected = 0;
      /*  country selected for sid */

  uint16 curr_sid = 0;
    /* Sid for which mcc is being sel */


    /*-----------------------------------------------------------------------*/

    /*lint -save -e641*/

    /* Allocate the memory for this */
    rsp_ptr = (DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_rsp_type *)
    diagpkt_subsys_alloc(DIAG_SUBSYS_CM,
                         CM_COUNTRY_SELECTED_BY_USER,
                         rsp_len );

    /*-----------------------------------------------------------------------*/

    /* Ensure response packet allocation */
    if (!rsp_ptr)
    {
        MSG_ERROR(" country selected by user, mem alloc fail for response pkt ", 0, 0, 0 );
        return NULL;
    }


    if (!req_ptr)
    {
      MSG_ERROR(" country selected by user req_ptr is NULL ", 0, 0, 0 );
      rsp_ptr->status = FALSE;
      return rsp_ptr;
    }

    /*-----------------------------------------------------------------------*/

    /* Sanity check request packet */
    if (!cmlog_check_request_length (
                                  pkt_len,
                                  req_ptr,
                                  CM_COUNTRY_SELECTED_BY_USER))
    {
      rsp_ptr->status = FALSE;

      MSG_ERROR("Country selected by user param length check failed \n", 0, 0, 0);

      /* Return status now.*/
      return rsp_ptr;

    }

    /*-----------------------------------------------------------------------*/

    mcc_selected =
      ((DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_req_type*)req_ptr)->country_sel_for_sid;

  curr_sid =
      ((DIAG_SUBSYS_CM_CM_COUNTRY_SELECTED_BY_USER_req_type*)req_ptr)->curr_sid;

    /*-----------------------------------------------------------------------*/

    /* Sanity check on country code values. Doing no sanity on sid
  */

  #if 0 /* Test tool can select CM_INVALID_MOBILE_COUNTRY_CODE to test user
        ** selecting no valid country
      */
    if (!BETWEEN(mcc_selected, 0, CM_MAX_MOBILE_COUNTRY_CODE_VAL))
    {

      rsp_ptr->status = FALSE;

      MSG_ERROR("Country selected error ", mcc_selected, 0, 0);

      /* Return status now.*/
      return rsp_ptr;
    }
  #endif

    CM_MSG_HIGH ("Diag country selected %d ", mcc_selected, 0, 0);

    /*-----------------------------------------------------------------------*/

    /* Issue selected country request */
    rsp_ptr->status = cm_ss_cmd_country_selected
              (
                NULL,
                diag_client.client,
        curr_sid,
                mcc_selected
             );


    CM_MSG_MED( "Status %d", rsp_ptr->status, 0, 0 );

    /* Return status.*/
    return rsp_ptr ;
}
/*lint +esym(818, req_ptr) */

#if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
#endif  /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */

/**************************************************************************
** CM SUBSYSTEM COMMAND DISPATCH TABLE.
Note:  Any new commands for CM need to be added here.
****************************************************************************/

static const diagpkt_user_table_entry_type cm_subsys_diag_tbl[] =
{
  {CM_STATE_INFO,    CM_STATE_INFO,    cmlog_cm_state_info},
  {CM_PLMN_SELECTION_MODE, CM_PLMN_SELECTION_MODE, cmlog_cm_get_plmn_list},


  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    {CM_CDMA_CALL_ORIGINATION, CM_CDMA_CALL_ORIGINATION, cmlog_cm_call_orig},
  #endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


    #if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
#error code not present
    #endif /* #ifdef FEATURE_CM_MMGPS || defined(FEATURE_CM_LCS)*/

    {CM_CALL_END, CM_CALL_END,   cmlog_cm_call_end},

    {CM_GET_CALL_INFORMATION_LIST, CM_GET_CALL_INFORMATION_LIST, cmlog_cm_get_call_information_list},

    {CM_SYSTEM_SELECTION_PREFERENCE,   CM_SYSTEM_SELECTION_PREFERENCE,   cmlog_cm_set_system_selection},

  #if defined(FEATURE_CALL_THROTTLE)
    {CM_ORIG_THR_TBL_RESET, CM_ORIG_THR_TBL_RESET, cmlog_orig_thr_tbl_reset},
  #endif /* defined(FEATURE_CALL_THROTTLE) */

   {CM_SYSTEM_SELECTION_PREFERENCE2,   CM_SYSTEM_SELECTION_PREFERENCE2,   cmlog_cm_set_system_selection2},

   {CM_GET_CELL_SERVICE_IND ,CM_GET_CELL_SERVICE_IND,   cmlog_cm_get_cell_service_ind},

  #if ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK))
#error code not present
  #endif /* ((defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) && defined (FEATURE_MULTIMEDIA_FALLBACK)) */ 
  
   {CM_COUNTRY_SELECTED_BY_USER,   CM_COUNTRY_SELECTED_BY_USER,   cmlog_cm_country_selected_by_user}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);

#endif

  void cmlog_init (void)
  {
    int loop;

    /*lint -save -e641 -e717 */
    #ifndef __cplusplus
    /*
    ** 717: do ... while(0)
    ** DIAGPKT_DISPATCH_TABLE_REGISTER expands to do..while (0) and this
    ** has been done deliberately. Suppressing lint
    */
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_CM, cm_subsys_diag_tbl);
    #endif

    /* 1) Initialize registered client. */
    diag_client.oprt_mode             = SYS_OPRT_MODE_NONE;
    diag_client.mode_pref             = CM_MODE_PREF_NONE;
    diag_client.acq_order_pref        = CM_GW_ACQ_ORDER_PREF_NONE;
    diag_client.srv_domain_pref       = CM_SRV_DOMAIN_PREF_NONE;
    diag_client.band_pref             = CM_BAND_PREF_NONE;
    diag_client.roam_pref             = CM_ROAM_PREF_NONE;
    diag_client.hybr_pref             = CM_HYBR_PREF_NONE;
    diag_client.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NONE;
    diag_client.srv_status            = SYS_SRV_STATUS_NONE;
    diag_client.offline_signal_recvd  = FALSE;
    (void) cm_client_init( CM_CLIENT_TYPE_DIAG, &diag_client.client );

    /* Invalidate all entries in cmlog_call_info_list */
    for (loop=0;loop<CM_CALL_ID_MAX;++loop)
    {
      call_state_list[loop].call_id = CM_CALL_ID_INVALID;
    }

    /* 2) Register for desired CM phone events. */
    (void) cm_client_ph_reg(
                      diag_client.client,
                      cmlog_ph_event_handler,
                      CM_CLIENT_EVENT_REG,
                      (cm_ph_event_e_type) (CM_PH_EVENT_NONE+1),
                      (cm_ph_event_e_type) (CM_PH_EVENT_MAX-1),
                      NULL                );

    (void) cm_mm_client_ss_reg(
                          diag_client.client,
                          cmlog_ss_event_handler,
                          CM_CLIENT_EVENT_REG,
                          (cm_ss_event_e_type) (CM_SS_EVENT_NONE+1),
                          (cm_ss_event_e_type) (CM_SS_EVENT_MAX-1),
                          NULL
                          );

    /* Register for supplementary service events */
    #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
    #endif

    /* Register for supplementary service events */

    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
    #endif /* #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM) ) */



    /* register for call events for diag */
    (void) cm_mm_client_call_reg(
                       diag_client.client,
                       cmlog_call_event_handler,
                       CM_CLIENT_EVENT_REG,
                       (cm_call_event_e_type) (CM_CALL_EVENT_NONE+1),
                       (cm_call_event_e_type) (CM_CALL_EVENT_MAX-1),
                       NULL );

    /* 3) Activate client. */
    (void) cm_client_act( diag_client.client );
    /*lint -restore */
  }

#if defined FEATURE_HDR_HYBRID && defined FEATURE_GPSONE
#error code not present
#endif


#if defined __cplusplus
  }
#endif

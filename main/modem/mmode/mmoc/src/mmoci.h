#ifndef MMOCI_H
#define MMOCI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the MMoC.

Copyright (c) 2002 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/APPS/vcs/mmoci.h_v   1.2   18 Jun 2002 16:40:50   sjaikuma  $
  $Header: //source/qcom/qct/modem/mmode/mmoc/rel/08H1_QSC1110_3.X/src/mmoci.h#2 $ $DateTime: 2011/03/02 23:45:43 $ $Author: skgupta $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/11   sg      MMOC would call TMC heap API to allocate memory dynamically.
05/31/09   pm      Change EXTERN define to MMEXTN to resolve compile error 
                   when other modules define EXTERN
12/18/08   sg      Get "mmoc_state_info" ptr throug a function.
11/14/08   jd      Corrected Lint errors
09/19/08   sv      Corrected Featurization FEATURE_MMOC_LOW_MEM_TARGET
09/19/08   jd      Memory reduction changes
09/12/08   fj/aj   Added support for Modem Reset
06/16/08   sv      Added MMOC_ENUM_IS_INRANGE
04/28/08   fj      Lint cleanup, CR 144389
04/16/07   ic      Moved inclusion of AEE* headers to mmoci.h
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/03/06   ka      Changing copyright.
12/30/05   ka      Adding string maps to mmoc commands, reports and state
                   transitions.
07/07/05   ka      Adding new transaction states to complete deactivation
                   for HDR.
03/02/05   ka      Handling subscription available along with protocol name
                   that has subscription available.
08/20/04   sj      Added support for prl_pref.
05/10/04   sj      Merged in changes for srch inactive state.
02/24/04   sj      Removed LINT warnings.
02/05/04   SJ      Increased srch response timer to 60 seconds.
01/13/04   sj      Added REX sig for duplicate command detection.
10/21/03   sj      Increased the cmd q size to 15 & sanity timeout to 1min.
09/15/03   sj      Fixed issues with HDR <-> 1x runtime switching.
08/15/03   sj      Changed usage of #ifdef T_PC to #if (TG==T_PC).
08/12/03   sj      Updates for placing srch in standby during power save
                   and LPM.
03/17/03   SJ      Increased mmoc's sanity timer to 20 seconds.
02/26/03   SJ      Modified sanity timeout err_fatal to print the offending
                   protocol task's name instead of mmoc.c.
02/14/03   SJ      Added support for new MMoC wakeup from pwr_save
                   transaction.
01/31/03   SJ      Increased insanity timer to 15 seconds.
12/18/02   SJ      Added mode_info_s_type type.
09/03/02   SJ      Unfeaturized GSM support in MMoC.
06/10/02   SJ      Corrected sanity timer value.
03/28/02   SJ      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "cmd.h"      /* Command header for inter-task communication */
#include "clk.h"      /* Interface for clock services */

#include "sd.h"       /* Interface for System Determination services */
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "mmoc.h"     /* External interface definitions for MMoC */

#include "task.h"     /* Interface for global task resources */
#include "dog.h"      /* Interface to watchdog procedures */

#include <stdarg.h>   /* vsprintf and va_end functions */
#include <stdio.h>    /* vsprintf function */

/* Gives prototype for std_xx calls 
*/
#include "AEEStdDef.h"
#include "AEEstd.h"
#include "AEEVaList.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG RELATED STUFF ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
      Compile MMoC with debug information, asserts and messages.
---------------------------------------------------------------------------*/
#define MMEXTN

#if !defined(_lint)
#define LOCALF
#define LOCALD extern
#else
#define LOCALF static
#define LOCALD static
#endif



/* <EJECT> */
/*---------------------------------------------------------------------------
                  MMOC_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/
#define MMOC_MSG_LOW( str, a, b, c )       MSG_LOW( "=MMOC= " str, a, b, c)
#define MMOC_MSG_MED( str, a, b, c )       MSG_MED( "=MMOC= " str, a, b, c)
#define MMOC_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=MMOC= " str, a, b, c)
#define MMOC_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=MMOC= " str, a, b, c)

#define MMOC_PRINT_MSG_HIGH                mmocdbg_print_message                                          

/* <EJECT> */
/*---------------------------------------------------------------------------
                   MMOC_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/

#ifdef MMOC_DEBUG
#error code not present
#elif (TG==T_PC)
#error code not present
#else

  #define MMOC_ERR_FATAL(str,a,b,c )      ERR_FATAL( "",a,b,c )

  #define MMOC_ERR( str,a,b,c )           ERR( "",a,b,c )
  #define MMOC_CASE_ERR( str,a,b,c )      ERR( "",a,b,c )
  #define MMOC_SWITCH_ERR( str,a,b,c )    ERR( "",a,b,c )
  #define MMOC_ELSE_ERR( str,a,b,c )      ERR( "",a,b,c )

  #define MMOC_FUNC_START( str,a,b,c )
  #define MMOC_FUNC_END( str,a,b,c )

  #define MMOC_FUNC_NOT_IMP( func )       ERR( "",0,0,0 )
  #define MMOC_CASE_NOT_IMP( _case )      ERR( "",0,0,0 )

#endif

/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#ifdef MMOC_DEBUG
#error code not present
#else
  #define MMOC_ASSERT( exp )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define MMOC_ENUM_IS_INRANGE( val, max ) \
                       /*lint -e568 -e641 Converting enum to int*/ \
                       ( (val) >= 0 && (val) < (max) ) \
                       /*lint +e568 -e641*/


#ifdef MMOC_DEBUG
#error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE( val, max )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef MMOC_DEBUG
#error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef MMOC_DEBUG
#error code not present
#else
  #define MMOC_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC COMMAND INTERFACE RELATED TYPES ===================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration for  type of command queued in the MMoC's command queue.
*/
typedef enum {

  MMOC_CMD_SUBSCRIPTION_CHGD,          /* Subscription changed command, could
                                       ** because of NAM selection change,
                                       ** GSM/WCDMA subscription availability
                                       ** change or CDMA/AMPS/HDR subscription
                                       ** availability change.
                                       */

  MMOC_CMD_PROT_GEN_CMD,               /* Generic command bound for protocol,
                                       ** could be
                                       ** pref sys changed command or
                                       ** reselection timeout command or
                                       ** get networks command for GW or
                                       ** terminate get networks command for
                                       ** GW.
                                       */

  MMOC_CMD_OPRT_MODE_CHGD,             /* Operating mode changed command type.
                                       */


  MMOC_CMD_WAKEUP_FROM_PWR_SAVE,       /* Power save wakeup command.
                                       */


  MMOC_CMD_MAX                         /* For internal use. */

} mmoc_cmd_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with subscription changed
** command to MMoC task.
*/
typedef struct {

  /* Type of subscription change.
  */
  mmoc_subsc_chg_e_type                chg_type;

  /* Current NAM selection (NAM-1, NAM-2, etc.).
  */
  byte                                 nam;

  /* The origination mode (normal, OTASP, emergency, other, etc.)
  ** that is associated with the selected NAM.
  */
  sd_ss_orig_mode_e_type               orig_mode;

  /* The mode preference that is associated with the selected NAM.
  */
  sd_ss_mode_pref_e_type               mode_pref;

  /* Band preference that is associated with the selected NAM.
  */
  sd_ss_band_pref_e_type               band_pref;

  /* PRL preference that is associated with the selected NAM.
  */
  sd_ss_prl_pref_e_type                prl_pref;

  /* Roam preference that is associated with the selected NAM.
  */
  sd_ss_roam_pref_e_type               roam_pref;

  /* New hybrid preference (none, CDMA/HDR).
  */
  sd_ss_hybr_pref_e_type               hybr_pref;

  /* Information about the manual system.
  */
  sys_manual_sys_info_s_type           manual_sys_info;

  /* Service domain preference that is associated with the selected
  ** NAM.
  ** Applies only for GSM/WCDMA modes.
  */
  sd_ss_srv_domain_pref_e_type         domain_pref;

  /* Preference for the order of acquisition ( WCDMA before GSM,
  ** GSM before WCDMA etc).
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /* Technology preference settings.
  */
  sd_ss_tech_pref_e_type               tech_pref;

  /* Preference for the wlan scanning.
  */
  sys_wlan_scan_pref_e_type            wlan_scan_pref;

  /* Preference for the wlan scanning.
  */
  sys_wlan_bss_e_type                  wlan_bss_type_pref;

  /* Current GSM/WCDMA subscription availability status.
  ** true = subscription available,
  ** false = subscription not available.
  */
  boolean                              is_gw_subsc_avail;

  /* Current CDMA/AMPS/HDR subscription availability status.
  ** true = subscription available,
  ** false = subscription not available.
  */
  boolean                              is_cdma_subsc_avail;

  /* Current WLAN subscription availability status.
  ** true = subscription available,
  ** false = subscription not available.
  */
  boolean                              is_wlan_subsc_avail;

  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  prot_subsc_chg_e_type                prot_subsc_chg;

} mmoc_cmd_subsc_chgd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with operating mode changed
** command to MMoC task.
*/
typedef struct {

  /* Indicates the new operating mode (ONLINE, OFFLINE etc) for the phone.
  */
  sys_oprt_mode_e_type                 oprt_mode;

} mmoc_cmd_oprt_mode_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with wakeup from pwr_save
** command to MMoC task.
*/
typedef struct {

  /* Indicates the instance to be woken up from pwr_save.
  */
  sd_ss_e_type                        ss;

} mmoc_cmd_wakeup_pwrsave_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Command header type.  Each command to MMoC starts with this header.
*/
typedef struct {

  /* Command header */
  cmd_hdr_type                         hdr;

  /* Command name */
  mmoc_cmd_e_type                      name;

} mmoc_cmd_hdr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with queuing a command to MMoC
** task.
*/
typedef struct {

  /* The header information that is associated with this command.
  */
  mmoc_cmd_hdr_s_type                  cmd;

  /* union of parameters that are associated with the different commands.
  */
  union   {

    /*  Parameters for subscription changed command.
    */
    mmoc_cmd_subsc_chgd_s_type           subsc_chgd;

    /* Parameters for operating mode changed command.
    */
    mmoc_cmd_oprt_mode_s_type            oprt_mode_chgd;

    /* Parameters for protocol generic command.
    */
    prot_gen_cmd_s_type                  gen_prot_cmd;

    /* Parameters for wakeup from pwr_save.
    */
    mmoc_cmd_wakeup_pwrsave_s_type       wakeup;

  } param;

} mmoc_cmd_msg_s_type;



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC REPORT INTERFACE RELATED TYPES ====================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration for the type of report queued in the report queueof MMoC
** task.
*/
typedef enum {

  MMOC_RPT_PROT_DEACTD_CNF,            /* Protocol deactivated confirmation
                                       ** report.
                                       */

  MMOC_RPT_PROT_AUTO_DEACTD_IND,       /* Autonomous protocol deactivated
                                       ** indication report.
                                       */

  MMOC_RPT_PH_STAT_CHGD_CNF,           /* Acknowledgement report for phone
                                       ** status changed command.
                                       */

  MMOC_RPT_PROT_GEN_CMD_CNF,           /* Acknowledgement report for Generic
                                       ** command received by the protocols.
                                       */

  MMOC_RPT_TMC_MODE_CNF,               /* Acknowledgement report from TMC 
                                       ** for operating mode
                                       */

  MMOC_RPT_MAX                         /* For internal use. */

} mmoc_rpt_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with protocol deactivated
** confirmation report to MMoC task.
*/
typedef struct {

  /* Transaction Id that associated with the corresponding deactivation
  ** command.
  */
  prot_trans_type                      trans_id;


} mmoc_rpt_deactd_cnf_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with protocol deactivated
** indication report to MMoC task.
*/
typedef struct {

  /* Reason for the protocol deactivation.
  */
  prot_deact_e_type                    reason;

  /* Protocol which sent the autonomous deactivation.
  */
  prot_state_e_type                    prot_state;

} mmoc_rpt_auto_deactd_ind_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with information on phone
** status changed acknowledgement protocol report.
*/
typedef struct {

  /* The sequence number that associated with the corresponding phone status
  ** changed command.
  */
  prot_trans_type                      trans_id;

  /* Protocol responding with the cnf.
  */
  sd_mode_e_type                       prot;

} mmoc_rpt_ph_stat_chgd_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with the acknowledgement
** for the generic command from the MMoC.
*/
typedef struct {

  /* Indicates whether the MMoC's generic command was processed or not.
  ** true - command was processed,
  ** false - command not processed.
  */
  boolean                              is_cmd_processed;

  /* Transaction Id that is associated with the corresponding generic
  ** command.
  */
  prot_trans_type                      trans_id;

  /* Indicates if the protocol is deactivating, because of processing
  ** the generic command. Valid only when is_cmd_processed = TRUE.
  */
  boolean                              is_prot_deactivating;

} mmoc_rpt_prot_gen_cmd_cnf_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure used by TMC to acknowledge operating
** mode changes.
*/

typedef struct {

  /* Transaction Id that is associated with the corresponding generic
  ** command.
  */
  prot_trans_type                      trans_id;

} mmoc_rpt_tmc_mode_cnf_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Report header type.  Each report to MMoC starts with this header.
*/

typedef struct {

  /* header info */
  cmd_hdr_type                         hdr;

  /* Report name */
  mmoc_rpt_e_type                      name;

} mmoc_rpt_hdr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that are associated with queuing a report to
** MMoC task.
*/
typedef struct {

  /* The header information that is associated with this report.
  */
  mmoc_rpt_hdr_s_type                  rpt;

  /* Union of parameters that are associated with the different reports.
  */
  union   {

    /* Parameters for protocol deactivated confirmation report.
    */
    mmoc_rpt_deactd_cnf_s_type           prot_deactd_cnf;

    /* Parameters for protocol deactivated indication report.
    */
    mmoc_rpt_auto_deactd_ind_s_type      prot_deactd_ind;

    /* Parameters for phone status changed acknowledgement report.
    */
    mmoc_rpt_ph_stat_chgd_cnf_s_type     ph_stat_cnf;

    /* Parameters for generic command acknowledgement report.
    */
    mmoc_rpt_prot_gen_cmd_cnf_s_type     gen_cmd_cnf;

    /* Parameter used by TMC to acknowledge operating
    ** mode change.
    */
    mmoc_rpt_tmc_mode_cnf_s_type         tmc_mode_cnf;


  } param;

} mmoc_rpt_msg_s_type;

/* Type for insanity err_fatal function to be implemented by the protocols.
*/
typedef void (mmoc_sanity_err_f_type) (

  /* Sanity error reason, either no response to deactivate req, generic cmd
  ** or phone status changed requests etc.
  */
  mmoc_sanity_err_e_type               reason

);

#ifdef FEATURE_MMOC_TMC_HEAP
/*---------------------------------------------------------------------------
                                 MMOC HEAP
---------------------------------------------------------------------------*/

/*
** This union is of the largest objects for which memory is dynamically
** allocated in the code. The SOLE purpose of the datatype is to obtain
** the size of the largest object for the MMOC_SIZE_OF_LARGEST_DYN_OBJ macro.
** It is not used anywhere else in the code.
*/
/*lint -esym(552, mmoc_large_obj_unused) disable not accessed error */
/*lint -esym(551, mmoc_large_obj_unused) disable not accessed error */
/*lint -esym(728, mmoc_large_obj_unused) disable not accessed error */
/*lint -esym(754, size_cal_dummy1) disable not accessed error */
/*lint -esym(754, size_cal_dummy2) disable not accessed error */
typedef union
{
  mmoc_rpt_msg_s_type     size_cal_dummy1;

  mmoc_cmd_msg_s_type     size_cal_dummy2;
} mmoc_large_obj_unused;

#define   MMOC_SIZE_OF_LARGEST_DYN_OBJ  ((unsigned long) \
                                          sizeof(mmoc_large_obj_unused))

/* The exact number of bytes that will be used to satisfy a malloc request
** for the largest object in CM.  As of not, the largest object is
** cmcall_s_type.
*/
#define   MMOC_MAX_ACTUAL_ALLOC_SIZE   ((( MMOC_SIZE_OF_LARGEST_DYN_OBJ \
                                         + sizeof(mem_block_header_type) \
                                         + kMinChunkSize - 1 ) / kMinChunkSize ) \
                                      * kMinChunkSize )

/* No. of fixed size requests that must be satisfied by the MMOC heap.
*/
#define   MMOC_HEAP_SLOT_COUNT         6

/* The additional 15 bytes is to take into account the paragraph boundary
** alignment done by mem_init_heap().
*/
#define   MMOC_HEAP_BUFFER_SIZE        ((MMOC_MAX_ACTUAL_ALLOC_SIZE * \
                                      MMOC_HEAP_SLOT_COUNT) + 15)
#endif /* FEATURE_MMOC_TMC_HEAP */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC REX TASK RELATED TYPES ============================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Signal Masks.
*/
/* This signal is set when a command is put on the mmoc command q.
*/
#define  MMOC_CMD_Q_SIG                  0x0001

/* This signal is set when NV completes a command,
*/
#define  MMOC_NV_CMD_SIG                 0x0002

/* This signal is set when the mmoc_rpt_timer expires.
*/
#define  MMOC_RPT_TIMER_SIG              0x0004

/* This signal is set when a report is put on the mmoc report q.
*/
#define  MMOC_RPT_Q_SIG                  0x0010

/* This signal is set when the MMoC's sanity timer expires.
*/
#define  MMOC_SANITY_TIMER_SIG           0x0020

/* This signal is set for srch reports.
*/
#define  MMOC_SRCH_RPT_SIG               0x0040

/* This signal is set for wait_timer .
*/
#define  MMOC_REX_WAIT_SIG               0x0080

/* This signal is set for removing dup commands.
*/
#define  MMOC_DUP_CMD_REM_SIG            0x0100


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef FEATURE_MMOC_TMC_HEAP
/* The number of command buffers or command q size.
*/
#ifdef FEATURE_MMOC_LOW_MEM_TARGET
#define  MMOC_CMD_BUFS_NUM               6
#else
#define  MMOC_CMD_BUFS_NUM               15
#endif

/* The number of report buffers or report q size.
*/
#define  MMOC_RPT_BUFS_NUM               6
#endif /* FEATURE_MMOC_TMC_HEAP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Address of the TCB for the MMOC task.
*/
#define  MMOC_TASK_PTR                    &mmoc_tcb

/* The MMoC's sanity timer value. Used to check is MMoC is not stuck with
** a particular transaction.
** This timer value should be used for all transactions except powerdown.
** So for all transactions MMOC will error fatal after 60 seconds based
** on INSANITY_COUNT of 5.
*/
#define  MMOC_SANITY_TIME                 (12000)

/* MMoC's sanity timer value for power down transaction.
** For power down transaction, MMoC will error fatal after 60 seconds based
** on INSANITY_COUNT of 5.
*/
#define  MMOC_PWRDOWN_SANITY_TIME         (12000)

/* The max count for which the MMoC's transaction was not changed, after
** this, MMoC will do ERR_FATAL.
*/
#define  MMOC_MAX_INSANITY_COUNT          (5)

/* WORKAROUND: Time to wait for response from MC for generic protocol command
** before sending HDR RF unlock command.
*/
#define  MMOC_HDR_RF_UNLOCK_TIME          (48000)

/* MMOC SRCH response timeout( 5 secs ).
*/
#define  MMOC_SRCH_RESP_TIMEOUT           (60000)

/* MMOC error fatal message maximum length
*/
#define MMOC_ERR_FATAL_MSG_MAX_LENGTH (100)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  MMOC_MAX_STRING_LENGTH 300

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type that associates the MMoC generic info other than state info.
*/
typedef struct
{
  /* Timer for reporting to watchdog.
  */
  rex_timer_type                       rpt_timer;

  /* Queue for reports to MMoC task.
  */
  q_type                               rpt_q;

  #ifndef FEATURE_MMOC_TMC_HEAP
  /* Free queue for reports to MMoC task.
  */
  q_type                               rpt_free_q;
  #endif /* FEATURE_MMOC_TMC_HEAP */

  /* Queue for commands to MMoC task.
  */
  q_type                               cmd_q;

  #ifndef FEATURE_MMOC_TMC_HEAP
  /* Free queue for commands to MMoC task.
  */
  q_type                               cmd_free_q;

  /* Statically allocated space for free report queue buffers.
  */
  mmoc_rpt_msg_s_type                  rpt_pool[MMOC_RPT_BUFS_NUM];

  /* Statically allocated space for free command queue buffers.
  */
  mmoc_cmd_msg_s_type                  cmd_pool[MMOC_CMD_BUFS_NUM];
  #endif /* FEATURE_MMOC_TMC_HEAP */


  /* System selection timer for SS_MAIN.
  */
  #ifdef FEATURE_USE_TIME_VU
  timer_type                           ss_main_timer;
  #else
  clk_cb_type                          ss_main_timer;
  #endif /* FEATURE_USE_TIME_VU */


  /* System selection timer for SS_HDR.
  */
  #ifdef FEATURE_USE_TIME_VU
  timer_type                           ss_hdr_timer;
  #else
  clk_cb_type                          ss_hdr_timer;
  #endif /* FEATURE_USE_TIME_VU */

  /* System selection timer for SS_HYBR_WLAN.
  */
  #ifdef FEATURE_USE_TIME_VU
  timer_type                           ss_hybr_wlan_timer;
  #else
  clk_cb_type                          ss_hybr_wlan_timer;
  #endif /* FEATURE_USE_TIME_VU */

  #ifdef FEATURE_MMOC_UNIT_TEST
  /* Stop execution upon ERR_FATAL
  */
  boolean                              is_stop_err_fatal;

  /* ERR_FATAL message buffer.
  */
  char                                 err_fatal_msg[MMOC_ERR_FATAL_MSG_MAX_LENGTH];
  #endif

  /* Timer to do rex timed wait
  */
  rex_timer_type                       rex_wait_timer;

} mmoc_task_info_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC IMPLEMENTATION RELATED TYPES ======================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of transaction types in the MMoC.
*/
typedef enum {

  MMOC_TRANS_NULL,                     /* Null transaction - idle state for
                                       ** MMoC.
                                       */

  MMOC_TRANS_SUBSC_CHGD,               /* Transaction to process the
                                       ** subscription changed command.
                                       */

  MMOC_TRANS_ONLINE,                   /* Transaction to process the ONLINE
                                       ** operating mode command.
                                       */

  MMOC_TRANS_OFFLINE,                  /* Transaction to process the OFFLINE,
                                       ** OFFLINE_CDMA and OFFLINE_AMPS
                                       ** operating mode command.
                                       */


  MMOC_TRANS_PROT_GEN_CMD,             /* Transaction to process the generic
                                       ** command bound for protocol.
                                       */

  MMOC_TRANS_PWR_DOWN,                 /* Transaction to process the power down/
                                       ** RESET/LPM operating mode command.
                                       */


  MMOC_TRANS_PWR_SAVE_ENTER,           /* Transaction to process the power save
                                       ** enter request from protocols/SD20.
                                       */

  MMOC_TRANS_PWR_SAVE_EXIT,            /* Transaction to process the power save
                                       ** exit command.
                                       */

  MMOC_TRANS_PROT_AUTO_DEACTD_IND,     /* Transaction to process the autonomous
                                       ** deactivated indication report from
                                       ** protocol.
                                       */

  MMOC_TRANS_WAKEUP_FROM_PWR_SAVE,     /* Transaction to process the comand to
                                       ** wakeup the phone from power save.
                                       */

  MMOC_TRANS_MAX                       /* For internal use. */

} mmoc_trans_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of transaction state types in the MMoC. Note that not all
** transaction states are required to process a particular transaction.
*/
typedef enum {

  MMOC_TRANS_STATE_NULL,               /* Initial transaction state. Required
                                       ** for processing any transaction.
                                       */

  MMOC_TRANS_STATE_PROT_DEACT_ENTER,   /* Transaction state which sends the
                                       ** protocol deactivate request.
                                       */

  MMOC_TRANS_STATE_WAIT_DEACTD_CNF,    /* Transaction state which waits for
                                       ** deactivated protocol report.
                                       */

  MMOC_TRANS_STATE_PROT_PH_STAT_ENTER, /* Transaction state which sends the
                                       ** phone status changed command to the
                                       ** first supported protocol.
                                       */

  MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,   /* Transaction state which waits for
                                       ** the acknowledgement for phone
                                       ** status changed request.
                                       */

  MMOC_TRANS_STATE_GEN_CMD_ENTER,      /* Transaction state which either
                                       ** sends the generic command to the
                                       ** active protocol or processes it in
                                       ** MMoC if there are no active
                                       ** protocols.
                                       */


  MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF,   /* Transaction state which waits for
                                       ** the acknowledgement for generic
                                       ** command.
                                       */

  MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND,/* Transaction state which waits for
                                        ** autonomous deactivated indication
                                        */

  MMOC_TRANS_STATE_HDR_DEACT_ENTER,     /* Transaction state that actually 
                                        ** sends deact request to HDR.
                                        */

  MMOC_TRANS_STATE_WAIT_HDR_DEACTD_CNF, /* Transaction state that waits for
                                        ** just HDR protocol on hybrid instance
                                        ** to deactivate
                                        */


  MMOC_TRANS_STATE_WAIT_TMC_MODE_CNF,  /* Transaction state which waits for
                                       ** Operating mode CNF from TMC.
                                       */



  MMOC_TRANS_STATE_MAX                 /* For internal use. */

} mmoc_trans_state_e_type;






/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration that describes whether the event (command or report) was
** processed to completion or not.
*/
typedef enum {

  MMOC_EVT_STATUS_CONSUMED,            /* The event was processed to
                                       ** completion.
                                       */

  MMOC_EVT_STATUS_NOT_CONSUMED,        /* The event was not processed, the
                                       ** MMoC should call the transaction
                                       ** handlers again.
                                       */

  MMOC_EVT_STATUS_MAX                  /* For internal use. */

} mmoc_evt_status_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for transaction exit cleanup function.
*/
typedef mmoc_evt_status_e_type (mmoc_exit_fn_f_type) (

  /* MMOC's state info.
  */
  void                         *info_ptr

);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the information regarding the transaction
** MMOC_TRANS_PROT_GEN_CMD being processed.
*/
typedef struct {

  /* Boolean flag to indicate if SS_MAIN needs to be
  ** activated at the end of transaction.
  */
  boolean                             is_activate_main;

  /* Info for protocol generic command transaction.
  */
  prot_gen_cmd_s_type                  cmd_info;


} mmoc_gen_cmd_trans_info_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the information regarding the transaction
** being processed. Initially it holds the information on the command or report
** which invoked this transaction. If additional information needs to be stored
** as part of trans_info, then new types have to be created.
*/
typedef union {

  /* Info for subscription changed transaction.
  */
  mmoc_cmd_subsc_chgd_s_type           subsc_chgd;

  /* Info for protocol generic command transaction.
  */
  mmoc_gen_cmd_trans_info_s_type       gen_prot_cmd;

  /* Info for Generic deactivated report transaction.
  */
  prot_deact_s_type                    gen_deact_rpt;

} mmoc_trans_info_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type that associates the mode with the task that implements the mode.
*/
typedef struct {

   /* Main task that implements the above mode.
   */
   char*             task_name;

   /* Pointer to the protocol's sanity error function.
   */
   mmoc_sanity_err_f_type    *sanity_err_fn;

   /* Trans info.
   */
   prot_trans_type   trans_id;

} mmoc_mode_info_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the state information of the MMoC task.
*/
typedef struct {

  /* Current transaction that the MMoC is processing. Set to MMOC_TRANS_NULL
  ** if no transactions are being processed.
  */
  mmoc_trans_e_type                    curr_trans;

  /* Current state of the transaction that the MMoC is processing.
  */
  mmoc_trans_state_e_type              trans_state;

  /* Information related to the transaction being processed. Valid only
  ** when curr_trans != MMOC_TRANS_NULL.
  */
  mmoc_trans_info_s_type               trans_info;

  /* Current protocol state corresponding to SS-Main and SS-HDR instances.
  */
  prot_state_e_type                    prot_state[SD_SS_MAX];

  /* Flag to indicate if SD was initialized with subscription
  ** information.
  */
  boolean                              is_sd_initialized;

  /* Last trans Id for the command issued by MMoC. Incremented after
  ** sending every acknowledgeable command.
  ** Cannot be set to MMOC_TRANS_AUTONOMOUS or MMOC_TRANS_PENDING.
  */
  prot_trans_type                      trans_id;

  /* Current NAM selection.
  */
  byte                                 curr_nam;

  /* Current GSM/WCDMA subscription availability status.
  */
  boolean                              is_gw_subs_avail;

  /* Current CDMA/AMPS/HDR subscription availability status.
  */
  boolean                              is_cdma_subs_avail;

  /* Current CDMA/AMPS/HDR subscription availability status.
  */
  boolean                              is_wlan_subs_avail;

  /* Current operating mode status of the phone.
  */
  prot_oprt_mode_e_type                curr_oprt_mode;

  /* Flag to indicate if RESET_MODEM cmd is being processed.
  */
  sys_oprt_mode_e_type                 true_oprt_mode;

  /* Indicates if phone status chgd command to all supported protocols
  ** have already been sent.
  */
  boolean                              is_ph_stat_sent;


  /* Context for current protocol to which the deactivate request was
  ** sent.
  */
  sd_ss_e_type                         deact_req_index;


  /* Insanity count to sanity check if the MMoC is stuck in a transaction.
  */
  byte                                 insanity_count;

  /* Timer to check sanity of MMoC.
  */
  rex_timer_type                       sanity_timer;

  /* Flag to indicate if rf was initialized. Note that on powerup, this
  ** flag will be set to TRUE, since TMC will be initializing the RF.
  ** Reset only when transitioning from FTM -> ONLINE.
  */
  boolean                              is_rf_initialized;


  /* Pointer to the transaction exit cleanup function.
  */
  mmoc_exit_fn_f_type                  *exit_fn_ptr;

  /* Flag to indicate if phone is in standby sleep.
  */
  boolean                              is_standby_sleep;

  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
  prot_subsc_chg_e_type                prot_subsc_chg;

} mmoc_state_info_s_type;

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_state_info_ptr

DESCRIPTION
  Return a pointer to the one and only mmoc_state_info object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern mmoc_state_info_s_type *mmoc_get_state_info_ptr( void );


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_main_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_ss_main_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

);

#define mmoc_ss_hdr_timer_done_cb(x) mmoc_ss_hybr_1_timer_done_cb(x)

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_hdr_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_ss_hybr_1_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_hybr_wlan_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection timer expires for SS-WLAN instance.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_ss_hybr_wlan_timer_done_cb
(

       int4                            ms_interval
          /*lint -esym(715,ms_interval)
          ** Have lint not complain about the ignored parameter 'ms_interval'
          ** which is specified to make this routine match the template for
          ** clk_Reg().
          */

);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_task_init

DESCRIPTION
  This function is responsible for initializing the MMoC task before task
  start. This includes
  1. Initializing command and report queues
  2. Performing system determination initializing before task start.

DEPENDENCIES
  This should be called only before task start.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_task_init
(

        mmoc_state_info_s_type         *mmoc_state_ptr
           /* Pointer to MMoC's state information.
           */

);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_event

DESCRIPTION
  This function will process the received event by calling the transaction
  handlers.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
extern void                            mmoc_process_event
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_sanity_timer_exp

DESCRIPTION
  This function will check if a particular transaction is stuck in MMoC
  thereby validating the MMoC sanity.
  The function will do ERR_FATAL if it determines that the MMoC is insane.

  Note! This function should be called at the expiration of the sanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                            mmoc_sanity_timer_exp
(

       mmoc_state_info_s_type          *mmoc_state_ptr
          /* Pointer to MMoC's state information.
          */

);



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_gen_cmd_cnf

DESCRIPTION
  This function is used to acknowledge the generic protocol command.

  Note!  This is a local function and should be called by
  mmoc_proc_prot_gen_cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern  void                           mmoc_rpt_prot_gen_cmd_cnf
(

        boolean                        is_cmd_processed,
           /* If the protocols received the generic command in the incorrect
           ** state, then this will be set to FALSE.
           */

        prot_trans_type                trans_id,
           /* Protocols set this same as the trans_id received in the generic
           ** command. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
           */

        boolean                        is_prot_deactivating
          /* Is the protocol deactiavting because of processing the generic
          ** command. Valid when when is_cmd_processed = TRUE.
          */


);

/* <EJECT> */
#if (TG==T_PC)
#error code not present
#endif /* if (TG == T_PC) */


#endif /* MMOCI_H */


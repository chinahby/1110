#ifndef CMREGPRXI_H
#define CMREGPRXI_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                            R E G P R O X Y

                 I N T E R N A L    H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal types necessary for implementing
  the RegProxy.

Copyright (c) 2002 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmregprxi.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Removed EXTERN define & defined MMEXTN in cmi.h
03/01/09   sv      Lint cleanup.
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
02/05/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
11/30/06   rp      Adding suport for background service search
04/10/06   pk      Lint fixes
01/06/06   ic      Lint cleanup  
11/14/05   ic      Removed #undef CMREGPRX_DEBUG so CUTE can define it in
                   order to enable assertions in cmregprx.c 
                   Lint80 clean up 
10/21/04   sj      Changed throttling mechanism for CM_SERVICE_CNF.
                   Removed handling of unused sanity timer mechanism.
07/30/04   sj      Added support for camped_ind.
02/06/04   ws      Initial jaguar/mainline merge.
11/03/03   ws      Fix LTK compile errors.
08/20/03   SJ      Added srv cnf hold period, if srv cnf comes back within
                   1 second.
03/18/03   sj      Added flag to indicate protocol is in warmup state.
01/21/03   ws      Updated copyright information for 2003
04/22/02   SJ      Initial release.

===========================================================================*/



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

#include "task.h"     /* Interface for global task resources */
#include "dog.h"      /* Interface to watchdog procedures */
#include "cmwll.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================
=============================================================================
=============================================================================
==================== RegProxy DEBUG RELATED STUFF ===========================
=============================================================================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
      Compile RegProxy with debug information, asserts and messages.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                  RegProxy_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/
#define CMREGPRX_MSG_LOW( str, a, b, c ) \
        MSG_LOW( "=CMREGPRX= " str, a, b, c)

#define CMREGPRX_MSG_MED( str, a, b, c ) \
        MSG_MED( "=CMREGPRX= " str, a, b, c)

#define CMREGPRX_MSG_HIGH( str, a, b, c ) \
        MSG_HIGH( "=CMREGPRX= " str, a, b, c)

#define CMREGPRX_MSG_ERROR( str, a, b, c ) \
        MSG_ERROR( "=CMREGPRX= " str, a, b, c)



/*---------------------------------------------------------------------------
                   CMREGPRX_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/

#ifdef CMREGPRX_DEBUG
#error code not present
#elif( TG==T_PC )
#error code not present
#else

  #define CMREGPRX_ERR_FATAL(str,a,b,c ) /*lint -e571 */ ERR_FATAL( "",a,b,c ) /*lint +e571 */ 

  #define CMREGPRX_ERR( str,a,b,c )           ERR( "",a,b,c )
  #define CMREGPRX_CASE_ERR( str,a,b,c )      ERR( "",a,b,c )
  #define CMREGPRX_SWITCH_ERR( str,a,b,c )    ERR( "",a,b,c )
  #define CMREGPRX_ELSE_ERR( str,a,b,c )      ERR( "",a,b,c )

  #define CMREGPRX_FUNC_START( str,a,b,c )
  #define CMREGPRX_FUNC_END( str,a,b,c )

  #define CMREGPRX_FUNC_NOT_IMP( func )       ERR( "",0,0,0 )
  #define CMREGPRX_CASE_NOT_IMP( _case )      ERR( "",0,0,0 )

#endif

/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#ifdef CMREGPRX_DEBUG
#error code not present
#else
  #define CMREGPRX_ASSERT( exp )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef CMREGPRX_DEBUG
#error code not present
#else
  #define CMREGPRX_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef CMREGPRX_DEBUG
#error code not present
#else
  #define CMREGPRX_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef CMREGPRX_DEBUG
#error code not present
#else
  #define CMREGPRX_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif



/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC COMMAND INTERFACE RELATED TYPES ===================
=============================================================================
=============================================================================
===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* A type for parameters that are associated with queuing a command to
** RegProxy module.
*/
typedef struct {

  /* The header information that is associated with this command.
  */
  cmd_hdr_type                         hdr;

  /* Regproxy command name.
  */
  prot_cmd_e_type                      name;


  /* Union of parameters that are associated with the different commands.
  */
  union   {

   /* Parameters for activate protocol command.
   */
   prot_act_s_type                       act_prot;

   /* Parameters for deactivate protocol command.
   */
   prot_deact_s_type                     deact_prot;

   /* Parameters for generic protocol command.
   */
   prot_gen_cmd_s_type                   gen_cmd;

   /* Parameters for phone status changed command.
   */
   prot_ph_stat_chgd_s_type              ph_stat_cmd;


  } param;

} cmregprx_cmd_msg_s_type;



/*===========================================================================
=============================================================================
=============================================================================
==================== REGPROXY IMPLEMENTATION RELATED TYPES ==================
=============================================================================
=============================================================================
===========================================================================*/

/* Number of command buffers for MMoC -> REG communication.
*/
#define  CMREGPRX_CMD_BUFS_NUM          (3)



/* Reference period in secs for counting the number of CM_SERVICE_CNF.
*/
#define  CMREGPRX_SRV_REQ_CNT_PERIOD    (dword)(2)

/* Maximum count of CM_SERVICE_CNF during SRV_CNF_CNT_PERIOD before
** Regproxy will start throttling CM_SERVICE_CNF to SD.
*/
#define  CMREGPRX_MAX_SRV_CNF_REQ       (int)(6)

/* Starting uptime in Regproxy.
*/
#define  CMREGPRX_UPTIME_NULL           (dword)(0)

/* Typedef for REG transaction id.
*/
typedef  uint32                          cmregprx_trans_type;

/* default value for measurement id
*/
#define CMREGPRX_MEAS_ID_DEFAULT      (0);

/* default value for band group position
*/
#define CMREGPRX_POS_DEFAULT      (0);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of RegProxy states.
*/
typedef enum {

  CMREGPRX_STATE_INACTIVE,             /* REG is in Inactive state.
                                       */

  CMREGPRX_STATE_ACTIVE,               /* REG is in active state.
                                       */

  CMREGPRX_STATE_DORMANT,              /* REG is in dormant state.
                                       */

  CMREGPRX_STATE_MAX                   /* For internal use. */

} cmregprx_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of RegProxy sub-states.
*/
typedef enum {

  CMREGPRX_SUBSTATE_NULL,              /* Regproxy is in idle operating
                                       ** state.
                                       */

  CMREGPRX_SUBSTATE_WAIT_SRV_CNF,      /* RegProxy is in a state waiting for
                                       ** service request confirmation from
                                       ** REG.
                                       */

  CMREGPRX_SUBSTATE_WAIT_STOP_CNF,     /* RegProxy is in a state waiting for
                                       ** Stop request confirmation from REG.
                                       */

  CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF, /* RegProxy is in a state waiting for
                                       ** get network list request confirmation
                                       ** from REG.
                                       */

  CMREGPRX_SUBSTATE_WAIT_SIM_AVL_CNF,  /* RegProxy is in a state waiting for
                                       ** confirmation for the SIM availability
                                       ** changed request.
                                       */

  CMREGPRX_SUBSTATE_WAIT_PH_STATUS_CHGD_CNF,
                                       /* RegProxy is in a state waiting for
                                       ** confirmation for the Phone status
                                       ** changed request.
                                       */

  CMREGPRX_SUBSTATE_MAX                /* For internal use. */

} cmregprx_substate_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of srv_req difference.
*/
typedef enum {

  CMREGPRX_SRV_REQ_DIFF_NONE,          /* No difference in srv_req comparison.
                                       */

  CMREGPRX_SRV_REQ_DIFF_FOR_REQ_ONLY,  /* Difference is only in for_request
                                       */

  CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS,   /* Difference in other parms too.
                                       */


  CMREGPRX_SRV_REQ_DIFF_MAX            /* For internal use. */

} cmregprx_srv_req_diff_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure to hold information pertaining to background service search
*/
typedef struct {

  /*Measurement ID for the background search */
  word meas_id;

  /* Current position for the bandgroup. Used to query SD to get the next
  ** band group to search 
  */
  uint8 pos;

 
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

  /* user requested or background search */
  reg_cm_network_list_req_e_type list_search_type;

  #endif


  
}cmregprx_list_search_info_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* A type for parameters that holds the state and other information of the
** RegProxy.
*/
typedef struct {

  /* Current state of RegProxy.
  */
  cmregprx_state_e_type                state;

  /* Current sub-state of RegProxy.
  */
  cmregprx_substate_e_type             substate;

  /* Transaction id of the last request to REG.
  */
  cmregprx_trans_type                  reg_trans_id;

  /* Trans_id for the MMoC's request, can handle only one request
  ** at a time.
  */
  prot_trans_type                      mmoc_trans_id;

  /* Current GSM/WCDMA subscription availability status.
  */
  boolean                              is_gw_subs_avail;

  /* Flag to indicate if SD20 reselection is allowed or not.
  */
  boolean                              is_resel_allowed;

  /* Queue for commands to MMoC task.
  */
  q_type                               cmd_q;

  /* Free queue for commands to MMoC task.
  */
  q_type                               cmd_free_q;

  /* Statically allocated space for free command queue buffers.
  */
  cmregprx_cmd_msg_s_type              cmd_pool[CMREGPRX_CMD_BUFS_NUM];

  /* REX signal to be set when a RegProxy command is queued.
  */
  rex_sigs_type                        cmd_q_sig;


  /* REX task context under which the RegProxy is running.
  */
  rex_tcb_type                         *rex_tcb_ptr;


  /* Start time for counting number of CM_SERVICE_REQ -> to determine
  ** CM_SERVICE_REQ should be throttled.
  */
  dword                                start_thrtl_ref_uptime;


  /* Count of CM_SERVICE_REQ during the reference period.
  */
  int                                  cnt_cm_srv_req;

  /* Camped_ind buffer
  */
  cm_rpt_type                          camped_ind_buf;

  /* Last srv_req action.
  */
  cm_service_req_s_type                last_srv_req;

  #if (TG==T_PC)
#error code not present
  #endif

  /* To hold background search information */
  cmregprx_list_search_info_s_type    list_search_info;

  /** The reason STOP_MODE_REQ was sent to REG. */
  sys_stop_mode_reason_e_type         stop_mode_reason;

} cmregprx_info_s_type;



/*===========================================================================
=============================================================================
=============================================================================
==================== REGPROXY INTERNAL FUNCTIONS ============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
MMEXTN cmregprx_info_s_type*           cmregprx_get_info_ptr
(
       void

);


#if (TG==T_PC)
#error code not present
#endif /* if (TG == T_PC) */

#endif /* CMREGPRXI_H */


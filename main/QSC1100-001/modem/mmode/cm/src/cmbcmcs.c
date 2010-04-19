/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   B C M C S   M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager BCMCS module.

  The BCMCS object is responsible for:
  1. Processing clients' BCMCS commands.
  2. Processing LL replies for clients' BCMCS commands.
  3. Processing LL BCMCS related notifications.
  4. Notifying the client list of BCMCS events.


EXTERNALIZED FUNCTIONS

  cmbcmcs_timer_proc
  cmbcmcs_client_cmd_proc
  cmbcmcs_init
  cmbcmcs_ptr
  cmbcmcs_mc_rpt_proc


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmbcmcs.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/09   sv      Correct Lint errors
12/03/08   ak      Corrected Feature name to FEATURE_HDR_BCMCS_DYNAMIC_MODEL.
12/03/08   ak      Featurized REG_HANDOFF and BOM_CACHE for QCHAT
08/25/08   ak      Fixed the general compiler warnings.
07/28/08   rm      Fixing compiler warnings
07/01/08   rm      Mainlining of MBMS changes
05/06/08   ak      BCMCS 2.0 changes
08/07/07   ic      Lint clean up
04/18/06   sk      Fixed compilation error.
04/06/06   sk      If HDR is in power save, wake it up before sending a BCMCS
                   flow request to it.
                   Removed cmbcmcs_timer_proc().
                   Let BCMCS commands go to HDR when BCMCS supported is true
                   irrespective of the HDR service status.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/13/05   ic      Send HDRMC_BCMCS_DISCONTINUE_BROADCAST_F regardless of 
                   whether HDR is BCMCS capable. 
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
09/09/05   ic      Removed debug message printing from cmbcmcs_orig_proc()
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
05/13/05   ic      Added cmbcmcs_srv_status_is_srv() [ moved from sys.* ] 
11/11/04   ic      Lint cleanup 
08/02/04   ic      CM Lint clean up - cmbcmcs.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations. 
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmbcmcs_dummy() to get rid of ADS 1.2 compiler
                   warning
07/12/04   ic      Fixed Lint issues
06/23/04   ic      Fixed ADS 120 compiler warnings (Jaguar build)
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
05/27/04   ic      Fixed featurization errors so it compiles without
                   FEATURE_HDR defined
03/03/04   ic      Fixed featurization error
03/02/04   ic      Initial release

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/
/* INSERT STATE MACHINE HERE */

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h)*/
#include "customer.h"  /* Customer configuration file */

#if (((defined FEATURE_BCMCS) && (defined FEATURE_HDR)) || \
      (defined FEATURE_MODEM_MBMS))
#include <stdlib.h>
#include <string.h>

#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"

#if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)
#include "hdrmc.h"
#endif

#include "cmdbg.h"
#include "cmi.h"
#include "cmbcmcs.h"
#include "cmclient.h"
#include "cmpref.h"
#ifdef FEATURE_MODEM_MBMS
#error code not present
#endif
#include "cmwll.h"

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cmxll.h"
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

static void cmbcmcs_client_cmd_err(

  const cm_cmd_s_type     *cmd_ptr,
    /* Pointer to a CM command
    */

  cm_bcmcs_cmd_err_e_type    cmd_err
    /* Indicate the BCMCS command error
    */
);

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal, common
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmbcmcs_get_bcmcs_srv_indicators

DESCRIPTION

  Get BCMCS service indicators based on whether we're in hybrid.
  Return values will be written into variables pointed to by the
  function parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_get_bcmcs_srv_indicators(
  boolean                     *bcmcs_srv_supported,
  sys_bcmcs_srv_status_e_type *bcmcs_srv_status
)
{
  cmss_s_type                *ss_ptr              =  cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify integrity of BCMCS & SS objects
  */
  CM_ASSERT( ss_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( ss_ptr->init_mask) );
  CM_ASSERT( bcmcs_srv_supported != NULL );
  CM_ASSERT( bcmcs_srv_status != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(cmpref_is_hybr_status_on())
  {
    *bcmcs_srv_supported = ss_ptr->info.hybr_hdr_bcmcs_srv_supported;
    *bcmcs_srv_status    = ss_ptr->info.hybr_hdr_bcmcs_srv_status;
  }
  else
  {
    *bcmcs_srv_supported = ss_ptr->info.ss_main_bcmcs_srv_supported;
    *bcmcs_srv_status    = ss_ptr->info.ss_main_bcmcs_srv_status;
  }
}

/*===========================================================================

FUNCTION cmbcmcs_reset_buffers

DESCRIPTION

  Reset BCMCS internal buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_reset_buffers( void )
{
 #if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)
  cmbcmcs_s_type      *bcmcs_ptr      =  cmbcmcs_ptr();
    /* Point at BCMCS object
    */

  cm_bcmcs_cmd_s_type *bcmcs_cmd_ptr  =  &bcmcs_ptr->bcmcs_cmd.cmd.bcmcs;
    /* Point at BCMCS object's copy of last BCMCS command
    */

  cm_mc_rpt_type      *bcmcs_rpt_ptr  =  &bcmcs_ptr->bcmcs_rpt;

  CM_ASSERT( bcmcs_ptr != NULL );
  CM_FUNC_START( "cmbcmcs_reset_buffers()",0,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( bcmcs_rpt_ptr, 0, sizeof( cm_mc_rpt_type ));

  memset( bcmcs_cmd_ptr, 0, sizeof( cm_bcmcs_cmd_s_type ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* 
  ** Reset BCMCS object's command buffer
  */
  bcmcs_cmd_ptr->cmd                 = CM_BCMCS_CMD_NONE;
  bcmcs_cmd_ptr->cmd_err             = CM_BCMCS_CMD_ERR_NOERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** As the report structures for both BCMCS reports are similar
  ** and they share the memory in a union, we can reset the fields in 
  ** either report.
  */
  bcmcs_rpt_ptr->bcmcs_rpt.hdr.cmd   = CM_RPT_NONE;
  bcmcs_rpt_ptr->bcmcs_rpt.protocol  = SYS_SYS_MODE_NONE;
  bcmcs_rpt_ptr->bcmcs_rpt.transaction_id 
                                     = CM_BCMCS_TRANSACTION_ID_NONE;
  #endif
}

/*===========================================================================

FUNCTION cmbcmcs_set_state

DESCRIPTION

  Set BCMCS object's state and perform any clean up
  related to state change


DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_set_state(cm_bcmcs_state_e_type new_state)
{

  cmbcmcs_s_type                *bcmcs_ptr     = cmbcmcs_ptr();
  /* Point at BCMCS object
  */

  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );
  CM_ASSERT( BETWEEN(new_state,CM_BCMCS_STATE_NONE, CM_BCMCS_STATE_MAX ));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(new_state == CM_BCMCS_STATE_IDLE)
  {

    /* Reset BCMCS internal buffers
    */
    cmbcmcs_reset_buffers();

    /* Forget the fact that we might be waiting for SD.
    */
    bcmcs_ptr->waiting_pwrsave_exit_for_ss = CM_SS_NONE;

    /* Inform SD that BCMCS flow request is done.
    */
    (void) cmph_update_orig_param( (cm_act_id_type) bcmcs_ptr, 
                                   CM_SS_MAIN, 
                                   CM_ACT_UPDATE_REAS_ACT_END );

    (void) cmph_update_orig_param( (cm_act_id_type) bcmcs_ptr, 
                                   CM_SS_HDR,  
                                   CM_ACT_UPDATE_REAS_ACT_END );

    (void) cmph_update_orig_param( (cm_act_id_type) bcmcs_ptr, 
                                   CM_SS_WLAN, 
                                   CM_ACT_UPDATE_REAS_ACT_END );
    
  }

  /* Now change the state
  */
  bcmcs_ptr->state = new_state;

}

#if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)
/*===========================================================================

FUNCTION cmbcmcs_mc_rpt_store

DESCRIPTION

  Store last LL report received in the BCMCS object


DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_mc_rpt_store(const cm_mc_rpt_type *cm_mc_rpt_ptr)
{
  cmbcmcs_s_type *bcmcs_ptr = cmbcmcs_ptr();

  /*------------------------------------------------------------------------*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( bcmcs_ptr     != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );

  /*------------------------------------------------------------------------*/

  /* Store the report inside of BCMCS object.
  */
  memcpy( &bcmcs_ptr->bcmcs_rpt,
          cm_mc_rpt_ptr,
          sizeof(cm_mc_rpt_type));
}

/*===========================================================================

FUNCTION cmbcmcs_ll_rpt_check

DESCRIPTION

  Check LL report for errors relative to the state of BCMCS object


DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint --e{528} suppress cmbcmcs_mc_rpt_check not referenced
** warning 
*/
static boolean cmbcmcs_mc_rpt_check(const cm_mc_rpt_type *cm_mc_rpt_ptr)
/*lint -esym(715, cm_mc_rpt_ptr) not referenced */
{
  cmbcmcs_s_type *bcmcs_ptr = cmbcmcs_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(cm_mc_rpt_ptr->hdr.cmd)
  {

    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F:
    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F:
    case CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F:

      /* If transaction id in the LL report does not match
      ** that of what BCMCS currently has for the last command that
      ** was sent to the LL, discard the report.
      ** This is expected as the user may have queued a new flow
      ** request command already and LL meantime sent notification
      ** pertaining to the old one.
      */
      if(bcmcs_ptr->transaction_id != 
                             cm_mc_rpt_ptr->bcmcs_rpt.transaction_id)
      {
        CM_ERR("Transaction ids do not match, discard report ...",0,0,0);
        CM_ERR("BCMCS id = %d, LL RPT id = %d",
                bcmcs_ptr->transaction_id,
                cm_mc_rpt_ptr->bcmcs_rpt.transaction_id,
                0);
        return FALSE;
      }
      return TRUE;

    case CM_HDR_BCMCS_FLOW_STATUS_F:
    case CM_HDR_BCMCS_BROADCAST_RESET_F:
    case CM_HDR_BCMCS_FLOW_MONITORING_START_F:
    case CM_HDR_BCMCS_FLOW_MONITORING_END_F:
    case CM_HDR_BCMCS_REG_STATUS_F:
      return TRUE;

    default:
      CM_MSG_LOW("Not BCMCS report %d, ignoring ...",cm_mc_rpt_ptr->hdr.cmd,0,0);
      return FALSE;
  } // switch
}
#endif
/*lint +esym(715, cm_mc_rpt_ptr) not referenced */
/*lint -restore */
/*===========================================================================

FUNCTION cmbcmcs_info_alloc

DESCRIPTION
 Allocate a BCMCS info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated BCMCS info buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cm_bcmcs_info_s_type  *cmbcmcs_info_alloc( void )
{
  static cm_bcmcs_info_s_type   bcmcs_info;
  /* Currently there is only a need for one BCMCS info buffer
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simply return a pointer to the statically allocated
  ** BCMCS info buffer.
  */
  return &bcmcs_info;

}

#if defined (FEATURE_MODEM_MBMS)
#error code not present
#endif /* FEATURE_MODEM_MBMS */

/*lint +esym(818, bcmcs_info_ptr) */
/*lint +esym(715, bcmcs_info_ptr) not referenced */
/*lint -restore */

#if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)
/*===========================================================================

FUNCTION cmbcmcs_info_get

DESCRIPTION

  Copy LL report info into a specified BCMCS event info buffer.

DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE

  TRUE if BCMCS information is valid, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*lint --e{528} suppress cmbcmcs_info_get not referenced
** warning 
*/
static boolean cmbcmcs_info_get(

  cm_bcmcs_info_s_type *bcmcs_info_ptr,
    /* Copy BCMCS info received from LL into this buffer
    */

  const cm_mc_rpt_type *cm_mc_rpt_ptr
    /* LL report with info to be passed to clients
    */

)
/*lint -esym(818, bcmcs_info_ptr) */
/*lint -esym(715, bcmcs_info_ptr) not referenced */
/*lint -esym(715, cm_mc_rpt_ptr) not referenced */
{
  cmbcmcs_s_type  *bcmcs_ptr = cmbcmcs_ptr();
    /* Point at BCMCS object
    */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( bcmcs_info_ptr != NULL );
  CM_ASSERT( cm_mc_rpt_ptr  != NULL );
  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Copy SYS_BCMCS_MAX_FLOWS instead of num_of_flows
  ** because we want to copy 0s as flow status if num_of_flows
  ** happens to be 0.
  ** This is for example when BCMCS status was no service
  ** when flow request command was queued to CM and BCMCS
  ** retry mechanism did not send anything to LL because there was
  ** no service and then SS declared that we've lost BCMCS
  ** service permanently - then we have not received any
  ** BCMCS report from LL and thus want to send one with
  ** all values set to default
  */
  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
    case CM_HDR_BCMCS_REG_STATUS_F :
      bcmcs_info_ptr->num_of_flows = cm_mc_rpt_ptr->bcmcs_reg_rpt.num_of_flows;
      bcmcs_info_ptr->protocol     = cm_mc_rpt_ptr->bcmcs_reg_rpt.protocol;

      memcpy( bcmcs_info_ptr->reg_status_array,
              cm_mc_rpt_ptr->bcmcs_reg_rpt.reg_status_array,
              SYS_BCMCS_MAX_FLOWS * sizeof(sys_bcmcs_flow_status_element_s_type) );
      break;

    case CM_HDR_BCMCS_FLOW_STATUS_F:
    case CM_HDR_BCMCS_BROADCAST_RESET_F:
    case CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F:
    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F:
    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F:
    default:

      bcmcs_info_ptr->num_of_flows = cm_mc_rpt_ptr->bcmcs_rpt.num_of_flows;
      bcmcs_info_ptr->protocol     = cm_mc_rpt_ptr->bcmcs_rpt.protocol;

      memcpy( bcmcs_info_ptr->flow_status_array,
              cm_mc_rpt_ptr->bcmcs_rpt.flow_status_array,
              SYS_BCMCS_MAX_FLOWS * sizeof(sys_bcmcs_flow_status_element_s_type) );
      break;
  }

  /* Return TRUE to indicate that the information is valid
  */
  return CM_INIT_CHECK( bcmcs_ptr->init_mask);
 
  /*lint +esym(613, cm_mc_rpt_ptr, bcmcs_info_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */
  
}
#endif
/*lint +esym(715, cm_mc_rpt_ptr) not referenced */
/*lint +esym(818, bcmcs_info_ptr) */
/*lint +esym(715, bcmcs_info_ptr) not referenced */
/*lint -restore */

/*===========================================================================

FUNCTION cmbcmcs_event

DESCRIPTION
  Notify the client list of a specified BCMCS event.

DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_event(

  cm_bcmcs_event_e_type    bcmcs_event
    /* notify client list of this BCMCS event
    */
)
/*lint -esym(529, bcmcs_ptr) not referenced */
{

  cm_bcmcs_info_s_type      *bcmcs_info_ptr = cmbcmcs_info_alloc();
    /* Allocate BCMCS info buffer
    */

  cmbcmcs_s_type            *bcmcs_ptr      = cmbcmcs_ptr();
    /* Point to BCMCS object
    */


  CM_ASSERT( BETWEEN(bcmcs_event, CM_BCMCS_EVENT_NONE, CM_BCMCS_EVENT_MAX) );
  CM_ASSERT( bcmcs_info_ptr != NULL );
  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );
  CM_FUNC_START( "cmbcmcs_event(), BCMCS event=%d",bcmcs_event,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy BCMCS information into
  ** allocated buffer.
  */
  #if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)
  (void) cmbcmcs_info_get( bcmcs_info_ptr, &bcmcs_ptr->bcmcs_rpt );
  #endif
  #if (defined (FEATURE_MODEM_MBMS))
#error code not present
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify the client list of the BCMCS event.
  */
  cmclient_list_bcmcs_event_ntfy( bcmcs_event, bcmcs_info_ptr );

}
/*lint +esym(529, bcmcs_ptr) not referenced */


/*===========================================================================

FUNCTION cmbcmcs_next_transaction_id

DESCRIPTION

  Return next transaction id.
  cmbcmcsi.h defines CM_BCMCS_TRANSACTION_ID_NONE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static byte cmbcmcs_next_transaction_id()
{

  cmbcmcs_s_type *bcmcs_ptr = cmbcmcs_ptr();

  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );

  /*------------------------------------------------------------------------*/
  bcmcs_ptr->transaction_id++;

  if(cmbcmcs_ptr()->transaction_id == CM_BCMCS_TRANSACTION_ID_NONE )
  {
    bcmcs_ptr->transaction_id++;
  }

  return bcmcs_ptr->transaction_id;

}


/*===========================================================================

FUNCTION cmbcmcs_cmd_xlate_ll_send

DESCRIPTION

  Translate CM command into LL command, send it to LL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_cmd_xlate_ll_send(void)
{
  #if (defined FEATURE_BCMCS) && (defined FEATURE_HDR) 
  cmbcmcs_s_type       *bcmcs_ptr      =  cmbcmcs_ptr();
  HDRMC_CMD_TYPE       *hdrmc_cmd_ptr  =  cm_hdrmc_get_buf_else_err_fatal();

  cm_bcmcs_cmd_s_type  *bcmcs_cmd_ptr  =  &bcmcs_ptr->bcmcs_cmd.cmd.bcmcs;
  uint8                 num_of_flows   =  0;
    /* Point at BCMCS object's copy of last BCMCS command
    */

  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( hdrmc_cmd_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(bcmcs_cmd_ptr->cmd)
  {

    case CM_BCMCS_CMD_FLOW_REQUEST_F:

      /* Figure out whether we need to send
      ** HDRMC_BCMCS_UPDATE_FLOW_REQ_F or
      ** HDRMC_BCMCS_DISCONTINUE_BROADCAST_F.
      ** Sending it over HDR is preferred so start
      ** from that, cmbcmcs_orig_proc() may switch it
      ** to 1X command later
      */
      num_of_flows = MIN(bcmcs_cmd_ptr->info.flow_req.num_of_flows, 
                         SYS_BCMCS_MAX_FLOWS );

      if(num_of_flows  == 0)
      {

        /* Gotta send HDRMC_BCMCS_DISCONTINUE_BROADCAST_F
        ** No payload
        */
        hdrmc_cmd_ptr->hdr.cmd = HDRMC_BCMCS_DISCONTINUE_BROADCAST_F;

        #ifdef FEATURE_BCMCS_20
#error code not present
        #else
        hdrmc_cmd_ptr->bcmcs_cmd.transaction_id = 
                                            cmbcmcs_next_transaction_id();
        #endif
      }
      else
      {

        /* Send command HDRMC_BCMCS_UPDATE_FLOW_REQ_F
        */
        hdrmc_cmd_ptr->hdr.cmd = HDRMC_BCMCS_UPDATE_FLOW_REQ_F;

        /* Copy flow information
        */

        #ifdef FEATURE_BCMCS_20
#error code not present
        #else
        hdrmc_cmd_ptr->bcmcs_cmd.num_of_flows = num_of_flows;

        memcpy(hdrmc_cmd_ptr->bcmcs_cmd.flow_addrs,
               bcmcs_cmd_ptr->info.flow_req.flow_addr_array,
               num_of_flows * sizeof(sys_bcmcs_flow_addr_s_type));

        /* Assign transaction id
        */
        hdrmc_cmd_ptr->bcmcs_cmd.transaction_id = cmbcmcs_next_transaction_id();

        #endif
      }


      /* Now queue it to HDR
      */
      cm_hdrmc_send_cmd( hdrmc_cmd_ptr );

      break; // CM_BCMCS_CMD_FLOW_REQUEST_F

    default:

      CM_SWITCH_ERR( "Bad BCMCS command %d",
                     bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.cmd, 0, 0);
      break;

  } // switch(bcmcs_cmd_ptr->cmd)
  #endif
}

/*========================================================================

FUNCTION cmbcmcs_cmd_xlate_ll_send2

DESCRIPTION

  Translate CM command into LL command, send it to LL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
static void cmbcmcs_cmd_xlate_ll_send2( 

    const cm_cmd_s_type    *cmd_ptr
    /* Pointer to a CM command
    */
)
{
  #if defined( FEATURE_BCMCS_20 ) 
#error code not present
  #else //FEATURE_BCMCS_20
  SYS_ARG_NOT_USED( cmd_ptr );
  #endif
}

#if defined (FEATURE_MODEM_MBMS)
#error code not present
#endif /* FEATURE_MODEM_MBMS */
/*lint +esym(818, tmgi_uint) */
/*lint +esym(818, tmgi_sdp) */
/*lint +esym(715, tmgi_uint) not referenced */
/*lint +esym(715, tmgi_sdp) not referenced */

/*===========================================================================

FUNCTION cmmbms_proc

DESCRIPTION

  Processing mbms requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmmbms_proc()
/*lint -esym(550, ret_val) */

{
  #if defined (FEATURE_MODEM_MBMS)
#error code not present
  #else
  CM_MSG_HIGH("FEATURE_MODEM_MBMS not defined",0,0,0);
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} /* cmmbms_proc() */
/*lint -esym(550, ret_val) */


/*===========================================================================

FUNCTION cmbcmcs_orig_proc

DESCRIPTION

  If BCMCS object is in CM_BCMCS_STATE_REQUESTING,
  continue trying to deliver LL command until
  positive ACK is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_orig_proc()
{

  cmbcmcs_s_type             *bcmcs_ptr                     =  cmbcmcs_ptr();    
  boolean                     bcmcs_srv_supported           = FALSE;
  sys_bcmcs_srv_status_e_type bcmcs_srv_status              = SYS_BCMCS_SRV_STATUS_NO_SRV;
  boolean                     sending_discontinue_broadcast = FALSE; 


  /* Verify integrity of BCMCS & SS objects
  */
  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.info.flow_req.flow_addr_array[0].flow_service
      == SYS_FLOW_SERVICE_MBMS)
  {
    cmmbms_proc ();
    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Figure out what set of BCMCS SS indicators to look at
  ** based on whether we're in hybrid or not
  */
  cmbcmcs_get_bcmcs_srv_indicators(&bcmcs_srv_supported,
                                   &bcmcs_srv_status);

  /* Now continue on based on BCMCS state
  */
  switch(bcmcs_ptr->state) {

    /* BCMCS is idle or waiting for ack from LL, nothing to do
    */
    case CM_BCMCS_STATE_IDLE:
    case CM_BCMCS_STATE_WAITING_FOR_ACK:
      CM_MSG_LOW("BCMCS is idle or waiting for LL ack, return ...",0,0,0);
      return; // case CM_BCMCS_STATE_IDLE

    /* Send BCMCS command to LL,
    ** move BCMCS object state to CM_BCMCS_STATE_WAITING_FOR_ACK
    */
    case CM_BCMCS_STATE_REQUESTING:

      /* 
      ** See if BCMCS is trying to send HDRMC_BCMCS_DISCONTINUE_BROADCAST_F
      ** This happens when CM_BCMCS_CMD_FLOW_REQUEST_F is queued with
      ** num_of_flows = 0 
      */ 
      sending_discontinue_broadcast = 
            (bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.cmd == CM_BCMCS_CMD_FLOW_REQUEST_F) &&
            (bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.info.flow_req.num_of_flows == 0); 

      /* 
      ** Send/resend the BCMCS command only if BCMCS is supported.
      ** Send HDRMC_BCMCS_DISCONTINUE_BROADCAST_F regardless of whether HDR
      ** is BCMCS capable. 
      ** Other commands will be resent when BCMCS on HDR system becomes available.
      */ 
      if ( bcmcs_srv_supported || sending_discontinue_broadcast )
      {
        cmss_s_type   *ss_ptr    = cmss_ptr();
        cm_ss_e_type  pref_ss    = CM_SS_NONE;
        boolean       in_pwrsave = FALSE;

        /* Figure out the SS that will be getting the BCMCS flow requests and
         * whether it's in power save.
        */
        if(cmpref_is_hybr_status_on())
        {
          pref_ss = CM_SS_HDR;
          if(ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_PWR_SAVE)
          {
            in_pwrsave = TRUE;
          }
        }
        else
        {
          pref_ss = CM_SS_MAIN;
          /* Not checking for sys_mode because a sys_mode other than HDR would turn
          ** off the BCMCS supported flag. 
          */
          if(ss_ptr->info.srv_status == SYS_SRV_STATUS_PWR_SAVE)
          {
            in_pwrsave = TRUE;
          }
        }

        /* If the LL is in power save, and we are not trying to discontinue
        ** BCMCS and it hasn't been kicked before, then kick the LL and wait
        ** for it to respond before sending it the BCMCS flow request.
        */
        if(!sending_discontinue_broadcast && (in_pwrsave || 
            bcmcs_ptr->waiting_pwrsave_exit_for_ss != CM_SS_NONE))
        {
          if(bcmcs_ptr->waiting_pwrsave_exit_for_ss != pref_ss )
          {
            CM_MSG_HIGH("Kicking SS=%d to wake it up",pref_ss,0,0);

            bcmcs_ptr->waiting_pwrsave_exit_for_ss = pref_ss;

            (void) cmph_force_pref_on_the_fly
                        (
                         pref_ss,
                         cmph_ptr(),
                         SD_SS_PREF_REAS_ORIG_START_PS,
                         CM_ACT_TYPE_DATA_CALL,
                         CM_PREF_TERM_CM_1_CALL_PLUS,
                         CM_MODE_PREF_NO_CHANGE,
                         CM_BAND_PREF_NO_CHANGE,
                         CM_PRL_PREF_NO_CHANGE,
                         CM_ROAM_PREF_NO_CHANGE,
                         CM_HYBR_PREF_NO_CHANGE,
                         cmph_ptr()->plmn,
                         CM_SRV_DOMAIN_PREF_NO_CHANGE,
                         CM_OTASP_ACT_CODE_NONE,
                         &cmph_ptr()->wlan_pref,
                         cmph_ptr()->acq_order_pref,
                         CM_NETWORK_SEL_MODE_PREF_NONE,
                         (cm_act_id_type) cmbcmcs_ptr(),
                         CM_ACT_UPDATE_REAS_ACT_START,
                         TRUE
                        );
          }
        }
        else
        {
          CM_MSG_HIGH("Send BCMCS cmd!!",0,0,0);

          cmbcmcs_cmd_xlate_ll_send();
          cmbcmcs_set_state(CM_BCMCS_STATE_WAITING_FOR_ACK);
        }

        return;
      }

      /* If HDR no longer supports BCMCS service,
      ** notify clients and go to idle 
      */
      CM_MSG_HIGH("!bcmcs_srv_supported",0,0,0);

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_FLOW_REQUEST_FAILURE );

      /* Move BCMCS into idle state
      */
      cmbcmcs_set_state(CM_BCMCS_STATE_IDLE);

      break; // case CM_BCMCS_STATE_REQUESTING

    default:
      CM_SWITCH_ERR( "Bad BCMCS state %d", bcmcs_ptr->state, 0, 0);
      break;
  }  // switch(bcmcs_ptr->state)

}

/*===========================================================================

FUNCTION cmbcmcs_cmd_store

DESCRIPTION

  Copy CM BCMCS command into the BCMCS object.


DEPENDENCIES
  Broadcast data object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_cmd_store(

    const cm_cmd_s_type    *cmd_ptr
        /* Pointer to a BCMCS command
        */
)
{

  cmbcmcs_s_type    *bcmcs_ptr  =  cmbcmcs_ptr();

  /* Verify integrity of BCMCS object and function parameters
  */
  CM_ASSERT( bcmcs_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK( bcmcs_ptr->init_mask) );
  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Simply copy the command into BCMCS object and
  ** put BCMCS into CM_BCMCS_STATE_REQUESTING state
  */
  memcpy(&bcmcs_ptr->bcmcs_cmd,
          cmd_ptr,
          sizeof(cm_cmd_s_type));


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if ((bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.info.flow_req.flow_addr_array[0].flow_service
       == SYS_FLOW_SERVICE_MBMS
       )
      )
  {
    CM_MSG_HIGH ("MBMS - storing cmd. num_flows = %d", 
                  bcmcs_ptr->bcmcs_cmd.cmd.bcmcs.info.flow_req.num_of_flows, 0, 0);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* It's legal to send a new CM_BCMCS_CMD_FLOW_REQUEST_F while
  ** BCMCS object is already processing one.
  ** In this case the old command will simply be replaced by
  ** the new one and all LL notifications pertaining to the
  ** old one will be dropped by CM
  */
  if(bcmcs_ptr->state != CM_BCMCS_STATE_IDLE)
  {
      CM_MSG_MED("BCMCS - drop previous CM_BCMCS_CMD_FLOW_REQUEST_F command ...",0,0,0);
  }

  bcmcs_ptr->state = CM_BCMCS_STATE_REQUESTING;

} // cmbcmcs_cmd_store()


/*===========================================================================

FUNCTION cmbcmcs_client_cmd_err

DESCRIPTION
  Notify clients of a specified BCMCS command error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmbcmcs_client_cmd_err(

  const cm_cmd_s_type       *cmd_ptr,
    /* Pointer to a CM command
    */

  cm_bcmcs_cmd_err_e_type    cmd_err
    /* Indicate the BCMCS command error
    */
)
{
  const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_BCMCS );
  CM_ASSERT( BETWEEN( cmd_err, CM_BCMCS_CMD_ERR_NONE, CM_BCMCS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at BCMCS command component
  */
  bcmcs_cmd_ptr =   BCMCS_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmbcmcs_client_cmd_err() bcmcs_cmd=%d, cmd_err=%d, client=%p",
                  bcmcs_cmd_ptr->cmd, cmd_err, bcmcs_cmd_ptr->client_id );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( bcmcs_cmd_ptr->cmd_cb_func != NULL )
  {
    bcmcs_cmd_ptr->cmd_cb_func( bcmcs_cmd_ptr->data_block_ptr,
                                bcmcs_cmd_ptr->cmd,
                                cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_BCMCS_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH( "BCMCS cmd err!, cmd=%d, err=%d, client=%ld",
                 bcmcs_cmd_ptr->cmd, cmd_err, bcmcs_cmd_ptr->client_id );

    cmclient_list_bcmcs_cmd_err_ntfy( cmd_err, bcmcs_cmd_ptr );
  }
}


/*===========================================================================

FUNCTION cmbcmcs_client_cmd_mmll_reply

DESCRIPTION

  Process LL reply to the command, send events to clients
  if necessary.

DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE

  None

SIDE EFFECTS

  None

===========================================================================*/
static void cmbcmcs_client_cmd_mmll_reply(

  const cm_cmd_s_type     *cmd_ptr,
    /* Pointer to a CM command
    */

  cm_bcmcs_cmd_err_e_type     cmd_err
    /* Indicate BCMCS command error
    */
)
/*lint -esym(715, cmd_err) */
{
  const cm_bcmcs_cmd_s_type       *bcmcs_cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( cmd_err == CM_BCMCS_CMD_ERR_NOERR );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_BCMCS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at BCMCS command component
  */
  bcmcs_cmd_ptr = BCMCS_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmbcmcs_client_cmd_mmll_reply(), cmd=%d, err=%d",
                  bcmcs_cmd_ptr->cmd, cmd_err, 0 );

  switch( bcmcs_cmd_ptr->cmd )
  {
    /*--------------------------------------------*/
    /* CM_BCMCS_CMD_FLOW_REQUEST_F was sent to LL */
    /*--------------------------------------------*/

    case CM_BCMCS_CMD_FLOW_REQUEST_F:
    case CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F:
    case CM_BCMCS_CMD_REG_REQUEST_F:
    #ifdef FEATURE_HDR_BCMCS_DYNAMIC_MODEL
#error code not present
    #endif
      /* Nothing to do here
      */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_SWITCH_ERR( "Bad BCMCS command %d", bcmcs_cmd_ptr->cmd, 0, 0);
      break;

  } // switch
}
/*lint +esym(715, cmd_err) */

/*===========================================================================

FUNCTION cmbcmcs_client_cmd_forward_mmll

DESCRIPTION

  Perform prep work before a client BCMCS command is sent to LL and
  (eventually) send the command to LL.
  The command itself may or may not be sent at the time this
  function is called.

DEPENDENCIES

  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE

  TRUE if CM needs to wait for a replay from LL before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmbcmcs_client_cmd_forward_mmll(

  const cm_cmd_s_type    *cmd_ptr
    /* Pointer to a CM command
    */
)
{
  const cm_bcmcs_cmd_s_type       *bcmcs_cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_BCMCS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at BCMCS command component
  */
  bcmcs_cmd_ptr = BCMCS_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmbcmcs_client_cmd_forward_mmll(),cmd=%d",bcmcs_cmd_ptr->cmd,0,0);

  switch( bcmcs_cmd_ptr->cmd )
  {

    /*---------------------------------------------------*/
    /* Process CM_BCMCS_CMD_FLOW_REQUEST_F here          */
    /*---------------------------------------------------*/
    /* Process CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F         */
    /* DEACT REQ Used by MBMS only at this point         */
    /*---------------------------------------------------*/

      case CM_BCMCS_CMD_FLOW_REQUEST_F:
      case CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F:

        /* Copy CM command into BCMCS object
        */
        cmbcmcs_cmd_store( cmd_ptr );
  
        /* Kick start BCMCS object
        */
        cmbcmcs_orig_proc();
  
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_BCMCS_CMD_REG_REQUEST_F:
      #ifdef FEATURE_HDR_BCMCS_DYNAMIC_MODEL
#error code not present
      #endif
        /* Send the command directly to LL.
        */

        cmbcmcs_cmd_xlate_ll_send2( cmd_ptr );

        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      CM_SWITCH_ERR( "Bad BCMCS command %d", bcmcs_cmd_ptr->cmd, 0, 0);
      break;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the caller of this function that BCMCS data is not
  ** waiting for the reply from MC.
  */
  return FALSE;
}

/*===========================================================================

FUNCTION cmbcmcs_client_cmd_check

DESCRIPTION
  Check for BCMCS command parameter errors and whether a specified BCMCS
  command is allowed in the current state of the system.


DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  CM_BCMCS_CMD_ERR_NOERR if command is allowed in the current state
  of the BCMCS, specific cm_bcmcs_cmd_err_e_type otherwise.

SIDE EFFECTS
  Might change one or more of the command parameters pointed by cmd_ptr.

===========================================================================*/
static cm_bcmcs_cmd_err_e_type  cmbcmcs_client_cmd_check(

  cm_cmd_s_type                  *cmd_ptr
    /* Pointer to a CM command
    */
)
{
  cm_bcmcs_cmd_s_type               *bcmcs_cmd_ptr = NULL;
    /* Point at BCMCS command component
    */

  cm_bcmcs_cmd_err_e_type            cmd_err       = CM_BCMCS_CMD_ERR_NOERR;
    /* Initialize command error to NO-ERROR
    */

  boolean                      bcmcs_srv_supported = FALSE;
  sys_bcmcs_srv_status_e_type  bcmcs_srv_status    = SYS_BCMCS_SRV_STATUS_NONE;
    /* BCMCS service indicators
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_BCMCS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bcmcs_cmd_ptr = BCMCS_CMD_PTR(cmd_ptr);
  CM_ASSERT( BETWEEN(bcmcs_cmd_ptr->cmd, CM_BCMCS_CMD_NONE, CM_BCMCS_CMD_MAX ));
  CM_FUNC_START( "cmbcmcs_client_cmd_check(), cmd=%d",bcmcs_cmd_ptr->cmd,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get BCMCS service indicators
  */
  cmbcmcs_get_bcmcs_srv_indicators(&bcmcs_srv_supported,
                                   &bcmcs_srv_status);

  /* Check whether a specified BCMCS command is allowed in the
  ** current state of the system.
  */
  switch( bcmcs_cmd_ptr->cmd )
  {

            /*---------------------------------*/
            /* Send flow request command to LL */
            /*---------------------------------*/


    case CM_BCMCS_CMD_FLOW_REQUEST_F:

      /* - For BCMCS Check whether HDR is configured to support BCMCS service
      ** (we don't care about BCMCS service status as we can resend the
      ** command if BCMCS service is temporarily unavailable)
      ** 
      ** - For MBMS, Check if WCDMA service is present and MBMS srv is avail
      */
      if( (!bcmcs_srv_supported)  &&
          (bcmcs_cmd_ptr->info.flow_req.num_of_flows > 0) &&
           bcmcs_cmd_ptr->info.flow_req.flow_addr_array[0].flow_service 
                                         == SYS_FLOW_SERVICE_BCMCS)
            
      {
        /* If CM_BCMCS_CMD_FLOW_REQUEST_F came with num_of_flows = 0,
        ** then it will result in CM sending HDRMC_BCMCS_DISCONTINUE_BROADCAST_F 
        ** to HDR. 
        ** HDRMC_BCMCS_DISCONTINUE_BROADCAST_F should be sent to HDR
        ** regardless of whether HDR is configured to support BCMCS.
        ** Otherwise flag an error.  
        */
        if(bcmcs_cmd_ptr->info.flow_req.num_of_flows != 0)
        {
          cmd_err = CM_BCMCS_CMD_ERR_BCMCS_SRV_NOT_AVAILABLE_S;
        }
      }
      break; // CM_BCMCS_CMD_FLOW_REQUEST_F

    case CM_BCMCS_CMD_REG_REQUEST_F:
    #ifdef FEATURE_HDR_BCMCS_DYNAMIC_MODEL
#error code not present
    #endif
      /*
      ** Num of Flows can not be zero as REG_REQUEST/HANDOFF 
      ** has to be sent for atleast 1 flow.
      */
      if (!INRANGE(bcmcs_cmd_ptr->info.reg_req.num_of_flows,
                   1,
                   SYS_BCMCS_MAX_FLOWS ))
      {
        cmd_err = CM_BCMCS_CMD_ERR_INVALID_NUMBER_OF_FLOW_IDS_P;
      }
      /*lint -e{774,831} */
      if ( bcmcs_cmd_ptr->info.reg_req.flow_addr_array == NULL ) 
      {
        cmd_err = CM_BCMCS_CMD_ERR_FLOW_IDS_NULL_PTR_P;
      }
      break;

    #ifdef FEATURE_HDR_BCMCS_DYNAMIC_MODEL
#error code not present
    #endif

    /* - For MBMS, Check if WCDMA service is present and MBMS srv is avail
    */
    case CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F:
      break;

    default:

      cmd_err = CM_BCMCS_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad BCMCS command %d", bcmcs_cmd_ptr->cmd, 0, 0);
      break;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_err;
} // cmbcmcs_client_cmd_check()

/**--------------------------------------------------------------------------
** Functions - external, common
** --------------------------------------------------------------------------
*/

#if (defined FEATURE_BCMCS) && (defined FEATURE_HDR) 
/*===========================================================================

FUNCTION cmbcmcs_mc_rpt_proc

DESCRIPTION

  Process LL reports related to BCMCS


DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmbcmcs_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  /*lint -esym(613, cm_mc_rpt_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

  /*lint -save -e826 */
  cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
  /*lint -restore */
    /* Pointer to MC report */
    
  cmbcmcs_s_type                *bcmcs_ptr     = cmbcmcs_ptr();
    /* Point at BCMCS object
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );
  CM_ASSERT( bcmcs_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmbcmcs_mc_rpt_proc(), cmd=%d",
                  cm_mc_rpt_ptr->hdr.cmd, 0, 0);

  /* Check the report for errors
  */
  if(!cmbcmcs_mc_rpt_check(cm_mc_rpt_ptr)) {
    return;
  }

  /* Store the report
  */
  cmbcmcs_mc_rpt_store(cm_mc_rpt_ptr);

  /* Process the report
  */
  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F:

      /* If BCMCS is in idle state, complain and return
      */
      if( bcmcs_ptr->state == CM_BCMCS_STATE_IDLE )
      {
        CM_ERR("CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F dropped - BCMCS is in idle state!",0,0,0);
        return;
      }

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_FLOW_REQUEST_SUCCESS );

      /* Move BCMCS into idle state
      */
      cmbcmcs_set_state(CM_BCMCS_STATE_IDLE);

      break; // CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F:

      /* If BCMCS is in idle state, complain and return
      */
      if( bcmcs_ptr->state == CM_BCMCS_STATE_IDLE )
      {
        CM_ERR("CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F dropped - BCMCS is in idle state!",0,0,0);
        return;
      }

      /* Move BCMCS into CM_BCMCS_STATE_REQUESTING state
      */
      cmbcmcs_set_state(CM_BCMCS_STATE_REQUESTING);

      /* Resend the last command
      */
      cmbcmcs_orig_proc();

      break; // CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* */
    case CM_HDR_BCMCS_BROADCAST_RESET_F:

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_BROADCAST_RESET );

      break; // CM_HDR_BCMCS_BROADCAST_RESET_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F:

      /* If BCMCS is in idle state, complain and return
      */
      if( bcmcs_ptr->state == CM_BCMCS_STATE_IDLE )
      {
        CM_ERR("CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F dropped - BCMCS is in idle state!",0,0,0);
        return;
      }

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_FLOW_REQUEST_SUCCESS );

      /* Move BCMCS into CM_BCMCS_STATE_IDLE state
      */
      cmbcmcs_set_state(CM_BCMCS_STATE_IDLE);

      break; // CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_FLOW_STATUS_F:

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_FLOW_STATUS_CHANGE );

      break; // CM_HDR_BCMCS_FLOW_STATUS_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_FLOW_MONITORING_START_F:

      bcmcs_ptr->flow_monitoring_state = CM_BCMCS_FLOW_MONITORING_STATE_ACTIVE;

      /* DDTM on */
      cmph_update_ddtm_status(cmph_ptr());
      break; // CM_HDR_BCMCS_FLOW_MONITORING_START_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /*
    */
    case CM_HDR_BCMCS_FLOW_MONITORING_END_F:

      bcmcs_ptr->flow_monitoring_state = CM_BCMCS_FLOW_MONITORING_STATE_NONE;

      /* DDTM off */
      cmph_update_ddtm_status(cmph_ptr());
      break; // CM_HDR_BCMCS_FLOW_MONITORING_END_F

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  case CM_HDR_BCMCS_REG_STATUS_F:

      /* Send event to registered clients
      */
      cmbcmcs_event( CM_BCMCS_EVENT_REGISTRATION_STATUS );

      break; // CM_HDR_BCMCS_REG_STATUS_F

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      CM_SWITCH_ERR("Unknown BCMCS report = %d!!!",cm_mc_rpt_ptr->hdr.cmd,0,0);
      break;

  } // switch( cm_mc_rpt_ptr->hdr.cmd )
}
#endif /* FEATURE_BCMCS */

/*===========================================================================

FUNCTION cmbcmcs_gw_rpt_proc

DESCRIPTION

  Process LL reports related to MBMS


DEPENDENCIES
  BCMCS object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmbcmcs_gw_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
/*lint -esym(715, rpt_ptr) not referenced */
{  
  /*lint -esym(613, cm_mc_rpt_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 

  #if defined(FEATURE_MODEM_MBMS)
#error code not present
  #else
  CM_MSG_HIGH("FEATURE_MODEM_MBMS not defined",0,0,0);
  #endif

  /*lint +esym(613, cm_mc_rpt_ptr) 
  ** NOTE -save/-restore do not work with -esym() 
  */ 
}
/*lint +esym(715, rpt_ptr) not referenced */

/*===========================================================================

FUNCTION cmbcmcs_sd_rpt_proc

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Bcmcs object must have already been initialized with
  cmbcmcs_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmbcmcs_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
  /*lint -save -e826 */
  const cm_sd_rpt_u_type  *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
  /*lint -restore */
    /* Pointer to SD report */

  cmbcmcs_s_type             *bcmcs_ptr                     = cmbcmcs_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at SD report component */
  CM_FUNC_START( "cmcall_sd_rpt_proc, cmd=%d", sd_rpt_ptr->hdr.cmd, 0, 0);

  switch( sd_rpt_ptr->hdr.cmd )
  {
                 /*----------------------------------------*/
                 /*  OK to send origination command to LL  */
                 /*----------------------------------------*/

    case CM_OK_TO_ORIG_F:

      if(cmph_map_sd_ss_type_to_cm_ss_type(sd_rpt_ptr->srv_ind_info.ss) ==
                                bcmcs_ptr->waiting_pwrsave_exit_for_ss)
      {
        CM_MSG_MED("bcmcs: OK_TO_ORIG, ss %d",
                    sd_rpt_ptr->ok_to_orig.ss, 0, 0);

        bcmcs_ptr->waiting_pwrsave_exit_for_ss = CM_SS_NONE;
        cmbcmcs_orig_proc();
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*-------------------------------------------*/
               /*   Acquisition on all systems has failed   */
               /*-------------------------------------------*/

    case CM_ACQ_FAIL_F:

      if(cmph_map_sd_ss_type_to_cm_ss_type(sd_rpt_ptr->srv_ind_info.ss) ==
                                bcmcs_ptr->waiting_pwrsave_exit_for_ss)
      {
        CM_MSG_MED("bcmcs: ACQ_FAIL, ss %d", sd_rpt_ptr->acq_fail.ss, 0, 0 );

        bcmcs_ptr->waiting_pwrsave_exit_for_ss = CM_SS_NONE;
        cmbcmcs_orig_proc();
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      break;

  } /* switch( sd_rpt_ptr->hdr.cmd ) */

} /* cmcall_sd_rpt_proc() */

/*===========================================================================

FUNCTION cmbcmcs_init

DESCRIPTION

  Initializing the BCMCS object.

  This function must be called before the BCMCS object
  is being used, in any way, place, or form.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmbcmcs_init( void )
{

  cmbcmcs_s_type    *bcmcs_ptr  =  cmbcmcs_ptr();
    /* Point at BCMCS object to be initialized
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( bcmcs_ptr != NULL );
  CM_FUNC_START( "cmbcmcs_init()",0,0,0 );

  /* Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_INIT_MARK( bcmcs_ptr->init_mask );

  /* Initialize BCMCS object as idle
  */
  bcmcs_ptr->state = CM_BCMCS_STATE_IDLE;

  /* Initialize transaction id
  */
  bcmcs_ptr->transaction_id = CM_BCMCS_TRANSACTION_ID_NONE;

  /* Initialize BCMCS object as not waiting for power save exit. 
  */
  bcmcs_ptr->waiting_pwrsave_exit_for_ss = CM_SS_NONE;

  /* Initialize the flow monitoring state
  */
  bcmcs_ptr->flow_monitoring_state = CM_BCMCS_FLOW_MONITORING_STATE_NONE; 

  /* Initialize BCMCS internal buffers
  */
  cmbcmcs_reset_buffers();

}

/*===========================================================================

FUNCTION cmbcmcs_client_cmd_proc

DESCRIPTION
  Process clients' BCMCS commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmbcmcs_client_cmd_proc(

  cm_cmd_s_type  *cmd_ptr
    /* pointer to a CM command
    */
)
{

  cm_bcmcs_cmd_err_e_type  bcmcs_cmd_err;
  boolean                  wait_for_reply;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmbcmcs_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_BCMCS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the system.
  */
  bcmcs_cmd_err = cmbcmcs_client_cmd_check( cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmbcmcs_client_cmd_err( cmd_ptr, bcmcs_cmd_err );
  if( bcmcs_cmd_err != CM_BCMCS_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  wait_for_reply = cmbcmcs_client_cmd_forward_mmll( cmd_ptr );

  /* If NO need to wait for replay from Lower Layer,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmbcmcs_client_cmd_mmll_reply( cmd_ptr,
                                   CM_BCMCS_CMD_ERR_NOERR );
  }

}

/*===========================================================================

FUNCTION cmbcmcs_ptr

DESCRIPTION

  Return a pointer to the one and only BCMCS object.

  The BCMCS object is responsible for:
  1. Processing clients' BCMCS commands.
  2. Processing LL replies for clients' BCMCS commands.
  3. Processing LL BCMCS related notifications.
  4. Notifying the client list of BCMCS events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmbcmcs_s_type  *cmbcmcs_ptr( void )
{

  static cmbcmcs_s_type cmbcmcs_local;
    /* The one and only BCMCS object
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmbcmcs_local;

}

/*==============================================================================
FUNCTION cmbcmcs_srv_status_is_srv

DESCRIPTION

  Function that verifies that the input value indicates BCMCS service.

PARAMETERS

  srv_status_is_srv  value      Value that is to be checked for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates service (TRUE)
           or no service (FALSE).

==============================================================================*/
boolean cmbcmcs_srv_status_is_srv
(
  sys_bcmcs_srv_status_e_type  value
)
{

  return (value == SYS_BCMCS_SRV_STATUS_SRV);

}

#else // #if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)

/*===========================================================================
FUNCTION cmbcmcs_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmbcmcs_dummy( void )
/*lint -esym(714,cmbcmcs_dummy) */
/*lint -esym(528,cmbcmcs_dummy) */
/*lint -esym(765,cmbcmcs_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}

#endif // #if (defined FEATURE_BCMCS) && (defined FEATURE_HDR)






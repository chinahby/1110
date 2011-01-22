/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   I N B A N D   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Inband Object.


EXTERNALIZED FUNCTIONS

 cminband_client_cmd_proc   - process client inband commnads

 cminband_mc_rpt_proc       - process inband related MC reports

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1991 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cminband.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   sv      Resolve Klocwork defects
11/16/07   bh      added rejection for burst dtmf in voip call
08/07/07   ic      Lint clean up
02/23/07   ic      Copy call id from MC report for CM_SND_BRST_DTMF_ACK_F 
                   processing.
12/28/06   ka      Allowing A, B, C, D digits as DTMF on GSM/WCDMA.
08/16/06   ic      Lint fixes
07/19/06   ic      Lint fixes
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/07/06   ka      Checking for voip call to be in conv before dmtf gets sent.
                   Sending duration as argument for stop cont dtmf.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/25/05   ic      Lint touch ups 
11/23/05   ka      Modification to allow dtmf on ip calls.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/02/05   ic      Print value that caused error in default switch case
08/03/04   ic      CM Lint clean up - cminband.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/12/07   ic      Fixed Lint issues (indentation)
07/06/04   dk      Modified signature of cm_find_connection_id().
06/23/04   ic      Fixed ADS 120 compiler warnings (Jaguar build)
06/22/04   ic      Fixed obvious RVCT 2.1 compiler warnings
05/27/04   ic      In cminband_client_cmd_proc() call cminband_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/13/04   ka      Merged changes from MSM6200
06/18/03   vt      Do not fatal error for dtmf digits over 12
05/02/03   ws      Fixed compiler/linking warnings when feature flags are off
04/28/03   AT      Lint cleanup.
01/29/03   vt      Rejects dtmf burst for GW
01/21/03   ws      Updated copyright information for 2003
10/21/02   vt      Replaced checking of call id with call object.
06/13/02   prk     Modified cminband_client_cmd_forward_ll to return if not
                   in digital mode.
05/13/02   prk     Modified arguments to call to CM_FUNC_START in
                   cminband_rpt_proc.
02/13/02   ic      Compile errors fixes related to the fact that call_id field
                   was removed from cm_inband_cmd_s_type (not needed, we use
                   call_id field in cm_inband_cmd__info_s_type instead).
                   Removed const modifier from cm_inband_cmd_s_type  *inband_cmd_ptr
                   function parameter (ripple effect of removing call_id from
                   cm_inband_cmd_s_type structure)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A_SVD undefined
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
01/23/01   ych     Merged T53 and JCDMA features.
08/30/99   SH      Fixed complier warning when forward DTMF is not defined
07/06/99   SH      Added feature flag for forward DTMF
06/21/99   SH      Added checking for cmd_err in cmd info
06/07/99   SH      Removed memcpy to copy digit info for forward burst DTMF
05/25/99   SH      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"
#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "cminband.h"  /* Interface to CM inband Object */
#include "comdef.h"    /* Definition for basic types and macros */
#include <string.h>    /* Interface to string library */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmcall.h"    /* interface to CM Call object */
#include "cmph.h"      /* interface to CM Phone object */
#include "cmss.h"

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cmxll.h"
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

#if defined(FEATURE_IP_CALL)
#include "cmipcall.h"  /* Interface to CM ip functionality */
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
static cm_inband_cmd_err_e_type  check_dtmf_on_length( uint8 on_length );
static cm_inband_cmd_err_e_type  check_dtmf_off_length( uint8 off_length );
static void cminband_client_cmd_forward_mc( const cm_cmd_s_type  *cmd_ptr );
#endif /* defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

/* Forwards dtmf requests to CM IP section
*/
static void cminband_client_cmd_forward_ip( const cm_cmd_s_type *cmd_ptr );

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION  check_dtmf_digits
DESCRIPTION
  Check whether the DTMF digits are in valid range

DEPENDENCIES
  none
RETURN VALUE
  CM_INBAND_CMD_ERR_NONE if no parameter related errors,
  otherwise CM_INBAND_CMD_ERR_OFF_LEN_P is returned

SIDE EFFECTS
  none

===========================================================================*/
static cm_inband_cmd_err_e_type  check_dtmf_digits(
  uint8 cnt,
  const uint8 *digit_buf
)
{
  cm_inband_cmd_err_e_type    cmd_err = CM_INBAND_CMD_ERR_NOERR;
  uint8                       i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(digit_buf != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for( i=0; i < cnt; i++ )
  {
    if( !DECCHK(digit_buf[i]) &&
        digit_buf[i] != '*'   &&
        digit_buf[i] != '#'

        /* A, B, C, D are allowed as DTMF digits on GW.
        ** 24.008 Release 5. Section 5.5.7.1.
        */
        #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
        #endif
      )
    {
      cmd_err = CM_INBAND_CMD_ERR_DIGIT_P;
      break;
    }
  }

  return cmd_err;
}




/*===========================================================================

FUNCTION  cminband_inband_cmd_burst_dtmf_para_check

DESCRIPTION
  Check parameter errors for the burst DTMF command

DEPENDENCIES
  none
RETURN VALUE
  CM_INBAND_CMD_ERR_NONE if no parameter related errors,
  otherwise specific cm_inband_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static cm_inband_cmd_err_e_type  cminband_inband_cmd_burst_dtmf_para_check(
  cm_inband_cmd_s_type       *inband_cmd_ptr
)
{

  cm_inband_cmd_info_s_type  *cmd_info_ptr =  NULL;
  cm_inband_cmd_err_e_type    cmd_err       =  CM_INBAND_CMD_ERR_NOERR;

  cm_call_mode_info_e_type    call_mode;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(inband_cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr =  CMD_INFO_PTR( inband_cmd_ptr );
  call_mode = cmcall_call_id_mode(inband_cmd_ptr->info.call_id );

  if ( (cmd_err = check_dtmf_digits(cmd_info_ptr->cnt, cmd_info_ptr->digits))
      != CM_INBAND_CMD_ERR_NOERR )
  {
      CM_DBG_ERR_FATAL( "CM_INBAND_CMD_ERR_DIGIT_P",0,0,0 );
  }

  #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  else if ( call_mode == CM_CALL_MODE_INFO_CDMA )
  {
    if( (cmd_err = check_dtmf_on_length(cmd_info_ptr->on_length))
        != CM_INBAND_CMD_ERR_NOERR )
    {
        CM_DBG_ERR_FATAL( "CM_INBAND_CMD_ERR_ON_LEN_P",0,0,0 );
    }
    else if ( (cmd_err = check_dtmf_off_length(cmd_info_ptr->off_length))
        != CM_INBAND_CMD_ERR_NOERR )
    {
        CM_DBG_ERR_FATAL( "CM_INBAND_CMD_ERR_OFF_LEN_P",0,0,0 );
    }
  } /* else if ( call_mode == CM_CALL_MODE_INFO_CDMA ) */
  #endif /* #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) */

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

  #if (defined(FEATURE_IP_CALL))
  /* CM_INBAND_CMD_BURST_DTMF is completely rejected, no supported in this feature */
  else if ( call_mode == CM_CALL_MODE_INFO_IP ) 
  {
    cmd_err = CM_INBAND_CMD_ERR_OTHER;
  }
  #endif
  
  return cmd_err;
} /* cminband_inband_cmd_burst_dtmf_para_check() */



/*===========================================================================

FUNCTION cminband_client_cmd_check

DESCRIPTION
  Check for inband command parameter errors and whether a specified inband
  command is allowed in the current state of the call/phone.


DEPENDENCIES
  None.

RETURN VALUE
  CM_INBAND_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_inband_cmd_err_e_type otherwise.

SIDE EFFECTS
  Might change one or more of the command parameters pointed by cmd_ptr.

===========================================================================*/
static cm_inband_cmd_err_e_type  cminband_client_cmd_check(

    cm_cmd_s_type               *cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_inband_cmd_s_type      *inband_cmd_ptr = NULL;
      /* Point at inband command component */

  cm_inband_cmd_info_s_type *cmd_info_ptr   = NULL;
      /* Point at command information component */


  cmcall_s_type             *call_ptr;
    /* Pointer to a call object */

  cm_inband_cmd_err_e_type  cmd_err         = CM_INBAND_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_INBAND );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  inband_cmd_ptr = INBAND_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cminband_client_cmd_check(), cmd=%d",inband_cmd_ptr->cmd,0,0 ); 
  cmd_info_ptr   = CMD_INFO_PTR( inband_cmd_ptr );


  /* check cmd is already in error or not */

  if (cmd_info_ptr->cmd_err != CM_INBAND_CMD_ERR_NOERR)
  {
    return cmd_info_ptr->cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether this call id is allocated */
  call_ptr = cmcall_ptr( cmd_info_ptr->call_id );
  if ( call_ptr == NULL )
  {
    return CM_INBAND_CMD_ERR_CALL_ID_P;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether a specified inband command is allowed in the
  ** current state of the call/phone.
  */
  switch( inband_cmd_ptr->cmd )
  {

         /*------------*/
         /* Burst DTMF */
         /*------------*/


    case CM_INBAND_CMD_BURST_DTMF:

      /* check command parameter errors */
      cmd_err = cminband_inband_cmd_burst_dtmf_para_check(inband_cmd_ptr);
	  break;


    case CM_INBAND_CMD_START_CONT_DTMF:

      /* check command parameter errors */
      cmd_err = check_dtmf_digits(1, cmd_info_ptr->digits);
      break;

    case CM_INBAND_CMD_STOP_CONT_DTMF:

      /* no check is needed */
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:

      cmd_err = CM_INBAND_CMD_ERR_OTHER;
      CM_SWITCH_ERR( "Bad inband command %d", inband_cmd_ptr->cmd, 0, 0);
      break;

  }

  /* Return error for inband cmd check */
  if(cmd_err != CM_INBAND_CMD_ERR_NOERR)
  {
    return cmd_err;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only process inband commands while in the conversation state
  */
  #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)  
#error code not present
  #endif // #if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)


  /* IF call is a IP call, it should be in CONV for DTMF to pass
  */
  if ( (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP) &&
       (call_ptr->call_state                 != CM_CALL_STATE_CONV )
     )
  {  
    return CM_INBAND_CMD_ERR_BAD_STATE_S;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Phone has to be online for inband to be processed 
  */
  if( cmph_ptr()->oprt_mode != SYS_OPRT_MODE_ONLINE )
  {
    cmd_err = CM_INBAND_CMD_ERR_OFFLINE_S;
  }
 
  

  return cmd_err;

}




/*===========================================================================

FUNCTION cminband_client_cmd_err

DESCRIPTION
  Notify clients of a specified inband command error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cminband_client_cmd_err(

    const cm_cmd_s_type     *cmd_ptr,
        /* Pointer to a CM command */

    cm_inband_cmd_err_e_type    cmd_err
        /* Indicate the inband command error */
)
{
  const cm_inband_cmd_s_type    *inband_cmd_ptr =  NULL;
      /* Point at inband command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_INBAND );
  CM_ASSERT( BETWEEN( cmd_err, CM_INBAND_CMD_ERR_NONE, CM_INBAND_CMD_ERR_MAX) );
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  inband_cmd_ptr =   INBAND_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cminband_client_cmd_err() inband_cmd=%d, cmd_err=%d, client=%p",
                  inband_cmd_ptr->cmd, cmd_err, inband_cmd_ptr->client_id );
  

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( inband_cmd_ptr->cmd_cb_func != NULL )
  {
    inband_cmd_ptr->cmd_cb_func( inband_cmd_ptr->data_block_ptr,
                                 inband_cmd_ptr->cmd,
                                 cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_INBAND_CMD_ERR_NOERR )
  {
    MSG_ERROR( "Inband cmd err!, cmd=%d, err=%d, client=%ld",
                 inband_cmd_ptr->cmd, cmd_err, inband_cmd_ptr->client_id );

    cmclient_list_inband_cmd_err_ntfy( cmd_err, inband_cmd_ptr );
  }

}



/*===========================================================================

FUNCTION cminband_info_alloc

DESCRIPTION
 Allocate a inband info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated inband info buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cm_inband_info_s_type  *cminband_info_alloc( void )
{
  static cm_inband_info_s_type   inband_info;
      /* Currently there is only a need for one inband info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simply return a pointer to the statically allocated
  ** inband info buffer.
  */
  return &inband_info;
}







/*===========================================================================

FUNCTION cminband_rev_event_ntfy

DESCRIPTION
  Notify the client list of a specified reverse inband event.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cminband_rev_event_ntfy(
  cm_cmd_s_type  *cmd_ptr

)
{
  cm_inband_cmd_s_type      *inband_cmd_ptr     = NULL;
        /* Point at inband command component */

  cm_inband_cmd_info_s_type *cmd_info_ptr       = NULL;
        /* Point at command information component */

  cm_inband_info_s_type     *inband_info_ptr    = cminband_info_alloc();
      /* Allocate inband info buffer */

  cm_inband_event_e_type    inband_event        = CM_INBAND_EVENT_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr         != NULL );
  CM_ASSERT( inband_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inband_cmd_ptr     = INBAND_CMD_PTR( cmd_ptr );
  cmd_info_ptr       = CMD_INFO_PTR( inband_cmd_ptr );

  /* Copy inband information into
  ** allocated buffer.
  */

  switch (inband_cmd_ptr->cmd)
  {

    case CM_INBAND_CMD_BURST_DTMF:
      inband_info_ptr->call_id    = cmd_info_ptr->call_id;
      inband_info_ptr->on_length  = cmd_info_ptr->on_length;
      inband_info_ptr->off_length = cmd_info_ptr->off_length;
      inband_info_ptr->cnt        = cmd_info_ptr->cnt;
      memcpy(inband_info_ptr->digits, cmd_info_ptr->digits, cmd_info_ptr->cnt);
      inband_event = CM_INBAND_EVENT_REV_BURST_DTMF;
      break;

      case CM_INBAND_CMD_START_CONT_DTMF:
      inband_info_ptr->call_id   = cmd_info_ptr->call_id;
      inband_info_ptr->cnt       = 1;
      inband_info_ptr->digits[0] = cmd_info_ptr->digits[0];
      inband_event = CM_INBAND_EVENT_REV_START_CONT_DTMF;
      break;

    case CM_INBAND_CMD_STOP_CONT_DTMF:
      inband_info_ptr->call_id  = cmd_info_ptr->call_id;
      inband_info_ptr->duration = cmd_info_ptr->duration;
      inband_event = CM_INBAND_EVENT_REV_STOP_CONT_DTMF;
      break;

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_SWITCH_ERR( "Bad inband command %d", inband_cmd_ptr->cmd, 0, 0);
      break;

  }

  /* indicate which client sending this request*/
  inband_info_ptr->client_id     = inband_cmd_ptr->client_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify the client list of the inband event.
  */
  cmclient_list_inband_event_ntfy( inband_event, inband_info_ptr );

}


#ifdef FEATURE_HANDLE_FWD_DTMF

/*===========================================================================

FUNCTION cminband_xlat_dtmf

DESCRIPTION
  This function converts BCD format DTMF digits received from MC
  to ASCII format.

DEPENDENCIES
  None

RETURN VALUE
  Translated ASCII character.

SIDE EFFECTS
  None

===========================================================================*/
static uint8 cminband_xlat_dtmf(
  uint8 bcd_dtmf
    /* BCD digit received from the MC */
)
{
  uint8 ascii_dtmf= ' ';
    /* DTMF digit in ASCII format */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (bcd_dtmf)
  {

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      ascii_dtmf = bcd_dtmf + '0';
      break;

    case 10:
      ascii_dtmf = '0';
      break;

    case 11:
      ascii_dtmf = '*';
      break;

    case 12:
      ascii_dtmf = '#';
      break;

    default:
      CM_ERR( "Illegal digit code %d", bcd_dtmf, 0, 0 );
      break;

  }

  return (ascii_dtmf);

}

#endif /* FEATURE_HANDLE_FWD_DTMF */

#if (defined FEATURE_HANDLE_FWD_DTMF) || (defined FEATURE_T53)

/*===========================================================================

FUNCTION cminband_fwd_event_ntfy

DESCRIPTION
  Notify the client list of a specified forward inband event.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cminband_fwd_event_ntfy
(
  const cm_mc_rpt_type  *cmd_ptr
)
{
  cm_inband_info_s_type     *inband_info_ptr  = cminband_info_alloc();
      /* Allocate inband info buffer */

  cm_inband_event_e_type    inband_event      = CM_INBAND_EVENT_NONE;

  int                       i;

  boolean                   digit_ok          = TRUE;
    /* Indicates whether BCD digit is legal or not */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy inband information into
  ** allocated buffer.
  */

  switch (cmd_ptr->hdr.cmd)
  {
    case CM_DTMF_BURST_F:
      inband_info_ptr->call_id    = cmd_ptr->dtmf.call_id;
      inband_info_ptr->on_length  = cmd_ptr->dtmf.dtmf_on;
      inband_info_ptr->off_length = cmd_ptr->dtmf.dtmf_off;
      inband_info_ptr->cnt        = cmd_ptr->dtmf.cnt;

      /* Translate 4 bit DTMF into corresponding ASCII format */
      for( i=0; i< cmd_ptr->dtmf.cnt; i++ )
      {
        inband_info_ptr->digits[i] =
        cminband_xlat_dtmf( cmd_ptr->dtmf.digits[i]);
        if ( inband_info_ptr->digits[i] == ' ' )
        {
          digit_ok = FALSE;
          break;
        }
      }

      inband_event = CM_INBAND_EVENT_FWD_BURST_DTMF;
      break;

    case CM_DTMF_CONT_START_F:
      inband_info_ptr->call_id   = cmd_ptr->dtmf_cont_start.call_id;
      inband_info_ptr->cnt       = 1;
      inband_info_ptr->digits[0] = cminband_xlat_dtmf(cmd_ptr->dtmf_cont_start.dtmf_code);
      if ( inband_info_ptr->digits[0] == ' ' )
      {
        digit_ok = FALSE;
        break;
      }
      inband_event = CM_INBAND_EVENT_FWD_START_CONT_DTMF;
      break;

    case CM_DTMF_CONT_STOP_F:
      inband_info_ptr->call_id   = cmd_ptr->dtmf_cont_stop.call_id;
      inband_event = CM_INBAND_EVENT_FWD_STOP_CONT_DTMF;
      break;

    #ifdef FEATURE_T53
    case CM_SND_BRST_DTMF_ACK_F:
      inband_info_ptr->call_id   = cmd_ptr->snd_brst_dtmf_ack.call_id;
      inband_event               = CM_INBAND_EVENT_REV_BURST_DTMF_ACK;
      break;
    #endif /* FEATURE_T53 */

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      CM_SWITCH_ERR( "Bad MC report %d", cmd_ptr->hdr.cmd, 0, 0);
      break;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify the client list of the inband event.
  */
  if ( digit_ok )
  {
    cmclient_list_inband_event_ntfy( inband_event, inband_info_ptr );
  }

}

#endif /* FEATURE_HANDLE_FWD_DTMF || FEATURE_T53 */




/*===========================================================================

FUNCTION cminband_client_cmd_proc

DESCRIPTION
  Process clients' inband commands.

DEPENDENCIES
  none
RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cminband_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
)
{
  cm_inband_cmd_err_e_type  inband_cmd_err;
  cm_call_mode_info_e_type  call_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cminband_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_INBAND );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  */
  inband_cmd_err = cminband_client_cmd_check( cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cminband_client_cmd_err( cmd_ptr, inband_cmd_err );
  if( inband_cmd_err != CM_INBAND_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine the current call mode
  */
  call_mode = cmcall_call_id_mode( cmd_ptr->cmd.inband.info.call_id );

  /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  switch ( call_mode )
  {
    #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case CM_CALL_MODE_INFO_CDMA:
      cminband_client_cmd_forward_mc( cmd_ptr );
      break;
    #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
    
    
    case CM_CALL_MODE_INFO_IP:
      cminband_client_cmd_forward_ip( cmd_ptr );
      break;
    

    default:
      CM_ERR_FATAL( "Invalid call mode (%d)", call_mode, 0, 0 );
      break;

  } /* switch ( call_mode ) */

  /* Notify the registered clients of a specified reverse inband event. */
  cminband_rev_event_ntfy(cmd_ptr);

} /* cminband_client_cmd_proc() */


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */



#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION  check_dtmf_on_length
DESCRIPTION
  Check wther the DTMF on length is in valid range

DEPENDENCIES
  none
RETURN VALUE
  CM_INBAND_CMD_ERR_NONE if no parameter related errors,
  otherwise CM_INBAND_CMD_ERR_ON_LEN_P is returned

SIDE EFFECTS
  none

===========================================================================*/
static cm_inband_cmd_err_e_type  check_dtmf_on_length
(
  uint8 on_length
)
{
  cm_inband_cmd_err_e_type   cmd_err = CM_INBAND_CMD_ERR_NOERR;

  /*-----------------------------------------------------------------------*/

  switch (on_length)
  {
    case CAI_DTMF_ON_95:
    case CAI_DTMF_ON_150:
    case CAI_DTMF_ON_200:
    case CAI_DTMF_ON_250:
    case CAI_DTMF_ON_300:
    case CAI_DTMF_ON_350:
    #ifdef FEATURE_JCDMA
    case CAI_DTMF_ON_SMS:
    #endif
      break;
    default:
      CM_ERR("Bad on_length = %d",on_length,0,0);
      cmd_err = CM_INBAND_CMD_ERR_ON_LEN_P;
  } /* switch (on_length) */

  return cmd_err;

} /* check_dtmf_on_length() */



/*===========================================================================

FUNCTION  check_dtmf_off_length
DESCRIPTION
  Check whether the DTMF inter-digit interval is in valid range

DEPENDENCIES
  none
RETURN VALUE
  CM_INBAND_CMD_ERR_NONE if no parameter related errors,
  otherwise CM_INBAND_CMD_ERR_OFF_LEN_P is returned

SIDE EFFECTS
  none

===========================================================================*/
static cm_inband_cmd_err_e_type  check_dtmf_off_length
(
  uint8 off_length
)
{
  cm_inband_cmd_err_e_type   cmd_err = CM_INBAND_CMD_ERR_NOERR;

  /*-----------------------------------------------------------------------*/

  switch (off_length)
  {
    case CAI_DTMF_OFF_60:
    case CAI_DTMF_OFF_100:
    case CAI_DTMF_OFF_150:
    case CAI_DTMF_OFF_200:
      break;
    default:
      CM_ERR("Bad off_length = %d",off_length,0,0);
      cmd_err = CM_INBAND_CMD_ERR_OFF_LEN_P;
  } /* switch (off_length) */

  return cmd_err;

} /* check_dtmf_off_length() */



/*===========================================================================

FUNCTION cminband_client_cmd_forward_mc

DESCRIPTION
  Forward a client inband commands to MC.

DEPENDENCIES
  none

RETURN VALUE
  None
SIDE EFFECTS
  none

===========================================================================*/
static void cminband_client_cmd_forward_mc(

    const cm_cmd_s_type    *cmd_ptr
        /* Pointer to a CM command */
)
{
  const cm_inband_cmd_s_type        *inband_cmd_ptr = NULL;
      /* Point at inband command component */

  const cm_inband_cmd_info_s_type   *cmd_info_ptr   = NULL;
      /* Point at command information component */

  const cmss_s_type                 *ss_ptr         = cmss_ptr();
     /* Point at serving system object */

  mc_msg_type                       *mc_ptr;
      /* MC command pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_INBAND );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inband_cmd_ptr = INBAND_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cminband_client_cmd_forward_mc(),cmd=%d",inband_cmd_ptr->cmd,0,0);
  cmd_info_ptr   = CMD_INFO_PTR( inband_cmd_ptr );

  /* Only forward CDMA related DTMF messages to MC 
  */
  if( !cm_is_sys_mode_digital( ss_ptr->info.sys_mode ))
  {
      return;
  }

  /* Get MC command buffer.
  */
  mc_ptr = cm_mc_get_buf_else_err_fatal();


  switch( inband_cmd_ptr->cmd )
  {

    case CM_INBAND_CMD_BURST_DTMF:
      mc_ptr->hdr.cmd               = MC_BURST_DTMF_F;
      mc_ptr->burst_dtmf.call_id    = cmd_info_ptr->call_id;
      mc_ptr->burst_dtmf.on_length  = cmd_info_ptr->on_length;
      mc_ptr->burst_dtmf.off_length = cmd_info_ptr->off_length;
      mc_ptr->burst_dtmf.cnt        = cmd_info_ptr->cnt;
      memcpy(mc_ptr->burst_dtmf.digits, cmd_info_ptr->digits, cmd_info_ptr->cnt);
      break;

    case CM_INBAND_CMD_START_CONT_DTMF:
      mc_ptr->hdr.cmd                 = MC_START_CONT_DTMF_F;
      mc_ptr->start_cont_dtmf.call_id = cmd_info_ptr->call_id;
      mc_ptr->start_cont_dtmf.digit   = cmd_info_ptr->digits[0];
      break;

    case CM_INBAND_CMD_STOP_CONT_DTMF:
      mc_ptr->hdr.cmd                = MC_STOP_CONT_DTMF_F;
      mc_ptr->stop_cont_dtmf.call_id = cmd_info_ptr->call_id;
      mc_ptr->stop_cont_dtmf.ms      = cmd_info_ptr->duration;
      break;


    default:

      CM_SWITCH_ERR( "Bad inband command %d", inband_cmd_ptr->cmd, 0, 0);
      break;
  }
  /* forward commnad to MC */
  cm_mc_send_cmd( mc_ptr );

}



/*===========================================================================

FUNCTION cminband_mc_rpt_proc

DESCRIPTION
  Process MC inband related reports

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cminband_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{

  /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
  cm_mc_rpt_type    *cm_mc_rpt_ptr = (cm_mc_rpt_type *) rpt_ptr;
    /* Pointer to MC report */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_mc_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cminband_mc_rpt_proc(), cmd=%d",
                  cm_mc_rpt_ptr->hdr.cmd, 0, 0);

  switch( cm_mc_rpt_ptr->hdr.cmd )
  {
    #ifdef FEATURE_HANDLE_FWD_DTMF
    case CM_DTMF_BURST_F:
    case CM_DTMF_CONT_START_F:
    case CM_DTMF_CONT_STOP_F:
      cminband_fwd_event_ntfy( cm_mc_rpt_ptr );
      break;
    #endif

    #ifdef FEATURE_T53
    case CM_SND_BRST_DTMF_ACK_F:
      cminband_fwd_event_ntfy( cm_mc_rpt_ptr );
      break;
    #endif /* FEATURE_T53 */

    default:
      break;
  } /* switch( cm_mc_rpt_ptr->hdr.cmd ) */
} /* cminband_mc_rpt_proc() */
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


/*===========================================================================

FUNCTION cminband_client_cmd_forward_ip

DESCRIPTION
  Forward client inband commands to ip app.

DEPENDENCIES
  none

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/
static void cminband_client_cmd_forward_ip(

    const cm_cmd_s_type    *cmd_ptr
        /* Pointer to a CM command */
)
{
  const cm_inband_cmd_s_type        *inband_cmd_ptr = NULL;
      /* Point at inband command component */

  const cm_inband_cmd_info_s_type   *cmd_info_ptr   = NULL;
      /* Point at command information component */

  const cmss_s_type                 *ss_ptr         = cmss_ptr();
     /* Point at serving system object */    

  cmcall_s_type                     *call_ptr;
    /* Pointer to a call object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_INBAND );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  inband_cmd_ptr = INBAND_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cminband_client_cmd_forward_ip(), cmd=%d",inband_cmd_ptr->cmd,0,0);

  cmd_info_ptr   = CMD_INFO_PTR( inband_cmd_ptr );

  /* Check whether this call id is allocated */
  call_ptr = cmcall_ptr( cmd_info_ptr->call_id );  

  if(call_ptr == NULL)
  {
    CM_ERR( "CM_INBAND_CMD_ERR_CALL_ID_P: %d", cmd_info_ptr->call_id,0,0 );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  

  /* This check is to avoid sending dtmf on non-digital systems like
  ** AMPS. Except MAIN instance other instances always acquire a digital
  ** system (HYBR_HDR - HDR, HYBR_WLAN - WLAN)
  */
  if( (call_ptr->ss == CM_SS_MAIN) && 
      !cm_is_sys_mode_digital( ss_ptr->info.sys_mode )
    )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  #if defined(FEATURE_IP_CALL)

  switch( inband_cmd_ptr->cmd )
  {

    case CM_INBAND_CMD_START_CONT_DTMF:
      cmipcall_start_cont_dtmf (call_ptr, cmd_info_ptr->digits[0]);      
      break;

    case CM_INBAND_CMD_STOP_CONT_DTMF:
      cmipcall_stop_cont_dtmf  (call_ptr, cmd_info_ptr->duration);      
      break;

    default:
      CM_SWITCH_ERR( "Bad inband command %d", inband_cmd_ptr->cmd, 0, 0);
      break;
  }

  #endif

} /* cminband_client_cmd_forward_ip () */

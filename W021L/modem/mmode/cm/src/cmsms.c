/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R  S M S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager SMS Object.

EXTERNALIZED FUNCTIONS

  cmsms_ptr - Return a pointer to the one and only SMS object.

  cmsms_init - Initializing the SMS object.

  cmsms_client_cmd_proc -Process clients' SMS commands.

  cmsms_get_sms_data_buffer - Return the pointer to MO SMS buffer

  cmxsms_sms_data_buffer_fill - Fill the MO SMS buffer with specified data.

  cmsms_event - Notified registered clients the SMS event.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1999 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmsms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/14/07   sk/ak   Added DCM
08/07/07   ic      Lint clean up
06/22/07   sk      DDTM rewrite.
03/07/07   jqi     Initial release.



===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#include "cm.h"        /* External interface to cm.c */
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmcall.h"    /* Interface to CM call object */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "cmxsms.h"    /* Interface to cmsms.c*/
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
#include "cmsms.h"    /* Interface to cmsms.c*/

#ifdef FEATURE_HDR_HYBRID
#error code not present
#endif

#ifdef FEATURE_SMS_OVER_IP
#include "cmipsms.h"
#endif

/* Static global memory storage for SMS messages shared by CP.
** Buffer to hold MO SMS sent over CDMA or IP.
*/
static cm_sms_msg_type cmsms_wms_msg;

/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cmsms_ptr

DESCRIPTION
  Return a pointer to the one and only SMS object.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmsms_s_type  *cmsms_ptr( void )
{

  static cmsms_s_type cmsms;
    /* The one and only SMS object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return &cmsms;
}


/*===========================================================================

FUNCTION cmsms_init

DESCRIPTION
  Initializing the SMS object.

  This function must be called before the SMS object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_init( void )
{

  cmsms_s_type    *sms_ptr  =  cmsms_ptr();
      /* Point at sms object to be initialized */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_FUNC_START( "cmsms_init()",0,0,0 );

  /* Verify object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( ! CM_INIT_CHECK(sms_ptr->init_mask) );
  CM_INIT_MARK( sms_ptr->init_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CDSMS
  sms_ptr->sms_state                  = CM_SMS_STATE_IDLE;
  sms_ptr->sms_client_ptr             = CM_CLIENT_PTR_NONE;
  sms_ptr->l2_ack_requested           = FALSE;
  sms_ptr->awi_ack.seq_no             = 0;
  sms_ptr->awi_ack.bf                 = 0;
  sms_ptr->awi_ack.error_class        = 0;
  sms_ptr->awi_ack.status             = 0;
  #endif
  sms_ptr->data                       = NULL;
  sms_ptr->mo_status                  = CM_SMS_MO_NONE;
  sms_ptr->mt_sms_ptr                 = NULL;
}


/*===========================================================================

FUNCTION  cmsms_client_cmd_check

DESCRIPTION
  Check for sms command parameter errors and whether a specified SMS command
  is allowed in the current state of the sms/call/phone.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  CM_SMS_CMD_ERR_NONE if command is allowed in the current state
  of the sms/call/phone, specific cm_sms_cmd_err_e_type otherwise.

SIDE EFFECTS
  none
===========================================================================*/
static cm_sms_cmd_err_e_type  cmsms_client_cmd_check(

    const cmsms_s_type     *sms_ptr,
        /* Pointer to a sms object */

    cm_cmd_s_type          *cmd_ptr
        /* Pointer to a CM command */
)
/*lint -esym(715,sms_ptr) */
/*lint -esym(818,cmd_ptr) */
{
  cm_sms_cmd_s_type       *sms_cmd_ptr  = NULL;
      /* Point at sms command component */

  cm_sms_cmd_info_s_type  *cmd_info_ptr = NULL;
      /* Point at command information component */

  cmph_s_type               *ph_ptr     = cmph_ptr();
      /* Pointer for phone object */

  cmss_s_type               *ss_ptr     = cmss_ptr();
      /* Pointer for ss object */

  cm_sms_cmd_err_e_type   cmd_err       = CM_SMS_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SMS );
  CM_ASSERT( ph_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sms_cmd_ptr  = SMS_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR( sms_cmd_ptr );
  CM_MSG_HIGH( "cmsms_cmd_check, cmd=%d", sms_cmd_ptr->cmd, 0, 0);

  /* check cmd is already in error or not */

  if (cmd_info_ptr->cmd_err != CM_SMS_CMD_ERR_NOERR)
  {
    return cmd_info_ptr->cmd_err;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */
  switch( sms_cmd_ptr->cmd )
  {
                         /*-------------------------*/
                         /* Mobile Originated SMS   */
                         /*-------------------------*/

    case CM_SMS_CMD_MO_MSG:

      /* The mode pref over which the SMS is sent.
      */
      switch( cmd_info_ptr->mode_pref )
      {
        case CM_MODE_PREF_CDMA_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_CDMA;

        break;

        case CM_MODE_PREF_HDR_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_HDR;

        break;

        case CM_MODE_PREF_WLAN_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_WLAN;

          break;

        case CM_MODE_PREF_WCDMA_ONLY:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_WCDMA;

          break;

        default:

          cmd_info_ptr->sys_mode = SYS_SYS_MODE_NONE;

          CM_MSG_HIGH(" MO SMS mode pref 0x%x ", cmd_info_ptr->mode_pref, 0, 0 );

          cmd_err = CM_SMS_CMD_ERR_MODE_PREF;

          CM_ERR( "CM_SMS_CMD_ERR_MODE_PREF",0,0,0 );

          return cmd_err;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_SMS_OVER_IP

      cmd_err =  cmipsms_client_cmd_check( cmd_info_ptr );

      if( cmd_err != CM_SMS_CMD_ERR_NOERR )
      {
        return cmd_err;
      }

      #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_SMS_CMD_ERR_OFFLINE_S;
        CM_ERR( "CM_SMS_CMD_ERR_OFFLINE_S",0,0,0 );
      }
      else if( cmph_is_in_emergency_cb() )
      {
        /* phone is in E911 callback mode only E911 call is allowed */
        cmd_err = CM_SMS_CMD_ERR_EMERGENCY_MODE_S;
        CM_ERR( "CM_SMS_CMD_ERR_EMERGENCY_MODE_S",0,0,0 );

      }
      else if( ph_ptr->cdma_lock_mode == CM_CDMA_LOCK_MODE_ON )
      {
        /* CDMA lock is On */
        cmd_err = CM_SMS_CMD_ERR_CDMA_LOCK_S;
        CM_ERR( "CM_SMS_CMD_ERR_CDMA_LOCK_S",0,0,0 );
      }
      #ifdef FEATURE_CDSMS
      else if (sms_ptr->sms_state != CM_SMS_STATE_IDLE)
      {
        /* SMS state is not idle */
        cmd_err = CM_SMS_CMD_ERR_NOT_IDLE_S;
        CM_ERR( "CM_SMS_CMD_ERR_NOT_IDLE_S",0,0,0 );
      }
      #endif
      #ifdef FEATURE_JCDMA
      /* Check if a DO call is in origination or connected, if it is then
      ** if the ddtm mask is set to block MO DBM return error
      */
      /*lint -e655 */
      /* 655: bit-wise operation uses (compatible) enum's */
      else if( ( cmcall_count_on_ss(CM_SS_HDR) != 0 ) && 
                ( ( ph_ptr->cur_ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM ) ||
                  ( ( ph_ptr->ddtm_pref == CM_DDTM_PREF_ON )&& 
                    ( ph_ptr->ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_MO_DBM )))
             )
      {
        cmd_err = CM_SMS_CMD_ERR_DDTM;
        CM_ERR( "CM_SMS_CMD_ERR_DDTM",0,0,0 );
      }
      /*lint +e655 */
      #endif
      else
      {
        if( ( !ss_ptr->info.hdr_hybrid ||
              ss_ptr->info.hdr_srv_status != SYS_SRV_STATUS_SRV ) &&
            ( ss_ptr->info.wlan_srv_status != SYS_SRV_STATUS_SRV ) &&
            (ss_ptr->info.sys_mode == SYS_SYS_MODE_NO_SRV))
        {
          cmd_err = CM_SMS_CMD_ERR_NO_SRV_S;
          CM_ERR( "CM_SMS_CMD_ERR_NO_SRV_S",0,0,0 );
        }
        else if (ss_ptr->info.sys_mode == SYS_SYS_MODE_AMPS)
        {
          cmd_err = CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S;
          CM_ERR( "CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S",0,0,0 );
        }
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SMS_CMD_AWI_ACK:

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        cmd_err = CM_SMS_CMD_ERR_OFFLINE_S;
        CM_ERR( "CM_SMS_CMD_ERR_OFFLINE_S",0,0,0 );
      }
      else
      {
        if( ss_ptr->info.sys_mode != SYS_SYS_MODE_AMPS )
        {
          cmd_err = CM_SMS_CMD_ERR_NOT_ANALOG_S;
          CM_ERR( "CM_SMS_CMD_ERR_NOT_ANALOG_S",0,0,0 );
        }
      }
      break;

      #ifdef FEATURE_BROADCAST_SMS

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /*-------------------------*/
                      /* Enable broadcast SMS    */
                      /*-------------------------*/

      case CM_SMS_CMD_BC_ENABLE:

      if( sms_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      else if( (sms_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
      {
        /* Invalid client ID */
        cmd_err  = CM_SMS_CMD_ERR_CLIENT_ID_P;
        CM_DBG_ERR_FATAL( "CM_SMS_CMD_ERR_CLIENT_ID_P",0,0,0 );
      }
      break;

      #endif /* FEATURE_BROADCAST_SMS */
  
    default:
      break;
  }

  return cmd_err;
}


/*===========================================================================

FUNCTION cmsms_client_cmd_err

DESCRIPTION
  Notify clients of a specified sms command error.

DEPENDENCIES
  SMS object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_client_cmd_err(

    const cm_cmd_s_type       *cmd_ptr,
        /* Pointer to a CM command */

    cm_sms_cmd_err_e_type      cmd_err
        /* Indicate the call command error */
)
{

  const cm_sms_cmd_s_type    *sms_cmd_ptr = NULL;
    /* Point at SMS command component */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SMS );
  CM_ASSERT( BETWEEN( cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sms_cmd_ptr = SMS_CMD_PTR( cmd_ptr );
  CM_FUNC_START( "cmsms_client_cmd_err() sms_cmd=%d, cmd_err=%d, client=%p",
                 sms_cmd_ptr->cmd, cmd_err, sms_cmd_ptr->client_ptr );

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( sms_cmd_ptr->cmd_cb_func != NULL )
  {
    sms_cmd_ptr->cmd_cb_func( sms_cmd_ptr->data_block_ptr,
                              sms_cmd_ptr->cmd,
                              cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_SMS_CMD_ERR_NOERR )
  {
    CM_ERR( "sms cmd err, cmd=%d, err=%d, client=%ld",
            sms_cmd_ptr->cmd, cmd_err, sms_cmd_ptr->client_id );

    cmclient_list_sms_cmd_err_ntfy( cmd_err, sms_cmd_ptr );
  }

}



/*===========================================================================

FUNCTION cmsms_client_cmd_forward_ll

DESCRIPTION
  Forward a client SMS commands to the protocal stack.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from the protocal statck before 
  continuing processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsms_client_cmd_forward_ll(

    const cmsms_s_type  *sms_ptr,
        /* Pointer to a sms object */

    const cm_cmd_s_type *cmd_ptr
        /* Pointer to a CM command */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sms_ptr == NULL || cmd_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
  
  return (cmxsms_client_cmd_forward_mc( sms_ptr, cmd_ptr ));

  #else

  return FALSE;

  #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

}

/*===========================================================================

FUNCTION cmsms_info_alloc

DESCRIPTION
  Allocate a sms info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sms info struct.

SIDE EFFECTS
  none

===========================================================================*/
static cm_sms_info_s_type  *cmsms_info_alloc( void )
{
  static cm_sms_info_s_type  sms_info;
      /* Currently there is only a need for one sms info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simple return a pointer to the statically allocated
  ** sms info buffer.
  */
  return &sms_info;
}


/*===========================================================================

FUNCTION cmsms_info_get

DESCRIPTION
  Copy the current sms state information into a specified buffer.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  TRUE if sms information is valid, FALSE otherwise.

  Note that sms information only becomes valid after cmsms_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsms_info_get(

    cm_sms_info_s_type    *sms_info_ptr
        /* Copy sms state info into this buffer */
)
{

  cmsms_s_type          *sms_ptr  =  cmsms_ptr();
      /* Point at sms object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_info_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(sms_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy sms parameters into sms_info struct.
  */
  #ifdef FEATURE_CDSMS
  sms_info_ptr->sms_state             = sms_ptr->sms_state;
  sms_info_ptr->sms_client_id         = cmclient_get_client_id (sms_ptr->sms_client_ptr );
  sms_info_ptr->awi_ack.bf            = sms_ptr->awi_ack.bf;
  sms_info_ptr->awi_ack.error_class   = sms_ptr->awi_ack.error_class;
  sms_info_ptr->awi_ack.seq_no        = sms_ptr->awi_ack.seq_no;
  sms_info_ptr->awi_ack.status        = sms_ptr->awi_ack.status;
  sms_info_ptr->l2_ack_requested      = sms_ptr->l2_ack_requested;
  #endif
  sms_info_ptr->mo_status             = sms_ptr->mo_status;

  if( sms_ptr->mt_sms_ptr != NULL )
  {
    sms_info_ptr->mt_sms_format     = (uint8)sms_ptr->mt_sms_ptr->sms.format;
    sms_info_ptr->length            = sms_ptr->mt_sms_ptr->sms.data_len;
    sms_info_ptr->data              = sms_ptr->mt_sms_ptr->sms.data;
    sms_info_ptr->sys_mode          = sms_ptr->mt_sms_ptr->sys_mode;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return boolean indicating whether sms information is valid.
  */
  return BOOLEAN( CM_INIT_CHECK(sms_ptr->init_mask) );
}

/*===========================================================================

FUNCTION cmsms_event

DESCRIPTION
  Notify the client list of a specified sms event.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_event(

    const cmsms_s_type     *sms_ptr,
        /* pointer to a sms object.  */

    cm_sms_event_e_type    sms_event
        /* notify client list of this sms event */
)
/*lint -esym(715,sms_ptr)*/
{
  cm_sms_info_s_type *sms_info_ptr = cmsms_info_alloc();
      /* Allocate a sms info buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( BETWEEN(sms_event, CM_SMS_EVENT_NONE, CM_SMS_EVENT_MAX) );
  CM_ASSERT( sms_info_ptr != NULL );
  CM_MSG_MED( "sms event=%d",sms_event,0,0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy sms information into
  ** allocated sms info buffer.
  */
  (void) cmsms_info_get( sms_info_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify clients list of sms event.
  */
  cmclient_list_sms_event_ntfy( sms_event, sms_info_ptr );

}
/*lint +esym(715,sms_ptr)*/


/*===========================================================================

FUNCTION cmsms_client_cmd_ll_reply

DESCRIPTION
  Process MC reply to clients sms commands.

DEPENDENCIES
  sms object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsms_client_cmd_ll_reply(

    cmsms_s_type              *sms_ptr,
        /* Pointer to a sms object */

    cm_cmd_s_type             *cmd_ptr, 
        /* Pointer to a CM command */

    cm_sms_cmd_err_e_type     cmd_err
        /* Indicate sms command error */
)
/*lint -esym(715,sms_ptr) */
/*lint -esym(818,sms_ptr) */
/*lint -esym(715,cmd_err) */
/*lint -esym(818,cmd_err) */
{
  cm_sms_cmd_s_type          *sms_cmd_ptr  = NULL; 
    /* Point at sms command component */
  cm_sms_cmd_info_s_type     *cmd_info_ptr = NULL; 
    /* sms command information pointer */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_SMS_CMD_ERR_NONE, CM_SMS_CMD_ERR_MAX));
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SMS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sms_cmd_ptr  = SMS_CMD_PTR( cmd_ptr );
  cmd_info_ptr = CMD_INFO_PTR(sms_cmd_ptr);
  CM_FUNC_START( "cmsms_client_cmd_ll_reply, cmd=%d, err=%d",
                 sms_cmd_ptr->cmd, 0, 0);

  /*
  ** 1. Update sms fields
  ** 2. Notify clients of sms event
  */

    switch( sms_cmd_ptr->cmd )
    {
                       /*-------------------------*/
                       /*           MO SMS        */
                       /*-------------------------*/

      case CM_SMS_CMD_MO_MSG:

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        switch( cmd_info_ptr->sys_mode)
        {
          case SYS_SYS_MODE_CDMA:

            #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

            /* Send MO SMS over CDMA protocol
            */

            /* update SMS state and copy relevant info to SMS object 
            */

            sms_ptr->sms_client_ptr     = sms_cmd_ptr->client_ptr;
            sms_ptr->sms_state          = CM_SMS_STATE_HOLD_ORIG_RETRY;
            sms_ptr->data               = cmd_info_ptr->data;
            sms_ptr->l2_ack_requested   = cmd_info_ptr->l2_ack_requested;
            sms_ptr->is_ok_to_send      = TRUE;


            cmxsms_mo_msg_start( sms_ptr );

            #else
            
            CM_CASE_ERR( "MO SMS invalid sys_mode=%d", 
                                            cmd_info_ptr->sys_mode, 0, 0 );
            #endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SYS_SYS_MODE_HDR:
          case SYS_SYS_MODE_WLAN:

            #ifdef FEATURE_SMS_OVER_IP
            /* Send MO SMS over IP APP
            */

            cmipsms_send_mo_sms_request( cmd_info_ptr->app_id,
                                         cmd_info_ptr->data,
                                         cmd_info_ptr->sys_mode );

            /* Notify client list of MO SMS event.
            */
            cmsms_event( sms_ptr, CM_SMS_EVENT_MO_MSG );

            break;

            #endif

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SYS_SYS_MODE_NONE:
          case SYS_SYS_MODE_NO_SRV:
          case SYS_SYS_MODE_AMPS:
          case SYS_SYS_MODE_GSM:
          case SYS_SYS_MODE_WCDMA:
          case SYS_SYS_MODE_GPS:
          case SYS_SYS_MODE_GW:
          case SYS_SYS_MODE_MAX:
          default:
            break;
        }

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                       /* Answer an incoming sms */
                       /*-------------------------*/
       #ifdef FEATURE_CDSMS

       case CM_SMS_CMD_AWI_ACK:

        /* update SMS state and copy relevant info to SMS object */
        sms_ptr->sms_client_ptr         = sms_cmd_ptr->client_ptr;
        sms_ptr->awi_ack.bf             = cmd_info_ptr->awi_ack.bf;
        sms_ptr->awi_ack.error_class    = cmd_info_ptr->awi_ack.error_class;
        sms_ptr->awi_ack.status         = cmd_info_ptr->awi_ack.status;
        sms_ptr->awi_ack.seq_no         = cmd_info_ptr->awi_ack.seq_no;

        cmsms_event( sms_ptr, CM_SMS_EVENT_AWI_ACK );

        break;

       #ifdef FEATURE_BROADCAST_SMS

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                       /* Enable broadcast SMS    */
                       /*-------------------------*/

       case CM_SMS_CMD_BC_ENABLE:

        /* update SMS state and copy relevant info to SMS object */
        sms_ptr->sms_client_ptr         = sms_cmd_ptr->client_ptr;
        sms_ptr->bc_enable              = cmd_info_ptr->bc_enable;

        cmsms_event( sms_ptr, CM_SMS_EVENT_BC_ENABLED );

        break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #endif /* FEATURE_BROADCAST_SMS */
      #endif /* FEATURE_CDSMS */

      default:

        CM_SWITCH_ERR( "Bad sms command %d", sms_cmd_ptr->cmd, 0, 0);

        break;

    } /* switch */

}
/*lint +esym(715,cmd_err) */
/*lint +esym(715,sms_ptr) */
/*lint +esym(818,cmd_err) */
/*lint +esym(818,sms_ptr) */



/*===========================================================================

FUNCTION cmsms_client_cmd_proc

DESCRIPTION
  Process clients' SMS commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmsms_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsms_client_cmd_proc(

    cm_cmd_s_type  *cmd_ptr
        /* pointer to a CM command */
)
{

  cmsms_s_type             *sms_ptr       = cmsms_ptr();
      /* Point at phone object */

  cm_sms_cmd_err_e_type     sms_cmd_err;

  boolean wait_for_reply;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_FUNC_START( "cmsms_client_cmd_proc()",0,0,0 );

  CM_ASSERT( cmd_ptr != NULL );
  CM_ASSERT( sms_ptr != NULL );
  CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SMS );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start by checking whether this is a valid command
  ** relative to the current state of the phone.
  */
  sms_cmd_err = cmsms_client_cmd_check( sms_ptr, cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmsms_client_cmd_err( cmd_ptr, sms_cmd_err );
  if( sms_cmd_err != CM_SMS_CMD_ERR_NOERR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** so forward the client command to protocal stacks.
  */
  wait_for_reply = cmsms_client_cmd_forward_ll( sms_ptr, cmd_ptr );

  /* If NO need to wait for replay from MC,
  ** call the reply function right a way.
  */
  if( ! wait_for_reply )
  {
    cmsms_client_cmd_ll_reply( sms_ptr,
                               cmd_ptr,
                               CM_SMS_CMD_ERR_NOERR );
  }

}


/*===========================================================================

FUNCTION cmsms_get_sms_data_buffer

DESCRIPTION
 Returns a pointer to the static global location to store outgoing SMS msgs.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to cm_sms_msg_type.

SIDE EFFECTS
  Nothing.

===========================================================================*/
cm_sms_msg_type *cmsms_get_sms_data_buffer()
{

   return &cmsms_wms_msg;

}  /* cmxsms_get_sms_data_buffer */

/*===========================================================================

FUNCTION cmxsms_sms_data_buffer_fill

DESCRIPTION
 Fill in the static global location for outgoing SMS msgs.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmsms_sms_data_buffer_fill
(
  const cm_sms_msg_type*    sms_msg
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sms_msg           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmsms_wms_msg.address.digit_mode = sms_msg->address.digit_mode;
  cmsms_wms_msg.address.number_of_digits = MIN( sms_msg->address.number_of_digits, 
                                                      CM_SMS_ADDRESS_MAX );
  memcpy (cmsms_wms_msg.address.digits,
          sms_msg->address.digits,
          cmsms_wms_msg.address.number_of_digits);
  cmsms_wms_msg.address.number_mode = sms_msg->address.number_mode;
  cmsms_wms_msg.address.number_plan = sms_msg->address.number_plan;
  cmsms_wms_msg.address.number_type = sms_msg->address.number_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmsms_wms_msg.ota.data_len = MIN( sms_msg->ota.data_len, CM_SMS_MAX_LEN );

  memcpy (cmsms_wms_msg.ota.data,
          sms_msg->ota.data,
          cmsms_wms_msg.ota.data_len);

  cmsms_wms_msg.ota.format = sms_msg->ota.format;
}  /* cmsms_sms_data_buffer_fill */


/*===========================================================================
FUNCTION cmsms_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsms_dem_is_apps_wakeup_from_suspend_event(

    cm_sms_event_e_type   sms_event
        /* Call event */
)
/*lint -esym(715, sms_event)*/  
{
  #if !(defined (FEATURE_APPS_POWER_COLLAPSE) && \
       (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)))
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else
#error code not present
  #endif
} /* cmsms_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, sms_event)*/  


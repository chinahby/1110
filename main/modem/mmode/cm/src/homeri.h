#ifndef HOMERI_H
#define HOMERI_H
/*===========================================================================

                      H A N D O F F   M A N A G E R
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the message definitions necessary for
  handoff between different protocols. for Internal use of CM only
  
Copyright (c) 2006 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/homeri.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Removed EXTERN define & moved MMEXTN define to cmi.h
12/07/07   ra      Added support for IMS VCC in HOMER files 
06/11/07   pk      VoIP -> 1x hho enahncements / CR fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
11/16/06   pk      Lint Fixes
10/10/06   pk      Initial Revision

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "cm.h"
#include "cmi.h"
#include "cmdbg.h"
#include "sys.h"

#ifdef  FEATURE_HDR_CSNA
#error code not present
#endif


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_UMTS_1X_HANDOVER 
#error code not present
#endif


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Homer state
*/
typedef enum homer_state_e {

  HOMER_STATE_NONE = -1,                /* INTERNAL CM use ONLY             */

  HOMER_STATE_VOIP_HDR_1X_NONE,         /* Not in VoIP on HDR to 1X handoff */

  HOMER_STATE_VOIP_HDR_1X_WAIT_SRDM,    /* Waiting for SRDM from HDR        */

  HOMER_STATE_VOIP_HDR_1X_WAIT_MC_ORIG, /* Waiting for MC origination       */

  HOMER_STATE_VOIP_HDR_1X_WAIT_UHDM,    /* Waiting for UHDM from HDR        */

  HOMER_STATE_VOIP_HDR_1X_WAIT_VOIP_CALL_END, /* Waiting for VoIP call end from CP */

  HOMER_STATE_VOIP_HDR_1X_WAIT_VOIP_CALL_END_CNF, /* Waiting for VoIP call end from CP */

  HOMER_STATE_VOIP_HDR_1X_WAIT_1X_CALL_CONNECT,/* Waiting for confirmation from MC */

  HOMER_STATE_VOIP_HDR_1X_MAX,          /* INTERNAL CM use ONLY             */

  HOMER_STATE_UMTS_1X_NONE,             /* Not in UMTS 1X handover          */

  HOMER_STATE_UMTS_1X_WAIT_PSEUDO_ONLINE,
                                        /* Waiting for 1X to be pseudo 
                                        ** online                           */

  HOMER_STATE_UMTS_1X_WAIT_HANDOFF_REQ, /* Wait for handoff req. from UMTS  */

  HOMER_STATE_UMTS_1X_HANDOFF_RETRY,    /* Re-dialing the call on 1x        */

  HOMER_STATE_UMTS_1X_WAIT_HANDOFF_END, /* Wait for handoff to end          */

  HOMER_STATE_MAX                       /* INTERNAL CM use ONLY             */


} homer_state_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration that describes whether the event (command or report) was
** processed to completion or not.
*/
typedef enum {

  HOMER_EVT_STATUS_CONSUMED,            /* The event was processed to
                                       ** completion.
                                       */

  HOMER_EVT_STATUS_NOT_CONSUMED,        /* The event was not processed, the
                                       ** CMHM should call the transaction
                                       ** handlers again.
                                       */

  HOMER_EVT_STATUS_MAX                  /* For internal use. */

} homer_evt_status_e_type;


/*----------------------------------------------------------------------*/

/** Enum that defines the handoff call ids   */
typedef enum {

    HOMER_SWITCH_OUT_CALL_ID,
    /**< Switch out call id*/

    HOMER_SWITCH_IN_CALL_ID,
    /**< Switch in call id*/

    HOMER_CALL_ID_MAX
    /**< @Internal use only*/

} homer_handoff_call_id_e_type;



/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Homer state
*/
typedef struct homer_s {

  homer_state_e_type state;
    /* The current state of the handoff manager
    */

  cm_call_id_type   call_id;
    /**< The call is associated with the call.  
    ** Also denotes switch_out call ID from HOM  
    */
   
  cm_call_id_type   switch_in_call_id;
    /* switch_in call ID from HOM  
    */

  boolean           ho_req_rcvd;
    /* Handoff request received
    */

  boolean           end_cnf_rvcd;
    /* Call end confirmation already received
    */

  #ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
  #endif



} homer_s_type;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef  FEATURE_HDR_CSNA
#error code not present
#endif

/*===========================================================================

@FUNCTION homer_umts_1x_handover_proc

@DESCRIPTION
  This function handles the Traffic handover for UMTS to 1X.
  In this state, the state handler will

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern  void                 homer_umts_1x_handover_proc(

        cmcall_s_type        *call_ptr,
            /* The pointer to the call object
            */

        cm_call_event_e_type  call_event,
            /* The call event that generated this request
            */

        cm_ss_event_e_type    ss_event
            /* The SS event that generated this request
            */
);


/*===========================================================================

FUNCTION homer_get_state

DESCRIPTION
  Retruns the current homer state
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  homer_state_e_type   homer_get_state( void );


/*===========================================================================

FUNCTION homer_init

DESCRIPTION
  Initializes HOMER
DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void                 homer_init( void );


/*===========================================================================

@FUNCTION homer_update_handoff_info

@DESCRIPTION
  Updates or initailizes the structure containing IMS HOM handoff call info.

  
@DEPENDENCIES
  none

@RETURN VALUE
  none

@SIDE EFFECTS
  none

@x
===========================================================================*/


extern void homer_update_handoff_info(

    cm_call_id_type    switch_out_call_id,
    /**< Switch out call id
    */

    cm_call_id_type    switch_in_call_id
    /**< Switch in call id
    */

    );

/*===========================================================================

FUNCTION homer_chk_orig_abort_in_switch_in_domain

DESCRIPTION
  If the origination for switch in call failed and if switch out 
  domain call conditions improve, then abort origination process

  
DEPENDENCIES
  none

RETURN VALUE
  TRUE if the switch in call origination should be aborted
  FALSE if the switch in call origination should not be aborted

SIDE EFFECTS
  none

===========================================================================*/
extern boolean homer_chk_orig_abort_in_switch_in_domain(
                                
     cmcall_s_type    *call_ptr
     /**< pointer to a call object. */

     );

/*===========================================================================

@FUNCTION homer_chk_umts_wlan_handoff_complete

@DESCRIPTION
  To check if the wlan umts handover is complete.
  When complete, switch call objects and reset handoff info

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern void homer_chk_umts_wlan_handoff_complete(
        
     cm_call_id_type    call_id,
     /**< call id */

     cm_call_event_e_type    call_event
     /**< notify client list of this call event */

     );

/*===========================================================================

@FUNCTION homer_end_switch_in_call

@DESCRIPTION
  End the switch-in call

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/

extern void homer_end_switch_in_call(

    cmcall_s_type *call_ptr
    /**< Pointer to the call object*/

    );

/*===========================================================================

@FUNCTION homer_copy_call_info

@DESCRIPTION
  Copy the call info from switch out call pointer to switch in call pointer 
  if the switch out call is ending.

@DEPENDENCIES
  None.

@RETURN VALUE
 None.

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern void homer_copy_call_info(

    cmcall_s_type *call_ptr
    /**< pointer to a call object. */

    );

/*===========================================================================

@FUNCTION homer_get_connection_id

@DESCRIPTION
  Get connection id from call id.

@DEPENDENCIES
  None.

@RETURN VALUE
 connection id

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern byte homer_get_connection_id(

    homer_handoff_call_id_e_type call_id
    /**< call id */

    );

/*===========================================================================

@FUNCTION homer_chk_handover

@DESCRIPTION
  Checks if the handover is in progress or not.
  Used for UT only.
@DEPENDENCIES
  None.

@RETURN VALUE
 -TRUE if handover is going on
 -FALSE if handover is not going on

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern boolean homer_chk_handover(void);

/*===========================================================================

@FUNCTION homer_umts_wlan_handover_proc

@DESCRIPTION
  Processes call events related to handoff calls. Checks 
  - if the call events need to be suppressed
  - if handover is complete and call objects need to be swapped

@DEPENDENCIES
  None.

@RETURN VALUE
 -TRUE if other client should be notified about the call event
 -FALSE if the other clients should not be notified

@SIDE EFFECTS
  None.

@x
===========================================================================*/
extern boolean homer_umts_wlan_handover_proc(
            
    cmcall_s_type    *call_ptr,
    /**< pointer to a call object. */

    cm_call_event_e_type    call_event
    /**< notify client list of this call event */
    
    );

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

#ifdef FEATURE_HDR_CSNA
#error code not present
#endif /* FEATURE_HDR_CSNA */

#endif /* HOMERI_H */


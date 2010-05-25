#ifndef DSMIP_RMSM_H
#define DSMIP_RMSM_H
/*===========================================================================

                          D S M I P _ R M S M . H

DESCRIPTION

 The Data Services Mobile IP Rm State Machine header file.

EXTERNALIZED FUNCTIONS
   mip_rmsm_init()
     Initialize the Registration State Machine.
   mip_rm_post_event()
     Post an event to a Registration State Machine.
   mip_rm_sm_process_event()
     Process the given Rm state machine event.  

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_rmsm.h_v   1.1   16 Aug 2001 22:13:02   syee  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_rmsm.h#1 $ $DateTime: 2007/11/06 01:07:58 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/25/03     vr    Added macro DSMIP_RMSM_IS_PACKET_CALL
08/15/01    sjy    Added comments, code cleanup.
07/16/01    sjy    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  RMSM_MIN_EV = -1,
  RMSM_RM_WANTS_MIP = 0,                     /* Rm wants M.IP              */
  RMSM_RM_IFACE_DOWN = 1,                    /* Rm interface is down       */
  RMSM_RM_IFACE_UP = 2,                      /* Rm interface is up         */   
  RMSM_UM_MIP_DOWN = 3,                      /* Um interface is down       */
  RMSM_UM_IFACE_UP = 4,                      /* Um interface is up         */ 
  RMSM_UM_MIP_UP = 5,                        /* Um M.IP is up              */
  RMSM_MAX_EV = 6
} mip_rm_sm_event_type;
           
/*---------------------------------------------------------------------------
  Declaration of the states of the state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  RMSM_MIN_STATE = -1,
  RMSM_RM_NO_MIP_STATE = 0,         /* Null state - wait for trigger event */                            
  RMSM_WAITING_FOR_RM_UP_STATE = 1, /* Waiting for Rm iface to come up     */
  RMSM_WAITING_FOR_UM_UP_STATE = 2, /* Waiting for Um iface to come up     */             
  RMSM_UM_RM_UP_STATE = 3,          /* Rm and Um interface is up           */
  RMSM_RESYNCING_UM_STATE = 4,      /* Resyncing Um with Rm options        */
  RMSM_MAX_STATE = 5
} mip_rmsm_state_type;

/*---------------------------------------------------------------------------
  The Rm state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  mip_rmsm_state_type state;          /* State machine state               */
  boolean             inited;         /* State machine been initialized?   */
  boolean             um_always_up;   /* Um provisioned to always stay up? */
  boolean             um_mip_changed; /* Has mip_mode changed on the Um?   */
} rmsmi_info_type;

/*---------------------------------------------------------------------------
  The control block of the Rm state machine.
---------------------------------------------------------------------------*/
extern rmsmi_info_type rmsmi_info;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
  MACRO DSMIP_RMSM_IS_PACKET_CALL()

  DESCRIPTION
    This macro checks whether a DTE is connected and we are in a laptop call

  PARAMETERS
    None

  RETURN VALUE
    TRUE if we are in a laptop call, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
#define DSMIP_RMSM_IS_PACKET_CALL() (rmsmi_info.state!=RMSM_RM_NO_MIP_STATE)

/*===========================================================================
  FUNCTION MIP_RM_SM_INIT()

  DESCRIPTION
    This function initializes the Rm state machine.
                                                                                        
  PARAMETERS
    None

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mip_rm_sm_init
(
  void
);

/*===========================================================================
  FUNCTION MIP_RM_SM_POST_EVENT()

  DESCRIPTION
    This function receives posted event and queues the command to ps_cmd() 
    to run in the ps task.
    
  PARAMETERS
    event: the event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    mip_rm_sm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void mip_rm_sm_post_event
(
  mip_rm_sm_event_type event
);

/*===========================================================================
  FUNCTION MIP_RM_SM_PROCESS_EVENT()

  DESCRIPTION
    This function processes the given Rm state machine event.

  PARAMETERS
    event: the event that is to be processed in the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    mip_rm_sm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void mip_rm_sm_process_event
(
  mip_rm_sm_event_type event
);
                       
#endif /* DSMIP_RMSM_H */


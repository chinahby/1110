#ifndef DSMIP_REGSM_H
#define DSMIP_REGSM_H
/*===========================================================================

                          D S M I P _ R E G S M . H

DESCRIPTION

 The Data Services Mobile IP Registration State Machine header file.

EXTERNALIZED FUNCTIONS
   mip_reg_sm_init()
     Initialize the Registration State Machine.
   mip_reg_post_event()
     Post an event to a Registration State Machine.
   mip_reg_sm_post_event_all()
     Pose an event to ALL of the Registrations State Machines.

Copyright (c) 2000 - 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_regsm.h_v   1.23   10 Oct 2002 15:56:52   jayanthm  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip_regsm.h#2 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
06/11/03    jd     Added mip_reg_sm_set_rtt_estimation
06/10/03    ss     Removed macro MIP_IS_REREG_IN_PROGRESS.
05/05/03    jd     Added mip_reg_sm_session_active() prototype
10/09/02    jay    Moved #define MIP constants to dsmip.h
09/17/02    jd     Under FEATURE_DS_IS_835_B changed UNSET_HA_ADDR to 0.0.0.0 
                   to support Dynamic HA Assignment
05/10/02    jd     Moved rsmi_state and rsm_info_type to dsmip.h since these
                   are part of session info block
05/07/02    jd     Moved session info type to dsmip.h
03/21/02   jd/sjy  Added function to check if all MIP sessions are registered
12/11/01    ss     Renamed mip_is_data_transferred to mip_is_rereg_reqd
11/26/01    vr     Added macro MIP_IS_REREG_IN_PROGRESS. Removed flag
                   mip_is_rereg_in_progress.
11/21/01    ss     Added declarations to support sending RRQ only if there
                   has been some data transfer since last registration.
11/14/01    ss     Added a new field, retry_interval_offset to 
                   mip_session_info structure.
11/05/01    vr     Add a flag mip_is_rereg_in_progress for ppp to determine if
                   re-registration is going on
09/16/01    ss     Removed MIP_MAX_SESSIONS macro definition (moved it to
                     dsmip.h)
09/11/01    mvl    Added some new constants for change in unset HA value.
                   Also reorganized some structure entries to be more
                   efficient.  Minor code cleanup.
08/27/01    mvl    Added time units in comments for session info block.
08/14/01    aku    Added app_id and force_dormancy fields in rsm_info_type
08/04/01    jd     added holdoff timer fields into registration info block.
07/12/01    jd     renamed MAX_MIP_SESSIONS to MIP_MAX_SESSIONS to make 
                   association of definition with mobile IP code clear.
06/20/01    jd     Added an entry in the session info block for MN-AAA SPI
06/12/01    mvl    Modified to support new design
05/17/01    jd     Added immediate retry event
02/16/01    mvl    Updates to reflect latest design.
12/02/00    mvl    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include <stdlib.h>
#include "dsmip.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  RSM_MIN_EV = -1,
  RSM_EXIT_MIP_EV,          /* Told to exit M.IP                           */
  RSM_MOVE_EV,              /* Move event - need to register               */
  RSM_SOL_FAILED_EV,        /* Solicitation state machine failed           */
  RSM_IMMED_RETRY_EV,       /* Immediate Retry                             */
  RSM_NEED_NEW_FA_INFO_EV,  /* Need new FA info                            */ 
  RSM_RRQ_SUCC_EV,          /* RRQ Successful                              */
  RSM_RRQ_FAIL_W_RETRY_EV,  /* RRQ Failed retry                            */
  RSM_RRQ_FAIL_NO_RETRY_EV, /* RRQ Failed no retry                         */
  RSM_RRQ_FAIL_HA_UNAVAIL_EV,/* RRQ failed HA unavailable                  */
  RSMI_RRQ_TIMER_EXP_EV,    /* RRQ Timer expired - re-RRQ (internal event) */
  RSMI_LIFE_TIMER_EXP_EV,   /* Reg Lifetime expired (internal event)       */
  RSMI_DEREG_MIP_EV,        /* Deregister M.IP before exiting M.IP         */
  RSM_MAX_EV
} mip_reg_sm_event_type;

extern boolean mip_is_rereg_reqd;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
MACROS MIP_SES_PTR_2_INDEX() and INDEX_2_MIP_SES_PTR()

DESCRIPTION
  These macros convert from either a pointer to A MIP session info block to
  an index or visa versa.

PARAMETERS
  ses_ptr: pointer to a MIP session info block
  index: index into the MIP seesion info block

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MIP_SES_PTR_2_INDEX( ses_ptr ) \
  abs((ses_ptr) - mip_ses_info_array)

#define INDEX_2_MIP_SES_PTR( index ) \
  (mip_ses_info_array + (index))


/*===========================================================================
MACRO MIP_DATA_TRANSFER_ACTION

DESCRIPTION
  Macro to be called whenever data transfer takes place on Um. If data is 
  being transferred for the first time since previous registration, we 
  want to register immediately (only if we are in re-rrq state).

PARAMETERS
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MIP_DATA_TRANSFER_ACTION         \
  if (mip_is_rereg_reqd == FALSE)  \
  {                                      \
    mip_is_rereg_reqd = TRUE;      \
    mip_reg_sm_immed_registration_all(); \
  }

/*===========================================================================
FUNCTION MIP_REG_SM_INIT()

DESCRIPTION
  This function initializes the agent solicitation state machine.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_reg_sm_init
(
  void
);


/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the Agent solicitation.

PARAMETERS
  mip_cb: pointer to the control block array
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event
(
  mip_session_info_type *mip_info,
  mip_reg_sm_event_type event
);


/*===========================================================================
FUNCTION MIP_REG_SM_POST_EVENT_ALL()

DESCRIPTION
  This function posts an event to ALL of the registration state machines.

PARAMETERS
  event: The event to be posted to the state machines.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reg_sm_post_event_all
(
  mip_reg_sm_event_type event
);

/*===========================================================================
FUNCTION MIP_REG_SM_IMMED_REGISTRATION_ALL

DESCRIPTION
  This function posts IMMED_RETRY_EV to every registration state machine
  which is in RE_RRQ_STATE.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void mip_reg_sm_immed_registration_all
(
  void
);


/*===========================================================================
FUNCTION MIP_REG_SM_ALL_SESSIONS_REGD

DESCRIPTION
  Checks if all sessions are registered

PARAMETERS
  None

RETURN VALUE
  TRUE if all sessions are registered
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_reg_sm_all_sessions_regd
(
  void
);

/*===========================================================================
FUNCTION MIP_REG_SM_SESSION_ACTIVE()

DESCRIPTION

  This function checks if Mobile IP session currently has a valid
  registration (registered or reregistering) on the Um interface.

  This function executes in the context of the application task.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE

  TRUE:  If MIP is active
  FALSE: If MIP is not active

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean mip_reg_sm_session_active
(
  mip_session_info_type * session
);

/*===========================================================================
FUNCTION MIP_REG_SM_SET_RTT_ESTIMATION

DESCRIPTION
  Sets the estimated RTT between sending RRQ and RRP based on the current
  rrq_timeout timer value.

PARAMETERS
  None

RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mip_reg_sm_set_rtt_estimation
(
  mip_session_info_type * session
);

#endif /* FEATURE_DS_MOBILE_IP */

#endif /* DSMIP_REGSM_H */

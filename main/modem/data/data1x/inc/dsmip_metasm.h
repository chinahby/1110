#ifndef DSMIP_METASM_H
#define DSMIP_METASM_H
/*===========================================================================

                         D S M I P _ M E T A S M . H

DESCRIPTION

 The Data Services Mobile IP Meta State Machine header file.

EXTERNALIZED FUNCTIONS
   mip_meta_sm_init()
     Initialize the Meta State Machine.
   mip_meta_post_event()
     Post an event to the Meta State Machine.
   mip_meta_sm_config_session()
     Load MIP session defaults from NV to all sessions.

Copyright (c) 2000 - 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_metasm.h_v   1.10   29 Dec 2002 15:45:36   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip_metasm.h#2 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported MOBILE_IP_DEREG feature.
11/14/02    jd     Removed netmodel_mode from msmi_info (unused)
                   mip_meta_sm_set_in_flow takes uint32 mask instead of 16
05/10/02    jd     moved session info array, ui session & password data
                   strutures, mip_meta_sm_config_session to dsmip.c in order
                   to seperate configuration functionality from metasm and
                   solve build dependency issues
10/24/01    js     Modified to support dsmip_info structure. 
10/09/01    js     Added declaration for externalized function
                   mip_meta_sm_config_session().
07/29/01    jd     Added accessor function to see if mip is active
06/14/01    mvl    Udated to support latest design.
12/18/00    mvl    Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_DS_MOBILE_IP

#ifdef FEATURE_DATA_MM
#include "ps707_dssnet.h"
#include "ps_mip_compat.h"
#else
#include "dssnet.h"
#endif /* FEATURE_DATA_MM */

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  MSM_MIN_EV = -1,
  MSM_IFACE_UP_W_MIP_EV  = 0,    /* Interface is UP and M.IP is supported    */
  MSM_IFACE_UP_WO_MIP_EV = 1,    /* Interface is UP and M.IP is unsupported  */
  MSM_IFACE_DOWN_EV      = 2,    /* Interface is down                        */
  MSM_INIT_RRQ_FAILED_EV = 3,    /* Initial Reg Req failed                   */ 
  MSM_RE_RRQ_FAILED_EV   = 4,    /* Re-Reg Req failed                        */
  MSM_BRING_DOWN_MIP_EV  = 5,    /* Start M.IP deregistration                */
  MSM_MAX_EV
} mip_meta_sm_event_type;

/*---------------------------------------------------------------------------
  Declaration of the states of the meta state machine.
---------------------------------------------------------------------------*/
typedef enum
{
  MSMI_MIN_STATE = -1,
  MSMI_CLOSED_STATE,       /* The null state - waiting for a trigger event */
  MSMI_OPEN_STATE,         /* Solicitation sent                            */
  MSMI_MAX_STATE
} msmi_state_type;

/*---------------------------------------------------------------------------
  The meta state machine's information block
---------------------------------------------------------------------------*/
typedef struct
{
  msmi_state_type state;
  boolean inited;
} msmi_info_type;

/*---------------------------------------------------------------------------
  The control block of the meta state machine.
---------------------------------------------------------------------------*/
extern msmi_info_type msmi_info;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION MIP_META_SM_INIT()

DESCRIPTION
  This function initializes the meta state machine.

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
int mip_meta_sm_init
(
  void
);


/*===========================================================================
FUNCTION MIP_META_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the meta state machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_meta_sm_post_event
(
  mip_meta_sm_event_type event
);

/*===========================================================================
FUNCTION MIP_META_SM_SET_IN_FLOW()

DESCRIPTION
  This function enables and disables flow control to both sockets and the
  serial port.  It also modifies the Um state - if flow is being disabled it
  then the Um mode is saved and changed to internal mode - when the flow is
  enabled the Um mode is restored.

PARAMETERS
  mask:    the flow control mask
  enabled: whether flow should be enabled

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_meta_sm_set_in_flow
(
  uint32  mask,
  boolean enabled
);

/*===========================================================================
FUNCTION MIP_META_SM_IS_INITIALIZED

DESCRIPTION
  Accessor function to see if MIP subsystem is initialized.

PARAMETERS
  None

RETURN VALUE
  TRUE  - yes, the MIP subsystem is initialized.
  FALSE - no, the MIP subsystem is not initialized.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_is_initialized
(
  void
);


/*===========================================================================
FUNCTION MIP_META_SM_OPEN()

DESCRIPTION
  Accessor function to see if Mobile IP is active or coming up.

PARAMETERS
  None

RETURN VALUE
  TRUE - mip is active or coming up
  FALSE - not active or coming up

DEPENDENCIES
  mip_meta_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_meta_sm_open
(
  void
);

#endif /* FEATURE_DS_MOBILE_IP */

#endif /* DSMIP_METASM_H */


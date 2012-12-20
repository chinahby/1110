#ifndef MCCTC_H
#define MCCTC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E 
                       H E A D E R   F I L E

GENERAL DESCRIPTION
  This module externalizes data items needed by the support routines
  for Mobile Station Control on the Traffic Channel state.

Copyright (c) 1990-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctci.h_v   1.1.2.0   30 Nov 2001 17:23:58   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mcctci.h#2 $ $DateTime: 2010/11/09 07:33:32 $ $Author: sshahi $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
09/29/05   sb      Removed tc_value.
05/19/05   sb      Removed tc_private_lcm_active.
06/02/03   bkm     Lint cleanup.
06/14/01   va      Added prototype for tc_handle_ccim_event
10/27/00   jq      Merged T_AUTH to FEATURE_AUTH.
07/09/99   ry      Removed the extern definitions for the handoff state
                   indicators mcc_frame_offset_ho_pending and
                   mcc_hard_handoff_in_progress
05/30/96   dna     Created header.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "mccccim.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Lookup table for Forward Power Control pwr_rep_frames parameter */
extern const word pwr_rep_frames [];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Buffer for message with requested action time                           */
typedef union
{
  caii_gen_tc_type   gen_tc;
  caii_ftc_ord_type  tc_ord;
  #ifdef FEATURE_IS2000_REL_A
  caii_security_mode_cmd_type  tcsmcm;
  #endif /* FEATURE_IS2000_REL_A */
} tc_action_msg_type;

extern tc_action_msg_type tc_action_msg;

/* Buffer for Service Option Control Order with requested action time      */

extern caii_ftc_ord_type tc_action_so_ctl_order;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Power Control Reporting state indicators                                */

/* Periodic power control reporting state  */
extern boolean mcc_rx_periodic_rpt_on;

/* Threshold power control reporting state */
extern boolean mcc_rx_threshold_rpt_on;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Variables associated with Start and Stop Continous DTMF processing      */

extern boolean tc_stop;
  /* Indicates that when mcc_dtmf_timer expires a DTMF_STOP order must
     be transmitted */
extern boolean tc_ack_wait;
  /* Indicator that we are waiting for an ack to a Start or Stop Continous
     DTMF order or a Burst DTMF message */
extern byte tc_cont_dtmf;
  /* Pending continous DTMF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
extern word mar_rate;
    /* Markov rate to send to Markov Service Option */

/*===========================================================================

FUNCTION TC_HANDLE_CCIM_EVENTS

DESCRIPTION
  This function processes the primitives coming from the Call Control
  Instance Manager

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This can result in more primitives being sent back to CCIM or messages
  to the BS.
===========================================================================*/

extern void tc_handle_ccim_event( mccccim_tc_event_type * evt_ptr);
/* Prototype for function that handles primitives from MCCCCIM */

#endif /* MCCTC_H */


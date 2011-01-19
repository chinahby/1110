#ifndef RXCIDLE_H
#define RXCIDLE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Receive Task Idle State Processing.

GENERAL DESCRIPTION
  Contains implementation of idle state operations that are not specifically
  tied to a physical layer channel.  Paging Channel, BCCH, and FCCCH all use
  this common functionality.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcidle.h_v   1.9   14 Aug 2002 13:22:24   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/rxcidle.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
03/05/03   fc      Consolidated f-csh fade timer implementation.
10/21/02   bn      Added support for NCC statistic counters
08/14/02   sr      Added #define MAX_SKIPPED_FRAMES
08/13/02   sr      Added Fast RAHO support
06/03/02   sr      Resolved compile errors for non-REL_A builds
05/31/02   sr      Added rxcidle_first_frame() to check if we are in the first
                   frame after wakeup
02/15/02   lad     Added rxcidle_chan_config().  Added rxcidle_pending_state()
                   for state transitions triggered in external context.
01/08/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "rxc.h"
#include "cai.h"

/* Set the number of frames that will be skipped while waiting for the
   first frame after the slot boundary, in the case that the first frame
   is not being recognized. */
#define MAX_SKIPPED_FRAMES 6

#include "cai.h"

/* -----------------------------------------------------
 * This represents the state of the mobile while it
 * it is monitoring the Paging channel for Fade reports.
 * ----------------------------------------------------- */
typedef enum {
  RXCIDLE_IDLE_FADE,
  RXCIDLE_ACCESS_FADE,
  RXCIDLE_CURR_FADE /* Used to specify the use of existing fade type. */
} rxcidle_fade_type;

#ifdef FEATURE_IS2000_REL_A_CC
typedef enum
{
  RXCIDLE_SLOTTED_ASLEEP,
  RXCIDLE_SLOTTED_AWAKE
} rxcidle_slotted_status_type;
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION RXCIDLE_DEF_TIMER

DESCRIPTION
  This function defines f-csh fade timer.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcidle_def_timer (void);

/*===========================================================================

FUNCTION RXCIDLE_INIT

DESCRIPTION
  This function initializes the module.
  
DEPENDENCIES
  This must be called prior to using this module for the first time.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcidle_init (void);

/*===========================================================================

FUNCTION RXCIDLE_SET_FADE_TIMER

DESCRIPTION
  This function sets the appropriate fade timer for idle mode operation.

DEPENDENCIES
  rxcidle_init() must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcidle_set_fade_timer (rxcidle_fade_type fade_type);


/*===========================================================================

FUNCTION RXCIDLE_CLR_FADE_TIMER

DESCRIPTION
  This function clears the appropriate fade timer for idle mode operation.

DEPENDENCIES
  rxcidle_init() must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcidle_clr_fade_timer (void);

/*===========================================================================

FUNCTION RXCIDLE_WAIT_MASK

DESCRIPTION
  This function returns the REX signals that need to be monitored when
  rex_wait() is called during idle state operation.

DEPENDENCIES
  rxcidle_init() must have been called.

RETURN VALUE
  REX wait mask.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type
rxcidle_wait_mask (void);


/*===========================================================================

FUNCTION RXCIDLE_PROCESS_SIGS

DESCRIPTION
  This function checks signals for timer expiration and handles them.

DEPENDENCIES
  rxcidle_init() must have been called.

RETURN VALUE
  Boolean indicating if it is OK to continue normal operation.

SIDE EFFECTS
  None.

===========================================================================*/
boolean
rxcidle_process_sigs (rex_sigs_type sigs, rxc_state_type state);


/*===========================================================================

FUNCTION RXCIDLE_FIRST_FRAME

DESCRIPTION
  This function determines whether it is the first frame after wake
  up

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is the first frame; FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcidle_first_frame( rxc_chan_type chan );

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION RXCIDLE_CHAN_CONFIG

DESCRIPTION
  Configures the idle state to set its channel configuration.
  This RXC internal funciton is not to be confused with 
  rxc_idle_chan_config () which is externalized in rxc.h.

DEPENDENCIES
  None.

RETURN VALUE
  REX wait mask.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type 
rxcidle_chan_config (rxc_idle_chan_info_type *config);

/*===========================================================================

FUNCTION RXCIDLE_GET_SLOTTED_STATUS

DESCRIPTION
  This function determines whether it is the first frame after wake
  up

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
rxcidle_slotted_status_type rxcidle_get_slotted_status( void );

/*===========================================================================

FUNCTION RXCIDLE_SET_SLOTTED_STATUS

DESCRIPTION
  This function determines whether it is the first frame after wake
  up

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void rxcidle_set_slotted_status( rxcidle_slotted_status_type status );


#ifdef FEATURE_IS2000_REL_A_CC_SMMC
rxc_state_type rxcidle_nccchan_config( rxc_chan_config_cmd_type *chan_config);
#endif /* FEATURE_IS2000_REL_A_CC_SMMC */

#endif /* FEATURE_IS2000_REL_A_CC */

#endif /* RXCIDLE_H */

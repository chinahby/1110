#ifndef RXCNCCCH_H
#define RXCNCCCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Common Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
  new Common Control Channel (NCCCH) operation.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath:
$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/06   trc     Add NCCCH failsafe timer support
02/07/03   bn,sr   Created the new file to handle all the NCC channels

===========================================================================*/

#include "customer.h"
#include "target.h"
#ifdef FEATURE_IS2000_REL_A_CC_SMMC

#include "comdef.h"

#include "cai.h"
#include "rxc.h"

/*=======================Global Variable=====================================*/
#define RXCNCCCH_PRIMARY_BCCH_BIT      0x01
#define RXCNCCCH_SECONDARY_BCCH_BIT    0x02
#define RXCNCCCH_FCCCH_BIT             0x04
extern byte rxcnccch_chan_enable_bits;


/* Duration of the simultaneous BCCH+FCCCH failsafe timer, in ms.
 * This timer ensures that BOTH the BCCH and FCCCH channels are
 * being properly demodulated, while the standard T30M timer
 * only ensures that ONE of the two are being properly demodulated.
 * For this reason, the failsafe timer should be longer than T30M,
 * to give the T30M timer first chance to declare that there is
 * a problem in the demodulation.  See the documentation of the
 * failsafe timer in function rxcnccch_failsafe_timer() below.
 */
#define RXCNCCCH_FAILSAFE_TIMER_MS    4000 /* T30M + 1000ms */

/* The following are valid commands/notifications that can be sent
 * to the failsafe timer via the rxcnccch_failsafe_timer() function
 * detailed below.
 */
typedef enum {
  /* Start the failsafe timer and clear the failure mask.
   * (Do nothing, however, if timer is already started.)
   */
  RXCNCCCH_FAILSAFE_START,
  /* Stop the failsafe timer and clear any pending signal */
  RXCNCCCH_FAILSAFE_STOP,
  /* Notify the failsafe of a valid FCCCH channel.
   * Reset the failsafe timer if BCCH is also presently valid.
   */
  RXCNCCCH_NOTIFY_FCCCH_GOOD,
  /* Notify the failsafe of an invalid FCCCH channel */
  RXCNCCCH_NOTIFY_FCCCH_BAD,
  /* Notify the failsafe of a valid BCCH channel.
   * Reset the failsafe timer if FCCCH is also presently valid.
   */
  RXCNCCCH_NOTIFY_BCCH_GOOD,
  /* Notify the failsafe of an invalid BCCH channel */
  RXCNCCCH_NOTIFY_BCCH_BAD
} rxcnccch_failsafe_cmd_type;

/*===========================================================================

FUNCTION RXCNCCCH_STATE

DESCRIPTION
  NCCCH state processing.

DEPENDENCIES


RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type rxcnccch_state (void);

void rxcnccch_isr(void);
void rxcnccch_activate (void);


/*===========================================================================
FUNCTION:
  rxcnccch_failsafe_timer

DESCRIPTION:
  This function encapsulates a failsafe timer for use during simultaneous
  BCCH + FCCCH demodulation.  The failsafe timer monitors BOTH the BCCH
  and FCCCH demodulation, and will set a signal (RXCNCCCH_FAILSAFE_TIMER_SIG)
  to the CDMA receive subtask when either (or both) of the BCCH and FCCCH
  have reported bad frames or messages for a duration of
  RXCNCCCH_FAILSAFE_TIMER_MS.

  This timer is needed due to the fact that the standard T30M timer only
  ensures that ONE of either BCCH or FCCCH has good messages.  In the
  unlikely (but possible) scenario that the BCCH is bad, but the FCCH is
  good (or vice-versa), the T30M timer will never fire, but the failsafe
  will.

  This function takes a command of type rxcnccch_failsafe_cmd_type that
  instruct the failsafe timer to start, stop, or report good or bad frames
  for either the BCCH or FCCCH channel.

DEPENDENCIES:
  This function uses the global variable rxcnccch_chan_enable_bits to
  detect which channels (FCCCH and/or BCCH) are currently active.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rxcnccch_failsafe_timer(rxcnccch_failsafe_cmd_type cmd);

#endif /* FEATURE_IS2000_REL_A_CC_SMMC */
#endif /* RXCNCCCH_H */

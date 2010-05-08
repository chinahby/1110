#ifndef RXCBCCH_H
#define RXCBCCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Broadcast Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
A Broadcast Control Channel (BCCH) operation.

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcbcch.h_v   1.4   23 Oct 2002 16:26:52   baon  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcbcch.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
10/03/02   bn      Added support for Broadcast BCCH
08/13/02   sr      Added Fast RAHO support
01/17/02   lad     Changed rxcbcch_init() to take rxc_bcch_info_type.
11/29/01   lad     Changed rxcbcch_init() to take cmd_ptr instead of 
                   individual parameters as arguments.
07/16/01   lad     Created file.

===========================================================================*/

#include "customer.h"
#include "target.h"

#ifdef FEATURE_IS2000_REL_A

#include "comdef.h"

#include "cai.h"
#include "rxc.h"
#include "atmr.h"

typedef struct {
  cai_code_rate_type code_rate;
  cai_bcch_rate_type data_rate;
  byte code_chan;
  boolean first_frame;
  atmr_handle_type atmr_handle;
} rxcbcch_info_type;

/* defince constant */
#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/* maximum number of BCBCCH byte per SDU frame : (744/8) -2 (CRC) */
#define BC_BCCH_BYTE_PER_FRAME   91
/* maximum of number of SDU frame per BC BCCH MSG 267 / 91 */
#define MAX_NUM_FRAME_PER_BCBCCH_MSG  3
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
void rxcbcch_exit (void);
void rxcbcch_process_frames(rxc_dec_type *buf_ptr);
boolean rxcbcch_frame_proc_init (void);
void rxcbcch_frame_info_log_send (void);
/*===========================================================================

FUNCTION RXCBCCH_INIT

DESCRIPTION
  This funtion initializes the Broadcast Control Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean
rxcbcch_init
(
  rxc_state_type curr_state, /* Current RXC state machine state */
  rxc_bcch_info_type *config_ptr, /* Configuration pointer */
  boolean init_secondary_bcch     /* indicated if secondary BCCH is active */
);


/*===========================================================================

FUNCTION RXCBCCH_STATE

DESCRIPTION
  BCCH state processing.

DEPENDENCIES
  rxcbcch_init() must have been called prior to entering this state.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type 
rxcbcch_state (void);

/*===========================================================================

FUNCTION RXCBCCH_ACTIVATE

DESCRIPTION
  Activates BCCH during slotted mode operation.

DEPENDENCIES
  Must be in RXC_BCCH_STATE.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcbcch_activate (void);

#endif /* FEATURE_IS2000_REL_A */
#endif /* RXCBCCH_H */

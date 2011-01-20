#ifndef RXCFCCCH_H
#define RXCFCCCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              CDMA2000 Forward Common Control Channel Monitoring

GENERAL DESCRIPTION
  Contains implementation of Physical, LAC and MAC layer of IS2000 release
A Forward Common Control Channel (FCCCH) operation.

  
EXTERNALIZED FUNCTIONS
  rxcfccch_init - initializes the FCCCH processing state
  rxcfccch - FCCCH state processing
  rxcfccch_isr - interrupt handler for the Decoder interrupt used when RXC
    is processing the FCCCH
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcfccch.h_v   1.3   05 Feb 2002 16:39:16   ldefauw  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcfccch.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/04   sr      Merged in changes from 6500 5004 build
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
01/17/02   lad     Updated signature for rxcfccch_init().  Added
                   rxcfccch_activate().  Removed prototype for ISR.
11/20/01   sr      Added prototype for ISR
07/16/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "cai.h"
#include "rxc.h"

typedef struct
{
  deint_code_rate_type code_rate;
  dec_ccch_data_rate_type data_rate;
  byte code_chan;
  boolean first_frame;
  cai_fccch_frame_size_type frame_size;
} rxcfccch_info_type;

typedef struct {
  word frame_size;
  byte subframe_size;
  byte subframe_flush_cnt;
} rxcfccch_frame_info_tbl_type;

void rxcfccch_exit (void);
void rxcfccch_frame_info_log_send (void);
boolean rxcfccch_frame_proc_init (dec_ccch_data_rate_type);
void rxcfccch_process_frames (rxc_dec_type *buf_ptr);
void rxcfccch_update_frame_counters(void);
/*===========================================================================

FUNCTION RXCFCCCH_INIT

DESCRIPTION
  This funtion initializes the Forward Common Control Channel processing state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean
rxcfccch_init
(
  rxc_state_type curr_state,     /* Current RXC state machine state */
  rxc_fccch_info_type *config_ptr /* Configuration pointer */
);


/*===========================================================================

FUNCTION RXCFCCCH_STATE

DESCRIPTION
  FCCCH state processing.

DEPENDENCIES
  rxcfccch_init() must have been called prior to entering this state.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
rxc_state_type 
rxcfccch_state (void);



/*===========================================================================

FUNCTION RXCFCCCH_ACTIVATE

DESCRIPTION
  Activates FCCCH during slotted mode operation.

DEPENDENCIES
  Must be in RXC_FCCCH_STATE.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcfccch_activate (void);

#endif /* RXCFCCCH_H */

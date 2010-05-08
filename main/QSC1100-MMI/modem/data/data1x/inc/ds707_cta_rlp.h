#ifndef DS707_CTA_H
#define DS707_CTA_H
/*===========================================================================

                         D S 7 0 7 _C T A _ R L P
GENERAL DESCRIPTION
  This file contains functions used to to handle idle timeout processing.
  A bitmask is maintained.  If the bitmask is all zeroes, then the idle timer
  is decremented.  If the bitmask is non-zero, then the idle timer is
  stopped.
  
EXTERNALIZED FUNCTIONS
  DS707_CTA_RESET_IDLE_TIMER
    Called at the beginning of a packet data call.  Resets the idle timer 
    bitmasks, etc.  For QNC calls, dormancy is not allowed.  In that case
    the bitmask is set so that the bitmask is never all zeroes.
    
  D707_CTA_SET_IDLE_BIT
    Called by when there is active traffic on the channel.  Do not go
    idle.  
    
  DS707_CTA_CLEAR_IDLE_BIT
    Called when there is no active traffic on the channel.  Allows mobile
    to go idle.
  
  DS_RLP_DATA_RXED
    Called by RLP 3 every 20 msecs.  RLP passes a param indicating if data
    was received or not.
    
  DS_PROCESS_RLP_RX_FRAMES
    Called by mux every 20 msecs for RLP 1 & RLP 2 calls.  This function
    then passes each frame down to RLP and sets/clears the idle bits
    appropriately.
  
  DS707_CTA_RLP_SYNC_TIMEOUT
    Called by RLP if the SYNC handshake takes too long.  Results in the
    call being torn down.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call DS707_CTA_RESET_IDLE_TIMER at the beginning of every packet
  call.

 Copyright (c) 2002-2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_cta_rlp.h_v   1.1   15 Nov 2002 18:25:26   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_cta_rlp.h#1 $ $DateTime: 2008/04/11 08:25:15 $ $Author: nsivakum $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   sn      Removed FEATURE_SPECIAL_MDR.
11/15/02   ak      Updated comments in file header.
05/10/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsm.h"
#include "ds707.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif 

/*===========================================================================
                     TYPEDEFS & VARIABLES
===========================================================================*/

#define DS707_CTA_IS_IDLE_MASK  0x0000 /* Traffic channel is idle, idle    */
                                       /* timer running. When a call starts*/
                                       /* this is the default bitmask (so  */
                                       /* idle timer starts running as soon*/ 
                                       /* as call comes up                 */
                                       
#define DS707_CTA_RLP_PHYS_LINK        0x0001 /* TC up or down             */
#define DS707_CTA_RLP_TX               0x0002 /* bitmask for RLP Tx        */
#define DS707_CTA_RLP_RX               0x0004 /* bitmask for RLP Rx        */
#define DS707_CTA_SIO_RX               0x0008 /* bitmask for SIO Rx        */


/*---------------------------------------------------------------------------
  Used in the reset() function to start/stop the idle timer.  
---------------------------------------------------------------------------*/
#define DS707_CTA_START_TIMER   TRUE        /* start the timer             */
#define DS707_CTA_STOP_TIMER    FALSE       /* stop the timer              */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CTA_RESET_IDLE_TIMER

DESCRIPTION   Called when the traffic channel comes up or goes down  If the
              conditions are correct, clears the idle timer mask and starts
              the idle timer.  
  
              Otherwise disables the idle timer by setting a reserved bit.
              Note that the reserved bit is only set in this function.  By
              doing this, we don't need to check for AT+CTA, QNC, or phys
              link in the set_/clear_idle_bit functions.
 
              The boolean can be either:
                DS707_CTA_START_TIMER - start the idle timer
                DS707_CTA_STOP_TIMER  - stop the idle timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_reset_timer
(
  boolean           start_stop,           /* start or stop timer?          */
  ds707_so_type     so                    /* service option being used     */
);

/*===========================================================================
FUNCTION      DS707_CTA_SET_IDLE_BIT

DESCRIPTION   This function takes a bitmask as a parameter, and then sets the
              appropriate bit in the dsi_dormant_bitmask.  If needed, it will
              also cancel the idle channel timer.
              
              Call this function when the channel is *not* IDLE; i.e., has 
              real data going back and forth.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_set_idle_bit
(
  uint32   idle_bitmask                                 /* Dormant Bitmask */
);

/*===========================================================================
FUNCTION      DS707_CTA_CLEAR_IDLE_BIT

DESCRIPTION   This function takes a bitmask as a parameter, and then clears
              the appropriate bit in the dsi_dormant_bitmask.  If needed, it
              will also start the idle channel timer.

              Call this function when the channel is IDLE; i.e., NO 
              real data going back and forth.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_clear_idle_bit
(
  uint32   idle_bitmask                                 /* Dormant Bitmask */
);

/*===========================================================================
FUNCTION      DS_RLP_DATA_RXED

DESCRIPTION   This function is called from the rlp layer, after the frames
              received in a 20 msec period have been processed.  If the
              input parameter is FALSE, then no data/ctl/init frames have
              been received in the past 20 msec.  Otherwise at least one
              data frame was received.  If data has been received, then the
              idle timer should be reset, so that it doesn't expire.
  
              This function should be called from the RX task.

DEPENDENCIES  Called only from RLP 3.  Implies only packet data calls.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_rlp_data_rxed
(
  boolean     data_frs_rxed      /* tells if any non-idle/fill frames rxed */
);

/*===========================================================================
FUNCTION      DS_PROCESS_RLP_RX_FRAMES

DESCRIPTION   This function invokes the RLP RX function to process RLP
              frames received over the air, and terminates the call if the
              RLP protocol fails to synchronize. For packet data calls, it
              also maintains the dormancy state based on the return value
              from the RLP RX processing,

              This function should be called from the RX task, for both MDR
              and non-MDR calls.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_process_rlp_rx_frames
(
#ifdef FEATURE_IS95B_MDR
  rlp_fwd_frame_block_type  *fwd_frame_block  /* pt to RLP fwd frame block */
#else
  rlp_rate_enum_type frame_rate,              /* Rate of RLP frame         */
  boolean            pri_chan,                /* Primary Channel indicator */
  dsm_item_type      *rlp_item_ptr            /* ptr to RLP frame item     */
#endif 
);

/*===========================================================================
FUNCTION      DS707_CTA_RLP_SYNC_TIMEOUT

DESCRIPTION   Called by RLP when there is a sync timeout.  Should bring
              down the traffic channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_rlp_sync_timeout(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_CTA_H        */


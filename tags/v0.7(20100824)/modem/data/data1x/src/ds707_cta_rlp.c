/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                         D A T A   S E R V I C E S

               C T A  (IDLE TIMEOUT) and related RLP functions

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

  Copyright (c) 1999-2009 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_cta_rlp.c_v   1.1   15 Nov 2002 18:25:18   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_cta_rlp.c#3 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Fixed featurization issue.
02/07/08   sn      Removed FEATURE_SPECIAL_MDR.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
05/23/06   squ     Featurized async data
11/15/02   ak      Updated comments in file header.
07/29/02   ak      On ps_iface commands, check for DS_EWOULDBLOCK.
07/17/02   ak      On ds_process_rlp_frames, do not assume only for pkt
                   calls.
07/03/02   ak      Change errno's to ps_iface funcs from uint16 -> int16.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_cta_rlp.h"
#include "ds707_pkt_mgr.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#include "ds707_async_mgr.h"
#endif /*FEATURE_ASYNC_DATA_NOOP */
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ds707_so_async.h"
#include "dsat707extctab.h"
#else
#include "dsatprofile.h"
#endif
#include "dserrno.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "ps_iface.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif 

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

LOCAL uint32 dsi_dormant_bitmask = DS707_CTA_IS_IDLE_MASK; /* dorm bitmask */

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((start_stop == DS707_CTA_START_TIMER)     &&
      #ifndef FEATURE_ASYNC_DATA_NOOP
      (ds707_so_async_so_is_async(so) == FALSE) && 
      #endif
      (dsat707_cta_val > 0)
     )
  {
    dsi_dormant_bitmask = DS707_CTA_IS_IDLE_MASK;
    ds3gi_start_timer( DS3G_TIMER_CTA, (dsat707_cta_val * 1000));
  }
  else
  {
    dsi_dormant_bitmask = DS707_CTA_RLP_PHYS_LINK;
    ds3gi_stop_timer( DS3G_TIMER_CTA);
  }
} /* ds707_reset_timer() */

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();
  if (dsi_dormant_bitmask == DS707_CTA_IS_IDLE_MASK)
  {
    ds3gi_stop_timer( DS3G_TIMER_CTA);
  }
  dsi_dormant_bitmask = dsi_dormant_bitmask | idle_bitmask;
  INTFREE();
} /* ds707_cta_set_idle_bit() */


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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INTLOCK();
  if (dsi_dormant_bitmask == idle_bitmask)    /* only this bit is set      */
  {
    ds3gi_start_timer( DS3G_TIMER_CTA, (dsat707_cta_val * 1000));
  }
  dsi_dormant_bitmask = dsi_dormant_bitmask & ~idle_bitmask;
  INTFREE();
} /* ds707_cta_clear_idle_bit() */


#ifndef MSM5000_IRAM_FWD
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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( data_frs_rxed == TRUE)
  {
    ds707_cta_set_idle_bit(DS707_CTA_RLP_RX);
  }
  else  /* no data rxed in RLP */
  {
    ds707_cta_clear_idle_bit(DS707_CTA_RLP_RX);
  }
} /* ds_rlp_data_rxed() */
#endif /* MSM5000_IRAM_FWD */

/*===========================================================================
FUNCTION      DS_PROCESS_RLP_RX_FRAMES

DESCRIPTION   This function invokes the RLP RX function to process RLP
              frames received over the air, and terminates the call if the
              RLP protocol fails to synchronize. For packet data calls, it
              also maintains the dormancy state based on the return value
              from the RLP RX processing,

              This function should be called from the RX task, for both MDR
              and non-MDR calls.
              
              Note that this calls the CTA start/stop functions for all
              calls (even async/fax).  This is okay, as the pkt mgr has 
              locked down the CTA timer unless there is an active Pkt 
              call.  Meaning that an async call will have no effect
              on the CTA timer.

DEPENDENCIES  This is called by RLP I and RLP II.  This means that it is
              called for both async and packet calls.  However, the idle
              timeout is specific only to packet calls.  Further, need to
              be able to figure out which ps_iface to use when checking to
              see if in a call.  Since async & pkt 707 calls are not 
              allowed simultaneously (assumption), can see which of the
              two ifaces is active, and make decision accordingly.

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
)
{
#ifdef FEATURE_IS95B_MDR
  rlp_rx_retval_enum_type rlp_rx_status;   /* return status from RLP RX    */
  byte                    i;
#else
  rlp_rx_status_enum_type rlp_rx_status;   /* return status from RLP RX    */
#endif
  ps_iface_type             *iface_ptr;
  int16                      errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    See if there is an active call by checking with pkt and async.  Only
    one can be active at any time.  Both may be inactive.
  -------------------------------------------------------------------------*/
  iface_ptr = ds707_pkt_get_active_iface_ptr();

#ifndef FEATURE_ASYNC_DATA_NOOP
  if (iface_ptr == NULL)
  {
    iface_ptr = ds707_async_get_active_iface_ptr();
  }
#endif /*FEATURE_ASYNC_DATA_NOOP*/
  /*-------------------------------------------------------------------------
    Do not do RLP RX processing unless an active call is in progress.
  -------------------------------------------------------------------------*/
  if (iface_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Pass RLP frame rate, channel type, and frame to RLP RX for processing.
      If the return value indicates data bytes are available then latch data
      available flag and cancel Idle timer, if in Timeout mode.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_IS95B_MDR
    rlp_rx_status = rlp_rx_process_frames(fwd_frame_block);
#else
    rlp_rx_status = rlp_rx_process_frame(frame_rate, pri_chan, rlp_item_ptr);
#endif

#ifdef FEATURE_IS95B_MDR
    if( rlp_rx_status == RLP_RX_NON_IDLE )
#else
    if( rlp_rx_status == RLP_RX_DATA_AVAILABLE)
#endif
    {
      /*---------------------------------------------------------------------
       Received data.  Cancel IDLE timer.
      ---------------------------------------------------------------------*/
      ds707_cta_set_idle_bit(DS707_CTA_RLP_RX);
    } /* if RLP data available */

#ifdef FEATURE_IS95B_MDR
    else if( (rlp_rx_status == RLP_RX_IDLE)   || 
             (rlp_rx_status == RLP_RX_ERROR))
#else 
    else if(( rlp_rx_status    == RLP_RX_NO_DATA_AVAILABLE))
#endif
    {
      /*--------------------------------------------------------------------- 
        Enable the idle timer if no data.
      ---------------------------------------------------------------------*/
      ds707_cta_clear_idle_bit(DS707_CTA_RLP_RX);
    } /* If no RLP RX data */
#ifdef FEATURE_IS95B_MDR
    else if( rlp_rx_status == RLP_RX_SYNCH_TIMEOUT )
#else
    else if( rlp_rx_status == RLP_RX_SYNC_TIMEOUT )
#endif
    {
      /*---------------------------------------------------------------------
        The RLP peers have failed to Sync. End the call.
      ---------------------------------------------------------------------*/
      MSG_HIGH("RLP failed to SYNC, Ending Call", 0, 0, 0);
      if ((ps_iface_phys_link_down_cmd(iface_ptr, &errno, NULL) < 0) &&
          (errno != DS_EWOULDBLOCK))
      {
        ERR_FATAL("Cannot bring down call",0,0,0);
        ERR("Cannot end call on sync timeout %d",errno,0,0);
      }
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      No data call in process, free the RLP frame buffer(s).
    -----------------------------------------------------------------------*/
#ifdef FEATURE_IS95B_MDR
    for(i = 0; i <= fwd_frame_block->num_sup; i++)
    {
      (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
    }
#else
    dsm_free_buffer( rlp_item_ptr);
#endif
  }
} /* ds_process_rlp_rx_frames() */

/*===========================================================================
FUNCTION      DS707_CTA_RLP_SYNC_TIMEOUT

DESCRIPTION   Called by RLP when there is a sync timeout.  Should bring
              down the traffic channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_rlp_sync_timeout(void)
{
  ps_iface_type             *iface_ptr;
  int16                      errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  iface_ptr     = ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);

  MSG_HIGH("RLP failed to SYNC, Ending Call", 0, 0, 0);
  if (( ps_iface_phys_link_down_cmd(iface_ptr, &errno, NULL) < 0) &&
      (errno != DS_EWOULDBLOCK))
  {
    ERR_FATAL("Cannot bring down call",0,0,0);
    ERR("Cannot end call on sync timeout %d",errno,0,0);
  }
}

#endif /* FEATURE_DATA_IS707 */
 

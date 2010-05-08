#ifndef SRCH_MUX_H
#define SRCH_MUX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ M U X ---  H E A D E R   F I L E 

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_mux.  srch_mux 
  contains functions which allow the MUX layer to interface with the SRCH
  task.

EXTERNALIZED FUNCTIONS
  srch_mux_finger_finder
  srch_mux_scomb_get_ref_time
  srch_mux_get_curr_20ms_frame
  srch_mux_page_match
  srch_mux_get_fing_rssi
  srch_mux_get_pilot_ecio
  srch_mux_owns_tx
  srch_mux_get_ulpn_state
  srch_mux_start_pc_state
  srch_mux_get_rx_agc
  srch_mux_get_tx_adj
  srch_mux_set_tx_adj
  srch_mux_set_tx_rate_adj
  srch_mux_report_frame
  srch_mux_notify_fpc_mode
  srch_mux_update_rc
  srch_mux_log_agc_pctl
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2003 - 2009
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_mux.h#2 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
03/17/09   tjc     Add function so MUX can report if we are in a data call
03/24/06   kwo     Rename frame ser to frame sec
03/22/06   kwo     Added srch_mux_idle_frame_stats()
09/08/05   ejv     Add a host of new interface functions for MUX.
07/19/05   grl     Added srch_mux_owns_tx call for mux to call.
05/24/05   ejv     Added srch_get_fing_rssi from srch.h.
05/04/05   kwo     Added srch_mux_page_match()
11/05/04   ejv     Added srch_mux_finger_finder.
10/25/04   ejv     Removed unused srch_mux_update_sch_walsh_len.
07/24/03   ejv     Implementation, first cut.
===========================================================================*/ 

#include "customer.h"
#include "comdef.h"
#include "ulpn.h"
#include "dec5000.h"
#include "deint.h"
#include "cai.h"

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* This is used to find out which 20ms frame the combiner is currently in*/
typedef enum
{
         FRAME_0_MS,
         FRAME_20_MS,
         FRAME_40_MS,
         FRAME_60_MS,
         FRAME_NOT_AVAILABLE

} srch_mux_20ms_frame_type;

/*-------------------------------------------------------------------------
      Prototypes
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       SRCH_MUX_FINGER_FINDER
DESCRIPTION    This function takes a pilot PN and returns which fingers
               are assigned to this pilot

DEPENDENCIES   None

RETURN VALUE   Bit mask of the fingers assigned to this pilot,
               ie. if bit0 is set, then finger0 is assigned
SIDE EFFECTS   None

===========================================================================*/
extern uint8 srch_mux_finger_finder
(
  uint16 pilot_pn  /* Pilot PN to check on */
);

/*===========================================================================

FUNCTION       SRCH_MUX_SCOMB_GET_REF_TIME
DESCRIPTION    This function reads the symbol combiner's time reference
               register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   The 16-bit time reference register value.  Note that this
               value is not exactly binary.  It is binary encoded, but
               its maximum value is 0xBFFF.  Each LSB is two PN chips.
               The top two bits are the 26.666... ms phase within an
               80ms superframe.
SIDE EFFECTS   The time reference latch-on-micro-write register is
               updated.

===========================================================================*/
extern uint16  srch_mux_scomb_get_ref_time( void );

/*===========================================================================

FUNCTION       SRCH_MUX_GET_CURR_20MS_FRAME
DESCRIPTION    This function returns the appropriate 20ms frame enum

DEPENDENCIES   None

RETURN VALUE   20ms frame enum
SIDE EFFECTS   None

===========================================================================*/
extern srch_mux_20ms_frame_type srch_mux_get_curr_20ms_frame( void );

/*===========================================================================

FUNCTION       SRCH_MUX_PAGE_MATCH
DESCRIPTION    This function is used to inform the idle state that
               a fast page match has occured.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_page_match
(
  boolean page_match_succ   /* flag whether page match succeesed or not */
);

/*===========================================================================

FUNCTION       SRCH_MUX_GET_FING_RSSI
DESCRIPTION    This function gets the RSSI of the best finger.
               This is intended to be called as a diagnostic tool.

DEPENDENCIES   None

RETURN VALUE   Word RSSI of best fing assigned and in lock.
               "-1" implies no fingers are in lock.
               "-2" implies no fingers were assigned.
SIDE EFFECTS   None

===========================================================================*/
extern int16 srch_mux_get_fing_rssi( void );

/*===========================================================================

FUNCTION       SRCH_MUX_GET_PILOT_ECIO
DESCRIPTION    This function returns a byte representing the current
               Page Channel active pilot Ec/Io value.

DEPENDENCIES   srch_aset[0]

RETURN VALUE   The 6-bit Ec/Io value.  If the energy is out of range,
               the return value is clipped to 0 or 63. This is in
               1/2 db unit.
SIDE EFFECTS   None

===========================================================================*/
extern uint8 srch_mux_get_pilot_ecio( void );

/*===========================================================================

FUNCTION       SRCH_MUX_OWNS_TX
DESCRIPTION    This function returns whether the transmitter is owned by 1x
               This is a TEMPORARY function which must be removed once 
               enc_set_mode no longer grabs control of the modulator upon 
               entering paging.  If paging is entered on RX1 rather than 
               TXRX0, this shouldn't be done.

DEPENDENCIES   None

RETURN VALUE   TRUE if the transmitter is controlled by 1x, otherwise FALSE.
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_mux_owns_tx( void );

/*===========================================================================

FUNCTION       SRCH_MUX_GET_ULPN_STATE
DESCRIPTION    This function reads the User Long PN Code state from
               the demod ASIC.

DEPENDENCIES   Calling this function asynchronously with the 26.666... ms
               boundary is dangerous.  If the 26.666... ms boundary passes
               while this function is reading the registers, a bogus value
               will be returned. Also, srch_dmod_reset should have been
               previously called,

RETURN VALUE   No formal return value,  but the User Long PN Code state
               is returned in a ulpn_type passed in by reference.  This
               state is the one that was valid at the last
               26.666... ms boundary.
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_get_ulpn_state
(
  ulpn_type   *ulpn  /* Pointer where to put the ULPN value read from demod */
);

/*===========================================================================

FUNCTION       SRCH_MUX_START_PC_STATE
DESCRIPTION    This function returns TRUE the first time it is called
               after Search enters PC state.

DEPENDENCIES   srch_state

RETURN VALUE   boolean indicating if this is first call (TRUE).
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_mux_start_pc_state
(
  boolean reset_only
);

/*===========================================================================

FUNCTION       SRCH_MUX_GET_RX_AGC
DESCRIPTION    This function returns the current value of the rx agc register.
               For targets using MSM2 or higher, this is simply a passthru
               for RF CAGC function calls.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern uint8 srch_mux_get_rx_agc( void );

/*===========================================================================

FUNCTION       SRCH_MUX_GET_TX_ADJ
DESCRIPTION    This function returns the current value of the tx gain
               adjust register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern uint8 srch_mux_get_tx_adj( void );

/*===========================================================================

FUNCTION       SRCH_MUX_SET_TX_ADJ
DESCRIPTION    This function allows external tasks (namely the Transmit
               Task) to have access to the tx gain adjust register of
               the demod chip.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None
SIDE EFFECTS   Changes the contents of the tx gain adjust register of the
               demod chip.

===========================================================================*/
extern void  srch_mux_set_tx_adj
(
  uint8  tx_adj  /* 8-bit value to be written to the tx gain adjust reg */

  /* Note that this is a signed value. For maximum power, use the
     value -128 (0x80); for minimum power, use +127 (0x7F). */
);

/*===========================================================================

FUNCTION       SRCH_MUX_SET_TX_RATE_ADJ
DESCRIPTION    This function allows external tasks (namely the Transmit
               Task) to have access to the tx gain adjust register for
               the data rate.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None
SIDE EFFECTS   Changes the contents of the tx rate adjust register of the
               demod chip.

===========================================================================*/
extern void  srch_mux_set_tx_rate_adj
(
  int16  tx_adj  /* 10-bit value to be written to the tx rate adjust reg */
);

/*===========================================================================

FUNCTION       SRCH_MUX_REPORT_FRAME
DESCRIPTION    This function tells search that there was a frame
               event, and whether frame was an erasure or not. 
               It also passes information about the rate decision
               and raw metrics for FCH.
               
               NOTE - This is valid for FCH ONLY. DCCH is not
               handled in this function at present. 

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_report_frame
( 
  boolean is_erasure,                    /* Frame was bad if TRUE, else good */
  uint16 cur_rate,                       /* Current rate decision on frame */
  uint16 cur_rc,                         /* Current radio config during frame */
  const dec_fch_status_type *fch_metrics /* Energy and SER metrics for frame */
);

/*===========================================================================
FUNCTION       SRCH_MUX_REPORT_DATA_CALL
DESCRIPTION    This function tells search whether the current SO specifies a
               data call is in progress.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_report_data_call
(
  boolean is_data_call                   /* Are we in a data call? */
);

/*===========================================================================

FUNCTION       SRCH_MUX_NOTIFY_FPC_MODE
DESCRIPTION    This function tells search that FPC is enabled.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_notify_fpc_mode
( 
  boolean fpc_enabled,    /* FPC on-off status */
  uint16 mode             /* Mode passed as a word for logging */
);

/*===========================================================================

FUNCTION       SRCH_MUX_UPDATE_RC
DESCRIPTION    This function updates the srch's local copy of the FCH RC
               value which is necessary for logging.  This function is
               called from the MUX layer where the RC value is updated.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mux_update_rc
(
  deint_rc_type  fch_rc_val,    /* Fundamental channel RC value */
  deint_rc_type  sch0_rc_val,   /* Supplemental channel 0 RC value */
  deint_rc_type  sch1_rc_val,   /* Supplemental channel 1 RC value */
  deint_rc_type  dcch_rc_val    /* Dedicated Control channel RC value */
);

/*===========================================================================

FUNCTION       SRCH_MUX_LOG_AGC_PCTL
DESCRIPTION    This function is invoked to log AGC and power control values
               for DIAG.

DEPENDENCIES   srch_agc_pctl_vars

RETURN VALUE   None
SIDE EFFECTS   Messages can be sent to DIAG task.

===========================================================================*/
void srch_mux_log_agc_pctl( void );

/*===========================================================================

FUNCTION       SRCH_MUX_IDLE_FRAME_STATS
DESCRIPTION    This function saves off the frame stats for logging 

DEPENDENCIES   None 

RETURN VALUE   None
SIDE EFFECTS   None 

===========================================================================*/
extern void srch_mux_idle_frame_stats
(
  cai_rx_channel_id_type chan,
  uint16  sec,
  uint32  energy,
  boolean crc
);

#endif /* SRCH_MUX_H */


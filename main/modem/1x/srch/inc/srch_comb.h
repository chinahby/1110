#ifndef SRCH_COMB_H
#define SRCH_COMB_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    C O M B I N E R

            H E A D E R F I L E


GENERAL DESCRIPTION
   This file is part of the breakup of the former file srchdrv.c. This file
   consists of all functions that deal with the symbol, power, and frequency
   combiner in the searcher subsystem.

  Copyright (c) 2001 - 2009 by QUALCOMM, Inc.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_comb.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/06/09   bb      Defined srch_comb_init() method
12/19/06   sst     Merge the following change from 11.02.02B branch
 12/01/06  sst      Remove setting of combiner_gain, it is now controlled by AFC
10/06/05   bt      Added srch_comb_get_rtc_at_sync80.
09/08/05   ejv     Add prototypes for functions in srch_comb.c.
07/27/05   pa      Moved quick slam related functions from srchzz_tl_common
                   to here.
02/17/05   abh/gs  Removed quantization effects on the TrkLo PDM
09/20/04   ejv     Update MAX_PCTL_COMB_CNT.
09/03/04   sfm     Finger Driver changes
09/03/04   ejv     Added srch_comb_chan_type, srch_pcomb_set_init_offset and
                   srch_set_chan_lc_mask.
008/09/04   ejv     Added srch_comb_chip_symbol_type and
                   removed srch_scomb_set_siz.
06/24/04   sfm     Removed prototype for srch_pcomb_set_thrsh()
04/08/04   ejv     Mainline SRCH_HAS_TRK_LO_CNTL.
03/31/04   ejv     Added function srch_scomb_sync() for 6700.
02/20/04   sfm     Added srch_comb_chan_data typedef and updated
                   srch_comb_set_chan() prototype
01/28/04   ejv     Mainline T_MSM6500.
11/11/03   bt      Added srch_comb_trk_lo_parked.
11/05/03   sst     Modified how trk_lo_adj parameters are handled
10/20/03   bt      Added srch_comb_enable_dsp_ints function.
10/10/03   bt      Added srch_scomb_get_master_fing function.
09/29/03   dna     Use FING_SC_DELAY_VAL in multiple files
07/22/03   ejv     Updated interface to srch_comb_set_chan().
03/24/03   kwo     Added srch_scomb_set_ch0_sym_size
08/23/02   kwo     Added srch_scomb_timeline_roll
07/30/02   gs      Initial merge into perforce for cougar.
05/30/02   ejv     Mainlined T_MSM5000.
05/24/02   ejv     Mainlined FEATURE_IS2000_REL_A_VP2.
03/15/02   ejv     Added FEATURE_IS2000_REL_A_VP2.
02/27/02   ejv     Modified date format in change history comments.
02/06/02   ejv     Added srch_comb_set_chan().
01/14/02   SST     Merged into tip to replace srchdrv.c
01/11/02   SST     Completed function name changes.
                        srch_comb_ttrk_ena -> srch_scomb_ttrk_ena
                        srch_comb_ttrk_dis -> srch_scomb_ttrk_dis
                        srch_comb_arm_slam -> srch_scomb_arm_slam
                        srch_comb_set_pn_phs -> srch_scomb_set_pn_phs
                        srch_comb_slam_timeline -> srch_scomb_slam_timeline
                        srch_set_sym_siz -> srch_scomb_set_siz
                        srch_get_ref_time -> srch_scomb_get_ref_time
                        srch_get_vsc_time -> srch_scomb_get_vsc_time
                        srch_clr_manual_roll -> srch_scomb_clr_manual_roll
                        srch_gen_manual_roll -> srch_scomb_gen_manual_roll
                        srch_comb_ftrk_acq -> srch_fcomb_ftrk_acq
                        srch_set_carrier_freq -> srch_fcomb_set_carrier_freq
                        srch_get_carrier_freq -> srch_fcomb_get_carrier_freq
                        srch_set_frq_trk_acc -> srch_fcomb_set_ftrk_acc
                        srch_get_ftrk_acc -> srch_fcomb_get_ftrk_acc
                        srch_park_trk_lo_adj -> srch_fcomb_park_trk_lo_adj
                        srch_pctl_ena -> srch_pcomb_ena
                        srch_pctl_rate_set -> srch_pcomb_rate_set
                        srch_set_pwr_comb_thrsh -> srch_pcomb_set_thrsh
01/10/02   SST     finished move for srchdrv.c breakup
01/01/02   SST     Created Module - refer to srch_hw.c for earlier edit
                      history.

==========================================================================*/

#include "srchi.h"
#include "ulpn.h"



#ifdef T_MSM6700
#define MAX_PCTL_COMB_CNT    6
#else
#define MAX_PCTL_COMB_CNT    3
#endif /* T_MSM6700 */
  /* Max number of power control symbol combiners suppoted by hardware */

#define FING_SC_DELAY_VAL                          0x1bf3e7L
  /* 0x1bf3c1 = 0x1c0000 - 0xc1a + 1...
     0xc1a = 3098 = 387.25 chips * 8 chipx8/chip...
     delay to be used to set the combiner position
     relative to the finger being slammed to. */

#define FING_COMB_OFFSET_CX8     3098     /* offset between comb. and finger */
#define FING_COMB_OFFSET_CHIPS   387      /* offset between combiner and
                                             finger in chips */
typedef enum
{
  COMB_CHAN_0 = 0,  /* Indicates CH0 */
  COMB_CHAN_1,      /* Indicates CH1 */
  COMB_CHAN_2,      /* Indicates CH2 */
  COMB_CHAN_3,      /* Indicates CH3 */
  COMB_CHAN_4,      /* Indicates CH4 */
  COMB_CHAN_5,      /* Indicates CH5 */
  COMB_CHAN_6,      /* Indicates CH6 */
  COMB_CHAN_7,      /* Indicates CH7 */
  COMB_CHAN_8       /* Indicates CH8 */
}
srch_comb_chan_type;

typedef enum
{
  CHIP_SYMBOL_64,   /* Indicates  64 chips per symbol */
  CHIP_SYMBOL_128,  /* Indicates 128 chips per symbol */
  CHIP_SYMBOL_256,  /* Indicates 256 chips per symbol */
  CHIP_SYMBOL_512   /* Indicates 512 chips per symbol */
}
srch_comb_chip_symbol_type;

/* Combiner Channel Data for srch_comb_set_chan() */
typedef struct
{
  boolean vp2;         /* Is VP2 mode active? */
  uint8   num_scch;    /* Number of SCCH channels */
  uint8   num_sch;     /* Number of SCH channels */
  uint8   num_pdcch;   /* Number of PDCCH channels */
}
srch_comb_chan_data;

#ifdef T_MSM6700

/*===========================================================================

FUNCTION SRCH_SCOMB_SYNC

DESCRIPTION    This function will read the symbol combiner count and slew
               the upper 80ms count to match the given input 80ms count,
               typically OTA system time.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The upper 7 bits of the symbol combiner count may change.

===========================================================================*/
extern void srch_scomb_sync
(
  uint32 systime_80ms
);
#endif /* T_MSM6700 */

/*===========================================================================

FUNCTION SRCH_COMB_ENABLE_DSP_INTS

DESCRIPTION    This function enables Interrupts to the DSP

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_comb_enable_dsp_ints
(
  boolean enable
);

/*===========================================================================

FUNCTION SRCH_COMB_SET_CHAN

DESCRIPTION    This function enables the necessary combiner channels, based
               on the channels being demodulated.

DEPENDENCIES   The srch_idle_info data must indicate the correct idle
               channel(s) as active/inactive.

RETURN VALUE   None.

SIDE EFFECTS   The combiner channel control register is updated.  Combiner
               channels are enabled/disabled.

===========================================================================*/
extern void  srch_comb_set_chan
(
  /* ptr to combiner channel data */
  srch_comb_chan_data * chan_data
);

/*===========================================================================

FUNCTION SRCH_COMB_CHAN_ENABLE

DESCRIPTION    This function enables the desired combiner code channel.

DEPENDENCIES   srch_dmod_reset should have been called.

RETURN VALUE   None.

SIDE EFFECTS   Combiner register is written.

===========================================================================*/
extern void  srch_comb_chan_enable
(
  uint8 chan_mask,
    /* Mask for speficying which channel to set */

  srch_is2000_system_enum_type
    /* The type of system we think we are on */
);


/*===========================================================================

FUNCTION SRCH_SCOMB_GET_REF_TIME

DESCRIPTION    This function reads the symbol combiner's time reference
      register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   The 16-bit time reference register value.  Note that this
      value is not exactly binary.  It is binary encoded, but its maximum
      value is 0xBFFF.  Each LSB is two PN chips.  The top two bits are
      the 26.666... ms phase within an 80ms superframe.

SIDE EFFECTS   The time reference latch-on-micro-write register is
      updated.

===========================================================================*/
extern uint16  srch_scomb_get_ref_time ( void );

/*===========================================================================

FUNCTION SRCH_SCOMB_GET_VSC_TIME

DESCRIPTION    This function reads the "virtual symbol combiner's"
      time reference register.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   The 16-bit time reference register value.  Note that this
      value is not exactly binary.  It is binary encoded.  Each LSB is
      two PN chips.  The value returned may correspond to a position
      larger than 80ms.

SIDE EFFECTS   The time reference latch-on-micro-write register is
      updated.

===========================================================================*/
extern uint16  srch_scomb_get_vsc_time ( void );

/*===========================================================================

FUNCTION SRCH_SCOMB_GET_MASTER_FING

DESCRIPTION    This function returns srch_comb_post_slam_tt_fing which is the
               finger that we should be time tracking as far as triage is
               concerned.

DEPENDENCIES   None.

RETURN VALUE   master finger or NO_FINGER

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_scomb_get_master_fing(void);

/*===========================================================================

FUNCTION SRCH_SCOMB_TTRK_ENA

DESCRIPTION    This function enables the symbol combiner time tracking
      circuits.  The symbol combiner can track time in two ways.
         1. It tries to keep its time aligned with a given reference
            finger (typically the earliest one) by slewing to match
            the finger's movements.  Finger time tracking has a strictly
            limited (by the CAI) rate.

         2. It can try to compensate for a fixed oscillator frequency error
            by slewing 1/8 chip every so often to compensate for the
            cumulative errors in time.  This compensation can be much
            larger than finger time tracking.

      This function enables only the first kind of time tracking.  In
      the production mobile and portable, the frequency tracking loop
      compensates the CHIPX8 clock itself, so there's no need for a
      time tracking correction.

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.
      If a slam is pending, the reference finger is stored, and
      assigned after the slam takes effect.

===========================================================================*/
extern void  srch_scomb_ttrk_ena
(
  uint16  fing
    /* Reference (earliest) finger, to be tracked by the combiner. */
);

/*===========================================================================

FUNCTION SRCH_SCOMB_TTRK_DIS

DESCRIPTION    This function disables the symbol combiner time tracking
      circuits.  It disables both kinds of combiner time tracking.

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.
      If a slam is pending, the time tracking is disabled after the
      slam takes effect.

===========================================================================*/
extern void  srch_scomb_ttrk_dis ( void );

/*===========================================================================

FUNCTION SRCH_SCOMB_ARM_SLAM

DESCRIPTION    This function arms the symbol combiner to
      align itself in time with ("slam to") the specified finger timing.
      The actual alignment occurs on the next PN roll of that finger.

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.

===========================================================================*/
extern void  srch_scomb_arm_slam
(
  uint16  f_ref
    /* The reference finger to align combiner timing with */
);

/*===========================================================================

FUNCTION       SRCH_SCOMB_QUICK_SLAM_TO_FING
DESCRIPTION    This function quick slams the symbol combiner immediately to
               a given finger in addition to a user specified slew.
DEPENDENCIES   The finger that is slammed to must not be slewing when this
               function is executed. Otherwise the slam will fail and timing
               will be in an undefined state.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_scomb_quick_slam_to_fing
(
  int8  fing,             /* finger to quick slam to */
  int32 slew,             /* additional slew value */
  int8  phase             /* phase to quick slam to */
);

/*===========================================================================

FUNCTION SRCH_SCOMB_SET_PN_PHS

DESCRIPTION    This function sets the symbol combiner to a desired
               phase80 value.

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Combiner control registers are written.

===========================================================================*/
extern void srch_scomb_set_pn_phs
(
  uint16 phase
    /* Phase of 80ms we want to set the combiner to */
);

/*===========================================================================

FUNCTION SRCH_SCOMB_SLAM_TIMELINE

DESCRIPTION    This function arms the symbol combiner to
      align itself in time with ("slam to") the specified finger timing.
      The actual alignment occurs on the next PN roll of that finger.
      The symbol combiner phase is set to the given phase after the
      slam takes effect.

DEPENDENCIES   srch_dmod_reset should have been previously called.
      Neither the symbol combiner nor the finger we are slamming to should
      be about to roll.

RETURN VALUE   None.

SIDE EFFECTS   Combiner control registers are written.

===========================================================================*/
extern void  srch_scomb_slam_timeline
(
  uint16  f_ref,
    /* The reference finger to align combiner timing with */

  uint16  new_phase,
    /* Desired phase after slam takes effect */

  uint16  new_offset
    /* Offset from system timing, in 2-chip units */
);

/*===========================================================================

FUNCTION SRCH_sCOMB_TIMELINE_ROLL

DESCRIPTION   This function completes the slam timeline on a PN Roll.  It
   should be called only from ISR context after a slam_timeline is armed.


DEPENDENCIES   srch_scomb_slam_time should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Combiner control registers are written.

===========================================================================*/
void srch_scomb_timeline_roll(void);

/*===========================================================================

FUNCTION SRCH_SCOMB_SET_CH0_SYM_SIZE

DESCRIPTION    This function sets the symbol size (walsh length) for demod
               chan 0

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.

===========================================================================*/
void srch_scomb_set_ch0_sym_size
(
  srch_comb_chip_symbol_type sym_size
);


/*===========================================================================

FUNCTION SRCH_PCOMB_ENA

DESCRIPTION    This function enables and disables puncturing and
      closed-loop power control.

DEPENDENCIES   BIO software switches.
               srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Various demod chip registers.

===========================================================================*/
extern void  srch_pcomb_ena
(
  boolean enable
    /* TRUE if we want to enable puncturing/pctl, else disable both */
);


/*   Misc combiner related functions */
/*===========================================================================

FUNCTION SRCH_PCOMB_SET_INIT_OFFSET

DESCRIPTION    This function sets the power combiner threshold of a finger.

DEPENDENCIES   srch_dmod_reset should have been previously called,

RETURN VALUE   None.

SIDE EFFECTS   Initial offset values are updated for F-CPCCH.

===========================================================================*/
extern void  srch_pcomb_set_init_offset
(
  /* Sector number, from 1-6 */
  uint8 sector_id,

  /* Sub-channel initial offset */
  uint8 init_offset
);

/*===========================================================================

FUNCTION       SRCH_GET_FING_SC_POS
DESCRIPTION    This function fills in the finger and symbol combiner PN
               counts for the given finger.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_get_fing_sc_pos
(
  int8    fing,       /* finger to quick slam to */
  int32 * f_cnt_ptr,  /* ptr to finger PN counter value in Cx8 */
  int32 * sc_cnt_ptr  /* ptr to symbol combiner count in Cx8 */
);

/*===========================================================================

FUNCTION SRCH_SET_ULPN_STATE

DESCRIPTION    This function sets the demod long code state register to a value.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod long code state register is written.

===========================================================================*/
extern void  srch_set_ulpn_state
(
  ulpn_type  lc_state
    /* PN long code state to load into the state register */
);

/*===========================================================================

FUNCTION SRCH_GET_ULPN_STATE

DESCRIPTION    This function reads the User Long PN Code state from
      the demod ASIC.

DEPENDENCIES   Calling this function asynchronously with the 26.666... ms
      boundary is dangerous.  If the 26.666... ms boundary passes while
      this function is reading the registers, a bogus value will be
      returned. Also, srch_dmod_reset should have been previously called,

RETURN VALUE   No formal return value,  but the User Long PN Code state
      is returned in a ulpn_type passed in by reference.  This state is
      the one that was valid at the last 26.666... ms boundary.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_get_ulpn_state
(
  ulpn_type   *ulpn
    /* Pointer to where to put the ULPN value read from the demod */
);

/*===========================================================================

FUNCTION SRCH_SET_CHAN_LC_MASK

DESCRIPTION    This function sets the demod long code mask register to a value
               for the specified channel.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   None.

SIDE EFFECTS   Demod long code mask register is written.

===========================================================================*/

extern void  srch_set_chan_lc_mask
(
  srch_comb_chan_type chan,
    /* Channel number to load */

  qword pn_mask
    /* PN long code mask to load into the mask register */
);

/*===========================================================================

FUNCTION SRCH_ROLL_CHECK

DESCRIPTION    Tests the hardware for a pending ROLL interrupt

DEPENDENCIES   None.

RETURN VALUE   TRUE if a ROLL has been signalled.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_roll_check( void ) ;

/*===========================================================================

FUNCTION       SRCH_COMB_GET_RTC_AT_SYNC80

DESCRIPTION    This function returns the RTC at the SYNC80 position of the
               symbol combiner.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern int32 srch_comb_get_rtc_at_sync80( void );

/*===========================================================================

FUNCTION       SRCH_COMB_INIT
DESCRIPTION    This function initializes the combiner related Hardware 
               variables

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_comb_init (void);

#endif /* SRCH_COMB_H */

#ifndef MUXMSM_H
#define MUXMSM_H
/*===========================================================================

           M S M   R E G I S T E R   A C C E S S   M A C R O S

                F O R   T H E   M U X   S U B S Y S T E M

                          H E A D E R    F I L E

DESCRIPTION
  This file contains macros to support interaction with the QUALCOMM
  MSM Register Set by the MUX subsystem (including the encoder and decoder).

Copyright (c) 2003 through 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/muxmsm.h#1 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/07   vlc/vm  Fix broken RPC in TX Gating Mode by changing value in
                   MUXMSM_AGC_CTL_MASK_DELAY macro (for feature
                   FEATURE_MDSP_SUPPORTS_TX_AGC only.)
01/09/07   vlc     Merged the following change to main line from SC2X branch.
                   Mainly, adding support for FEATURE_MDSP_SUPPORTS_TX_AGC.
           vm      Added the support for Firmware TX 
06/09/06   trc     Remove T_MSM6700 featurization
05/31/05   ejv     Include msm_help.h.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
10/27/04   jyw     added demod macros from the msm_drv.h
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   ejv     Added some DEM1X macros from msm_drv.h.
04/20/04   jrp     Fixed lint warnings.
04/09/04   bn      added macro for getting SCH NT_IO and WTD_PILOT
11/05/03   ejv     Added MUXMSM_FPC_IQACC_SCH_SCALE.
10/20/03   vlc     Undid changes to MUXMSM_GET_TD_STATUS_DONE.
09/25/03   vlc     Featurize MUXMSM_GET_TD_STATUS_DONE modification.
09/24/03   vlc     Modified MUXMSM_GET_TD_STATUS_DONE.
07/21/03   ejv     Added MUXMSM_DEMOD_SET_FRAME_OFFSET.
06/06/03   dna     Created file.

===========================================================================*/

#include "msm_help.h"

#ifdef  FEATURE_MDSP_SUPPORTS_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

/*===========================================================================

  COMBINER CONTROL REGs for CH4/CH5; used for GCH

===========================================================================*/
#define DEMOD_ENABLE_CH4_HW_COMBINING() \
    MSM_SET( DEM1X_CHANNEL4_FRAME, HW_EN )
#define DEMOD_DISABLE_CH4_HW_COMBINING() \
    MSM_CLEAR( DEM1X_CHANNEL4_FRAME, HW_EN )

#define DEMOD_ENABLE_CH5_HW_COMBINING() \
    MSM_SET( DEM1X_CHANNEL5_FRAME, HW_EN )
#define DEMOD_DISABLE_CH5_HW_COMBINING() \
    MSM_CLEAR( DEM1X_CHANNEL5_FRAME, HW_EN )

#define DEMOD_SET_MAC_RND_CH4( decoder ) \
    MSM_W_MASKED( DEM1X_CHANNEL4_FRAME, SOFTDEC_RND, (decoder) )
#define DEMOD_SET_MAC_RND_CH5( decoder ) \
    MSM_W_MASKED( DEM1X_CHANNEL5_FRAME, SOFTDEC_RND, (decoder) )

#define DEMOD_SET_CH4_SPR(spr)\
    MSM_W_MASKED( DEM1X_CHANNEL4_FRAME, SPR_MASK, (spr) )
#define DEMOD_SET_CH5_SPR(spr)\
    MSM_W_MASKED( DEM1X_CHANNEL5_FRAME, SPR_MASK, (spr) )

#define DEMOD_SET_SW_BETA_CH4( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL4_FRAME, SW_BETA, (scale)  )
#define DEMOD_SET_SW_BETA_CH5( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL5_FRAME, SW_BETA, (scale)  )


/*===========================================================================

  Fowrard Power Control

===========================================================================*/

/*==========================================================================
MACRO MUXMSM_FPC_ENABLE
MACRO MUXMSM_FPC_DISABLE

DESCRIPTION    These macros are used for enabling/disabling forward power
               control.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define MUXMSM_FPC_ENABLE( ) \
    MSM_SET( DEM1X_COMBINER_CTL, FPC_EN )

#define MUXMSM_FPC_DISABLE( ) \
    MSM_CLEAR( DEM1X_COMBINER_CTL, FPC_EN )

/*===========================================================================

MACRO MUXMSM_FPC_SEL

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_FPC_SEL( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_FRAME, FPC_SEL, (val) )

/*===========================================================================

MACRO MUXMSM_FPC_WALSH_GAIN

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define MUXMSM_FPC_WALSH_GAIN( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_FRAME, WALSH_GAIN, (val) )


/*===========================================================================

MACRO MUXMSM_FPC_IQACC_SCH_SCALE

DESCRIPTION     Writes the iqacc scale value for F-SCH.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define MUXMSM_FPC_IQACC_SCH_SCALE( val ) \
   mdsp_write( CDMA1X_schIqAccScale, (val) )

/*===========================================================================

MACRO MUXMSM_FPC_GET_HISTORY

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
/* There is no register in 6500 to report power control history */
#define MUXMSM_FPC_GET_HISTORY() \
   mdsp_read( CDMA1X_fpcHistory )

/*===========================================================================

MACRO MUXMSM_FPC_SCH_POS_DSP

DESCRIPTION
  Control the timing strobe to DSP

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define MUXMSM_FPC_SCH_POS_DSP( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_TIMING, SCH_POS_DSP, (val) )

/*===========================================================================

MACRO MUXMSM_FPC_SCH_POS
MACRO MUXMSM_FPC_POSITION1
MACRO MUXMSM_FPC_POSITION2

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_FPC_SCH_POS( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_TIMING, SCH_POS, (val) )
#define MUXMSM_FPC_POSITION1( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_TIMING, POS1, (val) )
#define MUXMSM_FPC_POSITION2( val ) \
   MSM_W_MASKED( DEM1X_FWD_PWR_CTL_TIMING, POS2, (val) )

/*===========================================================================

  Reverse Power Control

===========================================================================*/

/*===========================================================================

MACRO MUXMSM_RPC_GET_HISTORY

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
/* There is no register in 6500 to report power control history */
#define MUXMSM_RPC_GET_HISTORY() \
      mdsp_read( CDMA1X_rpcHistory )

/*===========================================================================

MACRO MUXMSM_RPC_SET_POWER_CTL

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
//lint -emacro(506, MUXMSM_RPC_SET_POWER_CTL)  Constant Value Boolean
#define MUXMSM_RPC_SET_POWER_CTL(power_ctl_pct, punc_pos_sel, pc_even)        \
    HWIO_OUTM(DEM1X_TRAFFIC_REV_PWR_CTL,                                      \
      HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,POWER_CTL_PCT) |                    \
      HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,PUNC_POS_SEL)  |                    \
      HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,PC_EVEN),                           \
      ((power_ctl_pct)?HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,POWER_CTL_PCT):0)| \
      ((punc_pos_sel) ?HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,PUNC_POS_SEL) :0)| \
      ((pc_even)      ?HWIO_FMSK(DEM1X_TRAFFIC_REV_PWR_CTL,PC_EVEN)      :0))

/*===========================================================================

                              DEMODULATOR

===========================================================================*/


/*==========================================================================
MACRO MUXMSM_SET_PILOT_GATING_MASK

DESCRIPTION

    This bitfield is used to program the mask value, which is used to
    determine when to disable the pc_symbol and not_pc_decision flags
    to the QDSP. A '1' is used to disable these signals to the QDSP during
    an entire PCG.
    Program these 4 bits as given below depending on the gating rate.

    Mask Value Gating Rate
    ---------- -----------
    0000       1
    1010       1/2
    1101       1/4

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define MUXMSM_SET_PILOT_GATING_MASK( mask ) \
        MSM_W_MASKED( DEM1X_COMBINER_CTL, PILOT_GATING_MASK, (mask) )

/*==========================================================================
MACRO MUXMSM_DEMOD_SET_FRAME_OFFSET

DESCRIPTION

    This bitfield is used to specify the frame offset in
    units of PCG (0-15).  It is used to compute the frame
    count and establish frame boundaries.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE   None

SIDE EFFECTS   Frame boundary may change.

=============================================================================*/

#define MUXMSM_DEMOD_SET_FRAME_OFFSET( offset ) \
        MSM_W_MASKED( DEM1X_FRAME_OFFSET, DATA, (offset) )

/*=========================================================================
MACRO DEMOD_ENABLE_MAC_ENGINE_CH1
MACRO DEMOD_DISABLE_MAC_ENGINE_CH1

DESCRIPTION    These macros enable/disable the MAC engine for CH1.

DEPENDENCIES   These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE   None

SIDE EFFECTS   None
==========================================================================*/

#define DEMOD_ENABLE_CH1_HW_COMBINING() \
    MSM_SET( DEM1X_CHANNEL1_FRAME, HW_EN )

#define DEMOD_DISABLE_CH1_HW_COMBINING() \
    MSM_CLEAR( DEM1X_CHANNEL1_FRAME, HW_EN )

/*=========================================================================
MACRO DEMOD_ENABLE_MAC_ENGINE_CH2
MACRO DEMOD_DISABLE_MAC_ENGINE_CH2
MACRO DEMOD_ENABLE_CHx_HW_COMBINING

DESCRIPTION    These macros enable/disable the MAC engine for CH2.

DEPENDENCIES   These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE   None

SIDE EFFECTS   None
==========================================================================*/

#define DEMOD_ENABLE_MAC_ENGINE()         DEMOD_ENABLE_CH2_HW_COMBINING()
#define DEMOD_ENABLE_MAC_ENGINE_FOR_CH2() DEMOD_ENABLE_CH2_HW_COMBINING()

#define DEMOD_ENABLE_CH2_HW_COMBINING() \
    MSM_SET( DEM1X_CHANNEL2_FRAME, HW_EN )

#define DEMOD_DISABLE_CH2_HW_COMBINING() \
    MSM_CLEAR( DEM1X_CHANNEL2_FRAME, HW_EN )

#define DEMOD_ENABLE_CH3_HW_COMBINING() \
    MSM_SET( DEM1X_CHANNEL3_FRAME, HW_EN )

#define DEMOD_DISABLE_CH3_HW_COMBINING() \
    MSM_CLEAR( DEM1X_CHANNEL3_FRAME, HW_EN )

/*===========================================================================

                                  DECODER

===========================================================================*/

/*===========================================================================

MACRO MUXMSM_GET_TD_STATUS_DONE

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_GET_TD_STATUS_DONE() \
        MSM_R_MASKED( TD_STATUS, DONE )

/*===========================================================================

MACRO MUXMSM_SET_MOD_MODE_1X

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_SET_MOD_MODE_1X() \
    HWIO_OUT(MOD_MODE, 0)






/*===========================================================================

                                  TX AGC

===========================================================================*/


/*===========================================================================

MACRO MUXMSM_AGC_CTL_MASK_DELAY

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef  FEATURE_MDSP_SUPPORTS_TX_AGC
#define MUXMSM_AGC_CTL_MASK_DELAY( delay )   \
        MSM_W_MASKED( TX_AGC_CTL, MASK_DELAY, (delay) );  \
        CDMA_MDSP_SET_TX_MASK_DATA_DELAY( delay + 1 )
#else
#define MUXMSM_AGC_CTL_MASK_DELAY( delay ) \
        MSM_W_MASKED( TX_AGC_CTL, MASK_DELAY, (delay) )
#endif


/*===========================================================================

MACRO MUXMSM_AGC_CTL_TX_MSM3100_MODE

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_AGC_CTL_TX_MSM3100_MODE() \
  MSM_SET( TX_AGC_CTL2, TX_MSM3100_MODE )






/*===========================================================================

                              MISCELANEIOUS

===========================================================================*/


/*===========================================================================

MACRO MUXMSM_GET_TIME_INT_PHASE

DESCRIPTION

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define MUXMSM_GET_TIME_INT_PHASE() \
          ((MSM_R_MASKED( DEM1X_TIME_INT_PHASE, TIME_INT2_PHASE )) & 0x3)

/*===========================================================================

MACRO MUXMSM_GET_SCH_RT_NT_IO

DESCRIPTION     This macro returns the supplemental weighted sqrt(Nt/Io)
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define MUXMSM_GET_SCH_RT_NT_IO( ) \
    mdsp_read(CDMA1X_suppWeightedNtMagAccFrame)

/*===========================================================================

MACRO MUXMSM_GET_SCH_WTD_PILOT

DESCRIPTION     This macro returns the supplemental weighted pilot magnitude
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define MUXMSM_GET_SCH_WTD_PILOT( ) \
    mdsp_read(CDMA1X_suppWeightedPilotAccFrame)

#endif /* MUXMSM_H */


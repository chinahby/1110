#ifndef MSM_DRV_H
#define MSM_DRV_H
/*===========================================================================

              M S M   R E G I S T E R   A C C E S S   M A C R O S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains macros to support interaction with the QUALCOMM
  MSM and QSC Register Sets.

Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/msm_drv.h#6 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/09   tjc     Added power control logging subpacket
03/12/09   vlc     Modified TX_SET_GAIN_ADJUST macro for FW-based TXAGC 
                   to a sequence which works better for MDSP FW.
02/23/09   jtm     Removed T_POSEIDON2 since this will only be supported 
                   in 1h09 branch
02/03/09   jtm     Added support for T_POSEIDON2
11/06/08   pk/aps  Corrected DEM1X_LATCH_ALL and DEM1X_LATCH_RTC_AND_SC
10/27/08   aps     Corrected QLIC featurizations and macros
10/23/08   jtm     Removed T_QUASAR
09/25/08   jtm     BIO_SW5_M is now checked inside TX_SET_GAIN_ADJUST instead 
                   of reading the current value of CDMA1X_cTxGainAdjOverride.
10/04/07   ljl     Supported multi-carriers.
05/17/07   pa      Corrected DEMOD_*_1X_RTC_GP? macros for 7600
04/24/07   sst     Update sample server macros for QSC60x5 == MSM7600
03/22/07   rkc     Rename QLIC_SET|GET_RTF_ADJUST() to SET|GET_RTF_ADJUST()
03/15/07   sst     Added SLEEP_CMD() for QSC60x5
02/23/07   sst     Add QLIC support
01/31/07   sst     Change T_QSC6055 to T_QSC60X5 to catch all tiers
12/19/06   sst     Merge the following change from 11.02.02B branch
 11/22/06  vm       Added the support for Firmware TX AGC
12/07/06   trc     Add SAMPLE_STOP_IMMED() macro for cross-target support
10/27/06   trc     MSM7600 support
10/23/06   sst     Merge from QSC6055 branched files
                   Update macros DEMOD_*_1X_RTC_GP# for QSC6055
10/09/06   trc     MSM7600 support
08/15/06   awj     Disallow inclusion of cagc.c for WINCE
07/27/06   jwob    Moved SLEEP register accessors macros to sclk.c
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
01/16/06   awj     Lint cleanup
07/29/05   grl     Added SVD only gated clocking macros.
05/31/05   rng     Merged from SHDR Sandbox.
03/25/05   bt      Changed DEMOD_1X_SELECT_RTC_ROLLOVER and
                   DEMOD_1X_SELECT_DEMOD_26MS_ROLLOVER() to
                   DEMOD_1X_SELECT_COMBINER_ROLL and
                   DEMOD_1X_SELECT_DELAYED_COMBINER_ROLL.
02/23/05   ejv     Added DEMOD_1X_SELECT_RTC_ROLLOVER() and
                   DEMOD_1X_SELECT_DEMOD_26MS_ROLLOVER() for 6700+.
01/25/05   ejv     Add T_MSM6800.
01/17/05   pj      Added COMBINER_CURRENT_TIMETRACK_FINGER(), returns
                   the finger being tracked by the combiner.
12/02/04   ejv     Added more T_QUASAR checks for non-existing registers.
11/15/04   ejv     Removed NOP macro COMBINER_SET_FREQUENCY_ADJUST.
11/12/04   ejv     Updates for 6700 sleep registers.
11/03/04   ejv     Removed finger _ALL macros.
09/30/04   sfm     Mainlined FEATURE_FINGER_DRIVER
09/28/04   ejv     Removed some unused macros and ported the interface
                   on some others.
09/23/04   kwo     Added demod macros to support logging
09/03/04   sfm     Finger Driver changes
09/03/04   ejv     Add COMBINER_SET_VALID_SECTORS.
08/09/04   ejv     Moved some DEM1X macros to muxmsm.h.
                   Changed macro name.
08/02/04   ejv     Temporarily do not disable accumulators for pre-6700.
                   New macros for power control support.
07/15/04   sfm     Removed FINGER_*, FINGERS_*, and FFE_* macros
07/14/04   ejv     Add T_QUASAR block.
07/08/04   ejv     Update FINGER_GET_LOCK_STATUS to consider both RX chains.
06/29/04   ejv     Update COMBINER_ENABLE_CPCCH for 6700.
06/14/04   ejv     Updated COMBINER_SLEW macros to NOT use shadow.
06/02/04   ejv     Update COMBINER_ENABLE/DISABLE_CH0 macros for 6700.
                   Write TdShift to MDSP for fingx when setting TD mode.
05/07/04   bt      Added DEMOD_1X_RTC_SLEEP_DISABLE and enabled FAST/NORMAL_AGC
                   macros.
04/29/04   ejv     Define FINGER_ACCUM_DISABLE_FRAME for 6700.
04/21/04   ejv     Changed int to uint16 for LINT.
04/20/04   ejv     Updated FINGER_DISABLE_CHx macros.  Add frame aligned
                   macros for accumulator enable/disable.  Added macro
                   FINGER_ENABLED to check if finger is enabled.
03/31/04   ejv     Modified DEM1X_COMBINER_GET_COUNT() and added
                   DEM1X_COMBINER_GET_80MS_COUNT() and
                   DEM1X_COMBINER_GET_SUB80MS_COUNT().
03/23/04   ejv     Changed FINGER_SET_WALSH to FINGER_SET_CHAN_WALSH.
                   Removed FINGER_SET_ACCUM_WALSH and
                   FINGER_SET_SUP_CODE_WALSH.
03/12/04   ejv     Load spreading mask for fingers on CH3.
03/11/04   ejv     Removed NOP macros QPCH_ENABLE/DISABLE.
03/08/04   ejv     Add NULL macros for CH3 enable/disable for 6500.
03/01/04   kwo     Lint Corrections
02/26/04   ejv     Removed unused 6700 macro.
02/25/04   ddh     Modified COMBINER_SLEW_SUB80MS macro to contain () on vars
02/23/04   sfm     Modified COMBINER_SLEW macro to appease lint.
02/23/04   ddh     Cleaned up lint disables/enables added one to SEARCH_SLEW
02/20/04   ejv     Added MSM_NOT_USED for unused params.
02/19/04   ejv     Added new macro COMBINER_SLEW_SUB80MS.  Updated interface
                   to COMBINER_SET_DELAY and COMBINER_SLEW.
02/18/04   ddh     More lint corrections.
                   Removed MSM_LOCK/MSM_UNLOCK from FINGER_RND_CH3_ALL Macro
                   Changed use of f for finger index to _macro_f.
02/12/04   sfm     Changed ul to UL for lint
02/12/04   ejv     Remove FEATURE_GPSONE from around macros.
02/12/04   ddh     Corrected lint errors.
01/28/04   ejv     Mainline T_MSM6500.
01/27/04   ejv     If fingers are not enabled, write to the IMMED register
                   rather then the FRAME register for chan config.
                   Add #define STOP_SAMPLE_MODE 0x0.
01/20/04   sfm     Added 6700 versions of COMBINER_SLEW() and
                   COMBINER_SET_DELAY() macros
01/16/04   ejv     Remove FEATURE_SEARCH2.
01/13/04   bt      Removed unused defines and made sw_beta macros names
                   consistent.
01/12/04   ejv     Updates for 6700.  Added FEATURE_SEARCH1.
12/09/03   ejv     Removed some unused macros.
11/05/03   bt      Cleaned up DEM1X_LATCH macros.
11/04/03   fu/aaj  Support for MMSE Metric and combined RSSI reading
10/24/03   bt      QPCH_ENABLE/DISABLE are not supported in MSM6500, include
                   DEM1X_CHANNEL0_IMMED:QPCH_EN in ARM/DISARM_OOK_DECODE.
10/06/03   ejv     Initial support for 6700.
10/03/03   ejv     Cleanup.  Added FINGER_SET_REP_FACTOR_CH2/ALL for 6500.
09/22/03   ejv     Update FINGER_DISABLE_CH1/2 for 6500.
09/11/03   aar     Fixed bug in macro FINGERS_SET_FREQ_COMB_GAIN().
09/09/03   ejv     Added FINGER_SET_CH1/2_SPR_ALL for 6500.  Changed CH1/2
                   updated from IMMED to FRAME for applicable registers.
08/21/03   ejv     Updates for 6500 firmware revision 0x0029.
08/20/03   fu/aaj  Added macros to pair accumulators for RX diversity
08/18/03   ejv     Modify order of operation in FINGER_DISABLE_CH1/2.
08/04/03   ejv     Update DEM1X_RTC_GET_COUNT for 6100.
07/16/03   ejv     Additional 6500 updates for setting the spreading factor
                   for various walsh lengths in the fingers.
07/20/03   bt      More T_MSM6500 updates.
07/02/03   ejv     Update FINGER_ENABLE_CHx macros for 6500.
06/19/03   ejv     T_MSM6500 updates.
06/17/03   ejv     T_MSM6500 updates.
06/06/03   ejv     Fix TRK_LO_ADJ_MICRO_OVERRIDE_EN( ) for 6100.
04/29/03   bt      Add SEARCH_ENABLE/DISABLE_DEMOD_BYPASS for Cougar.
04/28/03   ejv     Mainline 6100 macros.
03/26/03   bt      Surround writes to VD_MODE with clock gating macros.
02/18/03   ejv     Additional support for T_MSM6500.
02/05/03   gs/kwo  Rel A support for ZZ2.
01/24/03   ejv     Replaced FINGER_SET_BINARY_OFFSET with ..._PN_OFFSET.
01/21/03   ejv     Added T_MSM6500.
12/31/02   ejv     Added RTC section and modified RTC macro names.
11/04/02   rm      Added new macro to check the dsp active status.
10/31/02   bt      add micro processor dump macros
10/02/02   gs      Merge with MSM_SRCH.08.06.42
09/11/02   va      Added macros to read information from MDSP.
07/30/02   gs      Initial merge into perforce for cougar.
07/16/02   ejv     Removed redundant macro.
07/02/02   dlb     Removed unneeded '#error' statement.  Enclosed macros in
                   "do{}while(0)" block to simulate single statement.
06/26/02   dlb     Added macros for turning on/off Dynamic Clock Gating.  Added
                   workaround for MSM6050 DEC1 problem.  Read SLEEP_COUNT if
                   MSM6050_A1.
06/24/02   ejv     Added macro back in which had been inadvertently removed.
06/13/02   abh     Modified macros to read Trk Lo PDMs for MSM6050 Rev2.0
                   compatibility.
05/30/02   ejv     Mainlined T_MSM5000.
03/21/02   ht      Added DMOD_SET_MAC_SEQ_REP_* defs for MSM6050.
03/14/02   rm      Fixed compiler warning.
03/13/02   abh     Fixed the macro TRK_LO_ADJ_SET_FREQ_ERR_8P4.
03/12/02   rm      Work around for MSM6050, where the sleep counter can't
                   be read.
02/11/02   rm      Added changes for MSM6050.
02/11/02   rm      Added macro to read the tx gain adjust.
02/07/02   ejv     Added new macros for MSM6000.
01/25/02   bgc     Updated T_MSM6000.
01/24/02   ejv     Added FEATURE_INVERTED_TRK_LO_POLARITY.
01/11/02   ejv     Added T_MSM6000.
09/26/01   ejv     Added DEMOD_ENABLE_MAC_ENGINE().
09/25/01   ejv     Added SEARCH_SET_WALSH.
08/07/01   aaj     Added QPCH ULPN mask changes for MSM5500
06/13/01   rm      Clear DSP_SUP_OFF when enabling MAC Engine.
05/11/01   ejv     Removed T_MSM31.
03/07/01   ejv     Added clear/set of LOCK_RSSI_EXT_RANGE for chip/symbol macros.
03/01/01   rm      Check for DIP Switch Setting before enable RPC.
02/16/01   dgy     Modified macro COMBINER_SELECT_CH0_512_CHIP_SYMBOL to
                   support CCI decoding.
01/31/01   rm      Added/fixed macros for finger merge detect in hw.
01/22/01   ejv     Added QPCH_SET_ION_QON for MSM5100.
01/18/01   ejv/rm  Added new macros for MSM5100 support.
12/14/00   ejv     Fixed shadowing for QOF enable/disable macros.
12/13/00   hrk     Removed #ifdef FEATURE_GPSONE around Macro CAGC_WRITE_8P2.
10/31/00   rm      Enable Hardware duty cycle fix for MSM5105.
10/25/00   rm      Initialization's for MSM5105. Defined New Macros.
09/21/00   day     Backed out changes to I and Q OFFSET macros to correct
                   argument list error.
09/10/00   ejv     Added FINGER_FCH_QOF_ENABLE, FINGER_FCH_QOF_DISABLE,
                   FINGER_SCH_QOF_ENABLE, and FINGER_SCH_QOF_DISABLE.
07/31/00   ejv     Modified TRK_LO_ADJ_SET_OVERRIDE.
07/25/00   day     Code cleanup on I offset and Q offset macros.
07/18/00   eh      Added macro to initialize viterbi decoder for MSM5000.
07/13/00   yll/day Added support for "Tune Away Without Drop" for FEATURE_GPSONE -
                   Fixed I_OFFSET_GET_ACCUMULATOR and Q_OFFSET_GET_ACCUMULATOR macros.
                   Added macro FING_COMB_OFFSET_READ.
06/29/00   abh/day Added MSM3300 support - FEATURE_GPSONE.  Changed SRCH2_CLK_ENA
                   to enable only Srch2 Block(shadowed writes).
06/12/00   ejv     Added DEMOD_ENABLE/DISABLE_FPC and FINGER_SET_TTL_FAST/SLOW.
06/06/00   bgc     Added revision check for DEMOD_ENABLE_MAC_ENGINE_FOR_SCH.
03/17/00   ajn     Sleep init now assumes DUTY_CYCLE fix for PPanther.
03/02/00   ajn     Added "slow clock" Frequency Error Estimator (FEE) support
02/24/00    eh     Defined SEARCH_ENABLE_DEMOD_BYPASS and
                   SEARCH_DISABLE_DEMOD_BYPASS.
01/27/00   ejv     Define SRCH2_CLK_ENA for MSM3100 only.
01/14/00   ejv     Added macros for adjusting the AGC loop gain constant.
01/13/00   ejv     Added macros for uP int. bypass and sleep controller read.
12/13/99   ejv     Added comments and masks for some macros.
12/06/99   ajn     Added Decoder Zero-Initialize modes
11/29/99   ejv     Mask the bottom 2 bits in SRCH_GET_STATUS_DUMP.
11/10/99   ejv     Added FEATURE_SEARCH2 macros.
10/27/99   ajn     Added QPCH related macros.
10/21/99   rm      Enable DSP for CH2 SCCH demodulation.
10/20/99   dgy     Added new macro to configure QOF.
09/24/99   rm      Added Macros to Configure MAC ENGINE.
09/13/99   rm      Added new macros to control ULPN descramble for RC4.
08/11/99   rm      Added new macros to enable/disable I or Q channel from the
                   Demodulator.
08/11/99   rm      Added new macros for Symbol combiner symbol accumulation.
08/06/99   ejv     Merged jcw's initial MSM5000 support.
07/23/99   ajn     First cut of file.

===========================================================================*/

#include "customer.h"

#include "bio.h"
#include "dmod.h"
#include "hw.h"
#include "mdspext.h"
#include "msm_help.h"

#ifndef FEATURE_WINCE
#include "cagc.h"
#endif /* FEATURE_WINCE */

#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

/*===========================================================================

                            C O N S T A N T S

===========================================================================*/

#define DEM1X_ZERO                                          0x0
#define MSMDRV_SET                                          0xFFFF
#define CLEAR                                               0x0000
#define MSMDRV_ENABLE                                       0xFFFF
#define DISABLE                                             0x0000
#define BUFFER0                                             0
#define BUFFER1                                             1
#define ANTENNA0                                            0x0
#define ANTENNA1                                            0x1
#define LOCKED                                              0xFFFF
#define NOT_LOCKED                                          0x0

#define COMBINER_80MS_CX8_MASK                              0x00FFFFFUL
#define COMBINER_80MS_COUNT_MASK                            0x7F00000UL

/* 256 * PI / 2 = 0x192 - part of the necessary scaling factor to
   convert the new magnitude values read from the MDSP in 2 of
   the macros below */
#define MAG_SCALE                                           0x192

#ifdef T_MSM6700
/* 6700 can support 6 cells + NULL cell for power control combining */
#define NUM_PWR_CTRL                                        7
#else
/* 6500 can support 3 cells + NULL cell for power control combining */
#define NUM_PWR_CTRL                                        4
#endif /* T_MSM6700 */

/*===========================================================================

                              RTC REGISTERS

===========================================================================*/

/*===========================================================================

MACRO DEMOD_RESET_1X_RTC
MACRO DEMOD_RESET_HDR_RTC
MACRO DEMOD_RESET_GPS_RTC

DESCRIPTION     There macros set the RTC count for which a general
                purpose interrupt will be generated.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    An interrupt may soon be triggered.

===========================================================================*/
#define DEMOD_RESET_1X_RTC( ) \
    MSM_PULSE( RTC_CTL, RESET_1X )

#define DEMOD_RESET_HDR_RTC( ) \
    MSM_PULSE( RTC_CTL, RESET_HDR )

#define DEMOD_RESET_GPS_RTC( ) \
    MSM_PULSE( GPS_RTC_CTL, RESET_GPS )

/*===========================================================================

MACRO DEMOD_1X_RTC_SLEEP

DESCRIPTION     This macro enables a strobe to be generated on the
                matching RTC count which will put the MSM to sleep. Please
                note that after sleep is entered (or on wakeup)
                DEMOD_1X_RTC_SLEEP_DISABLE should be called to prevent
                additional sleep trigger events.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The MSM will enter sleep mode when the RTC count is matched.

===========================================================================*/
#define DEMOD_1X_RTC_SLEEP( count ) \
    MSM_W_MASKED( RTC_SLEEP, RTC_SLEEP, (count) ); \
    MSM_SET( RTC_CTL, SLEEP_EN )

/*===========================================================================

MACRO DEMOD_1X_RTC_SLEEP_DISABLE

DESCRIPTION     This macro disables a strobe to be generated on the
                matching RTC count which will put the MSM to sleep.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define DEMOD_1X_RTC_SLEEP_DISABLE( )  \
    MSM_CLEAR( RTC_CTL, SLEEP_EN )

/*===========================================================================

MACRO DEMOD_SET_1X_WAKEUP_RTC

DESCRIPTION     This macro sets the RTC value to be loaded upon wakeup.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The RTC is updated.

===========================================================================*/
#define DEMOD_SET_1X_WAKEUP_RTC( count ) \
    MSM_W_MASKED( RTC_LOAD, RTC_LOAD, (count) )

/*===========================================================================

MACRO DEMOD_GET_1X_HDR_RTC_DIFF

DESCRIPTION     This macro returns the difference between 1X and HDR
                RTC counts.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The difference between 1x and HDR RTCs in 2's complement.
                The value is positive when 1X RTC is larger then HDR RTC.

SIDE EFFECTS    None.

===========================================================================*/
#define DEMOD_GET_1X_HDR_RTC_DIFF( count ) \
    MSM_R_MASKED( RTC_DIFF_1XHDR, RTC_DIFF )

/*===========================================================================

MACRO DEMOD_SET_1X_RTC_GP1
MACRO DEMOD_SET_1X_RTC_GP2

DESCRIPTION     These macros set the RTC count for which a general
                purpose interrupt will be generated.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    An interrupt may soon be generated if so enabled.

===========================================================================*/
#define DEMOD_SET_1X_RTC_GP1( count ) \
    MSM_W_MASKED( RTC_GP_COMPARE1, RTC_GP_COMPARE1, count )

#define DEMOD_SET_1X_RTC_GP2( count ) \
    MSM_W_MASKED( RTC_GP_COMPARE2, RTC_GP_COMPARE2, count )

/*===========================================================================

MACRO DEMOD_ENABLE_1X_RTC_GP1
MACRO DEMOD_DISABLE_1X_RTC_GP1
MACRO DEMOD_ENABLE_1X_RTC_GP2
MACRO DEMOD_DISABLE_1X_RTC_GP2

DESCRIPTION     These macros enable/disable the interrupt which will
                be generated when the RTC count matches the programmed
                value.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    An interrupt will be generated when the specified value
                is matched.

===========================================================================*/
#if !defined( T_QSC60X5 ) && !( defined( T_MSM7600 ) )

#define DEMOD_ENABLE_1X_RTC_GP1( ) \
    MSM_SET( RTC_CTL, COMPARE1_MASK )

#define DEMOD_DISABLE_1X_RTC_GP1( ) \
    MSM_CLEAR( RTC_CTL, COMPARE1_MASK )

#define DEMOD_ENABLE_1X_RTC_GP2( ) \
    MSM_SET( RTC_CTL, COMPARE2_MASK )

#define DEMOD_DISABLE_1X_RTC_GP2( ) \
    MSM_CLEAR( RTC_CTL, COMPARE2_MASK )

#else /* !T_QSC60X5 && !T_MSM7600 */

#define DEMOD_ENABLE_1X_RTC_GP1( ) \
    MSM_SET( RTC_INT_ENABLE, RTC_CMP1 )

#define DEMOD_DISABLE_1X_RTC_GP1( ) \
    MSM_CLEAR( RTC_INT_ENABLE, RTC_CMP1 )

#define DEMOD_ENABLE_1X_RTC_GP2( ) \
    MSM_SET( RTC_INT_ENABLE, RTC_CMP2 )

#define DEMOD_DISABLE_1X_RTC_GP2( ) \
    MSM_CLEAR( INT_ENABLE, RTC_CMP2 )

#endif /* !T_QSC60X5 && !T_MSM7600 */

/*===========================================================================

MACRO DEMOD_ENABLE_RTC_ROLLOVER_INT
MACRO DEMOD_DISABLE_RTC_ROLLOVER_INT

DESCRIPTION     These macros enabled/disable the generation of an
                interrupt on the rollover of the RTC.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    An interrupt will be generated when the RTC rolls over
                if "enabled".

===========================================================================*/
#define DEMOD_ENABLE_RTC_ROLLOVER_INT( ) \
    MSM_SET( RTC_CTL, ROLLOVER_INT_EN )

#define DEMOD_DISABLE_RTC_ROLLOVER_INT( ) \
    MSM_CLEAR( RTC_CTL, ROLLOVER_INT_EN )

/*===========================================================================

MACRO DEMOD_GET_GPS_RTC

DESCRIPTION     This macro reads the latest GPS reference count value
                from hardware.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The latest GPS RTC.

SIDE EFFECTS    None.

===========================================================================*/
#define DEMOD_GET_GPS_RTC( ) \
    MSM_R( GPS_RTC_CNT, GPS_RTC_CNT ) & \
    (HWIO_FMSK(GPS_RTC_CNT##,##CNT_1MS##) | \
     HWIO_FMSK(GPS_RTC_CNT##,##CNT_CX8##))

/*===========================================================================

MACRO DEMOD_1X_RTC_SLAM

DESCRIPTION     These macros adjusts the current reference count value
                by an offset specified in chipx8 resolution

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The RTC experiences an instant change in value.

===========================================================================*/
#define DEMOD_1X_RTC_SLAM( slam ) \
    MSM_W_MASKED( RTC_OFFSET, RTC_OFFSET, (slam) ); \
    MSM_SET( RTC_CTL, RTC_ADJUST )

/*===========================================================================

MACRO DEMOD_GET_1X_RTC

DESCRIPTION     This macro reads the latest reference count value
                from hardware.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The latest RTC.

SIDE EFFECTS    None.

===========================================================================*/
#define DEMOD_GET_1X_RTC( ) \
    MSM_R_MASKED( RTC_CNT_1X, RTC_CNT_1X )

/*===========================================================================

MACRO DEMOD_GET_1X_80MS_RTC

DESCRIPTION     This macro reads the latest reference count value that
                was latched at CDMA sync80m

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The RTC as of the last 80ms strobe.

SIDE EFFECTS    None.

===========================================================================*/
#define DEMOD_GET_1X_80MS_RTC( ) \
    MSM_R_MASKED( CDMA_RTC_SYNC, CDMA_RTC_SYNC )

#ifdef T_MSM6700
/*===========================================================================

MACRO DEMOD_1X_SELECT_COMBINER_ROLL
MACRO DEMOD_1X_SELECT_DELAYED_COMBINER_ROLL

DESCRIPTION     These macros select the 1x PN rollover to either be from
                when the combiner rolls over or 128 chips delayed from
                that event.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The PN roll timing chould change if this bit is being
                flipped from it's previous value.

===========================================================================*/
#define DEMOD_1X_SELECT_COMBINER_ROLL( ) \
    MSM_CLEAR( RTC_INT_SELECT, ROLLOVER_1X_SEL )

#define DEMOD_1X_SELECT_DELAYED_COMBINER_ROLL( ) \
    MSM_SET( RTC_INT_SELECT, ROLLOVER_1X_SEL )
#endif /* T_MSM6700 */





/*===========================================================================

                        DEMODULATION REGISTERS

===========================================================================*/



/*===========================================================================

MACRO DEMODULATOR_RESET

DESCRIPTION     This macro resets the demodulator circuitry.

DEPENDENCIES    CHIPX8 must be oscillating.

RETURN VALUE    None.

SIDE EFFECTS    Too numerous to mention.

===========================================================================*/

#define DEMODULATOR_RESET( )    \
    MSM_PULSE( DEM1X_RESET, RESET )

/*===========================================================================

MACRO MDSP_ACTIVE

DESCRIPTION     This macro  checks the dsp active status.

DEPENDENCIES    MDSP image must be downloaded.

RETURN VALUE    None.

SIDE EFFECTS    Too numerous to mention.

===========================================================================*/
#define MDSP_ACTIVE( ) \
    mdsp_read( MDSP_dspActive )


/*==========================================================================
MACRO DEMOD_REP_FACTOR

DESCRIPTION    This macro sets the repetition factor in the demodulator.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_REP_FACTOR( factor ) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, REP_FACTOR, (factor) )

/*==========================================================================
MACRO DEMOD_SET_MAC_SEQ_REP_x

DESCRIPTION    These macros set the FFE_FINGER_CH2_TD_SIGN value
               for multi-frame interleaving.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_SET_MAC_SEQ_REP_1( ) \
    DEMOD_REP_FACTOR( 0 )

#define DEMOD_SET_MAC_SEQ_REP_2( ) \
    DEMOD_REP_FACTOR( 1 )

#define DEMOD_SET_MAC_SEQ_REP_4( ) \
    DEMOD_REP_FACTOR( 2 )

/*==========================================================================
MACRO DEMOD_ENABLE_CH0_RC4
MACRO DEMOD_DISABLE_CH0_RC4

DESCRIPTION    These macros enable/disable indicate that channel 0 is in
               RC4.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_ENABLE_CH0_RC4( ) \
    MSM_SET( DEM1X_CHANNEL0_IMMED, RC4 )

#define DEMOD_DISABLE_CH0_RC4( ) \
    MSM_CLEAR( DEM1X_CHANNEL0_IMMED, RC4 )

#define DEMOD_ENABLE_CH0_RC4_FRAME( ) \
    MSM_SET( DEM1X_CHANNEL0_FRAME, RC4 )

#define DEMOD_DISABLE_CH0_RC4_FRAME( ) \
    MSM_CLEAR( DEM1X_CHANNEL0_FRAME, RC4 )

/*==========================================================================
MACRO DEMOD_SET_CH0_TD_SIGN

DESCRIPTION    This macro sets the CH0_TD_SIGN value as requested for various
               RCs.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_SET_CH0_TD_SIGN( value ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_IMMED, TD_SIGN, (value) )

#define DEMOD_SET_CH0_TD_SIGN_FRAME( value ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_FRAME, TD_SIGN, (value) )


/*==========================================================================
MACRO DEMOD_SET_CH1_TD_SIGN

DESCRIPTION    This macro sets the CH1_TD_SIGN value as requested for various
               RCs.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/

#define DEMOD_SET_CH1_TD_SIGN( value ) \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, TD_SIGN, (value) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_128_64_CHIPS_CH1

DESCRIPTION  This macro sets MAC Engine CH1 rounding factors for spreading
              rate of 128 or 64 chips.

              For 6500 the rounding factors are moved as follows:

              MAC_RND_CH1 COMB_RND2 -> DEM1X_CHANNEL1 SOFTDEC_ACCUM_RND
              MAC_RND_CH2 COMB_RND2 -> DEM1X_CHANNEL2 SOFTDEC_ACCUM_RND
              MAC_RND_CH1 COMB_RND3 -> DEM1X_CHANNEL1 SOFTDEC_RND
              MAC_RND_CH2 COMB_RND3 -> DEM1X_CHANNEL2 SOFTDEC_RND

              MAC_RND_CH1 COMB_RND1 -> DEM1X_Fn_HW_CH1 RND
              MAC_RND_CH2 COMB_RND1 -> DEM1X_Fn_HW_CH2 RND

              Because the value that was in COMB_RND1 is now per-finger
              for MSM6500, this value is no longer written by these
              macros, but must be written per-finger, through the new
              macros FINGER_RND_CH1 and FINGER_RND_CH2.

              Also, the mapping from walsh length or decoder type to actual
              rounding factor is no longer done in the hardware.  The
              translation is as follows:

              DEM1X_CHANNEL1/DEM1X_CHANNEL2
              walsh length    frame length    SOFTDEC_ACCUM_RND
              128             80ms            1
              128             40,20ms         0
              64              any             0
              32              any             1
              16              any             1
              8               any             2
              4               any             2

              encoder type    truncation      SOFTDEC_RND
              convolutional   10 bits         0
              turbo           8 bits          1

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_128_CHIPS_80MS_FRAME_CH1(decoder) \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

#define DEMOD_SET_MAC_RND_128_64_CHIPS_CH1(decoder) \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 0); \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_32_CHIPS_CH1

DESCRIPTION  This macro sets MAC Engine CH1 rounding factors for spreading
              rate of 32 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_32_CHIPS_CH1(decoder ) \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_128_64_CHIPS

DESCRIPTION  This macro sets MAC Engine CH2 rounding factors for spreading
              rate of 128 or 64 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/


#define DEMOD_SET_MAC_RND_128_CHIPS_80MS_FRAME(decoder) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

#define DEMOD_SET_MAC_RND_128_64_CHIPS(decoder) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 0); \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_32_16_CHIPS

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
              rate of 32 or 16 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_32_16_CHIPS(decoder ) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 1); \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

/*=========================================================================
MACRO DEMOD_SET_MAC_RND_8_4_CHIPS

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
             rate of 8 or 4 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_8_4_CHIPS(decoder ) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_ACCUM_RND, 2); \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SOFTDEC_RND, (decoder) )

#ifdef T_MSM6700
/*=========================================================================
MACRO DEMOD_SET_MAC_RND_64_CHIPS_CH3

DESCRIPTION  This macro sets MAC Engine rounding factors for spreading
             rate of 64 chips.

RETURN VALUE  None

SIDE EFFECTS  None
==========================================================================*/

#define DEMOD_SET_MAC_RND_64_CHIPS_CH3(decoder) \
     MSM_W_MASKED( DEM1X_CHANNEL3_FRAME, SOFTDEC_ACCUM_RND, 0); \
     MSM_W_MASKED( DEM1X_CHANNEL3_FRAME, SOFTDEC_RND, (decoder) )
#endif /* T_MSM6700 */

/*=====================================================================
MACRO DEMOD_SET_CHx_SPR

DESCRIPTION These macros control the spreading rate of the demodulator
            channels 1 & 2.

RETURN VALUE None

SIDE EFFECTS None

======================================================================*/

#define DEMOD_SET_CH1_SPR(spr)\
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SPR_MASK, (spr) )

#define DEMOD_SET_CH2_SPR(spr)\
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SPR_MASK, (spr) )

#ifdef T_MSM6700
#define DEMOD_SET_CH3_SPR(spr)\
    MSM_W_MASKED( DEM1X_CHANNEL3_FRAME, SPR_MASK, (spr) )
#endif /* T_MSM6700 */

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH0

DESCRIPTION  This macro sets the FCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 0.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH0_IMMED( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_IMMED, SW_BETA, scale  )

#define DEMOD_SET_SW_BETA_CH0( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_FRAME, SW_BETA, scale  )

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH1

DESCRIPTION  This macro sets the FCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 1.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH1_IMMED( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL1_IMMED, SW_BETA, scale  )

#define DEMOD_SET_SW_BETA_CH1( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL1_FRAME, SW_BETA, scale  )

/*=====================================================================
MACRO DEMOD_SET_SW_BETA_CH2

DESCRIPTION  This macro sets the SCH frame scaling factor for demod soft
             decison symbols to the SVD, for Channel 2.

RETURN VALUE  None

SIDE EFFECTS  This scaling factor takes effect at the next frame boundary.
=========================================================================*/

#define DEMOD_SET_SW_BETA_CH2( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL2_FRAME, SW_BETA, (scale)  )

#ifdef T_MSM6700
#define DEMOD_SET_SW_BETA_CH3( scale ) \
    MSM_W_MASKED( DEM1X_CHANNEL3_FRAME, SW_BETA, (scale)  )
#endif /* T_MSM6700 */

/*==========================================================================
MACRO DEMOD_ENABLE_Q_CHANNEL

DESCRIPTION    This macro sets the demodulator to send Q channel soft decisions
               to the Viterbi Decoder. Enable Q Channel when receiving RC3, RC4
               and RC5 Traffic Channel Frames.

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define DEMOD_ENABLE_Q_CHANNEL( ) \
    MSM_CLEAR( DEM1X_COMBINER_CTL, Q_CHAN_OFF )

/*==========================================================================
MACRO DEMOD_DISABLE_Q_CHANNEL

DESCRIPTION    This macro stops the demodulator from sending Q channel soft decisions
               to the Viterbi Decoder. Disable Q channel when receiving Sync, Paging,
               RC1 & RC2 traffic channel frames

DEPENDECIES    These macros must be called from inside the context of a
               MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.


RETURN VALUE   None

SIDE EFFECTS   None.

=============================================================================*/

#define DEMOD_DISABLE_Q_CHANNEL( ) \
    MSM_SET( DEM1X_COMBINER_CTL, Q_CHAN_OFF )

/*===========================================================================

MACRO DEMOD_GET_PC_FILT_GAIN

DESCRIPTION     Read the PC Filter Gain from MDSP.

DEPENDENCIES

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_PC_FILT_GAIN(gain) \
    gain = mdsp_read( CDMA1X_pcFiltGain )

/*===========================================================================

MACRO DEMOD_SET_PC_FILT_GAIN

DESCRIPTION     These macros sets the filter gain for the forward link PC bit

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_SET_PC_FILT_GAIN(gain) \
    mdsp_write ( CDMA1X_pcFiltGain, (gain) )

/*===========================================================================

MACRO DEMOD_GET_NT_IO_FILT_GAIN

DESCRIPTION

DEPENDENCIES

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_NT_IO_FILT_GAIN(gain) \
    gain = mdsp_read ( CDMA1X_ntIoFiltGain )

/*===========================================================================

MACRO DEMOD_SET_NT_IO_FILT_GAIN

DESCRIPTION     This macro sets the filter gain for the forward link Nt/Io

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_SET_NT_IO_FILT_GAIN(gain) \
    mdsp_write ( CDMA1X_ntIoFiltGain, (gain) )

/*===========================================================================

MACRO DEMOD_GET_EB_NO_TARGET_FCH
      DEMOD_GET_EB_NO_TARGET_SCH

DESCRIPTION

DEPENDENCIES

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_EB_NO_TARGET_FCH(target) \
    target = mdsp_read( CDMA1X_ebNtMagTargetFch )

#define DEMOD_GET_EB_NO_TARGET_SCH(target) \
    target = mdsp_read( CDMA1X_ebNtMagTargetSch )

/*===========================================================================

MACRO DEMOD_SET_EB_NO_TARGET_FCH
      DEMOD_SET_EB_NO_TARGET_SCH

DESCRIPTION     This macro sets the fundamental and supplemental Eb/No target

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_SET_EB_NO_TARGET_FCH(target) \
    mdsp_write ( CDMA1X_ebNtMagTargetFch, (target) )

#define DEMOD_SET_EB_NO_TARGET_SCH(target) \
    mdsp_write ( CDMA1X_ebNtMagTargetSch, (target) )

/*===========================================================================

MACRO DEMOD_GET_SCH_IQ_ACC

DESCRIPTION     This macro returns the IQ accumulation of channel 2
                over a frame.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_SCH_IQ_ACC( ) \
    mdsp_read(CDMA1X_ch2AccFrame)

/*===========================================================================

MACRO DEMOD_GET_PC_ACC

DESCRIPTION     This macro returns the PC bit accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_PC_ACC( ) \
    mdsp_read(CDMA1X_pcBitAccFrame)

/*===========================================================================

MACRO DEMOD_GET_FCH_NT_IO

DESCRIPTION     This macro returns the fundamental weighted Nt/Io
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_FCH_NT_IO( ) \
    ((MAG_SCALE * (int4)mdsp_read(CDMA1X_fundWeightedNtMagAccFrame) * \
                  (int4)mdsp_read(CDMA1X_fundWeightedNtMagAccFrame))>>16)

/*===========================================================================

MACRO DEMOD_GET_SCH_NT_IO

DESCRIPTION     This macro returns the supplemental weighted Nt/Io
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEMOD_GET_SCH_NT_IO( ) \
    ((MAG_SCALE * (int4)mdsp_read(CDMA1X_suppWeightedNtMagAccFrame) * \
                  (int4)mdsp_read(CDMA1X_suppWeightedNtMagAccFrame))>>16)

/*===========================================================================

MACRO DEMOD_FPC_GET_HISTORY

DESCRIPTION     These macros returns a sample of the FPC history.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6500

#define DEMOD_FPC_GET_HISTORY() \
   mdsp_read( CDMA1X_fpcHistory )

#endif

/*===========================================================================

MACRO DEMOD_FPC_GET_HISTORY_VALID

DESCRIPTION     These macros returns whether the FPC history is valid.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6500

#define DEMOD_FPC_GET_HISTORY_VALID() \
   mdsp_read( CDMA1X_fpcHistoryValid )

#endif

/*===========================================================================

MACRO DEMOD_RPC_GET_HISTORY

DESCRIPTION     These macros returns a sample of the RPC history.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6500

#define DEMOD_RPC_GET_HISTORY() \
   mdsp_read( CDMA1X_rpcHistory )

#endif

/*===========================================================================

MACRO DEMOD_RPC_GET_HISTORY_VALID

DESCRIPTION     These macros returns whether the RPC history is valid.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6500

#define DEMOD_RPC_GET_HISTORY_VALID() \
   mdsp_read( CDMA1X_rpcHistoryValid )

#endif

/*===========================================================================

MACRO DEMOD_RPC_GET_CELL_HISTORY

DESCRIPTION     These macros return a sample of the RPC history for the 
                specific cell.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6500

#ifdef CDMA1X_rpcHistoryCellA

#define DEMOD_RPC_GET_CELL_HISTORY( cell ) \
   mdsp_read( CDMA1X_rpcHistoryCellA + ( ( cell ) * sizeof(uint16) ) )

#else /* !CDMA1X_rpcHistoryCellA */

#define DEMOD_RPC_GET_CELL_HISTORY( cell )  0

#endif /* CDMA1X_rpcHistoryCellA */

#endif

/*===========================================================================

MACRO DEMOD_GET_FCH_WTD_PILOT


DESCRIPTION     This macro returns the fundamental weighted pilot magnitude
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_MSM6500
#define DEMOD_GET_FCH_WTD_PILOT( ) \
    mdsp_read(CDMA1X_fundWeightedPilotAccFrame)
#endif

/*===========================================================================

MACRO DEMOD_GET_FCH_RT_NT_IO


DESCRIPTION     This macro returns the fundamental weighted sqrt(Nt/Io)
                accumulation over a frame

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_MSM6500
#define DEMOD_GET_FCH_RT_NT_IO( ) \
    mdsp_read(CDMA1X_fundWeightedNtMagAccFrame)
#endif


/*===========================================================================

                  DEMODULATOR SEARCH ENGINE REGISTERS

===========================================================================*/


/*===========================================================================

MACRO SEARCH_ANT_SELECT

DESCRIPTION     This macro selects the antenna to search from for searcher1.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

                DMA may only be used with the 1x searcher.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define SEARCH_ANT_SELECT( ant )



/*===========================================================================

                        DEMODULATOR SYMBOL COMBINER

===========================================================================*/

/*===========================================================================

MACRO COMBINER_SET_NUM_SCCH_SOFT_COMBINED

DESCRIPTION     This macro enables/disables the  supplemental code channels
                being soft combined.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_NUM_SCCH_SOFT_COMBINED( chan_mask ) \
    MSM_W( DEM1X_FW_CH_ENABLE_IMMED, chan_mask )

#define COMBINER_SET_NUM_SCCH_SOFT_COMBINED_FRAME( chan_mask ) \
    MSM_W( DEM1X_FW_CH_ENABLE_FRAME, chan_mask )


/*===========================================================================

MACRO COMBINER_CHANNEL_ENABLE_CHx
MACRO COMBINER_CHANNEL_DISABLE_CHx

DESCRIPTION     These macros enable and disable the channels that are
                demodulated in the combiner.

DEPENDENCIES    This function must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS

===========================================================================*/

#ifdef T_MSM6700
#define COMBINER_ENABLE_CH0( )                     \
    MSM_SET_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 )     \
    MSM_CLEAR( DEM1X_CHANNEL0_IMMED, CH0_DISABLE )

#define COMBINER_DISABLE_CH0( )                    \
    MSM_CLEAR_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 )   \
    MSM_SET( DEM1X_CHANNEL0_IMMED, CH0_DISABLE )
#else
#define COMBINER_ENABLE_CH0( )                     \
    MSM_SET_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 )

#define COMBINER_DISABLE_CH0( )                    \
    MSM_CLEAR_BIT( DEM1X_FW_CH_ENABLE_IMMED, 0 )
#endif /* T_MSM6700 */

/*===========================================================================

MACRO COMBINER_ENABLE_POWER_CONTROL
MACRO COMBINER_DISABLE_POWER_CONTROL

DESCRIPTION     These macros enable and disable closed loop power control

DEPENDENCIES    This function must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  #define COMBINER_ENABLE_POWER_CONTROL( ) \
    if ( ! BIO_GET_SW ( BIO_SW5_M ) )\
      CDMA_MDSP_SET_TX_CLOSED_LOOP_NORMAL()

  #define COMBINER_DISABLE_POWER_CONTROL( ) \
    CDMA_MDSP_ENABLE_TX_CLOSED_LOOP_OVERRIDE()
#else /* !FEATURE_MDSP_SUPPORTS_TX_AGC */
  #define COMBINER_ENABLE_POWER_CONTROL( ) \
    if ( ! BIO_GET_SW ( BIO_SW5_M ) )\
    {\
      MSM_SET( TX_AGC_CTL2, PWR_CTL_EN );\
    }

  #define COMBINER_DISABLE_POWER_CONTROL( ) \
    MSM_CLEAR( TX_AGC_CTL2, PWR_CTL_EN )
#endif /* FEATURE_MDSP_SUPPORTS_TX_AGC */

/*===========================================================================

MACRO COMBINER_ENABLE_PUNCTURING
MACRO COMBINER_DISABLE_PUNCTURING

DESCRIPTION     These macros enable and disable puncturing (erasures) in the
                combined data streams.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_ENABLE_PUNCTURING( ) \
    MSM_SET( DEM1X_TRAFFIC_REV_PWR_CTL, ERASE_EN )

#define COMBINER_DISABLE_PUNCTURING( ) \
    MSM_CLEAR( DEM1X_TRAFFIC_REV_PWR_CTL, ERASE_EN )

/*===========================================================================

MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET1
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET2
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET3
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET4
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET5
MACRO COMBINER_SET_INIT_PWR_CTL_OFFSET6

DESCRIPTION     These macros set the initial offset for power control.
                For 6500 it is for Cell A and Cell B.  For 6700 it is
                per sector, from 1-6.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_INIT_PWR_CTL_OFFSET1( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET1, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET2( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET2, (offset) )
#ifdef T_MSM6700
#define COMBINER_SET_INIT_PWR_CTL_OFFSET3( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET3, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET4( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL,  INIT_OFFSET4, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET5( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, INIT_OFFSET5, (offset) )
#define COMBINER_SET_INIT_PWR_CTL_OFFSET6( offset ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, INIT_OFFSET6, (offset) )
#endif /* T_MSM6700 */

/*===========================================================================

MACRO COMBINER_SET_VALID_SECTORS

DESCRIPTION     The write to the VALID_SECTOR part of this register
                indicates to the hardware which initial offset values
                (as written by COMBINER_SET_INIT_PWR_CTL_OFFSET macros)
                are valid.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_VALID_SECTORS( valid_sectors ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL2, VALID_SECTOR, (valid_sectors) )

/*===========================================================================

MACRO COMBINER_SET_PC_RATE

DESCRIPTION     This macro sets the power control rate.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_PC_RATE( rate ) \
     MSM_W_MASKED( DEM1X_COMMON_REV_PWR_CTL, PC_RATE_SEL, (rate) )

/*==========================================================================
MACRO COMBINER_SET_CH_SYMBOL_RATE

DESCRIPTION     This macro sets the combiner to accumulate symbols for a
                specific number of chips, on a specific channel.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_CH_SYMBOL_RATE( chan, rate ) \
     MSM_W_MASKED( DEM1X_CHANNEL##chan##_IMMED, RATE_SEL, (rate) )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_128_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_128_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over two PN
              words(128 chips). This should be done when demodulating RC4 frames and
              for 9.6kbps Quick Paging.


DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_128_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_128 )

#define COMBINER_SELECT_CH1_128_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_128 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_64_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_64_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 64 chips.
              This should be done when demodulating RC1, RC2, RC3, RC5 and
              Paging Channel frames.


DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_64_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_64 )

#define COMBINER_SELECT_CH1_64_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_64 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_256_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_256_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 256 chips.
              This should be done when demodulating SYNC Channel.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_256_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_256 )

#define COMBINER_SELECT_CH1_256_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_256 )

/*==========================================================================
MACRO COMBINER_SELECT_CH0_512_CHIP_SYMBOL
MACRO COMBINER_SELECT_CH1_512_CHIP_SYMBOL

DESCRIPTION   This macro sets the combiner to accumulate symbols over 256 chips.
              This should be done when demodulating SYNC Channel.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SELECT_CH0_512_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 0, DMOD_SPR_512 )

#define COMBINER_SELECT_CH1_512_CHIP_SYMBOL( ) \
     COMBINER_SET_CH_SYMBOL_RATE( 1, DMOD_SPR_512 )

/*===========================================================================
MACRO COMBINER_PUNCTURE_1_SYM
MACRO COMBINER_PUNCTURE_2_SYM

DESCRIPTION     These macros control whether 1 or 2 symbols are punctured.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_PUNCTURE_1_SYM( ) \
    MSM_SET( DEM1X_TRAFFIC_REV_PWR_CTL, POWER_CTL_PCT )

#define COMBINER_PUNCTURE_2_SYM( ) \
    MSM_CLEAR( DEM1X_TRAFFIC_REV_PWR_CTL, POWER_CTL_PCT )

/*===========================================================================

MACRO COMBINER_TIMETRACK_FINGER

DESCRIPTION     This macro selects the finger for the symbol combiner to
                timetrack.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_TIMETRACK_FINGER( finger ) \
    MSM_W_MASKED( DEM1X_TRACKING, FINGER, (finger) ); \
    MSM_SET( DEM1X_TRACKING, ENABLE )

/*===========================================================================

MACRO COMBINER_TIMETRACK_DISABLE

DESCRIPTION     This macro selects no finger for the symbol combiner to
                timetrack.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_TIMETRACK_DISABLE(  ) \
    MSM_CLEAR( DEM1X_TRACKING, ENABLE )

/*===========================================================================

MACRO COMBINER_CURRENT_TIMETRACK_FINGER

DESCRIPTION     This macro returns the finger currently being tracked by the
                combiner.

DEPENDENCIES    Requires the RXCHIPX8 clock regime.

RETURN VALUE    The finger in question.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_CURRENT_TIMETRACK_FINGER() \
    MSM_R_MASKED(DEM1X_TRACKING, FINGER)

/*===========================================================================

MACRO COMBINER_ARM_SLAM

DESCRIPTION     This macro arms a slam to a finger.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_ARM_SLAM( finger ) \
    COMBINER_TIMETRACK_FINGER( finger ); \
    MSM_SET( DEM1X_ARM_COMBINER_SLAM, SET )

/*===========================================================================

MACRO COMBINER_SET_DELAY

DESCRIPTION     This macro sets the combiner count relative to the finger
                the combiner is tracking.  The value is loaded on the PN
                roll of that finger if a slam is armed, or when
                DEM1X_SYNC:COMB_TIME cmd is issued.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6700
#define COMBINER_SET_DELAY( pos, systime80ms ) \
    MSM_W_MASKED( DEM1X_COMBINER_TIME_LOAD, SYS_TIME, systime80ms ); \
    MSM_W_MASKED( DEM1X_COMBINER_TIME_LOAD, TIMECHIPX8, pos )
#else
#define COMBINER_SET_DELAY( pos, unused )                            \
    /*lint -save -e774 Boolean in 'if' always evaluates to False */  \
    MSM_NOT_USED( unused );                                          \
    /*lint -restore */                                               \
    MSM_W_MASKED( DEM1X_COMBINER_TIME_LOAD, TIME80MS, (pos >> 20) ); \
    MSM_W_MASKED( DEM1X_COMBINER_TIME_LOAD, TIMECHIPX8, pos )
#endif /* T_MSM6700 */

/*===========================================================================

MACRO QPCH_SET_ION_QON

DESCRIPTION     This macro sets the firmware I_on and Q_on flags to help
                decode the correct PI bit.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define QPCH_SET_ION_QON( i_on, q_on ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_IMMED, QP_I_ON, (i_on) ); \
    MSM_W_MASKED( DEM1X_CHANNEL0_IMMED, QP_Q_ON, (q_on) )


/*===========================================================================

MACRO COMBINER_ENABLE_CPCCH
MACRO COMBINER_DISABLE_CPCCH

DESCRIPTION     These macros enable/disable CPCCH.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef T_MSM6700
#define COMBINER_ENABLE_CPCCH( ) \
        MSM_SET( DEM1X_CHANNEL8_IMMED, CPCCH_EN )

#define COMBINER_DISABLE_CPCCH( ) \
        MSM_CLEAR( DEM1X_CHANNEL8_IMMED, CPCCH_EN )
#else
#define COMBINER_ENABLE_CPCCH( ) \
        MSM_SET( DEM1X_CHANNEL0_IMMED, CPCCH_EN )

#define COMBINER_DISABLE_CPCCH( ) \
        MSM_CLEAR( DEM1X_CHANNEL0_IMMED, CPCCH_EN )
#endif /* T_MSM6700 */

/*===========================================================================

MACRO COMBINER_ENABLE
MACRO COMBINER_DISABLE


DESCRIPTION     These macros enable and disable the symbol combiner.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    Combiner is in low-power mode when disabled.

===========================================================================*/
#define COMBINER_ENABLE( ) \
    MSM_SET( DEM1X_COMBINER_CTL, ENABLE )

#define COMBINER_DISABLE( ) \
    MSM_CLEAR( DEM1X_COMBINER_CTL, ENABLE )


/*===========================================================================

MACRO COMBINER_FREQ_TRACK_ENABLE
MACRO COMBINER_FREQ_TRACK_DISABLE


DESCRIPTION     These macros set/clear the ft_enable to the DSP.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_FREQ_TRACK_ENABLE( ) \
        MSM_SET( DEM1X_COMBINER_CTL, FREQ_TRACK_EN )

#define COMBINER_FREQ_TRACK_DISABLE( ) \
        MSM_CLEAR( DEM1X_COMBINER_CTL, FREQ_TRACK_EN )

/*===========================================================================

MACRO DEM1X_LATCH_ALL

DESCRIPTION     This macro causes the symbol combiner's position, the RTC
                position and the LC state to be latched.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_LATCH_ALL( ) \
    HWIO_OUT(DEM1X_LATCH, HWIO_FMSK(DEM1X_LATCH,REF_COUNT) | \
                          HWIO_FMSK(DEM1X_LATCH,LC_STATE)  | \
                          HWIO_FMSK(DEM1X_LATCH,COMB_TIME))

/*===========================================================================

MACRO DEM1X_LATCH_RTC_AND_SC

DESCRIPTION     This macro causes the symbol combiner's position and RTC
                to be latched.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_LATCH_RTC_AND_SC( ) \
    HWIO_OUT(DEM1X_LATCH, HWIO_FMSK(DEM1X_LATCH,REF_COUNT) | \
                          HWIO_FMSK(DEM1X_LATCH,COMB_TIME))

/*===========================================================================

MACRO COMBINER_LATCH_POSITION

DESCRIPTION     This macro latches the symbol combiner's position.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_LATCH_POSITION( ) \
    MSM_SET( DEM1X_LATCH, COMB_TIME )

/*===========================================================================

MACRO RTC_LATCH_POSITION

DESCRIPTION     This macro latches the RTC's position.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define RTC_LATCH_POSITION( ) \
    MSM_SET( DEM1X_LATCH, REF_COUNT )

/*===========================================================================

MACRO DEM1X_RTC_GET_COUNT

DESCRIPTION     This macro reads the latest reference count value that
                was latched when DEM1X_LATCH:REF_COUNT was set.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The RTC as of the write to DEM1X_LATCH:REF_COUNT.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_RTC_GET_COUNT( ref_cnt ) \
    ref_cnt = MSM_R_MASKED( DEM1X_REF_COUNT, LATCHED )

/*===========================================================================

MACRO DEM1X_COMBINER_GET_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_COUNT( sc_pos ) \
    sc_pos = MSM_R_MASKED( DEM1X_COMBINER_TIME, LATCHED )

/*===========================================================================

MACRO DEM1X_COMBINER_GET_80MS_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_80MS_COUNT( sc_pos ) \
    sc_pos = (MSM_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) & \
              COMBINER_80MS_COUNT_MASK )>> 20;

/*===========================================================================

MACRO DEM1X_COMBINER_GET_SUB80MS_COUNT

DESCRIPTION     This macro reads the latest combiner count value that
                was latched when DEM1X_LATCH:COMB_TIME was set.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    The combiner count as of the write to DEM1X_LATCH:COMB_TIME.

SIDE EFFECTS    None.

===========================================================================*/

#define DEM1X_COMBINER_GET_SUB80MS_COUNT( sc_pos ) \
    sc_pos = (MSM_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) & \
              COMBINER_80MS_CX8_MASK)


/*===========================================================================

MACRO COMBINER_GET_POSITION

DESCRIPTION     This macro reads the latched symbol combiner's position.

DEPENDENCIES    This macro and the DMOD_SYMB_COMB_LATCH_POS function must
                be called from inside the context of a MSM_LOCK /
                MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_GET_POSITION( position ) \
    (position ) = (MSM_R_MASKED( DEM1X_COMBINER_TIME, LATCHED ) &     \
                   COMBINER_80MS_CX8_MASK )>> 4;
    /* converted to 2-chip and eliminated upper 80ms - that is what we get
       from the old macro */

/*===========================================================================

MACRO COMBINER_SLEW

DESCRIPTION     This macro slews the symbol combiner position to chipx8
                resolution.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The symbol combiner position is changed.

===========================================================================*/

#ifdef T_MSM6700
#define COMBINER_SLEW( slew, systime80ms )                          \
    MSM_W( DEM1X_SLEW_COMBINER_TIME,                                \
           ((slew) & HWIO_DEM1X_SLEW_COMBINER_TIME_OFFSET_BMSK)  |  \
           ((systime80ms) <<                                        \
           HWIO_DEM1X_SLEW_COMBINER_TIME_UPP_SYS_TIME_SHFT) &       \
           HWIO_DEM1X_SLEW_COMBINER_TIME_UPP_SYS_TIME_BMSK )
#else
#define COMBINER_SLEW( slew, unused )                               \
    /*lint -save -e774 Boolean in 'if' always evaluates to False */ \
    MSM_NOT_USED( unused );                                         \
    /*lint -restore */                                              \
    MSM_W( DEM1X_SLEW_COMBINER_TIME,                                \
           (slew) & HWIO_DEM1X_SLEW_COMBINER_TIME_OFFSET_BMSK )
#endif /* T_MSM6700 */

/*===========================================================================

MACRO COMBINER_SLEW_SUB80MS

DESCRIPTION     This macro slews the symbol combiner position to chipx8
                resolution, but only adjusts the sub-80ms count.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    The symbol combiner position is changed.

===========================================================================*/

#define COMBINER_SLEW_SUB80MS( slew ) \
    MSM_W( DEM1X_SLEW_COMBINER_TIME, (slew) & COMBINER_80MS_CX8_MASK )

/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_STATE
MACRO COMBINER_ZERO_LONG_CODE_STATE

DESCRIPTION     These macros set or zero the long code state.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

                The lc_state argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    The long code is transfered from the load register to the
                long code generator on the next PN roll.

===========================================================================*/

#define COMBINER_SET_LONG_CODE_STATE( lc_state )          \
    MSM_W( DEM1X_LC_STATE_LOAD_LO, D_PTR(lc_state)[0] );  \
    /*lint -save -e415 Access of out-of-bounds pointer */ \
    MSM_W( DEM1X_LC_STATE_LOAD_HI, D_PTR(lc_state)[1] );  \
    /*lint -restore */                                    \
    MSM_SET( DEM1X_ARM_LC_STATE_LOAD, SET )

#define COMBINER_ZERO_LONG_CODE_STATE( ) \
    MSM_W( DEM1X_LC_STATE_LOAD_LO, DEM1X_ZERO ); \
    MSM_W( DEM1X_LC_STATE_LOAD_HI, DEM1X_ZERO ); \
    MSM_SET( DEM1X_ARM_LC_STATE_LOAD, SET )


/*===========================================================================

MACRO COMBINER_GET_LONG_CODE_STATE

DESCRIPTION     This macro gets the long code state as of the last PN roll.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

                The lc_state argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_GET_LONG_CODE_STATE( lc_state ) \
      MSM_LOCK_REQUIRED \
      D_PTR(lc_state)[0] = MSM_R_MASKED( DEM1X_LC_STATE_LO, LATCHED ), \
      D_PTR(lc_state)[1] = MSM_R_MASKED( DEM1X_LC_STATE_HI, LATCHED )


/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_MASK_CHAN_ROLL
MACRO COMBINER_SET_LONG_CODE_MASK_CHAN_IMMED

DESCRIPTION     These macro set the long code mask for a specific channel.
                The IMMED version sets the long code immediately and
                doesn't wait for a roll.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#define COMBINER_SET_LONG_CODE_MASK_CHAN_ROLL( lc_mask, chan )   \
    COMBINER_SET_LONG_CODE_MASK_SPN( chan, lc_mask )

#define COMBINER_SET_LONG_CODE_MASK_CHAN_IMMED( lc_mask, chan )  \
    /* Cast to uint32 for IO */                                  \
    /*lint -e{571}*/                                             \
    COMBINER_SET_LONG_CODE_MASK( (uint32)(chan), lc_mask )

/*===========================================================================

MACRO COMBINER_SET_LONG_CODE_MASK
MACRO COMBINER_ZERO_LONG_CODE_MASK

DESCRIPTION     These macros set or zero the long code mask.

DEPENDENCIES    These macros must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

                The lc_mask argument must be l-value.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define COMBINER_SET_LONG_CODE_MASK( channel, lc_mask ) \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, D_PTR(lc_mask)[0], (uint32)channel ),\
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, D_PTR(lc_mask)[1], (uint32)channel )

#define COMBINER_ZERO_LONG_CODE_MASK( ) \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 0 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 0 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 1 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 1 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_IMMED, 0, 2 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_IMMED, 0, 2 )

#define COMBINER_SET_LONG_CODE_MASK_SPN( channel, lc_mask ) \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, D_PTR(lc_mask)[0], channel ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, D_PTR(lc_mask)[1], channel )

#define COMBINER_ZERO_LONG_CODE_MASK_SPN( ) \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 0 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 0 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 1 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 1 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_LO_SPN_ROLL, 0, 2 ), \
    MSM_W_i( DEM1X_CHn_LC_MASK_HI_SPN_ROLL, 0, 2 )



/*===========================================================================

           DECODER

===========================================================================*/
#define VD_MODE_WB__INTMODE_MASK                  HWIO_FMSK(VD_MODE,INITMODE)

/*===========================================================================

MACRO DEC_DYNAMIC_CLK_SAVE
MACRO DEC_DYNAMIC_CLK_OFF
MACRO DEC_DYNAMIC_CLK_ON
MACRO DEC_DYNAMIC_CLK_RESTORE

DESCRIPTION     On platforms that support Dynamic Clock Gating, this feature
                must be turned off before writing to the following registers:

                    VD_RESET
                    VD_MODE
                    VD_POLY2IJ
                    VD_POLY3IJ
                    VD_POLY3K
                    VD_POLY4IJ
                    VD_POLY4KL
                    VD_TESTCON
                    VD_TESTOUT

                    TD_RESET
                    TD_TEST_START
                    TD_TEST_MODE_CFG_MSB
                    TD_TEST_MODE_CFG_LSB
                    TD_TEST_SYM_LOAD
                    TD_TEST_STATUS
                    TD_TEST_DOUT_MSB
                    TD_TEST_DOUT_LSB
                    TD_ENERGY_METRIC_MSB
                    TD_ENERGY_METRIC_LSB
                    TD_STATUS_MSB
                    TD_STATUS_LSB

                These macros can be used to turn on/off or save/restore the
                state of Dynamic Clock Gating.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    Dynamic Clock Gating may be turned on or off.

===========================================================================*/

#ifdef MSMHW_DECODER_DYNAMIC_CLOCK_GATING

#ifdef FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF
#define DEC_DYNAMIC_CLK_SAVE()
#define DEC_DYNAMIC_CLK_OFF()
#define DEC_DYNAMIC_CLK_SVD_OFF()
#define DEC_DYNAMIC_CLK_ON()
#define DEC_DYNAMIC_CLK_SVD_ON()
#define DEC_DYNAMIC_CLK_RESTORE()
#else
/* Save the current state of dynamic clock gating */
#define DEC_DYNAMIC_CLK_SAVE()                                        \
   do {                                                               \
     uint32  dcg_state = HWIO_INM(MISC_CLK_CTL,                       \
                         HWIO_FMSK(MISC_CLK_CTL,TURBO_DEC_SLEEP_EN) | \
                         HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN))

/* Turn dynamic clock gating off */
#define DEC_DYNAMIC_CLK_OFF()                              \
   HWIO_OUTM(MISC_CLK_CTL,                                 \
             HWIO_FMSK(MISC_CLK_CTL,TURBO_DEC_SLEEP_EN) |  \
             HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN),         \
             0);                                           \
   clk_busy_wait(1)

/* Turn dynamic clock gating off for the viterbi decoder */
#define DEC_DYNAMIC_CLK_SVD_OFF()                          \
   HWIO_OUTM(MISC_CLK_CTL,                                 \
             HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN),         \
             0);                                           \
   clk_busy_wait(1)

/* Turn dynamic clock gating on */
#define DEC_DYNAMIC_CLK_ON()                               \
   clk_busy_wait(1);                                       \
   HWIO_OUTM(MISC_CLK_CTL,                                 \
             HWIO_FMSK(MISC_CLK_CTL,TURBO_DEC_SLEEP_EN) |  \
             HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN),         \
             -1)

/* Turn dynamic clock gating on for the viterbi decoder */
#define DEC_DYNAMIC_CLK_SVD_ON()                           \
   clk_busy_wait(1);                                       \
   HWIO_OUTM(MISC_CLK_CTL,                                 \
             HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN),         \
             -1)

/* Restore dynamic clock gating (should only be used after */
/* dec_dynamic_clk_save).                                  */
#define DEC_DYNAMIC_CLK_RESTORE()                            \
     clk_busy_wait(1);                                       \
     HWIO_OUTM(MISC_CLK_CTL,                                 \
               HWIO_FMSK(MISC_CLK_CTL,TURBO_DEC_SLEEP_EN) |  \
               HWIO_FMSK(MISC_CLK_CTL,SVD_SLEEP_EN),         \
               dcg_state);                                   \
   }while(0)
#endif /* FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF */

#else
#define DEC_DYNAMIC_CLK_SAVE()
#define DEC_DYNAMIC_CLK_OFF()
#define DEC_DYNAMIC_CLK_SVD_OFF()
#define DEC_DYNAMIC_CLK_ON()
#define DEC_DYNAMIC_CLK_SVD_ON()
#define DEC_DYNAMIC_CLK_RESTORE()
#endif


/*===========================================================================

MACRO CLKRGM_DEC1_START
MACRO CLKRGM_DEC1_STOP

DESCRIPTION     These macros provide a workaround for the MSM6050 DEC1
                hardware problem.  This problem could cause corruption in the
                following registers if one of these registers is written to
                when the decoder clock and uP clocks are not synchronized:

                    VD_MODE
                    VD_POLY2IJ
                    VD_POLY3IJ
                    VD_POLY3K
                    VD_POLY4IJ
                    VD_POLY4KL
                    VD_TESTCON
                    TEST_MEM_SEL
                    TEST_MEM_DIN

                The workaround is to call this function before and after writes
                to one of these registers.  Calling this function with a
                parameter of '0' synchronizes these two clock regimes, which
                should be done before a write to the above registers.  Calling
                this function with a '1' restores the clock regimes to there
                previouse values.


DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    Micro-processor and Decoder clock speeds are changed.

===========================================================================*/
#ifdef MSMHW_MSM6050_DEC1
#include "clkregim.h"
#define CLKRGM_DEC1_START()   do{clk_regime_dec1 (CLKRGM_START)
#define CLKRGM_DEC1_END()     clk_regime_dec1 (CLKRGM_END);}while(0)
#else
#define CLKRGM_DEC1_START()
#define CLKRGM_DEC1_END()
#endif


/*===========================================================================

MACRO DECODER_INITIAL_STATE_3_ZEROS
MACRO DECODER_INITIAL_STATE_4_ZEROS
MACRO DECODER_INITIAL_STATE_8_ZEROS

DESCRIPTION     These macros set the Decoder's State Metrics to values
                corresponding to the preceeding frame ending with 3, 4 or
                8 zeros.

DEPENDENCIES    Continuous mode only.  DECODER_INITIAL_STATE_NORMAL() must
                be used after the desired frame has been decoded.

RETURN VALUE    None.

SIDE EFFECTS    Decoders state metrics are zero'd or saturated at all
                future decoder interrupts until normal operation is
                restored.

===========================================================================*/

#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */

#define DECODER_INITIAL_STATE_3_ZEROS( ) \
    DEC_DYNAMIC_CLK_SVD_OFF();                \
    MSM_W_MASKED( VD_MODE, INITMODE, 0x06 );  \
    DEC_DYNAMIC_CLK_SVD_ON()

#define DECODER_INITIAL_STATE_4_ZEROS( ) \
    DEC_DYNAMIC_CLK_SVD_OFF();                \
    MSM_W_MASKED( VD_MODE, INITMODE, 0x04 );  \
    DEC_DYNAMIC_CLK_SVD_ON()

#define DECODER_INITIAL_STATE_8_ZEROS( ) \
    DEC_DYNAMIC_CLK_SVD_OFF();                \
    MSM_W_MASKED( VD_MODE, INITMODE, 0x02 );  \
    DEC_DYNAMIC_CLK_SVD_ON()

#endif /* !T_MSM7600 */

/*===========================================================================

MACRO DECODER_INITIAL_STATE_NORMAL

DESCRIPTION     This macro terminates any continuous mode decoder
                initialization started by:
                    DECODER_INITIAL_STATE_3_ZEROS( )
                    DECODER_INITIAL_STATE_4_ZEROS( )
                    DECODER_INITIAL_STATE_8_ZEROS( )

DEPENDENCIES    Continuous mode only.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */

#define DECODER_INITIAL_STATE_NORMAL( ) \
    DEC_DYNAMIC_CLK_SVD_OFF();                \
    MSM_W_MASKED( VD_MODE, INITMODE, 0x00 );  \
    DEC_DYNAMIC_CLK_SVD_ON()

#endif /* !T_MSM7600 */

/*===========================================================================

           CDMA AGC, FREQUENCY TRACKING AND BASEBAND OFFSET CONTROL

===========================================================================*/




/*===========================================================================

MACRO SEARCH_FAST_AGC_LOOP
MACRO SEARCH_NORMAL_AGC_LOOP

DESCRIPTION     These macros change the gain constant of the AGC loop, which
                changes the time it will take to settle and the stability of
                the loop.

DEPENDENCIES    Must include "cagc.h".

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_MDM7800

/* TBD */
/* Need to update the new RXF registers */
#define SEARCH_FAST_AGC_LOOP( rx_chain )

#define SEARCH_NORMAL_AGC_LOOP( rx_chain )

#else /* !T_MDM7800 */

#if defined(T_MSM7600)
#error code not present
#else /* if !T_MSM7600 */

#define SEARCH_FAST_AGC_LOOP( rx_chain ) \
    MSM_W_MASKED_i( RX_AGCc_GAIN_CTL, GAIN_CONSTANT, \
                    CAGC_RF_TC_0147_US, rx_chain )

#define SEARCH_NORMAL_AGC_LOOP( rx_chain ) \
    MSM_W_MASKED_i( RX_AGCc_GAIN_CTL, GAIN_CONSTANT, \
                    CAGC_RF_TC_0392_US, rx_chain )

#endif /* !T_MSM7600 */

#endif /* !T_MDM7800 */

#ifndef MSMHW_NOT_USES_CAGC_HIGH_LOW
/*===========================================================================

MACRO CAGC_WRITE_8P1
MACRO CAGC_READ_8P1

DESCRIPTION     Helper macro for 8+1 register writes.

DEPENDENCIES    This function must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    CAGC_LOW_HIGH/ control is left in HIGH/ state.

===========================================================================*/

#define CAGC_SELECT_LOW( ) \
    MSM_WH_SET( AGC_RDWR_CTL, AGC_LOW_HIGH_N)

#define CAGC_SELECT_HIGH( ) \
    MSM_WH_CLEAR( AGC_RDWR_CTL, AGC_LOW_HIGH_N)

#define CAGC_WRITE_8P1( address, value) \
    CAGC_SELECT_LOW( ), \
    MSM_W( address, (byte) ((value) << 7)), \
    CAGC_SELECT_HIGH( ), \
    MSM_W( address, (byte) ((value) >> 1))

#define CAGC_READ_8P1( address, var) \
    CAGC_SELECT_LOW( ), \
    B_PTR(var)[ 0 ] = MSM_RB( address ), \
    CAGC_SELECT_HIGH( ), \
    B_PTR(var)[ 1 ] = MSM_RB( address ), \
    var >>= 7


/*===========================================================================

MACRO CAGC_WRITE_8P2
MACRO CAGC_READ_8P2

DESCRIPTION     Helper macro for 8+2 register writes.

DEPENDENCIES    This function must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    CAGC_LOW_HIGH/ control is left in HIGH/ state.

===========================================================================*/

#define CAGC_WRITE_8P2( address, value) \
    CAGC_SELECT_LOW( ), \
    MSM_W( address, (byte) ((value) << 6) ), \
    CAGC_SELECT_HIGH( ), \
    MSM_W( address, (byte) ((value) >> 2) )

#define CAGC_READ_8P2( address, var) \
    CAGC_SELECT_LOW( ), \
    B_PTR(var)[ 0 ] = MSM_RB( address ), \
    CAGC_SELECT_HIGH( ), \
    B_PTR(var)[ 1 ] = MSM_RB( address ), \
    var >>= 6

/*===========================================================================

MACRO CAGC_VALUE_WRITE
MACRO CAGC_VALUE_READ

DESCRIPTION     This macro writes to or retrieves from the contents of the
                Adjusted Gain Control

DEPENDENCIES    This function must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS

===========================================================================*/
#define CAGC_VALUE_WRITE(value) \
    CAGC_WRITE_8P2( AGC_VALUE_WR, value )

#define CAGC_VALUE_READ(var) \
    CAGC_READ_8P2( AGC_VALUE_RD, var )

#endif /* !MSMHW_NOT_USES_CAGC_HIGH_LOW */


/*===========================================================================

MACRO TX_SET_GAIN_ADJUST

DESCRIPTION     This macro ....

                The power control enable bit is cleared first, so this write
                takes effect.  Power control is then restored to its previous
                state.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
/*  This macro has been modified for FW based-TXAGC targets. BIO_SW5_M is checked here
    instead of reading the current value of the CDMA1X_cTxGainAdjOverride
    register on the MDSP. 

    The sequence of instructions has been reordered to work with MDSP
    memory banks and MDSP read sequence.

    1. Adjust tx_gain_adj_val as required by MDSP
    2. Check dip switch to check if RPC is disabled
    3. If RPC is enabled, tell MDSP to do normal close loop PC
       after TX Gain Adjust Override
    4. Write to tx gain adjust override register on the MDSP
    3. Write new tx_gain_adj_val to MDSP
 */
  #define TX_SET_GAIN_ADJUST( gain ) \
    { \
      int16 tx_gain_adj_val; \
      tx_gain_adj_val = (int16)(((int16)gain) << 8); \
      if ( ! BIO_GET_SW ( BIO_SW5_M ) )  \
        CDMA_MDSP_RECOVER_TX_CLOSED_LOOP_NORMAL();  \
      COMBINER_DISABLE_POWER_CONTROL(); \
      CDMA_MDSP_WR_TX_CLOSED_LOOP_VAL( tx_gain_adj_val ); \
    }
#else /* !FEATURE_MDSP_SUPPORTS_TX_AGC */
/* The TX_AGC_CTL2 register is not readable, and with HWIO the only way to
 * get the current value is to peek the "shadow" value for TX_AGC_CTL2.
 */  
  #define TX_SET_GAIN_ADJUST( gain ) \
    { \
      int _bit_was_set; \
      _bit_was_set = (HWIO_TX_AGC_CTL2_shadow \
              & HWIO_FMSK(TX_AGC_CTL2, PWR_CTL_EN)) >> \
                HWIO_SHFT(TX_AGC_CTL2,PWR_CTL_EN); \
      COMBINER_DISABLE_POWER_CONTROL(); \
      MSM_W( TX_GAIN_ADJ_WR, ( (gain) << 1) ); \
      if (_bit_was_set) MSM_SET( TX_AGC_CTL2, PWR_CTL_EN ); \
    }
#endif /* FEATURE_MDSP_SUPPORTS_TX_AGC */

/*===========================================================================

MACRO TX_GET_GAIN_ADJUST
MACRO TX_AGC_READ_GAIN_ADJ

DESCRIPTION     This macro retrieves the tx gain adjust.

                The power control enable bit is cleared first, so this write
                takes effect.  Power control is then restored to its previous
                state.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  #define TX_GET_GAIN_ADJUST( ) \
    (((int2)CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) >> 8) & 0xff

  #define TX_AGC_READ_GAIN_ADJ() \
    (((int2)CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) >> 7) & 0x1ff
#else /* !FEATURE_MDSP_SUPPORTS_TX_AGC */
  #define TX_GET_GAIN_ADJUST( ) \
    (MSM_R_MASKED( TX_GAIN_ADJ_RD, DATA ) >> 1)

  #define TX_AGC_READ_GAIN_ADJ() \
    MSM_R_MASKED( TX_GAIN_ADJ_RD, DATA )
#endif /* FEATURE_MDSP_SUPPORTS_TX_AGC */

/*===========================================================================

MACRO TX_AGC_RESET

DESCRIPTION     This macro resets the tx AGC.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define TX_AGC_RESET() \
    MSM_SET( TX_AGC_RESET, DATA )

/*===========================================================================

MACRO MSM_PA_TX_IS_ON
MACRO MSM_PA_TX_ON
MACRO MSM_PA_TX_OFF

DESCRIPTION     These macros do 3 things:

                1)  Returns whether or not the TX_ON function is enabled.
                2)  Enables TX_ON function.
                3)  Disables TX_ON function.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

RETURN VALUE    MSM_PA_TX_IS_ON returns whether or not TX_ON function is
                enabled.

SIDE EFFECTS    None.

===========================================================================*/

#define MSM_PA_TX_IS_ON() \
    (HWIO_PA_ON_CTL_shadow & HWIO_FMSK(PA_ON_CTL, TX_ON_EN)) >> \
    HWIO_SHFT(PA_ON_CTL,TX_ON_EN)

#define MSM_PA_TX_ON() \
    MSM_SET( PA_ON_CTL, TX_ON_EN )
#define MSM_PA_TX_OFF() \
    MSM_CLEAR( PA_ON_CTL, TX_ON_EN )



/*===========================================================================

                        CDMA SLEEP CONTROL

===========================================================================*/

/*===========================================================================

MACRO SLEEP_CMD

DESCRIPTION     This macro triggers the 1X sleep controller to sleep
                immediately.

DEPENDENCIES    This macro must be called from inside the context of a
                MSM_LOCK / MSM_UNLOCK pair to ensure proper operation.

                SLEEP/WAKEUP timers should be set before triggering sleep.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#if defined T_QSC60X5
#define SLEEP_CMD() \
    MSM_SET_i( SLEEPn_MICRO_START, START, 0 );
#elif defined T_MSM6700
/* EEE 6700 revisit */
/* Use "1" below to indicate 1x sleep controller */
#define SLEEP_CMD() \
    MSM_SET_i( SLEEPn_MICRO_START, START, 1 );
#else
#define SLEEP_CMD() \
    MSM_SET( SLEEP_CMD, SLEEP_NOW );
#endif /* T_QSC60X5 */


/*===========================================================================

                        Interrupt Controller

===========================================================================*/

/*===========================================================================

MACRO INTERRUPT_CLEAR

DESCRIPTION     These macros reset the interrupt status register for the
                given interrupt line.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define INTERRUPT_CLEAR( interrupt_0_mask, interrupt_1_mask ) \
    MSM_WW( INT_CLEAR_0, (interrupt_0_mask) ); \
    MSM_WW( INT_CLEAR_1, (interrupt_1_mask) )


/*===========================================================================

MACRO INTERRUPT_STATUS

DESCRIPTION     These macros check the interrupt status for the
                given interrupt line.

DEPENDENCIES    None.

RETURN VALUE    0 if the specified interrupt in not pending
                Non-zero if the interrupt is pending.

SIDE EFFECTS    None.

===========================================================================*/
#if defined(T_MSM7600)
#define INTERRUPT_STATUS_26MS( )     ( HWIO_IN(RTC_INT_STATUS) & \
                                       HWIO_FMSK(RTC_INT_STATUS,PN_ROLLOVER_1X) )
#elif defined(T_MSM6800)
#define INTERRUPT_STATUS_26MS( )     ( HWIO_IN(RTC_INT_STATUS2) & \
                                       HWIO_FMSK(RTC_INT_STATUS2,PN_ROLLOVER_1X) )
#else /* if !T_MSM7600 && !T_MSM6800 */
#define INTERRUPT_STATUS_26MS( )     ( HWIO_IN(INT_STATUS_1) & \
                                       HWIO_FMSK(INT_STATUS_1,PN_ROLLOVER_1X) )
#endif /* !T_MSM7600 && !T_MSM6800 */



/*===========================================================================

                        TCXO PDM#1 and TCXO PDM#2

===========================================================================*/

/*===========================================================================

MACRO PDM1_SET_NORMAL_POLARITY
MACRO PDM1_SET_INVERTED_POLARITY


DESCRIPTION     These macros alter the polarity of PDM #1

DEPENDENCIES    These macros must be called within a MSM_LOCK / MSM_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_SET_NORMAL_POLARITY( ) \
    MSM_CLEAR( TCXO_PDM_CTL, PDM1_POLARITY)

#define PDM1_SET_INVERTED_POLARITY( ) \
    MSM_SET( TCXO_PDM_CTL, PDM1_POLARITY)


/*===========================================================================

MACRO PDM2_SET_NORMAL_POLARITY
MACRO PDM2_SET_INVERTED_POLARITY


DESCRIPTION     These macros alter the polarity of PDM #2

DEPENDENCIES    These macros must be called within a MSM_LOCK / MSM_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_SET_NORMAL_POLARITY( ) \
    MSM_CLEAR( TCXO_PDM_CTL, PDM2_POLARITY)

#define PDM2_SET_INVERTED_POLARITY( ) \
    MSM_SET( TCXO_PDM_CTL, PDM2_POLARITY)


/*===========================================================================

MACRO PDM1_ENABLE
MACRO PDM1_DISABLE


DESCRIPTION     These macros enable and disable PDM #1

DEPENDENCIES    These macros must be called within a MSM_LOCK / MSM_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_ENABLE( ) \
    MSM_SET( TCXO_PDM_CTL, PDM1_EN)

#define PDM1_DISABLE( ) \
    MSM_CLEAR( TCXO_PDM_CTL, PDM1_EN)


/*===========================================================================

MACRO PDM2_ENABLE
MACRO PDM2_DISABLE


DESCRIPTION     These macros enable and disable PDM #2

DEPENDENCIES    These macros must be called within a MSM_LOCK / MSM_UNLOCK
                pair to ensure proper operation.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_ENABLE( ) \
    MSM_SET( TCXO_PDM_CTL, PDM2_EN)

#define PDM2_DISABLE( ) \
    MSM_CLEAR( TCXO_PDM_CTL, PDM2_EN)


/*===========================================================================

MACRO PDM1_SET_PULSE_DENSITY

DESCRIPTION     This macro sets the density of pulses from PDM #1.  Assuming
                normal polarity:
                    0x80 - Low for 255/256 TCXO/4 pulses
                    0x00 - Low for 127/256 TCXO/4 pulses
                    0x7f - Low for   0/256 TCXO/4 pulses

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM1_SET_PULSE_DENSITY( value ) \
    MSM_W( PDM1_CTL, (value) )


/*===========================================================================

MACRO PDM2_SET_PULSE_DENSITY

DESCRIPTION     This macro sets the density of pulses from PDM #2.  Assuming
                normal polarity:
                    0x80 - Low for 255/256 TCXO/4 pulses
                    0x00 - Low for 127/256 TCXO/4 pulses
                    0x7f - Low for   0/256 TCXO/4 pulses

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

===========================================================================*/

#define PDM2_SET_PULSE_DENSITY( value ) \
    MSM_W( PDM2_CTL, (value) )


/*===========================================================================

                        MSM Clocks Control

===========================================================================*/


/*===========================================================================

                        Version Register

===========================================================================*/




/*===========================================================================

MACRO VERSION_GET

DESCRIPTION     This macro the MSM version number

DEPENDENCIES    None.

RETURN VALUE    The MSM version number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET( ) \
    MSM_R( HW_REVISION_NUMBER )


/*===========================================================================

MACRO VERSION_GET_PROCESS_ID

DESCRIPTION     This macro returns the MSM process version number

DEPENDENCIES    None.

RETURN VALUE    The MSM process version number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_PROCESS_ID( ) \
     MSM_R_MASKED( HW_REVISION_NUMBER, QUALCOMM_MFG_ID )


/*===========================================================================

MACRO VERSION_GET_MSM_ID

DESCRIPTION     This macro returns the MSM id number

DEPENDENCIES    None.

RETURN VALUE    The MSM id number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_MSM_ID( ) \
    MSM_R_MASKED( HW_REVISION_NUMBER, PARTNUM )


/*===========================================================================

MACRO VERSION_GET_VERSION_ID

DESCRIPTION     This macro returns the MSM version id number

DEPENDENCIES    None.

RETURN VALUE    The MSM version id number.

SIDE EFFECTS    None.

===========================================================================*/

#define VERSION_GET_VERSION_ID( ) \
    MSM_R_MASKED( HW_REVISION_NUMBER, VERSION_ID )


/*===========================================================================

                              Quick Paging Channel

===========================================================================*/


/*===========================================================================

MACRO QPCH_GET_SOFT_DECISION_I/Q
MACRO QPCH_GET_DECISION_THRESHOLD_I/Q

DESCRIPTION     These macros the QPCH soft decision values and thresholds for
                an OOK bit recieved on the Quick Paging Channel.

DEPENDENCIES    Information is only valid after the RX_QP_INT interrupt.

RETURN VALUE    Decision values (11 bits, signed)

SIDE EFFECTS    None.

===========================================================================*/

#define QPCH_GET_SOFT_DECISION_I( )  mdsp_read( CDMA1X_qpIndI );
#define QPCH_GET_SOFT_DECISION_Q( )  mdsp_read( CDMA1X_qpIndQ );

#define QPCH_GET_DECISION_THRESHOLD_1( )  mdsp_read( CDMA1X_qpThresh1 );
#define QPCH_GET_DECISION_THRESHOLD_2( )  mdsp_read( CDMA1X_qpThresh2 );


/*===========================================================================

MACRO QPCH_ARM_OOK_DECODE
MACRO QPCH_DISARM_OOK_DECODE

DESCRIPTION     These macros arm the QPCH OOK hardware to decode a bit at a
                given position, or disarm the OOK hardware.

                Position (9 bits) is specified in PN words (64 chips), for
                a range of upto 1 PN roll, and indicates the last PN word of
                the OOK bit.  Position is relative to the symbol combiner
                26.67ms roll position.

DEPENDENCIES    I/Q Channel configuration, symbol size, etc must be setup
                first for the QPCH channel.

                SYMB_COMB_CTL1:DESKEW_OFFSET needs correct offset.

RETURN VALUE    None.

SIDE EFFECTS    An "RX_QP_INT" interrupt will be generated within 26.7ms.
                QP_I_RD, QP_Q_RD, QP_I_TH1, and QP_I_TH2 will be modified.

===========================================================================*/

#define QPCH_ARM_OOK_DECODE( pos ) \
    MSM_W_MASKED( DEM1X_CHANNEL0_IMMED, OOK_POSITION, (pos) ); \
    MSM_SET( DEM1X_CHANNEL0_IMMED, QPCH_EN )

#define QPCH_DISARM_OOK_DECODE( ) \
    MSM_CLEAR( DEM1X_CHANNEL0_IMMED, QPCH_EN )


#if defined( FEATURE_SRCH_SUPPORTS_QLIC ) || defined ( T_MDM7800 )
/*===========================================================================

                              QLIC Interface

===========================================================================*/

/*===========================================================================

MACRO QLIC_SET_RTF_CTL_SEL
      QLIC_CLR_RTF_CTL_SEL
      QLIC_GET_RTF_CTL_SEL

DESCRIPTION     These macros sets/gets the RTF control select register

DEPENDENCIES    None.

RETURN VALUE    The "get" macro returns the RTF control select register value.

SIDE EFFECTS    None.

===========================================================================*/
 #define QLIC_SET_RTF_CTL_SEL() \
     MSM_SET( DEM1X_RTF_CTL_SEL, CTL_SEL )

 #define QLIC_CLR_RTF_CTL_SEL() \
     MSM_CLEAR( DEM1X_RTF_CTL_SEL, CTL_SEL )

 #define QLIC_GET_RTF_CTL_SEL() \
     MSM_R( DEM1X_RTF_CTL_SEL )

 #define QLIC_SET_LPN_ADV( syms ) \
     MSM_W_MASKED( DEM1X_LPN_ADV, SYM_ADV, (syms) )

 #define SET_RTF_ADJUST( lat )                           \
     MSM_W_MASKED( DEM1X_RTC_ADJUST, SYM_DLY, (lat) )

 #define GET_RTF_ADJUST() \
     MSM_R_MASKED( DEM1X_RTC_ADJUST, SYM_DLY )
#else  /* FEATURE_SRCH_SUPPORTS_QLIC || T_MDM7800 */
 #define QLIC_SET_RTF_CTL_SEL()
 #define QLIC_CLR_RTF_CTL_SEL()
 #define QLIC_GET_RTF_CTL_SEL()
 #define QLIC_SET_LPN_ADV( syms )
 #define SET_RTF_ADJUST( lat )
 #define GET_RTF_ADJUST() 0 
#endif  /* FEATURE_SRCH_SUPPORTS_QLIC || T_MDM7800 */



/*===========================================================================

                              END OF INTERFACE

===========================================================================*/

#endif /* MSM_DRV.H */


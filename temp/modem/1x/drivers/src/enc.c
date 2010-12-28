/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             M O B I L E    E N C O D E R    S E R V I C E S

GENERAL DESCRIPTION
  Library of routines to initialize, and operate on, the Qualcomm CDMA
  encoder circuit.  Note that this circuit also performs interleaving and
  deinterleaving.  It is referred to in the ASIC documentation as the
  Interleaver.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

EXTERNALIZED FUNCTIONS
  enc_init
    Initialize the circuit to Idle.
  enc_set_btf
    Set the back-to-the-future value loaded by enc_init().
  enc_set_frame_off
    Set the forward/reverse channel frame offset
  enc_set_mode
    Sets the operating mode of the driver: sleep, cdma, sync, page, traffic.
  enc_tx_enable
    Enable/disable the transmit path.
  enc_kill_pa
    Turn off the PA and keep it off until the system is reset.
  enc_int_timing_offset
    Specify the delay after the Transmit Frame Boundary when the Transmit Frame
    Interrupt will activate.
  enc_user_pn_state
    Set the User PN long code state.
  enc_user_pn_mask
    Set the User PN long code mask.
  enc_iq_pn_state
    Set the short code state.
  enc_bozo_mode
    Set the "bozo mode" bit, so that the User PN long code state is reloaded
    on each 80 ms Transmit Frame boundary.
  enc_rate_set
    Set the rate set to use for encoding data on the Traffic Channel.
  enc_data_out
    Send data to the encoder.
  enc_status
    Read the status of the encoder.
  enc_tx_clk_enable
    Control the CHIPX8 clock on the transmit half of the encoder circuit.
  enc_tx_if_loopback_init
    Initialize the circuit to IF loopback mode.
  enc_tx_if_loopback
    Transmit a packet in the IF loopback mode.
  enc_set_baud_clock
    Setup the baud clock
  enc_voc_ctl
    Control whether the vocoder chip selects and frame reference are enabled.
  enc_codec_clk_ctl
    Control whether the codec clock is enabled.
  enc_acp_pa_ctl
    Control PA_ON and TX_PUNCT outputs for ACP PA control.
  enc_init_pa
    Initialize the PA to off, and PCN/not-PCN
  enc_pn_rand_delay
    Delay the transmit time by a specified number of chips.
  enc_hw_reset
    Reset Encoder HW block.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Enc_set_btf() and enc_init() should be called before any of the rest of the
  Encoder routines in order to get the chip into a known state.  Enc_set_mode()
  needs to be called before enc_data_out() is called, in order to transmit the
  data in the specific channel mode.

  On the Portable an Epoch must be given by the Demodulator initialization
  before the Interleaver can totally recover from chipx8 runt pulses from
  during the period when the chipx8 synthisizer is being programmed.

Copyright (c) 1992 through 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/src/enc.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/09   vlc     Added function enc_hw_reset() which resets encoder HW block.
02/23/09   jtm     Removed T_POSEIDON2 since this will only be supported 
                   in 1h09 branch
02/03/09   jtm     Added support for T_POSEIDON2
10/23/08   jtm     Removed T_QUASAR
08/15/08   jtm     Restored test_enc_init for FTM testing.
07/16/08   jtm     Removed unused code. Also reduced debug string sizes to reduce
                   ROM for ULC effort.
07/01/08   vlc     Merged code changes from old tip archive.
04/03/08   trc     Let only clkrgm handle clocks if FEATURE_SRCH_CLKREGIM_RM
03/31/08   mca     Changed FEATURE_CLKREGIM_RM to FEATURE_SRCH_CLKREGIM_RM
03/05/08   vlc     User clock resource manager to manage TX clocks 
                   (FEATURE_CLKREGIM_RM.)
05/21/08   vlc     Added changes for 7800 modem to mainline. 
06/29/07   vm      Made the change so that Encoder software controls
                   the enabling of Tx_ON_EN
04/17/07   trc     MSM7600 support for cx32 modulator clock override
03/22/07   rkc     Changed QLIC_GET_RTF_ADJUST() to GET_RTF_ADJUST()
                   under FEATURE_SRCH_SUPPORTS_QLIC.
02/27/07   vlc     In enc_set_mode(), check if 1X has primary chain before
                   switching encoder to 1x mode (case ENC_CDMA_MODE.)
02/23/07   sst     Add QLIC support
01/09/07   vlc     Merged the following change to main line from SC2X branch.
                   Mainly, adding support for FEATURE_MDSP_SUPPORTS_TX_AGC.
           vm      Added the support for Firmware TX AGC
12/07/06   vlc     In enc_init(), replaced call to enc_tx_clk_enable() with
                   in line code.  Reverted changes for TX DAC sleep below to
                   fix R-EACH.  Since R-EACH frame can be 5ms in stead of 20ms,
                   ENC must be ready much earlier than R-ACH.
                   Removed enc_codec_clk_state, not used.
11/22/07   vlc     Fixed RVCT compiler warnings.
10/25/06   vlc     Updated to the correct register for TX DAC sleep.
10/24/06   vlc     In enc_init(), put TX DAC to sleep until it is needed.  Moved
                   TX DAC sleep control code from enc_tx_clk_enable() to
                   enc_tx_enable() to avoid waking up TX DAC during power up.
10/09/06   trc     Add MSM7600 support
09/19/06   vlc     Fixed lint errors.
06/05/06   bn      Only turn of the TX path if 1x is the only client that uses the it
04/19/06   vlc     Fixed compiler warning.
03/24/06   bn      Used the defined TXC_IS95A_FCH_GAIN instead of the hardcode value
03/08/06   bn      Applied feature T_MSM6800B
12/02/05   rkc     Replace T_MSM7500_ASIC20 with T_MSM7500B.
09/27/05   rkc     In enc_tx_clk_enable(), for feature T_MSM7500_ASIC20, use
                   MODEM_TX_DAC_SLEEP instead of MODEM_MISC_CFG0.
09/12/05   rkc     Add logic in enc_tx_clk_enable() to set or reset TX_DAC_SLEEP
07/26/05   grl     Fixed code that affected the HDR modulator during SHDR.
05/31/05   ejv     Remove include of dmod.h.
05/11/05   lp/vlc  In function enc_init(), fixed writing a whole value into the
                   MODEM_MISC_CFG0 register just to set one bit, overwriting the
                   value that was there before.  This change is under T_MSM7500.
04/25/05   db/vlc  In function enc_tx_enable(), modified code which was overwriting the Band
                   Selection field of the PA_ON_CTL register.  This change is under
                   FEATURE_RUMI only.
04/22/05   vlc     Added calls to MUXMSM_SET_MOD_MODE_1X() in enc_set_mode()
                   (merged this change from MSMSHARED archive.)
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/15/05   vlc     Fixed typo in feature name.
03/14/05   vlc     Merged in changes for MSM7500.  (lp/dlb')
01/07/05   jyw     Added the featurization of T_IO_CARD
12/03/05   bn      Fixed complilation errors due to feature MSM6700
07/30/04   bn      Featurized the specific code for QUASAR-BB
07/29/04   jrp     Featurized hardcoded btf delay.
07/20/04   bn      Store the actual rev frame offset into the txcmodint variable
06/29/04   jrp     Removed obsolete register writes.  Added pass-through gain
                   writes.  Intialized new 6700 registers.  Write IS95A gain
                   to mdsp.
06/18/04   bn      Renamed ENC chan enum type and fix the bug in 16-bit transfer
05/04/04   bn      Corrected the write to ERAM error in enc_write_data
05/04/04   bn      Modified the enc_write_data and enc_write_done to use the real HW registers
03/05/04   bn      Checked in encoder driver changes for MSM6700
09/03/03   dna     Ported for MSM6500
08/08/03   vlc     Fixed typo in enc_dcch_data_out().
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/31/03   yll     Added the BTF compensation for Frame Offset.
06/25/03   vlc     Added support for Rel B.  Created enc_current_dcch_rc
                   and changed enc_dcch_data_out to use the RC of DCCH channel.
05/20/03   bmg     Forced modulator to 1x mode in enc_init()
05/19/03   bmg     Some initial Jaguar porting in enc_init()
01/06/03   bn      Added new function to handle encoder's writing for MUXPDU5
11/27/02   cr      Delete init of TX_VERY_EARLY_FRAME_CTL and
                   TX_2_EARLY_PCG_CTL in conjunction with these having
                   been added to RF VU cagc.h.
11/05/02   et/dyc  Use rf_get_tx_warmup() for Tx puncture.
04/25/02   sr      Called enc_set_btf() for both Cellular and PCS in
                   enc_init_pa()
03/22/02   rjr     cleaned up comments in test_enc_init.
03/06/02   dlb     Added support for RF FTM.
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   sr      Removed enc_version()
02/08/02   tc      Supported Reverse Enhance Access Channel.
02/06/02   ems     Merged in changes for MSM6050.  Added register initialization
                   for TX_WARMUP, MOD_PRMBL_GAIN, MOD_TEST_CTL_WH and featurized
                   enc_voc_ctl and enc_codec_clk_ctl.  Added Rx polarity switching
                   for T_IO_CARD.  Removed ENC_CHIPX8_CLK_DISABLE and
                   ENC_CHIPX8_CLK_ENABLE.
11/09/01   hrk     Using Macro ENC_OUTHM() to program 16-bit reg ENC_TST_CTL.
11/08/01   hrk     Added function enc_pcbit_test_mask() to program register
                   MOD_PCBIT_TEST_MASK.
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/17/01   tc      Cleaned up encoder initialization and set mode.
06/06/01   tc      Supported reverse pilot gating for control hold.
05/03/01   tc      Supported reverse link DCCH.
04/26/01   vlc     No longer need to include debug.h.  Added PLT Reverse Link
                   SCH RC4 support.
03/23/01   sr      Added support for 32X
03/16/01   tc      Cleaned up encoder driver.
03/14/01   tc      Load enc_stmr_cmd at enc_user_pn_mask and enc_user_pn_state.
03/09/01   tn      Added new function to support programmable PA WARMUP delay.
02/28/01   tn      Added support for FEATURE_ENC_BTF_IN_NV.
02/23/01   tc      Fixed RC3 R-FCH 1/4 rate.
02/14/01   tn      Added enc_set_rx_polarity() and enc_set_tx_polarity().
01/17/01   tc      Fixed enc_pcbit_enable() and moved agc contol to rf_init.
01/16/01   tc      Replaced DEBUG_IO_CARD to T_IO_CARD.
01/10/01   tc      Cleaned up encoder feature definition code.
01/03/01   tc      Initialized modulator system timer command.
12/20/00   tc      Initialized modulator preamble control register.
12/15/00   tc      Fixed encoder mask function.
12/05/00   vlc     Added #include "debug.h".
11/09/00   tc      Defined feature T_MSM_5100.
11/07/00   tc      Added MSM5100 modulator driver support.
10/31/00   et      fixed the clock problem in reverse link
10/24/00   rmd     Fixed the masking of the value written to MOD_MISC_CTL.
10/22/00   rmd     Clean up code and merged with MSM5000 code.
09/29/00   jcw     Added ENC_PA_ON/OFF macros to support MSM5105
09/07/00   jcw     Added temp changes for VERY_EARLY_FRAME register in MSM5105
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING around turbo coding software.
07/31/00   sr      Marked several functions with #ifndef MSM5000_IRAM_REV
07/14/00   sr      Changed ENC_NO_GATING_V to ENC_NO_GATING_PATTERN
06/22/00   sr      Added boolean generate_crc to enc_sch_data_out_cfg()
05/08/00   sr      Added support for MSM5000B
                   Added function enc_sch_data_out_cfg()
                   Added macro ENC_SCH_DATA_OUT()
04/25/00   sr      Added function definition for enc_pcbit_test_data()
04/11/00   sr      Merged in rel 0.3 changes:
                      Added function descriptions and removed one warning
                      as part of code cleanup for release 0.3
11/19/99   sr      Changes to support Supplemental Channels
10/13/99   lcc     Removed a debugging message.
10/07/99   lcc     Merged from PLT 2.0 (with SCH support).
09/23/99   lcc     Merged in MSM5000 and PLT support from PLT archive.
01/27/99   udp     Initialize the deinterleaver for sync channel mode from power-up
11/05/98   dlb     Added support for MSM3000.
12/10/97   aks     Added enc_wrt_btf_reg().  Adjusts for MSM23 BTF shift and for
                   modulo-8 BTF problem observed with MSM23 rev A3.
12/03/97   mk      Removed ENC_TEST_POINT_SEL, intitialized in dec_init().
06/16/97   aks     Added CRC disable.  Added ENC_TEST_POINT_SEL.  Changes are
                   related to MSM2.3.
01/28/97   jah     Removed CODEC clock override from enc_set_mode() for the
                   entry into CDMA mode.  It was overriding Voc's request
                   for the CODEC clocks, and causing muting.
05/14/96   jah/gb  Move PA-off responsibility to TXC to avoid an ordering
                   dependency with enc_codec_clk_ctl().
04/08/96   jah/gb  changed enc_tx_enable() to not turn off the PA, so that
                   it could turn off nicely at the end of the frame.
02/05/96   jah     Add ISS-2 to phones with leave the CODEC clock on during
                   the transition to CDMA state.
12/22/95   jah     Added hook to enc_init_pa() for BTF configuration.
06/23/95   jah     Really moved enc reset from MC to enc_init().
05/16/95   jah     Moved enc reset from MC to enc_init(), fixed enc_data_out().
03/20/95   jah     Added support for 14400 rate set and MSM2p encoder mods.
01/11/95   jah     Made enc_pn_rand_delay() available for Beta 1 (non-MSM).
10/20/94   jah     Changed enc_pn_rand_delay() to return previous delay.
10/11/94   jah     Added enc_pn_rand_delay() for Access Channel PN Rand.
06/27/94   jah     Added interlock between CODEC and Tx clocks.
03/07/94   jah     Improve some function comments.
12/21/93   jah     Have enc_init() preserve any selected interrupt timing.
09/28/93   jah     Updated header comments
06/23/93   jah     Changed enc_tx_enable() to put MASK_DATA low during the
                   time when the transmitter is disabled.  This is needed for
                   Beta II support.
05/11/93   jah     Added enc_chipx8_clk_enable/disable.
04/26/93   jah     Added changes for Beta II mobile.
03/15/93   jah     Added separate setting for the PCN/AMPS PA_WARMUP values.
03/11/93   jah     Changed enc_set_mode() and enc_tx_enable() for tx clk off.
01/14/93   jah     Have enc_set_mode() (T_P) not touch voc_frame_ref.
01/08/93   jah     Have enc_init() and enc_set_mode(ENC_CDMA_MODE) init the
                   vocoder selects and clocks as tri-stated (T_P-only).
12/23/92   jah     Changed enc_tx_enable(), and enc_set_mode() to turn PA_ON
                   'on' in enc_tx_enable(), as TX_PUNCT is goes off 1 frame
                   after it is 'set' to 'off'.
12/08/92   jah     Changed enc_init() to be portable-friendly.  This changed
                   the init state to have PA_ON low and TX_PUNCT tri-stated
                   in idle to allow the Portable to enable the PA during
                   FM mode when chipx8 is not available.
10/25/92   jah     Removed ENC_RESET for enc_init, with the note that the
                   Demod reset sends Epoch to straighten out the states that
                   get trashed by runt pulses during chipx8 programming.
10/13/92   jah     Updated enc_version() to use the version from the Demod
                   when using the MSM (portable and stretch targets).
09/01/92   jah     New PCN PA control needs no special processing.
08/31/92   jah     Added enc_init_pa() to configure PA control for PCN/not
08/07/92   jah     Fix bug in enc_init() which was turning on the PA.
08/04/92   jah     Inverted order of tx_punct/PA_ON for mobile RAS.
07/31/92   jah     Added enc_acp_pa_ctl() to control the PA for Analog calls.
07/28/92   jah     Added enc_voc_ctl() and enc_codec_clk_ctl(), fixed enc_init
                   to not trash the baud clock setting.
07/27/92   jah     Made enc_image[] private, added BIO init and PA_ON calls
01/30/92   jah     Ported from the brassboard enc.c/tx.c

===========================================================================*/


/*===========================================================================
Porting Issues:
        ENC_INIT_BTF should be calibrated to the hardware
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "enc.h"                /* Encoder typedefs and prototypes      */
#include "enci.h"               /* Encoder register & macro definitions */
#include "misc.h"               /* Miscellaneous utilities prototypes   */
#include "err.h"                /* ERR error logging macro              */
#include "target.h"             /* Target specific definitions          */
#include "bio.h"                /* Bit I/O macros                       */
#include "hw.h"                 /* Hardware Utilies                     */
#include "assert.h"             /* ASSERT macro unitilities             */
#include "rf.h"
#if defined(FEATURE_FTM_RF) && !defined(FEATURE_HWTC)
#include "ftm_rf_enc.h"
#endif
#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif
#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */
#include "srch_mux.h"
#ifdef FEATURE_SRCH_SUPPORTS_QLIC
#include "msm_drv.h"
#endif /* FEATURE_SRCH_SUPPORTS_QLIC */

#include "muxmsm.h"
#include "muxmdsp.h"
#include "txccommon.h"
#ifdef FEATURE_SRCH_CLKREGIM_RM
#include "clkregim.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define MSM5100_ENC_MASK_LO_V               0x04     /* Set MASK low                  */
#define MSM5100_ENC_MASK_V                  0x00     /* MASK functions normally       */

#define MSM5100_ENC_IS95A_MODE_M            0x8000
#define MSM5100_ENC_IS95A_MODE_ENA_V        0x8000
#define MSM5100_ENC_IS95A_MODE_DIS_V        0x0000

#define MSM5100_ENC_TX_BBA_M                0x2000
#define MSM5100_ENC_TX_BBA_ENA_V            0x2000
#define MSM5100_ENC_TX_BBA_DIS_V            0x0000

#ifdef T_IO_CARD
#error code not present
#endif

#ifdef FEATURE_ENC_CONTROL_TX_ON_EN
#define ENC_TX_ON_ENA_SIG_ON_VAL 1
#endif
/* The Encoder hardware interface is a set of registers.  The 'write' registers
** cannot be read, and must be buffered in RAM so that when a single bit is
** set or cleared, it does not effect the value of adjacent control bits.
**
** Note that enc_image is initialized to 0, and thus lint's message about
** "no explicit initialization" can be ignored.
*/
/*lint -esym(728,enc_image) */

/* CHIPX8 delay for SYNC80M, biased +25, set by calling enc_set_btf()
** Value for the back-to-the-future counter for Init and CDMA mode.
*/
LOCAL dword enc_btf_chipx8 = ENC_INIT_BTF;
//LOCAL dword enc_btf_chipx8 = 0x00000C80L;

/* PN Rand Delay in chips
*/
LOCAL word enc_pn_rand_delay_chips = 0;

/* Setting for forward and reverse channel frame offsets
*/
LOCAL byte enc_frame_off = 0x0;

#ifdef FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP
/* Setting for forward channel frame offset
*/
LOCAL byte enc_fwd_frame_off = 0x0;
#endif

/* Buffer of data to send in IF loopback mode (zeroes)
*/
LOCAL byte enc_lb_data[ ENC_9600_FULL_RATE_SIZE ];

/* Once we've killed the PA due to an error, we want it to stay dead.
** This variable lets the other routines know to not turn the PA back on.
*/
LOCAL boolean enc_pa_dead = FALSE;

/* True if we have a PCN applique, which means no PA_ON,
** and False if we have a regular PA.
*/
LOCAL boolean enc_have_pcn = FALSE;

/*
** Full rate frame size in bytes
*/
LOCAL const word enc_fch_full_rate_size[ ENC_NUM_FCH_RCS ] = { 24, 36, 24, 36 };

/* Frame size in bytes for all data rates (SCH RC3 and RC4).
*/
LOCAL const word enc_sch_frm_size_tab
            [ ENC_NUM_SCH_RCS ][ ENC_NUM_SCH_RATES ] =
            {
              {
                   3,   6,  12,  24,  48,  96, 192, 384
#ifdef FEATURE_32X_SCH_DATA_RATE
                ,768                          /* RC3 32X:  307.2 Kbps    */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              },
              {
                   5,   9,  18,  36,  72, 144, 288, 576
#ifdef FEATURE_32X_SCH_DATA_RATE
                  ,0                          /* RC4 32X:  not supported */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              }
            };

LOCAL const enc_crc_info_type enc_fch_crc_info_tab
            [ ENC_NUM_FCH_RCS ][ ENC_NUM_FRAC_RATES ] =
            {
               { {0,0},      {0,0},      {8,0x9B00},  {12,0xF130} },
               { {6,0x1C00}, {8,0x9B00}, {10,0xF640}, {12,0xF130} },
               { {6,0x9C00}, {6,0x9C00}, {8,0x9B00},  {12,0xF130} },
               { {6,0x9C00}, {8,0x9B00}, {10,0xF640}, {12,0xF130} }
            };

LOCAL const enc_crc_info_type enc_sch_crc_info_tab
            [ ENC_NUM_SCH_RCS ][ ENC_NUM_SCH_RATES ] =
          {
            {
              {6,  0x9C00}, {6,  0x9C00}, {8,  0x9B00}, {12, 0xF130},
              {16, 0xC867}, {16, 0xC867}, {16, 0xC867}, {16, 0xC867}
#ifdef FEATURE_32X_SCH_DATA_RATE
             ,{16, 0xC867}                   /* RC3 32X:  307.2 Kbps    */
#endif /* FEATURE_32X_SCH_DATA_RATE */
            },
            {
              {6,  0x9C00}, {8,  0x9B00}, {10, 0xF640}, {12, 0xF130},
              {16, 0xC867}, {16, 0xC867}, {16, 0xC867}, {16, 0xC867}
#ifdef FEATURE_32X_SCH_DATA_RATE
             ,{0,  0     }                   /* RC4 32X:  not supported */
#endif /* FEATURE_32X_SCH_DATA_RATE */
            }
          };

/*
** c.f. table 2.1.3.1.2.1-8 and -9 of IS-2000.2 and ch. 8 of 80-24477-1
*/
LOCAL const word enc_fch_punct_0_tab
            [ ENC_NUM_FCH_RCS ][ ENC_NUM_FRAC_RATES ] =
            {
              { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
              { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
              { 0x0000, 0x0000, 0xFFFF, 0xFFFF },
              { 0xBAEA, 0xBAEA, 0xBAEA, 0xBAEA }
            };

LOCAL const word enc_fch_punct_1_tab
            [ ENC_NUM_FCH_RCS ][ ENC_NUM_FRAC_RATES ] =
            {
              { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
              { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
              { 0x00F0, 0x00FF, 0xFFFF, 0xFFFF },
              { 0x00EB, 0x00EB, 0x00EB, 0x00EB }
            };

LOCAL const word enc_sch_punct_0_tab
            [ ENC_NUM_SCH_RCS ][ ENC_NUM_SCH_RATES ] =
            {
              { 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
#ifdef FEATURE_32X_SCH_DATA_RATE
               ,0xFFFF                       /* RC3 32X:  307.2 Kbps    */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              },
              { 0xBAEA, 0xBAEA, 0xBAEA, 0xBAEA, 0xB000, 0xB000, 0xB000, 0xB000
#ifdef FEATURE_32X_SCH_DATA_RATE
               ,0xFFFF                       /* RC4 32X:  not supported */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              }
            };

LOCAL const word enc_sch_punct_1_tab
            [ ENC_NUM_SCH_RCS ][ ENC_NUM_SCH_RATES ] =
            {
              { 0x00F0, 0x00FF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
#ifdef FEATURE_32X_SCH_DATA_RATE
               ,0xFFFF                       /* RC3 32X:  307.2 Kbps    */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              },
              { 0x00EB, 0x00EB, 0x00EB, 0x00EB, 0x00D9, 0x00D9, 0x00D9, 0x00D9
#ifdef FEATURE_32X_SCH_DATA_RATE
               ,0xFFFF                       /* RC4 32X:  not supported */
#endif /* FEATURE_32X_SCH_DATA_RATE */
              }
            };

LOCAL const enc_code_rate_type enc_code_rate_tab[ ENC_NUM_FCH_RCS ]  =
                                          {
                                             ENC_CODE_1_BY_3_RATE,
                                             ENC_CODE_1_BY_2_RATE,
                                             ENC_CODE_1_BY_4_RATE,
                                             ENC_CODE_1_BY_4_RATE
                                          };

LOCAL const enc_rate_set_type enc_rate_set_tab[ ENC_NUM_FCH_RCS ] =
                                          {
                                             ENC_RATE_9600,
                                             ENC_RATE_14400,
                                             ENC_RATE_9600,
                                             ENC_RATE_14400
                                          };

/* These tables are here temproarily.  Once reverse link gain control is ready,
   the actual implementation may change */
LOCAL const byte enc_pch_gain_tab[ ENC_NUM_FRAC_RATES ] = {228,208,180,147};
LOCAL const byte enc_fch_gain_tab[ ENC_NUM_FRAC_RATES ] = {114,147,180,208};

/* Current rate set configured.  Initially 9600.
*/
LOCAL enc_fch_radio_config_type enc_current_fch_rc = ENC_FCH_RC_1;
LOCAL enc_sch_radio_config_type enc_current_sch_rc = ENC_SCH_RC_3;


#ifdef FEATURE_IS2000_REL_B
LOCAL enc_fch_radio_config_type enc_current_dcch_rc = ENC_FCH_RC_3;
#endif /* FEATURE_IS2000_REL_B */

#define ENC_22_TO_23A3_BTF_SHIFT  1           /* Tx delay diff. msm22->msm23*/
#define ENC_23A3_BTF_MOD          8           /* Shift amount of mod-8 bug  */
#define ENC_23A3_BTF_M            0x00000007  /* Mask for BTF modulo-8 fix  */

#ifdef FEATURE_ENC_CONTROL_TX_ON_EN
#define ENC_PA_ON() \
    HWIO_OUTM( PA_ON_CTL, HWIO_FMSK(PA_ON_CTL,PA_ON_EN), HWIO_FMSK(PA_ON_CTL,PA_ON_EN) ); \
    MSM_W_MASKED(RXF_MODEM(PA_ON_CTL), TX_ON_EN, ENC_TX_ON_ENA_SIG_ON_VAL)
#else
#define ENC_PA_ON() \
    HWIO_OUTM( PA_ON_CTL, HWIO_FMSK(PA_ON_CTL,PA_ON_EN), HWIO_FMSK(PA_ON_CTL,PA_ON_EN) )
#endif

#define ENC_PA_OFF() \
   HWIO_OUTM( PA_ON_CTL, HWIO_FMSK(PA_ON_CTL,PA_ON_EN), 0x00 )

static boolean enc_pilot_gating = FALSE;

#ifdef FEATURE_IS2000_REL_A_CC
//static boolean enc_reach_on = TRUE;

#define ENC_SCH_INTLV_SIZE_768_V     0x0020
#define ENC_SCH_INTLV_SIZE_1536_V    0x0040
#define ENC_SCH_INTLV_SIZE_3072_V    0x0060
#endif

/*===========================================================================

            LOCAL FUNCTION PROTOTYPES

===========================================================================*/

void enc_wrt_btf_reg( dword btf_value );


/*===========================================================================

FUNCTION ENC_INIT

DESCRIPTION
  Initialize the circuit to Idle.

DEPENDENCIES
  If enc_set_btf() is not called first, a default value will be used here.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the circuit to Idle, disables the transmit path, the PA, and
  puts the circuit in a state where it is drawing minimal power.  The CHIPX8
  clock is enabled though.  Undoes any effects from enc_pn_rand_delay().

===========================================================================*/
void enc_init( void )
{
#ifdef FEATURE_SRCH_CLKREGIM_RM
  /* Enable clock regimes before accessing TX registers and initialize MDSP */
  MSG_MED ("Enable CDMA Tx and MDSP clocks",0,0,0);
  clk_regime_resource_enable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TX);
  clk_regime_resource_enable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_MDSP);
#endif

  MUXMSM_SET_MOD_MODE_1X();
  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  CDMA_MDSP_SET_MOD_PHASE_ROTATOR(ENC_NO_MOD_ROTATION_V);
  #else
  HWIO_OUTM( MOD_ROTATOR_MAP, HWIO_RMSK(MOD_ROTATOR_MAP), ENC_NO_MOD_ROTATION_V );
  #endif

  /* Restore the GPIO's by setting the outputs with a mask of 0 so that
  ** the bits are set, but none are changed.
  */
  BIO_OUT( ENC_GPIO_0, 0x00, 0x00);
  BIO_OUT( ENC_GPIO_1, 0x00, 0x00);
  BIO_TRISTATE( ENC_GPIO_0, 0x00, 0x00);

  /* Turn off the PA */
  ENC_PA_OFF();

  /* Initialize Back-to-the-future counter for this particular hardware.
  ** Note that this relates to the time through the FIR & Tx RF to the
  ** antenna, and back down through Rx RF & Demod.
  */
  enc_set_btf( enc_btf_chipx8 );

#ifndef FEATURE_SRCH_CLKREGIM_RM
  /* Clock control:  Enable Tx Clocks
  */
#ifndef T_IO_CARD
  HWIO_OUTM( MOD_CLK_CTL, ENC_TX_CLKS_DIS_M, ENC_TX_CLKS_ENA_V );
#endif
#endif /* !FEATURE_SRCH_CLKREGIM_RM */

  /* Miscellaneous functions control:
  **    Tx puncture tri-stated (on), Interleaver NOT bypassed,
  **    Deinterleaver NOT bypassed, Sync channel not being received,
  **    Access channel not being transmitted, Tx data fmt is offset-binary
  */
  HWIO_OUTM(MOD_MISC_CTL, (HWIO_FMSK(MOD_MISC_CTL,PA_CTL) | HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH) |
             HWIO_FMSK(MOD_MISC_CTL,RESERVED_BITS6) | HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN) |
              HWIO_FMSK(MOD_MISC_CTL,PICH_GATING_RATE) | HWIO_FMSK(MOD_MISC_CTL,FCH_EIGHTH_GATE_EN) |
              HWIO_FMSK(MOD_MISC_CTL,RETRANS_DIS) | HWIO_FMSK(MOD_MISC_CTL,IS95A_MODE)),
            (ENC_TXP_ON_V | ENC_NO_ACCESS_V | ENC_TX_OFF_V | ENC_NO_IS95C_PILOT_V |
              0 << HWIO_SHFT(MOD_MISC_CTL, PICH_GATING_RATE) | 0 << HWIO_SHFT(MOD_MISC_CTL, FCH_EIGHTH_GATE_EN) |
              0 << HWIO_SHFT(MOD_MISC_CTL, RETRANS_DIS) | 1 << HWIO_SHFT(MOD_MISC_CTL, IS95A_MODE))
            );

  HWIO_OUT( FRAME_OFF, 0x00 );      /* Store the offsets */

  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  CDMA_MDSP_SET_MOD_PHASE_ROTATOR(0x0);
  #else
  HWIO_OUT( MOD_ROTATOR_MAP, 0x00 );
  #endif

  HWIO_OUT( MOD_WSYM_STATE, 0x00 );

  HWIO_OUT( MOD_CLK_CTL, 0x00 );

  HWIO_OUT( MOD_TEST_CTL, 0x00 );

  HWIO_OUT( MOD_PICH_PCBIT_DATA, 0xffff );

  HWIO_OUT( MOD_PICH_PCBIT_MASK, 0x0000 );

  /* Specify Frame stagger offset to 0x0 for forward/reverse
  ** traffic frames.  This is the offset from the system frame.
  */
  enc_set_frame_off( 0x0, 0x0 );

  /* Test control:
  **    Don't force I & Q high after pilot spreading, Set internal mask Low
  **    (note that this masks Tx), Tx data source is the FIR filter.
  **    Takes effect on the next transmit frame boundary.
  */

  HWIO_OUTM( MOD_TEST_CTL,
             HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH) | HWIO_FMSK(MOD_TEST_CTL,MASK_CTL) | HWIO_FMSK(MOD_TEST_CTL,TX_DATA_CTL),
             ENC_NO_SET_IQ_V | MSM5100_ENC_MASK_LO_V | ENC_TX_FIR_V );

  /* Load PA_WARMUP register, this can differ between the
  ** PCN / AMPS versions of the RAS.
  */

  if ( enc_have_pcn ) {
#ifndef FEATURE_SUBPCG_PA_WARMUP_DELAY
    HWIO_OUT( PA_WARMUP, ENC_PCN_PA_WARMUP_V );
    HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
#else
  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( PA_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
    HWIO_OUT( PA_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */

  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( TX_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
    HWIO_OUT( TX_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */
#endif
  } else {
#ifndef FEATURE_SUBPCG_PA_WARMUP_DELAY
    HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
    HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
#else
  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( PA_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
    HWIO_OUT( PA_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */

  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( TX_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
    HWIO_OUT( TX_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */
#endif
  }

  #if defined T_RUMI
  HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
  HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
  #endif /* T_RUMI */

  // Written in muxmdsp_cdma_init()
  //HWIO_OUTM( MOD_DCCH_GAIN, HWIO_RMSK(MOD_DCCH_GAIN), ENC_ZERO_GAIN_V);

  HWIO_OUTM( MOD_CH1_CRC_POLY, HWIO_RMSK(MOD_CH1_CRC_POLY), ENC_CLEAR_CRC_POLY_V);

  HWIO_OUTM( MOD_CH1_PUNCT_PATN_0, HWIO_RMSK(MOD_CH1_PUNCT_PATN_0),
             ENC_PUNC_PATTERN_NONE_V);

  HWIO_OUTM( MOD_CH1_PUNCT_PATN_1, HWIO_RMSK(MOD_CH1_PUNCT_PATN_1),
             ENC_PUNC_PATTERN_NONE_V);

  HWIO_OUTM( MOD_CH3_PUNCT_PATN_0, HWIO_RMSK(MOD_CH3_PUNCT_PATN_0),
             ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH2_ENC_CTL_0, HWIO_RMSK(MOD_CH2_ENC_CTL_0), ENC_FCH_CTL_DIS_V );
  HWIO_OUTM( MOD_CH3_ENC_CTL_0, HWIO_RMSK(MOD_CH3_ENC_CTL_0), ENC_SCH_CTL_DIS_V);

  /* Latch the gating bits every 20ms */
  HWIO_OUTM( MOD_TEST_CTL,
             HWIO_FMSK(MOD_TEST_CTL,PCH_GATE_5MS_LAT),
             ENC_PCH_GRATE_20MS_LAT_V);

  HWIO_OUTM( MOD_CH2_CRC_POLY, HWIO_RMSK(MOD_CH2_CRC_POLY), ENC_CLEAR_CRC_POLY_V);
  HWIO_OUTM( MOD_CH3_CRC_POLY, HWIO_RMSK(MOD_CH2_CRC_POLY), ENC_CLEAR_CRC_POLY_V);
  HWIO_OUTM( MOD_SCH_LTU_CTL, HWIO_RMSK(MOD_SCH_LTU_CTL), ENC_SCH_LTU_SIZE_ZERO_V);
  HWIO_OUTM( MOD_CH2_PUNCT_PATN_0, HWIO_RMSK(MOD_CH2_PUNCT_PATN_0), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH2_PUNCT_PATN_1, HWIO_RMSK(MOD_CH2_PUNCT_PATN_1), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH3_PUNCT_PATN_1, HWIO_RMSK(MOD_CH3_PUNCT_PATN_1), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUT( MOD_PICH_PCBIT_DATA, ENC_ZERO_PCBIT_V );

  enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
  enc_gating_mode( ENC_NO_GATING_MODE );
  enc_dcch_enable( FALSE );

  /* Reset the circuit again, in case it was "stuck" from an illegal state
  ** of the registers from start-up.  Notably, if CHIPX8 was off, this is
  ** the first reset that actually does anything.  Reset is an address-
  ** activated register, and has no effect when CHIPX8 is off.
  **
  ** This is not needed with the MSM.
  */
  HWIO_OUT( MOD_RESET, 0x0 );

  enc_wrt_btf_reg( enc_btf_chipx8 );    /* Set the BTF register   */

  HWIO_OUT( MOD_STMR_CMD, 0x00 );

  /* Restore the GPIO's by setting the outputs with a mask of 0 so that
  ** the bits are set, but none are changed.
  */
  BIO_OUT( ENC_GPIO_0, 0x00, 0x00);
  BIO_OUT( ENC_GPIO_1, 0x00, 0x00);
  BIO_TRISTATE( ENC_GPIO_0, 0x00, 0x00);
  BIO_TRISTATE( ENC_GPIO_1, 0x00, 0x00);

  /* Clear the test point select register. */
  HWIO_OUT (MOD_TEST_POINT_SEL, 0x0000);

  /* Disable R-SPICH */
  HWIO_OUTM( MOD_SPICH_CTL, HWIO_RMSK(MOD_SPICH_CTL),
    (0 << HWIO_SHFT(MOD_SPICH_CTL, EN) & HWIO_FMSK(MOD_SPICH_CTL, EN))
  );

  /* Disable R-PDCCH */
  HWIO_OUTM( MOD_PDCCH_CTL, HWIO_RMSK(MOD_PDCCH_CTL),
    (0 << HWIO_SHFT(MOD_PDCCH_CTL, EN) & HWIO_FMSK(MOD_PDCCH_CTL, EN))
  );

  /* Disable R-PDCH */
  HWIO_OUTM( MOD_PDCH_CTL, HWIO_RMSK(MOD_PDCH_CTL),
    (0 << HWIO_SHFT(MOD_PDCH_CTL, EN) & HWIO_FMSK(MOD_PDCH_CTL, EN))
  );

  /* Disable R-REQCH */
  HWIO_OUTM( MOD_CH3_ENC_CTL_1, HWIO_RMSK(MOD_CH3_ENC_CTL_1),
    (0 << HWIO_SHFT(MOD_CH3_ENC_CTL_1, CH3_SEL) & HWIO_FMSK(MOD_CH3_ENC_CTL_1, CH3_SEL))
  );

  /* Select SW Control for clocks */
  HWIO_OUTM(MOD_HW_CLK_CTL, HWIO_RMSK(MOD_HW_CLK_CTL),
    (0 << HWIO_SHFT(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA) & HWIO_FMSK(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA))
    | (0 << HWIO_SHFT(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA) & HWIO_FMSK(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA))
    | (0 << HWIO_SHFT(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA) & HWIO_FMSK(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA))
  );

#ifndef FEATURE_SRCH_CLKREGIM_RM
  /* Override all clocks on */
  #if defined(T_MSM7600)
  /* For 7600, we also need cx32 modulator support, but we'll take the previous
     mask of 0111 and turn it into 1111.  The SC2x case below, for whatever
     reason, sets the PDM clk override bit to 0.
     This all will be straightened out when we get the HWIO file and supporting
     hardware information.
   */
  HWIO_OUT(MOD_MICRO_CLK_CTL,0xF);
  #elif defined(FEATURE_MDSP_SUPPORTS_TX_AGC)
  /* This is done to set the cx32 for the modulator which is not defined in MODEM SWI.
  ** The hard coded value should be changed to bit masks when the register files are updated.
  */
  HWIO_OUT(MOD_MICRO_CLK_CTL,0xB);
  #else
  HWIO_OUTM(MOD_MICRO_CLK_CTL, HWIO_RMSK(MOD_MICRO_CLK_CTL),
    (1 << HWIO_SHFT(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE) & HWIO_FMSK(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE))
    | (1 << HWIO_SHFT(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE) & HWIO_FMSK(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE))
    | (1 << HWIO_SHFT(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE) & HWIO_FMSK(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE))
  );
  #endif
#endif /* !FEATURE_SRCH_CLKREGIM_RM */

  /* Set PA control to CDMA operation rather than HDR */
  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  HWIO_OUTM(MOD_PA_ON_CTL, HWIO_RMSK(MOD_PA_ON_CTL),
    (1 << HWIO_SHFT(MOD_PA_ON_CTL, PA_RANGE_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, PA_RANGE_SEL))
    | (0 << HWIO_SHFT(MOD_PA_ON_CTL, TX_PUNCT_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, TX_PUNCT_SEL))
    | (0 << HWIO_SHFT(MOD_PA_ON_CTL, PA_PUNCT_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, PA_PUNCT_SEL))
  );
  #else
  HWIO_OUTM(MOD_PA_ON_CTL, HWIO_RMSK(MOD_PA_ON_CTL),
    (0 << HWIO_SHFT(MOD_PA_ON_CTL, PA_RANGE_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, PA_RANGE_SEL))
    | (0 << HWIO_SHFT(MOD_PA_ON_CTL, TX_PUNCT_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, TX_PUNCT_SEL))
    | (0 << HWIO_SHFT(MOD_PA_ON_CTL, PA_PUNCT_SEL) & HWIO_FMSK(MOD_PA_ON_CTL, PA_PUNCT_SEL))
  );
  #endif
  /* Set reset interface on for dac output */
  HWIO_OUTM(MOD_STMR_CMD, HWIO_FMSK(MOD_STMR_CMD, RST_INTF_CNT), 0 << HWIO_SHFT(MOD_STMR_CMD, RST_INTF_CNT) ); //JRP

  /* Put TX DAC to sleep until it is needed.  This step is not necessary as the
  ** HW default is already set to this state.  Still, this code is added so it
  ** is more visible.
  */

  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0, MDSP_TXDAC_SW_EN),
              ( 0 << HWIO_SHFT (MODEM_MISC_CFG0, MDSP_TXDAC_SW_EN)) );

  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0, GSM_MODE),
              ( 0 << HWIO_SHFT (MODEM_MISC_CFG0, GSM_MODE)) );

#if defined T_MSM7500B || defined(T_MSM6800B)
  HWIO_OUTM ( MODEM_TX_DAC_SLEEP, HWIO_FMSK( MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP),
              ( 1 << HWIO_SHFT (MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP)) );
#else
  HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,TX_DAC_SLEEP),
              ( 1 << HWIO_SHFT (MODEM_MISC_CFG0,TX_DAC_SLEEP )) );
#endif /* T_MSM7500B || T_MSM6800B */

  muxmdsp_cdma_init();

#ifdef FEATURE_SRCH_CLKREGIM_RM
  /* Disable clock regimes */
  MSG_MED ("Disable CDMA Tx and MDSP clocks",0,0,0);
  clk_regime_resource_disable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_CDMA_TX);
  clk_regime_resource_disable(CLKRGM_CLIENT_1X, CLKRGM_RESOURCE_MDSP);
#endif
} /* end of enc_init */

#if defined(FEATURE_FTM_RF) || defined(FEATURE_HWTC)
/*===========================================================================

FUNCTION TEST_ENC_INIT

DESCRIPTION
  Initialize the circuit to Idle.

DEPENDENCIES
  If enc_set_btf() is not called first, a default value will be used here.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the circuit to Idle, disables the transmit path, the PA, and
  puts the circuit in a state where it is drawing minimal power.  The CHIPX8
  clock is enabled though.  Undoes any effects from enc_pn_rand_delay().

===========================================================================*/
void test_enc_init( void )
{
  int mod_addr;
  int i;

  /* Initialize all the Modulator write registers */
  mod_addr = (HWIO_ADDR(MOD_WCOVER_SEL) - HWIO_ADDR(MOD_RESET));

  for (i = 0; i <= mod_addr; i=i+4)
  {
      outpw((HWIO_ADDR(MOD_RESET)+i), 0x0000);
  }

  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
  CDMA_MDSP_SET_MOD_PHASE_ROTATOR(ENC_NO_MOD_ROTATION_V);
  #else
  HWIO_OUTM( MOD_ROTATOR_MAP, HWIO_RMSK(MOD_ROTATOR_MAP), ENC_NO_MOD_ROTATION_V );
  #endif

  MUXMSM_AGC_CTL_TX_MSM3100_MODE();

  /* Turn off the PA */
  ENC_PA_OFF();

  /* Initialize Back-to-the-future counter for this particular hardware.
  ** Note that this relates to the time through the FIR & Tx RF to the
  ** antenna, and back down through Rx RF & Demod.
  */
  enc_set_btf( enc_btf_chipx8 );

  /* Set the time offset of the Frame Interrupt from
  ** the Transmit Frame Boundary.
  */
  enc_int_timing_offset( ENC_INIT_5MS_BIN0_V, ENC_INIT_PCG_0_V, ENC_INIT_WS_T_OFF );

  enc_tx_clk_enable( TRUE );

  /* Miscellaneous functions control:
  **    Tx puncture tri-stated (on), Interleaver NOT bypassed,
  **    Deinterleaver NOT bypassed, Sync channel not being received,
  **    Access channel not being transmitted, Tx data fmt is offset-binary
  */
  HWIO_OUTM( MOD_MISC_CTL,
             HWIO_FMSK(MOD_MISC_CTL,PA_CTL) | HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH) | ENC_TX_FMT_M |
             ENC_IS95C_PCBIT_M | ENC_IS95C_PILOT_M,
             ENC_TXP_ON_V | ENC_NO_ACCESS_V | ENC_TX_OFF_V |
             ENC_NO_IS95C_PCBIT_V | ENC_NO_IS95C_PILOT_V );

  /* Specify Frame stagger offset to 0x0 for forward/reverse
  ** traffic frames.  This is the offset from the system frame.
  */
  enc_set_frame_off( 0x0, 0x0 );

  /* Test control:
  **    Don't force I & Q high after pilot spreading, Set internal mask Low
  **    (note that this masks Tx), Tx data source is the FIR filter.
  **    Takes effect on the next transmit frame boundary.
  */
  HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,SET_IQ_HIGH) | HWIO_FMSK(MOD_TEST_CTL,MASK_CTL) | HWIO_FMSK(MOD_TEST_CTL,TX_DATA_CTL),
                         ENC_NO_SET_IQ_V | ENC_MASK_LO_V | ENC_TX_FIR_V );


  /* Load PA_WARMUP register, this can differ between the
  ** PCN / AMPS versions of the RAS.
  */
  if ( enc_have_pcn ) {
    HWIO_OUT( PA_WARMUP, ENC_PCN_PA_WARMUP_V );
  } else {
    HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
  }

  HWIO_OUTM( MOD_CH2_ENC_CTL_0, HWIO_RMSK(MOD_CH2_ENC_CTL_0), ENC_FCH_CTL_DIS_V );
  HWIO_OUTM( MOD_CH3_ENC_CTL_0, HWIO_RMSK(MOD_CH3_ENC_CTL_0), ENC_SCH_CTL_DIS_V);

  muxmdsp_wr_mod_pch_gain(0);
  muxmdsp_wr_mod_fch_gain(0);
  muxmdsp_wr_mod_dcch_gain(0);
  muxmdsp_wr_mod_sch_gain(0);

  HWIO_OUTM( MOD_CH1_CRC_POLY, HWIO_RMSK(MOD_CH1_CRC_POLY), ENC_CLEAR_CRC_POLY_V);
  HWIO_OUTM( MOD_CH2_CRC_POLY, HWIO_RMSK(MOD_CH2_CRC_POLY), ENC_CLEAR_CRC_POLY_V);
  HWIO_OUTM( MOD_CH3_CRC_POLY, HWIO_RMSK(MOD_CH3_CRC_POLY), ENC_CLEAR_CRC_POLY_V);

  HWIO_OUTM( MOD_SCH_LTU_CTL, HWIO_RMSK(MOD_SCH_LTU_CTL), ENC_SCH_LTU_SIZE_ZERO_V);

  HWIO_OUTM( MOD_CH1_PUNCT_PATN_0, HWIO_RMSK(MOD_CH1_PUNCT_PATN_0), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH1_PUNCT_PATN_1, HWIO_RMSK(MOD_CH1_PUNCT_PATN_1), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH2_PUNCT_PATN_0, HWIO_RMSK(MOD_CH2_PUNCT_PATN_0), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH2_PUNCT_PATN_1, HWIO_RMSK(MOD_CH2_PUNCT_PATN_1), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH3_PUNCT_PATN_0, HWIO_RMSK(MOD_CH3_PUNCT_PATN_0), ENC_PUNC_PATTERN_NONE_V);
  HWIO_OUTM( MOD_CH3_PUNCT_PATN_1, HWIO_RMSK(MOD_CH3_PUNCT_PATN_1), ENC_PUNC_PATTERN_NONE_V);

  HWIO_OUT( MOD_PICH_PCBIT_DATA, ENC_ZERO_PCBIT_V );

  enc_dcch_enable( FALSE );

  HWIO_OUTM( MOD_MISC_CTL,
                    HWIO_FMSK(MOD_MISC_CTL,IS95A_MODE)        | HWIO_FMSK(MOD_MISC_CTL,RETRANS_DIS) |
                    HWIO_FMSK(MOD_MISC_CTL,FCH_EIGHTH_GATE_EN) | HWIO_FMSK(MOD_MISC_CTL,PICH_GATING_RATE),
                    MOD_MISC_CTL_IS95A_MODE          | MOD_MISC_CTL_RETRANS_DIS |
                   MOD_MISC_CTL_FCH_EIGHTH_GATE_DIS | MOD_MISC_CTL_PICH_GATING_RATE_FULL );

  HWIO_OUTM( MOD_CH2_ENC_CTL_1,
                    MOD_FCH_ENC_CTL_WB__FCH_RADIO_CFG_MASK | MOD_FCH_ENC_CTL_WB__FCH_FRAME_SIZE_MASK,
                     ENC_FCH_RC1_V | ENC_FCH_20MS_FRAME_V );
  enc_dcch_gain( 0 );
  enc_sch_gain( 0 );
  enc_sch_enable( FALSE ); /* in IS95-A mode, the IS95-C SCH enable bit must be cleared */
  enc_fch_enable( TRUE ); /* in IS95-A mode, the IS95-C FCH enable bit must be set */

  /* Restore the GPIO's by setting the outputs with a mask of 0 so that
  ** the bits are set, but none are changed.
  */
  BIO_OUT( ENC_GPIO_0, 0x00, 0x00);
  BIO_OUT( ENC_GPIO_1, 0x00, 0x00);
  BIO_TRISTATE( ENC_GPIO_0, 0x00, 0x00);
  BIO_TRISTATE( ENC_GPIO_1, 0x00, 0x00);

} /* end of test_enc_init */
#endif  // FEATURE_FTM_RF || FEATURE_HWTC 


/*===========================================================================

FUNCTION ENC_SET_BTF

DESCRIPTION
  Load the back-to-the-future, and set the back-to-the-future value loaded
  by enc_init().  Note that this value is biased +25, so to set a value of
  100 into the counter, the parameter must be 125.

  Takes effect at the next SYNC80M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Undoes any effects from enc_pn_rand_delay().

===========================================================================*/
void enc_set_btf
(
  dword btf_chipx8_delay        /* CHIPX8 delay for SYNC80M, biased +25 */
)
{
  #ifdef T_QUASAR_BB
#error code not present
  #endif

  enc_btf_chipx8 = btf_chipx8_delay;    /* Set for next enc_init() */

  enc_wrt_btf_reg( enc_btf_chipx8 );    /* Set the BTF register   */

} /* end of enc_set_btf */


/*===========================================================================

FUNCTION ENC_SET_FRAME_OFF

DESCRIPTION
  Specify Frame stagger offset for forward/reverse traffic frames
  This is the offset from the system frame in power control groups.
  This offset has no effect while in Sync Channel mode.  The value
  is reset to 0 offset by enc_init().

  Takes effect at the next SYNC80M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern byte txcmod_int_frame_offset;
void enc_set_frame_off
(
  byte fwd_off,         /* Forward channel frame stagger */
  byte rev_off          /* Reverse channel frame stagger */
)
{
  enc_frame_off = rev_off & 0x0f;

  #ifdef FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP
  enc_fwd_frame_off = fwd_off;
  #endif

  HWIO_OUT( FRAME_OFF, enc_frame_off );      /* Store the offsets */
  /* store it for the txcmod interrupt handler */
  txcmod_int_frame_offset = enc_frame_off;

  /* Update the mDSP copy of frame offset */
  muxmdsp_wr_fundFrameOffset (rev_off & 0x0f);

} /* end of enc_set_frame_off */


/*===========================================================================

FUNCTION ENC_SET_MODE

DESCRIPTION
  Sets the operating mode of the driver: sleep, cdma, sync, paging, traffic.

DEPENDENCIES
  CDMA mode  - must not be entered before 20ms after enc_init().
  Sleep mode - must not be entered before 20ms after enc_tx_enable(FALSE).

RETURN VALUE
  None

SIDE EFFECTS
  Changes the operating mode of the circuit.
  Sleep mode - masks the transmit path, turns off the PA, and disables
               the CHIPX8 clocks to the circuit.  PN state is lost.
  CDMA mode  - masks the transmit path, turns ON the PA, and enables the
               CHIPX8 clocks to the circuit.  If previously in sleep mode,
               the circuit will not re-synchronize until the next SYNC80M.
  SC mode    - initialize the circuit for the Sync Channel
  PC mode    - initialize the circuit for the Paging Channel
  TC mode    - initialize the circuit for the Traffic Channel

===========================================================================*/
void enc_set_mode
(
  enc_mode_type channel_mode    /* Mode in which the circuit is to run */
)
{
  MSG_MED("Enc mode: %d", channel_mode,0,0);

  /* This switch selects between the various modes to initialize into.
  */
  switch (channel_mode) {
    case ENC_CDMA_MODE:         /* CDMA Start mode */
      /* If 1X owns the primary chain, switch encoder/modulator to 1x mode. */
      if ( srch_mux_owns_tx() )
      {
        MUXMSM_SET_MOD_MODE_1X();
      }

      /* All of the write registers should be written to before the Interleaver
      ** can be considered operational.  Power-up states cannot be relied on
      ** for consistent operation.  The registers needed to "unstick" the ASIC
      ** are written in enc_init(), the rest of the values are written now.
      */
      HWIO_OUT( MOD_WSYM_STATE, 0x00 );   /* Walsh symbol clock offset */

      /* Set the back to the future count
      */
      enc_set_btf( enc_btf_chipx8 );

      HWIO_OUT(MOD_CH1_CRC_POLY, 0x0000);
      HWIO_OUT(MOD_CH2_CRC_POLY, 0x0000);
      HWIO_OUT(MOD_CH3_CRC_POLY, 0x0000);

      /*
      ** Turn OFF CRCs
      */
      HWIO_OUTM( MOD_CH1_ENC_CTL, ENC_DCCH_CRCLEN_M, ENC_CRCDIS_V );
      HWIO_OUTM( MOD_CH2_ENC_CTL_0, ENC_FCH_CRCLEN_M, ENC_CRCDIS_V );
      HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_SCH_CRCLEN_M, ENC_CRCDIS_V );
      /* Clock control:
      **    Turn off the Tx Clocks, CODEC_CLK & CODEC_SYNC,
      **    but do not override the CODEC clock control interface
      **    by calling enc_codec_clk_ctl().
      */

      /* only turn off the TX is 1x is the only client that uses it */
      if ( srch_mux_owns_tx() )
      {
        enc_tx_clk_enable( FALSE );

        /* Turn PA_ON low
        */
        ENC_PA_OFF();

        enc_tx_enable( FALSE );           /* Mask transmit path */
      }

      /* Miscellaneous functions control:
      **        Tx puncture on (Normal), Turn off access channel
      **        mode. Access channel not being transmitted, Tx data fmt
      **        is offset-binary. No IS95C PIlot channel. No Test IS95C PC bits
      **        generated. PCH transmits all PCGs. No FCH eighth rate gating.
      **        No retransmission. IS95A mode.
      */
      HWIO_OUTM( MOD_MISC_CTL,
        (HWIO_FMSK(MOD_MISC_CTL,PA_CTL) | HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH) | ENC_TX_FMT_M | ENC_IS95C_PILOT_M |
        ENC_PICH_GATING_RATE_M | ENC_FCH_EIGHTH_GATE_M | ENC_RETRANS_M | MSM5100_ENC_IS95A_MODE_M),
        (ENC_TXP_ON_V | ENC_NO_ACCESS_V | ENC_TX_OFF_V |
        ENC_NO_IS95C_PILOT_V | ENC_PICH_GATING_RATE_ALL_V | ENC_FCH_EIGHTH_GATE_DIS_V |
        ENC_RETRANS_DIS_V | MSM5100_ENC_IS95A_MODE_ENA_V) );

      break;

    case ENC_SC_MODE:           /* Sync Channel mode */

      /* Turn ON sync channel mode, and turn OFF access channel mode
      */

      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH), ENC_NO_ACCESS_V );

#ifdef T_IO_CARD
#error code not present
#else
      HWIO_OUTM( MOD_MISC_CTL, MSM5100_ENC_TX_BBA_M, MSM5100_ENC_TX_BBA_DIS_V );
#endif

      break;

    case ENC_PC_MODE:           /* Paging/Access Channel mode */

      /* Only if 1x actually owns the transmitter should it take over the
         modulator. The long term fix is to only do this when entering the
         access state in the MUX code. Right now it is done in 1x search
         during paging as well.
      */
      if ( srch_mux_owns_tx() )
      {
        /* Switch encoder/modulator to 1x mode. */
        MUXMSM_SET_MOD_MODE_1X();
      }

      /* Turn OFF sync channel mode, and turn ON access channel mode
      */

      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH), ENC_ACCESS_V );

#ifdef T_IO_CARD
#error code not present
#else
      HWIO_OUTM( MOD_MISC_CTL, MSM5100_ENC_TX_BBA_M, MSM5100_ENC_TX_BBA_DIS_V );
#endif


      break;

    case ENC_TC_MODE:           /* Traffic Channel mode */

      /* Turn OFF sync channel mode, and turn OFF access channel mode
      */

      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,IS95A_ACCESS_CH), ENC_NO_ACCESS_V );

#ifdef T_IO_CARD
#error code not present
#else
      HWIO_OUTM( MOD_MISC_CTL, MSM5100_ENC_TX_BBA_M, MSM5100_ENC_TX_BBA_DIS_V );
#endif

      break;

    case ENC_SLEEP_MODE:        /* Sleep/Analog mode */

      /* glie: The PA is properly turned on and off by the RF driver when the TX is
         enabled and disabled so this isn't necessary and will cause harm to
         HDR during simultaneous operation.  This comment will be removed when
         this change has been properly investigated and tested. */
      /* ENC_PA_OFF(); */

      /* Drive TX_Puncture so that the PA can be controlled
      ** during Analog call processing when chipx8 is absent.
      ** Chipx8 is needed to change the state of TX_Puncture.
      */
      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,PA_CTL), ENC_TXP_ON_V );

      break;

    default:                    /* Illegal Mode, we're in trouble! */
      ERR_FATAL("Bad ch mode %x", channel_mode, 0, 0);
  }
} /* end of enc_set_mode */


/*===========================================================================

FUNCTION ENC_TX_ENABLE

DESCRIPTION
  Enable/disable the transmit path by masking and unmasking it.
  The TX_PUNCT change takes effect on the next transmit frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_tx_enable
(
  boolean enable_tx     /* Enable/disable (mask/unmask) the transmitter */
)
{
  if ( enc_pa_dead || ! enable_tx )
  {
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,MASK_CTL), MSM5100_ENC_MASK_LO_V ); /* Mask  = OFF   */
    HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,PA_CTL), ENC_TXP_MASK_V );/* TX_PUNCT=MASK */
  }
  else
  {

    /* Drive the PA_ON line to the PA, this enables the PA,
    ** as it is pulled-high external to the ASIC when tri-stated.
    */
#ifndef FEATURE_HWTC /* ruben removed 11/26/01 */
    ENC_PA_ON();
#endif /* FEATURE_HWTC */

    /* Set TX_PUNCT to follow MASK_DATA, and then change MASK_DATA from LOW
    ** to having it follow the DBR output.
    */


#if defined (T_IO_CARD) && !defined (T_MSM7500)
#error code not present
#endif /* T_IO_CARD && !T_MSM7500 */
    HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,PA_CTL), ENC_TXP_MASK_V );/* TX_PUNCT=MASK */
    HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,MASK_CTL), MSM5100_ENC_MASK_V );    /* Mask=DBRout   */
#ifdef T_RUMI
#error code not present
#endif /* T_RUMI */

  }
} /* end of enc_tx_enable */


/*===========================================================================

FUNCTION ENC_KILL_PA

DESCRIPTION
  Kill the PA, a fatal error has occurred.  Turn off the PA and
  keep it off until we reset the system.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets enc_pa_dead to FALSE, which disallows re-enabling the PA.

===========================================================================*/
void enc_kill_pa( void )
{
  /* Turn off the PA (tri-state ASIC pin + external pull-up), will get
  ** turned-on when we re-awaken (enter a mode other than sleep).
  ** Also force mask low to turn off TX_PUNCT.
  */
  ENC_PA_OFF();
  HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,MASK_CTL), MSM5100_ENC_MASK_LO_V ); /* Mask Tx to 0    */

  enc_pa_dead = TRUE;           /* We've killed the PA */

} /* end of enc_kill_pa */


/*===========================================================================

FUNCTION ENC_INT_TIMING_OFFSET_PCG

DESCRIPTION
  This function enables the Encoder Interrupt at the specified PCGs in a 20 ms
  frame.  Input values are 0 to 2^15-1, where each bit corresponds to a PCG
  position in the fundicated frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_int_timing_offset_pcg
(
  word enc_pcg
)
{
  /* Load the interrupt timing offset in PCG.
  */
  HWIO_TX_PCG_INT_ENABLE_OUT(enc_pcg);
}


/*===========================================================================

FUNCTION ENC_INT_TIMING_OFFSET

DESCRIPTION
  Specify the delay after the Transmit Frame Boundary when the Transmit Frame
  Interrupt will activate.  This takes effect immediately.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_int_timing_offset
(
  byte bin_mask,                /* 5ms bins mask */
  byte power_ctl_groups,        /* Offset in power ctl groups */
  byte walsh_symbols            /* Offset in Walsh symbols    */
)
{
  word enc_t_offset = 0;

  /* Check for an illegal/fatal input
  */
  if ( (bin_mask == 0) || (bin_mask > 15) || (power_ctl_groups > 3) ||
       (walsh_symbols > 5)
     )
  {
    ERR_FATAL("Bad timing offset %x/%x/%x",bin_mask,power_ctl_groups,
              walsh_symbols);
  }

  /* Rel C DV hardware allows encoder interrupt to happen every PCG in a 20ms
  ** frame.  Concept of 5ms bin is replaced by specifying the PCG, ranging
  ** in value from 0 to 15.
  */

  if (bin_mask & ENC_INT_BIN_0)
  {
    enc_t_offset |= 0x1 << power_ctl_groups;
  }
  if (bin_mask & ENC_INT_BIN_1)
  {
    enc_t_offset |= 0x10 << power_ctl_groups;
  }
  if (bin_mask & ENC_INT_BIN_2)
  {
    enc_t_offset |= 0x100 << power_ctl_groups;
  }
  if (bin_mask & ENC_INT_BIN_3)
  {
    enc_t_offset |= 0x1000 << power_ctl_groups;
  }

  /* Perform actual register write in Rev C function enc_int_timing_offset_pcg().
  */
  enc_int_timing_offset_pcg (enc_t_offset);

} /* end of enc_int_timing_offset */


/*===========================================================================

FUNCTION ENC_USER_PN_STATE

DESCRIPTION
  Set the User PN long code state.  The User PN state takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  Loading 0's into the user PN state inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_user_pn_state
(
  qword user_pn_state   /* User PN state to load into User PN Generator */
)
{
  /* Load the 42-bit User PN state, byte 5 has some flags in it, so we
  ** mask the value we stuff & combine it with the previous contents.
  */
  HWIO_OUT( U_PN_STATE_0, QW_BYTE( user_pn_state, 0 ) );
  HWIO_OUT( U_PN_STATE_1, QW_BYTE( user_pn_state, 1 ) );
  HWIO_OUT( U_PN_STATE_2, QW_BYTE( user_pn_state, 2 ) );
  HWIO_OUT( U_PN_STATE_3, QW_BYTE( user_pn_state, 3 ) );
  HWIO_OUT( U_PN_STATE_4, QW_BYTE( user_pn_state, 4 ) );
  HWIO_OUTM( U_PN_STATE_5, ENC_U_PN_M, QW_BYTE( user_pn_state, 5 ) );

  /* Load the PN long code state at the Tx 80ms boundary
  */
  enc_stmr_cmd( ENC_STMR_CMD_LC_STATE_80MS_V );

} /* end of enc_user_pn_state */


/*===========================================================================

FUNCTION ENC_USER_PN_MASK

DESCRIPTION
  Set the User PN long code mask.  The User PN mask takes effect at the next
  20 ms Transmit Frame Boundary.  Loading 0's into the user PN mask inhibits
  spreading from the User PN Generator.  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_user_pn_mask
(
  qword user_pn_mask    /* User PN mask to load into User PN Generator */
)
{
  /* Load the 42-bit User PN state, byte 5 has some reserved bits
  ** in it, so we mask the value we stuff.
  */
  HWIO_OUTI( U_PN_MASK_w, 0, QW_BYTE( user_pn_mask, 0 ) );
  HWIO_OUTI( U_PN_MASK_w, 1, QW_BYTE( user_pn_mask, 1 ) );
  HWIO_OUTI( U_PN_MASK_w, 2, QW_BYTE( user_pn_mask, 2 ) );
  HWIO_OUTI( U_PN_MASK_w, 3, QW_BYTE( user_pn_mask, 3 ) );
  HWIO_OUTI( U_PN_MASK_w, 4, QW_BYTE( user_pn_mask, 4 ) );
  HWIO_OUTI( U_PN_MASK_w, 5, QW_BYTE( user_pn_mask, 5 ) & ENC_U_PN_M );

  /* Load the PN long code mask at the Tx 20ms boundary
  */
  enc_stmr_cmd( ENC_STMR_CMD_LC_MASK_20MS_V );

} /* end of enc_user_pn_mask */


/*===========================================================================

FUNCTION ENC_STMR_CMD

DESCRIPTION
  Load the User PN long code state and long code mask.
  TX_LC_STATE_LD   bit 3:2
     00   No operation
     01   Load on the next Tx 80 ms boundary
  TX_MASK_LD      bit 1:0
     00   No operation
     01   Load immediately
     10   Load on the next Tx 20 ms boundary

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_stmr_cmd
(
  byte stmr_cmd        /* load command */
)
{
   HWIO_OUT( MOD_STMR_CMD, stmr_cmd );
}


/*===========================================================================

FUNCTION ENC_IQ_PN_STATE

DESCRIPTION
  Set the I & Q PN states into the I & Q PN Generator.  Load 0's to inhibit
  spreading from the I & Q PN Generator.  This takes effect at the next
  80 ms Transmit Frame Boundary (which proceeds SYNC80M by the count in the
  Back-to-the-Future counter).  This operation is non-atomic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_iq_pn_state
(
  word i_pn_state,      /* I PN state */
  word q_pn_state       /* Q PN state */
)
{
  HWIO_OUTI( I_PN_STATE_w, 1, B_PTR( i_pn_state )[1] );     /* I-phase PN state */
  HWIO_OUTI( I_PN_STATE_w, 0, B_PTR( i_pn_state )[0] );
  HWIO_OUTI( Q_PN_STATE_w, 1, B_PTR( q_pn_state )[1] );     /* Q-phase PN state */
  HWIO_OUTI( Q_PN_STATE_w, 0, B_PTR( q_pn_state )[0] );

} /* end of enc_iq_pn_state */


/*===========================================================================

FUNCTION ENC_BOZO_MODE

DESCRIPTION
  Set the "bozo mode" bit, so that the User PN long code state is reloaded
  on each 80 ms Transmit Frame boundary.  Useful when debugging time sync
  problems between the Cel and the mobile.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_bozo_mode
(
  boolean enable_bozo_mode      /* Whether to enable/disable "bozo" mode */
)
{
  /* Set the "bozo" mode bit, this is in the same register as byte 5 of the
  ** User PN state, so we mask the value we stuff & combine it with the
  ** previous contents.
  */
  if ( enable_bozo_mode ) {
    HWIO_OUTM( U_PN_STATE_5, ENC_BOZO_M, ENC_BOZO_V );
  } else {
    HWIO_OUTM( U_PN_STATE_5, ENC_BOZO_M, ENC_NO_BOZO_V );
  }
} /* end of enc_bozo_mode */



/*===========================================================================

FUNCTION ENC_RATE_SET

DESCRIPTION
  Set the rate set to use for encoding data on the Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_rate_set
(
  enc_rate_set_type rate_set    /* Rate set to use for encoding data */
)
{
  if ( rate_set != ENC_RATE_9600 && rate_set != ENC_RATE_14400 ) {
     ERR_FATAL( "Bad Rate Set", 0, 0, 0 );
  }
} /* end of enc_rate_set */


/*===========================================================================

FUNCTION ENC_FCH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate. The Tiger2 definitions of (fractional) rates is
  the reverse of Panther's: 00 means 8th rate in Tiger2 whereas it
  meant full rate earlier, and so forth. Other modules use the old
  definitions of rate as defined in enc_rate_type, so we  keep the old one
  and do (3 - rate) in this function.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_fch_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_rate_type rate             /* Rate to encode the data as              */
)
{
  word enc_ctl;                 /* Encoder control value for this packet  */
  word data_size;               /* Data size for this packet              */
  byte crc_len;
  word crc_poly;
  enc_code_rate_type code_rate;

  ASSERT(
           ( enc_current_fch_rc >= ENC_FCH_RC_1 ) &&
           ( enc_current_fch_rc <= ENC_FCH_RC_4 )
        );
  /*lint -e650 Constant '4' out of range for operator '<' */
  ASSERT( rate < ENC_NUM_FRAC_RATES );

  /* Compute the data size in bytes; round up any fractional part
  */
  data_size = (word)(enc_fch_full_rate_size[ENC_FCH_RC_INX(enc_current_fch_rc)]
                    + ((1 << rate) - 1)) >> rate;

  // MSM5100 modulator needs to have 32 bits alignment data input.
  data_size = ((data_size + 3) >> 2) * 4 ;

  if( generate_crc )
  {
     crc_len = (enc_fch_crc_info_tab[ ENC_FCH_RC_INX( enc_current_fch_rc ) ]
                                    [ ENC_FCH_RATE_INX( rate ) ]).length;
     crc_poly = (enc_fch_crc_info_tab[ ENC_FCH_RC_INX( enc_current_fch_rc ) ]
                                     [ ENC_FCH_RATE_INX( rate ) ]).poly;
     HWIO_OUT( MOD_CH2_CRC_POLY, crc_poly );
  }
  else
  {
     crc_len = 0;
  }

  //
  // program puncture patterns (only affects RC 3 & 4)
  //
  HWIO_OUT(
             MOD_CH2_PUNCT_PATN_0,
             enc_fch_punct_0_tab[ENC_FCH_RC_INX(enc_current_fch_rc)]
                                [ENC_FCH_RATE_INX( rate )]
          );
  HWIO_OUT(
             MOD_CH2_PUNCT_PATN_1,
             enc_fch_punct_1_tab[ENC_FCH_RC_INX(enc_current_fch_rc)]
                                [ENC_FCH_RATE_INX( rate )]
          );

  code_rate = enc_code_rate_tab[ ENC_FCH_RC_INX(enc_current_fch_rc) ];

  enc_ctl = (word) (
                      ( (word) ENC_FCH_RATE_INX( rate ) << ENC_FCH_RATE_POS ) |
                      ( ( (word) code_rate ) << ENC_FCH_CODE_RATE_POS ) |
                      ( ( (word) crc_len ) << ENC_FCH_CRCLEN_POS )
                   );

  /* set the rate, rate set, code rate and crc length */
  HWIO_OUTM(
             MOD_CH2_ENC_CTL_0,
             (
                ENC_FCH_CRCLEN_M |
                HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_CODE_RATE) |
                HWIO_FMSK(MOD_CH2_ENC_CTL_0,FCH_ENC_RATE)
             ),
             enc_ctl
           );

  enc_ctl = (word) (
                      ( ( (word) enc_current_fch_rc ) << ENC_FCH_RADIO_CONFIG_POS ) |
                      ( ( (word) ENC_FCH_FRAME_SIZE_V ) << ENC_FCH_FRAME_SIZE_POS )
                   );
  /* set the radio configuration, 20 ms frame size */

  HWIO_OUT(  MOD_CH2_ENC_CTL_1, ( (byte) enc_ctl ) );

  /* move to right place, pcg count = 1, encode time = 00 */
  /* set the pcg count, encode time */
  HWIO_OUT( MOD_CH2_TIMING_CTL,
            (ENC_FCH_ENCODE_TIME_V | ENC_FCH_PCG_COUNT_V)
          );

  /* set the walshcover select */
  HWIO_OUTM( MOD_WCOVER_SEL,
             ENC_WALSHCOVER_SEL_FCH_M,
             ENC_WALSHCOVER_SEL_FCH_V
           );

  if( enc_current_fch_rc > ENC_FCH_RC_2 )
  {
     enc_is95c_enable( TRUE ); // IS-95C
     enc_pch_enable( TRUE );
  }
  else
  {
     /* in the normal DMSS mode, we already program the gains
     in the txc_traffic_isr(), but somehow we still do it again here??
     Maybe for FTM mode. Until we find out the exact reason, then we can remove it here  */
     enc_is95c_enable( FALSE ); // IS-95A
     enc_pch_gain( 0 );
     enc_fch_gain( TXC_IS95A_FCH_GAIN );
     enc_pch_enable( FALSE );
     enc_fch_enable( TRUE );
  }
  enc_write_data(ENC_CHAN_2,data_ptr,data_size,TRUE);

} /* end of enc_fch_data_out */


#if defined(FEATURE_FACTORY_TESTMODE) || defined (FEATURE_HWTC)
/*===========================================================================

FUNCTION ENC_SCH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size                  /* LTU block size with CRC in bits         */
)
{
  word enc_ctl;                 /* Encoder control value for this packet  */
  word data_size = 0;           /* Data size for this packet              */
  byte crc_len;
  word crc_poly;
  enc_rate_set_type rate_set;

#ifdef FEATURE_DS_IS2000_RS2
  ASSERT(
          (enc_current_sch_rc == ENC_SCH_RC_3) ||
          (enc_current_sch_rc == ENC_SCH_RC_4)
        );
#else
  ASSERT( enc_current_sch_rc == ENC_SCH_RC_3 );
#endif /* FEATURE_DS_IS2000_RS2 */

  ASSERT( rate < ENC_NUM_SCH_RATES );

  /* Look up the data size in bytes.
  */
  data_size = enc_sch_frm_size_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate];

#ifdef FEATURE_DS_IS2000_RS2
  if (enc_current_sch_rc == ENC_SCH_RC_3)
  {
    rate_set = ENC_RATE_9600;
  }
  else
  {
    rate_set = ENC_RATE_14400;
  }
#else
  rate_set = ENC_RATE_9600;
#endif /* FEATURE_DS_IS2000_RS2 */

  if( generate_crc )
  {
     crc_len =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).length;
     crc_poly =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).poly;
     HWIO_OUT( MOD_CH3_CRC_POLY, crc_poly );
  }
  else
  {
     crc_len = 0;
  }

  //
  // program puncture pattern
  //
#ifdef FEATURE_DS_IS2000_RS2
  if ( (turbo_code) && (enc_current_sch_rc == ENC_SCH_RC_4) )
  {
     HWIO_OUT(MOD_CH3_PUNCT_PATN_0, ENC_SCH_RC4_TURBO_PUNCT_0_PATT);
     HWIO_OUT(MOD_CH3_PUNCT_PATN_1, ENC_SCH_RC4_TURBO_PUNCT_1_PATT);
  }
  else
#endif /* FEATURE_DS_IS2000_RS2 */
  {
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_0,
              enc_sch_punct_0_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_1,
              enc_sch_punct_1_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
  }
#ifdef FEATURE_32X_SCH_DATA_RATE
  if( rate == ENC_SCH_32X_RATE )
  {
    enc_ctl = (word) (
                       ( (word) rate << ENC_SCH_RATE_POS ) |
                       ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                       ( ( (word) ENC_CODE_1_BY_2_RATE )
                                  << ENC_SCH_CODE_RATE_POS ) |
                       ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                     );
  }
  else
#endif /* FEATURE_32X_SCH_DATA_RATE */
  {
    enc_ctl = (word) (
                       ( (word) rate << ENC_SCH_RATE_POS ) |
                       ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                       ( ( (word) ENC_CODE_1_BY_4_RATE )
                                  << ENC_SCH_CODE_RATE_POS ) |
                       ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                     );
  }

  /* set the rate, rate set, code rate and crc length */
  HWIO_OUTM
  (
    MOD_CH3_ENC_CTL_0,
    (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
    enc_ctl
  );

/*
  data rate 32X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_32X_V ENC_SCH_WALSHCOVER_1_V
  data rate 16X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_16X_V ENC_SCH_WALSHCOVER_1_V
  data rate  8X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_8X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  4X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_4X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  2X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_2X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  1X  ENC_SCH_LOW_REPEAT_2_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/2  ENC_SCH_LOW_REPEAT_4_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/4  ENC_SCH_LOW_REPEAT_8_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/8  ENC_SCH_LOW_REPEAT_16_V  ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
*/

  switch (rate)
  {
        case ENC_SCH_8TH_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_16_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_QTR_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_8_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );

      break;
        case ENC_SCH_HALF_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_4_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_FULL_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_2_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_2X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_2X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_4X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_4X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_8X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_8X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
    /* program the same value to HWIO_ADDR(MOD_CH3_ENC_CTL_1) for rate = 16X or 32X. */
        case ENC_SCH_16X_RATE:
#ifdef FEATURE_32X_SCH_DATA_RATE
        case ENC_SCH_32X_RATE:
#endif /* FEATURE_32X_SCH_DATA_RATE */
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_16X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
  }

  /* set the radio configuration, 20 ms frame size */
  HWIO_OUT( MOD_CH3_ENC_CTL_1, enc_ctl );

  /* move to right place, pcg count = 1, encode time = 00 */
  /* set the pcg count, encode time */
  HWIO_OUT( MOD_CH3_TIMING_CTL,
            (ENC_SCH_ENCODE_TIME_V | ENC_SCH_PCG_COUNT_V)
          );

  /*
  ** Extended supplemental channel assignment message
  ** specifies REV_WALSH_ID for supplemental channel.
  ** If only one reverse supplemental channel is assigned,
  ** the base station should set the default value.
  ** Set the default walshcover select depends on the
  ** interleaver size.
  */
  /*
     Program MOD_WALSHCOVER_SEL:CH3_WALSHCOVER_SEL with 1 when rate
     is equal to 16X or 32X.
  */
  if
  (
    (rate == ENC_SCH_16X_RATE)
#ifdef FEATURE_32X_SCH_DATA_RATE
    || (rate == ENC_SCH_32X_RATE)
#endif /* FEATURE_32X_SCH_DATA_RATE */
  )
  {
     HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_1_V
              );
  }
  else
  {
     HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_2_V
              );
  }

  enc_is95c_enable( TRUE ); // IS-95C
  enc_sch_enable( TRUE ); // IS-95C

  if( turbo_code )
  {
     // enc_turbo_code_enable( TRUE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_TURBO_CODE_V );
  }
  else
  {
     // enc_turbo_code_enable( FALSE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_CONV_CODE_V );
     if( ltu_size > 0 )
     {
        ASSERT( ( ltu_size >= ENC_LTU_MIN ) && ( ltu_size <= ENC_LTU_MAX ) );
        enc_sch_ltu_size( ltu_size );
        // enc_sch_ltu_enable( TRUE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_ENA_V );
     }
     else
     {
        enc_sch_ltu_size( 0 );
        // enc_sch_ltu_enable( FALSE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_DIS_V );
     }
  }

  enc_write_data(ENC_CHAN_3,data_ptr,data_size, FALSE);

} /* end of enc_sch_data_out */

#endif /* FEATURE_FACTORY_TESTMODE || FEATURE_HWTC */

#if defined(FEATURE_IS2000_R_SCH) || defined(FEATURE_HWTC)
/*===========================================================================

FUNCTION ENC_SCH_MUXPDU5_DATA_OUT

DESCRIPTION
  Send data to the encoder for MUXPDU5, since for MUXPDU5, the size of the PDU can be
  odd, so this function can handle byte writing into the encoder RAM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_muxpdu5_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  word total_numberofbyte
)
{
  enc_write_data(ENC_CHAN_3,data_ptr,total_numberofbyte, FALSE);
}
/*===========================================================================

FUNCTION ENC_SCH_DATA_OUT_CFG

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_sch_data_out_cfg
(
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  boolean generate_crc           /* Have the Encoder generate a CRC, or not */
)
{
  word enc_ctl;                 /* Encoder control value for this packet  */
  byte crc_len;
  word crc_poly;
  enc_rate_set_type rate_set;

#ifdef FEATURE_DS_IS2000_RS2
  ASSERT(
          (enc_current_sch_rc == ENC_SCH_RC_3) ||
          (enc_current_sch_rc == ENC_SCH_RC_4)
        );
#else
  ASSERT( enc_current_sch_rc == ENC_SCH_RC_3 );
#endif /* FEATURE_DS_IS2000_RS2 */

  ASSERT( rate < ENC_NUM_SCH_RATES);

#ifdef FEATURE_DS_IS2000_RS2
  if (enc_current_sch_rc == ENC_SCH_RC_3)
  {
    rate_set = ENC_RATE_9600;
  }
  else
  {
    rate_set = ENC_RATE_14400;
  }
#else
  rate_set = ENC_RATE_9600;
#endif /* FEATURE_DS_IS2000_RS2 */

  if( generate_crc )
  {
     crc_len =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).length;
     crc_poly =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).poly;
     HWIO_OUT( MOD_CH3_CRC_POLY, crc_poly );
  }
  else
  {
     crc_len = 0;
  }

  //
  // program puncture pattern
  //
#ifdef FEATURE_DS_IS2000_RS2
  if ( (turbo_code) && (enc_current_sch_rc == ENC_SCH_RC_4) )
  {
     HWIO_OUT(MOD_CH3_PUNCT_PATN_0, ENC_SCH_RC4_TURBO_PUNCT_0_PATT);
     HWIO_OUT(MOD_CH3_PUNCT_PATN_1, ENC_SCH_RC4_TURBO_PUNCT_1_PATT);
  }
  else
#endif /* FEATURE_DS_IS2000_RS2 */
  {
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_0,
              enc_sch_punct_0_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_1,
              enc_sch_punct_1_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
  }

#ifdef FEATURE_32X_SCH_DATA_RATE
  if( rate == ENC_SCH_32X_RATE )
  {
    enc_ctl = (word) (
                       ( (word) rate << ENC_SCH_RATE_POS ) |
                       ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                       ( ( (word) ENC_CODE_1_BY_2_RATE )
                                  << ENC_SCH_CODE_RATE_POS ) |
                       ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                     );
  }
  else
#endif /* FEATURE_32X_SCH_DATA_RATE */
  {
        enc_ctl = (word) (
                                                ( (word) rate << ENC_SCH_RATE_POS ) |
                                                ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                                                ( ( (word) ENC_CODE_1_BY_4_RATE )
                                                                   << ENC_SCH_CODE_RATE_POS ) |
                                                ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                                         );
  }

  /* set the rate, rate set, code rate and crc length */
  HWIO_OUTM
  (
    MOD_CH3_ENC_CTL_0,
    (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
    enc_ctl
  );

/*
  data rate 32X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_32X_V ENC_SCH_WALSHCOVER_1_V
  data rate 16X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_16X_V ENC_SCH_WALSHCOVER_1_V
  data rate  8X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_8X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  4X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_4X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  2X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_2X_V  ENC_SCH_WALSHCOVER_2_V
  data rate  1X  ENC_SCH_LOW_REPEAT_2_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/2  ENC_SCH_LOW_REPEAT_4_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/4  ENC_SCH_LOW_REPEAT_8_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  data rate 1/8  ENC_SCH_LOW_REPEAT_16_V  ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
*/

  switch (rate)
  {
        case ENC_SCH_8TH_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_16_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_QTR_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_8_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );

      break;
        case ENC_SCH_HALF_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_4_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_FULL_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_2_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_2X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_2X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_4X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_4X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
        case ENC_SCH_8X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_8X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
    /* program the same value to HWIO_ADDR(MOD_CH3_ENC_CTL_1) for rate = 16X or 32X. */
        case ENC_SCH_16X_RATE:
#ifdef FEATURE_32X_SCH_DATA_RATE
        case ENC_SCH_32X_RATE:
#endif /* FEATURE_32X_SCH_DATA_RATE */
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_16X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
  }

  /* set the radio configuration, 20 ms frame size */
  HWIO_OUT( MOD_CH3_ENC_CTL_1, enc_ctl );

  /* set the pcg count, encode time */
  HWIO_OUT( MOD_CH3_TIMING_CTL,
            (ENC_SCH_ENCODE_TIME_V | ENC_SCH_PCG_COUNT_V)
          );

  /*
  ** Extended supplemental channel assignment message
  ** specifies REV_WALSH_ID for supplemental channel.
  ** If only one reverse supplemental channel is assigned,
  ** the base station should set the default value.
  ** Set the default walshcover select depends on the
  ** interleaver size.
  */
  /*
     Program MOD_WALSHCOVER_SEL:CH3_WALSHCOVER_SEL with 1 when rate
     is equal to 16X or 32X.
  */
  if
  (
    (rate == ENC_SCH_16X_RATE)
#ifdef FEATURE_32X_SCH_DATA_RATE
    || (rate == ENC_SCH_32X_RATE)
#endif /* FEATURE_32X_SCH_DATA_RATE */
  )
  {
         HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_1_V
              );
  }
  else
  {
     HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_2_V
              );
  }

  enc_is95c_enable( TRUE ); // IS-95C
  enc_sch_enable( TRUE ); // IS-95C

  if( turbo_code )
  {
     // enc_turbo_code_enable( TRUE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_TURBO_CODE_V );
  }
  else
  {
     // enc_turbo_code_enable( FALSE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_CONV_CODE_V );
     if( ltu_size > 0 )
     {
        ASSERT( ( ltu_size >= ENC_LTU_MIN ) && ( ltu_size <= ENC_LTU_MAX ) );
        enc_sch_ltu_size( ltu_size );
        // enc_sch_ltu_enable( TRUE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_ENA_V );
     }
     else
     {
        enc_sch_ltu_size( 0 );
        // enc_sch_ltu_enable( FALSE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_DIS_V );
     }
  }

} /* end of enc_sch_data_out_cfg() */
#endif /* FEATURE_IS2000_R_SCH || FEATURE_HWTC */


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION ENC_REACH_DATA_OUT

DESCRIPTION
  Send data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_reach_data_out
(
  byte *data_ptr,                /* Buffer to send to Encoder               */
  boolean generate_crc,          /* Have the Encoder generate a CRC, or not */
  enc_sch_rate_type rate,        /* Rate to encode the data as              */
  boolean turbo_code,            /* TRUE= Turbo, FALSE=Convolutional        */
  word ltu_size,                 /* LTU block size with CRC in bits         */
  word frame_size                /* Frame size                              */
)
{
  word enc_ctl;                 /* Encoder control value for this packet  */
  word data_size = 0;           /* Data size for this packet              */
  byte crc_len;
  word crc_poly;
  enc_rate_set_type rate_set;


#ifdef FEATURE_RC4_SCH
  ASSERT(
          (enc_current_sch_rc == ENC_SCH_RC_3) ||
          (enc_current_sch_rc == ENC_SCH_RC_4)
        );
#else
  ASSERT( enc_current_sch_rc == ENC_SCH_RC_3 );
#endif /* FEATURE_RC4_SCH */

  ASSERT( rate < ENC_NUM_SCH_RATES );

  /* Look up the data size in bytes.
  */
  data_size = enc_sch_frm_size_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate];

#ifdef FEATURE_RC4_SCH
  if (enc_current_sch_rc == ENC_SCH_RC_3)
  {
    rate_set = ENC_RATE_9600;
  }
  else
  {
    rate_set = ENC_RATE_14400;
  }
#else
  rate_set = ENC_RATE_9600;
#endif /* FEATURE_RC4_SCH */

  if( generate_crc )
  {
     crc_len =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).length;
     crc_poly =
       (enc_sch_crc_info_tab [ENC_SCH_RC_INX(enc_current_sch_rc)][rate]).poly;
     HWIO_OUT( MOD_CH3_CRC_POLY, crc_poly );
  }
  else
  {
     crc_len = 0;
  }

  //
  // program puncture pattern
  //
#ifdef FEATURE_RC4_SCH
  if ( (turbo_code) && (enc_current_sch_rc == ENC_SCH_RC_4) )
  {
     HWIO_OUT(MOD_CH3_PUNCT_PATN_0, ENC_SCH_RC4_TURBO_PUNCT_0_PATT);
     HWIO_OUT(MOD_CH3_PUNCT_PATN_1, ENC_SCH_RC4_TURBO_PUNCT_1_PATT);
  }
  else
#endif /* FEATURE_RC4_SCH */
  {
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_0,
              enc_sch_punct_0_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
     HWIO_OUT(
              MOD_CH3_PUNCT_PATN_1,
              enc_sch_punct_1_tab[ENC_SCH_RC_INX(enc_current_sch_rc)][rate]
             );
  }

#ifdef FEATURE_32X_SCH_DATA_RATE
  if( rate == ENC_SCH_32X_RATE )
  {
    enc_ctl = (word) (
                       ( (word) rate << ENC_SCH_RATE_POS ) |
                       ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                       ( ( (word) ENC_CODE_1_BY_2_RATE )
                                  << ENC_SCH_CODE_RATE_POS ) |
                       ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                     );
  }
  else
#endif /* FEATURE_32X_SCH_DATA_RATE */
  {
    enc_ctl = (word) (
                       ( (word) rate << ENC_SCH_RATE_POS ) |
                       ( (word) rate_set << ENC_SCH_RATE_SET_POS ) |
                       ( ( (word) ENC_CODE_1_BY_4_RATE )
                                  << ENC_SCH_CODE_RATE_POS ) |
                       ( ( (word) crc_len ) << ENC_SCH_CRCLEN_POS )
                     );
  }
  /* set the rate, rate set, code rate and crc length */
  // enc_reach_on
  if (frame_size == ENC_SCH_FRAME_SIZE_20_V)
  {

    // 20 ms, 9600 bps, 19200 bps, 38400 bps data rate
    HWIO_OUTM
    (
      MOD_CH3_ENC_CTL_0,
      (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
      enc_ctl
    );

  }
  else if (frame_size == ENC_SCH_FRAME_SIZE_10_V)
  {

    if (rate == ENC_SCH_FULL_RATE)
    {

      // 10 ms, 19200 bps data rate, 12 bits CRC, ENC_SCH_FULL_RATE
      HWIO_OUTM
      (
        MOD_CH3_ENC_CTL_0,
        (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
        0x0C89
      );
    }
    else if (rate == ENC_SCH_2X_RATE)
    {
      /* 10 ms, 38400 bps data rate, 16 bits CRC, ENC_SCH_2X_RATE */
      HWIO_OUTM
      (
        MOD_CH3_ENC_CTL_0,
        (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
        0x108B
      );
    } /* end if (rate == ENC_SCH_FULL_RATE) */
  }
  else if (frame_size == ENC_SCH_FRAME_SIZE_5_V)
  {

    // 5 ms, 38400 bps data rate, 12 bits CRC, ENC_SCH_FULL_RATE
    HWIO_OUTM
    (
      MOD_CH3_ENC_CTL_0,
      (ENC_SCH_CRCLEN_M|ENC_SCH_CODE_RATE_M|ENC_SCH_RATE_SET_M|ENC_SCH_DATA_RATE_M),
      0x0C8B
    );

  } /* end if (frame_size == ENC_SCH_FRAME_SIZE_20_V) */

  /*
    data rate 32X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_32X_V ENC_SCH_WALSHCOVER_1_V
    data rate 16X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_16X_V ENC_SCH_WALSHCOVER_1_V
    data rate  8X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_8X_V  ENC_SCH_WALSHCOVER_2_V
    data rate  4X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_4X_V  ENC_SCH_WALSHCOVER_2_V
    data rate  2X  ENC_SCH_LOW_REPEAT_1_V   ENC_SCH_INTLV_SIZE_2X_V  ENC_SCH_WALSHCOVER_2_V
    data rate  1X  ENC_SCH_LOW_REPEAT_2_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
    data rate 1/2  ENC_SCH_LOW_REPEAT_4_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
    data rate 1/4  ENC_SCH_LOW_REPEAT_8_V   ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
    data rate 1/8  ENC_SCH_LOW_REPEAT_16_V  ENC_SCH_INTLV_SIZE_1X_V  ENC_SCH_WALSHCOVER_2_V
  */

  switch (rate)
  {
    case ENC_SCH_8TH_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_16_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
    case ENC_SCH_QTR_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_8_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );

      break;
    case ENC_SCH_HALF_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_1X_V | ENC_SCH_LOW_REPEAT_4_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
    case ENC_SCH_FULL_RATE:
      //if( enc_reach_on )
      {
        if (frame_size == ENC_SCH_FRAME_SIZE_20_V)
        {

          // 20ms, Data rate = 9600 bps use ENC_SCH_FULL_RATE information and crc
          //       txcaccess.c sets data rate ENC_SCH_FULL_RATE
          //
          enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_20_V |
                             ENC_SCH_INTLV_SIZE_4X_V | ENC_SCH_LOW_REPEAT_4_V |
                             ENC_SCH_MULTIFRAME_1_V
                           );
        }
        else if (frame_size == ENC_SCH_FRAME_SIZE_10_V)
        {

        // 10ms, Data rate = 19200 bps uses ENC_SCH_FULL_RATE information bits and crc
        //       txcaccess.c sets data raet ENC_SCH_FULL_RATE
        // 10ms frame size, low repeat = 2, interleave size = 1536
        // 9600 bps data rate
        enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_10_V |
                           ENC_SCH_INTLV_SIZE_1536_V | ENC_SCH_LOW_REPEAT_2_V |
                           ENC_SCH_MULTIFRAME_1_V
                         );

        }
        else if (frame_size == ENC_SCH_FRAME_SIZE_5_V)
        {

        // 5ms, Data rate = 38400 bps uses ENC_SCH_FULL_RATE information bits and crc
        //      txcaccess.c sets data rate ENC_SCH_FULL_RATE
        // 5ms frame size, low repeat = 1, interleave size = 768
        // 9600 bps data rate
        enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_5_V |
                           ENC_SCH_INTLV_SIZE_768_V | ENC_SCH_LOW_REPEAT_1_V |
                           ENC_SCH_MULTIFRAME_1_V
                         );

        } /* end if (frame_size == ENC_SCH_FRAME_SIZE_20_V) */
      }
      break;
    case ENC_SCH_2X_RATE:
      //if( enc_reach_on)
      {
        if (frame_size == ENC_SCH_FRAME_SIZE_20_V)
        {
          // 20ms, Data rate = 19200 bps use ENC_SCH_2X_RATE information and crc
          //       txcaccess.c sets data rate ENC_SCH_2X_RATE
          //
          enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_20_V |
                             ENC_SCH_INTLV_SIZE_4X_V | ENC_SCH_LOW_REPEAT_2_V |
                             ENC_SCH_MULTIFRAME_1_V
                           );
        }
        else if (frame_size == ENC_SCH_FRAME_SIZE_10_V)
        {
          // 10ms, Data rate = 38400 bps uses ENC_SCH_2X_RATE information bits and crc
          //       txcaccess.c sets data raet ENC_SCH_2X_RATE
          // 10ms frame size, low repeat = 1, interleave size = 1536
          // 19200 bps data rate
          enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_10_V |
                             ENC_SCH_INTLV_SIZE_1536_V | ENC_SCH_LOW_REPEAT_1_V |
                             ENC_SCH_MULTIFRAME_1_V
                           );
        } /* end if (frame_size == ENC_SCH_FRAME_SIZE_20_V) */
      }
      break;
    case ENC_SCH_4X_RATE:
      //if( enc_reach_on)
      {
        // 20ms, Data rate = 38400 bps use ENC_SCH_4X_RATE information and crc
        //       txcaccess.c sets data rate ENC_SCH_4X_RATE
        //
        enc_ctl = (word) ( ENC_SCH_SELECT_EACH_V | ENC_SCH_FRAME_SIZE_20_V |
                           ENC_SCH_INTLV_SIZE_4X_V | ENC_SCH_LOW_REPEAT_1_V |
                           ENC_SCH_MULTIFRAME_1_V
                         );
      }
      break;
    case ENC_SCH_8X_RATE:
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_8X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;

    /* program the same value to HWIO_ADDR(MOD_CH3_ENC_CTL_1) for rate = 16X or 32X. */
    case ENC_SCH_16X_RATE:
#ifdef FEATURE_32X_SCH_DATA_RATE
    case ENC_SCH_32X_RATE:
#endif /* FEATURE_32X_SCH_DATA_RATE */
      enc_ctl = (word) ( ENC_SCH_SELECT_SCH_V | ENC_SCH_FRAME_SIZE_20_V |
                         ENC_SCH_INTLV_SIZE_16X_V | ENC_SCH_LOW_REPEAT_1_V |
                         ENC_SCH_MULTIFRAME_1_V
                       );
      break;
  }

  /* set the radio configuration, 20 ms frame size */
  HWIO_OUT( MOD_CH3_ENC_CTL_1, enc_ctl );

  /* set the pcg count, encode time */
  HWIO_OUT( MOD_CH3_TIMING_CTL,
            (ENC_SCH_ENCODE_TIME_V | ENC_SCH_PCG_COUNT_V)
          );

  /*
  ** Extended supplemental channel assignment message
  ** specifies REV_WALSH_ID for supplemental channel.
  ** If only one reverse supplemental channel is assigned,
  ** the base station should set the default value.
  ** Set the default walshcover select depends on the
  ** interleaver size.
  */
  /*
     Program MOD_WALSHCOVER_SEL:CH3_WALSHCOVER_SEL with 1 when rate
     is equal to 16X or 32X.
  */
  if
  (
    (rate == ENC_SCH_16X_RATE)
#ifdef FEATURE_32X_SCH_DATA_RATE
    || (rate == ENC_SCH_32X_RATE)
#endif /* FEATURE_32X_SCH_DATA_RATE */
  )
  {
     HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_1_V
              );
  }
  else
  {
     HWIO_OUTM( MOD_WCOVER_SEL,
                ENC_WALSHCOVER_SEL_SCH_M,
                ENC_WALSHCOVER_SEL_SCH_2_V
              );
  }

  enc_is95c_enable( TRUE ); // IS-95C
  enc_sch_enable( TRUE ); // IS-95C

  if( turbo_code )
  {
     // enc_turbo_code_enable( TRUE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_TURBO_CODE_V );
  }
  else
  {
     // enc_turbo_code_enable( FALSE );
     HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_TURBO_CODE_M, ENC_CONV_CODE_V );
     if( ltu_size > 0 )
     {
        ASSERT( ( ltu_size >= ENC_LTU_MIN ) && ( ltu_size <= ENC_LTU_MAX ) );
        enc_sch_ltu_size( ltu_size );
        // enc_sch_ltu_enable( TRUE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_ENA_V );
     }
     else
     {
        enc_sch_ltu_size( 0 );
        // enc_sch_ltu_enable( FALSE );
        HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_LTU_ENA_M, ENC_LTU_DIS_V );
     }
  }

  enc_write_data(ENC_CHAN_3,data_ptr,data_size,TRUE);

} /* end of enc_reach_data_out */
#endif


/*===========================================================================

FUNCTION ENC_STATUS

DESCRIPTION
  Read the status of the encoder circuit.

DEPENDENCIES
  None

RETURN VALUE
  Circuit status which can be masked with enc_status_mask_type.

SIDE EFFECTS
  Reading status latches the status in the encoder, clearing error status.

===========================================================================*/
word enc_status( void )
{
//      return( (word) ENC_INH( ENC_STATUS ) );        /* return the raw status byte */
  /* The following line is hand ported to HWIO as in_word on a BYTE register
    * due to the fact that the LEO CAF disagrees with the LEO SW I/F Manual
    * and there really are bits above bit 7 in MOD_STATUS without which DMSS
    * can not successfully make a loopback call
   */
  return( (word) in_word( HWIO_ADDR(MOD_STATUS) ) );        /* return the raw status byte */

} /* end of enc_status */

/*===========================================================================

FUNCTION ENC_TX_CLK_ENABLE

DESCRIPTION
  Disable the Reverse Link transmit path clocks to save power while not using
  the Reverse Link (e.g. listening to the Paging Channel, but not transmitting
  on the Access Channel).  The Reverse Link state is undefined when transmit
  clocks are re-enabled, the transmit path should be re-initialized into Paging
  or Traffic channel mode.

DEPENDENCIES
  The PA should be off to avoid errant transmissions when the clocks are
  turned on.

RETURN VALUE
  None

SIDE EFFECTS
  Disables/Enables the transmit half of the circuit, putting it into sleep mode.

===========================================================================*/
void enc_tx_clk_enable
(
  boolean enable_tx_clks        /* Enable/disable the CHIPX8 clock on Tx path */
)
{
  word isave;                   /* PSW parameter for INTLOCK/FREE_SAV    */

  /* Set the bit flag to indicate whether or not to disable the
  ** Reverse Channel transmit path is awake/enabled or asleep/disabled.
  */
  INTLOCK_SAV( isave );         /* Disable interrupts and save PSW */
  if ( enable_tx_clks )         /* Transmit clocks "disable off" == Enable */
  {
    /* Enable TX clocks and reset TXDAC_SLEEP bit.
    */

#ifndef FEATURE_SRCH_CLKREGIM_RM
#ifndef T_IO_CARD
    HWIO_OUTM( MOD_CLK_CTL, ENC_TX_CLKS_DIS_M, ENC_TX_CLKS_ENA_V );
#endif
#endif /* !FEATURE_SRCH_CLKREGIM_RM */

    /* Reset TDAC_SLEEP bit, do not move this code to enc_tx_enable.  R-EACH
    ** frame is 5ms so ENC must be ready much sooner than R-ACH.
    */
#if defined T_MSM7500B || defined(T_MSM6800B)
    HWIO_OUTM ( MODEM_TX_DAC_SLEEP, HWIO_FMSK( MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP),
                ( 0 << HWIO_SHFT (MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP)) );
#else
    HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,TX_DAC_SLEEP),
                ( 0 << HWIO_SHFT (MODEM_MISC_CFG0,TX_DAC_SLEEP)) );
#endif /* T_MSM7500B */
  }
  else                          /* Transmit clocks "disable on" == Disable */
  {
    /* Disable TX clocks and set TXDAC_SLEEP bit.
    */
#if defined T_MSM7500B || defined(T_MSM6800B)
    HWIO_OUTM ( MODEM_TX_DAC_SLEEP, HWIO_FMSK( MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP),
              ( 1 << HWIO_SHFT (MODEM_TX_DAC_SLEEP,TX_DAC_SLEEP)) );
#else
    HWIO_OUTM ( MODEM_MISC_CFG0, HWIO_FMSK( MODEM_MISC_CFG0,TX_DAC_SLEEP),
              ( 1 << HWIO_SHFT (MODEM_MISC_CFG0,TX_DAC_SLEEP)) );
#endif /* T_MSM7500B */

#ifndef FEATURE_SRCH_CLKREGIM_RM
#ifndef T_IO_CARD
    HWIO_OUTM( MOD_CLK_CTL, ENC_TX_CLKS_DIS_M, ENC_TX_CLKS_DIS_V );
#endif
#endif /* !FEATURE_SRCH_CLKREGIM_RM */
  }
  INTFREE_SAV( isave );         /* Restore interrupts (PSW) */

} /* end of enc_tx_clk_enable */


/*===========================================================================

FUNCTION ENC_TX_IF_LOOPBACK_INIT

DESCRIPTION
  Initialize the circuit to IF loopback mode.  Once this is done, packets
  can be transmitted by calling enc_tx_if_loopback().  The packets will
  loop-back from TX to RX and be received.  To go back to normal call modes,
  call enc_set_mode().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Puts the circuit in IF loopback

===========================================================================*/
void enc_tx_if_loopback_init( void )
{
  qword zero;

  /* Do IF Loopback.  Switch to access channel mode, long code
  ** off, short code on, no CRC.
  */
  enc_set_mode( ENC_PC_MODE );          /* Access channel mode */

  qw_set( zero, 0L, 0L );               /* Make a 0 quad word             */
  enc_user_pn_state( zero );            /* Zero PN long code state & mask */
  enc_user_pn_mask( zero );

  /* Init I & Q PN state
  */
  enc_iq_pn_state( ENC_PN_INIT_STATE, ENC_PN_INIT_STATE );

  enc_tx_enable( TRUE );                /* Enable the transmitter       */

} /* end of enc_tx_if_loopback_init */


/*===========================================================================

FUNCTION ENC_ACP_PA_CTL

DESCRIPTION
  Turn on/off PA_ON and forces TX_PUNCT high and low, this effectively
  controls the PA for Analog Call Processing.

  Note: this does not override enc_kill_pa().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets enc_pa_dead to FALSE, which disallows re-enabling the PA.
  TX_PUNCT is only manipulated in the Mobile, as chipx8 is required
  for changing TX_PUNCT, and it is not present during 'FM mode' on
  the Portable.  It would do no harm to "change it anyway", but if
  for some reason chipx8 was available in 'FM mode' it could render
  the PA as OFF until chipx8 was available again.

===========================================================================*/
void enc_acp_pa_ctl
(
  boolean enable
    /* Enable/Set (disable/clear) the PA_ON and TX_PUNCT outputs */
)
{
  /* If the PA has been killed, or is to be disabled, turn it off
  ** Else turn it on.
  */
  if ( enc_pa_dead || ! enable )
  {
    /* Turn off the PA sync it to 0.
    */
    ENC_PA_OFF();

    /* If this is done while chipx8 is on, it will be on later when we want
    ** to turn on the PA for ACP, without chipx8.
    */
    HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,PA_CTL), ENC_TXP_ON_V );  /* TX_PUNCT=ON */
  }
  else
  {
    /* Turn on the PA (tri-state ASIC pin + external pull-up),
    */
    ENC_PA_ON();
  }
} /* end of enc_acp_pa_ctl */


/*===========================================================================

FUNCTION ENC_INIT_PA

DESCRIPTION
  Initialize the PA to off, do this appropriate to which type of PA control
  hardware is available.  With the PCN-applique, PA_ON is never to be turned
  on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_ENC_BTF_IN_NV
void enc_init_pa
(
  boolean pcn,
    /* True if the PCN applique is attached, False if not */
  uint32 delay
    /* BTF delay value in chipx8 */
)
{
  /* Turn off PA_ON sync it to 0.
  */
  ENC_PA_OFF();

  enc_have_pcn = pcn; /*lint -esym(551,enc_have_pcn) */ /* not accessed yet */

  /* Set the PA_WARMUP value to the delay for PCN
  ** Set the BTF value to the delay for PCN
  */
  if ( enc_have_pcn )
  {
#ifndef FEATURE_SUBPCG_PA_WARMUP_DELAY
     HWIO_OUT( PA_WARMUP, ENC_PCN_PA_WARMUP_V );
  #ifdef T_MSM6050
     HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
  #endif

#else
  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( PA_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
     HWIO_OUT( PA_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */

  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( TX_WARMUP, rfm_get_tx_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
     HWIO_OUT( TX_WARMUP, rf_get_tx_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */
#endif
  }
  else
  {
#ifndef FEATURE_SUBPCG_PA_WARMUP_DELAY
     HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
     HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );

#else
  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( PA_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
     HWIO_OUT( PA_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */

  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( TX_WARMUP, rfm_get_tx_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
     HWIO_OUT( TX_WARMUP, rf_get_tx_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */
#endif
  }

  #if defined T_RUMI
  HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
  HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
  #endif /* T_RUMI */


  enc_set_btf( delay );
} /* end of enc_init_pa */
#else /* else of FEATURE_ENC_BTF_IN_NV */
void enc_init_pa
(
  boolean pcn
    /* True if the PCN applique is attached, False if not */
)
{
  /* Turn off PA_ON sync it to 0.
  */
  ENC_PA_OFF();

  enc_have_pcn = pcn; /*lint -esym(551,enc_have_pcn) */ /* not accessed yet */

  /* Set the PA_WARMUP value to the delay for PCN
  ** Set the BTF value to the delay for PCN
  */
  if ( enc_have_pcn )
  {
#ifndef FEATURE_SUBPCG_PA_WARMUP_DELAY
    HWIO_OUT( PA_WARMUP, ENC_PCN_PA_WARMUP_V );
  #ifdef T_MSM6050
     HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
  #endif

#else
  #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( PA_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
  #else
    HWIO_OUT( PA_WARMUP, rf_get_pa_warmup());
  #endif /* FEATURE_MULTI_RX_CHAIN */

 #ifdef T_MSM6050
   #ifdef FEATURE_MULTI_RX_CHAIN
    HWIO_OUT( TX_WARMUP, rfm_get_pa_warmup( RFCOM_TRANSCEIVER_0 ));
   #else
    HWIO_OUT( TX_WARMUP, rf_get_pa_warmup());
   #endif /* FEATURE_MULTI_RX_CHAIN */
  #endif  /* T_MSM6050 */
#endif

  #if defined T_RUMI
  HWIO_OUT( PA_WARMUP, ENC_PA_WARMUP_V );
  HWIO_OUT( TX_WARMUP, ENC_TX_WARMUP_V );
  #endif /* T_RUMI */


    enc_set_btf( ENC_INIT_BTF_PCS );
  }
} /* end of enc_init_pa */
#endif /* FEATURE_ENC_BTF_IN_NV */

/*===========================================================================

FUNCTION ENC_PN_RAND_DELAY

DESCRIPTION
  Delay the transmit time by a specified number of chips.  This value is
  loaded internally by the Interleaver at the next SYNC80M, and takes effect
  at the successive internal 80 millisecond boundary (the BTF roll-over).

  For CDMA transmit PN randomization, call this routine with the number of
  chips of delay desired.  Call it again with 0 to return to non-delayed
  transmit frame timing.

DEPENDENCIES
  None

RETURN VALUE
  Number of chips from the previous call to enc_pn_rand_delay.

SIDE EFFECTS
  None

===========================================================================*/
word enc_pn_rand_delay
(
  word chips        /* Chip delay for transmit frame timing */
)
{
  dword new_btf;
  dword chipsx8_delay;
  word old_delay;

  /* BTF is advancement of transmit frame timing in units of chipx8.
  ** We delay by decreasing the advancement.
  */

  /* Convert the chip delay to a chipx8 delay.
  */
  chipsx8_delay = chips * 8L;

  /* Check to see if the delay exceeds the BTF advance.
  ** If it does, log an error and set the delay to the
  ** maximum that we can manage.
  */
  if ( chipsx8_delay > enc_btf_chipx8 ) {
    ERR("Too much delay %d", chips, 0, 0);

    chipsx8_delay = enc_btf_chipx8;     /* Max delay = Max advance */
  }

  /* Calculate the temporary BTF value based on the
  ** We delay by decreasing the advancement.
  */
  new_btf = enc_btf_chipx8 - chipsx8_delay;

  enc_wrt_btf_reg( new_btf );           /* Set the BTF register   */

  /* Get old delay for the return value, and save the new delay.
  */
  old_delay = enc_pn_rand_delay_chips;
  enc_pn_rand_delay_chips = chips;

  return( old_delay );

} /* end of enc_pn_rand_delay */


/*===========================================================================

FUNCTION ENC_WRT_BTF_REG

DESCRIPTION
  This function adjusts a BTF value before writing it into the register.  A
  modification to the value is required because there is a difference in the
  Tx delay path due to the MSM23 hardware.

  There is also a modulo-8 problem in the MSM23 hardware.  If the intended
  value's three least significant bits are zeros, the MSM23 will shift the
  desired value by 8.  Therefore, the value must be adjusted before it is
  loaded into the register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The adjustment is currently implemented with the assumption that MSM
  2.3 rev A3 and above will have the modulo-8 problem.  If it gets fixed
  in the MSM, then the function must be changed to avoid unnecessary
  compensation.

===========================================================================*/
void enc_wrt_btf_reg
(
  dword  btf_value
)
{

#ifndef FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP
  word  tx_sync_st;
  word  sysfr_state;
#endif /* !FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP */
  dword modifier;

#ifdef T_IO_CARD
#error code not present
#endif

#ifdef T_QUASAR_BB
#error code not present
#endif

  /* Write values to the registers  */
  #ifdef FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP
  /* Add the support to compensate the BTF value with frame offset */
  modifier = (dword)(12288 * enc_fwd_frame_off) + (dword)btf_value + 2L;
  #else
  /*
  ** MODIFIER = 12288 x SYSFR_STATE + TX_SYNC_ST + 2
  */
  tx_sync_st = ((word *)(void *)&(btf_value))[0];

  sysfr_state = ((word *)(void *)&(btf_value))[1];

  modifier = (dword)(12288 * sysfr_state) + (dword)tx_sync_st + 2L;

  #endif /* FEATURE_MSM6500_BTF_FRAME_OFFSET_COMP */

  #ifdef FEATURE_SRCH_SUPPORTS_QLIC
  /* There is an RTF_ADJUST register that needs to be set if QLIC is enabled.
  ** If QLIC is not enabled, then the register will be set to 0 by default.
  ** This allows for the modifier to be adjusted by the combiner deskew depth
  ** in case QLIC is supported in the MDSP.
  */
  modifier += GET_RTF_ADJUST() * 64 * 8;
  #endif /* FEATURE_SRCH_SUPPORTS_QLIC */

  HWIO_OUT( MOD_STMR_MODIFIER_0, ((word *)(void *)&(modifier))[0]);

  HWIO_OUT( MOD_STMR_MODIFIER_1, ((word *)(void *)&(modifier))[1]);
  MSG_MED ("btf=%lx, modifier=%lx", btf_value, modifier, 0);
} /* end of enc_wrt_btf_reg */


/*===========================================================================

FUNCTION ENC_SCH_LTU_SIZE

DESCRIPTION
   Set supplemental channel LTU size.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_sch_ltu_size
(
   word sch_ltu_size        /* 32 <= ltu_size < 512 bits */
)
{
   HWIO_OUT( MOD_SCH_LTU_CTL, sch_ltu_size );
}


/*===========================================================================

FUNCTION ENC_PCH_GAIN

DESCRIPTION
  Set reverse Pilot Channel gain

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_pch_gain
(
   word pch_gain        /* 8:0 of MOD_PCH_GAIN register is rev. pilot ch gain */
)
{
   muxmdsp_wr_mod_pch_gain ( pch_gain );
}


/*===========================================================================

FUNCTION ENC_SCH_GAIN

DESCRIPTION
  Set reverse Supplemental Channel gain

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_sch_gain
(
   word sch_gain        /* 8:0 of MOD_SCH_GAIN register is rev. sch gain */
)
{
   muxmdsp_wr_mod_sch_gain ( sch_gain );
}


/*===========================================================================

FUNCTION ENC_FCH_GAIN

DESCRIPTION
  Set reverse Fundamental Channel gain

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_fch_gain
(
   word fch_gain        /* 8:0 of MOD_FCH_GAIN register is rev. fch gain */
)
{
   muxmdsp_wr_mod_fch_gain ( fch_gain );
}


/*===========================================================================

FUNCTION ENC_IS95C_ENABLE

DESCRIPTION
  Turns the IS95C bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_is95c_enable
(
  boolean is95c
)
{
  if ( is95c )
  {
    HWIO_OUTM( MOD_CH2_ENC_CTL_0, ENC_IS95_MODE_M, ENC_IS95_C_V );
    HWIO_OUTM( MOD_MISC_CTL, MSM5100_ENC_IS95A_MODE_M, MSM5100_ENC_IS95A_MODE_DIS_V );
  }
  else
  {
    HWIO_OUTM( MOD_CH2_ENC_CTL_0, ENC_IS95_MODE_M, ENC_IS95_A_V );
    HWIO_OUTM( MOD_MISC_CTL, MSM5100_ENC_IS95A_MODE_M, MSM5100_ENC_IS95A_MODE_ENA_V );
  }
}

/*===========================================================================

FUNCTION ENC_FCH_ENABLE

DESCRIPTION
  Turns the FCH_ENABLE bit of FCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_fch_enable
(
   boolean enable_fch
)
{
   if ( enable_fch )
   {
      HWIO_OUTM( MOD_CH2_ENC_CTL_0, ENC_IS95_FCH_ON_M, ENC_IS95C_FCH_ON_V );


      #if defined(FEATURE_PLT_BASEBAND_ZIF)
#error code not present
      #endif
   }
   else
   {
      HWIO_OUTM( MOD_CH2_ENC_CTL_0, ENC_IS95_FCH_ON_M, ENC_IS95C_FCH_OFF_V );
   }
}


/*===========================================================================

FUNCTION ENC_SCH_ENABLE

DESCRIPTION
  Turns the SCH_ENABLE bit of SCH_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_sch_enable
(
   boolean enable_sch
)
{
  if ( enable_sch )
  {
    HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_IS95_SCH_ON_M, ENC_IS95C_SCH_ON_V );
  }
  else
  {
    HWIO_OUTM( MOD_CH3_ENC_CTL_0, ENC_IS95_SCH_ON_M, ENC_IS95C_SCH_OFF_V );
  }
}


/*===========================================================================

FUNCTION ENC_PCH_ENABLE

DESCRIPTION
  Turns the PCH_ENABLE bit of MISC_CTL register on/off

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_pch_enable
(
   boolean enable_pch
)
{
   if ( enable_pch )
   {
      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN), ENC_PICH_ON_V );
   }
   else
   {
      HWIO_OUTM( MOD_MISC_CTL, HWIO_FMSK(MOD_MISC_CTL,CDMA2000_PICH_EN), ENC_PICH_OFF_V );
   }
}

/*===========================================================================

FUNCTION ENC_PCBIT_TEST_MASK

DESCRIPTION
   Sets the mask in MOD_PCBIT_TEST_MASK register. When a bit in this register
   is set to 1 the corresponding bit of MOD_PCBIT_TEST_DATA register is sent
   as power control bit on the reverse pilot channel.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_pcbit_test_mask
(
  word pcbit_test_mask
)
{
  HWIO_OUT( MOD_PICH_PCBIT_MASK, pcbit_test_mask );
}


/*===========================================================================

FUNCTION ENC_PCBIT_TEST_DATA

DESCRIPTION
   Sets the contents of MOD_PCBIT_TEST_DATA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_pcbit_test_data
(
   word pcbit_test_data
)
{
   HWIO_OUT( MOD_PICH_PCBIT_DATA , pcbit_test_data );
}


/*===========================================================================

FUNCTION ENC_FCH_RC

DESCRIPTION
  Set the reverse fundamental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_fch_rc
(
   enc_fch_radio_config_type fch_rc
)
{
   enc_current_fch_rc = fch_rc;
}


/*===========================================================================

FUNCTION ENC_DCCH_RC

DESCRIPTION
  Set the reverse fundamental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_dcch_rc
(
   enc_fch_radio_config_type dcch_rc
)
{
#ifdef FEATURE_IS2000_REL_B
  enc_current_dcch_rc = dcch_rc;
#else
  enc_current_fch_rc = dcch_rc;
#endif /* FEATURE_IS2000_REL_B */
}
/*===========================================================================

FUNCTION ENC_SCH_RC

DESCRIPTION
  Set the reverse supplemental channel radio configuration

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void enc_sch_rc
(
   enc_sch_radio_config_type sch_rc
)
{
   enc_current_sch_rc = sch_rc;
}

/*===========================================================================

FUNCTION ENC_IS95C_TX_PATN

DESCRIPTION
   Program the IS95C_TX_PATN register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_is95c_tx_patn( word pattern )
{
  /* Gating pattern for Pilot Gating and HHO_PRMBL. */
  if (enc_pilot_gating == TRUE)
  {
    if (pattern == ENC_NO_GATING_PATTERN)
    {
       HWIO_OUTM( MOD_MISC_CTL, ENC_PICH_GATING_RATE_M,
          ( ENC_PICH_GATING_RATE_ALL_V ) );
    }
    else if (pattern == ENC_HALF_RATE_GATING_PATTERN)
    {
       HWIO_OUTM( MOD_MISC_CTL, ENC_PICH_GATING_RATE_M,
          ( ENC_PICH_GATING_RATE_HALF_V ) );
    }
    else if (pattern == ENC_QTR_RATE_GATING_PATTERN)
    {
       HWIO_OUTM( MOD_MISC_CTL, ENC_PICH_GATING_RATE_M,
          ( ENC_PICH_GATING_RATE_QUARTER_V ) );
    }
  }
}

/*===========================================================================

FUNCTION ENC_GATING_MODE

DESCRIPTION
   Program the MOD_MISC_CTL register with one of 3 mutually exclusive pilot
   gating modes: PILOT_GATING_MODE, FCH_8TH_RATE_GATING_MODE or
   HHO_PREAMBLE_MODE, NO_GATING_MODE.

DEPENDENCIES
   The gating pattern must be specified using enc_is95c_tx_patn();

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_gating_mode( enc_gating_mode_type gating_mode )
{
   ASSERT
   (
      ( gating_mode == ENC_NO_GATING_MODE ) ||
      ( gating_mode == ENC_PILOT_GATING_MODE ) ||
      ( gating_mode == ENC_FCH_8TH_RATE_GATING_MODE ) ||
      ( gating_mode == ENC_HHO_PREAMBLE_MODE )
   );

  /*
  ** Program MOD_DCCH_ENC_CTL register.
  */
   if (gating_mode == ENC_NO_GATING_MODE)
   {
        HWIO_OUTM( MOD_MISC_CTL, (ENC_PICH_GATING_RATE_M | ENC_FCH_EIGHTH_GATE_M),
              ( ENC_PICH_GATING_RATE_ALL_V | ENC_FCH_EIGHTH_GATE_DIS_V ) );

         enc_pilot_gating = FALSE;
   }
   else if (gating_mode == ENC_PILOT_GATING_MODE)
   {
     enc_pilot_gating = TRUE;
   }
   else if (gating_mode == ENC_FCH_8TH_RATE_GATING_MODE)
   {
     HWIO_OUTM( MOD_MISC_CTL, ENC_FCH_EIGHTH_GATE_M,
              ( ENC_FCH_EIGHTH_GATE_ENA_V ) );

     enc_pilot_gating = FALSE;
   }
   else
   {
     HWIO_OUTM( MOD_MISC_CTL, ENC_PICH_GATING_RATE_M,
              ( ENC_PICH_GATING_RATE_ALL_V ) );
     enc_pilot_gating = FALSE;
   }
}

/*===========================================================================

FUNCTION ENC_DCCH_ENABLE

DESCRIPTION
  Turns the DCCH_ENABLE bit of MOD_MISC_CTL register on/off

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_enable( boolean dcch_en )
{
  /*
  ** Program MOD_DCCH_ENC_CTL register.
  */
   if( dcch_en )
   {
      HWIO_OUTM( MOD_CH1_ENC_CTL, ENC_DCCH_EN_M, ENC_DCCH_EN_V );
   }
   else
   {
      HWIO_OUTM( MOD_CH1_ENC_CTL, ENC_DCCH_EN_M, ENC_DCCH_DIS_V );
   }
}

/*===========================================================================

FUNCTION ENC_DCCH_DATA_OUT

DESCRIPTION
  Send DCCH data to the encoder, generate a CRC (or not), and encode it at
  a specified rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_data_out
(
  byte *data_ptr,               /* Buffer to send to Encoder               */
  boolean generate_crc,         /* Have the Encoder generate a CRC, or not */
  enc_rate_type rate            /* Rate to encode the data as              */
)
{
  word enc_ctl;                 /* Encoder control value for this packet  */
  word data_size;               /* Data size for this packet              */
  byte crc_len;
  word crc_poly;
  enc_code_rate_type code_rate;

  enc_fch_radio_config_type enc_current_rc = ENC_FCH_RC_3;

#ifdef FEATURE_IS2000_REL_B
  enc_current_rc = enc_current_dcch_rc;
#else
  enc_current_rc = enc_current_fch_rc;
#endif /* FEATURE_IS2000_REL_B */


  ASSERT(
       ( enc_current_rc >= ENC_FCH_RC_3 ) &&
       ( enc_current_rc <= ENC_FCH_RC_4 )
      );
  ASSERT( ( rate == ENC_FULL_RATE ) );


  /* Compute the data size in bytes; round up any fractional part
  */
  data_size = (word)(enc_fch_full_rate_size[ENC_FCH_RC_INX(enc_current_rc)]
            + ((1 << rate) - 1)) >> rate;


  // MSM5100 modulator needs to have 32 bits alignment data input.

  data_size = ((data_size + 3) >> 2) * 4 ;

  if( generate_crc )
  {
     crc_len = (enc_fch_crc_info_tab[ ENC_FCH_RC_INX( enc_current_rc ) ]
                    [ ENC_FCH_RATE_INX( rate ) ]).length;
     crc_poly = (enc_fch_crc_info_tab[ ENC_FCH_RC_INX( enc_current_rc ) ]
                     [ ENC_FCH_RATE_INX( rate ) ]).poly;

     HWIO_OUT( MOD_CH1_CRC_POLY, crc_poly );

  }
  else
  {
     crc_len = 0;
  }

  //
  // program puncture patterns (only affects RC 3 & 4)
  //

  HWIO_OUT(
         MOD_CH1_PUNCT_PATN_0,
         enc_fch_punct_0_tab[ENC_FCH_RC_INX(enc_current_rc)]
                  [ENC_FCH_RATE_INX( rate )]
      );
  HWIO_OUT(
         MOD_CH1_PUNCT_PATN_1,
         enc_fch_punct_1_tab[ENC_FCH_RC_INX(enc_current_rc)]
                  [ENC_FCH_RATE_INX( rate )]
      );


  code_rate = enc_code_rate_tab[ ENC_FCH_RC_INX(enc_current_rc) ];

    if ( enc_current_rc == ENC_FCH_RC_3 )
    {
     enc_ctl = (word) (
            ( ( (word) enc_current_rc ) << 4 ) |
            (                 0x0000 << 3        ) |
            (                 0x0002 << 1        ) |
            ( ( (word) code_rate ) << 7 ) |
            ( ( (word) crc_len ) << 9 )
              );
    }
    else
    {
     enc_ctl = (word) (
            ( ( (word) enc_current_rc ) << 4 ) |
            (                 0x0001 << 3        ) |
            (                 0x0002 << 1        ) |
            ( ( (word) code_rate ) << 7 ) |
            ( ( (word) crc_len ) << 9 )
              );
    }

  HWIO_OUTM( MOD_CH1_ENC_CTL,
         (  HWIO_FMSK(MOD_CH1_ENC_CTL,CH1_CRC_LEN) |
          HWIO_FMSK(MOD_CH1_ENC_CTL,CH1_CODE_RATE) |
          HWIO_FMSK(MOD_CH1_ENC_CTL,CH1_RADIO_CFG) |
          HWIO_FMSK(MOD_CH1_ENC_CTL,CH1_ENC_RATE) |
          HWIO_FMSK(MOD_CH1_ENC_CTL,CH1_FRAME_SIZE)
         ), enc_ctl );

  /* set the pcg count, encode time */

  HWIO_OUT( MOD_CH1_TIMING_CTL,
        (ENC_DCCH_ENCODE_TIME_V | ENC_FCH_PCG_COUNT_V)
      );

  /* set the walshcover select */

  HWIO_OUTM( MOD_WCOVER_SEL,
         ENC_WALSHCOVER_SEL_DCCH_M,
         ENC_WALSHCOVER_SEL_DCCH_V
       );

  if( enc_current_rc > ENC_FCH_RC_2 )
  {
     enc_is95c_enable( TRUE ); // IS-95C
     enc_pch_enable( TRUE );
  }
  else
  {
     /* in the normal DMSS mode, we already program the gains
     in the txc_traffic_isr(), but somehow we still do it again here??
     Maybe for FTM mode. Until we find out the exact reason, then we can remove it here  */
     enc_is95c_enable( FALSE ); // IS-95A
     enc_pch_gain( 0 );
     enc_fch_gain(TXC_IS95A_FCH_GAIN);
     enc_pch_enable( FALSE );
  }

  /* Send the data to the encoder */
  enc_write_data(ENC_CHAN_1,data_ptr,data_size,TRUE);
}

/*===========================================================================

FUNCTION ENC_DCCH_GAIN

DESCRIPTION
  Set DCCH gain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_dcch_gain( word dcch_gain )
{
   muxmdsp_wr_mod_dcch_gain ( dcch_gain );
}

/*===========================================================================

FUNCTION ENC_SET_TX_POLARITY

DESCRIPTION
  Set spectral polarity for TX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_tx_polarity( enc_tx_polarity_type tx_polarity )
{
  HWIO_OUTM( MOD_TEST_CTL, HWIO_FMSK(MOD_TEST_CTL,TX_SPECTRAL_INVERSION), tx_polarity );
}

#if !defined(T_MSM7800)
/*===========================================================================

FUNCTION ENC_SET_RX_POLARITY

DESCRIPTION
  Set spectral polarity for RX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void enc_set_rx_polarity( enc_rx_polarity_type rx_polarity )
{

#ifdef T_IO_CARD
#error code not present
#elif defined(T_MSM7600)
#error code not present
#elif defined(T_QSC6055)
  HWIO_OUTMI( RXF_SRCc_CTL, 0, ENC_RX_POL_M, rx_polarity );
  HWIO_OUTMI( RXF_SRCc_CTL, 1, ENC_RX_POL_M, rx_polarity );
#else /* !T_MSM7600 && !T_QSC6055 && !T_IO_CARD */
  HWIO_OUTMI( RXFc_CTL, 0, ENC_RX_POL_M, rx_polarity );
  HWIO_OUTMI( RXFc_CTL, 1, ENC_RX_POL_M, rx_polarity );
#endif /* !T_IO_CARD && !T_MSM7600 && !T_QSC6055 */

}
#endif /* !T_MSM7800 */

/*===========================================================================

FUNCTION enc_write_data

DESCRIPTION
  This function is used to write data into the encoder for a given channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
const uint16 enc_chan_ctl_mask_array[ENC_TOTAL_CHAN] = {0x0080,0x0081,0x0082,0x0083};
void enc_write_data
       (
         enc_chan_number_type  chan_num,     /* which channel will be written to */
         void                  *src_data_ptr, /* pointer points to source data */
         uint16                total_num_of_byte, /* Total number of byte */
         boolean               last_write /* indicated if this is the last write for a given channel */
       )
{
  uint16  enc_chan_ctl_mask = enc_chan_ctl_mask_array[chan_num];
  uint16  byte_remainder = total_num_of_byte;

  /* check if there is something to write */
  if ((total_num_of_byte > 0) && (src_data_ptr != NULL))
  {
    /* check if the source pointer is word or haft-word aligment */
    if ((((dword)src_data_ptr) % 4) == 0)
    {
      byte_remainder = (total_num_of_byte % 4);
      HWIO_OUT( MOD_ERAM_DATA_CTL,enc_chan_ctl_mask);
      outsw_rep((void *) HWIO_ADDR(MOD_ERAM_DATA), (src_data_ptr), (word) (total_num_of_byte/4));
    }
    else if ((((dword)src_data_ptr) % 2) == 0)
    {
      byte_remainder = (total_num_of_byte % 2);
      /* set haft-word acessing mode */
      enc_chan_ctl_mask = enc_chan_ctl_mask | 0x0004;
      HWIO_OUT( MOD_ERAM_DATA_CTL,enc_chan_ctl_mask);
      /* write the first even number of byte */
      outsbh_rep ((void *) HWIO_ADDR(MOD_ERAM_DATA),(src_data_ptr),(total_num_of_byte - byte_remainder));
    }

    /* check if there is a non-zero remainder, then use byte accessing mode */
    if (byte_remainder > 0)
    {
      uint8  *remain_ptr;
      remain_ptr = (uint8 *) src_data_ptr;
      enc_chan_ctl_mask = enc_chan_ctl_mask & 0xFFF3;
      enc_chan_ctl_mask = enc_chan_ctl_mask | 0x0008;
      HWIO_OUT( MOD_ERAM_DATA_CTL,enc_chan_ctl_mask);
      outsb_rep((void *) HWIO_ADDR(MOD_ERAM_DATA),(remain_ptr + (total_num_of_byte-byte_remainder)),byte_remainder);
    }
  }
  /* check if this is the last write, then tell the HW writing to ERAM is DONE */
  if (last_write)
  {
    HWIO_OUT( MOD_ERAM_WR_DONE,0x1);
  }
}
/*===========================================================================

FUNCTION enc_write_data_done

DESCRIPTION
  This function is used to indicate the HW writing to ENCODER is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_write_data_done (void)
{
  HWIO_OUT( MOD_ERAM_WR_DONE,0x1);
}

/*===========================================================================

FUNCTION enc_hw_reset

DESCRIPTION
  This function resets the encoder HW block.  This step is necessary to avoid
  failed 1x access attempts between EVDO and 1X transition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None.

===========================================================================*/
void enc_hw_reset (void)
{
  HWIO_OUT( MOD_RESET, 0x0 );
  MSG_MED ("Reset enc",0,0,0);
}

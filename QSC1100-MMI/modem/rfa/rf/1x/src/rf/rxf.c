/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 R X  F R O N T  E N D  D R I V E R

GENERAL DESCRIPTION
  This file contains variable declarations to support interaction
  This file contains Software Code for the RX Front End Driver.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2001, 2002, 2003 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007             by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rxf.c#3 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------
11/12/08   lp      Fixed Lint errors
10/30/08   sr      Klocwork fixes
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
03/12/08   vm      Program the resampler to be based on SC1x
03/05/08   vm      Made changes to Rx Friont HW register settings
                   for Napolean and RICO Bringup
11/28/07   ems     Support for QSC1100
10/17/07   ra      changed bbrx_gps() to integrate with aries engines
09/27/07   ans     Added capability to update FG offset scaler values based on
                   RFR device.
08/29/07   dyc     Update NV table featurization for BC3,4 and 6.
07/20/07   avi     Mainlined clockl regime management (previously featurized for
                   not MSM7600 targets)
05/14/07   vm      Made change so that function rxf_set_group_delay()
                   does not do anything for SC2X
04/18/07   vm      Added support for ADC and modem clock switching
                   for all the different modes of operation.
03/22/07   vm      Changed the settings of RXF_SRCc_Offset_ctl register
                   acc to HWSW DOC to change DC loop Bandwidth
03/13/07   dyc     Fixed unsigned shift compiler warning.
03/09/07   bmg     BC15 feature fix
03/04/07   dyc     Added support for QSC60x5
01/09/07   adk     Updated RXFRONT clk regimes.
01/05/07   adk     Merged changes from 6800 Rel 6: Added function rxf_hdr_wakeup().
                   Updated RXFRONT clk regimes.
12/12/06   hkk     Added support for D-450
11/16/06   jfc     Use RF HAL for HWIO shadow access
11/14/06   adk     Conditionally include BYPASS_DF2, TRK_LO_ADJ_SEL, and
                   BYPASS_GPS_MAT_FILT using the flag RF_TODO_7600_ADK
11/07/06   ra      start cleanup for CGPS
10/17/06   ycl     Modifications for initial MSM7600 build.
08/16/06   kss     Added rxf_hdr_wakeup().
03/30/06   zg/sar  Added two new functions for ZRF6185 support.
03/24/06   bmg     Removed the DACC Enable from rxf_program_mode_cdma().  During
                   sleep warmup the DC accumulator would be enabled before it was
                   necessary and could disrupt later AGC convergance.
02/15/06   sar     Populated GPS slot in the rxf_band_class_params table.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
01/04/05   et      LINT fixes- unsupressed error 641 and fixed it all over
12/15/05   et      LINT fixes
12/07/05   hkk     Stub functions to make Libraries compatible 
11/29/05   hkk     Removed the stub functions added for RFR6155
11/15/05   rmd     Added support for band class 11.
11/22/05   hkk     Stubbed 2 functions to resolve compile error with RFR6155
10/28/05   sar     Eliminated Lint errors.
10/27/05   bmg     Changed ASSERTs to RF_ASSERTs
10/14/05   zg      Explicitly selecting AGC mode for 1x/GPS mode to allow the 
                   the transition from HDR to GPS.
10/03/05   sar     Eliminated setting rxf mode to Inactive while in sleep. This fixes
                   a bug that prevented setting of Rx-1 chain DVGA offsets in sHDR mode.
09/28/05   jfb     Fixed compile error for non power mode lib targets
09/27/05   rmd     Added support for band class 14 (US PCS + 5MHz).
09/26/05   dyc     Converted vga gain info to MED msg type.
09/23/05   dyc     Add path parameter to rxf_init()
09/07/05   bmg     Added rxf_set_group_delay() function.
08/17/05   ra      changed rxf_bypass_rtc_ctl_on_off to handle device and on/off
08/13/05   ra      added support for bypass rtc(enable/disable). remove call to bypass rtc in
                   rxf_init()
07/25/05   grl     Moved trk_lo mux control into 1x and HDR search's control.
06/17/05   zg      Fixed LINT errors.
05/23/05   ans     Added support for software dc cancellation for SGPS for KPZRF.
05/09/05   ra      For DORA, added RF_HAS_MODEM_GENERIC_CTL
04/06/05   bt/ra   Added RF_HAS_NO_RTC_DEPENDENCY
03/30/05   zg      Cleaned up GPS DC polarity configuration.
03/30/05   lp      Added RF_HAS_MSM7500_EMULATION support to rxf_init.
03/16/05   bmg     Added IntelliCeiver mode support for saving DC offsets.
02/22/05   zg      Merged GPS vga_gain_offset fix from P4 tip.
02/21/05   bmg     Added polarity setting in rxf_set_mode_gps_preswitch(),
                   eventually ziflib_get_gps_dc_dac_polarity() can be removed.
02/07/05   zg      Explicitly configured vga_gain_offset for GPS mode.
01/09/05   dyc     Typo in DORA #ifdef
01/05/05   ra      DORA merge. Change TRK_LO_ADJ mode to HDR during HDR enable
12/29/04   bmg     Removed rxf_band_class_params member prescaler, replaced it
                   with a call to the new ziflib API ziflib_get_dc_mult(),
                   supporting RFR6500.  Also merged
                   ziflib_get_gps_dacc_mult_prescaler_value() functionality into
                   ziflib_get_dc_prescaler().
11/08/04   sar     Added initializer values for BC2 in rxf_band_class_params struct
                   to eliminate RVCT errors.
11/05/04   sar     Moved call to rfnv_get_nv_data() to the front in rxf_init().
10/31/04   dyc     Extern ziflib_get_gps_dacc_mult_prescaler_value()
10/29/04   ans     Added API ziflib_get_gps_dacc_mult_prescaler_value to choose 
                   GPS_DACC_MULT_SCALER based on RFR.
09/30/04   jac     Fixed compile error in rxf_init() for Hidra 5.0 merge.
09/30/04   aks     Merge in Hidra PLT RF changes.
09/07/04   dyc     Added prototypes for ziflib functions.
09/01/04   sar     Added function void rxf_set_rx_polarity().
08/29/04   dyc     Used ziflib_get_lna_range_override() to determine RFR setting.
08/19/04   jac     Added rf mode parameter to rflib_set_sbi_info_rxf() calls.
                   Added DC DAC polarity check to rxf_set_mode_gps().
08/16/04   dyc     Remove DB_RF_ANALOG components.
07/26/04   et      removed temporary change to disable interpolation for 
                   HDR runtime.
06/21/04   et      changed the rxf[path].mode to rf_hdr_is_enabled variable
                   when doing runtime detect for HDR for interpolation
06/11/04    et     added !hdr_enabled condition to interpolation, because
                   interpolation is not yet supported for HDR.
05/28/04   sar     Updated feature flag names for nv table.
05/12/04   sar     Updated for enhanced bands type.
05/12/04   dyc     Remove ifdef around RXF_DFM_IF_OFFSET, required for CDMA
04/22/04   AP/dyc  Feature-ized useage of registers for BYPASS_DF2 and DFM.
04/09/04   jac     Added band class 4 support
03/19/04   et      added interpolation (frequency comp) support
01/27/04   sar     Replaced mismatch comp nv items to hard-code values in 
                   rxf_program_mode_cdma().
11/05/03   dyc     Move RXF_CTL config to rxf_init().  Default to CDMA vs DFM.
11/03/03   kss     Switch to 1x mode in rxf_cdma_wakeup() if coming from HDR.
10/22/03   zg      Restored RXF_Reset in rxf_program_mode_cdma(), which was
                   lost while porting from 6100.
10/13/03   zg      Removed a call to an empty function clk_regime_config_gps
                   _rf_clocks().
10/08/0    zg      Muitlple chain BBRX support.
09/23/03   dyc     Updated for band class 3 and 6 support.
09/19/03   dbc     Changed value written to RXFc_CTL in rxf_program_mode_cdma()
                   when T_IO_CARD_PANDA is defined. 
09/10/03   zg      Added GPS Baseband PLT support (under T_IO_CARD_GPS_PANDA)
09/03/03   dbc     Use global RF variable rf_chain_status[] to access RF state
                   related variables
08/27/03   dyc     Update to use grp_delay_adj nv items.
08/11/03   sar/dyc Update nv for multi-path support.
08/21/03   kss     Added missing HDR case to switch statements.
08/10/03   dyc     Use nv DVGA_GAIN_OFFSET value vs hardcoded.
07/30/03   dyc     Updates for GPS
07/30/03   bmg     Correct fix for LNA state 4 added, DACC support for RFR6125
07/10/03   kss     Add initial support for multiple rxf paths.
06/26/03   dyc     removed #ifdef 0 and commented out code
06/24/03   et      set the CDMA_FM_SEL bit in SYSTEM_MODE to 1 for PCS and CDMA
06/18/03   et      cleaned  up more magic numbers
06/17/03   et      ported all the CLOCK code
06/16/03   dyc     removed temporary AGC_MIN value assignments from this file.
06/15/03   kss     Added initial HDR support. 
06/11/03   et      cleaned up most magic numbers
06/11/03   dyc     temporary mv AGC_MIN value assignement to rxf set mode functions
06/10/03   dyc     update DACC_CTL and OFFSET_CTL as per 6500 hw/sw doc
06/05/03    et     updated some #defines and added RXF_RF_ infront of all the
                   _V values inside T_MSM6500
06/04/03   et      fixed typo
06/04/03   et      additional porting for PCS, some cleanup, and register
                   value updates.
06/02/03   dyc     move RX_AGCc_MODE_SEL outside #ifdef RF_HAS_DACC_REG_ENHANCE
                   remove hardcoded RXFc_OFFSET_CTL value in rxf_set_mode_cdma
05/20/03   dyc     default to close loop rx operation.
05/19/03   dyc     rxf0_panda_close() rx open loop set lna_ctl to 0x60
05/13/03   et      ported msm6500 regs
05/11/03   dyc     Updated for current Jaguar RF debug via temp function
                   rxf0_panda_close()
                     -reduce dc gain
                     -freeze Rx AGC loop
04/30 03    et     removed extra call to rxf0_panda_close.
04/14/03    et     merged with MSM6100 3030 build
03/31/03    et     added rxf0_panda_close function
                   changed code to include RXFRONT for QUASAR_RF and BBRX
02/05/03    zg     Removed RXF_RESET from rxf_restore_dc_offset(). Added
                   a structure dacc_status for saving data before GPS visit.
01/14/03   dbc     Added support for different DACC_MULT_PRESCALER values in
                   CDMA/FM modes for RFR6125
01/13/03   aks     Changes for GSM PLT.
12/20/02   dbc     Added rxf_program_digital_mode().  Changed rxf_set_mode_cdma()
                   and rxf_set_mode_pcs() to take a boolean parameter.
11/26/02   sar     Changed all the scaling factors to 12 for PCS/CDMA/GPS mode
                   NV values for VGA Gain Offset frequency compensation tables.
11/05/02   dyc     Support for RF_HAS_DIFF_RXMOD_CLK and RF_HAS_BYPASS_MIS_COMP
                   Replace clk_regime_get_rxmod_plldiv() with
                   clk_regime_get_rxmod_src_select_value()
10/30/02    zg     Switch AGC clock in rxf_set_mode_gps.
10/24/02   dyc     Support for T_MSM6100B
                   Changes for PLT support.
10/09/02   bgc     Implemented workaround for MSM6000_CLK2 problem.
09/20/02    zg     Added RF_HAS_DACC_REG_ENHANCE for GPS mode(MSM6100).
09/20/02   dlb     Implemented workaround for MSM6050_CLK13 problem.
09/12/02   dyc     For MSM6100/MSM6300, use clk_regime_get_rxmod_plldiv()for
                   defining cell/pcs PLL rxmodclk divisor.
09/04/02   dbc     Remove legacy code.
08/13/02    zg     Moved disabling DACC to rxf_set_mode_gps_preswitch().
08/03/02    zg     Set dacc_mult_prescaler to half mode for GPS.
07/26/02   dyc     Corrected a comment.
                   Set CG_OFFSET_ACCUM_EN bit to 0. Disabled micro_lna_range_overide
                   in PCS/CDMA/GPS modes and enabled it in DFM mode.
                   Use CX16 as clock source for frequency tracking PDM in CDMA
                   Saved/Restored DACC_ACCUM0 instead of DACC_ACCUM3 for GPS.
                   Added a new function rxf_set_dacc_update().
                   (merged from aks,sar,dbc,zg MSM6000_RF.01.00.15-16)
07/17/02   dyc     Added rxf_disable_dacc() function API function . Disable
                   micro_lna_range_overide. Added rf_get_cagc_linearizer_range()
                   to dynamically set the VGA Gain scale.
                   (merged from dbc rf.01.00.13)
07/15/02   dyc     Split rxf_set_mode_gps() into two functions, and the first one
                   need to be called before SRCH switches mode.
                   (merged from zg rf.01.00.12)
07/09/02   dyc     Set rx_mod_clk to use PPLOUT/5
                   Set dc_offset_polarity to INV for DFM and GPS
                   Inverted the dacc_dc_offset_polarity for all modes.
                   Added shift to QVAL entry for DACC_GAIN_MULT
                   Cougar Port baseline.
                   Enable new SBI bit in DACC during init.
                   Duplicate the gain_mult value for separate I/Q gain entries.

06/24/02   sar     Revised the power-on programming sequence for Rev 2.0.
06/20/02   aks     For Leo2.0, DC offset polarity needs to be a value of INV.
06/18/02   aks     Added DACC_DC_OFFSET_POLARITY for DFM and gpsOne modes.
06/13/02   aks     Wrapped Leo rev2.0 code in appropriate flags.  This
                   polarity must be re-evaluated for the MSM6100 target.
06/12/02   dbc     Added rxf_cdma_wakeup() and rxf_cdma_wakeup_dc_acquire()
06/12/02   sar     Added run time DACC_DC_OFFSET_POLARITY selection to
                   support Leo rev2.0
06/10/02   sar     Disabled async_samp_clk for PCS mode.
06/06/02   dbc     Register changes for rev2 of MSM6050
05/09/02   dyc     Change DFM pre-scaler and est_accum_freq values
05/06/02   sar     Normalize the vga_gain_offset freq comp values by multiplying them
                   by 12 for fm and by 10 for cdma, pcs and gps modes since these
                   values were scaled down by the same factors at calibration.
04/24/02   sar     Corrected typo in the function rxf_save_accum_data().
04/19/02    zg     Added rxf_load_accum_data for GPS.
04/11/02   sar     Updated values for IQ_OFFSET_PDM_OE, IQ_OFFSET_PDM_SEL per
                   HW SW Document.Enabled cordic for all modes except GPS.
                   Added delays after acquisition command in rxf_dc_acquire()
04/10/02   sar     Removed unused local variable from rxf_init().
04/02/02   sar     Added checks for null pointers in rxf_save_accum_data() since
                   this function is called outside of Rx front.
04/01/02   sar     Moved calls to rxf_load_accum_data() and rxf_set_vga_gain_offset()
                   to rxf_set_mode().
03/26/02   dyc     Update DFM dac_prescaler value to DACC_MULT_PRESCALER_ONE_V
03/25/02   aks     Set dacc_mult_prescaler to half mode for Cell and PCS, and
                   to quarter mode for gpsOne.
03/25/02   dyc     Update DFM dac_prescaler value for alternate channel test
03/22/02   dyc     Disable DFM iq_offset_pdm
03/21/02   sar     Added functions rxf_set_vga_gain_offset() which allow setting
                   of vga gain offset with frequency compensation.
03/21/02   aks     Moved some code into Library API functions.
03/18/02   sar     Updated code to always use TCX0 for PDM clocks to eliminate
                   rxf dependency on DFM mode. Eliminated functions to switch PDM
                   clocks.
03/14/02   sar     Converted the NV items to Hard-coded values per new RF NV
                   Requirements.
03/14/02   dyc     DFM: Disable async_samp_clk and set dacc_mult_prescaler to half mode
03/08/02    zg     Added rxf_save_dc_offset and rxf_restore_dc_offset for GPS
                   tuneaway and tuneback.
03/07/02   sar     Moved calls to write macros to "DACC_EST_THRESH" into RF_HAS_ZIF
                   sections of the code for each mode.
03/05/02   dyc     Restore previous RXF_CTL and RXF_DACC changes for ZRF X1 operation.
02/28/02   sar     Added rxf_save_accum_data and rxf_load_accum_data functions
                   to save and retrieve accumulator values to and from NV. Cleaned
                   up code in rxf_set_mode_gps.
02/28/02   dyc     Updated rx_dacc_ctl value for DFM.
02/22/02    zg     Moved bbrx_set_mode into rxf_set_mode for CDMA and PCS.
                   Replaced FEATURE_GPSONE with RF_HAS_GPS.
02/15/02   dyc     Enable RX_MOD_CLK_EN bit for DFM (mapped as TXCO in MSM6000)
02/12/02   sar     Changed clock regime identifiers to conform to the release of
                   MSM_6050_CLK_RGM.01.00.14. Updated code to explicitely set
                   all mode related RXF_CTL bits that get affected in any mode.
02/08/02   dyc     Hard code FG_TRC_OFFSET_SCALER_DFM_V value for QRF mode.
02/06/02   sar     Corrected the switch identifier T_MSM_6050 to T_MSM6050.
02/06/02   dyc     Updates for ZIF/DFM operation.
02/01/02   zg      Merged in GPS related changes from PhoneT.
02/01/02   sar     Corrected use of unshifted NV Values at some places in the code.
01/31/02   sar     Added Enable command for LNA and MIX timers in rxf_dc_acquire
                   to allow FSM to return to Tracking after acquisition is complete.
                   Added Disable command for LNA and MIX timers in rxf_dc_track to
                   Disable these timers.This fixes the sleep/wakeup problems in
                   QRF5600 mode.
01/30/02   dbc     Removed blanking of RXF_CTL register in rxf_set_mode()
                   because it causes the system to crash when mode switching.
01/29/02   sar     Added code to properly handle the signed integer and long type
                   RF NV items.
                   Added code to blank out RXF Control register in rxf_set_mode,
                   to allow modes to set bits to their own requirements.
                   Added rxf_is_sleeping flag to bypass rxf wakeup processing when
                   rf_init_digital calls rxf_wakeup at initialization.
01/18/02   dyc     Support for QRF5600 DFM, enable txco clock for rx_mod for MSM6000
01/18/02   sar     Integrated changes from phone_t for zif mode.
01/15/02   sar     Added rxf_wakeup,rxf_wakeup_delayed,rxf_dc_acquire, rxf_dc_track,
                   rxf_set_pdm_clk_digital and rxf_set_pdm_clk_tcx0 to support
                   digital and analog sleep modes in RX Front.
                   Fixed clock selection using the disable/select/enable protocol.
01/10/02   sar     Fixed bug in rxf_init() to use 32-bit macro to write to
                   register DACC_EST_THRESH_R.
01/08/02   sar     Integrated PLT changes to: not bypass the front end for
                   ZIF digital IO testing; Set test mode to GPIO for plt; and
                   fixed mask values for setting test modes.
12/21/01   sar     Added code to initialize additional ZIF mode default parameters
                   per latest revision of RadioOne document.
12/13/01   bgc     Updated CLK_RGM_MOD to match MSM6050 and removed #ifdefs.
12/5/01    cml     Updated clk_regime_sel_clk_src to pass CLK_RGM_RXMOD_BLK.
12/3/01    sar     Modified rxf_init() and rxf_set_mode_xxxx() functions to
                   select clock source for CLK_RGM_RXMOD_BLK based on selected mode.
11/29/01   sar     Updated code to use new NV items. Enclosed PCS code segments in
                   FEATURE_TRIMODE_ITEMS switch.
11/21/01   sar     Included changes to select chipx16, single ended.
11/19/01   sar     Added API's to read DC I/Q offsets and Set VGA Gain Offset.
                   Updated static values per latest release of Radio One doc.
                   Made changes to the temporary NV structures to be in sync with
                   the latest NV Requirements.
11/15/01   aks     Moved location of call to rxf_reset().  Select and enable
                   the analog clock source.
11/14/01   aks     Remove extra commas from macro calls.
11/13/01   aks     Wrap control of rx_tcxo_en in T_MSM_6050.
                   macro to RF_HAS_ZIF feature.
11/12/01   sar     Fixed the clock selection code for each mode. Added ZIF mode
                   acquisition command (dacc update). Moved call to Freeze AGC
11/02/01   tn      ifdef's all calls for pdm clk sel under RF_HAS_PDM_CLOCK_SRC_SEL.
11/01/01   tn      added clk enable, clk sel and reset call for baseband option.
10/19/01   sar     Moved mode related initialization in rxf_init() to specific
                   function calls. Integrated ZIF and RFR code in rxf_init.
10/12/01   sar     Revised rxf_init and rxf_set_mode for updated masks and values.
                   Added PLT support and mode dependent clock switching.
09/06/01   sar     Created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rxf.h"
#include "nv.h"
#include "rfnv.h"
#include "clkregim.h"
#include "rf.h"
#include "rflib.h"
#include "rfi.h"
#include "bbrx.h"
#include "msm_help.h"
#include "cagc.h"
#include "rf_cards.h"

#include "rf_hal.h"

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
#include "rf_adc_samp_freq.h"
#endif

#ifdef RF_HAS_RXF_V2
#define RF_FG_DC_ACCUM_ENABLE_VAL 1
#define RF_FG_DC_ACCUM_DISABLE_VAL 0

#ifdef RF_HAS_RXF_V2_DEBUG
#error code not present
#endif /* RF_HAS_RXF_V2_DEBUG */

#endif /* RF_HAS_RXF_V2 */

#ifndef FEATURE_GSM_PLT

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* Pointer to all NV  items                                */
LOCAL rfnv_all_data_type         rxf_nv_all_ptr;

#ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
#endif /* RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL */


#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
extern rf_adc_clk_state_type rf_adc_clk_status;
#endif

#ifdef RF_HAS_RXF_V2
rxf_values_type rxf_defaults = 
{
    1, /*  bypass_imc */
    1, /*  bypass_mis_comp */
    0, /*  bypass_dcc */
    0, /*  bypass_bbf */
    0, /*  bypass_src */
    0, /*  bypass_rxf */
    1, /*  bypass_cordic */
    0, /*  bypass_dvga */
    1, /*  micro_gain_override_n */
    1, /*  spectral_inversion */
    0, /*  dacc_ctl */
    0, /*  dc_ioffset_adj */
    0, /*  dc_qoffset_adj */
 0x40, /*  rx_agc_lna_ctl */
    1, /*  rx_agc_lna_range */
    3, /*  rx_agc_decision */
    1, /*  grey map enable */
 0xdd, /*  grey_map */
 0x37, /*  micro_vga_gain */
 100,  /*  vga_gain_offset */
    0, /*  vga_mode */
    0  /*  vga_add_en */
};
#endif /* RF_HAS_RXF_V2 */

/* Structure for storing DC offset */
typedef struct {
  uint32 i_est;
  uint32 q_est;
  uint32 fg_i_offset;
  uint32 fg_q_offset;
  uint16 i_accum0;
  uint16 q_accum0;
  uint16 i_accum1;
  uint16 q_accum1;
  uint16 i_accum2;
  uint16 q_accum2;
  uint16 i_accum3;
  uint16 q_accum3;
  uint16 i_accum4;
  uint16 q_accum4;
} rxf_dacc_status_type;

/* Defines the valid Rx Front modes */
typedef enum
{
  RXF_POWERUP = 0,       /* Powerup state. Not initialized */
  RXF_CDMA_MODE,         /* 1x/CDMA mode */
  RXF_HDR_MODE,          /* HDR mode */
  RXF_DFM_MODE,          /* Analog mode - not supported */
  RXF_GPS_MODE,          /* GPS mode */
  RXF_INACTIVE           /* Inactive (sleep) */
} rxf_mode_type;

/* Number of Rx chains */
#define RXF_SUPPORTED_PATHS 2

/* Current status of each Rx Front block */
LOCAL struct
{
  /* Current mode */
  rxf_mode_type mode;

  /* Previous band class -- may not be needed */
  rf_card_band_type prev_band;

  /* Current band class */
  rf_card_band_type band;         //may have to use diff band class type

  /* Index into band class info array (for current band class) */
  uint16 band_class_index;

  /* Structure for storing DC offset */
  rxf_dacc_status_type dacc_status;

  /* Enum for setting up clock regime */
  clkrgm_rx_chain_type clock_setup;

  /* Pointer to current digital NV items */
  LOCAL rfnv_digital_items_type   *digital_nv_ptr;

} rxf[RXF_SUPPORTED_PATHS];


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* To implement additional band class for Rx Front, modify the tables below  */

/* Value used for unsupported band class */
#define RXF_BAND_CLASS_NOT_AVAILABLE 0xFF

/* Array index for band class 0 */
#define RXF_BAND_CLASS_0_INDEX 0

/* Array index for band class 1 */
#define RXF_BAND_CLASS_1_INDEX 1

/* Array index for band class 2 */
#define RXF_BAND_CLASS_2_INDEX 2

/* Array index for band class 3 */
#define RXF_BAND_CLASS_3_INDEX 3

/* Array index for band class 4 */
#define RXF_BAND_CLASS_4_INDEX 4

/* Array index for band class 5 */
#define RXF_BAND_CLASS_5_INDEX 5

/* Array index for band class 6 */
#define RXF_BAND_CLASS_6_INDEX 6

/* Array index for band class 7 */
#define RXF_BAND_CLASS_7_INDEX 7

/* Array index for band class 8 */
#define RXF_BAND_CLASS_8_INDEX 8

/* Array index for band class 9 */
#define RXF_BAND_CLASS_9_INDEX 9

/* Array index for band class 10 */
#define RXF_BAND_CLASS_10_INDEX 10

/* Array index for band class 11 */
#define RXF_BAND_CLASS_11_INDEX 11

/* Array index for band class 12 */
#define RXF_BAND_CLASS_12_INDEX 12

/* Array index for band class 13 */
#define RXF_BAND_CLASS_13_INDEX 13

/* Array index for band class 14 */
#define RXF_BAND_CLASS_14_INDEX 14

/* Array index for band class 15 */
#define RXF_BAND_CLASS_15_INDEX 15

/* Array index for GPS */
#define RXF_GPS_INDEX 16

/* Array index for FM */
#define RXF_FM_INDEX 17

/* Array index for MAX */
#define RXF_MAX_INDEX 18

/* Number of 1x/HDR bands supported */
#define RXF_BAND_CLASS_NUM     2

/* Max band class value allowed */
#define RXF_BAND_CLASS_MAX     16


/* The following array is index by band class, and indicates which
   bands are supported. */
byte rxf_band_index[] =
{
  RXF_BAND_CLASS_0_INDEX,            /* Band class 0 */
  RXF_BAND_CLASS_1_INDEX             /* Band class 1 */

#ifdef RF_HAS_BC2_NV_TABLE           /* Band class 2 */
  ,RXF_BAND_CLASS_2_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC3_NV_TABLE           /* Band class 3 */
#error code not present
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC4_NV_TABLE           /* Band class 4 */
  ,RXF_BAND_CLASS_4_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC5_NV_TABLE           /* Band class 5 */
  ,RXF_BAND_CLASS_5_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC6_NV_TABLE           /* Band class 6 */
#error code not present
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC7_NV_TABLE           /* Band class 7 */
  ,RXF_BAND_CLASS_7_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC8_NV_TABLE           /* Band class 8 */
  ,RXF_BAND_CLASS_8_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC9_NV_TABLE           /* Band class 9 */
  ,RXF_BAND_CLASS_9_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC10_NV_TABLE          /* Band class 10*/
  ,RXF_BAND_CLASS_10_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC11_NV_TABLE          /* Band class 11*/
#error code not present
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC12_NV_TABLE          /* Band class 12*/
  ,RXF_BAND_CLASS_12_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC13_NV_TABLE          /* Band class 13*/
  ,RXF_BAND_CLASS_13_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC14_NV_TABLE          /* Band class 14 */
  ,RXF_BAND_CLASS_14_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

#ifdef RF_HAS_BC15_NV_TABLE          /* Band class 15 */
  ,RXF_BAND_CLASS_15_INDEX
#else
  ,RXF_BAND_CLASS_NOT_AVAILABLE
#endif

  ,RXF_GPS_INDEX             /* GPS place holder */

  ,RXF_FM_INDEX                      /* FM place holder */

  ,RXF_MAX_INDEX                     /* max index place holder */

};

/* Band class-specific parameters. Table is indexed by the above
   rxf_band_index table */
struct
{ 
  /* BBRX mode for band           */
  bbrx_mode_type bbrx_mode;          

  /* rflib init function for band */
  void (*rflib_init_func)(rf_path_enum_type path);     

  /* NV data pointer              */
  rfnv_digital_items_type **nv_ptr; 

} rxf_band_class_params[] =
{
  { /* BAND CLASS 0 -- 800 MHz Cellular (RXF_BAND_CLASS_0_INDEX) */
    BBRX_CELL_MODE,
    &rflib_set_cdma_rxf, /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_cdma_items_ptr       
  },
  { /* BAND CLASS 1 -- 1900 MHz PCS     (RXF_BAND_CLASS_1_INDEX) */
    BBRX_PCS_MODE,
    &rflib_set_pcs_rxf,  /*lint !e546 valid function pointer*/
    #ifdef FEATURE_TRIMODE_ITEMS
    &rxf_nv_all_ptr.rfnv_pcs_items_ptr
    #else
    &rxf_nv_all_ptr.rfnv_cdma_items_ptr       
    #endif
  },
#ifdef RF_HAS_BC2_NV_TABLE
  { /* BAND CLASS 2 -- Overloaded with Cell */
    BBRX_CELL_MODE,
    &rflib_set_cdma_rxf, /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_bc2_items_ptr       
  },
#else
  { /* Place Holder for BC2 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#else
  { /* Place Holder for BC3 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
#ifdef RF_HAS_BC4_NV_TABLE
  { /* BAND CLASS 4 -- KPCS */
    BBRX_CELL_MODE,
    &rflib_set_pcs_rxf,   /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_bc4_items_ptr       
  },
#else
  { /* Place Holder for BC4 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
#ifdef RF_HAS_BC5_NV_TABLE
  { /* BAND CLASS 5 -- NMT */
    BBRX_CELL_MODE,
    &rflib_set_cdma_rxf,  /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_bc5_items_ptr       
  },
#else
 { /* Place Holder for BC5 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#else
 { /* Place Holder for BC6 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
  { /* Place Holder for BC7  */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
  { /* Place Holder for BC8  */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
  { /* Place Holder for BC9  */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
  { /* Place Holder for BC10 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#else
  { /* Place Holder for BC11 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
  
  { /* Place Holder for BC12 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
  { /* Place Holder for BC13 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#ifdef RF_HAS_BC14_NV_TABLE
  { /* BAND CLASS 14 -- US PCS + 5MHz.*/
    BBRX_PCS_MODE,
    &rflib_set_pcs_rxf,   /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_bc14_items_ptr
  },
#else
  { /* Place Holder for BC14 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif
#ifdef RF_HAS_BC15_NV_TABLE
  { /* BAND CLASS 15 -- AWS band */
    BBRX_PCS_MODE,
    &rflib_set_pcs_rxf,   /*lint !e546 valid function pointer*/
    &rxf_nv_all_ptr.rfnv_bc15_items_ptr
  },
#else
  { /* Place Holder for BC15 */
    BBRX_INIT_MODE,
    NULL,
    NULL
  },
#endif

  { /* GPS BAND */
    BBRX_GPS_MODE,
    &rflib_set_gps_rxf,   /*lint !e546 valid function pointer*/
    #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
    #else /*RF_HAS_GPS*/
    NULL
    #endif
  }
};

/* End of band class definitions                                             */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* library functions */
extern boolean ziflib_get_lna_range_override(rf_path_enum_type path);
extern int ziflib_get_dc_prescaler( rf_path_enum_type path, rf_card_band_type band );
extern boolean ziflib_is_gps_sw_dc_cancellation_needed( rf_path_enum_type path );
extern rf_card_type rf_card;

/* DC Gain Data type */
typedef struct ziflib_dc_gain_data
{
   byte polarity;
   byte gain_q_1;
   byte gain_i_1;
   byte gain_q_0;
   byte gain_i_0;

} ziflib_dc_gain_data_type;
void ziflib_get_dc_gain( ziflib_dc_gain_data_type *gain_data, rf_path_enum_type path, rf_card_band_type band );

typedef struct ziflib_fg_offset_data
{
    byte acq_scaler;
    byte trc_scaler;
} ziflib_fg_offset_data_type;
void ziflib_get_fg_offset_scaler( ziflib_fg_offset_data_type *fg_offset_data, 
                                  rf_path_enum_type path, 
                                  rf_card_band_type band );


/*===========================================================================

FUNCTION rxf_set_band

DESCRIPTION
  This function sets band class info for Rxf. It does NOT actually update
  the RxFront block with new band class settings.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_set_band(rf_path_enum_type path,
                  rf_card_band_type band_class )
{

  if ( path >= RF_PATH_MAX )
  {
    /* Error checking */
    RF_ASSERT( path < RF_PATH_MAX );
    return;
  }
  else if ( band_class >= RF_MAX_BAND )
  {
    /* Error checking */
    RF_ASSERT( band_class < RF_MAX_BAND );
    return;
  }
  else
  {
    if( band_class < RXF_MAX_INDEX)
    {
    /* Ensure band is supported */
    if (rxf_band_index[band_class] == RXF_BAND_CLASS_NOT_AVAILABLE)
    {
      ERR("Band class %d not supported", band_class, 0, 0);
      return;
    }
  
    /* Grab index to band class parameters */
    rxf[path].band_class_index = rxf_band_index[band_class];
  
    /* Set band class */
    rxf[path].prev_band = rxf[path].band;
    rxf[path].band = band_class;
  
    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    *rxf_band_class_params[rxf[path].band_class_index].nv_ptr = 
        rf_digital_nv_table[path][band_class] ;
    #endif

    /* Setup band class NV */
    rxf[path].digital_nv_ptr = 
        *rxf_band_class_params[rxf[path].band_class_index].nv_ptr;
  }
    else
    {
      ERR("Band class %d not supported", band_class, 0, 0);
      return;
    }
  }

}



/*===========================================================================

FUNCTION rxf_set_mode_hdr

DESCRIPTION
  This function programs the RXF for HDR mode.

DEPENDENCIES
  *The current implementation REQUIRES that this function call occurs after 
  the rxf is first set up for the 1x mode appropriate for the band intended. 
  This will change in future versions of rxf.c

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_set_mode_hdr(rf_path_enum_type path, 
                      rf_card_band_type band)
{

  rxf_set_mode_cdma( path, band);
 
#ifndef RF_HAS_RXF_V2
#error code not present
#else
  /* Select that VGA Gain Value Used is for HDR */
  MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_MODE, RXF_RF_VGA_MODE_HDR_V, (int)path); 
#endif /* !RF_HAS_RXF_V2 */
  
#ifdef FEATURE_QUASAR_RF
#error code not present
#endif /* FEATURE_QUASAR_RF */

  /* Select AGC mode settings to select HDR HLC mode */
  MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_MODE_SEL), AGC_MODE, 
                 RX_AGCc_MODE_SEL_HDR_HLC_V, (int)path);

  rxf[path].mode = RXF_HDR_MODE;

}

/*===========================================================================

FUNCTION rxf_hdr_wakeup

DESCRIPTION
  This function programs the RXF for HDR mode after sleep, and turns on 
  the Rx Front clocks.  
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_hdr_wakeup
(
  rf_path_enum_type path
)
{
#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
  rxf_enable_rx_front_clocks_sc2x(path);

  /* Select that VGA Gain Value Used is for HDR */
  MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_MODE,RXF_RF_VGA_MODE_HDR_V, (int)path);
#endif /* !RF_HAS_RXF_V2 */
  
#ifdef FEATURE_QUASAR_RF
#error code not present
#endif /* FEATURE_QUASAR_RF */

  /* Select AGC mode settings to select HDR HLC mode */
  MSM_W_MASKED_i(RXF_CDMA(RX_AGCc_MODE_SEL), AGC_MODE, 
                 RX_AGCc_MODE_SEL_HDR_HLC_V, (int)path);

  rxf[path].mode = RXF_HDR_MODE;

}


/*===========================================================================

FUNCTION rxf_chain_init

DESCRIPTION
  This function initializes the  RX Front End Block for operation.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_chain_init( rf_path_enum_type path )
{
  
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* Disabling the SBI DACC updates from DACC Controller in RX Front*/
  /* for SC2X and QSC1100. Will Consider turning it ON on later depending upon */
  /* whether it is required or not */
  MSM_W_MASKED_i( RXF_SRCc_DACC_CTL, RXF_SBI_EN,
                  RXF_RF_DACC_CTL_SBI_DIS_V , (int)path );
#endif /* !RF_HAS_RXF_V2 */

  /* Initialize OFFSET Control for ZIF mode of operation.
    - Disable fg_accum_clr_en (after the DACC clear timer expires).
    - Disable cg_offset_accum_en, to disable periodic accumulation by
      CG Offset Loop Accumulator.
    - Enable fg_offset_accum_en,to enable periodic accumulation by
      FG Offset Loop Accumulator.
  */
  //Need to ask from RF Systems..Been used 0 for all MSMs
  MSM_W_MASKED_i(RXF_CDMA(RXFc_OFFSET_CTL), FG_ACCUM_CLR_EN,
                 RXF_RF_FG_ACCUM_CLR_DIS_V, (int)path);

  MSM_W_MASKED_i(RXF_CDMA(RXFc_OFFSET_CTL), FG_OFFSET_ACCUM_EN, 
                 RXF_RF_FG_OFFSET_ACCUM_ENA_V, (int)path);

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */

  MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_ACCUM_FREQ, 3, (int)path);
  //Need to ask RF Systems abou this register
  HWIO_OUTMI(RXF_SRCc_DACC_CTL, (int)path ,0xFFFFFFFF, 0);
  HWIO_OUTMI(RXF_SRCc_DACC_TRC_TIMER, (int)path,0xFFFFFFFF, 0);

#endif /* !RF_HAS_RXF_V2 */

#ifdef RF_HAS_RXF_DF2
  /* Used to remove static DC caused by truncation for digital filter.*/
  HWIO_OUTMI(RXF_CDMA(RXFc_DF2_DC_OFFSET), (int)path, HWIO_RMSK(RXF_CDMA(RXFc_DF2_DC_OFFSET)), 
             RXF_RF_DF2_DC_OFFSET_V);
#endif /* RF_HAS_RXF_DF2 */

  /* Set mode */
  rxf[path].mode = RXF_INACTIVE;

} /* rxf_chain_init() */



/*===========================================================================

FUNCTION rxf_init

DESCRIPTION
  This function initializes the RX Front End Block for operation.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_init(rf_path_enum_type path)
{

  /* Get NV Table pointers */
  rfnv_get_nv_data(&rxf_nv_all_ptr);

  #ifdef T_SC2X_PBS
#error code not present
  #endif

  /* Do not bypass the front end for ZIF digital IO testing */
  #if (defined(T_IO_CARD) && (!defined (FEATURE_PLT_BASEBAND_ZIF)))
#error code not present
  #endif /* (defined(T_IO_CARD) && (!defined (FEATURE_PLT_BASEBAND_ZIF) */


  rxf[0].clock_setup = CLKRGM_RX_CHAIN_0;
  rxf[1].clock_setup = CLKRGM_RX_CHAIN_1;

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* Program the default settings for RX Front for SC2X */

  MSM_W_MASKED_i(RXF_SRCc_CTL, USE_RTC, 1, (int)path);

  #ifndef RF_HAS_HW_QSC11X0
  MSM_W_MASKED_i(RXF_SRCc_CTL, NOTCH2_EN, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, NOTCH_EN, 0, (int)path);
  #endif

  #ifndef RF_HAS_HW_QSC11X0
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_IMC,0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_MIS_COMP, 0, (int)path);
  #else
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_IMC,1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_MIS_COMP, 1, (int)path);
  #endif

  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_DCC, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, EC_Y2_SCALE, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, GREY_MAP_EN, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BBF_BYPASS, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, ALT_SRC_SEL, 1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_SRC, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, CHIPX8_INV, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_RXF,0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, TEST_MODE, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_CORDIC, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, ROT_PHACC_CLR, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, ROT_TEST_MODE, 0, (int)path);     
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_DVGA,0, (int)path);  
  MSM_W_MASKED_i(RXF_SRCc_CTL, MICRO_GAIN_OVERRIDE_N, 1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, RXF_TEST_MODE, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, SPECTRAL_INVERSION, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, GRP_DELAY_ADJ, 0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, FS_CLK_EN, (uint32)1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, CXN_CLK_EN, 1,(int)path);
 
#endif /* !RF_HAS_RXF_V2 */
  /* --------------------------------------------------------------------- */
  /* Reset the (1-bit) RX Front End Registers                              */
  /* --------------------------------------------------------------------- */
  rxf_reset(path);

#ifdef FEATURE_QUASAR_RF
#error code not present
#else   /* JAGUAR */

#ifdef BBRX_DEBUG
#error code not present
#endif /*BBRX_DEBUG */
#endif /* FEATURE_QUASAR_RF */

  /* Initialize SBI Info Reg for ZIF mode of operation. */
  rflib_set_sbi_info_rxf(path, DB_RF_CDMA);

  /* Init chains */
  rxf_chain_init(path);

  #ifndef RF_HAS_NO_RTC_DEPENDENCY
#error code not present
  #endif /* ! RF_HAS_NO_RTC_DEPENDENCY */

} /* rxf_init() */


/*===========================================================================

FUNCTION RXF_PROGRAM_MODE_CDMA

DESCRIPTION
  This programs the RX front to one of the digital modes DB_RF_CDMA or
  DB_RF_PCN.  RX sample clocks are not initialized.  DC acquisition is not
  performed.

DEPENDENCIES
  rf_mode database item DB_RF_MODE must be either DB_RF_CDMA or DB_RF_PCN

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_program_mode_cdma(rf_path_enum_type path, 
                           rf_card_band_type band )
{
  int bc_index;

  /* Set band */
  rxf_set_band(path, band);

  bc_index = rxf[path].band_class_index;

  rxf[path].mode = RXF_CDMA_MODE;

  /* Switch to CDMA clocks */
  switch (path) 
  {
  case RF_PATH_0:
    clk_regime_config_cdma_clocks(CLKRGM_RX_CHAIN_0);
    break;

  case RF_PATH_1:
    clk_regime_config_cdma_clocks(CLKRGM_RX_CHAIN_1);
    break;

  default:
    ERR("Invalid PATH specified! %d", path,0,0);
    return;
  }   

  #ifdef RF_HAS_Y1Y2_DBG_BUS
#error code not present
  #endif /* RF_HAS_Y1Y2_DBG_BUS */
  
  #ifdef T_SC2X_PBS
#error code not present
  #endif /* T_SC2X_PBS */

  /* Reset RXF by first pulling reset pin high */
  MSM_W_i(RXF_CDMA(RXFc_RESET), RXF_RF_RESET_HIGH_V, (int)path);

  #ifndef RF_HAS_RXF_V2
#error code not present
  #endif

  /* Set ADIE to CDMA mode.*/
  /*lint -save -e534*/
#ifdef RF_HAS_MULTI_RX_CHAIN  
  switch (path) 
  {
  case RF_PATH_0:
    bbrx_set_mode( BBRX_CHAIN_0,rxf_band_class_params[bc_index].bbrx_mode );
    break;

  case RF_PATH_1:
    bbrx_set_mode( BBRX_CHAIN_1,rxf_band_class_params[bc_index].bbrx_mode );
    break;

  default:
    ERR("Invalid PATH specified! %d", path,0,0);
    return;
  }
#else /*RF_HAS_MULTI_RX_CHAIN*/
  bbrx_set_mode(rxf_band_class_params[bc_index].bbrx_mode);
#endif /* RF_HAS_MULTI_RX_CHAIN*/
  /*lint -restore */

  /* Load estimators and accumulators */
  rxf_load_accum_data(path);

  /* Set vga_gain_offset, compensated for the tuned frequency */
  rxf_set_vga_gain_offset(path, rf_chain_status[path].rf_freq_index);

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  HWIO_OUTMI(RXF_SRCc_DC_IOFFSET_ADJ , (int)path , 0xFFFFFFFF, 
             rxf_defaults.dc_ioffset_adj);
  HWIO_OUTMI(RXF_SRCc_DC_QOFFSET_ADJ , (int)path ,0xFFFFFFFF, 
             rxf_defaults.dc_qoffset_adj);
#endif /* !RF_HAS_RXF_V2 */
  
  /* Call appropriate rflib function */
  rxf_band_class_params[bc_index].rflib_init_func(path);

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /*
  * Initialize RXF_DC_CORR_FACTOR
  */

  /* RXF_SRCc_FG_IOFFSET Register */
  HWIO_OUTMI(RXF_SRCc_FG_IOFFSET , (int)path,0xFFFFFFFF, 0);

  /* RXF_SRCc_FG_QOFFSET Register */
  HWIO_OUTMI(RXF_SRCc_FG_QOFFSET , (int)path,0xFFFFFFFF, 0);
  
  /* RXF_SRCc_DACC_EST_THRESH Register */    
  HWIO_OUTMI(RXF_SRCc_DACC_EST_THRESH,(int)path ,0xFFFFFFFF, 0);
 
#endif /* !RF_HAS_RXF_V2 */

  /*
  Setup RXF_CTL register

  - Enable micro_gain_override
  - Enable digital_vga_sel
  - Enable zif_sel
  - Enable bypass_gps_mat_filt
  - Enable bypass_df2
  - Enable cordic
  - Disable async_samp_clk_sel
  - Set grp_delay_adj

  */
#ifdef T_IO_CARD_PANDA
#error code not present
#else /*!T_IO_CARD_PANDA */

  /* ZIF_SEL and ASYNC_SAMP_CLK were removed */
  MSM_W_MASKED_i(RXF_CDMA(RXFc_CTL), MICRO_GAIN_OVERRIDE_N, 
                 RXF_RF_MICRO_GAIN_OVERRIDE_N_ENA_V, (int)path);
  MSM_W_MASKED_i(RXF_CDMA(RXFc_CTL), DIGITAL_VGA_BYPASS, 
                 RXF_RF_DIGITAL_VGA_NON_BYPASS_V, (int)path);

#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* !RF_HAS_RXF_V2 */

  MSM_W_MASKED_i(RXF_CDMA(RXFc_CTL), BYPASS_CORDIC, RXF_RF_BYPASS_CORDIC_DIS_V, (int)path);

#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* !RF_HAS_RXF_V2 */

#ifndef RF_TODO_7600_ADK
#ifdef RF_HAS_RXF_DF2
  MSM_W_MASKED_i(RXF_CDMA(RXFc_CTL), BYPASS_DF2, RXF_RF_BYPASS_DF2_ENA_V, (int)path);
#endif /* RF_HAS_RXF_DF2 */
#endif

#endif /*!T_IO_CARD_PANDA */

#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
MSM_W_MASKED_i( RXF_SRCc_CTL, BYPASS_MIS_COMP, 
                rxf_defaults.bypass_mis_comp, (int)path ); 

  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_IMC, rxf_defaults.bypass_imc, (int)path);  
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_DCC, rxf_defaults.bypass_dcc, (int)path);

#ifdef T_RUMI_SC2X
#error code not present
#else
  MSM_W_MASKED_i(RXF_SRCc_CTL, GREY_MAP_EN, 0, (int)path);
#endif /* T_RUMI_SC2X */

  MSM_W_MASKED_i(RXF_SRCc_CTL, BBF_BYPASS, rxf_defaults.bypass_bbf, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_SRC, rxf_defaults.bypass_src, (int)path); 
  MSM_W_MASKED_i(RXF_SRCc_CTL, ALT_SRC_SEL, 1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_RXF, rxf_defaults.bypass_rxf, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, BYPASS_DVGA,rxf_defaults.bypass_dvga, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_CTL, MICRO_GAIN_OVERRIDE_N, rxf_defaults.micro_gain_override_n, (int)path);

#endif /* !RF_HAS_RXF_V2 */

  /* Set grp_delay_adj */
  rxf_set_group_delay( path, band );

  /*
  Initialize DVGA Control registers

  - Set digital_vga_gain_sel, gain value used by DVGA.
  - Set vga_gain_scale, 4-bit positive value for VGA gain scale.
  */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
   /* Select that VGA Gain Value used is for CDMA */
   MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_MODE, RXF_RF_VGA_MODE_CDMA_V, (int)path); 
   MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_ACCUM_FREQ, 3, (int)path);

   #ifdef RF_HAS_HW_QSC11X0
   MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_ACQ_OFFSET_SCALER, 0xC, (int)path);
   MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_TRC_OFFSET_SCALER, 6, (int)path);
   #else
   MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_ACQ_OFFSET_SCALER, 0xA, (int)path);
   MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_TRC_OFFSET_SCALER, 4, (int)path);
   #endif

#endif /* !RF_HAS_RXF_V2 */

  /*
  Initialize Estimator parameters

  - Set dacc_mult_prescaler.
  - Set est_accum_freq.
  - Set est_acq_offset_scaler.
  - Set est_trc_offset_scaler.
  - Disable micro_lna_range_overide.
  */

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  HWIO_OUTMI(RXF_SRCc_DACC_CTL, (int)path,0xFFFFFFFF, rxf_defaults.dacc_ctl);
#endif  /* !RF_HAS_RXF_V2 */

#if defined(T_IO_CARD_PANDA)
#error code not present
#endif /* T_IO_CARD_PANDA */

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* Initialize DACC Acquisition Timer */
  HWIO_OUTMI(RXF_SRCc_DACC_ACQ_TIMER, (int)path,0xFFFFFFFF, 0);

  /* Initialize DACC Clear Timer */
  HWIO_OUTMI(RXF_SRCc_DACC_CLR_TIMER, (int)path,0xFFFFFFFF, 0);
#endif /* !RF_HAS_RXF_V2 */

  /* Initialize DACC Gain Multiplier */
  {
    ziflib_dc_gain_data_type dc_gain_data;
    
    ziflib_get_dc_gain( &dc_gain_data, path, band );
    
#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* RF_HAS_RXF_V2 */
  }

  /* AGC settings selects 1x/GPS mode for AGC -- move to rfagc.c???*/
  HWIO_OUTMI(RXF_CDMA(RX_AGCc_MODE_SEL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_MODE_SEL)), 
             RX_AGCc_MODE_SEL_1X_V);
  
  /* Freeze AGC to prevent large DC errors from disrupting the AGC Loop */
  rf_set_agc_dc_loop_gain(path, FREEZE_AGC_DC_LOOP_GAIN );

  /* Set Rxf Reset Low to complete the reset procedure */
  MSM_W_i(RXF_CDMA(RXFc_RESET), RXF_RF_RESET_LOW_V, (int)path);       

#ifdef RF_HAS_RXF_V2_DEBUG
#error code not present
#endif
}

/*===========================================================================

FUNCTION rxf_set_mode_cdma

DESCRIPTION
  This function initializes the cdma specific parameters. If FALSE is passed
  as the parameter the RX Front clock regime remain disabled and a DC
  acquisition is not performed.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_set_mode_cdma(rf_path_enum_type path, 
                       rf_card_band_type band)
{

  /* Enable the AGC and DC cancellation blocks */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  rxf_enable_rx_front_clocks_sc2x(path);
#endif  /* !RF_HAS_RXF_V2 */

  rxf_program_mode_cdma(path, band);

#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* !RF_HAS_RXF_V2 */

  /* Wait for RXF_CG_LNA_ACQ_TIME + 30 u sec */
  clk_busy_wait( RXF_ACQ_WAIT_TIME_CDMA ); /*lint !e834 */ //clk_busy_wait has - followed by -

}


  #ifdef FEATURE_DFM
#error code not present
  #endif /*FEATURE_DFM*/


#if (defined(RF_HAS_GPS))
#error code not present
#endif // RF_HAS_GPS


/*===========================================================================

FUNCTION rxf_clear_accum

DESCRIPTION
  This function clears the RX Front End DACC Estimators.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_clear_accum( rf_path_enum_type path )
{
#ifndef RF_HAS_RXF_V2
#error code not present
#else
  MSM_W_MASKED_i(RXF_SRCc_DACC_EST_IOFFSET,EST_I_DC_OFFSET, RXF_RF_DACC_EST_IOFFSET_CLR_V, 
                 (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_EST_QOFFSET,EST_Q_DC_OFFSET, RXF_RF_DACC_EST_QOFFSET_CLR_V, 
                 (int)path);
#endif /* !RF_HAS_RXF_V2 */
}
/*===========================================================================

FUNCTION  rxf_save_dc_offset

DESCRIPTION
  This function saves CDMA DC offset before tuning away to GPS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_save_dc_offset(rf_path_enum_type path )
{
  /* Remember the DC Offsets */
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  rxf[path].dacc_status.i_accum0 = HWIO_INI(RXF_SRCc_DACC_IACCUM0, (int)path);/*lint !e734 */
  rxf[path].dacc_status.q_accum0 = HWIO_INI(RXF_SRCc_DACC_QACCUM0, (int)path);/*lint !e734 */

  rxf[path].dacc_status.i_accum1 = HWIO_INI(RXF_SRCc_DACC_IACCUM1, (int)path);/*lint !e734 */
  rxf[path].dacc_status.q_accum1 = HWIO_INI(RXF_SRCc_DACC_QACCUM1, (int)path);/*lint !e734 */

  rxf[path].dacc_status.i_accum2 = HWIO_INI(RXF_SRCc_DACC_IACCUM2, (int)path);/*lint !e734 */
  rxf[path].dacc_status.q_accum2 = HWIO_INI(RXF_SRCc_DACC_QACCUM2, (int)path);/*lint !e734 */

  rxf[path].dacc_status.i_accum3 = HWIO_INI(RXF_SRCc_DACC_IACCUM3, (int)path);/*lint !e734 */
  rxf[path].dacc_status.q_accum3 = HWIO_INI(RXF_SRCc_DACC_QACCUM3, (int)path);/*lint !e734 */

  rxf[path].dacc_status.i_accum4 = HWIO_INI(RXF_SRCc_DACC_IACCUM4, (int)path);/*lint !e734 */
  rxf[path].dacc_status.q_accum4 = HWIO_INI(RXF_SRCc_DACC_QACCUM4, (int)path);/*lint !e734 */

  rxf[path].dacc_status.i_est =  HWIO_INI(RXF_SRCc_DACC_EST_IOFFSET, (int)path);
  rxf[path].dacc_status.q_est =  HWIO_INI(RXF_SRCc_DACC_EST_QOFFSET, (int)path);

  rxf[path].dacc_status.fg_i_offset = HWIO_INI(RXF_SRCc_FG_IOFFSET, (int)path);
  rxf[path].dacc_status.fg_q_offset = HWIO_INI(RXF_SRCc_FG_QOFFSET, (int)path);
#endif /* !RF_HAS_RXF_V2 */
}

/*===========================================================================

FUNCTION  rxf_restore_dc_offset

DESCRIPTION
  This function restores all the CDMA DC Offset Block after tuning back from
  GPS visit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_restore_dc_offset( rf_path_enum_type path )
{
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
   /* For that let's first Disable DACC */
  MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL),MICRO_DACC_EN,RXF_RF_MICRO_DACC_DIS_V, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM0,DACC_I_ACCUM0,rxf[path].dacc_status.i_accum0, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM0,DACC_Q_ACCUM0,rxf[path].dacc_status.q_accum0, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM1,DACC_I_ACCUM1,rxf[path].dacc_status.i_accum1, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM1,DACC_Q_ACCUM1,rxf[path].dacc_status.q_accum1, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM2,DACC_I_ACCUM2,rxf[path].dacc_status.i_accum2, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM2,DACC_Q_ACCUM2,rxf[path].dacc_status.q_accum2, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM3,DACC_I_ACCUM3,rxf[path].dacc_status.i_accum3, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM3,DACC_Q_ACCUM3,rxf[path].dacc_status.q_accum3, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM4,DACC_I_ACCUM4,rxf[path].dacc_status.i_accum4, (int)path);
  MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM4,DACC_Q_ACCUM4,rxf[path].dacc_status.q_accum4, (int)path);

  MSM_W_MASKED_i(RXF_SRCc_DACC_EST_IOFFSET,EST_I_DC_OFFSET, 
                 rxf[path].dacc_status.fg_i_offset, (int)path);/*lint !e737 */
  MSM_W_MASKED_i(RXF_SRCc_DACC_EST_QOFFSET,EST_Q_DC_OFFSET, 
                 rxf[path].dacc_status.fg_q_offset, (int)path);/*lint !e737 */

  MSM_W_MASKED_i(RXF_SRCc_FG_IOFFSET,FG_I_OFFSET , 
                 rxf[path].dacc_status.fg_i_offset, (int)path);/*lint !e737 */
  MSM_W_MASKED_i(RXF_SRCc_FG_QOFFSET, FG_Q_OFFSET, 
                 rxf[path].dacc_status.fg_q_offset, (int)path);/*lint !e737 */
#endif /* !RF_HAS_RXF_V2 */

  /* Issue an update on SBI */
  MSM_W_MASKED_i(RXF_CDMA(MICROc_DACC_UPDATE), DATA, 
                 RXF_RF_MICRO_DACC_UPDATE_V, (int)path);

  /* Wait for RXF_CG_LNA_ACQ_TIME + 30 u sec */
  clk_busy_wait( RXF_ACQ_WAIT_TIME_CDMA ); /*lint !e834 */ //clk_busy_wait has - followed by -

  /* And finally enable the DACC */
  MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL),MICRO_DACC_EN,RXF_RF_MICRO_DACC_ENA_V, (int)path);

  /* Allow for DACC settling( also includes time for SBI to complete update)*/
  clk_busy_wait( RXF_DACC_TRK_WAIT_TIME );  /*lint !e834 */ //clk_busy_wait has - followed by - // 100 us
}

/*===========================================================================

FUNCTION rxf_disable_dacc

DESCRIPTION
  This function disables the DACC

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_disable_dacc( rf_path_enum_type path )
{
  /* For that let's first Disable DACC */
  MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL),MICRO_DACC_EN,RXF_RF_MICRO_DACC_DIS_V,
                 (int)path);
}

/*===========================================================================

FUNCTION rxf_save_accum_data

DESCRIPTION
  This function is called when a mode change is initiated. The values of certain
  coefficients and offsets are saved in the local structure. These values are
  eventually written to the NV at power down.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_save_accum_data( rf_path_enum_type path)
{
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode = rflib_get_power_mode( path );
#endif /* RF_HAS_POWER_MODE_LIB */

  switch (rxf[path].mode)
  {
  case RXF_CDMA_MODE:
  case RXF_HDR_MODE:
#if (defined(RF_HAS_GPS))
#error code not present
#endif
#ifdef  RF_HAS_POWER_MODE_LIB
#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum0[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_IACCUM0, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum1[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_IACCUM1, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum2[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_IACCUM2, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum3[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_IACCUM3, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum4[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_IACCUM4, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum0[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_QACCUM0, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum1[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_QACCUM1, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum2[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_QACCUM2, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum3[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_QACCUM3, (int)path);/*lint !e734 */
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum4[path][power_mode]    =
          HWIO_INI(RXF_SRCc_DACC_QACCUM4, (int)path);/*lint !e734 */
#endif /* !RF_HAS_RXF_V2 */
#else  /* RF_HAS_POWER_MODE_LIB */
#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum0[path]     =
          HWIO_INI(RXF_SRCc_DACC_IACCUM0, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum1[path]     =
         HWIO_INI(RXF_SRCc_DACC_IACCUM1, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum2[path]     =
          HWIO_INI(RXF_SRCc_DACC_IACCUM2, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum3[path]     =
          HWIO_INI(RXF_SRCc_DACC_IACCUM3, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum4[path]     =
          HWIO_INI(RXF_SRCc_DACC_IACCUM4, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum0[path]     =
          HWIO_INI(RXF_SRCc_DACC_QACCUM0, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum1[path]     =
          HWIO_INI(RXF_SRCc_DACC_QACCUM1, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum2[path]     =
          HWIO_INI(RXF_SRCc_DACC_QACCUM2, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum3[path]     =
          HWIO_INI(RXF_SRCc_DACC_QACCUM3, (int)path);
    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum4[path]     =
          HWIO_INI(RXF_SRCc_DACC_QACCUM4, (int)path);
#endif /* !RF_HAS_RXF_V2 */
#endif /* RF_HAS_POWER_MODE_LIB */
    break;

  default:
    MSG_MED("Unknown RF Mode in rxf_save_accum_data()", 0, 0, 0);
    break;
  }
}
/*============================================================================

FUNCTION RXF_SET_VGA_GAIN_OFFSET

DESCRIPTION
  This function will load in vga gain offsets compensated for the tuned
  frequency.

DEPENDENCIES
  rf_freq_index must have been set before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void rxf_set_vga_gain_offset( rf_path_enum_type path, 
                              byte freq_index )/*lint -esym(715, freq_index) unused */
{
  dword vga_gain_offset;

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO
byte digital_filter_gain;
#endif

#ifndef RF_HAS_LINEAR_INTERPOLATION  
  byte index; 

  index = (freq_index>0 && freq_index<16) ? freq_index:0;
#endif

#ifdef RF_HAS_POWER_MODE_LIB
rflib_power_mode_type power_mode = RFLIB_NORMAL_POWER_MODE;
power_mode = rflib_get_power_mode( path );
#endif /* RF_HAS_POWER_MODE_LIB */

/* This block of code is not required since the mode will not be set to INACTIVE during rxf sleep */ 
#if 0
 if(path == RF_PATH_0)
 {
   MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"The path is %d, the mode is %d", path,rxf[path].mode);   
   rxf[path].mode= RXF_CDMA_MODE;
 }

 if((path == RF_PATH_1) && (rxf[path].mode!=RXF_GPS_MODE))
 {
   MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"The path is %d, the mode is %d", path,rxf[path].mode);   
   rxf[path].mode= RXF_CDMA_MODE;
 }
#endif /* 0 */
  switch (rxf[path].mode)
  {
  case RXF_POWERUP:
  case RXF_CDMA_MODE:
  case RXF_HDR_MODE:

    /* CDMA specific processing, freq comp value is scaled by 12 */
  
#ifdef RF_HAS_LINEAR_INTERPOLATION
  #ifdef RF_HAS_POWER_MODE_LIB
    vga_gain_offset = rf_freq_comp.i_vga_gain_offset_vs_freq[path][power_mode];/*lint !e732 */
  #else
    vga_gain_offset = rf_freq_comp.i_vga_gain_offset_vs_freq[path];
  #endif /* RF_HAS_POWER_MODE_LIB */
#else
    vga_gain_offset = 
        (dword)(rxf[path].digital_nv_ptr->rfnv_vga_gain_offset[path] 
      + (rxf[path].digital_nv_ptr->rfnv_vga_gain_offset_vs_freq[path][ index ]
      * 12));
#endif

#ifdef RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO

    digital_filter_gain = 
      rflib_get_digital_filter_gain(rf_adc_clk_status.samp_freq_bin[path],path,
                                    rf_adc_clk_status.mode[path],rxf[path].band,
                                    power_mode);                    

    vga_gain_offset += digital_filter_gain;

#endif /* RF_HAS_ADC_SAMP_FREQ_DERIVED_FROM_VCO */

    /* Set vga_gain_offset, compensated for the tuned frequency. */
#ifndef RF_HAS_RXF_V2
#error code not present
#else
    #ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
    #else
    MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_GAIN_OFFSET, vga_gain_offset,(int)path);
    #endif
    MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, ADD_EN, 0, 0);    
    MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_MODE, RXF_RF_VGA_MODE_CDMA_V , 0);
#endif /* !RF_HAS_RXF_V2 */

    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"The path is %d, the vga_gain_offset is %d", 
          path,vga_gain_offset);
    break;
  
#if defined(RF_HAS_EGPS)
#error code not present
#endif /* RF_HAS_EGPS */

  case RXF_DFM_MODE:
    /* FM not supported, fall through */

  default:
    
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"The path is %d, the mode is %d", path,rxf[path].mode);
    ERR("Unknown RF Mode in rxf_load_vga_gain_offset()", 0, 0, 0);
  }

} /* rxf_load_vga_gain_offset */

/*==========================================================================
FUNCTION rxf_load_accum_data


DESCRIPTION
  This function is called at power up.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_load_accum_data( rf_path_enum_type path )
{
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode = rflib_get_power_mode( path );
#endif /* RF_HAS_POWER_MODE_LIB */

  switch (rxf[path].mode)
  {
  case RXF_CDMA_MODE:
  case RXF_HDR_MODE:
#if (defined(RF_HAS_GPS))
#error code not present
#endif

    /* CDMA specific processing */

#ifdef  RF_HAS_POWER_MODE_LIB
#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
    /* Initialize DACC_IACCUM0 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM0,DACC_I_ACCUM0,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum0[path][power_mode], (int)path);

    /* Initialize DACC_IACCUM1 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM1,DACC_I_ACCUM1,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum1[path][power_mode], (int)path);

    /* Initialize DACC_IACCUM2 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM2,DACC_I_ACCUM2,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum2[path][power_mode], (int)path);

    /* Initialize DACC_IACCUM3 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM3,DACC_I_ACCUM3,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum3[path][power_mode], (int)path);

    /* Initialize DACC_IACCUM4 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM4,DACC_I_ACCUM4,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum4[path][power_mode], (int)path);

    /* Initialize DACC_QACCUM0 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM0,DACC_Q_ACCUM0,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum0[path][power_mode], (int)path);

    /* Initialize DACC_QACCUM1 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM1,DACC_Q_ACCUM1,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum1[path][power_mode], (int)path);

    /* Initialize DACC_QACCUM2 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM2,DACC_Q_ACCUM2,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum2[path][power_mode], (int)path);

    /* Initialize DACC_QACCUM3 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM3,DACC_Q_ACCUM3,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum3[path][power_mode], (int)path);

    /* Initialize DACC_QACCUM4 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM4,DACC_Q_ACCUM4,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum4[path][power_mode], (int)path);
#endif  /* RF_HAS_RXF_V2 */
#else   /* RF_HAS_POWER_MODE_LIB */
#ifndef RF_HAS_RXF_V2
#error code not present
#else   /* RF_HAS_RXF_V2 */
    /* Initialize DACC_IACCUM0 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM0,DACC_I_ACCUM0,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum0[path], (int)path);

    /* Initialize DACC_IACCUM1 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM1,DACC_I_ACCUM1,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum1[path], (int)path);

    /* Initialize DACC_IACCUM2 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM2,DACC_I_ACCUM2,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum2[path], (int)path);

    /* Initialize DACC_IACCUM3 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM3,DACC_I_ACCUM3,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum3[path], (int)path);

    /* Initialize DACC_IACCUM4 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_IACCUM4,DACC_I_ACCUM4,
                    rxf[path].digital_nv_ptr->rfnv_dacc_iaccum4[path], (int)path);

    /* Initialize DACC_QACCUM0 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM0,DACC_Q_ACCUM0,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum0[path], (int)path);

    /* Initialize DACC_QACCUM1 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM1,DACC_Q_ACCUM1,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum1[path], (int)path);

    /* Initialize DACC_QACCUM2 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM2,DACC_Q_ACCUM2,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum2[path], (int)path);

    /* Initialize DACC_QACCUM3 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM3,DACC_Q_ACCUM3,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum3[path], (int)path);

    /* Initialize DACC_QACCUM4 */
    MSM_W_MASKED_i(RXF_SRCc_DACC_QACCUM4,DACC_Q_ACCUM4,
                    rxf[path].digital_nv_ptr->rfnv_dacc_qaccum4[path], (int)path);
#endif /* !RF_HAS_RXF_V2 */
#endif /* RF_HAS_POWER_MODE_LIB */
    break;

#ifdef FEATURE_DFM
#error code not present
#endif /*FEATURE_DFM*/

  default:
    MSG_MED("Unknown RF Mode in rxf_load_accum_data()", 0, 0, 0);
    break;
  }
}

/*===========================================================================

FUNCTION rxf_reset

DESCRIPTION 
  This function resets the  RX Front End Block by writing to the RXF_RESET.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_reset(rf_path_enum_type path)
{
 
  /* --------------------------------------------------------------------- */
  /* Reset the (1-bit) RX Front End Registers                              */
  /* --------------------------------------------------------------------- */
  MSM_W_i(RXF_CDMA(RXFc_RESET), RXF_RF_RESET_HIGH_V, (int)path);

  /* Absolutely required since reset pulse should be at least 
     as wide as propagation time through rxf */
  clk_busy_wait( RXF_RESET_WAIT_US ); /*lint !e834 */ //clk_busy_wait has - followed by -

  MSM_W_i(RXF_CDMA(RXFc_RESET), RXF_RF_RESET_LOW_V, (int)path);

}



/*===========================================================================

FUNCTION rxf_sleep

DESCRIPTION
    This function puts rxf in the sleep mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_sleep(rf_path_enum_type path)
{
  /* Disable the AGC and DC cancellation blocks (turn off clocks) */

#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  rxf_disable_rx_front_clocks_sc2x(path);
#endif  /* !RF_HAS_RXF_V2 */
  
/* 
The rxf registers are preserved when the clocks are turned off  
during sleep. We therefore don't need to change the mode to INACTIVE 
*/
#if 0
  /* Track mode */
  rxf[path].mode = RXF_INACTIVE;
#endif  
}


/*=========================================================================

FUNCTION rxf_cdma_wakeup

DESCRIPTION
    This function is first function to called when waking up from cdma
    sleep. The DACC is disabled to prevent large DC errors from
    propogating from the RF into the baseband analog receiver. The DC
    accumulators are cleared and a DACC update is sent to the RF.
    The DACC is left in the disabled state.  Once RF is stabilized
    rxf_cdma_wakeup_dc_acquire() must be called to enable the DACC and
    do a DC acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  DACC is disabled.
=========================================================================*/
void rxf_cdma_wakeup( rf_path_enum_type path )
{
#ifndef RF_HAS_RXF_V2
#error code not present
#else /* RF_HAS_RXF_V2 */
  /* Select that VGA Gain Value used is for CDMA */
  MSM_W_MASKED_i(RXF_SRCc_DVGA_PARAMS, VGA_MODE,RXF_RF_VGA_MODE_CDMA_V , (int)path); 
#endif /* !RF_HAS_RXF_V2 */

  /* AGC settings selects 1x/GPS mode for AGC */
  HWIO_OUTMI(RXF_CDMA(RX_AGCc_MODE_SEL), (int)path, HWIO_RMSK(RXF_CDMA(RX_AGCc_MODE_SEL)), 
             RX_AGCc_MODE_SEL_1X_V);

#ifndef RF_HAS_RXF_V2
#error code not present
#else  /* RF_HAS_RXF_V2 */
  rxf_enable_rx_front_clocks_sc2x(path);
#endif /* !RF_HAS_RXF_V2 */

#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* !RF_HAS_RXF_V2 */

  rxf[path].mode = RXF_CDMA_MODE;
}



/*=========================================================================

FUNCTION rxf_cdma_wakeup_dc_acquire

DESCRIPTION
    This function is called when waking up from cdma sleep. It must be called
    after rxf_cdma_wakeup() has been called and the RF has stabilized. The
    DACC is re-enabled and a DC acquisition is performed.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void rxf_cdma_wakeup_dc_acquire(rf_path_enum_type path)
{
#ifndef RF_HAS_RXF_V2
#error code not present
#endif

  /* Freeze AGC to prevent large DC errors from disrupting the AGC Loop */
  rf_set_agc_dc_loop_gain(path, FREEZE_AGC_DC_LOOP_GAIN );

#ifndef RF_HAS_RXF_V2
#error code not present
#endif

}

/*===========================================================================

FUNCTION RXF_SET_RX_POLARITY

DESCRIPTION
  Set spectral polarity for specified RX path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_set_rx_polarity(rf_path_enum_type path, enc_rx_polarity_type rx_polarity )
{
  #ifndef T_IO_CARD
  HWIO_OUTMI( RXF_CDMA(RXFc_CTL), (int)path, ENC_RX_POL_M, (int)rx_polarity );
  #else
#error code not present
  #endif
}

/*===========================================================================

FUNCTION RXF_SET_DC_GAIN

DESCRIPTION
  Program the RX front DC loop gains for the specified band

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_set_dc_gain( rf_path_enum_type path, rf_card_band_type band )
{
#ifndef RF_HAS_RXF_V2
#error code not present
#endif  /* RF_HAS_RXF_V2 */
} /* rxf_set_dc_gain */


#ifdef RF_HAS_NO_RTC_DEPENDENCY
/*===========================================================================

FUNCTION RXF_BYPASS_RTC_CTL_PATH_ON_OFF

DESCRIPTION
  Enable or disable the Bypass RTC bit on a specific rx path

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_bypass_rtc_ctl_path_on_off( rf_path_enum_type path, boolean rxf_bypass_rtc_on_off )
{
  uint16 rxf_bypass_rtc_sync_v;
  if( rxf_bypass_rtc_on_off == TRUE )
  {
    rxf_bypass_rtc_sync_v = 0x1;
  }
  else
  {
    rxf_bypass_rtc_sync_v = 0x0;
  }
#ifndef RF_HAS_RXF_V2
#error code not present
#endif
}

#endif /* RF_HAS_NO_RTC_DEPENDENCY */

/*===========================================================================

FUNCTION RXF_SET_GROUP_DELAY

DESCRIPTION
  Program the RX front group delay based on chipset characteristics

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxf_set_group_delay( rf_path_enum_type path, rf_card_band_type band )/*lint -esym(715, band) unused */
{
#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* !RF_HAS_RXF_V2 */
} /* rxf_set_group_delay */


/*===========================================================================

FUNCTION RXF_ENABLE_DACC

DESCRIPTION
  This function enables the DACC

DEPENDENCIES
  RX Front clocks must be on.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_enable_dacc( rf_path_enum_type path )
{
  MSM_W_MASKED_i(RXF_CDMA(RXFc_DACC_CTL),MICRO_DACC_EN,RXF_RF_MICRO_DACC_ENA_V, path);
}

/*===========================================================================

FUNCTION rxf_is_dacc_disabled

DESCRIPTION
  This function checks if DACC is disabled. 
      
DEPENDENCIES
  RX Front clocks must be on.
 
RETURN VALUE
  TRUE - if DACC is disabled
  FALSE - if DACC is enabled
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean rxf_is_dacc_disabled( rf_path_enum_type path )
{    
    /* Check if DACC is disabled */
#ifndef RF_HAS_RXF_V2
#error code not present
#else
    return (( HWIO_RXF_SRCc_DACC_CTL_shadow[path]& HWIO_RXF_SRCc_DACC_CTL_MICRO_DACC_EN_BMSK ) 
                    >> HWIO_RXF_SRCc_DACC_CTL_MICRO_DACC_EN_SHFT ) ? FALSE : TRUE;
#endif /* !RF_HAS_RXF_V2 */
}

#ifdef RF_HAS_RXF_V2
/*===========================================================================

FUNCTION rxf_enable_rx_front_clocks

DESCRIPTION
  This function turns ON the Sampling and chipxN Clocks in the RX Front
  for SC2X
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_enable_rx_front_clocks_sc2x(rf_path_enum_type path)
{

  switch (path) 
  {
  case RF_PATH_0:
    clk_regime_rxf_request(CLKRGM_RXF_RF_0, CLKRGM_RXF_ENABLE);
    break;

  case RF_PATH_1:
    clk_regime_rxf_request(CLKRGM_RXF_RF_1, CLKRGM_RXF_ENABLE);
    break;

  default:
    ERR("Invalid PATH specified! %d", path,0,0);
    return;
  }
 
}

/*===========================================================================

FUNCTION rxf_disable_rx_front_clocks

DESCRIPTION
  This function turns OFF the Sampling and chipxN Clocks in the RX Front
  for SC2X.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_disable_rx_front_clocks_sc2x(rf_path_enum_type path)
{

  switch (path) 
  {
  case RF_PATH_0:
    clk_regime_rxf_request(CLKRGM_RXF_RF_0, CLKRGM_RXF_DISABLE);
    break;

  case RF_PATH_1:
    clk_regime_rxf_request(CLKRGM_RXF_RF_1, CLKRGM_RXF_DISABLE);
    break;

  default:
    ERR("Invalid PATH specified! %d", path,0,0);
    return;
  }

}

/*===========================================================================

FUNCTION RF_FREEZE_FG_DC_LOOP_ACCUM

DESCRIPTION
  This function disables the Fine Grained DC Loop Accum 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void rf_freeze_fg_dc_loop_accum(rf_path_enum_type path)
{
  MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_OFFSET_ACCUM_EN, RF_FG_DC_ACCUM_DISABLE_VAL, (int)path );
}

/*===========================================================================

FUNCTION RF_FREEZE_FG_DC_LOOP_ACCUM

DESCRIPTION
  This function enables the Fine Grained DC Loop Accum 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void rf_unfreeze_fg_dc_loop_accum(rf_path_enum_type path)
{
  MSM_W_MASKED_i(RXF_SRCc_OFFSET_CTL, FG_OFFSET_ACCUM_EN, RF_FG_DC_ACCUM_ENABLE_VAL, (int)path );
}

#endif /*RF_HAS_RXF_V2*/

#ifdef RF_HAS_RXF_V2_DEBUG
#error code not present
#endif /* RF_HAS_RXF_V2_DEBUG */

#ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
#endif /* RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL */

#ifdef RF_HAS_SUPPORT_FOR_RICO_BRINGUP
#error code not present
#endif
#endif /* !FEATURE_GSM_PLT */


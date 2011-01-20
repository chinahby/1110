/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        R F  C a r d  D r i v e r

GENERAL DESCRIPTION
  This is the SURF RF card driver.  Functions specific to the RF card such as
  initializing the card and tuning the synthesizer are performed from this
  module.  The interface provided by this module is INTERNAL to RF unit only.

EXTERNALIZED FUNCTIONS

  rfc_init_card
    This function initializes the synthesizer to the RF configuration.  It also
    performs other necessary intializations for the RF card.  And it test-
    tunes the synthesizer to a low and high channel of the appropriate band.

  rfc_init_card_cdma
    Initialize the RF card and its synthesizer for CDMA mode.

  rfc_init_card_pcs
    Initialize the RF card and its synthesizer for PCS mode.

  rfc_init_card_analog
    Initialize the RF card and its synthesizer for FM mode.
  
  rfc_init_card_gps
      Initialize the RF card and its synthesizer for GPS mode.
  
  rfc_tune_synth
    Tune the synthesizer to a specific channel number.

  rfc_retune_synth_for_rxtx
    Retune the synthesizer when transmit path is enabled.

  rfc_retune_synth_for_rx
    Retune the synthesizer when transmit path is disabled.

  rfc_chan_offset
    Calculate the offset from the channel with the lowest frequency.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The function rfc_init_card() must be called to initialize the RF card before
  any of rfc_init_card_cdma(), rfc_init_card_pcs(), and rfc_init_card_analog() 
  is called.

  rfc_init_card_cdma(), rfc_init_card_pcs(), or rfc_init_card_analog() must be
  called to initialize the RF card and synthesizer to the dedicated mode before
  rfc_tune_synth() is called.

  rfc_tune_synth() must be called before rfc_retune_synth_for_rxtx() or 
  rfc_retune_synth_for_rx().

  rfc_chan_offset() can be called at any time.

Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008,2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfcs.c#6 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/05/09   vb      Added support for RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
10/09/08   sr      Klocwork Fixes
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
04/16/08   vm      Added support for BC10
11/28/07   ems     Added support for QSC1100
10/04/07   Vish    Added support for initializing card specific RFConfig RX LNA
                   database using rfc_init_rfconfig_rx_lna().
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
08/09/07   sar     Added missing break in function rfc_set_card().
07/12/07   vm      Added new functional interface for rf cards to fix
                   the spectral inversion dependency on device
07/11/07   rmd/sar Fixed typo, replaced RF_HAS_TRF6285 with RF_HAS_HW_TRF6285.
07/11/07   rmd/sar Added support for the RTR6285.
06/06/07   ra      Remove card specific check from gps init 
06/03/07   hkk     1x, DO and temp waveform compensation
06/01/07   aak     Removal of JIDZRF card support
03/31/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
03/30/07   bmg     Added rfc_uses_internal_hdet() function for RTR6500 and
                     other chips that have built-in HDET
                   Created an rfc_ wrapper function rfc_get_hdet_adc_channel()
                   Warning fix
03/25/07   dyc     Remove RF_HAS_DEDICATED_RF_CARD implementation
03/19/07   dyc     Updated with featurization for all RF Card types.
                   Added QSC60x5.
03/14/07   sar     Featurized TRF6500 initialization.
03/14/07   rmd     Added Radio Configuration and temperature compensation for
                   HDET.
03/09/07   bmg     Added support for RTR6500
03/03/07   dyc     Added support for QSC60x5
03/02/07   sar     Removed compiler warnings.
02/15/07   sar     Added support for DZRF6285.
12/12/06   hkk     Added support for D-450
10/31/06   ycl     Include the correct rficap file and add support for WDZRF.
10/13/06   aak     Removal of support for FZRF
09/09/06   hkk     Removal of support for GZRF, KPZRF and CZRF
06/12/06   hkk     Added support for DZRFBC14
06/06/06   hkk     Removed support for ZRF6185.
03/30/15   zg/sar  Added support for ZRF6185.
01/18/06   sar     Added case, DB_RF_GPS in rfc_tune_synth function to not
                   error fatal when call in GPS mode. Needed for GPS fix.
01/16/06   rmd     Updated comments for bc14_band_sel_setting[].
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
01/13/06   lp      Added SSBI support for DZRF6500/JTDZRF6500.                   
01/11/06   et      LINT
01/10/06   sar     Eliminated lint errors.
12/07/05   sar     Eliminated lint errors.
11/14/05   rmd     * Updated rfc_tune_synth() to support BC14.
                   * Added support to BC11.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
10/17/05   sar     Changed return type for rfc_get_sub_class() to uint32.
10/17/05   sar     Updated comments for rfc_get_sub_class.
10/07/05   hkk     Included rfc_jtdzrf6500.h to eliminate compile warnings.
09/15/05   dyc     Add rfc_supports_opt_wu to rf card type intialization.
08/29/05   ra      Added support for JTDZRF
07/18/05   hkk     Added support for new card file
07/18/05   ra      removed dmod.h
05/12/05   sar     Added support for Band Subclass and Antenna Attributes.
03/17/05   dyc     Compiler warning fix.
03/16/05   jac     Changed all band parameters in rfc_init_card_gps() to RF_GPS_BAND.
03/04/05   dbc     Added calls to rflib_rfr_use_external_vco() in rfc_init_card_pcs() 
02/17/05   zg      Fixed GPS band selection in rfc_init_card_gps.
02/17/05   bmg     Added calls to rfc_uses_external_vco() for PCS mode as well
02/14/05   bmg     Added calls to new RFC function rfc_uses_external_vco.
01/13/05   sar     Added function rfc_manage_vregs().
11/16/04   ar/dyc  Support for FTM undefined.
11/08/04   ans     Modified call of rfr_use_external_vco to rflib_rfr_use_external_vco.
                   New API added to rflib for using external VCO. rflib_rfr_use_external_vco() 
                   should be called instead of rfr_use_external_vco() to turn on external VCO.
11/02/04   sar     Removed errorneous calls to rf_card.rfc_select_primary_vco()
                   from rfc_set_mode_cdma() and rfc_set_mode_pcs().
10/08/04   sar     Removed function rfc_copy_nv_items().
10/04/04   zg      Fixed array implementation of rfc_card_mode.
09/30/04   sar     Added function rfc_copy_nv_items() and changed rfc_card_mode 
                   to 2-element array to manage the status of the two rx chains.
09/10/04   zg      Fixed rfc_card_mode for the secondary chain.
08/19/04   dbc     Added support for ZRF617X (CDMA 450) RF card
08/16/04   dyc     Remove DB_RF_ANALOG components and rf_init_card_analog()
08/11/04   sar     Updated functions and definition to re-structure the rfc layer.
08/03/04   zg      Fixed compiling err in rfc_init_card() while undef-ing GPS.
07/01/04   dbc     Change rfc_init_card_gps() so that it only changes the 
                   RF card mode for RFCOM_TRANSCEIVER_0 
07/01/04   dbc     Only set/change the RF card mode for RFCOM_TRANSCEIVER_0
                   and RFCOM_RECEIVER_DIV devices
06/28/04   dyc     rfc_init_card_cdma/pcs() support diversity band/chan mode set
06/15/04   sar     Updated functions for GZRF6500 macros.
04/14/04   sar     Added support for Enhanced Bands.
04/12/04   dbc     Aded call to synth_rfrx_pll_enable() in rfc_init_card_gps()
                   to enable GPS PLL
02/06/04   dbc     Changes for ZIF library architecture update
10/29/03   ra      disabled ability to tune tx pll if device is RFCOM_RECEIVER_1
10/27/03   dyc     Fixed cell only compile error in rfc_init_card_pcs()
10/13/03   ra      added support for JCDMA IMT hw switch 
10/08/03   zg      BBRX multiple chain support.
10/02/03   dbc     Chain 1 changes to use external synthesizer
09/23/03   zg      Added GPS on Chain1 support in rfc_init_card().
                   Fixed error_fatal in rfc_tune_synth().  
09/15/03   sar     Ruben's change to support gps mode: modified rfc_init_card_gps 
                   to handle primary or secondary chain.
09/05/03   dbc     Added support for CDMA in IMT band
09/04/03   et      replaced all the indirect uses of rfcom_to_path[device] with
                   local variable path
08/18/03   dbc     Use new macro names for configuring RF GPIOs.
08/11/03   dbc     Added support for RF_GPIO_SELECT_PRIMARY_CHAIN_VCO() and 
                   RF_GPIO_SELECT_SECONDARY_CHAIN_VCO
08/05/03   dbc     Changed rfc_tune_synth() to use calls to synth driver to
                   program the PLLs
07/11/03   sar     Added support for multipath chipset drivers.
06/02/03   dyc     Added support for RF_HAS_SINGLE_BAND_PCS.
03/03/03   dbc     Moved usage of MDSP API for Rx spectral inversion to 
                   rf_init_delayed() (rf.c) 
02/06/03   dbc     Use MDSP API for Rx spectral inversion when 
                   RF_DSP_HAS_RX_SPECTRAL_INV_CONFIG is defined.
01/24/03   dbc     Changed RF_HAS_NV_VCO_CONFIG to RF_HAS_VCO_CONFIG
01/13/03   aks     Changes for GSM PLT.
01/08/03   dbc     Added support for internal/external VCO selection via 
                   RF_HAS_NV_VCO_CONFIG
10/30/02   dbc     removed include of rfigpio.h.  Wrapped A/B selection 
                   functionality in RFT_PCS_SUPPORTS_BAND_SELECTION.
10/24/02   dbc     changed rfc_tune_synth() to call rft_rf_pll_load() directly
                   instead of synth_tune_uhf().  Removed synth_tune_uhf().
10/18/02   dbc     made changes to rfc_tune_synth() to use 
                   rft_is_band_chan_supported() to check if channel is 
                   supported before programming PLLs. Added curr_cdma_band 
                   static and associated rf_set_cdma_band() access function.
                   Moved synth_tune_uhf() from synth.c to this unit.                   
10/08/02   dbc     removed rfc_get_uhf_freq() and associated synthesizer 
                   constants
09/17/02   tn      removed unnecessary and unused definitions
09/10/02   et      added check for channel out of range
09/06/02   dbc     Remove usage of RF_GPIO_CDMA_PCS_RXTX_HI_MODE() and
                   RF_GPIO_CDMA_PCS_RXTX_LO_MODE() macros and replace with 
                   RF_GPIO_CDMA_PCS_MODE()
09/04/02   dbc     Remove legacy code. Cleanup.
08/26/02   dbc     Added rf_set_band_sel_config(), rf_set_pcs_band(), and
                   rf_determine_a_b_pos() functions.  Added rfc_card_band, 
                   curr_pcs_band, pcs_band_sel_setting, and kpcs_band_sel_setting
                   local variables. These use to configure the A/B switching 
                   characteristic for the PCS band at run time.
05/26/02   jwh     Removed PHONE_T include.
03/08/02   dbc     rfc_init_card_xxx() now call rft_set_mode(xxx) and 
                   rfr_set_mode() instead of rft_init(xxx) and rfr_init(xxx) 
02/22/02    zg     Moved bbrx_set_mode from rfc_init_card_cdma and 
                   rfc_init_card_pcs to rxf_set_mode.
01/31/02    zg     Merged in GPS related changes from PhoneT. 
01/21/02    et     got rid of compiler warning
11/15/01    et     changed uhf_freq to channel_or_uhf_freq to account for
                   6000 chipset, where we need to pass down the channel
                   instead of the uhf_freq.  passing the appropriate value
                   to synth_tune_uhf 
11/13/01   aks     Remove use of RF_HAS_RFT and RF_HAS_RFR.  Function name
                   changes.
11/02/01   dbc     Remove backwards compatible (pre MSM6000) code.  Put 
                   RF_NOT_PORTED around unported code in rfc_init_card_gps().
10/25/01   dbc     Added support for MSM6000 PA Control
10/19/01   aks     Changed call from adie_set_system_mode() to bbrx_set_mode().
10/08/01   aks     Included db.h, merged changes from phone_t.
10/01/01   dbc     Made changes for MSM6000+ port.
01/15/01   fas     Add support for FFA Clouseau III.
01/10/01   djd     Changed direct ENC register writes to calls to
                   enc_set_rx_polarity and enc_set_tx_polarity to set
                   the spectral polarity for rx and tx.
12/15/00   fas     Undid inversion of Q_OFFSET polarity for MSM3100C
10/18/00   fas     Copied entirely from PPG:
           L:/src/asw/MSM3300/VCS/rfcs.c_v   1.14   18 Sep 2000 17:36:40   ddresser
09/18/00   dsb     Added code to compensate for GPS Rx spectral inversion.
08/28/00   dsb     Removed old Trimode macros.  Inserted new BSP-compatible
                   macros.  Renamed rfc_set_pcs_band_hw to rfc_set_pcs_band.
                   Removed rfc_get_pcs_band.  Cleaned up rfc_get_uhf_freq, 
                   ie. it no longer needs Trimode specific code.  Added
                   dual PA support for rfc_init_card.  Update all 
                   rfc_init_card_* functions to use new RF MODE macros.
                   ie. no more BAND_SEL or PCS_BAND_SEL.  Added
                   BSP spectral inversion support and RFT programming to
                   alll rfc_init_card_* functions.
07/24/00   dsb     Changed FEATURE_SURF3100_TM to RF_HAS_TRIMODE.  Added
                   support for new RF mode pins (replace FM_N and BAND_SEL).
05/08/00   dsb     Added support for new GPS_MODE GPIO register bindings.
04/12/00   dd      Added GPS support.
01/21/99   tn      updated feature names.
12/15/99   rv      Merge with TD1017 rf files.
12/13/99   ymc     Removed card band checking in rfc_init_card_*.
                   Added card band setting in rfc_init_card_*.
12/09/99   ymc     Renamed rf_cdma_band_type as rf_card_band_type;
                   Renamed RF_CDMA_BAND_800MHZ as RF_800_BAND;
                   Renamed RF_CDMA_BAND_1900MHZ as RF_PCS_BAND;
                   Moved macros IS_RF_CARD_INITIALIZED, RFC_GET_CARD_MODE
                   and RFC_GET_CARD_BAND to rfc.h;
                   Externalized functions rfc_get_uhf_freq and rfc_get_pcs_band;
                   Moved rfc_pcs_band_type declaration to rfc.h;
                   Renamed SYNTH_MODE_CDMA as RFC_CARD_MODE_CDMA;
                   Renamed SYNTH_MODE_PCS as RFC_CARD_MODE_PCS;
                   Renamed SYNTH_MODE_ANALOG as RFC_CARD_MODE_ANALOG;
                   Renamed SYNTH_MODE_NONE as RFC_CARD_MODE_NONE;
                   Renamed synth_mode_type as rfc_card_mode_type;
10/26/99   ymc     Modified for RF synthesizer BSP (Board Support Package) support.
01/10/01   djd     Changed direct ENC register writes to calls to
                   enc_set_rx_polarity and enc_set_tx_polarity to set
                   the spectral polarity for rx and tx.
                   Modified so that rfc_config is not used any more.
                   RF config. type of RFC_CDMA_1800MHZ is no longer supported.
09/10/99   ymc     Used RF_GPIO_* name definitions in rfigpio.h for 
                   PCS_BAND_SELECT signal.
08/02/99   jc      Removed new gpio_xxx MSM3100 support.
07/15/99   jc      Added new gpio_xxx MSM3100 support.
01/06/99   ychan   Modified for targets NGP, SURF, Q-1900.
01/06/99   ychan   Updated Copyright date.
06/05/98   thh     Changed VHF charge pump current back to 4mA and VHF
                   prescaler value to 64.
04/09/98   thh     Added retune at RxTx transition.
01/07/98   thh     Ported from TGP RF card driver (RFCT.C) on this date.

===========================================================================*/

 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target definitions */
#include "cagc.h"
#include "rficap.h"     /* RF capabilities */
#include "rfc.h"        /* RF card interface */
#include "rfifreq.h"    /* Frequency & channelization definitions */
#include "comdef.h"     /* Common definition */
#include "err.h"        /* Errors log & messages */
#include "clk.h"        /* Clock Services */
#include "bsp.h"        /* System RF settings */
#include "enc.h"        /* Encoder driver APIs */
#include "db.h"         /* Database services  */
#include "rft.h"        /* RFT3100 macros       */
#include "rfr.h"
#include "adie.h"
#include "bbrx.h"
#include "rflib.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif 
#include "rfcom.h"
#include "synth.h"
#include "rfi.h"

#ifdef RF_HAS_HW_DZRF
#error code not present
#endif
#ifdef RF_HAS_HW_JTDZRF
#error code not present
#endif
#ifdef RF_HAS_HW_DZRFBC14
#error code not present
#endif
#ifdef RF_HAS_HW_D450
#error code not present
#endif
#ifdef RF_HAS_HW_WDZRF
#error code not present
#endif
#ifdef RF_HAS_HW_DZRF6285
#error code not present
#endif
#ifdef RF_HAS_HW_QSC60X5
#include "rfc_qsc6055.h"
#endif
#ifdef RF_HAS_QSC11X0
#include "rfc_qsc1100.h"
#endif
#ifdef RF_HAS_HW_TRF6500
#error code not present
#endif

#ifdef RF_HAS_HW_TRF6285
#error code not present
#endif

#ifdef RF_HAS_HW_QRF7600
#error code not present
#endif

#ifndef FEATURE_GSM_PLT  
       
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* -------------------------------------------------------------------------

                              LOCAL DATA

---------------------------------------------------------------------------- */

/* TRUE if rfc_init_card() has been called and the function returned successfully.
*/
boolean rfc_is_card_init = FALSE;

static boolean rfc_is_set = FALSE;

/* Current mode in the RF card : RFC_CARD_MODE_CDMA, RFC_CARD_MODE_PCS 
   or RFC_CARD_MODE_ANALOG
   If it has not been configured into any mode, rfc_card_mode will hold the default
   value of RFC_CARD_MODE_NONE.
*/
rfc_card_mode_type rfc_card_mode[2] = { RFC_CARD_MODE_NONE, RFC_CARD_MODE_NONE };


#ifdef RF_HAS_SINGLE_BAND_PCS
/* single band filter */

/* US PCS */
static rflib_band_sel_chan_config_type pcs_band_sel_setting =  { 0,
                                                               1199,
                                                               1200, 
                                                               1200 
                                                             };
/* KPCS */
static rflib_band_sel_chan_config_type kpcs_band_sel_setting = { 0,
                                                               1199,
                                                               1200, 
                                                               1200 
                                                             };
/* IMT */
static rflib_band_sel_chan_config_type imt_band_sel_setting =  { 0,
                                                               1199,
                                                               1200, 
                                                               1200 
                                                             };

/* US PCS + 5MHz. */
static rflib_band_sel_chan_config_type bc14_band_sel_setting = { 0,
                                                               1299,
                                                               1300, 
                                                               1300 
                                                             };

/* AWS Band */
static rflib_band_sel_chan_config_type bc15_band_sel_setting = { 0,
                                                               899,
                                                               900, 
                                                               900 
                                                             };
#else  /* RF_HAS_SINGLE_BAND_PCS */
/* split band filter */

/* US PCS */
static rflib_band_sel_chan_config_type pcs_band_sel_setting =  { 0,
                                                               600,
                                                               601, 
                                                               1199  
                                                             };
/* KPCS */
static rflib_band_sel_chan_config_type kpcs_band_sel_setting = { 0,
                                                               600,
                                                               601, 
                                                               1199  
                                                             };
/* IMT */
static rflib_band_sel_chan_config_type imt_band_sel_setting =  { 0,
                                                               600,
                                                               601, 
                                                               1199  
                                                             };

/* US PCS + 5MHz. */
static rflib_band_sel_chan_config_type bc14_band_sel_setting = { 0,
                                                               650,
                                                               651, 
                                                               1299  
                                                             };
#endif /* RF_HAS_SINGLE_BAND_PCS */

static void rf_determine_a_b_pos(rfcom_device_enum_type device, const rflib_band_sel_chan_config_type *cptr, dword channel);

/* -------------------------------------------------------------------------

                              LOCAL DATA

---------------------------------------------------------------------------- */

rf_card_type rf_card=
{
    /* RFC Data */
    RF_HW_UNDEFINED,
    RF_MAX_BAND,
    (word) RFC_TUNE_CHAN_WAIT,
    (word) RF_TUNE_TO_CHAN_TIME_USEC,
    FALSE,
    #if defined (RF_HAS_ALTERNATE_GPS_ASIC)
#error code not present
    #endif /* RF_HAS_ALTERNATE_GPS_ASIC */
    /* RFC Functions */
    (rfc_init_card_type)                    rfc_empty_func,
    (rfc_set_mode_park_type)                rfc_empty_func,
    (rfc_set_mode_type)                     rfc_empty_func,
    (rfc_select_primary_vco_type)           rfc_empty_func,
    (rfc_select_secondary_vco_type)         rfc_empty_func,
    (rfc_rft_band_A_select_type)            rfc_empty_func,
    (rfc_rft_band_B_select_type)            rfc_empty_func,
    (rfc_enable_primary_chain_vco_type)     rfc_empty_func,
    (rfc_enable_secondary_chain_vco_type)   rfc_empty_func,
    (rfc_disable_primary_chain_vco_type)    rfc_empty_func,
    (rfc_disable_secondary_chain_vco_type)  rfc_empty_func,
    (rfc_get_hdet_adc_channel_type)         rfc_empty_func, 
    (rfc_manage_vregs_type)                 rfc_empty_func,
    (rfc_uses_external_vco_type)            rfc_empty_func,
    #if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
    (rfc_get_hdet_comp_value_type)          rfc_empty_func_return_0,
    (rfc_get_hdet_comp_value_for_hdr_type)	rfc_empty_func_return_0,
    #endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */
    #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    (rfc_uses_internal_hdet_type)           rfc_empty_func_return_0,
    #endif /* RF_HAS_INTERNAL_HDET_SUPPORT */
    #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
    (rfc_uses_device_info_for_spectral_inv_type) rfc_empty_func,
    #endif    
    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    (rfc_set_pa_access_probe_config_type)  rfc_empty_func_return_0,
    #endif
    #ifdef RF_HAS_RFCONFIG_RX_LNA
    (rfc_init_rfconfig_rx_lna_type)         rfc_empty_func,
    #endif /* RF_HAS_RFCONFIG_RX_LNA */
    
    /* Misc Attributes */
    (uint8)2,
    {0},/*lint -e651 */
    {0} /*lint -e651 */

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    ,(rfc_get_block_type) rfc_empty_func_return_0
    ,(rfc_block_mask_is_valid_type) rfc_empty_func_return_0
    ,(rfc_set_block_type) rfc_empty_func
    #endif

    #ifdef RF_HAS_INTERNAL_THERM_SUPPORT
    ,(rfc_uses_internal_therm_type)    rfc_empty_func_return_0
    ,(rfc_read_internal_therm_type)    rfc_empty_func_return_0
    #endif

    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    ,(rfc_support_temp_based_device_config_type)  rfc_empty_func_return_0
    #endif

}; 
/* confusing initializer, since the exact size is not known at this time */

/*===========================================================================

FUNCTION RFC_INIT_CARD                                            EXTERNAL

DESCRIPTION
  This function checks the RF band and confirms if the band is supported.  It
also initializes PCS_BAND_SELECT GPIO pin as output pin.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the RF card could not be initialized.
  TRUE if the RF card was properly initialized.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfc_init_card
(
  rfcom_device_enum_type device,
  rf_card_band_type band
)
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  
  /* Initializing local data */
  rfc_is_card_init = FALSE;

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return 0;
  }

  rfc_card_mode[path] = RFC_CARD_MODE_NONE;

  switch (band) {
  /* CDMA BANDS */
  case RF_BC0_BAND:  /* US Cellular */
  case RF_BC3_BAND:  /* JCDMA */
  case RF_BC10_BAND:
    rfc_is_card_init = TRUE;
    if(!rfc_init_card_cdma( device ))
    {
      ERR( "Failed to initialize rfc band:", band, 0, 0 );
      rfc_is_card_init = FALSE;
    }
    break;
  /* PCN BANDS */
  case RF_BC1_BAND:  /* US PCS         */
  case RF_BC4_BAND:  /* Korean PCS     */
  case RF_BC5_BAND:  /* CDMA 450       */
  case RF_BC6_BAND:  /* IMT            */
  case RF_BC11_BAND: /* BC11.          */
  case RF_BC14_BAND: /* US PCS + 5MHz. */
  case RF_BC15_BAND: /* AWS Band       */
    rfc_is_card_init = TRUE;
    if(!rfc_init_card_pcs( device ))
    {
      ERR( "Failed to initialize rfc band:", band, 0, 0 );
      rfc_is_card_init = FALSE;
    }
    break;

#ifdef RF_HAS_GPS  
#error code not present
#endif /*RF_HAS_GPS*/

  default:
    ERR( "Invalid RF band configuration : %d", band, 0, 0 );
  }

  return( rfc_is_card_init );     /* RF card successfully initialized */

} /* end of rfc_init_card */

/*===========================================================================

FUNCTION RFC_CHAN_OFFSET                                    EXTERNAL FUNCTION

DESCRIPTION
  Function to calculate offset from the channel with the lowest frequency.

DEPENDENCIES
  Only applies for Cellular mode

RETURN VALUE
  word value 0..831 representing the channel offset

SIDE EFFECTS
  None

===========================================================================*/
word rfc_chan_offset
(
  word channel          /* input channel number */
)
{
  return ((channel+32) % 1023);
} /* rfc_chan_offset */

/*===========================================================================

FUNCTION RFC_INIT_CARD_CDMA                                 EXTERNAL FUNCTION

DESCRIPTION
  Initialize the RF card and its synthesizer for CDMA mode.  This 
  function initializes the Q-offset PDM polarity based on the RF
  card configuration (identified by the BSP).  For example, an RF
  board with low-side injection at RF, and high-side injection at
  IF on the RX chain will have an inverted spectrum on Q data.  
  This results in having to invert the spectrum in the MSM at baseband 
  to appropriately recover the data.  This process takes place before
  the Q-offset PDM value is determined for the CAGC loop. As a result,
  the Q-offset PDM polarity must also be inverted.

DEPENDENCIES
  rfc_init_card() has to be called first to initialize RF card successfully.

RETURN VALUE
  TRUE : Cellular CDMA mode initialization is successful.
  FALSE : not successful.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfc_init_card_cdma( rfcom_device_enum_type device )
{
  boolean mode_inited = FALSE;    /* Successful mode initialization? */
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return 0;
  }

  /* The RF card must first be initialized.
  */
  if( IS_RF_CARD_INITIALIZED() == FALSE ) 
  {
    ERR("RF card not yet initialized",0,0,0);
  }
  else 
  {
    switch ( device ) 
    {
    case RFCOM_TRANSCEIVER_0:
        
        rfc_card_mode[path] = RFC_CARD_MODE_NONE;

        /* Set chain 0 to cdma mode */
        rf_card.rfc_set_mode(path,rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif

        /* Program RF chips for CDMA */
        rft_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA);
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA); 

        /* program synth */
        synth_set_mode( path,rf_chain_ptr->rf_curr_cdma_band,DB_RF_CDMA);

        MSG_LOW("RF card in CDMA mode",0,0,0);

        break;

    case RFCOM_RECEIVER_DIV:
        
        /* Set chain 1 to CDMA mode */
        rf_card.rfc_set_mode(RF_PATH_1,rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif

        /* For diversity use the VCO/PLL from first chain */
        rf_card.rfc_select_primary_vco();

        /* we are using the RFT PLL on chain 1 */
        rflib_switch_path_1_pll( RFT_PLL );

        /* Select the appropriate internal/external VCO for diversity mode */
        if ( rf_card.rfc_uses_external_vco(RF_PATH_1, RFC_DIV_VCO) )
        {
          rflib_rfr_use_external_vco(path, TRUE);
        }
        else
        {
          rflib_rfr_use_external_vco(path, FALSE);
        }
        
        /* Program RFR only */
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA);

        MSG_LOW("RF card in CDMA mode",0,0,0);

        break;

    case RFCOM_RECEIVER_1:

        /* Set chain 1 to CDMA */
        rf_card.rfc_set_mode(path,rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif
        
        /* For diversity use the VCO/PLL from first chain */
        rf_card.rfc_select_secondary_vco();

        /* we are using the EXT PLL on chain 1 */
        rflib_switch_path_1_pll( EXT_SYNTH );

        /* Select the appropriate internal/external VCO for SHDR mode */
        if ( rf_card.rfc_uses_external_vco(RF_PATH_1, RFC_DUAL_RX_VCO) )
        {
          rflib_rfr_use_external_vco(path, TRUE);
        }
        else
        {
          rflib_rfr_use_external_vco(path, FALSE);
        }

        /* program the RFR */
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA ); 

        /* program synth */
        synth_set_mode( path ,rf_chain_ptr->rf_curr_cdma_band,DB_RF_CDMA );

        break;

    default:
        ERR("rfc_init_card_cdma: device not valid",0,0,0);
        break;
    }

    /* Remember the mode that the RF card is in */
    rfc_card_mode[path] = RFC_CARD_MODE_CDMA;
    
    mode_inited = TRUE;

  }

  return( mode_inited );

} /* end of rfc_init_card_cdma */

 
/*===========================================================================

FUNCTION RFC_INIT_CARD_PCS                                  EXTERNAL FUNCTION

DESCRIPTION
  Initialize the RF card and its synthesizer for PCS mode.  This 
  function initializes the Q-offset PDM polarity based on the RF
  card configuration (identified by the BSP).  For example, an RF
  board with low-side injection at RF, and high-side injection at
  IF on the RX chain will have an inverted spectrum on Q data.  
  This results in having to invert the spectrum in the MSM at baseband 
  to appropriately recover the data.  This process takes place before
  the Q-offset PDM value is determined for the CAGC loop. As a result,
  the Q-offset PDM polarity must also be inverted.

DEPENDENCIES
  rfc_init_card() has to be called first to initialize RF card successfully.

RETURN VALUE
  TRUE : PCS mode initialization is successful.
  FALSE : failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfc_init_card_pcs( rfcom_device_enum_type device )
{
  boolean mode_inited = FALSE;    /* Successful mode initialization? */

  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];

  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return 0;
  }

  /* The RF card must first be initialized.
  */
  if( IS_RF_CARD_INITIALIZED() == FALSE ) 
  {
    ERR("RF card not yet initialized",0,0,0);
  }
  else 
  {
    switch ( device ) 
    {
    case RFCOM_TRANSCEIVER_0:

        rfc_card_mode[path] = RFC_CARD_MODE_NONE;
        
        /* Set chain 0 to PCS */
        rf_card.rfc_set_mode(path, rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif

        /* Program RF Chips for PCS */
        rft_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN);
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN);

        /* program synth */
        synth_set_mode( path ,rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN );

        MSG_LOW("RF card in PCS mode",0,0,0);

        break;

    case RFCOM_RECEIVER_DIV:

        /* Set chain 1 to PCS */
        rf_card.rfc_set_mode(RF_PATH_1, rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif
        
        /* For diversity use the VCO/PLL from first chain */
        rf_card.rfc_select_primary_vco();

        /* we are using the RFT PLL on chain 1 */
        rflib_switch_path_1_pll( RFT_PLL );

        /* Select the appropriate internal/external VCO for diversity mode */
        if ( rf_card.rfc_uses_external_vco(RF_PATH_1, RFC_DIV_VCO) )
        {
          rflib_rfr_use_external_vco(path, TRUE);
        }
        else
        {
          rflib_rfr_use_external_vco(path, FALSE);
        }
        
        /* Program RFR only */
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band,DB_RF_PCN );

        MSG_LOW("RF card in PCS mode",0,0,0);

        break;

    case RFCOM_RECEIVER_1:

        /* Set chain 1 to PCS */
        rf_card.rfc_set_mode(path, rf_chain_ptr->rf_curr_cdma_band);

        #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
        rf_card.rfc_uses_device_info_for_spectral_inv(device,rf_chain_ptr->rf_curr_cdma_band);
        #endif

        /* Select secondary vco */
        rf_card.rfc_select_secondary_vco();

        /* we are using the EXT PLL on chain 1 */
        rflib_switch_path_1_pll( EXT_SYNTH );

        /* Select the appropriate internal/external VCO for diversity mode */
        if ( rf_card.rfc_uses_external_vco(RF_PATH_1, RFC_DUAL_RX_VCO) )
        {
          rflib_rfr_use_external_vco(path, TRUE);
        }
        else
        {
          rflib_rfr_use_external_vco(path, FALSE);
        }
        
        /* program the RFR */
        rfr_set_mode( path, rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN );

        /* program synth */
        synth_set_mode( path ,rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN );

        break;

    default:
        ERR("rfc_init_card_pcs: device not valid",0,0,0);
        break;


    }

    /* Remember the mode that the RF card is in */
    rfc_card_mode[path] = RFC_CARD_MODE_PCS;

    mode_inited = TRUE;
    rfc_is_card_init = TRUE;
  }

  return( mode_inited );

} /* end of rfc_init_card_pcs */

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */
/*===========================================================================

FUNCTION RFC_TUNE_SYNTH                                     EXTERNAL FUNCTION

DESCRIPTION
  This function programs the synthesizer to tune to a specific channel.  The
  frequency of the channel is dependent on the current mode of the RF card.

DEPENDENCIES
  RF card has to be initialized into a certain mode.  Either rfc_init_card_cdma(),
  rfc_init_card_pcs(), or rfc_init_card_analog() has to be invoked first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfc_tune_synth
(
  rfcom_device_enum_type device,
  word channel       /* Channel to tune to */
)
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
  
  if (path >= RF_PATH_MAX)
  {
     MSG_FATAL("path >= RF_PATH_MAX", 0, 0, 0);
     rf_error_flag = TRUE;
     return;
  }

  /* If we don't know which RF configuration it is, don't tune.
  */
  if( rfc_card_mode[path] == RFC_CARD_MODE_NONE ) {
    ERR("Cannot tune before RF card mode init,Device:%d,Path:%d",device,path,0);
    return;
  }

  MSG_LOW("Card mode: %d - Chan: %d", rfc_card_mode, channel, 0);


  /* --------------------------------------------
  ** Program the new channel into the
  ** UHF Tx/Rx synthesizer main divider register.
  ** -------------------------------------------- */

  switch( rf_chain_ptr->rf_mode) 
  {

  case DB_RF_CDMA:   /* CDMA mode */

      if ( rflib_is_band_chan_supported( path, rf_chain_ptr->rf_curr_cdma_band,  channel ) )
      {
        synth_rfrx_pll_load ( device, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA, channel );

#ifdef FEATURE_FACTORY_TESTMODE 
        /* don't tune TX if in OFS  */
        if( device != RFCOM_RECEIVER_1)
        {         
          /* In FTM mode program the TX PLLs here */
          if ( ftm_mode == FTM_MODE )
            synth_rftx_pll_load( device, rf_chain_ptr->rf_curr_cdma_band, DB_RF_CDMA, channel );
        }
#endif 
      }
      else ERR("Channel %d not programmed ", channel, 0, 0);

     break;


  case DB_RF_PCN:    /* PCS mode */

       if ( rflib_is_band_chan_supported( path, rf_chain_ptr->rf_curr_cdma_band, channel ) )
       {
         synth_rfrx_pll_load ( device, rf_chain_ptr->rf_curr_cdma_band, DB_RF_PCN, channel );

#ifdef FEATURE_FACTORY_TESTMODE
         /* don't tune TX if in OFS  */
         if( device != RFCOM_RECEIVER_1)
         {
           /* In FTM mode program the TX PLLs here */
           if ( ftm_mode == FTM_MODE )
           {
             synth_rftx_pll_load( device,                                \
                                  rf_chain_ptr->rf_curr_cdma_band,   \
                                  DB_RF_PCN,                     \
                                  channel );
           }
         }
#endif 
           
         if ( rf_chain_ptr->rf_curr_cdma_band == RF_BC4_BAND  )
         {
            rf_determine_a_b_pos(device, &kpcs_band_sel_setting, channel); /* KPCS */
         }
         else if ( rf_chain_ptr->rf_curr_cdma_band == RF_BC6_BAND  )
         {
            rf_determine_a_b_pos(device, &imt_band_sel_setting, channel);  /* IMT */
         }
         else if ( rf_chain_ptr->rf_curr_cdma_band == RF_BC14_BAND  )
         {
            rf_determine_a_b_pos(device, &bc14_band_sel_setting, channel); /* US PCS + 5MHz.*/
         }
         else if ( rf_chain_ptr->rf_curr_cdma_band == RF_BC15_BAND  )
         {
            rf_determine_a_b_pos(device, &bc15_band_sel_setting, channel); /* AWS Band*/
         }
         else if (( rf_chain_ptr->rf_curr_cdma_band != RF_BC5_BAND  ) &&
                  ( rf_chain_ptr->rf_curr_cdma_band != RF_BC11_BAND  ))
         {
            rf_determine_a_b_pos(device, &pcs_band_sel_setting, channel);  /* US PCS */
         }
       }
       else ERR("Channel %d not programmed ", channel, 0, 0);
     
     break;
  #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
  case DB_RF_GPS:    /* Nothing to do for GPS mode */
     break;
  #endif

    default:
      ERR_FATAL("Unknown card mode.",0,0,0);
  }  
}

/*=========================================================================

FUNCTION rf_set_band_sel_config

DESCRIPTION
  This function populates the A/B channel configuration that determines 
  the switch points for the specified band. Currently supported
  bands are RF_US_PCS and RF_KPCS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_set_band_sel_config( rf_card_band_type band, word a_chan_min, word a_chan_max, 
                              word b_chan_min, word b_chan_max)
{
    
    if ( band ==  RF_BC1_BAND ) 
    {
       /* US PCS */
       pcs_band_sel_setting.a_band_max_chan =  a_chan_max;
       pcs_band_sel_setting.a_band_min_chan =  a_chan_min;
       pcs_band_sel_setting.b_band_max_chan =  b_chan_max;
       pcs_band_sel_setting.b_band_min_chan =  b_chan_min;
    }
    else if ( band == RF_BC4_BAND ) 
    {
       /* KPCS */
       kpcs_band_sel_setting.a_band_max_chan =  a_chan_max;
       kpcs_band_sel_setting.a_band_min_chan =  a_chan_min;
       kpcs_band_sel_setting.b_band_max_chan =  b_chan_max;
       kpcs_band_sel_setting.b_band_min_chan =  b_chan_min;
    }
    else if ( band == RF_BC6_BAND ) 
    {
       /* IMT */
       imt_band_sel_setting.a_band_max_chan =  a_chan_max;
       imt_band_sel_setting.a_band_min_chan =  a_chan_min;
       imt_band_sel_setting.b_band_max_chan =  b_chan_max;
       imt_band_sel_setting.b_band_min_chan =  b_chan_min;
    }
    else if ( band == RF_BC14_BAND ) 
    {
       /* US PCS + 5MHz */
       bc14_band_sel_setting.a_band_max_chan =  a_chan_max;
       bc14_band_sel_setting.a_band_min_chan =  a_chan_min;
       bc14_band_sel_setting.b_band_max_chan =  b_chan_max;
       bc14_band_sel_setting.b_band_min_chan =  b_chan_min;
    }    
}

/*=========================================================================

FUNCTION rf_determine_a_b_pos

DESCRIPTION
  This function configures the GPIOs and RFT chip A/B selection per the
  channel.  An rft_band_sel_chan_config_type determines if the channel is
  in the "A" or "B" position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void rf_determine_a_b_pos(rfcom_device_enum_type device, const rflib_band_sel_chan_config_type *cptr, dword channel)/*l-int -e818 cptr could be declared as const */
{
  rf_path_enum_type path = rfcom_to_path[ device ];
  if ( ( channel >= cptr->b_band_min_chan ) && ( channel < (cptr->b_band_max_chan + 1) ) )
  {
     rf_card.rfc_rft_band_B_select();
     rflib_set_band_sel_switch(path, RFLIB_SELECT_B );
  }
  else if ( ( channel >= cptr->a_band_min_chan ) && ( channel < (cptr->a_band_max_chan + 1)))
  {
     rf_card.rfc_rft_band_A_select();
     rflib_set_band_sel_switch(path, RFLIB_SELECT_A );
  }
  else 
  {
     ERR("Band select switch not defined for chan %x", channel, 0, 0);
  }
}

/*===========================================================================

FUNCTION    RFC_SET_CARD

DESCRIPTION
  Sets up the internal function pointers to control the specified RF HW Card.
  
PARAMETERS
  The rf hw type to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_set_card(rf_hw_type rf_hw)
{
    if(rfc_is_set) /* Card was already set */
    {
        return;
    }
    else
    {
        rfc_is_set = TRUE;
    }

    switch(rf_hw)
    {
#ifdef RF_HAS_HW_DZRF
#error code not present
#endif
#ifdef RF_HAS_HW_JTDZRF
#error code not present
#endif
#ifdef RF_HAS_HW_DZRFBC14
#error code not present
#endif
#ifdef RF_HAS_HW_D450
#error code not present
#endif
#ifdef RF_HAS_HW_WDZRF
#error code not present
#endif
#ifdef RF_HAS_HW_DZRF6285
#error code not present
#endif
#ifdef RF_HAS_HW_TRF6500
#error code not present
#endif
#ifdef RF_HAS_HW_QSC60X5
    case RF_HW_QSC60X5_DUAL_BAND:
    case RF_HW_QSC60X5_TRI_BAND:
        rfc_qsc60x5_init_card((void *) &rf_card);
        break;
#endif
#ifdef RF_HAS_HW_QSC11X0
    case RF_HW_QSC11X0_DUAL_BAND:
    case RF_HW_QSC11X0_TRI_BAND:
    case RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA:
        rfc_qsc11x0_init_card((void *) &rf_card);
        break;
#endif
#ifdef RF_HAS_HW_TRF6285
#error code not present
#endif

#ifdef RF_HAS_HW_QRF7600
#error code not present
#endif

    default:
        MSG_ERROR( "rf_init(), Illegal RF Card specified.",0,0,0);
        return;
    }

#ifdef T_RUMI_SC2X
#error code not present
#endif /* T_RUMI_SC2X */

}

/*===========================================================================

FUNCTION    RFC_SET_MODE_PARK

DESCRIPTION
  Sets the specified path to park.
  
PARAMETERS
  RF PATH to be parked.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_set_mode_park(rf_path_enum_type path)
{
    rf_card.rfc_set_mode_park(path);
}

/*===========================================================================

FUNCTION    RFC_SET_MODE

DESCRIPTION
  Sets the specified path to the mode of operation based on band setting.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_set_mode(rf_path_enum_type path, rf_card_band_type band)
{
    rf_card.rfc_set_mode(path, band);
}
/*===========================================================================

FUNCTION    RFC_SELECT_PRIMARY

DESCRIPTION
  Selects the primary vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_select_primary_vco(void)
{
    rf_card.rfc_select_primary_vco();
}

/*===========================================================================

FUNCTION    RFC_SELECT_SECONDARY

DESCRIPTION
  Selects the secondary chain vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_select_secondary_vco(void)
{
    rf_card.rfc_select_secondary_vco();
}

/*===========================================================================

FUNCTION    RFC_RFT_BAND_A_SELECT

DESCRIPTION
  Selects PCS Band A for RFT.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_rft_band_A_select(void)
{
    rf_card.rfc_rft_band_A_select();
}
/*===========================================================================

FUNCTION    RFC_RFT_BAND_B_SELECT

DESCRIPTION
  Selects PCS Band B for RFT.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_rft_band_B_select(void)
{
    rf_card.rfc_rft_band_B_select();
}

/*===========================================================================

FUNCTION    RFC_ENABLE_PRIMARY_CHAIN_VCO

DESCRIPTION
  Enables the primary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_enable_primary_chain_vco(void)
{
    rf_card.rfc_enable_primary_chain_vco();
}
  
/*===========================================================================

FUNCTION    RFC_ENABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Enables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_enable_secondary_chain_vco(void)
{
    rf_card.rfc_enable_secondary_chain_vco();
}
  
/*===========================================================================

FUNCTION    RFC_DISABLE_PRIMARY_CHAIN_VCO

DESCRIPTION
  Disables the primary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_disable_primary_chain_vco(void)
{
    rf_card.rfc_disable_primary_chain_vco();
}
  
/*===========================================================================

FUNCTION    RFC_DISABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Disables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_disable_secondary_chain_vco(void)
{
    rf_card.rfc_disable_secondary_chain_vco();
}

/*=========================================================================

FUNCTION RFC_MANAGE_VREGS

DESCRIPTION
  Manages the RFRX1, RFRX2, RFTX and PA voltage regulators for the RF Card
  to conform to the RF Chain State.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_manage_vregs(rfc_vregs_id_type vregs, pm_switch_cmd_type cmd, rf_path_enum_type path )
{
    rf_card.rfc_manage_vregs(vregs, cmd, path);
}

/*===========================================================================

FUNCTION RFC_GET_SUB_CLASS

DESCRIPTION
  This function takes band parameter and and returns an associated subclass
  bitmask. For bands that do not have subclasses, LSB of the mask is set to 1.

DEPENDENCIES
  None

RETURN VALUE
  A word containing bitmask set to indicate the supported subclasses with the
  following mapping:
  
    
    BIT  :  SUBCLASS 
   -------------------
     0         0   
     1         1   
     2         2   
     3         3   
     4         4   
     5         5   
     6         6   
     7         7   
     8         8   
     9         9   
    10        10  
    11        11  
    12        12  
    13        13  
    14        14  
    15        15  
  ----------------

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 rfc_get_sub_class( rf_card_band_type band )
{
    return (rf_card.rfc_subclass_mask_tbl[band]);
}

/*===========================================================================

FUNCTION RF_GET_NUM_ANTENNA

DESCRIPTION
  This function does not take a parameter and returns an unsigned int representing
  the no of rf antennas supported. 

DEPENDENCIES
  None

RETURN VALUE
  Returns an unsigned int representing the no of rf antennas supported.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rfc_get_num_antenna(void)
{
    return (uint8) rf_card.rfc_max_ants;
}


#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*=========================================================================

FUNCTION rfc_get_hdet_comp_value

DESCRIPTION
  This function calculates and returns the compensation value for HDET
  depending on the HDET thermitor temperature and Radio Configuration.

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
int16 rfc_get_hdet_comp_value(rfc_hdet_comp_cfg_type *hdet_comp_config)
{
  return rf_card.rfc_get_hdet_comp_value(hdet_comp_config);
}

/*=========================================================================

FUNCTION rfc_get_hdet_comp_value_for_hdr

DESCRIPTION
  This function calculates and returns the waveform compensation value
  for HDR
   
RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
int16 rfc_get_hdet_comp_value_for_hdr(rfc_hdet_comp_cfg_for_hdr_type *hdet_comp_config_for_hdr)
{
  return rf_card.rfc_get_hdet_comp_value_for_hdr(hdet_comp_config_for_hdr);
}

#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */


/*=========================================================================

FUNCTION RFC_GET_HDET_ADC_CHANNEL

DESCRIPTION
  This function returns the band-dependent ADC channel for HDET on the
  current RF card.

RETURN VALUE
  adc_logical_channel_type - the channel to pass to adc_read() in order
                             to get the HDET reading

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
adc_logical_channel_type rfc_get_hdet_adc_channel( rf_card_band_type band )
{
  return rf_card.rfc_get_hdet_adc_channel( band );
}


#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/*=========================================================================

FUNCTION RFC_USES_INTERNAL_HDET

DESCRIPTION
  This function returns true if the RF card is wired to use the internal
  RF chipset power detector for a given band.  If the card uses an
  external power detector the function returns false.

RETURN VALUE
  boolean - true if the target uses internal HDET for this band

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_uses_internal_hdet( rf_card_band_type band )
{
  return rf_card.rfc_uses_internal_hdet( band );
}
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef RF_HAS_INTERNAL_THERM_SUPPORT
/*=========================================================================

FUNCTION RFC_USES_INTERNAL_THERM

DESCRIPTION
  Query function for chipset platforms to determine if target uses
  internal Chip Thermistor for Temperature readings


RETURN VALUE
  QSC11x0 is true for all bands on the Qualcomm reference platform

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_uses_internal_therm( rf_card_band_type band )
{
  return rf_card.rfc_uses_internal_therm( band );
}

/*=========================================================================

FUNCTION RFC_READ_INTERNAL_THERM

DESCRIPTION
  Reads the internal Thermistor for QSC1100

RETURN VALUE
  Returns the 12 bit HKADC Read Value for Thermistor

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
word rfc_read_internal_therm(rf_card_band_type band)
{
  return rf_card.rfc_read_internal_therm( band );
}
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
/*=========================================================================

FUNCTION RFC_SUPPORT_TEMP_BASED_DEVICE_CONFIG

DESCRIPTION
  Returns TRUE or FALSE depending upon if Device Configration
  needs to be changed based on temperature or not

RETURN VALUE
  TRUE or FALSE

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_support_temp_based_device_config(rf_card_band_type band)
{
  return rf_card.rfc_support_temp_based_device_config( band );
}
#endif

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
/*=========================================================================
FUNCTION RFC_SET_PA_ACCESS_PROBE_CONFIG

DESCRIPTION
This function is called so that RF Driver can program different 
PA hysterisis timerand PA switchpoints for Access Probes different 
from traffic.
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB

DEPENDENCIES
  None

RETURN VALUE
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB


SIDE EFFECTS
  None
===========================================================================*/
int16 rfc_set_pa_access_probe_config(rfcom_device_enum_type device,rf_card_band_type band)
{
   return rf_card.rfc_set_pa_access_probe_config( device,band );
}
#endif


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*=========================================================================
FUNCTION   RFC_INIT_RFCONFIG_RX_LNA

DESCRIPTION
   This function is used for initializing RFConfig database containing Rx LNA
   switchpoints info. Card specific implementation may initialize using Rx
   chipset specific default values, and possibly also with RF NV items
   that contain LNA switchpoint information.

   This function serves as a hook for RF driver to refresh Rx LNA RFConfig
   database whenever the driver loads RF NV items from QCN file.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void rfc_init_rfconfig_rx_lna(void)
{
   rf_card.rfc_init_rfconfig_rx_lna();
}
#endif /* RF_HAS_RFCONFIG_RX_LNA */


#ifdef RF_HAS_SUB_BLOCK_SUPPORT

/*=========================================================================

FUNCTION RFC_GET_BLOCK

DESCRIPTION
This function is called on every band or channel change
Maps the Sub block for Band/Channel Combination
Returns the Sub class Mask for given Band/Channel combination to be tuned
If Block Mask is forced through FTM Mode or in RF CAL it returns the forced block Mask

  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the Block mask Valid for Current Channel as read from NV

SIDE EFFECTS
  None.

===========================================================================*/ 
uint32 rfc_get_block(rf_card_band_type band, word rf_chan)
{
 return rf_card.rfc_get_block( band,rf_chan );
}

/*=========================================================================
FUNCTION RFC_BLOCK_MASK_IS_VALID

DESCRIPTION
This function is called during RF Cal mode to check the validity of block Mask Passed 
Returns if the given Block Mask passed during RF Cal is Valid
If RF Cal's s Block Mask is such that it asks for 2 blocks which are supported on different 
RF Paths(Different duplexer and different Tx SAW) for given Band,this function will return FALSE, 
otherwise the functon returns TRUE
  
DEPENDENCIES
  None.

RETURN VALUE
 Returns TRUE if Block Mask is Valid otherwise returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/ 
boolean rfc_block_mask_is_valid(rf_card_band_type band,uint32 subclass_mask)
{
 return rf_card.rfc_block_mask_is_valid(band,subclass_mask );
}

/*=========================================================================
FUNCTION RFC_SET_BLOCK

DESCRIPTION
Configure the GPIOs for the current Band and Block combination 
  
DEPENDENCIES
  None.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/ 
void rfc_set_block(rfcom_device_enum_type device,rf_card_band_type band,uint32 subclass_mask)
{
  rf_card.rfc_set_block(device,band,subclass_mask );
}

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

/*===========================================================================

FUNCTION RFC_IS_TUNABLE

DESCRIPTION
  This function takes an index to the antenna array and band and returns a boolean TRUE
  if the antenna is tunable, FALSE otherwise. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if the specified antenna is tunable, FALSE otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rfc_is_tunable_on_antenna( int antenna_index,rf_card_band_type band )
{
    return rf_card.rfc_ant_attrib[antenna_index].tunnable[band];
}


/*=========================================================================

FUNCTION rfc_empty_func

DESCRIPTION
  Default (empty) function.  To be assigned to function pointers when the
  function pointer needs to point to a function that does nothing 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_empty_func( void* param1, ... )/*lint -esym(715, param1) -e818 unused */
{
    MSG_LOW( "Default (empty) function called in rfc", 0, 0, 0);
}


/*=========================================================================

FUNCTION rfc_empty_func_return_0

DESCRIPTION
  Default (empty) function.  To be assigned to function pointers when the
  function pointer needs to point to a function that does nothing except 
  return  0.
  

DEPENDENCIES
  None.

RETURN VALUE
  0

SIDE EFFECTS
  None.

===========================================================================*/ 
int16 rfc_empty_func_return_0( void* param1, ... )/*lint -esym(715, param1) -e818 unused */
{
    MSG_LOW( "Default (empty) function called in rfc", 0, 0, 0);
    return 0;
}
#endif /* !FEATURE_GSM_PLT */


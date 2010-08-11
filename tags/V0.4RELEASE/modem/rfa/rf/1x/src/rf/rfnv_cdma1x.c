/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     C D M A 1 X   R F   N V   M a n a g e r

DESCRIPTION
  This module retrieves and stores all of the 1X RF NV items.

EXTERNALIZED FUNCTIONS

rfnv_register_wait_handler
  This function requires a task to register with the RF NV manager so
  that a delay mechanism is established at task level.

rfnv_init_1x_nv_tbl
  This function populates the internal 1X NV database with data stored
  on the NV device.

rfnv_list_size
  This function returns the size of the RF NV items identifier array.
  This array defines the supported RF NV item identifier.

rfnv_get_nv_lst_addr
  Return the address of the RF NV items identifier array.

rfnv_get_nv_data
  This function fills the structure pointed to by the pass-in parameter with
  pointers to the calibrated RF NV item table. There are several RF NV item ta
  based on each supported mode (ie, WCDMA, PCS, CDMA, etc..).
  A NULL RF NV item table pointer indicates that the RF NV item table is unava

void rfnv_read_nv_item
  This function copies one RF NV data item, as specified by "item_code/identif
  into an element of the RF NV item table.
  This function must be called during initialization for each and every RF NV


Copyright (c) 1997 - 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x.c#10 $
$DateTime: 2009/02/19 19:50:03 $
$Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------------
02/19/09   vb/lp   Fixes for RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA
11/07/08   vm      Fixes for BC5 Sub Block Implementation
10/06/08   vb      Functions rfnv_get_band_nv_ptr() and rfnv_get_block_nv_ptr() will pass the address 
                   of default band-class digital items table if requested band/block not found.
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
07/25/08   sl      Added initialization of Common NV through RF_HAS_COMMON_NV_TABLE feature. 
                   This change is necessary for RC Tuner and PWC Cal.
05/20/08   vm      Changed the OLTA Backoff for G4 for QSC1100
04/29/08   sl      ULC: Changed features with ZIF_LIBRARY_SUPPORTS to RF_HAS_SUPPORT_FOR.
04/23/08   sl      Added NV_PROCESS_CAL_I, NV_DC_CAL_I, NV_SUPPLY_I support.
04/16/08   vm      Added Support for BC10 Band Class and OLTA Switchpoints for QSC1100
03/21/08   sl      Added NV_PROCESS_CAL_I support under rfnv_common_items_type.
11/28/07    xw     Updated nv_data_valid_flag per bc config in rfnv_set_card_defaults().
                   Added rfnv_validate_1x_rf_card().
10/20/07   rmd     Provided access to NV_RF_PM_CONFIG_I to the 1X side.
09/12/07   adk     Updated card_device[] table since RF_HW_QRF7600 is changed to 33.
09/11/07   adk     Featurized NV_THERM_BINS_I via RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS.
09/11/07   adk     Add NV_THERM_BINS_I to rfnv_cdma1x_common_items_list[].
09/09/07   vm/dyc  Updated OLTA Backoffs for 1X and DO on Sc2x
08/20/07   ans     OLTA updates for RTR6500.
08/14/07    ka     Added call to calibrate thermistor.
08/02/07   avi     Added support for QRF7600
07/26/07   vm      Added the support for NV for 8 Thermistor bins
07/26/07   adk     Corrected the definition of RF_HW_WDZRF in card_device[]
07/12/07   vm/Vish Fixed the issue of Table size Overflow for tx_cal_chan_tbl
07/11/07   rmd/sar Added support for the TRF6285.
06/20/07   Vish    Added support for RFCONFIG Rx LNA data structures for
                   primary and secondary chains in digital NV items storage
                   on a per band class basis.
06/13/07   adk     Added support for UNDP-1
06/01/07   aak     Removal of JIDZRF card support
04/27/07   aak     Removal of ZRF and JIZRF support
04/08/07   vm      Updated the OLTA switcpoints for HDR as per Ramsis HWSW
03/14/07   vm/dyc  Added the check for reference of NULL Pointer when loading
                   the OLTA switchpoints
                   Programmed the OLTA Switchpoints QSC60x5
03/13/07   dyc     Fix NV compiler warning.
03/09/07   bmg     Added TRF6500 support
03/04/07   dyc     Merge QSC60x5 to common.
           bmg     NV (channel list/OLTA) defaults for QSC60x5
           vm      Added the support for DVGa offset being 1/10th of db resolution
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
12/12/06   hkk     Added support for D-450
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial MSM7600 build.
10/13/06   aak     Removal of support for FZRF
09/28/06   hkk     BC15 support added
09/09/06   hkk     Removal of support for GZRF, KPZRF and CZRF
06/12/06   hkk     Added support for DZRFBC14
06/06/06   hkk     Removed support for ZRF6185.
03/30/06   zg/sar  - Added initial support for ZRF6185.
                   - Replaced a hardcoded number with RF_HW_MAX when defining 
                   card_device.
01/25/06   sar     Lint cleanup.
01/13/06   lp      Added SSBI support for DZRF6500/JTDZRF6500
01/11/06   et      Fixed lint errors.
01/10/06   sar     Fixed lint errors.
12/06/05   sar     Eliminated Lint errors.
11/15/05   rmd     Added support for BC11 within rfnv_set_defaults().
10/13/05   bmg     Added comments for zero cal channel list
10/13/05   rmd     Added support for BC14 within rfnv_set_defaults().
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
08/29/05   bmg     Corrected OLTA values to disable RFR6500 DRX OLTA in 1x.
08/29/05   ra      Added support for JTDZRF
07/25/05   hkk     Updated static tables in rfnv_set_defaults for RFR6525 
07/01/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
07/01/05   sar     Eliminated support for legacy bands.
06/30/05   bmg     Added BC5 OLTA items for RFR6500 to avoid null pointer
                   references.
06/08/05   sar     Fixed Lint warnings and errors.
06/03/05   dyc     Added overide defaults for RF_RFR6500_HDR_G1_IM_* values.
04/26/05   sar     Added function rfnv_get_band_mask().
04/11/05   bmg     Modified RFR6500 OLTA settings to support optimized current
03/31/05   bmg     Added functions rfnv_load_dvga_offset_data and
                   rfnv_load_lna_offset_data to consolidate computation of
                   RFNV RX calibration data
03/21/05   bmg     Added defaults for DZRF6500
02/07/04   dbc     Changed OLTA setting for RFR6175 + BC5 1X to olta_0
02/07/04   dbc     Changed OLTA setting for RFR6175 + BC5 HDR to olta_0
12/14/04   bmg     Added NV defaults for RFR6500 and RFR6150
12/16/04   sar     Added featurized support for NV_VCO_COARSE_TUNE_TABLE_I and 
                   NV_C1_VCO_COARSE_TUNE_TABLE_I for non-febc builds.
12/06/04   sar     Removed support for NV_VCO_COARSE_TUNE_TABLE_I and 
                   NV_C1_VCO_COARSE_TUNE_TABLE_I.
11/16/04   sar     Update rfnv_set_defaults function to set defaults for all bands.
11/11/04   sar     Update OLTA defaults in rfnv_set_defaults().
11/04/04   bmg     Use correct default cal chan list for RFR6000 in BC1
11/03/04   sar     Added the missing break in rfnv_cdma1x_copy_common_nv_items()
                   for NV_RF_HW_CONFIG_I. Also added default value for RF GPS Config.
10/31/04   dyc     Added NV_DIGITAL_PLL_LOCK_TIMER_I to the items list.
10/20/04   sar     Added functions rfnv_set_card_defaults() and rfnv_set_defaults()
                   to support runtime default setting of cal chan and olta items.
09/30/04   sar     Removed function calls to set rf card specific defaults.
09/18/04   dbc     Load BC5 defaults for FEBC builds into pcs and bc1 table
                   to enable use of legacy QCNs 
09/16/04   jac     Fixed the KPCS defaults to update the bc1 table for 
                   non-FEBC builds.
09/16/04   dbc     Added support to set Band Class 5 defaults at runtime
09/15/04   jac     Added support to set KPCS defaults at runtime.
09/02/04   sar     Added support to set Cell-ONLY defaults at runtime.
08/24/04   dyc     Remove RF_HAS_FM_NV_TABLE usage.
07/13/04   jac     Added NV_GPS_RF_CONFIG_I to common items list.
07/09/04   dyc     Added NV_C1_RFR_BB_FILTER_I to items list. 
                   Comment updates.
07/08/04   sar     Fixed a typo for the case of NV_C1_RFR_BB_FILTER_I in
                   rfnv_cdma1x_copy_common_nv_items.
06/08/04   sar     Featurized extraction of NV_RF_HW_CONFIG_I to enable for FEBC.
05/28/04   sar     Updated feature flag names for the nv tables.
05/25/04   sar     added support for legacy cdma & pcs bands.
05/24/04   sar     Added support for PAR MAP support.
04/08/04   sar     Added support for NV_RFR_BB_FILTER_I and 
                   NV_RFR_IQ_LINE_RESISTOR_I. Removed NV_XXX_RF_TUNE_RESERVED_I.
03/19/04   sar     Added support for new band classes.
02/09/04   dbc     Fixed syntax error
01/26/04   SAR     Removed NV Item NV_TX_WARMUP_I.
12/18/03   SAR     Added constant definitions for rfnv_cdma1x_common_tbl.
11/07/03   SAR     Added Support for BC6 items.
10/22/03   SAR     Added function rfnv_set_digital_table() to set the default
                   digital table pointer to the specified band.
10/23/03   SAR     Added Support for NV_C1_VCO_COARSE_TUNE_TABLE_I.
07/29/03   SAR     Initial Revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/
#include "comdef.h"
#include <memory.h>       /* Memory manipulation                            */
#include "msg.h"          /* MSG_XXX() macros                               */
#include "rficap.h"       /* Internal RF Feature definitions                */
#include "nv.h"           /* NV item declarations                           */
#include "rfi.h"
#include "rfnv.h"         /* common rf nv structures                        */
#include "rfnv_cdma1x.h"  /* 1X prototypes and definitions                  */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
extern void rftemp_calibrate_therm( uint8 therm_min, uint8 therm_max );

/* add additional mode include file here */

#ifdef RF_HAS_BC0_NV_TABLE
#include "rfnv_cdma1x_bc0.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc0_tbl;
#endif /*RF_HAS_BC0_NV_TABLE*/

#ifdef RF_HAS_BC1_NV_TABLE
#include "rfnv_cdma1x_bc1.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc1_tbl;
#endif /*RF_HAS_BC1_NV_TABLE*/

#ifdef RF_HAS_BC2_NV_TABLE
#include "rfnv_cdma1x_bc2.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc2_tbl;
#endif /*RF_HAS_BC2_NV_TABLE*/

#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif /*RF_HAS_BC3_NV_TABLE*/

#ifdef RF_HAS_BC4_NV_TABLE
#include "rfnv_cdma1x_bc4.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc4_tbl;
#endif /*RF_HAS_BC4_NV_TABLE*/

#ifdef RF_HAS_BC5_NV_TABLE
#include "rfnv_cdma1x_bc5.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc5_tbl;
#endif /*RF_HAS_BC5_NV_TABLE*/

#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif /*RF_HAS_BC6_NV_TABLE*/

#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif /*RF_HAS_BC11_NV_TABLE*/

#ifdef RF_HAS_BC14_NV_TABLE
#include "rfnv_cdma1x_bc14.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc14_tbl;
#endif /*RF_HAS_BC14_NV_TABLE*/

#ifdef RF_HAS_BC15_NV_TABLE
#include "rfnv_cdma1x_bc15.h"
extern rfnv_digital_items_type rfnv_cdma1x_bc15_tbl;
#endif /*RF_HAS_BC15_NV_TABLE*/


#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif /*RF_HAS_GPS*/

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
#include "rfnv_cdma1x_bcx_block_0.h"
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_0_tbl;
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
#include "rfnv_cdma1x_bcx_block_1.h"
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_1_tbl;
#endif

#endif

#ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION
#define RF_10TH_OF_DB_RESOLUTION_FOR_DVGA_OFFSET_VS_FREQ   1
#else
#error code not present
#endif

#define RF_BAND_CLASS_TABLE_SIZE 16
#define RF_RX_TABLE_SIZE 12
#define RF_TX_TABLE_SIZE 12
/* -----------------------------------------------------------------------
** Mandatory NV items regardless of RF mode
** ----------------------------------------------------------------------- */

rfnv_items_list_type rfnv_cdma1x_common_items_list[] =
{
#ifdef FEATURE_RFCAL_VERSION
    /*  DMSS S/W Version                                                        */
    {   NV_SYS_SW_VER_I,                    NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  RF CAL Program version                                                  */
    {   NV_RF_CAL_VER_I,                    NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  RF CAL configuration file version                                       */
    {   NV_RF_CONFIG_VER_I,                 NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Date RF calibration was done                                            */
    {   NV_RF_CAL_DATE_I,                   NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Date RF calibration data loaded                                         */
    {   NV_RF_NV_LOADED_DATE_I,             NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Name of RFCAL .dat                                                      */
    {   NV_RF_CAL_DAT_FILE_I,               NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Data Services domain name                                               */
#endif
    /*  CAGC dynamic range                                                      */
    {   NV_CDMA_DYNAMIC_RANGE_I,            NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Minimum Rx RSSI                                                         */
    {   NV_CDMA_MIN_RX_RSSI_I,              NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Tunes the RX baseband filter bandwidth                                  */
    {   NV_RFR_BB_FILTER_I,                 NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Tunes the RX baseband filter bandwidth (chain 1)                        */
    {   NV_C1_RFR_BB_FILTER_I,              NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Tunes the value of the baseband resistor                                */
    {   NV_RFR_IQ_LINE_RESISTOR_I,          NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  PA state mapping value                                                  */
    {   NV_PA_R_MAP_I,                      NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  GPS RF config mapping value                                             */
    {   NV_GPS_RF_CONFIG_I,                 NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
    /*  Synth lock iteration count                                              */
    {   NV_DIGITAL_PLL_LOCK_TIMER_I,        NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
    /*  Thermistor binning                                                      */
    {   NV_THERM_BINS_I,                    NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#endif
    /*  RF PMIC configuration */
    {   NV_RF_PM_CONFIG_I,                  NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#ifdef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
    /*  Process Cal */
    {   NV_PROCESS_CAL_I,                   NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#endif //RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
#ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
    /*  DC Cal */
    {   NV_DC_CAL_I,                   		NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#endif //RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
    /*  Supply Value */
    {   NV_SUPPLY_I,                   		NV_ITEM_ATTRIB_NON_CRTCL_BMSK       },
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL
};

/* -----------------------------------------------------------------------
** Common NV calibration table.  Initially contains default
** values.  If these items are not read from NV, the default values are
** used.
** ----------------------------------------------------------------------- */
rfnv_common_items_type rfnv_cdma1x_common_tbl = {

  /* test_code_ver
  */
  RF_TEST_CODE_VER,

  /* sys_sw_ver
  */
  RF_SYS_SW_VER,

  /* rf_cal_ver
  */
  RF_RF_CAL_VER,

  /* rf_config_ver
  */
  RF_CONFIG_VER,

  /* rf_cal_date
  */
  RF_CAL_DATE,

  /* rf_nv_loaded_date
  */
  RF_NV_LOADED_DATE,

  /* rf_cal_dat_file
  */
  RF_CAL_DAT_FILE,

  /* dacc_dc_offset_polarity
  */
  RF_DACC_DC_OFFSET_POLARITY,

  /* rtc_time_adjust
  */
  RF_RTC_TIME_ADJUST,

  /* digital_pll_lock_time 
  */
  RF_DIGITAL_PLL_LOCK_TIME,

  /* minimum rx rssi in 1/10 dB (85 dB dynamic range) 
  */
  RF_MINIMUM_RX_RSSI,

  /* cdma cagc dynamic range in 1/10 dB (85 dB dynamic range) 
  */
  RF_CDMA_CAGC_DYNAMIC_RANGE,
  
  /*  Filter Coarse Tune Table. 
  */
  {RF_RFR_BB_FILTER,RF_RFR_BB_FILTER},
  
  /* 
  Tunes the value of the baseband resistor 
  */
  RF_RFR_IQ_LINE_RESISTOR,

/*
  PA_R_MAP Value
  */  
  RF_PAR_MAP,

  /*
  GPS configuration in RF
  */
  RF_GPS_RF_CONFIG
  
  #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
  /*Thermistor bining*/
  ,RF_THERM_BIN_DEFAULT_V
  #endif

  ,RF_PM_CONFIG_DEFAULT_V

#ifdef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
  /*  Process Cal */
  ,RF_PROCESS_CAL_DEFAULT_V
#endif //RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL

#ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
  /*  DC Cal */
  ,RF_DC_CAL_DEFAULT_V
#endif //RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
  /*  Supply Value */
  ,RF_SUPPLY_VAL_DEFAULT_V
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL
};
/* -----------------------------------------------------------------------
** FM NV calibration table.
** ----------------------------------------------------------------------- */
rfnv_fm_items_type rfnv_cdma1x_fm_tbl; 

/* The 1X NV Driver equires that at least one of the following
   band classes are defined or alternatively rfnv_set_digital_table() 
   is called to explicitly set rfnv_digital_tbl_ptr to specified band
   before rfnv_digital_tabl() function may be used. 
*/
rfnv_digital_items_type *rfnv_digital_tbl_ptr =

#ifdef RF_HAS_BC0_NV_TABLE
    &rfnv_cdma1x_bc0_tbl;
#elif defined( RF_HAS_BC1_NV_TABLE )
    &rfnv_cdma1x_bc1_tbl;
#elif defined( RF_HAS_BC2_NV_TABLE )
    &rfnv_cdma1x_bc2_tbl;
#elif defined( RF_HAS_BC3_NV_TABLE )
#error code not present
#elif defined( RF_HAS_BC4_NV_TABLE )
    &rfnv_cdma1x_bc4_tbl;
#elif defined( RF_HAS_BC5_NV_TABLE )
    &rfnv_cdma1x_bc5_tbl;
#elif defined( RF_HAS_BC6_NV_TABLE )
#error code not present
#elif defined( RF_HAS_BC11_NV_TABLE )
#error code not present
#elif defined( RF_HAS_BC14_NV_TABLE )
    &rfnv_cdma1x_bc14_tbl;
#elif defined( RF_HAS_BC15_NV_TABLE )
    &rfnv_cdma1x_bc15_tbl;
#else
    NULL;
#endif

/* Pointer to the 1X bands */

rfnv_common_items_type *rfnv_common_tbl_ptr = &rfnv_cdma1x_common_tbl;/*lint -e714 not referenced
                                                                        in current configuration */

rfnv_fm_items_type *rfnv_fm_tbl_ptr = &rfnv_cdma1x_fm_tbl;/*lint -e714 not referenced
                                                            in current configuration */

#ifdef RF_HAS_BC0_NV_TABLE
rfnv_digital_items_type *rfnv_cdma_tbl_ptr = &rfnv_cdma1x_bc0_tbl;/*lint -e714 not referenced
                                                                    in current configuration */
#endif

#ifdef RF_HAS_BC1_NV_TABLE
rfnv_digital_items_type *rfnv_pcs_tbl_ptr = &rfnv_cdma1x_bc1_tbl;/*lint -e714 not referenced
                                                                   in current configuration */
#endif

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif

/* Struct to hold device info for the rf card */
typedef struct {
    rfr_device_enum rfr;
    rft_device_enum rft;
} card_devices_type;

static card_devices_type card_device [RF_HW_MAX] =
{
    {NO_RFR,NO_RFT},          /*  0: RF_HW_UNDEFINED   */
    {NO_RFR,NO_RFT},          /*  1: Placeholder       */
    {NO_RFR,NO_RFT},          /*  2: Placeholder       */
    {NO_RFR,NO_RFT},          /*  3: Placeholder       */
    {NO_RFR,NO_RFT},          /*  4: Placeholder       */
    {NO_RFR,NO_RFT},          /*  5: Placeholder       */
    {NO_RFR,NO_RFT},          /*  6: Placeholder       */
    {RFR6500,RFT6150},        /*  7: RF_HW_DZRF        */
    {NO_RFR,NO_RFT},          /*  8: Placeholder       */
    {RFR6525,RFT6150},        /*  9: RF_HW_JTDZRF      */
    {RFR6500,RFT6150},        /* 10: RF_HW_DZRF_SSBI   */
    {RFR6525,RFT6150},        /* 11: RF_HW_JTDZRF_SSBI */
    {NO_RFR,NO_RFT},          /* 12: RF_HW_ZRF6185     */
    {NO_RFR,NO_RFT},          /* 13: RF_HW_GWZRF6250   */
    {NO_RFR,NO_RFT},          /* 14: RF_HW_DGZRF6500   */
    {RFR6500,RFT6150},        /* 15: RF_HW_DZRFBC14    */
    {RFR6175,RFT6170},        /* 16: RF_HW_D450        */
    {RFR6500,RTR6275_1X},     /* 17: RF_HW_WDZRF       */
    {QSC60X5_RX,QSC60X5_TX},  /* 18: RF_HW_QSC6055     */
    {QSC60X5_RX,QSC60X5_TX},  /* 19: RF_HW_QSC6065     */
    {NO_RFR,NO_RFT},          /* 20: Placeholder       */
    {NO_RFR,NO_RFT},          /* 21: Placeholder       */
    {NO_RFR,NO_RFT},          /* 22: Placeholder       */
    {RFR6500,RTR6285_1X},     /* 23: RF_HW_U2          */
    {RTR6500_RX,RTR6500_TX},  /* 24: RF_HW_TRF6500     */
    {RTR6500_RX,RTR6500_TX},  /* 25: RF_HW_TRF6285     */
    {NO_RFR,NO_RFT},          /* 26: Placeholder (RF_HW_PLATFORM_B_GSM_ONLY) */
    {NO_RFR,NO_RFT},          /* 27: Placeholder (RF_HW_QSC6270_MAIN) */
    {NO_RFR,NO_RFT},          /* 28: Placeholder (RF_HW_QSC6270_EU)  */
    {NO_RFR,NO_RFT},          /* 29: Placeholder (RF_HW_QSC6270_US)  */
    {NO_RFR,NO_RFT},          /* 30: Placeholder (RF_HW_QSC6270_JP)  */
    {NO_RFR,NO_RFT},          /* 31: Placeholder (RF_HW_PLATFORM_D_GSM_ONLY) */
    {NO_RFR,NO_RFT},          /* 32: Placeholder (RF_HW_MXC6369)     */
    {RTR6500_RX,RTR6500_TX},  /* 33: RF_HW_QRF7600     */
    {NO_RFR,NO_RFT},          /* 34: Placeholder (RF_HW_PLATFORM_F_US_SMPS) */
    {NO_RFR,NO_RFT},          /* 35: Placeholder (RF_HW_PLATFORM_F_EU_SMPS) */
    {NO_RFR,NO_RFT},          /* 36: Placeholder (RF_HW_PLATFORM_F_JP_SMPS)  */
    {QSC11X0_RX,QSC11X0_TX},  /* 37: RF_HW_QSC11X0_DUAL_BAND */
    {QSC11X0_RX,QSC11X0_TX},  /* 38: RF_HW_QSC11X0_TRI_BAND */
    {NO_RFR,NO_RFT},          /* 39: Placeholder       */
    {NO_RFR,NO_RFT},          /* 40: Placeholder       */
    {NO_RFR,NO_RFT},          /* 41: Placeholder       */
    {NO_RFR,NO_RFT},          /* 42: Placeholder       */
    {NO_RFR,NO_RFT},          /* 43: Placeholder       */
    {NO_RFR,NO_RFT},          /* 44: Placeholder       */
    {NO_RFR,NO_RFT},          /* 45: Placeholder       */
    {NO_RFR,NO_RFT},          /* 46: Placeholder       */
    {NO_RFR,NO_RFT},          /* 47: Placeholder       */
    {NO_RFR,NO_RFT},          /* 48: Placeholder       */
    {NO_RFR,NO_RFT},          /* 49: Placeholder       */
    {NO_RFR,NO_RFT},          /* 50: Placeholder       */
    {NO_RFR,NO_RFT},          /* 51: Placeholder       */
    {NO_RFR,NO_RFT},          /* 52: Placeholder       */
    {NO_RFR,NO_RFT},          /* 53: Placeholder       */
    {NO_RFR,NO_RFT},          /* 54: Placeholder       */
    {NO_RFR,NO_RFT},          /* 55: Placeholder       */
    {NO_RFR,NO_RFT},          /* 56: Placeholder       */
    {NO_RFR,NO_RFT},          /* 57: Placeholder       */
    {NO_RFR,NO_RFT},          /* 58: Placeholder       */
    {NO_RFR,NO_RFT},          /* 59: Placeholder       */
    {NO_RFR,NO_RFT},          /* 60: Placeholder       */
    {NO_RFR,NO_RFT},          /* 61: Placeholder       */
    {NO_RFR,NO_RFT},          /* 62: Placeholder       */
    {NO_RFR,NO_RFT},          /* 63: Placeholder       */
    {NO_RFR,NO_RFT},          /* 64: Placeholder       */
    {NO_RFR,NO_RFT},          /* 65: Placeholder       */
    {NO_RFR,NO_RFT},          /* 66: Placeholder       */
    {NO_RFR,NO_RFT},          /* 67: Placeholder       */
    {NO_RFR,NO_RFT},          /* 68: Placeholder       */
    {NO_RFR,NO_RFT},          /* 69: Placeholder       */
    {QSC11X0_RX,QSC11X0_TX}   /* 70: RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA */
};

static boolean rfnv_1x_rf_card = FALSE; 

/* Internal function prototypes */

void rfnv_set_card_defaults(rf_hw_type hw);
/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_COPY_NV_ITEM

DESCRIPTION
  This function updates the specfied item (item_code) in the rfnv_common_tbl
  with the data supplied via the item_ptr.  This function used to be called
  rfnv_read_nv_item().

DEPENDENCIES
  Note that there must NOT be a dependency on the read order of the NV
  calibration items.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_cdma1x_copy_common_nv_items
(
  nv_items_enum_type item_code,
  nv_item_type *item_ptr
)
{
    word loop_ctr;        /* Loop Counter */    

  switch (item_code) 
  {
    /*------------------------ New RF NV Items -----------------*/

#ifdef FEATURE_RFCAL_VERSION

    case NV_SYS_SW_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rfnv_cdma1x_common_tbl.rfnv_sys_sw_ver[loop_ctr] =
          item_ptr->sys_sw_ver[loop_ctr] ;
      }
      break;
    case NV_RF_CAL_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rfnv_cdma1x_common_tbl.rfnv_rf_cal_ver[loop_ctr] =
          item_ptr->rf_cal_ver[loop_ctr] ;
      }
      break;
    case NV_RF_CONFIG_VER_I:
      for( loop_ctr=0; loop_ctr<NV_SIZE_OF_VERSION; loop_ctr++ )
      {
        rfnv_cdma1x_common_tbl.rfnv_rf_config_ver[loop_ctr] =
          item_ptr->rf_config_ver[loop_ctr] ;
      }
      break;
    case NV_RF_CAL_DATE_I:
      rfnv_cdma1x_common_tbl.rfnv_rf_cal_date = item_ptr->rf_cal_date;
      break;
    case NV_RF_NV_LOADED_DATE_I:
      rfnv_cdma1x_common_tbl.rfnv_rf_nv_loaded_date = item_ptr->rf_nv_loaded_date;
      break;
    case NV_RF_CAL_DAT_FILE_I:
      for( loop_ctr=0; loop_ctr<NV_QC_SERIAL_NUM_LEN; loop_ctr++ )
      {
        rfnv_cdma1x_common_tbl.rfnv_rf_cal_dat_file[loop_ctr] =
          item_ptr->rf_cal_dat_file[loop_ctr] ;
      }
      break;

#endif //FEATURE_RFCAL_VERSION

    case NV_DIGITAL_PLL_LOCK_TIMER_I:
      rfnv_cdma1x_common_tbl.rfnv_digital_pll_lock_timer =
        item_ptr->digital_pll_lock_timer;
      break;
    case NV_CDMA_DYNAMIC_RANGE_I:
      rfnv_cdma1x_common_tbl.rfnv_cdma_dynamic_range = (int2) item_ptr->cdma_dynamic_range;
      break;
    case NV_CDMA_MIN_RX_RSSI_I:
      rfnv_cdma1x_common_tbl.rfnv_cdma_min_rx_rssi = (int2) item_ptr->cdma_min_rx_rssi;
      break;
    case NV_RFR_BB_FILTER_I:
      rfnv_cdma1x_common_tbl.rfnv_rfr_bb_filter[0] = item_ptr->rfr_bb_filter;
      break;

    case NV_C1_RFR_BB_FILTER_I:
      rfnv_cdma1x_common_tbl.rfnv_rfr_bb_filter[1] = item_ptr->c1_rfr_bb_filter;
      break;

    case NV_RFR_IQ_LINE_RESISTOR_I:
      rfnv_cdma1x_common_tbl.rfnv_rfr_iq_line_resistor = item_ptr->rfr_iq_line_resistor;
      break;
    
    case NV_PA_R_MAP_I:
      rfnv_cdma1x_common_tbl.rfnv_pa_r_map = item_ptr->pa_r_map;
      break;

    case NV_GPS_RF_CONFIG_I:
      rfnv_cdma1x_common_tbl.rfnv_gps_rf_config = item_ptr->gps_rf_config;
      break;
  
    #ifdef RF_HAS_NV_SUPPORT_FOR_8_THERM_BINS
    case NV_THERM_BINS_I:
      for (loop_ctr=0; loop_ctr < NV_TEMP_TABLE_SIZ; loop_ctr++)
      {
        rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[loop_ctr] = item_ptr->therm_bins[loop_ctr] ;
      }
	  rftemp_calibrate_therm(rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[0],
							 rfnv_cdma1x_common_tbl.rfnv_therm_bin_type[NV_TEMP_TABLE_SIZ-1]);
      break;
    #endif

    case NV_RF_PM_CONFIG_I:
      rfnv_cdma1x_common_tbl.rf_pm_config = item_ptr->rf_pm_config;
      break;

#ifdef RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL
    case NV_PROCESS_CAL_I:
	  rfnv_cdma1x_common_tbl.rfnv_process_cal = item_ptr->process_cal;
	  break;
#endif //RF_HAS_SUPPORT_FOR_RC_PROCESS_ERROR_CAL

#ifdef RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL
    case NV_DC_CAL_I:
      rfnv_cdma1x_common_tbl.rfnv_dc_cal = item_ptr->dc_cal;
      break;
#endif //RF_HAS_SUPPORT_FOR_DC_OFFSET_CAL

#ifdef RF_HAS_SUPPORT_FOR_LO_PWC_CAL
    case NV_SUPPLY_I:
      rfnv_cdma1x_common_tbl.rfnv_supply_val = item_ptr->supply;
      break;
#endif //RF_HAS_SUPPORT_FOR_LO_PWC_CAL

    default:
      MSG_ERROR( "RF NV item not accounted for: %d", item_code, 0,0 );
      break;
  }
}
/*===========================================================================

FUNCTION RFNV_FM_TABLE                                      REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to FM table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to FM table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_fm_items_type *rfnv_fm_table( void)
{
  return ( &rfnv_cdma1x_fm_tbl);
} /* rfnv_fm_table */


/*===========================================================================

FUNCTION RFNV_DIGITAL_TABLE                                 REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to digital table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to digital table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_digital_items_type *rfnv_digital_table( void)
{
  return rfnv_digital_tbl_ptr;
} /* rfnv_digital_table */


/*===========================================================================

FUNCTION RFNV_COMMON_TABLE                                 REGIONAL FUNCTION

DESCRIPTION
  Returns pointer to common table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to common table.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_common_items_type *rfnv_common_table( void)
{
    return (&rfnv_cdma1x_common_tbl);
}  /* rfnv_common_table */

#ifdef RF_HAS_GPS_NV_TABLE
#error code not present
#endif

/*===========================================================================
FUNCTION RFNV_GET_NV_DATA                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function fills the structure pointed to by the pass-in parameter with
  NV items and pointers to calibration tables.  A NULL pointer indicates that
  the specific calibration table is unavailable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_get_nv_data
(
  rfnv_all_data_type *data_ptr   /* Pointer to the NV data structure */
)
{
   #if defined ( FEATURE_HWTC_ZRF612X_CDMA_EXT_VCO ) || \
       defined ( FEATURE_HWTC_ZRF612X_FM_EXT_VCO )
#error code not present
   #endif

   #ifdef FEATURE_HWTC_ZRF612X_CDMA_EXT_VCO
#error code not present
   #endif

   #ifdef FEATURE_HWTC_ZRF612X_FM_EXT_VCO
#error code not present
   #endif

   data_ptr->rfnv_rfcal_valid       = rfnv_get_cal_stat();

   data_ptr->rfnv_common_items_ptr  = &rfnv_cdma1x_common_tbl;

   data_ptr->rfnv_fm_items_ptr      = &rfnv_cdma1x_fm_tbl;

   #ifdef RF_HAS_BC0_NV_TABLE
   data_ptr->rfnv_cdma_items_ptr    = &rfnv_cdma1x_bc0_tbl;
   #endif

   #ifdef RF_HAS_BC1_NV_TABLE
   data_ptr->rfnv_pcs_items_ptr     = &rfnv_cdma1x_bc1_tbl;
   #endif
   
   #ifdef RF_HAS_BC2_NV_TABLE
   data_ptr->rfnv_bc2_items_ptr     = &rfnv_cdma1x_bc2_tbl;
   #endif
   
   #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
   #endif
   
   #ifdef RF_HAS_BC4_NV_TABLE
   data_ptr->rfnv_bc4_items_ptr     = &rfnv_cdma1x_bc4_tbl;
   #endif
   
   #ifdef RF_HAS_BC5_NV_TABLE
   data_ptr->rfnv_bc5_items_ptr     = &rfnv_cdma1x_bc5_tbl;
   #endif
   
   #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
   #endif

   #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
   #endif

   #ifdef RF_HAS_BC14_NV_TABLE
   data_ptr->rfnv_bc14_items_ptr    = &rfnv_cdma1x_bc14_tbl;
   #endif

   #ifdef RF_HAS_BC15_NV_TABLE
   data_ptr->rfnv_bc15_items_ptr    = &rfnv_cdma1x_bc15_tbl;
   #endif

   #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
   #endif

   #ifdef RF_HAS_SUB_BLOCK_SUPPORT

   #ifdef RF_HAS_BCX_BLOCK_0_NV
   data_ptr->rfnv_bcx_block_0_items_ptr =  &rfnv_cdma1x_bcx_block_0_tbl;
   #endif

   #ifdef RF_HAS_BCX_BLOCK_1_NV
   data_ptr->rfnv_bcx_block_1_items_ptr = &rfnv_cdma1x_bcx_block_1_tbl;
   #endif

   #endif
} /* end of rfnv_get_nv_data */

/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_LIST_SIZE                   EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF Common items NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_common_list_size( void )
{
  return ( ARR_SIZE( rfnv_cdma1x_common_items_list ) );
} /* end of rfnv_cdma1x_common_list_size */


/*===========================================================================

FUNCTION RFNV_CDMA1X_COMMON_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_common_item_table(void)
{
    return ((void *) &rfnv_cdma1x_common_tbl);
}

/*===========================================================================

FUNCTION RFNV_SET_DIGITAL_TABLE                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the default digital table pointer to the specified mode.
  This default pointer is returned by rfnv_digital_table().

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
void rfnv_set_digital_table(rfcom_device_enum_type device, rfnv_1x_band_class_type band_class, word rf_chan)
#else
void rfnv_set_digital_table(rfnv_1x_band_class_type band)
#endif
{
    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rfnv_digital_tbl_ptr = rfnv_get_nv_digital_item_ptr(device, band_class, rf_chan);
    #else
    switch(band)
    {
     
    /* BC 0 - US Cellular  */
    #ifdef RF_HAS_BC0_NV_TABLE
    case RF_BAND_CLASS_0:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc0_tbl;
      break;
    #endif

    /* BC 1 - US PCS */
    #ifdef RF_HAS_BC1_NV_TABLE
    case RF_BAND_CLASS_1:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc1_tbl;
      break;
    #endif
    
    /* BC 2 - JTACS */
    #ifdef RF_HAS_BC2_NV_TABLE
    case RF_BAND_CLASS_2:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc2_tbl;
      break;
    #endif

    /* BC 3 - JCDMA */
    #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
    #endif
    
    /* BC 4 - Korean PCS */
    #ifdef RF_HAS_BC4_NV_TABLE
    case RF_BAND_CLASS_4:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc4_tbl;
      break;
    #endif
    
    /* BC 5 - NMT (450MHz) */
    #ifdef RF_HAS_BC5_NV_TABLE
    case RF_BAND_CLASS_5:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc5_tbl;
      break;
    #endif
    
    /* BC 6 - IMT (2.1GHz) */
    #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
    #endif
    
    /* BC 11 */
    #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
    #endif
    
    /* BC 14 - US PCS + 5 MHz. */
    #ifdef RF_HAS_BC14_NV_TABLE
    case RF_BAND_CLASS_14:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc14_tbl;
      break;
    #endif
        
    /* AWS band */
    #ifdef RF_HAS_BC15_NV_TABLE
    case RF_BAND_CLASS_15:
      rfnv_digital_tbl_ptr = &rfnv_cdma1x_bc15_tbl;
      break;
    #endif
    default:

      MSG_HIGH( "Unsupported band passed in rfnv_set_digital_table: %d", band, 0,0 );
      break;

    }
    #endif/*RF_HAS_SUB_BLOCK_SUPPORT*/
}

/*===========================================================================

FUNCTION RFNV_SET_CARD_DEFAULTS                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the card specific defaults for all the bands.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_set_card_defaults(rf_hw_type hw)
{
  sd_ss_band_pref_e_type rfnv_pri_band_support;

  rfnv_pri_band_support = (sd_ss_band_pref_e_type)rf_get_rf_bc_config(RF_PATH_0);

  rfnv_1x_rf_card = FALSE;

  #ifdef RF_HAS_COMMON_NV_TABLE
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_COMMON, TRUE);
  #endif /*RF_HAS_COMMON_NV_TABLE*/

  #ifdef RF_HAS_BC0_NV_TABLE
  if ((rfnv_pri_band_support & SD_SS_BAND_PREF_BC0_A) ||
      (rfnv_pri_band_support & SD_SS_BAND_PREF_BC0_B) ||
      (rfnv_pri_band_support & SD_SS_BAND_PREF_BC0))
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC0, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc0_tbl,(byte) hw,(byte) RF_BC0_BAND);
  }
  #endif /*RF_HAS_BC0_NV_TABLE*/ 

  #ifdef RF_HAS_BC1_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC1)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC1, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc1_tbl,(byte) hw,(byte) RF_BC1_BAND);
  }
  #endif /*RF_HAS_BC1_NV_TABLE*/

  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif /*RF_HAS_BC3_NV_TABLE*/

  #ifdef RF_HAS_BC10_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC10)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC10, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc10_tbl,(byte) hw,(byte) RF_BC10_BAND);
  }
  #endif /*RF_HAS_BC10_NV_TABLE*/

  #ifdef RF_HAS_BC4_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC4)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC4, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc4_tbl,(byte) hw,(byte) RF_BC4_BAND);
  }
  #endif /*RF_HAS_BC4_NV_TABLE*/

  #ifdef RF_HAS_BC5_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC5)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC5, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc5_tbl,(byte) hw,(byte) RF_BC5_BAND);
  }
  #endif /*RF_HAS_BC5_NV_TABLE*/

  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif /*RF_HAS_BC6_NV_TABLE*/

  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif /*RF_HAS_BC11_NV_TABLE*/

  #ifdef RF_HAS_BC14_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC14)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC14, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc14_tbl,(byte) hw,(byte) RF_BC14_BAND);
  }
  #endif /*RF_HAS_BC14_NV_TABLE*/

  #ifdef RF_HAS_BC15_NV_TABLE
  if (rfnv_pri_band_support & SD_SS_BAND_PREF_BC15)
  {
    rfnv_1x_rf_card = TRUE;
    rfnv_set_nv_data_valid_flag(RFNV_CDMA1X_BC15, TRUE);
    rfnv_set_defaults((void *)&rfnv_cdma1x_bc15_tbl,(byte) hw,(byte) RF_BC15_BAND);
  }
  #endif /*RF_HAS_BC15_NV_TABLE*/

  #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
  #endif
}

/*=================================================================================

FUNCTION    RFNV_SET_DEFAULTS

DESCRIPTION
  This function sets the default values for selected items in band class nv tables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rfnv_set_defaults
(
    void *table_ptr,
    byte rfhw,
    byte rfband
)
{
    
    rfnv_digital_items_type *nv = (rfnv_digital_items_type *) table_ptr;
    rf_hw_type hw = (rf_hw_type)rfhw;
    rf_card_band_type band = (rf_card_band_type) rfband;
    int i;
    word *cur_rx_cal_chan, *cur_tx_cal_chan;
#ifndef RF_HAS_RFCONFIG_RX_LNA
    int1 *cur_olta,*hdr_cur_olta;
#endif //!RF_HAS_RFCONFIG_RX_LNA
    uint16 table_size = 0;

    /* ----------------------------------------------------------------------------------
    CAL CHAN Table 0 applicable to:
    Band Class  -   Devices    
    BC0             RFR6000, RFT6120, RFR6125, RFT6100
    BC1             RFR6000
    BC3             RFR6000, RFT6100
    ------------------------------------------------------------------------------------*/        
    static word cal_chan_0[16] = {1018,46,98,150,202,254,306,358,410,462,514,566,618,670,722,774};
    
    /* ----------------------------------------------------------------------------------
    CAL CHAN Table 1 applicable to:
    Band Class  -   Devices    
    BC5             RFT6100, RFR6135, RFT6170, RFR6175
    ------------------------------------------------------------------------------------*/        
    static word cal_chan_1[16] = {127,137,147,158,168,179,189,199,210,219,228,237,246,255,264,273};
    
    /* ----------------------------------------------------------------------------------
    CAL CHAN Table 2 applicable to:
    Band Class  -   Devices    
    BC1             RFT6100, RFR6135
    BC6             RFR6000, RFT6100, RFR6135
    ------------------------------------------------------------------------------------*/        
    static word cal_chan_2[16] = {25,113,188,263,338,413,488,563,638,713,788,863,938,1013,1088,1175};
    
    /* ----------------------------------------------------------------------------------
    CAL CHAN Table 3 applicable to:
    Band Class      Devices
    BC4             RFT6100, RFR6135
    ------------------------------------------------------------------------------------*/        
    static word cal_chan_3[16] = {22,59,96,133,170,207,244,281,318,355,392,429,466,503,540,577};
    
    /* ----------------------------------------------------------------------------------
    CAL CHAN Table 4 applicable to:
    Band Class      Devices
    Any             RFR6500/RFR6525
    With RF interpolation, the user can select a reduced calibration channel
    list.  RFR6500 (IntelliCeiver) requires RF interpolation, and FTM Cal V2
    may require an empty calibration channel list to fully work as expected.
    Using this empty channel list requires the user to be dilligent about
    correctly initializing the static RF NV. (See 80-V8871-11.)
    ------------------------------------------------------------------------------------*/        
    static word cal_chan_4[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    /* Selected rx cal chan table */
    static word *rx_cal_chan_tbl[RF_RX_TABLE_SIZE][RF_BAND_CLASS_TABLE_SIZE] = {
    /* 
                   BC0         BC1         BC2   BC3         BC4         BC5         BC6         BC7   BC8   BC9   BC10  BC11         BC12  BC13  BC14        BC15 */
    /* RFR6125 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFR6000 */{ cal_chan_0, cal_chan_0, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_0, cal_chan_0},
    /* RFR6175 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFR6135 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFR6500 */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
/* RFR6500_DRX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
    /* RFR6525 */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
/* RFR6525_DRX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
    /* RFR6155 */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* QSC60X5_RX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* RTR6500_RX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* QSC11X0_RX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, cal_chan_4, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
    };                                                                                                                                                                  
    
    /* Selected rx cal chan table */
    static word *tx_cal_chan_tbl[RF_TX_TABLE_SIZE][RF_BAND_CLASS_TABLE_SIZE] = {
    /* 
                   BC0         BC1         BC2   BC3         BC4         BC5         BC6         BC7   BC8   BC9   BC10  BC11         BC12  BC13  BC14        BC15 */
    /* RFT6100 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFT6120 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFT6102 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFT6170 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RFT6150 */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
    /* RTR6300 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
    /* RTR6350 */{ cal_chan_0, cal_chan_2, NULL, cal_chan_0, cal_chan_3, cal_chan_1, cal_chan_2, NULL, NULL, NULL, NULL, cal_chan_1 , NULL, NULL, cal_chan_2, cal_chan_2},
 /* RTR6275_1X */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* RTR6285_1X */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* QSC60X5_TX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* RTR6500_TX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, NULL, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
 /* QSC11X0_TX */{ cal_chan_4, cal_chan_4, NULL, cal_chan_4, cal_chan_4, cal_chan_4, cal_chan_4, NULL, NULL, NULL, cal_chan_4, cal_chan_4 , NULL, NULL, cal_chan_4, cal_chan_4},
    };
    
#ifndef RF_HAS_RFCONFIG_RX_LNA
    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 0 applicable to:
    Band Class      Devices
    BC0             RFR6000, RFR6125
    BC1             RFR6000
    BC3             RFR6000
    BC5             RFR6135, RFR6175
    ------------------------------------------------------------------------------------*/        
    static int1 olta_0[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ (int1)0x80,(int1)0x80,0,0,
                       /* Chan 1 */          0,         0,0,0 
                     };
    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 1 applicable to:
    Band Class      Devices
    BC1             RFR6135
    BC4             RFR6135
    BC6             RFR6000, RFR6135
    ------------------------------------------------------------------------------------*/        
    static int1 olta_1[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ (int1)0x80,0,0,0,
                       /* Chan 1 */          0,0,0,0
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 2 applicable to:
    Band Class      Devices
    BC1             RFR6000
     ------------------------------------------------------------------------------------*/        
    static int1 olta_2[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */          0,0,0,0,
                       /* Chan 1 */          0,0,0,0
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 3 applicable to:
    Band Class      Devices
    BC0  (1x)       RFR6500
    BC1  (1x)       RFR6500
    BC3  (1x)       RFR6500
    BC4  (1x)       RFR6500
    BC6  (1x)       RFR6500
    BC0  (1x)       RTR6500_RX (High Lin, Low Lin)
    BC3  (1x)       RTR6500_RX (High Lin, Low Lin)
    BC10 (1x)       RTR6500_RX (High Lin, Low Lin)
    BC1  (1x)       RTR6500_RX (High Lin, Low Lin)
    BC4  (1x)       RTR6500_RX (High Lin, Low Lin)
    BC6  (1x)       RTR6500_RX (High Lin, Low Lin)
    BC14 (1x)       RTR6500_RX (High Lin, Low Lin)
    BC15 (1x)       RTR6500_RX (High Lin, Low Lin)
    ------------------------------------------------------------------------------------*/        
    static int1 olta_3[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ -128, -128,  -7,   -7,
                       /* Chan 1 */ -128, -128,  -7,   -7
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 4 applicable to:
    HDR
    Band Class      Devices
    BC1  (1xEV-DO)  RFR6500
    BC3  (1xEV-DO)  RFR6500
    BC4  (1xEV-DO)  RFR6500
    BC6  (1xEV-DO)  RFR6500
    BC0  (1xEV-DO)  RTR6500_RX (Low Lin)
    BC3  (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
    BC10 (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
    ------------------------------------------------------------------------------------*/        
    static int1 olta_4[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */  -7,   -7,   -7,   -7,
                       /* Chan 1 */  -7,   -7,   -7,   -7
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 5 applicable to:
    Band Class      Devices
    BC0  (1xEV-DO)  RFR6500
    BC0  (1xEV-DO)  RTR6500_RX (High Lin)
    ------------------------------------------------------------------------------------*/        
    static int1 olta_5[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ -128, -128,  -7,   -7,
                       /* Chan 1 */  -7,   -7,   -7,   -7
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 6 applicable to:
    Band Class      Devices
    BC0  (1x)       QSC60X5_RX   
    BC3  (1x)       QSC60X5_RX
    BC10 (1x)       QSC60X5_RX  
    ------------------------------------------------------------------------------------*/        
    static int1 olta_6[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ -128, -128,  67,   60,
                       /* Chan 1 */ -128, -128,  74,   69
                     };

     /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 7 applicable to:
    Band Class      Devices
    BC1  (1x)       QSC60X5_RX   
    BC4  (1x)       QSC60X5_RX
    BC6  (1x)       QSC60X5_RX       
    BC14  (1x)      QSC60X5_RX   
    BC15  (1x)      QSC60X5_RX
    ------------------------------------------------------------------------------------*/        
    static int1 olta_7[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */ -128, -128,  50,   50,
                       /* Chan 1 */ -128, -128,  69,   64
                     };

     /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 8 applicable to:
    Band Class            Devices
    BC3   (1xEV-DO)       QSC60X5_RX   
    BC10  (1xEV-DO)       QSC60X5_RX    
    ------------------------------------------------------------------------------------*/        
    static int1 olta_8[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */   72, 71, 67, 60,
                       /* Chan 1 */   64, 54, 74, 69 
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 9 applicable to:
    Band Class            Devices
    BC0   (1xEV-DO)       QSC60X5_RX 
    ------------------------------------------------------------------------------------*/        
    static int1 olta_9[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */   -128, -128, 67, 60,
                       /* Chan 1 */    64,   54,  74, 69
                     };

    /* ----------------------------------------------------------------------------------
    OLTA BACKOFF Table 10 applicable to:
    Band Class           Devices
    BC1  (1xEV-DO)       QSC60X5_RX   
    BC4  (1xEV-DO)       QSC60X5_RX
    BC6  (1xEV-DO)       QSC60X5_RX       
    BC14  (1xEV-DO)      QSC60X5_RX   
    BC15  (1xEV-DO)      QSC60X5_RX
    ------------------------------------------------------------------------------------*/        
    static int1 olta_10[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                     { /* Chan 0 */   -128,  40,  50, 50,
                       /* Chan 1 */    66,   54,  69, 64
                     };

     /* ----------------------------------------------------------------------------------
     OLTA BACKOFF Table 11 applicable to:
     Band Class      Devices
     BC1  (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
     BC4  (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
     BC6  (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
     BC14 (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
     BC15 (1xEV-DO)  RTR6500_RX (High Lin, Low Lin)
     ------------------------------------------------------------------------------------*/        
     static int1 olta_11[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                      { /* Chan 0 */ -128, -7,  -7,   -7,
                        /* Chan 1 */  -7,   -7,   -7,   -7
                      };


     static int1 olta_12[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                      { /* Chain 0 */ -128, -128,  67,  -128,
                        /* Chain 1 */ -128, -128, -128,-128
                      };


     static int1 olta_13[RF_PATHS_TABLE_SIZ * RF_MAX_LNA_STAGES] = 
                      { /* Chain 0 */ -128, -128,  50,   -128,
                        /* Chain 1 */ -128, -128,  -128,   -128
                      };

    /* Selected olta table */
    static int1 *olta_tbl[RF_RX_TABLE_SIZE][RF_BAND_CLASS_TABLE_SIZE]= 
    {
    /* 
                   BC0     BC1     BC2   BC3     BC4    BC5     BC6      BC7   BC8   BC9   BC10  BC11    BC12  BC13  BC14    BC15 */
    /* RFR6125 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_1, olta_1, NULL, NULL, NULL, NULL, olta_1, NULL, NULL, olta_1, olta_1},
    /* RFR6000 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_1, olta_1, NULL, NULL, NULL, NULL, olta_1, NULL, NULL, olta_1, olta_1},
    /* RFR6175 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_0, olta_1, NULL, NULL, NULL, NULL, olta_0, NULL, NULL, olta_1, olta_1},
    /* RFR6135 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_1, olta_1, NULL, NULL, NULL, NULL, olta_1, NULL, NULL, olta_1, olta_1},
    /* RFR6500 */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
/* RFR6500_DRX */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
    /* RFR6525 */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
/* RFR6525_DRX */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
    /* RFR6155 */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
 /* QSC60X5_RX */ {olta_6, olta_7, NULL, olta_6, olta_7, NULL,   olta_7, NULL, NULL, NULL, olta_6,NULL , NULL, NULL, olta_7, olta_7},
 /* RTR6500_RX */ {olta_3, olta_3, NULL, olta_3, olta_3, olta_3, olta_3, NULL, NULL, NULL, NULL, olta_3, NULL, NULL, olta_3, olta_3},
  /* QSC11X0_RX */ {olta_12,olta_13, NULL, NULL, NULL,olta_12, olta_13, NULL, NULL, NULL, olta_12,olta_12, NULL, NULL,NULL, NULL},
    };


    /* Selected hdr olta table */
    static int1 *hdr_olta_tbl[RF_RX_TABLE_SIZE][RF_BAND_CLASS_TABLE_SIZE]= 
    {
    /* 
                   BC0     BC1     BC2   BC3     BC4     BC5     BC6     BC7   BC8   BC9   BC10  BC11    BC12  BC13  BC14   BC15*/
    /* RFR6125 */ {olta_0, olta_2, NULL, olta_2, olta_2, olta_2, olta_2, NULL, NULL, NULL, NULL, olta_2, NULL, NULL, olta_2, olta_1},
    /* RFR6000 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_1, olta_1, NULL, NULL, NULL, NULL, olta_1, NULL, NULL, olta_1, olta_1},
    /* RFR6175 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_0, olta_1, NULL, NULL, NULL, NULL, olta_0, NULL, NULL, olta_1, olta_1},
    /* RFR6135 */ {olta_0, olta_1, NULL, olta_2, olta_1, olta_1, olta_1, NULL, NULL, NULL, NULL, olta_1, NULL, NULL, olta_1, olta_1},
    /* RFR6500 */ {olta_5, olta_4, NULL, olta_4, olta_4, olta_4, olta_4, NULL, NULL, NULL, NULL, olta_4, NULL, NULL, olta_4, olta_3},
/* RFR6500_DRX */ {olta_5, olta_4, NULL, olta_4, olta_4, olta_4, olta_4, NULL, NULL, NULL, NULL, olta_4, NULL, NULL, olta_4, olta_3},
    /* RFR6525 */ {olta_5, olta_4, NULL, olta_4, olta_4, olta_4, olta_4, NULL, NULL, NULL, NULL, olta_4, NULL, NULL, olta_4, olta_3},
/* RFR6525_DRX */ {olta_5, olta_4, NULL, olta_4, olta_4, olta_4, olta_4, NULL, NULL, NULL, NULL, olta_4, NULL, NULL, olta_4, olta_3},
    /* RFR6155 */ {olta_5, olta_4, NULL, olta_4, olta_4, olta_4, olta_4, NULL, NULL, NULL, NULL, olta_4, NULL, NULL, olta_4, olta_3},
 /* QSC60X5_RX */ {olta_9, olta_10,NULL, olta_8, olta_10, NULL,  olta_10,NULL, NULL, NULL, olta_8,NULL,  NULL, NULL, olta_10, olta_10},
 /* RTR6500_RX */ {olta_5, olta_11,NULL, olta_4, olta_11, NULL,  olta_11,NULL, NULL, NULL, olta_4,NULL,  NULL, NULL, olta_11, olta_11},
   /* QSC11X0_RX */ {olta_6, olta_7, NULL, NULL, NULL,    olta_6, olta_7, NULL, NULL, NULL, olta_6,olta_6, NULL, NULL,NULL, NULL},
    };
#endif //!RF_HAS_RFCONFIG_RX_LNA
    
    /* This assert checks if we have a new rf card defined in rf_cards.h, but the 
       card_device table has not been updated to reflect this. */
    table_size = sizeof(card_device)/sizeof(card_devices_type);
    ASSERT( (uint16)hw < table_size );

    /* This assert checks if we have a new band class defined but the 
       rx calibration channel table has not been updated to reflect this. */
    table_size = sizeof(rx_cal_chan_tbl[card_device[0].rfr])/sizeof(word*);
    ASSERT( (uint16)band < table_size );

    /* This assert checks if we have a new band class defined but the 
       tx calibration channel table has not been updated to reflect this. */
    table_size = sizeof(tx_cal_chan_tbl[card_device[0].rft])/sizeof(word*);
    ASSERT( (uint16)band < table_size );

#ifndef RF_HAS_RFCONFIG_RX_LNA
    /* This assert checks if we have a new band class defined but the 
       optimize LNA threshold algorithm table has not been updated to reflect this. */
    table_size = sizeof(olta_tbl[card_device[0].rfr])/sizeof(int1 *);
    ASSERT( (uint16)band < table_size );

    /* This assert checks if we have a new band class defined but the 
       HDR optimize LNA threshold algorithm table has not been updated to reflect this. */
    table_size = sizeof(hdr_olta_tbl[card_device[0].rfr])/sizeof(int1 *);
    ASSERT( (uint16)band < table_size );
#endif //!RF_HAS_RFCONFIG_RX_LNA

    if((rf_hw_type)rfhw>RF_HW_UNDEFINED && (rf_hw_type)rfhw<RF_HW_MAX)
    {
      cur_rx_cal_chan = (word *) rx_cal_chan_tbl[card_device[hw].rfr][band];
      cur_tx_cal_chan = (word *) tx_cal_chan_tbl[card_device[hw].rft][band];
#ifndef RF_HAS_RFCONFIG_RX_LNA
      cur_olta = (int1 *) olta_tbl[card_device[hw].rfr][band];
      hdr_cur_olta = (int1 *) hdr_olta_tbl[card_device[hw].rfr][band];
#endif //!RF_HAS_RFCONFIG_RX_LNA
       
      for(i=0;i<16;i++)
      {
        if(cur_rx_cal_chan!=NULL)
        {
            nv->rfnv_rx_cal_chan[0][i] = 
            nv->rfnv_rx_cal_chan[1][i] = 
                                    cur_rx_cal_chan[i];
        }
                                         
        if(cur_tx_cal_chan!=NULL)
        {
            nv->rfnv_tx_cal_chan[0][i] =
            nv->rfnv_tx_cal_chan[1][i] =
                                    cur_tx_cal_chan[i];
        }
      }
      
#ifndef RF_HAS_RFCONFIG_RX_LNA
      /* Make sure the tables have been assigned */
      if( cur_olta == NULL)
      {
        RF_FATAL_ERROR("CDMA 1X OLTA switchpoints are referencing to NULL");
        return;
      }
      if (hdr_cur_olta == NULL)
      {
        RF_FATAL_ERROR(" HDR OLTA switchpoints are referencing to NULL");
        return;
      }

      for(i=0;i<4;i++)
      {
#ifdef RF_HAS_POWER_MODE_LIB
        nv->rfnv_olta_backoff[0][RFLIB_NORMAL_POWER_MODE][i] = cur_olta[i];
        nv->rfnv_olta_backoff[1][RFLIB_NORMAL_POWER_MODE][i] = cur_olta[i+4];
        nv->rfnv_hdr_olta_backoff[0][RFLIB_NORMAL_POWER_MODE][i] = hdr_cur_olta[i];
        nv->rfnv_hdr_olta_backoff[1][RFLIB_NORMAL_POWER_MODE][i] = hdr_cur_olta[i+4];

        nv->rfnv_olta_backoff[0][RFLIB_LOW_POWER_MODE_1][i] = cur_olta[i];
        nv->rfnv_olta_backoff[1][RFLIB_LOW_POWER_MODE_1][i] = cur_olta[i+4];

        if((band == RF_BC0_BAND) && (card_device[hw].rfr == QSC60X5_RX))
        {
          nv->rfnv_hdr_olta_backoff[0][RFLIB_LOW_POWER_MODE_1][i] = olta_8[i];
          nv->rfnv_hdr_olta_backoff[1][RFLIB_LOW_POWER_MODE_1][i] = olta_8[i+4];
        }
        else if ( (band == RF_BC0_BAND) && (card_device[hw].rfr == RTR6500_RX) ) 
        {
          nv->rfnv_hdr_olta_backoff[0][RFLIB_LOW_POWER_MODE_1][i] = olta_4[i];
          nv->rfnv_hdr_olta_backoff[1][RFLIB_LOW_POWER_MODE_1][i] = olta_4[i+4];
        }
        else
        {
          nv->rfnv_hdr_olta_backoff[0][RFLIB_LOW_POWER_MODE_1][i] = hdr_cur_olta[i];
          nv->rfnv_hdr_olta_backoff[1][RFLIB_LOW_POWER_MODE_1][i] = hdr_cur_olta[i+4];
        }

        nv->rfnv_olta_backoff[0][RFLIB_LOW_POWER_MODE_2][i] = cur_olta[i];
        nv->rfnv_olta_backoff[1][RFLIB_LOW_POWER_MODE_2][i] = cur_olta[i+4];
        /* Low power mode 2 is special for HDR, it is fully OLTA'ed */
        nv->rfnv_hdr_olta_backoff[0][RFLIB_LOW_POWER_MODE_2][i] = olta_4[i];
        nv->rfnv_hdr_olta_backoff[1][RFLIB_LOW_POWER_MODE_2][i] = olta_4[i+4];
#else
        nv->rfnv_olta_backoff[0][i] = cur_olta[i];
        nv->rfnv_olta_backoff[1][i] = cur_olta[i+4];
        nv->rfnv_hdr_olta_backoff[0][i] = hdr_cur_olta[i];
        nv->rfnv_hdr_olta_backoff[1][i] = hdr_cur_olta[i+4];
#endif /* RF_HAS_POWER_MODE_LIB */
      }
    
      /* 
      * There are different BC0 HDR IM RISE, FALL and LEVEL NV defaults depending 
      * on the RFR type.  The compiled default is the RFR6000 value.
      * Need to update with the RFR6500 defaults if RFR6500 is used.
      */
      if ((( hw == RF_HW_DZRF ) || ( hw == RF_HW_DZRF_SSBI )) && ( band == RF_BC0_BAND ))
      {
          nv->rfnv_hdr_im_rise  = (int1) RF_RFR6500_HDR_IM_RISE_V;
          nv->rfnv_hdr_im_fall  = (int1) RF_RFR6500_HDR_IM_FALL_V;
          nv->rfnv_hdr_im_level = (int1) RF_RFR6500_HDR_IM_LEVEL_V;
      }
#endif //!RF_HAS_RFCONFIG_RX_LNA


      #ifdef RF_HAS_RFCONFIG_RX_LNA
      // reset RFCONFIG Rx LNA info structures
         #ifdef RF_HAS_MULTI_RX_CHAIN
      rfconfig_rx_lna_reset(&nv->rfconfig_rx_lna_info[RF_PATH_0]);
      rfconfig_rx_lna_reset(&nv->rfconfig_rx_lna_info[RF_PATH_1]);
         #else /* RF_HAS_MULTI_RX_CHAIN */      
      rfconfig_rx_lna_reset(&nv->rfconfig_rx_lna_info[0]);
         #endif /* RF_HAS_MULTI_RX_CHAIN */
      #endif /* RF_HAS_RFCONFIG_RX_LNA */
    } /* rfhw<RF_HW_MAX */
    
    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    nv->rfnv_block_mask = RF_BLOCK_MASK_DEFAULT_VALUE;
    nv->block_item_ptr = NULL;
    #endif
}


/*===========================================================================

FUNCTION RFNV_LOAD_DVGA_OFFSET_DATA                         INTERNAL FUNCTION

DESCRIPTION
  This function copies DVGA offset vs frequency data from an NV
  structure into the internal RF driver NV structures.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_load_dvga_offset_data( PACKED int1 nv_offsets[],
                                 int2 *flat_rfnv_table_ptr,
                                 int2 base_offset, int count )
{
  int i;

  for (i = 0; i < count; i++)
  {
    flat_rfnv_table_ptr[i] = (RF_10TH_OF_DB_RESOLUTION_FOR_DVGA_OFFSET_VS_FREQ  * nv_offsets[i]) + base_offset;
  }
}


/*===========================================================================

FUNCTION RFNV_LOAD_LNA_OFFSET_DATA                          INTERNAL FUNCTION

DESCRIPTION
  This function copies LNA offset vs frequency data from an NV
  structure into the internal RF driver NV structures.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_load_lna_offset_data( PACKED int1 nv_offsets[],
                                int2 *flat_rfnv_table_ptr,
                                int2 base_offset, int count )
{
  int i;

  for (i = 0; i < count; i++)
  {
    flat_rfnv_table_ptr[i] = nv_offsets[i] + base_offset;
  }
}

/*===========================================================================

FUNCTION RFNV_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None
===========================================================================*/
sys_band_mask_type rfnv_get_band_mask(void)
{
  #ifdef T_QUASAR_X
#error code not present
  #else
  return (sys_band_mask_type) rf_get_rf_bc_config(RF_PATH_0);
  #endif
}

/*===========================================================================

FUNCTION RFNV_VALIDATE_1X_RF_CARD                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns whether the RF card supports 1x. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfnv_validate_1x_rf_card(void)
{
  return rfnv_1x_rf_card; 
}

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
/*===========================================================================

FUNCTION RFNV_GET_BAND_NV_PTR                    Internal FUNCTION

DESCRIPTION

Returns the Pointer to the NV item table of the Band Class passed as
Parameter. If the Band Class Passed is not supported then
the NV Pointer returned is NULL

DEPENDENCIES
  
RETURN VALUE
  Returns the Pointer to the corresponding Band Class NV

SIDE EFFECTS
  None

===========================================================================*/
rfnv_digital_items_type* rfnv_get_band_nv_ptr(rf_card_band_type band)
{
  rfnv_digital_items_type* nv_digital_item_ptr = NULL;

  switch(band)
  {
  #ifdef RF_HAS_BC0_NV_TABLE
  case RF_BC0_BAND:    
    nv_digital_item_ptr = &rfnv_cdma1x_bc0_tbl;              
    break;
    #endif

  #ifdef RF_HAS_BC1_NV_TABLE
  case RF_BC1_BAND:    
    nv_digital_item_ptr = &rfnv_cdma1x_bc1_tbl;
    break;          
  #endif

  #ifdef RF_HAS_BC2_NV_TABLE
  case RF_BC2_BAND:    
    nv_digital_item_ptr = &rfnv_cdma1x_bc2_tbl;
    break;
  #endif

  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC4_NV_TABLE
  case RF_BC4_BAND:    
    nv_digital_item_ptr = &rfnv_cdma1x_bc4_tbl;
    break;
  #endif

  #ifdef RF_HAS_BC5_NV_TABLE
  case RF_BC5_BAND:    
    nv_digital_item_ptr = &rfnv_cdma1x_bc5_tbl;
    break;
  #endif

  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC10_NV_TABLE
  case RF_BC10_BAND :
    nv_digital_item_ptr = &rfnv_cdma1x_bc10_tbl;
    break;
  #endif

  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif

  #ifdef RF_HAS_BC14_NV_TABLE
  case RF_BC14_BAND:
    nv_digital_item_ptr = &rfnv_cdma1x_bc14_tbl;
    break;
  #endif

  #ifdef RF_HAS_BC15_NV_TABLE
  case RF_BC15_BAND:
    nv_digital_item_ptr = &rfnv_cdma1x_bc15_tbl;
    break;
  #endif

  #ifdef RF_HAS_GPS_NV_TABLE
#error code not present
  #endif

  default:
    nv_digital_item_ptr = NULL;
    break;

  }/*switch()*/  

 if(nv_digital_item_ptr == NULL)
   {
     MSG_ERROR( "DEBUG_SUB_BLOCK rfnv_get_band_nv_ptr() returned null for Band: %d ",band, 0, 0 );
     nv_digital_item_ptr = &rfnv_cdma1x_bc0_tbl;
     return nv_digital_item_ptr;

   }

    return (nv_digital_item_ptr);
}

/*===========================================================================

FUNCTION RFNV_GET_BLOCK_NV_PTR                    INTERNAL FUNCTION

DESCRIPTION
Sets the pointer to the Block NV Table of the corresponding Block
Passed as Parameter.

DEPENDENCIES
Block Mask Should have been appropriately configured and 
this function should be called after rfnv_retrieve_nv_items

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
rfnv_digital_items_type* rfnv_get_block_nv_ptr(rf_card_blocks_type block)
{
  rfnv_digital_items_type* nv_digital_items_ptr = NULL;

  switch(block)
  {
  #ifdef RF_HAS_BCX_BLOCK_0_NV
  case RF_BAND_BLOCK_0:
    if(rfnv_cdma1x_bcx_block_0_tbl.rfnv_block_mask != 0)
    {
      nv_digital_items_ptr = &rfnv_cdma1x_bcx_block_0_tbl; 
    }
    break;
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
  case RF_BAND_BLOCK_1:
    if(rfnv_cdma1x_bcx_block_1_tbl.rfnv_block_mask != 0)
    {
      nv_digital_items_ptr = &rfnv_cdma1x_bcx_block_1_tbl; 
    }
    break;
  #endif/*switch(block)*/

  }

 if(nv_digital_items_ptr == NULL)
   {
     MSG_ERROR( "DEBUG_SUB_BLOCK rfnv_get_block_nv_ptr() returned null for Block: %d ",block, 0, 0 );
     nv_digital_items_ptr = &rfnv_cdma1x_bc0_tbl;
     return nv_digital_items_ptr;

   }

  return nv_digital_items_ptr;
}

/*===========================================================================

FUNCTION RFNV_UPDATE_BAND_DIGITAL_ITEMS_PTR               INTERNAL FUNCTION

DESCRIPTION
Updates the rf_digital_nv_table[band] where band is the parameter
passed to the function.
Also updates the rf_bcx_item_table_ptr where "x" represents the Band Class
to point to correct NV Table to be used.

DEPENDENCIES
None

RETURN VALUE


SIDE EFFECTS
Updates the NV Table Pointers and then these will be used all through.  

===========================================================================*/
void rfnv_update_band_digital_items_ptr(rfcom_device_enum_type device,rf_card_band_type band, rfnv_digital_items_type *digital_item_ptr)
{
  //extern rfnv_all_data_type rf_nv_data;
  
  rf_path_enum_type path = rfcom_to_path[ device ];

  rf_digital_nv_table[path][band] = digital_item_ptr;

  /*switch(band)
  {
  #ifdef RF_HAS_BC0_NV_TABLE
  case RF_BC0_BAND:    
    rf_nv_data.rfnv_cdma_items_ptr = digital_item_ptr;              
    break;
  #endif

  #ifdef RF_HAS_BC1_NV_TABLE
  case RF_BC1_BAND: 
    rf_nv_data.rfnv_pcs_items_ptr = digital_item_ptr;              
    break;
  #endif

  #ifdef RF_HAS_BC2_NV_TABLE
  case RF_BC2_BAND:    
    rf_nv_data.rfnv_bc2_items_ptr = digital_item_ptr;              
    break;
  #endif

  #ifdef RF_HAS_BC3_NV_TABLE
  case RF_BC3_BAND:    
    rf_nv_data.rfnv_bc3_items_ptr = digital_item_ptr;              
    break;
  #endif

  #ifdef RF_HAS_BC4_NV_TABLE
  case RF_BC4_BAND:        
    rf_nv_data.rfnv_bc4_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC5_NV_TABLE
  case RF_BC5_BAND:    
    rf_nv_data.rfnv_bc5_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC6_NV_TABLE
  case RF_BC6_BAND:
   rf_nv_data.rfnv_bc6_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC10_NV_TABLE
  case RF_BC10_BAND:
    rf_nv_data.rfnv_bc10_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC11_NV_TABLE
  case RF_BC11_BAND:
    rf_nv_data.rfnv_bc11_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC14_NV_TABLE
  case RF_BC14_BAND:
    rf_nv_data.rfnv_bc14_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_BC15_NV_TABLE
  case RF_BC15_BAND:
    rf_nv_data.rfnv_bc15_items_ptr = digital_item_ptr;                    
    break;
  #endif

  #ifdef RF_HAS_GPS_NV_TABLE
  case RF_GPS_BAND:
    rf_nv_data.rfnv_gps_items_ptr = digital_item_ptr;                    
    break;
  #endif

  }*//*switch()*/  
}

#endif


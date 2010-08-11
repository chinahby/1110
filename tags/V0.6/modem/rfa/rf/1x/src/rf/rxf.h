#ifndef RXF_H
#define RXF_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        RECEIVER FRONT END DRIVER HEADER FILE

GENERAL DESCRIPTION
  This header file defines the functions to drive the Receiver 
  Front End Block.  Any Receiver Front End Block driver has to implement all 
  the interface functions declared in this file in order to be used by any 
  modules/files in DMSS.

RXF FRONT END DRIVER INTERFACE FUNCTIONS

    
    rxf_init
        This function initializes and configures the RX Front End. This function 
        must be called before invoking any other API functions.
    
    rxf_reset
        This function resets the  RX Front End Block by writing to the RXF_RESET
    
    set_rxf_test_mode
        This function sets the rxf test mode. rxf_test_mode_type parameter can have 
        either of the following values:
            rxf_test_mode_0 ; I and Q get test_bus1 and test_bus2
            rxf_test_mode_1 ; I gets test_bus1 & "00000000", Q gets test_bus2 
                               and "00000000"
            rxf_test_mode_2 ; I gets test_bus1 sign extended, Q gets test_bus2 
                              sign extended
    
    set_test_mode
        This function selects the iput source for Y1 and Y2. 
        
    rxf_set_mode
        This function sets the mode of operation for the RX Front End Driver.


    disable_rxf_front
        This function sets rxf_bypass bit to bypass the entire rx_front, by 
        injecting digital I and Q into the IQMUX.

    enable_rxf_front
        This function clears rxf_bypass bit and the rx_front is included in the 
        data path.


    rxf_sleep
        This function puts rxf in the sleep mode. 
    
    rxf_wakeup
        This function transitions the rxf to active state from sleep mode. 
    

Copyright (c) 2001, 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004, 2005       by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007             by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rxf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/07   zg      Added c++ support for all functions in the header file.
03/04/07   dyc     Added support for QSC60x5
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
12/19/06   adk     Merged changes from 6800 Rel 6: Added rxf_hdr_wakeup() prototype.
11/09/06   adk     Deleted "#define MODEM_GENERIC_CTL_TRK_LO_ADJ_SEL_*"
                   since "TRK_LO_ADJ_SEL" bit no longer exists in
                   "MODEM_GENERIC_CTL" register.
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial MSM7600 build.
08/16/06   kss     Added rxf_hdr_wakeup().
06/12/07   hkk     Added proto for rxf_chain_int  
12/07/05   hkk     Stub functions to make Libraries compatible 
09/23/05   dyc     Added path parameter to rxf_init()
09/07/05   bmg     Added rxf_set_group_delay() function.
08/17/05   ra      changed rf_bypass_rtc_ctl_on_off to handle device and on/off
08/13/05   ra      added support for bypass rtc(enable/disable)
05/09/05   ra      For DORA, added RF_HAS_MODEM_GENERIC_CTL
03/16/05   bmg     Removed IntelliCeiver DC loop functions that will not
                   be used.
01/05/05   ra      DORA merge.
10/29/04   ans     Removed RXF_RF_DACC_MULT_PRESCALER_GPS_V to support different 
                   settings based on RFR chip.
09/01/04   sar     Added function void rxf_set_rx_polarity().
11/27/04   sar     Defined constant values for mismatch comp.
11/05/03   dyc     Added RXF_RESET_WAIT_US
11/05/03   sar     Reverted back the defaults in the last change and updated
                   values for RXF_RF_GPS_EST_TRC_OFFSET_SCALER_V and 
                   RXF_RF_GPS_FG_TRC_OFFSET_SCALER_V.
11/05/03   sar     Updated values for RXF_RF_GPS_EST_ACQ_OFFSET_SCALER_V and
                   RXF_RF_GPS_FG_ACQ_OFFSET_SCALER_V per hw/sw doc.
10/15/03   et      removed unnecessary comments
10/14/03   dyc/dbc Updates for RXFc_OFFSET_CTL, RXFc_DACC_CTL and DACCc_ACQ_TIMER 
10/01/03   dyc     Cleanup: replace parm name chain with path for consistency.
08/10/03   dyc     Updated EST_TRC_OFFSET_SCALER values from 0x6 to 0x4
07/31/03   dyc     New setting for RXF_RF_MODE_GPS from 0x11 to 0x3
07/30/03   dyc     Additions for GPS mode settings.
07/30/03   bmg     Correct fix for LNA state 4 added.
06/18/03   et      cleaned up more magic numbers
06/15/03   kss     Added values to support HDR.
06/11/03   et      added more values
06/05/03   et      updated some #defines and added RXF_RF_ infront of all the 
                   _V values inside T_MSM6500
05/19/03   dyc     added DVGA_GAIN defs
05/13/03    et     added value definitions for msm6500 values
04/14/03    et     merged with MSM6100 3030 build
03/31/03    et     added rxf0_panda_close function
02/05/03    zg     Added definition for RXF_DACC_TRK_WAIT_TIME.
01/22/03   dyc     Mainline DACC_MULT_PRESCALER to 0 (.25) for CELL and AMPS.
01/14/03   dbc     Added support for different DACC_MULT_PRESCALER values in 
                   CDMA/FM modes for RFR6125
12/20/02   dbc     Added rxf_program_digital_mode().  Changed rxf_set_mode_cdma()
                   and rxf_set_mode_pcs() to take a boolean parameter. 
11/25/02   dbc     Added support for RFR6125
11/05/02   dyc     Support for RF_HAS_BYPASS_MIS_COMP, RF_HAS_DIGITAL_VGA_DELAY
10/29/02   sar     Set IQ_OFFSET_PDM_OE_DIS_V and DIGITAL_VGA_DELAY_XXX_V values
                   directly to 0x0. The SHIFT_FROM_MASK macro for these 
                   definitions was causing compiler warnings.
10/26/02   dbc     DIGITAL_VGA_DELAY_MASK only defined for MSM6100. Set to 0x0
                   for MSM6000/MSM6050 
10/24/02   dyc     Support for T_MSM6100B.
                   Corrected DIGITAL_VGA_DELAY_MASK typo.
09/10/02   dbc     Changed value of DACC_CLR_TIMER for GPS mode. 
09/09/02   dbc     Changed the DACC_CLR_TIMER value for all modes.
09/04/02   dbc     Removed legacy code.
08/16/02   dyc     Update DFM est_acq and est_trc offset_scaler values
08/13/02    zg     Updated VGA_GAIN_SCALE_GPS to be consistent with CDMA 102 dB
                   settings.
07/26/02   dyc     Added prototype for rxf_set_dacc_update().
                   Updated EST_ACQ/TRC_OFFSET_SCALER values to match 
                   FG_ACQ/TRC_OFFSET SCALER values. 
                   Removed VGA_GAIN_SCALE defines for CDMA
                   (merged from zg,sar,dbc from MSM6000_RF.01.00.14-15)

07/17/02   dyc     Removed 102 dB dynamic range changes.  This is now detected
                   at run time.
                   (merged from dbc rf.01.00.13)
07/15/02   dyc     Added prototype for rxf_set_mode_gps_preswitch().
                   (merged from zg rf.01.00.12)
06/30/02   dyc     Cougar Port baseline.
                   Provide mask and definitions for new DACC_CTL bit sbi_en.
                   Provide mask for new DACC_CTL separate I/Q gain entries.

06/24/02   sar     Updated DACC_EST_THRESH values for all modes.
06/12/02   dbc     Added prototypes for rxf_cdma_wakeup() and 
                   rxf_cdma_wakeup_dc_acquire()
06/06/02   dbc     Register changes for rev2 of MSM6050
05/22/02   dbc     Added changes for 102 dB dynamic range in PCS/CDMA
05/09/02   dyc     Update DFM values est scalers and dac thresh
                   Set DFM_IF_OFFSET AMPS low IF setting, set to 0xE666
04/30/02   dyc     Backout DFM_IF_OFFSET AMPS low IF setting, set to 0xE000 
04/29/02   sar     updated the values for cdma and pcs mode gain correction.
04/24/02   sar     Removed outdated comments. Removed unused enum rxf_mode_proc_type.
                   Removed unused defines MAX_MODES_ALLOWED & CURRENT_RXF_MODE.
04/17/02   dbc     Changed DFM_IF_OFFSET to 0xe666
04/10/02   sar     Removed frivilous definitions. Made a correction in the
                   RXF_ACQ_WAIT_TIME_GPS macro.
03/29/02    zg     Increased gps daccest_thresh to 0xE000000.
03/22/02   dyc     Updated DFM and CDMA values.  cdma dacc_est_thrsh=0x4000000
03/21/02   sar     Added prototypes for functions rxf_set_vga_gain_offset()
                   rxf_save_dc_offset(), and rxf_restore_dc_offset().
03/21/02   aks     Export the dc_offset()functions.
03/21/02   aks     Moved some definitions into the Library.
03/18/02   sar     Removed prototypes for PDM Clock change functions.
03/14/02   sar     Defined values for items that were converted from NV items
                   to Hard-coded values. 
02/28/02   sar     Updated vaules of DC_CORRECTION_CDMA_V and DC_CORRECTION_PCS_V.
02/28/02   dyc     Update vga_gain_scale for DFM.  Support 102.4 dynamic range.
02/22/02    zg     Replaced FEATURE_GPSONE with RF_HAS_GPS.Added rxf_clear_accum(). 
02/12/02   sar     Changed definition of PDM_TRC_ACQ_ENA_V and IQ_OFFSET_PDM_OE_ENA_V
                   to eliminate ads1.01 compiler warnings.
02/08/02   dyc     Hardcode FG_TRC_OFFSET_SCALER_DFM_V for QRF5600
02/06/02   sar     Changed T_MSM_6050 flag to T_MSM6050.
02/06/02   dyc     Updated DFM/ZIF default values.
01/31/02   zg      Fixed feature name for gpsOne.
01/29/02   sar     Removed frivolous definitions of items that moved to NV.
01/18/02   dyc     Updates fg_trc_offset and dfm_if_offset for QRF5600 DFM.   
01/15/02   sar     Added protocols for rxf_wakeup,rxf_wakeup_delayed,rxf_dc_acquire, 
                   rxf_dc_track,rxf_set_pdm_clk_digital and rxf_set_pdm_clk_tcx0
                   to support digital and analog sleep modes in RX Front.
12/21/01   aks     Changed values, to reflect configuration which makes the zif
                   calls.
12/11/01   sar     Updated ZIF parameters as indicated by HW SW RadioOne Document.
12/7/01    sar     Changed DACC_GAIN_MULT value to 0x3 instead of 0x7 per Christian.
11/29/01   sar     Removed temporary data structures supporting NV access code.
11/19/01   sar     Added protypes for GetRxfIDCOffset, GetRxfQDCOffset and
                   SetVgaGainOffset. Updated static values per latest release
                   of RadioOne document.
11/12/01   sar     Added ZIF specific values for ZIF coefficients
10/12/01   sar     revised redefinitions of mask and values. Also added shifting 
                   of values to the right position.
09/10/01   sar     Created.

===========================================================================*/

#include "comdef.h"
#include "err.h"
#include "msm.h"
#include "clk.h"
#include "db.h"
#include "rficap.h"
#include "rf.h"
#include "enc.h"

#include "rflib.h"

#include "rf_hal.h"

/* ======== REDEFINITIONS OF RXF FRONT END REGISTERS AND MASKS =========== */

#define RXF_RF_MICRO_VREF_OVERIDE_ENA_V 0x1
#define RXF_RF_MICRO_VREF_OVERIDE_DIS_V 0x0

#define RXF_RF_MICRO_DACC_ENA_V 0x1
#define RXF_RF_MICRO_DACC_DIS_V 0x0

#define RXF_RF_MICRO_MIX_RANGE_OVERIDE_ENA_V 0x1
#define RXF_RF_MICRO_MIX_RANGE_OVERIDE_DIS_V 0x0

#define RXF_RF_MICRO_LNA_RANGE_OVERIDE_ENA_V 0x1
#define RXF_RF_MICRO_LNA_RANGE_OVERIDE_DIS_V 0x0

#define RXF_RF_DACC_GCH_UPDATE_ENA_V 0x1
#define RXF_RF_DACC_GCH_UPDATE_DIS_V 0x0

#define RXF_RF_DACC_PRD_UPDATE_ENA_V 0x1
#define RXF_RF_DACC_PRD_UPDATE_DIS_V 0x0

#define RXF_RF_DACC_DFT_UPDATE_ENA_V 0x1
#define RXF_RF_DACC_DFT_UPDATE_DIS_V 0x0

#define RXF_RF_EST_ACCUM_CLR_ENA_V 0x1
#define RXF_RF_EST_ACCUM_CLR_DIS_V 0x0

#define RXF_RF_EST_OFFSET_ACCUM_ENA_V 0x1
#define RXF_RF_EST_OFFSET_ACCUM_DIS_V 0x0

#define RXF_RF_DACC_MULT_PRESCALER_QTR_V 0x0
#define RXF_RF_DACC_MULT_PRESCALER_HALF_V 0x1
#define RXF_RF_DACC_MULT_PRESCALER_ONE_V 0x2
#define RXF_RF_DACC_MULT_PRESCALER_TWO_V 0x3

#define RXF_RF_DACC_MULT_PRESCALER_FM_V   RXF_RF_DACC_MULT_PRESCALER_QTR_V
#define RXF_RF_DACC_MULT_PRESCALER_CDMA_V RXF_RF_DACC_MULT_PRESCALER_QTR_V
#define RXF_RF_DACC_MULT_PRESCALER_PCS_V  RXF_RF_DACC_MULT_PRESCALER_HALF_V
// FOR GPS RXF_RF_DACC_MULT_PRESCALER_GPS_V  look rfr.c and rxf.c

#define RXF_RF_CDMA_EST_ACQ_OFFSET_SCALER_V 0x8
#define RXF_RF_PCS_EST_ACQ_OFFSET_SCALER_V 0x8
#define RXF_RF_DFM_EST_ACQ_OFFSET_SCALER_V 0xC
#define RXF_RF_GPS_EST_ACQ_OFFSET_SCALER_V 0xC

#define RXF_RF_CDMA_EST_TRC_OFFSET_SCALER_V 0x4
#define RXF_RF_PCS_EST_TRC_OFFSET_SCALER_V 0x4
#define RXF_RF_DFM_EST_TRC_OFFSET_SCALER_V 0x4
#define RXF_RF_GPS_EST_TRC_OFFSET_SCALER_V 0x6

#define RXF_RF_DACC_DC_OFFSET_POLARITY_NORM_V 0x0
#define RXF_RF_DACC_DC_OFFSET_POLARITY_INV_V 0x1

#ifdef RF_HAS_DACC_REG_ENHANCE
#define RXF_RF_DACC_CTL_SBI_ENA_V 0x1
#define RXF_RF_DACC_CTL_SBI_DIS_V 0x0
#endif /* RF_HAS_DACC_REG_ENHANCE */

#define RXF_RF_FG_ACCUM_CLR_ENA_V 0x1
#define RXF_RF_FG_ACCUM_CLR_DIS_V 0x0

#define RXF_RF_FG_OFFSET_ACCUM_ENA_V 0x1
#define RXF_RF_FG_OFFSET_ACCUM_DIS_V 0x0

#define RXF_RF_CDMA_FG_ACQ_OFFSET_SCALER_V 0x6
#define RXF_RF_PCS_FG_ACQ_OFFSET_SCALER_V 0x6
#define RXF_RF_GPS_FG_ACQ_OFFSET_SCALER_V 0xC

#define RXF_RF_CDMA_FG_TRK_OFFSET_SCALER_V 0x6
#define RXF_RF_PCS_FG_TRK_OFFSET_SCALER_V 0x6
#define RXF_RF_GPS_FG_TRK_OFFSET_SCALER_V 0x6

#define RXF_RF_CDMA_RXF_DC_IOFFSET_ADJ_V 0x0
#define RXF_RF_PCS_RXF_DC_IOFFSET_ADJ_V 0x0
#define RXF_RF_GPS_RXF_DC_IOFFSET_ADJ_V 0x0

#define RXF_RF_CDMA_RXF_DC_QOFFSET_ADJ_V 0x0
#define RXF_RF_PCS_RXF_DC_QOFFSET_ADJ_V 0x0
#define RXF_RF_GPS_RXF_DC_QOFFSET_ADJ_V 0x0

#define RXF_RF_MICRO_MIX_RANGE_V 0x0
#define RXF_RF_MICRO_LNA_RANGE_V 0x3

#define RXF_RF_RTC_LOAD_V 0x0

#define RXF_RF_DF2_DC_OFFSET_V 0x0
#define RXF_RF_DACC_TRC_TIMER_V 0xFFF
 
#define RXF_RF_GAIN_CORRECTION_CDMA_V 0x88
#define RXF_RF_GAIN_CORRECTION_PCS_V 0x88
#define RXF_RF_GAIN_CORRECTION_GPS_V 0xB5

#define RXF_RF_GAIN_MUL_FACTOR_CDMA_V 0x0
#define RXF_RF_GAIN_MUL_FACTOR_PCS_V 0x0
#define RXF_RF_GAIN_MUL_FACTOR_GPS_V 0x3

#define RXF_RF_GAIN_TRUNC_WIDTH_CDMA_V 0x2
#define RXF_RF_GAIN_TRUNC_WIDTH_PCS_V 0x2
#define RXF_RF_GAIN_TRUNC_WIDTH_GPS_V 0x3

#define RXF_RF_DC_CORRECTION_CDMA_V 0x03F
#define RXF_RF_DC_CORRECTION_PCS_V 0x03F
#define RXF_RF_DC_CORRECTION_GPS_V 0x001

#define RXF_RF_CDMA_RXF_FG_IOFFSET_V 0x0
#define RXF_RF_PCS_RXF_FG_IOFFSET_V 0x0
#define RXF_RF_GPS_RXF_FG_IOFFSET_V 0x0

#define RXF_RF_CDMA_RXF_FG_QOFFSET_V 0x0
#define RXF_RF_PCS_RXF_FG_QOFFSET_V 0x0
#define RXF_RF_GPS_RXF_FG_QOFFSET_V 0x0

#define RXF_RF_DFM_IF_OFFSET_CDMA_V 0x0000
#define RXF_RF_DFM_IF_OFFSET_PCS_V 0x0000
#define RXF_RF_DFM_IF_OFFSET_GPS_V 0x0000

#define RXF_RF_DACC_EST_THRESH_CDMA_V 0x8000000
#define RXF_RF_DACC_EST_THRESH_PCS_V 0x8000000
#define RXF_RF_DACC_EST_THRESH_GPS_V 0x3BA00000

#define RXF_RF_MICRO_VGA_GAIN_V 0x0

/* RXFc_CTL */
#ifdef RF_HAS_BYPASS_MIS_COMP
#define RXF_RF_BYPASS_MIS_COMP_ENA_V 0x1
#define RXF_RF_BYPASS_MIS_COMP_DIS_V 0x0
#endif

/* RXFc_CTL:RXF_MODE */
#ifndef RF_TODO_7600_ADK
#define RXF_MODE_DFM_V  0x00
#endif
#define RXF_MODE_CDMA_V 0x01
#define RXF_MODE_HDR_V  0x02

#ifndef RF_TODO_7600_ADK
#define RXF_MODE_GSM_V  0x03
#endif

#ifdef RF_HAS_RXF_V2
/*RXF_SRCc_DVGA_PARAMS:VGA_MODE*/
#define RXF_RF_VGA_MODE_CDMA_V 0x0
#define RXF_RF_VGA_MODE_HDR_V  0x1
#endif

/* RXFc_DVGA_CTL:DIGITAL_VGA_GAIN_SEL */
#define RXF_RF_DIGITAL_VGA_GAIN_SEL_CDMA_V 0x1
#define RXF_RF_DIGITAL_VGA_GAIN_SEL_PCS_V 0x1
#define RXF_RF_DIGITAL_VGA_GAIN_SEL_DFM_V 0x0
#define RXF_RF_DIGITAL_VGA_GAIN_SEL_GPS_V 0x1
#define RXF_RF_DIGITAL_VGA_GAIN_SEL_HDR_V 0x2

#define RXF_RF_VGA_GAIN_OFFSET_CDMA_V 0x8FC
#define RXF_RF_VGA_GAIN_OFFSET_PCS_V 0x8FC
#define RXF_RF_VGA_GAIN_OFFSET_GPS_V 0x8FC

#define RXF_RF_VGA_GAIN_SCALE_CDMA_V 0x6
#define RXF_RF_VGA_GAIN_SCALE_PCS_V 0x6
#define RXF_RF_VGA_GAIN_SCALE_DFM_V 0x6
#define RXF_RF_VGA_GAIN_SCALE_GPS_V 0x6

#define RXF_RF_DACC_ACQ_COUNT_V 0x00

#define RXF_RF_CDMA_DACC_ACQ_TIME_V 0x3F
#define RXF_RF_PCS_DACC_ACQ_TIME_V 0x3F
#define RXF_RF_GPS_DACC_ACQ_TIME_V 0x10

#define RXF_RF_CDMA_DACC_CLR_TIME_V 0x13
#define RXF_RF_PCS_DACC_CLR_TIMER_V 0x13
#define RXF_RF_GPS_DACC_CLR_TIMER_V 0x10

#define RXF_RF_DACC_GAIN_MULT_CDMA_V 0x1C
#define RXF_RF_DACC_GAIN_MULT_PCS_V 0x1C
#define RXF_RF_DACC_GAIN_MULT_GPS_V 0x03

#define RXF_RF_DACC_GAIN_MULT_TOTAL_PCS_V 0x1c1c1c1c
#define RXF_RF_DACC_GAIN_MULT_TOTAL_CDMA_V 0x1c1c1c1c

#define RXF_RF_RESET_HIGH_V 0x1
#define RXF_RF_RESET_LOW_V 0x0

#define RXF_RF_MICRO_GAIN_OVERRIDE_N_ENA_V 0x1
#define RXF_RF_MICRO_GAIN_OVERRIDE_N_DIS_V 0x0

#define RXF_RF_ASYNC_SAMP_CLK_SEL_ENA_V 0x1
#define RXF_RF_ASYNC_SAMP_CLK_SEL_DIS_V 0x0

#define RXF_RF_DIGITAL_VGA_BYPASS_V 0x1
#define RXF_RF_DIGITAL_VGA_NON_BYPASS_V 0x0

#define RXF_RF_ZIF_SEL_ENA_V 0x1
#define RXF_RF_ZIF_SEL_DIS_V 0x0

#define RXF_RF_BYPASS_CORDIC_ENA_V 0x1
#define RXF_RF_BYPASS_CORDIC_DIS_V 0x0

#ifndef RF_TODO_7600_ADK
#define RXF_RF_BYPASS_DF2_ENA_V 0x1
#define RXF_RF_BYPASS_DF2_DIS_V 0x0

#define RXF_RF_BYPASS_GPS_MAT_FILT_ENA_V 0x1
#define RXF_RF_BYPASS_GPS_MAT_FILT_DIS_V 0x0
#endif

#define RXF_RF_MICRO_FLC_GAIN_OVERRIDE_ENA_V 0x1

#define RXF_RF_MICRO_DACC_UPDATE_V 0x1

#define RXF_RF_MODE_CDMA_PCS_V 0x01

#ifndef RF_TODO_7600_ADK
#define RXF_RF_MODE_GPS_V      0x3
#endif

/* these are needed for the ACQ_WAIT_TIMEs */
#define RXF_CG_LNA_ACQ_TIME_CDMA_V 0x024EU
#define RXF_CG_LNA_ACQ_TIME_PCS_V 0x024EU
#define RXF_CG_LNA_ACQ_TIME_DFM_V 0x000BU
#define RXF_CG_LNA_ACQ_TIME_GPS_V 0x01EBU
#define RXF_CG_LNA_ACQ_TIME_MAX_V 0xFFFFU

#define RXF_RF_DACC_EST_IOFFSET_CLR_V          0
#define RXF_RF_DACC_EST_QOFFSET_CLR_V          0

#define RXF_RF_CTL_ALL_DIS_V        0x0000000
#define RXF_RF_PDM_RESET_HIGH_V  0x01
#define RXF_RF_PDM_RESET_LOW_V   0x00

#define RXF_RF_BYPASS_ENA_V 0x1
#define RXF_RF_BYPASS_DIS_V 0x0
#define RXF_RF_TEST_INPUT_MODE_TOM_V 0x0
#define RXF_RF_TEST_INPUT_MODE_GPIO_V 0x1
#define RXF_RF_TEST_INPUT_MODE_TSTBUS_V 0x2

#define RXF_RF_LNA_FILTER_OVERRIDE_CLR 0x0
#define RXF_RF_LNA_RANGE_RILT_SEL_CLR 0x0

#ifndef RF_HAS_RXF_V2
#error code not present
#endif /* RF_HAS_RXF_V2 */        

/* RX_AGCc_MODE_SEL:AGC_MODE */        
#define RX_AGCc_MODE_SEL_DISABLE_V 0x0
#define RX_AGCc_MODE_SEL_1X_V      0x1
#define RX_AGCc_MODE_SEL_HDR_HLC_V 0x2
#define RX_AGCc_MODE_SEL_HDR_FLC_V 0x3


/*
The acquisition wait time in uSec for each mode is defined as 
    CG_LNA_ACQ_TIME + 30 uSec. 
The the bit resolution for each mode is
    .814 uSec (CDMA)
    .978 uSec (GPS)
    44.44 uSec (DFM)
Therefore the wait time for each mode is defined as follows:
*/

#define RXF_ACQ_WAIT_TIME_CDMA          \
        (( RXF_CG_LNA_ACQ_TIME_CDMA_V * 0.814) +  30)
#define RXF_ACQ_WAIT_TIME_PCS           \
        (( RXF_CG_LNA_ACQ_TIME_PCS_V *  0.814) +  30)
#define RXF_ACQ_WAIT_TIME_DFM           \
        (( RXF_CG_LNA_ACQ_TIME_DFM_V *  44.44) +  30)
#define RXF_ACQ_WAIT_TIME_GPS           \
        (( RXF_CG_LNA_ACQ_TIME_GPS_V *  0.978) +  30)

#define  RXF_DACC_TRK_WAIT_TIME     100 
          
#define  RXF_RESET_WAIT_US          30

/* Mismatch compensation values
*/
#define  RXF_MIS_COMP_A_OFFSET_V        0x0
#define  RXF_MIS_COMP_B_OFFSET_V        32768

#ifdef  RF_HAS_MODEM_GENERIC_CTL

#define MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_M   0x1
#ifndef RF_TODO_7600_ADK
#define MODEM_GENERIC_CTL_TRK_LO_ADJ_SEL_M         0x2
#endif
#define MODEM_GENERIC_CTL_MDSP_IRQ_SEL_M           0x4
#define MODEM_GENERIC_CTL_HDEM_TESTBUS_M           0x40
#define MODEM_GENERIC_CTL_MDSP_IRQ_TEST_M          0x80

#define MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_0_V 0x0
#define MODEM_GENERIC_CTL_TX_AGC_CTL_SEL_CHAIN_1_V 0x1
#ifndef RF_TODO_7600_ADK
#define MODEM_GENERIC_CTL_TRK_LO_ADJ_SEL_CDMA_1X_V 0x0
#define MODEM_GENERIC_CTL_TRK_LO_ADJ_SEL_HDR_V     0x2
#endif
#define MODEM_GENERIC_CTL_MDSP_IRQ_SEL_HDR_V       0x0
#define MODEM_GENERIC_CTL_MDSP_IRQ_SEL_GSM_V       0x4
#define MODEM_GENERIC_CTL_HDEM_TESTBUS_ENABLE_V    0x40
#define MODEM_GENERIC_CTL_HDEM_TESTBUS_DISABLE_V   0x0
#define MODEM_GENERIC_CTL_MDSP_IRQ_TEST_ENABLE_V   0x80
#define MODEM_GENERIC_CTL_MDSP_IRQ_TEST_DISABLE_V  0x0

#endif  /* RF_HAS_MODEM_GENERIC_CTL */

#ifdef RF_HAS_RXF_V2_DEBUG
#error code not present
#endif /* RF_HAS_RXF_V2_DEBUG */

typedef struct 
{
int  bypass_imc;
int  bypass_mis_comp;
int  bypass_dcc;
int  bypass_bbf;
int  bypass_src;
int  bypass_rxf;
int  bypass_cordic;
int  bypass_dvga;
int  micro_gain_override_n;
int  spectral_inversion;
int  dacc_ctl;
int  dc_ioffset_adj;
int  dc_qoffset_adj;
int  rx_agc_lna_ctl;
int  rx_agc_lna_range;
int  rx_agc_decision;
int  grey_map_enable;
int  grey_map;
int  micro_vga_gain;
int  vga_gain_offset;
int  vga_mode;
int  vga_add_en;
} rxf_values_type;

#ifdef RF_HAS_GPS
#error code not present
#endif


/* ========================== FUNCTION PROTOTYPES ==================== */

#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION rxf_init

DESCRIPTION
  This function initializes the  RX Front End Block for operation.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rxf_init(rf_path_enum_type path);

/*===========================================================================

FUNCTION rxf_clear_accum

DESCRIPTION
  This function clears the RX Front End DACC accumulator.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rxf_clear_accum(rf_path_enum_type path);

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
void rxf_reset(rf_path_enum_type path);

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
                       rf_card_band_type band);

/*===========================================================================

FUNCTION rxf_set_mode_hdr

DESCRIPTION
  This function programs the RX Front for HDR mode.  Current implementation
  requires that RX front was previously set up for 1x in the desired band.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_set_mode_hdr(rf_path_enum_type path, 
                      rf_card_band_type band);


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
void rxf_hdr_wakeup(rf_path_enum_type path);


/*===========================================================================

FUNCTION rxf_program_cdma_mode

DESCRIPTION
  This function programs the RX Front to either CDMA or PCS mode w/o 
  enabling the RX Front clock regimes.  DC acquisition is also not 
  performed.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rxf_program_mode_cdma(rf_path_enum_type path, 
                           rf_card_band_type band);

/*===========================================================================

FUNCTION rxf_set_mode_dfm

DESCRIPTION
  This function initializes the dfm specific parameters.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
void rxf_set_mode_dfm(void);

#ifdef RF_HAS_GPS
#error code not present
#endif /*RF_HAS_GPS*/



/*===========================================================================

FUNCTION rxf_sleep

DESCRIPTION
    This function puts rxf in the sleep mode. 
    
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
void rxf_sleep(rf_path_enum_type path);

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
void rxf_cdma_wakeup(rf_path_enum_type path);

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
  None.
=========================================================================*/
void rxf_cdma_wakeup_dc_acquire(rf_path_enum_type path);



/*===========================================================================

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
void rxf_load_accum_data(rf_path_enum_type path);

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
void rxf_save_accum_data(rf_path_enum_type path);

/*============================================================================

FUNCTION RXF_LOAD_VGA_GAIN_OFFSET

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
void rxf_set_vga_gain_offset( rf_path_enum_type path, byte rf_freq_index );

#ifdef RF_HAS_GPS
#error code not present
#endif /*RF_HAS_GPS*/

/*===========================================================================

FUNCTION rxf_disable_dacc

DESCRIPTION
  This function disables the dacc
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
 dacc is disabled.
===========================================================================*/
void rxf_disable_dacc(rf_path_enum_type path);

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
void rxf_set_rx_polarity(rf_path_enum_type path, enc_rx_polarity_type rx_polarity );

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
void rxf_set_dc_gain( rf_path_enum_type path, rf_card_band_type band );

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
void rxf_bypass_rtc_ctl_path_on_off( rf_path_enum_type path, boolean rxf_bypass_rtc_on_off );

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
void rxf_set_group_delay( rf_path_enum_type path, rf_card_band_type band );

/*===========================================================================

FUNCTION RXF_ENABLE_DACC

DESCRIPTION
  This function enables the DACC - This is a stub so that RF libraries can 
  be kept same across MSMs.

DEPENDENCIES
  RX Front clocks must be on.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_enable_dacc( rf_path_enum_type path );

/*===========================================================================

FUNCTION RXF_DISABLE_DACC

DESCRIPTION
  This function disables the DACC

DEPENDENCIES
  RX Front clocks must be on.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rxf_disable_dacc( rf_path_enum_type path );

/*===========================================================================

FUNCTION rxf_is_dacc_disabled

DESCRIPTION
  This function checks if DACC is disabled. This is a stub so that RF libraries can 
  be kept same across MSMs    

DEPENDENCIES
  RX Front clocks must be on.
 
RETURN VALUE
  TRUE - if DACC is disabled
  FALSE - if DACC is enabled
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean rxf_is_dacc_disabled( rf_path_enum_type path ); 

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
void rxf_chain_init( rf_path_enum_type path );

#ifdef RF_HAS_RXF_V2
/*===========================================================================

FUNCTION rxf_settings_for_rumi_sc2x

DESCRIPTION
  This function programs the RX Front settings for SC2X RUMI. 
  Takes the RF chain as an argument.
      
DEPENDENCIES
  None.
 
RETURN VALUE
 None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rxf_settings_for_rumi_sc2x(byte chain);
#endif

#ifdef RF_HAS_RXF_V2_DEBUG
#error code not present
#endif

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
void rxf_enable_rx_front_clocks_sc2x(rf_path_enum_type path);

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
void rxf_disable_rx_front_clocks_sc2x(rf_path_enum_type path);

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

void rf_freeze_fg_dc_loop_accum(rf_path_enum_type path);

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

void rf_unfreeze_fg_dc_loop_accum(rf_path_enum_type path);

#endif /*RF_HAS_RXF_V2 */

#ifdef RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL
#error code not present
#endif /* RF_HAS_PAD3_CURRENT_REDUCTION_CONTROL */

#ifdef __cplusplus
}
#endif

#endif   /* RXF_H */


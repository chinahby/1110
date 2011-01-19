#ifndef RFLIB_H_1X
#define RFLIB_H_1X
/*===========================================================================

                      RF Library Header File

DESCRIPTION
  Contains the interface definitions for the functions which are exported
  out of the Library.


Copyright (c) 2002, 2003, 2004      by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2005, 2006, 2007      by QUALCOMM, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rflib.h#4 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   ems     Compiler Warning Cleanup
04/28/08   sl      Added LO PWC Cal error codes.
04/23/08   sl      Added INT_DEV_CAL_RC_PROC_ERR_TAU_MEAS_ERR enum.
03/21/08   sl      Added rflib_int_dev_cal_err_code_type.
08/22/07   zg      Fixed the return value type for 
03/06/07   dyc     Added support for QSC60x5
           bmg     Added MSM library support for TIA DC calibration in SC2x
                   C++ compatible function prototypes
03/06/07   ra      Added rf_gps_get_IF_freq_offset()
03/01/07   sar     Fixed compiler warnings.
01/11/07   ra      Added rflib_gps_sw_periodic_dc_update()
11/16/06   jfc     Changed the feature tag for this header file to be unique
11/15/06   adk     Commented out function prototypes that are no longer used
07/20/06   hkk     Featurization for Power Collapse
06/23/06   hkk     Changes for Power collapse
05/01/06   dyc     Added pad3 APIs
01/03/06   dyc     Lint fixes.
07/06/05   et      mainlined rflib_set_1x_sbi()
05/23/05   ans     Added rflib_gps_sw_dc_cancellation for software 
                   dc cancellation for SGPS.
05/04/05   ans     Moved rflib_power_mode_type from rflib.h to rflib_share.h 
                   so that pre-MSM6500 targets can use MP library.
03/31/05   bmg     Removed IntelliCeiver featurization from library
02/04/05   et      featurized rflib_set_1x_sbi under FEATURE_GSM
02/03/05   et      changed comment of rflib_set_1x_sbi api
01/13/05   et      added function rflib_set_1x_sbi to restore 1x sbi frequency
12/17/04   dbc     Changes to support sharing with pre-msm6500 targets
11/08/04   ans     Added new API for using external VCO. rflib_rfr_use_external_vco() 
                   should be called to turn on external VCO for RFR.
11/01/04   et      added NO_RFT to the end of rft_device_enum
10/21/04   bmg     Added initial RFR6500 and RFT6150 support
10/13/04   bmg     Added library power mode query API
08/19/04   jac     Added an rf mode parameter to rflib_set_sbi_info_rxf() for 
                   RFR6135 GPS mode.
08/16/04   dyc     Removed rflib_set_fm_rxf2()
07/13/04   jac     Added GPS mode element to the rf_path_0_config and 
                   rf_path_1_config structures.
04/14/04   sar     updated code to support Enhanced Bands Type.
04/09/04   jac     Added RFR6135 support
03/24/04   dbc     Added support for RFR6175/RFT6170
02/06/04   dbc     Changes for ZIF library architecture update
01/30/04   dbc     added rflib_set_gain_line_up() API function and 
                   rflib_gain_line_up_type 
10/15/03   et      removed RF_PATH_DIV from rf_path_enum_type
10/06/03   rs/dyc  Added RTR6300 to rft_device_enum
09/05/03   dbc     Added support for IMT 
08/28/03   dyc     Assign rf_path_enum_type RF_PATH_DIV to 1.
08/26/03   sar     Added RF_PATH_DIV to rf_path_enum_type.
08/25/03   zg      Use path for rflib_set_gps_rxf/rxf2().
08/11/03   dbc     Explicitly assign decimal values to RF_PATH_0 & RF_PATH_1 enums
08/09/03   dyc     Backed out RF_PATH_DIV
                   Added multi-path parm to rflib_set_agc_def_state()
07/24/03   et      added chain 1 support to prototypes
07/15/03   dbc     Added gpio_sig_type and rflib_serial_device_gpio_config()
07/10/03   dbc     Changes for LMX support
07/02/03   zg      Added channel selection on rflib_set_gps_rxf() and 
                   rflib_set_gps_rxf2() for msm6500.
01/24/03   dbc     Added rf_vco_config_type.  Changed RF_HAS_NV_VCO_CONFIG to
                   RF_HAS_VCO_CONFIG
01/08/03   dbc     Added rflib_set_vco_config() and rflib_is_vco_external() to support
                   internal/external VCO configuration via RF_HAS_NV_VCO_CONFIG
12/20/02   sar     Wrapped function rflib_init_gsm_comm() with T_MSM6300.
12/17/02   aks     Added gsm init APIs.
12/16/02   sar     Added rflib_set_sbi_info_rxf() function prototype to setup
                   the Rx Front SBI Info Register.
12/02/02   et      added rft_set_mux_aux_sbi and rft_set_mux_reg_sbi inside
                   RF_RFLIB_DEBUG so that we can continue to support peak/poke
                   internally, even though we don't support regular sbi read
09/11/02   aks     Wrap appropriate sections in RF_HAS_MULTIMODE_SBI flag.
08/26/02   zg      Removed rflib_disable_comm() and rflib_kill_comm(). 
08/02/02   et      added function rflib_set_mode to set SBI controller
07/26/02   dyc     Added rflib_set_agc_def_state()
                   Added rflib_kill_comm().
                   (merged from dbc,zg MSM6000_RF.01.00.15)
06/21/02   et      removed unused function
06/13/02   et      renamed rfr_I_Q_dac_set and added a transconductor
                   parameter
06/11/02   dbc     Added rflib_disable_comm()
06/06/02   dbc     Removed a prototype. Cleanup.
05/21/02   dbc     Added/Modified function descriptions.
03/22/02   dbc     Added declaration of rf_set_rx_agc_mode()
03/21/02   aks     Add read_trk_lo_adj() function.
03/14/02   aks     Created.

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "db.h"
#include "bio.h"
#include "rfifreq.h"
#include "rflib_share.h"


typedef enum {
  RF_SBI_MODE_ARM,
  RF_SBI_MODE_ARM_CM,
  RF_SBI_MODE_MDSP
} rf_sbi_mode_type;


/* type configuring the gain line up in the RFR */
typedef enum rflib_gain_line_up
{
  RFR6125_LINE_UP_0,
  RFR6125_LINE_UP_1
} rflib_gain_line_up_type;

typedef boolean coarse_dc_cal_state_backup_type;
                                     

typedef enum
{
  INT_DEV_CAL_NO_ERROR,
  INT_DEV_CAL_RC_PROC_ERR_RC_MEAS_TO,          //RC Process Error Cal, RC Measurement Timeout
  INT_DEV_CAL_RC_PROC_ERR_VDROP_REXT_MEAS_ERR, //RC Process Error Cal, Voltage-Drop R-External Measurement Error
  INT_DEV_CAL_RC_PROC_ERR_VDROP_RV_MEAS_ERR,   //RC Process Error Cal, Voltage-Drop R-Vertical Measurement Error
  INT_DEV_CAL_RC_PROC_ERR_VDROP_RH_MEAS_ERR,   //RC Process Error Cal, Voltage-Drop R-Horizontal Measurement Error
  INT_DEV_CAL_RC_PROC_ERR_TAU_MEAS_ERR,		   //RC Process Error Cal, RC tau Measurement Error ( if <= 0 )
  INT_DEV_CAL_LO_PWC_CAL_VDD4_INVALID,		   //LO PWC Cal, VDD_DIV_4 ( if <= 0 )
  INT_DEV_CAL_LO_PWC_CAL_DAC_CODE_INVALID,     //LO PWC Cal, DAC Code ( if <= 0 )
  INT_DEV_CAL_MAX
} rflib_int_dev_cal_err_code_type;//Internal Device Cal

#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION RFLIB_SET_CDMA_RXF

DESCRIPTION
  Configures parameters in the RX Front for CDMA mode of operation.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_cdma_rxf(rf_path_enum_type path);
                                     
/*===========================================================================

FUNCTION RFLIB_SET_PCS_RXF

DESCRIPTION      
  Configures parameters in the RX Front for PCS mode of operation.
  
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_pcs_rxf( rf_path_enum_type path );

/*===========================================================================

FUNCTION RFLIB_SET_GPS_RXF

DESCRIPTION      
  Configures parameters in the RX Front for GPS mode of operation.
  
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_gps_rxf( rf_path_enum_type path );

/*===========================================================================

FUNCTION RFLIB_SET_GPS_RXF2

DESCRIPTION      
  Configures parameters in the RX Front for GPS mode of operation.
  
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_gps_rxf2( rf_path_enum_type path );

/*===========================================================================

FUNCTION rflib_gps_sw_dc_cancellation

DESCRIPTION
  This function cancels the I&Q DC for gps chain.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void rflib_gps_sw_dc_cancellation( rf_path_enum_type path );
/*===========================================================================

FUNCTION rflib_gps_sw_periodic_dc_update

DESCRIPTION
  This function periodic cancels the I&Q DC for gps chain. It should be called
  by GPSSRCH task every 1 second after GPS receiver is turned on.

DEPENDENCIES
  - This function should only be used for SGPS on PATH 1.
  - The peoridic update can only be called after one-time initial update of 
  rflib_gps_sw_dc_cancellation 

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void rflib_gps_sw_periodic_dc_update( rf_path_enum_type path );

/*===========================================================================

FUNCTION rflib_gps_gain_adjust

DESCRIPTION
  This function adjusts bbrx vref as needed

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void rflib_gps_gain_adjust( rf_path_enum_type path );

/*===========================================================================

FUNCTION RFLIB_INIT

DESCRIPTION
  Initializes services used by library functions.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
#ifndef RF_TODO_7600_ADK
void rflib_init( void );
#endif

/*===========================================================================

FUNCTION RFLIB_INIT_COMM

DESCRIPTION
  Initializes the communications required for the Library functions.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void rflib_init_comm( void );

/*===========================================================================

FUNCTION RF_SET_RX_AGC_MODE                           EXTERNAL FUNCTION

DESCRIPTION
  This function configures the Rx AGC section dependent upon the RF Mode 

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rf_set_rx_agc_mode( rf_path_enum_type path, db_rf_mode_type rf_mode,  byte mixer_polarity);

/*===========================================================================

FUNCTION RFLIB_SET_AGC_DEF_STATE                          EXTERNAL FUNCTION

DESCRIPTION
  This function configures the CDMA AGC to the default state for CDMA

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void rflib_set_agc_def_state( rf_path_enum_type path );


/*===========================================================================

FUNCTION  rflib_init_gsm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef RF_TODO_7600_ADK
extern void rflib_init_gsm( void );
#endif

/*===========================================================================

FUNCTION  rflib_init_gsm_comm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef RF_TODO_7600_ADK
#ifdef T_MSM6300
#error code not present
#endif /* T_MSM6300 */
#endif


/*===========================================================================

FUNCTION  RFLIB_SET_PATH_1_DEVICES

DESCRIPTION
  This function is called by the upper layer to configure the devices on
  RF path 0.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE - devices were successfully configured
            FALSE - devices were not configured 

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean rflib_set_path_1_devices( const rf_path_1_config_type *);


/*===========================================================================

FUNCTION  RFLIB_SWITCH_PATH_1_PLL

DESCRIPTION
  This function sets a boolean flag in the rf_path_1_devices structure 
  indicating which PLL (RFT or LMX) to use for RX

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void rflib_switch_path_1_pll( pll_path_type_enum_type );


/*===========================================================================

FUNCTION rflib_set_gain_line_up                            EXTERNAL FUNCTION

DESCRIPTION
  API function to change the gain lineup of the RFR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rflib_set_gain_line_up( rf_path_enum_type path, 
                                    rflib_gain_line_up_type setting );

#ifdef FEATURE_INTELLICEIVER  //RF_TODO_7600_ADK: Preferred to the #ifdef below
#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

FUNCTION rflib_get_power_mode                               EXTERNAL FUNCTION

DESCRIPTION
  API function to query the RF library power managment state

DEPENDENCIES
  None

RETURN VALUE
  The current power state as controlled by the RF library

SIDE EFFECTS
  None

===========================================================================*/
extern rflib_power_mode_type rflib_get_power_mode( rf_path_enum_type path );
#endif /* RF_HAS_POWER_MODE_LIB */
#endif

/*==============================================================================

FUNCTION RFLIB_RFR_USE_EXTERNAL_VCO

DESCRIPTION
  This function is called to configure the RFR to use internal or external
  VCO

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rflib_rfr_use_external_vco(  rf_path_enum_type path, boolean select_external_vco );

/*==============================================================================
FUNCTION RFLIB_SET_1X_SBI

DESCRIPTION
  API function to restore 1x sbi setting

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
  changes he SBI frequency
===========================================================================*/
void rflib_set_1x_sbi(void);

/*==============================================================================
FUNCTION RFLIB_SET_PAD3_SLEEP

DESCRIPTION
  api function to configure the pad3 related pins for sleep condition.

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
void rflib_set_pad3_sleep(rf_path_enum_type path);

/*==============================================================================
FUNCTION RFLIB_SET_PAD3_WAKEUP

DESCRIPTION
  api function to configure the pad3 related pins for wakeup condition.

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
   none
===========================================================================*/
void rflib_set_pad3_wakeup(rf_path_enum_type path);

#ifdef RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
#error code not present
#endif /* RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE */

#ifdef RF_HAS_CGPS
#error code not present
#endif /* RF_HAS_CGPS */

/*===========================================================================

FUNCTION RFLIB_START_COARSE_DC_CAL                          EXTERNAL FUNCTION

DESCRIPTION
  Configures the RX Front for coarse DC calibration.
  This function is only used in RF CMOS platforms from QSC60x5 and RTR6500
  forward

DEPENDENCIES
  RX Front clocks must be enabled

RETURN VALUE
  State of DC before reconfiguration occurs

SIDE EFFECTS
  DACC will be disabled

===========================================================================*/
coarse_dc_cal_state_backup_type rflib_start_coarse_dc_cal( rf_path_enum_type path );


/*===========================================================================

FUNCTION RFLIB_END_COARSE_DC_CAL                            EXTERNAL FUNCTION

DESCRIPTION
  Restores DC configuration after coarse DC cal.
  This function is only used in RF CMOS platforms from QSC60x5 and RTR6500
  forward

DEPENDENCIES
  RX Front clocks must be enabled

RETURN VALUE
  None

SIDE EFFECTS
  If dc_state is passed as the value returned from the opening call to
  rflib_start_coarse_dc_cal(), the RX Front DC hardware is reconfigured
  to the original configuration.

===========================================================================*/
void rflib_end_coarse_dc_cal( rf_path_enum_type path,
                              coarse_dc_cal_state_backup_type dc_state );


/*===========================================================================

FUNCTION RFLIB_RESET_DC_CAL_MEASUREMENT                     EXTERNAL FUNCTION

DESCRIPTION
  Resets DC coarse tune measurement, used as part of the DC coarse tuning
  sequence.

DEPENDENCIES
  RX Front clocks must be enabled
  This function should be called within a rflib_start_coarse_dc_cal() sequence

RETURN VALUE
  None

SIDE EFFECTS
  Internal DC offset state is reset in preparation for a new measurement

===========================================================================*/
void rflib_reset_dc_cal_measurement( rf_path_enum_type path );


/*===========================================================================

FUNCTION RFLIB_GET_DC_I_OFFSET                              EXTERNAL FUNCTION

DESCRIPTION
  Returns the DC offset for the I channel

DEPENDENCIES
  RX Front clocks must be enabled
  This function should be called within a rflib_start_coarse_dc_cal() sequence

RETURN VALUE
  I channel DC offset

SIDE EFFECTS
  None

===========================================================================*/
int rflib_get_dc_i_offset( rf_path_enum_type path );


/*===========================================================================

FUNCTION RFLIB_GET_DC_Q_OFFSET                              EXTERNAL FUNCTION

DESCRIPTION
  Returns the DC offset for the Q channel

DEPENDENCIES
  RX Front clocks must be enabled
  This function should be called within a rflib_start_coarse_dc_cal() sequence

RETURN VALUE
  Q channel DC offset

SIDE EFFECTS
  None

===========================================================================*/
int rflib_get_dc_q_offset( rf_path_enum_type path );

#ifdef __cplusplus
}
#endif


/*===========================================================================

FUNCTION rflib_dev_internal_calibration

DESCRIPTION
  This function performs any required internal device calibration for all
  devices on the current target. It will perform the calibration regardless of the 
  state of the NV items.
  
DEPENDENCIES
  NV task must be running.

RETURN VALUE
  rflib_int_dev_cal_err_code_type

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
rflib_int_dev_cal_err_code_type rflib_dev_internal_calibration
(
  rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
  rex_sigs_type task_wait_sig,          /* Task signal to wait for when reading NV */
  void (*task_wait_handler)( rex_sigs_type ),
  boolean bypass_nv_check
);


#endif /* RFLIB_H_1X */


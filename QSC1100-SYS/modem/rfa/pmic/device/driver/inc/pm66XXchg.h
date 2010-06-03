#ifndef PM66XXCHG_H
#define PM66XXCHG_H
/*===========================================================================


               P M 6 6 X X C H G   H E A D E R    F I L E

DESCRIPTION
    This file contains functions prototypes and variable/type/constant 
  declarations to support the charger inside the Qualcomm 
  PM66XX chips.
  
Copyright (c) 2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2009      by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXchg.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/09   hs      Added support for USB boot Charging
03/24/09   jtn     Add API pm_chg_cleanup() 
03/12/09   hs      Added a parameter in pm_chg_failed_clear().
12/18/08   jtn     Corrected enum pm_auto_chg_state_name_type
12/17/08   jtn     Added API to force auto charger state, pm_chg_charge_state_set()
                   Moved auto state enum to this file from charger code.
06/27/08   jtn     Merge changes from QSC1100 branch
06/20/08   jtn     Added pm_chg_usb_chg_det_type for PMIC4 charger 
03/31/08   jtn     Added API pm_chg_enable_charging() 
03/12/08   jtn     hs added pm_chg_imaxsel_type for task-based charger
01/17/08   vk      Provided API to set current limit for PM6658 
09/07/07   cng     Added APIs pm_chg_is_vbatdet_cal_reqd() and pm_chg_is_single_chg_path()
                   for run-time charging detection 
06/08/07   cng     Added pm_chg_is_power_limit_irq_driven and pm_chg_is_internal_pnp_rsns
11/02/06   hs      Added a new API pm_chg_is_detection_reqd() which decides 
                   whether we need software workaround to detect the removal
                   of the charger or not.
10/05/06   hs      Added new API pm_chg_get_battery_transistor_status() to check 
                   if the battery transistor is open or not
03/14/06   cng     Expanded enum pm_chg_batt_alarm_hyst_type to allow all
                   possible settings of battery alarm time hysteresis 
10/24/05   cng     Added a more generic API pm_chg_charger_pwr_up_def_st that
                   can configure a specific power source to use at power up,
                   depending on the PMIC's charging configuration 
10/27/04   rmd     Changed the naming conversion of the pulse charger code 
                   to reflect that the PMIC pulse charger API supports  
                   applies for both wall and usb chargers.
10/26/04   rmd     Added the comments to the pulse charging code.
08/30/04   rmd     Updated the comments for the following function:  
                   pm_chg_usb_charger_suspend_mode_switch().
08/09/04   rmd     * Provided support to all IMAXSEL settings.
                   * Added the ability to select if the PMIC will use 
                     the battery or the USB charger as the default power
                     source.
                   * Added support for VBUS LPM mode.
07/13/04   rmd     Split the ability to program IMAXSEL and VMAXSEL into 2
                   function. This was done because VMAXSEL applies for both 
                   the Wall Charger and USB charger transistors.
06/07/04   rmd     Improved the examples for the pulse charger API.
05/24/04   rmd     Added the ability to configure the USB Charger IMAXSEL, 
                   USB Charger PMAXSEL and WALL Charger PMAXSEL.
04/26/04   rmd     Added support for Pulse Charging.
04/13/04   rmd     Added support for Battery Alarms/Thresholds.
04/06/04   rmd     Added support for VCP (voltage collapse protection).
03/08/04   rmd     Added the ability to enable/disable the USB charger.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/21/03   rmd     Added support for the coin cell charger.
11/14/02   rmd     Re-wrote the File.
05/14/02   rmd     Created.
===========================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/* Maximum trickle charging current */
typedef enum
{
   PM_CHG_TR_IMAX_00mA,
   PM_CHG_TR_IMAX_20mA,
   PM_CHG_TR_IMAX_30mA,
   PM_CHG_TR_IMAX_40mA,
   PM_CHG_TR_IMAX_50mA,
   PM_CHG_TR_IMAX_60mA,
   PM_CHG_TR_IMAX_70mA,
   PM_CHG_TR_IMAX_80mA,
   PM_CHG_TR_IMAX_INVALID
}pm_chg_tr_imax_type;

/* Maximum wall charger transistor current. */
typedef enum
{
   PM_CHG_WCHG_Q_IMAX_010mV_div_R, /* IMAX = (010mV)/R, where R = Rsense. */
   PM_CHG_WCHG_Q_IMAX_020mV_div_R, /* IMAX = (020mV)/R */
   PM_CHG_WCHG_Q_IMAX_030mV_div_R, /* IMAX = (030mV)/R */
   PM_CHG_WCHG_Q_IMAX_040mV_div_R, /* IMAX = (040mV)/R */
   PM_CHG_WCHG_Q_IMAX_050mV_div_R, /* IMAX = (050mV)/R */
   PM_CHG_WCHG_Q_IMAX_060mV_div_R, /* IMAX = (060mV)/R */
   PM_CHG_WCHG_Q_IMAX_070mV_div_R, /* IMAX = (070mV)/R */
   PM_CHG_WCHG_Q_IMAX_080mV_div_R, /* IMAX = (080mV)/R */
   PM_CHG_WCHG_Q_IMAX_090mV_div_R, /* IMAX = (090mV)/R */
   PM_CHG_WCHG_Q_IMAX_100mV_div_R, /* IMAX = (100mV)/R */  
   PM_CHG_WCHG_Q_IMAX_110mV_div_R, /* IMAX = (110mV)/R */
   PM_CHG_WCHG_Q_IMAX_120mV_div_R, /* IMAX = (120mV)/R */
   PM_CHG_WCHG_Q_IMAX_130mV_div_R, /* IMAX = (130mV)/R */
   PM_CHG_WCHG_Q_IMAX_140mV_div_R, /* IMAX = (140mV)/R */
   PM_CHG_WCHG_Q_IMAX_150mV_div_R, /* IMAX = (150mV)/R */
   PM_CHG_WCHG_Q_IMAX_160mV_div_R, /* IMAX = (160mV)/R */
   PM_CHG_WCHG_Q_IMAX_170mV_div_R, /* IMAX = (170mV)/R */
   PM_CHG_WCHG_Q_IMAX_180mV_div_R, /* IMAX = (180mV)/R */
   PM_CHG_WCHG_Q_IMAX_190mV_div_R, /* IMAX = (190mV)/R */
   PM_CHG_WCHG_Q_IMAX_200mV_div_R, /* IMAX = (200mV)/R */
   PM_CHG_WCHG_Q_IMAX_210mV_div_R, /* IMAX = (210mV)/R */
   PM_CHG_WCHG_Q_IMAX_220mV_div_R, /* IMAX = (220mV)/R */
   PM_CHG_WCHG_Q_IMAX_230mV_div_R, /* IMAX = (230mV)/R */
   PM_CHG_WCHG_Q_IMAX_240mV_div_R, /* IMAX = (240mV)/R */
   PM_CHG_WCHG_Q_IMAX_250mV_div_R, /* IMAX = (250mV)/R */
   PM_CHG_WCHG_Q_IMAX_260mV_div_R, /* IMAX = (260mV)/R */
   PM_CHG_WCHG_Q_IMAX_270mV_div_R, /* IMAX = (270mV)/R */
   PM_CHG_WCHG_Q_IMAX_280mV_div_R, /* IMAX = (280mV)/R */
   PM_CHG_WCHG_Q_IMAX_290mV_div_R, /* IMAX = (290mV)/R */
   PM_CHG_WCHG_Q_IMAX_300mV_div_R, /* IMAX = (300mV)/R */
   PM_CHG_WCHG_Q_IMAX_INVALID
}pm_chg_wchg_q_imax_type;

/* Maximum wall charger transistor current. */
typedef enum /* pm_chg_imaxsel_type */
{
   PM_CHG_IMAXSEL_010mV_div_R, /* IMAX = (010mV)/R, where R = Rsense. */
   PM_CHG_IMAXSEL_020mV_div_R, /* IMAX = (020mV)/R */
   PM_CHG_IMAXSEL_030mV_div_R, /* IMAX = (030mV)/R */
   PM_CHG_IMAXSEL_040mV_div_R, /* IMAX = (040mV)/R */
   PM_CHG_IMAXSEL_050mV_div_R, /* IMAX = (050mV)/R */
   PM_CHG_IMAXSEL_060mV_div_R, /* IMAX = (060mV)/R */
   PM_CHG_IMAXSEL_070mV_div_R, /* IMAX = (070mV)/R */
   PM_CHG_IMAXSEL_080mV_div_R, /* IMAX = (080mV)/R */
   PM_CHG_IMAXSEL_090mV_div_R, /* IMAX = (090mV)/R */
   PM_CHG_IMAXSEL_100mV_div_R, /* IMAX = (100mV)/R */  
   PM_CHG_IMAXSEL_110mV_div_R, /* IMAX = (110mV)/R */
   PM_CHG_IMAXSEL_120mV_div_R, /* IMAX = (120mV)/R */
   PM_CHG_IMAXSEL_130mV_div_R, /* IMAX = (130mV)/R */
   PM_CHG_IMAXSEL_140mV_div_R, /* IMAX = (140mV)/R */
   PM_CHG_IMAXSEL_150mV_div_R, /* IMAX = (150mV)/R */
   PM_CHG_IMAXSEL_160mV_div_R, /* IMAX = (160mV)/R */
   PM_CHG_IMAXSEL_170mV_div_R, /* IMAX = (170mV)/R */
   PM_CHG_IMAXSEL_180mV_div_R, /* IMAX = (180mV)/R */
   PM_CHG_IMAXSEL_190mV_div_R, /* IMAX = (190mV)/R */
   PM_CHG_IMAXSEL_200mV_div_R, /* IMAX = (200mV)/R */
   PM_CHG_IMAXSEL_210mV_div_R, /* IMAX = (210mV)/R */
   PM_CHG_IMAXSEL_220mV_div_R, /* IMAX = (220mV)/R */
   PM_CHG_IMAXSEL_230mV_div_R, /* IMAX = (230mV)/R */
   PM_CHG_IMAXSEL_240mV_div_R, /* IMAX = (240mV)/R */
   PM_CHG_IMAXSEL_250mV_div_R, /* IMAX = (250mV)/R */
   PM_CHG_IMAXSEL_260mV_div_R, /* IMAX = (260mV)/R */
   PM_CHG_IMAXSEL_270mV_div_R, /* IMAX = (270mV)/R */
   PM_CHG_IMAXSEL_280mV_div_R, /* IMAX = (280mV)/R */
   PM_CHG_IMAXSEL_290mV_div_R, /* IMAX = (290mV)/R */
   PM_CHG_IMAXSEL_300mV_div_R, /* IMAX = (300mV)/R */
   PM_CHG_IMAXSEL_INVALID
}pm_chg_imaxsel_type;

/* Selects the maximum charger transistor voltage. */
typedef enum
{               
   PM_CHG_Q_VMAXSEL_4000mV,
   PM_CHG_Q_VMAXSEL_4025mV,
   PM_CHG_Q_VMAXSEL_4050mV,
   PM_CHG_Q_VMAXSEL_4075mV,
   PM_CHG_Q_VMAXSEL_4100mV,
   PM_CHG_Q_VMAXSEL_4125mV,
   PM_CHG_Q_VMAXSEL_4150mV,
   PM_CHG_Q_VMAXSEL_4175mV,
   PM_CHG_Q_VMAXSEL_4200mV,
   PM_CHG_Q_VMAXSEL_4225mV,
   PM_CHG_Q_VMAXSEL_4250mV,
   PM_CHG_Q_VMAXSEL_4275mV,
   PM_CHG_Q_VMAXSEL_4300mV,
   PM_CHG_Q_VMAXSEL_4325mV,
   PM_CHG_Q_VMAXSEL_4350mV,
   PM_CHG_Q_VMAXSEL_4375mV,
   PM_CHG_Q_VMAXSEL_INVALID
}pm_chg_q_vmax_type;

/* Pulse charger type:
LOW SIDE  = Using Battery Transistor.
HIGH SIDE = Using Charger Transistor.
*/
typedef enum
{
   PM_PULSE_CHG_Q__LOW_SIDE,
   PM_PULSE_CHG_Q__HIGH_SIDE,
   PM_PULSE_CHG_Q__INVALID
}pm_pulse_chg_q_side_type;

/* This is the diff time settings for charging on time during pulse
   charging. */
typedef enum 
{
   PM_PULSE_CHG_TON__62p5ms,
   PM_PULSE_CHG_TON__125ms,
   PM_PULSE_CHG_TON__250ms,
   PM_PULSE_CHG_TON__500ms,
   PM_PULSE_CHG_TON__INVALID
}pm_pulse_chg_ton_type;

/* This is the diff time settings for charging off time during pulse
   charging. */
typedef enum 
{
   PM_PULSE_CHG_TOFF__62p5ms,
   PM_PULSE_CHG_TOFF__125ms,
   PM_PULSE_CHG_TOFF__250ms,
   PM_PULSE_CHG_TOFF__500ms,
   PM_PULSE_CHG_TOFF__INVALID
}pm_pulse_chg_toff_type;

/* This is the different choices for the number of consecutive Toff cycles 
   before we stop pulse charging.*/
typedef enum 
{
   PM_PULSE_CHG_TDONE__TOFF_X_16,
   PM_PULSE_CHG_TDONE__TOFF_X_32,
   PM_PULSE_CHG_TDONE__TOFF_X_64,
   PM_PULSE_CHG_TDONE__TOFF_X_128,
   PM_PULSE_CHG_TDONE__INVALID
}pm_pulse_chg_tdone_type;

/* This structure contains all the settings for wall pulse charging. */
typedef struct
{
   pm_pulse_chg_q_side_type p_type;
   pm_pulse_chg_ton_type    t_on;
   pm_pulse_chg_toff_type   t_off;
   pm_pulse_chg_tdone_type  t_done;
}pm_chg_pulse_chg_config_type;

/* Maximum wall charger power limits. */
typedef enum
{
   PM_CHG_PMAXSEL__DISABLE,
   PM_CHG_PMAXSEL__400mW,
   PM_CHG_PMAXSEL__500mW,
   PM_CHG_PMAXSEL__600mW,
   PM_CHG_PMAXSEL__750mW,
   PM_CHG_PMAXSEL__1000mW,
   PM_CHG_PMAXSEL__1500mW,
   PM_CHG_PMAXSEL__2000mW,
   PM_CHG_PMAXSEL__INVALID
}pm_chg_pmaxsel_type;

/* Maximum USB charger transistor current. */
typedef enum
{
   PM_CHG_UCHG_Q_IMAX_010mV_div_R, /* IMAX = (010mV)/R, where R = Rsense. */
   PM_CHG_UCHG_Q_IMAX_020mV_div_R, /* IMAX = (020mV)/R */
   PM_CHG_UCHG_Q_IMAX_030mV_div_R, /* IMAX = (030mV)/R */
   PM_CHG_UCHG_Q_IMAX_040mV_div_R, /* IMAX = (040mV)/R */
   PM_CHG_UCHG_Q_IMAX_050mV_div_R, /* IMAX = (050mV)/R */
   PM_CHG_UCHG_Q_IMAX_060mV_div_R, /* IMAX = (060mV)/R */
   PM_CHG_UCHG_Q_IMAX_070mV_div_R, /* IMAX = (070mV)/R */
   PM_CHG_UCHG_Q_IMAX_080mV_div_R, /* IMAX = (080mV)/R */
   PM_CHG_UCHG_Q_IMAX_090mV_div_R, /* IMAX = (090mV)/R */
   PM_CHG_UCHG_Q_IMAX_100mV_div_R, /* IMAX = (100mV)/R */  
   PM_CHG_UCHG_Q_IMAX_110mV_div_R, /* IMAX = (110mV)/R */
   PM_CHG_UCHG_Q_IMAX_120mV_div_R, /* IMAX = (120mV)/R */
   PM_CHG_UCHG_Q_IMAX_130mV_div_R, /* IMAX = (130mV)/R */
   PM_CHG_UCHG_Q_IMAX_140mV_div_R, /* IMAX = (140mV)/R */
   PM_CHG_UCHG_Q_IMAX_150mV_div_R, /* IMAX = (150mV)/R */
   PM_CHG_UCHG_Q_IMAX_160mV_div_R, /* IMAX = (160mV)/R */
   PM_CHG_UCHG_Q_IMAX_170mV_div_R, /* IMAX = (170mV)/R */
   PM_CHG_UCHG_Q_IMAX_180mV_div_R, /* IMAX = (180mV)/R */
   PM_CHG_UCHG_Q_IMAX_190mV_div_R, /* IMAX = (190mV)/R */
   PM_CHG_UCHG_Q_IMAX_200mV_div_R, /* IMAX = (200mV)/R */  
   PM_CHG_UCHG_Q_IMAX_INVALID
}pm_chg_uchg_q_imax_type;

/* Which power source it could use at power up: wall charger,
 * battery, or USB charger 
 */
typedef enum
{
   PM_CHG_POWER_ON_FROM__CHG,
   PM_CHG_POWER_ON_FROM__BAT,
   PM_CHG_POWER_ON_FROM__USB_CHG,
   PM_CHG_POWER_ON_FROM__INVALID
}pm_chg_power_on_type;

/* Which power source should the handset boot up with if the
   wall charger is not available. */
typedef enum
{
   PM_CHG_USB_POWER_ON_FROM__BAT,
   PM_CHG_USB_POWER_ON_FROM__UCHG,
   PM_CHG_USB_POWER_ON_FROM__INVALID
}pm_chg_usb_power_on_type;

/* Battery Alarm Hysteresis Settings. */
typedef enum
{
   PM_CHG_BATT_ALARM_HYST__122us,
   PM_CHG_BATT_ALARM_HYST__244us,
   PM_CHG_BATT_ALARM_HYST__488us,
   PM_CHG_BATT_ALARM_HYST__976us,
   PM_CHG_BATT_ALARM_HYST__1952us,
   PM_CHG_BATT_ALARM_HYST__3904us,
   PM_CHG_BATT_ALARM_HYST__7808us,
   PM_CHG_BATT_ALARM_HYST__15616us,
   PM_CHG_BATT_ALARM_HYST__INVALID
}pm_chg_batt_alarm_hyst_type;

/* Coin Cell Charger Voltage Settings. */
typedef enum
{
   PM_COIN_CELL_VSET_3p0V,
   PM_COIN_CELL_VSET_3p1V,
   PM_COIN_CELL_VSET_3p2V,
   PM_COIN_CELL_VSET_INVALID
}pm_coin_cell_vset_type;

/* Coin Cell Charger Current Limiting Resistor Settings. */
typedef enum
{
   PM_COIN_CELL_RSET_2100_OHMS,
   PM_COIN_CELL_RSET_1700_OHMS,
   PM_COIN_CELL_RSET_1200_OHMS,
   PM_COIN_CELL_RSET_800_OHMS,
   PM_COIN_CELL_RSET_INVALID
}pm_coin_cell_rset_type;

/* USB charger detection setting */
typedef enum
{
    USB_HOST_DETECTED = 0,
    USB_CHARGER_DETECTED
} pm_chg_usb_chg_det_type;

/*! \enum pm_auto_chg_state_name_type
 *  \brief This enum contains names for each charging state in the autonomous charging
 */
typedef enum 
{
    /*! Charger state machine entry point.                            */
    CHG_AUTO__INIT_ST,     
    /*! Power on from the charger state.                              */
    CHG_AUTO__PWR_ON_CHGR_ST,
    /*! Auto rickle charging state.                                   */
    CHG_AUTO__AUTO_TRICKLE_ST,  
    /*!Power on from the battery state.                               */
    CHG_AUTO__PWR_ON_BATT_ST,
    /*! Auto trickle charging has failed state.                       */
    CHG_AUTO__AUTO_TRICKLE_FAILED_ST,
    /*! Transition state, gives time to analog circuitry to settle.   */
    CHG_AUTO__TRANS_ST,
    /*! Power on from battery and charger after auto trickle charing. */                          
    CHG_AUTO__POST_AUTO_TRICKLE_ST,
    /*! Fast charging state.                                          */
    CHG_AUTO__FAST_ST,   
    /*! Trickle charging state.                                       */
    CHG_AUTO__TRICKLE_ST,
    /*! Charging failed state.                                        */
    CHG_AUTO__FAILED_ST,
    /*! End-of-charging state -- charging succeed.                    */
    CHG_AUTO__DONE_ST,
    /*! Temperary state: Low current liming charging                  */
    CHG_AUTO__LOW_ILIM_ST, 
    /*! USB suspend mode.                                             */
    CHG_AUTO__USB_SUSP_ST,
    /*! Invalid state.                                                */
    CHG_AUTO__INVALID_ST           
} pm_auto_chg_state_name_type;

/*===========================================================================

                 CHARGER DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION pm_chg_wall_charger_transistor_switch         EXTERNAL FUNCTION

DESCRIPTION
    This function Opens/Closes (ON/OFF, via CHG_CTL_N pin) the charger 
   transistor.  
   
   example:
   
   // Close (turn ON) the charger transistor.
   err |= pm_chg_wall_charger_transistor_switch(PM_ON_CMD);
            
INPUT PARAMETER
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Charger Transistor ON  (close).
        PM_OFF_CMD = Charger Transistor OFF (open).

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_wall_charger_transistor_switch(
                                                  pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_battery_transistor_switch         EXTERNAL FUNCTION

DESCRIPTION
     This function Opens/Closes (turn OFF/ON the battery MOSFET) the  
   battery transistor. 
   
   NOTES:
     1) Battery Transistor needs to be open for pulse charging to work.        
   
  example:
   
   // Close (turn ON) the battery transistor.
   err |= pm_chg_wall_battery_transistor_switch(PM_ON_CMD);
            
INPUT PARAMETER
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Battery Transistor ON  (close).
        PM_OFF_CMD = Battery Transistor OFF (open).

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_battery_transistor_switch(
                                                pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_wall_charger_transistor_imaxsel         EXTERNAL FUNCTION

DESCRIPTION
  This function updates the charger transistor maximum current (IMAXSEL).
    
  example:
  
    // Set the charger transistor maximum current (IMAXSEL) = 1A.  
    //     Where IMAX = (100mV)/R and Rsense = 0.1 ohms.
        
    err |= pm_chg_wall_charger_transistor_imaxsel(
                                PM_CHG_WCHG_Q_IMAX_100mV_div_R);
    
INPUT PARAMETERS
  1) Parameter name: imax.
     - Charger transistor maximum current.

     Parameter type: pm_chg_wchg_q_imax_type (enum).
     - Valid Inputs:  

        PM_CHG_WCHG_Q_IMAX_010mV_div_R = IMAX = (010mV)/R 
        PM_CHG_WCHG_Q_IMAX_020mV_div_R = IMAX = (020mV)/R
        PM_CHG_WCHG_Q_IMAX_030mV_div_R = IMAX = (030mV)/R
        PM_CHG_WCHG_Q_IMAX_040mV_div_R = IMAX = (040mV)/R
        PM_CHG_WCHG_Q_IMAX_050mV_div_R = IMAX = (050mV)/R
        PM_CHG_WCHG_Q_IMAX_060mV_div_R = IMAX = (060mV)/R
        PM_CHG_WCHG_Q_IMAX_070mV_div_R = IMAX = (070mV)/R
        PM_CHG_WCHG_Q_IMAX_080mV_div_R = IMAX = (080mV)/R
        PM_CHG_WCHG_Q_IMAX_090mV_div_R = IMAX = (090mV)/R
        PM_CHG_WCHG_Q_IMAX_100mV_div_R = IMAX = (100mV)/R  
        PM_CHG_WCHG_Q_IMAX_110mV_div_R = IMAX = (110mV)/R
        PM_CHG_WCHG_Q_IMAX_120mV_div_R = IMAX = (120mV)/R
        PM_CHG_WCHG_Q_IMAX_130mV_div_R = IMAX = (130mV)/R
        PM_CHG_WCHG_Q_IMAX_140mV_div_R = IMAX = (140mV)/R
        PM_CHG_WCHG_Q_IMAX_150mV_div_R = IMAX = (150mV)/R
        PM_CHG_WCHG_Q_IMAX_160mV_div_R = IMAX = (160mV)/R
        PM_CHG_WCHG_Q_IMAX_170mV_div_R = IMAX = (170mV)/R
        PM_CHG_WCHG_Q_IMAX_180mV_div_R = IMAX = (180mV)/R
        PM_CHG_WCHG_Q_IMAX_190mV_div_R = IMAX = (190mV)/R
        PM_CHG_WCHG_Q_IMAX_200mV_div_R = IMAX = (200mV)/R
        PM_CHG_WCHG_Q_IMAX_210mV_div_R = IMAX = (210mV)/R
        PM_CHG_WCHG_Q_IMAX_220mV_div_R = IMAX = (220mV)/R
        PM_CHG_WCHG_Q_IMAX_230mV_div_R = IMAX = (230mV)/R
        PM_CHG_WCHG_Q_IMAX_240mV_div_R = IMAX = (240mV)/R
        PM_CHG_WCHG_Q_IMAX_250mV_div_R = IMAX = (250mV)/R
        PM_CHG_WCHG_Q_IMAX_260mV_div_R = IMAX = (260mV)/R
        PM_CHG_WCHG_Q_IMAX_270mV_div_R = IMAX = (270mV)/R
        PM_CHG_WCHG_Q_IMAX_280mV_div_R = IMAX = (280mV)/R
        PM_CHG_WCHG_Q_IMAX_290mV_div_R = IMAX = (290mV)/R
        PM_CHG_WCHG_Q_IMAX_300mV_div_R = IMAX = (300mV)/R
                   
        WHERE R = CURRENT SENSE RESISTOR, usually 0.1 ohms.                     
                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED.

===========================================================================*/
extern pm_err_flag_type pm_chg_wall_charger_transistor_imaxsel( \
                                        pm_chg_wchg_q_imax_type imax);

/*===========================================================================

FUNCTION pm_chg_charger_transistor_vmaxsel           EXTERNAL FUNCTION

DESCRIPTION
    This function updates the charger transistor maximum voltage (VMAXSEL).
    
    NOTE: This applies for both the Wall and USB chargers.
  
  example:
  
    // Set the maximum voltage (VMAXSEL) = 4.2 Volts.        
    err |= pm_chg_charger_transistor_vmaxsel(PM_CHG_Q_VMAXSEL_4200mV);
    
INPUT PARAMETERS
  2) Parameter name: vmax.
     - Charger transistor maximum voltage.

     Parameter type: pm_chg_q_vmax_type (enum). 
     - Valid Inputs:

        PM_CHG_Q_VMAXSEL_4000mV
        PM_CHG_Q_VMAXSEL_4025mV
        PM_CHG_Q_VMAXSEL_4050mV
        PM_CHG_Q_VMAXSEL_4075mV
        PM_CHG_Q_VMAXSEL_4100mV
        PM_CHG_Q_VMAXSEL_4125mV
        PM_CHG_Q_VMAXSEL_4150mV
        PM_CHG_Q_VMAXSEL_4175mV
        PM_CHG_Q_VMAXSEL_4200mV
        PM_CHG_Q_VMAXSEL_4225mV
        PM_CHG_Q_VMAXSEL_4250mV
        PM_CHG_Q_VMAXSEL_4275mV
        PM_CHG_Q_VMAXSEL_4300mV
        PM_CHG_Q_VMAXSEL_4325mV
        PM_CHG_Q_VMAXSEL_4350mV
        PM_CHG_Q_VMAXSEL_4375mV
                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED.

===========================================================================*/
extern pm_err_flag_type pm_chg_charger_transistor_vmaxsel( \
                                                 pm_chg_q_vmax_type vmax);

/*===========================================================================

FUNCTION pm_chg_pulse_charger_config           EXTERNAL FUNCTION

DESCRIPTION
     This function configures the pulse charger.

  Notes: 
     1) If you want to re-start pulse charging after the first time pulse 
        charging is done. Disable and then re-enable pulse charging.
        
     2) Battery Transistor needs to be open for pulse charging to work.
        Refer to pm_chg_battery_transistor_switch(PM_OFF_CMD).
        
     3) PM6610 only: Battery Transistor needs to be close for Fast Charger 
        to resume.
        Refer to pm_chg_battery_transistor_switch(PM_ON_CMD).
        
     4) WARNING!!!: Battery removal while high side pulse charging
                    will reset the system.

     5) This API applies for both Wall and USB charger.
                              
  EXAMPLE:
     // Open the Battery Transistor. 
     err |= pm_chg_battery_transistor_switch(PM_OFF_CMD);

     // Set up Pulse charger type, Tdone, Ton and Toff. 
     pulse_chg_cfg.p_type    = PM_PULSE_CHG_Q__HIGH_SIDE;
     pulse_chg_cfg.t_done    = PM_PULSE_CHG_TDONE__TOFF_X_128;
     pulse_chg_cfg.t_on      = PM_PULSE_CHG_TON__500ms;
     pulse_chg_cfg.t_off     = PM_PULSE_CHG_TOFF__500ms;

     // Configure pulse charger type and timers. 
     err |= pm_chg_pulse_charger_config(&pulse_chg_cfg);
     
     // Configure vbatdet. 
     err |= pm_chg_pulse_charger_vbatdet_config(4200);

     // Enable pulse charging. 
     err |= pm_chg_pulse_charger_switch(PM_ON_CMD);

     // Find out if there was an PMIC API error.
     if (err != PM_ERR_FLAG__SUCCESS) 
     {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
     }
    
INPUT PARAMETERS
  1) Parameter name: settings.
     - Structure pointer containing the pulse charger settings.

     Parameter type: pm_chg_pulse_chg_config_type (struct).
     - Valid Inputs: 
          1.1) Structure member: p_type
               - Selects the charger pass transistor or the battery FET for 
                 pulse charging.
          
               Type: pm_pulse_chg_q_side_type (enum)
               - Valid Inputs:                
                    PM_PULSE_CHG_Q__LOW_SIDE:  Battery FET.
                    PM_PULSE_CHG_Q__HIGH_SIDE: Charger pass transistor.
   
          1.2) Structure member: t_on
               - Selects the charging on time during pulse charging.
          
               Type: pm_pulse_chg_ton_type (enum)
               - Valid Inputs:                
                    PM_PULSE_CHG_TON__62p5ms
                    PM_PULSE_CHG_TON__125ms
                    PM_PULSE_CHG_TON__250ms
                    PM_PULSE_CHG_TON__500ms
                    
          1.3) Structure member: t_off
               - Selects the charging off time during pulse charging.
          
               Type: pm_pulse_chg_toff_type (enum)
               - Valid Inputs:                
                    PM_PULSE_CHG_TOFF__62p5ms
                    PM_PULSE_CHG_TOFF__125ms
                    PM_PULSE_CHG_TOFF__250ms
                    PM_PULSE_CHG_TOFF__500ms
                                      
          1.4) Structure member: t_done
               - Selects the number of consecutive Toff cycles 
                 (VBAT voltage > VBATDET) required for PMIC to determine 
                 that the battery is charged. The PMIC will stop pulse charging 
                 after T_done expires.
          
               Type: pm_pulse_chg_tdone_type (enum)
               - Valid Inputs:                
                    PM_PULSE_CHG_TDONE__TOFF_X_16:   t_off * 16.
                    PM_PULSE_CHG_TDONE__TOFF_X_32:   t_off * 32.
                    PM_PULSE_CHG_TDONE__TOFF_X_64:   t_off * 64.
                    PM_PULSE_CHG_TDONE__TOFF_X_128:  t_off * 128.
                                       
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
                                           (settings = NULL pointer)
    - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter 2 is out of range.
                                           (1st item inside "settings" struct)
    - PM_ERR_FLAG__PAR3_OUT_OF_RANGE     = Input Parameter 3 is out of range.
                                           (2nd item inside "settings" struct)
    - PM_ERR_FLAG__PAR4_OUT_OF_RANGE     = Input Parameter 4 is out of range.
                                           (3rd item inside "settings" struct)
    - PM_ERR_FLAG__PAR5_OUT_OF_RANGE     = Input Parameter 5 is out of range.
                                           (4th item inside "settings" struct)
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_pulse_charger_config
                                  (const pm_chg_pulse_chg_config_type *settings);

/*===========================================================================

FUNCTION pm_chg_pulse_charger_switch                 EXTERNAL FUNCTION

DESCRIPTION
     This function enables and disables pulse charging. 
     
  Notes: 
     1) If you want to re-start pulse charging after the first time pulse 
        charging is done. Disable and then re-enable pulse charging.
        
     2) Battery Transistor needs to be open for pulse charging to work.
        Refer to pm_chg_battery_transistor_switch(PM_OFF_CMD).
        
     3) PM6610 only: Battery Transistor needs to be close for Fast Charger 
        to resume.
        Refer to pm_chg_battery_transistor_switch(PM_ON_CMD).

     4) WARNING!!!: Battery removal while high side pulse charging
                    will reset the system.
                    
     5) This API applies for both Wall and USB charger.
                            
  EXAMPLE:
     // CONFIGURE AND ENABLE HIGH SIDE PULSE CHARGING:
     
     // Open the Battery Transistor. 
     err |= pm_chg_battery_transistor_switch(PM_OFF_CMD);

     // Set up Pulse charger type, Tdone, Ton and Toff. 
     pulse_chg_cfg.p_type    = PM_PULSE_CHG_Q__HIGH_SIDE;
     pulse_chg_cfg.t_done    = PM_PULSE_CHG_TDONE__TOFF_X_128;
     pulse_chg_cfg.t_on      = PM_PULSE_CHG_TON__500ms;
     pulse_chg_cfg.t_off     = PM_PULSE_CHG_TOFF__500ms;

     // Configure pulse charger type and timers. 
     err |= pm_chg_pulse_charger_config(&pulse_chg_cfg);
     
     // Configure vbatdet. 
     err |= pm_chg_pulse_charger_vbatdet_config(4200);

     // Enable pulse charging. 
     err |= pm_chg_pulse_charger_switch(PM_ON_CMD);

     // Find out if there was an PMIC API error.
     if (err != PM_ERR_FLAG__SUCCESS) 
     {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
     }
    
INPUT PARAMETERS
  1) Parameter name: OnOff.
     - Enable/Disable pulse charging.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD
            PM_ON_CMD
                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_chg_pulse_charger_config()
  4) pm_chg_pulse_charger_vbatdet_config()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_pulse_charger_switch( \
                                                pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_pulse_charger_vbatdet_config         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the value for VBATDET. 
    
    VBATDET = threshold voltage to begin pulse charging. The PMIC will
              pulse charge the battery until VBATT stays above VBADET for
              more than Tdone.
              
    NOTES: 
     1) If you want to re-start pulse charging after the first time pulse 
        charging is done. Disable and then re-enable pulse charging.
        
     2) Battery Transistor needs to be open for pulse charging to work.
        Refer to pm_chg_battery_transistor_switch(PM_OFF_CMD).
        
     3) PM6610 only: Battery Transistor needs to be close for Fast Charger 
        to resume.
        Refer to pm_chg_battery_transistor_switch(PM_ON_CMD).
        
     4) WARNING!!!: Battery removal while high side pulse charging
                    will reset the system.              
     
     5) This API applies for both Wall and USB charger.
              
    EXAMPLE:
     // Open the Battery Transistor. 
     err |= pm_chg_battery_transistor_switch(PM_OFF_CMD);

     // Set up Pulse charger type, Tdone, Ton and Toff. 
     pulse_chg_cfg.p_type    = PM_PULSE_CHG_Q__HIGH_SIDE;
     pulse_chg_cfg.t_done    = PM_PULSE_CHG_TDONE__TOFF_X_128;
     pulse_chg_cfg.t_on      = PM_PULSE_CHG_TON__500ms;
     pulse_chg_cfg.t_off     = PM_PULSE_CHG_TOFF__500ms;

     // Configure pulse charger type and timers. 
     err |= pm_chg_pulse_charger_config(&pulse_chg_cfg);
     
     // Configure vbatdet. 
     err |= pm_chg_pulse_charger_vbatdet_config(4200);

     // Enable pulse charging. 
     err |= pm_chg_pulse_charger_switch(PM_ON_CMD);

     // Find out if there was an PMIC API error.
     if (err != PM_ERR_FLAG__SUCCESS) 
     {
       MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
     }
        
INPUT PARAMETERS
  1) Parameter name: vlt
     - VBATDET in mV.

     Parameter type: uint16 
     - Valid Inputs: 4000 mV to 4375 mV in 25 mV steps
                            
       NOTE: Voltage values between voltage steps will be rounded up to
             the next voltage setting.
                      
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_pulse_charger_vbatdet_config(uint16 vlt);

/*===========================================================================

FUNCTION pm_chg_wall_power_ctrl_limit                    EXTERNAL FUNCTION

DESCRIPTION
     This function sets the maximum power limit (PMAXSEL) of the wall 
   charger transistor.
                      
INPUT PARAMETERS
  1) Parameter name: pmaxsel.
     - Maximum power limit (PMAXSEL) of the wall charger transistor.

     Parameter type: pm_chg_pmaxsel_type.
     - Valid Inputs: 
     
        PM_CHG_PMAXSEL__DISABLE
        PM_CHG_PMAXSEL__400mW
        PM_CHG_PMAXSEL__500mW
        PM_CHG_PMAXSEL__600mW
        PM_CHG_PMAXSEL__750mW
        PM_CHG_PMAXSEL__1000mW
        PM_CHG_PMAXSEL__1500mW
        PM_CHG_PMAXSEL__2000mW     
                      
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_wall_power_ctrl_limit(   \
                                                pm_chg_pmaxsel_type pMaxSel);

/*===========================================================================

FUNCTION pm_chg_trickle_config                           EXTERNAL FUNCTION

DESCRIPTION
    This function configures the PMIC's trickle charge current. To
  disable trickle charge, Set the trickle charge current to 0.
      
  example:

    // To trickle Charge:
    // 1) Turn off the battery transistor.
    err |= pm_chg_wall_charger_transistor_switch(PM_OFF_CMD);
    
    // 2) Enable trickle charge by configuring the trickle charge current.
    err |= pm_chg_trickle_config(PM_CHG_TR_IMAX_50mA);
    
INPUT PARAMETERS
  Parameter name: imax.
  - The maximum amount of current allowed while trickle charging.

  Parameter type: pm_chg_tr_imax_type (enum).
  - Valid Inputs:
 
        PM_CHG_TR_IMAX_00mA,
        PM_CHG_TR_IMAX_20mA
        PM_CHG_TR_IMAX_30mA
        PM_CHG_TR_IMAX_40mA
        PM_CHG_TR_IMAX_50mA
        PM_CHG_TR_IMAX_60mA
        PM_CHG_TR_IMAX_70mA
        PM_CHG_TR_IMAX_80mA
           
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.        
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED.

===========================================================================*/
extern pm_err_flag_type  pm_chg_trickle_config(pm_chg_tr_imax_type imax_tr);

/*===========================================================================

FUNCTION pm_chg_vcp_switch                             EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables VCP (voltage collapse protection)
      
    VCP: If the charger can not provide enough power to keep the system
         operating then the VCP circuit (when enable) will close the battery 
         transistor so that the battery can assist the charger.
         
  example:
  
   // Enable VCP.
   err |= pm_chg_vcp_switch(PM_ON_CMD);
   
   // Find out if there was a PMIC API error.
   if (err != PM_ERR_FLAG__SUCCESS) 
   {
      MSG_ERROR(" PMIC API ERROR(0x%x).",errFlag,0,0);
   }
   
    
INPUT PARAMETERS
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enable VCP.
        PM_OFF_CMD = Disable VCP.

                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_vcp_switch(pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_bat_alarm_config                         EXTERNAL FUNCTION

DESCRIPTION
    This function configures the battery alarms(upper and lower thresholds).
         
    EXAMPLE:
    // Configure the PMIC to trigger the battery alarm if the battery
    // voltage goes above 4.1V or bellow 3.9V for more than 976 usecs.
    err | =pm_err_flag_type pm_chg_bat_alarm_config(
                                        4100,
                                        3900,
                                        PM_CHG_BATT_ALARM_HYST__976us);
    
INPUT PARAMETERS
  1) Parameter name: upper_vlt.
     - Upper battery voltage threshold.

     Parameter type: uint16.
     - Valid inputs:
       2800 mV to 4300 mV in 100 mV steps.
       
  2) Parameter name: lower_vlt.
     - lower battery voltage threshold.
              
     Parameter type: uint16.
     - Valid inputs:
       2800 mV to 4300 mV in 100 mV steps.
       
  3) Parameter name: pm_alarm_hyst.
     - Time hysteresis.
     
     Parameter type: pm_chg_batt_alarm_hyst_type.
     - Valid inputs:     
            PM_CHG_BATT_ALARM_HYST__122us
            PM_CHG_BATT_ALARM_HYST__244us
            PM_CHG_BATT_ALARM_HYST__488us
            PM_CHG_BATT_ALARM_HYST__976us     
            
   NOTE: Voltage values between voltage steps will be rounded up to
         the next voltage setting.            
                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = Input Parameter 2 is out of range.    
    - PM_ERR_FLAG__PAR3_OUT_OF_RANGE     = Input Parameter 3 is out of range.        
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_bat_alarm_config(
                                uint16                       upper_vlt, 
                                uint16                       lower_vlt, 
                                pm_chg_batt_alarm_hyst_type  pm_alarm_hyst);


/*===========================================================================

FUNCTION pm_chg_bat_alarm_read_status                    EXTERNAL FUNCTION

DESCRIPTION
     This function returns which threshold trigger the battery alarm.     
  
  EXAMPLE:
  
  boolean upper_thr;
  boolean lower_thr;
  
  // Get the battery alarm status.
  err |= pm_err_flag_type pm_chg_bat_alarm_read_status(&upper_thr,
                                                       &lower_thr);
                                                       
  if (upper_thr == TRUE) 
  {
    // Battery voltage above upper threshold.
  }
  else if  (lower_thr == TRUE) 
  {
    // Battery voltage bellow lower threshold.
  } 
    
INPUT PARAMETERS
  1) Parameter name: .

     Parameter type: .
     - Valid inputs:

                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_bat_alarm_read_status( boolean *upper_thr,
                                                      boolean *lower_thr );

/*===========================================================================

FUNCTION pm_chg_bat_alarm_switch                         EXTERNAL FUNCTION

DESCRIPTION
    This function enable and disable the battery alarms (upper and lower 
  battery thresholds)
         
  EXAMPLE:
     // Enable the battery alarms.
     err |= pm_chg_bat_alarm_switch(PM_ON_CMD);
    
INPUT PARAMETERS
  1) Parameter name: OnOff.
     - Enable and disable the battery alarms.
     
     Parameter type: pm_switch_cmd_type.
     - Valid inputs:
            PM_OFF_CMD
            PM_ON_CMD

RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_chg_bat_alarm_config()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_bat_alarm_switch(pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_charger_pwr_up_def_st                 EXTERNAL FUNCTION

DESCRIPTION
  This function configures the PMIC to power up using the battery or 
  the USB charger as the default power source (if the wall charger is not 
  available), and using the battery or wall (default) charger if there is 
  no dedicated path for USB charging
   
INPUT PARAMETER
  Parameter name: pwr_source.
  - Power up power source default setting.
  
  Parameter type: pm_chg_power_on_type.
  - Valid inputs:
       PM_CHG_POWER_ON_FROM__CHG = Power up using the default charger 
                                   (if there is no dedicated USB charging path) 
       PM_CHG_POWER_ON_FROM__BAT = Power up using the battery 
       PM_CHG_POWER_ON_FROM__USB_CHG = Power up using the USB charger 
                                   (if there is dedicated USB charging path) 
                                         
       Note: This bit will go into its default state if all power 
              (except coin cell) is removed from the PMIC.

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  1) Interrupts are LOCKED while we are communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_chg_charger_pwr_up_def_st(pm_chg_power_on_type pwr_source);

/*===========================================================================

FUNCTION pm_is_charger_supported                      EXTERNAL FUNCTION

DESCRIPTION
    Verifies that the PMIC's hardware support regular charging.
         
INPUT PARAMETERS
  Parameter name: chg_support.
  - Returns if the PMIC supports regular charging or not.

  Parameter type: boolean (pointer).
  - Valid Outputs:
        TRUE : Regular Charging is supported.
        FALSE: Regular Charging is not supported.
           
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.        
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED.

===========================================================================*/
extern pm_err_flag_type  pm_is_charger_supported( boolean *chg_support );

/*===========================================================================

                 COIN CELL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION pm_coin_cell_chg_switch                        EXTERNAL FUNCTION

DESCRIPTION
    This function turns ON/OFF charging of the coin cell.

  EXAMPLE:
     // Configure the coin cell charger voltage to 3.1 Volts and
     // Current Limiting resistor setting to 1.2K Ohms. 
     err = pm_coin_cell_chg_config(PM_COIN_CELL_VSET_3p1V,
                                   PM_COIN_CELL_RSET_1200_OHMS);
     // Enable coin cell charging.
     err = pm_coin_cell_chg_switch(PM_ON_CMD);
         
INPUT PARAMETERS
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = Enables charging of the coin cell.  By default
                     (without calling pm_coin_cell_chg_config()), 
                     turning on the coin cell charger will configure
                     the charger voltage to 3.2 Volts and the 
                     current resistor setting to 2.1K Ohms.
                     
        PM_OFF_CMD = Disables charging of the coin cell. (default)
        
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                 
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  Call the following functions before calling this function:
  1) rflib_init()
  2) pm_init()
  3) pm_coin_cell_chg_config()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_coin_cell_chg_switch(pm_switch_cmd_type cmd);

/*===========================================================================

FUNCTION pm_coin_cell_chg_config                        EXTERNAL FUNCTION

DESCRIPTION
    This function configures the coin cell charger voltage and current 
  limiting resistor value.
  
  EXAMPLE:
     // Configure the coin cell charger voltage to 3.1 Volts and
     // Current Limiting resistor setting to 1.2K Ohms. 
     err = pm_coin_cell_chg_config(PM_COIN_CELL_VSET_3p1V,
                                   PM_COIN_CELL_RSET_1200_OHMS);
     // Enable coin cell charging.
     pm_coin_cell_chg_switch(PM_ON_CMD);
    
INPUT PARAMETERS
  1) Parameter name: vset.
     - Configures the coin cell charger voltage.
     
     Parameter type: pm_coin_cell_vset_type.
     - Valid inputs:
        PM_COIN_CELL_VSET_3p0V  = 3.0 Volts.
        PM_COIN_CELL_VSET_3p1V  = 3.1 Volts.
        PM_COIN_CELL_VSET_3p2V  = 3.2 Volts. (Default)
    
  2) Parameter name: rset.
     - Configures the coin cell charger current limiting resistor setting.
     
     Parameter type: pm_coin_cell_rset_type.
     - Valid inputs:
        PM_COIN_CELL_RSET_2100_OHMS  = 2.1K Ohms. (Default)
        PM_COIN_CELL_RSET_1700_OHMS  = 1.7K Ohms.
        PM_COIN_CELL_RSET_1200_OHMS  = 1.2K Ohms.
        PM_COIN_CELL_RSET_800_OHMS   = 800  Ohms.                 
       
          
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter 1 is invalid.
     PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = The input parameter 2 is invalid.
     PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                          version of the PMIC.         
     PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                          communicate with the PMIC.
     PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  Call the following functions before calling this function:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are LOCKED while in this function.

===========================================================================*/
extern pm_err_flag_type pm_coin_cell_chg_config(pm_coin_cell_vset_type vset,
                                                pm_coin_cell_rset_type rset);

/*===========================================================================

FUNCTION pm_chg_usb_charger_transistor_switch         EXTERNAL FUNCTION

DESCRIPTION
    This function Opens/Closes (ON/OFF, via USB_CTL_N pin) the charger 
   transistor.  
   
   example:
   
   // Close (turn ON) the USB charger transistor.
   err |= pm_chg_usb_charger_transistor_switch(PM_ON_CMD);
            
INPUT PARAMETER
  Parameter name: OnOff.

  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  = USB Charger Transistor ON  (close).
        PM_OFF_CMD = USB Charger Transistor OFF (open).

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
pm_err_flag_type pm_chg_usb_charger_transistor_switch(
                                                pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_usb_charger_transistor_imaxsel           EXTERNAL FUNCTION

DESCRIPTION
    This function updates the USB charger transistor maximum current 
  (USB_IMAXSEL).
  
  example:
  
    // Set the charger transistor maximum current (IMAXSEL) = 1A.  
    //     Where IMAX = (100mV)/R and Rsense = 0.1 ohms.
        
    err |= pm_chg_usb_charger_transistor_limits(
                                PM_CHG_UCHG_Q_IMAX_100mV_div_R);
                                
    NOTE: USB Charger VMAXSEL, same as Wall Charger VMAXSEL, please refer to
          pm_chg_wall_charger_transistor_limits().
    
INPUT PARAMETERS
  1) Parameter name: usb_imaxsel.
     - USB Charger transistor maximum current.

     Parameter type: pm_chg_uchg_q_imax_type .
     - Valid Inputs:  
            PM_CHG_UCHG_Q_IMAX_010mV_div_R = IMAX = (010mV)/R.
            PM_CHG_UCHG_Q_IMAX_020mV_div_R = IMAX = (020mV)/R.
            PM_CHG_UCHG_Q_IMAX_030mV_div_R = IMAX = (030mV)/R.
            PM_CHG_UCHG_Q_IMAX_040mV_div_R = IMAX = (040mV)/R.
            PM_CHG_UCHG_Q_IMAX_050mV_div_R = IMAX = (050mV)/R.
            PM_CHG_UCHG_Q_IMAX_060mV_div_R = IMAX = (060mV)/R.
            PM_CHG_UCHG_Q_IMAX_070mV_div_R = IMAX = (070mV)/R.
            PM_CHG_UCHG_Q_IMAX_080mV_div_R = IMAX = (080mV)/R.
            PM_CHG_UCHG_Q_IMAX_090mV_div_R = IMAX = (090mV)/R.
            PM_CHG_UCHG_Q_IMAX_100mV_div_R = IMAX = (100mV)/R.  
            PM_CHG_UCHG_Q_IMAX_110mV_div_R = IMAX = (110mV)/R.
            PM_CHG_UCHG_Q_IMAX_120mV_div_R = IMAX = (120mV)/R.
            PM_CHG_UCHG_Q_IMAX_130mV_div_R = IMAX = (130mV)/R.
            PM_CHG_UCHG_Q_IMAX_140mV_div_R = IMAX = (140mV)/R.
            PM_CHG_UCHG_Q_IMAX_150mV_div_R = IMAX = (150mV)/R.
            PM_CHG_UCHG_Q_IMAX_160mV_div_R = IMAX = (160mV)/R.
            PM_CHG_UCHG_Q_IMAX_170mV_div_R = IMAX = (170mV)/R.
            PM_CHG_UCHG_Q_IMAX_180mV_div_R = IMAX = (180mV)/R.
            PM_CHG_UCHG_Q_IMAX_190mV_div_R = IMAX = (190mV)/R.
            PM_CHG_UCHG_Q_IMAX_200mV_div_R = IMAX = (200mV)/R.         
                   
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED.

===========================================================================*/
extern pm_err_flag_type pm_chg_usb_charger_transistor_imaxsel( \
                                       pm_chg_uchg_q_imax_type usb_imaxsel);

/*===========================================================================

FUNCTION pm_chg_usb_power_ctrl_limit                    EXTERNAL FUNCTION

DESCRIPTION
     This function sets the maximum power limit (PMAXSEL) of the USB
   charger transistor.
                      
INPUT PARAMETERS
  1) Parameter name: pmaxsel.
     - Maximum power limit (PMAXSEL) of the usb charger transistor.

     Parameter type: pm_chg_pmaxsel_type.
     - Valid Inputs: 
     
        PM_CHG_PMAXSEL__DISABLE
        PM_CHG_PMAXSEL__400mW
        PM_CHG_PMAXSEL__500mW
        PM_CHG_PMAXSEL__600mW
        PM_CHG_PMAXSEL__750mW
        PM_CHG_PMAXSEL__1000mW
        PM_CHG_PMAXSEL__1500mW     
        PM_CHG_PMAXSEL__2000mW
                      
RETURN VALUE
  Return value type: pm_err_flag_type.
  
    - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter 1 is out of range.
    - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                           version of the PMIC.               
    - PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                           communicate with the PMIC.    
    - PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while we are communicating with the PMIC.

===========================================================================*/
extern pm_err_flag_type pm_chg_usb_power_ctrl_limit( \
                                                pm_chg_pmaxsel_type pMaxSel);

/*===========================================================================

FUNCTION pm_chg_usb_charger_suspend_mode_switch         EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables PMIC's USB Charger suspend mode. Putting
  the PMIC USB Charger in suspend mode configures the PMIC's USB charger
  circuit not to draw power from the VBUS line if the wall charger is not
  present or is weaker than VBUS.
   
  EXAMPLE:
  // To put the PMIC USB Charger in suspend mode. 
  
  // 1) Disable USB charging.
  err =  pm_chg_usb_charger_transistor_switch(PM_OFF_CMD);
  // 2) Put the USB Charger into suspend mode.
  err |= pm_chg_usb_charger_suspend_mode_switch(PM_ON_CMD);
  
  // Find out if there was an PMIC API error. 
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }  
  
  NOTES: 1) The PMIC USB valid/invalid charger IRQs will not trigger 
            while in suspend mode.
         2) USB charging must be disable for this feature to take effect. 
            Please refer to pm_chg_usb_charger_transistor_switch().
            
INPUT PARAMETER
  Parameter name: OnOff.
  - Enable/Disable USB Charger suspend mode.
  
  Parameter type: pm_switch_cmd_type.
  - Valid inputs:
        PM_ON_CMD  =  Go into suspend mode.
        PM_OFF_CMD =  Disable suspend mode (default). 

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()
  3) USB charging must be disable for this feature to take effect. 
     Please refer to pm_chg_usb_charger_transistor_switch().

SIDE EFFECTS
  1) Interrupts are LOCKED while we are communicating with the PMIC.
  2) USB Charger Valid/invalid interrupts will not trigger if the USB
     charger is in suspend mode.

===========================================================================*/
extern pm_err_flag_type pm_chg_usb_charger_suspend_mode_switch(  \
                                                  pm_switch_cmd_type OnOff);

/*===========================================================================

FUNCTION pm_chg_usb_charger_pwr_up_def_st                 EXTERNAL FUNCTION

DESCRIPTION
    This function configures the PMIC to power up using the battery or 
  the USB charger as the default power source (if the wall charger is not 
  available).. 
   
  EXAMPLE:
  // Configure the PMIC to use the battery (USB charger disable) as the 
  // default power source (if wall charger not available).
  err |= pm_chg_usb_charger_pwr_up_def_st(PM_CHG_USB_POWER_ON_FROM__BAT);
  
  // Find out if there was an PMIC API error. 
  if (err != PM_ERR_FLAG__SUCCESS) 
  {
      MSG_ERROR(" PMIC API ERROR(0x%x).",err,0,0);
  }
            
INPUT PARAMETER
  Parameter name: pwr_source.
  - Power up power source default setting.
  
  Parameter type: pm_chg_usb_power_on_type.
  - Valid inputs:
        PM_CHG_USB_POWER_ON_FROM__BAT  = Power up using the battery.
        PM_CHG_USB_POWER_ON_FROM__UCHG = Power up using the USB Charger.
                                         (Default)
                                         
        Note: This bit will go into its default state if all power 
              (except coin cell) is removed from the PMIC.

RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
        PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter is invalid.
        PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
                                             version of the PMIC.                   
        PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                             communicate with the PMIC.        
        PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  1) Interrupts are LOCKED while we are communicating with the PMIC.
===========================================================================*/
extern pm_err_flag_type pm_chg_usb_charger_pwr_up_def_st(  \
                                        pm_chg_usb_power_on_type pwr_source);
/*===========================================================================

FUNCTION pm_chg_get_battery_transistor_status               EXTERNAL FUNCTION

DESCRIPTION
  This function gets the status of the battery transistor.
  
    
PARAMETER
  Return value name: OnOff.
     
  Return value type: pm_switch_cmd_type*.
    - Valid values:
        PM_ON_CMD  = Battery Transistor ON  (close).
        PM_OFF_CMD = Battery Transistor OFF (open).
          
RETURN VALUE
  Description:
  - Error Flag.
  
  Return value type: pm_err_flag_type.        
  - Valid output:
     PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = NULL pointer passed through parameter.
     PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to 
                                          communicate with the PMIC.
     PM_ERR_FLAG__SUCCESS               = Operation was successful.

DEPENDENCIES
  Call the following functions before calling this function:
  1) pm_init()

SIDE EFFECTS
  None.
===========================================================================*/
extern pm_err_flag_type pm_chg_get_battery_transistor_status(pm_switch_cmd_type *OnOff);

/*===========================================================================
FUNCTION  pm_chg_is_detection_reqd                       EXTERNAL FUNCTION

DESCRIPTION
   This function determines if the charger removal detection is required.

PARAMETERS
   None

RETURN VALUE
  Description:
  - if the PMIC hardware bug affects the current rev of the PMIC.

  Return value type: boolean.
    TRUE  = The software needs to do the detection.
    FALSE = The software does not need to do the detection.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  None
===========================================================================*/
extern boolean pm_chg_is_detection_reqd(void);

/*===========================================================================

FUNCTION  pm_chg_is_power_limit_irq_driven                EXTERNAL FUNCTION

DESCRIPTION
   This function checks if the PMIC charger power limit control scheme 
is interrupt driven

PARAMETERS
none

RETURN VALUE
  Description:
  - if the PMIC hardware bug affects the current rev of the PMIC.

  Return value type: boolean.
    TRUE  = The software needs to do the detection.
    FALSE = The software does not need to do the detection.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern boolean pm_chg_is_power_limit_irq_driven(void);

/*===========================================================================

FUNCTION  pm_chg_is_internal_pnp_rsns                       EXTERNAL FUNCTION

DESCRIPTION
   This function checks if the PMIC has internal charger pass transistor (PNP)
and sense resistor (RSNS)

PARAMETERS
none

RETURN VALUE
  Description:

  Return value type: boolean.
    TRUE  = PMIC has internal charger pass transistor and sense resistor
    FALSE = PMIC does NOT have internal charger pass transistor and sense resistor

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern boolean pm_chg_is_internal_pnp_rsns( void );

/*===========================================================================

FUNCTION  pm_chg_is_vbatdet_cal_reqd                       EXTERNAL FUNCTION

DESCRIPTION
   This function checks if the PMIC requires software VBATDET calibration
routine to be performed 

PARAMETERS
none

RETURN VALUE
  Description:

  Return value type: boolean.
    TRUE  = PMIC needs to perform software VBATDET calibration routine 
    FALSE = PMIC does NOT need to perform software VBATDET calibration routine 

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern boolean pm_chg_is_vbatdet_cal_reqd( void );

/*===========================================================================

FUNCTION  pm_chg_is_single_chg_path                       EXTERNAL FUNCTION

DESCRIPTION
   This function checks if the PMIC shares a single charging path between
DC wall charger and USB charger 

PARAMETERS
none

RETURN VALUE
  Description:

  Return value type: boolean.
    TRUE  = PMIC has single path shared by DC wall charger and USB charger 
    FALSE = PMIC has separate paths for DC wall charger and USB charger

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern boolean pm_chg_is_single_chg_path( void );
/*===========================================================================

FUNCTION  pm_chg_set_cur_ctrl_value                       EXTERNAL FUNCTION

DESCRIPTION
   This function sets imaxsel value based on the upper current limit and 
   pmic die temperature

PARAMETERS
 1>iupper : This is desired upper imaxsel value set by customer
 example : 1200mA (The resolution of the Iupper value should be 10mA)
                  (also 100mA > iupper < 1200mA)
 2>pmic_die_temp : pmic die temperature
 3>*chg_cur_ctrl_val : This is the imaxsel value based on iupper 
   and pmic_die_temp
   
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern pm_err_flag_type pm_chg_set_cur_ctrl_value(uint16 iupper, \
                                                  int16 pmic_die_temp, \
                                pm_chg_wchg_q_imax_type *chg_cur_ctrl_val);
/*===========================================================================

FUNCTION  pm_chg_set_cur_ctrl_value                       EXTERNAL FUNCTION

DESCRIPTION
   This function sets imaxsel value based on the upper current limit and 
   pmic die temperature

PARAMETERS
 1>iupper : This is desired upper imaxsel value set by customer
 example : 1200mA (The resolution of the Iupper value should be 10mA)
                  (also 100mA > iupper < 1200mA)
 2>pmic_die_temp : pmic die temperature
 3>*chg_cur_ctrl_val : This is the imaxsel value based on iupper 
   and pmic_die_temp
   
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) chg_init()

SIDE EFFECTS
  
===========================================================================*/
extern pm_err_flag_type pm_chg_set_cur_ctrl_value(uint16 iupper, \
                                                  int16 pmic_die_temp, \
                                pm_chg_wchg_q_imax_type *chg_cur_ctrl_val);

/*===========================================================================

FUNCTION  pm_chg_enable_charging                       EXTERNAL FUNCTION

DESCRIPTION
   This function enables charging on PMICs that have a charging enable 
   register 
PARAMETERS
   none
   
RETURN VALUE
  Return value type: pm_err_flag_type.

    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
                                         version of the PMIC.
    PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to communicate
                                         with the PMIC.
    PM_ERR_FLAG__SUCCESS               = SUCCESS.

DEPENDENCIES
  The following function must have been called:
  1) pm_init()

SIDE EFFECTS
  
===========================================================================*/
extern pm_err_flag_type pm_chg_enable_charging(void);

/*===========================================================================

FUNCTION pm_chg_boot_uchg_vreg_setup                           EXTERNAL FUNCTION

DESCRIPTION
    This function turns on some target specific VREGs that are needed for USB charging 
to function correctly in the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    err Error flag.

DEPENDENCIES
    pm_sbl_init

SIDE EFFECTS
    This function should be only used in the bootloader in preparing for 
the USB boot charging support

===========================================================================*/
extern pm_err_flag_type pm_chg_boot_uchg_vreg_setup(void);

/*===========================================================================

FUNCTION pm_chg_boot_uchg_vreg_cleanup                           EXTERNAL FUNCTION

DESCRIPTION
    This function turns off the target specific VREGs that are turned on in  
pm_chg_boot_uchg_vreg_init() 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    err Error flag.

DEPENDENCIES
    pm_sbl_init
    pm_chg_boot_uchg_vreg_init

SIDE EFFECTS
    This function should be only used in the bootloader when exiting 
the USB boot charging support

===========================================================================*/
extern pm_err_flag_type pm_chg_boot_uchg_vreg_cleanup(void);

/******************************************************************************
* New APIs for PMIC4 charger 
******************************************************************************/
/**
 * pm_err_flag_type pm_chg_trickle_enable(boolean enable)
 * 
 * @param enable TRUE:  Enables trickle charger.
 *               FALSE:  Disables trickle charger.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_trickle_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_usb_suspend(boolean enable)
 * 
 * @param enable TRUE:  Enable USB suspend mode:
 *               FALSE:  Disable USB suspend mode.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_usb_suspend(boolean enable);

/**
 * pm_err_flag_type pm_chg_imon_cal_set(int value)
 * 
 * @param value  Range 0 to 1.
 *               1:  Directs constant current source to IMON resistor.
 *               0:  Disconnects constant current source from IMON
 *               resistor.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_imon_cal_set(int value);

/**
 * pm_err_flag_type pm_chg_imon_gain_set(int value)
 * 
 * @param value  Range is 0 or 1.
 *               0: 1.0 V/A
 *               1: 10.0 V/A for higher resolution near end of charge.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_imon_gain_set(int value);

/**
 * pm_err_flag_type pm_chg_charge_bat_enable(boolean enable)
 * 
 * @param enable TRUE:  Closes the battery FET to start charging (BAT_FET_N
 *               is driven low)
 *               FALSE:  Opens the battery FET to stop charging (BAT_FET_N
 *               is driven high)
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_charge_bat_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_vbus_from_boost_override_enable(boolean
 * enable)
 * 
 * @param enable TRUE:  Charger is never disabled by VBUS_FROM_BOOST
 *               FALSE:  Charger is disabled when VBUS_FROM_BOOST = 1
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_vbus_from_boost_override_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_charge_disable(boolean disable)
 * 
 * @param disable TRUE:  Disable charging from wall adapter.
 *                FALSE: Enable charging from wall adapter.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_charge_disable(boolean disable);

/**
 * pm_err_flag_type pm_chg_vcp_enable(boolean enable)
 * 
 * @param enable TRUE: Enable VDD collapse protection
 *               FALSE:  Disable VDD collapse protection
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_vcp_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_atc_disable(boolean disable)
 * 
 * @param disable TRUE:  Disable automatic trickle charging before
 *                startup.
 *                FALSE:  Enable automatic trickle charging.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_atc_disable(boolean disable);

/**
 * pm_err_flag_type pm_chg_auto_disable(boolean disable)
 * 
 * @param disable TRUE:  Disable automatic charging.
 *                FALSE:  Enable automatic charging.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_auto_disable(boolean disable);

/**
 * pm_err_flag_type pm_chg_dumb_chg_ovrd_enable(boolean enable)
 * 
 * @param enable TRUE: The peripheral connected to VCHG/VBUS is always
 *               considered a not intelligent USB charger (regular
 *               wall charger).
 *               FALSE:  The peripheral connected to VCHG/VBUS is
 *               considered a USB intelligent device if DUMB_CHG
 *               = 0.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_dumb_chg_ovrd_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_enum_done_enable(boolean enable)
 * 
 * Enable/disable use of VCHG/VBUS as a power source after
 * enumeration from intelligent USB host.  Software should
 * set appropriate current limit (less than 500 mA) before
 * calling this API.
 * 
 * @param enable TRUE: enable
 *               FALSE: disable
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_enum_done_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_temp_mode_set(int value)
 * 
 * @param value  1:  Continuous regulation
 *               0:  Hysteresis control
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_temp_mode_set(int value);

/**
 * pm_err_flag_type pm_chg_batfet_slow_disable(boolean disable)
 * 
 * @param disable TRUE:  Disables slow battery FET turnon.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_batfet_slow_disable(boolean disable);

/**
 * pm_err_flag_type pm_chg_batt_temp_disable(boolean disable)
 * 
 * @param disable TRUE:  Disables battery temperature sensing comparators and
 *                battery temperature reference output.
 *                FALSE: don't disable
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_batt_temp_disable(boolean disable);

/**
 * pm_err_flag_type pm_chg_failed_clear(void)
 * 
 * @param clear TRUE:  Writes to PMIC to exit a failed charging
 *                     state.
 *              FALSE: set to default value.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_failed_clear(boolean clear);

/**
 * pm_err_flag_type pm_chg_vmaxsel_set(uint32 value_mv)
 * 
 * Sets the charger regulator output voltage.
 * 
 * @param value_mv Charger regulator output voltage in mV.
 *                 Range is 2400 mV to 5575 mV
 * 
 * @return pm_err_flag_type
 */

extern pm_err_flag_type pm_chg_vmaxsel_set(uint32 value_mv);

/**
 * pm_err_flag_type pm_chg_vbatdet_set(uint32 value_mv)
 * 
 * Sets the comparator triggering voltage.
 * 
 * @param value_mv Comparator triggering voltage in mV.
 *                 Range is 2400 mV to 5575 mV
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_vbatdet_set(uint32 value_mv);

/**
 * pm_err_flag_type pm_chg_imaxsel_set(uint32 value_ma)
 * 
 * Sets the fast charge current limit.
 * 
 * @param value_ma The fast charge current limit in mA.
 *                 Range is 50 mA to 1600 mA.
 *                 Note:  Calling this function with a value_ma=100 will
 *                 result in a fast charge current limit of 90 mA.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_imaxsel_set(uint32 value_ma);

/**
 * pm_err_flag_type pm_chg_vtrkl_set(uint32 value_mv)
 * 
 * Sets the voltage below which (USB) auto trickle 
 * charging must start.
 * 
 * @param value_mv Trickle charge start voltage in mV.
 *                 Range is 1800 mV to 3300 mV.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_vtrkl_set(uint32 value_mv);

/**
 * pm_err_flag_type pm_chg_itrkl_set(uint32 value_ma)
 * 
 * Sets the trickle charge current.
 * 
 * @param value_ma Trickle charge current in mA.
 *                 Range is 10 to 160 mA
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_itrkl_set(uint32 value_ma);

/**
 * pm_err_flag_type pm_chg_iterm_set(uint32 value_ma)
 * 
 * Sets the end-of-charge (termination) current used to generate
 * the ICHG_END interrupt and terminate atuomatic fast charging
 * 
 * @param value_ma Termination current in milliamps.
 *                 Range is 10 mA to 160 mA
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_iterm_set(uint32 value_ma);

/**
 * pm_err_flag_type pm_chg_ttrkl_set(uint32 value_minutes)
 * Sets the maximum time for automatic trickle charging.
 * 
 * @param value_minutes Maximum time for auto trickle charging.
 *                      Must be between 1 and 128 minutes.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_ttrkl_set(uint32 value_minutes);

/**
 * pm_err_flag_type pm_err_flag_type pm_chg_tchg_set(uint32 value_minutes)
 * 
 * Sets the maximum time for automatic fast charging.
 * 
 * @param value_minutes This is the maximum time for automatic fast charging.
 *                      Must be between 4 and 512 (8h32m) minutes.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_tchg_set(uint32 value_minutes);

/**
 * pm_err_flag_type pm_chg_temp_thresh_set(uint32 stop_deg_c,
 *                                         uint32 resume_deg_c)
 * 
 * Sets temperature on/off threshold.
 * 
 * @param stop_deg_c   Maximum charging temperature in hysteresis mode:
 *                     charging stops-pass device turns off at this temperature.
 *                     Must be between 75 and 130 degrees C.
 * @param resume_deg_c In hysteresis mode, pass device turns on, charging
 *                     resumes when the pass device has fallen below this
 *                     temperature.
 *                     Must be between 75 and 130 degrees C.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_temp_thresh_set(uint32 stop_deg_c, uint32 resume_deg_c);

/**
 * pm_chg_temp_limit_set(uint32 temp_limit_deg_c)
 * 
 * @param temp_limit_deg_c
 *               In continuous regulation mode, pass device drive is
 *               controlled to regulate the pass device at this
 *               temperature.
 *               Temperature must be between 75 and 130 degrees C.
 * 
 * @return pm_err_flag_type
 * @example pm_err_flag_type err;
 * err = pm_chg_temp_limit_set(115);
 * Sets the temperature limit to 115 degrees C.
 */
extern pm_err_flag_type pm_chg_temp_limit_set(uint32 temp_limit_deg_c);

/**
 * pm_err_flag_type pm_chg_charge_state_get(pm_chg_state_name_type *charge_state)
 * 
 * @param charge_state Used to return the value of the current
 *                     state of the automatic charger.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_charge_state_get(pm_auto_chg_state_name_type *charge_state);

/**
 * pm_err_flag_type pm_chg_charge_state_set(pm_auto_chg_state_name_type charge_state)
 * 
 * This function includes a 2 mS delay that is required by the
 * PMIC hardware.  This function should not be called from an 
 * interrupt or other time-critical code.
 * 
 * @param charge_state Used to set the value of the current
 *                     state of the automatic charger.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_charge_state_set(pm_auto_chg_state_name_type charge_state);

/**
 * boolean pm_chg_is_auto_charging_supported(void)
 * 
 * Returns TRUE if automatic charging is supported, FALSE if
 * not.
 * 
 * @return boolean
 */
extern boolean pm_chg_is_auto_charging_supported(void);

/**
 * pm_err_flag_type pm_chg_usb_chg_det_get(
 *                     pm_chg_usb_chg_det_type *usb_chg_det)
 * 
 * @param usb_chg_det Used to return the value of the USB charge detection
 *                    setting in the PMIC.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_usb_chg_det_get(
                              pm_chg_usb_chg_det_type *usb_chg_det);

/**
 * pm_err_flag_type  pm_chg_usb_chg_det_enable(boolean enable)
 * 
 * @param enable TRUE:  Enable USB charger detection.
 *               FALSE:  Disable USB charger detection.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type  pm_chg_usb_chg_det_enable(boolean enable);

/**
 * pm_err_flag_type pm_chg_cleanup(void)
 * 
 * Restore PMIC4 charger registers to their power-on default state.
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_chg_cleanup(void);

#endif /* PM66XXCHG_H */

#if !defined(PMSBL_H)
#define PMSBL_H
/*===========================================================================


                  P M S B L   H E A D E R    F I L E

GENERAL DESCRIPTION
     This file contains functions to manage and monitor battery charging for
the special case when the phone is booted from USB and the battery voltage
is too low and thus requires charging. NOTE: The functions defined in this file
are meant to be called from the bootloader only. 

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007, 2008, 2009       by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pmsbl.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   hs      Added pm_sbl_ext_usb_is_valid() API
09/12/06   cng     Added pm_sbl_enable_usb_chg API
10/04/05   cng     Initial version

============================================================================*/

/*===========================================================================

                     GLOBAL TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_sbl_init                           EXTERNAL FUNCTION

DESCRIPTION
    This function does the necessary initialization for PMIC when called from
the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    NONE

DEPENDENCIES
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_init(void);


/*===========================================================================

FUNCTION pm_sbl_enable_vreg_usb                           EXTERNAL FUNCTION

DESCRIPTION
    This function enables or disables the USB voltage regulator when called 
from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: enable.
        Valid Inputs:
            -- TRUE: enable USB voltage regulator
            -- FALSE: disable USB voltage regulator
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_enable_vreg_usb(boolean enable);


/*===========================================================================

FUNCTION pm_sbl_usb_otg_shutdown                           EXTERNAL FUNCTION

DESCRIPTION
    This function enables or disables SHUTDOWN mode for the USB transceiver
when called from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: shutdown
        Valid Inputs:
            -- TRUE: enable SHUTDOWN mode for USB transceiver
            -- FALSE: disable SHUTDOWN mode for USB transceiver
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_usb_otg_shutdown(boolean shutdown);


/*===========================================================================

FUNCTION pm_sbl_usb_sess_is_valid                           EXTERNAL FUNCTION

DESCRIPTION
    This function determines whether a USB session is valid or not when called
from the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    Return Value Type: boolean 
        -- TRUE: USB session is valid 
        -- FALSE: USB session is invalid

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern boolean pm_sbl_usb_sess_is_valid(void);


/*===========================================================================

FUNCTION pm_sbl_ext_chg_is_invalid                           EXTERNAL FUNCTION

DESCRIPTION
    This function determines whether an external DC wall charger is valid
or not when called from the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    Return Value Type: boolean 
        -- TRUE: DC wall charger is invalid 
        -- FALSE: DC wall charger is valid 

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern boolean pm_sbl_ext_chg_is_invalid(void);

/*===========================================================================

FUNCTION pm_sbl_ext_usb_is_valid                           EXTERNAL FUNCTION

DESCRIPTION
    This function determines whether an external USB device is valid
or not when called from the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    Return Value Type: boolean 
        -- TRUE: USB device is valid 
        -- FALSE: USB device is invalid 

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern boolean pm_sbl_ext_usb_is_valid(void);


/*===========================================================================

FUNCTION pm_sbl_set_batt_alarm_threshold                    EXTERNAL FUNCTION

DESCRIPTION
    This function sets the battery alarm upper and lower voltage threshold 
when called from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: lwr_thres
        -- Battery alarm lower voltage threshold in mV
        -- Valid Inputs: 2800 to 4300
            
    2) Parameter name: upr_thres
        -- Battery alarm upper voltage threshold in mV
        -- Valid Inputs: 2800 to 4300

RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_set_batt_alarm_threshold(uint16 lwr_thresh, uint16 upr_thresh);


/*===========================================================================

FUNCTION pm_sbl_enable_batt_alarm                           EXTERNAL FUNCTION

DESCRIPTION
    This function enables or disables the battery alarm comparators 
when called from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: enable
        Valid Inputs:
            -- TRUE: enable the battery alarm comparators
            -- FALSE: disable the battery alarm comparators
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_enable_batt_alarm(boolean enable);


/*===========================================================================

FUNCTION pm_sbl_open_bat_fet                           EXTERNAL FUNCTION

DESCRIPTION
    This function opens the battery FET when called from the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_open_bat_fet(void);


/*===========================================================================

FUNCTION pm_sbl_enable_usb_chg                           EXTERNAL FUNCTION

DESCRIPTION
    This function enables/disables USB charging when called from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: enable
        Valid Inputs:
            -- TRUE: enable USB charging
            -- FALSE: disable USB charging 
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_enable_usb_chg(boolean enable);

/*===========================================================================

FUNCTION pm_sbl_trickle_chg                           EXTERNAL FUNCTION

DESCRIPTION
    This function sets the trickle charge current in mA when called from the 
bootloader. Setting 0mA means disabling trickle charging.
                                                                             
INPUT PARAMETERS
    1) Parameter name: i_trickle
        Valid Inputs: {0, 20, 30, 40, 50, 60, 70, 80} mA
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_trickle_chg(uint8 i_trickle);


/*===========================================================================

FUNCTION pm_sbl_keypad_led                           EXTERNAL FUNCTION

DESCRIPTION
    This function sets the keypad backlight LED driving current in mA when 
called from the bootloader. Setting 0mA means turning off the backlight LED 
                                                                             
INPUT PARAMETERS
    1) Parameter name: i_led
        Valid Inputs: 0 to 150mA in 10mA increment
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_keypad_led(uint8 i_led);


/*===========================================================================

FUNCTION pm_sbl_read_bat_thresh                           EXTERNAL FUNCTION

DESCRIPTION
    This function reads the battery voltage status compared against the
preset battery alarm upper and lower voltage threshold when called from 
the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    Return Value Type: int : [-1, 0, 1] 
        -- 1: battery voltage is above the battery alarm upper threshold
              i.e. VBAT > BAT_ALARM_UPPER_THRES
        -- 0: battery voltage is below the battery alarm upper threshold
              and above the battery alarm lower threshold
              i.e. BAT_ALARM_LOWER_THRES < VBAT < BAT_ALARM_UPPER_THRES
        -- -1: battery voltage is below the battery alarm lower threshold 
              i.e. VBAT < BAT_ALARM_LOWER_THRES

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern int pm_sbl_read_bat_thresh(void);


/*===========================================================================

FUNCTION pm_sbl_pull_dp_high                           EXTERNAL FUNCTION

DESCRIPTION
    This function pulls the D+ line high or low when called from the bootloader 
                                                                             
INPUT PARAMETERS
    1) Parameter name: high
        Valid Inputs:
            -- TRUE: pulls the D+ line high 
            -- FALSE: pulls the D+ line low 
            
RETURN VALUE
    NONE

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern void pm_sbl_pull_dp_high(boolean high);


/*===========================================================================

FUNCTION pm_sbl_is_dm_high                           EXTERNAL FUNCTION

DESCRIPTION
    This function determines whether the D- line is pulled high or low when
called from the bootloader 
                                                                             
INPUT PARAMETERS
    NONE
            
RETURN VALUE
    1) Parameter type: boolean
        -- TRUE: D- line is high 
        -- FALSE: D- line is low 

DEPENDENCIES
    pm_sbl_init
    NOTE: This function should be called from the bootloader only. 

SIDE EFFECTS
    NONE

===========================================================================*/
extern boolean pm_sbl_is_dm_high(void);


#endif /* PMSBL_H */

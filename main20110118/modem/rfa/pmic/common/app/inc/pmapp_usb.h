#ifndef PMAPP_USB_H
#define PMAPP_USB_H

/*===========================================================================

              P M   A P P - U S B   A P P   H E A D E R   F I L E

DESCRIPTION
  This file contains functions prototypes and variable/type/constant 
  declarations for HS-USB application.
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_usb.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/09   aab     - Added error checking on pm_app_register_for_vbus_sn_valid_status()
02/06/09   aab     - Added pm_vote_usb_pwr_sel_switch() API to control Vreg_5V/Vbus
                     switch from multiple application Used pm_vote_vreg_request_vote()
                      to get application voting id to control Boost Regulator
                   - Added pm_app_register_for_vbus_sn_valid_status():  It is a 
                     callback registeration function used to register for PMIC 
                     Vbus session valid and Invalid events.

10/21/08   aab     Added FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION
                   this feature is used to select USB_LDO power input(VREG_5V or USB VBus) based 
                   on USB Vbus session valid interrupt.  
                   
12/04/07   hs    Created.
===========================================================================*/


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
//Remote A-dev type
typedef enum
{
   PM_APP_OTG_A_DEV_TYPE__USB_HOST,       // Standard USB host like a PC
   PM_APP_OTG_A_DEV_TYPE__USB_CARKIT,     // USB carkit as specified in CEA-936-A
   PM_APP_OTG_A_DEV_TYPE__USB_CHARGER,    // USB charger as specified in CEA-936-A
   PM_APP_OTG_A_DEV_TYPE__INVALID
} pm_app_otg_a_dev_type;


//PMIC USB power select switch command type
typedef enum
{
   PM_SWITCH_OFF_CMD,
   PM_SWITCH_ON_CMD,
   PM_SWITCH_INVALID_CMD
}pm_usb_pwr_sel_switch_cmd_type;


//PMIC USB power select switch ID type
typedef enum 
{
   PM_USB_PWR_SEL_SWITCH_ID = 0,
   PM_USB_PWR_SEL_SWITCH_ID_INVALID
}pm_usb_pwr_sel_switch_id_type;

//PMIC USB power source select switch voting application type
typedef enum 
{
   PM_VOTE_USB_PWR_SEL_SWITCH_APP__OTG    = (1 << 0),
   PM_VOTE_USB_PWR_SEL_SWITCH_APP__AUDIO  = (1 << 1),
   PM_VOTE_USB_PWR_SEL_SWITCH_APP__HSUSB  = (1 << 2)
}pm_vote_usb_pwr_sel_switch_app_type;



//PMIC Vbus session valid RE/FE state type. 
// It is used as an argument in pm_app_usb_vbus_sn_valid_status_cb_type
typedef enum
{
   PM_USB_VBUS_SN_VALID,
   PM_USB_VBUS_SN_INVALID,
   PM_USB_FEATURE_NOT_DEFINED
} pm_usb_vbus_sn_valid_state_type;

//USB Vbus session valid callback type defination
typedef void (*pm_app_usb_vbus_sn_valid_status_cb_type) (pm_usb_vbus_sn_valid_state_type vbus_sn_valid_status);



/*===========================================================================
FUNCTION   pm_app_config_usb_ldo_input

DESCRIPTION
   This function is used for initializing detection of USB device connection/disconnection
   by monitoring Vbus session valid interrupt 

PARAMETERS
   None.

RETURN VALUE
   None
   
DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked in this function.
===========================================================================*/
void pm_app_config_usb_ldo_input(void);






/*===========================================================================
FUNCTION pm_app_register_for_vbus_sn_valid_status  

DESCRIPTION
  This function is a callback registeration function used to register for PMIC Vbus 
  session valid and Invalid events. The registered callback function shall be called 
  with an argument PM_USB_VBUS_SN_VALID/PM_USB_VBUS_SN_INVALID when Vbus session 
  valid RE/FE edge interrupt is detected.
  
                
PARAMETERS
  1) Parameter name: pm_vbus_sn_valid_status_cb.
     Parameter type: pm_app_usb_vbus_sn_valid_status_cb_type
     - The callback function would have one argument
        Argument type:  pm_usb_vbus_sn_valid_state_type
               PM_USB_VBUS_SN_VALID   : Vbus session valid RE event detected
               PM_USB_VBUS_SN_INVALID : Vbus session valid FE event detected
               PM_USB_FEATURE_NOT_DEFINED : The required feature 
                FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION is not defined

USAGE EXAMPLE:
1. Define callback function:
pm_app_usb_vbus_sn_valid_status_cb_type pm_usb_vbus_session_valid_state(
                          pm_usb_vbus_sn_valid_state_type  vbus_sn_valid_type);

2. Sample implementation of callback function
pm_app_usb_vbus_sn_valid_status_cb_type pm_usb_vbus_session_valid_state(
                              pm_usb_vbus_sn_valid_state_type  vbus_sn_valid_type)
{
   switch(vbus_sn_valid_type)
   {
   case PM_USB_VBUS_SN_VALID:
      //Add your code here
      break;

   case PM_USB_VBUS_SN_INVALID:
      //Add your code here
      break;

   case PM_USB_FEATURE_NOT_DEFINED:
      //Add your code here
      //for this function to work 
      //FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION 
      // must be defined
     break;
   case default:
      break;
   }
}

3. Registering the callback function to PMIC driver
pm_app_register_for_vbus_sn_valid_status(pm_usb_vbus_session_valid_state);
   
     
RETURN VALUE
  None.

DEPENDENCIES
  The following functions need to be called before we call this 
  function:
 
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
extern void pm_app_register_for_vbus_sn_valid_status(
            pm_app_usb_vbus_sn_valid_status_cb_type pm_vbus_sn_valid_status_cb);
/*~ FUNCTION pm_app_register_for_vbus_sn_valid_status */



/*===========================================================================
FUNCTION pm_vote_usb_pwr_sel_switch  

DESCRIPTION
      Use this function if One or more applications need to control USB_PWR_SEL_SWITCH.
  This function keeps track of which apps require the switch to be set to VREG_5V side.
  This function will only allow the switch to be set to VBUS side, if all the apps that 
  have previously enabled the switch through this function, call this function to disable it. 
  
  WARNING!!!!: 
      Do not use this function in conjunction with "pm_otg_pwr_sel()". 
                 
PARAMETERS
  1) Parameter name: cmd.
     - Turn the VREG ON or OFF

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
            PM_OFF_CMD = Disable the VREG.
            PM_ON_CMD  = Enable the VREG.

  2) Parameter name: switch_id. 
     - Which Switch needs to be turn ON/OFF.

     Parameter type: pm_usb_pwr_sel_switch_id_type (enum).
     - Valid Inputs:  PM_USB_PWR_SEL_SWITCH_ID
 
  3) Parameter name: app_mask. 
     - Which Application wants to enable/disable the switch.

     Parameter type: pm_vote_usb_pwr_sel_switch_app_type (enum).
     - Valid Inputs:
        PM_VOTE_USB_PWR_SEL_SWITCH_APP__OTG,
        PM_VOTE_USB_PWR_SEL_SWITCH_APP__AUDIO,
        PM_VOTE_USB_PWR_SEL_SWITCH_APP__HSUSB
     
RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  
SIDE EFFECTS
  None  
===========================================================================*/
void pm_vote_usb_pwr_sel_switch(pm_usb_pwr_sel_switch_cmd_type cmd, 
                                 pm_usb_pwr_sel_switch_id_type switch_id,
                                 pm_vote_usb_pwr_sel_switch_app_type app_mask );
/*~ FUNCTION pm_vote_usb_pwr_sel_switch */


#endif /* PMAPP_USB_H */

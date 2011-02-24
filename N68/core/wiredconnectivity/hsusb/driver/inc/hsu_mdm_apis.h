#ifndef _HSU_MDM_APIS_H_
#define _HSU_MDM_APIS_H_

/*==============================================================================

     High Speed USB Module Remote APIs on Modem processor 
      H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module defines High Speed USB APIs that are implemented on the modem 
    processor. 

  EXTERNALIZED FUNCTIONS
    hsu_api_init_phy
    hsu_api_vbus_powerup
    hsu_api_vbus_shutdown
    hsu_api_update_product_id
    hsu_api_update_serial_number
    hsu_api_update_is_serial_number_null
    hsu_api_init_chg
    hsu_api_chg_is_connected
    hsu_api_enable_pmic_otg_comparators
    hsu_api_disable_pmic_otg_comparators
    hsu_api_init_uicc
    hsu_api_start_uicc
    hsu_api_disconnect_pulldowns_uicc

  INITALIZATION AND SEQUENCING REQUIREMENTS
    TODO.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_mdm_apis.h#10 $
  $DateTime: 2009/02/27 13:52:40 $ $Author: kimd $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  02/08/07  ke  Initial version
  05/02/07  me  Add functions to update the product id and the serial number
    
==============================================================================*/
#include "customer.h"
#include "comdef.h" /* For boolean */
/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION      HSU_API_INIT_PHY

DESCRIPTION
  TODO.

DEPENDENCIES
  FEATURE_HS_USB must be defined. 
  TOOD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_api_init_phy(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_init_phy */
#endif

/*===========================================================================

FUNCTION      hsu_api_init_uicc

DESCRIPTION
    Initialize the USB UICC core and GPIO's.

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_init_uicc(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_init_uicc */
#endif

/*===========================================================================

FUNCTION      hsu_api_start_uicc

DESCRIPTION
    Start the USB UICC core and GPIOs.

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_start_uicc(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_start_uicc */
#endif

/*===========================================================================

FUNCTION      hsu_api_polldown_uicc

DESCRIPTION   
    Pull down the D+ and D- of the USB UICC GPIOs

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_polldown_uicc(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_polldown_uicc */
#endif


/*===========================================================================

FUNCTION      HSU_API_VBUS_POWERUP

DESCRIPTION
  Turn on the 5V power supply on the VBUS pin (when the HS-USB stack 
  enters host mode).


DEPENDENCIES
  FEATURE_HS_USB must be defined. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_api_vbus_powerup(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_vbus_powerup */
#endif


/*===========================================================================

FUNCTION      HSU_API_VBUS_SHUTDOWN

DESCRIPTION
  Turn off the 5V power supply on the VBUS pin (when the HS-USB stack 
  exits host mode).

DEPENDENCIES
  FEATURE_HS_USB must be defined. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_api_vbus_shutdown(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_vbus_shutdown */
#endif

/*===========================================================================

FUNCTION      HSU_API_UPDATE_PRODUCT_ID

DESCRIPTION
Updates the ARM9 product id variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_product_id(unsigned short product_id);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_update_product_id */
/*~  PARAM IN product_id UINT 16*/
#endif


/*===========================================================================

FUNCTION      HSU_API_UPDATE_SERIAL_NUMBER

DESCRIPTION
Updates the ARM9 serial number variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_serial_number(const char* serial_number);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_update_serial_number */
/*~  PARAM IN serial_number STRING*/
#endif

/*===========================================================================

FUNCTION      HSU_API_UPDATE_IS_SERIAL_NUMBER_NULL

DESCRIPTION
Updates the ARM9 whether the serial number is null.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_is_serial_number_null(boolean is_serial_number_null);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_update_is_serial_number_null */
/*~  PARAM IN is_serial_number_null UINT 8*/
#endif


/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_NOT_AVAIL

DESCRIPTION
Notifies the charger module that it can no longer draw current.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_not_avail(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_notif_charger_not_avail */
#endif

/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_IS_AVAIL

DESCRIPTION
Notifies the charger module that it can start drawing current.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_is_avail(uint32 amount_current_to_draw);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_notif_charger_is_avail */
/*~  PARAM IN amount_current_to_draw UINT32*/
#endif

/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_DISCONNECTED

DESCRIPTION
Notifies the charger module that it is disconnected from host PC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_disconnected(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_notif_charger_disconnected */
#endif

/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_CONNECTED

DESCRIPTION
Notifies the charger module that it is connected to host PC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_connected(boolean is_wall_charger);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_notif_charger_connected */
/*~  PARAM IN is_wall_charger UINT 8*/
#endif

/*===========================================================================

FUNCTION      HSU_API_INIT_CHG

DESCRIPTION
Initializes the hsu_chg module.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_init_chg(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_init_chg */
#endif

/*===========================================================================

FUNCTION      HSU_API_CHG_IS_CONNECTED

DESCRIPTION
Returns whether charger module is connected.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Charger connected. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_api_chg_is_connected(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_chg_is_connected */
#endif

/*==============================================================================

FUNCTION          HSU_API_USB_RESET_REWORK_INSTALLED

DESCRIPTION
Checks whether or not USB reset rework is installed.
The rework is for the purpose of properly exiting TCXO shutdown upon receiving 
USB Reset.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Rework is installed. FALSE - Otherwise.

SIDE EFFECTS
None. 

==============================================================================*/
boolean hsu_api_usb_reset_rework_installed(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_usb_reset_rework_installed */
#endif
/*===========================================================================

FUNCTION hsu_api_enable_pmic_ulpidata0_irq_handler 

DESCRIPTION
Configure PMIC MPPxx to interrupt when ULPI Data(0)[D+] changes since 
this line is not monitored by the MSM during TCXO shutdown (sleep).
This work-around is required to support USB Reset [D+ D- both LOW] 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Registers an ISR to be invoked upon change in input value to MPP15
===========================================================================*/
void hsu_api_enable_pmic_ulpidata0_irq_handler(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_enable_pmic_ulpidata0_irq_handler */
#endif

/*===========================================================================

FUNCTION hsu_api_disable_pmic_ulpidata0_irq_handler 

DESCRIPTION
Disable PMIC MPPxx interrupt.
This is called when USB is no longer in LPM, hence ULPI Data(0)[D+] 
does not need to be monitored by the PMIC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Unregisters the ISR from the corresponding enable function
===========================================================================*/
void hsu_api_disable_pmic_ulpidata0_irq_handler(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_disable_pmic_ulpidata0_irq_handler */
#endif

/*===========================================================================

FUNCTION hsu_dload_set_usb_imax

DESCRIPTION
This function set the power limit register to the maximum current when
called from the bootloader 

INPUT PARAMETERS
NONE

RETURN VALUE
NONE

DEPENDENCIES
NONE

SIDE EFFECTS
NONE

===========================================================================*/
void hsu_dload_set_usb_imax(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_dload_set_usb_imax */
#endif

/*===========================================================================

FUNCTION hsu_dload_enable_disable_core

DESCRIPTION
This function int the HSU PHY when called from the bootloader 

INPUT PARAMETERS
Parameter type: boolean
-- TRUE - enable the PHY
-- FALSE - Disable the PHY

RETURN VALUE
NONE

DEPENDENCIES
NONE

SIDE EFFECTS
NONE

===========================================================================*/
void hsu_dload_enable_disable_core(boolean enable);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_dload_enable_disable_core */
/*~ PARAM IN enable UINT 8*/
#endif

/*===========================================================================

FUNCTION hsu_dload_is_wall_charger

DESCRIPTION
This function determines whether a wall charger is connected or not 
when called from the bootloader.

INPUT PARAMETERS
NONE

RETURN VALUE
1) Parameter type: boolean
-- TRUE: Its a wall charger 
-- FALSE: Its not

DEPENDENCIES
NONE

SIDE EFFECTS
NONE

===========================================================================*/
boolean hsu_dload_is_wall_charger(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_dload_is_wall_charger */
#endif

/*===========================================================================

FUNCTION hsu_dload_sess_is_valid

DESCRIPTION
This function determines whether a HS USB session is valid or not when called
from the bootloader 

INPUT PARAMETERS
NONE

RETURN VALUE
Return Value Type: boolean 
-- TRUE: HS-USB session is valid 
-- FALSE: HS-USB session is invalid

DEPENDENCIES
pm_sbl_init

SIDE EFFECTS
NONE

===========================================================================*/
boolean hsu_dload_sess_is_valid(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_dload_sess_is_valid */
#endif

/*===========================================================================

FUNCTION      HSU_API_ENABLE_PMIC_OTG_COMPARATORS

DESCRIPTION
  Turn on the PMIC OTG comparators. This is for detecting USB cable 
  connection events during LPM. 

  The goal of this function is to replace the HS-USB PHY's comparators with 
  the PMIC OTG comparators in targets which use an integrated HS-USB PHY. 
  
  This is because the PMIC OTG comparators are more power saving than the PHY's
  comparators (the PHY's comparators consume ~80 uA, while the PMIC OTG 
  comparators consume ~1.6 uA).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE\FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_api_enable_pmic_otg_comparators(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_enable_pmic_otg_comparators */
#endif

/*===========================================================================

FUNCTION      HSU_API_DISABLE_PMIC_OTG_COMPARATORS

DESCRIPTION
  Turn off the PMIC OTG comparators. (See hsu_api_enable_pmic_otg_comparators()
  for more details on PMIC comparators)

  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE\FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_api_disable_pmic_otg_comparators(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_disable_pmic_otg_comparators */
#endif

/*===========================================================================

FUNCTION      hsu_api_disconnect_polldowns_uicc

DESCRIPTION   
    Disconnect the pull downs on the D+ and D- of the USB UICC GPIOs

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_disconnect_pulldowns_uicc(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_disconnect_pulldowns_uicc */
#endif

/*===========================================================================
FUNCTION HSU_API_TURN_OFF_VREG_USB_LDO

DESCRIPTION
  This function turns off the 3.3v power rail which goes from the PMIC
  to the HS-USB PHY.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hsu_api_turn_off_vreg_usb_ldo(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_turn_off_vreg_usb_ldo */
#endif


/*===========================================================================
FUNCTION HSU_API_TURN_ON_VREG_USB_LDO

DESCRIPTION
  This function turns on the 3.3v power rail which goes from the PMIC
  to the HS-USB PHY.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hsu_api_turn_on_vreg_usb_ldo(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_turn_on_vreg_usb_ldo */
#endif

#endif /* _HSU_MDM_APIS_H_ */

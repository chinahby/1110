#ifndef _HSU_CONFIG_SELECTOR_H_
#define _HSU_CONFIG_SELECTOR_H_

/*==============================================================================

High Speed USB configuration Selector Module        
H E A D E R   F I L E

GENERAL DESCRIPTION
This module is responsible for the initialization of the High Speed USB 
stack. 

EXTERNALIZED FUNCTIONS
hsu_config_selector_init
hsu_get_num_of_net_instances
hsu_conf_sel_get_rm_wakeup_delay
hsu_conf_sel_diag_is_on_legacy_port
hsu_conf_sel_uim_first_inst_class
hsu_conf_sel_use_pmic_otg_comparators
hsu_conf_sel_reconnect_on_reset_during_suspend
hsu_conf_sel_number_of_sdcc_luns
hsu_conf_sel_disable_sleep_voting

INITALIZATION AND SEQUENCING REQUIREMENTS
hsu_config_selector_init() should be called after DSM pools are initalized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_config_selector.h#9 $
$DateTime: 2009/03/24 07:20:20 $ $Author: tlinder $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
09/12/06  ke      Initial version

==============================================================================*/

#include "nv_items.h"

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

/*==============================================================================

FUNCTION          HSU_CONFIG_SELECTOR_INIT

DESCRIPTION
Called from the Application's processor TMC, after DSM pools are initialized,
and before the creation of tasks that use USB, such as DIAG, Modem, RmNET, 
etc...
The function initializes the High Speed USB stack according to the default
composition.

DEPENDENCIES
Should be called after DSM pools are initialized, and before the creation of 
tasks that use USB, such as DIAG, Modem, RmNET, etc...

RETURN VALUE
None.

SIDE EFFECTS
Various tasks related to the HS-USB stack are created.

==============================================================================*/
void hsu_config_selector_init(void);

/*===========================================================================

FUNCTION      HSU_GET_NUM_OF_NET_INSTANCES

DESCRIPTION
This function returns the number of the network (ECM) FD instances included in 
the current composition, for the primary core.

DEPENDENCIES
FEATURE_HS_USB must be defined. 
Must be called after hsu_config_selector_init() was called.

RETURN VALUE
The number of the network (ECM) FD instances included in the current composition. 

SIDE EFFECTS
None

===========================================================================*/
uint8 hsu_get_num_of_net_instances(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_GET_RM_WAKEUP_DELAY

DESCRIPTION
This function returns the delay (in milliseconds)used for performing an 
explicit remote wakeup.

DEPENDENCIES
None.

RETURN VALUE
The remote wakeup delay (in milliseconds) 

SIDE EFFECTS
None.

==============================================================================*/
uint32 hsu_conf_sel_get_rm_wakeup_delay(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_DIAG_IS_ON_LEGACY_PORT

DESCRIPTION
This function reads the NV item that determines whether DIAG should run over 
full-speed or high-speed USB and saves it in the variable 
hsu_diag_on_legacy_usb_port.
This function is relevant only if both FS-legacy & HS-OTG cores are supported 
in the image. Otherwise, DIAG is routed always from the primary core.

DEPENDENCIES
hsu_config_selector_init must be called before.

RETURN VALUE
TRUE if DIAG should run over full-speed USB.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_diag_is_on_legacy_port(void);

#ifdef FEATURE_HS_USB_UICC
/*==============================================================================

FUNCTION          HSU_CONF_SEL_UIM_FIRST_INST_CLASS

DESCRIPTION
This function returns the operation mode of UICC.

DEPENDENCIES
hsu_config_selector_init must be called before.

RETURN VALUE
UICC operation mode.

SIDE EFFECTS
None.

==============================================================================*/
nv_uim_first_inst_class_type hsu_conf_sel_uim_first_inst_class(void);
#endif /* FEATURE_HS_USB_UICC */

#if defined(T_QSC6270) || defined(T_MSM8650)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_USE_PMIC_OTG_COMPARATORS

DESCRIPTION
This function returns whether or not to use PMIC OTG comparators, based
on the value of the NV item hs_usb_use_pmic_otg_comparators

DEPENDENCIES
None.

RETURN VALUE
TRUE if PMIC comparators will be used during LPM, Otherwise, FALSE is
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_use_pmic_otg_comparators(void);
#endif /* T_QSC6270 || T_MSM8650 */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_RECONNECT_ON_RESET_DURING_SUSPEND

DESCRIPTION
This function returns whether or not to use the workaround for the 
USB bus reset on during TCXO shutdown issue, based on the value of the 
NV item hs_usb_reconnect_on_reset_during_suspend.

DEPENDENCIES
None.

RETURN VALUE
TRUE if the workaround is used. Otherwise, FALSE is returned.
returned.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void);

#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NUMBER_OF_SDCC_LUNS

DESCRIPTION
This function returns the number of SDCC/MMC LUNs that are enabled.

DEPENDENCIES
None.

RETURN VALUE
The number of SDCC/MMC LUNs that are enabled.

SIDE EFFECTS
None.

==============================================================================*/
uint8 hsu_conf_sel_number_of_sdcc_luns (void);
#endif /* FEATURE_HS_USB_MS_FD */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_DISABLE_SLEEP_VOTING

DESCRIPTION
This function returns whether or not to disable voting for sleep when entering LPM, 
based on the value of the NV item hs_usb_disable_sleep_voting.

DEPENDENCIES
None.

RETURN VALUE
If the value of the NV item is TRUE - TRUE.
If the value of the NV item is FALSE or if the NV item couldn't't be read - FALSE

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_disable_sleep_voting(void);
#endif /* _HSU_CONFIG_SELECTOR_H_ */

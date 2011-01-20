#ifndef _HSU_CONF_SEL_NV_H_
#define _HSU_CONF_SEL_NV_H_

/*==============================================================================

High Speed USB configuration Selector NV
H E A D E R   F I L E

GENERAL DESCRIPTION
This file includes functions that allow reading/writing item from the NV 
memory, to be used by the High Speed USB component.

EXTERNALIZED FUNCTIONS
hsu_conf_sel_nv_dev_comp_write
hsu_conf_sel_nv_dev_comp_read
hsu_conf_sel_nv_imei_read
hsu_conf_sel_nv_esn_read
hsu_conf_sel_nv_esn_imei_read
hsu_conf_sel_nv_rem_wakeup_delay_read
hsu_conf_sel_nv_diag_on_legacy_port_read
hsu_conf_sel_nv_uim_first_inst_class_read
hsu_conf_sel_nv_use_pmic_otg_comparators_read
hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read
hsu_conf_sel_nv_number_of_sdcc_luns_read
hsu_conf_sel_nv_disable_sleep_voting_read


INITALIZATION AND SEQUENCING REQUIREMENTS
Functions should only be called from the TMC's context.     

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_nv.h#7 $
$DateTime: 2009/03/24 07:20:20 $ $Author: tlinder $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
11/19/06 ke  Initial version
04/22/07 me Adding functions to read the ESN and IMEI

==============================================================================*/
#ifndef T_REXNT
#include "nv_items.h"
#include "nv.h"

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

FUNCTION          HSU_CONF_SEL_NV_DEV_COMP_WRITE

DESCRIPTION
Write to the NV the given USB device composition.
Called during the first stack bring-up after power-up, from the TMC Apps 
context.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE if writing failed.

SIDE EFFECTS
The calling function is blocked until NV write operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_dev_comp_write
(
 nv_hs_usb_supported_compositions_type nv_composition_id
 );

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_DEV_COMP_READ

DESCRIPTION
Read the USB device composition from NV.
Called during the first stack bring-up after power-up, from the TMC 
APPS context, and before going into download mode, from the DIAG's context, 
running on ARM9.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_dev_comp_read
(
 nv_hs_usb_supported_compositions_type * nv_composition_id
 ); 

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_IMEI_READ

DESCRIPTION
Read the IMEI from NV and convert it into ascii.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_imei_read
(
 char * return_buff_ptr ,                 /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 );

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_ESN_READ

DESCRIPTION
Read the ESN from NV and convert it into ascii.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_read
(
 char * return_buff_ptr ,                 /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 );

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_ESN_IMEI_READ

DESCRIPTION
Read the ESN or IMEI from NV.
The function tries to read the ESN first. If it fails it tries to read the IMEI. 

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_imei_read
(
 char * return_buff_ptr ,                 /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 );

/*==============================================================================
FUNCTION          HSU_CONF_SEL_NV_REM_WAKEUP_DELAY_READ

DESCRIPTION
Read the value of the remote-wakeup delay from NV items.

DEPENDENCIES
None.

RETURN VALUE
FALSE for failure.
TRUE for success.
The value of the remote wakeup delay from NV items. 

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_rem_wakeup_delay_read(uint32 *rem_wakeup_delay);

/*==============================================================================
FUNCTION          HSU_CONF_SEL_NV_DIAG_ON_LEGACY_PORT_READ

DESCRIPTION
Read the setting of whether DIAG should run over high-speed or full-speed usb.

DEPENDENCIES
None.

RETURN VALUE
FALSE for failure.
TRUE for success.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_diag_on_legacy_port_read(boolean *nv_value);

/*==============================================================================
FUNCTION          HSU_CONF_SEL_NV_UIM_FIRST_INST_CLASS_READ

DESCRIPTION
Read the setting of whether UICC support should be turned on.

DEPENDENCIES
None.

RETURN VALUE
FALSE for failure.
TRUE for success.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_uim_first_inst_class_read
(
 nv_uim_first_inst_class_type * nv_uim_first_inst_class_value
 );

#if defined(T_QSC6270) || defined(T_MSM8650)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_USE_PMIC_OTG_COMPARATORS_READ

DESCRIPTION
Reads the NV item use_pmic_otg_comparators which determines whether or not 
to use PMIC OTG comparators during LPM. The use of PMIC OTG comparators in
LCU will disable host mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_use_pmic_otg_comparators_read
(
 boolean * nv_use_pmic_otg_comparators
 ); 
#endif /* T_QSC6270 || T_MSM8650 */

/*==============================================================================

FUNCTION      HSU_CONF_SEL_NV_RECONNECT_ON_RESET_DURING_SUSPEND_READ

DESCRIPTION
Reads the following NV item: HS USB Reconnect On Reset during Suspend, 
which determines whether or not to use the workaround for the reset on 
suspend issue. 

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean 
hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read
(
 boolean* nv_reconnect_on_reset_during_suspend
);

#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_NUMBER_OF_SDCC_LUNS_READ

DESCRIPTION
Reads the NV item number_of_sdcc_luns which defines the number of MMC/SDCC LUNs 
that are available.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/

boolean hsu_conf_sel_nv_number_of_sdcc_luns_read 
(
 uint8 * nv_number_of_sdcc_luns
 );
#endif /* FEATURE_HS_USB_MS_FD */

#endif /* ! T_REXNT */


/*==============================================================================

FUNCTION      HSU_CONF_SEL_NV_DISABLE_SLEEP_VOTING_READ

DESCRIPTION
Reads the following NV item: HS USB disable sleep voting, 
which determines whether or not to vote for sleep when entering LPM

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_disable_sleep_voting_read
(
 boolean* nv_disable_sleep_voting
 ); 
#endif /* _HSU_CONF_SEL_NV_H_ */

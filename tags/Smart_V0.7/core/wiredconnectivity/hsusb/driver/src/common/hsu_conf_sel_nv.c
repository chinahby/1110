/*==============================================================================

High Speed USB Configuration Selector NV

GENERAL DESCRIPTION
This file includes function that allow reading/writing item from the NV 
memory, to be used by the High Speed USB component.

EXTERNALIZED FUNCTIONS
hsu_conf_sel_nv_dev_comp_write
hsu_conf_sel_nv_dev_comp_read
hsu_conf_sel_nv_imei_read
hsu_conf_sel_nv_esn_read
hsu_conf_sel_nv_esn_imei_read
hsu_conf_sel_nv_rem_wakeup_delay_read
hsu_conf_sel_nv_uim_first_inst_class_read
hsu_conf_sel_nv_use_pmic_otg_comparators_read
hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read
hsu_conf_sel_nv_number_of_sdcc_luns_read
hsu_conf_sel_nv_disable_sleep_voting_read

INITALIZATION AND SEQUENCING REQUIREMENTS
Functions should only be called from the TMC's context. 

Copyright (c) 2006, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_conf_sel_nv.c#9 $
$DateTime: 2009/03/24 07:20:20 $ $Author: tlinder $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
11/19/06  ke      Initial version
04/22/07  me      Adding functions to read the ESN and IMEI

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_conf_sel_nv.h"
#include "tmcnv.h"
#include "diagnv.h"
#include "task.h"
#include "hsu_common.h"
#include "err.h"
#include "hsu_conf_sel_types.h"
#include "AEEstd.h"
#ifdef FEATURE_UIM_RUIM
#include "nvruimi.h"
#endif

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_WRITE

DESCRIPTION
Write an item to the NV memory.

DEPENDENCIES
Context must be of TMC, otherwise the function ASSERTs.

RETURN VALUE
nv_stat_enum_type (see nv.h).

SIDE EFFECTS
The calling function is stopped until NV write operation is finished.

==============================================================================*/
static nv_stat_enum_type hsu_conf_sel_nv_write
(
 nv_items_enum_type  nv_item,
 nv_item_type *      data_ptr  
 ) 
{
  rex_tcb_type * self_tcb = rex_self();
  if (self_tcb == &tmc_tcb)
  {
    return tmcnv_write(nv_item, data_ptr);
  }
  else
  {
    HSU_ASSERT("*_read(): tcb is not of TMC." == NULL);
    return NV_FAIL_S;
  }
} /* hsu_conf_sel_nv_write */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_READ

DESCRIPTION
Read an item from the NV memory.

DEPENDENCIES
Context must be of TMC or DIAG, otherwise the function ASSERTs.

RETURN VALUE
nv_stat_enum_type (see nv.h).

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
static nv_stat_enum_type hsu_conf_sel_nv_read
(
 nv_items_enum_type  nv_item,
 nv_item_type *      data_ptr  
 )
{
  rex_tcb_type * self_tcb = rex_self();
  if (self_tcb == &tmc_tcb)
  {
    return tmcnv_read(nv_item, data_ptr);
  }
  else if(self_tcb == &diag_tcb)
  {
    return diag_nv_read(nv_item, data_ptr);
  }
  else
  {
    HSU_ASSERT("*_read(): tcb is not of TMC or DIAG, error." == NULL);
    return NV_FAIL_S;
  }
}/* hsu_conf_sel_nv_read */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_DEV_COMP_WRITE

DESCRIPTION
Write to the NV the given USB device composition.
Called during the first stack bring-up after power-up, from the TMC APPS 
context.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE if writing failed.

SIDE EFFECTS
The calling function is stopped until NV write operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_dev_comp_write
(
 nv_hs_usb_supported_compositions_type nv_composition_id
 ) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_item_data.hs_usb_current_composition = nv_composition_id;
  nv_operation_status = hsu_conf_sel_nv_write(NV_HS_USB_CURRENT_COMPOSITION_I, &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "*_dev_comp_write(): Couldn't write %d to NV_HS_USB_CURRENT_COMPOSITION_I (status = %d!)",
      nv_composition_id, nv_operation_status, 0);
    ERR(
      "*_dev_comp_write(): Couldn't write %d to NV_HS_USB_CURRENT_COMPOSITION_I (status = %d!)",
      nv_composition_id, nv_operation_status, 0);
    return FALSE;
  }
  return TRUE;
} /* hsu_conf_sel_nv_dev_comp_write */

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
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_dev_comp_read
(
 nv_hs_usb_supported_compositions_type * nv_composition_id
 ) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_HS_USB_CURRENT_COMPOSITION_I, &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "*_dev_comp_read(): Couldn't read NV_HS_USB_CURRENT_COMPOSITION_I from NV (status = %d)",
      nv_operation_status, 0, 0);
    ERR(
      "*_dev_comp_read(): Couldn't read NV_HS_USB_CURRENT_COMPOSITION_I from NV (status = %d)",
      nv_operation_status, 0, 0);
    return FALSE;
  }
  *nv_composition_id = nv_item_data.hs_usb_current_composition;
  return TRUE;
} /* hsu_conf_sel_nv_dev_comp_read */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_IMEI_READ

DESCRIPTION
Read the IMEI from NV and convert it into ascii.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_imei_read
(
 char * return_buff_ptr,                  /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 ) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;
  uint8 imei_bcd_len = 0;
  uint8 n = 0;
  uint8 digit;
  /* We need an extra digit for the id digit so we keep a size of (NV_UE_IMEI_SIZE-1)*2+1 */
  char imei_ascii[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE+1];

  return_buff_ptr[0] = '\0';

  nv_operation_status = hsu_conf_sel_nv_read(NV_UE_IMEI_I, &nv_item_data);
  /* Note that because IMEI is a special read only variable, it may return an all 0s   */
  /* value to indicate that it has never been written rather than the result code that */
  /* indicates the item is not active */
  if ((nv_operation_status != NV_DONE_S) || (nv_item_data.ue_imei.ue_imei[0] == 0))
  {
    /*    HSU_MSG_HIGH(
    "*_nv_imei_read(): Couldn't read NV_UE_IMEI_I from NV (status = %d)",
    nv_operation_status, 0, 0);
    ERR(
    "*_nv_imei_read(): Couldn't read NV_UE_IMEI_I from NV (status = %d)",
    nv_operation_status, 0, 0);*/
    return FALSE;
  }

  /* Convert it to ASCII */
  /* The first byte is the imei_bcd_len */
  imei_bcd_len = nv_item_data.ue_imei.ue_imei[0];

  /* Each byte contains two digits.
  The first byte contains an ID (that we should ignore later on 
  when we copy the imei_ascii) and then the first actual digit*/
  if( imei_bcd_len <= (NV_UE_IMEI_SIZE-1) )
  {
    /* This is a valid IMEI */
    memset(imei_ascii, 0, HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE+1);

    for( n = 1; n <= imei_bcd_len; n++ )
    {
      digit = nv_item_data.ue_imei.ue_imei[n] & 0x0F;
      if( ( digit <= 9 ) || ( n <= 1 ) )
      {
        imei_ascii[ (n - 1) * 2 ] = digit + '0';
      }
      else
      {
        imei_ascii[ (n - 1) * 2 ] = '\0';
        break;
      }

      digit = nv_item_data.ue_imei.ue_imei[n] >> 4;
      if( ( digit <= 9 ) || ( n <= 1 ) )
      {
        imei_ascii[ ((n - 1) * 2) + 1 ] = digit + '0';
      }
      else
      {
        imei_ascii[ ((n - 1) * 2) + 1 ] = '\0';
        break;
      }
    }
    imei_ascii[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE] = '\0';

    /* Lose the first byte because it is just the ID */
    std_strlcpy(return_buff_ptr, imei_ascii + 1, (int)return_buff_size);
  }

  return TRUE;
} /* hsu_conf_sel_nv_imei_read */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_ESN_READ

DESCRIPTION
Read the ESN from NV.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_read
(
 char * return_buff_ptr ,                 /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 ) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

#ifdef FEATURE_UIM_RUIM
  nvruim_esn_usage_type esn_usage_type;
#endif

#ifdef FEATURE_UIM_RUIM
  /* RUIM, make sure we can read the ESN from the NV */
  esn_usage_type = nvruim_get_esn_usage();
  nvruim_set_esn_usage(NV_RUIM_USE_ESN);
#endif

  nv_operation_status = hsu_conf_sel_nv_read(NV_ESN_I, &nv_item_data);

#ifdef FEATURE_UIM_RUIM
  /* Restore the previous value of nvruim_esn_usage_type*/
  nvruim_set_esn_usage(esn_usage_type);
#endif

  /* since the ESN is read only field it may return 0 to indicate that it is 
  not active the result code that indicates the item is not active    */                          
  if ((nv_operation_status != NV_DONE_S) || (nv_item_data.esn.esn == 0))
  {
    /*    HSU_MSG_HIGH(
    "*_nv_esn_read(): Couldn't read NV_ESN_I from NV (status = %d)",
    nv_operation_status, 0, 0);
    ERR(
    "*_nv_esn_read(): Couldn't read NV_ESN_I from NV (status = %d)",
    nv_operation_status, 0, 0);*/
    return FALSE;
  }

  /* Convert the 32 bit unsigned number to a string */
  (void)std_strlprintf(return_buff_ptr, (int)return_buff_size, "%x", nv_item_data.esn.esn);

  return TRUE;
} /* hsu_conf_sel_nv_esn_read */


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
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_imei_read
(
 char * return_buff_ptr,                  /* Pointer to return buffer */
 uint32 return_buff_size                  /* The size of the giver buffer */
 ) 
{
  boolean ret_val;

  ret_val = hsu_conf_sel_nv_esn_read(return_buff_ptr, return_buff_size);
  if (ret_val == FALSE)
  {
    /* If we didn't manage to read the ESN - try to read the IMEI */
    ret_val = hsu_conf_sel_nv_imei_read(return_buff_ptr, return_buff_size);
    if (ret_val == FALSE)
    {
      /* We didn't manage to read either the ESN or the IMEI */
      HSU_MSG_ERROR(
        "*_esn_imei_read(): Couldn't read NV_ESN_I and NV_UE_IMEI_I from NV",
        0, 0, 0);
      ERR(
        "*_esn_imei_read(): Couldn't read NV_ESN_I and NV_UE_IMEI_I from NV",
        0, 0, 0);
    }
  }

  return ret_val;
} /* hsu_conf_sel_nv_esn_imei_read */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_REM_WAKEUP_DELAY_READ

DESCRIPTION
Read the value of the remote-wakeup delay from NV items.
Called during the first stack bring-up after power-up and before going into 
download mode, from the DIAG's context, running on ARM9.

DEPENDENCIES
None.

RETURN VALUE
FALSE for failure.
TRUE for success.
The value of the remote wakeup delay from NV items. 

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_rem_wakeup_delay_read(uint32 *rem_wakeup_delay) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_HS_USB_REMOTE_WAKEUP_DELAY_I, &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "*_nv_rem_wakeup_delay_read(): Couldn't read NV_HS_USB_REMOTE_WAKEUP_DELAY from NV (status = %d)",
      nv_operation_status, 0, 0);
    ERR(
      "*_nv_rem_wakeup_delay_read(): Couldn't read NV_HS_USB_REMOTE_WAKEUP_DELAY from NV (status = %d)",
      nv_operation_status, 0, 0);
    return FALSE;
  }
  /* nv_operation_status = NV_DONE_S */

  *rem_wakeup_delay = nv_item_data.hs_usb_remote_wakeup_delay;
  return TRUE;

} /* hsu_conf_sel_nv_rem_wakeup_delay_read */

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
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_diag_on_legacy_port_read(boolean *nv_value)
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_HS_USB_DIAG_ON_LEGACY_USB_PORT_I, &nv_item_data);

  if(nv_operation_status == NV_NOTACTIVE_S)
  {
    /* This might happen after the flash is erased.
    ** In this case, we'll write the default value to NV.
    */
    nv_item_data.hs_usb_diag_on_legacy_usb_port = FALSE;
    nv_operation_status = hsu_conf_sel_nv_write(NV_HS_USB_DIAG_ON_LEGACY_USB_PORT_I, &nv_item_data);
    if(nv_operation_status != NV_DONE_S)
    {
      HSU_MSG_ERROR(
        "*_nv_diag_on_legacy_port_read(): Read attempt failed with NV_NOTACTIVE_S. Couldn't write default value (status = %d)",
        nv_operation_status, 0, 0);
      ERR(
        "*_nv_diag_on_legacy_port_read(): Read attempt failed with NV_NOTACTIVE_S. Couldn't write default value (status = %d)",
        nv_operation_status, 0, 0);
      return FALSE;
    }
  }
  else
  {
    if(nv_operation_status != NV_DONE_S)
    {
      HSU_MSG_ERROR(
        "*_nv_diag_on_legacy_port_read(): Couldn't read NV_HS_USB_DIAG_ON_LEGACY_USB_PORT from NV (status = %d)",
        nv_operation_status, 0, 0);
      ERR(
        "*_nv_diag_on_legacy_port_read(): Couldn't read NV_HS_USB_DIAG_ON_LEGACY_USB_PORT from NV (status = %d)",
        nv_operation_status, 0, 0);
      return FALSE;
    }
  }
  /* Either nv_operation_status = NV_NOTACTIVE_S and default value was written
  ** or nv_operation_status = NV_DONE_S or  
  */
  *nv_value = nv_item_data.hs_usb_diag_on_legacy_usb_port;
  return TRUE;
}
/* hsu_conf_sel_nv_diag_on_legacy_port_read */

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
 )
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_UIM_FIRST_INST_CLASS_I, 
    &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "*_uim_first_inst_class_read): Couldn't read NV_UIM_FIRST_INST_CLASS_I from NV (status = %d)",
      nv_operation_status, 0, 0);
    return FALSE;
  }
  *nv_uim_first_inst_class_value = nv_item_data.uim_first_inst_class;
  return TRUE;
}
/* hsu_conf_sel_nv_uim_first_inst_class_read */

#if defined(T_QSC6270) || defined(T_MSM8650)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_USE_PMIC_OTG_COMPARATORS_READ

DESCRIPTION
Reads the NV item use_pmic_otg_comparators which determines whether or not 
to use PMIC OTG comparators during LPM. The use of PMIC OTG comparators in
LCU and Raptor2 will disable host mode.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_use_pmic_otg_comparators_read (boolean * nv_use_pmic_otg_comparators) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_HS_USB_USE_PMIC_OTG_COMPARATORS_I, &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "hsu_conf_sel_nv_use_pmic_otg_comparators_read(): "
      "Couldn't read NV_HS_USB_USE_PMIC_OTG_COMPARATORS_I from NV (status = %d)",
      nv_operation_status, 0, 0);

    return FALSE;
  }
  *nv_use_pmic_otg_comparators = nv_item_data.hs_usb_use_pmic_otg_comparators;
  return TRUE;
} /* hsu_conf_sel_nv_use_pmic_otg_comparators_read */
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
) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = 
    hsu_conf_sel_nv_read(NV_HS_USB_RECONNECT_ON_RESET_DURING_SUSPEND_I,
      &nv_item_data);

  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read(): "
      "Couldn't read NV_HS_USB_RECONNECT_ON_RESET_DURING_SUSPEND_I from NV "
      "(status = %d)",
      nv_operation_status, 0, 0);

    return FALSE;
  }

  *nv_reconnect_on_reset_during_suspend = 
    nv_item_data.hs_usb_reconnect_on_reset_during_suspend;

  return TRUE;
} /* hsu_conf_sel_nv_reconnect_on_reset_during_suspend_read */


#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_NUMBER_OF_SDCC_LUNS_READ

DESCRIPTION
Reads the NV item number_of_sdcc_luns which sets the number of MMC/SDCC LUNs 
that are available.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/

boolean hsu_conf_sel_nv_number_of_sdcc_luns_read (uint8 * nv_number_of_sdcc_luns)
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_HS_USB_NUMBER_OF_SDCC_LUNS_I, &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "hsu_conf_sel_nv_number_of_sdcc_luns_read(): Couldn't read NV_HS_USB_NUMBER_OF_SDCC_LUNS_I from NV (status = %d)",
      nv_operation_status, 0, 0);
    return FALSE;
  }
  *nv_number_of_sdcc_luns = nv_item_data.hs_usb_number_of_sdcc_luns;
  return TRUE;
} /* hsu_conf_sel_nv_number_of_sdcc_luns_read */
#endif /* FEATURE_HS_USB_MS_FD */

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
boolean 
hsu_conf_sel_nv_disable_sleep_voting_read
(
 boolean* nv_disable_sleep_voting
 ) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = 
    hsu_conf_sel_nv_read(NV_HS_USB_DISABLE_SLEEP_VOTING_I,
    &nv_item_data);

  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR(
      "hsu_conf_sel_nv_disable_sleep_voting_read(): "
      "Couldn't read NV_HS_USB_DISABLE_SLEEP_VOTING_I from NV "
      "(status = %d)",
      nv_operation_status, 0, 0);

    return FALSE;
  }

  *nv_disable_sleep_voting = 
    nv_item_data.hs_usb_disable_sleep_voting;

  return TRUE;
} /* hsu_conf_sel_nv_disable_sleep_voting_read */


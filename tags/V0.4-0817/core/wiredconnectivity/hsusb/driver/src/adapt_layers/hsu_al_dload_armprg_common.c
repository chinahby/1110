/*==============================================================================

HSU_AL_DLOAD_ARMPRG_COMMON

GENERAL DESCRIPTION
Common functions for the High Speed USB DLOAD and ARMPRG Adaptation Layers

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#include "comdef.h"
#include "msmhwio.h"
#include "hsu_al_dload_armprg_common.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_common.h"
#include "port.h"
#include "jerrno.h"
#include BOOTHW_MSM_H
#include "msm.h"
#include "boot_dload.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_boot_dload.h"
#include "hsu_conf_sel_core_config.h"

#ifdef FEATURE_HSU_EBI
#error code not present
#else /* !FEATURE_HSU_EBI */
byte                        hsu_al_dload_armprg_rx_buffer[2][HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE];
byte                        hsu_al_dload_armprg_tx_buffer[HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE];
#endif /* FEATURE_HSU_EBI */

/*===========================================================================

FUNCTION hsu_al_dload_armprg_init_buffers

DESCRIPTION
This function initializes the rx and tx buffers 
to the appropriate memory addresses.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_dload_armprg_init_buffers(void)
{
#ifdef FEATURE_HSU_EBI
#error code not present
#endif /* FEATURE_HSU_EBI */
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_handle_interrupts

DESCRIPTION
This function uses os abstraction functions in order to
poll for interrupts, and call the isr and dsr if an interrupt
occurred.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_dload_armprg_handle_interrupts(void)
{
  if (hsu_os_bus_poll_interrupt())
  {
    BOOTHW_KICK_WATCHDOG();
    hsu_os_bus_handle_interrupt();
  }
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_start_stack

DESCRIPTION
This function initializes the memory for the stack and starts the stack.

DEPENDENCIES
None.

RETURN VALUE
0 on success, error number otherwise.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_dload_armprg_start_stack(void)
{
  return hsu_al_dload_armprg_start_stack_ex(TRUE);
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_start_stack_ex

DESCRIPTION
This function initializes the memory
for the stack and starts the stack.
It receives a parameter that specifies whether to enumerate
using the same product ID used when running in AMSS (non-download mode).

DEPENDENCIES
None.

RETURN VALUE
0 on success, error number otherwise.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_dload_armprg_start_stack_ex(boolean use_amss_product_id)
{
  uint16  hsu_product_id;
  const hsu_conf_sel_comp_info_type* hsu_default_composition_info_ptr;
  hsu_dload_info_type* hsu_al_dload_armprg_info_data_ptr = NULL;
  struct hsu_device_params_type* boot_core_conf_sel_info = NULL;

  hsu_conf_sel_core_config_set_core_info();
  hsu_conf_sel_stack_utils_init_memory();

  hsu_al_dload_armprg_init_buffers();

  if (!use_amss_product_id)
  {
    hsu_product_id = HSU_PRODUCT_ID_NON_AMSS;
  }
  else
  {
    /* Check if we have a version number in the IRAM data or not.
    ** The version number can be invalid if we have a new version on the ARMPRG
    ** but the AMSS ran an old version so it didn't update the version_num in 
    ** transition to DLOAD.
    ** If the version number is not valid - it means that we have an old IRAM data
    ** so we need to update it with the new variables
    */
    hsu_boot_dload_check_and_update_version_num();

    hsu_al_dload_armprg_info_data_ptr =  hsu_boot_dload_get_hsu_dload_info_ptr();
    HSU_ASSERT(hsu_al_dload_armprg_info_data_ptr != NULL);

    if (!hsu_boot_dload_is_dload_info_valid(hsu_al_dload_armprg_info_data_ptr))
    {
      hsu_default_composition_info_ptr = 
        hsu_conf_sel_comp_utils_get_default_comp_info(); 

      if (hsu_default_composition_info_ptr != NULL)
      {
        hsu_product_id = hsu_default_composition_info_ptr->product_id;
      }
      else
      {
        /* Can't start the stack without a valid product ID */
        return EUNKNOWN;
      }
    }
    else
    {
      hsu_product_id = hsu_al_dload_armprg_info_data_ptr->hsu_product_id;
    }
  }

  boot_core_conf_sel_info = hsu_conf_sel_core_config_get_boot_core_conf_sel_info();
  if (boot_core_conf_sel_info == NULL)
  {
    return JEINVAL;
  }

  memset(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances, 
          0, 
          sizeof(boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances));
  boot_core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].num_of_fd_instances[HSU_CFG_SEL_OBEX_FD] = 1;
  boot_core_conf_sel_info->product_id = hsu_product_id;

  return hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
    use_amss_product_id
#endif
);
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_mark_dload_used_hsu

DESCRIPTION
This function sets a marker in IRAM that signifies that DLOAD worked
over high speed usb.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_dload_armprg_common_mark_dload_used_hsu(void)
{
  hsu_dload_info_type* hsu_al_dload_armprg_info_data_ptr = 
    hsu_boot_dload_get_hsu_dload_info_ptr();

  if (hsu_al_dload_armprg_info_data_ptr == NULL)
  {
    /* HSU_MSG_ERROR is needed since HSU_ASSERT does nothing in DLOAD/ARMPRG */
    HSU_MSG_ERROR("hsu_al_dload_armprg_info_data_ptr is NULL", 0, 0, 0);
    HSU_ASSERT(hsu_al_dload_armprg_info_data_ptr != NULL);
    return;
  }
  hsu_al_dload_armprg_info_data_ptr->dload_used_hsu = TRUE;
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_unmark_dload_used_hsu

DESCRIPTION
This function clears the marker in IRAM that signifies that DLOAD worked
over high speed usb (sets it to FALSE).

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_dload_armprg_common_unmark_dload_used_hsu(void)
{
  hsu_dload_info_type* hsu_al_dload_armprg_info_data_ptr = 
    hsu_boot_dload_get_hsu_dload_info_ptr();

  if (hsu_al_dload_armprg_info_data_ptr == NULL)
  {
    /* HSU_MSG_ERROR is needed since HSU_ASSERT does nothing in DLOAD/ARMPRG */
    HSU_MSG_ERROR("hsu_al_dload_armprg_info_data_ptr is NULL", 0, 0, 0);
    HSU_ASSERT(hsu_al_dload_armprg_info_data_ptr != NULL);
    return;
  }

  hsu_al_dload_armprg_info_data_ptr->dload_used_hsu = FALSE;
}

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_dload_used_hsu

DESCRIPTION
This function checks whether DLOAD worked over high speed usb.

DEPENDENCIES

RETURN VALUE
TRUE if DLOAD worked over high speed usb, FALSE otherwise.

SIDE EFFECTS

===========================================================================*/

boolean hsu_al_dload_armprg_common_dload_used_hsu(void)
{
  hsu_dload_info_type* hsu_al_dload_armprg_info_data_ptr = 
    hsu_boot_dload_get_hsu_dload_info_ptr();

  if (hsu_al_dload_armprg_info_data_ptr == NULL)
  {
    /* HSU_MSG_ERROR is needed since HSU_ASSERT does nothing in DLOAD/ARMPRG */
    HSU_MSG_ERROR("hsu_al_dload_armprg_info_data_ptr is NULL", 0, 0, 0);
    HSU_ASSERT(hsu_al_dload_armprg_info_data_ptr != NULL);
    return FALSE;
  }

  /* An fix to enable SW download from the first FC versions of Koel
     and Hawk to the completed solution versions.
     Since in the first versions the SW download info was not saved in shared 
     segment and IMEM but was determined hard-coded, when downloading a new
     version over an old one, the IMEM memory will not be valid.*/
  if (!hsu_boot_dload_is_dload_info_valid(hsu_al_dload_armprg_info_data_ptr))
  {
    return TRUE;
  }

  return hsu_al_dload_armprg_info_data_ptr->dload_used_hsu;
}

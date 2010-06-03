/*==============================================================================

                            HS-USB Core configuraion

GENERAL DESCRIPTION
    Contains the core configuration for HS-USB that is common for all targets.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
$DateTime: 

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/17/08 me  Initial version

==============================================================================*/

/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "hsu_core_config.h"

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
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
Variables
==============================================================================*/
extern hsu_core_config_type* hsu_core_config_table[];


hsu_core_config_type cores_details[HSU_NUM_OF_CORES_TYPES] = 
{
  { /* High-Speed USB core. */
#ifdef FEATURE_HS_USB_OTG
#error code not present
#else
    HSU_CORE_CONFIG_INIT_MODE_DEVICE,
#endif /* FEATURE_HS_USB_OTG */
    NULL,
    NULL
  },
  { /* UICC Core data */
    HSU_CORE_CONFIG_INIT_MODE_HOST,
    NULL,
    NULL
  },
  { /* boot Core data */
    HSU_CORE_CONFIG_INIT_MODE_DEVICE,
    NULL,
    NULL
  },
  { /* Legacy host Core data */
    HSU_CORE_CONFIG_INIT_MODE_HOST,
    NULL,
    NULL
  },
  { /* Legacy device Core data */
    HSU_CORE_CONFIG_INIT_MODE_DEVICE,
    NULL,
    NULL
  }
};


/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
#ifndef T_REXNT
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_CORE

DESCRIPTION
This function returns the core according to the core index

DEPENDENCIES
None.

RETURN VALUE
A pointer to the core in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_core(uint8 core_idx)
{
  if (core_idx >= hsu_core_config_get_cores_table_size())
  {
    return NULL;
  }
  return hsu_core_config_table[core_idx];
}

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_NUM_OF_CORES

DESCRIPTION
This .
The table will be function returns the number of active cores in the cores table 
(NOT including the NULL cores).

DEPENDENCIES
None.

RETURN VALUE
The number of active cores in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_num_of_cores(void)
{
  uint8 num_of_cores = 0;
  uint8 core_idx;
  uint8 cores_table_size = hsu_core_config_get_cores_table_size();

  for (core_idx = 0; core_idx < cores_table_size ; ++core_idx)
  {
    if (hsu_core_config_table[core_idx] != NULL)
    ++num_of_cores;
  }
  return num_of_cores;
}

#endif /* T_REXNT */

#if !defined(FEATURE_HS_USB_BASIC) && !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_HS_MAIN_CORE

DESCRIPTION
This function returns the HS-USB main core

DEPENDENCIES
None.

RETURN VALUE
The address of HS-USB main core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_hs_main_core(void)
{
  return &cores_details[HSU_HS_MAIN_CORE];
}
#endif /* !FEATURE_HS_USB_BASIC && !FEATURE_HS_USB_NO_HS_OTG_CORE */

#if !defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_UICC)
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_UICC_CORE

DESCRIPTION
This function returns the UICC core

DEPENDENCIES
None.

RETURN VALUE
The address of the UICC core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_uicc_core(void)
{
  return &cores_details[HSU_UICC_CORE];
}
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_UICC */

#ifdef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_BOOT_CORE

DESCRIPTION
This function returns the boot core (used in DLOAD/ARMPRG/fast enumeration/dead battery charging

DEPENDENCIES
None.

RETURN VALUE
The address of the boot core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_conf_sel_core_config_get_boot_core(void)
{
  return &cores_details[HSU_BOOT_CORE];
}
#endif /* FEATURE_HS_USB_BASIC */

#if !defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_LEGACY_DEVICE)
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_FS_LEGACY_CORE

DESCRIPTION
This function returns the UICC core

DEPENDENCIES
None.

RETURN VALUE
The address of the UICC core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_fs_legacy_core(void)
{
  return &cores_details[HSU_LEGACY_DEVICE_CORE];
}
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_LEGACY_DEVICE */

#ifndef T_REXNT
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_STACK_INIT_MODE

DESCRIPTION
Returns the mode in which the stack was configured at power up (device/OTG) for each core.

DEPENDENCIES
Should be called after hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
HSU_CORE_CONFIG_INIT_MODE_OTG / HSU_CORE_CONFIG_INIT_MODE_DEVICE / HSU_CORE_CONFIG_INIT_MODE_HOST
HSU_CORE_CONFIG_INIT_MODE_LAST for error.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_init_mode_enum hsu_core_config_get_stack_init_mode(uint8 core_idx)
{
  if (hsu_core_config_get_core(core_idx) == NULL)
  {
    HSU_MSG_ERROR("*_get_stack_init_mode((): hsu_core_config_table entry for core index %u is NULL",
      core_idx,0,0);
    return HSU_CORE_CONFIG_INIT_MODE_LAST;
  }
  return hsu_core_config_get_core(core_idx)->init_mode;
}
#endif /* !T_REXNT */

#if !defined (FEATURE_HS_USB_BASIC) && !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_SET_HS_MAIN_CORE_CONF_SEL_INFO

DESCRIPTION
Set the pointer to the config selector info for HS-USB main core.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_set_hs_main_core_conf_sel_info(void* conf_sel_info)
{
  cores_details[HSU_HS_MAIN_CORE].hsu_conf_sel_core_info = conf_sel_info;
}

#elif FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_SET_BOOT_CORE_CONF_SEL_INFO

DESCRIPTION
Set the pointer to the config selector info for boot core.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_set_boot_core_conf_sel_info(void* conf_sel_info)
{
  cores_details[HSU_BOOT_CORE].hsu_conf_sel_core_info = conf_sel_info;
}

#endif /* !FEATURE_HS_USB_BASIC && !FEATURE_HS_USB_NO_HS_OTG_CORE */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_SET_FS_LEGACY_CORE_CONF_SEL_INFO

DESCRIPTION
Set the pointer to the config selector info for FS legacy core.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_set_fs_legacy_core_conf_sel_info(void* conf_sel_info)
{
  cores_details[HSU_LEGACY_DEVICE_CORE].hsu_conf_sel_core_info = conf_sel_info;
}

#endif /* FEATURE_HS_USB_LEGACY_DEVICE */


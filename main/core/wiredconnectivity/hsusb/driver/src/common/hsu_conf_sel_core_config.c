/*==============================================================================

          Config selector data for HS-USB Core configuraion

GENERAL DESCRIPTION
    Contains the config selector data for HS-USB core configuration.

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
#include "hsu_conf_sel_core_config.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_al_ser.h"
#ifdef FEATURE_HS_USB_ECM
#include "hsu_al_ecm.h"
#endif /* FEATURE_HS_USB_ECM */


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

/* High-Speed USB core. */
#ifndef FEATURE_HS_USB_BASIC
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
static hsu_config_params_type hsu_hs_main_config_params[] = 
{ 
  /* Configuration 0 data */
  {
    {0,0,0,0,0,0,0,0,0},              /* num_of_fd_instances - to be set later according to the chosen composition */
    "HS-USB configuration 0",         /* config_name */
    NULL                              /* cur_dev_comp_details - to be set later according to the chosen composition */
  }
};

static hsu_device_params_type hsu_hs_main_device_params =
{
#ifdef FEATURE_HS_USB_FS_ONLY
  HSU_CONF_SEL_FULL_SPEED_CORE,
#else
  HSU_CONF_SEL_HIGH_SPEED_CORE,
#endif
  0,                                  /* product_id - to be set later according to the chosen composition */
  HSU_MAX_NUM_OF_CONFIGS,             /* num_of_configs */
  &hsu_hs_main_config_params[0]
};
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
#else /* FEATURE_HS_USB_BASIC */

static hsu_config_params_type hsu_boot_config_params[] = 
{ 
  /* Configuration 0 data */
  {
    {0,0,0,0,0,0,0,0,0},              /* num_of_fd_instances - to be set later according to the case
                                         (DLOAD/ARMPRG/fast enum/dead battery charging) */
    "HS-USB configuration 0",         /* config_name */
    NULL                              /* cur_dev_comp_details */
  }
};

static hsu_device_params_type hsu_boot_device_params =
{
#ifdef FEATURE_HS_USB_FS_ONLY
  HSU_CONF_SEL_FULL_SPEED_CORE,
#else
  HSU_CONF_SEL_HIGH_SPEED_CORE,
#endif
  0,                                /* product_id - to be set later according to the case
                                       (DLOAD/ARMPRG/fast enum/dead battery charging) */
  HSU_MAX_NUM_OF_CONFIGS,           /* num_of_configs */
  &hsu_boot_config_params[0]
};

#endif /* #ifndef FEATURE_HS_USB_BASIC */

/* Full-Speed Legacy USB core. */
#if defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC)
static hsu_config_params_type hsu_fs_legacy_config_params[] = 
{ 
  /* Configuration 0 data */
  {
      {0,1,0,0,0,0,0,0,0},              /* num_of_fd_instances - to be set later according to the chosen composition */
      "HS-USB configuration 0",         /* config_name */
      NULL                              /* cur_dev_comp_details - no composition for this core */
  }
};

static hsu_device_params_type hsu_fs_legacy_device_params =
{
  HSU_CONF_SEL_FULL_SPEED_CORE,
  HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM, /* product_id - to be set later according to the chosen composition */
  HSU_MAX_NUM_OF_CONFIGS,              /* num_of_configs */
  &hsu_fs_legacy_config_params[0]
};
#endif /* FEATURE_HS_USB_LEGACY_DEVICE && !FEATURE_HS_USB_BASIC */
/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/
/*===========================================================================
FUNCTION ()
===========================================================================*/
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_SET_CORE_INFO

DESCRIPTION
Set the pointers to the config selector info for all cores.

DEPENDENCIES
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_core_config_set_core_info(void)
{
#if !defined (FEATURE_HS_USB_BASIC) && !defined (FEATURE_HS_USB_NO_HS_OTG_CORE)
  hsu_core_config_set_hs_main_core_conf_sel_info((void*)&hsu_hs_main_device_params);
#elif defined (FEATURE_HS_USB_BASIC)
  hsu_core_config_set_boot_core_conf_sel_info((void*)&hsu_boot_device_params);
#endif

#if !defined (FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_LEGACY_DEVICE)
  hsu_core_config_set_fs_legacy_core_conf_sel_info((void*)&hsu_fs_legacy_device_params);
#endif
}

#if !defined(FEATURE_HS_USB_BASIC) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_HS_MAIN_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of the HS-USB main core

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of HS-USB main core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info(void)
{
  return &hsu_hs_main_device_params;
}

#elif defined(FEATURE_HS_USB_BASIC)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_BOOT_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of the boot core 
(used in DLOAD/ARMPRG/fast enumeration/dead battery charging

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of the boot core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_boot_core_conf_sel_info(void)
{
  return &hsu_boot_device_params;
}
#endif /* !FEATURE_HS_USB_BASIC && !FEATURE_HS_USB_NO_HS_OTG_CORE */

#if !defined (FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_LEGACY_DEVICE)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_CORE_CONFIG_GET_FS_LEGACY_CORE_CONF_SEL_INFO

DESCRIPTION
This function returns the config selector info of the FS legacy core.

DEPENDENCIES
None.

RETURN VALUE
The address of the config selector info of the FS legacy core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_device_params_type* hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info(void)
{
  return &hsu_fs_legacy_device_params;
}
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_LEGACY_DEVICE */

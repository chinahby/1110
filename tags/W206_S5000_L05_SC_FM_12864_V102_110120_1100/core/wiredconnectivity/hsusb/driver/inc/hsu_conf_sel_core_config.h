#ifndef _HSU_CONF_SEL_CORE_CONFIG_H_
#define _HSU_CONF_SEL_CORE_CONFIG_H_

/*==============================================================================

   Config selector data for HS-USB Core configuraion
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Config selector data for HS-USB Core configuraion 

  EXTERNALIZED FUNCTIONS
    None.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
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
  07/17/08  me      Initial version

==============================================================================*/
#include "hsu_core_config.h"
#include "hsu_conf_sel_types.h"


/*==============================================================================
Forward Declarations
==============================================================================*/
/*==============================================================================
Constants
==============================================================================*/

/* HSU_NUM_OF_DEVICE_CONTROLLERS defines the number of peripheral cores that we have.
   This constant is used for local arrays allocation to be supplied to Jungo on stack startup.
   HSU_MAX_NUM_OF_CONFIGS defines the maximum number of configuration that is defined in 
   all the cores. For example, if one core has 2 configurations and the second only one 
   configuration, it will be set to 2. This constant is used for local arrays allocation to be 
   supplied to Jungo on stack startup. */ 
#ifndef T_REXNT
#define HSU_MAX_NUM_OF_CONFIGS 1
#if (defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC))
#define HSU_NUM_OF_DEVICE_CONTROLLERS 2
#elif (defined(FEATURE_HS_USB_LEGACY_HOST) && !defined(FEATURE_HS_USB_BASIC))
#define HSU_NUM_OF_DEVICE_CONTROLLERS 1
#elif (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))
#define HSU_NUM_OF_DEVICE_CONTROLLERS 1
#else
#define HSU_NUM_OF_DEVICE_CONTROLLERS 1
#endif
#else /* T_REXNT */
#error code not present
#endif /* T_REXNT */

typedef enum
{
  HSU_CONFIG_IDX_0 = 0,
  HSU_CONFIG_IDX_1
}hsu_conf_sel_configs_enum;

/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/
typedef enum
{
  HSU_CONF_SEL_HIGH_SPEED_CORE,
  HSU_CONF_SEL_FULL_SPEED_CORE
} hsu_conf_sel_core_type;

typedef struct hsu_config_params_type
{
  uint8 num_of_fd_instances[HSU_CFG_SEL_NUM_OF_FD_TYPES];
  char* config_name;
  const hsu_conf_sel_comp_info_type* cur_dev_comp_details;
}hsu_config_params_type;

typedef struct hsu_device_params_type
{
  hsu_conf_sel_core_type core_type;
  uint16 product_id;
  uint8 num_of_configs;
  hsu_config_params_type* configs_arr;
}hsu_device_params_type;


/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
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
void hsu_conf_sel_core_config_set_core_info(void);

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
hsu_device_params_type* hsu_conf_sel_core_config_get_hs_main_core_conf_sel_info(void);

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
hsu_device_params_type* hsu_conf_sel_core_config_get_boot_core_conf_sel_info(void);
#endif /* !FEATURE_HS_USB_BASIC && !FEATURE_HS_USB_NO_HS_OTG_CORE */

#ifdef FEATURE_HS_USB_LEGACY_DEVICE
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
hsu_device_params_type* hsu_conf_sel_core_config_get_fs_legacy_core_conf_sel_info(void);
#endif /* FEATURE_HS_USB_LEGACY_DEVICE */

#endif /* _HSU_CONF_SEL_CORE_CONFIG_H_ */


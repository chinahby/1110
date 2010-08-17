#ifndef _HSU_CORE_CONFIG_H_
#define _HSU_CORE_CONFIG_H_

/*==============================================================================

     High Speed USB core configuration Module
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    core definitions used by the various HS-USB files. 

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
#include "hsu_common.h"
#include "comdef.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

/* Primary core is the USB core that is linked to general features such as 
   device composition. It can be for example:
   - HS-OTG core.
   - FS-Legacy on FS legacy only image (MODEM/APPS).
   Seconday core is FS legacy core on an image supporting both HS-OTG and 
   FS-Legacy cores (MODEM/APPS). General features such as device composition 
   are not supported for seconday core. */
typedef enum
{
#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
  /* HS-OTG core */
  HSU_CORE_IDX_PRIMARY = 0,
#elif defined(FEATURE_HS_USB_LEGACY_HOST) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
  /* FS-Legacy core (7k HAL) */
  HSU_CORE_IDX_PRIMARY = 1,
#endif /* FEATURE_HS_USB_LEGACY_HOST || FEATURE_HS_USB_LEGACY_HOST */

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  /* HS-OTG core */
  HSU_CORE_IDX_SECONDARY = 1
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
} hsu_cores_enum;

typedef enum
{
  HSU_HS_MAIN_CORE,
  HSU_UICC_CORE,
  HSU_BOOT_CORE,
  HSU_LEGACY_HOST_CORE,
  HSU_LEGACY_DEVICE_CORE,
  HSU_NUM_OF_CORES_TYPES
}hsu_core_type_enum;

/* HSU_MAX_NUM_OF_CORES defines the number of all cores (host and peripheral) in all 
the possible  configurations.
This constant is used for local arrays allocation to be supplied to Jungo on stack startup.*/ 
#ifndef T_REXNT
#define HSU_MAX_NUM_OF_CORES 2
#else
#error code not present
#endif
/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/

/*
* Enum for the various initialization options for the USB Stack (currently only 
* device / OTG modes are supported). 
*/
typedef enum
{   
  HSU_CORE_CONFIG_INIT_MODE_OTG,
  HSU_CORE_CONFIG_INIT_MODE_DEVICE,
  HSU_CORE_CONFIG_INIT_MODE_HOST,
  HSU_CORE_CONFIG_INIT_MODE_LAST
} hsu_core_config_init_mode_enum;


typedef struct hsu_core_config_type
{
  hsu_core_config_init_mode_enum init_mode;
  void* hsu_conf_sel_core_info;
  void* hsu_os_abstrct_core_info;
}hsu_core_config_type;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
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
hsu_core_config_type* hsu_core_config_get_core(uint8 core_idx);

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_CORES_TABLE_SIZE

DESCRIPTION
This .
The table will be function returns the number of cores in the cores table (including the NULL cores).

DEPENDENCIES
None.

RETURN VALUE
The number of cores in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_cores_table_size(void);

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
uint8 hsu_core_config_get_num_of_cores(void);

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
hsu_core_config_type* hsu_core_config_get_hs_main_core(void);
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
hsu_core_config_type* hsu_core_config_get_uicc_core(void);

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_CLEAR_UICC_CORE

DESCRIPTION
This function clears the UICC core

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_core_config_clear_uicc_core(void);
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_UICC */

#ifdef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_BOOT_CORE

DESCRIPTION
This function returns the boot core (used in DLOAD/ARMPRG/fast enumeration/dead battery charging

DEPENDENCIES
None.

RETURN VALUE
The address of the boot core.

SIDE EFFECTS
None. 

==============================================================================*/
hsu_core_config_type* hsu_core_config_get_boot_core(void);
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
hsu_core_config_type* hsu_core_config_get_fs_legacy_core(void);
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_LEGACY_DEVICE */

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
hsu_core_config_init_mode_enum hsu_core_config_get_stack_init_mode(uint8 core_idx);

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
void hsu_core_config_set_hs_main_core_conf_sel_info(void* conf_sel_info);

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
void hsu_core_config_set_boot_core_conf_sel_info(void* conf_sel_info);

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
void hsu_core_config_set_fs_legacy_core_conf_sel_info(void* conf_sel_info);

#endif /* FEATURE_HS_USB_LEGACY_DEVICE */

#endif /* _HSU_CORE_CONFIG_H_ */


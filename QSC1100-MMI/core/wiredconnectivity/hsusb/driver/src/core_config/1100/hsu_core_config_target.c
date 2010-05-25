/*==============================================================================

                            HS-USB Core configuration

GENERAL DESCRIPTION
    Contains the core configuration for HS-USB for QCS1100.

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
Forward Declarations
==============================================================================*/
extern hsu_core_config_type cores_details[HSU_NUM_OF_CORES_TYPES]; 

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
/* A table that holds the cores that are relevant for this target
in all its possible configurations */
hsu_core_config_type* hsu_core_config_table[]=
{
  /* High-Speed USB core. */
#ifndef FEATURE_HS_USB_BASIC
    &cores_details[HSU_HS_MAIN_CORE]
#else
    &cores_details[HSU_BOOT_CORE]
#endif
};


/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/


/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_CORES_TABLE_SIZE

DESCRIPTION
This function returns the number of cores in the cores table (including the NULL cores).

DEPENDENCIES
None.

RETURN VALUE
The number of cores in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_cores_table_size(void)
{
  return sizeof(hsu_core_config_table)/sizeof(hsu_core_config_type*);
}

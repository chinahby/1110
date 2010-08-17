#ifndef _HSU_LEGACY_PHY_H_
#define _HSU_LEGACY_PHY_H_

/*==============================================================================
  GENERAL DESCRIPTION
   
  EXTERNALIZED FUNCTIONS
  hsu_init_legacy_phy()
  hsu_init_fs_usb()

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_config_selector_init() should be called before any other function in 
    this file is called.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_legacy_phy.h#2 $
  $DateTime: 2009/02/11 06:48:45 $ $Author: ablay $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  02/06/07  ke  Initial version
    
==============================================================================*/
#include "customer.h"
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
FUNCTION HSU_INIT_LEGACY_PHY

DESCRIPTION
This function inits Philips phy 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_init_legacy_phy(void);

/*===========================================================================
FUNCTION hsu_pm_init_fs_usb

DESCRIPTION
Perform PMIC-related initialization needed for using the full-speed USB core.

DEPENDENCIES
PMIC must be initialized.

RETURN VALUE
NONE

SIDE EFFECTS
None.
===========================================================================*/
void hsu_init_fs_usb(void);

#endif /* _HSU_LEGACY_PHY_H_ */

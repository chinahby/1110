#ifndef _HSU_APP_APIS_H_
#define _HSU_APP_APIS_H_

/*==============================================================================

     High Speed USB Module Remote APIs on the Application processor
      H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module defines High Speed USB APIs that are implemented on the 
    application processor.  

  EXTERNALIZED FUNCTIONS
    hsu_api_get_product_id

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_config_selector_init() should be called before any other function in 
    this file is called.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_app_apis.h#4 $
  $DateTime: 2008/12/31 07:27:49 $ $Author: dsegal $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  02/06/07  ke  Initial version
    
==============================================================================*/
#include "customer.h"
#include "comdef.h" /* For boolean */

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

FUNCTION      HSU_API_GET_PRODUCT_ID

DESCRIPTION
  This function returns the product ID of the current HS-USB device composition.

DEPENDENCIES
  FEATURE_HS_USB must be defined. 
  Must be called after hsu_config_selector_init() was called.

RETURN VALUE
  The product ID in use.

SIDE EFFECTS
  None
===========================================================================*/
unsigned short hsu_api_get_product_id(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_get_product_id */
#endif

/*===========================================================================

FUNCTION      HSU_API_USE_PMIC_OTG_COMPARATORS

DESCRIPTION
  This function returns whether or not to use PMIC OTG comparators,
  based on the value of the NV item hs_usb_use_pmic_otg_comparators.

DEPENDENCIES
  update_use_pmic_otg_comparators_from_nv() must have been called before 
  calling this function.

RETURN VALUE
  TRUE if PMIC comparators will be used during LPM, Otherwise, FALSE is
  returned.

SIDE EFFECTS
  None

===========================================================================*/
boolean hsu_api_use_pmic_otg_comparators(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_use_pmic_otg_comparators */
#endif


/*===========================================================================

FUNCTION      HSU_API_CHG_FORCE_RESUME

DESCRIPTION
This function has been deprecated, and should not be used.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_api_chg_force_resume(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_chg_force_resume */
#endif

/*===========================================================================

FUNCTION      HSU_API_EXIT_LPM

DESCRIPTION
This function enables the HS-USB core running on the ARM9 processor 
to issue an LPM exit.

This function is aimed at enabling the usage of the PMIC OTG comparators
instead of the integrated PHY comparators.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_api_exit_lpm(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION hsu_api_exit_lpm */
#endif


#endif /* _HSU_APP_APIS_H_ */

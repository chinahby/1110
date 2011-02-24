/*==============================================================================

  High Speed USB Module Remote APIs on Application processor 
  
  GENERAL DESCRIPTION
    This module defines High Speed USB APIs that are implemented on the 
    application processor. 

  EXTERNALIZED FUNCTIONS
    hsu_api_get_product_id
    hsu_api_use_pmic_otg_comparators
    hsu_api_exit_lpm


  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_config_selector_init() should be called before any other function in 
    this file is called.

  Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_app_apis.c#6 $
  $DateTime: 2009/02/11 06:48:45 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  02/08/07  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_app_apis.h"
#include "hsu_config_selector.h"
#include "hsu_conf_sel_types.h"
#include "hsu_conf_sel_i.h"
#include "hsu_common.h"
#include "hsu_chg_api_helper.h"

#if !defined (FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)
#include "hsu_os_abstract_bus.h"
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS && !FEATURE_HS_USB_NO_HS_OTG_CORE */

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
  Variables
==============================================================================*/

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/


/*===========================================================================

FUNCTION      HSU_API_GET_PRODUCT_ID

DESCRIPTION
  This function returns the product ID of the current HS-USB device composition.

DEPENDENCIES
  FEATURE_HS_USB must be defined.
  hsu_config_selector_init() should have already been called.

RETURN VALUE
  The product ID in use.

SIDE EFFECTS
  None

===========================================================================*/
unsigned short hsu_api_get_product_id(void)
{
  const hsu_conf_sel_comp_info_type * cur_comp;
  cur_comp = hsu_conf_sel_get_cur_comp_details();
  HSU_ASSERT(cur_comp != NULL);
  return (unsigned short)(cur_comp->product_id);
} /* hsu_api_get_product_id */


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
boolean hsu_api_use_pmic_otg_comparators(void)
{
  #if defined(T_QSC6270) || defined(T_MSM8650)
  return hsu_conf_sel_use_pmic_otg_comparators();
  #else
  return FALSE;
  #endif  /* T_QSC6270 || T_MSM8650 */
} /* hsu_api_use_pmic_otg_comparators */



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
void hsu_api_chg_force_resume(void)
{
} /* hsu_api_chg_force_resume */


/*===========================================================================

FUNCTION      HSU_API_EXIT_LPM

DESCRIPTION
This function enables the HS-USB core running on the ARM9 processor 
to issue an LPM exit.

This function is aimed at enabling the usage of the PMIC OTG comparators
instead of the integrated PHY comparators.

If FEATURE_DONT_USE_PMIC_OTG_COMPARATORS is defined, this function does 
nothing.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_api_exit_lpm(void)
{
#if !defined (FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)
  /* Exit LPM only if the HS-OTG core is used */
  hsu_os_bus_exit_lpm(HS_USB_CORE_INDEX);
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS && !FEATURE_HS_USB_NO_HS_OTG_CORE */
}  /* hsu_api_exit_lpm */


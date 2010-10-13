/*==============================================================================

     High Speed USB Boot Dload

  GENERAL DESCRIPTION
    This file includes functions that are related to Software Download feature.

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_boot_dload.c#9 $
  $DateTime: 2009/04/06 03:48:57 $ $Author: evolfson $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  04/15/07 me  Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_boot_dload.h"
#include "boot_dload.h"
#include "clk.h"
#include "hsu_modem_proc_services.h"
#include "msm.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_common.h"

#ifndef FEATURE_HSU_EBI
  #include "msmhwioreg.h"
#endif

#ifndef FEATURE_HS_USB_BASIC
  #include "hsu_conf_sel_types.h"
  #include "AEEstd.h"
  #include "hsu_phy_utils.h" /* For spoof_disconnect */
#else
#ifndef BUILD_HOSTDL /* DLOAD */
  #include "hsu_al_dload.h"
#endif /* !BUILD_HOSTDL */
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
  #include "miaddress.h"
#endif /* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */
#endif /* !FEATURE_HS_USB_BASIC */

#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
#include "boot_shared_seg.h"
#endif

#ifdef FEATURE_HS_USB_PMIC_PHY
extern void hsu_api_spoof_reconnect(void);
#endif /* FEATURE_HS_USB_PMIC_PHY */

#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#else
#error   "HAL is required for HS-USB"
#endif /* FEATURE_HS_USB_HAL */
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
#pragma arm section zidata = "SHARED_DATA"
hsu_dload_info_type hsu_shared;
#pragma arm section zidata
#endif

/*==============================================================================
Constants and Macros
==============================================================================*/
#if defined (FEATURE_HS_USB_BASIC)
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
#if defined(MI_NAND_IMEM_ARMPRG_DLOAD_SHARE_ADDR)
#define MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR MI_NAND_IMEM_ARMPRG_DLOAD_SHARE_ADDR
#elif defined(MI_NOR_IMEM_ARMPRG_DLOAD_SHARE_ADDR)
#define MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR MI_NOR_IMEM_ARMPRG_DLOAD_SHARE_ADDR
#else
#error should define MI_NAND_IMEM_ARMPRG_DLOAD_SHARE_ADDR or MI_NOR_IMEM_ARMPRG_DLOAD_SHARE_ADDR
#endif
#endif /* FEATURE_SHARED_SEGMENT && FEATURE_HS_USB_SHARED_SEG */
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
  Typedefs
==============================================================================*/
/* A global variable used in ARMPRG to hold the IMEM SW download data.
Since all accesses to the IMEM memory should be of 32bits,and our structure includes 
also smaller fields we need to copy the memory to a static 
variable and use it to read the data
*/
#if (defined(FEATURE_HS_USB_BASIC) && defined(BUILD_HOSTDL) \
  && defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG))
hsu_dload_info_type imem_dload_info; 
#endif

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

#ifndef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_AMSS_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode.
In targets that don't have shared segment, we save the data in IRAM.
In targets that have shared segment, we save the data in the shared segment.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data.

SIDE EFFECTS

==============================================================================*/
static hsu_dload_info_type* hsu_boot_dload_get_amss_hsu_dload_info_ptr(void)
{
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
  hsu_dload_info_type* hsu_dload_info_ptr;

  /* We need to allocate shared memory */
  BOOT_SHARED_SEG_ALLOC(hsu_shared, sizeof(hsu_dload_info_type), FALSE, (void**)(&hsu_dload_info_ptr));
  if (hsu_dload_info_ptr == NULL)
  {
    HSU_MSG_ERROR("Failed to allocate the shared segment", 0, 0, 0);
  }
  return hsu_dload_info_ptr;

#else /* !(FEATURE_SHARED_SEGMENT and  FEATURE_HS_USB_SHARED_SEG) */
  return (hsu_dload_info_type*)HWIO_ADDR(IRAM_BASE);
#endif/* FEATURE_SHARED_SEGMENT and  FEATURE_HS_USB_SHARED_SEG */

}
/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_TRANSITION

DESCRIPTION
  Do all the operations needed when transitioning to DLOAD/ARMPRG 
  (like writing data to the IRAM)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_transition()
{
  hsu_dload_info_type* hsu_dload_info_ptr = hsu_boot_dload_get_hsu_dload_info_ptr();

  /* CR 175712 fix.
     In case of a crash on ARM9 during AMSS initialization, 
     which cause the phone to move to download mode, 
     the HAL did not get a chance to be initialized, 
     so we explicitly initialize it from here. */
  HAL_hsusb_Init();

  /* Stop HS-USB core */
  clk_busy_wait(10000);
  #ifdef FEATURE_HSU_EBI
#error code not present
  #else
    if (!(hsu_phy_utils_spoof_disconnect()))
    {
      HSU_MSG_ERROR("*_transition: Spoof disconnect failed!", 0, 0, 0);
    }
  #endif  /* FEATURE_HSU_EBI */

  if (hsu_dload_info_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_dload_info_ptr is NULL", 0, 0, 0);
    HSU_ASSERT(hsu_dload_info_ptr != NULL);
    return;
  }

  hsu_dload_info_ptr->hsu_version_num = HSU_BOOT_DLOAD_VERSION_NUM;

  hsu_dload_info_ptr->dload_used_hsu = FALSE;
  /* Update the product id according to the ARM9 static variable */
  hsu_dload_info_ptr->hsu_product_id = hsu_modem_proc_services_get_product_id();

  hsu_dload_info_ptr->is_null_serial_number = hsu_modem_proc_services_is_serial_number_null();

  /* save the Serial Number, to be used in DLOAD mode */
  if (hsu_modem_proc_services_is_serial_number_null() == FALSE)
  {
    memset(hsu_dload_info_ptr->serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    (void)std_strlcpy(hsu_dload_info_ptr->serial_number, hsu_modem_proc_services_get_serial_number(), HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
  }

} /* hsu_boot_dload_transition */

#else /* FEATURE_HS_USB_BASIC */
/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_ARMPRG_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. 
In targets that have IRAM, we read the data from IRAM in DLOAD and ARMPRG.
In targets that don't have IRAM, we read the data from the shared segment in DLOAD 
and from IMEM in ARMPRG (since shared segment is not integrated to ARMPRG).

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data.

SIDE EFFECTS

==============================================================================*/
hsu_dload_info_type* hsu_boot_dload_armprg_get_hsu_dload_info_ptr(void)
{
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
#ifndef BUILD_HOSTDL /* DLOAD */
  hsu_dload_info_type* hsu_dload_info_ptr;

  /* Download mode: We need to access the shared memory allocated in AMSS */
  BOOT_SHARED_SEG_GET_DATA(hsu_shared, (void**)(&hsu_dload_info_ptr));
  if (hsu_dload_info_ptr == NULL)
  {
    /* The memory has not been allocated --> try to allocate it now. */
    BOOT_SHARED_SEG_ALLOC(hsu_shared, sizeof(hsu_dload_info_type), FALSE, (void**)(&hsu_dload_info_ptr));

    if (hsu_dload_info_ptr == NULL)
    {
      /* Since HSU_ASSERT does nothing in DLOAD/ARMPRG I also add a message */
      HSU_MSG_ERROR("Shared segment is NULL", 0, 0, 0);
      HSU_ASSERT(hsu_dload_info_ptr != NULL);
    }
    else
    {
      /* Indicate that the data is invalid */
      hsu_boot_dload_mark_dload_info_invalid(hsu_dload_info_ptr);
    }
  }
  return hsu_dload_info_ptr;
#else /* !BUILD_HOSTDL (=ARMPROG) */
  /* All accesses to the IMEM memory should be of 32bits.
  Since our structure have also smaller variables we need to copy the memory to a static 
  variable and use it to read the data */
  memcpy((void*)&imem_dload_info, (void*)MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR, sizeof(hsu_dload_info_type));
  return (hsu_dload_info_type*)&imem_dload_info;
#endif /*BUILD_HOSTDL*/

#else /* !FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */
  return (hsu_dload_info_type*)HWIO_ADDR(IRAM_BASE);
#endif/* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_CHECK_AND_UPDATE_VERSION_NUM

DESCRIPTION
  Check if we have a version number in the IRAM data or not.
  The version number can be invalid if we have a new version on the ARMPRG
  but the AMSS ran an old version so it didn't update the version_num in 
  transition to DLOAD.
  If the version number is not valid - it means that we have an old IRAM data
  so we need to update it with the new variables

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_check_and_update_version_num(void)
{
#if !defined(FEATURE_SHARED_SEGMENT) && !defined(FEATURE_HS_USB_SHARED_SEG)
  uint32 version_num;
  uint32 product_id;
  uint32 dload_used_hsu;

  hsu_dload_info_type* hsu_dload_info_ptr = 
    hsu_boot_dload_armprg_get_hsu_dload_info_ptr();

  if (hsu_dload_info_ptr == NULL)
  {
    HSU_ASSERT(hsu_dload_info_ptr != NULL);
    return;
  }

  version_num = hsu_dload_info_ptr->hsu_version_num;

  /* We check if the data that we have as version_num in the IRAM is actually a version
      number (meaning the target is already loaded with a version that includes a 
      version number in the IRAM data structure and we have nothing to update).
      If the IRAM data in the version num position is actually a product id (and the
      dload_used_hsu flag), then the target is loaded with a version that includes only 
      dload_used_hsu and product_id in the IRAM data structure.
      In that case we update the IRAM memory to also include the an invalid version_num */
  dload_used_hsu = version_num && 0xFF;
  product_id = version_num >> 16;

  if (hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu(product_id))
  {
    /* The version_num is actually the product id.
    ** We need to set the version_num, the dload_used_hsu and the product_id.
    We set the version number to HSU_BOOT_DLOAD_INVALID_VERSION_NUM to indicate that we
    had an old IRAM version for setting the serial number correctly later
    */
    hsu_dload_info_ptr->hsu_version_num = HSU_BOOT_DLOAD_INVALID_VERSION_NUM;
    hsu_dload_info_ptr->dload_used_hsu = (boolean)dload_used_hsu;
    hsu_dload_info_ptr->hsu_product_id = (uint16)product_id;
  }
#endif /* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG*/
}

#ifndef BUILD_HOSTDL /* DLOAD */

/*===========================================================================

FUNCTION hsu_boot_dload_armprg_transition

DESCRIPTION
This function should be called before transitioning to ARMPRG.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_boot_dload_armprg_transition(void)
{
  hsu_al_dload_drain();
  hsu_al_dload_shutdown();
  hsu_al_dload_mark_dload_used_hsu();
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
  hsu_boot_dload_copy_shared_segment_to_imem();
#endif /* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */
#ifdef FEATURE_HS_USB_PMIC_PHY
  /* Do disconnect and connect cable back before transting to HOST DL.
   * It is because HOST DL will not control the state of D+/D- lines. */
  hsu_api_spoof_reconnect();
#endif /* FEATURE_HS_USB_PMIC_PHY */
}

/*===========================================================================

FUNCTION hsu_boot_dload_copy_shared_segment_to_imem

DESCRIPTION
This function copies the shared segment data to the IMEM to be used by ARMPRG
in targets that don't have IRAM.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
void hsu_boot_dload_copy_shared_segment_to_imem(void)
{
  /* TODO : may need to do this for ULC as well */
  hsu_dload_info_type* dload_info_data_ptr = hsu_boot_dload_get_hsu_dload_info_ptr();
  hsu_dload_info_type* imem_dload_info_data_ptr = (hsu_dload_info_type*)MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR;

  /*If this is a non IRAM target and we are in DLOAD mode, then save the info 
  on IMEM. This should be done because shared segment is not defined in 
  ARMPRG */
  memcpy((void*)imem_dload_info_data_ptr, 
    (void*)dload_info_data_ptr, 
    sizeof(hsu_dload_info_type));
}
#endif /* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */

#endif /* !BUILD_HOSTDL */

#endif /* !FEATURE_HS_USB_BASIC */

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_MARK_DLOAD_INFO_INVALID

DESCRIPTION
Mark that the DLOAD related data is invalid

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_mark_dload_info_invalid(hsu_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We mark that the data is invalid by setting an invalid product_id */
    dload_info_ptr->hsu_product_id = HSU_PRODUCT_ID_INVALID;
  }
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_IS_DLOAD_INFO_VALID

DESCRIPTION
Checks if the DLOAD related data is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the data is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean hsu_boot_dload_is_dload_info_valid(const hsu_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We verify that the data is valid according to the product_id validity */
    if (hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu(dload_info_ptr->hsu_product_id))
    {
      return TRUE;
    }
    else
    { 
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode, and read in download mode, both by DLOAD and ARMPRG.
In targets that have IRAM, we save the data in IRAM while in AMSS and read it 
from there in DLOAD and ARMPRG.
In targets that don't have IRAM, we save the data in the shared segment, 
read it from the shared segment in DLOAD and save it in IMEM for the ARMPRG
usage (since shared segment is not integrated to ARMPRG).

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data.

SIDE EFFECTS

==============================================================================*/
hsu_dload_info_type* hsu_boot_dload_get_hsu_dload_info_ptr(void)
{
#ifdef FEATURE_HS_USB_BASIC
  return hsu_boot_dload_armprg_get_hsu_dload_info_ptr();
#else  /* !FEATURE_HS_USB_BASIC */
  return hsu_boot_dload_get_amss_hsu_dload_info_ptr();
#endif /* FEATURE_HS_USB_BASIC */
}

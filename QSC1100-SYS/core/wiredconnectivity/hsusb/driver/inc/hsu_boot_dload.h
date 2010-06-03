#ifndef _HSU_BOOT_DLOAD_H_
#define _HSU_BOOT_DLOAD_H_

/*==============================================================================

     High Speed USB Boot Dload
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file includes functions that are related to Software Download feature.

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
    Functions should only be called from the TMC's context.     
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_boot_dload.h#3 $
  $DateTime: 2008/07/07 04:20:29 $ $Author: omanos $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  04/15/07 me  Initial version
    
==============================================================================*/
#include "hsu_conf_sel_types.h"

#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
#include "boot_shared_seg.h"
#else
#include "boot_dload.h"
#endif  /*FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */


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
/* Structure for passing information required for the SW download between AMSS,
DLOAD and ARMPRG */
typedef struct
{
#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
  BOOT_SHARED_SEG_HEADER
#else /*! (FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG) */
  dload_id_type                  dload_id;
#endif
  uint32                         hsu_version_num;
  /* Set to FALSE before going into download mode.
  ** Set to TRUE by the HS-USB DLOAD adaptation layer.
  ** Used for indicating to ARMPRG whether DLOAD worked over HSU or not.
  ** FALSE means that DLOAD worked over UART.
  */
  uint16                        dload_used_hsu; 
  /* The product ID used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  uint16                         hsu_product_id;
  /* The Serial Number used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  char                           serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN];

  uint16                         is_null_serial_number;
}
hsu_dload_info_type;

/* Version number of the IRAM/shared segment/IMEM data */
#define HSU_BOOT_DLOAD_VERSION_NUM 3
#define HSU_BOOT_DLOAD_INVALID_VERSION_NUM 0xFFFFFFFF

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_TRANSITION

DESCRIPTION
  Do all the operation needed before entering download mode 
  (like writing data to the IRAM)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_transition(void);

#else /* DLOAD/ARMPROG */

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_CHECK_AND_UPDATE_VERSION_NUM

DESCRIPTION
  Check if we have a version number in the IRAM data or not.
  The version number can be invalid if we have a new version on the ASRMPROG
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
void hsu_boot_dload_check_and_update_version_num(void);

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
void hsu_boot_dload_armprg_transition(void);

#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_HS_USB_SHARED_SEG)
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
void hsu_boot_dload_copy_shared_segment_to_imem(void);

#endif /* FEATURE_SHARED_SEGMENT and FEATURE_HS_USB_SHARED_SEG */

#endif /* !BUILD_HOSTDL */

#endif /* FEATURE_HS_USB_BASIC */

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
void hsu_boot_dload_mark_dload_info_invalid(hsu_dload_info_type* dload_info_ptr);

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
boolean hsu_boot_dload_is_dload_info_valid(const hsu_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
hsu_dload_info_type* hsu_boot_dload_get_hsu_dload_info_ptr(void);


#endif /* _HSU_BOOT_DLOAD_H_ */

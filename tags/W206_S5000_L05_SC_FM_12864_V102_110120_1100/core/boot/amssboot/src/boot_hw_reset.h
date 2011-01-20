#ifndef BOOT_HW_RESET_H
#define BOOT_HW_RESET_H

/*===========================================================================

                       B o o t  D o w n l o a d e r   
                           H e a d e r  F i l e

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_hw_reset.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifndef BUILD_BOOT_CHAIN

/*===========================================================================

FUNCTION  BOOT_INIT_ABNORMAL_HW_RESET_DETECTION

DESCRIPTION
 This function will init magic_2 in hw reset info in shared segment. Thus 
 informing OEMSBL that AMSS is using shared memory. This function should only be
 called from AMSS code.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_init_abnormal_hw_reset_detection(void);

/*===========================================================================

FUNCTION  BOOT_RESET_EARLY_ERROR_DETECTION

DESCRIPTION
 This function will reset magic_3 in hw reset info in shared segment. Thus
 information there is nothing wrong from 0x0 to tmc_init is done.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_reset_early_error_detection(void);

/*===========================================================================

FUNCTION  BOOT_RESET_IF_EARLY_ERROR

DESCRIPTION
 This function will check magic number 3 of hw_reset_info in shared segment.
 If it is set, it means err fatal or abort happened before tmc_init is done.
 Trigger watch dog reset and let the phone go into dload mode.
 
DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 Cause the phone reset.
===========================================================================*/
void boot_reset_if_early_error(void);

/*===========================================================================

FUNCTION  BOOT_HW_RESET_SHARED_INFO_RESET

DESCRIPTION
 This function will save hw reset info to shared segment

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_hw_reset_shared_info_reset(void);
#endif /* BUILD_BOOT_CHAIN */

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  OEMSBL_HW_RESET_SHARED_INFO_INIT

DESCRIPTION
 This function will init hw reset info to shared segment

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void oemsbl_hw_reset_shared_info_init(void);

/*===========================================================================

FUNCTION  BOOT_HW_RESET_DLOAD_ENTRY

DESCRIPTION
  Determines if identifier is present in IRAM to tell OEMSBL to enter
  the boot downloader, instead on continuing the normal boot process.
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates downloader should be entered.

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_hw_reset_dload_entry(void);

/*===========================================================================

FUNCTION  OSBL_HW_RESET_SHARED_INFO_INIT

DESCRIPTION
 This function will init hw reset info in shared segment. This function
 should only be called from OEMSBL. 

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void osbl_hw_reset_shared_info_init(void);
#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_HW_RESET_H */

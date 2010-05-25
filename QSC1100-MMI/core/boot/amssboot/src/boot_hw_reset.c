/*=============================================================================

                         B o o t  H a r d w a r e  R e s e t
 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006-2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/boot_hw_reset.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/30/07   ps       Adding support for SECBOOT 2.0
05/29/07   ah       Fixed logic in boot_early_error
04/11/07   ah       Separate functions to check for early error and reset
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#ifdef BUILD_BOOT_CHAIN
//#include "osbl_msm.h"
#else
#include "target.h"
#endif

#include "boot_hw_reset.h"
#include "boot_shared_seg.h"
#include "boot_dload.h"
#include "pm.h"
#include "msm.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Structure to define ID to enter download mode. */ 
typedef struct
{
  uint32 magic_1;   /* Will be initialized by OEMSBL code */
  uint32 magic_2;   /* Will be initialized by AMSS code   */
  uint32 magic_3;   /* Will be initialized in boot_ram_init after reach 0x0 */
} hw_reset_id_type;

#ifdef FEATURE_SHARED_SEGMENT
/* Magic numbers for notifying OEMSBL to boot into download mode. */
#define HW_RESET_MAGIC_NUM_1     0x12345678
#define HW_RESET_MAGIC_NUM_2     0x9ABCDEF0
#define HW_RESET_MAGIC_NUM_3     0x1A2B3C4D

typedef struct
{
    BOOT_SHARED_SEG_HEADER
    hw_reset_id_type cookies;
} hw_reset_shared_type;

#ifndef BUILD_BOOT_CHAIN
#pragma arm section zidata = "SHARED_DATA"
hw_reset_shared_type hw_reset_shared;
#pragma arm section zidata
#endif
#endif

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
#ifdef FEATURE_SHARED_SEGMENT
/*===========================================================================

FUNCTION  BOOT_GET_HW_RESET_SHARED_ID_ADDR

DESCRIPTION
  This function returns the address of QPST download magic numbers.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Address of QPST download magic numbers in SDRAM

SIDE EFFECTS
  None
===========================================================================*/
static hw_reset_id_type * boot_get_hw_reset_shared_id_addr( void )
{
  hw_reset_shared_type * hw_reset_shared_info_ptr = NULL;
  boolean seg_data_invalid;

  /* Get the pointer to hw_reset_shared */
  seg_data_invalid = BOOT_SHARED_SEG_GET_DATA( hw_reset_shared,
                                               (void**)(&hw_reset_shared_info_ptr)
                                             );

  if ( (hw_reset_shared_info_ptr != NULL) && (seg_data_invalid == FALSE))
  {
    return &(hw_reset_shared_info_ptr->cookies);
  }

  return NULL;
}
#endif

#ifdef BUILD_BOOT_CHAIN
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
void osbl_hw_reset_shared_info_init(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_shared_type * hw_reset_shared_info_ptr = NULL;
  /* Allocate hw_reset_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( hw_reset_shared, sizeof(hw_reset_shared_type), 
                         FALSE, (void**)(&hw_reset_shared_info_ptr)
                       );

  if(hw_reset_shared_info_ptr == NULL)
    return;

  /* First write zeroes to all magic numbers */
  hw_reset_shared_info_ptr->cookies.magic_1 = 0;
  hw_reset_shared_info_ptr->cookies.magic_2 = 0;
  hw_reset_shared_info_ptr->cookies.magic_3 = 0;

  /* Now Init Magic Number */
  hw_reset_shared_info_ptr->cookies.magic_1 = HW_RESET_MAGIC_NUM_1;
  
#ifdef FEATURE_EARLY_ERROR_RESET
  /* Set magice number 3 to indicate boot up process have reached 0x0 
   * This magic number will be cleared after tmc_init is done and 
   * magic number 2 will be set to indicate boot up process is done.
   */
  hw_reset_shared_info_ptr->cookies.magic_3 = HW_RESET_MAGIC_NUM_3;
#endif /* FEATURE_EARLY_ERROR_RESET */
  
#endif
}

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
boolean boot_hw_reset_dload_entry(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_id_type *hw_reset_id_ptr = 
                      (hw_reset_id_type*) boot_get_hw_reset_shared_id_addr();

  if (hw_reset_id_ptr == NULL)
    return FALSE;

  /* Check to see if download ID is present. */
  if ( (hw_reset_id_ptr->magic_1 == HW_RESET_MAGIC_NUM_1) &&
       (hw_reset_id_ptr->magic_2 == HW_RESET_MAGIC_NUM_2)
     )
  {
    /* Case 1: Abnormal HW reset happens after boot up */

    /* Clear ID so the downloader is not entered again on the next boot. */
    hw_reset_id_ptr->magic_1 = 0;
    hw_reset_id_ptr->magic_2 = 0;
    
    return TRUE;
  }
  else if ( (hw_reset_id_ptr->magic_1 == HW_RESET_MAGIC_NUM_1) &&
            (hw_reset_id_ptr->magic_3 == HW_RESET_MAGIC_NUM_3)
          ) 
  {
    /* Case 2: HW reset triggered due to error(s) happened during 0x0 to tmc_init */

    /* Clear ID so the downloader is not entered again on the next boot. */
    hw_reset_id_ptr->magic_1 = 0;
    hw_reset_id_ptr->magic_3 = 0;
    
    return TRUE;
  }
  else
  {
    return FALSE;
  }
#else
  return FALSE;
#endif

}
#endif /* BUILD_BOOT_CHAIN */

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_HW_RESET_SHARED_INFO_RESET

DESCRIPTION
 This function will reset the magic number set by OEMSBL. Thus preventing
 OEMSBL code from entering into Download Mode.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_hw_reset_shared_info_reset(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_id_type *hw_reset_id_ptr = 
                      (hw_reset_id_type*) boot_get_hw_reset_shared_id_addr();

  if (hw_reset_id_ptr == NULL)
    return;

  /* Clear the magic number set by OEMSBL code */
  hw_reset_id_ptr->magic_1 = 0;
#endif   
}

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
void boot_init_abnormal_hw_reset_detection(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_id_type *hw_reset_id_ptr = 
                      (hw_reset_id_type*) boot_get_hw_reset_shared_id_addr();

  if (hw_reset_id_ptr == NULL)
    return;

  /* Now Init Magic AMSS Number and hence completing the handshake between
   * OEMSBL and AMSS code base.
   */
  hw_reset_id_ptr->magic_2 = HW_RESET_MAGIC_NUM_2;

#endif /* FEATURE_SHARED_SEGMENT */
}

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
void boot_reset_early_error_detection(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_id_type *hw_reset_id_ptr = 
                      (hw_reset_id_type*) boot_get_hw_reset_shared_id_addr();

  if (hw_reset_id_ptr == NULL)
    return;

  /* Clear magic number 3 to indicate boot up process is done*/
  hw_reset_id_ptr->magic_3 = 0;

#endif /* FEATURE_SHARED_SEGMENT */
}

/*===========================================================================

FUNCTION  BOOT_EARLY_ERROR

DESCRIPTION
 This function will check magic number 3 of hw_reset_info in shared segment.
 If it is set, it means err fatal or abort happened before tmc_init is done.
 
DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 Cause the phone reset.
===========================================================================*/
boolean boot_early_error(void)
{
  boolean early_error = FALSE;

#ifdef FEATURE_SHARED_SEGMENT
  hw_reset_id_type *hw_reset_id_ptr = 
                      (hw_reset_id_type*) boot_get_hw_reset_shared_id_addr();

  /* magic number is cleared only after tmc_init is done */
  early_error = (hw_reset_id_ptr != NULL) && (hw_reset_id_ptr->magic_3 == HW_RESET_MAGIC_NUM_3);
#endif /* FEATURE_SHARED_SEGMENT */

  return early_error;
}

/*===========================================================================

FUNCTION  BOOT_TRIGGER_RESET

DESCRIPTION
 This function triggers a watch dog reset and lets the phone go into dload mode.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 Cause the phone reset.
===========================================================================*/
void boot_trigger_reset(void)
  {
  }

#endif /* BUILD_BOOT_CHAIN */


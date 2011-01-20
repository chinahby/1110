#ifndef __FLASH_NOR_MSM_H__
#define __FLASH_NOR_MSM_H__

/**********************************************************************
 * flash_nor_msm.h
 *
 * NOR Device interface.
 *
 * This file implements MSM specific macros used by NOR device
 * functions.  Every MSM will have its own version customized for
 * that particular target.
 *
 * Copyright (c) 2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/flash/MSM_FLASH.01.04/flash_nor_msm.h#3 $
  $DateTime: 2008/04/11 09:43:00 $
  $Author: c_surven $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-03-14   sv      Modified for m18 half-density support
2008-03-11   sv      Use of cfi data for block size and sector count
2008-03-05   op      Add Nor base address definition for shadow Ram mode
2008-01-08   op      Initial version
===========================================================================*/

#include "flash.h"
#include "flash_nor_device.h"
#include "msm.h"


#if !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) && defined(FLASH_NOR_PARTI_TBL)
  #include "miheader.h"
  #define FLASH_NON_ZERO_PROBE_ADDR_OFFSET (MI_NOR_AMSS_MIPIB_LOCATION_ADDR)
#else
  #define FLASH_NON_ZERO_PROBE_ADDR_OFFSET (FLASH_NOR_EFS2_START_BYTE_OFFSET)
#endif



/****************************************************************
 * Defines
 ***************************************************************/

#if defined(BOOT_MODE_NOR) && !defined(FLASH_NOR_PARTI_TBL)
/* Provide the ability to reserve the first n blocks for the flash for
 * non-dmss use. This is needed for the flash driver fs_nor_device
 * definition and possibly the MMU code to disable caching on EFS2 NOR sectors
 */
  /* EFS start offset (in bytes) in flash */
  #ifndef FLASH_NOR_EFS2_START_BYTE_OFFSET
    #error "Need to define EFS2 start address(FLASH_NOR_EFS2_START_BYTE_OFFSET) in custxxxx file"
  #endif

  #ifndef FLASH_NOR_EFS2_END_BYTE_OFFSET
    #error "Need to define EFS2 end address(FLASH_NOR_EFS2_END_BYTE_OFFSET) in custxxxx file"
  #endif

  /* If FDI is not being used on Intel MLC parts enable half-density mode */
  #if !defined(FEATURE_EFS_FDI)
    #define FLASH_NOR_HD_MODE
  #endif

#else

  #if defined(BOOT_MODE_NAND)
    #define FLASH_NOR_EFS2_START_BYTE_OFFSET   0
    #define FLASH_NOR_EFS2_END_BYTE_OFFSET     1
  #endif

#endif

#if ( defined(BUILD_JFLASH) && !defined(BUILD_FOR_X86) )

/*===========================================================================

MACRO    KICK_WATCHDOG

DESCRIPTION
  This macro resets the hardware watchdog timer so that it does not
  reset the processor.  The watchdog timer expires if it is not reset
  every 400ms (approximately).

DEPENDENCIES
  Assumes that the watchdog timer is reset by writing these two values
  to this particular register.

  This macro makes no provision for providing a minimum delay between
  kicks.  Since this macro may be used very often in short loops, it
  is essential that the hardware watchdog not require any such minimum
  delay.  The watchdog in the MSM2.0 has no minimum delay requirement.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

  #define  KICK_WATCHDOG()   /*lint -e717 do ... while(0) */      \
    do {                                                          \
         (void)outp(HWIO_WDOG_RESET_ADDR, HWIO_WDOG_RESET_RMSK);  \
    } while (0)    
                             /*lint +e717 */
#elif defined(BUILD_FOR_X86)

  #define KICK_WATCHDOG() /* */

/*===========================================================================

MACRO    CHECK_FOR_DATA

DESCRIPTION
  This macro is just a place holder.  Only used in ARMPRG

===========================================================================*/

  #define  CHECK_FOR_DATA()

#endif /* BUILD_JFLASH */

#if (defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)) && \
    !defined(BUILD_FOR_X86)

  #define KICK_DOG_AND_CHECK_DATA()    do {                 \
                                         KICK_WATCHDOG();   \
                                         CHECK_FOR_DATA();  \
                                       } while (0)

#else

  #define KICK_DOG_AND_CHECK_DATA()    /* Do Nothing */

#endif /* defined(BUILD_JFLASH) || defined(BUILD_ARMPRG) */


extern dword tramp_irq_mask_0;
extern dword tramp_irq_mask_1;

#if defined ( FEATURE_INT_125_FIQ )
  extern dword tramp_fiq_mask_0;
#endif

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  #define INTERRUPTED()     0
#else
  #if defined(FEATURE_INT_125_FIQ)
    #define INTERRUPTED()  (HWIO_INT_STATUS_0_INM(tramp_irq_mask_0)  || \
                            HWIO_INT_STATUS_1_INM(tramp_irq_mask_1)  || \
                            HWIO_INT_STATUS_0_INM(tramp_fiq_mask_0))
  #else
    #define INTERRUPTED()  (HWIO_INT_STATUS_0_INM(tramp_irq_mask_0)  || \
                            HWIO_INT_STATUS_1_INM(tramp_irq_mask_1))
  #endif

#endif

/* Function to do any MSM specific hardware initialization for
 * NOR flash operations
 */
extern void flash_nor_msm_init(void);

/*   Execute the CFI query command to read out block layout and fill
 *   corresponding flash fsi_nor_device structure. Also validate
 *   fsi_nor_device structure */
extern flash_status
flash_nor_geometry_init (flashi_nor_device *nor_device,
                         flash_ptr_type baseaddr);

/*  Validate the partition information with the flash geometry fields */
extern void
flash_nor_validate_structure (struct fsi_device_data *self,
  flash_ptr_type baseaddr);

#endif /* End of __FLASH_NOR_MSM_H__ */

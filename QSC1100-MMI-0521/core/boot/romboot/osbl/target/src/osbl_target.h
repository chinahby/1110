#ifndef OSBL_TARGET_H
#define OSBL_TARGET_H

/*===========================================================================

           O S  Secondary  B o o t  L o a d e r Target
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  that is target-specific

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/osbl/osbl_target.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

#include "miheader.h"
#include "customer.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Define the MIN macro if it has not been defined. This is necessary when
  the datamover is not used.
---------------------------------------------------------------------------*/
#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

/*---------------------------------------------------------------------------
  Definitions of address range where IRAM exists on the MSM.
---------------------------------------------------------------------------*/
#ifndef IRAM_SIZE
#define IRAM_SIZE               4096
#endif
#define IRAM_RANGE_START_ADDR   0x58010000
#define IRAM_RANGE_END_ADDR     (IRAM_RANGE_START_ADDR + IRAM_SIZE)
/*---------------------------------------------------------------------------
  Definitions of address range where SDRAM, PSRAM, NOR, or IMEM can exist on
  the MSM.
---------------------------------------------------------------------------*/
//Laura need to double check this
#define RANGE_END_ADDR          0x8003FFFFUL  

/*---------------------------------------------------------------------------
  Define the base address for the ARM9 vector table.
---------------------------------------------------------------------------*/
#define OSBL_VECTOR_TABLE_BASE  0x58000000

/*---------------------------------------------------------------------------
  Define the base address and size for the error log.
---------------------------------------------------------------------------*/
#define OSBL_ERR_DATA_BASE  0x58010000
#define OSBL_ERR_DATA_SIZE  4096

/*---------------------------------------------------------------------------
  Define the destination address and size of the OSBL.
---------------------------------------------------------------------------*/
#define OSBL_CODE_BASE       MI_NOR_OSBL_IMG_DEST_ADDR
#define OSBL_MAX_IMAGE_SIZE  0x20000
#define OSBL_MAX_CODE_SIZE   0x50000

/*---------------------------------------------------------------------------
  OSBL stack base address
---------------------------------------------------------------------------*/
#define OSBL_STACK_BASE         MI_NOR_OSBL_IMG_STACK_ADDR

/*---------------------------------------------------------------------------
  Define the format of the AMSS ELF image
---------------------------------------------------------------------------*/
#define OSBL_AMSS_ELF_FORMAT   BOOT_SEC_ELF_FORMAT_LARGE

/*---------------------------------------------------------------------------
  Define the ELF format that the ELF loader should support
---------------------------------------------------------------------------*/
#define BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT

/*---------------------------------------------------------------------------
  Define the number of instances that need to be supported by the factories.
---------------------------------------------------------------------------*/
#define BOOT_ELF_LOADER_MAX_INSTANCES 0
#define BOOT_SEC_ELF_LOADER_MAX_INSTANCES 1
#define BOOT_FLASH_TRANS_MAX_INSTANCES 1

#ifndef _ARM_ASM_
/* Forward declarations for the function prototype below */
struct bl_shared_data_type;
struct boot_dbl_if_shared_info_type;

/* Make the bl_shared_data a global */
extern struct bl_shared_data_type bl_shared_data;
#endif /* _ARM_ASM_ */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
/*===========================================================================

FUNCTION  OSBL_TARGET_RETRIEVE_SHARED_INFO_FROM_DBL

DESCRIPTION
  This function retreieves target-specific shared data from the DBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_target_retrieve_shared_info_from_dbl
(
  struct bl_shared_data_type *bl_shared_data,
  struct boot_dbl_if_shared_info_type *dbl_shared_info_ptr
);

/*=============================================================================
FUNCTION      OSBL_CLK_BUSY_WAIT

DESCRIPTION   This functions wait based on how long it is told.
DEPENDENCIES  None

RETURN VALUE  None
=============================================================================*/
extern void osbl_clk_busy_wait(uint32);

/*===========================================================================

FUNCTION  OSBL_TARGET_GET_CUST_AMSS_PARTI_NAME

DESCRIPTION
  Returns the name of the custom AMSS partition used by the customer.
  If the customer doesn't use a custom name, this function returns NULL.

DEPENDENCIES
  None

RETURN VALUE
  The custom AMSS partition name, or NULL to use the default name

SIDE EFFECTS
  None

===========================================================================*/
extern char *osbl_target_get_cust_amss_parti_name( void );

/*===========================================================================

FUNCTION  OSBL_IS_AMSS_HASH_CHECKING_REQUIRED

DESCRIPTION
  This function is used by the OSBL to determine if AMSS hash checking
  is required.
 
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if AMSS hash checking is required, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean osbl_is_amss_hash_checking_required( void );

/*=============================================================================
FUNCTION  OSBL_GET_NOR_PARTITION_TABLE_LOCATION

DESCRIPTION
  This function will try to find NOR partition table location stored somewhere
  in NOR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECT
  None.
=============================================================================*/
extern uint8* osbl_get_nor_partition_table_location(void);

#endif /* _ARM_ASM_ */

#endif  /* OSBL_TARGET_H */



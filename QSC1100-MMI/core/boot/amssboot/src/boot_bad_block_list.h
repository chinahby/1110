#ifndef BOOT_BAD_BLOCK_LIST_H
#define BOOT_BAD_BLOCK_LIST_H

/*===========================================================================

DESCRIPTION
  This header file gives the definition of Bad Block List structure.
  This structure is used to do Logical to Physical mapping.

Copyright(c) 2004,2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include  "comdef.h"

 
/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_bad_block_list.h#3 $ $DateTime: 2005/11/28 13:04:39 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/11/05   tbg     Changes for multiboot architecture
08/14/04   ktr     Created.

===========================================================================*/

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
 
/* 
 * These definitions related to the BBTB are arbitrary, and are not
 * limited by the bad_block_table definition itself.  The size
 * limitations have been added here to allow static alloaction of the
 * BBTB list in memory.
 */

#define MAX_BBTB_PAGES 8

/* size of all fields of the BBTB, minus the bad_block table itself) */
#define SIZEOF_BBTB_INFO (4 * sizeof(uint32))
#define BBTB_BUFFER_SIZE (512*MAX_BBTB_PAGES)

/*===========================================================================
                      FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION BOOT_NAND_FLASH_INITIALIZE

DESCRIPTION
  This function initializes nand flash  and the boot info block .

DEPENDENCIES
  None

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

boolean boot_nand_flash_initialize(void);


/*===========================================================================

FUNCTION boot_read_bad_block_list

DESCRIPTION
  This function creates the bad block list.

DEPENDENCIES

RETURN VALUE
  If success, return TRUE else FALSE

===========================================================================*/

boolean boot_read_bad_block_list(void);


boolean boot_progressive_init(void);


#endif /* BOOT_BAD_BLOCK_LIST_H */












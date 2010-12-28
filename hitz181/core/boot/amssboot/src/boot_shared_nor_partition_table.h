#ifndef BOOT_SHARED_NOR_PARTITION_TABLE_H
#define BOOT_SHARED_NOR_PARTITION_TABLE_H

/*===========================================================================

                   N O R   P a r t i t i o n   T a b l e
 
GENERAL DESCRIPTION
  This file contains the code to such that the processed config data can be
  stored in shared memory by OEMSBL and then later used by AMSS code.
  
EXTERNALIZED FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_nor_partition_table.h#2 $

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

#include "miparti.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  BOOT_NOR_PARTITION_TABLE_LOCATION

DESCRIPTION
 This function will get the pointer to the location of Partition Table stored
 in NOR. 

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
flash_partable_t boot_nor_partition_table_location(void);

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_NOR_PARTITION_TABLE_OSBL_INIT

DESCRIPTION
 This function will init pointer to the location of Partition Table stored
 in NOR

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_nor_partition_table_osbl_init(void);

#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_SHARED_NOR_PARTITION_TABLE_H */



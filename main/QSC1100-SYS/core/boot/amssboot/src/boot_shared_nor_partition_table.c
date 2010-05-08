/*===========================================================================

                   N O R   P a r t i t i o n   T a b l e
 
GENERAL DESCRIPTION
  This file contains code that will be used to determine the NOR partition
  table location in NOR flash.
  
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

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_nor_partition_table.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/


#ifdef BUILD_BOOT_CHAIN
#include "osbl_target.h"
#else
#include "target.h"
#endif

#include "boot_shared_seg.h"
#include "boot_shared_nor_partition_table.h"
#include "miparti.h"

#ifdef BUILD_BOOT_CHAIN
#include BOOT_SBL_H
#include <string.h> 
#else
#include "err.h"
#endif

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifdef FEATURE_SHARED_SEGMENT

#define NOR_PARTITION_TABLE_LOCATION_MAGIC_NUM   0xA1B3C5D7 
typedef struct
{
  BOOT_SHARED_SEG_HEADER
  uint32        magic;         /* Magic Number to determine that the data is valid */
  uint32        parti_tbl;     /* Memory to store NOR Partition Table Location     */
} nor_partition_table_type;

#ifndef BUILD_BOOT_CHAIN
/* Config Data's Shared segment in the shared memory */
#pragma arm section zidata = "SHARED_DATA"
nor_partition_table_type      nor_partition_table_shared;
#pragma arm section zidata
#endif

#endif

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

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
void boot_nor_partition_table_osbl_init(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  nor_partition_table_type * nor_partition_tbl_ptr = NULL;
  
  /* Allocate cfg_table_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( nor_partition_table_shared, sizeof(nor_partition_table_type),
                         FALSE, (void**)(&nor_partition_tbl_ptr)
                       );

  if(nor_partition_tbl_ptr == NULL)
    return;

  nor_partition_tbl_ptr->magic = NOR_PARTITION_TABLE_LOCATION_MAGIC_NUM;
  nor_partition_tbl_ptr->parti_tbl = (uint32)osbl_get_nor_partition_table_location();

#endif  
}
#endif

#ifdef BUILD_BOOT_CHAIN
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
flash_partable_t boot_nor_partition_table_location(void)
{
  return ((flash_partable_t)osbl_get_nor_partition_table_location());
}
#else
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
flash_partable_t boot_nor_partition_table_location(void)
{

#ifdef FEATURE_SHARED_SEGMENT
  nor_partition_table_type * nor_partition_tbl_ptr = NULL;

  /* Get the pointer to cfg_table_shared */
  (void)BOOT_SHARED_SEG_GET_DATA( nor_partition_table_shared,
                                  (void**)(&nor_partition_tbl_ptr)
                                );
  
  if ( (nor_partition_tbl_ptr != NULL) &&
       (nor_partition_tbl_ptr->magic == NOR_PARTITION_TABLE_LOCATION_MAGIC_NUM)
     )
  {
    return ((flash_partable_t) nor_partition_tbl_ptr->parti_tbl);
  }

  /*lint -save -e717 */
  ERR_FATAL("NOR Partition Table Location not found in Shared Segment ", 0, 0, 0);  
  /*lint -restore */
#endif
  /* Just to keep compiler happy */
   /*lint -e{527} */
  return NULL;
}
#endif

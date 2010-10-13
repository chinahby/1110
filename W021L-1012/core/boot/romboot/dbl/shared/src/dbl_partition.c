/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_partition.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_comdef.h"
#include "dbl_partition.h"
#include "dbl_error_handler.h"
#include "dbl_target_accessor.h"
#include "dbl_flash.h"
#include "boot_util.h"
#include "string.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_PARTITION_TABLE_SUPPORTED

DESCRIPTION
  Flash specific "find_partition_table" function that finds partition table
  in the given flash only if the the partition table is supported.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_partition_table_supported
(
  dbl_shared_data_type       *shared_data 
)
{
  if (dbl_is_partition_table_supported() == TRUE)
  {
    dbl_flash_find_partition_table(shared_data);
  }

}

/*===========================================================================

FUNCTION  DBL_PARTITION_LOAD_ENTRY

DESCRIPTION
  This function loads partition entry for partition table.

DEPENDENCIES
  This function will update the source field in image header.

RETURN VALUE
  None.
SIDE EFFECTS
  If failed, we will jump to dbl_error_handler to handle errors.

===========================================================================*/
void dbl_partition_load_entry
(
  flash_partable_t                     parti_tbl_ptr,    /* partition table         */
  dbl_flash_partition_image_info_type *shared_parti_ptr, /* image header pointer    */
  char                                *name              /* id to locate the entry 
                                                            of partition table      */
)
{
  uint32 i = 0; /* index for partition table */
  byte * ptr_to_name;

  /* Check the paramenters */
  DBL_VERIFY(parti_tbl_ptr != NULL, (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY(shared_parti_ptr != NULL, (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY(name != NULL, (uint32)DBL_ERR_NULL_PTR);

  /* Making sure that the loading and storing data from stack is at
  ** 32 bit boundary.
  */
  ptr_to_name = (byte *)name;

  /* Need double quotes around 0 and : to make sure
  ** the second argument to bByteCompare is a pointer
  */
  DBL_VERIFY( bByteCompare((uint8 *)ptr_to_name++, (uint8 *)"0", 1) == 0,
              (uint32)DBL_ERR_IMG_NAME_INVALID
            );
  DBL_VERIFY( bByteCompare((uint8 *)ptr_to_name, (uint8 *)":", 1) == 0,
              (uint32)DBL_ERR_IMG_NAME_INVALID
            );

  /* check if partition entry is present.  If not, fail the boot */
  for(i=0; i<FLASH_NUM_PART_ENTRIES;i++)
  {
    if(!strcmp((const char *)parti_tbl_ptr->part_entry[i].name,
               (const char *)name))
    {
      break;
    }
  }/* for */
  
  /* if we cannot find the entry, lets fail the boot */
  DBL_VERIFY (i < FLASH_NUM_PART_ENTRIES, (uint32)DBL_ERR_NO_ENTRY_FOR_IMG );
  
  if (i < FLASH_NUM_PART_ENTRIES)
  {
    shared_parti_ptr->length    = parti_tbl_ptr->part_entry[i].length;
    shared_parti_ptr->src       = parti_tbl_ptr->part_entry[i].offset;
  }

  return;
}/* dbl_load_partition_entry */


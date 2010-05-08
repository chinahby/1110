
/**********************************************************************
 * flash_partition.c
 *
 * Flash partition support
 *
 * This file implements functions and data used to access partitions
 * for flash parts.  These functions are not specific to any flash
 * technology.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH_PARTI_MIB.02.00/flash_partition.c#10 $ $DateTime: 2008/01/25 17:11:46 $ $Author: rtilak $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-01-06   op      Changes to compile with NOR-only driver files
2007-06-06   sb      Compilation changes for Jnand ext i/f for Img updt feature
2006-11-14   rt      Merge changes to compile driver with Windows Mobile
2006-11-09   sb      Removed a lint warning. 
2005-12-28   rt      Add support for user partition table.
2005-12-13   taw     Removed the restriction on the bootloader using strncmp().
2005-03-29   drh     Add some debug statements.
2004-12-12   drh     Comment an unusual usage of a function call.  Add some
                     additional debug statements.
2004-03-26   drh     Add comment header on function.
2004-02-25   drh     Created


===========================================================================*/

#include "comdef.h"
#include "flash_partition.h"
#include "flashi.h"
#include "flash_msm.h"
#ifndef FLASH_COMPILE_NOR_ONLY
  #include "flash_nand.h"
  #include "flash_nand_msm.h"
#endif
#include "mibib.h"
#include "flash_msg.h"
#include <string.h>


/****************************************************************
 * Functions
 ***************************************************************/
 





/***********************************************************************
FUNCTION      flash_partition_name_matches

DESCRIPTION   This function compares a partition enty name to a string
              to find a match.

RETURN VALUE  TRUE             if match found
              FALSE            if match not found 
**********************************************************************/

int
flash_partition_name_matches(flash_partentry_t partentry, char *name)
{
  if (strncmp(partentry->name, name,  FLASH_PART_NAME_LENGTH) == 0)
    return TRUE;
  else
    return FALSE;
}



/***********************************************************************
FUNCTION      flash_usr_partition_name_matches

DESCRIPTION   This function compares a partition enty name to a string
              to find a match.

RETURN VALUE  TRUE             if match found
              FALSE            if match not found 
**********************************************************************/

int
flash_usr_partition_name_matches(flash_usr_partentry_t partentry, char *name)
{
#if !defined(BOOT_LOADER) && !defined(FEATURE_WINCE_BOOTLOADER)
  /* don't use strncmp() if boot loader */
  if (strncmp(partentry->name, name,  FLASH_PART_NAME_LENGTH) == 0)
    return TRUE;
  else
    return FALSE;

#else /* BOOT_LOADER || FEATURE_WINCE_BOOTLOADER*/

/* Boot loader and flash programming tools which define
 * BOOT_LOADER or FEATURE_WINCE_BOOTLOADER should never call this 
 * function.  They open the device and get a view of the entire device
 * and never want to have their view restricted to a partition.  Even if
 * the call to this function would never be made by these entities, we
 * should never make reference to any string library functions as they
 * would pull in code that is not wanted and make the boot loader no longer
 * fit in its assigned size in the flash.  Maybe at some time we will have
 * to implement our own strncmp() if boot loader ever needs to open
 * partition and cannot include string library.
 */
#ifdef BUILD_JNAND_WITH_OEMSBL
/* When this feature is defined BOOTLOADER is calling this function */
  if (memcmp(partentry->name, name,  FLASH_PART_NAME_LENGTH) == 0)
    return TRUE;
  else
  return FALSE;
#else
  return FALSE;
#endif

#endif /* !BOOT_LOADER && !FEATURE_WINCE_BOOTLOADER */
}



/***********************************************************************
FUNCTION      flash_partition_tables_are_equal

DESCRIPTION   Compares two partition tables and sees if they are equal

              Returns TRUE if they are equal
                        
              Returns FALSE if they are not equal
              
**********************************************************************/
int 
flash_partition_tables_are_equal (flash_partable_t parti1, 
                                      flash_partable_t parti2)
{
  uint16 i;

  if ((parti1->magic1 != parti2->magic1) ||
      (parti1->magic2 != parti2->magic2) ||
      (parti1->version != parti2->version) ||
      (parti1->numparts != parti2->numparts))
  {
    return FALSE;
  }

  /* We can use number of partitions from either, since they are equal */
  for (i=0; i<parti1->numparts; i++)
  {
    TPRINTF(4,("fptae:  Comparing %s to %s \n",&parti1->part_entry[i].name,
               &parti2->part_entry[i].name));

    /* This call may look strange, but it is correct.  The function which
     * compares names, uses a partition entry record pointer and a name
     * as arguments.  Here we have to partition tables, so we create a
     * pointer to the name only for one of them.
     */
    if (!flash_partition_name_matches(&parti1->part_entry[i], 
                                      parti2->part_entry[i].name))
    {
      TPRINTF(3,("fptae:  Names do not match\n"));
      return FALSE;
    }
    if ((parti1->part_entry[i].offset != parti2->part_entry[i].offset) ||
        (parti1->part_entry[i].length != parti2->part_entry[i].length) ||
       (parti1->part_entry[i].which_flash != parti2->part_entry[i].which_flash))
    {
      return FALSE;
    }
  }

  /* Every field matches exactly, indicate success. */
  return TRUE;
}





/***********************************************************************
FUNCTION      flash_usr_partition_tables_are_equal

DESCRIPTION   Compares two user partition tables and sees if they are equal

              Returns TRUE if they are equal
                        
              Returns FALSE if they are not equal
              
**********************************************************************/
int 
flash_usr_partition_tables_are_equal (flash_usr_partable_t parti1, 
                                      flash_usr_partable_t parti2)
{
  uint16 i;

  if ((parti1->magic1 != parti2->magic1) ||
      (parti1->magic2 != parti2->magic2) ||
      (parti1->version != parti2->version) ||
      (parti1->numparts != parti2->numparts))
  {
    return FALSE;
  }

  /* We can use number of partitions from either, since they are equal */
  for (i=0; i<parti1->numparts; i++)
  {
    TPRINTF(4,("fuptae:  Comparing %s to %s \n",&parti1->part_entry[i].name,
               &parti2->part_entry[i].name));

    /* This call may look strange, but it is correct.  The function which
     * compares names, uses a partition entry record pointer and a name
     * as arguments.  Here we have to partition tables, so we create a
     * pointer to the name only for one of them.
     */
    if (!flash_usr_partition_name_matches(&parti1->part_entry[i], 
                                          parti2->part_entry[i].name))
    {
      TPRINTF(3,("fuptae:  Names do not match\n"));
      return FALSE;
    }
    if ((parti1->part_entry[i].img_size != parti2->part_entry[i].img_size) ||
        (parti1->part_entry[i].padding != parti2->part_entry[i].padding) ||
      (parti1->part_entry[i].which_flash != parti2->part_entry[i].which_flash))
    {
      return FALSE;
    }
  }

  /* Every field matches exactly, indicate success. */
  return TRUE;
}


/*===========================================================================

FUNCTION flash_partition_find_entry

DESCRIPTION
  This function will attempt to find the named partition in the partition
  table

DEPENDENCIES
  

RETURN VALUE
  If success, return pointer to entry
  If failure, return NULL

SIDE EFFECTS

===========================================================================*/
flash_partentry_t 
flash_partition_find_entry(flash_partable_t parti_ptr, 
                                    char *parti_name)
{
  int entries = parti_ptr->numparts;
  flash_partentry_t entry;
  int i;

  for (i=0; i<entries; i++)
  {
    entry = &parti_ptr->part_entry[i];
    TPRINTF(5,("\nPartition 0x%x ::  \n", i));
    TPRINTF(5,("\tName is %s\n", entry->name));
    TPRINTF(5,("\tStart is 0x%x\n", entry->offset));
    TPRINTF(5,("\tLength is 0x%x\n", entry->length));
    TPRINTF(5,("\tFlash is 0x%x\n", entry->which_flash));

    if (flash_partition_name_matches(entry, parti_name))
    {
      return entry;
    }
  }

  return (flash_partentry_t)0;
}


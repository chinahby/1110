#ifndef __FLASH_PARTITION_H__
#define __FLASH_PARTITION_H__

/**********************************************************************
 * flash_partition.h
 *
 * Flash partition interface
 *
 * This module implements a device-independent view of flash
 * partition handling.
 *
 * Copyright (C) 2004-2005, Qualcomm, Inc.
 *
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH_PARTI_MIB.02.00/flash_partition.h#5 $ $DateTime: 2006/01/30 18:58:42 $ $Author: rtilak $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2005-12-28   rt      Add support for user partition table
2004-11-02   drh     Move partition defines to miparti.h
2004-10-21   drh     Add define to allow partition to grow to part size
2004-09-30   drh     Move partition functions here
2004-02-25   drh     Created

===========================================================================*/


#include "comdef.h"
#include "customer.h"
#include "flash.h"
#include "miparti.h"


/*------------------------------------------------------------
 *
 *  Public Interface to Flash Partition Functions
 *
 * ---------------------------------------------------------*/

int flash_partition_name_matches(flash_partentry_t partentry, char *name);

flash_partentry_t  flash_partition_find_entry(flash_partable_t parti_ptr, 
                                    char *parti_name);

int flash_partition_tables_are_equal (flash_partable_t parti1, 
                                      flash_partable_t parti2);


int flash_usr_partition_tables_are_equal (flash_usr_partable_t parti1,
                                          flash_usr_partable_t parti2);


#endif /* __FLASH_PARTITION_H__ */

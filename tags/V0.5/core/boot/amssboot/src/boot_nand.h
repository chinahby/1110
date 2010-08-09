#ifndef BOOT_NAND_H
#define BOOT_NAND_H

/*===========================================================================
                       boot_info_block.h

DESCRIPTION
  This header file defines various NAND Device parameters and
  Declares a pointer to Nand Device Structure.

Copyright(c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_nand.h#1 $ $DateTime: 2005/05/18 14:00:12 $ $Author: tkuo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/04   ktr     Created.

===========================================================================*/

//#ifndef FIRST_POSSIBLE_BLOCK
#define FIRST_POSSIBLE_BLOCK 1
//#endif

/* NAND device structure */
struct fs_device_data * nand_device = 0;

/* Instance of device structure including private data */
static struct fsi_device_data nand_priv_device;

/* NAND device parameters */
int block_count;        /* number of erasable units in flash */
int pages_in_block;     /* number of pages in block          */
int page_size;          /* number of bytes in a page         */
int eu_size;            /* eraseable unit size               */
int max_pages;          /* number of pages in the device     */
char *flash_name;
block_id current_block;

#endif /* BOOT_NAND_H */
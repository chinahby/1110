#ifndef PROGRESSIVE_BOOT_H
#define PROGRESSIVE_BOOT_H
/*===========================================================================
                      progressive_boot.h

DESCRIPTION
  This header file defines constants and declares various parameters used
  in the progressive boot

Copyright(c) 2004,2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/progressive_boot.h#2 $ $DateTime: 2005/08/08 16:02:33 $ $Author: tkuo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/05   tbg     Fixed/optimized bad block checking
08/14/04   ktr     Created.

===========================================================================*/
#include "miprogressive.h"
#include "pboot_genhash.h"

	#ifndef	NUM_PAGES_IN4K
	#define NUM_PAGES_IN4K 8
	#endif

#ifndef	SIZE4K
#define SIZE4K 4096
#endif

#ifndef	NUM_PAGES_IN4K
#define NUM_PAGES_IN4K 8
#endif

#ifndef	SIZE_4K_MINUS_1
#define SIZE_4K_MINUS_1 (SIZE4K - 1)
#endif


/*===========================================================================
							   EXTERN VARIABLES
 ===========================================================================*/

/* Pointer to the Boot Information Block structure read in bad_block.c */
extern struct progressive_boot_block *prog_hdr;

/* Index of the first paged segment in progressive header */
/* (saves time when we don't care about non paged segments) */
extern uint32 paged_start_index;

/* Index of the first bad block in the bad block list that is after */
/* the start of our parition. */
extern uint32 bb_start_index;
  
/* Pointer to the Bad Block List structure read in the bad_block.c*/
extern struct bad_block_table *ptr_bbtb;

#ifdef SEC_BOOT
  /* Pointer to the hash table */
  extern uint8 *hash_tbl_ptr;
  
  extern uint8 *prog_digest_ptr;
  
  extern boolean prog_auth_enable;
#endif /* SEC_BOOT */
  
/* Number of Pages in Block (bad_block.c)*/
extern int pages_in_block;

/* Size of block in bytes (bad_block.c)*/
extern int eu_size;

/* Size of block in bytes (bad_block.c)*/
extern int page_size;

/* Number of Bits to shift for mult or div with page_size (bad_block.c)*/
extern uint32 page_size_shift_bits;

/* Number of Bits to shift for mult or div with pages_in_block (bad_block.c)*/
extern uint32 pages_in_block_shift_bits;

/* Number of Bits to shift for mult or div with block_size (bad_block.c)*/
extern uint32 block_size_shift_bits;

/* Starting page of AMSS partition */
extern uint32 partition_start_page;  

#define SIZE_PAGE_MINUS_1 (page_size - 1)

/* NAND device structure */
extern struct fs_device_data * nand_device;

extern void boot_error_handler ( void );

#define BOOT_ERR() boot_error_handler();

#endif /* PROGRESSIVE_BOOT_H */




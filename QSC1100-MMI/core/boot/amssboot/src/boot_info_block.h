#ifndef BOOT_INFO_BLOCK_H
#define BOOT_INFO_BLOCK_H

/*===========================================================================
                       boot_info_block.h

DESCRIPTION
  This header file gives the definition of Boot Information Block structure
  used by the Progressive Boot Loader to load NON PAGED Segments from NAND
  to RAM. This Structure is also used by Pager Task to load the PAGED Segment

Copyright(c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800/drivers/boot/2.1/boot_info_block.h#1 $ $DateTime: 2005/06/16 11:45:28 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/04   ktr     Created.

===========================================================================*/

#define BIB_MAGIC1 0x48E3DB5e
#define BIB_MAGIC2 0xEE55AA73
/* We can have as many segments as will fit in a BIB that occupies a single
 * page, but that would be 30, and that is overkill.  For now, we are
 * only planning on two segments, so making the maximum 16 allows us a
 * huge amount of growth room.
 */
		#define BIB_VERSION 3

		#define MAX_SEGMENTS 16

		#define NON_PAGED_SEGMENT  1
		#define PAGED_SEGMENT      2


		/* Definition of record for each loadable segment */
		struct segment_record {
		  uint32 type;
		  uint32 segment_offset; /* offset from first data block in NAND         */
		  uint32 v_address;	     /* Virtual execution address Address in RAM	 */
		  uint32 p_address;      /* Physical execution address Address in RAM	 */
		  uint32 size;		     /* Size in bytes that needs to be copied        */
		};

		/* Definition of Boot Information Block containing segment records */
		struct boot_info_block {
		  uint32          magic1;      /* MAGIC1 */
		  uint32          magic2;      /* MAGIC2 */
		  uint32	      version;     /* BIB version number */
		  uint32	      startaddr;   /* Start address in RAM */
		  uint32          length;      /* length of image */
		  uint32          crc;         /* CRC of image */
		  uint32          numsegments; /* Number of ELF segments */
		  struct segment_record segment[MAX_SEGMENTS];
									   /* One record for each segment */

		};
#endif /* BOOT_INFO_BLOCK_H */

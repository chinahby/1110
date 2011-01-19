/*=============================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                  F l a s h   I n i t i a l i z a t i o n

GENERAL DESCRIPTION
  This module determines the flash type booting from and performs the
  loading of header and image from flash. 

EXTERNALIZED FUNCTIONS
  osbl_prog_boot_save_amss_header

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/osbl/osbl_prog_boot_mproc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Ported from MDM7800.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "osbl_prog_boot_mproc.h"
#include "boot_error_if.h"
#include "miprogressive.h"
#include "smem.h"
#include "target.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================
FUNCTION  OSBL_ALLOCATE_PROG_HDR_IN_SMEM

DESCRIPTION
   This function allocates memory in shared memory to save prog_hdr info

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to allocated memory

SIDE EFFECTS
  None
===========================================================================*/
static progressive_boot_block_type *allocate_prog_hdr_in_smem
(
  boot_elf_loader_if_type *elf_if,
  image_type        image
)
{
  uint32 num_unused_segments = 0;
  progressive_boot_block_type *smem_prog_hdr_ptr = NULL;
  smem_mem_type smem_type = SMEM_MARM_BOOT_SEGMENT_INFO;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  if ( image == AMSS_IMG )
  {
    smem_type = SMEM_MARM_BOOT_SEGMENT_INFO;
  }
  else if ( image == APPS_IMG )
  {
    smem_type = SMEM_AARM_BOOT_SEGMENT_INFO;
  }
  else
  {
    BL_VERIFY( FALSE, BL_ERR_WRG_IMGHD_REQUESTED );
  }

  /* Determine number of unused segment */ 
  num_unused_segments = ( MI_PBT_MAX_SEGMENTS - boot_elf_get_num_segments( elf_if ) );

  /* Dynamically allocate a chunk of shared memory to store the boot segments
   * Copy the segment info to shared memory for use by bootmem in AMSS
   * to page in the segments in L4.
   */
  smem_prog_hdr_ptr = (progressive_boot_block_type *)
                      smem_alloc( smem_type,
                                  ( sizeof( progressive_boot_block_type ) - 
                                    num_unused_segments*sizeof(struct segment_record) ) );

  return smem_prog_hdr_ptr;
} /* allocate_prog_hdr_in_smem() */

/*===========================================================================
FUNCTION  OSBL_SAVE_PROG_HDR_TO_SMEM

DESCRIPTION
   This function saves prog_hdr info to shared memory

DEPENDENCIES
  None
  
RETURN VALUE
  PASS or FAIL

SIDE EFFECTS
  None
===========================================================================*/
static boolean save_prog_hdr_to_smem
( 
  boot_elf_loader_if_type *elf_if,
  progressive_boot_block_type *smem_prog_hdr_ptr
)
{
  uint32 size;
  boolean success = TRUE;
  void *dest;
  const void *source;

  if (elf_if == NULL || smem_prog_hdr_ptr == NULL) 
  {
    success = FALSE;
  }

  smem_prog_hdr_ptr->numsegments = boot_elf_get_num_segments( elf_if );

  size = sizeof(struct segment_record) * smem_prog_hdr_ptr->numsegments;
  dest = smem_prog_hdr_ptr->segment;
  source = boot_elf_get_program_header_ptr( elf_if );

  /* Copy prog_hdr_buff to shared memory */
  memcpy( dest, source, size );

  return success;
} /* save_prog_hdr_to_smem() */

/*===========================================================================

FUNCTION  OSBL_PROG_BOOT_SHARE_AMSS_HEADER

DESCRIPTION
  This function saves the AMSS ELF program header to shared memory

DEPENDENCIES
  None

RETURN VALUE
  TRUE if initialization was successful and input parameters were valid,
  otherwise false.

SIDE EFFECTS
  The provided error handler may be called if an error is detected.

===========================================================================*/
void osbl_prog_boot_share_amss_header
(
  boot_elf_loader_if_type *elf_if,
  image_type        image
)
{
  boolean success = TRUE;
  progressive_boot_block_type *smem_prog_hdr_ptr = NULL;

  BL_VERIFY( elf_if != NULL,
             BL_ERR_NULL_PTR_PASSED );

  /* Allocate memory in shared memory to save prog_hdr_info */
  smem_prog_hdr_ptr = allocate_prog_hdr_in_smem( elf_if, image );
  BL_VERIFY( smem_prog_hdr_ptr != NULL, BL_ERR_FAIL_ALLOCATE_SMEM );

  /* Save prog_hdr_info to shared memory */
  success = save_prog_hdr_to_smem( elf_if, smem_prog_hdr_ptr );
  BL_VERIFY( success, BL_ERR_FAIL_SAVE_TO_SMEM );

} /* osbl_prog_boot_share_amss_header() */


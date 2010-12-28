#ifndef BOOT_ELF_LOADER_PRIV_H
#define BOOT_ELF_LOADER_PRIV_H

/*===========================================================================

               G e n e r i c  E L F  L o a d e r  M o d u l e
                   I n t e r n a l  H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the generic ELF loader module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_elf_loader_priv.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/08   MJS     Add support for boot flash translation layer.
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.
============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miprogressive.h"
#include "boot_clobber_prot_local.h"
#include "boot_elf_loader_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Definition of private ELF loader module data structure */
typedef struct boot_elf_private_type
{
  /* This _MUST_ be the first entry */
  boot_elf_loader_if_type pub_if;

  /*=========================================================================

  FUNCTION POINTER  PRE_LOAD_SEGMENT

  DESCRIPTION
    Performs functionality required prior to loading a segment.

  DEPENDENCIES
    None
  
  RETURN VALUE
    The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_elf_error_type (*pre_load_segment)
  (
    struct boot_elf_private_type *elf_priv,
    uint32 segment,
    const struct segment_record *seg_ptr
  );

  /*=========================================================================

  FUNCTION POINTER  POST_LOAD_SEGMENT

  DESCRIPTION
    Performs functionality required after loading a segment.

  DEPENDENCIES
    None
  
  RETURN VALUE
    The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_elf_error_type (*post_load_segment)
  (
    struct boot_elf_private_type *elf_priv,
    uint32 segment,
    const struct segment_record *seg_ptr
  );

  flash_trans_if_type *trans_if;     /* Pointer to flash translation interface */

  boolean ehdr_is_loaded;             /* Is the ELF header loaded? */
  boolean phdr_is_loaded;             /* Is the ELF program header loaded? */

  boot_elf_error_type last_error;     /* The last error encountered */

  boot_clobber_local_type *clobber_tbl_ptr;  /* (optional) Pointer to 
                                                clobber table */

  uint32 ehdr_reloc_offset;           /* Offset for relocated ELF header */
  uint32 segment_reloc_offset;        /* Offset for relocated ELF segments */
  
  Elf32_Ehdr ehdr;                    /* Memory to load the ELF header */
  
  /* Buffer to store ELF program headers */
  struct segment_record prog_hdr_buf[MI_PBT_MAX_SEGMENTS];

  /* Function pointer for convenience when copying a segment, will either be
     equal to flash_trans_if_type::read or flash_trans_if_type::sloppy_read */
  boolean (*segment_copy)
  (
    struct  flash_trans_if_type *trans_if,
    void   *ram_addr,
    uint32  flash_addr,
    uint32  size
  );  
} boot_elf_private_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

extern void boot_elf_loader_init
(
  boot_elf_private_type *elf_priv,           /* ELF loader private data */
  boot_flash_trans_if_type *trans_if,        /* Pointer to an instance of 
                                                the flash translation module */
  boolean              sloppy_segment_load,  /* Whether or not we can round
                                                segment loads to whole pages
                                                for efficiency */
  uint32               header_offset,        /* Offset of the ELF header from
                                                the base of the flash region */
  uint32               segment_offset        /* Offset of the ELF segments from
                                                the base of the flash region */ 
);

extern void boot_elf_loader_init_generic
(
  boot_elf_private_type *elf_priv,           /* ELF loader private data */
  flash_trans_if_type *trans_if,             /* Pointer to an instance of 
                                                the flash translation module */
  boolean              sloppy_segment_load,  /* Whether or not we can round
                                                segment loads to whole pages
                                                for efficiency */
  uint32               header_offset,        /* Offset of the ELF header from
                                                the base of the flash region */
  uint32               segment_offset        /* Offset of the ELF segments from
                                                the base of the flash region */ 
);

#endif /* BOOT_ELF_LOADER_PRIV_H */


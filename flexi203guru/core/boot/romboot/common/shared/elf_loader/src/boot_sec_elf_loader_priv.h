#ifndef BOOT_SEC_ELF_LOADER_PRIV_H
#define BOOT_SEC_ELF_LOADER_PRIV_H

/*===========================================================================

                           B o o t   L o a d e r
                      E L F  L o a d e r  M o d u l e
                   I n t e r n a l  H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot-specific ELF loader module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_sec_elf_loader_priv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miheader.h"
#include "boot_flash_trans_if.h"
#include "boot_sec_elf_loader_if.h"
#include "boot_elf_loader_priv.h"

#ifdef BOOT_MSM_H
#include BOOT_MSM_H
#endif

#ifndef SIZE4K
#define SIZE4K (4*1024)
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* Definition of private boot ELF loader module data structure */
typedef struct boot_sec_elf_private_type
{
  /* This _MUST_ be the first entry */
  boot_elf_private_type  parent;          /* Generic ELF loader private data */
  
  boot_flash_trans_if_type *trans_if;     /* Pointer to flash interface */

  boolean enable_hash;                    /* Do we hash in nonsecure mode? */
  boolean hash_hdr_is_loaded;             /* Is the hash header loaded? */
  boolean hash_seg_is_loaded;             /* Is the hash segment loaded? */
  boolean entry_point_is_verified;        /* Is the entry point verified? */

  mi_boot_image_header_type *hash_header; /* Pointer to hash header */

  boot_sec_elf_error_type last_error;     /* The last error that occurred */

  
 /* Array of offsets to the expected hash values within hash table */
  const uint8 *expected_hash_ptr[MI_PBT_MAX_SEGMENTS];
  
#ifdef BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT
  /* Buffer to store the first 4K of the ELF header */
  uint8 first_4k_buf[SIZE4K];
#endif /* BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT */

  /* Function pointer to load the hash header (ELF format dependent) */
  boot_sec_elf_error_type (*load_hash_header)
  (
    boot_sec_elf_loader_if_type *sec_elf_if
  );
  
  /* Function pointer to load the hash segment (ELF format dependent) */
  boot_sec_elf_error_type (*load_hash_segment)
  (
   boot_sec_elf_loader_if_type *sec_elf_if
  );
} boot_sec_elf_private_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif /* BOOT_SEC_ELF_LOADER_PRIV_H */


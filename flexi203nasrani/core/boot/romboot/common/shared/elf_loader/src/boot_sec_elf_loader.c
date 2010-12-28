/*=============================================================================

                           B o o t   L o a d e r
                 S e c u r e  E L F  L o a d e r  M o d u l e

GENERAL DESCRIPTION
  This file contains the implementation for the secure boot ELF loader module.

EXTERNALIZED FUNCTIONS
  boot_sec_elf_loader_factory_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 - 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_sec_elf_loader.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/29/08   MJS     Add backwards-compatible support for hash table segment
                   not listed in program headers.
01/04/08   MJS     Reworked for Secure Boot 2.0.
12/02/07   MJS     Allow entry point to be a virtual address.
04/27/07   MJS     Modified to not process segments that have zero size.
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "boot_sec_elf_loader.h"
#include "boot_hash_if.h"
#include "boot_sec_elf_loader_priv.h" 

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define BOOT_SEC_ELF_CHECK_LAST_ERROR() \
  if (elf_priv->last_error != BOOT_ELF_ERROR_NONE ) \
  { \
    return elf_priv->last_error; \
  }

#define BOOT_ELF_CHECK_LAST_ERROR() \
  if (elf_priv->last_error != BOOT_ELF_ERROR_NONE ) \
  { \
    return (boot_elf_error_type)elf_priv->last_error; \
  }

#define BOOT_SEC_ELF_CHECK_LAST_ERROR2(retval) \
  if (elf_priv->last_error != BOOT_ELF_ERROR_NONE ) \
  { \
    return retval; \
  }


/* The secure boot ELF loader factory private data structure */
typedef struct boot_sec_elf_loader_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_sec_elf_loader_factory_if_type pub_if;

  /* Array of the ELF loader private data structures allocated by factory */
  boot_sec_elf_private_type elf_priv[BOOT_SEC_ELF_LOADER_MAX_INSTANCES];

  /* Array indicating which elements of the ELF loader array are in use */
  boolean elf_priv_inuse[BOOT_SEC_ELF_LOADER_MAX_INSTANCES];

} boot_sec_elf_loader_factory_private_type;


/* secure ELF loader factory private data */
static boot_sec_elf_loader_factory_private_type fact_priv;


/* Also keep around a separate copy of the boot ELF loader virtual table 
   so we can call the parent functions if we need to */
static boot_elf_loader_vtbl_type *elf_parent_vtbl = NULL;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION  IS_ZEROS

DESCRIPTION
  Test to check if a region of memory is equal to zero.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the region of memory is set to zero, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static __inline boolean is_zeros( const void *addr, uint32 size )
{
  const uint8 *start_addr = (uint8*)addr;
  const uint8 *end_addr = start_addr + size;

  for ( ; start_addr < end_addr; start_addr++ )
  {
    if ( *start_addr != 0 )
    {
      return FALSE;
    }
  }
  return TRUE;
}


/*=========================================================================

FUNCTION  BOOT_ELF_HASH_SEGMENT_FILTER

DESCRIPTION
  ELF segment callback filter for finding the hash segment.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the segment is the hash segment, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean boot_elf_hash_segment_filter
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return ( MI_PBT_SEGMENT_TYPE_VALUE(flags) == MI_PBT_HASH_SEGMENT );
}  /* boot_elf_hash_segment_filter() */


/*=========================================================================

FUNCTION  BOOT_ELF_COMPUTE_HASH_OFFSETS

DESCRIPTION
  Function to loop through all segments in the ELF file and compute the offset
  into the hash table corresponding to each ELF segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type boot_elf_compute_hash_offsets
(
  boot_sec_elf_private_type *elf_priv, /* Pointer to the private ELF loader data */
  const uint8 *hash_tbl_ptr,       /* Pointer to the hash table */
  const struct segment_record *prog_hdr_ptr /* Pointer to the program headers */
)
{
  /* Set segment record pointer initially equal to start of program headers */
  const struct segment_record *seg_ptr = prog_hdr_ptr;
  uint32 segment, num_segments;
  uint32 hash_tbl_offset;  /* Current offset into the hash table */
  uint32 hash_digest_size;
  
  /* Now go through all the segments and compute the hash table offsets */
  num_segments = elf_priv->parent.ehdr.e_phnum;
  BOOT_SEC_ELF_VERIFY( num_segments < MI_PBT_MAX_SEGMENTS, BOOT_ELF_ERROR_FORMAT );

  /* Get the size of the hash digest for indexing into the hash table */
  hash_digest_size = boot_hash_get_digest_size();

  /* Loop through all the ELF segments */
  for( segment = 0, hash_tbl_offset = 0, seg_ptr += segment;
       segment < num_segments;
       ++segment, ++seg_ptr )
  {
    /* Set the hash offset for the current segment */
    elf_priv->expected_hash_ptr[segment] = hash_tbl_ptr + hash_tbl_offset;

    /* Now update the current hash table offset */
    if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
         MI_PBT_PAGED_SEGMENT )
    {
      /* Compute number of 4K pages by rounding up */
      int num_4k_pages = ( ( (int) seg_ptr->size - 1 ) / SIZE4K ) + 1;

      /* Skip the offsets corresponding to Paged segments */
      if ( seg_ptr->size != 0 )
      {
        hash_tbl_offset += num_4k_pages * hash_digest_size;
      }
    }
    else if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
              MI_PBT_NON_PAGED_SEGMENT )
    {
      /* If the hash table segment has an entry for itself in the hash table,
       * it must be zeros. For compatibility, check if the hash table segment  
       * was added to the ELF file after the hash table was generated and thus
       * there is no entry in the hash table for itself.  In this case, the
       * hash table segment will be the first ELF segment and the first entry
       * in the hash table would not be all zeros.  Don't skip over the 
       * entry in the hash table for the hash table segment if the entry
       * doesn't exist. */
      if ( ( MI_PBT_SEGMENT_TYPE_VALUE(seg_ptr->memory_attribute) != 
             MI_PBT_HASH_SEGMENT ) ||
           ( segment > 0 ) ||
           is_zeros(hash_tbl_ptr + hash_tbl_offset, hash_digest_size) )
      {
        /* Point to hash for the next segment in the table */
        hash_tbl_offset += hash_digest_size;
      }
    }
    else
    {
      /* Fail if the segment is not PAGED or NON_PAGED */
      BOOT_SEC_ELF_VERIFY( NULL, BOOT_ELF_ERROR_FORMAT );
    }
  }

  return elf_priv->last_error;
  
} /* boot_elf_compute_hash_offsets() */


#ifdef BOOT_SEC_ELF_SUPPORT_SMALL_FORMAT
/*=========================================================================

FUNCTION  BOOT_ELF_SMALL_LOAD_HASH

DESCRIPTION
  Function to load the hash segment including the hash header, certificate
  chain and signature for a small segment format boot ELF image.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type boot_elf_small_load_hash
( 
  boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;
  uint8 *hash_tbl_ptr;
  uint32 hash_segment;
  boolean enable_hash_save;

  /* Scan all segments to find the location of the HASH segment. */
  hash_segment = boot_sec_elf_find_next_filtered_segment( sec_elf_if,
                                                          boot_elf_hash_segment_filter,
                                                          ELF_FIND_SEGMENT_START );
  BOOT_SEC_ELF_VERIFY( hash_segment != ELF_FIND_SEGMENT_NOT_FOUND,
                   BOOT_ELF_ERROR_HASH_TABLE_MISSING );
  
  /* Load the hash segment */
  /* Disable hash checking on the hash segment itself */
  enable_hash_save = elf_priv->enable_hash;
  elf_priv->enable_hash = FALSE;
  boot_sec_elf_load_segment( sec_elf_if, hash_segment );
  elf_priv->enable_hash = enable_hash_save;
  BOOT_SEC_ELF_CHECK_LAST_ERROR();

  /* Update the hash header and hash table pointer */
  elf_priv->hash_header = 
    (mi_boot_image_header_type*) elf_priv->parent.prog_hdr_buf[hash_segment].p_address;
  hash_tbl_ptr = (uint8*) elf_priv->hash_header->image_dest_ptr;

  /* Now go through and compute all the hash table offsets for all segments */
  boot_elf_compute_hash_offsets( elf_priv, hash_tbl_ptr, elf_priv->parent.prog_hdr_buf );
  BOOT_SEC_ELF_CHECK_LAST_ERROR();

  /* Update the ELF module hash state */
  elf_priv->hash_hdr_is_loaded = TRUE;
  elf_priv->hash_seg_is_loaded = TRUE;

  return elf_priv->last_error;
} /* boot_elf_small_load_hash() */
#endif /* BOOT_SEC_ELF_SUPPORT_SMALL_FORMAT */

#ifdef BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT
/*=========================================================================

FUNCTION  BOOT_ELF_LARGE_LOAD_HASH_HEADER

DESCRIPTION
  Function to load the hash segment header, certificate chain and signature
  for a large segment format boot ELF image.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type boot_elf_large_load_hash_header
( 
  boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;
  boolean success;

  /* First copy the first 4K of the ELF file to our local buffer */
  success = 
    boot_flash_trans_read( elf_priv->trans_if,
                           elf_priv->first_4k_buf,
                           elf_priv->parent.ehdr_reloc_offset,
                           sizeof(elf_priv->first_4k_buf) );
  BOOT_SEC_ELF_VERIFY( success, BOOT_ELF_ERROR_FLASH );

  /* Update the hash header pointer */
  elf_priv->hash_header = 
    (mi_boot_image_header_type*) ( elf_priv->first_4k_buf + sizeof(Elf32_Ehdr) );

  /* The signature and certificate chain pointers are initially relative to
     the beginning of the ELF image.  Update them to be relative to our
     4K buffer */
  elf_priv->hash_header->cert_chain_ptr += (uint32) elf_priv->first_4k_buf;
  elf_priv->hash_header->signature_ptr += (uint32) elf_priv->first_4k_buf;

  /* Verify that the pointers are still within the bounds of the buffer */
  BOOT_SEC_ELF_VERIFY( ( elf_priv->hash_header->cert_chain_ptr + 
                         elf_priv->hash_header->cert_chain_size ) <=
                       ( elf_priv->first_4k_buf + sizeof(elf_priv->first_4k_buf) ),
                       BOOT_ELF_ERROR_FORMAT );
  BOOT_SEC_ELF_VERIFY( ( elf_priv->hash_header->signature_ptr + 
                         elf_priv->hash_header->signature_size ) <=
                       ( elf_priv->first_4k_buf + sizeof(elf_priv->first_4k_buf) ),
                       BOOT_ELF_ERROR_FORMAT );
  
  /* Update the ELF module hash state */
  elf_priv->hash_hdr_is_loaded = TRUE;

  return elf_priv->last_error;
} /* boot_elf_large_load_hash_header() */

/*=========================================================================

FUNCTION  BOOT_ELF_LARGE_LOAD_HASH_SEGMENT

DESCRIPTION
  Function to load the hash segment in a large segment boot ELF image.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type boot_elf_large_load_hash_segment
(
  boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;
  uint8 *hash_tbl_ptr;
  uint32 hash_segment;
  boolean enable_hash_save;

  /* First make sure the hash header is loaded */
  if ( ! elf_priv->hash_hdr_is_loaded )
  {
    elf_priv->load_hash_header( sec_elf_if );
    BOOT_SEC_ELF_CHECK_LAST_ERROR();
  }

  /* Scan all segments to find the location of the HASH segment. */
  hash_segment = boot_sec_elf_find_next_filtered_segment( sec_elf_if,
                                                          boot_elf_hash_segment_filter,
                                                          ELF_FIND_SEGMENT_START );
  BOOT_SEC_ELF_VERIFY( hash_segment != ELF_FIND_SEGMENT_NOT_FOUND,
                       BOOT_ELF_ERROR_HASH_TABLE_MISSING );
  
  /* Disable hash checking on the hash segment itself */
  enable_hash_save = elf_priv->enable_hash;
  elf_priv->enable_hash = FALSE;
  /* Load the hash segment */
  boot_sec_elf_load_segment( sec_elf_if, hash_segment );
  elf_priv->enable_hash = enable_hash_save;
  BOOT_SEC_ELF_CHECK_LAST_ERROR();

  /* Verify that the hash header matches the hash segment */
  hash_tbl_ptr = (uint8*) elf_priv->hash_header->image_dest_ptr;
  BOOT_SEC_ELF_VERIFY( hash_tbl_ptr == 
                       (uint8*) elf_priv->parent.prog_hdr_buf[hash_segment].p_address,
                       BOOT_ELF_ERROR_FORMAT );
  
  /* Now go through and compute all the hash table offsets for all segments */
  boot_elf_compute_hash_offsets( elf_priv, hash_tbl_ptr, elf_priv->parent.prog_hdr_buf );
  BOOT_SEC_ELF_CHECK_LAST_ERROR();

  /* Update the ELF module hash state */
  elf_priv->hash_seg_is_loaded = TRUE;

  return elf_priv->last_error;
  
} /* boot_elf_large_load_hash_segment() */
#endif /* BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_CERTIFICATE_CHAIN

DESCRIPTION
  Function to retrieve a pointer to the certificate chain and the
  certificate chain size, to use for validating the certificate chain
  without authenticating the hash header.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type sec_elf_get_certificate_chain
(
  boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  boot_sec_elf_cert_info_type *cert_info /* Pointer to the cert info structure */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;
  
  BOOT_SEC_ELF_VERIFY( elf_priv != NULL && cert_info != NULL, 
                       BOOT_ELF_ERROR_NULL_PTR );
  
  /* First make sure the hash header is loaded */
  if ( ! elf_priv->hash_hdr_is_loaded )
  {
    elf_priv->load_hash_header( sec_elf_if );
    BOOT_SEC_ELF_CHECK_LAST_ERROR();
  }

  /* Update the certificate chain location and size */
  cert_info->cert_chain_ptr = elf_priv->hash_header->cert_chain_ptr;
  cert_info->cert_chain_size = elf_priv->hash_header->cert_chain_size;

  return BOOT_ELF_ERROR_NONE;
} /* boot_elf_get_certificate_chain() */


/*=========================================================================

FUNCTION  SEC_ELF_GET_HASH_HEADER

DESCRIPTION
  Function to retrieve a pointer to the multi-image hash header, to use
  for authenticating the ELF image hash segment.

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the hash header.

SIDE EFFECTS
  None

===========================================================================*/
static mi_boot_image_header_type* sec_elf_get_hash_header
(
  boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;

  BOOT_SEC_ELF_VERIFY2( elf_priv != NULL, BOOT_ELF_ERROR_NULL_PTR, NULL );

  /* First make sure the hash segment is loaded */
  if ( ! elf_priv->hash_seg_is_loaded )
  {
    elf_priv->load_hash_segment( sec_elf_if );
    BOOT_SEC_ELF_CHECK_LAST_ERROR2( NULL );
  }

  return elf_priv->hash_header;
} /* boot_elf_get_hash_header() */


/*=========================================================================

FUNCTION  BOOT_ELF_ENABLE_HASH_CHECKING

DESCRIPTION
  Function to enable or disable verifying each segment's hash against the
  hash table when loading the segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_error_type sec_elf_enable_hash_checking
(
  boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  boolean enable                    /* Whether to enable or disable hashing */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) sec_elf_if;

  BOOT_SEC_ELF_VERIFY( elf_priv != NULL, BOOT_ELF_ERROR_NULL_PTR );

  /* Update the hash enable state */
  elf_priv->enable_hash = enable;

  return elf_priv->last_error;
} /* boot_elf_enable_hash_checking() */


/*=========================================================================

FUNCTION  SEC_BOOT_ELF_PRE_LOAD_SEGMENT

DESCRIPTION
  Performs functionality required prior to loading a segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type sec_boot_elf_pre_load_segment
(
  boot_elf_private_type *parent_elf_priv,
  uint32 segment,
  const struct segment_record *seg_ptr
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) parent_elf_priv;
  boolean success;

  BL_VERIFY( ( elf_priv != NULL && seg_ptr != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  if ( ( elf_priv->enable_hash ) && 
       ( seg_ptr->size > 0 ) &&
       ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) ==
         MI_PBT_NON_PAGED_SEGMENT ) )
  {
    /* First make sure the hash segment is loaded if hashing is enabled */
    if ( ! elf_priv->hash_seg_is_loaded )
    {
      elf_priv->load_hash_segment( (boot_sec_elf_loader_if_type*) elf_priv );
     BOOT_ELF_CHECK_LAST_ERROR();
    }

    success = boot_hash_pre_verify_hash( (void*) seg_ptr->p_address,
                                         seg_ptr->size );
    BOOT_ELF_VERIFY( success, (boot_elf_error_type)BOOT_ELF_ERROR_HASH_MISMATCH );
  }

  /* Validate the entire memory range of the segment including BSS. */
  success = boot_clobber_check_local_address_range(
              boot_flash_trans_get_clobber_tbl_ptr( elf_priv->trans_if ),
              (void*) seg_ptr->p_address,
              seg_ptr->mem_size );
  BOOT_ELF_VERIFY( success, ELF_ERROR_FLASH );

  return ELF_ERROR_NONE;
} /* sec_boot_elf_pre_load_segment() */

/*=========================================================================

FUNCTION  SEC_BOOT_ELF_POST_LOAD_SEGMENT

DESCRIPTION
  Performs functionality required after loading a segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type sec_boot_elf_post_load_segment
(
  boot_elf_private_type *parent_elf_priv,
  uint32 segment,
  const struct segment_record *seg_ptr
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) parent_elf_priv;
  boolean success;

  BL_VERIFY( ( elf_priv != NULL && seg_ptr != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  if ( seg_ptr->size > 0 )
  {
    /* Hash the segment if hashing is enabled and we are processing
       a non-paged segment. */
    if ( elf_priv->enable_hash &&
         ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) ==
           MI_PBT_NON_PAGED_SEGMENT ) )
    {
      /* First make sure the hash segment is loaded if hashing is enabled */
      BOOT_ELF_VERIFY( elf_priv->hash_seg_is_loaded,
                  (boot_elf_error_type)BOOT_ELF_ERROR_HASH_TABLE_MISSING );

      success = boot_hash_verify_hash( (void*) seg_ptr->p_address,
                                       seg_ptr->size,
                                       elf_priv->expected_hash_ptr[segment] );
      BOOT_ELF_VERIFY( success, (boot_elf_error_type)BOOT_ELF_ERROR_HASH_MISMATCH );
    }

    /* For efficiency, try to verify the entry point while we load segments */
    if ( ! elf_priv->entry_point_is_verified )
    {
      if ( elf_segment_filter_qc_default( seg_ptr->type,
                                          seg_ptr->memory_attribute ) &&
           ( elf_priv->parent.ehdr.e_entry == seg_ptr->p_address ||
             elf_priv->parent.ehdr.e_entry == seg_ptr->v_address ) )
      {
        /* Make sure that the physical address is used for the entry point */
        elf_priv->parent.ehdr.e_entry = seg_ptr->p_address;
        elf_priv->entry_point_is_verified = TRUE;
      }
    }
  }

  return ELF_ERROR_NONE;
} /* sec_boot_elf_loader_post_load_segment() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_ENTRY_POINT

DESCRIPTION
  Function to return the ELF image entry point.

DEPENDENCIES
  None
  
RETURN VALUE
  The ELF image entry point.

SIDE EFFECTS
  None

===========================================================================*/
static void* sec_elf_get_entry_point
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_sec_elf_private_type *elf_priv = (boot_sec_elf_private_type*) elf_if;
  uint32 segment, num_segments;
  const struct segment_record *seg_ptr;
  void *entry_point;
  boolean success = FALSE;

  BOOT_SEC_ELF_VERIFY2( elf_priv != NULL, BOOT_ELF_ERROR_NULL_PTR, NULL );

  /* First call the boot ELF loader routine to get the entry point
     from the ELF header */
  entry_point = elf_parent_vtbl->get_entry_point( elf_if );

  /* If we have already verified the entry point (when loading the segments)
     then just return it */
  if ( elf_priv->entry_point_is_verified )
  {
    return entry_point;
  }
  
  num_segments = boot_elf_get_num_segments( elf_if );

  /* Get a pointer to the start of the progam header */
  seg_ptr = (struct segment_record*) 
            boot_elf_get_program_header_ptr( elf_if );
  BOOT_SEC_ELF_VERIFY2( seg_ptr != NULL, 
    (boot_sec_elf_error_type) boot_elf_get_last_error( elf_if ), NULL );

  /* Loop through the segments in the ELF image */
  for( segment = 0, seg_ptr += segment;
       segment < num_segments;
       ++segment, ++seg_ptr )
  {
    /* Only check segments that are non-paged and are not the hash segment */
    if( elf_segment_filter_qc_default(seg_ptr->type, seg_ptr->memory_attribute) )
    {
      /* Succeed if the segment start address is equal to the entry point */
      /* Allow entry point to be a virtual address */
      if ( entry_point == (void*) seg_ptr->p_address ||
           entry_point == (void*) seg_ptr->v_address )
      {
        success = TRUE;
        entry_point = (void*) seg_ptr->p_address;
        return entry_point;
      }
    }
  }

  /* Use the success variable just as a compiler workaround because the 
     BOOT_SEC_ELF_VERIFY2 macro may or may not include a return statement, by
     making the macro conditional based on "success", we can include
     a return statement at the end of the function without any warnings. */
  BOOT_SEC_ELF_VERIFY2( success, BOOT_ELF_ERROR_FORMAT, NULL );

  /* This is the default case which means that the entry point was not found */
  return NULL;

} /* boot_elf_get_entry_point() */


/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOADER_INIT

DESCRIPTION
  Function to initialize an instance of the secure boot ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void boot_sec_elf_loader_init
(
  boot_sec_elf_private_type *elf_priv,
  boot_flash_trans_if_type *trans_if,   /* Pointer to an instance of the boot
                                           flash translation module */
  boot_sec_elf_format_type  format,     /* Enumeration value of the ELF format,
                                           large or small */
  uint32                    header_offset,   /* Offset of the ELF header from
                                                the base of the flash region */
  uint32                    segment_offset   /* Offset of the ELF segments from
                                                the base of the flash region */ 
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_SEC_ELF_LOADER_NAME,
    BOOT_MODULE_VERSION( BOOT_SEC_ELF_LOADER_IF_VERSION,
                         BOOT_SEC_ELF_LOADER_VERSION )
  };

  static boolean vtbl_init = FALSE;
  static boot_sec_elf_loader_vtbl_type vtbl =
  {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    sec_elf_enable_hash_checking,
    sec_elf_get_certificate_chain,
    sec_elf_get_hash_header
  };

  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED );

  
  /* Initialize the generic ELF loader module used by the boot ELF loader */
  boot_elf_loader_init( &elf_priv->parent, trans_if,
                        TRUE, header_offset, segment_offset );

  /* Setup the module version structure and virtual function table */
  elf_priv->parent.pub_if.vsn = &vsn;

  /* Fisrt copy the boot elf loader virtual table to our vtable */
  if ( ! vtbl_init )
  {
    /* First save the parent's virtual table */
    elf_parent_vtbl = elf_priv->parent.pub_if.vtbl;
    
    /* Next copy the the parent vtable to our vtable */
    memcpy( &vtbl, elf_parent_vtbl,
            sizeof(*elf_parent_vtbl) );
    
    /* Setup overrides in the parent virtual table */
    vtbl.parent.get_entry_point = sec_elf_get_entry_point;
    
    vtbl_init = TRUE;
  }
  elf_priv->parent.pub_if.vtbl = &vtbl.parent;

  /* Setup the pre- and post-load function pointers in the ELF loader */
  elf_priv->parent.pre_load_segment = sec_boot_elf_pre_load_segment;
  elf_priv->parent.post_load_segment = sec_boot_elf_post_load_segment;
  
  /* Store pointers to the flash translation and hash interfaces */
  elf_priv->trans_if = trans_if;

  /* Hash checking is disabled unless enabled explicitly */
  elf_priv->enable_hash = FALSE;

  /* Reset the module state */
  elf_priv->hash_hdr_is_loaded = FALSE;
  elf_priv->hash_seg_is_loaded = FALSE;
  elf_priv->entry_point_is_verified = FALSE;
  elf_priv->last_error = BOOT_ELF_ERROR_NONE;
  elf_priv->hash_header = NULL;

#ifdef BOOT_SEC_ELF_SUPPORT_SMALL_FORMAT
  if ( format == BOOT_SEC_ELF_FORMAT_SMALL )
  {
    /* If we are using the small segment format, update the function pointers
       accordingly.  The small segment format loads the hash header and the
       hash segment at the same time, so both function pointers are the same. */
    elf_priv->load_hash_header = boot_elf_small_load_hash;
    elf_priv->load_hash_segment = boot_elf_small_load_hash;
  }
  else
#endif /* BOOT_SEC_ELF_SUPPORT_SMALL_FORMAT */
#ifdef BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT
  if ( format == BOOT_SEC_ELF_FORMAT_LARGE )
  {
    /* If we are using the large segment format, update the function pointers
       accordingly.  The large segment format can load the hash header
       and authentication data without loading the actual hash segment,
       so the function pointers below are different. */
    elf_priv->load_hash_header = boot_elf_large_load_hash_header;
    elf_priv->load_hash_segment = boot_elf_large_load_hash_segment;

    /* Set allowable memory overwritable range for first 4K buffer */
    boot_clobber_add_local_hole(
      boot_flash_trans_get_clobber_tbl_ptr( elf_priv->trans_if ),
      elf_priv->first_4k_buf,
      sizeof(elf_priv->first_4k_buf) );
  }
  else
#endif /* BOOT_SEC_ELF_SUPPORT_LARGE_FORMAT */
  {
    /* If we have not chosen a supported ELF format, then indicate failure */
    elf_priv->last_error = BOOT_ELF_ERROR_FORMAT;
  }

} /* boot_sec_elf_loader_init() */

/*=========================================================================

FUNCTION  SEC_ELF_LOADER_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the secure ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
static boot_sec_elf_loader_if_type* sec_elf_loader_factory_open
(
  boot_flash_trans_if_type *trans_if,   /* Pointer to an instance of the boot
                                           flash translation module */
  boot_sec_elf_format_type  format,     /* Enumeration value of the ELF format,
                                           large or small */
  uint32                    header_offset,   /* Offset of the ELF header from
                                                the base of the flash region */
  uint32                    segment_offset   /* Offset of the ELF segments from
                                                the base of the flash region */ 
)
{
  int i;
  boot_sec_elf_private_type *elf_priv = NULL;  /* Pointer to the private
                                                 ELF loader data structure */

  for ( i = 0; i < BOOT_SEC_ELF_LOADER_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.elf_priv_inuse[i] )
    {
      elf_priv = &fact_priv.elf_priv[i];
      fact_priv.elf_priv_inuse[i] = TRUE;
      break;
    }
  }
  BL_VERIFY( elf_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE );

  boot_sec_elf_loader_init( elf_priv, trans_if, format,
                            header_offset, segment_offset );

  return (boot_sec_elf_loader_if_type*) &elf_priv->parent.pub_if;
} /* sec_elf_loader_factory_open() */

/*=========================================================================

FUNCTION  SEC_ELF_LOADER_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the secure ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void sec_elf_loader_factory_close( boot_sec_elf_loader_if_type **sec_elf_if )
{
  int i;

  BL_VERIFY( sec_elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  for ( i = 0; i < BOOT_SEC_ELF_LOADER_MAX_INSTANCES; i++ )
  {
    if ( *sec_elf_if == (boot_sec_elf_loader_if_type*) &fact_priv.elf_priv[i] )
    {
      BL_VERIFY( fact_priv.elf_priv_inuse[i], BL_ERR_OTHERS );
      fact_priv.elf_priv_inuse[i] = FALSE;
      *sec_elf_if = NULL;
      return;
    }
  }

  /* If we have gotten here then the specified ELF loader cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_OTHERS );

} /* sec_elf_loader_factory_close() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOADER_FACTORY_INIT_AND_REGISTER

DESCRIPTION
  Initializes the secure boot ELF loader factory and registers the singleton.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_sec_elf_loader_factory_init_and_register( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_SEC_ELF_LOADER_FACTORY_NAME,
    BOOT_MODULE_VERSION( BOOT_SEC_ELF_LOADER_FACTORY_IF_VERSION,
                         BOOT_SEC_ELF_LOADER_FACTORY_VERSION )
  };
  static boot_sec_elf_loader_factory_vtbl_type vtbl = 
  {
    sec_elf_loader_factory_open,
    sec_elf_loader_factory_close
  };
  
  /* Setup the module version structure and virtual function table */
  fact_priv.pub_if.vsn = &vsn;
  fact_priv.pub_if.vtbl = &vtbl;

  /* Zero out the inuse marker for the private ELF data structure */
  memset( fact_priv.elf_priv_inuse, 0, sizeof(fact_priv.elf_priv_inuse) );

  /* Register the singleton */
  boot_sec_elf_loader_factory_if_register_singleton( &fact_priv.pub_if );
} /* boot_sec_elf_loader_factory_init_and_register() */


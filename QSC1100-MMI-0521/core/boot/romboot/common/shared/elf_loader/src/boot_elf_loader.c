/*=============================================================================

               G e n e r i c  E L F  L o a d e r  M o d u l e

GENERAL DESCRIPTION
  This file contains the implementation for the boot ELF loader module.
  
EXTERNALIZED FUNCTIONS
  boot_elf_loader_init_generic
  boot_elf_loader_init
  boot_elf_loader_factory_init_and_register
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 - 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_elf_loader.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/29/08   MJS     Add support for boot flash translation layer.
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_elf_loader.h"
#include "boot_elf_loader_priv.h"
#include <string.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define BOOT_ELF_CHECK_LAST_ERROR() \
  if (elf_priv->last_error != ELF_ERROR_NONE ) \
  { \
    return elf_priv->last_error; \
  }

#define BOOT_ELF_CHECK_LAST_ERROR2(retval) \
  if (elf_priv->last_error != ELF_ERROR_NONE ) \
  { \
    return retval; \
  }

/* Local enumeration for indexes into the program header structure */
typedef enum
{
  ELF_PHDR_TYPE = 0,
  ELF_PHDR_OFFSET = 1,
  ELF_PHDR_VADDR = 2,
  ELF_PHDR_PADDR = 3,
  ELF_PHDR_FILE_SIZE = 4,
  ELF_PHDR_MEMORY_SIZE = 5,
  ELF_PHDR_FLAGS = 6,
  ELF_PHDR_ALIGN = 7
} elf_phdr_field_type;


/* The boot ELF loader factory private data structure */
typedef struct boot_elf_loader_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_elf_loader_factory_if_type pub_if;

#if (BOOT_ELF_LOADER_MAX_INSTANCES > 0)
  /* Array of the ELF loader private data structures allocated by factory */
  boot_elf_private_type elf_priv[BOOT_ELF_LOADER_MAX_INSTANCES];

  /* Array indicating which elements of the ELF loader array are in use */
  boolean elf_priv_inuse[BOOT_ELF_LOADER_MAX_INSTANCES];
#endif /* (BOOT_ELF_LOADER_MAX_INSTANCES > 0) */

} boot_elf_loader_factory_private_type;


/* ELF loader factory private data */
static boot_elf_loader_factory_private_type fact_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION  ELF_PARSE_HEADER

DESCRIPTION
  Function parse and validate the ELF header.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE: Header processed successfully, FALSE: Error occurred.

SIDE EFFECTS
  None

===========================================================================*/
static boolean elf_parse_header
(
  boot_elf_private_type *elf_priv
)
{
  BL_VERIFY( elf_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Sanity Checks */
  if ( elf_priv->ehdr.e_ident[ELFINFO_MAG0_INDEX] != ELFINFO_MAG0 ||
       elf_priv->ehdr.e_ident[ELFINFO_MAG1_INDEX] != ELFINFO_MAG1 ||
       elf_priv->ehdr.e_ident[ELFINFO_MAG2_INDEX] != ELFINFO_MAG2 ||
       elf_priv->ehdr.e_ident[ELFINFO_MAG3_INDEX] != ELFINFO_MAG3 )
  {
    return FALSE;
  }

  if ( elf_priv->ehdr.e_ident[ELFINFO_CLASS_INDEX] != ELF_CLASS_32 )
  {
    return FALSE;
  }

  if ( elf_priv->ehdr.e_ident[ELFINFO_VERSION_INDEX] != ELF_VERSION_CURRENT )
  {
    return FALSE;
  }

  if ( elf_priv->ehdr.e_ehsize != sizeof(elf_priv->ehdr) )
  {
    return FALSE;
  }

  if ( elf_priv->ehdr.e_phentsize != sizeof(struct segment_record) )
  {
    return FALSE;
  }
  
  return TRUE;
} /* elf_parse_header() */


/*=========================================================================

FUNCTION  ELF_LOAD_HEADER

DESCRIPTION
  Function to load the ELF header from flash and parse the header.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_load_header
(
  boot_elf_private_type *elf_priv
)
{
  boolean success;

  BL_VERIFY( elf_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Copy the ELF header from flash into our local buffer */
  success = flash_trans_read( elf_priv->trans_if,
                              &elf_priv->ehdr,
                              elf_priv->ehdr_reloc_offset,
                              sizeof(elf_priv->ehdr) );
  BOOT_ELF_VERIFY( success, ELF_ERROR_FLASH );

  /* Parse and validate the ELF header */
  success = elf_parse_header( elf_priv );
  BOOT_ELF_VERIFY( success, ELF_ERROR_PARSE );

  /* Update the ELF module state */
  elf_priv->ehdr_is_loaded = TRUE;

  return elf_priv->last_error;
} /* elf_load_header() */

/*=========================================================================

FUNCTION  ELF_LOAD_PROGRAM_HEADER

DESCRIPTION
  Function to load the ELF program header table from flash.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_load_program_header
(
  boot_elf_private_type *elf_priv
)
{
  boolean success;
#if 0
  uint32 segment;
#endif /* 0 */
  
  BL_VERIFY( elf_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF header is loaded */
  if ( ! elf_priv->ehdr_is_loaded )
  {
    elf_load_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR();
  }

  /* Verify that our buffer is large enough for all the segments */
  BOOT_ELF_VERIFY( elf_priv->ehdr.e_phnum <= MI_PBT_MAX_SEGMENTS, ELF_ERROR_FORMAT );

  /* Copy the program header table from flash to our local buffer */
  success = flash_trans_read( elf_priv->trans_if,
                              &elf_priv->prog_hdr_buf,
                              ( elf_priv->ehdr.e_phoff +
                                elf_priv->ehdr_reloc_offset ),
                              ( elf_priv->ehdr.e_phnum *
                                sizeof(struct segment_record) ) );
  BOOT_ELF_VERIFY( success, ELF_ERROR_FLASH );

#if 0 /* This appears broken with Dite on some images */
  /* Validate some data in the non-loadable segments. */
  for ( segment = 0; segment < elf_priv->ehdr.e_phnum; segment++ )
  {
    uint32 seg_type = elf_priv->prog_hdr_buf[segment].type;

    if ( seg_type == ELF_PT_PHDR )
    {
      /* If the program header table contains an entry for the program headers,
         verify that the location and size in the ELF header match the entry
         in the program header table. */
      BOOT_ELF_VERIFY( ( elf_priv->prog_hdr_buf[segment].segment_offset == 
                         elf_priv->ehdr.e_phoff ) &&
                       ( elf_priv->prog_hdr_buf[segment].size == 
                         ( elf_priv->ehdr.e_phnum * sizeof(struct segment_record) ) ),
                       ELF_ERROR_FORMAT );
      break;
    }
    else if ( seg_type == ELF_PT_LOAD )
    {
      /* We can stop searching when we reach the first loadable segment. */
      break;
    }
  }
#endif /* 0 */
  
  /* NOTE: no longer need to rearrange program header fields */
  
  /* Update the ELF module state */
  elf_priv->phdr_is_loaded = TRUE;

  return elf_priv->last_error;
} /* elf_load_program_header() */

/*=========================================================================

FUNCTION  ELF_GET_PROGRAM_HEADER_PTR

DESCRIPTION
  Function to return a pointer to the ELF program header structure.

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the ELF program header structure.

SIDE EFFECTS
  None

===========================================================================*/
static const void* elf_get_program_header_ptr
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF program header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( NULL );
  }

  return elf_priv->prog_hdr_buf;
} /* elf_get_program_header_ptr() */

/*=========================================================================

FUNCTION  ELF_PROCESS_SEGMENT

DESCRIPTION
  Function to validate the load address range and bss range of the given
  ELF segment, and to load the segment.
  
DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_process_segment
(
  boot_elf_private_type *elf_priv,  /* Pointer to the private ELF loader data */
  uint32 segment,
  const struct segment_record *seg_ptr  /* Pointer to the program header entry
                                           corresponding to the segment */
)
{
  uint32 ram_offset, flash_offset, bss_offset;
  int segment_len, bss_len;
  boolean status;

  BL_VERIFY( elf_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Call the private virtual function for pre-segment load processing */
  if ( elf_priv->pre_load_segment )
  {
    elf_priv->pre_load_segment( elf_priv, segment, seg_ptr );
  }

  /* First calculate logical flash offset and bss offset */
  ram_offset = seg_ptr->p_address;
  flash_offset = seg_ptr->segment_offset + elf_priv->segment_reloc_offset;
  segment_len = seg_ptr->size;
  bss_offset = ram_offset + segment_len;
  bss_len = seg_ptr->mem_size - segment_len;

  /* Only process segment if its length is greater than zero */
  if ( segment_len > 0 )
  {
    status = elf_priv->segment_copy( elf_priv->trans_if, (void*) ram_offset,
                                    flash_offset, segment_len );
    BOOT_ELF_VERIFY( status, ELF_ERROR_FLASH );
  } 

  /* If the memory length of the segment is greater than the file length,
     the segment contains BSS data that we need to zero-init. */
  if ( bss_len > 0 )
  {
    BOOT_ELF_BSS_INIT( bss_offset, bss_len );
  }

  /* Call the private virtual function for post-segment load processing */
  if ( elf_priv->post_load_segment )
  {
    elf_priv->post_load_segment( elf_priv, segment, seg_ptr );
  }

  return elf_priv->last_error;
  
} /* elf_process_segment() */


/*=========================================================================

FUNCTION  ELF_PRE_LOAD_SEGMENT

DESCRIPTION
  Performs functionality required prior to loading a segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_pre_load_segment
(
  boot_elf_private_type *elf_priv,
  uint32 segment,
  const struct segment_record *seg_ptr
)
{

  BL_VERIFY( ( elf_priv != NULL && seg_ptr != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  /* Validate the entire memory range of the segment including BSS, only if
   * the clobber table pointer is not NULL. */
  if ( elf_priv->clobber_tbl_ptr != NULL )
  {
    boolean success;

    success = boot_clobber_check_local_address_range( elf_priv->clobber_tbl_ptr,
                                                      (void*) seg_ptr->p_address,
                                                      seg_ptr->mem_size );
    BOOT_ELF_VERIFY( success, ELF_ERROR_FLASH );
  }

  return ELF_ERROR_NONE;
} /* elf_pre_load_segment() */


/*=========================================================================

FUNCTION  ELF_LOAD_SEGMENT

DESCRIPTION
  Function to load a specific segment of the image given the segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_load_segment
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment to load */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF program header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR();
  }

  BOOT_ELF_VERIFY( segment < elf_priv->ehdr.e_phnum, ELF_ERROR_INVAL_PARAM );

  /* Load the segment from flash */
  elf_process_segment( elf_priv, segment, &elf_priv->prog_hdr_buf[segment] );

  return elf_priv->last_error;
} /* elf_load_segment() */


/*=========================================================================

FUNCTION  ELF_LOAD_FILTERED_SEGMENTS

DESCRIPTION
  Function to load all segments from the image that match according
  to the given callback filter.  The callback filter will be passed the
  type and memory_attributes fields of each program header, and if the
  filter returns TRUE then the segment will be loaded.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_load_filtered_segments
(
  boot_elf_loader_if_type          *elf_if,          /* Pointer to the private
                                                  ELF loader data structure */
  elf_segment_filter_func_type segment_filter_func  /* Callback filter */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;
  const struct segment_record *seg_ptr;
  uint32 segment;

  BL_VERIFY( ( elf_if != NULL && segment_filter_func != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF program header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR();
  }

  /* Loop through all the segments */
  for ( segment = 0, seg_ptr = elf_priv->prog_hdr_buf;
        segment < elf_priv->ehdr.e_phnum;
        ++segment, ++seg_ptr )
  {
    /* Use the callback filter to determine if we should load this segment */
    if ( segment_filter_func( seg_ptr->type, seg_ptr->memory_attribute ) )
    {
      /* Load the segment from flash */
      elf_process_segment( elf_priv, segment, seg_ptr );
      BOOT_ELF_CHECK_LAST_ERROR();
    }
  }

  return elf_priv->last_error;
} /* elf_load_filtered_segments() */

/*=========================================================================

FUNCTION  ELF_FIND_SEGMENT_BY_ADDR

DESCRIPTION
  Function to find the segment in the ELF image that contains the physical
  RAM address passed in as a parameter within the memory size of the segment.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment index of the segment that contains the physical offset within
  its memory size, otherwise ELF_FIND_SEGMENT_NOT_FOUND if no segment was
  found.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_find_segment_by_addr
(
  boot_elf_loader_if_type  *elf_if,   /* Pointer to the private ELF loader data */
  void              *phys_addr  /* Physical RAM address to test */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;
  const struct segment_record *seg_ptr;
  uint32 segment;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( ELF_FIND_SEGMENT_NOT_FOUND );
  }

  /* Loop through all the program segments */
  for ( segment = 0, seg_ptr = elf_priv->prog_hdr_buf;
        segment < elf_priv->ehdr.e_phnum;
        ++segment, ++seg_ptr )
  {
    /* If the address is within the bounds of the segment memory size,
       then we have a match */
    if ( ( (uint32) phys_addr >= seg_ptr->p_address ) &&
         ( (uint32) phys_addr < seg_ptr->p_address + seg_ptr->mem_size ) )
    {
      /* Return the segment index of the matching segment */
      return segment;
    }
  }

  /* if we get here then the segment was not found */
  return ELF_FIND_SEGMENT_NOT_FOUND;
} /* elf_find_segment_by_addr() */


/*=========================================================================

FUNCTION  ELF_FIND_NEXT_FILTERED_SEGMENT

DESCRIPTION
  Function to find the next segment in the image that matches according to
  the given callback filter.  The search will start with the segment index 
  that follows the previous_segment index passed in.  To start a new search,
  the special value ELF_FIND_SEGMENT_START should be passed in as the 
  previous_segment value.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment index of the next segment in the ELF image that matches the
  given callback filter.  If the next segment was not found, then 
  ELF_FIND_SEGMENT_NOT_FOUND will be returned.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_find_next_filtered_segment
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  elf_segment_filter_func_type segment_filter_func,  /* Callback filter */
  uint32 previous_segment   /* Start searching after this segment index */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;
  const struct segment_record *seg_ptr;

  /* First verify our input parameters */
  BL_VERIFY( ( elf_if != NULL && segment_filter_func != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF program header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( ELF_FIND_SEGMENT_NOT_FOUND );
  }

  /* First increment the previous segment number to start searching at the
     next segment.  Loop through all the program segments until we find a 
     match or we find that no other segments match. */
  for ( ++previous_segment, seg_ptr = &elf_priv->prog_hdr_buf[previous_segment];
        previous_segment < elf_priv->ehdr.e_phnum;
        ++previous_segment, ++seg_ptr )
  {
    /* Use the callback filter to determine if we have a match. */
    if ( segment_filter_func( seg_ptr->type, seg_ptr->memory_attribute ) )
    {
      /* If we have a match then return the segment index */
      return previous_segment;
    }
  }

  /* If we get here then we did not find another segment matching the filter */
  return ELF_FIND_SEGMENT_NOT_FOUND;
  
} /* elf_find_next_filtered_segment() */


/*=========================================================================

FUNCTION  ELF_GET_HEADER

DESCRIPTION
  Function to retrieve a pointer to the ELF header.  This routine should
  only be used if it is absolutely neccessary to reference the ELF internal
  header and it is not possible to use an existing interface function.

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the ELF header.

SIDE EFFECTS
  None

===========================================================================*/
static const Elf32_Ehdr* elf_get_header
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF header is loaded */
  if ( ! elf_priv->ehdr_is_loaded )
  {
    elf_load_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( NULL );
  }

  return &elf_priv->ehdr;
} /* elf_get_header() */


/*=========================================================================

FUNCTION  ELF_GET_PROGRAM_HEADER_FIELD

DESCRIPTION
  Function to retrieve the given field from the program header for the
  given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The value of the given field for the given segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_program_header_field
(
  boot_elf_loader_if_type *elf_if,
  uint32 segment,
  elf_phdr_field_type field_index
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF program header is loaded */
  if ( ! elf_priv->phdr_is_loaded )
  {
    elf_load_program_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( 0 );
  }

  BOOT_ELF_VERIFY2( segment <= elf_priv->ehdr.e_phnum, ELF_ERROR_INVAL_PARAM, 0 );

  return ((uint32*)&elf_priv->prog_hdr_buf[segment])[(uint32)field_index];
} /* elf_get_program_header_field() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_TYPE

DESCRIPTION
  Function to return the segment type field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment type field for the program header corresponding to the given
  segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_type
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_TYPE );
} /* elf_get_segment_type() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_OFFSET

DESCRIPTION
  Function to return the segment offset field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment offset field for the program header corresponding to the given
  segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_offset
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_OFFSET );
} /* elf_get_segment_offset() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_VADDR

DESCRIPTION
  Function to return the segment virtual address field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment virtual address field for the program header corresponding 
  to the given segment index.

SIDE EFFECTS
  None

===========================================================================*/
static void* elf_get_segment_vaddr
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return (void*) elf_get_program_header_field( elf_if, segment, ELF_PHDR_VADDR );
} /* elf_get_segment_vaddr() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_PADDR

DESCRIPTION
  Function to return the segment physical address field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment physical address field for the program header corresponding
  to the given segment index.

SIDE EFFECTS
  None

===========================================================================*/
static void* elf_get_segment_paddr
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return (void*) elf_get_program_header_field( elf_if, segment, ELF_PHDR_PADDR );
} /* elf_get_segment_paddr() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_FILE_SIZE

DESCRIPTION
  Function to return the segment file size field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment file size field for the program header corresponding to the given
  segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_file_size
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_FILE_SIZE );
} /* elf_get_segment_file_size() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_MEMORY_SIZE

DESCRIPTION
  Function to return the segment memory size field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment memory size field for the program header corresponding
  to the given segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_memory_size
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_MEMORY_SIZE );
} /* elf_get_segment_memory_size() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_FLAGS

DESCRIPTION
  Function to return the segment flags field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment flags field for the program header corresponding to the given
  segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_flags
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_FLAGS );
} /* elf_get_segment_flags() */

/*=========================================================================

FUNCTION  ELF_GET_SEGMENT_ALIGN

DESCRIPTION
  Function to return the segment alignment field for the program header 
  corresponding to the given segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The segment alignment field for the program header corresponding to the given
  segment index.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_segment_align
(
  boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  /* NOTE: error checking will be done by elf_get_program_header_field() */
  return elf_get_program_header_field( elf_if, segment, ELF_PHDR_ALIGN );
} /* elf_get_segment_align() */


/*=========================================================================

FUNCTION  ELF_GET_NUM_SEGMENTS

DESCRIPTION
  Function to return the number of program segments in the ELF image.

DEPENDENCIES
  None
  
RETURN VALUE
  The number of program segments in the ELF image.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 elf_get_num_segments
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF header is loaded */
  if ( ! elf_priv->ehdr_is_loaded )
  {
    elf_load_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( 0 );
  }

  return elf_priv->ehdr.e_phnum;
} /* elf_get_num_segments() */


/*=========================================================================

FUNCTION  ELF_GET_ENTRY_POINT

DESCRIPTION
  Function to return the ELF image entry point.

DEPENDENCIES
  None
  
RETURN VALUE
  The ELF image entry point.

SIDE EFFECTS
  None

===========================================================================*/
static void * elf_get_entry_point
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* First make sure that the ELF header is loaded */
  if ( ! elf_priv->ehdr_is_loaded )
  {
    elf_load_header( elf_priv );
    BOOT_ELF_CHECK_LAST_ERROR2( NULL );
  }

  return (void*) elf_priv->ehdr.e_entry;
} /* elf_get_entry_point() */


/*=========================================================================

FUNCTION  ELF_GET_LAST_ERROR

DESCRIPTION
  Function to return the enumeration value of the last error that occurred
  within the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  The enumeration value of the last error that occurred.

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_error_type elf_get_last_error
(
  boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  boot_elf_private_type *elf_priv = (boot_elf_private_type*) elf_if;

  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  return elf_priv->last_error;

} /* elf_get_last_error() */


/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_INIT_GENERIC

DESCRIPTION
  Function to initialize an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_elf_loader_init_generic
(
  boot_elf_private_type *elf_priv,           /* Pointer to ELF private type */
  flash_trans_if_type *trans_if,             /* Pointer to an instance of 
                                                the flash translation module */
  boolean              sloppy_segment_load,  /* Whether or not we can round
                                                segment loads to whole pages
                                                for efficiency */
  uint32               header_offset,        /* Offset of the ELF header from
                                                the base of the flash region */
  uint32               segment_offset        /* Offset of the ELF segments from
                                                the base of the flash region */ 
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_ELF_LOADER_NAME,
    BOOT_MODULE_VERSION( BOOT_ELF_LOADER_IF_VERSION, BOOT_ELF_LOADER_VERSION )
  };
  static boot_elf_loader_vtbl_type vtbl =
  {
    elf_load_segment,
    elf_load_filtered_segments,
    elf_find_next_filtered_segment,
    elf_find_segment_by_addr,
    elf_get_num_segments,
    elf_get_header,
    elf_get_segment_type,
    elf_get_segment_offset,
    elf_get_segment_vaddr,
    elf_get_segment_paddr,
    elf_get_segment_file_size,
    elf_get_segment_memory_size,
    elf_get_segment_flags,
    elf_get_segment_align,
    elf_get_program_header_ptr,
    elf_get_entry_point,
    elf_get_last_error
  };

  BL_VERIFY( elf_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  elf_priv->pub_if.vsn = &vsn;
  elf_priv->pub_if.vtbl = &vtbl;

  /* Store a pointer to the flash interface */
  elf_priv->trans_if = trans_if;

  /* Private interface function pointers */
  elf_priv->pre_load_segment = elf_pre_load_segment;
  elf_priv->post_load_segment = NULL;

  /* Reset the state variables */
  elf_priv->ehdr_is_loaded = FALSE;
  elf_priv->phdr_is_loaded = FALSE;
  elf_priv->last_error = ELF_ERROR_NONE;
  elf_priv->clobber_tbl_ptr = NULL;

  /* Store the relocation offsets */
  elf_priv->ehdr_reloc_offset = header_offset;
  elf_priv->segment_reloc_offset = segment_offset;

  /* Set the function to use to copy segments */
  if ( sloppy_segment_load )
  {
    elf_priv->segment_copy = trans_if->vtbl->sloppy_read;
  }
  else
  {
    elf_priv->segment_copy = trans_if->vtbl->read;
  }
} /* boot_elf_loader_init_generic() */

/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_INIT

DESCRIPTION
  Function to initialize an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_elf_loader_init
(
  boot_elf_private_type    *elf_priv,             /* Pointer to ELF private type */
  boot_flash_trans_if_type *trans_if,             /* Pointer to an instance of 
                                                     the flash translation module */
  boolean                   sloppy_segment_load,  /* Whether or not we can round
                                                     segment loads to whole pages
                                                     for efficiency */
  uint32                    header_offset,        /* Offset of the ELF header from
                                                     the base of the flash region */
  uint32                    segment_offset        /* Offset of the ELF segments from
                                                     the base of the flash region */ 
)
{
  boot_elf_loader_init_generic( elf_priv,
                                boot_flash_trans_cast_parent_if( trans_if ),
                                sloppy_segment_load,
                                header_offset,
                                segment_offset );

  /* Store pointer to the local clobber table */
  elf_priv->clobber_tbl_ptr = boot_flash_trans_get_clobber_tbl_ptr( trans_if );

  /* Set allowable memory overwriteable ranges for ELF loader */
  boot_clobber_add_local_hole( elf_priv->clobber_tbl_ptr,
                               &elf_priv->ehdr,
                               sizeof(elf_priv->ehdr) );
  boot_clobber_add_local_hole( elf_priv->clobber_tbl_ptr,
                               &elf_priv->prog_hdr_buf,
                               sizeof(elf_priv->prog_hdr_buf) );
  
} /* boot_elf_loader_init() */

/*=========================================================================

FUNCTION  ELF_LOADER_FACTORY_ALLOCATE

DESCRIPTION
  Allocates a new instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the new ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_private_type* elf_loader_factory_allocate( void )
{
  boot_elf_private_type *elf_priv = NULL;  /* Pointer to the private
                                              ELF loader data structure */

#if (BOOT_ELF_LOADER_MAX_INSTANCES > 0)
  int i;

  for ( i = 0; i < BOOT_ELF_LOADER_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.elf_priv_inuse[i] )
    {
      elf_priv = &fact_priv.elf_priv[i];
      fact_priv.elf_priv_inuse[i] = TRUE;
      break;
    }
  }
#endif /* (BOOT_ELF_LOADER_MAX_INSTANCES > 0) */

  return elf_priv;
}

/*=========================================================================

FUNCTION  ELF_LOADER_FACTORY_OPEN_GENERIC

DESCRIPTION
  Requests the factory to open an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_loader_if_type* elf_loader_factory_open_generic
(
  flash_trans_if_type *trans_if,             /* Pointer to an instance of 
                                                the flash translation module */
  boolean              sloppy_segment_load,  /* Whether or not we can round
                                                segment loads to whole pages
                                                for efficiency */
  uint32               header_offset,        /* Offset of the ELF header from
                                                the base of the flash region */
  uint32               segment_offset        /* Offset of the ELF segments from
                                                the base of the flash region */ 
)
{
  boot_elf_private_type    *elf_priv = NULL;  /* Pointer to the private
                                                 ELF loader data structure */

  elf_priv = elf_loader_factory_allocate();
  
  BL_VERIFY( elf_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE );

  boot_elf_loader_init_generic( elf_priv, trans_if, sloppy_segment_load,
                                header_offset, segment_offset );

  return &elf_priv->pub_if;

} /* elf_loader_factory_open_generic() */

/*=========================================================================

FUNCTION  ELF_LOADER_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
static boot_elf_loader_if_type* elf_loader_factory_open
(
  boot_flash_trans_if_type *trans_if,             /* Pointer to an instance of 
                                                     the flash translation module */
  boolean                   sloppy_segment_load,  /* Whether or not we can round
                                                    segment loads to whole pages
                                                     for efficiency */
  uint32                    header_offset,        /* Offset of the ELF header from
                                                     the base of the flash region */
  uint32                    segment_offset        /* Offset of the ELF segments from
                                                     the base of the flash region */ 
)
{
  boot_elf_private_type    *elf_priv = NULL;  /* Pointer to the private
                                                 ELF loader data structure */

  elf_priv = elf_loader_factory_allocate();

  BL_VERIFY( elf_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE );

  boot_elf_loader_init( elf_priv, trans_if, sloppy_segment_load,
                        header_offset, segment_offset );

  return &elf_priv->pub_if;

} /* elf_loader_factory_open() */

/*=========================================================================

FUNCTION  ELF_LOADER_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void elf_loader_factory_close( boot_elf_loader_if_type **elf_if )
{
#if (BOOT_ELF_LOADER_MAX_INSTANCES > 0)
  int i;
  
  BL_VERIFY( elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

  for ( i = 0; i < BOOT_ELF_LOADER_MAX_INSTANCES; i++ )
  {
    if ( *elf_if == &fact_priv.elf_priv[i].pub_if )
    {
      BL_VERIFY( fact_priv.elf_priv_inuse[i], BL_ERR_OTHERS );
      fact_priv.elf_priv_inuse[i] = FALSE;
      *elf_if = NULL;
      return;
    }
  }
#endif /* (BOOT_ELF_LOADER_MAX_INSTANCES > 0) */

  /* If we have gotten here then the specified ELF loader cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_OTHERS );

} /* elf_loader_factory_close() */


/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_FACTORY_INIT_AND_REGISTER

DESCRIPTION
  Initializes the boot ELF loader factory and registers the singleton.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_elf_loader_factory_init_and_register( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_ELF_LOADER_FACTORY_NAME,
    BOOT_MODULE_VERSION( BOOT_ELF_LOADER_FACTORY_IF_VERSION,
                         BOOT_ELF_LOADER_FACTORY_VERSION )
  };
  static boot_elf_loader_factory_vtbl_type vtbl = 
  {
    elf_loader_factory_open,
    elf_loader_factory_open_generic,
    elf_loader_factory_close
  };
  
  /* Setup the module version structure and virtual function table */
  fact_priv.pub_if.vsn = &vsn;
  fact_priv.pub_if.vtbl = &vtbl;

#if (BOOT_ELF_LOADER_MAX_INSTANCES > 0)
  /* Zero out the inuse marker for the private ELF data structure */
  memset( fact_priv.elf_priv_inuse, 0, sizeof(fact_priv.elf_priv_inuse) );
#endif /* (BOOT_ELF_LOADER_MAX_INSTANCES > 0) */

  /* Register the singleton */
  boot_elf_loader_factory_if_register_singleton( &fact_priv.pub_if );

} /* boot_elf_loader_factory_init_and_register() */



#ifndef BOOT_SEC_ELF_LOADER_IF_H
#define BOOT_SEC_ELF_LOADER_IF_H

/*===========================================================================

                           B o o t   L o a d e r
            S e c u r e  E L F  L o a d e r  I n t e r f a c e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions and declarations for the 
  secure boot ELF loader interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_sec_elf_loader_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miheader.h"
#include "boot_flash_trans_if.h"
#include "boot_elf_loader_if.h"

#ifdef BOOT_MSM_H
#include BOOT_MSM_H
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_SEC_ELF_LOADER_IF_VERSION 1
#define BOOT_SEC_ELF_LOADER_FACTORY_IF_VERSION 1


/* Enumeration of potential errors returned from ELF loader module */
typedef enum
{
  /* Before editing the enumeration, ensure that the first group of
     enumerations match what is in boot_elf_error_type */
  BOOT_ELF_ERROR_NONE,
  BOOT_ELF_ERROR_NULL_PTR,
  BOOT_ELF_ERROR_INVAL_PARAM,
  BOOT_ELF_ERROR_FLASH,
  BOOT_ELF_ERROR_PARSE,
  BOOT_ELF_ERROR_FORMAT,
  /* END enumerations that mirror boot_elf_error_type */
  BOOT_ELF_ERROR_HASH_TABLE_MISSING,
  BOOT_ELF_ERROR_HASH_MISMATCH,
  BOOT_ELF_ERROR_ENUM_SIZE = 0x7FFFFFFF
} boot_sec_elf_error_type;

/* Enumeration of current boot ELF formats */
typedef enum
{
  BOOT_SEC_ELF_FORMAT_SMALL,
  BOOT_SEC_ELF_FORMAT_LARGE,
  BOOT_SEC_ELF_FORMAT_ENUM_SIZE = 0x7FFFFFFF
} boot_sec_elf_format_type;

/* Structure defining certificate data */
typedef struct boot_sec_elf_cert_info_type
{
  void *cert_chain_ptr;      /* Pointer to the chain of attestation
                                certificates associated with the image. */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */
} boot_sec_elf_cert_info_type;


/* Forward declarations */
struct boot_sec_elf_loader_if_type;


/* Definition of the secure boot ELF loader module virtual table structure */
typedef struct boot_sec_elf_loader_vtbl_type
{
  /* This _MUST_ be the first entry */
  /* The parent module virtual table */
  boot_elf_loader_vtbl_type parent;

  /*=========================================================================

  FUNCTION  ENABLE_HASH_CHECKING

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
  boot_sec_elf_error_type (*enable_hash_checking)
  (
    struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
    boolean enable                    /* Whether to enable or disable hashing */
  );

  /*=========================================================================

  FUNCTION  GET_CERTIFICATE_CHAIN

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
  boot_sec_elf_error_type (*get_certificate_chain)
  (
    struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
    boot_sec_elf_cert_info_type *cert_info /* Pointer to the cert info structure */
  );

  /*=========================================================================

  FUNCTION  GET_HASH_HEADER

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
  mi_boot_image_header_type* (*get_hash_header)
  (
    struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
  );

} boot_sec_elf_loader_vtbl_type;


/* Definition of public interface to the secure boot ELF loader module */
typedef struct boot_sec_elf_loader_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_sec_elf_loader_vtbl_type *vtbl;

} boot_sec_elf_loader_if_type;


/* Definition of the secure boot ELF loader factory virtual table structure */
typedef struct boot_sec_elf_loader_factory_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  OPEN

  DESCRIPTION
    Requests the factory to open an instance of the ELF loader module.

  DEPENDENCIES
    None

  RETURN VALUE
    Pointer to the opened ELF loader interface

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_sec_elf_loader_if_type* (*open)
  (
    boot_flash_trans_if_type  *trans_if,       /* Pointer to an instance of 
                                                  the flash translation module */
    boot_sec_elf_format_type   format,         /* Enumeration value of the ELF
                                                  format, large or small */
    uint32                     header_offset,  /* Offset of the ELF header from
                                                  the base of the flash region */
    uint32                     segment_offset  /* Offset of the ELF segments from
                                                  the base of the flash region */ 
  );

  /*=========================================================================

  FUNCTION POINTER  CLOSE

 DESCRIPTION
    Requests the factory to close an instance of the ELF loader module.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*close)
  (
    boot_sec_elf_loader_if_type **p_sec_elf_if
  );
} boot_sec_elf_loader_factory_vtbl_type;


/* Definition of public interface to the secure boot ELF loader factory module */
typedef struct boot_sec_elf_loader_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_sec_elf_loader_factory_vtbl_type *vtbl;

} boot_sec_elf_loader_factory_if_type;



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Declare the singleton, do it here so we can access the singleton below */
BOOT_SINGLETON_DECLARE(boot_sec_elf_loader_factory_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOADER_CAST_PARENT_IF

 DESCRIPTION
  Takes a pointer to the secure boot ELF loader interface and casts to the
  parent interface type (boot ELF loader interface).

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to a boot ELF interface

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_loader_if_type* boot_sec_elf_loader_cast_parent_if
(
  boot_sec_elf_loader_if_type *sec_elf_if
)
{
  return (boot_elf_loader_if_type*) sec_elf_if;
} /* boot_sec_elf_loader_cast_parent_if() */


/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOAD_SEGMENT

 DESCRIPTION
  Function to load a specific segment of the image given the segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_error_type boot_sec_elf_load_segment
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment to load */
)
{
  return sec_elf_if->vtbl->parent.load_segment( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_load_segment() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOAD_FILTERED_SEGMENTS

DESCRIPTION
  Function to load all segments from the image that match according
  to the given callback filter.  The callback filter will be passed the
  type and memory_attributes fields of each program header, and if the
  filter returns TRUE then the segment will be loaded.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise BOOT_ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_error_type boot_sec_elf_load_filtered_segments
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,    /* Pointer to the private
                                                ELF loader data structure */
  elf_segment_filter_func_type segment_filter_func/* Callback filter */
)
{
  return sec_elf_if->vtbl->parent.load_filtered_segments( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment_filter_func );
} /* boot_sec_elf_load_filtered_segments() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_FIND_NEXT_FILTERED_SEGMENT

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
__inline uint32 boot_sec_elf_find_next_filtered_segment
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  elf_segment_filter_func_type segment_filter_func,  /* Callback filter */
  uint32 previous_segment   /* Start searching after this segment index */
)
{
  return sec_elf_if->vtbl->parent.find_next_filtered_segment( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ),
           segment_filter_func, previous_segment );
} /* boot_sec_elf_find_next_filtered_segment() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_FIND_SEGMENT_BY_ADDR

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
__inline uint32 boot_sec_elf_find_segment_by_addr
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,   /* Pointer to the private ELF loader data */
  void              *phys_addr  /* Physical RAM address to test */
)
{
  return sec_elf_if->vtbl->parent.find_segment_by_addr( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), phys_addr );
} /* boot_sec_elf_find_segment_by_addr() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_NUM_SEGMENTS

DESCRIPTION
  Function to return the number of program segments in the ELF image.

DEPENDENCIES
  None
  
RETURN VALUE
  The number of program segments in the ELF image.

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_sec_elf_get_num_segments
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->parent.get_num_segments( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ) );
} /* boot_sec_elf_get_num_segments() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_HEADER

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
__inline const Elf32_Ehdr* boot_sec_elf_get_header
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->parent.get_header( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ) );
} /* boot_sec_elf_get_header() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_TYPE

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
__inline uint32 boot_sec_elf_get_segment_type
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_type(
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_type() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_OFFSET

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
__inline uint32 boot_sec_elf_get_segment_offset
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_offset( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_offset() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_VADDR

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
__inline void* boot_sec_elf_get_segment_vaddr
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_vaddr( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_vaddr() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_PADDR

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
__inline void* boot_sec_elf_get_segment_paddr
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_paddr( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_paddr() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_FILE_SIZE

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
__inline uint32 boot_sec_elf_get_segment_file_size
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_file_size(
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_file_size() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_MEMORY_SIZE

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
__inline uint32 boot_sec_elf_get_segment_memory_size
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_memory_size(
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_memory_size() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_FLAGS

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
__inline uint32 boot_sec_elf_get_segment_flags
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_flags( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_flags() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_SEGMENT_ALIGN

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
__inline uint32 boot_sec_elf_get_segment_align
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return sec_elf_if->vtbl->parent.get_segment_align( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ), segment );
} /* boot_sec_elf_get_segment_align() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_PROGRAM_HEADER_PTR

DESCRIPTION
  Function to return a pointer to the ELF program header structure.

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the ELF program header structure.

SIDE EFFECTS
  None

===========================================================================*/
__inline const void* boot_sec_elf_get_program_header_ptr
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->parent.get_program_header_ptr(
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ) );
} /* boot_sec_elf_get_program_header_ptr() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_ENTRY_POINT

DESCRIPTION
  Function to return the ELF image entry point.

DEPENDENCIES
  None

RETURN VALUE
  The ELF image entry point.

SIDE EFFECTS
  None

===========================================================================*/
__inline void* boot_sec_elf_get_entry_point
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->parent.get_entry_point( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ) );
} /* boot_sec_elf_get_entry_point() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_LAST_ERROR

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
__inline boot_elf_error_type boot_sec_elf_get_last_error
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->parent.get_last_error( 
           boot_sec_elf_loader_cast_parent_if( sec_elf_if ) );
} /* boot_sec_elf_get_last_error() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_ENABLE_HASH_CHECKING

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
__inline boot_sec_elf_error_type boot_sec_elf_enable_hash_checking
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  boolean enable                    /* Whether to enable or disable hashing */
)
{
  return sec_elf_if->vtbl->enable_hash_checking( sec_elf_if, enable );
} /* boot_sec_elf_enable_hash_checking() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_CERTIFICATE_CHAIN

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
__inline boot_sec_elf_error_type boot_sec_elf_get_certificate_chain
(
  struct boot_sec_elf_loader_if_type *sec_elf_if,  /* Pointer to the private ELF loader data */
  boot_sec_elf_cert_info_type *cert_info /* Pointer to the cert info structure */
)
{
  return sec_elf_if->vtbl->get_certificate_chain( sec_elf_if, cert_info );
} /* boot_sec_elf_get_certificate_chain() */

/*=========================================================================

FUNCTION  BOOT_SEC_ELF_GET_HASH_HEADER

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
__inline mi_boot_image_header_type* boot_sec_elf_get_hash_header
(
  struct boot_sec_elf_loader_if_type *sec_elf_if  /* Pointer to the private ELF loader data */
)
{
  return sec_elf_if->vtbl->get_hash_header( sec_elf_if );
} /* boot_sec_elf_get_hash_header() */


/*=========================================================================

FUNCTION  BOOT_SEC_ELF_LOADER_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_sec_elf_loader_if_type* boot_sec_elf_loader_factory_open
(
  boot_flash_trans_if_type  *trans_if,       /* Pointer to an instance of 
                                                the flash translation module */
  boot_sec_elf_format_type   format,         /* Enumeration value of the ELF
                                                format, large or small */
  uint32                     header_offset,  /* Offset of the ELF header from
                                                the base of the flash region */
  uint32                     segment_offset  /* Offset of the ELF segments from
                                                the base of the flash region */ 
)
{
  return boot_sec_elf_loader_factory_if_get_singleton()->vtbl->open(
           trans_if, format, header_offset, segment_offset );
} /* boot_sec_elf_loader_factory_open() */

/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_sec_elf_loader_factory_close
(
  boot_sec_elf_loader_if_type **p_sec_elf_if
)
{
  boot_sec_elf_loader_factory_if_get_singleton()->vtbl->close( p_sec_elf_if );
} /* boot_sec_elf_loader_factory_close() */

/*=========================================================================

FUNCTION  ELF_SEGMENT_FILTER_QC_DEFAULT

DESCRIPTION
  This is the default Qualcomm-specific ELF segment callback filter.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the segment flags indicate the segment is non-paged, not the
  hash segment, not shared and not unused, otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean elf_segment_filter_qc_default
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
);

#endif /* BOOT_SEC_ELF_LOADER_IF_H */


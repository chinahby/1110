#ifndef BOOT_ELF_LOADER_IF_H
#define BOOT_ELF_LOADER_IF_H

/*===========================================================================

                           B o o t   L o a d e r
             G e n e r i c  E L F  L o a d e r  I n t e r f a c e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions and declarations for the
  ELF loader interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_elf_loader_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/08   MJS     Add support for boot flash translation layer.
01/04/08   MJS     Initial revision.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"
#include "boot_singleton.h"
#include "boot_flash_trans_if.h"
#include "miprogressive.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_ELF_LOADER_IF_VERSION 1
#define BOOT_ELF_LOADER_FACTORY_IF_VERSION 1


/* Defines used with the ELF find segment routines */
#define ELF_FIND_SEGMENT_START      0xFFFFFFFF
#define ELF_FIND_SEGMENT_NOT_FOUND  0xFFFFFFFE

/* Standard ELF segment type definitions */
#define ELF_PT_NULL    0
#define ELF_PT_LOAD    1
#define ELF_PT_DYNAMIC 2
#define ELF_PT_INTERP  3
#define ELF_PT_NOTE    4
#define ELF_PT_SHLIB   5
#define ELF_PT_PHDR    6
#define ELF_TLS        7

/* Enumeration of potential errors returned from ELF loader module */
typedef enum
{
  ELF_ERROR_NONE,
  ELF_ERROR_NULL_PTR,
  ELF_ERROR_INVAL_PARAM,
  ELF_ERROR_FLASH,
  ELF_ERROR_PARSE,
  ELF_ERROR_FORMAT,
  ELF_ERROR_ENUM_SIZE = 0x7FFFFFFF
} boot_elf_error_type;

/* Function pointer typedef for ELF flag filter callback.  Returns TRUE
   if the type and flag fields match, otherwise FALSE. */
typedef boolean (*elf_segment_filter_func_type)
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
);


/* Forward declaration */
struct boot_elf_loader_if_type;

/* Definition of the boot ELF loader module virtual table structure */
typedef struct boot_elf_loader_vtbl_type
{
  /*=========================================================================

  FUNCTION  LOAD_SEGMENT

  DESCRIPTION
    Function to load a specific segment of the image given the segment index.

  DEPENDENCIES
    None
  
  RETURN VALUE
    The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_elf_error_type (*load_segment)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment to load */
  );

  /*=========================================================================

  FUNCTION  LOAD_FILTERED_SEGMENTS

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
  boot_elf_error_type (*load_filtered_segments)
  (
    struct boot_elf_loader_if_type *elf_if,    /* Pointer to the private
                                                  ELF loader data structure */
    elf_segment_filter_func_type segment_filter_func /* Callback filter */
  );

  /*=========================================================================

  FUNCTION  FIND_NEXT_FILTERED_SEGMENT

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
  uint32 (*find_next_filtered_segment)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    elf_segment_filter_func_type segment_filter_func,  /* Callback filter */
    uint32 previous_segment   /* Start searching after this segment index */
  );

  /*=========================================================================

  FUNCTION  FIND_SEGMENT_BY_ADDR

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
  uint32 (*find_segment_by_addr)
  (
    struct boot_elf_loader_if_type *elf_if,   /* Pointer to the private ELF loader data */
    void              *phys_addr  /* Physical RAM address to test */
  );

  /*=========================================================================

  FUNCTION  GET_NUM_SEGMENTS

  DESCRIPTION
    Function to return the number of program segments in the ELF image.

  DEPENDENCIES
    None
  
  RETURN VALUE
    The number of program segments in the ELF image.

  SIDE EFFECTS
    None

  ===========================================================================*/
  uint32 (*get_num_segments)
  (
    struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
  );

  /*=========================================================================

  FUNCTION  GET_HEADER

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
  const Elf32_Ehdr* (*get_header)
  (
    struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_TYPE

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
  uint32 (*get_segment_type)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_OFFSET

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
  uint32 (*get_segment_offset)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_VADDR

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
  void* (*get_segment_vaddr)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_PADDR

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
  void* (*get_segment_paddr)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_FILE_SIZE

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
  uint32 (*get_segment_file_size)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_MEMORY_SIZE

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
  uint32 (*get_segment_memory_size)
  (
   struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
   uint32            segment   /* Index of the segment */
   );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_FLAGS

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
  uint32 (*get_segment_flags)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_SEGMENT_ALIGN

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
  uint32 (*get_segment_align)
  (
    struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
    uint32            segment   /* Index of the segment */
  );

  /*=========================================================================

  FUNCTION  GET_PROGRAM_HEADER_PTR

  DESCRIPTION
    Function to return a pointer to the ELF program header structure.

  DEPENDENCIES
    None
  
  RETURN VALUE
    A pointer to the ELF program header structure.

  SIDE EFFECTS
    None

  ===========================================================================*/
  const void* (*get_program_header_ptr)
  (
    struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
  );

  /*=========================================================================

  FUNCTION  GET_ENTRY_POINT

  DESCRIPTION
    Function to return the ELF image entry point.

  DEPENDENCIES
    None

  RETURN VALUE
    The ELF image entry point.

  SIDE EFFECTS
    None

  ===========================================================================*/
  void* (*get_entry_point)
  (
    struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
  );

  /*=========================================================================

  FUNCTION  GET_LAST_ERROR

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
  boot_elf_error_type (*get_last_error)
  (
    struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
  );
} boot_elf_loader_vtbl_type;


/* Definition of public interface to the boot ELF loader module */
typedef struct boot_elf_loader_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_elf_loader_vtbl_type *vtbl;

} boot_elf_loader_if_type;


/* Definition of the boot ELF loader factory virtual table structure */
typedef struct boot_elf_loader_factory_vtbl_type
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
  boot_elf_loader_if_type* (*open)
  (
   boot_flash_trans_if_type *trans_if,            /* Pointer to an instance of 
                                                     the flash translation module */
   boolean                   sloppy_segment_load, /* Whether or not we can round
                                                     segment loads to whole pages
                                                     for efficiency */
   uint32                    header_offset,       /* Offset of the ELF header from
                                                     the base of the flash region */
   uint32                    segment_offset       /* Offset of the ELF segments from
                                                     the base of the flash region */ 
  );

  /*=========================================================================

  FUNCTION POINTER  OPEN_GENERIC

  DESCRIPTION
    Requests the factory to open an instance of the ELF loader module (using
    the generic flash translation layer).

  DEPENDENCIES
    None
  
  RETURN VALUE
    Pointer to the opened ELF loader interface

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_elf_loader_if_type* (*open_generic)
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
    boot_elf_loader_if_type **p_elf_if
  );
} boot_elf_loader_factory_vtbl_type;


/* Definition of public interface to the boot ELF loader factory module */
typedef struct boot_elf_loader_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_elf_loader_factory_vtbl_type *vtbl;

} boot_elf_loader_factory_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Declare the singleton, do it here so we can access the singleton below */
BOOT_SINGLETON_DECLARE(boot_elf_loader_factory_if);

/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_ELF_LOAD_SEGMENT

 DESCRIPTION
  Function to load a specific segment of the image given the segment index.

DEPENDENCIES
  None
  
RETURN VALUE
  The error enumeration if an error occurs, otherwise ELF_ERROR_NONE.

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_error_type boot_elf_load_segment
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment to load */
)
{
  return elf_if->vtbl->load_segment( elf_if, segment );
} /* boot_elf_load_segment() */

/*=========================================================================

FUNCTION  BOOT_ELF_LOAD_FILTERED_SEGMENTS

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
__inline boot_elf_error_type boot_elf_load_filtered_segments
(
  struct boot_elf_loader_if_type *elf_if,    /* Pointer to the private
                                                ELF loader data structure */
  elf_segment_filter_func_type segment_filter_func/* Callback filter */
)
{
  return elf_if->vtbl->load_filtered_segments( elf_if, segment_filter_func );
} /* boot_elf_load_filtered_segments() */

/*=========================================================================

FUNCTION  BOOT_ELF_FIND_NEXT_FILTERED_SEGMENT

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
__inline uint32 boot_elf_find_next_filtered_segment
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  elf_segment_filter_func_type segment_filter_func,  /* Callback filter */
  uint32 previous_segment   /* Start searching after this segment index */
)
{
  return elf_if->vtbl->find_next_filtered_segment( elf_if, segment_filter_func, previous_segment );
} /* boot_elf_find_next_filtered_segment() */

/*=========================================================================

FUNCTION  BOOT_ELF_FIND_SEGMENT_BY_ADDR

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
__inline uint32 boot_elf_find_segment_by_addr
(
  struct boot_elf_loader_if_type *elf_if,   /* Pointer to the private ELF loader data */
  void              *phys_addr  /* Physical RAM address to test */
)
{
  return elf_if->vtbl->find_segment_by_addr( elf_if, phys_addr );
} /* boot_elf_find_segment_by_addr() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_NUM_SEGMENTS

DESCRIPTION
  Function to return the number of program segments in the ELF image.

DEPENDENCIES
  None
  
RETURN VALUE
  The number of program segments in the ELF image.

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_elf_get_num_segments
(
  struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  return elf_if->vtbl->get_num_segments( elf_if );
} /* boot_elf_get_num_segments() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_HEADER

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
__inline const Elf32_Ehdr* boot_elf_get_header
(
  struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  return elf_if->vtbl->get_header( elf_if );
} /* boot_elf_get_header() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_TYPE

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
__inline uint32 boot_elf_get_segment_type
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_type( elf_if, segment );
} /* boot_elf_get_segment_type() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_OFFSET

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
__inline uint32 boot_elf_get_segment_offset
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_offset( elf_if, segment );
} /* boot_elf_get_segment_offset() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_VADDR

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
__inline void* boot_elf_get_segment_vaddr
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_vaddr( elf_if, segment );
} /* boot_elf_get_segment_vaddr() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_PADDR

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
__inline void* boot_elf_get_segment_paddr
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_paddr( elf_if, segment );
} /* boot_elf_get_segment_paddr() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_FILE_SIZE

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
__inline uint32 boot_elf_get_segment_file_size
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_file_size( elf_if, segment );
} /* boot_elf_get_segment_file_size() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_MEMORY_SIZE

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
__inline uint32 boot_elf_get_segment_memory_size
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_memory_size( elf_if, segment );
} /* boot_elf_get_segment_memory_size() */

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
__inline uint32 boot_elf_get_segment_flags
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_flags( elf_if, segment );
} /* boot_elf_get_segment_flags() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_SEGMENT_ALIGN

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
__inline uint32 boot_elf_get_segment_align
(
  struct boot_elf_loader_if_type *elf_if,  /* Pointer to the private ELF loader data */
  uint32            segment   /* Index of the segment */
)
{
  return elf_if->vtbl->get_segment_align( elf_if, segment );
} /* boot_elf_get_segment_align() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_PROGRAM_HEADER_PTR

DESCRIPTION
  Function to return a pointer to the ELF program header structure.

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to the ELF program header structure.

SIDE EFFECTS
  None

===========================================================================*/
__inline const void* boot_elf_get_program_header_ptr
(
  struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  return elf_if->vtbl->get_program_header_ptr( elf_if );
} /* boot_elf_get_program_header_ptr() */

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
__inline void* boot_elf_get_entry_point
(
  struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  return elf_if->vtbl->get_entry_point( elf_if );
} /* boot_elf_get_entry_point() */

/*=========================================================================

FUNCTION  BOOT_ELF_GET_LAST_ERROR

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
__inline boot_elf_error_type boot_elf_get_last_error
(
  struct boot_elf_loader_if_type *elf_if  /* Pointer to the private ELF loader data */
)
{
  return elf_if->vtbl->get_last_error( elf_if );
} /* boot_elf_get_last_error() */

/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the ELF loader module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_loader_if_type* boot_elf_loader_factory_open
(
  boot_flash_trans_if_type *trans_if,        /* Pointer to an instance of 
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
  return boot_elf_loader_factory_if_get_singleton()->vtbl->open( trans_if,
                                                                 sloppy_segment_load,
                                                                 header_offset,
                                                                 segment_offset );
} /* boot_elf_loader_factory_open() */

/*=========================================================================

FUNCTION  BOOT_ELF_LOADER_FACTORY_OPEN_GENERIC

DESCRIPTION
  Requests the factory to open an instance of the ELF loader module (using
  the generic flash translation layer).

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the opened ELF loader interface

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_elf_loader_if_type* boot_elf_loader_factory_open_generic
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
  return boot_elf_loader_factory_if_get_singleton()->vtbl->open_generic( trans_if,
                                                                         sloppy_segment_load,
                                                                         header_offset,
                                                                         segment_offset );
} /* boot_elf_loader_factory_open_generic() */

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
__inline void boot_elf_loader_factory_close( boot_elf_loader_if_type **p_elf_if )
{
  boot_elf_loader_factory_if_get_singleton()->vtbl->close( p_elf_if );
} /* boot_elf_loader_factory_close() */

/*=========================================================================

FUNCTION  ELF_SEGMENT_FILTER_DEFAULT

DESCRIPTION
  This is the default generic ELF segment callback filter.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the segment type is PT_LOAD, otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean elf_segment_filter_default
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
);

#endif /* BOOT_ELF_LOADER_IF_H */


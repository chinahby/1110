/*=============================================================================

                           B o o t   S h a r e d
             S e c u r e  E L F  L o a d e r  S i n g l e t o n

GENERAL DESCRIPTION
  Singleton for the secure boot ELF laoder interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_sec_elf_loader_if.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/04/08   MJS     Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_sec_elf_loader_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/* Define the singleton */
BOOT_SINGLETON_DEFINE(boot_sec_elf_loader_factory_if);


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
boolean elf_segment_filter_qc_default
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return ( ( type == ELF_PT_LOAD ) &&
           ( MI_PBT_PAGE_MODE_VALUE(flags) == MI_PBT_NON_PAGED_SEGMENT ) &&
           ( MI_PBT_SEGMENT_TYPE_VALUE(flags) != MI_PBT_HASH_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_NOTUSED_SEGMENT ) &&
           ( MI_PBT_ACCESS_TYPE_VALUE(flags) != MI_PBT_SHARED_SEGMENT ) );
  
} /* elf_segment_filter_default() */


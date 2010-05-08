/*=============================================================================

                           B o o t   S h a r e d
                    E L F  L o a d e r  S i n g l e t o n

GENERAL DESCRIPTION
  Singleton for the boot ELF laoder interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/elf_loader/boot_elf_loader_if.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/04/08   MJS     Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_elf_loader_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/* Define the singleton */
BOOT_SINGLETON_DEFINE(boot_elf_loader_factory_if);


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
boolean elf_segment_filter_default
(
  uint32 type,   /* Segment type field in program header */
  uint32 flags   /* Segment flags field in program header */
)
{
  return (type == ELF_PT_LOAD );
  
} /* elf_segment_filter_default() */


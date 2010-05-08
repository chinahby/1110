/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                          S o u r c e  F i l e

GENERAL DESCRIPTION
  This file contains function stubs for FSBL

EXTERNALIZED FUNCTIONS
  rex_int_lock
  rex_int_free

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_stubs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "fsbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*===========================================================================

FUNCTION  REX_INT_LOCK

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dword rex_int_lock(void)
{
  return 0;
}

/*===========================================================================

FUNCTION  REX_INT_FREE

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dword rex_int_free(void)
{
  return 0;
}

#ifndef FSBL_ENABLED_DLOAD
/*===========================================================================

  FUNCTION     REX_INTS_ARE_LOCKED

  DESCRIPTION
    Function stub, full function can be found in intlock_ads.s.

  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE
===========================================================================*/
int rex_ints_are_locked( void )
{
  /* Return TRUE to indicate that interrupts are disabled/locked */
  return TRUE;
}

/*===========================================================================

  FUNCTION     REX_JETTISON_CORE

  DESCRIPTION
    Function stub, full function can be found in intlock_ads.s.

  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE
===========================================================================*/
void rex_jettison_core(void)
{
  return;
}

/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP

DESCRIPTION
  Function stub, full function can be found in err.c.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
  NONE

===========================================================================*/
void err_fatal_core_dump 
(
  unsigned int line,      /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format   /* format string */
)
{}

#endif


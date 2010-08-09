/*=============================================================================

                           B o o t   S h a r e d
               A u t h e n t i c a t i o n  S i n g l e t o n

GENERAL DESCRIPTION
  Singleton for the boot authentication interface.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_auth_if.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/04/08   MJS     Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_auth_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/* Define the singleton */
BOOT_SINGLETON_DEFINE(boot_auth_if);


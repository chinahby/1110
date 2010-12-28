/*=============================================================================

                        E r r  R e s e t  D e t e c t i o n
 
GENERAL DESCRIPTION
  This file contains the code to detect HW reset of the chip and
  transition into the downloader.

EXTERNALIZED FUNCTIONS 
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/25/07   tbg     Initial revision (pulled from boot_hw_reset.c)
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "target.h"

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */

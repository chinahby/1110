/*=============================================================================

            B O O T   S H A R E D   S E G M E N T   T A I L
 
GENERAL DESCRIPTION
  This file contains the tail segment for the shared segment region.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800b/drivers/boot/boot_shared_seg_tail.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_shared_seg.h"

/*=============================================================================
  Following is the tail segment in the Shared Region. This tail segment will
  keep moving when a new segment is added.
=============================================================================*/
#pragma arm section zidata = "SHARED_DATA" 
boot_shared_seg_tail_data_type boot_shared_seg_tail_data;
#pragma arm section zidata
 

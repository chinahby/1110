/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Progressive Boot Back Ground Loading

GENERAL DESCRIPTION
  Touch pages of PAGED segments of AMSS memory. This trigges
  a page fault to the Iguana server. Iguana server copies the
  4K from NAND and maps it. This Function is called from
  the sleep task to do the Background Loading for Progressive boot.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/06   tk     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "dog.h"

#if defined(FEATURE_L4_PROGRESSIVE_BOOT)
#error code not present
#endif

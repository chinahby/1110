#ifndef __PARSE_SYSTEM_PARTITION_H__
#define __PARSE_SYSTEM_PARTITION_H__
/*=============================================================================
        P R O G R E S S I V E   B O O T   E L F   U T I L I T I E S

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/headergen/parse_system_partition.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/15/06   anb     Created
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include "miparti.h"
#include "mibib.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*===========================================================================
FUNCTION  PARSE_SYSTEM_PARTITION_LAYOUT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
uint32 parse_system_partition_layout
(
  char *fname,  /* input file name. */
  FILE *filetoread,
  flash_partable_t  sys_parti
);

#endif /* __PARSE_SYSTEM_PARTITION_H__ */


#ifndef BOOT_MSM_H
#define BOOT_MSM_H
/*===========================================================================
           S E C O N D A R Y    B O O T    L O A D E R 
                  H E A D E R  F I L E 

DESCRIPTION
  This file contains all the MSM hardware specific header files. It also 
  includes target header file

Copyright (c) 2005 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc1100/romboot/shared/boot_msm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.           
===========================================================================*/
#include "msmhwioreg.h"
#ifdef BUILD_BOOT_CHAIN_FSBL
#include "msmhwio.h"
#include "msmredefs.h"
#else
#include "boot_msmhwio.h"
#endif
#endif /* BOOT_MSM_H */


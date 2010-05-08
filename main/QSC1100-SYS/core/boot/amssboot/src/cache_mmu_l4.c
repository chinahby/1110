/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              A R M   C A C H E   F U N C T I O N S   O N   L 4

GENERAL DESCRIPTION
  This module contains the AMSS cache functions which are wrappers of
  MSM syscall implemented on L4. The cache functions are performed inside
  the L4 micro kernel because AMSS is in processor's user mode which is
  nor allowed to execute the MCR/MRC coprocessor instructions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc6055/drivers/boot/cache_mmu_l4.c#3 $
$DateTime: 2008/01/14 10:14:20 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/05   adm     Added support for dcache clean all
08/17/05   sd      Merged additional L4 changes to mainline.
08/16/05   sd      Changed function definition.
08/04/05   adm     Added support for dcache_inval_region
07/27/05   adm     Added support for dcache clean line and region   
12/09/04   wr      Created

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include "rex.h"
#include "msg.h"
#include "cache_mmu.h"

#ifdef FEATURE_L4
#error code not present
#endif

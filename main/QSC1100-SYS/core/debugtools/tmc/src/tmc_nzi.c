/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             TASK MAIN CONTROLLER NONE ZERO INIT MEMORY SECTION

GENERAL DESCRIPTION

  Declare TMC memory section that do not need to be initialize to zero. This
  requires a scatter loader (after linking) that puts *_nzi object files in 
  a different memory segment so that at run time bootmem.c sees a smaller 
  value in Image__APP_RAM__ZI__Length and so zeroes fewer bytes,which will 
  reduce the power up time.


EXTERNALIZED FUNCTIONS

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

    

Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/tmc/tmc_nzi.c#1 $ $DateTime: 2008/06/12 15:21:51 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   vph     Disable tmc_heap_small by added #ifndef FEATURE_LOW_MEMORY_USAGE
04/22/03   jqi     Initial Release

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE                     

===========================================================================*/
#include "tmc.h"
/* -----------------------------------------------------------------------
** Intertask command heap declaration
** ----------------------------------------------------------------------- */
#ifdef FEATURE_REX_DYNA_MEM
/* Task command heap */
mem_heap_type   tmc_heap;
uint8 tmc_heap_mem_buffer[TMC_HEAP_MEM_BUFFER_SIZE];

  #ifndef FEATURE_LOW_MEMORY_USAGE
    mem_heap_type   tmc_heap_small;
    uint8 tmc_heap_small_mem_buffer[TMC_HEAP_SMALL_MEM_BUFFER_SIZE];
  #endif /* FEATURE_LOW_MEMORY_USAGE */
#endif




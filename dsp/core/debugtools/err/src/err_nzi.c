/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Error Handler non-init data
                
GENERAL DESCRIPTION
  Contains all data used to log exceptions to EFS.  Since the information is
  stored to RAM first, the logged to NV after reset, the data needs to go 
  untouched by any of the RAM initialization functions.  The scatter load
  file is set up to put any *_nzi.o files in an uninitialized portion of RAM.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/07   tbg     Removed FEATURE_MULTIPROCESSOR for 6k support
10/19/06   tbg     File created.
 

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_ERR_EXTENDED_STORE
#include "err.h"

#ifdef FEATURE_ERR_SAVE_CORE
#include "err_rle.h"
#endif

#endif
 
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_ERR_EXTENDED_STORE

  #if defined FEATURE_L4
#error code not present
  #endif

  err_data_type err_data;

  #ifdef FEATURE_ERR_SAVE_CORE
    err_core_dump_type err_core_dump;
  #endif /* FEATURE_ERR_SAVE_CORE */

  uint32 err_flash_write_counter;

  #if defined FEATURE_L4
#error code not present
  #endif

#endif /* FEATURE_ERR_EXTENDED_STORE */


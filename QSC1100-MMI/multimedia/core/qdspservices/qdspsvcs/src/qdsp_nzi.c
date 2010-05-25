/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         Q D S P   E R R O R   H A N D L E R   N O N - I N I T  D A T A
                         S O U R C E   F I L E
                
GENERAL DESCRIPTION
  Contains all data used to log exceptions to QDSP.  Since the information is
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
09/10/08   knm     File created.
 

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "qdspi.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif /* FEATURE_QDSP_ERR_LOGS_STORE */



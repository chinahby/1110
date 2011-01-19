/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the Protocol Statistics related AT modem commands for the
  IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/3/01     rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707pstats.h"
#include "dsat707pstatsctab.h"
#include "dsatparm.h"
#include "msg.h"


#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */

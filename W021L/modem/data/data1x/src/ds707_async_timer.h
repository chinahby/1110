#ifndef DS707_TIMER_H
#define DS707_TIMER_H
/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ T I M E R
DESCRIPTION
  This file contains functions related to timers in IS707 circuit switched
  data calls (Async/Fax).

EXTERNALIZED FUNCTIONS

ds707_async_timer_init
  This function registers timers used in 707 async/fax calls with DS3G 
  timer services.

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_timer.h_v   1.2   02 Oct 2002 13:22:36   ajithp  $
    $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_async_timer.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

-----------------------------------------------------------------------------
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_TIMER_INIT

DESCRIPTION   Register async 707 timers with 3G DSMGR Timer services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_async_timer_init(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_TIMER_H      */


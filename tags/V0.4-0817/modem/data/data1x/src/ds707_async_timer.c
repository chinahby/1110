/*===========================================================================

                        D S 7 0 7 _ T I M E R

DESCRIPTION
  This file contains functions and definitions related to timers in IS707
  circuit switched data calls (Async/Fax).

EXTERNALIZED FUNCTIONS

ds707_async_timer_init
  This function registers timers used in 707 async/fax calls with DS3G
  timer services.

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_timer.c_v   1.2   02 Oct 2002 13:22:30   ajithp  $
    $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_async_timer.c#2 $ $DateTime: 2009/01/29 05:35:22 $ $Author: nsivakum $
  
-----------------------------------------------------------------------------
05/23/06    squ    Featurized async data
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "ds3gtimer.h"
#include "dstask.h"
#include "ds707.h"
#include "ds707_async_timer.h"
#include "err.h"
#include "rex.h"



/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
   Clocks (Timer for protocols establishment)
---------------------------------------------------------------------------*/  

LOCAL rex_timer_type ds707_async_ptcl_opening_timer; 


/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_TIMER_CB

DESCRIPTION   Callback for async timer expiration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds707_async_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( cmd_ptr = ds_get_cmd_buf() ) == NULL )
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_ASYNC_PTCL_OPENING:
      cmd_ptr->hdr.cmd_id= DS_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD;
      break;

    default:
      ERR("Bad timer id on callback %d",timer_id,0,0);
      ASSERT(0);
      return;
  }
  
  ds_put_cmd( cmd_ptr );

} /* ds707_async_timer_cb() */


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_TIMER_INIT

DESCRIPTION   Register 707 async timers with 3G DSMGR Timer services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_async_timer_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3gi_timer_register( &ds707_async_ptcl_opening_timer,
                        ds707_async_timer_cb,
                        DS3G_TIMER_ASYNC_PTCL_OPENING );

} /* ds707_async_timer_init() */


#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */


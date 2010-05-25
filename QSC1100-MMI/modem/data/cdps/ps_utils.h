#ifndef PS_UTILS_H
#define PS_UTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S . H

GENERAL DESCRIPTION
  Collection of utility functions being used by various modules in PS.  
  Most of these functions assume that the caller is in PS task context.

  Copyright (c) 1997-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_utils.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/02   usb     Moved lcsum() from this file to ps_iputil.c
08/05/02   usb     Moved inclusion of nv.h out of FEATURE_MIP
07/31/02   usb     Renamed the file from psmisc.h
06/14/02   usb     Removed byte manipulation functions, use dsbyte.h.
04/17/02   rc      Wrapped include of rlcdl.h in FEATURE_DATA_WCDMA_PS
03/04/02   dwp     Add include of rlcdl.h.
12/21/01   dwp     Wrap get16 in !FEATURE_DATA_PS as it is defined else where
                   in MSM5200 archive.
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
08/05/99   rc      Added support for UDP debug statistics for Sockets.
03/04/99   hcg     Changed browser interface feature to FEATURE_DS_BROWSER_INTERFACE.
06/16/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97   ldg     Added new FEATURE_DS_PSTATS
04/11/95   jjw     Initial version of file


===========================================================================*/





/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "target.h" 

#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include "ps_svc.h"
#include "rex.h"
#include "nv.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DS_PSTATS
#error code not present
#else
#define DS_PSTATS(a) (0)
#endif


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_START_TIMER

DESCRIPTION
  This function will set the specified timer to the specified duration. The 
  time units are in milliseconds.
  
DEPENDENCIES
  The timer should already be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ps_start_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be started     */
  dword duration                         /* time interval, in msecs        */
);


/*===========================================================================

FUNCTION PS_CANCEL_TIMER

DESCRIPTION
  This function will cancel the specified timer and the associated signal 
  mask will also be cleared. NOTE that the signals are associated with 
  timers during initialization by the REX_DEF_TIMER function.

DEPENDENCIES
  The timer should already be initialized. The signal associated with the 
  timer should be a PS task signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ps_cancel_timer
(
  rex_timer_type  *timer_ptr,            /* ptr to timer to be cancelled   */
  ps_sig_enum_type sig                   /* signal associated with a timer */
);


/*===========================================================================

FUNCTION PS_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.

DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  

===========================================================================*/
extern nv_stat_enum_type ps_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
);


/*===========================================================================
FUNCTION PS_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.

DEPENDENCIES
  This function can only be called from PS task.  Also it is not 
  reentrant. Shouldn't be a problem, as it doesn't exit till we're done, and 
  it's only called from the PS task.
  
RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other PS task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.  
  
===========================================================================*/
extern nv_stat_enum_type ps_put_nv_item(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
);


/*===========================================================================

FUNCTION msclock

DESCRIPTION
  This function will return the time in milliseconds since ....

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dword msclock( void);

#endif /* FEATURE_DS || FEATURE_DATA_PS */
#endif /* PS_UTILS_H */


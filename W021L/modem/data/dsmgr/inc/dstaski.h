#ifndef DSTASKI_H
#define DSTASKI_H
/*===========================================================================

                     D A T A   S E R V I C E S   T A S K

                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This is the internal header file for the Data Services (DS) Task. This file
  contains all the functions, definitions and data types needed for other
  units within the Data Services Task.


  Copyright (c) 2001-2007 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dstaski.h_v   1.0   08 Aug 2002 11:19:50   akhare  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/dstaski.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/07 sq/mga/yz Merged extern dsi_wait.
07/14/02   ak      Added IS_IN_DS_TASK macro().
10/01/01   ar      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "nv.h"
#include "rex.h"
#include "task.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO         IS_IN_DS_TASK

DESCRIPTION   Returns TRUE if executing in DS task.  Can RETURN WRONG TASK
              if called in an ISR.

RETURN VALUE  TRUE - executing in DS task
              FALSE - not executing in DS TASK

DEPENDENCIES  Called only in task-context.

SIDE EFFECTS  None
===========================================================================*/
#define IS_IN_DS_TASK() ((rex_self() == &ds_tcb) ? TRUE : FALSE)


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSI_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.

DEPENDENCIES
  NV task is expected to be available

SIDE EFFECTS
  None

===========================================================================*/
extern nv_stat_enum_type dsi_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);



/*===========================================================================
FUNCTION DSI_PUT_NV_ITEM 

DESCRIPTION
  Put an item into the nonvolatile memory.  

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  NV task is expected to be available

SIDE EFFECTS
  None

===========================================================================*/
extern nv_stat_enum_type dsi_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);



/*===========================================================================

FUNCTION DS_WAIT

DESCRIPTION
  This function suspends execution of the Data Services task until one (or
  more) signals from the requested signal mask is set.

  Watchdog kicking is performed in this function.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask returned by rex_wait().

SIDE EFFECTS
  None

===========================================================================*/
extern rex_sigs_type  dsi_wait
(
   rex_sigs_type  requested_signal_mask       /* Signal mask to suspend on */
);


#endif /* FEATURE_DATA */

#endif /* DSTASKI_H */

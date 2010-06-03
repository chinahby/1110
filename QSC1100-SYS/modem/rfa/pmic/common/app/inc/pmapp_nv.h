#ifndef PMAPP_NV_H
#define PMAPP_NV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 PMIC DRIVER SERVICES FOR NV

GENERAL DESCRIPTION
     This file contains functions and variable declarations to support 
  the NV related operations inside the Qualcomm Power Manager IC.

INITIALIZATION AND SEQUENCING REQUIREMENTS                            
  None.

Copyright (c) 2008,2009 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_nv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/09   vk     Added API to read NV item
===========================================================================*/
/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/

#include "nv.h"

/*===========================================================================
                                 Function declarations
===========================================================================*/

/*===========================================================================

FUNCTION pm_get_nv_item

DESCRIPTION
  This function performs a Single NV Read operation to retrieve the requested item.

DEPENDENCIES
  The NV task has been started and is running.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other current task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.

===========================================================================*/
extern nv_stat_enum_type pm_get_nv_item(
  nv_items_enum_type  item_code,        /* Item to get */
  nv_item_type        *item_ptr,        /* Pointer where to put the item */
  rex_tcb_type        *task_ptr,        /* Calling task's TCB pointer */
  rex_sigs_type       task_wait_sig,    /* Task signal to wait for when
                                           reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
); 
#endif /* PMAPP_NV_H */

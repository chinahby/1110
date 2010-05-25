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

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_nv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/09   vk     Added API to read NV item
===========================================================================*/
/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/

#include "pmapp_nv.h"

/*===========================================================================
                                 Functions 
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
nv_stat_enum_type pm_get_nv_item(
  nv_items_enum_type  item_code,        /* Item to get */
  nv_item_type        *item_ptr,        /* Pointer where to put the item */
  rex_tcb_type        *task_ptr,        /* Calling task's TCB pointer */
  rex_sigs_type       task_wait_sig,    /* Task signal to wait for when
                                           reading NV */
  void          (*task_wait_handler)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
)
{
   /* Command buffer to NV */
   nv_cmd_type  nv_cmd_buf;

   /* Prepare command buffer to NV. */
   nv_cmd_buf.cmd = NV_READ_F;                /* Read request         */
   nv_cmd_buf.tcb_ptr = task_ptr;             /* Notify back to me    */
   nv_cmd_buf.sigs = task_wait_sig;           /* With this signal     */
   nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
   nv_cmd_buf.item = item_code;               /* Item to get          */
   nv_cmd_buf.data_ptr = item_ptr;            /* Where to return it   */

   /* Clear signal, issue the command, and wait for the respone. */
   /* Clear signal for NV  */
   (void) rex_clr_sigs(task_ptr, task_wait_sig);

   /* Issue the request    */
   nv_cmd(&nv_cmd_buf);

   /* Wait for completion  */
   task_wait_handler(task_wait_sig);

   /* clear the signal again */
   (void) rex_clr_sigs( task_ptr, task_wait_sig );

   /* return status */
   return (nv_cmd_buf.status);
}


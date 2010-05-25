/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

   ipc_post_msg()        - Allocate a new message

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/kernel/rte/main/latest/src/ipcrex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/06    gr     Replaced errno.h with IxErrno.h to avoid conflicts with
                   system files.
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "pq.h"
#include "ipcdefi.h"
#include "ipcrex.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/


/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/



/*---------------------------------------------------------------------------
  External function declarations.
---------------------------------------------------------------------------*/
void ipc_post_msg
(
   ipc_internal_msg_type* msg_ptr,
   struct rex_tcb_struct* tcb_ptr
)
{
   /* Post message to the receiver
   **/
   INTLOCK();
   {
      /* Add the message in the priority queue */
      pq_insert_msg( msg_ptr, tcb_ptr->ipc_info.mq_ptr );
   
      /* Signal the receiver that a message has arrived
      ** only when it expects this particular message **/
      if ( (tcb_ptr->ipc_info.msg_wanted == IPC_MSG_ANY)
           ||
           (tcb_ptr->ipc_info.msg_wanted == msg_ptr->msg_id) )
      {
         tcb_ptr->ipc_info.msg_wanted = IPC_MSG_ANY;
         (void)rex_set_sigs( tcb_ptr, tcb_ptr->ipc_info.receive_signal );
      }
   }
   INTFREE();
}

#endif /* FEATURE_REX_IPC */


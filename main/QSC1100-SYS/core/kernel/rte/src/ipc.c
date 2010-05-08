/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           S E C U R I T Y   S E R V I C E S   T A S K
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/




/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/kernel/rte/main/latest/src/ipc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/07   cz      Adding REX signal extension support.
11/07/06    rv     Added changes to Handle ipc calls from BREW thread on SDK
04/07/06   dlr     Fixed pointer assignment in send() 
01/20/06    gr     Replaced errno.h with IxErrno.h to avoid conflicts with
                   system files.
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stddef.h>
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "IxErrno.h"
#include "msg.h"

#include "ipcmem.h"
#include "ipc.h"
#include "ipcns.h"
#include "ipcdefi.h"
#include "ipcrex.h"
#ifdef UIONE_SDK
extern int ipc_is_brew_thread(void);
extern rex_tcb_type *ipc_brew_get_tcb(void);
extern union ipc_msg_type* ipc_receive_for_brew(ipc_msg_id_type msg_id);
#endif //UIONE_SDK

#ifdef FEATURE_REX_SIGS_EXT
   #ifndef HAVE_REX_SIGS_EXT
      #error "REX Signal API Extension is not avaibale. Check REX Version"
   #endif /* HAVE_REX_SIGS_EXT */
#endif /* FEATURE_REX_SIGS_EXT */

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
  Internal function declarations.
---------------------------------------------------------------------------*/
struct rex_tcb_type* ipc_get_task_tcb
(
   ipc_node_id_type task_id
);

/*---------------------------------------------------------------------------
  External function defintions.
---------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION IPC_NEW

DESCRIPTION
  Allocate a message in the shared memory pool. The calling entity is now the
  owner of the memory.
  Different memory pools can be used according. 
   
PARAMETERS
  msg_size:  the size of the message to allocate memory for
  msg_id:    the id of the message to allocate
  msg_pool:  use memory from the specified pool (e.g. public or protected pool)

RETURN VALUE
  A pointer to the allocated message NULL if allocation failed
  
SIDE EFFECTS
  None
===========================================================================*/
union ipc_msg_type* __ipc_new
(
   uint32            size,
   ipc_msg_id_type   msg_id,
   ipc_msg_pool_type pool_id
)
{
   ipc_internal_msg_type* msg_ptr;
   (void)pool_id; /* unused for now */

   INTLOCK();

   msg_ptr = (ipc_internal_msg_type*) 
              (IPC_MSG_EXT_TO_INT(ipcmem_allocate(size))); /*lint !e413 */   
   INTFREE();
   
   if ( msg_ptr == NULL )
   {
      return NULL;
   }
 #ifdef UIONE_SDK
   if (ipc_is_brew_thread())
     msg_ptr->owner = IPC_NODE_INFO(ipc_brew_get_tcb())->node_id;
   else
#endif //UIONE_SDK
   msg_ptr->owner    = IPC_NODE_INFO(rex_self())->node_id;
   msg_ptr->sender   = 0;
   msg_ptr->receiver = 0;
   msg_ptr->msg_id   = msg_id;

   return IPC_MSG_INT_TO_EXT(msg_ptr);  /*lint !e413 */
}


/*===========================================================================
FUNCTION IPC_DELETE

DESCRIPTION
  Free the memory associated with the message and return it to the memory pool
  from which it was allocated.
  The calling entity looses the ownership of the memory.
 
PARAMETERS
  msg_ptr:  the message pointer to deallocate

RETURN VALUE
  None.
 
SIDE EFFECTS
  The pointer to the message is set to NULL. 
===========================================================================*/
void __ipc_delete
(
   union ipc_msg_type** msg_ptr
)
{
   ipc_internal_msg_type* internal_msg_ptr;
   ipc_node_id_type       nid;
   
   if ( msg_ptr == NULL )
   {
      MSG_HIGH("Pointer to 'message pointer' is NULL", 0, 0, 0);
      return;
   }
   
   if ( *msg_ptr == NULL )
   {
      MSG_HIGH("Message pointer is NULL", 0, 0, 0);
      return;
   }

   internal_msg_ptr = IPC_MSG_EXT_TO_INT(*msg_ptr); /*lint !e413 !e826*/
 #ifdef UIONE_SDK
   if (ipc_is_brew_thread())
     nid = IPC_NODE_INFO(ipc_brew_get_tcb())->node_id;
   else
 #endif //UIONE_SDK
    nid = IPC_NODE_INFO(rex_self())->node_id;
   
   if ( internal_msg_ptr->owner != nid )
   {
      MSG_HIGH("Not Message owner of %p!!!", internal_msg_ptr, 0, 0);
      return;
   }

   INTLOCK();   
   ipcmem_deallocate(*msg_ptr);
   INTFREE();

   *msg_ptr = NULL;
}


/*===========================================================================
FUNCTION IPC_SEND

DESCRIPTION
  The message will be posted to the receiving module queue. 
  The calling entity looses the ownership of the memory.
 
PARAMETERS
  msg_ptr:   the message pointer to send
  module_id: the identification of the receiving module

RETURN VALUE
 E_SUCCESS: message has been sent (msg ownership is transferred to the destination)
  E_FAILURE: message could not be sent (msg ownership remain with the caller)

SIDE EFFECTS
  The module_id is added to the client list of used modules if new
  The pointer to the message is set to NULL when E_SUCCESS
===========================================================================*/
errno_enum_type __ipc_send
(
   union ipc_msg_type** msg_ptr,
   ipc_node_id_type     destination_node_id 
)
{
   ipc_internal_msg_type* internal_msg_ptr;
   void*                  destination_node_ptr;
   ipc_node_id_type       nid;
   
   /*
   * Sanity check on the message pointer
   */
   if ( msg_ptr == NULL )
   {
      MSG_HIGH("**msg_ptr is NULL", 0, 0, 0);
      return E_FAILURE;
   }
   
   if ( *msg_ptr == NULL )
   {
      MSG_HIGH("*msg_ptr is NULL", 0, 0, 0);
      return E_FAILURE;
   }

   internal_msg_ptr = IPC_MSG_EXT_TO_INT(*msg_ptr); /*lint !e413 !e826 */
#ifdef UIONE_SDK  
   if (ipc_is_brew_thread())
     nid = IPC_NODE_INFO(ipc_brew_get_tcb())->node_id;
   else
#endif //UIONE_SDK
   nid              = IPC_NODE_INFO(rex_self())->node_id;
   if ( internal_msg_ptr->owner != nid )
   {
      MSG_HIGH("Not Message owner of %p!!!", internal_msg_ptr, 0, 0);
      return E_FAILURE;
   }
   /* Should we verify that the buffer is actually allocated??? */

   /*
   * Get the TCB of the destination module id
   */
   destination_node_ptr = ipcns_node_get( destination_node_id );
   if ( destination_node_ptr == NULL )
   {
      MSG_ERROR("Task (%d) is not available", destination_node_id, 0, 0);
      return E_FAILURE;
   }
   
   /*
   * Update the internal message fields
   */
   internal_msg_ptr->owner    = IPC_NODE_RESERVED_ID;
   internal_msg_ptr->sender   = nid;
   internal_msg_ptr->receiver = destination_node_id;

   /*
   * Post the message into the receiver queue
   */   
   ipc_post_msg( internal_msg_ptr, destination_node_ptr );

   *msg_ptr = NULL;

   return E_SUCCESS;
}


/*===========================================================================
FUNCTION IPC_RECEIVE

DESCRIPTION
  Waits until a message is posted in the receive queue of the caller.
  This is a _blocking_ call when no message is available.

  A specific message ID may be specified, in which case only this
  specific message will trigger the function to return.  Any message
  can be received by specifying IPC_MSG_ANY.

PARAMETERS
  msg_id:  The required message ID (or IPC_MSG_ANY)

RETURN VALUE
  A pointer to the received message. 
==========================================================================*/
union ipc_msg_type* ipc_receive
(
   ipc_msg_id_type msg_id
)
{
   ipc_internal_msg_type* internal_msg_ptr = NULL;
   rex_tcb_type*          tcb_ptr;
   /*----------------------------------------------------------------------*/

   /* When entering this function, a message _may_ already be
   ** available in the incoming queue.
   ** Because ipc_receive() can be used in either pure RTE mode
   ** or REX/RTE mode, we cannot rely on the signal to be set
   ** to indicate a message is in the queue.
   ** In RTE mode the signal is guaranted to be set if a message
   ** is in the incoming queue.  In REX/RTE mode however, because
   ** the user might already have cleared the RECEIVE_SIG signal
   ** with rex_clr_sigs(), we cannot rely on the signal to be set
   ** to indicate a message is in the queue.
   ** The only option is to check the message queue first, and
   ** only wait on the signal if the queue is empty.
   */
   /*
   1 - [] - ipc_receive(any/id) ---> block
   2 - [......] - ipc_receive(any) ---> return msg
   3 - [...,id,...] - ipc_receive(id) ---> return msg
   4 - [^id] - ipc_receive(id) ---> block
   5 - [......] - rex_wait(), ipc_receive(any) ---> return msg
   6 - [......] - rex_wait(), ipc_receive(id) ---> block. BAD!!!
   */
#ifdef UIONE_SDK
   if (ipc_is_brew_thread())
     return ipc_receive_for_brew (msg_id);
#endif //UIONE_SDK

   tcb_ptr = rex_self();

   INTLOCK();
   {
      /* See if this message is already in the priority queue */
      internal_msg_ptr = pq_get_next_msg(tcb_ptr->ipc_info.mq_ptr, msg_id);

      /* Wait for it if not there */
      while (internal_msg_ptr == NULL)  /* TBD: need to look at send to find out
                                     why we get NULL pointer in rare occasions */
      {
         tcb_ptr->ipc_info.msg_wanted = msg_id;

         /* Clear the signal.
         ** It will be set again by ipc_send() when the required
         ** message shows up */
#ifdef FEATURE_REX_SIGS_EXT   
         tcb_ptr->sigs[0] &= ~tcb_ptr->ipc_info.receive_signal;
#else
         tcb_ptr->sigs &= ~tcb_ptr->ipc_info.receive_signal;
#endif /* FEATURE_REX_SIGS_EXT */

         (void)rex_wait(tcb_ptr->ipc_info.receive_signal);

         /* This is guaranted to be our message */
         internal_msg_ptr = pq_get_next_msg(tcb_ptr->ipc_info.mq_ptr, msg_id);

         tcb_ptr->ipc_info.msg_wanted = IPC_MSG_ANY;
      }
   
      /*
      ** Clear the signal if the queue is empty */
      if (pq_is_empty(tcb_ptr->ipc_info.mq_ptr))
      {
#ifdef FEATURE_REX_SIGS_EXT   
         tcb_ptr->sigs[0] &= ~tcb_ptr->ipc_info.receive_signal;
#else
         tcb_ptr->sigs &= ~tcb_ptr->ipc_info.receive_signal;
#endif /* FEATURE_REX_SIGS_EXT */         
      }
   }
   INTFREE();

   /*
   ** Adjust the internal message information */
   internal_msg_ptr->owner    = tcb_ptr->ipc_info.node_id;

   /*
   ** Give the message to the caller */
   return IPC_MSG_INT_TO_EXT(internal_msg_ptr); /*lint !e413 */
}


/*===========================================================================
FUNCTION IPC_SENDER

DESCRIPTION
  Returns the ID of the module which sent the message.

PARAMETERS
  msg_ptr:    The message to look at
  The module: ID of the message sender. 

RETURN VALUE
  The node ID of the message sender. 
==========================================================================*/
ipc_node_id_type __ipc_sender
(
   union ipc_msg_type* msg_ptr
)
{
   ipc_internal_msg_type* internal_msg_ptr;
   ipc_node_id_type       owner;
   
   if ( msg_ptr == NULL )
   {
      MSG_HIGH("Message pointer is NULL", 0, 0, 0);
      return IPC_NODE_RESERVED_ID;
   }
   
   internal_msg_ptr = IPC_MSG_EXT_TO_INT(msg_ptr);  /*lint !e413 !e826 */
#ifdef UIONE_SDK   
   if (ipc_is_brew_thread())
     owner = IPC_NODE_INFO(ipc_brew_get_tcb())->node_id;
   else
#endif //UIONE_SDK
     owner = IPC_NODE_INFO(rex_self())->node_id;
   
   if ( internal_msg_ptr->owner != owner )
   {
      MSG_HIGH("Not Message owner of %p!!!", internal_msg_ptr, 0, 0);
      return IPC_NODE_RESERVED_ID;
   }

   return internal_msg_ptr->sender;
}



/*===========================================================================
FUNCTION IPC_CONFIGURE_MSG_PRIORITY()

DESCRIPTION                                                                */
   /** Returns the ID of the module which sent the message.

@param  msg_ptr   The message to look at

@return  The module ID of the message sender. 
*/
/*========================================================================*/
boolean ipc_configure_msg_priority
(
   const ipc_msg_priority_type* list,
   uint32                 count
)
{
   uint32                 i;

   /* Verify priorities are in range */
   for(i=0; i<count; ++i)
   {
      if ( (list[i].priority < IPC_MSG_PRIORITY_MIN)
           ||
           (list[i].priority > IPC_MSG_PRIORITY_MAX) )
      {
         return FALSE;
      }
   }
   
   return pq_configure_msg_priority( rex_self()->ipc_info.mq_ptr,
                                     list,
                                     count );
}


/*===========================================================================
FUNCTION IPC_PRINT_MSG

DESCRIPTION

PARAMETERS

RETURN VALUE

==========================================================================*/
void ipc_print_msg(union ipc_msg_type* msg_ptr)
{
   ipc_internal_msg_type* internal_msg_ptr;
   
   if ( msg_ptr == NULL )
   {
      MSG_HIGH("Message pointer is NULL", 0, 0, 0);
      return;
   }
   
   internal_msg_ptr = IPC_MSG_EXT_TO_INT(msg_ptr);  /*lint !e413 !e826 */
   
   if ( internal_msg_ptr->owner != (uint32)rex_self() )
   {
      MSG_HIGH("Not Message owner of %p!!!", internal_msg_ptr, 0, 0);
      return;
   }

   MSG_MED("Msg Pointer: %p", internal_msg_ptr,0,0);
   MSG_MED("    Owner:    %d",internal_msg_ptr->owner,0,0);
   MSG_MED("    Sender:   %d",internal_msg_ptr->sender,0,0);
   MSG_MED("    Receiver: %d",internal_msg_ptr->receiver,0,0);
   MSG_MED("    Msg Id:   %d",internal_msg_ptr->msg_id,0,0);
}


/*===========================================================================
FUNCTION INIT_IPC()

DESCRIPTION
   IPC Initialization routine to be called at task init.

PARAMETERS
   service_id         the ipc_node_id 
   ipc_receive_sig    signal used to received ipc
RETURN
   None.
==========================================================================*/
void init_ipc(ipc_node_id_type service_id,
              uint32           ipc_receive_sig 
              )
{
  rex_tcb_type *ipc_thread_tcb_ptr = rex_self();
  (void) rex_clr_sigs( ipc_thread_tcb_ptr, (rex_sigs_type) ~ipc_receive_sig );
  (void) ipcns_service_register( service_id, ipc_thread_tcb_ptr );
  rex_register_receive_signal( (rex_sigs_type)ipc_receive_sig );
}

#endif /* FEATURE_REX_IPC */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/kernel/rte/main/latest/src/rteprocessrex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18-May-04  rwh     Changed FEATURE_REX_IPC to FEATURE_RTE_PROCESS
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC.
                   Feature-ized for FEATURE_MULTIMODE_ARCH. 
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================
   INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
   Include Files
-------------------------------------------------------------------*/
#include "comdef.h"

#ifdef FEATURE_RTE_PROCESS

#include "err.h"
#include "rex.h"
#include "task.h"
#include "rte.h"
#include "rteprocess.h"
#include "rteprocessrex.h"

/*-------------------------------------------------------------------
   Defines
-------------------------------------------------------------------*/
/* ACK Signal as defined in tmc.h as TMC_ACK_SIG */
#define RTEPROCESS_ACK_SIG             0x0001
/* IPC Receive signal for this process */
#define RTEPROCESS_PROCESS_RECEIVE_SIG 0x0002

/*-------------------------------------------------------------------
   Type Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
   Global Constant Declarations
-------------------------------------------------------------------*/


/*===========================================================================
   FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_os_prehook
(
   void* ptr
)
{
   (void)ptr;
   rex_register_receive_signal( RTEPROCESS_PROCESS_RECEIVE_SIG );
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_run
(
   void* ptr
)
{
   struct rteprocess_process_type* process_ptr;

   process_ptr = (struct rteprocess_process_type*)ptr;

   (void) rex_set_sigs(process_ptr->os_info.tcb_ptr, TASK_START_SIG);
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_process
(
   void* ptr
)
{
   (void)ptr;
   __rteprocess_wait_for_thread(rex_self());
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_thread
(
   void* ptr
)
{
   (void)ptr;
   (void)rex_wait( RTEPROCESS_ACK_SIG );
   (void)rex_clr_sigs( rex_self(), RTEPROCESS_ACK_SIG );
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_run_thread
(
   void* ptr
)
{
   struct rteprocess_thread_type* thread_ptr;

   thread_ptr = (struct rteprocess_thread_type*)ptr;

   (void) rex_set_sigs(thread_ptr->os_info.tcb_ptr, TASK_START_SIG);
   __rteprocess_wait_for_thread(thread_ptr->os_info.tcb_ptr);
}



/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
/*------------------------------------------------------------------------
Let the process know we are done running our hook code
-------------------------------------------------------------------------*/
void __rteprocess_notify_thread_ready
(
   void* ptr
)
{
   struct rteprocess_thread_type* thread_ptr;

   thread_ptr = (struct rteprocess_thread_type*)ptr;

   (void)rex_set_sigs( thread_ptr->process_ptr->os_info.tcb_ptr,
                       RTEPROCESS_ACK_SIG );
}



/*===========================================================================
FUNCTION 
  Wait for the process to give the thread the permission to run

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_run_order
(
   void* ptr
)
{
   (void)ptr;
   (void)rex_wait( TASK_START_SIG );
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_verify_process
(
   void* ptr
)
{
   struct rteprocess_process_type* process_ptr;

   process_ptr = (struct rteprocess_process_type*)ptr;

   if (process_ptr->os_info.tcb_ptr->ipc_info.receive_signal == 0)
   {
      ERR_FATAL("REX IPC Receive Signal not set",0,0,0);
   }
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_verify_thread
(
   void* ptr
)
{
   struct rteprocess_thread_type* thread_ptr;

   thread_ptr = (struct rteprocess_thread_type*)ptr;

   if (thread_ptr->os_info.tcb_ptr->ipc_info.receive_signal == 0)
   {
      ERR_FATAL("REX IPC Receive Signal not set",0,0,0);
   }
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_notify_system_monitor_process_ready
(
   void* ptr
)
{
   (void)ptr;
#ifdef FEATURE_MULTIMODE_ARCH
   (void)rex_set_sigs( &tmc_tcb, RTEPROCESS_ACK_SIG );
#else
   (void)rex_set_sigs(  &mc_tcb, RTEPROCESS_ACK_SIG );
#endif
}


/*===========================================================================
FUNCTION 

DESCRIPTION


PARAMETERS

RETURN
  None
===========================================================================*/
void __rteprocess_wait_for_system_monitor
(
   void* ptr
)
{
   (void)ptr;
   (void)rex_wait( TASK_START_SIG );
}

#endif /* FEATURE_RTE_PROCESS */


/*==============================================================================

                          H S U _ A L _ T A S K . C


GENERAL DESCRIPTION
  High-Speed USB Implementation of the Adaptation Layer Command Task.

  The Adaptation Layer Command Task is a Rex task in charge of executing 
  High-Speed USB adaptation layer commands.
  An adaptation layer command is a code section which needs to be executed
  after the occurrence of a Data Mover interrupt, but can't be executed within
  the interrupt context itself (for an example, when this code section 
  is blocking). Therefore, the interrupt queues a command in the commands queue.
  Later on, another task, the adaptation layer task, dequeues this item from 
  the command queue and executes the command outside of the interrupt context.

  This task manages a cyclic queue of commands. Each queue entry contains
  a function address and an actual parameter for this function, which is 
  of type void*.
  When commands are inserted into the queue (using the 'hsu_al_task_enqueue_cmd'
  function), a signal is sent to this task, and wakes it up. Then, this task 
  processes all the queued commands in a FIFO order. For each queued command,
  this task calls the supplied function, passing it its argument.

  While this task is processing command queue items, the Giant mutex is locked.
  When this task finishes processing the queue (i.e., it becomes empty 
  again), this task releases the giant mutex and returns waiting for its
  signal.

  The command queue is implemented using a cyclic buffer which has a head index
  and a tail index. The tail index points to the location of the next inserted
  item, and the head points to the location of the next popped item.
  When the head == tail, the queue is empty, and when head == tail + 1, the 
  queue is full. This way, the queue may be managed without a mutex protection
  since no item counter is needed, and there is no simultaneous access to 
  the same end of the queue.


EXTERNALIZED FUNCTIONS
  Externalized to outside of the High_Speed USB stack adaptation layer:
    hsu_al_task_main

  Externalized internally in the High_Speed USB stack adaptation layer:
    hsu_al_task_enqueue_cmd


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The High-Speed USB adaptation layer task need to be created before 
  using the API defined in this file.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*===========================================================================

EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/06   ds      Created.

===========================================================================*/


/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */
#include <dog.h>
#include <task.h>

/* High Speed USB subsystem include files. */
#include <hsu_common.h>
#include <port.h>

/* Local include files. */
#include "hsu_al_task.h"

/*==============================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

enum
{
  /* The queue must be large enough to contain entries for all of its clients:
  ** Currently, only CDC adaptation layers utilize this queue, and they do so 
  ** only for Rx and Tx commands.
  ** For each CDC adaptation layer (e.g., hsu_al_ser, hsu_al_ecm) there is a 
  ** maximum of 4 entries per instance (up to 2 for Rx, in case of Rx double 
  ** buffering, and up to 2 for Tx if double buffering is supported for Tx).
  ** 
  ** For hsu_al_ecm, there is a maximum of 3 RmNET instances, so this totals to 
  ** 3*4=12 entries.
  **
  ** For hsu_al_ser:
  ** There is a maximum of 2 CDC/OBEX instances, and only one CDC/ACM instance.
  ** So this totals to 12 entries.
  **
  ** Total number of entries is therefore 24, and an elbow room of 8 entries is 
  ** added.
  */
  AL_TASK_COMMAND_QUEUE_SIZE = 32
};

/* NOTE - These signals must not conflict with the High-Speed USB stack 
** OS abstraction layer signals (defined in 'hsu_os_abstract_sync.h'). 
** This is because this task calls user-supplied functions which may in 
** turn call High-Speed USB stack core API, which may internally use the 
** OS abstraction layer API.
*/
enum
{
  AL_TASK_DOG_RPT_TIMER_SIG = 0x00000001,
  AL_TASK_CMD_PENDING_SIG   = 0x00000002
};

/*==============================================================================
  Typedefs
==============================================================================*/

/* AL task command type, as stored in the AL task 
** command queue.
*/
typedef struct 
{
  /* Function to execute. */
  hsu_al_task_cmd_func_type  command_func;

  /* Argument to be passed to the function. */
  void*                      command_arg;

} al_task_command_type;

/* AL task information. */
typedef struct 
{
  /* AL task Dog report timer.
  */
  rex_timer_type hsu_al_task_rpt_timer;

  /* A cyclic queue of commands to be executed by the AL task.
  */
  al_task_command_type  command_queue[AL_TASK_COMMAND_QUEUE_SIZE];

  uint32                command_queue_head;
  uint32                command_queue_tail;

} al_task_info_type;

typedef struct 
{
  al_task_info_type al_task_info;

} file_static_vars_type;

/*==============================================================================
  Externalized Variables
==============================================================================*/

rex_tcb_type   hsu_al_task_tcb;
unsigned char  hsu_al_task_stack[AL_TASK_STACK_SIZE_IN_BYTES];


/*==============================================================================
  File Static Variables
==============================================================================*/

static file_static_vars_type hsu_al_task;

static file_static_vars_type* const
  file_static_vars_ptr = &hsu_al_task;


/*==============================================================================

                             FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_AL_TASK_ENQUEUE_CMD

DESCRIPTION
  Enters a new command into the High-Speed USB adaptation layer
  command queue, for execution by the High-Speed USB adaptation layer task.

  This function is intended to be invoked only in interrupt context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_enqueue_cmd(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg)
{
  al_task_info_type* const
    al_task_info_ptr       = &(file_static_vars_ptr->al_task_info);

  al_task_command_type* const
    command_queue_ptr      = al_task_info_ptr->command_queue;

  uint32 const
    command_queue_head     = al_task_info_ptr->command_queue_head;

  uint32* const
    command_queue_tail_ptr = &(al_task_info_ptr->command_queue_tail);


  HSU_ASSERT(command_func != NULL);

  /* Ensure atomic operation of this function, as it modifies global
  ** structures.
  ** There are scenarios in which while an interrupt is executing this function,
  ** another higher priority interrupt is invoked and wants to execute this 
  ** function, too.
  */
  INTLOCK();
  TASKLOCK();

  /* Integrity check. */
  HSU_ASSERT(command_queue_ptr[*command_queue_tail_ptr].command_func == NULL);
  HSU_ASSERT(command_queue_ptr[*command_queue_tail_ptr].command_arg  == NULL);

  /* If the command queue is full, generate an assertion. */
  HSU_ASSERT (
               !(
                 ((*command_queue_tail_ptr + 1) == command_queue_head)    ||
                 ((*command_queue_tail_ptr == AL_TASK_COMMAND_QUEUE_SIZE - 1) && 
                  (command_queue_head == 0))
                )
             );

  command_queue_ptr[*command_queue_tail_ptr].command_func = command_func;
  command_queue_ptr[*command_queue_tail_ptr].command_arg  = command_arg;

  *command_queue_tail_ptr = *command_queue_tail_ptr + 1;

  /* Wrap around. */
  if (*command_queue_tail_ptr == AL_TASK_COMMAND_QUEUE_SIZE)
  {
    *command_queue_tail_ptr = 0;
  }

  TASKFREE();
  INTFREE();
  
  (void)rex_set_sigs(&hsu_al_task_tcb,
                     (rex_sigs_type)AL_TASK_CMD_PENDING_SIG);

}  /* hsu_al_task_enqueue_cmd */


/*===========================================================================
FUNCTION HSU_AL_TASK_DOG_RPT

DESCRIPTION
  This function kicks the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_al_task_dog_rpt(void)
{
  dog_report(DOG_HSU_AL_TASK_RPT); 

  (void)rex_set_timer(&(file_static_vars_ptr->
                          al_task_info.hsu_al_task_rpt_timer),
                      DOG_HSU_AL_TASK_RPT_TIME);
} /* hsu_al_task_dog_rpt */


/*===========================================================================
FUNCTION HANDLE_AL_TASK_COMMANDS

DESCRIPTION
  Executes the command queue for the High-Speed USB stack adaptation 
  layer task.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
static void handle_al_task_commands(void)
{
  al_task_info_type* const
    al_task_info_ptr       = &(file_static_vars_ptr->al_task_info);

  al_task_command_type* const
    command_queue_ptr      = al_task_info_ptr->command_queue;

  uint32* const
    command_queue_head_ptr = &(al_task_info_ptr->command_queue_head);

  uint32* const
    command_queue_tail_ptr = &(al_task_info_ptr->command_queue_tail);

  void*                      command_arg  = NULL;
  hsu_al_task_cmd_func_type  command_func = NULL;


  /* Acquire the Giant Mutex. */
  jsafe_enter();

  /* While the command queue is not empty,
  ** execute all the queued commands.
  */
  while(*command_queue_head_ptr != *command_queue_tail_ptr)
  {
    /* Copy command entry data. */
    command_func = command_queue_ptr[*command_queue_head_ptr].command_func;
    command_arg  = command_queue_ptr[*command_queue_head_ptr].command_arg;

    /* Clear the command entry. */
    command_queue_ptr[*command_queue_head_ptr].command_func = NULL;
    command_queue_ptr[*command_queue_head_ptr].command_arg  = NULL;

    *command_queue_head_ptr = *command_queue_head_ptr + 1;

    if (*command_queue_head_ptr == AL_TASK_COMMAND_QUEUE_SIZE)
    {
      *command_queue_head_ptr = 0;
    }

    /* calling the command function */
    command_func(command_arg);
  }  /* while (*command ...) */

  /* Release the Giant Mutex. */
  jsafe_leave();

} /* handle_al_task_commands */


/*===========================================================================
FUNCTION HSU_AL_TASK_MAIN

DESCRIPTION
  The Adaptation Layer Command Task's main function.
  
  Infinitely waits for new AL task commands to be entered into the command
  queue (this event is notified by a Rex signal) and executes them.
  Additionally, this function handles common AMSS signals (Dog 
  task signals, offline, stop).

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_main
(
  unsigned long param
)
{
  rex_sigs_type received_signals;

  al_task_info_type* const
    al_task_info_ptr  = &(file_static_vars_ptr->al_task_info);


  HSU_USE_PARAM(param);

  /*-------------------------------*/
  /* Command queue initialization. */ 
  /*-------------------------------*/

  /* Clear the command queue. */
  memset(al_task_info_ptr->command_queue,
         NULL,
         sizeof(al_task_info_ptr->command_queue));

  al_task_info_ptr->command_queue_head = 0;
  al_task_info_ptr->command_queue_tail = 0;


  /*-------------------*/
  /* Set the Dog timer */
  /*-------------------*/

  rex_def_timer(&(al_task_info_ptr->hsu_al_task_rpt_timer),
                &hsu_al_task_tcb,
                (rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG);

  (void)rex_set_timer(&(al_task_info_ptr->hsu_al_task_rpt_timer),
                      DOG_HSU_AL_TASK_RPT_TIME);

  /*----------------------------------------------*/
  /* Signal the TMC task that we are ready to go. */
  /*----------------------------------------------*/

  task_start((rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG,
               /* Dog report timer signal for task */

             DOG_HSU_AL_TASK_RPT,        
               /* Watchdog report ID               */

             &(al_task_info_ptr->hsu_al_task_rpt_timer)
               /* pointer to Dog report timer.     */
            );

  /*--------------------------------------*/
  /* Loop forever, handling task signals. */
  /*--------------------------------------*/

  for (;;)
  {
    received_signals = 
      rex_wait((rex_sigs_type)AL_TASK_CMD_PENDING_SIG   |
               (rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG |
               (rex_sigs_type)TASK_STOP_SIG             |  /* Stop the task */
               (rex_sigs_type)TASK_OFFLINE_SIG);          /* Go off line   */

    /* Handle a dog report signal. */
    if (received_signals & (rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG)
    {
      (void)rex_clr_sigs(&hsu_al_task_tcb,
                         (rex_sigs_type)AL_TASK_DOG_RPT_TIMER_SIG);

      /* Kick the watchdog */
      hsu_al_task_dog_rpt();
    }

    /* Handle a Stop signal. */
    if (received_signals & TASK_STOP_SIG)
    {
      (void)rex_clr_sigs(&hsu_al_task_tcb,
                         (rex_sigs_type)TASK_STOP_SIG);

      /* Signal back to TMC */
      task_stop();
    }

    /* Handle an Offline signal. */
    if (received_signals & TASK_OFFLINE_SIG)
    {
      (void)rex_clr_sigs(&hsu_al_task_tcb,
                         (rex_sigs_type)TASK_OFFLINE_SIG);

      /* Signal back to TMC */
      task_offline();
    }

    /* Handle a Command Pending signal. */
    if (received_signals & (rex_sigs_type)AL_TASK_CMD_PENDING_SIG)
    {
      (void)rex_clr_sigs(&hsu_al_task_tcb, 
                         (rex_sigs_type)AL_TASK_CMD_PENDING_SIG);

      handle_al_task_commands();
    }
  }  /* for (;;) */

}  /* hsu_al_task_main */

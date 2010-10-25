#ifndef HSU_AL_TASK_H
#define HSU_AL_TASK_H

/*==============================================================================

                          H S U _ A L _ T A S K . H


GENERAL DESCRIPTION
  High-Speed USB Implementation of the Adaptation Layer Command Task.

  The Adaptation Layer Command Task is a Rex task in charge of executing 
  High-Speed USB adaptation layer commands.
  An adaptation layer command is a code section which needs to be executed
  after the occurrence of an interrupt, but can't be executed within
  the interrupt context itself (for an example, when this code section 
  is blocking). Therefore, the interrupt service routine enqueues a command 
  in the commands queue. Later on, another task, the adaptation layer task, 
  dequeues this item from the command queue and executes the command outside 
  of the interrupt context.

  This task manages a cyclic queue of commands. Each queue entry contains
  a function address and an actual parameter for this function, which is 
  of type void*.
  When commands are inserted into the queue (using the 'hsu_al_task_enqueue_cmd'
  function), a signal is sent to this task, and wakes it up. Then, this task 
  processes all the queued commands in a FIFO order. For each queued command,
  this task calls the supplied function, passing it its argument.

  While this task is processing command queue items, the Giant mutex is locked.
  When this task finishes processing the queue (i.e., it becomes empty 
  again), this task releases the giant mutex and resumes waiting for its
  signal.

  The command queue is implemented using a cyclic buffer which has a head index
  and a tail index. The tail index points to the location of the next inserted
  item, and the head points to the location of the next popped item.
  When the head == tail, the queue is empty, and when head == tail + 1, the 
  queue is full. This way, the queue may be managed without a mutex protection
  since no item counter is needed, and there is no simultaneous access to 
  the same end of the queue.


EXTERNALIZED FUNCTIONS
  Externalized to outside of the High-Speed USB stack adaptation layer:
    hsu_al_task_main

  Externalized internally in the High_Speed USB stack adaptation layer:
    hsu_al_task_enqueue_cmd


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The High-Speed USB adaptation layer task needs to be created before 
  using the API defined in this file.

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
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

#include <rex.h>

/*==============================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

#define HSU_AL_TASK_NAME "HSU AL TASK"

enum
{
  AL_TASK_STACK_SIZE_IN_BYTES = 2048
};

extern rex_tcb_type   hsu_al_task_tcb;
extern unsigned char  hsu_al_task_stack[AL_TASK_STACK_SIZE_IN_BYTES];

/*==============================================================================
  Typedefs
==============================================================================*/

typedef void (*hsu_al_task_cmd_func_type) (void*);

/*==============================================================================
  Externalized Variables
==============================================================================*/


/*==============================================================================

                           FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_AL_TASK_ENQUEUE_CMD

DESCRIPTION
  Enters a new command into the High-Speed USB adaptation layer
  command queue, for execution by the High-Speed USB adaptation layer task.

  This function is NOT re-entrant. Invoking this function twice
  simultaneously might yield unpredictable results.

  This function is intended to be invoked only by a High-Speed USB 
  interrupt (it is assumed that High-Speed USB interrupts do not
  interrupt each other) or by testing API.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_enqueue_cmd(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg);

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
);  /* hsu_al_task_main */

#endif /* HSU_AL_TASK_H */

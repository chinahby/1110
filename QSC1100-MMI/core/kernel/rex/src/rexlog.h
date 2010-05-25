#ifndef REXLOG_H
#define REXLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  R E X

DESCRIPTION
  This header file contains the definitions of log packets for task
  profiling, memory debug, code coverage, core dump, etc.
 
Copyright (c) 2002      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/kernel/rex/main/latest/rexlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/04   tbg     Added ENUM for rex_core log, to simplify referencing the
                   various registers.
07/20/02    gr     Created

===========================================================================*/

/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "customer.h"
#include "comdef.h"
#include "log.h"
#include "memheap.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/*============================================================================

PACKET   Memory Debug Information

ID       LOG_MEMDEBUG_C

DESCRIPTION
  Contains memory debug information.

  alloc_num:       A unique ID for this memory allocation operation

  mem_ptr:         A pointer to the allocated memory

  size:            The size in bytes of the allocated memory

  alloc_tcb_id:    The ID of the task that performed this particular
                   allocation

  alloc_line_num:  The line number in the file where the call to allocate
                   memory was made 

  alloc_heap_used: The number of bytes used in the heap after the memory
                   allocation

  free_tcb_id:     The ID of the task that freed the allocated memory. This
                   will be zero if the memory has not been freed yet.

  free_line_num:   The line number in the file where the call to free
                   memory was made. This will be zero if the memory has not
                   been freed yet.

  free_heap_used:  The number of bytes used in the heap just before the memory
                   was freed.  This will be zero if the memory has not been
                   freed yet.
 
  status:          A field of type mem_debug_status_type that indicates the
                   status of the allocation. The possible values are
                   MEM_DEBUG_STATUS_OK             = 1
                   MEM_DEBUG_STATUS_MULTIPLY_FREED = 2
                   MEM_DEBUG_STATUS_BAD_ADDRESS    = 3
                   MEM_DEBUG_STATUS_OVERRUN        = 4
                   MEM_DEBUG_STATUS_POSSIBLE_LEAK  = 5

  heap_name:       A null-terminated string containing the name of the heap
                   from which memory was allocated

  alloc_file_name: A null-terminated string containing the name of the file
                   from which the call to allocate memory was made.

  free_file_name:  A null-terminated string containing the name of the file
                   from which the call to free memory was made. This string
                   will be empty if the memory has not been freed yet.

============================================================================*/

LOG_RECORD_DEFINE(LOG_MEMDEBUG_C)

   uint32   alloc_num;
   void    *mem_ptr;
   uint32   size;
   uint32   alloc_tcb_id;
   uint32   alloc_line_num;
   uint32   alloc_heap_used;
   uint32   free_tcb_id;
   uint32   free_line_num;
   uint32   free_heap_used;
   uint32   status;
   char     heap_name[1];         /* variable length field */
   char     alloc_file_name[1];   /* variable length field */
   char     free_file_name[1];    /* variable length field */

LOG_RECORD_END


/*============================================================================

PACKET   System Level Task Profiling Information

ID       LOG_SYSPROFILE_C

DESCRIPTION
  Contains system-level information such as the number of tasks and their
  ids, total time samples elapsed etc.

  total_num_tasks:    Total number of tasks in the system (in REX's task list).

  task_id:      A unique identifier for a task.

  stack_size:   The stack size (in number of bytes) for a task. For an ISR,
                this will be the size of the system stack.

  task_sl:      Stack limit value. This value will be limit of the system
                stack for an ISR.

  max_intlock_time: The maximum time for which a task or ISR has kept
                interrupts locked.

  max_intlock_lr: The value of the link register during the call to
                rex_int_lock that corresponds to the maximum lock time for a
                task or ISR. This indicates from where the call to
                rex_int_lock was made.

  task_name:    Null-terminated string name of task or ISR up to a max of 
                LOG_REX_SYSPROFILE_TASK_NAME_LEN characters - 1 

============================================================================*/

#define LOG_REX_SYSPROFILE_TASK_NAME_LEN 16  

LOG_RECORD_DEFINE(LOG_SYSPROFILE_C)

   uint32 total_num_tasks;
   uint32 task_id;
   uint32 stack_size;
   uint32 stack_limit;
   uint32 max_intlock_time;
   uint32 max_intlock_lr;
   char   task_name[LOG_REX_SYSPROFILE_TASK_NAME_LEN]; 

LOG_RECORD_END


/*============================================================================

PACKET   Per Task Profiling Information

ID       LOG_TASKPROFILE_C

DESCRIPTION
  Contains profiling information for a specific task. The same packet is
  also used to send profiling information about ISRs. Many of the fields
  will be zero or null for ISRs because the fields do not make sense in
  that context.

  task_id:    Unique identifier for the task or ISR.

  sp:         The stack pointer stored in the TCB of the task (except for
              the current task, for which we return the contents of the
              stack pointer register)
  
  sigs:       The value of the sigs field from the task's TCB. This represents
              the mask of signals set for the task.

  wait:       The value of the wait field from the task's TCB. This represents
              the mask of signals on which the task is currently waiting.

  pri:        The value of the pri field from the task's TCB. This denotes
              the priority of the task.

  time_samples: A measure of the CPU time used by the task. This field contains
              the number of time samples during which this task was running,
              if this is available. Otherwise this field is set to the
              value of the slices field in the TCB. The slices field denotes
              the number of times a task has been scheduled. Bit 2 of the
              info_mask field determines which of the two meanings applies.

  total_samples: The total number of time samples elapsed so far, or the total
                 number of scheduling operations performed by REX so far. Bit 2 
                 of the info_mask field determines which of the two meanings
                 applies.
	
  flags:
                 Bit [ 0 ] - interrupt state
                             1 - interrupts are locked
                             0 - interrupts are not locked
									
                 Bit [ 1 ] - suspend state
                             1 - task is suspended (by way of a request
                                 via rex_suspend_task)
                             0 - task is not suspended
									 
                 Bit [ 2 ] - slice type
                             0 - time_samples field contains the value of the
                                 slices field in the TCB. The slices field is a
                                 count of the number of times a task has been
                                 scheduled, and is a very rough measure of the
                                 CPU time allotted to a task.
                             1 - time_samples field contains more accurate profiling
                                 information generated by using a timer to
                                 measure how long the task has run. The bit is 0
                                 typically if the build does not have software task
                                 profiling turned on.
============================================================================*/

#define LOG_TASKPROFILE_REX_INT_STATE_BIT  0x01
#define LOG_TASKPROFILE_REX_SUSPENDED_BIT  0x02
#define LOG_TASKPROFILE_REX_SLICE_TYPE_BIT 0x04

LOG_RECORD_DEFINE(LOG_TASKPROFILE_C)

   uint32    task_id;
   uint32    sp;
   uint32    sigs;
   uint32    wait;
   uint32    pri;
   uint32    time_samples;
   uint32    total_samples;
   uint8     flags;
   
LOG_RECORD_END


/*============================================================================

PACKET   Core Dump Information

ID       LOG_COREDUMP_C

DESCRIPTION
  Contains core dump information.
  
  regs: contents of the ARM registers, ordered as follows
        regs[0]  to regs[16] -> supervisor mode registers, spsr
        regs[17] to regs[19] -> r13, r14, spsr in system/user mode
        regs[20] to regs[22] -> r13, r14, spsr in irq mode
        regs[23] to regs[25] -> r13, r14, spsr in abort mode
        regs[26] to regs[28] -> r13, r14, spsr in undef mode
        regs[29] to regs[36] -> r8 to r14, spsr in fiq mode
        regs[37] to regs[39] -> r13, r14, spsr in java mode mode

  cpsr: the current program status register.

  curr_tcb_ptr: pointer to the TCB of the current task.

  tasklock_status: 1 if TASKLOCK is in effect, 0 otherwise.

  stack_frame: the contents of the last frame pushed on to the current
               stack. if the current mode is system mode, the frame will
               be taken from the system stack. if the current mode is
               supervisor mode, the frame will be taken from the stack of
               the current task.
 
  errlinenum:  the line number of the ERR_FATAL call. This field may be zero
               if the core dump was caused by something other than an
               ERR_FATAL.

  errparms:    The parameters to ERR_FATAL. All the entries in this array may
               be zero if the core dump was caused by something other than an
               ERR_FATAL.

  errfile:     a null-terminated string containing the name of the file
               from which ERR_FATAL was called. If the core dump was caused
               by something other than an ERR_FATAL, this string may be empty.

  errmsg:      a null-terminated string containing the string passed in as a
               parameter to ERR_FATAL. If the core dump was caused by
               something other than an ERR_FATAL, this string may be empty.

============================================================================*/

#define LOG_STACK_FRAME_SIZE  16
#define LOG_ERR_NUM_PARMS      4

enum
{
  REX_SVC_R0 = 0,
  REX_SVC_R1,
  REX_SVC_R2,
  REX_SVC_R3,
  REX_SVC_R4,
  REX_SVC_R5,
  REX_SVC_R6,
  REX_SVC_R7,
  REX_SVC_R8,
  REX_SVC_R9,
  REX_SVC_R10,
  REX_SVC_R11,
  REX_SVC_R12,
  REX_SVC_SP,
  REX_SVC_LR,
  REX_SVC_SPSR,
  REX_SVC_PC,
  REX_SYS_SP,
  REX_SYS_LR,
  REX_IRQ_SP,
  REX_IRQ_LR,
  REX_IRQ_SPSR,
  REX_ABT_SP,
  REX_ABT_LR,
  REX_ABT_SPSR,
  REX_UDF_SP,
  REX_UDF_LR,
  REX_UDF_SPSR,
  REX_FIQ_R8,
  REX_FIQ_R9,
  REX_FIQ_R10,
  REX_FIQ_R11,
  REX_FIQ_R12,
  REX_FIQ_SP,
  REX_FIQ_LR,
  REX_FIQ_SPSR,
  REX_JVA_SP,
  REX_JVA_LR,
  REX_JVA_SPSR,
  SIZEOF_LOG_COREDUMP_REGISTERS
};

LOG_RECORD_DEFINE(LOG_COREDUMP_C)

   uint32   regs[SIZEOF_LOG_COREDUMP_REGISTERS];
   uint32   cpsr;
   uint32   curr_tcb_ptr;
   uint32   tasklock_status;
   uint32   stack_frame[LOG_STACK_FRAME_SIZE];
   uint32   errlinenum;
   uint32   errparms[LOG_ERR_NUM_PARMS];
   char     errfile[1]; 

LOG_RECORD_END


#endif /* REXLOG_H */


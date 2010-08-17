/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       R E X   A R M   P O R T

GENERAL DESCRIPTION
   This is the ARM Port of REX (Real Time Executive).
   It is a simple, efficient real-time executive for the ARM7TDMI processors.
   It supports multi-tasking according to a priority  scheduling algorithm,
   and provides a small set of services for task suspension and reactivation.
   All REX services follow the APCS conventions for function call/return and
   parameter passing.

 TASKS

   REX treats tasks as independently dispatchable entities, each having
   its own stack, signal flags and priority which collectively constitute
   the context of the task. Each task has an associated data structure,
   called the task context block (TCB), which REX uses for keeping track of
   the task's context. REX maintains a list of tasks in which all task TCBs
   are linked in order of decreasing task priority. The first TCB in the
   task list belongs to the highest priority task, while the last TCB be-
   longs to the lowest priority task (i.e., the kernel task whose priority
   is zero).

   REX allows an arbitrary number of tasks to be created dynamically at
   any time during execution. Realistically, however, REX's performance
   deteriorates slightly with each added task, so care is called for in
   keeping the number of tasks to a minimum. Ultimately, the task load
   which REX can support depends on the processor chosen, the processor
   clock speed and the interrupt characteristics of the particular system.

 TASK STACKS

   As mentioned above, each task has its own stack which is used when-
   ever the task is running. When the task is suspended (for example, to
   run another task or to service an interrupt), the task's registers
   are pushed onto the task's stack, and then the top of stack pointer
   is saved in the task's TCB. Later, when the task is selected for
   running again, the top of stack pointer is restored from the task's TCB,
   the task's registers are popped off it's stack, and task execution
   resumes where it left off. The process of task switching is virtually
   invisible to tasks.

 TASK SIGNALS

   Each task has associated with it 32 general purpose signal flags which
   are kept as part of the task's context in the task's TCB. These signals
   are used to notify the task that some kind of event has occurred. A
   task's signals may be set or cleared by any task, and by any interrupt
   handler.

   A task can poll the current state of its signals at any time. Moreover,
   a task can also suspend its own execution pending the setting of one or
   more of its signals. So, for example, Task A may suspend waiting for
   signal 1 to be set. At some later time, Task B  or an interrupt handler
   can wake up Task A by setting signal 1 for A. Once awakened, Task A is
   again eligible for execution, although it will actually run only if it
   is the highest priority task that is ready to run.

   Notice that task signals are treated as a 32 bit wide mask. This means
   that all 32 signals for a task may be set or cleared in a single op-
   eration and, similarly, a task may suspend on up to 32 different signals
   at the same time, waking up when any one of them is set.

    The important points to keep in mind about signals are:

     1. 32 signals are defined on a per task basis. For purposes of setting,
        clearing and waiting, signals are treated as 32 bit masks.

     2. Signals are general purpose flags whose meanings may be defined
        any way the user sees fit.

     3. A task may optionally suspend execution pending the setting of
        particular signal(s), presumably by another task or an interrupt
        handler. The emphasis here should be on the word 'optionally'
        since setting a signal for a task that is not suspended on the
        signal does not directly affect the execution state of that task.


 TASK PRIORITIES AND SCHEDULING

   Each task has associated with it a priority that is stored in its TCB.
   Priorities range from 1 to 2^32-1, with 1 being the lowest priority and
   2^32-1 being the highest. REX reserves priority value zero for use with
   the kernel task.

   When scheduling, REX ALWAYS chooses the highest priority ready task,
   i.e., the highest priority task that is not suspended waiting on sig-
   nal(s). REX will continue to execute this task until it voluntarily
   suspends, or until an interrupt occurs which reactivates a higher pri-
   ority task by setting signals for the task. In the case where all user
   defined tasks suspend, the kernel task, defined by REX, will execute.

   REX also provides a mechanism by which a task can raise or lower its
   own priority dynamically.

===========================================================================

  INTERRUPTS

    When an interrupt occurs, the corresponding interrupt handler must
    immediately save the register context of the interrupted entity,
    which may be a task or a lower priority interrupt.

===========================================================================

  TIMERS

    Timer-related functions are now in rextime.c.

===========================================================================

  CRITICAL SECTIONS

    Critical sections are used to provide mutual exclusion between tasks,
    while allowing other un-involved tasks to still be scheduled as well
    as allowing interrupts to happen.  A critical section variable is
    declared globally, and must be initialized via a function call before
    it can be used.  Tasks can henceforth "enter" and "leave" the critical
    section.

    If a task attempts to enter a critical section while it is being held
    by a different task, the attempting task will be blocked until the
    critical section becomes available.

    REX keep a list of waiting tasks attached to the critical section
    variable. The list itself is constructed by linking pointers that appear
    within the TCBs of the tasks themselves.

    When a task releases a critical section, REX will select the highest
    priority waiting task to schedule.

=============================================================================

===========================================================================

  ASYNCHRONOUS PROCEDURE CALLS

    Asynchronous Procedure Calls provide a means to postpone the execution
    of a function until the REX is executing in the designated context.
    Queueing an APC for a task causes the APC to be executed the next time
    the task is given CPU.  APC's cannot be queued for the currently
    running task.

=============================================================================

EXTERNALIZED FUNCTIONS
  rex_init_crit_sect
     Initilalzes a critical section, setting elements to 0, NULL, etc.

  rex_enter_crit_sect
     Acquires the lock on a critical section, or suspends the calling task
     until the critical section is available.

  rex_leave_crit_sect
     Releases the lock on a critical section and grants it to a waiting task
     (if there is one waiting).

  rex_clr_sigs
     Clears a specified mask of signals for a specified task.

  rex_set_sigs
    Sets a specified mask of signals for a specified task. If the specified
    task is waiting on any of the signals set, it will be again be eligible
    for execution.

  rex_get_sigs
    Returns the state of the signals for a specified task.

  rex_wait
    Suspends the calling task until at least one signal of a specified
    signal mask is set for the task. If any signal of the signal mask is
    already set prior to calling rex_wait, an immediate return is made
    from the function and the calling task continues to execute.

  rex_def_task
    Creates a new task using a specified TCB, stack, priority, entry point
    and task parameter. Task will start running only if it is the highest
    priority ready task.

  rex_def_task_ext
    An extended version of rex_def_task that adds two parameters to the
    argument list.  The first is a string for the task name, the second
    is a boolean variable that indicates if the task should be started in
    "suspended" mode.  If that option is chosen, the task will not actually
    run until a rex_resume_task() call is issued.

  rex_suspend_task
    Suspends a REX task.

  rex_resume_task
    Allows a suspended task to be scheduled.

  rex_kill_task
    Kill a task by yanking the TCB from the list of active tasks.  If the
    same TCB is used in a future call to rex_def_task, the task will be
    started from the beginning, in other words, no state information from
    the previous existence will still be around.

  rex_self
    Returns a pointer to the TCB of the calling task.
    to obtain the address of its own TCB.

  rex_set_pri
    Changes the priority of the calling task to a specified priority.

  rex_task_pri
    Sets the priority of the task chosen to a determined value.

  rex_get_pri
    Returns the current priority of the calling task.

  rex_init
    Initializes REX and fires up a user startup task using a specified TCB,
    stack, priority, entry point and startup parameter.

  rex_set_interrupt_vector
    Given the id v of an installable vector and a pointer to the routine to
    be called when the interrupts occurs, this function installs the routine.

  rex_enable_interrupt
    Enables the interrupt associated with id v.

  rex_disable_interrupt
    Disables the interrupt associated with id v.

  rex_get_version_number
   Returns a REX version number. The version number returned is a
   decimal number in which the least significant two digits indicate
   the minor version number and the remaining digits (at most two)
   indicate the major version number. For instance, if the value
   returned is 307, the major version number is 3 and the minor version
   number is 7.

  rex_clr_profile
    Clears stored profiling information. Clears the numticks and
    rex_IRQnumticks fields in each TCB in the task list. The rex_pnumticks
    does not need to be cleared because it would be in sync with
    rex_time_tick2.

  rex_init_profile
    Performs initialization necessary for software profiling.

  rex_enable_profile
    Enables software profiling.

  rex_disable_profile
    Disables software profiling.

  rex_get_profile
    Retrieves software profiling information. Traverses rex_task_list and
    gets the numticks field from each TCB in the task list. In addition,
    gets rex_IRQnumticks as well.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  rex_init must be called to initialize the operating system.

Copyright (c) 1998,1999 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2000,2001 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2002-2008 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rex.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   cz      Fixed a priority inverion issue in crit_sect..
06/11/08   cz      Merged changes needed for QSC1100.
07/13/07   cz      Added support for DAC based memory protection feature from umr.
07/25/06    pc     Added support for FEATURE_CPU_BASED_FLOW_CONTROL.
03/07/06    ck     Initialized dpc_param flags.
10/14/05    ck     Merged in the fix to not lock interrupts during a DPC call
                   Cleaned up REX_INTLOCK to match with REX_INTFREE
09/21/05    cr     Added PC logging functionality as an overloaded
                   functionality of stack checked builds. This requires a
                   value in r10 stack limit register that will always result
                   in overflow function being called.
03/09/05    ck     Merged in the fix to not lock interrupts when APC call is
                   executed.
11/17/04    gr     Fixed a bug that was preventing rex_kill_task_ext from
                   releasing critical sections correctly.
11/11/04    rp     Added Progressive boot support. New functions
                   rex_get_best_task and rex_resume_task_from_abort.
08/11/04    is     Externalized rex_kernel_tcb for REX profiling.
06/23/04    gr     Added IPC support in rex_kill_task_ext.
06/03/04   gfr     Rex intlock profiling support.
05/12/04    gr     Fixed a bug in the interaction of critical sections and
                   APCs.
05/12/04    gr     Changed REX_ASSERT to ASSERT.
04/16/04    gr     Minor modification to rex_set_best_task to make it more
                   efficient.
03/04/04    gr     Added IPC support.
11/25/03    gr     Deleted the debug versions of the Rex memory allocation
                   functions.
10/28/03    gr     Made rex_nest_depth global, since it is used elsewhere in
                   DMSS.
09/22/03   ksu     A task can now enter more than one critical section.
07/02/03    gr     Modified rex_apc_prolog to handle user mode.
06/20/03    gr     Modifications to get FEATURE_REX_PROFILE to work. Added
                   functions to disable and enable task switching.
04/15/03    cr     Fixed critical section bug in case where there is only a
                   single task on the critical section wait list.
02/28/03   eam     Masked off unused bits from SPSR in rex_apc_prolog, to
                   ensure Jazelle bit is not set during execution of an APC.
09/04/02    gr     Added the rex_def_task_ext2 function that allows the user
                   to specify a dog report value. rex_autodog_enable() and
                   rex_autodog_disable() now return the previous state.
07/16/02    gr     Updated REX version number.
07/16/02    gr     Renamed rex_autodog_init to rex_autodog_enable. Added
                   function rex_autodog_disable.
07/15/02    gr     Moved timer-related functions to rextime.c.
07/10/02    gr     Minor change to suppress a warning in rex_decrement_timers.
07/03/02    gr     Added support for pausing the monitoring of a task by Dog if
                   the task is blocked by a call to rex_wait or a call to
                   rex_enter_crit_sect. A task must call rex_autodog_init in
                   order to use this functionality. Monitoring by Dog resumes
                   when the task is made runnable.
06/04/02    gr     Fixed a comment.
05/29/02    gr     Fixed a featurization problem.
05/23/02    gr     Updated critical section code to allow a task to seize a
                   critical section more than once. Added code to initialize
                   the err_num field in the tcb. Added a function to allow
                   access to the internals of REX.
03/29/02   plm     Initialized tcb.leds field for rex_idle_task and rex_dpc_task
                   in rex_init() to support TIMETEST.
03/27/02    gr     Removed all references to rex_defer_sched, which is no
                   longer used.
03/26/02    gr     Fixed a bug in REX timer management.
03/21/02    gr     Cosmetic changes. Updated some comments, fixed indentation
                   problems and updated the copyright notice.
03/20/02    dc     Added support for FEATURE_SOFTWARE_PROFILE.
03/13/02    gr     Added function rex_get_version_number.
10/02/01    gr     Added support for memory debug functionality. Fixed a
                   stack-size bug in rex_def_task. Added assertions to catch
                   calls to the REX memory allocation functions from interrupt
                   context.
08/21/01    gr     Added function rex_exit.
08/15/01    gr     New APC implementation that is simpler than the old one and
                   is not dependent on the exact sequence of actions in
                   IRQ_Handler. Support for system signals.
06/06/01    gr     Modifications to enable the use of the extended timer
                   interface without turning on dynamic memory support.
12/19/00   jct     Merged MSM5000, MSM5200, MSP1000 versions of REX
06/08/00   rmd     Added MSM5000_IRAM_FWD #ifndefs around the functions
                   that will be copy into Internal RAM.
02/02/00   jct     Integrated preemptive interrupt changes
02/02/00   tim     Support for PIC.
1/??/00    tim     Added priority inversion prevention to critical sections.
12/16/99   tim     Introduction of feature whereby a task starts out suspended,
                   also added task name to TCB.
12/07/99   tim     Introduction of critical sections construct for providing
                   mutual exclusion between tasks.
11/30/99   tim     Added rex_kill_task() function, changed rex_task_preamble
                   so that it will invoke rex_kill_task() if a task returns.
07/07/99   jct     Integrated Windows NT support
05/19/99    ms     Changes in rex_report_stack_overflow.
04/09/99    ms     Lint cleanup.
03/31/99    ms     Fixed rex_set_sigs to correctly compare priorities.
03/19/99    ms     Defined a new function rex_report_stack_overflow.
02/19/99    ms     Now running in Supervisor mode.
                   Changed interface of rex_init and rex_def_task. The stack
                   pointers in their parameter lists are void *.
                   Using a new type rex_stack_size_type for stack sizes.
02/12/99    ms     Added support for TIMETEST port.
02/05/99    sk     Reduced Kernel Task stack size to 40 words.
02/04/99    ms     Support for FEATURE_SI_STACK_WM.
12/02/98    ms     rex_task_list and rex_timer_list changed to headnodes.
11/30/98   jkl     Added new functions. Revised for code standards.
11/04/98   jkl     Revised for code standard (rex types)
09/10/98   jkl     Adapted for new context frame (Delta Rex)
05/27/98   jbh     Created module based on rex.asm 1.7
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#define REX_C

#include "customer.h"

#include "comdef.h"
#include "rex.h"

#ifdef FEATURE_REX_CRIT_SECT_DEBUG
#include "msg.h"
#endif /*FEATURE_REX_CRIT_SECT_DEBUG*/

#ifdef T_WINNT
#error code not present
#else
#include "armasm.h"
#include "arm.h"
#endif

#ifdef FEATURE_ZREX
#error code not present
#endif

#include <string.h>

#ifdef FEATURE_SI_STACK_WM
#error code not present
#endif

#ifdef TIMETEST
#include "timetest.h"
#endif

#include "task.h"

#ifdef FEATURE_REX_IPC
#include "ipctypes.h"
#include "ipcns.h"
#include "pq.h"
#endif

#include "assert.h"

#ifdef FEATURE_DAC_MEMORY_PROTECTION
#include "armmmu.h"
#endif /* FEATURE_DAC_MEMORY_PROTECTION */

#ifdef FEATURE_REX_STAT
#error code not present
#endif

#ifdef FEATURE_REX_PROFILE
extern void rextp_remove_task (rex_tcb_type *tcb_ptr );
#endif /* FEATURE_REX_PROFILE */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
/* If a test scenario expects phone resets, then init trap_hw_reset to 0.
 * If a test scenario does not expect phone resets,
 * then init trap_hw_reset to 1.
 */
int trap_hw_reset = 0;
int bootloader_has_run = 1;
#endif /* defined(FEATURE_LOG_FUNCTIONS) && defined(FEATURE_STACK_CHECK) */

/*---------------------------------------------------------------------
** Defines
**--------------------------------------------------------------------*/
#ifdef FEATURE_REX_APC
#define REX_APC_EXTRA_STACK_SIZE (sizeof(rex_context_frame_type) * 3)
   /* Extra space that we assume is needed on a thread's stack to
   ** make an APC function call. This is a conservative estimate that
   ** assumes the possibility of an interrupt frame and a context
   ** frame being pushed on the thread's stack before the APC is
   ** actually called.
   */
#endif

/*----------------------------------------------------------------------
** Kernel stack size in bytes.
**--------------------------------------------------------------------*/
#ifndef REX_KERNEL_STACK_SIZE
#define REX_KERNEL_STACK_SIZE 1024
#endif

/*----------------------------------------------------------------------
** The interrupt stack pointer (not static - the scheduler is not
**                              in this module)
**--------------------------------------------------------------------*/
rex_stack_word_type *  rex_int_stack;

/*-----------------------------------------------------------------------
** The current task pointer (not static - the scheduler is not
**                              in this module)
**---------------------------------------------------------------------*/
rex_tcb_type  * rex_curr_task;

/*-----------------------------------------------------------------------
** The highest priority ready task (not static - the scheduler is not
**                              in this module)
** Notes: rex_best_task is set to the desired task to swap in prior
**  to making calls to rex_sched().  However, in some cases, such as
**  while a TASKLOCK is in effect or while ISRs are being processed,
**  rex_sched() will not be allowed to actually make the swap until
**  sometime later.  As such, rex_best_task could be written to several
**  times before rex_sched() happens. So, in all places when making
**  the decision to update rex_best_task, the decision needs to be
**  based on a comparison with what is already in rex_best_task, NOT
**  with rex_curr_task.  rex_best_task should be updated only when a
**  new task is found of higher priority than what rex_best_task has
**  already been set to.
**---------------------------------------------------------------------*/
rex_tcb_type  * rex_best_task;


/*-----------------------------------------------------------------------
   The task that will be scheduled next.This is normally the bext task
   except when a page fault happens when tasks are locked. In this case
   we cannot schedule the best task. The aborted task will be scheduled
   in this case

**---------------------------------------------------------------------*/
rex_tcb_type  * rex_task_to_schedule;
/*-----------------------------------------------------------------------
  The CPSR value saved. This is needed so the rex_sched_force() can save
  the CPSR value and invoke rex_sched_common().
**---------------------------------------------------------------------*/
uint32        rex_cpsr_value;
/*----------------------------------------------------------------------
** The task_list head node.
**--------------------------------------------------------------------*/
#ifdef FEATURE_ZREX
#error code not present
#else
rex_tcb_type rex_task_list;
#endif

/* --------------------------------------------------------------------
** Count of REX Tasks
** -------------------------------------------------------------------- */
int rex_num_tasks = 0;

/*----------------------------------------------------------------------
** The kernel task which is the last member on the task list
**--------------------------------------------------------------------*/
#ifdef FEATURE_REX_PROFILE
rex_tcb_type rex_kernel_tcb;
#else
static rex_tcb_type rex_kernel_tcb;
#endif
rex_stack_word_type rex_kernel_stack[ REX_KERNEL_STACK_SIZE / sizeof(rex_stack_word_type) ];

#ifdef FEATURE_REX_DPC
/*-------------------------------------------------------------------
** Deferred Procedure Calls
**    Definitions and data to support DPCs
**-----------------------------------------------------------------*/
void rex_dpc_task( unsigned long param );

static rex_dpc_item_type    rex_dpc_ring[ REX_DPC_NUM_RING_ITEMS ];
   /* DPC ring buffer. Holds currently queued DPCs.
   */
static int                  rex_dpc_rd_index = 0;
   /* DPC ring buffer read index.
   */
static int                  rex_dpc_wr_index = 0;
   /* DPC ring buffer write index.
   */
static rex_tcb_type         rex_dpc_tcb;
   /* TCB for DPC task.
   */
#define REX_DPC_THREAD_PRI  0x400000C0
rex_stack_word_type rex_dpc_stack[ REX_DPC_STACK_SIZE / sizeof(rex_stack_word_type) ];
   /* Stack for the DPC task
   */
#endif

#ifdef FEATURE_REX_DYNA_MEM
#include "memheap.h"
/*-------------------------------------------------------------------
** Dynamic Memory Support
**    Definitions and data to support rex_malloc/realloc/calloc/free
**-----------------------------------------------------------------*/
#ifndef REX_MEM_BUFFER_SIZE
#define REX_MEM_BUFFER_SIZE 2048
   /* The size of the REX memory buffer in bytes.
   */
#endif
static mem_heap_type rex_heap;
   /* The primary REX heap.
   */
static char rex_mem_buffer[ REX_MEM_BUFFER_SIZE ];
   /* The REX memory buffer used by the heap manager
   */
void rex_mem_init( void );
   /* Function that initializes rex_heap.
   */
#endif

/*----------------------------------------------------------------------
** The vectors that contain entrypoints to user defined ISR handlers
**--------------------------------------------------------------------*/
void   (* rex_irq_vector) (void);
void   (* rex_fiq_vector) (void);

/*---------------------------------------------------------------------
** External declarations
**--------------------------------------------------------------------*/
extern void *rex_set_cpsr( dword );
extern void dog_monitor_pause( int dog_report_val );
extern void dog_monitor_resume( int dog_report_val );

/*---------------------------------------------------------------------
** The interrupt nesting level - 0 means a task is being executed, 1
** means a single interrupt, 2 means a nested interrupt, etc.
**--------------------------------------------------------------------*/
word rex_int_nest_level;

/*---------------------------------------------------------------------
** A rex_tick counter for debugging
**--------------------------------------------------------------------*/
#ifdef REX_DEBUG
#error code not present
#endif

/*---------------------------------------------------------------------
** Shadow of value last written to Timetest port.
**--------------------------------------------------------------------*/
#if defined TIMETEST
word rex_led_buf  =  0;
#endif


#if defined FEATURE_SOFTWARE_PROFILE
byte  rex_profile_enabled = 0;    /* turns profile on/off */
dword rex_IRQnumticks = 0;
word  rex_pnumticks = 0;
dword rex_time_tick2 = 0;
#endif


/*---------------------------------------------------------------------
** Variables for supporting TASKLOCK/TASKFREE, macros that turn off
** context switching (but not interrupts) when mutual exlcusion is
** needed.
**--------------------------------------------------------------------*/

int rex_sched_allow = TRUE;    /* turns sched on/off */

unsigned int rex_nest_depth = 0;  /* supports nesting of TASKLOCK/
                                   * FREE.
                                   */

static int rex_init_complete = 0; /* Set to 1 after rex_init */

#ifdef FEATURE_REX_TICK_CONTROL
#error code not present
#endif

/* --------------------------------------------------------------------
** Program Counter Dump
**   Causes the scheduler to write the PC periodically (on a periodic
**   FIQ interrupt).  This information can then be post processed
**   against an address map file to obtain the functions in which the
**   most execution time is spent.
** -------------------------------------------------------------------- */
#ifdef FEATURE_REX_PC_DUMP
#error code not present
#endif

/* --------------------------------------------------------------------
** Profiling Information for Threads
** -------------------------------------------------------------------- */
#ifdef FEATURE_REX_PROFILE
/* Stores the total number of time samples that have elapsed since
** profiling was enabled.
*/
unsigned long rex_total_time_samples = 0;

/* Boolean that determines whether profiling is enabled.
*/
unsigned long rex_profiling_is_enabled = 0;

/* Boolean state variable indicating whether or not the rex is
 * currently involved a kernel intlock.
 */
unsigned long rex_intlocked = 0;

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
rex_sleep_report_type rex_sleep_report = {0};

unsigned long rex_sleep_reporting_is_enabled = 0;

unsigned long rex_profiling_to_be_disabled = 0;
#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */
#endif

#ifdef FEATURE_DAC_MEMORY_PROTECTION
static rex_crit_sect_type rex_dac_crit_sect;
#endif /* FEATURE_DAC_MEMORY_PROTECTION */ 

/* --------------------------------------------------------------------
** Rex version number. A decimal number of the form yxx or yyxx, where
** y denotes the major version number and x the minor version number.
** -------------------------------------------------------------------- */
#define REX_VERSION_NUMBER ((unsigned long) 403)

/* --------------------------------------------------------------------
** REX Task List Manipulation Macros
**   Allows remapping of task manipulation functions in other operation
**   environments.
** -------------------------------------------------------------------- */
#ifdef FEATURE_ZREX
#error code not present
#else
#define REX_TASK_LIST_FRONT() (&rex_task_list)
#define REX_TASK_LIST_NEXT( tcb_ptr ) ((rex_tcb_type *) tcb_ptr->link.next_ptr )
#define REX_TASK_LIST_PREV( tcb_ptr ) ((rex_tcb_type *) tcb_ptr->link.prev_ptr )
#define REX_TASK_LIST_POP( tcb_ptr )\
   tcb_ptr->link.next_ptr->link.prev_ptr = tcb_ptr->link.prev_ptr;\
   tcb_ptr->link.prev_ptr->link.next_ptr = tcb_ptr->link.next_ptr;
#endif

/*===========================================================================
* MACRO REX_TASK_RUNNABLE
*
* Macro that captures testing of three conditions as to whether a task is
* runnable or not. See block comment in REX_SET_BEST_TASK for more details.
*
* Pass in ptr to TCB to be tested.  Interrupts must be locked prior to
* calling.
*
*==========================================================================*/
#ifdef FEATURE_ZREX
#error code not present
#else

   /* ------------------------------------------------
   ** REX, REX++
   ** ------------------------------------------------ */

   #if defined  FEATURE_REX_SYS_SIG
#error code not present
   #elif defined FEATURE_REX_APC
      /* REX + APC's
      **    A task is runnable if it is:
      **       NOT suspended
      **                              AND
      **       NOT waiting on a crit sect that is in the non signalled state
      **                              AND
      **       Has a signal set on which it is waiting
      **                              OR
      **       Has an APC queued for it
      */
      #define REX_TASK_RUNNABLE(tcb) ((tcb->suspended == FALSE) && \
                                      (tcb->cs_link.next_ptr == NULL) && \
                                      ((tcb->wait      == 0)     || \
                                       (tcb->num_apcs > 0)))
   #else
      /* REX
      **    A task is runnable if it is:
      **       NOT suspended
      **                              AND
      **       NOT waiting on a crit sect that is in the non signalled state
      **                              OR
      **       Has a signal set on which it is waiting
      */
      #define REX_TASK_RUNNABLE(tcb)  ((tcb->suspended == FALSE) && \
                                       (tcb->wait      == 0)     && \
                                       (tcb->cs_link.next_ptr == NULL))
   #endif
#endif


/* ==========================================================================
MACRO REX_PAUSE_DOG_MONITOR
DESCRIPTION
   Tells Dog to stop monitoring a specific task. Does nothing if auto-dog
   is not turned on for the task.

============================================================================= */
#define REX_PAUSE_DOG_MONITOR( tcb_ptr ) \
{ \
   if ( ( tcb_ptr->autodog_enabled ) && ( tcb_ptr->dog_report_val >= 0 ) )\
   { \
      dog_monitor_pause( tcb_ptr->dog_report_val ); \
   } \
}


/* ==========================================================================
MACRO REX_RESUME_DOG_MONITOR
DESCRIPTION
   Tells Dog to resume monitoring a specific task. Does nothing if auto-dog
   is not turned on for the task.

============================================================================= */
#define REX_RESUME_DOG_MONITOR( tcb_ptr ) \
{ \
   if ( tcb_ptr->dog_report_val >= 0 ) \
   { \
      dog_monitor_resume( tcb_ptr->dog_report_val ); \
   } \
}

/*===========================================================================
FUNCTION REX_TRY_ENTER_CRIT_SECT
===========================================================================*/
boolean rex_try_enter_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
   dword intsav;
   
   INTLOCK_SAV(intsav);

   /* mark the TCB with the critical section that we want */
   ASSERT( rex_curr_task->cs_sp <
           rex_curr_task->cs_stack + REX_CRIT_SECT_MAX - 1 );

   /* If this task already owns this critical section, just increment the
   ** lock count and return.
   */
   if ( crit_sect->lock_count > 0  &&
        rex_curr_task == crit_sect->owner)
   {
      ASSERT( crit_sect->lock_count < 0xff );
      crit_sect->lock_count++;
   }
   else
   {
      /* If it's unlocked, we simply lock it and proceed */
      if ( crit_sect->lock_count == 0 )
      {
         ++rex_curr_task->cs_sp;
         *rex_curr_task->cs_sp = crit_sect;

         crit_sect->lock_count = 1;
         crit_sect->owner      = rex_curr_task;

         crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;

         /* If we bump the task's priority up to prevent priority inversion,
         ** we need to be able to restore it to its original priority once
         ** done.
         */
         crit_sect->orig_pri = rex_curr_task->pri;
      }
      /* not available, just return */
      else
      {
         INTFREE_SAV(intsav);
         return FALSE;
      }
   }

   INTFREE_SAV(intsav);
   return TRUE;
} /* END rex_try_enter_crit_sect */

/*===========================================================================
FUNCTION REX_SET_BEST_TASK

DESCRIPTION
  This function searches a portion of the TCB list to find the highest
  priority TCB which has is eligible for scheduling.  Since REX is
  pre-emptive, normally the highest priority non-blocked task will be
  running at any given time (except for a few situations like TASKLOCK).
  As such, normally one can start traversing the TCB list at the first task
  of lower priority than the one that was just blocked.  rex_set_best_task()
  accepts a pointer to a TCB as an argument, and will begin searching the
  list at the next TCB after the argument.  It will set rex_best_task to
  the first (and therefore highest priority) task it finds that is not
  blocked awaiting a resource.

  There are three conditions that need to be met for a task to be runnable.

  1. The task is not blocked on a rex_wait() function call.  This will be
     true if the task's TCB->wait variable is zero.

  2. The task is not blocked on a rex_crit_sect_enter() function call. This
     will be true if the task's TCB->cs_link.next_ptr is NULL.

  3. The task was not created in the suspended mode.  This will be true if the
     task's TCB->suspended flag is false.

  This function will call the macro REX_TASK_RUNNABLE to test the three
  conditions.

  Note that a given task can be blocked on only one of these constructs at
  a time.  For example, if the task comes up in the SUSPENDED mode, then
  it will never get the change to execute either rex_wait() or rex_enter_crit_
  section().  As such, at various places throughout REX, when a condition is
  met, there isn't a need to check for the other conditions before making a
  scheduling decision.  For example, in rex_set_sigs() a task that was blocked
  on a rex_wait() call is potentially woken up.  The code that does the wakeup
  needs only look at the tasks "sigs" and "wait" fields in the TCB to decide.
  The fact that the task was blocked on rex_wait() means that it could not
  be SUSPENDED nor could it be blocked on a critical section.


DEPENDENCIES
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.

RETURN VALUE
  void

SIDE EFFECTS
  rex_best_task is modified.
===========================================================================*/
void rex_set_best_task(
  rex_tcb_type *start_tcb
)
{
   rex_tcb_type *candidate_task;

   ASSERT( start_tcb != NULL );

#ifdef FEATURE_ZREX
#error code not present
#else
   candidate_task = start_tcb->link.next_ptr;
#endif

   ASSERT( candidate_task != NULL );

   /* find first runnable task */
   while ( REX_TASK_RUNNABLE(candidate_task) == FALSE)
   {
#ifdef FEATURE_ZREX
#error code not present
#else
      candidate_task = candidate_task->link.next_ptr;
#endif
      ASSERT( candidate_task != NULL );
   }

   rex_best_task = candidate_task;
   return;
} /* END rex_set_best_task */


/*===========================================================================
FUNCTION REX_INIT_CRIT_SECT

DESCRIPTION
  Initializes a critical section.  Must be done prior to use.

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
void rex_init_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
   crit_sect->lock_count = 0;
   crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;
   crit_sect->orig_pri   = (rex_priority_type)-1;
   crit_sect->owner      = REX_CRIT_SECT_FLAG;
   return;
} /* END rex_init_crit_sect */


/*===========================================================================
FUNCTION REX_ENTER_CRIT_SECT

DESCRIPTION
  Acquires a critical section and proceeds.  If the critical section is
  locked by another task, puts the calling task onto a queue to be woken up
  later when the owning task releases it.

DEPENDENCIES
  critical section must have been initialized

RETURN VALUE
  void

SIDE EFFECTS
  The calling task will be suspended if the critical section is not
  available.
===========================================================================*/
void rex_enter_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
   if (rex_curr_task)
   {
      ASSERT( !TASKS_ARE_LOCKED( ) );
      ASSERT( !rex_is_in_irq_mode( ) );
   
      REX_INTLOCK();
   
      /* mark the TCB with the critical section that we want */
      ASSERT( rex_curr_task->cs_sp <
              rex_curr_task->cs_stack + REX_CRIT_SECT_MAX - 1 );
   
      /* If this task already owns this critical section, just increment the
      ** lock count and return.
      */
      if ( crit_sect->lock_count > 0  &&
           rex_curr_task == crit_sect->owner)
      {
         ASSERT( crit_sect->lock_count < 0xff );
         crit_sect->lock_count++;
      }
      else
      {
         ++rex_curr_task->cs_sp;
         *rex_curr_task->cs_sp = crit_sect;
   
         /* If it's unlocked, we simply lock it and proceed */
         if ( crit_sect->lock_count == 0 )
         {
            crit_sect->lock_count = 1;
            crit_sect->owner      = rex_curr_task;
   
            crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;
   
            /* If we bump the task's priority up to prevent priority inversion,
            ** we need to be able to restore it to its original priority once
            ** done.
            */
            crit_sect->orig_pri = rex_curr_task->pri;
         }
         else
         {
            /* Add this TCB to the list attached to the crit section variable.
            ** Keep it simple, just put in on the front.
            */
   
            /* previous first element's prev_ptr needs to point to us */
            if (crit_sect->tcb_link != REX_CRIT_SECT_FLAG)
            {
               crit_sect->tcb_link->cs_link.prev_ptr = rex_curr_task;
            }
   
            /* set our next ptr to what the crit sect var previously pointed to */
            rex_curr_task->cs_link.next_ptr = crit_sect->tcb_link;
   
            /* we are the new head, so make crit sect var point to us, and set
            ** our prev_ptr to the flag value.
            */
            crit_sect->tcb_link = rex_curr_task;
            rex_curr_task->cs_link.prev_ptr = REX_CRIT_SECT_FLAG;
   
            /* If the requesting task is of higher priority than the task that
            ** holds the critical section, then to prevent priority inversion,
            ** we are going to temporarily bump the holding task's priority up
            ** to the priority of the requesting task long enough for it to
            ** release the crit section.
            */
            if ( rex_curr_task->pri > crit_sect->owner->pri )
            {
               /* besides raising priority of owner of crit_sect, the original priority
                 * of the critical sections in the owner thread's crit sect stack
                 * which are on top of this critical section will be bumped up to caller
                 * thread's priority. This has to be done to prevent "multiple
                 * critical section priority inversion issue". */
   
               rex_crit_sect_type **cs_sp_parser;
               rex_crit_sect_type *prev_crit_sect;
                  
               cs_sp_parser = crit_sect->owner->cs_sp;
               prev_crit_sect = *cs_sp_parser;
   
               /* while (not finding the crit_sect in the cs_stack) && (we are still in the cs_stack) */
               while ((prev_crit_sect != crit_sect) && (cs_sp_parser >= crit_sect->owner->cs_stack))                                                
               {
                  if ((NULL != prev_crit_sect ) && ((rex_crit_sect_type *)REX_CRIT_SECT_FLAG != prev_crit_sect))
                  {
                     if (prev_crit_sect->owner == crit_sect->owner)
                     {
                        prev_crit_sect->orig_pri = rex_curr_task->pri;
                     }
                  }
                  else
                  {
                     /* ASSERT, something wrong */
                     ASSERT("invalid crit_sect in cs_stack");
                  }                  
                     
                  /* get next crit_sect in the stack; */
                  prev_crit_sect = *(--cs_sp_parser);
               }
   
               /* we could not find the crit_sect in the cs_stack */
               if (cs_sp_parser < crit_sect->owner->cs_stack)
               {
                  /* ASSERT, something wrong */
                  ASSERT("Could not find crit_sect in cs_stack");
               }
               (void) rex_task_pri( crit_sect->owner, rex_curr_task->pri );
            }
   
            /* Tell Dog to stop monitoring this task.
            */
            REX_PAUSE_DOG_MONITOR( rex_curr_task );
   
            /* Schedule the best runnable task.
            */
   #ifdef FEATURE_REX_SYS_SIG
#error code not present
   #else
            rex_set_best_task( REX_TASK_LIST_FRONT() );
            rex_sched();
   #endif
         }
      }
   
      REX_INTFREE();
   }
   return;
} /* END rex_enter_crit_sect */


/*===========================================================================
FUNCTION REX_LEAVE_CRIT_SECT_INTERNALS

DESCRIPTION
  This function releases a critical section for a specified task.  Normally
  one would expect a calling task to release only a critical section that
  it owned.  However, suppose task A calls rex_kill_task() to kill task B.
  If task B is holding a critical region, the rex_kill_task() function needs
  to release that region, otherwise other tasks waiting on that region could
  be blocked forever.

  When rex_kill_task() calls this function, it does not wish to immediately
  invoke a possible rex_sched() call.  So a flag is provided as a parameter
  to specify whether rescheduling is allowed.

  An API version of this function, which only allows a task to release a
  critical region that it owns, follows.

DEPENDENCIES
  This function is for *internal* REX use only.

  The calling function must lock interrupts prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  If the resched_allowed parameter is set to true, a task swap may occur.

===========================================================================*/
static void rex_leave_crit_sect_internals(
  rex_crit_sect_type *crit_sect,
  rex_tcb_type       *p_tcb,
  boolean             resched_allowed
)
{
   rex_tcb_type *index_ptr = NULL;
   rex_tcb_type *save_ptr  = NULL;
   rex_priority_type best_pri;

   /* No INTLOCK - calling function is responsible */

   /* basic sanity check
   */
   if ( (p_tcb->cs_sp < p_tcb->cs_stack) ||
        (crit_sect->owner != p_tcb)      ||
        (p_tcb->cs_link.next_ptr != NULL) )
   {
      /* calling task is not holding this lock */
      ASSERT( 0 );
      return;
   }

   /* If this critical section has been taken more than once, just decrement
   ** the lock count and return.
   */
   if ( crit_sect->lock_count > 1 )
   {
      --crit_sect->lock_count;
      return;
   }

   if (*p_tcb->cs_sp != crit_sect)
   {
      /* Caller is trying to release critical sections in something other
      ** than LIFO order. We do not support this.
      */
      ASSERT( 0 );
      return;
   }

   /* mark that we are no longer holding/waiting-in this crit section */
   --p_tcb->cs_sp;

   /* if our priority got changed as part of priority-inversion-prevention,
   ** then change it back.
   */
   if ( crit_sect->orig_pri != p_tcb->pri )
   {
      (void) rex_task_pri( p_tcb, crit_sect->orig_pri );
   }

   /* If there are no waiting tasks, we can simply unlock and proceed */
   if ( crit_sect->tcb_link == REX_CRIT_SECT_FLAG )
   {
      crit_sect->lock_count = 0;
      crit_sect->owner      = REX_CRIT_SECT_FLAG;
      crit_sect->orig_pri   = (rex_priority_type)-1;
   }
   else
   {
      /* Find the highest priority task on the list attached to the crit
      ** sect variable and update the critical section variable and the
      ** task's TCB to indicate that this task now owns the critical section.
      ** Note that the list traversed is not the global TCB list that we
      ** normally traverse when looking for a task.
      */
      best_pri = 0;

      for( index_ptr = crit_sect->tcb_link;
           index_ptr != REX_CRIT_SECT_FLAG;
           index_ptr = index_ptr->cs_link.next_ptr
         )
      {
         if ( index_ptr->pri > best_pri )
         {
            best_pri = index_ptr->pri;
            save_ptr = index_ptr;
         }
      }

      /* Remove item from list */

      /* If found task is the first on the list, fix up list head */
      if ( save_ptr->cs_link.prev_ptr == REX_CRIT_SECT_FLAG )
      {
         crit_sect->tcb_link = save_ptr->cs_link.next_ptr;
      }
      else  /* fix up previous element */
      {
         save_ptr->cs_link.prev_ptr->cs_link.next_ptr =
                   save_ptr->cs_link.next_ptr;
      }

      /* look to next element*/

      /* if found task is NOT the last item on the list */
      if ( save_ptr->cs_link.next_ptr != REX_CRIT_SECT_FLAG )
      {
         save_ptr->cs_link.next_ptr->cs_link.prev_ptr =
                   save_ptr->cs_link.prev_ptr;
      }

      /* Set this task's pointers to show that the task is not in the list .
      ** (Recall that the scheduler uses  these to determine if a task is
      ** blocked on a crit section or not )
      */
      save_ptr->cs_link.next_ptr = save_ptr->cs_link.prev_ptr = NULL;

      /* mark the crit section appropriately */
      crit_sect->owner    = save_ptr;
      crit_sect->orig_pri = save_ptr->pri;

      /* Tell Dog to resume monitoring the found task.
      */
      REX_RESUME_DOG_MONITOR( save_ptr );

      /* Determine if the found task should be swapped in or not */

      /* Always compare with REX_BEST_TASK, not REX_CURR_TASK! */
      if ( ( resched_allowed                    ) &&
           ( save_ptr->pri > rex_best_task->pri ) &&
           ( !save_ptr->suspended               )  )
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #else
         rex_best_task = save_ptr;
         rex_sched();
         #endif
      }
      else
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #endif
      }
   }

   /* No INTFREE - calling function is responsible */
   return;
} /* END rex_leave_crit_sect_internals */


/*===========================================================================
FUNCTION REX_LEAVE_CRIT_SECT

DESCRIPTION
  Allows a task to release a critical section that it is holding.  This is
  the API version of rex_leave_crit_sect_internals.  It is the only version that
  should be called outside of REX.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECT
  If a higher priority task is waiting on the critical section, then when
  this task releases that critical section, the higher priority task will
  be swapped in.
===========================================================================*/
void rex_leave_crit_sect(
   rex_crit_sect_type *crit_sect
)
{
   if (rex_curr_task)
   {
      ASSERT( !rex_is_in_irq_mode( ) );
   
      REX_INTLOCK();
   
      /* call "real" routine, tell it that it's OK to reschedule */
      rex_leave_crit_sect_internals( crit_sect, rex_curr_task, TRUE );
      REX_INTFREE();
   }
   return;
} /* END rex_leave_crit_sect */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION REX_CLR_SIGS

DESCRIPTION
  Clears the p_sigs from the tcb passed in.

DEPENDENCIES
  p_tcb is a valid task control block.

RETURN VALUE
  The previous signal mask.

SIDE EFFECTS
  The specified signals will be cleared for the specified task
===========================================================================*/
rex_sigs_type rex_clr_sigs(
   rex_tcb_type   *p_tcb,        /* tcb for which the signals will be cleared */
   rex_sigs_type   p_sigs        /* which signals to clear                    */
)
{
   rex_sigs_type prev_sigs = 0;  /* holds the signals prior to the clear */

   /*-------------------------------------------------------
   ** Save current signals for the return value and then
   ** clear the signals listed in p_sigs
   **-----------------------------------------------------*/
   REX_INTLOCK();

#ifdef FEATURE_REX_IPC
   /* Don't clear IPC Receive Signal if a message is pending */
   if ((p_sigs & p_tcb->ipc_info.receive_signal) &&
        !pq_is_empty(p_tcb->ipc_info.mq_ptr))
   {
      p_sigs &= ~p_tcb->ipc_info.receive_signal; /* IPCDEFI_RECEIVE_SIG */
   }
#endif

   p_sigs = ~p_sigs;

   prev_sigs  = p_tcb->sigs;
   p_tcb->sigs = p_tcb->sigs & p_sigs;

   REX_INTFREE();

   return prev_sigs;
} /* END rex_clr_sigs */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION REX_SET_SIGS

DESCRIPTION
  Sets the signals for a task. If the task is waiting for one or more of the
  set signals and it is of higher priority then the current task, a task
  swap is induced.

DEPENDENCIES
   p_tcb must be a valid task control block.

RETURN VALUE
  The sigs prior to the set action.

SIDE EFFECTS
  Changes a tasks signals which may induce a task swap.
===========================================================================*/
rex_sigs_type rex_set_sigs(
   rex_tcb_type   *p_tcb,          /* tcb for which the sigs will be set */
   rex_sigs_type   p_sigs          /* the sigs to set                    */
)
{
   rex_sigs_type prev_sigs = 0;    /* the sigs before setting */

   REX_INTLOCK();

   prev_sigs = p_tcb->sigs;

   p_tcb->sigs = p_tcb->sigs | p_sigs;

   /*-------------------------------------------------------
   ** Check and see if the p_tcb task was waiting for the
   ** sigs. If so, clear out the wait sigs and reschedule
   ** if the task is the new best task
   **-----------------------------------------------------*/

   if((p_tcb->wait & p_sigs) != 0)
   {
      p_tcb->wait = 0;

      /* Tell Dog to resume monitoring this task.
      */
      REX_RESUME_DOG_MONITOR( p_tcb );

#ifdef FEATURE_ZREX
#error code not present
#else
      if ((p_tcb->pri > rex_best_task->pri) && REX_TASK_RUNNABLE(p_tcb))
#endif
      {
#ifdef FEATURE_REX_SYS_SIG
#error code not present
#else
         rex_best_task = p_tcb;
         rex_sched();
#endif
      }
      else
      {
#ifdef FEATURE_REX_SYS_SIG
#error code not present
 #endif
      }
   }

   REX_INTFREE();

   return prev_sigs;
} /* END rex_set_sigs */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION REX_GET_SIGS

DESCRIPTION
  Returns the current signatures for a task.

DEPENDENCIES
  p_tcb is a valid task control block

RETURN VALUE
  The current sigs for a task.

SIDE EFFECTS
  None.
===========================================================================*/
rex_sigs_type rex_get_sigs(
  rex_tcb_type *p_tcb           /* tcb for which sigs will be returned */
)
{
   return p_tcb->sigs;
} /* END rex_get_sigs */
#endif /*MSM5000_IRAM_FWD*/


#if defined( FEATURE_REX_IPC )
/*===========================================================================
FUNCTION REX_REGISTER_RECEIVE_SIGNAL

DESCRIPTION

PARAMETERS

RETURN VALUE

==========================================================================*/
void rex_register_receive_signal
(
   rex_sigs_type signal
)
{
   rex_self()->ipc_info.receive_signal = signal;
}
#endif /*FEATURE_REX_IPC*/

#if defined( FEATURE_REX_APC )

#if !defined( T_WINNT )

/* ==================================================================
FUNCTION REX_CALL_APC

DESCRIPTION
   This function makes the actual APC call. APC calls are wrapped in
   this function because we would like to do some post-processing
   after the APC call has been completed.

   NOTE 1: This function is for internal use only.

   NOTE 2: This function should only be called in task context.

   NOTE 3: This function is not called directly from anywhere. It is
           called as a result of a context frame getting pushed on to
           the stack of a thread in rex_apc_prolog. In rex_apc_prolog
           the PC field of the context frame is set to the address of
           rex_call_apc, so that when registers are restored using
           the contents of the frame, this function gets called.
           rex_apc_prolog sets the LR field of the context frame to
           the address of the rex_apc_epilog function. Thus after
           the call to rex_call_apc, execution proceeds to
           rex_apc_epilog.

===================================================================== */
void rex_call_apc(
   rex_apc_func_type apc,
   unsigned long     param
)
{
   ASSERT( apc != NULL );

   apc( param );

   /*----------------------------------------------------------------
   ** Set the best task. Note that this need not be the current task,
   ** because the current task may no longer be schedulable (its
   ** num_apcs field may be zero).
   **
   ** NOTE: We lock interrupts before returning. Recall that on
   **       returning, execution resumes at rex_apc_epilog. Since
   **       rex_apc_epilog restores registers using the contents of
   **       a context frame, interrupts need to be locked during its
   **       execution. There is no rex_int_free call that matches this
   **       rex_int_lock call. This is because when rex_apc_epilog
   **       restores registers, the interrupt state of the thread is
   **       also restored.
   ----------------------------------------------------------------*/
   (void) rex_int_lock( );

   rex_curr_task->num_apcs--;

   ASSERT( rex_curr_task->num_apcs >= 0 );

   rex_set_best_task( REX_TASK_LIST_FRONT() );

   return;
} /* END rex_call_apc */


/* ==================================================================
FUNCTION REX_APC_PROLOG
DESCRIPTION
   Pushes an APC context frame on to the stack of the thread for which
   the APC is queued.

   The following fields in the context frame are filled in.

   spsr: spsr of the previous context frame.
   r0:   pointer to the APC function that is being queued.
   r1:   the argument to the APC function.
   LR:   PC of the previous frame on the thread's stack.
   PC:   the address of the rex_call_apc function.

   NOTE 1: This function is for internal REX use only.

   NOTE 2: This function should not be called from an ISR.

   NOTE 3: This function ORs PSR_Thumb into the SPSR for the thread
         on which rex_call_apc will run. If our code is ever changed
         to run exclusively in ARM mode, then this will have to change.
         In current implementation, PSR_Thumb has to be OR'ed into
         SPSR to insure that rex_call_apc is never called from ARM mode
         since it is THUMB code, called through a pointer, and thus the
         linker has not generated a interworking veneer. We were getting
         alignment faults due to rex_call_apc being called from ARM mode.
         This is done based on what mode REX is compiled in (Thumb/ARM).
===================================================================== */
static void rex_apc_prolog(
   rex_apc_func_type apc,
   unsigned long     param,
   rex_tcb_type     *tcb_ptr
)
{
   char                   *stack_ptr;
   rex_context_frame_type *context_frame_ptr;

   ASSERT( apc != NULL );
   ASSERT( tcb_ptr != NULL );
   ASSERT( tcb_ptr != rex_curr_task );
   ASSERT( !rex_is_in_irq_mode( ) );

   /* Fill in a context frame and push it on to the stack of the
   ** relevant thread. Update the stack pointer field in the TCB.
   */
   stack_ptr = (char *) tcb_ptr->sp - sizeof(rex_context_frame_type);

   ASSERT( (unsigned long) stack_ptr <
           (unsigned long) tcb_ptr->stack_limit +
           tcb_ptr->stack_size );

   memset( (void *) stack_ptr, 0x00, sizeof(rex_context_frame_type) );
   context_frame_ptr = (rex_context_frame_type *) stack_ptr;
   context_frame_ptr->spsr.val =
           (((rex_context_frame_type *) tcb_ptr->sp)->spsr.val &
            PSR_Mode_Mask)
#if defined(__thumb) || defined(__thumb_thumb__)
                           | PSR_Thumb ; // Needs to match the compile mode of this module
#endif

   /* If spsr_val indicates user mode, set it to supervisor mode.
   */
   if ( (context_frame_ptr->spsr.val & PSR_Mode_Mask) == PSR_User )
   {
      context_frame_ptr->spsr.val &= ~PSR_Mode_Mask;
      context_frame_ptr->spsr.val |= PSR_Supervisor;
   }

   context_frame_ptr->r[0].arg = (unsigned long) apc;
   context_frame_ptr->r[1].arg = param;
   #if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
   context_frame_ptr->r[10].val = (unsigned long) 0xffffffff;
   #else
   context_frame_ptr->r[10].val = (unsigned long) tcb_ptr->stack_limit;
   #endif
   context_frame_ptr->lr.arg   = (unsigned long) rex_apc_epilog;
   context_frame_ptr->pc.arg   = (unsigned long) rex_call_apc;
   tcb_ptr->sp = stack_ptr;

   return;
} /* rex_apc_prolog */


/* ==================================================================
FUNCTION REX_APC_STACK_AVAIL
DESCRIPTION
   Returns TRUE if these is enough space in the stack of a REX
   thread to hold an APC context frame. In addition to the frame,
   the local variables in the APC function will also need to be
   stored on the stack. We don't know exactly how large these
   variables are, so we make the crude assumption that the total
   size of these variables is at most REX_APC_EXTRA_STACK_SIZE bytes.

   This function is for internal REX use only.
===================================================================== */
static boolean rex_apc_stack_avail(
   rex_tcb_type *tcb_ptr
)
{
   unsigned long stack_ptr;
   unsigned long stack_limit_ptr;

   ASSERT( tcb_ptr != NULL );

   stack_ptr = (unsigned long) tcb_ptr->sp;

   stack_limit_ptr = (unsigned long) tcb_ptr->stack_limit;

   ASSERT( stack_ptr < stack_limit_ptr + tcb_ptr->stack_size );

   return ( stack_ptr >= stack_limit_ptr                +
                         sizeof(rex_context_frame_type) +
                         REX_APC_EXTRA_STACK_SIZE
          ) ? TRUE : FALSE;
} /* END rex_apc_stack_avail */


/* ==========================================================================
FUNCTION REX_QUEUE_APC_INTERNAL
DESCRIPTION
   Performs the actual work of queueing an APC. This function is for internal
   REX use only. It must never be called from an ISR. The function exported
   to the user is rex_queue_apc, which causes rex_queue_apc_internal to be
   called in the context of the DPC task.
============================================================================= */
boolean rex_queue_apc_internal(
   unsigned long apc_handle,
   unsigned long tcb_handle,
   unsigned long param
)
{
   boolean           ret_status = FALSE;
   rex_apc_func_type apc        = (rex_apc_func_type) apc_handle;
   rex_tcb_type     *tcb_ptr    = (rex_tcb_type *) tcb_handle;

   ASSERT( apc     != NULL );
   ASSERT( tcb_ptr != NULL );
   ASSERT( !rex_is_in_irq_mode( ) );

   /* An APC does not need to be queued if tcb_ptr belongs to the
   ** current task. Otherwise, check whether there is sufficient space on
   ** the stack of the task for which the APC is to be queued.
   */
   if ( tcb_ptr == rex_curr_task )
   {
      /* A thread is queueing an APC for itself. Just call the
      ** function here.
      */
      apc( param );
      ret_status = TRUE;
   }
   else if( rex_apc_stack_avail( tcb_ptr ) )
   {
      REX_INTLOCK();
         rex_apc_prolog( apc, param, tcb_ptr );
         tcb_ptr->num_apcs++;

         /* Schedule the task for which the APC is being queued, if
         ** necessary.
         */
         if ( ( tcb_ptr->pri > rex_best_task->pri ) &&
              REX_TASK_RUNNABLE( tcb_ptr ) )
         {
            #ifdef FEATURE_REX_SYS_SIG
#error code not present
            #else
            rex_best_task = tcb_ptr;
            rex_sched( );
            #endif
         }
         else
         {
            #ifdef FEATURE_REX_SYS_SIG
#error code not present
            #endif
         }
         ret_status = TRUE;
      REX_INTFREE();
   }

   return ret_status;
} /* END rex_queue_apc_internal*/

#endif /* !defined( T_WINNT ) */

/* ==================================================================
FUNCTION REX_QUEUE_APC
DESCRIPTION
   Queues an asynchronous procedure call (APC) for a rex thread.
   An APC is executed in the context of the thread for which it is
   queued. Queueing an APC for a thread makes it schedulable, but
   only until the execution of the APC is complete. Multiple APCs may
   be queued for the same thread, in which case the APC functions are
   called in FIFO order. If a thread queues an APC for itself, the
   APC is executed immediately exactly as if the function were called
   directly.

   Example Scenarios:
   Threads A, B, C and D.
   pri of A < pri of B < pri of C < pri of D.
   Threads A and C are currently schedulable, B and D are not.
   C is the current task.

   C queues an APC for B
   =====================
   B becomes runnable. Since C has higher priority than B, B does not
   get scheduled immediately. If C calls rex_wait or suspends itself
   in some other fashion, then B is scheduled, the queued APC is
   executed in B's context, and then B goes back to the state that
   it was in before the APC was executed. Assuming no signals have
   been set for B and no events posted, B will not be schedulable at
   this point. A will get schdeduled.

   C queues an APC for D
   =====================
   D becomes runnable and starts running since it is the
   highest-priority runnable task. The APC is executed in D's context
   and then D goes back into its previous state (non-runnable). C
   will now get scheduled again.
===================================================================== */
boolean rex_queue_apc(
   rex_apc_func_type apc,
   rex_tcb_type     *tcb_ptr,
   unsigned long     param
)
{
   if ( ( apc == NULL ) || ( tcb_ptr == NULL ) )
   {
      return 0;
   }

#ifndef T_WINNT
   if( !rex_apc_stack_avail( tcb_ptr ) )
   {
      return 0;
   }
#endif

   return rex_queue_dpc(
                        rex_queue_apc_internal,
                        (unsigned long) apc,
                        (unsigned long) tcb_ptr,
                        param
          );
} /* END rex_queue_apc */

#endif /* FEATURE_REX_APC */


/*===========================================================================
FUNCTION REX_WAIT

DESCRIPTION
  Suspends a task until the one or more of the signals in the mask are set for
  the current task.

DEPENDENCIES
  None.

RETURN VALUE
  The signals that were set for the task.

SIDE EFFECTS
  Will cause a task swap if the signals for the task are not already set at
  the time of the call.
===========================================================================*/
rex_sigs_type rex_wait(
  rex_sigs_type p_sigs          /* sigs to wait on */
)
{
   rex_sigs_type sigs = 0;

   ASSERT( !rex_is_in_irq_mode( ) );
   ASSERT( !TASKS_ARE_LOCKED( ) );

   REX_INTERCEPT_WAIT( p_sigs );

   REX_INTLOCK();

   /*-------------------------------------------------------
   ** if no signals are set,  wait, and reschedule
   **-----------------------------------------------------*/
   if( (rex_curr_task->sigs & p_sigs) == 0 )
   {
      /*-------------------------------------------------------
      ** Set the sigs the task should wait upon
      **-----------------------------------------------------*/
      rex_curr_task->wait = p_sigs;

      /* Tell Dog to stop monitoring this task.
      */
      REX_PAUSE_DOG_MONITOR( rex_curr_task );

      /*-------------------------------------------------------
      ** Find the next best_task and schedule it.  We know
      ** that it will be a lower priority task than the one
      ** we just blocked, because a higher priority task that
      ** became ready would have already pre-empted the one
      ** we are blocking.
      **-----------------------------------------------------*/
      #ifdef FEATURE_REX_SYS_SIG
#error code not present
      #else
      rex_set_best_task( REX_TASK_LIST_FRONT() );
      rex_sched();
      #endif
   }
   /*-------------------------------------------------------
   ** return with the signal mask of the current task
   **-----------------------------------------------------*/
   sigs = rex_curr_task->sigs;
   REX_INTFREE();
   return sigs;
} /* END rex_wait */


/*===========================================================================
FUNCTION REX_IDLE_TASK

DESCRIPTION
  The lowest possible task priority (0) and runs only when no other
  task is ready to run.

DEPENDENCIES
  rex_def_task should spawn this task

RETURN VALUE
  None

SIDE EFFECTS
  this task never suspends, waits or returns.
===========================================================================*/
/*lint -e715 arg not referenced */
/*lint -e527 Unreachable */
void rex_idle_task(
   dword arg
)
{
#ifdef T_WINNT
#error code not present
#endif
#ifdef FEATURE_ZREX
#error code not present
#else
   for(;;){}
#endif
   return;
} /* END rex_idle_task */

/*lint +e527 Unreachable */
/*lint +e715 arg not referenced */

/*===========================================================================
FUNCTION REX_TASK_PREAMBLE

DESCRIPTION
  Special Note: rex_task_preamble is an internal function that should be called
  only from rex service functions. It should not be called directly by user
  software.
  rex_task_preamble is a helper function which is the first thing to execute in
  every newly created task. Upon receiving control, rex_task_preamble merely
  calls the user-defined entry point for the new task, passing it the user-
  specified parameter for the new task.

DEPENDENCIES
  assumes that function rex_def_task has set up the correct
  context frame on the stack for the new task.
RETURN VALUE
  None. This function does not return.********
SIDE EFFECTS
  None
===========================================================================*/
void rex_task_preamble(
   void (*func_ptr)( dword arg ),
   dword arg
)
{
  #ifdef FEATURE_ZREX
#error code not present
  #else
     func_ptr( arg );
  #endif

   /* if we return, kill the task */
#ifdef FEATURE_ZREX
#error code not present
#else
   rex_kill_task( rex_self() );
#endif
} /* END rex_task_preamble */


/*===========================================================================
FUNCTION REX_DEF_TASK/REX_DEF_TASK_EXT/REX_DEF_TASK_INTERNAL

DESCRIPTION
  Define a new rex task.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer
  p_stack must be on an aligned boundary and the size must be the number
  of bytes in the stack

  Should only be called by other REX functions.

RETURN VALUE
  None.

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task.
===========================================================================*/
void rex_def_task_internal(
  rex_tcb_type         *p_tcb,             /* valid tcb for new task         */
  unsigned char*       p_stack,            /* stack for new task             */
  rex_stack_size_type  p_stksiz,           /* stack size in bytes            */
  rex_priority_type    p_pri,              /* priority for new task          */
  rex_task_func_type   p_task,             /* task startup function          */
  dword                p_param,            /* parameter for new task         */
  char                 *p_tskname,         /* A/N string for task name       */
  boolean              p_susp,             /* is task initially suspended?   */
  void                 *p_parent,          /* opaque handle to container     */
  boolean              p_dynamic,          /* stack/tcb alloc'd via dyna mem */
  int                  dog_report_val      /* Dog report value               */
)
{
   word                   index      = 0;
   byte                   *stack_ptr = NULL;
   rex_context_frame_type *cf_ptr    = NULL;

   #ifdef FEATURE_ZREX
#error code not present
   #endif

   /*-------------------------------------------------------
   ** Task stack pointer points to the bottom of allocated
   ** stack memory. p_stksiz is the number of 8-bit bytes.
   **-----------------------------------------------------*/
   stack_ptr = (byte *)((unsigned long)p_stack + (unsigned long)p_stksiz - sizeof(unsigned long) );

   #if defined FEATURE_ZREX && defined FEATURE_ZREX_STACK_DEBUG
#error code not present
   #endif

   /*-------------------------------------------------------
   ** Creates room for the context.
   ** sp points to the top of the context frame.
   **-----------------------------------------------------*/
   stack_ptr -= sizeof( rex_context_frame_type );

   /*-------------------------------------------------------
   ** Defines the initial context.
   **-----------------------------------------------------*/
   #ifdef T_WINNT
#error code not present
   #else
   cf_ptr              = (rex_context_frame_type*)stack_ptr;
	  cf_ptr->spsr.val    = PSR_Supervisor
#if defined(__thumb) || defined(__thumb_thumb__)
                                           | PSR_Thumb; // Needs to match the compile mode of this module
#endif
   cf_ptr->r[0].task   = p_task;
   cf_ptr->r[1].arg    = p_param;
   #if defined(FEATURE_STACK_CHECK) && defined(FEATURE_LOG_FUNCTIONS)
   cf_ptr->r[10].arg   = (unsigned long) 0xffffffff;
   #else
   cf_ptr->r[10].arg   = (unsigned long)p_stack;
   #endif
   cf_ptr->lr.preamble = rex_task_preamble;
   cf_ptr->pc.preamble = rex_task_preamble;
   #endif

   /* ------------------------------------------------------
   ** Initialize the task control block (TCB)
   ** ------------------------------------------------------ */
   p_tcb->sp               =      stack_ptr;
   p_tcb->stack_limit      =      p_stack;
   p_tcb->stack_size       =      p_stksiz;
   p_tcb->slices           =      0;
   p_tcb->sigs             =      0;
   p_tcb->wait             =      0;
   p_tcb->pri              =      p_pri;
   p_tcb->cs_link.next_ptr =      NULL;
   p_tcb->cs_link.prev_ptr =      NULL;
   p_tcb->cs_sp            =      p_tcb->cs_stack;   --p_tcb->cs_sp;
   p_tcb->suspended        =      p_susp;
#ifdef FEATURE_DAC_MEMORY_PROTECTION
   p_tcb->dac              =      MMU_PROTECT_EFS_MEMORY; //default No access
#endif /* FEATURE_DAC_MEMORY_PROTECTION */
   #ifdef FEATURE_SI_STACK_WM
#error code not present
   #endif /* FEATURE_SI_STACK_WM */

   #if !defined T_WINNT
   p_tcb->thread_id     =      (unsigned long)p_tcb;
   #endif

   #if defined FEATURE_REX_SYS_SIG
#error code not present
   #endif

   p_tcb->task_name[REX_TASK_NAME_LEN] = '\0';
   if (p_tskname != NULL) /* copy task name if one was supplied */
   {
      /* copy bytes until /0 received or enough chars have been copied */
      while ( (p_tcb->task_name[index] = p_tskname[index] ) &&
            ( index++ <  REX_TASK_NAME_LEN  ) );;
   }

#if defined FEATURE_REX_APC
   p_tcb->num_apcs = 0;     /* Number of queued APCs */
#endif

#if defined FEATURE_REX_PROFILE
   p_tcb->time_samples = 0; /* Profiling information */
#endif

#if defined FEATURE_DS_SOCKETS_BSD
   p_tcb->bsdcb_ptr = NULL;
#endif

#if defined FEATURE_SOFTWARE_PROFILE
   p_tcb->numticks    = 0;
#endif

   p_tcb->err_num = 0;

   p_tcb->dog_report_val  = dog_report_val;
   p_tcb->autodog_enabled = FALSE;

#if defined FEATURE_REX_CREATE_TASK && !defined FEATURE_ZREX
   p_tcb->is_dynamic = TRUE;
#endif

   /*-------------------------------------------------------
   ** We are done if this is the idle task (kernel task) defined.
   **-------------------------------------------------------*/
   if (p_pri == 0) {
      #ifdef FEATURE_ZREX
#error code not present
      #endif
      #ifdef T_WINNT
#error code not present
      #endif
      return;
   }

   /*-------------------------------------------------------
   ** Find the first task whose priority is lower than the
   ** new task and insert the new task in front of the
   ** lower priority task.
   ** rex_idle_task (the kernel task) is at the end of the list
   ** with priority 0 and therefore no need to check for null ptrs.
   **-----------------------------------------------------*/

#ifdef FEATURE_ZREX
#error code not present
#else
   REX_INTLOCK();
   {
      rex_tcb_type *tcb_ptr;

      tcb_ptr = rex_task_list.link.next_ptr;

      /* The rex_kernel_tcb check is present in the following loop
      ** condition because the priority of the idle task is temporarily
      ** bumped up in rex_init to prevent scheduling. Tasks defined before
      ** the idle task's priority is reset to 0 can then cause tcb_ptr
      ** to go off the end of the list.
      */
      while ((tcb_ptr->pri > p_pri) && (tcb_ptr != &rex_kernel_tcb)) {
         tcb_ptr = tcb_ptr->link.next_ptr;
      }

      p_tcb->link.prev_ptr = tcb_ptr->link.prev_ptr;
      p_tcb->link.next_ptr = tcb_ptr;
      tcb_ptr->link.prev_ptr->link.next_ptr = p_tcb;
      tcb_ptr->link.prev_ptr = p_tcb;
   }
#endif

#ifdef FEATURE_REX_IPC
   if (ipcns_node_register(p_tcb) == FALSE)
   {
      return;
   }
#endif

   /*---------------------------------------------------
   ** Make this task the best task if it is higher
   ** priority than the present best task.
   **---------------------------------------------------*/

   /* Always compare with REX_BEST_TASK, not REX_CURR_TASK */
   if  ( (p_pri > rex_best_task->pri)  && (p_tcb->suspended == FALSE) )
   {
      rex_best_task = p_tcb;

      /* swap the task in */
      rex_sched();
   }

   rex_num_tasks++;

   REX_INTFREE();

   return;
} /* END rex_def_task_internal */


/* rex_def_task_ext takes two parameters in addition to the ones taken by
** rex_def_task. These allow the user to specify a name for the task and
** whether the task should be started in suspended mode or not. Note also
** that rex_def_task_ext treats stacks as arrays of characters and expects
** the stack size to be specified in units of bytes.
*/
void rex_def_task_ext(
   rex_tcb_type         *p_tcb,              /* valid tcb for new task       */
   unsigned char*        p_stack,            /* stack for new task           */
   rex_stack_size_type   p_stksiz,           /* stack size in bytes          */
   rex_priority_type     p_pri,              /* priority for new task        */
   rex_task_func_type    p_task,             /* task startup function        */
   dword                 p_param,            /* parameter for new task       */
   char                 *p_tskname,          /* A/N string for task name     */
   boolean               p_susp              /* is task initially suspended? */
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes.  The adjustment of the stack pointer
   ** is to assure alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%4)?
      (void*)(((unsigned long)p_stack+(sizeof(long)) - (unsigned long)p_stack%4)):p_stack;

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz - ((unsigned long)adj_stack_ptr - (unsigned long)p_stack),
      p_pri,
      p_task,
      p_param,
      p_tskname,
      p_susp,
      #ifdef FEATURE_ZREX
#error code not present
      #else
      NULL,
      #endif
      FALSE,
      -1
   );
   return;
} /* END rex_def_task_ext */


/* rex_def_task_ext2 takes one parameter in addition to the ones taken by
** rex_def_task_ext. This parameter allows the user to specify a dog report
** value for the task. Just as with rex_def_task_ext, the stack size should
** be specified in units of bytes.
*/
void rex_def_task_ext2(
   rex_tcb_type         *p_tcb,              /* valid tcb for new task       */
   unsigned char*        p_stack,            /* stack for new task           */
   rex_stack_size_type   p_stksiz,           /* stack size in bytes          */
   rex_priority_type     p_pri,              /* priority for new task        */
   rex_task_func_type    p_task,             /* task startup function        */
   dword                 p_param,            /* parameter for new task       */
   char                 *p_tskname,          /* A/N string for task name     */
   boolean               p_susp,             /* is task initially suspended? */
   int                   dog_report_val      /* Dog report value             */
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes.  The adjustment of the stack pointer
   ** is to assure alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%4)?
      (void*)(((unsigned long)p_stack+(sizeof(long)) - (unsigned long)p_stack%4)):p_stack;

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz - ((unsigned long)adj_stack_ptr - (unsigned long)p_stack),
      p_pri,
      p_task,
      p_param,
      p_tskname,
      p_susp,
      #ifdef FEATURE_ZREX
#error code not present
      #else
      NULL,
      #endif
      FALSE,
      dog_report_val
   );
   return;
} /* END rex_def_task_ext2 */

/* rex_def_task: original task definition function.
*/
void rex_def_task(
   rex_tcb_type        *p_tcb,              /* valid tcb for new task       */
   void *               p_stack,            /* stack for new task           */
   rex_stack_size_type  p_stksiz,           /* stack size in units of
                                               rex_stack_word_type          */
   rex_priority_type    p_pri,              /* priority for new task        */
   void                 (*p_task)( dword ), /* task startup function        */
   dword                p_param             /* parameter for new task       */
)
{
   /* In this case, stacks are passed as arrays of type rex_stack_word_type,
   ** with the size indicating the number of elements in the array. So we
   ** adjust here. The adjustment of the stack pointer is to assure
   ** alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%sizeof(rex_stack_word_type)) ?
      (void*)(((unsigned long)p_stack+(sizeof(rex_stack_word_type)) - (unsigned long)p_stack%sizeof(rex_stack_word_type))):p_stack;

   if (adj_stack_ptr != p_stack)
   {
      p_stksiz--;
   }

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz*sizeof(rex_stack_word_type),
      p_pri,
      p_task,
      p_param,
      NULL,
      FALSE,
      #ifdef FEATURE_ZREX
#error code not present
      #else
      NULL,
      #endif
      FALSE,
      -1
   );
   return;
} /* END rex_def_task */


/*===========================================================================
FUNCTION REX_SUSPEND_TASK

DESCRIPTION
  Suspend a task so that it is not eligible for scheduling. If the task
  being suspended is the current task, then a context switch will occur.

  Note: If called to suspend the current task in IRQ mode, the suspension
  will not happen until the next time rex_best_task is set and rex_sched
  is called.

===========================================================================*/
void rex_suspend_task(
   rex_tcb_type *p_tcb
)
{
   p_tcb->suspended = TRUE;
   REX_INTLOCK();
      if ( ( p_tcb == rex_curr_task ) && !rex_is_in_irq_mode( ) )
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #else
         rex_set_best_task( REX_TASK_LIST_FRONT() );
         rex_sched( );
         #endif
      }
      else
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #endif
      }
   REX_INTFREE();

   return;
} /* END rex_suspend_task */


/*===========================================================================
FUNCTION REX_RESUME_TASK

DESCRIPTION
  Make a suspended task eligible for scheduling.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  If the task being resumed is higher priority than the calling task and it
  is not blocked for some other reason, then a context switch will occur.

===========================================================================*/
void rex_resume_task(
  rex_tcb_type         *p_tcb
)
{
   REX_INTLOCK();

   /* basic sanity check to see if we should even be here or not */
   if (p_tcb->suspended == TRUE)
   {
      p_tcb->suspended = FALSE;

      if ((p_tcb->pri > rex_best_task->pri) && REX_TASK_RUNNABLE(p_tcb))
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #else
         rex_best_task = p_tcb;
         rex_sched();
         #endif
      }
      else
      {
         #ifdef FEATURE_REX_SYS_SIG
#error code not present
         #endif
      }
   }

   REX_INTFREE();

   return;
} /* END rex_resume_task */


/*===========================================================================
FUNCTION REX_REMOVE_TASK

DESCRIPTION
  Function that implements the process of removing a TCB from
  the REX task list. Interrupts must be locked before calling this function.
===========================================================================*/
void rex_remove_task(
  rex_tcb_type *tcb_ptr   /* pointer to tcb */
)
{
   rex_tcb_type *prev_tcb_ptr;
   rex_tcb_type *next_tcb_ptr;

   prev_tcb_ptr = REX_TASK_LIST_PREV( tcb_ptr );
   next_tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );

   if ( ( prev_tcb_ptr == NULL                 ||
      prev_tcb_ptr->pri != tcb_ptr->pri  ) &&
      next_tcb_ptr != NULL                 &&
      next_tcb_ptr->pri == tcb_ptr->pri     )
   {
      /* tcb_ptr is the priority representative for one or more other
      ** threads with the same priority. Once tcb_ptr is removed, the
      ** priority representative for these threads will be the thread
      ** that follows tcb_ptr in the thread list.
      */
      rex_tcb_type *temp_tcb_ptr = next_tcb_ptr;
      while ( temp_tcb_ptr->pri == tcb_ptr->pri )
      {
         temp_tcb_ptr->pri_rep_ptr = next_tcb_ptr;
         temp_tcb_ptr = REX_TASK_LIST_NEXT( temp_tcb_ptr );
      }
   }

   REX_TASK_LIST_POP( tcb_ptr );

   tcb_ptr->link.prev_ptr = NULL;
   tcb_ptr->link.next_ptr = NULL;

   return;
} /* END rex_remove_task */


/*===========================================================================
FUNCTION REX_KILL_TASK

DESCRIPTION
  Kill a specified task by yanking its TCB from the active list.

DEPENDENCIES
  None.

RETURN VALUE
  Void in normal cases.
  If a thread calls rex_kill_task on itself, then this function will not
  return at all (as the thread goes away).

===========================================================================*/
void rex_kill_task_ext(
   rex_tcb_type *p_tcb,
   boolean       schedule_new_task
)
{
   #ifdef FEATURE_REX_CREATE_TASK
   boolean is_dynamic = p_tcb->is_dynamic;
   #endif

   #ifdef FEATURE_ZREX
#error code not present
   #endif

   #ifdef REX_INTERCEPT_KILL_TASK
   REX_INTERCEPT_KILL_TASK( p_tcb );
   #endif

   REX_INTLOCK();
   TASKLOCK();

   #ifdef T_WINNT
#error code not present
   #endif

   /* Task is alive only if it is still linked into TCB list.
   */
   if ( (p_tcb->link.prev_ptr != NULL ) || (p_tcb->link.next_ptr != NULL) )
   {
      #ifdef FEATURE_ZREX
#error code not present
      #endif

      #ifdef FEATURE_REX_PROFILE
      /* Change rextp.per_task_tcb_ptr if required */
      rextp_remove_task ( p_tcb );
      #endif /*  FEATURE_REX_PROFILE */

      /* Remove TCB from the task list.
      */
      rex_remove_task( p_tcb );

      #ifdef FEATURE_REX_IPC
      ipcns_node_deregister(p_tcb);
      #endif

      /* Remove REX timers associated with the task from the timer list.
      */
      rex_delete_task_timers( p_tcb );

      /* Tell Dog to stop monitoring this task.
      */
      REX_PAUSE_DOG_MONITOR( p_tcb );

      /* Check if we were holding or waiting on a critical section */
      while (p_tcb->cs_sp >= p_tcb->cs_stack)
      {
         if ( p_tcb->cs_link.next_ptr == NULL) /* holding crit section */
         {
            /* free the crit section, but don't call rex_sched() yet */
            rex_leave_crit_sect_internals( *p_tcb->cs_sp, p_tcb, FALSE);
         }
         else /* we were waiting on the list */
         {
            /* if item is first on the list, fix up list head */
            if (p_tcb->cs_link.prev_ptr == REX_CRIT_SECT_FLAG)
            {
               (*p_tcb->cs_sp)->tcb_link = p_tcb->cs_link.next_ptr;
            }
            else /* fix up previous item on list */
            {
               p_tcb->cs_link.prev_ptr->cs_link.next_ptr =
                  p_tcb->cs_link.next_ptr;
            }

            /* if item is NOT the last on the list */
            if (p_tcb->cs_link.next_ptr != REX_CRIT_SECT_FLAG)
            {
               p_tcb->cs_link.next_ptr->cs_link.prev_ptr =
                  p_tcb->cs_link.prev_ptr;
            }
            --p_tcb->cs_sp;
         }
      } /* END we needed to deal with crit section */

      rex_num_tasks--;

      if( schedule_new_task )
      {
         /* if the calling task is killing itself, and holding the task lock,
         ** then release the lock.
         */
         if (p_tcb == rex_curr_task)
         {
            /* If the task to be killed is holding the task lock, we need to
            ** release that lock.  It turns out that only if a task is killing
            ** itself is this situation possible. We check for this case in the
            ** next few lines of code.  To clarify, the other situation is where
            ** task A is calling rex_kill_task to kill task B.  We are guaranteed
            ** that task B does not hold the task lock, otherwise task A, which is
            ** issuing the rex_kill_task(), couldn't possibly be running.
            */
            if (rex_nest_depth > 0)
            {
               rex_nest_depth = 0;
               rex_sched_allow = TRUE;
            }
         } /* end-if task was killing itself */

         /* At this point, we've possibly given a critical section to a task that
         ** had been waiting, possibly released a TASKLOCK, and possibly had a
         ** calling task kill itself, all of which could require a scheduling change.
         ** It makes sense to search for a ready task from the top of the list.
         */
         rex_set_best_task( REX_TASK_LIST_FRONT() );
         rex_sched();
      } /* END needed to reschedule */
   } /* END TCB was still in active list */

   #ifdef FEATURE_REX_CREATE_TASK
   /* For a dynamically created task, free the stack and
   ** TCB memories
   */
   if( is_dynamic == TRUE )
   {
      rex_free( p_tcb->stack_limit );
      rex_free( p_tcb );
   }
   #endif

   TASKFREE();
   REX_INTFREE();
   return;
} /* END rex_kill_task_ext */


/*===========================================================================
FUNCTION REX_KILL_TASK

DESCRIPTION
  Kill the current task by yanking its TCB from the active list.
  If a task calls rex_kill_task on itself, then this function will not
  return at all (as the task goes away).

===========================================================================*/
void rex_kill_task(
  rex_tcb_type *p_tcb
)
{
  rex_kill_task_ext( p_tcb, TRUE );
  return;
} /* END rex_kill_task */


/*===========================================================================
FUNCTION REX_SELF

DESCRIPTION
  Returns the task control block for the current task

DEPENDENCIES
  None.

RETURN VALUE
  The task control block for the current task

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type *rex_self( void )
{
   /*-------------------------------------------------------
   ** The currently running task is in rex_curr_task
   **-----------------------------------------------------*/
#ifdef REX_INTERCEPT_SELF
   return REX_INTERCEPT_SELF( );
#else
   return rex_curr_task;
#endif
} /* END rex_self */


/*===========================================================================
FUNCTION REX_SET_PRI

DESCRIPTION
  Sets the priority of the calling task.

DEPENDENCIES
  None.

RETURN VALUE
  The priority before it was changed.

SIDE EFFECTS
  The priority change will cause a change in the whole scheduling order for
  all of the tasks. This could result in an imediate task swap.
===========================================================================*/
rex_priority_type rex_set_pri(
   rex_priority_type p_pri       /* the new priority */
)
{
   /*-------------------------------------------------------
   ** A wrapper function that just calls rex_task_pri with
   ** the current task
   **-----------------------------------------------------*/
   return rex_task_pri(rex_curr_task, p_pri);
} /* END rex_set_pri */


/*===========================================================================
FUNCTION REX_TASK_PRI

DESCRIPTION
  Sets the priority of the passed in task.

DEPENDENCIES
  p_tcb must be a valid task control block

RETURN VALUE
  The priority before it was changed.

SIDE EFFECTS
  The priority change will cause a change in the whole scheduling order for
  all of the tasks. This could result in an imediate task swap.
===========================================================================*/
rex_priority_type rex_task_pri(
  rex_tcb_type       *p_tcb,    /* tcb to set priority on      */
  rex_priority_type  p_pri      /* the new priority            */
)
{
   rex_priority_type prev_pri   = p_tcb->pri;   /* the priority before the set */
   #ifndef FEATURE_ZREX
   rex_tcb_type  *search_ptr;                /* temp pointer                */
   #else
#error code not present
   #endif
   boolean comp = FALSE;                        /* Comparator */

   REX_INTLOCK();
#ifdef FEATURE_ZREX
#error code not present
#endif
   comp = (p_pri == p_tcb->pri);
   REX_INTFREE();

   /* Return if the priority is the same
   */
   if( comp )
   {
      return prev_pri;
   }
#if (defined FEATURE_ZREX && defined FEATURE_REX_CRIT_SECT_DEBUG)
#error code not present
#endif

   REX_INTLOCK();

#ifdef FEATURE_ZREX
#error code not present
#endif

   /* Remove from the list and re insert in a new position
   ** based upon the new priority
   */
#ifdef FEATURE_ZREX
#error code not present
#else
   p_tcb->link.next_ptr->link.prev_ptr = p_tcb->link.prev_ptr;
   p_tcb->link.prev_ptr->link.next_ptr = p_tcb->link.next_ptr;
#endif

   p_tcb->pri = p_pri;

   /* Find the task with the priority <= the new priority.
   ** Insert in front of this task
   ** rex_idle_task (the kernel task) is at the end of the list
   ** with priority 0 and therefore no need to check for null ptrs.
   */
#ifdef FEATURE_ZREX
#error code not present
#else
   search_ptr = rex_task_list.link.next_ptr;
   while(search_ptr->pri > p_pri) {
      search_ptr = search_ptr->link.next_ptr;
   }

   p_tcb->link.prev_ptr = search_ptr->link.prev_ptr;
   p_tcb->link.next_ptr = search_ptr;
   search_ptr->link.prev_ptr->link.next_ptr = p_tcb;
   search_ptr->link.prev_ptr = p_tcb;
#endif

   /* If the task is runnable, and its priority is now higher than
   ** that of rex_best_task, then we can context switch it in.
   */
   if ( (p_pri > rex_best_task->pri) && ( REX_TASK_RUNNABLE(p_tcb) ) )
   {
      rex_best_task = p_tcb;
      rex_sched();
   }

   REX_INTFREE();
   return prev_pri;
} /* END rex_task_pri */


/*===========================================================================
FUNCTION REX_GET_PRI

DESCRIPTION
  Return the priority of the current task.

DEPENDENCIES
  None.

RETURN VALUE
  The priority of the current task.

SIDE EFFECTS
  None.
===========================================================================*/
rex_priority_type rex_get_pri( void )
{
  /*-------------------------------------------------------
  ** Just return the priority field of the current task
  **-----------------------------------------------------*/
  return rex_curr_task->pri;
} /* END rex_get_pri */


/*===========================================================================
FUNCTION REX_INIT

DESCRIPTION
  Initializes REX. It initializes the kernel task and calls the p_task main
  rex function.

DEPENDENCIES
  p_tcb must be a valid task control block.
  p_task must be a valid function pointer.

RETURN VALUE
  None.

SIDE EFFECTS
  When the initialization is done, this thread will wait until
  the exit event is triggered at which point it will clean up
  and return.
===========================================================================*/
void rex_init(
   void *              p_istack,           /* interrupt stack      */
   rex_stack_size_type p_istksiz,          /* interrupt stack size */
   rex_tcb_type        *p_tcb,             /* task control block   */
   void *              p_stack,            /* stack                */
   rex_stack_size_type p_stksiz,           /* stack size           */
   rex_priority_type   p_pri,              /* task priority        */
   void                (*p_task)( dword ), /* task function        */
   dword               p_param             /* task parameter       */
)
{
   #ifdef FEATURE_REX_INTERNAL_ACCESS
   void rex_access_init( void );
   rex_access_init();
   #endif

   /*-------------------------------------------------------
   ** Change to Supervisor mode
   **-----------------------------------------------------*/
   #ifdef T_WINNT
#error code not present
   #else
   #ifdef FEATURE_REX_PC_DUMP
#error code not present
   #endif
   (void) rex_set_cpsr( PSR_Supervisor | PSR_Irq_Mask | PSR_Fiq_Mask );
   #endif

   #if defined( FEATURE_ZREX )
#error code not present
   #endif

   #if defined ( FEATURE_REX_DYNA_MEM )
   rex_mem_init( );
   #endif

   #ifdef FEATURE_DAC_MEMORY_PROTECTION
   rex_init_crit_sect( &rex_dac_crit_sect );
   #endif

   /*-------------------------------------------------------
   ** Setup the interrupt stack.
   **-----------------------------------------------------*/

   REX_INTLOCK();

   /* Point to the top of the stack */
   rex_int_stack = (rex_stack_word_type *) p_istack;

   /*-------------------------------------------------------
   ** Initialize the interrupt nest level at zero
   **-----------------------------------------------------*/
   rex_int_nest_level = 0;

   /*-------------------------------------------------------
   ** Initialize the timer list
   **-----------------------------------------------------*/
   rex_init_timer_list( );

   /*-------------------------------------------------------
   ** Initialize the task list.
   **-----------------------------------------------------*/
   #ifdef FEATURE_ZREX
#error code not present
   #else
   rex_task_list.link.next_ptr  = &rex_kernel_tcb;
   rex_task_list.link.prev_ptr  = NULL;
   rex_kernel_tcb.link.next_ptr = NULL;
   rex_kernel_tcb.link.prev_ptr = &rex_task_list;
   #endif

   REX_INTERCEPT_INIT_READY();

   /*-------------------------------------------------------
   ** rex_curr_task must be equal to rex_best_task to avoid
   ** scheduling at this moment
   **-----------------------------------------------------*/
   rex_curr_task = &rex_kernel_tcb;
   rex_best_task = &rex_kernel_tcb;

   #ifdef FEATURE_ZREX
#error code not present
   #endif

   #ifdef TIMETEST
   rex_kernel_tcb.leds = TIMETEST_REX_TASK_ID;
   #endif

   rex_def_task_ext(
      &rex_kernel_tcb,                            /* tcb        */
      (unsigned char *) &rex_kernel_stack[0],     /* stack      */
      REX_KERNEL_STACK_SIZE,                      /* stack size */
      0,                                          /* priority   */
      rex_idle_task,                              /* function   */
      0,                                          /* arguments  */
      "REX Idle Task",                            /* task name  */
      FALSE                                       /* suspended  */
   );

   rex_kernel_tcb.pri = 0xFFFFFFFF;

   #ifdef FEATURE_ZREX
#error code not present
   #endif

   /* Start REX CPU usage monitoring in WinNT only
   */
   #if defined( T_WINNT ) && defined( FEATURE_REX_PROFILE )
#error code not present
   #endif

   #ifdef FEATURE_REX_DPC

   #ifdef TIMETEST
   rex_dpc_tcb.leds = TIMETEST_DPC_TASK_ID;
   #endif

      rex_def_task_ext(
         &rex_dpc_tcb,
         (unsigned char *) rex_dpc_stack,
         REX_DPC_STACK_SIZE,
         REX_DPC_THREAD_PRI,
         rex_dpc_task,
         0,
         "REX DPC Task",
         FALSE
      );
   #endif

   #ifdef FEATURE_REX_STAT
#error code not present
   #endif

   rex_kernel_tcb.pri = 0;

   /*-------------------------------------------------------
   ** rex_curr_task must be equal to rex_best_task to avoid
   ** scheduling at this moment
   **-----------------------------------------------------*/
   rex_curr_task = p_tcb;
   rex_best_task = p_tcb;

   /*-------------------------------------------------------
   ** Define the user first task for the main control task
   **------------------------------------------------------*/
   rex_def_task_ext(
      p_tcb,                                   /* tcb        */
      (unsigned char *)p_stack,                /* stack      */
      p_stksiz*sizeof(rex_stack_word_type),    /* stack size */
      p_pri,                                   /* priority   */
      p_task,                                  /* function   */
      p_param,                                 /* arguments  */
      "Main Task",                             /* name       */
      FALSE
   );

   REX_INTFREE();

   rex_set_best_task( REX_TASK_LIST_FRONT() );

   rex_curr_task = rex_best_task;

   rex_init_complete = 1;

   rex_start_task( rex_best_task );

} /* END rex_init */


/*===========================================================================
FUNCTION REX_FIRST_TASK

DESCRIPTION
  Assign input iterator to first task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the first task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_first_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_FRONT();
   return *iter;
} /* END rex_first_task */


/*===========================================================================
FUNCTION REX_NEXT_TASK

DESCRIPTION
  Assign input iterator to the next task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the next task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_next_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_NEXT((*iter));
   return *iter;
} /* END rex_next_task */


/*===========================================================================
FUNCTION REX_PREV_TASK

DESCRIPTION
  Assign input iterator to the prev task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the prev task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_prev_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_PREV((*iter));
   return *iter;
} /* END rex_prev_task */


/*===========================================================================
FUNCTION REX_SET_INTERRUPT_VECTOR

DESCRIPTION
  Given the id v of an installable vector and a pointer to the routine
  to be called when the interrupts occurs, this function installs the routine

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rex_set_interrupt_vector (
   rex_vect_type v,                /* Vector                    */
   void (*fnc_ptr)( void )         /* *function to be installed */
)
{
   if (v == P_IRQ_VECT)
   {
     rex_irq_vector =  fnc_ptr;
   }
   else
   {
     rex_fiq_vector =  fnc_ptr;
   }
} /* END rex_set_interrupt_vector */


#if defined FEATURE_STACK_CHECK
#include "err.h"

/*===========================================================================
FUNCTION REX_REPORT_STACK_OVERFLOW

DESCRIPTION
  This function is called by the stack overflow handlers.
  Those handlers are in assembly, whereas ERR_FATAL is available only in C.

  The ERR_FATAL message prints the stack pointer, link register, processor
  mode (LSW) and current task pointer (LSW).

DEPENDENCIES
  None.

RETURN VALUE
  None. The application will stop here.

SIDE EFFECTS
  None.
===========================================================================*/
void rex_report_stack_overflow(
   dword sp,
   dword lr,
   dword mode
)
{
   ERR_FATAL(
      "STACK OVERFLOW. %lx | %lx | %lx.",
      sp, lr, ((mode << 16)|((dword)rex_curr_task & 0xffff))
   );
} /* END rex_report_stack_overflow */

#endif /* FEATURE_STACK_CHECK */

#ifdef FEATURE_REX_DYNA_MEM
/* ==================================================================
FUNCTION REX_MEM_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
void rex_mem_init( void )
{
   mem_init_heap(
                 &rex_heap,
                 rex_mem_buffer,
                 sizeof( rex_mem_buffer ),
                 NULL
   );
   return;
} /* END rex_mem_init */


/* ==================================================================
FUNCTION REX_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *rex_malloc(
   unsigned long num_bytes
)
{
   ASSERT( !rex_is_in_irq_mode( ) );

   return mem_malloc( &rex_heap, num_bytes );
} /* END rex_malloc */


/* ==================================================================
FUNCTION REX_CALLOC
DESCRIPTION
   Dynamically allocates a specified number of blocks of the specified
   size and returns a pointer to this chunk of memory.
===================================================================== */
void *rex_calloc(
   unsigned long num_blocks,
   unsigned long block_size
)
{
   ASSERT( !rex_is_in_irq_mode( ) );

   return mem_calloc( &rex_heap, num_blocks, block_size );
} /* END rex_calloc */


/* ==================================================================
FUNCTION REX_REALLOC
DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===================================================================== */
void *rex_realloc(
   void         *ptr,
      /* Block previously allocated via REX allocation call
      */
   unsigned long size
      /* New size (in bytes)
      */
)
{
   ASSERT( !rex_is_in_irq_mode( ) );

   return mem_realloc( &rex_heap, ptr, size );
} /* END rex_calloc */


/* ==================================================================
FUNCTION REX_FREE
DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
void rex_free(
   void *mem_ptr
)
{
   ASSERT( !rex_is_in_irq_mode( ) );

   mem_free( &rex_heap, mem_ptr );
   return;
} /* END rex_free */

#endif /* FEATURE_REX_DYNA_MEM */

#ifdef FEATURE_REX_DPC
/* ==================================================================
FUNCTION REX_EXECUTE_NEXT_DPC
DESCRIPTION
   Reads the next DPC from the DPC ring buffer and executes it.
   The read index will be updated.
   Returns TRUE if a DPC was executed, FALSE otherwise.
   NOTE: This function is for internal REX use only. It must be
   called with interrupts locked.
===================================================================== */
static boolean rex_execute_next_dpc( void )
{
   rex_dpc_func_type *dpc_ptr;
   unsigned long      dpc_param1 = 0;
   unsigned long      dpc_param2 = 0;
   unsigned long      dpc_param3 = 0;
   boolean            dpc_found;

   /* Lock interrupts while playing with the DPC ring */
   REX_INTLOCK();

   if( rex_dpc_rd_index != rex_dpc_wr_index )
   {
     dpc_ptr    = rex_dpc_ring[ rex_dpc_rd_index ].dpc_ptr;
     dpc_param1 = rex_dpc_ring[ rex_dpc_rd_index ].dpc_param1;
     dpc_param2 = rex_dpc_ring[ rex_dpc_rd_index ].dpc_param2;
     dpc_param3 = rex_dpc_ring[ rex_dpc_rd_index ].dpc_param3;
     rex_dpc_rd_index = (rex_dpc_rd_index+1) & REX_DPC_RING_MASK;
     dpc_found = TRUE;
   }
   else
   {
     dpc_ptr = NULL;
     dpc_found = FALSE;
   }

   REX_INTFREE();

   /* Execute the DPC with interrupts freed */
   if( dpc_ptr != NULL )
   {
     (void) dpc_ptr( dpc_param1, dpc_param2, dpc_param3 );
   }

   return dpc_found;

} /* END rex_execute_next_dpc */


/* ==================================================================
FUNCTION  REX_QUEUE_DPC
DESCRIPTION
   A function that executes in a context which is deferred from
   interrupt context.  This can only be called at interrupt context
   and otherwise causes a diagnostics to be logged and the requested
   call to be ignored.

   Operates much like an APC except is executed in the context of a
   REX owned thread.
===================================================================== */
boolean rex_queue_dpc(
   rex_dpc_func_type *dpc_ptr,
      /* Pointer to the function to call
      */
   unsigned long      param1,
      /* Parameter to pass to the dpc
      */
   unsigned long      param2,
      /* Parameter to pass to the dpc
      */
   unsigned long      param3
      /* Parameter to pass to the dpc
      */
)
{
   boolean ret_status = 0;

   if ( dpc_ptr == NULL )
   {
      return 0;
   }

   REX_INTLOCK();
      /* Make sure there is space in the ring buffer, add the DPC to the
      ** ring and signal the DPC task.
      */
      if( ((rex_dpc_wr_index+1) & REX_DPC_RING_MASK) != rex_dpc_rd_index )
      {
         rex_dpc_ring[ rex_dpc_wr_index ].dpc_ptr    = dpc_ptr;
         rex_dpc_ring[ rex_dpc_wr_index ].dpc_param1 = param1;
         rex_dpc_ring[ rex_dpc_wr_index ].dpc_param2 = param2;
         rex_dpc_ring[ rex_dpc_wr_index ].dpc_param3 = param3;
         rex_dpc_wr_index = (rex_dpc_wr_index+1) & REX_DPC_RING_MASK;

         (void) rex_set_sigs( &rex_dpc_tcb, REX_DPC_SIG );
         ret_status = 1;
      }
      else
      {
         ret_status = 0;
      }
   REX_INTFREE();

   return ret_status;
} /* END rex_queue_dpc */


/* ==================================================================
FUNCTION REX_DPC_TASK
DESCRIPTION
  Loops executing DPC's while DPC's are present.  Otherwise blocks
  waiting for DPC's to be added to the DPC ring buffer.
===================================================================== */
void rex_dpc_task(
   unsigned long param
      /*lint -esym(715,param)*/
)
{
   for(;;)
   {
      /* Block waiting for a DPC to be ready
      */
      (void) rex_wait( REX_DPC_SIG );
      (void) rex_clr_sigs( rex_self( ), REX_DPC_SIG );

      /* Execute DPC's until the ring buffer is empty
      */
      while( rex_execute_next_dpc( ) == TRUE );
   }
} /* END rex_dpc_task */
#endif /* FEATURE_REX_DPC */


/* ==================================================================
FUNCTION REX_IS_IN_IRQ_MODE

DESCRIPTION
   Returns TRUE if the current mode is IRQ mode or System mode
   and FALSE otherwise.
===================================================================== */
boolean rex_is_in_irq_mode( void )
{
#ifdef REX_INTERCEPT_IS_IN_IRQ_MODE
   return REX_INTERCEPT_IS_IN_IRQ_MODE( );
#else
   {
      unsigned long cpsr_val = rex_get_cpsr( );
      return ( ( ( cpsr_val & PSR_Mode_Mask ) == PSR_Irq ) ||
               ( ( cpsr_val & PSR_Mode_Mask ) == PSR_System ) ) ? TRUE : FALSE;
   }
#endif
} /* END rex_is_in_irq_mode */


/*===========================================================================
FUNCTION REX_ASSIGN_BEST_TASK

DESCRIPTION
  Sets the rex_best_task variable. The user must ensure that the input
  tcb is indeed the highest-priority runnable task.
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.
===========================================================================*/
void rex_assign_best_task(
   rex_tcb_type *tcb_ptr
)
{
   if ( tcb_ptr != NULL )
   {
      rex_best_task = tcb_ptr;
   }
   return;
} /* END rex_assign_best_task */

/* ==================================================================
FUNCTION REX_SET_BEST_TASK_FROM_ALL

DESCRIPTION
   Searches the entire task list and sets the rex_best_task variable.

   NOTE: Interrupts must be locked when this function is called.
===================================================================== */
void rex_set_best_task_from_all( void )
{
   rex_set_best_task( REX_TASK_LIST_FRONT() );
   return;
} /* END rex_set_best_task_from_all */


#ifdef FEATURE_REX_PROFILE
/* --------------------------------------------------------------------------
**
** REX Profile Support
**
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION REX_ENABLE_PROFILING

DESCRIPTION
   Turns on profiling.
===================================================================== */
void rex_enable_profiling( void )
{
   rex_profiling_is_enabled = TRUE;
} /* END rex_enable_profiling */


#ifdef FEATURE_CPU_BASED_FLOW_CONTROL
/* ==================================================================
FUNCTION REX_DISABLE_PROFILING

DESCRIPTION
   Turns on profiling.
===================================================================== */
void rex_disable_profiling( void )
{
   if (!rex_sleep_reporting_is_enabled)
   {
      rex_profiling_is_enabled = FALSE;
   }
   else
   {
      rex_profiling_to_be_disabled = TRUE;
   }
} /* END rex_disable_profiling */

#else

/* ==================================================================
FUNCTION REX_DISABLE_PROFILING

DESCRIPTION
   Turns on profiling.
===================================================================== */
void rex_disable_profiling( void )
{
   rex_profiling_is_enabled = FALSE;
} /* END rex_disable_profiling */
#endif


/* ==================================================================
FUNCTION REX_GET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Returns the total number of time samples.
===================================================================== */
unsigned long rex_get_total_time_samples( void )
{
   return rex_total_time_samples;
} /* END rex_get_total_time_samples */

/* ==================================================================
FUNCTION REX_SET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Alters the total number of time samples to the parameter value.
===================================================================== */
void rex_set_total_time_samples( unsigned long new_sample_value )
{
   rex_total_time_samples = new_sample_value;
} /* END rex_get_total_time_samples */

#ifdef FEATURE_CPU_BASED_FLOW_CONTROL

/* ==================================================================
FUNCTION REX_GET_SLEEP_REPORT

DESCRIPTION
===================================================================== */
rex_sleep_report_type rex_get_sleep_report(void)
{
   rex_tcb_type *tcb_ptr = NULL;

   rex_sleep_report.sleep_time_samples =
     rex_sleep_report.total_time_samples = 0;

   if (rex_sleep_reporting_is_enabled)
   {
     tcb_ptr = &rex_task_list;

     while (tcb_ptr != NULL)
     {
       if (tcb_ptr == &sleep_tcb)
       {
         rex_sleep_report.sleep_time_samples = tcb_ptr->time_samples;
         rex_sleep_report.total_time_samples = rex_get_total_time_samples();
         break;
       }

       tcb_ptr = tcb_ptr->link.next_ptr;
     }
   }

   return rex_sleep_report;
}

/* ==================================================================
FUNCTION REX_ENABLE_SLEEP_REPORTING

DESCRIPTION
===================================================================== */
void rex_enable_sleep_reporting(void)
{
   if (!rex_profiling_is_enabled)
   {
      rex_profiling_is_enabled = TRUE;
   }

   rex_sleep_reporting_is_enabled = 1;
}


/* ==================================================================
FUNCTION REX_DISABLE_SLEEP_REPORTING

DESCRIPTION
===================================================================== */
void rex_disable_sleep_reporting(void)
{
   if (rex_profiling_to_be_disabled)
   {
     rex_profiling_to_be_disabled = rex_profiling_is_enabled = FALSE;
   }

   rex_sleep_reporting_is_enabled = 0;
}

#endif /* FEATURE_CPU_BASED_FLOW_CONTROL */

#endif /* FEATURE_REX_PROFILE */

#ifdef FEATURE_REX_PC_DUMP
#error code not present
#endif /* FEATURE_REX_PC_DUMP */

#if defined FEATURE_REX_SYS_SIG
#error code not present
#endif /* FEATURE_REX_SYS_SIG */

#if defined (REX_ASSERT_ENABLE) || defined (T_WINNT)

#ifdef T_WINNT
#error code not present
#endif

/* ==================================================================
FUNCTION REX_EXIT
DESCRIPTION
   Called when a REX assertion fails. May eventually shut the system
   down, but for now is meant to be used as a place to set a breakpoint
   to detect a REX assertion failure.
===================================================================== */
void rex_exit(
   int condition
)
{
   static int err_cond;
   err_cond = condition;

   #ifdef T_WINNT
#error code not present
   #endif

   return;
} /* END rex_exit */

#endif /* defined (REX_ASSERT_ENABLE) || defined (T_WINNT) */

/* ==================================================================
FUNCTION REX_GET_VERSION_NUMBER
DESCRIPTION
   Returns a REX version number. The version number returned is a
   decimal number in which the least significant two digits indicate
   the minor version number and the remaining digits (at most two)
   indicate the major version number. For instance, if the value
   returned is 307, the major version number is 3 and the minor version
   number is 7.
===================================================================== */
unsigned long rex_get_version_number(
   void
)
{
   return REX_VERSION_NUMBER;
} /* END rex_get_version_number */


#if defined FEATURE_SOFTWARE_PROFILE
/*===========================================================================

FUNCTION REX_CLR_PROFILE

DESCRIPTION
  Clears stored profiling information. Clears the numticks and
  rex_IRQnumticks fields in each TCB in the task list. The rex_pnumticks
  does not need to be cleared because it would be in sync with rex_time_tick2.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_clr_profile( void )
{
   rex_tcb_type *tcb_ptr = NULL;

   /* atomic operation */

   REX_INTLOCK();

   rex_task_list.numticks = 0;

   /*-------------------------------------------------------
   ** Traverse the rex_task_list and clear each task's numticks
   ** field of the rex_tcb_struct
   **-----------------------------------------------------*/

   tcb_ptr = rex_task_list.link.next_ptr;
   while (tcb_ptr != NULL) {
      tcb_ptr->numticks = 0;
      tcb_ptr =  tcb_ptr->link.next_ptr;
   }

   /*-------------------------------------------------------
   ** Clear the interrupt numticks.  All interrupts are considered
   ** as one task with the task LED denoted by TIMETEST_INT
   **-----------------------------------------------------*/
   rex_IRQnumticks = 0;

   REX_INTFREE();

   return;
} /* END rex_clr_profile */


/*===========================================================================

FUNCTION REX_INIT_PROFILE

DESCRIPTION
  Performs initialization necessary for software profiling. Sets the
  rex_time_tick2 variable to the address passed in.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_init_profile(
   dword rex_time_tick2_addr
)
{
   rex_time_tick2 = rex_time_tick2_addr;
   return;
} /* END rex_init_profile */


/*===========================================================================

FUNCTION REX_ENABLE_PROFILE

DESCRIPTION
  Enables software profiling by setting rex_enable_profile, clearing
  rex_pnumticks and restarting the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_enable_profile( void )
{
   /* atomic operation */

   REX_INTLOCK();

   rex_profile_enabled = 1; /* true; cmp in rexarm.s */
   rex_pnumticks = 0;

   /* 16 bits counter, ~30 khz clock ~30 usec tick, ~2 sec */
   outpw(rex_time_tick2, 0x0);
   outpw(rex_time_tick2, 0xffff);

   REX_INTFREE();

   return;
} /* END rex_enable_profile */


/*===========================================================================

FUNCTION REX_DISABLE_PROFILE

DESCRIPTION
  Disables software profiling by resetting rex_profile_enabled, clearing
  rex_pnumticks and stopping the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_disable_profile( void )
{
   /* atomic operation */

   REX_INTLOCK();

   rex_profile_enabled = 0; /* false; cmp in rexarm.s */
   rex_pnumticks = 0;
   outpw(rex_time_tick2, 0x0);

   REX_INTFREE();

   return;
} /* END rex_disable_profile */

/*===========================================================================

FUNCTION REX_GET_PROFILE

DESCRIPTION
  Retrieves software profiling information. Traverses rex_task_list and gets
  the numticks field from each TCB in the task list. In addition, gets
  rex_IRQnumticks as well.

DEPENDENCIES
  Assumes that the TCB of each task is in rex_task_list.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_get_profile(
   sw_profile_type *sw_profile,
   byte            *num_task
)
{
   rex_tcb_type *tcb_ptr = NULL;
   byte i;

   REX_INTLOCK();

   sw_profile[0].tick = rex_IRQnumticks;
   memcpy(sw_profile[0].name,(byte*)"IRQ ",4);
   tcb_ptr = rex_task_list.link.next_ptr;

   for (i = 1; tcb_ptr != NULL && i < REX_MAX_PROFILE_ENTRY; i++)
   {
      sw_profile[i].tick = tcb_ptr->numticks;
      memcpy(sw_profile[i].name,tcb_ptr->task_name,4);
      tcb_ptr =  tcb_ptr->link.next_ptr;
   }

   *num_task = i;

   REX_INTFREE();

   return;
} /* END rex_get_profile */

#endif /* FEATURE_SOFTWARE_PROFILE */

#ifdef FEATURE_REX_INTERNAL_ACCESS
/* -------------------------------------------------------------------
** REX Accessor Function
**   Provides external component access to REX internals.  Used with
**   REXNT to provide programmatic access to REX internals w/o compile
**   time knowledge of selected REX features or build flags.  REXNT then
**   can be built independent of the feature set for REX including allowance
**   for differences in structure sizes.
** ------------------------------------------------------------------- */
#include "rexnt_access.h"

/* ==========================================================================
FUNCTION REX_ACCESS
DESCRIPTION
   Programmatic access to REX internals independent of the feature set used
   to compile REX.  This function is passed to REXNT at startup during
   rex_init.  Must be installed before any other calls into REX that lead
   to calls into REXNT (that lead back to REX).

   See rexnt_access.h for details on the usage and values for each parameter.
============================================================================= */
unsigned long rex_access(
   unsigned long  cmd_id,
   unsigned long  item_id,
   void          *item_ptr,
   unsigned long  value
)
{
   unsigned long    ret_val = 0;
   rex_tcb_type    *tcb_ptr;
   rex_tcb_type   **tcb_list_ptr;
   rex_tcb_type    *temp_tcb_ptr;
   rex_timer_type  *tmr_ptr;
   int              i;

   tcb_ptr = (rex_tcb_type*)item_ptr;
   tmr_ptr = (rex_timer_type*)item_ptr;

   switch( cmd_id )
   {
      case REXNT_CMD_GET:
           switch( item_id )
           {
              case REXNT_ITEM_TCB_THREAD_ID:
                   if( tcb_ptr != NULL )
                   {
                      ret_val = (unsigned long)tcb_ptr->thread_id;
                   }
                   else
                   {
                      ret_val = 0;
                   }
                   break;

              case REXNT_ITEM_TCB_NUM_APCS:
                   #ifdef FEATURE_REX_APC
                   ret_val = (unsigned long)tcb_ptr->num_apcs;
                   #else
                   ret_val = 0;
                   #endif
                   break;

              case REXNT_ITEM_TCB_PRI:
                   ret_val = (unsigned long)tcb_ptr->pri;
                   break;

              case REXNT_ITEM_TCB_THREAD_ID_ADDR:
                   ret_val = (unsigned long)&tcb_ptr->thread_id;
                   break;

              case REXNT_ITEM_TCB_LIST:
                   REX_INTLOCK();
                      tcb_list_ptr = (rex_tcb_type**)item_ptr;
                      i = 0;
                      temp_tcb_ptr = REX_TASK_LIST_FRONT();
                      while( temp_tcb_ptr != NULL )
                      {
                         tcb_list_ptr[ i++ ] = temp_tcb_ptr;
                         temp_tcb_ptr = REX_TASK_LIST_NEXT( temp_tcb_ptr );
                      }
                      tcb_list_ptr[ i ] = NULL;
                      ret_val = 0;
                   REX_INTFREE();
                   break;

              case REXNT_ITEM_TICK_ENABLED:
                   #ifdef FEATURE_REX_TICK_CONTROL
#error code not present
                   #else
                   ret_val = 1;
                   #endif
                   break;

              case REXNT_ITEM_TCB_EXT:
                   #ifdef FEATURE_REX_EXTENDED_CONTEXT
#error code not present
                   #else
                   ret_val = 0;
                   #endif
                   break;

              case REXNT_ITEM_TIMER_EXT:
                   #ifdef FEATURE_REX_EXTENDED_TIMER
#error code not present
                   #else
                   ret_val = 0;
                   #endif
                   break;

              case REXNT_ITEM_TCB_SIGS_ADDR:
                   ret_val = (unsigned long)&tcb_ptr->sigs;
                   break;

              case REXNT_ITEM_TIMER_COUNT_ADDR:
                   ret_val = (unsigned long)&tmr_ptr->cnt;
                   break;

              case REXNT_ITEM_TCB_SIGS:
                   ret_val = (unsigned long)tcb_ptr->sigs;
                   break;

              case REXNT_ITEM_SELF:
                   ret_val = (unsigned long)rex_self();
                   break;

              case REXNT_ITEM_IS_INTERACTIVE:
                   #ifdef FEATURE_REX_INTERACTIVE
#error code not present
                   #else
                   ret_val = 0;
                   #endif
                   break;

              case REXNT_ITEM_TCB_TASKNAME:
                   ret_val = (unsigned long)tcb_ptr->task_name;
                   break;

              default:
                   rex_exit( 1 );
           }
           break;

      case REXNT_CMD_SET:
           switch( item_id )
           {
              case REXNT_ITEM_TICK_ENABLED:
                  #ifdef FEATURE_REX_TICK_CONTROL
#error code not present
                  #else
                  rex_exit( 1 );
                  #endif
                  break;

             case REXNT_ITEM_TCB_SIGS:
                  rex_set_sigs( tcb_ptr, value );
                  break;

             case REXNT_ITEM_TIMER_EXT:
                  #ifdef FEATURE_REX_EXTENDED_TIMER
#error code not present
                  #else
                  rex_exit( 1 );
                  #endif
                  break;

             case REXNT_ITEM_TCB_EXT:
                  #ifdef FEATURE_REX_EXTENDED_CONTEXT
#error code not present
                  #else
                  rex_exit( 1 );
                  #endif
                  break;

              default:
                  rex_exit( 1 );
           }
           break;

      case REXNT_CMD_SET_BEST_TASK_FROM_ALL:
           rex_set_best_task_from_all();
           break;

      case REXNT_CMD_CLR_BLOCK_COND:
           #ifdef FEATURE_REX_SYS_SIG
#error code not present
           #else
           rex_exit( 1 );
           #endif
           break;

      case REXNT_CMD_DEC:
           if( item_id == REXNT_ITEM_TCB_NUM_APCS )
           {
              #ifdef FEATURE_REX_APC
              tcb_ptr->num_apcs--;
              #else
              rex_exit( 1 );
              #endif
           }
           else
           {
              rex_exit( 1 );
           }
           break;

      case REXNT_CMD_INC:
           switch( item_id )
           {
              #ifdef FEATURE_REX_APC
              case REXNT_ITEM_TCB_NUM_APCS:
                   tcb_ptr->num_apcs++;
                   break;
              #endif

              case REXNT_ITEM_TCB_SLICES:
                   tcb_ptr->slices++;
                   break;

              #ifdef FEATURE_REX_PROFILE
              case REXNT_ITEM_TOTAL_TIME_SAMPLES:
                   rex_total_time_samples++;
                   break;

              case REXNT_ITEM_TCB_TIME_SAMPLES:
                   tcb_ptr->time_samples++;
                   break;
              #endif

              default:
                 rex_exit( 1 );
           }
           break;

      default:
           rex_exit( 1 );

   } /* END switch cmd_id */


   return ret_val;
} /* END rex_access */


/* ==========================================================================
FUNCTION REX_ACCESS_INIT
DESCRIPTION
   Initializes the accessor function.
============================================================================= */
void rex_access_init( void )
{
   rexnt_set_rex_accessor( rex_access );
   return;
} /* END rex_access_init */

#endif /* FEATURE_REX_INTERNAL_ACCESS */


/* ==========================================================================
FUNCTION REX_AUTODOG_ENABLE
DESCRIPTION
   Performs initialization needed to set up the auto dog service for a task.
   When auto-dog is on, the task does not need to wake up periodically to
   kick the dog when making a blocking REX call. REX takes care of notifying
   Dog that the task is blocked and that there is no need to monitor the
   task until further notice. When the task is scheduled again, Dog is
   notified that the task is ready to be monitored again.

   Returns the previous state of the autodog_enabled field in the TCB.
============================================================================= */
int rex_autodog_enable(
   int dog_report_val
)
{
   int prev_state;
   REX_INTLOCK();
      prev_state = rex_curr_task->autodog_enabled;
      rex_curr_task->dog_report_val  = dog_report_val;
      rex_curr_task->autodog_enabled = TRUE;
   REX_INTFREE();
   return prev_state;
} /* END rex_autodog_enable */


/* ==========================================================================
FUNCTION REX_AUTODOG_DISABLE
DESCRIPTION
   Disables the auto dog service for a task.

   Returns the previous state of the autodog_enabled field in the TCB.
============================================================================= */
int rex_autodog_disable( void )
{
   int prev_state;
   REX_INTLOCK();
      prev_state = rex_curr_task->autodog_enabled;
      rex_curr_task->autodog_enabled = FALSE;
   REX_INTFREE();

   return prev_state;
} /* END rex_autodog_disable */

/*==========================================================================
FUNCTION REX_GET_NUM_TASKS

DESCRIPTION
   Returns the number of currently dispatchable tasks, includes threads if
   FEATURE_REX_THREADS is on and threads are created, includes any task or
   thread regardless of its state (running, blocked, suspended, etc.)
============================================================================ */
int rex_get_num_tasks( void )
{
   return rex_num_tasks;
} /* END rex_get_num_tasks */

/*==========================================================================
FUNCTION REX_SLEEP

DESCRIPTION
  Blocks the current thread for a specified time. The thread becomes
  schedulable after the specified time has elapsed.  This is equivalent
  to creating a REX timer and calling rex_timed_wait on the timer with
  only the associated timer signal being waited upon
============================================================================ */
void rex_sleep(
   unsigned long ms
)
{
   boolean        sleeping = TRUE;
   boolean        sig_set  = FALSE;
   unsigned long  tmo      = ms;
   rex_timer_type sleep_timer;

   rex_def_timer(
      &sleep_timer,
      rex_self(),
      REX_SLEEP_SIG
   );

   while( sleeping )
   {
      (void) rex_timed_wait( REX_SLEEP_SIG, &sleep_timer, tmo );
      REX_INTLOCK();
         if( (tmo = rex_get_timer( &sleep_timer )) == 0 )
         {
            sleeping = FALSE;
         }
         else
         {
            sig_set = TRUE;
         }
      REX_INTFREE();
   }
   if( sig_set )
   {
      (void) rex_set_sigs( rex_self(), REX_SLEEP_SIG );
   }
   return;
} /* END rex_sleep */

#ifdef FEATURE_REX_CREATE_TASK
/*===========================================================================
FUNCTION  REX_CREATE_TASK
DESCRIPTION
   Similar to rex_def_task. REX allocates space for the tcb and
   the stack. Returns a handle to the thread or NULL on error.  Creates
   an independently dispatchable context.
===========================================================================*/
rex_tcb_type*
   /* TCB of created task, NULL otherwise
   */
rex_create_task(
   rex_stack_size_type  stack_size,
      /* Requested stack size
      */
   rex_priority_type    priority,
      /* Requested priority
      */
   rex_task_func_type   thread_ptr,
      /* Thread function
      */
   unsigned long        param,
      /* Parameter to pass to thread function when it is executed
      */
   char                 *thread_name,
      /* ASCII string name for the thread, can be NULL
      */
   boolean              suspended
      /* Indicated whether or not the thread should be started
      ** in the suspended state.  When TRUE, the thread will be
      ** created but only scheduable after a call to zrex_resume_thread,
      ** FALSE indicates the thread is immediately scheduable after
      ** creation
      */
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes.  The adjustment of the stack pointer
   ** is to assure alignment.
   */
   void          *adj_stack_ptr = NULL;
   rex_tcb_type  *tcb_ptr       = NULL;
   unsigned char *stack_ptr     = NULL;

   tcb_ptr = (rex_tcb_type*)rex_malloc( sizeof( rex_tcb_type ) );

   if( tcb_ptr )
   {
      stack_ptr = (unsigned char*)rex_malloc( stack_size );
      if( stack_ptr )
      {
         ASSERT( !rex_is_in_irq_mode( ) );

         adj_stack_ptr = ((unsigned long)stack_ptr%4)?
            (void*)(((unsigned long)stack_ptr+(sizeof(long)) - (unsigned long)stack_ptr%4)):stack_ptr;

         rex_def_task_internal(
            tcb_ptr,
            (unsigned char*)adj_stack_ptr,
            stack_size - ((unsigned long)adj_stack_ptr - (unsigned long)stack_ptr),
            priority,
            thread_ptr,
            param,
            thread_name,
            suspended,
            #ifdef FEATURE_ZREX
#error code not present
            #else
            NULL,
            #endif
            TRUE,
            -1
         );
      }
      else
      {
         rex_free( tcb_ptr );
         tcb_ptr = NULL;
      }
   }
   return tcb_ptr;
} /* END rex_create_task */
#endif /* FEATURE_CREATE_TASK */

/*===========================================================================
FUNCTION rex_task_lock

DESCRIPTION
  rex_task_lock prevents context switching from the point at which it is
  invoked until a matching rex_task_free is invoked.  While tasks are locked,
  the current task is guaranteed to not be replaced by a different task, even
  if the other task has a higher priority.  However, rex_task_lock does not
  block interrupts from occurring.  An ISR will pre-empt the task, but even
  if the ISR causes a higher priority task to become ready, control will
  still resume at the interrupted task.

  Note that a rex_wait() while a rex_task_lock is in effect will not work as
  normal, as the calling task will not be suspended.

  Also, rex_task_lock/rex_task_free have no effect if called from within an
  ISR.
===========================================================================*/
void rex_task_lock( void )
{
   if ( !rex_is_in_irq_mode( ) )
   {
      REX_INTLOCK();
      rex_sched_allow = FALSE;
      rex_nest_depth++;
      REX_INTFREE();
   }
} /* END rex_task_lock */

/*===========================================================================
FUNCTION rex_task_free

DESCRIPTION
  rex_task_free re-enables context switching after a rex_task_lock. Context
  switching is re-enabled if rex_task_free has been called as many times as
  rex_task_lock.

  rex_task_free has no effect if called from within an ISR.
===========================================================================*/
void rex_task_free( void )
{
   if ( !rex_is_in_irq_mode( ) )
   {
      REX_INTLOCK();
      if (rex_nest_depth > 0)
         rex_nest_depth--;
      if (rex_nest_depth == 0)
      {
         rex_sched_allow = TRUE;
         rex_sched();
      }
      REX_INTFREE();
   }
} /* END rex_task_free */

/*===========================================================================
FUNCTION rex_tasks_are_locked

DESCRIPTION
  Returns 1 if tasks are currently locked and 0 otherwise.
===========================================================================*/
int rex_tasks_are_locked( void )
{
  return !rex_sched_allow;
} /* END rex_tasks_are_locked */

#ifdef FEATURE_PROGRESSIVE_BOOT

/*===========================================================================
FUNCTION REX_RESUME_TASK_FROM_ABORT

DESCRIPTION
  resume a task  after loading a page when page faulted.

  Note: Should only be called from abort mode.

===========================================================================*/
void rex_resume_task_from_abort(
   rex_tcb_type *p_tcb
)
{
   /* Tell Dog to resume monitoring the found task.
   */
   REX_RESUME_DOG_MONITOR( p_tcb );
   REX_INTLOCK();
   if ( rex_tasks_are_locked( ) )
   {
      p_tcb->suspended = FALSE;
      rex_sched_force( p_tcb );
   }
   else
   {
      rex_resume_task( p_tcb );
   }
   REX_INTFREE();

} /* END rex_resume_task_from_abort */


/*==========================================================================
FUNCTION REX_GET_BEST_TASK

DESCRIPTION
   Returns the pointer to the best task in rex
============================================================================ */
rex_tcb_type* rex_get_best_task( void )
{
   return rex_best_task;
} /* END rex_get_best_task */


#endif /*FEATURE_PROGRESSIVE_BOOT */

/*===========================================================================
FUNCTION rex_is_initialized

DESCRIPTION
  Returns 1 if Rex has been initialized and 0 if not.
===========================================================================*/
int rex_is_initialized( void )
{
  return rex_init_complete;
} /* END rex_is_initialized */

/*===========================================================================
FUNCTION rex_set_dac

DESCRIPTION
  Sets the DAC variable in the rex TCB and asserts MMU DAC.

DEPENDENCIES
  Should be called after rex_init().
===========================================================================*/
#ifdef FEATURE_DAC_MEMORY_PROTECTION
void rex_set_dac( rex_tcb_type* tcb, unsigned int dac )
{
  rex_enter_crit_sect( &rex_dac_crit_sect );
  tcb->dac = dac;
  mmu_set_dac ( tcb->dac );
  rex_leave_crit_sect( &rex_dac_crit_sect );
} /* END rex_set_dac */
#endif /* FEATURE_DAC_MEMORY_PROTECTION */ 


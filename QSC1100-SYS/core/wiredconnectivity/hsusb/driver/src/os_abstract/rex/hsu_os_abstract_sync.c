/*==============================================================================

		           H S U _ O S _ A B S T R A C T _ S Y N C . C

  
GENERAL DESCRIPTION
  High-Speed USB Implementation of the REX OS abstraction layer 
  synchronization API for the REX OS.
	
EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    os_delay_us
    os_delay_ms
    os_get_time
    os_get_hz
    os_spinlock_init
    os_spinlock_uninit
    os_spinlock_lock
    os_spinlock_unlock
    os_thread_create
    os_msleep
    os_wakeup
    os_safe_enter
    os_safe_leave
    os_get_context

  Externalized internally in the OS abstraction layer:
    hsu_os_sync_init
    hsu_os_sync_uninit
    hsu_os_safe_enter_d
    hsu_os_safe_leave_d

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_os_init should be called before using the API defined in this file.
		
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/06   ds      Created.

===========================================================================*/


/*==============================================================================

LOCAL LINT FLAGS

==============================================================================*/

/*lint +libh(port_sync.h)             Ignore lint errors in port_sync.h.  */
/*lint -emacro(717, ASSERT_CONTEXT)   This macro is defined by 3rd party. */


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Rex include files */
#include <rex.h>
#include <task.h>

#ifndef T_REXNT
#include <clk.h>
#include <time_svc.h>
#endif

/* High Speed USB subsystem include files. */
#include <hsu_common.h>

/* USB stack include files */
#include <jerrno.h>

/* Global OS abstraction layer include files */
#include <port_generic.h>
#include <port_sync.h>

/* For TIMETEST profiling support */
#ifdef TIMETEST
#include "timetest.h"
#endif /* TIMETEST */

/* Local include files. */
#include "hsu_os_abstract_sync.h"

#ifdef T_REXNT
#error code not present
#endif

/* C-Runtime library include files */
#include <memory.h>
#include <stdio.h>

#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */

#ifdef FEATURE_HS_USB_BASIC
/* Empty implementations, needed for DLOAD/ARMPRG, where
** there is no multi-tasking. 
*/
jresult_t os_thread_create
(
 jthread_func func,
 void *arg,
 os_thread_priority priority,
 void **handle
)
{
  return 0;
} /* os_thread_create */

jresult_t os_spinlock_init(os_spinlock_h* lock){return 0;}
void os_spinlock_uninit(os_spinlock_h lock){}
void os_spinlock_lock(os_spinlock_h lock){}
void os_spinlock_unlock(os_spinlock_h lock){}

void os_delay_us(juint32_t usec){}
void os_delay_ms(juint32_t msec){}

jresult_t os_msleep_init(os_msleep_h* msleep_h){return 0;}
jresult_t os_msleep(os_msleep_h handle, juint32_t msec){return 0;}
void os_msleep_uninit(os_msleep_h msleep_h){}
jresult_t os_wakeup(os_msleep_h handle){return 0;}

void os_get_time(os_time_t* time){}
juint32_t os_get_hz(void){return 0;}

jresult_t hsu_os_sync_init(void){return 0;}
void hsu_os_sync_uninit(void){}

void os_mutex_lock(os_mutex_h mtx){}
void os_mutex_unlock(os_mutex_h mtx){}

#else /* FEATURE_HS_USB_BASIC */
/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
static void os_thread_wrapper_function( dword pool_index );

/*==============================================================================
  Constants and Macros
==============================================================================*/
#ifndef CONFIG_TASK_SINGLE
#define OS_THREAD_STACK_SIZE_IN_BYTES   (1024 * STACK_SIZ_FACTOR)
#else
#define OS_THREAD_STACK_SIZE_IN_BYTES   (1024 * STACK_SIZ_FACTOR * 2)
#endif

enum
{
  /* Thread pool size. */
  #ifdef CONFIG_TASK_SINGLE
  OS_MAX_THREADS          =  1,  
  #elif !defined(FEATURE_HSU_TEST)
  OS_MAX_THREADS          =  6, /* 4 + JMS_ASYNC_THREADS */
  #else
#error code not present
  #endif /* FEATURE_HSU_TEST */

  /* msleep handle pool size. */
  #ifdef CONFIG_TASK_SINGLE
  OS_MAX_MSLEEP_HANDLES   = 1,
  #else
  #ifdef FEATURE_HS_USB_OTG
#error code not present
  #else
  OS_MAX_MSLEEP_HANDLES   = 60,
  #endif
  #endif /* CONFIG_TASK_SINGLE */

  OS_MAX_SPINLOCK_HANDLES = 10,

  OS_MAX_MUTEXES          = 10
};

enum
{
  /* This value must be a power of 2, since the 
  ** implementation assumes it.
  */
  OS_MAX_THREADS_PER_MSLEEP_HANDLE = 4
};

enum
{
  OS_MUTEX_SLEEP_TIME_MS = 3
};

#define HSU_OS_ABSTRACT_SYNC_TIME_WRAPAROUND  0x100000000ll

/*==============================================================================
  Typedefs
==============================================================================*/

#ifndef T_REXNT
typedef uint64 os_sync_msecs_time_type;
#else
#error code not present
#endif

/* Execution information of an OS abstraction layer thread. */
typedef struct
{
  rex_tcb_type         os_thread_tcb;

  unsigned char        os_thread_stack[OS_THREAD_STACK_SIZE_IN_BYTES];

  /* The function which is currently executed by the thread.
  ** NULL means that the thread is currently idle.
  */
  jthread_func         thread_function;

  /* The argument passed to the thread function. */
  void*                thread_function_argument;

  /* The thread name */
  char os_thread_name[REX_TASK_NAME_LEN + 1];

} os_thread_pool_entry_type;


/* task msleep status */
typedef enum {
  HSU_OS_SYNC_MLSEEP_INIT_VAL,
  /*status has this value at all times except from the time it wakes up,
    until it's handled in os_msleep */
  HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED, 
  HSU_OS_SYNC_MSLEEP_WAKEN_UP 
} os_msleep_status_enum; 


/* Information of a thread sleeping on an msleep handle. */
typedef struct 
{
  /* The TCB of the sleeping thread. */
  rex_tcb_type*   thread_tcb_ptr;

  /* The thread's msleep timeout timer. */
  rex_timer_type  thread_sleep_timer;

  /* task msleep status */
  os_msleep_status_enum status; 

} os_sleeping_thread_info;


/* msleep handle pool entry. */
typedef struct 
{
  /* Marks whether the pool entry is available or 
  ** occupied.
  */
  boolean  is_available;

  /* Positive value - The number of pending wake-ups.
  ** Negative value - The number of waiting threads.
  */
  int32    count;

  /* The index of the next thread that will be waked up
  ** by an os_wakeup call. Each os_wakeup call advances this value,
  ** and thus starvation of the waiting threads is prevented.
  */
  uint32   next_waked_up;

  /* A list of threads sleeping on this handle */
  os_sleeping_thread_info  sleep_list[OS_MAX_THREADS_PER_MSLEEP_HANDLE];

} os_msleep_handle_pool_entry_type;

/* mutex pool entry. */
typedef struct 
{
  /* Marks whether the pool entry is available or 
  ** occupied.
  */
  boolean  is_available;
  boolean  is_locked;

} os_mutex_pool_entry_type;

typedef struct 
{
  dword   intlock_state;
  uint32  lock_nesting;
} os_spinlock_state_type;

typedef struct
{
  char        *locker_file;
  uint32       locker_line;
} giant_mutex_debug_type;

/* Files global variables. */
typedef struct
{
  boolean                    os_thread_pool_is_initialized;

  os_thread_pool_entry_type  os_thread_pool[OS_MAX_THREADS];

  os_mutex_pool_entry_type   os_mutex_pool[OS_MAX_MUTEXES];

  os_msleep_handle_pool_entry_type 
    os_msleep_handle_pool[OS_MAX_MSLEEP_HANDLES];

  os_spinlock_state_type  spinlock_state;

  /* The 'Giant Mutex'. used by the High-Speed USB stack core's
  ** code for its synchronization.
  */
  rex_crit_sect_type os_giant_mutex;

  giant_mutex_debug_type giant_mutex_debug_info;

} os_sync_file_static_vars_type;

/*==============================================================================
  Externalized Variables
==============================================================================*/


/*==============================================================================
  File Static Variables
==============================================================================*/

static os_sync_file_static_vars_type hsu_os_abstract_sync;

static os_sync_file_static_vars_type* const
  file_static_vars_ptr = &hsu_os_abstract_sync;

/*==============================================================================

                    FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION OS_SAFE_ENTER

DESCRIPTION
  Acquires the High-Speed USB stack Giant mutex.
  This function may not be invoked from an interrupt context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Prevents execution of other High-Speed USB tasks until the mutex is 
  released.

===========================================================================*/
void os_safe_enter_impl(void)
{
  ASSERT_CONTEXT(NORMAL_CONTEXT | DSR_CONTEXT);

  rex_enter_crit_sect(&(file_static_vars_ptr->os_giant_mutex));

} /* os_safe_enter */


/*===========================================================================
FUNCTION OS_SAFE_LEAVE

DESCRIPTION
  Releases the High-Speed USB stack Giant mutex.
  This function may not be invoked from an interrupt context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Enables execution of other High-Speed USB tasks.

===========================================================================*/
void os_safe_leave_impl(void)
{
  ASSERT_CONTEXT(NORMAL_CONTEXT | DSR_CONTEXT);
  /* We want to alert if the mutex isn't locked */
  HSU_ASSERT(file_static_vars_ptr->os_giant_mutex.lock_count > 0);
  rex_leave_crit_sect(&(file_static_vars_ptr->os_giant_mutex));

} /* os_safe_leave */

/*===========================================================================
FUNCTION OS_SAFE_ENTER_D

DESCRIPTION
Wrapper function to acquire the High-Speed USB stack Giant mutex, and save
the locker details.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Prevents execution of other High-Speed USB tasks until the mutex is 
released.

===========================================================================*/
void os_safe_enter_d(char *file, uint32 line)
{
  os_safe_enter_impl();
  hsu_os_abstract_sync.giant_mutex_debug_info.locker_file = file;
  hsu_os_abstract_sync.giant_mutex_debug_info.locker_line = line;
} /* os_safe_enter_d */

/*===========================================================================
FUNCTION OS_SAFE_LEAVE_D

DESCRIPTION
Wrapper function to release the High-Speed USB stack Giant mutex, and reset
locker details.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Enables execution of other High-Speed USB tasks.

===========================================================================*/
void os_safe_leave_d(void)
{
  if (hsu_os_abstract_sync.os_giant_mutex.lock_count == 1)
  {
    hsu_os_abstract_sync.giant_mutex_debug_info.locker_file = NULL;
    hsu_os_abstract_sync.giant_mutex_debug_info.locker_line = 0;
  }
  os_safe_leave_impl();
} /* os_safe_leave_d */


/*===========================================================================
FUNCTION OS_GET_CONTEXT

DESCRIPTION
  Return a unique identifier of the current running thread (from the local OS 
  perspective).

DEPENDENCIES
  None.

RETURN VALUE
  Return a unique identifier of the current running thread (from the local OS 
  perspective).  

SIDE EFFECTS
  None.

===========================================================================*/
juint32_t os_get_context(void)
{
#ifdef FEATURE_L4
#error code not present
#else /* FEATURE_L4 */
  /* non-L4 target returns rex_curr_task in interrupt context. It caused an issue in
  * jos_sync.c  jungo drop, Change 747926. jtask_schedule missed task_signal, when
  * it is called in interrupt context and interrupted task was the same task as 
  * task_thread_arr[task->type].context. 
  */
  return (juint32_t)(rex_is_in_irq_mode() ? NULL : rex_self());
#endif /* !FEATURE_L4 */
} /* os_get_context */


/*===========================================================================
FUNCTION OS_THREAD_CREATE

DESCRIPTION
  Creates a new USB stack thread which will execute the user's function.
  This function seeks for an available entry in the OS abstraction layer 
  thread pool. When such an entry is found, the user's function address 
  and the argument are stored in this entry. Then, a signal is sent to
  the pool's REX task which corresponds to this pool entry. When the 
  REX task gets the signal, it wakes up and executes the user's function.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  0 on success. Otherwise, returns one of error codes defined in jtypes.h.

SIDE EFFECTS
  Make a REX task runnable.

===========================================================================*/
jresult_t os_thread_create
(
  /* (IN) Thread's main function.          */
  jthread_func func,

  /* (IN) Thread's main function argument. */
  void *arg,

  /* (IN) Thread's priority */
  os_thread_priority priority,

  /* (OUT) Returned thread handle.         */
  void **handle
)
{
  uint32     i;
  jresult_t  return_value;
  boolean    found_avail_pool_entry;

  rex_priority_type rex_priority;

  os_thread_pool_entry_type* const 
    threadl_ptr = file_static_vars_ptr->os_thread_pool;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  ASSERT_CONTEXT(NORMAL_CONTEXT);

  HSU_MSG_HIGH("os_thread_create: Creating a new HS-USB stack thread with priority %d.",
                priority, 0, 0);

  switch(priority)
  {
  case THREAD_PRIORITY_CONTROLLER:
    rex_priority = TASK_PRIORITY(HSU_CONTROLLER_PRI_ORDER);
    break;

  case THREAD_PRIORITY_CORE:
    rex_priority = TASK_PRIORITY(HSU_CORE_PRI_ORDER);
    break;

  case THREAD_PRIORITY_DRIVER:
    rex_priority = TASK_PRIORITY(HSU_DRIVER_PRI_ORDER);
    break;

  case THREAD_PRIORITY_OTHER:
    rex_priority = TASK_PRIORITY(HSU_OTHER_PRI_ORDER);
    break;

  default:
    HSU_MSG_ERROR("os_thread_create: Error - Illegal thread priority value: %d",
                  priority, 0, 0);

    HSU_ASSERT((char*)"Illegal thread priority value." == NULL);
    return JEINVAL;
  }

  for (i = 0, found_avail_pool_entry = FALSE;
      i < OS_MAX_THREADS;
      i++)
  {
    if (threadl_ptr[i].thread_function == NULL)
    {
      threadl_ptr[i].thread_function          = func;
      threadl_ptr[i].thread_function_argument = arg;

      found_avail_pool_entry = TRUE;
      break;
    }
  }

  if (!found_avail_pool_entry)
  {
    HSU_MSG_ERROR("os_thread_create: Error - Ran out of available threads",
                  0, 0, 0);

    os_panic("Could not create a thread - Ran out of available threads.");
    return_value = JENOMEM;
  }
  else
  {
    if (handle != NULL)
    {
      /* Double casting is done in order to eliminate 
      ** Lint message #826.
      */
      *handle = (void*)((int32)(&(threadl_ptr[i])));
    }

    /* Create a thread on demand */
    /*lint -e{662} 
    ** Suppress Lint error 662: Possible creation of out-of-bounds pointer,
    ** Since no out of bounds access is possible due to the check in line 475.
    */
	  rex_def_task_ext(&(threadl_ptr[i].os_thread_tcb),
		  threadl_ptr[i].os_thread_stack,
		  OS_THREAD_STACK_SIZE_IN_BYTES,
		  rex_priority,
		  os_thread_wrapper_function,
		  (dword)i,
		  threadl_ptr[i].os_thread_name,
		  FALSE);

    HSU_MSG_HIGH("os_thread_create: Created a new HS-USB thread. Pool entry %d. Priority: %d.",
                 i, priority, 0);

    return_value = 0;
  }

  return return_value;

} /* os_thread_create */


/*===========================================================================
FUNCTION OS_MUTEX_INIT

DESCRIPTION
  Creates a new mutex object and returns a handle to this mutex.
  Seeks for an available mutex in the OS abstraction layer mutex pool. If
  such a mutex is found, it is marked in the pool as allocated, and the 
  mutex index in the pool is returned as a handle to the user.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_mutex_init(os_mutex_h *mtx)
{
  uint32 i;

  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;


  HSU_ASSERT(mtx != NULL);

  for (i = 0; i < OS_MAX_MUTEXES; i++)
  {
    if (os_mutex_pool_ptr[i].is_available)
    {
      os_mutex_pool_ptr[i].is_available = FALSE;
      break;
    }
  }

  HSU_ASSERT(i < OS_MAX_MUTEXES);

  if (i < OS_MAX_MUTEXES)
  {
    *mtx = (os_mutex_h)i;
    return HSU_JSUCCESS;
  }
  else
  {
    *mtx = NULL;
    return JENOMEM;
  }

} /* os_mutex_init */


/*===========================================================================
FUNCTION OS_MUTEX_UNINIT

DESCRIPTION
  Frees a mutex object.
  Marks the mutex as available in the OS abstraction layer mutex pool.
  Consequently, the mutex may be used by other threads.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_mutex_uninit(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;


  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_locked == FALSE);

  os_mutex_pool_ptr[mutex_index_in_pool].is_available = TRUE;

} /* os_mutex_uninit */


/*===========================================================================
FUNCTION OS_MUTEX_LOCK

DESCRIPTION
  Locks a mutex object.

  If the mutex non-initialized or was un-initiaized prior to this call,
  this function generates an assertion.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  The mutex needs to be initialized (using the os_mutex_init function)
  before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  Other threads which try to lock the same mutex will be blocked until 
  the mutex is unlocked (using the os_mutex_unlock function).
===========================================================================*/
void os_mutex_lock(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;
  
#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 


  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);

  while(os_mutex_pool_ptr[mutex_index_in_pool].is_locked)
  {
    os_delay_ms(OS_MUTEX_SLEEP_TIME_MS);
  }

  os_mutex_pool_ptr[mutex_index_in_pool].is_locked = TRUE;

}  /* os_mutex_lock */


/*===========================================================================
FUNCTION OS_MUTEX_UNLOCK

DESCRIPTION
  Unlocks a mutex object.

DEPENDENCIES
  The mutex needs to be initialized (using the os_mutex_init function)
  prior to calling this function.

  This function may not be invoked from an interrupt context 
  or a DSR context.

RETURN VALUE
  None.

SIDE EFFECTS
  If there are other threads waiting to lock the mutex, one of them
  will lock the mutex and become runnable.

  If the mutex was not initialized prior to calling this function, 
  an assertion is generated.

===========================================================================*/
void os_mutex_unlock(os_mutex_h mtx)
{
  os_mutex_pool_entry_type* const
    os_mutex_pool_ptr = file_static_vars_ptr->os_mutex_pool;

  const uint32 mutex_index_in_pool = (uint32)mtx;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_ASSERT(mutex_index_in_pool < OS_MAX_MUTEXES);
  HSU_ASSERT(os_mutex_pool_ptr[mutex_index_in_pool].is_available == FALSE);

  os_mutex_pool_ptr[mutex_index_in_pool].is_locked = FALSE;

}  /* os_mutex_unlock */


/*===========================================================================
FUNCTION OS_SPINLOCK_INIT

DESCRIPTION
  Initialize a spinlock object, and returns a handle to a spinlock object.

  This function may not be invoked from an interrupt context 
  or from a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success. Otherwise returns an error code.

SIDE EFFECTS
  None.

===========================================================================*/
jresult_t os_spinlock_init(os_spinlock_h* lock)
{
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  HSU_ASSERT(lock != NULL);
  ASSERT_CONTEXT(NORMAL_CONTEXT);

  *lock = &(file_static_vars_ptr->spinlock_state);
  return HSU_JSUCCESS;

} /* os_spinlock_init */


/*===========================================================================
FUNCTION OS_SPINLOCK_UNINIT

DESCRIPTION
  Un-Initialize a spinlock object.

  This function may not be invoked from an interrupt context 
  or from a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void os_spinlock_uninit(os_spinlock_h lock)
{
  HSU_USE_PARAM(lock);
  ASSERT_CONTEXT(NORMAL_CONTEXT);

} /*  os_spinlock_uninit*/


/*===========================================================================
FUNCTION OS_SPINLOCK_LOCK

DESCRIPTION
  Locks a spinlock object.

  Spinlock nesting is supported. That means that if a thread calls
  os_spinlock_lock n times consecutively, then it needs to call 
  os_spinlock_unlock n times in order to release the lock.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While a spinlock is locked, all the interrupts in the system are blocked,
  and preemptions are disabled. Therefore, extra care should be taken
  while using this API. A spinlock object must be locked only for very 
  short periods.

  NOTE - A context switch still can occur if a Rex blocking call is called.

===========================================================================*/
void os_spinlock_lock(os_spinlock_h lock)
{
  os_spinlock_state_type* spinlock_state_ptr;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */  

  spinlock_state_ptr = (os_spinlock_state_type*)lock;

  HSU_ASSERT(spinlock_state_ptr == 
             &(file_static_vars_ptr->spinlock_state));

  /* If the spinlock is currently unlocked, lock it.
  ** Otherwise, only increment the nesting level.
  */
  if (spinlock_state_ptr->lock_nesting == 0)
  {
    INTLOCK_SAV(spinlock_state_ptr->intlock_state);

    #ifndef T_REXNT
    TASKLOCK_SAV();
    #endif  /* !T_REXNT */
  }

  spinlock_state_ptr->lock_nesting++;

} /* os_spinlock_lock */


/*===========================================================================
FUNCTION OS_SPINLOCK_UNLOCK

DESCRIPTION
  Unlock a spinlock object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the spinlock becomes unlocked (i.e., no nesting is left), interrupts and
  context switches become enabled again.

===========================================================================*/
void os_spinlock_unlock(os_spinlock_h lock)
{
  os_spinlock_state_type* spinlock_state_ptr;
  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  spinlock_state_ptr = (os_spinlock_state_type*)lock;

  HSU_ASSERT(spinlock_state_ptr ==
             &(file_static_vars_ptr->spinlock_state));

  spinlock_state_ptr->lock_nesting--;

  if (spinlock_state_ptr->lock_nesting == 0)
  {
    #ifndef T_REXNT
    TASKFREE_SAV();
    #endif  /* !T_REXNT */

    INTFREE_SAV(spinlock_state_ptr->intlock_state);
  }

} /* os_spinlock_unlock */


/*===========================================================================
FUNCTION OS_DELAY_US

DESCRIPTION
  Delays the execution of the calling thread for the specified 
  amount of microseconds.

  The function performs a busy sleep (does not yield the context).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_delay_us
(
  /* (IN) Busy-wait time in microseconds. */
  juint32_t usec
)
{
  
#ifdef FEATURE_HS_USB_POLLING
      if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  #ifndef T_REXNT
  /*lint -e{834, 713} */
  clk_busy_wait(usec);
  #else
#error code not present
  #endif

} /* os_delay_us */


/*===========================================================================
FUNCTION OS_DELAY_MS

DESCRIPTION
  Delays the calling thread's execution for the specified 
  amount of milliseconds.

  The function performs a non-busy sleep (may lose the context).

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_delay_ms
(
  /* (IN) Sleep time in milliseconds. */
  juint32_t msec
)
{
  int lock_count;
  int i;

#ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 
 

  ASSERT_CONTEXT(NORMAL_CONTEXT);
  
  /* Enter the critical section to access the global lock count */
  jsafe_enter(); 
  lock_count = hsu_os_abstract_sync.os_giant_mutex.lock_count;
  for(i = 0; i < lock_count; i++)
  {
    /* Release the Giant mutex. */
    jsafe_leave();
  }

  rex_sleep((unsigned long)msec);

  /* Re-acquire the Giant mutex.
     It should have been locked "lock_count" times,
     and then released once - because of the additional lock in this function
     (the one for accessing the global lock count).
     So in order to save two accesses to the Giant mutex,
     that actually cancel one another,
     we only lock it "lock_count-1" times
  */
  for(i = 0; i < lock_count-1; i++)
  {
    /* Re-aquire the Giant mutex. */
    jsafe_enter();
  }


} /* os_delay_ms */


/* ==========================================================================
FUNCTION OS_MSLEEP_TIMER_CB

DESCRIPTION
This callback is called when the sleep timer expires.
It signals the sleeping task (the one that called the os_msleep)

RETURN VALUE
None.

SIDE EFFECTS
Resume the waiting task
============================================================================= */
void os_msleep_timer_cb(unsigned long task_info_ptr)
{ 
  os_sleeping_thread_info* sleep_thread = (os_sleeping_thread_info*)task_info_ptr; 
  /* lock this func in case the timer will expire simultaneously with an os_wakeup call*/
  INTLOCK(); 
  {

#ifndef T_REXNT 
    TASKLOCK(); 
#endif  /* !T_REXNT */ 

    /* Make sure that the sleeping thread's entry wasn't 
    ** removed by an 'os_wakeup' call just now. */
    if (sleep_thread->thread_tcb_ptr != NULL)
    {
      sleep_thread->status = HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED; 
      rex_set_sigs(sleep_thread->thread_tcb_ptr, HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
    }
#ifndef T_REXNT 
    TASKFREE(); 
#endif  /* !T_REXNT */ 
  }
  INTFREE(); 
} 


/*===========================================================================
FUNCTION OS_MSLEEP_INIT

DESCRIPTION
  Initializes a new msleep handle and returns a pointer to it.
  This handle may then be passed to other msleep/wakeup functions.

  Seeks for an available msleep handle in the OS abstraction layer 
  msleep handles pool. If such a handle is found, it is marked in the pool 
  as allocated, and the handle index in the pool is returned to the user.

  This function may not be invoked from an interrupt context 
  or a DSR context.

DEPENDENCIES
  None.

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_msleep_init
(
  /* (OUT) The new allocated msleep handle. */
  os_msleep_h* msleep_h
)
{
  uint32     i;
  jresult_t  return_value;

  boolean    found_avail_pool_entry;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;
  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  

  HSU_ASSERT(msleep_h != NULL);

  for (i = 0, found_avail_pool_entry = FALSE;
       i < OS_MAX_MSLEEP_HANDLES;
       i++)
  {
    if (os_msleep_handle_pool_ptr[i].is_available == TRUE)
    {
      os_msleep_handle_pool_ptr[i].is_available = FALSE;
      found_avail_pool_entry                    = TRUE;

      break;
    }
  }

  if (!found_avail_pool_entry)
  {
    HSU_MSG_ERROR("Ran out of available msleep handles. (%d)",
                  OS_MAX_MSLEEP_HANDLES, 0, 0);
    return_value = JENOMEM;
  }
  else
  {
    *msleep_h    = (os_msleep_h)i;
    return_value = 0;
  }

  return return_value;

} /* os_msleep_init */


/*===========================================================================
FUNCTION OS_MSLEEP

DESCRIPTION
  Suspends the execution of the calling thread until the given 
  handle is awaken or the specified timeout period (in milliseconds)
  expires.

  The function performs a non-busy sleep.

  If the 'handle' argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

  This function may not be called from an interrupt context or a DSR context.

DEPENDENCIES
  The handle must be previously initialized using os_msleep_init.

RETURN VALUE
  0 on success. Otherwise, returns one of the error codes 
  defined in jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_msleep
(
  /* (IN) The msleep handle to sleep on. */
  os_msleep_h handle,

  /* (IN) Sleep timeout in milliseconds. */
  juint32_t msec
)
{
  const uint32
    msleep_handle_pool_index = (uint32)handle;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
      &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;

  rex_tcb_type* const my_tcb = rex_self();

  rex_sigs_type    sigs;
  dword            intlock_state;
  boolean          wakeups_pending;

  jresult_t        return_value           = EUNKNOWN;
  uint32           sleep_list_index       = 0;
  rex_timer_type*  thread_sleep_timer_ptr = NULL;

  int lock_count;
  int i;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */  


  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  ASSERT_CONTEXT(NORMAL_CONTEXT);

  /* Make sure that the msleep signals do not conflict with any
  ** other signals which were set somewhere outside the HS-USB module.
  */
  sigs   = rex_get_sigs(my_tcb);
  HSU_ASSERT((sigs & (HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL)) == 0);


  INTLOCK_SAV(intlock_state);

  #ifndef T_REXNT
  TASKLOCK_SAV();
  #endif  /* !T_REXNT */

  wakeups_pending = (boolean)(os_msleep_handle_pool_entry_ptr->count > 0);

  /* If there are no pending wake-ups, add the current thread
  ** to the handle's sleeping threads queue and wait for a wakeup signal
  ** or a timeout.
  */
  if (!wakeups_pending)
  {
    /* Verify that there is an available entry in the msleep handle's
    ** waiting threads queue;
    */
    if (-os_msleep_handle_pool_entry_ptr->count == 
        OS_MAX_THREADS_PER_MSLEEP_HANDLE)
    {

      os_panic("msleep handle's waiting threads queue is full!.");

      #ifndef T_REXNT
      TASKFREE_SAV();
      #endif  /* !T_REXNT */

      INTFREE_SAV(intlock_state);

      return JENOMEM;
    }

    /* Find an available slot in the handle's sleep list. */
    for (sleep_list_index = 0; 
         sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE;
         sleep_list_index++)
    {
      if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr == NULL)
      {
        break;
      }
    }

    /* Verify that an available sleep slot was found
    ** (should always happen).
    */
    HSU_ASSERT(sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE);

    /*lint -e{661}
    ** No out of bounds access occurs here, since it is done only 
    ** if an available (in bounds) entry was found 
    ** (otherwise, an assertion occurs).
    */
    sleep_list_ptr[sleep_list_index].thread_tcb_ptr = my_tcb;

    /*lint -e{662}
    ** No out of bounds access occurs here, since it is done only 
    ** if an available (in bounds) entry was found 
    ** (otherwise, an assertion occurs).
    */
    thread_sleep_timer_ptr =
      &(sleep_list_ptr[sleep_list_index].thread_sleep_timer);

    /* If msec > 0, there is an msleep timeout period, and therefore a 
    ** timeout timer is armed. Otherwise, msleep blocks unlimitedly until 
    ** a wakeup signal is received.
    */
    if (msec > 0)
    { 
      rex_def_timer_ex(thread_sleep_timer_ptr,os_msleep_timer_cb,
                        (unsigned long)(&(sleep_list_ptr[sleep_list_index])));
      (void)rex_set_timer(thread_sleep_timer_ptr, msec);
    }

  }  /* if !(wakeups_pending) */

  os_msleep_handle_pool_entry_ptr->count--;

  #ifndef T_REXNT
  TASKFREE_SAV();
  #endif  /* !T_REXNT */

  INTFREE_SAV(intlock_state);

  if (wakeups_pending)
  {
    return_value = HSU_JSUCCESS;
  }
  else
  {
    /* Enter the critical section to access the global lock count */
    jsafe_enter(); 

    lock_count = hsu_os_abstract_sync.os_giant_mutex.lock_count;

    for(i = 0; i < lock_count; i++)
    {
      /* Release the Giant mutex. */
      jsafe_leave();
    }

    sigs = rex_wait(HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);

    /* Re-acquire the Giant mutex.
    It should have been locked "lock_count" times,
    and then released once - because of the additional lock in this function
    (the one for accessing the global lock count).
    So in order to save two accesses to the Giant mutex,
    that actually cancel one another,
    we only lock it "lock_count-1" times
    */
    for(i=0; i < lock_count-1; i++)
    {
      jsafe_enter();
    }

    if (sleep_list_ptr[sleep_list_index].status == HSU_OS_SYNC_MSLEEP_WAKEN_UP) 
    { 
      return_value = HSU_JSUCCESS; 
    }
    else if (sleep_list_ptr[sleep_list_index].status == HSU_OS_SYNC_MLSEEP_TIMER_EXPIRED)
    { 
      /* If msec == 0, no timeout period exists, and thus
      ** receiving a timeout signal is an error.
      */
      HSU_ASSERT(msec > 0); 
      return_value = JEWOULDBLOCK; 
    }
    else 
    { 
      HSU_MSG_ERROR("os_msleep: A wakeup or timeout is expected", 0, 0, 0);
      HSU_ASSERT(FALSE);
    } 
    
    INTLOCK(); 

#ifndef T_REXNT 
    TASKLOCK(); 
#endif  /* !T_REXNT */ 
    /* Make sure that the sleeping thread's entry wasn't 
    ** removed by an 'os_wakeup' call. 
    */ 
    if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr != NULL) 
    { 
      /* Remove the thread from the sleep list. */ 
      rex_clr_timer(thread_sleep_timer_ptr);
      sleep_list_ptr[sleep_list_index].thread_tcb_ptr = NULL; 

      os_msleep_handle_pool_entry_ptr->count++; 
    } 
    sleep_list_ptr[sleep_list_index].status = HSU_OS_SYNC_MLSEEP_INIT_VAL;

#ifndef T_REXNT 
    TASKFREE(); 
#endif  /* !T_REXNT */ 

    INTFREE(); 
  
    (void)rex_clr_sigs(my_tcb,  HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);

  }  /* if (wakeups_pending) */
  return return_value;

} /* os_msleep */


/*===========================================================================
FUNCTION OS_MSLEEP_UNINIT

DESCRIPTION
  Frees a OS abstraction layer msleep handle that was previously initialized
  using the os_msleep_init function.

  Marks the handle as available in the OS abstraction layer msleep
  handles pool. The msleep handle may then be used by other threads.

  If the handle argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

DEPENDENCIES
  The handle must be previously initialized using os_msleep_init.

  All the threads sleeping on this handle must be waked-up before 
  calling this function. Otherwise, an assertion occurs.

RETURN VALUE
  None.

SIDE EFFECTS
  Stops all the timers associated with this handle and removes them 
  from the Rex active timers list .
===========================================================================*/
void os_msleep_uninit
(
  /* (IN) The msleep handle to uninitialize. */
  os_msleep_h msleep_h
)
{
  const uint32
    msleep_handle_pool_index = (uint32)msleep_h;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
      &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;

  uint32 sleep_list_index;

  
#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */  

  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  /* There should be no threads sleeping on this handle. */
  HSU_ASSERT(os_msleep_handle_pool_entry_ptr->count >= 0);

  /* Go over the handle's sleep list and clear it. */
  for (sleep_list_index = 0;
       sleep_list_index < OS_MAX_THREADS_PER_MSLEEP_HANDLE;
       sleep_list_index++)
  {
    if (sleep_list_ptr[sleep_list_index].thread_tcb_ptr != NULL)
    {
      (void)rex_clr_timer(&(sleep_list_ptr[sleep_list_index].thread_sleep_timer));
      sleep_list_ptr[sleep_list_index].thread_tcb_ptr = NULL;
    }
  }

  os_msleep_handle_pool_entry_ptr->next_waked_up = 0;
  os_msleep_handle_pool_entry_ptr->count         = 0;
  os_msleep_handle_pool_entry_ptr->is_available  = TRUE;

} /* os_msleep_uninit */


/*===========================================================================
FUNCTION OS_WAKEUP

DESCRIPTION
  Wakes up a thread waiting on the given handle.

  If the 'handle' argument does not refer to a valid msleep handle pool
  entry, an assertion is generated.

  If the handle is not initialized or was un-initiaized prior to this call,
  an assertion is generated.

DEPENDENCIES
  The handle must be previously initialized using os_msleep_init.

RETURN VALUE
  Always returns 0.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_wakeup
(
  /* The msleep handle to wake-up. */
  os_msleep_h handle
)
{
  const uint32
    msleep_handle_pool_index = (uint32)handle;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_entry_ptr =
    &(file_static_vars_ptr->os_msleep_handle_pool[msleep_handle_pool_index]);

  os_sleeping_thread_info* const
    sleep_list_ptr = os_msleep_handle_pool_entry_ptr->sleep_list;

  int32* const 
    os_msleep_handle_count_ptr = &(os_msleep_handle_pool_entry_ptr->count);

  uint32* const
    next_waked_up_ptr = &(os_msleep_handle_pool_entry_ptr->next_waked_up);

  rex_tcb_type* awaken_thread_tcb_ptr;

  boolean  sleeping_thread_found;
  uint32   i;

  dword    intlock_state;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */ 

  /* Make sure that the handle refers to a valid pool entry */
  HSU_ASSERT(msleep_handle_pool_index < OS_MAX_MSLEEP_HANDLES);

  /* Make sure that the handle is allocated. */
  HSU_ASSERT(!os_msleep_handle_pool_entry_ptr->is_available);

  /* INTLOCK_SAV and TASKLOCK_SAV are used here, since 'os_wakeup' may be 
  ** called from an interrupt context. Therefore, a REX critical section 
  ** can't be used.
  */
  INTLOCK_SAV(intlock_state);

  #ifndef T_REXNT
  TASKLOCK_SAV();
  #endif  /* !T_REXNT */

  HSU_ASSERT(*os_msleep_handle_count_ptr <= 1);

  /* If no threads are currently waiting on this handle,
  ** return immediately. Otherwise, wake-up a waiting thread.
  */
  if (*os_msleep_handle_count_ptr == 1)
  {
    #ifndef T_REXNT
    TASKFREE_SAV();
    #endif  /* !T_REXNT */

    INTFREE_SAV(intlock_state);
    return HSU_JSUCCESS;
  }
  else if (*os_msleep_handle_count_ptr == 0)
  {
    /* Let the next 'os_msleep' call return immediately.
    */
    (*os_msleep_handle_count_ptr) = 1;

    #ifndef T_REXNT
    TASKFREE_SAV();
    #endif  /* !T_REXNT */

    INTFREE_SAV(intlock_state);
    return HSU_JSUCCESS;
  }

  /* *os_msleep_handle_count_ptr < 0 
  ** ==> there are threads sleeping on this handle.
  */

  /* Go over the handle's sleep list from the location 
  ** of the last awaken thread entry and on, and wake-up
  ** the first sleeping thread found.
  */
  sleeping_thread_found = FALSE;

  for (i = 0;
        !sleeping_thread_found && (i < OS_MAX_THREADS_PER_MSLEEP_HANDLE);
        i++)
  {
    if (sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr != NULL)
    {
      awaken_thread_tcb_ptr = 
        sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr;

      sleeping_thread_found = TRUE;

      /* Make sure that sleeping thread's entry wasn't
      ** already removed due to an os_msleep timeout.
      */
      if (sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr != NULL)
      {
        /* Remove the awaken thread from the list. */
        (void)rex_clr_timer(&(sleep_list_ptr[*next_waked_up_ptr].
                              thread_sleep_timer));

        sleep_list_ptr[*next_waked_up_ptr].thread_tcb_ptr = NULL;

        /* Update the msleep handle's sleep count. */
        os_msleep_handle_pool_entry_ptr->count++;
      }
      sleep_list_ptr[*next_waked_up_ptr].status = HSU_OS_SYNC_MSLEEP_WAKEN_UP; 
      rex_set_sigs(awaken_thread_tcb_ptr, HSU_OS_SYNC_MSLEEP_STOP_SLEEP_SIGNAL);
    }

    (*next_waked_up_ptr)++;

    /*  Wrap around */
    if (*next_waked_up_ptr == OS_MAX_THREADS_PER_MSLEEP_HANDLE)
    {
      *next_waked_up_ptr = 0;
    }
  }

  /* There should have been a sleeping thread that
  ** was awaken up. 
  */
  HSU_ASSERT(sleeping_thread_found);

  #ifndef T_REXNT
  TASKFREE_SAV();
  #endif  /* !T_REXNT */

  INTFREE_SAV(intlock_state);

  return HSU_JSUCCESS;

} /* os_wakeup */

/*===========================================================================
FUNCTION OS_GET_TIME

DESCRIPTION
Get the current system time.
Time accuracy is in milliseconds.

This function uses "timetick_get_ms" which returns time in 32b format
(granularity is  30.5us per tick).
It stores the higher part of a 64b variable in "saved_time_high". It is updated every time
the 23b value wraps-around.
(which is expected to happen every 49 days -according to "timetick_get_ms" documentation).


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void os_get_time
(
 /* (OUT) Returned current time. */
 os_time_t* time
 )
{
  os_sync_msecs_time_type  time_in_msecs;
#ifndef T_REXNT
  uint32 short_time_ms;
#endif
  /* holds the higher 32b of the time (in 64b format). updated upon wrap-around*/
  static os_sync_msecs_time_type saved_time_high = 0;
  /* holds the lower 32b of last time taken. updated every call*/
  static uint32 saved_time_low = 0;

#ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  #ifndef T_REXNT

  short_time_ms = timetick_get_ms();
  if (short_time_ms < saved_time_low)
  { /* wrap-around*/
    saved_time_high += HSU_OS_ABSTRACT_SYNC_TIME_WRAPAROUND;
  }

  /* calc returned value */
  time_in_msecs = saved_time_high + short_time_ms;

  /* update stored val every time */
  saved_time_low = short_time_ms;
  
  #else
#error code not present
  #endif

  time->tv_sec  = (jint32_t)(time_in_msecs / 1000);
  time->tv_usec = (jint32_t)((time_in_msecs % 1000) * 1000);

}/* os_get_time */


#if 0
/* This function is not used so far, so it is compiled out.
**
** inside the comment there is an apparently correct implementation, but
** it was not tested, due to its lack of usage. Whenever this API is used,
** It should be verified that this implementation is meets the requirements
** of this API.
*/
/*===========================================================================
FUNCTION OS_GET_HZ

DESCRIPTION
  Get number of clock ticks-in-second.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the number of clock ticks per second.

SIDE EFFECTS
  None.
===========================================================================*/
juint32_t os_get_hz(void)
{
  #ifndef T_REXNT
  return clk_regime_msm_get_clk_freq_khz(CLKRGM_MARM_CLK);
  #else
#error code not present
  #endif
} /* os_get_hz */
#endif  /* 0 */


/* ==========================================================================
FUNCTION OS_THREAD_WRAPPER_FUNCTION

DESCRIPTION
   The entry function of an OS abstraction layer thread.
   This function does the following:
   
     1. Reads the address to the function to execute and its argument 
        (stored by os_thread_create in its entry in the thread pool array).

     2. Invokes the function and passes it the argument.

     3. Clears the function and argument addresses from the thread pool entry,
        making it available again.

DEPENDENCIES
   None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================= */
static void os_thread_wrapper_function
(
  /* The index of this thread in the thread pool. */
  dword pool_index
)
{
  jthread_func my_function;
  void*        my_argument;

  os_thread_pool_entry_type* const
    thread_pool_entry_ptr = &(file_static_vars_ptr->os_thread_pool[pool_index]);


  HSU_ASSERT(pool_index < OS_MAX_THREADS);

  /* Acquire the Giant mutex. */
    jsafe_enter();

    /* Fetch the thread execution information. */
    my_function = thread_pool_entry_ptr->thread_function;
    my_argument = thread_pool_entry_ptr->thread_function_argument;

    HSU_ASSERT(my_function != NULL);

    /* Execute the requested function. */
    my_function(my_argument);

    /* After executing the thread function, clear the thread pool entry data.
    ** This makes the OS thread pool entry available again.
    */
    thread_pool_entry_ptr->thread_function          = NULL;
    thread_pool_entry_ptr->thread_function_argument = NULL;

    /* Release the Giant mutex. */
    jsafe_leave();

}  /* os_thread_wrapper_function */


/* ==========================================================================
FUNCTION HSU_OS_SYNC_INIT

DESCRIPTION
   Initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
   None.

RETURN VALUE
   If the initialization is successful, returns zero. Otherwise, returns 
   a non-zero error code.

SIDE EFFECTS
   Creates new REX threads.
============================================================================= */
jresult_t hsu_os_sync_init(void)
{
  uint32    i, k;
 
  /*lint -esym(550, result)
  ** This symbol is accessed, but for some reason Lint thinks it is not.
  */
  
  jint32_t  result;
  
  os_thread_pool_entry_type* const
    os_thread_ptr = file_static_vars_ptr->os_thread_pool;

  os_msleep_handle_pool_entry_type* const
    os_msleep_handle_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return 0;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_MSG_HIGH("hsu_os_sync_init: Initializing HS-USB OS AL synchronization module.",
               0, 0, 0);
  
  /*-----------------------------*/
  /* Thread pool initialization. */
  /*-----------------------------*/

  if (!file_static_vars_ptr->os_thread_pool_is_initialized)
  {
    /* Clear the thread pool data. */
    (void)memset((void*)(file_static_vars_ptr->os_thread_pool),
                 0,
                 sizeof(file_static_vars_ptr->os_thread_pool));

    for (i = 0; i < OS_MAX_THREADS; i++)
    {
      result = os_snprintf(os_thread_ptr[i].os_thread_name,
                           sizeof(os_thread_ptr[i].os_thread_name),
                           "HS-USB %u",
                           i);

      HSU_ASSERT(result > 0);
#ifdef TIMETEST
      if (i == 0)
      {
        os_thread_ptr[i].os_thread_tcb.leds = TIMETEST_HSU_PERIPHERAL_TASK_ID;
      }
      else
      {   
        os_thread_ptr[i].os_thread_tcb.leds = TIMETEST_HSU_HOST_TASK_ID;
      }
#endif /* TIMETEST */
    }
    file_static_vars_ptr->os_thread_pool_is_initialized = TRUE;
  }

  /*-----------------------------*/
  /* Initialize the Giant mutex. */
  /*-----------------------------*/
  rex_init_crit_sect(&file_static_vars_ptr->os_giant_mutex);
  /* 
   * lock the giant for the rest of the execution. It is released by the calling 
   * function at the config selector's level (outside jstart_stack()). 
   */
  jsafe_enter(); 

  /*------------------------------------*/
  /* msleep handle pool initialization. */
  /*------------------------------------*/
  (void)memset(file_static_vars_ptr->os_msleep_handle_pool,
               0,
               sizeof(file_static_vars_ptr->os_msleep_handle_pool));

  for (i = 0; i < OS_MAX_MSLEEP_HANDLES; i++)
  {
    os_msleep_handle_pool_ptr[i].count         = 0;
    os_msleep_handle_pool_ptr[i].next_waked_up = 0;
    os_msleep_handle_pool_ptr[i].is_available  = TRUE;
    for (k=0; k< OS_MAX_THREADS_PER_MSLEEP_HANDLE; k++)
    {
      (os_msleep_handle_pool_ptr[i].sleep_list[k]).status = HSU_OS_SYNC_MLSEEP_INIT_VAL;
    }
    
  }

  /*------------------------------------*/
  /* Mutex pool initialization.         */
  /*------------------------------------*/
  for (i = 0; i < OS_MAX_MUTEXES; i++)
  {
    file_static_vars_ptr->os_mutex_pool[i].is_available = TRUE;
    file_static_vars_ptr->os_mutex_pool[i].is_locked    = FALSE;
  }

  /*---------------------------*/
  /* Spinlocks initialization. */
  /*---------------------------*/
  file_static_vars_ptr->spinlock_state.lock_nesting = 0;


  HSU_MSG_HIGH("hsu_os_sync_init: HS-USB OS AL synchronization module initialization is completed.",
               0, 0, 0);

  return HSU_JSUCCESS;

} /* hsu_os_sync_init */


/* ==========================================================================
FUNCTION HSU_OS_SYNC_UNINIT

DESCRIPTION
   Un-initializes the REX OS abstraction layer thread synchronization module.

DEPENDENCIES
   The caller must make sure that all the synchronization resources 
   (e.g., threads) were released (using the proper un-init 
   functions) prior to calling this function.

RETURN VALUE
   None.

SIDE EFFECTS
   Rex tasks in the OS abstraction layer 'thread pool' are not killed during
   the OS abstraction layer un-initialization, since the behavior of 
   'rex_kill_task' is unknown.
============================================================================= */
void hsu_os_sync_uninit(void)
{
  uint32 i;

  /*lint -esym(529, os_msleep_pool_ptr)
  ** This symbol is referenced by the HSU_ASSERT macro, but Lint
  ** does not catch it.
  */
  os_msleep_handle_pool_entry_type* const
    os_msleep_pool_ptr = file_static_vars_ptr->os_msleep_handle_pool;

#ifdef FEATURE_HS_USB_POLLING
   if(hsu_polling_is_active()){return;}
#endif /* FEATURE_HS_USB_POLLING */ 

  HSU_MSG_HIGH("hsu_os_sync_uninit: Un-initializing HS-USB OS AL synchronization module.",
               0, 0, 0);

  for (i = 0; i < OS_MAX_MSLEEP_HANDLES; i++)
  {
    HSU_ASSERT(os_msleep_pool_ptr[i].is_available == TRUE);
  }

  HSU_MSG_HIGH("hsu_os_sync_uninit: HS-USB OS AL synch. module un-initialization is completed.",
               0, 0, 0);

} /* hsu_os_sync_uninit */

#endif /* FEATURE_HS_USB_BASIC */

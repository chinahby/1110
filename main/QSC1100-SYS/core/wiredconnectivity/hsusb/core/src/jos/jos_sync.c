/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "jos_internal.h"

/* HSU Addition Begin */
#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */
/* HSU Addition End */

#ifndef SINGLE_THREADED

/* Minimal interval between non DSR (level 0) tasks */
#define TASK_EXEC_INTERVAL 2

/* Task thread context structure */
typedef struct task_thread_s {
    jmsleep_h           sleep_handle;
    struct jtask_s      list;
    void                *handle;
#define TASK_THREAD_SLEEP_HANDLE_INIT   0x01
#define TASK_THREAD_THREAD_INIT         0x02
#define TASK_THREAD_ACTIVE              0x04
#define TASK_THREAD_FINISHED            0x08
#define TASK_THREAD_PAUSED              0x10
    juint8_t             flags;

#ifdef JDEBUG
# define TASK_THREAD_SLEEPING            0x01
# define TASK_THREAD_RUNNING             0x02
    juint8_t             state;
    juint32_t            task_runs;
    jtime_t             run_start;
    jtask_h             curr_task;
#endif
    juint32_t            context;
} *task_thread_h;

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */

/* Array holding all task context information */
static struct task_thread_s    task_thread_arr[TASK_TYPE_COUNT];

/* Lock for syncing task scheduling with ISR */
static jspinlock_h task_lock = NULL;

/* Local function declarations */
static void             task_add(jtask_h task, jtime_t time, juint32_t timeout, 
      jthread_func cb, void *arg);
static void             task_remove(jtask_h task);
static void             task_stop_wait(jtask_h task);
static jresult_t        task_thread_init(jtask_type index);
static void             task_thread_uninit(jtask_type index);
static void             task_signal(jtask_type type, jtime_t *time);

#endif /* HSU addition */

#if defined(JDEBUG) && !defined(CONFIG_TASK_SINGLE)
static void             task_monitor_thread(void *thread_arg);
#endif

/**
 * Function name:  jtask_init
 * Description: Initialize a new task
 * Parameters: 
 *     @task: Pointer to task to initialize
 *     @type: The type of the task
 *
 * Return value: jresult_t
 * Scope: global
 **/
jresult_t jtask_init(jtask_h *task, jtask_type type)
{
    if (!task)
        return JEINVAL;
    
    DBG_V(DJOS_SYNC, ("TASK: %d Initializing task %p\n", type, task));
    
    (*task) = jmalloc(sizeof(struct jtask_s), M_ZERO);
    if (!(*task))
        return JENOMEM;

    DBG_X(DJOS_SYNC, ("TASK: %d Allocated %p for the task\n", type, *task));

    (*task)->type = type;
    return 0;
}

/**
 * Function name:  jtask_uninit
 * Description: Un-Initialize a task
 * Parameters: 
 *     @task: The task to un-initialize
 *
 * Return value: jresult_t
 * Scope: global
 **/
void jtask_uninit(jtask_h task)
{
    DBG_V(DJOS_SYNC, ("TASK: %d Starting task uninit %p\n", 
        task ? task->type : 99, task));
    
    if (!task)
        return;

#ifdef JDEBUG
    if (task->state == JTASK_STOPPING)
        DBG_E(DJOS_SYNC, ("TASK: Request to uninit a task pending a stop\n"));
#endif

    jtask_stop(task);

    jfree(task);
}

/**
 * Function name:  jtask_schedule
 * Description: Schedule a task to run in specified timeout
 * Parameters: 
 *     @task: Task handle created using task_init()
 *     @timeout: Minimum time in milliseconds to wait before executing
 *     @cb: Callback to call when timeout expires
 *     @arg: Argument to pass the callback
 *
 * Return value: jresult_t
 * Scope: global
 **/
#ifndef JDEBUG
jresult_t jtask_schedule(jtask_h task, juint32_t timeout,
    jthread_func cb, void *arg)
#else
jresult_t jtask_schedule_d(jtask_h task, juint32_t timeout,
    jthread_func cb, void *arg, char *file, jint_t line)
#endif
{
    #ifdef FEATURE_HS_USB_BASIC
    /* No multi-tasking, so we call the function immediately,
    ** without enqueueing a task. 
    */
    KASSERT(timeout==0,("jtask_schedule: FEATURE_HS_USB_BASIC is on but timeout is not 0"));
    KASSERT(cb!=NULL,("jtask_schedule: FEATURE_HS_USB_BASIC is on, cb is NULL"));
    cb(arg);
    #else
    jtime_t time;

    /* HSU Addition Begin */
    #ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active())
    {
      cb(arg);
      return 0;
    }
    #endif /* FEATURE_HS_USB_POLLING */
    /* HSU Addition End */
    if (!task)
        return JEINVAL;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_SYNC, ("TASK: %d Scheduling task %p in %d [%p:%p]\n",
        task->type, task, timeout, cb, arg));
#endif /* CONFIG_DATA_PATH_DEBUG */
    jget_time(&time);

    jspinlock_lock(task_lock);
    {
        switch (task->state)
        {
        case JTASK_STOPPED:
            task->state = JTASK_PENDING;
            break;

        case JTASK_PENDING:
            task_remove(task);
            break;

        case JTASK_RUNNING:
            task->state = JTASK_RUN_N_PEND;
            break;

        case JTASK_RUN_N_PEND:
            task_remove(task);
            break;

        case JTASK_STOPPING:
            jspinlock_unlock(task_lock);
            DBG_E(DJOS_SYNC, ("TASK: Trying to schedule a stopping task\n"));
            return JEINVAL;
        }

#ifdef JDEBUG
        task->file = file;
        task->line = line;
#endif

        task_add(task, time, timeout, cb, arg);
    }
    jspinlock_unlock(task_lock);

    /* Update the wakeup time of the relevant task thread - Only if
     * the relevent task is not already running */
    if (jget_context() != task_thread_arr[task->type].context)
        task_signal(task->type, &time);
    #endif /* FEATURE_HS_USB_BASIC */
    return 0;
}

/**
 * Function name:  jtask_pending
 * Description: Check if a task is pending execution
 * Parameters: 
 *     @task: Handle of the task
 *
 * Return value: True (1) if pending execution or False (0) otherwise
 * Scope: global
 **/
jbool_t jtask_pending(jtask_h task)
{
    /* HSU addition (FEATURE_HS_USB_BASIC flag) */
    #ifndef FEATURE_HS_USB_BASIC
    /* HSU Addition Begin */
    #ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()) {return 0;}
    #endif /* FEATURE_HS_USB_POLLING */
    /* HSU Addition End */
    if (!task)
        return 0;

    DBG_V(DJOS_SYNC, ("TASK: %d Check if task %p is pending %p\n",
        task->type, task, task->cb));

    if (task->state == JTASK_PENDING)
        return 1;
    #endif

    return 0;
}

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_remove
 * Description: Remove a task from the task list
 * Parameters: 
 *     @task: Handle of the task
 *
 * Return value: none
 * Scope: local
 **/
static void task_remove(jtask_h task)
{
    jtask_h *ptr;
    
    /* Find the task */
    for (ptr = &task_thread_arr[task->type].list.next;
        *ptr && *ptr != task; 
        ptr = &((*ptr)->next))
        ;

    KASSERT((*ptr), ("TASK: %d Stopping task that is not scheduled ! %p\n",
        task->type, task));

    /* Remove the task from the list */
    *ptr = (*ptr)->next;
    task->cb = NULL;
}
#endif /* HSU addition */

/**
 * Function name:  jtask_stop
 * Description: Stop a scheduled task
 * Parameters: 
 *     @task: Handle of the task
 *
 * Return value: none
 * Scope: global
 **/
void jtask_stop(jtask_h task)
{
	/* HSU addition (FEATURE_HS_USB_BASIC flag) */
    #ifndef FEATURE_HS_USB_BASIC
    jtime_t time;
    /* HSU Addition Begin */
    #ifdef FEATURE_HS_USB_POLLING
    if(hsu_polling_is_active()){return;}
    #endif /* FEATURE_HS_USB_POLLING */
    /* HSU Addition End */
    jget_time(&time);

    KASSERT(task, ("TASK: NULL task passed to jtask_stop\n"));

    DBG_V(DJOS_SYNC, ("TASK: %d Stopping task %p cb - %p\n",
        task ? task->type : 99, task,
        task ? (void *)task->cb : (void *)0xFFFF0000));
    jspinlock_lock(task_lock);

    switch (task->state)
    {
    case JTASK_STOPPED:
        jspinlock_unlock(task_lock);
        return;
        
    case JTASK_PENDING:
        task_remove(task);
        task->state = JTASK_STOPPED;
        jspinlock_unlock(task_lock);
        break;
        
    case JTASK_RUNNING:
        task->state = JTASK_STOPPING;
        jspinlock_unlock(task_lock);

        task_stop_wait(task);
        return;
        
    case JTASK_RUN_N_PEND:
        task_remove(task);
        task->state = JTASK_STOPPING;
        jspinlock_unlock(task_lock);
        
        task_stop_wait(task);
        break;
    }

    /* Update the wakeup time of the relevant task thread */
    task_signal(task->type, &time);
}

/**
 * Function name:  task_stop_wait
 * Description: Waits for a running task to finish.
 * Parameters:
 *     @task: The task to wait for.
 *
 * Return value:   None
 * Scope:          Local
 **/
static void task_stop_wait(jtask_h task)
{
#ifdef JDEBUG
    if (jget_context() == task_thread_arr[task->type].context)
    {
        DBG_E(DJOS_SYNC, ("TASK: Task trying to stop itself !\n"));
        return;
    }
#endif

    while (task->state != JTASK_STOPPED)
    {
        DBG_E(DJOS_SYNC, ("TASK: Waiting for task %p to stop %d [%x:%x]\n",
            task, task->state, jget_context(), 
            task_thread_arr[task->type].context));

        jmsleep(task_thread_arr[task->type].sleep_handle, 50);
        
#ifdef JDEBUG
        if (task->state == JTASK_STOPPED)
            DBG_I(DJOS_SYNC, ("TASK: Task %p stopped\n", task));
#endif
    }
	#endif /* FEATURE_HS_USB_BASIC */
}

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_add
 * Description: Add a task to the threads task schedule list
 * Parameters: 
 *     @task: Task to schedule
 *     @time: Current system time
 *     @timeout: In how many msecs the task should run
 *     @cb: Callback to call when timeout expires
 *     @arg: Argument to pass the callback
 *     Scope: local
 *    */
static void task_add(jtask_h task, jtime_t time, juint32_t timeout, 
    jthread_func cb, void *arg)
{
    jtask_h *ptr;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DJOS_SYNC, ("TASK: %d Adding task %p in %d\n",
        task->type, task, TIME_DIFF(time, task->timeout)));
#endif /* CONFIG_DATA_PATH_DEBUG */

    task->cb    = cb;
    task->arg   = arg;

    TIME_ADD_MS(&time, timeout);

    j_memcpy(&task->timeout, &time, sizeof(time));

    /* Insert into a (time to run) sorted list */
    for (ptr = &task_thread_arr[task->type].list.next; 
        *ptr && (TIME_OVER(task->timeout, (*ptr)->timeout));
        ptr = &(*ptr)->next)
        ;

    task->next = *ptr;
    *ptr = task;
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_signal
 * Description: Wake the task thread
 * next task.
 * Parameters: 
 *     @type: Type of thread to wake 
 *     @time: The current time
 *     Scope: local
 *    */
static void task_signal(jtask_type type, jtime_t *time)
{
    task_thread_h ctx = &(task_thread_arr[type]);

    /* If there are no tasks pending, let it sleep forever */
    if (!task_thread_arr[type].list.next)
    {
        DBG_X(DJOS_SYNC, ("TASK: %d No task, letting thread sleep\n", type));
        return;
    }
    
    /* If the thread needs waking up, do it ! */
    jwakeup(ctx->sleep_handle);
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_thread
 * Description: Main task thread, runs continiously dispatching tasks
 * Parameters: 
 *     @thread_arg: The type of the task thread
 *     Scope: local
 *    */
static void task_thread(void *thread_arg)
{
    jtask_h     task;
    jtime_t     time;
    jtask_type  type = (jtask_type)((juint32_t)thread_arg);
    jthread_func cb;
    void        *arg;
    jint32_t     next_task_time;
    task_thread_h ctx = &(task_thread_arr[type]);

    ctx->context = jget_context();

    DBG_I(DJOS_SYNC, ("TASK: %d Task thread created [%x]\n", 
        type, ctx->context));

#ifndef CONFIG_TASK_SINGLE
    if (type == TASK_CLASS_DRIVER)
        CORE_SAFE_ENTER;
#endif

    /* Run executing tasks until stack uninit */
    while (ctx->flags & TASK_THREAD_ACTIVE)
    {
        /* Get current time */
        jget_time(&time);

#ifdef CONFIG_DATA_PATH_DEBUG
       DBG_X(DJOS_SYNC, ("TASK: %d Task thread sleeping\n", type));
#endif /* CONFIG_DATA_PATH_DEBUG */

        /* Protected region (so DSR does not interfere) */
        jspinlock_lock(task_lock);
        {
            /* Get next task to execute */
            task = ctx->list.next;

            /* Calculate when next task should expire */
            if (!task)
            {
                /* No task will make msleep() sleep forever */
                next_task_time = 0;
            }
            else 
            {
                /* How long is it until the next task ? */
                next_task_time = TIME_DIFF(time, task->timeout);

                /* For lower priority tasks, we force the thread to 
                 * "rest" between tasks to allow more important things to run */
                if (task->type != TASK_CONTROLLER)
                    next_task_time = MAX(next_task_time, TASK_EXEC_INTERVAL);
            }
        }
        jspinlock_unlock(task_lock);

        /* Check if to wake up immediately or sleep until needed */
        if (next_task_time > 0 || !task)
        {
#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_X(DJOS_SYNC, ("TASK: %d Going to sleep for %d\n", 
                type, next_task_time));
#endif /* CONFIG_DATA_PATH_DEBUG */
            
#ifdef JDEBUG
            ctx->state = TASK_THREAD_SLEEPING;
#endif
#ifndef CONFIG_TASK_SINGLE
            if (type == TASK_CLASS_DRIVER)
                CORE_SAFE_LEAVE;
#endif
             jmsleep(ctx->sleep_handle, next_task_time);

#ifndef CONFIG_TASK_SINGLE
            if (type == TASK_CLASS_DRIVER)
                CORE_SAFE_ENTER;
#endif
        }

#ifdef JDEBUG
        ctx->state = TASK_THREAD_RUNNING;
        ctx->task_runs++;
#endif

        ctx->flags &= ~TASK_THREAD_PAUSED;

#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_I(DJOS_SYNC, ("TASK: %d Task thread wokeup\n", type));
#endif /* CONFIG_DATA_PATH_DEBUG */
        
        /* Protected region (so DSR does not interfere) */
        jspinlock_lock(task_lock);
        {
            /* Get first task */
            task = ctx->list.next;
            if (!task)
            {
                jspinlock_unlock(task_lock);
                continue;
            }

#ifdef JDEBUG
            if (task->state != JTASK_PENDING)
            {
                jspinlock_unlock(task_lock);
                DBG_E(DJOS_SYNC, ("TASK: Task on list is not in "
                    "Pending state but in %d\n", task->state));
                continue;
            }
#endif

            KASSERT(task->cb, ("TASK: %d Scheduled task has no CB ! %p\n", 
                task->type, task));

            /* Get current time */
            jget_time(&time);

#ifdef JDEBUG
            j_memcpy(&ctx->run_start, &time, sizeof(jtime_t));
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_V(DJOS_SYNC, ("TASK: %d Task thread checking %p [%d] %s\n",
                type, task, TIME_DIFF(time, task->timeout),
                TIME_OVER(time, task->timeout) ? "ready" : "not ready"));
#endif /* CONFIG_DATA_PATH_DEBUG */

            /* Check if its too soon to run the task */
            if (!TIME_OVER_MS(time, task->timeout))
            {
                jspinlock_unlock(task_lock);    
#ifdef CONFIG_DATA_PATH_DEBUG
                DBG_V(DJOS_SYNC, ("TASK: %d Still not time for %p left %d\n",
                    task->type, task, TIME_DIFF(time, task->timeout)));
#endif /* CONFIG_DATA_PATH_DEBUG */
                continue;
            }

            cb = task->cb;
            arg = task->arg;

#ifdef JDEBUG
            ctx->curr_task = task;
#endif
            /* Remove from task list */
            task_remove(task);

            task->state = JTASK_RUNNING;

#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_I(DJOS_SYNC, ("TASK: %d Running task %p\n", task->type, task));
#endif /* CONFIG_DATA_PATH_DEBUG */
        }
        jspinlock_unlock(task_lock);    

        /* Call task's callback */
        cb(arg);
       
        jspinlock_lock(task_lock);
        switch (task->state)
        {
        case JTASK_RUNNING:
            task->state = JTASK_STOPPED;
            break;
        case JTASK_RUN_N_PEND:
            task->state = JTASK_PENDING;
            break;
        case JTASK_STOPPING:
            task->state = JTASK_STOPPED;
            /* Wakeup anyone who wants to know what happened 
             * to the thread/tasks */
            jwakeup(ctx->sleep_handle);

            break;
#ifdef JDEBUG
        default:
            jspinlock_unlock(task_lock);
            j_panic("TASK: Unexpected state of finished thread, %d\n",
                task->state);
#endif
        }
        jspinlock_unlock(task_lock);
        
#ifdef JDEBUG
        ctx->curr_task = NULL;
#endif
    }

#ifndef CONFIG_TASK_SINGLE
    if (type == TASK_CLASS_DRIVER)
        CORE_SAFE_LEAVE;
#endif

    DBG_I(DJOS_SYNC, ("TASK: %d Task thread shutting down\n", type));
    ctx->flags |= TASK_THREAD_FINISHED;
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_thread_init
 * Description: Initialize and start a single task thread
 * Parameters: 
 *     @index: Type of task thread to start
 *
 * Return value: jresult_t
 * Scope: local
 **/
static jresult_t task_thread_init(jtask_type index)
{
    jresult_t rc;
    task_thread_h ctx = &(task_thread_arr[index]);

    KASSERT(ctx, ("TASK: %d Trying to init a thread without a context\n", 
        index));

    j_memset(ctx, 0, sizeof(struct task_thread_s));

    /* Initialize thread sleep mutex */
    rc = jmsleep_init(&ctx->sleep_handle);
    if (rc)
        goto error;
    
    ctx->flags |= TASK_THREAD_SLEEP_HANDLE_INIT;

    /* Start the task thread */
#ifdef JDEBUG
    ctx->state = TASK_THREAD_SLEEPING;
#endif

    ctx->flags |= TASK_THREAD_ACTIVE;

#if defined(JDEBUG) && !defined(CONFIG_TASK_SINGLE)
    if (index == TASK_DEBUG)
    {
        rc = jthread_create(task_monitor_thread, (void *)index, 
            (os_thread_priority)index, &ctx->handle);
    }
    else
#endif
    {
        rc = jthread_create(task_thread, (void *)index, 
            (os_thread_priority)index, &ctx->handle);
    }
    if (rc)
        goto error;
    
    ctx->flags |= TASK_THREAD_THREAD_INIT;

    return 0;

error:
    task_thread_uninit(index);

    return rc;
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_thread_uninit
 * Description: Uninitialize a single task thread
 * Parameters: 
 *     @index: Type of task thread to un-initialize
 *     Scope: local
 *    */
static void task_thread_uninit(jtask_type index)
{
    task_thread_h ctx = &(task_thread_arr[index]);

    DBG_I(DJOS_SYNC, ("TASK: %d Starting thread uninit\n", index));
   
    if (ctx->flags | TASK_THREAD_THREAD_INIT)
    {
        /* Check if there are any pending tasks left */
        if (ctx->list.next)
        {
            DBG_E(DJOS_SYNC, ("TASK: %d Task thread still has "
                "tasks left\n", index));

            ctx->list.next = NULL;
        }

        /* Signal thread that it should die */
        ctx->flags &= ~TASK_THREAD_ACTIVE;

        jwakeup(ctx->sleep_handle);

        /* Wait for thread to stop */
        while (!(ctx->flags & TASK_THREAD_FINISHED))
        {
            DBG_I(DJOS_SYNC, ("TASK: %d waiting for task to die\n", index));
            jdelay_ms(55);
        }
    }

    if (ctx->flags | TASK_THREAD_SLEEP_HANDLE_INIT)
    {
        ctx->flags &= ~TASK_THREAD_SLEEP_HANDLE_INIT;
        jmsleep_uninit(ctx->sleep_handle);
    }
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_threads_init
 * Description: Initialize all task threads
 * Parameters: 
 *
 * Return value: jresult_t
 * Scope: local
 **/
static jresult_t task_threads_init(void)
{
    jresult_t rc = 0;
    jint_t i;

    /* Initialize the task sync lock */
    rc = jspinlock_init(&task_lock);
    if (rc)
        return rc;
    
    /* Start all the threads */
    for (i = 0; i < TASK_TYPE_COUNT && !rc; i++)
        rc = task_thread_init((jtask_type)i);

    /* If all is fine, return */
    if (rc) 
        goto error;
    
    return 0;

error:
    /* Un-Init what we did manage to start */
    for (i--; i >= 0; i--)
        task_thread_uninit((jtask_type)i);

    if (task_lock)
        jspinlock_uninit(task_lock);

    return rc;
}
#endif /* HSU addition */

#ifndef FEATURE_HS_USB_BASIC /* HSU addition */
/**
 * Function name:  task_threads_uninit
 * Description: Un-Initialize all the task threads
 * Parameters: 
 * Scope: local
 **/
static void task_threads_uninit(void)
{
    jint_t i;

    for (i = 0; i < TASK_TYPE_COUNT; i++)
        task_thread_uninit((jtask_type)i);
 
    if (task_lock)
    {
        jspinlock_uninit(task_lock);
        task_lock = NULL;
    }
}
#endif /* HSU addition */

#if defined(JDEBUG) && !defined(CONFIG_TASK_SINGLE)
/* Approximate time between stack getting stuck and developer 
 * resetting the pc */
#define TASK_MAX_REASONABLE_RUN_TIME 33000

/**
 * Function name:  task_list_dump
 * Description: Prints out the information about each task in the schedule
 * Parameters: 
 *     @index: Index of task thread
 *     Scope: local
 *    */
static void task_list_dump(jtask_type index)
{
    juint32_t i;
    jtask_h task;
    jtime_t time;
    task_thread_h ctx = &(task_thread_arr[index]);
    jget_time(&time);

    DBG_E(DJOS_SYNC, ("TASK: %d Dumping task list, Runs: %d\n",
        index, ctx->task_runs));

    for (i = 0, task = ctx->list.next; task; i++, task = task->next)
    {
        DBG_E(DJOS_SYNC, ("TASK: %d %d : task %p [%p:%p] in %d - [%s:%d]\n",
            index, i, task, task->cb, task->arg, 
            TIME_DIFF(time, task->timeout), task->file, task->line));
    }
}

/**
 * Function name:  task_monitor_check
 * Description: Check if current task is running too long and if there are tasks
 * that have been waiting too long
 * Parameters: 
 *     @index: Index of task thread to check
 *
 * Return value: 0 if there are no suspicious tasks and non-zero otherwise
 * Scope: local
 **/
static jresult_t task_monitor_check(jtask_type index)
{
    jtask_h task;
    jtime_t time;
    task_thread_h ctx = &(task_thread_arr[index]);
    jint32_t run_time;
    jresult_t rc = 0;

    jget_time(&time);

    if (ctx->state == TASK_THREAD_RUNNING)
    {
        if (!ctx->curr_task)
        {
            DBG_E(DJOS_SYNC, ("TASK: %d thread set as running but "
                "no task set\n", index));
            return JEINVAL;
        }
        
        /* Check if current task has been running too long */
        run_time = TIME_DIFF(ctx->run_start, time);
        if (run_time > TASK_MAX_REASONABLE_RUN_TIME)
        {
            DBG_E(DJOS_SYNC, ("TASK: %d Possible deadlock:\n", index));
            DBG_E(DJOS_SYNC, ("  Task %p [%p:%p] "
                "from [%s:%d] run %d msec\n",
                ctx->curr_task, ctx->curr_task->cb, 
                ctx->curr_task->arg, ctx->curr_task->file, 
                ctx->curr_task->line, run_time));
            rc = JEBUSY;
        }
    }
        
    /* Check if there are any tasks which have been waiting too long */
    for (task = ctx->list.next; task; task = task->next)
    {
        if (TIME_DIFF(time, task->timeout) < -TASK_MAX_REASONABLE_RUN_TIME)
        {
            DBG_E(DJOS_SYNC, ("TASK: %d Possible deadlock:\n", index));
            DBG_E(DJOS_SYNC, ("  Task %p [%p:%p] from [%s:%d] "
                "waiting for %d msec\n", task, task->cb, task->arg, 
                task->file, task->line, -(TIME_DIFF(time, task->timeout))));
            rc = JEBUSY;
        }
    }

    return rc;
}

/**
 * Function name:  task_monitor_thread
 * Description: Runs deadlock detection on all task threads periodicaly
 * Parameters: 
 *     @thread_arg: Index of the task_monitor thread in task_array
 *     Scope: local
 *    */
static void task_monitor_thread(void *thread_arg)
{
    jtask_type type = (jtask_type)((juint32_t)thread_arg);
    task_thread_h ctx = &(task_thread_arr[type]);
    jresult_t rc;
    jint_t i;

    DBG_I(DJOS_SYNC, ("TASK: Task monitor thread created\n"));

    /* Run executing tasks until stack uninit */
    while (ctx->flags & TASK_THREAD_ACTIVE)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_SYNC, ("TASK: Task monitor thread running\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

        /* Check all thread lists for problems */
        for (i = 0; i < TASK_DEBUG; i++)
        {
            rc = task_monitor_check((jtask_type)i);
            if (rc)
                task_list_dump((jtask_type)i);
        }

        jmsleep(ctx->sleep_handle, 5000);
    }

    DBG_I(DJOS_SYNC, ("TASK: Task monitor thread shutting down\n"));
    ctx->flags |= TASK_THREAD_FINISHED;
}

#endif

#endif

#ifndef SINGLE_THREADED
static jmutex_h CoreMtx;
#endif

#ifndef SINGLE_THREADED
void jcore_safe_enter(void)
{
    jmutex_lock(CoreMtx);
}

void jcore_safe_leave(void)
{
    jmutex_unlock(CoreMtx);
}

#ifdef DEBUG_SYNC
typedef struct jsafe_info_s {
    juint32_t    locker_id;
    char        *locker_file;
    int         locker_line;
    jbool_t      locked;
    os_mutex_h  os_handle;
} jsafe_info_t;

static jsafe_info_t giant_info;
static jspinlock_h sync_lock;

void jsafe_enter_d(char *file, int line)
{
    if (sync_lock)
    {
        jspinlock_lock(sync_lock);
        {
            KASSERT(!(giant_info.locked && 
                giant_info.locker_id == jget_context()),
                ("JOS: Thread holding giant is trying to "
                "lock it again [%s:%d], last locked at [%s:%d]\n", file, line,
                giant_info.locker_file, giant_info.locker_line));
        }
        jspinlock_unlock(sync_lock);
    }

    os_safe_enter();
    
    giant_info.locked = TRUE;
    giant_info.locker_file = file;
    giant_info.locker_line = line;
    giant_info.locker_id = jget_context();
}

void jsafe_leave_d(char *file, int line)
{
    KASSERT(giant_info.locked, ("JOS: Unlocking giant without it "
        "being locked [%s:%d]\n", file, line));

    KASSERT(giant_info.locker_id == jget_context(),
        ("JOS: Unlocking giant at [%s:%d] that was locked "
        "by another thread at [%s:%d]\n", file, line,
        giant_info.locker_file, giant_info.locker_line));

    giant_info.locked = FALSE;

    os_safe_leave();
}

jbool_t jsafe_check(char *file, int line)
{
    KASSERT(giant_info.locked, 
        ("JOS: Giant is not locked ! [%s:%d]\n", file, line));

    KASSERT(giant_info.locker_id == jget_context(),
        ("JOS: Thread running at [%s:%d] while giant is "
        "held by someone else, taken at [%s:%d]\n", file, line,
        giant_info.locker_file, giant_info.locker_line));

    DBG_X(DJOS_SYNC, ("JOS: Current thread at [%s:%d] %x taken giant "
        "at [%s:%d]\n", file, line, jget_context(),
        giant_info.locker_file, giant_info.locker_line));

    return TRUE;
}

jresult_t jmutex_init_d(jmutex_h *mtx_h, char *file, int line)
{
    jsafe_info_t *info;
    jresult_t rc;
    
    info = jmalloc(sizeof(struct jsafe_info_s), M_ZERO);
    if (!info)
        return JENOMEM;

    rc = os_mutex_init(&info->os_handle);
    if (rc)
    {
        jfree(info);
        return rc;
    }

    *mtx_h = (jmutex_h)info;

    return 0;
}

void jmutex_uninit_d(jmutex_h mtx_h, char *file, int line)
{
    jsafe_info_t *info = (jsafe_info_t *)mtx_h;
    KASSERT(info, ("JOS: No mutex handle given to jmutex_uninit [%s:%d]\n",
        file, line));

    KASSERT(!info->locked, ("JOS: Uninit at [%s:%d] of a "
        "mutex locked at [%s:%d]\n", file, line,
        info->locker_file, info->locker_line));

    os_mutex_uninit(info->os_handle);
    jfree(info);
}

void jmutex_lock_d(jmutex_h mtx_h, char *file, int line)
{
    jsafe_info_t *info = (jsafe_info_t *)mtx_h;
    
    KASSERT(info, ("JOS: No mutex handle given to jmutex_lock [%s:%d]\n",
        file, line));

    KASSERT(!(info->locked && info->locker_id == jget_context()),
        ("JOS: Thread holding mutex is trying to "
        "lock it again [%s:%d], last locked at [%s:%d]\n", file, line,
        info->locker_file, info->locker_line));

    os_mutex_lock(info->os_handle);

    info->locked = TRUE;
    info->locker_file = file;
    info->locker_line = line;
    info->locker_id = jget_context();
}

void jmutex_unlock_d(jmutex_h mtx_h, char *file, int line)
{
    jsafe_info_t *info = (jsafe_info_t *)mtx_h;
    
    KASSERT(info, ("JOS: No mutex handle given to jmutex_unlock [%s:%d]\n",
        file, line));

    KASSERT(info->locked, ("JOS: Unlocking mutex without it "
        "being locked [%s:%d]\n", file, line));

    KASSERT(info->locker_id == jget_context(),
        ("JOS: Unlocking mutex at [%s:%d] that was locked "
        "by another thread at [%s:%d]\n", file, line,
        info->locker_file, info->locker_line));

    info->locked = FALSE;

    os_mutex_unlock(info->os_handle);
}

#endif
#endif

/**
 * Function name:  jos_sync_init
 * Description: Initialize all sync releated jos implementations
 * Parameters: 
 *
 * Return value: jresult_t
 * Scope: global
 **/
jresult_t jos_sync_init(void)
{
    jresult_t rc = 0;

    DBG_I(DJOS_SYNC, ("JOS: Starting sync init\n"));

/* HSU addition, use to be "#ifndef SINGLE_THREADED" */
#if (!defined(SINGLE_THREADED) && !defined(FEATURE_HS_USB_BASIC))
    rc = task_threads_init();
    if (rc)
        goto Exit;

    rc = jmutex_init(&CoreMtx);
    if (rc)
    {
        task_threads_uninit();
        goto Exit;
    }

# ifdef DEBUG_SYNC
    rc = jspinlock_init(&sync_lock);
    if (rc)
    {
        task_threads_uninit();
        jmutex_uninit(CoreMtx);
        goto Exit;
    }
# endif

 Exit:
#endif
       
    DBG_I(DJOS_SYNC, ("JOS: Finished sync init %d\n", rc));

    return rc;
}

/**
 * Function name:  jos_sync_uninit
 * Description: Un-initialize all sync releated jos implementations
 * Parameters: 
 * Scope: global
 **/
void jos_sync_uninit(void)
{
    DBG_I(DJOS_SYNC, ("JOS: Starting sync un-init\n"));
#if (!defined(SINGLE_THREADED) && !defined(FEATURE_HS_USB_BASIC))
    task_threads_uninit();
    jmutex_uninit(CoreMtx);
# ifdef DEBUG_SYNC
    jspinlock_uninit(sync_lock);
# endif
#endif
    DBG_I(DJOS_SYNC, ("JOS: Finished sync un-init\n"));
}



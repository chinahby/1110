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
#include "includes.h"

typedef struct {
    void *arg;
    jthread_func func;
} daemonizer_arg_t;


static int deamonizer(void *arg)
{
    daemonizer_arg_t *ptr = (daemonizer_arg_t*)arg;

    daemonize("");

    jsafe_enter();
    ptr->func(ptr->arg);
    jsafe_leave();

    kfree(arg);

    return 0;
}

juint32_t os_get_context(void)
{
    return (juint32_t)(current);
}

jresult_t os_thread_create(jthread_func func, void *arg,
    os_thread_priority priority ,void **handle)
{
    jint_t pid;
    juint32_t linux_flags = CLONE_VM;

    daemonizer_arg_t *ptr = kmalloc(sizeof(daemonizer_arg_t), 0);

    if (!ptr)
        panic("thread create failed: Cannot allocate daemonizer memory");

    ptr->arg = arg;
    ptr->func = func;
    
    /* TODO: Handle the new priority parameter correctly */
    pid = kernel_thread(deamonizer, ptr, linux_flags);

    if (handle)
        *handle = (void *)pid;
    
    return pid < 0;
}

jresult_t os_mutex_init(os_mutex_h *mtx)
{
    *mtx = jos_malloc(sizeof(struct semaphore), M_ZERO);
    if (!*mtx)
        return -JENOMEM;

    init_MUTEX((struct semaphore *)*mtx);
    return 0;
}

void os_mutex_uninit(os_mutex_h mtx)
{
    if (mtx)
        jos_free(mtx);
}

void os_mutex_lock(os_mutex_h mtx)
{
    jsafe_leave();
    down((struct semaphore *)mtx);
    jsafe_enter();
}

void os_mutex_unlock(os_mutex_h mtx)
{
    up((struct semaphore *)mtx);
}

jresult_t os_spinlock_init(os_spinlock_h *lock_h)
{
    struct os_spinlock *lock;
    lock = *lock_h = jos_malloc(sizeof(struct os_spinlock), M_ZERO);
    if (!*lock_h)
        return -JENOMEM;
    
    spin_lock_init(&lock->spinlock);
    return 0;
}

void os_spinlock_uninit(os_spinlock_h lock)
{
    if (lock)
        jos_free(lock);
}

void os_spinlock_lock(os_spinlock_h lock)
{
    spin_lock_irqsave( &((struct os_spinlock*)lock)->spinlock,
        ((struct os_spinlock*)lock)->flags );
}

void os_spinlock_unlock(os_spinlock_h lock)
{
    spin_unlock_irqrestore( &((struct os_spinlock*)lock)->spinlock,
        ((struct os_spinlock*)lock)->flags );
}

#ifdef SINGLE_THREADED
struct os_callout_t
{
    struct timer_list t;
    struct work_struct bh;
    jthread_func ftn;
    void *arg;
} os_callout_t;

static void bh_func(void *arg)
{
    struct os_timer *c = (struct os_timer *)arg;
    OS_ASSERT(c, ("SYNC: Null callout\n"));

    if (c->status != CALLOUT_STARTED)
        return;

    c->os_callout->ftn(c->os_callout->arg);
    /* make sure not to use the callout struct after calling ftn() */
}

static void th_func(unsigned long arg)
{
    struct os_timer *c = (struct os_timer *)arg;
    OS_ASSERT(c, ("SYNC: Null callout\n"));

    if (c->status != CALLOUT_STARTED)
        return;

    schedule_work(&c->os_callout->bh);
}

jresult_t os_timer_init(os_timer_h *timer_h)
{
    struct os_timer *timer = (struct os_timer *)
        jos_malloc(sizeof(struct os_timer), M_ZERO);
    if (!timer)
        return -JENOMEM;
    
    timer->os_callout = (void *)&(timer->os_padding);
    timer->initialized = CALLOUT_INIT;
    init_timer(&(timer->os_callout->t));
    INIT_WORK(&(timer->os_callout->bh), bh_func, timer);
    timer->status = CALLOUT_STOPPED;

    *timer_h = timer;
    return 0;
}

void os_timer_uninit(os_timer_h timer_h)
{
    if (timer_h)
        jos_free(timer_h);
}

void os_timer_reset(os_timer_h timer_h, juint32_t timeout, jthread_func func, void *arg)
{
    struct os_timer *timer = (struct os_timer*)timer_h;

    if (timer->initialized != CALLOUT_INIT)
        panic("SYNC: Resetting an unitialized callout\n");

    timer->os_callout->ftn = func;
    timer->os_callout->arg = arg;
    del_timer(&timer->os_callout->t);
    timer->status = CALLOUT_STARTED;
    if (timeout <= 0)
    {
        schedule_work(&timer->os_callout->bh);
    }
    else
    {
        timer->os_callout->t.expires = jiffies + (timeout * os_get_hz() / 1000);
        timer->os_callout->t.function = th_func;
        timer->os_callout->t.data = (unsigned long) timer;
        add_timer(&timer->os_callout->t);
    }
}

void os_timer_stop(os_timer_h timer_h)
{
    struct os_timer *timer = (struct os_timer*)timer_h;
    int rc = 0;

    if (!timer)
        panic("SYNC: Trying to stop a NULL timer\n");
    
    if (timer->initialized != CALLOUT_INIT)
        panic("SYNC: Stopping an unitialized callout\n");
    
    timer->status = CALLOUT_STOPPED;
    rc = del_timer(&timer->os_callout->t);
}

jbool_t os_timer_pending(os_timer_h timer_h)
{
    struct os_timer *timer = (struct os_timer*)timer_h;
    int rc = 0;

    if (timer->initialized != CALLOUT_INIT)
        panic("SYNC: Pending check on an unitialized callout\n");

    rc = timer_pending((struct timer_list *)timer->os_callout);
    return rc;
}
#endif

void os_delay_us(juint32_t usec)
{
    /* udelay appears to be up to 10% off on some platforms,
     * we allow for longer then requested delays but not shorter.
     * So we add another 10% */
    udelay((usec * 11) / 10);
}

void os_delay_ms(juint32_t msec)
{
    /* schedule_timeout() appears to be up to 10% off on some platforms,
     * we allow for longer then requested delays but not shorter.
     * So we add about 10% */
    int timeout = 1 + (msec * os_get_hz()) / 900;
    jsafe_leave();
    while (timeout)
    {
        timeout = schedule_timeout(timeout);
        set_current_state(TASK_UNINTERRUPTIBLE);
        rmb();
    }
    set_current_state(TASK_RUNNING);
    jsafe_enter();
}

typedef struct sleep_q_t {
    wait_queue_head_t wqh;
    int done;
} sleep_q_t;

jresult_t os_msleep_init(os_msleep_h *msleep_h)
{
    jresult_t rc = 0;
    sleep_q_t *sq = jos_malloc(sizeof(sleep_q_t), M_ZERO);

    if (!sq)
    {
        rc = JENOMEM;
        goto Exit;
    }

    init_waitqueue_head(&sq->wqh);     

    *msleep_h = sq;
Exit:
    return rc;
}

void os_msleep_uninit(os_msleep_h msleep_h)
{
    sleep_q_t *sq = (sleep_q_t *)msleep_h;
    
    jos_free(sq);
}

jresult_t os_msleep(os_msleep_h msleep_h, juint32_t timeout)
{
    jresult_t rc = 0;
    sleep_q_t *sq = (sleep_q_t *)msleep_h;
    int timeo;

    DECLARE_WAITQUEUE(waitq, current);

    set_current_state(TASK_UNINTERRUPTIBLE);
    add_wait_queue(&sq->wqh, &waitq);
    timeo = timeout ? (timeout * os_get_hz() / 900) : MAX_SCHEDULE_TIMEOUT;

    jsafe_leave();
    while (!sq->done && timeo)
    {
        timeo = schedule_timeout(timeo);
        set_current_state(TASK_UNINTERRUPTIBLE);
        rmb();
    }
    if (!sq->done)
        rc = -JEWOULDBLOCK;

    sq->done = 0;
    
    remove_wait_queue(&sq->wqh, &waitq);
    
    set_current_state(TASK_RUNNING);
    jsafe_enter();

    return rc;
}

jresult_t os_wakeup(os_msleep_h msleep_h)
{
    sleep_q_t *sq = (sleep_q_t *)msleep_h;
    sq->done = 1;
    wmb();
    wake_up(&sq->wqh);
    return 0;
}

void os_get_time(os_time_t *time)
{
    struct timeval tvp;
    do_gettimeofday(&tvp);
    time->tv_sec = tvp.tv_sec;
    time->tv_usec = tvp.tv_usec;
}

juint32_t os_get_hz(void)
{
    return HZ;
}

void os_safe_enter()
{
    down(&GlobalMtx);
}

void os_safe_leave()
{
    up(&GlobalMtx);
}

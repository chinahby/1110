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
#ifndef _PORT_SYNC_H_
#define _PORT_SYNC_H_

/* HSU addition (begin) */
#include <port.h>
#include <jtypes.h>
/* HSU addition (end) */

/* Thread priorities enumeration.
 * The first three values must correspond to jtask_type and are for use by
 * the stack's task threads. 
 * THREAD_PRIORITY_OTHER is used by all additional threads required for
 * specific driver purposes. 
 * The priorities are ordered from highest to lowest (0 - highest) */
typedef enum {
    THREAD_PRIORITY_CONTROLLER = 0,
    THREAD_PRIORITY_CORE,
    THREAD_PRIORITY_DRIVER,
    THREAD_PRIORITY_OTHER
} os_thread_priority;

jresult_t os_thread_create(jthread_func func, void *arg, 
    os_thread_priority priority, void **handle);

juint32_t os_get_context(void);

jresult_t os_mutex_init(os_mutex_h *mtx);
void os_mutex_uninit(os_mutex_h mtx);
void os_mutex_lock(os_mutex_h mtx);
void os_mutex_unlock(os_mutex_h mtx);

jresult_t os_spinlock_init(os_spinlock_h *lock);
void os_spinlock_uninit(os_spinlock_h lock);
void os_spinlock_lock(os_spinlock_h lock);
void os_spinlock_unlock(os_spinlock_h lock);

jresult_t os_timer_init(os_timer_h *timer);
void os_timer_uninit(os_timer_h timer);
void os_timer_reset(os_timer_h timer, juint32_t msec, jthread_func func,
    void *arg);
void os_timer_stop(os_timer_h timer);
jbool_t os_timer_pending(os_timer_h timer);
    
void os_delay_us(juint32_t usec);
void os_delay_ms(juint32_t msec);

jresult_t os_msleep_init(os_msleep_h *msleep_h);
void  os_msleep_uninit(os_msleep_h msleep_h);
jresult_t os_msleep(os_msleep_h msleep_h, juint32_t timeout);
jresult_t os_wakeup(os_msleep_h msleep_h);

void os_get_time(os_time_t *time);
juint32_t os_get_hz(void);

#ifndef SINGLE_THREADED
/* context tracker (ISR, DSR or normal) */
#define NORMAL_CONTEXT 0x1
#define DSR_CONTEXT 0x2
#define ISR_CONTEXT 0x4
extern juint8_t jcurr_ctx;

#ifdef JDEBUG
#define ASSERT_CONTEXT(valid_ctx)                               \
    do {                                                        \
        if (!((jcurr_ctx) & (valid_ctx)))                        \
        {                                                       \
            os_panic("%s (%d): %s context invaild here!\n",     \
                __FILE__, __LINE__,                             \
                (jcurr_ctx == ISR_CONTEXT) ?  "ISR" :            \
                (jcurr_ctx == DSR_CONTEXT) ? "DSR" : "NORMAL");  \
        }                                                       \
    } while (0)
#else

#define ASSERT_CONTEXT(valid_ctx)                               

#endif
#endif

#endif


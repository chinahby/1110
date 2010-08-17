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
#include <jos.h>
#include "jms_async.h"
#include <uw_host_mass.h>

typedef struct async_thread_s {
    juint8_t     active;
    TAILQ_ENTRY(async_thread_s) next;
} async_thread_t;

typedef struct transfer_desc_s {
    jresult_t    status;
    void        *dev;
    disk_async_cb callback;
    void        *arg;
    juint8_t     *buffer; 
    juint8_t     flags; 
    juint32_t    start_sector;
    juint32_t    num_sectors;
    juint_t      direction;
    TAILQ_ENTRY(transfer_desc_s) next;
} transfer_desc_t;

static jmsleep_h task_list_sleep_h;
static TAILQ_HEAD(task_list_q, transfer_desc_s) task_list;
static TAILQ_HEAD(thread_list_q, async_thread_s) thread_list;

static void async_thread(void *arg);

static jresult_t async_thread_create(void)
{
    async_thread_t *context = NULL;
    void *handle;
    jresult_t rc;

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Creating task thread\n"));

    context = jmalloc(sizeof(async_thread_t), 0);
    if (!context)
        return JENOMEM;

    context->active = 1;

    rc = jthread_create(async_thread, context, THREAD_PRIORITY_OTHER, &handle);
    if (rc)
    {
        jfree(context);
        return rc;
    }

    /* Add to active list */
    TAILQ_INSERT_TAIL(&thread_list, context, next);

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Task thread created\n"));

    return 0;
}

static void async_thread_killall(void)
{
    async_thread_t *context = NULL;

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Killing all task threads\n"));

    /* Mark all threads as waiting to die */
    TAILQ_FOREACH(context, &thread_list, next)
    {
        context->active = 0;
    }

    /* Wait for all threads to die */
    while (TAILQ_FIRST(&thread_list))
    {
        DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Waiting for task threads to die %p\n",
            TAILQ_FIRST(&thread_list)));

        jwakeup(task_list_sleep_h);
        jdelay_ms(50);
    }
    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: All task threads killed\n"));
}

jresult_t jms_async_init(void)
{
    jresult_t rc;
    jint_t i;
    task_list_sleep_h = NULL;

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Starting init\n"));

    /* Init task list */
    TAILQ_INIT(&task_list);

    /* Init thread list */
    TAILQ_INIT(&thread_list);

    /* Create the task sleep handle */
    rc = jmsleep_init(&task_list_sleep_h);
    if (rc)
        goto Error;

    /* Create async threads */
    for (i = 0; i < JMS_ASYNC_THREADS; i++)
    {
        rc = async_thread_create();
        if (rc)
            goto Error;
    }

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Successfuly started\n"));

    return 0;

Error:
    jms_async_uninit();

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Error during init %d\n", rc));

    return rc;
}

void jms_async_uninit(void)
{
    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Starting un-init\n"));

    /* We should not be called before all requests have been finished */
    if (TAILQ_FIRST(&task_list))
    {
        DBG_E(DMASS_ASYNC, ("JMS_ASYNC: Uninit called with pending tasks !\n"));
        return;
    }

    /* Kill all threads */
    async_thread_killall();

    /* Uninit task sleep handle */
    if (task_list_sleep_h)
        jmsleep_uninit(task_list_sleep_h);

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Un-init finished\n"));
}

jresult_t disk_async_transfer(void *dev, disk_async_cb callback, void *arg,
    juint8_t *buffer, juint8_t flags, 
    juint32_t start_sector, juint32_t num_sectors, juint_t direction)
{
    transfer_desc_t *xfer;

    DBG_V(DMASS_ASYNC, ("JMS_ASYNC: Starting transfer cb %p\n", callback));

    /* Make sure we have someone to call */
    if (!callback)
        return JEINVAL;

    /* Get a descriptor */
    xfer = jmalloc(sizeof(struct transfer_desc_s), M_ZERO);
    if (!xfer)
        return JENOMEM;

    /* Save relevant data */
    xfer->dev = dev;
    xfer->callback = callback;
    xfer->arg = arg;
    xfer->buffer = buffer;
    xfer->flags = flags;
    xfer->start_sector = start_sector;
    xfer->num_sectors = num_sectors;
    xfer->direction = direction;

    /* Add the transfer */
    TAILQ_INSERT_TAIL(&task_list, xfer, next);

    DBG_X(DMASS_ASYNC, ("JMS_ASYNC: Transfer started\n"));

    /* Wake up someone */
    jwakeup(task_list_sleep_h);

    return 0;
}

static void async_thread(void *arg)
{
    async_thread_t *context = (async_thread_t *)arg;
    transfer_desc_t *xfer;
    jresult_t rc;

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Async thread started %p\n", arg));

    while (context->active)
    {
        /* Check if there are any pending tasks */
        xfer = (transfer_desc_t *)TAILQ_FIRST(&task_list);
        if (!xfer)
        {
            DBG_X(DMASS_ASYNC, ("JMS_ASYNC: Async thread sleeping %p\n", arg));

            /* Sleep until something happens */
            jmsleep(task_list_sleep_h, 0);

            continue;
        }

        DBG_V(DMASS_ASYNC, ("JMS_ASYNC: Async thread %p starting %p\n",
            arg, xfer));

        /* Remove transfer from task list */
        TAILQ_REMOVE(&task_list, xfer, next);

        /* Start the transfer */
        rc = disk_sync_transfer(xfer->dev, xfer->buffer, xfer->flags,
            xfer->start_sector, xfer-> num_sectors, xfer->direction);

        /* Save the result */
        xfer->status = rc;

        DBG_V(DMASS_ASYNC, ("JMS_ASYNC: Async thread %p finished %p with %d\n",
            arg, xfer, rc));

        /* Tell the user we are done */
        xfer->callback(xfer->arg, xfer->status); 

        /* Release the xfer descriptor */
        jfree(xfer);
    }

    DBG_I(DMASS_ASYNC, ("JMS_ASYNC: Async thread terminating %p\n", arg));

    /* Remove ourselves from thread list */
    TAILQ_REMOVE(&thread_list, context, next);

    /* Free our context memory */
    jfree(context);
}

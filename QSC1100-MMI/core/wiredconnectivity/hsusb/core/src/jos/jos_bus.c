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

typedef struct {
    jthread_func soft_intr;
    void        *arg;
    jtask_h     isr_task_h;
} isr_data_t;

/**
 * Function name:  jgeneric_dsr
 * Description: Manages context switch from DSR to software interrupt
 * Parameters: 
 *     @arg: Interrupt handler wrapper
 *     Scope: global
 **/
void jgeneric_dsr(void *arg)
{
    isr_data_t *data = (isr_data_t *)arg;
    KASSERT(arg, ("JBUS: No wrapper handle given to generic dsr\n"));
   
    /* If there is no software interrupt handler, just quit */
    if (!data->soft_intr)
        return;
#ifdef SINGLE_THREADED
    data->soft_intr(data->arg);
#else
    /* Schedule software interrupt task */
    jtask_schedule(data->isr_task_h, 0, data->soft_intr, data->arg);
#endif
}

/**
 * Function name:  jinterrupt_teardown
 * Description: Remove an interrupt handler
 * Parameters: 
 *     @res: Resource to unbind from
 *     @handle: Handle to interrupt wrapper created in interrupt_setup
 *
 * Return value: None
 * Scope: global
 *    */
void jinterrupt_teardown(jbus_resource_h res, void *handle)
{
    isr_data_t *data = (isr_data_t *)handle;
    jresult_t rc;

    KASSERT(handle, ("JBUS: No wrapper handle given to interrupt teardown\n"));

    rc = os_interrupt_teardown(res);
    if (rc)
        DBG_E(DJOS_DRIVER, ("JBUS: Failed to tear down interrupt\n"));
#ifndef SINGLE_THREADED
    jtask_uninit(data->isr_task_h);
#endif

    jfree(data);
}

/**
 * Function name:  jinterrupt_setup
 * Description: Sets up an interrupt handler with a wrapper to give correct
 * context to software interrupts
 * Parameters: 
 *     @res:      (IN)  Bus resource to attach to
 *     @isr_func: (IN)  Hardware interrupt function to call
 *     @isr_arg:  (IN)  Argument to hardware interrupt
 *     @dsr_func: (IN)  Software interrupt function to call
 *     @dsr_arg:  (IN)  Argument to software interrupt
 *     @handle:   (OUT) Handle to newly created interrupt
 *
 * Return value: 0 on success, error otherwise
 * Scope: global
 **/
jresult_t jinterrupt_setup(jbus_resource_h res, 
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg, void **handle)
{
    isr_data_t *data;
    jresult_t rc;

    /* Alloc data for interrupt wrapper */
    data = jmalloc(sizeof(isr_data_t), 0);
    if (!data)
        return JENOMEM;

#ifndef SINGLE_THREADED
    /* Init task for software interrupt */
    rc = jtask_init(&data->isr_task_h, TASK_CONTROLLER);
    if (rc)
        goto Exit;
#endif

    data->soft_intr = dsr_func;
    data->arg = dsr_arg;

    /* Setup actual interrupt handler with the bus */
    rc = os_interrupt_setup(res, isr_func, isr_arg,
        jgeneric_dsr, (void *)data);
    if (rc)
        goto Exit;
    
    *handle = data;

    return 0;

Exit:
#ifndef SINGLE_THREADED
    if (data && data->isr_task_h)
        jtask_uninit(data->isr_task_h);
#endif

    if (data)
        jfree(data);

    *handle = NULL;

    return rc;
}


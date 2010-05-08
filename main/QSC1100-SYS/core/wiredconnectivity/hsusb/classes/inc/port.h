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
#ifndef _PORT_H_
#define _PORT_H_

#include <jconfig.h>
#include <compiler.h>
#include <controllers.h>
#include <os_common.h>

void os_safe_enter(void);
void os_safe_leave(void);

typedef void *os_bus_resource_h;
typedef void *os_bus_h;
typedef void *os_mutex_h;
typedef void *os_spinlock_h;
typedef void *os_timer_h;
typedef void *os_msleep_h;

typedef void (*jthread_func)(void *);

#define ISR_DONT_CALL_SOFT_INTR 0
#define ISR_CALL_SOFT_INTR 1
#define ISR_NOT_RECOGNIZED -1

typedef jint_t (*interrupt_handler)(void *);

typedef struct os_time {
    jint32_t tv_sec;     /* seconds */
    jint32_t tv_usec;    /* microseconds */
} os_time_t;

typedef enum {
    CONTROLLER_HOST,
    CONTROLLER_DEVICE,
    CONTROLLER_OTG
} controller_t;

#include <port_sync.h>
#include <port_mem.h>
#include <port_generic.h>
#include <port_bus.h>
#include "uw_args.h"

#ifdef CONFIG_BUS_PCI
# include <pci_defs.h>
# include <port_pci.h>
#endif

#ifdef CONFIG_STATISTICS   
# include <port_statistics.h>
#endif

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
# include <port_test.h>
#endif

/* Usb stack API's to be used by the porting layer */

/* Initialize and load the stack */
jresult_t jstart_stack(uw_args_t *args);

/* Un-Initialize and un-load the stack */
void jstop_stack(void);

/* Register a controller with the host stack */
jresult_t add_generic_controller(os_bus_h bus, jint_t id, void **handle);

/* Un-Register a controller */
jresult_t del_generic_controller(void *handle);

jresult_t generic_controller_suspend(void *handle);
jresult_t generic_controller_resume(void *handle);

#endif


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
#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#define OS_ASSERT(expr,msg) do {                                \
        if (!(expr)) {                                          \
            os_printf msg ;                                     \
            os_panic("Assertion");                              \
        }                                                       \
    } while(0)

/* Global sync mutex */
extern struct semaphore GlobalMtx;

struct os_bus_resource 
{
    struct os_bus_resource *next; /* use simple linked list instead of macros */
    juint32_t    r_start;        /* index of the first entry in this resource */
    juint32_t    r_len;          
    int         r_type;         /* SYS_RES_IRQ/SYS_RES_MEMORY/SYS_RES_IOPORT */
    void        *r_virtual;     /* virtual address of this resource */
    struct      os_bus *r_bus;  /* bus which has allocated this resource */
    jint_t       r_rid;          /* optional rid for this resource.
                                 * for PCI mem/io this is the offset of the BAR
                                 * e.g. 0x10 for ehci/ohci and 0x20 for uhci
                                 * it is 0 for interrupts */
    void        *cookie;
};

struct os_bus
{
#ifdef CONFIG_BUS_PCI
    struct pci_dev *os_device;
#endif
    struct os_bus_resource *resources;
    void *device_handle;
};

#ifdef SINGLE_THREADED
struct os_callout_t;
#define OS_CALLOUT_SIZE         32
struct os_timer {
#define CALLOUT_INIT            0xB00A0123
    int initialized;
#define CALLOUT_STARTED         1
#define CALLOUT_STOPPED         2
    int status;
    int to_ticks;
    jthread_func ftn;
    void *arg;
    struct callout *next;

    struct os_callout_t *os_callout;
    int os_padding[OS_CALLOUT_SIZE];
};
#endif

struct os_spinlock
{
    spinlock_t spinlock;
    unsigned long flags;
};

#if defined CONFIG_JOS_BLOCK
int jos_block_init(void);
void jos_block_uninit(void);
#endif

#if defined CONFIG_JSERIAL
int serial_dev_init(void);
void serial_dev_uninit(void);
#endif

#if defined CONFIG_JHOST_CDC_ECM
int cdc_ecm_init(void);
void cdc_ecm_uninit(void);
#endif

#ifdef CONFIG_PORT_NET_DEV
jresult_t net_dev_init(void);
void net_dev_uninit(void);
#endif 

#endif


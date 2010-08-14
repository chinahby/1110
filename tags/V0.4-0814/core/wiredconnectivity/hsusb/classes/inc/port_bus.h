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
#ifndef _PORT_BUS_
#define _PORT_BUS_

jresult_t os_interrupt_setup(os_bus_resource_h res, 
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg);

jresult_t os_interrupt_teardown(os_bus_resource_h res);

jresult_t os_hw_init(void);
void os_hw_uninit(void);

#define SYS_RES_IRQ     1       /* interrupt lines */
#define SYS_RES_MEMORY  3       /* i/o memory */
#define SYS_RES_IOPORT  4       /* i/o ports */
#define SYS_RES_TRANSCEIVER  5   /* Transceiver controller connected to port */

os_bus_resource_h os_bus_alloc_resource(os_bus_h bus, jint_t type, jint_t id);
void os_bus_release_resource(os_bus_h bus, os_bus_resource_h res);

juint8_t os_bus_space_read_1(os_bus_resource_h res, juint32_t offset);
juint16_t os_bus_space_read_2(os_bus_resource_h res, juint32_t offset);
juint32_t os_bus_space_read_4(os_bus_resource_h res, juint32_t offset);
void os_bus_space_write_1(os_bus_resource_h res, juint32_t offset,
    juint8_t value);
void os_bus_space_write_2(os_bus_resource_h res, juint32_t offset, 
    juint16_t value);
void os_bus_space_write_4(os_bus_resource_h res, juint32_t offset, 
    juint32_t value);

os_bus_h os_bus_get_transceiver(os_bus_resource_h res);
jint_t os_bus_get_controller_id(os_bus_resource_h res);

typedef enum {
#ifdef CONFIG_JSLAVE
    NOTIFY_DEVICE_CONNECT       = 0,
    NOTIFY_DEVICE_DISCONNECT,
    NOTIFY_DEVICE_SUSPEND,
    NOTIFY_DEVICE_RESUME,
    NOTIFY_DEVICE_RESUME_COMPLETED,
    NOTIFY_DEVICE_REMOTE_WAKEUP,
    NOTIFY_DEVICE_CONFIGURED,
    NOTIFY_DEVICE_UNCONFIGURED,
    /* HSU Addition */
    NOTIFY_DEVICE_RESET,
    /* HSU Addition end */
#endif
    
#ifdef CONFIG_JHOST 
    NOTIFY_HOST_CONNECT         = 50,
    NOTIFY_HOST_SUSPEND,
    NOTIFY_HOST_RESUME,
#endif

#ifdef CONFIG_JOTG
    NOTIFY_OTG_IDLE             = 100,
    NOTIFY_OTG_BUSY, 
#endif

    NOTIFY_LAST
} jevent_t;

jresult_t os_notify(jevent_t event, void *param);

#ifdef CONFIG_PORT_INTERCHIP
typedef struct {
    jbool_t is_interchip;
} port_info_t;

jbool_t os_power_request(juint8_t voltage_class, juint8_t max_current, 
    juint8_t *set_voltage_class, juint8_t *set_max_current);

void os_get_port_info(os_bus_h bus, juint8_t port, port_info_t *info);
#endif

#endif


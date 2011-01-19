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

struct int_wrapper
{
    struct int_wrapper *next;
    void *isr_ctx;
    void *dsr_ctx;
    interrupt_handler isr_func;
    jthread_func dsr_func;
    struct tasklet_struct tl;
};

static void tasklet_wrapper(unsigned long v)
{
    struct int_wrapper *c = (struct int_wrapper *)v;
    
    if (c && c->dsr_func)
        c->dsr_func(c->dsr_ctx);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static irqreturn_t wrapper_hw_intr(int irq, void *ctx)
#else
static irqreturn_t wrapper_hw_intr(int irq, void *ctx, struct pt_regs *pt)
#endif
{
    struct int_wrapper *c = (struct int_wrapper *)ctx;
    jint_t rc = 0;

    if (c && c->isr_func)
    {
        rc = c->isr_func(c->isr_ctx);
        if (rc == ISR_CALL_SOFT_INTR && c->dsr_func)
            tasklet_schedule(&c->tl);
    }
    
    /* Always return 1 (unless this is not our interrupr) to signal the kernel
     * that we handled the interrupt - otherwise after a while the kernel will
     * disable this IRQ */
    return (rc == ISR_NOT_RECOGNIZED ? 0 : 1);
}

jresult_t os_interrupt_setup(os_bus_resource_h res_h, interrupt_handler isr_func,
    void *isr_arg, jthread_func dsr_func, void *dsr_arg)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;
    juint32_t flags = SA_INTERRUPT;
    juint32_t irq = res->r_start;
    struct int_wrapper *c;
    jresult_t rc;
    jbool_t is_shared = 1;

    c = jos_malloc(sizeof(struct int_wrapper), M_ZERO);
    if (!c)
        return JENOMEM;

    c->isr_func = isr_func;
    c->isr_ctx = isr_arg;
    c->dsr_func = dsr_func;
    c->dsr_ctx = dsr_arg;
    if (is_shared)
        flags |= SA_SHIRQ;

    rc = request_irq(irq, wrapper_hw_intr, flags, "int", c);
    if (rc)
    {
        jos_free(c);
        return rc;
    }
    res->cookie = c;
   
    if(dsr_func)
        tasklet_init(&c->tl, tasklet_wrapper, (unsigned long)c);
    
    return rc;
}

jresult_t os_interrupt_teardown(os_bus_resource_h res_h)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;
    struct int_wrapper *c = (struct int_wrapper *)res->cookie;
    
    if(c->dsr_func)
        tasklet_kill(&c->tl);

    free_irq(res->r_start, res->cookie);
    jos_free(res->cookie);
    return 0;
}

/* Get notifications about bus state changes */
jresult_t os_notify(jevent_t event, void *param)
{
    return 0;
}

#ifdef CONFIG_BUS_PCI
#define USB_CLASS PCIC_SERIALBUS << 16 | PCI_CLASS_SERIAL_USB << 8
#define OHCI_CLASS (USB_CLASS | PCIP_SERIALBUS_USB_OHCI)
#define UHCI_CLASS (USB_CLASS | PCIP_SERIALBUS_USB_UHCI)
#define EHCI_CLASS (USB_CLASS | PCIP_SERIALBUS_USB_EHCI)
#define ISP116X_VID 0x10b5
#define ISP116X_DID 0x5406
/* Verify vedor and product id of ISP1362 */
#define ISP1362_VID 0x10b5
#define ISP1362_DID 0x5406
/* Verify vedor and product id of ISP176x */
#define ISP176x_VID 0x10b5
#define ISP176x_DID 0x5406
/* Verify vedor and product id of TD243 */
#define TD243_VID 0x10B5
#define TD243_DID 0x9030

/* PCI driver selection metadata; PCI hotplugging uses this */
static const struct pci_device_id __devinitdata pci_ids [] = {
#ifdef CONFIG_EHCI
{
        class:          EHCI_CLASS,
        class_mask:     ~0,
        driver_data:    (unsigned long) NULL,

        /* no matter who makes it */
        vendor:         PCI_ANY_ID,
        device:         PCI_ANY_ID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#ifdef CONFIG_OHCI
{
        class:          OHCI_CLASS,
        class_mask:     ~0,
        driver_data:    (unsigned long) NULL,

        /* no matter who makes it */
        vendor:         PCI_ANY_ID,
        device:         PCI_ANY_ID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#ifdef CONFIG_UHCI
{
        class:          UHCI_CLASS,
        class_mask:     ~0,
        driver_data:    (unsigned long) NULL,

        /* no matter who makes it */
        vendor:         PCI_ANY_ID,
        device:         PCI_ANY_ID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#ifdef CONFIG_ISP116X
{
        device:         ISP116X_DID,
        vendor:         ISP116X_VID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#ifdef CONFIG_ISP1362
{
        device:         ISP1362_DID,
        vendor:         ISP1362_VID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#ifdef CONFIG_ISP176x
{
        device:         ISP176x_DID,
        vendor:         ISP176x_VID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
#if defined(CONFIG_TD243FC) || defined(CONFIG_TD243HC)
{
        device:         TD243_DID,
        vendor:         TD243_VID,
        subvendor:      PCI_ANY_ID,
        subdevice:      PCI_ANY_ID,

},
#endif
{ /* end: all zeroes */ }
};

MODULE_DEVICE_TABLE (pci, pci_ids);

static jresult_t linux_device_fill_func(os_bus_h bus_h, struct pci_dev *pcidev)
{
    struct os_bus *bus = (struct os_bus *)bus_h;
    struct os_bus_resource **p_res = &bus->resources;
    jint_t bar;
    jint_t mem_res_id = 0, io_res_id = 0;

    for (bar = 0; bar < DEVICE_COUNT_RESOURCE; bar++)
    {
        if (!pci_resource_start(pcidev, bar))
        {
            continue;
        }

        *p_res = jos_malloc(sizeof(struct os_bus_resource), M_ZERO);
        if (!*p_res)
            return JENOMEM;
        (*p_res)->r_start = pci_resource_start(pcidev, bar);
        (*p_res)->r_len = pci_resource_len(pcidev, bar);
        (*p_res)->r_bus = bus;
        (*p_res)->r_type = (pci_resource_flags(pcidev, bar) & IORESOURCE_MEM)?
                SYS_RES_MEMORY : SYS_RES_IOPORT;
        (*p_res)->r_rid = (*p_res)->r_type == SYS_RES_MEMORY ? mem_res_id++ :
            io_res_id++;

        DBG_V(DPORT_BUS, ("linux_device_fill_func : found res (bar) 0x%x "
            "type %d id %d\n", bar, (*p_res)->r_type, (*p_res)->r_rid));
        p_res = &(*p_res)->next;
    }

    // interrupt resource
    *p_res = jos_malloc(sizeof(struct os_bus_resource), M_ZERO);
    if (!*p_res)
        return JENOMEM;


    (*p_res)->r_start = pcidev->irq;
    (*p_res)->r_bus = bus;
    (*p_res)->r_type = SYS_RES_IRQ;
    (*p_res)->next = NULL;

    return 0;
}

static jresult_t linux_pci_probe(struct pci_dev *pci_dev,
    const struct pci_device_id *id)
{
    struct os_bus *bus;
    jresult_t rc;

    bus = (struct os_bus *)jos_malloc(sizeof(struct os_bus), M_ZERO);
    if (!bus)
        return JENOMEM;

    pci_set_drvdata(pci_dev, (void *)bus);
    bus->os_device = pci_dev;
    
    rc = linux_device_fill_func(bus, pci_dev);
    if (rc)
    {
        DBG_E(DPORT_BUS, ("BUS: Error allocating resources %d\n", rc));
        return rc;
    }

#ifdef CONFIG_UHCI
    if (id->class == UHCI_CLASS)
    {
        rc = add_generic_controller(bus, controller_type_uhci_pci, 
            &(bus->device_handle));
    }
#endif
#ifdef CONFIG_OHCI
    if (id->class == OHCI_CLASS)
    {
        rc = add_generic_controller(bus, controller_type_ohci_pci, 
            &(bus->device_handle));
    }
#endif
#ifdef CONFIG_EHCI
    if (id->class == EHCI_CLASS)
    {
        rc = add_generic_controller(bus, controller_type_ehci_pci, 
            &(bus->device_handle));
    }
#endif
#ifdef CONFIG_ISP116X
    if (id->vendor == ISP116X_VID && id->device == ISP116X_DID)
    {
        rc = add_generic_controller(bus, controller_type_isp116x_pci, 
            &(bus->device_handle));
    }
#endif
#ifdef CONFIG_ISP1362
    if (id->vendor == ISP1362_VID && id->device == ISP1362_DID)
    {
        rc = add_generic_controller(bus, controller_type_isp1362, 
            &(bus->device_handle));
    }
#endif
#ifdef CONFIG_ISP176x
    if (id->vendor == ISP176x_VID && id->device == ISP176x_DID)
    {
        DBG_X(DPORT_BUS, ("linux_pci_probe: isp 1761 attached\n"));
#ifdef CONFIG_JSLAVE
        DBG_V(DPORT_BUS, ("linux_pci_probe: adding device controller\n"));
        rc = add_generic_controller(bus, controller_type_isp_1582, 
            &(bus->device_handle));
#endif
#ifdef CONFIG_JHOST
        if (!rc)
        {
            DBG_V(DPORT_BUS, ("linux_pci_probe: adding host controller\n"));
            rc = add_generic_controller(bus, controller_type_isp176x, 
                &(bus->device_handle));
        }
#endif
    }
#endif
#if defined(CONFIG_TD243FC) || defined(CONFIG_TD243HC)
    if (id->vendor == TD243_VID && id->device == TD243_DID)
    {
#ifdef CONFIG_TD243FC
        DBG_V(DPORT_BUS, ("linux_pci_probe: adding td243 device controller\n"));
        rc = add_generic_controller(bus, controller_type_td243fc, 
            &(bus->device_handle));
#endif
#ifdef CONFIG_TD243HC
        if (!rc)
        {
            DBG_V(DPORT_BUS, ("linux_pci_probe: adding td243 host "
                "controller\n"));
            rc = add_generic_controller(bus, controller_type_td243hc, 
                &(bus->device_handle));
        }
#endif
    }
#endif
    if (rc)
        DBG_E(DPORT_BUS, ("linux_pci_probe: Error adding controller %d\n", rc));

    return rc;
}

static void device_free_func(os_bus_resource_h res_h)
{
    struct os_bus_resource *res = (struct os_bus_resource *)res_h;
    struct os_bus_resource *next, *curr;
    
    for (curr = res; curr; curr = next)
    {
        next = curr->next;
        jos_free(curr);
    }
}

static void linux_pci_remove(struct pci_dev *dev)
{
    struct os_bus *bus = (struct os_bus *)pci_get_drvdata(dev);
    
    if (bus->device_handle)
        del_generic_controller(bus->device_handle);

    if (bus->resources)
        device_free_func(bus->resources);

    jos_free(bus);
}

/* pci driver glue; this is a "new style" PCI driver module */
static struct pci_driver linux_pci_driver = {
        name:           "pci_driver",
        id_table:       pci_ids,

        probe:          linux_pci_probe,
        remove:         linux_pci_remove,
};

static jbool_t pci_init_done = 0;
#endif

#ifdef CONFIG_NETCHIP
static void *device_handle;
#endif

jresult_t os_hw_init()
{
    jresult_t rc;
#ifdef CONFIG_BUS_PCI
    rc = pci_module_init(&linux_pci_driver);

    if (!rc)
        pci_init_done = 1;
#endif

#ifdef CONFIG_NETCHIP
    /* XXX: Since slave currently does not rely on this we have to hack it */
    if (rc == -JENODEV)
        rc = 0;

    add_generic_controller(NULL, controller_type_netchip,
        &device_handle);
#endif

    return rc;
}

void os_hw_uninit()
{
#ifdef CONFIG_NETCHIP
    if (device_handle)
        del_generic_controller(device_handle);
#endif

#ifdef CONFIG_BUS_PCI
    if (pci_init_done)
        pci_unregister_driver(&linux_pci_driver);
    pci_init_done = 0;
#endif
}

os_bus_resource_h os_bus_alloc_resource(os_bus_h bus_h, jint_t type, jint_t id)
{
    struct os_bus_resource *r = NULL;
    struct os_bus *bus = (struct os_bus*)bus_h;

    // find the right BAR resource
    OS_ASSERT(bus, ("bus is NULL"));
    for (r = bus->resources; r; r = r->next)
    {
        if (r->r_type == type && r->r_rid == id)
            break;
    }
    if (!r)
        return NULL;

    switch (r->r_type) 
    {
    case SYS_RES_IRQ:
        break;
    case SYS_RES_IOPORT:
        if (!request_region(r->r_start, r->r_len, "io")) 
            return NULL;
        
        r->r_virtual = (void *)r->r_start;
        break;

    case SYS_RES_MEMORY:
        if (!request_mem_region(r->r_start, r->r_len, "mem")) 
        {
            os_printf("controller already in use\n");
            return NULL;
        }
        r->r_virtual = ioremap_nocache(r->r_start, r->r_len);
        if (r->r_virtual == NULL) 
        {
            os_printf("error mapping memory\n");
            release_mem_region(r->r_start, r->r_len);
            return NULL;
        }
        break;
    default:
        return NULL;
    }
    return r;
}

void os_bus_release_resource(os_bus_h bus_h, os_bus_resource_h res_h)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    switch (res->r_type) 
    {
    case SYS_RES_IRQ:
        break;
    case SYS_RES_IOPORT:
        release_region(res->r_start, res->r_len);
        break;

    case SYS_RES_MEMORY:
        iounmap(res->r_virtual);
        release_mem_region(res->r_start, res->r_len);
        break;
    default:
        break;
    }

    res->r_virtual = 0;
}

#define JADDR(type, start, off) \
    ((__VOLATILE__ type *)(start) + (off)/sizeof(type))
#define JADDRS(type) JADDR(type, res->r_virtual, offset)

juint8_t os_bus_space_read_1(os_bus_resource_h res_h, juint32_t offset)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    if (res->r_type == SYS_RES_MEMORY)
        return *JADDRS(juint8_t);
    
    return inb((int)JADDRS(juint8_t));
}

juint16_t os_bus_space_read_2(os_bus_resource_h res_h, juint32_t offset)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    if (res->r_type == SYS_RES_MEMORY)
        return *JADDRS(juint16_t);
    
    return inw((int)JADDRS(juint16_t));
}

juint32_t os_bus_space_read_4(os_bus_resource_h res_h, juint32_t offset)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    if (res->r_type == SYS_RES_MEMORY)
        return *JADDRS(juint32_t);
    
    return inl((int)JADDRS(juint32_t));
}

void os_bus_space_write_1(os_bus_resource_h res_h, juint32_t offset, 
    juint8_t value)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    OS_ASSERT(offset % sizeof(juint8_t)==0, ("allignment error"));
    if (res->r_type == SYS_RES_MEMORY)
        *JADDRS(juint8_t) = value;
    else
        outb(value, (int)JADDRS(juint8_t));
}

void os_bus_space_write_2(os_bus_resource_h res_h, juint32_t offset, 
    juint16_t value)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    OS_ASSERT(offset % sizeof(juint16_t)==0, ("allignment error"));
    if (res->r_type == SYS_RES_MEMORY)
        *JADDRS(juint16_t) = value;
    else
        outw(value, (int)JADDRS(juint16_t));
}

void os_bus_space_write_4(os_bus_resource_h res_h, juint32_t offset, 
    juint32_t value)
{
    struct os_bus_resource *res = (struct os_bus_resource*)res_h;

    OS_ASSERT(res, ("Resource is NULL\n"));
    OS_ASSERT(offset % sizeof(juint32_t)==0, ("allignment error"));
    if (res->r_type == SYS_RES_MEMORY)
        *JADDRS(juint32_t) = value;
    else
        outl(value, (int)JADDRS(juint32_t));
}

#ifdef CONFIG_PORT_INTERCHIP
jbool_t os_power_request(juint8_t voltage_class, juint8_t max_current, 
    juint8_t *set_voltage_class, juint8_t *set_max_current)
{
    *set_voltage_class = voltage_class;
    *set_max_current = max_current;

    return 1;
}

void os_get_port_info(os_bus_h bus, juint8_t port, port_info_t *info)
{
    info->is_interchip = 0;
}
#endif


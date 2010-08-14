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

/* *  Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com          */
#include <jos.h>

#ifdef CONFIG_JSLAVE
#include <transceiver_device.h>
#endif
#ifdef CONFIG_JHOST
#include <transceiver_host.h>
#endif
#ifdef CONFIG_JOTG
#include <transceiver_otg.h>
#endif

#include <transceiver_tcd.h>
#include <lists.h>

#define TRCV(x) ((trcv_ctx_t *)(x))

#ifdef CONFIG_JHOST
typedef struct tcd_host_wrap {
    tcd_ops_t ops;
    jdevice_t dev;
    jbus_resource_h res;
} tcd_host_wrap_t;
#endif

typedef struct tcd_wrap {
    tcd_ops_t ops;
    jdevice_t dev;
} tcd_wrap_t;

typedef struct trcv_ctx {
    tcd_wrap_t tcd;

#ifdef CONFIG_JOTG
    TAILQ_ENTRY(trcv_ctx) next;
    jdevice_t otg;
    jbus_resource_h otg_res;
#   define OTG_EXIST(trcv_core_ctx) ((trcv_core_ctx)->otg)
#else
#   define OTG_EXIST(trcv_core_ctx) (0)
#endif

#ifdef CONFIG_JSLAVE
    jdevice_t device;
    jbus_resource_h device_res;
#   define DEVICE_EXIST(trcv_core_ctx) ((trcv_core_ctx)->device)
#else
#   define DEVICE_EXIST(trcv_core_ctx) (0)
#endif

#ifdef CONFIG_JHOST
    jdevice_t host;
    jint_t port_max_num;
    jint_t otg_port;
    tcd_host_wrap_t *host_tcd;
#   define HOST_EXIST(trcv_core_ctx) ((trcv_core_ctx)->host)
#else
#   define HOST_EXIST(trcv_core_ctx) (0)
#endif
} trcv_ctx_t;

#ifdef CONFIG_JOTG
static TAILQ_HEAD(m_trcv_list, trcv_ctx) trcv_list = 
TAILQ_HEAD_INITIALIZER(trcv_list);

static trcv_ctx_t *find_trcv(jbus_h bus)
{
    trcv_ctx_t *t;

    TAILQ_FOREACH(t, &trcv_list, next)
    {
        if (j_device_get_bus(t->tcd.dev) == bus)
            return t;
    }
    return NULL;
}

void update_trcv(trcv_ctx_t *t)
{
    if (t->host && t->device && t->otg)
        TAILQ_REMOVE(&trcv_list, t, next);                        
}
#else

#define find_trcv(x) NULL
#define update_trcv(x) do {} while (0)

#endif

#ifdef CONFIG_JSLAVE
static jresult_t trcv_add_tcd(trcv_ctx_t *trcv_core_ctx, jdevice_t dev, 
    jbus_h bus)
{
    jresult_t rc;

    rc = add_generic_controller_ex(bus, j_bus_get_controller_id(bus), 
        (void **)&trcv_core_ctx->tcd.dev, NULL);
    if (rc)
        return rc;

    rc = j_device_ioctl(trcv_core_ctx->tcd.dev, DRV_IOCTL_TCD_SET_CTX, 
        trcv_core_ctx);
    if (rc)
        goto Error;

    rc = j_device_ioctl(trcv_core_ctx->tcd.dev, DRV_IOCTL_TCD_GET_OPS, 
        &trcv_core_ctx->tcd.ops);
    if (rc)
        goto Error;

    return 0;

Error:
    del_generic_controller_ex(trcv_core_ctx->tcd.dev);
    return rc;
}
#endif

#ifdef CONFIG_JOTG
jresult_t trcv_otg_init(jdevice_t otg, trcv_ctx_h *trcv_core_ctx)
{
    trcv_ctx_t *t;
    jresult_t rc;
    jbus_h bus;
    jbus_resource_h res;
    DECLARE_FNAME("trcv_otg_init");
    
    *trcv_core_ctx = NULL;
    
    DBG_I(DTRCV_CORE, ("TRCV: %s otg=%p\n", fname, otg));

    res = j_bus_alloc_resource(j_device_get_bus(otg), SYS_RES_TRANSCEIVER, 1);
    if (!res)
        return 0;

    bus = j_bus_get_transceiver(res);
    KASSERT(bus, ("TRCV: %s - NULL bus handle\n", fname));

    DBG_V(DTRCV_CORE, ("TRCV: %s found transceiver\n", fname));

    t = (trcv_ctx_t *)jmalloc(sizeof(trcv_ctx_t), M_ZERO);
    if (!t)
    {
        DBG_E(DTRCV_CORE, ("TRCV: %s failed - no memory\n", fname));
        rc = JENOMEM;
        goto Error1;
    }

    rc = trcv_add_tcd(t, otg, bus);
    if (rc)
        goto Error2;    

    t->otg_res = res;
    t->otg = otg;

    *trcv_core_ctx = t;

    TAILQ_INSERT_TAIL(&trcv_list, t, next);

    return 0;

Error2:
    jfree(t);
Error1:
    jbus_release_resource(bus, res);
    return rc;
}

void trcv_otg_uninit(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    DECLARE_FNAME("trcv_otg_uninit");

    DBG_I(DTRCV_CORE, ("TRCV: %s core_ctx=%p\n", fname, trcv_core_ctx));

    if (!t)
        return;

    del_generic_controller_ex(t->tcd.dev);
    jbus_release_resource(j_device_get_bus(t->otg), t->otg_res);
    jfree(t);
}

jresult_t trcv_enable(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_enable)
        return 0;

    return t->tcd.ops.tcd_enable(j_device_get_softc(t->tcd.dev), TRCV_TYPE_OTG);
}

void trcv_disable(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_disable)
        return;
    
    t->tcd.ops.tcd_disable(j_device_get_softc(t->tcd.dev));
}

jresult_t trcv_srp_detection_enable(trcv_ctx_h trcv_core_ctx, jbool_t enable)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    
    if (!t || !t->tcd.ops.tcd_srp_detection_enable)
        return 0;
    
    return t->tcd.ops.tcd_srp_detection_enable(j_device_get_softc(t->tcd.dev),
        enable);
}

jresult_t trcv_vbus_drive(trcv_ctx_h trcv_core_ctx, jbool_t drive)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_vbus_drive)
        return 0;
    
    return t->tcd.ops.tcd_vbus_drive(j_device_get_softc(t->tcd.dev), drive);
}

jresult_t trcv_vbus_charge(trcv_ctx_h trcv_core_ctx, jbool_t charge)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_vbus_charge)
        return 0;
    
    return t->tcd.ops.tcd_vbus_charge(j_device_get_softc(t->tcd.dev), charge);
}

jresult_t trcv_srp_initiate(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_srp_initiate)
        return 0;
    
    return t->tcd.ops.tcd_srp_initiate(j_device_get_softc(t->tcd.dev));
}

jresult_t trcv_state_get(trcv_ctx_h trcv_core_ctx, otg_ctrl_t *otg_ctrl)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_state_get)
        return 0;
    
    return t->tcd.ops.tcd_state_get(j_device_get_softc(t->tcd.dev), otg_ctrl);
}

jresult_t trcv_local_dplus_set(trcv_ctx_h trcv_core_ctx, jbool_t enable)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_local_dplus_set)
        return 0;
    
    return t->tcd.ops.tcd_local_dplus_set(j_device_get_softc(t->tcd.dev),
        enable);
}

jresult_t trcv_local_dminus_set(trcv_ctx_h trcv_core_ctx, jbool_t enable)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_local_dminus_set)
        return 0;
    
    return t->tcd.ops.tcd_local_dminus_set(j_device_get_softc(t->tcd.dev),
        enable);
}

void trcv_otg_intr_handle(trcv_ctx_h trcv_core_ctx)
{
    jresult_t rc;
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    rc = j_device_ioctl(t->otg, DRV_IOCTL_OTG_INTR, NULL);
    if (rc)
    {
        DBG_E(DTRCV_CORE, ("TRCV: trcv_jotg_intr_handle - OTG_INTR ioctl "
            "failed rc=%d\n", rc));
    }
}
#endif

void trcv_mode_set(trcv_ctx_h trcv_core_ctx, jbool_t mode)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_mode_set)
        return;

    t->tcd.ops.tcd_mode_set(j_device_get_softc(t->tcd.dev), mode);
}

void trcv_interface_type_get(trcv_ctx_h trcv_core_ctx, 
    trcv_interface_type_t *type)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_interface_type_get)
        return;

    t->tcd.ops.tcd_interface_type_get(j_device_get_softc(t->tcd.dev), type);
}

#ifdef CONFIG_JHOST
static void release_host_tcd(jbus_h bus, tcd_host_wrap_t *tcd, 
    jint_t port_max_num, jint_t otg_port)
{
    jint_t i;

    DBG_I(DTRCV_CORE, ("Releasing Host with %d ports (otg port is %d)\n",
        port_max_num, otg_port));

    for (i = 0; i < port_max_num; i++)
    {
        if (!tcd[i].res)
            continue;

        jbus_release_resource(bus, tcd[i].res);

        if ((i+1) == otg_port)
            continue;

        if (tcd[i].ops.tcd_disable)
            tcd[i].ops.tcd_disable(j_device_get_softc(tcd[i].dev));

        if (tcd[i].dev)
            del_generic_controller_ex(tcd[i].dev);
    }
    jfree(tcd);
}

jresult_t trcv_host_init(jdevice_t host, jint_t port_max_num, 
    trcv_ctx_h *trcv_core_ctx)
{
    jresult_t rc;
    trcv_ctx_t *t = NULL;
    tcd_host_wrap_t *tcd;
    jint_t i, otg_port = 0;
    DECLARE_FNAME("trcv_host_init");

    DBG_I(DTRCV_CORE, ("TRCV: %s host=%p port_max_num=%d\n", fname, host,
        port_max_num));

    *trcv_core_ctx = NULL;

    tcd = (tcd_host_wrap_t *)jmalloc(sizeof(tcd_host_wrap_t) * port_max_num, 
        M_ZERO);    

    for (i = 0; i < port_max_num; i++)
    {
        jbus_h bus;

        tcd[i].res = j_bus_alloc_resource(j_device_get_bus(host), 
            SYS_RES_TRANSCEIVER, i+1);
        if (!tcd[i].res)
            continue;
        
        DBG_I(DTRCV_CORE, ("TRCV: %s found transceiver connected to port %d\n",
            fname, i+1));

        bus = j_bus_get_transceiver(tcd[i].res);
        KASSERT(bus, ("TRCV: %s - NULL bus handle\n", fname));

        if (!t)
            t = find_trcv(bus);

        /*TODO Check if verifying otg_port is needed */
        if (t && !otg_port)
        {
            otg_port = i + 1;
            tcd[i].dev = t->tcd.dev;
            tcd[i].ops = t->tcd.ops;

            DBG_V(DTRCV_CORE, ("TRCV: %s transceiver %p on port %d is otg\n",
                fname, tcd[i].dev, i+1));
        }
        else
        {
            rc = add_generic_controller_ex(bus, j_bus_get_controller_id(bus), 
                (void **)&tcd[i].dev, NULL);
            if (rc)        
                goto Error1;
            
            DBG_V(DTRCV_CORE, ("TRCV: %s - added transceiver %p on port %d\n",
                fname, tcd[i].dev, i+1));
        }
    }
    
    if (!t)
    {
        t = (trcv_ctx_t *)jmalloc(sizeof(trcv_ctx_t), M_ZERO);
        if (!t)
        {
            DBG_E(DTRCV_CORE, ("TRCV: %s failed - no memory\n", fname));
            rc = JENOMEM;
            goto Error1;
        }
    }
    
    for (i = 0; i < port_max_num; i++)
    {
        if ((i+1) == otg_port || !tcd[i].dev)
            continue;

        rc = j_device_ioctl(tcd[i].dev, DRV_IOCTL_TCD_SET_CTX, t);
        if (rc)
            goto Error2;

        rc = j_device_ioctl(tcd[i].dev, DRV_IOCTL_TCD_GET_OPS, &tcd[i].ops);
        if (rc)
            goto Error2;

        if (tcd[i].ops.tcd_enable)
        {
            rc = tcd[i].ops.tcd_enable(j_device_get_softc(tcd[i].dev), 
                TRCV_TYPE_HOST);
            if (rc)
                goto Error2;
        }
    }

    t->host_tcd = tcd;
    t->otg_port = otg_port;
    t->port_max_num = port_max_num;
    t->host = host;

    *trcv_core_ctx = t;

    update_trcv(t);

    return 0;

Error2:
    DBG_E(DTRCV_CORE, ("TRCV: %s - transceivers init failed, rc=%d\n", fname,
        rc));

    if (!OTG_EXIST(t))
        jfree(t);
    
Error1:
    release_host_tcd(j_device_get_bus(host), tcd, port_max_num, otg_port);

    return rc;
}


void trcv_host_uninit(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    DECLARE_FNAME("trcv_host_uninit");

    DBG_I(DTRCV_CORE, ("TRCV: %s core_ctx=%p\n", fname, trcv_core_ctx));

    if (!t)
        return;

    release_host_tcd(j_device_get_bus(t->host), t->host_tcd, t->port_max_num,
        t->otg_port);

    if (!OTG_EXIST(t) && !DEVICE_EXIST(t))
        jfree(t);
}

void trcv_host_port_set(trcv_ctx_h trcv_core_ctx, juint8_t index, jint_t value, 
    jbool_t set)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t)
        return;

    KASSERT(t->host_tcd, ("TRCV: Null host transceiver list\n"));
    
    if (!t->host_tcd[index-1].dev || 
        !t->host_tcd[index-1].ops.tcd_host_port_set)
        return;

    t->host_tcd[index-1].ops.tcd_host_port_set(
        j_device_get_softc(t->host_tcd[index-1].dev), value, set);
}

void trcv_host_port_status_get(trcv_ctx_h trcv_core_ctx, juint8_t index, 
    juint16_t *status)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    juint16_t tcd_status, mask;

    if (!t)
        return;

    KASSERT(t->host_tcd, ("TRCV: Null host transceiver list\n"));
    
    if (!t->host_tcd[index-1].dev || 
        !t->host_tcd[index-1].ops.tcd_host_port_status_get)
        return;

    t->host_tcd[index-1].ops.tcd_host_port_status_get(
        j_device_get_softc(t->host_tcd[index-1].dev), &tcd_status, &mask);

    *status &= ~mask;
    *status |= tcd_status;
}

void trcv_host_port_change_get(trcv_ctx_h trcv_core_ctx, juint8_t index,
    juint16_t *change)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    juint16_t tcd_change, mask;

    if (!t)
        return;

    KASSERT(t->host_tcd, ("TRCV: Null host transceiver list\n"));
    
    if (!t->host_tcd[index-1].dev || 
        !t->host_tcd[index-1].ops.tcd_host_port_change_get)
        return;

    t->host_tcd[index-1].ops.tcd_host_port_change_get(
        j_device_get_softc(t->host_tcd[index-1].dev), &tcd_change, &mask);

    *change &= ~mask;
    *change |= tcd_change;
}

void trcv_host_intr_handle(trcv_ctx_h trcv_core_ctx)
{
    jresult_t rc;
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    rc = j_device_ioctl(t->host, DRV_IOCTL_HOST_PORT_CHANGE, NULL);
    if (rc)
    {
        DBG_E(DTRCV_CORE, ("TRCV: trcv_host_intr_handle - HOST_PORT_CHANGE "
            "ioctl failed rc=%d\n", rc));
    }
}
#endif

#ifdef CONFIG_JSLAVE
jresult_t trcv_device_init(jdevice_t device, trcv_ctx_h *trcv_core_ctx)
{
    jresult_t rc;
    trcv_ctx_t *t;
    jbus_h bus;
    jbus_resource_h res;
    DECLARE_FNAME("trcv_device_init");

    *trcv_core_ctx = NULL;

    res = j_bus_alloc_resource(j_device_get_bus(device), SYS_RES_TRANSCEIVER,
        1);
    if (!res)
        return 0;

    bus = j_bus_get_transceiver(res);
    KASSERT(bus, ("TRCV: %s - NULL bus handle\n", fname));

    DBG_V(DTRCV_CORE, ("TRCV: %s found transceiver\n", fname));

    t = find_trcv(bus);
    if (!t)
    {
        t = (trcv_ctx_t *)jmalloc(sizeof(trcv_ctx_t), M_ZERO);
        if (!t)
        {
            DBG_E(DTRCV_CORE, ("TRCV: %s failed - no memory\n", fname));
            rc = JENOMEM;
            goto Error1;
        }
        rc = trcv_add_tcd(t, device, bus);
        if (rc)
            goto Error2;
        
        if (t->tcd.ops.tcd_enable)
        {
            rc = t->tcd.ops.tcd_enable(j_device_get_softc(t->tcd.dev), 
                TRCV_TYPE_DEVICE);
            if (rc)
                goto Error3;
        }
    }
#ifdef JDEBUG
    else
    {
        DBG_V(DTRCV_CORE, ("TRCV: %s -transceiver %p is otg\n", fname, t->tcd));
    }
#endif

    t->device = device;
    t->device_res = res;

    *trcv_core_ctx = t;

    update_trcv(t);

    return 0;

Error3:
    DBG_E(DTRCV_CORE, ("TRCD: %s - transceiver enable failed rc=%d\n", fname, 
        rc));
    del_generic_controller_ex(t->tcd.dev);
Error2:
    DBG_E(DTRCV_CORE, ("TRCV: %s - transceiver init failed, rc=%d\n", fname,
        rc));
    jfree(t);
Error1:
    jbus_release_resource(bus, res);
    return rc;
}

void trcv_device_uninit(trcv_ctx_h trcv_core_ctx)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);
    DECLARE_FNAME("trcv_device_uninit");

    DBG_I(DTRCV_CORE, ("TRCV: %s core_ctx=%p\n", fname, trcv_core_ctx));

    if (!t)
        return;

    if (!HOST_EXIST(t) && !OTG_EXIST(t))
    {
        if (t->tcd.ops.tcd_disable)
            t->tcd.ops.tcd_disable(j_device_get_softc(t->tcd.dev));
        del_generic_controller_ex(t->tcd.dev);
    }

    jbus_release_resource(j_device_get_bus(t->device), t->device_res);
    
    if (!HOST_EXIST(t) && !OTG_EXIST(t))
        jfree(t);
}

void trcv_device_enable(trcv_ctx_h trcv_core_ctx, jbool_t enable)
{
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    if (!t || !t->tcd.ops.tcd_device_enable)
        return;
    
    t->tcd.ops.tcd_device_enable(j_device_get_softc(t->tcd.dev), enable);
}

void trcv_device_intr_handle(trcv_ctx_h trcv_core_ctx, juint16_t status)
{
    jresult_t rc;
    trcv_ctx_t *t = TRCV(trcv_core_ctx);

    rc = j_device_ioctl(t->device, DRV_IOCTL_DEVICE_INTR, 
        (void *)((juint32_t)status));
    if (rc)
    {
        DBG_E(DTRCV_CORE, ("TRCV: trcv_device_intr_handle - DEVICE_INTR ioctl "
            "failed rc=%d\n", rc));
    }
}
#endif


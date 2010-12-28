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

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

#include <td243_rev2_reg.h>
#include <td243_rev2_var.h>

#include <root_hub.h>
#include "xfrag.h"

#define NOT_IMPLEMENTED(fname) jprintf(fname ": Not implemented yet\n");

#define TOGGLE_IF_SET(reg, val) \
    if (READ4(reg) & val) \
        WRITE4(reg, val)

/* --- BUS Methods --- */
static usbd_status td243hc_rev2_open(usbd_pipe_handle pipe);
static usbd_status td243hc_rev2_allocm(struct usbd_bus *bus, jdma_handle *dma,
    juint32_t size);
static void td243hc_rev2_freem(struct usbd_bus *bus, jdma_handle *dma);
static usbd_xfer_handle td243hc_rev2_allocx(struct usbd_bus *bus);
static void td243hc_rev2_freex(struct usbd_bus *bus, usbd_xfer_handle xfer);
static jbool_t td243hc_rev2_is_active(struct usbd_bus *bus);
static jresult_t td243hc_rev2_suspend(usbd_bus_handle bus);
static jresult_t td243hc_rev2_resume(usbd_bus_handle bus);
#ifdef CONFIG_JOTG
static jbool_t td243hc_rev2_is_otg_host_enabled(struct usbd_bus *bus);
#endif

/* --- Pipe Methods --- */
static usbd_status td243hc_rev2_device_transfer(usbd_xfer_handle xfer);
static usbd_status td243hc_rev2_device_start(usbd_xfer_handle xfer);
static void td243hc_rev2_device_abort(usbd_xfer_handle);
static void td243hc_rev2_device_close(usbd_pipe_handle pipe);
static void td243hc_rev2_device_done(usbd_xfer_handle xfer);

static usbd_status td243hc_rev2_device_isoc_transfer(usbd_xfer_handle xfer);
static usbd_status td243hc_rev2_device_isoc_start(usbd_xfer_handle xfer);
static void td243hc_rev2_device_isoc_abort(usbd_xfer_handle);
static void td243hc_rev2_device_isoc_close(usbd_pipe_handle pipe);
static void td243hc_rev2_device_isoc_done(usbd_xfer_handle xfer);

static void td243hc_rev2_device_clear_toggle(usbd_pipe_handle pipe);
static void td243hc_rev2_noop(usbd_pipe_handle pipe);
static void td243hc_rev2_abort_xfer(usbd_xfer_handle xfer, usbd_status status);

/* --- Root hub Methods --- */
static usbd_status td243hc_rev2_do_port_change(void *context, juint8_t index, 
    jint_t value, jbool_t set);
static juint16_t td243hc_rev2_get_port_status(void *context, juint8_t index);
static juint16_t td243hc_rev2_get_port_change(void *context, juint8_t index);
static jint_t td243hc_rev2_port_interrupt_set(void *context, jbool_t enable);

/* --- Internal Functions --- */
static jresult_t td243hc_rev2_enable(td243hc_rev2_softc_t *sc);
static jresult_t td243hc_rev2_reset(td243hc_rev2_softc_t *sc);
static void td243hc_rev2_init_etd_table(td243hc_rev2_softc_t *sc);
static jint_t td243hc_rev2_alloc_etd(td243hc_rev2_softc_t *sc, 
    td243hc_rev2_xfer_t *xfer);
static void  td243hc_rev2_free_etd(td243hc_rev2_softc_t *sc, jint_t etd);

static struct usbd_bus_methods td243hc_rev2_bus_methods = {
    td243hc_rev2_open,
    td243hc_rev2_allocm,
    td243hc_rev2_freem,
    td243hc_rev2_allocx,
    td243hc_rev2_freex,
    td243hc_rev2_is_active,
    td243hc_rev2_suspend,
    td243hc_rev2_resume
#ifdef CONFIG_JOTG
    , td243hc_rev2_is_otg_host_enabled
#endif
};

static struct root_hub_methods td243hc_rev2_root_hub_methods = {
    td243hc_rev2_do_port_change,
    td243hc_rev2_get_port_status,
    td243hc_rev2_get_port_change,
    td243hc_rev2_port_interrupt_set
};

static struct usbd_pipe_methods td243hc_rev2_device_methods = {
    td243hc_rev2_device_transfer,
    td243hc_rev2_device_start,
    td243hc_rev2_device_abort,
    td243hc_rev2_device_close,
    td243hc_rev2_device_clear_toggle,
    td243hc_rev2_device_done,
};

static struct usbd_pipe_methods td243hc_rev2_device_isoc_methods = {
    td243hc_rev2_device_isoc_transfer,
    td243hc_rev2_device_isoc_start,
    td243hc_rev2_device_isoc_abort,
    td243hc_rev2_device_isoc_close,
    td243hc_rev2_noop,
    td243hc_rev2_device_isoc_done,
};

static void td243hc_rev2_init_etd_table(td243hc_rev2_softc_t *sc)
{
    jint_t i;

    j_memset(sc->etd_table, 0, sizeof(sc->etd_table));
    for (i = 0; i < TD243HC_R2_NUM_ETDS; i++)
    {
        sc->etd_table[i].xbuf_offset = 2 * TD243HC_R2_XY_SIZE * i;
        sc->etd_table[i].ybuf_offset = 2 * TD243HC_R2_XY_SIZE * i 
            + TD243HC_R2_XY_SIZE;

        sc->etd_table[i].list_head = NULL;
        sc->etd_table[i].current_active = NULL;
    }
}

static jint_t td243hc_rev2_alloc_etd(td243hc_rev2_softc_t *sc, 
    td243hc_rev2_xfer_t *xfer)
{
    jint_t i;

    for (i = 0; i < TD243HC_R2_NUM_ETDS; i++)
    {
        if (!sc->etd_table[i].xfer)
            break;
    }

    if (i != TD243HC_R2_NUM_ETDS)
        sc->etd_table[i].xfer = xfer;
#if JDEBUG
    else
        DBG_W(DHCD_TRANSFER, ("alloc_etd: no availavle pipes for transfer\n"));
#endif
    return i;
}

static void td243hc_rev2_free_etd(td243hc_rev2_softc_t *sc, jint_t etd)
{
    sc->etd_table[etd].xfer = NULL;
}

static void td243hc_rev2_mem_read(td243hc_rev2_softc_t *sc, juint32_t reg_offset,
    juint16_t size, juint8_t *buf)
{
    juint32_t data;

    while (size > 3)
    {
        data = READ4(reg_offset);
        j_memcpy(buf, &data, 4);
        buf += 4;
        size -= 4;
        reg_offset += 4;
    }

    if (size)
    {
        data = READ4(reg_offset);
        j_memcpy(buf, &data, size);
    }
}

static void td243hc_rev2_mem_write(td243hc_rev2_softc_t *sc, 
    juint32_t reg_offset, juint16_t size, juint8_t *buf)
{
    juint32_t data;

    while (size > 3)
    {
        j_memcpy(&data, buf, 4);
        WRITE4(reg_offset, data);
        buf += 4;
        size -= 4;
        reg_offset +=4;
    }

    if (size)
    {
        j_memcpy(&data, buf, size);
        WRITE4(reg_offset, data);
    }
}

static __INLINE__ void td243hc_rev2_etd_read(td243hc_rev2_softc_t *sc, 
    juint8_t etd_num, juint8_t *buf)
{
    DBG_V(DHCD_TRANSFER, ("HCD: etd_read etd=%d addr=0x%04x\n", etd_num,
        TD243HC_R2_ETD_ADDR(etd_num)));

    td243hc_rev2_mem_read(sc, TD243HC_R2_ETD_ADDR(etd_num), 
        sizeof(td243hc_rev2_etd_t), buf);
}

static __INLINE__ void td243hc_rev2_etd_write(td243hc_rev2_softc_t *sc, 
    juint8_t etd_num, juint8_t *buf)
{
    DBG_V(DHCD_TRANSFER, ("HCD: etd_write etd=%d addr=0x%04x\n", etd_num,
        TD243HC_R2_ETD_ADDR(etd_num)));
    td243hc_rev2_mem_write(sc, TD243HC_R2_ETD_ADDR(etd_num), 
        sizeof(td243hc_rev2_etd_t), buf);
}

jresult_t td243hc_rev2_hw_init(td243hc_rev2_softc_t *sc)
{
    root_hub_desc_t *root_desc;
    juint32_t val;
#if JDEBUG
    juint32_t hst_rev;
    juint32_t dma_rev;
#endif
    td243hc_rev2_etd_t etd;
    juint_t i;

    /* Start Clock*/
    WRITE4(TD243HC_R2_OTG_CLK_CTRL_REG, TD243HC_R2_HSTCLK);
#if JDEBUG
    hst_rev = BFGET4(TD243HC_R2_HSTREV, READ4(TD243HC_R2_HARDWARE_MODE_REG));
    dma_rev = BFGET4(TD243HC_R2_DMA_REVCODE, READ4(TD243HC_R2_DMA_REV_REG));
#endif
    DBG_V(DHCD_INIT, ("td243hc_rev2_hw_init: Host Rev 0x%x, DMA Rev 0x%x\n",
        hst_rev, dma_rev));

    td243hc_rev2_init_etd_table(sc);

    /* Set up the bus struct. */
    sc->sc_bus.methods = &td243hc_rev2_bus_methods;
    sc->sc_bus.pipe_size = sizeof(struct td243hc_rev2_pipe);
    sc->sc_bus.usbrev = USBREV_2_0;
    sc->sc_lock = NULL;

    jspinlock_init(&sc->sc_lock);

    /* Init Controller */
    /* Reset all Host Module registers */
    WRITE4(TD243HC_R2_OTG_RESET_CTRL_REG, 
        TD243HC_R2_RSTHC | TD243HC_R2_RSTHSIE);

    while (READ4(TD243HC_R2_OTG_RESET_CTRL_REG) & 
        (TD243HC_R2_RSTHC | TD243HC_R2_RSTHSIE))
    {
        jdelay_ms(1);
    }
    
    /* Set Host only mode */
    WRITE4(TD243HC_R2_HARDWARE_MODE_REG, 
        BFSET4(TD243HC_R2_CRECFG, TD243HC_R2_HOST_ONLY) | 
        BFSET4(TD243HC_R2_HOSTXCVR2, TD243HC_R2_XCVR_SS) | 
        BFSET4(TD243HC_R2_HOSTXCVR, TD243HC_R2_XCVR_SS) | 
        BFSET4(TD243HC_R2_OTGXCVR, TD243HC_R2_XCVR_SS)); 
    
    /* Reset the controller */
    td243hc_rev2_reset(sc);

    /* Init root hub descriptor */
    val = READ4(TD243HC_R2_ROOTHUB_DESC_A_REG);
    root_desc = &sc->sc_bus.root_desc;
    root_desc->noport   = BFGET4(TD243HC_R2_NDNSTMPRT, val);
    root_desc->speed    = USB_SPEED_FULL;
    root_desc->self_powered = 1;
    root_desc->remote_wakeup = 1;
    root_desc->pwr_on_2_pwr_good = BFGET4(TD243HC_R2_PWRTOGOOD, val);

    /* Init the root hub */
    root_hub_init(&(sc->sc_bus.rootsoftc), &td243hc_rev2_root_hub_methods,
        root_desc, sc); 

    j_memset(&etd, 0, sizeof(td243hc_rev2_etd_t));
    for (i = 0; i < TD243HC_R2_NUM_ETDS; i++)
        td243hc_rev2_etd_write(sc, i, (juint8_t*)&etd);  

    /* Enable for operation */
    td243hc_rev2_enable(sc);

    sc->sc_flags |= TD243HC_R2_SCFLG_DONEINIT;

    return 0;
}

static jresult_t td243hc_rev2_reset(td243hc_rev2_softc_t *sc)
{
    /* Reset controller and put it to the reset state */
    WRITE4(TD243HC_R2_CONTROL_REG, TD243HC_R2_HCRESET);

    /* When reset is complete the HC will automatically enter the
     * USBSuspend state */
    while (BFGET4(TD243HC_R2_HCUSBSTE ,READ4(TD243HC_R2_CONTROL_REG)) 
        != TD243HC_R2_USB_SUSPEND)
    {
        jdelay_ms(1);
    }

    DBG_V(DHCD_INIT, ("td243hc_rev2_reset: Done\n"));

    return 0;
}

static jresult_t td243hc_rev2_enable(td243hc_rev2_softc_t *sc)
{
    DBG_V(DHCD_INIT, ("td243hc_rev2_enable: Entered\n"));

    /* Disable and Clear all pending interrupts */
    WRITE4(TD243HC_R2_SYS_INTR_EN_REG, 0x0);
    WRITE4(TD243HC_R2_SYS_INTR_STAT_REG, 0x7f);

    /* Enable host controller interrupt */
    sc->eintrs = TD243HC_R2_SYS_INTR_RHSC |  /* Root Hub port change */
        TD243HC_R2_SYS_INTR_RDQ;          /* Read Back Done */
    WRITE4(TD243HC_R2_SYS_INTR_EN_REG, sc->eintrs);

    /* Enable the OTG Module interrupt */
    WRITE4(TD243HC_R2_OTG_INTR_EN_REG, TD243HC_R2_HCINT);

    /* Set controller to operational state */
    WRITE4(TD243HC_R2_CONTROL_REG, BFSET4(TD243HC_R2_HCUSBSTE, 
        TD243HC_R2_USB_OPER) | TD243HC_R2_RMTWUEN);

    sc->etd_to_proceed = 0;
    sc->add_to_list = FALSE;

    DBG_V(DHCD_INIT, ("td243hc_rev2_enable: Done\n"));
    return 0;
}

#ifdef JDEBUG
static juint32_t num_intr;
#endif

jresult_t td243hc_rev2_intr(void *v)
{
    jresult_t rc = ISR_CALL_SOFT_INTR;
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)v;
    __VOLATILE__ juint32_t intrs, eintrs, sys_intr;
    __VOLATILE__ juint32_t etd_done_stat;

    if (sc == NULL || sc->sc_dying || 
        !(sc->sc_flags & TD243HC_R2_SCFLG_DONEINIT))
    {
        rc = ISR_NOT_RECOGNIZED;
        goto Exit;
    }
    
    sys_intr = READ4(TD243HC_R2_OTG_INTR_STAT_REG);

    if (!(sys_intr & TD243HC_R2_HCINT))
    {
        rc = ISR_NOT_RECOGNIZED;
        goto Exit;
    }

    /* Read interrupt */
    intrs = READ4(TD243HC_R2_SYS_INTR_STAT_REG);
    eintrs = intrs & sc->eintrs;
    
    DBG_X(DHCD_INTR, ("td243hc_rev2_intr: sys_intr=0x%08x intrs=0x%08x "
        "eintrs=0x%08x num_intr=%d\n", sys_intr, intrs, eintrs, num_intr++));
    
    if (!eintrs)
    {
        rc = ISR_NOT_RECOGNIZED;
        goto Exit;
    }

    /* Save interrupt status for software-interrupt handler */
    sc->intrs |= eintrs;

    if (eintrs & TD243HC_R2_SYS_INTR_RDQ)
    {
        etd_done_stat = READ4(TD243HC_R2_ETD_DONE_STATUS_REG);
        etd_done_stat &= READ4(TD243HC_R2_ETD_DONE_ENA_REG);

        sc->done_queue |= etd_done_stat;
        
        /* Disable DONE interrupt */
        CLEAR4(TD243HC_R2_ETD_DONE_ENA_REG, etd_done_stat);

        /* Disable ETD */
        WRITE4(TD243HC_R2_ETD_ENA_CLR_REG, etd_done_stat);
    }

    /* Disable interrupts */
    CLEAR4(TD243HC_R2_SYS_INTR_EN_REG, eintrs);
    CLEAR4(TD243HC_R2_OTG_INTR_EN_REG, TD243HC_R2_HCINT);
    
Exit:
#ifdef JDEBUG
    if (num_intr > 100)
    {
        DBG_E(DHCD_INTR, ("td243hc_rev2_intr: DISABLE interrupts no soft called"
            "\n"));
        WRITE4(TD243HC_R2_OTG_INTR_EN_REG, 0);
    }
#endif

    return rc;
}

static jresult_t td243hc_rev2_transfer_frag(td243hc_rev2_softc_t *sc, 
    juint_t etd_num);

static void td243hc_rev2_handle_comp(td243hc_rev2_softc_t *sc, juint_t etd_num)
{
    td243hc_rev2_etd_table_t *etd_entry = &sc->etd_table[etd_num];
    td243hc_rev2_xfer_t *tdxfer = etd_entry->xfer;
    td243hc_rev2_pipe_t *tdpipe = (td243hc_rev2_pipe_t *)tdxfer->xfer.pipe;
    td243hc_rev2_etd_t etd_hw;
    juint32_t transfer_status;
    struct xfrag *cur_frag = etd_entry->current_active; 

    DBG_V(DHCD_TRANSFER, ("td243hc_rev2_handle_comp: etd_index %d\n", etd_num));

    /* Get ETD from HW */
    td243hc_rev2_etd_read(sc, etd_num, (juint8_t *)&etd_hw);

    /* Update pipe with relevant part of etd */
    tdpipe->epd = etd_hw.dw0;

    if (tdxfer->xfer.status != USBD_IN_PROGRESS)
    {
        /*The transfer is aborted or timed out
         * we have nothing to do here exit.*/
        return;
    }

    /* Get last transfer status */
    transfer_status = BFGET4(TD243HC_R2_DW2_COMPCODE, etd_hw.dw2);
    if (transfer_status)
    {
        tdxfer->xfer.status = (transfer_status == TD243HC_R2_STATUS_STALL) ? 
            USBD_STALLED : USBD_IOERROR;
        DBG_E(DHCD_TRANSFER, ("td243hc_rev2_handle_comp: etd_index %d "
            "transfer %c, status %d\n", etd_num, 
            transfer_status == TD243HC_R2_STATUS_STALL ? 'S' : 'F', 
            transfer_status));
    }

    /* count xfered data in actlen (do not count setup) */
    if (((cur_frag->flags & XFRAG_TOKEN_MASK) == XFRAG_DATA) &&
        (tdxfer->xfer.length))
    {
        juint32_t leftover = 0, xfrag_transfered = 0;
        
        leftover = BFGET4(TD243HC_R2_DW3_TOTBYECNT, etd_hw.dw3);
        xfrag_transfered = cur_frag->size - leftover;

        tdxfer->xfer.actlen += xfrag_transfered;

        if (leftover)
        {
            /* We did not send/recieved as much as expected 
             * skip to next frag */
            xfrag_skip_same(cur_frag);
        }
    }

    etd_entry->current_active = xfrag_next(cur_frag);

    if (tdxfer->xfer.status != USBD_IN_PROGRESS)
        goto xfer_done;

    if (etd_entry->current_active)
    {
        /* We need to continue */
        td243hc_rev2_transfer_frag(sc, etd_num);
        return;
    }

    tdxfer->xfer.status = USBD_NORMAL_COMPLETION;

xfer_done:

    if (jtask_pending(tdxfer->xfer.timeout_handle))
        jtask_stop(tdxfer->xfer.timeout_handle);

    usb_transfer_complete((usbd_xfer_handle)tdxfer);
}

void td243hc_rev2_softintr(void *v)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)v;
    juint32_t to_handle, etd_done, etd_ack;

    DBG_V(DHCD_INTR, ("td243hc_rev2_softintr: Entered\n"));
    
    jspinlock_lock(sc->sc_lock);
    {
        to_handle = sc->intrs;
        sc->intrs = 0;
        etd_ack = etd_done = sc->done_queue;
        sc->done_queue = 0;
#ifdef JDEBUG
        num_intr = 0;
#endif
    }
    jspinlock_unlock(sc->sc_lock);
    
    if (to_handle & TD243HC_R2_SYS_INTR_RD)
    {
        if (sc->sc_suspend)
            controller_usb_resume_self(sc->sc_bus.usb_wrap); 
    }

    if (to_handle & TD243HC_R2_SYS_INTR_RHSC)
    {
        if (sc->sc_suspend)
            controller_usb_resume_self(sc->sc_bus.usb_wrap); 

        /* This interrupt is enabled and disabled by root hub*/
        to_handle &= ~TD243HC_R2_SYS_INTR_RHSC;
        WRITE4(TD243HC_R2_SYS_INTR_STAT_REG, TD243HC_R2_SYS_INTR_RHSC);

        root_hub_port_change(&(sc->sc_bus.rootsoftc));
    }

    if (to_handle & TD243HC_R2_SYS_INTR_RDQ)
    {
        juint_t etd_index;
        DBG_X(DHCD_INTR, ("td243hc_rev2_softintr: etd_done=0x%04x\n", 
            etd_done));

        sc->add_to_list = TRUE;
        for (etd_index = 0; etd_done; etd_done >>= 1, etd_index++)
        {
            if (etd_done & 1)
                td243hc_rev2_handle_comp(sc, etd_index);
        }
        sc->add_to_list = FALSE;
        WRITE4(TD243HC_R2_ETD_DONE_STATUS_REG, etd_ack);
    }
    
    /* Acknowledge and enable back the interrupts */
    WRITE4(TD243HC_R2_SYS_INTR_STAT_REG, to_handle);
    WRITE4(TD243HC_R2_OTG_INTR_STAT_REG, TD243HC_R2_HCINT);

    SET4(TD243HC_R2_SYS_INTR_EN_REG, to_handle);
    SET4(TD243HC_R2_OTG_INTR_EN_REG, TD243HC_R2_HCINT);

    /* Enable pending transfers */
    if (sc->etd_to_proceed)
    {
        WRITE4(TD243HC_R2_ETD_ENA_REG, sc->etd_to_proceed);
        sc->etd_to_proceed = 0;
    }
}

void td243hc_rev2_hw_uninit(struct td243hc_rev2_softc *sc, 
    jint_t flags)
{
    sc->sc_flags &= ~TD243HC_R2_SCFLG_DONEINIT;
    sc->sc_dying = 1;

    root_hub_uninit(&(sc->sc_bus.rootsoftc));
}

static jresult_t td243hc_rev2_port_reset(td243hc_rev2_softc_t *sc, 
    juint8_t port)
{
    DBG_V(DHCD_HUB, ("td243hc_rev2_port_reset: port %d entered\n", port));

    if (!(READ4(TD243HC_R2_PORT_STATUS_REG(port)) & TD243HC_R2_PORTSC_CCS))
    {
        DBG_E(DHCD_HUB, ("td243hc_rev2_port_reset: Trying to reset the not "
            "connected port %d\n", port));
        return JEINVAL;
    }

    /* Reset the Port*/
    WRITE4(TD243HC_R2_PORT_STATUS_REG(port), TD243HC_R2_PORTSC_PRS);
   
    return 0;
}

static jresult_t td243hc_rev2_suspend(usbd_bus_handle bus)
{
    struct td243hc_rev2_softc *sc = (struct td243hc_rev2_softc *)bus;
    juint_t index;

    DBG_V(DHCD_POWER, ("td243hc_rev2_suspend: SUSPEND called\n"));

    if (sc->sc_suspend)
        return 0;

    /* Stop transfers schedule */
    WRITE4(TD243HC_R2_ETD_DMA_CHAN_CLR_REG, 0xffffffff);
    WRITE4(TD243HC_R2_ETD_ENA_CLR_REG, 0xffffffff);

    /* Suspend all active ports */
    for (index = 1; index <= TD243HC_R2_MAX_PORTS; index++)
        td243hc_rev2_do_port_change((void *)sc, index, UHF_PORT_SUSPEND, 1);

    /* Allow only for resume and port change interrupts */
    WRITE4(TD243HC_R2_SYS_INTR_EN_REG, TD243HC_R2_SYS_INTR_RD |
        TD243HC_R2_SYS_INTR_RHSC);

    /* Stop controller */
    WRITE4(TD243HC_R2_CONTROL_REG, BFSET4(TD243HC_R2_HCUSBSTE, 
        TD243HC_R2_USB_SUSPEND) | TD243HC_R2_RMTWUEN);

    sc->sc_suspend = TRUE;

    return 0;
}

static jresult_t td243hc_rev2_resume(usbd_bus_handle bus)
{
    struct td243hc_rev2_softc *sc = (struct td243hc_rev2_softc *)bus;
    juint_t index;

    DBG_V(DHCD_POWER, ("td243hc_rev2_resume: RESUME called\n"));

    if (!sc->sc_suspend)
    {
        return 0;
    }

    /* Turn ON the controller */
    /* Resume controller */
    WRITE4(TD243HC_R2_CONTROL_REG, BFSET4(TD243HC_R2_HCUSBSTE, 
        TD243HC_R2_USB_OPER) | TD243HC_R2_RMTWUEN);

    sc->sc_suspend = FALSE;

    /* Resume the ports */
    for (index = 1; index <= TD243HC_R2_MAX_PORTS; index++)
        td243hc_rev2_do_port_change((void *)sc, index, UHF_PORT_SUSPEND, 0);

    jdelay_ms(USB_RESUME_DELAY);

    /* Turn on interrupts */
    WRITE4(TD243HC_R2_SYS_INTR_EN_REG, sc->eintrs);

    return 0;
}

static usbd_status td243hc_rev2_allocm(struct usbd_bus *bus, 
    jdma_handle *dma, juint32_t size)
{
    jresult_t rc;

    /* The DMA controller has limitation DWORD access to the data 
     * To allow each transfer to be done by DMA we need to enlarge 
     * the buffer size */
    if (size & (TD243HC_R2_XY_SIZE - 1))
        size = (size + TD243HC_R2_XY_SIZE) & ~(TD243HC_R2_XY_SIZE - 1);
    
    rc = jdma_alloc(size, 0, 0, 0, M_CACHABLE, dma);
#if JDEBUG
    if (rc)
        DBG_W(DHCD_TRANSFER, ("td243hc_rev2__allocm: jdma_alloc()=%d\n", rc));
#endif
    return (rc ? USBD_NOMEM : USBD_NORMAL_COMPLETION);
}

static void td243hc_rev2_freem(struct usbd_bus *bus, jdma_handle *dma)
{
    jdma_free(*dma);
}

static usbd_xfer_handle td243hc_rev2_allocx(struct usbd_bus *bus)
{
    return (usbd_xfer_handle)(jmalloc(sizeof(struct td243hc_rev2_xfer), 
        M_ZERO));
}

static void td243hc_rev2_freex(struct usbd_bus *bus, usbd_xfer_handle xfer)
{
    jfree((void *)xfer);
}

static jbool_t td243hc_rev2_is_active(struct usbd_bus *bus)
{
    struct td243hc_rev2_softc *sc = (struct td243hc_rev2_softc *)bus;
    return (!sc->sc_dying);
}

#ifdef CONFIG_JOTG
static jbool_t td243hc_rev2_is_otg_host_enabled(struct usbd_bus *bus)
{
    /* This controller will never be used in OTG configuration 
     * we always return true */
    return 1;
}
#endif

static void td243hc_rev2_device_clear_toggle(usbd_pipe_handle pipe)
{
    struct td243hc_rev2_pipe *tdpipe = (struct td243hc_rev2_pipe *)pipe;
    tdpipe->nexttoggle = 0;
    tdpipe->epd &= ~(TD243HC_R2_ETD_HALTED);
}

static void td243hc_rev2_noop(usbd_pipe_handle pipe)
{
}

static usbd_status td243hc_rev2_open(usbd_pipe_handle pipe)
{
    td243hc_rev2_pipe_t *tdpipe = (td243hc_rev2_pipe_t *)pipe;
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)pipe->device->bus;
    usb_endpoint_descriptor_t *ed = pipe->endpoint->edesc;
    juint8_t  pipe_type = ed->bmAttributes & UE_XFERTYPE;
    usbd_status rc = USBD_NORMAL_COMPLETION;
    juint8_t ep_index = UE_GET_ADDR(ed->bEndpointAddress);
    juint8_t ep_direct = UE_GET_DIR(ed->bEndpointAddress) ? 
        TD243HC_R2_ETD_DIRECT_IN : TD243HC_R2_ETD_DIRECT_OUT;
    juint16_t max_packet_size = ua_get16(ed->wMaxPacketSize);
    juint8_t ep_type = 0;
    juint8_t ep_speed = pipe->device->speed == USB_SPEED_LOW ? 
        TD243HC_R2_ETD_SPEED_LOW : TD243HC_R2_ETD_SPEED_FULL;
    
    DBG_V(DHCD_TRANSFER, ("td243hc_rev2_open: Entered pipe_type=0x%x "
        "ep_index=0x%x %d\n", pipe_type, ep_index, 
        ep_direct == TD243HC_R2_ETD_DIRECT_OUT ? 0 : 1));

    if (sc->sc_dying)
        return (USBD_IOERROR);

    tdpipe->nexttoggle = 0;

    switch (pipe_type)
    {
    case UE_CONTROL:
        pipe->methods = &td243hc_rev2_device_methods;
        ep_type = TD243HC_R2_ETD_FORMAT_CTRL;
        ep_direct = TD243HC_R2_ETD_DIRECT_IN | TD243HC_R2_ETD_DIRECT_OUT;
        break;
    case UE_BULK:
        pipe->methods = &td243hc_rev2_device_methods;
        ep_type = TD243HC_R2_ETD_FORMAT_BULK;
        break;
    case UE_INTERRUPT:
        pipe->methods = &td243hc_rev2_device_methods;
        ep_type = TD243HC_R2_ETD_FORMAT_INTR;
        break;
    case UE_ISOCHRONOUS:
        pipe->methods = &td243hc_rev2_device_isoc_methods;
        ep_type = TD243HC_R2_ETD_FORMAT_ISOC;
        break;
    default:
        rc = USBD_INVAL;
        break;
    }

    if (!rc)
    {
        tdpipe->epd = BFSET4(TD243HC_R2_ETD_ADDRESS, pipe->device->address) |
            BFSET4(TD243HC_R2_ETD_ENDPT, ep_index) |
            BFSET4(TD243HC_R2_ETD_DIRECT, ep_direct) |
            BFSET4(TD243HC_R2_ETD_SPEED, ep_speed) |
            BFSET4(TD243HC_R2_ETD_FORMAT, ep_type) |
            BFSET4(TD243HC_R2_ETD_MAXPKTSIZ, max_packet_size);
    }
    
    DBG_V(DHCD_TRANSFER, ("td243hc_rev2_open: Done\n"));
    return rc;
}

static usbd_status td243hc_rev2_do_port_change(void *context, juint8_t index, 
    jint_t value, jbool_t set)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)context;
    usbd_status err = USBD_NORMAL_COMPLETION;
    juint32_t reg_val = 0;

    DBG_V(DHCD_HUB, ("td243hc_rev2_do_port_change: port=%d feature=0x%x %d\n", 
        index, value, set));

    if (set)
    {
        switch(value) 
        {
        case UHF_PORT_ENABLE:
            reg_val = TD243HC_R2_PORTSC_PES;
            break;
        case UHF_PORT_SUSPEND:
            reg_val = TD243HC_R2_PORTSC_PSS;
            break;
        case UHF_PORT_POWER:
            reg_val = TD243HC_R2_PORTSC_PPS;
            break;
        case UHF_PORT_RESET:
            if (td243hc_rev2_port_reset(sc, index))
                err = USBD_IOERROR;
            break;
        default:
            err = USBD_IOERROR;
        }
    }
    else
    {
        switch(value) 
        {
        case UHF_PORT_ENABLE:
            reg_val = TD243HC_R2_PORTSC_CCS;
            break;
        case UHF_PORT_SUSPEND:
            reg_val = TD243HC_R2_PORTSC_POCI;
            break;
        case UHF_PORT_POWER:
            reg_val = TD243HC_R2_PORTSC_LSDA;
            break;
        case UHF_PORT_TEST:
            break;
        case UHF_PORT_INDICATOR:
            break;
        case UHF_C_PORT_CONNECTION:
            reg_val = TD243HC_R2_PORTSC_CSC;
            break;
        case UHF_C_PORT_ENABLE:
            reg_val = TD243HC_R2_PORTSC_PESC;
            break;
        case UHF_C_PORT_SUSPEND:
            reg_val = TD243HC_R2_PORTSC_PSSC;
            break;
        case UHF_C_PORT_OVER_CURRENT:
            reg_val = TD243HC_R2_PORTSC_OCIC;
            break;
        case UHF_C_PORT_RESET:
            reg_val = TD243HC_R2_PORTSC_PRSC;
            break;
        default:
            err = USBD_IOERROR;
            break;
        }
    }

    if (reg_val)
        WRITE4(TD243HC_R2_PORT_STATUS_REG(index), reg_val);
   
    return err;
}

static juint16_t td243hc_rev2_get_port_status(void *context, juint8_t index)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)context;
    juint16_t status = 0;
    juint32_t v = READ4(TD243HC_R2_PORT_STATUS_REG(index));

    if (v & TD243HC_R2_PORTSC_CCS)
        status |= UPS_CURRENT_CONNECT_STATUS;
    if (v & TD243HC_R2_PORTSC_PES)
        status |= UPS_PORT_ENABLED;
    if (v & TD243HC_R2_PORTSC_PSS)
        status |= UPS_SUSPEND;
    if (v & TD243HC_R2_PORTSC_POCI)
        status |= UPS_OVERCURRENT_INDICATOR;
    if (v & TD243HC_R2_PORTSC_PRS)
        status |= UPS_RESET;
    if (v & TD243HC_R2_PORTSC_PPS)
        status |= UPS_PORT_POWER;
    if (v & TD243HC_R2_PORTSC_LSDA)
        status |= UPS_LOW_SPEED;
 
    DBG_V(DHCD_HUB, ("td243hc_rev2_get_port_status: port=%d status=0x%04x\n", 
        index, status));

    return status;
}

static juint16_t td243hc_rev2_get_port_change(void *context, juint8_t index)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)context;
    juint16_t status = 0;
    juint32_t v = READ4(TD243HC_R2_PORT_STATUS_REG(index));
    
    if (v & TD243HC_R2_PORTSC_CSC)
        status |= UPS_C_CONNECT_STATUS;
    if (v & TD243HC_R2_PORTSC_PESC)
        status |= UPS_C_PORT_ENABLED;
    if (v & TD243HC_R2_PORTSC_OCIC)
        status |= UPS_C_OVERCURRENT_INDICATOR;
    if (v & TD243HC_R2_PORTSC_PRSC)
        status |= UPS_C_PORT_RESET;
    if (v & TD243HC_R2_PORTSC_PSSC)
        status |= UPS_C_SUSPEND;
    
    DBG_V(DHCD_HUB, ("td243hc_rev2_get_port_change: port=%d status=0x%04x\n", 
        index, status));

    return status;
}

static jint_t td243hc_rev2_port_interrupt_set(void *context, jbool_t enable)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)context;
    
    DBG_V(DHCD_HUB, ("td243hc_rev2_port_interrupt_set: %d port change interrupt"
        "\n", enable));

    if (enable)
    {
        sc->eintrs |= TD243HC_R2_SYS_INTR_RHSC;
        SET4(TD243HC_R2_SYS_INTR_EN_REG, TD243HC_R2_SYS_INTR_RHSC);
    }
    else
    {
        sc->eintrs &= ~TD243HC_R2_SYS_INTR_RHSC;
        CLEAR4(TD243HC_R2_SYS_INTR_EN_REG, TD243HC_R2_SYS_INTR_RHSC);
    }

    return 0;
}

static void td243hc_rev2_abort_xfer(usbd_xfer_handle xfer, 
    usbd_status status)
{
    jint_t etd_num = (jint_t)xfer->hcpriv;
    usbd_device_handle dev = xfer->pipe->device;
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)dev->bus;

    DBG_X(DHCD_ABORT, ("td243hc_rev2_abort_xfer:etd[%d] xfer %p status %d\n",
        etd_num, xfer, status));

    xfer->status = status;

    if (etd_num != TD243HC_R2_NUM_ETDS)
    {
        /* Stop transfer onthe HW and disable ETD */
        WRITE4(TD243HC_R2_ETD_DMA_CHAN_CLR_REG, TD243HC_R2_ETD_DMA(etd_num));

        /* Disable ETD */
        WRITE4(TD243HC_R2_ETD_ENA_CLR_REG, TD243HC_R2_ETD(etd_num));
    }
    usb_transfer_complete(xfer);
}

static void td243hc_rev2_setup_dma_transfer(td243hc_rev2_softc_t *sc, 
    struct xfrag *curr, juint_t etd_num)
{
    juint32_t start_addr = BFSET4(TD243HC_R2_ETDSMSA, 
        ((juint32_t)curr->cpu_addr + curr->offset));

    /* Clear XY statuses */
    TOGGLE_IF_SET(TD243HC_R2_XFILLED_STATUS_REG, TD243HC_R2_ETD(etd_num));
    TOGGLE_IF_SET(TD243HC_R2_YFILLED_STATUS_REG, TD243HC_R2_ETD(etd_num));
    TOGGLE_IF_SET(TD243HC_R2_XBUF_INT_STATUS_REG, TD243HC_R2_ETD(etd_num));
    TOGGLE_IF_SET(TD243HC_R2_YBUF_INT_STATUS_REG, TD243HC_R2_ETD(etd_num));

    /* Update the correct system addres for DMA transfer */
    WRITE4(TD243HC_R2_ETD_SYS_MEM_START_ADDR_REG(etd_num), start_addr);

    /* Enable DMA for specific ETD */
    WRITE4(TD243HC_R2_ETD_DMA_ENABLE_REG, TD243HC_R2_ETD(etd_num));
}

static jresult_t td243hc_rev2_transfer_frag(td243hc_rev2_softc_t *sc, 
    juint_t etd_num)
{
    td243hc_rev2_etd_t etd;
    td243hc_rev2_etd_table_t *etd_entry = &sc->etd_table[etd_num];
    td243hc_rev2_xfer_t *tdxfer = etd_entry->xfer; 
    td243hc_rev2_pipe_t *tdpipe = (td243hc_rev2_pipe_t *)tdxfer->xfer.pipe;
    struct xfrag *curr = etd_entry->current_active;
    juint16_t max_packet_size = 
        ua_get16(tdpipe->pipe.endpoint->edesc->wMaxPacketSize); 
    juint32_t buf_size = curr->size; 

    j_memset(&etd, 0, sizeof(td243hc_rev2_etd_t));

    /* DWORD 0 */
    etd.dw0 = tdpipe->epd;
    etd.dw0 &= ~BFMASK4(TD243HC_R2_ETD_MAXPKTSIZ);
    etd.dw0 |= BFSET4(TD243HC_R2_ETD_MAXPKTSIZ, max_packet_size);
    
    if (BFGET4(TD243HC_R2_ETD_FORMAT, etd.dw0) == TD243HC_R2_ETD_FORMAT_CTRL)
        etd.dw0 &= ~TD243HC_R2_ETD_HALTED;

    /* DWORD 1 */
    etd.dw1 = BFSET4(TD243HC_R2_DW1_XBUFSRTAD, etd_entry->xbuf_offset) | 
        BFSET4(TD243HC_R2_DW1_YBUFSRTAD, etd_entry->ybuf_offset);

     /* DWORD 2 */
    if ((curr->flags & XFRAG_TOKEN_MASK) == XFRAG_SETUP)
    {
        DBG_V(DHCD_TRANSFER, ("SENDING SETUP TOKEN \n"));
        etd.dw2 = BFSET4(TD243HC_R2_DW2_RTRYDELAY, 1) |
            BFSET4(TD243HC_R2_DW2_DIRPID, TD243HC_R2_DW2_DIRPID_SETUP) |
            BFSET4(TD243HC_R2_DW2_DATATOGL, 2);
    }
    else
    {
        DBG_V(DHCD_TRANSFER, ("SENDING %c %c TOKEN\n", 
            (curr->flags & XFRAG_TOKEN_MASK) == XFRAG_STATUS ? 'S':'D',
            ((curr->flags & XFRAG_DIR_MASK) == XFRAG_IN) ? 'I':'O'));

        etd.dw2 = BFSET4(TD243HC_R2_DW2_DIRPID, 
            (((curr->flags & XFRAG_DIR_MASK) == XFRAG_IN) ? 
            TD243HC_R2_DW2_DIRPID_IN : TD243HC_R2_DW2_DIRPID_OUT));

        etd.dw2 |= TD243HC_R2_DW2_BUFROUND;

        if ((curr->flags & XFRAG_TOKEN_MASK) == XFRAG_STATUS)
            etd.dw2 |= BFSET4(TD243HC_R2_DW2_DATATOGL, 3);

        if (BFGET4(TD243HC_R2_ETD_FORMAT, etd.dw0) == 
            TD243HC_R2_ETD_FORMAT_INTR)
        {
            jint8_t pol_interval = tdpipe->pipe.interval;
            juint8_t rel_pol_pos = 0;
            
            if (pol_interval == USBD_DEFAULT_INTERVAL)
                    pol_interval = tdpipe->pipe.endpoint->edesc->bInterval;
            
            rel_pol_pos = (juint8_t)(BFGET4(TD243HC_R2_FRMNUMB,
                READ4(TD243HC_R2_FRAME_NUMBER_REG)) + pol_interval);
            
            etd.dw2 |= BFSET4(TD243HC_R2_DW2_POLINTERV, pol_interval - 1) | 
                BFSET4(TD243HC_R2_DW2_RELPOLPOS, rel_pol_pos);
        }
        else
        {
            etd.dw2 |= BFSET4(TD243HC_R2_DW2_RTRYDELAY, 1);
        }
    }
    
    /* DWORD 3*/
    etd.dw3 = BFSET4(TD243HC_R2_DW3_TOTBYECNT, buf_size) | 
        BFSET4(TD243HC_R2_DW3_BUFSIZE, TD243HC_R2_XY_SIZE - 1);
    
    /* Enable the done interrupt and make sure it will happend immediate */
    SET4(TD243HC_R2_ETD_DONE_ENA_REG, TD243HC_R2_ETD(etd_num));
    SET4(TD243HC_R2_IMMEDIATE_INT_REG, TD243HC_R2_ETD(etd_num));
   
    /* Set etd */
    td243hc_rev2_etd_write(sc, etd_num, (juint8_t*)&etd);  

    /* Prepare transfer */
    td243hc_rev2_setup_dma_transfer(sc, curr, etd_num);

    /* Procceed ETD */
    if (sc->add_to_list)
        sc->etd_to_proceed |= TD243HC_R2_ETD(etd_num);
    else
        WRITE4(TD243HC_R2_ETD_ENA_REG, TD243HC_R2_ETD(etd_num));

    return 0;
}

static void td243hc_rev2_timeout_task(void *addr)
{
    usbd_xfer_handle xfer = addr;

    DBG_V(DHCD_TRANSFER, ("td243hc_rev2_timeout_task: xfer=%p\n", xfer));

    td243hc_rev2_abort_xfer(xfer, USBD_TIMEOUT);
}

static usbd_status td243hc_rev2_device_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /* Pipe isn't running, start first */
    return (td243hc_rev2_device_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

static usbd_status td243hc_rev2_device_start(usbd_xfer_handle xfer)
{
    td243hc_rev2_pipe_t *tdpipe = (td243hc_rev2_pipe_t *)xfer->pipe;
    td243hc_rev2_xfer_t *tdxfer = (td243hc_rev2_xfer_t *)xfer;
    usb_device_request_t *req = &xfer->request;
    usbd_device_handle dev = tdpipe->pipe.device;
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)dev->bus;
    juint_t etd;
    usbd_status err;
    jresult_t rc;
    td243hc_rev2_etd_table_t *etd_entry = NULL;

    UNUSED_VAR(req);
    DBG_V(DHCD_TRANSFER, ("td243hc_rev2_device_start: type=0x%02x, "
        "request=0x%02x, wValue=0x%04x, wIndex=0x%04x, addr=%d, "
        "endpt=%d\n", req->bmRequestType, req->bRequest, 
        ua_get16(req->wValue), ua_get16(req->wIndex), dev->address, 
        xfer->pipe->endpoint->edesc->bEndpointAddress));
   
    xfer->hcpriv = (void *)(juint32_t)TD243HC_R2_NUM_ETDS;

    etd = td243hc_rev2_alloc_etd(sc, tdxfer);
    if (etd == TD243HC_R2_NUM_ETDS)
    {
        DBG_E(DHCD_TRANSFER, ("td243hc_rev2_device_start: NO etd\n"));
        err = USBD_NOT_STARTED;
        goto Error;
    }

    etd_entry = &sc->etd_table[etd];
    
    rc = xfrag_alloc_chain(xfer, MAX(xfer->length, TD243HC_R2_XY_SIZE), NULL, 
        0, 0, &tdpipe->nexttoggle, &etd_entry->list_head, XFRAG_NO_DMA);
    if (rc)
    {
        DBG_E(DHCD_TRANSFER, ("td243hc_rev2_device_start: Failed to allocate"
            " xfrag\n"));
        err = USBD_NOT_STARTED;
        goto Error;
    }

    xfer->hcpriv = (void *)etd;
    xfer->actlen = 0;
    xfer->status = USBD_IN_PROGRESS;
    etd_entry->current_active = etd_entry->list_head;

    /* Update device address */
    tdpipe->epd &= ~BFMASK4(TD243HC_R2_ETD_ADDRESS);
    tdpipe->epd |= BFSET4(TD243HC_R2_ETD_ADDRESS, dev->address);
    
    DBG_X(DHCD_TRANSFER, ("td243hc_rev2_device_start: etd %d\n", etd));
    
    rc = td243hc_rev2_transfer_frag(sc, etd);
    if (rc)
    {
        DBG_E(DHCD_TRANSFER, ("td243hc_rev2_device_start: Failed to transfer "
            "xfrag\n"));
        err = USBD_NOT_STARTED;
        goto Error;
    }

    if (xfer->timeout)
    {
        DBG_V(DHCD_TRANSFER, ("td243hc_rev2_device_start xfer_timeout 0x%lx\n", 
            xfer->timeout));
        jtask_schedule(xfer->timeout_handle, xfer->timeout,
            td243hc_rev2_timeout_task, xfer);
    }

    return (USBD_IN_PROGRESS);

Error:
    
    xfer->status = err;
    /* The usb_transfer_complete call to the done method 
     * this will clean all we need */
    usb_transfer_complete(xfer);
    return err;
}

static void td243hc_rev2_device_done(usbd_xfer_handle xfer)
{
    jint_t etd = (jint_t)xfer->hcpriv;
    usbd_device_handle dev = xfer->pipe->device;
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)dev->bus;
    td243hc_rev2_etd_table_t *etd_entry = &sc->etd_table[etd];
    jresult_t rc = 0;
    
    if (xfer->pipe->repeat && xfer->status != USBD_CANCELLED)
    {
        xfer->actlen = 0;
        xfer->status = USBD_IN_PROGRESS;
        etd_entry->current_active = etd_entry->list_head;
        rc = td243hc_rev2_transfer_frag(sc, etd);
        if (rc)
        {
            DBG_E(DHCD_TRANSFER, ("td243hc_rev2_device_done: Failed to transfer"
                " xfrag\n"));
            goto xfer_done;
        }
        return;
    }

xfer_done:   
    if (etd != TD243HC_R2_NUM_ETDS)
    {
        if (etd_entry->list_head)
        {
            xfrag_free_chain(etd_entry->list_head);
            etd_entry->current_active = NULL;
            etd_entry->list_head = NULL;
        }
        td243hc_rev2_free_etd(sc, etd);
        xfer->hcpriv = (void *)TD243HC_R2_NUM_ETDS;
    }
}

static void td243hc_rev2_device_abort(usbd_xfer_handle xfer)
{
    DBG_I(DHCD_ABORT, ("td243hc_rev2_device_abort: xfer=%p\n", xfer));

    if (jtask_pending(xfer->timeout_handle))
       jtask_stop(xfer->timeout_handle);
    
    td243hc_rev2_abort_xfer(xfer, USBD_CANCELLED);
}

static void td243hc_rev2_device_close(usbd_pipe_handle pipe)
{
    td243hc_rev2_pipe_t *tdpipe = (td243hc_rev2_pipe_t *)pipe;

    tdpipe->epd = 0;
    pipe->methods = NULL;
}

static void td243hc_rev2_device_isoc_close(usbd_pipe_handle pipe)
{
    NOT_IMPLEMENTED("td243hc_rev2_device_isoc_close");
}

static usbd_status td243hc_rev2_device_isoc_transfer(usbd_xfer_handle xfer) 
{
    NOT_IMPLEMENTED("td243hc_rev2_device_isoc_transfer");

    return USBD_NOT_STARTED;
}

static usbd_status td243hc_rev2_device_isoc_start(usbd_xfer_handle xfer)
{ 
    NOT_IMPLEMENTED("td243hc_rev2_device_isoc_start");

    return USBD_NOT_STARTED;
}

static void td243hc_rev2_device_isoc_abort(usbd_xfer_handle xfer)
{
    NOT_IMPLEMENTED("td243hc_rev2_device_isoc_abort");

    xfer->status = USBD_CANCELLED;
    usb_transfer_complete(xfer);        
}

static void td243hc_rev2_device_isoc_done(usbd_xfer_handle xfer)
{
    NOT_IMPLEMENTED("td243hc_rev2_device_isoc_done");
}

jresult_t td243hc_rev2_hw_ioctl(td243hc_rev2_softc_t *sc, drv_ioctl_t ioctl, 
    void *arg)
{
    DBG_E(DHCD_TRANSFER, ("TD243: Unsupported IOCTL %x\n", ioctl));
    return JEINVAL;
}


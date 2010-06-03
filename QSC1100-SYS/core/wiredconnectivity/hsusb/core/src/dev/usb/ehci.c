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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <jos.h>

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>
#include <usb_quirks.h>

#include <ehcireg.h>
#include <ehcivar.h>
#include <root_hub.h>

#ifdef CONFIG_JOTG
#include <otg_core_host.h>
#endif

#ifdef CONFIG_JHOST_ETEST
#include <jetest.h>
#endif

#ifdef USB_DEBUG
#define bitmask_snprintf(q,f,b,l) j_snprintf((b), (l), "%b", (q), (f))
#endif

#ifdef CONFIG_TDI_4X
#define NO_COMPANION
#define CONTROLLER_BYTE_ORDER CONFIG_BYTE_ORDER
#else
#define CONTROLLER_BYTE_ORDER CPU_LITTLE_ENDIAN
#endif

#if CONTROLLER_BYTE_ORDER == CONFIG_BYTE_ORDER

#    define htoc16(x)   ((juint16_t)(x))
#    define htoc32(x)   ((juint32_t)(x)) 
#    define ctoh16(x)   ((juint16_t)(x))  
#    define ctoh32(x)   ((juint32_t)(x))  

#else

#    define htoc16(x)   bswap16(x)  
#    define htoc32(x)   bswap32(x)  
#    define ctoh16(x)   bswap16(x)  
#    define ctoh32(x)   bswap32(x)  

#endif

typedef juint32_t phys_addr;

usbd_status             ehci_enable(ehci_softc_t *sc);
static usbd_status      ehci_open(usbd_pipe_handle pipe);
static jint_t            ehci_intr1(ehci_softc_t *sc);
static void             ehci_check_intr(ehci_softc_t *sc, struct ehci_xfer *ex);
static void             ehci_idone(struct ehci_xfer *ex);
static void             ehci_timeout_task(void *addr);

static usbd_status      ehci_allocm(struct usbd_bus *bus, jdma_handle *dma,
    juint32_t size);
static void             ehci_freem(struct usbd_bus *bus, jdma_handle *dma);

static usbd_xfer_handle ehci_allocx(struct usbd_bus *bus);
static void             ehci_freex(struct usbd_bus *bus, usbd_xfer_handle xfer);
static jbool_t           ehci_is_active(struct usbd_bus *bus);
static jresult_t         ehci_suspend(usbd_bus_handle bus);
static jresult_t         ehci_resume(usbd_bus_handle bus);
#ifdef CONFIG_JOTG
static jbool_t           ehci_is_otg_host_enabled(struct usbd_bus *bus);
#endif

static usbd_status      ehci_device_ctrl_transfer(usbd_xfer_handle xfer);
static usbd_status      ehci_device_ctrl_start(usbd_xfer_handle xfer);
static void             ehci_device_ctrl_abort(usbd_xfer_handle xfer);
static void             ehci_device_ctrl_close(usbd_pipe_handle pipe);
static void             ehci_device_ctrl_done(usbd_xfer_handle xfer);

static usbd_status      ehci_device_bulk_transfer(usbd_xfer_handle xfer);
static usbd_status      ehci_device_bulk_start(usbd_xfer_handle xfer);
static void             ehci_device_bulk_abort(usbd_xfer_handle xfer);
static void             ehci_device_bulk_close(usbd_pipe_handle pipe);
static void             ehci_device_bulk_done(usbd_xfer_handle xfer);

static usbd_status      ehci_device_intr_transfer(usbd_xfer_handle xfer);
static usbd_status      ehci_device_intr_start(usbd_xfer_handle xfer);
static void             ehci_device_intr_abort(usbd_xfer_handle xfer);
static void             ehci_device_intr_close(usbd_pipe_handle pipe);
static void             ehci_device_intr_done(usbd_xfer_handle xfer);

static usbd_status      ehci_device_isoc_transfer(usbd_xfer_handle xfer);
static usbd_status      ehci_device_isoc_start(usbd_xfer_handle xfer);
static void             ehci_device_isoc_abort(usbd_xfer_handle xfer);
static void             ehci_device_isoc_close(usbd_pipe_handle pipe);
static void             ehci_device_isoc_done(usbd_xfer_handle xfer);
#ifdef CONFIG_PORT_INTERCHIP
static jbool_t           ehci_is_port_interchip(void *context, juint8_t index);
#endif

static void             ehci_device_clear_toggle(usbd_pipe_handle pipe);
static void             ehci_noop(usbd_pipe_handle pipe);

#ifndef NO_COMPANION
static void             ehci_disown(ehci_softc_t *sc, jint_t index,
    jint_t lowspeed);
#endif

static ehci_soft_qh_t  *ehci_alloc_sqh(ehci_softc_t *sc);
static void             ehci_free_sqh(ehci_softc_t *sc, ehci_soft_qh_t *sqh);

static ehci_soft_qtd_t  *ehci_alloc_sqtd(ehci_softc_t *sc);
static void             ehci_free_sqtd(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd);
static usbd_status      ehci_alloc_sqtd_chain(struct ehci_pipe *epipe,
    ehci_softc_t *sc, jint_t alen, jint_t rd, usbd_xfer_handle xfer,
    ehci_soft_qtd_t **sp, ehci_soft_qtd_t **ep);
static void             ehci_free_sqtd_chain(ehci_softc_t *sc, 
    ehci_soft_qtd_t *sqtd, ehci_soft_qtd_t *sqtdend);

static void             ehci_free_all_dma(ehci_softc_t *sc);

static usbd_status      ehci_device_request(usbd_xfer_handle xfer);

static usbd_status      ehci_device_setintr(ehci_softc_t *sc, 
    ehci_soft_qh_t *sqh, jint_t ival);

static void             ehci_add_qh(ehci_soft_qh_t *sqh, ehci_soft_qh_t *head);
static void             ehci_rem_qh(ehci_softc_t *sc, ehci_soft_qh_t *sqh,
    ehci_soft_qh_t *head);
static void             ehci_set_qh_qtd(ehci_soft_qh_t *sqh,
    ehci_soft_qtd_t *sqtd);
static void             ehci_sync_hc(ehci_softc_t *sc);

static void             ehci_close_pipe(usbd_pipe_handle pipe, 
    ehci_soft_qh_t *head);
static void             ehci_abort_xfer(usbd_xfer_handle xfer, 
    usbd_status status);

static usbd_status      ehci_do_port_change(void *context, juint8_t index, 
    jint_t value, jbool_t set);
static juint16_t         ehci_get_port_status(void *context, juint8_t index);
static juint16_t         ehci_get_port_change(void *context, juint8_t index);
static jint_t            ehci_port_interrupt_set(void *context, jbool_t enable);
static void             find_resumed_ports(ehci_softc_t *sc);
static void             ehci_xfer_complete(usbd_xfer_handle xfer);

#ifdef JDEBUG
static void             ehci_dump_regs(ehci_softc_t *sc);
void                    ehci_dump(void);
static ehci_softc_t     *theehci;
static void             ehci_dump_link(ehci_link_t link, jint_t type);
static void             ehci_dump_sqtds(ehci_soft_qtd_t *sqtd);
static void             ehci_dump_sqtd(ehci_soft_qtd_t *sqtd);
static void             ehci_dump_qtd(ehci_qtd_t *qtd);
static void             ehci_dump_sqh(ehci_soft_qh_t *sqh);
#ifdef DIAGNOSTIC
static void             ehci_dump_exfer(struct ehci_xfer *ex);
#endif
#endif

#define EHCI_NULL htoc32(EHCI_LINK_TERMINATE)

static usbd_status ehci_bw_alloc(usbd_pipe_handle pipe, juint16_t treshold,
    juint16_t *uf_shift);
static void ehci_bw_free(usbd_pipe_handle pipe);
static juint16_t ehci_get_fl_uf_num(ehci_softc_t *sc);
static jbool_t ehci_flist_find_uf(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint16_t *next_uf);
static ehci_itd_soft_t * pipe_alloc_itd_slot(ehci_pipe_t *epipe);
static void pipe_free_itd_slot(ehci_pipe_t *epipe, ehci_itd_soft_t *itd_soft);
#ifdef JDEBUG
static void dump_itd_soft(char *title, ehci_pipe_t *epipe, 
    ehci_itd_soft_t *itd_soft);
#endif
static usbd_status ehci_init_itd_pool(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint_t td_size, juint16_t td_align, juint_t link_type);
static void ehci_uninit_itd_pool(ehci_softc_t *sc, ehci_pipe_t *epipe);
static usbd_status ehci_device_set_isoc(ehci_softc_t *sc, 
    usbd_pipe_handle pipe, juint16_t ival, juint8_t hubport, juint8_t hubaddr);
static usbd_status ehci_device_isoc_transfer(usbd_xfer_handle xfer);
static usbd_status ehci_device_isoc_start(usbd_xfer_handle xfer);
static void ehci_isoc_sched(ehci_softc_t *sc, ehci_pipe_t *pipe);
static void ehci_isoc_check_intr(ehci_softc_t *sc, ehci_pipe_t *epipe);
static void ehci_isoc_pipe_update_sync(ehci_softc_t *sc, ehci_pipe_t *epipe);
void ehci_insert_itd_to_flist(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint16_t fl_itd, ehci_itd_soft_t *itd_soft);
static void ehci_isoc_uf_done(ehci_softc_t *sc, ehci_pipe_t *epipe, 
    usbd_xfer_handle xfer);

#define EHCI_INTR_ENDPT 1

#define ehci_add_intr_list(sc, ex) \
    LIST_INSERT_HEAD(&(sc)->sc_intrhead, (ex), inext);
#define ehci_del_intr_list(ex) \
    do { \
        LIST_REMOVE((ex), inext); \
        (ex)->inext.le_prev = NULL; \
    } while (0)
#define ehci_active_intr_list(ex) ((ex)->inext.le_prev != NULL)

static struct usbd_bus_methods ehci_bus_methods = {
    ehci_open,
    ehci_allocm,
    ehci_freem,
    ehci_allocx,
    ehci_freex,
    ehci_is_active,
    ehci_suspend,
    ehci_resume,
#ifdef CONFIG_JOTG
    ehci_is_otg_host_enabled
#endif
};

static struct root_hub_methods ehci_root_hub_methods = {
    ehci_do_port_change,
    ehci_get_port_status,
    ehci_get_port_change,
    ehci_port_interrupt_set
#ifdef CONFIG_PORT_INTERCHIP
    /* HSU temp fix */,/* HSU temp fix */ehci_is_port_interchip
#endif

};

static struct usbd_pipe_methods ehci_device_ctrl_methods = {
    ehci_device_ctrl_transfer,
    ehci_device_ctrl_start,
    ehci_device_ctrl_abort,
    ehci_device_ctrl_close,
    ehci_noop,
    ehci_device_ctrl_done,
};

static struct usbd_pipe_methods ehci_device_intr_methods = {
    ehci_device_intr_transfer,
    ehci_device_intr_start,
    ehci_device_intr_abort,
    ehci_device_intr_close,
    ehci_device_clear_toggle,
    ehci_device_intr_done,
};

static struct usbd_pipe_methods ehci_device_bulk_methods = {
    ehci_device_bulk_transfer,
    ehci_device_bulk_start,
    ehci_device_bulk_abort,
    ehci_device_bulk_close,
    ehci_device_clear_toggle,
    ehci_device_bulk_done,
};

static struct usbd_pipe_methods ehci_device_isoc_methods = {
    ehci_device_isoc_transfer,
    ehci_device_isoc_start,
    ehci_device_isoc_abort,
    ehci_device_isoc_close,
    ehci_noop,
    ehci_device_isoc_done,
};

static juint8_t revbits[EHCI_MAX_POLLRATE] = {
    0x00,0x40,0x20,0x60,0x10,0x50,0x30,0x70,0x08,0x48,0x28,0x68,0x18,0x58,0x38,
    0x78,0x04,0x44,0x24,0x64,0x14,0x54,0x34,0x74,0x0c,0x4c,0x2c,0x6c,0x1c,0x5c,
    0x3c,0x7c,0x02,0x42,0x22,0x62,0x12,0x52,0x32,0x72,0x0a,0x4a,0x2a,0x6a,0x1a,
    0x5a,0x3a,0x7a,0x06,0x46,0x26,0x66,0x16,0x56,0x36,0x76,0x0e,0x4e,0x2e,0x6e,
    0x1e,0x5e,0x3e,0x7e,0x01,0x41,0x21,0x61,0x11,0x51,0x31,0x71,0x09,0x49,0x29,
    0x69,0x19,0x59,0x39,0x79,0x05,0x45,0x25,0x65,0x15,0x55,0x35,0x75,0x0d,0x4d,
    0x2d,0x6d,0x1d,0x5d,0x3d,0x7d,0x03,0x43,0x23,0x63,0x13,0x53,0x33,0x73,0x0b,
    0x4b,0x2b,0x6b,0x1b,0x5b,0x3b,0x7b,0x07,0x47,0x27,0x67,0x17,0x57,0x37,0x77,
    0x0f,0x4f,0x2f,0x6f,0x1f,0x5f,0x3f,0x7f,
};


static usbd_status ehci_reset(ehci_softc_t *sc)
{
    juint32_t hcr = 0;
    juint_t i;
    /* Reset the controller */
    DBG_V(DHCD_INIT, ("%s: resetting\n", 
        j_device_get_nameunit(sc->sc_bus.bdev)));
    EOWRITE4(sc, EHCI_USBCMD, 0);       /* Halt controller */
    jdelay_ms(2);
    EOWRITE4(sc, EHCI_USBCMD, EHCI_CMD_HCRESET);
    for (i = 0; i < 100; i++) 
    {
        jdelay_ms(2);
        hcr = EOREAD4(sc, EHCI_USBCMD) & EHCI_CMD_HCRESET;
        if (!hcr)
            break;
    }
    if (hcr) 
    {
        jprintf("%s: reset timeout\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
        return (USBD_IOERROR);
    }
    return USBD_NORMAL_COMPLETION;
}

usbd_status ehci_init(ehci_softc_t *sc)
{
    juint32_t vers, sparams;
    juint_t i, ncomp;
    usbd_status err = USBD_NORMAL_COMPLETION;
    ehci_soft_qh_t *sqh;
    root_hub_desc_t *root_desc;
    jresult_t rc = 0;

    DBG_I(DHCD_INIT, ("ehci_init: start\n"));

#ifdef CONFIG_JOTG
    sc->sc_bus.otg_port = sc->ocd_handle ? EHCI_OTG_PORT_NUM : 0;
#endif

#ifdef JDEBUG
    theehci = sc;
#endif
    LIST_INIT(&sc->sc_dma_allocs);
    LIST_INIT(&sc->sc_intrhead);
    TAILQ_INIT(&sc->sc_free_xfers);

    sc->sc_lock = NULL;
    rc = jspinlock_init(&sc->sc_lock);
    if (rc)
    {
        err = USBD_NOMEM;
        goto bad2;
    }

    rc = jmsleep_init(&sc->sleep_handle);
    if (rc)
    {
        err = USBD_NOMEM;
        goto bad2;
    }

    sc->fl_uf_num = 0;
    TAILQ_INIT(&sc->isoc_pipe_list);

    sc->sc_offs = EREAD1(sc, EHCI_CAPLENGTH);

    vers = EREAD2(sc, EHCI_HCIVERSION);
    sparams = EREAD4(sc, EHCI_HCSPARAMS);
    sc->sc_noport = EHCI_HCS_N_PORTS(sparams);
    jprintf("%s: EHCI version %x.%x, with %d ports\n", 
        j_device_get_nameunit(sc->sc_bus.bdev),
        vers >> 8, vers & 0xff, sc->sc_noport);

    DBG_I(DHCD_INIT, ("ehci_init: sparams=0x%x\n", sparams));
    sc->sc_npcomp = EHCI_HCS_N_PCC(sparams);
    ncomp = EHCI_HCS_N_CC(sparams);
    if (!ncomp && sc->sc_ncomp) 
    {
        jprintf("%s: no companion controllers found !\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
    }
    sc->sc_ncomp = ncomp;

    if (sc->sc_ncomp > 0) 
    {
        jprintf("%s: companion controller%s, %d port%s each\n",
            j_device_get_nameunit(sc->sc_bus.bdev), sc->sc_ncomp!=1 ? "s" : "",
            EHCI_HCS_N_PCC(sparams),
            EHCI_HCS_N_PCC(sparams)!=1 ? "s" : "");
    }
    
    sc->sc_hasppc = (juint8_t)EHCI_HCS_PPC(sparams);

    sc->sc_bus.usbrev = USBREV_2_0;

    /* Clear port flags */
    for (i = 0; i < sc->sc_noport; i++)
        sc->sc_port_flags[i] = 0;

    /* Init root hub descriptor */
    root_desc = &sc->sc_bus.root_desc;
    root_desc->noport   = sc->sc_noport;
    root_desc->speed    = USB_SPEED_HIGH;
    root_desc->self_powered = 1;
    root_desc->pwr_on_2_pwr_good = 200;
    root_desc->remote_wakeup = 1;

    /* Init the root hub */
    root_hub_init(&(sc->sc_bus.rootsoftc), &ehci_root_hub_methods,
        root_desc, sc); 
#ifdef CONFIG_PORT_INTERCHIP
    for (i =0; i < sc->sc_noport; i++)
    {
        port_info_t info;
        j_get_port_info(j_device_get_bus(sc->sc_bus.bdev) , i, &info);
        if (info.is_interchip)
            sc->interchip_ports |= (1 << i);
    }
#endif
    sc->sc_rand = 96;

    /* frame list size at default, read back what we got and use that */
    switch (EHCI_CMD_FLS(EOREAD4(sc, EHCI_USBCMD))) 
    {
    case 0: 
        sc->sc_flsize = 1024;
        break;
    case 1: 
        sc->sc_flsize = 512; 
        break;
    case 2: 
        sc->sc_flsize = 256;
        break;
    case 3: 
        return (USBD_IOERROR);
    }
    rc = jdma_alloc(sc->sc_flsize * sizeof(ehci_link_t),
        EHCI_FLALIGN_ALIGN, 0, 0, 0, &sc->sc_fldma);
    if (rc)
        return USBD_NOMEM;

    sc->sc_flist_soft = 
        jmalloc(sc->sc_flsize * sizeof(*(sc->sc_flist_soft)), M_ZERO);

    if (!sc->sc_flist_soft)
        return USBD_NOMEM;

    sc->sc_flist = KERNADDR(&sc->sc_fldma, 0);
    DBG_V(DHCD_INIT, ("%s: flsize=%d flist=%x fldma=%x\n", 
        j_device_get_nameunit(sc->sc_bus.bdev), sc->sc_flsize,
        sc->sc_flist, DMAADDR(&sc->sc_fldma,0)));

    /* Set up the bus struct. */
    sc->sc_bus.methods = &ehci_bus_methods;
    sc->sc_bus.pipe_size = sizeof(struct ehci_pipe);

    /*
     * Allocate the interrupt dummy QHs. These are arranged to give
     * poll intervals that are powers of 2 times 1ms.
     */
    for (i = 0; i < EHCI_INTRQHS; i++) 
    {
        sqh = ehci_alloc_sqh(sc);
        if (sqh == NULL) 
        {
            err = USBD_NOMEM;
            goto bad1;
        }
        sc->sc_islots[i].sqh = sqh;
    }
    for (i = 0; i < EHCI_INTRQHS; i++) 
    {
        sqh = sc->sc_islots[i].sqh;
        if (i == 0) 
        {
            /* The last (1ms) QH terminates. */
            sqh->qh.qh_link = EHCI_NULL;
            sqh->next = NULL;
        } 
        else
        {
            /* Otherwise the next QH has half the poll interval */
            sqh->next = sc->sc_islots[(i + 1) / 2 - 1].sqh;
            sqh->qh.qh_link = htoc32(sqh->next->physaddr |
                EHCI_LINK_QH);
        }
        sqh->qh.qh_endp = htoc32(EHCI_QH_SET_EPS(EHCI_QH_SPEED_HIGH));
        sqh->qh.qh_curqtd = EHCI_NULL;
        sqh->next = NULL;
        sqh->qh.qh_qtd.qtd_next = EHCI_NULL;
        sqh->qh.qh_qtd.qtd_altnext = EHCI_NULL;
        sqh->qh.qh_qtd.qtd_status = htoc32(EHCI_QTD_HALTED);
        sqh->sqtd = NULL;
    }
    /* Point the frame list at the last level (128ms). */
    for (i = 0; i < sc->sc_flsize; i++) 
    {
        jint_t j;

        j = (i & ~(EHCI_MAX_POLLRATE-1)) |
            revbits[i & (EHCI_MAX_POLLRATE-1)];
        sc->sc_flist[j] = htoc32(EHCI_LINK_QH |
            sc->sc_islots[EHCI_IQHIDX(EHCI_IPOLLRATES - 1,
            i)].sqh->physaddr);
    }

    /* Allocate dummy QH that starts the async list. */
    sqh = ehci_alloc_sqh(sc);
    if (sqh == NULL) 
    {
        err = USBD_NOMEM;
        goto bad1;
    }
    /* Fill the QH */
    sqh->qh.qh_endp =
        htoc32(EHCI_QH_SET_EPS(EHCI_QH_SPEED_HIGH) | EHCI_QH_HRECL);
    sqh->qh.qh_link =
        htoc32(sqh->physaddr | EHCI_LINK_QH);
    sqh->qh.qh_curqtd = EHCI_NULL;
    sqh->next = NULL;
    /* Fill the overlay qTD */
    sqh->qh.qh_qtd.qtd_next = EHCI_NULL;
    sqh->qh.qh_qtd.qtd_altnext = EHCI_NULL;
    sqh->qh.qh_qtd.qtd_status = htoc32(EHCI_QTD_HALTED);
    sqh->sqtd = NULL;

    DBG_RUN(DHCD_INIT, DL_EX_VERBOSE, ehci_dump_sqh(sqh));

    /* Point to async list */
    sc->sc_async_head = sqh;

#ifdef CONFIG_JOTG
    if (!sc->ocd_handle)
#endif
    {
        rc = ehci_reset(sc);
        if (rc)
            goto bad1;

        rc = ehci_enable(sc);
        if (rc)
            goto bad1;
    }
    return USBD_NORMAL_COMPLETION;

bad1:
    jdma_free(sc->sc_fldma);
    /* HSU addition: Memory leak */
    sc->sc_fldma = NULL;
    /* End HSU addition */

bad2:
    if (sc->sleep_handle)
    {
        jmsleep_uninit(sc->sleep_handle);
        sc->sleep_handle = NULL;
    }

    if (sc->sc_lock)
    {
        jspinlock_uninit(sc->sc_lock);
        sc->sc_lock = NULL;
    }
    return (err);
}

usbd_status ehci_enable(ehci_softc_t *sc)
{
    juint32_t cparams;
    juint_t i;
    juint32_t hcr = 0;

    cparams = EREAD4(sc, EHCI_HCCPARAMS);
    DBG_I(DHCD_INIT, ("ehci_enable: cparams=0x%x\n", cparams));

    if (EHCI_HCC_64BIT(cparams)) 
    {
        /* MUST clear segment register if 64 bit capable. */
        EOWRITE4(sc, EHCI_CTRLDSSEGMENT, 0);
    }

#ifdef CONFIG_TDI_4X
#ifdef CONFIG_CX2417X
    EOWRITE4(sc, TDI_4X_BURST_SIZE, TDI_4X_BURST_SIZE_QUIRK);
    EOWRITE4(sc,TDI_4X_USBMODE, 
        TDI_4X_USBMODE_HOST | TDI_4X_USBMODE_SDIS_ACTIVE |
        ((CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN) ? TDI_4X_USBMODE_BIGENDIAN : 0));
    EOWRITE4(sc, TDI_4X_TTCTRL, 0x1000000);
    EOWRITE4(sc, EHCI_PORTSC(1), 0);
#else

#ifdef CONFIG_JOTG
    if (!sc->ocd_handle)
#endif
    {
      /* HSU addition - added TDI_4X_USBMODE_SDIS_ACTIVE */
      EOWRITE4(sc,TDI_4X_USBMODE, 
               TDI_4X_USBMODE_HOST | TDI_4X_USBMODE_SDIS_ACTIVE |
               ((CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN) ? 
               TDI_4X_USBMODE_BIGENDIAN : 0));

#ifdef CONFIG_FULL_SPEED_ONLY
        DBG_I(DHCD_INIT, ("ehci_enable: forcing host to connect as full "
            "speed\n"));
    EOWRITE4(sc, EHCI_PORTSC(1), EOREAD4(sc, EHCI_PORTSC(1)) | TDI_4X_PFSC);
#endif
    }
#endif
#endif

    EOWRITE4(sc, EHCI_PERIODICLISTBASE, DMAADDR(&sc->sc_fldma, 0));

    EOWRITE4(sc, EHCI_ASYNCLISTADDR, sc->sc_async_head->physaddr|EHCI_LINK_QH);

    /* Turn on controller */
    EOWRITE4(sc, EHCI_USBCMD,
        sc->itc_num | 
        (EOREAD4(sc, EHCI_USBCMD) & EHCI_CMD_FLS_M) |
        EHCI_CMD_ASE |
        EHCI_CMD_PSE |
        EHCI_CMD_RS);

  /* HSU addition */
  /* For the UICC core, set bit 27 of USBCMD (Select 2 Wire interface).      */
  /* We need this to prevent false port connect detection by the link.       */
  /* TODO: Update this to generic solution, according to Jungo Inter-chip    */
  /* support when B67659 is released.                                        */
  if ((juint32_t)j_device_get_bus(sc->sc_bus.bdev) == 1)
  {
      EOWRITE4(sc, EHCI_USBCMD,
      EOREAD4(sc, EHCI_USBCMD) | 0x08000000UL);
  }
  /* End of HSU addition */

#ifndef CONFIG_CX2417X
    /* Take over port ownership */
    EOWRITE4(sc, EHCI_CONFIGFLAG, EHCI_CONF_CF);
#endif

    for (i = 0; i < 100; i++)
    {
        jdelay_ms(2);
        hcr = EOREAD4(sc, EHCI_USBSTS) & EHCI_STS_HCH;
        if (!hcr)
            break;
    }
    if (hcr)
    {
        jprintf("%s: run timeout\n", j_device_get_nameunit(sc->sc_bus.bdev));
        return (USBD_IOERROR);
    }

    sc->sc_flags = EHCI_SCFLG_DONEINIT;

#ifdef CONFIG_JOTG
    sc->otg_host_enabled = 1;
#endif
    
    /* Enable interrupts */
    DBG_X(DHCD_INIT,("ehci_init: enabling interrupts\n"));
    sc->sc_eintrs = EHCI_NORMAL_INTRS;
    EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);

    return USBD_NORMAL_COMPLETION;
}

jint_t ehci_intr(void *v)
{
    ehci_softc_t *sc = (ehci_softc_t *)v;

    if (sc == NULL || sc->sc_dying)
        return (0);

    return (ehci_intr1(sc));
}

static jint_t ehci_intr1(ehci_softc_t *sc)
{
    juint32_t intrs, eintrs;
    jint_t rc = ISR_DONT_CALL_SOFT_INTR; 
    DBG_I(DHCD_INTR, ("ehci_intr1: enter\n"));

    /* In case the interrupt occurs before initialization has completed. */
    if (sc == NULL) 
    {
#ifdef DIAGNOSTIC
        jprintf("ehci_intr: sc == NULL\n");
#endif
        return rc;
    }

    intrs = EHCI_STS_INTRS(EOREAD4(sc, EHCI_USBSTS));
    if (!intrs)
        return rc;

    eintrs = intrs & sc->sc_eintrs;
    DBG_X(DHCD_INTR, ("ehci_intr: sc=%p intrs=0x%x(0x%x) eintrs=0x%x\n",
        sc, (juint_t)intrs, EOREAD4(sc, EHCI_USBSTS),
        (juint_t)eintrs));
    if (!eintrs)
        return rc;

    EOWRITE4(sc, EHCI_USBSTS, intrs); /* Acknowledge */
    sc->sc_bus.intr_context++;
    sc->sc_bus.no_intrs++;
    if (eintrs & EHCI_STS_IAA) 
    {
        DBG_I(DHCD_INTR, ("ehci_intr1: door bell\n"));
        sc->sc_flags |= EHCI_DOOR_BELL;
        eintrs &= ~EHCI_STS_IAA;
        rc = ISR_CALL_SOFT_INTR;
    }
    if (eintrs & EHCI_STS_FLR) 
    {
        DBG_X(DHCD_INTR,("ehci_intr1: flist rollover\n"));
        sc->sc_flags |= EHCI_FL_ROLLOVER;
        eintrs &= ~EHCI_STS_FLR;
        rc = ISR_CALL_SOFT_INTR;
        sc->fl_uf_num += (juint16_t)(sc->sc_flsize * EHCI_UFRAME_IN_ITD);    
    }
    if (eintrs & (EHCI_STS_INT | EHCI_STS_ERRINT)) 
    {
        DBG_X(DHCD_INTR, ("ehci_intr1: %s %s\n",
            eintrs & EHCI_STS_INT ? "INT" : "",
            eintrs & EHCI_STS_ERRINT ? "ERRINT" : ""));
        sc->sc_flags |= EHCI_TRANSMIT_COMPLETE;
        eintrs &= ~(EHCI_STS_INT | EHCI_STS_ERRINT);
        sc->sc_flags |= EHCI_TRANSFER;
        rc = ISR_CALL_SOFT_INTR;
    }
    if (eintrs & EHCI_STS_HSE) 
    {
        jprintf("%s: unrecoverable error, controller halted\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
        sc->sc_dying = 1;
    }
    if (eintrs & EHCI_STS_PCD) 
    {        
        /*
         * Disable PCD interrupt for now, because it will be
         * on until the port has been reset.
         */
        sc->sc_flags |= EHCI_ENB_INTR_ONLY; 
        rc = ISR_CALL_SOFT_INTR;
    }

    sc->sc_bus.intr_context--;

    if (eintrs != 0)
    {
        /* Block unprocessed interrupts. */
        sc->sc_eintrs &= ~eintrs;
        EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
        DBG_I(DHCD_INTR,("%s: blocking intrs 0x%x\n",
            j_device_get_nameunit(sc->sc_bus.bdev), eintrs));
    }

    return rc;
}

void ehci_softintr(void *v)
{
    ehci_softc_t *sc = v;
    struct ehci_xfer *ex, *nextex;
    ehci_pipe_t *epipe;
    jint_t sc_flags;

    DBG_I(DHCD_INTR,("%s: ehci_softintr (%d)\n", 
        j_device_get_nameunit(sc->sc_bus.bdev),
        sc->sc_bus.intr_context));

    jspinlock_lock(sc->sc_lock);
    {
        sc_flags = sc->sc_flags;
        sc->sc_flags &= ~(EHCI_ENB_INTR_ONLY | EHCI_DOOR_BELL | 
            EHCI_TRANSMIT_COMPLETE | EHCI_FL_ROLLOVER | EHCI_TRANSFER);
    }
    jspinlock_unlock(sc->sc_lock);
    
    if (sc_flags & EHCI_ENB_INTR_ONLY)
    {
        /* In suspend state we resume the stack upon a port change */
        if (sc->sc_suspend)
        {
            jprintf("%s: resume detected\n", 
                j_device_get_nameunit(sc->sc_bus.bdev));
#ifdef CONFIG_TDI_4X
            /* On TDI, after the controller is resumed, there is no way to
             * determine which port cause to the resume, so we read this before
             * we resume the controller itself */
            find_resumed_ports(sc);
#endif
            controller_usb_resume_self(sc->sc_bus.usb_wrap); 
        }

        root_hub_port_change(&(sc->sc_bus.rootsoftc));
    }

    if (sc_flags & EHCI_DOOR_BELL)
    {
        DBG_I(DHCD_INTR, ("EHCI: Door Bell\n"));
        jwakeup(sc->sleep_handle);
    }
    
    if (sc_flags & (EHCI_TRANSMIT_COMPLETE | EHCI_FL_ROLLOVER))
    {
        TAILQ_FOREACH(epipe, &sc->isoc_pipe_list, next)
        {
            ehci_isoc_check_intr(sc, epipe);
            ehci_isoc_sched(sc, epipe);
        }
    }

    if (sc_flags & EHCI_TRANSFER)
    {
        sc->sc_bus.intr_context++;

        for (ex = LIST_FIRST(&sc->sc_intrhead); ex; ex = nextex) 
        {
            nextex = LIST_NEXT(ex, inext);
            ehci_check_intr(sc, ex);
        }

        sc->sc_bus.intr_context--;
    }
}

/* Check for an interrupt. */
void ehci_check_intr(ehci_softc_t *sc, struct ehci_xfer *ex)
{
    ehci_soft_qtd_t *sqtd, *lsqtd;
    juint32_t status;

    DBG_I(DHCD_INTR, ("ehci_check_intr: ex=%p\n", ex));

    if (ex->sqtdstart == NULL) 
    {
        jprintf("ehci_check_intr: sqtdstart=NULL\n");
        return;
    }
    lsqtd = ex->sqtdend;
#ifdef DIAGNOSTIC
    if (lsqtd == NULL) 
    {
        jprintf("ehci_check_intr: sqtd==0\n");
        return;
    }
#endif
    /*
     * If the last TD is still active we need to check whether there
     * is a an error somewhere in the middle, or whether there was a
     * short packet (SPD and not ACTIVE).
     */
    if (ctoh32(lsqtd->qtd.qtd_status) & EHCI_QTD_ACTIVE) 
    {
        DBG_V(DHCD_INTR, ("ehci_check_intr: active ex=%p\n", ex));
        for (sqtd = ex->sqtdstart; sqtd != lsqtd; sqtd=sqtd->nextqtd) 
        {
            status = ctoh32(sqtd->qtd.qtd_status);
            /* If there's an active QTD the xfer isn't done. */
            if (status & EHCI_QTD_ACTIVE)
                break;
            /* Any kind of error makes the xfer done. */
            if (status & EHCI_QTD_HALTED)
                goto done;
            /* We want short packets, and it is short: it's done */
            if (EHCI_QTD_GET_BYTES(status) != 0)
            { 
                if ((ex->xfer.pipe->endpoint->edesc->bmAttributes & 
                    UE_XFERTYPE) == UE_CONTROL) 
                { 
                    /* need to wait for the status td to become not active */ 
                    break; 
                } 
                else 
                    goto done;
            }
        }
        DBG_V(DHCD_INTR, ("ehci_check_intr: ex=%p std=%p still active\n",
            ex, ex->sqtdstart));
        return;
    }
done:
    DBG_V(DHCD_INTR, ("ehci_check_intr: ex=%p done\n", ex));
    
    /* Stop timout task if pending */
    if (jtask_pending(ex->xfer.timeout_handle))
        jtask_stop(ex->xfer.timeout_handle);
    
    ehci_idone(ex);
}

void ehci_idone(struct ehci_xfer *ex)
{
    usbd_xfer_handle xfer = &ex->xfer;
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    ehci_soft_qtd_t *sqtd, *lsqtd;
    juint32_t status = 0, nstatus = 0;
    jint_t actlen;

    DBG_I(DHCD_TRANSFER, ("ehci_idone: ex=%p\n", ex));
#ifdef DIAGNOSTIC
    {
        if (ex->isdone) 
        {
#ifdef JDEBUG
            jprintf("ehci_idone: ex is done!\n   ");
            ehci_dump_exfer(ex);
#else
            jprintf("ehci_idone: ex=%p is done!\n", ex);
#endif
            return;
        }
        ex->isdone = 1;
    }
#endif

    if (xfer->status == USBD_CANCELLED || xfer->status == USBD_TIMEOUT) 
    {
        DBG_I(DHCD_TRANSFER, ("ehci_idone: aborted xfer=%p\n", xfer));
        return;
    }

    DBG_V(DHCD_TRANSFER, ("ehci_idone: xfer=%p, pipe=%p ready\n", xfer, epipe));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(ex->sqtdstart));

    /* The transfer is done, compute actual length and status. */
    lsqtd = ex->sqtdend;
    actlen = 0;
    nstatus = 0;
    status = 0;
    for (sqtd = ex->sqtdstart; sqtd != lsqtd->nextqtd; sqtd=sqtd->nextqtd) 
    {
        nstatus = ctoh32(sqtd->qtd.qtd_status);
        if (nstatus & EHCI_QTD_ACTIVE)
            break;

        status = nstatus;
        if (EHCI_QTD_GET_PID(status) != EHCI_QTD_PID_SETUP)
            actlen += sqtd->len - EHCI_QTD_GET_BYTES(status);
    }

    /* Status is used on purpose since we want the toggle of the
     * last non-active TD */
    epipe->nexttoggle = EHCI_QTD_GET_TOGGLE(status);
    DBG_X(DHCD_TRANSFER, ("ehci_idone: updating nexttoggle: %u\n",
        epipe->nexttoggle));
    
    DBG_I(DHCD_TRANSFER, ("ehci_idone: len=%ld, actlen=%d, status=0x%lx\n",
        xfer->length, actlen, status));
    xfer->actlen = actlen;
    if (status & EHCI_QTD_HALTED)
    {
#ifdef JDEBUG
        char sbuf[128];

        bitmask_snprintf((juint32_t)status,
            "\20\7HALTED\6BUFERR\5BABBLE\4XACTERR"
            "\3MISSED\1PINGSTATE", sbuf, sizeof(sbuf));

        DBG_V(DHCD_TRANSFER, ("ehci_idone: error, addr=%d, endpt=0x%02x, "
            "status 0x%s\n",
            xfer->pipe->device->address,
            xfer->pipe->endpoint->edesc->bEndpointAddress,
            sbuf));

        DBG_RUN(DHCD_TRANSFER, DL_VERBOSE, ehci_dump_sqh(epipe->sqh));
        DBG_RUN(DHCD_TRANSFER, DL_VERBOSE, ehci_dump_sqtds(ex->sqtdstart));
#endif
        /* low&full speed has an extra error flag */
        if (EHCI_QH_GET_EPS(epipe->sqh->qh.qh_endp) != EHCI_QH_SPEED_HIGH)
            status &= EHCI_QTD_STATERRS | EHCI_QTD_PINGSTATE;
        else
            status &= EHCI_QTD_STATERRS;
        if (status == 0) /* no other errors means a stall */
            xfer->status = USBD_STALLED;
        else
            xfer->status = USBD_IOERROR; /* more info XXX */
        /* XXX need to reset TT on missed microframe */
        if (status & EHCI_QTD_MISSEDMICRO) 
        {
            ehci_softc_t *sc = (ehci_softc_t *) xfer->pipe->device->bus;

            jprintf("%s: missed microframe, TT reset not "
                "implemented, hub might be inoperational\n",
                j_device_get_nameunit(sc->sc_bus.bdev));
        }
    }
    else 
    {
        xfer->status = USBD_NORMAL_COMPLETION;
    }

    ehci_xfer_complete(xfer);
    DBG_V(DHCD_TRANSFER, ("ehci_idone: ex=%p done\n", ex));
}

jint_t ehci_detach(struct ehci_softc *sc, jint_t flags)
{
    jint_t rv = 0;

    sc->sc_dying = 1;

    EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
    EOWRITE4(sc, EHCI_USBCMD, 0);
    EOWRITE4(sc, EHCI_USBCMD, EHCI_CMD_HCRESET);
    root_hub_uninit(&(sc->sc_bus.rootsoftc));

    jdelay_ms(300); /* let stray task complete */

    jfree(sc->sc_flist_soft);
    /* HSU addition: Memory leak */
    sc->sc_flist_soft = NULL;
    /* End HSU addition */

    return (rv);
}

void ehci_freemem(ehci_softc_t *sc)
{
    usbd_xfer_handle xfer;

    /* Free all xfers associated with this HC. */
    for (;;) 
    {
        xfer = TAILQ_FIRST(&sc->sc_free_xfers);
        if (xfer == NULL)
            break;
        TAILQ_REMOVE_HEAD(&sc->sc_free_xfers, next);
        jfree(xfer);
    }

    ehci_free_all_dma(sc);
    /* HSU addition: Memory leak */
    if (sc->sc_flist_soft)
    {
        jfree(sc->sc_flist_soft);
        sc->sc_flist_soft = NULL;
    }
    if (sc->sleep_handle)
    {
        jmsleep_uninit(sc->sleep_handle);
        sc->sleep_handle = NULL;
    }
    if (sc->sc_fldma)
    {
        jdma_free(sc->sc_fldma);
        sc->sc_fldma = NULL;
    }
    if (sc->sc_lock)
    {
        jspinlock_uninit(sc->sc_lock);
        sc->sc_lock = NULL;
    }
    /* End HSU addition */
}


/**
 * Function name:  ehci_port_suspend
 * Description: Suspend a port according to ehci spec, sec 4.3.1
 * Parameters: 
 *     @sc: EHCI handle
 *     Scope: local
 *    */
static jresult_t ehci_port_suspend(ehci_softc_t *sc, juint8_t port)
{
    juint32_t state;    

    DBG_V(DHCD_POWER, ("EHCI: Suspending port %d\n", port));

    /* Suspend all ports */
    state = EOREAD4(sc, EHCI_PORTSC(port));
    state &= ~EHCI_PS_CLEAR;

    /* Suspend port only if ~PORT_OWNER & PORT_ENABLE are set */
    if ((state & EHCI_PS_PO) || !(state & EHCI_PS_PE))
    {
        DBG_V(DHCD_POWER, ("EHCI: Cannot suspend, bad port state %d - %08x\n",
            port, state));
        return JEINVAL;
    }
    
    state |= EHCI_PS_WKOC_E| EHCI_PS_WKDSCNNT_E | EHCI_PS_WKCNNT_E;

    /* Suspend the port */
    EOWRITE4(sc, EHCI_PORTSC(port), state | EHCI_PS_SUSP);

    /* We must wait until host controller finishes processing the 
     * current frame, but we wait a tid longer just to make sure */
    jdelay_us(1000);

    return 0;
}

/**
 * Function name:  ehci_port_resume
 * Description: Resume all ports according to ehci spec, sec 4.3.1
 * Parameters: 
 *     @sc: EHCI handle
 *
 * Scope: local
 **/
static jresult_t ehci_port_resume(ehci_softc_t *sc, juint8_t port)
{
    juint32_t state;

    DBG_V(DHCD_POWER, ("EHCI: Starting port resume %d\n", port));

    /* We must make sure port suspend bit was up for at least 10 msec before
     * starting a resume - so we wait before resume to make sure. */
    jdelay_ms(USB_RESUME_WAIT);

    /* Start resume */
    state = EOREAD4(sc, EHCI_PORTSC(port));

    /* Resume port only if already suspended */
    if (!(state & EHCI_PS_SUSP))
    {
        DBG_I(DHCD_POWER, ("EHCI: Port %d will not be resumed, not suspended\n",
            port));
        return JEPERM;
    }

    /* Resume the port */
    EOWRITE4(sc, EHCI_PORTSC(port), state | EHCI_PS_FPR);

    /* Let the resume propogate */
    jdelay_ms(USB_RESUME_DELAY);

    /* Finish resume */
    state = EOREAD4(sc, EHCI_PORTSC(port));

    /* Check if in resume state */
#ifndef CONFIG_TDI_4X
    if (!(state & EHCI_PS_FPR))
    {
        DBG_E(DHCD_POWER, ("EHCI: Cannot resume, failed resume state port %d\n",
            port));
        return JEINVAL;
    }

    /* Finish resume sequence */
    EOWRITE4(sc, EHCI_PORTSC(port), state & ~EHCI_PS_FPR);
#endif

    return 0;
}

/*
 * Handle suspend/resume.
 */
static jresult_t ehci_suspend(usbd_bus_handle bus)
{
    ehci_softc_t *sc = (ehci_softc_t *)bus;
    juint32_t cmd, hcr = 0;
    jint_t i;
    jresult_t rc = 0;

    if (sc->sc_suspend)
    {
        DBG_E(DHCD_POWER, ("EHCI: %s Skip suspend, already suspended\n",
            j_device_get_nameunit(sc->sc_bus.bdev)));
        return 0;
    }

    DBG_I(DHCD_POWER, ("EHCI: %s Starting suspend: sc=%p\n", 
        j_device_get_nameunit(sc->sc_bus.bdev), sc));

    DBG_RUN(DHCD_POWER, DL_EX_VERBOSE, ehci_dump_regs(sc));

    sc->sc_cmd = EOREAD4(sc, EHCI_USBCMD);

    /* Stop transfers schedule */
    cmd = sc->sc_cmd & ~(EHCI_CMD_ASE | EHCI_CMD_PSE);
    EOWRITE4(sc, EHCI_USBCMD, cmd);

    for (i = 0; i < 100; i++) 
    {
        hcr = EOREAD4(sc, EHCI_USBSTS) &
            (EHCI_STS_ASS | EHCI_STS_PSS);
        if (hcr == 0)
            break;

        jdelay_ms(2);
    }

    if (hcr != 0) 
    {
        jprintf("%s: Cannot stop transfers schedules, reset timeout\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
        rc = JETIMEDOUT;
        goto Exit;
    }

#ifdef CONFIG_JOTG
    if (sc->ocd_handle)
    {
        rc = j_device_ioctl(sc->ocd_handle, DRV_IOCTL_HOST_SUSPEND, NULL);
        if (rc)
        {
            DBG_E(DHCD_POWER, ("EHCI: Failed to send HOST_SUSPEND IOCTL to "
                "OTG Core - %d\n", rc));            
        }
    }
#endif

    /* Suspend all active ports */
    for (i = 1; i <= sc->sc_noport; i++)
    {
        ehci_port_suspend(sc, i);
    }

    /* Allow only for resume and system error interrupts */
    sc->sc_eintrs |= EHCI_STS_PCD;
    EOWRITE4(sc, EHCI_USBINTR, EHCI_STS_PCD | EHCI_STS_HSE);

    /* Stop controller */
    cmd &= ~EHCI_CMD_RS;
    EOWRITE4(sc, EHCI_USBCMD, cmd);

    for (i = 0; i < 100; i++) 
    {
        hcr = EOREAD4(sc, EHCI_USBSTS) & EHCI_STS_HCH;
        if (hcr == EHCI_STS_HCH)
            break;

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
    {
        DBG_V(DHCD_POWER, ("EHCI: Host was disabled during port suspend\n"));
        sc->sc_suspend = TRUE;
        rc = 0;
        goto Exit;
    }
#endif

        jdelay_ms(2);
    }
    if (hcr != EHCI_STS_HCH) 
    {
        jprintf("%s: Cannot stop controller, config timeout\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
        rc = JETIMEDOUT;
        goto Exit;
    }

    sc->sc_suspend = TRUE;

    DBG_I(DHCD_POWER, ("EHCI: [%s] Controller suspended\n",
        j_device_get_nameunit(sc->sc_bus.bdev)));

Exit:
    return rc;
}

static jresult_t ehci_resume(usbd_bus_handle bus)
{
    ehci_softc_t *sc = (ehci_softc_t *)bus;
    juint32_t hcr = 0;
    juint32_t v;
    jint_t i;
    jresult_t rc = 0;
    juint_t resumed_ports = 0;

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
    {
        DBG_W(DHCD_POWER, ("EHCI: Trying to resume controller while host is "
            "not enabled\n"));
        sc->sc_suspend = FALSE;
        return 0;
    }
#endif

    if (!sc->sc_suspend)
    {
        DBG_E(DHCD_POWER, ("EHCI: %s Skip resume, already resumed\n",
            j_device_get_nameunit(sc->sc_bus.bdev)));
        return 0;
    }

    DBG_I(DHCD_POWER, ("EHCI: Controller resume started %p\n", sc));
    DBG_RUN(DHCD_POWER, DL_EX_VERBOSE, ehci_dump_regs(sc));

    /* restore things in case the bios sucks */
    EOWRITE4(sc, EHCI_CTRLDSSEGMENT, 0);
    EOWRITE4(sc, EHCI_PERIODICLISTBASE, DMAADDR(&sc->sc_fldma, 0));
    EOWRITE4(sc, EHCI_ASYNCLISTADDR,
        sc->sc_async_head->physaddr | EHCI_LINK_QH);

    /* Turn ON the controller */
    EOWRITE4(sc, EHCI_USBCMD, sc->sc_cmd & ~(EHCI_CMD_ASE | EHCI_CMD_PSE));

    for (i = 0; i < 100; i++) 
    {
        hcr = EOREAD4(sc, EHCI_USBSTS) & EHCI_STS_HCH;
        if (hcr != EHCI_STS_HCH)
            break;

        jdelay_ms(2);
    }

    if (hcr == EHCI_STS_HCH) 
    {
        jprintf("%s: config timeout\n",
            j_device_get_nameunit(sc->sc_bus.bdev));
        rc = JETIMEDOUT;
        goto Exit;
    }

    sc->sc_suspend = FALSE;

    find_resumed_ports(sc);

    /* Resume the ports */
    for (i = 1; i <= sc->sc_noport; i++)
    {
        v = EOREAD4(sc, EHCI_PORTSC(i));

        v &= ~(EHCI_PS_CLEAR | EHCI_PS_WKOC_E | 
            EHCI_PS_WKDSCNNT_E| EHCI_PS_WKCNNT_E);
    
        if (v & EHCI_PS_SUSP)
        {
            resumed_ports++;
            v |= EHCI_PS_FPR;
        }

        EOWRITE4(sc, EHCI_PORTSC(i), v);
    }

    if (!resumed_ports)
        goto Turn_On;    

    /* Let the ports resume */
#ifdef CONFIG_JOTG
    if (sc->ocd_handle)
    {
        jdelay_ms(USB_RESUME_DELAY_OTG);
    }
    else
#endif
    {
        jdelay_ms(USB_RESUME_DELAY);
    }

    for (i = 1; i <= sc->sc_noport; i++)
    {
        v = EOREAD4(sc, EHCI_PORTSC(i));

        if (!(v & EHCI_PS_SUSP))
            continue;

        v &= ~EHCI_PS_CLEAR;

        /* On TDI_4X, the FPR bits in the port status register is cleared by
         * hardware */
#ifndef CONFIG_TDI_4X
        v &= ~EHCI_PS_FPR;
#endif

        EOWRITE4(sc, EHCI_PORTSC(i), v);

        DBG_V(DHCD_POWER, ("EHCI: Resumed port %d\n", i));
    }

    /* EHCI host has up to 2ms to transition port from Resume to Normal
     * after the FPR (above) is cleared */
    jdelay_ms(10);

Turn_On:
    /* Turn ON the schedules */
    EOWRITE4(sc, EHCI_USBCMD, sc->sc_cmd);

    /* Turn on interrupts */
    EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);

#ifdef CONFIG_JOTG 
    if (sc->ocd_handle)
    {
        rc = j_device_ioctl(sc->ocd_handle, DRV_IOCTL_HOST_RESUME, NULL);
        if (rc)
        {
            DBG_E(DHCD_POWER, ("EHCI: Failed to send HOST_RESUME IOCTL to "
                "OTG Core - %d\n", rc));            
        }
    }
#endif

    DBG_I(DHCD_POWER, ("EHCI: [%s] Controller resumed\n",
        j_device_get_nameunit(sc->sc_bus.bdev)));
Exit:
    return rc;
}

static void find_resumed_ports(ehci_softc_t *sc)
{
    jint_t i;
    juint32_t v;

    /* Check which ports triggered the remote wakeup */
    for (i = 1; i <= sc->sc_noport; i++)
    {
        /* Re-Read port status, the suspend bit should of changed */
        v = EOREAD4(sc, EHCI_PORTSC(i));

        if (!(v & EHCI_PS_FPR))
            continue;
        
        if (!(v & EHCI_PS_SUSP))
            continue;

        DBG_I(DHCD_POWER, ("EHCI: Remote wakeup on port %d\n", i + 1));

        sc->sc_port_flags[i] |= EHCI_PORT_IS_AWAKEN;
    }
}

/*
 * Shut down the controller when the system is going down.
 */
void ehci_shutdown(void *v)
{
    ehci_softc_t *sc = v;

    DBG_W(DHCD_INIT, ("ehci_shutdown: stopping the HC\n"));
    EOWRITE4(sc, EHCI_USBCMD, 0);       /* Halt controller */
    EOWRITE4(sc, EHCI_USBCMD, EHCI_CMD_HCRESET);
}

usbd_status ehci_allocm(struct usbd_bus *bus, jdma_handle *dma, juint32_t size)
{
    jresult_t err;

    err = jdma_alloc(size, 0, 0, 0, M_CACHABLE, dma);
#ifdef JDEBUG
    if (err)
        DBG_W(DHCD_TRANSFER, ("ehci_allocm: jdma_alloc()=%d\n", err));
#endif
    return (err ? USBD_NOMEM : USBD_NORMAL_COMPLETION);
}

void ehci_freem(struct usbd_bus *bus, jdma_handle *dma)
{
    jdma_free(*dma);
}

usbd_xfer_handle ehci_allocx(struct usbd_bus *bus)
{
    struct ehci_softc *sc = (struct ehci_softc *)bus;
    usbd_xfer_handle xfer;

    xfer = TAILQ_FIRST(&sc->sc_free_xfers);
    if (xfer != NULL) 
    {
        TAILQ_REMOVE_HEAD(&sc->sc_free_xfers, next);

        j_memset(xfer, 0, sizeof(struct ehci_xfer));

#ifdef DIAGNOSTIC
        if (xfer->busy_free != XFER_FREE) 
        {
            jprintf("ehci_allocx: xfer=%p not free, 0x%08x\n", xfer,
                xfer->busy_free);
        }
#endif
    } 
    else
    {
        xfer = jmalloc(sizeof(struct ehci_xfer), M_ZERO);
    }
#ifdef DIAGNOSTIC
    if (xfer != NULL) 
    {
        EXFER(xfer)->isdone = 1;
        xfer->busy_free = XFER_BUSY;
    }
#endif
    return (xfer);
}

void ehci_freex(struct usbd_bus *bus, usbd_xfer_handle xfer)
{
    struct ehci_softc *sc = (struct ehci_softc *)bus;

#ifdef DIAGNOSTIC
    if (xfer->busy_free != XFER_BUSY) 
    {
        jprintf("ehci_freex: xfer=%p not busy, 0x%08x\n", xfer,
            xfer->busy_free);
        return;
    }
    xfer->busy_free = XFER_FREE;
    if (!EXFER(xfer)->isdone) 
    {
        jprintf("ehci_freex: !isdone\n");
        return;
    }
#endif
    TAILQ_INSERT_TAIL(&sc->sc_free_xfers, xfer, next);
}

jbool_t ehci_is_active(struct usbd_bus *bus)
{
    struct ehci_softc *sc = (struct ehci_softc *)bus;
    return (!sc->sc_dying);
}

#ifdef CONFIG_JOTG
static jbool_t ehci_is_otg_host_enabled(struct usbd_bus *bus)
{
    struct ehci_softc *sc = (struct ehci_softc *)bus;

    return (!sc->ocd_handle || sc->otg_host_enabled)? 1 : 0;
}
#endif

static void ehci_device_clear_toggle(usbd_pipe_handle pipe)
{
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;

    DBG_V(DHCD_TRANSFER, ("ehci_device_clear_toggle: epipe=%p status=0x%x\n",
        epipe, epipe->sqh->qh.qh_qtd.qtd_status));
    
    DBG_RUN(DHCD_TRANSFER, DL_VERBOSE, usbd_dump_pipe(pipe));
    epipe->nexttoggle = 0;
}

static void ehci_noop(usbd_pipe_handle pipe)
{
}

#ifdef JDEBUG
void ehci_dump_regs(ehci_softc_t *sc)
{
    jint_t i;
    jprintf("cmd=0x%08x, sts=0x%08x, ien=0x%08x\n",
        EOREAD4(sc, EHCI_USBCMD),
        EOREAD4(sc, EHCI_USBSTS),
        EOREAD4(sc, EHCI_USBINTR));
    jprintf("frindex=0x%08x ctrdsegm=0x%08x periodic=0x%08x async=0x%08x\n",
        EOREAD4(sc, EHCI_FRINDEX),
        EOREAD4(sc, EHCI_CTRLDSSEGMENT),
        EOREAD4(sc, EHCI_PERIODICLISTBASE),
        EOREAD4(sc, EHCI_ASYNCLISTADDR));
    for (i = 1; i <= sc->sc_noport; i++)
        jprintf("port %d status=0x%08x\n", i, EOREAD4(sc, EHCI_PORTSC(i)));
}

/*
 * Unused function - this is meant to be called from a kernel
 * debugger.
 */
void ehci_dump()
{
    ehci_dump_regs(theehci);
}

void ehci_dump_link(ehci_link_t link, jint_t type)
{
    link = ctoh32(link);
    jprintf("0x%08x", link);
    if (link & EHCI_LINK_TERMINATE)
        jprintf("<T>");
    else
    {
        jprintf("<");
        if (type) 
        {
            switch (EHCI_LINK_TYPE(link)) 
            {
            case EHCI_LINK_ITD:
                jprintf("ITD"); 
                break;
            case EHCI_LINK_QH:
                jprintf("QH");
                break;
            case EHCI_LINK_SITD: 
                jprintf("SITD"); 
                break;
            case EHCI_LINK_FSTN: 
                jprintf("FSTN");
                break;
            }
        }
        jprintf(">");
    }
}

void ehci_dump_sqtds(ehci_soft_qtd_t *sqtd)
{
    jint_t i;
    juint32_t stop;

    stop = 0;
    for (i = 0; sqtd && i < 20 && !stop; sqtd = sqtd->nextqtd, i++) 
    {
        ehci_dump_sqtd(sqtd);
        stop = sqtd->qtd.qtd_next & htoc32(EHCI_LINK_TERMINATE);
    }
    if (sqtd)
        jprintf("dump aborted, too many TDs\n");
}

void ehci_dump_sqtd(ehci_soft_qtd_t *sqtd)
{
    jprintf("QTD(%p) at 0x%08x:\n", sqtd, sqtd->physaddr);
    ehci_dump_qtd(&sqtd->qtd);
}

void ehci_dump_qtd(ehci_qtd_t *qtd)
{
    juint32_t s;
    char sbuf[128];

    jprintf("  next="); ehci_dump_link(qtd->qtd_next, 0);
    jprintf(" altnext="); ehci_dump_link(qtd->qtd_altnext, 0);
    jprintf("\n");
    s = ctoh32(qtd->qtd_status);
    bitmask_snprintf(EHCI_QTD_GET_STATUS(s),
        "\20\10ACTIVE\7HALTED\6BUFERR\5BABBLE\4XACTERR"
        "\3MISSED\2SPLIT\1PING", sbuf, sizeof(sbuf));
    jprintf("  status=0x%08x: toggle=%d bytes=0x%x ioc=%d c_page=0x%x\n",
        s, EHCI_QTD_GET_TOGGLE(s), EHCI_QTD_GET_BYTES(s),
        EHCI_QTD_GET_IOC(s), EHCI_QTD_GET_C_PAGE(s));
    jprintf("    cerr=%d pid=%d stat=0x%s\n", EHCI_QTD_GET_CERR(s),
        EHCI_QTD_GET_PID(s), sbuf);
    for (s = 0; s < 5; s++)
        jprintf("  buffer[%d]=0x%08x\n", s, ctoh32(qtd->qtd_buffer[s]));
}

void ehci_dump_sqh(ehci_soft_qh_t *sqh)
{
    ehci_qh_t *qh = &sqh->qh;
    juint32_t endp, endphub;

    jprintf("QH(%p) at 0x%08x:\n", sqh, sqh->physaddr);
    jprintf("  link="); ehci_dump_link(qh->qh_link, 1); jprintf("\n");
    endp = ctoh32(qh->qh_endp);
    jprintf("  endp=0x%08x\n", endp);
    jprintf("    addr=0x%02x inact=%d endpt=%d eps=%d dtc=%d hrecl=%d\n",
        EHCI_QH_GET_ADDR(endp), EHCI_QH_GET_INACT(endp),
        EHCI_QH_GET_ENDPT(endp),  EHCI_QH_GET_EPS(endp),
        EHCI_QH_GET_DTC(endp), EHCI_QH_GET_HRECL(endp));
    jprintf("    mpl=0x%x ctl=%d nrl=%d\n",
        EHCI_QH_GET_MPL(endp), EHCI_QH_GET_CTL(endp),
        EHCI_QH_GET_NRL(endp));
    endphub = ctoh32(qh->qh_endphub);
    jprintf("  endphub=0x%08x\n", endphub);
    jprintf("    smask=0x%02x cmask=0x%02x huba=0x%02x port=%d mult=%d\n",
        EHCI_QH_GET_SMASK(endphub), EHCI_QH_GET_CMASK(endphub),
        EHCI_QH_GET_HUBA(endphub), EHCI_QH_GET_PORT(endphub),
        EHCI_QH_GET_MULT(endphub));
    jprintf("  curqtd="); ehci_dump_link(qh->qh_curqtd, 0); jprintf("\n");
    jprintf("Overlay qTD:\n");
    ehci_dump_qtd(&qh->qh_qtd);
}

#ifdef DIAGNOSTIC
static void ehci_dump_exfer(struct ehci_xfer *ex)
{
    jprintf("ehci_dump_exfer: ex=%p\n", ex);
}
#endif
#endif

usbd_status ehci_open(usbd_pipe_handle pipe)
{
    usbd_device_handle dev = pipe->device;
    ehci_softc_t *sc = (ehci_softc_t *)dev->bus;
    usb_endpoint_descriptor_t *ed = pipe->endpoint->edesc;
    juint8_t  addr = dev->address;
    juint8_t  xfertype = ed->bmAttributes & UE_XFERTYPE;
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;
    ehci_soft_qh_t *sqh;
    jresult_t rc;
    jint_t ival, speed = 0, naks;
    jint_t hshubaddr, hshubport;

    DBG_I(DHCD_TRANSFER, ("ehci_open: pipe=%p, addr=%d, endpt=%d (%d)\n",
        pipe, addr, ed->bEndpointAddress, sc->sc_addr));

    if (dev->myhighhub) 
    {
        hshubaddr = dev->myhighhub->address;
        hshubport = dev->myhighport;
    }
    else
    {
        hshubaddr = 0;
        hshubport = 0;
    }

    if (sc->sc_dying)
        return (USBD_IOERROR);

    epipe->nexttoggle = 0;

    switch (dev->speed) 
    {
    case USB_SPEED_LOW: 
        speed = EHCI_QH_SPEED_LOW;
        break;
    case USB_SPEED_FULL:
        speed = EHCI_QH_SPEED_FULL; 
        break;
    case USB_SPEED_HIGH:
        speed = EHCI_QH_SPEED_HIGH;
        break;
    default:
        j_panic("ehci_open: bad device speed %d", dev->speed);
    }
    naks = 8;
    sqh = ehci_alloc_sqh(sc);
    if (sqh == NULL)
        goto bad0;
    /* qh_link filled when the QH is added */
    sqh->qh.qh_endp = htoc32(
        EHCI_QH_SET_ADDR(addr) |
        EHCI_QH_SET_ENDPT(UE_GET_ADDR(ed->bEndpointAddress)) |
        EHCI_QH_SET_EPS(speed) |
        EHCI_QH_DTC |
        EHCI_QH_SET_MPL(UGETW(ed->wMaxPacketSize)) |
        (speed != EHCI_QH_SPEED_HIGH && xfertype == UE_CONTROL ?
        EHCI_QH_CTL : 0) |
        EHCI_QH_SET_NRL(naks));
    sqh->qh.qh_endphub = htoc32(
        EHCI_QH_SET_MULT(1) |
        EHCI_QH_SET_HUBA(hshubaddr) |
        EHCI_QH_SET_PORT(hshubport) |
        /* HSU addition: value changed to 0x18 instead of 0x08
        for a 2nd CSplit incase of MDATA sent, for intterupt endpoint*/
        EHCI_QH_SET_CMASK(0x18) | /* XXX */        
        EHCI_QH_SET_SMASK(xfertype == UE_INTERRUPT ? 0x02 : 0));
    sqh->qh.qh_curqtd = EHCI_NULL;
    /* Fill the overlay qTD */
    sqh->qh.qh_qtd.qtd_next = EHCI_NULL;
    sqh->qh.qh_qtd.qtd_altnext = EHCI_NULL;
    sqh->qh.qh_qtd.qtd_status = htoc32(0);

    epipe->sqh = sqh;

    switch (xfertype)
    {
    case UE_CONTROL:
        rc = jdma_alloc(sizeof(usb_device_request_t), 0, 0, 0, 0, 
            &epipe->u.ctl.reqdma);
#ifdef JDEBUG
        if (rc)
            DBG_W(DHCD_TRANSFER, ("ehci_open: jdma_alloc()=%d\n", rc));
#endif
        if (rc)
            goto bad1;
        pipe->methods = &ehci_device_ctrl_methods;
        ehci_add_qh(sqh, sc->sc_async_head);
        break;
    case UE_BULK:
        pipe->methods = &ehci_device_bulk_methods;
        ehci_add_qh(sqh, sc->sc_async_head);
        break;
    case UE_INTERRUPT:
        pipe->methods = &ehci_device_intr_methods;
        ival = pipe->interval;
        if (ival == USBD_DEFAULT_INTERVAL)
            ival = ed->bInterval;
        if (dev->speed == USB_SPEED_HIGH)
        {
            ival = (1 << (ival-1)) / 8; /* microframes to frames*/
            if (!ival) 
                ival = 1; /* The minimum ehci poll rate is 1 frame*/
        }
        return (ehci_device_setintr(sc, sqh, ival));
    case UE_ISOCHRONOUS:
        ival = pipe->interval;
        if (ival == USBD_DEFAULT_INTERVAL)
            ival = ed->bInterval;
        pipe->methods = &ehci_device_isoc_methods;

#ifdef CONFIG_TDI_4X
        if (hshubaddr == 1)
            hshubaddr = 0;
#endif
        return ehci_device_set_isoc(sc, pipe, (juint16_t)ival, hshubport,
            hshubaddr);
    default:
        return (USBD_INVAL);
    }
    return (USBD_NORMAL_COMPLETION);

bad1:
    ehci_free_sqh(sc, sqh);
bad0:
    return (USBD_NOMEM);
}

/*
 * Add an ED to the schedule.
 */
void ehci_add_qh(ehci_soft_qh_t *sqh, ehci_soft_qh_t *head)
{
    sqh->next = head->next;
    sqh->qh.qh_link = head->qh.qh_link;
    head->next = sqh;
    head->qh.qh_link = htoc32(sqh->physaddr | EHCI_LINK_QH);

    DBG_X(DHCD_TRANSFER, ("ehci_add_qh:\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
}

/*
 * Remove an ED from the schedule.
 */
void ehci_rem_qh(ehci_softc_t *sc, ehci_soft_qh_t *sqh, ehci_soft_qh_t *head)
{
    ehci_soft_qh_t *p;

    for (p = head; p != NULL && p->next != sqh; p = p->next)
        ;
    if (p == NULL)
        j_panic("ehci_rem_qh: ED not found");
    p->next = sqh->next;
    p->qh.qh_link = sqh->qh.qh_link;

    ehci_sync_hc(sc);
}

void ehci_set_qh_qtd(ehci_soft_qh_t *sqh, ehci_soft_qtd_t *sqtd)
{
    jint_t i;
    juint32_t status;

    /* Save toggle bit and ping status. */
    status = sqh->qh.qh_qtd.qtd_status &
        htoc32(EHCI_QTD_TOGGLE_MASK |
            EHCI_QTD_SET_STATUS(EHCI_QTD_PINGSTATE));
    /* Set HALTED to make hw leave it alone. */
    sqh->qh.qh_qtd.qtd_status =
        htoc32(EHCI_QTD_SET_STATUS(EHCI_QTD_HALTED));
    sqh->qh.qh_curqtd = 0;
    sqh->qh.qh_qtd.qtd_next = htoc32(sqtd->physaddr);
    sqh->qh.qh_qtd.qtd_altnext = 0;
    for (i = 0; i < EHCI_QTD_NBUFFERS; i++)
        sqh->qh.qh_qtd.qtd_buffer[i] = 0;
    sqh->sqtd = sqtd;
    /* Set !HALTED && !ACTIVE to start execution, preserve some fields */
    sqh->qh.qh_qtd.qtd_status = status;
}

/*
 * Ensure that the HC has released all references to the QH.  We do this
 * by asking for a Async Advance Doorbell interrupt and then we wait for
 * the interrupt.
 * To make this easier we first obtain exclusive use of the doorbell.
 */
void ehci_sync_hc(ehci_softc_t *sc)
{
    jresult_t error;

    if (sc->sc_dying) 
    {
        DBG_W(DHCD_TRANSFER, ("ehci_sync_hc: dying\n"));
        return;
    }

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
        return;
#endif

    DBG_X(DHCD_TRANSFER, ("ehci_sync_hc: enter\n"));

    /* ask for doorbell */
    EOWRITE4(sc, EHCI_USBCMD, EOREAD4(sc, EHCI_USBCMD) | EHCI_CMD_IAAD);
    DBG_V(DHCD_TRANSFER, ("ehci_sync_hc: cmd=0x%08x sts=0x%08x\n",
        EOREAD4(sc, EHCI_USBCMD), EOREAD4(sc, EHCI_USBSTS)));
    error = jmsleep(sc->sleep_handle, 1000); /* bell wait */
    UNUSED_VAR(error);
    DBG_V(DHCD_TRANSFER, ("ehci_sync_hc: cmd=0x%08x sts=0x%08x\n",
        EOREAD4(sc, EHCI_USBCMD), EOREAD4(sc, EHCI_USBSTS)));

#ifdef DIAGNOSTIC
    if (error)
        jprintf("ehci_sync_hc: jmsleep() = %d\n", error);
#endif
    DBG_X(DHCD_TRANSFER, ("ehci_sync_hc: exit\n"));
}

/***********/

/*
 * Simulate a hardware hub by handling all the necessary requests.
 */

usbd_status ehci_do_port_change(void *context, juint8_t index, jint_t value,
    jbool_t set)
{
    ehci_softc_t *sc = (ehci_softc_t *)context;
    juint32_t v;
    jint_t port, wait_time;
    jresult_t rc;
    usbd_status err = USBD_NORMAL_COMPLETION;

    DBG_X(DHCD_HUB, ("EHCI: Port %s index=%d val=%d\n", set?"set":"clear",
        index, value));

    port = EHCI_PORTSC(index);
    v    = EOREAD4(sc, port) &~ EHCI_PS_CLEAR;

    if (set)
    {
        switch(value) 
        {
        case UHF_PORT_ENABLE:
            EOWRITE4(sc, port, v | EHCI_PS_PE);
            break;
        case UHF_PORT_SUSPEND:
            if (ehci_port_suspend(sc, index))
                err = USBD_INVAL;
            break;
        case UHF_PORT_POWER:
            EOWRITE4(sc, port, v | EHCI_PS_PP);
            break;
        case UHF_PORT_RESET:

            DBG_V(DHCD_HUB, ("EHCI: Starting port reset %d\n", index));

            #ifndef NO_COMPANION
                if (EHCI_PS_IS_LOWSPEED(v)) 
                {
                    /* Low speed device, give up ownership. */
                    ehci_disown(sc, index, 1);
                    DBG_I(DHCD_HUB, ("EHCI: Giving up ownership %d\n", index));
                    break;
                }
            #endif
            /* Start reset sequence. */
            v &= ~ (EHCI_PS_PE | EHCI_PS_PR);
            EOWRITE4(sc, port, v | EHCI_PS_PR);
            
            /* Wait for reset to complete. */
            for (wait_time = USB_PORT_ROOT_RESET_DELAY; 
                (wait_time > 0) && (EOREAD4(sc, port) & EHCI_PS_PR);
                wait_time -= USB_PORT_ROOT_RESET_DELAY_SHORT)
            {
                jdelay_ms(USB_PORT_ROOT_RESET_DELAY_SHORT);
            }

            if (sc->sc_dying) 
            {
                err = USBD_IOERROR;
                DBG_W(DHCD_HUB, ("EHCI: Reset failed, controller dying\n"));
                break;
            }
#ifndef CONFIG_TDI_4X
            /* Terminate reset sequence. */
            EOWRITE4(sc, port, v);
#endif
            /* Wait for HC to complete reset. */
            jdelay_ms(EHCI_PORT_RESET_COMPLETE);
            if (sc->sc_dying) 
            {
                DBG_W(DHCD_HUB, ("EHCI: Reset failed, controller dying\n"));
                err = USBD_IOERROR;
                break;
            }
            v = EOREAD4(sc, port);
            DBG_V(DHCD_INIT, ("ehci after reset, status=0x%08x\n", v));
            if (v & EHCI_PS_PR) 
            {
                jprintf("%s: port reset timeout\n",
                    j_device_get_nameunit(sc->sc_bus.bdev));
                err = USBD_TIMEOUT;
                break;
            }

            #ifndef NO_COMPANION
            if (!(v & EHCI_PS_PE)) 
            {
                /* Not a high speed device, give up ownership.*/
                ehci_disown(sc, index, 0);
                DBG_I(DHCD_HUB, ("EHCI: Reset failed, not high speed\n"));
                break;
            }
            #endif
            DBG_V(DHCD_INIT, ("ehci port %d reset, status = 0x%08x\n",
                index, v));

            DBG_V(DHCD_HUB, ("EHCI: Reset for port %d finished\n", index));
            sc->sc_port_flags[index] |= EHCI_PORT_IS_RESET;
#if defined(CONFIG_TDI_4X) && !defined(CONFIG_CX2417X)
            /* We should delay only if not part of OTG */
#ifdef CONFIG_JOTG
            if (!sc->ocd_handle)
#endif
            {
                jdelay_ms(2010); /* TODO: Work around for cold boot issue*/
            }
#endif
            break;
        }
    }
    else
    {
        switch(value) 
        {
        case UHF_PORT_ENABLE:
            EOWRITE4(sc, port, v &~ EHCI_PS_PE);
            break;
        case UHF_PORT_SUSPEND:
            rc = ehci_port_resume(sc, index);
            /* Global resume on EHCI will mark all ports as resumed,
             * so if the get JEPERM the port was already resumed and is ready */
            if (rc && rc != JEPERM)
                err = USBD_INVAL;
            break;
        case UHF_PORT_POWER:
            EOWRITE4(sc, port, v &~ EHCI_PS_PP);
            break;
        case UHF_PORT_TEST:
            DBG_I(DHCD_HUB, ("ehci_root_ctrl_transfer: clear port test "
                "%d\n", index));
            break;
        case UHF_PORT_INDICATOR:
            DBG_I(DHCD_HUB, ("ehci_root_ctrl_transfer: clear port ind "
                "%d\n", index));
            EOWRITE4(sc, port, v &~ EHCI_PS_PIC);
            break;
        case UHF_C_PORT_CONNECTION:
            EOWRITE4(sc, port, v | EHCI_PS_CSC);
            break;
        case UHF_C_PORT_ENABLE:
            EOWRITE4(sc, port, v | EHCI_PS_PEC);
            break;
        case UHF_C_PORT_SUSPEND:
            sc->sc_port_flags[index] &= ~EHCI_PORT_IS_AWAKEN;
            break;
        case UHF_C_PORT_OVER_CURRENT:
            EOWRITE4(sc, port, v | EHCI_PS_OCC);
            break;
        case UHF_C_PORT_RESET:
            sc->sc_port_flags[index] &= ~EHCI_PORT_IS_RESET;
            break;
        default:
            err = USBD_IOERROR;
            break;
        }
    }

    DBG_X(DHCD_HUB, ("EHCI: Port change finished with %d\n", err));
    return err;
}

static juint16_t ehci_get_port_status(void *context, juint8_t index)
{
    ehci_softc_t *sc = (ehci_softc_t *)context;
    juint16_t status = 0;
    juint32_t v;

    v = EOREAD4(sc, EHCI_PORTSC(index));
#ifdef CONFIG_TDI_4X
    if (TDI_4X_IS_HISPD(v))     
        status |= UPS_HIGH_SPEED;
    else
    { 
        if (TDI_4X_IS_LOWSPD(v)) 
            status |= UPS_LOW_SPEED;
    }
#else
    status = UPS_HIGH_SPEED;
#endif
    if (v & EHCI_PS_CS)
        status |= UPS_CURRENT_CONNECT_STATUS;
    if (v & EHCI_PS_PE)
        status |= UPS_PORT_ENABLED;
    if (v & EHCI_PS_SUSP)
        status |= UPS_SUSPEND;
    if (v & EHCI_PS_OCA)
        status |= UPS_OVERCURRENT_INDICATOR;
    if (v & EHCI_PS_PR)
        status |= UPS_RESET;
    if (v & EHCI_PS_PP)
        status |= UPS_PORT_POWER;
    
    DBG_X(DHCD_HUB, ("EHCI: Port status index=%d staus=%.4x\n", index, status));
    
    return status;
}

static juint16_t ehci_get_port_change(void *context, juint8_t index)
{
    ehci_softc_t *sc = (ehci_softc_t *)context;
    juint16_t status = 0;
    juint32_t v;

    v = EOREAD4(sc, EHCI_PORTSC(index));
    if (v & EHCI_PS_CSC)
        status |= UPS_C_CONNECT_STATUS;
    if (v & EHCI_PS_PEC)
        status |= UPS_C_PORT_ENABLED;
    if (v & EHCI_PS_OCC)
    {
        status |= UPS_C_OVERCURRENT_INDICATOR;
        /* Port Power Control affect the Port Power bit in PORTSC */
        if (EHCI_HCS_PPC(EREAD4(sc, EHCI_HCSPARAMS)))
        {
            /* 
             * according to ehci specification the Port Power (PP)
             * bit may be transitioned by the host controller from 1 to 0.
             * in the cases where the ehci controller does not perfrom this
             * transition we do it manually. the other bits in the PORTSC 
             * fields that are also turned off are write clear bits.
             * */
            EOWRITE4(sc, EHCI_PORTSC(index),
                v & ~(EHCI_PS_CSC | EHCI_PS_PEC | EHCI_PS_OCC | EHCI_PS_PP) );
        }
    }
    if (sc->sc_port_flags[index] & EHCI_PORT_IS_RESET)
        status |= UPS_C_PORT_RESET;

    if (sc->sc_port_flags[index] & EHCI_PORT_IS_AWAKEN)
        status |= UPS_C_SUSPEND;

    DBG_X(DHCD_HUB, ("EHCI: Get port change %d - %.4x - %p\n",
        index, status, sc));

    return status;
}

static jint_t ehci_port_interrupt_set(void *context, jbool_t enable)
{
    ehci_softc_t *sc = (ehci_softc_t *)context;

    if (enable)
        sc->sc_eintrs |= EHCI_STS_PCD;
    else
        sc->sc_eintrs &= ~EHCI_STS_PCD;

    EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
    return 0;
}

#ifndef NO_COMPANION
void ehci_disown(ehci_softc_t *sc, jint_t index, jint_t lowspeed)
{
    jint_t port;
    juint32_t v;

    DBG_I(DHCD_HUB, ("ehci_disown: index=%d lowspeed=%d\n", index, lowspeed));
#ifdef DIAGNOSTIC
    if (sc->sc_npcomp != 0) 
    {
        jint_t i = (index-1) / sc->sc_npcomp;
        if (i >= sc->sc_ncomp)
        {
            jprintf("%s: strange port\n",
                j_device_get_nameunit(sc->sc_bus.bdev));
        }
        else
        {
            jprintf("%s: handing over %s speed device on "
                "port %d to %s\n",
                j_device_get_nameunit(sc->sc_bus.bdev),
                lowspeed ? "low" : "full",
                index, j_device_get_nameunit(sc->sc_comps[i]->bdev));
        }
    } 
    else
    {
        jprintf("%s: npcomp == 0\n", j_device_get_nameunit(sc->sc_bus.bdev));
    }
#endif
    port = EHCI_PORTSC(index);
    v = EOREAD4(sc, port) &~ EHCI_PS_CLEAR;
    EOWRITE4(sc, port, v | EHCI_PS_PO);
}
#endif
/************************/

ehci_soft_qh_t *ehci_alloc_sqh(ehci_softc_t *sc)
{
    ehci_soft_qh_t *sqh;
    jresult_t err;
    jint_t i, offs;
    ehci_dma_allocs_t *dmaw;

    if (sc->sc_freeqhs == NULL) 
    {
        DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqh: allocating chunk\n"));

        dmaw = jmalloc(sizeof(ehci_dma_allocs_t), 0);
        if (!dmaw)
            return (0);

        err = jdma_alloc(EHCI_SQH_SIZE * EHCI_SQH_CHUNK,
            32, 0, 0, M_PAGE_ALIGN, &dmaw->dma);
#ifdef JDEBUG
        if (err)
            DBG_W(DHCD_TRANSFER, ("ehci_alloc_sqh: jdma_alloc()=%d\n", err));
#endif
        if (err)
        /* HSU addition: Memory leak */
        {
            jfree(dmaw);
            return (NULL);
        }
        /* End HSU addition */

        LIST_INSERT_HEAD(&(sc->sc_dma_allocs), (dmaw), list);

        for(i = 0; i < EHCI_SQH_CHUNK; i++) 
        {
            offs = i * EHCI_SQH_SIZE;
            sqh = KERNADDR(&dmaw->dma, offs);
            sqh->physaddr = DMAADDR(&dmaw->dma, offs);
            sqh->next = sc->sc_freeqhs;
            sc->sc_freeqhs = sqh;
        }
    }
    sqh = sc->sc_freeqhs;
    sc->sc_freeqhs = sqh->next;
    j_memset(&sqh->qh, 0, sizeof(ehci_qh_t));
    sqh->next = NULL;
    return (sqh);
}

void ehci_free_sqh(ehci_softc_t *sc, ehci_soft_qh_t *sqh)
{
    sqh->next = sc->sc_freeqhs;
    sc->sc_freeqhs = sqh;
}

ehci_soft_qtd_t *ehci_alloc_sqtd(ehci_softc_t *sc)
{
    ehci_soft_qtd_t *sqtd;
    jresult_t err;
    jint_t i, offs;
    ehci_dma_allocs_t *dmaw;

    if (sc->sc_freeqtds == NULL) 
    {
        DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd: allocating chunk\n"));

        dmaw = jmalloc(sizeof(ehci_dma_allocs_t), 0);
        if (!dmaw)
            goto Error;

        err = jdma_alloc(EHCI_SQTD_SIZE*EHCI_SQTD_CHUNK,
            32, 0, 0, M_PAGE_ALIGN, &dmaw->dma);
#ifdef JDEBUG
        if (err)
            DBG_W(DHCD_TRANSFER, ("ehci_alloc_sqtd: jdma_alloc()=%d\n", err));
#endif
        if (err)
            goto Error;

        LIST_INSERT_HEAD(&(sc->sc_dma_allocs), (dmaw), list);

        for(i = 0; i < EHCI_SQTD_CHUNK; i++) 
        {
            offs = i * EHCI_SQTD_SIZE;
            sqtd = KERNADDR(&dmaw->dma, offs);
            sqtd->physaddr = DMAADDR(&dmaw->dma, offs);
            sqtd->nextqtd = sc->sc_freeqtds;
            sc->sc_freeqtds = sqtd;
        }
    }

    sqtd = sc->sc_freeqtds;
    sc->sc_freeqtds = sqtd->nextqtd;
    j_memset(&sqtd->qtd, 0, sizeof(ehci_qtd_t));
    sqtd->nextqtd = NULL;
    sqtd->xfer = NULL;

    return (sqtd);
Error:
    if (dmaw)
        jfree(dmaw);
    return NULL;
}

void ehci_free_sqtd(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd)
{
    sqtd->nextqtd = sc->sc_freeqtds;
    sc->sc_freeqtds = sqtd;
}

static void ehci_free_all_dma(ehci_softc_t *sc)
{
    ehci_dma_allocs_t *p;

    for (p = LIST_FIRST(&(sc->sc_dma_allocs)); p;
        p =  LIST_FIRST(&(sc->sc_dma_allocs)))
    {
        LIST_REMOVE(p, list);
        jdma_free(p->dma);
        jfree(p);
    }
}

usbd_status ehci_alloc_sqtd_chain(struct ehci_pipe *epipe, ehci_softc_t *sc,
    jint_t alen, jint_t rd, usbd_xfer_handle xfer, ehci_soft_qtd_t **sp, 
    ehci_soft_qtd_t **ep)
{
    ehci_soft_qtd_t *next, *cur;
    ehci_physaddr_t dataphys = 0, dataphyspage = 0;
    ehci_physaddr_t dataphyslastpage = 0, nextphys = 0;
    juint32_t qtdstatus;
    juint32_t len, curlen, mps;
    juint32_t i, tog;
    jdma_handle *dma = &xfer->dmabuf;
    juint16_t flags = xfer->flags;

    DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: start len=%d\n", alen));

    len = alen;
    if (len)
    {
        dataphys = DMAADDR(dma, 0);
        dataphyslastpage = EHCI_PAGE(dataphys + len - 1);
    }
    qtdstatus = EHCI_QTD_ACTIVE |
        EHCI_QTD_SET_PID(rd ? EHCI_QTD_PID_IN : EHCI_QTD_PID_OUT) |
        EHCI_QTD_SET_CERR(3)
        /* IOC set below */
        /* BYTES set below */
        ;
    mps = UGETW(epipe->pipe.endpoint->edesc->wMaxPacketSize);
    tog = epipe->nexttoggle;
    qtdstatus |= EHCI_QTD_SET_TOGGLE(tog);

    cur = ehci_alloc_sqtd(sc);
    *sp = cur;
    if (cur == NULL)
        goto nomem;
    for (;;) 
    {
        dataphyspage = EHCI_PAGE(dataphys);
        /* The EHCI hardware can handle at most 5 pages. */
        if (dataphyspage == dataphyslastpage) 
            curlen = len;
        else 
            curlen = EHCI_PAGE_SIZE - EHCI_PAGE_MASK(dataphys);

        cur->qtd.qtd_buffer[0] = htoc32(dataphys);
        cur->qtd.qtd_buffer_hi[0] = 0;

        for (i = 1; (curlen < len) && (i < EHCI_QTD_NBUFFERS); i++) 
        {
            ehci_physaddr_t a = dataphys + i * EHCI_PAGE_SIZE;

            curlen += EHCI_PAGE_SIZE;
            a = EHCI_PAGE(a);
            cur->qtd.qtd_buffer[i] = htoc32(a);
            cur->qtd.qtd_buffer_hi[i] = 0;

            if( curlen > len)
                curlen = len;               
        }

        /* In case we need to add multiply QTD 
         * the length must be a multiple of the max size */
        if (curlen != len)
        {
            curlen -= curlen % mps;
            DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: multiple QTDs, "
                "curlen=%ld\n", curlen));
        }
#ifdef DIAGNOSTIC
        if (curlen == 0)
            j_panic("ehci_alloc_std: curlen == 0");
#endif
        DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain : dataphys=0x%08x "
            "dataphyslastpage=0x%08lx len=%ld curlen=%ld\n",
            dataphys, dataphyslastpage,
            len, curlen));
        len -= curlen;

        /*
         * Allocate another transfer if there's more data left, 
         * or if force last short transfer flag is set and we're 
         * allocating a multiple of the max packet size.
         */
        if (len != 0 ||
            ((curlen % mps) == 0 && !rd && curlen != 0 &&
            (flags & USBD_FORCE_SHORT_XFER))) 
        {
            DBG_X(DHCD_TRANSFER, ("ehci alloc in if: len=%d, curlen=%d"
                "mps=%d, rd=%d, flags=%d",len, curlen, mps, 
                rd, flags));
            next = ehci_alloc_sqtd(sc);
            if (next == NULL)
                goto nomem;
            nextphys = htoc32(next->physaddr);
        } 
        else 
        {
            next = NULL;
            nextphys = EHCI_NULL;
        }

        cur->nextqtd = next;
        cur->qtd.qtd_next = cur->qtd.qtd_altnext = nextphys;
        cur->qtd.qtd_status =
            htoc32(qtdstatus | EHCI_QTD_SET_BYTES(curlen));
        cur->xfer = xfer;
        cur->len = (juint16_t)curlen;
        DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: cbp=0x%08x end=0x%08x\n",
            dataphys, dataphys + curlen));
        /* adjust the toggle based on the number of packets in this
           qtd */
        if (((curlen + mps - 1) / mps) & 1) 
        {
            tog ^= 1;
            qtdstatus ^= EHCI_QTD_TOGGLE_MASK;
        }
        if (next == NULL)
            break;
        DBG_X(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: extend chain\n"));
        dataphys += curlen;
        cur = next;
    }
    cur->qtd.qtd_status |= htoc32(EHCI_QTD_IOC);
    *ep = cur;
    epipe->nexttoggle = tog;

    DBG_V(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: return sqtd=%p sqtdend=%p\n",
        *sp, *ep));

    return (USBD_NORMAL_COMPLETION);

nomem:

    DBG_E(DHCD_TRANSFER, ("ehci_alloc_sqtd_chain: no memory\n"));
    return (USBD_NOMEM);
}

static void ehci_free_sqtd_chain(ehci_softc_t *sc, ehci_soft_qtd_t *sqtd,
    ehci_soft_qtd_t *sqtdend)
{
    ehci_soft_qtd_t *p;
    jint_t i;

    DBG_X(DHCD_TRANSFER, ("ehci_free_sqtd_chain: sqtd=%p sqtdend=%p\n",
        sqtd, sqtdend));

    for (i = 0; sqtd != sqtdend; sqtd = p, i++) 
    {
        p = sqtd->nextqtd;
        ehci_free_sqtd(sc, sqtd);
    }
}

/****************/

/*
 * Close a reqular pipe.
 * Assumes that there are no pending transactions.
 */
void ehci_close_pipe(usbd_pipe_handle pipe, ehci_soft_qh_t *head)
{
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;
    ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;
    ehci_soft_qh_t *sqh = epipe->sqh;

    ehci_rem_qh(sc, sqh, head);
    ehci_free_sqh(sc, epipe->sqh);

    /* Free the memory allocated for control pipes during open */ 
    if ((pipe->endpoint->edesc->bmAttributes & UE_XFERTYPE) == UE_CONTROL) 
        jdma_free(epipe->u.ctl.reqdma); 
}

/*
 * Abort a device request.
 * It's impossible to guarantee that the requested transfer will not
 * have happened since the hardware runs concurrently.
 * If the transaction has already happened we rely on the ordinary
 * interrupt processing to process it.
 */
void ehci_abort_xfer(usbd_xfer_handle xfer, usbd_status status)
{
#define exfer EXFER(xfer)
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    ehci_softc_t *sc = (ehci_softc_t *)epipe->pipe.device->bus;
    ehci_soft_qh_t *sqh = epipe->sqh;
    ehci_soft_qtd_t *sqtd;
    ehci_physaddr_t cur;
    juint32_t qhstatus;
    jint_t hit;
    jint_t wake;

    DBG_V(DHCD_ABORT, ("ehci_abort_xfer: xfer=%p pipe=%p\n", xfer, epipe));

    if (sc->sc_dying
#ifdef CONFIG_JOTG
        || !sc->otg_host_enabled
#endif
        ) 
    {
        /* If we're dying, just do the software part. */
        xfer->status = status;  /* make software ignore it */
        ehci_xfer_complete(xfer);
        return;
    }

    if (xfer->device->bus->intr_context)
        j_panic("ehci_abort_xfer: not in process context");

    /*
     * If an abort is already in progress then just wait for it to
     * complete and return.
     */
    if (xfer->hcflags & UXFER_ABORTING) 
    {
        DBG_I(DHCD_ABORT, ("ehci_abort_xfer: already aborting\n"));
#ifdef DIAGNOSTIC
        if (status == USBD_TIMEOUT)
            jprintf("ehci_abort_xfer: TIMEOUT while aborting\n");
#endif
        /* Override the status which might be USBD_TIMEOUT. */
        xfer->status = status;
        DBG_I(DHCD_ABORT,
            ("ehci_abort_xfer: waiting for abort to finish\n"));
        xfer->hcflags |= UXFER_ABORTWAIT;
        while (xfer->hcflags & UXFER_ABORTING)
            jmsleep(xfer->abort_sleep_h, 0);
        return;
    }
    xfer->hcflags |= UXFER_ABORTING;

    /*
     * Step 1: Make interrupt routine and hardware ignore xfer.
     */
    xfer->status = status;      /* make software ignore it */
    qhstatus = sqh->qh.qh_qtd.qtd_status;
    sqh->qh.qh_qtd.qtd_status = qhstatus | htoc32(EHCI_QTD_HALTED);
    for (sqtd = exfer->sqtdstart; ; sqtd = sqtd->nextqtd) 
    {
        sqtd->qtd.qtd_status |= htoc32(EHCI_QTD_HALTED);
        if (sqtd == exfer->sqtdend)
            break;
    }

    /*
     * Step 2: Wait until we know hardware has finished any possible
     * use of the xfer.  Also make sure the soft interrupt routine
     * has run.
     */
    ehci_sync_hc(sc);
    sc->sc_flags |= EHCI_TRANSFER;
    ehci_softintr(sc);

    /*
     * Step 3: Remove any vestiges of the xfer from the hardware.
     * The complication here is that the hardware may have executed
     * beyond the xfer we're trying to abort.  So as we're scanning
     * the TDs of this xfer we check if the hardware points to
     * any of them.
     */
    cur = EHCI_LINK_ADDR(ctoh32(sqh->qh.qh_curqtd));
    hit = 0;
    for (sqtd = exfer->sqtdstart; ; sqtd = sqtd->nextqtd) 
    {
        hit |= cur == sqtd->physaddr;
        if (sqtd == exfer->sqtdend)
            break;
    }
    sqtd = sqtd->nextqtd;
    /* Zap curqtd register if hardware pointed inside the xfer. */
    if (hit && sqtd != NULL) 
    {
        DBG_I(DHCD_ABORT,
            ("ehci_abort_xfer: cur=0x%08x\n", sqtd->physaddr));
        sqh->qh.qh_curqtd = htoc32(sqtd->physaddr); /* unlink qTDs */
        sqh->qh.qh_qtd.qtd_status = qhstatus;
    }
    else
    {
        DBG_I(DHCD_ABORT,("ehci_abort_xfer: no hit\n"));
    }

    /*
     * Step 4: Execute callback.
     */
#ifdef DIAGNOSTIC
    exfer->isdone = 1;
#endif
    wake = xfer->hcflags & UXFER_ABORTWAIT;
    xfer->hcflags &= ~(UXFER_ABORTING | UXFER_ABORTWAIT);
    ehci_xfer_complete(xfer);
    if (wake)
        jwakeup(xfer->abort_sleep_h);
#undef exfer
}

void ehci_timeout_task(void *addr)
{
    usbd_xfer_handle xfer = addr;

    DBG_V(DHCD_TRANSFER, ("ehci_timeout_task: xfer=%p\n", xfer));

    ehci_abort_xfer(xfer, USBD_TIMEOUT);
}

static usbd_status ehci_device_ctrl_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /* Pipe isn't running, start first */
    return (ehci_device_ctrl_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

static usbd_status ehci_device_ctrl_start(usbd_xfer_handle xfer)
{
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
    usbd_status err;

    if (sc->sc_dying)
        return (USBD_IOERROR);

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
        return USBD_IOERROR;
#endif

#ifdef DIAGNOSTIC
    if (!(xfer->rqflags & URQ_REQUEST)) 
    {
        jprintf("ehci_device_ctrl_transfer: not a request\n");
        return (USBD_INVAL);
    }
#endif

#define exfer EXFER(xfer)
    exfer->started = 1;
    err = ehci_device_request(xfer);
    if (err)
        return (err);

    return (USBD_IN_PROGRESS);
}

void ehci_device_ctrl_done(usbd_xfer_handle xfer)
{
    struct ehci_xfer *ex = EXFER(xfer);
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;

    DBG_I(DHCD_TRANSFER, ("ehci_ctrl_done: xfer=%p\n", xfer));

#ifdef DIAGNOSTIC
    if (!(xfer->rqflags & URQ_REQUEST))
        j_panic("ehci_ctrl_done: not a request");
#endif

    if (xfer->status != USBD_NOMEM && ehci_active_intr_list(ex))
    {
        ehci_del_intr_list(ex); /* remove from active list */
        ehci_free_sqtd_chain(sc, ex->sqtdstart, NULL);
    }

    DBG_I(DHCD_TRANSFER, ("ehci_ctrl_done: length=%d\n", xfer->actlen));
}

/* Abort a device control request. */
static void ehci_device_ctrl_abort(usbd_xfer_handle xfer)
{
    DBG_I(DHCD_ABORT, ("ehci_device_ctrl_abort: xfer=%p\n", xfer));

#define exfer EXFER(xfer)
    if (!exfer->started)
    {
        xfer->status = USBD_CANCELLED;
        ehci_xfer_complete(xfer);
        return;
    }

    jtask_stop(xfer->timeout_handle);
    ehci_abort_xfer(xfer, USBD_CANCELLED);
}

/* Close a device control pipe. */
static void ehci_device_ctrl_close(usbd_pipe_handle pipe)
{
    ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;

    DBG_I(DHCD_INIT, ("ehci_device_ctrl_close: pipe=%p\n", pipe));
    ehci_close_pipe(pipe, sc->sc_async_head);
}

usbd_status ehci_device_request(usbd_xfer_handle xfer)
{
#define exfer EXFER(xfer)
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    usb_device_request_t *req = &xfer->request;
    usbd_device_handle dev = epipe->pipe.device;
    ehci_softc_t *sc = (ehci_softc_t *)dev->bus;
    juint8_t addr = dev->address;
    ehci_soft_qtd_t *setup, *stat, *next;
    ehci_soft_qh_t *sqh;
    jint_t isread;
    jint_t len;
    usbd_status err;

    isread = req->bmRequestType & UT_READ;
    len = UGETW(req->wLength);

    DBG_I(DHCD_TRANSFER, ("ehci_device_control type=0x%02x, request=0x%02x, "
        "wValue=0x%04x, wIndex=0x%04x len=%d, addr=%d, endpt=%d\n",
        req->bmRequestType, req->bRequest, UGETW(req->wValue),
        UGETW(req->wIndex), len, addr,
        epipe->pipe.endpoint->edesc->bEndpointAddress));

    setup = ehci_alloc_sqtd(sc);
    if (setup == NULL) 
    {
        err = USBD_NOMEM;
        goto bad1;
    }
    stat = ehci_alloc_sqtd(sc);
    if (stat == NULL) 
    {
        err = USBD_NOMEM;
        goto bad2;
    }

    sqh = epipe->sqh;
    epipe->u.ctl.length = len;

    /* Update device address and length since they may have changed
       during the setup of the control pipe in usbd_new_device(). */
    sqh->qh.qh_endp =
        (sqh->qh.qh_endp & htoc32(~(EHCI_QH_ADDRMASK | EHCI_QH_MPLMASK))) |
        htoc32(
            EHCI_QH_SET_ADDR(addr) |
            EHCI_QH_SET_MPL(UGETW(epipe->pipe.endpoint->edesc->wMaxPacketSize))
            );

    /* Set up data transaction */
    if (len != 0) 
    {
        ehci_soft_qtd_t *end;

        /* Start toggle at 1. */
        epipe->nexttoggle = 1;
        err = ehci_alloc_sqtd_chain(epipe, sc, len, isread, xfer,
            &next, &end);
        if (err)
            goto bad3;
        end->qtd.qtd_status &= htoc32(~EHCI_QTD_IOC);
        end->nextqtd = stat;
        end->qtd.qtd_next =
            end->qtd.qtd_altnext = htoc32(stat->physaddr);
    } 
    else 
        next = stat;

    j_memcpy(KERNADDR(&epipe->u.ctl.reqdma, 0), (void *)req, 
        sizeof(usb_device_request_t));

    /* Clear toggle */
    setup->qtd.qtd_status = htoc32(
        EHCI_QTD_ACTIVE |
        EHCI_QTD_SET_PID(EHCI_QTD_PID_SETUP) |
        EHCI_QTD_SET_CERR(3) |
        EHCI_QTD_SET_TOGGLE(0) |
        EHCI_QTD_SET_BYTES(sizeof(usb_device_request_t))
        );
    setup->qtd.qtd_buffer[0] = htoc32(DMAADDR(&epipe->u.ctl.reqdma, 0));
    setup->qtd.qtd_buffer_hi[0] = 0;
    setup->nextqtd = next;
    setup->qtd.qtd_next = setup->qtd.qtd_altnext = htoc32(next->physaddr);
    setup->xfer = xfer;
    setup->len = sizeof(usb_device_request_t);

    stat->qtd.qtd_status = htoc32(
        EHCI_QTD_ACTIVE |
        EHCI_QTD_SET_PID(isread ? EHCI_QTD_PID_OUT : EHCI_QTD_PID_IN) |
        EHCI_QTD_SET_CERR(3) |
        EHCI_QTD_SET_TOGGLE(1) |
        EHCI_QTD_IOC
        );
    stat->qtd.qtd_buffer[0] = 0; 
    stat->qtd.qtd_buffer_hi[0] = 0; 
    stat->nextqtd = NULL;
    stat->qtd.qtd_next = stat->qtd.qtd_altnext = EHCI_NULL;
    stat->xfer = xfer;
    stat->len = 0;

    DBG_X(DHCD_TRANSFER, ("ehci_device_request:\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(setup));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(stat));

    exfer->sqtdstart = setup;
    exfer->sqtdend = stat;
#ifdef DIAGNOSTIC
    if (!exfer->isdone) 
        jprintf("ehci_device_request: not done, exfer=%p\n", exfer);
    exfer->isdone = 0;
#endif
    ehci_add_intr_list(sc, exfer);
    xfer->status = USBD_IN_PROGRESS;

    DBG_X(DHCD_TRANSFER, ("ehci_device_request %p:\n", setup));

    if (xfer->timeout)
    {
        DBG_V(DHCD_TRANSFER, ("ehci_device_request xfer_timeout 0x%lx\n", 
            xfer->timeout));
        jtask_schedule(xfer->timeout_handle, xfer->timeout,
            ehci_timeout_task, xfer);
    }

    /* Insert qTD in QH list. */
    ehci_set_qh_qtd(sqh, setup);

    DBG_X(DHCD_TRANSFER, ("ehci_device_request: status=%x\n",
        EOREAD4(sc, EHCI_USBSTS)));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, jdelay_us(10000));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_regs(sc));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sc->sc_async_head));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(setup));

    return (USBD_NORMAL_COMPLETION);

bad3:
    ehci_free_sqtd(sc, stat);
bad2:
    ehci_free_sqtd(sc, setup);
bad1:
    DBG_E(DHCD_TRANSFER, ("ehci_device_request: no memory\n"));
    xfer->status = err;
    ehci_xfer_complete(xfer);
    return (err);
#undef exfer
}

/************************/

static usbd_status ehci_device_bulk_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /* Pipe isn't running, start first */
    return (ehci_device_bulk_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

usbd_status ehci_device_bulk_start(usbd_xfer_handle xfer)
{
#define exfer EXFER(xfer)
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    usbd_device_handle dev = epipe->pipe.device;
    ehci_softc_t *sc = (ehci_softc_t *)dev->bus;
    ehci_soft_qtd_t *data, *dataend;
    ehci_soft_qh_t *sqh;
    usbd_status err;
    jint_t len, isread;
    juint8_t endpt;

    DBG_I(DHCD_TRANSFER, ("ehci_device_bulk_transfer: xfer=%p len=%d "
        "flags=%d\n", xfer, xfer->length, xfer->flags));

    if (sc->sc_dying)
        return (USBD_IOERROR);

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
        return USBD_IOERROR;
#endif

#ifdef DIAGNOSTIC
    if (xfer->rqflags & URQ_REQUEST)
        j_panic("ehci_device_bulk_transfer: a request");
#endif

    exfer->started = 1;
    len = xfer->length;
    endpt = epipe->pipe.endpoint->edesc->bEndpointAddress;
    isread = UE_GET_DIR(endpt) == UE_DIR_IN;
    sqh = epipe->sqh;

    epipe->u.bulk.length = len;

    err = ehci_alloc_sqtd_chain(epipe, sc, len, isread, xfer, &data,
        &dataend);
    if (err) 
    {
        DBG_E(DHCD_TRANSFER, ("ehci_device_bulk_transfer: no memory\n"));
        xfer->status = err;
        ehci_xfer_complete(xfer);
        return (err);
    }

    DBG_X(DHCD_TRANSFER, ("ehci_device_bulk_transfer: data(1)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(data));

    /* Set up interrupt info. */
    exfer->sqtdstart = data;
    exfer->sqtdend = dataend;
#ifdef DIAGNOSTIC
    if (!exfer->isdone) 
        jprintf("ehci_device_bulk_transfer: not done, ex=%p\n", exfer);
    exfer->isdone = 0;
#endif

    ehci_add_intr_list(sc, exfer);
    xfer->status = USBD_IN_PROGRESS;

    ehci_set_qh_qtd(sqh, data);
    if (xfer->timeout)
    {
        jtask_schedule(xfer->timeout_handle, xfer->timeout,
            ehci_timeout_task, xfer);
    }

    DBG_X(DHCD_TRANSFER, ("ehci_device_bulk_transfer: data(2)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, jdelay_us(10000));
    DBG_X(DHCD_TRANSFER, ("ehci_device_bulk_transfer: data(3)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_regs(sc));
    DBG_X(DHCD_TRANSFER, ("sqh:\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(data));

    return (USBD_IN_PROGRESS);
#undef exfer
}

static void ehci_device_bulk_abort(usbd_xfer_handle xfer)
{
    DBG_I(DHCD_ABORT, ("ehci_device_bulk_abort: xfer=%p\n", xfer));

    if (!EXFER(xfer)->started)
    {
        xfer->status = USBD_CANCELLED;
        ehci_xfer_complete(xfer);
        return;
    }

    jtask_stop(xfer->timeout_handle);
    ehci_abort_xfer(xfer, USBD_CANCELLED);
}

/*
 * Close a device bulk pipe.
 */
static void ehci_device_bulk_close(usbd_pipe_handle pipe)
{
    ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;

    DBG_I(DHCD_INIT, ("ehci_device_bulk_close: pipe=%p\n", pipe));
    ehci_close_pipe(pipe, sc->sc_async_head);
}

void ehci_device_bulk_done(usbd_xfer_handle xfer)
{
    struct ehci_xfer *ex = EXFER(xfer);
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
    /*struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;*/

    DBG_I(DHCD_TRANSFER, ("ehci_bulk_done: xfer=%p, actlen=%d\n",
        xfer, xfer->actlen));

    if (xfer->status != USBD_NOMEM && ehci_active_intr_list(ex)) 
    {
        ehci_del_intr_list(ex); /* remove from active list */
        ehci_free_sqtd_chain(sc, ex->sqtdstart, NULL);
    }

    DBG_I(DHCD_TRANSFER, ("ehci_bulk_done: length=%d\n", xfer->actlen));
}

/************************/

static usbd_status ehci_device_setintr(ehci_softc_t *sc, ehci_soft_qh_t *sqh,
    jint_t ival)
{
    struct ehci_soft_islot *isp;
    jint_t islot, lev;

    /* Find a poll rate that is large enough. */
    for (lev = EHCI_IPOLLRATES - 1; lev > 0; lev--)
    {
        if (EHCI_ILEV_IVAL(lev) <= ival)
            break;
    }

    /* Pick an interrupt slot at the right level. */
    sc->sc_rand = (sc->sc_rand + 191) % sc->sc_flsize;
    islot = EHCI_IQHIDX(lev, sc->sc_rand);

    sqh->islot = islot;
    isp = &sc->sc_islots[islot];
    ehci_add_qh(sqh, isp->sqh);

    return (USBD_NORMAL_COMPLETION);
}


static usbd_status ehci_device_intr_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /*
     * Pipe isn't running (otherwise err would be USBD_INPROG),
     * so start it first.
     */
    return (ehci_device_intr_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

static usbd_status ehci_device_intr_start(usbd_xfer_handle xfer)
{
#define exfer EXFER(xfer)
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    usbd_device_handle dev = xfer->pipe->device;
    ehci_softc_t *sc = (ehci_softc_t *)dev->bus;
    ehci_soft_qtd_t *data, *dataend;
    ehci_soft_qh_t *sqh;
    usbd_status err;
    jint_t len, isread;
    juint8_t endpt;

    DBG_I(DHCD_TRANSFER, ("ehci_device_intr_start: xfer=%p len=%d flags=%d\n",
        xfer, xfer->length, xfer->flags));

    if (sc->sc_dying)
        return (USBD_IOERROR);

#ifdef CONFIG_JOTG
    if (!sc->otg_host_enabled)
        return USBD_IOERROR;
#endif

#ifdef DIAGNOSTIC
    if (xfer->rqflags & URQ_REQUEST)
        j_panic("ehci_device_intr_start: a request");
#endif

    exfer->started = 1;
    len = xfer->length;
    endpt = epipe->pipe.endpoint->edesc->bEndpointAddress;
    isread = UE_GET_DIR(endpt) == UE_DIR_IN;
    sqh = epipe->sqh;

    epipe->u.intr.length = len;

    err = ehci_alloc_sqtd_chain(epipe, sc, len, isread, xfer, &data,
        &dataend);
    if (err) 
    {
        DBG_E(DHCD_TRANSFER, ("ehci_device_intr_start: no memory\n"));
        xfer->status = err;
        ehci_xfer_complete(xfer);
        return (err);
    }

    DBG_X(DHCD_TRANSFER, ("ehci_device_intr_start: data(1)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(data));

    /* Set up interrupt info. */
    exfer->sqtdstart = data;
    exfer->sqtdend = dataend;
#ifdef DIAGNOSTIC
    if (!exfer->isdone)
        jprintf("ehci_device_intr_start: not done, ex=%p\n", exfer);
    exfer->isdone = 0;
#endif

    ehci_add_intr_list(sc, exfer);
    xfer->status = USBD_IN_PROGRESS;

    ehci_set_qh_qtd(sqh, data);
    if (xfer->timeout)
    {
        jtask_schedule(xfer->timeout_handle, xfer->timeout,
            ehci_timeout_task, xfer);
    }

    DBG_X(DHCD_TRANSFER, ("ehci_device_intr_start: data(2)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, jdelay_us(10000));
    DBG_X(DHCD_TRANSFER, ("ehci_device_intr_start: data(3)\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_regs(sc));
    DBG_X(DHCD_TRANSFER, ("sqh:\n"));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqh(sqh));
    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, ehci_dump_sqtds(data));

    return (USBD_IN_PROGRESS);
#undef exfer
}

static void ehci_device_intr_abort(usbd_xfer_handle xfer)
{
    DBG_I(DHCD_ABORT, ("ehci_device_intr_abort: xfer=%p\n", xfer));
    
    if (xfer->pipe->intrxfer == xfer)
        DBG_I(DHCD_ABORT, ("ehci_device_intr_abort: remove\n"));

    if (!EXFER(xfer)->started)
    {
        xfer->status = USBD_CANCELLED;
        ehci_xfer_complete(xfer);
        return;
    }

    jtask_stop(xfer->timeout_handle);
    ehci_abort_xfer(xfer, USBD_CANCELLED);
}

static void ehci_device_intr_close(usbd_pipe_handle pipe)
{
    ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;
    struct ehci_soft_islot *isp;

    isp = &sc->sc_islots[epipe->sqh->islot];
    ehci_close_pipe(pipe, isp->sqh);
}

static void ehci_device_intr_done(usbd_xfer_handle xfer)
{
#define exfer EXFER(xfer)
    struct ehci_xfer *ex = EXFER(xfer);
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;
    ehci_soft_qtd_t *data, *dataend;
    ehci_soft_qh_t *sqh;
    usbd_status err;
    jint_t len, isread;
    juint8_t endpt;

    DBG_I(DHCD_TRANSFER, ("ehci_device_intr_done: xfer=%p, actlen=%d\n",
        xfer, xfer->actlen));

    if (xfer->pipe->repeat) 
    {
        ehci_free_sqtd_chain(sc, ex->sqtdstart, NULL);

        len = epipe->u.intr.length;
        xfer->length = len;
        endpt = epipe->pipe.endpoint->edesc->bEndpointAddress;
        isread = UE_GET_DIR(endpt) == UE_DIR_IN;
        sqh = epipe->sqh;

        err = ehci_alloc_sqtd_chain(epipe, sc, len, isread, xfer,
            &data, &dataend);
        if (err) 
        {
            DBG_E(DHCD_TRANSFER, ("ehci_device_intr_done: no memory\n"));
            xfer->status = err;
            return;
        }

        /* Set up interrupt info. */
        exfer->sqtdstart = data;
        exfer->sqtdend = dataend;
#ifdef DIAGNOSTIC
        if (!exfer->isdone) 
        {
            jprintf("ehci_device_intr_done: not done, ex=%p\n",
                exfer);
        }
        exfer->isdone = 0;
#endif

        ehci_set_qh_qtd(sqh, data);
        if (xfer->timeout)
        {
            jtask_schedule(xfer->timeout_handle,
                xfer->timeout, ehci_timeout_task, xfer);
        }

        xfer->status = USBD_IN_PROGRESS;
    } 
    else if (xfer->status != USBD_NOMEM && ehci_active_intr_list(ex)) 
    {
        ehci_del_intr_list(ex); /* remove from active list */
        ehci_free_sqtd_chain(sc, ex->sqtdstart, NULL);
    }
#undef exfer
}

#define BitTime(bytecount)  (7 * 8 * bytecount / 6)
#define BW_HOST_DELAY   1000L           /* nanoseconds */
static const juint8_t smask_out [] = { 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f };

static jint32_t usb_calc_bus_time (jint_t is_input, jint_t bytecount)
{
    juint32_t tmp;

    tmp = (8354L * (31L + 10L * BitTime (bytecount))) / 1000L;
    return (((is_input) ? 7268L : 6265L) + BW_HOST_DELAY + tmp);
}

static juint32_t ehci_sitd_mask(usbd_pipe_handle pipe)
{
    juint32_t tmp, mask;
    jint_t maxp;
    usb_endpoint_descriptor_t *ep = pipe->endpoint->edesc;

    maxp = UGETW(ep->wMaxPacketSize) & UE_SIZE_VALID;

    if (UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN)
    {
        mask = 1;

        /* pessimistic c-mask */
        tmp = usb_calc_bus_time (1, maxp) / (125L * 1000);
        mask |= 3 << (tmp + 9);
        mask = 0xfe01; 
    } 
    else
        mask = smask_out [maxp / 188];
   
    return mask;
}

static void ehci_setup_isoc_sitd(ehci_pipe_t *epipe, ehci_itd_soft_t *itd_soft,
    phys_addr buf, juint16_t len, jbool_t need_intr)
{
    usbd_pipe_handle pipe = (usbd_pipe_handle)epipe;
    usb_endpoint_descriptor_t *ep = pipe->endpoint->edesc;
    ehci_sitd_t *sitd = itd_soft->u.sitd;
    jbool_t in = (UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN);

    sitd->sitd_addr = htoc32(
        EHCI_SITD_DIR(BSET, in ? EHCI_SITD_IN : EHCI_SITD_OUT) |
        EHCI_SITD_PORTNUM(BSET, epipe->u.isoc.hubport) |
        EHCI_SITD_HUBADDR(BSET, epipe->u.isoc.hubaddr) |
        EHCI_SITD_EPT(BSET, UE_GET_ADDR(ep->bEndpointAddress)) |
        EHCI_SITD_DEVADDR(BSET, pipe->device->address));

    sitd->sitd_mask = htoc32(ehci_sitd_mask(pipe));

    sitd->sitd_status = htoc32(
        EHCI_SITD_IOC(BSET, need_intr) |
        EHCI_SITD_PG(BSET, 0) |
        EHCI_SITD_LEN(BSET, len) |
        EHCI_SITD_CPROGMASK(BSET, 0) |
        EHCI_SITD_STATUS(BSET, EHCI_SITD_ACTIVE));

    sitd->sitd_buffer[0] = htoc32(buf);
    
    len = (len + 187) / 188;
    if (len > 1) /* BEGIN vs ALL */
        len |= 1 << 3;
    
    if (in)
        len &= ~0x7;
        
    sitd->sitd_buffer[1] = htoc32(
        EHCI_SITD_BUFP(BSET, EHCI_SITD_BUFP(BGET, buf)+1) |
        len);
    
    sitd->sitd_back = htoc32(EHCI_LINK_TERMINATE);
    sitd->sitd_buffer_hi[0] = 0;
    sitd->sitd_buffer_hi[1] = 0;
}

static void ehci_itd_init(usbd_pipe_handle pipe, ehci_itd_t *itd)
{
    usb_endpoint_descriptor_t *ep = pipe->endpoint->edesc;
    jint_t i;

    for (i = 0; i < EHCI_ITD_NTRANS; i++)
        itd->itd_trans[i] = 0;

    itd->itd_buffer[0] = htoc32(
        EHCI_ITD_EPT(BSET, UE_GET_ADDR(ep->bEndpointAddress)) |
        EHCI_ITD_DEVADDR(BSET, pipe->device->address));

    itd->itd_buffer[1] = htoc32(
        EHCI_ITD_DIR(BSET, UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN ? 
        EHCI_ITD_IN : EHCI_ITD_OUT) |
        EHCI_ITD_MAXPKT(BSET, UGETW(ep->wMaxPacketSize) & UE_SIZE_VALID));

    itd->itd_buffer[2] = htoc32(
        EHCI_ITD_MULT(BSET, UE_GET_MULT(UGETW(ep->wMaxPacketSize))+1));

    for (i = 3; i < EHCI_ITD_NBUFFERS; i++)
        itd->itd_buffer[i] = 0;
    
    for (i = 0; i < EHCI_ITD_NBUFFERS; i++)
        itd->itd_buffer_hi[i] = 0;
}

static void ehci_itd_alloc_page(ehci_itd_soft_t *itd_soft, phys_addr buf)
{
    KASSERT(itd_soft->next_page < EHCI_ITD_NBUFFERS, ("No more pages in itd."));

    itd_soft->u.itd->itd_buffer[itd_soft->next_page]  |= htoc32(
        EHCI_ITD_BUFP(BSET, EHCI_ITD_BUFP(BGET, buf)));
    itd_soft->next_page++;
}

static void ehci_setup_isoc_itd_uf(ehci_itd_soft_t *itd_soft,
    juint8_t uf_num, phys_addr buf, juint16_t len, jbool_t need_intr)
{
    ehci_itd_t *itd = itd_soft->u.itd;
    juint8_t page;
    jbool_t page_ready = 0;

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_setup_isoc_itd_uf pipe itd(%p,%u)"
        " buf=%.8lx len=%u intr=%d\n", itd, uf_num, buf, len, need_intr));

    if (itd_soft->next_page)
    {
        /* Previous and new transfer have same page */
        if (EHCI_ITD_BUFP(BGET, 
            ctoh32(itd->itd_buffer[itd_soft->next_page-1])) ==
            EHCI_ITD_BUFP(BGET, buf))
        {
            page_ready = 1;
        }
    }

    if (!page_ready)
        ehci_itd_alloc_page(itd_soft, buf);

    /* Page overflow */
    page = itd_soft->next_page - 1;
    if (EHCI_ITD_BUFP(BGET, buf) != EHCI_ITD_BUFP(BGET, buf + len))
        ehci_itd_alloc_page(itd_soft, buf + len);

    itd->itd_trans[uf_num] = htoc32(
        EHCI_ITD_STATUS(BSET, EHCI_ITD_ACTIVE) |
        EHCI_ITD_LEN   (BSET, len) | 
        EHCI_ITD_IOC   (BSET, need_intr) |
        EHCI_ITD_PG    (BSET, page) |
        EHCI_ITD_OFS   (BSET, EHCI_ITD_OFS(BGET, buf)));

    itd_soft->uf_active |= 1 << uf_num;
}

static void ehci_itd_soft_init(ehci_pipe_t *epipe, ehci_itd_soft_t *itd_soft)
{
    if (EHCI_HIGH_SPEED(epipe))
    {
        /* one itd for 8 uframe */
        ehci_itd_init((usbd_pipe_handle)epipe, itd_soft->u.itd);
        itd_soft->uf_active = 0;
        itd_soft->next_page = 0;
    }
}

static void ehci_setup_isoc_td(ehci_pipe_t *epipe, ehci_itd_soft_t *itd_soft,
    juint8_t uf_num, phys_addr buf, juint16_t len, jbool_t need_intr)
{
    if (EHCI_HIGH_SPEED(epipe))
        ehci_setup_isoc_itd_uf(itd_soft, uf_num, buf, len, need_intr);
    else
        ehci_setup_isoc_sitd(epipe, itd_soft, buf, len, need_intr);
}

/* Checks 80% bandwidth limit for new pipe 
 * Updates controller bw info 
 * Calculates shift for microframes 
 * if pipe does not support time shift - uf_shift should be NULL */
static usbd_status ehci_bw_alloc(usbd_pipe_handle pipe, juint16_t treshold,
    juint16_t *uf_shift)
{
    if (uf_shift)
        *uf_shift = 0;

    DBG_I(DHCD_TRANSFER, ("EHCI: ehci_bw_alloc pipe=%p treshold=%u shift=%u\n",
        pipe, treshold, *uf_shift));
    return USBD_NORMAL_COMPLETION;
}

static void ehci_bw_free(usbd_pipe_handle pipe)
{
    DBG_I(DHCD_TRANSFER, ("EHCI: ehci_bw_free pipe=%p\n", pipe));
}

#define EHCI_GET_FRINDEX(sc) \
    ((juint16_t)((EOREAD4(sc, EHCI_FRINDEX)>>3) % sc->sc_flsize))

static juint16_t ehci_get_fl_uf_num(ehci_softc_t *sc)
{
    return (juint16_t) (sc->fl_uf_num + 
        (EHCI_GET_FRINDEX(sc) + EHCI_SAFE_TRESHOLD)* EHCI_UFRAME_IN_ITD);
}

static void ehci_isoc_pipe_update_sync(ehci_softc_t *sc, 
    ehci_pipe_t *epipe)
{
    if (epipe->u.isoc.last_sync)
    {
        /* Check transactions sync
         * cur_uf - last_uf < interval */
        jint16_t  safe_ival = (jint16_t)(epipe->u.isoc.last_uf + 
            epipe->u.isoc.interval_uf - ehci_get_fl_uf_num(sc));
        if (safe_ival < 0)
        {
            DBG_W(DHCD_TRANSFER, ("EHCI: lost sync on isoc pipe lag %d\n",
                safe_ival));
            epipe->u.isoc.last_sync = 0;
        }
    }
}

static jbool_t ehci_flist_find_uf(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint16_t *next_uf)
{
    juint16_t cur_uf   = ehci_get_fl_uf_num(sc);
    juint16_t last_uf  = epipe->u.isoc.last_uf;
    juint16_t ival     = epipe->u.isoc.interval_uf;
    juint16_t shift_uf = epipe->u.isoc.shift_uf;
    jint16_t  safe_ival;
    juint16_t uf_in_flist = (juint16_t)(sc->sc_flsize * EHCI_UFRAME_IN_ITD - 
        EHCI_SAFE_TRESHOLD_UF);
    
    if (epipe->u.isoc.last_sync)
    {
        *next_uf = last_uf + ival;
    }
    else
    {
        *next_uf = cur_uf / ival * ival + shift_uf;
        safe_ival = (jint16_t)(*next_uf - cur_uf); 

        if (safe_ival < 0)
            *next_uf += ival;
    }

    return *next_uf - cur_uf < uf_in_flist;
}

static ehci_itd_soft_t * pipe_alloc_itd_slot(ehci_pipe_t *epipe)
{
    ehci_itd_soft_t *itd_soft = TAILQ_FIRST(&epipe->u.isoc.itd_free_list);

    if (itd_soft)
    {
        TAILQ_REMOVE(&epipe->u.isoc.itd_free_list, itd_soft,next);

        ehci_itd_soft_init(epipe, itd_soft);

        TAILQ_INSERT_TAIL(&epipe->u.isoc.itd_run_list, itd_soft, next);
        DBG_X(DHCD_TRANSFER, ("EHCI: pipe_alloc_itd_slot itd=%p\n", itd_soft));
    }
    return itd_soft;
}

static void pipe_free_itd_slot(ehci_pipe_t *epipe, ehci_itd_soft_t *itd_soft)
{
    DBG_X(DHCD_TRANSFER, ("EHCI: pipe_free_itd_slot itd=%p\n", itd_soft));
    TAILQ_REMOVE(&epipe->u.isoc.itd_run_list, itd_soft, next);
    TAILQ_INSERT_TAIL(&epipe->u.isoc.itd_free_list, itd_soft, next);
}

#ifdef JDEBUG
static void dump_itd_soft(char *title, ehci_pipe_t *epipe, 
    ehci_itd_soft_t *itd_soft)
{
    jint_t i;
    jprintf("%s itd_soft dump %p\n", title, itd_soft);
    jprintf("  next %.8lx\n", *itd_soft->u.td_next);

    if (EHCI_HIGH_SPEED(epipe))
    {
        jprintf("trans= ");
    for (i = 0; i < EHCI_ITD_NTRANS; i++)
            jprintf("   %.8lx", itd_soft->u.itd->itd_trans[i]);
    jprintf("\nbuf=   ");
        for (i = 0; i < 3; i++) /*EHCI_ITD_NBUFFERS; i++)*/
            jprintf("   %.8lx", itd_soft->u.itd->itd_buffer[i]);
    }
    else
    {
        jprintf("addr = %.8lx    mask = %.8lx    stat = %.8lx\n", 
            itd_soft->u.sitd->sitd_addr, 
            itd_soft->u.sitd->sitd_mask,
            itd_soft->u.sitd->sitd_status);
        jprintf("bufp0= %.8lx    bufp1= %.8lx    back = %.8lx\n", 
            itd_soft->u.sitd->sitd_buffer[0], 
            itd_soft->u.sitd->sitd_buffer[1],
            itd_soft->u.sitd->sitd_back);
    }
    jprintf("\n");
}
#endif

static usbd_status ehci_init_itd_pool(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint_t td_size, juint16_t td_align, juint_t link_type)
{
    juint16_t i;
    jresult_t rc;
    usbd_status rt;
    ehci_itd_soft_t *itd_soft;
    juint16_t ival_f = epipe->u.isoc.interval_uf/EHCI_UFRAME_IN_FRAME;
    
    epipe->u.isoc.itd_pool_size = EHCI_SAFE_TRESHOLD*2 + EHCI_INTR_RATE;
    if (ival_f)
        epipe->u.isoc.itd_pool_size /= ival_f;

    DBG_I(DHCD_TRANSFER, ("EHCI: ehci_init_itd_pool epipe=%p pool size=%u\n", 
        epipe, epipe->u.isoc.itd_pool_size));

    TAILQ_INIT(&epipe->u.isoc.itd_run_list);
    TAILQ_INIT(&epipe->u.isoc.itd_free_list);
    
    epipe->u.isoc.itd_soft_pool = jmalloc(epipe->u.isoc.itd_pool_size * 
        sizeof(ehci_itd_soft_t), M_ZERO);
    if (!epipe->u.isoc.itd_soft_pool)
    {
        rt = USBD_NOMEM;
        goto Error;
    }

    rc = jdma_alloc(epipe->u.isoc.itd_pool_size * td_size, td_align,
        NULL, NULL, M_ZERO, &epipe->u.isoc.itd_pool);
    if (rc)
    {
        rt = USBD_NOMEM;
        goto Error;
    }

    for (i = 0, itd_soft = epipe->u.isoc.itd_soft_pool; 
        i < epipe->u.isoc.itd_pool_size; 
        i++, itd_soft++)
    {
        TAILQ_INSERT_TAIL(&epipe->u.isoc.itd_free_list, itd_soft, next);
        itd_soft->u.td = KERNADDR(&(epipe)->u.isoc.itd_pool, td_size * i);
        itd_soft->tdlink = htoc32(
            DMAADDR(&(epipe)->u.isoc.itd_pool, td_size * i) |
            EHCI_LINK_TYPE(link_type));
    }
    return USBD_NORMAL_COMPLETION;

Error:
    if (epipe->u.isoc.itd_soft_pool)
        jfree(epipe->u.isoc.itd_soft_pool);
    return rt;
}

static void ehci_uninit_itd_pool(ehci_softc_t *sc, ehci_pipe_t *epipe)
{
    if (epipe->u.isoc.itd_pool)
        jdma_free(epipe->u.isoc.itd_pool);

    if (epipe->u.isoc.itd_soft_pool)
        jfree(epipe->u.isoc.itd_soft_pool);
}

/* open isoc pipe */
static usbd_status ehci_device_set_isoc(ehci_softc_t *sc, 
    usbd_pipe_handle pipe, juint16_t ival, juint8_t hubport, juint8_t hubaddr)
{
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;
    usbd_status rc;

    DBG_I(DHCD_TRANSFER, ("EHCI: ehci_device_set_isoc pipe=%p ival=%u\n", 
        pipe, ival));
    
    epipe->u.isoc.hubport = hubport;
    epipe->u.isoc.hubaddr = hubaddr;
    epipe->u.isoc.last_sync = 0;
    rc = ehci_bw_alloc(pipe, EHCI_SAFE_TRESHOLD_UF, &epipe->u.isoc.shift_uf);
    if (rc)
        return rc;

    if (EHCI_HIGH_SPEED(pipe))
    {
        epipe->u.isoc.interval_uf = 1 << (ival - 1);
        rc = ehci_init_itd_pool(sc, epipe, EHCI_ITD_SIZE, EHCI_ITD_ALIGN, 
            EHCI_LINK_ITD);
    }
    else
    {
        epipe->u.isoc.interval_uf = (1 << (ival - 1)) * EHCI_UFRAME_IN_FRAME;
        rc = ehci_init_itd_pool(sc, epipe, EHCI_SITD_SIZE, EHCI_SITD_ALIGN, 
            EHCI_LINK_SITD);
    }

    if (rc)
        goto Error;

    TAILQ_INSERT_TAIL(&sc->isoc_pipe_list, epipe, next);

    return USBD_NORMAL_COMPLETION;
Error:
    ehci_bw_free(pipe);
    return rc;
}

static void ehci_device_isoc_close(usbd_pipe_handle pipe)
{
    ehci_softc_t *sc = (ehci_softc_t *)pipe->device->bus;
    struct ehci_pipe *epipe = (struct ehci_pipe *)pipe;

    DBG_I(DHCD_TRANSFER, ("EHCI: ehci_device_isoc_close\n"));
    
    TAILQ_REMOVE(&sc->isoc_pipe_list, epipe, next);

    ehci_uninit_itd_pool(sc, epipe);
    ehci_bw_free(pipe);
}

static usbd_status ehci_device_isoc_transfer(usbd_xfer_handle xfer) 
{
    usbd_status rc;

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_device_isoc_transfer xfer=%p\n", xfer));
    /* Insert last in queue. */
    xfer->status = USBD_IN_PROGRESS;
    EXFER(xfer)->buf_offset = 0;
    EXFER(xfer)->isoc_frame = 0;
    EXFER(xfer)->isoc_frame_done = 0;
    xfer->hcflags = 0;
    rc = usb_insert_transfer(xfer);
    if (rc && rc != USBD_IN_PROGRESS)
        return rc;

    return (ehci_device_isoc_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

static usbd_status ehci_device_isoc_start(usbd_xfer_handle xfer)
{ 
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_device_isoc_start xfer=%p\n", xfer));

    EXFER(xfer)->started = 1;

    ehci_isoc_sched(sc, epipe);
    return USBD_IN_PROGRESS;
}

static void ehci_isoc_sched(ehci_softc_t *sc, ehci_pipe_t *epipe)
{
    usbd_xfer_handle xfer;
    ehci_itd_soft_t *itd_soft = NULL;
    juint16_t ehci_uf, last_itd_uf = 0;
    juint32_t offset;
    juint16_t uf_len;
    jbool_t need_intr, itd_ready = 0;

    DBG_V(DHCD_TRANSFER, ("EHCI: ehci_isoc_sched pipe=%p\n", epipe));

    ehci_isoc_pipe_update_sync(sc, epipe);

    xfer = TAILQ_FIRST(&epipe->pipe.queue);
    if (!xfer)
        goto Exit;

    while (ehci_flist_find_uf(sc, epipe, &ehci_uf))
    {
        /* Try to find xfer with frame is not scheduled */
        /* or xfer cancelled ? */
        while (EXFER(xfer)->isoc_frame == xfer->nframes ||
            (xfer->hcflags & UXFER_ABORTING))
        {
            xfer = TAILQ_NEXT(xfer, next);
            if (!xfer)
                goto Exit;
        }

        /* if we have ready itd and next uframe must be in new itd
         *  -> insert ready itd to periodic frame list */
        if (itd_ready && GET_ITD(last_itd_uf) != GET_ITD(ehci_uf))
        {
            /*Put the transfer to the hw */
            ehci_insert_itd_to_flist(sc, epipe, 
                (juint16_t)GET_FLIST_ITD(sc, last_itd_uf), itd_soft);
            itd_ready = 0;
        }

        /* For first cycle and every time that itd inserted to frame list
         * we need allocate new itd */
        if (!itd_ready)
        {
            itd_soft = pipe_alloc_itd_slot(epipe); 
            if (!itd_soft)
            {
                DBG_X(DHCD_TRANSFER, ("EHCI: no more pipe itd slots\n"));
                goto Exit;
            }

            itd_ready = 1;
            last_itd_uf = ehci_uf;
        }

        /* Update xfer */
        /* it is first itd for xfer */
        if (EXFER(xfer)->isoc_frame == EXFER(xfer)->isoc_frame_done)
        {
            DBG_X(DHCD_TRANSFER, ("EHCI: first xfer frame\n"));
            EXFER(xfer)->itd_uf_first = GET_UF(ehci_uf);
            EXFER(xfer)->itd_first = itd_soft;
        }

        offset = EXFER(xfer)->buf_offset;
        uf_len = xfer->frlengths[EXFER(xfer)->isoc_frame];
        EXFER(xfer)->buf_offset += uf_len;
        EXFER(xfer)->isoc_frame++;

        /* enable interrupt for last xfer frame or ... */
        need_intr = (EXFER(xfer)->isoc_frame == xfer->nframes) ||
            (ehci_uf - epipe->u.isoc.last_intr_uf) > EHCI_INTR_RATE_UF;

        ehci_setup_isoc_td(epipe, itd_soft, GET_UF(ehci_uf),
            DMAADDR(&xfer->dmabuf,  offset), uf_len, need_intr);

        /* Update pipe */
        epipe->u.isoc.last_uf = ehci_uf;
        epipe->u.isoc.last_sync = 1;
        if (need_intr)
            epipe->u.isoc.last_intr_uf = ehci_uf;
    }
Exit:
    if (itd_ready)
    {
        ehci_insert_itd_to_flist(sc, epipe, 
            (juint16_t)GET_FLIST_ITD(sc, last_itd_uf), itd_soft);
    }
    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_isoc_sched exit pipe=%p\n", epipe));
}


void ehci_insert_itd_to_flist(ehci_softc_t *sc, ehci_pipe_t *epipe,
    juint16_t fl_itd, ehci_itd_soft_t *itd_soft)
{
    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_insert_itd_to_flist itd=%p "
        "fl_itd=%u %.4lx %.2x\n", 
        itd_soft, fl_itd, EOREAD4(sc, EHCI_FRINDEX), itd_soft->uf_active));
    /* save previous pipe itd ptr & frame list num */
    itd_soft->fl_num = fl_itd;
    itd_soft->flnext = sc->sc_flist_soft[fl_itd];
    sc->sc_flist_soft[fl_itd] = itd_soft;

    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, 
        dump_itd_soft("send", epipe, itd_soft));

    *itd_soft->u.td_next = sc->sc_flist[fl_itd];

    /* put itd to ehci frame list */
    sc->sc_flist[fl_itd] = itd_soft->tdlink;
}

void ehci_remove_itd_from_flist(ehci_softc_t *sc, ehci_itd_soft_t *itd_soft)
{
    ehci_itd_soft_t *p = sc->sc_flist_soft[itd_soft->fl_num];

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_remove_itd_from_flist %d\n", 
        itd_soft->fl_num));

    if (p == itd_soft)
    {
        sc->sc_flist_soft[itd_soft->fl_num] = itd_soft->flnext;
        sc->sc_flist[itd_soft->fl_num] = *itd_soft->u.td_next;
        return;
    }

    for (; p->flnext != itd_soft; p = p->flnext)
        KASSERT(p, ("EHCI: No ITD for transfer\n"));

    p->flnext = itd_soft->flnext;
    *p->u.td_next = *itd_soft->u.td_next;
}

jbool_t ehci_isoc_uf_active(ehci_pipe_t *epipe, usbd_xfer_handle xfer)
{
    if (EHCI_HIGH_SPEED(epipe))
    {
        return (EHCI_ITD_STATUS(BGET, EXFER_FIRST_ITD_UF(xfer)) & 
            EHCI_ITD_ACTIVE)&&1;
    }
    else
    {
        return (EHCI_SITD_STATUS(BGET, 
            ctoh32(EXFER(xfer)->itd_first->u.sitd->sitd_status)) & 
            EHCI_SITD_ACTIVE) && 1;
    }
}

void ehci_isoc_clear_uf_active(ehci_pipe_t *epipe, usbd_xfer_handle xfer)
{
    if (EHCI_HIGH_SPEED(epipe))
    {
        EXFER_FIRST_ITD_UF(xfer) &= ~htoc32(
            EHCI_ITD_STATUS(BMASK, EXFER_FIRST_ITD_UF(xfer)));
    }
    else
    {
        EXFER(xfer)->itd_first->u.sitd->sitd_status &= ~htoc32(
            EHCI_SITD_STATUS(BMASK, EHCI_SITD_ACTIVE));
    }
}

static void ehci_isoc_check_intr(ehci_softc_t *sc, ehci_pipe_t *epipe)
{
    usbd_xfer_handle xfer;

    if (!TAILQ_FIRST(&epipe->u.isoc.itd_run_list))
        return;

    while ((xfer = TAILQ_FIRST(&epipe->pipe.queue)) != NULL)
    {
        /* xfer transmit is not started */
        if (!EXFER(xfer)->isoc_frame)
        {
            /* xfer transmit is not started */
            return;
        }
        while (EXFER(xfer)->isoc_frame_done != EXFER(xfer)->isoc_frame)
        {
            /* If first xfer uframe is active */
            if (ehci_isoc_uf_active(epipe, xfer))
                return;
            ehci_isoc_uf_done(sc, epipe, xfer);
        }

        if (xfer->hcflags & UXFER_ABORTING)
            xfer->status = USBD_CANCELLED;
        else
        {
            if (EXFER(xfer)->isoc_frame_done < xfer->nframes)
                return;

            if (xfer->status == USBD_IN_PROGRESS)
                xfer->status = USBD_NORMAL_COMPLETION;
        }

        ehci_xfer_complete(xfer);        
    }
}

static void ehci_device_isoc_abort(usbd_xfer_handle xfer)
{
    ehci_softc_t *sc = (ehci_softc_t *)xfer->pipe->device->bus;
    struct ehci_pipe *epipe = (struct ehci_pipe *)xfer->pipe;

    DBG_V(DHCD_TRANSFER, ("EHCI: ehci_device_isoc_abort\n"));
    xfer->hcflags |= UXFER_ABORTING;

    while (EXFER(xfer)->isoc_frame_done != EXFER(xfer)->isoc_frame)
    {
        /* If first xfer uframe is active */
        if (ehci_isoc_uf_active(epipe, xfer))
        {
            if ((EXFER(xfer)->itd_first->fl_num - EHCI_GET_FRINDEX(sc)) <
                EHCI_SAFE_TRESHOLD)
            {
                DBG_V(DHCD_TRANSFER, ("EHCI: abort - wait 100 ms\n"));
                jdelay_ms(100);
            }
            else
            {
                /* Remove uframe transaction active status */
                ehci_isoc_clear_uf_active(epipe, xfer);

                ehci_isoc_uf_done(sc, epipe, xfer);
            }
        }
        else
            ehci_isoc_uf_done(sc, epipe, xfer);
    }

    if (xfer->status != USBD_CANCELLED)
    {
        xfer->status = USBD_CANCELLED;
        ehci_xfer_complete(xfer);        
    }
}

static void update_xfer_status(usbd_xfer_handle xfer, juint8_t status)
{
    if (status && xfer->status == USBD_IN_PROGRESS)
    {
        if (EHCI_HIGH_SPEED(xfer->pipe))
        {
            switch (status)
            {
            case EHCI_ITD_BABBLE:
            case EHCI_ITD_BUFERR:
            case EHCI_ITD_XACTERR:
            default:
                xfer->status = USBD_IOERROR;
            }
        }
        else
        {
            switch (status)
            {
            case EHCI_SITD_ERR:
            case EHCI_SITD_BUFERR:
            case EHCI_SITD_BABBLE:
            case EHCI_SITD_XACTERR:
            case EHCI_SITD_MISSEDUF:
            default:
                xfer->status = USBD_IOERROR;
            }
        }
    }
}

static void ehci_isoc_sitd_done(ehci_softc_t *sc, ehci_pipe_t *epipe, 
    usbd_xfer_handle xfer)
{
    usb_endpoint_descriptor_t *ep = epipe->pipe.endpoint->edesc;
    ehci_itd_soft_t *itd_soft = EXFER(xfer)->itd_first;
    ehci_itd_soft_t *itd_soft_next;
    ehci_sitd_t      *sitd = itd_soft->u.sitd;
    juint8_t         status;

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_isoc_sitd_done itd_soft=%p \n"
        "%ld %ld\n", itd_soft, EXFER(xfer)->isoc_frame,
        EXFER(xfer)->isoc_frame_done));

    status = (juint8_t)EHCI_SITD_STATUS(BGET, ctoh32(sitd->sitd_status));

    /* Clear "Complete-Split" status, it is not an error */
    status &= ~EHCI_SITD_COMPLETE;

    update_xfer_status(xfer, status);

    if (UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN)
    {
        xfer->frlengths[EXFER(xfer)->isoc_frame_done] -= 
            (juint16_t)EHCI_SITD_LEN(BGET, ctoh32(sitd->sitd_status));
    }

    if (xfer->status == USBD_IN_PROGRESS)
        xfer->actlen += xfer->frlengths[EXFER(xfer)->isoc_frame_done];

    EXFER(xfer)->isoc_frame_done++;

    /* free sitd go to next sitd */

    /* remove itd from HW */
    ehci_remove_itd_from_flist(sc, itd_soft);

    DBG_RUN(DHCD_TRANSFER, DL_EX_VERBOSE, 
        dump_itd_soft("receive", epipe, itd_soft));

    itd_soft_next = TAILQ_NEXT(itd_soft, next);
    pipe_free_itd_slot(epipe, itd_soft);

    itd_soft = itd_soft_next;

    if (EXFER(xfer)->isoc_frame_done != EXFER(xfer)->isoc_frame)
    {
        KASSERT(itd_soft, ("frame=%ld done=%ld\n", 
            EXFER(xfer)->isoc_frame_done, EXFER(xfer)->isoc_frame));

        EXFER(xfer)->itd_first = itd_soft;
    }
}

static void ehci_isoc_itd_uf_done(ehci_softc_t *sc, ehci_pipe_t *epipe, 
    usbd_xfer_handle xfer)
{
    usb_endpoint_descriptor_t *ep = epipe->pipe.endpoint->edesc;
    ehci_itd_soft_t *itd_soft = EXFER(xfer)->itd_first;
    ehci_itd_soft_t *itd_soft_next;
    ehci_itd_t      *itd = itd_soft->u.itd;
    juint8_t         uf_n =  EXFER(xfer)->itd_uf_first;
    juint8_t         status;
    juint16_t        done_size;

    DBG_X(DHCD_TRANSFER, ("EHCI: ehci_isoc_uf_done itd_soft=%p(%u %.2x) \n"
        "%ld %ld\n",itd_soft, uf_n, itd_soft->uf_active,
        EXFER(xfer)->isoc_frame, EXFER(xfer)->isoc_frame_done));

    status = (juint8_t)EHCI_ITD_STATUS(BGET, EXFER_FIRST_ITD_UF(xfer));

    update_xfer_status(xfer, status);

    done_size = (juint16_t)EHCI_ITD_LEN(BGET, ctoh32(itd->itd_trans[uf_n]));

    xfer->actlen += done_size;

    if (UE_GET_DIR(ep->bEndpointAddress) == UE_DIR_IN)
        xfer->frlengths[EXFER(xfer)->isoc_frame_done] = done_size;

    EXFER(xfer)->isoc_frame_done++;

    /* free uframe */
    itd_soft->uf_active &= ~(1 << uf_n);

    /* free itd if nessesary and go to next itd */
    if (!itd_soft->uf_active)
    {
        /*remove itd from HW*/
        ehci_remove_itd_from_flist(sc, itd_soft);

        itd_soft_next = TAILQ_NEXT(itd_soft, next);
        pipe_free_itd_slot(epipe, itd_soft);

        itd_soft = itd_soft_next;
        uf_n = 0;
    }

    if (EXFER(xfer)->isoc_frame_done != EXFER(xfer)->isoc_frame)
    {
        KASSERT(itd_soft, ("frame=%ld done=%ld\n", 
            EXFER(xfer)->isoc_frame_done, EXFER(xfer)->isoc_frame));
        KASSERT(itd_soft->uf_active, ("xxx\n"));

        /* find next uframe */
        for (; !(itd_soft->uf_active & (1 << uf_n)); uf_n++)
            ;

        EXFER(xfer)->itd_uf_first = uf_n;
        EXFER(xfer)->itd_first = itd_soft;
    }
}

static void ehci_isoc_uf_done(ehci_softc_t *sc, ehci_pipe_t *epipe, 
    usbd_xfer_handle xfer)
{
    if (EHCI_HIGH_SPEED(epipe))
        ehci_isoc_itd_uf_done(sc, epipe, xfer);
    else
        ehci_isoc_sitd_done(sc, epipe, xfer);
}

static void ehci_device_isoc_done(usbd_xfer_handle xfer) 
{
}

#ifdef CONFIG_JHOST_ETEST
static jresult_t suspend_resume_test(ehci_softc_t *sc)
{
    /* Host issues SOF for 15 seconds */
    DBG_I(DHOST_ETEST, ("suspend_resume_test: Continue SOF for 15 seconds\n"));
    jdelay_ms(15*1000);

    /* Host suspends downstream port */
    DBG_I(DHOST_ETEST, ("suspend_resume_test: Suspend the port\n"));
    ehci_port_suspend(sc, 1);

    /* Wait for 15 seconds */
    DBG_I(DHOST_ETEST, ("suspend_resume_test: Waiting for 15 more seconds "
        "before resume\n"));
    jdelay_ms(15*1000);

    /* Host resumes port */
    DBG_I(DHOST_ETEST, ("suspend_resume_test: Resume the port\n"));
    ehci_port_resume(sc, 1);

    return 0;
}

static jresult_t single_step_set_feature_test(ehci_softc_t *sc, 
    usbd_device_handle dev)
{
    usb_device_descriptor_t d;
    usbd_xfer_handle xfer;
    struct ehci_xfer *exfer;
    struct ehci_pipe *epipe = (struct ehci_pipe *)dev->default_pipe;
    juint8_t addr = dev->address;
    ehci_soft_qtd_t *setup = NULL, *stat = NULL, *data = NULL, *end;
    ehci_soft_qh_t *sqh;
    jint_t len = USB_DEVICE_DESCRIPTOR_SIZE;
    jresult_t rc = 0;

    /* Allocate "dummy" CORE xfer */
    xfer = usbd_alloc_xfer(dev);
    if (xfer == NULL)
    {
        rc = JENOMEM;
        goto Exit;
    }
    /* HSU addition: Memory leak */
    if (ehci_allocm(&sc->sc_bus, &xfer->dmabuf, USB_DEVICE_DESCRIPTOR_SIZE) == USBD_NOMEM)
    {
        rc = JENOMEM;
        goto Exit;
    }
    /* End HSU addition */
    exfer = (struct ehci_xfer *)xfer;

    /* Initialize GET_DESC_DEVICE request */
    xfer->request.bmRequestType = UT_READ_DEVICE;
    xfer->request.bRequest = UR_GET_DESCRIPTOR;
    USETW2(xfer->request.wValue, UDESC_DEVICE, 0);
    USETW(xfer->request.wIndex, 0);
    USETW(xfer->request.wLength, len);
    xfer->buffer = (void *)&d;
    xfer->length = USB_DEVICE_DESCRIPTOR_SIZE;
    xfer->rqflags |= URQ_REQUEST;

    /* Initialize the SETUP TD */
    setup = ehci_alloc_sqtd(sc);
    if (setup == NULL) 
    {
        rc = JENOMEM;
        goto Exit;
    }
    j_memcpy(KERNADDR(&epipe->u.ctl.reqdma, 0), (void *)&xfer->request, 
        sizeof(usb_device_request_t));
    
    /* Clear toggle */
    setup->qtd.qtd_status = htoc32(
        EHCI_QTD_ACTIVE |
        EHCI_QTD_SET_PID(EHCI_QTD_PID_SETUP) |
        EHCI_QTD_SET_CERR(3) |
        EHCI_QTD_SET_TOGGLE(0) |
        EHCI_QTD_SET_BYTES(sizeof(usb_device_request_t)) |
        EHCI_QTD_IOC
        );
    setup->qtd.qtd_buffer[0] = htoc32(DMAADDR(&epipe->u.ctl.reqdma, 0));
    setup->qtd.qtd_buffer_hi[0] = 0;
    setup->nextqtd = NULL;
    setup->qtd.qtd_next = setup->qtd.qtd_altnext = EHCI_NULL;
    setup->xfer = xfer;
    setup->len = sizeof(usb_device_request_t);

    /* Update EP0 QH */
    sqh = epipe->sqh;
    epipe->u.ctl.length = USB_DEVICE_DESCRIPTOR_SIZE;

    /* Update device address and length since they may have changed
       during the setup of the control pipe in usbd_new_device(). */
    sqh->qh.qh_endp =
        (sqh->qh.qh_endp & htoc32(~(EHCI_QH_ADDRMASK | EHCI_QH_MPLMASK))) |
        htoc32(
            EHCI_QH_SET_ADDR(addr) |
            EHCI_QH_SET_MPL(UGETW(epipe->pipe.endpoint->edesc->wMaxPacketSize))
            );

    /* Start the setup transfer */
    DBG_I(DHOST_ETEST, ("single_step_set_feature_test: Sending SETUP "
        "packet\n"));
    ehci_set_qh_qtd(sqh, setup);

    /* Wait 15 seconds before sending the IN token for the data stage */
    DBG_I(DHOST_ETEST, ("single_step_set_feature_test: Sending SOF for 15 "
        "seconds\n"));
    jdelay_ms(15 * 1000);
    ehci_free_sqtd(sc, setup);
    setup = NULL;

    /* Build data and status stage TDs */
    stat = ehci_alloc_sqtd(sc);
    if (stat == NULL) 
    {
        rc = JENOMEM;
        goto Exit;
    }

    /* Set up data transaction */
    /* Start toggle at 1. */
    epipe->nexttoggle = 1;
    rc = ehci_alloc_sqtd_chain(epipe, sc, USB_DEVICE_DESCRIPTOR_SIZE, 1, 
        xfer, &data, &end);
    if (rc)
        goto Exit;
    end->qtd.qtd_status &= htoc32(~EHCI_QTD_IOC);
    end->nextqtd = stat;
    end->qtd.qtd_next =
        end->qtd.qtd_altnext = htoc32(stat->physaddr);

    stat->qtd.qtd_status = htoc32(
        EHCI_QTD_ACTIVE |
        EHCI_QTD_SET_PID(EHCI_QTD_PID_OUT) |
        EHCI_QTD_SET_CERR(3) |
        EHCI_QTD_SET_TOGGLE(1) |
        EHCI_QTD_IOC
        );
    stat->qtd.qtd_buffer[0] = 0; 
    stat->qtd.qtd_buffer_hi[0] = 0; 
    stat->nextqtd = NULL;
    stat->qtd.qtd_next = stat->qtd.qtd_altnext = EHCI_NULL;
    stat->xfer = xfer;
    stat->len = 0;

    exfer->sqtdstart = data;
    exfer->sqtdend = stat;

    /* Start the data transfer */
    DBG_I(DHOST_ETEST, ("single_step_set_feature_test: Continue with DATA and "
        "STATUS stages\n"));
    ehci_set_qh_qtd(sqh, data);

    /* Wait for the transfer to finish */
    jdelay_ms(2500);

Exit:
    /* Cleanup */
    if (data)
        ehci_free_sqtd_chain(sc, data, NULL);
    if (stat)
        ehci_free_sqtd(sc, stat);
    if (setup)
        ehci_free_sqtd(sc, setup);
    if (xfer)
    {
        if (xfer->dmabuf)
            ehci_freem(&sc->sc_bus, &xfer->dmabuf);
        usbd_free_xfer(xfer);
    }
    return rc;
}

static jresult_t ehci_set_hw_test_mode(ehci_softc_t *sc, juint32_t test_mode)
{
    __VOLATILE__ juint32_t v;
    jint_t i;

    /* Disable the periodic and asynchronous schedules by setting the 
     * Asynchronous Schedule Enable and Periodic Schedule Enable bits in the
     * USBCMD register to a zero. */
    v = EOREAD4(sc, EHCI_USBCMD);
    v &= ~(EHCI_CMD_ASE | EHCI_CMD_PSE);
    EOWRITE4(sc, EHCI_USBCMD, v);

    /* Place all enabled root ports into the suspended state by setting the 
     * Suspend bit in each appropriate PORTSC register to a one. */
    for (i = 1; i <= sc->sc_noport; i++)
        ehci_port_suspend(sc, i);

    /* Set the Run/Stop bit in the USBCMD register to a zero and wait for 
     * the HCHalted bit in the USBSTS register, to transition to a one. 
     * Note that an EHCI host controller implementation may optionally
     * allow port testing with the Run/Stop bit set to a one. However, all
     * host controllers must support port testing with Run/Stop set to a 
     * zero and HCHalted set to a one. */
    v = EOREAD4(sc, EHCI_USBCMD);
    v &= ~EHCI_CMD_RS;
    EOWRITE4(sc, EHCI_USBCMD, v);

    do {
        v = EOREAD4(sc, EHCI_USBSTS);
    } while (!(v & EHCI_STS_HCH));

    /* Set the Port Test Control field in the port under test PORTSC 
     * register to the value corresponding to the desired test mode.
     * If the selected test is Test_Force_Enable, then the Run/Stop bit in
     * the USBCMD register must then be transitioned back to one, in order 
     * to enable transmission of SOFs out of the port under test. */
    v = EOREAD4(sc, EHCI_PORTSC(1));
    v |= test_mode;
    EOWRITE4(sc, EHCI_PORTSC(1), v);

    /* For FORCE_ENABLE mode, the Run/Stop bit should be set to one */
    if (test_mode == EHCI_PS_TEST_FORCE_ENABLE_HS)
    {
        v = EOREAD4(sc, EHCI_USBCMD);
        v |= EHCI_CMD_RS;
            EOWRITE4(sc, EHCI_USBCMD, v);
        }
    return 0;
}

static jresult_t ehci_set_etest_mode(ehci_softc_t *sc, void *arg)
{
    jresult_t rc = 0;
    etest_param_t *param = (etest_param_t *)arg;
    juint32_t portsc_test_mode = 0;

    switch (param->mode)
    {
    case TEST_MODE_SUSPEND_RESUME:
        rc = suspend_resume_test(sc);
        break;
    case TEST_MODE_SINGLE_STEP_SET_FEATURE:
        rc = single_step_set_feature_test(sc, param->test_dev);
        break;
    case TEST_MODE_J_STATE:
        portsc_test_mode = EHCI_PS_TEST_J_STATE;
        break;
    case TEST_MODE_K_STATE:
        portsc_test_mode = EHCI_PS_TEST_K_STATE;
        break;
    case TEST_MODE_SE0NAK:
        portsc_test_mode = EHCI_PS_TEST_SE0_NAK;
        break;
    case TEST_MODE_PACKET:
        portsc_test_mode = EHCI_PS_TEST_PACKET;
        break;
    case TEST_MODE_FORCE_ENABLE:
        portsc_test_mode = EHCI_PS_TEST_FORCE_ENABLE_HS;
        break;
    default:
        rc = JEINVAL;
        break;
    }

    if (portsc_test_mode)
        rc = ehci_set_hw_test_mode(sc, portsc_test_mode);

    return rc;
}
#endif
    
#ifdef CONFIG_JOTG
static jresult_t forward_otg_ioctl(ehci_softc_t *sc, drv_ioctl_t ioctl, 
    void *arg)
{
    jresult_t rc = 0;

    if (sc->ocd_handle)
        rc = j_device_ioctl(sc->ocd_handle, ioctl, arg);
    else
    {
        switch(ioctl)
        {
        case DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED:       
            ((periph_support_t *)(arg))->is_supported = 1;
            break;
        default:
            break;
        }
    }

    return rc;
}

static jresult_t forward_hub_ioctl(ehci_softc_t *sc, drv_ioctl_t ioctl, 
    void *arg)
{
    return j_device_ioctl(sc->sc_bus.root_hub->bdev, ioctl, arg);
}
#endif

jresult_t ehci_ioctl(ehci_softc_t *sc, drv_ioctl_t ioctl, void *arg)
{
    jresult_t rc = JEINVAL;

    DBG_V(DHCD_TRANSFER, ("EHCI: IOCTL ioctl=%x arg=%p\n", ioctl, arg));
    
    switch(ioctl)
    {
#ifdef CONFIG_JOTG
    case DRV_IOCTL_HOST_SUSPEND:
    case DRV_IOCTL_HOST_RESUME:
    case DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED:       
    case DRV_IOCTL_HOST_REMOTE_CONNECTED:
    case DRV_IOCTL_HOST_REMOTE_DISCONNECTED:
    case DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED:
    case DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED:
        rc = forward_otg_ioctl(sc, ioctl, arg);
        break;

    case DRV_IOCTL_HOST_PORT_ENABLE:
        if (sc->ocd_handle)
        {
            rc = ehci_enable(sc);
            if (rc)
                break;
            rc = forward_hub_ioctl(sc, ioctl, arg);
        }                    
        break;

    case DRV_IOCTL_HOST_PORT_DISABLE:
        if (sc->ocd_handle)
            sc->otg_host_enabled = 0;
        /* HSU addition - The interrupt EHCI_STS_FLR is handled 
        by the interrupt handler ehci_intr. We have to disable
        it before we tear down ehci_intr or else it could rise 
        periodically, and cause resets.  */
        sc->sc_eintrs &= ~EHCI_STS_FLR;
        EOWRITE4(sc, EHCI_USBINTR, sc->sc_eintrs);
        /* End of HSU addition */
    case DRV_IOCTL_HOST_PORT_SUSPEND:
    case DRV_IOCTL_HOST_PORT_RESUME:
    case DRV_IOCTL_HOST_ENABLE_HNP:
        if (sc->ocd_handle)
            rc = forward_hub_ioctl(sc, ioctl, arg);
        break;
#endif
#ifdef CONFIG_JHOST_ETEST
    case DRV_IOCTL_SET_ETEST_MODE:    
        rc = ehci_set_etest_mode(sc, arg);
        break;
#endif
    case DRV_IOCTL_HOST_SET_PARAMS:
        sc->itc_num = (juint32_t)arg;
        rc = 0;
        break;
    default:
        DBG_E(DHCD_TRANSFER, ("EHCI: Unsupported IOCTL %x\n", ioctl));
        break;
    }

    return rc;
}

static void ehci_xfer_complete(usbd_xfer_handle xfer)
{
    EXFER(xfer)->started = 0;
    usb_transfer_complete(xfer);
}
#ifdef CONFIG_PORT_INTERCHIP
static jbool_t ehci_is_port_interchip(void *context, juint8_t index)
{
    ehci_softc_t *sc = (ehci_softc_t *)context;

    return (sc->interchip_ports & (1 << index)) ? 1 : 0;
}
#endif

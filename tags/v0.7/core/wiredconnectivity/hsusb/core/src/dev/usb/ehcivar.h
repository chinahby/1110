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

#ifndef __EHCI_VAR_H__
#define __EHCI_VAR_H__

#include <jos.h>

#define EHCI_UFRAME_IN_FRAME      8
#define EHCI_UFRAME_US            125
#define EHCI_UFRAME_IN_ITD        EHCI_ITD_NTRANS

#define GET_ITD(itd_uf) ((itd_uf) / EHCI_UFRAME_IN_ITD)
#define GET_UF(itd_uf)  ((juint8_t)((itd_uf) % EHCI_UFRAME_IN_ITD))

#define GET_FLIST_ITD(sc, uf) ((uf) / EHCI_UFRAME_IN_ITD % sc->sc_flsize)

/* recommended min intr period for low interval isoc pipe 
 * can be critial for low speed cpu (or not?) */
#define EHCI_INTR_RATE            50 /* ms / frames */
#define EHCI_INTR_RATE_UF         \
    ((juint16_t)((juint32_t)EHCI_INTR_RATE*1000/EHCI_UFRAME_US))

#define EHCI_SAFE_TRESHOLD        4 /* number of itd's */
#define EHCI_SAFE_TRESHOLD_UF     (EHCI_SAFE_TRESHOLD*EHCI_UFRAME_IN_ITD)

typedef struct ehci_itd_soft
{
    TAILQ_ENTRY(ehci_itd_soft)   next;
    struct ehci_itd_soft        *flnext;

    ehci_link_t                  tdlink;
    juint16_t                     fl_num;
    union
    {
        ehci_itd_t              *itd;
        ehci_sitd_t             *sitd;
        ehci_link_t             *td_next;
        void                    *td;
    } u;

    juint8_t                      uf_active;    
    juint8_t                      next_page;    
} ehci_itd_soft_t;
#define EHCI_HIGH_SPEED(pipe) \
    (((usbd_pipe_handle)pipe)->device->speed == USB_SPEED_HIGH)
#define EHCI_ITD_SIZE (((sizeof (ehci_itd_t) + EHCI_ITD_ALIGN - 1) / \
    EHCI_ITD_ALIGN) * EHCI_ITD_ALIGN)

#define EHCI_SITD_SIZE (((sizeof (ehci_sitd_t) + EHCI_SITD_ALIGN - 1) / \
    EHCI_SITD_ALIGN) * EHCI_SITD_ALIGN)

typedef struct ehci_soft_qtd {
    ehci_qtd_t qtd;
    struct ehci_soft_qtd *nextqtd; /* mirrors nextqtd in TD */
    ehci_physaddr_t physaddr;
    usbd_xfer_handle xfer;
    LIST_ENTRY(ehci_soft_qtd) hnext;
    juint16_t len;
} ehci_soft_qtd_t;
#define EHCI_SQTD_SIZE \
    (((sizeof (struct ehci_soft_qtd) + EHCI_QTD_ALIGN - 1) / \
    EHCI_QTD_ALIGN) * EHCI_QTD_ALIGN)
#define EHCI_SQTD_CHUNK (EHCI_PAGE_SIZE / EHCI_SQTD_SIZE)

typedef struct ehci_soft_qh {
    ehci_qh_t qh;
    struct ehci_soft_qh *next;
    struct ehci_soft_qtd *sqtd;
    ehci_physaddr_t physaddr;
    jint_t islot;                /* Interrupt list slot. */
} ehci_soft_qh_t;
#define EHCI_SQH_SIZE \
    ((sizeof (struct ehci_soft_qh) + EHCI_QH_ALIGN - 1) / \
    EHCI_QH_ALIGN * EHCI_QH_ALIGN)
#define EHCI_SQH_CHUNK (EHCI_PAGE_SIZE / EHCI_SQH_SIZE)

struct ehci_xfer {
    struct usbd_xfer xfer;
    LIST_ENTRY(ehci_xfer) inext; /* list of active xfers */
    ehci_soft_qtd_t *sqtdstart;
    ehci_soft_qtd_t *sqtdend;
    jbool_t started;
#ifdef DIAGNOSTIC
    jint_t isdone;
#endif
    ehci_itd_soft_t *itd_first;
    juint8_t itd_uf_first;
    juint32_t isoc_frame;
    juint32_t isoc_frame_done;
    juint32_t buf_offset;
};
#define EHCI_XFER_ABORTING      0x0001  /* xfer is aborting. */
#define EHCI_XFER_ABORTWAIT     0x0002  /* abort completion is being awaited. */

#define EXFER(xfer) ((struct ehci_xfer *)(xfer))
#define EXFER_FIRST_ITD_UF(xfer) \
    (EXFER(xfer)->itd_first->u.itd->itd_trans[EXFER(xfer)->itd_uf_first])

/*
 * Information about an entry in the interrupt list.
 */
struct ehci_soft_islot {
    ehci_soft_qh_t *sqh;                /* Queue Head. */
};

#define EHCI_FRAMELIST_MAXCOUNT 1024
#define EHCI_IPOLLRATES         8       /* Poll rates (1ms, 2, 4, 8 ... 128) */
#define EHCI_INTRQHS            ((1 << EHCI_IPOLLRATES) - 1)
#define EHCI_MAX_POLLRATE       (1 << (EHCI_IPOLLRATES - 1))
#define EHCI_IQHIDX(lev, pos) \
        ((((pos) & ((1 << (lev)) - 1)) | (1 << (lev))) - 1)
#define EHCI_ILEV_IVAL(lev)     (1 << (lev))

#define EHCI_HASH_SIZE 128
#define EHCI_COMPANION_MAX 8


typedef struct ehci_dma_allocs {
    LIST_ENTRY(ehci_dma_allocs) list;
    jdma_handle dma;
} ehci_dma_allocs_t;

typedef struct ehci_pipe {
    struct usbd_pipe pipe;

    TAILQ_ENTRY(ehci_pipe) next;

    jint_t nexttoggle;

    ehci_soft_qh_t *sqh;
    union {
        ehci_soft_qtd_t *qtd;
        /* ehci_soft_itd_t *itd; */
    } tail;
    union {
        /* Control pipe */
        struct {
            jdma_handle reqdma;
            juint_t length;
        } ctl;
        /* Interrupt pipe */
        struct {
            juint_t length;
        } intr;
        /* Bulk pipe */
        struct {
            juint_t length;
        } bulk;
        /* Isoc pipe */
        struct {
            jdma_handle       itd_pool;
            ehci_itd_soft_t  *itd_soft_pool;
            juint16_t          itd_pool_size;
            TAILQ_HEAD(itd_free_list_m, ehci_itd_soft) itd_free_list;
            TAILQ_HEAD(itd_run_list_m,  ehci_itd_soft) itd_run_list;

            juint16_t  interval_uf;
            juint16_t  shift_uf;

            juint16_t  last_uf;
            juint16_t  last_intr_uf;
            jbool_t    last_sync;
            juint8_t   hubport;
            juint8_t   hubaddr;
        } isoc;
    } u;
} ehci_pipe_t;


typedef struct ehci_softc {
    struct usbd_bus sc_bus;             /* base device */
    jint_t sc_flags;
#define EHCI_SCFLG_DYING        0x0001
#define EHCI_SCFLG_DONEINIT     0x0002  /* ehci_init() has been called. */
#define EHCI_SCFLG_DONEHWINIT   0x0004
#define EHCI_ENB_INTR_ONLY      0x0010
#define EHCI_DOOR_BELL          0x0020
#define EHCI_TRANSFER           0x0040
#define EHCI_FL_ROLLOVER        0x0100
#define EHCI_TRANSMIT_COMPLETE  0x0200

    jbus_resource_h io_res;
    jbus_resource_h irq_res;
    void *interrupt_h;
    juint_t sc_offs;                     /* offset to operational regs */

    char sc_vendor[32];                 /* vendor string for root hub */
    jint_t sc_id_vendor;                 /* vendor ID for root hub */

    juint32_t sc_cmd;                    /* shadow of cmd reg during suspend */

    juint_t sc_ncomp;
    juint_t sc_npcomp;

    jdma_handle sc_fldma;
    ehci_link_t *sc_flist;
    juint_t sc_flsize;
    juint_t sc_rand;                     /* XXX need proper intr scheduling */

    struct ehci_soft_islot sc_islots[EHCI_INTRQHS];

    LIST_HEAD(ehci_sc_intrhead, ehci_xfer) sc_intrhead;
    LIST_HEAD(ehci_sc_sma_allocs, ehci_dma_allocs) sc_dma_allocs;

    ehci_soft_qh_t *sc_freeqhs;
    ehci_soft_qtd_t *sc_freeqtds;

    jint_t sc_noport;
    juint8_t  sc_hasppc;         /* has Port Power Control */
    juint8_t  sc_addr;           /* device address */
    juint8_t  sc_conf;           /* device configuration */
    usbd_xfer_handle sc_intrxfer;
#define EHCI_PORT_IS_RESET      0x01
#define EHCI_PORT_IS_AWAKEN     0x02
    juint8_t sc_port_flags[EHCI_MAX_PORTS];

    juint32_t sc_eintrs;
    ehci_soft_qh_t *sc_async_head;

    /* free xfers */
    TAILQ_HEAD(ehci_sc_free_xfers, usbd_xfer) sc_free_xfers; 

    jdevice_t sc_child;         /* /dev/usb# device */

    char sc_dying;
    jbool_t sc_suspend;
    jmsleep_h sleep_handle;
    juint16_t fl_uf_num;             /* periodic frame list uframe count */
    juint32_t itc_num;

    ehci_itd_soft_t **sc_flist_soft;
    TAILQ_HEAD(isoc_pipe_list_m, ehci_pipe) isoc_pipe_list;

#ifdef CONFIG_JOTG
    jdevice_t ocd_handle;
    jbool_t otg_host_enabled;
#endif
    jspinlock_h sc_lock;
#ifdef CONFIG_PORT_INTERCHIP
    /* Mask for all interchip ports */
    juint16_t interchip_ports;
#endif
} ehci_softc_t;

#define EREAD1(sc, a) j_bus_space_read_1((sc)->io_res, (a))
#define EREAD2(sc, a) j_bus_space_read_2((sc)->io_res, (a))
#define EREAD4(sc, a) j_bus_space_read_4((sc)->io_res, (a))
#define EWRITE1(sc, a, x) j_bus_space_write_1((sc)->io_res, (a), (x))
#define EWRITE2(sc, a, x) j_bus_space_write_2((sc)->io_res, (a), (x))
#define EWRITE4(sc, a, x) j_bus_space_write_4((sc)->io_res, (a), (x))
#define EOREAD1(sc, a) j_bus_space_read_1((sc)->io_res, (sc)->sc_offs+(a))
#define EOREAD2(sc, a) j_bus_space_read_2((sc)->io_res, (sc)->sc_offs+(a))
#define EOREAD4(sc, a) j_bus_space_read_4((sc)->io_res, (sc)->sc_offs+(a))
#define EOWRITE1(sc, a, x) j_bus_space_write_1((sc)->io_res, (sc)->sc_offs+(a), (x))
#define EOWRITE2(sc, a, x) j_bus_space_write_2((sc)->io_res, (sc)->sc_offs+(a), (x))
#define EOWRITE4(sc, a, x) j_bus_space_write_4((sc)->io_res, (sc)->sc_offs+(a), (x))

usbd_status     ehci_init(ehci_softc_t *sc);
jint_t           ehci_intr(void *v);
void            ehci_softintr(void *v);
jint_t           ehci_detach(ehci_softc_t *sc, jint_t flags);
void            ehci_freemem(ehci_softc_t *sc);
void            ehci_shutdown(void *v);
jresult_t        ehci_ioctl(ehci_softc_t *sc, drv_ioctl_t ioctl, void *arg);
#endif

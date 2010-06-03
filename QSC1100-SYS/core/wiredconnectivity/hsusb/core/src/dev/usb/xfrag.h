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
#ifndef _XFRAG_H_
#define _XFRAG_H_

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

/* Number of frags to preallocate (minimize allocation at runtime) */
#define XFRAG_PREALLOCATE       100

struct xfrag {
    struct xfrag *next; /* Next fragment in chain */
    void *cpu_addr;     /* Address in cpu memory */
    void *usb_addr;     /* Address for use by the usb controller */
    juint32_t offset;    /* Offset in cpu memory */
    juint32_t size;      /* Fragment size */

#define XFRAG_NOT_STARTED       0x0000
#define XFRAG_READY             0x0001
#define XFRAG_WORKING           0x0002
#define XFRAG_STATE_MASK        0x0003

#define XFRAG_SETUP             0x0010
#define XFRAG_DATA              0x0020  /* Also bulk */
#define XFRAG_STATUS            0x0030
#define XFRAG_TOKEN_MASK        0x00f0

#define XFRAG_IN                0x0100
#define XFRAG_OUT               0x0000
#define XFRAG_DIR_MASK          0x0100

#define XFRAG_TOGGLE            0x0400
#define XFRAG_SKIP              0x0800

#define XFRAG_LAST              0x1000
#define XFRAG_ERROR             0x2000

#define XFRAG_DEFAULT_SETUP  (XFRAG_SETUP  | XFRAG_OUT    | XFRAG_NOT_STARTED)
#define XFRAG_DEFAULT_DATA   (XFRAG_DATA   | XFRAG_TOGGLE | XFRAG_NOT_STARTED)
#define XFRAG_DEFAULT_STATUS (XFRAG_STATUS | XFRAG_TOGGLE | XFRAG_NOT_STARTED)
#define XFRAG_DEFAULT_BULK   (XFRAG_DATA   | XFRAG_NOT_STARTED)
#define XFRAG_DEFAULT_ISOC   (XFRAG_DATA   | XFRAG_NOT_STARTED)
    juint16_t flags;
    void *priv;
};

#define XFRAG_IS_NULL(x) \
    (!(x))

#define XFRAG_IS_OUT(x) \
    ((x) && ((x)->flags & XFRAG_DIR_MASK) == XFRAG_OUT)

#define XFRAG_IS_IN(x) \
    ((x) && ((x)->flags & XFRAG_DIR_MASK) == XFRAG_IN)

#define XFRAG_IS_WORKING(x) \
    ((x) && ((x)->flags & XFRAG_STATE_MASK) == XFRAG_WORKING)

#define XFRAG_IS_READY(x) \
    ((x) && ((x)->flags & XFRAG_STATE_MASK) == XFRAG_READY)

#define XFRAG_IS_NOT_STARTED(x) \
    ((x) && ((x)->flags & XFRAG_STATE_MASK) == XFRAG_NOT_STARTED)

#define XFRAG_CHECK_NEXT(x, head) \
    ((x) ? xfrag_next(x) : (head))

#define XFRAG_SET_STATE(frag, state) \
    ((frag) ? (frag)->flags = ((frag)->flags & ~XFRAG_STATE_MASK) | (state) : 0)

#define XFRAG_NO_DMA  0
#define XFRAG_USE_DMA 1

/* Allocate a chain out of a xfer structure */
jresult_t xfrag_alloc_chain(usbd_xfer_handle xfer, juint32_t frag_size,
    void **buf_arr, juint32_t buf_count, juint32_t *buf_start, juint8_t *toggle,
    struct xfrag **head, juint8_t use_dma);

/* Free a chain of frags allocated with xfrags_alloc */
void xfrag_free_chain(struct xfrag *head);

/* Get next frag */
struct xfrag *xfrag_next(struct xfrag *curr);

/* Advance USB & DMA frags according to their directions */
jresult_t xfrag_advance(struct xfrag *head, struct xfrag **usb,
    struct xfrag **dma);

/* Skip frags of the same type */
void xfrag_skip_same(struct xfrag *frag);

#endif


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
#include <jutil.h>
#include "xfrag.h"

/* Allocate a single frag (try to get from the free list first */
static struct xfrag *xfrag_alloc(void)
{
    struct xfrag *frag;

    frag = jmalloc(sizeof(struct xfrag), M_ZERO);

    if (frag)
        frag->flags = XFRAG_NOT_STARTED;
    
    return frag;
}

/* Add a frag to the free list */
static void xfrag_free(struct xfrag *frag)
{
    jfree((void *)frag);
}

/* Free a chain of frags allocated with xfrags_alloc */
void xfrag_free_chain(struct xfrag *head)
{
    struct xfrag *prev, *curr;
    prev = curr = head;

    DBG_X(DUTIL_XFRAG, ("XFRAG: Freeing frag chain starting from %p\n", head));
    while (curr)
    {
        prev = curr;
        curr = curr->next;
        DBG_X(DUTIL_XFRAG, ("XFRAG: Remove %p\n", prev));
        xfrag_free(prev);
    }
}

/* Break a transfer into a chain of fragments for an iso transfer.
 * Parameters:
 * IN   flags   - Flags to be set in frag
 * IN   xfer    - Transfer to fragment
 * IN   *phys   - Physical address to put in frag
 * IN   **buf   - Array of buffers, each givn to concecutive frag in round robin
 * IN   buf_count - Number of above buffers to distribute
 * IN/OUT start_buf - From which buffer to start round robin 
 * Return Value:
 * Head of allocated frag chain or NULL on failure */
static struct xfrag *xfrag_alloc_isoc_chain(juint32_t flags,
    usbd_xfer_handle xfer, void *phys, void **buf, juint32_t buf_count,
    juint32_t *start_buf)
{
    juint32_t i, offset = 0, curr_buf = *start_buf;
    struct xfrag *curr, *prev = NULL, *head = NULL;

    DBG_V(DUTIL_XFRAG, ("XFRAG: Allocating isoc chain buffer %p \n", phys));

    for (i = 0; i < xfer->nframes; i++)
    {
        curr = xfrag_alloc();
        if (!curr)
        {
            xfrag_free_chain(head);
            return NULL;
        }

        curr->cpu_addr  = phys;
        curr->usb_addr  = buf ? buf[curr_buf] : NULL;
        curr->offset    = offset;
        curr->flags     = (juint16_t)flags;
        curr->size      = xfer->frlengths[i];

        DBG_V(DUTIL_XFRAG, ("XFRAG: ++ Frag %p [%p:%p] offset %lx, "
            "size %ld flags %x buf %ld\n", curr, curr->cpu_addr, curr->usb_addr,
            curr->offset, curr->size, curr->flags, curr_buf));

        /* Total transfer offset */
        offset  += xfer->frlengths[i];
        
        /* Next frag */
        if (!head)
            head = prev = curr;
        else
        {
            prev->next  = curr;
            prev        = curr;
        }

        /* Advance buffer counter (if there are usb buffers in use) */
        if (buf_count)
            curr_buf = (curr_buf + 1) % buf_count;
    }

    if (prev)
    {
        prev->flags |= XFRAG_LAST;
        prev->priv = (void *)xfer;
        prev->next = NULL;
    }

    /* Save the last buffer number used */
    *start_buf = curr_buf;
    
    return head;
}

/* Allocate a chain of a particular type
 * Parameters: 
 * IN   flags   - Flags to set for each frag
 * IN   frag_size - Max fragment size
 * IN   transfer_size - Total transfer size
 * IN   max_packet - Maximum frag size
 * IN   *phys   - CPU address to give the frags
 * IN   **buf   - Array of buffers, each givn to concecutive frag in round robin
 * IN   buf_count - Number of above buffers to distribute
 * IN/OUT start_buf - From which buffer to start round robin 
 * OUT  **last  - Pointer to the last allocated fragment
 * Return Value:
 * Head of allocated frag chain or NULL on failure */
static struct xfrag *xfrag_alloc_single_chain(juint32_t flags,
    juint32_t frag_size, juint32_t transfer_size, juint16_t max_packet,
    void *phys, void **buf, juint32_t buf_count, juint32_t *buf_start,
    struct xfrag **last)
{
    juint32_t i, packets, curr_buf = buf_start ? *buf_start : 0;
    struct xfrag *curr, *next, *head;

    DBG_V(DUTIL_XFRAG, ("XFRAG: Allocating single chain. "
        "Transfer size %ld buffer %p flags %x\n",
        transfer_size, phys, flags));

    if (frag_size == 0)
    {
        DBG_E(DUTIL_XFRAG, 
            ("XFRAG: Chain with 0 sized frags cannot be allocated !\n"));
        return NULL;
    }

    /* Alloc first frag (there is allways atleast one) */
    head = curr = next  = xfrag_alloc();
    if (!head)
        return NULL;

    curr->cpu_addr      = phys;
    curr->usb_addr      = buf ? buf[curr_buf] : NULL;
    curr->offset        = 0;
    curr->flags         = (juint16_t)flags;
    curr->size          = (transfer_size > frag_size ?
        frag_size : transfer_size);
    
    DBG_V(DUTIL_XFRAG, ("XFRAG: ++ Frag [%p:%p] offset %lx, size %ld "
        "flags %x\n", curr->cpu_addr, curr->usb_addr, curr->offset, curr->size,
        curr->flags));
 
    /* Divide transfer to frags of frag_size */
    for (i = frag_size; i < transfer_size; i+= frag_size)
    {
        next = xfrag_alloc();
        if (!next)
        {
            curr->next = NULL;
            xfrag_free_chain(head);
            return NULL;
        }
        
        /* Advance buffer counter (if there are usb buffers in use) */
        if (buf_count)
            curr_buf = (curr_buf + 1) % buf_count;
        
        next->cpu_addr  = phys;
        next->usb_addr  = buf ? buf[curr_buf] : NULL;
        next->offset    = i;
        next->size      = MIN(frag_size, transfer_size - i);
        next->flags     = curr->flags;
        curr->next      = next;

        DBG_V(DUTIL_XFRAG, ("XFRAG: ++ Frag [%p:%p] offset %ld, size %ld "
            "flags %x\n", next->cpu_addr, next->usb_addr, next->offset,
            next->size, next->flags));

        /* Calculate frag toggle */
        packets = curr->size / max_packet;

        /* Check if toggle should be changed */
        if (packets % 2)
            next->flags ^= XFRAG_TOGGLE;
        
        curr = next;
    }
    curr->next = NULL;
    if (buf_start)
        *buf_start = curr_buf;

    if (last)
        *last = curr;

    return head;
}

/* Allocate a chain for the current transfer 
 * Parameters:
 * IN   xfer    - Transfer to fragment
 * IN   frag_size - Size of fragments to use
 * IN   **buf_arr - Array of buffers, each givn to concecutive frag in round robin
 * IN   buf_count - Number of above buffers to distribute
 * IN/OUT *buf_start - What buffer to start round robin at.
 * IN/OUT *toggle - What toggle to start with
 * OUT  **head  - Pointer to head of chain
 * IN   use_dma - Where to get frag data address phys or virt memory 
 * Return Value:
 * 0 on success or JEINVAL on error */
jresult_t xfrag_alloc_chain(usbd_xfer_handle xfer, juint32_t frag_size,
    void **buf_arr, juint32_t buf_count, juint32_t *buf_start, juint8_t *toggle,
    struct xfrag **head, juint8_t use_dma)
{
    struct xfrag *last, *curr;
    juint32_t dir, flags, len;
    juint16_t max_packet;
    juint32_t temp;
    usb_device_request_t *req;
    usb_endpoint_descriptor_t *desc;
    void *buffer;
    
    dir         = XFRAG_OUT; /* Status stage direction */
    req         = &xfer->request;
    desc        = xfer->pipe->endpoint->edesc;
    max_packet  = UGETW(desc->wMaxPacketSize);

    DBG_V(DUTIL_XFRAG, ("XFRAG: Alloc chain, xfer %p max %d size %ld attr %x\n",
        xfer, max_packet, frag_size, desc->bmAttributes));

    /* Control transfer, create stages */
    if ((desc->bmAttributes & UE_XFERTYPE) == UE_CONTROL)
    {
        /* Setup stage */
        
        /* XXX: We use direct address of xfer->request */
        flags           = XFRAG_DEFAULT_SETUP;
        *head           = xfrag_alloc_single_chain(flags, frag_size,
            sizeof(usb_device_request_t), max_packet, (void *)&xfer->request,
            buf_arr, buf_count, 0, &last);

        if (!*head)
            return JEINVAL;
        
        /* Data stage (if present) */
        len = UGETW(req->wLength);
        if (len)
        {
            /* Data stage must start on next buffer then ctrl */
            temp        = 1;
            curr        = last;
            dir         = (req->bmRequestType & UT_READ) ? XFRAG_IN : XFRAG_OUT;
            flags       = dir | XFRAG_DEFAULT_DATA;

            buffer = (use_dma ? (void *)DMAADDR(&xfer->dmabuf, 0) :
                (void *)KERNADDR(&xfer->dmabuf, 0)); 
            
            curr->next  = xfrag_alloc_single_chain(flags, frag_size,
                len, max_packet, buffer, buf_arr, buf_count, &temp, &last);
            
            if (!curr->next)
            {
                xfrag_free_chain(*head);
                return JEINVAL;
            }
        }
        
        /* Status stage */
        flags           = XFRAG_DEFAULT_STATUS; 
        flags           |= ~dir & XFRAG_IN; /* Opposite of data stage */
        last->next      = xfrag_alloc_single_chain(flags, frag_size, 0,
            max_packet, NULL, buf_arr, buf_count, 0, NULL);
        
        if (!last->next)
        {
            xfrag_free_chain(*head);
            return JEINVAL;
        }
    }
    /* Bulk/Interrupt transfer*/
    else if ((desc->bmAttributes & UE_XFERTYPE) == UE_BULK ||
        (desc->bmAttributes & UE_XFERTYPE) == UE_INTERRUPT)
    {
        DBG_X(DUTIL_XFRAG, ("XFRAG: Alloc chain for bulk/intr pipe %p,"
            " toggle %d\n", xfer->pipe, *toggle));

        flags   =  XFRAG_DEFAULT_BULK; 
        flags   |= *toggle ? XFRAG_TOGGLE : 0;
        flags   |= (UE_GET_DIR(desc->bEndpointAddress) == UE_DIR_IN) ?
            XFRAG_IN : XFRAG_OUT;
        
        buffer = (use_dma ? (void *)DMAADDR(&xfer->dmabuf, 0) : (void *)KERNADDR(&xfer->dmabuf, 0)); 
        *head = xfrag_alloc_single_chain(flags, frag_size, xfer->length,
            max_packet, buffer, buf_arr, buf_count, buf_start, &last);
        
        if (!*head)
            return JEINVAL;

        *toggle = (last->flags & XFRAG_TOGGLE ? 1 : 0);
    }
    else if ((desc->bmAttributes & UE_XFERTYPE) == UE_ISOCHRONOUS)
    {
        DBG_X(DUTIL_XFRAG, ("XFRAG: Alloc chain for isoc pipe %p\n",
            xfer->pipe));

        flags   =  XFRAG_DEFAULT_ISOC; 
        flags   |= (UE_GET_DIR(desc->bEndpointAddress) == UE_DIR_IN) ?
            XFRAG_IN : XFRAG_OUT;
        
        buffer = (use_dma ? (void *)DMAADDR(&xfer->dmabuf, 0) : (void *)KERNADDR(&xfer->dmabuf, 0)); 
        
        *head = xfrag_alloc_isoc_chain(flags, xfer, buffer, buf_arr, buf_count,
            buf_start);
        
        if (!*head)
            return JEINVAL;
    }
    else 
        return JEINVAL;
    
    return 0;
}

/* Get next frag to transfer (ignore frags marked with SKIP) */
struct xfrag *xfrag_next(struct xfrag *curr)
{
    struct xfrag *next;
    
    if (!curr)
        return NULL;
    
    next = curr->next;
    
    while (next && (next->flags & XFRAG_SKIP))
        next = next->next;
   
    return next;
}

/* Advance usb/dma fragment chain. Check type of finished and direction
 * to determine what is needed, dma/usb and which frag to schedule next 
 * Parameters:
 * IN           head    - Current head of frag chain
 * IN/OUT       usb     - Current usb frag 
 * IN/OUT       dma     - Current dma frag 
 * Return Value:
 * 0 if chain finished or JEAGAIN if more processing will be needed */
jresult_t xfrag_advance(struct xfrag *head, struct xfrag **usb,
    struct xfrag **dma)
{
    struct xfrag *next_usb;
    struct xfrag *next_dma;

    DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, usb [%p] dma [%p] head [%p]\n",
        *usb, *dma, head));

    /* USB = DMA : [X,O]O- */
    if ((XFRAG_IS_NULL(*usb) || XFRAG_IS_OUT(*usb)) && XFRAG_IS_OUT(*dma))
    {
        next_usb = *dma;
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, usb = dma [%p]\n", next_usb));
    }
    /* USB = NEXT : [II,-X]I */
    else if (XFRAG_IS_IN(XFRAG_CHECK_NEXT(*usb, head)) &&
        ((XFRAG_IS_IN(*usb) && XFRAG_IS_IN(*dma)) ||
        XFRAG_IS_NULL(*dma)))
    {
        next_usb = XFRAG_CHECK_NEXT(*usb, head);
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, usb = next [%p]\n", next_usb));
    }
    else
    {
        next_usb = NULL;
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, usb = NULL\n"));
    }

    /* DMA = USB : I[X,I]- */
    if ((XFRAG_IS_IN(*dma) || XFRAG_IS_NULL(*dma)) && XFRAG_IS_IN(*usb))
    {
        next_dma = *usb;
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, dma = usb [%p]\n", next_dma));
    }
    /* DMA = NEXT : [X-,OO]O */
    else if (XFRAG_IS_OUT(XFRAG_CHECK_NEXT(*dma, head)) &&
        ((XFRAG_IS_OUT(*usb) && XFRAG_IS_OUT(*dma)) ||
        (XFRAG_IS_NULL(*usb))))
    {
        next_dma = XFRAG_CHECK_NEXT(*dma, head);
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, dma = next [%p]\n", next_dma));
    }
    else
    {
        next_dma = NULL;
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, dma = NULL\n"));
    }
    
    XFRAG_SET_STATE(next_usb, XFRAG_NOT_STARTED);
    XFRAG_SET_STATE(next_dma, XFRAG_NOT_STARTED);

    *usb = next_usb;
    *dma = next_dma;

    /* Done : XX- */
    if (XFRAG_IS_NULL(next_usb) && XFRAG_IS_NULL(next_dma))
    {
        DBG_I(DUTIL_XFRAG, ("XFRAG: Advance finished frag chain\n"));
        return 0;
    }

    DBG_I(DUTIL_XFRAG, ("XFRAG: Advance, usb: %p dma: %p\n",
        next_usb, next_dma));

    return JEAGAIN;
}

void xfrag_skip_same(struct xfrag *frag)
{
    struct xfrag *curr = frag->next;

    /* Skip all frags of the same type */
    while (curr && 
        (curr->flags & ~(XFRAG_TOGGLE | XFRAG_STATE_MASK)) == 
        (frag->flags & ~(XFRAG_TOGGLE | XFRAG_STATE_MASK)))
    {
        curr->flags |= XFRAG_SKIP;
        curr = curr->next;
    }
}


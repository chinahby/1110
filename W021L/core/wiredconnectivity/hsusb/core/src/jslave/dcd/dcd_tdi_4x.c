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
#include <jusb_common.h>
#include <jusb_chp9.h>
#include <jusb_core_dcd.h>
#ifdef CONFIG_JOTG
#include <otg_core_device.h>
#endif

#include "dcd_tdi_4x.h"
/* HSU addition, ITC optimization */
#ifndef FEATURE_HS_USB_BASIC
#include "hsu_conf_sel_i.h"
#endif /* FEATURE_HS_USB_BASIC */

/* HSU Addition - to allow spoof_disconnect */
#include "hsu_phy_utils.h"
#include "hsu_os_abstract_bus_ext.h"  /* HSU addition */

#define htoc32(x) (x)
#define ctoh32(x) (x)

/* write / read register */
#define TDI_4X_WRITE_REG(sc, offset, value) \
    htoc32(j_bus_space_write_4((sc)->reg_res, offset, value))
#define TDI_4X_READ_REG(sc, offset) \
    ctoh32(j_bus_space_read_4((sc)->reg_res, offset))
#define TDI_4X_START_TRANSFER(sc, prime) \
    do { \
        /* starting xfer */ \
        TDI_4X_WRITE_REG(sc, TDI_4X_EP_PRIM_REG, \
            TDI_4X_READ_REG(sc, TDI_4X_EP_PRIM_REG) | prime); \
        /* waiting for prime bit */ \
        while (TDI_4X_READ_REG(sc, TDI_4X_EP_PRIM_REG)); \
    } while(0)

static jresult_t dcd_enable(jdevice_t dev, jbool_t is_full_speed);
static void dcd_disable(jdevice_t dev);
static jresult_t wake_the_host(jdevice_t dev);
#ifdef CONFIG_JOTG
static jbool_t   dcd_is_otg(jdevice_t dev);
static jresult_t dcd_invoke_otg_ioctl(jdevice_t dev, drv_ioctl_t ioctl, 
    void *args);
#endif
static jresult_t dcd_vbus_connect(jdevice_t dev);
static jresult_t dcd_vbus_disconnect(jdevice_t dev);
static jresult_t dcd_enable_ep(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t dcd_disable_ep(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t dcd_abort_ep(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t dcd_stall_ep(jdevice_t dev, pipe_desc_t *pipe, jbool_t flag);
static jresult_t dcd_send_io_request(jdevice_t dev, pipe_desc_t *pipe, 
    request_t *core_req);
static jresult_t dcd_alloc_pipe(jdevice_t dev, pipe_desc_t *pipe, 
    juint8_t config_index, juint8_t iface_index, juint8_t alt_index);
static jresult_t dcd_free_pipe(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t dcd_get_ep_status(jdevice_t dev, juint8_t *status, 
    juint8_t ep_num);
static jresult_t dcd_set_address_state(jdevice_t dev, juint16_t address);
static jresult_t dcd_set_config_state(jdevice_t dev, juint8_t config);
static jresult_t dcd_handle_feature(jdevice_t dev, juint16_t feature, 
    juint16_t index, juint8_t set);
static juint8_t dcd_get_max_packet0(jdevice_t dev);
static void dcd_handle_tx_completion(jdevice_t dev, pipe_desc_t *pipe);
#if defined(CONFIG_JSLAVE_TEST_MODE)
static void dcd_set_test_mode(jdevice_t dev, juint16_t test_mode);
#endif

static dcd_ops_t ops = {
     dcd_enable,                /* dcd_enable */ 
     dcd_disable,               /* dcd_disable */
     wake_the_host,             /* wake_the_host */
     NULL,                      /* set_self_power */
#ifdef CONFIG_JOTG
     dcd_is_otg,                /* dcd_is_otg */
     dcd_invoke_otg_ioctl,      /* dcd_invoke_otg_ioctl */
#endif
     dcd_vbus_connect,          /* vbus_connect */
     NULL,                      /* vbus_draw_amount */
     dcd_vbus_disconnect,       /* vbus_disconnect */
     NULL,                      /* dcd_connect */
     NULL,                      /* dcd_disconnect */
     dcd_enable_ep,             /* dcd_enable_ep */
     dcd_disable_ep,            /* dcd_disable_ep */
     dcd_abort_ep,              /* dcd_abort_ep */
     dcd_stall_ep,              /* dcd_stall_ep */
     dcd_send_io_request,       /* dcd_send_io_request */
     dcd_alloc_pipe,            /* dcd_alloc_pipe */
     dcd_free_pipe,             /* dcd_free_pipe */
     dcd_get_ep_status,         /* dcd_get_ep_status */
     dcd_set_address_state,     /* dcd_set_address_state */
     dcd_handle_feature,        /* dcd_handle_feature */
     dcd_set_config_state,      /* dcd_set_config_state */
     dcd_get_max_packet0,       /* dcd_get_max_packet0 */       
     dcd_handle_tx_completion,  /* dcd_handle_tx_completion */       
#if defined(CONFIG_JSLAVE_TEST_MODE)
     dcd_set_test_mode          /* dcd_set_test_mode */
#else
     NULL
#endif
};

/* isr handlers */
static jint_t dcd_intr(void *arg);
static void dcd_soft_intr(void *arg);

/* xfers handlers */
static void handle_setup_intr(tdi4x_soft_dc_t *sc);
static void abort_ep0(tdi4x_soft_dc_t *sc, jbool_t is_in);
static void dcd_handle_comp(tdi4x_soft_dc_t *sc, ep_desc_t *ep);
    
static pipe_desc_t* get_pipe_desc(tdi4x_soft_dc_t *sc, juint8_t ep);
static ep_desc_t* get_ep_desc(tdi4x_soft_dc_t *sc, juint8_t ep);
static jresult_t init_endpoints(tdi4x_soft_dc_t *sc);
static void handle_disconnect(tdi4x_soft_dc_t *sc);
static void handle_reset(tdi4x_soft_dc_t *sc);
static void handle_suspend(tdi4x_soft_dc_t *sc);
static void handle_resume(tdi4x_soft_dc_t *sc);
static void reset_hw(tdi4x_soft_dc_t *sc);
static void cancel_queue(tdi4x_soft_dc_t *sc, ep_desc_t *ep);
static void cancel_active_request(tdi4x_soft_dc_t *sc, ep_desc_t *ep);
static jresult_t start_next_transfer(tdi4x_soft_dc_t *sc, ep_desc_t *ep);

static void reset_ep(tdi4x_soft_dc_t *sc, ep_desc_t *ep);

/**
 * Function name:  build_single_td
 * Description: Allocate a single transfer descriptor, and initialize it 
 * Parameters: 
 *     @byte_count: (IN) size of data in the transfer descriptor
 *     @ioc: (IN) interrupt on completion
 *     @phys_buf: (IN) address of data buffer for the specific transfer
 *                     descriptor
 *
 * Return value: The allocated transfer descriptor
 * Scope: local
 **/
static struct tdi4x_soft_td *build_single_td(juint32_t byte_count, juint32_t ioc,
    void *phys_buf)
{
    juint32_t left = byte_count;
    juint32_t curr_ptr = (juint32_t)phys_buf;
    jint_t i;
    jresult_t rc ;
    tdi4x_td_h td;
    struct tdi4x_soft_td *td_p = (struct tdi4x_soft_td *)
        jmalloc(sizeof(struct tdi4x_soft_td), M_ZERO);

    if (!td_p)
    {
        j_panic("build_single_td: NOT ENOUGH MEMORY !\n");
        return NULL;
    }

    rc = jdma_alloc(TDI_4X_TRANSFER_DESCRIPTOR_SIZE, 
        TDI_4X_TRANSFER_DESCRIPTOR_ALIGN, (void **)(&td_p->virt),
        &td_p->phys, M_ZERO | M_PAGE_ALIGN, &td_p->cookie);
    if (rc)
    {
        j_panic("panic - dma_alloc failed : rc = %d\n",rc);
        jfree(td_p);
        return NULL;
    }
    td = TD(td_p);

    /* 1. Initialize first 7DWords to 0 */
    j_memset((void *)td, TDI_4X_TRANSFER_DESCRIPTOR_SIZE, 0);
   
    /* 2. Set the terminate bit */
    td->next_td_ptr = htoc32(BFSET(TDI_4X_TD_TERMINATE, 1));

    /* 3. Fill in total bytes with transfer size */
    /* 4. Set the interrupt on complete */
    /* 5. Initialize the status field with active set "1"
     * and the rest set to "0" */
    td->td_header = htoc32(
        BFSET(TDI_4X_TD_TOTAL_BYTES, byte_count) |
        BFSET(TDI_4X_TD_IOC, ioc) |
        BFSET(TDI_4X_TD_STATUS, TDI_4X_TD_ACTIVE));

    /* Init the pages */
    for (i = 0; i < 5 && left; i++)
    {
        td->buf_ptr[i] = htoc32(curr_ptr);

        /* Get the ammount of data in previous buffer */
        left -= MIN(left, (0x1000 - (curr_ptr & 0x0fff)));

        /* Advance page */
        curr_ptr = (curr_ptr & ~(0x0fff)) + 0x1000;

        if (left)
        {
#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_I(DSLAVE_DCD, ("-- %d Start [%x:%x] total %d left %d (%d) new "
                "head [%x:%x]\n",
                i, 
                ((juint32_t)phys_buf) & ~0xfff,
                ((juint32_t)phys_buf) & 0xfff,
                byte_count, left, byte_count - left, 
                curr_ptr & ~0x0fff,
                curr_ptr & 0x0fff));
#endif /* CONFIG_DATA_PATH_DEBUG */
        }
    }

    /* Init the remaining pages to be one greater than the previous */
    for (; i < 5; i++)
    {
        td->buf_ptr[i] = htoc32(curr_ptr);
        curr_ptr = (curr_ptr & ~(0x0fff)) + 0x1000;
    }

    return td_p;
}
/* inserting TD to head point queue (same queue for in and out)*/
#define TDI_4X_INSERT_TD(size, ioc, addr) \
{ \
    curr_td = build_single_td((size), (ioc), (addr)); \
    if (!curr_td) \
    { \
        DBG_E(DSLAVE_DCD, ("Failed to allocate TD\n")); \
        goto Error; \
    } \
    curr_td->td_size = size; \
    curr_td->num_of_td = 0; \
    TAILQ_INSERT_TAIL(&ep->td_head, curr_td, next); \
    /* Update the pointer of the last TD to point here */ \
    if (prev_td) \
    { \
        TD(prev_td)->next_td_ptr = htoc32(TD_PHYS(curr_td) & \
            ~BFMASK(TDI_4X_TD_TERMINATE)); \
    } \
    prev_td = curr_td; \
}

struct tdi4x_soft_td* get_active_td(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    struct tdi4x_soft_td *td_ptr;
    juint32_t status;
    tdi4x_td_h td;

    for (td_ptr = TAILQ_FIRST(&ep->td_head); td_ptr; 
        td_ptr = TAILQ_NEXT(td_ptr, next))
    {
        td = TD(td_ptr);
        status = BFGET(TDI_4X_TD_STATUS, td->td_header);

        if (status & TDI_4X_TD_ACTIVE)
            break;
    }

    return td_ptr;
}

#define FREE_TD(ep, td_ptr) \
    TAILQ_REMOVE(&ep->td_head, td_ptr, next); \
    jdma_free(td_ptr->cookie); \
    jfree(td_ptr);
 /**
 * Function name:  build_multiple_td_list
 * Description: Build a chain of transfer descriptors for the specific
 *              transfer, and prime the relevant endpoint register
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address
 *     @core_req: (IN) CORE request
 *
 * Return value: The bitmask of the primed endpoint
 * Scope: local
 **/

static jresult_t build_multiple_td_list(tdi4x_soft_dc_t *sc, ep_desc_t *ep, 
    request_t *core_req)
{
    struct tdi4x_soft_td *head_td = NULL;
    struct tdi4x_soft_td *prev_td = NULL;
    struct tdi4x_soft_td *curr_td = NULL;
    juint32_t size, byte_count;
    request_t *curr_request;
    void *addr;
    jbool_t need_zero;
    LIST_HEAD(req_list_head, request_t) req_list;
    juint16_t max_packet = 
        (ep->pipe ? ep->pipe->max_pkt_size : dcd_get_max_packet0(sc->dev));

    if (TAILQ_FIRST(&ep->td_head))
        prev_td = TAILQ_LAST(&ep->td_head, dcd_tdi_4x_td_head);

    LIST_FIRST(&req_list) = core_req;

    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        byte_count = curr_request->transfer_size;
        addr = curr_request->buffer.dma_addr;
        need_zero = 0;

        if (curr_request->zero && byte_count && !(byte_count % max_packet))
            need_zero = 1;

        do
        {
            /* FIXME: The current API for multiple requests does not support
             * isochronous pipes. For isochronous pipes, TD size should be equal
             * to the data sent in a single (micro)frame */
            size = MIN(byte_count, TDI_4X_TD_MAX_BYTES);
            byte_count -= size;

            if (core_req->direction == DIRECTION_IN)
            {
                TDI_4X_INSERT_TD(size, 0, addr);
                ep->handle_comp_from_intr = 0;
            }
            else
            {
                TDI_4X_INSERT_TD(size, LIST_NEXT(curr_request,
                    next_request_in_list) ? 0 : 1, addr);
            }

            if (!head_td)
                head_td = curr_td;

            head_td->num_of_td++;

            addr = &(((juint8_t *)addr)[size]);
        }while (byte_count);

        if (need_zero)
        {
            TDI_4X_INSERT_TD(0, 0, addr);
            head_td->num_of_td++;
        }

        head_td = NULL;
    }

     return 0;

Error:
    for (curr_td = TAILQ_FIRST(&ep->td_head) ; curr_td ;
        curr_td = TAILQ_FIRST(&ep->td_head)) 
    {
        /* Unlink and free the TD struct */
        FREE_TD(ep, curr_td)
    }
    return JENOMEM;
}

/**
 * Function name:  build_td_list
 * Description: Build a chain of transfer descriptors for the specific
 *              transfer, and prime the relevant endpoint register
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address
 *     @byte_count: (IN) transfer size
 *     @dir: (IN) transfer direction
 *     @addr: (IN) address of transfer data buffer
 *
 * Return value: The bitmask of the primed endpoint
 * Scope: local
 **/
static juint32_t build_td_list(tdi4x_soft_dc_t *sc, ep_desc_t *ep, 
    juint32_t byte_count, jint_t dir, void *addr, jbool_t need_zero)
{
    pipe_desc_t *pipe = ep->pipe;
    struct tdi4x_soft_td *start_td = NULL;
    struct tdi4x_soft_td *prev_td = NULL;
    struct tdi4x_soft_td *curr_td = NULL;
    tdi4x_qh_h qh;
    juint32_t prime, size;

    /* For OUT direction, we build a single TD. In case anymore TD's will be
     * needed, they will be added upon the completion of the first TD */
    if (pipe && PIPE_TYPE_MASK(pipe->type) != TYPE_ISOC &&
        dir == DIRECTION_OUT)
    {
        size = MIN(byte_count, TDI_4X_TD_MAX_BYTES);
        TDI_4X_INSERT_TD(size, 1, addr);
        start_td = curr_td;
        start_td->num_of_td++;
    }
    else
    {
        juint32_t max_td_size = TDI_4X_TD_MAX_BYTES;

        /* For isochronous pipes, TD size should be equal to the data sent in a
         * single (micro)frame */
        if (pipe && PIPE_TYPE_MASK(pipe->type) == TYPE_ISOC)
        {
            max_td_size = pipe->max_pkt_size * 
                ((sc->dev_speed == SPEED_FULL) ? 1 : 
                (pipe->transaction_per_microframe + 1));
        }
        
        /* For IN direction and Isochronous pipes, we build all the needed 
         * TD's */
        do 
        {
            size = MIN(byte_count, max_td_size);
            byte_count -= size;

            TDI_4X_INSERT_TD(size, (!byte_count && !need_zero) ? 1 : 0, addr);

            /* Update the start TD */
            if (!start_td)
                start_td = curr_td;

            start_td->num_of_td++;
            addr = &(((juint8_t *)addr)[size]);
        }
        while (byte_count);

        if (need_zero)
        {
            TDI_4X_INSERT_TD(0, 1, addr);
            start_td->num_of_td++;
        }
    }

    /* Get the QH to attach to */
    qh = ep->qhead;

    /* Clear Active & Halt bit in QH */
    qh->cur_td.td_header &= htoc32(~BFMASK(TDI_4X_TD_STATUS));

    /* Inserting TD list to QH */
    qh->cur_td.next_td_ptr = htoc32(TD_PHYS(start_td));

    /* Setting prime bit */
    prime = ((dir == DIRECTION_IN) ? 
        TDI_4X_EP_PRIM_TX((ep->hw_addr)) : 
        TDI_4X_EP_PRIM_RX((ep->hw_addr)));

    return prime;

Error:
    for (curr_td = TAILQ_FIRST(&ep->td_head) ; curr_td ;
        curr_td = TAILQ_FIRST(&ep->td_head)) 
    {
        /* Unlink and free the TD struct */
        FREE_TD(ep, curr_td);
    }
    return 0;
}

/**
 * Function name:  start_multiple_transfer
 * Description: Start the transfer request on the hardware, including
 *              allocate of TD list for the transfer
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t start_multiple_transfer(tdi4x_soft_dc_t *sc, ep_desc_t *ep, 
    request_t *core_req)
{
    juint32_t prime = 0;
    jresult_t rc;
    jbool_t queue_empty;
    tdi4x_qh_h qh;
    request_t *curr_request;
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = core_req;

#ifdef JDEBUG
    LIST_FOREACH(curr_request, &req_list, next_request_in_list)
    {
        KASSERT(!(curr_request->direction == DIRECTION_OUT &&
            curr_request->transfer_size > 16384), ("start_multiple_transfer does"
                " not support OUT requests larger than 16384\n"));
    }
#endif
    
    if ((core_req->direction == DIRECTION_IN && !ep->is_in) ||
       (core_req->direction == DIRECTION_OUT && ep->is_in))
    {
        j_panic("DCD: Transfer and Endpoint directions don't match, "
            "soft_addr %x!\n", ep->soft_addr); 
    }

    if (ep->is_in)
    {
        LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        {
            if (curr_request->transfer_size)
            {
                j_cache_flush(curr_request->buffer.dma_addr,
                    curr_request->transfer_size);
            }
        }
    }

    queue_empty = TAILQ_FIRST(&ep->td_head) ? 0 : 1;

    /* setup the transaction */
    rc = build_multiple_td_list(sc, ep, core_req);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("no TDs created for transfer\n"));
        goto Exit;
    }

    /* Remember who to call when we are done */
    if (ep->cur_req)
    {
        LIST_FIRST(&req_list) = ep->cur_req;

        LIST_FOREACH(curr_request, &req_list, next_request_in_list)
        {
            if (!LIST_NEXT(curr_request, next_request_in_list))
            {
                LIST_NEXT(curr_request, next_request_in_list) = core_req;
                break;
            }
        }
    }
    else
    {
        ep->cur_req = core_req;
    }

    /* Setting prime bit */
    prime = ((core_req->direction == DIRECTION_IN) ? 
        TDI_4X_EP_PRIM_TX((ep->hw_addr)) : 
        TDI_4X_EP_PRIM_RX((ep->hw_addr)));

#ifdef CONFIG_IN_HW_QUEUE
    if (ep->is_in && !queue_empty)
    {
        juint32_t status = 0;
        
        if (TDI_4X_READ_REG(sc, TDI_4X_EP_PRIM_REG) & prime)
        {
#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_V(DSLAVE_DCD, ("already primed\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
            prime = 0;
            goto Exit;
        }

        /* ensuring the proper dTD add */
        do
        {
            TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
                TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) |
                TDI_4X_USB_CMD_ATDTW);

            status = TDI_4X_READ_REG(sc, TDI_4X_EP_STAT_REG);
        }
        while (!(TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) &
            TDI_4X_USB_CMD_ATDTW));

        TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
            TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & ~TDI_4X_USB_CMD_ATDTW);

        if (status & prime)
        {
            prime = 0;
            goto Exit;
        }
    }
#endif

   /* Get the QH to attach to */
    qh = ep->qhead;

    /* Clear Active & Halt bit in QH */
    qh->cur_td.td_header &= htoc32(~BFMASK(TDI_4X_TD_STATUS));
                
    /* Inserting TD list to QH */
    if (queue_empty)
    {
        qh->cur_td.next_td_ptr = htoc32(TD_PHYS(TAILQ_FIRST(&ep->td_head)));
    }
#ifdef CONFIG_IN_HW_QUEUE
    else
    {
        struct tdi4x_soft_td *active_td;

        active_td = get_active_td(sc, ep);
        if (!active_td)
        {
#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_I(DSLAVE_DCD, ("no active td's found\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
            prime = 0;
            goto Exit;
        }
        else
        {
            qh->cur_td.next_td_ptr = htoc32(TD_PHYS(active_td));
        }
    }
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_DCD, ("DCD: Starting ep %d [%d]"
        "prime %08x qh %08x req %p\n", ep->soft_addr, ep->hw_addr,
        prime, ep->qhead, core_req));
#endif /* CONFIG_DATA_PATH_DEBUG */

Exit:
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("start_multiple_transfer: Error - rc = %d\n", rc));
        return rc;
    }

    if (prime)
            TDI_4X_START_TRANSFER(sc, prime);
#ifdef CONFIG_DATA_PATH_DEBUG
    else
        DBG_I(DSLAVE_DCD, ("Endpoint is already primed\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */

    return 0;
}

/**
 * Function name:  start_transfer
 * Description: Start the specific transfer request on the hardware, including
 *              allocate of TD list for the transfer
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address
 *     @core_req: (IN) CORE request
 *     @dir: (IN) transfer direction
 *     @size: (IN) transfer size
 *     @buf: (IN) transfer data buffer
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t start_transfer(tdi4x_soft_dc_t *sc, ep_desc_t *ep, 
    request_t *core_req, jint_t dir, juint32_t size, void *buf)
{
    juint32_t prime;
    jbool_t need_zero = 0;
    juint16_t max_packet = 
        (ep->pipe ? ep->pipe->max_pkt_size : dcd_get_max_packet0(sc->dev));

    if ((dir == DIRECTION_IN && !ep->is_in) ||
       (dir == DIRECTION_OUT && ep->is_in))
    {
        j_panic("DCD: Transfer and Endpoint directions don't match dir %x,"
            " soft_addr %x!\n", dir, ep->soft_addr);
    }

    if (core_req->zero && size && !(size % max_packet))
        need_zero = 1;

    /* Setup the transaction */
    prime = build_td_list(sc, ep, size, dir, buf, need_zero);
    if (!prime)
    {
        DBG_E(DSLAVE_DCD, ("DCD: No TDs created for transfer\n"));
        return JEINVAL;
    }
    
    /* Remember who to call when we are done */
    ep->cur_req = core_req;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_DCD, ("DCD: Starting ep %d [%d] transfer %s size %d "
        "prime %08x qh %08x req %p\n", ep->soft_addr, ep->hw_addr,
        dir == DIRECTION_IN ? "IN" : "OUT", 
        size, prime, ep->qhead, core_req));
#endif /* CONFIG_DATA_PATH_DEBUG */

    if (ep->is_in && size)
        j_cache_flush(buf, size);

    TDI_4X_START_TRANSFER(sc, prime);

    return 0;
}

/**
 * Function name:  start_next_transfer
 * Description: Queue a new reseust on the hardware in case there are pending
 *              requests on the pipe queue
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) Endpoint context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t start_next_transfer(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    request_t *next;

    /* No more requests */
    if (STAILQ_EMPTY(&ep->pipe->req_queue))
        return 0;

    /* Start the next request on queue */
    next = STAILQ_FIRST(&ep->pipe->req_queue);

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_X(DSLAVE_DCD, ("start_next_transfer: Sending next request on ep "
        "%d\n", ep->soft_addr));
#endif /* CONFIG_DATA_PATH_DEBUG */
    return dcd_send_io_request(sc->dev, ep->pipe, next);
}

/**
 * Function name:  wake_the_host
 * Description: Called by CORE to Handle a remote wakeup request
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t wake_the_host(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    jbus_h tdi4x_bus = j_device_get_bus(dev);
    juint32_t portsc;
    
    if (jnotify_os(NOTIFY_DEVICE_REMOTE_WAKEUP, (void *)tdi4x_bus))
    {
        DBG_W(DSLAVE_DCD, ("wake_the_host: remote wakeup prevented by the "
            "porting layer\n"));
        return JEPERM;
    }

    /* Write '1' to Force Port Resume bit in PORTSC to start a resume signal to
     * the host */
    portsc = TDI_4X_READ_REG(sc, TDI_4X_PORTSC_REG);
    portsc |= TDI_4X_PORTSC_FPR;
    TDI_4X_WRITE_REG(sc, TDI_4X_PORTSC_REG, portsc);

    return 0;
}

/**
 * Function name:  dcd_vbus_connect
 * Description: Enable the pull up resistor on the D+ line, causing to vBus to
 *              rise
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_vbus_connect(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);

    DBG_I(DSLAVE_DCD, ("dcd_vbus_connect: enable vbus\n"));

    /* Check if DCD is disabled - we shouldn't start vbus unless we are 
       enabled */
    if (!sc->interrupt_h)
    {
        DBG_W(DSLAVE_DCD, ("dcd_vbus_connect: DCD is disabled, can't start "
            "vbus\n"));
        return JEPERM;
    }

#ifdef CONFIG_JTRANSCEIVER
    trcv_device_enable(sc->trcv, 1);
#endif

    /* Writing 1 to Run/Stop bit will cause to a detach from the host */
    TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
        TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) | TDI_4X_USB_CMD_RS);

    return 0;
}

/**
 * Function name:  dcd_vbus_disconnect
 * Description: Disable the pull up resistor on the D+ line, causing to vBus to
 *              fall
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_vbus_disconnect(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);

    DBG_I(DSLAVE_DCD, ("dcd_vbus_disconnect: disable vbus\n"));

    /* Check if DCD is disabled - we shouldn't stop vbus unless we are 
       enabled */
    if (!sc->interrupt_h)
    {
        DBG_W(DSLAVE_DCD, ("dcd_vbus_disconnect: DCD is disabled, can't stop "
            "vbus\n"));
        return JEPERM;
    }

    /* HSU Addition: spoof disconnect */
    if (!(hsu_phy_utils_spoof_disconnect()))
    {
      DBG_E(DSLAVE_DCD, ("dcd_vbus_disconnect: Spoof disconnect failed!\n"));    
      return EUNKNOWN;
    }

#ifdef CONFIG_JTRANSCEIVER
    trcv_device_enable(sc->trcv, 0);
#endif

    return 0;
}

static jresult_t dcd_free_pipe(jdevice_t dev, pipe_desc_t *pipe)
{
    ep_desc_t *ep = (ep_desc_t*)pipe->dcd_handle;

    if (ep->inuse)
        j_panic("dcd_free_pipe: Trying to free a used ep\n");

    /* Mark the endpoint as free */
    ep->claimed_config = 0;
    ep->claimed_iface = 0;
    ep->claimed_alt = 0;

    return 0;
}
/**
 * Function name:  dcd_alloc_pipe
 * Description: Called by CORE to allocate DCD endpoint for the specific pipe
 *              properties
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @iface_index: (IN) interface number for the requested pipe
 *     @alt_index: (IN) alternate setting for the requested pipe
 *     @config_index: (IN) config value for the requested pipe
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_alloc_pipe(jdevice_t dev, pipe_desc_t *pipe, 
    juint8_t config_index, juint8_t iface_index, juint8_t alt_index)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    jint_t i;

    DBG_V(DSLAVE_DCD, ("DCD: Finding a matching for %p iface = %d alt = %d\n", 
        pipe, iface_index + 1, alt_index + 1));

    for (i = 2; i < sc->max_available_eps; i++)
    {
        DBG_X(DSLAVE_DCD, ("DCD: allocating pipe (ep = %d)\n",
            sc->ep_desc_table[i].soft_addr));

        KASSERT(config_index >= sc->ep_desc_table[i].claimed_config,
            ("DCD: pipes allocated out of order (claimed config %u config "
            "index %u\n", sc->ep_desc_table[i].claimed_config, config_index));

        if (sc->ep_desc_table[i].claimed_config == config_index)
        {
            /* Check that pipe not claimed by other FD */
            if (sc->ep_desc_table[i].claimed_iface != (iface_index  + 1) &&
                sc->ep_desc_table[i].claimed_iface)
            {
                continue;
            }

            DBG_X(DSLAVE_DCD, ("DCD: unclaimed pipe was found\n"));

            KASSERT(alt_index + 1 >= sc->ep_desc_table[i].claimed_alt,
                ("DCD: pipes allocated out of order (claimed alt %u alt "
                "index %u\n", sc->ep_desc_table[i].claimed_alt - 1, alt_index));

            /* Cant have same alt-interface claim same pipes */
            if (sc->ep_desc_table[i].claimed_alt == (alt_index + 1))
                continue;
        }

        DBG_X(DSLAVE_DCD, ("DCD: unclaimed pipe by alt iface was found\n"));

        /* Check that the direction we need is supported */
        if ((pipe->direction == DIRECTION_OUT && sc->ep_desc_table[i].is_in) ||
            (pipe->direction == DIRECTION_IN && !sc->ep_desc_table[i].is_in)) 
            continue;

        DBG_X(DSLAVE_DCD,("DCD: pipe supports the direction\n"));

        /* Check that the packet size we need is supported */
        if (pipe->max_pkt_size_full > sc->ep_desc_table[i].packet_size)
        {
            DBG_X(DSLAVE_DCD, ("DCD: pipe does NOT support packet size %d\n",
                pipe->max_pkt_size_full));
            continue;
        }

        DBG_X(DSLAVE_DCD, ("DCD: pipe supports packet size %d\n",
            pipe->max_pkt_size_full));

        /* Check that the pipe type we need is supported */
        if (!SUPPORTED_PIPE_TYPE(sc->ep_desc_table[i].type_map, pipe->type))
        {
            DBG_X(DSLAVE_DCD, ("DCD: pipe does NOT support type %d\n",
                pipe->type));
            continue;
        }

        DBG_X(DSLAVE_DCD, ("DCD: pipe supports type %d\n", pipe->type));

        /* Found it */
        break;
    }

    /* Check why we exited the loop */
    if (i == sc->max_available_eps)
    {
        DBG_E(DSLAVE_DCD,
            ("DCD: Cannot match requested pipe configuration with "
            "available EPs\n"));
        return JEINVAL;
    }

    sc->ep_desc_table[i].claimed_alt = alt_index + 1;
    sc->ep_desc_table[i].claimed_iface = iface_index + 1;
    sc->ep_desc_table[i].claimed_config = config_index;

    /* setting address */
    pipe->address = sc->ep_desc_table[i].hw_addr;

    /* setting direction */
    if (pipe->direction == DIRECTION_IN)
        pipe->address |= USB_DIR_IN;

    /* storing references pipe <--> ep */ 
    pipe->dcd_handle = (void *)(&sc->ep_desc_table[i]);

    DBG_V(DSLAVE_DCD, ("DCD: EP %d matched to address 0x%x\n", 
        sc->ep_desc_table[i].soft_addr, pipe->address));

    return 0;
}

/**
 * Function name:  dcd_get_ep_status
 * Description: Return the endpoint status (Stalled or not)
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @status: (OUT) endpoint status
 *     @ep_num: (IN) endpoint USB address
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_get_ep_status(jdevice_t dev, juint8_t *status, 
    juint8_t ep_num)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    juint8_t soft_addr = (ep_num & 0x0F) * 2 + (ep_num >> 7);
    ep_desc_t *ep = get_ep_desc(sc, soft_addr);
    
    /* if ep is stalled returns 1 else returns 0 */
    *status = (juint8_t)ep->ep_stalled;

    return 0;
}

/**
 * Function name:  dcd_enable_ep
 * Description: Called by CORE to enable a specific endpoint
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_enable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    ep_desc_t *ep = (ep_desc_t*)pipe->dcd_handle;
    juint32_t pipe_type = TDI_4X_EP_CTRL_RXR | TDI_4X_EP_CTRL_TXR;
    juint32_t mult = 0;
    tdi4x_qh_h qh;
    juint32_t conf;
    juint_t dir = pipe->direction;
    jresult_t rc;

    DBG_I(DSLAVE_DCD, ("DCD: Enabling pipe 0x%x (ep %d), pkt size %d dir %s\n",
        pipe->address, ep->soft_addr, pipe->max_pkt_size, 
        dir == DIRECTION_IN ? "in" : "out")); 

    ep->pipe = pipe;
    qh = ep->qhead;

    /* configurating max packet size */
    qh->qh_header = htoc32(BFRESET(TDI_4X_QH_MAX_PKT_LEN,
        ctoh32(qh->qh_header), pipe->max_pkt_size));

    if (ep->inuse)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Error, enabling an ep that is inuse ! %d\n",
            pipe->address));
        return JEBUSY;
    }

    ep->sc = sc;
    ep->abort_timeout_task = 0;
    rc = jtask_init(&ep->timeout_task, TASK_CONTROLLER);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Unable to init the timeout task rc = %d\n",
            rc));
        return rc;
    }

    /* setting direction */
    pipe_type |= (dir == DIRECTION_IN) ? 
        TDI_4X_EP_CTRL_TXE : TDI_4X_EP_CTRL_RXE;

    /* setting pipe type */
    switch (PIPE_TYPE_MASK(pipe->type))
    {
    case TYPE_CTRL:
        pipe_type |= (dir == DIRECTION_OUT) ? 
            TDI_4X_EP_CTRL_RXT_CTRL : TDI_4X_EP_CTRL_TXT_CTRL;
        break;
    case TYPE_BULK:
        pipe_type |=  (dir == DIRECTION_OUT) ?
            TDI_4X_EP_CTRL_RXT_BULK : TDI_4X_EP_CTRL_TXT_BULK;
        break;
    case TYPE_INTR:
        pipe_type |= (dir == DIRECTION_OUT) ?
            TDI_4X_EP_CTRL_RXT_INTR : TDI_4X_EP_CTRL_TXT_INTR;
        break;
    case TYPE_ISOC:
        if (sc->dev_speed == SPEED_FULL)
            mult = 1;
        else
            mult = pipe->transaction_per_microframe + 1;

        pipe_type |= (dir == DIRECTION_OUT) ?
            TDI_4X_EP_CTRL_RXT_ISOC : TDI_4X_EP_CTRL_TXT_ISOC;
        break;
    }

    DBG_V(DSLAVE_DCD, ("DCD: Set pipe %d [%d] %s type 0x%x\n", 
        ep->soft_addr, ep->hw_addr, dir == DIRECTION_IN ? "In" : "Out",
        pipe_type));

    conf = TDI_4X_READ_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr));

    if (dir == DIRECTION_IN)
    {
        conf &= 0x0000ffff;
        conf |= pipe_type & 0xffff0000; 
    }
    else
    {
        conf &= 0xffff0000;
        conf |= pipe_type & 0x0000ffff;
    }

    qh->qh_header = htoc32(BFRESET(TDI_4X_QH_MULT, ctoh32(qh->qh_header), 
        mult));
    
    /* configurating EP */ 
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr), conf);

    pipe->status = PIPE_ENABLED; 
    ep->inuse = 1;
    
    return 0;
}

/**
 * Function name:  dcd_disable_ep
 * Description: Call by CORE to disable a specific endpoint
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_disable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    ep_desc_t *ep = (ep_desc_t*)pipe->dcd_handle;

    DBG_I(DSLAVE_DCD, ("DCD: Disable EP %d \n", ep->soft_addr));

    if (!ep->inuse)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Error, disabling a non used ep ! %d\n",
            pipe->address));
        return JEBUSY;
    } 

    if (pipe->status != PIPE_ENABLED &&
        pipe->status != PIPE_STALLLED) 
    {
        DBG_E(DSLAVE_DCD, ("Attempt to disable a non-enabled pipe\n"));
        return 0;
    }

    /* disabling ep */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr), 
        TDI_4X_READ_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr)) & 
        (ep->is_in ? TDI_4X_EP_CTRL_DISABLE_TX : TDI_4X_EP_CTRL_DISABLE_RX));

    /* Release ep to other uses */
    pipe->status = PIPE_DISABLED;
    ep->inuse = 0;

    if (ep->timeout_task)
    {
        jtask_uninit(ep->timeout_task);
        ep->timeout_task = NULL;
    }

    DBG_I(DSLAVE_DCD, ("Pipe %d disabled\n",ep->soft_addr));

    return 0;
}

/**
 * Function name:  dcd_abort_ep
 * Description: Called by CORE to abort all pending transfer on the specific
 *              pipe
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_abort_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    ep_desc_t *ep = NULL;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_DCD, ("DCD: abort all transfers on ep %d\n",
        (pipe ? pipe->address : 0)));
#endif /* CONFIG_DATA_PATH_DEBUG */

    if (pipe && !STAILQ_EMPTY(&pipe->req_queue))
        pipe->status = PIPE_ABORTING;

    if (pipe)
        ep = get_ep_desc(sc, ((ep_desc_t*)(pipe->dcd_handle))->soft_addr);
    else if (sc->ep_desc_table[0].cur_req)
        ep = &sc->ep_desc_table[0];
    else if (sc->ep_desc_table[1].cur_req)
        ep = &sc->ep_desc_table[1];

    if (ep && ep->cur_req)
        cancel_active_request(sc, ep);
    if (ep)
        cancel_queue(sc, ep);

    return 0;
}

/**
 * Function name:  reset_ep
 * Description: Hardware reset of the specific endpoint, including stop active
 *              transfer and clear STALL condition
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) Endpoint context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static void reset_ep(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    juint32_t bits;
    
    DBG_I(DSLAVE_DCD, ("--------- Pipe %d aborted \n",ep->soft_addr));

    bits = ep->is_in ? 
        TDI_4X_FLUSH_TX_EP(ep->hw_addr) :
        TDI_4X_FLUSH_RX_EP(ep->hw_addr);
    
    do {
        /* Start flushing */
        TDI_4X_WRITE_REG(sc, TDI_4X_EP_FLUSH_REG, bits);

        /* Waiting for flush to finish */
        while (TDI_4X_READ_REG(sc, TDI_4X_EP_FLUSH_REG));

        /* Check if the flush failed - if yes, it means that there is a
           transfer in progress (Page 185 in TDI spec */
    } while (TDI_4X_READ_REG(sc, TDI_4X_EP_STAT_REG) & bits);

    ep->ep_stalled = 0;
    ep->last_buf = 0;
}

/**
 * Function name:  dcd_stall_ep
 * Description: Set/Clear stall on the specific pipe
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @flag: 1 for stall the endpoint, 0 for clear stalled endpoint
 *
 * Return value: 
 * Scope: local
 **/
static jresult_t dcd_stall_ep(jdevice_t dev, pipe_desc_t *pipe, jbool_t flag)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    juint8_t ep_num;
    ep_desc_t *ep;
    juint32_t ep_stat;
    juint32_t ep_flag;

    ep_num = pipe ? ((ep_desc_t*)pipe->dcd_handle)->soft_addr : 0;
    ep = get_ep_desc(sc, ep_num);

    DBG_V(DSLAVE_DCD, ("DCD: dcd_stall_ep %s %d\n", 
        (flag ? "STALL" : "CLEAR"), ep_num));

    /* Figure out what flag to set/clear */
    if (!ep_num)
    {
        ep_flag = TDI_4X_EP_CTRL_TXS | TDI_4X_EP_CTRL_RXS;
    }
    else
    {
        ep_flag = (pipe->direction == DIRECTION_IN) ?
            TDI_4X_EP_CTRL_TXS : TDI_4X_EP_CTRL_RXS;
    }

    /* Get the current status */
    ep_stat = TDI_4X_READ_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr));

    DBG_I(DSLAVE_DCD, ("DCD: Handle Stall 1: stat %x, flag %x\n",
        ep_stat, ep_flag));

    /* Update the flag based on request */
    ep_stat = (flag) ? (ep_stat | ep_flag) : (ep_stat & ~ep_flag);
    if (!flag)
    {
        /* When clear stall feature, reset data toggle bit */
        if (!ep_num)
            ep_stat |= (TDI_4X_EP_CTRL_TXR | TDI_4X_EP_CTRL_RXR);
        else
        {
            ep_stat |= ((pipe->direction == DIRECTION_IN) ?
                TDI_4X_EP_CTRL_TXR : TDI_4X_EP_CTRL_RXR);
        }
    }

    DBG_I(DSLAVE_DCD, ("DCD: Handle Stall 2: stat %x, flag %x\n",
        ep_stat, ep_flag));
    /* Notify controller of change */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_CTRL_REG(ep->hw_addr), ep_stat);

    ep->ep_stalled = flag;
        
    if (pipe)
        pipe->status = flag ? PIPE_STALLLED : PIPE_ENABLED;

    return 0;
}

static void dcd_handle_timeout(void *arg)
{
    ep_desc_t *ep = (ep_desc_t*)arg;
    tdi4x_td_t *cur_td;
    struct tdi4x_soft_td *td_ptr;
    request_t *cur_req;
    jbool_t call_completion = 0;
    juint32_t status, num_of_td = 0;

    for (td_ptr = TAILQ_FIRST(&ep->td_head), cur_req = ep->cur_req; 
        td_ptr && cur_req; td_ptr = TAILQ_NEXT(td_ptr, next))
    {
        cur_td = TD(td_ptr);
        status = BFGET(TDI_4X_TD_STATUS, cur_td->td_header);
        call_completion = 1;

        if (!num_of_td)
            num_of_td = td_ptr->num_of_td;

        num_of_td--;

        if (status & TDI_4X_TD_ACTIVE)
        {
            if (cur_req == ep->cur_req)
            {
                call_completion = 0;
                break;
            }
            else
            {
                cur_req->status = REQUEST_PENDING;
            }
        }

        if (!ep->cur_req->single_request && !num_of_td)
            cur_req = LIST_NEXT(cur_req, next_request_in_list);
    }

    if (ep->cur_req && ep->cur_req->timeout)
    {
        jtask_schedule(ep->timeout_task, ep->cur_req->timeout,
            dcd_handle_timeout, ep);
    }

    if (call_completion)
    {
        ep->abort_timeout_task = 0;
        dcd_handle_comp(ep->sc, ep);
    }
}

static void dcd_handle_tx_completion(jdevice_t dev, pipe_desc_t *pipe)
{
    ep_desc_t *ep;

    KASSERT(pipe, ("dcd_handle_tx_completion: Invalid argument - No pipe\n"));
    ep = (ep_desc_t *)pipe->dcd_handle;
    dcd_handle_timeout(ep);
}

/**
 * Function name:  dcd_send_io_request
 * Description: Called by CORE to queue a request on the hardware
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @pipe: (IN) CORE pipe context
 *     @core_req: (IN) CORE request
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_send_io_request(jdevice_t dev, pipe_desc_t *pipe, 
    request_t *core_req)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    ep_desc_t *ep;
    request_t *start_req = NULL, *curr_req = NULL;
    juint_t dir = core_req->direction;
    jresult_t rc = 0;
    LIST_HEAD(req_list_head, request_t) req_list;

    /* See if this is the right time to transfer stuff */
    if ((sc->state == TDI_4X_DC_NOT_RDY) || 
        (sc->state == TDI_4X_DC_RST_DONE && pipe))
    {
        DBG_E(DSLAVE_DCD, ("DCD: Not ready to queue requests\n"));
        rc = JEINVAL;
        goto Exit;
    }
    
    /* If pipe == NULL then we are handling EP0 */
    if (pipe)
        ep = (ep_desc_t *)pipe->dcd_handle;
    else
        ep = get_ep_desc(sc, dir == DIRECTION_OUT ? 0 : 1);

    if (dir == DIRECTION_IN && !core_req->single_request)
    {
        if (!get_active_td(sc, ep))
        {
            dcd_handle_comp(sc, ep);

#ifndef CONFIG_IN_HW_QUEUE
            {
                request_t *request;

                /* Going over all the requests in the queue and
                 * creating one list from all of the requests */
                start_req = STAILQ_FIRST(&ep->in_req_queue);
                while (!STAILQ_EMPTY(&ep->in_req_queue))
                {
                    request = STAILQ_FIRST(&ep->in_req_queue);
                    LIST_FIRST(&req_list) = request;

                    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
                    {
                        if (!LIST_NEXT(curr_req, next_request_in_list))
                        {
                            LIST_NEXT(curr_req, next_request_in_list) = 
                                STAILQ_NEXT(request, next_request);
                            break;
                        }
                    }
                    STAILQ_REMOVE_HEAD(&ep->in_req_queue, next_request);
                }

                if (start_req)
                    LIST_NEXT(curr_req, next_request_in_list) = core_req;
            }
        }
        else
        {
            STAILQ_INSERT_TAIL(&ep->in_req_queue, core_req, next_request);
            goto Exit;
#endif
        }
    }

    if (!start_req)
        start_req = core_req;

    /* Checking if pipe is available */
#ifndef CONFIG_IN_HW_QUEUE
    if (ep->cur_req)
    {
        j_panic("DCD: cannot perform io request, pipe is busy 0x%x!!!\n",
            ep->soft_addr);
    }
#endif

    if (ep->timeout_task && start_req->timeout)
    {
        jtask_schedule(ep->timeout_task, core_req->timeout,
            dcd_handle_timeout, ep);
        ep->abort_timeout_task = 1;
    }

    /* Fire up the transfer */
    if (!start_req->single_request)
    {
        rc = start_multiple_transfer(sc, ep, start_req);
    }
    else
    {
        rc = start_transfer(sc, ep, start_req, dir, start_req->transfer_size, 
            start_req->buffer.dma_addr);
    }

Exit:
    if (rc)
    {
        LIST_FIRST(&req_list) = start_req;
        LIST_FOREACH(curr_req, &req_list, next_request_in_list)
            start_req->completed_req++;
    }
    return rc;
}

/**
 * Function name:  cancel_active_request
 * Description: Cancel the request in progress
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) Endpoint context
 *
 * Return value: None
 * Scope: local
 **/
static void cancel_active_request(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    tdi4x_td_t *cur_td;
    struct tdi4x_soft_td *td_ptr;
    request_t *cur_req;
    juint32_t status;
    juint32_t num_of_td = 0;

    DBG_I(DSLAVE_DCD, ("cancel_active_request: Entered. ep 0x%x\n", 
        ep->hw_addr));

    reset_ep(sc, ep);
    for (td_ptr = TAILQ_FIRST(&ep->td_head), cur_req = ep->cur_req; 
        td_ptr && cur_req; td_ptr = TAILQ_NEXT(td_ptr, next))
    {
        cur_td = TD(td_ptr);
        status = BFGET(TDI_4X_TD_STATUS, cur_td->td_header);

        if (!num_of_td)
            num_of_td = td_ptr->num_of_td;

        num_of_td--;

        if (status & TDI_4X_TD_ACTIVE)
            cur_req->status = REQUEST_CANCELLED;

        cur_td = &ep->qhead->cur_td;

        /* Clear Active & Halt bit in QH */
        cur_td->td_header &= htoc32(~BFMASK(TDI_4X_TD_STATUS));

        /* Mark next TD as NULL */
        cur_td->next_td_ptr = htoc32(0);

        if (!ep->cur_req->single_request && !num_of_td)
            cur_req = LIST_NEXT(cur_req, next_request_in_list);
    }

    dcd_handle_comp(sc, ep);
}

#define TDI_4X_CANCEL_QUEUE(head) do {\
    while (!STAILQ_EMPTY(head)) \
    { \
        request = STAILQ_FIRST(head); \
        LIST_FIRST(&req_list) = request; \
        LIST_FOREACH(cur_req, &req_list, next_request_in_list) \
        { \
            cur_req->status = REQUEST_CANCELLED; \
            request->completed_req++; \
        } \
        \
        /* Call completion callback */ \
        if (request->complete) \
            request->complete(request); \
        STAILQ_REMOVE_HEAD(head, next_request); \
    } \
} while (0)

/**
 * Function name:  cancel_queue
 * Description: Cancel all requests pending on the specific endpoint, and
 *              calling the completion callback for each request.
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) Endpoint context
 *
 * Return value: None
 * Scope: local
 **/
static void cancel_queue(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    request_t *request, *cur_req;
    LIST_HEAD(req_list_head, request_t) req_list;

    if (!ep->is_in && !ep->pipe)
        return; 

    if (ep->is_in)
        TDI_4X_CANCEL_QUEUE(&ep->in_req_queue);

    if (ep->pipe)
    {
        TDI_4X_CANCEL_QUEUE(&ep->pipe->req_queue);
        ep->pipe->status = PIPE_ENABLED;
    }
}

#ifdef CONFIG_JTRANSCEIVER
static void set_interface_type(tdi4x_soft_dc_t *sc)
{
    trcv_interface_type_t type = TRCV_INTRFACE_TYPE_DEFAULT;
    juint32_t portsc = 0;
    DECLARE_FNAME("set_interface_type");

    trcv_interface_type_get(sc->trcv, &type);

    switch (type)
    {
    case TRCV_INTRFACE_TYPE_DEFAULT:
        return;
    case TRCV_INTRFACE_TYPE_UTMI:
        portsc = TDI_4X_PORTSC_PTS_UTMI;
        break;
    case TRCV_INTRFACE_TYPE_ULPI:
        portsc = TDI_4X_PORTSC_PTS_ULPI;
        break;
    case TRCV_INTRFACE_TYPE_SERIAL:
        portsc = TDI_4X_PORTSC_PTS_SERIAL;
        break;
    default:
        DBG_E(DSLAVE_DCD, ("%s: Unknown type = %d\n", fname, type));
        return;
    }

    DBG_V(DSLAVE_DCD, ("%s: Setting Interface type to 0x%x\n", fname, portsc));

    TDI_4X_WRITE_REG(sc, TDI_4X_PORTSC_REG, 
        (TDI_4X_READ_REG(sc, TDI_4X_PORTSC_REG) & ~TDI_4X_PORTSC_PTS_MASK) |
        portsc);
}
#endif

/**
 * Function name:  dcd_enable
 * Description: Enable DCD by registering interrupt handlers(ISR & DSR)
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @is_full_speed: (IN) TRUE for full speed initialization mode. FALSE
 *                          otherwise
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_enable(jdevice_t dev, jbool_t is_full_speed)
{
    jresult_t rc;
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    juint32_t v;
#ifdef JDEBUG
    juint32_t x;
#endif
    
    DBG_V(DSLAVE_DCD, ("DCD: dcd_enable\n"));

#ifdef CONFIG_JOTG
    if (!sc->otg_dev)
#endif
    {
        /* asserting reset */
        TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, TDI_4X_USB_CMD_RST);

        /* waiting for reset to complete */
        while (TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & TDI_4X_USB_CMD_RST);

    /* moving to device mode FIXME */
#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#  define MODE_ENDIAN TDI_4X_MODE_BIG_ENDIAN
#elif CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
#  define MODE_ENDIAN 0x0
#endif

#ifdef CONFIG_TDI_4X_STREAM_DISABLE
#define MODE_SDIS TDI_4X_MODE_SDIS
#else
#define MODE_SDIS 0x0
#endif

#ifdef CONFIG_JTRANSCEIVER
        set_interface_type(sc);
#endif

#ifdef PRE_2_3
        TDI_4X_WRITE_REG(sc, TDI_4X_MODE_REG, 
            TDI_4X_MODE_DEVICE | MODE_ENDIAN | MODE_SDIS);
#else
        TDI_4X_WRITE_REG(sc, TDI_4X_MODE_REG, 
            TDI_4X_MODE_DEVICE | TDI_4X_MODE_SLOM | MODE_ENDIAN | MODE_SDIS);
#endif
    
        if (is_full_speed)
        {
            DBG_W(DSLAVE_DCD, ("dcd_enable: forcing device to connect as full "
                "speed\n"));
            TDI_4X_WRITE_REG(sc, TDI_4X_PORTSC_REG,
                TDI_4X_READ_REG(sc, TDI_4X_PORTSC_REG) | TDI_4X_PORTSC_PFSC);
        }

    #if defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)

      { /* creating a local scope to handle amplitude adjustment */
        juint8_t  read_value = 0;
        boolean   result = TRUE;
        const juint8_t ulpi_addr = 0x31;

       /* Qualcomm addition. Try to raise amplitude to 400mV */
       /* Set USB PHY Config 2/4 (addr: 0x31), hsdrvamplitude (bit 3:2) to '11' */

        result = hsu_os_bus_read_from_phy(ulpi_addr, &read_value);
        HSU_ASSERT(result);

        result = hsu_os_bus_write_to_phy(ulpi_addr, read_value | 0x0C);
        HSU_ASSERT(result);
      }
    #endif /* FEATURE_HS_USB_INTEGRATED_PHY && !FEATURE_HS_USB_NO_HS_OTG_CORE */
    }

#ifdef CONFIG_TDI_4X_ITC
    #define TDI_4X_USB_CMD_ITC CONFIG_TDI_4X_ITC
#else
    #define TDI_4X_USB_CMD_ITC TDI_4X_USB_CMD_ITC_IMMDIATE
#endif

    v = TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & ~TDI_4X_USB_CMD_ITC_MSK;
    v |= sc->itc_num;
    TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, v);

#if (defined(FEATURE_HS_USB_MS_FD) && !defined(FEATURE_HS_USB_BASIC))
    /* HSU addition, ITC optimization */
    /* ITC optimization according to the current USB composition */
    /* Note that the dcd also changes ITC so we assume this cb is invoked
       after dcd is already enabled */
    if (hsu_conf_sel_get_cur_comp_id() == HSU_CONF_SEL_MS_COMP)
    {
      /* for Mass Storage only composition use 0ms ITC */
      TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
        (TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & ~TDI_4X_USB_CMD_ITC_MSK));
    }
#endif /* defined(FEATURE_HS_USB_MS_FD) && !defined(FEATURE_HS_USB_BASIC) */

    /***********************/
    /* HSU Additions - end */

    /* updating queue list register */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_LST_ADDR_REG, (juint32_t)(sc->qhead_phys));

    rc = jinterrupt_setup(sc->irq_res, dcd_intr, sc, dcd_soft_intr, sc, 
        &sc->interrupt_h);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("dcd_enable: Failed to register interrupt %d\n",
            rc));
        goto Exit;
    }

    /* activating reset and suspend interrupts only */
    sc->intr_msk = TDI_4X_INTR_URE | TDI_4X_INTR_SLE;
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_ENB_REG, sc->intr_msk);

    /* HSU Temp fix */
    /* Enable bSessionEnd & bSessionValid interrupt for disconnect & connect */
    /* detection */
#ifdef CONFIG_JOTG
    if (!sc->otg_dev)
#endif
    {
        /* Clear the current interrupt bits */
        TDI_4X_WRITE_REG(sc, TDI_4X_OTGSC, 0x7F0000);
        TDI_4X_WRITE_REG(sc, TDI_4X_OTGSC, 
          TDI_4X_OTGSC_BSEIE | TDI_4X_OTGSC_BSVIE); 
    }
    /* HSU Temp fix */

    /* activating device */
    rc = dcd_vbus_connect(dev);
    
#ifdef JDEBUG
    x = TDI_4X_READ_REG(sc, TDI_4X_ID_REG);
#endif
    DBG_I(DSLAVE_DCD, ("DCD: dcd_enable id %x NID %x Rev %d\n\n\n",
        x & 0x1f, (x >> 7) & 0x3f, (x >> 15) & 0xff));

Exit:
    if (rc)
        dcd_disable(dev);
    return rc;
}

/**
 * Function name:  dcd_disable
 * Description: Disable DCD operation by disabling all USB interrupts
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static void dcd_disable(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);

    DBG_I(DSLAVE_DCD, ("   DCD: Disable !!! \n\n"));

    sc->intr_ep_setup = 0;
    sc->intr_ep_data = 0;
   
    /* Clear hardware registers */
    reset_hw(sc);

#ifdef CONFIG_JOTG
    if (!sc->otg_dev)
#endif
    {
        /* deactivating device */
        TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 0);
    }

    sc->intr_msk = 0;
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_ENB_REG, sc->intr_msk);

    if (sc->interrupt_h)
    {
        jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
        sc->interrupt_h = NULL;
    }

    DBG_X(DSLAVE_DCD, ("DCD: dcd_disable\n"));
}

#if defined(CONFIG_JSLAVE_TEST_MODE)
/**
 * Function name:  dcd_set_test_mode
 * Description: Configure the device into electrical test mode
 *              
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @test_mode: (IN) 
 *
 * Return value: 
 * Scope: local
 **/
static void dcd_set_test_mode(jdevice_t dev, juint16_t test_mode)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
   juint32_t portsc;

   DBG_V(DSLAVE_DCD, ("DCD: setting test mode (0x%x)\n", test_mode));

   /* Set the test mode */
   portsc = TDI_4X_READ_REG(sc, TDI_4X_PORTSC_REG);
   portsc &= 0xfff0ffff;
   portsc |= ((juint32_t)test_mode) << 16;
   TDI_4X_WRITE_REG(sc, TDI_4X_PORTSC_REG, portsc);

   return;
}
#endif

/**
 * Function name:  dcd_handle_feature
 * Description: Called by CORE when clear/set feature request is recieved from
 *              the host
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @feature: (IN) the feature to handle
 *     @index: (IN) endpoint number
 *     @set: (IN) 1 for set the feature, 0 for clear it
 *
 * Return value: 
 * Scope: local
 **/
static jresult_t dcd_handle_feature(jdevice_t dev, juint16_t feature, 
    juint16_t index, juint8_t set)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    juint8_t pipe_address = 0;
    jresult_t rc = JENOTSUP;

    DBG_V(DSLAVE_DCD, ("DCD: handle feature (%d) index (%d) set (%d)\n", 
        feature, index, set));

    /* changing feature */
    switch (feature)
    {
    case USB_ENDPOINT_HALT:
        /* setting stall */
        /* The pipes that it's software address is mod 2 are OUT pipes*/
        pipe_address = ((index & ~USB_DIR_IN)*2) + ((index & USB_DIR_IN)?1:0); 
        rc = dcd_stall_ep(sc->dev, get_pipe_desc(sc, pipe_address), set);
        break;
        
    default:
        break;
    }

    return rc;
}

/**
 * Function name:  dcd_set_address_state
 * Description: Called by CORE when set address request recieved from the host
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @address: (IN) Device address
 *
 * Return value: 0 on success, otherwise an error code 
 * Scope: local
 **/
static jresult_t dcd_set_address_state(jdevice_t dev, juint16_t address)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);

    DBG_V(DSLAVE_DCD, ("DCD: Set Address (%d)\n", address));

    /* storing address */       
    sc->address = address;

    /* setting address */
    TDI_4X_WRITE_REG(sc, TDI_4X_DEVICE_ADDR_REG, 
        TDI_4X_DEVICE_ADDR_SET(address) | 
        TDI_4X_DEVICE_ADDR_SET_ACC);
   
    /* Change state */
    sc->state = TDI_4X_DC_ADDR_SET;
    
    return 0;
}

/**
 * Function name:  dcd_set_config_state
 * Description: Called by CORE when set configuration completed
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *     @config: (IN) The selected configuration value
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t dcd_set_config_state(jdevice_t dev, juint8_t config)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);

    DBG_V(DSLAVE_DCD, ("DCD: Set Config \n"));

    sc->state = TDI_4X_DC_CONFIG_SET;

    return 0;
}

#ifdef CONFIG_JOTG
/**
 * Function name:  dcd_is_otg
 * Description:   Return True is device is part of and OTG Device 
 * Parameters:
 *     @dev: (IN) pointer to the device structure
 *
 * Return value:  True for OTG Device, else False 
 * Scope:          Local
 **/
static jbool_t dcd_is_otg(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    return sc->otg_dev ? 1 : 0;
}

/**
 * Function name:  dcd_invoke_otg_ioctl
 * Description:   Invoke an IOCTL from the DCD to the OTG's OCD 
 * Parameters:
 *     @dev: (IN) pointer to the device structure
 *     @ioctl: (IN) The IOCTL to invoke
 *     @args:  (IN) Parameters to the invoked IOCTL
 *
 * Return value: 0 on success, otherwise an error code
 * Scope:          Local
 **/
static jresult_t dcd_invoke_otg_ioctl(jdevice_t dev, drv_ioctl_t ioctl, 
    void *args)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    DECLARE_FNAME("dcd_invoke_otg_ioctl");

    if (!sc)
    {
        DBG_E(DSLAVE_DCD, ("%s: Invalid DCD Context!\n", fname));
        goto Error;
    }

    if (!sc->otg_dev)
    {
        DBG_E(DSLAVE_DCD, ("%s: DCD is not part of an OTG Device\n", fname));
        goto Error;
    }

    return j_device_ioctl(sc->otg_dev, ioctl, args);

Error:
    return JEINVAL;
}

#endif

/**
 * Function name:  dcd_handle_comp
 * Description: Handle completion of transfer completion - called from DSR
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) Endpoint context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static void dcd_handle_comp(tdi4x_soft_dc_t *sc, ep_desc_t *ep)
{
    struct tdi4x_soft_td *td_ptr;
    tdi4x_td_h td;
    request_t *curr_req, *req = ep->cur_req;
    request_t *prev_req = NULL;
    jint_t i, request_completed = 1;
    jresult_t rc = 0;
    juint32_t byte_not_xfered = 0; 
    juint32_t num_of_td = 0;
    juint8_t status;
    LIST_HEAD(req_list_head, request_t) req_list;

    LIST_FIRST(&req_list) = req;

    if (!req)
    {
        if (get_ep_desc(sc, 0) == ep)
        {
            /* For EP0 OUT, completion with no request is "kosher", due to the
             * automatic OUT status stage - we need to clean the TD for the
             * status stage */
            for (td_ptr = TAILQ_FIRST(&ep->td_head); td_ptr;
                td_ptr = TAILQ_FIRST(&ep->td_head)) 
            {
                /* Unlink and free the TD struct */
                FREE_TD(ep, td_ptr);
            }
        }
        else
        {
            /* For other endpoints, this should not happen */
#ifdef CONFIG_DATA_PATH_DEBUG
            DBG_W(DSLAVE_DCD, ("DCD: HANDLE COMP: no request (ep = %d) [%p]\n", 
                ep->soft_addr, ep));
#endif /* CONFIG_DATA_PATH_DEBUG */
        }
        return;
    }

    if ((ep->pipe && ep->pipe->status == PIPE_ABORTING) ||
        req->status == REQUEST_CANCELLED)
    {
        DBG_I(DSLAVE_DCD, ("DCD: HANDLE COMP: recieved completion on aborting "
            "pipe\n"));
    }
    else
    {
        /* Pass all TD's and check that all TD's completed ,
         * if not this ep is not finished yet.*/
        for (curr_req = req, td_ptr = TAILQ_FIRST(&ep->td_head);
            td_ptr && curr_req; td_ptr = TAILQ_NEXT(td_ptr, next))
        {
            td = TD(td_ptr);
            status = BFGET(TDI_4X_TD_STATUS, td->td_header);

            if (!num_of_td)
                num_of_td = td_ptr->num_of_td;

            num_of_td--;

            if (curr_req->status == REQUEST_PENDING)
            {
                request_completed = 0;
                break;
            }

            if (status & TDI_4X_TD_ACTIVE)
            {
                /* Active transfers still exist */
                return;
            }

            if (!req->single_request && !num_of_td)
                curr_req = LIST_NEXT(curr_req, next_request_in_list);
        }
    }    
    
    if (!request_completed)
    {
        /* Going over all the requests. Seperating the comleted request
           list from the non completed */
        LIST_FOREACH(curr_req, &req_list, next_request_in_list)
        {
            if (curr_req->status == REQUEST_PENDING)
            {
                ep->cur_req = curr_req;
                break;
            }
            prev_req = curr_req;
        }

        if (prev_req)
            LIST_NEXT(prev_req, next_request_in_list) = NULL;

        LIST_FIRST(&req_list) = ep->cur_req;

        /* Initializing the status of the non completed requests */
        LIST_FOREACH(curr_req, &req_list, next_request_in_list)
            curr_req->status = REQUEST_INPROGRESS;
    }
    else
    {
        /* freeing request from queue */
        ep->cur_req = NULL;
    }

    /* running through all the TDs looking for errors and freeing them */
    for (curr_req = req, i = 0, td_ptr = TAILQ_FIRST(&ep->td_head), 
        num_of_td = 0; td_ptr && curr_req; i++, 
        td_ptr = TAILQ_FIRST(&ep->td_head))
    {
        td = TD(td_ptr);

        if (!num_of_td)
            num_of_td = td_ptr->num_of_td;

        num_of_td--;

        /* error check */
        status = BFGET(TDI_4X_TD_STATUS, td->td_header);
        
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_V(DSLAVE_DCD, ("----- DCD: Finished ep %d td %d status %02x "
            "[%s%s%s%s] left %d bytes\n",
            ep->soft_addr, i, status,
            status & TDI_4X_TD_ACTIVE ? "A" : "",
            status & TDI_4X_TD_HALTED ? "H" : "", 
            status & TDI_4X_TD_DATA_ERROR ? "D" : "",
            status & TDI_4X_TD_TRAN_ERROR ? "T" : "",
            BFGET(TDI_4X_TD_TOTAL_BYTES, td->td_header)));
#endif /* CONFIG_DATA_PATH_DEBUG */
        
        if (status &
            (TDI_4X_TD_HALTED | TDI_4X_TD_DATA_ERROR | TDI_4X_TD_TRAN_ERROR))
        {
            DBG_E(DSLAVE_DCD, ("Found Error in TD%d: status 0x%x\n",
                i, status));
            rc = JEIO;
        }

        /* reading byte count */
        byte_not_xfered = 
            ctoh32(BFGET(TDI_4X_TD_TOTAL_BYTES, td->td_header));

        /* Updating bytes transferred counter */
        curr_req->bytes_transferred += td_ptr->td_size - byte_not_xfered;
        
        /* Unlink and free the TD struct */
        FREE_TD(ep, td_ptr);

        if (!req->single_request && !num_of_td)
            curr_req = LIST_NEXT(curr_req, next_request_in_list);
    }

    if (!rc && req->direction == DIRECTION_OUT && ep->pipe &&
        PIPE_TYPE_MASK(ep->pipe->type) != TYPE_ISOC)
    {
        /* Check if we need to create another TD to complete the transfer */
        if (req->single_request && req->bytes_transferred < req->transfer_size
            && byte_not_xfered == 0)
        {
            rc = start_transfer(sc, ep, req, req->direction, 
                req->transfer_size - req->bytes_transferred, 
                (juint8_t *)req->buffer.dma_addr + req->bytes_transferred);
            if (rc)
            {
                DBG_E(DSLAVE_DCD, ("dcd_handle_comp: Failed to start next TD "
                    "to complete the transfer (%d)\n", rc));
            }

            return;
        }
    }

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DSLAVE_DCD, ("DCD: Finished ep %d [%d] transfer %s size %d "
        "status %d done %d left %d req %p (%p)\n",
        ep->soft_addr, ep->hw_addr,
        req->direction == DIRECTION_IN ? "IN" : "OUT", 
        req->transfer_size, rc, req->bytes_transferred,
        req->transfer_size - req->bytes_transferred, req, req->complete));
#endif /* CONFIG_DATA_PATH_DEBUG */


    /* Handle (out) status stage of control transfers */
    if (req->status != REQUEST_CANCELLED && !rc && ep->soft_addr == 1 &&
        (req->ep0_zlp_reply || req->bytes_transferred != 0))
    {
        juint32_t prime;

        DBG_I(DSLAVE_DCD, ("DCD: Starting OUT Status stage\n"));

        req->ep0_zlp_reply = 0;
        /* Setup the transaction */
        prime = build_td_list(sc, get_ep_desc(sc, 0), 0, DIRECTION_OUT, NULL, 
            0);
        if (prime)
            TDI_4X_START_TRANSFER(sc, prime);
        else
            DBG_E(DSLAVE_DCD, ("DCD: No TDs created for transfer\n"));
    }

    LIST_FIRST(&req_list) = req;

    /* Set the request's status */
    LIST_FOREACH(curr_req, &req_list, next_request_in_list)
    {
        switch(curr_req->status)
        {
        case REQUEST_INPROGRESS:
            if (!rc)
                curr_req->status = REQUEST_COMPLETED;
            else 
                curr_req->status = REQUEST_ERROR;
            req->completed_req++;
            break;
        case REQUEST_CANCELLED:
            req->completed_req++;
            break;
        default:
            DBG_E(DSLAVE_DCD, ("DCD: request status incompatible for "
                "completion\n"));
        }
    }

    if (ep->abort_timeout_task && ep->timeout_task && req->timeout)
        jtask_stop(ep->timeout_task);

    if (ep->cur_req)
        ep->abort_timeout_task = 1;

    if (ep->pipe && (!ep->is_in || req->single_request))
    {
        /* Remove current request from the queue */
        STAILQ_REMOVE_HEAD(&ep->pipe->req_queue, next_request);
        if (request_completed)
        {
            if (ep->pipe->status != PIPE_ABORTING)
                start_next_transfer(sc, ep);
        }
        else
        {
            STAILQ_INSERT_HEAD(&ep->pipe->req_queue, ep->cur_req, next_request);
        }
    }

    /* calling request callback */
    if (req->complete)
    {
        /* Invalidate the cache before calling the completion call back */
        LIST_FOREACH(curr_req, &req_list, next_request_in_list)
        {
            if (!ep->is_in && curr_req->bytes_transferred)
            {
                j_cache_invalidate(curr_req->buffer.vaddr,
                    curr_req->bytes_transferred);
            }
        }

        req->complete(req);
    }
}

/**
 * Function name:  dcd_soft_intr
 * Description: DCD software interrupt handler. Handle post processing of
 *              hardware interrupts
 * Parameters: 
 *     @arg: (IN) tdi4x software context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static void dcd_soft_intr(void* arg)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)arg;
    jint_t rc = 0;
    juint32_t ep_data, ep_setup;
    juint8_t to_handle;
    
    jspinlock_lock(sc->lock);
    to_handle   = sc->intr_to_handle;
    sc->intr_to_handle   = 0;

    ep_data             = sc->intr_ep_data;
    sc->intr_ep_data    = 0;

    ep_setup            = sc->intr_ep_setup;
    sc->intr_ep_setup   = 0;
    jspinlock_unlock(sc->lock);
    
#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_X(DSLAVE_DCD, ("DCD: Soft interrupt ENTERED (setup 0x%x data 0x%x "
        "to_handle 0x%x) prime 0x%x status 0x%x complete 0x%x\n",
        ep_setup, ep_data, to_handle,
        TDI_4X_READ_REG(sc, TDI_4X_EP_PRIM_REG),
        TDI_4X_READ_REG(sc, TDI_4X_EP_STAT_REG),
        TDI_4X_READ_REG(sc, TDI_4X_EP_COMP_REG)));
#endif /* CONFIG_DATA_PATH_DEBUG */
    
#ifdef CONFIG_JOTG
    if (!sc->otg_dev)
#endif
    {
        if (to_handle & TDI_4X_DC_INTR_BSESS_END)
        {
            handle_disconnect(sc);
            sc->is_connected = 0;

            jnotify_os(NOTIFY_DEVICE_DISCONNECT, 
                (void *)j_device_get_bus(sc->dev));
        }
        /* HSU Temp fix */
        /* Needed an event for triggering USB wall charger detection */
        if (to_handle & TDI_4X_DC_INTR_BSESS_VALID)
        {
          jnotify_os(NOTIFY_DEVICE_CONNECT, 
            (void *)j_device_get_bus(sc->dev));
        }
        /* HSU Temp fix */
    }

    /* Handle bus reset */
    if (to_handle & TDI_4X_DC_INTR_BUS_RESET)
    {
        DBG_V(DSLAVE_DCD, ("DCD: Received bus reset\n"));

#ifdef CONFIG_JOTG
        if (!sc->otg_dev)
#endif
        {
            /* HSU Temp fix */
            /* If the device was not connected, notify on connect event */
            /* if (!sc->is_connected)
            {
                jnotify_os(NOTIFY_DEVICE_CONNECT, 
                    (void *)j_device_get_bus(sc->dev));
            }*/
            /* HSU Temp fix */
        }

        handle_resume(sc);
        handle_reset(sc);
    }

    /* Handle port change */
    if (to_handle & TDI_4X_DC_INTR_PORT_ATTACHED)
    {
        DBG_V(DSLAVE_DCD, ("DCD: Port change detected \n"));
       
        switch (TDI_4X_PORT_SPEED(TDI_4X_READ_REG(sc, TDI_4X_PORTSC_REG)))
        {
        case TDI_4X_PORT_SPEED_FULL:
            sc->dev_speed = SPEED_FULL;
            break;
        case TDI_4X_PORT_SPEED_HIGH:
            sc->dev_speed = SPEED_HIGH;
            break;
        default:
            DBG_E(DSLAVE_DCD, ("DCD: Wrong port speed detected\n"));
            sc->dev_speed = SPEED_HIGH;
        }

        /* If the device was in SUSPEND mode when port change detected, it means
         * that the host resume the bus activity */
        handle_resume(sc);

        DBG_V(DSLAVE_DCD, ("DCD: Port speed set to %s \n",
            sc->dev_speed==SPEED_HIGH ? "HIGH":"FULL"));
    }

    /* Handle suspend */
    if (to_handle & TDI_4X_DC_INTR_SUSPEND)
    {
        DBG_I(DSLAVE_DCD, ("DCD: Suspend from the host\n"));

        if (sc->is_connected)
            handle_suspend(sc);
    }

    /* Handle DATA completion */
    if (ep_data)
    {
        juint8_t i;
        juint8_t ep_num;
        ep_desc_t *ep;

        /* checking RX buffer */
        while (ep_data)
        {
            /* finding the first ep that assert the interrupt */
            for (i = 0 ; !(ep_data & (1 << i)) ; i++);

            if (i < 16)
                ep_num = i << 1;
            else
                ep_num = ((i - 16) << 1) + 1;

            ep = get_ep_desc(sc, ep_num);
            if (ep->handle_comp_from_intr)
            {
#ifdef CONFIG_DATA_PATH_DEBUG
                DBG_V(DSLAVE_DCD, ("DCD: Handling comp ep %s %d\n",
                    i < 16 ? "OUT" : "IN", ep_num));
#endif /* CONFIG_DATA_PATH_DEBUG */
                dcd_handle_comp(sc, get_ep_desc(sc, ep_num));
            }

            ep_data &= ~(1 << i);
        }
    }
    
    /* Handle Control Transfer */
    if (ep_setup)
    {
        struct usb_ctrlrequest *ctrl;
            
        DBG_I(DSLAVE_DCD, ("DCD: Soft: handling setup packet\n"));
        
        /* Since the controller is LE and host is BE there is endian-conversion
         * performed on the "control space". Since the data about the
         * control-packet is kept in the "control space" instead of the 
         * "data space" we need to convert it back to get the "real" data */
        ctrl = (struct usb_ctrlrequest *)((void *)sc->setup_packet);
        /* simple FIXME */
        ctrl->wValue    = htole16(ctrl->wValue);
        ctrl->wIndex    = htole16(ctrl->wIndex);
        ctrl->wLength   = htole16(ctrl->wLength);
        
        rc = core_handle_ep0(sc->core_ctx, sc->setup_packet, sc->dev_speed);

        /* STALL ep if retval is negative */
        if (rc)
        {
            DBG_E(DSLAVE_DCD, ("DCD: Setup request was handled rc = %d\n", rc));
            rc = dcd_stall_ep(sc->dev, NULL, 1);
        }

    }
}

/**
 * Function name:  dcd_intr
 * Description: DCD Interrupt handler. Handles all hardware interrupts, and
 *              prepare data for the software interrupt handler
 * Parameters: 
 *     @arg: (IN) tdi4x software context
 *
 * Return value: ISR_CALL_SOFT_INTR when software interrupt handling is needed,
 *               ISR_DONT_CALL_SOFT_INTR otherwise
 * Scope: local
 **/
static jint_t dcd_intr(void* arg)
{
    tdi4x_soft_dc_t *sc = (tdi4x_soft_dc_t *)arg;
    juint32_t rc = ISR_DONT_CALL_SOFT_INTR;
    /* HSU fix - Don't use ack to clear interrupts. This fix removed all 
       references to ack in this function */
    /*juint32_t ack = 0;*/
    juint32_t ep_setup, ep_data, intr_stat;
    /* HSU temp fix */
    juint32_t otgsc_reg, otgsc_ack;
    /* HSU temp fix */

    intr_stat = TDI_4X_READ_REG(sc, TDI_4X_INTR_STS_REG);
    /* HSU fix - clear interrupts here instead of at the end of this function */
    /* acknowledging interrupt */
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_STS_REG, intr_stat);
    ep_setup = TDI_4X_READ_REG(sc, TDI_4X_EP_SETUP_STAT_REG);
    ep_data = TDI_4X_READ_REG(sc, TDI_4X_EP_COMP_REG);

    /* DC Suspend */
    if (intr_stat & TDI_4X_INTR_STS_SLI)
    {
        sc->intr_to_handle |= TDI_4X_DC_INTR_SUSPEND;
        rc = ISR_CALL_SOFT_INTR;
    }

    /* Bus Reset */
    if (intr_stat & TDI_4X_INTR_STS_URE)
    {
        sc->intr_to_handle |= TDI_4X_DC_INTR_BUS_RESET;
        rc = ISR_CALL_SOFT_INTR;
    }
    
    /* Port Change Detected */
    if (intr_stat & TDI_4X_INTR_STS_PCI)
    {
        sc->intr_to_handle |= TDI_4X_DC_INTR_PORT_ATTACHED;
        rc = ISR_CALL_SOFT_INTR;
    }

    if (intr_stat & TDI_4X_INTR_STS_UE)
    {
        /* got setup packet */
        if (ep_setup)
        {
            /* Check if we need to abort previous TDs on EP0 OUT */
            if (!((sc->intr_ep_data & TDI_4X_COMP_RX_EP(0)) || 
                (ep_data & TDI_4X_COMP_RX_EP(0))))
            {
                abort_ep0(sc, 0);
            }
                
            /* Check if we need to abort previous TDs on EP0 IN */
            if (!((sc->intr_ep_data & TDI_4X_COMP_TX_EP(0)) ||
                (ep_data & TDI_4X_COMP_TX_EP(0))))
            {
                abort_ep0(sc, 1);
            }

            handle_setup_intr(sc);

            rc = ISR_CALL_SOFT_INTR;
            sc->intr_ep_setup |= ep_setup;
        }

        if (ep_data)
        {
            /* data xfer completed */
            rc = ISR_CALL_SOFT_INTR;
            sc->intr_ep_data |= ep_data;

            /* acking completion */
            TDI_4X_WRITE_REG(sc, TDI_4X_EP_COMP_REG, ep_data);
        }
    }

#ifdef CONFIG_JOTG
    if (!sc->otg_dev)
#endif
    {
        /* HSU Temp Fix */

        otgsc_reg = TDI_4X_READ_REG(sc, TDI_4X_OTGSC);
        otgsc_ack = 0;
        if (otgsc_reg  & TDI_4X_OTGSC_BSEIS)
        {
            rc = ISR_CALL_SOFT_INTR;
            sc->intr_to_handle |= TDI_4X_DC_INTR_BSESS_END;
            otgsc_ack |= TDI_4X_OTGSC_BSEIS;
        }
        if (otgsc_reg  & TDI_4X_OTGSC_BSVIS)
        {
            if (otgsc_reg  & TDI_4X_OTGSC_BSV)
            {
                rc = ISR_CALL_SOFT_INTR;
                sc->intr_to_handle |= TDI_4X_DC_INTR_BSESS_VALID;
            }
            otgsc_ack |= TDI_4X_OTGSC_BSVIS;
        }

        if (otgsc_ack != 0)
        {
            /* Clear all interrupt bits */
            otgsc_reg &= 0xff80ffff;
            /* Acknowledge only handled interrupts */
            TDI_4X_WRITE_REG(sc, TDI_4X_OTGSC, otgsc_reg | otgsc_ack); 
        }

        /* HSU Temp Fix */
    }

    /* HSU addition - Catching core system errors (Start). */
    /* High-Speed USB core system error. */
    if (intr_stat & TDI_4X_INTR_STS_SEI)
    {
      rc = ISR_DONT_CALL_SOFT_INTR;

      j_panic("HSU core system error has occurred.");
    }
    /* HSU addition - Catching core system errors (End). */

#ifdef FEATURE_HS_USB_PMIC_PHY    
    /* handle events posted from the outside */
    if ( ((interrupt_resource_info_type *)(sc->irq_res))->event & 
                                       HSU_OS_BUS_DISCONNECT_EVT )
    {
        rc = ISR_CALL_SOFT_INTR;
        sc->intr_to_handle |= TDI_4X_DC_INTR_BSESS_END;
    
        ((interrupt_resource_info_type *)(sc->irq_res))->event &= 
                                     (~HSU_OS_BUS_DISCONNECT_EVT);
        }

    if ( ((interrupt_resource_info_type *)(sc->irq_res))->event & 
                                       HSU_OS_BUS_CONNECT_EVT )
    {
        rc = ISR_CALL_SOFT_INTR;
        sc->intr_to_handle |= TDI_4X_DC_INTR_BSESS_VALID;
    
        ((interrupt_resource_info_type *)(sc->irq_res))->event &= 
                                     (~HSU_OS_BUS_CONNECT_EVT);
    }

#endif /* FEATURE_HS_USB_PMIC_PHY */

    return rc;
}

/**
 * Function name:  get_pipe_desc
 * Description: Search and return pipe_desc_t from ep_desc_table that represent
 *              endpoint <ep>
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address 
 *
 * Return value: Pointer to pipe_desc_t from ep_desc_table 
 * Scope: local
 **/
static pipe_desc_t* get_pipe_desc(tdi4x_soft_dc_t *sc, juint8_t ep)
{
    jint_t i;

    for (i = 0 ; i < sc->max_available_eps ; i++)
    {
        if ((sc->ep_desc_table[i].soft_addr) == (ep))
            return sc->ep_desc_table[i].pipe;
    }

    DBG_E(DSLAVE_DCD, ("DCD: cannot find pipe desc for ep %d", ep));

    return NULL;
}

/**
 * Function name:  get_ep_desc
 * Description: Search and return the ep_desc_table entry that represent
 *              endpoint <ep>
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @ep: (IN) endpoint software address
 *
 * Return value: Pointer to ep_desc_table entry
 * Scope: local
 **/
static ep_desc_t* get_ep_desc(tdi4x_soft_dc_t *sc, juint8_t ep)
{
    jint_t i;

    for (i = 0 ; i < sc->max_available_eps ; i++)
    {
        if ((sc->ep_desc_table[i].soft_addr) == (ep))
            return &sc->ep_desc_table[i];
    }

    DBG_E(DSLAVE_DCD, ("DCD: cannot find pipe desc for ep %d", ep));

    return NULL;
}

/**
 * Function name:  init_endpoints
 * Description: Initialize EP description table according to the controller
 *              capabilites (EP - hardware wise, PIPE - driver wise)
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t init_endpoints(tdi4x_soft_dc_t *sc)
{
    jint_t i;
    jresult_t rc;

    sc->ep_desc_table = (ep_desc_t *)
        jmalloc(sizeof(ep_desc_t) * sc->max_available_eps, M_ZERO);
    if (!sc->ep_desc_table)
    {
        DBG_E(DSLAVE_DCD, ("init_endpoints: Error allocating endpoint "
            "descriptors array\n"));
        return JENOMEM;
    }

    /* building qh */

    /* allocating EP queue head list (2 queues (IN and OUT) for each ep)*/
    rc = jdma_alloc((TDI_4X_DC_QUEUE_HEAD_SIZE * sc->max_available_eps), 
        TDI_4X_DC_QUEUE_HEAD_ALIGN, ((void **)(&sc->qhead_list)), 
        &sc->qhead_phys, M_ZERO | M_PAGE_ALIGN, &sc->qhead_list_cookie);

    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("init_endpoints: Error allocating buffer for QH\n"));
        return JENOMEM;
    }

    /* Initialize the endpoint QHeads */
    for (i = 0 ; i < sc->max_available_eps; i++)
    {
        tdi4x_qh_h qh;
        qh = (tdi4x_qh_h)(&sc->qhead_list[i * TDI_4X_DC_QUEUE_HEAD_SIZE]);

        /* i=0,1 - EP0 QHeads  */ 
        sc->ep_desc_table[i].soft_addr   = (juint8_t)i;
        sc->ep_desc_table[i].hw_addr     = (juint8_t)i >> 1;
        sc->ep_desc_table[i].packet_size = (i < 2) ? 64 : 1024; 
        sc->ep_desc_table[i].type_map    = (i < 2) ? TYPE_CTRL : TYPE_ALL;
        sc->ep_desc_table[i].is_in       = i % 2;
        sc->ep_desc_table[i].qhead       = qh;
        sc->ep_desc_table[i].inuse       = (i < 2) ? 1 : 0;
        sc->ep_desc_table[i].handle_comp_from_intr = 1;
        
        TAILQ_INIT(&sc->ep_desc_table[i].td_head);
        STAILQ_INIT(&sc->ep_desc_table[i].in_req_queue);

        qh->qh_header = htoc32(
            BFSET(TDI_4X_QH_MAX_PKT_LEN, 
                (i < 2) ? TDI_4X_MAX_PACKET_SIZE_EP0 : 0) |
            BFSET(TDI_4X_QH_MULT, 0) |
            BFSET(TDI_4X_QH_IOS, 1) |
            BFSET(TDI_4X_QH_ZLT, 1));
        qh->cur_td.next_td_ptr = htoc32(BFSET(TDI_4X_TD_TERMINATE, 1));
        qh->cur_td.td_header = htoc32(BFSET(TDI_4X_TD_STATUS, 0));
    }

    return 0;
}

/**
 * Function name:  abort_ep0
 * Description: Abort pending transfers on EP0
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *     @is_in: (IN) TRUE if aborting EP0 IN, FALSE otherwise
 *
 * Return value:   None
 * Scope:          Local
 **/
static void abort_ep0(tdi4x_soft_dc_t *sc, jbool_t is_in)
{
    ep_desc_t *ep;
    tdi4x_td_t *cur_td;

    ep = get_ep_desc(sc, is_in ? 1 : 0);
    reset_ep(sc, ep);
    sc->intr_ep_data |= (is_in ? 0x00010000 : 0x00000001);

    cur_td = &ep->qhead->cur_td;

    /* Clear Active & Halt bit in QH */
    cur_td->td_header &= htoc32(~BFMASK(TDI_4X_TD_STATUS));

    /* Mark next TD as NULL */
    cur_td->next_td_ptr = htoc32(0);

    if (ep->cur_req)
        ep->cur_req->status = REQUEST_CANCELLED;
}

/**
 * Function name:  handle_setup_intr
 * Description: Read setup packet from the controller and save it for the DRS 
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *
 * Return value: None
 * Scope: local
 **/
static void handle_setup_intr(tdi4x_soft_dc_t *sc)
{
    tdi4x_qh_h qh = sc->ep_desc_table[0].qhead;

#ifdef PRE_2_3
    TDI_4X_QH_GET_SETUP_REQ(qh, sc->setup_packet);

    /* clearing bit */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_SETUP_STAT_REG, TDI_4X_EP_SETUP_ACK(0));
#else
    /* clearing bit */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_SETUP_STAT_REG, TDI_4X_EP_SETUP_ACK(0));

    while (TDI_4X_READ_REG(sc, TDI_4X_EP_SETUP_STAT_REG) &
        TDI_4X_EP_SETUP_ACK(0))
    {
        DBG_V(DSLAVE_DCD, ("DCD: Wait for ep setup to clear\n"));
    }

    /* storing setup packet */
    do
    {
        /* setting Setup Trip Wire */
        TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
            TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) | TDI_4X_USB_CMD_SUTW);

        TDI_4X_QH_GET_SETUP_REQ(qh, sc->setup_packet);
    } 
    while (!(TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & TDI_4X_USB_CMD_SUTW))
        ;

    /* resetting Setup Trip Wire */
    TDI_4X_WRITE_REG(sc, TDI_4X_USB_CMD_REG, 
        TDI_4X_READ_REG(sc, TDI_4X_USB_CMD_REG) & ~TDI_4X_USB_CMD_SUTW);
#endif
}

/**
 * Function name:  reset_hw
 * Description: Reset the controller from all pending transfers, setup tokens
 *              and reset DSR data to prevent completion handling
 * Parameters: 
 *     @sc: (IN) tdi4x software context 
 *     
 * Return value: None
 * Scope: local
 **/
static void reset_hw(tdi4x_soft_dc_t *sc)
{
    /* Flushing ep data */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_FLUSH_REG, 0xFFFFFFFF);
   
    /* Wait for hardware to finish aborting */
    while (TDI_4X_READ_REG(sc, TDI_4X_EP_FLUSH_REG))
        ;

    /* Clear all the endpoint complete status bits */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_COMP_REG, 
        TDI_4X_READ_REG(sc, TDI_4X_EP_COMP_REG));

    /* Clear all setup token semaphores */
    TDI_4X_WRITE_REG(sc, TDI_4X_EP_SETUP_STAT_REG, 
        TDI_4X_READ_REG(sc, TDI_4X_EP_SETUP_STAT_REG));

    /* Block soft-intr from processing canceled requests */
    sc->intr_ep_data = 0;
}

/**
 * Function name:  handle_suspend
 * Description: Handle suspend signal from the host
 * Parameters:
 *     @sc: (IN) tdi4x software context
 *
 * Return value:   None
 * Scope:          Local
 **/
static void handle_suspend(tdi4x_soft_dc_t *sc)
{
    if (sc->is_suspended)
        return;

    /* Notify core on bus suspend */
    core_suspend(sc->core_ctx);

#ifdef CONFIG_JOTG
    if (sc->otg_dev)
    {
        /* Notify the OTG Stack that the peripheral is suspended */
        jresult_t rc = j_device_ioctl(sc->otg_dev, DRV_IOCTL_DEV_SUSPEND, NULL);
        if (rc)
        {
            DBG_E(DSLAVE_DCD, ("handle_suspend: Failed to notify OTG on "
                "suspend state\n"));
        }
    }
#endif

    sc->is_suspended = 1;
}

/**
 * Function name:  handle_resume
 * Description: Handle resume signal from the host
 * Parameters:
 *     @sc: (IN) tdi4x software context
 *
 * Return value:   None
 * Scope:          Local
 **/
static void handle_resume(tdi4x_soft_dc_t *sc)
{
    if (!sc->is_suspended)
        return;

    DBG_I(DSLAVE_DCD, ("DCD: Resume from the host\n"));

    core_resume(sc->core_ctx);

#ifdef CONFIG_JOTG
    /* Notify the OTG Stack that the peripheral is resumed */
    if (sc->otg_dev)
    {
        jresult_t rc = j_device_ioctl(sc->otg_dev, DRV_IOCTL_DEV_RESUME, NULL);
        if (rc)
        {
            DBG_E(DSLAVE_DCD, ("handle_resume: Failed to notify OTG on "
                "resume state\n"));
        }
    }
#endif

    sc->is_suspended = 0;
}

static void handle_disconnect(tdi4x_soft_dc_t *sc)
{
    /* Disable interrupts */
    sc->intr_msk = 0;
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_ENB_REG, sc->intr_msk);
    
    /* Waiting until all bits in the EP_PRIM_REG 0 */
    while (TDI_4X_READ_REG(sc, TDI_4X_EP_PRIM_REG))
        ;

    /* Tell the core we disconnected */
    core_disconnect(sc->core_ctx);

    /* activiating reset and suspend interrupts only */
    sc->intr_msk = TDI_4X_INTR_URE | TDI_4X_INTR_SLE;
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_ENB_REG, sc->intr_msk);
}

/**
 * Function name:  handle_reset
 * Description: Handle reset interrupt recieved on the bus
 * Parameters: 
 *     @sc: (IN) tdi4x software context
 *
 * Return value: None
 * Scope: local
 **/
static void handle_reset(tdi4x_soft_dc_t *sc)
{
    /* Updating status */       
    sc->state = TDI_4X_DC_RST_DONE;

    /*
     * Call disconnect only when:
     * 1. We are running in OTG configuration, or:
     * 2. Reset while connected.
     */
#ifdef CONFIG_JOTG
    if (sc->otg_dev || sc->is_connected)
#else
    if (sc->is_connected)
#endif
    {
        handle_disconnect(sc);
    }

    sc->is_connected = 1;

    /* Activating all interrupts */
    sc->intr_msk = TDI_4X_INTR_START;
    TDI_4X_WRITE_REG(sc, TDI_4X_INTR_ENB_REG, sc->intr_msk);
    
    /* HSU Addition - notify on device reset */
    jnotify_os(NOTIFY_DEVICE_RESET, (void *)j_device_get_bus(sc->dev));
    /* HSU Addition end */
}

/**
 * Function name:  dcd_get_max_packet0
 * Description: Return endpoint 0 max packet size
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: Endpoint 0 max packet size
 * Scope: local
 **/
static juint8_t dcd_get_max_packet0(jdevice_t dev)
{
    return TDI_4X_MAX_PACKET_SIZE_EP0;
}

/**
 * Function name:  tdi_4x_probe
 * Description: Check if dev is a TDI_4X controller 
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 when there is a match, otherwise an error code
 * Scope: local
 **/
static jresult_t tdi_4x_probe(jdevice_t dev)
{
    return 0;
}

/**
 * Function name:  tdi_4x_detach
 * Description: Detach a controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t tdi_4x_detach(jdevice_t dev)
{
    tdi4x_soft_dc_t *sc;
    jbus_h tdi4x_bus;

    if (!dev)
        return 0;

    tdi4x_bus = j_device_get_bus(dev);
    sc = (tdi4x_soft_dc_t *)j_device_get_softc(dev);
    if (!sc)
        return 0;

    core_disable_by_ctx(sc->core_ctx);

    if (sc->core_ctx)
        core_unregister_dcd(sc->core_ctx);

    if (sc->lock)
        jspinlock_uninit(sc->lock);
    if (sc->qhead_list_cookie)
        jdma_free(sc->qhead_list_cookie);
    if (sc->ep_desc_table)
    /* HSU addition: Memory leak */
    {
        /* Abort active TDS for all endpoints */
        juint32_t i;
        for (i = 0 ; i < sc->max_available_eps; i++)
        {
            ep_desc_t *ep = &sc->ep_desc_table[i];
            struct tdi4x_soft_td *td_ptr;
            for (td_ptr = TAILQ_FIRST(&ep->td_head); td_ptr;
                td_ptr = TAILQ_FIRST(&ep->td_head)) 
            {
                /* Unlink and free the TD struct */
                FREE_TD(ep, td_ptr);
            }
        }
        jfree(sc->ep_desc_table);
        sc->ep_desc_table = NULL;
    }
    /* End HSU addition */
    if (sc->reg_res)
        jbus_release_resource(tdi4x_bus, sc->reg_res);
    if (sc->irq_res)
        jbus_release_resource(tdi4x_bus, sc->irq_res);

#ifdef CONFIG_JTRANSCEIVER
    trcv_device_uninit(sc->trcv);
#endif

    return 0;
}

/**
 * Function name:  tdi_4x_attach
 * Description: Attachs a new controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t tdi_4x_attach(jdevice_t dev)
{    
    tdi4x_soft_dc_t *sc = j_device_get_softc(dev);
    jbus_h tdi4x_bus = j_device_get_bus(dev);
    jint_t resource_id = 0;
    jresult_t rc;

    DBG_I(DSLAVE_DCD, ("DCD: Attaching controller: %s\n", 
        j_device_get_nameunit(dev)));

#ifdef CONFIG_JOTG
    sc->otg_dev = j_device_get_ivars(dev);
    DBG_I(DSLAVE_DCD, ("DCD: DCD is %spart of and OTG Device\n",
        ((sc->otg_dev)? "" : "not ")));
    /* Make sure that the bus.c adds the MEM and IRQ resources for the DCD 
     * with ID = 1 */
    if (sc->otg_dev)
        resource_id = 1;
#endif
    
    /* initializing device controller data */
    sc->dev = dev;
    sc->dev_speed = SPEED_HIGH;
    sc->address = TDI_4X_DC_NO_ADDR;
    sc->state = TDI_4X_DC_NOT_RDY;
    /* Upon attach, mark the device as not connected and in suspend mode, to
     * allow detection of resume upon the first connection. */
    sc->is_connected = 0;
    sc->is_suspended = 1;

    /* getting registers base address */
    sc->reg_res = j_bus_alloc_resource(tdi4x_bus, SYS_RES_MEMORY, resource_id);
    sc->irq_res = j_bus_alloc_resource(tdi4x_bus, SYS_RES_IRQ, resource_id);
    if (!sc->reg_res || !sc->irq_res)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Error allocating bus resource for DCD\n"));
        rc = JENOMEM;
        goto Error;
    }

    sc->itc_num = TDI_4X_USB_CMD_ITC << TDI_4X_USB_CMD_ITC_SHFT;
    /* hard coded initialization of end point data */ 
    sc->max_available_eps = 2 * TDI_4X_HW_DEVICE_DEV_EP(
        TDI_4X_READ_REG(sc, TDI_4X_HW_DEVICE_REG));
    rc = init_endpoints(sc);
    if (rc)
        goto Error;

    rc = jspinlock_init(&sc->lock);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("DCD: failed to allocate spinlock (%d)\n", rc));
        goto Error;
    }
    
    /* Register controller with the Core */
    rc = core_register_dcd(dev, &ops, &sc->core_ctx);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("DCD: attach failed\n"));
        goto Error;
    }

#ifdef CONFIG_JTRANSCEIVER
    rc = trcv_device_init(dev, &sc->trcv);
    if (rc)
    {
        sc->trcv = NULL;
        DBG_E(DSLAVE_DCD, ("DCD: Transceiver init failed, rc=%d\n", rc));
        goto Error;
    }
#endif

    DBG_I(DSLAVE_DCD, ("DCD: Controller attached, available endpoints %d\n",
        sc->max_available_eps)); 
    return 0;

Error:
    tdi_4x_detach(dev);
    return rc;
}

/**
 * Function name:  tdi_4x_suspend
 * Description: Suspends the controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t tdi_4x_suspend(jdevice_t dev)
{
    return 0;
}

/**
 * Function name:  tdi_4x_resume
 * Description: Resumes the controller
 * Parameters: 
 *     @dev: (IN) pointer to the device structure
 *
 * Return value: 0 on success, otherwise an error code
 * Scope: local
 **/
static jresult_t tdi_4x_resume(jdevice_t dev)
{
    return 0;
}

static jresult_t tdi_4x_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    tdi4x_soft_dc_t *sc;
    jresult_t rc = 0;

    if (!self)
        goto Exit;

    sc = (tdi4x_soft_dc_t *)j_device_get_softc(self);
    if (!sc)
        goto Exit;
    
    switch(ioctl)
    {
#ifdef CONFIG_JTRANSCEIVER
    case DRV_IOCTL_DEVICE_INTR:
        /* Handle transceiver interrupt (connect/disconnect) */
        break;
#endif
    case DRV_IOCTL_DEV_ENABLE:
        rc = core_enable_by_ctx(sc->core_ctx);
        break;
    case DRV_IOCTL_DEV_DISABLE:
        core_disable_by_ctx(sc->core_ctx);
        break;
    case DRV_IOCTL_DEV_SET_PARAMS:
        sc->itc_num = (juint32_t)arg;
        break;
    default:
        DBG_E(DSLAVE_DCD, ("DCD: Unsupported IOCTL 0x%x\n", ioctl));
        rc = JEINVAL;
        break;
    }

Exit:
    return rc;
}

DRIVER_CONTROLLER_DECLARE(tdi_4x, sizeof(tdi4x_soft_dc_t))


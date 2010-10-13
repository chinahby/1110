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
#include "dcd_td243fc_rev2.h" 

#define NOT_IMPLEMETED(fname) jprintf(fname ": not implemented yet\n")

/* Convert from memory address to controller internal address format */
#define TD243FC_GET_IADDR(a) ((a) + CONFIG_TD243FC_DATA_MEMORY_OFFSET)

#define ENABLE_EP(n, is_in) \
    do { \
        sc->ep_intr_mask |= TD243FC_EP_BIT(n, is_in); \
        SET4(TD243FC_DMA_EP_ENABLE_REG, TD243FC_EP_BIT(n, is_in)); \
        SET4(TD243FC_EP_ENABLE_REG, TD243FC_EP_BIT(n, is_in)); \
        SET4(TD243FC_EP_DONE_ENABLE_REG, TD243FC_EP_BIT(n, is_in)); \
        SET4(TD243FC_EP_IMMEDIATE_INTR, TD243FC_EP_BIT(n, is_in)); \
    } while (0);

#define TOGGLE_IF_NOT_SET(reg, val) \
    if (!(READ4(reg) & val)) \
        WRITE4(reg, val);

#define TOGGLE_IF_SET(reg, val) \
    if (READ4(reg) & val) \
        WRITE4(reg, val)

#define EP_IS_IN(index) ((index) & 0x1 ? TRUE : FALSE)

/* HSU Addition */
#define TD243FC_DMA_SETTLE_TIME 100
/* HSU End */

static jresult_t dcd_enable(jdevice_t dev, jbool_t is_full_speed);
static void dcd_disable(jdevice_t dev);
static jresult_t dcd_wake_the_host(jdevice_t dev);
#ifdef CONFIG_JOTG
static jbool_t   dcd_is_otg(jdevice_t dev);
#endif
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

static dcd_ops_t dcd_ops = {
    dcd_enable,                /* dcd_enable */ 
    dcd_disable,               /* dcd_disable */
    dcd_wake_the_host,         /* dcd_wake_the_host */
    NULL,                      /* dcd_set_self_power */
#ifdef CONFIG_JOTG
     dcd_is_otg,               /* dcd_is_otg */
     NULL,                     /* dcd_invoke_otg_ioctl */
#endif
    NULL,                      /* dcd_vbus_connect */
    NULL,                      /* dcd_vbus_draw_amount */
    NULL,                      /* dcd_vbus_disconnect */
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
    NULL,                      /* dcd_handle_tx_completion */       
    NULL                       /* dcd_set_test_mode */
};

static dcd_hw_init(td243fc_rev2_softc_t *sc);

#define GET_FREE_SIZE(sc) \
    (CONFIG_TD243FC_DATA_MEMORY_SIZE - (sc)->free_mem_start)
static jresult_t td243fc_mem_alloc(td243fc_rev2_softc_t *sc, juint16_t size, 
    juint16_t *addr)
{
    if (size <= GET_FREE_SIZE(sc))
    {
        *addr = sc->free_mem_start;
        sc->free_mem_start += size;
        
        DBG_X(DSLAVE_DCD, ("DCD: td243_mem_alloc size=%d addr=0x%.4x, free=%d"
            "\n", size, *addr, GET_FREE_SIZE(sc)));
        
        return 0;
    }
    return JENOMEM;
}

static void td243fc_mem_free(td243fc_rev2_softc_t *sc, juint16_t size)
{
    sc->free_mem_start -= size;
    DBG_V(DSLAVE_DCD, ("DCD: td243_mem_free size=%d, free=%d (start=%d)\n", 
        size, GET_FREE_SIZE(sc), sc->free_mem_start));
}

static void start_zero_packet(td243fc_rev2_softc_t *sc, jint_t ep_n)
{
    if (EP_IS_IN(ep_n))
    {
        TOGGLE_IF_NOT_SET(TD243FC_XFILLED_STATUS_REG, 
            TD243FC_EP_BIT_INDEX(ep_n));
    }

    if (sc->add_to_list)
        sc->ep_ready |= TD243FC_EP_BIT_INDEX(ep_n);
    else
        WRITE4(TD243FC_EP_READY_REG, TD243FC_EP_BIT_INDEX(ep_n));
}

static void start_dma_transfer(td243fc_rev2_softc_t *sc, jint_t ep_n, 
     juint32_t size)
{
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n];
    jbool_t is_in = EP_IS_IN(ep_n);
    juint32_t xybuf_size = ep->buf_size;

    WRITE4(TD243FC_DMA_EP_SYSTEM_MEM_ADDR_REG((jint_t)(ep_n / 2), is_in), 
        ep->req ? (juint32_t)ep->req->buffer.dma_addr : 
        (juint32_t)sc->dma_buf.dma_addr);

    if (ep->req)
        xybuf_size = MIN(xybuf_size, ep->req->buffer.buffer_size);
    else
        xybuf_size = MIN(xybuf_size, sc->dma_buf.buffer_size); 

    TOGGLE_IF_SET(TD243FC_XFILLED_STATUS_REG, TD243FC_EP_BIT_INDEX(ep_n));
    TOGGLE_IF_SET(TD243FC_YFILLED_STATUS_REG, TD243FC_EP_BIT_INDEX(ep_n));
    TOGGLE_IF_SET(TD243FC_XBUF_INT_STATUS_REG, TD243FC_EP_BIT_INDEX(ep_n));
    TOGGLE_IF_SET(TD243FC_YBUF_INT_STATUS_REG, TD243FC_EP_BIT_INDEX(ep_n));

    WRITE4(TD243FC_EPN_PACKET_CONTROL_REG((jint_t)(ep_n / 2), is_in),
        BFSET4(TD243FC_BUFFERSIZE, xybuf_size - 1) | 
        BFSET4(TD243FC_TTLBTECNT, size));

    SET4(TD243FC_DMA_EP_ENABLE_REG, TD243FC_EP_BIT_INDEX(ep_n));

    if (sc->add_to_list)
        sc->ep_ready |= TD243FC_EP_BIT_INDEX(ep_n);
    else
        WRITE4(TD243FC_EP_READY_REG, TD243FC_EP_BIT_INDEX(ep_n));
}

static void prepare_setup_transfer(td243fc_rev2_softc_t *sc)
{
    /* Set EP OUT for setup transfer */
    TOGGLE_IF_SET(TD243FC_EP_TOGGLE_REG, TD243FC_EP_BIT(0, 0));
    start_dma_transfer(sc, 0, 8);
}

static void set_hw_address(td243fc_rev2_softc_t *sc)
{
    WRITE4(TD243FC_DEVICEADDRESS_REG, BFSET4(TD243FC_DEVADDR, sc->address));
    sc->pendig_address = FALSE;
}

static void handle_reset(td243fc_rev2_softc_t *sc)
{
    if (sc->is_suspend)
    {
        core_resume(sc->core_ctx);
        sc->is_suspend = 0;
    } 

    core_disconnect(sc->core_ctx);

    sc->address = 0;
    set_hw_address(sc);

    sc->sys_intr_mask  |= (TD243FC_SUSP | TD243FC_DONEREG);
    WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr_mask);

    /* Configure EP0 OUT for SETUP packet */
    WRITE4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 0), 
        BFSET4(TD243FC_FORMAT, TD243FC_CTRL_EP) |
        BFSET4(TD243FC_MAXPKTSIZ, TD243FC_EP0_MAX_PACKET_SIZE));
    WRITE4(TD243FC_EPN_XY_BUFFER_ADDRESS_REG(0,0),
        BFSET4(TD243FC_XBSA, TD243FC_GET_IADDR(sc->ep[0].xaddr)) | 
        BFSET4(TD243FC_YBSA, TD243FC_GET_IADDR(sc->ep[0].yaddr)));

    /* Configure EP0 IN */
    WRITE4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 1), 
        BFSET4(TD243FC_FORMAT, TD243FC_CTRL_EP) | 
        BFSET4(TD243FC_MAXPKTSIZ, TD243FC_EP0_MAX_PACKET_SIZE));
    WRITE4(TD243FC_EPN_XY_BUFFER_ADDRESS_REG(0,1), 
        BFSET4(TD243FC_XBSA, TD243FC_GET_IADDR(sc->ep[1].xaddr)) |
        BFSET4(TD243FC_YBSA, TD243FC_GET_IADDR(sc->ep[1].yaddr)));

    /* Reset toggle for all endpoints */
    WRITE4(TD243FC_EP_TOGGLE_REG, READ4(TD243FC_EP_TOGGLE_REG));

    /* Enable EP0 */
    ENABLE_EP(0, 0);
    ENABLE_EP(0, 1);

    WRITE4(TD243FC_EP_READY_REG, READ4(TD243FC_EP_READY_REG)); 
    prepare_setup_transfer(sc);
}

static jresult_t start_transfer(td243fc_rev2_softc_t *sc, jint_t ep_n,
    request_t *req)
{
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n]; 
    juint32_t transfer_size = req->transfer_size - req->bytes_transferred;
    KASSERT(!ep->req, ("Request still running\n"));

    DBG_V(DSLAVE_DCD, ("DCD: start_transfer, ep=%d %s %d bytes "
        "(z%d zlp%d)\n", ep_n, req->direction == DIRECTION_IN ? "IN" : "OUT",
        req->transfer_size, req->zero, req->ep0_zlp_reply));

    if (ep->pipe && ep->pipe->status == PIPE_STALLLED)
    {
        DBG_V(DSLAVE_DCD, ("Trying to start transfer on Stalled endpoint\n"));
        goto Exit;
    }

    ep->req = req;

    if (req->direction == DIRECTION_IN && ep_n == 1)
    {
        /* Make sure data stage start with DATA1 token */
        TOGGLE_IF_NOT_SET(TD243FC_EP_TOGGLE_REG, TD243FC_EP_BIT(0, 1));
    }

    /* HSU Addition */
    /* Don't start DMA on zero length OUT transaction. Sometimes it interferes
       with the DMA of the next SETUP transaction */
    if (transfer_size)
    /* HSU End */
        start_dma_transfer(sc, ep_n, transfer_size);
    else
        start_zero_packet(sc, ep_n);
Exit:
    return 0;
}

static jresult_t start_next_transfer(td243fc_rev2_softc_t *sc, jint_t ep_n)
{
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n]; 

    KASSERT(ep_n > 1, ("No queue for ep0\n"));

    if (STAILQ_EMPTY(&ep->pipe->req_queue))
    {
        DBG_X(DSLAVE_DCD, ("DCD: start_next_trasfer: queue is empty\n"));
        return 0;
    }

    /* Start the next request on queue */
    return dcd_send_io_request((void *)sc, ep->pipe, 
        STAILQ_FIRST(&ep->pipe->req_queue));
}

static void req_finish(td243fc_rev2_softc_t *sc, jint_t ep_n, request_t *req,
    request_status_t status)
{
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n]; 

    DBG_V(DSLAVE_DCD, ("DCD: req_finish %s len=%d act_len=%d\n", 
        (req->direction == DIRECTION_IN ? "IN" : "OUT"), req->transfer_size,
        req->bytes_transferred));

    req->status = status;
    ep->req = NULL;

    if (ep_n > 1) /* Non Control EP's*/
    {
        /* Remove current request from the queue */
        STAILQ_REMOVE_HEAD(&ep->pipe->req_queue, next_request);

        if (status != REQUEST_CANCELLED)
            start_next_transfer(sc, ep_n);
    }

    /* calling request callback */
    if (req->complete)
    {
        /* Invalidate the cache before calling the completion call back */
        if (req->direction != DIRECTION_IN && req->bytes_transferred)
            j_cache_invalidate(req->buffer.vaddr, req->bytes_transferred);

        req->complete(req);
    }
    
    if (ep_n < 2)
    {
        if (sc->pendig_address)
            set_hw_address(sc);
        prepare_setup_transfer(sc);
    }
}

static void handle_epn(td243fc_rev2_softc_t *sc, jint_t ep_n)
{
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n]; 
    request_t *req = ep->req;
    jbool_t is_in = EP_IS_IN(ep_n);

    DBG_V(DSLAVE_DCD, ("DCD: handle_epn_out_req %d\n", ep_n));

    /* Canceled requests handled in abort */
    if (req->status == REQUEST_CANCELLED) 
        return;

    req->bytes_transferred = req->transfer_size - BFGET4(TD243FC_TTLBTECNT, 
        READ4(TD243FC_EPN_PACKET_CONTROL_REG(ep_n/2, is_in)));

    req_finish(sc, ep_n, req, REQUEST_COMPLETED);
}

/* 
 * IN transfer on EP0
 * 1. SETUP IN(0)
 * 2. SETUP IN(0, ep0_zlp_reply)   OUT
 * 3. SETUP IN(act=len, zero)      OUT
 * 4. SETUP IN(act=len)            OUT
 * 5. SETUP IN(act<len)            OUT 
 */
static void handle_ep0_in_req(td243fc_rev2_softc_t *sc)
{
    td243fc_rev2_ep_t *ep = &sc->ep[1]; 
    request_t *req = ep->req;

    DBG_V(DSLAVE_DCD, ("DCD: handle_ep0_in_req\n"));

    /* Canceled requests handled in abort */
    if (req->status == REQUEST_CANCELLED) 
        return;

    req->bytes_transferred = req->transfer_size - BFGET4(TD243FC_TTLBTECNT, 
        READ4(TD243FC_EPN_PACKET_CONTROL_REG(0, 1)));

    /* Check if it is a completion on data stage */
    if ((req->transfer_size && !req->ep0_zlp_reply) ||
        (!req->transfer_size && req->ep0_zlp_reply))
    {
        ep->req = NULL;
        /* Start status OUT status stage */
        start_transfer(sc, 0, req); 
    }
    else
    {
        req_finish(sc, 1, req, REQUEST_COMPLETED);
    }
}

static void handle_ep0_out_req(td243fc_rev2_softc_t *sc)
{
    td243fc_rev2_ep_t *ep = &sc->ep[0]; 
    request_t *req = ep->req;

    DBG_V(DSLAVE_DCD, ("DCD: handle_ep0_out_req\n"));
    
    /* Canceled requests handled in abort */
    if (req->status == REQUEST_CANCELLED) 
        return;

    req->bytes_transferred = req->transfer_size - BFGET4(TD243FC_TTLBTECNT, 
        READ4(TD243FC_EPN_PACKET_CONTROL_REG(0, 0)));

    req_finish(sc, 0, req, REQUEST_COMPLETED);
}

static void handle_setup(td243fc_rev2_softc_t *sc)
{
    jresult_t rc;

    /* HSU Addition */
    /* Fix for the case where SETUP EP interrupt received before previous 
       OUT EP interrupt was handled. In this case we need to first handle 
       the OUT transaction. */
    td243fc_rev2_ep_t *ep = &sc->ep[0]; 
    request_t *req = ep->req;

    if (req != NULL)
    {
      DBG_W(DSLAVE_DCD, ("DCD: handle_setup, ep0_out is pending\n"));
      handle_ep0_out_req(sc);
      /* Wait for Setup DMA initiated in handle_ep0_out_req by req_finish to 
         complete before processing the SETUP. */
      jdelay_us(TD243FC_DMA_SETTLE_TIME);
    }
    /* HSU End */

    DBG_V(DSLAVE_DCD, ("DCD: handle SETUP\n"));

    CLEAR4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 0), TD243FC_SETUP_RECV);

    rc = core_handle_ep0(sc->core_ctx, sc->dma_buf.vaddr , SPEED_FULL);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Setup request was handled rc = %d\n", rc));
        dcd_stall_ep(sc->dev, NULL, 1);
        prepare_setup_transfer(sc);
    }
}

static void handle_ep0(td243fc_rev2_softc_t *sc, juint32_t ep_intr)
{
    DBG_V(DSLAVE_DCD, ("DCD: handle_ep0\n"));
    
    if(READ4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 1)) & TD243FC_STL)
        dcd_stall_ep(sc->dev, NULL, 0);

    if (ep_intr & TD243FC_EP_BIT(0, 1))
    {
        handle_ep0_in_req(sc);
    }
    else 
    {
        if (READ4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 0)) & TD243FC_SETUP_RECV)
            handle_setup(sc);
        else
            handle_ep0_out_req(sc);
    }
}

/* Set/Clear endpoint STALL flag */
/* Flag: 0/1-Clear/Set */
static jresult_t dcd_stall_ep(jdevice_t dev, pipe_desc_t *pipe, jbool_t flag)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);

    DBG_V(DSLAVE_DCD, ("DCD: dcd_stall_ep, addr=%02x\n", pipe?pipe->address:0));

    if (pipe)
    {
        jint_t ep_n = (jint_t)(pipe->dcd_handle);
        jbool_t is_in = EP_IS_IN(ep_n);

        ep_n /= 2;

        KASSERT(pipe->dcd_handle, ("Pipe was not allocated\n"));

        if (flag)
        {
            SET4(TD243FC_EPN_CONFIG_CONTROL_REG(ep_n, is_in), TD243FC_STL);
            pipe->status = PIPE_STALLLED;
        }
        else
        {
            TOGGLE_IF_SET(TD243FC_EP_TOGGLE_REG, TD243FC_EP_BIT(ep_n, is_in));
            if (pipe->status == PIPE_STALLLED)
            {
                CLEAR4(TD243FC_EPN_CONFIG_CONTROL_REG(ep_n, is_in), 
                    TD243FC_STL);
                pipe->status = PIPE_ENABLED;
                start_next_transfer(sc, (jint_t)(pipe->dcd_handle));
            }
        }
    }
    else
    {
        if (flag)
            SET4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 1), TD243FC_STL);
        else
            CLEAR4(TD243FC_EPN_CONFIG_CONTROL_REG(0, 1), TD243FC_STL);
    }

    return 0;
}

static jint_t dcd_intr(void* arg)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)arg;
    juint32_t chip_intr;

    chip_intr = READ4(TD243FC_CHIP_INT_STATUS_REG);

    if (!(chip_intr & TD243FC_INTF))
        return ISR_DONT_CALL_SOFT_INTR;

    sc->sys_intr |= READ4(TD243FC_SYSTEM_INT_STATUS_REG);
    sc->ep_intr |= READ4(TD243FC_EP_DONE_STATUS_REG);
    sc->fc_cmd |= READ4(TD243FC_COMMAND_REG);

    /* Disable device controller global interrupt */
    CLEAR4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr);
    CLEAR4(TD243FC_CHIP_INT_ENABLE_REG, TD243FC_INTF);

    return ISR_CALL_SOFT_INTR;
}

static void dcd_soft_intr(void *arg)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)arg;
    juint32_t sys_intr, ep_intr;
    juint32_t fc_cmd;

    DBG_V(DSLAVE_DCD, ("DCD: dcd_soft_intr\n"));

    jspinlock_lock(sc->sc_lock);
    {
        sys_intr = sc->sys_intr;
        ep_intr = sc->ep_intr; 
        fc_cmd = sc->fc_cmd;
        sc->sys_intr = sc->ep_intr = sc->fc_cmd = 0;
    }
    jspinlock_unlock(sc->sc_lock);
    
    sys_intr &= sc->sys_intr_mask;
    ep_intr &= sc->ep_intr_mask;

    /* Acknowledge the interuppts */
    WRITE4(TD243FC_SYSTEM_INT_STATUS_REG, sys_intr);

    /* HSU Addition */
    /* Handle EP interrupts first. Fix for the case where OUT EP interrupt and 
       subsequent RESET interrupt are handled together. So the OUT EP interrupt 
       has to be handled first. The opposite case was not observed. */
    if (sys_intr & TD243FC_DONEREG)
    {
        juint_t i;

        sc->add_to_list = TRUE;

        /* Handle interrupts on EP0 (IN and OUT interrupts) */
        if (ep_intr & (TD243FC_EP_BIT(0, 0) | TD243FC_EP_BIT(0, 1)))
            handle_ep0(sc, ep_intr);

        /* Handle interrupts on endpoints other than EP0 */
        for (i = 2; i < TD243FC_EP_NUM * 2; i++)
        {
            if (ep_intr & (1<<i))
                handle_epn(sc, i);
        }

        /* Clear interrupts of handled EPs */
        WRITE4(TD243FC_EP_DONE_STATUS_REG, ep_intr);
        sc->add_to_list = FALSE;
        if (sc->ep_ready)
        {
            WRITE4(TD243FC_EP_READY_REG, sc->ep_ready);
            sc->ep_ready = 0;
        }
    }
    /* HSU End */

    if ((sys_intr & TD243FC_BUSRST) && (fc_cmd & TD243FC_RESETDET))
    {
      DBG_V(DSLAVE_DCD, ("DCD: Bus reset detected\n"));

      handle_reset(sc);

      /* Since we enable interrupts other then BUSRST only in handle_reset,
      * no need to handle other interrupts together with BUSRST */
      goto Exit;
    }

    if ((sys_intr & TD243FC_RESUME) && (fc_cmd & TD243FC_RSMINPROG))
    {
      if (sc->is_suspend)
      {
        DBG_V(DSLAVE_DCD, ("DCD: RESUME detected\n"));
        core_resume(sc->core_ctx);
        sc->is_suspend = 0;
        sc->sys_intr_mask  &= ~TD243FC_RESUME;
        sc->sys_intr_mask  |= TD243FC_SUSP;
        WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr_mask);
      }
    }

    if ((sys_intr & TD243FC_SUSP) && (fc_cmd & TD243FC_SUSPDET))
    {
      if (!sc->is_suspend)
      {
        DBG_V(DSLAVE_DCD, ("DCD: SUSPEND detected\n"));
        core_suspend(sc->core_ctx);
        sc->is_suspend = 1;
        sc->sys_intr_mask  &= ~TD243FC_SUSP;
        sc->sys_intr_mask  |= TD243FC_RESUME;
        WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr_mask);
        SET4(TD243FC_COMMAND_REG, TD243FC_SUSPDET);
      }
    }

    /* HSU Addition */

    /* Moved handling of TD243FC_DONEREG interrupt from here to the beginning 
       of the function */

    /* HSU End */
    WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr_mask);
Exit:
    /* Ack & Enable device controller global interrupt */
    SET4(TD243FC_CHIP_INT_ENABLE_REG, TD243FC_INTF);
    DBG_V(DSLAVE_DCD, ("DCD: dcd_soft_intr finished\n\n\n"));
}

/* Enable the controller to accept connections */
static jresult_t dcd_enable(jdevice_t dev, jbool_t is_full_speed)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jresult_t rc;
    __VOLATILE__ juint32_t v;

    DBG_I(DSLAVE_DCD, ("DCD: dcd_enable\n"));

    rc = jinterrupt_setup(sc->irq_res, dcd_intr, sc, dcd_soft_intr, sc, 
        &sc->interrupt_h);
    if (rc)
        return rc;

    sc->is_suspend = 0;
    sc->free_mem_start = 0;

    /* Allocate buffers for EP0 IN and OUT */
    sc->ep[0].buf_size = TD243FC_EP0_MAX_PACKET_SIZE;
    sc->ep[1].buf_size = TD243FC_EP0_MAX_PACKET_SIZE;

    sc->pendig_address = FALSE;
    sc->address = 0;
    sc->add_to_list = FALSE;
    sc->ep_ready = 0;

    rc = td243fc_mem_alloc(sc, sc->ep[0].buf_size, &sc->ep[0].xaddr);
    KASSERT(!rc, ("Can't alloc buffers for EP0 X-OUT\n"));
    rc = td243fc_mem_alloc(sc, sc->ep[0].buf_size, &sc->ep[0].yaddr);
    KASSERT(!rc, ("Can't alloc buffers for EP0 Y-OUT\n"));
    
    rc = td243fc_mem_alloc(sc, sc->ep[1].buf_size, &sc->ep[1].xaddr);
    KASSERT(!rc, ("Can't alloc buffers for EP0 X-IN\n"));
    rc = td243fc_mem_alloc(sc, sc->ep[1].buf_size, &sc->ep[1].yaddr);
    KASSERT(!rc, ("Can't alloc buffers for EP0 Y-IN\n"));
    
    /* Disable Chip level interrupt */
    WRITE4(TD243FC_CHIP_INT_ENABLE_REG, 0);

    /* System interrupts init */
    WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, 0);

    /* Clear interrupts */
    v = READ4(TD243FC_SYSTEM_INT_STATUS_REG);
    WRITE4(TD243FC_SYSTEM_INT_STATUS_REG, v);

    sc->sys_intr_mask = (TD243FC_BUSRST | TD243FC_SUSP);
    WRITE4(TD243FC_SYSTEM_INT_ENABLE_REG, sc->sys_intr_mask);

    /* Disable and clear all endpoint done interrupts */
    sc->ep_intr_mask = 0;
    WRITE4(TD243FC_EP_DONE_ENABLE_REG, sc->ep_intr_mask);
    WRITE4(TD243FC_EP_DONE_STATUS_REG, 0xffffffff);

    /* Zero the address */
    WRITE4(TD243FC_DEVICEADDRESS_REG, BFSET4(TD243FC_DEVADDR, 0x0));

    /* Clear all EpDma channels */
    WRITE4(TD243FC_DMA_EP_ENABLE_REG, 0x0);
    WRITE4(TD243FC_DMA_INTR_STATUS_REG, TD243FC_DMA_EPERR);
    WRITE4(TD243FC_DMA_EP_ERROR_STATUS_REG, TD243FC_DMA_EPERR);

    /* Enable device controller interrupts */
    SET4(TD243FC_CHIP_INT_ENABLE_REG, TD243FC_INTF);

    return 0;
}

/* Stop and Disconnect controller */
static void dcd_disable(jdevice_t dev)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    
    DBG_I(DSLAVE_DCD, ("DCD: dcd_disable\n"));

    CLEAR4(TD243FC_CHIP_INT_ENABLE_REG, TD243FC_INTF);

    td243fc_mem_free(sc, sc->ep[0].buf_size * 2);
    td243fc_mem_free(sc, sc->ep[1].buf_size * 2);

    if (sc->interrupt_h)
    {
        jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
        sc->interrupt_h = NULL;
    }
}

/* Trigger REMOTE-WAKEUP event */
static jresult_t dcd_wake_the_host(jdevice_t dev)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    
    WRITE4(TD243FC_COMMAND_REG, TD243FC_RSMINPROG);

    return 0;
}

#ifdef CONFIG_JOTG
static jbool_t dcd_is_otg(jdevice_t dev)
{
  return FALSE;
}
#endif

/* Enable an endpoint for operation */
static jresult_t dcd_enable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jint_t ep_n = (jint_t)(pipe->dcd_handle);
    td243fc_rev2_ep_t *ep = &sc->ep[ep_n];
    jresult_t rc;
    juint_t type = 0;
    jbool_t is_in = EP_IS_IN(ep_n);

    KASSERT(pipe->dcd_handle, ("Pipe was not allocated\n"));
    KASSERT(!ep->pipe, ("Pipe already enabled\n"));
    
    switch (pipe->type)
    {
    case PIPE_BULK:
        type = TD243FC_BULK_EP;
        break;
    case PIPE_INTR:
        type = TD243FC_INTR_EP;
        break;
    case PIPE_ISOC:
        type = TD243FC_ISOC_EP;
        break;
    default:
        rc = JEINVAL;
        goto Exit;
    }

    ep->pipe = pipe;

    /* alloc buffer */
    rc = td243fc_mem_alloc(sc, ep->buf_size, &ep->xaddr);
    KASSERT(!rc, ("Can't allocate pipe(%p) xbuffer, size=%d\n", pipe,
        ep->buf_size));

    rc = td243fc_mem_alloc(sc, ep->buf_size, &ep->yaddr);
    KASSERT(!rc, ("Can't allocate pipe(%p) ybuffer, size=%d\n", pipe,
        ep->buf_size));

    DBG_I(DSLAVE_DCD, ("DCD: dcd_enable_ep pipe=%p, buf_size=%d, "
        "max_packet=%d XY(%04x,%04x)\n", pipe, ep->buf_size, 
        pipe->max_pkt_size_full, ep->xaddr, ep->yaddr));

    /* Clear TOGGLE */
    TOGGLE_IF_SET(TD243FC_EP_TOGGLE_REG, TD243FC_EP_BIT_INDEX(ep_n));

    /* Init ep control registers */

    WRITE4(TD243FC_EPN_CONFIG_CONTROL_REG(ep_n/2, is_in), 
        BFSET4(TD243FC_FORMAT, type) |
        BFSET4(TD243FC_MAXPKTSIZ, pipe->max_pkt_size_full));

    WRITE4(TD243FC_EPN_XY_BUFFER_ADDRESS_REG(ep_n/2, is_in),
        BFSET4(TD243FC_XBSA, TD243FC_GET_IADDR(sc->ep[ep_n].xaddr)) | 
        BFSET4(TD243FC_YBSA, TD243FC_GET_IADDR(sc->ep[ep_n].yaddr)));

    ENABLE_EP(ep_n/2, is_in);
    pipe->status = PIPE_ENABLED;

Exit: 
    return rc;
}

/* Disable an endpoint */
static jresult_t dcd_disable_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jint_t ep_n = (jint_t)(pipe->dcd_handle);
    td243fc_rev2_ep_t *ep = NULL;
    jbool_t is_in = FALSE;

    KASSERT(pipe, ("Pipe is NULL\n"));
    KASSERT(pipe->dcd_handle, ("Pipe was not allocated\n"));

    /* HSU Addition */
    ep = &sc->ep[ep_n];
    /* First set value in ep, only then try to ASSERT on ep->pipe */
    KASSERT(ep->pipe, ("Pipe was not enabled\n"));
    /* End of HSU Addition */

    is_in = EP_IS_IN(ep_n);

    DBG_I(DSLAVE_DCD, ("DCD: dcd_disable_ep pipe=%p, buf_size=%d\n", pipe, 
        ep->buf_size));

    WRITE4(TD243FC_EPN_CONFIG_CONTROL_REG(ep_n/2, is_in), 0);

    pipe->status = PIPE_DISABLED;

    td243fc_mem_free(sc, ep->buf_size * 2);

    ep->pipe = NULL;

    return 0;
}

/* Cancel all pending IO requests */

static void cancel_active_request(td243fc_rev2_softc_t *sc, jint_t ep_num)
{
    request_t *req = sc->ep[ep_num].req;
    
    if (!req)
        return;

    req->status = REQUEST_CANCELLED;
    /* ABORT DMA TRANSFER */
    WRITE4(TD243FC_DMA_EP_CHANNEL_CLEAR_REG, TD243FC_EP_BIT_INDEX(ep_num));
    
    /* unset the ep */
    WRITE4(TD243FC_EP_READY_CLEAR_REG, TD243FC_EP_BIT_INDEX(ep_num));

    req_finish(sc, ep_num, req, REQUEST_CANCELLED);
}

static void cancel_queue(td243fc_rev2_softc_t *sc, jint_t ep_n)
{
    request_t *request;
    td243fc_rev2_ep_t * ep = &sc->ep[ep_n];

    if (!ep->pipe)
        return; 

    while (!STAILQ_EMPTY(&ep->pipe->req_queue))
    {
        request = STAILQ_FIRST(&ep->pipe->req_queue);

        request->status = REQUEST_CANCELLED;

        /* Call completion callback */
        if (request->complete)
            request->complete(request);
        
        STAILQ_REMOVE_HEAD(&ep->pipe->req_queue, next_request);
    }

    ep->pipe->status = PIPE_ENABLED;
}

static jresult_t dcd_abort_ep(jdevice_t dev, pipe_desc_t *pipe)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jint_t ep_n = 0; 

    DBG_V(DSLAVE_DCD, ("DCD: dcd_abort_ep, addr=%02x\n", pipe?pipe->address:0));

    if (pipe && !STAILQ_EMPTY(&pipe->req_queue))
        pipe->status = PIPE_ABORTING;

    if (pipe)
        ep_n = (jint_t) pipe->dcd_handle;
    else if (sc->ep[1].req)
        ep_n = 1; /* There is active IN transaction */
    else
        ep_n = 0; /* There is active OUT transaction or SETUP request */

    cancel_active_request(sc, ep_n);

    cancel_queue(sc, ep_n);

    return 0;
}

/* Queue an IO request */
/* NOTES: 
 * - EP0 is automaticly queued by the DCD and passed to core_handle_ep0() 
 * - The DCD handles SET_ADDRESS and CLEAR_FEATURE(EP_STALL) itself */
static jresult_t dcd_send_io_request(jdevice_t dev, pipe_desc_t *pipe, 
    request_t *core_req)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jint_t ep_n = 0;

    if (pipe)
        ep_n = (jint_t)(pipe->dcd_handle);
    else
        ep_n = (core_req->direction == DIRECTION_IN) ? 1 : 0;

    return start_transfer(sc, ep_n, core_req);
}

static jresult_t dcd_alloc_pipe(jdevice_t dev, pipe_desc_t *pipe, 
    juint8_t config_index, juint8_t iface_index, juint8_t alt_index)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    td243fc_rev2_ep_t *ep;
    jint_t i;
    jint_t is_in = (pipe->direction == DIRECTION_IN);

    /* we use zero value for unallocated ep */
    iface_index++;
    alt_index++;

    DBG_V(DSLAVE_DCD, ("DCD: Finding a matching for %p iface = %d alt = %d\n", 
        pipe, iface_index, alt_index));

    for (i = 2 + is_in, ep = &sc->ep[2]; i < TD243FC_EP_NUM * 2; i+=2)
    {
        ep = &sc->ep[i];
        if (!ep->iface || (ep->iface == iface_index && ep->alt != alt_index))
            break;
    }

    if (i < TD243FC_EP_NUM)
    {
        ep->iface = iface_index;
        ep->alt   = alt_index;
        ep->global_max_packet = MAX(ep->global_max_packet, 
            pipe->max_pkt_size_full);

        pipe->address = (i/2) | (is_in ? USB_DIR_IN : 0);
        pipe->dcd_handle = (void *)i;

        DBG_V(DSLAVE_DCD, ("DCD: Found ep address=0x%02x(%d) max_packet=%d\n", 
            pipe->address, i, pipe->max_pkt_size_full));
        return 0;
    }

    return JEINVAL;
}

static jresult_t dcd_free_pipe(jdevice_t dev, pipe_desc_t *pipe)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jint_t i = (jint_t)(pipe->dcd_handle);

    DBG_V(DSLAVE_DCD, ("DCD: dcd_free_pipe address=0x%02x(%d)\n", pipe->address,
        i));

    sc->ep[i].iface = 0;
    sc->ep[i].alt   = 0;
    sc->ep[i].global_max_packet = 0;
    pipe->dcd_handle = NULL;

    return 0;
}

/* if ep is stalled returns 1 else returns 0 */
static jresult_t dcd_get_ep_status(jdevice_t dev, juint8_t *status, 
    juint8_t ep_num)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    juint8_t addr = ep_num & 0x7F;
    juint8_t is_in = ep_num & 0x80;

    DBG_V(DSLAVE_DCD, ("DCD: dcd_get_ep_status, ep_num=%.2x\n", ep_num));

    if (READ4(TD243FC_EPN_CONFIG_CONTROL_REG(addr, is_in)) & TD243FC_STL)
        *status = 1;
    else
        *status = 0;

    return 0;
}

static jresult_t dcd_set_address_state(jdevice_t dev, juint16_t address)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);

    DBG_V(DSLAVE_DCD, ("DCD: dcd_set_address_state, address=%d\n", address));

    sc->pendig_address = TRUE;
    sc->address = address;

    return 0;
}

static jresult_t dcd_handle_feature(jdevice_t dev, juint16_t feature, 
    juint16_t index, juint8_t set)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    juint8_t pipe_address = 0;
    jresult_t rc = 0;

    DBG_V(DSLAVE_DCD, ("DCD: handle feature (%d) index (%d) set (%d)\n", 
        feature, index, set));

    pipe_address = ((index & ~USB_DIR_IN)*2) + ((index & USB_DIR_IN)?1:0); 

    /* changing feature */
    switch (feature)
    {
    case USB_ENDPOINT_HALT:
        /* setting stall */
        dcd_stall_ep(sc->dev, sc->ep[pipe_address].pipe , set);
        break;

    default:
        rc = JENOTSUP;
    }

    return rc;
}

static jresult_t calculate_ep_bufs(td243fc_rev2_softc_t *sc)
{
    jint_t i;
    juint16_t size = 0, free_size = GET_FREE_SIZE(sc);
    juint16_t n;

    /* calculate ep buffers */
    for (i = 2; i < TD243FC_EP_NUM; i++)
        size += sc->ep[i].global_max_packet;

    DBG_V(DSLAVE_DCD, ("DCD: td243fc_resize_ep_buf need=%d, free=%d\n", size,
        free_size));

    n = free_size / size;

    /* double-buffering */
    if (n < 2)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Not enough space in data memory for all "
            "asked sizes free = %d, need = %d\n", free_size, size));
        return JENOMEM;
    }
    /* even number */
    n /= 2; 
            
    for (i = 2; i < TD243FC_EP_NUM; i++)
        sc->ep[i].buf_size = n * sc->ep[i].global_max_packet;

    return 0;
}

static jresult_t dcd_set_config_state(jdevice_t dev, juint8_t config)
{
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    
    DBG_V(DSLAVE_DCD, ("DCD: dcd_set_config_state %d\n", config));

    if (config)
        return calculate_ep_bufs(sc);

    return 0;
}

static juint8_t dcd_get_max_packet0(jdevice_t dev)
{
    return TD243FC_EP0_MAX_PACKET_SIZE;
}

static jresult_t td243fc_rev2_probe(jdevice_t dev)
{
    DBG_I(DSLAVE_DCD, ("DCD: td243dc_probe\n"));
    return 0;
}

static jresult_t td243fc_rev2_detach(jdevice_t dev)
{ 
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jbus_h bus = j_device_get_bus(dev);

    DBG_I(DSLAVE_DCD, ("DCD: Detaching controller: %s\n", 
        j_device_get_nameunit(dev)));

    /* Cleanup DCD context and unregister DCD */
    if (sc->core_ctx)
        core_unregister_dcd(sc->core_ctx);

    if (sc->irq_res)
        jbus_release_resource(bus, sc->irq_res);

    if (sc->mem_res)
        jbus_release_resource(bus, sc->mem_res);

    if (sc->dma_buf.cookie)
        jdma_free(sc->dma_buf.cookie);

    if (sc->sc_lock)
        jspinlock_uninit(sc->sc_lock);

    DBG_V(DSLAVE_DCD, ("DCD: Controller detached\n"));
    return 0;
}

static dcd_hw_init(td243fc_rev2_softc_t *sc)
{
    WRITE4(TD243FC_RESET_CONTROL_REG, 
        TD243FC_RESET_FUNCTION_SIE | TD243FC_RESET_FUNCTION_CONTROLLER);

    /* force USB core work as device bit1:0 = 10b, and XCVR mode 
     * bit 7:6 = 11b */ 
    WRITE4(TD243FC_HARDWARE_MODE_REG, BFSET4(TD243FC_HF, TD243FC_DEVICE) |
        BFSET4(TD243FC_OTGXCVR, TD243FC_SE_SE) |
        BFSET4(TD243FC_HOSTXCVR, TD243FC_SE_SE));

    /* Enable function controller clock */
    SET4(TD243FC_CLOCK_CONTROL_REG, TD243FC_FCCLK);
}

static jresult_t td243fc_rev2_attach(jdevice_t dev)
{                                               
    td243fc_rev2_softc_t *sc = (td243fc_rev2_softc_t *)j_device_get_softc(dev);
    jbus_h bus = j_device_get_bus(dev);
    jresult_t rc;

    DBG_I(DSLAVE_DCD, ("DCD: Attaching controller: %s\n", 
        j_device_get_nameunit(dev)));

    /* initializing device controller data */
    sc->dev = dev;

    /* Due to DMA limitation - it's operating not with transfer size but with 
     * "mega buffers" size we have to define all out requests as 64 bytes
     * alligned for ep0*/
    rc = jdma_alloc(TD243FC_EP0_MAX_PACKET_SIZE, 0, &sc->dma_buf.vaddr, 
        &sc->dma_buf.dma_addr, 0, &sc->dma_buf.cookie);
    KASSERT(!rc, ("Can't alloc dma buffer for EP0 OUT\n"));

    sc->dma_buf.buffer_size = TD243FC_EP0_MAX_PACKET_SIZE;

    sc->mem_res = j_bus_alloc_resource(bus, SYS_RES_MEMORY, 0);
    if (!sc->mem_res)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Error allocating memory resource\n"));
        rc = JEINVAL;
        goto Error;
    }

    sc->irq_res = j_bus_alloc_resource(bus, SYS_RES_IRQ, 0);
    if (!sc->irq_res)
    {
        DBG_E(DSLAVE_DCD, ("DCD: Error allocating interrupt resource\n"));
        rc = JEINVAL;
        goto Error;
    }

    sc->sc_lock = NULL;
    jspinlock_init(&sc->sc_lock);

    dcd_hw_init(sc);

    rc = core_register_dcd(dev, &dcd_ops, &sc->core_ctx);
    if (rc)
    {
        DBG_E(DSLAVE_DCD, ("DCD: register_dcd failed, rc=%d\n", rc));
        goto Error;
    }

    DBG_V(DSLAVE_DCD, ("DCD: TD243 attached, controller revision=%.2x\n",
        BFGET4(TD243FC_FCREV, READ4(TD243FC_HARDWARE_MODE_REG))));

    return 0;

Error:
    td243fc_rev2_detach(dev);

    return rc;
}

static jresult_t td243fc_rev2_suspend(jdevice_t dev)
{
    return 0;
}

static jresult_t td243fc_rev2_resume(jdevice_t dev)
{
    return 0;
}

static jresult_t td243fc_rev2_ioctl(jdevice_t self, drv_ioctl_t ioctl, 
    void *arg)
{
    DBG_E(DSLAVE_DCD, ("DCD: Unsupported IOCTL %x\n", ioctl));
    return JEINVAL;
}

DRIVER_CONTROLLER_DECLARE(td243fc_rev2, sizeof(td243fc_rev2_softc_t))


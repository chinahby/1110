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

/***************************************************************************
 *      CDC EEM application
 *      A wrapper to the CDC EEM Class Driver.
 ***************************************************************************/
#include <uw_host_cdc_eem.h>
#include <jos.h>
#include <port_net_dev.h>

#define MAX_RX_PACKETS 10

enum eem_states {
    EEM_DISCONNECTED = 0,
    EEM_DISABLED,
    EEM_SUSPENDED,
    EEM_ENABLED
};

typedef struct {
    uwh_cdc_dev_h dev_ctx;
    netdev_h netdev_ctx;
    enum eem_states state;
    jtask_h task_handle;
    jtask_h task_timeout_handle;
    jbool_t polling_dev;
    jbool_t complete_hint;
#ifdef JDEBUG
    juint_t active_tx;
#endif
} eem_dev_t;

typedef struct {
    net_complete_cb_t cb;
    void *net_priv;
    jdma_handle dma_handle;
} eem_priv;

/* Application callbackes*/
static jresult_t eem_attach(uwh_cdc_dev_h dev_ctx, uwh_cdc_app_h *app_ctx);
static void eem_detach(uwh_cdc_app_h app_ctx);
static void echo_cb(uwh_cdc_app_h app_ctx, void *buffer, juint16_t size, 
    jbool_t response);
static void suspend_hint_cb(uwh_cdc_app_h app_ctx);
static void response_hint_cb(uwh_cdc_app_h app_ctx, juint16_t interval, 
    jbool_t complete);

/* Application's private functions */
static jresult_t eem_tx(app_ctx_h app_ctx, void *membuf, juint32_t size, 
        net_complete_cb_t cb, void *net_priv);
static void eem_abort_tx(app_ctx_h app_ctx);
static void eem_suspend(app_ctx_h app_ctx);
static jresult_t eem_resume(app_ctx_h app_ctx);
static void transfer_task(void *arg);
static void echo(eem_dev_t *eem_dev, jbool_t response);
static jresult_t eem_start_rx_xfers(eem_dev_t *eem_dev);

static uwh_eem_callbacks_t eem_handle_cb = {
    eem_attach,
    eem_detach,
    echo_cb,
    suspend_hint_cb,
    response_hint_cb
};

/*************************************************************************/
/*                   Application init and uninit                         */
/*************************************************************************/
void cdc_eem_uninit(void)
{
    DBG_X(DHOST_CDC_EEM, ("cdc_eem_uninit: Entered\n"));

    uwh_eem_unregister();
}

int cdc_eem_init(void)
{
    jresult_t rc;

    DBG_X(DHOST_CDC_EEM, ("cdc_eem_init: Entered\n"));

    /* Register to the EEM driver */
    rc = uwh_eem_register(&eem_handle_cb);
    DBG_IF(rc)
    {
        DBG_E(DHOST_CDC_EEM, ("cdc_eem_init: Failed to register EEM driver "
            "[%d]\n", rc));
    }

    return rc;
}

static net_app_ops_t net_ops = {
    eem_tx,
    eem_suspend,
    eem_resume,
    eem_abort_tx
};

/*************************************************************************/
/*                   Implementation of EEM driver callbacks              */
/*************************************************************************/
static jresult_t eem_attach(uwh_cdc_dev_h dev_ctx, uwh_cdc_app_h *app_ctx)
{
    jresult_t rc = 0;
    struct net_macaddr mac_address;
    char arr[6] = {0x0, 0x50, 0xbf, 0xda, 0xb9, 0xf0};
    eem_dev_t *ctx;
    
    ctx = jmalloc(sizeof(eem_dev_t), M_ZERO);
    if (!ctx)
    {
        DBG_E(DHOST_CDC_EEM, ("eem_attach: Couldn't alloc ctx\n"));
        return JENOMEM;
    }

    *app_ctx = (uwh_cdc_app_h)ctx;
    ctx->dev_ctx = dev_ctx;
   
    jtask_init(&ctx->task_handle, TASK_CLASS_DRIVER);
    jtask_init(&ctx->task_timeout_handle, TASK_CLASS_DRIVER);

    /* TODO: Makeup mac address */
    j_memcpy(mac_address.bytes, arr, 6);

    jsafe_leave();
    /* Initialize upper networking layer */
    rc = net_attach(ctx, &net_ops, (struct net_macaddr*)&mac_address, 
        &ctx->netdev_ctx);
    jsafe_enter();

    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("eem_attach: net_attach() failed\n"));
        goto Exit;
    }

    rc = uwh_eem_enable(dev_ctx);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("eem_attach: eem_enable failed\n"));
        goto Exit;
    }
    
    ctx->state = EEM_ENABLED;
    
    jsafe_leave();
    net_enable(ctx->netdev_ctx);
    jsafe_enter();
     
    rc = jtask_schedule(ctx->task_handle, 0, transfer_task, ctx);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM,("Attach iccd: Error starting task\n"));
        goto Exit;
    }

Exit:
    if (rc && ctx)
    {
        if (ctx->task_timeout_handle)
            jtask_uninit(ctx->task_timeout_handle);

        if (ctx->task_handle)
        {
            jtask_stop(ctx->task_handle);
            jtask_uninit(ctx->task_handle);
        }

        ctx->state = EEM_DISCONNECTED;
        jfree(ctx);
    }

    return rc;
}

static void eem_detach(uwh_cdc_app_h app_ctx)
{
    eem_dev_t *ctx = (eem_dev_t *)app_ctx;

    DBG_V(DHOST_CDC_EEM, ("eem_detach\n"));

    jtask_stop(ctx->task_handle);
    jtask_uninit(ctx->task_handle);
    jtask_uninit(ctx->task_timeout_handle);

    jsafe_leave();
    net_disable(ctx->netdev_ctx);
    net_detach(ctx->netdev_ctx);
    jsafe_enter();

    uwh_eem_disable(ctx->dev_ctx);
    jfree(ctx);
}

static void echo_cb(uwh_cdc_app_h app_ctx, void *buffer, juint16_t size, 
    jbool_t response)
{
    DBG_X(DHOST_CDC_EEM, ("echo_cb: echo was received from the device "
        "response: %d\n", response));
}

static void suspend_hint_cb(uwh_cdc_app_h app_ctx)
{
    eem_dev_t *ctx = (eem_dev_t *)app_ctx;
    /* The EEM device reports it is safe to suspend */
    DBG_X(DHOST_CDC_EEM, ("suspend_hint_cb: suspend hint was received from"
        "the device\n"));

    KASSERT(ctx, ("Null context!!!\n"));
    /* Check active xfers */
    DBG_IF(ctx->active_tx)
    {
        DBG_W(DHOST_CDC_EEM, ("Received suspend_hint_cb while still %d active"
            " TX\n", ctx->active_tx));
    }

    if (uwh_eem_suspend(ctx->dev_ctx))
    {
        /* Failed suspending device */
        DBG_E(DHOST_CDC_EEM, ("Failed suspending device\n"));
        return;
    }
    ctx->state = EEM_SUSPENDED;
}

static void response_complete(void *arg)
{
    jresult_t rc;
    eem_dev_t *ctx = (eem_dev_t *)arg;
    DECLARE_FNAME("response_complete task");

    DBG_V(DHOST_CDC_EEM, ("%s: entered\n", fname));

    /* Timeout for ResponseCompleteHint has expired - send Tickle cmd to the
     * device */
    if (ctx->complete_hint)
    {
        rc = uwh_eem_tickle(ctx->dev_ctx);
        DBG_IF(rc)
        {
            DBG_E(DHOST_CDC_EEM, ("%s: Sending Tickle failed [%d]\n", fname, 
                rc));
        }
    }
    /* Restart polling the device for data */
    rc = eem_start_rx_xfers(ctx);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("%s: Failed starting RX xfers %d\n", fname, rc));
}

static void response_hint_cb(uwh_cdc_app_h app_ctx, juint16_t interval, 
    jbool_t complete)
{
    eem_dev_t *ctx = (eem_dev_t *)app_ctx;
    jresult_t rc;

    /* Suspend polling the device until:
     * 1.next tx
     * 2. timeout expires (if complete is TRUE) */
    DBG_X(DHOST_CDC_EEM, ("response_hint_cb: response hint was received from "
        "the device interval %d complete %d\n", interval, complete));

    uwh_eem_stop_rx(ctx->dev_ctx);
    ctx->polling_dev = 0;

    ctx->complete_hint = complete;

    /* ResponseCompleteHint - need to tickle the device when timeout 
     * expires and renew data polling */
    rc = jtask_schedule(ctx->task_timeout_handle, interval, response_complete,
        ctx);
    DBG_IF(rc)
    {
        DBG_E(DHOST_CDC_EEM, ("Failed scheduling ResponseComplete task - "
            "%d, discarding Hint\n", rc));
        /* RX polling will seize until next TX */
    }
}

/*************************************************************************/
/*                          Private functions                            */
/*************************************************************************/
static void get_data_cb(uwh_cdc_app_h app_ctx, jresult_t status, void *buf,
    juint32_t size, void *priv)
{
    eem_dev_t *ctx = (eem_dev_t *)app_ctx;
    DBG_V(DHOST_CDC_EEM, ("get_data_cb: Entered - status %d, data len %lu, "
        "current %x\n", status, size, jget_context()));

    if (!status)
    {
        jsafe_leave();
        net_rx_complete(ctx->netdev_ctx, buf, size, NULL, NULL);
        jsafe_enter();
    }
}

#ifdef CONFIG_EEM_BUNDLE_PACKET
/*****************************************************************************
 * This implementation is for demonstrations only and meant to show how to
 * use the uwh_eem_send_data_bundle API.
 * The logic behind a decision when to use this API is application dependent
 * And should take into consideration the EEM packet size as well as expected
 * response time (if packets are accumulated rather then sent at arrival).
 * FIXME - When implementing such a logic bare in mind that accumulating
 * packets and additional info should be at a device level (per device), not
 * global as implemented is this example.
 * **************************************************************************/
#define BUNDLE_SIZE 2
#define CONSECUTIVE_SINGLE_PACKETS 2
static juint_t counter = 0;
static eem_priv **bundle_priv = NULL;
static juint8_t **bundle_buf = NULL;
static juint32_t *bundle_size = NULL;
static jbool_t bundle = 0;

static void send_data_bundle_cb(uwh_cdc_app_h app_ctx, jresult_t status, 
    void **buffers, juint32_t *sizes, juint16_t num_data_pkts, void *priv)
{
    juint16_t i;

    DBG_V(DHOST_CDC_EEM, ("send_data_bundle_cb: Entered\n"));

    for (i = 0; i < num_data_pkts; i++)
    {
        eem_priv *app_priv = ((eem_priv **)priv)[i];

        if (app_priv && app_priv->cb)
        {
            jsafe_leave();
            app_priv->cb(((eem_dev_t *)app_ctx)->netdev_ctx, app_priv->net_priv,
                buffers[i]);
            jsafe_enter();
        }
        
        jfree(app_priv);
    }

    jfree(buffers);
    jfree(sizes);
    jfree(priv);
}
#endif

static void send_data_cb(uwh_cdc_app_h app_ctx, jresult_t status, void *buf,
    juint32_t lengh, void *priv)
{
    eem_priv *app_priv = (eem_priv *)priv;

    /* Called from class driver upon completion of send_data */
    DBG_V(DHOST_CDC_EEM, ("send_data_cb: Entered - status %d\n", status));

    if (app_priv->cb)
    {
        jsafe_leave();
        app_priv->cb(((eem_dev_t *)app_ctx)->netdev_ctx, app_priv->net_priv, 
            buf);
        jsafe_enter();
    }

    jfree(priv);
}

/*************************************************************************/
/*                 Implementation of net_dev ops                         */
/*************************************************************************/
static jresult_t eem_tx(app_ctx_h app_ctx, void *buf, juint32_t size, 
    net_complete_cb_t cb, void *net_priv)
{
    jresult_t rc;
    eem_priv *app_priv;
    eem_dev_t *ctx = (eem_dev_t *)app_ctx;
    

    DBG_V(DHOST_CDC_EEM, ("eem_tx: Entered, buf %p size %d]\n", buf, size));

    if (ctx->state == EEM_SUSPENDED)
    {
        rc = uwh_eem_resume(ctx->dev_ctx);
        if (rc)
        {
            DBG_E(DHOST_CDC_EEM, ("eem_tx: Failed to resume [%d]\n"));
            return rc;
        }
        ctx->state = EEM_ENABLED;
    }

    app_priv = jmalloc(sizeof(eem_priv), M_ZERO);
    if (!app_priv)
    {
        DBG_E(DHOST_CDC_EEM, ("eem_tx: Failed allocating eem_priv\n"));
        return JENOMEM;
    }

    app_priv->cb = cb;
    app_priv->net_priv = net_priv;

#ifdef CONFIG_EEM_BUNDLE_PACKET
    counter++;

    if (counter > CONSECUTIVE_SINGLE_PACKETS)
    {
        if (counter == CONSECUTIVE_SINGLE_PACKETS + 1)
        {
            bundle_priv = 
                (eem_priv **)jmalloc(sizeof(eem_priv *) * BUNDLE_SIZE, M_ZERO);
            bundle_size = 
                (juint32_t *)jmalloc(sizeof(juint32_t) * BUNDLE_SIZE, M_ZERO);
            bundle_buf = 
                (juint8_t **)jmalloc(sizeof(juint8_t *) * BUNDLE_SIZE, M_ZERO);
        }
        
        /* save packet to be sent as a bundle with the next TX */
        bundle_size[counter - CONSECUTIVE_SINGLE_PACKETS - 1] = size;
        bundle_buf[counter - CONSECUTIVE_SINGLE_PACKETS - 1] = buf;
        bundle_priv[counter - CONSECUTIVE_SINGLE_PACKETS - 1] = app_priv;

        if (counter == BUNDLE_SIZE + CONSECUTIVE_SINGLE_PACKETS)
            bundle = 1;
        else
            return 0;
    }

    if (bundle)
    {
        rc = uwh_eem_send_data_bundle(ctx->dev_ctx, (void *)bundle_buf,
            bundle_size, BUNDLE_SIZE, send_data_bundle_cb, bundle_priv);
        if (rc)
        {
            juint_t i;
            DBG_E(DHOST_CDC_EEM, ("eem_tx: eem_send_data failed [%d]\n", rc));
            for (i = 0; i < BUNDLE_SIZE; i++)
            {
                jfree(bundle_priv[i]);
                bundle_priv[i] = NULL;
            }
        }
        bundle = 0;
        counter = 0;
        bundle_priv = NULL;
        bundle_size = NULL;
        bundle_buf = NULL;
    }
    else
#endif
    {
        rc = uwh_eem_send_data_packet(ctx->dev_ctx, buf, size, 0, 
            send_data_cb, app_priv);
        if (rc)
        {
            DBG_E(DHOST_CDC_EEM, ("eem_tx: eem_send_data failed [%d]\n", rc));
            jfree(app_priv);
        }
    }
    if (!ctx->polling_dev)
    {
        jtask_stop(ctx->task_timeout_handle);
        rc = eem_start_rx_xfers(ctx);
    }

#ifdef JDEBUG
    ctx->active_tx++;
#endif

    return rc;
}

static void eem_abort_tx(app_ctx_h app_ctx)
{
    eem_dev_t *eem_dev = (eem_dev_t *)app_ctx;

    DBG_V(DHOST_CDC_EEM, ("eem_abort_tx: Entered\n"));
    
    uwh_eem_disable(eem_dev->dev_ctx);
    uwh_eem_enable(eem_dev->dev_ctx);
}
    
static void eem_suspend(app_ctx_h app_ctx)
{
    eem_dev_t *eem_dev = (eem_dev_t *)app_ctx;
    jresult_t rc;

    jsafe_enter();

    rc = uwh_eem_suspend(eem_dev->dev_ctx);
#ifdef JDEBUG
    if (rc)
        DBG_E(DHOST_CDC_EEM, ("eem_suspend: suspend failed (%d)\n", rc));
#endif

    jsafe_leave();
}

static jresult_t eem_start_rx_xfers(eem_dev_t *eem_dev)
{
    jresult_t rc;

    eem_dev->polling_dev = 1;

    rc = uwh_eem_start_rx(eem_dev->dev_ctx, get_data_cb, NULL, MAX_RX_PACKETS);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("eem_start_rx_xfers: read data failed (%d)\n",
            rc));
        return rc;
    }

    return 0;
}

static jresult_t eem_resume(app_ctx_h app_ctx)
{
    eem_dev_t *eem_dev = (eem_dev_t *)app_ctx;
    jresult_t rc;
    DECLARE_FNAME("eem_resume");

    jsafe_enter();

    rc = uwh_eem_resume(eem_dev->dev_ctx);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s:: resume failed (%d)\n", fname, rc));
        goto Exit;
    }

    rc = eem_start_rx_xfers(eem_dev);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("%s: Failed starting RX xfers %d\n", fname, rc));

Exit:
    jsafe_leave();
    return rc;
}

/*************************************************************************/
/*                             Help functions                            */
/*************************************************************************/
static void transfer_task(void *arg)
{
    eem_dev_t *eem_dev = (eem_dev_t *)arg;
    
    uwh_eem_start_rx(eem_dev->dev_ctx, get_data_cb, NULL, MAX_RX_PACKETS);

    echo(eem_dev, 0);
}

static void echo_complete(uwh_cdc_app_h app_ctx, jresult_t status, void *buffer,
    juint32_t size, void *priv)
{
    DBG_V(DHOST_CDC_EEM, ("echo_complete: entered\n"));   
  
    jfree(priv);
}

static void echo(eem_dev_t *eem_dev, jbool_t response)
{
    jresult_t rc;
    juint16_t size;
    juint8_t *echo_buf;

    DBG_V(DHOST_CDC_EEM, ("echo: Entered, response %d\n", response));

    size = 3;

    echo_buf = jmalloc(size, 0);
    if (!echo_buf)
    {
        DBG_E(DHOST_CDC_EEM, ("echo: Couldn't alloc trans_buffer\n"));
        return;
    }

    if (!response)
        j_memcpy(echo_buf, "abc", size);

    rc = uwh_eem_echo(eem_dev->dev_ctx, echo_buf, size, echo_complete, 
        echo_buf, response);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("echo: error sending echo %d\n", rc));
        jfree(echo_buf);
        echo_buf = NULL;
        return;
    }
}

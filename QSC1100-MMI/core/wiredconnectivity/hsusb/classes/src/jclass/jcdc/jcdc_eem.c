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

/********************************************
 * CDC EEM Class Driver Implementation
 ********************************************/

#include "jcdc_general.h"
#include <uw_host_cdc_eem.h>

/* EEM-Specific USB Codes */
#define EEM_IFACE_SUBCLASS_CODE 0x0C
#define MAX_READ_SIZE 1520
#define EEM_IVAL_BM 0x7ff
#define EEM_HEADER_LEN 2
#define EEM_ECHO_MAX_LEN 2047
#define EEM_CRC_LEN 4
#define EEM_HEADER_TYPE_COMMAND 0x8000
#define EEM_COMMAND_TYPE_ECHO 0x87FF
#define EEM_COMMAND_TYPE_ECHO_RESPONSE 0x8FFF
#define EEM_COMMAND_TYPE_SUSPEND_HINT 0x9000 
#define EEM_COMMAND_TYPE_RESPONSE_HINT 0x9FFF
#define EEM_COMMAND_TYPE_RESPONSE_HINT_COMPLETE 0xA7FF
#define EEM_HEADER_LENGTH_MASK 0x3FFF 

/* Application Callbacks Structure */
static uwh_eem_callbacks_t *app_callback_h = NULL;

/* EEM software context; includes general CDC context. */
typedef struct jcdc_eem_softc {
    jcdc_dev_softc_t cdc_sc;
} jcdc_eem_softc_t;

/* EEM request private structure */
typedef struct {
    void *handle;
    uwh_eem_data_cb_t cb;
    uwh_eem_data_bundle_cb_t bundle_cb;
    void *app_priv;
    void **orig_buffer;
    juint32_t *orig_size;
    juint32_t single_orig_size;
    void *single_orig_buffer;
    juint16_t packet_num;
} eem_req_priv_t;

static void send_data_cb(uwh_cdc_app_h app_ctx, void *priv, void *buffer, 
    juint32_t length, jresult_t status);

static jresult_t eem_read_data_packet(uwh_cdc_dev_h dev_ctx, 
    uwh_eem_data_cb_t cb, void *app_priv);

/**
 * Function name:  uwh_eem_abort_tx
 * Description:    Discards all outgoing EEM packets and bundles.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_abort_tx(uwh_cdc_dev_h dev_ctx)
{
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    DBG_V(DHOST_CDC_EEM, ("uwh_eem_abort_tx: Entered\n"));

    if (sc->cdc_sc.bulk_pipe_out)
        usbd_abort_pipe(sc->cdc_sc.bulk_pipe_out);

    return 0;
}

/**
 * Function name:  uwh_eem_stop_rx
 * Description:    Disables receiving data from the device.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_stop_rx(uwh_cdc_dev_h dev_ctx)
{
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    DBG_V(DHOST_CDC_EEM, ("uwh_eem_stop_rx: Entered\n"));

    if (sc->cdc_sc.bulk_pipe_in)
        usbd_abort_pipe(sc->cdc_sc.bulk_pipe_in);

    return 0;
}

static jresult_t eem_tx(uwh_cdc_app_h app_ctx, void *buffer, juint32_t size, 
    uwh_eem_data_cb_t cb, void *app_priv, juint16_t header, juint32_t crc)
{
    jresult_t rc = 0;
    uwh_cdc_app_h ctx = app_ctx;
    juint8_t *buf;
    eem_req_priv_t *eem_priv = NULL;      
    juint32_t new_size;
    jdma_handle handle = NULL;
    DECLARE_FNAME("eem_tx");

    DBG_V(DHOST_CDC_EEM, ("%s: Entered\n", fname));

    /* new_size contains the total size that needs to be sent, the buffer size
     * and the header size, and if it is a data packet the crc len */
    new_size = size + EEM_HEADER_LEN;

    if (crc)
        new_size += EEM_CRC_LEN;

    rc = jdma_alloc(new_size, 0, NULL, NULL, 0, &handle);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Couldn't alloc trans_buffer\n", fname));
        goto Error;
    }

    eem_priv = (eem_req_priv_t *)jmalloc(sizeof(eem_req_priv_t), M_ZERO);
    if (!eem_priv)
    {
        rc = JENOMEM;
        goto Error;
    }

    eem_priv->cb = cb;
    eem_priv->app_priv = app_priv;
    eem_priv->handle = handle;
    eem_priv->single_orig_buffer = buffer;
    eem_priv->single_orig_size = size;
    eem_priv->packet_num = 1;

    buf = jdma_virt_addr(handle, 0);
    /* Copy the header */
    j_memcpy(buf, &header, EEM_HEADER_LEN);

    /* Copy the data after the header */
    j_memcpy(buf + EEM_HEADER_LEN, buffer, size);

    /* Copy the crc after the data and the header */
    if (crc)
        j_memcpy(buf + EEM_HEADER_LEN + size, &crc, EEM_CRC_LEN);

    rc = cdc_data_send(ctx, buf, new_size, send_data_cb, ctx, eem_priv);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: cdc_send_data failed [%d]\n", fname, rc));
        goto Error;
    }

    return 0;

Error:

    if (handle)
        jdma_free(handle);

    if (eem_priv)
        jfree(eem_priv);

    return rc;
}

static void send_data_cb(uwh_cdc_app_h app_ctx, void *priv, void *buffer, 
    juint32_t length, jresult_t status)
{
    eem_req_priv_t *eem_priv = (eem_req_priv_t *)priv;
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)app_ctx;

    /* Called from class driver upon completion of send_data */
    DBG_V(DHOST_CDC_EEM, ("send_data_cb: Entered - status %d, data len %lu,\n",
        status, length));

    if (eem_priv->bundle_cb)
    {
        eem_priv->bundle_cb(sc->cdc_sc.app_ctx, status, eem_priv->orig_buffer,
            eem_priv->orig_size, eem_priv->packet_num, eem_priv->app_priv);
    }
    else if (eem_priv->cb)
    {
        eem_priv->cb(sc->cdc_sc.app_ctx, status, eem_priv->single_orig_buffer,
            eem_priv->single_orig_size, eem_priv->app_priv);
    }

    jdma_free(eem_priv->handle);
    jfree(eem_priv);
}

static void get_data_cb(uwh_cdc_app_h app_ctx, void *priv, void *buffer, 
    juint32_t length, jresult_t status)
{
    eem_req_priv_t *eem_priv = (eem_req_priv_t *)priv;
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)app_ctx;
    juint16_t header, ival;
    juint32_t size = 0;
    juint8_t *temp_buf = NULL;
    juint8_t *buffer_pos = (juint8_t *)buffer;
    juint32_t current_size = length;

    /* Called from class driver upon completion of send_data */
    DBG_V(DHOST_CDC_EEM, ("get_data_cb: Entered - status %d, data len %lu,"
        "\n", status, length));

    /* If some error occurred, informing the application because it is
     * the one that initiated the read data */
    if (status)
    {
        eem_priv->cb(sc->cdc_sc.app_ctx, status, temp_buf, size,
            eem_priv->app_priv);
        goto Exit;
    }

    /* Parsing the data received and passing the data only to the app */
    header = ua_le16toh(buffer);

    if (header & EEM_HEADER_TYPE_COMMAND)
    {
        temp_buf = (juint8_t *)buffer + EEM_HEADER_LEN;

        if ((header & EEM_COMMAND_TYPE_ECHO) == EEM_COMMAND_TYPE_ECHO)
        {
            if ((app_callback_h) && (app_callback_h->echo))
                app_callback_h->echo(sc->cdc_sc.app_ctx, temp_buf, length, 0);
        }
        else if ((header & EEM_COMMAND_TYPE_ECHO_RESPONSE) == 
            EEM_COMMAND_TYPE_ECHO_RESPONSE)
        {
            if ((app_callback_h) && (app_callback_h->echo))
                app_callback_h->echo(sc->cdc_sc.app_ctx, temp_buf, length, 1);
        }
        else if ((header & EEM_COMMAND_TYPE_SUSPEND_HINT) == 
            EEM_COMMAND_TYPE_SUSPEND_HINT)
        {
            if ((app_callback_h) && (app_callback_h->suspend_hint))
                app_callback_h->suspend_hint(sc->cdc_sc.app_ctx);
        }
        else if ((header & EEM_COMMAND_TYPE_RESPONSE_HINT) == 
            EEM_COMMAND_TYPE_RESPONSE_HINT)
        {
            /* Unpack interval from header and call the cb */
            ival = header & EEM_IVAL_BM;
            if (app_callback_h->response_hint)
                app_callback_h->response_hint(sc->cdc_sc.app_ctx, ival, 0); 
        }
        else if ((header & EEM_COMMAND_TYPE_RESPONSE_HINT_COMPLETE) ==
            EEM_COMMAND_TYPE_RESPONSE_HINT_COMPLETE)
        {
            /* Unpack interval from header and call the cb */
            ival = header & EEM_IVAL_BM;
            if (app_callback_h->response_hint)
                app_callback_h->response_hint(sc->cdc_sc.app_ctx, ival, 1);
        }
    }
    else if (eem_priv->cb)
    {
        while (current_size)
        {
            /* Getting the next eem packet - changing the buffer, 
             * getting the new header, and fixing the temp buffer to be
             * the buffer without the header */
            header = ua_le16toh(buffer_pos);
            temp_buf = buffer_pos + EEM_HEADER_LEN;
            size = header & EEM_HEADER_LENGTH_MASK;
            /* In case of eem packet is garbage we don't want to stuck here
             * forever */
            if (size > current_size)
            {
                DBG_E(DHOST_CDC_EEM, ("The eem packet size readed from buffer "
                    "is invalid = %d[%d]\n", size, current_size));
                eem_priv->cb(sc->cdc_sc.app_ctx, JEIO, temp_buf, size,
                    eem_priv->app_priv);
                goto Exit;
            }
            else
            {
                eem_priv->cb(sc->cdc_sc.app_ctx, status, temp_buf, size,
                    eem_priv->app_priv);
            }

            /* Removing the size of the buffer that we read, 
             * and the header from the total length of the transfer */
            current_size -= (size + EEM_HEADER_LEN);
            buffer_pos = buffer_pos + size + EEM_HEADER_LEN;
        }
    }

Exit:
    jdma_free(eem_priv->handle);
    
    if (status != ECANCEL)
        eem_read_data_packet(sc, eem_priv->cb, eem_priv->app_priv);
    
    jfree(eem_priv);
}

/************************************************************************/
/*                         EEM API Implementation                        */
/*************************************************************************/

/* EEM Class Specific Requests Implementation */

/**
 * Function name:  uwh_eem_enable
 * Description:    Enables an attached CDC EEM device.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_enable(uwh_cdc_dev_h dev_ctx)
{
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    DBG_V(DHOST_CDC_EEM, ("uwh_eem_enable: Entered\n"));

    if (sc->cdc_sc.state == CDC_DEV_ENABLED)
        goto Exit;

    sc->cdc_sc.state = CDC_DEV_ENABLED;
    DBG_V(DHOST_CDC_EEM, ("uwh_eem_enable: EEM device [%p] enabled\n", sc));

Exit:
    return 0;
}

/**
 * Function name:  uwh_eem_disable
 * Description:    Disables an attached CDC EEM device.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
  Scope:          Global
 **/
jresult_t uwh_eem_disable(uwh_cdc_dev_h dev_ctx)
{
    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    DBG_V(DHOST_CDC_EEM, ("uwh_eem_disable: Entered\n"));

    if (sc->cdc_sc.state == CDC_DEV_ENABLED)
        cdc_abort_data_pipes(dev_ctx);

    DBG_V(DHOST_CDC_EEM, ("uwh_eem_disable: EEM device [%p] disabled\n", sc));
    sc->cdc_sc.state = CDC_DEV_ATTACHED;

    return 0;
}

#define ECHO_COMMAND_HEADER 0x8000
#define ECHO_RESPONSE_COMMAND_HEADER 0x8800

/**
 * Function name:  uwh_eem_echo
 * Description:    Sends an Echo command or Echo Response command to the device.
 * Parameters:
 *     @dev_ctx:  (IN) EEM class driver context
 *     @buffer:   (IN) Pointer to a buffer containing the data to be sent
 *     @size:     (IN) Size of the data to be sent, in bytes
 *     @cb:       (IN) Pointer to a callback function to be called when the
 *                     command has been sent
 *     @app_priv: (IN) Private application data for the callback function
 *     @response: (IN) TRUE  - Send an Echo Response command
 *                     FALSE - Send an Echo command
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_echo(uwh_cdc_dev_h dev_ctx, void *buffer, juint16_t size,
    uwh_eem_data_cb_t cb, void *app_priv, jbool_t response)
{
    juint16_t header, new_header;
    jresult_t rc;

    if (size > EEM_ECHO_MAX_LEN)
    {
        DBG_E(DHOST_CDC_EEM, ("uwh_eem_echo: Invalid length for echo "
            "request\n"));
        return JEINVAL;
    }

    header = (response ? ECHO_RESPONSE_COMMAND_HEADER : ECHO_COMMAND_HEADER) |
        size;
    
    ua_htole16(&new_header, header);
        
    rc = eem_tx(dev_ctx, buffer, size, cb, app_priv, new_header, 0);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("uwh_eem_echo: eem_tx failed\n"));

    return rc;
}

/**
 * Function name:  uwh_eem_tickle
 * Description:    Sends a Tickle command to the device to signal a timeout
 *                 from a ResponseCompleteHint command.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_tickle(uwh_cdc_dev_h dev_ctx)
{
    return JENOTSUP;
}

#define WITH_CRC_HEADER 0x4000

static jresult_t eem_send_usb_xfer(uwh_cdc_dev_h dev_ctx, void **buf, 
    juint32_t *size, juint16_t packet_num, uwh_eem_data_cb_t single_cb, 
    uwh_eem_data_bundle_cb_t bundle_cb, void *app_priv)
{
    jresult_t rc;
    juint32_t total_size = 0;
    jdma_handle usb_buf_handle = NULL;
    juint8_t *usb_buf;
    juint8_t *usb_buf_pos;
    juint32_t crc_data;
    juint16_t i;
    eem_req_priv_t *eem_priv = NULL;
    DECLARE_FNAME("eem_send_usb_xfer");

    ua_htobe32(&crc_data, 0xdeadbeef);

    for (i = 0; i < packet_num; i++)
        total_size += EEM_HEADER_LEN + size[i] + EEM_CRC_LEN;

    rc = jdma_alloc(total_size, 0, NULL, NULL, 0, &usb_buf_handle);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Failed to allocate USB buffer (size %ld)\n",
            fname, total_size));

        return JENOMEM;
    }
    usb_buf = jdma_virt_addr(usb_buf_handle, 0);

    /* Construct the USB xfer */
    for (i = 0, usb_buf_pos = usb_buf; i < packet_num; i++)
    {
        juint16_t new_header, header = (juint16_t)size[i] + EEM_CRC_LEN;
        ua_htole16(&new_header, header);    

        /* Copy the header */
        j_memcpy(usb_buf_pos, &new_header, EEM_HEADER_LEN);
        usb_buf_pos += EEM_HEADER_LEN;

        /* Copy the data after the header */
        j_memcpy(usb_buf_pos, buf[i], size[i]);
        usb_buf_pos += size[i];

        /* Copy the crc after the data and the header */
        j_memcpy(usb_buf_pos, &crc_data, EEM_CRC_LEN);
        usb_buf_pos += EEM_CRC_LEN;
    }
    
    eem_priv = (eem_req_priv_t *)jmalloc(sizeof(eem_req_priv_t), M_ZERO);
    if (!eem_priv)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Failed to allocate eem_priv\n", fname));
        rc = JENOMEM;
        goto Error;
    }

    eem_priv->cb = single_cb;
    eem_priv->bundle_cb = bundle_cb;
    eem_priv->app_priv = app_priv;
    eem_priv->handle = usb_buf_handle;
    
    if (bundle_cb)
    {
        eem_priv->orig_buffer = buf;
        eem_priv->orig_size = size;
    }
    else 
    {
        eem_priv->single_orig_buffer = *buf;
        eem_priv->single_orig_size = *size;
    }
    
    eem_priv->packet_num = packet_num;

    /* Send USB xfer */
    rc = cdc_data_send(dev_ctx, usb_buf, total_size, send_data_cb, dev_ctx, 
        eem_priv);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Failed sending data - %d\n", fname, rc));
        goto Error;
    }

    return 0;

Error:

    if (usb_buf_handle)
        jdma_free(usb_buf_handle);

    if (eem_priv)
        jfree(eem_priv);

    return rc;
}

/**
 * Function name:  uwh_eem_send_data_bundle
 * Description:    Sends a bundle of data packets to the device.
 * Parameters:
 *     @dev_ctx:       (IN) EEM class driver context
 *     @buffers:       (IN) Pointer to an array of buffers containing the data 
 *                          to be sent
 *     @sizes:         (IN) Pointer to an array of values depicting the sizes,
 *                          in bytes, of the data buffers to be sent
 *     @num_data_pkts: (IN) Number of data packets in the @buffers array
 *     @cb:            (IN) Pointer to a callback function to be called when all
 *                          of the data packets have been sent
 *     @app_priv:      (IN) Private application data for the callback function
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_send_data_bundle(uwh_cdc_dev_h dev_ctx, void **buffers,
    juint32_t *sizes, juint16_t num_data_pkts, uwh_eem_data_bundle_cb_t cb, 
    void *app_priv)
{
    jresult_t rc;
    DECLARE_FNAME("uwh_eem_send_data_bundle");

    rc = eem_send_usb_xfer(dev_ctx, buffers, sizes, num_data_pkts, NULL, cb, 
        app_priv);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("%s: Failed constructing USB xfer\n", fname));

    return rc;
}

/**
 * Function name:  uwh_eem_send_data_packet
 * Description:    Sends a single EEM data packet to the device.
 * Parameters:
 *     @dev_ctx:  (IN) EEM class driver context
 *     @buffer:   (IN) Pointer to a buffer containing the data to be sent
 *     @size:     (IN) Size of the data to be sent, in bytes
 *     @crc:      (IN) CRC checksum of the data
 *     @cb:       (IN) Pointer to a callback function to be called when the
 *                     data packet has been sent
 *     @app_priv: (IN) Private application data for the callback function
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_send_data_packet(uwh_cdc_dev_h dev_ctx, void *buffer, 
    juint32_t size, juint32_t crc, uwh_eem_data_cb_t cb, void *app_priv)
{
    jresult_t rc;
    DECLARE_FNAME("uwh_eem_send_data_packet");

    rc = eem_send_usb_xfer(dev_ctx, &buffer, &size, 1, cb, NULL, app_priv);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("%s: Failed constructing USB xfer\n", fname));

    return rc;
}

/**
 * Function name:  uwh_eem_start_rx
 * Description:    Enables receiving data from the device.
 * Parameters:
 *     @dev_ctx:          (IN) EEM class driver context
 *     @cb:               (IN) Pointer to the callback function to be called 
 *                             when an EEM data packet is received
 *     @app_priv:         (IN) Private application data for the callback
 *                             function
 *     @rx_req_pool_size: (IN) Number of buffers to allocate for receipt of EEM
 *                             packets on the USB
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_start_rx(uwh_cdc_dev_h dev_ctx, 
    uwh_eem_data_cb_t cb, void *app_priv, juint32_t rx_req_pool_size)
{
    juint_t i;
    jresult_t rc = 0;
    DECLARE_FNAME("uwh_eem_start_rx");
    
    DBG_V(DHOST_CDC_EEM, ("%s: Starting Rx transfers\n", fname));
    
    for (i = 0; i < rx_req_pool_size && !rc; i++)
        rc = eem_read_data_packet(dev_ctx, cb, app_priv);

    DBG_IF(rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Failed to queue transfer no:%d of %d\n",
            fname, i, rx_req_pool_size));
    }
    return rc;
}

static jresult_t eem_read_data_packet(uwh_cdc_dev_h dev_ctx, 
    uwh_eem_data_cb_t cb, void *app_priv)
{
    jresult_t rc = 0;
    jdma_handle handle = NULL;
    eem_req_priv_t *eem_priv = NULL;
    DECLARE_FNAME("eem_read_data_packet");
    
    DBG_V(DHOST_CDC_EEM, ("%s: Entered\n", fname));
    
    rc = jdma_alloc(MAX_READ_SIZE, 0, NULL, NULL, 0, &handle);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Couldn't alloc buffer\n", fname));
        return rc;
    }

    eem_priv = (eem_req_priv_t *)jmalloc(sizeof(eem_req_priv_t), M_ZERO);
    if (!eem_priv)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Couldn't alloc eem_priv\n", fname));
        rc = JENOMEM;
        goto Error;
    }

    eem_priv->handle = handle;
    eem_priv->app_priv = app_priv;
    eem_priv->cb = cb;

    rc = cdc_data_get(dev_ctx, jdma_virt_addr(handle, 0), MAX_READ_SIZE, 
        get_data_cb, dev_ctx, eem_priv);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: cdc_data_get failed\n", fname));
        goto Error;
    }

    return 0;
Error:
    if (handle)
        jdma_free(handle);

    if (eem_priv)
        jfree(eem_priv);

    return rc;
}

/*****************************************************************************/
/*                              USB Core Required Functions                  */
/*****************************************************************************/

/* Probe / Attach Functions.
 * Two levels of probe/attach - device level & IAD level */

/**
 * Function name:  jcdc_eem_probe
 * Description:    Checks the device's compatibility level with the driver.
 * Parameters:
 *     @dev: (IN) Device handle.
 *
 * Return value:   UMATCH_NONE if the required interfaces weren't found, or if
 *                 no application is registered with the driver;
 *                 otherwise UMATCH_IFACECLASS_IFACESUBCLASS_IFACEPROTO.
 * Scope:          Global
 **/
static jresult_t jcdc_eem_probe(jdevice_t dev)
{
    usb_interface_descriptor_t *iface_d;
    struct usb_attach_arg *uaa = 
        (struct usb_attach_arg *)j_device_get_ivars(dev);    
    DECLARE_FNAME("jcdc_eem_probe");

    DBG_V(DHOST_CDC_EEM, ("%s: Entered\n", fname));
    if (!uaa->device)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Invalid attach argument\n", fname));
        return UMATCH_NONE;
    }
    
    if (!uaa->iface)
        return UMATCH_NONE;

    /* Get the Communication Class Interface descriptor */
    iface_d = usbd_get_interface_descriptor(uaa->iface);

    if (!iface_d)
        return UMATCH_NONE;

    if ((iface_d->bInterfaceClass == CDC_COMM_IFACE_CLASS_CODE) && 
        (iface_d->bInterfaceSubClass == EEM_IFACE_SUBCLASS_CODE))
    {
        DBG_V(DHOST_CDC_EEM, ("%s: Finished\n", fname));
        return UMATCH_IFACECLASS_IFACESUBCLASS;
    }

    return UMATCH_NONE;
}

/**
 * Function name:  jcdc_eem_detach
 * Description:    EEM device detach driver method callback.
 * Parameters:
 *     @dev: (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jcdc_eem_detach(jdevice_t dev)
{
    jcdc_eem_softc_t *sc;

    DBG_V(DHOST_CDC_EEM, ("jcdc_eem_detach: Detaching EEM device %p\n", dev));
    if (!dev)
    {
        DBG_E(DHOST_CDC_EEM, ("jcdc_eem_detach: Invalid dev\n"));
        return JEINVAL;
    }

    sc = (jcdc_eem_softc_t *)j_device_get_softc(dev);
    if (!sc)
    {
        DBG_E(DHOST_CDC_EEM, ("jcdc_eem_detach: Invalid ctx\n"));
        return JEINVAL;
    }

    cdc_close_data_pipes(sc);

/* HSU temp fix - start*/
    /* Call the application's detach callback */
    if ((app_callback_h) && (app_callback_h->detach))
        app_callback_h->detach(sc->cdc_sc.app_ctx);
/* HSU temp fix - End*/

    /* Check that the application called uwh_eem_disable */
    /* HSU temp fix - start*/
    KASSERT((sc->cdc_sc.state == CDC_DEV_ATTACHED) || 
      (sc->cdc_sc.state == CDC_DEV_SUSPENDED), 
        ("CDC EEM was not disabled properly.\n"));
    /* HSU temp fix - End*/

    sc->cdc_sc.state = CDC_DEV_DISCONNECTED;

    return 0;
}

/**
 * Function name:  jcdc_eem_attach
 * Description:    EEM device attach driver method callback.
 * Parameters:
 *     @dev: (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jcdc_eem_attach(jdevice_t dev)
{
    jresult_t rc = JENXIO;
    jcdc_eem_softc_t *sc;
    struct usb_attach_arg *uaa;
    jint_t i;
    usb_endpoint_descriptor_t *ed;
    juint8_t num_endpoints = 0;
    DECLARE_FNAME("jcdc_eem_attach");
    
    DBG_V(DHOST_CDC_EEM, ("%s: Entered\n", fname));
    
    if (!app_callback_h)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: No upper layer is registered\n", fname));
        return JEINVAL;
    }
    
    uaa = j_device_get_ivars(dev);    

    /* Build the softc structure */
    sc = j_device_get_softc(dev);
    sc->cdc_sc.dev = dev;
    sc->cdc_sc.device = uaa->device;
    sc->cdc_sc.drv_type = JCDC_EEM;
    sc->cdc_sc.d_iface = uaa->iface;

    if (usbd_endpoint_count(sc->cdc_sc.d_iface, &num_endpoints))
    {
        DBG_E(DHOST_CDC_EEM,("%s: Invalid Interface Descriptor\n", fname));
        return JEINVAL;
    }

    /* Going over all the endpoints and finding the endpoints we need */
    for (i = 0; i < num_endpoints; i++) 
    {
        DBG_V(DHOST_CDC_EEM, ("%s: num_endpoints = %d iface number "
            "is %d\n", fname, num_endpoints, 
            sc->cdc_sc.d_iface->idesc->bInterfaceNumber));

        ed = usbd_interface2endpoint_descriptor(sc->cdc_sc.d_iface, 
            (juint8_t)i);
        if (!ed) 
        {    
            DBG_E(DHOST_CDC_EEM, ("%s: couldn't read endpoint descriptor\n", 
                fname));
             return JEINVAL;
        }

        if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN
            && (UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK)) 
        {
            sc->cdc_sc.bulk_ep_in = ed->bEndpointAddress;
        } 
        else if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_OUT
            && (UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK)) 
        {
            sc->cdc_sc.bulk_ep_out = ed->bEndpointAddress;
        }
    }

    if (!sc->cdc_sc.bulk_ep_out || !sc->cdc_sc.bulk_ep_in)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: couldn't find all endpoints\n", fname));
        return JEINVAL;
    }
    
    /* Open pipes */
    rc = cdc_open_data_pipes(sc);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: error opening pipe %d\n", fname, rc));
        goto Exit;
    }

    sc->cdc_sc.state = CDC_DEV_ATTACHED;

/* HSU temp fix - start*/
    /* Report new device to the application and get a context */
    if ((app_callback_h) && (app_callback_h->attach))
	{
    rc = app_callback_h->attach(sc, &sc->cdc_sc.app_ctx);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Failed to attach to application - %d\n", 
            fname, rc));
        jcdc_eem_detach(dev);
        goto Exit;
    }
	}
/* HSU temp fix - End */

    DBG_V(DHOST_CDC_EEM, ("%s: Attaching EEM device successful dev=%p "
        "app_ctx=%p\n", fname, sc, sc->cdc_sc.app_ctx));

Exit:
    return rc;
}

/**
 * Function name:  jcdc_eem_ioctl
 * Description:    EEM IOCTL handler driver method callback.
 * Parameters:
 *     @dev:   (IN) Device handle
 *     @ioctl: (IN) IOCTL code
 *     @arg:   (IN) Pointer to IOCTL arguments (NULL if there are no arguments)
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jcdc_eem_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg)
{
    /* Not supported */
    DBG_E(DHOST_CDC_EEM, ("jcdc_eem_ioctl: Ioctl %d not supported\n", ioctl));

    return JEINVAL;
}

/**
 * Function name:  jcdc_eem_suspend
 * Description:    EEM device suspend driver method callback.
 * Parameters:
 *     @dev: (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jcdc_eem_suspend(jdevice_t dev)
{
    jresult_t rc = 0;
    jcdc_eem_softc_t *sc;

    sc = j_device_get_softc(dev);

    DBG_V(DHOST_CDC_EEM, ("jcdc_eem_suspend: Suspending EEM device\n"));

    if (sc->cdc_sc.state == CDC_DEV_SUSPENDED) 
    {
        DBG_W(DHOST_CDC_EEM, ("jcdc_eem_suspend: Device is already "
            "suspended\n"));

        /* Return with no error code */
        goto Exit;
    }

    /* Disable & abort pipes */
    sc->cdc_sc.pre_suspention_state = sc->cdc_sc.state;

    rc = uwh_eem_disable(sc);
    DBG_IF(rc)
        DBG_E(DHOST_CDC_EEM, ("jcdc_eem_suspend: Failed disable [%d]\n", rc));

    sc->cdc_sc.state = CDC_DEV_SUSPENDED;

    DBG_V(DHOST_CDC_EEM, ("jcdc_eem_suspend: EEM device suspended [%p]\n", sc));

Exit:
    return rc;
}

/**
 * Function name:  jcdc_eem_resume
 * Description:    EEM device resume driver method callback.
 * Parameters:
 *     @dev: (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code.
 * Scope:          Local
 **/
static jresult_t jcdc_eem_resume(jdevice_t dev)
{
    jresult_t rc = 0;
    jcdc_eem_softc_t *sc;

    sc = j_device_get_softc(dev);

    DBG_V(DHOST_CDC_EEM, ("jcdc_eem_resume: Resuming EEM device\n"));

    if (sc->cdc_sc.state != CDC_DEV_SUSPENDED) 
    {
        DBG_E(DHOST_CDC_EEM, ("jcdc_eem_resume: Device is already active\n"));
        goto Exit;
    }

    /* Set state to attached - allowing default pipe communication */
    sc->cdc_sc.state = CDC_DEV_ATTACHED;

    /* Re-enable the device if it was enabled before suspension */
    if (sc->cdc_sc.pre_suspention_state == CDC_DEV_ENABLED)
    {
        rc = uwh_eem_enable(sc);
        /* Even if failed to enable the device - it is still resumed*/
        DBG_IF(rc)
        {
            DBG_E(DHOST_CDC_EEM, ("jcdc_eem_resume: Failed enabling EEM device "
                "[%p]\n", sc));
        }
    }

Exit:
    return rc;
}

/**
 * Function name:  uwh_eem_suspend
 * Description:    Suspends operation of a CDC EEM device.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_suspend(uwh_cdc_dev_h dev_ctx)
{
    jresult_t rc = JEINVAL;
    
    DECLARE_FNAME("uwh_eem_suspend");

    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Invalid EEM device context\n", fname));
        goto Exit;
    }
        
    CORE_SAFE_ENTER;

    /* Suspending the device using the Core's suspend function */
    rc = jcdc_eem_suspend(sc->cdc_sc.dev);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Error suspending the deivce rc = %d\n",
            fname, rc));
        goto Exit;
    }
    
    rc = j_device_ioctl(j_device_get_parent(sc->cdc_sc.dev), 
        DRV_IOCTL_PWR_CHILD_SUSPEND, sc->cdc_sc.dev);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Cannot send suspend ioctal %d\n",
            fname, rc));
    } 

    CORE_SAFE_LEAVE;

Exit:
    return rc;
}

/**
 * Function name:  uwh_eem_resume
 * Description:    Resumes normal operation of a suspended CDC EEM device.
 * Parameters:
 *     @dev_ctx: (IN) EEM class driver context
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_resume(uwh_cdc_dev_h dev_ctx)
{
    jresult_t rc = JEINVAL;

    DECLARE_FNAME("uwh_eem_resume");

    jcdc_eem_softc_t *sc = (jcdc_eem_softc_t *)dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Invalid EEM device context\n", fname));
        return rc;
    }
        
    CORE_SAFE_ENTER;

    rc = j_device_ioctl(j_device_get_parent(sc->cdc_sc.dev), 
        DRV_IOCTL_PWR_CHILD_RESUME, sc->cdc_sc.dev);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Cannot send resume ioctal %d\n",
            fname, rc));

        goto Exit;
    }   

    /* Suspending the device using the Core's suspend function */
    rc = jcdc_eem_resume(sc->cdc_sc.dev);
    if (rc)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Error resuming the deivce rc = %d\n",
            fname, rc));

        j_device_ioctl(j_device_get_parent(sc->cdc_sc.dev), 
            DRV_IOCTL_PWR_CHILD_SUSPEND, sc->cdc_sc.dev);
    }
    
Exit:
    CORE_SAFE_LEAVE;
    return rc;

}

/**
 * Function name:  uwh_eem_register
 * Description:    Registers an application with the CDC EEM class driver.
 *                 As part of the registration, the application passes to the 
 *                 class driver a list of its callback functions (@eem_cb).
 *                 Note: The CDC EEM class driver will only handle CDC EEM 
 *                 devices that are attached to the host platform after 
 *                 uwh_eem_register() returns successfully.
 * Parameters:
 *     @eem_cb: (IN) Application callback functions
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Global
 **/
jresult_t uwh_eem_register(uwh_eem_callbacks_t *eem_cb)
{
    DECLARE_FNAME("uwh_eem_register");

    if (!eem_cb)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Error registering CDC_EEM - NULL callback"
            "structure\n", fname));
        goto Error;
    }
    
    if (!eem_cb->attach || !eem_cb->detach)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Missing mandatory callbacks (attach/detach)"
            "\n", fname));
        goto Error;
    }

    if (app_callback_h)
    {
        DBG_E(DHOST_CDC_EEM, ("%s: Error registering CDC_EEM - application "
            "already registered\n", fname));
        goto Error;
    }

    app_callback_h = eem_cb;

    DBG_V(DHOST_CDC_EEM, ("%s: Finished successfully\n", fname));

    return 0;

Error:
    return JEINVAL;
}

/**
 * Function name:  uwh_eem_unregister
 * Description:    Unregisters an application from the CDC EEM class driver.
 * Parameters:     None
 * Return value:   None
 * Scope:          Global
 **/
void uwh_eem_unregister(void)
{
    app_callback_h = NULL;
}

/* Register the EEM class driver with the USB Core */
DRIVER_USB_DECLARE(jcdc_eem)


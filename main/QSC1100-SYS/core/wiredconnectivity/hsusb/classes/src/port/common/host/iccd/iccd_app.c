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
#include <usbdi_status.h>
#include <uw_host_iccd.h>

enum iccd_state {
    POWER_OFF,
    POWER_ON,
    DATA_BLOCK,
    XFER_BLOCK
};

typedef struct {
    uwh_iccd_drv_h driver_ctx;
    void *task_handle;
    enum iccd_state state;
} iccd_app_softc_t;

static jint_t apdu_count = 0;

static jresult_t attach(uwh_iccd_drv_h drv_ctx, uwh_iccd_app_h *app_ctx);
static void detach(uwh_iccd_app_h app_ctx);
static void slot_change(juint8_t message_type, juint8_t slot_status, 
    uwh_iccd_app_h app_ctx);
static void transfer_task(void *arg);
static void general_cb(uwh_iccd_app_h app_ctx, void *data, 
    juint16_t size, iccd_response_type_t response_type, ab_data_t ab_data, 
    iccd_error_t error, void *arg, jresult_t usb_status);
static jresult_t send_apdu_response(iccd_app_softc_t *sc, void *data,
    juint16_t size);
static jresult_t get_data_block(iccd_app_softc_t *sc);

static uwh_iccd_callbacks_t functions = {
    attach,
    detach,
    slot_change
};

jresult_t iccd_app_init(void)
{
    uwh_iccd_register(&functions);
    return 0;
}

void iccd_app_uninit(void)
{
    uwh_iccd_unregister();
}

static jresult_t attach(uwh_iccd_drv_h drv_ctx, uwh_iccd_app_h *app_ctx)
{
    iccd_app_softc_t *sc = NULL;
    jresult_t rc;
    DECLARE_FNAME("attach");

    DBG_V(DHOST_ICCD_APP,("%s: Entered\n", fname));
    
    sc = (iccd_app_softc_t *)jmalloc(sizeof(iccd_app_softc_t), M_ZERO);
    if (!sc)
    {
        DBG_E(DHOST_ICCD_APP,("%s: Out of memory!\n", fname));
        rc = JENOMEM;
        goto Exit;
    }

    *app_ctx = (uwh_iccd_app_h)sc;
    sc->driver_ctx = drv_ctx;
    
    apdu_count = 0;
    rc = jthread_create(transfer_task, (void *)sc, THREAD_PRIORITY_OTHER,
        &sc->task_handle);   
#ifdef JDEBUG
    if (rc)
        DBG_E(DHOST_ICCD_APP,("%s: Error starting thread\n", fname));
#endif

Exit:
    if (rc && sc)
        jfree(sc);

    return rc;
}

static void detach(uwh_iccd_app_h app_ctx)
{
    iccd_app_softc_t *sc = (iccd_app_softc_t *)app_ctx;
    DECLARE_FNAME("detach");

    DBG_V(DHOST_ICCD_APP,("%s: Entered\n", fname));

    if (sc)
        jfree(sc);
}

static void slot_change(juint8_t message_type, juint8_t slot_status, 
    uwh_iccd_app_h app_ctx)
{
    DECLARE_FNAME("slot_change");

    DBG_I(DHOST_ICCD_APP, ("%s: message_type = %d slot_status = %d\n", 
        fname, message_type, slot_status));
}

static void transfer_task(void *arg)
{
    iccd_app_softc_t *sc = (iccd_app_softc_t *)arg;
#ifdef JDEBUG
    uwh_iccd_descriptor_t *desc;
  
    DECLARE_FNAME("transfer_task");

    desc = uwh_get_info(sc->driver_ctx);
    DBG_I(DHOST_ICCD_APP, ("%s: length = %d\n", fname, desc->length));
    DBG_I(DHOST_ICCD_APP, ("%s: release_number = %d\n", fname, 
        desc->release_number));
    DBG_I(DHOST_ICCD_APP, ("%s: max_slot_index = %d\n", fname,
        desc->max_slot_index));
    DBG_I(DHOST_ICCD_APP, ("%s: voltage_support = %d\n", fname,
        desc->voltage_support));
    DBG_I(DHOST_ICCD_APP, ("%s: protocols = %ld\n", fname, desc->protocols));
    DBG_I(DHOST_ICCD_APP, ("%s: default_clock = %ld\n", fname,
        desc->default_clock));
    DBG_I(DHOST_ICCD_APP, ("%s: maximum_clock = %ld\n", fname,
        desc->maximum_clock));
    DBG_I(DHOST_ICCD_APP, ("%s: num_clock_supported = %ld\n", fname,
        desc->num_clock_supported));
    DBG_I(DHOST_ICCD_APP, ("%s: data_rate = %ld\n", fname, desc->data_rate));
    DBG_I(DHOST_ICCD_APP, ("%s: max_data_rate = %ld\n", fname, 
        desc->max_data_rate));
    DBG_I(DHOST_ICCD_APP, ("%s: num_data_rate_supported= %d\n", fname, 
        desc->num_data_rate_supported));
    DBG_I(DHOST_ICCD_APP, ("%s: max_ifsd = %ld\n", fname, desc->max_ifsd));
    DBG_I(DHOST_ICCD_APP, ("%s: synch_protocols = %ld\n", fname, 
        desc->synch_protocols));
    DBG_I(DHOST_ICCD_APP, ("%s: mechanical = %ld\n", fname, desc->mechanical));
    DBG_I(DHOST_ICCD_APP, ("%s: features = %ld\n", fname, desc->features));
    DBG_I(DHOST_ICCD_APP, ("%s: max_ccid_message_length = %ld\n", fname,
        desc->max_ccid_message_length));   
    DBG_I(DHOST_ICCD_APP, ("%s: class_get_response = %d\n", fname, 
        desc->class_get_response));
    DBG_I(DHOST_ICCD_APP, ("%s: class_envelope = %d\n", fname, 
        desc->class_envelope));
    DBG_I(DHOST_ICCD_APP, ("%s: lcd_layout = %d\n", fname, desc->lcd_layout));
    DBG_I(DHOST_ICCD_APP, ("%s: pin_support = %d\n", fname, 
        desc->pin_support));
    DBG_I(DHOST_ICCD_APP, ("%s: max_ccid_busy_slots = %d\n", fname,
        desc->max_ccid_busy_slots));
#endif
   
    sc->state = POWER_OFF;
    uwh_iccd_power(sc->driver_ctx, 0, general_cb);
}

static void general_cb(uwh_iccd_app_h app_ctx, void *data, 
    juint16_t size, iccd_response_type_t response_type, ab_data_t ab_data, 
    iccd_error_t error, void *arg, jresult_t usb_status)
{
    iccd_app_softc_t *sc = (iccd_app_softc_t *)app_ctx;
    DECLARE_FNAME("general_cb");

    DBG_V(DHOST_ICCD_APP,("%s: Entered \n", fname));

#ifdef JDEBUG
    if (ab_data.status == UWH_ICCD_BSTATUS_PRESENT_NOT_ACTIVE)
        DBG_V(DHOST_ICCD_APP,("%s: Card present not activated\n", fname));
    else if (ab_data.status == UWH_ICCD_BSTATUS_VIRTUALLY_NOT_PRESENT)
        DBG_V(DHOST_ICCD_APP,("%s: Card virtually not present\n", fname));
    else
        DBG_V(DHOST_ICCD_APP,("%s: Card is present and activated\n", fname));

    if (error)
    {
        DBG_E(DHOST_ICCD_APP,("%s: Error occurred error is %d\n", fname, 
            error));
    }
#endif

    if (response_type == UWH_ICCD_BRESPONSE_POLLING)
    {
        juint32_t delay = ab_data.delay * 10;
        
        DBG_I(DHOST_ICCD_APP,("%s: Received polling from the device delay "
            "time is %d\n", fname, delay));

        jdelay_ms(delay);
    }

    if (usb_status)
        return;

    switch (sc->state)
    {
    case POWER_OFF:
        sc->state = POWER_ON;
        uwh_iccd_power(sc->driver_ctx, 1, general_cb);
        break;
    case POWER_ON:
    case DATA_BLOCK:
        sc->state = XFER_BLOCK;
        get_data_block(sc);
        break;
    case XFER_BLOCK:
        send_apdu_response(sc, data, size);
        break;
    }
}

static void data_block_cb(uwh_iccd_app_h app_ctx, void *data, 
    juint16_t size, iccd_response_type_t response_type, ab_data_t ab_data, 
    iccd_error_t error, void *arg, jresult_t usb_status)
{
    general_cb(app_ctx, data, size, response_type, ab_data, error, arg, 
        usb_status);

    jfree(arg);
}

static jresult_t send_apdu_response(iccd_app_softc_t *sc, void *data,
    juint16_t size)
{
    juint8_t new_data[7]; 
    juint16_t new_size = 0;
    juint8_t *buf = data;
    jbool_t next_level = 0;
    DECLARE_FNAME("send_apdu_response");

    if (apdu_count > 4)
        return JENOTSUP;
    
    switch (apdu_count)
    {
    case 0:
        new_data[0] = 0x80;
        new_data[1] = 0xF2;
        new_data[2] = 0x00;
        new_data[3] = 0x0C;
        new_size = 4;
        next_level = 1;
        break;
    case 1:
        new_data[0] = 0x00;
        new_data[1] = 0xA4;
        new_data[2] = 0x00;
        new_data[3] = 0x0C;
        new_data[4] = 0x02;
        new_data[5] = 0x2F;
        new_data[6] = 0xE2;
        new_size = 7;

        if (buf[1] == 0x90 && buf[2] == 0)
            next_level = 1;

        break;
    case 2:
        new_data[0] = 0x00;
        new_data[1] = 0xB0;
        new_data[2] = 0x00;
        new_data[3] = 0x00;
        new_data[4] = 0x0A;
        new_size = 5;

        if (buf[1] == 0x90 && buf[2] == 0)
            next_level = 1;

        break;
        
    case 3:
        new_data[0] = 0x00;
        new_data[1] = 0xA4;
        new_data[2] = 0x00;
        new_data[3] = 0x04;
        new_data[4] = 0x02;
        new_data[5] = 0x2F;
        new_data[6] = 0xE2;
        new_size = 7;

        if (buf[11] == 0x90 && buf[12] == 0)
            next_level = 1;

        break;
    case 4:
        if (buf[1] == 0x62 && buf[2] == 0x20 && buf[3] == 0x82 && 
            buf[4] == 0x02)
        {
            DBG_V(DHOST_ICCD_APP, ("%s: Response for fourth command tested "
                "received succsesfully. No more data is sent\n", fname));
            apdu_count++;
            goto Last;
        }
    }

    if (next_level)
    {
        apdu_count ++;
        sc->state = DATA_BLOCK;
        return uwh_iccd_xfr_block(sc->driver_ctx, new_data, new_size, 
            general_cb, NULL);
    }
    else
    {
        sc->state = XFER_BLOCK;
        get_data_block(sc);
    }

Last:
    return 0;
    
}

static jresult_t get_data_block(iccd_app_softc_t *sc)
{
    juint8_t *data;
    juint16_t size = 40;
    
    data = (juint8_t *)jmalloc(size, M_ZERO);
    if (!data)
    {
        DBG_E(DHOST_ICCD_APP, ("get_data_block:Couldn't alloc memory\n"));
        return JENOMEM;
    }

    return uwh_iccd_data_block(sc->driver_ctx, data, size, data_block_cb, 
        data);
}

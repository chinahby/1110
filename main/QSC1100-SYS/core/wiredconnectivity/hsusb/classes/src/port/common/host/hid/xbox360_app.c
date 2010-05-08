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
#include "host_hid_app.h"
#include <uw_host_hid.h>

#define KEY_ERROR 1
#define KEY_PRESSED 1
#define KEY_RELEASED 0

struct out_args {
    hid_gen_dev_ctx_t hid_handler;
    juint8_t *data;
    juint8_t report_id;
};

struct jxbx_loc {
    juint32_t loc_usage;
    void *location;
    TAILQ_ENTRY(jxbx_loc) next_loc;
};

typedef struct jxbox360_softc {
    hid_gen_dev_ctx_t           hid_handler;
    input_device_reg_t          input_device;
    jtask_h                     out_task;
    os_input_ops_t              os_in_ops;
    os_input_ctx_t              os_input_dev;
    void                        *dpad_up;
    void                        *dpad_down;
    void                        *dpad_left;
    void                        *dpad_right;
    void                        *x_loc;
    void                        *rx_loc;
    void                        *y_loc;
    void                        *ry_loc;
    void                        *z_loc;
    void                        *rz_loc;
    void                        *leds_byte_count;
    void                        *leds_loc;
    void                        *motor_byte_count;
    void                        *big_motor;
    void                        *little_motor;
    TAILQ_HEAD(_jxbx_loc_head, jxbx_loc) buttons_queue;
    jbool_t is_enabled;
    TAILQ_ENTRY(jxbox360_softc) next_dev;
} jxbox360_softc_t;

static TAILQ_HEAD(_xbox360_head, jxbox360_softc) jxbox360_dev_queue;

char jxbox360_name[]={'H','I','D', ' ', 'X', 'b', 'o', 'x', '3', '6', '0', ' ',
    'G', 'a', 'm', 'e', 'P', 'a', 'd', '\0'};

static jxbox360_softc_t *find_jxbx_dev(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jxbox360_softc_t *dev;
    struct _xbox360_head *list_head = (struct _xbox360_head*)app_ctx;

    TAILQ_FOREACH(dev, list_head, next_dev)
    {
        if (dev->hid_handler == hid_gen_dev_ctx)
            return dev;
    }

    return NULL;
}

static void actual_send_report(void *args)
{
    struct out_args *oa = (struct out_args *)args;
    hid_general_write(oa->hid_handler, oa->data, oa->report_id, 
        UHID_OUTPUT_REPORT);
    jfree(oa->data);
    jfree(oa);
}

static jresult_t jxbox360_enable(hid_app_dev_ctx_t app_dev_ctx)
{
    jresult_t rc = 0;
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        DBG_I(DHOST_HID_GENERAL, ("jxbox360_enable: Trying to enable an "
            "already enabled device\n"));
    }
    else
    {
        sc->hid_handler = hid_general_open(sc->hid_handler, 0);
        if (!sc->hid_handler)
            rc = JEIO;
        else
            sc->is_enabled = TRUE;
    }

    return rc;
}

static void jxbox360_disable(hid_app_dev_ctx_t app_dev_ctx)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        /* Remove out task if exists */
        jtask_stop(sc->out_task);

        hid_general_close(sc->hid_handler);
        sc->is_enabled = FALSE;
    }
#ifdef JDEBUG
    else
    {
        DBG_I(DHOST_HID_GENERAL, ("jxbox360_disable: Trying to disable an "
            "already disabled device\n"));
    }
#endif
}

static jresult_t jxbox360_get_usg_att(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, usage_att_t *att)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;
    jresult_t rc = 0;

    j_memset(att, 0, sizeof(usage_att_t));

    switch (usage)
    {
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X):
        hid_general_get_att_from_location(sc->x_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y):
        hid_general_get_att_from_location(sc->y_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z):
        hid_general_get_att_from_location(sc->z_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RZ):
        hid_general_get_att_from_location(sc->rz_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RX):
        hid_general_get_att_from_location(sc->rx_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_RY):
        hid_general_get_att_from_location(sc->ry_loc, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_UP):
        hid_general_get_att_from_location(sc->dpad_up, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_DOWN):
        hid_general_get_att_from_location(sc->dpad_down, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_LEFT):
        hid_general_get_att_from_location(sc->dpad_left, att);
        break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_DPAD_RIGHT):
        hid_general_get_att_from_location(sc->dpad_right, att);
        break;

    default:
        if (HID_USAGE_PAGE(usage) == HUP_BUTTON)
        {
            struct jxbx_loc *but_loc;
            TAILQ_FOREACH(but_loc, &sc->buttons_queue, next_loc)
            {
                if (but_loc->loc_usage == HID_USAGE_USAGE(usage))
                    hid_general_get_att_from_location(but_loc->location, att);
            }
        }
        else
        {
            DBG_E(DHOST_HID_GENERAL, ("Requested usage(0x%x) not in the report"
                "\n", usage));
            rc = JENOENT;
        }
    }

    return rc;
}

static juint32_t jxbox360_rep_size(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t report_id)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;

    return hid_general_get_report_size(sc->hid_handler, report_id, 
        UHID_OUTPUT_REPORT);
}

static jresult_t jxbox360_set_field(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, juint32_t value, juint8_t *buf, juint8_t report_id)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;
    jresult_t rc = JEINVAL;
    void *loc = NULL;

    if (!report_id) /* Vibration report */
    {
        if (hid_general_is_location_of_usage(usage, sc->motor_byte_count))
            loc = sc->motor_byte_count;
        else if (hid_general_is_location_of_usage(usage, sc->big_motor))
            loc = sc->big_motor;
        else if (hid_general_is_location_of_usage(usage, sc->little_motor))
            loc = sc->little_motor;
    }
    else if (report_id == 1) /* Leds report */
    {
        if (hid_general_is_location_of_usage(usage, sc->leds_byte_count))
            loc = sc->leds_byte_count;
        else if (hid_general_is_location_of_usage(usage, sc->leds_loc))
            loc = sc->leds_loc;
    }

    if (loc)
        rc = hid_general_set_field_value(loc, buf, value);

    return rc;
}

static jresult_t jxbox360_send_rep(void *app_dev_ctx, juint8_t *data, 
    juint8_t size, juint8_t report_id)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;
    struct out_args *oa = NULL;

    oa = (struct out_args *)jmalloc(sizeof(struct out_args), M_ZERO);
    if (!oa)
        return JENOMEM;
    
    oa->hid_handler = sc->hid_handler;
    oa->report_id = report_id;
    
    oa->data = (juint8_t *)jmalloc(size, M_ZERO);
    if(!oa->data)
    {
        jfree(oa);
        return JENOMEM;
    }
    
    j_memcpy(oa->data, data, size);

    jtask_schedule(sc->out_task, 0, actual_send_report, oa);
    return 0;
}

#define XBOX360_NUM_BUTTONS 11
static juint32_t jxbox360_get_max_but(hid_app_dev_ctx_t app_dev_ctx)
{
    return XBOX360_NUM_BUTTONS;
}

static void *jxbox360_get_usbd_handle(hid_app_dev_ctx_t app_dev_ctx)
{
    jxbox360_softc_t *sc = (jxbox360_softc_t *)app_dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL, ("jxbox360_get_usbd_handle: received invalid "
            "device handle\n"));
        return NULL;
    }
 
    return hid_general_get_usbd_handle(sc->hid_handler);
}

#define GET_HID_LOC(hid_page, hid_usage, hid_rep_type, hid_rep_id, hid_to) \
    do { \
        rc = hid_general_get_field_location(hid_gen_dev_ctx, \
            HID_USAGE2(hid_page, hid_usage), hid_rep_type, hid_rep_id, \
            &hid_to);\
        if (rc)\
        {\
            DBG_E(DHOST_HID_GENERAL,\
                ("Error during parsing of the report descriptor for " \
                #hid_usage ": %d\n", rc)); \
            goto Error; \
        } \
    } while(0)

#define FREE_HID_LOC(loc) \
    do { \
        if (loc) \
            hid_general_free_location(&loc); \
    } while (0)

static void jxbox360_detach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx);

static jbool_t jxbox360_attach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jresult_t rc;
    juint_t t;
    struct _xbox360_head *list_head = (struct _xbox360_head*)app_ctx;
    jxbox360_softc_t *sc = NULL;
    
    if (!hid_general_is_collection(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_GAME_PAD)))
    {
        goto Error;
    }

    sc = (jxbox360_softc_t*)jmalloc(sizeof(jxbox360_softc_t), M_ZERO);
    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL,("Attach xbox360: Out of memory!\n"));
        goto Error; 
    }
    
    /* add device to the list*/
    TAILQ_INSERT_TAIL(list_head, sc, next_dev);

    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_X, UHID_INPUT_REPORT, 0, sc->x_loc);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_Y, UHID_INPUT_REPORT, 0, sc->y_loc);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_Z, UHID_INPUT_REPORT, 0, sc->z_loc);

    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_RX, UHID_INPUT_REPORT, 0, sc->rx_loc);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_RY, UHID_INPUT_REPORT, 0, sc->ry_loc);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_RZ, UHID_INPUT_REPORT, 0, sc->rz_loc);

    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_DPAD_UP, UHID_INPUT_REPORT, 0, 
        sc->dpad_up);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_DPAD_DOWN, UHID_INPUT_REPORT, 0, 
        sc->dpad_down);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_DPAD_LEFT, UHID_INPUT_REPORT, 0, 
        sc->dpad_left);
    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_DPAD_RIGHT, UHID_INPUT_REPORT, 0, 
        sc->dpad_right);
    
    TAILQ_INIT(&sc->buttons_queue);

    for (t = 0; t < XBOX360_NUM_BUTTONS; t++)
    {
        struct jxbx_loc *but_loc = NULL; 

        but_loc = (struct jxbx_loc *)jmalloc(sizeof (struct jxbx_loc), M_ZERO);
        if (!but_loc)
        {
            DBG_E(DHOST_HID_GENERAL,("Out of memory \n"));
            goto Error;
        }

        but_loc->loc_usage = t; 

        rc = hid_general_get_field_location(hid_gen_dev_ctx, 
            HID_USAGE2(HUP_BUTTON, (t+1)), UHID_INPUT_REPORT, 0, 
            &but_loc->location);
        if (rc)
        {
            DBG_E(DHOST_HID_GENERAL,
                ("Error during parsing of the report descriptor for "
                "Button_%d: %d\n", (t+1), rc));
            jfree(but_loc);
            goto Error;
        }

        TAILQ_INSERT_TAIL(&sc->buttons_queue, but_loc, next_loc);
    }

    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_BYTE_COUNT, UHID_OUTPUT_REPORT, 0, 
        sc->motor_byte_count);
    GET_HID_LOC(HUP_MOTOR, HUM_BIG_MOTOR, UHID_OUTPUT_REPORT, 0, sc->big_motor);
    GET_HID_LOC(HUP_MOTOR, HUM_LITTLE_MOTOR, UHID_OUTPUT_REPORT, 0, 
        sc->little_motor);

    GET_HID_LOC(HUP_GENERIC_DESKTOP, HUG_BYTE_COUNT, UHID_OUTPUT_REPORT, 1, 
        sc->leds_byte_count);
    GET_HID_LOC(HUP_LEDS, HUL_GENERIC_IND, UHID_OUTPUT_REPORT, 1, sc->leds_loc);

    sc->hid_handler = hid_gen_dev_ctx;
    sc->input_device.enable_cb = jxbox360_enable;
    sc->input_device.disable_cb = jxbox360_disable;
    sc->input_device.get_out_report_size_cb = jxbox360_rep_size;
    sc->input_device.set_report_field_cb = jxbox360_set_field;
    sc->input_device.send_out_cb = jxbox360_send_rep;
    sc->input_device.set_idle_cb = NULL;
    sc->input_device.get_idle_cb = NULL;
    sc->input_device.set_protocol_cb = NULL;
    sc->input_device.get_protocol_cb = NULL;
    sc->input_device.query_usage_att_cb = jxbox360_get_usg_att;
    sc->input_device.get_max_buttons_cb = jxbox360_get_max_but;
    sc->input_device.get_usbd_handle_cb = jxbox360_get_usbd_handle;
    sc->input_device.name = jxbox360_name;
    sc->input_device.device_type = HID_XBOX;
    sc->input_device.idvendor = 0;
    sc->input_device.idproduct = 0;
    sc->input_device.idversion = 0;
    sc->input_device.app_dev_ctx = sc;

    rc = jtask_init(&sc->out_task, TASK_CLASS_DRIVER);
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL, ("XBOX ATTACH: Cannot initialize task %d\n",
            rc));
        goto Error;
    }

    rc = register_input_device(&sc->input_device, &sc->os_input_dev, 
        &sc->os_in_ops);
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL, ("XBOX Gamepad device Failed to register %d\n",
            rc));
        goto Error;
    }

    /* Check existance of needed os callbacks */
    if ((!sc->os_in_ops.unregister_input_device) ||
        (!sc->os_in_ops.report_event) ||
        (!sc->os_in_ops.report_absolute_data))
    {
        DBG_E(DHOST_HID_GENERAL, ("Xbox device os_callbacks absent \n"));
        goto Error;
    }
    
    DBG_I(DHOST_HID_GENERAL, ("XBOX Gamepad Attached\n"));
    return 1;

Error:
  
    jxbox360_detach(app_ctx, hid_gen_dev_ctx);
    return 0;
}
    
static void jxbox360_detach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jxbox360_softc_t *sc = find_jxbx_dev(app_ctx, hid_gen_dev_ctx);
    struct _xbox360_head *list_head = (struct _xbox360_head*)app_ctx;
    struct jxbx_loc *but_loc;

    if (!sc)
        goto Exit;

    if (sc->os_in_ops.unregister_input_device && sc->os_input_dev)
    {
        /* Disable xbox360 before unregistering*/
        jxbox360_disable(sc);
        sc->os_in_ops.unregister_input_device(sc->os_input_dev);
        sc->os_input_dev = NULL;
    }

    /* Remove leds task if exists */
    if (sc->out_task)
        jtask_uninit(sc->out_task); 

    /* Free allocated locations */
    FREE_HID_LOC(sc->x_loc);
    FREE_HID_LOC(sc->y_loc);
    FREE_HID_LOC(sc->z_loc);

    FREE_HID_LOC(sc->rx_loc);
    FREE_HID_LOC(sc->ry_loc);
    FREE_HID_LOC(sc->rz_loc);

    FREE_HID_LOC(sc->dpad_up);
    FREE_HID_LOC(sc->dpad_down);
    FREE_HID_LOC(sc->dpad_left);
    FREE_HID_LOC(sc->dpad_right);

    FREE_HID_LOC(sc->motor_byte_count);
    FREE_HID_LOC(sc->big_motor);
    FREE_HID_LOC(sc->little_motor);

    FREE_HID_LOC(sc->leds_loc);
    FREE_HID_LOC(sc->leds_byte_count);

    but_loc = TAILQ_FIRST(&sc->buttons_queue);
    while (but_loc)
    {
        hid_general_free_location(&but_loc->location);
        TAILQ_REMOVE(&sc->buttons_queue, but_loc, next_loc);
        jfree(but_loc);
        but_loc = TAILQ_FIRST(&sc->buttons_queue);
    }

    TAILQ_REMOVE(list_head, sc, next_dev);
    jfree(sc);

Exit:
    return;
}

static void jxbox360_complete(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx, juint8_t rep_id, juint32_t report_size)
{
    usage_att_t att;
    struct jxbx_loc *but_loc;
    jxbox360_softc_t *sc = find_jxbx_dev(app_ctx, hid_gen_dev_ctx);
    juint8_t *buf = (juint8_t *)jmalloc(report_size, M_ZERO);

    if (!buf || !sc)
        goto Exit;

    if (hid_general_read(sc->hid_handler, (void *)buf, rep_id))
        goto Exit;

    hid_general_get_att_from_location(sc->x_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 0,
        hid_general_get_field_value(sc->x_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->y_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 1,
        hid_general_get_field_value(sc->y_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->z_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 2,
        hid_general_get_field_value(sc->z_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->rx_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 3,
        hid_general_get_field_value(sc->rx_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->ry_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 4,
        hid_general_get_field_value(sc->ry_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->rz_loc, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 5,
        hid_general_get_field_value(sc->rz_loc,(void *)buf), &att);

    hid_general_get_att_from_location(sc->dpad_up, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 6,
        hid_general_get_field_value(sc->dpad_up,(void *)buf), &att);

    hid_general_get_att_from_location(sc->dpad_down, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 7,
        hid_general_get_field_value(sc->dpad_down,(void *)buf), &att);

    hid_general_get_att_from_location(sc->dpad_left, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 8,
        hid_general_get_field_value(sc->dpad_left,(void *)buf), &att);

    hid_general_get_att_from_location(sc->dpad_right, &att);
    sc->os_in_ops.report_absolute_data(sc->os_input_dev, 9,
        hid_general_get_field_value(sc->dpad_right,(void *)buf), &att);

    TAILQ_FOREACH(but_loc, &sc->buttons_queue, next_loc)
    {
        hid_general_get_att_from_location(but_loc->location, &att);
        sc->os_in_ops.report_event(sc->os_input_dev, but_loc->loc_usage, 
            hid_general_get_field_value(but_loc->location,(void *)buf), &att);
    }

Exit:
    if (buf)
        jfree(buf);
    return;
}

static notify_t xbox360_notify_callbacks = {
    jxbox360_attach,
    jxbox360_detach,
    jxbox360_complete
};

jresult_t jxbox360_init(void)
{
    TAILQ_INIT(&jxbox360_dev_queue);
    if (hid_general_register(&xbox360_notify_callbacks, &jxbox360_dev_queue))
    {
        DBG_E(DHOST_HID_GENERAL, ("fail to initialize\n"));
        return JEINVAL; 
    }
    DBG_I(DHOST_HID_GENERAL, ("\nXbox360 Gamepad driver registered \n"));
    return 0;
}

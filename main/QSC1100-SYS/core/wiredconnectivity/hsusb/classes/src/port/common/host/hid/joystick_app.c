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

struct out_args 
{
    hid_gen_dev_ctx_t hid_handler;
    juint8_t *data;
    juint8_t report_id;
};

struct jstk_loc
{
    juint32_t loc_usage;
    void *location;
    TAILQ_ENTRY(jstk_loc) next_loc;
};

typedef struct jjoystick_softc
{
    hid_gen_dev_ctx_t            hid_handler;
    input_device_reg_t          input_device;
    struct out_args             oa;
    jtask_h                     out_task;
    os_input_ops_t              os_in_ops;
    os_input_ctx_t              os_input_dev;
    void                        *x_loc;
    void                        *y_loc;
    void                        *z_loc;
    void                        *hat_switch_loc;
    void                        *throttle_loc;
    TAILQ_HEAD(_jstk_loc_head, jstk_loc) buttons_queue;
    jbool_t is_enabled;
    TAILQ_ENTRY(jjoystick_softc) next_dev;
}jjoystick_softc_t;

static TAILQ_HEAD(_joystick_head, jjoystick_softc) jjoystick_dev_queue;

char jjoystick_name[]={'H','I','D', ' ',
    'J', 'o', 'y', 's', 't', 'i', 'c', 'k', '\0'};

static jjoystick_softc_t *find_jstk_dev(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jjoystick_softc_t *dev;
    struct _joystick_head *list_head = (struct _joystick_head*)app_ctx;

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
}

static jresult_t jjoystick_enable(hid_app_dev_ctx_t app_dev_ctx)
{
    jresult_t rc = 0;
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        DBG_I(DHOST_HID_GENERAL, ("jjoystick_enable: Trying to enable an "
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

static void jjoystick_disable(hid_app_dev_ctx_t app_dev_ctx)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        /* Remove out task if exists */
        jtask_stop(sc->out_task);

        hid_general_close(sc->hid_handler);
        sc->is_enabled = FALSE;
    }
    else
    {
        DBG_I(DHOST_HID_GENERAL, ("jjoystick_disable: Trying to disable an "
            "already disabled device\n"));
    }
}

static jresult_t jjoystick_set_idle(hid_app_dev_ctx_t app_dev_ctx, juint8_t value)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    return hid_general_set_idle(sc->hid_handler, 0, value);
}

static jresult_t jjoystick_get_idle(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t *value)
{
    jresult_t rc = 0;
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    
    rc = hid_general_get_idle(sc->hid_handler, 0, value);
    if (rc)
        *value = 0;
    return rc;
}

static jresult_t jjoystick_set_protocol(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t value)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    return hid_general_set_protocol(sc->hid_handler, value);
}

static jresult_t jjoystick_get_protocol(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t *value)
{
    jresult_t rc = 0;
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    
    rc = hid_general_get_protocol(sc->hid_handler, value);
    if (rc)
        *value = 0;
    return rc;
}

static jresult_t jjoystick_get_usg_att(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, usage_att_t *att)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    jresult_t rc = JENOENT;

    j_memset(att, 0, sizeof(usage_att_t));

    switch (usage)
    {
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X):
            hid_general_get_att_from_location(sc->x_loc, att);
            rc = 0;
            break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y):
            hid_general_get_att_from_location(sc->y_loc, att);
            rc = 0;
            break;
     case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z):
            if (sc->z_loc)
            {
                hid_general_get_att_from_location(sc->z_loc, att);
                rc = 0;
            }
            break;
    case HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_HAT_SWITCH):
            if (sc->hat_switch_loc)
            {
                hid_general_get_att_from_location(sc->hat_switch_loc, att);
                rc = 0;
            }
            break;
    case HID_USAGE2(HUP_SIMULATION, HUG_HAT_SWITCH):
            if (sc->throttle_loc)
            {
                hid_general_get_att_from_location(sc->throttle_loc, att);
                rc = 0;
            }
            break;

    default:
            if (HID_USAGE_PAGE(usage) == HUP_BUTTON)
            {
                struct jstk_loc *but_loc;
                TAILQ_FOREACH(but_loc, &sc->buttons_queue, next_loc)
                {
                    if (but_loc->loc_usage == HID_USAGE_USAGE(usage))
                    {
                        hid_general_get_att_from_location(but_loc->location, 
                            att);
                        rc = 0;
                    }
                }
            }
    }
    
    return rc;
}

static juint32_t jjoystick_rep_size(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t report_id)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;

    return hid_general_get_report_size(sc->hid_handler, report_id, 
        UHID_OUTPUT_REPORT);
}

static jresult_t jjoystick_set_field(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, juint32_t value, juint8_t *buf, juint8_t report_id)
{
    jresult_t rc = JENOENT;
    return rc;
}

static jresult_t jjoystick_send_rep(void *app_dev_ctx, juint8_t *data, 
    juint8_t size, juint8_t report_id)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;
    
    sc->oa.hid_handler = sc->hid_handler;
    sc->oa.report_id = report_id;
    
    sc->oa.data = jmalloc(size, M_ZERO);
    if(!sc->oa.data)
        return JENOMEM;
    
    j_memcpy(sc->oa.data, data, size);

    jtask_schedule(sc->out_task, 0, actual_send_report, &(sc->oa));
    return 0;
}

static juint32_t jjoystick_get_max_but(hid_app_dev_ctx_t app_dev_ctx)
{
    juint32_t max_but = 0;
    usage_att_t att;
    if (!jjoystick_get_usg_att(app_dev_ctx, HID_USAGE2(HUP_BUTTON, 1), &att))
        max_but = att.usage_max - att.usage_min + 1;

    return max_but;
}

static void *jjoystick_get_usbd_handle(hid_app_dev_ctx_t app_dev_ctx)
{
    jjoystick_softc_t *sc = (jjoystick_softc_t *)app_dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL, ("jjoystick_get_usbd_handle: received invalid "
            "device handle\n"));
        return NULL;
    }
 
    return hid_general_get_usbd_handle(sc->hid_handler);
}

static void jjoystick_detach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx);

static jbool_t jjoystick_attach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jresult_t rc;
    juint_t t;
    struct _joystick_head *list_head = (struct _joystick_head*)app_ctx;
    struct jstk_loc *but_loc = NULL; 
    
    jjoystick_softc_t *sc = NULL;
    
    if (!hid_general_is_collection(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_JOYSTICK)))
    {
        goto Error;
    }

    sc = (jjoystick_softc_t*)jmalloc(sizeof(jjoystick_softc_t), 
        M_ZERO);
    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL,("Attach joystick: Out of memory!\n"));
        goto Error; 
    }
    
    /* add device to the list*/
    TAILQ_INSERT_TAIL(list_head, sc, next_dev);

    rc = hid_general_get_field_location(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X),
        UHID_INPUT_REPORT, 0, &sc->x_loc);
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL,
            ("Error during parsing of the report descriptor for X :%d\n", rc));
        goto Error;
    }
   
    rc = hid_general_get_field_location(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y),
        UHID_INPUT_REPORT, 0, &sc->y_loc);
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL,
            ("Error during parsing of the report descriptor for Y: %d\n", rc));
        goto Error;
    }

    rc = hid_general_get_field_location(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z),
        UHID_INPUT_REPORT, 0, &sc->z_loc);
    if (rc)
        sc->z_loc = NULL;
    
    rc = hid_general_get_field_location(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_HAT_SWITCH),
        UHID_INPUT_REPORT, 0, &sc->hat_switch_loc);
    if (rc)
        sc->hat_switch_loc = NULL; /*no hat_switch found */

    rc = hid_general_get_field_location(hid_gen_dev_ctx, 
        HID_USAGE2(HUP_SIMULATION, HUS_THROTTLE),
        UHID_INPUT_REPORT, 0, &sc->throttle_loc);
    if (rc)
        sc->throttle_loc = NULL; /*no throttle found */

    TAILQ_INIT(&sc->buttons_queue);

    t = 0; /* First Button */

    do
    {
        but_loc = (struct jstk_loc *)jmalloc(sizeof (struct jstk_loc), M_ZERO);
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
#ifdef JDEBUG
            if (!t)
            {
                DBG_E(DHOST_HID_GENERAL, ("Joystick without buttons !\n"));
            }
#endif
            jfree(but_loc);
            break;
        }

        TAILQ_INSERT_TAIL(&sc->buttons_queue, but_loc, next_loc);
        t++;

    } while(1); 

    sc->hid_handler = hid_gen_dev_ctx;
    sc->input_device.enable_cb = jjoystick_enable;
    sc->input_device.disable_cb = jjoystick_disable;
    sc->input_device.get_out_report_size_cb = jjoystick_rep_size;
    sc->input_device.set_report_field_cb = jjoystick_set_field;
    sc->input_device.send_out_cb = jjoystick_send_rep;
    sc->input_device.set_idle_cb = jjoystick_set_idle;
    sc->input_device.get_idle_cb = jjoystick_get_idle;
    sc->input_device.set_protocol_cb = jjoystick_set_protocol;
    sc->input_device.get_protocol_cb = jjoystick_get_protocol;
    sc->input_device.query_usage_att_cb = jjoystick_get_usg_att;
    sc->input_device.get_max_buttons_cb = jjoystick_get_max_but;
    sc->input_device.get_usbd_handle_cb = jjoystick_get_usbd_handle;
    sc->input_device.name = jjoystick_name;
    sc->input_device.device_type = HID_JOYSTICK;
    sc->input_device.idvendor = 0;
    sc->input_device.idproduct = 0;
    sc->input_device.idversion = 0;
    sc->input_device.app_dev_ctx = sc;

    rc = jtask_init(&sc->out_task, TASK_CLASS_DRIVER);
    if (rc)
    {
        DBG_E(DHOST_HID_GENERAL, ("JSTK ATTACH: Cannot initialize task %d\n",
            rc));
        goto Error;
    }

    rc = register_input_device(&sc->input_device, &sc->os_input_dev, 
        &sc->os_in_ops);
    if (rc)
    {
        jprintf("Joystick device Failed to register %d\n", rc);
        goto Error;
    }

    /* Check existance of needed os callbacks */
    if ((!sc->os_in_ops.unregister_input_device) ||
        (!sc->os_in_ops.report_event) ||
        (!sc->os_in_ops.report_absolute_data) ||
        (!sc->os_in_ops.report_relative_data))
    {
        jprintf("Joystick device os_callbacks absent \n");
        goto Error;
    }

    jjoystick_set_idle((hid_app_dev_ctx_t)sc, 0);
    jprintf("Joystick Attached\n");
    return 1;

Error:
  
    jjoystick_detach(app_ctx, hid_gen_dev_ctx);
    return 0;
}
    
static void jjoystick_detach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx)
{
    jjoystick_softc_t *sc = find_jstk_dev(app_ctx, hid_gen_dev_ctx);
    struct _joystick_head *list_head = (struct _joystick_head*)app_ctx;
    struct jstk_loc *but_loc;
 
    if (!sc)
        goto Exit;
    
    if (sc->os_in_ops.unregister_input_device && sc->os_input_dev)
    {
        /* Disable joystick before unregistering*/
        jjoystick_disable(sc);
        sc->os_in_ops.unregister_input_device(sc->os_input_dev);
        sc->os_input_dev = NULL;
    }

    /* Remove leds task if exists */
    if (sc->out_task)
        jtask_uninit(sc->out_task); 

    if (sc->x_loc)
        hid_general_free_location(&sc->x_loc);
    if (sc->y_loc)
        hid_general_free_location(&sc->y_loc);
    if (sc->z_loc)
        hid_general_free_location(&sc->z_loc);
    if (sc->throttle_loc)
        hid_general_free_location(&sc->throttle_loc);
    if (sc->hat_switch_loc)
        hid_general_free_location(&sc->hat_switch_loc);

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
    return ;
}

static void jjoystick_complete(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t hid_gen_dev_ctx, juint8_t rep_id, juint32_t report_size)
{
    usage_att_t att;
    struct jstk_loc *but_loc;
    
    jjoystick_softc_t *sc = find_jstk_dev(app_ctx, hid_gen_dev_ctx);
   
    juint8_t *buf = (juint8_t *)jmalloc(report_size, M_ZERO);

    if (!buf || !sc)
        goto Exit;

    if (hid_general_read(sc->hid_handler, (void *)buf, rep_id))
        goto Exit;

    hid_general_get_att_from_location(sc->x_loc, &att);
    if (att.flags & HIO_RELATIVE)
    {
        sc->os_in_ops.report_relative_data(sc->os_input_dev,
            0, hid_general_get_field_value(sc->x_loc,(void *)buf), &att);
    }
    else 
    {
        sc->os_in_ops.report_absolute_data(sc->os_input_dev,
            0, hid_general_get_field_value(sc->x_loc,(void *)buf), &att);
    }

    hid_general_get_att_from_location(sc->y_loc, &att);
    if (att.flags & HIO_RELATIVE)
    {
        sc->os_in_ops.report_relative_data(sc->os_input_dev,
            1, hid_general_get_field_value(sc->y_loc,(void *)buf), &att);
    }
    else
    {
        sc->os_in_ops.report_absolute_data(sc->os_input_dev,
            1, hid_general_get_field_value(sc->y_loc,(void *)buf), &att);
    }

    if (sc->z_loc)
    {
        hid_general_get_att_from_location(sc->z_loc, &att);
        if (att.flags & HIO_RELATIVE)
        {
            sc->os_in_ops.report_relative_data(sc->os_input_dev,
                2, hid_general_get_field_value(sc->z_loc,(void *)buf), &att);
        }
        else
        {
            sc->os_in_ops.report_absolute_data(sc->os_input_dev,
                2, hid_general_get_field_value(sc->z_loc,(void *)buf), &att);
        }
    }

    if (sc->hat_switch_loc)
    {
        hid_general_get_att_from_location(sc->hat_switch_loc, &att);
        if (att.flags & HIO_RELATIVE)
        {
            sc->os_in_ops.report_relative_data(sc->os_input_dev,
                4, hid_general_get_field_value(sc->hat_switch_loc,(void *)buf), 
                &att);
        }
        else
        {
            sc->os_in_ops.report_absolute_data(sc->os_input_dev,
                4, hid_general_get_field_value(sc->hat_switch_loc,(void *)buf), 
                &att);
        }
    }

    if (sc->throttle_loc)
    {
        hid_general_get_att_from_location(sc->throttle_loc, &att);
        if (att.flags & HIO_RELATIVE)
        {
            sc->os_in_ops.report_relative_data(sc->os_input_dev,
                3, hid_general_get_field_value(sc->throttle_loc,(void *)buf), 
                &att);
        }
        else
        {
            sc->os_in_ops.report_absolute_data(sc->os_input_dev,
                3, hid_general_get_field_value(sc->throttle_loc,(void *)buf), 
                &att);
        }
    }

    TAILQ_FOREACH(but_loc, &sc->buttons_queue, next_loc)
    {
        hid_general_get_att_from_location(but_loc->location, &att);
        sc->os_in_ops.report_event(sc->os_input_dev,
            but_loc->loc_usage, 
            hid_general_get_field_value(but_loc->location,(void *)buf), &att);
    }

Exit:
    if(buf)
        jfree(buf);
    return;
}

static notify_t joystick_notify_callbacks = {
    jjoystick_attach,
    jjoystick_detach,
    jjoystick_complete
};

jresult_t jjoystick_init(void)
{
    TAILQ_INIT(&jjoystick_dev_queue);
    if (hid_general_register(&joystick_notify_callbacks, &jjoystick_dev_queue))
    {
        DBG_E(DHOST_HID_GENERAL, ("fail to initialize\n"));
        return JEINVAL; 
    }
    DBG_I(DHOST_HID_GENERAL, ("\nJoystick driver registered \n"));
    return 0;
}

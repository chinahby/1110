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
#include "includes.h"
#include <input.h>
#include "jdebug.h"
#include "port.h"
#include <os_port.h>
#include <host_hid_app.h>

#if defined CONFIG_JHOST_HID_KBD || defined CONFIG_JHOST_HID_KEYBOARD
extern juint8_t jusb_kbd_keycode[256];
#endif

static jresult_t device_open(struct input_dev *dev);
static void device_close(struct input_dev *dev);
static jint_t device_event(struct input_dev *dev, juint_t type, juint_t code, 
    jint_t value);

/* input callbacks */
static void unregister_input_device (os_input_ctx_t os_ctx);
static void report_event (os_input_ctx_t os_ctx, juint8_t keycode, juint8_t event,
    usage_att_t *att);
static void report_relative_data (os_input_ctx_t os_ctx, juint8_t keycode, 
    jint32_t data, usage_att_t *att);
static void report_absolute_data (os_input_ctx_t os_ctx, juint8_t keycode, 
    juint32_t data, usage_att_t *att);

jresult_t register_input_device (input_device_reg_t *input_device,
    os_input_ctx_t *os_ctx, os_input_ops_t *os_ops)
{
    jresult_t rt = 0;
    struct input_dev *dev_p = NULL; 
    
    if (!input_device || !os_ops)
    {
        rt = JEINVAL;
        DBG_E(DHOST_HID, ("Invalid params: in_dev: %p os_ops: %p", 
            input_device, os_ops));
        goto Exit;
    }
    
    dev_p = *os_ctx = (struct input_dev *)jmalloc(sizeof(struct input_dev), 
        M_ZERO);
    if (!(*os_ctx))
    {
        rt = JENOMEM;
        DBG_E(DHOST_HID,("Out of memory: Unable to create the input device"));
        goto Exit;
    }
    
    dev_p->private = input_device;
    dev_p->name = input_device->name;
    dev_p->id.bustype = BUS_USB;
    dev_p->id.vendor = input_device->idvendor;
    dev_p->id.product = input_device->idproduct;
    dev_p->id.version = input_device->idversion;

    dev_p->open = device_open;
    dev_p->close = device_close;
    dev_p->event = device_event;
    
    switch (input_device->device_type)
    {
    case HID_KEYBOARD:
#if defined CONFIG_JHOST_HID_KBD || defined CONFIG_JHOST_HID_KEYBOARD
        {
            jint_t i;
            dev_p->evbit[0] = BIT(EV_KEY) | BIT(EV_LED) | BIT(EV_REP);
            dev_p->ledbit[0] = BIT(LED_NUML) | BIT(LED_CAPSL) | 
                BIT(LED_SCROLLL) | BIT(LED_COMPOSE) | BIT(LED_KANA);

            for (i = 0; i < 255; i++)
                set_bit(jusb_kbd_keycode[i], dev_p->keybit);
            clear_bit(0, dev_p->keybit);
        }
#endif
        break;
    case HID_MOUSE:
        {
            jint_t i;
            juint32_t max_but = 
                input_device->get_max_buttons_cb(input_device->app_dev_ctx);
            dev_p->evbit[0] = BIT(EV_KEY) | BIT(EV_REL);
           
            dev_p->keybit[LONG(BTN_MOUSE)] = 0;
            for (i = BTN_LEFT; max_but > 0; i++, max_but --)
                dev_p->keybit[LONG(BTN_MOUSE)] |= BIT(i);
            
            dev_p->relbit[0] = BIT(REL_X) | BIT(REL_Y) | BIT(REL_WHEEL);
        }
        break;
    case HID_JOYSTICK:
        {
            jint_t i;
            usage_att_t att;
            juint32_t max_but = 
                input_device->get_max_buttons_cb(input_device->app_dev_ctx);
            dev_p->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
           
            dev_p->keybit[LONG(BTN_MISC)] = 0;
            for (i = BTN_0; max_but > 0; i++, max_but--)
                dev_p->keybit[LONG(BTN_MISC)] |= BIT(i);
            
            dev_p->absbit[0] = 0;

            input_device->query_usage_att_cb(input_device->app_dev_ctx,
                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X), &att);

            dev_p->absmin[ABS_X] = att.logical_min;
            dev_p->absmax[ABS_X] = att.logical_max;
            dev_p->absfuzz[ABS_X] = (att.logical_max - att.logical_min) >> 8;
            dev_p->absflat[ABS_X] = (att.logical_max - att.logical_min) >> 4;
            dev_p->absbit[0] |= BIT(ABS_X);

            input_device->query_usage_att_cb(input_device->app_dev_ctx,
                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y), &att);
            
            dev_p->absmin[ABS_Y] = att.logical_min;
            dev_p->absmax[ABS_Y] = att.logical_max;
            dev_p->absfuzz[ABS_Y] = (att.logical_max - att.logical_min) >> 8;
            dev_p->absflat[ABS_Y] = (att.logical_max - att.logical_min) >> 4;
            dev_p->absbit[0] |= BIT(ABS_Y);
           
            if (!input_device->query_usage_att_cb(input_device->app_dev_ctx,
                HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Z), &att))
            {
                dev_p->absmin[ABS_Z] = att.logical_min;  
                dev_p->absmax[ABS_Z] = att.logical_max;
                dev_p->absfuzz[ABS_Z] = 
                    (att.logical_max - att.logical_min) >> 8;
                dev_p->absflat[ABS_Z] = 
                    (att.logical_max - att.logical_min) >> 4;
                dev_p->absbit[0] |= BIT(ABS_Z);
            }
        }
        break;
    default:
        DBG_E(DHOST_HID, ("Not Supported Device type 0x%x \n", 
            input_device->device_type));
        rt = JEINVAL;
        goto Exit;
    }

    os_ops->unregister_input_device = unregister_input_device;
    os_ops->report_event = report_event;
    os_ops->report_relative_data = report_relative_data;
    os_ops->report_absolute_data = report_absolute_data;
    
    input_register_device(dev_p);
    
Exit:
    if (rt && dev_p)
    {
        jfree(dev_p);
        *os_ctx = NULL;
    }
    return rt;
}

static void unregister_input_device (os_input_ctx_t os_ctx)
{
    input_unregister_device (os_ctx);
    jfree(os_ctx);
}

static void report_event (os_input_ctx_t os_ctx, juint8_t keycode, juint8_t event,
    usage_att_t *att)
{
    struct input_dev *dev_p = (struct input_dev *)os_ctx;
    input_device_reg_t *input_device = (input_device_reg_t *)dev_p->private;
    unsigned int kc;
    
    switch (input_device->device_type)
    {
    case HID_KEYBOARD:
        kc = keycode;
        break;
    case HID_MOUSE:
        kc = BTN_LEFT + keycode; 
        break;
    case HID_JOYSTICK:
        kc = BTN_0 + keycode; 
        break;
    default:
        return;
    }
    
    DBG_V(DHOST_HID_GENERAL,("The key %d - \t%s \n", kc, 
        event?"PRESSED":"Released"));
    input_report_key(dev_p, kc, event);
}

static void report_relative_data (os_input_ctx_t os_ctx, juint8_t keycode, 
    jint32_t data, usage_att_t *att)
{
    struct input_dev *dev_p = (struct input_dev *)os_ctx;
    input_device_reg_t *input_device = (input_device_reg_t *)dev_p->private;
    unsigned int kc;

    switch (input_device->device_type)
    {
    case HID_MOUSE:
    case HID_JOYSTICK:
        kc = REL_X + keycode; 
        break;
    default:
        return;
    }
   
    DBG_V(DHOST_HID,("The REL %d reported - \t%d \n", kc, data));
    input_report_rel(dev_p, kc, data);
}

static void report_absolute_data (os_input_ctx_t os_ctx, juint8_t keycode, 
    juint32_t data, usage_att_t *att)
{
    struct input_dev *dev_p = (struct input_dev *)os_ctx;
    input_device_reg_t *input_device = (input_device_reg_t *)dev_p->private;
    unsigned int kc;

    switch (input_device->device_type)
    {
    case HID_JOYSTICK:
        kc = ABS_X + keycode; 
        break;
    default:
        return;
    }
   
    DBG_V(DHOST_HID_GENERAL,("The ABS %d reported - \t%d \n", keycode, data));
    input_report_abs(dev_p, kc, data);
}

static jresult_t device_open(struct input_dev *dev)
{
    input_device_reg_t  *input_device = (input_device_reg_t  *)dev->private;
    jresult_t rt = 0;
    DBG_V(DHOST_HID,("Device Open\n"));
    if(input_device->enable_cb)
        rt = input_device->enable_cb (input_device->app_dev_ctx);
    
    return rt;
}

static void device_close(struct input_dev *dev)
{
    input_device_reg_t  *input_device = (input_device_reg_t  *)dev->private;
    DBG_V(DHOST_HID,("Device CLOSE\n"));
    if (input_device->disable_cb)
        input_device->disable_cb (input_device->app_dev_ctx);
}

static jint_t device_event(struct input_dev *dev, juint_t type, juint_t code, 
    jint_t value)
{
    input_device_reg_t  *input_device = (input_device_reg_t  *)dev->private;
    juint8_t leds = 0;
    jint_t rt = 0;
    DBG_V(DHOST_HID_GENERAL,("Device EVENT\n"));
    
    if (type != EV_LED)
    {
        rt = -1;
        goto Exit;
    }
       
    input_device->set_report_field_cb(input_device->app_dev_ctx,
        HID_USAGE2(HUP_LEDS,HUL_KANA), test_bit(LED_KANA, dev->led), &leds);

    input_device->set_report_field_cb(input_device->app_dev_ctx,
        HID_USAGE2(HUP_LEDS,HUL_COMPOSE), test_bit(LED_COMPOSE, dev->led), 
        &leds);

    input_device->set_report_field_cb(input_device->app_dev_ctx,
        HID_USAGE2(HUP_LEDS,HUL_SCROLL_LOCK), 
        test_bit(LED_SCROLLL, dev->led), &leds);

    input_device->set_report_field_cb(input_device->app_dev_ctx,
        HID_USAGE2(HUP_LEDS,HUL_CAPS_LOCK), 
        test_bit(LED_CAPSL, dev->led), &leds);

    input_device->set_report_field_cb(input_device->app_dev_ctx,
        HID_USAGE2(HUP_LEDS,HUL_NUM_LOCK), 
        test_bit(LED_NUML, dev->led), &leds);

    if (!input_device->send_out_cb)
        goto Exit;
    
    rt = input_device->send_out_cb(input_device->app_dev_ctx, &leds, 1);

Exit:
    return rt;
}

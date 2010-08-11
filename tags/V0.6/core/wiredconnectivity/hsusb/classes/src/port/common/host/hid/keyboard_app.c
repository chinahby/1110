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

#define MODIFIERS_INDEX 224
#define MAX_KEYS 6
#define KEY_ERROR 1
#define KEY_PRESSED 1
#define KEY_RELEASED 0
#define UPROTO_BOOT_KEYBOARD 1

struct jkeyboard_data {
    juint8_t     keycode[MAX_KEYS];
};

typedef enum 
{
    LEFT_CTRL = 0,
    LEFT_SHIFT,
    LEFT_ALT,
    LEFT_GUI,
    RIGHT_CTRL,
    RIGHT_SHIFT,
    RIGHT_ALT,
    RIGHT_GUI,
    KEYS_ARRAY,
    LED_NUM,
    LED_CAPS,
    LED_SCROLL,
    LED_COMPOSE,
    LED_KANA,
    LED_POWER,
    LED_SHIFT,
    MAX_KEYBOARD_LOCATIONS
} keyboard_loc_val_t;

struct led_args {
    hid_gen_dev_ctx_t hid_handler;
    juint8_t *leds;
    juint8_t report_id;
};

typedef struct jkeyboard_softc
{
    hid_gen_dev_ctx_t           hid_handler;
    struct jkeyboard_data       keys_new;
    struct jkeyboard_data       keys_old;
    input_device_reg_t          input_device;
    struct led_args             la;
    jtask_h                     leds_task;
    os_input_ops_t              os_in_ops;
    os_input_ctx_t              os_input_dev;
    void                        *locations[MAX_KEYBOARD_LOCATIONS];
    jbool_t is_enabled;
    TAILQ_ENTRY(jkeyboard_softc) next_dev;
} jkeyboard_softc_t;

static TAILQ_HEAD(_keyboard_head, jkeyboard_softc) jkeyboard_dev_queue;

juint8_t jusb_kbd_keycode[256] = {
          0,  0,  0,  0, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38,
         50, 49, 24, 25, 16, 19, 31, 20, 22, 47, 17, 45, 21, 44,  2,  3,
          4,  5,  6,  7,  8,  9, 10, 11, 28,  1, 14, 15, 57, 12, 13, 26,
         27, 43, 84, 39, 40, 41, 51, 52, 53, 58, 59, 60, 61, 62, 63, 64,
         65, 66, 67, 68, 87, 88, 99, 70,119,110,102,104,111,107,109,106,
        105,108,103, 69, 98, 55, 74, 78, 96, 79, 80, 81, 75, 76, 77, 71,
         72, 73, 82, 83, 86,127,116,117, 85, 89, 90, 91, 92, 93, 94, 95,
        120,121,122,123,134,138,130,132,128,129,131,137,133,135,136,113,
        115,114,  0,  0,  0,124,  0,181,182,183,184,185,186,187,188,189,
        190,191,192,193,194,195,196,197,198,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
          0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         29, 42, 56,125, 97, 54,100,126,164,166,165,163,161,115,114,113,
        150,158,159,128,136,177,178,176,142,152,173,140
};

static jbool_t is_present (juint8_t *start, juint8_t what, juint8_t size)
{
    jint_t j;
    jbool_t rt = 0;
    
    for (j = 0; j < size; ++j)
    {
        if (start[j] == what)
        {
            rt = 1;
            break;
        }
    }
    return rt;
}

static char jkeyboard_name[]={'H','I','D', ' ',
    'K', 'e', 'y', 'b', 'o', 'a', 'r', 'd', '\0'};

static jkeyboard_softc_t *find_kbd_dev(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    jkeyboard_softc_t *dev;
    struct _keyboard_head *list_head = (struct _keyboard_head*)app_ctx;

    TAILQ_FOREACH(dev, list_head, next_dev)
    {
        if (dev->hid_handler == device)
            return dev;
    }

    return NULL;
}

static void actual_set_leds(void *args)
{
    struct led_args *la = (struct led_args *)args;

    hid_general_write(la->hid_handler, la->leds, la->report_id, 
        UHID_OUTPUT_REPORT);
    jfree(la->leds);
}

static jresult_t jkeyboard_enable(hid_app_dev_ctx_t app_dev_ctx)
{
    jresult_t rc = 0;
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        DBG_I(DHOST_HID_GENERAL, ("jkeyboard_enable: Trying to enable an "
            "already enabled device\n"));
        goto Exit;
    }

    j_memset(&sc->keys_old, 0, sizeof(struct jkeyboard_data));
    j_memset(&sc->keys_new, 0, sizeof(struct jkeyboard_data));

    sc->hid_handler = hid_general_open(sc->hid_handler, 0);

    if (sc->hid_handler)
    {
        DBG_I(DHOST_HID_KBD,("Keyboard enabled\n"));
        sc->is_enabled = TRUE;
    }
    else
    {
        DBG_E(DHOST_HID_KBD,("Keyboard enable failed\n"));        
        rc = JEIO;
    }

Exit:
    return rc;
}

static void jkeyboard_disable(hid_app_dev_ctx_t app_dev_ctx)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    if (sc->is_enabled)
    {
        hid_general_close(sc->hid_handler);
        sc->is_enabled = FALSE;
    }
    else
    {
        DBG_I(DHOST_HID_GENERAL, ("jkeyboard_disable: Trying to disable an "
            "already disabled device\n"));
    }
}

static jresult_t jkeyboard_set_idle(hid_app_dev_ctx_t app_dev_ctx, juint8_t value)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    return hid_general_set_idle(sc->hid_handler, 0, value);
}

static jresult_t jkeyboard_get_idle(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t *value)
{
    jresult_t rc = 0;
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    rc = hid_general_get_idle(sc->hid_handler, 0, value);
    if (rc)
        *value = 0;
    return rc;
}

static jresult_t jkeyboard_set_protocol(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t value)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    return hid_general_set_protocol(sc->hid_handler, value);
}

static jresult_t jkeyboard_get_protocol(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t *value)
{
    jresult_t rc = 0;
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    rc = hid_general_get_protocol(sc->hid_handler, value);
    if (rc)
        *value = 0;
    return rc;
}

static jresult_t jkeyboard_get_usg_att(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, usage_att_t *att)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;
    jresult_t rc = JENOENT;
    jint_t i;

    j_memset(att, 0, sizeof(usage_att_t));
    for (i = 0; i < MAX_KEYBOARD_LOCATIONS && sc->locations[i]; ++i)
    {
        if (hid_general_is_location_of_usage(usage, sc->locations[i]))
        {
            hid_general_get_att_from_location(sc->locations[i], att);
            rc = 0;
            break;
        }
    }

    return rc;
}

static void *jkeyboard_get_usbd_handle(hid_app_dev_ctx_t app_dev_ctx)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL, ("jkeyboard_get_usbd_handle: received invalid "
            "device handle\n"));
        return NULL;
    }
 
    return hid_general_get_usbd_handle(sc->hid_handler);
}

static juint32_t jkeyboard_rep_size(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t report_id)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;

    return hid_general_get_report_size(sc->hid_handler, report_id, 
        UHID_OUTPUT_REPORT);
}

static jresult_t jkeyboard_set_field(hid_app_dev_ctx_t app_dev_ctx, 
    juint32_t usage, juint32_t value, juint8_t *buf, juint8_t report_id)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;
    jresult_t rc = JENOENT;
    jint_t i;

    for (i = 0; i < MAX_KEYBOARD_LOCATIONS && sc->locations[i]; ++i)
    {
        if (hid_general_is_location_of_usage(usage, sc->locations[i]))
        {
            rc = hid_general_set_field_value(sc->locations[i], buf, value);
            break;
        }
    }
    return rc;
}

static jresult_t jkeyboard_send_rep(hid_app_dev_ctx_t app_dev_ctx, 
    juint8_t *leds, juint8_t size, juint8_t report_id)
{
    jkeyboard_softc_t *sc = (jkeyboard_softc_t *)app_dev_ctx;
    
    sc->la.hid_handler = sc->hid_handler;
    sc->la.report_id = report_id;
    

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    sc->la.leds = jmalloc(size, M_ZERO);
    if (!sc->la.leds)
        return JENOMEM;
    
    j_memcpy(sc->la.leds, leds, size);

    jtask_schedule(sc->leds_task, 0, actual_set_leds, &(sc->la));
    return 0;
}

/* Begin – Qualcomm HSUSB addition  */ 
/* keyboard boot protocol report descriptor */
static const byte bp_kb_rd[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};
/* End – Qualcomm HSUSB addition  */ 

static jbool_t jkeyboard_attach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    jresult_t rc;
    keyboard_loc_val_t i;
    juint_t t;
    struct _keyboard_head *list_head = (struct _keyboard_head*)app_ctx;
    jkeyboard_softc_t *sc = NULL;
    
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    if (!hid_general_is_collection(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_KEYBOARD)))
    {
        goto Error;
    }

    sc = (jkeyboard_softc_t*)jmalloc(sizeof(jkeyboard_softc_t), M_ZERO);
    if (!sc)
    {
        DBG_E(DHOST_HID_KBD,("Attach keyboard: Out of memory!\n"));
        goto Error;
    }

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    /* Getting location for key array */
    /* Begin - Qualcomm HSUSB addition */
    /* Accoding to the boot protocol or keyboard and mouse, the report descriptor is predefined
     * and the report descritpor from the device shouldn't be parsed to get the location of the
     * items of usage. The location of the report items will be drived from the hard coded boot
     * protocol report descriptor. For more detail, refer to appendix B and F in Device Class Definition 
     * for Human Interface Devices (HID) Firmware Specification—6/27/01 Version 1.11  
     */
    rc = hid_general_get_bootprotocol_location((void *)bp_kb_rd, sizeof(bp_kb_rd),
           HID_USAGE2(HUP_KEYBOARD, 0),
           UHID_INPUT_REPORT, &sc->locations[KEYS_ARRAY]);
    /* End – Qualcomm HSUSB addition  */ 
    /* Blcok for Qualcomm addition
    rc = hid_general_get_field_location(device, HID_USAGE2(HUP_KEYBOARD, 0),
        UHID_INPUT_REPORT, report_id, &sc->locations[KEYS_ARRAY]);
    */
    if (rc) 
    {
        DBG_E(DHOST_HID_KBD, ("Attach keyboard: Failed to get location of "
            "KEYS!\n"));
        goto Error;
    }
    
    /* Getting locations for modifiers */
    for (i = LEFT_CTRL; i <= RIGHT_GUI; i++)
    {
      /* Begin – Qualcomm HSUSB addition  */ 
      hid_general_get_bootprotocol_location((void *)bp_kb_rd, sizeof(bp_kb_rd),
             HID_USAGE2(HUP_KEYBOARD,(HUK_LEFT_CTRL + i)), 
            UHID_INPUT_REPORT, &sc->locations[i]);
      /* End – Qualcomm HSUSB addition  */ 
      /* Block for Qualcomm addition
        hid_general_get_field_location(device, 
            HID_USAGE2(HUP_KEYBOARD,(HUK_LEFT_CTRL + i)), UHID_INPUT_REPORT, report_id,
            &sc->locations[i]);
      */
    }

    /* Geting location for LEDs */

    for (i = LED_NUM, t = 0; i <= LED_SHIFT; i++, t++)
    {
      /* Begin – Qualcomm HSUSB addition  */ 
      hid_general_get_bootprotocol_location((void *)bp_kb_rd, sizeof(bp_kb_rd),
            HID_USAGE2(HUP_LEDS, (HUL_NUM_LOCK + t)), 
            UHID_OUTPUT_REPORT, &sc->locations[i]);
      /* End – Qualcomm HSUSB addition  */ 
      /* Block for Qualcomm addition 
        hid_general_get_field_location(device,
            HID_USAGE2(HUP_LEDS, (HUL_NUM_LOCK + t)), UHID_OUTPUT_REPORT, report_id,
            &sc->locations[i]);
      */
    }

    sc->hid_handler = device;
    sc->input_device.enable_cb = jkeyboard_enable;
    sc->input_device.disable_cb = jkeyboard_disable;
    sc->input_device.get_out_report_size_cb = jkeyboard_rep_size;
    sc->input_device.set_report_field_cb = jkeyboard_set_field;
    sc->input_device.send_out_cb = jkeyboard_send_rep;
    sc->input_device.set_idle_cb = jkeyboard_set_idle;
    sc->input_device.get_idle_cb = jkeyboard_get_idle;
    sc->input_device.set_protocol_cb = jkeyboard_set_protocol;
    sc->input_device.get_protocol_cb = jkeyboard_get_protocol;
    sc->input_device.query_usage_att_cb = jkeyboard_get_usg_att;
    sc->input_device.get_max_buttons_cb = NULL;
    sc->input_device.get_usbd_handle_cb = jkeyboard_get_usbd_handle;
    sc->input_device.name = jkeyboard_name;
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else
    sc->input_device.device_type = HID_KEYBOARD;
#endif /* FEATURE_BT_EXTPF_HID_HOST */
    sc->input_device.idvendor = 0;
    sc->input_device.idproduct = 0;
    sc->input_device.idversion = 0;
    sc->input_device.app_dev_ctx = sc;
    
    rc = jtask_init(&sc->leds_task, TASK_CLASS_DRIVER);
    if (rc)
    {
        DBG_E(DHOST_HID_KBD, ("KBD ATTACH: Cannot initialize task %d\n", rc));
        goto Error;
    }

    TAILQ_INSERT_TAIL(list_head, sc, next_dev);
    rc = register_input_device(&sc->input_device, &sc->os_input_dev, 
        &sc->os_in_ops);
    if (rc)
    {
        jprintf("Keyboard device Failed to register %d\n", rc);
        TAILQ_REMOVE(list_head, sc, next_dev);
        goto Error;
    }

    /* Check existance of needed os callbacks */
    if ((!sc->os_in_ops.unregister_input_device) ||
        (!sc->os_in_ops.report_event))
    {
        jprintf("Keyboard device os_callbacks absent \n");
        TAILQ_REMOVE(list_head, sc, next_dev);
        goto Error;
    }

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
    {
        jkeyboard_set_idle((hid_app_dev_ctx_t)sc, 0);
        jkeyboard_set_protocol((hid_app_dev_ctx_t)sc, 0);
    }
 
    jprintf("Keyboard Attached\n");
    return 1;

Error:
    if (sc)
    {
        for (i = LEFT_CTRL; i < MAX_KEYBOARD_LOCATIONS; i++)
            hid_general_free_location(&sc->locations[i]);

        if (sc->leds_task)
            jtask_uninit(sc->leds_task);

        jfree(sc);
        sc = NULL;
    }
    
    return 0;
}
    
static void jkeyboard_detach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    jkeyboard_softc_t *sc = find_kbd_dev(app_ctx, device);
    keyboard_loc_val_t i;
    struct _keyboard_head *list_head = (struct _keyboard_head*)app_ctx;
 
    if (!sc)
    {
        DBG_E(DHOST_HID_GENERAL,("Device not found\n"));
        goto Exit;
    }
   
    /* Disable keyboard before unregistering*/
    jkeyboard_disable(sc);
    sc->os_in_ops.unregister_input_device(sc->os_input_dev);
    sc->os_input_dev = NULL;

    /* Remove leds task if exists */
    if (sc->leds_task)
        jtask_uninit(sc->leds_task);
 
    for (i = LEFT_CTRL; i < MAX_KEYBOARD_LOCATIONS; i++)
        hid_general_free_location(&sc->locations[i]);

    TAILQ_REMOVE(list_head, sc, next_dev);
    jfree(sc);

Exit:
    return ;
}

static void jkeyboard_complete(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device, juint8_t rep_id, juint32_t report_size)
{
    struct jkeyboard_data *new_data;
    struct jkeyboard_data *old_data;
    juint8_t key;
    juint_t j;
    usage_att_t att_keys;
    keyboard_loc_val_t i;
    jkeyboard_softc_t *sc = find_kbd_dev(app_ctx, device);
    juint8_t *buf = (juint8_t *)jmalloc(report_size, M_ZERO);

    if (!buf || !sc)
        goto Exit;

    if (hid_general_read(sc->hid_handler, (void *)buf, rep_id))
        goto Exit;

    /* Reporting for modifiers */
    for (i = LEFT_CTRL; i <= RIGHT_GUI; i++)
    {
        if (sc->locations[i])
        {
        #ifdef CANDLEBOX_FEATURE_USBHID
            juint8_t event;
            static boolean bCtrl, bShift, bAlt, bGui = 0;
        #endif /* CANDLEBOX_FEATURE_USBHID */
            usage_att_t att;
            hid_general_get_att_from_location(sc->locations[i], &att);
	      #ifdef CANDLEBOX_FEATURE_USBHID
            event = hid_general_get_field_value(sc->locations[i], (void *)buf);
            if((HUK_LEFT_CTRL == (att.usage & 0xffff)) || (HUK_RIGHT_CTRL == (att.usage &0xffff)))
            {
               if(0 == bCtrl)
               	{
               	   sc->os_in_ops.report_event(sc->os_input_dev,
                                      jusb_kbd_keycode[i + MODIFIERS_INDEX],
                                      hid_general_get_field_value(sc->locations[i], (void *)buf),
                                      &att);
               	   if (event == 1) bCtrl = 1;
               	}
            }

            else if((HUK_LEFT_SHIFT == (att.usage & 0xffff)) || (HUK_RIGHT_SHIFT == (att.usage & 0xffff)))
            {
               if(0 == bShift)
               	{
               	   sc->os_in_ops.report_event(sc->os_input_dev,
                                      jusb_kbd_keycode[i + MODIFIERS_INDEX],
                                      hid_general_get_field_value(sc->locations[i], (void *)buf),
                                      &att);
               	   if (event == 1) bShift = 1;
               	}
            }

            else if((HUK_LEFT_ALT == (att.usage & 0xffff)) || (HUK_RIGHT_ALT == (att.usage & 0xffff)))
            {
               if(0 == bAlt)
               	{
               	   sc->os_in_ops.report_event(sc->os_input_dev,
                                      jusb_kbd_keycode[i + MODIFIERS_INDEX],
                                      hid_general_get_field_value(sc->locations[i], (void *)buf),
                                      &att);
               	   if (event == 1) bAlt = 1;
               	}
            }

            else if((HUK_LEFT_GUI == (att.usage & 0xffff)) || (HUK_RIGHT_GUI == (att.usage & 0xffff)))
            {
               if(0 == bGui)
               	{
               	   sc->os_in_ops.report_event(sc->os_input_dev,
                                      jusb_kbd_keycode[i + MODIFIERS_INDEX],
                                      hid_general_get_field_value(sc->locations[i], (void *)buf),
                                      &att);
               	   if (event == 1) bGui = 1;
               	}
            }

            else
            {
               sc->os_in_ops.report_event(sc->os_input_dev,
                                      jusb_kbd_keycode[i + MODIFIERS_INDEX],
                                      hid_general_get_field_value(sc->locations[i], (void *)buf),
                                      &att);
            }

              if(RIGHT_GUI == i)
              {
                 bCtrl = 0;
                 bShift = 0;
                 bAlt = 0;
                 bGui = 0;
              }

	     	#else /* CANDLEBOX_FEATURE_USBHID */
            sc->os_in_ops.report_event(sc->os_input_dev,
                jusb_kbd_keycode[i + MODIFIERS_INDEX],
                hid_general_get_field_value(sc->locations[i], (void *)buf),
                &att);
		    #endif /* CANDLEBOX_FEATURE_USBHID */
        }    
    }

    new_data = &sc->keys_new;
    old_data = &sc->keys_old;

    j_memcpy(new_data, buf+2, sizeof(struct jkeyboard_data));
    hid_general_get_att_from_location(sc->locations[KEYS_ARRAY], &att_keys);

    if (new_data->keycode[0] == KEY_ERROR)
    {
        DBG_I(DHOST_HID_KBD,("JKBD_INTR: Keyboard report phantom state\n"));
        goto Exit; /* Ignore for now */
    }

    for (j = 0; j < MAX_KEYS; ++j)
    {
        key = old_data->keycode[j];
        if ((key > 3) && (!is_present(new_data->keycode, key, MAX_KEYS)))
        {
            att_keys.usage = HID_USAGE2(HUP_KEYBOARD, key);
            key = jusb_kbd_keycode[key];

            if (key)
            {
                sc->os_in_ops.report_event(sc->os_input_dev, key, KEY_RELEASED,
                    &att_keys);
            }
            else
            {
                DBG_E(DHOST_HID_KBD,("Unknown key (scancode %#x) released.\n",
                    key));
            }
        }
        key = new_data->keycode[j];
        
        if ((key > 3) && (!is_present(old_data->keycode, key, MAX_KEYS)))
        {
            att_keys.usage = HID_USAGE2(HUP_KEYBOARD, key);
            key = jusb_kbd_keycode[key];
            if (key)
            {
                sc->os_in_ops.report_event(sc->os_input_dev, key, KEY_PRESSED,
                    &att_keys);
            }
            else
            {
                DBG_E(DHOST_HID_KBD,("Unknown key (scancode %#x) pressed.\n",
                    key));
            }
        }
    }
    
    j_memcpy(old_data, new_data, sizeof(struct jkeyboard_data));    

Exit:
    if (buf)
        jfree(buf);
    return;
}

static notify_t keyboard_notify_callbacks = {
    jkeyboard_attach,
    jkeyboard_detach,
    jkeyboard_complete
};

jresult_t jkeyboard_init(void)
{
    TAILQ_INIT(&jkeyboard_dev_queue);
    if (hid_general_register(&keyboard_notify_callbacks, &jkeyboard_dev_queue))
    {
        DBG_E(DHOST_HID_GENERAL, ("fail to initialize\n"));
        return JEINVAL; 
    }
    DBG_I(DHOST_HID_GENERAL, ("\nKeyboard driver registered \n"));
    return 0;
}

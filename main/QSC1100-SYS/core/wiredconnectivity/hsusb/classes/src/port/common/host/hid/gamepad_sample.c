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
#include <uw_host_hid.h>

/* This demo application work with 12 Button Creative USB Gamepad */

static hid_gen_dev_ctx_t device_attached;
static jbool_t is_gamepad;
#define MAX_LOCATIONS 14 

static void *location[MAX_LOCATIONS];
enum gamepad_values
{
    x_ordinate = 0,
    y_ordinate,
    button1,
    button2,
    button3,
    button4,
    button5,
    button6,
    button7,
    button8,
    button9,
    button10,
    button11,
    button12
};

static jbool_t gamepad_local_attach(hid_gen_app_ctx_t app_ctx,
    hid_gen_dev_ctx_t device)
{
    enum gamepad_values i;
    jresult_t rc;
    juint_t t;

    rc = hid_general_get_field_location(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_X),
        UHID_INPUT_REPORT, 0, &location[x_ordinate]);
    if (rc)
    {
        jprintf("Error during parsing of the report descriptor for X :%d\n", 
            rc);
        goto Exit;
    }
    
    rc = hid_general_get_field_location(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_Y),
        UHID_INPUT_REPORT, 0, &location[y_ordinate]);
    if (rc)
    {
        jprintf("Error during parsing of the report descriptor for Y: %d\n", 
            rc);
        goto Exit;
    }
    
    for (i = button1; i <= button12; ++i)
    {
        t = (i - button1) + 1;
        rc = hid_general_get_field_location(device, 
            HID_USAGE2(HUP_BUTTON, t),
            UHID_INPUT_REPORT, 0, &location[i]);
        if (rc)
        {
            jprintf("Error during parsing of the report descriptor button "
                "no %d\n", t);
            goto Exit;
        }
    }
    
    device_attached = hid_general_open(device, 0);

Exit:
    if (!device_attached)
    {
        for (i = x_ordinate; i<=button12; ++i)
            hid_general_free_location(&location[i]);
    }
    else
        is_gamepad = 1; 
    return is_gamepad;
}

static jbool_t gamepad_attach(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device)
{
    jbool_t rc = 0;

    if (device_attached)
    {
        jprintf("Device already attached\n");
        goto Exit;
    }
    
    if (hid_general_is_collection(device, 
        HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_GAME_PAD)))
    {
        rc = gamepad_local_attach(app_ctx, device);
    }

Exit:
    return rc;
}

static void gamepad_detach(hid_gen_app_ctx_t app_ctx, hid_gen_dev_ctx_t device)
{
    jint_t i;
    
    hid_general_close(device);
    if (!device_attached)
        return;
    
    device_attached = NULL;
    
    for (i = 0; i<=MAX_LOCATIONS; ++i)
        hid_general_free_location(&location[i]);

    is_gamepad = 0;
}

static void gamepad_complete(hid_gen_app_ctx_t app_ctx, 
    hid_gen_dev_ctx_t device, juint8_t rep_id, juint32_t report_size)
{
    jresult_t rc;
    juint8_t *buf = (juint8_t *)jmalloc(report_size, M_ZERO);

    if (!buf)
        goto Exit;

    rc = hid_general_read(device, (void *)buf, rep_id);
    if (rc)
        goto Exit;
    if (is_gamepad)
    {
        jprintf("X: %ld, Y:%ld\n"
            "Buttons:\n"
            "1:%ld, 2 :%ld, 3 :%ld, 4 :%ld\n"
            "5:%ld, 6 :%ld, 7 :%ld, 8 :%ld\n"
            "9:%ld, 10:%ld, 11:%ld, 12:%ld\n",
            hid_general_get_field_value(location[x_ordinate],(void *)buf),
            hid_general_get_field_value(location[y_ordinate],(void *)buf),
            hid_general_get_field_value(location[button1],(void *)buf),
            hid_general_get_field_value(location[button2],(void *)buf),
            hid_general_get_field_value(location[button3],(void *)buf),
            hid_general_get_field_value(location[button4],(void *)buf),
            hid_general_get_field_value(location[button5],(void *)buf),
            hid_general_get_field_value(location[button6],(void *)buf),
            hid_general_get_field_value(location[button7],(void *)buf),
            hid_general_get_field_value(location[button8],(void *)buf),
            hid_general_get_field_value(location[button9],(void *)buf),
            hid_general_get_field_value(location[button10],(void *)buf),
            hid_general_get_field_value(location[button11],(void *)buf),
            hid_general_get_field_value(location[button12],(void *)buf));
    }

Exit:
    if (buf)
        jfree(buf);
    return;
}

static notify_t notify_callbacks = {
    gamepad_attach,
    gamepad_detach,
    gamepad_complete
};

void hid_generic_demo_start(void)
{
    if (hid_general_register(&notify_callbacks, NULL))
    {
        jprintf("fail to ininitialize\n");
        return; 
    }
    jprintf("\nApplication registered\n");
}


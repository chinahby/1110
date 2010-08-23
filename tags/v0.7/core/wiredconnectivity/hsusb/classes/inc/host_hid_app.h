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
/* OS Porting Layer HID Class Driver API */

#ifndef _HOST_HID_APP
#define _HOST_HID_APP

#include <uw_host_hid_common.h> /* Common HID Class Driver API */

/* Application device context */
typedef void *hid_app_dev_ctx_t;
/* Private porting layer data */
typedef void *os_input_ctx_t;

/* HID input device types */
typedef enum
{
    HID_UNDEF = 0,
    HID_KEYBOARD,
    HID_MOUSE,
    HID_GAMEPAD,
    HID_JOYSTICK,
    HID_XBOX,
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
} input_device_type_t;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/********************************************************
 * HID Class Driver Application -> OS Porting Layer API *
 ********************************************************/
/* HID input device registration information structure */
typedef struct
{
    /* HID class driver application device context */
    hid_app_dev_ctx_t app_dev_ctx;

    /* HID device properties */
    input_device_type_t device_type;
    char *name;
    juint16_t idvendor;
    juint16_t idproduct;
    juint16_t idversion;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

    /* Application Callbacks for use by the OS porting layer */

    /* Enables device
     * Requirement: Mandatory */
    jresult_t (*enable_cb)(hid_app_dev_ctx_t app_dev_ctx);

    /* Disables device
     * Requirement: Mandatory */
    void (*disable_cb)(hid_app_dev_ctx_t app_dev_ctx);

    /* Returns the size (in bytes) of the device's output report, or zero if
     * the device does not support output reports
     * Requirement: Mandatory */
    juint32_t (*get_out_report_size_cb)(hid_app_dev_ctx_t app_dev_ctx,
        juint8_t report_id);

    /* Sets output report field
     * Requirement: Mandatory for devices that support output reports
     *              (e.g. keyboards) */
    jresult_t (*set_report_field_cb)(hid_app_dev_ctx_t app_dev_ctx,
        juint32_t usage, juint32_t value, juint8_t *buf, juint8_t report_id);

    /* Sends output report
     * Requirement: Mandatory for devices that support output reports
     *              (e.g. keyboards) */
    jresult_t (*send_out_cb)(hid_app_dev_ctx_t app_dev_ctx, juint8_t *buf,
        juint8_t size, juint8_t report_id);

    /* Sets the device's idle rate
     * Requirement: Mandatory */
    jresult_t (*set_idle_cb)(hid_app_dev_ctx_t app_dev_ctx, juint8_t value);

    /* Gets the device's idle rate
     * Requirement: Mandatory */
    jresult_t (*get_idle_cb)(hid_app_dev_ctx_t app_dev_ctx, juint8_t *value);

    /* Sets the device's protocol (0=boot / 1=report)
     * Requirement: Optional */
    jresult_t (*set_protocol_cb)(hid_app_dev_ctx_t app_dev_ctx, juint8_t value);

    /* Gets the device's protocol (0=boot / 1=report)
     * Requirement: Optional */
    jresult_t (*get_protocol_cb)(hid_app_dev_ctx_t app_dev_ctx,
        juint8_t *value);

    /* Retrieves the specified usage attributes
     * Requirement: Mandatory */
    jresult_t (*query_usage_att_cb)(hid_app_dev_ctx_t app_dev_ctx,
        juint32_t usage, usage_att_t *att);

    /* Returns a device's maximum button count, or zero if the device has no
     * buttons usage
     * Requirement: Mandatory */
    juint32_t (*get_max_buttons_cb)(hid_app_dev_ctx_t app_dev_ctx);

    void *(*get_usbd_handle_cb)(hid_app_dev_ctx_t app_dev_ctx);
} input_device_reg_t;

/********************************************************
 * OS Porting Layer -> HID Class Driver Application API *
 ********************************************************/
/* OS porting layer HID class driver callbacks structure */
typedef struct
{
    /* Unregisters a HID input device
     * Requirement: Mandatory */
    void (*unregister_input_device)(os_input_ctx_t os_ctx);
    /* Report a device's key/button event to the OS
     * Requirement: Mandatory */
    void (*report_event)(os_input_ctx_t os_ctx, juint8_t keycode,
        juint8_t event, usage_att_t *att);
    /* Reports a device's relative data to the OS
     * Requirement: Mandatory if the device's report descriptor includes a
     *              boolean data state value (On/Off; Pressed/Released) - e.g.
     *              keyboard keys, mouse buttons, etc. */
    void (*report_relative_data)(os_input_ctx_t os_ctx, juint8_t data_type,
        jint32_t data, usage_att_t *att);
    /* Reports a device's absolute data to the OS
     * Requirement: Mandatory if the device's report descriptor includes a
     *              boolean data state value (On/Off; Pressed/Released) - e.g.
     *              keyboard keys, mouse buttons, etc. */
    void (*report_absolute_data)(os_input_ctx_t os_ctx, juint8_t data_type, 
        juint32_t data, usage_att_t *att);
} os_input_ops_t;

/* Registers a HID input device with the OS Porting Layer
 * Requirement: Mandatory */
jresult_t register_input_device(input_device_reg_t *input_dev,
    os_input_ctx_t *os_ctx, os_input_ops_t *ops);

#endif


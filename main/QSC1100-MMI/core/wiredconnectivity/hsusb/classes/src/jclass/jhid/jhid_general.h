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
#ifndef __JHID_GENERAL_H
#define __JHID_GENERAL_H

typedef enum
{
    HID_NOT_CONNECTED = 0,
    HID_CONNECTED,
    HID_ATTACHED,
    HID_ENABLED
} jhid_general_state_t;

typedef struct jhid_report
{
    jhid_report_desc_t report_descriptor;
    juint16_t report_descriptor_size;
    juint8_t report_id;
    juint16_t in_report_size;
    juint16_t out_report_size;
    juint16_t feature_report_size;
    juint8_t filled_index;
    juint8_t readed_index;
    void *in_report;
    TAILQ_ENTRY(jhid_report) next_report;
} jhid_report_t;

typedef struct jhid_gen_dev
{
    notify_t    *notify_callback;
    hid_gen_app_ctx_t context;
    TAILQ_ENTRY(jhid_gen_dev) next_handle;
} jhid_gen_dev_t;

#define JHID_NO_QUIRK            0x0
#define JHID_XBOX360_NO_REPORT_D 0x1
#define JHID_FORCE_BOOT_KBD      0x2
#define JHID_FORCE_BOOT_MOUSE    0x4

/* HSU Addition Begin */
typedef enum { INTF_USB, INTF_BT } hid_intf_type; 
/* HSU Addition End */

typedef struct jhid_general_softc
{
    /* HSU Addition Begin */
    hid_intf_type               intf_type;
    juint16_t                   conn_id;
    jtask_h                     bt_intr_task;
    /* HSU Addition End */
    jdevice_t                   sc_dev;
    usbd_device_handle          dev;
    usbd_interface_handle       iface;
    jhid_gen_dev_t              *hid_handle;
    juint8_t                     ep_addr_in;
    juint8_t                     ep_addr_out;
    usbd_pipe_handle            in_intr_pipe;  
    usbd_pipe_handle            out_intr_pipe;
    jhid_general_state_t        state;
    jhid_general_state_t        state_before_suspend;
    jbool_t                      is_suspended;
    juint8_t                     max_report_size;
    juint8_t                     *in_data;
    juint8_t                     num_of_reports;
    juint8_t                     open_flag;
    juint32_t                    hid_quirks;
    TAILQ_HEAD(dum1, jhid_report) report_list; 
    TAILQ_ENTRY(jhid_general_softc) next_device;

    jhid_report_t               boot_protocol_report;
    jbool_t                      is_boot_protocol;

#ifdef CONFIG_TEST_SELECTIVE_SUSPEND
    jtask_h                     power_task;
#endif
} jhid_general_softc_t;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif

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
/*
 * CDC General (jcdc_general)
 * Class driver library for various communication devices containing a
 * Communication Class Interface and one or more Data Class Interfaces.
 */

#ifndef _JCDC_GENERAL_H_
#define _JCDC_GENERAL_H_

#include <jos.h>
#include <usbdi_status.h>
#include <jusb.h>
#include <usbdivar.h>
#include <usbdi.h>
#include <usbdi_util.h>
#include <usb_quirks.h>
#include <uw_host_cdc.h>

#define CDC_DATA_XFER_TIMEOUT           0       /* Infinite Timeout */
#define CDC_CONTROL_XFER_TIMEOUT        2000
/* CDC specific USB codes*/
#define CDC_CLASS_CODE                  0x02
#define CDC_COMM_IFACE_CLASS_CODE       0x02    /* 0x02 - Communication 
                                                   Interface Class Code */
#define CDC_DATA_IFACE_CLASS_CODE       0x0a    /* 0x0a - Data Interface Class 
                                                   Code */
#define CDC_DATA_IFACE_SUBCLASS_CODE    0x00    /* 0x00 - Data Interface 
                                                   Subclass Code */
/* CDC Communication Interface Class Control Protocol Codes */
#define CDC_IFACE_PROTOCOL_NONE         0x00    /* No class protocol */
#define CDC_IFACE_PROTOCOL_AT           0x01    /* Common AT command (Hayes) */
#define CDC_IFACE_PROTOCOL_VS           0xff    /* Vendor specific */
/* IAD Specific USB Class Definitions */
#define USB_CLASS_MISC                  0xef
#define USB_MISC_SUBCLASS_COMMON        0x02

/* Class-Specific Request Codes */
#define CDC_SEND_ENC_COMMAND            0x00
#define CDC_GET_ENC_RESPONSE            0x01
/* Class-Specific Notification Codes */
#define CDC_NETWORK_CONNECTION          0x00
#define CDC_RESPONSE_AVAILABLE          0x01
#define CDC_SERIAL_STATE                0x20
#define CDC_CONNECTION_SPEED_CHANGE     0x2a

/* CDC driver type associated with the device*/
typedef enum {
    JCDC_ECM = 0,
    JCDC_ACM,
    JCDC_OBEX,
    JCDC_EEM,
    /* Must be the last field */
    JCDC_LAST_DRV
} jcdc_drv_type;

/* CDC Device State */
typedef enum {
    CDC_DEV_DISCONNECTED = 0,
    CDC_DEV_SUSPENDED,
    CDC_DEV_ATTACHED,
    CDC_DEV_ENABLED
} jcdc_state_t;

/* Device Software Context */
typedef struct {
    usbd_device_handle          device;
    jdevice_t                   dev;
    jcdc_drv_type               drv_type;
    jcdc_state_t                state;
    jcdc_state_t                pre_suspention_state;
    juint8_t                     iface_num;
    usbd_interface_handle       c_iface;  /* Handle to Communication iface */
    usbd_interface_handle       d_iface;  /* Handle to Data iface */
    juint8_t                     d_iface_def; /* Number of the default Data 
                                                Interface */
    juint8_t                     d_iface_alt; /* Number of the Data Interface's
                                                alternate setting */

    /* Data Interface pipe */
    juint8_t             bulk_ep_in;     /* Bulk IN EP address */
    juint8_t             bulk_ep_out;    /* Bulk OUT EP address */
    usbd_pipe_handle    bulk_pipe_in;
    usbd_pipe_handle    bulk_pipe_out;
    /* Device Notification pipe */
    juint8_t             intr_ep;     /* Interrupt EP address */
    usbd_pipe_handle    intr_pipe_in;      

    /* Applications context - parameter to callbacks */
    uwh_cdc_app_h        app_ctx;
} jcdc_dev_softc_t;

typedef __PACKED_PRE__ struct {
        juint8_t         bmRequestType;
        juint8_t         bNotification;
        juint16_t        wValue;
        juint16_t        wIndex;
        juint16_t        wLength;
} __PACKED_POST__ cdc_notification_t;

typedef enum {
    SEND_ENCAP_CMD,
    GET_ENCAP_RESPONSE,
    SEND_DATA,
    GET_DATA
} cdc_req_type;

/* CDC request private handle */
typedef struct {
    jcdc_dev_softc_t    *sc;
    cdc_req_type        req_type;
    struct {
        cdc_general_command_cb_t          command_cb;
        cdc_general_data_cb_t             data_cb;
    }                   cdc_callback;
    uwh_cdc_app_h        app_ctx;
    void *app_priv;
} cdc_cmd_priv_t;

typedef jbool_t (*check_desc_func)(usb_descriptor_t *desc, void *arg);

usb_descriptor_t *cdc_find_desc( usb_config_descriptor_t *conf_d, 
    check_desc_func func, void *arg);
jresult_t cdc_open_data_pipes(uwh_cdc_dev_h dev_ctx);
void cdc_close_data_pipes(uwh_cdc_dev_h dev_ctx);
void cdc_abort_data_pipes(uwh_cdc_dev_h dev_ctx);

#ifdef CONFIG_JHOST_CDC_UTILS
jresult_t cdc_utils_init(void);
void cdc_utils_uninit(void);
void cdc_utils_abort(uwh_cdc_dev_h dev_ctx);
#endif

/* Extracts general software context out of class-specific software context */
#define GENERAL_CDC_SC(sc) ((jcdc_dev_softc_t *)(sc))
/* Checks if the software context contains all the necessary information for
 * normal CDC operation */
#define IS_CDC_SC_FULL(sc) (((sc).intr_ep && (sc).bulk_ep_in &&  \
    (sc).bulk_ep_out && (sc).c_iface && (sc).d_iface) ? \
    1: 0)

#endif  /* _JCDC_GENERAL_H */

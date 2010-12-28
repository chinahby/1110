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
#ifndef _CDC_FD_H_
#define _CDC_FD_H_
#include <uw_device_cdc.h>

#ifdef CONFIG_FD_CDC_ACM
#include <uw_device_cdc_acm.h>
#endif
#ifdef CONFIG_FD_CDC_ECM
#include <uw_device_cdc_ecm.h>
#endif
#ifdef CONFIG_FD_CDC_OBEX
#include <uw_device_cdc_obex.h>
#endif
#include <jusb_common.h>

/* CCI subclass codes */
#define USB_CDC_SUBCLASS_ACM            0x02
#define USB_CDC_SUBCLASS_ECM            0x06
#define USB_CDC_SUBCLASS_OBEX           0x0B

/* CCI protocol codes */
#define USB_CDC_CTRL_PROTO_NONE         0x00
#define USB_CDC_CTRL_PROTO_AT           0x01    /* Hayes compatible */

/* Functional descriptor subtypes */
#define USB_CDC_SUBTYPE_HEADER          0x00
#define USB_CDC_SUBTYPE_CALL_MGMT       0x01
#define USB_CDC_SUBTYPE_ACM             0x02
#define USB_CDC_SUBTYPE_UNION           0x06
#define USB_CDC_SUBTYPE_COUNTRY         0x07
#define USB_CDC_SUBTYPE_ECM             0x0F
#define USB_CDC_SUBTYPE_OBEX            0x15


#if defined(FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND) \
  || defined(FEATURE_HS_USB_TLP) || defined (T_UNDP)

#define USB_CDC_SUBTYPE_MDLM            0x12
#define USB_CDC_SUBTYPE_MDLM_DETAIL     0x13

#define USB_CDC_MDLM_DETAIL_GUID_DISCRIMINATOR           0x0
#define USB_CDC_MDLM_DETAIL_CONTROL_CAPS_VENDOR_SPECIFIC 0x1
#define USB_CDC_MDLM_DETAIL_DATA_CAPS_BYTE_PADDING      0x10
#define USB_CDC_MDLM_DETAIL_BI_DIR_NMEA                 0x20
/* Enable Net packets bundling on the down link (TLP)*/
#define USB_CDC_MDLM_DETAIL_DATA_CAPS_DL_TLP      0x4
/* Enable Net packets bundling on the up link (TLP)*/
#define USB_CDC_MDLM_DETAIL_DATA_CAPS_UL_TLP      0x8

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND || FEATURE_HS_USB_TLP || T_UNDP*/


/* Functional Descriptors */
__PACKED_PRE__
struct cdc_header_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdCDC;
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_call_mgmt_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bmCapabilities;
    juint8_t bDataIface;
} __PACKED_POST__;

#ifdef CONFIG_FD_CDC_ACM
__PACKED_PRE__
struct cdc_acm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bmCapabilities;
} __PACKED_POST__;
#endif

__PACKED_PRE__
struct cdc_union_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bMasterIface;
    juint8_t bSlaveIface[1]; /* Should be able to handle more slave interfaces */
} __PACKED_POST__;

#ifdef CONFIG_FD_CDC_ECM
__PACKED_PRE__
struct cdc_ecm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t iMACAddress;
    juint32_t bmEtherStats;
    juint16_t wMaxSegSize;
    juint16_t wNumMCFilters;
    juint8_t bNumPowerFilters;
} __PACKED_POST__;
#endif

#ifdef CONFIG_FD_CDC_OBEX
__PACKED_PRE__
struct wmcdc_obex_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdVersion;
} __PACKED_POST__;
#endif


#if defined (FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND) \
  || defined(FEATURE_HS_USB_TLP) || defined (T_UNDP)

#define QCOM_16_BYTES_GUID {0x98, 0xB0, 0x6A, 0x49, 0xB0, 0x9E, 0x48, 0x96,\
                            0x94, 0x46, 0xd9, 0x9a, 0x28, 0xca, 0x4e, 0x5d}

__PACKED_PRE__
struct cdc_mdlm_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint16_t bcdVersion;
    juint8_t bGUID[16];
} __PACKED_POST__;

__PACKED_PRE__
struct cdc_mdlm_detail_fdesc
{
    juint8_t bLength;
    juint8_t bType;
    juint8_t bSubtype;
    juint8_t bGUIDType;
    juint8_t bmCtrlCapabilities;
    juint8_t bmDataCapabilities;
} __PACKED_POST__;
#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND || FEATURE_HS_USB_TLP || T_UNDP */


/* Request Codes defined by CDC */
enum cdc_requests_t
{
    CDC_REQ_SEND_ENCAPSULATED_COMMAND = 0x00,
    CDC_REQ_GET_ENCAPSULATED_RESPONSE = 0x01,
    CDC_REQ_SET_COMM_FEATURE = 0x02,
    CDC_REQ_GET_COMM_FEATURE = 0x03,
    CDC_REQ_CLEAR_COMM_FEATURE = 0x04,
    CDC_REQ_SET_AUX_LINE_STATE = 0x10,
    CDC_REQ_SET_HOOK_STATE = 0x11,
    CDC_REQ_PULSE_SETUP = 0x12,
    CDC_REQ_SEND_PULSE = 0x013,
    CDC_REQ_SET_PULSE_TIME = 0x14,
    CDC_REQ_RING_AUX_JACK = 0x15,
    CDC_REQ_SET_LINE_CODING = 0x20,
    CDC_REQ_GET_LINE_CODING = 0x21,
    CDC_REQ_SET_CONTROL_LINE_STATE = 0x22,
    CDC_REQ_SEND_BREAK = 0x23,
    CDC_REQ_SET_RINGER_PARMS = 0x30,
    CDC_REQ_GET_RINGER_PARMS = 0x31,
    CDC_REQ_SET_OPERATION_PARMS = 0x32,
    CDC_REQ_GET_OPERATION_PARMS = 0x33,
    CDC_REQ_SET_LINE_PARMS = 0x34,
    CDC_REQ_GET_LINE_PARMS = 0x35,
    CDC_REQ_DIAL_DIGITS = 0x36,
    CDC_REQ_SET_UNIT_PARAMETER = 0x37,
    CDC_REQ_GET_UNIT_PARAMETER = 0x38,
    CDC_REQ_CLEAR_UNIT_PARAMETER = 0x39,
    CDC_REQ_GET_PROFILE = 0x3A,
    CDC_REQ_SET_ETHERNET_MULTICAST_FILTERS = 0x40,
    CDC_REQ_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER = 0x41,
    CDC_REQ_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER = 0x42,
    CDC_REQ_SET_ETHERNET_PACKET_FILTER = 0x43,
    CDC_REQ_GET_ETHERNET_STATISTIC = 0x44,
    CDC_REQ_SET_ATM_DATA_FORMAT = 0x50,
    CDC_REQ_GET_ATM_DEVICE_STATISTICS = 0x51,
    CDC_REQ_SET_ATM_DEFAULT_VC = 0x52,
    CDC_REQ_GET_ATM_VC_STATISTICS = 0x53
};

/* Notification Codes defined by CDC */
enum cdc_notifications_t
{
    CDC_NTF_NETWORK_CONNECTION = 0x00,
    CDC_NTF_RESPONSE_AVAILABLE = 0x01,
    CDC_NTF_AUX_JACK_HOOK_STATE = 0x08,
    CDC_NTF_RING_DETECT = 0x09,
    CDC_NTF_SERIAL_STATE = 0x20,
    CDC_NTF_CALL_STATE_CHANGE = 0x28,
    CDC_NTF_LINE_STATE_CHANGE = 0x29,
    CDC_NTF_CONNECTION_SPEED_CHANGE = 0x2A
};

/* CDC context information */
typedef struct
{
    pipe_desc_t *desc;
    juint16_t if_num;
} cdc_pipe_t;

typedef struct _cdc_ctx_t
{
    void *core_ctx;             /* CORE context */
    cdc_appctx_t app_ctx;       /* Application context */
    fd_ops_t ops;               /* CORE callbacks */
    fd_desc_t *desc;            /* Descriptors for USB CORE */
    cdc_pipe_t in_pipe;
    cdc_pipe_t out_pipe;
    cdc_pipe_t intr_pipe;
    request_t *ctrl_read;       /* Preallocated request for control reads */
    request_t *ctrl_write;      /* Preallocated request for control writes */

    /* Application disabled/enabled */
    juint8_t app_enabled;

    /* Current max_packet_size, as determined by connection speed */
    juint16_t usb_max_packet_size;

    /* This is where we keep the last setup packet while waiting for control
     * data to arrive from the host */
    struct usb_ctrlrequest setup_packet;
    
    /* State and Data for sending an encapsulated response */
    struct
    {
        enum
        {
            CDC_ENCAP_READY=0,
            CDC_ENCAP_RESP_SENDING,
            CDC_ENCAP_RESP_SENT
        } state;
        
        jbool_t notification_pending;
        jbool_t sending_zldp;
        
        void *buffer;
        juint32_t size;
        juint32_t bytes_sent;
    } encap_response;

    /* Relevant callbacks according to model implemented */
    union
    {
        cdc_callbacks_t basic;
        #ifdef CONFIG_FD_CDC_ACM
        acm_callbacks_t acm;
        #endif
        #ifdef CONFIG_FD_CDC_ECM
        ecm_callbacks_t ecm;
        #endif
        #ifdef CONFIG_FD_CDC_OBEX
        obex_callbacks_t obex;
        #endif
    } cbs;
    
    /* Relevant data according to model implemented */
    union
    {
        cdc_init_info_t basic;
        #ifdef CONFIG_FD_CDC_ACM
        acm_init_info_t acm;
        #endif
        #ifdef CONFIG_FD_CDC_ECM
        ecm_init_info_t ecm;
        #endif
        #ifdef CONFIG_FD_CDC_OBEX
        obex_init_info_t obex;
        #endif
    } info;
} cdc_ctx_t;

/* Internal APIs */

cdc_iostatus_t cdc_get_iostatus(request_t *req, jbool_t check_size);

typedef juint32_t (*cdc_get_func_headers_fn)(cdc_ctx_t *ctx, void *buffer, 
    jbool_t is_single_iface, juint8_t interface_number);

typedef struct
{
    if_control_msg_fn cc_handler;       /* ep0 handler */
    juint8_t cci_subclass;               /* control-if CDC subclass */
    juint8_t cci_protocol;               /* control-if CDC protocol */
    jbool_t cc_ntf_supported;            /* control-if notification pipe 
                                           support */
    jbool_t dc_supported;                /* data-if support */
    jbool_t dc_empty_alt_0;              /* provide empty data-if alt setting */
    cdc_get_func_headers_fn get_func_headers;   /* functional header filler 
                                                   function */
    jbool_t is_single_iface;
    juint8_t interface_number;
} cdc_desc_init_t;

jresult_t cdc_alloc_descriptors(cdc_ctx_t *ctx, cdc_desc_init_t *init_info);

void cdc_free_descriptors(cdc_ctx_t *ctx);

request_t* cdc_create_notification(cdc_ctx_t *ctx, juint8_t bNotification,
    juint16_t wValue, juint16_t wIndex, juint16_t wLength, juint8_t *Data);

request_t* cdc_create_ep0_read(cdc_ctx_t *ctx, juint32_t size);

request_t* cdc_create_ep0_write(cdc_ctx_t *ctx, void *buffer, juint32_t size, 
    juint32_t max_size);

jresult_t cdc_send_status(cdc_ctx_t *ctx);

jresult_t cdc_handle_encapsulated_command(cdc_ctx_t *ctx,
    struct usb_ctrlrequest *req);

jresult_t cdc_handle_encapsulated_response(cdc_ctx_t *ctx, 
    struct usb_ctrlrequest *req);

jresult_t cdc_enable(cdc_ctx_t *ctx);
void cdc_disable(cdc_ctx_t *ctx);
    
jresult_t cdc_init(cdc_ctx_t *ctx);

jresult_t cdc_uninit(context_t context);

#endif /* _CDC_FD_H_ */


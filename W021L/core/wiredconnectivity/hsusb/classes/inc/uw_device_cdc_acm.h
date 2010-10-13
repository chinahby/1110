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

#ifndef _UW_DEVICE_CDC_ACM_H_
#define _UW_DEVICE_CDC_ACM_H_

#include <uw_device_cdc.h>
#include <compiler.h>

/*
 * Host to Device - Requests
 */

typedef enum
{
    ACM_FS_ABSTRACT_STATE       = 0x01,
    ACM_FS_COUNTRY_STATE        = 0x02  
} acm_comm_fs_t;

enum acm_abstract_state_t
{
    ACM_COMM_DATA_MULTIPLEXING  = 0x0001,
    ACM_COMM_IDLE_SETTING       = 0x0002
};

union acm_comm_feature_setting_t
{
    juint16_t abstract_state;
    juint16_t country_code;      /* Country code as defined in ISO3166 */
};

typedef jresult_t (*acm_set_comm_feature_cb)(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx);

typedef jresult_t (*acm_get_comm_feature_cb)(acm_comm_fs_t selector,
    union acm_comm_feature_setting_t *status, cdc_appctx_t ctx);

typedef jresult_t (*acm_clear_comm_feature_cb)(acm_comm_fs_t selector,
    cdc_appctx_t ctx);

enum acm_lc_stop_bits_t
{
    ACM_LC_1_STOP_BITS,
    ACM_LC_1_5_STOP_BITS,
    ACM_LC_2_STOP_BITS
};

enum acm_lc_parity_t
{
    ACM_LC_PARITY_NONE,
    ACM_LC_PARITY_ODD,
    ACM_LC_PARITY_EVEN,
    ACM_LC_PARITY_MARK,
    ACM_LC_PARITY_SPACE
};

enum acm_lc_data_bits_t
{
    ACM_LC_DATA_5_BITS = 5,
    ACM_LC_DATA_6_BITS = 6,
    ACM_LC_DATA_7_BITS = 7,
    ACM_LC_DATA_8_BITS = 8,
    ACM_LC_DATA_16_BITS = 16
};

__PACKED_PRE__
struct _acm_line_coding_t
{
    juint32_t dte_rate;          /* BAUD rate */
    juint8_t char_format;        /* Use acm_lc_stop_bits_t */
    juint8_t parity_type;        /* Use acm_lc_parity_t */
    juint8_t data_bits;          /* Use acm_lc_data_bits_t */
} __PACKED_POST__;

typedef struct _acm_line_coding_t acm_line_coding_t;

typedef jresult_t (*acm_set_line_coding_cb)(acm_line_coding_t *coding,
    cdc_appctx_t ctx);

typedef jresult_t (*acm_get_line_coding_cb)(acm_line_coding_t *coding,
    cdc_appctx_t ctx);

enum acm_control_line_t
{
    ACM_CONTROL_SIG_DTR = 0x0001,
    ACM_CONTROL_SIG_RTS = 0x0002
};

typedef jresult_t (*acm_set_control_line_state_cb)(juint16_t ctrl_sig_bitmap,
    cdc_appctx_t ctx);

#define ACM_BREAK_START 0xFFFF;
#define ACM_BREAK_STOP 0x0000;

typedef jresult_t (*acm_send_break_cb)(juint16_t break_duration, cdc_appctx_t ctx);

/*
 * Device to Host - Notifications
 */

enum acm_serial_state_t
{
    ACM_SERIAL_DCD              = 0x0001,
    ACM_SERIAL_DSR              = 0x0002,
    ACM_SERIAL_BREAK            = 0x0004,
    ACM_SERIAL_RING_SIG         = 0x0008,
    ACM_SERIAL_FRAMING_ERROR    = 0x0010,
    ACM_SERIAL_PARITY_ERROR     = 0x0020,
    ACM_SERIAL_OVERRUN_ERROR    = 0x0040
};

jresult_t acm_notify_serial_state(cdc_handle_t handle, juint16_t serial_state);

jresult_t acm_notify_network_connection(cdc_handle_t handle, jbool_t link_up);

/*
 * Initialization
 */

typedef struct
{
    /* Basic CDC handlers */
    cdc_callbacks_t             basic;
        
    /* ACM Host to device request handlers */
    acm_set_comm_feature_cb             set_comm_feature_h;
    acm_get_comm_feature_cb             get_comm_feature_h;
    acm_clear_comm_feature_cb           clear_comm_feature_h;
    acm_set_line_coding_cb              set_line_coding_h;
    acm_get_line_coding_cb              get_line_coding_h;
    acm_set_control_line_state_cb       set_control_line_state_h;
    acm_send_break_cb                   send_break_h;

    /* ACM Request completion handlers */
    cdc_io_complete_cb          get_comm_feature_complete_h;
    cdc_io_complete_cb          get_line_coding_complete_h;

    /* ACM Notification completion handlers */
    cdc_io_complete_cb          notify_net_conn_complete_h;
    cdc_io_complete_cb          notify_serial_state_complete_h;
} acm_callbacks_t;

enum acm_enum_capabilities_t
{
    /* ACM functional header */
    ACM_CAP_COMM_FEATURE        = 0x01,
    ACM_CAP_LINE_CODING         = 0x02,
    ACM_CAP_SEND_BREAK          = 0x04,
    ACM_CAP_NETWORK_CONNECTION  = 0x08,

    /* Call Management functional header */
    ACM_CAP_CALL_MGMT           = 0x10, /* Call mgmt handled in device */
    ACM_CAP_CALL_MGMT_OVER_DC   = 0x20  /* Call mgmt in CC only or DC capable */
};

typedef struct
{
    cdc_init_info_t basic;              /* Info relevant for all CDC modules */
    juint32_t capabilities;              /* Use acm_enum_capabilities_t */
} acm_init_info_t;

jresult_t acm_init(cdc_handle_t handle, acm_callbacks_t *cbs,
    acm_init_info_t *info, cdc_appctx_t *ctx);

#endif


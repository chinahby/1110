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

#ifndef _UW_DEVICE_CDC_ECM_H_
#define _UW_DEVICE_CDC_ECM_H_

#include <uw_device_cdc.h>

/*
 * Host to Device - Requests
 */

#define MAC_ADDR_SIZE 6
typedef struct
{
    char addr[MAC_ADDR_SIZE];
} mac_addr_t;

typedef jresult_t (*ecm_set_mc_filter_cb)(mac_addr_t *mc_filters,
    juint16_t nfilters, cdc_appctx_t ctx);

typedef struct
{
    juint16_t mask_size; /* in bytes */
    juint8_t *mask;      /* each bit denotes a byte in 'pattern' */
    juint8_t *pattern;
} ecm_pm_filter_t;

typedef jresult_t (*ecm_set_pm_filter_cb)(juint16_t filter_id,
    juint16_t filter_size, ecm_pm_filter_t *pm_filter, cdc_appctx_t ctx);

typedef jresult_t (*ecm_get_pm_filter_cb)(juint16_t filter_id,
    jbool_t *is_active, cdc_appctx_t ctx);

enum ecm_pf_bitmask_t
{
    ECM_PF_PROMISCUOUS          = 0x0001,
    ECM_PF_ALL_MULTICAST        = 0x0002,
    ECM_PF_DIRECTED             = 0x0004,
    ECM_PF_BROADCAST            = 0x0008,
    ECM_PF_MULTICAST            = 0x0010
};

typedef jresult_t (*ecm_set_pkt_filter_cb)(juint16_t pf_bitmask,
    cdc_appctx_t ctx);

/* HSU Fix: Adding support for SET_CONTROL_LINE_STATE on ECM */
typedef jresult_t (*ecm_set_control_line_state_cb)(juint16_t ctrl_sig_bitmap,
                                                   cdc_appctx_t ctx);
/* End of HSU fix*/

typedef enum
{
    ECM_FS_NONE                 = 0x00000000L,
    ECM_FS_XMIT_OK              = 0x00000001L,
    ECM_FS_RCV_OK               = 0x00000002L,
    ECM_FS_XMIT_ERR             = 0x00000004L,
    ECM_FS_RCV_ERR              = 0x00000008L,
    ECM_FS_RCV_NO_BUFFER        = 0x00000010L,
    ECM_FS_DIRECTED_BYTES_XMIT  = 0x00000020L,
    ECM_FS_DIRECTED_FRAMES_XMIT = 0x00000040L,
    ECM_FS_MCAST_BYTES_XMIT     = 0x00000080L,
    ECM_FS_MCAST_FRAMES_XMIT    = 0x00000100L,
    ECM_FS_BCAST_BYTES_XMIT     = 0x00000200L,
    ECM_FS_BCAST_FRAMES_XMIT    = 0x00000400L,
    ECM_FS_DIRECTED_BYTES_RCV   = 0x00000800L,
    ECM_FS_DIRECTED_FRAMES_RCV  = 0x00001000L,
    ECM_FS_MCAST_BYTES_RCV      = 0x00002000L,
    ECM_FS_MCAST_FRAMES_RCV     = 0x00004000L,
    ECM_FS_BCAST_BYTES_RCV      = 0x00008000L,
    ECM_FS_BCAST_FRAMES_RCV     = 0x00010000L,
    ECM_FS_RCV_CRC_ERR          = 0x00020000L,
    ECM_FS_XMIT_Q_LEN           = 0x00040000L,
    ECM_FS_RCV_ERR_ALIGN        = 0x00080000L,
    ECM_FS_XMIT_ONE_COLLISION   = 0x00100000L,
    ECM_FS_XMIT_MULT_COLLISIONS = 0x00200000L,
    ECM_FS_XMIT_DEFERRED        = 0x00400000L,
    ECM_FS_XMIT_MAX_COLLISIONS  = 0x00800000L,
    ECM_FS_RCV_OVERRUN          = 0x01000000L,
    ECM_FS_XMIT_UNDERRUN        = 0x02000000L,
    ECM_FS_XMIT_HB_FAILURE      = 0x04000000L,
    ECM_FS_XMIT_CRS_LOST        = 0x08000000L,
    ECM_FS_XMIT_LATE_COLLISIONS = 0x10000000L
} ecm_enum_stats_t;

typedef jresult_t (*ecm_get_stats_cb)(ecm_enum_stats_t feature, juint32_t *count,
    cdc_appctx_t ctx);

/*
 * Device to Host - Notifications
 */

typedef struct
{
    juint32_t upstream_bitrate;
    juint32_t downstream_bitrate;
} ecm_conn_speed_t;

jresult_t ecm_notify_conn_speed_change(cdc_handle_t handle,
    ecm_conn_speed_t *conn_speed);

jresult_t ecm_notify_network_connection(cdc_handle_t handle, jbool_t link_up);

/*
 * Initialization
 */

typedef struct
{
    /* Basic CDC handlers */
    cdc_callbacks_t             basic;

    /* ECM Host to device request handlers */
    ecm_set_mc_filter_cb        set_multicast_filter_h;
    ecm_set_pm_filter_cb        set_power_filter_h;
    ecm_get_pm_filter_cb        get_power_filter_h;
    ecm_set_pkt_filter_cb       set_packet_filter_h;
    ecm_get_stats_cb            get_stats_h;
/* HSU Fix: Adding support for SET_CONTROL_LINE_STATE on ECM */
    ecm_set_control_line_state_cb       set_control_line_state_h;
/* End of HSU fix */

    /* ECM Request completion handlers */
    cdc_io_complete_cb          get_power_filter_complete_h;
    cdc_io_complete_cb          get_stats_complete_h;

    /* ECM Notification completion handlers */
    cdc_io_complete_cb          notify_net_conn_complete_h;
    cdc_io_complete_cb          notify_speed_change_complete_h;
   
} ecm_callbacks_t;

typedef struct
{
    cdc_init_info_t basic;
#define MAC_ADDR_STR_SIZE 13
    char mac_addr[MAC_ADDR_STR_SIZE]; /* String: 'xxxxxxxxxxxx' */
    juint32_t eth_statistics;      /* Flags defined in ecm_enum_stats_t */
    juint8_t num_power_filters;
    struct
    {
        juint_t perfect_filtering : 1;
        juint_t num_filters : 15;
    } mc_filters;
} ecm_init_info_t;

jresult_t ecm_init(cdc_handle_t handle, ecm_callbacks_t *cbs,
    ecm_init_info_t *info, cdc_appctx_t *ctx);
    
#endif


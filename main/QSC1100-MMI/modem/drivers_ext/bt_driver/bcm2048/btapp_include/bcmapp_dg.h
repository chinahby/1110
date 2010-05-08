/*****************************************************************************
**                                                                            
**  Name:             btapp_dg.h                                              
**                                                                            
**  Description:     This file contains btui internal interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2004, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/


#ifndef BTAPP_DG_H
#define BTAPP_DG_H

#define BTUI_DG_ID_SPP          0
#define BTUI_DG_ID_DUN          1
#define BTUI_DG_ID_FAX          2
#define BTUI_DG_ID_CLIENT       3

#define BTUI_DG_NUM_SERVICES    4

#define BTUI_DG_DATA_TEST_TOUT  2

typedef struct
{
    BUFFER_Q        loopback_data_q;
    BT_HDR          * p_rx_buf;
    UINT16          port_handle;
    UINT16          api_handle;
    UINT16          rx_buf_len;
    UINT16          tx_sent;
    UINT16          rx_sent;
    BOOLEAN         is_open;
    UINT8           port_id;
    tBTA_SERVICE_ID service_id;
    UINT32          time_taken;
    UINT32          data_send;
    UINT32          prev_data_send;
    UINT32          data_recvd;
    UINT32          prev_data_recvd;
    TIMER_LIST_ENT  data_test_tle;
    UINT8           data_pattern;
} tBTUI_DG_APP_CB;

/* typedef for BTUI DG control block */
typedef struct
{
    tBTUI_DG_APP_CB app_cb[BTUI_DG_NUM_SERVICES];
} tBTUI_DG_CB;

extern tBTUI_DG_CB btui_dg_cb;

extern void btapp_dg_init(void);
extern void btapp_dg_close_connection(UINT8 conn_index);
extern void btapp_dg_connect(void);
extern void btapp_dg_set_device_authorized (tBTUI_REM_DEVICE * p_device_rec);

#endif

/*****************************************************************************
**                                                                            
**  Name:             bcmapp_ct.h                                              
**                                                                            
**  Description:     This file contains btui internal interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2004, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/

#include "gki.h"

#ifndef BCMAPP_CT_H
#define BCMAPP_CT_H

typedef struct
{
    BD_ADDR   gw_bd_addr;
    char      gw_name[BTUI_DEV_NAME_LENGTH+1];
    UINT8     num_members;
    tBTA_CT_WUG_MMB_DB wug_db;

}tBTUI_CT_WUG_DB;


typedef struct
{
    UINT8   ct_handle;
    UINT8   icom_handle;
    UINT8   active_handle;
    BD_ADDR   gw_bd_addr;
    char      gw_name[BTUI_DEV_NAME_LENGTH+1];
    BOOLEAN   gw_connected;
    BOOLEAN   gw_call_active;
    BOOLEAN   ic_call_active;
    BOOLEAN   incoming_call;
    tBTA_SERVICE_MASK ui_current_active_connection;    /* active connection mask */
    UINT32    ct_menu_handle;
} tBTUI_CT_CB;

#define BTUI_ICOM_APPID 2 



//extern tBTUI_CT_CB bt_ui_ct_cb;
//extern tBTUI_CT_WUG_DB bt_ui_ct_wug_db;

void bcmapp_ct_init(void);
void bcmapp_ct_enable(void);
void bcmapp_ct_disable(void);
void bcmapp_ct_send_dtmf(UINT8 digit);
void bcmapp_ct_orginate_call(UINT8 * p_num, UINT8 num_len);
void bcmapp_ct_end_call(void);
void bcmapp_ct_disconnect_connection(void);
void bcmapp_ct_answer(void);
void bcmapp_ct_call_icom_by_extn(UINT8 intercom_device);
void bcmapp_ct_open_ct_conn(void);
void bcmapp_ct_open_icom_conn(void);
tBTUI_CT_CB * bcmapp_ct_get_ct_db(void);
tBTUI_CT_WUG_DB * bcmapp_ct_get_wug_db(void);
//for test
#if 0
extern void btapp_ct_init(void);
extern void btui_ct_menu_main(void);
extern void btui_ct_input_hdlr(tBTUI_BTA_MSG *p_msg);
extern void btui_ct_bta_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#endif
//for test
//extern void btui_platform_ct_init(void);
//extern void btui_platform_ct_event(tBTA_CT_EVT event, tBTA_CT *p_data);
//extern void btui_platform_ct_init(void);

#endif

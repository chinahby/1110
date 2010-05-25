/****************************************************************************
**                                                                           
**  Name:          bcmapp_ct.c                                           
**                 
**  Description:   Contains application code for cordless telephony
**                 
**                                                                           
**  Copyright (c) 2003, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/


#include "bt_target.h"

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)

#include "gki.h"
#include "bta_api.h"
#include "bta_ct_api.h"
#include "bd.h"
#include "bt_ui.h"
#include "bt_ui_int.h"
#include "bt_ui_ht_common.h"
#include "bcmapp_ct.h"

static tBTUI_CT_CB bt_ui_ct_cb;
static tBTUI_CT_WUG_DB bt_ui_ct_wug_db;

static void bta_ct_cback(tBTA_CT_EVT event, tBTA_CT *p_data);
static void bta_ct_add_device(void);
static boolean bt_ui_nv_store_wug_db(void);
static boolean bt_ui_nv_get_wug_db(void);

//for test
#if 0
void btapp_ct_init(void)
{
    bcmapp_ct_init();
}

void btui_ct_menu_main(void)
{
}

void btui_ct_input_hdlr(tBTUI_BTA_MSG *p_msg)
{
}

void btui_ct_bta_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
}
#endif
//for test
/*******************************************************************************
**
** Function         bcmapp_ct_init
**
** Description      Initialises Cordless terminal
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_init(void)
{

    tBTA_SEC            sec_mask[2];
    BCM_FUN_STAR("bcmapp_ct_init", 0,0,0);
    sec_mask[0] = bt_ui_cfg.ct_security;
    sec_mask[1] = BTA_SEC_AUTHORIZE;

    if(bt_ui_cfg.supported_services & BTA_CTP_SERVICE_MASK)
    {
        BTA_CtEnable(bt_ui_cfg.ct_security, sec_mask[1], bta_ct_cback);
        BTA_CtIcRegister("Intercom Terminal", BTUI_ICOM_APPID);
    }
    memset(&bt_ui_ct_wug_db, 0, sizeof(bt_ui_ct_wug_db));
    bt_ui_nv_get_wug_db();
    //bt_ui_platform_ct_init();
}
tBTUI_CT_CB * bcmapp_ct_get_ct_db(void)
{
    return &bt_ui_ct_cb;
}

tBTUI_CT_WUG_DB * bcmapp_ct_get_wug_db(void)
{
    return &bt_ui_ct_wug_db;
}
/*******************************************************************************
**
** Function         bcmapp_ct_enable
**
** Description      Enable Cordless terminal
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_enable(void)// not use
{
    tBTA_SEC    sec_mask[2];
    BCM_FUN_STAR("bcmapp_ct_enable", 0,0,0);
    if(bt_ui_cfg.supported_services & BTA_CTP_SERVICE_MASK)
    {
        sec_mask[0] = bt_ui_cfg.ct_security;
        sec_mask[1] = BTA_SEC_AUTHORIZE;

        BTA_CtEnable(bt_ui_cfg.ct_security, sec_mask[1], bta_ct_cback);
    }
}

void bcmapp_ct_disable(void)
{
    BTA_CtIcDeregister(bt_ui_ct_cb.ct_handle);
    BTA_CtDisable();
}
/*******************************************************************************
**
** Function         bcmapp_ct_send_dtmf
**
** Description      Sends DTMF tone to gateway
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_send_dtmf(UINT8 digit)
{
    BCM_FUN_STAR("bcmapp_ct_send_dtmf %x", bt_ui_ct_cb.active_handle,0,0);
    BTA_CtDtmf(bt_ui_ct_cb.active_handle, digit, TRUE);
    BTA_CtDtmf(bt_ui_ct_cb.active_handle, digit, FALSE);
}
/*******************************************************************************
**
** Function         bcmapp_ct_orginate_call
**
** Description      Orginates a call to the passed in number
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_orginate_call(UINT8 * p_num, UINT8 num_len)
{
    tBTA_CT_IE_CALLED_NUM cld_num;
    BCM_FUN_STAR("bcmapp_ct_orginate_call", 0,0,0);
    cld_num.num_type = BTA_CT_CALL_NUM_UNKNOWN;
    cld_num.num_plan = BTA_CT_CALL_NP_UNKNOWN;
    cld_num.num_len = num_len;
    memcpy(cld_num.num, p_num, cld_num.num_len);
    BTA_CtOriginate(1, BTA_CT_CALL_CLASS_EXTERNAL, &cld_num, NULL, NULL);
    bt_ui_ct_cb.gw_call_active = TRUE;
}

/*******************************************************************************
**
** Function         bcmapp_ct_end_call
**
** Description      Ends the active call
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_end_call(void)
{
    BCM_FUN_STAR("bcmapp_ct_end_call %d", bt_ui_ct_cb.active_handle,0,0);
    BTA_CtEnd(bt_ui_ct_cb.active_handle, BTA_CT_CAUSE_UNKNOWN, NULL);
}
/*******************************************************************************
**
** Function         bcmapp_ct_disconnect_connection
**
** Description      Disconnects the connection to gateway
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_disconnect_connection(void)
{
    BCM_FUN_STAR("bcmapp_ct_disconnect_connection %d", bt_ui_ct_cb.active_handle,0,0);
    BTA_CtGwDeregister(bt_ui_ct_cb.active_handle);
}
/*******************************************************************************
**
** Function         bcmapp_ct_answer
**
** Description      Answers the incoming call
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_answer(void)
{
    BCM_FUN_STAR("bcmapp_ct_answer %d", bt_ui_ct_cb.active_handle,0,0);
    BTA_CtAnswer(bt_ui_ct_cb.active_handle , NULL);
    bt_ui_ct_cb.incoming_call = FALSE;
}

/*******************************************************************************
**
** Function         bcmapp_ct_call_icom_by_extn
**
** Description      Initiates a call to another intercom terminal in the WUG
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_call_icom_by_extn(UINT8 intercom_device)
{
    tBTA_CT_IE_CALLED_NUM called_num;
    BCM_FUN_STAR("bcmapp_ct_call_icom_by_extn", 0,0,0);
    called_num.num_len = 2;
    called_num.num[0] = bt_ui_ct_wug_db.wug_db[intercom_device].extn.digit1;
    called_num.num[1] = bt_ui_ct_wug_db.wug_db[intercom_device].extn.digit2;
    BTA_CtOriginate(2, BTA_CT_CALL_CLASS_INTERCOM_WUG,  &called_num, NULL,  NULL);  
}

/*******************************************************************************
**
** Function         bcmapp_ct_open_ct_conn
**
** Description      Initiates a call to another intercom terminal in the WUG
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_open_ct_conn(void)
{
    BCM_FUN_STAR("bcmapp_ct_open_ct_conn", 0,0,0);
    if(!bdcmp(bt_ui_cb.p_selected_rem_device->bd_addr, bt_ui_ct_wug_db.gw_bd_addr))//user gw
    {
        bta_ct_add_device();
        BTA_CtGwRegister(bt_ui_cb.p_selected_rem_device->bd_addr,TRUE,  &bt_ui_ct_wug_db.wug_db, bt_ui_ct_wug_db.num_members, 1);
    }
    else
    {
        BTA_CtGwRegister(bt_ui_cb.p_selected_rem_device->bd_addr, TRUE,  NULL, 0, 1);
    }

    bdcpy(bt_ui_ct_cb.gw_bd_addr, bt_ui_cb.p_selected_rem_device->bd_addr);
    if(strlen ( (const char* )bt_ui_cb.p_selected_rem_device->name))
        strncpy(bt_ui_ct_cb.gw_name, bt_ui_cb.p_selected_rem_device->name, BTUI_DEV_NAME_LENGTH);
    else
        strncpy(bt_ui_ct_cb.gw_name, "<NO NAME>", BTUI_DEV_NAME_LENGTH);

}
/*******************************************************************************
**
** Function         bcmapp_ct_open_icom_conn
**
** Description      Initiates connection to an intercom device
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ct_open_icom_conn(void)
{
    tBTA_CT_IE_CALLED_NUM called_num;

    called_num.num_type = BTA_CT_CALL_NUM_UNKNOWN;
    called_num.num_plan = BTA_CT_CALL_NP_UNKNOWN;
    called_num.num_len = 6;
    BCM_FUN_STAR("bcmapp_ct_open_icom_conn", 0,0,0);
    memcpy(called_num.num, bt_ui_cb.p_selected_rem_device->bd_addr, called_num.num_len);

    bt_ui_ct_cb.active_handle = bt_ui_ct_cb.icom_handle;
    bt_ui_ct_cb.ic_call_active = TRUE;
    bt_ui_ct_cb.ui_current_active_connection |= BTA_ICP_SERVICE_MASK;

    BTA_CtOriginate(2, BTA_CT_CALL_CLASS_INTERCOM,  &called_num, NULL, NULL);
}

/*******************************************************************************
**
** Function         bta_ct_cback
**
** Description      Callback from BTA CT
**                  
**
** Returns          void
*******************************************************************************/
static void bta_ct_cback(tBTA_CT_EVT event, tBTA_CT *p_data)
{
    tBTUI_BTA_MSG * p_msg;
    //char msg_str[64];


    BCM_FUN_STAR("bta_ct_cback event:%d", event,0,0);

    switch (event)
    {
        case BTA_CT_ENABLE_EVT:
            break;

        case BTA_CT_ALERTING_EVT:
            BCM_MSG_MED("BTA_CT_ALERTING_EVT %d %d %x", p_data->alert.app_id,p_data->alert.handle,p_data->alert.p_ie);
            break;

        case BTA_CT_INFO_EVT:
            BCM_MSG_MED("BTA_CT_INFO_EVT %d %d %x", p_data->info.app_id,p_data->info.handle,p_data->info.p_ie);
            break;

        case BTA_CT_OPEN_EVT:
            BCM_MSG_MED("BTA_CT_OPEN_EVT  %d %d %d",p_data->open.app_id, p_data->open.handle, p_data->open.status);

            if (p_data->open.status == BTA_CT_SUCCESS)
            {
                bt_ui_ct_cb.gw_connected = TRUE;
                /* start signal strength reporting */
                BTA_DmSignalStrength((BTA_SIG_STRENGTH_RSSI_MASK | BTA_SIG_STRENGTH_LINK_QUALITY_MASK), 30, TRUE);
            }
            else
            {
                bt_ui_ct_cb.gw_connected = FALSE;
            }
            if ((p_msg = (tBTUI_BTA_MSG *)bt_ui_get_msg()) != NULL)
            {
                p_msg->open.hdr.event = BTUI_MMI_CONN_UP;
                p_msg->open.service = BTA_CTP_SERVICE_ID;
                p_msg->open.status = p_data->open.status;
                bt_ui_ct_cb.ct_handle = p_data->open.handle;
                bt_ui_ct_cb.active_handle = p_data->open.handle;
                bt_ui_send_msg(BTUI_CTP_MSG_TYPE,  p_msg);
            }

            break;

        case BTA_CT_CLOSE_EVT:
            BCM_MSG_MED("BTA_CT_CLOSE_EVT app_id:%d handle:%d ",p_data->close.app_id,  p_data->close.handle,0);
            bt_ui_ct_cb.active_handle = bt_ui_ct_cb.icom_handle;
            
            bt_ui_ct_cb.gw_connected = FALSE;
            bt_ui_ct_cb.gw_call_active = FALSE;
            bt_ui_ct_cb.incoming_call = FALSE;

            /* turn off signal strength reporting */
            BTA_DmSignalStrength(0, 0, FALSE);
            if ((p_msg = (tBTUI_BTA_MSG *)bt_ui_get_msg()) != NULL)
            {
                p_msg->close.hdr.event = BTUI_MMI_CONN_DOWN;
                p_msg->close.service = BTA_CTP_SERVICE_ID;
                bt_ui_send_msg(BTUI_CTP_MSG_TYPE,  p_msg);
            }

            break;
        case BTA_CT_REGISTER_EVT:
            BCM_MSG_MED("BTA_CT_REGISTER_EVT app_id:%d handle:%d ",p_data->reg.app_id,  p_data->reg.handle,0);
            if(p_data->reg.app_id == BTUI_ICOM_APPID)
            {
                bt_ui_ct_cb.icom_handle = p_data->reg.handle;
            }
            break;

        case BTA_CT_INCOMING_EVT:
            BCM_MSG_MED("BTA_CT_INCOMING_EVT app_id:%d handle:%d %d",p_data->incoming.app_id,p_data->incoming.handle,bt_ui_ct_cb.icom_handle);
            //bt_ui_cb.record_screen = FALSE;

            bt_ui_ct_cb.active_handle = p_data->incoming.handle;
            bt_ui_ct_cb.incoming_call = TRUE;
            if(p_data->incoming.handle == bt_ui_ct_cb.icom_handle)
            {
                bt_ui_ct_cb.ui_current_active_connection |= BTA_ICP_SERVICE_MASK;
                bt_ui_ct_cb.ic_call_active = TRUE;
            }
            else
            {
                bt_ui_ct_cb.gw_call_active = TRUE;
            }

            if ((p_msg = (tBTUI_BTA_MSG *)bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_CT_CALL_INCOMING;
                bdcpy(p_msg->ct_incoming.bd_addr, p_data->incoming.bd_addr);
                p_msg->ct_incoming.call_class = p_data->incoming.call_class;
                bt_ui_send_msg(BTUI_CTP_MSG_TYPE,  p_msg);
            }

            //sprintf (msg_str, "peer bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n", 
            //         p_data->incoming.bd_addr[0], p_data->incoming.bd_addr[1],
            //         p_data->incoming.bd_addr[2], p_data->incoming.bd_addr[3],
            //         p_data->incoming.bd_addr[4], p_data->incoming.bd_addr[5]);
            //APPL_TRACE_EVENT0(msg_str);
            break;

        case BTA_CT_CONNECT_EVT:
            BCM_MSG_MED("BTA_CT_CONNECT_EVT app_id:%d handle:%d %x",p_data->connect.app_id, p_data->connect.handle,p_data->connect.p_ie);
            bt_ui_ct_cb.active_handle = p_data->connect.handle;
            bt_ui_ct_cb.incoming_call = FALSE;
            if(p_data->connect.handle == bt_ui_ct_cb.icom_handle)
            {
                bt_ui_ct_cb.ic_call_active = TRUE;
                bt_ui_ct_cb.ui_current_active_connection |= BTA_ICP_SERVICE_MASK;
            }
            else
            {
                bt_ui_ct_cb.gw_call_active = TRUE;
            }
            if ((p_msg = (tBTUI_BTA_MSG *)bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_CT_CALL_CONN;
                bt_ui_send_msg(BTUI_CTP_MSG_TYPE,  p_msg);
            }   
            break;

        case BTA_CT_DISCONNECT_EVT:
            BCM_MSG_MED("BTA_CT_DISCONNECT_EVT app_id:%d handle:%d %x",p_data->disconnect.app_id, p_data->disconnect.handle,p_data->disconnect.p_ie);
            bt_ui_ct_cb.active_handle = bt_ui_ct_cb.ct_handle;
            if(p_data->incoming.handle == bt_ui_ct_cb.icom_handle)
            {
                bt_ui_ct_cb.ic_call_active = FALSE;
                bt_ui_ct_cb.ui_current_active_connection &= ~BTA_ICP_SERVICE_MASK;
            }
            else
            {
                bt_ui_ct_cb.gw_call_active = FALSE;
            }
            bt_ui_ct_cb.incoming_call = FALSE;
            if ((p_msg = (tBTUI_BTA_MSG *)bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_CT_CALL_DISCONN;
                bt_ui_send_msg(BTUI_CTP_MSG_TYPE,  p_msg);
            }  
            break;

        case BTA_CT_WUG_CFG_EVT:
            bt_ui_ct_wug_db.num_members = p_data->wug_cfg.num_members;
            memcpy(&bt_ui_ct_wug_db.wug_db, p_data->wug_cfg.p_mmb_db, sizeof(bt_ui_ct_wug_db.wug_db));
            bdcpy(bt_ui_ct_wug_db.gw_bd_addr, bt_ui_ct_cb.gw_bd_addr);
            strncpy(bt_ui_ct_wug_db.gw_name, bt_ui_ct_cb.gw_name, BTUI_DEV_NAME_LENGTH);
            bt_ui_nv_store_wug_db();
            bta_ct_add_device();
            BCM_MSG_MED("BTA_CT_WUG_CFG_EVT  event app_id:%d handle:%d num_members %d",p_data->wug_cfg.app_id, p_data->wug_cfg.handle, p_data->wug_cfg.num_members);
            break;

        default:
            break;
    }

    //bt_ui_platform_ct_event(event, p_data);

}

/*******************************************************************************
**
** Function         bta_ct_add_device
**
** Description      Provides link keys to DM
**                  
**
** Returns          void
*******************************************************************************/
static void bta_ct_add_device(void)
{
    UINT8 i;
    tBTUI_REM_DEVICE * p_device_rec;
    BCM_FUN_STAR("bta_ct_add_device", 0,0,0);
    for(i=0; i<bt_ui_ct_wug_db.num_members; i++)
    {
        if((p_device_rec = bt_ui_get_device_record(bt_ui_ct_wug_db.wug_db[i].addr)) != NULL)
        {
            /* if link key already present, use that
            link key */
            if(p_device_rec->link_key_present)
            {
                BTA_DmAddDevice(p_device_rec->bd_addr, p_device_rec->link_key,
                        p_device_rec->trusted_mask, p_device_rec->is_trusted,
                        p_device_rec->key_type, p_device_rec->peer_io_cap);
            }
            else
            {
                /* use link key got from WUG master */
                BTA_DmAddDevice(p_device_rec->bd_addr, bt_ui_ct_wug_db.wug_db[i].key,
                        p_device_rec->trusted_mask, p_device_rec->is_trusted,
                        p_device_rec->key_type, p_device_rec->peer_io_cap);
            }
        }
        else
        {
            BTA_DmAddDevice(bt_ui_ct_wug_db.wug_db[i].addr, bt_ui_ct_wug_db.wug_db[i].key,
                    0, FALSE, BTM_LKEY_TYPE_IGNORE, BTA_IO_CAP_NONE);
        }
    }
}
static boolean bt_ui_nv_store_wug_db(void)
{
    IFile *pif;
    tBTUI_CT_WUG_DB *wug_db =  &bt_ui_ct_wug_db;//bcmapp_ct_get_wug_db();
    pif = bt_ui_open_file(BT_WUB_DB_FILE, _OFM_READWRITE);	
    if(NULL == pif)
    {
        return FALSE;
    }

    if(wug_db)
    {
        IFILE_Write(pif, (void *)wug_db, sizeof(tBTUI_CT_WUG_DB));/*rewrite all*/
    }
    IFILE_Release(pif);
    return TRUE;
}

static boolean bt_ui_nv_get_wug_db(void)
{
    IFile *pif;
    uint32 cbOffset = 0;	
    uint32 read_size =0;

    tBTUI_CT_WUG_DB *wug_db =  &bt_ui_ct_wug_db;//bcmapp_ct_get_wug_db();
    pif = bt_ui_open_file(BT_WUB_DB_FILE, _OFM_READ);	
    if(NULL == pif)
    {
        return FALSE;
    }
    cbOffset = sizeof(tBTUI_CT_WUG_DB);
    read_size = (uint32)IFILE_Read(pif, (void *)wug_db, cbOffset);
    if(cbOffset != read_size)
    {
        memset(wug_db, 0, sizeof(tBTUI_CT_WUG_DB));
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#endif

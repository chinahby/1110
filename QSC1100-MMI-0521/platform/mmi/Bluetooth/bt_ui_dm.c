/****************************************************************************
**
**  Name:          bt_ui_dm.c
**
**  Description:   contains handset ui related code for device manager
**
**
**  Copyright (c) 2002-2007, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
******************************************************************************/



#include "bt_target.h"

#include "gki.h"
#include "bta_api.h"
#include "bt_ui.h"
#include "bt_ui_int.h"
#include "bt_ui_ht_common.h"
#include "bd.h"
#include "bcmapp_dm.h"
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
#include "bcmapp_fm.h"
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if (defined BTA_AA_INCLUDED) && (BTA_AA_INCLUDED == TRUE)
#include <bt_ui_codec.h>
#endif
#include "AEEBacklight.h"

#include "OEMFS.h"
#ifndef AEE_SIMULATOR
#include "Bt_bcm.h"
#endif
#include "Appscommon_color.brh"
//#define BTAPP_TEST_MODE /*only for add bt test mode menu*/
#ifdef BT_BROADCOM_SAMPLE_CODE
/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void bt_ui_menu_local_di(void);
static void bt_ui_act_discover_di(void);
//static void bt_ui_act_conn_up (tBTUI_BTA_MSG *p_msg);
//static void bt_ui_act_conn_down (tBTUI_BTA_MSG *p_msg);
static void bt_ui_act_pin_req (tBTUI_BTA_MSG *p_msg);
static void bt_ui_act_auth_req (tBTUI_BTA_MSG *p_msg);
static void bt_ui_act_search_new_device(tBTUI_MMI_MSG *p_msg);
static void bt_ui_act_idle(tBTUI_MMI_MSG *p_msg);
static void bt_ui_act_make_find_me(void);
static void bt_ui_act_set_trusted(void);
static void bt_ui_act_set_not_trusted(void);
static void bt_ui_act_find_me_devices(void);
static void bt_ui_menu_select_stored_device_name(void);
static void bt_ui_menu_stored_device_unbond(void);
static void bt_ui_act_delete_device(void);
static void bt_ui_menu_new_device_name(void);
static void bt_ui_act_add_new_device(void);
static void bt_ui_act_trust_new_device(void);
static void bt_ui_act_disc_result_message_cback(UINT32 handle, UINT32 key);
static void bt_ui_act_discover_new_device(void);
static void bt_ui_act_discover_stored_device(void);
static void bt_ui_menu_discovering_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_new_device_name_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_select_stored_device_name_cback(UINT32 handle, UINT32 key);
static void bt_ui_act_new_dev_disc_cmpl (tBTUI_MMI_MSG *p_msg);
#endif
static void bt_ui_menu_passkey_cback(UINT32 handle, UINT32 key);
static void bt_ui_act_auth_reply_cback(UINT32 handle, UINT32 key);
#ifdef FEATURE_BLUETOOTH_APP
static void bt_ui_act_show_stored_device(void);
#endif
static void bt_ui_act_show_bonded_device(void);
static void bt_ui_act_find_new_devices(void);
static void bt_ui_menu_visibility(void);
static void bt_ui_menu_change_name_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_change_name(void);
static void bt_ui_menu_select_stored_device(void);
static void bt_ui_menu_select_new_device(void);
static void bt_ui_act_make_discoverable(void);
static void bt_ui_act_make_non_discoverable(void);
static void bt_ui_show_stored_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_searching_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_new_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_act_dev_bond(void);
static void bt_ui_discovery(BOOLEAN is_new);
static void bt_ui_act_dev_bond_n_disc(void);

static void bt_ui_act_disable_bt(void);
static void bt_ui_act_enable_bt(void);
static void bt_ui_menu_cancel_bonding_cback(UINT32 handle, UINT32 key);
#if (BTM_LISBON_INCLUDED == TRUE)
static void bt_ui_act_confirm_reply_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_sp_rmt_oob_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_sp_passkey_notif_cback(UINT32 handle, UINT32 key);
#endif
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
static void bt_ui_act_switch_bb2mm(void);
static void bt_ui_act_switch_mm2bb(void);
#endif
static void bt_ui_menu_change_remote_name_cback(UINT32 handle, UINT32 key);
static void bt_ui_dm_get_device_name(tBTUI_REM_DEVICE * device, char * p_name);
static void bt_ui_show_device_detail(void);
static void bt_ui_show_local_device_detail(void);
static void bt_ui_menu_support_profiles (void);
static void bt_ui_proc_link_change(BD_ADDR bd_addr, BOOLEAN is_up);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
static void bt_ui_act_find_opp_devices(void);
static void bt_ui_menu_support_opp (void);
static void bt_ui_show_new_opp_search_cback(UINT32 handle, UINT32 key);
#endif
static UINT8 bt_ui_display_bonded_devices(tHT_KEY_PRESSED_CBACK p_menu_callback, TCHAR* sft_key2_msg);
static void bt_ui_show_new_devices (tHT_KEY_PRESSED_CBACK  p_menu_callback);
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
static uint16 bt_ui_dm_get_device_image(bt_cod_type *cod);
#endif
/*main control block*/
tBTUI_CB bt_ui_cb;

/* maps the index of the device in the database to the menu options */
//UINT8 bt_ui_device_db_to_menu_option_lkup_tbl[BTUI_NUM_REM_DEVICE];


/* service id to service name look up table */
static const char * bt_ui_bta_service[] =
{
   "",                      /*  Reserved */
        "Serial Port",           /*  BTA_SPP_SERVICE_ID */
        "Dial Up",               /*  BTA_DUN_SERVICE_ID */
        "Fax",                   /*  BTA_FAX_SERVICE_ID */
        "Lan Access",            /*  BTA_LAP_SERVICE_ID */
        "Headset",               /*  BTA_HSP_SERVICE_ID */
        "Handsfree",             /*  BTA_HFP_SERVICE_ID */
        "Object Push",           /*  BTA_OPP_SERVICE_ID */
        "File Transfer",         /*  BTA_FTP_SERVICE_ID */
        "Cordless",              /*  BTA_CTP_SERVICE_ID  */
        "Intercom",              /*  BTA_ICP_SERVICE_ID */
        "Synchronization",       /*  BTA_SYNC_SERVICE_ID */
        "Basic Printing",        /*  BTA_BPP_SERVICE_ID */
        "Basic Imaging",         /*  BTA_BIP_SERVICE_ID */
        "Pan User",              /*  BTA_PANU_SERVICE_ID */
        "PAN NAP",               /*  BTA_NAP_SERVICE_ID  */
        "PAN GN",                /*  BTA_GN_SERVICE_ID  */
        "SIM Access",            /*  BTA_SAP_SERVICE_ID  */
        "Advanced Audio",        /*  BTA_A2DP_SERVICE_ID */
        "Remote Control",         /*  BTA_AVRCP_SERVICE_ID */
        "Human Interface",       /* BTA_HID_SERVICE_ID */
        "Advanced Video",        /* BTA_VDP_SERVICE_ID */
        "PhoneBook Access"       /* BTA_PBAP_SERVICE_ID */
};

enum
{
    //BTUI_BT_ENABLE_ITEM = 0,
    //BTUI_BT_VISBILITY_ITEM,
    //BTUI_AUDIO_DEVICES_ITEM,
    //BTUI_OPP_DEVICES_ITEM,
    //BTUI_FTS_DEVICES_ITEM,
    //BTUI_VIEW_DEVICES_ITEM,
    //BTUI_BT_CHANGE_NAME_ITEM,
    BTUI_BT_ADD_NEW_DEVICES = 0x00,
    BTUI_BT_PAIRED_DEVICES,
    BTUI_BT_SETTING,
    BTUI_BT_TEST_MODE
};

typedef enum
{
    BT_ENABLE_FROM_APP_START= 0x00,
    BT_ENABLE_FROM_SETTING ,
    BT_ENABLE_FROM_ADD_NEW_DEVIVE,
    BT_ENABLE_FROM_PAIRED_DEVICE,
    BT_ENABLE_FROM_SEND_FILE
}BT_ENABLE_FROM;
static BT_ENABLE_FROM bt_enable_from = BT_ENABLE_FROM_APP_START;
/*******************************************************************************
**
** Function         bt_ui_startup
**
** Description      Initializes bt_ui and waits for device to come up
**
** Returns          void
*******************************************************************************/
void bt_ui_startup(void)
{
    bcmapp_dm_startup();
}



/*******************************************************************************
**
** Function         bt_ui_dm_init
**
** Description      Initializes Device manger part of UI
**
** Returns          void
*******************************************************************************/
void bt_ui_dm_init(void)
{
    bcmapp_dm_init();
    //bt_ui_cfg.p_dm_event_hdlr = &bt_ui_dm_event_hdlr;
}

/*******************************************************************************
**
** Function         bt_ui_dm_get_name
**
** Description
**
** Returns          void
*******************************************************************************/
static char * bt_ui_dm_get_name (tBTUI_REM_DEVICE * p_device_rec, char *p_name)
{
    char *p_ret_name = "<No Name>";
    BCM_FUN_STAR("bt_ui_dm_get_name %x", p_device_rec,0,0);
    if (p_device_rec)   /* some device in record */
    {
        if (p_device_rec->short_name[0])
        {
            p_ret_name = p_device_rec->short_name;
        }
        else if (p_device_rec->name[0])
        {
            p_ret_name = p_device_rec->name;
        }
        else if (p_name[0])
        {
            /* the stored device does not have a name. copy the one in the event */
            strncpy(p_device_rec->name, (const char* )p_name, BTUI_DEV_NAME_LENGTH);
            p_device_rec->name[ BTUI_DEV_NAME_LENGTH] = 0;
            p_ret_name = p_device_rec->name;
        }
    }
    else if (p_name[0])
    {
        p_ret_name = p_name;
    }

    return p_ret_name;
}
/*******************************************************************************
**
** Function         bt_ui_act_pin_req_msgbox
**
** Description      Process a pin req message box event
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_pin_req_msgbox(UINT32 handle, UINT32 key)
{
    UINT32 edit_handle;
    BCM_FUN_STAR("bt_ui_act_pin_req_msgbox %x %x", handle,key,0);
    switch(key)
    {
    case HT_KEY_SOFT1:
    case HT_KEY_SELECT:
        edit_handle = bt_ui_edit_box(HT_EB_NUMBERS, bt_ui_menu_passkey_cback, bt_ui_get_brx_string(IDS_ENTER_PASSWORD),BTUI_MAX_AUTH_KEY_LENGTH,BTBAR_OK_BACK,NULL,FALSE);
        if(edit_handle)
        {
            bt_ui_cb.auth_handle = edit_handle;
        }
        else
        {
            bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
            bt_ui_cb.auth_pin_active = FALSE;
            bt_ui_cb.auth_handle = BT_UI_NULL_HANDLE;
        }
        bt_ui_close_object(handle);
        break;

    case HT_KEY_SOFT2:
        if(bt_ui_cb.after_resume_menu != BT_UI_NULL_HANDLE)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
        else
        {
            bt_ui_close_object(handle);
        }
        BTA_DmPinReply(bt_ui_cb.peer_bdaddr, FALSE, 0, (UINT8*)"");
        
        bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
        bt_ui_cb.auth_pin_active = FALSE;
        bt_ui_cb.auth_handle = BT_UI_NULL_HANDLE;
        break;
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_pin_req
**
** Description      Process a pin req event from BTA
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_pin_req (tBTUI_BTA_MSG *p_msg)
{
#ifdef BT_OEM_DEBUG
    char *msg_str = bt_ui_get_comm_buffer(NULL);
#endif
    //TCHAR *msg_w_str;
    //int msgWLen;
    tBTUI_REM_DEVICE * p_device_rec;
    UINT32 handle;
    char * p_name = NULL;
    
    BCM_FUN_STAR("bt_ui_act_pin_req", 0,0,0);
    /* Allow only one pin request at a time */
    // if (bt_ui_cb.auth_pin_active == TRUE)
    if(UI_PIN_REPLY == bt_ui_cb.ui_state)
    {
        BCM_MSG_MED("Reject Pin Req (Busy)",0,0,0);
        BTA_DmPinReply( p_msg->pin_req.bd_addr, FALSE, 0, (UINT8*)"");
        return;
    }
    bt_ui_enable_backlight();
    /*save to peer*/
    bdcpy(bt_ui_cb.peer_bdaddr, p_msg->pin_req.bd_addr);
    STRCPY(bt_ui_cb.peer_name,p_msg->pin_req.dev_name);
    MEMCPY(bt_ui_cb.peer_cod,p_msg->pin_req.dev_class,sizeof(DEV_CLASS));
    //bt_ui_cb.peer_service = bcmapp_dm_process_cod(bt_ui_cb.peer_cod);
#ifdef BT_OEM_DEBUG
    sprintf(msg_str, " Pin request for bdaddr   %02x:%02x:%02x:%02x:%02x:%02x, service %8x ,cod %02x%02x%02x, name [%s]\n",
        bt_ui_cb.peer_bdaddr[0], bt_ui_cb.peer_bdaddr[1],
        bt_ui_cb.peer_bdaddr[2], bt_ui_cb.peer_bdaddr[3],
        bt_ui_cb.peer_bdaddr[4], bt_ui_cb.peer_bdaddr[5],
        bt_ui_cb.peer_service,
        p_msg->pin_req.dev_class[0], p_msg->pin_req.dev_class[1],
        p_msg->pin_req.dev_class[2], p_msg->pin_req.dev_name);

    DBGPRINTF(msg_str);
#endif
    if(bt_ui_cb.ui_state != UI_PIN_REPLY)
        bt_ui_cb.ui_next_state = bt_ui_cb.ui_state;

    /* pause recording the menus displayed */
    //bt_ui_cb.record_screen = FALSE;

    p_device_rec = bt_ui_get_device_record(p_msg->pin_req.bd_addr);
    p_name = bt_ui_dm_get_name (p_device_rec, p_msg->pin_req.dev_name);
    if((bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND))
    {
        handle = bt_ui_edit_box(HT_EB_NUMBERS, bt_ui_menu_passkey_cback, bt_ui_get_brx_string(IDS_ENTER_PASSWORD),BTUI_MAX_AUTH_KEY_LENGTH,BTBAR_OK_BACK,NULL,FALSE);
        if(handle)
        {
            bt_ui_cb.auth_pin_active = TRUE;
            bt_ui_cb.auth_handle = handle;
            bt_ui_cb.ui_state = UI_PIN_REPLY;

            //sprintf (msg_str, "Passkey request from %s.", p_name);
            //msg_w_str = bt_ui_get_comm_wbuffer(&msgWLen);
            //StrToWStr(msg_str, msg_w_str, msgWLen<<1);
            //HT_MessageBoxTimeout_ex(IDS_ENTER_PASSWORD, BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
    else
    {
        //#define BTUI_TIMEOUT_PIN_REQ_PERIOD     20000
        //sprintf (msg_str, "Passkey request from %s, input key?", p_name);
        //msg_w_str = bt_ui_get_comm_wbuffer(&msgWLen);
        //StrToWStr(msg_str, msg_w_str, msgWLen<<1);
        //IDS_BT_BE_PAIREING
        AECHAR buf[65] = {0};
        AECHAR bufname[BTUI_DEV_NAME_LENGTH+1] = {0};
        TCHAR *msg_w_str;
        int msg_w_len;

        ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_BT_BE_PAIREING, buf, sizeof(buf)) ;

        //sprintf (msg_str, "Authorization request from %s for %s service", p_name, bt_ui_bta_service[p_msg->auth_req.service]);
        msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
        //STRTOWSTR(p_name,bufname,sizeof(bufname));
        UTF8TOWSTR((byte *) p_name, BTUI_DEV_NAME_LENGTH+1, bufname, sizeof(bufname));
        WSPRINTF(msg_w_str, msg_w_len*2, buf, bufname);
        bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
        handle = bt_ui_message_box_timeout(msg_w_str, bt_ui_act_pin_req_msgbox, BTUI_TIMEOUT_MSG_PERIOD * 10,BTBAR_ACCEPT_REJECT);
        if(handle)
        {
            bt_ui_cb.auth_pin_active = TRUE;
            bt_ui_cb.auth_handle = handle;
            bt_ui_cb.ui_state = UI_PIN_REPLY;
        }
    }
}

#if (BTM_LISBON_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bt_ui_act_cfm_req
**
** Description      Process a user confirm req event from BTA
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_cfm_req (tBTUI_BTA_MSG *p_msg)
{
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    TCHAR *msg_w_str;
    int msg_w_len;
    tBTUI_REM_DEVICE * p_device_rec;
    UINT32 handle;
    char * p_name = NULL;

    bdcpy(bt_ui_cb.peer_bdaddr, p_msg->cfm_req.bd_addr);

    if(bt_ui_cb.ui_state != UI_CONFIRM_REPLY)
        bt_ui_cb.ui_next_state = bt_ui_cb.ui_state;

    bt_ui_cb.ui_state = UI_CONFIRM_REPLY;

    /* pause recording the menus displayed */
    //bt_ui_cb.record_screen = FALSE;

    p_device_rec = bt_ui_get_device_record(p_msg->cfm_req.bd_addr);
    p_name = bt_ui_dm_get_name(p_device_rec, p_msg->cfm_req.dev_name);

    if(p_msg->cfm_req.just_works)
        sprintf(msg_str, "Do you want to pair with device %s.", p_name);
    else
        sprintf(msg_str, "Is the digit displayed the same in both deviced? %06d", p_msg->cfm_req.num_val);

    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
    handle = bt_ui_message_box(msg_w_str, bt_ui_act_confirm_reply_cback,BTBAR_CANCEL);

    bt_ui_cb.auth_pin_active = TRUE;
    bt_ui_cb.auth_handle = handle;
#ifdef BT_OEM_DEBUG
    sprintf (msg_str, " User Confirm request for bdaddr   %02x:%02x:%02x:%02x:%02x:%02x, cod %02x%02x%02x, name [%s]\n",
        bt_ui_cb.peer_bdaddr[0], bt_ui_cb.peer_bdaddr[1],
        bt_ui_cb.peer_bdaddr[2], bt_ui_cb.peer_bdaddr[3],
        bt_ui_cb.peer_bdaddr[4], bt_ui_cb.peer_bdaddr[5],
        p_msg->cfm_req.dev_class[0], p_msg->cfm_req.dev_class[1],
        p_msg->cfm_req.dev_class[2], p_msg->cfm_req.dev_name);

    DBGPRINTF(msg_str);
#endif
}


#define BTUI_DM_KEY_PRESS_OFFSET    20
/*******************************************************************************
**
** Function         bt_ui_act_key_press_notif
**
** Description      Process a key press notification event
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_key_press_notif (tBTUI_BTA_MSG *p_msg)
{
    UINT8 i, handle;
    int     temp = 0,
        passkey = bt_ui_cb.pass_key;
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    TCHAR *msg_w_str;
    int msg_w_len;

    /* only processed for active Passkey entry menu, update menu GUI */
    if (bt_ui_cb.auth_pin_active)
    {
        sprintf(msg_str, "Please enter digit: %06d", bt_ui_cb.pass_key);

        switch (p_msg->key_press.notif_type)
        {
        case BTA_SP_KEY_ENTERED: /* add one bullet star */
            bt_ui_cb.notif_index ++;
            break;
        case BTA_SP_KEY_ERASED: /* back off one bullet star */
            if (bt_ui_cb.notif_index > 0)
                bt_ui_cb.notif_index --;
            break;
        case BTA_SP_KEY_CLEARED: /* display all passkey */
            bt_ui_cb.notif_index = 0;
            break;
        default:
            break;
        }
        /* if passkey have leading '0', or in other words: less than 6 digits */
        if (( passkey = bt_ui_cb.pass_key) < 100000)
            passkey += 100000;

        for (i = 0; i < bt_ui_cb.notif_index && temp < passkey; i ++)
        {
            /* translate every bullet point as digit '9', can not exceed value of 999999 */
            temp = temp * 10 + 9;
            msg_str[i + BTUI_DM_KEY_PRESS_OFFSET] = '*';
        }

        bt_ui_close_object(bt_ui_cb.auth_handle);

        msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
        StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
        handle = bt_ui_message_box(msg_w_str,  bt_ui_menu_sp_passkey_notif_cback,BTBAR_CANCEL);

        bt_ui_cb.auth_pin_active = TRUE;
        bt_ui_cb.auth_handle = handle;

    }
}

/*******************************************************************************
**
** Function         bt_ui_act_key_notif
**
** Description      Process a passkey notification event from BTA
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_key_notif (tBTUI_BTA_MSG *p_msg)
{
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    tBTUI_REM_DEVICE * p_device_rec;
    UINT32 handle;
    char * p_name = NULL;
    TCHAR *msg_w_str;
    int msg_w_len;

    bdcpy(bt_ui_cb.peer_bdaddr, p_msg->key_notif.bd_addr);

    if(bt_ui_cb.ui_state != UI_PASSKEY_REPLY)
        bt_ui_cb.ui_next_state = bt_ui_cb.ui_state;

    bt_ui_cb.ui_state = UI_PASSKEY_REPLY;

    /* pause recording the menus displayed */
    //bt_ui_cb.record_screen = FALSE;

    p_device_rec = bt_ui_get_device_record(p_msg->key_notif.bd_addr);
    p_name = bt_ui_dm_get_name (p_device_rec, p_msg->key_notif.dev_name);

    bt_ui_cb.notif_index = 0;
    bt_ui_cb.pass_key = p_msg->key_notif.passkey;

    sprintf (msg_str, "Please enter digit: %06d", p_msg->key_notif.passkey);

    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
    handle = bt_ui_message_box(msg_w_str,  bt_ui_menu_sp_passkey_notif_cback,BTBAR_CANCEL);

    bt_ui_cb.auth_pin_active = TRUE;
    bt_ui_cb.auth_handle = handle;
#ifdef BT_OEM_DEBUG
    sprintf (msg_str, " SP Passkey request for bdaddr   %02x:%02x:%02x:%02x:%02x:%02x, cod %02x%02x%02x, name [%s]\n",
        bt_ui_cb.peer_bdaddr[0], bt_ui_cb.peer_bdaddr[1],
        bt_ui_cb.peer_bdaddr[2], bt_ui_cb.peer_bdaddr[3],
        bt_ui_cb.peer_bdaddr[4], bt_ui_cb.peer_bdaddr[5],
        p_msg->key_notif.dev_class[0], p_msg->key_notif.dev_class[1],
        p_msg->key_notif.dev_class[2], p_msg->key_notif.dev_name);

    DBGPRINTF(msg_str);
#endif
}
/*******************************************************************************
**
** Function         bt_ui_act_rmt_oob
**
** Description      Process a remote oob req event from BTA
**
** Returns          void
*******************************************************************************/
static UINT32 bt_ui_act_rmt_oob_box (tBTUI_STATE ui_state, TCHAR *p_prompt_text)
{
    UINT32 handle;
    if(bt_ui_cb.ui_state != UI_OOB_C_REPLY && bt_ui_cb.ui_state != UI_OOB_R_REPLY)
        bt_ui_cb.ui_next_state = bt_ui_cb.ui_state;

    bt_ui_cb.ui_state = ui_state;
    handle = bt_ui_edit_box(HT_EB_LETTERS, bt_ui_menu_sp_rmt_oob_cback, p_prompt_text,BTUI_MAX_AUTH_KEY_LENGTH,BTBAR_OK_BACK,NULL,FALSE);
    return handle;
}

/*******************************************************************************
**
** Function         bt_ui_act_rmt_oob
**
** Description      Process a remote oob req event from BTA
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_rmt_oob (tBTUI_BTA_MSG *p_msg)
{
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    tBTUI_REM_DEVICE * p_device_rec;
    UINT32 handle;
    char * p_name = NULL;
    TCHAR *msg_w_str;
    int msg_w_len;

    bdcpy(bt_ui_cb.peer_bdaddr, p_msg->rmt_oob.bd_addr);

    /* pause recording the menus displayed */
    //bt_ui_cb.record_screen = FALSE;

    p_device_rec = bt_ui_get_device_record(p_msg->rmt_oob.bd_addr);
    p_name = bt_ui_dm_get_name (p_device_rec, p_msg->rmt_oob.dev_name);

    sprintf (msg_str, "Remote OOB request from %s.", p_name);

    handle = bt_ui_act_rmt_oob_box(UI_OOB_C_REPLY, L"Enter Hash C");

    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
    HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);

    bt_ui_cb.auth_pin_active = TRUE;
    bt_ui_cb.auth_handle = handle;
#ifdef BT_OEM_DEBUG
    sprintf (msg_str, " SP Rmt OOB request for bdaddr   %02x:%02x:%02x:%02x:%02x:%02x, cod %02x%02x%02x, name [%s]\n",
        bt_ui_cb.peer_bdaddr[0], bt_ui_cb.peer_bdaddr[1],
        bt_ui_cb.peer_bdaddr[2], bt_ui_cb.peer_bdaddr[3],
        bt_ui_cb.peer_bdaddr[4], bt_ui_cb.peer_bdaddr[5],
        p_msg->rmt_oob.dev_class[0], p_msg->rmt_oob.dev_class[1],
        p_msg->rmt_oob.dev_class[2], p_msg->rmt_oob.dev_name);

    DBGPRINTF(msg_str);
#endif
}
#endif
#if 0
/*******************************************************************************
**
** Function         bt_ui_act_conn_up
**
** Description      Process a connection up event
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_conn_up (tBTUI_BTA_MSG *p_msg)
{
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    TCHAR *msg_w_str;
    int msg_w_len;
    BCM_FUN_STAR("bt_ui_act_conn_up", 0,0,0);
    if(p_msg->open.status)
    {
    /* temporarily stop recording the screen
    since this message is not part of any menu
    only the menus that are displayed on the screen
        have to be recorded for redisplay */
        bt_ui_cb.record_screen = FALSE;

        /* redisplay the current screen
        then appens the connection failed message */
        sprintf (msg_str, "%s Connection Closed", bt_ui_bta_service[p_msg->open.service] );
        DBGPRINTF(msg_str);

        msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
        StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
        HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);

        /* continue with recording the displayed
        options */
        bt_ui_cb.record_screen = TRUE;
    }
    else
    {
        sprintf (msg_str, "%s Connection Opened", bt_ui_bta_service[p_msg->open.service] );
        DBGPRINTF(msg_str);

        msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
        StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
        HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
        bt_ui_cb.ui_current_active_connection |= (1<<p_msg->open.service);

    }

}

/*******************************************************************************
**
** Function         bt_ui_act_conn_down
**
** Description      Process a connection down event
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_conn_down (tBTUI_BTA_MSG *p_msg)
{
    char *msg_str = bt_ui_get_comm_buffer(NULL);
    TCHAR *msg_w_str;
    int msg_w_len;
    BCM_FUN_STAR("bt_ui_act_conn_down", 0,0,0);
    sprintf (msg_str, "%s Connection Closed", bt_ui_bta_service[p_msg->close.service] );
    DBGPRINTF(msg_str);

    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    StrToWStr(msg_str, msg_w_str, msg_w_len<<1);

    HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
    bt_ui_cb.ui_current_active_connection &= ~(1<<p_msg->close.service);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_passkey_cback
**
** Description      Process the passkey entered by user
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_passkey_cback(UINT32 handle, UINT32 key)
{
    TCHAR* p_wtext;
    int szLen;
    char *szText = bt_ui_get_comm_buffer(&szLen);
    BOOLEAN accept = TRUE;
    BCM_FUN_STAR("bt_ui_menu_passkey_cback %x %x %x", handle,key,bt_ui_cb.after_resume_menu);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        if(!p_wtext || !p_wtext[0])
        {
            accept = FALSE;
            return;
        }
        MEMSET(szText, 0, szLen);
        WSTRTOSTR(p_wtext, szText, szLen);

        bcmapp_dm_pin_code_reply(accept,
            (UINT8)strlen((const char*)szText),
            (UINT8*)szText);

        bt_ui_close_object(handle);
        bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
        bt_ui_cb.auth_pin_active = FALSE;
        bt_ui_cb.auth_handle = BT_UI_NULL_HANDLE;

        if (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND)
        {
            bt_ui_set_pop_msg_type(MESSAGE_WAITING);
            bt_ui_cb.current_bonding_handle = bt_ui_message_box_ex(IDS_PAIRING, bt_ui_menu_cancel_bonding_cback,BTBAR_CANCEL);
            bt_ui_cb.is_bonding = TRUE;
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        if(BT_UI_NULL_HANDLE != bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
        else
        {
            bt_ui_close_object(handle);
        }
        if(bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND)
        {
            BTA_DmBondCancel(bt_ui_cb.p_selected_rem_device->bd_addr);
        }
        else
        {
            BTA_DmPinReply(bt_ui_cb.peer_bdaddr, FALSE, 0, (UINT8*)"");
        }
        bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
        bt_ui_cb.auth_pin_active = FALSE;
        bt_ui_cb.auth_handle = BT_UI_NULL_HANDLE;
    }
}

#if (BTM_LISBON_INCLUDED == TRUE)
/*****************************************************************************
**  Function        scru_ascii_2_hex
**
**  Description     This function converts an ASCII string into HEX
**
**  Returns         the number of hex bytes filled.
*/
int ascii_2_octet16 (char *p_ascii, UINT8 *p_hex)
{
    unsigned int     x;
    UINT8   c;
    unsigned int len = BT_OCTET16_LEN * 2;

    if(strlen(p_ascii) != len)
        return 0;

    for (x = 0; (x < len) && (*p_ascii); x++)
    {
        if (isdigit (*p_ascii))
            c = (*p_ascii - '0') << 4;
        else
            c = (toupper(*p_ascii) - 'A' + 10) << 4;

        p_ascii++;
        if (*p_ascii)
        {
            if (isdigit (*p_ascii))
                c |= (*p_ascii - '0');
            else
                c |= (toupper(*p_ascii) - 'A' + 10);

            p_ascii++;
        }
        *p_hex++ = c;
    }

    return (x);
}


/*******************************************************************************
**
** Function         bt_ui_menu_sp_rmt_oob_cback
**
** Description      Process the simple pairing passkey entered by user
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_sp_rmt_oob_cback(UINT32 handle, UINT32 key)
{
    TCHAR * p_wtext;
    int szLen;
    char *szText = bt_ui_get_comm_buffer(&szLen);
    BT_OCTET16 c;
    UINT32 new_handle = 1;
    tBTUI_STATE ui_state = UI_OOB_R_REPLY;
    TCHAR  *p_prompt_text = L"Enter Rand R";
    BOOLEAN is_ok = TRUE;

    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        MEMSET(szText, 0, szLen);
        WSTRTOSTR(p_wtext, szText, szLen);
        //DBGPRINTF("bt_ui_menu_sp_rmt_oob_cback: %s", szText);
        if(bt_ui_cb.ui_state == UI_OOB_C_REPLY)
        {
            /* convert ascii to hex */
            if(0 == ascii_2_octet16(szText, bt_ui_cb.sp_c))
            {
                /* bad hash C - cancel/reject */
                is_ok = FALSE;
            }
        }
        else
        {
            /* must be for SP R */
            if(ascii_2_octet16(szText, bt_ui_cb.sp_r))
            {
                /* successful. end */
                new_handle = 0;
            }
            else
            {
                is_ok = FALSE;
            }
        }
        if(is_ok)
        {
            bt_ui_close_object(handle);
            if(!new_handle)
            {
                bt_ui_cb.auth_pin_active = FALSE;
                bcmapp_dm_rmt_oob_reply(TRUE, bt_ui_cb.sp_c, bt_ui_cb.sp_r); //TODO_SP
            }
            else
            {
                bt_ui_cb.auth_handle = bt_ui_act_rmt_oob_box(ui_state, p_prompt_text);
            }
        }
        else
        {
            key = HT_KEY_SOFT2;
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        bt_ui_cb.auth_pin_active = FALSE;
        /* cancel/reject */
        bcmapp_dm_rmt_oob_reply(FALSE, c, bt_ui_cb.sp_r);
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_confirm_reply_cback
**
** Description      Process the confirm entered by user
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_confirm_reply_cback(UINT32 handle, UINT32 key)
{
    UINT8 accept = 2;

    if(key == HT_KEY_SOFT1)
    {
        accept = TRUE;
    }
    if(key == HT_KEY_SOFT2)
    {
        accept = FALSE;
    }

    if(2 != accept)
    {
        bt_ui_cb.auth_pin_active = FALSE;
        bcmapp_dm_confirm_reply(accept);
        bt_ui_close_object(handle);
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_sp_passkey_notif_cback
**
** Description      Process the passkey notification
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_sp_passkey_notif_cback(UINT32 handle, UINT32 key)
{
    if(key == HT_KEY_SOFT1)
    {
        bt_ui_cb.auth_pin_active = FALSE;

        /* passkey entry is canceled by initiator */
        bcmapp_dm_passkey_cancel();
        bt_ui_close_object(handle);
        bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
    }
}
#endif

/*******************************************************************************
**
** Function         bt_ui_act_auth_req
**
** Description      Process a authorization req event from BTA
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_auth_req (tBTUI_BTA_MSG *p_msg)
{
#ifdef BT_OEM_DEBUG
    char *msg_str = bt_ui_get_comm_buffer(NULL);
#endif
    char *p_name = NULL;
    tBTUI_REM_DEVICE * p_device_rec;
    AECHAR buf[65] = {0};
    AECHAR bufname[BTUI_DEV_NAME_LENGTH+1] = {0};
    AECHAR bufserver[30] = {0};
//  tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    TCHAR *msg_w_str;
    int msg_w_len;
    bt_ui_enable_backlight();
    bdcpy(bt_ui_cb.peer_bdaddr, p_msg->auth_req.bd_addr);
    strncpy(bt_ui_cb.peer_name, p_msg->auth_req.dev_name, BTUI_DEV_NAME_LENGTH);
    bt_ui_cb.peer_service = p_msg->auth_req.service;

    if(bt_ui_cb.ui_state != UI_AUTH_REPLY)
        bt_ui_cb.ui_next_state = bt_ui_cb.ui_state;
    bt_ui_cb.ui_state = UI_AUTH_REPLY;

    p_device_rec = bt_ui_get_device_record(bt_ui_cb.peer_bdaddr);
    p_name = bt_ui_dm_get_name (p_device_rec, p_msg->auth_req.dev_name);

//  p_menu[0].p_item_name = "Accept";
//  p_menu[0].item_data = 1;
//  p_menu[0].item_index = 1;
//
//  p_menu[1].p_item_name = "Reject";
//  p_menu[1].item_data = 2;
//  p_menu[1].item_index = 2;

    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_BT_AUTH_REQ, buf, sizeof(buf)) ;
    //sprintf (msg_str, "Authorization request from %s for %s service", p_name, bt_ui_bta_service[p_msg->auth_req.service]);
    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    //STRTOWSTR(p_name,bufname,sizeof(bufname));
    UTF8TOWSTR((byte *) p_name, BTUI_DEV_NAME_LENGTH+1, bufname, sizeof(bufname));
    STRTOWSTR(bt_ui_bta_service[p_msg->auth_req.service],bufserver,sizeof(bufserver));
    WSPRINTF(msg_w_str, msg_w_len*2, buf, bufname,bufserver);
    //StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
    bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
    bt_ui_cb.auth_handle = bt_ui_message_box(msg_w_str, bt_ui_act_auth_reply_cback,BTBAR_ACCEPT_REJECT);

    //bt_ui_cb.auth_handle = handle;
    bt_ui_cb.auth_pin_active = TRUE;
#ifdef BT_OEM_DEBUG
    sprintf (msg_str, "auth request for bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n ,name [%s],service %x",
        bt_ui_cb.peer_bdaddr[0], bt_ui_cb.peer_bdaddr[1],
        bt_ui_cb.peer_bdaddr[2], bt_ui_cb.peer_bdaddr[3],
        bt_ui_cb.peer_bdaddr[4], bt_ui_cb.peer_bdaddr[5],
        bt_ui_cb.peer_name,bt_ui_cb.peer_service);

    DBGPRINTF(msg_str);
#endif
}

/*******************************************************************************
**
** Function         bt_ui_dm_event_hdlr
**
** Description      Process user inputs to menu options
**                  related to bluetooth device manager
**
** Returns          void
*******************************************************************************/
//static void bt_ui_dm_event_hdlr(tBTUI_MMI_MSG *p_msg)
//{
//    switch(bt_ui_cb.ui_state)
//    {
//
//    default:
//
//        break;
//    }
//}

/*******************************************************************************
**
** Function         bt_ui_act_auth_reply
**
** Description      Action function to process auth reply
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_auth_reply_cback(UINT32 handle, UINT32 key)
{
    bt_ui_cb.auth_pin_active = FALSE;
    BCM_FUN_STAR("bt_ui_act_auth_reply_cback %x %x %x", handle,key,bt_ui_cb.after_resume_menu);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        bcmapp_dm_authorize_resp(BTA_DM_AUTH_PERM);
        bt_ui_close_object(handle);
    }
    else if(key == HT_KEY_SOFT2)
    {
        bcmapp_dm_authorize_resp(BTA_DM_NOT_AUTH);
        if(bt_ui_cb.after_resume_menu != BT_UI_NULL_HANDLE)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
        else
        {
            bt_ui_close_object(handle);
        }
    }

    bt_ui_cb.auth_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.auth_pin_active = FALSE;
    if(bt_ui_cb.after_resume_menu)
    {
        bt_ui_cb.after_resume_menu();
        bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_enable_bt_msg_box
**
** Description      Action function for the message box to enable bt device.
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_enable_bt_msg_box(UINT32 handle, UINT32 key)
{
    bt_ui_close_object(handle);
    BCM_FUN_STAR("bt_ui_act_enable_bt_msg_box %x %x %x", handle,key,bt_ui_cb.m_bsend_opc_file);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        bt_ui_act_enable_bt();
    }
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(key == HT_KEY_SOFT2 && bt_ui_cb.m_bsend_opc_file)
    {
        bt_ui_close_applet();
    }
#endif    
}

/*******************************************************************************
**
** Function         bt_ui_act_menu_idle_cback
**
** Description      Action function for the idle menu
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_menu_idle_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_act_menu_idle_cback %x %x", handle,key,0);
    switch(key)
    {
    case HT_KEY_SELECT:
    case HT_KEY_SOFT1:
        {
            tBTUI_MENU *pfnCBItem = NULL;
            uint16 itemSel;

            itemSel = IMENUCTL_GetSel((IMenuCtl *)handle);
            if(!bt_ui_device_db.bcm_enabled && (BTUI_BT_SETTING!= itemSel /*&& BTUI_BT_PAIRED_DEVICES != itemSel*/))
            {
                bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
                /*uint32 handle = */bt_ui_message_box_ex(IDS_TURN_BT_ON, bt_ui_act_enable_bt_msg_box,BTBAR_OK_CANCEL);
                if(BTUI_BT_ADD_NEW_DEVICES == itemSel)
                {
                    bt_enable_from = BT_ENABLE_FROM_ADD_NEW_DEVIVE;
                }
                else if(BTUI_BT_PAIRED_DEVICES == itemSel)
                {
                    bt_enable_from = BT_ENABLE_FROM_PAIRED_DEVICE;
                }
                return;
            }

            IMENUCTL_GetItemData((IMenuCtl *)handle, itemSel, (uint32 *)(&pfnCBItem));
            if(pfnCBItem)
                pfnCBItem();
        }
        break;

    case HT_KEY_SOFT2:
        bt_ui_close_applet();
        break;
    }
}
#ifdef BTAPP_TEST_MODE
static void bt_ui_dm_enable_dut(void)
{
#ifdef AEE_STATIC
    btapp_dm_enable_dut();
#endif
    HT_MessageBoxTimeout_ex(IDS_ENABBE_TEST_MODE, BTUI_TIMEOUT_MSG_PERIOD);
}
#endif
/*******************************************************************************
FUNCTION
    bt_ui_menu_idle

Description
    BTUI idle menu.

PROTOTYPE:
    UINT32 bt_ui_menu_idle(void)

Returns
    The handle of the menu.
*******************************************************************************/
void bt_ui_menu_idle(void)
{
    tHT_MENU_ITEM *p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT16 i = 0;
    UINT32 pMenu = 0;
    BCM_FUN_STAR("bt_ui_menu_idle bcm_enabled= %d %d",bt_ui_device_db.bcm_enabled,bt_ui_cb.m_bsend_opc_file,0);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(bt_ui_cb.m_bsend_opc_file)
    {
        if(!bt_ui_device_db.bcm_enabled )
        {
            bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
            bt_ui_message_box_ex(IDS_TURN_BT_ON, bt_ui_act_enable_bt_msg_box,BTBAR_OK_CANCEL);
            bt_enable_from = BT_ENABLE_FROM_SEND_FILE;
            return;
        }
        bt_ui_cb.show_bonded_service = BTA_OPP_SERVICE_MASK;/*in this state,only show opp device*/
        bt_ui_act_show_bonded_device();
        return;
    }
#endif

    p_menu[i].item_data = (UINT32)bt_ui_act_find_new_devices;
    p_menu[i].item_index = BTUI_BT_ADD_NEW_DEVICES;
#ifdef FEATURE_BLUETOOTH_APP
    p_menu[i++].item_string_id= IDS_ADD_NEW;
#endif
#ifdef FEATURE_CS09_BLUETOOTH_APP
   p_menu[i++].item_string_id= IDS_NEW_AUDIO_DEVICE;
#endif

    if(g_pBtuiApp->m_bQuicktest == FALSE)
    {
        bt_ui_cb.show_bonded_service = 0;
        p_menu[i].item_index = BTUI_BT_PAIRED_DEVICES;
        p_menu[i].item_data = (UINT32)bt_ui_act_show_bonded_device;
        p_menu[i++].item_string_id = IDS_PAIRED_LIST;

        p_menu[i].item_index = BTUI_BT_SETTING;
        p_menu[i].item_data = (UINT32)bt_ui_menu_setting;
        p_menu[i++].item_string_id = IDS_BTAPP_SETTING;

#ifdef BTAPP_TEST_MODE
        p_menu[i].item_index = BTUI_BT_TEST_MODE;
        p_menu[i].item_data = (UINT32)bt_ui_dm_enable_dut;
        p_menu[i++].item_string_id = IDS_TEST_MODE;
#endif
    }
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
        p_menu[i].item_index = BTUI_BT_TEST_MODE;
        p_menu[i].item_data = (UINT32)bt_ui_menu_ct_devices;
        //p_menu[i++].item_string_id = IDS_TEST_MODE;
        p_menu[i++].p_item_name = "CTP";
#endif
    pMenu = bt_ui_create_menu(p_menu, i, bt_ui_act_menu_idle_cback, bt_ui_get_brx_string(IDS_BTAPP_TITLE) ,BTBAR_SELECT_BACK,FALSE,0);
}

/*******************************************************************************
**
** Function         bt_ui_menu_setting
**
** Description      BT Settings menu
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_setting (void)
{

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    UINT16 i = 0;
    BCM_FUN_STAR("bt_ui_menu_setting", 0,0,0);
    
    if(bt_ui_device_db.bcm_enabled)
    {
        p_menu[i].item_data = (UINT32)bt_ui_act_disable_bt;
        p_menu[i].item_string_id = IDS_BT_DISABLE;
    }
    else
    {
        bt_enable_from = BT_ENABLE_FROM_SETTING;
        p_menu[i].item_data = (UINT32)bt_ui_act_enable_bt;
        p_menu[i].item_string_id = IDS_BT_ENABLE;
    }
    p_menu[i].item_index = i;
    i++;
    
#ifdef FEATURE_BLUETOOTH_APP
    p_menu[i].item_string_id = IDS_DISCOVERY_MODE;
    p_menu[i].item_data = (UINT32)bt_ui_menu_visibility;
    p_menu[i].item_index = i;
    i++;
#endif

    p_menu[i].item_string_id = IDS_DEVICE_RENAME;
    p_menu[i].item_data = (UINT32)bt_ui_menu_change_name;
    p_menu[i].item_index = i;
    i++;

    p_menu[i].item_string_id = IDS_SUP_PROFILES;
    p_menu[i].item_data = (UINT32)bt_ui_menu_support_profiles;
    p_menu[i].item_index = i;
    i++;

    p_menu[i].item_string_id = IDS_DEVICE_INFO;
    p_menu[i].item_data = (UINT32)bt_ui_show_local_device_detail;
    p_menu[i].item_index = i;
    i++;

#if (BTU_DUAL_STACK_INCLUDED == TRUE )
    if( bt_ui_cb.is_switched )
    {
        p_menu[i].p_item_name = "Switch to BB";
        p_menu[i].item_data = (UINT32)bt_ui_act_switch_mm2bb;
    }
    else
    {
        p_menu[i].p_item_name = "Switch to MM";
        p_menu[i].item_data = (UINT32)bt_ui_act_switch_bb2mm;
    }
    p_menu[i].item_index = i;
    i++;
#endif
    bt_ui_cb.current_menu_handle = bt_ui_create_menu(p_menu, i, bt_ui_menu_cback, bt_ui_get_brx_string(IDS_BTAPP_SETTING) ,BTBAR_SELECT_BACK,FALSE,0);
}
static void bt_ui_menu_support_cback(UINT32 handle, UINT32 key)
{
    switch(key)
    {
        case HT_KEY_SOFT2:      
            bt_ui_close_object(handle);
            break;

        default:
            break;
    }
}
static void bt_ui_menu_support_headset (void)
{
    bt_ui_create_static(bt_ui_get_brx_string(IDS_HANDSET_EX) ,  bt_ui_menu_support_cback,  IDS_HSP ,BTBAR_BACK);
}

static void bt_ui_menu_support_handfree (void)
{
    AECHAR w_buf[256] = {0};
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_HANDFREE_EX, w_buf, sizeof(w_buf)) ;
    bt_ui_create_static(w_buf ,  bt_ui_menu_support_cback,  IDS_HFP ,BTBAR_BACK);
}
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
static void bt_ui_menu_support_fts (void)
{
    bt_ui_create_static(bt_ui_get_brx_string(IDS_FILE_TRANSFER_EX) ,  bt_ui_menu_support_cback, IDS_FILE_TRANSFER ,BTBAR_BACK);
}
#endif
static void bt_ui_menu_support_profiles (void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    UINT16 i = 0;
    BCM_FUN_STAR("bt_ui_menu_support_profiles", 0,0,0);

    p_menu[i].item_string_id = IDS_HSP;
    p_menu[i].item_data = (UINT32)bt_ui_menu_support_headset;
    p_menu[i].item_index = i;
    i++;

    p_menu[i].item_string_id = IDS_HFP;
    p_menu[i].item_data = (UINT32)bt_ui_menu_support_handfree;
    p_menu[i].item_index = i;
    i++;
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    p_menu[i].item_string_id = IDS_OPJECT_PUSH;
    p_menu[i].item_data = (UINT32)bt_ui_menu_support_opp;
    p_menu[i].item_index = i;
    i++;
#endif
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    p_menu[i].item_string_id = IDS_FILE_TRANSFER;
    p_menu[i].item_data = (UINT32)bt_ui_menu_support_fts;
    p_menu[i].item_index = i;
    i++;
#endif
    /*handle = */bt_ui_create_menu(p_menu, i, bt_ui_menu_cback, bt_ui_get_brx_string(IDS_SUP_PROFILES) ,BTBAR_SELECT_BACK,FALSE,0);
}
/*******************************************************************************
**
** Function         bt_ui_act_disable_bt
**
** Description      Disables Bluetooth.
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_disable_bt(void)
{
    bcmapp_dm_disable_bt();
    //bt_ui_close_object(HT_GetCurrMenuHandle());
#ifdef AEE_SIMULATOR
    bt_ui_device_db.bcm_enabled = FALSE;
    bt_ui_store_bt_enable_setting(FALSE);

    bt_ui_menu_setting();

    bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
    HT_MessageBoxTimeout_ex(IDS_BT_SET_OFF, BTUI_TIMEOUT_MSG_PERIOD);

#else
    if(g_pBtuiApp->m_bsuspend == FALSE)
    {
        bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
        HT_MessageBoxTimeout_ex(IDS_BT_SET_OFF, BTUI_TIMEOUT_MSG_PERIOD);
    }
    bt_ui_ag_disable();
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    bt_ui_opc_disable();
    bt_ui_ops_disable();
#endif
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    bt_ui_fts_disable();
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
    bt_ui_ct_disable();
#endif
#endif
}

/*******************************************************************************
**
** Function         bt_ui_act_enable_bt
**
** Description      Enables Bluetooth.
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_enable_bt(void)
{
    //if(bt_enable_from == BT_ENABLE_FROM_SETTING)
    //{
    //    bt_ui_close_object(HT_GetCurrMenuHandle());
    //}
    bcmapp_dm_init();

    //bt_ui_message_box(bt_ui_get_brx_string(IDS_BT_SET_ON), NULL,BTBAR_NONE);
}

#if (BTU_DUAL_STACK_INCLUDED == TRUE )
/*******************************************************************************
**
** Function         bt_ui_act_switch_bb2mm
**
** Description      Switch from Baseband to Multimedia.
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_switch_bb2mm(void)
{
    bt_ui_close_object(HT_GetCurrMenuHandle());
    bt_ui_cb.manual_switch = TRUE;
    HT_MessageBoxTimeout(L"Switching from BB->MM", BTUI_TIMEOUT_MSG_PERIOD);
    bcmapp_dm_switch_bb2mm();
}
/*******************************************************************************
**
** Function         bt_ui_act_switch_mm2bb
**
** Description      Switch from Multimedia to Baseband.
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_switch_mm2bb(void)
{
    bt_ui_close_object(HT_GetCurrMenuHandle());
    bt_ui_cb.manual_switch = TRUE;
    HT_MessageBoxTimeout(L"Switching from MM->BB", BTUI_TIMEOUT_MSG_PERIOD);
    bcmapp_dm_switch_mm2bb();
}
#endif /*(BTU_DUAL_STACK_INCLUDED == TRUE )*/

/*******************************************************************************
**
** Function         bt_ui_menu_visibility
**
** Description      Displays current visibility status
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_visibility(void)
{

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    uint16 ui16_return = IDS_BT_DISABLE;

    BCM_FUN_STAR("bt_ui_menu_visibility", bt_ui_device_db.visibility,0,0);
    if(bt_ui_device_db.visibility)
    {
        ui16_return = IDS_BT_ENABLE;
    }

    p_menu[0].item_string_id = IDS_BT_ENABLE;
    p_menu[0].item_data = (UINT32)bt_ui_act_make_discoverable;
    p_menu[0].item_index = IDS_BT_ENABLE;

    p_menu[1].item_string_id = IDS_BT_DISABLE;
    p_menu[1].item_data = (UINT32)bt_ui_act_make_non_discoverable;
    p_menu[1].item_index = IDS_BT_DISABLE;

    //p_menu[2].p_item_name = "Find Me";
    //p_menu[2].item_data = (UINT32)bt_ui_act_make_find_me;
    //p_menu[2].item_index = 3;
    bt_ui_set_menu_sel(ui16_return);
    bt_ui_cb.select_discover_handle = bt_ui_create_menu(p_menu, 2, bt_ui_menu_cback_for_discovery_mode, bt_ui_get_brx_string(IDS_DISCOVERY_MODE)  ,BTBAR_SELECT_BACK,FALSE,MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
}

static void bt_ui_act_discover_cb(UINT32 handle, UINT32 key)
{
    bt_ui_close_object(handle);
    if(bt_ui_cb.select_discover_handle != BT_UI_NULL_HANDLE)
    {
        bt_ui_close_object(bt_ui_cb.select_discover_handle);
        bt_ui_cb.select_discover_handle = BT_UI_NULL_HANDLE;
    }
    bt_ui_menu_visibility();
}
/*******************************************************************************
**
** Function         bt_ui_act_make_discoverable
**
** Description      Action function to make device discoverable
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_make_discoverable(void)
{
    if(FALSE == bt_ui_device_db.visibility )
    {
        /* make device discoverable */
        bt_ui_cb.ui_state = UI_VISIBILITY;
        //BTUI_cls();
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_DISCOVERY_ON), BTUI_TIMEOUT_MSG_PERIOD);
        bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_DISCOVERY_ON), bt_ui_act_discover_cb,BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
        /* other devices will be able to find this device
        in an inquiry */
        bcmapp_dm_set_visibility(TRUE, FALSE);
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_make_non_discoverable
**
** Description      Action function to make device non discoverable
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_make_non_discoverable(void)
{
    if(TRUE == bt_ui_device_db.visibility )
    {
        /* make device non discoverable */
        /* other devices will not be able to find this device
        in an inquiry */
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_DISCOVERY_OFF), BTUI_TIMEOUT_MSG_PERIOD);
        bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_DISCOVERY_OFF), bt_ui_act_discover_cb,BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
        bcmapp_dm_set_visibility(FALSE, FALSE);
        bt_ui_cb.ui_state = UI_VISIBILITY;
    }
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_act_make_find_me
**
** Description      Action function to make device find-me discoverable
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_make_find_me(void)
{
    /* make device non discoverable */
    /* other devices will not be able to find this device
    in an inquiry */
    HT_MessageBoxTimeout(L"Phone is in Find-Me mode", BTUI_TIMEOUT_MSG_PERIOD);
    bcmapp_dm_set_find_me();
    bt_ui_cb.ui_state = UI_VISIBILITY;

}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_set_name
**
** Description      Prompts the user to enter the name
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_change_name (void)
{
    //UINT32 handle;

    //tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);

    /*handle =*/ bt_ui_edit_box(HT_EB_LETTERS, bt_ui_menu_change_name_cback, bt_ui_get_brx_string(IDS_DEVICE_RENAME),BTUI_DEV_NAME_LENGTH,BTBAR_OK_BACK,bt_ui_device_db.local_device_name,TRUE);
}

static void bt_ui_menu_change_remote_name (void)
{
    //UINT32 handle;

    //tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);

    /*handle = */bt_ui_edit_box(HT_EB_LETTERS, bt_ui_menu_change_remote_name_cback, bt_ui_get_brx_string(IDS_DEVICE_RENAME),BTUI_DEV_NAME_LENGTH,BTBAR_OK_BACK,bt_ui_cb.p_selected_rem_device->name,TRUE);
}

static void bt_ui_dm_get_device_name(tBTUI_REM_DEVICE * device, char * p_name)
{
    if(device->short_name[0])
    {
        STRCPY(p_name,device->short_name);
    }
    else if(device->name[0])
    {
        STRCPY(p_name,device->name);
    }
    else
    {
        STRCPY(p_name,"<NO NAME>");
    }

}
#if BTA_DI_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bt_ui_dm_add_di_record
**
** Description      Add local DI record
**
**
** Returns          void
*******************************************************************************/
void bt_ui_dm_add_di_record(void)
{
    if (bcmapp_dm_add_di_record() == 0)
    {
        bt_ui_close_object(bt_ui_cb.current_menu_handle);
        bt_ui_menu_local_di();
        HT_MessageBoxTimeout(L"Di Record Added.", BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout(L"DI record Add Failed. ", BTUI_TIMEOUT_MSG_PERIOD);
    }
}
/*******************************************************************************
**
** Function         bt_ui_dm_get_local_di_gen
**
** Description      Get local DI record general function, when handle is 0, will
**                  get primary DI record.
**
**
** Returns          void
*******************************************************************************/
void bt_ui_dm_get_local_di_gen(UINT32 handle)
{
    tBTA_DI_GET_RECORD  di_record;
    char msg_str = bt_ui_get_comm_buffer(NULL);
    int msg_w_len;
    TCHAR msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);

    if (bcmapp_dm_get_di_local_record(&di_record, handle) == 0)
    {
        sprintf (msg_str, "Primary:[%d] SpecID: [%04x] VendorID: [%04x] Version [%04x] ProductID: [%04x]",
            di_record.rec.primary_record, di_record.spec_id, di_record.rec.vendor,
            di_record.rec.version, di_record.rec.product);

        StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
        HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD*2);
    }
    else
    {
        HT_MessageBoxTimeout(L"Error Getting DI record. ", BTUI_TIMEOUT_MSG_PERIOD*2);
    }
}

/*******************************************************************************
**
** Function         bt_ui_dm_get_local_di
**
** Description      Get local DI record
**
**
** Returns          void
*******************************************************************************/
void bt_ui_dm_get_local_di(void)
{
    bt_ui_dm_get_local_di_gen(bt_ui_cb.di_handle);
}
/*******************************************************************************
**
** Function         bt_ui_dm_get_primary_di
**
** Description      Get primary DI record
**
**
** Returns          void
*******************************************************************************/
void bt_ui_dm_get_primary_di(void)
{
    bt_ui_dm_get_local_di_gen(0);
}

/*******************************************************************************
**
** Function         bt_ui_di_menu_cback
**
** Description      DI list menu callback function.
**
** Returns          void
*******************************************************************************/
static void bt_ui_di_menu_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM* p_item;
    char msg_str = bt_ui_get_comm_buffer(NULL);
    int msg_w_len;
    TCHAR msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    tBTA_DI_GET_RECORD      di_record;

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
    else if ((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        p_item = HT_GetMenuSelectedItem(handle);

        if (p_item)
        {
            if (!bcmapp_dm_get_di_remote_record(&di_record, (UINT8)(p_item->item_data)))
            {
                sprintf (msg_str, "Primary:[%d] SpecID: [%04x] VendorID: [%04x] Version [%04x] ProductID: [%04x]",
                    di_record.rec.primary_record, di_record.spec_id,
                    di_record.rec.vendor, di_record.rec.version, di_record.rec.product);

                StrToWStr(msg_str, msg_w_str, msg_w_len<<1);
                HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD*2);
            }
            else
                HT_MessageBoxTimeout("Get Remote DI record fail.", BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
}
/*******************************************************************************
**
** Function         bt_ui_show_di_record_menu
**
** Description
**
** Returns          void
*******************************************************************************/
void bt_ui_show_di_record_menu(UINT16 num_record)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle, i ;
    /* 12 is length of string "record < >" */
    TCHAR menu_name[BTA_DI_NUM_MAX][12];

    if (num_record > 0)
    {
        memset(&menu_name[0][0], 0, BTA_DI_NUM_MAX*12*2);

        for ( i = 0; i < num_record; i ++)
        {
            WSPRINTF(menu_name[i], sizeof(menu_name[i]), L"<record %d>", i+1);
            p_menu[i].p_item_name = menu_name[i];
            p_menu[i].item_data = i + 1;
            p_menu[i].item_index = i + 1;
        }

        handle = bt_ui_create_menu(p_menu, num_record , bt_ui_di_menu_cback, L"DI Record" ,BTBAR_BACK,FALSE,0);
    }
    else
    {
        if (bt_ui_cb.p_di_db)
        {
            GKI_freebuf(bt_ui_cb.p_di_db);
            bt_ui_cb.p_di_db = NULL;
        }

        HT_MessageBoxTimeout(L"No DI record found !", BTUI_TIMEOUT_MSG_PERIOD);
    }

    if (bt_ui_cb.current_menu_handle)
    {
        bt_ui_close_object(bt_ui_cb.current_menu_handle);
        bt_ui_cb.current_menu_handle = 0;
    }
}
#endif
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_menu_local_di
**
** Description      Prompts the user for the local DI record menu
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_local_di(void)
{
#if BTA_DI_INCLUDED == TRUE
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 i = 2;

    p_menu[0].p_item_name = "Add DI record";
    p_menu[0].item_data = (UINT32)bt_ui_dm_add_di_record;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Get Primary DI";
    p_menu[1].item_data = (UINT32)bt_ui_dm_get_primary_di;
    p_menu[1].item_index = 2;


    if (bt_ui_cb.di_handle != 0)
    {
        p_menu[2].p_item_name = "Get DI record";
        p_menu[2].item_data = (UINT32)bt_ui_dm_get_local_di;
        p_menu[2].item_index = 3;
        i ++;
    }

    bt_ui_cb.current_menu_handle = bt_ui_create_menu(p_menu, i, bt_ui_menu_cback, L"SETTINGS",BTBAR_BACK,FALSE,0);
#else
    HT_MessageBoxTimeout(L"DI not supported", BTUI_TIMEOUT_MSG_PERIOD);
#endif
}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_change_name_cback
**
** Description      Process user enetered name
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_change_name_cback(UINT32 handle, UINT32 key)
{
    //char text[BTUI_DEV_NAME_LENGTH + 15];
    TCHAR * p_wtext;
    int szLen;
    char *szText = bt_ui_get_comm_buffer(&szLen);
    //char szText[BTUI_DEV_NAME_LENGTH];
    //TCHAR *szWText;
    BCM_FUN_STAR("bt_ui_menu_change_name_cback %x %x", handle,key,0);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        if(p_wtext && p_wtext[0])
        {
            MEMSET(szText, 0, szLen);
            //WSTRTOSTR(p_wtext, szText, szLen);
            WSTRTOUTF8((AECHAR*) p_wtext, WSTRLEN((AECHAR*) p_wtext),(byte *)szText,  BTUI_DEV_NAME_LENGTH);

            bcmapp_dm_set_local_name(szText);

            bt_ui_set_draw_hold_down(TRUE);

            bt_ui_close_object(handle);
            
            //bt_ui_close_object(HT_GetCurrMenuHandle());//wuraojin 20081215
            //if(bt_ui_device_db.pre_settingmenu_changename_handle != BT_UI_NULL_HANDLE)
            //{
            //     bt_ui_close_object(bt_ui_device_db.pre_settingmenu_changename_handle);
            //     bt_ui_device_db.pre_settingmenu_changename_handle = BT_UI_NULL_HANDLE;
            //}

            //bt_ui_menu_setting();

            //sprintf(text, "Name Changed: %s", bt_ui_device_db.local_device_name);
            //szWText = bt_ui_get_comm_wbuffer(&szLen);
            //StrToWStr(text, szWText, szLen);
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);

            bt_ui_set_draw_hold_down(FALSE);
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}

static void bt_ui_menu_change_remote_name_cback(UINT32 handle, UINT32 key)
{
    //char text[BTUI_DEV_NAME_LENGTH + 15];
    TCHAR * p_wtext;
    //int szLen;
    //TCHAR *szWText;
    BCM_FUN_STAR("bt_ui_menu_change_name_cback %x %x %x", handle,key,bt_ui_cb.select_bonded_handle);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        if(p_wtext && p_wtext[0])
        {
            MEMSET(bt_ui_cb.p_selected_rem_device->name, 0, BTUI_DEV_NAME_LENGTH);
            //WSTRTOSTR(p_wtext, bt_ui_cb.p_selected_rem_device->name, BTUI_DEV_NAME_LENGTH);
            WSTRTOUTF8((AECHAR*) p_wtext, WSTRLEN((AECHAR*) p_wtext),(byte *)bt_ui_cb.p_selected_rem_device->name,  BTUI_DEV_NAME_LENGTH);
            //bt_ui_nv_store_device_db();
            bt_ui_store_device(bt_ui_cb.p_selected_rem_device);
            bt_ui_set_draw_hold_down(TRUE);
            bt_ui_close_object(handle);//text
            //bt_ui_close_object(HT_GetCurrMenuHandle());//option
            if(bt_ui_cb.select_bonded_handle != BT_UI_NULL_HANDLE)
            {
                bt_ui_close_object(bt_ui_cb.select_bonded_handle);//show bond need redraw
                bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
            }
            bt_ui_display_bonded_devices(bt_ui_show_stored_device_cback, NULL);
            //bt_ui_menu_select_stored_device();
            //sprintf(text, "Name Changed: %s", bt_ui_cb.p_selected_rem_device->name);
            //szWText = bt_ui_get_comm_wbuffer(&szLen);
            //StrToWStr(text, szWText, szLen);
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);

            bt_ui_set_draw_hold_down(FALSE);
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_menu_view_devices
**
** Description      Paired device menu
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_view_devices (void)
{

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_view_devices", 0,0,0);
    p_menu[0].p_item_name = "Saved Devices";
    p_menu[0].item_data = (UINT32)bt_ui_act_show_stored_device;
    p_menu[0].item_index = 1;

    bt_ui_cb.show_bonded_service = 0;
    p_menu[1].p_item_name = "Paired Devices ";
    p_menu[1].item_data = (UINT32)bt_ui_act_show_bonded_device;
    p_menu[1].item_index = 2;

    p_menu[2].p_item_name = "New Devices ";
    p_menu[2].item_data = (UINT32)bt_ui_act_find_new_devices;
    p_menu[2].item_index = 3;

    p_menu[3].p_item_name = "Find-Me Devices ";
    p_menu[3].item_data = (UINT32)bt_ui_act_find_me_devices;
    p_menu[3].item_index = 4;

    handle = bt_ui_create_menu(p_menu, 4, bt_ui_menu_cback, L"Devices" ,BTBAR_SELECT_BACK,FALSE,0);
}
#endif
#ifdef FEATURE_BLUETOOTH_APP
/*******************************************************************************
**
** Function         bt_ui_act_show_stored_device
**
** Description      Displays devices stored in database
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_show_stored_device(void)
{
    BCM_FUN_STAR("bt_ui_act_show_stored_device", 0,0,0);
    bt_ui_cb.is_show_bonded_devices = FALSE;
    bt_ui_display_devices(0, bt_ui_show_stored_device_cback, NULL);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_show_bonded_device
**
** Description      Displays devices stored in database
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_show_bonded_device(void)
{
    BCM_FUN_STAR("bt_ui_act_show_bonded_device", 0,0,0);
    bt_ui_cb.is_show_bonded_devices = TRUE;
    bt_ui_display_bonded_devices(bt_ui_show_stored_device_cback, NULL);
}

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
static void bt_ui_menu_support_opp (void)
{
    bt_ui_create_static(bt_ui_get_brx_string(IDS_OPJECT_PUSH_EX) ,  bt_ui_menu_support_cback,  IDS_OPJECT_PUSH ,BTBAR_BACK);
}
/*******************************************************************************
**
** Function         bt_ui_show_stored_device_cback
**
** Description
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_new_opp_search_cback(UINT32 handle, UINT32 key)
{

    tHT_MENU_ITEM menu_item;
    BCM_FUN_STAR("bt_ui_show_new_opp_search_cback %x %x", handle,key,0);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &menu_item))
        {
            if((IDS_NO_RECORD == menu_item.item_index) && (TRUE == bt_ui_cb.m_bsend_opc_file))
            {
                bt_ui_act_find_opp_devices();
            }
            else
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[menu_item.item_data];
                bt_ui_menu_select_stored_device();
            }
        }
        else if(bt_ui_cb.m_bsend_opc_file)
        {
            bt_ui_act_find_opp_devices();
        }
        
    }

    if(key == HT_KEY_SOFT2)
    {
        //if send opp file,not find record need back to bt idle
        if(bt_ui_cb.m_bsend_opc_file)
        {
            bt_ui_close_applet();
        }
        else
        {
            bt_ui_close_object(handle);
        }
    }
}

static void bt_ui_act_find_opp_devices(void)
{
    /* filter: Major service class is set on 20th bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x10,0x00, 0x00}, {0x10,0x00, 0x00}};
    BCM_FUN_STAR("bt_ui_act_find_opp_devices",0,0,0);
    bt_ui_cb.ui_state = UI_NEW_OP_DEVICE;
    bt_ui_search(BTA_DM_GENERAL_INQUIRY, BTA_OPP_SERVICE_MASK, bt_ui_show_new_opp_search_cback, &dev_filt);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_show_stored_device_cback
**
** Description
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_stored_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM menu_item;
    BCM_FUN_STAR("bt_ui_show_stored_device_cback %x %x", handle,key,0);
    //bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &menu_item))
        {
            if(menu_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
                if(bt_ui_cb.m_bsend_opc_file)
                {
                    bt_ui_act_find_opp_devices();
                }
                else
#endif
                {
                    bt_ui_act_find_new_devices();
                    //bt_ui_search(BTA_DM_GENERAL_INQUIRY, 0, bt_ui_menu_new_device_cback, NULL);
                }
            }
            else
            {
                bt_ui_set_menu_sel(menu_item.item_index);
                bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[menu_item.item_data];
                bt_ui_menu_select_stored_device();
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)   
        //if send opp file,not find record need back to bt idle
        if(bt_ui_cb.m_bsend_opc_file)
        {
            bt_ui_close_applet();
        }
        else
#endif        
        {
            bt_ui_close_object(handle);
        }
        bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    }
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_act_set_trusted
**
** Description      Sets device as trusted
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_set_trusted(void)
{
    bcmapp_dm_set_trusted(BTA_ALL_SERVICE_MASK, bt_ui_cb.p_selected_rem_device);

    HT_MessageBoxTimeout(L"Device Authorized", BTUI_TIMEOUT_MSG_PERIOD);
}

/*******************************************************************************
**
** Function         bt_ui_act_set_not_trusted
**
** Description      Sets device as not trusted
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_set_not_trusted(void)
{

    bcmapp_dm_set_not_trusted(bt_ui_cb.p_selected_rem_device);

    HT_MessageBoxTimeout(L"Device Unauthorized", BTUI_TIMEOUT_MSG_PERIOD);
}
#endif
static void bt_ui_show_device_detail(void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    tBTUI_REM_DEVICE * device = bt_ui_cb.p_selected_rem_device;
    //UINT32 handle;
    AECHAR menu_name[5][30];
    char c_name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR name[BTUI_DEV_NAME_LENGTH+1];
    BCM_FUN_STAR("bt_ui_show_device_detail", 0,0,0);

    bt_ui_dm_get_device_name(device, c_name);
    //STRTOWSTR(c_name,name,sizeof(name));
    UTF8TOWSTR((byte *) c_name, BTUI_DEV_NAME_LENGTH+1, name, sizeof(name));
    //WSTRCPY(menu_name[0],L"Name:");
    MEMSET(menu_name,0,sizeof(menu_name));
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_DEVICE_NAME, menu_name[0], 30) ;
    WSTRLCAT(menu_name[0],name,BTUI_DEV_NAME_LENGTH+11);
    p_menu[0].p_w_item_name = menu_name[0];
    p_menu[0].item_data = (UINT32)0;
    p_menu[0].item_index = 1;
    
    //WSTRCPY(menu_name[1],L"State:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_STATE, menu_name[1], 30) ;
    if(device->in_use && device->link_key_present)
    {
        //WSTRLCAT(menu_name[1],L"Pair",BTUI_DEV_NAME_LENGTH+11);
        ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_PAIR, menu_name[1]+WSTRLEN(menu_name[1]), 30) ;
    }
    else
    {
        //WSTRLCAT(menu_name[1],L"Not Pair",BTUI_DEV_NAME_LENGTH+11);
        ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_NOT_PAIR, menu_name[1]+WSTRLEN(menu_name[1]), 30) ;
    }
    p_menu[1].p_w_item_name = menu_name[1];
    p_menu[1].item_data = (UINT32)0;
    p_menu[1].item_index = 2;

    //WSTRCPY(menu_name[2],L"Address:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_ADDRESS, menu_name[2], 30) ;
    sprintf (c_name, "%02x:%02x:%02x:%02x:%02x:%02x ",  
                    device->bd_addr[0], device->bd_addr[1],
                    device->bd_addr[2], device->bd_addr[3],
                    device->bd_addr[4], device->bd_addr[5] );
    STRTOWSTR(c_name,name,sizeof(name));
    WSTRLCAT(menu_name[2],name,BTUI_DEV_NAME_LENGTH+11);
    p_menu[2].p_w_item_name = menu_name[2];
    p_menu[2].item_data = (UINT32)0;
    p_menu[2].item_index = 3;
    if(device->bond_time>0)
    {
#ifdef AEE_SIMULATOR
        WSPRINTF(name, sizeof(name),L"2008/12/12");
#else
        JulianType  jDate;
        GetJulianDate(device->bond_time, &jDate);
        WSPRINTF(name, sizeof(name),L"%4d/%02d/%02d",jDate.wYear, jDate.wMonth, jDate.wDay);
#endif
    }
    //else
    //{
    //    WSTRCPY(name,L"---------");
    //}
    //WSTRCPY(menu_name[3],L"Last pare:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_LAST_PAIR, menu_name[3], 30) ;
    if(device->bond_time>0)
    {
        WSTRLCAT(menu_name[3],name,BTUI_DEV_NAME_LENGTH+11);
    }
    p_menu[3].p_w_item_name = menu_name[3];
    p_menu[3].item_data = (UINT32)0;
    p_menu[3].item_index = 4;

    /*handle = */bt_ui_create_menu(p_menu, 4, bt_ui_menu_cback, bt_ui_get_brx_string(IDS_DEVICE_INFO),BTBAR_BACK,FALSE,0);
}
static void bt_ui_show_local_device_detail(void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    AECHAR menu_name[4][30];
    char c_name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR name[BTUI_DEV_NAME_LENGTH+1];
    BCM_FUN_STAR("bt_ui_show_local_device_detail", 0,0,0);
    //name
    //state
    //address
    //paired date
    //STRTOWSTR(bt_ui_device_db.local_device_name,name,sizeof(name));
    MEMSET(menu_name,0,sizeof(menu_name));
    UTF8TOWSTR((byte *) bt_ui_device_db.local_device_name, BTUI_DEV_NAME_LENGTH+1, name, sizeof(name));
    //WSTRCPY(menu_name[0],L"Name:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_NAME, menu_name[0], 30) ;
    WSTRLCAT(menu_name[0],name,BTUI_DEV_NAME_LENGTH+11);
    p_menu[0].p_w_item_name = menu_name[0];
    p_menu[0].item_data = (UINT32)0;
    p_menu[0].item_index = 1;
    
    //WSTRCPY(menu_name[1],L"Type:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_TYPE, menu_name[1], 30) ;
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_BT_MOBILE_PHONE, name, 30) ;
    WSTRLCAT(menu_name[1],name,BTUI_DEV_NAME_LENGTH+11);
    p_menu[1].p_w_item_name = menu_name[1];
    p_menu[1].item_data = (UINT32)0;
    p_menu[1].item_index = 2;

    //WSTRCPY(menu_name[2],L"Address:");
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_STRING_ADDRESS, menu_name[2], 30) ;
#ifndef AEE_SIMULATOR
    if(FALSE == bt_ui_device_db.bcm_enabled)
    {
        byte bt_addr[BD_ADDR_LEN+1];
        btui_get_bt_addr(bt_addr,BD_ADDR_LEN+1);
        sprintf (c_name, "%02x:%02x:%02x:%02x:%02x:%02x ",  bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3],bt_addr[4], bt_addr[5] );
    }
    else
#endif
    {
        sprintf (c_name, "%02x:%02x:%02x:%02x:%02x:%02x ",  
                        bt_ui_cb.local_bd_addr[0], bt_ui_cb.local_bd_addr[1],
                        bt_ui_cb.local_bd_addr[2], bt_ui_cb.local_bd_addr[3],
                        bt_ui_cb.local_bd_addr[4], bt_ui_cb.local_bd_addr[5] );
    }
    STRTOWSTR(c_name,name,sizeof(name));
    WSTRLCAT(menu_name[2],name,BTUI_DEV_NAME_LENGTH+11);
    p_menu[2].p_w_item_name = menu_name[2];
    p_menu[2].item_data = (UINT32)0;
    p_menu[2].item_index = 3;

    /*handle = */bt_ui_create_menu(p_menu, 3, bt_ui_menu_cback, bt_ui_get_brx_string(IDS_DEVICE_INFO),BTBAR_BACK,FALSE,0);
}

#if 0
static void bt_ui_show_device_detail(void)
{
    tBTUI_REM_DEVICE * device = bt_ui_cb.p_selected_rem_device;
    AEERect rc;
    char c_name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR text[BTUI_DEV_NAME_LENGTH+11];
    IDisplay* pdisplay =  bt_ui_get_display();
    BottomBar_Param_type BarParam;
    bt_ui_get_client(&rc);
    //name
    //state
    //address
    //paired date
    IDISPLAY_EraseRect (pdisplay,&rc);
    MEMSET(name,0,sizeof(name)) ;
    bt_ui_dm_get_device_name(device, c_name);
    STRTOWSTR(c_name,name,sizeof(name));
    WSTRCPY(text,L"Name:");
    WSTRLCAT(text,name,BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(name,0,sizeof(name)) ;
    rc.y = rc.y + 16;
    WSTRCPY(text,L"State:");
    if(device->in_use && device->link_key_present)
    {
        WSTRLCAT(text,L"Bond",BTUI_DEV_NAME_LENGTH+11);
    }
    else
    {
        WSTRLCAT(text,L"Not Bond",BTUI_DEV_NAME_LENGTH+11);
    }
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(name,0,sizeof(name)) ;
    rc.y = rc.y + 16;
    WSTRCPY(text,L"Address:");
    
    sprintf (c_name, "%02x:%02x:%02x:%02x:%02x:%02x ",  
                    device->bd_addr[0], device->bd_addr[1],
                    device->bd_addr[2], device->bd_addr[3],
                    device->bd_addr[4], device->bd_addr[5] );
    STRTOWSTR(c_name,name,sizeof(name));
    WSTRLCAT(text,name,BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(name,0,sizeof(name)) ;
    rc.y = rc.y + 16;
    WSTRCPY(text,L"Last connect:");
    WSTRLCAT(text,L"2008/12/12",BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(&BarParam, 0, sizeof(BarParam)); 
    BarParam.eBBarType = BTBAR_BACK;  
    DrawBottomBar(pdisplay,&BarParam);

    IDISPLAY_UpdateEx (pdisplay,FALSE);
}

static void bt_ui_show_local_device_detail(void)
{
    AEERect rc;
    char c_name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR text[BTUI_DEV_NAME_LENGTH+11];
    IDisplay* pdisplay =  bt_ui_get_display();
    BottomBar_Param_type BarParam;
    bt_ui_get_client(&rc);
    //name
    //state
    //address
    //paired date
    IDISPLAY_EraseRect (pdisplay,&rc);
    MEMSET(name,0,sizeof(name)) ;
    STRTOWSTR(bt_ui_device_db.local_device_name,name,sizeof(name));
    WSTRCPY(text,L"Name:");
    WSTRLCAT(text,name,BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(name,0,sizeof(name)) ;
    rc.y = rc.y + 16;
    WSTRCPY(text,L"Type:");
    WSTRLCAT(text,L"Mobile phone",BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);

    MEMSET(name,0,sizeof(name)) ;
    rc.y = rc.y + 16;
    WSTRCPY(text,L"Address:");
    sprintf (c_name, "%02x:%02x:%02x:%02x:%02x:%02x ",  
                    bt_ui_cb.local_bd_addr[0], bt_ui_cb.local_bd_addr[1],
                    bt_ui_cb.local_bd_addr[2], bt_ui_cb.local_bd_addr[3],
                    bt_ui_cb.local_bd_addr[4], bt_ui_cb.local_bd_addr[5] );
    STRTOWSTR(c_name,name,sizeof(name));
    WSTRLCAT(text,name,BTUI_DEV_NAME_LENGTH+11);
    IDISPLAY_DrawText(pdisplay, AEE_FONT_NORMAL,  text ,  -1,  rc.x,  rc.y, &rc, IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
    
    MEMSET(&BarParam, 0, sizeof(BarParam)); 
    BarParam.eBBarType = BTBAR_BACK;  
    DrawBottomBar(pdisplay,&BarParam);

    IDISPLAY_UpdateEx (pdisplay,FALSE);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_select_stored_device_name
**
** Description      Deletes a device from data base
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_delete_device(UINT32 handle, UINT32 key)
{
    BOOLEAN result = FALSE;
    BCM_FUN_STAR("bt_ui_act_delete_device %x %x %x", bt_ui_cb.p_selected_rem_device,bt_ui_cb.is_show_bonded_devices,bt_ui_cb.select_bonded_handle);
    bt_ui_close_object(handle);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        if(bt_ui_cb.p_selected_rem_device)
        {
            result = bcmapp_dm_delete_device(bt_ui_cb.p_selected_rem_device->bd_addr);
        }
        
        //BTUI_cls();
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        if(bt_ui_cb.select_bonded_handle != BT_UI_NULL_HANDLE)
        {
            bt_ui_close_object(bt_ui_cb.select_bonded_handle);//show bond need redraw
            bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
        }
#ifdef FEATURE_BLUETOOTH_APP
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        if(bt_ui_cb.is_show_bonded_devices)
        {
            bt_ui_act_show_bonded_device ();
        }
        else
        {
            bt_ui_act_show_stored_device ();
        }
#endif

#ifdef FEATURE_CS09_BLUETOOTH_APP
	    bt_ui_act_show_bonded_device ();
#endif
        if (result)
        {
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            if(bt_ui_cb.p_selected_rem_device->ag_config.ag_connect)
            {
                AECHAR bufname[BTUI_DEV_NAME_LENGTH+1] = {0};
                AECHAR buf[65] = {0};
                AECHAR * msg_w_str;
                int len;
                msg_w_str = bt_ui_get_comm_wbuffer(&len);
                UTF8TOWSTR((byte *) bt_ui_cb.p_selected_rem_device->name, BTUI_DEV_NAME_LENGTH+1, bufname, sizeof(bufname));
                ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_DEL_AG_FAIL, buf, sizeof(buf)) ;
                WSPRINTF(msg_w_str, len*2, buf, bufname);
                bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
                HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
            }
            else
            {
                HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_delete_all_stored_device
**
** Description      Deletes all the devices from data base
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_delete_all_stored_device(UINT32 handle, UINT32 key)
{
    uint8 i;
    BOOLEAN result = TRUE;
    bt_ui_close_object(handle);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        //num_devices = bt_ui_cb.num_devices;
        for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
        {
            //j = bt_ui_device_db_to_menu_option_lkup_tbl[i];
            if(bt_ui_device_db.device[i].in_use == TRUE)
            {
                BCM_FUN_STAR("bt_ui_act_delete_all_stored_device %d",i,0,0);
                if(!bcmapp_dm_delete_device(bt_ui_device_db.device[i].bd_addr))
                {
                    result = FALSE;
                    break;
                }
            }
        }
        //BTUI_cls();
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_close_object(bt_ui_cb.select_bonded_handle);
        if(bt_ui_cb.select_bonded_handle != BT_UI_NULL_HANDLE)
        {
            bt_ui_close_object(bt_ui_cb.select_bonded_handle);//show bond need redraw
            bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
        }
#ifdef FEATURE_BLUETOOTH_APP
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        if(bt_ui_cb.is_show_bonded_devices)
        {
            bt_ui_act_show_bonded_device ();
        }
        else
        {
            bt_ui_act_show_stored_device ();
        }
#endif

#ifdef FEATURE_CS09_BLUETOOTH_APP
	    //bt_ui_close_object(HT_GetCurrMenuHandle());
	    bt_ui_act_show_bonded_device ();
#endif
        if (result)
        {
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            if(bt_ui_device_db.device[i].ag_config.ag_connect)
            {
                AECHAR bufname[BTUI_DEV_NAME_LENGTH+1] = {0};
                AECHAR buf[65] = {0};
                AECHAR * msg_w_str;
                int len;
                msg_w_str = bt_ui_get_comm_wbuffer(&len);
                UTF8TOWSTR((byte *) bt_ui_device_db.device[i].name, BTUI_DEV_NAME_LENGTH+1, bufname, sizeof(bufname));
                ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_DEL_AG_FAIL, buf, sizeof(buf)) ;
                WSPRINTF(msg_w_str, len*2, buf, bufname);
                bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
                HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
            }
            else
            {
                HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
            }
        }
    }
}
static void bt_ui_act_delete_device_pop(void)
{
    bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
    bt_ui_message_box_ex(IDS_STRING_DELETE, bt_ui_act_delete_device,BTBAR_OK_CANCEL);
}
static void bt_ui_act_delete_all_stored_device_pop(void)
{
    bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
    bt_ui_message_box_ex(IDS_BT_DELETE_ALL, bt_ui_act_delete_all_stored_device,BTBAR_OK_CANCEL);
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_stored_device
**
** Description      Displays the menu options for a selected device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_stored_device()
{
    //char p_name[BTUI_DEV_NAME_LENGTH+1];
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    //TCHAR *szWText;
    //int szLen;
    uint16 i = 0;
    BCM_FUN_STAR("bt_ui_menu_select_stored_device %x %x %x", bt_ui_cb.p_selected_rem_device,bt_ui_cb.p_selected_rem_device->services,bt_ui_cb.m_bsend_opc_file);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(bt_ui_cb.m_bsend_opc_file)
    {
        p_menu[i].item_string_id= IDS_OPP_SEND;
        p_menu[i].item_data = (UINT32)bt_ui_act_send_file_ex;
        p_menu[i].item_index = i;
        i++;

        p_menu[i].item_string_id = IDS_SEL_SERACH;
        p_menu[i].item_data = (UINT32)bt_ui_act_find_opp_devices;
        p_menu[i].item_index = i;
        i++;
    }
    else
#endif    
    {
        if((bt_ui_cb.p_selected_rem_device->services & BTA_HSP_SERVICE_MASK) || (bt_ui_cb.p_selected_rem_device->services & BTA_HFP_SERVICE_MASK))
        {
            if(bt_ui_cb.p_selected_rem_device->ag_config.ag_connect)
            {
                p_menu[i].item_string_id = IDS_AG_DISCONNECT;
                p_menu[i].item_data = (UINT32)bt_ui_act_audio_device_disconnect;
            }
            else
            {
                p_menu[i].item_string_id = IDS_AG_CONNECT;
                p_menu[i].item_data = (UINT32)bt_ui_act_audio_device_connect;
            }
            p_menu[i].item_index = i;
            i++;
        }
        p_menu[i].item_string_id = IDS_SEL_SERACH;
        p_menu[i].item_data = (UINT32)bt_ui_act_find_new_devices;
        p_menu[i].item_index = i;
        i++;

        p_menu[i].item_string_id = IDS_DEVICE_RENAME;
        p_menu[i].item_data = (UINT32)bt_ui_menu_change_remote_name;
        p_menu[i].item_index = i;
        i++;
    }
    p_menu[i].item_string_id = IDS_DEVICE_INFO;
    p_menu[i].item_data = (UINT32)bt_ui_show_device_detail;
    p_menu[i].item_index = i;
    i++;
/*
    p_menu[i].p_item_name = "Send contact";
    p_menu[i].item_data = (UINT32)0;
    p_menu[i].item_index = i;
    i++;

    p_menu[i].p_item_name = "Unbond ";
    p_menu[i].item_data = (UINT32)bt_ui_menu_stored_device_unbond;
    p_menu[i].item_index = i;
    i++;
*/
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(bt_ui_cb.m_bsend_opc_file == FALSE)
#endif    
    {
        p_menu[i].item_string_id = IDS_STRING_DELETE;
        p_menu[i].item_data = (UINT32)bt_ui_act_delete_device_pop;
        p_menu[i].item_index = i;
        i++;

        p_menu[i].item_string_id = IDS_BT_DELETE_ALL;
        p_menu[i].item_data = (UINT32)bt_ui_act_delete_all_stored_device_pop;
        p_menu[i].item_index = i;
        i++;
    }
    /* Display the name of the device */
    //bt_ui_dm_get_device_name(bt_ui_cb.p_selected_rem_device,p_name);

    //szWText = bt_ui_get_comm_wbuffer(&szLen);
    //StrToWStr(p_name, szWText, szLen);
    //UTF8TOWSTR((const byte *)p_name,  BTUI_DEV_NAME_LENGTH + 1, szWText, szLen);
    /*handle = */bt_ui_create_menu(p_menu, i, bt_ui_menu_cback, /*szWText*/NULL ,BTBAR_SELECT_BACK,TRUE,0);
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_act_discover_new_device
**
** Description      Discovers services on the selected new device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_discover_new_device(void)
{
    bt_ui_cb.ui_state = UI_NEW_DEVICE_DISCOVER;
    bt_ui_discovery(TRUE);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_discover_stored_device
**
** Description      Discovers services on the selected stored device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_discover_stored_device(void)
{
    bt_ui_cb.ui_state = UI_NEW_DEVICE_DISCOVER;
    bt_ui_discovery(FALSE);
}

/*******************************************************************************
**
** Function         bt_ui_menu_discovering_cback
**
** Description      Cancels the discovering
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_discovering_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_menu_discovering_cback %x %x", handle,key,0);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
        //BTUI_cls();
        HT_MessageBoxTimeout(L"Discovering Canceled.", BTUI_TIMEOUT_MSG_PERIOD);
    }
}
/*******************************************************************************
**
** Function         bt_ui_discovery
**
** Description      Searches for services on designated device.
**
**
** Returns          void
*******************************************************************************/

static void bt_ui_discovery(BOOLEAN is_new)
{
    BCM_FUN_STAR("bt_ui_discovery %d", is_new,0,0);
    bt_ui_set_pop_msg_type(MESSAGE_WAITING);
    bt_ui_cb.current_menu_handle = bt_ui_message_box_ex(IDS_BT_DISCOVERING, bt_ui_menu_discovering_cback,BTBAR_CANCEL);

    bt_ui_cb.ui_prev_state = bt_ui_cb.ui_state;
    bt_ui_cb.ui_state = UI_NEW_DEVICE_DISCOVER_CMPL;

    bcmapp_dm_discover_device(bt_ui_cb.p_selected_rem_device->bd_addr, is_new);
}
#ifdef BT_BROADCOM_SAMPLE_CODE

/*******************************************************************************
**
** Function         bt_ui_act_discover_di
**
** Description      Discovers DI record on the selected device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_discover_di(void)
{
#if BTA_DI_INCLUDED == TRUE
    bt_ui_cb.ui_state = UI_NEW_DEVICE_DISCOVER;

    bt_ui_cb.current_menu_handle = bt_ui_message_box(L"Discovering DI...",NULL ,BTBAR_NONE);

    bt_ui_cb.ui_prev_state = bt_ui_cb.ui_state;
    bt_ui_cb.ui_state = UI_NEW_DEVICE_DISCOVER_CMPL;

    bcmapp_dm_di_discover(bt_ui_cb.p_selected_rem_device->bd_addr);
#else
    HT_MessageBoxTimeout(L"DI not supported", BTUI_TIMEOUT_MSG_PERIOD);
#endif
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_stored_device_name
**
** Description      Prompts user to enter a name for a stored device
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_select_stored_device_name (void)
{
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_select_stored_device_name", 0,0,0);
    //tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);

    handle = bt_ui_edit_box(HT_EB_LETTERS, bt_ui_menu_select_stored_device_name_cback, L"Enter new",BTUI_DEV_NAME_LENGTH,BTBAR_OK_BACK,bt_ui_cb.p_selected_rem_device->short_name,TRUE);
}
/*******************************************************************************
**
** Function         bt_ui_menu_select_stored_device_name_cback
**
** Description      Prompts user to enter a name for a stored device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_stored_device_name_cback(UINT32 handle, UINT32 key)
{

    TCHAR * p_wtext;
    int szLen;
    char *szText = bt_ui_get_comm_buffer(&szLen);
    BCM_FUN_STAR("bt_ui_menu_select_stored_device_name_cback %x %x", handle,key,0);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        if(p_wtext && p_wtext[0])
        {
            WSTRTOSTR(p_wtext, szText, szLen);
            bcmapp_dm_rename_device(bt_ui_cb.p_selected_rem_device, szText);

            bt_ui_close_object(handle);
            HT_MessageBoxTimeout(L"Name Changed", BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            HT_MessageBoxTimeout(L"Please input name!", BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }

}

/*******************************************************************************
**
** Function         bt_ui_menu_stored_device_unbond
**
** Description      Unbond selected device
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_stored_device_unbond ()
{
    char msg_str[64];
    int szLen;
    TCHAR *szWText = bt_ui_get_comm_wbuffer(&szLen);
    BCM_FUN_STAR("bt_ui_menu_stored_device_unbond", 0,0,0);
    if (bcmapp_dm_stored_device_unbond())
    {
        sprintf (msg_str, "Link Key Deleted, Unbond Device %s!",
            bt_ui_cb.p_selected_rem_device->name);
    }
    else
    {
        sprintf (msg_str, "Not Paired or Conn Active, Operation Failed.");
    }

    StrToWStr(msg_str, szWText, szLen);
    HT_MessageBoxTimeout(szWText, BTUI_TIMEOUT_MSG_PERIOD);
}
/*******************************************************************************
**
** Function         bt_ui_act_find_me_devices
**
** Description      Searches for bluetooth devices in find-me mode
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_find_me_devices(void)
{
    bt_ui_search(BTA_DM_LIMITED_INQUIRY, 0, bt_ui_menu_new_device_cback, NULL);
}


/*******************************************************************************
**
** Function         bt_ui_act_search_new_device
**
** Description      User input when search is active
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_search_new_device(tBTUI_MMI_MSG *p_msg)
{

    bcmapp_dm_cancel_search();
    bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
    //BTUI_cls();
    bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_find_new_devices
**
** Description      Searches for nearby bluetooth devices
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_find_new_devices(void)
{
#ifdef FEATURE_BLUETOOTH_APP
    bt_ui_search(BTA_DM_GENERAL_INQUIRY, 0, bt_ui_menu_new_device_cback, NULL);
#endif

#ifdef FEATURE_CS09_BLUETOOTH_APP
    /* filter: Major service class is set on 21st bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x20,0x00, 0x00}, {0x20,0x00, 0x00}};
    BCM_FUN_STAR("bt_ui_dm_find_audio_devices",0,0,0);
    bt_ui_cb.ui_state = UI_NEW_AUDIO_DEVICE;

    bt_ui_search(BTA_DM_GENERAL_INQUIRY,
        bt_ui_cfg.supported_services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK),
        bt_ui_menu_new_device_cback, &dev_filt);
#endif
}

/*******************************************************************************
**
** Function         bt_ui_search
**
** Description      Searches for devices supporting the services specified
**
**
** Returns          void
*******************************************************************************/
void bt_ui_search(tBTA_DM_INQ_MODE mode, tBTA_SERVICE_MASK services,
                 tHT_KEY_PRESSED_CBACK  p_cback, tBTA_DM_COD_COND *p_cod_filt_cond)
{
    tBTA_DM_INQ inq_params;
    BCM_FUN_STAR("bt_ui_search %x %x", mode,services,0);
    bt_ui_cb.p_search_cback = (tHT_KEY_PRESSED_CBACK  *)p_cback;

    inq_params.mode = mode;
    inq_params.duration = BTUI_DEFAULT_INQ_DURATION;
    /* use a shorter duration for limited inquiry */
    if(mode == BTA_DM_LIMITED_INQUIRY)
        inq_params.duration /= 3;
    inq_params.max_resps = bt_ui_cfg.num_inq_devices;

    /* set filter condition if type is not specified in cfg */
    if (bt_ui_cfg.dm_inq_filt_type == BTA_DM_INQ_CLR && p_cod_filt_cond)
    {
        inq_params.filter_type = BTA_DM_INQ_DEV_CLASS;
        memcpy(&inq_params.filter_cond, p_cod_filt_cond, sizeof(tBTA_DM_INQ_COND));
    }
    else
    {
        inq_params.filter_type = bt_ui_cfg.dm_inq_filt_type;
        memcpy(&inq_params.filter_cond, &bt_ui_cfg.dm_inq_filt_cond, sizeof(tBTA_DM_INQ_COND));

        if (inq_params.filter_type == BTA_DM_INQ_BD_ADDR)
            inq_params.max_resps = 1;   /* Only need one response when looking for bd_addr */
    }
#ifndef AEE_SIMULATOR
    /*we don't search all device,*/
    if(services == 0)
    {
        //services = bt_ui_cfg.supported_services;
    }
#endif
    bcmapp_dm_search(services, &inq_params);

    //bt_ui_cb.search_msgbox_handle = bt_ui_message_box(L"Searching ...", bt_ui_menu_searching_cback);
    if(bt_ui_cb.search_menu_handle != BT_UI_NULL_HANDLE)
    {
        bt_ui_close_object(bt_ui_cb.search_menu_handle );
        bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
    }
    bt_ui_set_pop_msg_type(MESSAGE_WAITING);
    bt_ui_cb.search_msgbox_handle = bt_ui_message_box_ex(IDS_SEARCH, bt_ui_menu_searching_cback,BTBAR_CANCEL);

    bt_ui_cb.ui_prev_state = bt_ui_cb.ui_state;
    bt_ui_cb.ui_state = UI_SEARCH_NEW_DEVICE;
}

/*******************************************************************************
**
** Function         bt_ui_menu_searching_cback
**
** Description      Cancels the serach
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_searching_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_menu_searching_cback %x %x %x", handle,key,bt_ui_cb.m_bsend_opc_file);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
        bcmapp_dm_cancel_search();

        bt_ui_close_object(handle);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)        
        /*if in send opc file state,we need close app when in search*/
        if(bt_ui_cb.m_bsend_opc_file)
        {
            bt_ui_close_applet();
        }
#endif        
        bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
        bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
        //bt_ui_act_search_new_device(NULL);
    }
}

static void bt_ui_menu_searching_item_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_menu_searching_item_cback %x %x %x", handle,key,bt_ui_cb.p_search_cback);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
        bcmapp_dm_cancel_search();
        bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_new_device_cback
**
** Description
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_new_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM menu_item;
    BCM_FUN_STAR("bt_ui_menu_new_device_cback %x %x %d", handle,key,bt_ui_cb.ui_state);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
#ifdef FEATURE_BLUETOOTH_APP
        if (bt_ui_cb.ui_state == UI_SEARCH_NEW_DEVICE)
        {
            bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
            bcmapp_dm_cancel_search();
        }
#endif
        if(
#ifdef FEATURE_CS09_BLUETOOTH_APP
            (handle == bt_ui_cb.search_menu_handle)&&
#endif
            (SUCCESS == HT_GetMenuSelectedItem(handle, &menu_item)))
        {
            if(IDS_NO_RECORD == menu_item.item_index)
            {
                bt_ui_act_find_new_devices();
            }
            else
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[menu_item.item_data];
                bt_ui_menu_select_new_device();
            }
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        if (bt_ui_cb.ui_state == UI_SEARCH_NEW_DEVICE)
        {
            bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
            bcmapp_dm_cancel_search();
        }

        // Close the serach handle.
        bt_ui_close_object(handle);
        bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
        bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_show_new_devices
**
** Description      Displays the device found during inquiry
**
**
** Returns          void
*******************************************************************************/
//void bt_ui_act_show_new_devices (void)
//{
//    bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
//}

/*******************************************************************************
**
** Function         bt_ui_menu_select_new_device
**
** Description      Displays the menu options for a newly found device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_new_device()
{
    //char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    //int szLen;
    UINT16 i = 0;
    //TCHAR *szWText = bt_ui_get_comm_wbuffer(&szLen);
    if(g_pBtuiApp->m_bQuicktest)
    {
        p_menu[i].item_string_id= IDS_STRING_PAIR;
        p_menu[i].item_data = (UINT32)bt_ui_act_dev_bond_n_disc;
        p_menu[i].item_index = i;
        i++;
#ifndef AEE_SIMULATOR
        if(bt_ui_cb.p_selected_rem_device->services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK))/*only show ag function when device support HFP/HSP*/
        {
            p_menu[i].item_string_id = IDS_AG_CONNECT;
            p_menu[i].item_data = (UINT32)bt_ui_act_audio_device_connect;
            p_menu[i].item_index = i;
            i++;

            p_menu[i].item_string_id = IDS_BT_MAKE_CALL;
            p_menu[i].item_data = (UINT32)bt_ui_ag_make_quicktest_call;
            p_menu[i].item_index = i;
            i++;
        }
#endif
        p_menu[i].item_string_id = IDS_BLUETOOTH_DISABLE;
        p_menu[i].item_data = (UINT32)bt_ui_act_disable_bt;
        p_menu[i].item_index = i;
        i++;
    }
    else
    {
        //p_menu[i].p_item_name = "Save Device ";
        //p_menu[i].item_data = (UINT32)bt_ui_act_add_new_device;
        //p_menu[i].item_index = i;
        //i++;

        //p_menu[i].p_item_name = "Discover ";
        //p_menu[i].item_data = (UINT32)bt_ui_act_discover_new_device;
        //p_menu[i].item_index = i;
        //i++;

        //p_menu[i].p_item_name = "Find DI ";
        //p_menu[i].item_data = (UINT32)bt_ui_act_discover_di;
        //p_menu[i].item_index = i;
        //i++;
        if(!(bt_ui_cb.p_selected_rem_device->in_use && bt_ui_cb.p_selected_rem_device->link_key_present))
        {
            p_menu[i].item_string_id = IDS_STRING_PAIR;
            p_menu[i].item_data = (UINT32)bt_ui_act_dev_bond;
            p_menu[i].item_index = i;
            i++;
        }
        p_menu[i].item_string_id = IDS_SEL_SERACH;
        p_menu[i].item_data = (UINT32)bt_ui_act_find_new_devices;
        p_menu[i].item_index = i;
        i++;

        p_menu[i].item_string_id = IDS_DEVICE_INFO;
        p_menu[i].item_data = (UINT32)bt_ui_show_device_detail;
        p_menu[i].item_index = i;
        i++;

        //p_menu[i].p_item_name = "Short Name ";
        //p_menu[i].item_data = (UINT32)bt_ui_menu_new_device_name;
        //p_menu[i].item_index = i;
        //i++;

        //p_menu[i].p_item_name = "Authorize ";
        //p_menu[i].item_data = (UINT32)bt_ui_act_trust_new_device;
        //p_menu[i].item_index = i;
        //i++;

        //p_menu[i].p_item_name = "Bond and Discover ";
        //p_menu[i].item_data = (UINT32)bt_ui_act_dev_bond_n_disc;
        //p_menu[i].item_index = i;
        //i++;
    }
    /* Display the name of the device */
    //if(bt_ui_cb.p_selected_rem_device->short_name[0])
    //{
    //    p_name = bt_ui_cb.p_selected_rem_device->short_name;
    //}
    //else if(bt_ui_cb.p_selected_rem_device->name[0])
    //{
    //    p_name =  bt_ui_cb.p_selected_rem_device->name;
    //}
    //else
    //{
    //    p_name = "<NO NAME>";
    //}
    //STRTOWSTR(p_name, szWText, szLen);
    bt_ui_create_menu(p_menu,i, bt_ui_menu_cback, /*szWText*/NULL ,BTBAR_SELECT_BACK,TRUE,0);
    //if(g_pBtuiApp->m_bQuicktest)
    //{
    //    handle = bt_ui_create_menu(p_menu, 4, bt_ui_menu_cback, szWText ,BTBAR_SELECT_BACK,TRUE,0);
    //}
    //else
    //{
    //    handle = bt_ui_create_menu(p_menu, 7, bt_ui_menu_cback, szWText ,BTBAR_SELECT_BACK,TRUE,0);
    //}
}

#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_act_trust_new_device
**
** Description      Sets device as trusted
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_trust_new_device(void)
{
    bcmapp_dm_set_trusted(BTA_ALL_SERVICE_MASK, bt_ui_cb.p_selected_rem_device);

    HT_MessageBoxTimeout(L"Device Authorized", BTUI_TIMEOUT_MSG_PERIOD);
}

/*******************************************************************************
**
** Function         bt_ui_act_add_new_device
**
** Description      Adds a new device to database
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_add_new_device(void)
{
    if(bcmapp_dm_add_device())
    {
        HT_MessageBoxTimeout(L"Device Saved", BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout(L"Database full", BTUI_TIMEOUT_MSG_PERIOD);
    }
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_dev_bond_n_disc
**
** Description      Initiates bonding with selected device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_dev_bond_n_disc ()
{
    bt_ui_cb.menu_action = BTUI_MENU_ACT_BOND_N_DISC | BTUI_MENU_ACT_BOND;
    bcmapp_dm_bond(bt_ui_cb.p_selected_rem_device);
}

/*******************************************************************************
**
** Function         bt_ui_act_dev_bond
**
** Description      Initiates bonding with selected device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_dev_bond (void)
{
    bt_ui_cb.menu_action = BTUI_MENU_ACT_BOND;
    bcmapp_dm_bond(bt_ui_cb.p_selected_rem_device);
    bt_ui_set_pop_msg_type(MESSAGE_WAITING);
    HT_MessageBoxTimeout_ex(IDS_PAIRING, -1);
}

/*******************************************************************************
**
** Function         bt_ui_menu_cancel_bonding_cback
**
** Description      Bonding cancelled
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_cancel_bonding_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_menu_cancel_bonding_cback %x %x", handle,key,0);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        bcmapp_dm_bond_cancel(bt_ui_cb.p_selected_rem_device);
        bt_ui_cb.is_bonding = FALSE;
    }

}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_menu_new_device_name
**
** Description      Prompts user for a new name
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_new_device_name (void)
{

    UINT32 handle;

    //tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);

    handle = bt_ui_edit_box(HT_EB_LETTERS, bt_ui_menu_new_device_name_cback, L"Enter new",BTUI_DEV_NAME_LENGTH,BTBAR_OK_BACK,bt_ui_cb.p_selected_rem_device->short_name,TRUE);
}

/*******************************************************************************
**
** Function         bt_ui_menu_new_device_name_cback
**
** Description      Process user entered name
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_new_device_name_cback(UINT32 handle, UINT32 key)
{
    TCHAR* p_wtext;
    int szLen;
    char *szText = bt_ui_get_comm_buffer(&szLen);
    BCM_FUN_STAR("bt_ui_menu_new_device_name_cback %x %x", handle,key,0);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        p_wtext = HT_GetEditBoxText(handle);
        if(p_wtext && p_wtext[0])
        {
            WSTRTOSTR(p_wtext, szText, szLen);
            bcmapp_dm_rename_device(bt_ui_cb.p_selected_rem_device, szText);

            bt_ui_close_object(handle);
            HT_MessageBoxTimeout(L"Name Changed!", BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            HT_MessageBoxTimeout(L"Please Enter Name!", BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
    else if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}


/*******************************************************************************
**
** Function         bt_ui_act_new_dev_disc_cmpl
**
** Description      Display new devices menu
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_new_dev_disc_cmpl (tBTUI_MMI_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_new_dev_disc_cmpl", 0,0,0);
    //BTUI_cls();
    bt_ui_menu_select_new_device();
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_disc_result_message_cback
**
** Description      Closes the discovery results window
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_disc_result_message_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_act_disc_result_message_cback %x %x", handle,key,0);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}
/*******************************************************************************
**
** Function         bt_ui_display_services_found
**
** Description      Displays services found on discovery
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_display_services_found(tBTUI_DISC_MSG *p_msg)
{
    UINT32 handle;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    tBTA_SERVICE_MASK services = p_msg->services;
    UINT8 i, count = 0;
    BCM_FUN_STAR("bt_ui_display_services_found %x %d", services,p_msg->result,0);
    /* Is remote stack Broadcom? */
    if (services & BTA_RES_SERVICE_MASK)
    {
        p_menu[count].p_item_name = "[Broadcom Stack]";
        p_menu[count].item_data = 0;
        p_menu[count].item_index = count;
        count++;
    }

    if((services & ~BTA_RES_SERVICE_MASK))
    {
        for(i=BTA_RES_SERVICE_ID+1; i<=BTA_MAX_SERVICE_ID; i++)
        {
            if(services & (1<<i))
            {
#if(defined BT_OEM_DEBUG)
                DBGPRINTF("%s\r\n", bt_ui_bta_service[i]);
#endif
                p_menu[count].p_item_name = (char*)bt_ui_bta_service[i];
                p_menu[count].item_data = 0;
                p_menu[count].item_index = count;
                count++;
            }
        }

        handle = bt_ui_create_menu(p_menu, count, bt_ui_act_disc_result_message_cback, L"Services Found",BTBAR_BACK,FALSE,0);
    }
    else
    {
        if (p_msg->result == BTA_FAILURE)
        {
            handle = HT_MessageBoxTimeout(L"Service Search Failure!", BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            handle = HT_MessageBoxTimeout(L"No Services!", BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
}

/*******************************************************************************
**
** Function         bt_ui_display_devices
**
** Description      Displays stored devices supporting the services specified
**
**
** Returns          void
*******************************************************************************/
UINT8 bt_ui_display_devices(tBTA_SERVICE_MASK services, tHT_KEY_PRESSED_CBACK p_menu_callback, TCHAR* sft_key1_msg)
{
    UINT8 i;
#ifdef BT_OEM_DEBUG
    char                msg_str[64];
#endif
    UINT8               count = 0;
#ifdef FEATURE_BLUETOOTH_APP
    BottomBar_e_Type BottomBar = BTBAR_OPTION_BACK;
#endif    

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    BCM_FUN_STAR("bt_ui_display_devices %x", services,0,0);
    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        if (bt_ui_device_db.device[i].in_use )
        {
            if(!services || (services & bt_ui_device_db.device[i].services))
            {
                p_menu[count].p_item_name = bt_ui_get_dev_alias(&bt_ui_device_db.device[i]);
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
                p_menu[count].item_img_id = bt_ui_dm_get_device_image(&bt_ui_device_db.device[i].cod_type);
#endif                
                p_menu[count].item_data = i;
                p_menu[count].item_index= i;
                //bt_ui_device_db_to_menu_option_lkup_tbl[count] = i;
                count++;
#ifdef BT_OEM_DEBUG
                sprintf (msg_str, "  %d  %02x:%02x:%02x:%02x:%02x:%02x services=%08X\n",  i+1,
                    bt_ui_device_db.device[i].bd_addr[0], bt_ui_device_db.device[i].bd_addr[1],
                    bt_ui_device_db.device[i].bd_addr[2], bt_ui_device_db.device[i].bd_addr[3],
                    bt_ui_device_db.device[i].bd_addr[4], bt_ui_device_db.device[i].bd_addr[5],
                    bt_ui_device_db.device[i].services);
                DBGPRINTF(msg_str);
#endif
            }
        }
    }

    //bt_ui_cb.num_devices = count;
#ifdef FEATURE_BLUETOOTH_APP
    if(0 == count)
    {
        p_menu[count].item_data = (UINT32)IDS_NO_RECORD;
        p_menu[count].item_index = IDS_NO_RECORD;
        p_menu[count].item_string_id= IDS_NO_RECORD;
        count++;
        BottomBar = BTBAR_FIND_BACK;
    }
    bt_ui_cb.select_bonded_handle = bt_ui_create_menu(p_menu, count, p_menu_callback, bt_ui_get_brx_string(IDS_PAIRED_LIST) ,BottomBar,FALSE,~MP_BIND_ITEM_TO_NUMBER_KEY);
    return count;
#endif
#if 0
    if(0 == count)
    {
        p_menu[0].p_item_name = "";
        p_menu[count].item_data = 0;
        bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
        handle = bt_ui_message_box_ex(IDS_NO_RECORD, p_menu_callback,BTBAR_BACK);
    }
    else
    {
        handle = bt_ui_create_menu(p_menu, count, p_menu_callback, L"SAVED DEVICES" ,BTBAR_SELECT_BACK,FALSE,0);
    }
    return count;
#endif
}

/*******************************************************************************
**
** Function         bt_ui_display_bonded_devices
**
** Description      Displays stored devices bonded.
**
** Returns          void
*******************************************************************************/
static UINT8 bt_ui_display_bonded_devices(tHT_KEY_PRESSED_CBACK p_menu_callback, TCHAR* sft_key2_msg)
{
    UINT8 i;
#ifdef BT_OEM_DEBUG
    char                msg_str[64];
#endif
    UINT8               count = 0;
#ifdef FEATURE_BLUETOOTH_APP
    BottomBar_e_Type BottomBar = BTBAR_OPTION_BACK;
#endif    
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    BCM_FUN_STAR("bt_ui_display_bonded_devices %x", bt_ui_cb.show_bonded_service, 0 ,0);
    bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    for(i=0; i < BTUI_NUM_REM_DEVICE; i++)
    {
#ifdef BT_OEM_DEBUG
#if 0
        sprintf (msg_str, "%d  %02x:%02x:%02x:%02x:%02x:%02x services=%08X in_use %d %d\n",  i+1,
            bt_ui_device_db.device[i].bd_addr[0], bt_ui_device_db.device[i].bd_addr[1],
            bt_ui_device_db.device[i].bd_addr[2], bt_ui_device_db.device[i].bd_addr[3],
            bt_ui_device_db.device[i].bd_addr[4], bt_ui_device_db.device[i].bd_addr[5],
            bt_ui_device_db.device[i].services,bt_ui_device_db.device[i].in_use,bt_ui_device_db.device[i].link_key_present);
        DBGPRINTF(msg_str);
#endif
#endif
        if (bt_ui_device_db.device[i].in_use && bt_ui_device_db.device[i].link_key_present && ((bt_ui_cb.show_bonded_service==0) ||(bt_ui_cb.show_bonded_service & bt_ui_device_db.device[i].services) != 0))
        {
            p_menu[count].p_item_name = bt_ui_get_dev_alias(&bt_ui_device_db.device[i]);
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
            p_menu[count].item_img_id = bt_ui_dm_get_device_image(&bt_ui_device_db.device[i].cod_type);
#endif            
            p_menu[count].item_data = i;
            p_menu[count].item_index= count;
            count++;
#ifdef BT_OEM_DEBUG
            sprintf (msg_str, "%d  %02x:%02x:%02x:%02x:%02x:%02x services=%08X\n",  i+1,
                    bt_ui_device_db.device[i].bd_addr[0], bt_ui_device_db.device[i].bd_addr[1],
                    bt_ui_device_db.device[i].bd_addr[2], bt_ui_device_db.device[i].bd_addr[3],
                    bt_ui_device_db.device[i].bd_addr[4], bt_ui_device_db.device[i].bd_addr[5],
                    bt_ui_device_db.device[i].services);
            DBGPRINTF(msg_str);
#endif
        }
    }
    //bt_ui_cb.num_devices = count;

#ifdef FEATURE_BLUETOOTH_APP
    if(0 == count)
    {
        //p_menu[0].p_item_name = "";
        //p_menu[count].item_data = 0;
        //handle = bt_ui_message_box(L"No Bonded Devices", p_menu_callback,BTBAR_BACK);
        //bt_ui_set_pop_msg_type(MESSAGE_ERR);
        //if(bt_ui_cb.m_bsend_opc_file)
        //{
        //    handle = bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_NO_RECORD), p_menu_callback, 0, BTBAR_FIND_BACK);
        //}
        //else
        //{
        //    handle = bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_NO_RECORD), p_menu_callback,BTUI_TIMEOUT_MSG_PERIOD,BTBAR_BACK);
        //}
        p_menu[count].item_data = (UINT32)IDS_NO_RECORD;
        p_menu[count].item_index = IDS_NO_RECORD;
        p_menu[count].item_string_id= IDS_NO_RECORD;
        count++;
        //if(bt_ui_cb.m_bsend_opc_file)
        //{
        //    BottomBar = BTBAR_FIND_BACK;
        //}
        //else
        //{
        //    BottomBar = BTBAR_BACK;
        //}
        BottomBar = BTBAR_FIND_BACK;
    }

    //if(bt_ui_cb.m_bsend_opc_file)
    //{
    //    handle = bt_ui_create_menu(p_menu, count, p_menu_callback, bt_ui_get_brx_string(IDS_PAIRED_LIST) ,BTBAR_FIND_BACK,FALSE,0);
    //}
    //else
    //{
    //    handle = bt_ui_create_menu(p_menu, count, p_menu_callback, bt_ui_get_brx_string(IDS_PAIRED_LIST) ,BTBAR_OPTION_BACK,FALSE,0);
    //}
    bt_ui_cb.select_bonded_handle = bt_ui_create_menu(p_menu, count, p_menu_callback, bt_ui_get_brx_string(IDS_PAIRED_LIST) ,BottomBar,FALSE,~MP_BIND_ITEM_TO_NUMBER_KEY);
    return count;
#endif

#ifdef FEATURE_CS09_BLUETOOTH_APP
    if(0 == count)
    {
        bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_NO_DEVICE_FOUND), p_menu_callback,BTUI_TIMEOUT_MSG_PERIOD,BTBAR_BACK);//L"No bonded devices"
    }
    else
    {
        UINT32 handle;
        handle = bt_ui_create_menu(p_menu, count, p_menu_callback, bt_ui_get_brx_string(IDS_PAIRED_LIST), BTBAR_SELECT_BACK, FALSE, 0);//L"Paired devices" 
        bt_ui_cb.select_bonded_handle = handle;
    }
    return count;
#endif
}

/*******************************************************************************
**
** Function         bt_ui_show_search_results
**
** Description      Displays the device found during device search
**
**
** Returns          void
*******************************************************************************/
void bt_ui_show_search_results (void)
{
#ifdef BT_OEM_DEBUG
    char                *msg_str = bt_ui_get_comm_buffer(NULL);
#endif
    tHT_MENU_ITEM       *p_menu = bt_ui_get_menu_item_buffer(NULL);
    tBTUI_REM_DEVICE    *p_device_rec ;
    UINT8               i = 0, xx;

    BCM_FUN_STAR("bt_ui_show_search_results: %d %x %x remote devices in inquiry db", bt_ui_inq_db.rem_index,bt_ui_cb.search_msgbox_handle,bt_ui_cb.search_menu_handle );

    for(xx = 0; xx < bt_ui_inq_db.rem_index; xx++)
    {
        p_device_rec = &bt_ui_inq_db.remote_device[xx];

        if( !p_device_rec->in_use )
            continue;

        if(!bt_ui_cb.search_services || (bt_ui_cb.search_services & p_device_rec->services))
        {
            if (p_device_rec && p_device_rec->short_name[0])
            {
                p_menu[i].p_item_name = p_device_rec->short_name;
            }
            //else if (bt_ui_inq_db.remote_device[i].short_name[0])
            //{
            //    p_menu[i].p_item_name = bt_ui_inq_db.remote_device[i].short_name;
            //}
            else if (p_device_rec && p_device_rec->name[0])
            {
                p_menu[i].p_item_name = p_device_rec->name;
            }
            //else if (bt_ui_inq_db.remote_device[i].name[0])
            //{
            //    p_menu[i].p_item_name = bt_ui_inq_db.remote_device[i].name;
            //}
            else
            {
                p_menu[i].p_item_name = "<NO NAME>";
            }
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE            
            p_menu[i].item_img_id = bt_ui_dm_get_device_image(&p_device_rec->cod_type);
#endif            
            p_menu[i].item_index = xx;
            p_menu[i].item_data = xx;
#ifdef BT_OEM_DEBUG
            sprintf (msg_str, "%d  %02x:%02x:%02x:%02x:%02x:%02x\n,name [%s]  cod %02x%02x%02x, services %8x \n",  i+1,
                bt_ui_inq_db.remote_device[xx].bd_addr[0], bt_ui_inq_db.remote_device[xx].bd_addr[1],
                bt_ui_inq_db.remote_device[xx].bd_addr[2], bt_ui_inq_db.remote_device[xx].bd_addr[3],
                bt_ui_inq_db.remote_device[xx].bd_addr[4], bt_ui_inq_db.remote_device[xx].bd_addr[5],
                bt_ui_inq_db.remote_device[xx].name, bt_ui_inq_db.remote_device[xx].dev_class[0],
                bt_ui_inq_db.remote_device[xx].dev_class[1], bt_ui_inq_db.remote_device[xx].dev_class[2],
                bt_ui_inq_db.remote_device[xx].services);
            DBGPRINTF(msg_str);
#endif
            i++;
        }
    }

    if(BT_UI_NULL_HANDLE == bt_ui_cb.search_menu_handle)
    {
        bt_ui_cb.search_menu_handle = bt_ui_create_menu(p_menu, i,
                                                                bt_ui_menu_searching_item_cback, bt_ui_get_brx_string(IDS_SEARCH),BTBAR_CANCEL,FALSE,
#ifdef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
                                                                ~MP_BIND_ITEM_TO_NUMBER_KEY
#else
                                                                0
#endif
                                                                );
    }
    else
    {
        HT_ClearMenuItems(bt_ui_cb.search_menu_handle);
        HT_AddMenuItems(bt_ui_cb.search_menu_handle, p_menu, i);
        HT_UpdateHandle(bt_ui_cb.search_menu_handle);
    }

    if(bt_ui_cb.search_msgbox_handle)
    {
        bt_ui_close_object(bt_ui_cb.search_msgbox_handle);
        bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
    }
}


/*******************************************************************************
**
** Function         bt_ui_show_new_devices
**
** Description      Displays the device found during device search
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_new_devices (tHT_KEY_PRESSED_CBACK  p_menu_callback)
{
    UINT32 prev_handle;
    UINT8               i = 0, xx;
#ifdef BT_OEM_DEBUG
    char                msg_str[255];
#endif
    tBTUI_REM_DEVICE    *p_device_rec ;
    tHT_MENU_ITEM       *p_menu = bt_ui_get_menu_item_buffer(NULL);
    BottomBar_e_Type BottomBar = BTBAR_OPTION_BACK;
    BCM_FUN_STAR("bt_ui_show_new_devices: %d remote devices in inquiry db", bt_ui_inq_db.rem_index,0,0 );
    for(xx = 0; xx < bt_ui_inq_db.rem_index; xx++)
    {
        p_device_rec = &bt_ui_inq_db.remote_device[xx];

        if( !p_device_rec->in_use )
            continue;

        if(!bt_ui_cb.search_services || (bt_ui_cb.search_services & p_device_rec->services))
        {
            if (p_device_rec && p_device_rec->short_name[0])
            {
                p_menu[i].p_item_name = p_device_rec->short_name;
            }
            else if (bt_ui_inq_db.remote_device[i].short_name[0])
            {
                p_menu[i].p_item_name = bt_ui_inq_db.remote_device[i].short_name;
            }
            else if (p_device_rec && p_device_rec->name[0])
            {
                p_menu[i].p_item_name = p_device_rec->name;
            }
            else if (bt_ui_inq_db.remote_device[i].name[0])
            {
                p_menu[i].p_item_name = bt_ui_inq_db.remote_device[i].name;
            }
            else
            {
                p_menu[i].p_item_name = "<NO NAME>";
            }
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
            p_menu[i].item_img_id = bt_ui_dm_get_device_image(&p_device_rec->cod_type);
#endif            
            p_menu[i].item_index = xx;
            p_menu[i].item_data = xx;
#ifdef BT_OEM_DEBUG
            sprintf (msg_str, "  %d  %02x:%02x:%02x:%02x:%02x:%02x(%d)(%d) %s, services=%08X\n, cod %02x%02x%02x ",  i+1,
                bt_ui_inq_db.remote_device[i].bd_addr[0], bt_ui_inq_db.remote_device[i].bd_addr[1],
                bt_ui_inq_db.remote_device[i].bd_addr[2], bt_ui_inq_db.remote_device[i].bd_addr[3],
                bt_ui_inq_db.remote_device[i].bd_addr[4], bt_ui_inq_db.remote_device[i].bd_addr[5],
                p_device_rec->rssi, (p_device_rec->rssi + p_device_rec->rssi_offset),
                p_menu[i].p_item_name, p_device_rec->services,
                bt_ui_inq_db.remote_device[i].dev_class[0],bt_ui_inq_db.remote_device[i].dev_class[1],
                bt_ui_inq_db.remote_device[i].dev_class[2]);
            DBGPRINTF(msg_str);
#endif
            i++;
        }
    }

    if(i == 0)
    {
        //prev_handle = bt_ui_cb.search_msgbox_handle;
        //bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
        //bt_ui_cb.search_msgbox_handle = bt_ui_message_box_ex(IDS_NO_RECORD , p_menu_callback,BTBAR_BACK);

        //if(prev_handle)
        //    bt_ui_close_object(prev_handle);

        p_menu[i].item_data = (UINT32)IDS_NO_RECORD;
        p_menu[i].item_index = IDS_NO_RECORD;
        p_menu[i].item_string_id= IDS_NO_RECORD;
        i++;
        BottomBar = BTBAR_FIND_BACK;
    }
    
    //else
    {
        BCM_FUN_STAR("Rebuild the serach result menu%x %x", bt_ui_cb.search_menu_handle,bt_ui_cb.search_msgbox_handle,0 );
        prev_handle = bt_ui_cb.search_menu_handle;
        // Rebuild the serach result menu.
        bt_ui_cb.search_menu_handle = bt_ui_create_menu(p_menu, i, p_menu_callback, bt_ui_get_brx_string(IDS_ADD_NEW) ,BottomBar,FALSE,
#ifdef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
                                                                ~MP_BIND_ITEM_TO_NUMBER_KEY
#else
                                                                0
#endif
        );
        if(bt_ui_cb.search_msgbox_handle)
        {
            bt_ui_close_object(bt_ui_cb.search_msgbox_handle);
            bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
        }
        if(prev_handle)
        {
            bt_ui_close_object(prev_handle);
            bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
        }
    }
}
#if 0
/*****************************************************************************
**  Function        bt_ui_bdaddr_2_ascii
**
**  Description     This function converts an ASCII string into a BD_ADDR
**
******************************************************************************/
void bt_ui_bdaddr_2_ascii (UINT8 *p_bd, char *p_ascii)
{
    int     x;
    UINT8   c;
    BCM_FUN_STAR("bt_ui_bdaddr_2_ascii", 0,0,0);
    for (x = 0; x < BD_ADDR_LEN; x++)
    {
        c = (*p_bd >> 4) & 0x0f;
        if (c < 10)
            *p_ascii++ = c + '0';
        else
            *p_ascii++ = c - 10 + 'A';

        c = *p_bd & 0x0f;
        if (c < 10)
            *p_ascii++ = c + '0';
        else
            *p_ascii++ = c - 10 + 'A';

        p_bd++;
    }
    *p_ascii++ = '\0';
}
#endif
static void bt_ui_act_enable_msg_cb(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_act_enable_msg_cb %d", bt_enable_from,0,0);
    bt_ui_close_object(handle);
    if(bt_enable_from == BT_ENABLE_FROM_SETTING)
    {
        if(bt_ui_cb.current_menu_handle != BT_UI_NULL_HANDLE)
        {
            bt_ui_close_object(bt_ui_cb.current_menu_handle);
            bt_ui_cb.current_menu_handle = BT_UI_NULL_HANDLE;
        }
        // Rebuild the setting menu.
        bt_ui_menu_setting();
    }
    else if((bt_enable_from == BT_ENABLE_FROM_PAIRED_DEVICE) || (BT_ENABLE_FROM_SEND_FILE == bt_enable_from))
    {
        if(BT_ENABLE_FROM_SEND_FILE == bt_enable_from)
        {
            bt_ui_cb.show_bonded_service = BTA_OPP_SERVICE_MASK;/*in this state,only show opp device*/
        }
        bt_ui_act_show_bonded_device();
    }

    else if(bt_enable_from == BT_ENABLE_FROM_ADD_NEW_DEVIVE)
    {
        bt_ui_act_find_new_devices();
    }
    else if(bt_enable_from == BT_ENABLE_FROM_APP_START)
    {
        bt_ui_menu_idle();
    }

    bt_enable_from = BT_ENABLE_FROM_APP_START;
}
/*******************************************************************************
**
** Function         bt_ui_act_enable_evt
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_enable_evt (void)
{
    BCM_FUN_STAR("bt_ui_act_enable_evt", 0,0,0);
    bt_ui_set_draw_hold_down(TRUE);
    bt_ui_print_btaddr(bt_ui_cb.local_bd_addr);
    //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_BT_SET_ON), BTUI_TIMEOUT_MSG_PERIOD);
    bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
    bt_ui_message_box_timeout(bt_ui_get_brx_string(IDS_BT_SET_ON), bt_ui_act_enable_msg_cb,BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
#ifndef AEE_SIMULATOR
    IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_ON);
#endif
    /*if(bt_enable_from == BT_ENABLE_FROM_SETTING)
    {
        // Rebuild the setting menu.
        bt_ui_menu_setting();
    }
    else if(bt_enable_from == BT_ENABLE_FROM_PAIRED_DEVICE)
    {
        bt_ui_act_show_bonded_device();
    }
    else if(bt_enable_from == BT_ENABLE_FROM_ADD_NEW_DEVIVE)
    {
        bt_ui_act_find_new_devices();
    }
    else if(bt_enable_from == BT_ENABLE_FROM_APP_START)
    {
        bt_ui_menu_idle();
    }

    bt_enable_from = BT_ENABLE_FROM_APP_START;*/

    bt_ui_set_draw_hold_down(FALSE);
}

/*******************************************************************************
**
** Function         bt_ui_act_disable_evt
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_disable_evt (void)
{
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
    if (bt_ui_fm_cb.play_via != BTUI_FM_VIA_WIRED)
        bcmapp_fm_disable();
#endif
    BCM_FUN_STAR("bt_ui_act_disable_evt", 0,0,0);
    bt_ui_set_draw_hold_down(TRUE);
#ifndef AEE_SIMULATOR
    IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_OFF);
#endif
    //HT_MessageBoxTimeout_ex(IDS_BT_SET_OFF, BTUI_TIMEOUT_MSG_PERIOD);
    // Rebuild the idle menu.
    if(bt_ui_cb.current_menu_handle != BT_UI_NULL_HANDLE)
    {
        bt_ui_close_object(bt_ui_cb.current_menu_handle);
        bt_ui_cb.current_menu_handle = BT_UI_NULL_HANDLE;
    }
    if(g_pBtuiApp->m_bQuicktest == FALSE)
    {
        bt_ui_menu_setting();
    }
    else
    {
        bt_ui_close_applet();
    }
    g_pBtuiApp->m_bQuicktest = FALSE;

    bt_ui_set_draw_hold_down(FALSE);
}
static void bt_ui_act_ag_auth_cmpl_evt_cb(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_act_ag_auth_cmpl_evt_cb %x %x", handle,key,0);
    bt_ui_close_object(handle);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        bt_ui_act_audio_device_connect();
    }
    else if(key == HT_KEY_SOFT2)
    {
        if(BT_UI_NULL_HANDLE != bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
        else if(BT_UI_NULL_HANDLE != bt_ui_cb.select_bonded_handle)
        {
            /*if current windows is bond list, need redraw */
            bt_ui_close_object(bt_ui_cb.select_bonded_handle);
            bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
            bt_ui_display_bonded_devices(bt_ui_show_stored_device_cback, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bt_ui_act_auth_cmpl_evt
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_auth_cmpl_evt(tBTUI_BTA_MSG *p_msg)
{
    char * p_name;
    uint16 strid = 0;
    tBTUI_REM_DEVICE *device = NULL;
    //char   msg_str[64];
    //int szLen;
    //TCHAR *szWText = bt_ui_get_comm_wbuffer(&szLen);
    BCM_FUN_STAR("auth_cmpl menu %d bond %x resume %x", bt_ui_cb.menu_action, bt_ui_cb.select_bonded_handle,bt_ui_cb.after_resume_menu);
    //bt_ui_set_draw_hold_down(TRUE);

    /* close pin request or authorization request menu if its being displayed */
    if (bt_ui_cb.auth_pin_active && !bdcmp(bt_ui_cb.peer_bdaddr, p_msg->auth_cmpl.bd_addr))
    {
        bt_ui_cb.auth_pin_active = FALSE;
        bt_ui_close_object(bt_ui_cb.auth_handle);
        bt_ui_cb.ui_state = bt_ui_cb.ui_next_state;
    }

    if(!p_msg->auth_cmpl.is_success)
    {
        if(p_msg->auth_cmpl.dev_name[0])
            p_name = (char*)p_msg->auth_cmpl.dev_name;
        else
            p_name = "<No Name>";

//      if(bt_ui_cb.is_bonding && (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND))
//      {
//          // Close the bonding message box.
//          bt_ui_close_object(bt_ui_cb.current_bonding_handle);
//      }
//      bt_ui_cb.is_bonding = FALSE;
        if(bt_ui_cb.is_bonding)
        {
            bt_ui_cb.is_bonding = FALSE;
            // Close the bonding message box.
            bt_ui_close_object(bt_ui_cb.current_bonding_handle);
        }

        //sprintf (msg_str, "Authentication With %s failed!",p_name);
        //STRTOWSTR(msg_str, szWText, szLen);
        //HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        strid = IDS_FAIL;
        bt_ui_cb.menu_action = 0;
    }
    else if (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND)
    {
        if (bt_ui_cb.is_bonding)
        {
            // Close the bonding message box.
            bt_ui_close_object(bt_ui_cb.current_bonding_handle);
        }
        /* Start service discovery if needed */
        if (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND_N_DISC)
        {
            bt_ui_act_discover_stored_device();
        }
        else
        {
            //tBTUI_REM_DEVICE *device = NULL;
            strid = IDS_DONE;
            device = bt_ui_get_device_record(p_msg->auth_cmpl.bd_addr);
            if(device)
            {
                BCM_MSG_MED("bt_ui_act_auth_cmpl_evt %x %d", device->services,bt_ui_cb.menu_action,0);
                if(((device->services & BTA_HSP_SERVICE_MASK) || (device->services & BTA_HFP_SERVICE_MASK)) && (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND))
                {
                    strid = IDS_AG_CONNECT;
                }
            }
            bt_ui_cb.is_bonding = FALSE;

            //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_DONE), BTUI_TIMEOUT_MSG_PERIOD);
            //strid = IDS_DONE;
            bt_ui_cb.menu_action = 0;
        }
    }
    else
    {
        //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_DONE), BTUI_TIMEOUT_MSG_PERIOD);
        strid = IDS_DONE;
    }

    //if(TRUE == p_msg->auth_cmpl.is_success)
    //{
    //    if(NULL == device)
    //    {
    //        device = bt_ui_get_device_record(p_msg->auth_cmpl.bd_addr);
    //    }

    //    if(device)
    //    {
    //        device->bond_time = GETUTCSECONDS();
    //    }
    //}
    if(strid != 0)
    {
        if(strid == IDS_AG_CONNECT)
        {
            bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
            bt_ui_message_box_timeout(bt_ui_get_brx_string(strid), bt_ui_act_ag_auth_cmpl_evt_cb, 0,BTBAR_OK_CANCEL);
        }
        else
        {
            //if(IDS_FAIL == strid)
            //{
            //    bt_ui_set_pop_msg_type(MESSAGE_WARNNING);
            //}
            if(BT_UI_NULL_HANDLE != bt_ui_cb.select_bonded_handle)
            {
                /*if current windows is bond list, need redraw */
                bt_ui_close_object(bt_ui_cb.select_bonded_handle);
                bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
                bt_ui_display_bonded_devices(bt_ui_show_stored_device_cback, NULL);
            }
            HT_MessageBoxTimeout_ex(strid, BTUI_TIMEOUT_MSG_PERIOD);
            //bt_ui_message_box_timeout(bt_ui_get_brx_string(strid), bt_ui_act_ag_auth_cmpl_evt_cb, BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
        }
    }

    //if(bt_ui_cb.after_resume_menu)
    //{
    //    bt_ui_cb.after_resume_menu();
    //    bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    //}
    //else if(BT_UI_NULL_HANDLE != bt_ui_cb.select_bonded_handle)
    //{
    //    /*if current windows is bond list, need redraw */
    //    bt_ui_close_object(bt_ui_cb.select_bonded_handle);
    //    bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    //    bt_ui_display_bonded_devices(bt_ui_show_stored_device_cback, NULL);
    //}
    //bt_ui_set_draw_hold_down(FALSE);
}
/*******************************************************************************
**
** Function         bt_ui_act_search_discv_res
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_search_discv_res(tBTUI_BTA_MSG *p_msg)
{
#ifdef BT_OEM_DEBUG
    //char                *msg_str = bt_ui_get_comm_buffer(NULL);
    //sprintf (msg_str, "%02x:%02x:%02x:%02x:%02x:%02x\n, name [%s] %x \n",
    //    p_msg->disc_result.bd_addr[0], p_msg->disc_result.bd_addr[1],
    //    p_msg->disc_result.bd_addr[2], p_msg->disc_result.bd_addr[3],
    //    p_msg->disc_result.bd_addr[4], p_msg->disc_result.bd_addr[5],
    //    p_msg->disc_result.dev_name,p_msg->disc_result.services);
    //DBGPRINTF(msg_str);
#endif

    BCM_FUN_STAR("bt_ui_act_search_discv_res", 0,0,0);
    if(!bt_ui_cb.search_services ||  (bt_ui_cb.search_services & p_msg->disc_result.services))
    {
        if (!bt_ui_cb.search_services)
        {
            bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
        }
        else
        {
            bt_ui_show_search_results();
        }
    }
}
/*******************************************************************************
**
** Function         bt_ui_act_device_discv_res
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_device_discv_res(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_device_discv_res", 0,0,0);
   if (bt_ui_cb.ui_state != UI_NEW_DEVICE_DISCOVER_CMPL)
        return;

    /* Clean up bonding state if combined bond and discover */
    if (bt_ui_cb.menu_action & BTUI_MENU_ACT_BOND_N_DISC)
    {
        bt_ui_cb.is_bonding = FALSE;

        /*        HT_MessageBoxTimeout("Bond succeed !", BTUI_TIMEOUT_MSG_PERIOD);*/
        bt_ui_cb.menu_action = 0;
    }
 
    bt_ui_close_object(bt_ui_cb.current_menu_handle);
    bt_ui_display_services_found(&(p_msg->disc_result));
}

/*******************************************************************************
**
** Function         bt_ui_act_discv_cmpl
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_discv_cmpl(tBTUI_BTA_MSG *p_msg)
{
    bt_ui_cb.ui_state  = bt_ui_cb.ui_prev_state;
    BCM_FUN_STAR("bt_ui_act_discv_cmpl %x %x", p_msg->disc_result.result,p_msg->disc_result.services,0);
    //bt_ui_print_btaddr(p_msg->disc_result.bd_addr);
//  if (HT_GetCurrMenuHandle() == bt_ui_cb.current_search_menu_handle)
//      bt_ui_close_object(bt_ui_cb.current_search_menu_handle);
#if BTA_DI_INCLUDED
    if (p_msg->hdr.layer_specific == BTA_DM_DI_DISC_CMPL_EVT)
    {
        bt_ui_show_di_record_menu(p_msg->hdr.offset);
    }
    else
#endif
    {
        bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
    }

    bt_ui_cb.search_services = 0;
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_disp_di_record
**
** Description
**
** Returns          void
*******************************************************************************/
void  bt_ui_disp_di_record(tBTA_DI_GET_RECORD *p_rec)
{
    int szLen;
    TCHAR *szWText = bt_ui_get_comm_wbuffer(&szLen);
    BCM_FUN_STAR("bt_ui_disp_di_record", 0,0,0);
    WSPRINTF (szWText, szLen, L"Primary:[%d] SpecID: [%04x] VendorID: [%04x] Version [%04x] ProductID: [%04x]",
        p_rec->rec.primary_record, p_rec->spec_id, p_rec->rec.vendor, p_rec->rec.version,
        p_rec->rec.product);

    HT_MessageBoxTimeout(szWText, BTUI_TIMEOUT_MSG_PERIOD*2);
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_inq_res
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_inq_res(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_inq_res %d", bt_ui_cb.search_services,0,0);
    if (!bt_ui_cb.search_services)
    {
//      boolean bSearchResRending = FALSE;
//
//      if(bt_ui_cb.current_search_menu_handle)
//          bSearchResRending = HT_IsActiveHandle(bt_ui_cb.current_search_menu_handle);
//      else if (bt_ui_cb.search_msgbox_handle)
//          bSearchResRending = HT_IsActiveHandle(bt_ui_cb.search_msgbox_handle);
//
//      if(bSearchResRending)
        {
            bt_ui_show_search_results();
        }
//      else
//      {
//          bcmapp_dm_cancel_search();
//      }
    }
}
/*******************************************************************************
**
** Function         bt_ui_act_inq_cmpl
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_inq_cmpl(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_inq_cmpl %d", bt_ui_cb.search_services,0,0);
    if (!bt_ui_cb.search_services)
    {
//      if(HT_GetCurrMenuHandle() == bt_ui_cb.current_search_menu_handle)
//          bt_ui_close_object(bt_ui_cb.current_search_menu_handle);
        bt_ui_show_new_devices((tHT_KEY_PRESSED_CBACK *)bt_ui_cb.p_search_cback);
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_link_down
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_link_down(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_link_down", 0,0,0);
    /* close pin request or authorization request menu if its being displayed */
    if(bt_ui_cb.auth_pin_active && !bdcmp(bt_ui_cb.peer_bdaddr, p_msg->link_change.bd_addr))
    {
        bt_ui_close_object(bt_ui_cb.auth_handle);
        bt_ui_cb.auth_pin_active = FALSE;
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_bond_cancel_cmpl
**
** Description
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_bond_cancel_cmpl(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_act_bond_cancel_cmpl", 0,0,0);
#if 0    
    if (p_msg->bond_cancel_cmpl.result == BTUI_SUCCESS)
        HT_MessageBoxTimeout(L"Pairing canceled", BTUI_TIMEOUT_MSG_PERIOD);
    else
        HT_MessageBoxTimeout(L"Pairing could not be canceled", BTUI_TIMEOUT_MSG_PERIOD);
#endif        
}

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bt_ui_act_switch_stack_cmpl
**
** Description
**
** Returns          void
*******************************************************************************/
void bt_ui_act_switch_stack_cmpl(tBTUI_BTA_MSG *p_msg)
{
    /* Display message only if this is a result of manual switch */
    if (bt_ui_cb.manual_switch == TRUE)
    {
        /* Introduce a delay for scripts to make use of
        "Switching from MM->BB/BB->MM" message */
        GKI_delay(1000);
        if (p_msg->switch_stack_cmpl.result == BTUI_SUCCESS)
            HT_MessageBoxTimeout("Switch success!", BTUI_TIMEOUT_MSG_PERIOD);
        else
            HT_MessageBoxTimeout("Switch failed!", BTUI_TIMEOUT_MSG_PERIOD);

        bt_ui_cb.manual_switch = FALSE;
    }
}
#endif

/*******************************************************************************
**
** Function         bt_ui_bta_evt_hdlr
**
** Description      Process events from BTA
**
** Returns          void
*******************************************************************************/
void bt_ui_bta_dm_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_dm_evt_hdlr event = %d", p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
    //case BTUI_MMI_CONN_UP:
    //    bt_ui_act_conn_up (p_msg);
    //    break;

    //case BTUI_MMI_CONN_DOWN:
    //    bt_ui_act_conn_down (p_msg);
    //    break;

    case BTUI_MMI_PIN_REQ:
        if(TRUE == bt_ui_get_suspend())
        {
            bt_ui_process_suspend_event(p_msg,bt_ui_act_pin_req);
        }
        else
        {
            bt_ui_act_pin_req (p_msg);
        }
        break;

#if (BTM_LISBON_INCLUDED == TRUE)
    case BTUI_MMI_CFM_REQ:
        bt_ui_act_cfm_req (p_msg);
        break;
    case BTUI_MMI_KEY_NOTIF:
        bt_ui_act_key_notif (p_msg);
        break;
    case BTUI_MMI_KEY_PRESS: /* key press notification, only used in passkey entry mode */
        bt_ui_act_key_press_notif(p_msg);
        break;

    case BTUI_MMI_RMT_OOB:
        bt_ui_act_rmt_oob (p_msg);
        break;
#endif

    case BTUI_MMI_AUTH_REQ:
        if(TRUE == bt_ui_get_suspend())
        {
            bt_ui_process_suspend_event(p_msg,bt_ui_act_auth_req);
        }
        else
        {
            bt_ui_act_auth_req (p_msg);
        }
        break;

    case BTUI_MMI_ENABLE:
        bt_ui_act_enable_evt();
        break;

    case BTUI_MMI_DISABLE:
        bt_ui_act_disable_evt();
        break;

    case BTUI_MMI_AUTH_CMPL:
        bt_ui_act_auth_cmpl_evt (p_msg);
        break;

    case BTUI_MMI_SEARCH_DISCV_RES:
        bt_ui_act_search_discv_res (p_msg);
        break;

    case BTUI_MMI_DISCV_CMP:
        bt_ui_act_discv_cmpl (p_msg);
        break;

    case BTUI_MMI_INQ_CMP:
        bt_ui_act_inq_cmpl (p_msg);
        break;

    case BTUI_MMI_INQ_RES:
        bt_ui_act_inq_res (p_msg);
        break;

    case BTUI_MMI_LINK_UP:
        bt_ui_proc_link_change(p_msg->link_change.bd_addr, TRUE);
        break;

    case BTUI_MMI_LINK_DOWN:
        bt_ui_proc_link_change(p_msg->link_change.bd_addr, FALSE);
        bt_ui_act_link_down (p_msg);
        break;

    case BTUI_MMI_DEVICE_DISCV_RES:
        bt_ui_act_device_discv_res (p_msg);
        break;

    case BTUI_MMI_BOND_CANCEL_CMPL_EVT:
        bt_ui_act_bond_cancel_cmpl(p_msg);
        break;

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    case BTUI_MMI_SWITCH_STACK_CMPL_EVT:
        bt_ui_act_switch_stack_cmpl(p_msg);
        break;
#endif

    default:
        break;
    }

}

void bt_ui_print_btaddr(BD_ADDR   bd_addr)
{
    char msg_str[64];
    sprintf (msg_str, "print_btaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
        bd_addr[0], bd_addr[1],
        bd_addr[2], bd_addr[3],
        bd_addr[4], bd_addr[5]);
    DBGPRINTF(msg_str);
}

void bt_ui_enable_backlight(void)
{
    IBacklight   *m_pBacklight = NULL;
    if (ISHELL_CreateInstance(bt_ui_get_shell(),
                                            AEECLSID_BACKLIGHT,
                                            (void **)&m_pBacklight)==AEE_SUCCESS)
    {
        IBACKLIGHT_Enable(m_pBacklight);
        IBACKLIGHT_Release(m_pBacklight);
    }
}
static void bt_ui_proc_link_change(BD_ADDR bd_addr, BOOLEAN is_up)
{
    bt_ui_print_btaddr(bd_addr);
}
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
static uint16 bt_ui_dm_get_device_image(bt_cod_type *cod)
{
#ifndef AEE_SIMULATOR
    if(cod)
    {
        BCM_FUN_STAR("%x %x %x bt_ui_dm_get_device_image", cod->svc_cls_ptr,cod->mjr_dev_cls_ptr,cod->mnr_dev_cls_ptr);
        switch(cod->mjr_dev_cls_ptr)
        {
            case BT_SD_MAJOR_DEVICE_CLASS_COMPUTER:
                return IDI_BTPC;

            case BT_SD_MAJOR_DEVICE_CLASS_PHONE:
                return IDI_BTPHONE;

            case BT_SD_MAJOR_DEVICE_CLASS_AUDIO:
                return IDI_BTHEADSET;

            default:
                return IDI_BTOTHER;
        }
    }
#endif
    return 0;
}
#endif//FEATURE_BLUETOOTH_DEVICE_IMAGE
#ifndef AEE_SIMULATOR
void bt_ui_restore_set(void)
{
    OEMFS_Remove( BT_DEV_DB_FILE); 
    if(g_pBtuiApp)//bt app is start
    {
        if(bt_ui_device_db.bcm_enabled)
        {
            bt_ui_act_disable_bt();
        }
        bt_ui_init_device_db();
    }
}
#endif


/****************************************************************************
**
**  Name:          bt_ui_ag.c
**
**  Description:   Contains ui (handset gui menu) code for audio gateway
**
**
**  Copyright (c) 2002-2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
******************************************************************************/

#include "bt_target.h"
#include "gki.h"

#include "bt_ui_ht_common.h"

#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
#include "bta_api.h"
#include "bta_ag_api.h"
#include "bt_ui.h"
#include "bcmapp_dm.h"
#include "bcmapp_ag.h"
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
#include "bcmapp_fm.h"
#endif
#ifndef AEE_SIMULATOR
#include "AEECM.h"
#include "CallApp.h"

#ifdef FEATURE_CALL_HISTORY_APP
#include "AEECallHistory.h"
#endif//FEATURE_BLUETOOTH_APP

#ifdef FEATURE_CALL_LIST_APP
#include "AEECallList.h"
#endif//FEATURE_CS09_BLUETOOTH_APP

#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif
#include "AEE_OEMEvent.h"
#endif
//#include <stdio.h>
//#include <string.h>
#define BT_AG_CONNECT_TIMEOUT 15000
//////////////////////////////////////////////////////////////////////////
tHT_MENU_ITEM g_ag_menu_item;

/* BTUI AG main control block */
static tBTUI_AG_CB *bt_ui_ag_cb = NULL;

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
#ifdef BT_BROADCOM_SAMPLE_CODE
//static void bt_ui_ag_event_hdlr(tBTUI_MMI_MSG *p_msg);
static void bt_ui_menu_view_audio_devices(void);
static void bt_ui_act_audio_devices(UINT32 handle, UINT32 key);
static void bt_ui_menu_select_audio_devices (void);
static void bt_ui_act_new_audio_device(tBTUI_MMI_MSG *p_msg);
static void bt_ui_act_find_audio_devices(void);
static void bt_ui_menu_select_audio_device_bond(void);
static void bt_ui_act_select_audio_device_trusted(void);
static void bt_ui_act_set_default_audio_device(void);
static void bt_ui_act_delete_stored_audio_device(void);
static void bt_ui_menu_select_new_audio_devices (void);
static void bt_ui_act_new_audio_device_save (void);
static void bt_ui_act_show_new_audio_device(void);
static void bt_ui_menu_active_ag_connection(UINT8 app_id);
static void bt_ui_act_active_ag_connection(UINT32 handle, UINT32 key);

static void bt_ui_show_stored_audio_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_ag_new_device_cback(UINT32 handle, UINT32 key);
#endif
static boolean bt_ui_ag_make_last_recent_call(void);
static boolean bt_ui_ag_make_call(AECHAR * number);
static void bt_ui_act_audio_device_connect_cb(void);
/*******************************************************************************
**
** Function         bt_ui_ag_init
**
** Description      Initialises BTUI AG and BTA AG
**
**
** Returns          void
*******************************************************************************/
void bt_ui_ag_init(void)
{
    tBTA_SERVICE_MASK    services;
    BCM_FUN_STAR("bt_ui_ag_init",0,0,0);
    bt_ui_ag_cb = bcmapp_ag_get_ag_cb();
    //bt_ui_ag_call_table = bcmapp_ag_get_ag_call_table();
    /* get the supported services */
    services = bcmapp_ag_enable();

    if (services)
    {
        //         /* handles AG related user inputs from MMI */
        //         bt_ui_cfg.p_ag_event_hdlr = &bt_ui_ag_event_hdlr;
        //
        //         /* main AG menu */
        //         bt_ui_cfg.p_ag_menu = &bt_ui_menu_audio_devices;
        int i = 0;
        for(i = 0;i < BTUI_NUM_REM_DEVICE; i++)
        {
            bt_ui_device_db.device[i].ag_config.ag_connect = FALSE;
            bt_ui_device_db.device[i].ag_config.ag_proceing= FALSE;
        }
        bcmapp_ag_platform_init();
    }
}

void bt_ui_ag_disable(void)
{
    bcmapp_ag_disable();
}

#ifdef BT_BROADCOM_SAMPLE_CODE
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
/*******************************************************************************
**
** Function         bt_ui_view_audio_devices
**
** Description      Audio device list menu specifically for FM.
**
**
** Returns          void
*******************************************************************************/
void bt_ui_view_audio_devices(void)
{
    tBTUI_BTA_MSG    *p_msg;
    UINT8    connected_handle = 0xff;

    if (bt_ui_ag_cb->call_state != 0)
    {
        HT_MessageBoxTimeout("There is Active Call!", BTUI_TIMEOUT_MSG_PERIOD);
        bt_ui_ag_cb->play_fm = FALSE;
        return;
    }

    BT_UI_APPL_TRACE_ERROR3("HANDLE 1 = %d, HANDLE 2 = %d current hdl = %d", bt_ui_ag_cb->app[BTUI_AG_ID_1].handle,
        bt_ui_ag_cb->app[BTUI_AG_ID_2].handle, bt_ui_ag_cb->call_handle);

        /* check to see if there is active connection without active call,
    if there is, connect to FM directly */
    if (bt_ui_ag_cb->app[BTUI_AG_ID_1].is_open)
        connected_handle = BTUI_AG_ID_1;
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
    if (bt_ui_ag_cb->app[BTUI_AG_ID_2].is_open)
        connected_handle = BTUI_AG_ID_2;
#endif
    /* there is active connection, open sco */
    if (connected_handle != 0xff)
    {
        if (bt_ui_ag_cb->app[connected_handle].bvra_active) /* check if BVRA is active */
        {
            HT_MessageBoxTimeout("Voice Recognition is Active!", BTUI_TIMEOUT_MSG_PERIOD);
            bt_ui_ag_cb->play_fm = FALSE;
            return;
        }
        else
        {
            bt_ui_ag_cb->fm_connect = FALSE;
            bt_ui_ag_cb->sco_owner = 2;

            if (bt_ui_ag_cb->app[connected_handle].is_audio_open == FALSE)
            {
                BT_UI_APPL_TRACE_ERROR0("Open SCO for an active AG connection");
                BTA_AgAudioOpen(bt_ui_ag_cb->app[connected_handle].handle);
            }
            //else if ((p_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
            else if ((p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_FM_BT_OPEN;
                p_msg->hdr.layer_specific = bt_ui_ag_cb->app[connected_handle].handle;
                //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_msg);
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
            return;
        }
    }

    bt_ui_ag_cb->fm_connect = TRUE;
    bt_ui_menu_view_audio_devices();

}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_audio_devices
**
** Description      Audio device menu
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_audio_devices (void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    UINT8 i = 0;
    BOOLEAN hs_id_1_connected = FALSE;
    BCM_FUN_STAR("bt_ui_menu_audio_devices %d %d",bt_ui_ag_cb->app[BTUI_AG_ID_1].is_open, 0,0);
    p_menu[i].p_item_name = "View Devices";
    p_menu[i].item_data = 0;
    p_menu[i].item_index = i;
    i++;
    /* if HS or HF connection is active add a menu for the
    active connection */
    if(bt_ui_ag_cb->app[BTUI_AG_ID_1].is_open)
    {
        hs_id_1_connected = TRUE;
        if (bt_ui_ag_cb->app[BTUI_AG_ID_1].service == BTA_HSP_SERVICE_ID)
        {
            p_menu[i].p_item_name = "Connected HS 1";
        }
        else
        {
            p_menu[i].p_item_name = "Connected HF 1";
        }
        p_menu[i].item_data = BTUI_AG_ID_1;
        p_menu[i].item_index = i;
        i++;
    }
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
    if(bt_ui_ag_cb->app[BTUI_AG_ID_2].is_open)
    {
        if(hs_id_1_connected)
        {
            if (bt_ui_ag_cb->app[BTUI_AG_ID_2].service == BTA_HSP_SERVICE_ID)
            {
                p_menu[i].p_item_name = "Connected HS 2";
            }
            else
            {
                p_menu[i].p_item_name = "Connected HF 2";
            }
        }
        else
        {
            if (bt_ui_ag_cb->app[BTUI_AG_ID_2].service == BTA_HSP_SERVICE_ID)
            {
                p_menu[i].p_item_name = "Connected HS 1";
            }
            else
            {
                p_menu[i].p_item_name = "Connected HF 1";
            }

        }
        p_menu[i].item_data = BTUI_AG_ID_2;
        p_menu[i].item_index = i;
        i++;
    }
#endif

    handle = bt_ui_create_menu(p_menu, i, bt_ui_act_audio_devices, L"AUDIO DEVICES" ,BTBAR_SELECT_BACK,FALSE,0);
}

/*******************************************************************************
**
** Function         bt_ui_act_audio_devices
**
** Description
**
**
** Returns          void
**
*******************************************************************************/
static void bt_ui_act_audio_devices(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM*      p_item = &g_ag_menu_item;
    BCM_FUN_STAR("bt_ui_menu_audio_devices %x %x",handle,key,0);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
    else if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, p_item))
        {
            if (p_item->item_index == 0)
            {
                bt_ui_menu_view_audio_devices();
            }
            else
            {
                bt_ui_menu_active_ag_connection((UINT8) p_item->item_data);
            }
        }
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_active_ag_connection
**
** Description      Menu options for a active headset connection
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_active_ag_connection(UINT8 app_id)
{

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_active_ag_connection %d",app_id,0,0);

    p_menu[0].p_item_name = "Disconnect ";
    p_menu[0].item_data = (UINT32)app_id;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "+ Spk Vol";
    p_menu[1].item_data = (UINT32)app_id;
    p_menu[1].item_index = 2;

    p_menu[2].p_item_name = "- Spk Vol";
    p_menu[2].item_data = (UINT32)app_id;
    p_menu[2].item_index = 3;

    p_menu[3].p_item_name = "+ Mic Vol";
    p_menu[3].item_data = (UINT32)app_id;
    p_menu[3].item_index = 4;

    p_menu[4].p_item_name = "- Mic Vol";
    p_menu[4].item_data = (UINT32)app_id;
    p_menu[4].item_index = 5;


    handle = bt_ui_create_menu(p_menu, 5, bt_ui_act_active_ag_connection, L"Active Conn" ,BTBAR_SELECT_BACK,FALSE,0);
}

/*******************************************************************************
**
** Function         bt_ui_act_active_ag_connection
**
** Description
**
**
** Returns          void
**
*******************************************************************************/
static void bt_ui_act_active_ag_connection(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM*      p_item = &g_ag_menu_item;
    UINT8               app_id;
    BCM_FUN_STAR("bt_ui_act_active_ag_connection %x %x",handle,key,0);
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        return;
    }

    if(key == HT_KEY_SELECT ||key == HT_KEY_SOFT1)
    {
        HT_GetMenuSelectedItem(handle, p_item);
        app_id = (UINT8) p_item->item_data;
        switch (p_item->item_index)
        {
                /* Disconnect */
            case 1:
                /* closes active connection */
                bcmapp_ag_close_conn(app_id);
                HT_MessageBoxTimeout(L"Disconnecting", BTUI_TIMEOUT_MSG_PERIOD);
                break;

                /* + Spk Vol */
            case 2:

                bcmapp_ag_change_spk_volume(app_id, TRUE);
                HT_MessageBoxTimeout(L"Spk Vol Increased", BTUI_TIMEOUT_MSG_PERIOD);
                break;

                /* - Spk Vol */
            case 3:

                bcmapp_ag_change_spk_volume(app_id, FALSE);
                HT_MessageBoxTimeout(L"Spk Vol Decreased", BTUI_TIMEOUT_MSG_PERIOD);
                break;

                /* + Mic Vol */
            case 4:
                bcmapp_ag_change_mic_volume(app_id, TRUE);
                HT_MessageBoxTimeout(L"Mic Vol Increased", BTUI_TIMEOUT_MSG_PERIOD);
                break;

                /* - Mic Vol */
            case 5:
                bcmapp_ag_change_mic_volume(app_id, FALSE);
                HT_MessageBoxTimeout(L"Mic Vol Decreased", BTUI_TIMEOUT_MSG_PERIOD);
                break;

            default:
                break;
        }
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_audio_devices
**
** Description      Headset devices menu options
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_audio_devices(void)
{
    /* diaplays all audio devices in data base */
    BCM_FUN_STAR("bt_ui_menu_view_audio_devices %x",bt_ui_cfg.supported_services ,0,0);
    //bt_ui_cb.num_audio_devices =
    bt_ui_display_devices(bt_ui_cfg.supported_services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK),
                                    bt_ui_show_stored_audio_device_cback, L"Find New");
}

/*******************************************************************************
**
** Function         bt_ui_show_stored_audio_device_cback
**
** Description
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_stored_audio_device_cback(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_show_stored_audio_device_cback %x %x",handle,key,0);
    switch(key)
    {
        case HT_KEY_SELECT:
        {
            tHT_MENU_ITEM*  p_item = &g_ag_menu_item;

            if(SUCCESS == HT_GetMenuSelectedItem(handle, p_item))
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[p_item->item_data];
#if (defined BTA_FM_INCLUDED && BTA_FM_INCLUDED == TRUE)
                if (bt_ui_ag_cb->play_fm)
                {
                    bt_ui_act_audio_device_connect();
                }
                else
#endif
                    bt_ui_menu_select_audio_devices();
            }
            break;
        }
        

        case HT_KEY_SOFT1:
            bt_ui_act_find_audio_devices();
            break;

        case HT_KEY_SOFT2:
            bt_ui_set_draw_hold_down(TRUE);
            // Close view audio devices menu.
            bt_ui_close_object(handle);
            // Close and rebuild audio devices menu.
            bt_ui_close_object(HT_GetCurrMenuHandle());
            bt_ui_menu_audio_devices();
            bt_ui_set_draw_hold_down(FALSE);
            break;
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_audio_devices
**
** Description      Displays menu options for a selected  headset device
**
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_select_audio_devices (void)
{

    char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_select_audio_devices",0,0,0);
    p_menu[0].p_item_name = "Bond";
    p_menu[0].item_data = (UINT32)bt_ui_menu_select_audio_device_bond;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Authorize ";
    p_menu[1].item_data = (UINT32)bt_ui_act_select_audio_device_trusted;
    p_menu[1].item_index = 2;

    p_menu[2].p_item_name = "Set Default ";
    p_menu[2].item_data = (UINT32)bt_ui_act_set_default_audio_device;
    p_menu[2].item_index = 3;

    p_menu[3].p_item_name = "Connect";
    p_menu[3].item_data = (UINT32)bt_ui_act_audio_device_connect;
    p_menu[3].item_index = 4;

    p_menu[4].p_item_name = "Delete ";
    p_menu[4].item_data = (UINT32)bt_ui_act_delete_stored_audio_device;
    p_menu[4].item_index = 5;

    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->short_name) > 0)
    {
        p_name =  bt_ui_cb.p_selected_rem_device->short_name;
    }
    else if(strlen(bt_ui_cb.p_selected_rem_device->name) > 0)
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }

    {
        int szLen;
        TCHAR *szWStr = bt_ui_get_comm_wbuffer(&szLen);
        STRTOWSTR(p_name, szWStr, szLen<<1);
        handle = bt_ui_create_menu(p_menu, 5, bt_ui_menu_cback, szWStr ,BTBAR_SELECT_BACK,FALSE,0);
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_find_audio_devices
**
** Description      Action function to search for headset devices
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_find_audio_devices(void)
{
    /* filter: Major service class is set on 21st bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x20,0x00, 0x00}, {0x20,0x00, 0x00}};
    BCM_FUN_STAR("bt_ui_act_find_audio_devices",0,0,0);
    bt_ui_cb.ui_state = UI_NEW_AUDIO_DEVICE;

    bt_ui_search(BTA_DM_GENERAL_INQUIRY,
        bt_ui_cfg.supported_services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK),
        bt_ui_menu_ag_new_device_cback, &dev_filt);
}

/*******************************************************************************
**
** Function         bt_ui_menu_ag_new_device_cback
**
** Description
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_ag_new_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM*      p_item = &g_ag_menu_item;
    BCM_FUN_STAR("bt_ui_menu_ag_new_device_cback %x %x",handle,key,0);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        if(handle == bt_ui_cb.search_menu_handle)
        {
            HT_GetMenuSelectedItem(handle, p_item);
            if(p_item)
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[p_item->item_data];
                bt_ui_menu_select_new_audio_devices();
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        // Close the search handle.
        bt_ui_close_object(handle);
        bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
        bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;

        // Rebuild the menu view audio devices menu.
        bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_menu_view_audio_devices();
    }
}


/*******************************************************************************
**
** Function         bt_ui_menu_select_audio_device_bond
**
** Description      Menu requesting passkey for bonding
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_audio_device_bond(void)
{
    BCM_FUN_STAR("bt_ui_menu_select_audio_device_bond",0,0,0);
    bcmapp_dm_bond(bt_ui_cb.p_selected_rem_device);
}

/*******************************************************************************
**
** Function         bt_ui_act_select_audio_device_trusted
**
** Description      Sets the device as a trusted device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_select_audio_device_trusted(void)
{

    tBTUI_REM_DEVICE * p_device_rec;
    BCM_FUN_STAR("bt_ui_act_select_audio_device_trusted",0,0,0);
    if((p_device_rec = bt_ui_get_device_record(bt_ui_cb.p_selected_rem_device->bd_addr))==NULL)
    {
        p_device_rec = bt_ui_cb.p_selected_rem_device;
    }

    bcmapp_ag_set_audio_device_authorized (p_device_rec);

    bt_ui_cb.ui_state = UI_SELECT_AUDIO_DEVICE;
    HT_MessageBoxTimeout(L"Device   Authorized", BTUI_TIMEOUT_MSG_PERIOD);
}


/*******************************************************************************
**
** Function         bt_ui_act_set_default_audio_device
**
** Description      Sets the device as the default headset
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_set_default_audio_device(void)
{
    BCM_FUN_STAR("bt_ui_act_set_default_audio_device",0,0,0);
    bcmapp_ag_set_device_default_headset(bt_ui_cb.p_selected_rem_device);
    HT_MessageBoxTimeout(L"Set as Default", BTUI_TIMEOUT_MSG_PERIOD);

}


/*******************************************************************************
**
** Function         bt_ui_act_delete_stored_audio_device
**
** Description      Deletes the device from device data base
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_delete_stored_audio_device(void)
{
    BCM_FUN_STAR("bt_ui_act_delete_stored_audio_device",0,0,0);
    if(TRUE == bcmapp_dm_delete_device(bt_ui_cb.p_selected_rem_device->bd_addr))
    {
        //bt_ui_delete_device(bt_ui_cb.p_selected_rem_device->bd_addr);
        //could not delete device direct,need call dm.
        bt_ui_set_draw_hold_down(TRUE);

        bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_menu_view_audio_devices();

        HT_MessageBoxTimeout(L"Device Deleted!", BTUI_TIMEOUT_MSG_PERIOD);

        bt_ui_set_draw_hold_down(FALSE);
    }
    else
    {
        HT_MessageBoxTimeout(L"Device Not Deleted: Active Connection", BTUI_TIMEOUT_MSG_PERIOD);
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_new_audio_devices
**
** Description      Displays options for a newly discovered headset device
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_new_audio_devices (void)
{
    char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_select_new_audio_devices",0,0,0);

    p_menu[0].p_item_name = "Bond";
    p_menu[0].item_data = (UINT32)bt_ui_menu_select_audio_device_bond;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Authorize ";
    p_menu[1].item_data = (UINT32)bt_ui_act_select_audio_device_trusted;
    p_menu[1].item_index = 2;

    p_menu[2].p_item_name = "Set Default ";
    p_menu[2].item_data = (UINT32)bt_ui_act_set_default_audio_device;
    p_menu[2].item_index = 3;

    p_menu[3].p_item_name = "Connect";
    p_menu[3].item_data = (UINT32)bt_ui_act_audio_device_connect;
    p_menu[3].item_index = 4;

    p_menu[4].p_item_name = "Save ";
    p_menu[4].item_data = (UINT32)bt_ui_act_new_audio_device_save;
    p_menu[4].item_index = 5;

    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }

    {
        int szLen;
        TCHAR *szWStr = bt_ui_get_comm_wbuffer(&szLen);
        STRTOWSTR(p_name, szWStr, szLen<<1);
        handle = bt_ui_create_menu(p_menu, 5, bt_ui_menu_cback, szWStr ,BTBAR_SELECT_BACK,FALSE,0);
	}
}

/*******************************************************************************
**
** Function         bt_ui_act_new_audio_device
**
** Description      Action function for  state UI_SELECT_NEW_AUDIO_DEVICE
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_new_audio_device(tBTUI_MMI_MSG *p_msg)
{
    /* exit */
    BCM_FUN_STAR("bt_ui_act_new_audio_device",0,0,0);
    if((p_msg->data[0] == MENU_ITEM_0))
    {
        bt_ui_menu_view_audio_devices();
    }
    else if ((p_msg->data[0] > MENU_ITEM_0)
        &&((p_msg->data[0] - MENU_ITEM_0) <= bt_ui_inq_db.rem_index))
    {
        /* select one headset from the newly discovered headset */
        bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[(p_msg->data[0] - MENU_ITEM_0)-1];
        bt_ui_cb.ui_state = UI_SELECT_NEW_AUDIO_DEVICE;
        bt_ui_menu_select_new_audio_devices();
    }
}
#endif
/*******************************************************************************
**
** Function         bt_ui_act_audio_device_connect
**
** Description      Connects to the audio gateway
**
**
** Returns          void
*******************************************************************************/
void bt_ui_act_audio_device_connect(void)
{
    BCM_FUN_STAR("bt_ui_act_audio_device_connect %d",bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing,0,0);
    if(bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing == TRUE)
    {
        /*prevent repeat press key*/
        return;
    }
    bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing = TRUE;
#if (defined BTA_FM_INCLUDED && BTA_FM_INCLUDED == TRUE)
    if(bcmapp_ag_connect_device(bt_ui_cb.p_selected_rem_device, bt_ui_ag_cb->play_fm))
#else
    if(bcmapp_ag_connect_device(bt_ui_cb.p_selected_rem_device, FALSE))
#endif
    {
        HT_MessageBoxTimeout_ex(IDS_CONNECT_PORC, -1);
        ISHELL_SetTimer(bt_ui_get_shell(), BT_AG_CONNECT_TIMEOUT, (PFNNOTIFY)bt_ui_act_audio_device_connect_cb, NULL);
    }
    else
    {
        uint8 i;// num_devices = bt_ui_cb.num_devices;
        TCHAR w_con_buf[64] = {0};
        TCHAR w_temp_buf[64] = {0};

        bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing = FALSE;
        for(i=0; i < BTUI_NUM_REM_DEVICE; i++)
        {
            if((TRUE == bt_ui_device_db.device[i].ag_config.ag_connect) && (TRUE == bt_ui_device_db.device[i].in_use))
            {
                break;
            }
        }
        if(i == BTUI_NUM_REM_DEVICE)
        {
            HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
            return;
        }
        UTF8TOWSTR((byte *) bt_ui_device_db.device[i].name, BTUI_DEV_NAME_LENGTH+1, w_temp_buf, sizeof(w_temp_buf));
        //STRTOWSTR(bt_ui_device_db.device[i].name, w_temp_buf, 64 << 1);
        WSPRINTF(w_con_buf, 64 << 1, bt_ui_get_brx_string(IDS_AG_CONNECT_FAIL), w_temp_buf);//IDS_DEVICE_CONNECTED

        HT_MessageBoxTimeout(w_con_buf, BTUI_TIMEOUT_MSG_PERIOD);//L"AG busy"

        //bt_ui_act_audio_device_connect_cb();
        //HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_FAIL), BTUI_TIMEOUT_MSG_PERIOD);
    }
}
static void bt_ui_act_audio_device_connect_cb(void)
{
    bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing = FALSE;
    HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
}
void bt_ui_act_audio_device_disconnect(void)
{
    if(bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing == TRUE)
    {
        /*prevent repeat press key*/
        return;
    }
    bt_ui_cb.p_selected_rem_device->ag_config.ag_proceing = TRUE;
    HT_MessageBoxTimeout_ex(IDS_DISCONNECT_PORC, -1);
    bcmapp_ag_close_conn(bt_ui_cb.p_selected_rem_device->ag_config.ag_id);
    ISHELL_SetTimer(bt_ui_get_shell(), BT_AG_CONNECT_TIMEOUT, (PFNNOTIFY)bt_ui_act_audio_device_connect_cb, NULL);
}

#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_act_new_audio_device_save
**
** Description      Saves the device to device database
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_new_audio_device_save (void)
{
    BCM_FUN_STAR("bt_ui_act_new_audio_device_save",0,0,0);
    if(bt_ui_store_device(bt_ui_cb.p_selected_rem_device))
        HT_MessageBoxTimeout(L"Device Saved", BTUI_TIMEOUT_MSG_PERIOD);
    else
        HT_MessageBoxTimeout(L"Database full", BTUI_TIMEOUT_MSG_PERIOD);

}
/*******************************************************************************
**
** Function         bt_ui_act_show_new_audio_device
**
** Description      Dispalys newly discovered audio devices
**
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_show_new_audio_device(void)
{
    BCM_FUN_STAR("bt_ui_act_show_new_audio_device",0,0,0);
    bt_ui_show_search_results();
}
#endif

static void bt_ui_ag_act_audio_open(tBTUI_BTA_MSG *p_msg)
{
    //TCHAR szWBuf[24];
    BCM_FUN_STAR("bt_ui_ag_act_audio_open",0,0,0);
    if(g_pBtuiApp->m_bBackground == FALSE)
    {
        //WSPRINTF(szWBuf, sizeof(szWBuf), L"Audio %d Open", p_msg->hdr.layer_specific);
        //HT_MessageBoxTimeout(szWBuf, BTUI_TIMEOUT_MSG_PERIOD);
    }
}

static void bt_ui_ag_act_audio_close(tBTUI_BTA_MSG *p_msg)
{
    //TCHAR szWBuf[24];
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
    UINT16  play_fm = p_msg->hdr.offset;
    tBTUI_BTA_MSG    *p_mmi_msg;
#endif
    BCM_FUN_STAR("bt_ui_ag_act_audio_close",0,0,0);
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
    BT_UI_APPL_TRACE_DEBUG3("play_fm = %d bt_ui_ag_cb->fm_disconnect = %d bt_ui_ag_cb->sco_owner = %d",
        play_fm, bt_ui_ag_cb->fm_disconnect, bt_ui_ag_cb->sco_owner );

    if (bt_ui_fm_cb.fm_pending && play_fm &&
        bt_ui_ag_cb->sco_owner == 2)
    {
        BTA_AgAudioOpen(bt_ui_ag_cb->call_handle);
    }
    else
#endif
    {
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
        /* connection drop when FM is playing */
        if (play_fm && !bt_ui_ag_cb->fm_disconnect && bt_ui_ag_cb->sco_owner == 2)
        {
            //if ((p_mmi_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
            if ((p_mmi_msg = bt_ui_get_msg()) != NULL)
            {
                p_mmi_msg->hdr.event = BTUI_MMI_FM_BT_DROP;
                p_mmi_msg->hdr.layer_specific = BTUI_FM_VIA_BT_HFP;
                //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_mmi_msg);
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_mmi_msg);
            }
        }
        else if (bt_ui_ag_cb->fm_disconnect && bt_ui_ag_cb->sco_owner == 2)
        {
            bt_ui_ag_cb->play_fm = FALSE;
            bt_ui_ag_cb->fm_disconnect = FALSE;
        }
        else
#endif
        if(g_pBtuiApp->m_bBackground == FALSE)
        {
            //WSPRINTF(szWBuf, sizeof(szWBuf), L"Audio %d Down", p_msg->hdr.layer_specific);
            //HT_MessageBoxTimeout(szWBuf, BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
}

static void bt_ui_ag_act_ckpd_evt(tBTUI_BTA_MSG *p_msg)/*HSP event*/
{
    /*1. incoming->answer*/
    /*2. connect ->end cal */
    /*3. idle->originate call*/
    /*4. outgoing->not process*/
    boolean ok = FALSE;
#ifndef AEE_SIMULATOR
    ICM   *m_pICM = NULL;
    ISHELL_CreateInstance(bt_ui_get_shell(), AEECLSID_CM, (void **) &m_pICM);
    if(m_pICM != NULL)
    {
        if((AEECM_IS_VOICECALL_CONNECTED(m_pICM) == TRUE) ||(AEE_Active() == AEECLSID_DIALER)) /*in call state send key to dialerapp.*/
        {
            ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, 0, 0);
            ok = TRUE;
        }
        else /*idle state,call the last number*/
        {
            ok = bt_ui_ag_make_last_recent_call();
        }
        ICM_Release(m_pICM);
        m_pICM = NULL;
    }
#endif
    if(ok)
    {
        bcmapp_ag_send_ok(bt_ui_ag_cb->call_handle);
    }
    else
    {
        bcmapp_ag_send_error(bt_ui_ag_cb->call_handle, BTA_AG_ERR_PHONE_FAILURE);
    }
}
/*3. idle->originate last call*/
static void bt_ui_ag_act_bldn_evt(tBTUI_BTA_MSG *p_msg)
{
    boolean ok = FALSE;
    ok = bt_ui_ag_make_last_recent_call();

    if(ok)
    {
        bcmapp_ag_send_ok(bt_ui_ag_cb->call_handle);
    }
    else
    {
        bcmapp_ag_send_error(bt_ui_ag_cb->call_handle, BTA_AG_ERR_PHONE_FAILURE);
    }
}


static void bt_ui_ag_act_at_dail(tBTUI_BTA_MSG *p_msg)
{
    char data[52];
    AECHAR num[53] = {0};
    if(p_msg->ag_val.ag_val.str[0]!='>')
    {     
        int i=0;
        STRNCPY(data,p_msg->ag_val.ag_val.str,50);
        data[50]='\0';
        for(i=0;i<50;i++)
        {
            if(data[i]==';')
            {
                data[i]='\0';
                break;
            }
        }
    }
    else
    {
        data[0]= ATOI(p_msg->ag_val.ag_val.str+1);/*?????*/
    }
    STRTOWSTR(data,num,sizeof(num));
    bt_ui_ag_make_call(num);
    //bta_send_msg_to_mmi (MSG_ID_MMI_BCHS_HFG_SPEAKER_GAIN_REQ, (void *) msg_p);		
}

static void bt_ui_ag_act_at_chld(tBTUI_BTA_MSG *p_msg)
{
#ifndef AEE_SIMULATOR
    tBTA_AG_VAL * ag_val = NULL;
    ICM        *m_pICM = NULL; 
    boolean in_call  =FALSE;
    AVKType key_evt = AVK_UNDEFINED;
    tBTA_AG_RES_DATA data;
    ag_val = &(p_msg->ag_val.ag_val);
    
    BCM_MSG_MED("bt_ui_ag_act_at_chld %d %d",ag_val->str[0],ag_val->idx,0);
    if(ISHELL_CreateInstance(bt_ui_get_shell(),
                                            AEECLSID_CM,
                                            (void **) &m_pICM) != SUCCESS)
    {
        bcmapp_ag_send_error(bt_ui_ag_cb->call_handle, BTA_AG_ERR_INVALID_INDEX);
        return;
    }
    in_call = AEECM_IS_VOICECALL_CONNECTED(m_pICM);
    /* Send OK or ERROR */
    if(((ag_val->str[0] >= '0') && (ag_val->str[0] <= '4'))
        && (!((!(bt_ui_cfg.ag_features & BTA_AG_FEAT_ECC)) && (ag_val->idx != 0))))
    {
        /* Validate the index */
        if((('1' == ag_val->str[0]) || ('2' == ag_val->str[0])) && ag_val->idx)
        {
            if(FALSE == in_call)
            {
                BCM_MSG_MED("Invalid index %d", ag_val->idx,0,0);
                bcmapp_ag_send_error(bt_ui_ag_cb->call_handle, BTA_AG_ERR_INVALID_INDEX);
                ICM_Release(m_pICM);
                m_pICM = NULL;
                return;
            }
        }
        bcmapp_ag_send_ok(bt_ui_ag_cb->call_handle);
    }
    else
    {
        bcmapp_ag_send_error(bt_ui_ag_cb->call_handle, BTA_AG_ERR_PHONE_FAILURE);
        ICM_Release(m_pICM);
        m_pICM = NULL;
        return;
    }

    /* chld=0 releases all held calls or sets busy for a waiting call. */
    if (ag_val->str[0] == '0')
    {
         /* if incoming call is waiting treat as call cancel, otherwise ignore */
        if (bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] == BTUI_AG_CALLSETUP_INCOMING)
        {
            /* Find the entry of the call in call setup and release it */
            key_evt = AVK_CLR; /*send AVK_CLR to cannel the waiting call*/
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] =BTUI_AG_CALLSETUP_NONE;
            bt_ui_ag_cb->ind[BTUI_AG_IND_CALL] =BTUI_AG_CALL_IND_ACTIVE;
            data.audio_handle = bt_ui_ag_cb->call_handle;
            BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_CANCEL_RES, &data);
        }
        else
        {
            /* release all held calls */
            key_evt = AVK_ENDCALL; 
        }
    }
    /* chld=1 releases all active calls (if any exist) and accepts the other (held or waiting) call */
    else if (ag_val->str[0] == '1')
    {
        /* If an index is specified release that call, otherwise release all active calls */
        if (ag_val->idx)
        {
            key_evt = AVK_ENDCALL; 
        }
        else
        {
            key_evt = AVK_ENDCALL; 
        }

        /* if incoming call is waiting answer the call, else treat as hangup */
        if (bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] == BTUI_AG_CALLSETUP_INCOMING)
        {
            /* Find the entry of the call in call setup and make it active */
            key_evt = AVK_SEND; 
        }
        /* If there are any held call, accept the held call and drop the current call. */
        else
        {
            if (bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] == '0')
            {
                /* There is no held call simply hangup the current call. */
                key_evt = AVK_ENDCALL; 
            }
            /* Look for the first held call and make it active */
            else
            {
                key_evt = AVK_SEND; 
            }
        }
        bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] = '0';
    }
    /* chld=2 places all active calls (if any exist) on hold and accepts the other (held or waiting) call */
    else if (ag_val->str[0] == '2')
    {
        key_evt = AVK_SEND; 
        /* if incoming call is waiting answer the call */
        if (bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] == BTUI_AG_CALLSETUP_INCOMING)
        {
            bt_ui_ag_cb->ind[BTUI_AG_IND_SETUP] = BTUI_AG_CALLSETUP_NONE;
        }
    }
    /* chld=3 adds a held call to the conversation; we don't keep track of held calls, so do nothing */
    else if (ag_val->str[0] == '3')
    {
        bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] = '0';
    }
    /* chld=4 connects the two calls and disconnects the subscriber from both calls; treat as hangup */
    else if (ag_val->str[0] == '4')
    {
        key_evt = AVK_ENDCALL; 
        bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] = '0';
    }
    data.ind.id = BTA_AG_IND_CALLHELD;
    data.ind.value = bt_ui_ag_cb->ind[BTUI_AG_IND_HELD] - '0';
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);
    if(key_evt != AVK_UNDEFINED)
    {
        ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, key_evt, 0);
    }
#endif
}
static void bt_ui_ag_act_open(tBTUI_BTA_MSG *p_msg)
{
    tBTUI_REM_DEVICE * device;
    device = bt_ui_get_device_record(p_msg->open.bd_addr);
    if(device)
    {
        if(BTA_AG_SUCCESS != p_msg->open.status)
        {
            ISHELL_CancelTimer(bt_ui_get_shell(), (PFNNOTIFY)bt_ui_act_audio_device_connect_cb, NULL);
            HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
            device->ag_config.ag_proceing = FALSE;
            device->ag_config.ag_connect =FALSE;
            return;
        }
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_HEADSET_ON);
#endif
        device->services |= bcmapp_dm_change_id_to_server(p_msg->open.service);
        bt_ui_store_device(device);
        BCM_FUN_STAR("bt_ui_ag_act_open %x,%x",device,device->services,0);
        device->ag_config.ag_connect =TRUE;
        device->ag_config.ag_proceing = FALSE;
        ISHELL_CancelTimer(bt_ui_get_shell(), (PFNNOTIFY)bt_ui_act_audio_device_connect_cb, NULL);
        HT_MessageBoxTimeout_ex(IDS_AG_CONNECT, BTUI_TIMEOUT_MSG_PERIOD);
    }
}
static void bt_ui_ag_act_close(tBTUI_BTA_MSG *p_msg)
{
    tBTUI_REM_DEVICE * device;

    bt_ui_print_btaddr(p_msg->close.bd_addr);
    device = bt_ui_get_device_record(p_msg->close.bd_addr);
    if(device)
    {
        BCM_FUN_STAR("bt_ui_ag_act_close %x",device,0,0);
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_HEADSET_OFF);
#endif
        device->ag_config.ag_connect =FALSE;
        device->ag_config.ag_proceing = FALSE;
        ISHELL_CancelTimer(bt_ui_get_shell(), (PFNNOTIFY)bt_ui_act_audio_device_connect_cb, NULL);
        HT_MessageBoxTimeout_ex(IDS_AG_DISCONNECT, BTUI_TIMEOUT_MSG_PERIOD);
    }
}

void bt_ui_bta_ag_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_ag_evt_hdlr %x",p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
        case BTUI_MMI_AG_AUDIO_OPEN:
            bt_ui_ag_act_audio_open (p_msg);
#ifndef AEE_SIMULATOR
            ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, AVK_BT_HEADSET_CONNECT, 0);
#endif
            break;

        case BTUI_MMI_AG_AUDIO_CLOSE:
            bt_ui_ag_act_audio_close (p_msg);
#ifndef AEE_SIMULATOR
            ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, AVK_BT_HEADSET_DISCONNECT, 0);
#endif
            break;

        case BTUI_MMI_AG_AT_BLDN_EVT:
            /*3. idle->originate last call*/
            bt_ui_ag_act_bldn_evt(p_msg);
            break;
#ifndef AEE_SIMULATOR
        case BTUI_MMI_AG_AT_CHUP_EVT:
            /*2. connect ->end cal */
            ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, AVK_ENDCALL, 0);
            break;

        case BTUI_MMI_AG_AT_A_EVT:
            /*1. incoming->answer*/
            ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_DIALER, EVT_ORIGINATE_CALL, AVK_SEND, 0);
            break;
#endif
        case BTUI_MMI_AG_AT_CKPD_EVT:
            /*1. incoming->answer*/
            /*2. connect ->end cal */
            /*3. idle->originate call*/
            /*4. outgoing->not process*/
            bt_ui_ag_act_ckpd_evt(p_msg);
            break;

        case BTUI_MMI_AG_AT_D_EVT:
            /*originate val.str*/
            bt_ui_ag_act_at_dail(p_msg);
            break;

        case BTUI_MMI_AG_AT_CHLD_EVT:
            bt_ui_ag_act_at_chld(p_msg);
            break;

        case BTUI_MMI_CONN_UP:
        //    break;

        //case BTUI_MMI_AG_CONNECT:
            bt_ui_ag_act_open(p_msg);
#ifdef PLAY_MUSIC_USING_BT_AG
            if(!IsMp3PlayerStatusNone())
            {
               ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_APP_MUSICPLAYER, AVK_BT_HEADSET_CONNECT, 0, 0);
            }
#endif
            break;

        case BTUI_MMI_CONN_DOWN:
            bt_ui_ag_act_close(p_msg);
#ifdef PLAY_MUSIC_USING_BT_AG
            if(!IsMp3PlayerStatusNone())
            {
               ISHELL_PostEvent(bt_ui_get_shell(), AEECLSID_APP_MUSICPLAYER, AVK_BT_HEADSET_DISCONNECT, 0, 0);
            }
#endif
            break;

        default:
            break;
    }

}

boolean bt_ui_process_cmss_notify(void  *pssInfo)
{
    return TRUE;
}
static boolean bt_ui_ag_make_last_recent_call(void)
{
#ifndef AEE_SIMULATOR
#ifdef FEATURE_CALL_HISTORY_APP
    ICallHistory  *pCallHistory = NULL;
    boolean ok = FALSE;
    if (AEE_SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(), AEECLSID_CALLHISTORY,(void **)&pCallHistory))
    {
        AECHAR *last_num = NULL;
        last_num = ICALLHISTORY_GetLastRecNumByType(pCallHistory,AEECALLHISTORY_CALL_TYPE_TO);
        if(NULL != last_num)/*get last number ok,*/
        {
            ok = bt_ui_ag_make_call(last_num);
        }
        FREEIF(last_num);
        ICALLHISTORY_Release(pCallHistory);
        pCallHistory = NULL;
    }
    return ok;
#endif//FEATURE_CALL_HISTORY_APP

#ifdef FEATURE_CALL_LIST_APP
    ICallList   *pCallList = NULL;
    boolean ok = FALSE;
    OEMCallHistoryEntry ent;
    if (AEE_SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(), AEECLSID_CALLLIST,(void **)&pCallList))
    {
        if(ICALLLIST_GetEntry ( pCallList,CALLHISTORY_OUTGOING_CATEGORY,0,&ent ) == TRUE)/*get last number ok,*/
        {
            ok = bt_ui_ag_make_call(ent.digits+2);
        }
        ICALLLIST_Release(pCallList);
        pCallList = NULL;
    }
    return ok;
#endif//FEATURE_CALL_LIST_APP
    
#else//AEE_SIMULATOR
	return TRUE;
#endif
}

static boolean bt_ui_ag_make_call(AECHAR * number)
{
#ifndef AEE_SIMULATOR
    ICallApp *pCallApp = NULL;
    if(SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(), AEECLSID_DIALER, (void**)&pCallApp))
    {
#if(defined BT_OEM_DEBUG)
        DBGPRINTF("bt_ui_ag_act_bldn_evt %S",number);
#endif
        if (SUCCESS == ICallApp_BTCallNumber(pCallApp,number))
        {
            bt_ui_enable_backlight();/*need to enable backlight in this time*/
            ICallApp_Release(pCallApp);
            pCallApp = NULL;
            return TRUE;
        }
    }
#endif
    return FALSE;
}

void bt_ui_ag_make_quicktest_call(void)
{
    AECHAR num[32];
    WSTRLCPY(num,L"10000",32);
    bt_ui_close_object(HT_GetCurrMenuHandle());/*close current hand.so resume */
    bt_ui_ag_make_call(num);
}
#endif


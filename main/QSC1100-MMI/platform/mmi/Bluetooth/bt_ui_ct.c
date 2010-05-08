/****************************************************************************
**                                                                           
**  Name:          bt_ui_ct.c                                           
**                 
**  Description:   Contains btui functions for cordless terminal
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
#include "bcmapp_dm.h"
#include "bcmapp_ct.h"
#include "AEESound.h"
#include "AppComFunc.h"
//extern void bt_ui_platform_ct_init(void);

//extern void bt_ui_platform_ct_event(tBTA_CT_EVT event, tBTA_CT *p_data);

//extern UINT32 bt_ui_create_menu(tHT_MENU_ITEM*, UINT32, tHT_KEY_PRESSED_CBACK*, UINT8*);
//extern UINT32 bt_ui_message_box(UINT8*, tHT_KEY_PRESSED_CBACK*);
//extern UINT32 bt_ui_edit_box(UINT32, tHT_KEY_PRESSED_CBACK *, UINT8 *);

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
//static void bt_ui_ct_event_hdlr(tBTUI_MMI_MSG *p_msg);
//static void bt_ui_menu_ct_devices (void);
static void bt_ui_menu_active_ct_connection (void);
static void bt_ui_menu_view_ct_devices(void);
static void bt_ui_act_find_ct_devices(void);
static void bt_ui_menu_select_ct_devices (void);
static void bt_ui_menu_select_new_ct_device (void);
static void bt_ui_act_save_ct_device(void);
//static void bt_ui_menu_view_new_ct_devices(void);
static void bt_ui_act_open_ct_conn(void);
static void bt_ui_menu_view_icom_devices(void);
static void bt_ui_act_find_icom_devices(void);
static void bt_ui_act_save_icom_device(void);
static void bt_ui_menu_select_new_icom_device (void);
static void bt_ui_menu_select_icom_devices (void);
static void bt_ui_act_open_icom_conn(void);
static void bt_ui_menu_view_icom_devices_by_name(void);
static void bt_ui_menu_view_icom_devices_by_extn(void);
static void bt_ui_ct_orginate_call(void);
static void bt_ui_ct_end_call(void);
static void bt_ui_ct_disconnect_connection(void);
static void bt_ui_ct_answer(void);
static void bt_ui_show_stored_ct_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_menu_ct_new_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_show_stored_icom_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_ct_connect_new_device(void);
static void bt_ui_ct_connect_new_icom_device(void);
static void bt_ui_menu_icom_new_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_act_view_icom_devices_by_extn_cback(UINT32 handle, UINT32 key);
#ifdef BT_BROADCOM_SAMPLE_CODE
static void bt_ui_menu_view_icom_devices_cback(UINT32 handle, UINT32 key);
#endif
static void bt_ui_menu_enter_number_cback(UINT32 handle, UINT32 key);
static boolean bt_ui_ct_sel_snd_device(AEESoundDevice device);
static void bt_ui_ct_connect_new_icom_device_cb(UINT32 handle, UINT32 key);
static void bt_ui_ct_show_device_detail(void);
static void bt_ui_ct_delete_device_pop(void);
#define BT_UI_CT_APPID   1
#define BT_UI_ICOM_APPID 2
static tBTUI_CT_CB *bt_ui_ct_cb = NULL;
static tBTUI_CT_WUG_DB *bt_ui_ct_wug_db = NULL;
static ISound       *g_pSound= NULL;
/*******************************************************************************
**
** Function         bt_ui_ct_init
**
** Description      Initialises Cordless terminal
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ct_init(void)
{
    bt_ui_ct_cb = bcmapp_ct_get_ct_db();
    bt_ui_ct_wug_db =  bcmapp_ct_get_wug_db();
    bt_ui_ct_cb->ct_menu_handle = BT_UI_NULL_HANDLE;
    bcmapp_ct_init();
    /* handles CT related user inputs from MMI */
    //bt_ui_cfg.p_ct_event_hdlr = &bt_ui_ct_event_hdlr;
    /* main AG menu */
    //bt_ui_cfg.p_ct_menu = &bt_ui_menu_ct_devices;
    //bt_ui_platform_ct_init();
}

/*******************************************************************************
**
** Function         bt_ui_ct_enable
**
** Description      Enable Cordless terminal
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ct_enable(void)
{
    bcmapp_ct_enable();
}

void bt_ui_ct_disable(void)
{
    if(g_pSound)
    {
        ISound_Release(g_pSound);
    }
    g_pSound = NULL;
    bcmapp_ct_disable();
}

/*******************************************************************************
**
** Function         bt_ui_ct_event_hdlr
**
** Description      Process user inputs to menu options
**                  related to bluetooth cordless services
**                  
**
** Returns          void
*******************************************************************************/
//static void bt_ui_ct_event_hdlr(tBTUI_MMI_MSG *p_msg)
//{
//    switch(bt_ui_cb.ui_state)
//    {
//     default:
//         break;
//    }
//}

/*******************************************************************************
**
** Function         bt_ui_menu_ct_devices
**
** Description      Cordless menu
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_ct_devices (void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    UINT8 i =2;
    BCM_FUN_STAR("bt_ui_menu_ct_devices %x %d %d", bt_ui_ct_cb->ui_current_active_connection,bt_ui_ct_cb->ic_call_active,bt_ui_ct_cb->gw_connected);
    p_menu[0].p_item_name = "Gateway";
    p_menu[0].item_data = (UINT32)bt_ui_menu_view_ct_devices;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Terminals";
    p_menu[1].item_data = (UINT32)bt_ui_menu_view_icom_devices;
    p_menu[1].item_index = 2;

    if(((bt_ui_ct_cb->ui_current_active_connection 
        & (BTA_CTP_SERVICE_MASK | BTA_ICP_SERVICE_MASK))&& bt_ui_ct_cb->ic_call_active) ||bt_ui_ct_cb->gw_connected)
    {
        //if (bt_ui_ct_cb->gw_connected || bt_ui_ct_cb->ic_call_active) 
        {
            p_menu[2].p_item_name = "Active Call";
            p_menu[2].item_data = (UINT32)bt_ui_menu_active_ct_connection;
            p_menu[2].item_index = 3;
            i = 3;
        }
    }

    /*handle = */bt_ui_create_menu(p_menu, i,bt_ui_menu_cback, L"Cordless",BTBAR_SELECT_BACK,FALSE,0);
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         bt_ui_menu_active_call_cback
**
** Description      Ui callback
**                  
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_active_call_cback(UINT32 handle, UINT32 key)
{

    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_active_call_cback", 0,0,0);
    bt_ui_cb.current_menu_handle = handle;

    if(key == HT_KEY_SELECT||key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
          
            if(p_item.item_data)
                ((tBTUI_ACT_NO_PARAM)p_item.item_data)();
        }

    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);

    }

    if((key>= AVK_0) && (key <= AVK_9) 
        && (bt_ui_ct_cb->ic_call_active || bt_ui_ct_cb->gw_call_active))
    {

        BTA_CtDtmf(bt_ui_ct_cb->active_handle, (UINT8)(30 + (UINT8)key), TRUE);
        BTA_CtDtmf(bt_ui_ct_cb->active_handle, (UINT8)(30 + (UINT8)key), FALSE);
    }
}


/*******************************************************************************
**
** Function         bt_ui_menu_active_ct_connection
**
** Description      Cordless menu
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_active_ct_connection (void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT8  menu_index = 0;
    BCM_FUN_STAR("bt_ui_menu_active_ct_connection %d %d %d", bt_ui_ct_cb->gw_connected,bt_ui_ct_cb->gw_call_active,bt_ui_ct_cb->gw_connected);

    if(bt_ui_ct_cb->gw_connected && !bt_ui_ct_cb->gw_call_active)
    {
        p_menu[menu_index].p_item_name = "Call";
        p_menu[menu_index].item_data = (UINT32)bt_ui_ct_orginate_call;
        p_menu[menu_index].item_index = menu_index + 1;
        menu_index ++;
    }

    if(bt_ui_ct_cb->ic_call_active || bt_ui_ct_cb->gw_call_active)
    {
        p_menu[menu_index].p_item_name = "End Call";
        p_menu[menu_index].item_data = (UINT32)bt_ui_ct_end_call;
        p_menu[menu_index].item_index = menu_index + 1;
        menu_index++;
    }

    if(bt_ui_ct_cb->gw_connected)
    {
        p_menu[menu_index].p_item_name = "Disconnect";
        p_menu[menu_index].item_data = (UINT32)bt_ui_ct_disconnect_connection;
        p_menu[menu_index].item_index = menu_index + 1;
        menu_index++;
    }

    if(bt_ui_ct_cb->incoming_call)
    {
        p_menu[menu_index].p_item_name = "Answer Call";
        p_menu[menu_index].item_data = (UINT32)bt_ui_ct_answer;
        p_menu[menu_index].item_index = menu_index + 1;
        menu_index++;
    }

    bt_ui_create_menu(p_menu, menu_index ,bt_ui_menu_active_call_cback, L"Call Menu",BTBAR_SELECT_BACK,TRUE,0);//used option menu
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         bt_ui_ct_orginate_call
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_orginate_call(void)
{
    UINT32 handle;
    handle = bt_ui_edit_box(HT_EB_NUMBERS,    bt_ui_menu_enter_number_cback, L"Enter Number",32,BTBAR_OK_BACK,NULL,FALSE);
    //HT_SetSoftKeyMessages(handle, "Call", "Cancel");
}


/*******************************************************************************
**
** Function         bt_ui_menu_enter_number_cback
**
** Description      Makes call to number entered by user
**                  
** Returns          void
*******************************************************************************/
static void bt_ui_menu_enter_number_cback(UINT32 handle, UINT32 key)
{
    int szLen;
    TCHAR *szWText = bt_ui_get_comm_wbuffer(&szLen);
    char * p_text = bt_ui_get_comm_buffer(&szLen);
    BCM_FUN_STAR("bt_ui_menu_enter_number_cback", 0,0,0);
    if(key == HT_KEY_SOFT1)
    {
        szWText = HT_GetEditBoxText(handle);
        WSTRTOSTR(szWText, p_text, szLen);
        bt_ui_ct_sel_snd_device(AEE_SOUND_DEVICE_BT_CTP_HANDDSET);
        bcmapp_ct_orginate_call((UINT8*)p_text, (UINT8)strlen((const char*)p_text));
        bt_ui_close_object(handle);
        bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_menu_active_ct_connection();
    }
    else if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}
/*******************************************************************************
**
** Function         bt_ui_ct_end_call
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_end_call(void)
{
    bcmapp_ct_end_call();
}
/*******************************************************************************
**
** Function         bt_ui_ct_disconnect_connection
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_disconnect_connection(void)
{
    bcmapp_ct_disconnect_connection();
    bt_ui_close_object(HT_GetCurrMenuHandle());
    bt_ui_menu_active_ct_connection();
}

/*******************************************************************************
**
** Function         bt_ui_ct_answer
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_answer(void)
{
    bcmapp_ct_answer();
    bt_ui_close_object(HT_GetCurrMenuHandle());
    bt_ui_menu_active_ct_connection();
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_ct_devices
**
** Description      Object push menu
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_ct_devices(void)
{
    /*gw*/
    /*bt_ui_cb.num_ct_devices = */bt_ui_display_devices(BTA_CTP_SERVICE_MASK, bt_ui_show_stored_ct_device_cback, L"Find New");
    //bt_ui_cb.show_bonded_service = BTA_CTP_SERVICE_MASK;
    //bt_ui_act_show_bonded_device();
}
/*******************************************************************************
**
** Function         bt_ui_show_stored_ct_device_cback
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_stored_ct_device_cback(UINT32 handle, UINT32 key)
{
    /*gw*/
    tHT_MENU_ITEM menu_item;
    BCM_FUN_STAR("bt_ui_show_stored_ct_device_cback", 0,0,0);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &menu_item))
        {
            if(menu_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
                bt_ui_act_find_ct_devices();
            }
            else
            {
                bt_ui_set_menu_sel(menu_item.item_index);
                bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[menu_item.item_data];
                bt_ui_menu_select_ct_devices(); 
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    }
    //if(key == HT_KEY_SOFT1)
    //{
    //    bt_ui_close_object(handle);
    //    bt_ui_close_object(HT_GetCurrMenuHandle());
    //    bt_ui_menu_ct_devices();
    //}
}



/*******************************************************************************
**
** Function         bt_ui_act_find_ct_devices
**
** Description      Action function to search for ct devices
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_find_ct_devices(void)
{
    /*gw*/
    /* filter: Major service class is set on 22nd bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x40,0x00, 0x00}, {0x40,0x00, 0x00}};
    bt_ui_search(BTA_DM_GENERAL_INQUIRY, BTA_CTP_SERVICE_MASK, bt_ui_menu_ct_new_device_cback, &dev_filt );
}

/*******************************************************************************
**
** Function         bt_ui_menu_ct_new_device_cback
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_ct_new_device_cback(UINT32 handle, UINT32 key)
{
    /*gw*/
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_ct_new_device_cback", 0,0,0);
    if(key == HT_KEY_SELECT||key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
            if(p_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
                bt_ui_act_find_ct_devices();
            }
            else
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[p_item.item_data];            
                bt_ui_menu_select_new_ct_device(); 
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        //bt_ui_menu_ct_devices();
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_icom_devices
**
** Description      displays icom devices
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_icom_devices(void)
{
    /*client*/
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    BCM_FUN_STAR("bt_ui_menu_view_icom_devices", 0,0,0);
    p_menu[0].p_item_name = "By Name";
    p_menu[0].item_data = (UINT32)bt_ui_menu_view_icom_devices_by_name;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "By EXTN";
    p_menu[1].item_data = (UINT32)bt_ui_menu_view_icom_devices_by_extn;
    p_menu[1].item_index = 2;

    bt_ui_create_menu(p_menu, 2,bt_ui_menu_cback/*bt_ui_menu_view_icom_devices_cback*/, L"View Terminals",BTBAR_SELECT_BACK,FALSE,0);
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_menu_view_icom_devices_cback
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_icom_devices_cback(UINT32 handle, UINT32 key)
{

    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_view_icom_devices_cback", 0,0,0);
    bt_ui_cb.current_menu_handle = handle;

    if(key == HT_KEY_SELECT||key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
            if(p_item.item_data)
            {
                ((tBTUI_ACT_NO_PARAM)p_item.item_data)();
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}
#endif
/*******************************************************************************
**
** Function         bt_ui_menu_view_icom_devices_by_name
**
** Description      displays intercom devices by name
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_icom_devices_by_name(void)
{
    /*client*/
    /*bt_ui_cb.num_icom_devices =*/ bt_ui_display_devices(BTA_ICP_SERVICE_MASK, bt_ui_show_stored_icom_device_cback, L"Find New");
}
/*******************************************************************************
**
** Function         bt_ui_show_stored_icom_device_cback
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_show_stored_icom_device_cback(UINT32 handle, UINT32 key)
{
    /*client*/
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_show_stored_icom_device_cback", 0,0,0);
    if(key == HT_KEY_SELECT|| key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
            if(p_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
                bt_ui_act_find_icom_devices();
            }
            else
            {
                bt_ui_set_menu_sel(p_item.item_index);
                bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[p_item.item_data];
                bt_ui_menu_select_icom_devices(); 
            }
            //bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[p_item.item_data];            
            //bt_ui_menu_select_icom_devices();  
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_icom_devices_by_extn
**
** Description      displays intercom devices by extn
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_icom_devices_by_extn(void)
{
    char                msg_str[7][64];
    UINT8               i;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    BottomBar_e_Type BottomBar = BTBAR_SELECT_BACK;
    BCM_FUN_STAR("bt_ui_menu_view_icom_devices_by_extn", 0,0,0);

    for(i=0; i<bt_ui_ct_wug_db->num_members;i++)
    {
        sprintf (msg_str[i], "Call EXTN %d %d",bt_ui_ct_wug_db->wug_db[i].extn.digit1,
                       bt_ui_ct_wug_db->wug_db[i].extn.digit2);
        p_menu[i].p_item_name = msg_str[i];
        p_menu[i].item_data = i;
    }

    if(i==0)
    {
        p_menu[i].item_data = (UINT32)IDS_NO_RECORD;
        p_menu[i].p_item_name = "No EXTNs ";
        p_menu[i].item_index = IDS_NO_RECORD;
        //p_menu[i].item_string_id= IDS_NO_RECORD;
        i++;
        BottomBar = BTBAR_BACK;
        //handle = bt_ui_message_box(L"No EXTNs ",bt_ui_act_view_icom_devices_by_extn_cback,BTBAR_SELECT_BACK);
    }
    //else
    {
         bt_ui_create_menu(p_menu, i ,bt_ui_act_view_icom_devices_by_extn_cback, L"EXTN Numbers",BottomBar,FALSE,0);
    }

    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         bt_ui_act_view_icom_devices_by_extn_cback
**
** Description      calls the selected extension
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_view_icom_devices_by_extn_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_act_view_icom_devices_by_extn_cback", 0,0,0);
    if(key == HT_KEY_SELECT||key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
            if(p_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
            }
            else
            {
                bcmapp_ct_call_icom_by_extn((UINT8)p_item.item_data);
                HT_MessageBoxTimeout(L"Call Orginated", BTUI_TIMEOUT_MSG_PERIOD);
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
}



/*******************************************************************************
**
** Function         bt_ui_act_find_icom_devices
**
** Description      Action function to search for icom devices
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_find_icom_devices(void)
{
    /*client*/
    /* filter: Major service class is set on 22th bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x40,0x00, 0x00}, {0x40,0x00, 0x00}};
    bt_ui_search(BTA_DM_GENERAL_INQUIRY, BTA_ICP_SERVICE_MASK, bt_ui_menu_icom_new_device_cback, &dev_filt );
}
/*******************************************************************************
**
** Function         bt_ui_menu_icom_new_device_cback
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_icom_new_device_cback(UINT32 handle, UINT32 key)
{
    /*client*/
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_icom_new_device_cback", 0,0,0);
    if(key == HT_KEY_SELECT || key == HT_KEY_SOFT1)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle, &p_item))
        {
            if(p_item.item_data == IDS_NO_RECORD)
            {
                bt_ui_close_object(handle);
                bt_ui_act_find_icom_devices();
            }
            else
            {
                bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[p_item.item_data];            
                bt_ui_menu_select_new_icom_device();   
            }
        }
    }

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        //bt_ui_close_object(HT_GetCurrMenuHandle());
        //bt_ui_menu_view_icom_devices_by_name();
    }
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_ct_devices
**
** Description      Displays menu options for a selected  cordless device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_ct_devices (void)
{
    /*gw*/
    //char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //UINT32 handle;
    //TCHAR *szWText;
    //int szLen;
    BCM_FUN_STAR("bt_ui_menu_select_ct_devices", 0,0,0);
    p_menu[0].item_string_id= IDS_AG_CONNECT;
    p_menu[0].item_data = (UINT32)bt_ui_act_open_ct_conn;
    p_menu[0].item_index = 1;

    p_menu[1].item_string_id = IDS_STRING_DELETE;
    p_menu[1].item_data = (UINT32)bt_ui_ct_delete_device_pop;
    p_menu[1].item_index = 2;
    
    p_menu[2].item_string_id = IDS_DEVICE_INFO;
    p_menu[2].item_data = (UINT32)bt_ui_ct_show_device_detail;
    p_menu[2].item_index = 3;

    p_menu[3].item_string_id = IDS_SEL_SERACH;
    p_menu[3].item_data = (UINT32)bt_ui_act_find_ct_devices;
    p_menu[3].item_index = 4;

#if 0
    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->short_name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->short_name;
    }
    else if(strlen(bt_ui_cb.p_selected_rem_device->name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }
    szWText = bt_ui_get_comm_wbuffer(&szLen);
    UTF8TOWSTR((const byte *)p_name,  BTUI_DEV_NAME_LENGTH + 1, szWText, szLen);
#endif
    bt_ui_create_menu(p_menu, 4,bt_ui_menu_cback, NULL,BTBAR_SELECT_BACK,TRUE,0);
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}
//static void bt_ui_ct_delete_device(void)
//{
//    
//    bt_ui_delete_device(bt_ui_cb.p_selected_rem_device->bd_addr);
//}
static void bt_ui_ct_delete_device_cb(UINT32 handle, UINT32 key)
{
    /*client*/
    BOOLEAN result = FALSE;
    BCM_FUN_STAR("bt_ui_act_delete_device %x %x %x", bt_ui_cb.p_selected_rem_device,bt_ui_cb.is_show_bonded_devices,bt_ui_cb.select_bonded_handle);
    bt_ui_close_object(handle);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        if(bt_ui_cb.p_selected_rem_device)
        {
            result = bt_ui_delete_device(bt_ui_cb.p_selected_rem_device->bd_addr);
        }
        
        if(bt_ui_cb.select_bonded_handle != BT_UI_NULL_HANDLE)
        {
            bt_ui_close_object(bt_ui_cb.select_bonded_handle);//show bond need redraw
            bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
            bt_ui_menu_view_icom_devices_by_name();
        }

        if (result)
        {
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
}

static void bt_ui_ct_delete_device_pop(void)
{
    /*client*/
    bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
    bt_ui_message_box_ex(IDS_STRING_DELETE, bt_ui_ct_delete_device_cb,BTBAR_OK_CANCEL);
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_icom_devices
**
** Description      Displays menu options for a selected  intercom device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_icom_devices (void)
{
    /*client*/
    //char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //TCHAR *szWText;
    //int szLen;
    BCM_FUN_STAR("bt_ui_menu_select_icom_devices", 0,0,0);
    p_menu[0].item_string_id= IDS_AG_CONNECT;
    p_menu[0].item_data = (UINT32)bt_ui_act_open_icom_conn;
    p_menu[0].item_index = 1;

    p_menu[1].item_string_id = IDS_STRING_DELETE;
    p_menu[1].item_data = (UINT32)bt_ui_ct_delete_device_pop;
    p_menu[1].item_index = 2;
    
    p_menu[2].item_string_id = IDS_DEVICE_INFO;
    p_menu[2].item_data = (UINT32)bt_ui_ct_show_device_detail;
    p_menu[2].item_index = 3;

    p_menu[3].item_string_id = IDS_SEL_SERACH;
    p_menu[3].item_data = (UINT32)bt_ui_act_find_icom_devices;
    p_menu[3].item_index = 4;


#if 0
    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->short_name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->short_name;
    }
    else if(strlen(bt_ui_cb.p_selected_rem_device->name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }
    szWText = bt_ui_get_comm_wbuffer(&szLen);
    UTF8TOWSTR((const byte *)p_name,  BTUI_DEV_NAME_LENGTH + 1, szWText, szLen);
#endif
    bt_ui_create_menu(p_menu, 4 ,bt_ui_menu_cback, NULL,BTBAR_SELECT_BACK,TRUE,0);
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         bt_ui_act_open_ct_conn
**
** Description      Action function for UI_SELECT_CORDLESS_DEVICE state
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_open_ct_conn(void)
{
    /*gw*/
    bcmapp_ct_open_ct_conn();
    HT_MessageBoxTimeout_ex(IDS_CONNECT_PORC, BTUI_TIMEOUT_MSG_PERIOD);
}

/*******************************************************************************
**
** Function         bt_ui_act_open_icom_conn
**
** Description      Action function for UI_SELECT_CORDLESS_DEVICE state
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_open_icom_conn(void)
{
    /*client*/
    bt_ui_ct_sel_snd_device(AEE_SOUND_DEVICE_BT_CTP_HANDDSET);
    bcmapp_ct_open_icom_conn();
    //HT_MessageBoxTimeout(L"Connecting", BTUI_TIMEOUT_MSG_PERIOD);
    bt_ui_ct_cb->ct_menu_handle = bt_ui_message_box(L"Call Orginated",bt_ui_ct_connect_new_icom_device_cb,BTBAR_END);
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_new_ct_devices
**
** Description      New ct devices menu
**                  
**
** Returns          void
*******************************************************************************/
//static void bt_ui_menu_view_new_ct_devices(void)
//{
//    bt_ui_show_new_devices(bt_ui_menu_ct_new_device_cback);
//}




/*******************************************************************************
**
** Function         bt_ui_menu_select_new_ct_device
**
** Description      Displays menu options for a selected  ct device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_new_ct_device (void)
{
    /*gw*/
    //char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //TCHAR *szWText;
    //int szLen;

    BCM_FUN_STAR("bt_ui_menu_select_new_ct_device", 0,0,0);
    p_menu[0].item_string_id = IDS_AG_CONNECT;
    p_menu[0].item_data = (UINT32)bt_ui_ct_connect_new_device;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Save device ";
    p_menu[1].item_data = (UINT32)bt_ui_act_save_ct_device;
    p_menu[1].item_index = 2;
#if 0
    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }
    szWText = bt_ui_get_comm_wbuffer(&szLen);
    UTF8TOWSTR((const byte *)p_name,  BTUI_DEV_NAME_LENGTH + 1, szWText, szLen);
#endif
    bt_ui_create_menu(p_menu, 2, bt_ui_menu_cback, NULL,BTBAR_SELECT_BACK,TRUE,0);//option

    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_new_icom_device
**
** Description      Displays menu options for a selected  icom device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_new_icom_device (void)
{
    /*client*/
    //char * p_name;
    byte i = 0;
    tBTUI_REM_DEVICE *device = NULL;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    //TCHAR *szWText;
    //int szLen;
    BCM_FUN_STAR("bt_ui_menu_select_new_icom_device", 0,0,0);
    p_menu[i].item_string_id = IDS_AG_CONNECT;
    p_menu[i].item_data = (UINT32)bt_ui_ct_connect_new_icom_device;
    p_menu[i].item_index = i;
    i ++;

    p_menu[i].item_string_id = IDS_DEVICE_INFO;
    p_menu[i].item_data = (UINT32)bt_ui_ct_show_device_detail;
    p_menu[i].item_index = i;
    i ++;
    
    device = bt_ui_get_device_record(bt_ui_cb.p_selected_rem_device->bd_addr);
    if(NULL == device)
    {
        p_menu[i].p_item_name = "Save device ";
        p_menu[i].item_data = (UINT32)bt_ui_act_save_icom_device;
        p_menu[i].item_index = i;
        i ++;
    }

#if 0
    /* Display the name of the device */
    if(strlen(bt_ui_cb.p_selected_rem_device->name))
    {
        p_name =  bt_ui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }
    szWText = bt_ui_get_comm_wbuffer(&szLen);
    UTF8TOWSTR((const byte *)p_name,  BTUI_DEV_NAME_LENGTH + 1, szWText, szLen);
#endif
    bt_ui_create_menu(p_menu, i, bt_ui_menu_cback, NULL,BTBAR_SELECT_BACK,TRUE,0);//option
    //HT_SetSoftKeyMessages(handle, "Back", NULL);
}
/*******************************************************************************
**
** Function         bt_ui_ct_connect_new_device
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_connect_new_device(void)
{
    /*gw*/
    bcmapp_ct_open_ct_conn();
}


static void bt_ui_ct_connect_new_icom_device_cb(UINT32 handle, UINT32 key)
{   /*client*/
    bt_ui_close_object(handle);
    bt_ui_ct_cb->ct_menu_handle = BT_UI_NULL_HANDLE;
    BCM_FUN_STAR("bt_ui_ct_connect_new_icom_device_cb %x %x ", handle,key,0);
    if(key == HT_KEY_SOFT2 )
    {
        bcmapp_ct_end_call();
    }
}

/*******************************************************************************
**
** Function         bt_ui_ct_connect_new_icom_device
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_ct_connect_new_icom_device(void)
{
    /*client*/
    tBTA_CT_IE_CALLED_NUM called_num;
    BCM_FUN_STAR("bt_ui_ct_connect_new_icom_device", 0,0,0);
    called_num.num_type = BTA_CT_CALL_NUM_UNKNOWN;
    called_num.num_plan = BTA_CT_CALL_NP_UNKNOWN;
    called_num.num_len = 6;
    memcpy(called_num.num, bt_ui_cb.p_selected_rem_device->bd_addr, called_num.num_len);
    bt_ui_ct_sel_snd_device(AEE_SOUND_DEVICE_BT_CTP_HANDDSET);
    BTA_CtOriginate(2, BTA_CT_CALL_CLASS_INTERCOM,  &called_num, NULL, 
                             NULL);
    bt_ui_ct_cb->ct_menu_handle = bt_ui_message_box(L"Call Orginated",bt_ui_ct_connect_new_icom_device_cb,BTBAR_END);
    //HT_MessageBoxTimeout(L"Call Orginated", BTUI_TIMEOUT_MSG_PERIOD);
}



/*******************************************************************************
**
** Function         bt_ui_act_save_ct_device
**
** Description      Action function to save ct device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_save_ct_device(void)
{
    /*gw*/
    BCM_FUN_STAR("bt_ui_act_save_ct_device", 0,0,0);
    if(bt_ui_store_device(bt_ui_cb.p_selected_rem_device))
    {
        HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
    }
}

/*******************************************************************************
**
** Function         bt_ui_act_save_icom_device
**
** Description      Action function to save icom device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_save_icom_device(void)
{
    /*client*/
    tBTUI_REM_DEVICE *device = NULL;
    BCM_FUN_STAR("bt_ui_act_save_icom_device", 0,0,0);
    device = bt_ui_get_device_record(bt_ui_cb.p_selected_rem_device->bd_addr);
    if(NULL == device)
    {
        device = bt_ui_alloc_device_record(bt_ui_cb.p_selected_rem_device->bd_addr);
        memcpy(device, bt_ui_cb.p_selected_rem_device, sizeof(tBTUI_REM_DEVICE));
        device->in_use =TRUE;
        if(bt_ui_store_device(device))
        {
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        }
    }
}

static void bt_ui_ct_act_incoming_call_cb(UINT32 handle, UINT32 key)
{
    bt_ui_close_object(handle);
    bt_ui_ct_cb->ct_menu_handle = BT_UI_NULL_HANDLE;
    BCM_FUN_STAR("bt_ui_ct_act_incoming_call_cb %x %x ", handle,key,0);
    if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        bcmapp_ct_answer();
    }
    else if(key == HT_KEY_SOFT2 )
    {
        bcmapp_ct_end_call();
    }
}
/*******************************************************************************
**
** Function         bt_ui_ct_act_incoming_call
**
** Description      Notifies user of incoming call
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ct_act_incoming_call(void)
{
    bt_ui_ct_sel_snd_device(AEE_SOUND_DEVICE_BT_CTP_HANDDSET);
    bt_ui_enable_backlight();
    //HT_MessageBoxTimeout(L"Incoming Call", BTUI_TIMEOUT_MSG_PERIOD);
    bt_ui_ct_cb->ct_menu_handle = bt_ui_message_box(L"Incoming Call",bt_ui_ct_act_incoming_call_cb,BTBAR_ANSWER_REJECT);
}

static void bt_ui_ct_act_call_conn_cb(UINT32 handle, UINT32 key)
{
    bt_ui_close_object(handle);
    bt_ui_ct_cb->ct_menu_handle = BT_UI_NULL_HANDLE;
    BCM_FUN_STAR("bt_ui_ct_act_call_conn_cb %x %x ", handle,key,0);
    if(key == HT_KEY_SOFT2 )
    {
        bcmapp_ct_end_call();
    }
}
/*******************************************************************************
**
** Function         bt_ui_ct_act_call_conn
**
** Description      Notifies user of call connected status
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ct_act_call_conn(void)
{
    if(BT_UI_NULL_HANDLE != bt_ui_ct_cb->ct_menu_handle)
    {
        bt_ui_close_object(bt_ui_ct_cb->ct_menu_handle);
    }
    //HT_MessageBoxTimeout(L"Call Connected", BTUI_TIMEOUT_MSG_PERIOD);
    bt_ui_ct_cb->ct_menu_handle = bt_ui_message_box(L"Call Connected",bt_ui_ct_act_call_conn_cb,BTBAR_END);
}
/*******************************************************************************
**
** Function         bt_ui_ct_act_call_disconn
**
** Description      Notifies user of call disconnection
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ct_act_call_disconn(void)
{
    if(BT_UI_NULL_HANDLE != bt_ui_ct_cb->ct_menu_handle)
    {
        bt_ui_close_object(bt_ui_ct_cb->ct_menu_handle);
    }
    bt_ui_enable_backlight();
    if(bt_ui_ct_sel_snd_device(AEE_SOUND_DEVICE_HANDSET))
    {
        HT_MessageBoxTimeout_ex(IDS_AG_DISCONNECT, BTUI_TIMEOUT_MSG_PERIOD);
    }
}

/*******************************************************************************
**
** Function         bt_ui_bta_ct_evt_hdlr
**
** Description      Handles events from bcmapp to ui
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_bta_ct_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_ct_evt_hdlr %d", p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
    case BTUI_MMI_CT_CALL_INCOMING:
        bt_ui_ct_act_incoming_call();
        break;

    case BTUI_MMI_CT_CALL_CONN:
        bt_ui_ct_act_call_conn();
        break;

    case BTUI_MMI_CT_CALL_DISCONN:
        bt_ui_ct_act_call_disconn();
        break;

    default:
        break;
    }

}

static boolean bt_ui_ct_sel_snd_device(AEESoundDevice device)
{
    /*need set sound device to AEE_SOUND_DEVICE_BT_CTP_HANDDSET*/
    BCM_FUN_STAR("bt_ui_ct_sel_snd_device", 0,0,0);
    if(NULL == g_pSound)
    {
        ISHELL_CreateInstance(bt_ui_get_shell(),  AEECLSID_SOUND,  (void**)&g_pSound);
    }
    
    if(g_pSound)
    {
        AEESoundInfo                soundStuff;
        byte vol = 0;
        OEM_GetConfig(CFGI_EAR_VOL, (void*)&vol, sizeof(vol));
        //vol_ag = bcmapp_ag_get_vol(vol);

        ISOUND_Get(g_pSound, &soundStuff);
        soundStuff.eDevice = AEE_SOUND_DEVICE_BT_CTP_HANDDSET;//AEE_SOUND_DEVICE_BT_CTP_HANDDSET;//
        soundStuff.eMethod = AEE_SOUND_METHOD_VOICE;
        soundStuff.eAPath = AEE_SOUND_APATH_BOTH;
        if(AEE_SOUND_DEVICE_HANDSET == device)
        {
            soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_MUTED;
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_MUTED;
        }
        else
        {
            soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
            soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
        }
        (void) ISOUND_Set(g_pSound,  &soundStuff);
        ISOUND_SetDevice(g_pSound); 
        ISOUND_SetVolume(g_pSound,  GET_ISOUND_VOL_LEVEL(vol));
        //ISound_Release(pSound);
        return TRUE;
    }
    return FALSE;
}
static void bt_ui_ct_show_device_detail(void)
{
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    tBTUI_REM_DEVICE * device = bt_ui_cb.p_selected_rem_device;
    //UINT32 handle;
    AECHAR menu_name[5][30];
    char c_name[BTUI_DEV_NAME_LENGTH+1];
    AECHAR name[BTUI_DEV_NAME_LENGTH+1];
    BCM_FUN_STAR("bt_ui_show_device_detail", 0,0,0);

    if(device->short_name[0])
    {
        STRCPY(c_name,device->short_name);
    }
    else if(device->name[0])
    {
        STRCPY(c_name,device->name);
    }
    else
    {
        STRCPY(c_name,"<NO NAME>");
    }

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
        JulianType  jDate;
        GetJulianDate(device->bond_time, &jDate);
        WSPRINTF(name, sizeof(name),L"%4d/%02d/%02d",jDate.wYear, jDate.wMonth, jDate.wDay);
    }
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

#endif

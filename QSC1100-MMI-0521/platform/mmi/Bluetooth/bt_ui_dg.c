/****************************************************************************
**                                                                           
**  Name:          btui_dg.c                                           
**                 
**  Description:   Contains btui functions for data gateway
**                 
**                                                                           
**  Copyright (c) 2002, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"

#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)

#include "gki.h"
#include "bta_api.h"
#include "bta_dg_api.h"
#include "btui.h"
#include "bta_dg_ci.h"
#include "btui_int.h"
#include <stdio.h>
#include <string.h>
#include "bd.h"
#include "btapp_dm.h"
#include "btapp_dg.h"

extern UINT32 btui_create_menu(tHT_MENU_ITEM*, UINT32, tHT_KEY_PRESSED_CBACK*, UINT8*);

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void btui_dg_event_hdlr(tBTUI_MMI_MSG *p_msg);
static void btui_act_dg_connections(UINT32 handle, UINT32 key);
static void btui_menu_view_dg_devices(void);
static void btui_dg_event_hdlr(tBTUI_MMI_MSG *p_msg);
static void btui_act_dg_connections(UINT32 handle, UINT32 key);
static void btui_menu_view_dg_devices(void);
static void btui_show_stored_dg_device_cback(UINT32 handle, UINT32 key);
static void btui_menu_select_dg_devices(void);
static void btui_act_connect_dg_device(void);
static void btui_act_delete_stored_dg_device(void);
static void btui_act_find_dg_devices(void);
static void btui_menu_dg_new_device_cback(UINT32 handle, UINT32 key);
static void btui_menu_select_new_dg_devices (void);
static void btui_act_new_dg_device_save (void);
static void btui_act_dg_loopback_test(void);
static void btui_dg_loopback_test_quit_display(UINT32 handle, UINT32 key);

void btui_dg_cback(tBTA_DG_EVT event, tBTA_DG *p_data);

tBTUI_DG_CB btui_dg_cb;

/*******************************************************************************
**
** Function         btui_dg_init
**
** Description      Initialises data gateway 
**                  
**
** Returns          void
**
*******************************************************************************/
void btui_dg_init(void)
{

    btapp_dg_init();

    btui_cfg.p_dg_event_hdlr = &btui_dg_event_hdlr;
    btui_cfg.p_dg_menu = &btui_menu_dg_connections;


}


/*******************************************************************************
**
** Function         btui_dg_event_hdlr
**
** Description      Process MMI commands related to data gateway
**                  
**
** Returns          void
**
*******************************************************************************/
static void btui_dg_event_hdlr(tBTUI_MMI_MSG *p_msg)
{

}

/*******************************************************************************
**
** Function         btui_menu_dg_connections
**
** Description      Data gateway menu
**                  
**
** Returns          void
**
*******************************************************************************/
void btui_menu_dg_connections(void)
{

    UINT8 next_menu = 0;

    tHT_MENU_ITEM * p_menu = menu_buffer;
    UINT32 handle;


    /* the main DG menu depends on  whether there
    is an active connection */
    if(!(btui_dg_cb.app_cb[BTUI_DG_ID_SPP].is_open ||
         btui_dg_cb.app_cb[BTUI_DG_ID_DUN].is_open ||
         btui_dg_cb.app_cb[BTUI_DG_ID_FAX].is_open ||
         btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].is_open))
    {
        p_menu[0].p_item_name = "View Devices";
        p_menu[0].item_data = (UINT32)btui_menu_view_dg_devices;
        p_menu[0].item_index = 1;

        handle = btui_create_menu(p_menu, 1, btui_menu_cback, "Data Connection");
        HT_SetSoftKeyMessages(handle, "Back", NULL);
        return;
    }

    if(btui_dg_cb.app_cb[BTUI_DG_ID_SPP].is_open)
    {
        p_menu[next_menu].p_item_name = "Disconnect SPP";
        p_menu[next_menu].item_data = BTUI_DG_ID_SPP;
        p_menu[next_menu].item_index = next_menu++;
    }
    if(btui_dg_cb.app_cb[BTUI_DG_ID_DUN].is_open)
    {
        p_menu[next_menu].p_item_name = "Disconnect DUN";
        p_menu[next_menu].item_data = BTUI_DG_ID_DUN;
        p_menu[next_menu].item_index = next_menu++;
    }
    if(btui_dg_cb.app_cb[BTUI_DG_ID_FAX].is_open)
    {
        p_menu[next_menu].p_item_name = "Disconnect FAX";
        p_menu[next_menu].item_data = BTUI_DG_ID_FAX;
        p_menu[next_menu].item_index = next_menu++;
    }
    if(btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].is_open)
    {
        p_menu[next_menu].p_item_name = "Disconnect client";
        p_menu[next_menu].item_data = BTUI_DG_ID_CLIENT;
        p_menu[next_menu].item_index = next_menu++;
    }
    /* if loopback mode is active display the menu */
    if(((btui_cfg.spp_loopback_mode == TRUE) || (btui_cfg.spp_senddata_mode == TRUE))
        &&(btui_dg_cb.app_cb[BTUI_DG_ID_SPP].is_open 
            || (btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].is_open
                 && btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].service_id ==BTA_SPP_SERVICE_ID)))
    {
        p_menu[next_menu].p_item_name = "SPP Test";
        p_menu[next_menu].item_data = BTUI_DG_ID_CLIENT+1;
        p_menu[next_menu].item_index = next_menu++;
    }
    handle = btui_create_menu(p_menu, next_menu, btui_act_dg_connections, "Data Connection");
    HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         btui_act_dg_connections
**
** Description      Action function when ui is in UI_DG_CURRENT_CONNECTIONS state
**                  
**
** Returns          void
**
*******************************************************************************/
static void btui_act_dg_connections(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM* p_item;

    if(key == HT_KEY_SOFT1)
    {
        btui_close_object(handle);
        return;
    }


    if(key == HT_KEY_SELECT)
    {
        p_item = HT_GetMenuSelectedItem(handle);

        if(p_item->item_data == (BTUI_DG_ID_CLIENT+1))
        {

            btui_act_dg_loopback_test();
            return;
        }

        btapp_dg_close_connection((UINT8)p_item->item_data);

        btui_close_object(handle);
        HT_MessageBoxTimeout("Disconnecting", BTUI_TIMEOUT_MSG_PERIOD);
    }
}

/*******************************************************************************
**
** Function         btui_act_dg_loopback_test
**
** Description      Displays loopback test status
**                  
**
** Returns          void
**
*******************************************************************************/
static void btui_act_dg_loopback_test(void)
{
 
    UINT32 handle;
    char msg_str[256];

    handle = btui_message_box("SPP TEST", btui_dg_loopback_test_quit_display);
    HT_SetSoftKeyMessages(handle, "Back", NULL);

    btui_cb.loop_back_test_status = TRUE;

    btui_close_object(HT_GetCurrMenuHandle());
    sprintf (msg_str, "SPP TEST      DETAILS IN MMI TERMINAL   "); 
    handle = btui_message_box(msg_str, btui_dg_loopback_test_quit_display);
    HT_SetSoftKeyMessages(handle, "Back", NULL); 
}

/*******************************************************************************
**
** Function         btui_dg_loopback_test_quit_display
**
** Description      Closes the loopback status display screen
**                  
**
** Returns          void
*******************************************************************************/
static void btui_dg_loopback_test_quit_display(UINT32 handle, UINT32 key)
{


    if(key == HT_KEY_SOFT1)
    {
        btui_cb.loop_back_test_status = FALSE;
        btui_close_object(handle);
        BTUI_cls();
    }
}


/*******************************************************************************
**
** Function         btui_menu_view_dg_devices
**
** Description      
**                  
**
** Returns          void
*******************************************************************************/
static void btui_menu_view_dg_devices(void)
{
    /* displays data devices in data base */
    btui_display_devices((1 << btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].service_id),
        btui_show_stored_dg_device_cback, L"Find New");
}
/*******************************************************************************
**
** Function         btui_show_stored_dg_device_cback
**
** Description     
**                  
**
** Returns          void
**
*******************************************************************************/
static void btui_show_stored_dg_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM* p_item;

    if(key == HT_KEY_SELECT)
    {
        p_item = HT_GetMenuSelectedItem(handle);

        btui_cb.p_selected_rem_device = &btui_device_db.device[p_item->item_data];            
        btui_menu_select_dg_devices();    
    }

    if(key == HT_KEY_SOFT1)
    {
        btui_close_object(handle);
        btui_close_object(HT_GetCurrMenuHandle());
        btui_menu_dg_connections();
    }

    if(key == HT_KEY_SOFT2)
    {
        btui_act_find_dg_devices();
    }
}

/*******************************************************************************
**
** Function         btui_menu_select_dg_devices
**
** Description      Displays menu options for a selected device
**                  
**
** Returns          void
*******************************************************************************/
static void btui_menu_select_dg_devices(void)
{

    UINT8 * p_name;
    tHT_MENU_ITEM * p_menu = menu_buffer;
    UINT32 handle;

    p_menu[0].p_item_name = "Connect";
    p_menu[0].item_data = (UINT32)btui_act_connect_dg_device;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Delete ";
    p_menu[1].item_data = (UINT32)btui_act_delete_stored_dg_device;
    p_menu[1].item_index = 2;

    /* Display the name of the device */
    if (strlen (btui_cb.p_selected_rem_device->short_name) > 0)
    {
         p_name = btui_cb.p_selected_rem_device->short_name;
    }
    else if (strlen(btui_cb.p_selected_rem_device->name))
    {
         p_name =  btui_cb.p_selected_rem_device->name;
    }
    else
    {
         p_name = "<NO NAME>";
    }

    handle = btui_create_menu(p_menu, 2, btui_menu_cback, p_name);
    HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         btui_act_connect_dg_device
**
** Description      Connects to the selected device
**                  
**
** Returns          void
*******************************************************************************/
static void btui_act_connect_dg_device(void)
{

    if (!btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].is_open)
    {


        HT_MessageBoxTimeout("Connecting", BTUI_TIMEOUT_MSG_PERIOD);

        btapp_dg_connect();
    }
}

/*******************************************************************************
**
** Function         btui_act_delete_stored_dg_device
**
** Description      Deletes the device from device data base
**                  
**
** Returns          void
*******************************************************************************/
static void btui_act_delete_stored_dg_device(void)
{
    btui_delete_device(btui_cb.p_selected_rem_device->bd_addr);
    btui_close_object(HT_GetCurrMenuHandle());
    btui_close_object(HT_GetCurrMenuHandle());
    btui_menu_view_dg_devices();

    HT_MessageBoxTimeout("Device   Deleted", BTUI_TIMEOUT_MSG_PERIOD);
}

/*******************************************************************************
**
** Function         btui_act_find_dg_devices
**
** Description      Action function to search for devices
**                  
**
** Returns          void
*******************************************************************************/
static void btui_act_find_dg_devices(void)
{
    btui_cb.ui_state = UI_NEW_DG_DEVICE;

    btui_search(BTA_DM_GENERAL_INQUIRY, (1 << btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].service_id),
        btui_menu_dg_new_device_cback, NULL);
}
/*******************************************************************************
**
** Function         btui_menu_dg_new_device_cback
**
** Description     
**                  
**
** Returns          void
*******************************************************************************/
static void btui_menu_dg_new_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM* p_item;

    if(key == HT_KEY_SELECT)
    {

        p_item = HT_GetMenuSelectedItem(handle);

        btui_cb.p_selected_rem_device = &btui_inq_db.remote_device[p_item->item_data];            
        btui_menu_select_new_dg_devices();    
    }

    if(key == HT_KEY_SOFT1)
    {
        btui_close_object(handle);
        btui_close_object(HT_GetCurrMenuHandle());
        btui_menu_view_dg_devices();
    }
}

/*******************************************************************************
**
** Function         btui_menu_select_new_dg_devices
**
** Description      Displays options for a newly discovered headset device
**                  
**
** Returns          void
*******************************************************************************/
static void btui_menu_select_new_dg_devices (void)
{
    UINT8 * p_name;
    tHT_MENU_ITEM * p_menu = menu_buffer;
    UINT32 handle;

    p_menu[0].p_item_name = "Connect";
    p_menu[0].item_data = (UINT32) btui_act_connect_dg_device;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Save ";
    p_menu[1].item_data = (UINT32)btui_act_new_dg_device_save;
    p_menu[1].item_index = 2;

    /* Display the name of the device */
    if(strlen(btui_cb.p_selected_rem_device->name))
    {
        p_name =  btui_cb.p_selected_rem_device->name;
    }
    else
    {
        p_name = "<NO NAME>";
    }

    handle = btui_create_menu(p_menu, 2, btui_menu_cback, p_name);

    HT_SetSoftKeyMessages(handle, "Back", NULL);
}

/*******************************************************************************
**
** Function         btui_act_new_dg_device_save
**
** Description      Saves the device to device database
**                  
**
** Returns          void
*******************************************************************************/
static void btui_act_new_dg_device_save (void)
{
    if(btui_store_device(btui_cb.p_selected_rem_device))
        HT_MessageBoxTimeout("Device Saved", BTUI_TIMEOUT_MSG_PERIOD);
    else
        HT_MessageBoxTimeout("Database full", BTUI_TIMEOUT_MSG_PERIOD);

}



#endif
/*****************************************************************************
**
**  Name:           bt_ui_opc.c
**
**  Description:    
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include "bt_target.h"

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)

#include <stdio.h>
#include "gki.h"
#include "bta_api.h"
#include "bta_op_api.h"
#include "bta_fs_api.h"
#include "bta_fs_co.h"
#include "bd.h"
#include "bt_ui.h"
#include "bt_ui_int.h"
#include "bcmapp_dm.h"
#include "bcmapp_opc.h"
#include "bt_ui_ht_common.h"
#include "AEEFILE.h"
#include "Contapp.h"
/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
//static void bt_ui_opc_event_hdlr(tBTUI_MMI_MSG *p_msg);
#ifdef BT_BROADCOM_SAMPLE_CODE
//static void bt_ui_menu_op_devices (void);
static void bt_ui_menu_view_op_devices(void);
//static void bt_ui_act_find_op_devices(void);
static void bt_ui_menu_select_op_devices (boolean is_new);
//static void bt_ui_act_new_op_devices(tBTUI_MMI_MSG *p_msg);
//static void bt_ui_act_select_op_device(tBTUI_MMI_MSG *p_msg);
//static void bt_ui_menu_select_new_op_device (void);
static void bt_ui_act_save_op_device(void);
//static void bt_ui_menu_view_new_op_devices(void);
//static void bt_ui_act_select_new_op_device(tBTUI_MMI_MSG *p_msg);
static void bt_ui_act_send_file (UINT8 index);
static void bt_ui_menu_view_files(void);
//static void bt_ui_opc_push(void);
//void bt_ui_opc_cback(tBTA_OPC_EVT event, tBTA_OPC *p_data);
static void bt_ui_menu_opc_new_device_cback(UINT32 handle, UINT32 key);
static void bt_ui_opc_close(void);

static void bt_ui_show_stored_opc_device_cback(UINT32 handle, UINT32 key);
static UINT8 bt_ui_platform_opc_list_files(void);
static char *bt_ui_opc_platform_get_path(UINT8 id, char *p_name);
//extern UINT32 bt_ui_create_menu(tHT_MENU_ITEM*, UINT32, tHT_KEY_PRESSED_CBACK*, UINT8*);
#endif
/* Maximum path length supported by MMI */
//#ifndef BTUI_PATH_LEN
//#define BTUI_PATH_LEN           260
//#endif
//#define BTUI_OPC_FILE_NAME_SIZE 196
#define BTUI_OPC_FILE_NAME_SIZE 64
#define BTUI_MAX_FILES_TO_SEND  100

#define BTUI_OPC_MEM_LEN            8224
#define BTUI_OPC_NUM_PROP           8
//UINT8   bt_ui_opc_test_mem[BTUI_OPC_MEM_LEN];

/* BTUI OPC main control block */
static tBTUI_OPC_CB *bt_ui_opc_cb = NULL;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
static char  file_name[BTUI_MAX_FILES_TO_SEND][BTUI_OPC_FILE_NAME_SIZE];
#endif
//static boolean bt_ui_opc_change_brewaddr_to_fs(char * brewaddr,char * fsaddr );
#ifdef BT_UI_PULL_EXCH_CARD
static void bt_ui_opc_pull_card(void);
static void bt_ui_opc_exch_card(void);
#endif
/*******************************************************************************
**
** Function         bt_ui_opc_init
**
** Description     Initializes BTUI & BTA OPC 
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_opc_init(void)
{
    //bt_ui_cfg.p_opc_event_hdlr = bt_ui_opc_event_hdlr;
    //bt_ui_cfg.p_opc_menu = &bt_ui_menu_op_devices;
    
    bt_ui_opc_cb = bcmapp_ag_get_opc_cb();
    if(bt_ui_opp_test_file_addr(BT_OPC_PUSH_ADDR))
    {
        bcmapp_opc_init();
    }
    BCM_FUN_STAR("bt_ui_opc_cb->single_op: %d", bt_ui_opc_cb->single_op,0,0);
}

/*******************************************************************************
**
** Function         bt_ui_opc_event_hdlr
**
** Description      Process user inputs to menu options
**                  related to bluetooth object push client
**                  
**
** Returns          void
*******************************************************************************/
//static void bt_ui_opc_event_hdlr(tBTUI_MMI_MSG *p_msg)
//{
//    switch(bt_ui_cb.ui_state)
//    {
//    default:
//        break;
//    }
//}
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_menu_opc_devices
**
** Description      Object push menu
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_menu_opc_devices (void)
{
    
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    BCM_FUN_STAR("bt_ui_menu_opc_devices %d",bt_ui_opc_cb->is_enabled,0,0);
    
    if (bt_ui_opc_cb->is_enabled)
    {
        p_menu[0].p_item_name = "View Devices";
        p_menu[0].item_data = (UINT32)bt_ui_menu_view_op_devices;
        p_menu[0].item_index = 1;
        handle = bt_ui_create_menu(p_menu, 1,bt_ui_menu_cback, L"Object Push" ,BTBAR_SELECT_BACK,FALSE,0);
    }
    else
    {
        p_menu[0].p_item_name = "";
        p_menu[0].item_data = (UINT32)bt_ui_show_stored_opc_device_cback;
        p_menu[0].item_index = 1;
        handle = bt_ui_create_menu(p_menu, 1,bt_ui_menu_cback, L"OPC Disabled!!!" ,BTBAR_SELECT_BACK,FALSE,0);
    }
}


/*******************************************************************************
**
** Function         bt_ui_menu_view_op_devices
**
** Description      Object push menu
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_op_devices(void)
{
    BCM_FUN_STAR("bt_ui_menu_view_op_devices",0,0,0);
    /*bt_ui_cb.num_op_devices = */bt_ui_display_devices(BTA_OPP_SERVICE_MASK, 
        bt_ui_show_stored_opc_device_cback, 
        L"Find New");
}

static void bt_ui_show_stored_opc_device_cback(UINT32 handle, UINT32 key)
{
    
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_show_stored_opc_device_cback %x %x",handle,key,0);
    if (!bt_ui_opc_cb->is_enabled)
        return;
    
    if(key == HT_KEY_SELECT)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle,&p_item))
        {
            bt_ui_cb.p_selected_rem_device = &bt_ui_device_db.device[p_item.item_data];            
            bt_ui_menu_select_op_devices(FALSE);    
        }
    }
    
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
    }
    
    if(key == HT_KEY_SOFT1)
    {
        bt_ui_act_find_op_devices();
    }
    
}




/*******************************************************************************
**
** Function         bt_ui_act_find_op_devices
**
** Description      Action function to search for op devices
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_act_find_op_devices(void)
{
    /* filter: Major service class is set on 20th bit*/
    tBTA_DM_COD_COND  dev_filt = {{0x10,0x00, 0x00}, {0x10,0x00, 0x00}};
    BCM_FUN_STAR("bt_ui_act_find_op_devices",0,0,0);
    bt_ui_cb.ui_state = UI_NEW_OP_DEVICE;
    
    bt_ui_search(BTA_DM_GENERAL_INQUIRY, BTA_OPP_SERVICE_MASK, bt_ui_menu_opc_new_device_cback, &dev_filt);
}

/*******************************************************************************
*******************************************************************************/
static void bt_ui_menu_opc_new_device_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_opc_new_device_cback %x %x",handle,key,0);
    if((key == HT_KEY_SELECT) || (key == HT_KEY_SOFT1))
    {       
        if(SUCCESS == HT_GetMenuSelectedItem(handle,&p_item)) 
        {
            bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[p_item.item_data];            
            bt_ui_menu_select_op_devices(TRUE);    
        }
        
    }
    
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_set_draw_hold_down(TRUE);
        // Close the serach handle.
        bt_ui_close_object(handle);
        bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
        bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
        // Rebuild the view opp devices menu.
        bt_ui_close_object(HT_GetCurrMenuHandle());
        bt_ui_menu_view_op_devices();
        bt_ui_set_draw_hold_down(FALSE);
    }
}


/*******************************************************************************
**
** Function         bt_ui_menu_select_op_devices
**
** Description      Displays menu options for a selected  headset device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_op_devices (boolean is_new)
{
    char * p_name;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    UINT16 i = 0;
    BCM_FUN_STAR("bt_ui_menu_select_op_devices %d",is_new,0,0);
    p_menu[i].p_item_name = "View Objects";
    p_menu[i].item_data = (UINT32)bt_ui_menu_view_files;
    p_menu[i].item_index = i;
    i++;
    
    if(is_new)
    {
        p_menu[i].p_item_name = "Save Device ";
        p_menu[i].item_data = (UINT32)bt_ui_act_save_op_device;
        p_menu[i].item_index = i;
        i++;
    }
    
    p_menu[i].p_item_name = "Pull Card";
    p_menu[i].item_data = (UINT32)bt_ui_opc_pull_card;
    p_menu[i].item_index = i;
    i++;
    
    p_menu[i].p_item_name = "Exch Card";
    p_menu[i].item_data = (UINT32)bt_ui_opc_exch_card;
    p_menu[i].item_index = i;
    i++;
    
    if(!bt_ui_opc_cb->single_op)
    {
        p_menu[i].p_item_name = "Close";
        p_menu[i].item_data = (UINT32)bt_ui_opc_close;
        p_menu[i].item_index = i;
        i++;
    }
    
#if (GKI_DEBUG == TRUE)
    p_menu[i].p_item_name = "GKI buffers";
    p_menu[i].item_data = (UINT32)GKI_PrintBufferUsage;
    p_menu[i].item_index = i;
    i++;
#endif
    
    /* Display the name of the device */
    if (strlen (bt_ui_cb.p_selected_rem_device->short_name) > 0)
    {
        p_name = bt_ui_cb.p_selected_rem_device->short_name;
    }
    else if (strlen(bt_ui_cb.p_selected_rem_device->name))
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
        handle = bt_ui_create_menu(p_menu, i,bt_ui_menu_cback, szWStr ,BTBAR_SELECT_BACK,FALSE,0);
    }
}

#if 0
/*******************************************************************************
**
** Function         bt_ui_menu_view_new_op_devices
**
** Description      New op devices menu
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_new_op_devices(void)
{
	
	
	bt_ui_show_new_devices(bt_ui_menu_opc_new_device_cback);
	
	
}

/*******************************************************************************
**
** Function         bt_ui_act_new_op_devices
**
** Description      Action function for UI_SELECT_OP_DEVICE state
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_new_op_devices(tBTUI_MMI_MSG *p_msg)
{
	/* exit */
	if((p_msg->data[0] == MENU_ITEM_0))
	{
		bt_ui_menu_view_op_devices();
	}
	else if ((p_msg->data[0] > MENU_ITEM_0) 
		&&((p_msg->data[0] - MENU_ITEM_0) <= bt_ui_inq_db.rem_index))
	{
		/* select one headset from the newly discovered headset */
		bt_ui_cb.p_selected_rem_device = &bt_ui_inq_db.remote_device[(p_msg->data[0] - MENU_ITEM_0)-1];
		BTUI_cls();
		bt_ui_menu_select_new_op_device();
	}
}

/*******************************************************************************
**
** Function         bt_ui_menu_select_new_op_device
**
** Description      Displays menu options for a selected  op device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_select_new_op_device (void)
{
	UINT8 * p_name;
	tHT_MENU_ITEM * p_menu = menu_buffer;
	UINT32 handle;
	UINT32 menu_item_count = 4;
	
	
	p_menu[0].p_item_name = "View Objects";
	p_menu[0].item_data = (UINT32)bt_ui_menu_view_files;
	p_menu[0].item_index = 1;
	
	p_menu[1].p_item_name = "Save Device ";
	p_menu[1].item_data = (UINT32)bt_ui_act_save_op_device;
	p_menu[1].item_index = 2;
	
	p_menu[2].p_item_name = "Pull Card";
	p_menu[2].item_data = (UINT32)bt_ui_opc_pull_card;
	p_menu[2].item_index = 3;
	
	p_menu[3].p_item_name = "Exch Card";
	p_menu[3].item_data = (UINT32)bt_ui_opc_exch_card;
	p_menu[3].item_index = 4;
	
	if(!bt_ui_opc_cb.single_op)
	{
		p_menu[menu_item_count].p_item_name = "Close";
		p_menu[menu_item_count].item_data = (UINT32)bt_ui_opc_close;
		p_menu[menu_item_count].item_index = menu_item_count++;
	}
	
	/* Display the name of the device */
	if(strlen(bt_ui_cb.p_selected_rem_device->name))
	{
		p_name =  bt_ui_cb.p_selected_rem_device->name;
	}
	else
	{
		p_name = "<NO NAME>";
	}
	
	handle = bt_ui_create_menu(p_menu, menu_item_count,bt_ui_menu_cback, p_name);
	
	HT_SetSoftKeyMessages(handle, "Back", NULL);
	
	
	
}



#endif

/*******************************************************************************
**
** Function         bt_ui_act_save_op_device
**
** Description      Action function to save op device
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_save_op_device(void)
{
    BCM_FUN_STAR("bt_ui_act_save_op_device",0,0,0);
    if(bt_ui_store_device(bt_ui_cb.p_selected_rem_device))
        HT_MessageBoxTimeout(L"Device   Saved", BTUI_TIMEOUT_MSG_PERIOD);
    else
        HT_MessageBoxTimeout(L"Database  Full", BTUI_TIMEOUT_MSG_PERIOD);
}

/*******************************************************************************
**
** Function         bt_ui_menu_view_files
**
** Description      List the files available for sending
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_menu_view_files(void)
{
    BCM_FUN_STAR("bt_ui_menu_view_files",0,0,0);
    bt_ui_cb.ui_state = UI_VIEW_OP_FILES;
    
    bt_ui_opc_cb->num_files = bt_ui_platform_opc_list_files();
}
#endif
//extern void bt_ui_opc_platform_get_default_card(char *p_send);
//extern void bt_ui_opc_platform_get_default_rcv_path(char *p_rcv_path);
#ifdef BT_UI_PULL_EXCH_CARD
/*******************************************************************************
**
** Function         bt_ui_menu_view_files
**
** Description      List the files available for sending
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_opc_pull_card(void)
{
    BCM_FUN_STAR("bt_ui_opc_pull_card %x %x",bt_ui_opc_cb->is_connected,bt_ui_opc_cb->p_connected_rem_device,0);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(memcmp (bt_ui_opc_cb->p_connected_rem_device->bd_addr, 
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        return;
    }
    
    if (!bcmapp_opc_pull_card())
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        
    }
    
}

/*******************************************************************************
**
** Function         bt_ui_opc_exch_card
**
** Description      Initiates card exchange
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_opc_exch_card(void)
{
    BCM_FUN_STAR("bt_ui_opc_exch_card %x %x",bt_ui_opc_cb->is_connected,bt_ui_opc_cb->p_connected_rem_device,0);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(memcmp (bt_ui_opc_cb->p_connected_rem_device->bd_addr, 
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        return;
    }
    
    if (!bcmapp_opc_exch_card())
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        
    }
    
}
#endif
#ifdef BT_BROADCOM_SAMPLE_CODE
/*******************************************************************************
**
** Function         bt_ui_opc_exch_card
**
** Description      Initiates card exchange
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_opc_close(void)
{
    BCM_FUN_STAR("bt_ui_opc_close %x %x",bt_ui_opc_cb->is_connected,bt_ui_opc_cb->p_connected_rem_device,0);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(!memcmp (bt_ui_opc_cb->p_connected_rem_device->bd_addr, 
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        BTA_OpcClose();
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
    }
}


void bt_ui_menu_opc_select_file_cback(UINT32 handle, UINT32 key)
{
    tHT_MENU_ITEM p_item;
    BCM_FUN_STAR("bt_ui_menu_opc_select_file_cback %x %x",handle,key,0);
    if(key == HT_KEY_SELECT)
    {
        if(SUCCESS == HT_GetMenuSelectedItem(handle,&p_item)) 
        {
            bt_ui_act_send_file((UINT8)(p_item.item_data + 1)); 
        }
    }
    
    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        
    }
    
    
    
}

/*******************************************************************************
**
** Function         bt_ui_act_send_file
**
** Description      Sends the file 
**                  
**
** Returns          void
*******************************************************************************/
static void bt_ui_act_send_file (UINT8 index)
{
    char  btui_opc_path[BTUI_PATH_LEN];
    char  btui_fs_path[BTUI_PATH_LEN];
    BCM_FUN_STAR("bt_ui_act_send_file %x %x %x",bt_ui_opc_cb->is_connected,bt_ui_opc_cb->p_connected_rem_device,index);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(memcmp (bt_ui_opc_cb->p_connected_rem_device->bd_addr,
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        return;
    }
    
    bt_ui_opc_platform_get_path(index,btui_opc_path);
    bcmapp_dm_change_brewaddr_to_fs(btui_opc_path,btui_fs_path);
#if(defined BT_OEM_DEBUG)
    DBGPRINTF("btui_opc_path %s %s",btui_opc_path,btui_fs_path,0);
#endif
    //strcpy(btui_opc_path,"/brew/default_opc.vcf");
    if(bcmapp_opc_act_send_file(btui_fs_path))
    {
        
        HT_MessageBoxTimeout_ex(IDS_OPC_SENDING, BTUI_TIMEOUT_MSG_PERIOD);
        
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        
        bt_ui_menu_view_files();
    }
    
}
#endif
static boolean bt_ui_opc_proc_contid_to_fs(sSelectFieldListNode * node,char * fsaddr )
{
#ifdef FEATURE_CONTACT_APP
    IContApp * pContApp = NULL;
    BCM_FUN_STAR("bt_ui_opc_proc_contid_to_fs", 0,0,0);
    if(NULL == node)
        return FALSE;
    
    if (SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(),
        AEECLSID_APP_CONTACT,
        (void**)&pContApp))
    {
        if (pContApp == NULL)
        {
            return FALSE;
        }

        fsaddr[0] = '\0';
        ICONTAPP_CreateVcfFile(pContApp, node->SelectFieldInfo.wIndex, fsaddr);
#if(defined BT_OEM_DEBUG)
        DBGPRINTF("tangwq after bt call create:%s ", fsaddr,0,0);
#endif
        if(fsaddr[0] != '\0')
        {
            STRCPY(bt_ui_cb.opp_send_file_addr,fsaddr);
        }
        else
        {
            (void)ICONTAPP_Release(pContApp);
            return FALSE;
        }
    }

    (void)ICONTAPP_Release(pContApp);
#endif

    return TRUE;
}

static void bt_ui_act_send_file_local (void)
{
    char  btui_fs_path[BTUI_PATH_LEN];
    BCM_FUN_STAR("bt_ui_act_send_file_local %x %x %x",bt_ui_opc_cb->is_connected,bt_ui_cb.ui_state,bt_ui_opc_cb->opp_send_file_p_loc);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(0 == MEMCMP (bt_ui_opc_cb->p_connected_rem_device->bd_addr,
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        return;
    }
    if(bt_ui_opc_cb->opp_send_file_p_loc != NULL)
    {
        bt_ui_opc_proc_contid_to_fs((sSelectFieldListNode*)bt_ui_opc_cb->opp_send_file_p_loc,btui_fs_path);
    }
    else
    {
        bcmapp_dm_change_brewaddr_to_fs(bt_ui_cb.opp_send_file_addr,btui_fs_path);
    }
#if(defined BT_OEM_DEBUG)
    DBGPRINTF("btui_opc_path %s %s",bt_ui_cb.opp_send_file_addr,btui_fs_path,0);
#endif
    //strcpy(btui_opc_path,"/brew/default_opc.vcf");
    if(bcmapp_opc_act_send_file(btui_fs_path))
    {
        bt_ui_cb.b_opp_show_proc = TRUE;
        HT_MessageBoxTimeout_ex(IDS_OPC_SENDING, BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        //bt_ui_menu_view_files();
    }
    //bt_ui_cb.m_bsend_opc_file = FALSE;
}

void bt_ui_act_send_file_ex (void)
{
#if 1
    bt_ui_opc_cb->opp_send_file_p_loc = bt_ui_cb.opp_send_file_p;
    bt_ui_act_send_file_local();
#else
    char  btui_fs_path[BTUI_PATH_LEN];
    BCM_FUN_STAR("bt_ui_act_send_file %x %x ",bt_ui_opc_cb->is_connected,bt_ui_opc_cb->p_connected_rem_device,0);
    if((bt_ui_opc_cb->is_connected)
        &&(bt_ui_opc_cb->p_connected_rem_device)
        &&(bt_ui_cb.p_selected_rem_device->bd_addr)
        &&(MEMCMP (bt_ui_opc_cb->p_connected_rem_device->bd_addr,
        bt_ui_cb.p_selected_rem_device->bd_addr, BD_ADDR_LEN)))
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        return;
    }
    if(bt_ui_cb.opp_send_file_p != NULL)
    {
        bt_ui_opc_proc_contid_to_fs((sSelectFieldListNode*)bt_ui_cb.opp_send_file_p,btui_fs_path);
    }
    else
    {
        bcmapp_dm_change_brewaddr_to_fs(bt_ui_cb.opp_send_file_addr,btui_fs_path);
    }
#if(defined BT_OEM_DEBUG)
    DBGPRINTF("btui_opc_path %s %s",bt_ui_cb.opp_send_file_addr,btui_fs_path,0);
#endif
    //strcpy(btui_opc_path,"/brew/default_opc.vcf");
    if(bcmapp_opc_act_send_file(btui_fs_path))
    {
        bt_ui_cb.b_opp_show_proc = TRUE;
        HT_MessageBoxTimeout_ex(IDS_OPC_SENDING, BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        //bt_ui_menu_view_files();
    }
    //bt_ui_cb.m_bsend_opc_file = FALSE;
#endif
}

static void bt_ui_opc_act_object(tBTUI_BTA_MSG *p_msg)
{
    AECHAR t_msg[100];
    BCM_FUN_STAR("bt_ui_opc_act_object %d",p_msg->hdr.layer_specific,0,0);
    if (p_msg->hdr.layer_specific == BTA_OPC_OK)
    {
        UTF8TOWSTR((byte *)p_msg->ops_object.name,100,t_msg,sizeof(t_msg));
        ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_DONE, t_msg+ WSTRLEN(t_msg), sizeof(t_msg)) ;
        HT_MessageBoxTimeout(t_msg, BTUI_TIMEOUT_MSG_PERIOD);
    }
    //else if (p_msg->hdr.layer_specific == BTA_OPC_NOT_FOUND)
    //{
    //    HT_MessageBoxTimeout(bt_ui_get_brx_string(IDS_FAIL), BTUI_TIMEOUT_MSG_PERIOD);
    //}
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
    }
#if(defined BT_OEM_DEBUG)
    DBGPRINTF("bt_ui_opc_act_object %S",t_msg);
#endif
}

static void bt_ui_opc_act_close(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_opc_act_close %d",p_msg->opc_close.status,0,0);
    if (p_msg->opc_close.status == BTA_OPC_OK)
    {
        HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
    }
    else
    {
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
    }
}
static void bt_ui_opc_act_pshd(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_opc_act_pshd %d",p_msg->open.status,0,0);
    switch (p_msg->open.status)
    {
    case BTA_OPC_OK:
        HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);
        break;
        
    //case BTA_OPC_NO_PERMISSION:
    //    HT_MessageBoxTimeout(L"Object Not Sent [No Permission]", BTUI_TIMEOUT_MSG_PERIOD);
    //    break;
        
    default:
        HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        break;
    }
}
static void bt_ui_opc_act_proc_cb(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_opc_act_proc_cb %x %x %x",handle,key,bt_ui_cb.after_resume_menu);
    if( HT_KEY_SOFT2 == key)
    {
        bt_ui_cb.b_opp_show_proc = FALSE;
        bt_ui_close_object(handle);
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = TRUE;
#endif
        if(bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
    }
}
static void bt_ui_opc_act_proc(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_opc_act_proc %d %d",p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,0);
    if(bt_ui_cb.b_opp_show_proc && (FALSE == bt_ui_get_suspend()))
    {
        HT_MessageBoxProcess(bt_ui_get_brx_string(IDS_OPC_SENDING),p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,bt_ui_opc_act_proc_cb,BTBAR_CANCEL);
    }
}
void bt_ui_bta_opc_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_opc_evt_hdlr %d",p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
    case BTUI_MMI_OPC_OBJECT:/* Object Pulled */
        bt_ui_opc_act_object (p_msg);
        break;
        
    case BTUI_MMI_OPC_CLOSE:
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif

        if(bt_ui_opc_cb->opp_send_file_p_loc!= NULL)//this only for send vcard!!@!!!
        {
            if(SUCCESS == IFILEMGR_Test(bt_ui_get_FileMgr(),bt_ui_cb.opp_send_file_addr) )
            {
                IFILEMGR_Remove(bt_ui_get_FileMgr(),bt_ui_cb.opp_send_file_addr);
            }

            MEMSET(bt_ui_cb.opp_send_file_addr,0,sizeof(bt_ui_cb.opp_send_file_addr));
            if(((sSelectFieldListNode*)bt_ui_opc_cb->opp_send_file_p_loc)->pNextNode!= NULL)
            {
                bt_ui_opc_cb->opp_send_file_p_loc = ((sSelectFieldListNode*)bt_ui_opc_cb->opp_send_file_p_loc)->pNextNode;
                bt_ui_act_send_file_local();
            }
        }
        else
        {
            bt_ui_opc_act_close (p_msg);
        }
        break;
        
    case BTUI_MMI_OPC_PSHD: /* Object pushed */
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif
        bt_ui_opc_act_pshd (p_msg);
        break;

    case BTUI_MMI_OPC_PROC:
        bt_ui_opc_act_proc (p_msg);
        break;

    case BTUI_MMI_OPC_OPEN:/* Connection to peer is open. */
        bt_ui_cb.ui_state = UI_SENDING_OP_FILE;
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_ON);
#endif
        break;
        
    default:
        break;
    }
    
}
#ifdef BT_BROADCOM_SAMPLE_CODE
static UINT8 bt_ui_platform_opc_list_files(void)
{
#ifdef BT_UI_OPC_TEST_CARD_MEMU
    char    dir_name [BTUI_MAX_PATH_LENGTH];
    UINT16     i;
    UINT8 xx = 0;
    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;
    char *temp_array;
    IFileMgr        *m_pIFileMgr = NULL; 
    if (AEE_SUCCESS != ISHELL_CreateInstance(bt_ui_get_shell(),
                                            AEECLSID_FILEMGR,
                                            (void **)&m_pIFileMgr))
    {
        return 0;
    }
    sprintf (dir_name, "%s%s",AEEFS_ROOT_DIR,BT_OPC_PUSH_ADDR);
    bt_ui_cfg.hset_ui_mode = TRUE;
    if(IFILEMGR_EnumInit(m_pIFileMgr, dir_name, FALSE)  != SUCCESS)
    {

        if(bt_ui_cfg.hset_ui_mode == TRUE)
        {
            //p_menu[0].p_item_name = "No Files";
            //p_menu[0].item_data = (UINT32)NULL;
            //p_menu[0].item_index = 1;
            //handle = bt_ui_create_menu(p_menu, 1,bt_ui_menu_opc_select_file_cback, NULL);
            //HT_SetSoftKeyMessages(handle, L"Select", L"Back");
            HT_MessageBoxTimeout(L"No Files", BTUI_TIMEOUT_MSG_PERIOD);
        }
        else
        {
            //BTUI_puts(" No Files\n");
            //BTUI_puts(" 0.Previous Menu \n");
        }
        return 0;
    }
    else
    {
        FileInfo  Photopos;
        //BCM_MSG_MED("IFILEMGR_EnumNext",0,0,0);
        while(IFILEMGR_EnumNext(m_pIFileMgr, &Photopos))
        {
            strncpy(file_name[xx], Photopos.szName, BTUI_OPC_FILE_NAME_SIZE);
            xx++;
        }
        IFILEMGR_Release(m_pIFileMgr);
        m_pIFileMgr = NULL;
    }
    BCM_MSG_MED("IFILEMGR_EnumNext %d",xx,0,0);
    if(xx)
    {
        for(i=0; i<xx; i++)
        {
            if(bt_ui_cfg.hset_ui_mode == FALSE)
            {
                //sprintf(msg_str, " %d  %s \n", i+1, file_name[i]);
                //BTUI_puts(msg_str);
            }
            else
            {
                int Start_bit;
                Start_bit = STRLEN(dir_name);
                temp_array = &file_name[i][Start_bit];
#if(defined BT_OEM_DEBUG)
                DBGPRINTF("temp_array %s",temp_array,0,0);
#endif
                p_menu[i].p_item_name = temp_array;
                p_menu[i].item_data = (UINT32)i;
                p_menu[i].item_index = i+1;
            }
        }
        if(bt_ui_cfg.hset_ui_mode == FALSE)
        {

            //BTUI_puts(" 0.Previous Menu \n");
        }
        else
        {
            handle = bt_ui_create_menu(p_menu, i,bt_ui_menu_opc_select_file_cback, L"Send File" ,BTBAR_SELECT_BACK,FALSE,0);
        }
        //for(i=0; i<xx; i++)
        //{
        //    file_name[i][BTUI_SCREEN_LENGTH] = temp_array[i];
        //}
    }
    else
    {
        if(bt_ui_cfg.hset_ui_mode == TRUE)
        {
            handle = bt_ui_message_box(L"No Files",bt_ui_menu_opc_select_file_cback,BTBAR_BACK);
        }
        else
        {
            //BTUI_puts(" No Files\n");
            //BTUI_puts(" 0.Previous Menu \n");
        }
    }

    return xx;
#else
    return 0;
#endif
}

static char *bt_ui_opc_platform_get_path(UINT8 id, char *p_name)
{
    char *name = NULL;
    if(id > BTUI_MAX_FILES_TO_SEND)
        return NULL;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
    name = file_name[id-1];
#endif
    if(!name)
        return NULL;
    strcpy(p_name,name);
    //sprintf (p_name, "%s\\test_files\\opc\\%s",bt_ui_cfg.root_path, *name);
    return name;
}
#endif
void bt_ui_opc_disable(void)
{
    bcmapp_opc_disable();
}

//static boolean bt_ui_opc_change_brewaddr_to_fs(char * brewaddr,char * fsaddr )
//{
//    int len = 0;
//    if(brewaddr == NULL ||fsaddr == NULL )
//    {
//        return FALSE;
//    }
//    //brewaddr -> fs:/opp/
//    //fsaddr     -> /brew/opp/
//    len = STRLEN(AEEFS_ROOT_DIR);
//    STRCPY(fsaddr, bt_ui_cfg.root_path);
//    STRCAT(fsaddr, brewaddr + len);
//    return TRUE;
//}

boolean bt_ui_opp_test_file_addr(char * file_addr)
{
    IFileMgr        *m_pIFileMgr = NULL; 
    boolean ok = FALSE;
    if (AEE_SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(),
                                            AEECLSID_FILEMGR,
                                            (void **)&m_pIFileMgr))
    {
        char addr[100] ;
        sprintf(addr,"%s%s",AEEFS_ROOT_DIR,file_addr);
#if(defined BT_OEM_DEBUG)
        DBGPRINTF("addr ===%s %s",addr,file_addr);
#endif
        if(EFAILED == IFILEMGR_Test(m_pIFileMgr,addr))
        {
            //need creat
            //BCM_FUN_STAR("bt_ui_opp_test_file_addr0", 0,0,0);
            if(SUCCESS == IFILEMGR_MkDir(m_pIFileMgr,addr))
            {
                ok = TRUE;
            }
        }
        else
        {
            ok = TRUE;
        }
        BCM_FUN_STAR("bt_ui_opp_test_file_addr %d", ok,0,0);
        IFILEMGR_Release(m_pIFileMgr);
        m_pIFileMgr = NULL;
    }
    return ok;
}
#endif


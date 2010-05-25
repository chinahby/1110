/****************************************************************************
**                                                                           
**  Name:          btui_fts.c                                           
**                 
**  Description:   Contains btui functions for file transfer server
**                 
**                                                                           
**  Copyright (c) 2003, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"

#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)


//#ifndef __CYGWIN__
//#include <direct.h>
//#endif

#include "gki.h"
#include "bta_api.h"
#include "bt_ui.h"
#include "bta_fs_api.h"
#include "bta_ft_api.h"
#include "bta_fs_co.h"
#include "bt_ui_int.h"
#include "bcmapp_fts.h"
#include "bt_ui_ht_common.h"
/* Root name off of the working directory */
//#define FTS_ROOT_FOLDER         "test_files/fts"
//#define FTS_DEFAULT_ROOT_FOLDER "bta_fts_root"

/* Maximum path length supported by MMI */
//#ifndef BTUI_PATH_LEN
//#define BTUI_PATH_LEN           260
//#endif

/* BTUI FT server main control block */
tBTUI_FTS_CB *btui_fts_cb = NULL;

//extern UINT32 btui_create_menu(tHT_MENU_ITEM*, UINT32, tHT_KEY_PRESSED_CBACK*, UINT8*);

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
//static void btui_fts_event_hdlr(tBTUI_MMI_MSG *p_msg);
//static void btui_menu_fts_main(void);
static void btui_act_fts_main(UINT32 handle, UINT32 key);


/*******************************************************************************
**
** Function         btui_fts_init
**
** Description      FTS main menu event initialization function.
**                  
**
** Returns          void
**
*******************************************************************************/
void bt_ui_fts_init(void)
{
    btui_fts_cb = bcmapp_get_fts_cb();
    if (!btui_fts_cb->b_fts_enable)
    {
        bcmapp_fts_init();
    }
}

void bt_ui_fts_disable(void)
{
    if (btui_fts_cb->b_fts_enable)
    {
        bcmapp_fts_disable();
    }
}
static void bt_ui_fts_act_proc_cb(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_fts_act_proc_cb %x %x %x",handle,key,bt_ui_cb.after_resume_menu);
    if( HT_KEY_SOFT2 == key)
    {
        bt_ui_cb.b_opp_show_proc = FALSE;
        bt_ui_close_object(handle);
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = TRUE;
#endif
#ifndef AEE_SIMULATOR
            IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif
        if(bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
    }
}

static void bt_ui_fts_act_proc(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_fts_act_proc %d %d %d",p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,bt_ui_cb.b_opp_show_proc);
    if(bt_ui_cb.b_opp_show_proc && (FALSE == bt_ui_get_suspend()))
    {
        HT_MessageBoxProcess(bt_ui_get_brx_string(IDS_FILE_TRANSFER),p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,bt_ui_fts_act_proc_cb,BTBAR_CANCEL);
    }
}
/*******************************************************************************
**
** Function         btui_fts_event_hdlr
**
** Description      FTS main menu event handling function.
**                  
**
** Returns          void
**
*******************************************************************************/
void bt_ui_bta_fts_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_ops_evt_hdlr %d", p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
        case BTUI_MMI_FTS_ACCESS:
            bt_ui_cb.ui_state = UI_SENDING_OP_FILE;

#ifndef AEE_SIMULATOR
            IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_ON);
#endif
            break;

        case BTUI_MMI_FTS_CLOSE:
        case BTUI_MMI_FTS_CMPL:
            bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
            HT_MessageBoxTimeout_ex(IDS_DONE, BTUI_TIMEOUT_MSG_PERIOD);

#ifndef AEE_SIMULATOR
            IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif
            break;

        case BTUI_MMI_FTS_PROGRESS:
            if(TRUE == bt_ui_get_suspend() && bt_ui_cb.b_opp_show_proc)
            {
                bt_ui_process_suspend_event(p_msg,bt_ui_fts_act_proc);
            }
            else
            {
                bt_ui_fts_act_proc (p_msg);
            }
            break;
        default:
            break;
    }
}

/*******************************************************************************
**
** Function         btui_menu_fts_main
**
** Description      FTS main menu function.
**                  
**
** Returns          void
**
*******************************************************************************/
void btui_menu_fts_main(void)
{

    tHT_MENU_ITEM * p_menu = bt_ui_get_menu_item_buffer(NULL);
    UINT32 handle;


    p_menu[0].p_item_name = "Enable Server";
    p_menu[0].item_data = 1;
    p_menu[0].item_index = 1;

    p_menu[1].p_item_name = "Disable Server";
    p_menu[1].item_data = 2;
    p_menu[1].item_index = 2;

    handle = bt_ui_create_menu(p_menu, 2,btui_act_fts_main, L"Visibility" ,BTBAR_SELECT_BACK,FALSE,0);
}

/*******************************************************************************
**
** Function         btui_act_fts_main
**
** Description      FTS main menu action function.
**                  
**
** Returns          void
**
*******************************************************************************/
static void btui_act_fts_main(UINT32 handle, UINT32 key)
{
    //static BOOLEAN enabled = TRUE;

    tHT_MENU_ITEM p_item;

    if(key == HT_KEY_SOFT2)
    {
        bt_ui_close_object(handle);
        return;

    }
    btui_fts_cb = bcmapp_get_fts_cb();
    if(key == HT_KEY_SELECT ||key == HT_KEY_SOFT1 )
    {

        HT_GetMenuSelectedItem(handle,&p_item);

        switch (p_item.item_data)
        {
 
        case 1:
            if (!btui_fts_cb->b_fts_enable)
            {
                bt_ui_fts_init();
                HT_MessageBoxTimeout(L"Server  Enabled...", BTUI_TIMEOUT_MSG_PERIOD);
                //enabled = TRUE;
            }
            else
            {
                HT_MessageBoxTimeout(L"Server is Already Enabled...", BTUI_TIMEOUT_MSG_PERIOD);
            }



            break;
        case 2:
            if (btui_fts_cb->b_fts_enable)
            {
                bcmapp_fts_disable();
                HT_MessageBoxTimeout(L"Server Disabled...", BTUI_TIMEOUT_MSG_PERIOD);
                //enabled = FALSE;

            }
            else
                HT_MessageBoxTimeout(L"Server is Already Disabled...", BTUI_TIMEOUT_MSG_PERIOD);

            break;

        default:
            break;
        }
    }


}



#endif


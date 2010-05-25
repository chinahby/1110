/****************************************************************************
**                                                                           
**  Name:          btui_ops.c                                           
**                 
**  Description:   Contains btui functions for object push server
**                 
**                                                                           
**  Copyright (c) 2002, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)

#include "gki.h"
#include "bta_api.h"
#include "bta_op_api.h"
#include "bta_fs_co.h"
#include "bd.h"
#include "bt_ui.h"
#include "bt_ui_int.h"
#include "bcmapp_dm.h"
#include "bcmapp_ops.h"
#include "bt_ui_ht_common.h"
#include "Contapp.h"
/* BTUI OP server main control block */
static tBTUI_OPS_CB *btui_ops_cb = NULL;
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
static tBTUI_OPS_ACCESS *bt_ui_ops_access = NULL;
#endif
/*****************************************************************************
**  Constants
*****************************************************************************/
static const char * const bta_ops_fmt_2_string[] = 
{
    "Other",
    "vCard 2.1",
    "vCard 3.0",
    "vCal 1.0",
    "vCal 2.0",
    "vNote",
    "vMessage"
};

/*******************************************************************************
**
** Function         btui_ops_init
**
** Description      Initialises OP server
**                  
**
** Returns          void
*******************************************************************************/
void bt_ui_ops_init(void)
{
    btui_ops_cb = bcmapp_get_ops_cb();
    bt_ui_cb.recv_vcard_not_ack = FALSE;
    if(bt_ui_opp_test_file_addr(BT_OPS_PUSH_ADDR))
    {
        bcmapp_ops_init();
    }
}
#if 0
static void bt_ui_ops_act_object_cb(UINT32 handle, UINT32 key)//received file end ,return idle?
{
    BCM_FUN_STAR("bt_ui_ops_act_object_cb %x %x %x",handle,key,bt_ui_cb.after_resume_menu);
    if( HT_KEY_SOFT2 == key)
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
    }
}
#endif
static void bt_ui_ops_act_object(tBTUI_BTA_MSG *p_msg)
{
    UINT8 acc_index;
    AECHAR buf[65] = {0};
    AECHAR *msg_w_str;
    int msg_w_len = 0;
    AECHAR bufserver[30] = {0};
    AECHAR A_msg_str[100];
    BCM_FUN_STAR("bt_ui_ops_act_object %d", p_msg->ops_object.format,0,0);
    acc_index = (p_msg->ops_object.format != BTA_OP_OTHER_FMT) ? p_msg->ops_object.format : 0;
    bt_ui_enable_backlight();
    //sprintf (msg_str, "Received Object %s, type %s", p_msg->ops_object.name,
    //         bta_ops_fmt_2_string[acc_index]);
    //if(bt_ui_cb.after_resume_menu)/*back to bluetooth IDLE*/
    //{
    //    bt_ui_cb.after_resume_menu();
    //    bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    //}

    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_OPS_RECEIVE_FILE, buf, sizeof(buf)) ;
    msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
    UTF8TOWSTR((byte *)p_msg->ops_object.name,100,A_msg_str,sizeof(A_msg_str));
    STRTOWSTR( bta_ops_fmt_2_string[acc_index],bufserver,sizeof(bufserver));
    WSPRINTF(msg_w_str, msg_w_len*2, buf, A_msg_str,bufserver);

    HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
    //bt_ui_message_box_timeout(msg_w_str, bt_ui_ops_act_object_cb, BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
#ifndef AEE_SIMULATOR
    if(p_msg->ops_object.format != BTA_OP_OTHER_FMT)
    {
        char p_path[BTUI_MAX_PATH_LENGTH] ;
        sprintf (p_path, "%s%s%s",bt_ui_cfg.root_path,BT_OPS_PUSH_ADDR,p_msg->ops_object.name);
        ISHELL_SendEvent(bt_ui_get_shell(), AEECLSID_APP_CONTACT, EVT_NEWMSGIN, 0, (uint32)p_path);
    }
#endif
    //if(bt_ui_cb.after_resume_menu)/*back to bluetooth IDLE*/
    //{
    //    bt_ui_cb.after_resume_menu();
    //    bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    //}
    //HT_MessageBoxTimeout(msg_w_str, BTUI_TIMEOUT_MSG_PERIOD);
}
static void bt_ui_ops_act_proc_cb(UINT32 handle, UINT32 key)
{
    BCM_FUN_STAR("bt_ui_ops_act_proc_cb %x %x %x",handle,key,bt_ui_cb.after_resume_menu);
    if( HT_KEY_SOFT2 == key)
    {
        bt_ui_cb.b_opp_show_proc = FALSE;
        bt_ui_close_object(handle);
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = TRUE;
#endif
        if(BT_UI_NULL_HANDLE != bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
    }
}

static void bt_ui_ops_act_proc(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_ops_act_proc %d %d %d",p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,bt_ui_cb.b_opp_show_proc);
    if(bt_ui_cb.b_opp_show_proc && (FALSE == bt_ui_get_suspend()))
    {
        HT_MessageBoxProcess(bt_ui_get_brx_string(IDS_BT_OPP_RECEIVE),p_msg->opp_proc.val.bytes_transferred,p_msg->opp_proc.val.obj_size,bt_ui_ops_act_proc_cb,BTBAR_CANCEL);
    }
}
static void bt_ui_ops_set_recv_vcard_not_ack()
{
    bt_ui_cb.recv_vcard_not_ack = FALSE;
}
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
static void bt_ui_ops_act_access_cb(UINT32 handle, UINT32 key)
{
    char p_path[BTUI_MAX_PATH_LENGTH] ;
    btui_ops_cb->bytes_transferred = 0;
    BCM_FUN_STAR("bt_ui_ops_act_access_cb %x %x",handle,key,0);
    bt_ui_close_object(handle);
    if(bt_ui_ops_access == NULL)
    {
        return;
    }
    if( HT_KEY_SOFT2 == key)
    {
        btui_ops_cb->access_flag = BTA_OP_ACCESS_FORBID;
        if(bt_ui_cb.after_resume_menu)
        {
            bt_ui_cb.after_resume_menu();
            bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
        }
    }
    else if((key == HT_KEY_SOFT1) || (key == HT_KEY_SELECT))
    {
        btui_ops_cb->access_flag = BTA_OP_ACCESS_ALLOW;
        bt_ui_cb.b_opp_show_proc = TRUE;
        //if(bt_ui_cb.after_resume_menu)
        //{
            //bt_ui_cb.after_resume_menu();
            //bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
            //bt_ui_menu_idle();//not close app,show bt idle,for show process
        //}
        bt_ui_cb.ui_state = UI_SENDING_OP_FILE;
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_ON);
#endif
    }

    BCM_MSG_MED("BTA_OPS_ACCESS_EVT %d %d %d", bt_ui_ops_access->oper,bt_ui_ops_access->format,btui_ops_cb->access_flag);
    if (btui_ops_cb->access_flag == BTA_OP_ACCESS_ALLOW)
    {
        //strcat(msg_str, "GRANTED for:");
        if (bt_ui_ops_access->oper == BTA_OP_OPER_PUSH)
        {
            if(bt_ui_ops_access->format == BTA_OP_OTHER_FMT)
            {
                byte save_path = BT_SAVE_TO_NONE;
                save_path = bcmapp_dm_get_stroe_path(bt_ui_ops_access->size,p_path,bt_ui_ops_access->p_name);
                if(BT_SAVE_TO_NONE != save_path)
                {
                    //if(BT_SAVE_TO_CARD == save_path)
                    //{
                    //    Appscommon_bluetooth_used_sd(1);
                    //}
                    STRCAT(p_path,bt_ui_ops_access->p_name);
                }
                else
                {
                    /**/
                    BTA_OpsAccessRsp(bt_ui_ops_access->oper, BTA_OP_ACCESS_FORBID, p_path);
                    HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
                    return;
                }
            }
            else
            {
                sprintf (p_path, "%s%s%s",bt_ui_cfg.root_path,BT_OPS_PUSH_ADDR,bt_ui_ops_access->p_name);
                bt_ui_cb.recv_vcard_not_ack = TRUE;
            }
        }
        else
        {
                sprintf (p_path, "%s%sdefault_ops.vcf",bt_ui_cfg.root_path,BT_OPS_PULL_ADDR);
        }
    }
    //{
    //    if (bt_ui_ops_access->oper == BTA_OP_OPER_PUSH)
    //    {
    //            sprintf (p_path, "%s%s%s",bt_ui_cfg.root_path,BT_OPS_PUSH_ADDR,bt_ui_ops_access->p_name);
    //    }
    //    else
    //    {
    //            sprintf (p_path, "%s%sdefault_ops.vcf",bt_ui_cfg.root_path,BT_OPS_PULL_ADDR);
    //    }
    //}
    BTA_OpsAccessRsp(bt_ui_ops_access->oper, btui_ops_cb->access_flag, p_path);
    BTUI_FreeIf(bt_ui_ops_access);
    
}
static void bt_ui_ops_act_access(tBTUI_BTA_MSG *p_msg)
{
    boolean is_full = TRUE;
    ISHELL_CancelTimer(bt_ui_get_shell(), (PFNNOTIFY)bt_ui_ops_set_recv_vcard_not_ack, NULL);
    //bt_ui_ops_access = (tBTUI_OPS_ACCESS*)BTUI_Malloc(sizeof(tBTUI_OPS_ACCESS));
    if(bt_ui_ops_access!= NULL)
    {
        BCM_FUN_STAR("bt_ui_ops_act_access %x %x %x", bt_ui_ops_access,bt_ui_cb.recv_vcard_not_ack,bt_ui_ops_access->format );
        if(bt_ui_ops_access->format != BTA_OP_OTHER_FMT)//is vcard
        {
            IContApp * pContApp = NULL;
            boolean is_full = TRUE;
            if (SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(),
                    AEECLSID_APP_CONTACT,
                    (void**)&pContApp))
            {
                if (pContApp != NULL)
                {
                    ICONTAPP_CheckCapacity(pContApp,&is_full);
                    ICONTAPP_Release(pContApp); 
                }
            }
        }
        else
        {
            is_full = FALSE;
        }

        if(is_full)
        {
            char p_path[10] ;
            BTA_OpsAccessRsp(bt_ui_ops_access->oper, BTA_OP_ACCESS_FORBID, p_path);
            BTUI_FreeIf(bt_ui_ops_access);
        }
        else
        {
            if(bt_ui_cb.recv_vcard_not_ack && bt_ui_ops_access->format != BTA_OP_OTHER_FMT)
            {
                char p_path[BTUI_MAX_PATH_LENGTH] ;
                sprintf (p_path, "%s%s%s",bt_ui_cfg.root_path,BT_OPS_PUSH_ADDR,bt_ui_ops_access->p_name);
                BTA_OpsAccessRsp(bt_ui_ops_access->oper, BTA_OP_ACCESS_ALLOW, p_path);
                BTUI_FreeIf(bt_ui_ops_access);
            }
            else
            {
                UINT8 acc_index;
                AECHAR buf[65] = {0};
                AECHAR *msg_w_str;
                int msg_w_len = 0;
                AECHAR bufserver[30] = {0};
                AECHAR A_msg_str[100];
                
                //MEMCPY((void*)bt_ui_ops_access,(void*)&p_msg->ops_access,sizeof(tBTUI_OPS_ACCESS));
                bt_ui_enable_backlight();
                ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, IDS_OPS_RECEIVE_FILE, buf, sizeof(buf)) ;
                msg_w_str = bt_ui_get_comm_wbuffer(&msg_w_len);
                UTF8TOWSTR((byte *)bt_ui_ops_access->p_name,BTUI_MAX_PATH_LENGTH,A_msg_str,sizeof(A_msg_str));
                acc_index = (bt_ui_ops_access->format != BTA_OP_OTHER_FMT) ? bt_ui_ops_access->format : 0;
                STRTOWSTR( bta_ops_fmt_2_string[acc_index],bufserver,sizeof(bufserver));
                WSPRINTF(msg_w_str, msg_w_len*2, buf, A_msg_str,bufserver);
                bt_ui_set_pop_msg_type(MESSAGE_CONFIRM);
                bt_ui_message_box(msg_w_str, bt_ui_ops_act_access_cb,BTBAR_OK_CANCEL);
            }
        }
    }
}
#endif

void bt_ui_bta_ops_evt_hdlr(tBTUI_BTA_MSG *p_msg)
{
    BCM_FUN_STAR("bt_ui_bta_ops_evt_hdlr %d", p_msg->hdr.event,0,0);
    switch(p_msg->hdr.event)
    {
    case BTUI_MMI_OPS_OBJECT:/* Object has been received. */
        if(TRUE == bt_ui_get_suspend())
        {
            bt_ui_process_suspend_event(p_msg,bt_ui_ops_act_object);
        }
        else
        {
            bt_ui_ops_act_object (p_msg);
        }
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;

#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif
        break;

    case BTUI_MMI_OPS_CLOSE:/* Connection to peer closed. */
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
#ifdef FEATURE_KEYGUARD
        if(bt_ui_cb.b_auto_keyguard)
        {
            bt_ui_cb.b_auto_keyguard = FALSE;
            OEMKeyguard_SetState(TRUE);
        }
#endif
        if(bt_ui_cb.recv_vcard_not_ack)
        {
            ISHELL_SetTimer(bt_ui_get_shell(), 5000, (PFNNOTIFY)bt_ui_ops_set_recv_vcard_not_ack, (void *)NULL); 
        }
        if(p_msg->close.status == BTA_OPC_FAIL)
        {
            HT_MessageBoxTimeout_ex(IDS_FAIL, BTUI_TIMEOUT_MSG_PERIOD);
        }
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
#endif
        break;
        
    case BTUI_MMI_OPS_PROC:/* Object being sent or received. */
        if(TRUE == bt_ui_get_suspend() && bt_ui_cb.b_opp_show_proc)
        {
            bt_ui_process_suspend_event(p_msg,bt_ui_ops_act_proc);
        }
        else
        {
            bt_ui_ops_act_proc (p_msg);
        }
        //bt_ui_ops_act_proc(p_msg);
        break;

    case BTUI_MMI_OPS_ACCESS:/* Request for access to push or pull object */
        BCM_FUN_STAR("BTUI_MMI_OPS_ACCESS", 0,0,0);
#ifdef FEATURE_BT_SHOW_OPP_BITRATE
        HtWinStack_Draw_bitrate(0,TRUE);
#endif
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
        if(bt_ui_ops_access== NULL)
        {
            bt_ui_ops_access = (tBTUI_OPS_ACCESS*)BTUI_Malloc(sizeof(tBTUI_OPS_ACCESS));
        }
        if(bt_ui_ops_access!= NULL)
        {
            MEMCPY((void*)bt_ui_ops_access,(void*)&p_msg->ops_access,sizeof(tBTUI_OPS_ACCESS));
        }
        else
        {
            return;
        }
        if(TRUE == bt_ui_get_suspend())
        {
            bt_ui_process_suspend_event(p_msg,bt_ui_ops_act_access);
        }
        else
        {
            bt_ui_ops_act_access (p_msg);
        }
        //bt_ui_ops_act_access(p_msg);
#else
#ifndef AEE_SIMULATOR
        IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_ON);
#endif

#endif
        break;

    default:
        break;
    }

}

void bt_ui_ops_disable(void)
{
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
    BTUI_FreeIf(bt_ui_ops_access);
#endif
    bcmapp_ops_disable();
}
#endif


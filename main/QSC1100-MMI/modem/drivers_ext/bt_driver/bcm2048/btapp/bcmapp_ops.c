/****************************************************************************
**                                                                           
**  Name:          bcmapp_ops.c                                           
**                 
**  Description:   Contains application functions for object push server
**                 
**                                                                           
**  Copyright (c) 2002-2006, Broadcom Corp., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"
#include "bt_ui_ht_trace.h"


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
#include "Bt_ui_ht_common.h"
/* BTUI OP server main control block */
static tBTUI_OPS_CB bt_ops_cb;

/*****************************************************************************
**  Constants
*****************************************************************************/
//static const char * const bta_ops_fmt_2_string[] = 
//{
//    "Other",
//    "vCard 2.1",
//    "vCard 3.0",
//    "vCal 1.0",
//    "vCal 2.0",
//    "vNote",
//    "vMessage"
//};

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
void bcmapp_ops_cback(tBTA_OPS_EVT event, tBTA_OPS *p_data);

#define BTUI_OPS_FORMATS            (BTA_OP_VCARD21_MASK | BTA_OP_VCARD30_MASK | \
                                     BTA_OP_VCAL_MASK | BTA_OP_ICAL_MASK | \
                                     BTA_OP_VNOTE_MASK | BTA_OP_VMSG_MASK | \
                                     BTA_OP_ANY_MASK )
tBTUI_OPS_CB * bcmapp_get_ops_cb(void)
{
    return &bt_ops_cb;
}
/*******************************************************************************
**
** Function         bcmapp_ops_init
**
** Description      Initialises OP server
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ops_init(void)
{
    if (bt_ui_cfg.ops_included)
        BTA_OpsEnable(bt_ui_cfg.ops_security, BTUI_OPS_FORMATS,
                      bt_ui_cfg.ops_service_name, bcmapp_ops_cback, 0);

    /* Sample app always allows object push operations unless changed in test menu */
    bt_ops_cb.access_flag = BTA_OP_ACCESS_ALLOW;
}

/*******************************************************************************
**
** Function         bta_ops_cback
**
** Description      Callback from BTA OPS
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_ops_cback(tBTA_OPS_EVT event, tBTA_OPS *p_data)
{
    UINT8 acc_index;
    //char *p_path;
    //char  msg_str[132];
    tBTUI_BTA_MSG * p_msg;
    BCM_MSG_MED("bcmapp_ops_cback %d show_proc %d", event,bt_ui_cb.b_opp_show_proc,0);
    switch (event)
    {
    case BTA_OPS_PROGRESS_EVT:
        bt_ops_cb.bytes_transferred += p_data->prog.bytes;
        bt_ops_cb.obj_size = p_data->prog.obj_size;
        if ((p_msg = bt_ui_get_msg()) != NULL && bt_ui_cb.b_opp_show_proc)
        {
            if (p_data->prog.obj_size != BTA_FS_LEN_UNKNOWN)
            {
                BCM_MSG_MED("Object Transfer Server PROGRESS (%d of %d total)...",
                                  bt_ops_cb.bytes_transferred, p_data->prog.obj_size,0);
                p_msg->opp_proc.val.obj_size = p_data->prog.obj_size;
            }
            else
            {
                BCM_MSG_MED("Object Transfer Server PROGRESS (%d bytes total)...",
                                  bt_ops_cb.bytes_transferred,0,0);
                p_msg->opp_proc.val.obj_size = 0;
            }
            p_msg->opp_proc.val.bytes_transferred = bt_ops_cb.bytes_transferred;
            p_msg->ops_object.hdr.event = BTUI_MMI_OPS_PROC;
            bt_ui_send_msg(BTUI_OPS_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPS_OBJECT_EVT:
        acc_index = (p_data->object.format != BTA_OP_OTHER_FMT) ? p_data->object.format : 0;
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
        //BT_UI_APPL_TRACE_EVENT2("Object Received: Name [%s], Type [%s]",
        //   p_data->object.p_name, bta_ops_fmt_2_string[acc_index]);

        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_OPS_OBJECT;
            p_msg->ops_object.format = p_data->object.format;
            strncpy(p_msg->ops_object.name, p_data->object.p_name, 100/*move*/);
            bt_ui_send_msg(BTUI_OPS_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPS_OPEN_EVT:
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = FALSE;
#endif
        BCM_MSG_MED("OPS Connection Opened with %02x%02x%02x",
            p_data->bd_addr[0], p_data->bd_addr[1], p_data->bd_addr[2]);
        BCM_MSG_MED("OPS Connection Opened with %02x%02x%02x",
            p_data->bd_addr[3], p_data->bd_addr[4], p_data->bd_addr[5]);
        break;

    case BTA_OPS_CLOSE_EVT:
        //if(1 == Appscommon_bluetooth_used_sd(-1))
        //{
        //#ifdef FEATURE_SUPPORT_VC0848
        //    MediaGalleryApp_StopSDCard();
        //#endif
        //    Appscommon_bluetooth_used_sd(0);
        //}
        bt_ui_cb.ui_state = UI_SENDED_OP_FILE;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            BCM_MSG_MED("BTA_OPS_CLOSE_EVT (%d of %d total)...",
                                  bt_ops_cb.bytes_transferred, bt_ops_cb.obj_size,0);
            p_msg->hdr.event = BTUI_MMI_OPS_CLOSE;
            if(bt_ops_cb.bytes_transferred == bt_ops_cb.obj_size)
            {
                p_msg->close.status =BTA_OPC_OK;
            }
            else
            {
                p_msg->close.status =BTA_OPC_FAIL;
            }
            bt_ui_send_msg(BTUI_OPS_MSG_TYPE, p_msg);
        }
        //BCM_MSG_MED("OPS Connection Closed...",0,0,0);
        break;

    case BTA_OPS_ACCESS_EVT:
    {
        char p_path[BTUI_MAX_PATH_LENGTH] ;
#ifndef BTUI_OPS_NEED_RECEIVE_ACK
        //char  msg_str[132];
        bt_ops_cb.bytes_transferred = 0;
        BCM_MSG_MED("BTA_OPS_ACCESS_EVT %d %d %d", p_data->access.oper,p_data->access.format,bt_ops_cb.access_flag);
        //if (p_data->access.oper == BTA_OP_OPER_PUSH)
        //    strcpy(msg_str, "Object Transfer: PUSH Access request ");
        //else if (p_data->access.oper == BTA_OP_OPER_PULL)
        //    strcpy(msg_str, "Object Transfer: PULL Access request ");

        if (bt_ops_cb.access_flag == BTA_OP_ACCESS_ALLOW)
        {
            //strcat(msg_str, "GRANTED for:");
            if (p_data->access.oper == BTA_OP_OPER_PUSH)
            {
                if(p_data->access.format == BTA_OP_OTHER_FMT)
                {
                    byte save_path = BT_SAVE_TO_NONE;
                    save_path = bcmapp_dm_get_stroe_path(p_data->access.size,p_path,p_data->access.p_name);
                    if(BT_SAVE_TO_NONE != save_path)
                    {
                        //if(BT_SAVE_TO_CARD == save_path)
                        //{
                        //    Appscommon_bluetooth_used_sd(1);
                        //}
                        strcat(p_path,p_data->access.p_name);
                    }
                    else
                    {
                        /**/
                        BTA_OpsAccessRsp(p_data->access.oper, BTA_OP_ACCESS_FORBID, p_path);
                        return;
                    }
                }
                else
                {
                    sprintf (p_path, "%s%s%s",bt_ui_cfg.root_path,BT_OPS_PUSH_ADDR,p_data->access.p_name);
                }
            }
            else
            {
                    sprintf (p_path, "%s%sdefault_ops.vcf",bt_ui_cfg.root_path,BT_OPS_PULL_ADDR);
            }
        }
        else
        {
            //strcat(msg_str, "DENIED for:");
        }
        acc_index = (p_data->access.format != BTA_OP_OTHER_FMT) ? p_data->access.format : 0;

        //BT_UI_APPL_TRACE_EVENT0(msg_str);

        //if (p_data->access.oper == BTA_OP_OPER_PUSH)
        //{
        //    if (p_data->access.p_type)
        //    {
                //BT_UI_APPL_TRACE_EVENT4("   Obj [%s], Type [%s (%s)], Size [%d]",
                //                    p_data->access.p_name, p_data->access.p_type,
                //                    bta_ops_fmt_2_string[acc_index],
                //                    p_data->access.size);
        //    }
        //    else
        //    {
                //BT_UI_APPL_TRACE_EVENT3("   Obj [%s], Type [%s], Size [%d]",
                //                    p_data->access.p_name, bta_ops_fmt_2_string[acc_index],
                //                    p_data->access.size);
        //    }
        //}
        //else
        //{
            //BT_UI_APPL_TRACE_EVENT1("   Type [%s]", bta_ops_fmt_2_string[acc_index]);
        //}

        //BT_UI_APPL_TRACE_EVENT2("   Device [%s]%s", p_data->access.dev_name,
        //                  bt_ui_addr_str(p_data->access.bd_addr));
        if(TRUE == bcmapp_dm_get_incall_state(TRUE))
        {
            BCM_MSG_MED("BTA_OPS_ACCESS_EVT fail because incall state",0,0,0);
            BTA_OpsAccessRsp(p_data->access.oper, BTA_OP_ACCESS_FORBID, p_path);
            return;
        }
        bt_ui_cb.b_opp_show_proc = TRUE;/*because ui can not receive this msg,so set flag in here*/
        BTA_OpsAccessRsp(p_data->access.oper, bt_ops_cb.access_flag, p_path);
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_OPS_ACCESS;
            bt_ui_send_msg(BTUI_OPS_MSG_TYPE, p_msg);
        }
#else
        //BCM_MSG_MED("BTA_OPS_ACCESS_EVT fail because incall state %x %x",bt_ui_cb.ui_state,bt_ui_cb.ui_next_state,0);
        if(TRUE == bcmapp_dm_get_incall_state(TRUE) ||bt_ui_cb.ui_state == UI_SEARCH_NEW_DEVICE || bt_ui_cb.ui_state == UI_SENDING_OP_FILE)
        {
            BCM_MSG_MED("BTA_OPS_ACCESS_EVT fail because incall state %x %x",bt_ui_cb.ui_state,bt_ui_cb.ui_next_state,0);
            BTA_OpsAccessRsp(p_data->access.oper, BTA_OP_ACCESS_FORBID, p_path);
            return;
        }
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_OPS_ACCESS;
            
            strncpy(p_msg->ops_access.p_name, p_data->access.p_name,BTUI_MAX_PATH_LENGTH);
            strncpy(p_msg->ops_access.p_type, p_data->access.p_type,19);
            p_msg->ops_access.size = p_data->access.size;
            memcpy((void*)p_msg->ops_access.dev_name,(void*)p_data->access.dev_name,sizeof(tBTM_BD_NAME));
            bdcpy(p_msg->ops_access.bd_addr, p_data->access.bd_addr);
            p_msg->ops_access.oper = p_data->access.oper;
            p_msg->ops_access.format = p_data->access.format;
            bt_ui_send_msg(BTUI_OPS_MSG_TYPE, p_msg);
        }
#endif
        break;
    }
    default:
        break;
    }
}

void bcmapp_ops_disable(void)
{
    BTA_OpsDisable();
}
#endif


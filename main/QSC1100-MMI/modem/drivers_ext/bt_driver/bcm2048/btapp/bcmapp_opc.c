/*****************************************************************************
**
**  Name:           bcmapp_opc.c
**
**  Description:     Contains application functions for OP Client
**
**  Copyright (c) 2003-2005, Widcomm Inc., All Rights Reserved.
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
#include "Bt_ui_ht_common.h"

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/

static void bcmapp_opc_cback(tBTA_OPC_EVT event, tBTA_OPC *p_data);
static void bcmapp_opc_platform_get_default_rcv_path(char *p_rcv_path);
static void bcmapp_opc_platform_get_default_card(char *p_send);

/* Maximum path length supported by MMI */
//#ifndef BTUI_PATH_LEN
//#define BTUI_PATH_LEN           260
//#endif
#define BTUI_OPC_MEM_LEN            8224
#define BTUI_OPC_NUM_PROP           8
#ifdef BT_UI_OPC_TEST_CARD_MEMU
static UINT8   bt_ui_opc_test_mem[BTUI_OPC_MEM_LEN];
#endif
/* BTUI OPC main control block */
static tBTUI_OPC_CB bt_opc_cb;

tBTUI_OPC_CB * bcmapp_ag_get_opc_cb(void)
{
    return &bt_opc_cb;
}
/*******************************************************************************
**
** Function         bt_ui_opc_init
**
** Description     Initializes BTUI & BTA OPC 
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_opc_init(void)
{
    BCM_FUN_STAR("bcmapp_opc_init",0,0,0);
    memset(&bt_opc_cb, 0, sizeof(tBTUI_OPC_CB));

    bt_opc_cb.single_op = bt_ui_cfg.opc_single_op;
    BTA_OpcEnable(bt_ui_cfg.opc_security, bcmapp_opc_cback, bt_opc_cb.single_op, 0);

}
//extern void bt_ui_opc_platform_get_default_card(char *p_send);
//extern void bt_ui_opc_platform_get_default_rcv_path(char *p_rcv_path);

/*******************************************************************************
**
** Function         bcmapp_opc_pull_card
**
** Description      Pulls default card from peer device
**                  
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_opc_pull_card(void)
{
    char p_rcv_path[BTUI_PATH_LEN];
    BCM_FUN_STAR("bcmapp_opc_pull_card %d %x",bt_opc_cb.is_enabled ,bt_ui_cb.p_selected_rem_device,0);
    if (bt_opc_cb.is_enabled)
    {
        bcmapp_opc_platform_get_default_rcv_path(p_rcv_path);
        BTA_OpcPullCard(bt_ui_cb.p_selected_rem_device->bd_addr, p_rcv_path);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
**
** Function         bcmapp_opc_exch_card
**
** Description      Initiates card exchange
**                  
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_opc_exch_card(void)
{
    char    p_send[BTUI_PATH_LEN];
    char    p_rcv_path[BTUI_PATH_LEN];
    BCM_FUN_STAR("bcmapp_opc_exch_card %d",bt_opc_cb.is_enabled,0,0);
    if (bt_opc_cb.is_enabled )
    {
        bcmapp_opc_platform_get_default_card(p_send);
        bcmapp_opc_platform_get_default_rcv_path(p_rcv_path);
        BTA_OpcExchCard(bt_ui_cb.p_selected_rem_device->bd_addr, p_send, p_rcv_path);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*******************************************************************************
**
** Function         bcmapp_opc_close
**
** Description      Closes connection
**                  
**
** Returns          void
*******************************************************************************/
void bcmapp_opc_close(void)
{
    BCM_FUN_STAR("bcmapp_opc_close",0,0,0);
    BTA_OpcClose();
}

/*******************************************************************************
**
** Function         bcmapp_opc_act_send_file
**
** Description      Sends the file 
**                  
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_opc_act_send_file (char * name)
{
    char    *p_name = 0;
    char    *p_path;
    tBTA_OP_FMT format ;
    BCM_FUN_STAR("bcmapp_opc_act_send_file",0,0,0);
    p_path = name;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
    int i;
    UINT16  len;
    UINT8   *p_data;
    tBTA_OP_PROP prop_array[BTUI_OPC_NUM_PROP];
    UINT8  num_prop = BTUI_OPC_NUM_PROP;
    tBTA_OP_VCAL  vcal;
    tBTA_OP_STATUS status;
    
    memset(prop_array, 0, BTUI_OPC_NUM_PROP*sizeof(tBTA_OP_PROP));


    if(bt_ui_cfg.test_included)
    {
        for(i=0; i<BTUI_OPC_NUM_PROP; i++)
        {
            prop_array[i].len = BTUI_OPC_MEM_LEN/BTUI_OPC_NUM_PROP;
            prop_array[i].p_data = &bt_ui_opc_test_mem[prop_array[i].len*i];
        }
    }
#endif
    if(bt_opc_cb.is_enabled && p_path)
    {
        if(strstr(p_name, ".vcf"))
        {
            format = BTA_OP_VCARD21_FMT;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
            if(bt_ui_cfg.test_included)
            {
                status = BTA_OpParseCard((tBTA_OP_PROP*)prop_array, &num_prop, p_data, len);
                APPL_TRACE_DEBUG1(" opc parse status %d ",status);
                len = BTUI_OPC_MEM_LEN;
                status = BTA_OpBuildCard(p_data, &len, (tBTA_OP_PROP*)prop_array, num_prop);
                APPL_TRACE_DEBUG1(" opc build status %d ",status);
            }
#endif
        }
        else if(strstr(p_name, ".vcd"))
        {
            format = BTA_OP_VCARD30_FMT;
        }
        else if(strstr(p_name, ".vcs"))
        {
            format = BTA_OP_VCAL_FMT;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
            if(bt_ui_cfg.test_included)
            {
                status = BTA_OpParseCal((tBTA_OP_PROP*)prop_array, &num_prop, p_data, len, &vcal);
                APPL_TRACE_DEBUG1(" opc parse status %d ",status);
                len = BTUI_OPC_MEM_LEN;
                status = BTA_OpBuildCal(p_data, &len, (tBTA_OP_PROP*)prop_array, num_prop, vcal);
                APPL_TRACE_DEBUG1(" opc build status %d ",status);
            }
#endif
        }
        else if(strstr(p_name, ".ics"))
        {
            format = BTA_OP_ICAL_FMT;
        }
        else if(strstr(p_name, ".vnt"))
        {
            format = BTA_OP_VNOTE_FMT;
#ifdef BT_UI_OPC_TEST_CARD_MEMU
            if(bt_ui_cfg.test_included)
            {
                status = BTA_OpParseNote((tBTA_OP_PROP*)prop_array, &num_prop, p_data, len);
                APPL_TRACE_DEBUG1(" opc parse status %d ",status);
                len = BTUI_OPC_MEM_LEN;
                status = BTA_OpBuildNote(p_data, &len, (tBTA_OP_PROP*)prop_array, num_prop);
                APPL_TRACE_DEBUG1(" opc build status %d ",status);
            }
#endif
        }
        else if(strstr(p_name, ".vmg"))
            format = BTA_OP_VMSG_FMT;
        else 
            format = BTA_OP_OTHER_FMT;
        BCM_FUN_STAR("bcmapp_opc_act_send_file %d",format,0,0);
        BTA_OpcPush(bt_ui_cb.p_selected_rem_device->bd_addr, format, p_path);

        return TRUE;

    }
    else
    {

        return FALSE;
    }

}

/*******************************************************************************
**
** Function         bt_opc_cback
**
** Description      Call back from BTA opc
**                  
**
** Returns          void
*******************************************************************************/
static void bcmapp_opc_cback(tBTA_OPC_EVT event, tBTA_OPC *p_data)
{
    tBTUI_BTA_MSG * p_msg;
    BCM_FUN_STAR("bcmapp_opc_cback %d",event,0,0);
    switch (event)
    {
    case BTA_OPC_ENABLE_EVT:
        bt_opc_cb.is_enabled = TRUE;
        bt_opc_cb.p_connected_rem_device = NULL;
        //APPL_TRACE_EVENT0("BTUI Object Transfer Client ENABLED");
        break;

    case BTA_OPC_OPEN_EVT:
        //APPL_TRACE_EVENT0("BTUI Object Transfer Client CONNECTED");
        bt_opc_cb.bytes_transferred = 0;
        bt_opc_cb.operation = 0;  /* No operation active yet */
        bt_opc_cb.is_connected = TRUE;
        bt_opc_cb.p_connected_rem_device = bt_ui_cb.p_selected_rem_device;
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = FALSE;
#endif
        //if (strcmp(bt_ui_cb.opp_send_file_addr, AEEFS_CARD0_DIR/*MG_MASSCARD_ROOTDIR*/) == 0)
        //{
        //    Appscommon_bluetooth_used_sd(1);
        //}
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_OPC_OPEN;
            bt_ui_send_msg(BTUI_OPC_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPC_PROGRESS_EVT:
        if (bt_opc_cb.operation != p_data->prog.operation)
        {
            bt_opc_cb.operation = p_data->prog.operation;
            bt_opc_cb.bytes_transferred = 0;
        }

        bt_opc_cb.bytes_transferred += p_data->prog.bytes;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            if (p_data->prog.obj_size != BTA_FS_LEN_UNKNOWN)
            {
                BCM_MSG_MED("BTUI Object Transfer Client PROGRESS (%d of %d total)...",
                                  bt_opc_cb.bytes_transferred, p_data->prog.obj_size,0);
                p_msg->opp_proc.val.obj_size = p_data->prog.obj_size;
            }
            else
            {
                BCM_MSG_MED("BTUI Object Transfer Client PROGRESS (%d bytes total)...",
                                  bt_opc_cb.bytes_transferred,0,0);
                p_msg->opp_proc.val.obj_size = 0;
            }
            p_msg->opp_proc.val.bytes_transferred = bt_opc_cb.bytes_transferred;
            p_msg->ops_object.hdr.event = BTUI_MMI_OPC_PROC;
            bt_ui_send_msg(BTUI_OPC_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPC_OBJECT_EVT:
        bt_opc_cb.bytes_transferred = 0;
        bt_opc_cb.operation = 0;
        BCM_MSG_MED("BTA_OPC_OBJECT_EVT %d",p_data->object.status,0,0);
        switch (p_data->object.status)
        {
        case BTA_OPC_OK:
            //APPL_TRACE_EVENT1("BTUI Object Transfer Client: vCard Received [name %s]",
            //                  p_data->object.p_name);
            break;

        case BTA_OPC_NOT_FOUND:
            //APPL_TRACE_EVENT1("BTUI Object Transfer Client: vCard Not Found [name %s]",
            //                  p_data->object.p_name);
            break;

        case BTA_OPC_NO_PERMISSION:
            //APPL_TRACE_EVENT0("BTUI Object Transfer Client: vCard [No Permission]");
            break;

        default:
            //APPL_TRACE_EVENT0("BTUI Object Transfer Client: vCard [Failed]");
            break;
        }

        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->ops_object.hdr.event = BTUI_MMI_OPC_OBJECT;
            p_msg->ops_object.hdr.layer_specific = p_data->object.status;
            if(p_data->object.p_name)
            {
                strcpy(p_msg->ops_object.name,p_data->object.p_name);
            }
            bt_ui_send_msg(BTUI_OPC_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPC_OBJECT_PSHD_EVT:
        bt_opc_cb.bytes_transferred = 0;
        bt_opc_cb.operation = 0;
        BCM_MSG_MED("BTA_OPC_OBJECT_PSHD_EVT %d",p_data->object.status,0,0);
        switch (p_data->object.status)
        {
        case BTA_OPC_OK:
            //APPL_TRACE_EVENT1("BTUI Object Transfer Client: Object Sent [name %s]",
            //                  p_data->object.p_name);
            break;

        case BTA_OPC_NO_PERMISSION:
            //APPL_TRACE_EVENT0("BTUI Object Transfer Client: Object Not Sent [No Permission]");
            break;

        default:
            //APPL_TRACE_EVENT0("BTUI Object Transfer Client: Object Not Sent [Failed]");
            break;
        }

        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->open.hdr.event = BTUI_MMI_OPC_PSHD;
            p_msg->open.status = p_data->object.status;
            bt_ui_send_msg(BTUI_OPC_MSG_TYPE, p_msg);
        }
        break;

    case BTA_OPC_CLOSE_EVT:
        //if(1 == Appscommon_bluetooth_used_sd(-1))
        //{
        //#ifdef FEATURE_SUPPORT_VC0848
        //    MediaGalleryApp_StopSDCard();
        //#endif
        //    Appscommon_bluetooth_used_sd(0);
        //}

        bt_opc_cb.is_connected = FALSE;
        bt_opc_cb.p_connected_rem_device = NULL;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->opc_close.status = p_data->status;
            p_msg->hdr.event = BTUI_MMI_OPC_CLOSE;
            bt_ui_send_msg(BTUI_OPC_MSG_TYPE, p_msg);
        }
        //APPL_TRACE_EVENT1("BTUI Object Transfer Client DISCONNECTED (status %d)", p_data->status);
        break;
    }
}

static void bcmapp_opc_platform_get_default_rcv_path(char *p_rcv_path)
{
    sprintf (p_rcv_path, "%s%s",bt_ui_cfg.root_path,BT_OPC_PULL_ADDR);
    //strcpy(p_rcv_path,"/brew/");
    //sprintf (p_rcv_path, "%s\\test_files\\opc",bt_ui_cfg.root_path);
}

static void bcmapp_opc_platform_get_default_card(char *p_send)
{
    sprintf (p_send, "%s%sdefault_opc.vcf",bt_ui_cfg.root_path,BT_OPC_PUSH_ADDR);
    //strcpy(p_send,"/brew/default_opc.vcf");
    //sprintf (p_send, "%s\\test_files\\opc\\default_opc.vcf",bt_ui_cfg.root_path);
}

void bcmapp_opc_disable(void)
{
    BTA_OpcDisable();
}
#endif


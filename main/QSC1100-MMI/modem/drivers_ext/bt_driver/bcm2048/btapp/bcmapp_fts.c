/****************************************************************************
**                                                                           
**  Name:          bcmapp_fts.c                                           
**                 
**  Description:   Contains  functions for file transfer server
**                 
**                                                                           
**  Copyright (c) 2003-2006, Broadcom Corp., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"

#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)


#ifndef __CYGWIN__
//#include <direct.h>
#endif 

#include "gki.h"
#include "bta_api.h"
#include "bt_ui.h"
#include "bta_fs_api.h"
#include "bta_ft_api.h"
#include "bta_fs_co.h"
#include "bt_ui_int.h"
#include "bcmapp_fts.h"
#include "bt_ui_ht_common.h"
#include "Bcmapp_dm.h"
/* Root name off of the working directory */
//#define FTS_ROOT_FOLDER         "fts/"
//#define FTS_DEFAULT_ROOT_FOLDER "bta_fts_root"

/* Maximum path length supported by MMI */
//#ifndef BTUI_PATH_LEN
//#define BTUI_PATH_LEN           260
//#endif

/* BTUI FT server main control block */
static tBTUI_FTS_CB bt_fts_cb = {0};


/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void bcmapp_fts_cback(tBTA_FTS_EVT event, tBTA_FTS *p_data);

tBTUI_FTS_CB * bcmapp_get_fts_cb(void)
{
    return &bt_fts_cb;
}

/*******************************************************************************
**
** Function         bcmapp_fts_init
**
** Description      FTS initialization function.
**                  
**
** Returns          void
**
*******************************************************************************/
void bcmapp_fts_init(void)
{
    char   p_root_path[BTUI_PATH_LEN];
#if 0
    BOOLEAN start_ok = TRUE;
    BOOLEAN is_dir;
#else
    BOOLEAN start_ok = FALSE;
    //IFileMgr        *m_pIFileMgr = NULL; 
#endif
    /* No menus for FT server
    Keep the previous code as such.
    It can be used later if needed */

    //bt_ui_cfg.p_fts_event_hdlr = NULL;
    //bt_ui_cfg.p_fts_menu = NULL;
    
    /* Sample app always allows PUT file operations unless changed in test menu */
    bt_fts_cb.access_flag = BTA_FT_ACCESS_ALLOW;
    bt_fts_cb.b_fts_enable = FALSE;
    memset(p_root_path, 0, BTUI_PATH_LEN + 1);
    
    /* Use the BTA environment if it exists, otherwise create a default root path */
    //sprintf(p_root_path, "%s%s", AEEFS_ROOT_DIR, FTS_ROOT_FOLDER);
#if 1
    if(bt_ui_cfg.b_sdcard_state)
    {
        bcmapp_dm_change_brewaddr_to_fs(MG_MASSCARD_ROOTDIR,p_root_path);
    }
    else
    {
        bcmapp_dm_change_brewaddr_to_fs(MG_PHONE_ROOTDIR,p_root_path);
    }
    start_ok = TRUE;
#if 0
    if (AEE_SUCCESS == ISHELL_CreateInstance(bt_ui_get_shell(),
                                            AEECLSID_FILEMGR,
                                            (void **)&m_pIFileMgr))
    {
        if(EFAILED == IFILEMGR_Test(m_pIFileMgr,p_root_path))
        {
            //need creat
            //BCM_FUN_STAR("bt_ui_opp_test_file_addr0", 0,0,0);
            if(SUCCESS == IFILEMGR_MkDir(m_pIFileMgr,p_root_path))
            {
                start_ok = TRUE;
            }
        }
        else
        {
            start_ok = TRUE;
        }
        IFILEMGR_Release(m_pIFileMgr);
        m_pIFileMgr = NULL;
        sprintf(p_root_path, "%s%s", "/brew/", FTS_ROOT_FOLDER);
    }
#endif
#else
    if ((bta_fs_co_access (p_root_path, BTA_FS_ACC_EXIST, &is_dir, UI_FTS_ID)) != BTA_FS_CO_OK)
    {
        _getcwd(p_root_path, BTUI_PATH_LEN);
        sprintf(p_root_path, "%s%s", p_root_path, FTS_DEFAULT_ROOT_FOLDER);
        
        /* Create the root directory if it does not exist */
        if ((bta_fs_co_access (p_root_path, BTA_FS_ACC_EXIST, &is_dir, UI_FTS_ID)) != BTA_FS_CO_OK)
        {
            if ((bta_fs_co_mkdir(p_root_path, UI_FTS_ID)) != BTA_FS_CO_OK)
            {
                start_ok = FALSE;
                BCM_MSG_MED ("FTS: Could NOT create root directory",0,0,0);
            }
        }
    }
#endif
    if (start_ok)
    {
        BTA_FtsEnable(bt_ui_cfg.fts_security, bt_ui_cfg.fts_service_name,
            p_root_path, bt_ui_cfg.fts_obex_auth,
            (UINT8)strlen(bt_ui_cfg.fts_realm),
            (UINT8*)bt_ui_cfg.fts_realm, bcmapp_fts_cback, UI_FTS_ID);
        BCM_MSG_MED("FTP SERVER enabled with Root Path %d", bt_ui_cfg.b_sdcard_state,0,0);
    }

}

/*******************************************************************************
**
** Function         bcmapp_fts_disable
**
** Description      Disables FTS
**                  
**
** Returns          void
**
*******************************************************************************/
void bcmapp_fts_disable(void)
{

    BTA_FtsDisable();

}



/*******************************************************************************
**
** Function         bt_ui_fts_cback
**
** Description      FTS UI Callback function.  Handles all FTS events for the UI
**                  
**
** Returns          void
**
*******************************************************************************/
static void bcmapp_fts_cback(tBTA_FTS_EVT event, tBTA_FTS *p_data)
{
    //char buf[132];
    tBTUI_BTA_MSG * p_msg;
    BCM_FUN_STAR("bcmapp_fts_cback %d",event,0,0);
    switch (event)
    {
    case BTA_FTS_ENABLE_EVT:
        bt_fts_cb.b_fts_enable = TRUE;
        //BT_UI_APPL_TRACE_EVENT0("File Transfer: Server Enabled...");
        break;

    case BTA_FTS_AUTH_EVT:
        if (p_data->auth.p_userid)
        {
            BCM_MSG_MED("File Transfer: Authentication Processed for User ID [%s]...",
                              p_data->auth.p_userid,0,0);
        }
        else
            BCM_MSG_MED("File Transfer: Authentication Processed without client user id...",0,0,0);

        if (p_data->auth.userid_required)
        {
            BCM_MSG_MED("               *** Client Requests User ID ***",0,0,0);
        }

        BTA_FtsAuthRsp(bt_ui_cfg.fts_key, "Guest");
        break;

    case BTA_FTS_ACCESS_EVT:
        bt_fts_cb.bytes_transferred = 0;
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
        bt_ui_cb.b_stop_op_file = FALSE;
#endif

        if(TRUE == bcmapp_dm_get_incall_state(TRUE) ||bt_ui_cb.ui_state == UI_SEARCH_NEW_DEVICE/* || bt_ui_cb.ui_state == UI_SENDING_OP_FILE*/)
        {
            BTA_FtsAccessRsp(p_data->access.oper, BTA_FT_ACCESS_FORBID, p_data->access.p_name);
            break;
        }

        //if(bt_ui_cfg.b_sdcard_state)//use sd card
        //{
        //    Appscommon_bluetooth_used_sd(1);
        //}
#if 0
        switch (p_data->access.oper)
        {
        case BTA_FT_OPER_PUT:
            strcpy(buf, "File Transfer: PUT File Access request ");
            break;
        case BTA_FT_OPER_GET:
            strcpy(buf, "File Transfer: GET File Access request ");
            break;
        case BTA_FT_OPER_DEL_FILE:
            strcpy(buf, "File Transfer: DELETE File Access request ");
            break;
        case BTA_FT_OPER_DEL_DIR:
            strcpy(buf, "File Transfer: DELETE Folder Access request ");
            break;
        case BTA_FT_OPER_CHG_DIR:
            strcpy(buf, "File Transfer: CHANGE Folder Access request ");
            break;
        case BTA_FT_OPER_MK_DIR:
        default:
            strcpy(buf, "File Transfer: MAKE Folder Access request ");
            break;
        }

        if (bt_fts_cb.access_flag == BTA_FT_ACCESS_ALLOW)
            strcat(buf, "GRANTED for:");
        else
            strcat(buf, "DENIED for:");

        //BT_UI_APPL_TRACE_EVENT0(buf);
        //BT_UI_APPL_TRACE_EVENT4("   File [%s], Size [%d], Device [%s]%s",
        //   p_data->access.p_name, p_data->access.size, p_data->access.dev_name,
        //   bt_ui_addr_str(p_data->access.bd_addr));
#else
        BCM_MSG_MED("BTA_FTS_ACCESS_EVT %d %d",p_data->access.oper,bt_fts_cb.access_flag,0);
#endif
        bt_ui_cb.b_opp_show_proc = TRUE;
        BTA_FtsAccessRsp(p_data->access.oper, bt_fts_cb.access_flag, p_data->access.p_name);
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_FTS_ACCESS;
            bt_ui_send_msg(BTUI_FTS_MSG_TYPE, p_msg);
        }
        break;

    case BTA_FTS_OPEN_EVT:
        BCM_MSG_MED("File Transfer: Connection Opened with %02x%02x%02x",
            p_data->bd_addr[0], p_data->bd_addr[1], p_data->bd_addr[2]);
        BCM_MSG_MED("File Transfer: Connection Opened with %02x%02x%02x",
            p_data->bd_addr[3], p_data->bd_addr[4], p_data->bd_addr[5]);
        break;

    case BTA_FTS_CLOSE_EVT:
        //if(bt_ui_cfg.b_sdcard_state)//use sd card
        //{
        //    Appscommon_bluetooth_used_sd(0);
        //}

        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_FTS_CLOSE;
            bt_ui_send_msg(BTUI_FTS_MSG_TYPE, p_msg);
        }
        break;

    case BTA_FTS_PROGRESS_EVT:
        bt_fts_cb.bytes_transferred += p_data->prog.bytes;
        if (p_data->prog.file_size != BTA_FS_LEN_UNKNOWN)
        {
            BCM_MSG_MED("File Transfer Server PROGRESS (%d of %d total)...",
                              bt_fts_cb.bytes_transferred, p_data->prog.file_size,0);
        }
        else
        {
            BCM_MSG_MED("File Transfer Server PROGRESS (%d bytes total)...",
                              bt_fts_cb.bytes_transferred,0,0);
        }
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_FTS_PROGRESS;
            p_msg->opp_proc.val.bytes_transferred = bt_fts_cb.bytes_transferred;
            p_msg->opp_proc.val.obj_size = p_data->prog.file_size;
            bt_ui_send_msg(BTUI_FTS_MSG_TYPE, p_msg);
        }
        break;

    case BTA_FTS_DEL_CMPL_EVT:
    case BTA_FTS_GET_CMPL_EVT:
    case BTA_FTS_PUT_CMPL_EVT:
        //if(bt_ui_cfg.b_sdcard_state)//use sd card
        //{
        //    Appscommon_bluetooth_used_sd(0);
        //}

        //BCM_MSG_MED("File Transfer: Delete Complete, status %d, [%s]",
        //                  p_data->obj.status, p_data->obj.p_name,0);
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_FTS_CMPL;
            bt_ui_send_msg(BTUI_FTS_MSG_TYPE, p_msg);
        }
        break;
#if 0
    case BTA_FTS_GET_CMPL_EVT:
        BCM_MSG_MED("File Transfer: Get File Complete, status %d, [%s]",
                          p_data->obj.status, p_data->obj.p_name,0);
        break;

    case BTA_FTS_PUT_CMPL_EVT:
        BCM_MSG_MED("File Transfer: Put File Complete, status %d, [%s]",
                          p_data->obj.status, p_data->obj.p_name,0);
        break;
#endif
    case BTA_FTS_DISABLE_EVT:
        bt_fts_cb.b_fts_enable = FALSE;
        BCM_MSG_MED("File Transfer: Server Disabled...",0,0,0);
        break;

    default:
        break;
    }
}

#endif


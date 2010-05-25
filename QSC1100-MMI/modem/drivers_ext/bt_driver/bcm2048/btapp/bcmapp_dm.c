/****************************************************************************
**
**  Name:          bcmapp_dm.c
**
**  Description:   contains  device manager application
**
**
**  Copyright (c) 2002-2007, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
******************************************************************************/

#include "bt_target.h"

#include "bt_ui_ht_common.h"

#include "gki.h"
#include "bta_api.h"
#include "bt_ui.h"
#include "bt_ui_int.h"
#include "bd.h"
#include "bcmapp_dm.h"
#if (BTM_LISBON_INCLUDED == TRUE)
#include "bta_dm_ci.h"
#endif
#include <stdio.h>
#include <string.h>
#ifndef AEE_SIMULATOR
#include "db.h"
#endif
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
#include "OEMFS.h"
#endif
#if ((defined BTA_AA_INCLUDED) && (BTA_AA_INCLUDED == TRUE)) || \
((defined BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE))
#include <bt_ui_codec.h>
#if (BTU_DUAL_STACK_INCLUDED == TRUE )
#include "bt_ui_av_audio.h"
#include "bta_av_ci.h"
#endif
#endif
#include "OEMSVC.h"
#ifndef BTAPP_DM_FIND_ME_TIME
#define BTAPP_DM_FIND_ME_TIME       62  /* Tgap(104) more than 1 min */
#endif

#ifndef BTAPP_DM_LIMITED_RSSI_OFFSET
#define BTAPP_DM_LIMITED_RSSI_OFFSET    30
#endif

#ifndef BTAPP_DM_DI_DB_SIZE
#define BTAPP_DM_DI_DB_SIZE      1024
#endif
/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void bcmapp_dm_security_cback (tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data);
static void bcmapp_dm_add_devices(void);
static void bcmapp_dm_discover_cb(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
static void bcmapp_dm_search_cb(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);



/* maps the index of the device in the database to the menu options */
//UINT8 bt_ui_device_db_to_menu_option_lkup_tbl[BTUI_NUM_REM_DEVICE];

/* newly found devices */
tBTUI_INQ_DB bt_ui_inq_db;
/* device database */
tBTUI_DEV_DB bt_ui_device_db;
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
typedef enum _bt_file_type
{
    BT_FILE_TYPE_OTHER = 0x00,
    BT_FILE_TYPE_MUSIC,
    BT_FILE_TYPE_PICTRUE,
    BT_FILE_TYPE_VIDEO
}bt_file_type;
#endif
/*******************************************************************************
**
** Function         bcmapp_startup
**
** Description      Initializes bt application and waits for device to come up
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_startup(void)
{

    //     memset(&bt_ui_device_db,0x00,sizeof(bt_ui_device_db));
    //     memset(&bt_ui_device_db_to_menu_option_lkup_tbl, 0,
    //                 sizeof(bt_ui_device_db_to_menu_option_lkup_tbl));
    //
    //     while(!BTA_DmIsDeviceUp())
    //     {
    //         GKI_delay(200);
    //     }
    //    
    // 
    //     /* read all parmeters stored in nvram */
    //     bt_ui_init_device_db();
    // 
    //     bt_ui_platform_startup();

}
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
static void bcmapp_dm_get_sd_card_state(void)
{
    bt_ui_cfg.b_sdcard_state = FALSE;
#ifdef FEATURE_SUPPORT_VC0848
    bt_ui_cfg.b_sdcard_state = MediaGalleryApp_CheckSDCard();
    MediaGalleryApp_StopSDCard();
#else
    //if(SUCCESS == IFILEMGR_Test(pMe->m_pIFileMgr, MG_MASSCARD_ROOTDIR))
    //{
    //    bt_ui_cfg.b_sdcard_state = TRUE;
    //}
    //else 
    {
        bt_ui_cfg.b_sdcard_state = FALSE;
    }
#endif
    BCM_FUN_STAR("%x bcmapp_dm_get_sd_card_state",bt_ui_cfg.b_sdcard_state,0,0);
}
#endif

/*******************************************************************************
**
** Function         bcmapp_dm_init
**
** Description      Initializes Device manger
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_init(void)
{
/* enable bluetooth before calling
    other BTA API */
    BCM_MSG_MED("bcmapp_dm_init",0,0,0);
    BTA_EnableBluetooth(&bcmapp_dm_security_cback);

    /* set local bluetooth name */
    BTA_DmSetDeviceName(bt_ui_device_db.local_device_name);

    /* add devices from nv data base to BTA */
    bcmapp_dm_add_devices();

#ifdef FEATURE_BLUETOOTH_DEVICE_VISIBILITY
    /* set visibility and connectability */
    if(bt_ui_device_db.visibility)
        BTA_DmSetVisibility(BTA_DM_GENERAL_DISC, BTA_DM_CONN);
    else
#endif    
        BTA_DmSetVisibility(BTA_DM_NON_DISC, BTA_DM_CONN);
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
        bcmapp_dm_get_sd_card_state();
#endif
        
#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )
        if( bt_ui_cfg.p_ag_init)
        {
             bt_ui_cfg.p_ag_init();
        }
#endif

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if( bt_ui_cfg.p_ops_init)
    {
         bt_ui_cfg.p_ops_init();
    }

    if( bt_ui_cfg.p_opc_init)
    {
         bt_ui_cfg.p_opc_init();
    }
#endif 
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    if(bt_ui_cfg.p_fts_init)
    {
        bt_ui_cfg.p_fts_init();
    }
#endif

#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
    if(bt_ui_cfg.p_ct_init)
    {
        bt_ui_cfg.p_ct_init();
    }
#endif
}


/*******************************************************************************
**
** Function         bcmapp_dm_pin_code_reply
**
** Description      Process the passkey entered by user
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_pin_code_reply(BOOLEAN accept, UINT8 pin_len,
                                        UINT8 *p_pin)

{
    BCM_MSG_MED("bcmapp_dm_pin_code_reply %d",accept,0,0);
    BTA_DmPinReply( bt_ui_cb.peer_bdaddr,
        accept,
        pin_len,
        (UINT8*)p_pin);
}

#if (BTM_LISBON_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bcmapp_dm_confirm_reply
**
** Description      Process the confirm/reject entered by user
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_confirm_reply(BOOLEAN accept)
{
    BTA_DmConfirm( bt_ui_cb.peer_bdaddr, accept);
}

/*******************************************************************************
**
** Function         bcmapp_dm_passkey_cancel
**
** Description      Process the passkey cancel entered by user
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_passkey_cancel(void)
{
    BTA_DmPasskeyCancel(bt_ui_cb.peer_bdaddr);
}


/*******************************************************************************
**
** Function         bcmapp_dm_proc_io_req
**
** Description
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_proc_io_req(BD_ADDR bd_addr, tBTA_IO_CAP *p_io_cap, tBTA_OOB_DATA *p_oob_data,
                                    tBTA_AUTH_REQ *p_auth_req, BOOLEAN is_orig)
{
    UINT8   dd_bit = (bt_ui_cfg.sp_auth_req & BTUI_AUTH_REQ_DD_BIT);
    UINT8   yes_no_bit = BTA_AUTH_SP_NO;

    if(0 == bdcmp(bt_ui_cb.oob_bdaddr, bd_addr))
        *p_oob_data = TRUE;
    BCM_MSG_MED("bcmapp_dm_proc_io_req cfg:%d, auth_req:%d, oob:%d",
        bt_ui_cfg.sp_auth_req, *p_auth_req, *p_oob_data);

    if(dd_bit)
        yes_no_bit = BTA_AUTH_SP_YES;

    switch(bt_ui_cfg.sp_auth_req)
    {
    case BTUI_AUTH_REQ_NO:  /* 0:not required */
    case BTUI_AUTH_REQ_YES: /* 1:required */
        *p_auth_req = bt_ui_cfg.sp_auth_req;
        break;
    case BTUI_AUTH_REQ_GEN_BOND: /* 2:use default + general bonding DD=NO*/
    case BTUI_AUTH_REQ_GEN_BOND_DD:/* 4:use default + general bonding DD=YES*/
        /* the new cswg discussion wants us to indicate the bonding bit */
        if(bt_ui_get_device_record(bd_addr))
        {
            if(bt_ui_cb.is_dd_bond)
            {
                /* if initing/responding to a dedicated bonding, use dedicate bonding bit */
                *p_auth_req = BTA_AUTH_DD_BOND | yes_no_bit;
            }
            else
                *p_auth_req |= BTA_AUTH_GEN_BOND; /* set the general bonding bit for stored device */
        }
        break;
    default:/*and BTUI_AUTH_REQ_DEFAULT 3:use default */
        if(bt_ui_cb.is_dd_bond)
        {
            /* if initing/responding to a dedicated bonding, use dedicate bonding bit */
            *p_auth_req = BTA_AUTH_DD_BOND | yes_no_bit;
        }
        break;
    }
    BCM_MSG_MED("auth_req:%d", *p_auth_req,0,0);
}

/*******************************************************************************
**
** Function         bcmapp_dm_proc_io_rsp
**
** Description
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_proc_io_rsp(BD_ADDR bd_addr, tBTA_IO_CAP io_cap, tBTA_AUTH_REQ auth_req)
{
    tBTUI_REM_DEVICE    *p_device_rec;
    p_device_rec = bt_ui_get_device_record(bd_addr);
    if(p_device_rec)
    {
        p_device_rec->peer_io_cap = io_cap;
    }
    if(auth_req & BTA_AUTH_BONDS)
    {
        if(auth_req & BTA_AUTH_DD_BOND)
            bt_ui_cb.is_dd_bond = TRUE;
        /* store the next generator link key */
        bdcpy(bt_ui_cb.sp_bond_bdaddr, bd_addr);
        bt_ui_cb.sp_io_cap = io_cap;
        bt_ui_cb.sp_bond_bits = (auth_req & BTA_AUTH_BONDS);
    }
    BT_UI_APPL_TRACE_DEBUG2("bcmapp_dm_proc_io_rsp auth_req:%d, is_dd_bond:%d",
        auth_req, bt_ui_cb.is_dd_bond);
}

/*******************************************************************************
**
** Function         bcmapp_dm_proc_lk_upgrade
**
** Description
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_proc_lk_upgrade(BD_ADDR bd_addr, BOOLEAN *p_upgrade)
{
    if(bt_ui_cfg.sp_auth_req == BTUI_AUTH_REQ_NO)
    {
        /* if hard coded to use no MITM, do not upgrade the link key */
        *p_upgrade = FALSE;
    }
}

/*******************************************************************************
**
** Function         bcmapp_dm_rmt_oob_reply
**
** Description      Process the hash C, randomizer r/reject entered by user
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_rmt_oob_reply(BOOLEAN accept, BT_OCTET16 c, BT_OCTET16 r)
{
    BT_UI_APPL_TRACE_API0("calling bta_dm_ci_rmt_oob");
    bta_dm_ci_rmt_oob(accept, bt_ui_cb.peer_bdaddr, c, r);
}

/*******************************************************************************
**
** Function         bcmapp_dm_loc_oob
**
** Description      Read OOB data from local LM
**
** Returns          void
*******************************************************************************/
void  bcmapp_dm_loc_oob(void)
{
#if (BTM_OOB_INCLUDED == TRUE)
    BTA_DmLocalOob();
#else
    BT_UI_APPL_TRACE_ERROR0("BTM_OOB_INCLUDED is FALSE!!(bcmapp_dm_loc_oob)");
#endif
}

#endif

/*******************************************************************************
**
** Function         bcmapp_dm_authorize_resp
**
** Description      Action function to process auth reply
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_authorize_resp(tBTA_AUTH_RESP response)
{
    tBTUI_REM_DEVICE * p_device_rec;
    tBTUI_REM_DEVICE  device_rec;
    BCM_MSG_MED("bcmapp_dm_authorize_resp %d",response,0,0);
    if(response == BTA_DM_AUTH_PERM)
    {
        if((p_device_rec = bt_ui_get_device_record(bt_ui_cb.peer_bdaddr))!= NULL)
        {
            p_device_rec->is_trusted = TRUE;
            p_device_rec->trusted_mask |= (1<<bt_ui_cb.peer_service);
            //p_device_rec->in_use =TRUE;
            //p_device_rec->link_key_present = TRUE;
            bt_ui_store_device(p_device_rec);
        }
        else
        {
            memset(&device_rec, 0, sizeof(device_rec));
            device_rec.trusted_mask  = (1<<bt_ui_cb.peer_service);
            strncpy(device_rec.name, bt_ui_cb.peer_name, BTUI_DEV_NAME_LENGTH);
            bdcpy(device_rec.bd_addr, bt_ui_cb.peer_bdaddr);
            device_rec.is_trusted = TRUE;
            //p_device_rec->in_use =TRUE;
            //p_device_rec->link_key_present = TRUE;
            bt_ui_store_device(&device_rec);
        }

        BTA_DmAuthorizeReply(bt_ui_cb.peer_bdaddr, bt_ui_cb.peer_service,   BTA_DM_AUTH_PERM);
    }
    else if(response == BTA_DM_AUTH_TEMP)
    {
        BTA_DmAuthorizeReply(bt_ui_cb.peer_bdaddr, bt_ui_cb.peer_service,   BTA_DM_AUTH_TEMP);
    }
    else
    {
        BTA_DmAuthorizeReply(bt_ui_cb.peer_bdaddr, bt_ui_cb.peer_service,   BTA_DM_NOT_AUTH);
    }
}

/*******************************************************************************
**
** Function         bcmapp_dm_disable_bt
**
** Description      Disables Bluetooth.
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_disable_bt()
{
    BTA_DisableBluetooth();
}

#if (BTU_DUAL_STACK_INCLUDED == TRUE )
/*******************************************************************************
**
** Function         bcmapp_dm_switch_stack_cback
**
** Description      Callback indicating result of bb2mm switch
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_switch_stack_cback(tBTA_DM_SWITCH_EVT evt, tBTA_STATUS status)
{
    tBTUI_BTA_MSG       *p_event_msg;

    BCM_MSG_MED("SWITCH STACK evt %d %d %d",evt,status,bt_ui_cb.is_switched);
    if (status == BTA_SUCCESS)
    {
        if( bt_ui_cb.is_switched )
        {
            bt_ui_cb.is_switched = FALSE;
            //DBGPRINTF("SWITCHED to BB");

#if (BTA_AV_INCLUDED == TRUE)
            if(bt_ui_av_cb.audio_open_count > 0)
            {
                bt_ui_av_send_sync_request(BTUI_AV_AUDIO_SYNC_TO_FULL);
            }
#endif
        }
        else
        {
            bt_ui_cb.is_switched = TRUE;
            //DBGPRINTF("SWITCHED to MM");

#if (BTA_AV_INCLUDED == TRUE)
            if(bt_ui_av_cb.audio_open_count > 0)
            {
                bt_ui_av_send_sync_request(BTUI_AV_AUDIO_SYNC_TO_LITE);
            }

            if(bt_ui_cb.is_starting_stream)
            {
                bt_ui_cb.is_starting_stream = FALSE;
                bcmapp_av_start_stream();
            }

            if(bt_ui_av_cb.is_prestart)
            {
                bt_ui_av_cb.is_prestart = FALSE;
                bta_av_ci_startok(bt_ui_av_cb.prestart_hndl);
            }
#endif
        }
    }
    else
    {
        BCM_MSG_MED("STACK SWITCH FAILED!",0,0,0);
    }

    //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
    if ((p_event_msg = bt_ui_get_msg()) != NULL)
    {
        p_event_msg->switch_stack_cmpl.hdr.event = BTUI_MMI_SWITCH_STACK_CMPL_EVT;
        p_event_msg->switch_stack_cmpl.result = (BTA_SUCCESS == status)? BTUI_SUCCESS : BTUI_FAIL;

        //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
        bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
    }

}

/*******************************************************************************
**
** Function         bcmapp_dm_switch_bb2mm
**
** Description      Switches from Baseband to Multimedia
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_switch_bb2mm(void)
{
    BTA_DmSwitchStack(BTA_DM_SW_BB_TO_MM, bcmapp_dm_switch_stack_cback);
}
/*******************************************************************************
**
** Function         bcmapp_dm_switch_mm2bb
**
** Description      Switches from Baseband to Multimedia
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_switch_mm2bb(void)
{
    BTA_DmSwitchStack(BTA_DM_SW_MM_TO_BB, bcmapp_dm_switch_stack_cback);
}
#endif /*(BTU_DUAL_STACK_INCLUDED == TRUE )*/

/*******************************************************************************
**
** Function         bcmapp_dm_set_visibility
**
** Description      Sets visibilty
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_set_visibility( BOOLEAN is_visible, BOOLEAN is_temp)
{
    BCM_MSG_MED("bcmapp_dm_set_visibility",bt_ui_device_db.visibility,0,0);
    BTA_DmSetVisibility( (tBTA_DM_DISC) ((is_visible) ? BTA_DM_GENERAL_DISC : BTA_DM_NON_DISC),
        BTA_DM_CONN);

    bt_ui_device_db.visibility = is_visible;


    if(!is_temp)
    {
        /* update to nvram */
        bt_ui_store_visibility_setting(bt_ui_device_db.visibility);
    }

}

/*******************************************************************************
**
** Function         bcmapp_dm_timer_cback
**
** Description      Timer used to end find-me disconverable mode
**
**
** Returns          void
**
*******************************************************************************/
void bcmapp_dm_timer_cback(void *p_tle)
{
    BCM_MSG_MED(" Find me mode ends. vis: %d", bt_ui_device_db.visibility,0,0);
    /* go back to the previous DISC mode */
    bcmapp_dm_set_visibility(bt_ui_device_db.visibility, TRUE);
}

/*******************************************************************************
**
** Function         bcmapp_dm_set_find_me
**
** Description      Sets visibilty
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_set_find_me( void)
{
    BCM_MSG_MED("bcmapp_dm_set_find_me",0,0,0);
    //bt_ui_device_db.dev_tle.p_cback = bcmapp_dm_timer_cback;
    //bt_ui_app_start_timer(&bt_ui_device_db.dev_tle, 0, BTAPP_DM_FIND_ME_TIME);

    BTA_DmSetVisibility( BTA_DM_LIMITED_DISC, BTA_DM_CONN);
}

/*******************************************************************************
**
** Function         bcmapp_dm_set_local_name
**
** Description      Sets local name
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_set_local_name(char *p_name)
{
    BTA_DmSetDeviceName(p_name);
    //strncpy(bt_ui_device_db.local_device_name, p_name, BTUI_DEV_NAME_LENGTH);
    /* update to nv memory */
    bt_ui_store_local_name(p_name);
}


/*******************************************************************************
**
** Function         bcmapp_dm_sec_add_device
**
** Description      Sets device as trusted
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_sec_add_device(tBTUI_REM_DEVICE * p_device_rec)
{
    BCM_MSG_MED("bcmapp_dm_sec_add_device %d",p_device_rec,0,0);
    /* update BTA with new settings */
    if(p_device_rec->link_key_present)
        BTA_DmAddDevice(p_device_rec->bd_addr, p_device_rec->link_key,
        p_device_rec->trusted_mask, p_device_rec->is_trusted,
        p_device_rec->key_type, p_device_rec->peer_io_cap);
    else
        BTA_DmAddDevice(p_device_rec->bd_addr, NULL, p_device_rec->trusted_mask,
        p_device_rec->is_trusted, p_device_rec->key_type, p_device_rec->peer_io_cap);

}


/*******************************************************************************
**
** Function         bcmapp_dm_set_trusted
**
** Description      Sets device as trusted
**
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_dm_set_trusted(tBTA_SERVICE_MASK trusted_mask, tBTUI_REM_DEVICE * p_device_rec)
{
    BCM_MSG_MED("bcmapp_dm_set_trusted %x",trusted_mask,0,0);
    p_device_rec->trusted_mask |= trusted_mask;
    p_device_rec->is_trusted = TRUE;
    bt_ui_store_device(p_device_rec);
    /* update BTA with new settings */
    bcmapp_dm_sec_add_device(p_device_rec);

    return TRUE;
}

/*******************************************************************************
**
** Function         bcmapp_dm_set_not_trusted
**
** Description      Sets device as not trusted
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_set_not_trusted(tBTUI_REM_DEVICE * p_device_rec)
{
    BCM_MSG_MED("bcmapp_dm_set_not_trusted",0,0,0);
    p_device_rec->is_trusted = FALSE;
    bt_ui_store_device(p_device_rec);
    bcmapp_dm_sec_add_device(p_device_rec);


}
/*******************************************************************************
**
** Function         bcmapp_dm_delete_device
**
** Description      Deletes a device from data base
**
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_dm_delete_device(BD_ADDR bd_addr)
{
    BCM_MSG_MED("bcmapp_dm_delete_device",0,0,0);
    if ((BTA_DmRemoveDevice (bd_addr)) == BTA_SUCCESS)
    {
        return bt_ui_delete_device(bd_addr);
        //return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bcmapp_dm_discover_device
**
** Description      Searches for services on designated device.
**
**
** Returns          void
*******************************************************************************/

void bcmapp_dm_discover_device(BD_ADDR bd_addr, BOOLEAN is_new)
{
    tBTA_SERVICE_MASK client_services;
    BOOLEAN sdp_search;

    /* we need to find only services for which we can be in client role */
    client_services = (bt_ui_cfg.supported_services & ~(BTA_SPP_SERVICE_MASK | BTA_DUN_SERVICE_MASK | BTA_FAX_SERVICE_MASK | BTA_LAP_SERVICE_MASK));
    BCM_MSG_MED(" bcmapp_dm_discover_device  client_services x%x %c",client_services, bt_ui_cfg.dg_client_service_id[0],0);

#if( defined BTA_FT_INCLUDED ) && (BTA_FT_INCLUDED == TRUE)
    if(!bt_ui_cfg.ftc_included)
        client_services &= ~ BTA_FTP_SERVICE_MASK;
#endif

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    if(!bt_ui_cfg.opc_included)
        client_services &= ~ BTA_OPP_SERVICE_MASK;
#endif        

    client_services |= 1 << (bt_ui_cfg.dg_client_service_id[0] - '0');

    if( is_new )
        sdp_search = FALSE;
    else
        sdp_search = TRUE;

    BTA_DmDiscover(bd_addr, client_services, bcmapp_dm_discover_cb, sdp_search);
}




/*******************************************************************************
**
** Function         bcmapp_dm_stored_device_unbond
**
** Description      Unbond selected device
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_dm_stored_device_unbond ()
{
    BCM_MSG_MED("bcmapp_dm_stored_device_unbond",0,0,0);
    if (bt_ui_cb.p_selected_rem_device->link_key_present &&
        (BTA_SUCCESS == BTA_DmRemoveDevice(bt_ui_cb.p_selected_rem_device->bd_addr)))
    {
        bt_ui_cb.p_selected_rem_device->link_key_present = FALSE;
        bt_ui_store_device(bt_ui_cb.p_selected_rem_device);
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bcmapp_dm_cancel_search
**
** Description      Cancels search
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_cancel_search(void)
{
    BCM_MSG_MED("bcmapp_dm_cancel_search",0,0,0);
    BTA_DmSearchCancel();

}
#if BTA_DI_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bcmapp_dm_di_discover
**
** Description      Start DI discover
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_di_discover(BD_ADDR bd_addr)
{
    bt_ui_cb.p_di_db = (tBTA_DISCOVERY_DB *)GKI_getbuf(BTAPP_DM_DI_DB_SIZE);

    BTA_DmDiDiscover(bd_addr, bt_ui_cb.p_di_db, BTAPP_DM_DI_DB_SIZE, bcmapp_dm_discover_cb);

}
/*******************************************************************************
**
** Function         bcmapp_dm_add_di_record
**
** Description      Set local DI record
**
**
** Returns          void
*******************************************************************************/
UINT16 bcmapp_dm_add_di_record(void)
{
    tBTA_DI_RECORD      device_info;

    memset(&device_info, 0, sizeof(tBTA_DI_RECORD));

    device_info.vendor = LMP_COMPID_WIDCOMM ;           /* 17 */
    device_info.vendor_id_source = DI_VENDOR_ID_SOURCE_BTSIG;  /* from Bluetooth SIG */
    device_info.product = 0x1234;
    device_info.version = 0x0312;       /* version 3.1.2 */
    device_info.primary_record = FALSE;

    return BTA_DmSetLocalDiRecord(&device_info, &(bt_ui_cb.di_handle));

}
/*******************************************************************************
**
** Function         bcmapp_dm_get_di_local_record
**
** Description      Get local DI record
**
**
** Returns          void
*******************************************************************************/
tBTA_STATUS bcmapp_dm_get_di_local_record(tBTA_DI_GET_RECORD *p_di_record, UINT32 handle)
{
    UINT32 di_handle = handle;

    return BTA_DmGetLocalDiRecord(p_di_record, &di_handle);
}
/*******************************************************************************
**
** Function         bcmapp_dm_get_di_remote_record
**
** Description      Get remote DI record by index.
**
**
** Returns          void
*******************************************************************************/
tBTA_STATUS bcmapp_dm_get_di_remote_record(tBTA_DI_GET_RECORD *p_record, UINT8 index)
{
    tBTA_STATUS status = BTA_FAILURE;

    memset(p_record, 0 , sizeof(tBTA_DI_GET_RECORD));

    if (bt_ui_cb.p_di_db != NULL)
    {
        status = BTA_DmGetDiRecord(index, p_record, bt_ui_cb.p_di_db);
    }

    return status;
}
#endif
/*******************************************************************************
**
** Function         bcmapp_dm_search
**
** Description      Searches for devices supporting the services specified
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_search(tBTA_SERVICE_MASK services,tBTA_DM_INQ *p_data)
{
    tBTA_DM_INQ inq_params;

    if(!p_data)
    {
        inq_params.mode = 0;
        inq_params.duration = BTUI_DEFAULT_INQ_DURATION;
        inq_params.max_resps = bt_ui_cfg.num_inq_devices;
        inq_params.filter_type = bt_ui_cfg.dm_inq_filt_type;
        memcpy(&inq_params.filter_cond, &(bt_ui_cfg.dm_inq_filt_cond), sizeof(tBTA_DM_INQ_COND));
    }
    else
    {
        memcpy(&inq_params, p_data, sizeof(tBTA_DM_INQ));
#ifdef FEATURE_BLUETOOTH_APP
        if(0 == inq_params.duration)
        {      
            inq_params.duration = BTUI_DEFAULT_INQ_DURATION;
        }
        if(0 == inq_params.max_resps)
        {      
            inq_params.max_resps = bt_ui_cfg.num_inq_devices;
        }
        /* set filter condition in cfg if type is not specified */
        if(BTA_DM_INQ_CLR == inq_params.filter_type)
        {
            inq_params.filter_type = bt_ui_cfg.dm_inq_filt_type;
            memcpy(&inq_params.filter_cond, &(bt_ui_cfg.dm_inq_filt_cond), sizeof(tBTA_DM_INQ_COND));
        }
#endif        
    }

#ifdef FEATURE_BLUETOOTH_APP
    /* Only need one response when looking for bd_addr */
    if (inq_params.filter_type == BTA_DM_INQ_BD_ADDR)
    {   
        inq_params.max_resps = 1;   
    }
    /* Use a shorter duration for limited inquiry */
    if(BTA_DM_LIMITED_INQUIRY == inq_params.mode)
    {   
        inq_params.duration /= 3;
    }
#endif

    bt_ui_inq_db.rem_index = 0;
    memset(&bt_ui_inq_db, 0, sizeof(bt_ui_inq_db));
    bt_ui_cb.search_services = services;

    /* find nearby devices */
    BTA_DmSearch(&inq_params, services, bcmapp_dm_search_cb);
}

/*******************************************************************************
**
** Function         bcmapp_dm_add_device
**
** Description      Adds a new device to database
**
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_dm_add_device(void)
{
    BCM_MSG_MED("bcmapp_dm_add_device",0,0,0);

    if(bt_ui_store_device(bt_ui_cb.p_selected_rem_device))
        return TRUE;

    return FALSE;
}



/*******************************************************************************
**
** Function         bcmapp_dm_bond
**
** Description      Initiates bonding with selected device
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_bond(tBTUI_REM_DEVICE * p_device_rec)
{
#if (BTM_LISBON_INCLUDED == TRUE)
    bt_ui_cb.is_dd_bond = TRUE;
    bdcpy(bt_ui_cb.sp_bond_bdaddr, p_device_rec->bd_addr);
#endif
    BTA_DmBond (p_device_rec->bd_addr);

    //bt_ui_store_device(p_device_rec);
}


/*******************************************************************************
**
** Function         bcmapp_dm_bond_cancel
**
** Description      Cancels bonding with selected device
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_bond_cancel(tBTUI_REM_DEVICE * p_device_rec)
{
#if (BTM_LISBON_INCLUDED == TRUE)
    bt_ui_cb.is_dd_bond = FALSE;
#endif
    BTA_DmBondCancel (p_device_rec->bd_addr);
}


/*******************************************************************************
**
** Function         bcmapp_dm_rename_device
**
** Description      sets user friendly name for remote device
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_rename_device(tBTUI_REM_DEVICE * p_device_rec, char * p_text)
{
    strncpy(p_device_rec->short_name, p_text, BTUI_DEV_NAME_LENGTH);
    /* update to nv memory */
    bt_ui_store_device(p_device_rec);
}

/*******************************************************************************
**
** Function         bcmapp_dm_security_cback
**
** Description      Security callback from bta
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_dm_security_cback (tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data)
{
    tBTUI_BTA_MSG * p_event_msg;
    tBTUI_REM_DEVICE * p_device_record;
#if (BTM_LISBON_INCLUDED == TRUE)
    BOOLEAN alloc = FALSE;
#endif
    //char msg_str[64];

    BCM_MSG_MED("bcmapp_dm_security_cback  event %d ",event,0,0);

    switch(event)
    {
    case BTA_DM_ENABLE_EVT:
        bdcpy(bt_ui_cb.local_bd_addr, p_data->enable.bd_addr);
        bt_ui_device_db.bcm_enabled = TRUE;
        bt_ui_store_bt_enable_setting(TRUE);
        //sprintf (msg_str, "local bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
        //  p_data->enable.bd_addr[0], p_data->enable.bd_addr[1],
        //  p_data->enable.bd_addr[2], p_data->enable.bd_addr[3],
        //  p_data->enable.bd_addr[4], p_data->enable.bd_addr[5]);
        ////DBGPRINTF(msg_str);
#if (BTM_LISBON_INCLUDED == TRUE)
        BCM_MSG_MED("auth_req:%d", bt_ui_cfg.sp_auth_req,0,0);
#endif
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_ENABLE;
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        return;

    case BTA_DM_DISABLE_EVT:
        bt_ui_device_db.bcm_enabled = FALSE;
        bt_ui_store_bt_enable_setting(FALSE);
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_DISABLE;
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

    case BTA_DM_SIG_STRENGTH_EVT:

        if(p_data->sig_strength.mask & BTA_SIG_STRENGTH_RSSI_MASK)
        {
            BCM_MSG_MED("rssi value %d", p_data->sig_strength.rssi_value,0,0);
        }

        if(p_data->sig_strength.mask & BTA_SIG_STRENGTH_LINK_QUALITY_MASK)
        {
            BCM_MSG_MED("link quality value %d", p_data->sig_strength.link_quality_value,0,0);
        }
        return;

    case BTA_DM_LINK_UP_EVT:

        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_LINK_UP;
            bdcpy(p_event_msg->link_change.bd_addr, p_data->link_up.bd_addr);

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }

        //sprintf (msg_str, "connected to bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
        //  p_data->link_up.bd_addr[0], p_data->link_up.bd_addr[1],
        //  p_data->link_up.bd_addr[2], p_data->link_up.bd_addr[3],
        //  p_data->link_up.bd_addr[4], p_data->link_up.bd_addr[5]);

        ////DBGPRINTF(msg_str);
        return;

    case BTA_DM_LINK_DOWN_EVT:
        //sprintf (msg_str, "disconnected from bdaddr %02x:%02x:%02x:%02x:%02x:%02x reason %04x \n",
        //  p_data->link_down.bd_addr[0], p_data->link_down.bd_addr[1],
        //  p_data->link_down.bd_addr[2], p_data->link_down.bd_addr[3],
        //  p_data->link_down.bd_addr[4], p_data->link_down.bd_addr[5],p_data->link_down.status);

        ////DBGPRINTF(msg_str);

        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_LINK_DOWN;
            bdcpy(p_event_msg->link_change.bd_addr, p_data->link_down.bd_addr);
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }

#if (BTM_LISBON_INCLUDED == TRUE)
        if(bdcmp(bt_ui_cb.sp_bond_bdaddr, p_data->link_down.bd_addr)==0)
        {
            /* bonding must have failed - clear the is_bond flag */
            bt_ui_cb.sp_bond_bits = 0;
            bt_ui_cb.is_dd_bond = FALSE;
        }
#endif
        return;

    case BTA_DM_PIN_REQ_EVT:
#ifdef FEATURE_BLUETOOTH_APP
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if(TRUE == bcmapp_dm_get_incall_state(FALSE))
        {
            BCM_MSG_MED("BTA_DM_PIN_REQ_EVT fail because incall state",0,0,0);
            BTA_DmPinReply( p_data->pin_req.bd_addr, FALSE, 0, (UINT8*)"");
            return;
        }
#endif        
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->pin_req.hdr.event = BTUI_MMI_PIN_REQ;
            bdcpy(p_event_msg->pin_req.bd_addr, p_data->pin_req.bd_addr);
            strncpy(p_event_msg->pin_req.dev_name, (char*)p_data->pin_req.bd_name, BTUI_DEV_NAME_LENGTH);
            p_event_msg->pin_req.dev_name[BTUI_DEV_NAME_LENGTH] = 0;
            p_event_msg->pin_req.dev_class[0] = p_data->pin_req.dev_class[0];
            p_event_msg->pin_req.dev_class[1] = p_data->pin_req.dev_class[1];
            p_event_msg->pin_req.dev_class[2] = p_data->pin_req.dev_class[2];
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

#if (BTM_LISBON_INCLUDED == TRUE)
    case BTA_DM_SP_CFM_REQ_EVT: /* 9  Simple Pairing User Confirmation request. */
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->pin_req.hdr.event = BTUI_MMI_CFM_REQ;
            bdcpy(p_event_msg->cfm_req.bd_addr, p_data->cfm_req.bd_addr);
            strncpy(p_event_msg->cfm_req.dev_name, p_data->cfm_req.bd_name, BTUI_DEV_NAME_LENGTH);
            p_event_msg->cfm_req.dev_class[0] = p_data->cfm_req.dev_class[0];
            p_event_msg->cfm_req.dev_class[1] = p_data->cfm_req.dev_class[1];
            p_event_msg->cfm_req.dev_class[2] = p_data->cfm_req.dev_class[2];
            p_event_msg->cfm_req.num_val = p_data->cfm_req.num_val;
            p_event_msg->cfm_req.just_works = p_data->cfm_req.just_works;
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

    case BTA_DM_SP_KEY_NOTIF_EVT:
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->key_notif.hdr.event = BTUI_MMI_KEY_NOTIF;
            bdcpy(p_event_msg->key_notif.bd_addr, p_data->key_notif.bd_addr);
            strncpy(p_event_msg->key_notif.dev_name, p_data->key_notif.bd_name, BTUI_DEV_NAME_LENGTH);
            p_event_msg->key_notif.dev_class[0] = p_data->key_notif.dev_class[0];
            p_event_msg->key_notif.dev_class[1] = p_data->key_notif.dev_class[1];
            p_event_msg->key_notif.dev_class[2] = p_data->key_notif.dev_class[2];
            p_event_msg->key_notif.passkey = p_data->key_notif.passkey;

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

    case BTA_DM_SP_RMT_OOB_EVT:
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->rmt_oob.hdr.event = BTUI_MMI_RMT_OOB;
            bdcpy(p_event_msg->rmt_oob.bd_addr, p_data->rmt_oob.bd_addr);
            strncpy(p_event_msg->rmt_oob.dev_name, p_data->rmt_oob.bd_name, BTUI_DEV_NAME_LENGTH);
            p_event_msg->rmt_oob.dev_class[0] = p_data->rmt_oob.dev_class[0];
            p_event_msg->rmt_oob.dev_class[1] = p_data->rmt_oob.dev_class[1];
            p_event_msg->rmt_oob.dev_class[2] = p_data->rmt_oob.dev_class[2];
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

    case BTA_DM_SP_KEYPRESS_EVT: /* key press notification */
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->pin_req.hdr.event = BTUI_MMI_KEY_PRESS;
            bdcpy(p_event_msg->key_press.bd_addr, p_data->key_press.bd_addr);
            p_event_msg->key_press.notif_type = p_data->key_press.notif_type;
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;
#endif

    case BTA_DM_AUTH_CMPL_EVT:
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        //if ((p_event_msg = bt_ui_get_msg()) != NULL)
        //{
        //    p_event_msg->auth_cmpl.hdr.event = BTUI_MMI_AUTH_CMPL;
        //    bdcpy(p_event_msg->auth_cmpl.bd_addr, p_data->auth_cmpl.bd_addr);
        //    p_event_msg->auth_cmpl.is_success = p_data->auth_cmpl.success;
        //    strncpy((char*)p_event_msg->auth_cmpl.dev_name, (char*)p_data->auth_cmpl.bd_name, BTUI_DEV_NAME_LENGTH);

        //    //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
        //    bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        //}

#if (BTM_LISBON_INCLUDED == TRUE)
        if(bdcmp(bt_ui_cb.sp_bond_bdaddr, p_data->auth_cmpl.bd_addr)==0)
        {
            /* peer indicated bonding during SP pairing process.
            * Allocate a device record to store the link key if pairing is successful */
            if(bt_ui_cb.sp_bond_bits)
                alloc = TRUE;
            bt_ui_cb.sp_bond_bits = 0;
            bt_ui_cb.is_dd_bond = FALSE;
        }
#endif

        if (p_data->auth_cmpl.key_present)
        {
            //sprintf (msg_str, "link_key %02x:%02x:%02x:%02x:%02x:%02x \n",
            //  p_data->auth_cmpl.key[0], p_data->auth_cmpl.key[1],
            //  p_data->auth_cmpl.key[2], p_data->auth_cmpl.key[3],
            //  p_data->auth_cmpl.key[4], p_data->auth_cmpl.key[5]);

            ////DBGPRINTF(msg_str);

            p_device_record = bt_ui_get_device_record(p_data->auth_cmpl.bd_addr);

            if(!p_device_record 
#if (BTM_LISBON_INCLUDED == TRUE)
                && alloc
#endif
                )
            {
                p_device_record = bt_ui_alloc_device_record(p_data->auth_cmpl.bd_addr);
                if(p_device_record)
                {
                    BCM_MSG_MED("allocating a device record for the newly bonded device!%d",p_device_record->in_use,0,0);
#if (BTM_LISBON_INCLUDED == TRUE)
                    p_device_record->peer_io_cap = bt_ui_cb.sp_io_cap;
#endif
                }
            }

            /* update data base with new link key */
            if(p_device_record)
            {
                tBTUI_REM_DEVICE *p_inquiry_rec = NULL;
                p_inquiry_rec = bt_ui_get_inquiry_record(p_data->auth_cmpl.bd_addr);
                
#ifdef FEATURE_BLUETOOTH_APP
                if(p_inquiry_rec != NULL)
                {
                    p_device_record->services = p_inquiry_rec->services;
                }
                /*add cod and service to record*/
                if((0 == bt_ui_cb.peer_cod[0])&&(0 == bt_ui_cb.peer_cod[1])&&(0 == bt_ui_cb.peer_cod[2]))
                {
                    BCM_MSG_MED("peer_cod is 0,get it from inq_db",0,0,0);
                    memcpy(p_device_record->dev_class,p_inquiry_rec->dev_class,sizeof(DEV_CLASS));
                }
                else
                {
                    memcpy(p_device_record->dev_class,bt_ui_cb.peer_cod,sizeof(DEV_CLASS));
                }
                bcmapp_dm_process_cod(p_device_record->dev_class,&p_device_record->cod_type.svc_cls_ptr,&p_device_record->cod_type.mjr_dev_cls_ptr,&p_device_record->cod_type.mnr_dev_cls_ptr,&p_device_record->services);
                //p_device_record->services = p_device_record->services|bt_ui_cb.peer_service;
#endif
                memcpy(p_device_record->link_key, p_data->auth_cmpl.key, LINK_KEY_LEN);
                if(strlen((char*)p_data->auth_cmpl.bd_name) > 0)
                {
                    strncpy(p_device_record->name, (char*)p_data->auth_cmpl.bd_name, BTUI_DEV_NAME_LENGTH);/*add this to show bonded deviced name*/
                }
                else
                {
                    /*if the auth evt don't send name,we get it from inquiry db*/
                    if((p_inquiry_rec != NULL) && (!bdcmp(p_device_record->bd_addr, p_inquiry_rec->bd_addr)))
                    {
                        strncpy(p_device_record->name, p_inquiry_rec->name, BTUI_DEV_NAME_LENGTH);
                    }
                }
                p_device_record->link_key_present = TRUE;
                p_device_record->in_use = TRUE;
                p_device_record->bond_time = clk_read_secs();
                if(p_inquiry_rec != NULL)
                {
                    p_inquiry_rec->link_key_present = TRUE;
                    p_inquiry_rec->bond_time = p_device_record->bond_time;
                }
                //BCM_MSG_MED("p_device_record->bond_time %d",p_device_record->bond_time,0,0);
                bt_ui_store_device( p_device_record);
            }
        }

        if(! p_data->auth_cmpl.success && bt_ui_get_suspend())
        {
            BCM_MSG_MED("not send auth_cmpl because suspend",0,0,0);
            break;
        }

        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->auth_cmpl.hdr.event = BTUI_MMI_AUTH_CMPL;
            bdcpy(p_event_msg->auth_cmpl.bd_addr, p_data->auth_cmpl.bd_addr);
            p_event_msg->auth_cmpl.is_success = p_data->auth_cmpl.success;
            strncpy((char*)p_event_msg->auth_cmpl.dev_name, (char*)p_data->auth_cmpl.bd_name, BTUI_DEV_NAME_LENGTH);

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }

        break;

    case BTA_DM_AUTHORIZE_EVT:
        BCM_FUN_STAR("BTA_DM_AUTHORIZE_EVT %d",p_data->authorize.service,0,0);
#ifdef FEATURE_BLUETOOTH_APP  
        if((TRUE == bcmapp_dm_get_incall_state(FALSE)) && ((p_data->authorize.service !=BTA_HSP_SERVICE_ID) && (p_data->authorize.service !=BTA_HFP_SERVICE_ID)))
        {
            /*incall state ,auto reject authorize resp if not HFP or HSP*/
            bcmapp_dm_authorize_resp(BTA_DM_NOT_AUTH);        
        }
        /*save the services to local*/
        p_device_record = bt_ui_get_device_record(p_data->authorize.bd_addr);
        if(p_device_record)
        {
            p_device_record->services = p_device_record->services|bcmapp_dm_change_id_to_server(p_data->authorize.service);
            bt_ui_store_device( p_device_record);
        }
#endif    
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->auth_req.hdr.event = BTUI_MMI_AUTH_REQ;
            bdcpy(p_event_msg->auth_req.bd_addr, p_data->authorize.bd_addr);
            strncpy(p_event_msg->auth_req.dev_name, (char*)p_data->authorize.bd_name, BTUI_DEV_NAME_LENGTH);
            p_event_msg->auth_req.service = p_data->authorize.service;
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;

    case BTA_DM_BUSY_LEVEL_EVT:
#if ((defined BTA_AA_INCLUDED) && (BTA_AA_INCLUDED == TRUE)) || \
        ((defined BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE))
        bt_ui_codec_update_busy_level(p_data->busy_level.level);
#endif
        break;

    case BTA_DM_BOND_CANCEL_CMPL_EVT:
#if 1
        BCM_MSG_MED("BTA_DM_BOND_CANCEL_CMPL_EVT %d",p_data->bond_cancel_cmpl.result,0,0);
#else
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->auth_req.hdr.event = BTUI_MMI_BOND_CANCEL_CMPL_EVT;

            if (p_data->bond_cancel_cmpl.result == BTA_SUCCESS)
            {
                p_event_msg->bond_cancel_cmpl.result = BTUI_SUCCESS;
                BCM_MSG_MED("bond cancel complete evt, success",0,0,0);
            }
            else
            {
                p_event_msg->bond_cancel_cmpl.result = BTUI_FAIL;
                BCM_MSG_MED("bond cancel complete evt, fail",0,0,0);
            }

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
#endif
        break;
    }
}


/*******************************************************************************
**
** Function         bcmapp_dm_add_devices
**
** Description      called during startup to add the devices which are
**                  stored in NVRAM
**
** Returns          void
*******************************************************************************/
static void bcmapp_dm_add_devices(void)
{
    UINT8 i;
    BCM_MSG_MED("bcmapp_dm_add_devices",0,0,0);
    /* Update BTA with peer device information stored in NVRAM  */
    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        if(!bt_ui_device_db.device[i].in_use)
            continue;

        if(bt_ui_device_db.device[i].link_key_present)
            BTA_DmAddDevice(bt_ui_device_db.device[i].bd_addr, bt_ui_device_db.device[i].link_key,
                    bt_ui_device_db.device[i].trusted_mask, bt_ui_device_db.device[i].is_trusted,
                    bt_ui_device_db.device[i].key_type, bt_ui_device_db.device[i].peer_io_cap);
        else if (bt_ui_device_db.device[i].is_trusted)
            BTA_DmAddDevice(bt_ui_device_db.device[i].bd_addr, NULL,
                    bt_ui_device_db.device[i].trusted_mask, bt_ui_device_db.device[i].is_trusted, 
                    bt_ui_device_db.device[i].key_type,bt_ui_device_db.device[i].peer_io_cap);

    }
}

/*******************************************************************************
**
** Function         bcmapp_dm_sort_inq_db
**
** Description      checks if the given inq_res is in the inq_db
**
**
** Returns          void
*******************************************************************************/
void bcmapp_dm_sort_inq_db(UINT8 index)
{
    tBTUI_REM_DEVICE    inq_rec;
    tBTUI_REM_DEVICE    *p_inq_rec;
    UINT8               i;
    BOOLEAN             copy = FALSE;
    int                 rssi_tgt;

    BCM_MSG_MED("bcmapp_dm_sort_inq_db:%d, rssi:%d",    index, bt_ui_inq_db.remote_device[index].rssi,0);

    if(index == 0 ||
        bt_ui_inq_db.remote_device[index].rssi == BTA_DM_INQ_RES_IGNORE_RSSI)
        return;

    memcpy(&inq_rec, &(bt_ui_inq_db.remote_device[index]), sizeof(tBTUI_REM_DEVICE));
    rssi_tgt = inq_rec.rssi + inq_rec.rssi_offset;
    i=index-1;
    //while(i>=0)
    for(;;)
    {
        p_inq_rec = &(bt_ui_inq_db.remote_device[i]);
        if(p_inq_rec->rssi == BTA_DM_INQ_RES_IGNORE_RSSI ||
            (p_inq_rec->rssi + p_inq_rec->rssi_offset) < rssi_tgt)
        {
            BCM_MSG_MED("moving:%d to :%d", i, i+1,0);
            memcpy(&(bt_ui_inq_db.remote_device[i+1]), p_inq_rec, sizeof(tBTUI_REM_DEVICE));
            copy = TRUE;
        }
        else
        {
            i++;
            break;
        }
        if(i==0)
            break;
        i--;
    }
    if(copy)
    {
        BCM_MSG_MED("moving:%d to :%d", index, i,0);
        memcpy(&(bt_ui_inq_db.remote_device[i]), &inq_rec, sizeof(tBTUI_REM_DEVICE));
    }
}

/*******************************************************************************
**
** Function         bcmapp_dm_chk_inq_db
**
** Description      checks if the given inq_res is in the inq_db
**
**
** Returns          TRUE, is already in the inq_db
*******************************************************************************/
BOOLEAN bcmapp_dm_chk_inq_db(tBTA_DM_INQ_RES *p_res)
{
    tBTUI_REM_DEVICE    *p_inq_rec;
    UINT8               i;
    BOOLEAN             exist = FALSE;

    BCM_MSG_MED("bcmapp_dm_chk_inq_db:%d, rssi:%d",bt_ui_inq_db.rem_index, p_res->rssi,0);
    if(bt_ui_inq_db.rem_index)
    {
        for(i=0; i<bt_ui_inq_db.rem_index; i++)
        {
            p_inq_rec = &(bt_ui_inq_db.remote_device[i]);
            if(memcmp(p_inq_rec->bd_addr, p_res->bd_addr, BD_ADDR_LEN) == 0)
            {
                p_inq_rec->rssi = p_res->rssi;
                bcmapp_dm_sort_inq_db(i);
                exist = TRUE;
                break;
            }
        }
    }
    return exist;
}

/*******************************************************************************
**
** Function         bcmapp_dm_search_cb
**
** Description      callback to notify the completion of device search
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_dm_search_cb(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{

    tBTUI_REM_DEVICE    *p_device_rec, *p_inquiry_rec;
    tBTUI_BTA_MSG       *p_event_msg;
    tBTA_SERVICE_MASK   services = 0;
    //char msg_str[128];
#ifdef FEATURE_BLUETOOTH_APP
    UINT8               *p_eir_remote_name;
    UINT8               remote_name_len;
#endif

    BCM_MSG_MED("bcmapp_dm_search_cb %d",event,0,0);
    switch (event)
    {
    case BTA_DM_DISC_RES_EVT:
    //if(event == BTA_DM_DISC_RES_EVT)
    {
#ifdef BT_OEM_DEBUG
        char string[64];
        sprintf (string, "%02x:%02x:%02x:%02x:%02x:%02x name %s\0",
            p_data->disc_res.bd_addr[0], p_data->disc_res.bd_addr[1],
            p_data->disc_res.bd_addr[2], p_data->disc_res.bd_addr[3],
            p_data->disc_res.bd_addr[4], p_data->disc_res.bd_addr[5],
            (char *)p_data->disc_res.bd_name);
        APPL_TRACE_DEBUG1("BTA_DM_DISC_RES_EVT %s",string);
#endif
        BCM_MSG_MED("BTA_DM_DISC_RES_EVT %x %x",bt_ui_cb.search_services,p_data->disc_res.services,0);
        if(!bt_ui_cb.search_services ||  (bt_ui_cb.search_services & p_data->disc_res.services))
        {
            p_device_rec = bt_ui_get_device_record(p_data->disc_res.bd_addr);
            if(p_device_rec)
            {
                p_device_rec->services |= p_data->disc_res.services;
                if(p_data->disc_res.bd_name[0])
                    strncpy ((char *)p_device_rec->name, (char *)p_data->disc_res.bd_name, BTUI_DEV_NAME_LENGTH);

                bt_ui_store_device(p_device_rec);
            }

            p_inquiry_rec = bt_ui_get_inquiry_record(p_data->disc_res.bd_addr);
            if (p_inquiry_rec)
            {
                if(p_device_rec)
                {
                    memcpy(p_inquiry_rec, p_device_rec, sizeof(tBTUI_REM_DEVICE));
                    //p_inquiry_rec->link_key_present = FALSE ;
                }
                else
                {
                    p_inquiry_rec->in_use = TRUE;
                    p_inquiry_rec->services |= p_data->disc_res.services;
                    //memcpy ((void *)&p_inquiry_rec->bd_addr, (const void *)p_data->disc_res.bd_addr, BD_ADDR_LEN);
                    if (p_data->disc_res.bd_name[0])
                        strncpy ((char *)p_inquiry_rec->name, (char *)p_data->disc_res.bd_name, BTUI_DEV_NAME_LENGTH);
                }
            }

            //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
            if ((p_event_msg = bt_ui_get_msg()) != NULL)
            {
                p_event_msg->hdr.event = BTUI_MMI_SEARCH_DISCV_RES;
                ((tBTUI_BTA_MSG *)p_event_msg)->disc_result.services = p_data->disc_res.services;
                //bdcpy(p_event_msg->disc_result.bd_addr, p_data->disc_res.bd_addr);
                //strncpy ((char *)p_event_msg->disc_result.dev_name, (char *)p_data->disc_res.bd_name, BTUI_DEV_NAME_LENGTH);
                //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
                bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
            }
        }
        break;
    }
    case BTA_DM_DISC_CMPL_EVT:
    //else if(event == BTA_DM_DISC_CMPL_EVT)
    {
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_DISCV_CMP;
            //p_event_msg->disc_result.services= p_data->disc_res.services;
            //p_event_msg->disc_result.result= p_data->disc_res.result;
            //memcpy((void *)&p_event_msg->disc_result.bd_addr, (const void *)p_data->disc_res.bd_addr, BD_ADDR_LEN);
            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;
    }
    case BTA_DM_INQ_RES_EVT:
    //else if (event == BTA_DM_INQ_RES_EVT)
    {
        //sprintf (msg_str, "%02x:%02x:%02x:%02x:%02x:%02x\0",
        //    p_data->inq_res.bd_addr[0], p_data->inq_res.bd_addr[1],
        //    p_data->inq_res.bd_addr[2], p_data->inq_res.bd_addr[3],
        //    p_data->inq_res.bd_addr[4], p_data->inq_res.bd_addr[5]);
        //DBGPRINTF(msg_str);

        if(bcmapp_dm_chk_inq_db(&p_data->inq_res))
        {
            /* already in the inq_db */
            return;
        }

        p_inquiry_rec = &(bt_ui_inq_db.remote_device[bt_ui_inq_db.rem_index]);
        p_inquiry_rec->in_use = TRUE;
        memcpy((void *)&p_inquiry_rec->bd_addr, (const void *)p_data->inq_res.bd_addr, BD_ADDR_LEN);
        memcpy( p_inquiry_rec->dev_class, p_data->inq_res.dev_class, sizeof(DEV_CLASS));
        p_inquiry_rec->name[0] = 0;
        p_inquiry_rec->services = 0;
        p_inquiry_rec->rssi_offset = 0;
        p_inquiry_rec->rssi = p_data->inq_res.rssi;
#ifdef FEATURE_BLUETOOTH_APP
        bcmapp_dm_process_cod(p_inquiry_rec->dev_class,&p_inquiry_rec->cod_type.svc_cls_ptr,&p_inquiry_rec->cod_type.mjr_dev_cls_ptr,&p_inquiry_rec->cod_type.mnr_dev_cls_ptr,&p_inquiry_rec->services);
#endif
        if(p_data->inq_res.is_limited)
            p_inquiry_rec->rssi_offset = BTAPP_DM_LIMITED_RSSI_OFFSET;
            
#ifdef FEATURE_BLUETOOTH_APP
        if( p_data->inq_res.p_eir )
        {
            p_eir_remote_name = BTA_CheckEirData( p_data->inq_res.p_eir,
                BTM_EIR_COMPLETE_LOCAL_NAME_TYPE, &remote_name_len );
            if( !p_eir_remote_name )
            {
                p_eir_remote_name = BTA_CheckEirData( p_data->inq_res.p_eir,
                    BTM_EIR_SHORTENED_LOCAL_NAME_TYPE, &remote_name_len );
            }

            if( p_eir_remote_name )
            {
                if( remote_name_len > BTUI_DEV_NAME_LENGTH )
                    remote_name_len = BTUI_DEV_NAME_LENGTH;

                memcpy( p_inquiry_rec->name, p_eir_remote_name, remote_name_len );
                p_inquiry_rec->name[remote_name_len] = 0;
            }

            BTA_GetEirService( p_data->inq_res.p_eir, &services);
            BCM_MSG_MED("EIR BTA services = %08X", services,0,0);
        }
#endif

        p_device_rec = bt_ui_get_device_record(p_data->inq_res.bd_addr);
        if(p_device_rec)/*find same address*/
        {
#ifdef FEATURE_BLUETOOTH_APP
            /*add in 2009/02/03*/
            p_device_rec->cod_type.svc_cls_ptr = p_inquiry_rec->cod_type.svc_cls_ptr;
            p_device_rec->cod_type.mjr_dev_cls_ptr = p_inquiry_rec->cod_type.mjr_dev_cls_ptr;
            p_device_rec->cod_type.mnr_dev_cls_ptr = p_inquiry_rec->cod_type.mnr_dev_cls_ptr;
            p_device_rec->services = p_device_rec->services|p_inquiry_rec->services;
            memcpy( p_device_rec->dev_class, p_data->inq_res.dev_class, sizeof(DEV_CLASS));
            /*add in 2009/02/03*/
#endif
            if( p_inquiry_rec->name[0] )
            {
                //DBGPRINTF("EIR remote name = %s", p_inquiry_rec->name);
                strncpy( p_device_rec->name, p_inquiry_rec->name, BTUI_DEV_NAME_LENGTH );
                p_data->inq_res.remt_name_not_required = TRUE;
            }
            else if(p_device_rec->name[0])
            {
                //DBGPRINTF("stored remote name = %s", p_device_rec->name);
                strncpy( p_inquiry_rec->name,   p_device_rec->name, BTUI_DEV_NAME_LENGTH );
                /*if we know the name of the device, tell BTA not to get it */
                p_data->inq_res.remt_name_not_required = TRUE;
            }
            memcpy(p_inquiry_rec, p_device_rec, sizeof(tBTUI_REM_DEVICE));
            //p_inquiry_rec->link_key_present = FALSE ;
        }
        else if( p_inquiry_rec->name[0] )
        {
            //DBGPRINTF("EIR remote name = %s", p_inquiry_rec->name);
            p_data->inq_res.remt_name_not_required = TRUE;
        }
        else/*no name,show bt addr*/
        {
            char  name[BTUI_DEV_NAME_LENGTH] = {0};
            sprintf (name, "%02x:%02x:%02x:%02x:%02x:%02x\0",
                    p_data->inq_res.bd_addr[0], p_data->inq_res.bd_addr[1],
                    p_data->inq_res.bd_addr[2], p_data->inq_res.bd_addr[3],
                    p_data->inq_res.bd_addr[4], p_data->inq_res.bd_addr[5]);
            strncpy( p_inquiry_rec->name,  name, BTUI_DEV_NAME_LENGTH );
            //memcpy((void *)p_inquiry_rec->name, (const void *)&p_data->inq_res.bd_addr, BTUI_DEV_NAME_LENGTH);
        }
        p_inquiry_rec->services |= services;

        bt_ui_inq_db.rem_index++;
        bcmapp_dm_sort_inq_db((UINT8)(bt_ui_inq_db.rem_index-1));

        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_INQ_RES;

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;
    }
    case BTA_DM_INQ_CMPL_EVT:
    //else if(event == BTA_DM_INQ_CMPL_EVT)
    {
        //if ((p_event_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_INQ_CMP;

            //GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_event_msg);
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
        break;
    }
    case BTA_DM_SEARCH_CANCEL_CMPL_EVT:
    {
        break;
    }

    default:
        break;
    }
}

/*******************************************************************************
**
** Function         bcmapp_dm_discover_cb
**
** Description      callback to notify the completion of device service discovery
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_dm_discover_cb(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{

    tBTUI_REM_DEVICE * p_device_rec;
    tBTUI_BTA_MSG * p_event_msg;

    BCM_MSG_MED("bcmapp_dm_discover_cb %d",event,0,0);

    if(event == BTA_DM_DISC_RES_EVT)
    {

        /* At least 1 service found */
        if((p_data->disc_res.services & ~BTA_RES_SERVICE_MASK))
        {
            if ((p_device_rec = bt_ui_get_device_record(bt_ui_cb.p_selected_rem_device->bd_addr))!= NULL)
            {
                p_device_rec->services |= p_data->disc_res.services;
                bt_ui_store_device(p_device_rec);
            }
        }

        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_DEVICE_DISCV_RES;
            ((tBTUI_BTA_MSG *)p_event_msg)->disc_result.services = p_data->disc_res.services;
            ((tBTUI_BTA_MSG *)p_event_msg)->disc_result.result = p_data->disc_res.result;
            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
    }
#if BTA_DI_INCLUDED == TRUE
    else if(event == BTA_DM_DI_DISC_CMPL_EVT)
    {
        if ((p_event_msg = bt_ui_get_msg()) != NULL)
        {
            p_event_msg->hdr.event = BTUI_MMI_DISCV_CMP;
            p_event_msg->hdr.layer_specific = event;
            p_event_msg->hdr.offset         = p_data->di_disc.num_record;

            bt_ui_send_msg(BTUI_DM_MSG_TYPE, p_event_msg);
        }
    }
#endif
}


/*******************************************************************************

 $Function:        bcmapp_dm_db_get_device_info

 $Description:        gets the device record of a stored device.

 $Returns:        NULL if device not found. Pointer to a device structure if found. This data should
                 be copied if wanted to be used somewhere else.
                 If the device is not stored in Flash, the is_new flag is set => this means it is
                 a newly found device ( from an inquiry or discovery result ).

 $Arguments:        DB_ADDR of the device wanted.


*******************************************************************************/
tBTUI_REM_DEVICE * bcmapp_dm_db_get_device_info(BD_ADDR bd_addr)
{
    UINT8 i;
    BCM_MSG_MED("bcmapp_dm_db_get_device_info",0,0,0);
    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        if(    bt_ui_device_db.device[i].in_use
            && !memcmp(bt_ui_device_db.device[i].bd_addr, bd_addr, BD_ADDR_LEN))
        {
            return &(bt_ui_device_db.device[i]);
        }
    }

    /* we didn't find our device, look into the inquiry db */
    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        if(    bt_ui_inq_db.remote_device[i].in_use
            && !memcmp(bt_ui_inq_db.remote_device[i].bd_addr, bd_addr, BD_ADDR_LEN))
        {
            return &(bt_ui_inq_db.remote_device[i]);
        }
    }

    return NULL;

}

/*******************************************************************************
**
** Function         bcmapp_dm_db_get_device_list
**
** Description      gets the devices which mmets the input conditions
**
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_dm_db_get_device_list(    tBTA_SERVICE_MASK services,
                                                 tBTUI_REM_DEVICE * p_device,
                                                 UINT8*    number_of_devices,
                                                 BOOLEAN new_only)
{
    UINT8 i;

    *number_of_devices = 0;
    if( services == 0 )
        services = BTA_ALL_SERVICE_MASK;

    BCM_MSG_MED("bt_ui_get_device_list - searched services = %x", services ,0,0);

    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        if(new_only == FALSE )
        {
            /* first, get the stored devices - only if not new_only asked */
            if(    bt_ui_device_db.device[i].in_use &&
                (( bt_ui_device_db.device[i].services & services)||(services==BTA_ALL_SERVICE_MASK))  )
            {
                memcpy(&p_device[*number_of_devices], &(bt_ui_device_db.device[i]), sizeof(tBTUI_REM_DEVICE));
                (*number_of_devices)++;
            }
        }


        /* then, get the new devices */
        BCM_MSG_MED("bt_ui_get_device_list - device services = %x",bt_ui_inq_db.remote_device[i].services,0,0);

        if( (bt_ui_inq_db.remote_device[i].in_use) && ( (bt_ui_inq_db.remote_device[i].services & services)||(services==BTA_ALL_SERVICE_MASK)) )
        {
            if ( &p_device[*number_of_devices] == NULL  )
            {
                BCM_MSG_MED("pp_device[*number_of_devices] is NULL!",0,0,0);
            }
            else if ( &(bt_ui_inq_db.remote_device[i]) == NULL  )
            {
                BCM_MSG_MED("&(bt_ui_inq_db[i]) is NULL!",0,0,0);
            }
            else
                memcpy(&p_device[*number_of_devices], &(bt_ui_inq_db.remote_device[i]), sizeof(tBTUI_REM_DEVICE));

            (*number_of_devices) ++;
        }

    }



    BCM_MSG_MED("%i devices into inq db",(*number_of_devices),0,0);

    if(*number_of_devices == 0 )
        return BTUI_FAIL;

    return BTUI_SUCCESS;
}

#ifndef AEE_SIMULATOR
#ifdef FEATURE_CS09_BLUETOOTH_APP
//begin,liuhaijun,2009/01/07
extern void BTA_EnterTestMode(void);
extern void hs_bt_wakeup(boolean bIsWakeup);

void bcmapp_dm_enter_bt_testmode(void)
{
#if 0
    	HCILP_WakeupBTDevice(NULL);
    	BTA_HCILP_Enable(FALSE);
	btapp_dm_enable_dut();
#else	
	hs_bt_wakeup(TRUE);

	BTA_EnterTestMode();
#endif
}
//end,liuhaijun,2009/01/07
#endif
#ifdef FEATURE_BLUETOOTH_APP
void bcmapp_set_bt_test_mode(void)
{
    HCILP_WakeupBTDevice(NULL);
    BTA_HCILP_Enable(FALSE);
    btapp_dm_enable_dut();
}
#endif
#endif
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
#ifndef AEE_SIMULATOR
#include "sio.h"
sio_bitrate_type get_bt_bitrate_handle()
{
#if defined(FEATURE_UART_TCXO_CLK_FREQ) && defined(FEATURE_UART_RATE1)
  return SIO_BITRATE_460800;
#else
  return SIO_BITRATE_ILLEGAL_10;
#endif
//#ifdef FEATURE_UART_TCXO_CLK_FREQ
//  return SIO_BITRATE_230400;
//#else
//  return SIO_BITRATE_ILLEGAL_2;
//#endif
}
#endif
#endif
/*===========================================================================

FUNCTION
  bt_sd_parse_class_of_device

DESCRIPTION
  Extracts from the class of device its 3 components: service class, major
  device class, and minor device class.

===========================================================================*/
void bcmapp_dm_process_cod( 
  DEV_CLASS cod_bytes, 
  uint16*            svc_cls_ptr,
  uint16*            mjr_dev_cls_ptr,
  uint16*            mnr_dev_cls_ptr,
  tBTA_SERVICE_MASK *service
)
{
  BCM_MSG_MED("bcmapp_dm_process_cod= %2x %2x %2x", cod_bytes[0],cod_bytes[1],cod_bytes[2]);
  if(service == NULL)
  {
      return;
  }
#if 1 /*broadcom use cod[0] to indicate device type */
  /* Extract service class */
  if(svc_cls_ptr != NULL)
  {
      *svc_cls_ptr     = BT_SERVICE_CLASS_UNKNOWN;
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_INFORMATION )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_INFORMATION;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_TELEPHONY;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_AUDIO )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_AUDIO;
        *service |= BTA_HSP_SERVICE_MASK;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_OBJECT_TRANSFER;
        *service |= BTA_OPP_SERVICE_MASK;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_CAPTURING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_CAPTURING;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_RENDERING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_RENDERING;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_NETWORKING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_NETWORKING;
      }
      if (cod_bytes[0] & BT_SD_MAJOR_SERVICE_CLASS_POSITIONING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_POSITIONING;
      }
      if (cod_bytes[1] & 0x20 )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE;
      }
      BCM_MSG_MED("bcm get svc_cls_ptr= %x", *svc_cls_ptr ,0,0);
  }
  /* Extract major and minor device class */
  if((mjr_dev_cls_ptr != NULL)&&(mnr_dev_cls_ptr != NULL))
  {
      *mjr_dev_cls_ptr = BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
      *mnr_dev_cls_ptr = BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;

      *mjr_dev_cls_ptr = (cod_bytes[1] & 0x1F) << 8;
      switch( *mjr_dev_cls_ptr )
      {
        case BT_SD_MAJOR_DEVICE_CLASS_COMPUTER:
        case BT_SD_MAJOR_DEVICE_CLASS_PHONE:
        case BT_SD_MAJOR_DEVICE_CLASS_AUDIO:
          *mnr_dev_cls_ptr =
            ((cod_bytes[2] >> 2) & 0x3F) | *mjr_dev_cls_ptr;
          break;
        case BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT:
          *mnr_dev_cls_ptr =
            ((cod_bytes[2] >> 5) & 0x07) | *mjr_dev_cls_ptr;
          break;
        case BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL:
          break;
      }
      BCM_MSG_MED("bcm get mjr/mnr dev_cls_ptr= %x %x %x", *mjr_dev_cls_ptr ,*mnr_dev_cls_ptr,*service);
  }
#else /*qualcomm use cod[2] to indicate device type */
  /* Extract service class */
  if(svc_cls_ptr != NULL)
  {
      *svc_cls_ptr     = BT_SERVICE_CLASS_UNKNOWN;
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_INFORMATION )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_INFORMATION;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_TELEPHONY )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_TELEPHONY;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_AUDIO )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_AUDIO;
        *service |= BTA_HSP_SERVICE_MASK;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_OBJECT_TRANSFER )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_OBJECT_TRANSFER;
        *service |= BTA_OPP_SERVICE_MASK;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_CAPTURING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_CAPTURING;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_RENDERING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_RENDERING;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_NETWORKING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_NETWORKING;
      }
      if (cod_bytes[2] & BT_SD_MAJOR_SERVICE_CLASS_POSITIONING )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_POSITIONING;
      }
      if (cod_bytes[1] & 0x20 )
      {
        *svc_cls_ptr |= BT_SERVICE_CLASS_LIMITED_DISCOVERABLE_MODE;
      }
      BCM_MSG_MED("bcm get svc_cls_ptr= %x", *svc_cls_ptr ,0,0);
  }
  /* Extract major and minor device class */
  if((mjr_dev_cls_ptr != NULL)&&(mnr_dev_cls_ptr != NULL))
  {
      *mjr_dev_cls_ptr = BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
      *mnr_dev_cls_ptr = BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;

      *mjr_dev_cls_ptr = (cod_bytes[1] & 0x1F) << 8;
      switch( *mjr_dev_cls_ptr )
      {
        case BT_SD_MAJOR_DEVICE_CLASS_COMPUTER:
        case BT_SD_MAJOR_DEVICE_CLASS_PHONE:
        case BT_SD_MAJOR_DEVICE_CLASS_AUDIO:
          *mnr_dev_cls_ptr =
            ((cod_bytes[0] >> 2) & 0x3F) | *mjr_dev_cls_ptr;
          break;
        case BT_SD_MAJOR_DEVICE_CLASS_LAN_ACCESS_POINT:
          *mnr_dev_cls_ptr =
            ((cod_bytes[0] >> 5) & 0x07) | *mjr_dev_cls_ptr;
          break;
        case BT_SD_MAJOR_DEVICE_CLASS_PERIPHERAL:
          break;
      }
      BCM_MSG_MED("bcm get mjr/mnr dev_cls_ptr= %x %x %x", *mjr_dev_cls_ptr ,*mnr_dev_cls_ptr,*service);
  }
#endif
}

tBTA_SERVICE_MASK bcmapp_dm_change_id_to_server(tBTA_SERVICE_ID id)
{
    BCM_MSG_MED("bcmapp_dm_change_id_to_server %d",id,0,0);
    switch(id)
    {
        case BTA_OPP_SERVICE_ID:
            return BTA_OPP_SERVICE_MASK;

        case BTA_HSP_SERVICE_ID:
            return BTA_HSP_SERVICE_MASK;

        case BTA_HFP_SERVICE_ID:
            return BTA_HFP_SERVICE_MASK;

        case BTA_FTP_SERVICE_ID:
            return BTA_FTP_SERVICE_MASK;

        default:
            return 0;
    }
}
extern boolean is_g_sportsapp_pedometer_flag(void);
extern boolean is_g_sportsapp_pedometer_bground_flag(void);
extern boolean get_recorder_play_state(void);
extern boolean get_fm_play_state(void);
boolean bcmapp_dm_get_incall_state(boolean b_use_sd)
{
#ifndef AEE_SIMULATOR
    db_items_value_type db;
    db_get(DB_IN_USE, &db);
    if(TRUE == db.in_use)
    {
        return TRUE;
    }
    
    if(DEVICE_CAMERA_STATE_ON == GetDeviceState(DEVICE_TYPE_CAMERA))
    {
        return TRUE;
    }

    if(DEVICE_MP4_STATE_ON == GetDeviceState(DEVICE_TYPE_MP4))
    {
        return TRUE;
    }

    if(MediaGallery_CheckUdiskStat())
    {
        return TRUE;
    }
    //BCM_MSG_MED("bcmapp_dm_get_incall_state %d %d %d",is_g_sportsapp_pedometer_flag(),is_g_sportsapp_pedometer_bground_flag(),get_recorder_play_state());
    if(is_g_sportsapp_pedometer_flag() && !is_g_sportsapp_pedometer_bground_flag())
    {
        return TRUE;
    }
    
    if(get_recorder_play_state() || get_fm_play_state() )
    {
        return TRUE;
    }
    
    if(b_use_sd)
    {
	    if(!IsMp3PlayerStatusNone())
	    {
	        return TRUE;
	    }
    }
    else
    {
        if(IsMp3PlayerStatusOnBG())
        {
            return TRUE;
        }
    }
    return FALSE;
#else
	 return FALSE;
#endif
}
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
#ifndef AEE_SIMULATOR
static char * bcmapp_dm_get_tpye(char * name)
{
#ifdef FEATURE_BT_OPP_FILE_TYPE
    bt_file_type file_type = BT_FILE_TYPE_OTHER;
    if(name)/*get type*/
    {
        char *     ext = NULL;
        ext = strchr(name,'.');
        if(NULL == ext)
        {
            return MG_PICTURES_FOLDER;
        }
        /*PICTRUE*/
        if (0 == strncmp(ext,(".jpg"),4))
        {
            file_type = BT_FILE_TYPE_PICTRUE;
        }
        else if (0 == strncmp(ext,(".bmp"),4))
        {
            file_type = BT_FILE_TYPE_PICTRUE;
        }
        else if (0 == strncmp(ext,(".jpeg"),5))
        {
            file_type = BT_FILE_TYPE_PICTRUE;
        }
        else if (0 == strncmp(ext,(".gif"),4))
        {
            file_type = BT_FILE_TYPE_PICTRUE;
        }
        else if (0 == strncmp(ext,(".png"),4))
        {
            file_type = BT_FILE_TYPE_PICTRUE;
        }
        /*MUSIC*/
        else if (0 == strncmp(ext,(".mp3"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".wma"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".wav"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".mid"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".midi"),5))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".mmf"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".qcp"),4))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        else if (0 == strncmp(ext,(".qcelp"),6))
        {
            file_type = BT_FILE_TYPE_MUSIC;
        }
        /*VIDEO*/
        else if (0 == strncmp(ext,(".mp4"),4))
        {
            file_type = BT_FILE_TYPE_VIDEO;
        }
        /*OTHER*/
        else
        {
            file_type = BT_FILE_TYPE_OTHER;
        }
        BCM_MSG_MED("receive file type %d",file_type,0,0);
        switch(file_type)
        {
            case BT_FILE_TYPE_PICTRUE:
                return MG_PICTURES_FOLDER;

            case BT_FILE_TYPE_MUSIC:
                return MG_MUSIC_FOLDER;

            case BT_FILE_TYPE_VIDEO:
                return MG_VIDEOS_FOLDER;

            default:
            case BT_FILE_TYPE_OTHER:
                return MG_PICTURES_FOLDER;
        }
    }
    return NULL;
#else
    return "receive";
#endif
}
#endif
#define KEEP_FREE_MEMERY_FOR_SYSTEM_MALLOC 100*1024  //keep 100k memery for system malloc
byte bcmapp_dm_get_stroe_path(uint32 size, char *path,char * name)
{
#ifdef AEE_SIMULATOR
    strcpy(path,"fs:/shared/");
    return BT_SAVE_TO_PHONE;
#else
    uint32 pdwTotal = 0;
    uint32 pdwFree = 0;
    char * name_type = NULL;
    if(NULL == path)
    {
        return BT_SAVE_TO_NONE;
    }

    name_type = bcmapp_dm_get_tpye(name);
    if(NULL == name_type)
    {
        path[0] = 0;
        return BT_SAVE_TO_NONE;
    }


    if(
#ifndef FEATURE_SET_REV_FILE_TO_PHONE_FIRST
        !bt_ui_cfg.b_sdcard_state &&
#endif
        SUCCESS == OEMFS_GetFreeSpaceEx(MG_PHONE_ROOTDIR, &pdwTotal, &pdwFree))
    {
        BCM_MSG_MED("phone size %x %x %x",size,pdwTotal,pdwFree);
        pdwFree = pdwFree - KEEP_FREE_MEMERY_FOR_SYSTEM_MALLOC;//keep 100k memery for system malloc
        if(size < pdwFree)
        {
            //strcpy(path,"/brew/hsmm/");
            bcmapp_dm_change_brewaddr_to_fs(MG_PHONE_ROOTDIR,path);
            strcat(path,name_type);
            strcat(path,"/");
            return BT_SAVE_TO_PHONE;
        }
    }
    if(bt_ui_cfg.b_sdcard_state)
    {
        pdwTotal = 0;
        pdwFree = 0;
        if(SUCCESS == OEMFS_GetFreeSpaceEx(MG_MASSCARD_ROOTDIR, &pdwTotal, &pdwFree))
        {
            BCM_MSG_MED("card size %x %x %x",size,pdwTotal,pdwFree);
            if(size < pdwFree)
            {
                //strcpy(path,"/brew/card0/");
                bcmapp_dm_change_brewaddr_to_fs(MG_MASSCARD_ROOTDIR,path);
                strcat(path,name_type);
                strcat(path,"/");
                return BT_SAVE_TO_CARD;
            }
        }
    }
    path[0] = 0;
    return BT_SAVE_TO_NONE;
#endif
}

boolean bcmapp_dm_change_brewaddr_to_fs(char * brewaddr,char * fsaddr )
{
#ifndef FEATURE_USE_OEM_FILEMGR
    int len = 0;
    if(brewaddr == NULL ||fsaddr == NULL )
    {
        return FALSE;
    }
    //strcpy(fsaddr, brewaddr);
    //brewaddr -> fs:/opp/
    //fsaddr     -> /brew/opp/
    len = strlen(AEEFS_ROOT_DIR);
    strcpy(fsaddr, bt_ui_cfg.root_path);
    strcat(fsaddr, brewaddr + len);

#else
    //int len = 0;
    if(brewaddr == NULL ||fsaddr == NULL )
    {
        return FALSE;
    }
    strcpy(fsaddr, brewaddr);
    //brewaddr -> fs:/opp/
    //fsaddr     -> /brew/opp/
    //len = strlen(AEEFS_ROOT_DIR);
    //strcpy(fsaddr, bt_ui_cfg.root_path);
    //strcat(fsaddr, brewaddr + len);
#endif
    return TRUE;
}
#endif

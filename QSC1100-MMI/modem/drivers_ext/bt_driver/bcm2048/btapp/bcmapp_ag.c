/****************************************************************************
**
**  Name:          bcmapp_ag.c
**
**  Description:   Contains application code for audio gateway
**
**
**  Copyright (c) 2002-2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
******************************************************************************/

#include "bt_target.h"
#include "gki.h"

#if( defined BTA_AG_INCLUDED ) && ( BTA_AG_INCLUDED == TRUE )

#include "bta_api.h"
#include "bta_ag_api.h"
#include "bt_ui.h"
#include "bcmapp_ag.h"
#include "bcmapp_dm.h"
#include "bd.h"
//#include <stdio.h>
//#include <string.h>
#include "bt_ui_ht_common.h"
#ifndef AEE_SIMULATOR
#include "bt_bcm.h"
#include "OEMCFGI.h"
#include "OEMConfig.h"
#endif
#ifndef BTUI_AG_DEBUG
#define BTUI_AG_DEBUG   FALSE
#endif
enum
{
    BTUI_AG_CLCC_STATE_ACTIVE,
    BTUI_AG_CLCC_STATE_HELD,
    BTUI_AG_CLCC_STATE_DIALING,
    BTUI_AG_CLCC_STATE_ALERTING,
    BTUI_AG_CLCC_STATE_INCOMING,
    BTUI_AG_CLCC_STATE_WAITING
};

enum
{
    BTUI_AG_CLCC_MPARTY_FALSE,
    BTUI_AG_CLCC_MPARTY_TRUE
};
#if BTUI_AG_DEBUG == TRUE
static char *bcmapp_ag_evt_str(tBTA_AG_EVT event);
#endif
static void bcmapp_ag_platform_ag_event(tBTA_AG_EVT event, tBTA_AG *p_data);
//static void bcmapp_ag_reset_call_table(void);
//static void bcmapp_ag_handle_call_event(UINT16 handle, UINT8 event, UINT8 app_id);
static void bcmapp_ag_disable_vr(UINT8 app_id);
//static void bcmapp_ag_get_call_stats(UINT8 *num_active, UINT8 *num_held);

/* BTUI AG main control block */
static tBTUI_AG_CB bcm_ag_cb = {0};
static const char * bcm_ag_ind_default = "0,0,1,3,0,3,0";
//static tBTUI_CALL_DATA     bcm_ag_call_table[BTUI_MAX_CALLS] =
//{
//    {0, "", 0, BTUI_CALL_STATE_NONE, 0, "8581231230",0},
//    {0, "", 0, BTUI_CALL_STATE_NONE, 0, "8581231231",0},
//    {0, "", 0, BTUI_CALL_STATE_NONE, 0, "8581231232",0}
//};

// Set the pcm configure of different path, TRUE using 848 configure, FALSE using 6020 configure.
extern void bta_ag_setpath(BOOLEAN sco_path);

/*******************************************************************************
**
** Function         bcmapp_ag_get_ag_call_table
**
** Description      get the pointer of call table.
**
**
** Returns          pointer of call table.
*******************************************************************************/
//tBTUI_CALL_DATA * bcmapp_ag_get_ag_call_table(void)
//{
//    return bcm_ag_call_table;
//}

/*******************************************************************************
**
** Function         bcmapp_ag_audio_open
**
** Description      Set the vc848 audio output channel.
**
**
** Returns          void.
*******************************************************************************/
__inline void  bcmapp_ag_set_vc848_channel(vc_audio_channel eChannel)
{
    static vc_union_type vc_data;
    vc_data.parm.p1 = eChannel;
        ///*VC_AUDIO_CHN_SPEAKER;*/ VC_AUDIO_CHN_PCM; /*VC_AUDIO_CHN_EARPHONE;*/
    
   VC_DeviceControl(VC_ITM_SET_AUDIO_CHANNEL_I, VC_FUNC_SET_PARM, &vc_data);
}

/*******************************************************************************
**
** Function         bcmapp_ag_get_ag_cb
**
** Description      get the ag control block.
**
**
** Returns          pointer of the ag control block.
*******************************************************************************/
tBTUI_AG_CB * bcmapp_ag_get_ag_cb(void)
{
    return &bcm_ag_cb;
}

#ifdef PLAY_MUSIC_USING_BT_AG

/*******************************************************************************
**
** Function         bcmapp_is_ag_connected
**
** Description      Check there is bt devices connected.
**
**
** Returns         app id of the opened ag connection.
*******************************************************************************/
UINT8 bcmapp_is_ag_connected(void)
{
    UINT8 app_id;

    for (app_id = 0; app_id < BTUI_AG_NUM_APP; app_id++)
    {
        if(bcm_ag_cb.app[app_id].is_open)
            return app_id;
    }

    return BCMAPP_AG_BAD_APPID;
}

/*******************************************************************************
**
** Function         bcmapp_ag_audio_open
**
** Description     open sco of the connected ag handle.
**
**
** Returns          the error code.     
*******************************************************************************/
int bcmapp_ag_audio_open(UINT8 app_id, PFN_AG_NOTIFY pfnNotify, void *pCxt, 
                                              OEMSound_Volume_Type eVolume, BOOLEAN bUse848)
{
    // Check the app_id.
    if(app_id >= BTUI_AG_NUM_APP)
        return BCMAPP_AG_BADPARAM;
    
    if(BTA_AG_HANDLE_NONE == bcm_ag_cb.app[app_id].handle)
        return BCMAPP_AG_BADSTATE;   // The ag connection is not open.

    bcm_ag_cb.app[app_id].pfnNotfiy = pfnNotify;
    bcm_ag_cb.app[app_id].pCxt = pCxt;
    bcm_ag_cb.app[app_id].eVolume = eVolume;
    bcm_ag_cb.app[app_id].use_848 = bUse848;
    if(bcm_ag_cb.app[app_id].pISound)    
    {
        ISOUND_Release(bcm_ag_cb.app[app_id].pISound);
        bcm_ag_cb.app[app_id].pISound = NULL;
    }
        
    // Disconnect the opened sco.
    if(bcm_ag_cb.app[app_id].is_audio_open) 
    {
        bcm_ag_cb.app[app_id].need_reconnect = TRUE;
        BTA_AgAudioClose(bcm_ag_cb.app[app_id].handle);
        return BCMAPP_AG_SUCCESS;
    }

    // Check if we need to set the 848 output mode as pcm or i2c
    if(bUse848)
    {
        bcmapp_ag_set_vc848_channel(VC_AUDIO_CHN_PCM);
    }

    bta_ag_setpath(bUse848);
    BTA_AgAudioOpen(bcm_ag_cb.app[app_id].handle);
    return BCMAPP_AG_SUCCESS;
}

/*******************************************************************************
**
** Function         bcmapp_ag_audio_open
**
** Description     close sco of the connected ag handle.
**
**
** Returns          the error code.     
*******************************************************************************/
int bcmapp_ag_audio_close(UINT8 app_id)
{
    // Check the app_id.
    if(app_id>=BTUI_AG_NUM_APP)
        return BCMAPP_AG_BADPARAM;

    // Check if we need set the 848 output mode as i2c
    if(bcm_ag_cb.app[app_id].use_848)
    {
        bcmapp_ag_set_vc848_channel(VC_AUDIO_CHN_SPEAKER);        
    }

    // Switch the sound deivice if need.
    if(bcm_ag_cb.app[app_id].pISound)    
    {
        // Release the ISOUND can switch to default sound device.
        ISOUND_Release(bcm_ag_cb.app[app_id].pISound);
        bcm_ag_cb.app[app_id].pISound = NULL;
    }
    
    bcm_ag_cb.app[app_id].pfnNotfiy = NULL;
    bcm_ag_cb.app[app_id].pCxt = NULL;
    bcm_ag_cb.app[app_id].need_reconnect = FALSE;

    if(BTA_AG_HANDLE_NONE != bcm_ag_cb.app[app_id].handle)
        BTA_AgAudioClose(bcm_ag_cb.app[app_id].handle);
    
    return BCMAPP_AG_SUCCESS;
}

/*******************************************************************************
**
** Function         bcmapp_ag_set_device
**
** Description     Set parameter for playing music using bt device.
**
**
** Returns          error code.
*******************************************************************************/
int bcmapp_ag_set_device(UINT8 app_id)
{
    tBTUI_AG_APP *p_btui_app;
    AEESoundInfo  soundStuff;

    p_btui_app = &bcm_ag_cb.app[app_id];
    
    if(p_btui_app->use_848)
    {
        // Set the volume;
        
        return BCMAPP_AG_SUCCESS;
    }

    if(NULL == p_btui_app->pISound)
    {
        if(ISHELL_CreateInstance(bt_ui_get_shell(), AEECLSID_SOUND, (void **)&(p_btui_app->pISound)))
            return BCMAPP_AG_FAILURE;
    }

    // Set the sound device info.
    soundStuff.eDevice = AEE_SOUND_DEVICE_BT_HEADSET;
    soundStuff.eMethod = AEE_SOUND_METHOD_MIDI;
    soundStuff.eAPath = AEE_SOUND_APATH_BOTH;
    soundStuff.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
    soundStuff.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

    // Switch the sound device to bt device.
    ISOUND_Set(p_btui_app->pISound, &soundStuff);
    ISOUND_SetDevice(p_btui_app->pISound);

    // Set the volume, pay attention to the volume can't be 0.
    ISOUND_SetVolume(p_btui_app->pISound, 
        GET_ISOUND_VOL_LEVEL(p_btui_app->eVolume ? p_btui_app->eVolume : OEMSOUND_1ST_VOL));

    return BCMAPP_AG_SUCCESS;
}

/*******************************************************************************
**
** Function         bcmapp_ag_notify
**
** Description      notify ui the ag event. 
**
**
** Returns          void.
*******************************************************************************/
void bcmapp_ag_notify(void *pCxt)
{
    UINT8 app_id = (UINT8)((UINT32)pCxt);

    if(BCMAPP_AG_SUCCESS == bcm_ag_cb.app[app_id].err_code)
    {
        bcmapp_ag_set_device(app_id);        
    }
    else
    {
        bcmapp_ag_audio_close(app_id);
    }
    
    if(bcm_ag_cb.app[app_id].pfnNotfiy)
        bcm_ag_cb.app[app_id].pfnNotfiy(bcm_ag_cb.app[app_id].pCxt, bcm_ag_cb.app[app_id].err_code);
}

#endif

/*******************************************************************************
**
** Function         bcm_ag_cback
**
** Description      AG callback from BTA
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_cback(tBTA_AG_EVT event, tBTA_AG *p_data)
{
    tBTUI_BTA_MSG    *p_msg;
    UINT16            handle = 0;
    UINT8             app_id  = 0;

    if (p_data)
    {
        handle = p_data->hdr.handle;
        app_id = p_data->hdr.app_id;
    }

    BCM_FUN_STAR("bcmapp_ag_cback: Event %d %d %d", event,handle,app_id);

    switch (event)
    {
        case BTA_AG_ENABLE_EVT:
            break;

        case BTA_AG_REGISTER_EVT:
            bcm_ag_cb.app[app_id].handle = handle;
            break;

        case BTA_AG_OPEN_EVT:
            BCM_MSG_MED("BTA_AG_OPEN_EVT:%d :%d",  p_data->open.service_id, p_data->open.status,0);
            if (p_data->open.status == BTA_AG_SUCCESS)
            {
                bcm_ag_cb.app[app_id].is_open = TRUE;
                bcm_ag_cb.app[app_id].bvra_active = FALSE; /* Voice Recognition starts off */
                bcm_ag_cb.app[app_id].service = p_data->open.service_id;
                bdcpy(bcm_ag_cb.app[app_id].bd_addr, p_data->open.bd_addr);
            }

            bcm_ag_cb.call_handle = handle;

#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
            bcm_ag_cb.cur_handle = handle;

            if (bcm_ag_cb.play_fm == FALSE)
#endif
            {
                if ((p_msg = bt_ui_get_msg()) != NULL)
                {
                    p_msg->open.hdr.event = BTUI_MMI_CONN_UP;
                    bdcpy(p_msg->open.bd_addr, p_data->open.bd_addr);
                    p_msg->open.service = p_data->open.service_id;
                    p_msg->open.status = p_data->open.status;
                    bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
                }
            }
            break;

        case BTA_AG_CLOSE_EVT:
            bcm_ag_cb.app[app_id].is_open = FALSE;
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
            if (bcm_ag_cb.play_fm )
            {
                if (!bcm_ag_cb.app[app_id].is_audio_open)
                    bcm_ag_cb.play_fm = FALSE;
            }
            else
#endif
            if ((p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->close.hdr.event = BTUI_MMI_CONN_DOWN;
                p_msg->close.service = bcm_ag_cb.app[app_id].service;
                bdcpy(p_msg->close.bd_addr,bcm_ag_cb.app[app_id].bd_addr);
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
            break;

        case BTA_AG_CONN_EVT:
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
           if ( bcm_ag_cb.play_fm)
           {
                bcm_ag_cb.sco_owner = 2;

                BCM_MSG_MED("sending BTUI_MMI_AG_FM_OPEN",0,0,0);
                BTA_AgAudioOpen(bcm_ag_cb.call_handle);
           }
#endif

            break;

        case BTA_AG_AUDIO_OPEN_EVT:
            bcm_ag_cb.app[app_id].is_audio_open = TRUE;

#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
            if ( bcm_ag_cb.play_fm && bcm_ag_cb.sco_owner == 2 &&
                (p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_FM_BT_OPEN;
                p_msg->hdr.layer_specific = handle;
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
            else
#endif
#ifdef PLAY_MUSIC_USING_BT_AG
            if(bcm_ag_cb.app[app_id].pfnNotfiy)
            {
                // check if the sco is opened.
                // if(p_data->conn.
                bcm_ag_cb.app[app_id].err_code = BCMAPP_AG_SUCCESS;
                memset(&bcm_ag_cb.app[app_id].cb, 0, sizeof(AEECallback));
                CALLBACK_Init(&bcm_ag_cb.app[app_id].cb, bcmapp_ag_notify, (void *)app_id);
                ISHELL_Resume(bt_ui_get_shell(), &bcm_ag_cb.app[app_id].cb);
            }
            else
#endif
            if ((p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_AG_AUDIO_OPEN;
                p_msg->hdr.layer_specific = handle;
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
            break;

        case BTA_AG_AUDIO_CLOSE_EVT:
            /* If voice recognition is enabled send the unsolicited disable to peer
               because this could have closed because of a SCO transfer */
            bcmapp_ag_disable_vr(app_id);

            bcm_ag_cb.app[app_id].is_audio_open = FALSE;
#ifdef PLAY_MUSIC_USING_BT_AG
            if(bcm_ag_cb.app[app_id].need_reconnect)
            {
                bcm_ag_cb.app[app_id].need_reconnect = FALSE;
                bta_ag_setpath(bcm_ag_cb.app[app_id].use_848);
                BTA_AgAudioOpen(bcm_ag_cb.app[app_id].handle);
            }
            else
            {
#endif
            if ((p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.layer_specific = handle;
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
                p_msg->hdr.offset           = bcm_ag_cb.play_fm;
#endif
                p_msg->hdr.event = BTUI_MMI_AG_AUDIO_CLOSE;
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
#if (defined BTA_FM_INCLUDED &&(BTA_FM_INCLUDED == TRUE))
            if (!bcm_ag_cb.app[app_id].is_open)
                bcm_ag_cb.play_fm = FALSE;
#endif
#ifdef PLAY_MUSIC_USING_BT_AG
            }
#endif
            break;

        default:
            bcmapp_ag_platform_ag_event(event, p_data);
            break;
    }
}



/*******************************************************************************
**
** Function         bcmapp_ag_enable
**
** Description      Enable AG service
**
**
** Returns          void
*******************************************************************************/
tBTA_SERVICE_MASK bcmapp_ag_enable(void)
{

    tBTA_SERVICE_MASK   services;
    char * p_service_names[2];
    /* get the supported services, In real phones, both HFP and HSP services are always supported
       The configuration option is mainly for test purposes */
    services = ( bt_ui_cfg.supported_services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK) );
    BCM_FUN_STAR("bcmapp_ag_enable %x %d %x", services,bt_ui_cfg.ag_instances,bt_ui_cfg.ag_features);
    if (services)
    {
        /* Enable Audio gateway service.
           BTA_AG_PARSE -> AT commands are parsed by BTA, and application
           receives events */
        BTA_AgEnable(BTA_AG_PARSE, bcmapp_ag_cback);

        /* service names received by the peer device during service discovery */
        p_service_names[0] = bt_ui_cfg.hsag_service_name;
        p_service_names[1] = bt_ui_cfg.hfag_service_name;

        /* BTA supports 2 AG instances at the same time,
           There can be two simulataneous AG connections,
           for example simultaneous service level connection to a
           headset and a car kit */
        if (bt_ui_cfg.ag_instances > 0)
        {
            BTA_AgRegister(services, bt_ui_cfg.ag_security, (tBTA_AG_FEAT) bt_ui_cfg.ag_features,
                           p_service_names, BTUI_AG_ID_1);
        }
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
        if (bt_ui_cfg.ag_instances > 1)
        {
            BTA_AgRegister(services, bt_ui_cfg.ag_security, (tBTA_AG_FEAT) bt_ui_cfg.ag_features,
                           p_service_names, BTUI_AG_ID_2);
        }
#endif
    }

    return services;
}

void bcmapp_ag_disable(void)
{
    BTA_AgDisable();
    //BTA_AgDeregister()
}
/*******************************************************************************
**
** Function         bcmapp_ag_close_conn
**
** Description      Close the AG service connection specified by the app_id
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_close_conn(UINT8 app_id)
{
    BCM_FUN_STAR("bcmapp_ag_close_conn %d %d", app_id,bcm_ag_cb.app[app_id].handle,0);
    BTA_AgClose(bcm_ag_cb.app[app_id].handle);

}

/*******************************************************************************
**
** Function         bcmapp_ag_change_spk_volume
**
** Description      Change speaker volume on the connection specified by the app_id
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_change_spk_volume(UINT8 app_id, BOOLEAN increase)
{
    tBTA_AG_RES_DATA    data;
    BCM_FUN_STAR("bcmapp_ag_change_spk_volume %d %d", app_id,increase,0);
    if(increase)
    {
        /* the upper limit for volume is 15 */
        if(bcm_ag_cb.app[app_id].spk_vol < 15)
            bcm_ag_cb.app[app_id].spk_vol++;

        data.num = bcm_ag_cb.app[app_id].spk_vol;

    }
    else
    {
        /* the lower limit for volume is 0 */
        if(bcm_ag_cb.app[app_id].spk_vol > 0)
            bcm_ag_cb.app[app_id].spk_vol--;

        data.num = bcm_ag_cb.app[app_id].spk_vol;
        data.ok_flag = BTA_AG_OK_DONE;
    }

    BTA_AgResult(bcm_ag_cb.app[app_id].handle, BTA_AG_SPK_RES, &data);

}

/*******************************************************************************
**
** Function         bcmapp_ag_change_spk_volume
**
** Description      Change MIC volume on the connection specified by the app_id
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_change_mic_volume(UINT8 app_id,BOOLEAN increase)
{
    tBTA_AG_RES_DATA    data;
    BCM_FUN_STAR("bcmapp_ag_change_mic_volume %d %d", app_id,increase,0);
    if(increase)
    {
        /* the upper limit for volume is 15 */
        if(bcm_ag_cb.app[app_id].mic_vol < 15)
            bcm_ag_cb.app[app_id].mic_vol++;

        data.num = bcm_ag_cb.app[app_id].mic_vol;
        data.ok_flag = BTA_AG_OK_DONE;
    }
    else
    {
        /* the upper limit for volume is 15 */
        if(bcm_ag_cb.app[app_id].mic_vol > 0)
            bcm_ag_cb.app[app_id].mic_vol--;

        data.num = bcm_ag_cb.app[app_id].mic_vol;


    }

    BTA_AgResult(bcm_ag_cb.app[app_id].handle, BTA_AG_MIC_RES, &data);


}


/*******************************************************************************
**
** Function         bcmapp_ag_set_audio_device_authorized
**
** Description      Bond with the device
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_set_audio_device_authorized (tBTUI_REM_DEVICE * p_device_rec)
{


    /* update BTA with this information.If a device is set as trusted, BTA will
    not ask application for authorization, if authorization is required for
    AG connections */
    BCM_FUN_STAR("bcmapp_ag_set_audio_device_authorized", 0,0,0);
    p_device_rec->is_trusted = TRUE;
    p_device_rec->trusted_mask |= BTA_HSP_SERVICE_MASK |BTA_HFP_SERVICE_MASK;
    bt_ui_store_device(p_device_rec);
    bcmapp_dm_sec_add_device(p_device_rec);


}

/*******************************************************************************
**
** Function         bcmapp_ag_set_device_default_headset
**
** Description      Sets the device as default headset
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_set_device_default_headset (tBTUI_REM_DEVICE * p_device_rec )
{

    int i;
    BCM_FUN_STAR("bcmapp_ag_set_device_default_headset", 0,0,0);
    /* if any other device was set as
    default change that.*/
    for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
    {
        bt_ui_device_db.device[i].is_default = FALSE;

    }

    /* phone can try a audio connection to default
    audio device during a incoming call if no audio
    connections are active at that time */
    p_device_rec->is_default = TRUE;

    bt_ui_store_device(p_device_rec);



}

/*******************************************************************************
**
** Function         bcmapp_ag_connect_device
**
** Description      Makes an HSP/HFP connection to the specified device
**
**
** Returns          void
*******************************************************************************/
BOOLEAN bcmapp_ag_connect_device(tBTUI_REM_DEVICE * p_device_rec, BOOLEAN is_fm)
{
    UINT16 handle;
#ifdef BTUI_AG_ID_2
    BCM_FUN_STAR("bcmapp_ag_connect_device %d %d", bcm_ag_cb.app[BTUI_AG_ID_1].is_open, bcm_ag_cb.app[BTUI_AG_ID_2].is_open,0);
#else
    BCM_FUN_STAR("bcmapp_ag_connect_device %d", bcm_ag_cb.app[BTUI_AG_ID_1].is_open, 0,0);
#endif
    if (bcm_ag_cb.app[BTUI_AG_ID_1].is_open == FALSE)
    {
        handle = bcm_ag_cb.app[BTUI_AG_ID_1].handle;
        p_device_rec->ag_config.ag_id = BTUI_AG_ID_1;
    }
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
    else if (bcm_ag_cb.app[BTUI_AG_ID_2].is_open == FALSE)
    {
        handle = bcm_ag_cb.app[BTUI_AG_ID_2].handle;
        p_device_rec->ag_config.ag_id = BTUI_AG_ID_2;
    }
#endif    
    else
    {
        /* both handles are busy, cannot make a new connection now */
        return FALSE;
    }
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
    bcm_ag_cb.play_fm = is_fm;
#endif
    BTA_AgOpen(handle, p_device_rec->bd_addr, bt_ui_cfg.ag_security, bt_ui_cfg.supported_services & (BTA_HSP_SERVICE_MASK | BTA_HFP_SERVICE_MASK));

    return TRUE;
}

/*******************************************************************************
**
** Function         bcmapp_ag_find_active_vr
**
** Description      Searches for the handle with Voice Recognition active.
**                  BTUI_AG_NUM_APP is returned if none active..
**                  Note: Assumes only one active VR at a time.
**
** Returns          void
*******************************************************************************/
static UINT8 bcmapp_ag_find_active_vr(void)
{
    UINT8 xx;

    /* Find an active VR if any */
    for (xx = 0; xx < BTUI_AG_NUM_APP; xx++)
    {
        /* Should only be one active SCO */
        if (bcm_ag_cb.app[xx].bvra_active)
        {
            break;
        }
    }

    return xx;
}

/*******************************************************************************
**
** Function         bcmapp_ag_disable_vr
**
** Description      Disables the voice recognition if enabled by sending an
**                  unsolicited BVRA:0 to the peer HS.
**                  BTUI_AG_NUM_APP is used to turn off any active VR peer.
**                  Note: Assumes only one active VR at a time.
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_disable_vr(UINT8 app_id)
{
    tBTA_AG_RES_DATA  data;
    BOOLEAN           is_active = FALSE;

    if (app_id < BTUI_AG_NUM_APP && bcm_ag_cb.app[app_id].bvra_active)
    {
        is_active = TRUE;   /* instance is specified and active */
    }
    /* See if disabling unspecified peer */
    else if (app_id == BTUI_AG_NUM_APP)
    {
        if ((app_id = bcmapp_ag_find_active_vr()) < BTUI_AG_NUM_APP)
        {
            is_active = TRUE;   /* instance is specified and active */
        }
    }

    if (is_active)
    {
        BCM_MSG_MED("bcmapp_ag_disable_vr: Sending BVRA:0 for handle %d",
                          bcm_ag_cb.app[app_id].handle,0,0);
        bcm_ag_cb.app[app_id].bvra_active = FALSE;
        data.state = FALSE;
        BTA_AgResult(bcm_ag_cb.app[app_id].handle, BTA_AG_BVRA_RES, &data);
    }
}

#if BTUI_AG_DEBUG == TRUE
static char *bcmapp_ag_evt_str(tBTA_AG_EVT event)
{
    switch (event)
    {
    case BTA_AG_ENABLE_EVT:
        return "Enable Evt";
    case BTA_AG_REGISTER_EVT:
        return "Register Evt";
    case BTA_AG_OPEN_EVT:
        return "Open Evt";
    case BTA_AG_CLOSE_EVT:
        return "Close Evt";
    case BTA_AG_CONN_EVT:
        return "Connect Evt";
    case BTA_AG_AUDIO_OPEN_EVT:
        return "Audio Opened";
    case BTA_AG_AUDIO_CLOSE_EVT:
        return "Audio Closed";
    default:
        return "Other";
    }
}
#endif

#if 0
/*under this line is bcmui_ag_insight.c*/
/*******************************************************************************
**
** Function         bcmapp_ag_get_call_stats
**
** Description      Returns the number of active and held calls
**
**
**
** Returns          void.
*******************************************************************************/
static void bcmapp_ag_get_call_stats(UINT8 *num_active, UINT8 *num_held)
{
    int i;
    *num_active = *num_held = 0;
    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use)
        {
            if(BTUI_CALL_STATE_ACTIVE == bcm_ag_call_table[i].call_state)
                (*num_active)++;
            else if(BTUI_CALL_STATE_ACTIVE_HELD == bcm_ag_call_table[i].call_state)
                (*num_held)++;
        }
    }
    BCM_FUN_STAR("bcmapp_ag_get_call_stats %d %d %d", *num_held,*num_held,i);
}

/*******************************************************************************
**
** Function         bcmapp_ag_release_call_by_index
**
** Description      Releases the call specified by the index and adjusts the
**                  indexes of other calls.
**
**
**
** Returns          void.
*******************************************************************************/
static void bcmapp_ag_release_call_by_index(UINT8 index)
{
    int i;
    tBTUI_CALL_DATA    *p_bct;
    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        p_bct = &bcm_ag_call_table[i];
        if(p_bct->is_in_use)
        {
            if(p_bct->call_index == index)
            {
                BT_UI_APPL_TRACE_DEBUG2("Original Call %d in state %d Dropped", p_bct->call_index, p_bct->call_state);
                p_bct->is_in_use = FALSE;
            }
            else if(p_bct->call_index > index)
            {
                BT_UI_APPL_TRACE_DEBUG2("Adjusting call index %d to %d", p_bct->call_index, p_bct->call_index-1);
                p_bct->call_index--;
            }
        }
    }
    BCM_FUN_STAR("bcmapp_ag_release_call_by_index %d %d", i,index,0);
}

/*******************************************************************************
**
** Function         bcmapp_ag_release_calls_in_state
**
** Description      Releases all the calls in the state passed
**
**
**
** Returns          void.
*******************************************************************************/
static void bcmapp_ag_release_calls_in_state(UINT8 call_state)
{
    int i;
    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use && (bcm_ag_call_table[i].call_state == call_state))
        {
            bcmapp_ag_release_call_by_index(bcm_ag_call_table[i].call_index);
        }
    }
    BCM_FUN_STAR("bcmapp_ag_release_calls_in_state %d", i,0,0);
}

/*******************************************************************************
**
** Function         bcmapp_ag_move_call_state
**
** Description      Moves all the calls in 'from' state to 'to' state
**
**
**
** Returns          void.
*******************************************************************************/
static void bcmapp_ag_move_call_state(UINT8 from, UINT8 to)
{
    int i;
    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use && (bcm_ag_call_table[i].call_state == from))
        {
            //BT_UI_APPL_TRACE_DEBUG3("Call %d moved to %d from %d", bcm_ag_call_table[i].call_index, to, from);
            bcm_ag_call_table[i].call_state = to;
        }
    }
    BCM_FUN_STAR("bcmapp_ag_move_call_state %d", i,0,0);
}
/*******************************************************************************
**
** Function         bcmapp_ag_get_call_table_entry
**
** Description      returns an unused entry in call table
**                  returns NULL if all 3 entries are used
**
**
** Returns          Pointer to tBTUI_CALL_DATA or NULL.
*******************************************************************************/
static tBTUI_CALL_DATA* bcmapp_ag_get_call_table_entry(void)
{
    int i;
    tBTUI_CALL_DATA* retVal = NULL;

    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(!bcm_ag_call_table[i].is_in_use)
        {
            retVal = &bcm_ag_call_table[i];
        }
    }
    BCM_FUN_STAR("bcmapp_ag_move_call_state %x", retVal,0,0);
    return retVal;
}

/*******************************************************************************
**
** Function         bcmapp_ag_get_call_data_by_index
**
** Description      returns pointer to the call data specified by the index
**
**
** Returns          Pointer to tBTUI_CALL_DATA or NULL.
*******************************************************************************/
static tBTUI_CALL_DATA* bcmapp_ag_get_call_data_by_index(UINT8 index)
{
    int i;
    tBTUI_CALL_DATA* retVal = NULL;

    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use && (bcm_ag_call_table[i].call_index == index))
        {
            retVal = &bcm_ag_call_table[i];
        }
    }
    BCM_FUN_STAR("bcmapp_ag_get_call_data_by_index %x", retVal,0,0);
    return retVal;
}

/*******************************************************************************
**
** Function         bcmapp_ag_get_call_data_in_call_setup
**
** Description      returns pointer to the call data of the call in call setup
**
**
** Returns          Pointer to tBTUI_CALL_DATA or NULL.
*******************************************************************************/
static tBTUI_CALL_DATA* bcmapp_ag_get_call_data_in_call_setup(void)
{
    int i;
    tBTUI_CALL_DATA* retVal = NULL;

    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use &&
           ((BTUI_CALL_STATE_INCOMING == bcm_ag_call_table[i].call_state)
             || (BTUI_CALL_STATE_OUTGOING == bcm_ag_call_table[i].call_state)))
        {
            retVal = &bcm_ag_call_table[i];
        }
    }
    BCM_FUN_STAR("bcmapp_ag_get_call_data_in_call_setup %x", retVal,0,0);
    return retVal;
}

/*******************************************************************************
**
** Function         bcmapp_ag_is_in_call_setup
**
** Description      returns TRUE if we are in call setup(incoming/outgoing)
**                  for any of the calls
**
** Returns          TRUE if any call is in callsetup, FALSE otherwise
*******************************************************************************/
static BOOLEAN bcmapp_ag_is_in_call_setup(void)
{
    int i;

    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        if(bcm_ag_call_table[i].is_in_use &&
           ((BTUI_CALL_STATE_INCOMING == bcm_ag_call_table[i].call_state)
             || (BTUI_CALL_STATE_OUTGOING == bcm_ag_call_table[i].call_state)))
        {
            return(TRUE);
        }
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bcmapp_ag_reset_call_table
**
** Description      resets all call table entries as not in use
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_reset_call_table(void)
{
    int i;
    BCM_FUN_STAR("bcmapp_ag_reset_call_table ", 0,0,0);
    for(i=0; i < BTUI_MAX_CALLS; i++)
    {
        bcm_ag_call_table[i].is_in_use = FALSE;
        bcm_ag_call_table[i].call_index = 0;
        bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_NONE;
        bcm_ag_call_table[i].dial_num[0] = '\0';
        bcm_ag_call_table[i].call_id = 0 ;
    }
}

/*******************************************************************************
**
** Function         bcmapp_ag_calc_held_ind
**
** Description      Looks at the active and held calls and determines the held
**                  indicator value
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_calc_held_ind(void)
{
    UINT8           num_active, num_held;
    tBTUI_CALL_DATA* call_data = NULL;
    tBTUI_AG_CB      *p_cb = &bcm_ag_cb;
    BCM_FUN_STAR("bcmapp_ag_calc_held_ind ", 0,0,0);
    bcmapp_ag_get_call_stats(&num_active, &num_held);
    call_data = bcmapp_ag_get_call_data_in_call_setup();
    if(call_data)
    {
        if(BTUI_CALL_STATE_OUTGOING == call_data->call_state)
            num_active++;
    }

    if(num_active && num_held)
        p_cb->ind[BTUI_AG_IND_HELD] = '1';
    else if(num_held)
        p_cb->ind[BTUI_AG_IND_HELD] = '2';
    else
        p_cb->ind[BTUI_AG_IND_HELD] = '0';

}
#endif
/*******************************************************************************
**
** Function         bcmapp_ag_send_ok
**
** Description      Format and send OK AT cmd
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_send_ok(UINT16 handle)
{
    tBTA_AG_RES_DATA  data;
    BCM_FUN_STAR("bcmapp_ag_send_ok %d", handle,0,0);
    data.ok_flag = BTA_AG_OK_DONE;
    data.str[0] = 0;
    BTA_AgResult(handle, BTA_AG_UNAT_RES, &data);
}

/*******************************************************************************
**
** Function         bcmapp_ag_send_error
**
** Description      Format and send ERROR AT cmd
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_send_error(UINT16 handle, UINT16 errCode)
{
    tBTA_AG_RES_DATA  data;
    BCM_FUN_STAR("bcmapp_ag_send_error %d %d", handle,errCode,0);
    data.ok_flag = BTA_AG_OK_ERROR;
    data.errcode = errCode;
    BTA_AgResult(handle, BTA_AG_UNAT_RES, &data);
}
#if 0
/*******************************************************************************
**
** Function         bcmapp_ag_get_clcc_status
**
** Description      Translate call state to CLCC state
**
** Returns          CLCC state
*******************************************************************************/
static UINT8 bcmapp_ag_get_clcc_status(UINT8 call_state, UINT8 num_active)
{
    switch(call_state)
    {
    case BTUI_CALL_STATE_INCOMING:
        if(num_active)
            return BTUI_AG_CLCC_STATE_WAITING;
        else
            return BTUI_AG_CLCC_STATE_INCOMING;
        break;
    case BTUI_CALL_STATE_OUTGOING:
        return BTUI_AG_CLCC_STATE_DIALING;
        break;
    case BTUI_CALL_STATE_ACTIVE:
        return BTUI_AG_CLCC_STATE_ACTIVE;
        break;
    case BTUI_CALL_STATE_ACTIVE_HELD:
        return BTUI_AG_CLCC_STATE_HELD;
        break;
    default:
        BT_UI_APPL_TRACE_ERROR1("Unknown Call State %d", call_state);
        return BTUI_AG_CLCC_STATE_ACTIVE;
        break;
    }
}

/*******************************************************************************
**
** Function         bcmapp_ag_get_clcc_mparty
**
** Description      Return Multi-party status
**
** Returns          Multi-party status
*******************************************************************************/
static UINT8 bcmapp_ag_get_clcc_mparty(UINT8 call_state, UINT8 num_active)
{
    if((num_active > 1) && (BTUI_CALL_STATE_ACTIVE == call_state))
        return BTUI_AG_CLCC_MPARTY_TRUE;
    else
        return BTUI_AG_CLCC_MPARTY_FALSE;
}
#endif
#ifndef AEE_SIMULATOR
byte bcmapp_ag_get_vol(byte input_vol)
{
    switch(input_vol)
    {
        case OEMSOUND_1ST_VOL:
            return 3;

        case OEMSOUND_2ND_VOL:
            return 6;

        case OEMSOUND_3RD_VOL:
            return 9;

        case OEMSOUND_4TH_VOL:
            return 11;

        case OEMSOUND_5TH_VOL:
            return 14;

        default:
            return 11; 
    }
}
#endif
/*******************************************************************************
**
** Function         bcmapp_ag_platform_init
**
** Description      Platform specific initialization for audio
**
**
** Returns          void
*******************************************************************************/
void bcmapp_ag_platform_init(void)
{
    /* initialize volume settings */
    byte vol = 0;
    byte vol_ag = 0;
    BCM_FUN_STAR("bcmapp_ag_platform_init", 0,0,0);
#ifndef AEE_SIMULATOR
    OEM_GetConfig(CFGI_EAR_VOL, (void*)&vol, sizeof(vol));
    vol_ag = bcmapp_ag_get_vol(vol);
#else
    vol_ag = 7;
#endif
    bcm_ag_cb.app[BTUI_AG_ID_1].mic_vol = vol_ag;
    bcm_ag_cb.app[BTUI_AG_ID_1].spk_vol = vol_ag;
    bcm_ag_cb.app[BTUI_AG_ID_1].is_open = FALSE;
    
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
    bcm_ag_cb.app[BTUI_AG_ID_2].mic_vol = vol_ag;
    bcm_ag_cb.app[BTUI_AG_ID_2].spk_vol = vol_ag;
    bcm_ag_cb.app[BTUI_AG_ID_2].is_open = FALSE;
#endif
    /* Set response & hold to inactive state */
    bcm_ag_cb.btrh_state = BTA_AG_BTRH_NO_RESP;
    bcm_ag_cb.btrh_no_sco = FALSE;

    /* initialize indicators */
    strcpy(bcm_ag_cb.ind, bcm_ag_ind_default);

    /* initialize inband ring */
    bcm_ag_cb.inband_ring = FALSE;

    bcm_ag_cb.call_state = BTUI_AG_CALL_NONE;
    //bcmapp_ag_reset_call_table();
}


/*******************************************************************************
**
** Function         bcmapp_ag_check_dial_str
**
** Description      Place holder function for real applications to
**                  to check the dial strings and outgoing call restrictions.
**
**
** Returns          TRUE if call is allowed, FALSE otherwise.
*******************************************************************************/
//static BOOLEAN bcmapp_ag_check_dial_str(char *str)
//{
//    /* Check the dial digits here */
//    return TRUE;
//}

/*******************************************************************************
**
** Function         bcmapp_ag_platform_ag_event
**
** Description      platform specific processing of ag events
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_platform_ag_event(tBTA_AG_EVT event, tBTA_AG *p_data)
{
    tBTA_AG_RES_DATA  data;
    UINT16            handle = p_data->hdr.handle;
    UINT8             app_id = p_data->hdr.app_id;
    tBTUI_AG_CB      *p_cb = &bcm_ag_cb;

    BCM_FUN_STAR("handle:%d app_id:%d num %d bcmapp_ag_platform_ag_event", handle, app_id,p_data->val.num);

    switch (event)
    {
#if 0
    case BTA_AG_CLOSE_EVT:

        /* if no connections are open, reset the call state. scripts may leave the call state
           in some state, which may affect scripts which are run later */
        if(!((p_cb->app[BTUI_AG_ID_1].is_open)|| (p_cb->app[BTUI_AG_ID_2].is_open)))
        {
            p_cb->call_state = BTUI_AG_CALL_NONE;
            p_cb->call_handle = BTA_AG_HANDLE_NONE;
        }
        break;
#endif
    case BTA_AG_SPK_EVT:
        p_cb->app[app_id].spk_vol = (UINT8) p_data->val.num;
        break;

    case BTA_AG_MIC_EVT:
        p_cb->app[app_id].mic_vol = (UINT8) p_data->val.num;
        break;

    case BTA_AG_AT_CKPD_EVT:/*bluetooth hsp headset press key event,  vaild data is tBTA_AG_VAL num*/
    {
#if 0
        /* If there is a call in call setup,
         * make it active, if its an incoming call
         * end it, if its an outgoing call */
        call_data = bcmapp_ag_get_call_data_in_call_setup();
        //BT_UI_APPL_TRACE_DEBUG2("BTA_AG_AT_CKPD_EVT %d %d",call_data,p_data->val.num);
        if(call_data)
        {
            if(BTUI_CALL_STATE_INCOMING == call_data->call_state)/*is incoming ,answer it, need send event to cm*/
            {
                call_data->call_state = BTUI_CALL_STATE_ACTIVE;
                BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE", call_data->call_index);
            }
            else/*if is outgoing,please end the call,need send event to cm*/
            {
                bcmapp_ag_release_call_by_index(call_data->call_index);
            }
        }
        else/*no active call or have a connect ,need make a outgoing call*/
        {
            //BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Answer");

            /* If there is an active call end it */
            for(i=0; i < BTUI_MAX_CALLS; i++)
            {
                if(bcm_ag_call_table[i].is_in_use &&
                   (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE))/*if is connect,please end the call,need send event to cm*/
                {
                    bcmapp_ag_release_call_by_index(bcm_ag_call_table[i].call_index);
                    break;
                }
            }
            /* If there is no active call, we have to initiate an outgoing call */
            if(BTUI_MAX_CALLS == i)/*IDLE*/
            {
                call_data = bcmapp_ag_get_call_table_entry();
                if(call_data)
                {
                    bcmapp_ag_get_call_stats(&num_active, &num_held);
                    call_data->is_in_use = TRUE;
                    call_data->call_index = num_active + num_held + 1;
                    call_data->call_state = BTUI_CALL_STATE_OUTGOING;
                    call_data->call_dir = BTUI_CALL_DIR_MO;
                    strcpy(call_data->dial_num, call_data->dial_num_to_use);
                    BT_UI_APPL_TRACE_DEBUG1("Call %d (CKPD) moved to OUTGOING", call_data->call_index);
                }
            }
        }
        p_cb->call_handle = handle;
        bcmapp_ag_handle_call_event(handle, BTUI_AG_HS_CKPD_EVT, app_id);
#else
        /*1. incoming->answer*/
        /*2. connect ->end cal */
        /*3. idle->originate call*/
        /*4. outgoing->not process*/
        tBTUI_BTA_MSG    *p_msg;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_AG_AT_CKPD_EVT;
            strcpy(p_data->val.str,p_msg->ag_val.ag_val.str);
            bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
        }
#endif
        break;
    }
    case BTA_AG_AT_CIND_EVT:
        strcpy(data.str, p_cb->ind);/*send current state to headset*/
        BTA_AgResult(handle, BTA_AG_CIND_RES, &data);
        break;

    case BTA_AG_AT_A_EVT:/*headset answer a call, vaild data is tBTA_AG_VAL*/
    {
        tBTUI_BTA_MSG    *p_msg;
        p_cb->call_handle = handle;
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
        p_cb->sco_owner = 1;
#endif
#if 0
        /* Find the entry of the call in call setup and make it active */
        call_data = bcmapp_ag_get_call_data_in_call_setup();
        if(call_data)
        {
            call_data->call_state = BTUI_CALL_STATE_ACTIVE;
            BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE", call_data->call_index);
        }
        else
            BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Answer");
        bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_ANS_EVT, app_id);
#else
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_AG_AT_A_EVT;
            bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
        }
#endif
        break;
    }

    case BTA_AG_AT_D_EVT:/*headset org a call , vaild data is tBTA_AG_VAL str*/
    {
#if 0
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
        if (p_cb->play_fm)
        {
            /* re configure FM Audio */
            bcmapp_fm_route_audio(BTA_FM_SCO_OFF);
            bt_ui_fm_cb.fm_pending = TRUE;
            bt_ui_fm_cb.sco_pending = TRUE;
        }
#endif
        if(bcmapp_ag_check_dial_str(p_data->val.str))
        {
            /* Make sure there is no call in call setup
             * Get an unused entry from call table
             * Find the number of active calls and held calls
             * set the index */

            call_data = bcmapp_ag_get_call_table_entry();
            if(!call_data || bcmapp_ag_is_in_call_setup())
            {
                bcmapp_ag_send_error(handle, BTA_AG_ERR_PHONE_FAILURE);
            }
            else
            {
                /* Send OK now*/
                bcmapp_ag_send_ok(handle);

                /* If any active calls, move them to held */
                bcmapp_ag_move_call_state(BTUI_CALL_STATE_ACTIVE, BTUI_CALL_STATE_ACTIVE_HELD);

                bcmapp_ag_get_call_stats(&num_active, &num_held);
                call_data->is_in_use = TRUE;
                call_data->call_index = num_active + num_held + 1;
                call_data->call_state = BTUI_CALL_STATE_OUTGOING;
                call_data->call_dir = BTUI_CALL_DIR_MO;
                if('>' == p_data->val.str[0])
                {
                    strcpy(call_data->dial_num, call_data->dial_num_to_use);
                    BT_UI_APPL_TRACE_DEBUG1("Call %d (memdial) moved to OUTGOING", call_data->call_index);
                }
                else
                {
                    strcpy(call_data->dial_num, p_data->val.str);
                    BT_UI_APPL_TRACE_DEBUG1("Call %d (dial) moved to OUTGOING", call_data->call_index);
                }

                p_cb->call_handle = handle;
                bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_DIAL_EVT, app_id);
            }
        }
        else
        {
            bcmapp_ag_send_error(handle, BTA_AG_ERR_PHONE_FAILURE);
        }
#else
        tBTUI_BTA_MSG    *p_msg;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_AG_AT_D_EVT;
            strcpy(p_msg->ag_val.ag_val.str,p_data->val.str);
            bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
        }
#endif
        break;
    }
    case BTA_AG_AT_CHLD_EVT:/*An integer from 0 to 4,tBTA_AG_VAL num*/
#if 1
    {
        tBTUI_BTA_MSG    *p_msg;
        p_cb->call_handle = handle;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_AG_AT_CHLD_EVT;
            strcpy(p_msg->ag_val.ag_val.str,p_data->val.str);
            p_msg->ag_val.ag_val.idx = p_data->val.idx;
            bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
        }
    }
#else
        p_cb->call_handle = handle;
        BCM_MSG_MED("BTA_AG_AT_CHLD_EVT %d %d",p_data->val.str[0],p_data->val.idx,0);
        /* Send OK or ERROR */
        if(((p_data->val.str[0] >= '0') && (p_data->val.str[0] <= '4'))
            && (!((!(bt_ui_cfg.ag_features & BTA_AG_FEAT_ECC)) && (p_data->val.idx != 0))))
        {
            /* Validate the index */
            if((('1' == p_data->val.str[0]) || ('2' == p_data->val.str[0])) && p_data->val.idx)
            {
                if(!bcmapp_ag_get_call_data_by_index(p_data->val.idx))
                {
                    BT_UI_APPL_TRACE_DEBUG1("Invalid index %d", p_data->val.idx);
                    bcmapp_ag_send_error(handle, BTA_AG_ERR_INVALID_INDEX);
                    break;
                }

            }
            bcmapp_ag_send_ok(handle);
        }
        else
        {
            bcmapp_ag_send_error(handle, BTA_AG_ERR_PHONE_FAILURE);
            break;
        }

        /* chld=0 releases all held calls or sets busy for a waiting call. */
        if (p_data->val.str[0] == '0')
        {
             /* if incoming call is waiting treat as call cancel, otherwise ignore */
            if (p_cb->ind[BTUI_AG_IND_SETUP] == '1')
            {
                /* Find the entry of the call in call setup and release it */
                call_data = bcmapp_ag_get_call_data_in_call_setup();
                if(call_data)
                {
                    bcmapp_ag_release_call_by_index(call_data->call_index);
                }
                else
                    BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Reject");
                bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_CANCEL_EVT, app_id);
            }
            else
            {
                /* release all held calls */
                bcmapp_ag_release_calls_in_state(BTUI_CALL_STATE_ACTIVE_HELD);

                /* If there are only held calls and no active calls, drop the call and SCO */
                if (p_cb->ind[BTUI_AG_IND_HELD] == '2')
                {
                    bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_HANGUP_EVT, app_id);
                }
                p_cb->ind[BTUI_AG_IND_HELD] = '0';
            }
        }
        /* chld=1 releases all active calls (if any exist) and accepts the other (held or waiting) call */
        else if (p_data->val.str[0] == '1')
        {
            /* If an index is specified release that call, otherwise release all active calls */
            if (p_data->val.idx)
            {
                call_data = bcmapp_ag_get_call_data_by_index(p_data->val.idx);
                if(call_data)
                {
                    bcmapp_ag_release_call_by_index(call_data->call_index);
                }
                else
                    BT_UI_APPL_TRACE_DEBUG1("No call with specified index %d to release", p_data->val.idx);
            }
            else
                bcmapp_ag_release_calls_in_state(BTUI_CALL_STATE_ACTIVE);

            /* if incoming call is waiting answer the call, else treat as hangup */
            if (p_cb->ind[BTUI_AG_IND_SETUP] == '1')
            {
                /* Find the entry of the call in call setup and make it active */
                call_data = bcmapp_ag_get_call_data_in_call_setup();
                if(call_data)
                {
                    call_data->call_state = BTUI_CALL_STATE_ACTIVE;
                    BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE", call_data->call_index);
                }
                bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_ANS_EVT, app_id);
            }
            /* If there are any held call, accept the held call and drop the current call. */
            else
            {
                if (p_cb->ind[BTUI_AG_IND_HELD] == '0')
                {
                    /* There is no held call simply hangup the current call. */
                    bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_HANGUP_EVT, app_id);
                }
                /* Look for the first held call and make it active */
                else
                {
                    for(i=0; i < BTUI_MAX_CALLS; i++)
                    {
                        if(bcm_ag_call_table[i].is_in_use && (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE_HELD))
                        {
                            BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE from HELD", bcm_ag_call_table[i].call_index);
                            bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_ACTIVE;
                            break;
                        }
                    }
                }
            }

            p_cb->ind[BTUI_AG_IND_HELD] = '0';
        }
        /* chld=2 places all active calls (if any exist) on hold and accepts the other (held or waiting) call */
        else if (p_data->val.str[0] == '2')
        {
            /* If an index is specified enter into private consultation mode */
            if (p_data->val.idx)
            {
                for(i=0; i < BTUI_MAX_CALLS; i++)
                {
                    if(bcm_ag_call_table[i].is_in_use &&
                       (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE) &&
                       (bcm_ag_call_table[i].call_index != p_data->val.idx))
                    {
                        BT_UI_APPL_TRACE_DEBUG1("Call %d moved to HELD from ACTIVE", bcm_ag_call_table[i].call_index);
                        bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_ACTIVE_HELD;
                    }
                }
            }
            else
            {
                /* if incoming call is waiting answer the call */
                if (p_cb->ind[BTUI_AG_IND_SETUP] == '1')
                {
                    call_data = bcmapp_ag_get_call_data_in_call_setup();
                    if(call_data)
                    {
                        no_held_call_index = call_data->call_index;
                        call_data->call_state = BTUI_CALL_STATE_ACTIVE;
                        BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE", call_data->call_index);

                        bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_ANS_EVT, app_id);
                    }
                    else
                        BT_UI_APPL_TRACE_DEBUG0("Failed to find call in callsetup");
                }
                /* find the first call in held state and make it active
                 * move all the other active calls to held */
                else
                {
                    for(i=0; i < BTUI_MAX_CALLS; i++)
                    {
                        if(bcm_ag_call_table[i].is_in_use &&
                           (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE_HELD))
                        {
                            BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE from HELD", bcm_ag_call_table[i].call_index);
                            bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_ACTIVE;
                            no_held_call_index = bcm_ag_call_table[i].call_index;
                            break;
                        }
                    }
                }

                /* Now put all ACTIVE calls other than the one with no_held_call_index to HELD */
                for(i=0; i < BTUI_MAX_CALLS; i++)
                {
                    if(bcm_ag_call_table[i].is_in_use &&
                       (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE) &&
                       (bcm_ag_call_table[i].call_index != no_held_call_index))
                    {
                        BT_UI_APPL_TRACE_DEBUG1("Call %d moved to HELD from ACTIVE", bcm_ag_call_table[i].call_index);
                        bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_ACTIVE_HELD;
                    }
                }
            }
#if 0
            /* if incoming call is waiting answer the call */
            if (p_cb->ind[BTUI_AG_IND_SETUP] == '1')
            {
                bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_ANS_EVT, app_id);
                p_cb->ind[BTUI_AG_IND_HELD] = '1';
            }
            /* If there is no held call, put the active call on hold */
            else if(p_cb->ind[BTUI_AG_IND_HELD] = '0')
            {
                p_cb->ind[BTUI_AG_IND_HELD] = '2';
            }
            /* If there is only held call, make that call active*/
            else if (p_cb->ind[BTUI_AG_IND_HELD] = '2')
            {
                p_cb->ind[BTUI_AG_IND_HELD] = '0';
            }
            /* If there is an active call and a held call, toggle them*/
            else if (p_cb->ind[BTUI_AG_IND_HELD] = '1')
            {
                p_cb->ind[BTUI_AG_IND_HELD] = '1';
            }
#endif
        }
        /* chld=3 adds a held call to the conversation; we don't keep track of held calls, so do nothing */
        else if (p_data->val.str[0] == '3')
        {
            for(i=0; i < BTUI_MAX_CALLS; i++)
            {
                if(bcm_ag_call_table[i].is_in_use &&
                   (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE_HELD))
                {
                    BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE from HELD", bcm_ag_call_table[i].call_index);
                    bcm_ag_call_table[i].call_state = BTUI_CALL_STATE_ACTIVE;
                }
            }
            p_cb->ind[BTUI_AG_IND_HELD] = '0';
        }
        /* chld=4 connects the two calls and disconnects the subscriber from both calls; treat as hangup */
        else if (p_data->val.str[0] == '4')
        {
            for(i=0; i < BTUI_MAX_CALLS; i++)
            {
                bcm_ag_call_table[i].is_in_use = FALSE;
            }
            p_cb->ind[BTUI_AG_IND_HELD] = '0';
            bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_HANGUP_EVT, app_id);
        }

        bcmapp_ag_calc_held_ind();

        data.ind.id = BTA_AG_IND_CALLHELD;
        data.ind.value = p_cb->ind[BTUI_AG_IND_HELD] - '0';
        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);
#endif
        break;

    case BTA_AG_AT_CHUP_EVT:/*tBTA_AG_VAL*/
    {
        /* If there is a call in callsetup, end it */
#if 0
        call_data = bcmapp_ag_get_call_data_in_call_setup();
        if(call_data)
        {
            bcmapp_ag_release_call_by_index(call_data->call_index);
        }
        else /* End all active calls */
        {
            bcmapp_ag_release_calls_in_state(BTUI_CALL_STATE_ACTIVE);
        }

        p_cb->call_handle = handle;
        bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_HANGUP_EVT, app_id);
#else
        tBTUI_BTA_MSG    *p_msg;
        if ((p_msg = bt_ui_get_msg()) != NULL)
        {
            p_msg->hdr.event = BTUI_MMI_AG_AT_CHUP_EVT;
            p_cb->call_handle = handle;
            bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
        }
#endif
        break;
    }

    case BTA_AG_AT_VTS_EVT:
        /* dial dtmf digit */
        break;

    case BTA_AG_AT_BINP_EVT:
        data.ok_flag = BTA_AG_OK_DONE;
        strcpy(data.str, "\"8584538400\"");/*my number?*/
        BTA_AgResult(handle, BTA_AG_BINP_RES, &data);
        break;

    case BTA_AG_AT_BLDN_EVT:/*tBTA_AG_VAL*/
        if(bt_ui_cfg.ag_bldn_enable)
        {
            /* Make sure there is no call in call setup
             * Get an unused entry from call table
             * Find the number of active calls and held calls
             * set the index */
#if 0
            call_data = bcmapp_ag_get_call_table_entry();
            if(!call_data && !bcmapp_ag_is_in_call_setup())	// if(!call_data || bcmapp_ag_is_in_call_setup())   //tonny markup
            {
                bcmapp_ag_send_error(handle, BTA_AG_ERR_PHONE_FAILURE);
                break;
            }
            else
            {
                /* Send OK now*/
                bcmapp_ag_send_ok(handle);

                /* If any active calls, move them to held */
                bcmapp_ag_move_call_state(BTUI_CALL_STATE_ACTIVE, BTUI_CALL_STATE_ACTIVE_HELD);

                bcmapp_ag_get_call_stats(&num_active, &num_held);
                call_data->is_in_use = TRUE;
                call_data->call_index = num_active + num_held + 1;
                call_data->call_state = BTUI_CALL_STATE_OUTGOING;
                call_data->call_dir = BTUI_CALL_DIR_MO;
                strcpy(call_data->dial_num, call_data->dial_num_to_use);
                //BT_UI_APPL_TRACE_DEBUG1("Call %d (BLDN) moved to OUTGOING", call_data->call_index);

                p_cb->call_handle = handle;
                bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_DIAL_EVT, app_id);
            }
#else
            tBTUI_BTA_MSG    *p_msg;
            if ((p_msg = bt_ui_get_msg()) != NULL)
            {
                p_msg->hdr.event = BTUI_MMI_AG_AT_BLDN_EVT;
                p_cb->call_handle = handle;
                bt_ui_send_msg(BTUI_AG_MSG_TYPE, p_msg);
            }
#endif
        }
        else
        {
            data.ok_flag = BTA_AG_OK_ERROR;
            data.errcode = BTA_AG_ERR_PHONE_FAILURE;
            BTA_AgResult(handle, BTA_AG_UNAT_RES, &data);
        }
        break;

    case BTA_AG_AT_BVRA_EVT:
#if 0
        if (p_data->val.num)
        {
            /* Turn on Voice Recognition */
            if (!p_cb->app[app_id].bvra_active)
            {
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                     /* turn off FM on SCO, turn on voice for outgoing call */
                if (p_cb->play_fm)
                {
                    BT_UI_APPL_TRACE_ERROR0("Voice recognition received: turn off FM over SCO");
                    bcmapp_fm_route_audio(BTA_FM_SCO_OFF);
                    //bt_ui_fm_cb.fm_pending = TRUE;
                    bt_ui_fm_cb.sco_pending = TRUE;
                }
#endif

                if (p_cb->call_state != BTUI_AG_CALL_ACTIVE)
                {
                    /* Ok to enable Voice Recognition */
                    p_cb->app[app_id].bvra_active = TRUE;
                    p_cb->call_handle = handle;
                    BTA_AgAudioOpen(handle);
                }
                else /* If a call is in progress; issue a disable */
                {
                    bcmapp_ag_disable_vr(app_id);
                }
            }
        }
        /* Turn off Voice Recognition */
        else if (p_cb->app[app_id].bvra_active)
        {

            p_cb->app[app_id].bvra_active = FALSE;
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
            if (!p_cb->play_fm)
            {
                BTA_AgAudioClose(handle);
                p_cb->call_handle = BTA_AG_HANDLE_NONE;
            }
            else if (bt_ui_fm_cb.sco_pending == TRUE)
            {
                bcmapp_fm_route_audio(BTA_FM_SCO_ON);
            }

#endif
        }
#else
        data.state = FALSE;
        BTA_AgResult(bcm_ag_cb.app[app_id].handle, BTA_AG_BVRA_RES, &data);
#endif
        break;

    case BTA_AG_AT_NREC_EVT:
        /* disable noise reduction/echo cancellation */
        break;

    case BTA_AG_AT_CNUM_EVT:
        data.ok_flag = BTA_AG_OK_DONE;

        if (p_cb->call_state != BTUI_AG_CALL_NONE)
        {
            strcpy(data.str, ",\"8587953512\",129,,4");
        }
        else
        {
            data.str[0] = '\0'; /* No active subscriber number */
        }
        BTA_AgResult(handle, BTA_AG_CNUM_RES, &data);
        break;

    case BTA_AG_AT_BTRH_EVT:
#if 0
        /* If Read command send back last set value */
        if (p_data->val.num == BTA_AG_BTRH_READ)
        {
            data.ok_flag = BTA_AG_OK_DONE;
            data.num = (UINT16)p_cb->btrh_state;
            BTA_AgResult(handle, BTA_AG_BTRH_RES, &data);
        }
        else
        {
            /* Process hold/acc/rej request */
            p_cb->btrh_cmd = (UINT8) p_data->val.num;
            p_cb->call_handle = handle;
            bcmapp_ag_handle_call_event(handle, BTUI_AG_HF_ANS_HELD_EVT, app_id);
        }
#endif
        break;

    case BTA_AG_AT_CLCC_EVT:
        data.ok_flag = BTA_AG_OK_CONTINUE;
        /*temp process,need get current call table*/
#if 1
        if (p_cb->call_state != BTUI_AG_CALL_NONE)
        {
            /* If active call, call list should be returned <idx>,<dir>,<status>,<mode>,<mprty>,<number>,<type> */
            if (p_cb->call_state == BTUI_AG_CALL_INC)
            {
                strcpy(data.str, "1,1,4,0,0,\"8587953512\",4");
            }
            else if (p_cb->call_state == BTUI_AG_CALL_OUT)
            {
                strcpy(data.str, "1,0,2,0,0,\"8587953512\",4");
            }
            else if (p_cb->call_state == BTUI_AG_CALL_HELD)
            {
                strcpy(data.str, "1,1,1,0,0,\"8587953512\",4");
            }

            else /* Active */
            {
                strcpy(data.str, "1,1,0,0,0,\"8587953512\",4");
            }
        }
        else
        {
            data.str[0] = '\0'; /* No active subscriber number */
        }
        BTA_AgResult(handle, BTA_AG_CLCC_RES, &data);
#else
        bcmapp_ag_get_call_stats(&num_active, &num_held);
        for(i=0;i<BTUI_MAX_CALLS;i++)
        {
            if(bcm_ag_call_table[i].is_in_use)
            {
                UINT8             clcc_idx, clcc_dir, clcc_status, clcc_mparty;
                clcc_idx = bcm_ag_call_table[i].call_index;
                clcc_dir = bcm_ag_call_table[i].call_dir;
                clcc_status = bcmapp_ag_get_clcc_status(bcm_ag_call_table[i].call_state, num_active);
                clcc_mparty = bcmapp_ag_get_clcc_mparty(bcm_ag_call_table[i].call_state, num_active);

                sprintf(data.str,"%d,%d,%d,0,%d,\"%s\",129", clcc_idx, clcc_dir, clcc_status, clcc_mparty,
                                                            bcm_ag_call_table[i].dial_num);
                BTA_AgResult(handle, BTA_AG_CLCC_RES, &data);
            }
        }
#endif
        /* Send OK now*/
        bcmapp_ag_send_ok(handle);
        break;

    case BTA_AG_AT_COPS_EVT:
        data.ok_flag = BTA_AG_OK_DONE;
        strcpy(data.str, "3,0,\"Sing Tel\"");
        BTA_AgResult(handle, BTA_AG_COPS_RES, &data);
        break;

    case BTA_AG_AT_UNAT_EVT:
        /* unknown AT command; only happens when AG uses BTA_AG_FEAT_UNAT */
        /* do some processing for test purposes only */

        if (strcmp(p_data->val.str, "+CGMI") == 0)
        {
            data.ok_flag = BTA_AG_OK_DONE;
            strcpy(data.str, "Broadcom Corp.");
        }
        else if (strcmp(p_data->val.str, "+CLIR=0") == 0)
        {
            data.ok_flag = BTA_AG_OK_DONE;
            data.str[0] = 0;
        }
        else
        {
            data.ok_flag = BTA_AG_OK_ERROR;
            data.errcode = BTA_AG_ERR_OP_NOT_SUPPORTED;
        }
        BTA_AgResult(handle, BTA_AG_UNAT_RES, &data);
        break;

    default:
        break;
    }
}

#if 0
/*******************************************************************************
**
** Function         bcmapp_ag_handle_call_event
**
** Description      Simple call processing simulation state machine
**
**
** Returns          void
*******************************************************************************/
static void bcmapp_ag_handle_call_event(UINT16 handle, UINT8 event, UINT8 app_id)
{
    tBTA_AG_RES_DATA data;
    UINT8             num_active, num_held, i;
    tBTUI_CALL_DATA* call_data = NULL;
    tBTUI_AG_CB      *p_cb = &bcm_ag_cb;

    BCM_FUN_STAR("bcmapp_ag_handle_call_event:%d state:%d app_id %d", event, p_cb->call_state,app_id);

    switch (p_cb->call_state)
    {
        case BTUI_AG_CALL_NONE:
            switch (event)
            {
                case BTUI_AG_HF_DIAL_EVT:
                case BTUI_AG_OUT_CALL_ORIG_EVT:
                case BTUI_AG_OUT_CALL_ORIG_PH_EVT:
                case BTUI_AG_HS_CKPD_EVT:
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                     /* turn off FM on SCO, turn on voice for outgoing call */
                     if (p_cb->play_fm)
                     {
                         bcmapp_fm_route_audio(BTA_FM_SCO_OFF);
                         bt_ui_fm_cb.fm_pending = TRUE;
                         bt_ui_fm_cb.sco_pending = TRUE;
                     }
#endif

                    p_cb->call_state = BTUI_AG_CALL_OUT;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '3';	// BTA_AG_CALLSETUP_ALERTING,

                    /* If voice recognition is enabled send the unsolicited disable to peer
                       because call is active */
                    bcmapp_ag_disable_vr(BTUI_AG_NUM_APP);

                    /* route audio to active SCO, otherwise default */
                    if (p_cb->call_handle != BTA_AG_HANDLE_NONE)
                        data.audio_handle = p_cb->call_handle;
                    else
                        data.audio_handle = handle;

                    /* send originating and alerting */
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ORIG_RES, &data);
                    break;

                case BTUI_AG_IN_CALL_EVT:

#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                    if (p_cb->play_fm)
                    {
                        p_cb->sco_owner = 0;
                        bcmapp_fm_route_audio(BTA_FM_SCO_OFF);
                        BTA_AgAudioClose(p_cb->cur_handle);
                        bt_ui_fm_cb.fm_pending = TRUE;
                        bt_ui_fm_cb.sco_pending = TRUE;
                    }
#endif
                    /* Set response & hold to inactive state */
                    p_cb->btrh_state = BTA_AG_BTRH_NO_RESP;

                    /* If voice recognition is enabled send the unsolicited disable to peer
                       because call is active */
                    bcmapp_ag_disable_vr(BTUI_AG_NUM_APP);

                    p_cb->call_state = BTUI_AG_CALL_INC;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '1';	// BTA_AG_CALLSETUP_INCOMING

                    /* indicate incoming call */
                    call_data = bcmapp_ag_get_call_data_in_call_setup();
                    if(call_data)
                    {
                        sprintf(data.str, "\"%s\"", call_data->dial_num);
                    }
                    else
                    {
                        BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Answer");
                        strcpy(data.str, "\"8584538400\"");
                    }

                    data.num = 129;
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_RES, &data);
                    break;
            }
            break;/*BTUI_AG_CALL_NONE*/

        case BTUI_AG_CALL_INC:
            switch (event)
            {
                case BTUI_AG_HF_HANGUP_EVT:
                case BTUI_AG_END_CALL_EVT:
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                   p_cb->sco_owner = 0;
                   if (p_cb->play_fm)
                   {
                       p_cb->sco_owner = 2;
                       bt_ui_fm_cb.fm_pending = TRUE;
                       BTA_AgAudioOpen(p_cb->cur_handle);
                   }
#endif

                    p_cb->call_state = BTUI_AG_CALL_NONE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';/*BTA_AG_CALLSETUP_NONE*/

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
                    break;

                case BTUI_AG_HF_ANS_EVT:
                case BTUI_AG_IN_CALL_CONN_EVT:
                case BTUI_AG_IN_CALL_CONN_PH_EVT:
                case BTUI_AG_HS_CKPD_EVT:
                    p_cb->call_state = BTUI_AG_CALL_ACTIVE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';	// BTA_AG_CALLSETUP_NONE
                    p_cb->ind[BTUI_AG_IND_CALL] = '1';	// BTA_AG_CALL_ACTIVE
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                    p_cb->sco_owner = 1;
#endif
                    /* route audio */
                    data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);

                    break;


                case BTUI_AG_HF_ANS_HELD_EVT:
                    if (p_cb->btrh_cmd == BTA_AG_BTRH_SET_HOLD)
                    {
                        p_cb->call_state = BTUI_AG_CALL_HELD;
                        p_cb->btrh_state = BTA_AG_BTRH_SET_HOLD;

                        /* do not route audio (call held) */
                        data.audio_handle = BTA_AG_HANDLE_NONE;

                        /* Find the entry of the call in call setup and make it active */
                        call_data = bcmapp_ag_get_call_data_in_call_setup();
                        if(call_data)
                        {
                            call_data->call_state = BTUI_CALL_STATE_ACTIVE;
                            BT_UI_APPL_TRACE_DEBUG1("Call %d moved to ACTIVE", call_data->call_index);
                        }
                        else
                            BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Connect");

                        p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                        p_cb->ind[BTUI_AG_IND_CALL] = '1';

                        /* If voice recognition is enabled send the unsolicited disable to peer
                           because call is active */
                        bcmapp_ag_disable_vr(BTUI_AG_NUM_APP);
                        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_HELD_RES, &data);

                    }
                    break;
            }
            break;

        case BTUI_AG_CALL_OUT:
            switch (event)
            {
                case BTUI_AG_HF_HANGUP_EVT:
                case BTUI_AG_END_CALL_EVT:
                    p_cb->call_state = BTUI_AG_CALL_NONE;
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                    if (p_cb->play_fm)
                    {
                        /* always set fm_pending to be TRUE when call out is ended */
                       p_cb->sco_owner = 2;
                       bt_ui_fm_cb.fm_pending = TRUE;
                       BTA_AgAudioOpen(p_cb->cur_handle);
                    }
                    else
#endif
                        p_cb->call_handle = BTA_AG_HANDLE_NONE;

                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';	// BTA_AG_CALLSETUP_NONE

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, &data);

                    break;
                case BTUI_AG_HS_CKPD_EVT:
                    /*
                     * If SCO connection is open, end the call
                     * If not, setup SCO and transfer audio.
                     */
                    if (p_cb->app[app_id].is_audio_open)
                    {
                        p_cb->call_state = BTUI_AG_CALL_NONE;
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                        if (p_cb->play_fm)
                        {
                            /* always set fm_pending to be TRUE when call out is ended */
                           p_cb->sco_owner = 2;
                           bt_ui_fm_cb.fm_pending = TRUE;
                           BTA_AgAudioOpen(p_cb->cur_handle);
                        }
                        else
#endif
                            p_cb->call_handle = BTA_AG_HANDLE_NONE;
                        p_cb->ind[BTUI_AG_IND_SETUP] = '0';

                        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, &data);
                    }
                    else
                    {
                        BT_UI_APPL_TRACE_DEBUG2("Opening SCO  handle:%d app_id:%d", handle, app_id);
                        /* route audio */
                        data.audio_handle = handle;
                        p_cb->call_handle = handle;
                        BTA_AgAudioOpen(handle);
                    }
                    break;


                case BTUI_AG_OUT_CALL_CONN_EVT:
                    p_cb->call_state = BTUI_AG_CALL_ACTIVE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';	// BTA_AG_CALLSETUP_NONE
                    p_cb->ind[BTUI_AG_IND_CALL] = '1';	// BTA_AG_CALL_ACTIVE

                    /* route audio */
                    if (p_cb->call_handle != BTA_AG_HANDLE_NONE)
                        data.audio_handle = p_cb->call_handle;
                    else
                        data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ALERT_RES, &data);
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);
                    break;

                case BTUI_AG_OUT_CALL_CONN_PH_EVT:

                    p_cb->call_state = BTUI_AG_CALL_ACTIVE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                    p_cb->ind[BTUI_AG_IND_CALL] = '1';

                    /* route audio */
                    data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ALERT_RES, &data);
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);

                    break;
            }
            break;

        case BTUI_AG_CALL_ACTIVE:
            switch (event)
            {
                case BTUI_AG_HF_DIAL_EVT:
                    p_cb->ind[BTUI_AG_IND_SETUP] = '3';	// BTA_AG_CALLSETUP_ALERTING

                    bcmapp_ag_calc_held_ind();
                    data.ind.id = BTA_AG_IND_CALLHELD;
                    data.ind.value = p_cb->ind[BTUI_AG_IND_HELD] - '0';
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);

                    /* send originating and alerting */
                    data.audio_handle = handle;
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ORIG_RES, &data);
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_ALERT_RES, &data);
                    break;

                case BTUI_AG_HF_HANGUP_EVT:
                case BTUI_AG_END_CALL_EVT:
                    bcmapp_ag_get_call_stats(&num_active, &num_held);
                    /* If there are no active or held calls, sned end call */
                    if(!num_active && !num_held)
                    {
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                        p_cb->sco_owner = 0;
                        if (p_cb->play_fm)
                        {
                            p_cb->sco_owner = 2;
                            bt_ui_fm_cb.fm_pending = TRUE;
                            BTA_AgAudioOpen(p_cb->cur_handle);
                        }
#endif
                        p_cb->call_state = BTUI_AG_CALL_NONE;
                        p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                        p_cb->ind[BTUI_AG_IND_CALL] = '0';
                        p_cb->ind[BTUI_AG_IND_HELD] = '0';

                        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
                    }

                    bcmapp_ag_calc_held_ind();
                    data.ind.id = BTA_AG_IND_CALLHELD;
                    data.ind.value = p_cb->ind[BTUI_AG_IND_HELD] - '0';
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);
                    break;

                case BTUI_AG_HS_CKPD_EVT:
                    /*
                     * If SCO connection is open, end the call
                     * If not, setup SCO and transfer audio.
                     */
                    if (p_cb->app[app_id].is_audio_open)
                    {
#if( defined BTA_FM_INCLUDED ) && ( BTA_FM_INCLUDED == TRUE )
                        p_cb->sco_owner = 0;
                        if (p_cb->play_fm)
                        {
                            p_cb->sco_owner = 2;
                            bt_ui_fm_cb.fm_pending = TRUE;
                            BTA_AgAudioOpen(p_cb->cur_handle);
                        }
#endif

                        BT_UI_APPL_TRACE_DEBUG2("Ending call  handle:%d app_id:%d", handle, app_id);
                        p_cb->call_state = BTUI_AG_CALL_NONE;
                        p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                        p_cb->ind[BTUI_AG_IND_CALL] = '0';

                        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
                    }
                    else
                    {
                        BT_UI_APPL_TRACE_DEBUG2("Opening SCO  handle:%d app_id:%d", handle, app_id);
                        /* route audio */
                        data.audio_handle = handle;
                        p_cb->call_handle = handle;
                        BTA_AgAudioOpen(handle);
                    }
                    break;

                case BTUI_AG_HF_CANCEL_EVT:
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_CANCEL_RES, &data);
                    break;

                case BTUI_AG_IN_CALL_CONN_EVT:
                case BTUI_AG_IN_CALL_CONN_PH_EVT:
                case BTUI_AG_HF_ANS_EVT:
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';

                    /* route audio */
                    data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);
                    break;

                case BTUI_AG_OUT_CALL_CONN_EVT:
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';

                    /* route audio */
                    data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);

                    bcmapp_ag_calc_held_ind();
                    data.ind.id = BTA_AG_IND_CALLHELD;
                    data.ind.value = p_cb->ind[BTUI_AG_IND_HELD] - '0';
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &data);
                    break;

                case BTUI_AG_CALL_WAIT_EVT:
                    p_cb->ind[BTUI_AG_IND_SETUP] = '1';

                    /* send call waiting */
                    call_data = bcmapp_ag_get_call_data_in_call_setup();
                    if(call_data)
                        sprintf(data.str,"\"%s\"", call_data->dial_num);
                    else
                    {
                        BT_UI_APPL_TRACE_DEBUG0("No call in call setup to Answer");
                        strcpy(data.str, "\"8584535735\"");
                    }
                    strcat(data.str, ",129");
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_WAIT_RES, &data);
                    break;
            }
            break;

        case BTUI_AG_CALL_HELD:
            switch (event)
            {
                case BTUI_AG_HF_HANGUP_EVT:
                case BTUI_AG_END_CALL_EVT:
                case BTUI_AG_HS_CKPD_EVT:
                    p_cb->call_state = BTUI_AG_CALL_NONE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                    p_cb->ind[BTUI_AG_IND_CALL] = '0';
                    p_cb->btrh_state = BTA_AG_BTRH_NO_RESP;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
                    break;

                case BTUI_AG_IN_CALL_CONN_EVT:
                case BTUI_AG_HF_ANS_EVT:
                    /* route audio */
                    data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);
                    break;

                case BTUI_AG_OUT_CALL_CONN_EVT:
                    p_cb->call_state = BTUI_AG_CALL_ACTIVE;
                    p_cb->ind[BTUI_AG_IND_SETUP] = '0';

                    /* route audio */
                    if (p_cb->call_handle != BTA_AG_HANDLE_NONE)
                        data.audio_handle = p_cb->call_handle;
                    else
                        data.audio_handle = handle;

                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_OUT_CALL_CONN_RES, &data);
                    break;

                case BTUI_AG_CALL_WAIT_EVT:
                    p_cb->ind[BTUI_AG_IND_SETUP] = '1';

                    /* send call waiting */
                    strcpy(data.str, "\"8584535735\",129");
                    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_CALL_WAIT_RES, &data);
                    break;
                case BTUI_AG_HF_ANS_HELD_EVT:
                    if (p_cb->btrh_cmd == BTA_AG_BTRH_SET_ACC)
                    {
                        p_cb->call_state = BTUI_AG_CALL_ACTIVE;
                        /* route audio (call active) */
                        data.audio_handle = handle;

                        /* if accepted on AG, SCO should not be up. */
                        if(p_cb->btrh_no_sco)	// tonny
                        {
                            p_cb->btrh_no_sco = FALSE;
                        }
                        else
                            BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IN_CALL_CONN_RES, &data);
                    }
                    else if (p_cb->btrh_cmd == BTA_AG_BTRH_SET_REJ)
                    {
                        p_cb->call_state = BTUI_AG_CALL_NONE;
                        p_cb->ind[BTUI_AG_IND_SETUP] = '0';
                        p_cb->ind[BTUI_AG_IND_CALL] = '0';

                        for(i=0; i < BTUI_MAX_CALLS; i++)
                        {
                            if(bcm_ag_call_table[i].is_in_use &&
                               (bcm_ag_call_table[i].call_state == BTUI_CALL_STATE_ACTIVE))
                            {
                                bcmapp_ag_release_call_by_index(bcm_ag_call_table[i].call_index);
                                break;
                            }
                        }

                        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
                    }
                    p_cb->btrh_state = BTA_AG_BTRH_NO_RESP;
                    break;
            }
            break;
    }
}
#endif
#endif

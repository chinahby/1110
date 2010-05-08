/*****************************************************************************
**                                                                            
**  Name:             bcmapp_ag.h                                              
**                                                                            
**  Description:     This file contains bcmui internal interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2005, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/

#include "gki.h"

#ifndef BCMAPP_AG_H
#define BCMAPP_AG_H
#include "bt_ui_app.h"
#include "bta_ag_api.h"

#include "aeesound.h"
#include "appcomfunc.h"


#define PLAY_MUSIC_USING_BT_AG

#ifdef PLAY_MUSIC_USING_BT_AG

#include "vc0848.h"

#define BCMAPP_AG_SUCCESS              0
#define BCMAPP_AG_FAILURE               1
#define BCMAPP_AG_BADPARAM           2
#define BCMAPP_AG_BADSTATE            3
#define BCMAPP_AG_BAD_APPID          0xff
#define BCMAPP_AG_AUDIO_OPEN_FAILURE  -1

// The ag notify function prototype.
typedef void (*PFN_AG_NOTIFY)(void *pCxt, int nErr);

// Check there is bt devices connected. Return app id of the opened ag connection(-1 if there is not a opened ag connection).
UINT8 bcmapp_is_ag_connected(void);
// Open sco of the opened ag connection. Return the error code,  more information will be notified in the pfnNotify so the pfnNotify can not be NULL.
int bcmapp_ag_audio_open(UINT8 app_id, PFN_AG_NOTIFY pfnNotify, void *pCxt, 
        OEMSound_Volume_Type eVolume, BOOLEAN bUse848);
// Close sco of the opened ag connection.
int bcmapp_ag_audio_close(UINT8 app_id);
//Set parameter for playing music using bt device.
int bcmapp_ag_set_device(UINT8 app_id);

#endif

/* phone call-related events */
enum
{
    BTUI_AG_HF_DIAL_EVT,
    BTUI_AG_HF_HANGUP_EVT,
    BTUI_AG_HF_ANS_EVT,
    BTUI_AG_HF_ANS_HELD_EVT,
    BTUI_AG_HF_CANCEL_EVT,
    BTUI_AG_IN_CALL_EVT,
    BTUI_AG_IN_CALL_CONN_PH_EVT,
    BTUI_AG_IN_CALL_CONN_EVT,
    BTUI_AG_OUT_CALL_ORIG_EVT,
    BTUI_AG_OUT_CALL_ORIG_PH_EVT,
    BTUI_AG_OUT_CALL_CONN_EVT,
    BTUI_AG_OUT_CALL_CONN_PH_EVT,
    BTUI_AG_END_CALL_EVT,
    BTUI_AG_CALL_WAIT_EVT,
    BTUI_AG_HS_CKPD_EVT
};

#define BTUI_AG_IND_CALL    0
#define BTUI_AG_IND_SETUP   2
#define BTUI_AG_IND_SVC     4
#define BTUI_AG_IND_SIG     6
#define BTUI_AG_IND_ROAM    8
#define BTUI_AG_IND_BATT    10
#define BTUI_AG_IND_HELD    12
#define BTUI_AG_IND_LEN     14

/* call indicator values */
#define BTUI_AG_CALL_IND_INACTIVE        '0'   /* Phone call inactive */
#define BTUI_AG_CALL_IND_ACTIVE          '1'   /* Phone call active */

/* callsetup indicator values */
#define BTUI_AG_CALLSETUP_NONE       '0'   /* Not currently in call set up */
#define BTUI_AG_CALLSETUP_INCOMING   '1'   /* Incoming call process ongoing */
#define BTUI_AG_CALLSETUP_OUTGOING   '2'   /* Outgoing call set up is ongoing */
#define BTUI_AG_CALLSETUP_ALERTING   '3'   /* Remote party being alerted in an outgoing call */

#define BTUI_AG_ID_1        0
#ifdef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
#define BTUI_AG_ID_2        1

#define BTUI_AG_NUM_APP     2
#else
#define BTUI_AG_NUM_APP     1
#endif
/* phone call state */
enum
{
    BTUI_AG_CALL_NONE,//idle
    BTUI_AG_CALL_INC,//incoming
    BTUI_AG_CALL_OUT,//outgoing
    BTUI_AG_CALL_ACTIVE,//connect
    BTUI_AG_CALL_HELD
};

typedef struct
{
    tBTA_SERVICE_ID service;
    UINT16          handle;
    BOOLEAN         is_open;
    BOOLEAN         is_audio_open;
    UINT8           spk_vol;
    UINT8           mic_vol;
    BOOLEAN         bvra_active;
    BD_ADDR		bd_addr;

#ifdef PLAY_MUSIC_USING_BT_AG
    PFN_AG_NOTIFY pfnNotfiy;
    void *pCxt;
    int err_code;    
    AEECallback cb;
    ISound *pISound;
    OEMSound_Volume_Type eVolume;
    BOOLEAN need_reconnect;
    BOOLEAN use_848;
#endif

} tBTUI_AG_APP;

typedef struct
{
    tBTUI_AG_APP    app[BTUI_AG_NUM_APP];
    char            ind[BTUI_AG_IND_LEN];
    UINT16          call_handle;
    UINT8           call_state;
    BOOLEAN         inband_ring;
    UINT8           btrh_state;
    BOOLEAN         btrh_active;
    UINT8           btrh_cmd;
    BOOLEAN         btrh_no_sco;

#if (defined BTA_FM_INCLUDED && BTA_FM_INCLUDED == TRUE)
/* add for FM */
    BOOLEAN         play_fm;
    BOOLEAN         fm_disconnect;
    BOOLEAN         fm_connect;
    UINT16          cur_handle;
    UINT8           sco_owner; /* 0, none; 1: AG, 2: FM */
#endif

} tBTUI_AG_CB;


tBTA_SERVICE_MASK bcmapp_ag_enable(void);
void bcmapp_ag_disable(void);
void bcmapp_ag_close_conn(UINT8 app_id);
void bcmapp_ag_change_spk_volume(UINT8 app_id, BOOLEAN increase);
void bcmapp_ag_change_mic_volume(UINT8 app_id,BOOLEAN increase);
void bcmapp_ag_set_audio_device_authorized (tBTUI_REM_DEVICE * p_device_rec);
void bcmapp_ag_set_device_default_headset (tBTUI_REM_DEVICE * p_device_rec );
BOOLEAN bcmapp_ag_connect_device(tBTUI_REM_DEVICE * p_device_rec, BOOLEAN is_fm );

#if (defined BTA_FM_INCLUDED && BTA_FM_INCLUDED == TRUE)
extern void bcmui_view_audio_devices (void);
#endif

tBTUI_AG_CB * bcmapp_ag_get_ag_cb(void);
//tBTUI_CALL_DATA * bcmapp_ag_get_ag_call_table(void);
void bcmapp_ag_platform_init(void);
void bcmapp_ag_send_ok(UINT16 handle);
void bcmapp_ag_send_error(UINT16 handle, UINT16 errCode);

#endif

/*****************************************************************************
**                                                                           *
**  Name:          bt_ui.h                                                    *
**                                                                           *
**  Description:    This is the interface file for the bt_ui                  *
**                                                                             *
**                                                                           *
**  Copyright (c) 2000-2002, Widcomm Inc., All Rights Reserved.              *
**  Widcomm Bluetooth Core. Proprietary and confidential.                    *
******************************************************************************/
#include "bta_api.h"
#ifndef AEE_SIMULATOR
#include "Msg.h"
#endif
#ifndef BTUI_H
#define BTUI_H

#include "AEEFile.h"

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
#include "bta_fm_api.h"
#endif

#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
#include "bta_ac_api.h"
#endif

#if( defined BTA_AG_INCLUDED ) && (BTA_AG_INCLUDED == TRUE)
#include "bta_ag_api.h"
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
#include "bta_ct_api.h"
#endif
#ifdef FEATURE_CS09
#define FEATURE_CS09_BLUETOOTH_APP  //CS09 normal feature
#define FEATURE_CALL_LIST_APP              //make last call feature
#undef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
#undef FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
//#define BTUI_OPS_NEED_RECEIVE_ACK
#undef FEATURE_USE_OEM_FILEMGR            /*for support vm848*/
#undef FEATURE_BT_SHOW_OPP_BITRATE    /*for show opp ,ftp bitrate in proc state*/
#else
#define FEATURE_BLUETOOTH_APP
#define FEATURE_BLUETOOTH_DEVICE_IMAGE                //image sign for device such as headset,handset,etc
#define FEATURE_BLUETOOTH_DEVICE_VISIBILITY         //support visibility and can be find by other bt device
#define FEATURE_CALL_HISTORY_APP                            //make last call feature
#define FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY  //support item to number key  
#define FEATURE_BLUETOOTH_SUPPORT_TWO_CHANNEL
#define BTUI_OPS_NEED_RECEIVE_ACK
#define FEATURE_USE_OEM_FILEMGR                             /*for support vm848*/
//#define FEATURE_BT_SHOW_OPP_BITRATE                      /*for show opp ,ftp bitrate in proc state*/
//#define FEATURE_SET_REV_FILE_TO_PHONE_FIRST      /*for select store path,phone fisrt,if no define this feature,the phone can not receive file if not use sd card*/
#define FEATURE_BT_OPP_FILE_TYPE                          /*for select store path,distinguish from file type to save*/
#define FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
#endif

/* event masks for events to the task */
#define BTUI_MMI_EVT_MASK  0x0100  /* events from mmi to the BTUI application */
#define BTUI_BTA_EVT_MASK  0x0200  /* events from BTA to the BTUI application */
#ifndef FEATURE_OEM_DEBUG
#define BT_OEM_DEBUG
#endif
#if(!defined AEE_SIMULATOR) && (defined BT_OEM_DEBUG)
#define BCM_MSG_MED( str, a, b, c ) MSG_ERROR( str, a, b, c )
#define BCM_FUN_STAR( str, a, b, c ) MSG_ERROR( str, a, b, c )
#define BCM_WIN_MSG( str, a, b, c ) //MSG_ERROR( str, a, b, c )
#else
#define BCM_MSG_MED( str, a, b, c ) //MSG_ERROR( str, a, b, c )
#define BCM_FUN_STAR( str, a, b, c ) //MSG_ERROR( str, a, b, c )
#define BCM_WIN_MSG( str, a, b, c ) //MSG_ERROR( str, a, b, c )
#endif
/*typedef*/ enum {
    BTUI_UNKNOWN_DEVICE    ,            
    BTUI_DATA_BASE_FULL    ,            
    BTUI_NOT_INITIALISED,            
    BTUI_FAILED_TO_STORE_EVENT    ,    
    BTUI_INVALID_DATA,                
    BTUI_INVALID_EVENT,                
    BTUI_INVALID_ORIG,                
    BTUI_UNABLE_TO_CREATE_EVT_BUF,    
    BTUI_FAIL,                        
    BTUI_SUCCESS                    
 };

typedef UINT8 tBTUI_STATUS;


/* events from MMI */
enum
{
    BTUI_MMI_CMD      =     BTUI_MMI_EVT_MASK  
};

/* events from BTA */
enum
{

    BTUI_MMI_CONN_UP      =     BTUI_BTA_EVT_MASK,
    BTUI_MMI_CONN_DOWN,
    BTUI_MMI_PIN_REQ,
    BTUI_MMI_CFM_REQ,
    BTUI_MMI_KEY_NOTIF,
    BTUI_MMI_KEY_PRESS,
    BTUI_MMI_RMT_OOB,
    BTUI_MMI_AUTH_REQ,
    BTUI_MMI_AUTH_CMPL,
    BTUI_MMI_DISCV_CMP,
    BTUI_MMI_INQ_CMP,
    BTUI_MMI_INQ_RES,
    BTUI_MMI_SEARCH_DISCV_RES,
    BTUI_MMI_DEVICE_DISCV_RES,
    BTUI_MMI_LINK_DOWN,
    BTUI_MMI_LINK_UP,
    BTUI_MMI_ENABLE,
    BTUI_MMI_DISABLE,
    BTUI_MMI_INQ_RES_EVT,
    BTUI_MMI_INQ_CMPL_EVT,
    BTUI_MMI_DISC_RES_EVT,
    BTUI_MMI_DISC_CMPL_EVT,
    BTUI_MMI_SEARCH_CANCEL_CMPL_EVT,
    BTUI_MMI_BOND_CANCEL_CMPL_EVT,
#if ((BTU_DUAL_STACK_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE))
    BTUI_MMI_SWITCH_STACK_CMPL_EVT,
#endif

    BTUI_MMI_AG_AUDIO_OPEN,
    BTUI_MMI_AG_AUDIO_CLOSE,
    BTUI_MMI_AG_AT_CKPD_EVT,/*1. incoming->answer*//*2. connect ->end cal *//*3. idle->originate call*//*4. outgoing->not process*/
    BTUI_MMI_AG_AT_A_EVT,   /*1. incoming->answer*/
    BTUI_MMI_AG_AT_CHUP_EVT,/*2. connect ->end cal */
    BTUI_MMI_AG_AT_BLDN_EVT,/*3. idle->originate call*/
    BTUI_MMI_AG_AT_D_EVT,   /*originate val.str*/
    BTUI_MMI_AG_AT_CHLD_EVT,
    BTUI_MMI_AG_CONNECT,

    BTUI_MMI_CT_CALL_INCOMING,
    BTUI_MMI_CT_CALL_CONN,
    BTUI_MMI_CT_CALL_DISCONN,

    BTUI_MMI_OPC_OPEN,
    BTUI_MMI_OPC_OBJECT,
    BTUI_MMI_OPC_CLOSE,
    BTUI_MMI_OPC_PSHD,
    BTUI_MMI_OPC_PROC,

    BTUI_MMI_OPS_OBJECT,
    BTUI_MMI_OPS_PROC,
    BTUI_MMI_OPS_ACCESS,
    BTUI_MMI_OPS_CLOSE,
    BTUI_MMI_SC_OPEN,
    BTUI_MMI_SC_CLOSE,

    BTUI_MMI_FTS_ACCESS,
    BTUI_MMI_FTS_PROGRESS,
    BTUI_MMI_FTS_CMPL,
    BTUI_MMI_FTS_CLOSE,

    BTUI_MMI_FTC_ENABLE,
    BTUI_MMI_FTC_OPEN,
    BTUI_MMI_FTC_BI_CAPS,
    BTUI_MMI_FTC_THUMBNAIL,
    BTUI_MMI_FTC_CLOSE,
    BTUI_MMI_FTC_LIST,
    BTUI_MMI_FTC_REMOVE,
    BTUI_MMI_FTC_MKDIR,
    BTUI_MMI_FTC_CHDIR,
    BTUI_MMI_FTC_PUT,
    BTUI_MMI_FTC_GET,

    BTUI_MMI_PBC_ENABLE,
    BTUI_MMI_PBC_OPEN,
    BTUI_MMI_PBC_CLOSE,
    BTUI_MMI_PBC_LIST,
    BTUI_MMI_PBC_CHDIR,
    BTUI_MMI_PBC_GET,

    BTUI_MMI_HD_OPEN,
    BTUI_MMI_HD_CLOSE,

    BTUI_MMI_PR_GETCAPS,
    BTUI_MMI_PR_PROGRESS,
    BTUI_MMI_PR_THUMBNAIL,
    BTUI_MMI_PR_CLOSE,

    BTUI_MMI_AV_AUDIO_EOF_EVT,
    BTUI_MMI_AV_AUDIO_FERR_EVT,
    BTUI_MMI_AV_AUDIO_ERR_EVT,
    BTUI_MMI_AV_VIDEO_EOF_EVT,
    BTUI_MMI_AV_VIDEO_FERR_EVT,

    BTUI_MMI_ACC_ENABLE,
    BTUI_MMI_ACC_OPEN,
    BTUI_MMI_ACC_CLOSE,
    BTUI_MMI_ACC_TEST_STATUS,
    BTUI_MMI_ACC_GET_MONITOR,
    BTUI_MMI_ACC_THUMBNAIL,
    BTUI_MMI_ACC_GET_IMAGE,
    BTUI_MMI_ACC_PROPERTIES,
    BTUI_MMI_ACC_CLOSE_MENU,
    BTUI_MMI_ACC_ARCHIVE_CLOSE,

    BTUI_MMI_ACS_OPEN,
    BTUI_MMI_ACS_PUT_FINAL,

    BTUI_MMI_SS_ENABLED,
    BTUI_MMI_SS_DISABLED,
    BTUI_MMI_SS_SYNC_CMD,

    BTUI_MMI_HH_ENABLE,
    BTUI_MMI_HH_OPEN,
    BTUI_MMI_HH_CLOSE,
    BTUI_MMI_HH_GETDSCP,
    BTUI_MMI_HH_SETRPT,
    BTUI_MMI_HH_VCUNPLUG,

    BTUI_MMI_FM_ENABLE,
    BTUI_MMI_FM_BT_OPEN,
    BTUI_MMI_FM_TUNED,
    BTUI_MMI_FM_AF_JUMP,
    BTUI_MMI_FM_SEARCHED,
    BTUI_MMI_FM_SCH_CMPL,
    BTUI_MMI_FM_AUDIO_DATA,
    BTUI_MMI_FM_AUDIO_MODE,
    BTUI_MMI_FM_AUD_MUTE,
    BTUI_MMI_FM_SCAN_STEP,
    BTUI_MMI_FM_DEEMPHA,
    BTUI_MMI_FM_BT_DROP,
    BTUI_MMI_RDS_PS,
    BTUI_MMI_RDS_PTY,
    BTUI_MMI_RDS_PTYN,
    BTUI_MMI_RDS_RT,
    BTUI_MMI_RDS_AF,
    BTUI_MMI_RDS_INDICATOR,
    BTUI_MMI_RDS_MODE,
    BTUI_MMI_FM_DISABLE

};


#define BTUI_MMI_DM_EVT_FIRST BTUI_MMI_CONN_UP
#if ((BTU_DUAL_STACK_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE))
#define BTUI_MMI_DM_EVT_LAST  BTUI_MMI_SWITCH_STACK_CMPL_EVT
#else
#define BTUI_MMI_DM_EVT_LAST  BTUI_MMI_BOND_CANCEL_CMPL_EVT
#endif

#define BTUI_MMI_AG_EVT_FIRST BTUI_MMI_AG_AUDIO_OPEN
#define BTUI_MMI_AG_EVT_LAST  BTUI_MMI_AG_AUDIO_CLOSE

#define BTUI_MMI_CT_EVT_FIRST BTUI_MMI_CT_CALL_INCOMING
#define BTUI_MMI_CT_EVT_LAST  BTUI_MMI_CT_CALL_DISCONN

#define BTUI_MMI_OPC_EVT_FIRST BTUI_MMI_OPC_OBJECT
#define BTUI_MMI_OPC_EVT_LAST  BTUI_MMI_OPC_PSHD

#define BTUI_MMI_OPS_EVT_FIRST BTUI_MMI_OPS_OBJECT
#define BTUI_MMI_OPS_EVT_LAST  BTUI_MMI_OPS_OBJECT

#define BTUI_MMI_SC_EVT_FIRST BTUI_MMI_SC_OPEN
#define BTUI_MMI_SC_EVT_LAST  BTUI_MMI_SC_CLOSE

#define BTUI_MMI_FTC_EVT_FIRST BTUI_MMI_FTC_ENABLE
#define BTUI_MMI_FTC_EVT_LAST  BTUI_MMI_FTC_GET

#define BTUI_MMI_PBC_EVT_FIRST BTUI_MMI_PBC_ENABLE
#define BTUI_MMI_PBC_EVT_LAST  BTUI_MMI_PBC_GET

#define BTUI_MMI_HD_EVT_FIRST BTUI_MMI_HD_OPEN
#define BTUI_MMI_HD_EVT_LAST  BTUI_MMI_HD_CLOSE

#define BTUI_MMI_HH_EVT_FIRST BTUI_MMI_HH_ENABLE
#define BTUI_MMI_HH_EVT_LAST  BTUI_MMI_HH_VCUNPLUG

#define BTUI_MMI_PR_EVT_FIRST BTUI_MMI_PR_GETCAPS
#define BTUI_MMI_PR_EVT_LAST  BTUI_MMI_PR_CLOSE

#define BTUI_MMI_AV_EVT_FIRST BTUI_MMI_AV_AUDIO_EOF_EVT
#define BTUI_MMI_AV_EVT_LAST  BTUI_MMI_AV_VIDEO_FERR_EVT

#define BTUI_MMI_ACC_EVT_FIRST BTUI_MMI_ACC_ENABLE
#define BTUI_MMI_ACC_EVT_LAST  BTUI_MMI_ACC_ARCHIVE_CLOSE

#define BTUI_MMI_ACS_EVT_FIRST BTUI_MMI_ACS_OPEN
#define BTUI_MMI_ACS_EVT_LAST  BTUI_MMI_ACS_PUT_FINAL

#define BTUI_MMI_SS_EVT_FIRST BTUI_MMI_SS_ENABLED
#define BTUI_MMI_SS_EVT_LAST  BTUI_MMI_SS_SYNC_CMD

#define BTUI_MMI_FM_EVT_FIRST BTUI_MMI_FM_ENABLE
#define BTUI_MMI_FM_EVT_LAST  BTUI_MMI_FM_DISABLE


#define BTUI_DATA_LEN          56
#define BTUI_DEV_NAME_LENGTH   17

/* data type for MMI message */
typedef struct
{
    BT_HDR    hdr;
    UINT8     data[BTUI_DATA_LEN+1];
} tBTUI_MMI_MSG;

/* data type for connection up event */
typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    tBTA_STATUS status;
    tBTA_SERVICE_ID service;

} tBTUI_CONN_UP_MSG;

/* data type for connection down event */
typedef struct
{
    BT_HDR    hdr;
    tBTA_SERVICE_ID service;
    BD_ADDR   bd_addr;
    UINT8     status;

} tBTUI_CONN_DOWN_MSG;

/* data type for pin request message */
typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    char      dev_name[BTUI_DEV_NAME_LENGTH+1];
    DEV_CLASS dev_class;
} tBTUI_PIN_REQ_MSG;

/* data type for user confirm request message */
typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    char      dev_name[BTUI_DEV_NAME_LENGTH+1];
    DEV_CLASS dev_class;
    UINT32    num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN   just_works;     /* TRUE, if "Just Works" association model */
} tBTUI_CFM_REQ_MSG;

/* data type for user confirm request message */
typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    char      dev_name[BTUI_DEV_NAME_LENGTH+1];
    DEV_CLASS dev_class;
    UINT32    passkey;        /* the numeric value for comparison. If just_works, do not show this number to UI */
} tBTUI_KEY_NOT_MSG;

typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    tBTA_SP_KEY_TYPE    notif_type;
} tBTUI_KEY_PRE_MSG;

/* data type for remote oob request message */
typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    char      dev_name[BTUI_DEV_NAME_LENGTH+1];
    DEV_CLASS dev_class;
} tBTUI_RMT_OOB_MSG;

typedef struct
{
    BT_HDR    hdr;
    BD_ADDR   bd_addr;
    char      dev_name[BTUI_DEV_NAME_LENGTH+1];
    tBTA_SERVICE_ID service;

} tBTUI_AUTH_REQ_MSG;


typedef struct
{
    BT_HDR      hdr;
    BD_ADDR    bd_addr;
    UINT8        dev_name[BTUI_DEV_NAME_LENGTH+1];
    BOOLEAN        is_success;
}
tBTUI_DM_AUTH_CMPL;

typedef struct
{
    BT_HDR    hdr;
    BD_ADDR    bd_addr;
    UINT8        dev_name[BTUI_DEV_NAME_LENGTH+1];
    tBTA_SERVICE_MASK services;
    tBTA_STATUS       result;
} tBTUI_DISC_MSG;

typedef struct
{
       BT_HDR      hdr;
    BD_ADDR    bd_addr;
}
tBTUI_DM_LINK_CHANGE;

typedef struct
{
    BT_HDR      hdr;
    UINT16       data;
    /* length should be AG_AT_STR_MAX_LENGTH. Don't know where it is defined! */
    char            str[50];
} tBTUI_BTA_AG_AT_MSG;

/* data type for MMI message */
typedef struct
{
    BT_HDR    hdr;
    BT_HDR     *data;
    UINT8       app_id;
} tBTUI_DATA_MSG;

/* data type for MMI message */
typedef struct
{
    BT_HDR    hdr;
    UINT8     status;
} tBTUI_OPC_CLOSE;

/* data type for MMI message */
typedef struct
{
    BT_HDR    hdr;
    UINT8     format;
    char      name[100];/* move */
} tBTUI_OPS_OBJECT;

typedef struct
{
    BT_HDR    hdr;
    UINT8     status;

} tBTUI_FTC_STATUS;

typedef struct
{
    BT_HDR    hdr;
    UINT8     status;

} tBTUI_PBC_STATUS;

typedef struct
{
    BT_HDR    hdr;
    UINT8     status;

} tBTUI_SS_STATUS;
 
typedef struct
{
    BT_HDR    hdr;
    UINT8     status;
    BOOLEAN   final;
    BOOLEAN   parse_status;
} tBTUI_FTC_LIST;

typedef struct
{
    BT_HDR    hdr;
    UINT8     status;
    BOOLEAN   final;
    BOOLEAN   parse_status;
} tBTUI_PBC_LIST;

typedef struct
{
    BT_HDR    hdr;
    tBTA_SERVICE_MASK   services; 

} tBTUI_PR_GETCAPS;

typedef struct
{
    BT_HDR    hdr;
    UINT8     bta_event; 

} tBTUI_ACC_ARCHIVE_CBACK;

typedef struct
{
    BT_HDR    hdr;
    UINT8     obx_rsp; 

} tBTUI_ACC_TEST_STATUS;

typedef struct
{
    BT_HDR    hdr;
    UINT8     img_hdl[/*change*/ 20]; 

} tBTUI_ACC_GET_MONITOR;

typedef struct
{
    BT_HDR    hdr;
    UINT8     img_hdl[/*change*/ 20]; 

} tBTUI_ACC_GET_THUMBNAIL;

typedef struct
{
    BT_HDR    hdr;
    UINT32    menu; 
} tBTUI_ACC_CLOSE_MENU;

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)

typedef struct
{
    BT_HDR        hdr;
} tBTUI_FM_RDS_DATA;


typedef struct
{
    BT_HDR      hdr;
    UINT16      num_af;
    UINT16      af_list[BTA_FM_AF_MAX_NUM];
    UINT8       af_type[BTA_FM_AF_MAX_NUM];
} tBTUI_FM_AF_DATA;

typedef struct
{
    BT_HDR      hdr;
    UINT8       indicator;
    UINT8       enable;
} tBTUI_FM_RDS_IND;

typedef struct
{
    BT_HDR          hdr;
    tBTA_FM_STATUS  status;
    UINT8           rssi;
    UINT16          freq;
} tBTUI_FM_CHNL_DATA;

#endif


typedef struct
{
    BT_HDR      hdr;
    tBTUI_STATUS    result;
} tBTUI_BOND_CANCEL_CMPL;

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
typedef struct
{
    BT_HDR      hdr;
    tBTUI_STATUS    result;
} tBTUI_SWITCH_STACK_CMPL;
#endif
typedef struct
{
    BT_HDR      hdr;
    tBTA_AG_VAL   ag_val;
} tBTUI_AG_VAL;

typedef struct
{
    UINT32      bytes_transferred;
    UINT32      obj_size;
} tBTUI_OPP_PROC_VAL;

typedef struct
{
    BT_HDR      hdr;
    tBTUI_OPP_PROC_VAL val;
} tBTUI_OPP_PROC;

#ifdef BTUI_OPS_NEED_RECEIVE_ACK
#ifndef BTUI_MAX_PATH_LENGTH
#define BTUI_MAX_PATH_LENGTH AEE_MAX_FILE_NAME
#endif

typedef struct
{
    BT_HDR    hdr;
    char                p_name[BTUI_MAX_PATH_LENGTH+1];        /* Object name */
    char                p_type[20];        /* Object type (NULL if not specified) */
    UINT32              size;           /* Object size */
    tBTM_BD_NAME        dev_name;       /* Name of device, "" if unknown */
    BD_ADDR             bd_addr;        /* Address of device */
    UINT8        oper;           /* Operation (push or pull) */
    UINT8         format;         /* Object format */
} tBTUI_OPS_ACCESS;
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
typedef struct
{
    BT_HDR    hdr;
    UINT8                   handle;
    UINT8                   app_id;    
    BD_ADDR                 bd_addr;
    tBTA_CT_CALL_CLASS      call_class;
    tBTA_CT_IE             p_ie;
    tBTA_CT_SIGNAL          signal_value;
} tBTUI_CT_INCOMING;
#endif
/* union of all message type */
typedef union
{
    BT_HDR    hdr;
    tBTUI_CONN_UP_MSG   open;
    tBTUI_CONN_DOWN_MSG close;
    tBTUI_PIN_REQ_MSG   pin_req;
    tBTUI_CFM_REQ_MSG   cfm_req;
    tBTUI_KEY_NOT_MSG   key_notif;
    tBTUI_KEY_PRE_MSG   key_press;    
    tBTUI_RMT_OOB_MSG   rmt_oob;
    tBTUI_AUTH_REQ_MSG  auth_req;
    tBTUI_DISC_MSG disc_result;
    tBTUI_DM_AUTH_CMPL auth_cmpl;
    tBTUI_DM_LINK_CHANGE link_change;
    tBTUI_BTA_AG_AT_MSG ag_msg;
    tBTUI_DATA_MSG data_msg;
    tBTUI_OPC_CLOSE opc_close;
    tBTUI_OPS_OBJECT ops_object;
    tBTUI_FTC_STATUS ftc_status;
    tBTUI_PBC_STATUS pbc_status;
    tBTUI_SS_STATUS  ss_status;
    tBTUI_FTC_LIST   ftc_list;
    tBTUI_PBC_LIST   pbc_list;
    tBTUI_PR_GETCAPS get_caps;
    tBTUI_ACC_ARCHIVE_CBACK acc_archive_cback;
    tBTUI_ACC_TEST_STATUS  acc_test_status;
    tBTUI_ACC_GET_MONITOR  acc_get_monitor;
    tBTUI_ACC_GET_THUMBNAIL acc_get_thumbmail;
    tBTUI_ACC_CLOSE_MENU    acc_close_menu;
#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
    tBTUI_FM_RDS_DATA rds_data;  
    tBTUI_FM_AF_DATA af_data;
    tBTUI_FM_RDS_IND rds_ind;
    tBTUI_FM_CHNL_DATA  chnl_data;
#endif
    tBTUI_BOND_CANCEL_CMPL bond_cancel_cmpl;
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    tBTUI_SWITCH_STACK_CMPL switch_stack_cmpl;
#endif
    tBTUI_AG_VAL   ag_val;
    tBTUI_OPP_PROC opp_proc;
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
    tBTUI_OPS_ACCESS ops_access;
#endif
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
    tBTUI_CT_INCOMING ct_incoming;
#endif
} tBTUI_BTA_MSG;




extern UINT8 appl_trace_level;

//void bt_ui_test_event_hdlr(tBTUI_MMI_MSG *p_msg);
//void bt_ui_mmi_evt_hdlr(tBTUI_MMI_MSG *p_msg);
//void bt_ui_bta_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_main_init (void);
void bt_ui_bta_dm_evt_hdlr(tBTUI_BTA_MSG *p_msg);
void bt_ui_bta_ag_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#if( defined BTA_CT_INCLUDED ) && (BTA_CT_INCLUDED == TRUE)
void bt_ui_bta_ct_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#endif
//void bt_ui_bta_ftc_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#if( defined BTA_FT_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
void bt_ui_bta_fts_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#endif
//void bt_ui_bta_pbc_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
void bt_ui_bta_opc_evt_hdlr(tBTUI_BTA_MSG *p_msg);
void bt_ui_bta_ops_evt_hdlr(tBTUI_BTA_MSG *p_msg);
#endif //wuraojin 20081215
//void bt_ui_bta_sc_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_hd_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_acc_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_acs_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_pr_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_ss_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_av_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_hh_evt_hdlr(tBTUI_BTA_MSG *p_msg);
//void bt_ui_bta_fm_evt_hdlr(tBTUI_BTA_MSG *p_msg);

/* Local Constants */
#ifndef BTUI_AG_SECURITY
#define BTUI_AG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif
#ifndef BTUI_HSAG_SERVICE_NAME
#define BTUI_HSAG_SERVICE_NAME "Headset Gateway"
#endif
#ifndef BTUI_HFAG_SERVICE_NAME
#define BTUI_HFAG_SERVICE_NAME "Handsfree Gateway"
#endif

#ifndef BTUI_SPPDG_SECURITY
#define BTUI_SPPDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif
#ifndef BTUI_SPPDG_SERVICE_NAME
#define BTUI_SPPDG_SERVICE_NAME "Serial Port"
#endif

#ifndef BTUI_DUNDG_SECURITY
#define BTUI_DUNDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_DUNDG_SERVICE_NAME
#define BTUI_DUNDG_SERVICE_NAME "Dial-up Networking"
#endif

#ifndef BTUI_FAXDG_SECURITY
#define BTUI_FAXDG_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FAXDG_SERVICE_NAME
#define BTUI_FAXDG_SERVICE_NAME "Fax"
#endif


/* SIM Card server defaults */
#ifndef BTUI_SC_SECURITY
#define BTUI_SC_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif
#ifndef BTUI_SC_SERVICE_NAME
#define BTUI_SC_SERVICE_NAME "SIM Access"
#endif


#ifndef BTUI_OPS_SECURITY
#define BTUI_OPS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_OPS_SERVICE_NAME
#define BTUI_OPS_SERVICE_NAME "Object Push"
#endif

#ifndef BTUI_OPC_SECURITY
#define BTUI_OPC_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_OPC_SERVICE_NAME
#define BTUI_OPC_SERVICE_NAME "Object Push"
#endif

#ifndef BTUI_FTS_SECURITY
#define BTUI_FTS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_FTS_SERVICE_NAME
#define BTUI_FTS_SERVICE_NAME "OBEX File Transfer"
#endif

#ifndef BTUI_FTS_KEY
#define BTUI_FTS_KEY "1234"
#endif

#ifndef BTUI_FTS_REALM
#define BTUI_FTS_REALM "guest"
#endif

#ifndef BTUI_PBS_SECURITY
#define BTUI_PBS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_PBS_SERVICE_NAME
#define BTUI_PBS_SERVICE_NAME "Phonebook Access PSE"
#endif

#ifndef BTUI_PBS_KEY
#define BTUI_PBS_KEY "1234"
#endif

#ifndef BTUI_PBS_REALM
#define BTUI_PBS_REALM "guest"
#endif

#ifndef BTUI_ACC_SECURITY
#define BTUI_ACC_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_ACC_CAM_GET_IMAGE
#define BTUI_ACC_CAM_GET_IMAGE  TRUE
#endif

#ifndef BTUI_ACS_SECURITY
#define BTUI_ACS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
#endif

#ifndef BTUI_ACS_SERVICE_NAME
#define BTUI_ACS_SERVICE_NAME "Advanced Camera"
#endif

#ifndef BTUI_ACS_REQUEST_THUMB
#define BTUI_ACS_REQUEST_THUMB  TRUE
#endif

#if( defined BTA_AC_INCLUDED ) && (BTA_AC_INCLUDED == TRUE)
#ifndef BTUI_ACS_FEATURES
#define BTUI_ACS_FEATURES   (BIP_FT_IMG_PUSH|BIP_FT_IMG_PULL|BIP_FT_REMOTE_CAMERA)
#endif
#endif

#ifndef BTUI_ACS_CAPACITY
#define BTUI_ACS_CAPACITY   "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#endif

#define BTUI_SS_SECURITY (BTA_SEC_AUTHORIZE | BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)

#define BTUI_SS_SERVICE_NAME "OBEX Synchronization"

#define BTUI_SS_KEY "1234"

#define BTUI_SS_REALM "guest"

#define BTUI_SS_DATA_TYPES (BTA_SS_API_VCARD | BTA_SS_API_VCAL | BTA_SS_API_VNOTE | BTA_SS_API_VMSG)

#define SS_MAX_LUID_SIZE        50
#define SS_MAX_CL_ENTRIES       20
#define SS_MAX_PIM_ENTRIES      40
#define SS_MAX_DID_SIZE         10

#define BTUI_MENU_ACT_BOND          0x1
#define BTUI_MENU_ACT_BOND_N_DISC   0x2

#endif

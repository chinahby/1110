/*****************************************************************************
**
**  Name:           bta_aa_api.h
**
**  Description:    This is the public interface file for the advanced
**                  audio streaming (AA) subsystem of BTA, Widcomm's 
**                  Bluetooth application layer for mobile phones.
**
**  Copyright (c) 2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AA_API_H
#define BTA_AA_API_H

#include "avrc_api.h"
#include "avdt_api.h"
#include "a2d_api.h"
#include "bta_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
#ifndef BTA_AA_MAX_SEPS
#define BTA_AA_MAX_SEPS         2
#endif

/* status values */
#define BTA_AA_SUCCESS          0       /* operation successful */
#define BTA_AA_FAIL             1       /* generic failure */
#define BTA_AA_FAIL_SDP         2       /* service not found */
#define BTA_AA_FAIL_STREAM      3       /* stream connection failed */

typedef UINT8 tBTA_AA_STATUS;

/* AA features masks */
#define BTA_AA_FEAT_RCTG        0x0001  /* remote control target */
#define BTA_AA_FEAT_RCCT        0x0002  /* remote control controller */
#define BTA_AA_FEAT_PROTECT     0x0004  /* streaming media contect protection */
#define BTA_AA_FEAT_VENDOR      0x0008  /* remote control vendor dependent commands */
#define BTA_AA_FEAT_SUSPEND     0x0010  /* automatically sends SUSPEND when SNK sends START */

typedef UINT16 tBTA_AA_FEAT;

/* codec type */
#define BTA_AA_CODEC_SBC        A2D_MEDIA_CT_SBC    /* SBC media codec type */
#define BTA_AA_CODEC_M12        A2D_MEDIA_CT_M12    /* MPEG-1, 2 Audio media codec type */
#define BTA_AA_CODEC_M24        A2D_MEDIA_CT_M24    /* MPEG-2, 4 AAC media codec type */
#define BTA_AA_CODEC_ATRAC      A2D_MEDIA_CT_ATRAC  /* ATRAC family media codec type */

typedef UINT8 tBTA_AA_CODEC;

/* operation id list for BTA_AaRemoteCmd */
#define BTA_AA_RC_SELECT        AVRC_ID_SELECT      /* select */
#define BTA_AA_RC_UP            AVRC_ID_UP          /* up */
#define BTA_AA_RC_DOWN          AVRC_ID_DOWN        /* down */
#define BTA_AA_RC_LEFT          AVRC_ID_LEFT        /* left */
#define BTA_AA_RC_RIGHT         AVRC_ID_RIGHT       /* right */
#define BTA_AA_RC_RIGHT_UP      AVRC_ID_RIGHT_UP    /* right-up */
#define BTA_AA_RC_RIGHT_DOWN    AVRC_ID_RIGHT_DOWN  /* right-down */
#define BTA_AA_RC_LEFT_UP       AVRC_ID_LEFT_UP     /* left-up */
#define BTA_AA_RC_LEFT_DOWN     AVRC_ID_LEFT_DOWN   /* left-down */
#define BTA_AA_RC_ROOT_MENU     AVRC_ID_ROOT_MENU   /* root menu */
#define BTA_AA_RC_SETUP_MENU    AVRC_ID_SETUP_MENU  /* setup menu */
#define BTA_AA_RC_CONT_MENU     AVRC_ID_CONT_MENU   /* contents menu */
#define BTA_AA_RC_FAV_MENU      AVRC_ID_FAV_MENU    /* favorite menu */
#define BTA_AA_RC_EXIT          AVRC_ID_EXIT        /* exit */
#define BTA_AA_RC_0             AVRC_ID_0           /* 0 */
#define BTA_AA_RC_1             AVRC_ID_1           /* 1 */
#define BTA_AA_RC_2             AVRC_ID_2           /* 2 */
#define BTA_AA_RC_3             AVRC_ID_3           /* 3 */
#define BTA_AA_RC_4             AVRC_ID_4           /* 4 */
#define BTA_AA_RC_5             AVRC_ID_5           /* 5 */
#define BTA_AA_RC_6             AVRC_ID_6           /* 6 */
#define BTA_AA_RC_7             AVRC_ID_7           /* 7 */
#define BTA_AA_RC_8             AVRC_ID_8           /* 8 */
#define BTA_AA_RC_9             AVRC_ID_9           /* 9 */
#define BTA_AA_RC_DOT           AVRC_ID_DOT         /* dot */
#define BTA_AA_RC_ENTER         AVRC_ID_ENTER       /* enter */
#define BTA_AA_RC_CLEAR         AVRC_ID_CLEAR       /* clear */
#define BTA_AA_RC_CHAN_UP       AVRC_ID_CHAN_UP     /* channel up */
#define BTA_AA_RC_CHAN_DOWN     AVRC_ID_CHAN_DOWN   /* channel down */
#define BTA_AA_RC_PREV_CHAN     AVRC_ID_PREV_CHAN   /* previous channel */
#define BTA_AA_RC_SOUND_SEL     AVRC_ID_SOUND_SEL   /* sound select */
#define BTA_AA_RC_INPUT_SEL     AVRC_ID_INPUT_SEL   /* input select */
#define BTA_AA_RC_DISP_INFO     AVRC_ID_DISP_INFO   /* display information */
#define BTA_AA_RC_HELP          AVRC_ID_HELP        /* help */
#define BTA_AA_RC_PAGE_UP       AVRC_ID_PAGE_UP     /* page up */
#define BTA_AA_RC_PAGE_DOWN     AVRC_ID_PAGE_DOWN   /* page down */
#define BTA_AA_RC_POWER         AVRC_ID_POWER       /* power */
#define BTA_AA_RC_VOL_UP        AVRC_ID_VOL_UP      /* volume up */
#define BTA_AA_RC_VOL_DOWN      AVRC_ID_VOL_DOWN    /* volume down */
#define BTA_AA_RC_MUTE          AVRC_ID_MUTE        /* mute */
#define BTA_AA_RC_PLAY          AVRC_ID_PLAY        /* play */
#define BTA_AA_RC_STOP          AVRC_ID_STOP        /* stop */
#define BTA_AA_RC_PAUSE         AVRC_ID_PAUSE       /* pause */
#define BTA_AA_RC_RECORD        AVRC_ID_RECORD      /* record */
#define BTA_AA_RC_REWIND        AVRC_ID_REWIND      /* rewind */
#define BTA_AA_RC_FAST_FOR      AVRC_ID_FAST_FOR    /* fast forward */
#define BTA_AA_RC_EJECT         AVRC_ID_EJECT       /* eject */
#define BTA_AA_RC_FORWARD       AVRC_ID_FORWARD     /* forward */
#define BTA_AA_RC_BACKWARD      AVRC_ID_BACKWARD    /* backward */
#define BTA_AA_RC_ANGLE         AVRC_ID_ANGLE       /* angle */
#define BTA_AA_RC_SUBPICT       AVRC_ID_SUBPICT     /* subpicture */
#define BTA_AA_RC_F1            AVRC_ID_F1          /* F1 */
#define BTA_AA_RC_F2            AVRC_ID_F2          /* F2 */
#define BTA_AA_RC_F3            AVRC_ID_F3          /* F3 */
#define BTA_AA_RC_F4            AVRC_ID_F4          /* F4 */
#define BTA_AA_RC_F5            AVRC_ID_F5          /* F5 */

typedef UINT8 tBTA_AA_RC;

/* state flag for pass through command */
#define BTA_AA_STATE_PRESS      AVRC_STATE_PRESS    /* key pressed */
#define BTA_AA_STATE_RELEASE    AVRC_STATE_RELEASE  /* key released */

typedef UINT8 tBTA_AA_STATE;

/* command codes for BTA_AaVendorCmd */
#define BTA_AA_CMD_CTRL         AVRC_CMD_CTRL    
#define BTA_AA_CMD_STATUS       AVRC_CMD_STATUS  
#define BTA_AA_CMD_SPEC_INQ     AVRC_CMD_SPEC_INQ                 
#define BTA_AA_CMD_NOTIF        AVRC_CMD_NOTIF   
#define BTA_AA_CMD_GEN_INQ      AVRC_CMD_GEN_INQ

/* response codes for BTA_AaVendorRsp */
#define BTA_AA_RSP_NOT_IMPL     AVRC_RSP_NOT_IMPL 
#define BTA_AA_RSP_ACCEPT       AVRC_RSP_ACCEPT   
#define BTA_AA_RSP_REJ          AVRC_RSP_REJ      
#define BTA_AA_RSP_IN_TRANS     AVRC_RSP_IN_TRANS 
#define BTA_AA_RSP_IMPL_STBL    AVRC_RSP_IMPL_STBL
#define BTA_AA_RSP_CHANGED      AVRC_RSP_CHANGED                            
#define BTA_AA_RSP_INTERIM      AVRC_RSP_INTERIM  

typedef UINT8 tBTA_AA_CODE;

/* error codes for BTA_AaProtectRsp */
#define BTA_AA_ERR_NONE             A2D_SUCCESS         /* Success, no error */
#define BTA_AA_ERR_BAD_STATE        AVDT_ERR_BAD_STATE  /* Message cannot be processed in this state */
#define BTA_AA_ERR_RESOURCE         AVDT_ERR_RESOURCE   /* Insufficient resources */
#define BTA_AA_ERR_BAD_CP_TYPE      A2D_BAD_CP_TYPE     /* The requested Content Protection Type is not supported */
#define BTA_AA_ERR_BAD_CP_FORMAT    A2D_BAD_CP_FORMAT   /* The format of Content Protection Data is not correct */
                                                    
typedef UINT8 tBTA_AA_ERR;

/* AA callback events */
#define BTA_AA_ENABLE_EVT       0       /* AA enabled */
#define BTA_AA_OPEN_EVT         1       /* connection opened */
#define BTA_AA_CLOSE_EVT        2       /* connection closed */
#define BTA_AA_START_EVT        3       /* stream data transfer started */
#define BTA_AA_STOP_EVT         4       /* stream data transfer stopped */
#define BTA_AA_PROTECT_REQ_EVT  5       /* content protection request */
#define BTA_AA_PROTECT_RSP_EVT  6       /* content protection response */
#define BTA_AA_RC_OPEN_EVT      7       /* remote control channel open */
#define BTA_AA_RC_CLOSE_EVT     8       /* remote control channel closed */
#define BTA_AA_REMOTE_CMD_EVT   9       /* remote control command */
#define BTA_AA_REMOTE_RSP_EVT   10      /* remote control response */
#define BTA_AA_VENDOR_CMD_EVT   11      /* vendor dependent remote control command */
#define BTA_AA_VENDOR_RSP_EVT   12      /* vendor dependent remote control response */
#define BTA_AA_RECONFIG_EVT     13      /* reconfigure response */
#define BTA_AA_SUSPEND_EVT      14      /* suspend response */
#define BTA_AA_PENDING_EVT      15      /* incoming connection pending:
                                         * signal channel is open and media channel is not open
                                         * after BTA_AA_STR_INCOMING_TIME_VAL ms */

typedef UINT8 tBTA_AA_EVT;

/* data associated with BTA_AA_OPEN_EVT */
typedef struct
{
    BD_ADDR         bd_addr;
    tBTA_AA_STATUS  status;
} tBTA_AA_OPEN;

/* data associated with BTA_AA_START_EVT */
typedef struct
{
    tBTA_AA_STATUS  status;
    BOOLEAN         suspending;
} tBTA_AA_START;

/* data associated with BTA_AA_SUSPEND_EVT */
typedef struct
{
    tBTA_AA_STATUS  status;
} tBTA_AA_SUSPEND;

/* data associated with BTA_AA_RECONFIG_EVT */
typedef struct
{
    tBTA_AA_STATUS  status;
} tBTA_AA_RECONFIG;

/* data associated with BTA_AA_PROTECT_REQ_EVT */
typedef struct
{
    UINT8           *p_data;
    UINT16          len;
} tBTA_AA_PROTECT_REQ;

/* data associated with BTA_AA_PROTECT_RSP_EVT */
typedef struct
{
    UINT8           *p_data;
    UINT16          len;
    tBTA_AA_ERR     err_code;
} tBTA_AA_PROTECT_RSP;

/* data associated with BTA_AA_RC_OPEN_EVT */
typedef struct
{
    tBTA_AA_FEAT    peer_features;
} tBTA_AA_RC_OPEN;

/* data associated with BTA_AA_REMOTE_CMD_EVT */
typedef struct
{
    tBTA_AA_RC      rc_id;
    tBTA_AA_STATE   key_state;
} tBTA_AA_REMOTE_CMD;

/* data associated with BTA_AA_REMOTE_RSP_EVT */
typedef struct
{
    tBTA_AA_RC      rc_id;
    tBTA_AA_STATE   key_state;
    tBTA_AA_CODE    rsp_code;
    UINT8           label;
} tBTA_AA_REMOTE_RSP;

/* data associated with BTA_AA_VENDOR_CMD_EVT, BTA_AA_VENDOR_RSP_EVT */
typedef struct
{
    UINT32          company_id;
    UINT8           *p_data;
    UINT8           len;
    tBTA_AA_CODE    code;
    UINT8           label;
} tBTA_AA_VENDOR;

/* data associated with BTA_AA_PENDING_EVT */
typedef struct
{
    BD_ADDR         bd_addr;
} tBTA_AA_PEND;


/* union of data associated with AA callback */
typedef union
{
    tBTA_AA_OPEN        open;
    tBTA_AA_START       start;
    tBTA_AA_PROTECT_REQ protect_req;
    tBTA_AA_PROTECT_RSP protect_rsp;
    tBTA_AA_RC_OPEN     rc_open;
    tBTA_AA_REMOTE_CMD  remote_cmd;
    tBTA_AA_REMOTE_RSP  remote_rsp;
    tBTA_AA_VENDOR      vendor_cmd;
    tBTA_AA_VENDOR      vendor_rsp;
    tBTA_AA_RECONFIG    reconfig;
    tBTA_AA_SUSPEND     suspend;
    tBTA_AA_PEND        pend;
} tBTA_AA;

/* AA callback */
typedef void (tBTA_AA_CBACK)(tBTA_AA_EVT event, tBTA_AA *p_data);

/* AA configuration structure */
typedef struct
{
    UINT32  company_id;         /* AVRCP Company ID */
    UINT16  avrc_mtu;           /* AVRCP MTU at L2CAP */
    UINT16  avrc_ct_cat;        /* AVRCP controller categories */
    UINT16  avrc_tg_cat;        /* AVRCP target categories */
    UINT16  sig_mtu;            /* AVDTP signaling channel MTU at L2CAP */
    UINT16  trans_mtu;          /* AVDTP transport channel MTU at L2CAP */
    UINT16  trans_flush_to;     /* AVDTP transport channel flush timeout */
} tBTA_AA_CFG;

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_AaEnable
**
** Description      Enable the advanced audio service. When the enable 
**                  operation is complete the callback function will be
**                  called with a BTA_AA_ENABLE_EVT. This function must
**                  be called before other function in the AA API are
**                  called.
**
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaEnable(tBTA_SEC sec_mask, const char *p_service_name,
                          tBTA_AA_FEAT features, tBTA_AA_CBACK *p_cback, UINT8 app_id);

/*******************************************************************************
**
** Function         BTA_AaDisable
**
** Description      Disable the advanced audio service.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaDisable(void);

/*******************************************************************************
**
** Function         BTA_AaOpen
**
** Description      Opens an advanced audio connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AA_OPEN_EVT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaOpen(BD_ADDR bd_addr, tBTA_SEC sec_mask);

/*******************************************************************************
**
** Function         BTA_AaClose
**
** Description      Close the current connection a peer device.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaClose(void);

/*******************************************************************************
**
** Function         BTA_AaStart
**
** Description      Start audio stream data transfer.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaStart(void);

/*******************************************************************************
**
** Function         BTA_AaStop
**
** Description      Stop audio stream data transfer.
**                  If suspend is TRUE, this function sends AVDT suspend signal
**                  to the connected peer.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaStop(BOOLEAN suspend);

/*******************************************************************************
**
** Function         BTA_AaReconfig
**
** Description      Reconfigure the audio stream.
**                  This function closes and re-opens the AVDT connection.
**                  If suspend is TRUE, this function tries the suspend/reconfigure
**                  procedure first.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaReconfig(BOOLEAN suspend, UINT8 sep_info_idx,
                            UINT8 *p_codec_info,
                            UINT8 num_protect, UINT8 *p_protect_info);

/*******************************************************************************
**
** Function         BTA_AaProtectReq
**
** Description      Send a content protection request.  This function can only
**                  be used if AA is enabled with feature BTA_AA_FEAT_PROTECT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaProtectReq(UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_AaProtectRsp
**
** Description      Send a content protection response.  This function must
**                  be called if a BTA_AA_PROTECT_REQ_EVT is received.  
**                  This function can only be used if AA is enabled with
**                  feature BTA_AA_FEAT_PROTECT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaProtectRsp(UINT8 error_code, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_AaRemoteCmd
**
** Description      Send a remote control command.  This function can only
**                  be used if AA is enabled with feature BTA_AA_FEAT_RCCT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaRemoteCmd(UINT8 label, tBTA_AA_RC rc_id, tBTA_AA_STATE key_state);

/*******************************************************************************
**
** Function         BTA_AaVendorCmd
**
** Description      Send a vendor dependent remote control command.  This
**                  function can only be used if AA is enabled with feature
**                  BTA_AA_FEAT_VENDOR.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaVendorCmd(UINT8 label, tBTA_AA_CODE cmd_code, UINT8 *p_data, UINT8 len);

/*******************************************************************************
**
** Function         BTA_AaVendorRsp
**
** Description      Send a vendor dependent remote control response.
**                  This function must be called if a BTA_AA_VENDOR_CMD_EVT  
**                  is received. This function can only be used if AA is
**                  enabled with feature BTA_AA_FEAT_VENDOR.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AaVendorRsp(UINT8 label, tBTA_AA_CODE rsp_code, UINT8 *p_data, UINT8 len);

#ifdef __cplusplus
}
#endif

#endif /* BTA_AA_API_H */

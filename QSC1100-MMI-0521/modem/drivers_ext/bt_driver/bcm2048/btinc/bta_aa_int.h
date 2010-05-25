/*****************************************************************************
**
**  Name:           bta_aa_int.h
**
**  Description:    This is the private interface file for the BTA advanced
**                  audio.
**
**  Copyright (c) 2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AA_INT_H
#define BTA_AA_INT_H

#include "bta_sys.h"
#include "bta_api.h"
#include "bta_aa_api.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

enum
{
    /* these events are handled by the state machine */
    BTA_AA_API_OPEN_EVT = BTA_SYS_EVT_START(BTA_ID_AA),
    BTA_AA_API_CLOSE_EVT,
    BTA_AA_API_DISABLE_EVT,
    BTA_AA_API_START_EVT,
    BTA_AA_API_STOP_EVT,
    BTA_AA_API_RECONFIG_EVT,
    BTA_AA_API_PROTECT_REQ_EVT,
    BTA_AA_API_PROTECT_RSP_EVT,
    BTA_AA_API_REMOTE_CMD_EVT,
    BTA_AA_API_VENDOR_CMD_EVT,
    BTA_AA_API_VENDOR_RSP_EVT,
    BTA_AA_CI_SRC_DATA_READY_EVT,
    BTA_AA_CI_SETCONFIG_OK_EVT,
    BTA_AA_CI_SETCONFIG_FAIL_EVT,
    BTA_AA_SDP_A2D_DISC_OK_EVT,  
    BTA_AA_SDP_A2D_DISC_FAIL_EVT,
    BTA_AA_SDP_AVRC_DISC_EVT,
    BTA_AA_STR_DISC_OK_EVT,  
    BTA_AA_STR_DISC_FAIL_EVT,
    BTA_AA_STR_GETCAP_OK_EVT,
    BTA_AA_STR_GETCAP_FAIL_EVT,
    BTA_AA_STR_OPEN_OK_EVT,  
    BTA_AA_STR_OPEN_FAIL_EVT,
    BTA_AA_STR_START_OK_EVT, 
    BTA_AA_STR_START_FAIL_EVT,
    BTA_AA_STR_CLOSE_EVT,    
    BTA_AA_STR_CONFIG_IND_EVT,
    BTA_AA_STR_SECURITY_IND_EVT,
    BTA_AA_STR_SECURITY_CFM_EVT,
    BTA_AA_STR_WRITE_CFM_EVT,
    BTA_AA_STR_SUSPEND_CFM_EVT,
    BTA_AA_STR_RECONFIG_CFM_EVT,
    BTA_AA_AVDT_CONNECT_EVT, 
    BTA_AA_AVDT_DISCONNECT_EVT,
    BTA_AA_AVRC_OPEN_EVT,
    BTA_AA_AVRC_CLOSE_EVT,
    BTA_AA_AVRC_TIMER_EVT,
    BTA_AA_AVRC_MSG_EVT,

    /* these events are handled outside of the state machine */
    BTA_AA_API_ENABLE_EVT
};

/* maximum number of SEPS in stream discovery results */
#define BTA_AA_NUM_SEPS         8

/* initialization value for AVRC handle */
#define BTA_AA_RC_HANDLE_NONE   0xFF

/*****************************************************************************
**  Data types
*****************************************************************************/

/* data type for BTA_AA_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;
    char                p_service_name[BTA_SERVICE_NAME_LEN+1];
    tBTA_AA_CBACK       *p_cback;
    tBTA_AA_FEAT        features;
    tBTA_SEC            sec_mask;
    UINT8               app_id;
} tBTA_AA_API_ENABLE;

/* data type for BTA_AA_API_OPEN_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    tBTA_SEC            sec_mask;
} tBTA_AA_API_OPEN;

/* data type for BTA_AA_API_STOP_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             suspend;
} tBTA_AA_API_STOP;

/* data type for BTA_AA_API_PROTECT_REQ_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
} tBTA_AA_API_PROTECT_REQ;

/* data type for BTA_AA_API_PROTECT_RSP_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
    UINT8               error_code;
} tBTA_AA_API_PROTECT_RSP;

/* data type for BTA_AA_API_REMOTE_CMD_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_PASS      msg;
    UINT8               label;
} tBTA_AA_API_REMOTE_CMD;

/* data type for BTA_AA_API_VENDOR_CMD_EVT and RSP */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_VENDOR    msg;
    UINT8               label;
} tBTA_AA_API_VENDOR;

/* data type for BTA_AA_API_RECONFIG_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             suspend;
    UINT8               sep_info_idx;
    UINT8               codec_info[AVDT_CODEC_SIZE];    /* codec configuration */
    UINT8               *p_protect_info;
    UINT8               num_protect;
} tBTA_AA_API_RCFG;


/* data type for BTA_AA_CI_SETCONFIG_OK_EVT and FAIL */
typedef struct
{
    BT_HDR              hdr;
    UINT8               err_code;
    UINT8               category;
    UINT8               num_seid;
    UINT8               *p_seid;
} tBTA_AA_CI_SETCONFIG;

/* data type for all stream events from AVDTP */
typedef struct {
    BT_HDR              hdr;
    tAVDT_CFG           cfg;        /* configuration/capabilities parameters */
    tAVDT_CTRL          msg;        /* AVDTP callback message parameters */
    BD_ADDR             bd_addr;    /* bd address */
    UINT8               handle;     /* AVDTP handle */
} tBTA_AA_STR_MSG;

/* data type for BTA_AA_RC_MSG_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG           msg;
    UINT8               handle;
    UINT8               label;
    UINT8               opcode;
} tBTA_AA_RC_MSG;

/* union of all event datatypes */
typedef union
{
    tBTA_AA_API_ENABLE      api_enable;
    tBTA_AA_API_OPEN        api_open;
    tBTA_AA_API_STOP        api_stop;
    tBTA_AA_API_PROTECT_REQ api_protect_req;
    tBTA_AA_API_PROTECT_RSP api_protect_rsp;
    tBTA_AA_API_REMOTE_CMD  api_remote_cmd;
    tBTA_AA_API_VENDOR      api_vendor;
    tBTA_AA_API_RCFG        api_reconfig;
    tBTA_AA_CI_SETCONFIG    ci_setconfig;
    tBTA_AA_STR_MSG         str_msg;
    tBTA_AA_RC_MSG          rc_msg;
} tBTA_AA_DATA;

/* type for SEP control block */
typedef struct
{
    UINT8               av_handle;    /* AVDTP handle */
    tBTA_AA_CODEC       codec_type;     /* codec type */
} tBTA_AA_SEP;

/* initiator/acceptor role for adaption */
#define BTA_AA_ROLE_AD_INT          0x00       /* initiator */
#define BTA_AA_ROLE_AD_ACP          0x01       /* acceptor */

/* initiator/acceptor signaling roles */
#define BTA_AA_ROLE_START_ACP       0x00
#define BTA_AA_ROLE_START_INT       0x10    /* do not change this value */

#define BTA_AA_ROLE_SUSPEND         0x20    /* suspending on start */
#define BTA_AA_ROLE_SUSPEND_OPT     0x40    /* Suspend on Start option is set */

/* type for AA control block */
typedef struct
{
    tAVDT_SEP_INFO      sep_info[BTA_AA_NUM_SEPS];      /* stream discovery results */
    UINT8               codec_info[AVDT_CODEC_SIZE];    /* codec configuration */
    BD_ADDR             peer_addr;      /* peer BD address */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AA_CBACK       *p_cback;       /* application callback function */
    tAVDT_CFG           *p_cap;         /* buffer used for get capabilities */
    tBTA_AA_SEP         seps[BTA_AA_MAX_SEPS];
    tAVDT_CFG           avdt_cfg;       /* the CFG for reconfigure, also include the current codec_info */
    UINT32              sdp_src_handle; /* SDP record handle for audio src */
    UINT32              sdp_ct_handle;  /* SDP record handle for rc controller */
    UINT32              sdp_tg_handle;  /* SDP record handle for rc target */
    TIMER_LIST_ENT      timer;
    tBTA_AA_FEAT        features;       /* features mask */
    tBTA_AA_FEAT        peer_features;  /* peer features mask */
    UINT16              stream_mtu;     /* MTU of stream */
    tBTA_AA_CODEC       codec_type;     /* codec type */
    tBTA_SEC            sec_mask;       /* security mask */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    BOOLEAN             rc_open;        /* TRUE if AVRCP open */
    BOOLEAN             sig_open;       /* TRUE if AVDTP signal channel is connected */
    BOOLEAN             disabling;      /* TRUE if api disabled called */
    BOOLEAN             started;        /* TRUE if stream started */
    BOOLEAN             co_started;     /* TRUE if stream started from call-out perspective */
    tBTA_AA_STATUS      open_status;    /* open failure status */
    UINT8               num_seps;       /* number of seps returned by stream discovery */
    UINT8               sep_info_idx;   /* current index into sep_info */
    UINT8               sep_idx;        /* current index into local seps[] */
    UINT8               rcfg_idx;       /* reconfig requested index into sep_info */
    UINT8               app_id;         /* application id */
    UINT8               state;          /* state machine state */
    UINT8               rc_handle;      /* connected AVRCP handle */
    UINT8               rc_int_handle;  /* initiating AVRCP handle */
    UINT8               rc_acp_handle;  /* accepting AVRCP handle */
    UINT8               avdt_label;     /* AVDTP label */
    UINT8               role;
    UINT8               num_recfg;      /* number of reconfigure sent */
    BOOLEAN             recfg_sup;      /* TRUE if the first attempt to reconfigure the stream was successfull, else False if command fails */
    BOOLEAN             suspend_sup;    /* TRUE if Suspend stream is supported, else FALSE if suspend command fails */

} tBTA_AA_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* control block declaration */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_AA_CB bta_aa_cb;
#else
extern tBTA_AA_CB *bta_aa_cb_ptr;
#define bta_aa_cb (*bta_aa_cb_ptr)
#endif

/* config struct */
extern tBTA_AA_CFG *p_bta_aa_cfg;

/* rc id config struct */
extern UINT16 *p_bta_aa_rc_id;

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

/* main functions */
extern void bta_aa_sm_execute(tBTA_AA_CB *p_cb, UINT16 event, tBTA_AA_DATA *p_data);
extern BOOLEAN bta_aa_hdl_event(BT_HDR *p_msg);
extern void bta_aa_stream_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);

/* action functions */
extern void bta_aa_init(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_set_disabling(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_do_a2d_disc(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_do_avrc_disc(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_avrc_disc_tmr(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_disc_failed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_free_db(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_setconfig_ind(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_setconfig_rsp(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_str_opened(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_open_failed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_str_closed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_do_close(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_connect_req(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_disconnect_req(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_discover_req(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_conn_failed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_disc_results(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_getcap_results(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_do_start(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_set_started(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_str_started(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_start_failed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_str_stopped(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_setconfig_rej(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_security_req(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_security_rsp(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_security_rej(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_security_ind(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_security_cfm(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_data_path(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_clr_cong(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_reconfig(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_open(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_connect(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_discntd(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_failed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_str_ok(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rcfg_cfm(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_suspend_cfm(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_suspend_cont(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_disc_done(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_close(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_listen(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_opened(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_closed(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_remote_cmd(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_vendor_cmd(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_vendor_rsp(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_rc_msg(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_save_caps(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_sig_opened(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);
extern void bta_aa_pend(tBTA_AA_CB *p_cb, tBTA_AA_DATA *p_data);


#endif /* BTA_AA_INT_H */

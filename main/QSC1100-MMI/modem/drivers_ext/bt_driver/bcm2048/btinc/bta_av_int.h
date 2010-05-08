/*****************************************************************************
**
**  Name:           bta_av_int.h
**
**  Description:    This is the private interface file for the BTA advanced
**                  audio/video.
**
**  Copyright (c) 2004-2007, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AV_INT_H
#define BTA_AV_INT_H

#include "bta_sys.h"
#include "bta_api.h"
#include "bta_av_api.h"
#include "avdt_api.h"
#include "bta_av_co.h"

/**********************/
/* Not for Lite Stack */
/**********************/
#if (BTU_STACK_LITE_ENABLED == FALSE)

/*****************************************************************************
**  Constants
*****************************************************************************/

enum
{
    /* these events are handled by the AV main state machine */
    BTA_AV_API_DISABLE_EVT = BTA_SYS_EVT_START(BTA_ID_AA),
    BTA_AV_API_REMOTE_CMD_EVT,
    BTA_AV_API_VENDOR_CMD_EVT,
    BTA_AV_API_VENDOR_RSP_EVT,
    BTA_AV_API_META_CMD_EVT,
    BTA_AV_API_META_RSP_EVT,
    BTA_AV_API_RC_CLOSE_EVT,
    BTA_AV_AVRC_OPEN_EVT,
    BTA_AV_AVRC_MSG_EVT,
    BTA_AV_AVRC_NONE_EVT,

    /* these events are handled by the AV stream state machine */
    BTA_AV_API_OPEN_EVT,
    BTA_AV_API_CLOSE_EVT,
    BTA_AV_AP_START_EVT,        /* the following 2 events must be in the same order as the *API_*EVT */
    BTA_AV_AP_STOP_EVT,
    BTA_AV_API_RECONFIG_EVT,
    BTA_AV_API_PROTECT_REQ_EVT,
    BTA_AV_API_PROTECT_RSP_EVT,
    BTA_AV_API_RC_OPEN_EVT,
    BTA_AV_SRC_DATA_READY_EVT,
    BTA_AV_CI_SETCONFIG_OK_EVT,
    BTA_AV_CI_SETCONFIG_FAIL_EVT,
    BTA_AV_SDP_DISC_OK_EVT,  
    BTA_AV_SDP_DISC_FAIL_EVT,
    BTA_AV_STR_DISC_OK_EVT,  
    BTA_AV_STR_DISC_FAIL_EVT,
    BTA_AV_STR_GETCAP_OK_EVT,
    BTA_AV_STR_GETCAP_FAIL_EVT,
    BTA_AV_STR_OPEN_OK_EVT,  
    BTA_AV_STR_OPEN_FAIL_EVT,
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    BTA_AV_STR_PROC_START_EVT,
#endif
    BTA_AV_STR_START_OK_EVT,
    BTA_AV_STR_START_FAIL_EVT,
    BTA_AV_STR_CLOSE_EVT,    
    BTA_AV_STR_CONFIG_IND_EVT,
    BTA_AV_STR_SECURITY_IND_EVT,
    BTA_AV_STR_SECURITY_CFM_EVT,
    BTA_AV_STR_WRITE_CFM_EVT,
    BTA_AV_STR_SUSPEND_CFM_EVT,
    BTA_AV_STR_RECONFIG_CFM_EVT,
    BTA_AV_AVRC_TIMER_EVT,
    BTA_AV_AVDT_CONNECT_EVT, 
    BTA_AV_AVDT_DISCONNECT_EVT,

    /* these events are handled outside of the state machine */
    BTA_AV_API_ENABLE_EVT,
    BTA_AV_API_REGISTER_EVT,
    BTA_AV_API_DEREGISTER_EVT,
    BTA_AV_API_DISCONNECT_EVT,
    BTA_AV_CI_SRC_DATA_READY_EVT,
    BTA_AV_SIG_CHG_EVT,
    BTA_AV_SIG_TIMER_EVT,
    BTA_AV_SDP_AVRC_DISC_EVT,
    BTA_AV_AVRC_CLOSE_EVT,
    BTA_AV_AVRC_OPEN_REQ_EVT,
    BTA_AV_CONN_CHG_EVT,
    BTA_AV_DEREG_COMP_EVT,
#if (AVDT_REPORTING == TRUE)
    BTA_AV_AVDT_RPT_CONN_EVT, 
#endif
    BTA_AV_API_START_EVT,       /* the following 2 events must be in the same order as the *AP_*EVT */
    BTA_AV_API_STOP_EVT
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    ,                           /* Needed for compiler warnings if FALSE */
    BTA_AV_SYNC_REQ_EVT         /* Sync request from DM */
#endif
};

/* events for AV control block state machine */
#define BTA_AV_FIRST_SM_EVT     BTA_AV_API_DISABLE_EVT
#define BTA_AV_LAST_SM_EVT      BTA_AV_AVRC_NONE_EVT

/* events for AV stream control block state machine */
#define BTA_AV_FIRST_SSM_EVT    BTA_AV_API_OPEN_EVT
#define BTA_AV_LAST_SSM_EVT     BTA_AV_AVDT_DISCONNECT_EVT

/* events that do not go through state machine */
#define BTA_AV_FIRST_NSM_EVT    BTA_AV_API_ENABLE_EVT

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
#define BTA_AV_LAST_NSM_EVT     BTA_AV_SYNC_REQ_EVT
#else
#define BTA_AV_LAST_NSM_EVT     BTA_AV_API_STOP_EVT
#endif

/* API events passed to both SSMs (by bta_av_api_to_ssm) */
#define BTA_AV_FIRST_A2S_API_EVT    BTA_AV_API_START_EVT
#define BTA_AV_FIRST_A2S_SSM_EVT    BTA_AV_AP_START_EVT

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
#define BTA_AV_LAST_EVT             BTA_AV_SYNC_REQ_EVT
#else
#define BTA_AV_LAST_EVT             BTA_AV_API_STOP_EVT
#endif

/* maximum number of SEPS in stream discovery results */
#define BTA_AV_NUM_SEPS         8

/* initialization value for AVRC handle */
#define BTA_AV_RC_HANDLE_NONE   0xFF

/* size of database for service discovery */
#define BTA_AV_DISC_BUF_SIZE        1000

/* offset of media type in codec info byte array */
#define BTA_AV_MEDIA_TYPE_IDX       1

/* maximum length of AVDTP security data */
#define BTA_AV_SECURITY_MAX_LEN     400

/* check number of buffers queued at L2CAP when this amount of buffers are queued to L2CAP */
#define BTA_AV_QUEUE_DATA_CHK_NUM   5

/* the number of ACL links with AVDT */
#define BTA_AV_NUM_LINKS            AVDT_NUM_LINKS

/* Metadata AVRC revision */
#define BTA_AV_RC_REV_1_3        0x103    

#define BTA_AV_CO_ID_TO_BE_STREAM(p, u32) {*(p)++ = (UINT8)((u32) >> 16); *(p)++ = (UINT8)((u32) >> 8); *(p)++ = (UINT8)(u32); }
#define BTA_AV_BE_STREAM_TO_CO_ID(u32, p) {u32 = (((UINT32)(*((p) + 2))) + (((UINT32)(*((p) + 1))) << 8) + (((UINT32)(*(p))) << 16)); (p) += 3;}

/*****************************************************************************
**  Data types
*****************************************************************************/

/* function types for call-out functions */
typedef BOOLEAN (*tBTA_AV_CO_INIT) (UINT8 *p_codec_type, UINT8 *p_codec_info,
                                   UINT8 *p_num_protect, UINT8 *p_protect_info, UINT8 index);

typedef void (*tBTA_AV_CO_DISC_RES) (tBTA_AV_HNDL hndl, UINT8 num_seps,
                                     UINT8 num_snk, BD_ADDR addr);

typedef UINT8 (*tBTA_AV_CO_GETCFG) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type,
                                     UINT8 *p_codec_info, UINT8 *p_sep_info_idx, UINT8 seid,
                                     UINT8 *p_num_protect, UINT8 *p_protect_info);
typedef void (*tBTA_AV_CO_SETCFG) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type,
                                    UINT8 *p_codec_info, UINT8 seid, BD_ADDR addr,
                                    UINT8 num_protect, UINT8 *p_protect_info);
typedef void (*tBTA_AV_CO_OPEN) (tBTA_AV_HNDL hndl, 
                                 tBTA_AV_CODEC codec_type, UINT8 *p_codec_info, 
                                   UINT16 mtu);
typedef void (*tBTA_AV_CO_CLOSE) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type, UINT16 mtu);
typedef void (*tBTA_AV_CO_START) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type);
typedef void (*tBTA_AV_CO_STOP) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type);
typedef void * (*tBTA_AV_CO_DATAPATH) (tBTA_AV_CODEC codec_type,
                                       UINT32 *p_len, UINT32 *p_timestamp);

/* the call-out functions for one stream */
typedef struct
{
    tBTA_AV_CO_INIT     init;
    tBTA_AV_CO_DISC_RES disc_res;
    tBTA_AV_CO_GETCFG   getcfg;
    tBTA_AV_CO_SETCFG   setcfg;
    tBTA_AV_CO_OPEN     open;
    tBTA_AV_CO_CLOSE    close;
    tBTA_AV_CO_START    start;
    tBTA_AV_CO_STOP     stop;
    tBTA_AV_CO_DATAPATH data;
} tBTA_AV_CO_FUNCTS;

/* data type for BTA_AV_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_AV_CBACK       *p_cback;
    tBTA_AV_FEAT        features;
    tBTA_SEC            sec_mask;
} tBTA_AV_API_ENABLE;

/* data type for BTA_AV_API_REG_EVT */
typedef struct
{
    BT_HDR              hdr;
    char                p_service_name[BTA_SERVICE_NAME_LEN+1];
    UINT8               app_id;
} tBTA_AV_API_REG;

/* data type for BTA_AV_API_OPEN_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    BOOLEAN             use_rc;
    tBTA_SEC            sec_mask;
} tBTA_AV_API_OPEN;

/* data type for BTA_AV_API_STOP_EVT */
typedef struct
{
    BT_HDR              hdr;
    BOOLEAN             suspend;
    BOOLEAN             flush;
} tBTA_AV_API_STOP;

/* data type for BTA_AV_API_DISCONNECT_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
} tBTA_AV_API_DISCNT;

/* data type for BTA_AV_API_PROTECT_REQ_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
} tBTA_AV_API_PROTECT_REQ;

/* data type for BTA_AV_API_PROTECT_RSP_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
    UINT8               error_code;
} tBTA_AV_API_PROTECT_RSP;

/* data type for BTA_AV_API_REMOTE_CMD_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_PASS      msg;
    UINT8               label;
} tBTA_AV_API_REMOTE_CMD;

/* data type for BTA_AV_API_VENDOR_CMD_EVT and RSP */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG_VENDOR    msg;
    UINT8               label;
} tBTA_AV_API_VENDOR;

/* data type for BTA_AV_API_RC_OPEN_EVT */
typedef struct
{
    BT_HDR              hdr;
} tBTA_AV_API_OPEN_RC;

/* data type for BTA_AV_API_RC_CLOSE_EVT */
typedef struct
{
    BT_HDR              hdr;
} tBTA_AV_API_CLOSE_RC;

typedef tBTA_AV_API_VENDOR      tBTA_AV_API_METADATA;

/* data type for BTA_AV_API_RECONFIG_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               codec_info[AVDT_CODEC_SIZE];    /* codec configuration */
    UINT8               *p_protect_info;
    UINT8               num_protect;
    BOOLEAN             suspend;
    UINT8               sep_info_idx;
} tBTA_AV_API_RCFG;

/* data type for BTA_AV_CI_SETCONFIG_OK_EVT and BTA_AV_CI_SETCONFIG_FAIL_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_AV_HNDL        hndl;
    UINT8               err_code;
    UINT8               category;
    UINT8               num_seid;
    UINT8               *p_seid;
    BOOLEAN             recfg_needed;
} tBTA_AV_CI_SETCONFIG;

/* data type for all stream events from AVDTP */
typedef struct {
    BT_HDR              hdr;
    tAVDT_CFG           cfg;        /* configuration/capabilities parameters */
    tAVDT_CTRL          msg;        /* AVDTP callback message parameters */
    BD_ADDR             bd_addr;    /* bd address */
    UINT8               handle;
} tBTA_AV_STR_MSG;

/* data type for BTA_AV_AVRC_MSG_EVT */
typedef struct
{
    BT_HDR              hdr;
    tAVRC_MSG           msg;
    UINT8               handle;
    UINT8               label;
    UINT8               opcode;
} tBTA_AV_RC_MSG;

/* data type for BTA_AV_CONN_CHG_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    UINT8               handle;
} tBTA_AV_RC_CONN_CHG;

/* data type for BTA_AV_CONN_CHG_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    BOOLEAN             is_up;
} tBTA_AV_CONN_CHG;

/* type for SEP control block */
typedef struct
{
    UINT8               av_handle;      /* AVDTP handle */
    tBTA_AV_CODEC       codec_type;     /* codec type */
} tBTA_AV_SEP;


#if (BTU_DUAL_STACK_INCLUDED == TRUE)
/* data type for BTA_AV_SYNC_REQ_EVT */
typedef struct
{
    BT_HDR              hdr;
} tBTA_AV_SYNC_REQ;
#endif /* BTU_DUAL_STACK_INCLUDED */

/* initiator/acceptor role for adaption */
#define BTA_AV_ROLE_AD_INT          0x00       /* initiator */
#define BTA_AV_ROLE_AD_ACP          0x01       /* acceptor */

/* initiator/acceptor signaling roles */
#define BTA_AV_ROLE_START_ACP       0x00
#define BTA_AV_ROLE_START_INT       0x10    /* do not change this value */

#define BTA_AV_ROLE_SUSPEND         0x20    /* suspending on start */
#define BTA_AV_ROLE_SUSPEND_OPT     0x40    /* Suspend on Start option is set */

/* union of all event datatypes */
typedef union
{
    BT_HDR                  hdr;
    tBTA_AV_API_ENABLE      api_enable;
    tBTA_AV_API_REG         api_reg;
    tBTA_AV_API_OPEN        api_open;
    tBTA_AV_API_STOP        api_stop;
    tBTA_AV_API_DISCNT      api_discnt;
    tBTA_AV_API_PROTECT_REQ api_protect_req;
    tBTA_AV_API_PROTECT_RSP api_protect_rsp;
    tBTA_AV_API_REMOTE_CMD  api_remote_cmd;
    tBTA_AV_API_VENDOR      api_vendor;
    tBTA_AV_API_RCFG        api_reconfig;
    tBTA_AV_CI_SETCONFIG    ci_setconfig;
    tBTA_AV_STR_MSG         str_msg;
    tBTA_AV_RC_MSG          rc_msg;
    tBTA_AV_RC_CONN_CHG     rc_conn_chg;
    tBTA_AV_CONN_CHG        conn_chg;
    tBTA_AV_API_METADATA    api_meta_cmd;
    tBTA_AV_API_METADATA    api_meta_rsp;
} tBTA_AV_DATA;

typedef void (tBTA_AV_VDP_DATA_ACT)(void *p_scb);

typedef struct
{
    tBTA_AV_VDP_DATA_ACT    *p_act;
    UINT8                   *p_frame;
    UINT16                  buf_size;
    UINT32                  len;
    UINT32                  offset;
    UINT32                  timestamp;
} tBTA_AV_VF_INFO;

typedef union
{
    BUFFER_Q        a2d;            /* used for audio channels only */
    tBTA_AV_VF_INFO vdp;
} tBTA_AV_Q_INFO;


/* type for AV stream control block */
typedef struct
{
    const tBTA_AV_ACT   *p_act_tbl;     /* the action table for stream state machine */
    const tBTA_AV_CO_FUNCTS *p_cos;     /* the associated callout functions */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AV_SEP         seps[BTA_AV_MAX_SEPS];
    tAVDT_CFG           *p_cap;         /* buffer used for get capabilities */
    tBTA_AV_Q_INFO      q_info;
    tAVDT_SEP_INFO      sep_info[BTA_AV_NUM_SEPS];      /* stream discovery results */
    tAVDT_CFG           cfg;            /* local SEP configuration */
    TIMER_LIST_ENT      timer;          /* delay timer for AVRC CT */
    BD_ADDR             peer_addr;      /* peer BD address */
    UINT16              l2c_cid;        /* L2CAP channel ID */
    UINT16              stream_mtu;     /* MTU of stream */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AV_CODEC       codec_type;     /* codec type */
    UINT8               media_type;     /* Media type */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    tBTA_AV_STATUS      open_status;    /* open failure status */
    tBTA_AV_CHNL        chnl;           /* the channel: audio/video */
    tBTA_AV_HNDL        hndl;           /* the handle: ((hdi + 1)|chnl) */
    UINT8               cur_psc_mask;   /* Protocol service capabilities mask for current connection */
    UINT8               avdt_handle;    /* AVDTP handle */
    UINT8               hdi;            /* the index to SCB[] */
    UINT8               num_seps;       /* number of seps returned by stream discovery */
    UINT8               sep_info_idx;   /* current index into sep_info */
    UINT8               sep_idx;        /* current index into local seps[] */
    UINT8               rcfg_idx;       /* reconfig requested index into sep_info */
    UINT8               state;          /* state machine state */
    UINT8               avdt_label;     /* AVDTP label */
    UINT8               app_id;         /* application id */
    UINT8               num_recfg;      /* number of reconfigure sent */
    UINT8               role;
    UINT8               l2c_bufs;       /* the number of buffers queued to L2CAP */
    UINT8               rc_handle;      /* connected AVRCP handle */
    BOOLEAN             use_rc;         /* TRUE if AVRCP is allowed */
    BOOLEAN             started;        /* TRUE if stream started */
    BOOLEAN             co_started;     /* TRUE if stream started from call-out perspective */
    BOOLEAN             recfg_sup;      /* TRUE if the first attempt to reconfigure the stream was successfull, else False if command fails */
    BOOLEAN             suspend_sup;    /* TRUE if Suspend stream is supported, else FALSE if suspend command fails */
    BOOLEAN             deregistring;   /* TRUE if deregistering */
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    BOOLEAN             closing;        /* TRUE if closing this stream */
    BOOLEAN             closing_rcfg;   /* TRUE if closing to reconfigure */
    BOOLEAN             suspend;        /* TRUE if stream to be suspended */
#endif
} tBTA_AV_SCB;

#define BTA_AV_RC_ROLE_MASK     0x10
#define BTA_AV_RC_ROLE_INT      0x00
#define BTA_AV_RC_ROLE_ACP      0x10

#define BTA_AV_RC_CONN_MASK     0x20

/* type for AV RCP control block */
/* index to this control block is the rc handle */
typedef struct
{
    UINT8   status;
    UINT8   handle;
    UINT8   shdl;   /* stream handle (hdi + 1) */
    UINT8   lidx;   /* (index+1) to LCB */
} tBTA_AV_RCB;
#define BTA_AV_NUM_RCB      (BTA_AV_NUM_STRS  + 2)

enum
{
    BTA_AV_LCB_FREE,
    BTA_AV_LCB_FIND
};

/* type for AV ACL Link control block */
typedef struct
{
    BD_ADDR             addr;           /* peer BD address */
    UINT8               conn_msk;       /* handle mask of connected stream handle */
    UINT8               lidx;           /* index + 1 */
} tBTA_AV_LCB;

/* type for Metadata fragmentation control block */
typedef struct 
{
    BOOLEAN             frag_enabled;   /* fragmentation flag */
    UINT16              total_size;     /* total size of vendor_depent_data field */
    UINT16              remaining_size; /* size of non-transmitted vendor_depenent_data field*/
    UINT8               packet_type;    /* packet type */
    tAVRC_META_MSGS     prev_rx_msg;    /* previous received Metadata message */
} tBTA_AV_META_FRAG;

/* type for stream state machine action functions */
typedef void (*tBTA_AV_SACT)(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);

#endif /* BTU_STACK_LITE_ENABLED = FALSE */
/****************************/
/* End...Not for Lite Stack */
/****************************/


/***********************/
/**  FOR BOTH STACKs  **/
/***********************/
#if ((BTU_DUAL_STACK_INCLUDED == TRUE) || (BTU_STACK_LITE_ENABLED == TRUE))
/* op_code */
#define BTA_AV_SYNC_TO_LITE_REQ             0
#define BTA_AV_SYNC_TO_LITE_RESP            1
#define BTA_AV_STR_START_TO_LITE_REQ        2
#define BTA_AV_STR_START_TO_LITE_RESP       3
#define BTA_AV_STR_STOP_TO_LITE_REQ         4
#define BTA_AV_STR_STOP_TO_LITE_RESP        5
#define BTA_AV_STR_CLEANUP_TO_LITE_REQ      6
#define BTA_AV_STR_CLEANUP_TO_LITE_RESP     7
#define BTA_AV_STR_SUSPEND_TO_LITE_REQ      8
#define BTA_AV_STR_SUSPEND_TO_LITE_RESP     9
#define BTA_AV_SYNC_ERROR_RESP              10

typedef UINT8 tBTA_AV_DUAL_STACK_EVT;

typedef struct
{
    UINT8               avdt_handle;    /* AVDTP handle */
    tBTA_AV_CHNL        chnl;           /* the channel: audio/video */
    tBTA_AV_CODEC       codec_type;     /* codec type */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    UINT8               hdi;            /* the index to SCB[] */
    tBTA_AV_HNDL        hndl;           /* the handle: ((hdi + 1)|chnl) */
    UINT8               l2c_bufs;       /* the number of buffers queued to L2CAP */
    UINT16              l2c_cid;        /* L2CAP channel ID */
    BD_ADDR             peer_addr;      /* peer BD address */
}tBTA_AV_SYNC_INFO;

typedef struct
{
    tBTA_AV_DUAL_STACK_EVT  event;
    tBTA_AV_SYNC_INFO       sync_info;
}tBTA_AV_SYNC_INFO_REQ; /* SYNC_TO_LITE_REQ */

/* Dual stack stream events */
typedef struct
{
    tBTA_AV_DUAL_STACK_EVT          event;
    UINT8                           scb_idx;
}tBTA_AV_SCB_EVT;

/* Dual stack stream events */
typedef struct
{
    tBTA_AV_DUAL_STACK_EVT          event;
    UINT8                           scb_idx;
    UINT8                           audio_bits;
    tBTA_AV_AUDIO_CODEC_INFO        p_codec_cfg;
}tBTA_AV_SCB_REQ
;
/* Add request/response structures if needed ...
typedef struct
{
    event;
    data;
}tBTA_AV_SYNC_*_REQ/RESP;
*/

typedef union
{
    tBTA_AV_DUAL_STACK_EVT          event;
    tBTA_AV_SYNC_INFO_REQ           sync_info_req;
    tBTA_AV_SCB_EVT                 scb_evt;
    tBTA_AV_SCB_REQ                 scb_req;
}tBTA_AV_DUAL_STACK_DATA;

extern void bta_av_ipc_evt_hdlr(tBTM_STATUS status, BT_HDR *p_buf);
extern void bta_av_ipc_send(tBTA_AV_DUAL_STACK_DATA *dual_stack_data);

#endif /* BTU_DUAL_STACK_INCLUDED || BTU_STACK_LITE_ENABLED */

/***********************/
/**  FOR Dual STACK   **/
/***********************/
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
extern void bta_av_bb2mm_sync(tBTA_AV_SCB *p_scb, tBTA_AV_EVT sync_reason);
extern void bta_av_sync_request(tBTA_AV_SCB *p_scb);
extern void bta_av_send_sync_result(tBTM_STATUS status, tBTM_SYNC_INFO *sync_info);
extern BOOLEAN bta_av_notify_hdlr(void);
extern void bta_av_sync_req_hdlr(void);
#endif /* BTU_DUAL_STACK_INCLUDED */

/***********************/
/**  FOR Lite STACK   **/
/***********************/
#if (BTU_STACK_LITE_ENABLED == TRUE)
#define BD_ADDR_LEN 6

typedef void (*tBTA_AV_CO_START) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type);
typedef void (*tBTA_AV_CO_STOP) (tBTA_AV_HNDL hndl, tBTA_AV_CODEC codec_type);
typedef void * (*tBTA_AV_CO_DATAPATH) (tBTA_AV_CODEC codec_type,
                                       UINT32 *p_len, UINT32 *p_timestamp);
typedef void (*tBTA_AV_CO_SETCODEC) (tBTA_AV_HNDL hndl, tBTA_AV_AUDIO_CODEC_INFO codec_cfg);


/* Define SYS events for BTA AV Lite */
enum
{
    BTA_AV_LITE_SRC_DATA_READY
};

/* data type for BTA_AV_LITE_SRC_DATA_READY_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               scb_idx;
} tBTA_AV_LITE_SRC_DATA_READY;

/* data type for AVDTP_WRITE_CFM_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT8               scb_idx;
} tBTA_AV_LITE_WRITE_CFM_EVT;

/* union of all event datatypes */
typedef union
{
    BT_HDR                  hdr;
    tBTA_AV_LITE_SRC_DATA_READY  src_data_ready;
    tBTA_AV_LITE_WRITE_CFM_EVT   write_cfm_evt;
}tBTA_AV_LITE_DATA;

/* type for AV stream control block on Lite stack*/
typedef struct
{
    BUFFER_Q            out_q;          /* used for audio channels only */
    BD_ADDR             peer_addr;      /* peer BD address */
    UINT16              l2c_cid;        /* L2CAP channel ID */
    tBTA_AV_CODEC       codec_type;     /* codec type */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    tBTA_AV_CHNL        chnl;           /* the channel: audio/video */
    tBTA_AV_HNDL        hndl;           /* the handle: ((hdi + 1)|chnl) */
    UINT8               avdt_handle;    /* AVDTP handle */
    UINT8               hdi;            /* the index to SCB[] */
    UINT8               l2c_bufs;       /* the number of buffers queued to L2CAP */
    BOOLEAN             co_started;     /* TRUE if stream started from call-out perspective */
 
} tBTA_AV_LITE_SCB;

extern void bta_av_lite_init(void);
extern void bta_av_lite_start_ok(UINT8 scb_idx, tBTA_AV_AUDIO_CODEC_INFO *p_codec_cfg);
extern void bta_av_lite_str_stop(UINT8 scb_idx);
extern void bta_av_lite_str_cleanup(UINT8 scb_idx);
extern void bta_av_lite_str_suspend (UINT8 scb_idx);
extern void bta_av_lite_stream0_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event,
                                     tAVDT_CTRL *p_data);
extern void bta_av_lite_stream1_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event,
                                     tAVDT_CTRL *p_data);
void bta_av_lite_clear_cong (UINT8 scb_idx);
void bta_av_lite_data_path (UINT8 scb_idx);
#endif /* BTU_STACK_LITE_ENABLED */

/* type for AV control block */
typedef struct
{
#if (BTU_STACK_LITE_ENABLED == FALSE)
    tBTA_AV_SCB         *p_scb[BTA_AV_NUM_STRS];    /* stream control block */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AV_CBACK       *p_cback;       /* application callback function */
    tBTA_AV_RCB         rcb[BTA_AV_NUM_RCB];  /* RCP control block */
    tBTA_AV_LCB         lcb[BTA_AV_NUM_LINKS+1];  /* link control block */
    tBTA_AV_META_FRAG   meta_fragment[BTA_AV_NUM_RCB];  /* Metadata fragmentation control block */
    TIMER_LIST_ENT      sig_tmr;        /* link timer */
    UINT32              sdp_a2d_handle; /* SDP record handle for audio src */
    UINT32              sdp_vdp_handle; /* SDP record handle for video src */
    UINT32              sdp_ct_handle;  /* SDP record handle for rc controller */
    UINT32              sdp_tg_handle;  /* SDP record handle for rc target */
    tBTA_AV_FEAT        features;       /* features mask */
    tBTA_AV_FEAT        peer_features;  /* peer features mask */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AV_HNDL        handle;         /* the handle for SDP activity */
    BOOLEAN             disabling;      /* TRUE if api disabled called */
    UINT8               disc;           /* (hdi+1) if p_disc_db is in use */
    UINT8               state;          /* state machine state */
    UINT8               conn_rc;        /* handle mask of connected RCP channels */
    UINT8               conn_audio;     /* handle mask of connected audio channels */
    UINT8               conn_video;     /* handle mask of connected video channels */
    UINT8               conn_lcb;       /* index mask of used LCBs */
    UINT8               audio_bits;     /* bits set of connected audio channels */
    UINT8               reg_audio;      /* handle mask of registered audio channels */
    UINT8               reg_video;      /* handle mask of registered video channels */
    UINT8               rc_acp_handle;
    UINT8               rc_acp_idx;     /* (index + 1) to RCB */
#endif

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    BOOLEAN             rcfg_cmpl;
    UINT8               ds_curr_hdi; /* current stream index */
    BOOLEAN             ds_sync_idx[BTA_AV_NUM_STRS]; /* hdi of all p_scb's that need syncing */
    tBTA_AV_EVT         ds_sync_reason;    /* syncronization reason between dual stacks */
#endif /*(BTU_DUAL_STACK_INCLUDED == TRUE)*/

#if (BTU_STACK_LITE_ENABLED == TRUE)
    UINT8               audio_bits;     /* bits set of connected audio channels */
    tBTA_AV_LITE_SCB    lite_scb[BTA_AV_NUM_STRS]; /* stream control block for the lite stack */
#endif /*(BTU_STACK_LITE_ENABLED == TRUE)*/

} tBTA_AV_CB;



/*****************************************************************************
**  Global data
*****************************************************************************/

/* control block declaration */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_AV_CB bta_av_cb;
#else
extern tBTA_AV_CB *bta_av_cb_ptr;
#define bta_av_cb (*bta_av_cb_ptr)
#endif

/**********************/
/* Not for Lite Stack */
/**********************/
#if (BTU_STACK_LITE_ENABLED == FALSE)

/* config struct */
extern tBTA_AV_CFG *p_bta_av_cfg;

/* rc id config struct */
extern UINT16 *p_bta_av_rc_id;

extern const tBTA_AV_SACT bta_av_a2d_action[];
extern const tBTA_AV_CO_FUNCTS bta_av_a2d_cos;
extern const tBTA_AV_SACT bta_av_vdp_action[];
extern tAVDT_CTRL_CBACK * bta_av_dt_cback[];

/*****************************************************************************
**  Function prototypes
*****************************************************************************/
/* utility functions */
extern void bta_av_proc_sig_chg(BD_ADDR addr, UINT16 event);
extern BOOLEAN bta_av_chk_start(tBTA_AV_SCB *p_scb);
extern UINT16 bta_av_chk_mtu(tBTA_AV_SCB *p_scb);
extern void bta_av_conn_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
extern void bta_av_rc_create(tBTA_AV_CB *p_cb, UINT8 role, UINT8 shdl, UINT8 lidx);

/* main functions */
extern void bta_av_api_deregister(tBTA_AV_DATA *p_data);
extern void bta_av_dup_audio_buf(tBTA_AV_SCB *p_scb, BT_HDR *p_buf, UINT32 timestamp);
extern void bta_av_sm_execute(tBTA_AV_CB *p_cb, UINT16 event, tBTA_AV_DATA *p_data);
extern void bta_av_ssm_execute(tBTA_AV_SCB *p_scb, UINT16 event, tBTA_AV_DATA *p_data);
extern BOOLEAN bta_av_hdl_event(BT_HDR *p_msg);
#if (defined(BTA_AV_DEBUG) && BTA_AV_DEBUG == TRUE)
extern char *bta_av_evt_code(UINT16 evt_code);
#endif

/* nsm action functions */
extern void bta_av_api_disconnect(tBTA_AV_DATA *p_data);
extern void bta_av_sig_chg(tBTA_AV_DATA *p_data);
extern void bta_av_sig_timer(tBTA_AV_DATA *p_data);
extern void bta_av_rc_disc_done(tBTA_AV_DATA *p_data);
extern void bta_av_rc_closed(tBTA_AV_DATA *p_data);
extern void bta_av_rc_disc(tBTA_AV_DATA *p_data);
extern void bta_av_conn_chg(tBTA_AV_DATA *p_data);
extern void bta_av_dereg_comp(tBTA_AV_DATA *p_data);
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
extern void bta_av_sync_proc(tBTA_AV_DATA *p_data);
#endif /* BTU_DUAL_STACK_INCLUDED */

/* sm action functions */
extern void bta_av_disable (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_opened (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_remote_cmd (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_vendor_cmd (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_vendor_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_msg (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_close (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_meta_cmd (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_meta_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);

/* ssm action functions */
extern void bta_av_do_disc_a2d (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_cleanup (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_free_sdb (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_config_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_disconnect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_setconfig_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_st_rc_timer(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_opened (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_do_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_connect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_sdp_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_disc_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_getcap_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_setconfig_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_discover_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_conn_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_do_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_stopped (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_reconfig (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_data_path (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
extern void bta_av_proc_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
#endif
extern void bta_av_start_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_start_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_closed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_clr_cong (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_suspend_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_str_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_connect (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_discntd (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_suspend_cont (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_open (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_chk_2nd_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_save_caps (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rej_conn (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rej_conn (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_set_use_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_cco_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);

/* ssm action functions - vdp specific */
extern void bta_av_do_disc_vdp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_vdp_str_opened (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_reg_vdp (tAVDT_CS *p_cs, char *p_service_name, void *p_data);

/* Metadata specific functions */
extern UINT16 bta_av_check_meta_fragmentation(UINT8 rc_handle, tBTA_AV_META_MSGS *p_params, UINT8 *p_data, UINT16 len);

#endif /* BTU_STACK_LITE_ENABLED = FALSE */
/****************************/
/* End...Not for Lite Stack */
/****************************/

#endif /* BTA_AV_INT_H */

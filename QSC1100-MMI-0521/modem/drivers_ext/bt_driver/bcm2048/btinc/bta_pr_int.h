/*****************************************************************************
**
**  Name:           bta_pr_int.h
**
**  Description:    This is the private header file for the Printer
**                  Client (PR).
**
**  Copyright (c) 2004-2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_PR_INT_H
#define BTA_PR_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "obx_api.h"
#include "bta_pr_api.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"
#include "bpp_api.h"
#include "bta_bp_int.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

#define BTA_PR_MAX_AUTH_KEY_SIZE        16  /* Must not be greater than OBX_MAX_AUTH_KEY_SIZE */

/* For converting from image format to document format mask */
#define BTA_PR_IMG_FMT_MASK(img_format) ((tBTA_PR_FMT_MASK)(1 << (BTA_PR_IMAGE_FMT + img_format)))

/* Maximum number of services used by BTA_PR */
#define BTA_PR_SERVICES_MAX             4

/* state machine events */
enum
{
    /* these events are handled by the state machine */
    BTA_PR_API_DISABLE_EVT = BTA_SYS_EVT_START(BTA_ID_PR),
    BTA_PR_API_CLOSE_EVT,           /* Close connection event */
    BTA_PR_API_GETCAPS_EVT,         /* GetCaps api event */
    BTA_PR_API_PRINT_EVT,           /* Print Object request */
    BTA_PR_API_AUTHRSP_EVT,         /* AuthRsp api event */
    BTA_PR_SDP_OK_EVT,              /* Service search was successful */
    BTA_PR_SDP_NEXT_EVT,
    BTA_PR_SDP_FAIL_EVT,            /* Service search failed */
    BTA_PR_CI_READ_EVT,             /* Call-in response to Read request */
    BTA_PR_CI_OPEN_EVT,             /* Call-in response to File Open request */
    BTA_PR_OBX_CONN_RSP_EVT,        /* OBX Channel Connect Request */
    BTA_PR_OBX_ABORT_RSP_EVT,       /* OBX_operation aborted */
    BTA_PR_OBX_TOUT_EVT,            /* OBX Operation Timeout */
    BTA_PR_OBX_PASSWORD_EVT,        /* OBX password requested */
    BTA_PR_OBX_CLOSE_EVT,           /* OBX Channel Disconnected (Link Lost) */
    BTA_PR_OBX_PUT_RSP_EVT,         /* Write file data or delete */
    BTA_PR_OBX_CMPL_EVT,            /* operation has completed */
    BTA_PR_CLOSE_CMPL_EVT,          /* Finish the closing of the channel */
    BTA_PR_DISABLE_CMPL_EVT,        /* Finished disabling system */
    BTA_PR_BP_ACTIVE_EVT,           /* BP is open and Active */
    BTA_PR_BP_INACTIVE_EVT,         /* BP is inactive */

    /* these events are handled outside the state machine */
    BTA_PR_API_ENABLE_EVT,

    /* BP Events */
    BTA_BP_CANCEL_STATUS_EVT,
    BTA_BP_CI_READ_EVT,             /* Call-in response to Read request */
    BTA_BP_CI_OPEN_EVT,             /* Call-in response to File Open request */
    BTA_BP_REF_OBJ_RSP_EVT,         /* Reference Object Response (API) */
    BTA_BR_JOB_ATTR_TOUT_EVT,       /* GetJobAttributes timer expire */
    BTA_BP_MAX_EVT
};

typedef UINT16 tBTA_PR_INT_EVT;

typedef UINT8 tBTA_PR_INT_STATE;


/* Image type enumeration */
enum
{
    BTA_PR_IMG_FMT_JPEG,
    BTA_PR_IMG_FMT_GIF,
    BTA_PR_IMG_FMT_UNKNOWN
};
typedef UINT8 tBTA_PR_IMG_FMT;
#define BTA_PR_IMG_FMT_MAX      BTA_PR_IMG_FMT_UNKNOWN


/* Document format version strings */
#define BTA_PR_FMT_VERS_MAXLEN  6
typedef UINT8 tBTA_PR_FMT_VERS[BTA_PR_FMT_VERS_MAXLEN+1];

#define BTA_PR_GIF_VERS_HEADER_LEN  6   /* Length GIF version string in header */

/* data type for BTA_PR_SDP_OK_EVT */
typedef struct
{
    tBPP_UINT128     char_rep;          /* Char repertoires supported. See BPP spec. section 12.2.3 */
    UINT8            sts_scn;           /* SCN of the status channel */
    BOOLEAN          color_supported;   /* Is color supported */
    BOOLEAN          enhanced_layout;   /* TRUE if printer supports enhanced layout */
    tBTA_1284_INFO ID1284_info;
} tBTA_BP_SDP_PARAMS;


/* Enumeration of op for operation */
enum
{
    BTA_PR_OP_IDLE,
    BTA_PR_OP_GETCAPS,
    BTA_PR_OP_PRINT
};
typedef UINT8 tBTA_PR_OP;

/* data type for BTA_PR_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_PR_CBACK      *p_cback;
    UINT8               sec_mask;
    UINT8               app_id;
} tBTA_PR_API_ENABLE;

/* data type for BTA_PR_API_GETCAPS_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    tBTA_SERVICE_MASK   services;   /* Service(s) to use for printing */
} tBTA_PR_API_GETCAPS;

/* data type for BTA_PR_API_PRINT_EVT */
typedef struct
{
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    char               *p_name;
    tBTA_PR_FMT         format;
    tBTA_SERVICE_MASK   services;   /* Service(s) to use for printing */
    tBTA_MIME_HDR       mime_type_hdr;
    tBTA_PR_BI_DESC    *p_bi_desc;  /* Additional parameters */
    tBTA_PR_BP_PARAMS  *p_bp_parms;
} tBTA_PR_API_PRINT;

/* data type for BTA_PR_API_AUTHRSP_EVT */
typedef struct
{
    BT_HDR  hdr;
    UINT8   key [BTA_PR_MAX_AUTH_KEY_SIZE];      /* The authentication key.*/
    UINT8   key_len;
    UINT8   userid [OBX_MAX_REALM_LEN];      /* The authentication user id.*/
    UINT8   userid_len;
} tBTA_PR_API_AUTHRSP;

/* data type for BTA_PR_SDP_OK_EVT */
typedef struct
{
    BT_HDR              hdr;
    UINT16              sdp_status;
    UINT8               scn;
    tBTA_PR_FMT_MASK    doc_formats;
    tBIP_FEATURE_FLAGS  bip_features;
    tBTA_BP_SDP_PARAMS  bpp_params;
} tBTA_PR_SDP_OK_EVT;

/* data type for all obex events
    hdr.event contains the PR event
*/
typedef struct
{
    BT_HDR              hdr;
    tOBX_HANDLE         handle;
    tOBX_EVT_PARAM      param;
    BT_HDR             *p_pkt;
    tOBX_EVENT          obx_event;
    UINT8               rsp_code;
} tBTA_PR_OBX_EVT;

/* union of all event data types */
typedef union
{
    BT_HDR                  hdr;
    tBTA_PR_API_ENABLE      api_enable;
    tBTA_PR_API_GETCAPS     api_getcaps;
    tBTA_PR_API_PRINT       api_print;
    tBTA_PR_API_REFOBJ_RSP  api_refobj_rsp;
    tBTA_PR_API_AUTHRSP     auth_rsp;
    tBTA_PR_SDP_OK_EVT      sdp_ok;
    tBTA_PR_OBX_EVT         obx_evt;
    tBTA_FS_CI_OPEN_EVT     open_evt;
    tBTA_FS_CI_READ_EVT     read_evt;
    tBTA_FS_CI_WRITE_EVT    write_evt;
} tBTA_PR_DATA;


/* OBX Response Packet Structure - Holds current command/response packet info */
typedef struct
{
    BT_HDR  *p_pkt;             /* (Get/Put) Holds the current OBX header for Put or Get */
    UINT8   *p_start;           /* (Get/Put) Start of the Body of the packet */
    UINT16   offset;            /* (Get/Put) Contains the current offset into the Body (p_start) */
    UINT16   bytes_left;        /* (Get/Put) Holds bytes available leop in Obx packet */
    BOOLEAN  final_pkt;         /* (Get)     Holds the final bit of the Put packet */
    UINT8    rsp_code;
} tBTA_PR_OBX_PKT;


/* SDP info */
typedef struct
{
    tBTA_SERVICE_ID     service_id;
    UINT8               scn;
    tBTA_PR_FMT_MASK    doc_formats;
#if (defined BIP_INCLUDED && BIP_INCLUDED == TRUE)
    tBIP_FEATURE_FLAGS  bip_features;
#endif

    tBTA_BP_SDP_PARAMS  bpp_params;
} tBTA_PR_SDP_INFO;

/* PR control block */
typedef struct
{
    UINT8              operation;     /* Current operation being performed by BTA_PR */
    tBTA_PR_CBACK     *p_cback;       /* pointer to application callback function */
    char              *p_name;        /* Holds the local path and file name of pushed item */
    tSDP_DISCOVERY_DB *p_db;          /* pointer to discovery database */
    tBTA_PR_BI_DESC   *p_bi_desc;     /* Additional parameters */
    tBTA_PR_BP_PARAMS  bp_parms;
    tBTA_PR_OBX_PKT    obx;           /* Holds the current OBX packet information */
    int                fd;            /* File Descriptor of opened file */
    UINT32             src_size;      /* (Push/Pull) file length */
    UINT32             src_bytes_left;

    tBTA_SERVICE_ID    svc_list[BTA_PR_SERVICES_MAX];       /* List of services to for performing SDP */
    UINT16             svc_uuid[BTA_PR_SERVICES_MAX];       /* UUID of services  */
    UINT8              svc_list_size;
    UINT8              svc_idx;     /* Index of service we are using to for printing (index into svc_list) */

    BD_ADDR            sdp_bda;                                 /* Device whose sdp info is stored in svc_info_list */
    tBTA_PR_SDP_INFO   sdp_results[BTA_PR_SERVICES_MAX];        /* List of services found */
    UINT8              sdp_results_size;
    tBTA_PR_FMT_VERS   sdp_doc_fmt_version[BTA_PR_IMG_FMT_MAX]; /* Version numbers of image formats supported by printer */

    tOBX_HANDLE        obx_handle;
    BD_ADDR            bd_addr;
    UINT8              sec_mask;
    tBTA_PR_INT_STATE  state;                   /* Connection state machine state */
    tBTA_PR_FMT        format;                  /* Format of document to print */
    tBTA_PR_IMG_FMT    image_format;            /* Format of image */
    UINT8              app_id;
    tBTA_PR_STATUS     status;
    BOOLEAN            cout_active;             /* TRUE if call-out is currently active */
    BOOLEAN            req_pending;             /* TRUE if an obx request to peer is in progress */
    BOOLEAN            printer_supports_xml;    /* TRUE if printer supports xhtml-print+xml */
    BOOLEAN            fmt_vers_verified;       /* TRUE if BTA_PR has verified that printer supports doc's format version */
    BOOLEAN            disabling;               /* TRUE if client is in process of disabling */
    BOOLEAN            is_enabled;              /* TRUE if client is enabled */
    UINT8              scn;

#if (defined BIP_INCLUDED && BIP_INCLUDED == TRUE)
    UINT8              bic_handle;
    UINT8              bis_handle;
    tBIP_IMAGING_CAPS  *p_caps;                 /* BIP imaging capabilities */
#endif
    BOOLEAN            bp_parms_used;           /* TRUE if specified by application */
    UINT8             *p_other_fmt;             /* other formats supported by BPP printer */
    UINT8             *p_header;                /* application specified type header when printing BTA_PR_OTHER_FMT using BPP */
    tBTA_BP_CB         bp_cb;                   /* BPP control block for sub-state machine */
} tBTA_PR_CB;

/* type for action functions */
typedef void (*tBTA_PR_ACTION)(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);

extern const tBTA_PR_ACTION p_pr_bi_action[];

enum
{
    BTA_PR_BI_SEND_PART_ACT_IDX         = 0,
    BTA_PR_BI_DEREGISTER_IDX,
    BTA_PR_BI_REGISTER_IDX,
    BTA_PR_BI_FREEXMLDATA_IDX,
    BTA_PR_BI_AUTH_RSP_IDX 
};

typedef const tBTA_PR_ACTION  (*tBTA_PR_BI_TBL);

/* run-time configuration struct */
typedef struct
{
    tBTA_PR_BI_TBL       p_pr_bi_action;      /* action function table for PR object channel */
    BOOLEAN              bi_included;         /* PR module include object channel or not */
} tBTA_PR_CFG;

extern tBTA_PR_CFG *p_bta_pr_cfg;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* PR control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_PR_CB  bta_pr_cb;
#else
extern tBTA_PR_CB *bta_pr_cb_ptr;
#define bta_pr_cb (*bta_pr_cb_ptr)
#endif

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

extern BOOLEAN  bta_pr_hdl_event(BT_HDR *p_msg);
extern void     bta_pr_sm_execute(tBTA_PR_CB *p_cb, UINT16 event,
                                   tBTA_PR_DATA *p_data);
extern void     bta_pr_obx_cback (tOBX_HANDLE handle, tOBX_EVENT event,
                                   UINT8 rsp_code, tOBX_EVT_PARAM param,
                                   BT_HDR *p_pkt);

/* action functions */
extern void bta_pr_init_getcaps(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_init_print(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_send_authrsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_sdp_next(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_send_authrsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_ci_read(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_ci_open(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_conn_rsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_abort_rsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_password(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_timeout(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_put_rsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_obx_get_rsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_initialize(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_trans_cmpl(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_stop_client(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_start_client(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_free_db(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_ignore_obx(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_find_service(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_close(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_close_complete(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_set_disable(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_snd_part(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_process_bp_evt(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);

/* miscellaneous functions */
extern UINT8    bta_pr_send_put_req(tBTA_PR_CB *p_cb, BOOLEAN first_pkt);

extern tBTA_PR_STATUS bta_pr_convert_obx_to_pr_status(tOBX_STATUS obx_status);
extern void bta_pr_cont_read(tBTA_PR_CB *p_cb);

extern UINT32   bta_pr_get_xhtml_doc_fmts(UINT8 *p_doc_fmts, UINT16 attr_len);
extern UINT32   bta_pr_get_opp_doc_fmts(tSDP_DISC_ATTR *p_disc_attr);
extern BOOLEAN  bta_pr_chk_fmt_vers(tBTA_PR_CB *p_cb, UINT8 *p_buf);
extern char *bta_bp_get_header(tBTA_PR_FMT format);

extern BOOLEAN bta_pr_service_already_found(tBTA_PR_CB *p_cb,
                                            tBTA_SERVICE_ID service_id,
                                            UINT8 *p_sdp_results_idx);

/* BI related function */
extern void bta_pr_snd_part_act(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_bi_deregister(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_bi_register(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_bi_freexmldata(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);
extern void bta_pr_bi_auth_rsp(tBTA_PR_CB *p_cb, tBTA_PR_DATA *p_data);

extern void bta_pr_bic_cback(tBTA_BIC_EVT event, tBTA_BIC *p_data);
extern void bta_pr_bpc_cback(tBPP_HANDLE handle, tBPP_CHANNEL channel,
                         tBPP_SND_EVT event, tBPP_SND *p_data);
#endif /* BTA_PR_INT_H */

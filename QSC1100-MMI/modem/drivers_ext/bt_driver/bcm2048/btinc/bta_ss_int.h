/*****************************************************************************
**
**  Name:           bta_ss_int.h
**
**  Description:    This is the private file for the synchronization
**                  server (SS).
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_SS_INT_H
#define BTA_SS_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "obx_api.h"
#include "bta_ss_api.h"
#include "bta_ss_co.h"
#include "bta_ss_ci.h"
#include "bta_fs_ci.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/



#define BTA_SS_TARGET_UUID                  "IRMC-SYNC"
#define BTA_SS_UUID_LENGTH                  9
#define BTA_SS_MAX_AUTH_KEY_SIZE            16  /* Must not be greater than OBX_MAX_AUTH_KEY_SIZE */

#define BTA_SS_DEFAULT_VERSION             0x0100

#define BTA_SS_DEV_INFO_SN_IND          10
#define BTA_SS_DT_INFO_MAX_REC_IND      2

#define BTA_SS_APP_LUID_TAG             1
#define BTA_SS_APP_CC_TAG               2

/* Data types supported - used for SDP record */
#define BTA_SS_SDP_VCARD        0x01
#define BTA_SS_SDP_VCAL         0x03
#define BTA_SS_SDP_VNOTE        0x05
#define BTA_SS_SDP_VMSG         0x06

#define SYNC_CMD_OBJ_NAME       "telecom/push.txt"

/* for the result of bta_ss_parse_name */
enum
{
    BTA_SS_RES_FAIL,
    BTA_SS_RES_OK,
    BTA_SS_RES_WHOLE
};

/* state machine events */
enum
{
    /* these events are handled by the state machine */
    BTA_SS_INT_DISABLE_EVT      = BTA_SYS_EVT_START(BTA_ID_SS), /* lower 8 bit = 0x00 */

    BTA_SS_INT_PSWD_EVT,        /*0x1 Response to password request */
    BTA_SS_FCI_WRITE_EVT,       /*0x2 Response to FS Write request */
    BTA_SS_FCI_READ_EVT,        /*0x3 Response to FS Read request */
    BTA_SS_FCI_OPEN_EVT,        /*0x4 Response to FS Open request */
    BTA_SS_CI_OPEN_EVT,         /*0x5 Response to PIM Open request */
    BTA_SS_CI_PUT_EVT,          /*0x6 Response to PIM PUT request */
    BTA_SS_CI_GET_EVT,          /*0x7 Response to PIM GET request */
    BTA_SS_OBX_CONN_EVT,        /*0x8 OBX Channel Connect Request */
    BTA_SS_OBX_DISC_EVT,        /*0x9 OBX Channel Disconnect */
    BTA_SS_OBX_ABORT_EVT,       /*0xA OBX_operation aborted */
    BTA_SS_OBX_PASSWORD_EVT,    /*0xB OBX password requested */
    BTA_SS_OBX_CLOSE_EVT,       /*0xC OBX Channel Disconnected (Link Lost) */
    BTA_SS_OBX_PUT_EVT,         /*0xD Write file data or delete */
    BTA_SS_OBX_GET_EVT,         /*0xE Read file data or folder listing */
    BTA_SS_CLOSE_CMPL_EVT,      /*0xF Finished closing channel */


    /* these events are handled by the SyncCmd client state machine */
    BTA_SSC_API_SYNC_CMD_EVT,   /* 0x10 */
    BTA_SSC_INT_PSWD_EVT,       /* 0x11 */
    BTA_SSC_CLOSE_EVT,          /* 0x12 */
    BTA_SSC_SDP_OK_EVT,         /* 0x13 */
    BTA_SSC_OBX_CONN_EVT,       /* 0x14 */
    BTA_SSC_OBX_PUT_RSP_EVT,    /* 0x15 */
    BTA_SSC_OBX_PASSWORD_EVT,   /* 0x16 */
    BTA_SSC_OBX_CLOSE_EVT,      /* 0x17 */
    BTA_SSC_OBX_ABORT_EVT,      /* 0x18 */

    /* these events are handled outside the state machine */
    BTA_SS_API_PSWD_EVT,        /* 0x19  Response to password request */
    BTA_SS_API_DISABLE_EVT,     /* 0x1a */
    BTA_SS_API_ENABLE_EVT
};

typedef UINT16 tBTA_SS_INT_EVT;

#define BTA_SSC_EVT_MASK    0x000F          /* from 0x0010 - 0x0018 is events for SSC */
#define BTA_SS_EVT_MASK     0x0010

/* state machine states */
typedef UINT8  tBTA_SS_STATE;

typedef struct
{
    char *p_id;
    char *p_val;
} tBTA_SS_PROP;

/* data type for BTA_FTS_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_SS_CBACK      *p_cback;
    char               *p_path;
    char                servicename[BTA_SERVICE_NAME_LEN + 1];
    UINT8               realm [OBX_MAX_REALM_LEN];    /* The realm is intended to be
                                                         displayed to users so they know
                                                         which userid and password to use. 
                                                         The first byte of the string is
                                                         the character set of the string.
                                                       */
    UINT8               realm_len;
    UINT8               sec_mask;
    UINT8               app_id;
    BOOLEAN             auth_enabled;
    tBTA_SS_DATA_TYPE_MASK  type_mask;
} tBTA_SS_API_ENABLE;

/* data type for BTA_FTS_API_AUTHRSP_EVT */
typedef struct
{
    BT_HDR  hdr;
    UINT8   key [BTA_SS_MAX_AUTH_KEY_SIZE];      /* The authentication key.*/
    UINT8   key_len;
    UINT8   userid [OBX_MAX_REALM_LEN];      /* The authentication user id.*/
    UINT8   userid_len;
} tBTA_SS_API_PSWD;

/* data type for all obex events
    hdr.event contains the FTS event
*/
typedef struct
{
    BT_HDR              hdr;
    tOBX_HANDLE         handle;
    tOBX_EVT_PARAM      param;
    BT_HDR             *p_pkt;
    tOBX_EVENT          obx_event;
    UINT8               rsp_code;
} tBTA_SS_OBX_EVENT;

/* data type for BTA_SSC_API_SYNC_CMD_EVT */
typedef struct
{
    BT_HDR                  hdr;
    BD_ADDR                 bd_addr;
    tBTA_SS_DATA_TYPE_MASK  type_mask;
    UINT8                   sec_mask;
}tBTA_SSC_API_SYNC;

/* data type for BTA_SSC_SDP_OK_EVT */
typedef struct
{
    BT_HDR  hdr;
    UINT8   scn;
} tBTA_SSC_SDP_OK_EVT;


/* union of all event data types */
typedef union
{
    BT_HDR                  hdr;
    tBTA_SS_API_ENABLE      api_enable;
    tBTA_SS_API_PSWD        auth_rsp;
    tBTA_SS_OBX_EVENT       obx_evt;
    tBTA_FS_CI_OPEN_EVT     open_evt;
    tBTA_FS_CI_READ_EVT     read_evt;
    tBTA_FS_CI_WRITE_EVT    write_evt;
    tBTA_SS_CI_CL_OPEN_EVT  cl_open;
    tBTA_SS_CI_PUT_EVT      put_evt;
    tBTA_SS_CI_GET_EVT      get_evt;
    /* data only used in Sync Cmd OBX client action func */
    tBTA_SSC_API_SYNC        api_sync;
    tBTA_SSC_SDP_OK_EVT     sdp_ok;
} tBTA_SS_DATA;


/* OBX Response Packet Structure - Holds current response packet info */
typedef struct
{
    BT_HDR  *p_pkt;             /* Holds the current OBX header for Put or Get */
    UINT8   *p_start;           /* Start of the Body of the packet */
    UINT32   file_size;         /* length of file (BTA_FS_LEN_UNKNOWN if not available) */
    UINT16   offset;            /* Contains the current offset into the Body (p_start) */
    UINT16   bytes_left;        /* Holds bytes available left in Obx packet */
} tBTA_SS_OBX_PKT;


#define BTA_SS_GET_ITEM_EOF     0x80    /* this bit is set to signal getting next item */
#define BTA_SS_PUT_ITEM_FINAL   0x80    /* this bit is set to signal EndOfBody */
#define BTA_SS_ALL_ITEMS        0x40    /* this bit is set to signal all PIM items */

/* control block for sync client obex client */
typedef struct
{
    UINT8                   sec_mask;           /* security level put on obex client channel for sync command */
    UINT8                   bit_read;           /* how many bit mask has been read into telecom/push.txt */
    BOOLEAN                 req_pending;        /* OBEX client channel status */
    tBTA_SS_STATUS          sts;                /* sync command op status */
    tOBX_HANDLE             obx_handle;         /* obex handle for the obex client */
    tBTA_SS_STATE           state;              /* state of obex client state machine */
    tBTA_SS_OBX_PKT         obx_pkt;            /* Holds the current OBX packet information */
    tBTA_SS_DATA_TYPE_MASK  type_mask;          /* A bit mask of the data types to be synchronized. */
    tSDP_DISCOVERY_DB       *p_db;              /* pointer to discovery database */
}tBTA_SSC_CB;

/* SS control block */
typedef struct
{
    tBTA_SS_CBACK   *p_cback;       /* pointer to application callback function */
    char            *p_obj_name;    /* Holds name of current operation */
    char            *p_file_name;   /* Holds file name for PUT operation */
    char            *p_did;         /* Hold the Database Id for this PIM */
    tBTA_SS_OBX_PKT  obx_pkt;       /* Holds the current OBX packet information */
    UINT32           sdp_handle;    /* SDP record handle */
    UINT32           cur_cc;        /* current change counter */
    int              fd_obx;        /* File Descriptor of opened file for obx operation */
    UINT16           obj_count;     /* total number of objects in PIM of the current data type */
    tOBX_HANDLE      obx_handle;    /* the handle for OBX server */
    tBTA_SS_OP       ss_op;         /* current active SYNC operation */
    UINT8            scn;           /* SCN of the SYNC server */
    UINT8            app_id;        /* Application ID passed into call out functions */
    tBTA_SS_STATE    state;         /* state machine state */
    BOOLEAN          cout_active;   /* TRUE when waiting for a call-in function */
    tBTA_SS_DATA_TYPE data_type;    /* the active PIM data type */
    BD_ADDR          peer_bdaddr;   /* bdaddr of peer device */
    tBTA_SSC_CB      sync_cmd_cb;
} tBTA_SS_CB;

/* type for action functions */
typedef void (*tBTA_SS_ACTION)(tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);

/* state table information used in both client/server state machine */
#define BTA_SS_NUM_ACTIONS         1       /* number of actions */
#define BTA_SS_ACTION              0       /* position of action */
#define BTA_SS_NEXT_STATE          1       /* position of next state */
#define BTA_SS_NUM_COLS            2       /* number of columns in state tables */

/* type for state table */
typedef const UINT8 (*tBTA_SS_ST_TBL)[BTA_SS_NUM_COLS];

/*****************************************************************************
**  Global data
*****************************************************************************/

/* SS control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_SS_CB  bta_ss_cb;
#else
extern tBTA_SS_CB *bta_ss_cb_ptr;
#define bta_ss_cb (*bta_ss_cb_ptr)
#endif

/* bta_ss_cfg.c */
/* SS configuration constants */
extern tBTA_SS_CFG *p_bta_ss_cfg;

extern  tBTA_SS_PROP * p_bta_ss_dev_info_prop ;
extern  tBTA_SS_PROP * p_bta_ss_pb_info_prop ;
extern  tBTA_SS_PROP * p_bta_ss_cal_info_prop ;
extern  tBTA_SS_PROP * p_bta_ss_note_info_prop ;
extern  tBTA_SS_PROP * p_bta_ss_msg_info_prop ;

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

/* bta_ss_main.c */
extern BOOLEAN  bta_ss_hdl_event(BT_HDR *p_msg);
extern void bta_ss_sm_execute(tBTA_SS_CB *p_cb, UINT16 event, tBTA_SS_DATA *p_data);
/* bta_ss_csm.c */
extern void bta_ssc_sm_execute(tBTA_SS_CB *p_ss_cb, UINT16 event, tBTA_SS_DATA *p_data);

/* bta_ss_act.c */
extern void bta_ss_api_disable (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_connect (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_conn_auth (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_pswd_evt (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_disc_evt (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_close (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_close_cmpl (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_pswd_rsp (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_write (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_read (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_open (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_pim (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_put (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_done_get (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_abort (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_proc_put (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ss_proc_get (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);

/* bta_ss_cact.c                                            */
/* action function used in Sync Cmd obx client state machine */
extern void bta_ssc_disable (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_find_service (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_start_client (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_conn_rsp (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_pswd_evt (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_pswd_rsp (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_proc_put (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_disconnect (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_close (tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);
extern void bta_ssc_ignore_obx(tBTA_SS_CB *p_cb, tBTA_SS_DATA *p_data);

extern void bta_ss_obx_cback (tOBX_HANDLE handle, tOBX_EVENT obx_event,
                        tOBX_EVT_PARAM param, BT_HDR *p_pkt);
/* bta_ss_utils.c */
extern void bta_ss_sdp_register (tBTA_SS_CB *p_cb, tBTA_SS_API_ENABLE *p_api);
extern UINT8 bta_ss_parse_name(BT_HDR *p_pkt, tBTA_SS_OP *p_op,
                          tBTA_SS_DATA_TYPE *p_type, char *p_name);
extern void bta_ss_hdr_init(tBTA_SS_OBX_PKT *p_cb, tOBX_HANDLE obx_handle);
extern BOOLEAN bta_ssc_send_put_req(tBTA_SS_OBX_PKT *p_obx, 
                                    tBTA_SS_DATA_TYPE_MASK type_mask, UINT8 *p_bit);
extern tBTA_SS bta_ss_pass_pswd (tBTA_SS_DATA *p_data);
extern void bta_ss_snd_pswd(tOBX_HANDLE obx_handle, tBTA_SS_DATA *p_data);

#endif /* BTA_SS_INT_H */


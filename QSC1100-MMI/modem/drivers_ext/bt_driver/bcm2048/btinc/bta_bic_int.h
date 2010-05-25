/*****************************************************************************
**
**  Name:           bta_bic_int.h
**
**  Description:    This is the private header file for the Basic Imaging (BI)
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_BIC_INT_H
#define BTA_BIC_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bip_api.h"
#include "bta_bi_api.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"


/*****************************************************************************
**  Client Constants and data types
*****************************************************************************/


/* state machine events */
enum
{
    /* these events are handled by the state machine */
    BTA_BIC_API_REG_EVT = BTA_SYS_EVT_START(BTA_ID_BIC),
    BTA_BIC_API_DEREG_EVT,
    BTA_BIC_CI_WRITE_EVT,       /* Call-in response to Write request */
    BTA_BIC_CI_READ_EVT,        /* Call-in response to Read request */
    BTA_BIC_CI_OPEN_EVT,        /* Call-in response to File Open request */
    BTA_BIC_API_REQ_EVT,        /* Push Object request */
    BTA_BIC_API_CLOSE_EVT,      /* Close connection event */
    BTA_BIC_BIP_EVT,            /* Events from BIP */
    BTA_BIC_CON_EVT,            /* Connected to the responder */
    BTA_BIC_CLOSED_EVT,         /* Finish the closing of the channel */
    BTA_BIC_INVALID_EVT
};

/* the state machine only handles events >= BTA_BIC_FIRST_SM_EVT */
/* the event >= BTA_BIC_RUN_SM_EVT is always handled by state machine */
/* the event < BTA_BIC_RUN_SM_EVT has a function in bta_bic_non_sm_evt[] to check if
                                       we need to run SM or not */
#define BTA_BIC_FIRST_EVT       BTA_BIC_API_REG_EVT /* register */
#define BTA_BIC_FIRST_SM_EVT    BTA_BIC_API_REQ_EVT /* request */
#define BTA_BIC_RUN_SM_EVT      BTA_BIC_BIP_EVT /* request */
#define BTA_BIC_FIRST_FS_EVT    BTA_BIC_CI_WRITE_EVT

typedef UINT16 tBTA_BIC_INT_EVT;

typedef UINT8 tBTA_BIC_STATE;

/* data type for BTA_BIC_API_REG_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_BIC_CBACK     *p_cback;
    UINT8               app_id;
} tBTA_BIC_API_REG;

typedef struct
{
    UINT8   key [GOEP_MAX_AUTH_KEY_SIZE];      /* The authentication key.*/
    UINT8   key_len;
    UINT8   userid [OBX_MAX_REALM_LEN];      /* The authentication user id.*/
    UINT8   userid_len;
} tBTA_BIC_API_AUTHRSP;

typedef union
{
    tBIP_PUT_IMAGE_REQ      put_img;
    tBIP_PUT_THUMB_REQ      put_thm;
    tBIP_START_PR_REQ       print;
    tBIP_SERVICE_ID         archive;
    tBIP_RMT_DISP_REQ       rmt_dsp;
    tBIP_GET_MONITOR_REQ    get_moni;
    tBIP_GET_IMG_REQ        get_img;
    tBIP_GET_THUMB_REQ      get_thm;
    tBIP_IMG_LIST_REQ       get_list;
    tBIP_IMG_HDL_STR        img_hdl;
    tBIP_GET_OBJ_RSP        get_part;
    tBTA_BIC_API_AUTHRSP    auth;
} tBTA_BIC_REQ_DATA;

/* data type for BTA_BIC_API_REQ_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_BIS_CBACK      *p_is_cback;
    UINT8               bic_handle;
    tBIP_SERVICE        bip_service;
    UINT8               scn;
    UINT8               bip_op;
    UINT8               sec_mask;
    BD_ADDR             addr;
    tBTA_BIC_REQ_DATA   param;
} tBTA_BIC_API_REQ;


/* data type for all bip events
    hdr.event contains the BIC event
*/
typedef union
{
    tBIP_PUT_RSP_EVT        put_rsp;
    tBIP_IMG_HDL_STR        img_hdl;
    tBIP_IMAGING_CAPS       caps;
    tBIP_HANDLE             bip_handle;
    tBIP_LIST_RSP_EVT       list;
    tBIP_IMAGE_PROPERTIES   props;
    tBIP_AUTH_EVT           auth;
    UINT32                  size;
    tOBX_RSP_CODE           obx_rsp;
} tBTA_BIC_BIP_DATA;

typedef struct
{
    BT_HDR              hdr;
    tBIP_IP_EVENT       evt;
    tBIP_STATUS         status;
    tBTA_BIC_BIP_DATA   *p_param;
} tBTA_BIC_BIP_EVT;

/* union of all event data types */
typedef union
{
    BT_HDR                  hdr;
    tBTA_BIC_API_REG        api_regst;
    tBTA_BIC_API_REQ        api_req;
    tBTA_BIC_BIP_EVT        bip_evt;
} tBTA_BIC_DATA;


/* BIC app registration block -
 * the BIC handle is (index).
 * invalid handle is BTA_BIC_MAX_REG
 * when p_cback is NULL, the entry is not used */
typedef struct
{
    tBTA_BIC_CBACK    *p_cback;     /* pointer to application callback function */
    UINT8              app_id;      /* module ID. 0 if this entry is disabled */
} tBTA_BIC_REG_BLK;


enum
{
    BTA_BIC_IS_NONE,    /* no initiator secondary channel */
    BTA_BIC_IS_API,     /* issued the BIP API to start IS */
    BTA_BIC_IS_FAIL,    /* fail to start IS */
    BTA_BIC_IS_UP       /* START event is received from IS */
};

/* the initiator secondary channel uses these events to talk to primary channel state machine
 * they pretend to be events from BIP. */
#define BTA_BI_IS_START_EVT     (BIP_IP_MAX_EVT)
#define BTA_BI_IS_STOP_EVT      (BIP_IP_MAX_EVT + 1)

/* BIC control block */
typedef struct
{
    tBTA_BIC_REG_BLK    reg[BTA_BIC_MAX_REG];/* Holds the registration parameters for each instance registered to BIC */
    tBTA_BIC_REG_BLK    *p_act;         /* The active registered instance */
    tBTA_BIC            *p_evt_data;    /* the event data for BI GET events */
    tBTA_BI_XML         *p_xml;         /* XML event data */
    UINT32              src_size;       /* (Push/Pull) file length */
    tGOEP_FD            fd;
    UINT16              fs_event;       /* the FS event for last call-out */
    tBIP_SERVICE        bip_service;    /* the BIP service for this connection */
    UINT8               bip_op;         /* the BIP OP for the API request */
    tBIP_HANDLE         bip_handle;     /* the BIP handle */
    tBTA_BIC_STATE      state;          /* state machine state */
    UINT8               dereg;          /* deregistering the client with handle as (dereg-1) */
    UINT8               sec_mask;       /* security mask */
    tBIP_SERVICE_ID     service_id;     /* The service ID of the Referenced Object service record. */
    tBTA_BIC_STATUS     status;         /* for BTA_BI_CLOSE_CEVT */
    tBTA_BIC_EVT        bi_evt;         /* BI GET event */
    UINT8               is_stat;        /* The status of initiator secondary channel */
} tBTA_BIC_CB;


/*****************************************************************************
**  Global data
*****************************************************************************/

/* BIC control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_BIC_CB  bta_bic_cb;
#else
extern tBTA_BIC_CB *bta_bic_cb_ptr;
#define bta_bic_cb (*bta_bic_cb_ptr)
#endif



/*****************************************************************************
**  Client Function prototypes
*****************************************************************************/

extern BOOLEAN  bta_bic_hdl_event(BT_HDR *p_msg);
extern void     bta_bic_sm_execute(tBTA_BIC_CB *p_cb, UINT16 event,
                                   tBTA_BIC_DATA *p_data);

/* state machine action functions */
extern void bta_bic_start_client(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_stop_client(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_auth_rsp(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_wc_bi_evt(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_connected(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_snd_req(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_con_bi_evt(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_con_close(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_cl_bi_evt(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern void bta_bic_cl_close(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);

/* non-state machine event handle functions */
extern BOOLEAN bta_bic_ci_write(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_ci_read(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_ci_open(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_api_register(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_api_deregister(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_api_request(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_data);
extern BOOLEAN bta_bic_api_close(tBTA_BIC_CB *p_cb, tBTA_BIC_DATA *p_msg);

extern void bta_bi_open (const UINT8 *p_name, UINT16 flags, UINT32 size,
                         UINT16 event_id, UINT8 app_id);
extern void bta_bi_close (tGOEP_FD fd, UINT8 app_id);
extern void bta_bi_read (tGOEP_FD fd, void *p_data, INT16 size, UINT16 event_id, UINT8 app_id);
extern void bta_bi_write (tGOEP_FD fd, const void *p_data, INT16 size,
                                  UINT16 event_id, UINT8 app_id);

#if BTA_BI_DEBUG == TRUE
extern char *bic_bip_evt_str(tBIP_IP_EVENT evt);
#endif

extern void bta_bis_start_is(tBTA_BIC_CB *p_cb, tBTA_BIS_CBACK *p_cback);


#endif /* BTA_BIC_INT_H */

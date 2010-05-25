/*****************************************************************************
**
**  Name:           bta_bis_int.h
**
**  Description:    This is the private header file for the Basic Imaging
**                  server (BIS)
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_BIS_INT_H
#define BTA_BIS_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bip_api.h"
#include "bta_bi_api.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"


/*****************************************************************************
**  Server Constants and data types
*****************************************************************************/
#define BTA_BI_IS_EVT_MASK          0x0010
#define BTA_BI_INT_SSM_EVT_MASK     0x00EF

#define BTA_BI_IS_EVT               (BTA_SYS_EVT_START(BTA_ID_BIC) | BTA_BI_IS_EVT_MASK)
#define BTA_BI_RP_EVT               (BTA_SYS_EVT_START(BTA_ID_BIS))

enum
{
    BTA_BI_INT_API_ENABLE_EVT,
    BTA_BI_INT_CI_WRITE_EVT,       /* Call-in response to Write request */
    BTA_BI_INT_CI_READ_EVT,        /* Call-in response to Read request */
    BTA_BI_INT_CI_OPEN_EVT,        /* Call-in response to File Open request */
    BTA_BI_INT_API_DISABLE_EVT,
    BTA_BI_INT_API_CLOSE_EVT,      /* close the connection */
    BTA_BI_INT_API_RSP_EVT,        /* response to an event */
    BTA_BI_INT_BIP_EVT,            /* Events from BIP */
    BTA_BI_INT_CON_EVT,            /* Connected to the responder */
    BTA_BI_INT_CLOSED_EVT,         /* Finish the closing of the channel */
    BTA_BI_INT_INVALID_EVT
};

/* the state machine only handles events >= BTA_BI_INT_FIRST_SM_EVT */
#define BTA_BI_INT_FIRST_EVT       BTA_BI_INT_CI_WRITE_EVT /* disable */
#define BTA_BI_INT_FIRST_SM_EVT    BTA_BI_INT_API_DISABLE_EVT /* request */


/*****************************************************************************
**  Initiator Secondary channel
*****************************************************************************/

/* state machine events for Initiator Secondary channel */
#define BTA_BI_IS_API_ENABLE_EVT    (BTA_BI_INT_API_ENABLE_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_API_DISABLE_EVT   (BTA_BI_INT_API_DISABLE_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_API_CLOSE_EVT     (BTA_BI_INT_API_CLOSE_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_CI_WRITE_EVT      (BTA_BI_INT_CI_WRITE_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_CI_READ_EVT       (BTA_BI_INT_CI_READ_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_CI_OPEN_EVT       (BTA_BI_INT_CI_OPEN_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_API_RSP_EVT       (BTA_BI_INT_API_RSP_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_BIP_EVT           (BTA_BI_INT_BIP_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_CON_EVT           (BTA_BI_INT_CON_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_CLOSED_EVT        (BTA_BI_INT_CLOSED_EVT | BTA_BI_IS_EVT)
#define BTA_BI_IS_INVALID_EVT       (BTA_BI_INT_INVALID_EVT | BTA_BI_IS_EVT)

#define BTA_BI_IS_FIRST_FS_EVT      BTA_BI_IS_CI_WRITE_EVT

/*****************************************************************************
**  Responder Primary channel
*****************************************************************************/
/* state machine events for Responder Primary channel */
#define BTA_BI_RP_API_ENABLE_EVT    (BTA_BI_INT_API_ENABLE_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_API_DISABLE_EVT   (BTA_BI_INT_API_DISABLE_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_API_CLOSE_EVT     (BTA_BI_INT_API_CLOSE_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_CI_WRITE_EVT      (BTA_BI_INT_CI_WRITE_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_CI_READ_EVT       (BTA_BI_INT_CI_READ_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_CI_OPEN_EVT       (BTA_BI_INT_CI_OPEN_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_API_RSP_EVT       (BTA_BI_INT_API_RSP_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_BIP_EVT           (BTA_BI_INT_BIP_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_CON_EVT           (BTA_BI_INT_CON_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_CLOSED_EVT        (BTA_BI_INT_CLOSED_EVT | BTA_BI_RP_EVT)
#define BTA_BI_RP_INVALID_EVT       (BTA_BI_INT_INVALID_EVT | BTA_BI_RP_EVT)

#define BTA_BI_RP_FIRST_FS_EVT      BTA_BI_RP_CI_WRITE_EVT


typedef UINT16 tBTA_BIS_INT_EVT;

typedef UINT8 tBTA_BIS_STATE;

typedef struct
{
    tBTA_SEC        sec_mask;
    tBIP_SDP_PARAMS sdp;
    tBTA_BIS_CBACK  *p_cback;
    UINT8           app_id;
} tBTA_BIS_ENABLE;

typedef union
{
    tBTA_BIS_ENABLE         enable;
    tBIP_GET_OBJ_RSP        obj;    /* BTA_BisPartialImageRsp, BTA_AcsGetImageRsp,
                                     * BTA_AcsGetThumbRsp,     BTA_AcsGetAttachRsp */
    tBIP_IMG_LIST_RSP       list;   /* BTA_AcsImagesListingRsp */
    tBIP_IMG_PROPS_RSP      prop;   /* BTA_AcsImagePropertiesRsp */
    tBIP_GET_MONITOR_RSP    moni;   /* BTA_AcsGetMonitorImageRsp */
    tOBX_RSP_CODE           rsp;    /* BTA_AcsDelImageRsp, BTA_AcsPutThumbRsp, BTA_AcsPutAttachRsp */
    UINT8                  *fname;  /* BTA_AcsPutContinue */
    tBIP_PUT_IMAGE_RSP      put_img;/* BTA_AcsPutImageRsp */
} tBTA_BIS_REQ_DATA;

typedef struct
{
    BT_HDR              hdr;
    UINT8               bic_handle;
    UINT8               bip_op;
    tBTA_BIS_REQ_DATA   param;
} tBTA_BIS_API_REQ;

/* data type for all bip events
    hdr.event contains the BIS event
*/
typedef union
{
    tBIP_HANDLE             bip_handle; /* BIP_IS_START_EVT         */
    BD_ADDR                 peer;       /* BIP_IS_CON_EVT           */
    tBIP_GET_PART_REQ_EVT   part;       /* BIP_IS_GET_PARTIAL_EVT   */
    tBIP_LIST_REQ_EVT       list;       /* BIP_IS_GET_LIST_EVT      */
    tBIP_IMG_HDL_STR        img_hdl;    /* BIP_IS_GET_PROP_EVT, BIP_IS_GET_THUMB_EVT,
                                         * BIP_IS_DEL_IMG_EVT, BIP_RP_PUT_THUMB_EVT */
    tBIP_GET_IMG_REQ_EVT    get_img;    /* BIP_IS_GET_IMG_EVT       */
    tBIP_RMT_DISP_REQ       rmt_dsp;    /* BIP_RP_REMOTE_DISP_EVT   */
    tBIP_GET_ATTACH_EVT     get_att;    /* BIP_IS_GET_ATTACH_EVT    */
    BOOLEAN                 store;      /* BIP_RP_GET_MONITOR_EVT   */
    tBIP_PUT_IMAGE_EVT      put_img;    /* BIP_RP_PUT_IMG_EVT       */
    tBIP_SERVICE_ID         service_id; /* BIP_RP_PRINT_EVT, BIP_RP_ARCHIVE_EVT     */
    UINT8                  *p;
} tBTA_BIS_BIP_DATA;

typedef struct
{
    BT_HDR              hdr;
    UINT8               evt;    /* BIP event: tBIP_IS_EVENT or tBIP_RP_EVENT */
    UINT8               bi_hdl; /* used only for enable event */
    tBIP_STATUS         status;
    tBTA_BIS_EVT        bi_evt;
    tBTA_BIS_BIP_DATA   *p_param;
} tBTA_BIS_BIP_EVT;

/* union of all event data types */
typedef union
{
    BT_HDR              hdr;
    tBTA_BIS_API_REQ    req;
    tBTA_BIS_BIP_EVT    bip_evt;
} tBTA_BIS_DATA;


typedef struct
{
    tBTA_BIS_CBACK     *p_cback;       /* pointer to application callback function */
    UINT32              src_size;       /* (Push/Pull) file length */
    UINT16              int_event;      /* the internal event mask */
    UINT16              fs_event;       /* the FS event for last call-out */
    tBIP_FEATURE_FLAGS  features;       /* supported features */
    tBIP_HANDLE         bip_handle;     /* the BIP handle */
    tBTA_BIS_STATE      state;          /* state machine state */
    UINT8               scn;
    UINT8               app_id;         /* module ID. 0 if this entry is disabled */
} tBTA_BI_SR_CB;

#define BTA_BIS_MAX_HANDLE      2       /* initiator secondary channel and responder primary channel */
#define BTA_BI_IS_CB_IND        (BTA_BI_IS_HANDLE - BTA_BIC_MAX_REG)
#define BTA_BI_RP_CB_IND        (BTA_BI_RP_HANDLE - BTA_BIC_MAX_REG)

/* BIS control block */
typedef struct
{
    tBTA_BI_SR_CB       sr[BTA_BIS_MAX_HANDLE];
} tBTA_BIS_CB;
/*****************************************************************************
**  Global data
*****************************************************************************/


/* BIS control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_BIS_CB  bta_bis_cb;     /* responder primary channel */
#else
extern tBTA_BIS_CB *bta_bis_cb_ptr;
#define bta_bis_cb (*bta_bis_cb_ptr)
#endif


/* from bta_bi_cfg.c */
extern tBTA_BI_CFG *p_bta_bi_cfg;

/* file system functions */
extern void bta_bi_open (const UINT8 *p_name, UINT16 flags, UINT32 size,
                         UINT16 event_id, UINT8 app_id);
extern void bta_bi_close (tGOEP_FD fd, UINT8 app_id);
extern void bta_bi_read (tGOEP_FD fd, void *p_data, INT16 size, UINT16 event_id, UINT8 app_id);
extern void bta_bi_write (tGOEP_FD fd, const void *p_data, INT16 size,
                                  UINT16 event_id, UINT8 app_id);
extern void bta_bi_seek (tGOEP_FD fd, INT32 offset, INT16 origin, UINT8 app_id);

/*****************************************************************************
**  Server Function prototypes
*****************************************************************************/
extern void bta_bis_sm_execute(tBTA_BI_SR_CB *p_cb, UINT16 event, tBTA_BIS_DATA *p_data);
extern void bta_bis_int_event(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);

extern BOOLEAN bta_bis_hdl_event(BT_HDR *p_msg);
extern void bta_bi_is_cback(tBIP_IS_EVENT evt, void *p, tBIP_STATUS status);
extern void bta_bi_rp_cback(tBIP_RP_EVENT evt, void *p, tBIP_STATUS status);

/* server action functions */
extern void bta_bis_disable(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_ls_bi_evt(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_con_rsp(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_con_bi_evt(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_con_close(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);

/* non-sm Event Handle Functions*/
extern void bta_bis_ci_write(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_ci_read(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);
extern void bta_bis_ci_open(tBTA_BI_SR_CB *p_cb, tBTA_BIS_DATA *p_data);

#if BTA_BI_DEBUG == TRUE
extern char *bis_bip_evt_str(tBIP_IS_EVENT evt);
#endif

#endif /* BTA_BIS_INT_H */

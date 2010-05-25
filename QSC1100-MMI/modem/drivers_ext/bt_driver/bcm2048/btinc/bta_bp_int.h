/*****************************************************************************
**
**  Name:           bta_bp_int.h
**
**  Description:    This is the private header file for the Basic Printing (BP)
**                  client subcomponent of the PR module.
**
**  Copyright (c) 2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_BP_INT_H
#define BTA_BP_INT_H
 
#include "bt_target.h"
#include "bta_sys.h"
#include "bpp_api.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"
#include "bta_pr_api.h"

/*****************************************************************************
**  Client Constants and data types
*****************************************************************************/
#define BTA_PR_BP_PENDING_NONE      0


typedef struct
{
    BD_ADDR *p_bda;
    UINT8    sec_level;
    UINT8    scn;
    UINT8    app_id;
} tBTA_BP_START_SND;

/* data type for BTA_BP_REF_OBJ_RSP_EVT */
typedef struct
{
    BT_HDR  hdr;
    char    *p_name;
} tBTA_PR_API_REFOBJ_RSP;

/* union of all event data types */
typedef union
{
    tBTA_BP_START_SND   start_snd;
    tBPP_CHANNEL        conn_chan;
    tBPP_PR_ATR_RSP     prattr_rsp;
    tBPP_CREATE_JOB_RSP job_rsp;
    tBPP_JOB_ATR_RSP    get_jattr;   
    tBPP_GET_EVT_RSP    getevent_rsp;
    tBPP_GET_ROBJ_REQ   get_robj;
    tBTA_PR_BP_PARAMS   job_parms;
    tBTA_FS_CI_OPEN_EVT ci_open;
    tBTA_FS_CI_READ_EVT ci_read;
    tBTA_PR_API_REFOBJ_RSP refobj_rsp;
    tBPP_STATUS         bpp_status;
    BT_HDR              hdr;
} tBTA_BP_DATA;


/* BP control block */
typedef struct
{
    tBTA_BP_PR_CAPS *p_pr_caps;      /* BP printer cababilities */
    tBPP_PR_ATR_RSP *p_pr_attrs;     /* BPP printer attributes */
    TIMER_LIST_ENT   job_attr_timer; /* Timer used to poll job status */   
    UINT32           job_id;
    UINT16           fs_evt;         /* Active File System call */
    BOOLEAN          is_doc_active;  /* Is BP currently transmitting a document? */
    BOOLEAN          keep_stat_act;  /* Keep BP status channel active? */
    BOOLEAN          use_ref_channel;
    BOOLEAN          cancel_job_pend;
    UINT8            auth_fail_cnt;
    tBTA_PR_STATUS   pr_status;
    UINT8            open_chnl;
    UINT16           req_pending_evt;
} tBTA_BP_CB;

/*****************************************************************************
**  Client Function prototypes
*****************************************************************************/

/* state machine action functions */
extern void bta_bp_start_sender(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_auth_rsp(UINT8 *p_password, UINT8 password_len,UINT8 *p_userid, UINT8 userid_len);
extern void bta_bp_create_job(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_init_status_chan(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_get_pr_attr(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_get_pr_attr_rsp(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_create_job_rsp(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_ci_open(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_ci_read(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_doc_complete(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_cancel_job_cmpl(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_proc_get_event(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_cancel_status(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_connect(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_proc_get_job_attr(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_cancel_job(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_data);
extern void bta_bp_timeout(tBTA_BP_CB *p_cb, tBTA_BP_DATA *p_null_data);

/* non-state machine event handle functions */
extern void bta_bp_handle_evt(tBTA_BP_DATA *p_data);
extern void bta_bp_open (const UINT8 *p_name, UINT16 flags, UINT32 size,
                         UINT16 event_id, UINT8 app_id);
extern void bta_bp_close (tGOEP_FD fd, UINT8 app_id);
extern void bta_bp_read (tGOEP_FD fd, void *p_data, INT16 size, UINT16 event_id,
                         UINT8 app_id);
extern void bta_bp_close (tGOEP_FD fd, UINT8 app_id);
extern void bta_bp_seek (tGOEP_FD fd, INT32 offset, INT16 origin, UINT8 app_id);

#endif /* BTA_BP_INT_H */

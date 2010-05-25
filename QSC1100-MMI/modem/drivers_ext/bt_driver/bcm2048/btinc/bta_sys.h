/*****************************************************************************
**
**  Name:           bta_sys.h
**
**  Description:    This is the public interface file for the BTA system
**                  manager.
**
**  Copyright (c) 2003-2005, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_SYS_H
#define BTA_SYS_H

#include "gki.h"


/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* event handler function type */
typedef BOOLEAN (tBTA_SYS_EVT_HDLR)(BT_HDR *p_msg);

/* disable function type */
typedef void (tBTA_SYS_DISABLE)(void);



#define BTA_ID_SYS          0            /* system manager */
#define BTA_ID_DM           1            /* device manager */
#define BTA_ID_DM_SEARCH    2            /* device manager search */
#define BTA_ID_DM_SEC       3            /* device manager security */
#define BTA_ID_DG           4            /* data gateway */
#define BTA_ID_AG           5            /* audio gateway */
#define BTA_ID_OPC          6            /* object push client */
#define BTA_ID_OPS          7            /* object push server */
#define BTA_ID_FTS          8            /* file transfer server */
#define BTA_ID_CT           9            /* cordless telephony terminal */
#define BTA_ID_FTC          10           /* file transfer client */
#define BTA_ID_SS           11           /* synchronization server */
#define BTA_ID_PR           12           /* Printer client */
#define BTA_ID_BIC          13           /* Basic Imaging Client */
#define BTA_ID_PAN          14           /* Personal Area Networking */
#define BTA_ID_BIS          15           /* Basic Imaging Server */
#define BTA_ID_ACC          16           /* Advanced Camera Client */
#define BTA_ID_SC           17           /* SIM Card Access server */
#define BTA_ID_AA           18           /* Advanced audio */
#define BTA_ID_HD           19           /* HID Device */
#define BTA_ID_CG           20           /* Cordless Gateway */
#define BTA_ID_BP           21           /* Basic Printing Client */
#define BTA_ID_PRM          22
#define BTA_ID_HH           23           /* Human Interface Device Host */
#define BTA_ID_PBS          24           /* Phone Book Access Server */
#define BTA_ID_PBC          25           /* Phone Book Access Client */
#define BTA_ID_JV           26           /* Java */
#define BTA_ID_FM           27           /* FM  */
#define BTA_ID_MAX          28
typedef UINT8 tBTA_SYS_ID;


#define BTA_SYS_CONN_OPEN           0
#define BTA_SYS_CONN_CLOSE          1
#define BTA_SYS_APP_OPEN            2
#define BTA_SYS_APP_CLOSE           3
#define BTA_SYS_SCO_OPEN            4
#define BTA_SYS_SCO_CLOSE           5
#define BTA_SYS_CONN_IDLE           6
#define BTA_SYS_CONN_BUSY           7


typedef UINT8 tBTA_SYS_CONN_STATUS;

/* tBTA_PREF_ROLES */
typedef UINT8 tBTA_SYS_PREF_ROLES;

/* conn callback for role / low power manager*/
typedef void (tBTA_SYS_CONN_CBACK)(tBTA_SYS_CONN_STATUS status,UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

#if (BTM_LISBON_INCLUDED == TRUE)&&( BTM_EIR_SERVER_INCLUDED == TRUE )&&(BTA_EIR_CANNED_UUID_LIST != TRUE)
/* eir callback for adding/removeing UUID */
typedef void (tBTA_SYS_EIR_CBACK)(UINT16 uuid16, BOOLEAN adding);
#endif

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
#include "btm_api.h"
/* data type for bta subsystems sync result */
typedef struct
{
    tBTM_STATUS     status;
    tBTM_SYNC_INFO  sync_info;
}tBTA_SYS_SYNC_RESULT;

typedef void (tBTA_SYS_IPC_EVT_CBACK)(tBTM_STATUS status, BT_HDR *p_data);
typedef BOOLEAN (tBTA_SYS_NOTIFY_CBACK)(void);
typedef void (tBTA_SYS_SYNC_REQ_CBACK)(void);

typedef void (tBTA_SYS_SYNC_RESULT_CBACK)(UINT8 subsys_id, tBTA_SYS_SYNC_RESULT *p_data);
typedef void (tBTA_SYS_IPC_REG_CBACK)(UINT8 subsys_id, tBTA_SYS_IPC_EVT_CBACK *p_cback);
typedef void (tBTA_SYS_NOTIFY_REG_CBACK)(UINT8 subsys_id, tBTA_SYS_NOTIFY_CBACK *p_cback);
typedef BOOLEAN (tBTA_SYS_SWITCH_STATUS_CBACK)(void);
typedef void (tBTA_SYS_SYNC_REG_CBACK)(UINT8 subsys_id, tBTA_SYS_SYNC_REQ_CBACK *p_cback);
#endif

/* registration structure */
typedef struct
{
    tBTA_SYS_EVT_HDLR   *evt_hdlr;
    tBTA_SYS_DISABLE    *disable;
} tBTA_SYS_REG;

/* system manager configuration structure */
typedef struct
{
    UINT16          mbox_evt;                       /* GKI mailbox event */
    UINT8           mbox;                           /* GKI mailbox id */
#if (BTU_STACK_LITE_ENABLED == FALSE)
    UINT8           timer;                          /* GKI timer id */
#endif
    UINT8           trace_level;                    /* initial trace level */
} tBTA_SYS_CFG;


/*****************************************************************************
**  Global data
*****************************************************************************/

/* trace level */
extern UINT8 appl_trace_level;

/*****************************************************************************
**  Macros
*****************************************************************************/

/* Calculate start of event enumeration; id is top 8 bits of event */
#define BTA_SYS_EVT_START(id)       ((id) << 8)

/*****************************************************************************
**  Function declarations
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BTA_API extern void bta_sys_init(void);
BTA_API extern void bta_sys_event(BT_HDR *p_msg);
BTA_API extern void bta_sys_timer_update(void);
BTA_API extern void bta_sys_disable_timers(void);
BTA_API extern void bta_sys_set_trace_level(UINT8 level);
extern void bta_sys_register(UINT8 id, const tBTA_SYS_REG *p_reg);
extern void bta_sys_deregister(UINT8 id);
extern BOOLEAN bta_sys_is_register(UINT8 id);
extern void bta_sys_sendmsg(void *p_msg);
extern void bta_sys_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, INT32 timeout);
extern void bta_sys_stop_timer(TIMER_LIST_ENT *p_tle);
extern void bta_sys_disable(void);



extern void bta_sys_rm_register(tBTA_SYS_CONN_CBACK * p_cback);
extern void bta_sys_pm_register(tBTA_SYS_CONN_CBACK * p_cback);
extern void bta_sys_compress_register(tBTA_SYS_CONN_CBACK * p_cback);
extern BOOLEAN bta_sys_check_compress(tBTA_SYS_ID id, UINT8 app_id, BD_ADDR peer_addr);

extern void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_app_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_app_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

#if (BTM_LISBON_INCLUDED == TRUE)&&( BTM_EIR_SERVER_INCLUDED == TRUE )&&(BTA_EIR_CANNED_UUID_LIST != TRUE)
extern void bta_sys_eir_register(tBTA_SYS_EIR_CBACK * p_cback);
extern void bta_sys_add_uuid(UINT16 uuid16);
extern void bta_sys_remove_uuid(UINT16 uuid16);
#endif

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
extern void bta_sys_sync_result_register(tBTA_SYS_SYNC_RESULT_CBACK *p_cback);
extern void bta_sys_notify_cback_register(tBTA_SYS_NOTIFY_REG_CBACK *p_cback);
extern void bta_sys_ipc_cback_register(tBTA_SYS_IPC_REG_CBACK *p_cback);
extern void bta_sys_switch_status_register(tBTA_SYS_SWITCH_STATUS_CBACK *p_cback);
extern void bta_sys_sync_cback_register(tBTA_SYS_SYNC_REG_CBACK *p_cback);

extern void bta_sys_sync_result(UINT8 subsys_id, tBTA_SYS_SYNC_RESULT *p_data);
extern void bta_sys_notify_register(UINT8 subsys_id, tBTA_SYS_NOTIFY_CBACK *p_cback);
extern void bta_sys_ipc_register(UINT8 subsys_id, tBTA_SYS_IPC_EVT_CBACK *p_cback);
extern BOOLEAN bta_sys_is_lite_active(void);
extern void bta_sys_sync_register(UINT8 subsys_id, tBTA_SYS_SYNC_REQ_CBACK *p_cback);
#endif

#ifdef __cplusplus
}
#endif

#endif /* BTA_SYS_H */

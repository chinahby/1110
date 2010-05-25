/*****************************************************************************
**
**  Name:           bta_ct_int.h
**
**  Description:    This is the private interface file for the BTA cordless
**                  terminal
**
**  Copyright (c) 2003-2005, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_CT_INT_H
#define BTA_CT_INT_H

#include "bta_sys.h"
#include "bta_api.h"
#include "bta_ct_api.h"


/*****************************************************************************
**  Constants
*****************************************************************************/



/* Number of CT connections */
#define BTA_CT_NUM_CONN         2

/* Temp till simultaneous connections
are supported */
#define BTA_CT_CTP_HANDLE       1
#define BTA_CT_IC_HANDLE        2


enum
{
    /* CT events */
    BTA_CT_API_CLOSE_EVT = BTA_SYS_EVT_START(BTA_ID_CT),
    BTA_CT_API_ORIGINATE_EVT,
    BTA_CT_API_ANSWER_EVT,
    BTA_CT_API_END_EVT,
    BTA_CT_API_FLASH_EVT,
    BTA_CT_API_DTMF_EVT,
    BTA_CT_L2CAP_OPEN_EVT,
    BTA_CT_L2CAP_CLOSE_EVT,
    BTA_CT_TCS_EVT,
    BTA_CT_CALL_OVER_EVT,
    BTA_CT_FIMA_SUGGEST_EVT,
    BTA_CT_FIMA_CONNECTED,
    BTA_CT_FIMA_DISCONNECTED,




    /* handled outside the state machine */
    BTA_CT_API_ENABLE_EVT,
    BTA_CT_API_DISABLE_EVT,
    BTA_CT_API_IC_REG_EVT,
    BTA_CT_API_GW_REG_EVT,
    BTA_CT_API_IC_DEREG_EVT,
    BTA_CT_API_GW_DEREG_EVT

};



/*****************************************************************************
**  Data types
*****************************************************************************/

/* data type for BTA_CT_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;                /* Event header */
    tBTA_SEC            sec_mask[2];
    tBTA_CT_CBACK      *p_cback;

} tBTA_CT_API_ENABLE;

/* data type for BTA_CT_API_IC_REG_EVT */
typedef struct
{
    BT_HDR              hdr;                /* Event header */
    UINT8               app_id;
    char                p_ic_name[BTA_SERVICE_NAME_LEN+1];

} tBTA_CT_API_IC_REG;


/* data type for BTA_CT_API_GW_REG_EVT */
typedef struct
{
    BT_HDR              hdr;                /* Event header */
    UINT8               app_id;
    BD_ADDR             bd_addr;
    BOOLEAN             wug_member;
    tCTP_TL_WUG_PARAMS  init_wug_params;

} tBTA_CT_API_GW_REG;


/* data type for BTA_CT_API_CLOSE_EVT */
typedef struct
{
    BT_HDR              hdr;                /* Event header */

} tBTA_CT_API_CLOSE;


/* data type for BTA_CT_API_ORIGINATE_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    tBTA_CT_IE_CALLED_NUM       cld_ie;
    BOOLEAN                     cld_ie_present;
    tBTA_CT_IE_CALLING_NUM      clg_ie;
    BOOLEAN                     clg_ie_present;
    tBTA_CT_CALL_CLASS          call_class;
    tBTA_CT_IE_CO_SPEC          cs;
    BOOLEAN                     cs_present;
    BOOLEAN                     fima_initiated;


} tBTA_CT_API_ORIGINATE;


/* data type for BTA_CT_API_ANSWER_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    tBTA_CT_IE_CO_SPEC          cs;
    BOOLEAN                     cs_present;


} tBTA_CT_API_ANSWER;


/* data type for BTA_CT_API_END_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    tBTA_CT_IE_CO_SPEC          cs;
    BOOLEAN                     cs_present;
    tBTA_CT_CLEAR_REASON        clear_reason;


} tBTA_CT_API_END;


/* data type for BTA_CT_API_FLASH_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */

} tBTA_CT_API_FLASH;


/* data type for BTA_CT_API_DTMF_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    UINT8                       dtmf;
    BOOLEAN                     key_status;

} tBTA_CT_API_DTMF;



/* data type for BTA_CT_INFO_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    tTCS_IE_KEYPAD_FACILITY     keypad_fac_ie;
    tBTA_CT_IE_CALLING_NUM      clg_num_ie;
    tBTA_CT_IE_CALLED_NUM       cld_num_ie;
    tBTA_CT_IE_AUDIO_CONTROL    audio_ctl_ie;
    tBTA_CT_IE_CO_SPEC          co_spec_ie;
} tBTA_CT_INFO_EVT;

/* data type for BTA_CT_TCS_EVT */
typedef struct
{
    BT_HDR                      hdr;                /* Event header */
    BD_ADDR                     bd_addr;
    tBTA_CT_CALL_CLASS          call_class;
    tBTA_CT_IE                  ie;
    UINT16                      link_id;
    tBTA_CT_SIGNAL              signal_value;
    UINT8                       cc_event;
} tBTA_CT_TCS_EVT;


/* union of all event datatypes */
typedef union
{

    BT_HDR                  hdr;
    tBTA_CT_API_ENABLE      enable;
    tBTA_CT_API_IC_REG      ic_reg;
    tBTA_CT_API_GW_REG      gw_reg;
    tBTA_CT_API_CLOSE       close;
    tBTA_CT_API_ORIGINATE   orginate;
    tBTA_CT_API_ANSWER      answer;
    tBTA_CT_API_END         end;
    tBTA_CT_API_FLASH       flash;
    tBTA_CT_API_DTMF        dtmf_cmd;
    tBTA_CT_INFO_EVT        info;
    tBTA_CT_TCS_EVT         tcs_evt;

} tBTA_CT_MSG;


/* typedef for state machine control block */
typedef struct
{


    UINT16                      state;
    UINT16                      tcs_cc_handle;
    UINT16                      link_id;
    BD_ADDR                     peer_bd_addr;
    UINT8                       handle;
    UINT8                       app_id;
    BOOLEAN                     in_use;
    BOOLEAN                     audio_up;
    tBTA_CT_CLEAR_REASON        cause;


} tBTA_CT_SCB;

typedef struct
{

    tBTA_CT_CBACK  *p_cback;                    /* application call back */
    tBTA_CT_SCB     scb[BTA_CT_NUM_CONN];       /* state machine control blocks */
    tBTA_SEC        ct_sec_mask;                /* security mask for CT */
    tBTA_SEC        ic_sec_mask;                /* security mask for IC */
    tBTA_SERVICE_MASK connected_service;        /* mask for currently active CT services */
    BOOLEAN         fima_connection;            /* flag to indicate that a connection is active through fima */
    UINT8           ct_bearer_info;             /* ct SCO settings */
    UINT8           ic_bearer_info;             /* ic SCO settings */
    char            ic_name[BTA_SERVICE_NAME_LEN+1];  /* ic service name for re registration */
    BOOLEAN         ic_enabled;
    BOOLEAN         enable;

} tBTA_CT_CB;

typedef void (*tBTA_CT_ACTION)(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);


#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_CT_CB bta_ct_cb;
#else
extern tBTA_CT_CB *bta_ct_cb_ptr;
#define bta_ct_cb (*bta_ct_cb_ptr)
#endif
typedef const tBTA_CT_ACTION  (*tCT_ACT_TBL);

extern const tBTA_CT_ACTION bta_ct_action[];
extern const tBTA_CT_ACTION bta_ic_action[];

typedef struct
{
    tCT_ACT_TBL             p_ct_action;
    tCT_ACT_TBL             p_ic_action;
    BOOLEAN                 access_request;
    tBTA_CT_LINK_TYPE       desired_link_type;  /* Desired SCO type (initiator only) */
} tBTA_CT_CFG;


extern tBTA_CT_CFG *p_bta_ct_cfg;


extern BOOLEAN bta_ct_hdl_event(BT_HDR *p_msg);

extern BOOLEAN bta_ct_sm_execute(tBTA_CT_SCB *p_scb, BT_HDR *p_msg);





extern void bta_ct_enable(tBTA_CT_MSG *p_data);       
extern void bta_ct_disable(tBTA_CT_MSG *p_data); 
extern void bta_ct_gw_register(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_deregister(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_orginate(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_answer(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_end(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_flash(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_dtmf(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_tcs_evt(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ct_l2cap_close(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_l2cap_open(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_l2cap_fail(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_call_over(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_fima_sugg(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_fima_open(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ct_fima_close(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);

extern void bta_ct_scb_dealloc(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern tBTA_CT_SCB *bta_ct_scb_by_handle(UINT8 handle);
extern tBTA_CT_SCB *bta_ct_scb_by_cc_handle(UINT16 cc_handle);


extern void bta_ic_register(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ic_deregister(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);


extern void bta_ic_orginate(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_answer(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_end(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_flash(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_dtmf(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_tcs_evt(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data); 
extern void bta_ic_l2cap_close(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ic_l2cap_open(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ic_l2cap_fail(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);
extern void bta_ic_call_over(tBTA_CT_SCB *p_scb, tBTA_CT_MSG *p_data);


extern void bta_ic_re_register(void);

#endif /* BTA_CT_INT_H */

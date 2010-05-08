/*****************************************************************************
**
**  Name:           bta_dm_int.h
**
**  Description:    This is the private interface file for the BTA device
**                  manager.
**
**  Copyright (c) 2003-2007, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_DM_INT_H
#define BTA_DM_INT_H




/*****************************************************************************
**  Constants and data types
*****************************************************************************/


#define BTA_COPY_DEVICE_CLASS(coddst, codsrc)   {((UINT8 *)(coddst))[0] = ((UINT8 *)(codsrc))[0]; \
                                                 ((UINT8 *)(coddst))[1] = ((UINT8 *)(codsrc))[1];  \
                                                 ((UINT8 *)(coddst))[2] = ((UINT8 *)(codsrc))[2];}


#define BTA_DM_MSG_LEN 50

#ifndef BTA_DM_REMOTE_DEVICE_NAME_LENGTH
#define BTA_DM_REMOTE_DEVICE_NAME_LENGTH 32
#endif

#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)       (1 << (id))


/* DM events */
enum
{
    /* device manager local device API events */
    BTA_DM_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_DM),
    BTA_DM_API_DISABLE_EVT,
    BTA_DM_API_SET_NAME_EVT,
    BTA_DM_API_SET_VISIBILITY_EVT,
    BTA_API_DM_SIG_STRENGTH_EVT,
    BTA_DM_ACL_CHANGE_EVT,

    /* security API events */
    BTA_DM_API_BOND_EVT,
    BTA_DM_API_BOND_CANCEL_EVT,
    BTA_DM_API_PIN_REPLY_EVT,
    BTA_DM_API_LINK_POLICY_EVT,
    BTA_DM_API_AUTH_REPLY_EVT,

    /* power manger events */
    BTA_DM_PM_BTM_STATUS_EVT,
    BTA_DM_PM_TIMER_EVT,

    /* simple pairing events */
#if (BTM_LISBON_INCLUDED == TRUE)
    BTA_DM_API_CONFIRM_EVT,
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
    BTA_DM_API_PASKY_CANCEL_EVT,
#endif
#if (BTM_OOB_INCLUDED == TRUE)
    BTA_DM_API_LOC_OOB_EVT,
    BTA_DM_CI_IO_REQ_EVT,
    BTA_DM_CI_RMT_OOB_EVT,
#endif /* BTM_OOB_INCLUDED */
#endif /* BTM_LISBON_INCLUDED */

    /* dual stack event */
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    BTA_DM_API_SWITCH_STACK_EVT, /* Event when App initiates BTA_DmSwitchStack() */
#endif

    BTA_DM_MAX_EVT
};


/* DM search events */
enum
{
    /* DM search API events */
    BTA_DM_API_SEARCH_EVT = BTA_SYS_EVT_START(BTA_ID_DM_SEARCH),
    BTA_DM_API_SEARCH_CANCEL_EVT,
    BTA_DM_API_DISCOVER_EVT,
    BTA_DM_INQUIRY_CMPL_EVT,
    BTA_DM_REMT_NAME_EVT,
    BTA_DM_SDP_RESULT_EVT,
    BTA_DM_SEARCH_CMPL_EVT,
    BTA_DM_DISCOVERY_RESULT_EVT,
    BTA_DM_API_DI_DISCOVER_EVT

};

/* data type for BTA_DM_API_ENABLE_EVT */
typedef struct
{
    BT_HDR              hdr;      
    tBTA_DM_SEC_CBACK *p_sec_cback;
} tBTA_DM_API_ENABLE;

/* data type for BTA_DM_API_SET_NAME_EVT */
typedef struct
{
    BT_HDR              hdr;      
    char    name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH];
} tBTA_DM_API_SET_NAME;

/* data type for BTA_DM_API_SET_VISIBILITY_EVT */
typedef struct
{
    BT_HDR              hdr;      
    tBTA_DM_DISC    disc_mode;
    tBTA_DM_CONN    conn_mode;
} tBTA_DM_API_SET_VISIBILITY;
 
/* data type for BTA_DM_API_SEARCH_EVT */
typedef struct
{
    BT_HDR      hdr;  
    tBTA_DM_INQ inq_params;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK * p_cback;

} tBTA_DM_API_SEARCH;

/* data type for BTA_DM_API_DISCOVER_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR bd_addr;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK * p_cback;
    BOOLEAN     sdp_search;

} tBTA_DM_API_DISCOVER;

/* data type for BTA_DM_API_DI_DISC_EVT */
typedef struct
{
    BT_HDR              hdr;  
    BD_ADDR             bd_addr;
    tBTA_DISCOVERY_DB   *p_sdp_db;
    UINT32              len;
    tBTA_DM_SEARCH_CBACK * p_cback;
}tBTA_DM_API_DI_DISC;

/* data type for BTA_DM_API_BOND_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR bd_addr;
} tBTA_DM_API_BOND;

/* data type for BTA_DM_API_BOND_CANCEL_EVT */
typedef struct
{
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
} tBTA_DM_API_BOND_CANCEL;

/* data type for BTA_DM_API_PIN_REPLY_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR bd_addr;
    BOOLEAN accept;
    UINT8 pin_len;
    UINT8 p_pin[PIN_CODE_LEN];
} tBTA_DM_API_PIN_REPLY;

/* data type for BTA_DM_API_LINK_POLICY_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR     bd_addr;
    UINT16      policy_mask;
    BOOLEAN     set;
} tBTA_DM_API_LINK_POLICY;

/* data type for BTA_DM_API_AUTH_REPLY_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR bd_addr;
    tBTA_SERVICE_ID service;
    tBTA_AUTH_RESP response;
} tBTA_DM_API_AUTH_REPLY;

/* data type for BTA_DM_API_LOC_OOB_EVT */
typedef struct
{
    BT_HDR      hdr;  
} tBTA_DM_API_LOC_OOB;

/* data type for BTA_DM_API_CONFIRM_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR     bd_addr;
    BOOLEAN     accept;
} tBTA_DM_API_CONFIRM;

/* data type for BTA_DM_API_PASKY_CANCEL_EVT*/
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR     bd_addr;
} tBTA_DM_API_PASKY_CANCEL;

/* data type for BTA_DM_CI_IO_REQ_EVT */
typedef struct
{
    BT_HDR          hdr;  
    BD_ADDR         bd_addr;
    tBTA_IO_CAP     io_cap;
    tBTA_OOB_DATA   oob_data;
    tBTA_AUTH_REQ   auth_req;
} tBTA_DM_CI_IO_REQ;

/* data type for BTA_DM_CI_RMT_OOB_EVT */
typedef struct
{
    BT_HDR      hdr;  
    BD_ADDR     bd_addr;
    BT_OCTET16  c;
    BT_OCTET16  r;
    BOOLEAN     accept;
} tBTA_DM_CI_RMT_OOB;

/* data type for BTA_DM_REMT_NAME_EVT */
typedef struct
{
    BT_HDR      hdr;  
    tBTA_DM_SEARCH  result;
} tBTA_DM_REM_NAME;

/* data type for tBTA_DM_DISC_RESULT */
typedef struct
{
    BT_HDR      hdr;  
    tBTA_DM_SEARCH  result;
} tBTA_DM_DISC_RESULT;


/* data type for BTA_DM_INQUIRY_CMPL_EVT */
typedef struct
{
    BT_HDR      hdr;  
    UINT8       num;
} tBTA_DM_INQUIRY_CMPL;

/* data type for BTA_DM_SDP_RESULT_EVT */
typedef struct
{
    BT_HDR      hdr;  
    UINT16 sdp_result;
} tBTA_DM_SDP_RESULT;

/* data type for BTA_API_DM_SIG_STRENGTH_EVT */
typedef struct
{
    BT_HDR      hdr;  
    tBTA_SIG_STRENGTH_MASK mask; 
    UINT16 period;
    BOOLEAN start;
} tBTA_API_DM_SIG_STRENGTH;

/* data type for BTA_DM_ACL_CHANGE_EVT */
typedef struct
{
    BT_HDR      hdr;  
    tBTM_BL_EVENT       event;
    UINT8               busy_level;
    BOOLEAN is_new;
    BD_ADDR bd_addr;
} tBTA_DM_ACL_CHANGE;

/* data type for BTA_DM_PM_BTM_STATUS_EVT */
typedef struct
{

    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tBTM_PM_STATUS  status;
    UINT16          value;
    UINT8           hci_status;

} tBTA_DM_PM_BTM_STATUS;

/* data type for BTA_DM_PM_TIMER_EVT */
typedef struct
{
    BT_HDR          hdr;
    BD_ADDR         bd_addr;

} tBTA_DM_PM_TIMER;


#if (BTU_DUAL_STACK_INCLUDED == TRUE)
/* data type for BTA_DM_API_SWITCH_STACK_EVT */
typedef struct
{
    BT_HDR          hdr;
    tBTA_DM_SW_DIR  dir;
    tBTA_DM_SWITCH_CBACK *p_cback;
}tBTA_DM_API_SWITCH_STACK;

#endif /* BTU_DUAL_STACK_INCLUDED */

/* union of all data types */
typedef union
{
    /* GKI event buffer header */
    BT_HDR              hdr;      
    tBTA_DM_API_ENABLE  enable;
    
    tBTA_DM_API_SET_NAME set_name;

    tBTA_DM_API_SET_VISIBILITY set_visibility;

    tBTA_DM_API_SEARCH search;

    tBTA_DM_API_DISCOVER discover;

    tBTA_DM_API_BOND bond;

    tBTA_DM_API_BOND_CANCEL bond_cancel;

    tBTA_DM_API_PIN_REPLY pin_reply;
    tBTA_DM_API_LINK_POLICY link_policy;

    tBTA_DM_API_LOC_OOB     loc_oob;
    tBTA_DM_API_CONFIRM     confirm;
    tBTA_DM_API_PASKY_CANCEL passkey_cancel;
    tBTA_DM_CI_IO_REQ       ci_io_req;
    tBTA_DM_CI_RMT_OOB      ci_rmt_oob;

    tBTA_DM_API_AUTH_REPLY auth_reply;

    tBTA_DM_REM_NAME rem_name;

    tBTA_DM_DISC_RESULT disc_result;

    tBTA_DM_INQUIRY_CMPL inq_cmpl;

    tBTA_DM_SDP_RESULT sdp_event;

    tBTA_API_DM_SIG_STRENGTH sig_strength;

    tBTA_DM_ACL_CHANGE  acl_change;

    tBTA_DM_PM_BTM_STATUS pm_status;

    tBTA_DM_PM_TIMER pm_timer;

    tBTA_DM_API_DI_DISC     di_disc;

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    tBTA_DM_API_SWITCH_STACK switch_stack;
#endif
} tBTA_DM_MSG;


#define BTA_DM_NUM_PEER_DEVICE 7

#define BTA_DM_NOT_CONNECTED  0
#define BTA_DM_CONNECTED      1

typedef UINT8 tBTA_DM_CONN_STATE;




typedef struct
{
    BD_ADDR                     peer_bdaddr;
    tBTA_DM_CONN_STATE          conn_state;
    tBTA_PREF_ROLES             pref_role;
    BOOLEAN                     in_use;
#if (BTM_LISBON_INCLUDED == TRUE)
    BOOLEAN                     use_ssr;    /* TRUE, if ssr is supported for this link */
    tBTM_PM_STATUS              prev_low;   /* previous low power mode used */
#endif
    tBTA_DM_PM_ACTTION          pm_mode_attempted;
    tBTA_DM_PM_ACTTION          pm_mode_failed;

} tBTA_DM_PEER_DEVICE;



/* structure to store list of
  active connections */
typedef struct
{
    tBTA_DM_PEER_DEVICE    peer_device[BTA_DM_NUM_PEER_DEVICE];
    UINT8                  count;

} tBTA_DM_ACTIVE_LINK;


typedef struct
{
    BD_ADDR                 peer_bdaddr;
    tBTA_SYS_ID             id;
    UINT8                   app_id;
    tBTA_SYS_CONN_STATUS    state;


} tBTA_DM_SRVCS;

#define BTA_DM_NUM_CONN_SRVS   5

typedef struct
{

    UINT8 count;
    tBTA_DM_SRVCS  conn_srvc[BTA_DM_NUM_CONN_SRVS];

}  tBTA_DM_CONNECTED_SRVCS;

typedef struct 
{
    TIMER_LIST_ENT          timer; 
    BD_ADDR                 peer_bdaddr;
    BOOLEAN                 in_use;

} tBTA_PM_TIMER;

extern tBTA_DM_CONNECTED_SRVCS bta_dm_conn_srvcs;

#define BTA_DM_NUM_PM_TIMER 3

/* DM control block */
typedef struct
{

    tBTA_DM_ACTIVE_LINK         device_list;
    tBTA_DM_SEC_CBACK           *p_sec_cback;
    TIMER_LIST_ENT              signal_strength_timer;
    tBTA_SIG_STRENGTH_MASK      signal_strength_mask;
    UINT16                      state;
    UINT16                      signal_strength_period;
    BOOLEAN                     disabling;
    TIMER_LIST_ENT              disable_timer;
    UINT8                       num_master_only;
    UINT32                      wbt_sdp_handle;          /* WIDCOMM Extensions SDP record handle */
    UINT8                       wbt_scn;                 /* WIDCOMM Extensions SCN */
    UINT8                       pm_id;
    tBTA_PM_TIMER               pm_timer[BTA_DM_NUM_PM_TIMER];

    /* Storage for pin code request parameters */
    BD_ADDR                     pin_bd_addr;
    DEV_CLASS                   pin_dev_class;
#if (BTM_LISBON_INCLUDED == TRUE)
    tBTA_DM_SEC_EVT             pin_evt;
    UINT32          num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN         just_works;     /* TRUE, if "Just Works" association model */
#if ( BTM_EIR_SERVER_INCLUDED == TRUE )&&( BTA_EIR_CANNED_UUID_LIST != TRUE )
    /* store UUID list for EIR */
    TIMER_LIST_ENT              app_ready_timer;
    UINT32                      eir_uuid[BTM_EIR_SERVICE_ARRAY_SIZE];
#endif
#endif

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
    BOOLEAN                     switched_to_lite;
    tBTA_DM_SWITCH_CBACK        *p_switch_stack_cback;
    tBTA_SYS_IPC_EVT_CBACK      *p_bta_av_ipc_evt_cback;
    tBTA_SYS_NOTIFY_CBACK       *p_bta_av_notify_cback;
    tBTA_SYS_SYNC_REQ_CBACK     *p_bta_av_sync_req_cback;
#endif
} tBTA_DM_CB;

#ifndef BTA_DM_SDP_DB_SIZE
#define BTA_DM_SDP_DB_SIZE 250
#endif

/* DM search control block */
typedef struct
{

    tBTA_DM_SEARCH_CBACK * p_search_cback;
    tBTM_INQ_INFO        * p_btm_inq_info;
    tBTA_SERVICE_MASK      services;
    tBTA_SERVICE_MASK      services_to_search;
    tBTA_SERVICE_MASK      services_found;
    tSDP_DISCOVERY_DB    * p_sdp_db;
    UINT16                 state;
    BD_ADDR                peer_bdaddr;
    char                   peer_name[BTA_DM_REMOTE_DEVICE_NAME_LENGTH];
    TIMER_LIST_ENT         search_timer;
    UINT8                  service_index;
    tBTA_DM_MSG          * p_search_queue;   /* search or discover commands during search cancel stored here */
    BOOLEAN                wait_disc;
    BOOLEAN                sdp_results;


} tBTA_DM_SEARCH_CB;

/* DI control block */
typedef struct
{
    tSDP_DISCOVERY_DB    * p_di_db;     /* pointer to the DI discovery database */
    UINT8               di_num;         /* total local DI record number */
    UINT32              di_handle[BTA_DI_NUM_MAX];  /* local DI record handle, the first one is primary record */
}tBTA_DM_DI_CB;

/* DM search state */
enum
{

    BTA_DM_SEARCH_IDLE,
    BTA_DM_SEARCH_ACTIVE,
    BTA_DM_SEARCH_CANCELLING,
    BTA_DM_DISCOVER_ACTIVE

};



typedef struct
{
    DEV_CLASS      dev_class;          /* local device class */
    UINT16         policy_settings;    /* link policy setting hold, sniff, park, MS switch */
    UINT16         page_timeout;       /* timeout for page in slots */
    UINT16         link_timeout;       /* link supervision timeout in slots */

} tBTA_DM_CFG;

extern const UINT32 bta_service_id_to_btm_srv_id_lkup_tbl[];

extern const tBTA_DM_CFG bta_dm_cfg;



#define BTA_ALL_APP_ID 0xff

typedef struct
{
    UINT8   id;
    UINT8   app_id;
    UINT8   cfg;

} tBTA_DM_RM ;

extern tBTA_DM_CFG *p_bta_dm_cfg;
extern tBTA_DM_RM *p_bta_dm_rm_cfg;

#define BTA_DM_NUM_COMPRESS_ENTRY    5
#define BTA_DM_COMPRESS_ENABLE        0x01      
#define BTA_DM_COMPRESS_DISALLOW      0x02

/*
 * indicate which BTA SYS can be compressed 
 */
typedef struct
{
    UINT8   id;
    UINT8   app_id;
    UINT8   mask;  /* enable or disallow */

} tBTA_DM_COMPRESS;

extern tBTA_DM_COMPRESS *p_bta_dm_compress_cfg;

/* 
 * Runtime BTA SYS compress state 
 */
typedef struct
{
    BD_ADDR                 peer_bdaddr;
    tBTA_SYS_ID             id;
    UINT8                   app_id;
    tBTA_SYS_CONN_STATUS    state;

} tBTA_DM_COMPRESS_SRVCS;
    

typedef struct
{

  UINT8  id;
  UINT8  app_id;
  UINT8  spec_idx;  /* index of spec table to use */

} tBTA_DM_PM_CFG;


typedef struct
{

  tBTA_DM_PM_ACTTION  power_mode;
  UINT16              timeout;

} tBTA_DM_PM_ACTN;

typedef struct
{

  UINT8  allow_mask;         /* mask of sniff/hold/park modes to allow */
#if (BTM_LISBON_INCLUDED == TRUE)
  UINT8  ssr;                /* set SSR on conn open/unpark */
#endif
  tBTA_DM_PM_ACTN actn_tbl [BTA_DM_PM_NUM_EVTS][2];

} tBTA_DM_PM_SPEC;

typedef struct
{
    UINT16      max_lat;
    UINT16      min_rmt_to;
    UINT16      min_loc_to;
} tBTA_DM_SSR_SPEC;

extern tBTA_DM_PM_CFG *p_bta_dm_pm_cfg;
extern tBTA_DM_PM_SPEC *p_bta_dm_pm_spec;
extern tBTM_PM_PWR_MD *p_bta_dm_pm_md;
#if (BTM_LISBON_INCLUDED == TRUE)
extern tBTA_DM_SSR_SPEC *p_bta_dm_ssr_spec;
#endif

#if (BTM_LISBON_INCLUDED == TRUE)&&( BTM_EIR_SERVER_INCLUDED == TRUE )
typedef struct
{
    UINT8   bta_dm_eir_min_name_len;        /* minimum length of local name when it is shortened */
#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
    UINT8   bta_dm_eir_uuid16_len;          /* length of 16-bit UUIDs */
    UINT8  *bta_dm_eir_uuid16;              /* 16-bit UUIDs */
#else
    UINT32  uuid_mask[BTM_EIR_SERVICE_ARRAY_SIZE]; /* mask of UUID list in EIR */
#endif
    UINT8   bta_dm_eir_flag_len;            /* length of flags in bytes */
    UINT8  *bta_dm_eir_flags;               /* flags for EIR */
    UINT8   bta_dm_eir_manufac_spec_len;    /* length of manufacturer specific in bytes */
    UINT8  *bta_dm_eir_manufac_spec;        /* manufacturer specific */
} tBTA_DM_EIR_CONF;

extern tBTA_DM_EIR_CONF *p_bta_dm_eir_cfg;
#endif

/* DM control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_CB  bta_dm_cb;
#else
extern tBTA_DM_CB *bta_dm_cb_ptr;
#define bta_dm_cb (*bta_dm_cb_ptr)
#endif

/* DM search control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_SEARCH_CB  bta_dm_search_cb;
#else
extern tBTA_DM_SEARCH_CB *bta_dm_search_cb_ptr;
#define bta_dm_search_cb (*bta_dm_search_cb_ptr)
#endif

/* DI control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_DI_CB  bta_dm_di_cb;
#else
extern tBTA_DM_DI_CB *bta_dm_di_cb_ptr;
#define bta_dm_di_cb (*bta_dm_di_cb_ptr)
#endif

extern BOOLEAN bta_dm_sm_execute(BT_HDR *p_msg);
extern BOOLEAN bta_dm_search_sm_execute(BT_HDR *p_msg);

extern void bta_dm_enable (tBTA_DM_MSG *p_data);
extern void bta_dm_disable (tBTA_DM_MSG *p_data);       
extern void bta_dm_set_dev_name (tBTA_DM_MSG *p_data);
extern void bta_dm_set_visibility (tBTA_DM_MSG *p_data);
extern void bta_dm_bond (tBTA_DM_MSG *p_data);         
extern void bta_dm_bond_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_pin_reply (tBTA_DM_MSG *p_data);     
extern void bta_dm_link_policy (tBTA_DM_MSG *p_data);
extern void bta_dm_auth_reply (tBTA_DM_MSG *p_data);    
extern void bta_dm_signal_strength(tBTA_DM_MSG *p_data);
extern void bta_dm_acl_change(tBTA_DM_MSG *p_data); 

extern void bta_dm_pm_btm_status(tBTA_DM_MSG *p_data);
extern void bta_dm_pm_timer(tBTA_DM_MSG *p_data);

#if (BTM_LISBON_INCLUDED == TRUE)
extern void bta_dm_confirm(tBTA_DM_MSG *p_data);
extern void bta_dm_passkey_cancel(tBTA_DM_MSG *p_data);
#if (BTM_OOB_INCLUDED == TRUE)
extern void bta_dm_loc_oob(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_io_req_act(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_rmt_oob_act(tBTA_DM_MSG *p_data);
#endif /* BTM_OOB_INCLUDED */
#endif /* BTM_LISBON_INCLUDED */

extern void bta_dm_init_pm(void);
extern void bta_dm_disable_pm(void);

extern void bta_dm_search_start (tBTA_DM_MSG *p_data); 
extern void bta_dm_search_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_discover (tBTA_DM_MSG *p_data); 
extern void bta_dm_di_disc (tBTA_DM_MSG *p_data);  
extern void bta_dm_inq_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_rmt_name (tBTA_DM_MSG *p_data);
extern void bta_dm_sdp_result (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_free_sdp_db (tBTA_DM_MSG *p_data);
extern void bta_dm_disc_result (tBTA_DM_MSG *p_data);
extern void bta_dm_search_result (tBTA_DM_MSG *p_data);
extern void bta_dm_discovery_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_queue_search (tBTA_DM_MSG *p_data);
extern void bta_dm_queue_disc (tBTA_DM_MSG *p_data);
extern void bta_dm_search_clear_queue (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_notify (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_transac_cmpl(tBTA_DM_MSG *p_data);
extern void bta_dm_disc_rmt_name (tBTA_DM_MSG *p_data);
extern tBTA_DM_PEER_DEVICE * bta_dm_find_peer_device(BD_ADDR peer_addr);

extern void bta_dm_pm_active(BD_ADDR peer_addr);

#if (BTM_LISBON_INCLUDED == TRUE)&&( BTM_EIR_SERVER_INCLUDED == TRUE )
void bta_dm_eir_update_uuid(UINT16 uuid16, BOOLEAN adding);
#endif

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
/*************************************************************************************/
/* Function called to switch stack when Application invokes BTA_DmSwitchStack() API  */
/*************************************************************************************/
extern void bta_dm_switch_stack (tBTA_DM_MSG *p_data);

/*************************************************************************************/
/* Function called by BTM (as a callback) to handle IPC events. This DM function     */
/* then calls the registered bta layer (bta_av) callback to inform of IPC events     */
/*************************************************************************************/
extern void bta_dm_hdl_ipc_evt(tBTM_STATUS status, BT_HDR *p_data);

/*************************************************************************************/
/* Function called by bta_sys when bta subsystems register a callback with DM.       */
/* The callback is invoked by DM when a IPC message received for the bta subsystem   */
/*************************************************************************************/
extern void bta_dm_register_ipc_cback(UINT8 subsys_id, tBTA_SYS_IPC_EVT_CBACK *p_cback);

/*************************************************************************************/
/* Function called by bta_sys when bta subsystems need to inform DM of the sync      */
/* result.                                                                           */
/* NOTE that the sync request would have been originated by DM and so it is being    */
/* informed of the result. DM initiates the bta sync process when App originates the */
/* bb->mm switch stack request.                                                      */
/*************************************************************************************/
extern void bta_dm_proc_sync_result(UINT8 subsys_id, tBTA_SYS_SYNC_RESULT *p_data);

/*************************************************************************************/
/* Function called by bta_sys when bta subsystems register 'notify' callbacks with   */
/* DM.                                                                               */
/*************************************************************************************/
extern void bta_dm_register_notify_cback(UINT8 subsys_id, tBTA_SYS_NOTIFY_CBACK *p_cback);

/*************************************************************************************/
/* Function called by bta_sys when bta subsystems need to ask DM whether switched    */
/* to Lite stack.                                                                    */
/* Returns TRUE if switched to Lite stack, else FALSE                                */
/*************************************************************************************/
extern BOOLEAN bta_dm_is_switched_to_lite(void);

/*************************************************************************************/
/* Function called by bta_sys when bta subsystems register a callback with DM.       */
/* The callback is invoked by DM when it wants to send sync request message to the   */
/* subsystem.                                                                        */
/*************************************************************************************/
extern void bta_dm_register_sync_cback(UINT8 subsys_id, tBTA_SYS_SYNC_REQ_CBACK *p_cback);

#endif /* BTU_DUAL_STACK_INCLUDED */

#endif /* BTA_DM_INT_H */


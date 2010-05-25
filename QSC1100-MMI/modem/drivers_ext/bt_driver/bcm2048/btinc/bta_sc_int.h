/*****************************************************************************
**
**  Name:           bta_sc_int.h
**
**  Description:    This is the private file for the SIM card
**                  server (SC).
**
**  Copyright (c) 2003-2004, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_SC_INT_H
#define BTA_SC_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bta_sc_api.h"
#include "bta_sc_ci.h"
#include "sap_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* RFCOMM MTU SIZE */
#define BTA_SC_MTU              127

/* Card reader status bits for TRANSFER_CARD_READER_STATUS message */
/* Described in GSM 11.14, section 12.33 */
#define BTA_SC_READER_ST_REMOVABLE          0x08
#define BTA_SC_READER_ST_READER_PRESENT     0x10
#define BTA_SC_READER_ST_CARD_PRESENT       0x40
#define BTA_SC_READER_ST_CARD_POWERED       0x80

/* Enumeration of internal BTA_SC events */
enum
{
    /* API events */
    BTA_SC_API_ENABLE = BTA_SYS_EVT_START(BTA_ID_SC),   /* Callback from SAP */
    BTA_SC_API_DISABLE,
    BTA_SC_API_CLOSE,
    BTA_SC_API_CARDSTATUS,
    BTA_SC_API_READERSTATUS,

    /* SAP callback event */
    BTA_SC_SAP_CBACK_EVT,

    /* Call in events */
    BTA_SC_CI_APDU_EVT,
    BTA_SC_CI_ATR_EVT,
    BTA_SC_CI_SIM_ON_EVT,
    BTA_SC_CI_SIM_OFF_EVT,
    BTA_SC_CI_SIM_RESET_EVT
};
#define BTA_SC_EVT_FIRST        (BTA_SYS_EVT_START(BTA_ID_SC))
typedef UINT8 BTA_SC_INT_EVT;


/* data type for BTA_SC_API_ENABLE_EVT internal event */
typedef struct
{
    BT_HDR      hdr;
    tBTA_SEC    sec_mask;
    char        *p_service_name;
    UINT8       reader_id;
    tBTA_SC_READER_FLAGS reader_flags;
    UINT16      msg_size_min;
    UINT16      msg_size_max;
    tBTA_SC_CBACK *p_cback;
} tBTA_SC_API_ENABLE;

/* data type for BTA_SC_API_CLOSE_EVT internal event */
typedef struct
{
    BT_HDR      hdr;
    tBTA_SC_DISCONNECT_TYPE type;
} tBTA_SC_API_CLOSE;

/* data type for BTA_SC_API_CARDSTATUS_EVT internal event */
typedef struct
{
    BT_HDR      hdr;
    tBTA_SC_CARD_STATUS status;
} tBTA_SC_API_CARDSTATUS;

/* data type for BTA_SC_API_READERSTATUS_EVT internal event */
typedef struct
{
    BT_HDR      hdr;
    tBTA_SC_READER_STATUS status;
} tBTA_SC_API_READERSTATUS;

/* data type for BTA_SC_SAP_CBACK_EVT internal event */
typedef struct
{
    BT_HDR                  hdr;
    void                   *p_req;      /* Pointer to APDU request (for sap_event=SAP_TRANSFER_APDU_REQ_EVT) */
    UINT16                  msg_size;   /* message size requested by client (for sap_event=SAP_CONNECT_REQ_EVT) */
    BD_ADDR                 bd_addr;
    tSAP_EVT                sap_event;  /* SAP callback event */
} tBTA_SC_SAP_CBACK;

/* data type for BTA_SC_CI_SIM_CTRL_EVT */
typedef struct
{
    BT_HDR      hdr;
    tBTA_SC_RESULT          result;
} tBTA_SC_CI_SIM_CTRL;

/* union of all event data types */
typedef union
{
    BT_HDR                  hdr;
    tBTA_SC_API_ENABLE      api_enable;
    tBTA_SC_API_CLOSE       api_close;
    tBTA_SC_API_CARDSTATUS  api_cardstatus;
    tBTA_SC_API_READERSTATUS api_readerstatus;
    tBTA_SC_SAP_CBACK       sap_cback;
    tBTA_SC_CI_SIM_CTRL     ci_sim_ctrl;
} tBTA_SC_DATA;

/* SC control block */
typedef struct
{
    tBTA_SC_CBACK   *p_cback;       /* pointer to application callback function */
    BT_HDR          *p_apdu_req;    /* APDU request buffer current being processed */
    UINT32          sdp_handle;     /* Handle for SAP service SDP record */
    UINT32          reader_flags;   /* Card reader description (used in TRANSFER_CARD_READER_STATUS_REQ) */
    UINT16          msg_size_min;   /* Min message size for SIM APDU commands */
    UINT16          msg_size_max;   /* Max message size for SIM APDU commands */
    UINT16          msg_size_client;/* Max message size requested by client */
    BD_ADDR         peer_bdaddr;    /* bdaddr of peer device */
    BOOLEAN         enabled;        /* TRUE if server is enabled */
    BOOLEAN         connecting;     /* TRUE if received CONNECT_REQ, but waiting for SIM to reset */
    tBTA_SC_CARD_STATUS     card_status;
    tBTA_SC_READER_STATUS   reader_status;
    UINT8           scn;            /* SCN number for service */
    UINT8           reader_id;      /* ID of card reader (used in TRANSFER_CARD_READER_STATUS_REQ) */

} tBTA_SC_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* SS control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_SC_CB  bta_sc_cb;
#else
extern tBTA_SC_CB *bta_sc_cb_ptr;
#define bta_sc_cb (*bta_sc_cb_ptr)
#endif

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

/* bta_scs_main.c */
extern BOOLEAN  bta_sc_hdl_event(BT_HDR *p_msg);

/* SAP callback function */
void bta_sc_sap_cback(tSAP_EVT event, tSAP_EVT_DATA *p_evt_data);

/* Action functions for BTA_SC internal events */
BOOLEAN bta_sc_act_api_enable(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_api_disable(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_api_close(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_api_cardstatus(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_api_readerstatus(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_sap_event(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_ci_apdu(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_ci_atr(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_ci_sim_on(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_ci_sim_off(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);
BOOLEAN bta_sc_act_ci_sim_reset(tBTA_SC_CB *p_cb, tBTA_SC_DATA *p_data);

/* Handlers for SAP callback notifications (BTA_SC_SAP_CBACK_EVT) */
void bta_sc_rfcomm_open(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_rfcomm_close(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_connect_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_disconnect_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_transfer_apdu_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_transfer_atr_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_power_sim_on_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_power_sim_off_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_reset_sim_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);
void bta_sc_card_reader_status_req(tBTA_SC_CB *p_cb, tBTA_SC_SAP_CBACK *p_sap_evt);





#endif /* BTA_SC_INT_H */


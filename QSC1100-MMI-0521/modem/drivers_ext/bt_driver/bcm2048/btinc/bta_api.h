/*****************************************************************************
**
**  Name:           bta_api.h
**
**  Description:    This is the public interface file for BTA, Widcomm's 
**                  Bluetooth application layer for mobile phones.
**
**  Copyright (c) 2003-2007, Broadcom Corp., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_API_H
#define BTA_API_H

#include "data_types.h"
#include "bt_target.h"
#include "bt_types.h"
#include "btm_api.h"
 
#ifndef     BTA_DI_INCLUDED
#define     BTA_DI_INCLUDED     FALSE
#endif
/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Status Return Value */
#define BTA_SUCCESS             0           /* Successful operation. */
#define BTA_FAILURE             1           /* Generic failure. */

typedef UINT8 tBTA_STATUS;

/*
 * Service ID 
 *
 * NOTES: When you add a new Service ID for BTA AND require to change the value of BTA_MAX_SERVICE_ID, 
 *        make sure that the correct security ID of the new service from Security service definitions (btm_api.h) 
 *        should be added to bta_service_id_to_btm_srv_id_lkup_tbl table in bta_dm_act.c.
 */

#define BTA_RES_SERVICE_ID      0           /* Reserved */
#define BTA_SPP_SERVICE_ID      1           /* Serial port profile. */
#define BTA_DUN_SERVICE_ID      2           /* Dial-up networking profile. */
#define BTA_FAX_SERVICE_ID      3           /* Fax profile. */
#define BTA_LAP_SERVICE_ID      4           /* LAN access profile. */
#define BTA_HSP_SERVICE_ID      5           /* Headset profile. */
#define BTA_HFP_SERVICE_ID      6           /* Hands-free profile. */
#define BTA_OPP_SERVICE_ID      7           /* Object push  */
#define BTA_FTP_SERVICE_ID      8           /* File transfer */
#define BTA_CTP_SERVICE_ID      9           /* Cordless Terminal */
#define BTA_ICP_SERVICE_ID      10          /* Intercom Terminal */
#define BTA_SYNC_SERVICE_ID     11          /* Synchronization */
#define BTA_BPP_SERVICE_ID      12          /* Basic printing profile */
#define BTA_BIP_SERVICE_ID      13          /* Basic Imaging profile */
#define BTA_PANU_SERVICE_ID     14          /* PAN User */
#define BTA_NAP_SERVICE_ID      15          /* PAN Network access point */
#define BTA_GN_SERVICE_ID       16          /* PAN Group Ad-hoc networks */
#define BTA_SAP_SERVICE_ID      17          /* SIM Access profile */
#define BTA_A2DP_SERVICE_ID     18          /* Advanced audio distribution */
#define BTA_AVRCP_SERVICE_ID    19          /* A/V remote control */
#define BTA_HID_SERVICE_ID      20          /* HID */
#define BTA_VDP_SERVICE_ID      21          /* Video distribution */
#define BTA_PBAP_SERVICE_ID     22          /* PhoneBook Access */
#define BTA_MAX_SERVICE_ID      23

/* service IDs (BTM_SEC_SERVICE_FIRST_EMPTY + 1) to (BTM_SEC_MAX_SERVICES - 1)
 * are used by BTA JV */
#define BTA_FIRST_JV_SERVICE_ID     (BTM_SEC_SERVICE_FIRST_EMPTY + 1)
#define BTA_LAST_JV_SERVICE_ID      (BTM_SEC_MAX_SERVICES - 1)

typedef UINT8 tBTA_SERVICE_ID;

/* Service ID Mask */
#define BTA_RES_SERVICE_MASK        0x00000001  /* Reserved */
#define BTA_SPP_SERVICE_MASK        0x00000002  /* Serial port profile. */
#define BTA_DUN_SERVICE_MASK        0x00000004  /* Dial-up networking profile. */
#define BTA_FAX_SERVICE_MASK        0x00000008  /* Fax profile. */
#define BTA_LAP_SERVICE_MASK        0x00000010  /* LAN access profile. */
#define BTA_HSP_SERVICE_MASK        0x00000020  /* Headset profile. */
#define BTA_HFP_SERVICE_MASK        0x00000040  /* Hands-free profile. */
#define BTA_OPP_SERVICE_MASK        0x00000080  /* Object push  */
#define BTA_FTP_SERVICE_MASK        0x00000100  /* File transfer */
#define BTA_CTP_SERVICE_MASK        0x00000200  /* Cordless Terminal */
#define BTA_ICP_SERVICE_MASK        0x00000400  /* Intercom Terminal */
#define BTA_SYNC_SERVICE_MASK       0x00000800  /* Synchronization */
#define BTA_BPP_SERVICE_MASK        0x00001000  /* Print server */
#define BTA_BIP_SERVICE_MASK        0x00002000  /* Basic Imaging */
#define BTA_PANU_SERVICE_MASK       0x00004000  /* PAN User */
#define BTA_NAP_SERVICE_MASK        0x00008000  /* PAN Network access point */
#define BTA_GN_SERVICE_MASK         0x00010000  /* PAN Group Ad-hoc networks */
#define BTA_SAP_SERVICE_MASK        0x00020000  /* PAN Group Ad-hoc networks */
#define BTA_A2DP_SERVICE_MASK       0x00040000  /* Advanced audio distribution */
#define BTA_AVRCP_SERVICE_MASK      0x00080000  /* A/V remote control */
#define BTA_HID_SERVICE_MASK        0x00100000  /* HID */
#define BTA_VDP_SERVICE_MASK        0x00200000  /* Video distribution */
#define BTA_PBAP_SERVICE_MASK       0x00400000  /* PhoneBook Access */
#define BTA_ALL_SERVICE_MASK        0x007FFFFF  /* All services supported by BTA. */

typedef UINT32 tBTA_SERVICE_MASK;

/* Security Setting Mask */
#define BTA_SEC_NONE            BTM_SEC_NONE                                         /* No security. */
#define BTA_SEC_AUTHORIZE       (BTM_SEC_IN_AUTHORIZE )                              /* Authorization required (only needed for out going connection )*/
#define BTA_SEC_AUTHENTICATE    (BTM_SEC_IN_AUTHENTICATE | BTM_SEC_OUT_AUTHENTICATE) /* Authentication required. */
#define BTA_SEC_ENCRYPT         (BTM_SEC_IN_ENCRYPT | BTM_SEC_OUT_ENCRYPT)           /* Encryption required. */

typedef UINT8 tBTA_SEC;

/* Discoverable Modes */
#define BTA_DM_NON_DISC         BTM_NON_DISCOVERABLE        /* Device is not discoverable. */
#define BTA_DM_GENERAL_DISC     BTM_GENERAL_DISCOVERABLE    /* General discoverable. */
#define BTA_DM_LIMITED_DISC     BTM_LIMITED_DISCOVERABLE    /* Limited discoverable. */

typedef UINT8 tBTA_DM_DISC;

/* Connectable Modes */
#define BTA_DM_NON_CONN         BTM_NON_CONNECTABLE         /* Device is not connectable. */
#define BTA_DM_CONN             BTM_CONNECTABLE             /* Device is connectable. */

typedef UINT8 tBTA_DM_CONN;

/* Inquiry Modes */
#define BTA_DM_GENERAL_INQUIRY  BTM_GENERAL_INQUIRY         /* Perform general inquiry. */
#define BTA_DM_LIMITED_INQUIRY  BTM_LIMITED_INQUIRY         /* Perform limited inquiry. */

typedef UINT8 tBTA_DM_INQ_MODE;

/* Inquiry Filter Type */
#define BTA_DM_INQ_CLR          BTM_CLR_INQUIRY_FILTER          /* Clear inquiry filter. */
#define BTA_DM_INQ_DEV_CLASS    BTM_FILTER_COND_DEVICE_CLASS    /* Filter on device class. */
#define BTA_DM_INQ_BD_ADDR      BTM_FILTER_COND_BD_ADDR         /* Filter on a specific  BD address. */

typedef UINT8 tBTA_DM_INQ_FILT;

/* Authorize Response */
#define BTA_DM_AUTH_PERM        0      /* Authorized for future connections to the service */
#define BTA_DM_AUTH_TEMP        1      /* Authorized for current connection only */
#define BTA_DM_NOT_AUTH         2      /* Not authorized for the service */ 

typedef UINT8 tBTA_AUTH_RESP;

/* M/S preferred roles */
#define BTA_ANY_ROLE          0x00
#define BTA_MASTER_ROLE_PREF  0x01
#define BTA_MASTER_ROLE_ONLY  0x02

typedef UINT8 tBTA_PREF_ROLES;

enum
{

    BTA_DM_NO_SCATTERNET,        /* Device doesn't support scatternet, it might 
                                    support "role switch during connection" for
                                    an incoming connection, when it already has
                                    another connection in master role */
    BTA_DM_PARTIAL_SCATTERNET,   /* Device supports partial scatternet. It can have
                                    simulateous connection in Master and Slave roles
                                    for short period of time */
    BTA_DM_FULL_SCATTERNET       /* Device can have simultaneous connection in master
                                    and slave roles */

};


/* Inquiry filter device class condition */
typedef struct             
{
    DEV_CLASS       dev_class;        /* device class of interest */
    DEV_CLASS       dev_class_mask;   /* mask to determine the bits of device class of interest */
} tBTA_DM_COD_COND;


/* Inquiry Filter Condition */
typedef union
{
    BD_ADDR              bd_addr;            /* BD address of  device to filter. */
    tBTA_DM_COD_COND     dev_class_cond;     /* Device class filter condition */
} tBTA_DM_INQ_COND;

/* Inquiry Parameters */
typedef struct
{
    tBTA_DM_INQ_MODE    mode;           /* Inquiry mode, limited or general. */
    UINT8               duration;       /* Inquiry duration in 1.28 sec units. */
    UINT8               max_resps;      /* Maximum inquiry responses.  Set to zero for unlimited responses. */
    tBTA_DM_INQ_FILT    filter_type;    /* Filter condition type. */
    tBTA_DM_INQ_COND    filter_cond;    /* Filter condition data. */
} tBTA_DM_INQ;


typedef INT8 tBTA_DM_RSSI_VALUE;
typedef UINT8 tBTA_DM_LINK_QUALITY_VALUE;


/* signal strength mask */
#define BTA_SIG_STRENGTH_RSSI_MASK          1
#define BTA_SIG_STRENGTH_LINK_QUALITY_MASK  2

typedef UINT8 tBTA_SIG_STRENGTH_MASK;


/* Security Callback Events */
#define BTA_DM_ENABLE_EVT               0       /* Enable Event */
#define BTA_DM_DISABLE_EVT              1       /* Disable Event */
#define BTA_DM_PIN_REQ_EVT              2       /* PIN request. */
#define BTA_DM_AUTH_CMPL_EVT            3       /* Authentication complete indication. */
#define BTA_DM_AUTHORIZE_EVT            4       /* Authorization request. */
#define BTA_DM_LINK_UP_EVT              5       /* Connection UP event */
#define BTA_DM_LINK_DOWN_EVT            6       /* Connection DOWN event */
#define BTA_DM_SIG_STRENGTH_EVT         7       /* Signal strength for bluetooth connection */
#define BTA_DM_BUSY_LEVEL_EVT           8       /* System busy level */
#define BTA_DM_BOND_CANCEL_CMPL_EVT     9       /* Bond cancel complete indication */
#define BTA_DM_SP_CFM_REQ_EVT           10      /* Simple Pairing User Confirmation request. */
#define BTA_DM_SP_KEY_NOTIF_EVT         11      /* Simple Pairing Passkey Notification */
#define BTA_DM_SP_RMT_OOB_EVT           12      /* Simple Pairing Remote OOB Data request. */
#define BTA_DM_SP_KEYPRESS_EVT          13      /* Key press notification event. */

typedef UINT8 tBTA_DM_SEC_EVT;

/* Structure associated with BTA_DM_ENABLE_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address of local device. */
} tBTA_DM_ENABLE;

/* Structure associated with BTA_DM_PIN_REQ_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    DEV_CLASS       dev_class;          /* Class of Device */
} tBTA_DM_PIN_REQ;


/* Structure associated with BTA_DM_AUTH_CMPL_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    BOOLEAN         key_present;        /* Valid link key value in key element */
    LINK_KEY        key;                /* Link key associated with peer device. */
    BOOLEAN         success;            /* TRUE of authentication succeeded, FALSE if failed. */
} tBTA_DM_AUTH_CMPL;

/* Structure associated with BTA_DM_AUTHORIZE_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    tBTA_SERVICE_ID service;            /* Service ID to authorize. */
} tBTA_DM_AUTHORIZE;

/* Structure associated with BTA_DM_LINK_UP_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
} tBTA_DM_LINK_UP;

/* Structure associated with BTA_DM_LINK_DOWN_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    UINT8           status;             /* connection open/closed */
} tBTA_DM_LINK_DOWN;

/* Structure associated with BTA_DM_SIG_STRENGTH_EVT */
typedef struct
{
    BD_ADDR         bd_addr;            /* BD address peer device. */
    tBTA_SIG_STRENGTH_MASK mask;        /* mask for the values that are valid */
    tBTA_DM_RSSI_VALUE  rssi_value;
    tBTA_DM_LINK_QUALITY_VALUE link_quality_value; 

} tBTA_DM_SIG_STRENGTH;

/* Structure associated with BTA_DM_BUSY_LEVEL_EVT */
typedef struct
{
    UINT8           level;     /* when paging or inquiring, level is 10.
                                    Otherwise, the number of ACL links */
} tBTA_DM_BUSY_LEVEL;

#define BTA_IO_CAP_OUT      BTM_IO_CAP_OUT      /* DisplayOnly */
#define BTA_IO_CAP_IO       BTM_IO_CAP_IO       /* DisplayYesNo */
#define BTA_IO_CAP_IN       BTM_IO_CAP_IN       /* KeyboardOnly */
#define BTA_IO_CAP_NONE     BTM_IO_CAP_NONE     /* NoInputNoOutput */
typedef tBTM_IO_CAP     tBTA_IO_CAP;

#define BTA_AUTH_SP_NO    BTM_AUTH_SP_NO      /* 0 MITM Protection Not Required - Single Profile/non-bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_SP_YES   BTM_AUTH_SP_YES     /* 1 MITM Protection Required - Single Profile/non-bonding
                                                Use IO Capabilities to determine authentication procedure */
#define BTA_AUTH_AP_NO    BTM_AUTH_AP_NO      /* 2 MITM Protection Not Required - All Profiles/dedicated bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_AP_YES   BTM_AUTH_AP_YES     /* 3 MITM Protection Required - All Profiles/dedicated bonding
                                                Use IO Capabilities to determine authentication procedure */
#define BTA_AUTH_SPGB_NO  BTM_AUTH_SPGB_NO    /* 4 MITM Protection Not Required - Single Profiles/general bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_SPGB_YES BTM_AUTH_SPGB_YES   /* 5 MITM Protection Required - Single Profiles/general bonding
                                                Use IO Capabilities to determine authentication procedure */
typedef tBTM_AUTH_REQ   tBTA_AUTH_REQ;

#define BTA_AUTH_DD_BOND    BTM_AUTH_DD_BOND  /* 2 this bit is set for dedicated bonding */
#define BTA_AUTH_GEN_BOND   BTM_AUTH_SPGB_NO  /* 4 this bit is set for general bonding */
#define BTA_AUTH_BONDS      BTM_AUTH_BONDS    /* 6 the general/dedicated bonding bits  */

#define BTA_OOB_NONE        BTM_OOB_NONE,
#define BTA_OOB_PRESENT     BTM_OOB_PRESENT
#if BTM_OOB_INCLUDED == TRUE
#define BTA_OOB_UNKNOWN     BTM_OOB_UNKNOWN
#endif
typedef tBTM_OOB_DATA   tBTA_OOB_DATA;           

/* Structure associated with BTA_DM_SP_CFM_REQ_EVT */
typedef struct
{
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
    UINT32          num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN         just_works;     /* TRUE, if "Just Works" association model */
} tBTA_DM_SP_CFM_REQ;

enum
{
    BTA_SP_KEY_STARTED,         /* passkey entry started */
    BTA_SP_KEY_ENTERED,         /* passkey digit entered */
    BTA_SP_KEY_ERASED,          /* passkey digit erased */
    BTA_SP_KEY_CLEARED,         /* passkey cleared */
    BTA_SP_KEY_COMPLT           /* passkey entry completed */
};
typedef UINT8   tBTA_SP_KEY_TYPE;

/* Structure associated with BTA_DM_SP_KEYPRESS_EVT */
typedef struct 
{
    BD_ADDR             bd_addr;        /* peer address */
    tBTA_SP_KEY_TYPE   notif_type;
}tBTA_DM_SP_KEY_PRESS;

/* Structure associated with BTA_DM_SP_KEY_NOTIF_EVT */
typedef struct
{
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
    UINT32          passkey;        /* the numeric value for comparison. If just_works, do not show this number to UI */
} tBTA_DM_SP_KEY_NOTIF;

/* Structure associated with BTA_DM_SP_RMT_OOB_EVT */
typedef struct
{
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
} tBTA_DM_SP_RMT_OOB;

/* Structure associated with BTA_DM_BOND_CANCEL_CMPL_EVT */
typedef struct
{
    tBTA_STATUS     result;    /* TRUE of bond cancel succeeded, FALSE if failed. */
} tBTA_DM_BOND_CANCEL_CMPL;

/* Union of all security callback structures */
typedef union
{
    tBTA_DM_ENABLE      enable;         /* BTA enabled */
    tBTA_DM_PIN_REQ     pin_req;        /* PIN request. */
    tBTA_DM_AUTH_CMPL   auth_cmpl;      /* Authentication complete indication. */
    tBTA_DM_AUTHORIZE   authorize;      /* Authorization request. */
    tBTA_DM_LINK_UP     link_up;       /* ACL connection down event */
    tBTA_DM_LINK_DOWN   link_down;       /* ACL connection down event */
    tBTA_DM_SIG_STRENGTH sig_strength;  /* rssi and link quality value */
    tBTA_DM_BUSY_LEVEL  busy_level;     /* System busy level */
    tBTA_DM_SP_CFM_REQ  cfm_req;        /* user confirm request */
    tBTA_DM_SP_KEY_NOTIF key_notif;     /* passkey notification */
    tBTA_DM_SP_RMT_OOB  rmt_oob;        /* remote oob */
    tBTA_DM_BOND_CANCEL_CMPL bond_cancel_cmpl; /* Bond Cancel Complete indication */
    tBTA_DM_SP_KEY_PRESS   key_press;   /* key press notification event */
} tBTA_DM_SEC;

/* Security callback */
typedef void (tBTA_DM_SEC_CBACK)(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data);

/* Search callback events */
#define BTA_DM_INQ_RES_EVT              0       /* Inquiry result for a peer device. */
#define BTA_DM_INQ_CMPL_EVT             1       /* Inquiry complete. */
#define BTA_DM_DISC_RES_EVT             2       /* Discovery result for a peer device. */
#define BTA_DM_DISC_CMPL_EVT            3       /* Discovery complete. */
#define BTA_DM_DI_DISC_CMPL_EVT         4       /* Discovery complete. */
#define BTA_DM_SEARCH_CANCEL_CMPL_EVT   5       /* Search cancelled */ 

typedef UINT8 tBTA_DM_SEARCH_EVT;

#define BTA_DM_INQ_RES_IGNORE_RSSI      BTM_INQ_RES_IGNORE_RSSI /* 0x7f RSSI value not supplied (ignore it) */

/* Structure associated with BTA_DM_INQ_RES_EVT */
typedef struct
{
    BD_ADDR         bd_addr;                /* BD address peer device. */
    DEV_CLASS       dev_class;              /* Device class of peer device. */
    BOOLEAN         remt_name_not_required; /* Application sets this flag if it already knows the name of the device */
                                            /* If the device name is known to application BTA skips the remote name request */
    BOOLEAN         is_limited;             /* TRUE, if the limited inquiry bit is set in the CoD */
    INT8            rssi;                   /* The rssi value */
    UINT8           *p_eir;                 /* received EIR */
} tBTA_DM_INQ_RES;

/* Structure associated with BTA_DM_INQ_CMPL_EVT */
typedef struct
{
    UINT8           num_resps;          /* Number of inquiry responses. */
} tBTA_DM_INQ_CMPL;

/* Structure associated with BTA_DM_DI_DISC_CMPL_EVT */
typedef struct
{
    BD_ADDR             bd_addr;        /* BD address peer device. */
    UINT8               num_record;     /* Number of DI record */
    tBTA_STATUS         result;
} tBTA_DM_DI_DISC_CMPL;

/* Structure associated with BTA_DM_DISC_RES_EVT */
typedef struct
{
    BD_ADDR             bd_addr;        /* BD address peer device. */
    BD_NAME             bd_name;        /* Name of peer device. */
    tBTA_SERVICE_MASK   services;       /* Services found on peer device. */
    tBTA_STATUS         result;
} tBTA_DM_DISC_RES;

/* Union of all search callback structures */
typedef union
{
    tBTA_DM_INQ_RES     inq_res;        /* Inquiry result for a peer device. */
    tBTA_DM_INQ_CMPL    inq_cmpl;       /* Inquiry complete. */
    tBTA_DM_DISC_RES    disc_res;       /* Discovery result for a peer device. */
    tBTA_DM_DI_DISC_CMPL    di_disc;        /* DI discovery result for a peer device */
} tBTA_DM_SEARCH;

/* Search callback */
typedef void (tBTA_DM_SEARCH_CBACK)(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);

/* Maximum service name length */
#define BTA_SERVICE_NAME_LEN    35

/* link policy masks  */
#define BTA_DM_LP_SWITCH        HCI_ENABLE_MASTER_SLAVE_SWITCH
#define BTA_DM_LP_HOLD          HCI_ENABLE_HOLD_MODE
#define BTA_DM_LP_SNIFF         HCI_ENABLE_SNIFF_MODE
#define BTA_DM_LP_PARK          HCI_ENABLE_PARK_MODE
typedef UINT16 tBTA_DM_LP_MASK;

/* power mode actions  */
#define BTA_DM_PM_NO_ACTION    0x00       /* no change to the current pm setting */
#define BTA_DM_PM_PARK         0x10       /* prefers park mode */
#define BTA_DM_PM_SNIFF        0x20       /* prefers sniff mode */
#define BTA_DM_PM_SNIFF1       0x21       /* prefers sniff1 mode */
#define BTA_DM_PM_SNIFF2       0x22       /* prefers sniff2 mode */
#define BTA_DM_PM_SNIFF3       0x23       /* prefers sniff3 mode */
#define BTA_DM_PM_SNIFF4       0x24       /* prefers sniff4 mode */
#define BTA_DM_PM_SNIFF5       0x25       /* prefers sniff5 mode */
#define BTA_DM_PM_SNIFF6       0x26       /* prefers sniff6 mode */
#define BTA_DM_PM_SNIFF7       0x27       /* prefers sniff7 mode */
#define BTA_DM_PM_ACTIVE       0x40       /* prefers active mode */
#define BTA_DM_PM_RETRY        0x80       /* retry power mode based on current settings */
#define BTA_DM_PM_NO_PREF      0x01       /* service has no prefernce on power mode setting. eg. connection to service got closed */

typedef UINT8 tBTA_DM_PM_ACTTION;

/* index to bta_dm_ssr_spec */
#define BTA_DM_PM_SSR0          0
#define BTA_DM_PM_SSR1          1
#define BTA_DM_PM_SSR2          2
#define BTA_DM_PM_SSR3          3
#define BTA_DM_PM_SSR4          4
#define BTA_DM_PM_SSR5          5
#define BTA_DM_PM_SSR6          6

#define BTA_DM_PM_NUM_EVTS  9

#ifndef BTA_DM_PM_PARK_IDX
#define BTA_DM_PM_PARK_IDX      5 /* the actual index to bta_dm_pm_md[] for PARK mode */
#endif

/* Dual Stack */
typedef tBTM_SW_DIR tBTA_DM_SW_DIR;
typedef UINT8 tBTA_DM_SWITCH_EVT;
typedef void (tBTA_DM_SWITCH_CBACK)(tBTA_DM_SWITCH_EVT event, tBTA_STATUS status);

#if (BTU_DUAL_STACK_INCLUDED == TRUE)
#define BTA_DM_SW_BB_TO_MM      BTM_SW_BB_TO_MM
#define BTA_DM_SW_MM_TO_BB      BTM_SW_MM_TO_BB

/* Switch callback events */
#define BTA_DM_SWITCH_CMPL_EVT      0       /* Completion of the Switch API */

#endif

/* Device Identification (DI) data structure
*/
/* Used to set the DI record */
typedef tSDP_DI_RECORD          tBTA_DI_RECORD;
/* Used to get the DI record */
typedef tSDP_DI_GET_RECORD      tBTA_DI_GET_RECORD;  
/* SDP discovery database */
typedef tSDP_DISCOVERY_DB       tBTA_DISCOVERY_DB;

#ifndef         BTA_DI_NUM_MAX
#define         BTA_DI_NUM_MAX       3
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         BTA_EnableBluetooth
**
** Description      This function initializes BTA and prepares BTA and the
**                  Bluetooth protocol stack for use.  This function is
**                  typically called at startup or when Bluetooth services
**                  are required by the phone.  This function must be called
**                  before calling any other API function.
**                  
**
** Returns          BTA_SUCCESS if successful.
**                  BTA_FAIL if internal failure.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_DisableBluetooth
**
** Description      This function disables BTA and the Bluetooth protocol
**                  stack.  It is called when BTA is no longer being used
**                  by any application in the system.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DisableBluetooth(void);

/*******************************************************************************
**
** Function         BTA_DmIsDeviceUp
**
** Description      This function tests whether the Bluetooth module is up
**                  and ready.  This is a direct execution function that
**                  may lock task scheduling on some platforms.
**                  
**
** Returns          TRUE if the module is ready.
**                  FALSE if the module is not ready. 
**
*******************************************************************************/
BTA_API extern BOOLEAN BTA_DmIsDeviceUp(void);

/*******************************************************************************
**
** Function         BTA_DmSetDeviceName
**
** Description      This function sets the Bluetooth name of the local device.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmSetDeviceName(char *p_name);

/*******************************************************************************
**
** Function         BTA_DmSetVisibility
**
** Description      This function sets the Bluetooth connectable and
**                  discoverable modes of the local device.  This controls
**                  whether other Bluetooth devices can find and connect to
**                  the local device.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmSetVisibility(tBTA_DM_DISC disc_mode, tBTA_DM_CONN conn_mode);

/*******************************************************************************
**
** Function         BTA_DmSearch
**
** Description      This function searches for peer Bluetooth devices.  It
**                  first performs an inquiry; for each device found from the
**                  inquiry it gets the remote name of the device.  If
**                  parameter services is nonzero, service discovery will be
**                  performed on each device for the services specified.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmSearch(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK services,
                                 tBTA_DM_SEARCH_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_DmSearchCancel
**
** Description      This function cancels a search that has been initiated
**                  by calling BTA_DmSearch().
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmSearchCancel(void);

/*******************************************************************************
**
** Function         BTA_DmDiscover
**
** Description      This function performs service discovery for the services
**                  of a particular peer device.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmDiscover(BD_ADDR bd_addr, tBTA_SERVICE_MASK services,
                                   tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search);

/*******************************************************************************
**
** Function         BTA_DmBond
**
** Description      This function initiates a bonding procedure with a peer
**                  device.  The bonding procedure enables authentication
**                  and optionally encryption on the Bluetooth link.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmBond(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_DmBondCancel
**
** Description      This function cancels a bonding procedure with a peer
**                  device.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmBondCancel(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_DmPinReply
**
** Description      This function provides a PIN when one is requested by DM
**                  during a bonding procedure.  The application should call
**                  this function after the security callback is called with
**                  a BTA_DM_PIN_REQ_EVT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmPinReply(BD_ADDR bd_addr, BOOLEAN accept, UINT8 pin_len,
                                   UINT8 *p_pin);

/*******************************************************************************
**
** Function         BTA_DmLinkPolicy
**
** Description      This function sets/clears the link policy mask to the given
**                  bd_addr.
**                  If clearing the sniff or park mode mask, the link is put
**                  in active mode.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmLinkPolicy(BD_ADDR bd_addr, tBTA_DM_LP_MASK policy_mask,
                                     BOOLEAN set);

#if (BTM_LISBON_INCLUDED == TRUE)
#if (BTM_OOB_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmLocalOob
**
** Description      This function retrieves the OOB data from local controller.
**                  The result is reported by bta_dm_co_loc_oob().
**
** Returns          void                  
**
*******************************************************************************/
BTA_API extern void BTA_DmLocalOob(void);
#endif /* BTM_OOB_INCLUDED */

/*******************************************************************************
**
** Function         BTA_DmConfirm
**
** Description      This function accepts or rejects the numerical value of the
**                  Simple Pairing process on BTA_DM_SP_CFM_REQ_EVT
**
** Returns          void                  
**
*******************************************************************************/
BTA_API extern void BTA_DmConfirm(BD_ADDR bd_addr, BOOLEAN accept);

/*******************************************************************************
**
** Function         BTA_DmPasskeyCancel
**
** Description      This function is called to cancel the simple pairing process
**                  reported by BTA_DM_SP_KEY_NOTIF_EVT
**
** Returns          void                  
**
*******************************************************************************/
BTA_API extern void BTA_DmPasskeyCancel(BD_ADDR bd_addr);

#endif /* BTM_LISBON_INCLUDED */

/*******************************************************************************
**
** Function         BTA_DmAddDevice
**
** Description      This function adds a device to the security database list
**                  of peer devices. This function would typically be called
**                  at system startup to initialize the security database with
**                  known peer devices.  This is a direct execution function
**                  that may lock task scheduling on some platforms.
**                  
**
** Returns          BTA_SUCCESS if successful.
**                  BTA_FAIL if operation failed.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_DmAddDevice(BD_ADDR bd_addr, LINK_KEY link_key,
                                           tBTA_SERVICE_MASK trusted_mask,
                                           BOOLEAN is_trusted, UINT8 key_type,
                                           tBTA_IO_CAP io_cap);

/*******************************************************************************
**
** Function         BTA_DmRemoveDevice
**
** Description      This function removes a device from the security database.
**                  This is a direct execution function that may lock task
**                  scheduling on some platforms.
**                  
**
** Returns          BTA_SUCCESS if successful.
**                  BTA_FAIL if operation failed.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_DmRemoveDevice(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_DmAuthorizeReply
**
** Description      This function provides an authorization reply when
**                  authorization is requested by BTA.  The application calls
**                  this function after the security callback is called with
**                  a BTA_DM_AUTHORIZE_EVT.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmAuthorizeReply(BD_ADDR bd_addr, tBTA_SERVICE_ID service,
                                         tBTA_AUTH_RESP response);

/*******************************************************************************
**
** Function         BTA_DmSignalStrength
**
** Description      This function initiates RSSI and channnel quality 
**                  measurments. BTA_DM_SIG_STRENGTH_EVT is sent to
**                  application with the values of RSSI and channel
**                  quality
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_DmSignalStrength(tBTA_SIG_STRENGTH_MASK mask, UINT16 period, BOOLEAN start);

/*******************************************************************************
**
** Function         BTA_CheckEirData
**
** Description      This function is called to get EIR data from significant part.
**
** Parameters       p_eir - pointer of EIR significant part
**                  type   - finding EIR data type
**                  p_length - return the length of EIR data
**
** Returns          pointer of EIR data 
**
*******************************************************************************/
BTA_API extern UINT8 *BTA_CheckEirData( UINT8 *p_eir, UINT8 tag, UINT8 *p_length );

/*******************************************************************************
**
** Function         BTA_GetEirService
**
** Description      This function is called to get BTA service mask from EIR.
**
** Parameters       p_eir - pointer of EIR significant part
**                  p_services - return the BTA service mask
**
** Returns          None 
**
*******************************************************************************/
BTA_API extern void BTA_GetEirService( UINT8 *p_eir, tBTA_SERVICE_MASK *p_services );

/*******************************************************************************
**
** Function         BTA_DmUseSsr
**
** Description      This function is called to check if the connected peer device
**                  supports SSR or not.
**
** Returns          TRUE, if SSR is supported
**
*******************************************************************************/
BTA_API extern BOOLEAN BTA_DmUseSsr( BD_ADDR bd_addr );


/*******************************************************************************
**
** Function         BTA_DmSetLocalDiRecord
**
** Description      This function adds a DI record to the local SDP database.
**
** Returns          BTA_SUCCESS if record set sucessfully, otherwise error code.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_DmSetLocalDiRecord( tBTA_DI_RECORD *p_device_info, 
	                          UINT32 *p_handle );

/*******************************************************************************
**
** Function         BTA_DmGetLocalDiRecord
**
** Description      Get a specified DI record to the local SDP database. If no  
**                  record handle is provided, the primary DI record will be 
**                  returned. 
**
** Returns          BTA_SUCCESS if record set sucessfully, otherwise error code.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_DmGetLocalDiRecord( tBTA_DI_GET_RECORD *p_device_info, 
	                          UINT32 *p_handle );

/*******************************************************************************
**
** Function         BTA_DmDiDiscover
**
** Description      This function queries a remote device for DI information.
**
** Returns          None.
**
*******************************************************************************/
BTA_API extern void BTA_DmDiDiscover( BD_ADDR remote_device, tBTA_DISCOVERY_DB *p_db,
                       UINT32 len, tBTA_DM_SEARCH_CBACK *p_cback );

/*******************************************************************************
**
** Function         BTA_DmGetDiRecord
**
** Description      This function retrieves a remote device's DI record from
**                  the specified database.
**
** Returns          None.
**
*******************************************************************************/
BTA_API extern tBTA_STATUS BTA_DmGetDiRecord( UINT8 get_record_index, tBTA_DI_GET_RECORD *p_device_info,
                        tBTA_DISCOVERY_DB *p_db );

BTA_API extern void btapp_dm_enable_dut(void);
#if (BTU_DUAL_STACK_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmSwitchStack
**
** Description      This function is called to switch from the full stack to the 
**                  light stack, or from the light stack to the full stack.  Upon
**                  completion of the switch, the BTA_DM_SWITCH_CMPL_EVT event is 
**                  returned containting the status of the operation.
**                  
**                  Note: The switch cannot be called while AV data streaming.
**
** Parameters       dir     - Direction of switch (BTA_DM_SW_BB_TO_MM or 
**                                                 BTA_DM_SW_MM_TO_BB)
**                  p_cback - (tBTA_DM_SWITCH_CBACK) callback called upon 
**                              completion of the operation
**
** Returns          None 
**
*******************************************************************************/
BTA_API extern void BTA_DmSwitchStack( tBTA_DM_SW_DIR dir, tBTA_DM_SWITCH_CBACK *p_cback );

#endif

#ifdef __cplusplus
}
#endif

#endif /* BTA_API_H */


#ifndef _AEE_BT_EXT_RM_H_
#define _AEE_BT_EXT_RM_H_

/*===========================================================================
FILE:      AEEBTExtRM.h

SERVICES:  BlueTooth Radio Manager Interface

GENERAL DESCRIPTION: BREW interface to the BT Radio Manager

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtRM

        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/inc/AEEBTExtRM.h#2 $
===========================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_RM       0x0104c9b0
#define AEECLSID_BLUETOOTH_RM_OLD   0x0101387b // prior to BT 2.1 features

// Some upper limits
#define AEEBT_MAX_NUM_OF_SRV_REC   20

#define AEEBT_MAX_NUM_UUID128         1
#define AEEBT_MAX_NUM_UUID32          1
#define AEEBT_MAX_NUM_UUID16          6
#define AEEBT_NUM_UUID_TYPES          3
#define AEEBT_MAX_EIR_MANUF_DATA_LEN 32

// Some defaults
#define AEEBT_DEFAULT_SNIFF_INTERVAL_MS   1280
#define AEEBT_DEFAULT_SNIFF_LATENCY_MS    1280
#define AEEBT_DEFAULT_SNIFF_IDLE_TO_MS    7000

#define AEEBT_IS_EIR_UUID_LIST_COMPLETE( bitmap, uuidType )  \
          (bitmap & (0x01 << uuidType))

// each of the LMP features are represented by a bit in the 8-byte mask
// there can be multiple pages of LMP features, starting with page 0
// position 0 is the least significant bit of byte 0
// position 8 is the least significant bit of byte 1
// position 16 is the least significant bit of byte 2
#define AEEBT_LMP_FEAT_3_SLOT_PKTS         0
#define AEEBT_LMP_FEAT_5_SLOT_PKTS         1
#define AEEBT_LMP_FEAT_ENCRYPT             2
#define AEEBT_LMP_FEAT_SLOT_OFFSET         3
#define AEEBT_LMP_FEAT_TIMING_ACCURACY     4
#define AEEBT_LMP_FEAT_SWITCH              5
#define AEEBT_LMP_FEAT_HOLD_MODE           6
#define AEEBT_LMP_FEAT_SNIFF_MODE          7
#define AEEBT_LMP_FEAT_PARK_MODE           8
#define AEEBT_LMP_FEAT_RSSI                9
#define AEEBT_LMP_FEAT_AUTO_DATA_RATE     10
#define AEEBT_LMP_FEAT_SCO_LINK           11
#define AEEBT_LMP_FEAT_HV2_PKTS           12
#define AEEBT_LMP_FEAT_HV3_PKTS           13
#define AEEBT_LMP_FEAT_U_LAW_LOG          14
#define AEEBT_LMP_FEAT_A_LAW_LOG          15
#define AEEBT_LMP_FEAT_CVSD               16
#define AEEBT_LMP_FEAT_PAGING_SCHEME      17
#define AEEBT_LMP_FEAT_POWER_CTRL         18
#define AEEBT_LMP_FEAT_TRANS_SCO_DATA     19
#define AEEBT_LMP_FEAT_FLOW_CTRL_LAG_LSB  20
#define AEEBT_LMP_FEAT_FLOW_CTRL_LAG_MID  21
#define AEEBT_LMP_FEAT_FLOW_CTRL_LAG_MSB  22
#define AEEBT_LMP_FEAT_BROADCAST_ENCRYPT  23
#define AEEBT_LMP_FEAT_EDR_ACL_2M_MODE    25
#define AEEBT_LMP_FEAT_EDR_ACL_3M_MODE    26
#define AEEBT_LMP_FEAT_ENH_INQ_SCAN       27
#define AEEBT_LMP_FEAT_INTRLCD_INQ_SCAN   28
#define AEEBT_LMP_FEAT_INTRLCD_PG_SCAN    29
#define AEEBT_LMP_FEAT_RSSI_WITH_INQ      30
#define AEEBT_LMP_FEAT_ESCO_LINK          31
#define AEEBT_LMP_FEAT_EV4_PKTS           32
#define AEEBT_LMP_FEAT_EV5_PKTS           33
#define AEEBT_LMP_FEAT_AFH_CAPABLE_SLAVE  35
#define AEEBT_LMP_FEAT_AFH_CLASS_SLAVE    36
#define AEEBT_LMP_FEAT_3S_EDR_ACL_PKTS    39
#define AEEBT_LMP_FEAT_5S_EDR_ACL_PKTS    40
#define AEEBT_LMP_FEAT_SNIFF_SUBRATING    41
#define AEEBT_LMP_FEAT_PAUSE_ENCRYPTION   42
#define AEEBT_LMP_FEAT_AFH_CAPABLE_MASTER 43
#define AEEBT_LMP_FEAT_AFH_CLASS_MASTER   44
#define AEEBT_LMP_FEAT_EDR_ESCO_2M_MODE   45
#define AEEBT_LMP_FEAT_EDR_ESCO_3M_MODE   46
#define AEEBT_LMP_FEAT_3S_EDR_ESCO_PKTS   47
#define AEEBT_LMP_FEAT_EIR                48
#define AEEBT_LMP_FEAT_SSP_CONTROLLER     51
#define AEEBT_LMP_FEAT_ENCAP_PDU          52
#define AEEBT_LMP_FEAT_ERR_DATA_REPORTING 53
#define AEEBT_LMP_FEAT_NON_FLUSH_PBF      54
#define AEEBT_LMP_FEAT_SUP_TO_CHGD_EV     56
#define AEEBT_LMP_FEAT_INQ_RSP_TX_POWER   57
#define AEEBT_LMP_FEAT_EXT_FEATURES       63

// extended LMP features - page 1
#define AEEBT_LMP_FEAT_SSP_HOST           64

#define AEEBT_LMP_FEATURE_SUPPORTED( FEATURE, feature_ptr )  \
  ( ( ( (uint8*) (feature_ptr) )[ FEATURE / 8 ] ) &       \
    ( 0x01 << ( FEATURE % 8 ) ) )

/* --------------------------------------------------------------------------*/
/* event types returned via HandleEvent() */
/* --------------------------------------------------------------------------*/
#define AEEBT_RM_EVT_LOCAL_NAME_SET          0  /* response to SetName() */
#define AEEBT_RM_EVT_LOCAL_COD_SET           1  /* response to SetClassOfDevice() */
#define AEEBT_RM_EVT_LOCAL_SECURITY_SET      2  /* response to SetSecurity() */
#define AEEBT_RM_EVT_PIN_REQUESTED           3  /* remote device initiated bonding */
#define AEEBT_RM_EVT_BONDED                  4  /* when bonding completed */
#define AEEBT_RM_EVT_UNBONDED                5  /* response to Unbond() */
#define AEEBT_RM_EVT_BONDABLE_MODE           6  /* response to SetBondable() */
#define AEEBT_RM_EVT_PIN_REPLIED             7  /* response to PinReply() */
#define AEEBT_RM_EVT_DEVICE_ADDED            8  /* response to DeviceAdd() */
#define AEEBT_RM_EVT_DEVICE_UPDATED          9  /* response to DeviceUpdate() */
#define AEEBT_RM_EVT_DEVICE_REMOVED          10 /* response to DeviceRemove() */
#define AEEBT_RM_EVT_LINK_STATUS_REG         11 /* response to RegisterLinkStatus() */
#define AEEBT_RM_EVT_LINK_STATUS_UNREG       12 /* response to UnregisterLinkStatus() */
#define AEEBT_RM_EVT_CONN_ROLE_SET           13 /* response to SetConnectionRole() */
#define AEEBT_RM_EVT_CONN_SECURITY_SET       14 /* response to SetConnectionSecurity() */
#define AEEBT_RM_EVT_DEVICE_SECURITY_SET     15 /* response to SetDeviceSecurity() */
#define AEEBT_RM_EVT_LINK_STATUS             16 /* status info of existing BT link */
#define AEEBT_RM_EVT_SERVICE_SECURITY_SET    17 /* response to SetServiceSecurity() */
#define AEEBT_RM_EVT_AUTHORIZE_REQUEST       18 /* response to AuthorizeReply() */
#define AEEBT_RM_EVT_AUTHORIZE_REPLIED       19 /* response to AuthorizeReply() */
#define AEEBT_RM_EVT_TEST_MODE_ENTERED       20 /* response to EnterTestMode() */
#define AEEBT_RM_EVT_TEST_MODE_EXITED        21 /* response to ExitTestMode() */
#define AEEBT_RM_EVT_APPLICATION_CONFIGURED  22 /* response to ConfigApplication() */
#define AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_ENABLED  23
/* response to AutoServiceSearchEnable() */
#define AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED 24
/* response to AutoServiceSearchDisable() */
#define AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED 25 /* response to DeviceServicesUpdate() */
#define AEEBT_RM_EVT_ROLE_SW_CTRL_SET        26 /* response to AllowRoleSwitch() */
#define AEEBT_RM_EVT_RADIO_STATUS            27 /* response to DisableRadio() */
#define AEEBT_RM_EVT_VISIBILITY_STATUS       28 /* response to DisableVisibility() */
#define AEEBT_RM_EVT_CONN                    29 /* when connected to a device */
#define AEEBT_RM_EVT_DISCONN                 30 /* when disconnected from a device */
#define AEEBT_RM_EVT_RADIO_ACTIVITY_REG      31 /* response to RegisterRadioActivity() */
#define AEEBT_RM_EVT_RADIO_ACTIVITY_UNREG    32 /* response to UnregisterRadioActivity() */

/* Extented Inquiry Response related events */
#define AEEBT_RM_EVT_NICK_NAME_SET           33 /* response to SetNickName() */
#define AEEBT_RM_EVT_MANUF_DATA_SET          34 /* response to SetManufData() */

/* Secure Simple Paring and Security Mode 4 related events */
#define AEEBT_RM_EVT_IO_CAP_SET              35 /* response to SetIOCap() */
#define AEEBT_RM_EVT_USER_CFM_REQUEST        36 /* when bonding using numeric comparison method */
#define AEEBT_RM_EVT_USER_CFM_REPLIED        37 /* response to UserConfirmationReply()*/
#define AEEBT_RM_EVT_PASSKEY_NOTIFICATION    38 /* when bonding using passkey method */
#define AEEBT_RM_EVT_PASSKEY_REQUEST         39 /* when bonding using passkey method*/
#define AEEBT_RM_EVT_PASSKEY_REPLIED         40 /* response to PasskeyReply() */
#define AEEBT_RM_EVT_KEYPRESS_NOTIFICATION   41 /* a key pressed on remote for bonding */
#define AEEBT_RM_EVT_OOB_REMOTE_DATA_REQUEST 43 /* when bonding using OOB */
#define AEEBT_RM_EVT_REBOND_REQUEST          44 /* remote requests rebond */
#define AEEBT_RM_EVT_KEYPRESS_NOTIFICATION_REPLIED   45 /* response to KeyPressNotification() */

#define AEEBT_RM_EVT_LPM_CONFIGURED          46 /* response to LowPowerConfig() */


/* --------------------------------------------------------------------------*/
/* error codes for AEEBT_RM_EVT_* */
/* --------------------------------------------------------------------------*/
#define AEEBT_RM_ERR_NONE             0   /* no worry */
#define AEEBT_RM_ERR_BAD_PARAM        1   /* for any function */
#define AEEBT_RM_ERR_BOND_NO_KEY      2   /* when Bond() failed due to no key */
#define AEEBT_RM_ERR_NO_DEVICE        3   /* for DeviceUpdate() or DeviceRemove() */
#define AEEBT_RM_ERR_DEVICE_EXISTS    4   /* for DeviceAdd() */
#define AEEBT_RM_ERR_DB_WRITE_FAILED  5   /* for any Device*() that changes DB */
#define AEEBT_RM_ERR_DB_FULL          6   /* for DeviceAdd() */
#define AEEBT_RM_ERR_MAX_CONN         7   /* max connections reached */
#define AEEBT_RM_ERR_NO_CONN          8   /* no connection */
#define AEEBT_RM_ERR_FAILED           9   /* general failure */
#define AEEBT_RM_ERR_BUSY             10  /* another operation in progress */
#define AEEBT_RM_ERR_SECURITY_NOT_MET 11  /* when Bond() failed due to wrong pin or pin missing */
#define AEEBT_RM_ERR_RADIO_OFF        12  /* for any function */
#define AEEBT_RM_ERR_CONN_FAIL        13  /* for any function */
#define AEEBT_RM_ERR_CMD_NOT_ALLOWED  14  /* for any connection control function */
#define AEEBT_RM_ERR_REMOTE_REJECTED  15  /* for any connection control function */

/* Radio Activity */
#define AEEBT_RA_RADIO_IDLE     0x0000  /* might be in a connection           */
#define AEEBT_RA_RADIO_OFF      0x0001  /* if set, other bits'd better not be */
#define AEEBT_RA_INQUIRING      0x0002  /* if set, other bits'd better not be */
#define AEEBT_RA_PAGING         0x0004  /* if set, other bits'd better not be */
#define AEEBT_RA_INQ_SCANNING   0x0008  /* this and/or ...                    */
#define AEEBT_RA_PAGE_SCANNING  0x0010  /* ... this can be set or cleared     */
typedef uint16    AEEBTRadioActivity;

#define AEEBT_RM_RADIO_OFF            0 
#define AEEBT_RM_RADIO_ON             1
typedef uint8     AEEBTRadioStatus;

/* Visibility status */
#define AEEBT_RM_VISIBILITY_OFF       0
#define AEEBT_RM_VISIBILITY_ON        1
typedef uint8     AEEBTVisibilityStatus;

/* Link key type */
#define AEEBT_LINK_KEY_TYPE_COMB      0
#define AEEBT_LINK_KEY_TYPE_LOCAL     1
#define AEEBT_LINK_KEY_TYPE_REMOTE    2
#define AEEBT_LINK_KEY_TYPE_NONE      3
#define AEEBT_LINK_KEY_TYPE_DEBUG     4
#define AEEBT_LINK_KEY_TYPE_UNAUTH    5
#define AEEBT_LINK_KEY_TYPE_AUTH      6
#define AEEBT_LINK_KEY_TYPE_CHANGED   7
typedef uint8     AEEBTLinkKeyType;

/* Link key Status */
#define AEEBT_LINK_KEY_STATUS_NONE           0
#define AEEBT_LINK_KEY_STATUS_GENERAL        1
#define AEEBT_LINK_KEY_STATUS_AUTH_GENERAL   2
#define AEEBT_LINK_KEY_STATUS_DEDICATED      3
#define AEEBT_LINK_KEY_STATUS_AUTH_DEDICATED 4
#define AEEBT_LINK_KEY_STATUS_DEBUG          5
#define AEEBT_LINK_KEY_STATUS_COMBINATION    6
#define AEEBT_LINK_KEY_STATUS_UNBONDED       7
#define AEEBT_LINK_KEY_STATUS_AUTH_UNBONDED  8
#define AEEBT_LINK_KEY_STATUS_UNIT           9
typedef uint8     AEEBTLinkKeyStatus;

/* security setting for individual remote device */
#define AEEBT_SEC_NONE                     0 /* no security */
#define AEEBT_SEC_AUTHENTICATE             1 /* authentication only */
#define AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT 2 /* authentication and encryption */
typedef uint8     AEEBTSecurity;

/* security requirement for rebonding */
#define AEEBT_REBOND_DONT_CARE    0 /* no MITM protection, non-bonding */
#define AEEBT_REBOND_BOND         1 /* no MITM protection, bonding */
#define AEEBT_REBOND_MITM         2 /* MITM protection, non-bonding */
#define AEEBT_REBOND_MITM_BOND    3 /* MITM protection, bonding */
typedef uint8 AEEBTRebondSecurity;


/* service ID method */
#define AEEBT_SIM_NONE            0
#define AEEBT_SIM_SDP_UUID        1 /* service is identified by the sdp_uuid field. */
#define AEEBT_SIM_RFCOMM_SCN      2 /* service is identified by the rfcomm_scn field. */
#define AEEBT_SIM_L2CAP_PSM       3 /* service is identified by the l2cap_psm field. */
#define AEEBT_SIM_DEFAULT_RFCOMM  4 /* for all services provided via BT SIO ports. */
#define AEEBT_SIM_DEFAULT_L2CAP   5 /* for all services provided via protocols over L2CAP. */
typedef uint8 AEEBTServiceIdMethod;


/* Link mode */
#define AEEBT_LM_ACTIVE           0
#define AEEBT_LM_HOLD             1
#define AEEBT_LM_SNIFF            2
#define AEEBT_LM_PARK             3
typedef uint8     AEEBTLinkMode;

/* Low Power Control type */
#define AEEBT_LPC_DONT_CARE       0
#define AEEBT_LPC_ACTIVE          1
#define AEEBT_LPC_SNIFF           2
#define AEEBT_LPC_PARK            3
#define AEEBT_LPC_HOLD            4
#define AEEBT_LPC_SNIFF_LOCK      5
typedef uint8     AEEBTLowPowerControl;

/* Device Update Bitmap - each bit represents a modifiable field in  */
/* a record in the device database */
#define AEEBT_RM_DU_UNBOND_B    0x0001
#define AEEBT_RM_DU_SECURITY_B  0x0002
#define AEEBT_RM_DU_NICK_NAME_B 0x0004
#define AEEBT_RM_DU_VALUE_1_B   0x0008  /* user defined */
#define AEEBT_RM_DU_VALUE_2_B   0x0010  /* user defined */
#define AEEBT_RM_DU_VALUE_3_B   0x0020  /* user defined */
#define AEEBT_RM_DU_VALUE_4_B   0x0040  /* user defined */
typedef uint16    AEEBTDeviceUpdateBitmap;

/* Device Update Status Bitmap - each bit represents a modified field */
/* in a record in the device database */
#define AEEBT_RM_DUS_UNBONDED_B   AEEBT_RM_DU_UNBOND_B
#define AEEBT_RM_DUS_SECURITY_B   AEEBT_RM_DU_SECURITY_B
#define AEEBT_RM_DUS_NICK_NAME_B  AEEBT_RM_DU_NICK_NAME_B
#define AEEBT_RM_DUS_VALUE_1_B    AEEBT_RM_DU_VALUE_1_B
#define AEEBT_RM_DUS_VALUE_2_B    AEEBT_RM_DU_VALUE_2_B
#define AEEBT_RM_DUS_VALUE_3_B    AEEBT_RM_DU_VALUE_3_B
#define AEEBT_RM_DUS_VALUE_4_B    AEEBT_RM_DU_VALUE_4_B
#define AEEBT_RM_DUS_ADDED_B      0x8000
#define AEEBT_RM_DUS_BONDED_B     0x4000
#define AEEBT_RM_DUS_LMP_INFO_B   0x2000
#define AEEBT_RM_DUS_NAME_B       0x1000
#define AEEBT_RM_DUS_SERVICES_B   0x0800
#define AEEBT_RM_DUS_REMOVED_B    0x0400
typedef uint16    AEEBTDeviceUpdateStatusBitmap;

#define AEEBT_RM_EC_ALL                  0 /* no filtering */
#define AEEBT_RM_EC_MATCH_SERVICE_CLASS  1 /* matching service class only */
#define AEEBT_RM_EC_MATCH_BONDED         2 /* matching bond status only */
#define AEEBT_RM_EC_MATCH_BD_ADDR_MASK   3 /* matching BD address mask only */
#define AEEBT_RM_EC_MATCH_VALUE_1        4 /* user defined */
#define AEEBT_RM_EC_MATCH_VALUE_2        5 /* user defined */
#define AEEBT_RM_EC_MATCH_VALUE_3        6 /* user defined */
#define AEEBT_RM_EC_MATCH_VALUE_4        7 /* user defined */
typedef uint8     AEEBTEnumCtrl;


#define AEEBT_RM_LU_SECURITY_B    0x0001
#define AEEBT_RM_LU_LINK_MODE_B   0x0002
#define AEEBT_RM_LU_ROLE_B        0x0004
#define AEEBT_RM_LU_ACL_PACKETS_B 0x0008
#define AEEBT_RM_LU_SCO_PACKETS_B 0x0010
#define AEEBT_RM_LU_RSSI_B        0x0020
#define AEEBT_RM_LU_ERROR_VALID_B 0x0040
typedef uint16    AEEBTLinkUpdateBitmap;


#define AEEBT_RM_AVP_AUTOMATIC                0
#define AEEBT_RM_AVP_SMALLEST_ACT_MOST_POWER  1
#define AEEBT_RM_AVP_LARGER_ACT_LESS_POWER    2
#define AEEBT_RM_AVP_LARGEST_ACT_LEAST_POWER  3
typedef uint8     AEEBTActVsPwr;


#define AEEBT_HCIM_OFF                  0
#define AEEBT_HCIM_STANDARD_HCI_ON      1 /*  Stand alone DSP image.  */
#define AEEBT_HCIM_STANDARD_HCI_VOC_ON  2 /*  BT/vocoder DSP image.   */
#define AEEBT_HCIM_APIT_HCI_ON          3 /*  BT/vocoder DSP image.   */
#define AEEBT_HCIM_APIT_HCI_SA_ON       4 /*  Stand alone DSP image.  */
#ifdef FEATURE_FACTORY_TESTMODE
#define AEEBT_HCIM_STANDARD_HCI_FTM     5 /*  HCI via FTM.            */
#endif
typedef uint8     AEEBTHCIMode;

/* security setting for individual service */
#define AEEBT_RM_SVC_SEC_NONE           0 /* no security */
#define AEEBT_RM_SVC_SEC_LOW            1 /* Low level security */
#define AEEBT_RM_SVC_SEC_MED            2 /* Medium security */
#define AEEBT_RM_SVC_SEC_HIGH           3 /* Highest security */
#define AEEBT_RM_SVC_SEC_SPECIAL        4 /* Special(MITM_NO_BONDING) security */
typedef uint8     AEEBTSvcSecLevel;

/* UUID type */
#define AEEBT_RM_UUID16                 0 /* 16-bit UUID */
#define AEEBT_RM_UUID32                 1 /* 32-bit UUID */
#define AEEBT_RM_UUID128                2 /* 128-bit UUID */
typedef uint8 AEEBTUUIDType;

/* Keypress type */
#define AEEBT_RM_KPN_STARTED            0
#define AEEBT_RM_KPN_DIGIT_ENTERED      1
#define AEEBT_RM_KPN_DIGIT_ERASED       2
#define AEEBT_RM_KPN_CLEARED            3
#define AEEBT_RM_KPN_COMPLETED          4

/* IO capability type */
#define AEEBT_RM_IOC_DISPLAY_YES_NO     0 /* default */
#define AEEBT_RM_IOC_DISPLAY_ONLY       1
#define AEEBT_RM_IOC_KEYBOARD_ONLY      2
#define AEEBT_RM_IOC_NO_IO              3
typedef uint8 AEEBTIOCapType;

/* EIR flag bits */
#define AEEBT_EIR_DATA_RCVD_B           0x01
#define AEEBT_EIR_NAME_CMPLT_B          0x02
#define AEEBT_EIR_UUID16_LIST_CMPLT_B   0x04
#define AEEBT_EIR_UUID32_LIST_CMPLT_B   0x08
#define AEEBT_EIR_UUID128_LIST_CMPLT_B  0x10

/* Simple service is service infomation retrieved during bonding process. */
/* Each device record can have up to AEEBT_MAX_NUM_OF_SRV_REC simple services. */
typedef struct tagAEEBTService
{
  uint16  uServiceClass;
  uint16  uProfileVersion;
  uint8   uServerChannel;
} AEEBTSimpleService;

/* service identifier used in SetServiceSecurity */
typedef struct tagAEEBTServiceIdentifier
{
  AEEBTServiceIdMethod  serviceIdMethod;  /* which of the following to use */
  union
  {
    uint16              uServiceClass;  /* SDP UUID */
    uint8               uServerChannel; /* RFCOMM server channel number */
    uint16              uPSM;           /* L2CAP Protocol Service Multiplexor  */
  } id;
} AEEBTServiceIdentifier;

typedef enum
{
   AEEBT_RM_INVALID,
   AEEBT_RM_INCOMING,
   AEEBT_RM_OUTGOING
}AEEBTRM_atzrq_type;


typedef struct _AEEBTAuthorizeReq
{
  AEEBTBDAddr            bdAddr;
  AEEBTServiceIdentifier serviceId;
  AEEBTRM_atzrq_type     authorize_dir;
} AEEBTAuthorizeReq;

typedef struct _AEEBTOOB
{
  uint8     hash[ 16 ];
  uint8     randomizer[ 16 ];
} AEEBTOOB;

/* Data structure to pass EIR data received from remote during device discovery */
typedef struct tagAEEBTEIRDataType
{
  uint8       uFlags;  // use with AEEBT_EIR_ bitmap
  uint8       uNumUUIDs[ AEEBT_NUM_UUID_TYPES ];
  uint8       deviceRanking;
  uint8       uManufDataSize;
} AEEBTEIRDataType;

/* infomation for remote devices */
typedef struct tagAEEBTDevice
{
  boolean                 bValid;
  boolean                 bBonded;
  AEEBTBDAddr             bdAddr;
  uint8                   uLMPFeatures[3];  /* lower 3 bytes of LMP features bitmap */
  uint8                   uLMPVersion;
  uint16                  uManufacturerID;
  uint16                  uLMPSubversion;
  AEEBTSecurity           security;
  AEEBTLinkKeyType        linkKeyType;
  AEEBTServiceClass       serviceClass;
  AEEBTMajorDeviceClass   majorDevClass;
  AEEBTMinorDeviceClass   minorDevClass;
  AECHAR                  wName[ AEEBT_MAX_DEVICENAME_LEN+1 ];
  AECHAR                  wNickName[ AEEBT_MAX_NICKNAME_LEN+1 ];
  uint16                  uValue1;
  uint16                  uValue2;
  uint16                  uValue3;
  uint16                  uValue4;

  AEEBTSimpleService      svcs[ AEEBT_MAX_NUM_OF_SRV_REC ];
  uint8                   uNumSvcs;

  /* for BT 2.1 features */
  uint8                   uLMPFeaturesHi[5];  /* higher 5 bytes of LMP feature bitmap */
  uint8                   uLMPFeaturesPg1[8]; /* 2nd page of LMP feature bitmap */
  AEEBTEIRDataType        EIRData;
  AEEBTLinkKeyStatus      linkKeyStatus;
  boolean                 bSSPCapable;

} AEEBTDeviceInfo;


/* information for the local device */
typedef struct
{
  AEEBTBDAddr           bdAddr;
  uint8                 uLMPVersion;
  uint16                uCompanyID;
  uint16                uLMPSubVersion;
  uint8                 uLMPFeatures[3];  /* bitmap */
  AEEBTServiceClass     svcCls;
  AEEBTMajorDeviceClass majorDevCls;
  AEEBTMinorDeviceClass minorDevCls;
  AEEBTSecurity         security;
  AECHAR                wName[ AEEBT_MAX_DEVICENAME_LEN+1 ];

  /* for BT 2.1 features */
  AECHAR                wShortName[ AEEBT_MAX_NICKNAME_LEN+1 ];
  uint8                 uLMPFeaturesHi[5];  /* higher 5 bytes of LMP feature bitmap */
  uint8                 uLMPFeaturesPg1[8]; /* 2nd page of LMP feature bitmap */
} AEEBTLocalInfo;

typedef struct
{
  AEEBTEnumCtrl       control;      /* criteria to match */
  AEEBTServiceClass   svcCls;       /* valid if control == SERVICE_CLASS */
  boolean             bBonded;      /* valid if control == BONDED */
  AEEBTBDAddr         bdAddrMask;   /* valid if control == BD_ADDR_MASK */
  AEEBTBDAddr         bdAddrValue;  /* valid if control == BD_ADDR_MASK */
  uint16              uValue1;      /* valid if control == VALUE_1 */
  uint16              uValue2;      /* valid if control == VALUE_2 */
  uint16              uValue3;      /* valid if control == VALUE_3 */
  uint16              uValue4;      /* valid if control == VALUE_4 */
} AEEBTDeviceEnumerator;

struct _AEEBTLinkStatus
{
  AEEBTBDAddr           bdAddr;       /* the other end of the link */
  AEEBTLinkUpdateBitmap updateBitmap;
  AEEBTSecurity         security;
  AEEBTLinkMode         linkMode;
  uint16                uCurrIntervalMs;
  boolean               bMaster;        /* TRUE if local device is master */
  uint16                ACLPktTypes;    /* ACL packet types */
  uint16                SCOPktTypes;    /* SCO packet types */
  int8                  rssi;
};

/* Device Update Status is passed to applications registered for RM  */
/* notifications whenever device DB is updated.  The updateBitmap is  */
/* invalid for BONDED, UNBONDED, DEVICE_ADDED, DEVICE_REMOVED,  */
/* DEVICE_SECURITY_SET, and DEVICE_SERVICES_UPDATED events.   */
/* It is valid only for DEVICE_UPDATED event. */
struct _AEEBTDeviceUpdateStatus
{
  AEEBTBDAddr                   bdAddr;       /* the other end of the link */
  AEEBTDeviceUpdateStatusBitmap updateBitmap;
  uint16                        error;
};

typedef struct _IBTExtRM IBTExtRM;

AEEINTERFACE(IBTExtRM)
{
   INHERIT_IQueryInterface(IBTExtRM);
   int (*GetLocalInfo)(IBTExtRM* po, AEEBTLocalInfo* pLocalInfo);
   int (*SetName)(IBTExtRM* po, const AECHAR* pName);
   int (*SetClassOfDevice)(IBTExtRM* po, AEEBTServiceClass svcCls,
                           AEEBTMajorDeviceClass majDevCls,
                           AEEBTMinorDeviceClass minDevCls);
   int (*SetSecurity)(IBTExtRM* po, AEEBTSecurity security);
   int (*Bond)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, const AECHAR* pPinCode);
   int (*Unbond)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr);
   int (*SetBondable)(IBTExtRM* po, boolean bBondable);
   int (*PinReply)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                   const AECHAR* pPinCode);
   int (*RegisterLinkStatus)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                             uint16 uRSSIDelta);
   int (*UnregisterLinkStatus)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr);
   int (*GetLinkStatus)(IBTExtRM* po, AEEBTLinkStatus* pLinkStatus);
   int (*SetConnectionRole)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                            boolean bMaster);
   int (*SetConnectionSecurity)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                                AEEBTSecurity security);
   int (*SetDeviceSecurity)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                            AEEBTSecurity security);
   int (*DeviceAdd)(IBTExtRM* po, const AEEBTDeviceInfo* pDevice);
   int (*DeviceUpdate)(IBTExtRM* po, AEEBTDeviceUpdateBitmap bm,
                       const AEEBTDeviceInfo* pDevice);
   int (*DeviceRead)(IBTExtRM* po, AEEBTDeviceInfo* pDevice);
   int (*DeviceEnumInit)(IBTExtRM* po, const AEEBTDeviceEnumerator *pEnum);
   int (*DeviceEnumNext)(IBTExtRM* po, AEEBTDeviceInfo* pDevice);
   int (*DeviceRemove)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr);

   int (*SetHCIMode)(IBTExtRM* po, AEEBTHCIMode HCIMode);
   int (*GetHCIMode)(IBTExtRM* po, AEEBTHCIMode* pHCIMode);
   int (*SetServiceSecurity)(IBTExtRM* po, 
                             const AEEBTServiceIdentifier* pServiceId,
                             AEEBTSecurity minSecurity,
                             boolean bAuthorize,
                             boolean bAuthorizeFirst);
   int (*AuthorizeReply)(IBTExtRM* po, const AEEBTServiceIdentifier* pServiceId,
                         const AEEBTBDAddr* pBDAddr, boolean bAuthorize);
   int (*EnterTestMode)(IBTExtRM* po);
   int (*ExitTestMode)(IBTExtRM* po);
   int (*ConfigApplication)(IBTExtRM* po, boolean bRemoveAppConfig, 
                            AEEBTActVsPwr pageScanActVsPwr,
                            AEEBTActVsPwr inqScanActVsPwr,
                            boolean bDeliverDevUpdates);
   int (*AutoServiceSearchEnable)(IBTExtRM* po);
   int (*AutoServiceSearchDisable)(IBTExtRM* po);
   int (*DeviceServicesUpdate)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                               uint16 numServices, 
                               const AEEBTSimpleService* pServices);
   int (*GetBTOnOffState)(IBTExtRM* po, boolean* btIsOn);
   int (*AllowRoleSwitch)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                          boolean bAllow);
   int (*DisableRadio)(IBTExtRM* po, boolean bDisable);
   int (*DisableVisibility)(IBTExtRM* po, boolean bDisable);
   int (*RegisterConnStatus)(IBTExtRM* po);
   int (*UnregisterConnStatus)(IBTExtRM* po);
   int (*RegisterRadioActivity)(IBTExtRM* po, AEEBTRadioActivity* pRadioAct,
                                AEECallback* pCb);
   int (*UnregisterRadioActivity)(IBTExtRM* po);
   int (*GetRadioActivity)(IBTExtRM* po, AEEBTRadioActivity* pRadioActivity);
   int (*GetConnectionRole)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                            boolean* pbMaster);
   int (*GetRoleSwitchPolicy)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                              boolean* pbAllow);
   /* EIR related APIs */
   int (*SetNickName)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                      const AECHAR* pwNickName);
   int (*SetEIRManufData)(IBTExtRM* po, const uint8* pManufData, uint8 uSize );
   int (*GetEIRUUIDList)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                         AEEBTUUIDType uuidType,
                         uint8  uuidListStartIdx,
                         uint8* puuidListSize,
                         void*  pUUIDList); // could be uint8*, uint16*, or AEEBTUUID128*
   int (*GetEIRManufData)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                          uint8* pSize, uint8* pManufData);

   /* SSP and SM4 related APIs */
   int (*SetIOCap) (IBTExtRM* po, AEEBTIOCapType IOCap); // next phase
   int (*SetSecBySvcCls)(IBTExtRM* po, uint16 uServiceClass,
                         AEEBTSvcSecLevel minSecurity,
                         boolean bAuthorize,
                         boolean bAuthorizeFirst);
   int (*SetSecBySrvrChnl)(IBTExtRM* po, uint8 uServerChannel,
                           AEEBTSvcSecLevel minSecurity,
                           boolean bAuthorize,
                           boolean bAuthorizeFirst);
   int (*SetSecByPSM)(IBTExtRM* po, uint16 uPSM,
                      AEEBTSvcSecLevel minSecurity,
                      boolean bAuthorize,
                      boolean bAuthorizeFirst);
   int (*SetRemoteOOBData)(IBTExtRM* po, const uint8* pOOBData, 
                           uint8 uSize);
   int (*UserConfirmationReply)(IBTExtRM* po, 
                                const AEEBTBDAddr* pBDAddr, 
                                boolean bConfirmed);
   int (*PasskeyReply)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                       const char szPasskey[ AEEBT_SSP_PASSKEY_LEN ]); // nice to have
   int (*CreateOOBData)(IBTExtRM* po, uint8* pOOBDataSize, AEECallback* pCb); 
   int (*ReadOOBData)(IBTExtRM* po, uint8* pOOBDataSize, uint8* pOOBData); 
   int (*AuthorizeRebond)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                          boolean bAuthorized, AEEBTRebondSecurity rebondSec);
   int (*KeypressNotification)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                               AEEBTKeypressType Keypress); // nice to have
   int (*SSPBond)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                  boolean bMITMProtectionRequired );
   int (*SetSM4DebugMode)(IBTExtRM* po, boolean bDebugMode );

   int (*LowPowerConfig)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                         AEEBTLowPowerControl lpc, uint16 uMaxIntervalMs,
                         uint16 uIdleTOMs, uint16 uMaxLatencyMs, 
                         boolean bMustUse, boolean bSniffInitiatorOriented);
   int (*LowPowerConfigTest)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                         AEEBTLowPowerControl lpc, uint16 uMaxIntervalMs,
                         uint16 uIdleTOMs, uint16 uMaxLatencyMs, 
                         boolean bMustUse, boolean bSniffInitiatorOriented, uint8 testId);
   int (*BondCancel)(IBTExtRM* po, const AEEBTBDAddr* pBDAddr);
};


#define IBTEXTRM_AddRef(p)                              \
        AEEGETPVTBL((p),IBTExtRM)->AddRef((p))

#define IBTEXTRM_Release(p)                             \
        AEEGETPVTBL((p),IBTExtRM)->Release((p))
        
#define IBTEXTRM_QueryInterface(p,i,ppo)                \
        AEEGETPVTBL((p),IBTExtRM)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTRM_ConfigApplication(p,r,pavp,iavp,d)     \
        AEEGETPVTBL((p),IBTExtRM)->ConfigApplication((p),(r),(pavp),(iavp),(d))
       
#define IBTEXTRM_AutoServiceSearchEnable(p)             \
        AEEGETPVTBL((p),IBTExtRM)->AutoServiceSearchEnable((p))

#define IBTEXTRM_AutoServiceSearchDisable(p)            \
        AEEGETPVTBL((p),IBTExtRM)->AutoServiceSearchDisable((p))

#define IBTEXTRM_GetLocalInfo(p,pi)                     \
        AEEGETPVTBL((p),IBTExtRM)->GetLocalInfo((p),(pi))
        
#define IBTEXTRM_SetName(p,pn)                          \
        AEEGETPVTBL((p),IBTExtRM)->SetName((p),(pn))
        
#define IBTEXTRM_SetClassOfDevice(p,sc,mj,mn)           \
        AEEGETPVTBL((p),IBTExtRM)->SetClassOfDevice((p),(sc),(mj),(mn))
        
#define IBTEXTRM_SetSecurity(p,s)                       \
        AEEGETPVTBL((p),IBTExtRM)->SetSecurity((p),(s))
        
#define IBTEXTRM_Bond(p,pa,pc)                          \
        AEEGETPVTBL((p),IBTExtRM)->Bond((p),(pa),(pc))
        
#define IBTEXTRM_Unbond(p,pa)                           \
        AEEGETPVTBL((p),IBTExtRM)->Unbond((p),(pa))
        
#define IBTEXTRM_SetBondable(p,b)                       \
        AEEGETPVTBL((p),IBTExtRM)->SetBondable((p),(b))
        
#define IBTEXTRM_PinReply(p,pa,pc)                      \
        AEEGETPVTBL((p),IBTExtRM)->PinReply((p),(pa),(pc))
        
#define IBTEXTRM_RegisterLinkStatus(p,pa,r)             \
        AEEGETPVTBL((p),IBTExtRM)->RegisterLinkStatus((p),(pa),(r))
        
#define IBTEXTRM_UnregisterLinkStatus(p,pa)             \
        AEEGETPVTBL((p),IBTExtRM)->UnregisterLinkStatus((p),(pa))
        
#define IBTEXTRM_GetLinkStatus(p,ps)                    \
        AEEGETPVTBL((p),IBTExtRM)->GetLinkStatus((p),(ps))
        
#define IBTEXTRM_GetConnectionRole(p,pa,pb)             \
        AEEGETPVTBL((p),IBTExtRM)->GetConnectionRole((p),(pa),(pb))
        
#define IBTEXTRM_SetConnectionRole(p,pa,b)              \
        AEEGETPVTBL((p),IBTExtRM)->SetConnectionRole((p),(pa),(b))
        
#define IBTEXTRM_SetConnectionSecurity(p,pa,s)          \
        AEEGETPVTBL((p),IBTExtRM)->SetConnectionSecurity((p),(pa),(s))
        
#define IBTEXTRM_SetDeviceSecurity(p,pa,s)              \
        AEEGETPVTBL((p),IBTExtRM)->SetDeviceSecurity((p),(pa),(s))
        
#define IBTEXTRM_DeviceAdd(p,pd)                        \
        AEEGETPVTBL((p),IBTExtRM)->DeviceAdd((p),(pd))
        
#define IBTEXTRM_DeviceUpdate(p,bm,pd)                  \
        AEEGETPVTBL((p),IBTExtRM)->DeviceUpdate((p),(bm),(pd))
        
#define IBTEXTRM_DeviceServicesUpdate(p,pa,n,ps)        \
        AEEGETPVTBL((p),IBTExtRM)->DeviceServicesUpdate((p),(pa),(n),(ps))
        
#define IBTEXTRM_DeviceRead(p,pd)                       \
        AEEGETPVTBL((p),IBTExtRM)->DeviceRead((p),(pd))
        
#define IBTEXTRM_DeviceEnumInit(p,pe)                   \
        AEEGETPVTBL((p),IBTExtRM)->DeviceEnumInit((p),(pe))
        
#define IBTEXTRM_DeviceEnumNext(p,po)                   \
        AEEGETPVTBL((p),IBTExtRM)->DeviceEnumNext((p),(po))
        
#define IBTEXTRM_DeviceRemove(p,pa)                     \
        AEEGETPVTBL((p),IBTExtRM)->DeviceRemove((p),(pa))
        
#define IBTEXTRM_SetHCIMode(p,m)                        \
        AEEGETPVTBL((p),IBTExtRM)->SetHCIMode((p),(m))
        
#define IBTEXTRM_GetHCIMode(p,pm)                       \
        AEEGETPVTBL((p),IBTExtRM)->GetHCIMode((p),(pm))

#define IBTEXTRM_SetServiceSecurity(p,si,ms,b1,b2)      \
        AEEGETPVTBL((p),IBTExtRM)->SetServiceSecurity((p),(si),(ms),(b1),(b2))

#define IBTEXTRM_AuthorizeReply(p,ps,pa,b)              \
        AEEGETPVTBL((p),IBTExtRM)->AuthorizeReply((p),(ps),(pa),(b))

#define IBTEXTRM_EnterTestMode(p)                       \
        AEEGETPVTBL((p),IBTExtRM)->EnterTestMode((p))

#define IBTEXTRM_ExitTestMode(p)                        \
        AEEGETPVTBL((p),IBTExtRM)->ExitTestMode((p))
        
#define IBTEXTRM_GetBTOnOffState(p,pb)                  \
        AEEGETPVTBL((p),IBTExtRM)->GetBTOnOffState((p),(pb))

#define IBTEXTRM_GetRoleSwitchPolicy(p,pa,pb)           \
        AEEGETPVTBL((p),IBTExtRM)->GetRoleSwitchPolicy((p),(pa),(pb))
        
#define IBTEXTRM_AllowRoleSwitch(p,a,b)                 \
        AEEGETPVTBL((p),IBTExtRM)->AllowRoleSwitch((p),(a),(b))

#define IBTEXTRM_DisableRadio(p,b)                      \
        AEEGETPVTBL((p),IBTExtRM)->DisableRadio((p),(b))

#define IBTEXTRM_DisableVisibility(p,b)                 \
        AEEGETPVTBL((p),IBTExtRM)->DisableVisibility((p),(b))
        
#define IBTEXTRM_RegisterConnStatus(p)                  \
        AEEGETPVTBL((p),IBTExtRM)->RegisterConnStatus((p))
        
#define IBTEXTRM_UnregisterConnStatus(p)                \
        AEEGETPVTBL((p),IBTExtRM)->UnregisterConnStatus((p))
        
#define IBTEXTRM_RegisterRadioActivity(p,pra,pcb)       \
        AEEGETPVTBL((p),IBTExtRM)->RegisterRadioActivity((p),(pra),(pcb))
        
#define IBTEXTRM_UnregisterRadioActivity(p)             \
        AEEGETPVTBL((p),IBTExtRM)->UnregisterRadioActivity((p))
        
#define IBTEXTRM_GetRadioActivity(p,pra)                \
        AEEGETPVTBL((p),IBTExtRM)->GetRadioActivity((p),(pra))

#define IBTEXTRM_SetNickName(p,a,pn)                    \
        AEEGETPVTBL((p),IBTExtRM)->SetNickName((p),(a),(pn))

#define IBTEXTRM_SetEIRManufData(p,pn,s)                \
        AEEGETPVTBL((p),IBTExtRM)->SetEIRManufData((p),(pn),(s))

#define IBTEXTRM_GetEIRUUIDList(p,pa,n,x,pm,pn)         \
        AEEGETPVTBL((p),IBTExtRM)->GetEIRUUIDList((p),(pa),(n),(x),(pm),(pn))

#define IBTEXTRM_GetEIRManufData(p,pa,pn,ps)            \
        AEEGETPVTBL((p),IBTExtRM)->GetEIRManufData((p),(pa),(pn),(ps))

#define IBTEXTRM_SetIOCap(p,i)                          \
        AEEGETPVTBL((p),IBTExtRM)->SetIOCap((p),(i))

#define IBTEXTRM_SetSecBySvcCls(p,s,ms,b1,b2)           \
        AEEGETPVTBL((p),IBTExtRM)->SetSecBySvcCls((p),(s),(ms),(b1),(b2))

#define IBTEXTRM_SetSecBySrvrChnl(p,sc,ms,b1,b2)        \
        AEEGETPVTBL((p),IBTExtRM)->SetSecBySrvrChnl((p),(sc),(ms),(b1),(b2))

#define IBTEXTRM_SetSecByPSM(p,psm,ms,b1,b2)            \
        AEEGETPVTBL((p),IBTExtRM)->SetSecByPSM((p),(psm),(ms),(b1),(b2))

#define IBTEXTRM_SetRemoteOOBData(p,po,s)                 \
        AEEGETPVTBL((p),IBTExtRM)->SetRemoteOOBData((p),(po),(s))

#define IBTEXTRM_UserConfirmationReply(p,pa,b)          \
        AEEGETPVTBL((p),IBTExtRM)->UserConfirmationReply((p),(pa),(b))

#define IBTEXTRM_PasskeyReply(p,pa,pk)                  \
        AEEGETPVTBL((p),IBTExtRM)->PasskeyReply((p),(pa),(pk))

#define IBTEXTRM_CreateOOBData(p,pn,pcb)                  \
        AEEGETPVTBL((p),IBTExtRM)->CreateOOBData((p),(pn),(pcb))

#define IBTEXTRM_ReadOOBData(p,pn,ps)                  \
        AEEGETPVTBL((p),IBTExtRM)->ReadOOBData((p),(pn),(ps))

#define IBTEXTRM_AuthorizeRebond(p,pa,b,s)              \
        AEEGETPVTBL((p),IBTExtRM)->AuthorizeRebond((p),(pa),(b),(s))

#define IBTEXTRM_KeypressNotification(p,pa,kp)          \
        AEEGETPVTBL((p),IBTExtRM)->KeypressNotification((p),(pa),(kp))

#define IBTEXTRM_SSPBond(p,pa,b1)                       \
        AEEGETPVTBL((p),IBTExtRM)->SSPBond((p),(pa),(b1))

#define IBTEXTRM_SetSM4DebugMode(p,b1)                  \
        AEEGETPVTBL((p),IBTExtRM)->SetSM4DebugMode((p),(b1))

#define IBTEXTRM_LowPowerConfig(p,pa,c,mi,it,ml,b1,b2) \
        AEEGETPVTBL((p),IBTExtRM)->LowPowerConfig((p),(pa),(c),(mi),(it),(ml), \
                                                  (b1),(b2))
#define IBTEXTRM_LowPowerConfigTest(p,pa,c,mi,it,ml,b1,b2,ti) \
        AEEGETPVTBL((p),IBTExtRM)->LowPowerConfigTest((p),(pa),(c),(mi),(it),(ml), \
                                                  (b1),(b2),(ti))
#define IBTEXTRM_BondCancel(p,b1)                       \
        AEEGETPVTBL((p),IBTExtRM)->BondCancel((p),(b1))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtRM

Description: BlueTooth Radio Manager

  Consult the Bluetooth ISOD for description of Radio Manager.
  
  The use of IBTExtRM consists of sending commands and receiving events.  
  These events are delivered to IBTExtRM clients as notifications, hence
  the clients must register for RM notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_RM.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as a pin request
  sent by remote device.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_RM_EVT_* events when the notification mask
  is NMASK_BT_RM.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  
  Event                                      pData->
  -----                                      ----------------------------
  AEEBT_RM_EVT_APPLICATION_CONFIGURED        uError
  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_ENABLED   uError
  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED  uError
  AEEBT_RM_EVT_PIN_REQUESTED                 bdAddr - BD addr of remote device
  AEEBT_RM_EVT_PIN_REPLIED                   uError
  AEEBT_RM_EVT_BONDED                        pDevUpdateStatus
  AEEBT_RM_EVT_UNBONDED                      pDevUpdateStatus
  AEEBT_RM_EVT_DEVICE_ADDED                  pDevUpdateStatus
  AEEBT_RM_EVT_DEVICE_UPDATED                pDevUpdateStatus
  AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED       pDevUpdateStatus
  AEEBT_RM_EVT_DEVICE_REMOVED                pDevUpdateStatus
  AEEBT_RM_EVT_DEVICE_SECURITY_SET           pDevUpdateStatus
  AEEBT_RM_EVT_LINK_STATUS                   pLinkStatus
  AEEBT_RM_EVT_SERVICE_SECURITY_SET          uError
  AEEBT_RM_EVT_AUTHORIZE_REQUEST             pAuthorizeReq
  AEEBT_RM_EVT_AUTHORIZE_REPLIED             uError
  AEEBT_RM_EVT_TEST_MODE_ENTERED             uError
  AEEBT_RM_EVT_TEST_MODE_EXITED              uError
  AEEBT_RM_EVT_LOCAL_SHORT_NAME_SET          uError
  AEEBT_RM_EVT_MANUF_DATA_SET                uError
  AEEBT_RM_EVT_USER_CFM_REQUEST              sPasskey
  AEEBT_RM_EVT_USER_CFM_REPLIED              uError
  AEEBT_RM_EVT_PASSKEY_NOTIFICATION          sPasskey
  AEEBT_RM_EVT_PASSKEY_REQUEST               bdAddr
  AEEBT_RM_EVT_PASSKEY_REPLIED               uError
  AEEBT_RM_EVT_KEYPRESS_NOTIFICATION         keypressType
  AEEBT_RM_EVT_OOB_REMOTE_DATA_REQUEST       bdAddr
  AEEBT_RM_EVT_REBOND_REQUEST                bdAddr

Interfaces:

  IBTEXTRM provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTRM_AddRef()
    IBTEXTRM_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTRM_QueryInterface()

=======================================================================

Function:  IBTEXTRM_ConfigApplication()

Description: Sets application specific Radio Manager settings of the local 
             device.

Prototype:

  int IBTEXTRM_ConfigApplication( IBTExtRM*      po,
                                  boolean        bRemoveAppConfig,
                                  AEEBTActVsPwr  pageScanActVsPwr,
                                  AEEBTActVsPwr  inqScanActVsPwr,
                                  boolean        bDeliverDevUpdates
 );

Parameters:

  po                 - pointer to the IBTExtRM object
  bRemoveAppConfig   - TRUE to remove previous application configuration 
                       information from the RM; FALSE to retain configurattion
  pageScanActVsPwr   - tradeoff scheme used when in page scan mode 
  inqScanActVsPwr    - tradeoff scheme used when in inquiry scan mode
  bDeliverDevUpdates - FALSE to disable delivery of unsolicited 
                       AEEBT_RM_EVT_DEVICE_UPDATED events (i.e. ones that are
                       not caused by actions initiated by the application 
                       specifically); TRUE to deliver all device updated events
                       when RM device database is updated for any reason

Return value:

  SUCCESS indicates that the configuration request was queued and that
  AEEBT_RM_EVT_APPLICATION_CONFIGURED event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
    
    EFAILED   - invalid pointer to the IBTExtRM object
    ENOMEMORY - out of memory
  
Event:

  AEEBT_RM_EVT_APPLICATION_CONFIGURED with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_FAILED   - failed to configure application, no specific reason
    
=======================================================================

Function:  IBTEXTRM_AutoServiceSearchEnable()

Description: Enables automatic service search after bonding to a remote BT 
             device. When enabled, an SDP service search request will be sent 
             to the bonded device immediately after bonding is completed.

Prototype:

  int IBTEXTRM_AutoServiceSearchEnable( IBTExtRM*  po );

Parameters:

  po          - pointer to the IBTExtRM object

Return value:

  SUCCESS indicates that the enable command was queued and that
  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_ENABLED event will be generated at a later 
  time. All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
  
    EFAILED   - invalid pointer to the IBTExtRM object
    ENOMEMORY - out of memory
    EALREADY  - the automatic service search setting is in the process of being
                updated (by a previous call to this function or the 
                IBTEXTRM_AutoServiceSearchDisable() function).
                
Event:

  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_ENABLED with accompanying error code of one 
  of the following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_FAILED   - failed to enable automatic service search, no 
                            specific reason 

=======================================================================

Function:  IBTEXTRM_AutoServiceSearchDisable()

Description: Disables automatic service search after bonding to a remote BT 
             device. When disabled, no SDP service search request will be sent 
             to the bonded device after bonding is completed.

Prototype:

  int IBTEXTRM_AutoServiceSearchDisable( IBTExtRM*  po );

Parameters:

  po          - pointer to the IBTExtRM object

Return value:

  SUCCESS indicates that the enable command was queued and that
  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED event will be generated at a later 
  time. All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
  
    EFAILED   - invalid pointer to the IBTExtRM object
    ENOMEMORY - out of memory
    EALREADY  - the automatic service search setting is in the process of being
                updated (by a previous call to this function or the 
                IBTEXTRM_AutoServiceSearchDisable() function).
                
Event:

  AEEBT_RM_EVT_AUTO_SERVICE_SEARCH_DISABLED with accompanying error code of one 
  of the following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_FAILED   - failed to disable automatic service search, no 
                            specific reason
                            
=======================================================================

Function:  IBTEXTRM_GetLocalInfo()

Description: Gets the Bluetooth information of the local device.

Prototype:

  int IBTEXTRM_GetLocalInfo( IBTExtRM*         po,
                             AEEBTLocalInfo*   pInfo );

Parameters:

  po           - pointer to the IBTExtRM object
  pInfo        - pointer to memory location to store local info

Return value:

  SUCCESS indicates that the local info has been copied to pInfo.  No events
  will be generated for this command.  In this context, the following error 
  codes have special meaning:
  
    EBADPARM  - invalid input pointers

=======================================================================

Function:  IBTEXTRM_SetName()

Description: Sets new Bluetooth name for the local device.

Prototype:

  int IBTEXTRM_SetName( IBTExtRM* po, const AECHAR* pName );

Parameters:

  po           - pointer to the IBTExtRM object
  pName        - pointer to the string to be used as local device's name

Return value:

  SUCCESS indicates that the request to set BT name was queued and that
  AEEBT_RM_EVT_LOCAL_NAME_SET event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
  
    ENOMEMORY - out of memory
    EBADPARM  - invalid input pointers

Event:
  AEEBT_RM_EVT_LOCAL_NAME_SET with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error

=======================================================================

Function:  IBTEXTRM_SetClassOfDevice()

Description: Sets new class of device for the local device using the 
             input bitmaps.

Prototype:

  int IBTEXTRM_SetClassOfDevice( IBTExtRM* po, 
                                 AEEBTServiceClass     svcCls,
                                 AEEBTMajorDeviceClass mjrDevCls,
                                 AEEBTMinorDeviceClass mnrDevCls );

Parameters:

  po          - pointer to the IBTEXTRM object
  svcCls      - new service class bitmap
  mjrDevCls   - new major device class bitmap
  mnrDevCls   - new minor device class bitmap

Return value:

  SUCCESS indicates that the request to set the class of device was queued and
  that AEEBT_RM_EVT_LOCAL_COD_SET event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:
  
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_LOCAL_COD_SET with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error

=======================================================================

Function:  IBTEXTRM_SetSecurity()

Description: Sets security level for the local device.

Prototype:

  int IBTEXTRM_SetSecurity( IBTExtRM* po, AEEBTSecurity security );

Parameters:

  po           - pointer to the IBTExtRM object
  security     - the new security level for local device

Return value:

  SUCCESS indicates that the request to set security level was queued and
  that AEEBT_RM_EVT_LOCAL_SECURITY_SET event will be generated at a later 
  time.  All other values indicate failure, and will not generate an event.
  In this context, the following errorcodes have special meaning:
  
    ENOMEMORY - out of memory
    EBADPARM  - invalid input pointers

Event:
  AEEBT_RM_EVT_LOCAL_SECURITY_SET with accompanying error code of one of the
  following values:
  
  AEEBT_RM_ERR_NONE             - no error
  AEEBT_RM_ERR_DB_WRITE_FAILED  - failed to write new setting to file

=======================================================================

Function:  IBTEXTRM_Bond()

Description: Performs Bluetooth bonding (pairing) with a remote device.

Prototype:

  int IBTEXTRM_Bond( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                     const AECHAR* pPinCode );

Parameters:

   po           - pointer to the IBTExtRM object
   pBDAddr      - pointer to the BT device address of the remote device
   pPinCode     - pointer to the PIN code used for bonding

Return value:

  SUCCESS indicates that the request to bond with the remote device was queued 
  and that AEEBT_RM_EVT_BONDED event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_BONDED with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error
    AEEBT_RM_ERR_BOND_NO_KEY      - failed to bond due to no PIN code
    AEEBT_RM_ERR_BOND_FAILED      - failed to bond, reason unknown

=======================================================================

Function:  IBTEXTRM_Unbond()

Description: Unbonds a remote device.

Prototype:

  int IBTEXTRM_Unbond( IBTExtRM* po, const AEEBTBDAddr* pBDAddr );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device

Return value:

  SUCCESS indicates that the request to bond with the remote device was queued 
  and that AEEBT_RM_EVT_UNBONDED event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:

   EBADPARM  - invalid input pointers
   ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_UNBONDED with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error
    AEEBT_RM_ERR_NO_DEVICE        - device not found in device DB
    AEEBT_RM_ERR_DB_WRITE_FAILED  - error occurred writing DB to flash

=======================================================================

Function:  IBTEXTRM_SetBondable()

Description: Enables/disables local device's bondable mode.  This allows remote
             devices to initiate bonding with the local device.

Prototype:

  int IBTEXTRM_SetBondable( IBTExtRM* po, boolean bBondable );

Parameters:

  po           - pointer to the IBTExtRM object
  bBondable    - TRUE to enable, FALSE disable bondable mode

Return value:

  SUCCESS indicates that the request to enable/disable local device's bondable 
  mode was queued and that AEEBT_RM_EVT_BONDABLE_MODE event will be generated 
  at a later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_BONDABLE_MODE with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_FAILED   - failed to set bondable mode, no specific reason

=======================================================================

Function:  IBTEXTRM_GetConnectionRole()

Description: Gets the local role (master or slave) for an existing connection
             to a given BD address, or the role local device will attempt to be
             for future connection initiated by a remote device if no BD 
             address is given.

Prototype:

  int IBTEXTRM_GetConnectionRole( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                  boolean* pbMaster );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device; NULL to indicate local device
  pbMaster    - destination for the connection role value

Return value:

  SUCCESS indicates that the connection role has been copied to pbMaster.
  No events will be generated for this command.  In this context, the 
  following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOSUCH   - no connection for given BD address

=======================================================================

Function:  IBTEXTRM_SetConnectionRole()

Description: Sets the role (master or slave) for the existing connection.

Prototype:

  int IBTEXTRM_SetConnectionRole( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                  boolean bMaster );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device; NULL to indicate local device
  bMaster     - TRUE to make the local device the master of the connection,
                FALSE the slave

Return value:

  SUCCESS indicates that the request to switch role with the remote device 
  was queued and that AEEBT_RM_EVT_CONN_ROLE_SET event will be generated at 
  a later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_CONN_ROLE_SET with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_NO_CONN  - not connected to remote device
    AEEBT_RM_ERR_FAILED   - connection role not set, no specific reason

=======================================================================

Function:  IBTEXTRM_SetConnectionSecurity()

Description: Sets the security level for the existing connection.

Prototype:

  int IBTEXTRM_SetConnectionSecurity( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                      AEEBTSecurity security );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device
  security    - new security level for this connection

Return value:

  SUCCESS indicates that the request to change security level for the
  current connection with the remote device was queued and that 
  AEEBT_RM_EVT_CONN_SECURITY_SET event will be generated at a later time.
  All other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_CONN_SECURITY_SET with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_NO_CONN  - not connected to remote device
    AEEBT_RM_ERR_FAILED   - connection security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_SetDeviceSecurity()

Description: Sets the security level for a given remote device found in
             RM device DB.

Prototype:

  int IBTEXTRM_SetDeviceSecurity( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                  AEEBTSecurity security );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device
  security    - new security level for this device

Return value:

  SUCCESS indicates that the request to change security level for the remote
  device was queued and that AEEBT_RM_EVT_DEVICE_SECURITY_SET event will be 
  generated at a later time.  All other values indicate failure, and will 
  not generate an event.  In this context, the following error codes have 
  special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_DEVICE_SECURITY_SET with accompanying Device Update Status
  whose Error field is of one of the following error codes:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_NO_DEVICE  - remote device not in RM device DB
    AEEBT_RM_ERR_FAILED     - device security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_PinReply()

Description: Sends the PIN Code reply.  This should be called in response 
             to a PIN_REQUEST event.

Prototype:

  int IBTEXTRM_PinReply( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                         const AECHAR* pPinCode );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device
  pPinCode     - pointer to the PIN code used for bonding; could be NULL

Return value:

  SUCCESS indicates that the request to send the PIN code to remote device was
  queued and that AEEBT_RM_EVT_PIN_REPLIED event will be generated at a 
  later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_PIN_REPLIED with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE    - no error
    AEEBT_RM_ERR_FAILED  - pin reply failed
    
=======================================================================

Function:  IBTEXTRM_RegisterLinkStatus()

Description: Registers for Link Status update for an existing connection.

Prototype:

  int IBTEXTRM_RegisterLinkStatus( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                                   uint16 uRSSIDelta );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device
  uRSSIDelta   - unused 

Return value:

  SUCCESS indicates that the request to register for Link Status update is
  queued and that AEEBT_RM_EVT_LINK_STATUS_REG and AEEBT_RM_EVT_LINK_STATUS 
  events will be generated at a later time.  All other values indicate 
  failure, and will not generate an event.  In this context, the following 
  error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_LINK_STATUS_REG with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_NO_CONN  - no existing connection to remote device
    AEEBT_RM_ERR_FAILED     - failed to register for link status
    
=======================================================================

Function:  IBTEXTRM_UnregisterLinkStatus()

Description: Unregisters for Link Status update for an existing connection.

Prototype:

  int IBTEXTRM_UnregisterLinkStatus( IBTExtRM* po, 
                                     const AEEBTBDAddr* pBDAddr );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device

Return value:

  SUCCESS indicates that the request to unregister for Link Status update is
  queued and that AEEBT_RM_EVT_LINK_STATUS_UNREG event will be generated at 
  a later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_LINK_STATUS_UNREG with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_NO_CONN  - no existing connection to remote device
    AEEBT_RM_ERR_FAILED   - failed to unregister for link status
    
=======================================================================

Function:  IBTEXTRM_GetLinkStatus()

Description: Gets Link Status for an existing connection.  The BD address in
             pLinkStatus tells for which remote device to get link status.

Prototype:

  int IBTEXTRM_GetLinkStatus( IBTExtRM* po, AEEBTLinkStatus* pLinkStatus );

Parameters:

  po           - pointer to the IBTExtRM object
  pLinkStatus  - memory location to which to copy Link Status 

Return value:

  SUCCESS indicates that the Link Status for the given remote device has been
  copied to the given memory location.  No events will be generated for this 
  command. In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    EFAILED   - no existing connection to given remote device

=======================================================================

Function:  IBTEXTRM_DeviceAdd()

Description: Adds a device to the device database maintained by RM.

             Set the 'bValid' flag in pDevice is FALSE to indicate that the
             device information is expected to exists in RM.  Set that
             flag to FALSE if pDevice contains information for the device
             to be added.  The 'bBonded' and 'linkKeyType' will be ignored.

Prototype:

  int IBTEXTRM_DeviceAdd( IBTExtRM* po, const AEEBTDevice* pDevice );

Parameters:

  po           - pointer to the IBTExtRM object
  pDevice      - pointer to the Device structure to be added to RM database

Return value:

  SUCCESS indicates that the request to add device to device database was
  queued and that AEEBT_RM_EVT_DEVICE_ADDED event will be generated at a 
  later time.  All other values indicate failure, and will not generate an 
  event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory
   
Event:
  AEEBT_RM_EVT_DEVICE_ADDED with accompanying Device Update Status whose 
  Error field is of one of the following error codes:
  
    AEEBT_RM_ERR_DEVICE_EXISTS    - this device has already been added
    AEEBT_RM_ERR_DB_WRITE_FAILED  - error occurred writing DB to flash
    AEEBT_RM_ERR_DB_FULL          - device DB is full

=======================================================================

Function:  IBTEXTRM_DeviceUpdate()

Description: Updates a device in the device database maintained by RM.
             Multiple fields in a device record can be updated.

Prototype:

  int IBTEXTRM_DeviceUpdate( IBTExtRM* po, AEEBTDeviceUpdateBitmap bm,
                             const AEEBTDevice* pDevice );

Parameters:

  po           - pointer to the IBTExtRM object
  bm           - bitmap of Device structure fields to be updated
  pDevice      - pointer to the Device structure containing updated info

Return value:

  SUCCESS indicates that the request to update device record in the 
  device database was queued and that AEEBT_RM_EVT_DEVICE_UPDATED event 
  will be generated at a later time.  All other values indicate failure, 
  and will not generate an event.  In this context, the following error 
  codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_DEVICE_UPDATED with accompanying Device Update Status whose 
  Error field is of one of the following error codes:

    AEEBT_RM_ERR_NO_DEVICE        - device not found in DB
    AEEBT_RM_ERR_DB_WRITE_FAILED  - error occurred writing DB to flash
  
=======================================================================

Function:  IBTEXTRM_DeviceServicesUpdate()

Description: Updates the service information of a device in the device database 
             maintained by RM.
             
Prototype:

  int IBTEXTRM_DeviceServicesUpdate( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                     uint16 uNumServices,
                                     const AEEBTSimpleService* pServices );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device
  uNumServices - number of services to be added to the device in the database
  pServices    - pointer to the array of service information to be added

Return value:

  SUCCESS indicates that the request to update device service record in the 
  device database was queued and that AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED event 
  will be generated at a later time.  All other values indicate failure, 
  and will not generate an event.  In this context, the following error 
  codes have special meaning:
  
    EFAILED   - invalid pointer to the IBTExtRM object
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_DEVICE_SERVICES_UPDATED with accompanying Device Update Status 
  whose Error field is of one of the following error codes:

    AEEBT_RM_ERR_NO_DEVICE        - device not found in DB
    AEEBT_RM_ERR_DB_WRITE_FAILED  - error occurred writing DB to flash
  
=======================================================================

Function:  IBTEXTRM_DeviceRead()

Description: Copies from device database information on the device whose
             BD address is passed in via pDevice.  The 'valid' flag in
             pDevice indicates whether device is found.

Prototype:

  int IBTEXTRM_DeviceRead( IBTExtRM* po, AEEBTDeviceInfo* pDevice );

Parameters:

  po           - pointer to the IBTExtRM object
  pDevice      - pointer to memory location to store device info

Return value:

  SUCCESS indicates that the device info has been copied from device database.
  No events will be generated for this command. In this context, the following 
  error codes have special meaning:
  
    EBADPARM  - invalid input pointers or device not found in DB

=======================================================================

Function:  IBTEXTRM_DeviceEnumInit()

Description: Initializes/resets the enumeration in the RM device database.

Prototype:

  int IBTEXTRM_DeviceEnumInit( IBTExtRM*              po, 
                               AEEBTDeviceEnumerator* pEnum );

Parameters:

  po           - pointer to the IBTExtRM object
  pEnum        - pointer to the enumerator info

Return value:

  SUCCESS indicates that the enumeration in the RM device database is ready.
  No events will be generated for this command. In this context, the 
  following error codes have special meaning:
  
    EBADPARM  - invalid input pointer(s) or invalid info passed in via pEnum

=======================================================================

Function:  IBTEXTRM_DeviceEnumNext()

Description: Retrieves the next entry in RM device database matching 
             the criteria specified via IBTEXTRM_DeviceEnumInit().  This API
             should be called repeatedly until the 'valid' flag in pDevice is
             FALSE.

Prototype:

  int IBTEXTRM_DeviceEnumNext( IBTExtRM*        po, 
                               AEEBTDeviceInfo* pDevice );

Parameters:

  po           - pointer to the IBTExtRM object
  pDevice      - pointer to memory location to store device info

Return value:

  SUCCESS indicates that a device matching the previously set criteria has
  been copied from device database.  No events will be generated for this 
  command.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointer(s)

=======================================================================

Function:  IBTEXTRM_DeviceRemove()

Description: Removes a device from the RM device database.

Prototype:

  int IBTEXTRM_DeviceRemove( IBTExtRM* po, const AEEBTBDAddr* pBDAddr );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of device to be removed

Return value:

  SUCCESS indicates that the request to remove device from device database was
  queued and that AEEBT_RM_EVT_DEVICE_REMOVED event will be generated 
  at a later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_DEVICE_REMOVED can be accompanied by any of the following
  error codes:
  
    AEEBT_RM_ERR_NONE             - no error
    AEEBT_RM_ERR_NO_DEVICE        - device not found in device DB
    AEEBT_RM_ERR_DB_WRITE_FAILED  - error occurred writing DB to flash

=======================================================================

Function:  IBTEXTRM_GetHCIMode()

Description: Get the current HCI mode of BT driver. This API is reserved for
             system testing.  Only apps with system priviledge can use this.

Prototype:

  int IBTEXTRM_GetHCIMode( IBTExtRM* po, AEEBTHCIMode* pHCIMode );

Parameters:

  po           - pointer to the IBTExtRM object
  pHCIMode     - pointer to the variable where the HCI mode is returned

Return value:

  SUCCESS      - the BT on/off status has been successfully read
  EBADPARM     - invalid input pointer
  
Event:

  None

=======================================================================

Function:  IBTEXTRM_SetHCIMode()

Description: Set the HCI mode of BT driver. This API is reserved for
             system testing.  Only apps with system priviledge can use this.

Prototype:

  int IBTEXTRM_SetHCIMode( IBTExtRM* po, AEEBTHCIMode newHCIMode );

Parameters:

  po           - pointer to the IBTExtRM object
  newHCIMode   - the desired HCI mode to set

Return value:

  SUCCESS      - the new HCI mode has been set
  EFAILED      - could not set the new HCI mode, unknown reason
  AEEBT_EPORT  - must map the port before setting HCI mode
  
Event:

  None

=======================================================================

Function:  IBTEXTRM_SetServiceSecurity()

Description: Sets the security level of all connections to a BT service.

Prototype:

  int IBTEXTRM_SetServiceSecurity( IBTExtRM* po, 
                                   const AEEBTServiceIdentifier* pServiceId,
                                   AEEBTSecurity minSecurity, 
                                   boolean bAuthorize, 
                                   boolean bAuthorizeFirst );

Parameters:

  po              - pointer to the IBTExtRM object
  pServiceId      - service identifier
  minSecurity     - mininum security level for connections to this service
  bAuthorize      - TRUE to enable Authorization upon connection requests 
                    to this service
  bAuthorizeFirst - TRUE if Authorization should occur before security level 
                    is enforced

Return value:

  SUCCESS indicates that the request to set security level of all connections
  to a BT service was queued and that AEEBT_RM_EVT_SERVICE_SECURITY_SET event 
  will be generated at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error codes have 
  special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_SERVICE_SECURITY_SET with accompanying with accompanying 
  error code of one of the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_FAILED     - service security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_AuthorizeReply()

Description: Sends an authorization result in response to a 
             AEEBT_RM_EVT_AUTHORIZE_REQUEST.

Prototype:

  int IBTEXTRM_AuthorizeReply( IBTExtRM* po, 
                               const AEEBTServiceIdentifier* pServiceId,
                               const AEEBTBDAddr* pBDAddr,
                               boolean bAuthorize );

Parameters:

  po              - pointer to the IBTExtRM object
  pServiceId      - service identifier
  pBDAddr         - BD address of remote device
  bAuthorize      - TRUE to authorization the connection request

Return value:

  SUCCESS indicates that the request to send the authorization result was 
  queued and that AEEBT_RM_EVT_AUTHORIZE_REPLIED event will be generated at a 
  later time.  All other values indicate failure, and will not generate an 
  event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_AUTHORIZE_REPLIED with accompanying error code of one of 
  the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_NO_CONN    - no matching connections found
    AEEBT_RM_ERR_BUSY       - another BT operation is in progress
    AEEBT_RM_ERR_FAILED     - device security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_EnterTestMode()

Description: Enables Device Under Test (DUT) mode for BT RF testing.  
             This API is reserved for system testing.  Only apps with 
             system priviledge can use this.

Prototype:

  int IBTEXTRM_EnterTestMode( IBTExtRM* po );

Parameters:

  po           - pointer to the IBTExtRM object

Return value:

  SUCCESS indicates that the request to enter DUT mode was queued and that 
  AEEBT_RM_EVT_DUT_ENTERED event will be generated at a later time.  All 
  other values indicate failure, and will not generate an event.  In this 
  context, the following error codes have special meaning:
  
    ENOMEMORY - out of memory
    EALREADY  - already in DUT mode
  
Event:

  AEEBT_RM_EVT_DUT_ENTERED with accompanying error code:
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BUSY       - another BT operation is in progress

=======================================================================

Function:  IBTEXTRM_ExitTestMode()

Description: Disables Device Under Test (DUT) mode for BT RF testing.  
             This API is reserved for system testing.  Only apps with 
             system priviledge can use this.

Prototype:

  int IBTEXTRM_DisableTestMode( IBTExtRM* po );

Parameters:

  po           - pointer to the IBTExtRM object

Return value:

  SUCCESS indicates that the request to exit DUT mode was queued and that 
  AEEBT_RM_EVT_DUT_EXITED event will be generated at a later time.  All 
  other values indicate failure, and will not generate an event.  In this 
  context, the following error codes have special meaning:
  
    ENOMEMORY - out of memory
    EALREADY  - already out of DUT mode
  
Event:

  AEEBT_RM_EVT_DUT_EXITED with accompanying error code:
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BUSY       - another BT operation is in progress
    
=======================================================================

Function:  IBTEXTRM_GetBTOnOffState()

Description: Returns whether BT is currently "on". BT is "on" if the device
             is in a connection, paging, inquiring, scanning, or in one of the 
             low power modes.

Prototype:

  int IBTEXTRM_IsBTOn( IBTExtRM* po, boolean* pBTIsOn );

Parameters:

  po           - pointer to the IBTExtRM object
  pBTIsOn      - pointer to the boolean variable where the result will be 
                 returned

Return value:

  SUCCESS      - the BT on/off status has been successfully read
  EBADPARM     - invalid input pointer
  
Event:

  None
                                                                       
=======================================================================

Function:  IBTEXTRM_GetRoleSwitchPolicy()

Description: Gets the local role switch policy (allowed or not) on an 
             existing connection to a given BD address, or on future
             connections if no BD address is given.

Prototype:

  int IBTEXTRM_GetRoleSwitchPolicy( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                                    boolean* pbAllowed );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device; NULL to indicate local device
  pbAllowed   - destination for the role switch policy

Return value:

  SUCCESS indicates that the role switch policy has been copied to pbAllowed.
  No events will be generated for this command.  In this context, the 
  following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOSUCH   - no connection for given BD address

=======================================================================

Function:  IBTEXTRM_AllowRoleSwitch()

Description: Allows/disallows master/slave role switch with remote device

Prototype:

  int IBTEXTRM_AllowRoleSwitch( IBTExtRM* po, boolean bAllow );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - BD address of remote device; NULL indicates this will 
                 affect all future connections
  bAllow       - TRUE to allow role switching; FALSE to disallow it

Return value:

  SUCCESS      - request was successfully queued and 
                 AEEBT_RM_EVT_ROLE_SW_CTRL_SET will be generated at a later time.
  EBADPARM     - invalid input pointer
  
Event:

  AEEBT_RM_EVT_ROLE_SW_CTRL_SET with accompanying error code of one of 
  the following values:

    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_NO_CONN    - not connected to given remote device
    
=======================================================================

Function:  IBTEXTRM_DisableRadio()

Description: Turns off/on the radio.

Prototype:

  int IBTEXTRM_IsBTOn( IBTExtRM* po, boolean bDisable );

Parameters:

  po           - pointer to the IBTExtRM object
  bDisable     - TRUE to disable radio; FALSE to enable it.

Return value:

  SUCCESS      - request was successfully queued and AEEBT_RM_EVT_RADIO_STATUS
                 will be generated at a later time.
  EBADPARM     - invalid input pointer
  
Event:

  AEEBT_RM_EVT_RADIO_STATUS with accompanying status code of one of 
  the following values:
  
    AEEBT_RM_RADIO_OFF
    AEEBT_RM_RADIO_ON
                                                                       
=======================================================================

Function:  IBTEXTRM_DisableVisibility()

Description: Turns off/on the radio's discoverability.

Prototype:

  int IBTEXTRM_DisableVisibility( IBTExtRM* po, boolean bDisable );

Parameters:

  po           - pointer to the IBTExtRM object
  bDisable     - TRUE to disable visibility; FALSE to enable it.

Return value:

  SUCCESS      - request was successfully queue and 
                 AEEBT_RM_EVT_VISIBILITY_STATUS will be generated at a later time.
  EBADPARM     - invalid input pointer
  
Event:

  AEEBT_RM_EVT_VISIBILITY_STATUS with accompanying status code of one of 
  the following values:
  
    AEEBT_RM_VISIBILITY_OFF
    AEEBT_RM_VISIBILITY_ON

=======================================================================

Function:  IBTEXTRM_RegisterConnStatus

Description: Registers for connection status event, to inform app of
             connection/disconnection events.

Prototype:

  int IBTEXTRM_RegisterConnStatus( IBTExtRM* po );

Parameters:

  po           - pointer to the IBTExtRM object

Return value:

  SUCCESS      - request was successfully enqueued and AEEBT_RM_EVT_CONN or
                 AEEBT_RM_EVT_DISCONN may be generated at a later time.
Event:

  AEEBT_RM_EVT_CONN with accompanying BD Address data
  AEEBT_RM_EVT_DISCONN with accompanying BD Address data

=======================================================================

Function:  IBTEXTRM_UnregisterConnStatus

Description: Unregisters for connection status event, to no longer inform 
             app of connection/disconnection events.

Prototype:

  int IBTEXTRM_UnregisterConnStatus( IBTExtRM* po );

Parameters:

  po           - pointer to the IBTExtRM object

Return value:

  SUCCESS      - request was successfully enqueued and AEEBT_RM_EVT_CONN 
                 or DISCONN will no longer be generated.
Event:

  None

=======================================================================

Function:  IBTEXTRM_RegisterRadioActivity()

Description: Registers for Radio Activity update.

Prototype:

  int IBTEXTRM_RegisterRadioActivity( IBTExtRM* po, 
                                      AEEBTRadioActivity* pRadioAct,
                                      AEECallback* pCb );

Parameters:

  po           - pointer to the IBTExtRM object
  pRadioAct    - updated radio activity to be stored here
  pCb          - pointer to callback to be called when radio activity changes

Return value:

  SUCCESS indicates that the request to register for Radio Activity update is
  queued and that AEEBT_RM_EVT_RADIO_ACTIVITY_REG and AEEBT_RM_EVT_RADIO_ACTIVITY 
  events will be generated at a later time.  All other values indicate 
  failure, and will not generate an event.  In this context, the following 
  error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_RADIO_ACTIVITY_REG with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_FAILED   - failed to register for radio activity
    
=======================================================================

Function:  IBTEXTRM_UnregisterRadioActivity()

Description: Unregisters for Link Status update for an existing connection.

Prototype:

  int IBTEXTRM_UnregisterRadioActivity( IBTExtRM* po, 
                                        const AEEBTBDAddr* pBDAddr );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - pointer to the BT device address of the remote device

Return value:

  SUCCESS indicates that the request to unregister for Link Status update is
  queued and that AEEBT_RM_EVT_LINK_STATUS_UNREG event will be generated at 
  a later time.  All other values indicate failure, and will not generate 
  an event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_LINK_STATUS_UNREG with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE     - no error
    AEEBT_RM_ERR_NO_CONN  - no existing connection to remote device
    AEEBT_RM_ERR_FAILED   - failed to unregister for link status
    
=======================================================================

Function:  IBTEXTRM_GetRadioActivity()

Description: Gets Link Status for an existing connection.  The BD address in
             pRadioActivity tells for which remote device to get link status.

Prototype:

  int IBTEXTRM_GetRadioActivity( IBTExtRM* po, 
                                 AEEBTRadioActivity* pRadioActivity );

Parameters:

  po              - pointer to the IBTExtRM object
  pRadioActivity  - memory location to which to copy Link Status 

Return value:

  SUCCESS indicates that the Link Status for the given remote device has been
  copied to the given memory location.  No events will be generated for this 
  command. In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    EFAILED   - no existing connection to given remote device

=======================================================================
=======================================================================

Function:  IBTEXTRM_SetNickName()

Description: Sets a nick name for the device of given BD address.  
             If no BD address is given, the nick name is set for local
             device and is the name sent in outgoing EIR.  Only up to
             the first AEEBT_MAX_NICKNAME_LEN characters of the given
             nick name will be saved.

Prototype:

  int IBTEXTRM_SetNickName( IBTExtRM* po, const AEEBTBDAddr* pBDAddr,
                            const AECHAR* pNickName );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - remote device address if not NULL, local device otherwise
  pNickName    - nick name for the given device

Return value:

  SUCCESS indicates that the request to set BT name was queued and that
  AEEBT_RM_EVT_NICK_NAME_SET event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
  
    ENOMEMORY - out of memory
    EBADPARM  - invalid input pointers

Event:
  AEEBT_RM_EVT_NICK_NAME_SET with accompanying error code of one 
  of the following values:
  
    AEEBT_RM_ERR_NONE             - no error
    AEEBT_RM_ERR_NO_DEVICE        - device not found in device DB

=======================================================================

Function:  IBTEXTRM_SetEIRManufData()

Description: Sets the Manufacturer data for outgoing EIR.

Prototype:

  int IBTEXTRM_SetEIRManufData( IBTExtRM* po, const uint8* pManufData,
                                uint8 uSize );

Parameters:

  po           - pointer to the IBTExtRM object
  pManufData   - pointer to data to be used as manufacturer specific
                 data in outgoing EIR
  uSize        - size, in bytes, of Manufacturer data

Return value:

  SUCCESS indicates that the request to set Manufacturer data was queued 
  and that AEEBT_RM_EVT_MANUF_DATA_SET event will be generated at a later time.  
  All other values indicate failure, and will not generate an event.  
  In this context, the following errorcodes have special meaning:
  
    ENOMEMORY - out of memory
    EBADPARM  - invalid input pointers

Event:
  AEEBT_RM_EVT_MANUF_DATA_SET with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error
=======================================================================

Function:  IBTEXTRM_GetEIRUUIDList()

Description: Reads UUID list for a particular remote device.

Prototype:

  int IBTEXTRM_GetEIRUUIDList( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                               AEEBTUUIDType uuidType,
                               uint8 uuidListStartIdx,
                               uint8* uuidListSize,
                               void* pUUIDList );

Parameters:

  po           - pointer to the IBTExtRM object
  pBDAddr      - BD address of remote device
  uuidType     - identifies the type of UUID(16/32/128) requested
  uuidListStartIdx - the start idx of the UUID from the entire list
                     of UUIDs for this remote device. The UUID list will be
                     returned starting at this index.
  uuidListSize - pointer to size of the array which will hold the UUID list. 
                 The driver will set this to the size of data copied.
  pUUIDList    - pointer to the array where the UUID list will be copied

Return value:

  SUCCESS indicates that the requested info has been copied from device database.
  No events will be generated for this command. In this context, the following 
  error codes have special meaning:
  
    EBADPARM        - invalid input parameters
    ENOSUCH         - no UUID at given index
    EBUFFERTOOSMALL - pUUIDList too small
=======================================================================

Function:  IBTEXTRM_GetEIRManufData()

Description: Gets the manufacturer specific data for a particular remote 
             device received in an incoming EIR.

Prototype:

  int IBTEXTRM_GetEIRManufData( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                                uint8* pSize, uint8* pManufData );

Parameters:

  po          - pointer to the IBTExtRM object
  pBDAddr     - BD address of remote device
  pManufData  - pointer to the array used to store the manufacturer specific
                data from incoming EIR
  pSize       - pointer to size, in bytes, of the array which will hold the 
                manufacturer specifc data. The driver will set this to the
                size of data copied.

Return value:

  SUCCESS indicates that the requested info has been copied.
  No events will be generated for this command. In this context, the following 
  error codes have special meaning:
  
    EBADPARM        - invalid input parameters
    ENOSUCH         - no UUID at given index
    EBUFFERTOOSMALL - pUUIDList too small

=======================================================================

Function:  IBTEXTRM_SetIOCap()

Description: Sets the IO capability of the local device.

Prototype:

  int IBTEXTRM_SetIOCap( IBTExtRM* po, AEEBTIOCapType IOCap );

Parameters:

  po          - pointer to the IBTExtRM object
  IOCap       - IO capability of the local device

Return value:

  SUCCESS indicates that the IO Capability settings for the local device
  has been scheduled to be stored in the driver and that at a later time
  AEEBT_RM_EVT_IO_CAP_SET will be generated to report the result.
  In this context, the following error codes have special meaning:
  
    EBADPARM        - invalid input parameters

Event:
  AEEBT_RM_EVT_IO_CAP_SET with accompanying with accompanying 
  error code of one of the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    
=======================================================================

Function:  IBTEXTRM_SetSecBySvcCls()

Description: Sets the security level for a particular service identified
             by the given Service Class ID.

Prototype:

  int IBTEXTRM_SetSecBySvcCls( IBTExtRM* po, 
                               uint16 uServiceClass,
                               AEEBTSvcSecLevel minSecurity, 
                               boolean bAuthorize, 
                               boolean bAuthorizeFirst );

Parameters:

  po              - pointer to the IBTExtRM object
  uServiceClass   - class ID of the service of interest
  minSecurity     - mininum security level for connections to this service
  bAuthorize      - TRUE to enable Authorization upon connection requests 
                    to this service
  bAuthorizeFirst - TRUE if Authorization should occur before security level 
                    is enforced

Return value:

  SUCCESS indicates that the request to set security level of all connections
  to a BT service was queued and that AEEBT_RM_EVT_SERVICE_SECURITY_SET event 
  will be generated at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error codes have 
  special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_SERVICE_SECURITY_SET with accompanying with accompanying 
  error code of one of the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_FAILED     - service security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_SetSecBySrvrChnl()

Description: Sets the security level for a particular service identified
             by the given server channel number.

Prototype:

  int IBTEXTRM_SetSecBySrvrChnl( IBTExtRM* po, 
                                 uint8 uSCN,
                                 AEEBTSvcSecLevel minSecurity, 
                                 boolean bAuthorize, 
                                 boolean bAuthorizeFirst );

Parameters:

  po              - pointer to the IBTExtRM object
  uSCN            - server channel number of the service
  minSecurity     - mininum security level for connections to this service
  bAuthorize      - TRUE to enable Authorization upon connection requests 
                    to this service
  bAuthorizeFirst - TRUE if Authorization should occur before security level 
                    is enforced

Return value:

  SUCCESS indicates that the request to set security level of all connections
  to a BT service was queued and that AEEBT_RM_EVT_SERVICE_SECURITY_SET event 
  will be generated at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error codes have 
  special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_SERVICE_SECURITY_SET with accompanying with accompanying 
  error code of one of the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_FAILED     - service security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_SetSecByPSM()

Description: Sets the security level for a particular service identified
             by the given L2CAP Protocol Service Multiplexor number.

Prototype:

  int IBTEXTRM_SetSecByPSM( IBTExtRM* po, 
                            uint16 uPSM,
                            AEEBTSvcSecLevel minSecurity, 
                            boolean bAuthorize, 
                            boolean bAuthorizeFirst );

Parameters:

  po              - pointer to the IBTExtRM object
  uPSM            - L2CAP Protocol Service Multiplexor
  minSecurity     - mininum security level for connections to this service
  bAuthorize      - TRUE to enable Authorization upon connection requests 
                    to this service
  bAuthorizeFirst - TRUE if Authorization should occur before security level 
                    is enforced

Return value:

  SUCCESS indicates that the request to set security level of all connections
  to a BT service was queued and that AEEBT_RM_EVT_SERVICE_SECURITY_SET event 
  will be generated at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error codes have 
  special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_SERVICE_SECURITY_SET with accompanying with accompanying 
  error code of one of the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_FAILED     - service security not set, no specific reason

=======================================================================

Function:  IBTEXTRM_SetRemoteOOBData()

Description: Sets the OOB data received from remote device via OOB mechanism.
             This data will be used for pairing, hence this command should be
             issued before pairing begins.

Prototype:

  int IBTEXTRM_SetOOBData( IBTExtRM*     po, 
                           const uint8*  pOOBData,
                           uint8         uSize );

Parameters:

  po       - pointer to the IBTExtRM object
  pOOBData - pointer to the OOB data 
  uSize    - OOB data size

Return value:

  SUCCESS indicates that the requested info has been copied.
  No events will be generated for this command. In this context, the following 
  error codes have special meaning:
  
    EBADPARM        - invalid input parameters

=======================================================================

Function:  IBTEXTRM_UserConfirmationReply()

Description: Sends a user confirmation reply in response to a 
             AEEBT_RM_EVT_USER_CFM_REQUEST.  This event may come
             with or without a 6-digit passkey.  The user is expected to
             verify that the passkey is the same as that displayed on
             remote device.  If MITM protection is not required, the event
             would not contain a passkey.

Prototype:

  int IBTEXTRM_UserConfirmationReply( IBTExtRM* po, 
                                      const AEEBTBDAddr* pBDAddr,
                                      boolean bConfirmed );

Parameters:

  po              - pointer to the IBTExtRM object
  pBDAddr         - BD address of remote device
  bConfirmed      - TRUE to confirm that passkey matches 

Return value:

  SUCCESS indicates that the request to send the user confirmation reply was 
  queued and that AEEBT_RM_EVT_USER_CFM_REPLIED event will be generated at a 
  later time.  All other values indicate failure, and will not generate an 
  event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_USER_CFM_REPLIED with accompanying error code of one of 
  the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_NO_CONN    - no matching connections found

=======================================================================

Function:  IBTEXTRM_KeypressNotification()

Description: Sends a keypress notification to the remote.  When app receives
             AEEBT_RM_EVT_PASSKEY_REQUEST, it should prompt user to enter
             a 6-digit passkey which is displayed on the remote device.
             This command shall be issued for each key pressed.  
             IBTEXTRM_PasskeyReply command should then be issued to indicate 
             completion of passkey entry.

Prototype:

  int IBTEXTRM_KeypressNotification( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                                     AEEBTKeypressType keypress );

Parameters:

  po        - pointer to the IBTExtRM object
  pBDAddr   - BD address of remote device
  Keypress  - type of keypress on the local device to be sent to the remote

Return value:

  SUCCESS indicates that the request to send keypress to the remote has been
  queued. No events will be generated for this 
  command. In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    EBADSTATE - unexpected notification
=======================================================================

Function:  IBTEXTRM_PasskeyReply()

Description: Informs BT driver that passkey entry is completed.
             If the passkey is not exactly 6-digit, it's considered invalid
             and treated as an indication that user has canceled the process.
             This command should be used in conjunction with 
             IBTEXTRM_KeypressNotification command.
             
             This command can also be used to cancel a pairing process which 
             began with AEEBT_RM_EVT_PASSKEY_NOTIFICATION by making the passkey
             response be equal to a NULL string.

Prototype:

  int IBTEXTRM_PasskeyReply( IBTExtRM* po, 
                             const AEEBTBDAddr* pBDAddr,
                             const char passkey[ AEEBT_SSP_PASSKEY_LEN ] );

Parameters:

  po              - pointer to the IBTExtRM object
  pBDAddr         - BD address of remote device
  szPasskey       - the 6-digit passkey

Return value:

  SUCCESS indicates that the request to send the passkey reply was 
  queued and that AEEBT_RM_EVT_PASSKEY_REPLIED event will be generated at a 
  later time.  All other values indicate failure, and will not generate an 
  event.  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_PASSKEY_REPLIED with accompanying error code of one of 
  the following values:
  
    AEEBT_RM_ERR_NONE       - no error
    AEEBT_RM_ERR_BAD_PARAM  - invalid parameters
    AEEBT_RM_ERR_NO_CONN    - no matching connections found

=======================================================================

Function:  IBTEXTRM_CreateOOBData()

Description: Asks the driver to create the OOB data.  This data is
             intended to be transferred to remote device via OOB mechanism.

Prototype:

  int IBTEXTRM_CreateOOBData( IBTExtRM*     po, 
                              uint8*        pOOBDataSize,
                              AEECallback*  pCb );

Parameters:

  po           - pointer to the IBTExtRM object
  pOOBDataSize - Size of the generated OOB data 
  pCb          - callback to be invoked when the OOB data is available

Return value:

  SUCCESS indicates that the local OOB data retrieval has been scheduled
  and the callback will be invoked when OOB data is made available and size 
  of the generated data is known. In this context, the following error codes
  have special meaning:
  
    EBADPARM  - invalid input pointers

=======================================================================

Function:  IBTEXTRM_ReadOOBData()

Description: Read the OOB data generated by the driver.  This data is
             intended to be transferred to remote device via OOB mechanism.

Prototype:

  int IBTEXTRM_ReadOOBData( IBTExtRM* po, 
                            uint8*    pOOBDataSize,
                            uint8*    pOOBData );

Parameters:

  po           - pointer to the IBTExtRM object
  pOOBDataSize - OOB Data size
  pOOBData     - destination for the locally generated OOB

Return value:

  SUCCESS indicates that the requested info has been copied.
  No events will be generated for this command. In this context, the following 
  error codes have special meaning:
  
    EBADPARM        - invalid input parameters

=======================================================================

Function:  IBTEXTRM_AuthorizeRebond()

Description: Replies to the AEEBT_RM_EVT_REBOND_REQUEST.

Prototype:

  int IBTEXTRM_AuthorizeRebond( IBTExtRM* po, 
                                const AEEBTBDAddr* pBDAddr,
                                boolean bAuthorized,
                                AEEBTRebondSecurity rebondSec );

Parameters:

  po              - pointer to the IBTExtRM object
  pBDAddr         - BD address of remote device
  bAuthorized     - TRUE to confirm rebonding
  rebondSec       - valid if bAuthorized=TRUE

Return value:

  SUCCESS indicates that the reply to the rebond request was queued and that
  AEEBT_RM_EVT_BONDED vent will be generated at a later time.  All other values 
  indicate failure, and will not generate an event.  In this context, the 
  following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_BONDED with accompanying error code of one of 
  the following values:
  
    AEEBT_RM_ERR_NONE         - no error
    AEEBT_RM_ERR_BAD_PARAM    - invalid parameters
    AEEBT_RM_ERR_NO_CONN      - no matching connections found
    AEEBT_RM_ERR_BOND_FAILED  - failed to bond, reason unknown

=======================================================================

Function:  IBTEXTRM_SSPBond()

Description: Performs Bluetooth bonding with a remote device that supports
             Secure Simple Paring feature of BT 2.1.

Prototype:

  int IBTEXTRM_SSPBond( IBTExtRM* po, const AEEBTBDAddr* pBDAddr, 
                        boolean bMITMProtectionRequired );

Parameters:

   po           - pointer to the IBTExtRM object
   pBDAddr      - pointer to the BT device address of the remote device
   bMITMProtectionRequired - TRUE if MITM protection is required, FALSE otherwise

Return value:

  SUCCESS indicates that the request to bond with the remote device was queued 
  and that AEEBT_RM_EVT_BONDED event will be generated at a later time. All 
  other values indicate failure, and will not generate an event.  
  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  AEEBT_RM_EVT_BONDED with accompanying error code of one of the
  following values:
  
    AEEBT_RM_ERR_NONE             - no error
    AEEBT_RM_ERR_NO_DEVICE        - no such device in DB
    AEEBT_RM_ERR_BOND_FAILED      - failed to bond, reason unknown

=======================================================================

Function:  IBTEXTRM_SetSM4DebugMode

Description: Forces creation of a Debug Link Key when using Security Mode
             4. Debug link keys allow external packet analyzers to view
             and debug Bluetooth traffic.
             Note that even when Debug Mode is disabled, debug keys may
             still be created if the remote device requests a debug key.

Prototype:

  int IBTEXTRM_SetSM4DebugMode( IBTExtRM* po, boolean bDebugMode );

Parameters:

   po           - pointer to the IBTExtRM object
   bDebugMode   - TRUE forces creation of debug keys

Return value:

  SUCCESS indicates that the request to enter debug mode was queued.
  All other values indicate failure.
  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  None.
=======================================================================

Function:  IBTEXTRM_LowPowerConfig

Description: Configures Low Power Mode settings for a given connection.

Prototype:

  int IBTEXTRM_LowPowerConfig( IBTExtRM* po, 
                               const AEEBTBDAddr* pBDAddr, 
                               AEEBTLowPowerControl lpc, 
                               uint16 uMaxInterval,
                               uint16 uIdleTO, 
                               uint16 uMaxLatency, 
                               boolean bMustUse, 
                               boolean bSniffInitiatorOriented );

Parameters:

   po           - pointer to the IBTExtRM object
   pBDAddr      - address of remote end of the connection
   lpc          - low power choice
   uMaxInteval  - max num milliseconds between sniff anchor points
   uIdleTO      - milliseconds of idle time before LPM is entered;
                  0 means "dont care"
   uMaxLatency  - max num milliseconds local device can wait for response from 
                  remote; 0 means "disable SSR"
   bMustUse     - TRUE means the given time paramters must be used;
                  FALSE means remote negotiated values can be used
   bSniffInitiatorOriented - TRUE means only Sniff initiator can initate SSR

Return value:

  SUCCESS indicates that the request to configure Low Power mode was queued.
  All other values indicate failure.
  In this context, the following error codes have special meaning:
  
    EBADPARM  - invalid input pointers
    ENOMEMORY - out of memory

Event:
  None.
=======================================================================

Function:  IBTEXTRM_BondCancel()

Description: Cancels the Bond operation that is currently in progress
             with the specified remote device.
Prototype:

  int IBTEXTRM_BondCancel( IBTExtRM* po, const AEEBTBDAddr* pBDAddr );

Parameters:

   po           - pointer to the IBTExtRM object
   pBDAddr      - pointer to the BT device address of the remote device

Return value:

   SUCCESS indicates that the request to cancel bond with the remote device was queued 
   and that AEEBT_RM_EVT_BONDED event will be generated at a later time. All 
   other values indicate failure, and will not generate an event.  
   In this context, the following error codes have special meaning:

   EBADPARM  - invalid input pointers
   ENOMEMORY - out of memory

Event:
  If bonding was cancelled, event AEEBT_RM_EVT_BONDED
  will be generated with an error code.

=======================================================================*/
#endif /* _AEE_BT_EXT_RM_H_ */

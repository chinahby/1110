#ifndef AEETSUPPSDEF_H
#define AEETSUPPSDEF_H
/*======================================================
FILE:  AEETSuppsDef.h

SERVICES:  Definitions of data structures used for Supplementary services.

GENERAL DESCRIPTION:

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEStdDef.h"

/* identifies a supplementary service transaction by ID. */
typedef uint32 AEETSuppsTransId;

/* A reserved ID used to create a mobile initiated transaction */
#define AEET_SUPPSVC_TRANSID_NEW   0xFFFFFFFF

/* The maximum size of string request acceptable by standard */
#define AEETSUPPS_MAX_DATA_LEN 160

#define AEETSUPPS_FWD_FEATURE_LIST_SIZE       13 /*Max Number of Fwd To Numbers*/
#define AEETSUPPS_CALL_BARRING_LIST_SIZE      13 /*Max Number of Call Barred Numbers*/
#define AEETSUPPS_BASIC_SERV_GROUP_LIST_SIZE  13 /*Max Number of Basic Srv Groups*/

#define AEETSUPPS_MAX_PASSWD_LEN              9  /* 8 characters acceptable for password + '\0' */
#define AEETSUPPS_MAX_IMEI_LEN                16 /* 15 bytes of IMEI followed by NULL termination */

#define AEETSUPPS_MAX_ALPHA_LEN               256 /* Maximum length of alphanumeric tag */

#define AEETSUPPS_ERR_BM(val) ((val) << 16)


/* ERROR tags */
#define AEETSUPPS_ERROR_CODE_TAG          0x02
#define AEETSUPPS_MN_SS_ERROR_TAG         0xc0  /* used by MN -> network */
#define AEETSUPPS_GENERAL_PROBLEM         0x80
#define AEETSUPPS_INVOKE_PROBLEM          0x81
#define AEETSUPPS_RETURN_RESULT_PROBLEM   0x82
#define AEETSUPPS_RETURN_ERROR_PROBLEM    0x83

/* SS error codes */
typedef enum
{
   AEETSUPPS_ERR_NO_ERR            = 0x00 | (AEETSUPPS_ERR_BM(0x00)),
   /*Unknown Subscriber Error*/
   AEETSUPPS_ERR_UNKNOWN_SUBR=0x01 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Illegal Subscriber*/
   AEETSUPPS_ERR_ILLEGAL_SUBS=0x09 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Bearer Service Not Provisioned*/
   AEETSUPPS_ERR_BR_SRV_NOT_PRV  =0x0A | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Tele Service Not Provisioned*/
   AEETSUPPS_ERR_TELE_SRV_NOT_PRV=0x0B | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Illegal Equipment*/
   AEETSUPPS_ERR_ILLEGAL_EQUIP   =0x0C | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Call Barred Error*/
   AEETSUPPS_ERR_CALL_BARRED     =0x0D | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*ILllegal SS Operation*/
   AEETSUPPS_ERR_ILLEGAL_SS_OP=0x10 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*SS Error Status*/
   AEETSUPPS_ERR_SS_ERR_STATUS   =0x11 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Supplemtary Service Not Available*/
   AEETSUPPS_ERR_SS_NOT_AVAIL    =0x12 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Supplementaruy Service Subscription Violation*/
   AEETSUPPS_ERR_SS_SUBR_VIOLATION =0x13 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Supplementary Service Not compatible*/
   AEETSUPPS_ERR_SS_INCOMPATIBLE =0x14 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Facility Not Supported */
   AEETSUPPS_ERR_FACILITY_NOT_SUPP =0x15 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Subscription Absent*/
   AEETSUPPS_ERR_ABSENT_SUBR       =0x1B | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Short Term Denial*/
   AEETSUPPS_ERR_SHORT_TERM_DENIAL=0x1D | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Long Term Denial*/
   AEETSUPPS_ERR_LONG_TERM_DENIAL  =0x1E | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*System Failure*/
   AEETSUPPS_ERR_SYSTEM_FAILURE=0x22 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Missing Information*/
   AEETSUPPS_ERR_DATA_MISSING      =0x23 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Unexpected data value*/
   AEETSUPPS_ERR_UNEXPT_DATA_VALUE =0x24 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Password Registration Failure*/
   AEETSUPPS_ERR_PASSWD_REG_FAILURE   =0x25 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Neagtive Password Check*/
   AEETSUPPS_ERR_NEGATIVE_PASSWD_CHECK=0x26 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Maximum number of password attempts reached*/
   AEETSUPPS_ERR_MAX_PASSWD_ATTEMPTS=0x2C | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Position MEthod Failure*/
   AEETSUPPS_ERR_POS_METHOD_FAILURE=0x36 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Unknown Alphabet*/
   AEETSUPPS_ERR_UNKNOWN_ALPHABET=0x47 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Sent USSD while User was busy*/
   AEETSUPPS_ERR_USSD_BUSY         =0x48 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Rejected By User*/
   AEETSUPPS_ERR_REJ_BY_USER=0x79 | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Rejected By Network*/
   AEETSUPPS_ERR_REJ_BY_NETWORK    =0x7A | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Deflected to Service Scubriber*/
   AEETSUPPS_ERR_DEF_TO_SRV_SUBR   =0x7B | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Special Service Code*/
   AEETSUPPS_ERR_SPECIAL_SRV_CODE  =0x7C | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Invalid Deflected To Number */
   AEETSUPPS_ERR_INVALID_DEF_TO_NUM=0x7D | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),
   /*Maximum Multiparty Calls Exceeded*/
   AEETSUPPS_ERR_MAX_MPTY_EXCEEDED =0x7E | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)), 
   /*Resources Not Avilable*/
   AEETSUPPS_ERR_RES_NOT_AVAILABLE =0x7F | (AEETSUPPS_ERR_BM(AEETSUPPS_ERROR_CODE_TAG)),

   /* MN specific values for ss errors     */
   AEETSUPPS_ERR_MN_INSUFFICIENT_RESOURCES    =0x01 |AEETSUPPS_ERR_BM(AEETSUPPS_MN_SS_ERROR_TAG ), 
   /*Split Multiparty Rejected*/
   AEETSUPPS_ERR_MN_SPLIT_MPTY_REJ            =0x02 |AEETSUPPS_ERR_BM(AEETSUPPS_MN_SS_ERROR_TAG ), 
   AEETSUPPS_ERR_MN_CALL_HOLD_REJ             =0x03 |AEETSUPPS_ERR_BM(AEETSUPPS_MN_SS_ERROR_TAG ), 
   AEETSUPPS_ERR_MN_CALL_RETRIEVE_REJ         =0x04 |AEETSUPPS_ERR_BM(AEETSUPPS_MN_SS_ERROR_TAG ), 
   AEETSUPPS_ERR_MN_NETWORK_NOT_RESPONDING    =0x05 |AEETSUPPS_ERR_BM(AEETSUPPS_MN_SS_ERROR_TAG ), 

   /* General problem codes */
   AEETSUPPS_ERR_UNRECOGNISED_COMPONENT     = 0x00 |AEETSUPPS_ERR_BM(AEETSUPPS_GENERAL_PROBLEM),  
   AEETSUPPS_ERR_MISTYPED_COMPONENT         = 0x01 |AEETSUPPS_ERR_BM(AEETSUPPS_GENERAL_PROBLEM),       
   AEETSUPPS_ERR_BADLY_STRUCTURED_COMPONENT = 0x02 |AEETSUPPS_ERR_BM(AEETSUPPS_GENERAL_PROBLEM),
   AEETSUPPS_ERR_INSUFFICIENT_PRIVILEGE     = 0x03 |AEETSUPPS_ERR_BM(AEETSUPPS_GENERAL_PROBLEM),

   /* Invoke problem codes */
   AEETSUPPS_ERR_DUPLICATE_INVOKE_ID         =0x00 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_UNRECOGNISED_OPERATION      =0x01 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_MISTYPED_PARAMETER          =0x02 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_RESOURCE_LIMITATION         =0x03 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_INITIATING_RELEASE          =0x04 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_UNRECOGNISED_LINKED_ID      =0x05 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_LINKED_RESPONSE_UNEXPECTED  =0x06 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),
   AEETSUPPS_ERR_UNEXPECTED_LINKED_OPERATION =0x07 | AEETSUPPS_ERR_BM(AEETSUPPS_INVOKE_PROBLEM ),

   /* Return Result codes */
   AEETSUPPS_ERR_UNRECOGNISED_INVOKE_ID          =0x00 | AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_RESULT_PROBLEM ),
   AEETSUPPS_ERR_RETURN_RESULT_UNEXPECTED        =0x01 | AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_RESULT_PROBLEM ),
   AEETSUPPS_ERR_RETURN_RESULT_MISTYPED_PARAMETER=0x02 | AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_RESULT_PROBLEM ),

   /* Return Error codes */
   AEETSUPPS_ERR_RETURN_ERROR_UNRECOGNISED_INVOKE_ID =0x00 |AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_ERROR_PROBLEM ), 
   AEETSUPPS_ERR_RETURN_ERROR_UNEXPECTED             =0x01 |AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_ERROR_PROBLEM ), 
   AEETSUPPS_ERR_UNRECOGNISED_ERROR                  =0x02 |AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_ERROR_PROBLEM ), 
   AEETSUPPS_ERR_UNEXPECTED_ERROR                    =0x03 |AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_ERROR_PROBLEM ), 
   AEETSUPPS_ERR_RETURN_ERROR_MISTYPED_PARAMETER     =0x04 |AEETSUPPS_ERR_BM(AEETSUPPS_RETURN_ERROR_PROBLEM ),

   AEETSUPPS_ERR_BIG = 0x10000000
}AEETSuppsErrorType;

/*Supplementary Service Data*/
typedef struct
{
   uint32                           length;
   char                             *data;
} AEETSuppsData;

/* Network SS Codes   
** Note:  Only aoci, aocc are used for now  
*/
typedef enum {
   AEETSUPPS_CODE_CLIP                       =   0x11, /* calling_line_identification_presentation_service */
   AEETSUPPS_CODE_CLIR                       =   0x12, /* calling_line_identification_restriction_service presentation_service */
   AEETSUPPS_CODE_COLP                       =   0x13, /* connected_line_identification_presentation_service */
   AEETSUPPS_CODE_COLR                       =   0x14, /* connected_line identification_restriction_service */
   AEETSUPPS_CODE_ALL_FORWARDING_SS          =   0x20, /* all_call_forwarding */
   AEETSUPPS_CODE_CFU                        =   0x21, /*  call_forwarding_unconditional */
   AEETSUPPS_CODE_ALL_CONDFWD_SS             =   0x28, /* all conditional call forwarding  */
   AEETSUPPS_CODE_CFB                        =   0x29, /* call_forwarding_on_mobile_subscriber_busy */
   AEETSUPPS_CODE_CFNRY                      =   0x2a, /* call_forwarding_on_no_reply */
   AEETSUPPS_CODE_CFNRC                      =   0x2b, /* call_forwarding_on_mobile_subscriber_unreachable */
   AEETSUPPS_CODE_CW                         =   0x41, /* call waiting */
   AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS         =   0x90,
   AEETSUPPS_CODE_BARRING_OUTGOING_CALLS     =   0x91,
   AEETSUPPS_CODE_BAOC                       =   0x92, /* barring_of_all_outgoing_calls */
   AEETSUPPS_CODE_BOIC                       =   0x93, /* barring_of_outgoing_international_calls */
   AEETSUPPS_CODE_BOIC_EXHC                  =   0x94, /* barring_of_outgoing_international_calls_except_those_to_home_plmn */
   AEETSUPPS_CODE_BARRING_OF_INCOMING_CALLS  =   0x99,
   AEETSUPPS_CODE_BAIC                       =   0x9a, /* barring of all incoming calls */
   AEETSUPPS_CODE_BICROAM                    =   0x9b, /* barring of incomming calls when roaming outside home PLMN Country */
   AEETSUPPS_CODE_BIG                        =   0x10000000
} AEETSuppsCodeType;

/* Basic Service Codes */
typedef enum {
   AEETSUPPS_BASIC_SERVICE_ALL = 0,                   /*All Basic Services*/

   /* BEARER-SERVICES */
   AEETSUPPS_BASIC_SERVICE_ALL_BEARER,                /*All Bearer Services*/
   AEETSUPPS_BASIC_SERVICE_ALL_ASYNCHRONOUS,          /*All Asynchrnous Services*/
   AEETSUPPS_BASIC_SERVICE_ALL_SYNCHRONOUS,           /*All Synchronous services*/

   /* TELE-SERVICES */
   AEETSUPPS_BASIC_SERVICE_ALL_TELESERVICES,          /*All TeleServices*/ 
   AEETSUPPS_BASIC_SERVICE_ALL_SPEECH_TRANSMISSION,   /*All Voice*/
   AEETSUPPS_BASIC_SERVICE_ALL_SMS,                   /*All SMS*/
   AEETSUPPS_BASIC_SERVICE_ALL_FAX,                   /*All Fax*/ 
   AEETSUPPS_BASIC_SERVICE_NETWORK_FORMAT = 0x0000000F, /* Standards defined */
   AEETSUPPS_BASIC_SERVICE_BIG = 0x10000000    
}AEETSuppsBasicServiceType;

/*Type of end result data in a transaction.*/
typedef enum
{
   AEETSUPPS_FWD_INFO = 1,
   AEETSUPPS_FWD_FEATURE_LIST_INFO,
   AEETSUPPS_CALL_BARRING_INFO,
   AEETSUPPS_CLI_RESTRICTION_INFO,
   AEETSUPPS_CUG_INFO,
   AEETSUPPS_BSG_LIST_INFO,
   AEETSUPPS_USS_DATA_INFO,
   AEETSUPPS_USSD_DATA_INFO,
   AEETSUPPS_NEW_PASSWD_INFO,
   AEETSUPPS_SS_DATA_INFO,
   AEETSUPPS_SS_STATUS_INFO,
   AEETSUPPS_SS_ERROR_INFO,
   AEETSUPPS_UNKNOWN_CONF_INFO, 
   AEETSUPPS_CC_CAUSE_INFO,
   AEETSUPPS_NO_INFO,
   AEETSUPPS_DEVICE_IMEI,
   AEETSUPPS_PIN_INFO,
   AEETSUPPS_CC_INFO,
   AEETSUPPS_RES_BIG = 0x10000000
} AEETSuppsResultType;

/* Result of pin block/unblock request */
typedef enum {
   AEETSUPPS_PIN_SUCCESS,
   AEETSUPPS_PIN_ERR_INCOMPATIBLE,
   AEETSUPPS_PIN_ERR_BLOCKED,
   AEETSUPPS_PIN_ERR_INCORRECT,
   AEETSUPPS_PIN_ERR_UNKNOWN,
   AEETSUPPS_PIN_ERR_BIG = 0x10000000
}AEETSuppsPinStatus;

/*Operation Status Received from the network*/
typedef struct {
   boolean                         ss_operative;
   boolean                         ss_provisioned;
   boolean                         ss_registered;
   boolean                         ss_active;
} AEETSuppsOpStatus;

/*CLI Restrict Option  Types*/
typedef enum {
   AEETSUPPS_CLI_OPTION_DEFAULT,   /*Default Option*/
   AEETSUPPS_CLI_OPTION_RESTRICT,  /*Restricted Option*/
   AEETSUPPS_CLI_OPTION_OVERRIDE,  /*Override Option*/
   AEETSUPPS_CLI_OPTION_UNKNOWN,   /*Unknown Option*/
   AEETSUPPS_CLI_OPTION_BIG = 0x10000000
} AEETSuppsCliOptionType;

/*Caller ID Display Options*/
typedef enum {
   AEETSUPPS_CLI_RESTRICTION_PERMANENT,
   AEETSUPPS_CLI_RESTRICTION_TEMPORARY_DEFAULT,
   AEETSUPPS_CLI_RESTRICTION_TEMPORARY_ALLOWED,
   AEETSUPPS_CLI_RESTRICTION_BIG = 0x10000000
}AEETSuppsCliRestrictOptionType;

/*CLI Info Received From the Network*/
typedef struct {
   AEETSuppsOpStatus                   ss_status;
   AEETSuppsCliRestrictOptionType      cli_restriction_option;
} AEETSuppsCliRestrictInfo;

/*Susbcription Info Received from the Network*/
typedef struct {
   AEETSuppsCliOptionType                option_type;
   union {
      AEETSuppsCliRestrictOptionType      cli_restrict;
      boolean                             override_category;
   } option;
} AEETSuppsSubscriptionOption;

/* enum for forwarding_reason */
typedef enum
{
   AEETSUPPS_FWD_REASON_MS_NOT_REACHABLE,
   AEETSUPPS_FWD_REASON_MS_BUSY,
   AEETSUPPS_FWD_REASON_NO_REPLY,
   AEETSUPPS_FWD_REASON_UNCONDITIONAL,
   AEETSUPPS_FWD_REASON_FWD_REASON_NONE,
   AEETSUPPS_FWD_REASON_MAX_FWD_REASON = 0x10000000
}AEETSuppsFwdReasonType;

/* connected number  */
typedef struct {
   AEETSuppsFwdReasonType           fwd_reason;
   boolean                          notification_to_fp;
   boolean                          notification_to_cp;
} AEETSuppsForwardingOption;

/*Forwarding Feature Info*/
typedef struct {
   AEETSuppsBasicServiceType        basic_service;
   AEETSuppsOpStatus                ss_status;
   AEETSuppsData                    fwd_to_number;  
   AEETSuppsData                    long_fwd_to_number;
   AEETSuppsForwardingOption        fwd_option;
   uint32                           timer_value;
   AEETSuppsData                    fwd_to_subaddress;
} AEETSuppsFwdFeature;

typedef struct {
   uint32                           length;
   AEETSuppsFwdFeature              fwd_feature[AEETSUPPS_FWD_FEATURE_LIST_SIZE];
} AEETSuppsFwdFeatureList;

typedef struct {
   AEETSuppsCodeType                ss_code;
   AEETSuppsFwdFeatureList          feature_list;
} AEETSuppsForwardingInfo;

/*Call Barring Info*/
typedef struct {
   AEETSuppsBasicServiceType           basic_service;
   AEETSuppsOpStatus                   ss_status;
} AEETSuppsCallBarringFeature;

typedef struct {
   AEETSuppsCodeType                   ss_code;
   uint32                              length;
   AEETSuppsCallBarringFeature         call_barring_feature_list[AEETSUPPS_CALL_BARRING_LIST_SIZE];
} AEETSuppsCallBarringInfo;

/*List of Basic Service Groups*/
typedef struct {
   int                                 length;
   AEETSuppsBasicServiceType           bsg_list[AEETSUPPS_BASIC_SERV_GROUP_LIST_SIZE];
} AEETSuppsBasicServiceGroupListInfo;

/*Supplmentary Service Data*/
typedef struct
{
   uint32                              length;
   AEETSuppsCodeType                   ss_code;
   AEETSuppsOpStatus                   ss_status;
   AEETSuppsSubscriptionOption         ss_subscription_option;
   AEETSuppsBasicServiceGroupListInfo  basic_service_group_list;

} AEETSuppsDataInfo;

typedef enum {
   AEETSUPPS_CCOP_NONE,
   AEETSUPPS_CCOP_NOT_ALLOWED,
   AEETSUPPS_CCOP_ALLOWED,
   AEETSUPPS_CCOP_MODIFIED,
   AEETSUPPS_CCOP_BIG = 0x10000000
} AEETSuppsCCOp;

typedef struct {
   AEETSuppsCCOp      cc_op;
   uint32             alpha_count;    /* count of AECHARs in alpha buffer 
                                      ** including the appended null AECHAR. */
   AECHAR             alpha[AEETSUPPS_MAX_ALPHA_LEN];
   boolean            alpha_valid;    /* alpha_count & alpha buffer valid? */
} AEETSuppsCCResponse;

/* This data structure represents the end result of the transaction. */
typedef struct
{
   AEETSuppsResultType       result_type;
   boolean                   success;
   union
      {
      AEETSuppsForwardingInfo                fwd_info;                           /*AEETSUPPS_FWD_INFO*/
      AEETSuppsFwdFeatureList                fwd_feature_list;                   /*AEETSUPPS_FWD_FEATURE_LIST_INFO*/
      AEETSuppsCallBarringInfo               call_barring_info;                  /*AEETSUPPS_CALL_BARRING_INFO*/
      AEETSuppsCliRestrictInfo               cli_restriction;                    /*AEETSUPPS_CLI_RESTRICTION_INFO*/
      AEETSuppsData                          cug_info;                           /*AEETSUPPS_CUG_INFO*/
      AEETSuppsBasicServiceGroupListInfo     bsg_list_info;                      /*AEETSUPPS_BSG_LIST_INFO*/
      AEETSuppsData                          uss_data_info;                      /*AEETSUPPS_USS_DATA_INFO*/
      AEETSuppsData                          ussd_data_info;                     /*AEETSUPPS_USSD_DATA_INFO*/
      char                                   ss_new_pwd[AEETSUPPS_MAX_PASSWD_LEN];  /*AEETSUPPS_NEW_PWD_INFO*/
      AEETSuppsDataInfo                      ss_data;                            /*AEETSUPPS_SS_DATA_INFO*/
      AEETSuppsErrorType                     ss_error;                           /*AEETSUPPS_SS_ERROR_INFO*/
      AEETSuppsOpStatus                      ss_status;                          /*AEETSUPPS_SS_STATUS_INFO*/
      uint32 /*AEETEndCause*/                cc_cause;                           /*AEETSUPPS_CC_CAUSE_INFO*/
      char                                   imei[AEETSUPPS_MAX_IMEI_LEN];       /*AEETSUPPS_DISPLAY_IMEI */
      AEETSuppsPinStatus                     pin_status;                         /*AEETSUPPS_PIN_INFO*/
      AEETSuppsCCResponse                    cc_info;                            /*AEETSUPPS_CC_INFO*/
   }data;
} AEETSuppsResult;

typedef enum {
   AEETSUPPS_STATE_NONE,                  /* when this transaction is not started. */
   AEETSUPPS_STATE_RUNNING,               /* when mobile is executing the transaction. */
   AEETSUPPS_STATE_NW_INDICATION,         /* network sent a query. */
   AEETSUPPS_STATE_DONE,                  /* transaction is complete or aborted with errors. */

   AEETSUPPS_STATE_BIG = 0x10000000
}AEETSuppsTransState;

/*Supplementary Service Operations*/
typedef enum
{
   AEETSUPPS_REQ_CHANGE_PIN,         /*Change Pin*/                  
   AEETSUPPS_REQ_UNBLOCK_PIN,        /*Unblock Pin*/                 
   AEETSUPPS_REQ_DEVICE_IMEI,        /*Get IMEI*/                    
   AEETSUPPS_REQ_REGISTER,           /*Register Operation*/          
   AEETSUPPS_REQ_ERASE,              /*Erase Operation*/             
   AEETSUPPS_REQ_ACTIVATE,           /*Activate Operation*/          
   AEETSUPPS_REQ_DEACTIVATE,         /*Deactivate Operation*/        
   AEETSUPPS_REQ_INTERROGATE,        /*Interrogate Operation*/       
   AEETSUPPS_REQ_REGISTER_PASSWD,    /*Register the password*/       
   AEETSUPPS_REQ_USS,                /*USSD Request*/                
   AEETSUPPS_REQ_NETWORK_IND,        /*Get Network Indication.*/     
   AEETSUPPS_REQ_CHANGE_PIN2,        /*Change Pin2*/                 
   AEETSUPPS_REQ_UNBLOCK_PIN2,       /*Unblock Pin2*/                
   AEETSUPPS_REQ_BIG =  0x10000000
}AEETSuppsRequestType;

/*Password Information*/
typedef struct {
   char      szCurPasswd[AEETSUPPS_MAX_PASSWD_LEN];
   char      szNewPasswd[AEETSUPPS_MAX_PASSWD_LEN];
   char      szNewPasswdRepeated[AEETSUPPS_MAX_PASSWD_LEN];
}AEETSuppsChangePassword;

/*Registration Information*/
typedef struct {
   AEETSuppsData data;
   uint32    nr_timer;
}AEETSuppsRegistration;

/*Supplementary Service Mobile initiated request*/
typedef struct
{
   AEETSuppsRequestType           operation;
   AEETSuppsCodeType              code;
   AEETSuppsBasicServiceType      basic_service; 

   union {
      AEETSuppsRegistration      reg;
      AEETSuppsChangePassword    passwd;
      AEETSuppsData              ussd;
   }req;

}AEETSuppsRequest;

/* Types of messages Network indication contain */
typedef enum {
   AEETSUPPS_IND_ENTER_PASSWD,                        /* No data */
   AEETSUPPS_IND_ENTER_NEW_PASSWD,                    /* No data */
   AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN,              /* No data */
   AEETSUPPS_IND_FWD_CHECK,
   AEETSUPPS_IND_USS,                                 /* ind_data present */
   AEETSUPPS_IND_USS_NOTIFY,                          /* ind_data present */
   AEETSUPPS_IND_BAD_PASSWD_TRY_AGAIN,                /* No data */  
   AEETSUPPS_IND_BAD_PASSWD_FORMAT_TRY_AGAIN,         /* No data */

   AEETSUPPS_IND_BIG = 0x10000000
} AEETSuppsNetworkIndType;

typedef struct {
   AEETSuppsTransId              transaction_id;
   AEETSuppsNetworkIndType       ind_type; 
}AEETSuppsEventData;

/* USS response types*/
typedef enum
{
   AEETSUPPS_RES_PASSWD,                  /* for one of 
                                          ** AEETSUPPS_IND_ENTER_PASSWD, 
                                          ** AEETSUPPS_IND_ENTER_NEW_PASSWD, 
                                          ** AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN 
                                          ** AEETSUPPS_IND_BAD_PASSWD_TRY_AGAIN
                                          ** AEETSUPPS_IND_BAD_PASSWD_FORMAT_TRY_AGAIN
                                          */                                          
   AEETSUPPS_RES_USS_UNKNOWN_ALPHABET,    /* for AEETSUPPS_IND_USS unknown alphabet */
   AEETSUPPS_RES_USS_BUSY_DISP,           /* for AEETSUPPS_IND_USS display is busy  */
   AEETSUPPS_RES_USS_OK,                  /* ACK for AEETSUPPS_IND_USS */
   AEETSUPPS_RES_USS_NONE,                /* no response */
   AEETSUPPS_RES_USS_BIG = 0x10000000
} AEETSuppsResType;

/*Supplementary Service Response Info*/
typedef struct {
   AEETSuppsResType           response_type;
   AEETSuppsData              response_data;
}AEETSuppsResponse;

/*Supplementary Service Indication sent by the Network */
typedef struct {
   AEETSuppsNetworkIndType      ind_type;
   AEETSuppsData                ind_data;
}AEETSuppsNetworkInd;

typedef enum {
   AEETSUPPS_STATUS_IND,
   AEETSUPPS_STATUS_COMPLETE,

   AEETSUPPS_STATUS_BIG = 0x10000000
}AEETSuppsStatusType;

/* status information. */
typedef struct {
   AEETSuppsStatusType   status;
   
   union {
      AEETSuppsNetworkInd  NetworkInd;          /* An indication or a query from Network */
      AEETSuppsResult      Result;              /* result of the transaction. */
   } data;
}AEETSuppsStatus;

typedef enum {

   AEETSUPPS_MSG_REQUEST,
   AEETSUPPS_MSG_RESPONSE,

   AEETSUPPS_MSG_BIG = 0x10000000

}AEETSuppsMessageType;

typedef struct {
   AEETSuppsMessageType type;
   union {
      AEETSuppsRequest  request;
      AEETSuppsResponse response;
   }data;
} AEETSuppsMessage;

typedef struct {
   AEETSuppsMessage     *pMsg;  /* message being serviced */

   AEETSuppsTransId     transaction_id; /* an id correpsonding to this request */

   AEETSuppsCCResponse  *pCCResponse;
}AEETSuppsMessageEventData;

typedef struct {
   AEETSuppsResult      *pRes;
   AEETSuppsTransId     transaction_id;
}AEETSuppsResultEventData;



/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEETSuppsMessageType

Description:
   Specifies the type of message.

Definition:
typedef enum {
   AEETSUPPS_MSG_REQUEST,
   AEETSUPPS_MSG_RESPONSE,
}AEETSuppsMessageType;

Members:
   AEETSUPPS_MSG_REQUEST   : Message is a request.
   AEETSUPPS_MSG_RESPONSE  : Message is in response to a network indication.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsMessage

Description:
   Represents a message for the transaction.

Definition:
typedef struct {
   AEETSuppsMessageType type;
   union {
      AEETSuppsRequest  request;
      AEETSuppsResponse response;
   }data;
} AEETSuppsMessage;

Members:
   type : The message type. Indicates the valid members of data.
   data.request : specifies a request to process.
   data.response : specifies a response to a network indication.
   
Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsStatus

Description:
   A status result of transaction

Definition:
typedef struct {
   AEETSuppsStatusType   status;
   
   union {
      AEETSuppsNetworkInd  NetworkInd;
      AEETSuppsResult      Result;
   } data;
}AEETSuppsStatus;

Members:
   status : type of status. Indicates the valid members of data.
   data.NetworkInd : specifies the network indication. Mobile must respond to this query.
   data.Result : specifies the data for a completed transaction.
   
Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsStatusType

Description:
   Type of status.

Definition:
typedef enum {
   AEETSUPPS_STATUS_IND,
   AEETSUPPS_STATUS_COMPLETE,
}AEETSuppsStatusType;

Members:
   AEETSUPPS_STATUS_IND : specifies Network indication is present status.
   AEETSUPPS_STATUS_COMPLETE : specifies transaction is complete with a result in status.
   
Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsNetworkInd

Description:
   Data structure representing network indication.

Definition:
typedef struct {
   AEETSuppsNetworkIndType      ind_type;
   AEETSuppsData                ind_data;
}AEETSuppsNetworkInd;

Members:
   ind_type : specifies the type of indication.
   ind_data : the corresponding data.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsRequestType

Description:
   Type of request for AEETSuppsRequest. Depending on type appropriate members of the AEETSuppsRequest
   are used.

Definition:
typedef enum
{
   AEETSUPPS_REQ_CHANGE_PIN,         
   AEETSUPPS_REQ_UNBLOCK_PIN,        
   AEETSUPPS_REQ_DEVICE_IMEI,        
   AEETSUPPS_REQ_REGISTER,           
   AEETSUPPS_REQ_ERASE,              
   AEETSUPPS_REQ_ACTIVATE,           
   AEETSUPPS_REQ_DEACTIVATE,         
   AEETSUPPS_REQ_INTERROGATE,        
   AEETSUPPS_REQ_REGISTER_PASSWD,    
   AEETSUPPS_REQ_USS,                
   AEETSUPPS_REQ_NETWORK_IND,        
   AEETSUPPS_REQ_CHANGE_PIN2,        
   AEETSUPPS_REQ_UNBLOCK_PIN2,       
}AEETSuppsRequestType;

Members:
   AEETSUPPS_REQ_CHANGE_PIN : Change PIN of SIM card. Uses AEETSuppsChangePassword. No network communication involved.
   AEETSUPPS_REQ_UNBLOCK_PIN : Unblocks the PIN of SIM card. Uses AEETSuppsChangePassword. No network communication involved.
   AEETSUPPS_REQ_DEVICE_IMEI : Responds with device IMEI. No network communication involved. Application require access to AEECLSID_GSMCREDENTIALS.
   AEETSUPPS_REQ_REGISTER : Register operation. Uses AEETSuppsRegistration.
   AEETSUPPS_REQ_ERASE : Erase Operation. Uses AEETSuppsCodeType.
   AEETSUPPS_REQ_ACTIVATE : Activate operation. Uses AEETSuppsCodeType and AEETSuppsBasicServiceType
   AEETSUPPS_REQ_DEACTIVATE : Deactivate Operation. Uses AEETSuppsCodeType and AEETSuppsBasicServiceType
   AEETSUPPS_REQ_INTERROGATE : Interrogate Operation. Uses AEETSuppsCodeType and AEETSuppsBasicServiceType
   AEETSUPPS_REQ_REGISTER_PASSWD : Register the password. Uses AEETSuppsCodeType and AEETSuppsChangePassword
   AEETSUPPS_REQ_USS : Unstructured Supplementary Service Data Request. Uses AEETSuppsData.
   AEETSUPPS_REQ_NETWORK_IND : Extract Network Indication. No network communication involved.
   AEETSUPPS_REQ_CHANGE_PIN2 : Change PIN2 of SIM card. Uses AEETSuppsChangePassword. No network communication involved.
   AEETSUPPS_REQ_UNBLOCK_PIN2 : Unblocks the the PIN2 of SIM card. Uses AEETSuppsChangePassword. No network communication involved.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsNetworkIndType

Description:
   Specifies the type of network indication.

Definition:
typedef enum {
   AEETSUPPS_IND_ENTER_PASSWD,
   AEETSUPPS_IND_ENTER_NEW_PASSWD,
   AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN,
   AEETSUPPS_IND_FWD_CHECK,
   AEETSUPPS_IND_USS,
   AEETSUPPS_IND_USS_NOTIFY,
   AEETSUPPS_IND_BAD_PASSWD_TRY_AGAIN,                
   AEETSUPPS_IND_BAD_PASSWD_FORMAT_TRY_AGAIN,     
} AEETSuppsNetworkIndType;

Members:
   AEETSUPPS_IND_ENTER_PASSWD :   network requesting current password.
   AEETSUPPS_IND_ENTER_NEW_PASSWD : Send a new password. mobile intends to change the password.
   AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN : repeat the new password.
   AEETSUPPS_IND_FWD_CHECK :
   AEETSUPPS_IND_USS : USSD message to just prompt the user.
   AEETSUPPS_IND_USS_NOTIFY : USSD message to process.
   AEETSUPPS_IND_BAD_PASSWD_TRY_AGAIN :  Bad password, try again
   AEETSUPPS_IND_BAD_PASSWD_FORMAT_TRY_AGAIN : Format of password is wrong, try again 
   
Comments: 
   None

See Also:
   None

======================================================================= 

AEETSuppsTransId

Description:
   Identifies a supplementary service transaction by ID.

Definition:
typedef uint32 AEETSuppsTransId;

Supported values:
   AEET_SUPPSVC_TRANSID_NEW : to start a new transaction.
   uint32 : obtained from AEET_NMASK_NEW_NI_SUPP_TRANS   

Comments: 
   None

See Also:
   AEET_NMASK_NEW_NI_SUPP_TRANS, IPHONECTL_GetSuppsTrans()

======================================================================= 

AEETSuppsResult

Description:
   Represents end result of the transaction.

Definition:
typedef struct
{
   AEETSuppsResultType       result_type;
   boolean                   success;
   union
      {
      AEETSuppsForwardingInfo                fwd_info;                           
      AEETSuppsFwdFeatureList                fwd_feature_list;                   
      AEETSuppsCallBarringInfo               call_barring_info;                  
      AEETSuppsCliRestrictInfo               cli_restriction;                    
      AEETSuppsData                          cug_info;                           
      AEETSuppsBasicServiceGroupListInfo     bsg_list_info;                      
      AEETSuppsData                          uss_data_info;                      
      AEETSuppsData                          ussd_data_info;                     
      char                                   ss_new_pwd[AEETSUPPS_MAX_PASSWD_LEN];
      AEETSuppsDataInfo                      ss_data;                            
      AEETSuppsErrorType                     ss_error;                           
      AEETSuppsOpStatus                      ss_status;                          
      uint32                                 cc_cause;                           
      char                                   imei[AEETSUPPS_MAX_IMEI_LEN];       
      AEETSuppsPinStatus                     pin_status;                         
      AEETSuppsCCResponse                    cc_info;
   }data;
} AEETSuppsResult;

Members:
   result_type : identifies the data member.
   success : Whether or not this is a successful transaction.
   fwd_info : available with result type AEETSUPPS_FWD_INFO
   fwd_feature_list : available with result type AEETSUPPS_FWD_FEATURE_LIST_INFO
   call_barring_info : available with result type AEETSUPPS_CALL_BARRING_INFO
   cli_restriction : available with result type AEETSUPPS_CLI_RESTRICTION_INFO
   cug_info : for result type AEETSUPPS_CUG_INFO
   bsg_list_info : for result type AEETSUPPS_BSG_LIST_INFO
   uss_data_info : for result type AEETSUPPS_USS_DATA_INFO
   ussd_data_info : for result type AEETSUPPS_USSD_DATA_INFO
   ss_new_pwd : for result type AEETSUPPS_NEW_PWD_INFO
   ss_data : for result type AEETSUPPS_SS_DATA_INFO
   ss_error : for result type AEETSUPPS_SS_ERROR_INFO
   ss_status : for result type AEETSUPPS_SS_STATUS_INFO
   cc_cause : for result type AEETSUPPS_CC_CAUSE_INFO
   imei : for result type AEETSUPPS_DISPLAY_IMEI
   pin_status : for result type AEETSUPPS_PIN_INFO
   cc_info : for result type AEETSUPPS_CC_INFO

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsForwardingInfo

Description:
   Specifies the forwarding information.

Definition:
typedef struct {
   AEETSuppsCodeType                ss_code;
   AEETSuppsFwdFeatureList          feature_list;
} AEETSuppsForwardingInfo;

Members:
   ss_code : Call forwarding code.
   feature_list : forwarding information for one or more basic services.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsFwdFeatureList

Description:
   Array of forward features.

Definition:
typedef struct {
   uint32                           length;
   AEETSuppsFwdFeature              fwd_feature[AEETSUPPS_FWD_FEATURE_LIST_SIZE];
} AEETSuppsFwdFeatureList;

Members:
   length : number of elements in fwd_feature array
   fwd_feature : array of forward features.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsFwdFeature

Description:
   Forwarding feature information.

Definition:
typedef struct {
   AEETSuppsBasicServiceType        basic_service;
   AEETSuppsOpStatus                ss_status;
   AEETSuppsData                    fwd_to_number;  
   AEETSuppsData                    long_fwd_to_number;
   AEETSuppsForwardingOption        fwd_option;
   uint32                           timer_value;
   AEETSuppsData                    fwd_to_subaddress;
} AEETSuppsFwdFeature;

Members:
   basic_service : basic bearer service.
   ss_status : operation status received from network.
   fwd_to_number : forwarding directory number.
   long_fwd_to_number : long forwarding directory number.
   fwd_option : forwarding option.
   timer_value : no reply condition time.
   fwd_to_subaddress : forwarding number subaddress.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCallBarringInfo

Description:
   Call barring feature information.

Definition:
typedef struct {
   AEETSuppsCodeType                   ss_code;
   uint32                              length;
   AEETSuppsCallBarringFeature         call_barring_feature_list[AEETSUPPS_CALL_BARRING_LIST_SIZE];
} AEETSuppsCallBarringInfo;

Members:
   ss_code : Call barring code of AEETSuppsCodeType
   length : number of elements in call_barring_feature list.
   call_barring_feature_list : barring information for one or more basic services.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCallBarringFeature

Description:
   Call barring feature detail.

Definition:
typedef struct {
   AEETSuppsBasicServiceType           basic_service;
   AEETSuppsOpStatus                   ss_status;
} AEETSuppsCallBarringFeature;

Members:
   basic_service : basic bearer service
   ss_status : Operation status received from network.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCliRestrictInfo

Description:
   Calling line identification restrict info.

Definition:
typedef struct {
   AEETSuppsOpStatus                   ss_status;
   AEETSuppsCliRestrictOptionType      cli_restriction_option;
} AEETSuppsCliRestrictInfo;

Members:
   ss_status : Operation status received from network.
   cli_restriction_option : CLI status.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsBasicServiceGroupListInfo

Description:
   List of basic service groups.

Definition:
typedef struct {
   int                                 length;
   AEETSuppsBasicServiceType           bsg_list[AEETSUPPS_BASIC_SERV_GROUP_LIST_SIZE];
} AEETSuppsBasicServiceGroupListInfo;

Members:
   length : number of elements in bsg_list array.
   bsg_list : basic service (bearer and teleservice)

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsDataInfo

Description:
   Data structure captures the response not explained in other members of AEETSuppsResult::data.

Definition:
typedef struct {
   uint32                              length;
   AEETSuppsCodeType                   ss_code;
   AEETSuppsOpStatus                   ss_status;
   AEETSuppsSubscriptionOption         ss_subscription_option;
   AEETSuppsBasicServiceGroupListInfo  basic_service_group_list;
} AEETSuppsDataInfo;

Members:
   length : sizeof the AEETSuppsDataInfo.
   ss_code : code.
   ss_status : Operation status received from network.
   ss_subscription_option : Subscription info received from network.
   basic_service_group_list : list of basic service groups. 

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsPinStatus

Description:
   Status indication for pin change or pin unblock requests.

Definition:
typedef enum {
   AEETSUPPS_PIN_SUCCESS,
   AEETSUPPS_PIN_ERR_INCOMPATIBLE,
   AEETSUPPS_PIN_ERR_BLOCKED,
   AEETSUPPS_PIN_ERR_INCORRECT,
   AEETSUPPS_PIN_ERR_UNKNOWN,
}AEETSuppsPinStatus;

Members:
   AEETSUPPS_PIN_SUCCESS : operation successful
   AEETSUPPS_PIN_ERR_INCOMPATIBLE : new pin is not compatible.
   AEETSUPPS_PIN_ERR_BLOCKED : requires unblock.
   AEETSUPPS_PIN_ERR_INCORRECT : given pin is not correct.
   AEETSUPPS_PIN_ERR_UNKNOWN : all other errors.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCCOp

Description:
   Call Control Operation result.

Definition:

typedef enum {
   AEETSUPPS_CCOP_NONE,
   AEETSUPPS_CCOP_NOT_ALLOWED,
   AEETSUPPS_CCOP_ALLOWED,
   AEETSUPPS_CCOP_MODIFIED
} AEETSuppsCCOp;

Members:
   AEETSUPPS_CCOP_NONE : Call control was not performed
   AEETSUPPS_CCOP_NOT_ALLOWED : Call Control responded rejecting 
                                the operation. 
   AEETSUPPS_CCOP_ALLOWED : call control responded allowing the operation.
   AEETSUPPS_CCOP_MODIFIED : Call control responded allowing the operation 
                             with modification.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCCResponse

Description:
   Status indication for Call Control.

Definition:
typedef struct {
   AEETSuppsCCOp      cc_op;
   uint32             alpha_count;
   AECHAR             alpha[AEETSUPPS_MAX_ALPHA_LEN];
   boolean            alpha_valid;
} AEETSuppsCCResponse;

Members:
   cc_op : operation status
   alpha_count : count of AECHARs in alpha buffer 
                 including the appended null AECHAR.
   alpha : alpha buffer.
   alpha_valid: boolean value to indicate if the alpha_count
                and alpha buffer are valid.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsMessageEventData

Description:
   Event data with Supplementary service message issued to network

Definition:
typedef struct {
   AEETSuppsMessage     *pMsg;

   AEETSuppsTransId     transaction_id;

   AEETSuppsCCResponse  *pCCResponse;
}AEETSuppsMessageEventData;

Members:
   pMsg : message being serviced
   transaction_id : an id correpsonding to this request
   pCCResponse : Points to call control response.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsResultEventData

Description:
   Event data with Supplementary service message result.

Definition:
typedef struct {
   AEETSuppsResult      *pRes;

   AEETSuppsTransId     transaction_id;

}AEETSuppsResultEventData;


Members:
   pRes : Points to result from the supplementary service operation
   transaction_id : an id correpsonding to this request

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsTransState

Description:
   State of transaction.

Definition:
typedef enum {
   AEETSUPPS_STATE_NONE,
   AEETSUPPS_STATE_RUNNING,
   AEETSUPPS_STATE_NW_INDICATION,
   AEETSUPPS_STATE_DONE,
}AEETSuppsTransState;

Members:
   AEETSUPPS_STATE_NONE : When the transaction is not started.
   AEETSUPPS_STATE_RUNNING : when device is executing the transaction
   AEETSUPPS_STATE_NW_INDICATION : network sent a query
   AEETSUPPS_STATE_DONE : transaction is complete or aborted with errors.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsChangePassword

Description:
   Used on requests that register password, change pin or unblock pin.

Definition:
typedef struct {
   char      szCurPasswd[AEETSUPPS_MAX_PASSWD_LEN];
   char      szNewPasswd[AEETSUPPS_MAX_PASSWD_LEN];
   char      szNewPasswdRepeated[AEETSUPPS_MAX_PASSWD_LEN];
}AEETSuppsChangePassword;

Members:
   szCurPasswd : current password
   szNewPasswd : new password.
   szNewPasswdRepeated : new password repeated.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsRegistration

Description:
   Additional data for Register SS operation.

Definition:
typedef struct {
   AEETSuppsData data;
   uint32    nr_timer;
}AEETSuppsRegistration;

Members:
   data : forwarded to number for requests with ss_code indicating call forwarding.
   nr_timer : no reply condition time.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsData

Description:
   Represents a data buffer.

Definition:
typedef struct {
   uint32                           length;
   char                             *data;
} AEETSuppsData;

Members:
   length : number of octets at address given by data.
   data : address of buffer.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsCodeType

Description:
   Supplemetary service code.

Definition:
typedef enum {
   AEETSUPPS_CODE_CLIP,
   AEETSUPPS_CODE_CLIR,
   AEETSUPPS_CODE_COLP,
   AEETSUPPS_CODE_COLR,
   AEETSUPPS_CODE_ALL_FORWARDING_SS,
   AEETSUPPS_CODE_CFU,
   AEETSUPPS_CODE_ALL_CONDFWD_SS,
   AEETSUPPS_CODE_CFB,
   AEETSUPPS_CODE_CFNRY,
   AEETSUPPS_CODE_CFNRC,
   AEETSUPPS_CODE_CW,
   AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS,
   AEETSUPPS_CODE_BARRING_OUTGOING_CALLS,
   AEETSUPPS_CODE_BAOC,
   AEETSUPPS_CODE_BOIC,
   AEETSUPPS_CODE_BOIC_EXHC,
   AEETSUPPS_CODE_BARRING_OF_INCOMING_CALLS,
   AEETSUPPS_CODE_BAIC,
   AEETSUPPS_CODE_BICROAM,
} AEETSuppsCodeType;

Members:
   AEETSUPPS_CODE_CLIP : Calling Line Identification Presentation service.
   AEETSUPPS_CODE_CLIR : Calling Line Identification Restriction service
   AEETSUPPS_CODE_COLP : Connected line identification Presentation service
   AEETSUPPS_CODE_COLR : Connected Line identification Restriction service
   AEETSUPPS_CODE_ALL_FORWARDING_SS : All call forwarding
   AEETSUPPS_CODE_CFU : Call Forwarding Unconditional
   AEETSUPPS_CODE_ALL_CONDFWD_SS : All Conditional Call Forwarding
   AEETSUPPS_CODE_CFB : Call Forwarding ON when subscriber is busy.
   AEETSUPPS_CODE_CFNRY : Call Forwarding ON when no reply
   AEETSUPPS_CODE_CFNRC : Call Forwarding ON when mobile subscriber is unreachable
   AEETSUPPS_CODE_CW : Call Waiting
   AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS : All Call Restriction
   AEETSUPPS_CODE_BARRING_OUTGOING_CALLS : Barring Outgoing Calls.
   AEETSUPPS_CODE_BAOC : Barring All Outgoing Calls.
   AEETSUPPS_CODE_BOIC : Barring Outgoing International Calls.
   AEETSUPPS_CODE_BOIC_EXHC : Barring Outgoing International Calls Except thos to home PLMN
   AEETSUPPS_CODE_BARRING_OF_INCOMING_CALLS : Barring of Incoming Calls.
   AEETSUPPS_CODE_BAIC : Barring of All Incoming Calls.
   AEETSUPPS_CODE_BICROAM : Barring of incoming calls when roaming outside home PLMN country.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsBasicServiceType

Description:
   Describes Basic service including teleservice.

Definition:
typedef enum {
   AEETSUPPS_BASIC_SERVICE_ALL = 0,
   AEETSUPPS_BASIC_SERVICE_ALL_BEARER,
   AEETSUPPS_BASIC_SERVICE_ALL_ASYNCHRONOUS,
   AEETSUPPS_BASIC_SERVICE_ALL_SYNCHRONOUS,
   AEETSUPPS_BASIC_SERVICE_ALL_TELESERVICES,
   AEETSUPPS_BASIC_SERVICE_ALL_SPEECH_TRANSMISSION,
   AEETSUPPS_BASIC_SERVICE_ALL_SMS,
   AEETSUPPS_BASIC_SERVICE_ALL_FAX,
   AEETSUPPS_BASIC_SERVICE_NETWORK_FORMAT,
}AEETSuppsBasicServiceType;

Members:
   AEETSUPPS_BASIC_SERVICE_ALL : All Basic services
   AEETSUPPS_BASIC_SERVICE_ALL_BEARER : All Bearer services
   AEETSUPPS_BASIC_SERVICE_ALL_ASYNCHRONOUS : All Asynchronous services
   AEETSUPPS_BASIC_SERVICE_ALL_SYNCHRONOUS : All Synchronous services.
   AEETSUPPS_BASIC_SERVICE_ALL_TELESERVICES : All Teleservices
   AEETSUPPS_BASIC_SERVICE_ALL_SPEECH_TRANSMISSION : All Voice
   AEETSUPPS_BASIC_SERVICE_ALL_SMS : All SMS
   AEETSUPPS_BASIC_SERVICE_ALL_FAX : All FAX
   AEETSUPPS_BASIC_SERVICE_NETWORK_FORMAT : A value greater than which represents 
                  a specific format representing Basic service or Teleservice codes.
                  If bit 16 is clear, lower half word represent basic service code.
                  If bit 16 is set, lower half word represet Teleservice code.
                  eg : 0x11 represents Asynchronous 300bps.
                  eg : 0x10011 represents Teleservice 11 for Telephony.
                  eg : 0x10012 represents Teleservice 12 for emergency.
                  Refer to 3GPP specs for Basic service codes and Teleservice codes
                  (eg : 3GPP TS 29.002 version 5.1.0 Release 5; Sections 17.7.9 and
                  section 17.7.10)

Comments: 
   None

See Also:
   None

=======================================================================
*/

#endif /* !AEETSUPPSDEF_H */

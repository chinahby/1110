
#ifndef AEESUPPSVC_H
#define AEESUPPSVC_H

/*============================================================================
FILE:  AEESuppSvc.h

SERVICES:  AEE Supplimentary Services Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc. for AEE SuppSvc/

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEESuppSvc.h#35 $

when       who     what, where, why
-------    ---     ----------------------------------------------------------
12/15/06   alb     Changed prototype of ISUPPSVC_StartSS to indicate whether
                   FDN check is required or not.
12/15/06   alb     Changed AEESUPPSVC_MAXNO_FORWARD_TO_NUMBER to 80 to 
                   accomodate GCF 27.22.4.11.1 Seq 1.4.
09/01/06   alb     Add L3_data to AEESUPPSVC_EVENT_DATA.
06/22/06   ak      Length reverted back to since max BCD octets can be ten.
06/15/06   ak      Length inline with GCF test case 
12/05/05   sun     Added new Opt for Not Displaying Strings
09/29/05   RI      Changed MAX USSD length to 183 char.
06/28/05   ak      Added support for FDN verification
06/07/05   sun     Added support for CCBS
06/06/05   aps     Added Supplementary Service string return code.
05/31/05   RI      Changed the Max USSD length.
03/31/05   RI      Added DCS field in SS response and SS indication.
03/01/05   SUN     Added support for UCS2
03/01/05   SUN     Added more Release Operations
01/12/05   SUN     Added DCS info 
01/07/05   SUN     Added the CDA/CDS Data Services
09/27/04   SUN     Added Support for CPHS
09/16/04   SUN     Fixed SS Strings
05/07/04   sun     Added new SuppSvcOpt - ALPHA
01/19/04   sun     Added SuppSvcOpts
12/17/03   sun     Added CC Cause
11/21/03   sun     Removed Wizard APIs
10/24/03   sun     Added new Functions
10/16/03   sun     Moved ss_code to Event Data Structure
10/16/03   sun     Added new Fwd Reason Type, Modified AEESuppSvcCugInfo
10/14/03   ram     Added AEESUPPSVC_MAX_USS_LENGTH constant
10/03/03   sun     Added invoke_id to EventData struct
09/19/03   sun     Code Review Changes
09/04/03   sun     Removed AEESuppSys, Code Review Changes
08/22/03   sun     Added support for event_data
08/21/03   sun     Fixed Password Code
08/18/03   sun     Added Support for all Supplementary services
04/03/03   att     Initial version.
=============================================================================*/


#include "AEE.h"
#include "AEEComdef.h"
#include "AEEWebOpts.h"

/* Class Ids
*/
#include "SUPPSVC.BID"
#include "SUPPSVC_NOTIFIER.BID"
#include "SUPPSVC_MODEL.BID"
#include "SUPPSVCOPTS.BID"      /* AEECLSID_SUPPSVCOPTS */


#define AEESUPPSVC_MAX_USS_LENGTH              183 /*Max num of characters in the USSD string encoded in
                                                    160 byte Def 7 bit Alphabet*/
#define AEESUPPSVC_MAXNO_FORWARD_TO_NUMBER     80 /* Max forward to addr.  I changed this to 80 to match */
                                                  /* the number of dialed digits supported in cm/icm */
                                                  /* 20 was not sufficient to pass GCF 27.22.4.11.1 Seq 1.4 */
#define AEESUPPSVC_FWD_FEATURE_LIST_SIZE       13 /*Max Num of Fwd To Numbers*/
#define AEESUPPSVC_CALL_BARRING_LIST_SIZE      13 /*Max Num of Call Barred Numbers*/
#define AEESUPPSVC_BASIC_SERV_GROUP_LIST_SIZE  13 /*Max Number of Basic Srv Groups*/
#define AEESUPPSVC_CUG_INDICATOR_SIZE          2  /*Max Number of CUG Indicators*/
#define AEESUPPSVC_CUG_INTERLOCK_SIZE          4  /*Max number of CUG Interlock*/
#define AEESUPPSVC_CUG_SUBSCRIPTION_LIST_SIZE  10 /*Max number of CUG susbcriptions*/
#define AEESUPPSVC_CUG_FEATURE_LIST_SIZE       13 /* Max number of CUG feature*/ 
#define AEESUPPSVC_MAX_PWD_CHAR                4  /* Max szied password*/
#define AEESUPPSVC_MAX_L3_DATA_LENGTH         260 /* Max number of raw L3 data bytes
                                                     should be same as
                                                     CM_MAX_SUPS_PARAMETER_LENGTH */
#define AEESUPPSVC_BM( val ) (val << 16)


/*===========================================================================
      
                    ISUPPSVCOpts Definitions

===========================================================================*/

/* IWebOpts ID space used by ISUPPSVCOPTs */ 
#define SUPPSVCOPT_OFFSET         0x1100
#define SUPPSVCOPT_SIZE           0x00FF

/* NULL Terminated AECHAR strings */
/* [0x00011100, 0x000111FF] */
#define SUPPSVCOPT_SZ             (WEBOPT_SZ + SUPPSVCOPT_OFFSET) 
#define SUPPSVCOPT_SZ_LAST        (SUPPSVCOPT_SZ + SUPPSVCOPT_SIZE) 

/* 32-bit quantities */
/* [0x00021100, 0x000211FF] */
#define SUPPSVCOPT_32BIT          (WEBOPT_32BIT + SUPPSVCOPT_OFFSET)
#define SUPPSVCOPT_32BIT_LAST     (SUPPSVCOPT_32BIT + SUPPSVCOPT_SIZE)

/* Interface pointers */
/* [0x00031100, 0x000311FF] */
#define SUPPSVCOPT_INTERFACE      (WEBOPT_INTERFACE + SUPPSVCOPT_OFFSET)
#define SUPPSVCOPT_INTERFACE_LAST (SUPPSVCOPT_INTERFACE + SUPPSVCOPT_SIZE)

/* Variable length buffer. First two bytes give the length of the following 
   data. The length is little-endian, low byte first. */
/* [0x00041100, 0x000411FF] */
#define SUPPSVCOPT_VARBUFFER      (WEBOPT_VARBUFFER + SUPPSVCOPT_OFFSET)
#define SUPPSVCOPT_VARBUFFER_LAST (SUPPSVCOPT_VARBUFFER + SUPPSVCOPT_SIZE)

/* ASN.1 data */
/* [0x00051100, 0x000511FF] */
#define SUPPSVCOPT_ASN1           (WEBOPT_ASN1 + SUPPSVCOPT_OFFSET)
#define SUPPSVCOPT_ASN1_LAST      (SUPPSVCOPT_ASN1 + SUPPSVCOPT_SIZE)

/* low 12 bits of the id specify length of the option, in bytes */
/* [0x11100000, 0x111FF000] */
#define SUPPSVCOPT_VARIABLE       (WEBOPT_VARIABLE + (SUPPSVCOPT_OFFSET << 3))
#define SUPPSVCOPT_VARIABLE_LAST  (SUPPSVCOPT_VARIABLE + (SUPPSVCOPT_SIZE << 3))


/* Call options supported by ICM */

#define AEESUPPSVC_SUPPSVCOPT_TON       (SUPPSVCOPT_32BIT)
  /* AEECMNumberType: */

#define AEESUPPSVC_SUPPSVCOPT_NPI       (SUPPSVCOPT_32BIT+1)
  /* AEECMNumberPlan: */ 

#define AEESUPPSVC_SUPPSVCOPT_ALPHA      (SUPPSVCOPT_32BIT+2)

#define AEESUPPSVC_SUPPSVCOPT_DCS       (SUPPSVCOPT_32BIT+3)

#define AEESUPPSVC_SUPPSVCOPT_NO_DISPLAY (SUPPSVCOPT_32BIT+4)

#define AEESUPPSVC_MAX_CCBS_FEATURE_SIZE      5


/*Supplementary Service Operations*/
typedef enum
{
  AEESUPPSVC_CHANGE_PIN,        /*Change Pin*/
  AEESUPPSVC_UNBLOCK_PIN,       /*Unblock Pin*/
  AEESUPPSVC_DISPLAY_IMEI,      /*Display IMEI*/
  AEESUPPSVC_REGISTER_SS,       /*Register Operation*/
  AEESUPPSVC_ERASE_SS,          /*Erase Operation*/
  AEESUPPSVC_ACTIVATE_SS,       /*Activate Operation*/
  AEESUPPSVC_DEACTIVATE_SS,     /*Deactivate Operation*/
  AEESUPPSVC_INTERROGATE_SS,    /*Interrogate Operation*/ 
  AEESUPPSVC_REGISTER_PASSWD,   /*Register the password*/
  AEESUPPSVC_USS_REQ,           /*USSD Request*/
  AEESUPPSVC_GET_PASSWD,        /*Get Password from Client*/
  AEESUPPSVC_PROCESS_USSD_DATA, /*Process USSD*/ 
  AEESUPPSVC_FWD_CHECK_SS_IND,  /*Forward Check Operation*/
  AEESUPPSVC_PROCESS_USSD_REQ,  /*Process USSD Request*/ 
  AEESUPPSVC_USS_NOTIFY,        /*USSD Notify*/
  AEESUPPSVC_NULL_SS_OPERATION, /*NULL*/ 
  AEESUPPSVC_MAX_SS_OPERATION =  0x10000000
}AEESuppSvcSSOperationType;


/************************************************************************
*                                                                       *
*               Basic Service Codes                                     *
*                                                                       *
*************************************************************************
*/
typedef enum
{
  AEESUPPSVC_ALL_SERVICES =0, /*All Basic Services*/
/*
** BEARER-SERVICES
*/
  AEESUPPSVC_ALL_BEARER_SERVICES,       /*All Bearer Services*/
  AEESUPPSVC_ALL_ASYNCHRONOUS_SERVICES, /*All Asynchrnous Services*/
  AEESUPPSVC_ALL_SYNCHRONOUS_SERVICES,  /*All Synchronous services*/
/*
** TELE-SERVICES
*/
  AEESUPPSVC_ALL_TELESERVICES,                 /*All TeleServices*/ 
  AEESUPPSVC_ALL_SPEECH_TRANSMISSION_SERVICES, /*All Voice*/
  AEESUPPSVC_ALL_SMS_SERVICES,                 /*All SMS*/
  AEESUPPSVC_ALL_FAX_TRANSMISSION_SERVICES,    /*All Fax*/ 
  AEESUPPSVC_ALL_DATA_CDA_SERVICES,
  AEESUPPSVC_ALL_DATA_CDS_SERVICES,
  AEESUPPSVC_MAX_BASIC_SERVICE = 0x10000000    
}AEESuppSvcBasicServiceType;

/*Error Code Tags*/
typedef enum
{
  AEESUPPSVC_ERROR_CODE_TAG=0x02,
  AEESUPPSVC_MN_SS_ERROR_TAG=0xc0,  /* used by MN -> network */
  AEESUPPSVC_GENERAL_PROBLEM=0x80,
  AEESUPPSVC_INVOKE_PROBLEM,
  AEESUPPSVC_RETURN_RESULT_PROBLEM,
  AEESUPPSVC_RETURN_ERROR_PROBLEM,
  AEESUPPSVC_MAX_CODE_TAG = 0x10000000
}AEESuppSvcErrorCodeTagType;


/* SS error codes */

typedef enum
{
  AEESUPPSVC_NO_ERR            = 0x00 | (AEESUPPSVC_BM(0x00)),
  /*Unknown Subscriber Error*/
  AEESUPPSVC_UNKNOWN_SUBR=0x01 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Illegal Subscriber*/
  AEESUPPSVC_ILLEGAL_SUBS=0x09 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Bearer Service Not Provisioned*/
  AEESUPPSVC_BR_SRV_NOT_PRV  =0x0A | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Tele Service Not Provisioned*/
  AEESUPPSVC_TELE_SRV_NOT_PRV=0x0B | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Illegal Equipment*/
  AEESUPPSVC_ILLEGAL_EQUIP   =0x0C | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Call Barred Error*/
  AEESUPPSVC_CALL_BARRED     =0x0D | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*ILllegal SS Operation*/
  AEESUPPSVC_ILLEGAL_SS_OP=0x10 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*SS Error Status*/
  AEESUPPSVC_SS_ERR_STATUS   =0x11 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Supplemtary Service Not Available*/
  AEESUPPSVC_SS_NOT_AVAIL    =0x12 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Supplementaruy Service Subscription Violation*/
  AEESUPPSVC_SS_SUBR_VIOLATION =0x13 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Supplementary Service Not compatible*/
  AEESUPPSVC_SS_INCOMPATIBLE =0x14 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Facility Not Supported */
  AEESUPPSVC_FACILITY_NOT_SUPP =0x15 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Subscription Absent*/
  AEESUPPSVC_ABSENT_SUBR       =0x1B | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Short Term Denial*/
  AEESUPPSVC_SHORT_TERM_DENIAL=0x1D | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Long Term Denial*/
  AEESUPPSVC_LONG_TERM_DENIAL  =0x1E | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*System Failure*/
  AEESUPPSVC_SYSTEM_FAILURE=0x22 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Missing Information*/
  AEESUPPSVC_DATA_MISSING      =0x23 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Unexpected data value*/
  AEESUPPSVC_UNEXPT_DATA_VALUE =0x24 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Password Registration Failure*/
  AEESUPPSVC_PWD_REG_FAILURE   =0x25 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Neagtive Password Check*/
  AEESUPPSVC_NEGATIVE_PWD_CHECK=0x26 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Maximum number of pwd attempts reached*/
  AEESUPPSVC_MAX_PWD_ATTEMPTS=0x2C | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Position MEthod Failure*/
  AEESUPPSVC_POS_METHOD_FAILURE=0x36 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Unknown Alphabet*/
  AEESUPPSVC_UNKNOWN_ALPHABET=0x47 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Sent USSD while User was busy*/
  AEESUPPSVC_USSD_BUSY         =0x48 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Rejected By User*/
  AEESUPPSVC_REJ_BY_USER=0x79 | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Rejected By Network*/
  AEESUPPSVC_REJ_BY_NETWORK    =0x7A | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Deflected to Service Scubriber*/
  AEESUPPSVC_DEF_TO_SRV_SUBR   =0x7B | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Special Service Code*/
  AEESUPPSVC_SPECIAL_SRV_CODE  =0x7C | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Invalid Deflected To Number */
  AEESUPPSVC_INVALID_DEF_TO_NUM=0x7D | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),
  /*Maximum Multiparty Calls Exceeded*/
  AEESUPPSVC_MAX_MPTY_EXCEEDED =0x7E | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)), 
  /*Resources Not Avilable*/
  AEESUPPSVC_RES_NOT_AVAILABLE =0x7F | (AEESUPPSVC_BM(AEESUPPSVC_ERROR_CODE_TAG)),

/* MN specific values for ss errors     */
   AEESUPPSVC_MN_INSUFFICIENT_RESOURCES    =0x01 |AEESUPPSVC_BM(AEESUPPSVC_MN_SS_ERROR_TAG ), 
   /*Split Multiparty Rejected*/
   AEESUPPSVC_MN_SPLIT_MPTY_REJ            =0x02 |AEESUPPSVC_BM(AEESUPPSVC_MN_SS_ERROR_TAG ), 
   AEESUPPSVC_MN_CALL_HOLD_REJ             =0x03 |AEESUPPSVC_BM(AEESUPPSVC_MN_SS_ERROR_TAG ), 
   AEESUPPSVC_MN_CALL_RETRIEVE_REJ         =0x04 |AEESUPPSVC_BM(AEESUPPSVC_MN_SS_ERROR_TAG ), 
   AEESUPPSVC_MN_NETWORK_NOT_RESPONDING    =0x05 |AEESUPPSVC_BM(AEESUPPSVC_MN_SS_ERROR_TAG ), 

/* General problem codes */
  AEESUPPSVC_UNRECOGNISED_COMPONENT     = 0x00 |AEESUPPSVC_BM(AEESUPPSVC_GENERAL_PROBLEM),  
  AEESUPPSVC_MISTYPED_COMPONENT         = 0x01 |AEESUPPSVC_BM(AEESUPPSVC_GENERAL_PROBLEM),       
  AEESUPPSVC_BADLY_STRUCTURED_COMPONENT = 0x02 |AEESUPPSVC_BM(AEESUPPSVC_GENERAL_PROBLEM),

/* Invoke problem codes */
  AEESUPPSVC_DUPLICATE_INVOKE_ID         =0x00 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_UNRECOGNISED_OPERATION      =0x01 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_MISTYPED_PARAMETER          =0x02 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_RESOURCE_LIMITATION         =0x03 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_INITIATING_RELEASE          =0x04 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_UNRECOGNISED_LINKED_ID      =0x05 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_LINKED_RESPONSE_UNEXPECTED  =0x06 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),
  AEESUPPSVC_UNEXPECTED_LINKED_OPERATION =0x07 | AEESUPPSVC_BM(AEESUPPSVC_INVOKE_PROBLEM ),

/* Return Result codes */
  AEESUPPSVC_UNRECOGNISED_INVOKE_ID          =0x00 | AEESUPPSVC_BM(AEESUPPSVC_RETURN_RESULT_PROBLEM ),
  AEESUPPSVC_RETURN_RESULT_UNEXPECTED        =0x01 | AEESUPPSVC_BM(AEESUPPSVC_RETURN_RESULT_PROBLEM ),
  AEESUPPSVC_RETURN_RESULT_MISTYPED_PARAMETER=0x02 | AEESUPPSVC_BM(AEESUPPSVC_RETURN_RESULT_PROBLEM ),

/* Return Error codes */
  AEESUPPSVC_RETURN_ERROR_UNRECOGNISED_INVOKE_ID =0x00 |AEESUPPSVC_BM(AEESUPPSVC_RETURN_ERROR_PROBLEM ), 
  AEESUPPSVC_RETURN_ERROR_UNEXPECTED             =0x01 |AEESUPPSVC_BM(AEESUPPSVC_RETURN_ERROR_PROBLEM ), 
  AEESUPPSVC_UNRECOGNISED_ERROR                  =0x02 |AEESUPPSVC_BM(AEESUPPSVC_RETURN_ERROR_PROBLEM ), 
  AEESUPPSVC_UNEXPECTED_ERROR                    =0x03 |AEESUPPSVC_BM(AEESUPPSVC_RETURN_ERROR_PROBLEM ), 
  AEESUPPSVC_RETURN_ERROR_MISTYPED_PARAMETER     =0x04 |AEESUPPSVC_BM(AEESUPPSVC_RETURN_ERROR_PROBLEM ),

  AEESUPPSVC_MAX_SS_ERROR = 0x10000000
}AEESuppSvcSSErrorType;


/*CLI Restrict Option  Types*/
typedef enum
{
  AEESUPPSVC_CLI_DEFAULT_OPTION,   /*Default Option*/
  AEESUPPSVC_CLI_RESTRICT_OPTION,  /*Restricted Option*/
  AEESUPPSVC_CLI_OVERRIDE_OPTION,  /*Override Option*/
  AEESUPPSVC_CLI_UNKNOWN_OPTION,   /*Unknown Option*/
  AEESUPPSVC_CLI_MAX_OPTION = 0x10000000
} AEESuppSvcCliOptionType;


/* ss password values */
typedef enum
{
   AEESUPPSVC_ENTER_PWD =0,
   AEESUPPSVC_ENTER_NEWPWD,
   AEESUPPSVC_ENTER_NEWPWD_AGAIN,
   AEESUPPSVC_MAX_PWD = 0x10000000
}AEESuppSvcSSPwdType;

/*Confirmation Types Received from the Network*/
typedef enum
{
  AEESUPPSVC_FWD_INFO = 1,
  AEESUPPSVC_FWD_FEATURE_LIST_INFO,
  AEESUPPSVC_CALL_BARRING_INFO,
  AEESUPPSVC_CLI_RESTRICTION_INFO,
  AEESUPPSVC_CUG_INFO,
  AEESUPPSVC_BSG_LIST_INFO,
  AEESUPPSVC_USS_DATA_INFO,
  AEESUPPSVC_USSD_DATA_INFO,
  AEESUPPSVC_NEW_PWD_INFO,
  AEESUPPSVC_SS_DATA_INFO,
  AEESUPPSVC_SS_STATUS_INFO,
  AEESUPPSVC_SS_ERROR_INFO,
  AEESUPPSVC_UNKNOWN_CONF_INFO, 
  AEESUPPSVC_CC_CAUSE_INFO,
  AEESUPPSVC_NO_INFO,
  AEESUPPSVC_CHECK_STATUS_INFO,
  AEESUPPSVC_CCBS_INFO,
  AEESUPPSVC_MAX_CONF_TYPE = 0x10000000
} AEESuppSvcSSConfType;


/*USS Response Types*/
typedef enum {

  AEESUPPSVC_USS_RES_SS_DATA,
    /* USS response with data */
  AEESUPPSVC_USS_RES_SS_ERROR,
    /* USS response indicating SS error */
  AEESUPPSVC_MAX_USS_TYPE = 0x10000000
} AEESUPPSYSUssResType;


/*
** enum for forwarding_reason
*/

typedef enum
{
   AEESUPPSVC_MS_NOT_REACHABLE,
   AEESUPPSVC_MS_BUSY,
   AEESUPPSVC_NO_REPLY,
   AEESUPPSVC_UNCONDITIONAL,
   AEESUPPSVC_FWD_REASON_NONE,
   AEESUPPSVC_MAX_FWD_REASON = 0x10000000
}AEESuppSvcFwdReasonsType;

/*Response Types*/
typedef enum
{
   AEESUPPSVC_GET_PASSWORD_RES,
   AEESUPPSVC_USS_NOTIFY_RES,
   AEESUPPSVC_USS_RES,
   AEESUPPSVC_RESP_MAX = 0x10000000
}AEESuppSvcRespType;


/* USS response types*/
typedef enum
{
  AEESUPPSVC_USS_RES_UNKNOWN_ALPHABET,    /* unlnown alphabet */
  AEESUPPSVC_USS_RES_BUSY_DISP,           /* display is busy  */
  AEESUPPSVC_USS_RES_OK,                  /* response */
  AEESUPPSVC_USS_RES_NONE,                /* no response */
  AEESUPPSVC_MAX_RES = 0x10000000
} AEESuppSvcNotifyRespType;

/*Caller ID Display Options*/
typedef enum {
   AEESUPPSVC_PERMANENT,
   AEESUPPSVC_TEMPORARY_DEFAULT_RESTRICTED,
   AEESUPPSVC_TEMPORARY_DEFAULT_ALLOWED,
   AEESUPPSVC_MAX_RESTRICT_OPTION = 0x10000000
}AEESuppSvcCliRestrictOptionType;

/************************************************************************
*                                                                       *
*               Network SS Codes                                        *
*                                                                       *
*    Note:  Only aoci, aocc are used for now                            *
*************************************************************************
*/

typedef enum 
{
 AEESUPPSVC_CLIP                       =   0x11, /* calling_line_identification_
                                       presentation_service */
 AEESUPPSVC_CLIR                       =   0x12, /* calling_line_identification_
                                       restriction_service presentation_service */
 AEESUPPSVC_COLP                       =   0x13, /* connected_line_identification_
                                       presentation_service */
 AEESUPPSVC_COLR                       =   0x14, /* connected_line identification_
                                       restriction_service */
 AEESUPPSVC_ALL_FORWARDING_SS          =   0x20, /* all_call_forwarding */
 AEESUPPSVC_CFU                        =   0x21, /*  call_forwarding_unconditional */
 AEESUPPSVC_ALL_CONDFWD_SS             =   0x28, /* all conditional call forwarding  */
 AEESUPPSVC_CFB                        =   0x29, /* call_forwarding_on_mobile_sub-
                                       scriber_busy */
 AEESUPPSVC_CFNRY                      =   0x2a, /* call_forwarding_on_no_reply */
 AEESUPPSVC_CFNRC                      =   0x2b, /* call_forwarding_on_mobile_subscriber_unreachable */
 AEESUPPSVC_CW                         =   0x41, /* call waiting */
 AEESUPPSVC_CCBS                       =   0x43, /* completion of call to busy subscribers */
 AEESUPPSVC_ALL_CALL_RSTRCT_SS         =   0x90,
 AEESUPPSVC_BARRING_OUTGOING_CALLS     =   0x91,
 AEESUPPSVC_BAOC                       =   0x92, /* barring_of_all_outgoing_calls */
 AEESUPPSVC_BOIC                       =   0x93, /* barring_of_outgoing_inter-
                                                national_calls */
 AEESUPPSVC_BOIC_EXHC                  =   0x94, /* barring_of_outgoing_inter-
                                                national_calls_except_those_
                                                to_home_plmn */
 AEESUPPSVC_BARRING_OF_INCOMING_CALLS  =   0x99,
 AEESUPPSVC_BAIC                       =   0x9a, /* barring of all incoming calls */
 AEESUPPSVC_BICROAM                    =   0x9b, /* barring of incomming calls when
                                                roaming outside home PLMN 
                                                Country */
 AEESUPPSVC_NULL_SS_CODE               =   0x10000000
} AEESuppSvcSSCodeType;


/*Supplementary Service Data*/
typedef struct
{
   uint32                           length;
   AECHAR *                        data;
} AEESuppSvcData;

/*Status Received from the network*/
typedef struct
{
   boolean                         present;
   boolean                         ss_operative;
   boolean                         ss_provisioned;
   boolean                         ss_registered;
   boolean                         ss_active;

} AEESuppSvcSSStatus;

/*CLI Info Received From the Network*/
typedef struct
{
   AEESuppSvcSSStatus                  ss_status;
   AEESuppSvcCliRestrictOptionType             cli_restriction_option;

} AEESuppSvcCliRestrictInfo;


/*Susbcription Info Received from the Network*/
typedef struct
{
  
  AEESuppSvcCliOptionType       option_type;
  union
  {
    AEESuppSvcCliRestrictOptionType     cli_restrict;      /*AEESUPPSVC_CLI_RESTRICT_OPTION*/
    boolean                             override_category; /*AEESUPPSVC_CLI_OVERRIDE_OPTION*/
  } option;

} AEESuppSvcSSSubscriptionOption;

/*Incompatibility Error Info Received from the Network*/
typedef struct
{
   AEESuppSvcSSCodeType                ss_code;
   AEESuppSvcBasicServiceType          basic_service;
   AEESuppSvcSSStatus                 ss_status;

} AEESuppSvcSSIncompatibilityError;

/*Invoke Info*/
typedef struct 
{
   uint32               invoke_id;
   AEESuppSvcData       deflectedToNumber;
   AEESuppSvcData       defelectedToSubAddress;
} AEESuppSvcInvoke;


/* connected number  */

typedef struct
{
   AEESuppSvcFwdReasonsType        fwd_reason;
   boolean                         notification_to_fp;
   boolean                         notification_to_cp;

} AEESuppSvcForwardingOptions;

/*Forwarding Feature Info*/
typedef struct
{
   AEESuppSvcBasicServiceType          basic_service;
   AEESuppSvcSSStatus                 ss_status;
   /*Separate them out*/
   AEESuppSvcData                     fwd_to_number;  
   AEESuppSvcData                     long_fwd_to_number;
   AEESuppSvcForwardingOptions        fwd_options;
   uint32                             timer_value;
   AEESuppSvcData                     fwd_to_subaddress;
} AEESuppSvcFwdFeature;


typedef struct
{
   uint32                          length;
   AEESuppSvcFwdFeature            fwd_feature[AEESUPPSVC_FWD_FEATURE_LIST_SIZE];

} AEESuppSvcFwdFeatureList;

typedef struct
{
   AEESuppSvcSSCodeType             ss_code;
   AEESuppSvcFwdFeatureList          fwd_feature_list;

} AEESuppSvcForwardingInfo;

/*Call Barring Info*/
typedef struct
{
   AEESuppSvcBasicServiceType      basic_service;
   AEESuppSvcSSStatus              ss_status;

} AEESuppSvcCallBarringFeature;

typedef struct
{
   AEESuppSvcSSCodeType           ss_code;
   uint32                         length;
   AEESuppSvcCallBarringFeature   call_barring_feature_list[AEESUPPSVC_CALL_BARRING_LIST_SIZE];

} AEESuppSvcCallBarringInfo;


/*List fo Basic Service Groups*/
typedef struct
{
   int                          length;
   AEESuppSvcBasicServiceType   bsg_list[AEESUPPSVC_BASIC_SERV_GROUP_LIST_SIZE];

} AEESuppSvcBasicServiceGroupListInfo;

/*Suuplmentary Service Data*/
typedef struct
{
   uint32                                length;
   AEESuppSvcSSCodeType              ss_code;
   AEESuppSvcSSStatus               ss_status;
   AEESuppSvcSSSubscriptionOption    ss_subscription_option;
   AEESuppSvcBasicServiceGroupListInfo   basic_service_group_list;

} AEESuppSvcSSDataInfo;



typedef byte AEESuppSvcSSNotification;

typedef struct
{
   uint32                        cug_list_length;
   AECHAR*                       cug_index[AEESUPPSVC_CUG_SUBSCRIPTION_LIST_SIZE];
} AEESuppSvcCUGInfo;


typedef struct
{
    uint32                       ccbsIndex;
    AEESuppSvcData               subscriberNumber;
    AEESuppSvcData               subscriberAddress;
    AEESuppSvcBasicServiceType   basicService;
} AEESuppSvcCCBSInd;

typedef struct
 {
    AEESuppSvcSSStatus              ss_status;
    uint32                          ccbs_feature_list_length;
    AEESuppSvcCCBSInd               ccbs_feature_list[AEESUPPSVC_MAX_CCBS_FEATURE_SIZE];
 } AEESuppSvcCCBSInfo;


/*Password Information*/
typedef struct
{
  AECHAR  *ss_password;
  AECHAR  *ss_new_password;
  AECHAR  *ss_new_password_again;
}AEESuppSvcPassword;

/*Registration Information*/
typedef struct
{
  AECHAR  *data;
  uint32    len;
  uint32    nr_timer;
}AEESuppSvcRegistration; 


typedef enum
{
/* release SS operations */
   AEESUPPSVC_REL_NO_OP=0, 
   AEESUPPSVC_REL_REGISTER_SS,
   AEESUPPSVC_REL_ERASE_SS,
   AEESUPPSVC_REL_ACTIVATE_SS,
   AEESUPPSVC_REL_DEACTIVATE_SS,
   AEESUPPSVC_REL_INTERROGATE_SS,
   AEESUPPSVC_REL_REG_PASSWD_SS,
   AEESUPPSVC_REL_GET_PWD_IND,
   AEESUPPSVC_REL_FWD_CHECK_SS_IND,
   AEESUPPSVC_REL_USS,        
   AEESUPPSVC_REL_PROCESS_USS,
   AEESUPPSVC_MAX_REL_OP = 0x10000000
}AEESuppSvcReleaseSSOp;


typedef enum
{
  AEESUPPSVC_CONTENT_TON = 0,
  AEESUPPSVC_CONTENT_NPI = 1,
  AEESUPPSVC_CONTENT_MAX = 0x10000000
}AEESuppSvcContentType;

/* This structure is sent with the completed event. */

typedef enum{
  AEESUPPSVC_DCS_7_BIT =0,
  AEESUPPSVC_DCS_8_BIT,
  AEESUPPSVC_DCS_UCS2,
  AEESUPPSVC_DCS_MAX =0x10000000
}AEESuppSvcDCS;

/**********************************************************************

 ISUPPSVC and ISUPPSVC_INOTIFY Interfaces

***********************************************************************
 Notification Masks...
 These are used when to determine what changed in the status function.
 Users can register for this notification via the INOTIFY interface.
 The below are implemented in the ISUPPSVC_INOTIFY class ID.*/

#define NMASK_SS_STARTED        0x0001  /* Requesting... */
#define NMASK_SS_COMPLETE       0x0002  /* SS request has completed.*/
#define NMASK_SS_NW_INITIATED   0x0004  /* network Initiated SS*/
#define NMASK_SS_NW_RESPONSE    0x0008  /* Response to NW initiated SS requests */
#define NMASK_SS_END            0x0010  /* SS Abort/Release */

typedef enum 
{
  /* SS Started Events: Notification Group/Bit - 0 */
  AEESUPPSVC_EVENT_REGISTER = 0x0001,      /* Register Supplementary Service*/
  AEESUPPSVC_EVENT_ERASE,                  /* Erase Supplementary Service Request */
  AEESUPPSVC_EVENT_ACTIVATE,               /* Activate Supplementary Service Request */
  AEESUPPSVC_EVENT_DEACTIVATE,             /* Deactivate Supplementary Service Request */
  AEESUPPSVC_EVENT_INTERROGATE,            /* Interrogate Supplementary Service Request */
  AEESUPPSVC_EVENT_REG_PASSWORD,           /* Register Password Supplementary Service Request*/
  AEESUPPSVC_EVENT_PROCESS_USS,            /* Process Unstructured Supplementary Service Request */
/*-------------------------------------------------------------------*/
   /* SS Confirmed Events: Notification Group/Bit - 1 */
  AEESUPPSVC_EVENT_CHANGE_PIN  = 0x101,    /* Change Pin Complete Event*/
  AEESUPPSVC_EVENT_UNBLOCK_PIN,            /* Unblock Pin Complete Event*/
  AEESUPPSVC_EVENT_DISPLAY_IMEI,           /* Display IMEI Result*/
  AEESUPPSVC_EVENT_REGISTER_CONF,          /* Register Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_ERASE_CONF,             /* Erase Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_ACTIVATE_CONF,          /* Activate Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_DEACTIVATE_CONF,        /* Deactivate Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_INTERROGATE_CONF,       /* Interrogate Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_REG_PASSWORD_CONF,      /* Register Password Supplementary Service Confirmation */
  AEESUPPSVC_EVENT_PROCESS_USS_CONF,       /* Process Unstructured Supplementary Service Confirmation */
/*-------------------------------------------------------------------*/
  /* NW Initiated SS  Group/Bit - 2 */
  AEESUPPSVC_EVENT_USS_NOTIFY_IND = 0x201, /* Unstructured Supplementary Service Notify Indication*/
  AEESUPPSVC_EVENT_USS_IND,                /* Unstructured Supplementary Service Indication */
  AEESUPPSVC_EVENT_FWD_CHECK_IND,          /* Forward Check Supplementary Service Indication */
  AEESUPPSVC_EVENT_GET_PASSWORD_IND,       /* Get Password Supplementary Service Indication */
  AEESUPPSVC_EVENT_RELEASE_USS_IND,        /* Release Unstructured Supplementary Service Indication*/
/*-------------------------------------------------------------------*/
  /* SS Info Requested  Group/Bit - 3 */
  AEESUPPSVC_EVENT_USS_NOTIFY_RES =0x0301, /* Unstructured Supplementary Service Notify Response */
  AEESUPPSVC_EVENT_USS_RES,                /* Unstructured Supplementary Service Response */
  AEESUPPSVC_EVENT_GET_PASSWORD_RES,       /* Get Password Supplementary Service Response */
/*-------------------------------------------------------------------*/
  /* SS End  Group/Bit - 4 */
  AEESUPPSVC_EVENT_RELEASE = 0x0401,       /* Release Supplementary Service Request */
  AEESUPPSVC_EVENT_ERROR,                  /* Error in Call Cmd*/
/*-------------------------------------------------------------------*/
  AEESUPPSVC_MAX_EVENT = 0x10000000
}AEESuppSvcEventType;


// Supplementary Services String Return codes.
#define AEESUPPSVC_NOT_SS                    -1   /* Given string is not a SS cmd.   */
#define AEESUPPSVC_IS_SS                     -2   /* Given string is a SS cmd.       */
#define AEESUPPSVC_IS_PARTIAL_SS             -3   /* Given string is a partial SS Straing*/  
#define AEESUPPSVC_INVALID_PARAM             -4   /* Function parameter is invalid*/
#define AEESUPPSVC_ILLEGAL_ENTRY             -5   /* Invalid component of SS string (bad SIA, SIB, SIC) */

// Return code from Start SS command
#define AEESUPPSVC_SS_CMD_STARTED             0    /* Successful start to command.   */
#define AEESUPPSVC_SS_FAILED_UNKNOWN          1    /* SS Failed, other reasons       */
#define AEESUPPSVC_SS_FAILED_NOT_SS           2    /* SS Failed, string was not SS   */
#define AEESUPPSVC_SS_FAILED_PINS_DONT_MATCH  3    /* SS PIN change failed New Pins  didn't match */
#define AEESUPPSVC_SS_FAILED_NO_RESOURCES     4    /* SS Failed, No Resources       */
#define AEESUPPSVC_SS_FAILED_INCORRECT_MODE   5
#define AEESUPPSVC_SS_FAILED_NOT_SUPPORTED    6
#define AEESUPPSVC_SS_FAILED_FDN_CHECK        7    /* SS Failed FDN verification    */

#define AEESUPPSVC_SS_SUCCESS                 0  /* SS Completed successfully.     */


#define AEESUPPSVC_MAX_USS_CHAR                 160
#define AEESUPPSVC_MAX_PWD_CHAR                 4

/*SS States, The state is determined as the user enters the SS string*/
typedef enum
{
  AEESUPPSVC_NONE_STATE,
  AEESUPPSVC_OPER_STATE,
  AEESUPPSVC_CODE_STATE,
  AEESUPPSVC_DN_STATE,
  AEESUPPSVC_TIMER_STATE,
  AEESUPPSVC_PASSWD_STATE,
  AEESUPPSVC_BSG_STATE,
  AEESUPPSVC_COMPLETE_STATE,
  AEESUPPSVC_MAX_STATE = 0x10000000
}AEESupsSvcState;

typedef struct
{
  boolean voiceActive;
  boolean dataActive;
  boolean faxActive;
}AEESuppSvcActiveService;

/*Supplementary Service Start Info*/
typedef struct
{
  AEESuppSvcSSOperationType     ss_operation;       
  AEESuppSvcSSCodeType          ss_code;        
  AEESuppSvcBasicServiceType    basic_service; 
  uint32                         user_data;
  AEESupsSvcState               currentState;
  union
  {
    AEESuppSvcRegistration  reg;
    AEESuppSvcPassword      passwd;
    AEESuppSvcData          ussd;
    uint32                  ccbsIndex;
  }req;

}AEESuppSvcSSStart;

/*Supplemnetray Service Response Info*/
typedef struct
{
  AEESuppSvcRespType        resp_type;
  AEESuppSvcNotifyRespType  notify;
  uint32                    invoke_id;
  uint32                    length;
  AECHAR*                   user_input;
  AEESuppSvcDCS             USSDdcs;
}AEESuppSvcSSResponse;


/*RegisterSS Operation Could return any of the following data
  ForwardingInfo
  CallBarringInfo
  SSDataInfo 
  SSError  :  


  EraseSS Operation Could return any of the following data
  ForwardingInfo
  CallBarringInfo 
  SSDataInfo
  SSError

  ActivateSS Operation Could return any of the following data
  ForwardingInfo
  CallBarringInfo
  SSDataInfo
  SSError

DeactivateSS   Operation Could return any of the following data
  ForwardingInfo
  CallBarringInfo
  SSDataInfo
  SSError

InterrogateSS Operation Could return any of the following data
  BasicServiceGroupList
  SSStatus
  ForwardingFeatureList
  ForwardingInfo
  CallBarringInfo
  SSDataInfo
  SSError
RegisterPassword   Operation Could return any of the following data
NewPassword
SSError

AEESUPPSVC_USS_REQ,
AEESUPPSVC_PROCESS_USSD_DATA,

AEESUPPSVC_USS_NOTIFY,        
AEESUPPSVC_PROCESS_USSD_REQ, will return 
AEESuppSvcData
SSError
*/


typedef struct
{
  AEESuppSvcSSConfType conf_type;
  boolean              ss_success;
  uint32               invoke_id;
  AEESuppSvcDCS        dcs;
  union
  {
    AEESuppSvcForwardingInfo              fwd_info;          /*AEESUPPSVC_FWD_INFO*/
    AEESuppSvcFwdFeatureList              fwd_feature_list;  /*AEESUPPSVC_FWD_FEATURE_LIST_INFO*/
    AEESuppSvcCallBarringInfo             call_barring_info; /*AEESUPPSVC_CALL_BARRING_INFO*/
    AEESuppSvcCliRestrictInfo             cli_restriction;   /*AEESUPPSVC_CLI_RESTRICTION_INFO*/
    AEESuppSvcCUGInfo                     cug_info;          /*AEESUPPSVC_CUG_INFO*/
    AEESuppSvcBasicServiceGroupListInfo   bsg_list_info;     /*AEESUPPSVC_BSG_LIST_INFO*/
    AEESuppSvcData                        uss_data_info;     /*AEESUPPSVC_USS_DATA_INFO*/
    AEESuppSvcData                        ussd_data_info;    /*AEESUPPSVC_USSD_DATA_INFO*/
    AECHAR                                *ss_new_pwd;       /*AEESUPPSVC_NEW_PWD_INFO*/
    AEESuppSvcSSDataInfo                  ss_data;           /*AEESUPPSVC_SS_DATA_INFO*/
    AEESuppSvcSSErrorType                 ss_error;          /*AEESUPPSVC_SS_ERROR_INFO*/
    AEESuppSvcSSStatus                    ss_status;         /*AEESUPPSVC_SS_STATUS_INFO*/
    uint32 /*AEECMEndCause*/              cc_cause;          /*AEESUPPSVC_CC_CAUSE_INFO*/
	AEESuppSvcActiveService               check_status;      /*AEESUPPSVC_CHECK_STATUS_INFO*/ 
    AEESuppSvcCCBSInfo                    ccbs_info;         /*AEESUPPSVC_CCBS_INFO*/ 
  }complete;
} AEESuppSvcSSCompleted;

/*
AEESUPPSVC_GET_PASSWD
AEESuppSvcSSPwdType

AEESUPPSVC_PROCESS_USSD_DATA
AEESuppSvcData
*/

/*Supplementary Service Indicationsent by the Network */
typedef struct
{
  AEESuppSvcData               uss_data;
  AEESuppSvcSSPwdType          pwd_type;
  AEESuppSvcDCS                USSDdcs;
}AEESuppSvcSSInd;

/*Supplementary Service Raw L3 Data*/
typedef struct
{
   /* length in bytes of pData array */
   uint16 num_bytes;

   /* array of bytes containing l3 data, numBytes bytes long
      This is expected to be copied by the client. */
   uint8 *data_ptr;

} AEESuppSvcL3Data;

/*Complete Event Information sent from the network*/
typedef struct{
 int evt;
 int invoke_id;
 AEESuppSvcSSCodeType ss_code;
 AECHAR *alpha;
 AEESuppSvcSSOperationType ss_operation;
 uint8 requestId;
 AEESuppSvcL3Data L3_data;
 union
 {
    AEESuppSvcSSCompleted         ss_complete;
    AEESuppSvcSSInd               ss_ind;
    AECHAR*                       data;        /* The results message to be displayed */
 }evt_data;
}AEESUPPSVC_EVENT_DATA;


/* See data structure documentation */
typedef WebOpt SuppSvcOpt;


/* Forward typedef reference of the objects. */
typedef struct ISUPPSVC ISUPPSVC;
typedef struct ISUPPSVC_NOTIFIER ISUPPSVC_NOTIFIER;
typedef struct ISUPPSVC_MODEL ISUPPSVC_MODEL;
typedef struct ISUPPSVCOpts ISUPPSVCOpts;

/* The following structure defines the interface functions to the
 * ISUPPSVC interface.
 */
AEEINTERFACE(ISUPPSVC_NOTIFIER)
{
  INHERIT_INotifier(ISUPPSVC_NOTIFIER);
};

/*APIs*/
AEEINTERFACE(ISUPPSVC)
{
   INHERIT_IQueryInterface(ISUPPSVC);

   /* This function checks a string to determine if it is a SS/USSD command.  */
   int    (*StringCheck)(ISUPPSVC *, int length, AECHAR *, int *command);

   /* This function takes a string, determines if it is a SS/USSD command, 
    * and then sends out the request.  If the request is valid, it provides
    * an ID number which is associated with other commands and the response.
    */
  int     (*StartSS) (ISUPPSVC * po, AECHAR *wstr_buffer, uint8 *requestId, boolean bCheckFDN);

  int     (*StartSSEx) (ISUPPSVC * po, AEESuppSvcSSStart *ss_start, uint8 *requestId);

  int     (*StartSSWithOpts) (ISUPPSVC * po, ISUPPSVCOpts *pSupsOpts, AECHAR *wstr_buffer, uint8 *requestId, boolean bCheckFDN);

   /* This function cancels a SS request which was started. */
   int    (*CancelSS)(ISUPPSVC * po, uint8 requestId, AEESuppSvcReleaseSSOp ss_oper);

   /* If a SS command requires more information, this function provides that
    * information. */
   int    (*ContinueSS)(ISUPPSVC * po, AEESuppSvcSSResponse *ss_resp);

   int   (*NumberToBCD)(ISUPPSVC * po, const byte *addr, uint32 len, byte *bcd_number);
   int   (*GetOPStatus)(ISUPPSVC *po, AEESuppSvcSSStart *ss_start);


};

AEEINTERFACE(ISUPPSVCOpts)
{
  INHERIT_IQueryInterface(ISUPPSVCOpts);

  int (*AddOpt)(ISUPPSVCOpts *po, SuppSvcOpt *pOpt);
  int (*RemoveOpt)(ISUPPSVCOpts *po, int32 nOptId);
  int (*GetOpt)(ISUPPSVCOpts *po, int32 nOptId, SuppSvcOpt *pOpt);
};

/* The following defines make for easier naming of the function */
#define ISUPPSVC_AddRef(p)                 AEEGETPVTBL(p,ISUPPSVC)->AddRef(p)
#define ISUPPSVC_Release(p)                AEEGETPVTBL(p,ISUPPSVC)->Release(p)
#define ISUPPSVC_QueryInterface(p, clsid, pp)   \
        AEEGETPVTBL(p,ISUPPSVC)->QueryInterface(p,clsid,pp)

#define ISUPPSVC_StringCheck(p,len,str,pcmd)      \
        AEEGETPVTBL(p,ISUPPSVC)->StringCheck(p,len,str,pcmd)


#define ISUPPSVC_StartSS(p,p_wstr,id,b)  \
        AEEGETPVTBL(p,ISUPPSVC)->StartSS(p,p_wstr,id,b)

#define ISUPPSVC_StartSSEx(p,p_ss_start,id)  \
        AEEGETPVTBL(p,ISUPPSVC)->StartSSEx(p,p_ss_start,id) 

#define ISUPPSVC_StartSSWithOpts(p,p_opts,p_wstr,id,b)  \
        AEEGETPVTBL(p,ISUPPSVC)->StartSSWithOpts(p,p_opts,p_wstr,id,b)

#define ISUPPSVC_CancelSS(p,id, oper)      \
        AEEGETPVTBL(p,ISUPPSVC)->CancelSS(p,id,oper)

#define ISUPPSVC_ContinueSS(p,p_ss_resp)      \
        AEEGETPVTBL(p,ISUPPSVC)->ContinueSS(p,p_ss_resp)

#define ISUPPSVC_NumberToBCD(p,p_number,len, p_bcd)      \
        AEEGETPVTBL(p,ISUPPSVC)->NumberToBCD(p,p_number,len,p_bcd)

#define ISUPPSVC_GetOPStatus(p,p_start)  \
	   AEEGETPVTBL(p,ISUPPSVC)->GetOPStatus(p,p_start)


#define ISUPPSVCOpts_AddRef(p)                                                   \
                AEEGETPVTBL(p,ISUPPSVCOpts)->AddRef(p)
#define ISUPPSVCOpts_Release(p)                                                  \
                AEEGETPVTBL(p,ISUPPSVCOpts)->Release(p)
#define ISUPPSVCOpts_QueryInterface(p, clsid, ppo)                               \
                AEEGETPVTBL(p,ISUPPSVCOpts)->QueryInterface(p, clsid, ppo)
#define ISUPPSVCOpts_AddOpt(p, o)                                                \
                AEEGETPVTBL((p),ISUPPSVCOpts)->AddOpt(p, o)
#define ISUPPSVCOpts_RemoveOpt(p, i)                                             \
                AEEGETPVTBL(p,ISUPPSVCOpts)->RemoveOpt(p, i)
#define ISUPPSVCOpts_GetOpt(p, i, o)                                             \
                AEEGETPVTBL(p,ISUPPSVCOpts)->GetOpt(p, i, o)


/*===========================================================================
FUNCTION ISUPPSVC_StringCheck

DESCRIPTION
  This function is called to Check the String provided by the user to
  see if it is a supplementary service.   It first converts the string
  from wide characters to 1 byte characters, and then it runs the tokenizer
  to determine if it is a supplementary services string.

DEPENDENCIES
  None.

RETURN VALUE
  AEESUPPSVC_NOT_SS, AEESUPPSVC_IS_SS, AEESUPPSVC_PARTIAL_SS

SIDE EFFECTS
  None
===========================================================================*/
/*===========================================================================
FUNCTION ISUPPSVC_StartSS

DESCRIPTION
  This function is called to start the SS running.  The user input is tokenized to 
  determine if the operation is a SS, if so, it calls the function which starts the
  SS.  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns. It also returns the requestId if
  the Supplemnetary Service is started successfully

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS,  EFAILED

SIDE EFFECTS
  Plays with the display
===========================================================================*/
/*===========================================================================
FUNCTION ISUPPSVC_StartSSEx

DESCRIPTION
  This function is called to start the SS running.  Based on the info in the 
  AEESuppSvcSSStart, the Supplementary Service Operation is started
  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns. It also returns the request 
  Id if the supplmentary service is started successfully

DEPENDENCIES
  None.

RETURN VALUE
  SUCCEESS, EFAILED

SIDE EFFECTS
  Plays with the display
===========================================================================*/
/*===========================================================================
FUNCTION ISUPPSVC_CancelSS

DESCRIPTION
  This function is called to cancel a SS which is in progress.
  It is not yet implemented but will be needed when SS goes OTA.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*===========================================================================
FUNCTION ISUPPSVC_ContinueSS

DESCRIPTION
  This function is called to continue a SS providing it additional
  information.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*===========================================================================
FUNCTION ISUPPSVC_NumberToBCD

DESCRIPTION
  This function is called to convert the number into a BCD format

DEPENDENCIES
  None.

RETURN VALUE
  Length of the BCD Number

SIDE EFFECTS
  Converts the number into a BCD format
===========================================================================*/

/*=============================================================================
FUNCTION: ISUPPSVCOPTs_AddOpt

DESCRIPTION:  
  Adds an ID-Value pair to ISUPPSVCOPTs. Only one value is allowed for each
  ID unlike IWebOpts which maintains a stack of values for each ID.

PROTOTYPE:
  int ISUPPSVCOPTs_AddOpt(ISUPPSVCOPTs *po, SuppSvcOpt *pOpt)

PARAMETERS:
  po                [in]: ISUPPSVCOPTs interface pointer.
  pOpt              [in]: Call option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:
  
SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================*/

/*=============================================================================
FUNCTION: ISUPPSVCOPTs_RemoveOpt

DESCRIPTION:  
  Remove the ID-value pair matching the specified ID. 

PROTOTYPE:
  boolean ISUPPSVCOPTs_RemoveOpt(ISUPPSVCOPTs *po, int nOptId)

PARAMETERS:
  po                [in]: ISUPPSVCOPTs interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  SUCCESS: If remove was successful
  EFAILED: If ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_RemoveOpt()
  
=============================================================================*/

/*=============================================================================
FUNCTION: ISUPPSVCOPTs_GetOpt

DESCRIPTION:  
  Find the ID-value pair matching the specified ID. 

PROTOTYPE:
  int ISUPPSVCOPTs_GetOpt(ISuppsvcOpts *po, int32 nOptId, SuppSvcOpt *pOpt)

PARAMETERS:
  po                [in]: ISUPPSVCOPTs interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to SuppSvcOpt where matching Suppplementary service option
                          is copied.

RETURN VALUE:
  SUCCESS: If a matching call option is found.
  EFAILED: Otherwise

COMMENTS:
  Only the SuppSvcOpt structure is copied and not the data pointed by pVal field.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/

#endif    /* AEESUPPSVC_H */

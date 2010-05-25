#ifndef SSMSG_H
#define SSMSG_H

/*=========================================================================
FILE: SSMsg.h

SERVICES: GSM1x SS signaling messages structures

GENERAL DESCRIPTION:
    Internal format of GSM1x SS signaling messages. For message specifications
    see GSM1x Supplementary Services document.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEEComdef.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// max length of address field in forwarded_to_number parameter
#define SSMSG_MAX_ADDRESS_LENGTH 15
// max length of subaddress field in subaddress's parameters
#define SSMSG_MAX_SUBADDRESS_LENGTH 20
// max length of address field in long_ftn parameter
#define SSMSG_MAX_FTN_ADDRESS_LENGTH 28
// max number of basic service groups
// currently, only AllBasicServices group is supported
#define SSMSG_MAX_NUM_OF_BASIC_SERVICE_GROUPS 1
// max length of password field in password parameter
#define SSMSG_MAX_PASSWORD_LENGTH 4
// max length of diagnostic field in cause IE
#define SSMSG_DIAGNOSTIC_FIELD_MAX_LENGTH 28
// max length of number field in calling/called_party_bcd_number IE
#define SSMSG_NUMBER_FIELD_MAX_LENGTH 22
// max length of contents field in bearer_capability IE
// currently, bearer capability is hard coded into 1 octet
#define SSMSG_BEARER_CAPABILITY_MAX_LENGTH 1    

// max length of GSM1x SS message in external format
#define SSMSG_MSG_MAX_LENGTH 255

//*****************************************************************************
//
// constants for Facility ie
//
//*****************************************************************************

// component type
#define SSMSG_COMPONENT_TYPE_INVOKE         0xa1
#define SSMSG_COMPONENT_TYPE_RETURN_RESULT  0xa2
#define SSMSG_COMPONENT_TYPE_RETURN_ERROR   0xa3
#define SSMSG_COMPONENT_TYPE_REJECT         0xa4

typedef int SSMsg_component_type_type;

// parameter tag
#define SSMSG_PARAMETER_TAG_END_OF_CONTENT          0x00
#define SSMSG_PARAMETER_TAG_INVOKE_ID               0x02
#define SSMSG_PARAMETER_TAG_NULL                    0x05
#define SSMSG_PARAMETER_TAG_OPERATION_CODE          0x02
#define SSMSG_PARAMETER_TAG_ERROR_CODE              0x02
#define SSMSG_PARAMETER_TAG_GENERAL_PROBLEM         0x80
#define SSMSG_PARAMETER_TAG_INVOKE_PROBLEM          0x81
#define SSMSG_PARAMETER_TAG_RETURN_RESULT_PROBLEM   0x82
#define SSMSG_PARAMETER_TAG_RETURN_ERROR_PROBLEM    0x83

typedef int SSMsg_parameter_tag_type;

// operation code
#define SSMSG_OPERATION_CODE_REGISTERSS                     10
#define SSMSG_OPERATION_CODE_ERASESS                        11
#define SSMSG_OPERATION_CODE_ACTIVATESS                     12
#define SSMSG_OPERATION_CODE_DEACTIVATESS                   13
#define SSMSG_OPERATION_CODE_INTERROGATESS                  14
#define SSMSG_OPERATION_CODE_NOTIFYSS                       16
#define SSMSG_OPERATION_CODE_REGISTERPASSWORD               17
#define SSMSG_OPERATION_CODE_GETPASSWORD                    18
#define SSMSG_OPERATION_CODE_PROCESSUNSTRUCTUREDSS_DATA     19
#define SSMSG_OPERATION_CODE_FORWARDCHECKSS_INDICATION      38
#define SSMSG_OPERATION_CODE_PROCESSUNSTRUCTUREDSS_REQUEST  59
#define SSMSG_OPERATION_CODE_UNSTRUCTUREDSS_REQUEST         60
#define SSMSG_OPERATION_CODE_UNSTRUCTUREDSS_NOTIFY          61
#define SSMSG_OPERATION_CODE_ERASECCENTRY                   77
#define SSMSG_OPERATION_CODE_CALLDEFLECTION                117
#define SSMSG_OPERATION_CODE_USERUSERSERVICE               118
#define SSMSG_OPERATION_CODE_ACCESSREGISTERCCENTRY         119
#define SSMSG_OPERATION_CODE_FORWARDCUG_INFO               120
#define SSMSG_OPERATION_CODE_SPLITMPTY                     121
#define SSMSG_OPERATION_CODE_RETRIEVEMPTY                  122
#define SSMSG_OPERATION_CODE_HOLDMPTY                      123
#define SSMSG_OPERATION_CODE_BUILDMPTY                     124
#define SSMSG_OPERATION_CODE_FORWARDCHARGEADVICE           125
#define SSMSG_OPERATION_CODE_EXPLICITCT                    126

typedef int SSMsg_operation_code_type;

// error code
#define SSMSG_ERROR_CODE_UNKNOWNSUBSCRIBER                        1
#define SSMSG_ERROR_CODE_ILLEGALSUBSCRIBER                        9
#define SSMSG_ERROR_CODE_BEARERSERVICENOTPROVISIONED             10
#define SSMSG_ERROR_CODE_TELESERVICENOTPROVISIONED               11
#define SSMSG_ERROR_CODE_ILLEGALEQUIPMENT                        12
#define SSMSG_ERROR_CODE_CALLBARRED                              13
#define SSMSG_ERROR_CODE_ILLEGALSS_OPERATION                     16
#define SSMSG_ERROR_CODE_SS_ERRORSTATUS                          17
#define SSMSG_ERROR_CODE_SS_NOTAVAILABLE                         18
#define SSMSG_ERROR_CODE_SS_SUBSCRIPTIONVIOLATION                19
#define SSMSG_ERROR_CODE_SS_INCOMPATIBILITY                      20
#define SSMSG_ERROR_CODE_FACILITYNOTSUPPORTED                    21
#define SSMSG_ERROR_CODE_ABSENTSUBSCRIBER                        27
#define SSMSG_ERROR_CODE_SHORTTERMDENIAL                         29
#define SSMSG_ERROR_CODE_LONGTERMDENIAL                          30
#define SSMSG_ERROR_CODE_SYSTEMFAILURE                           34
#define SSMSG_ERROR_CODE_DATAMISSING                             35
#define SSMSG_ERROR_CODE_UNEXPECTEDDATAVALUE                     36
#define SSMSG_ERROR_CODE_PW_REGISTRATIONFAILURE                  37
#define SSMSG_ERROR_CODE_NEGATIVEPW_CHECK                        38
#define SSMSG_ERROR_CODE_NUMBEROFPW_ATTEMPTSVIOLATION            43
#define SSMSG_ERROR_CODE_POSITIONMETHODFAILURE                   54
#define SSMSG_ERROR_CODE_UNKNOWNALPHABET                         71
#define SSMSG_ERROR_CODE_USSD_BUSY                               72
#define SSMSG_ERROR_CODE_REJECTEDBYUSER                         121
#define SSMSG_ERROR_CODE_REJECTEDBYNETWORK                      122
#define SSMSG_ERROR_CODE_DEFLECTIONTOSERVEDSUBSCRIBER           123
#define SSMSG_ERROR_CODE_SPECIALSERVICECODE                     124
#define SSMSG_ERROR_CODE_INVALIDDEFLECTEDTONUMBER               125
#define SSMSG_ERROR_CODE_MAXNUMBEROFMPTY_PARTICIPANTSEXCEEDED   126
#define SSMSG_ERROR_CODE_RESOURCESNOTAVAILABLE                  127

typedef int SSMsg_error_code_type;

// problem code
// general problem codes
#define SSMSG_PROBLEM_CODE_GENERAL_UNRECOGNIZED_COMPONENT           0x00
#define SSMSG_PROBLEM_CODE_GENERAL_MISTYPED_COMPONENT               0x01
#define SSMSG_PROBLEM_CODE_GENERAL_BADLY_STRUCTURED_COMPONENT       0x02
// invoke problem codes
#define SSMSG_PROBLEM_CODE_INVOKE_DUPLICATE_INVOKE_ID               0x00
#define SSMSG_PROBLEM_CODE_INVOKE_UNRECOGNIZED_OPERATION            0x01
#define SSMSG_PROBLEM_CODE_INVOKE_MISTYPED_PARAMETER                0x02
#define SSMSG_PROBLEM_CODE_INVOKE_RESOURCE_LIMITATION               0x03
#define SSMSG_PROBLEM_CODE_INVOKE_INITIATING_RELEASE                0x04
#define SSMSG_PROBLEM_CODE_INVOKE_UNRECOGNIZED_LINKED_ID            0x05
#define SSMSG_PROBLEM_CODE_INVOKE_LINKED_RESPONSE_UNEXPECTED        0x06
#define SSMSG_PROBLEM_CODE_INVOKE_UNEXPECTED_LINKED_OPERATION       0x07
// Return Result Problem Codes
#define SSMSG_PROBLEM_CODE_RETURN_RESULT_UNRECOGNIZED_INVOKE_ID     0x00
#define SSMSG_PROBLEM_CODE_RETURN_RESULT_RETURN_RESULT_UNEXPECTED   0x01
#define SSMSG_PROBLEM_CODE_RETURN_RESULT_MISTYPED_PARAMETER         0x02
// Return Error Problem Codes
#define SSMSG_PROBLEM_CODE_RETURN_ERROR_UNRECOGNIZED_INVOKE_ID      0x00
#define SSMSG_PROBLEM_CODE_RETURN_ERROR_RETURN_ERROR_UNEXPECTED     0x01
#define SSMSG_PROBLEM_CODE_RETURN_ERROR_UNRECOGNIZED_ERROR          0x02
#define SSMSG_PROBLEM_CODE_RETURN_ERROR_UNEXPECTED_ERROR            0x03
#define SSMSG_PROBLEM_CODE_RETURN_ERROR_MISTYPED_PARAMETER          0x04

typedef int SSMsg_problem_code_type;

// ss code
#define SSMSG_SS_CODE_ALLSS  0x00 
// reserved for possible future use, all SS
#define SSMSG_SS_CODE_ALLLINEIDENTIFICATIONSS  0x10   
// reserved for possible future use, all line identification SS
#define SSMSG_SS_CODE_CLIP  0x11
// calling line identification presentation
#define SSMSG_SS_CODE_CLIR  0x12
// calling line identification restriction
#define SSMSG_SS_CODE_COLP  0x13
// connected line identification presentation
#define SSMSG_SS_CODE_COLR  0x14
// connected line identification restriction
#define SSMSG_SS_CODE_MCI  0x15
// reserved for possible future use, malicious call identification
#define SSMSG_SS_CODE_ALLNAMEIDENTIFICATIONSS  0x18
// all name identification SS
#define SSMSG_SS_CODE_CNAP  0x19
// calling name presentation
// SS-Codes '00011010'B to '00011111'B are reserved for future 
// NameIdentification Supplementary Service use.
#define SSMSG_SS_CODE_ALLFORWARDINGSS  0x20
// all forwarding SS
#define SSMSG_SS_CODE_CFU  0x21
// call forwarding unconditional
#define SSMSG_SS_CODE_ALLCONDFORWARDINGSS  0x28
// all conditional forwarding SS
#define SSMSG_SS_CODE_CFB  0x29
// call forwarding on mobile subscriber busy
#define SSMSG_SS_CODE_CFNRY  0x2a
// call forwarding on no reply
#define SSMSG_SS_CODE_CFNRC  0x2b
// call forwarding on mobile subscriber not reachable
#define SSMSG_SS_CODE_CD  0x24
// call deflection
#define SSMSG_SS_CODE_ALLCALLOFFERINGSS  0x30
// reserved for possible future use, all call offering SS includes also all 
// forwarding SS
#define SSMSG_SS_CODE_ECT  0x31
// explicit call transfer
#define SSMSG_SS_CODE_MAH  0x32
// reserved for possible future use, mobile access hunting
#define SSMSG_SS_CODE_ALLCALLCOMPLETIONSS  0x40
// reserved for possible future use, all Call completion SS
#define SSMSG_SS_CODE_CW  0x41
// call waiting
#define SSMSG_SS_CODE_HOLD  0x42
// call hold
#define SSMSG_SS_CODE_CCBS_A  0x43
// completion of call to busy subscribers, originating side
#define SSMSG_SS_CODE_CCBS_B  0x44
// completion of call to busy subscribers, destination side
// this SS-Code is used only in InsertSubscriberData and DeleteSubscriberData
#define SSMSG_SS_CODE_MC  0x45
// multicall
#define SSMSG_SS_CODE_ALLMULTIPARTYSS  0x50
// reserved for possible future use, all multiparty SS
#define SSMSG_SS_CODE_MULTIPTY  0x51
// multiparty
#define SSMSG_SS_CODE_ALLCOMMUNITYOFINTEREST_SS  0x60
// reserved for possible future use, all community of interest SS
#define SSMSG_SS_CODE_CUG  0x61
// closed user group
#define SSMSG_SS_CODE_ALLCHARGINGSS  0x70
// reserved for possible future use, all charging SS
#define SSMSG_SS_CODE_AOCI  0x71
// advice of charge information
#define SSMSG_SS_CODE_AOCC  0x72
// advice of charge charging
#define SSMSG_SS_CODE_ALLADDITIONALINFOTRANSFERSS  0x80
// reserved for possible future use, all additional information transfer SS
#define SSMSG_SS_CODE_UUS1  0x81
// UUS1 user-to-user signalling
#define SSMSG_SS_CODE_UUS2  0x82
// UUS2 user-to-user signalling
#define SSMSG_SS_CODE_UUS3  0x83
// UUS3 user-to-user signalling
#define SSMSG_SS_CODE_ALLBARRINGSS  0x90
// all barring SS
#define SSMSG_SS_CODE_BARRINGOFOUTGOINGCALLS  0x91
// barring of outgoing calls
#define SSMSG_SS_CODE_BAOC  0x92
// barring of all outgoing calls
#define SSMSG_SS_CODE_BOIC  0x93
// barring of outgoing international calls
#define SSMSG_SS_CODE_BOICEXHC  0x94
// barring of outgoing international calls except those directed
// to the home PLMN
#define SSMSG_SS_CODE_BARRINGOFINCOMINGCALLS  0x99
// barring of incoming calls
#define SSMSG_SS_CODE_BAIC  0x9a
// barring of all incoming calls
#define SSMSG_SS_CODE_BICROAM  0x9b
// barring of incoming calls when roaming outside home PLMN
// Country} 

#define SSMSG_SS_CODE_ALLPLMN_SPECIFICSS  0xf0
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_1  0xf1
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_2  0xf2
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_3  0xf3
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_4  0xf4
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_5  0xf5
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_6  0xf6
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_7  0xf7
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_8  0xf8
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_9  0xf9
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_A  0xfa
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_B  0xfb
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_C  0xfc
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_D  0xfd
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_E  0xfe
#define SSMSG_SS_CODE_PLMN_SPECIFICSS_F  0xff

#define SSMSG_SS_CODE_ALLCALLPRIORITYSS  0xa0
// reserved for possible future use, all call priority SS
#define SSMSG_SS_CODE_EMLPP  0xa1
// enhanced Multilevel Precedence Preemption (EMLPP) service
#define SSMSG_SS_CODE_ALLLCSPRIVACYEXCEPTION  0xb0
// all LCS Privacy Exception Classes
#define SSMSG_SS_CODE_UNIVERSAL  0xb1
// allow location by any LCS client
#define SSMSG_SS_CODE_CALLRELATED  0xb2
// allow location by any value added LCS client to which a call
// is established from the target MS
#define SSMSG_SS_CODE_CALLUNRELATED  0xb3
// allow location by designated external value added LCS clients
#define SSMSG_SS_CODE_PLMNOPERATOR  0xb4
// allow location by designated PLMN operator LCS clients
#define SSMSG_SS_CODE_ALLMOLR_SS  0xc0
// all Mobile Originating Location Request Classes
#define SSMSG_SS_CODE_BASICSELFLOCATION  0xc1
// allow an MS to request its own location
#define SSMSG_SS_CODE_AUTONOMOUSSELFLOCATION  0xc2
// allow an MS to perform self location without interaction
// with the PLMN for a predetermined period of time
#define SSMSG_SS_CODE_TRANSFERTOTHIRDPARTY  0xc3
// allow an MS to request transfer of its location to another LCS client

typedef int SSMsg_ss_code_type;

// hold indicator
#define SSMSG_HOLD_INDICATOR_CALL_RETRIEVED 0x00
#define SSMSG_HOLD_INDICATOR_CALL_ON_HOLD   0x01

typedef int SSMsg_hold_indicator_type;

// nature of address
#define SSMSG_NOA_UNKNOWN                       0x00
#define SSMSG_NOA_INTERNATIONAL_NUMBER          0x01
#define SSMSG_NOA_NATIONAL_SIGNIFICANT_NUMBER   0x02
#define SSMSG_NOA_NETWORK_SPECIFIC_NUMBER       0x03
#define SSMSG_NOA_SUBSCRIBER_NUMBER             0x04
#define SSMSG_NOA_ABBREVIATED_NUMBER            0x06

typedef int SSMsg_noa_type;

// numbering plan indicator
#define SSMSG_NPI_UNKNOWN           0x00
#define SSMSG_NPI_ISDN_TELEPHONY    0x01
#define SSMSG_NPI_DATA              0x03
#define SSMSG_NPI_TELEX             0x04
#define SSMSG_NPI_LAND_MOBILE       0x06
#define SSMSG_NPI_NATIONAL          0x08
#define SSMSG_NPI_PRIVATE           0x09

typedef int SSMsg_npi_type;

// type of subaddress
#define SSMSG_TOS_NSAP              0x00
#define SSMSG_TOS_USER_SPECIFIED    0x02

typedef int SSMsg_tos_type;

// cli restriction option
#define SSMSG_CLI_RESTRICTION_OPTION_PERMANENT                  0x00
#define SSMSG_CLI_RESTRICTION_OPTION_TEMPORARYDEFAULTRESTRICTED 0x01
#define SSMSG_CLI_RESTRICTION_OPTION_TEMPORARYDEFAULTALLOWED    0x02

typedef int SSMsg_cli_restriction_option_type;

// progress description
// Call is not end-to-end PLMN/ISDN, further call progress information may be 
// available in-band
#define SSMSG_PROGRESS_DESCRIPTION_NOT_END_TO_END_PLMN_ISDN_IN_BAND_MAY_BE_AVAILABLE 0x01
// Destination address in non-PLMN/ISDN
#define SSMSG_PROGRESS_DESCRIPTION_DESTINATION_ADDRESS_IN_NON_PLMN_ISDN 0x02
// Origination address in non-PLMN/ISDN
#define SSMSG_PROGRESS_DESCRIPTION_ORIGINATION_ADDRESS_IN_NON_PLMN_ISDN 0x03
// Call has returned to the PLMN/ISDN
#define SSMSG_PROGRESS_DESCRIPTION_RETURNED_TO_THE_PLMN_ISDN            0x04
// In-band information or appropriate pattern now available
#define SSMSG_PROGRESS_DESCRIPTION_IN_BAND_NOW_AVAILABLE                0x08
// Call is end-to-end PLMN/ISDN
#define SSMSG_PROGRESS_DESCRIPTION_END_TO_END_PLMN_ISDN                 0x20
// Queueing
#define SSMSG_PROGRESS_DESCRIPTION_QUEUEING                             0x80
// All other values
#define SSMSG_PROGRESS_DESCRIPTION_UNSPECIFIC                           0xff

typedef int SSMsg_progress_description_type;

// forwarding reason
#define SSMSG_FORWARDING_REASON_MS_NOT_REACHABLE 0x00
#define SSMSG_FORWARDING_REASON_MS_BUSY 0x01
#define SSMSG_FORWARDING_REASON_NO_REPLY 0x02
#define SSMSG_FORWARDING_REASON_UNCONDITIONAL 0x03

typedef int SSMsg_forwarding_reason_type;

//*****************************************************************************
//
// constants for Cause ie
//
//*****************************************************************************

// coding standard
#define SSMSG_CODING_STANDARD_NOT_SUPPORTED   -1
// Standard defined for the GSM PLMNS
#define SSMSG_CODING_STANDARD_GSM           0x03

typedef int SSMsg_coding_standard_type;

// location of the cause originator.
// user
#define SSMSG_LOCATION_USER                         0x00
// private network serving the local user
#define SSMSG_LOCATION_PRIVATE_NETWORK_LOCAL_USER   0x01   
// public network serving the local user
#define SSMSG_LOCATION_PUBLIC_NETWORK_LOCAL_USER    0x02    
// transit network
#define SSMSG_LOCATION_TRANSIT_NETWORK              0x03
// public network serving the remote user
#define SSMSG_LOCATION_PUBLIC_NETWORK_REMOTE_USER   0x04   
// private network serving the remote user
#define SSMSG_LOCATION_PRIVATE_NETWORK_REMOTE_USER  0x05  
// international network
#define SSMSG_LOCATION_INTERNATIONAL_NETWORK        0x07        
// network beyond interworking point
#define SSMSG_LOCATION_BEYOND_INTERWORKING_POINT    0x0a     

typedef int SSMsg_location_type;

// cause value
#define SSMSG_CAUSE_VALUE_UNASSIGNED_NUMBER  0x01
#define SSMSG_CAUSE_VALUE_NO_ROUTE_TO_DESTINATION  0x03
#define SSMSG_CAUSE_VALUE_CHANNEL_UNACCEPTABLE  0x06
#define SSMSG_CAUSE_VALUE_OPERATOR_DETERMINED_BARRING  0x08
#define SSMSG_CAUSE_VALUE_NORMAL_CALL_CLEARING  0x10
#define SSMSG_CAUSE_VALUE_USER_BUSY  0x11
#define SSMSG_CAUSE_VALUE_NO_USER_RESPONDING  0x12
#define SSMSG_CAUSE_VALUE_USER_ALERTING_NO_ANSWER  0x13
#define SSMSG_CAUSE_VALUE_CALL_REJECTED  0x15
#define SSMSG_CAUSE_VALUE_NUMBER_CHANGED  0x16
#define SSMSG_CAUSE_VALUE_PRE_EMPTION  0x19
#define SSMSG_CAUSE_VALUE_NON_SELECTED_USER_CLEARING  0x1a
#define SSMSG_CAUSE_VALUE_DESTINATION_OUT_OF_ORDER  0x1b
#define SSMSG_CAUSE_VALUE_INVALID_NUMBER_FORMAT  0x1c
#define SSMSG_CAUSE_VALUE_FACILITY_REJECTED  0x1d
#define SSMSG_CAUSE_VALUE_RESPONSE_TO_STATUS_ENQUIRY  0x1e
#define SSMSG_CAUSE_VALUE_NORMAL_UNSPECIFIED  0x1f
#define SSMSG_CAUSE_VALUE_NO_CIRCUIT_CHANNEL_AVAILABLE  0x22
#define SSMSG_CAUSE_VALUE_NETWORK_OUT_OF_ORDER  0x26
#define SSMSG_CAUSE_VALUE_TEMPORARY_FAILURE  0x29
#define SSMSG_CAUSE_VALUE_SWITCHING_EQUIPMENT_CONGESTION  0x2a
#define SSMSG_CAUSE_VALUE_ACCESS_INFORMATION_DISCARDED  0x2b
#define SSMSG_CAUSE_VALUE_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE  0x2c
#define SSMSG_CAUSE_VALUE_RESOURCES_UNAVAILABLE_UNSPECIFIED  0x2f
#define SSMSG_CAUSE_VALUE_QUALITY_OF_SERVICE_UNAVAILABLE  0x31
#define SSMSG_CAUSE_VALUE_REQUESTED_FACILITY_NOT_SUBSCRIBED  0x32
#define SSMSG_CAUSE_VALUE_INCOMING_CALLS_BARRED_WITHIN_THE_CUG  0x37
#define SSMSG_CAUSE_VALUE_BEARER_CAPABILITY_NOT_AUTHORIZED  0x39
#define SSMSG_CAUSE_VALUE_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE  0x3a
#define SSMSG_CAUSE_VALUE_SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED  0x3f
#define SSMSG_CAUSE_VALUE_BEARER_SERVICE_NOT_IMPLEMENTED  0x41
#define SSMSG_CAUSE_VALUE_ACM_EQUAL_TO_OR_GREATER_THAN_ACMMAX  0x44
#define SSMSG_CAUSE_VALUE_REQUESTED_FACILITY_NOT_IMPLEMENTED  0x45
#define SSMSG_CAUSE_VALUE_ONLY_RESTRICTED_DIGITAL_INFORMATION_BEARER_CAPABILITY_IS_AVAILABLE  0x46
#define SSMSG_CAUSE_VALUE_SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED  0x4f
#define SSMSG_CAUSE_VALUE_INVALID_TRANSACTION_IDENTIFIER_VALUE  0x51
#define SSMSG_CAUSE_VALUE_USER_NOT_MEMBER_OF_CUG  0x57
#define SSMSG_CAUSE_VALUE_INCOMPATIBLE_DESTINATION  0x58
#define SSMSG_CAUSE_VALUE_INVALID_TRANSIT_NETWORK_SELECTION  0x5b
#define SSMSG_CAUSE_VALUE_SEMANTICALLY_INCORRECT_MESSAGE  0x5f
#define SSMSG_CAUSE_VALUE_INVALID_MANDATORY_INFORMATION  0x60
#define SSMSG_CAUSE_VALUE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED  0x61
#define SSMSG_CAUSE_VALUE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE  0x62
#define SSMSG_CAUSE_VALUE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED  0x63
#define SSMSG_CAUSE_VALUE_CONDITIONAL_IE_ERROR  0x64
#define SSMSG_CAUSE_VALUE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE  0x65
#define SSMSG_CAUSE_VALUE_RECOVERY_ON_TIMER_EXPIRY  0x66
#define SSMSG_CAUSE_VALUE_PROTOCOL_ERROR_UNSPECIFIED  0x6f
#define SSMSG_CAUSE_VALUE_INTERWORKING_UNSPECIFIED  0x7f

typedef int SSMsg_cause_value_type;

//*****************************************************************************
//
// constants for SETUP message
//
//*****************************************************************************

// type of number
#define SSMSG_TON_UNKNOWN  0x00
#define SSMSG_TON_INTERNATIONAL_NUMBER  0x01
#define SSMSG_TON_NATIONAL_NUMBER  0x02
#define SSMSG_TON_NETWORK_SPECIFIC_NUMBER  0x03
#define SSMSG_TON_DEDICATED_ACCESS_SHORT_CODE  0x04

typedef int SSMsg_ton_type;   

// numbering plan identification
#define SSMSG_NP_ID_UNKNOWN  0x00
#define SSMSG_NP_ID_ISDN_TELEPHONY  0x01
#define SSMSG_NP_ID_DATA  0x03
#define SSMSG_NP_ID_TELEX  0x04
#define SSMSG_NP_ID_NATIONAL  0x08
#define SSMSG_NP_ID_PRIVATE  0x09

typedef int SSMsg_np_id_type;

// presentation indicator
#define SSMSG_PI_PRESENTATION_ALLOWED  0x00
#define SSMSG_PI_PRESENTATION_RESTRICTED  0x01
#define SSMSG_PI_NUMBER_NOT_AVAILABLE_DUE_TO_INTERWORKING  0x03

typedef int SSMsg_pi_type;

// screening indicator
#define SSMSG_SI_USER_PROVIDED_NOT_SCREENED  0x00
#define SSMSG_SI_USER_PROVIDED_VERIFIED_AND_PASSED  0x01
#define SSMSG_SI_USER_PROVIDED_VERIFIED_AND_FAILED  0x02
#define SSMSG_SI_NETWORK_PROVIDED  0x03

typedef int SSMsg_si_type;

// cause of no cli
#define SSMSG_CAUSE_OF_NO_CLI_UNAVAILABLE  0x00
#define SSMSG_CAUSE_OF_NO_CLI_REJECT_BY_USER  0x01
#define SSMSG_CAUSE_OF_NO_CLI_INTERACTION_WITH_OTHER_SERVICE  0x02
#define SSMSG_CAUSE_OF_NO_CLI_COIN_LINE_PAYPHONE  0x03

typedef int SSMsg_cause_of_no_cli_type;

// signal value
#define SSMSG_SIGNAL_VALUE_DIAL_TONE_ON  0x00
#define SSMSG_SIGNAL_VALUE_RING_BACK_TONE_ON  0x01
#define SSMSG_SIGNAL_VALUE_INTERCEPT_TONE_ON  0x02
#define SSMSG_SIGNAL_VALUE_NETWORK_CONGESTION_TONE_ON  0x03
#define SSMSG_SIGNAL_VALUE_BUSY_TONE_ON  0x04
#define SSMSG_SIGNAL_VALUE_CONFIRM_TONE_ON  0x05
#define SSMSG_SIGNAL_VALUE_ANSWER_TONE_ON  0x06
#define SSMSG_SIGNAL_VALUE_CALL_WAITING_TONE_ON  0x07
#define SSMSG_SIGNAL_VALUE_OFF_HOOK_WARNING_TONE_ON  0x08
#define SSMSG_SIGNAL_VALUE_TONES_OFF  0x3f
#define SSMSG_SIGNAL_VALUE_ALERTING_OFF  0x4f

typedef int SSMsg_signal_value_type;

//*****************************************************************************
//
// GSM1x tags
//
//*****************************************************************************

// ie tags
#define SSMSG_IE_TAG_BEARER_CAPABILITY  0x04
#define SSMSG_IE_TAG_CAUSE  0x08
#define SSMSG_IE_TAG_FACILITY  0x1c
#define SSMSG_IE_TAG_PROGRESS_INDICATOR  0x1e
#define SSMSG_IE_TAG_SIGNAL  0x34
#define SSMSG_IE_TAG_CAUSE_OF_NO_CLI  0x3a
#define SSMSG_IE_TAG_CALLING_PARTY_BCD_NUMBER  0x5c
#define SSMSG_IE_TAG_CALLING_PARTY_SUBADDRESS  0x5d
#define SSMSG_IE_TAG_CALLED_PARTY_BCD_NUMBER  0x5e
#define SSMSG_IE_TAG_CALLED_PARTY_SUBADDRESS  0x6d

typedef int SSMsg_ie_tag_type;

// protocol discriminator
// Call control; call-related SS messages
#define SSMSG_PD_CC  0x03
// Non call-related SS messages
#define SSMSG_PD_SS  0x0b

typedef int SSMsg_pd_type;

// messages tags
#define SSMSG_MSG_TYPE_ALERTING  0x01
#define SSMSG_MSG_TYPE_CALL_PROCEEDING  0x02
#define SSMSG_MSG_TYPE_SETUP  0x05
#define SSMSG_MSG_TYPE_CONNECT  0x07
#define SSMSG_MSG_TYPE_CALL_CONFIRMED  0x08
#define SSMSG_MSG_TYPE_CONNECT_ACKNOWLEDGE  0x0f
#define SSMSG_MSG_TYPE_HOLD  0x18
#define SSMSG_MSG_TYPE_HOLD_ACKNOWLEDGE  0x19
#define SSMSG_MSG_TYPE_HOLD_REJECT  0x1a
#define SSMSG_MSG_TYPE_RETRIEVE  0x1c
#define SSMSG_MSG_TYPE_RETRIEVE_ACKNOWLEDGE  0x1d
#define SSMSG_MSG_TYPE_RETRIEVE_REJECT  0x1e
#define SSMSG_MSG_TYPE_DISCONNECT  0x25
#define SSMSG_MSG_TYPE_RELEASE  0x2d
#define SSMSG_MSG_TYPE_RELEASE_COMPLETE  0x2a
#define SSMSG_MSG_TYPE_FACILITY  0x3a

typedef int SSMsg_msg_type_type;

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

//*****************************************************************************
//
// Parameters
//
//*****************************************************************************

typedef struct
{
    boolean present;    // parameter present
} SSMsg_param_header_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_operation_code_type value;
} SSMsg_param_operation_code_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_error_code_type value;
} SSMsg_param_error_code_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_parameter_tag_type problem_tag;
    SSMsg_problem_code_type value;
} SSMsg_param_problem_code_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    boolean quiescent;
    boolean provisioned;
    boolean registered;
    boolean active;
} SSMsg_param_ss_status_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_ss_code_type value;
} SSMsg_param_ss_code_type;

typedef struct
{
    SSMsg_param_header_type hdr;
} SSMsg_param_call_is_waiting_indicator_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_hold_indicator_type value;
} SSMsg_param_call_on_hold_indicator_type;

typedef struct
{
    SSMsg_param_header_type hdr;
} SSMsg_param_clir_suppression_rejected_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_noa_type noa;
    SSMsg_npi_type npi;
    uint8 address_len;   // number of chars
    uint8 address[SSMSG_MAX_ADDRESS_LENGTH]; // ascii chars
} SSMsg_param_forwarded_to_number_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_tos_type tos;
    boolean odd;        // odd number of address signals.
                        // used when the type of subaddress is "user specified" and
                        // the coding is BCD
    uint8 subaddress_len;
    uint8 subaddress[SSMSG_MAX_SUBADDRESS_LENGTH];
} SSMsg_param_forwarded_to_subaddress_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    boolean notification_to_forwarding_party;
    boolean redirecting_presentation;
    boolean notification_to_calling_party;
    SSMsg_forwarding_reason_type forwarding_reason;
} SSMsg_param_forwarding_options_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    uint8 value;
} SSMsg_param_no_reply_condition_time_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_noa_type noa;
    SSMsg_npi_type npi;
    uint8 address_len;   // number of chars
    uint8 address[SSMSG_MAX_FTN_ADDRESS_LENGTH]; // ascii chars
} SSMsg_param_long_ftn_type;    // long forwarded to number

typedef struct
{
    // basic service is ignored - always AllBasicServices Group
    SSMsg_param_ss_status_type ss_status;
    SSMsg_param_forwarded_to_number_type forwarded_to_number;
    SSMsg_param_forwarded_to_subaddress_type forwarded_to_subaddress;
    SSMsg_param_forwarding_options_type forwarding_options;
    SSMsg_param_no_reply_condition_time_type no_reply_condition_time;
    SSMsg_param_long_ftn_type long_ftn;
} SSMsg_param_forwarding_feature_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    uint8 forwarding_feature_count;
    SSMsg_param_forwarding_feature_type forwarding_feature[SSMSG_MAX_NUM_OF_BASIC_SERVICE_GROUPS];
} SSMsg_param_forwarding_feature_list_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_param_ss_code_type ss_code;
    SSMsg_param_forwarding_feature_list_type forwarding_feature_list;
} SSMsg_param_forwarding_info_type;

typedef struct
{
    // basic service is ignored - always AllBasicServices Group
    SSMsg_param_ss_status_type ss_status;
} SSMsg_param_call_barring_feature_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    uint8 call_barring_feature_count;
    SSMsg_param_call_barring_feature_type call_barring_feature[SSMSG_MAX_NUM_OF_BASIC_SERVICE_GROUPS];
} SSMsg_param_call_barring_feature_list_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_param_ss_code_type ss_code;
    SSMsg_param_call_barring_feature_list_type call_barring_feature_list;
} SSMsg_param_call_barring_info_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_cli_restriction_option_type value;
} SSMsg_param_cli_restriction_option_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_param_ss_code_type ss_code;
    SSMsg_param_ss_status_type ss_status;
    SSMsg_param_cli_restriction_option_type cli_restriction_option;
} SSMsg_param_ss_data_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_param_ss_status_type ss_status;
    SSMsg_param_cli_restriction_option_type cli_restriction_option;
} SSMsg_param_generic_service_info_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    uint8 password_len;
    uint8 password[SSMSG_MAX_PASSWORD_LENGTH];
} SSMsg_param_password_type;

//********************************************************************************************************************************
//
// Facility IE Components
//
//********************************************************************************************************************************

typedef struct
{
    boolean present;    // component present;
    SSMsg_component_type_type component_type;
} SSMsg_component_header_type;

typedef struct
{
    SSMsg_component_header_type hdr;
    SSMsg_param_operation_code_type operation_code;
    SSMsg_param_ss_code_type ss_code;
    SSMsg_param_call_is_waiting_indicator_type call_is_waiting_indicator;
    SSMsg_param_call_on_hold_indicator_type call_on_hold_indicator;
    SSMsg_param_clir_suppression_rejected_type clir_suppression_rejected;
} SSMsg_component_invoke_type;

typedef struct
{
    SSMsg_component_header_type hdr;
    SSMsg_param_operation_code_type operation_code;
    // Register, Erase, Activate, Deactivate parameters
    SSMsg_param_forwarding_info_type forwarding_info;
    SSMsg_param_call_barring_info_type call_barring_info;
    SSMsg_param_ss_data_type ss_data;
    // Interrogation parameters
    SSMsg_param_ss_status_type ss_status;
    SSMsg_param_forwarding_feature_list_type forwarding_feature_list;
    SSMsg_param_generic_service_info_type generic_service_info;
    // RegisterPassword parameters
    SSMsg_param_password_type new_password; 
} SSMsg_component_return_result_type;

typedef struct
{
    SSMsg_component_header_type hdr;
    SSMsg_param_error_code_type error_code;
    //SSMsg_param_operation_code_type operation_code;
} SSMsg_component_return_error_type;

typedef struct
{
    SSMsg_component_header_type hdr;
    SSMsg_param_problem_code_type problem_code;
    //SSMsg_param_operation_code_type operation_code;
} SSMsg_component_reject_type;

typedef union
{
    SSMsg_component_header_type gen_hdr;    // generic header for all components
    SSMsg_component_invoke_type invoke;
    SSMsg_component_return_result_type return_result;
    SSMsg_component_return_error_type return_error;
    SSMsg_component_reject_type reject;
} SSMsg_param_component_type;

//********************************************************************************************************************************
//
// Information Elements
//
//********************************************************************************************************************************

typedef struct
{
    boolean present;    // ie present;
} SSMsg_ie_header_type;

typedef struct SSMsg_ie_facility_type
{
    SSMsg_ie_header_type hdr;
/* support only a single component in a facility ie */
    SSMsg_param_component_type component;
} SSMsg_ie_facility_type;

typedef struct
{
    SSMsg_ie_header_type hdr;
    SSMsg_coding_standard_type coding_standard;
    SSMsg_location_type location;
    SSMsg_cause_value_type cause_value;
    uint8 diagnostic_len;
    uint8 diagnostic[SSMSG_DIAGNOSTIC_FIELD_MAX_LENGTH];
} SSMsg_ie_cause_type;

typedef struct
{
    SSMsg_ie_header_type hdr;
    SSMsg_ton_type ton;
    SSMsg_np_id_type np_id;
    SSMsg_pi_type pi;
    SSMsg_si_type si;
    uint8 number_len;
    uint8 number[SSMSG_NUMBER_FIELD_MAX_LENGTH];
} SSMsg_ie_calling_party_bcd_number_type;

typedef struct
{
    SSMsg_ie_header_type hdr;
    SSMsg_ton_type ton;
    SSMsg_np_id_type np_id;
    uint8 number_len;
    uint8 number[SSMSG_NUMBER_FIELD_MAX_LENGTH];
} SSMsg_ie_called_party_bcd_number_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_tos_type tos;
    boolean odd;        // odd number of address signals.
                        // used when the type of subaddress is "user specified" and
                        // the coding is BCD
    uint8 subaddress_len;
    uint8 subaddress[SSMSG_MAX_SUBADDRESS_LENGTH];
} SSMsg_ie_calling_party_subaddress_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_tos_type tos;
    boolean odd;        // odd number of address signals.
                        // used when the type of subaddress is "user specified" and
                        // the coding is BCD
    uint8 subaddress_len;
    uint8 subaddress[SSMSG_MAX_SUBADDRESS_LENGTH];
} SSMsg_ie_called_party_subaddress_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_cause_of_no_cli_type value;
} SSMsg_ie_cause_of_no_cli_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    uint8 contents_len;
    uint8 contents[SSMSG_BEARER_CAPABILITY_MAX_LENGTH];
} SSMsg_ie_bearer_capability_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_coding_standard_type coding_standard;
    SSMsg_location_type location;
    SSMsg_progress_description_type progress_description;
} SSMsg_ie_progress_indicator_type;

typedef struct
{
    SSMsg_param_header_type hdr;
    SSMsg_signal_value_type signal_value;
} SSMsg_ie_signal_type;
//********************************************************************************************************************************
//
// GSM1x Messages
//
//********************************************************************************************************************************

typedef struct
{
    SSMsg_pd_type pd;   // protocol discriminator
    /* transaction identifier flag. Identifies who allocated the TI value.
    set to "0" (FALSE) when it belongs to transaction initiated by its sender
    and to "1" (TRUE) otherwise. */
    boolean tiflag;     
    uint8 tivalue;    // transaction identifier or skip indicator
    SSMsg_msg_type_type msg_type;   // RIL3 message type
} SSMsg_header_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_alerting_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_call_proceeding_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_bearer_capability_type bearer_capability_1;
    SSMsg_ie_signal_type signal;
    SSMsg_ie_calling_party_bcd_number_type calling_party_bcd_number;
    SSMsg_ie_calling_party_subaddress_type calling_party_subaddress;
    SSMsg_ie_called_party_bcd_number_type called_party_bcd_number;
    SSMsg_ie_called_party_subaddress_type called_party_subaddress;
    SSMsg_ie_cause_of_no_cli_type cause_of_no_cli;
} SSMsg_msg_setup_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_connect_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
} SSMsg_msg_call_confirmed_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_connect_acknowledge_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_hold_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_hold_acknowledge_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
} SSMsg_msg_hold_reject_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_retrieve_type;

typedef struct
{
    SSMsg_header_type hdr;
} SSMsg_msg_retrieve_acknowledge_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
} SSMsg_msg_retrieve_reject_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
    SSMsg_ie_progress_indicator_type progress_indicator;
} SSMsg_msg_disconnect_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
    SSMsg_ie_cause_type second_cause;
} SSMsg_msg_release_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_cause_type cause;
    SSMsg_ie_facility_type facility;
} SSMsg_msg_release_complete_type;

typedef struct
{
    SSMsg_header_type hdr;
    SSMsg_ie_facility_type facility;
} SSMsg_msg_facility_type;

typedef union SSMsg_int_msg_type
{
    SSMsg_header_type gen_hdr;  // generic header for all messages
    SSMsg_msg_alerting_type alerting;
    SSMsg_msg_call_proceeding_type call_proceeding;
    SSMsg_msg_setup_type setup;
    SSMsg_msg_connect_type connect;
    SSMsg_msg_call_confirmed_type call_confirmed;
    SSMsg_msg_connect_acknowledge_type connect_acknowledge;
    SSMsg_msg_hold_type hold;
    SSMsg_msg_hold_acknowledge_type hold_acknowledge;
    SSMsg_msg_hold_reject_type hold_reject;
    SSMsg_msg_retrieve_type retrieve;
    SSMsg_msg_retrieve_acknowledge_type retrieve_acknowledge;
    SSMsg_msg_retrieve_reject_type retrieve_reject;
    SSMsg_msg_disconnect_type disconnect;
    SSMsg_msg_release_type release;
    SSMsg_msg_release_complete_type release_complete;
    SSMsg_msg_facility_type facility;
} SSMsg_int_msg_type;

//********************************************************************************************************************************
//
// External formats
//
//********************************************************************************************************************************

/*
typedef struct
{
    uint8 msg_len;
    uint8 msg[SSMSG_MSG_MAX_LENGTH];
} SSMsg_ext_msg_type;
*/
#endif // SSMSG_H

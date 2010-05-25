#ifndef _BTPFCS_H
#define _BTPFCS_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF 

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    E X T E R N A L    P R O F I L E
     
               C O M M A N D    S T A T U S    C O D E S

DESCRIPTION
  This file contains command status codes for the Bluetooth external profile
  API.

Copyright (c) 2004-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpfcs.h#1 $ 

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-05-28   hs   Added the support for AVRCP 1.3 meta data feature
                   by adding mapping for AVRCP error codes
$Log: $
*
*    #15       07 Nov 2007             GS
* Added HID profile host role support.
*
*    #14       31 May 2007            DSN
* Adding newly created header file to allow remoting of btfp apis for SAP.
*
*    #13       18 Oct  2006            PR
* Added OBEX error codes while merging OI Code v3.0.14.
*
*    #12       25 Sept 2006            MH
* Added OBEX error codes.
*
*    #10-11    26 Jul 2005            JTL
* Adding SBC/AVDTP error codes.
*
*    #9        17 Mar 2005            JTL
* Removing mapping for HCI error codes, as they're all mapped into
* the same block in CS as EVR's.
*
*    #8        04 Mar 2005            JTL
* Mapping OI_STATUS_STILL_REGISTERED to BT_CS_PF_STILL_REGISTERED
*
*    #4-7      10 Aug 2004            JTL
* Add #defines for status relating to TCS and L2.
*
*    #2        14 Apr 2004            JH
* Revamped error code definitions
*
*    #1        14 Apr 2004            JH
* Moved error codes from btpf.h to this file
*

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "btcomdef.h"

/*-------------------------------------------------------------------------*/
/*                      Profile Error Codes                                */
/*-------------------------------------------------------------------------*/
                      
#define OI_OK                             BT_CS_GN_SUCCESS
#define OI_STATUS_SUCCESS                 BT_CS_GN_SUCCESS

/* OxD000 */
#define OI_STATUS_INVALID_PARAMETERS      BT_CS_PF_INVALID_PARAMETERS           /* invalid function input parameters */
#define OI_STATUS_NOT_IMPLEMENTED         BT_CS_PF_NOT_IMPLEMENTED              /* attempt to use unimplemented function*/
#define OI_STATUS_NOT_INITIALIZED         BT_CS_PF_NOT_INITIALIZED              /* data not initialized */
#define OI_STATUS_NO_RESOURCES            BT_CS_PF_NO_RESOURCES                 /* generic resource allocation failure */
#define OI_STATUS_INTERNAL_ERROR          BT_CS_PF_INTERNAL_ERROR               /* internal inconsistency */
#define OI_STATUS_OUT_OF_MEMORY           BT_CS_PF_OUT_OF_MEMORY                /* generally, OI_Malloc failed */
#define OI_ILLEGAL_REENTRANT_CALL         BT_CS_PF_ILLEGAL_REENTRANT_CALL       /* violation of non-reentrant module policy */
#define OI_STATUS_INITIALIZATION_FAILED   BT_CS_PF_INITIALIZATION_FAILED        /* module initialization failed */
#define OI_STATUS_INITIALIZATION_PENDING  BT_CS_PF_INITIALIZATION_PENDING       /* inititialization not yet complete */
#define OI_STATUS_NO_SCO_SUPPORT          BT_CS_PF_NO_SCO_SUPPORT               /* SCO operation rejected; system not configured for SCO */
#define OI_STATUS_OUT_OF_STATIC_MEMORY    BT_CS_PF_OUT_OF_STATIC_MEMORY         /* actually a fatal error */
#define OI_TIMEOUT                        BT_CS_PF_TIMEOUT                      /* generic timeout */
#define OI_OS_ERROR                       BT_CS_PF_OS_ERROR                     /* some operating system error */
#define OI_FAIL                           BT_CS_PF_FAIL                         /* generic failure */
#define OI_STRING_FORMAT_ERROR            BT_CS_PF_STRING_FORMAT_ERROR          /* error in VarString formatting string */
#define OI_STATUS_PENDING                 BT_CS_PF_PENDING                      /* The operation is pending. */
/* 0xD010 */
#define OI_STATUS_INVALID_COMMAND         BT_CS_PF_INVALID_COMMAND              /* The command was invalid. */
#define OI_BUSY_FAIL                      BT_CS_PF_BUSY_FAIL                    /* command rejected due to busy */
#define OI_STATUS_ALREADY_REGISTERED      BT_CS_PF_ALREADY_REGISTERED           /* registration already performed. */
#define OI_STATUS_NOT_FOUND               BT_CS_PF_NOT_FOUND                    /* referenced resource not found */
#define OI_STATUS_NOT_REGISTERED          BT_CS_PF_NOT_REGISTERED               /* not registered */
#define OI_STATUS_NOT_CONNECTED           BT_CS_PF_NOT_CONNECTED                /* not connected */
#define OI_CALLBACK_FUNCTION_REQUIRED     BT_CS_PF_CALLBACK_FUNCTION_REQUIRED   /* callback function parameter required */
#define OI_STATUS_MBUF_OVERFLOW           BT_CS_PF_MBUF_OVERFLOW                /* no room to add another buffer to mbuf*/
#define OI_STATUS_MBUF_UNDERFLOW          BT_CS_PF_MBUF_UNDERFLOW               /* There was an attempt to pull too many bytes from an mbuf. */
#define OI_STATUS_CONNECTION_EXISTS       BT_CS_PF_CONNECTION_EXISTS            /* connection exists */
#define OI_STATUS_NOT_CONFIGURED          BT_CS_PF_NOT_CONFIGURED               /* module not configured */
#define OI_LOWER_STACK_ERROR              BT_CS_PF_LOWER_STACK_ERROR          . /* error was reported by lower stack API. This is used for embedded platforms*/
#define OI_STATUS_RESET_IN_PROGRESS       BT_CS_PF_RESET_IN_PROGRESS            /* Request failed/rejected because we're busy resetting. */
#define OI_STATUS_ACCESS_DENIED           BT_CS_PF_ACCESS_DENIED                /* Generic access denied error. */
#define OI_STATUS_DATA_ERROR              BT_CS_PF_DATA_ERROR                   /* Generic data error. */
#define OI_STATUS_INVALID_ROLE            BT_CS_PF_INVALID_ROLE                 /* The requested role was invalid. */
/* 0xD020 */
#define OI_STATUS_ALREADY_CONNECTED       BT_CS_PF_ALREADY_CONNECTED            /* The requested connection is already established. */
#define OI_STATUS_PARSE_ERROR             BT_CS_PF_PARSE_ERROR                  /* Parse error */
#define OI_STATUS_END_OF_FILE             BT_CS_PF_END_OF_FILE                  /* End of file */
#define OI_STATUS_READ_ERROR              BT_CS_PF_READ_ERROR                   /* Generic read error */
#define OI_STATUS_WRITE_ERROR             BT_CS_PF_WRITE_ERROR                  /* Generic write error */
#define OI_STATUS_NEGOTIATION_FAILURE     BT_CS_PF_NEGOTIATION_FAILURE          /* Error in negotiation */
#define OI_STATUS_READ_IN_PROGRESS        BT_CS_PF_READ_IN_PROGRESS             /* A read is already in progress */
#define OI_STATUS_ALREADY_INITIALIZED     BT_CS_PF_ALREADY_INITIALIZED          /* Initialization has already been done */
#define OI_STATUS_STILL_CONNECTED         BT_CS_PF_STILL_CONNECTED              /* The service cannot be shutdown because there are still active connections.*/
#define OI_STATUS_MTU_EXCEEDED            BT_CS_PF_MTU_EXCEEDED                 /* The packet is too big */
#define OI_STATUS_LINK_TERMINATED         BT_CS_PF_LINK_TERMINATED              /* The link was terminated */
#define OI_STATUS_CHANNEL_NOT_FOUND       BT_CS_PF_CHANNEL_NOT_FOUND            /* CID does not represent a current connection */
#define OI_STATUS_INVALID_STATE           BT_CS_PF_INVALID_STATE                /* invalid state */
#define OI_STATUS_INVALID_PACKET          BT_CS_PF_INVALID_PACKET               /* invalid packet */
#define OI_STATUS_INVALID_HANDLE          BT_CS_PF_INVALID_HANDLE               /* invalid handle */
#define OI_DISPATCH_INVALID_CB_HANDLE     BT_CS_PF_DISPATCH_INVALID_CB_HANDLE   /* Dispatcher was handed an invalid callback handle */
/* 0xD030 */
#define OI_DISPATCH_TABLE_OVERFLOW        BT_CS_PF_DISPATCH_TABLE_OVERFLOW      /* Dispatcher table is full */
#define OI_SDP_NO_MATCHING_SERVICE_RECORD BT_CS_PF_SDP_NO_MATCHING_SERVICE_RECORD/* No service record matched UUID list */
#define OI_SDP_ATTRIBUTE_NOT_FOUND        BT_CS_PF_SDP_ATTRIBUTE_NOT_FOUND      /* A requested attribute was not found in a service record */
#define OI_SDP_DATABASE_OUT_OF_RESOURCES  BT_CS_PF_SDP_DATABASE_OUT_OF_RESOURCES/* SDP server database is out of memory */
#define OI_RFCOMM_WRITE_IN_PROGRESS       BT_CS_PF_RFCOMM_WRITE_IN_PROGRESS
#define OI_RFCOMM_LINK_NOT_FOUND          BT_CS_PF_RFCOMM_LINK_NOT_FOUND
#define OI_RFCOMM_ESTABLISH_FAILURE       BT_CS_PF_RFCOMM_ESTABLISH_FAILURE
#define OI_RFCOMM_SESSION_SHUTDOWN        BT_CS_PF_RFCOMM_SESSION_SHUTDOWN
#define OI_RFCOMM_OUT_OF_SERVER_CHANNELS  BT_CS_PF_RFCOMM_OUT_OF_SERVER_CHANNELS
#define OI_STATUS_STILL_REGISTERED        BT_CS_PF_STILL_REGISTERED
#ifdef FEATURE_BT_EXTPF_GOEP
/* 0xD100 */
#define OI_OBEX_CONTINUE                  BT_CS_PF_OBEX_CONTINUE                /* Continue processing OBEX request */
#define OI_OBEX_COMMAND_ERROR             BT_CS_PF_OBEX_COMMAND_ERROR           /* An unrecognized OBEX command opcode */
#define OI_OBEX_CONNECTION_TIMEOUT        BT_CS_PF_OBEX_CONNECTION_TIMEOUT      /* Timeout waiting for a response */
#define OI_OBEX_CONNECT_FAILED            BT_CS_PF_OBEX_CONNECT_FAILED          /* Connection request did not succeed */
#define OI_OBEX_DISCONNECT_FAILED         BT_CS_PF_OBEX_DISCONNECT_FAILED       /* A disconnect failed probably because the connection did not exist */
#define OI_OBEX_ERROR                     BT_CS_PF_OBEX_ERROR                   /* Unspecified OBEX error */
#define OI_OBEX_INCOMPLETE_PACKET         BT_CS_PF_OBEX_INCOMPLETE_PACKET       /* Packet too short or corrupt */
#define OI_OBEX_LENGTH_REQUIRED           BT_CS_PF_OBEX_LENGTH_REQUIRED         /* Length header required in command */
#define OI_OBEX_NOT_CONNECTED             BT_CS_PF_OBEX_NOT_CONNECTED           /* No connection to server */
#define OI_OBEX_NO_MORE_CONNECTIONS       BT_CS_PF_OBEX_NO_MORE_CONNECTIONS     /* Reached max connections limit */
#define OI_OBEX_OPERATION_IN_PROGRESS     BT_CS_PF_OBEX_OPERATION_IN_PROGRESS   /* Another operation still in progress */
#define OI_OBEX_PUT_RESPONSE_ERROR        BT_CS_PF_OBEX_PUT_RESPONSE_ERROR      /* Error in response to a PUT command */
#define OI_OBEX_GET_RESPONSE_ERROR        BT_CS_PF_OBEX_GET_RESPONSE_ERROR      /* Error in response to a GET command */
#define OI_OBEX_REQUIRED_HEADER_NOT_FOUND BT_CS_PF_OBEX_REQUIRED_HEADER_NOT_FOUND/* Packet missing a required header */
#define OI_OBEX_SERVICE_UNAVAILABLE       BT_CS_PF_OBEX_SERVICE_UNAVAILABLE     /* Unknown target or required service */
#define OI_OBEX_TOO_MANY_HEADER_BYTES     BT_CS_PF_OBEX_TOO_MANY_HEADER_BYTES   /* Headers won't fit in single packet */
 /* 0xD110 */
#define OI_OBEX_UNKNOWN_COMMAND           BT_CS_PF_OBEX_UNKNOWN_COMMAND         /* Unrecognized OBEX command */
#define OI_OBEX_UNSUPPORTED_VERSION       BT_CS_PF_OBEX_UNSUPPORTED_VERSION     /* Version mismatch */
#define OI_OBEX_CLIENT_ABORTED_COMMAND    BT_CS_PF_OBEX_CLIENT_ABORTED_COMMAND  /* Server received abort command */
#define OI_OBEX_BAD_PACKET                BT_CS_PF_OBEX_BAD_PACKET              /* Any malformed OBEX packet */
#define OI_OBEX_BAD_REQUEST               BT_CS_PF_OBEX_BAD_REQUEST             /* Maps to OBEX response of same name */
#define OI_OBEX_OBJECT_OVERFLOW           BT_CS_PF_OBEX_OBJECT_OVERFLOW         /* Too many bytes received. */
#define OI_OBEX_NOT_FOUND                 BT_CS_PF_OBEX_NOT_FOUND               /* Maps to OBEX response of same name */
#define OI_OBEX_ACCESS_DENIED             BT_CS_PF_OBEX_ACCESS_DENIED           /* Object could not be read or written */
#define OI_OBEX_VALUE_NOT_ACCEPTABLE      BT_CS_PF_OBEX_VALUE_NOT_ACCEPTABLE    /* Value in command not out of range */
#define OI_OBEX_PACKET_OVERFLOW           BT_CS_PF_OBEX_PACKET_OVERFLOW         /* Buffer won't fit in a single packet */
#define OI_OBEX_NO_SUCH_FOLDER            BT_CS_PF_OBEX_NO_SUCH_FOLDER          /* Error returned by setpath operation */
#define OI_OBEX_NAME_REQUIRED             BT_CS_PF_OBEX_NAME_REQUIRED           /* Name must be non-null and non-empty */
#define OI_OBEX_PASSWORD_TOO_LONG         BT_CS_PF_OBEX_PASSWORD_TOO_LONG       /* Password exceeds implementation imposed length limit. */
#define OI_OBEX_PRECONDITION_FAILED       BT_CS_PF_OBEX_PRECONDITION_FAILED     /* Maps to OBEX response of same name. */
#define OI_OBEX_UNAUTHORIZED              BT_CS_PF_OBEX_UNAUTHORIZED            /* Authentication was not successful */
#define OI_OBEX_NOT_IMPLEMENTED           BT_CS_PF_OBEX_NOT_IMPLEMENTED         /* Unimplemented feature */
/* 0xD120 */
#define OI_OBEX_INVALID_AUTH_DIGEST       BT_CS_PF_OBEX_INVALID_AUTH_DIGEST     /* An authentication digest was bad */
#define OI_OBEX_INVALID_OPERATION         BT_CS_PF_OBEX_INVALID_OPERATION       /* Operation not allowed at this time */
#define OI_OBEX_DATABASE_FULL             BT_CS_PF_OBEX_DATABASE_FULL           /* Sync database full */
#define OI_OBEX_DATABASE_LOCKED           BT_CS_PF_OBEX_DATABASE_LOCKED         /* Sync database locked */
#define OI_OBEX_INTERNAL_SERVER_ERROR     BT_CS_PF_OBEX_INTERNAL_SERVER_ERROR   /* Maps to OBEX response of same name */
#define OI_OBEX_UNSUPPORTED_MEDIA_TYPE    BT_CS_PF_OBEX_UNSUPPORTED_MEDIA_TYPE  /* Maps to OBEX response of same name */
#define OI_OBEX_PARTIAL_CONTENT           BT_CS_PF_OBEX_PARTIAL_CONTENT         /* Maps to OBEX response of same name */
#define OI_OBEX_METHOD_NOT_ALLOWED        BT_CS_PF_OBEX_METHOD_NOT_ALLOWED      /* Maps to OBEX response of same name */
#define OI_OBEXSRV_INCOMPLETE_GET         BT_CS_PF_OBEXSRV_INCOMPLETE_GET       /* Indicates to a GET handler that the request phase is still in progress */
#define OI_OBEX_FOLDER_BROWSING_NOT_ALLOWED BT_CS_PF_OBEX_FOLDER_BROWSING_NOT_ALLOWED/* Indicates that an FTP server does not allow folder browsing */
#define OI_OBEX_SERVER_FORCED_DISCONNECT  BT_CS_PF_OBEX_SERVER_FORCED_DISCONNECT/* OBEX connection was forcibly terminated by the server */
#define OI_OBEX_CONNECTION_NOT_FOUND      BT_CS_PF_OBEX_CONNECTION_NOT_FOUND 
#define OI_OBEX_NOT_MODIFIED              BT_CS_PF_OBEX_NOT_MODIFIED            /* Not modified*/
#define OI_OBEX_GONE                      BT_CS_PF_OBEX_GONE                    /* OBEX request is no more with server */
#define OI_OBEX_REQ_TIMEOUT               BT_CS_PF_OBEX_REQ_TIMEOUT             /* Request timeout */
#define OI_OBEX_OFS_ERROR                 BT_CS_PF_OBEX_OFS_ERROR               /* Failed to write, to the file (OPP)*/
#define OI_OBEX_FILEOP_ERROR              BT_CS_PF_OBEX_FILEOP_ERROR            /* Failed to write, to the file (FTP)*/    
#endif /* FEATURE_BT_EXTPF_GOEP */

#ifdef FEATURE_BT_EXTPF_CTP
/* 0xD700 */
#define OI_TCS_INVALID_ELEMENT_TYPE       BT_CS_PF_CTP_INVALID_ELEMENT_TYPE     /* TCS: element type is invalid */
#define OI_TCS_INVALID_PACKET             BT_CS_PF_CTP_INVALID_PACKET           /* TCS: packet is invalid */
#define OI_TCS_CALL_IN_PROGRESS           BT_CS_PF_CTP_CALL_IN_PROGRESS         /* TCS: call is in progress */
#define OI_TCS_NO_CALL_IN_PROGRESS        BT_CS_PF_CTP_NO_CALL_IN_PROGRESS      /* TCS: no call in progress */
#endif /* FEATURE_BT_EXTPF_CTP */

#ifdef FEATURE_BT_EXTPF_AV
#define OI_CODEC_SBC_NO_SYNCWORD            BT_CS_PF_SBC_NO_SYNCWORD            /**< CODEC: Couldn't find an SBC SYNCWORD */
#define OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA BT_CS_PF_SBC_NOT_ENOUGH_HEADER_DATA /**< CODEC: Not enough data provided to decode an SBC header */
#define OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA   BT_CS_PF_SBC_NOT_ENOUGH_BODY_DATA   /**< CODEC: Decoded the header, but not enough data to contain the rest of the frame */
#define OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA  BT_CS_PF_SBC_NOT_ENOUGH_AUDIO_DATA  /**< CODEC: Not enough audio data for this frame */
#define OI_CODEC_SBC_CHECKSUM_MISMATCH      BT_CS_PF_SBC_CHECKSUM_MISMATCH      /**< CODEC: The frame header didn't match the checksum */
#define OI_AVDTP_CONNECTION_SEQ_ERROR       BT_CS_PF_AVDTP_CONNECTION_SEQ_ERR   /**< AVDTP: sequencing of signalling/media channel connections broken. */


#define OI_AVRCP_TOO_MANY_CONNECTIONS         BT_CS_PF_AVRCP_TOO_MANY_CONNECTIONS 
#define OI_AVRCP_NOT_IMPLEMENTED              BT_CS_PF_AVRCP_NOT_IMPLEMENTED 
#define OI_AVRCP_REJECTED                     BT_CS_PF_AVRCP_REJECTED 
#define OI_AVRCP_INVALID_RESPONSE             BT_CS_PF_AVRCP_INVALID_RESPONSE 
#define OI_AVRCP_RESPONSE_PACKET_OVERFLOW     BT_CS_PF_AVRCP_RESPONSE_PACKET_OVERFLOW
#define OI_AVRCP_RESPONSE_INVALID_PDU         BT_CS_PF_AVRCP_RESPONSE_INVALID_PDU
#define OI_AVRCP_RESPONSE_INVALID_PARAMETER   BT_CS_PF_AVRCP_RESPONSE_INVALID_PARAMETER
#define OI_AVRCP_RESPONSE_PARAMETER_NOT_FOUND BT_CS_PF_AVRCP_RESPONSE_PARAMETER_NOT_FOUND
#define OI_AVRCP_RESPONSE_INTERNAL_ERROR      BT_CS_PF_AVRCP_RESPONSE_INTERNAL_ERROR


#endif /* FEATURE_BT_EXTPF_AV */

#define OI_STATUS_WRITE_IN_PROGRESS       BT_CS_PF_WRITE_IN_PROGRESS      /**< L2CAP: write in progress */
#define OI_STATUS_PSM_ALREADY_REGISTERED  BT_CS_PF_PSM_ALREADY_REGISTERED /**< L2CAP: The specified PSM has already been registered. */
#define OI_STATUS_INVALID_CID             BT_CS_PF_INVALID_CID            /**< L2CAP: CID is invalid or no longer valid (connection terminated) */
#define OI_STATUS_CID_NOT_FOUND           BT_CS_PF_CID_NOT_FOUND          /**< L2CAP: CID does not represent a current connection */
#define OI_STATUS_PSM_NOT_FOUND           BT_CS_PF_PSM_NOT_FOUND          /**< L2CAP: PSM not found */
#define OI_STATUS_SEND_COMPLETE           BT_CS_PF_SEND_COMPLETE          /**< L2CAP: send is complete */

#define OI_L2CAP_DISCONNECT_LOWER_LAYER     BT_CS_PF_DISCONNECT_LOWER_LAYER     /**< L2CAP: The lower level forced a disconnect. */
#define OI_L2CAP_DISCONNECT_REMOTE_REQUEST  BT_CS_PF_DISCONNECT_REMOTE_REQUEST  /**< L2CAP: The remote device requested a disconnect. */
#define OI_L2CAP_DATA_WRITE_ERROR_LINK_TERM BT_CS_PF_DATA_WRITE_ERROR_LINK_TERM /**< L2CAP: Data write error LINK_TERM */
#define OI_L2CAP_DISCONNECT_LOCAL_REQUEST   BT_CS_PF_DISCONNECT_LOCAL_REQUEST   /**< L2CAP: Disconnect local requEST */

#define OI_L2CAP_CONNECT_TIMEOUT            BT_CS_PF_CONNECT_TIMEOUT    /**< L2CAP: Connect timeout */
#define OI_L2CAP_DISCONNECT_TIMEOUT         BT_CS_PF_DISCONNECT_TIMEOUT /**< L2CAP: Disconnect timeout */
#define OI_L2CAP_PING_TIMEOUT               BT_CS_PF_PING_TIMEOUT       /**< L2CAP: Ping timeout */
#define OI_L2CAP_GET_INFO_TIMEOUT           BT_CS_PF_GET_INFO_TIMEOUT   /**< L2CAP: Get info timeout */
#define OI_L2CAP_INVALID_ADDRESS            BT_CS_PF_INVALID_ADDRESS    /**< L2CAP: Invalid address */
#define OI_L2CAP_CMD_REJECT_RCVD            BT_CS_PF_CMD_REJECT_RCVD    /**< L2CAP: remote sent us 'command reject' response */

#define OI_L2CAP_CONNECT_PENDING              BT_CS_PF_CONNECT_PENDING              /**< L2CAP: Connect pending */
#define OI_L2CAP_CONNECT_REFUSED_INVALID_PSM  BT_CS_PF_CONNECT_REFUSED_INVALID_PSM  /**< L2CAP: Connect refused invalid PSM */
#define OI_L2CAP_CONNECT_REFUSED_SECURITY     BT_CS_PF_CONNECT_REFUSED_SECURITY     /**< L2CAP: Connect refused security */
#define OI_L2CAP_CONNECT_REFUSED_NO_RESOURCES BT_CS_PF_CONNECT_REFUSED_NO_RESOURCES /**< L2CAP: Connect refused no resources */

#define OI_L2CAP_CONFIG_FAIL_INVALID_PARAMETERS BT_CS_PF_CONFIG_FAIL_INVALID_PARAMETERS /**< L2CAP: Config fail invalid parameters */
#define OI_L2CAP_CONFIG_FAIL_NO_REASON          BT_CS_PF_CONFIG_FAIL_NO_REASON          /**< L2CAP: Config fail no reason */
#define OI_L2CAP_CONFIG_FAIL_UNKNOWN_OPTIONS    BT_CS_PF_CONFIG_FAIL_UNKNOWN_OPTIONS    /**< L2CAP: Config fail unknown options */

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /*  FEATURE_BT_EXTPF_HID_HOST */

#define OI_UNICODE_INVALID_SOURCE              BT_CS_PF_UNICODE_INVALID_SOURCE        /**< Unicode Conversion: Source string has invalid character encoding. */
#define OI_UINCODE_SOURCE_EXHAUSTED            BT_CS_PF_UNICODE_SOURCE_EXHAUSTED      /**< Unicode Conversion: Incomplete Unicode character at end of source buffer. */
#define OI_UINCODE_DESTINATION_EXHAUSTED       BT_CS_PF_UNICODE_DESTINATION_EXHAUSTED /**< Unicode Conversion: Destination buffer not large enough to hold resulting Unicode string. */

#endif /* FEATURE_BT_EXTPF */
#endif /* FEATURE_BT */
#endif /* _BTPFCS_H */


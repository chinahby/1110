#ifndef _OI_OBEXSPEC_H
#define _OI_OBEXSPEC_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_obexspec.h#1 $ 

$Log: $

 *     #7         21 Mar 2007               MH
 * Defined realm string type values.
 *
 *     #6         19 Sept 2006              MH
 * Changed number of apps value.
 *
 *     #5         12 Aug 2006               BK
 * Added a #define for number of Apps
 *
 *     #4         29 Jul 2006               BK
 * Modified for obex client initiated authentication.
 *
 *    #3         14 Sep 2004                JH
 * Changed the MIME type strings for vcard, vcalendar, vmsg and vnote to all 
 *  lower case (see comment next to the definitions).
 *        
===========================================================================*/
/**
 @file  
 This file contains definitions for commands, responses, and data structures as specified 
 in the OBEX standard.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_rfcomm.h"

#ifndef OI_CODE
/*This value can be derived from SIO table value....
This value corresponds to Number of Applications on IOBEX 
*/
#define OI_MAX_NUM_APPS     10 
#endif /* OI_CODE */

/**
 * @name OBEX Version Constant
 * @{
 * This is the OBEX protocol version not the OBEX specification version. The
 * current OBEX protocol version is 1.0
 */
#define OI_OBEX_MAJOR_VERSION    1
#define OI_OBEX_MINOR_VERSION    0

#define OI_OBEX_VERSION_NUMBER   ((OI_OBEX_MAJOR_VERSION << 4) | OI_OBEX_MINOR_VERSION)

#define OI_OBEX_MAJOR_VERSION_NUMBER(v)  (((v) >> 4) & 0x0F)
#define OI_OBEX_MINOR_VERSION_NUMBER(v)  ((v) & 0x0F)

#ifndef OI_CODE

#define OI_OBEX_MAX_REALM_LEN       255

typedef struct
{
    OI_UINT8 realmStr[OI_OBEX_MAX_REALM_LEN];
    OI_UINT8 realmLen;
    OI_UINT8 realmStrType;
}OI_REALM_INFO;

#define REALM_STR_ASCII             0
#define REALM_STR_ISO_8859_1        1
#define REALM_STR_ISO_8859_2        2
#define REALM_STR_ISO_8859_3        3
#define REALM_STR_ISO_8859_4        4
#define REALM_STR_ISO_8859_5        5
#define REALM_STR_ISO_8859_6        6
#define REALM_STR_ISO_8859_7        7
#define REALM_STR_ISO_8859_8        8
#define REALM_STR_ISO_8859_9        9
#define REALM_STR_UNICODE           0xFF
#endif
/**@}*/

/**
 * @name OBEX Header types
 * OBEX headers consist of a single-byte header ID followed by zero or more bytes
 * of header value. The most significant 2 bits of the header ID identify
 * one of four different kinds of header.
 * @{
 */
#define OI_OBEX_HDR_ID_UNICODE   0x00  /**< null-terminated Unicode text, length prefixed with 2-byte unsigned integer */
#define OI_OBEX_HDR_ID_BYTESEQ   0x40  /**< byte sequence, length prefixed with 2-byte unsigned integer */
#define OI_OBEX_HDR_ID_UINT8     0X80  /**< 1-byte quantity */
#define OI_OBEX_HDR_ID_UINT32    0xC0  /**< 4-byte quantity, transmitted in network byte order (big endian) */

#define OI_OBEX_HDR_KIND(h)      ((h) & 0xC0)


#define OI_OBEX_VAR_LEN_HDR(h)   (((h) & 0x80) == 0)


/**@}*/

/**
 * @name Standard OBEX header IDs
 * @{
 */
#define OI_OBEX_HDR_COUNT                      0xC0 /**< number of objects (used by Connect) */
#define OI_OBEX_HDR_NAME                       0x01 /**< name of the object (often a file name) */
#define OI_OBEX_HDR_TYPE                       0x42 /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
#define OI_OBEX_HDR_LENGTH                     0xC3 /**< the length of the object in bytes */
#define OI_OBEX_HDR_TIME                       0x44 /**< date/time stamp (ISO 8601) */
#define OI_OBEX_HDR_DESCRIPTION                0x05 /**< text description of the object */
#define OI_OBEX_HDR_TARGET                     0x46 /**< name of service to which operation is targeted */
#define OI_OBEX_HDR_HTTP                       0x47 /**< HTTP 1.x header */
#define OI_OBEX_HDR_BODY                       0x48 /**< a chunk of the object body. */
#define OI_OBEX_HDR_END_OF_BODY                0x49 /**< the final chunk of the object body */
#define OI_OBEX_HDR_WHO                        0x4A /**< identifies the OBEX application; used to tell if talking to a peer */
#define OI_OBEX_HDR_CONNECTION_ID              0xCB /**< an identifier used for OBEX connection multiplexing */
#define OI_OBEX_HDR_APPLICATION_PARAMS         0x4C /**< extended application request and response information */
#define OI_OBEX_HDR_AUTHENTICATION_CHALLENGE   0x4D /**< authentication digest-challenge */
#define OI_OBEX_HDR_AUTHENTICATION_RESPONSE    0x4E /**< authentication digest-response */
#define OI_OBEX_HDR_CREATOR_ID                 0xCF /**< identifies the creator of object */
#define OI_OBEX_HDR_OBJECT_CLASS               0x51 /**< OBEX object class of object */

/**@}*/


/**
 * @name OBEX Final bit
 * The most significant bit in a command or response is called the 'final bit'. If
 * set, it indicates that the command or response is complete; otherwise it indicates
 * that more of the present command or response is coming. For example, a put request may need to be
 * broken into a sequence of put commands; the final bit will only be set on the last
 * put command in the sequence. Many commands and responses always have the final bit set.
 * @{
 */
#define OI_OBEX_FINAL_BIT                         0x80

#define OI_OBEX_FINAL(n)       ((OI_UINT8) ((n) | OI_OBEX_FINAL_BIT))
#define OI_OBEX_IS_FINAL(n)    (((n) & OI_OBEX_FINAL_BIT) == OI_OBEX_FINAL_BIT)


/**@}*/

/**
 * @name OBEX Opcodes
 * @{
 */
#define OI_OBEX_CMD_CONNECT                       0x80 /**< connect and negotiate capabilities */
#define OI_OBEX_CMD_DISCONNECT                    0x81 /**< signal the end of the session */
#define OI_OBEX_CMD_PUT                           0x02 /**< send an object */
#define OI_OBEX_CMD_GET                           0x03 /**< get an object */
#define OI_OBEX_CMD_SET_PATH                      0x85 /**< modifies the current path on the receiving side */
#define OI_OBEX_CMD_ABORT                         0xFF /**< abort the current operation. */

/**@}*/


/**
 * @name OBEX Response codes
 * @{
 */
typedef OI_UINT8 OI_OBEX_RSP_CODE;

#define OI_OBEX_RSP_CONTINUE                      0x10
#define OI_OBEX_RSP_OK                            0x20
#define OI_OBEX_RSP_CREATED                       0x21
#define OI_OBEX_RSP_ACCEPTED                      0x22
#define OI_OBEX_RSP_NON_AUTHORITATIVE_INFORMATION 0x23
#define OI_OBEX_RSP_NO_CONTENT                    0x24
#define OI_OBEX_RSP_RESET_CONTENT                 0x25
#define OI_OBEX_RSP_PARTIAL_CONTENT               0x26
#define OI_OBEX_RSP_MULTIPLE_CHOICES              0x30
#define OI_OBEX_RSP_MOVED_PERMANENTLY             0x31
#define OI_OBEX_RSP_MOVED_TEMPORARILY             0x32
#define OI_OBEX_RSP_SEE_OTHER                     0x33
#define OI_OBEX_RSP_NOT_MODIFIED                  0x34
#define OI_OBEX_RSP_USE_PROXY                     0x35
#define OI_OBEX_RSP_BAD_REQUEST                   0x40 /**< Server could not understand request. */
#define OI_OBEX_RSP_UNAUTHORIZED                  0x41
#define OI_OBEX_RSP_PAYMENT_REQUIRED              0x42
#define OI_OBEX_RSP_FORBIDDEN                     0x43 /**< Operation is understood but refused. */
#define OI_OBEX_RSP_NOT_FOUND                     0x44
#define OI_OBEX_RSP_METHOD_NOT_ALLOWED            0x45
#define OI_OBEX_RSP_NOT_ACCEPTABLE                0x46
#define OI_OBEX_RSP_PROXY_AUTHENTICATION_REQUIRED 0x47
#define OI_OBEX_RSP_REQUEST_TIME_OUT              0x48
#define OI_OBEX_RSP_CONFLICT                      0x49
#define OI_OBEX_RSP_GONE                          0x4A
#define OI_OBEX_RSP_LENGTH_REQUIRED               0x4B
#define OI_OBEX_RSP_PRECONDITION_FAILED           0x4C
#define OI_OBEX_RSP_REQUESTED_ENTITY_TOO_LARGE    0x4D
#define OI_OBEX_RSP_REQUEST_URL_TOO_LARGE         0x4E
#define OI_OBEX_RSP_UNSUPPORTED_MEDIA_TYPE        0x4F
#define OI_OBEX_RSP_INTERNAL_SERVER_ERROR         0x50
#define OI_OBEX_RSP_NOT_IMPLEMENTED               0x51
#define OI_OBEX_RSP_BAD_GATEWAY                   0x52
#define OI_OBEX_RSP_SERVICE_UNAVAILABLE           0x53
#define OI_OBEX_RSP_GATEWAY_TIMEOUT               0x54
#define OI_OBEX_RSP_HTTP_VERSION_NOT_SUPPORTED    0x55
#define OI_OBEX_RSP_DATABASE_FULL                 0x60
#define OI_OBEX_RSP_DATABASE_LOCKED               0x61

/**@}*/


/**
 * @name OBEX UUIDs
 * @{
 */

#define OI_OBEX_UUID_SIZE  16

#define OI_OBEX_FILE_BROWSING_UUID { 0xF9,0xEC,0x7B,0xC4,0x95,0x3C,0x11,0xD2,0x98,0x4E,0x52,0x54,0x00,0xDC,0x9E,0x09 }


/**@}*/


/**
 * @name MIME types used by OBEX
 * @{
 */

#define OI_OBEX_FOLDER_LISTING_TYPE "x-obex/folder-listing"
#define OI_OBEX_ICALENDAR_TYPE      "text/calendar"
#ifdef OI_CODE
#error code not present
#else
/*
 * According to Internet Assigned Numbers Authority RFC2045, 
 * Multipurpose Internet Mail Extensions (MIME) Part One: Format of Internet 
 * Message Bodies (http://www.isi.edu/in-notes/rfc2045.txt), p.11:
 *  "Matching of media type and subtype is ALWAYS case-insensitive"
 * So it should not matter what case is used for these type fields.
 * Here, we use all lower case becasue certain implementation (IAR tester)
 * require all lower case when matching.
 */
#define OI_OBEX_VCALENDAR_TYPE      "text/x-vcalendar"
#define OI_OBEX_VCARD_TYPE          "text/x-vcard"
#define OI_OBEX_VNOTE_TYPE          "text/x-vnote"
#define OI_OBEX_VMESSAGE_TYPE       "text/x-vmsg"
#endif /* OI_CODE */
#define OI_OBEX_UPF_TYPE            "image/x-UPF"
#define OI_OBEX_JPEG_TYPE           "image/jpeg"
#define OI_OBEX_TEXT_TYPE           "text/plain"

#define OI_OBEX_ICALENDAR_SUFFIX    ".ics"
#define OI_OBEX_VCALENDAR_SUFFIX    ".vcs"
#define OI_OBEX_VCARD_SUFFIX        ".vcf"
#define OI_OBEX_VNOTE_SUFFIX        ".vnt"
#define OI_OBEX_VMESSAGESUFFIX      ".vmg"

/**
 * OBEX object push types
 */

#define OI_OBEX_OBJ_FORMAT_VCARD_2_1   0x01
#define OI_OBEX_OBJ_FORMAT_VCARD_3_0   0x02
#define OI_OBEX_OBJ_FORMAT_VCAL_1_0    0x03
#define OI_OBEX_OBJ_FORMAT_ICAL_2_0    0x04
#define OI_OBEX_OBJ_FORMAT_VNOTE       0x05
#define OI_OBEX_OBJ_FORMAT_VMESSAGE    0x06
#define OI_OBEX_OBJ_FORMAT_ANY         0xFF

/**@}*/


/**
 * Other OBEX constants
 */

#define OI_OBEX_INVALID_CONNECTION_ID  0xFFFFFFFF

#define OI_OBEX_CONNECT_FLAGS     0  /**< Must be zero for OBEX 1.0 */


#define OI_OBEX_SETPATH_UP_LEVEL  BIT0  /**< Backup before applying name */
#define OI_OBEX_SETPATH_NO_CREATE BIT1  /**< Don't create directory if it doesn't exist */

#define OI_OBEX_MAX_PACKET_SIZE   OI_MAX_UINT16
#define OI_OBEX_MIN_PACKET_SIZE   255

/**
 * minimum size of an OBEX packet
 */

#define OI_OBEX_SMALLEST_PKT    (sizeof(OI_UINT8) + sizeof(OI_UINT16))

/**
 * Header prefix for a variable-length header is the 1-byte header ID followed
 * by a 2-byte length.
 */

#define OI_OBEX_HEADER_PREFIX_LEN  (sizeof(OI_UINT8) + sizeof(OI_UINT16))

/**
 * Header prefix for an application parameter value is a 1-byte tag followed by
 * a 1-byte length.
 */

#define OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN (sizeof(OI_UINT8) + sizeof(OI_UINT8))


/**
 * packet overhead for a packet containing a single body header
 */

#define OI_OBEX_BODY_PKT_OVERHEAD (OI_OBEX_SMALLEST_PKT + OI_OBEX_HEADER_PREFIX_LEN) 

/**
 * size of a connection id header
 * 1-byte tag plus 4-byte value
 */
#define OI_OBEX_CONNECTION_ID_LEN (sizeof(OI_UINT8) + sizeof(OI_UINT32))

/**
 * constants for OBEX authentication
 */

#define OI_OBEX_CHALLENGE_DIGEST_NONCE_TAG   0
#define OI_OBEX_CHALLENGE_DIGEST_OPTIONS_TAG 1
#define OI_OBEX_CHALLENGE_DIGEST_REALM_TAG   2

#define OI_OBEX_RESPONSE_DIGEST_REQUEST_TAG  0
#define OI_OBEX_RESPONSE_DIGEST_USERID_TAG   1
#define OI_OBEX_RESPONSE_DIGEST_NONCE_TAG    2
 
#define OI_OBEX_AUTH_USERID_OPTION           BIT0
#define OI_OBEX_AUTH_READONLY_OPTION         BIT1

/**
 * size of an OBEX authentication message digest
 */

#define OI_OBEX_AUTH_DIGEST_LEN   16

/**
 * maximum user id length as specified in the OBEX specification.
 */

#define OI_OBEX_MAX_USERID_LEN    20

/**
 * maximum password length supported by this implementation
 */

#define OI_OBEX_MAX_PASSWORD_LEN  16

/**
 * Utility macro for testing if an OBEX header is one of the two "body" headers.
 */

#define OI_OBEX_IS_A_BODY_HEADER(h) (((h) == OI_OBEX_HDR_BODY) || ((h) == OI_OBEX_HDR_END_OF_BODY))

/**
 * OBEX uses network (big-endian) byte order.
 */

#define OI_OBEX_BO   NETWORK_BYTE_ORDER

/**
 * struct for a Unicode (UCS-16) character string
 */

typedef struct {
    OI_UINT16 len;
    OI_CHAR16 *str;
} OI_OBEX_UNICODE;

/**
 * struct for an OBEX header of byte sequence type
 */

typedef struct {
    OI_UINT16 len;
    OI_BYTE *data;
} OI_OBEX_BYTESEQ;

/**
 * struct for describing an OBEX application parameter header entry
 */

typedef struct {
    OI_UINT8 tag;   /**< type of application parameter */
    OI_UINT8 len;   /**< length of tag data */
    OI_BYTE *data;  /**< pointer to tag data */
} OI_OBEX_APP_PARAM;

/**
 * struct for passing a list of OBEX application parameter entries
 */

typedef struct {
    OI_UINT8 count;            /**< number of application parameters */
    OI_OBEX_APP_PARAM *list;   /**< pointer to list of application parameters */
} OI_OBEX_APP_PARAM_LIST;


/**
 * struct for an OBEX header
 */

typedef struct {
    OI_UINT8 id;
    union {
        /**
         * generic header types
         */
        OI_UINT8 uInt8;
        OI_UINT32 uInt32;
        OI_OBEX_BYTESEQ byteseq;
        OI_OBEX_UNICODE unicode;
        /**
         * specific header types
         */
        OI_UINT32 count;
        OI_UINT32 length;
        OI_UINT32 connectionId;
        OI_OBEX_BYTESEQ type;
        OI_OBEX_BYTESEQ time;
        OI_OBEX_BYTESEQ target;
        OI_OBEX_BYTESEQ http;
        OI_OBEX_BYTESEQ body;
        OI_OBEX_BYTESEQ endOfBody;
        OI_OBEX_BYTESEQ who;
        OI_OBEX_BYTESEQ applicationParams;
        OI_OBEX_BYTESEQ authenticationChallenge;
        OI_OBEX_BYTESEQ authenticationResponse;
        OI_OBEX_BYTESEQ objectClass;
        OI_OBEX_UNICODE name;
        OI_OBEX_UNICODE description;
    } val;

} OI_OBEX_HEADER;


/**
 * struct for passing lists of OBEX headers
 */

typedef struct {
    OI_OBEX_HEADER *list;
    OI_UINT8 count;
} OI_OBEX_HEADER_LIST;


/*****************************************************************************/
#endif /* _OI_OBEXSPEC_H */


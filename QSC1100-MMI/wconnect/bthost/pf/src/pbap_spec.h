/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E  B O O K   A C C E S S   P R O F I L E   
                                C O M M O N

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile data and code 
  common to client and server.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_spec.h#1 $ 

$Log: $
  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-04-30   rb  Fix warning - Superfluous ',' at the end of enum

*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/

#ifndef _PBAP_SPEC_H
#define _PBAP_SPEC_H

#include "oi_bt_spec.h"
#include "oi_stddefs.h"
#include "oi_obexspec.h"
#include "pbap_private.h"

#define PBAP_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

#define PBAP_VERSION 0x0009


/** SDP constants
 */
#define PBAP_SDP_FEATURE_PHONE_BOOK_DOWNLOAD        0x00
#define PBAP_SDP_FEATURE_PHONE_BOOK_BROWSING        0x02

#define PBAP_SDP_REPOSITORY_LOCAL                   0x00
#define PBAP_SDP_REPOSITORY_SIM                     0x02


enum {
    PBAP_OBEX_APP_PARAM_ORDER = 0x01,                    /* UINT8 */
    PBAP_OBEX_APP_PARAM_SEARCH_VALUE = 0x02,             /* String */
    PBAP_OBEX_APP_PARAM_SEARCH_ATTRIBUTE = 0x03,         /* UINT8 */
    PBAP_OBEX_APP_PARAM_MAX_LIST_COUNT = 0x04,           /* UINT16 */
    PBAP_OBEX_APP_PARAM_LIST_START_OFFSET = 0x05,        /* UINT16 */
    PBAP_OBEX_APP_PARAM_FILTER = 0x06,                   /* UINT32 */
    PBAP_OBEX_APP_PARAM_FORMAT = 0x07,                   /* UINT8 */
    PBAP_OBEX_APP_PARAM_PHONE_BOOK_SIZE = 0x08,          /* UINT16 */
    PBAP_OBEX_APP_PARAM_NEW_MISSED_CALLS = 0x09          /* UINT8 */
};

typedef enum {
    PBAP_REQUEST_INITIAL,
    PBAP_REQUEST_CONTINUE,
    PBAP_REQUEST_CLEANUP
} PBAP_REQUEST_STATE;


#define PBAP_OBJ_TYPE_PHONE_BOOK "x-bt/phonebook"
#define PBAP_OBJ_TYPE_VCARD_LISTING "x-bt/vcard-listing"
#define PBAP_OBJ_TYPE_VCARD_ENTRY "x-bt/vcard"


/* PBAP operation parameter structures */
typedef struct {
    struct {
        OI_OBEX_UNICODE *objName;
        OI_OBEX_BYTESEQ *objType;
        UINT64    filter;
        OI_UINT8  format;
        OI_UINT16 listCount;
        OI_UINT16 startOffset;

        PBAP_PARAM_FLAGS flags;
    } request;

    struct {
        OI_UINT16 phoneBookSize;
        OI_UINT8  missedCalls;
        OI_BOOL   final;
        OI_OBEX_BYTESEQ *data;

        PBAP_PARAM_FLAGS flags;
    } response;
} PBAP_PULL_PHONE_BOOK_PARAMS;

typedef struct {
    struct {
        OI_OBEX_UNICODE *objName;
        OI_OBEX_BYTESEQ *objType;
        OI_UINT8  order;
        OI_OBEX_BYTESEQ searchValue;
        OI_UINT8  searchAttrib;
        OI_UINT16 listCount;
        OI_UINT16 startOffset;

        PBAP_PARAM_FLAGS flags;
    } request;

    struct {
        OI_UINT16 phoneBookSize;
        OI_UINT8  missedCalls;
        OI_BOOL   final;
        OI_OBEX_BYTESEQ *data;

        PBAP_PARAM_FLAGS flags;
    } response;
} PBAP_PULL_VCARD_LISTING_PARAMS;

typedef struct {
    struct {
        OI_OBEX_UNICODE *objName;
        OI_OBEX_BYTESEQ *objType;
        UINT64    filter;
        OI_UINT8  format;

        PBAP_PARAM_FLAGS flags;
    } request;

    struct {
        OI_BOOL   final;
        OI_OBEX_BYTESEQ *data;
    } response;
} PBAP_PULL_VCARD_ENTRY_PARAMS;

/**
 * A union of all of the various PBAP parameters.
 */
typedef union {
    PBAP_PULL_PHONE_BOOK_PARAMS         pullPhoneBook;
    PBAP_PULL_VCARD_LISTING_PARAMS      pullvCardListing;
    PBAP_PULL_VCARD_ENTRY_PARAMS        pullvCardEntry;
} PBAP_PARAM_UNION;


#define OBEX_PHONE_BOOK_ACCESS_UUID \
    { 0x79, 0x61, 0x35, 0xF0, 0xF0, 0xC5, 0x11, 0xD8, 0x09, 0x66, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 }

#endif /* _PBAP_SPEC_H */



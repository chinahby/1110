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

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_private.h#1 $ 

$Log: $
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/


#ifndef _PBAP_PRIVATE_H
#define _PBAP_PRIVATE_H

#include "oi_bytestream.h"
#include "oi_obexspec.h"
#include "pbap_spec.h"

/** application parameter manipulation functions */

typedef struct _PBAP_APP_PARAM_BOOKKEEPING {
    OI_OBEX_BYTESEQ byteseq;
    OI_INT16 nextTagPos;
    OI_BYTE_STREAM bs;
} PBAP_APP_PARAM_BOOKKEEPING;

typedef void *PBAP_GENERIC_PARAMS;


/** Resets the parameter bookkeeping data structure in preperation for building
 * an application parameters header.
 * @param bk    Pointer to the bookkeeping structure
 * @param data  Pointer to the raw data buffer to contain the finished header
 * @param len   Length of the raw data buffer
 */
void PBAP_InitAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len);


/** Appends an application parameter entry in UINT8 format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void PBAP_AppendAppParamUINT8(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 value);


/** Appends an application parameter entry in UINT16 (big endian) format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void PBAP_AppendAppParamUINT16(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 value);


/** Appends an application parameter entry in UINT32 (big endian) format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void PBAP_AppendAppParamUINT32(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 value);


/** Appends an application parameter entry in UINT64 (big endian) format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void PBAP_AppendAppParamUINT64(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, UINT64 value);


/** Appends an application parameter entry in UUID128 format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param uuid  Pointer to the UUID. This data is copied by the function.
 */
void PBAP_AppendAppParamUUID128(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid);

/** Appends an application parameter entry in UTF-8 format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void PBAP_AppendAppParamUTF8(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag,  OI_OBEX_BYTESEQ *value);


/** Finalizes the bookkeeping data structure. After this call bk is passed
 * to BIP_GetRequest or BIP_SetRequest via the "appParam" function parameter.
 * @param bk    Pointer to the bookkeeping structure
 */
void PBAP_FinishAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk);

void PBAP_ResetAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len);
OI_STATUS PBAP_FindAppParamUINT8(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 *value);
OI_STATUS PBAP_FindAppParamUINT16(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 *value);
OI_STATUS PBAP_FindAppParamUINT32(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 *value);
OI_STATUS PBAP_FindAppParamUINT64(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, UINT64 *value);
OI_STATUS PBAP_FindAppParamUUID128(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid);

extern const OI_OBEX_BYTESEQ OI_PBAP_PullPhoneBookType;
extern const OI_OBEX_BYTESEQ OI_PBAP_PullvCardListingType;
extern const OI_OBEX_BYTESEQ OI_PBAP_PullvCardEntryType;

typedef enum {
    PBAP_CMD_NONE = 0,
    PBAP_CMD_PULL_PHONE_BOOK,
    PBAP_CMD_SET_PHONE_BOOK,
    PBAP_CMD_PULL_VCARD_LISTING,
    PBAP_CMD_PULL_VCARD_ENTRY
} PBAP_CMD;

typedef enum {
    PBAP_PARAM_NONE = 0,
    PBAP_PARAM_NAME = BIT0,
    PBAP_PARAM_ORDER = BIT1,
    PBAP_PARAM_VALUE = BIT2,
    PBAP_PARAM_ATTRIBUTE = BIT3,
    PBAP_PARAM_FILTER = BIT4,
    PBAP_PARAM_FORMAT = BIT5,
    PBAP_PARAM_MAX_LIST_COUNT = BIT6,
    PBAP_PARAM_LIST_START_OFFSET = BIT7,
    PBAP_PARAM_PHONE_BOOK_SIZE = BIT8,
    PBAP_PARAM_NEW_MISSED_CALLS = BIT9,
    PBAP_PARAM_DATA = BIT10,
    PBAP_PARAM_LENGTH = BIT11,
    PBAP_PARAM_TYPE = BIT12
} PBAP_PARAM_FLAGS;

typedef struct {    
    PBAP_PARAM_FLAGS flags;

    OI_OBEX_BYTESEQ *type;
    OI_OBEX_UNICODE *name;
    OI_UINT32 length;
    OI_OBEX_BYTESEQ *body;
    OI_BOOL   final;
    
    OI_UINT8  order;
    OI_UINT8  attribute;
    OI_OBEX_BYTESEQ value;

    UINT64    filter;
    OI_UINT8  format;
    OI_UINT16 listCount;
    OI_UINT16 startOffset;

    OI_UINT16 phoneBookSize;
    OI_UINT8  missedCalls;

} PBAP_REQUEST_HEADERS;

void PBAP_FreeHeaders(PBAP_REQUEST_HEADERS *reqHeaders);

OI_STATUS PBAP_UnmarshallHeaderData(PBAP_REQUEST_HEADERS *reqHeaders,
                                    OI_OBEX_HEADER_LIST *obexHeaders);


#endif /* _PBAP_PRIVATE_H */

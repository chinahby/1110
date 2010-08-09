#ifdef FEATURE_BT_EXTPF_CTP
/**
@file  
functions for encoding and decoding of TCS protocol data
*/

/** \addtogroup Telephony Telephony APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _TCS_CODING_H
#define _TCS_CODING_H

#include "oi_tcs_spec.h"
#include "oi_tcs_msg.h"
#include "oi_bytestream.h"

/** a data structure for holding all possible information elements */
typedef struct {
    OI_TCS_AUDIO_CONTROL audioControl;
    OI_TCS_BEARER *bearer;
    OI_TCS_CALL_CLASS class;
    OI_TCS_NUMBER *calledNumber;
    OI_TCS_NUMBER *callingNumber;
    OI_TCS_CAUSE cause;
    TCS_CLOCK_OFFSET offset;
    TCS_CONFIG_DATA *config;
    OI_L2CAP_CID destCID;
    OI_CHAR keypadFacility;
    OI_TCS_PROGRESS progress;
    SCO_HANDLE handle;
    OI_BOOL sendingComplete;
    OI_TCS_SIGNAL signal;
    OI_UINT8 *companySpecific;
    OI_UINT8 companySpecificSize;
} TCS_ELEMS;

/** Encode a single octet information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeSingle(OI_BYTE_STREAM *bs, TCS_ELEM elem);

/** Encode a double octet information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeDouble(OI_BYTE_STREAM *bs, TCS_ELEM elem, OI_UINT8 value);

/** Encode a variable length information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeVariable(OI_BYTE_STREAM *bs, TCS_ELEM elem, OI_UINT8 *data, OI_UINT16 dataLen);

/** Encode a bearer into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeBearer(OI_BYTE_STREAM *bs, OI_TCS_BEARER *bearer);

/** 
 * Encode a OI_TCS_NUMBER type into the specified byte stream. 
 * 
 * @param bs      pointer to the byte stream
 * @param number  pointer to the number to encode
 */
OI_STATUS OI_Tcs_EncodeNumber(OI_BYTE_STREAM *bs,
                          OI_TCS_NUMBER *number);

/**
 * Encode configuration data into a byte stream.
 *
 * @param bs      pointer to the byte stream
 * @param config  pointer to the configuration data
 */
OI_STATUS OI_Tcs_EncodeConfig(OI_BYTE_STREAM *bs,
                         TCS_CONFIG_DATA *config);

OI_STATUS OI_Tcs_DecodeDouble(OI_BYTE_STREAM *bs,
                              TCS_ELEM type,
                              TCS_ELEMS *elems );

OI_STATUS OI_Tcs_DecodeConfig(OI_BYTE_STREAM *bs,
                              TCS_ELEMS *elems,
                              OI_UINT8 length);

OI_STATUS OI_Tcs_DecodeBearer(OI_BYTE_STREAM *bs,
                              TCS_ELEMS *elems,
                              OI_UINT8 length);

OI_STATUS OI_Tcs_DecodeNumber(OI_BYTE_STREAM *bs,
                              TCS_ELEM type,
                              TCS_ELEMS *elems,
                              OI_UINT8 length);

OI_STATUS OI_Tcs_DecodeVariable(OI_BYTE_STREAM *bs,
                                TCS_ELEM type,
                                TCS_ELEMS *elems); 

OI_STATUS OI_Tcs_DecodeMsg(OI_BYTE_STREAM *bs,
                           TCS_ELEMS *elems);
 
#ifdef OI_DEBUG
#error code not present
#else
#define OI_Tcs_DumpMsg(prefix, addr, msgType, elems)
#endif

#endif /* _TCS_CODING_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_CTP */


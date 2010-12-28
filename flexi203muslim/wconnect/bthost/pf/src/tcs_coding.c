#ifdef FEATURE_BT_EXTPF_CTP
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/tcs_coding.c#1 $ 

$Log: $
*
*    #4        14 Mar 2006            RY
* Added proper featurization.
*
*    #3        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
===========================================================================*/

/**
@file  
functions for encoding and decoding of TCS protocol data
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_TCS

#include "oi_tcs_coding.h"
#include "oi_tcs_spec.h"
#include "oi_tcs_msg.h"

#include "oi_bytestream.h"
#include "oi_assert.h"
#include "oi_debug.h"

/** Encode a single octet information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeSingle(OI_BYTE_STREAM *bs, 
                              TCS_ELEM elem) 
{
    if (ByteStream_NumWriteBytesAllowed(*bs) < 1) {
        return OI_STATUS_MTU_EXCEEDED;
    }

    ByteStream_PutUINT8(*bs, elem);
    return OI_OK;
}

/** Encode a double octet information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeDouble(OI_BYTE_STREAM *bs, 
                              TCS_ELEM elem, 
                              OI_UINT8 value) 
{
    if (ByteStream_NumWriteBytesAllowed(*bs) < 2) {
        return OI_STATUS_MTU_EXCEEDED;
    }

    ByteStream_PutUINT8(*bs, elem);
    ByteStream_PutUINT8(*bs, value);
    return OI_OK;
}

/** Encode a variable length information element into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeVariable(OI_BYTE_STREAM *bs, 
                                TCS_ELEM elem, 
                                OI_UINT8 *data, 
                                OI_UINT16 dataLen)
{
    OI_UINT8 i;

    if (ByteStream_NumWriteBytesAllowed(*bs) < (2 + dataLen)) {
        return OI_STATUS_MTU_EXCEEDED;
    }

    ByteStream_PutUINT8(*bs, elem);
    ByteStream_PutUINT8(*bs, dataLen);
    for(i = 0; i < dataLen; i++) {
        ByteStream_PutUINT8(*bs, data[i]);
    }
    return OI_OK;
}

/** Encode a bearer into the specified bytestream. */
OI_STATUS OI_Tcs_EncodeBearer(OI_BYTE_STREAM *bs, 
                              OI_TCS_BEARER *bearer)
{
    OI_ASSERT(bs);
    OI_ASSERT(bearer);

    if (ByteStream_NumWriteBytesAllowed(*bs) < 1) {
        return OI_STATUS_MTU_EXCEEDED;
    }
    ByteStream_PutUINT8(*bs, TCS_ELEM_BEARER_CAPABILITY);

    if (OI_TCS_BEARER_LINK_TYPE_SCO == bearer->type) {
        if (ByteStream_NumWriteBytesAllowed(*bs) < 3) {
            return OI_STATUS_MTU_EXCEEDED;
        }
        //length
        ByteStream_PutUINT8(*bs, (OI_UINT8) 2);
        //type
        ByteStream_PutUINT8(*bs, (OI_UINT8) bearer->type);
        //packet type
        ByteStream_PutUINT8(*bs, (OI_UINT8) (bearer->info.sco.packetType | bearer->info.sco.coding) );
    } else if (OI_TCS_BEARER_LINK_TYPE_ACL == bearer->type) {
        OI_L2CAP_FLOWSPEC *flowspec = bearer->info.acl.flowspec;

        if (ByteStream_NumWriteBytesAllowed(*bs) < 25) {
            return OI_STATUS_MTU_EXCEEDED;
        }
        //length
        ByteStream_PutUINT8(*bs, (OI_UINT8) 22);
        //type
        ByteStream_PutUINT8(*bs, (OI_UINT8) bearer->type);
        
        //ACL configuration
        ByteStream_PutUINT8(*bs, (OI_UINT8) 0); //flags - must be set to 0
        if (flowspec) {
            ByteStream_PutUINT8(*bs,  flowspec->serviceType);
            ByteStream_PutUINT32(*bs, flowspec->tokenRate, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, flowspec->tokenBucketSize, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, flowspec->peakBandwidth, TCS_BYTE_ORDER); 
            ByteStream_PutUINT32(*bs, flowspec->latency, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, flowspec->delayVariation, TCS_BYTE_ORDER);
        } else {
            ByteStream_PutUINT8(*bs,  0);
            ByteStream_PutUINT32(*bs, 0, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, 0, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, 0, TCS_BYTE_ORDER); 
            ByteStream_PutUINT32(*bs, 0, TCS_BYTE_ORDER);
            ByteStream_PutUINT32(*bs, 0, TCS_BYTE_ORDER);
        }
        
        OI_ASSERT(OI_TCS_BEARER_UI_LAYER2_RFCOMM_OVER_L2CAP == bearer->info.acl.layer2);
        ByteStream_PutUINT8(*bs, (OI_UINT8) (bearer->info.acl.layer2 | bearer->info.acl.layer3) );        
    }
    else if (OI_TCS_BEARER_LINK_TYPE_NONE == bearer->type) {
        if (ByteStream_NumWriteBytesAllowed(*bs) < 2) {
            return OI_STATUS_MTU_EXCEEDED;
        }
        //length
        ByteStream_PutUINT8(*bs, (OI_UINT8) 1);
        //type
        ByteStream_PutUINT8(*bs, (OI_UINT8) bearer->type);
    }
    return OI_OK;
}

/** 
 * This function encodes an OI_TCS_NUMBER into the specified byte stream. 
 * 
 * @param bs      the byte stream
 * @param number  the number to encode
 */
OI_STATUS OI_Tcs_EncodeNumber(OI_BYTE_STREAM *bs,
                              OI_TCS_NUMBER *number) 
{
    OI_UINT8 i;
    OI_UINT8 length;
    TCS_ELEM elemType;

    OI_ASSERT(bs != NULL);
    OI_ASSERT(number != NULL);

    OI_ASSERT(TRUE == number->calling || FALSE == number->calling);

    if (ByteStream_NumWriteBytesAllowed(*bs) < (3 + (number->calling ? 1 : 0) + number->numDigits)) {
        return OI_STATUS_MTU_EXCEEDED;
    }

    if (TRUE == number->calling) { //calling party
        length = number->numDigits + 2; // +2 for type/plan byte and presentation/screening byte
        elemType = TCS_ELEM_CALLING_PARTY_NUMBER;
    }
    else { //called party
        length = number->numDigits + 1; // +1 for type/plan byte
        elemType = TCS_ELEM_CALLED_PARTY_NUMBER;
    }

    //type
    ByteStream_PutUINT8(*bs, elemType);
    //length
    ByteStream_PutUINT8(*bs, length);
    //type/plan
    ByteStream_PutUINT8(*bs, (number->type | number->plan) );
    //presentation/screening
    if (TRUE == number->calling) {
        ByteStream_PutUINT8(*bs, (number->presentation | number->screening));
    }

    //digits
    for (i = 0; i < number->numDigits; i++) {
        ByteStream_PutUINT8(*bs, number->digits[i]);
    }
    return OI_OK;
}

/**
 * Encode configuration data into a byte stream.
 *
 * @param bs  The byte stream.
 * @param config  The configuration data.
 */
OI_STATUS OI_Tcs_EncodeConfig(OI_BYTE_STREAM *bs,
                              TCS_CONFIG_DATA *config)
{
    OI_UINT8 i;
    OI_UINT8 j;

    OI_ASSERT(bs != NULL);
    OI_ASSERT(config != NULL);
    OI_ASSERT(config->numMembers <= TCS_MAX_WUG_SIZE);
    
    if (0 == config->numMembers) {
        return OI_OK;
    }
    if (ByteStream_NumWriteBytesAllowed(*bs) < (2 + (config->numMembers * (2 + OI_BD_ADDR_BYTE_SIZE + OI_BT_LINK_KEY_SIZE)))) {
        return OI_STATUS_MTU_EXCEEDED;
    }

    //type
    ByteStream_PutUINT8(*bs, TCS_ELEM_CONFIG_DATA);
    //length
    ByteStream_PutUINT8(*bs, (24 * config->numMembers));
    //members
    for (i = 0; i < config->numMembers; i++) {
        ByteStream_PutUINT8(*bs, config->members[i].number1);
        ByteStream_PutUINT8(*bs, config->members[i].number2);
        ByteStream_PutBDADDR(*bs, config->members[i].addr, TCS_BYTE_ORDER);
        for (j = 0; j < OI_BT_LINK_KEY_SIZE; j++) {
            ByteStream_PutUINT8(*bs, config->members[i].linkKey.key[j]);
        }
    }
    return OI_OK;
}


/*****************************************************************************

  Decoding

*****************************************************************************/

OI_STATUS OI_Tcs_DecodeDouble(OI_BYTE_STREAM *bs,
                              TCS_ELEM type,
                              TCS_ELEMS *elems) 
{
    OI_UINT8 temp = 0;

    OI_ASSERT(bs != NULL);
    OI_ASSERT(elems != NULL);

    ByteStream_GetUINT8_Checked(*bs, temp);

    switch (type) {
    case TCS_ELEM_CALL_CLASS:
        elems->class = (OI_TCS_CALL_CLASS)temp;
        break;
    case TCS_ELEM_CAUSE:
        elems->cause = (OI_TCS_CAUSE)temp;
        break;
    case TCS_ELEM_PROGRESS_INDICATOR:
        elems->progress = (OI_TCS_PROGRESS)temp;
        break;
    case TCS_ELEM_SIGNAL:
        elems->signal = (OI_TCS_SIGNAL)temp;
        break;
    case TCS_ELEM_KEYPAD_FACILITY:
        elems->keypadFacility = (OI_CHAR)temp;
        break;    
    case TCS_ELEM_SCO_HANDLE:
        elems->handle = (SCO_HANDLE)temp;
        break;
    default:
        return OI_TCS_INVALID_PACKET;
    }
    return OI_OK;
}

OI_STATUS OI_Tcs_DecodeConfig(OI_BYTE_STREAM *bs,
                              TCS_ELEMS *elems,
                              OI_UINT8 length) 
{
    /*
     * We currently don't do any validation on this element since it's 
     * unused by us (no WUG support).
     */
    ByteStream_Skip_Checked(*bs, length);
    return OI_OK;
}

OI_STATUS OI_Tcs_DecodeBearer(OI_BYTE_STREAM *bs,
                              TCS_ELEMS *elems,
                              OI_UINT8 length) 
{
    OI_UINT8 info = 0;
    OI_TCS_BEARER *bearer = NULL;
    OI_L2CAP_FLOWSPEC *flowspec = NULL;
    OI_UINT8 tmp = 0;
    OI_STATUS status;

    OI_ASSERT(bs);
    OI_ASSERT(elems);

    bearer = OI_Malloc(sizeof(OI_TCS_BEARER));
    if (!bearer) {
        status = OI_STATUS_OUT_OF_MEMORY;
        goto Error;
    }
    OI_MemSet(bearer, 0, sizeof(OI_TCS_BEARER));

    ByteStream_GetUINT8_Checked(*bs, tmp);
    bearer->type = (OI_TCS_BEARER_LINK_TYPE)tmp;
    if (OI_TCS_BEARER_LINK_TYPE_SCO == bearer->type) {
        if (length != 2) {
            status = OI_TCS_INVALID_PACKET;
            goto Error;
        }
        ByteStream_GetUINT8_Checked(*bs, info);
        bearer->info.sco.packetType = (OI_TCS_BEARER_PACKET_TYPE)(info & OI_TCS_BEARER_PACKET_TYPE_MASK);
        bearer->info.sco.coding = (OI_TCS_BEARER_CODING)(info & OI_TCS_BEARER_CODING_MASK);
    } else if (OI_TCS_BEARER_LINK_TYPE_ACL == bearer->type) {
        if (length != 24) {
            status = OI_TCS_INVALID_PACKET;
            goto Error;
        }
        flowspec = OI_Malloc(sizeof(OI_L2CAP_FLOWSPEC));
        if (!flowspec) {
            status = OI_STATUS_OUT_OF_MEMORY;
            goto Error;
        }
        ByteStream_GetUINT8_Checked(*bs, tmp); //flags - discard
        ByteStream_GetUINT8_Checked(*bs, flowspec->serviceType);
        ByteStream_GetUINT32_Checked(*bs, flowspec->tokenRate, TCS_BYTE_ORDER);
        ByteStream_GetUINT32_Checked(*bs, flowspec->tokenBucketSize, TCS_BYTE_ORDER);
        ByteStream_GetUINT32_Checked(*bs, flowspec->peakBandwidth, TCS_BYTE_ORDER);
        ByteStream_GetUINT32_Checked(*bs, flowspec->latency, TCS_BYTE_ORDER);
        ByteStream_GetUINT32_Checked(*bs, flowspec->delayVariation, TCS_BYTE_ORDER);
        ByteStream_GetUINT8_Checked(*bs, tmp);
        bearer->info.acl.layer2 = (OI_TCS_BEARER_UI_LAYER2)(tmp & 0xF);
        bearer->info.acl.layer3 = (OI_TCS_BEARER_UI_LAYER3)(tmp & 0xF0);        
        bearer->info.acl.flowspec = flowspec;
    } else if (OI_TCS_BEARER_LINK_TYPE_NONE == bearer->type) {
        if (length != 1) {
            status = OI_TCS_INVALID_PACKET;
            goto Error;
        }
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Invalid bearer type"));
#endif
        status = OI_STATUS_INVALID_PARAMETERS;
        goto Error;
    }

    elems->bearer = bearer;
    return OI_OK;

Error:
    OI_FreeIf(&flowspec);
    OI_FreeIf(&bearer);
    return status;
}


OI_STATUS OI_Tcs_DecodeNumber(OI_BYTE_STREAM *bs,
                              TCS_ELEM type,
                              TCS_ELEMS *elems,
                              OI_UINT8 length) 
{
    OI_TCS_NUMBER *number = NULL;
    OI_UINT8 temp = 0;
    OI_STATUS status;
    OI_UINT8 i;

    number = OI_Malloc(sizeof(OI_TCS_NUMBER));
    if (NULL == number) {
        status = OI_STATUS_OUT_OF_MEMORY;
        goto Error;
    }
    number->digits = NULL;

    if (TCS_ELEM_CALLING_PARTY_NUMBER == type) {
        if (length < 2) {
            status = OI_TCS_INVALID_PACKET;
            goto Error;
        }
        elems->callingNumber = number;
        number->numDigits = length - 2; /* calling party header has one byte more than called party header */
        number->calling = TRUE;
    } else {
        if (length < 1) {
            status = OI_TCS_INVALID_PACKET;
            goto Error;
        }
        elems->calledNumber = number;
        number->numDigits = length - 1;
        number->calling = FALSE;
    }

    ByteStream_GetUINT8_Checked(*bs, temp);
    number->type = (OI_TCS_NUMBER_TYPE)(OI_TCS_NUMBER_TYPE_MASK & temp);
    number->plan = (OI_TCS_NUMBER_PLAN)(OI_TCS_NUMBER_PLAN_MASK & temp);
    
    if (TRUE == number->calling) {
        ByteStream_GetUINT8_Checked(*bs, temp);
        number->presentation = (OI_TCS_PRESENTATION)(OI_TCS_PRESENTATION_MASK & temp);
        number->screening = (OI_TCS_SCREENING)(OI_TCS_SCREENING_MASK & temp);
    } else {
        number->presentation = (OI_TCS_PRESENTATION)0;
        number->screening = (OI_TCS_SCREENING)0;
    }

    if (number->numDigits > 0) {
        number->digits = OI_Malloc(number->numDigits);
        if (NULL == number->digits) {
            status = OI_STATUS_OUT_OF_MEMORY;
            goto Error;
        }
        for (i = 0; i < number->numDigits; i++) {
            ByteStream_GetUINT8_Checked(*bs, number->digits[i]);
        }
    }
    return OI_OK;

Error:
    OI_FreeIf(&number->digits);
    OI_FreeIf(&number);
    return status;
}

OI_STATUS OI_Tcs_DecodeVariable(OI_BYTE_STREAM *bs,
                                TCS_ELEM type,
                                TCS_ELEMS *elems) 
{
    OI_UINT8 length = 0;
    OI_UINT8 tmp = 0;
    OI_STATUS status = OI_OK;

    OI_ASSERT(bs != NULL);
    OI_ASSERT(elems != NULL);

    //length
    ByteStream_GetUINT8_Checked(*bs, length);

    switch (type) {
    case TCS_ELEM_CLOCK_OFFSET:
        if (length != 2) {
            return OI_TCS_INVALID_PACKET;
        }
        ByteStream_GetUINT16_Checked(*bs, elems->offset, TCS_BYTE_ORDER);
        break;
    case TCS_ELEM_DEST_CID:
        if (length != 2) {
            return OI_TCS_INVALID_PACKET;
        }
        ByteStream_GetUINT16_Checked(*bs, elems->destCID, TCS_BYTE_ORDER);
        break;
    case TCS_ELEM_AUDIO_CONTROL:
        if (length != 1) {
            return OI_TCS_INVALID_PACKET;
        }
        ByteStream_GetUINT8_Checked(*bs, tmp);
        elems->audioControl = (OI_TCS_AUDIO_CONTROL)tmp;
        break;
    case TCS_ELEM_CONFIG_DATA:
        status = OI_Tcs_DecodeConfig(bs, elems, length);
        break;
    case TCS_ELEM_BEARER_CAPABILITY:
        status = OI_Tcs_DecodeBearer(bs, elems, length);
        break;
    case TCS_ELEM_COMPANY_SPECIFIC:
        elems->companySpecific = ByteStream_GetCurrentBytePointer(*bs);
        elems->companySpecificSize = length;
        ByteStream_Skip_Checked(*bs, length);
        break;
    case TCS_ELEM_CALLING_PARTY_NUMBER:
    case TCS_ELEM_CALLED_PARTY_NUMBER:
        status = OI_Tcs_DecodeNumber(bs, type, elems, length);
        break;
    default:
        status = OI_TCS_INVALID_ELEMENT_TYPE;
        break;
    }
    return status;
}

/**
 * Decode the data elements found in the specified byte stream.
 *
 * @param bs  The bytestream from which to read the data.
 * @param elems  The data structure in which to put the results.
 */
OI_STATUS OI_Tcs_DecodeMsg(OI_BYTE_STREAM *bs,
                           TCS_ELEMS *elems) 
{
    TCS_ELEM type;
    OI_STATUS result = OI_OK;
    OI_UINT8 tmp = 0;

    OI_ASSERT(bs != NULL);
    OI_ASSERT(elems != NULL);

    while (ByteStream_NumReadBytesAvail(*bs) > 0) {
        ByteStream_GetUINT8_Checked(*bs, tmp);
        type = (TCS_ELEM)tmp;

        switch ((type & TCS_ELEM_TYPE_MASK)) {
            case TCS_ELEM_TYPE_SINGLE:
                if (type == TCS_ELEM_SENDING_COMPLETE) {
                    elems->sendingComplete = TRUE;
                } else {
                    result = OI_TCS_INVALID_PACKET;
                }
                break;
            case TCS_ELEM_TYPE_DOUBLE:
                result = OI_Tcs_DecodeDouble(bs, type, elems);
                break;
            case TCS_ELEM_TYPE_VARIABLE:
                result = OI_Tcs_DecodeVariable(bs, type, elems);
                break;
        }
        if (!OI_SUCCESS(result)) {
            OI_LOG_ERROR(("Error decoding message\n"));
            break;
        } else if (ByteStream_Error(*bs)) {
            OI_LOG_ERROR(("Error decoding message\n"));
            result = OI_STATUS_PARSE_ERROR;
            break;
        }
    }
    return result;
}
#endif /* FEATURE_BT_EXTPF_CTP */


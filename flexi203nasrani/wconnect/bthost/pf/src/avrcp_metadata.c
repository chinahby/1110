/**
 * @file
 * @internal  
 * A/V Remote Control Profile internal controller-specific functions
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVRCP

#include "oi_avctp.h"
#include "oi_avrcp_helper.h"
#include "oi_avrcp_consts.h"
#include "oi_avrcp_metadata.h"
#include "oi_debug.h"
#include "oi_bytestream.h"
#include "oi_utils.h"
#include "oi_argcheck.h"


#define ERROR_RESPONSE_OPERAND_COUNT 8

/**
 * This helper function maps OI_Status status to Error Code that accompanies OI_AVRCP_RESPONSE_REJECTED response.
 */
OI_UINT8 MapStatus2Error( OI_STATUS status )
{
    switch (status) {
        case OI_AVRCP_RESPONSE_INVALID_PDU:
            return OI_AVRCP_RESPONSE_ERROR_INVALID_PDU;
        case OI_AVRCP_RESPONSE_INVALID_PARAMETER:
            return OI_AVRCP_RESPONSE_ERROR_INVALID_PARAMETER;
        case OI_AVRCP_RESPONSE_PARAMETER_NOT_FOUND:
            return OI_AVRCP_RESPONSE_ERROR_PARAMETER_NOT_FOUND;
        default:
            return OI_AVRCP_RESPONSE_ERROR_INTERNAL_ERROR;
    }
}

/**
 * This helper function maps Error Code that accompanies OI_AVRCP_RESPONSE_REJECTED response to OI_Status status.
 */
OI_STATUS MapError2Status( OI_UINT8 error )
{

    switch (error) {
        case OI_AVRCP_RESPONSE_ERROR_INVALID_PDU:
            return OI_AVRCP_RESPONSE_INVALID_PDU;
        case OI_AVRCP_RESPONSE_ERROR_INVALID_PARAMETER:
            return OI_AVRCP_RESPONSE_INVALID_PARAMETER;
        case OI_AVRCP_RESPONSE_ERROR_PARAMETER_NOT_FOUND:
            return OI_AVRCP_RESPONSE_PARAMETER_NOT_FOUND;
        case OI_AVRCP_RESPONSE_ERROR_INTERNAL_ERROR:
            return OI_AVRCP_RESPONSE_INTERNAL_ERROR;
        default:
            OI_DBGPRINT(("Invalid error response code 0x%x", error));
            return OI_AVRCP_REJECTED; /* Error code sent by TG is invalid */
    }
}


/****************************************************************************
 *
 * Common enhanced AVRCPinternal functions
 *
 ****************************************************************************/

static void SetGroupNavigationOperands(OI_BYTE *operands,
                                       OI_UINT16 vendorUniqueID,
                                       OI_BOOL buttonPress)
{
    operands[0] = (((buttonPress) ? OI_AVRCP_OP_PRESS : OI_AVRCP_OP_RELEASE)) | OI_AVRCP_OP_VENDOR_UNIQUE; /* operation ID */
    operands[1] = 0x5; /* operation data field length */
    SetUINT24_BigEndian(&operands[2], OI_AVRCP_METADATA_ID); /* Company ID: BT SIG registered CompanyID */
    SetUINT16_BigEndian(&operands[5], vendorUniqueID); /* Vendor unique id */

}

/****************************************************************************
 *
 * Enhanced AVRCP controller internal functions
 *
 ****************************************************************************/
static OI_STATUS MarshalCommand(OI_AVRCP_COMMAND *command,
                                OI_BYTE *operands,
                                OI_UINT16 *operandCount)
{
    OI_BYTE_STREAM bs;
    OI_UINT16 pos;
    OI_UINT16 paramLen;
    OI_UINT16 i;
#if 0
    OI_TRACE_USER(("MarshalCommand(command=0x%x,operands=0x%x,operandCount=0x%x)", 
                      command, operands, operandCount));
#endif

    ByteStream_Init(bs, operands, (*operandCount));
    ByteStream_Open(bs, BYTESTREAM_WRITE);
    ByteStream_PutUINT24(bs, OI_AVRCP_METADATA_ID, OI_AVRCP_BYTE_ORDER);
    ByteStream_PutUINT8(bs, command->id);
    ByteStream_PutUINT8(bs, OI_AVRCP_PDU_COMPLETE);
    ByteStream_Skip_Checked(bs, 2); /* Fill in parameter length later */
    pos = ByteStream_GetPos(bs);

    switch (command->id) {
        case OI_AVRCP_GET_CAPABILITIES:
            ByteStream_PutUINT8(bs, command->u.capabilityID);
            break;

        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:            
        case OI_AVRCP_GET_PLAY_STATUS:
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:            
            ByteStream_PutUINT8(bs, *(command->u.attribute.attrSetting.ids));
            break;
            
        case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
            ByteStream_PutUINT8(bs, command->u.attribute.numAttrs);
            ByteStream_PutBytes(bs, command->u.attribute.attrSetting.ids, command->u.attribute.numAttrs);
            break;
            
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            ByteStream_PutUINT8(bs, command->u.attrValueText.attrID);
            ByteStream_PutUINT8(bs, command->u.attrValueText.numValues);
            ByteStream_PutBytes(bs, command->u.attrValueText.values, command->u.attrValueText.numValues);
            break;
            
        case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:            
            ByteStream_PutUINT8(bs, command->u.attribute.numAttrs);
            ByteStream_PutBytes(bs, command->u.attribute.attrSetting.attrs, command->u.attribute.numAttrs*2);
            break;

        case OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET:
            ByteStream_PutUINT8(bs, command->u.charSetCT.numCharSets);
            for (i = 0; i < command->u.charSetCT.numCharSets; ++i) {
                ByteStream_PutUINT16(bs, command->u.charSetCT.charSetIDs[i], OI_AVRCP_BYTE_ORDER);
            }
            break;
            
        case OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT:
            ByteStream_PutUINT8(bs, command->u.batteryStatus);
            break;
            
        case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
            ByteStream_PutBytes(bs, command->u.getElementAttributes.id, 8);
            ByteStream_PutUINT8(bs, command->u.getElementAttributes.numAttr);
            for (i = 0; i < command->u.getElementAttributes.numAttr; ++i) {
                ByteStream_PutUINT32(bs, command->u.getElementAttributes.attrs[i], OI_AVRCP_BYTE_ORDER);
            }
            break;

        case OI_AVRCP_REGISTER_NOTIFICATION:
            ByteStream_PutUINT8(bs, command->u.notification.eventID);
            ByteStream_PutUINT32(bs, command->u.notification.playbackInterval, OI_AVRCP_BYTE_ORDER);
            break;
            
        case OI_AVRCP_REQUEST_CONTINUING_RESPONSE:
        case OI_AVRCP_ABORT_CONTINUING_RESPONSE:
            ByteStream_PutUINT8(bs, command->u.responsePDU);
            break;
            
        default:
            (*operandCount) = 0;
            return OI_STATUS_NOT_IMPLEMENTED;
    }

    paramLen = ByteStream_GetPos(bs) - pos;
    ByteStream_SetPos(bs, pos - 2);
    ByteStream_PutUINT16(bs, paramLen, OI_AVRCP_BYTE_ORDER);

    ByteStream_Skip_Checked(bs, paramLen);
    ByteStream_Close(bs);

    (*operandCount) = ByteStream_GetSize(bs);
    return OI_OK;
}

static OI_STATUS SendCommand(OI_BD_ADDR *addr,
                             OI_UINT8 *transaction,
                             OI_AVRCP_COMMAND *cmd)
{
    OI_AVRCP_FRAME frame;
    OI_BYTE operands[OI_AVRCP_MAX_FRAME_LENGTH];
    OI_STATUS status;

    frame.cmd = cmd->type;
    frame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    frame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
    frame.opcode = OI_AVRCP_OPCODE_VENDOR_DEPENDENT;
    frame.operand = operands;

    frame.operandCount = OI_ARRAYSIZE(operands);
    status = MarshalCommand(cmd, frame.operand, &frame.operandCount);
    if (OI_SUCCESS(status)) {
        status = OI_AVRCPController_SendGeneric(addr, transaction, &frame);
    }
    return status;
}

static OI_STATUS UnmarshalResponseShort(OI_BYTE *operand,
                                        OI_UINT16 operandCount,
                                        OI_UINT8 *pduID,
                                        OI_UINT8 *packetType,
                                        OI_BOOL isRejected)
{
    OI_STATUS status = OI_OK;
    OI_BYTE_STREAM bs;
    OI_UINT32      vendorId = 0;
    OI_UINT8 errorCode = 0xff; /* initialize to invalid value */
    
    OI_DBGPRINT2(("OI_AVRCP_UnmarshalResponseShort(operand=0x%x,operandCount=%d)", operand, operandCount));
    
    OI_ASSERT(operand != NULL);

    if ((operandCount == 0) || (operand == NULL)) {
        return OI_STATUS_PARSE_ERROR;
    }
    ByteStream_Init(bs, operand, operandCount);
    ByteStream_Open(bs, BYTESTREAM_READ);
    ByteStream_GetUINT24_Checked(bs, vendorId, OI_AVRCP_BYTE_ORDER);
    ByteStream_GetTyped8_Checked(bs, *pduID, OI_UINT8);
    ByteStream_GetTyped8_Checked(bs, *packetType, OI_UINT8);

    if (isRejected) {
        ByteStream_Skip_Checked(bs, 2); /* skip parameter length */
        ByteStream_GetTyped8_Checked(bs, errorCode, OI_UINT8);
        if (!ByteStream_Error(bs)) {
            status = MapError2Status(errorCode);
        }
    }
    
    if (ByteStream_Error(bs) || (vendorId != OI_AVRCP_METADATA_ID)) {
        return OI_STATUS_PARSE_ERROR;
    }
    
    ByteStream_Close(bs);

    return status;
}


 /****************************************************************************
 *
 * Enhanced AVRCP controller interface functions
 *
 ****************************************************************************/
OI_STATUS OI_AVRCPController_GetCompanyIDs(OI_BD_ADDR *addr, 
                                           OI_UINT8 *transaction)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GetCompanyIDs (addr %:, transaction 0x%x)", addr, *transaction));
#endif 
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_GET_CAPABILITIES;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.capabilityID = OI_AVRCP_CAPABILITY_COMPANY_ID;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_GetSupportedEvents(OI_BD_ADDR *addr, 
                                                OI_UINT8 *transaction)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GetSupportedEvents (addr %:, transaction 0x%x)", addr, *transaction));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_GET_CAPABILITIES;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.capabilityID = OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_ListPlayerAppSettingAttributes(OI_BD_ADDR *addr, 
                                                            OI_UINT8 *transaction)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_ListPlayerAppSettingAttributes (addr %:, transaction 0x%x)", 
                   addr, *transaction));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_ListPlayerAppSettingValues(OI_BD_ADDR *addr, 
                                                        OI_UINT8 *transaction,
                                                        OI_UINT8 attrID)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_ListPlayerAppSettingValues (addr %:, transaction 0x%x, attrID 0x%x)", 
                   addr, *transaction, attrID));
#endif 
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.attribute.attrSetting.ids  = &attrID;
    cmd.u.attribute.numAttrs  = 1;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_GetCurrentPlayerAppSettingValue(OI_BD_ADDR *addr, 
                                                             OI_UINT8 *transaction,
                                                             OI_UINT8 count,
                                                             OI_UINT8 *attrIDs)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GetCurrentPlayerAppSettingValue (addr %:, transaction 0x%x, count %d, attrs 0x%x)", 
                   addr, *transaction, count, attrIDs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    ARGCHECK(attrIDs != NULL);
    ARGCHECK(count != 0);

    cmd.id   = OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.attribute.attrSetting.ids  = attrIDs;
    cmd.u.attribute.numAttrs  = count;
    return SendCommand(addr, transaction, &cmd);    
}

OI_STATUS OI_AVRCPController_SetPlayerAppSettingValue(OI_BD_ADDR *addr, 
                                                      OI_UINT8 *transaction,
                                                      OI_UINT8 count,
                                                      OI_AVRCP_ATTRIBUTE_PAIR attrs)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_SetPlayerAppSettingValue (addr %:, transaction 0x%x, count %d, attrs 0x%x)", 
                   addr, *transaction, count, attrs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    ARGCHECK(attrs != NULL);
    ARGCHECK(count != 0);

    cmd.id   = OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE;
    cmd.type = OI_AVRCP_CTYPE_CONTROL;
    cmd.u.attribute.attrSetting.attrs  = attrs;
    cmd.u.attribute.numAttrs  = count;
    return SendCommand(addr, transaction, &cmd);    
}

OI_STATUS OI_AVRCPController_GetPlayerAppSettingAttributeText(OI_BD_ADDR *addr, 
                                                              OI_UINT8 *transaction,
                                                              OI_UINT8 count,
                                                              OI_UINT8 *attrIDs)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GetPlayerAppSettingAttributeText (addr %:, transaction 0x%x, count %d, attrs 0x%x)", 
                   addr, *transaction, count, attrIDs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    ARGCHECK(attrIDs != NULL);
    ARGCHECK(count != 0);

    cmd.id   = OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.attribute.attrSetting.ids  = attrIDs;
    cmd.u.attribute.numAttrs  = count;

    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_GetPlayerAppSettingValueText(OI_BD_ADDR *addr, 
                                                          OI_UINT8 *transaction,
                                                          OI_UINT8 attrID,
                                                          OI_UINT8 count,
                                                          OI_UINT8 *valueIDs)
{
   OI_AVRCP_COMMAND cmd;
#if 0
   OI_TRACE_USER(("OI_AVRCPController_GetPlayerAppSettingAttributeValueText (addr %:, transaction 0x%x, attr 0x%x, count %d, values 0x%x)", 
                   addr, *transaction, attrID, count, valueIDs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    ARGCHECK(valueIDs != NULL);
    ARGCHECK(count != 0);

    cmd.id   = OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    cmd.u.attrValueText.attrID  = attrID;
    cmd.u.attrValueText.numValues  = count;
    cmd.u.attrValueText.values = valueIDs;

    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_InformCharacterSet(OI_BD_ADDR *addr, 
                                                OI_UINT8 *transaction,
                                                OI_UINT8 count,
                                                OI_UINT16 *charSetIDs)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_InformCharacterStatus (addr %:, transaction 0x%x, count %d charSet 0x%x)",
                   addr, *transaction, count, charSetIDs));
#endif    
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    ARGCHECK(charSetIDs != NULL);
    ARGCHECK(count != 0);
    
    cmd.id   = OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET;
    cmd.type = OI_AVRCP_CTYPE_CONTROL;
    cmd.u.charSetCT.numCharSets = count;
    cmd.u.charSetCT.charSetIDs = charSetIDs;
    
    return SendCommand(addr, transaction, &cmd);

}

OI_STATUS OI_AVRCPController_InformBatteryStatusOfCT(OI_BD_ADDR *addr, 
                                                     OI_UINT8 *transaction,
                                                     OI_UINT8 batteryStatus)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_InformBatteryStatus (addr %:, transaction 0x%x, battery status0x%x)",
                   addr, *transaction, batteryStatus));
#endif    
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT;
    cmd.type = OI_AVRCP_CTYPE_CONTROL;
    cmd.u.batteryStatus = batteryStatus;
    
    return SendCommand(addr, transaction, &cmd);

}

OI_STATUS OI_AVRCPController_GetPlayStatus(OI_BD_ADDR *addr, 
                                           OI_UINT8 *transaction)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GetPlayStatus (addr %:, transaction 0x%x)", addr, *transaction));
#endif
    ARGCHECK(addr != NULL);
    
    cmd.id   = OI_AVRCP_GET_PLAY_STATUS;
    cmd.type = OI_AVRCP_CTYPE_STATUS;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_RegisterNotification(OI_BD_ADDR *addr, 
                                                  OI_UINT8 *transaction,
                                                  OI_UINT8 eventID,
                                                  OI_UINT32 playbackInterval)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_RegisterNotification (addr %:, transaction 0x%2x, event 0x%4x, playbackInterval 0x%x)",
                   addr, *transaction, eventID, playbackInterval));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_REGISTER_NOTIFICATION;
    cmd.type = OI_AVRCP_CTYPE_NOTIFY;
    cmd.u.notification.eventID = eventID;
    cmd.u.notification.playbackInterval = playbackInterval;
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_GetElementAttributes(OI_BD_ADDR *addr, 
                                                  OI_UINT8 *transaction,
                                                  OI_UINT8 *id,
                                                  OI_UINT8 count,
                                                  OI_UINT32 *attributes)
{
    OI_AVRCP_COMMAND cmd;
#if 0    
    OI_TRACE_USER(("OI_AVRCPController_GetElementAttributes (addr %:, transaction 0x%2x, id 0x%x, count %d, attribuites 0x%x)",
                   addr, *transaction, id, count, attributes));
#endif   
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);

    if (count) {
        ARGCHECK(attributes != NULL);
    } else {
        attributes = NULL; /* just making sure that we don't send some stray pointer */
    }
    
    cmd.id   = OI_AVRCP_GET_ELEMENT_ATTRIBUTES;
    cmd.type = OI_AVRCP_CTYPE_STATUS;

    cmd.u.getElementAttributes.id = id;
    cmd.u.getElementAttributes.numAttr = count;
    cmd.u.getElementAttributes.attrs = attributes;
    
    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_ContinueResponse(OI_BD_ADDR *addr, 
                                              OI_UINT8 *transaction,
                                              OI_UINT8 pduID)
{
    OI_AVRCP_COMMAND cmd;
#if 0
    OI_TRACE_USER(("OI_AVRCPController_ContinueResponse( (addr %:, transaction 0x%2x, PDU 0x%x)",
                   addr, *transaction, pduID));
#endif  
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_REQUEST_CONTINUING_RESPONSE;
    cmd.type = OI_AVRCP_CTYPE_CONTROL;

    cmd.u.responsePDU = pduID;

    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_AbortResponse(OI_BD_ADDR *addr, 
                                           OI_UINT8 *transaction,
                                           OI_UINT8 pduID)
{
    OI_AVRCP_COMMAND cmd;
#if 0    
    OI_TRACE_USER(("OI_AVRCPController_AbortResponse( (addr %:, transaction 0x%2x, PDU 0x%x)",
                   addr, *transaction, pduID));
#endif 
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);
    
    cmd.id   = OI_AVRCP_ABORT_CONTINUING_RESPONSE;
    cmd.type = OI_AVRCP_CTYPE_CONTROL;

    cmd.u.responsePDU = pduID;

    return SendCommand(addr, transaction, &cmd);
}

OI_STATUS OI_AVRCPController_GroupNavigationCommand(OI_BD_ADDR *addr,
                                                    OI_UINT8 *transaction,
                                                    OI_UINT16 vendorUniqueID,
                                                    OI_BOOL buttonPress)
{
    OI_BYTE operands[7];
    OI_AVRCP_FRAME frame = {
        OI_AVRCP_CTYPE_CONTROL,
        OI_AVRCP_SUBUNIT_TYPE_PANEL,
        OI_AVRCP_SUBUNIT_ID_0,
        OI_AVRCP_OPCODE_PASS_THROUGH,
    };
#if 0
    OI_TRACE_USER(("OI_AVRCPController_GroupNavigationCommand( (addr %:, transaction 0x%2x, vendof ID 0x%x, buttonPress 0x%x)",
                   addr, *transaction, vendorUniqueID, buttonPress));
#endif    
    ARGCHECK(addr != NULL);
    ARGCHECK(transaction != NULL);

    frame.operand = operands;
    frame.operandCount = OI_ARRAYSIZE(operands);

    SetGroupNavigationOperands( operands, vendorUniqueID, buttonPress);
    return OI_AVRCPController_SendGeneric(addr, transaction, &frame);
}

void AVRCPMetadata_RecvInd(OI_BD_ADDR *addr,
                           OI_UINT8 transaction,
                           OI_AVRCP_FRAME *frame)
{
    OI_STATUS status;
    OI_UINT8 pduID = 0;
    OI_UINT8 packetType = 0;
    OI_BOOL isRejected = (frame->cmd == OI_AVRCP_RESPONSE_REJECTED) ? TRUE : FALSE;

    OI_DBGPRINT2(("AVRCPMetadata_RecvInd: cmd 0x%2x",  frame->cmd));

    OI_ASSERT(OI_AVRCP_This->controllerCb->metadataReplyInd);
    OI_ASSERT(frame);
    
    status = UnmarshalResponseShort(frame->operand, frame->operandCount, &pduID, &packetType, isRejected);

    if (frame->cmd == OI_AVRCP_RESPONSE_NOT_IMPLEMENTED){
        status = OI_AVRCP_NOT_IMPLEMENTED;
    }
    
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("Response %!\n", status));  
    }
    
    OI_AVRCP_This->controllerCb->metadataReplyInd(addr, transaction, frame, pduID, packetType, status);    
}

OI_STATUS OI_AVRCPController_UnmarshalMetaResponse(OI_AVRCP_FRAME *frame,
                                                   OI_UINT8 pduID,
                                                   OI_AVRCP_RESPONSE *response,
                                                   OI_AVRCP_RESPONSE_DATA *dataBuffer)
{
    OI_BYTE_STREAM bs;
    OI_UINT8       packetType = 0xff; /* initialize to invalid value */
    OI_UINT16      paramLength = 0;
    OI_UINT16      i;
    OI_UINT8       count;
    dataBuffer->overflow = FALSE;
#if 0    
    OI_TRACE_USER(("OI_AVRCPController_UnmarshalMetaResponse( (frame 0x%x, PDU 0x%x, rspBuffer 0x%x, dataBuffer 0x%x)",
                   frame, pduID, response, dataBuffer));
#endif 
    ARGCHECK(NULL != frame);
    ARGCHECK(NULL != response);
    ARGCHECK(NULL != dataBuffer);

    if ((frame->operandCount == 0) || (frame->operand == NULL)) {
        return OI_STATUS_PARSE_ERROR;
    }
    
    ByteStream_Init(bs, frame->operand, frame->operandCount);
    ByteStream_Open(bs, BYTESTREAM_READ);

    /* Skip Vendor ID and PDU ID */
    ByteStream_Skip_Checked(bs, 4);

    ByteStream_GetTyped8_Checked(bs, packetType, OI_UINT8);

    /* This function cannot handle fragmented packets... */
    if (!ByteStream_Error(bs)) {
        if (packetType != OI_AVRCP_PDU_COMPLETE) {
            OI_DBGPRINT(("Fragmented packet 0x%x", packetType));
            return OI_STATUS_INVALID_PACKET;
        }
    } else {
        return OI_STATUS_PARSE_ERROR;
    }

    ByteStream_GetUINT16_Checked(bs, paramLength, OI_AVRCP_BYTE_ORDER);

    /* Additional check for consistensy */    
    if (paramLength > (frame->operandCount - 7)) {
        OI_DBGPRINT(("Parameter length too big %d, exceeds (op count - 7) %d", paramLength, (frame->operandCount - 7)));
    }
    
    switch (pduID) {
        case OI_AVRCP_GET_CAPABILITIES:
            ByteStream_GetTyped8_Checked(bs, response->u.getCapabilities.capabilityID, OI_UINT8);
            ByteStream_GetTyped8_Checked(bs, response->u.getCapabilities.capabilityCount, OI_UINT8);
            switch (response->u.getCapabilities.capabilityID) {
                case OI_AVRCP_CAPABILITY_COMPANY_ID:
                    response->u.getCapabilities.capabilities.companyID = (OI_UINT32 *) dataBuffer->data;
                    count = OI_ARRAYSIZE(dataBuffer->data)/(sizeof(OI_UINT32));
                    if(count < response->u.getCapabilities.capabilityCount) {
                        dataBuffer->overflow = TRUE;
                    } else {
                        count = response->u.getCapabilities.capabilityCount;
                    }
                    
                    for (i = 0; i < count; ++i) {
                        ByteStream_GetUINT24_Checked(bs, response->u.getCapabilities.capabilities.companyID[i], OI_AVRCP_BYTE_ORDER);
                    }
                    break;
                case OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED:
                    response->u.getCapabilities.capabilities.eventID = (OI_UINT8 *) dataBuffer->data;
                    ByteStream_GetBytes_Checked(bs, response->u.getCapabilities.capabilities.eventID, response->u.getCapabilities.capabilityCount);
                    break;
                default:
                    return OI_STATUS_INVALID_PARAMETERS;
            }
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            response->u.playerAttributes.attrSetting.ids = (OI_UINT8 *) dataBuffer->data;
            ByteStream_GetTyped8_Checked(bs, response->u.playerAttributes.numAttrs, OI_UINT8);
            ByteStream_GetBytes_Checked(bs, response->u.playerAttributes.attrSetting.ids, response->u.playerAttributes.numAttrs);
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            response->u.playerAttributes.attrSetting.attrValues = (OI_UINT8 *) dataBuffer->data;
            ByteStream_GetTyped8_Checked(bs, response->u.playerAttributes.numAttrs, OI_UINT8);
            ByteStream_GetBytes_Checked(bs, response->u.playerAttributes.attrSetting.attrValues, response->u.playerAttributes.numAttrs);
            break;
            
        case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            response->u.playerAttributes.attrSetting.attrs = (OI_AVRCP_ATTRIBUTE_PAIR) dataBuffer->data;
            ByteStream_GetTyped8_Checked(bs, response->u.playerAttributes.numAttrs, OI_UINT8);
            ByteStream_GetBytes_Checked(bs, response->u.playerAttributes.attrSetting.attrs, response->u.playerAttributes.numAttrs * 2);
            break;
            
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            {
                /* text buffer starts at the end of data buffer and grows backward */
                OI_UINT8 *textBuffer = (OI_UINT8*) ((OI_UINT32)dataBuffer->data + OI_AVRCP_MAX_FRAME_LENGTH);
                /* attribute buffer starts at the beginning of data buffer and grows forward */
                OI_AVRCP_ATTRIBUTE_TEXT *attrBuffer;
                
                attrBuffer = response->u.playerAttributesText.attrs = (OI_AVRCP_ATTRIBUTE_TEXT *) dataBuffer->data;
                ByteStream_GetTyped8_Checked(bs, response->u.playerAttributes.numAttrs, OI_UINT8);
                
                for (i = 0; i < response->u.playerAttributes.numAttrs; i++) {
                    OI_UINT8 size = 0;
                    
                    ByteStream_GetTyped8_Checked(bs, response->u.playerAttributesText.attrs[i].id, OI_UINT8);
                    ByteStream_GetUINT16_Checked(bs, response->u.playerAttributesText.attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                    ByteStream_GetTyped8_Checked(bs, size, OI_UINT8);
                    response->u.playerAttributesText.attrs[i].value.sz = size;

                    attrBuffer++;
                    textBuffer = (OI_UINT8 *)((OI_UINT32)textBuffer - response->u.playerAttributesText.attrs[i].value.sz);
                    
                    if ((OI_UINT32)textBuffer < (OI_UINT32)attrBuffer) {
                        dataBuffer->overflow = TRUE;
                        response->u.playerAttributesText.attrs[i].value.sz -= ((OI_UINT32)attrBuffer - (OI_UINT32)textBuffer);
                        OI_DBGPRINT2(("Overflow: attr[%d] textBuffer 0x%x, attrBuffer 0x%x, size %d\n",
                                      i, textBuffer, attrBuffer, response->u.playerAttributesText.attrs[i].value.sz));                        
                    }
                        
                    response->u.playerAttributesText.attrs[i].value.p = textBuffer;

                    ByteStream_GetBytes_Checked(bs, response->u.playerAttributesText.attrs[i].value.p,
                                                response->u.playerAttributesText.attrs[i].value.sz);

                    /* If detected overflow, stop unmarshalling */
                    if (dataBuffer->overflow) {
                        response->u.playerAttributes.numAttrs = (OI_UINT8)(i + 1);
                        break;
                    }
                }
                break;
            }
            
        case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
            {
                /* text buffer starts at the end of data buffer and grows backward */
                OI_UINT8 *textBuffer = (OI_UINT8*) ((OI_UINT32)dataBuffer->data + OI_AVRCP_MAX_FRAME_LENGTH);
                /* attribute buffer starts at the beginning of data buffer and grows forward */
                OI_AVRCP_ELEMENT_ATTRIBUTE *attrBuffer;
                
                attrBuffer = response->u.elementAttributes.attrs = (OI_AVRCP_ELEMENT_ATTRIBUTE *) dataBuffer->data;
                
                ByteStream_GetTyped8_Checked(bs, response->u.elementAttributes.numAttr, OI_UINT8);

                for (i = 0; i < response->u.elementAttributes.numAttr; i++) {
                    OI_UINT16 size = 0;
                    ByteStream_GetUINT32_Checked(bs, response->u.elementAttributes.attrs[i].attrID, OI_AVRCP_BYTE_ORDER);
                    ByteStream_GetUINT16_Checked(bs, response->u.elementAttributes.attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                    ByteStream_GetUINT16_Checked(bs, size,  OI_AVRCP_BYTE_ORDER);
                    response->u.elementAttributes.attrs[i].value.sz = size;
                    attrBuffer++;
                    
                    textBuffer = (OI_UINT8 *)((OI_UINT32)textBuffer - response->u.elementAttributes.attrs[i].value.sz);
                                 
                    if ((OI_UINT32)textBuffer < (OI_UINT32)attrBuffer) {
                        dataBuffer->overflow = TRUE;
                        response->u.elementAttributes.attrs[i].value.sz -= ((OI_UINT32)attrBuffer - (OI_UINT32)textBuffer);
                        OI_DBGPRINT2(("Overflow: element[%d] 0x%x: textBuffer 0x%x, attrBuffer 0x%x, size %d\n",
                                      i, response->u.elementAttributes.attrs[i].attrID,
                                      textBuffer, attrBuffer, response->u.elementAttributes.attrs[i].value.sz));                        
                    }
                    
                    response->u.elementAttributes.attrs[i].value.p = textBuffer;
                    
                    ByteStream_GetBytes_Checked(bs, response->u.elementAttributes.attrs[i].value.p,
                                                response->u.elementAttributes.attrs[i].value.sz);

                    /* If detected overflow, stop unmarshalling */
                    if (dataBuffer->overflow) {
                        response->u.elementAttributes.numAttr = (OI_UINT8)(i + 1);
                        break;
                    }                    
                }
                break;
            }
             
        case OI_AVRCP_GET_PLAY_STATUS:
            ByteStream_GetUINT32_Checked(bs, response->u.playStatus.songLength, OI_AVRCP_BYTE_ORDER);
            ByteStream_GetUINT32_Checked(bs, response->u.playStatus.songPosition, OI_AVRCP_BYTE_ORDER);
            ByteStream_GetTyped8_Checked(bs, response->u.playStatus.status, OI_UINT8);
            break;
            
        case OI_AVRCP_REGISTER_NOTIFICATION:
            response->u.event = (OI_AVRCP_EVENT *) dataBuffer->data;
                
            ByteStream_GetTyped8_Checked(bs, response->u.event->id, OI_UINT8);

            switch (response->u.event->id) {
                case OI_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
                    ByteStream_GetTyped8_Checked(bs, response->u.event->u.playStatus, OI_UINT8);
                    break;
                case OI_AVRCP_EVENT_BATT_STATUS_CHANGED:
                case OI_AVRCP_EVENT_SYSTEM_STATUS_CHANGED:
                    ByteStream_GetTyped8_Checked(bs, response->u.event->u.systemStatus, OI_UINT8);
                    break;
                case OI_AVRCP_EVENT_TRACK_CHANGED:
                    ByteStream_GetBytes_Checked(bs, response->u.event->u.trackIndex, 8);
                    break;
                case OI_AVRCP_EVENT_PLAYBACK_POS_CHANGED:
                    ByteStream_GetUINT32_Checked(bs, response->u.event->u.playbackPosition, OI_AVRCP_BYTE_ORDER);
                    break;
                case OI_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
                    response->u.event->u.deviceSetting.attributes = (OI_AVRCP_ATTRIBUTE_PAIR)((OI_UINT32)(dataBuffer->data) + sizeof(OI_AVRCP_EVENT));                                
                    ByteStream_GetTyped8_Checked(bs, response->u.event->u.deviceSetting.numAttr, OI_UINT8);
                    ByteStream_GetBytes_Checked(bs, response->u.event->u.deviceSetting.attributes, response->u.event->u.deviceSetting.numAttr * 2);
                    break;    

                case OI_AVRCP_EVENT_TRACK_REACHED_END:
                case OI_AVRCP_EVENT_TRACK_REACHED_START:
                    break;

                default:
                     return OI_STATUS_INVALID_PARAMETERS;
            }

            break;

        default:
            return OI_STATUS_NOT_IMPLEMENTED;
    }    
    
    ByteStream_Close(bs);

    if (ByteStream_Error(bs)) {
        return OI_STATUS_PARSE_ERROR;
    } else {
        return OI_OK;
    }
    
}

/****************************************************************************
 *
 * Enhanced AVRCP target internal functions
 *
 ****************************************************************************/
static OI_STATUS CheckPacketOverflow(OI_UINT16 availableBytes,
                                     OI_UINT16 totalCount,
                                     OI_UINT8 dataElSize,
                                     OI_UINT16 *count)
{
    if (availableBytes < totalCount * dataElSize) { 
        *count = availableBytes/dataElSize;    
        return OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
    } else {
        *count = totalCount;
        return OI_OK;
    }
}

static OI_STATUS InitFragment(OI_BD_ADDR  *addr,
                              OI_UINT16    currAttrIndex,
                              OI_UINT16    currAttrDataPos,
                              OI_UINT16    count,
                              OI_UINT8     pdu,
                              void        *attr,
                              OI_UINT8    *data)
{
    
        AVRCP_TRANS_TABLE *table = LookupTransactionTable(addr, FALSE);

        OI_DBGPRINT(("Data Fragmented, sending first packet."));
        
        if (NULL == table) {
            OI_LOG_ERROR(("AVRCP Could not handle fragmented message: connection not found for %:", addr));
            return OI_STATUS_INTERNAL_ERROR;
        }
        
        table->fragment = (AVRCP_FRAGMENT *)OI_Calloc(sizeof(AVRCP_FRAGMENT));
        if (NULL == table->fragment) {
            OI_LOG_ERROR(("AVRCP Could not handle fragmented message: no memory for fragment allocation"));
            return OI_STATUS_OUT_OF_MEMORY;
        }
        
        table->fragment->currAttrIndex = currAttrIndex;
        table->fragment->u.currAttrDataPos = currAttrDataPos;

        table->fragment->count = count;
        table->fragment->PDU = pdu;
        table->fragment->attrFragment.attr = attr;
        table->fragment->attrFragment.data = data;

        return OI_OK;
}

static void UpdateFragment(AVRCP_FRAGMENT *fragment,
                           OI_UINT16       currAttrIndex,
                           OI_UINT16       currAttrDataPos,
                           void           *attr,
                           OI_UINT8       *data)
{
    
    OI_DBGPRINT2(("UpdateFragment currAttrIndex %d,  currAttrDataPos 0x%x, attr 0x%x, data 0x%x",
                  currAttrIndex, currAttrDataPos, attr, data));

    fragment->currAttrIndex = currAttrIndex;
    fragment->u.currAttrDataPos = currAttrDataPos;
    fragment->attrFragment.attr = attr;
    fragment->attrFragment.data = data;

}

static OI_STATUS MarshalResponse(OI_BD_ADDR *addr,
                                 OI_AVRCP_RESPONSE *response,
                                 OI_BYTE *operands,
                                 OI_UINT16 *operandCount)
{

    OI_STATUS status = OI_OK;
    OI_STATUS result;
    OI_BYTE_STREAM bs;
    OI_UINT16 totalSize = *operandCount;
    OI_UINT16 pos;
    OI_UINT16 paramLen;
    OI_UINT16 i;
    OI_UINT16 availableBytes;
    /* Fragmentation info */
    OI_UINT16 count;           /* Total count of attrs/elements/capabilities */
    OI_UINT8 dataElSize;
    
    OI_DBGPRINT2(("MarshalResponse(response=0x%x,operands=0x%x,operandCount=0x%x)", 
                  response, operands, *operandCount));

    ByteStream_Init(bs, operands, (*operandCount));
    ByteStream_Open(bs, BYTESTREAM_WRITE);
    ByteStream_PutUINT24(bs, OI_AVRCP_METADATA_ID, OI_AVRCP_BYTE_ORDER);
    ByteStream_PutUINT8(bs, response->pduID);
    ByteStream_Skip_Checked(bs, 3); /* Fill in packet type and parameter length later */
    pos = ByteStream_GetPos(bs);

    switch (response->pduID) {
    case OI_AVRCP_GET_CAPABILITIES:
            ByteStream_PutUINT8(bs, response->u.getCapabilities.capabilityID);
            ByteStream_PutUINT8(bs, response->u.getCapabilities.capabilityCount);
            
            availableBytes = totalSize - ByteStream_GetPos(bs);

            switch (response->u.getCapabilities.capabilityID) {
                case OI_AVRCP_CAPABILITY_COMPANY_ID:
                    dataElSize = 3;
                    /* Check for message fragmentation */
                    status = CheckPacketOverflow(availableBytes, response->u.getCapabilities.capabilityCount, 3, &count);            
                    for (i = 0; i < count; ++i) {
                        ByteStream_PutUINT24(bs, response->u.getCapabilities.capabilities.companyID[i], OI_AVRCP_BYTE_ORDER);
                    }
                    dataElSize = 4; /* for fragmentation, the company ID offset will be counted in UINT32 */
                    break;
                case OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED:
                    dataElSize = 1;
                    /* Check for message fragmentation */
                    status = CheckPacketOverflow(availableBytes, response->u.getCapabilities.capabilityCount, 1,& count);
                    ByteStream_PutBytes(bs, response->u.getCapabilities.capabilities.eventID, count);
                    break;
                default:
                    return OI_STATUS_INVALID_PARAMETERS;
            }

            /* If the response message has to be fragmented, do some housekeeping */
            if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {

                result = InitFragment(addr,
                                      count,
                                      (OI_UINT16) response->u.getCapabilities.capabilityID,
                                      response->u.getCapabilities.capabilityCount,
                                      response->pduID,
                                      (OI_UINT8 *)((OI_INT)(response->u.getCapabilities.capabilities.eventID) + count*dataElSize),
                                      NULL);
                if (result != OI_OK) {
                    return result;
                }
            }

            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            ByteStream_PutUINT8(bs, response->u.playerAttributes.numAttrs);

            /* Check for message fragmentation */
            availableBytes = totalSize - ByteStream_GetPos(bs);
            status = CheckPacketOverflow(availableBytes, response->u.playerAttributes.numAttrs, 1, &count);

            ByteStream_PutBytes(bs, response->u.playerAttributes.attrSetting.ids, count);

            /* If the response message has to be fragmented, do some housekeeping */
            if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                result = InitFragment(addr,
                                      count,
                                      0xffff,
                                      response->u.playerAttributes.numAttrs,
                                      response->pduID,
                                      &(response->u.playerAttributes.attrSetting.ids[count]),
                                      NULL);
                if (result != OI_OK) {
                    return result;
                }
            }
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            ByteStream_PutUINT8(bs, response->u.playerAttributes.numAttrs);

            /* Check for message fragmentation */
            availableBytes = totalSize - ByteStream_GetPos(bs);
            status = CheckPacketOverflow(availableBytes, response->u.playerAttributes.numAttrs, 1, &count);
            
            ByteStream_PutBytes(bs, response->u.playerAttributes.attrSetting.attrValues, response->u.playerAttributes.numAttrs);
            
            /* If the response message has to be fragmented, do some housekeeping */
            if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                result = InitFragment(addr,
                                      count,
                                      0xffff,
                                      response->u.playerAttributes.numAttrs,
                                      response->pduID,
                                      &(response->u.playerAttributes.attrSetting.attrValues[count]),
                                      NULL);
                if (result != OI_OK) {
                    return result;
                }
            }
            break;
            
        case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            ByteStream_PutUINT8(bs, response->u.playerAttributes.numAttrs);

            /* Check for message fragmentation */
            dataElSize = 2; /* 2 bytes per attr ID & value*/
            availableBytes = totalSize - ByteStream_GetPos(bs);
            status = CheckPacketOverflow(availableBytes, response->u.playerAttributes.numAttrs, dataElSize, &count);

            ByteStream_PutBytes(bs, response->u.playerAttributes.attrSetting.attrs, count*dataElSize);
            
            /* If the response message has to be fragmented, do some housekeeping */
            if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                result = InitFragment(addr,
                                      count,
                                      0xffff,
                                      response->u.playerAttributes.numAttrs,
                                      response->pduID,
                                       &response->u.playerAttributes.attrSetting.attrs[count*dataElSize], 
                                      NULL);
                if (result != OI_OK) {
                    return result;
                }
            }
            break;
            
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            ByteStream_PutUINT8(bs, response->u.playerAttributesText.numAttrs);

            for (i = 0; i < response->u.playerAttributesText.numAttrs; i++) {
                
                /* Check for message fragmentation */
                availableBytes = totalSize - ByteStream_GetPos(bs);
                
                /* If the response message has to be fragmented, do some housekeeping */
                if (availableBytes < 4) {
                    /* Not enough data to put in attribute header: ID, size, etc. */
                    status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    result = InitFragment(addr,
                                         (OI_UINT16)i,
                                          0xffff,
                                          response->u.playerAttributesText.numAttrs,
                                          response->pduID,
                                          &response->u.playerAttributesText.attrs[i],
                                          response->u.playerAttributesText.attrs[i].value.p);
                    if (result != OI_OK) {
                        return result;
                    }
                    break;
                }

                ByteStream_PutUINT8(bs, response->u.playerAttributesText.attrs[i].id);
                ByteStream_PutUINT16(bs, response->u.playerAttributesText.attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                ByteStream_PutUINT8(bs, (OI_UINT8)response->u.playerAttributesText.attrs[i].value.sz);

                /* Check for message fragmentation */
                availableBytes = totalSize - ByteStream_GetPos(bs);
                count = (OI_UINT8)response->u.playerAttributesText.attrs[i].value.sz;
                
                /* If the response message has to be fragmented, do some housekeeping */
                if (availableBytes < count) {
                    count = availableBytes;
                    status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    result = InitFragment(addr,
                                          i,
                                          count,
                                          response->u.playerAttributesText.numAttrs,
                                          response->pduID,
                                          &response->u.playerAttributesText.attrs[i],
                                          &response->u.playerAttributesText.attrs[i].value.p[count]);
                    if (result != OI_OK) {
                        return result;
                    }
                }
                ByteStream_PutBytes(bs, response->u.playerAttributesText.attrs[i].value.p, count);

                if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW){
                    break;
                }

            }
            break;

        case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:
        case OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET:
        case OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT:
        case OI_AVRCP_ABORT_CONTINUING_RESPONSE:            
            break;
            
        case OI_AVRCP_GET_PLAY_STATUS:
            ByteStream_PutUINT32(bs, response->u.playStatus.songLength, OI_AVRCP_BYTE_ORDER);
            ByteStream_PutUINT32(bs, response->u.playStatus.songPosition, OI_AVRCP_BYTE_ORDER);
            ByteStream_PutUINT8(bs, response->u.playStatus.status);
            break;
            
        case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
            ByteStream_PutUINT8(bs, response->u.elementAttributes.numAttr);
            for (i = 0; i < response->u.elementAttributes.numAttr; i++) {
                
                /* Check for message fragmentation */
                availableBytes = totalSize - ByteStream_GetPos(bs);

                /* If the response message has to be fragmented, do some housekeeping */
                if (availableBytes < 8) {
                    /* Not enough data to put in element attribute header: ID, size, etc. */
                    status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    result = InitFragment(addr,
                                          i,
                                          0xffff,
                                          response->u.elementAttributes.numAttr,
                                          response->pduID,
                                          &response->u.elementAttributes.attrs[i],
                                          response->u.elementAttributes.attrs[i].value.p);
                    if (result != OI_OK) {
                        return result;
                    }
                    
                    break;
                }
                ByteStream_PutUINT32(bs, response->u.elementAttributes.attrs[i].attrID, OI_AVRCP_BYTE_ORDER);
                ByteStream_PutUINT16(bs, response->u.elementAttributes.attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                ByteStream_PutUINT16(bs, (OI_UINT16)response->u.elementAttributes.attrs[i].value.sz,  OI_AVRCP_BYTE_ORDER);
                
                /* Check for message fragmentation */
                availableBytes = totalSize -  ByteStream_GetPos(bs);
                count = (OI_UINT16)response->u.elementAttributes.attrs[i].value.sz;

                /* If the response message has to be fragmented, do some housekeeping */
                if (availableBytes < count) {
                    count = availableBytes;
                    status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    result = InitFragment(addr,
                                          i,
                                          count,
                                          response->u.elementAttributes.numAttr,
                                          response->pduID,
                                          &response->u.elementAttributes.attrs[i],
                                          &response->u.elementAttributes.attrs[i].value.p[count]);
                    if (result != OI_OK) {
                        return result;
                    }
                }                
                ByteStream_PutBytes(bs, response->u.elementAttributes.attrs[i].value.p, count);
                
                if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW){
                    break;
                }

            }

            break;
            
        case OI_AVRCP_REGISTER_NOTIFICATION:
            
            ByteStream_PutUINT8(bs, response->u.event->id);
            switch (response->u.event->id) {
                case OI_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
                    ByteStream_PutUINT8(bs, response->u.event->u.playStatus);
                    break;
                case OI_AVRCP_EVENT_BATT_STATUS_CHANGED:
                case OI_AVRCP_EVENT_SYSTEM_STATUS_CHANGED:
                    ByteStream_PutUINT8(bs, response->u.event->u.systemStatus);
                    break;
                case OI_AVRCP_EVENT_TRACK_CHANGED:
                    ByteStream_PutBytes(bs, response->u.event->u.trackIndex, 8);
                    break;
                case OI_AVRCP_EVENT_PLAYBACK_POS_CHANGED:
                    ByteStream_PutUINT32(bs, response->u.event->u.playbackPosition, OI_AVRCP_BYTE_ORDER);
                    break;
                case OI_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
                    ByteStream_PutUINT8(bs, response->u.event->u.deviceSetting.numAttr);
                    
                    /* Check for message fragmentation */
                    dataElSize = 2; /* 2 bytes per attr ID & value*/
                    availableBytes = totalSize - ByteStream_GetPos(bs);
                    status = CheckPacketOverflow(availableBytes, response->u.event->u.deviceSetting.numAttr, dataElSize, &count);

                    ByteStream_PutBytes(bs, response->u.event->u.deviceSetting.attributes, count * dataElSize);
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                        result = InitFragment(addr,
                                              count,
                                              0xffff,
                                              response->u.event->u.deviceSetting.numAttr,
                                              response->pduID,
                                              &response->u.event->u.deviceSetting.attributes[count*dataElSize],
                                              NULL);
                        if (result != OI_OK) {
                            return result;
                        }
                    }
                    break;    

                case OI_AVRCP_EVENT_TRACK_REACHED_END:
                case OI_AVRCP_EVENT_TRACK_REACHED_START:
                    break;

                default:
                     return OI_STATUS_INVALID_PARAMETERS;
            }

            break;
            
        default:
            (*operandCount) = 0;
            return OI_AVRCP_NOT_IMPLEMENTED;
    }

    paramLen = ByteStream_GetPos(bs) - pos;
    ByteStream_SetPos(bs, pos - 3);

    if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {        
        ByteStream_PutUINT8(bs, OI_AVRCP_PDU_FIRST);
    } else {
        ByteStream_PutUINT8(bs, OI_AVRCP_PDU_COMPLETE);
    }
    
    ByteStream_PutUINT16(bs, paramLen, OI_AVRCP_BYTE_ORDER);

    ByteStream_Skip_Checked(bs, paramLen);
    ByteStream_Close(bs);

    (*operandCount) = ByteStream_GetSize(bs);

    return status;
}


static OI_STATUS SendResponse(OI_BD_ADDR *addr,
                              OI_UINT8 transaction,
                              OI_UINT8 result,
                              OI_AVRCP_RESPONSE *response)
{
    OI_AVRCP_FRAME frame;
    OI_BYTE operands[OI_AVRCP_MAX_FRAME_LENGTH];
    OI_STATUS status;
    OI_STATUS mStatus;
    
    frame.cmd = result;
    frame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    frame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
    frame.opcode = OI_AVRCP_OPCODE_VENDOR_DEPENDENT;
    frame.operand = operands;

    frame.operandCount = OI_AVRCP_This->target->tgResponseFrameSize; //OI_ARRAYSIZE(operands);
    mStatus = MarshalResponse(addr, response, frame.operand, &frame.operandCount);
    if (OI_SUCCESS(mStatus) || (mStatus == OI_AVRCP_RESPONSE_PACKET_OVERFLOW)) {
        status = OI_AVRCPTarget_SendGenericReply(addr, transaction, &frame);        
    } else {
        OI_DBGPRINT(("MarshalResponse failed %!", mStatus));
        return mStatus;
    }

    if (OI_SUCCESS(status) && (mStatus == OI_AVRCP_RESPONSE_PACKET_OVERFLOW)) {
        /* If a fragmented message was sent, return OI_AVRCP_RESPONSE_PACKET_OVERFLOW to the app */
        return OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
    } else {
        return status;
    }
}
OI_STATUS OI_AVRCPTarget_SendSimpleMetaReply(OI_BD_ADDR *addr, 
                                             OI_UINT8 transaction, 
                                             OI_UINT8 pdu,
                                             OI_STATUS status)
{
    OI_AVRCP_FRAME frame;
    OI_BYTE operands[ERROR_RESPONSE_OPERAND_COUNT];
    OI_BYTE_STREAM bs;
    OI_UINT8 errorCode;
#if 0
    OI_TRACE_USER(("OI_AVRCPTarget_SendSimpleMetaReply( addr%:, trans %d, PDU 0x%x, status %!)", addr, transaction, pdu, status));
#endif
    ARGCHECK(addr != NULL);

    if (status == OI_OK) {
        frame.cmd = OI_AVRCP_RESPONSE_ACCEPTED;
    } else {
        frame.cmd = (status == OI_AVRCP_NOT_IMPLEMENTED) ? OI_AVRCP_RESPONSE_NOT_IMPLEMENTED : OI_AVRCP_RESPONSE_REJECTED;
    }
    
    frame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    frame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
    frame.opcode = OI_AVRCP_OPCODE_VENDOR_DEPENDENT;
    frame.operand = operands;

    ByteStream_Init(bs, operands, ERROR_RESPONSE_OPERAND_COUNT);
    ByteStream_Open(bs, BYTESTREAM_WRITE);
    ByteStream_PutUINT24(bs, OI_AVRCP_METADATA_ID, OI_AVRCP_BYTE_ORDER);
    ByteStream_PutUINT8(bs, pdu);
    ByteStream_PutUINT8(bs, OI_AVRCP_PDU_COMPLETE);
    
    /* For REJECTED response, add spec-defined error reason */
    if ((status != OI_OK) && (status != OI_AVRCP_NOT_IMPLEMENTED)) {
        errorCode = MapStatus2Error(status);
        ByteStream_PutUINT16(bs, (OI_UINT16)0x0001, OI_AVRCP_BYTE_ORDER); /* parameter length */
        ByteStream_PutUINT8(bs, errorCode);        
    } else {        
        ByteStream_PutUINT16(bs, (OI_UINT16)0x0000, OI_AVRCP_BYTE_ORDER); /* parameter length */
    } 

    ByteStream_Close(bs);
 
    frame.operandCount = ByteStream_GetSize(bs);

    status = OI_AVRCPTarget_SendGenericReply(addr, transaction, &frame);

    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Failed to send Simple Meta Response %!", status));
    }
    
    return status;
}

static OI_STATUS UnmarshalCommand(OI_BYTE *operand,
                                  OI_UINT16 operandCount,
                                  OI_AVRCP_COMMAND *command,
                                  OI_UINT32 *dataBuffer)
{
    OI_BYTE_STREAM bs;
    OI_UINT32      vendorId = 0;
    OI_UINT16      i;
    
    OI_DBGPRINT(("UnmarshalCommand(operand=0x%x,operandCount=%d,command=0x%x)", operand, operandCount, command));

    OI_ASSERT(operand != NULL);
    OI_ASSERT(command != NULL);
    
    if ((operandCount == 0) || (operand == NULL)) {
        return OI_AVRCP_RESPONSE_INTERNAL_ERROR;
    }
    
    ByteStream_Init(bs, operand, operandCount);
    ByteStream_Open(bs, BYTESTREAM_READ);
    ByteStream_GetUINT24_Checked(bs, vendorId, OI_AVRCP_BYTE_ORDER);
    ByteStream_GetTyped8_Checked(bs, command->id, OI_UINT8);
    ByteStream_Skip_Checked(bs, 3); /* Ignore the packet type and parameter length */

    if (ByteStream_Error(bs) || (vendorId != OI_AVRCP_METADATA_ID)) {
        return OI_AVRCP_RESPONSE_INTERNAL_ERROR;
    }

    switch (command->id) {
        case OI_AVRCP_GET_CAPABILITIES:
            OI_DBGPRINT2(("Received GET_CAPABILITIES command"));
            ByteStream_GetTyped8_Checked(bs, command->u.capabilityID, OI_UINT8);
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            OI_DBGPRINT2(("Received LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES command"));
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            OI_DBGPRINT2(("Received LIST_PLAYER_APPLICATION_SETTING_VALUES command"));
            ByteStream_GetTyped8_Checked(bs, command->u.attribute.attrSetting.attrID, OI_UINT8);
            break;
            
        case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
            ByteStream_GetTyped8_Checked(bs, command->u.attribute.numAttrs, OI_UINT8);
            command->u.attribute.attrSetting.ids = (OI_UINT8*)(operand + ByteStream_GetPos(bs));
            break;
            
        case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:
            OI_DBGPRINT2(("Received SET_PLAYER_APPLICATION_SETTING_VALUE command"));
            ByteStream_GetTyped8_Checked(bs, command->u.attribute.numAttrs, OI_UINT8);
            command->u.attribute.attrSetting.attrs = (OI_UINT8*)(operand + ByteStream_GetPos(bs));
            break;
            
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            ByteStream_GetTyped8_Checked(bs, command->u.attrValueText.attrID, OI_UINT8);
            ByteStream_GetTyped8_Checked(bs, command->u.attrValueText.numValues, OI_UINT8);
            command->u.attrValueText.values = (OI_UINT8*)(operand + ByteStream_GetPos(bs));
            break;
 
        case OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET:
            OI_DBGPRINT2(("Received INFORM DISPLAYABLE CHARACTER SET command"));
            command->u.charSetCT.charSetIDs = (OI_UINT16 *) dataBuffer;
            ByteStream_GetTyped8_Checked(bs, command->u.charSetCT.numCharSets, OI_UINT8);
            for (i = 0; i < command->u.charSetCT.numCharSets; ++i) {
                ByteStream_GetUINT16_Checked(bs, command->u.charSetCT.charSetIDs[i], OI_AVRCP_BYTE_ORDER);
            }
            break;
            
        case OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT:
            ByteStream_GetTyped8_Checked(bs, command->u.batteryStatus, OI_UINT8);
            break;
            
        case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
            OI_DBGPRINT2(("Received GET ELEMENT ATTRIBUTES command"));
            command->u.getElementAttributes.attrs = dataBuffer;
            command->u.getElementAttributes.id = (OI_UINT8*)(operand + ByteStream_GetPos(bs));
            ByteStream_Skip_Checked(bs, 8); /* skip media attribute ID */
            ByteStream_GetTyped8_Checked(bs, command->u.getElementAttributes.numAttr, OI_UINT8);
            for (i = 0; i < command->u.getElementAttributes.numAttr; ++i) {
                ByteStream_GetUINT32_Checked(bs, command->u.getElementAttributes.attrs[i], OI_AVRCP_BYTE_ORDER);
            }
            break;
        case OI_AVRCP_GET_PLAY_STATUS:
            OI_DBGPRINT2(("Received GET PLAY STATUS command"));
            break;
        case OI_AVRCP_REGISTER_NOTIFICATION:
            ByteStream_GetTyped8_Checked(bs, command->u.notification.eventID, OI_UINT8);
            OI_DBGPRINT2(("Received REGISTER NOTIFICATION command for event 0x%2x", command->u.notification.eventID));
            ByteStream_GetUINT32_Checked(bs, command->u.notification.playbackInterval, OI_AVRCP_BYTE_ORDER);
            break;

        case OI_AVRCP_REQUEST_CONTINUING_RESPONSE:
        case OI_AVRCP_ABORT_CONTINUING_RESPONSE:
            ByteStream_GetTyped8_Checked(bs, command->u.responsePDU, OI_UINT8);
            break;
            
        default:
            return OI_AVRCP_RESPONSE_INVALID_PDU;
    }

    ByteStream_Close(bs);
    
    if (ByteStream_Error(bs)) {
        return OI_AVRCP_RESPONSE_INVALID_PARAMETER;
    } else {
        return OI_OK;
    }

}

static OI_STATUS MarshalFragmentResponse (OI_BD_ADDR *addr,
                                          OI_BYTE *operands,
                                          OI_UINT16 *operandCount,
                                          OI_UINT8 pduID,
                                          AVRCP_FRAGMENT *fragment)
{
    OI_STATUS status = OI_OK;
    OI_BYTE_STREAM bs;
    OI_UINT16 totalSize = *operandCount;
    OI_UINT16 pos;
    OI_UINT16 paramLen;
    OI_UINT16 i;
    OI_UINT8  dataElSize;
    OI_UINT16 availableBytes;
    OI_UINT16 numElementsLeft; /* Number of attrs/elements/capabilities that need to be sent */
    OI_UINT16 count;           /* Total count of attrs/elements/capabilities */
    OI_UINT16 currAttrIndex;
    OI_UINT16 currAttrDataPos;
    
    OI_DBGPRINT2(("MarshalFragmentResponse(operands=0x%x, operandCount=0x%x, PDU 0x%x, fragment 0x%x)", 
                  operands, operandCount, pduID, fragment));


    ByteStream_Init(bs, operands, (*operandCount));
    ByteStream_Open(bs, BYTESTREAM_WRITE);
    ByteStream_PutUINT24(bs, OI_AVRCP_METADATA_ID, OI_AVRCP_BYTE_ORDER);
    ByteStream_PutUINT8(bs, pduID);
    ByteStream_Skip_Checked(bs, 3); /* Fill in packet type and parameter length later */
    pos = ByteStream_GetPos(bs);

    availableBytes = totalSize - pos;
    numElementsLeft = fragment->count - fragment->currAttrIndex;
    
    switch (pduID) {
        case OI_AVRCP_GET_CAPABILITIES:
            switch (fragment->u.capability) {
                case OI_AVRCP_CAPABILITY_COMPANY_ID:
                    {
                        OI_UINT32 *companyIDs = (OI_UINT32 *) fragment->attrFragment.attr;
                        
                        dataElSize = 3;
                        /* Check for message fragmentation */
                        status = CheckPacketOverflow(availableBytes, numElementsLeft, dataElSize, &count);            
                        for (i = 0; i < count; ++i) {
                            ByteStream_PutUINT24(bs, companyIDs[i], OI_AVRCP_BYTE_ORDER);
                        }
                        dataElSize = 4; /* for fragmentation, the company ID offset will be counted in UINT32 */
                        break;
                    }
                case OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED:
                    {
                        OI_UINT8 * eventIDs = (OI_UINT8 *) fragment->attrFragment.attr;
                        
                        dataElSize = 1;
                        /* Check for message fragmentation */
                        status = CheckPacketOverflow(availableBytes, numElementsLeft, dataElSize, &count);
                        ByteStream_PutBytes(bs, eventIDs, count);
                        break;
                    }
                default:
                    return OI_STATUS_INTERNAL_ERROR;
            }

            /* If the response message still has to be fragmented, do some housekeeping */
            if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {                    
                
                currAttrIndex = fragment->currAttrIndex + count;
                UpdateFragment(fragment,
                               currAttrIndex,
                               fragment->u.capability,
                               (OI_UINT8 *)((OI_UINT)fragment->attrFragment.attr + count*dataElSize),
                               (OI_UINT8 *) NULL);
            }

            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
            {
                OI_UINT8 *attrIDs = (OI_UINT8 *) fragment->attrFragment.attr;
                dataElSize  = 1;
                
                /* Check for message fragmentation */
                status = CheckPacketOverflow(availableBytes, numElementsLeft, dataElSize, &count);
                
                ByteStream_PutBytes(bs, attrIDs, count);
                
                /* If the response message has to be fragmented, do some housekeeping */
                if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                    currAttrIndex = fragment->currAttrIndex + count;
                    UpdateFragment(fragment,
                                   currAttrIndex,
                                   0xffff,
                                   &(attrIDs[count]),
                                   (OI_UINT8 *) NULL);
                    
                }
            }
            break;
            
        case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:
            {  
                OI_UINT8 *attrValues = (OI_UINT8 *) fragment->attrFragment.attr;
                dataElSize = 1;
                /* Check for message fragmentation */
                status = CheckPacketOverflow(availableBytes, numElementsLeft, dataElSize, &count);
                
                ByteStream_PutBytes(bs, attrValues, count);
                
                /* If the response message has to be fragmented, do some housekeeping */
                if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                    currAttrIndex = fragment->currAttrIndex + count;
                    UpdateFragment(fragment,
                                   currAttrIndex,
                                   0xffff,
                                   &(attrValues[count]),
                                   NULL);
                }
                break;
            }
         case OI_AVRCP_REGISTER_NOTIFICATION:
            /* Since the only event notification response that ca be fragmented is EVENT_PLAYER_APPLICATION_SETTING_CHANGED,
             * there is no need to check for Event ID and we can merge this case with GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE */
         case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
            {
                OI_AVRCP_ATTRIBUTE_PAIR *attrs = (OI_AVRCP_ATTRIBUTE_PAIR *) fragment->attrFragment.attr;
                
                /* Check for message fragmentation */
                dataElSize = 2; /* 2 bytes per attr ID & value*/
                status = CheckPacketOverflow(availableBytes, numElementsLeft, dataElSize, &count);
                
                ByteStream_PutBytes(bs, attrs, count*dataElSize);
                
                /* If the response message has to be fragmented, do some housekeeping */
                if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                    currAttrIndex = fragment->currAttrIndex + count;
                    UpdateFragment(fragment,
                                   currAttrIndex,
                                   0xffff,
                                   &(attrs[count * dataElSize]),
                                   NULL);
                }
                break;
            }
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
        case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
            {
                OI_AVRCP_ATTRIBUTE_TEXT *attrs = (OI_AVRCP_ATTRIBUTE_TEXT *) fragment->attrFragment.attr;
                
                /* If we have unfinished string to send, send it first */
                if (fragment->u.currAttrDataPos != 0xffff) {
                    /* Check for message fragmentation */
                    availableBytes = totalSize - pos;
                    count = attrs[0].value.sz - fragment->u.currAttrDataPos;
                    if(availableBytes < count) {
                        count = availableBytes;
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    }
                    ByteStream_PutBytes(bs, fragment->attrFragment.data , count);
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if(status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrDataPos = fragment->u.currAttrDataPos + count;
                        UpdateFragment(fragment,
                                       fragment->currAttrIndex,
                                       currAttrDataPos,
                                       &attrs[0],
                                       (OI_UINT8 *)((OI_UINT)fragment->attrFragment.data + count));
                        
                        break;
                    } else {
                        fragment->currAttrIndex = fragment->currAttrIndex + 1;
                        attrs++;
                    }
                }
                
                OI_ASSERT(fragment->count > fragment->currAttrIndex);
                
                for (i = 0; i < (OI_UINT)(fragment->count - fragment->currAttrIndex); i++) {
                    
                    /* Check for message fragmentation */
                    availableBytes = totalSize - ByteStream_GetPos(bs);
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if (availableBytes < 4) {
                        /* Not enough data to put in attribute header: ID, size, etc. */
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrIndex = fragment->currAttrIndex + i;
                        UpdateFragment(fragment,
                                       currAttrIndex,
                                       0xffff,
                                       &attrs[i],
                                       attrs[i].value.p);
                        break;
                    }
                    
                    ByteStream_PutUINT8(bs, attrs[i].id);
                    ByteStream_PutUINT16(bs, attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                    ByteStream_PutUINT8(bs, (OI_UINT8)attrs[i].value.sz);
                    
                    /* Check for message fragmentation */
                    availableBytes = totalSize - ByteStream_GetPos(bs);
                    count = (OI_UINT8)attrs[i].value.sz;
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if (availableBytes < count) {
                        count = availableBytes;
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrIndex = fragment->currAttrIndex + i;
                        UpdateFragment(fragment,
                                       currAttrIndex,
                                       count,
                                       &attrs[i],
                                       &attrs[i].value.p[count]);
                    }
                
                    ByteStream_PutBytes(bs, attrs[i].value.p, count);
                    
                    if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW){
                        break;
                    }
                    
                }
                break;
            }
        case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
            {
                OI_AVRCP_ELEMENT_ATTRIBUTE *attrs = (OI_AVRCP_ELEMENT_ATTRIBUTE *) fragment->attrFragment.attr;

                /* If we have unfinished string to send, send it first */
                if (fragment->u.currAttrDataPos != 0xffff) {
                    /* Check for message fragmentation */
                    availableBytes = totalSize - pos;
                    count = attrs[0].value.sz - fragment->u.currAttrDataPos;
                    if(availableBytes < count) {
                        count = availableBytes;
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                    }
                    ByteStream_PutBytes(bs, fragment->attrFragment.data, count);
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if(status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrDataPos = fragment->u.currAttrDataPos + count;
                        UpdateFragment(fragment,
                                       fragment->currAttrIndex,
                                       currAttrDataPos,
                                       &attrs[0],
                                       (OI_UINT8 *)((OI_UINT)fragment->attrFragment.data + count));
                        
                        break;
                    } else {
                        fragment->currAttrIndex = fragment->currAttrIndex + 1;
                        attrs++;
                    }
                    
                }
                
                OI_ASSERT(fragment->count > fragment->currAttrIndex);
                
                for (i = 0; i < (OI_UINT)(fragment->count - fragment->currAttrIndex); i++) {
                    
                    /* Check for message fragmentation */
                    availableBytes = totalSize - ByteStream_GetPos(bs);

                    /* If the response message has to be fragmented, do some housekeeping */
                    if (availableBytes < 8) {
                        /* Not enough data to put in element attribute header: ID, size, etc. */
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrIndex = fragment->currAttrIndex + i;
                        UpdateFragment(fragment,
                                       currAttrIndex,
                                       0xffff,
                                       &attrs[i],
                                       attrs[i].value.p);
                        break;
                    }
                    ByteStream_PutUINT32(bs, attrs[i].attrID, OI_AVRCP_BYTE_ORDER);
                    ByteStream_PutUINT16(bs, attrs[i].charSet, OI_AVRCP_BYTE_ORDER);
                    ByteStream_PutUINT16(bs, (OI_UINT16) attrs[i].value.sz,  OI_AVRCP_BYTE_ORDER);
                    
                    /* Check for message fragmentation */
                    availableBytes = totalSize -  ByteStream_GetPos(bs);
                    count = (OI_UINT16)attrs[i].value.sz;
                    
                    /* If the response message has to be fragmented, do some housekeeping */
                    if (availableBytes < count) {
                        count = availableBytes;
                        status = OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
                        currAttrIndex = fragment->currAttrIndex + i;
                        UpdateFragment(fragment,
                                       currAttrIndex,
                                       count,
                                       &attrs[i],
                                       &attrs[i].value.p[count]);
                    }
                    
                    ByteStream_PutBytes(bs, attrs[i].value.p, count);
                    
                    if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW){
                        break;
                    }
                    
                }
                
                break;
            }
            /*
              Assumption: The responses corresponding to the following PDUs cannot be fragmented
              case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:
              case OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET:
              case OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT:
              case OI_AVRCP_GET_PLAY_STATUS:
              case OI_AVRCP_REQUEST_CONTINUING_RESPONSE:
              case OI_AVRCP_ABORT_CONTINUING_RESPONSE:
            */
        default:
            (*operandCount) = 0;
            return OI_AVRCP_NOT_IMPLEMENTED;
    }

    paramLen = ByteStream_GetPos(bs) - pos;
    ByteStream_SetPos(bs, pos - 3);

    if (status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {        
        ByteStream_PutUINT8(bs, OI_AVRCP_PDU_CONTINUE);
    } else {
        ByteStream_PutUINT8(bs, OI_AVRCP_PDU_END);
    }
    
    ByteStream_PutUINT16(bs, paramLen, OI_AVRCP_BYTE_ORDER);

    ByteStream_Skip_Checked(bs, paramLen);
    ByteStream_Close(bs);

    (*operandCount) = ByteStream_GetSize(bs);

    return status;

}


static OI_STATUS HandleVendorPassThroughCommand(OI_BD_ADDR *addr,
                                                OI_UINT8 transaction,
                                                OI_AVRCP_FRAME *frame)
{

    OI_UINT16 vendorUniqueID;
    OI_BOOL buttonPress;
    OI_STATUS status = OI_OK;
        
    if ((OI_AVRCP_OP_CODE(frame->operand[0]) != OI_AVRCP_OP_VENDOR_UNIQUE) || (frame->operandCount < 7)) {
#ifndef OI_CODE 
        status = ( OI_STATUS )OI_AVRCP_RESPONSE_REJECTED;
        return status;
#else
#error code not present
#endif /* OI_CODE */
    }

    buttonPress = OI_AVRCP_OP_IS_PRESS(frame->operand[0]);
        
    /* Ignore data field length and compnay ID */
    vendorUniqueID = GetUINT16_BigEndian(&frame->operand[5]);

    if (OI_AVRCP_This->targetCb->metadataCb->groupNavigationCommandInd) {
        OI_AVRCP_This->targetCb->metadataCb->groupNavigationCommandInd(addr, transaction, vendorUniqueID, buttonPress);
    } else {
#ifndef OI_CODE
        status = ( OI_STATUS )OI_AVRCP_RESPONSE_NOT_IMPLEMENTED;
#else
#error code not present
#endif  /* OI_CODE */
    }
    
    return status;
}

 /****************************************************************************
 *
* Enhanced AVRCP target interface
 *
 ****************************************************************************/

OI_STATUS OI_AVRCPTarget_SendCompanyIDReply(OI_BD_ADDR *addr, 
                                            OI_UINT8 transaction, 
                                            OI_UINT8 count,
                                            OI_UINT32 *companyIDs)
{
    OI_AVRCP_RESPONSE rsp;

#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_SendCompanyIDReply(transaction %d, count=%d,companyIDs=0x%x)", transaction, count, companyIDs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(companyIDs != NULL);
    ARGCHECK(count != 0);
    
    if ((!count) || (NULL == companyIDs)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    rsp.pduID = OI_AVRCP_GET_CAPABILITIES;
    rsp.u.getCapabilities.capabilityCount = count;
    rsp.u.getCapabilities.capabilities.companyID = companyIDs;
    rsp.u.getCapabilities.capabilityID = OI_AVRCP_CAPABILITY_COMPANY_ID;

    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
        
}


OI_STATUS OI_AVRCPTarget_SendEventIDReply(OI_BD_ADDR *addr, 
                                          OI_UINT8 transaction, 
                                          OI_UINT8 count,
                                          OI_UINT8 *eventIDs)
{

    OI_AVRCP_RESPONSE rsp;
#if 0
    OI_TRACE_USER(("OI_AVRCPTarget_SendEventIDReply(transaction %d, count=%d, eventIDs=0x%x)", transaction, count, eventIDs));
#endif    
    ARGCHECK(addr != NULL);
    ARGCHECK(eventIDs != NULL);
    ARGCHECK(count != 0);

    if ((count < OI_AVRCP_MINIMUM_SUPPORTED_EVENTS) || (NULL == eventIDs)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    rsp.pduID = OI_AVRCP_GET_CAPABILITIES;
    rsp.u.getCapabilities.capabilityCount = count;
    rsp.u.getCapabilities.capabilities.eventID = eventIDs;
    rsp.u.getCapabilities.capabilityID = OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED;

    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
            
}

    

OI_STATUS OI_AVRCPTarget_ListAttrIDReply(OI_BD_ADDR *addr, 
                                          OI_UINT8 transaction, 
                                          OI_UINT8 count,
                                          OI_UINT8 *attrIDs)
{

    OI_AVRCP_RESPONSE rsp;
#if 0
    OI_TRACE_USER(("OI_AVRCPTarget_ListAttrIDReply(transaction %d, count=%d, attrIDs=0x%x)", transaction, count, attrIDs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(attrIDs != NULL);
    ARGCHECK(count != 0);
        
    if ((count == 0) || (attrIDs == 0)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    rsp.pduID = OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
    rsp.u.playerAttributes.numAttrs = count;
    rsp.u.playerAttributes.attrSetting.ids = attrIDs;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
            
}

OI_STATUS OI_AVRCPTarget_ListAttrValuesReply(OI_BD_ADDR *addr, 
                                             OI_UINT8 transaction,
                                             OI_UINT8 attrID,
                                             OI_UINT8 count,
                                             OI_UINT8 *attrSettingVals)
{
    OI_AVRCP_RESPONSE rsp;
    
#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_ListAttrValuesReply(transaction %d, attrID=0x%2x, count=%d, values=0x%x)",
                   transaction, attrID, count, attrSettingVals));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(attrSettingVals != NULL);
    ARGCHECK(count != 0);
    
    if ((count == 0) || (!attrSettingVals)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    rsp.pduID = OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES;
    rsp.u.playerAttributes.numAttrs = count;
    rsp.u.playerAttributes.attrSetting.attrValues = attrSettingVals;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
            
}

OI_STATUS OI_AVRCPTarget_GetAttrValueReply(OI_BD_ADDR *addr, 
                                           OI_UINT8 transaction,
                                           OI_UINT8 count,
                                           OI_AVRCP_ATTRIBUTE_PAIR attrs)
{
    OI_AVRCP_RESPONSE rsp;
#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_GetAttrValueReply(transaction %d, attrID=0x%x, count=%d)", transaction, count, attrs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(attrs != NULL);
    ARGCHECK(count != 0);
 
    rsp.pduID = OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
    rsp.u.playerAttributes.numAttrs = count;
    rsp.u.playerAttributes.attrSetting.attrs = attrs;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
     
}

OI_STATUS OI_AVRCPTarget_GetAttrSettingTextReply(OI_BD_ADDR *addr, 
                                                 OI_UINT8 transaction,
                                                 OI_UINT8 count,
                                                 OI_AVRCP_ATTRIBUTE_TEXT *attrs)
{
    OI_AVRCP_RESPONSE rsp;
#if 0        
    OI_TRACE_USER(("OI_AVRCPTarget_GetAttrSettingTextReply(transaction %d, count=%d, attrID=0x%x)",
                   transaction, count, attrs));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(attrs != NULL);

    rsp.pduID = OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    rsp.u.playerAttributesText.numAttrs = count;
    rsp.u.playerAttributesText.attrs = attrs;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
     
}

OI_STATUS OI_AVRCPTarget_GetAttrSettingValueTextReply(OI_BD_ADDR *addr, 
                                                      OI_UINT8 transaction,
                                                      OI_UINT8 count,
                                                      OI_AVRCP_ATTRIBUTE_TEXT *settingValues)
{
    OI_AVRCP_RESPONSE rsp;
#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_GetAttrSettingValueTextReply(transaction %d, count=%d, settingValues=0x%x)",
                   transaction, count, settingValues));
#endif    
    ARGCHECK(addr != NULL);
    ARGCHECK(settingValues != NULL);    
    ARGCHECK(count != 0);
             
    rsp.pduID = OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    rsp.u.playerAttributesText.numAttrs = count;
    rsp.u.playerAttributesText.attrs = settingValues;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
     
    
}

OI_STATUS OI_AVRCPTarget_PlayStatusReply(OI_BD_ADDR *addr, 
                                           OI_UINT8 transaction,
                                           OI_AVRCP_PLAY_STATUS *playStatus)
{
    OI_AVRCP_RESPONSE rsp;
#if 0       
    OI_TRACE_USER(("OI_AVRCPTarget_PlayStatusReply(transaction %d, playStatus 0x%x)", transaction, playStatus));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(playStatus != NULL);
 
    rsp.pduID = OI_AVRCP_GET_PLAY_STATUS;
    rsp.u.playStatus.songLength = playStatus->songLength;
    rsp.u.playStatus.songPosition = playStatus->songPosition;
    rsp.u.playStatus.status = playStatus->status;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);
}
 
OI_STATUS OI_AVRCPTarget_GetElementAttributesReply(OI_BD_ADDR *addr, 
                                                   OI_UINT8 transaction,
                                                   OI_UINT8 count,
                                                   OI_AVRCP_ELEMENT_ATTRIBUTE *element)
{
    
    OI_AVRCP_RESPONSE rsp;
    
    ARGCHECK(addr != NULL);
    ARGCHECK(count != 0);
    ARGCHECK(element != NULL);
#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_GetElementAttributesReply(transaction %d, numAttributes %d, element 0x%x)", transaction, count, element));
#endif
    rsp.pduID = OI_AVRCP_GET_ELEMENT_ATTRIBUTES;
    rsp.u.elementAttributes.numAttr = count;
    rsp.u.elementAttributes.attrs = element;
    
    return SendResponse(addr, transaction, OI_AVRCP_RESPONSE_STABLE, &rsp);

}
    
OI_STATUS OI_AVRCPTarget_RegisterNotificationReply(OI_BD_ADDR *addr, 
                                                   OI_UINT8 transaction,
                                                   OI_AVRCP_EVENT *event,
                                                   OI_BOOL isFinal)
{
    OI_AVRCP_RESPONSE rsp;
    OI_UINT8 result = isFinal ? OI_AVRCP_RESPONSE_CHANGED : OI_AVRCP_RESPONSE_INTERIM;
#if 0    
    OI_TRACE_USER(("OI_AVRCPTarget_RegisterNotificationReply(transaction %d, event 0x%x, %s)",
                   transaction, event, isFinal ? "CHANGED" : "INTERIM"));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(event != NULL);

    rsp.pduID = OI_AVRCP_REGISTER_NOTIFICATION;
    rsp.u.event = event;

    return SendResponse(addr, transaction, result, &rsp);
    
}

OI_STATUS OI_AVRCPTarget_FragmentReply (OI_BD_ADDR *addr, 
                                        OI_UINT8 transaction,
                                        OI_UINT8 pduID,
                                        OI_AVRCP_FRAGMENT_DATA *fragmentMsg)
{
    OI_AVRCP_FRAME frame;
    OI_BYTE operands[OI_AVRCP_MAX_FRAME_LENGTH];
    OI_STATUS status;
    OI_STATUS mStatus;
    AVRCP_TRANS_TABLE *table = LookupTransactionTable(addr, FALSE);
#if 0
    OI_TRACE_USER(("OI_AVRCPTarget_FragmentReply(transaction %d, pdu 0x%2x, fragmentMsg 0x%x)",
                   transaction, pduID, fragmentMsg));
#endif
    ARGCHECK(addr != NULL);
    ARGCHECK(fragmentMsg != NULL);    
    
    if (table == NULL) {
        OI_DBGPRINT (("Cannot find transaction table for %:!", addr));
        return OI_STATUS_INTERNAL_ERROR;
    }
    
    if (table->fragment == NULL) {
        OI_DBGPRINT (("Cannot find pending fragmented message info for %:!", addr));
        return OI_STATUS_INTERNAL_ERROR;
    }
    
    /* Check if the application supplied updated fragment data buffer pointers. If yes, use these pointers.
    * Otherwise, use internally kept pointers. */
    if (fragmentMsg->attr != NULL) {
        table->fragment->attrFragment.attr = fragmentMsg->attr;
    }
    if (fragmentMsg->data != NULL) {
        table->fragment->attrFragment.data = fragmentMsg->data;
    }
    

    /* Since the OI_AVRCP_REQUEST_CONTINUING_RESPONSE is a CONTORL type command, the response type should be ACCEPTED */       
    frame.cmd = OI_AVRCP_RESPONSE_ACCEPTED; 
    frame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    frame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
    frame.opcode = OI_AVRCP_OPCODE_VENDOR_DEPENDENT;
    frame.operand = operands;

    frame.operandCount = OI_AVRCP_This->target->tgResponseFrameSize; //OI_ARRAYSIZE(operands);

    /* Additional check for inconsistensy */
    if(table->fragment->count < table->fragment->currAttrIndex) {
        OI_LOG_ERROR(("AVRCP internal error: total attr count %d less than current attr index %d!",
                      table->fragment->count, table->fragment->currAttrIndex));
        return OI_STATUS_INTERNAL_ERROR;
    }
    
    mStatus = MarshalFragmentResponse(addr, frame.operand, &frame.operandCount, pduID, table->fragment);


    if (OI_SUCCESS(mStatus) || (mStatus == OI_AVRCP_RESPONSE_PACKET_OVERFLOW)) {
        status = OI_AVRCPTarget_SendGenericReply(addr, transaction, &frame);
    } else {
        OI_DBGPRINT(("MarshalFragmentResponse failed %!", mStatus));
        return mStatus;
    }
    
    if (OI_SUCCESS(status)) {
        if (mStatus == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) {
            /* If a fragmented message was sent, return OI_AVRCP_RESPONSE_PACKET_OVERFLOW to the app */
            return OI_AVRCP_RESPONSE_PACKET_OVERFLOW;
        } else if (mStatus == OI_OK) {
            /* If the complete message has been sent, free fragment buffer */
               OI_DBGPRINT2(("Free fragment buffer 0x%x @ 0x%x\n", table->fragment, &table->fragment));
               /* Free fragment buffer */
               OI_FreeIf(&table->fragment);
               table->fragment = NULL;
        }
    }
    
    return status;
}

OI_STATUS OI_AVRCPTarget_GroupNavigationReply(OI_BD_ADDR *addr, 
                                              OI_UINT8 transaction,
                                              OI_UINT16 vendorUniqueID,
                                              OI_BOOL buttonPress,
                                              OI_UINT8 responseCode)
{
    OI_BYTE operands[7];
    OI_AVRCP_FRAME frame;

    ARGCHECK(addr != NULL);

    frame.cmd = responseCode;
    frame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    frame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
#ifdef OI_CODE
#error code not present
#else
	frame.opcode = OI_AVRCP_OPCODE_PASS_THROUGH;
#endif 

    frame.operand = operands;
    frame.operandCount = OI_ARRAYSIZE(operands);

    SetGroupNavigationOperands( operands, vendorUniqueID, buttonPress);

    return OI_AVRCPTarget_SendGenericReply(addr, transaction, &frame);
}
    

OI_STATUS AVRCPMetadata_CmdInd(OI_BD_ADDR *addr,
                               OI_UINT8 transaction,
                               OI_AVRCP_FRAME *frame)
{
    OI_AVRCP_COMMAND command = { 0, 0, { 0 } };  /* Make GCC 4.1 happy */
    OI_STATUS status = OI_OK;
    OI_AVRCP_TARGET_METADATA_CALLBACKS *metadataCBs;
    struct {
        OI_UINT32 data[OI_AVRCP_MAX_FRAME_LENGTH >> 2];
    } dataBuffer;
    
    OI_ASSERT(addr != NULL);
    OI_ASSERT(frame != NULL);    
    OI_ASSERT(OI_AVRCP_This->targetCb->metadataCb);

    metadataCBs = OI_AVRCP_This->targetCb->metadataCb;

    if (frame->opcode == OI_AVRCP_OPCODE_PASS_THROUGH) {
        status = HandleVendorPassThroughCommand(addr,
                                                transaction,
                                                frame);
    } else {
        AVRCP_TRANS_TABLE *table = LookupTransactionTable(addr, FALSE);

        if (table == NULL) {
            OI_DBGPRINT (("Cannot find transaction table for %:!", addr));
            status = OI_AVRCP_RESPONSE_INTERNAL_ERROR;
        }
        
        if (OI_SUCCESS(status)) {
            status = UnmarshalCommand(frame->operand, frame->operandCount, &command, dataBuffer.data);

            /* In case of pending fragmented response, any command with the exception of OI_AVRCP_ABORT_CONTINUING_RESPONSE
             * effectively means that the response should be aborted. Therefore, we need to clean up fragment info structure
             */
            if (command.id != OI_AVRCP_REQUEST_CONTINUING_RESPONSE) {
                OI_DBGPRINT2(("Free fragment buffer 0x%x\n", table->fragment));
                if( table->fragment != NULL) {
                    OI_FreeIf(&table->fragment);
                    table->fragment = NULL;
                }
            }
        }
        
        if (OI_SUCCESS(status)) {
            switch (command.id) {
                case OI_AVRCP_GET_CAPABILITIES:
                    if (metadataCBs->capabilityCommandInd == NULL) {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    } else {
                        metadataCBs->capabilityCommandInd(addr, transaction, command.u.capabilityID);
                    }
                    break;
                case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
                    if (metadataCBs->listAttrCommandInd) {
                        metadataCBs->listAttrCommandInd(addr, transaction);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;   
                case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:
                    if (metadataCBs->listAttrValuesCommandInd) {
                        metadataCBs->listAttrValuesCommandInd(addr, transaction, command.u.attribute.attrSetting.attrID);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;   
                case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
                    if (metadataCBs->getAttrValuesCommandInd) {
                        metadataCBs->getAttrValuesCommandInd(addr, transaction, command.u.attribute.numAttrs, command.u.attribute.attrSetting.ids);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;   
                case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:
                    if (metadataCBs->setAttrValuesCommandInd) {
                        metadataCBs->setAttrValuesCommandInd(addr, transaction, command.u.attribute.numAttrs, command.u.attribute.attrSetting.attrs);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
                    if (metadataCBs->getAttrSettingTextCommandInd) {
                        metadataCBs->getAttrSettingTextCommandInd(addr, transaction, command.u.attribute.numAttrs, command.u.attribute.attrSetting.ids);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;   
                case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
                    if (metadataCBs->getAttrSettingValueTextCommandInd) {
                        metadataCBs->getAttrSettingValueTextCommandInd(addr, transaction,
                                                                       command.u.attrValueText.attrID,
                                                                       command.u.attrValueText.numValues,
                                                                       command.u.attrValueText.values);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
                    if (metadataCBs->getElementAttrCommandInd) {
                        metadataCBs->getElementAttrCommandInd(addr, transaction,
                                                              command.u.getElementAttributes.id,
                                                              command.u.getElementAttributes.numAttr,
                                                              command.u.getElementAttributes.attrs);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;            
                case OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET:
                    if (metadataCBs->informCharacterSetCommandInd) {
                        metadataCBs->informCharacterSetCommandInd(addr, transaction, command.u.charSetCT.numCharSets,
                                                                  command.u.charSetCT.charSetIDs);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                    
                case OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT:
                    if (metadataCBs->informBatteryStatusOfCTCommandInd) {
                        metadataCBs->informBatteryStatusOfCTCommandInd(addr, transaction, command.u.batteryStatus);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
            
                case OI_AVRCP_GET_PLAY_STATUS:
                    if (metadataCBs->getPlayStatusCommandInd) {
                        metadataCBs->getPlayStatusCommandInd(addr, transaction);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                    
                case OI_AVRCP_REGISTER_NOTIFICATION:
                    if (metadataCBs->eventNotificationCommandInd) {
                        metadataCBs->eventNotificationCommandInd(addr, transaction, command.u.notification.eventID,
                                                                 command.u.notification.playbackInterval);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                    
                case OI_AVRCP_REQUEST_CONTINUING_RESPONSE:
                    if (metadataCBs->continueResponseCommandInd) {
                        if (NULL != table->fragment) {
                            metadataCBs->continueResponseCommandInd(addr, transaction, command.u.responsePDU,
                                                                    table->fragment->currAttrIndex, table->fragment->u.currAttrDataPos);
                        } else {
                            status = OI_AVRCP_RESPONSE_INTERNAL_ERROR;
                        }
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                    
                case OI_AVRCP_ABORT_CONTINUING_RESPONSE:
                    if (metadataCBs->abortResponseCommandInd) {
                        metadataCBs->abortResponseCommandInd(addr, transaction, command.u.responsePDU);
                    } else {
                        status = OI_AVRCP_NOT_IMPLEMENTED;
                    }
                    break;
                    
                default:
                    status = OI_AVRCP_NOT_IMPLEMENTED;
                    break;
            }
        }
    }
    
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT2(("AVRCPMetadata_CmdInd: %!", status));

        /*
         * If this was not a malformed AVRCP command, but the profile failed to handle the frame either due to
         * unrecognized parameters or unimplemented functionality, and if the generic callback was provided by the app,
         * send the frame up to the application so that the user can decide how (and whether) to respond this command.
         * Otherwise, send either REJECTED or NOT IMPLEMENTED response (based on the value of the errorCode).
         */  
#ifndef OI_CODE
		if ((status != OI_AVRCP_RESPONSE_INTERNAL_ERROR) && (OI_AVRCP_This->targetCb->genericInd != NULL)
			&& (status != OI_AVRCP_RESPONSE_INVALID_PDU)) {
#else
#error code not present
#endif /* OI_CODE */
            OI_DBGPRINT(("%s: call OI_AVRCP_TARGET_GENERIC_IND(addr=%:,transaction=%d,frame=0x%x)",
                         (status == OI_AVRCP_NOT_IMPLEMENTED) ? "NOT IMPLEMENTED" : "REJECTED",
                         addr, transaction, frame));
            OI_AVRCP_This->targetCb->genericInd(addr, transaction, frame);
        } else {
            status = OI_AVRCPTarget_SendSimpleMetaReply(addr, transaction, command.id, status);

            if (!OI_SUCCESS(status)) {
                OI_DBGPRINT(("Failed to send error reply %!", status));
            }
        }
    }

    return OI_OK;
        
}

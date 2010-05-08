#ifdef FEATURE_BT_EXTPF_AV
/**
 * @file
 * @internal  
 * Audio/Video Remote Control Profile internal target-specific functions
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVRCP

#include "oi_bt_assigned_nos.h"
#include "oi_avctp.h"
#include "oi_avrcp.h"
#include "oi_avrcp_helper.h"
#include "oi_avrcp_metadata.h"
#include "oi_avrcp_consts.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_memmgr.h"
#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

/* SDP constants */
static const OI_DATAELEM AvrcpTargetServiceClass[] = {
    OI_ELEMENT_UUID32(OI_UUID_AV_RemoteControlTarget)
};

static OI_DATAELEM SupportedTargetCategories;

static const OI_SDPDB_ATTRIBUTE AvrcpTargetAttributeList[] = {
    { OI_ATTRID_ServiceClassIDList,                    OI_ELEMENT_SEQ(AvrcpTargetServiceClass) },
    { OI_ATTRID_ProtocolDescriptorList,                OI_ELEMENT_SEQ(OI_ProtocolDescriptorList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,        OI_ELEMENT_SEQ(OI_ProfileDescriptorList) },
    { OI_ATTRID_SupportedFeatures,                     OI_ELEMENT_REF(SupportedTargetCategories) }
};


/* Local function prototypes */

static void Target_ReplyWithFailure(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame, OI_UINT8 reason)
{
    OI_STATUS status;
    frame->cmd = reason;
    
    status = AVRCPCOMMON_Send(addr, transaction, frame);
    if (OI_SUCCESS(status)) {
        OI_DBGPRINT(("Sent failure response"));
    } else {
        OI_DBGPRINT(("Error sending failure response: %!", status));
    }
}

static OI_STATUS Target_HandleVendorDependent(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame)
{
    OI_STATUS status = OI_OK;
    
    OI_DBGPRINT(("Received vendor dependent command"));
    if (OI_AVRCP_This->targetCb->metadataCb != NULL) {
        status = AVRCPMetadata_CmdInd(addr, transaction, frame);
    } else if (OI_AVRCP_This->targetCb->genericInd != NULL) {
        OI_DBGTRACE(("OI_AVRCP_TARGET_GENERIC_IND(addr=%:,transaction=%d,frame=0x%x)",
                     addr, transaction, frame));
        OI_AVRCP_This->targetCb->genericInd(addr, transaction, frame);
    } else {
        status = OI_STATUS_NOT_IMPLEMENTED;
    }
    
    return status;
}

static OI_STATUS Target_SendUnitInfoReply(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame)
{
    OI_BYTE operand[] = { 0x7, OI_AVRCP_INFO_PAGE_DATA(OI_AVRCP_SUBUNIT_TYPE_PANEL, 0), 0, 0, 0};
    OI_AVRCP_FRAME replyFrame = {
        OI_AVRCP_RESPONSE_STABLE,
        OI_AVRCP_SUBUNIT_TYPE_UNIT,
        OI_AVRCP_SUBUNIT_ID_IGNORE,
        OI_AVRCP_OPCODE_UNIT_INFO,
        NULL,
        0
    };

    OI_DBGPRINT2(("Received UNIT INFO command"));

    replyFrame.operand = operand;
    replyFrame.operandCount = OI_ARRAYSIZE(operand);
    
    operand[2] = OI_AVRCP_This->target->companyID[0];
    operand[3] = OI_AVRCP_This->target->companyID[1];
    operand[4] = OI_AVRCP_This->target->companyID[2];
    
    return AVRCPCOMMON_Send(addr, transaction, &replyFrame);
}

static OI_STATUS Target_SendSubunitInfoReply(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame)
{
    OI_BYTE operand[] = { 0x07, 0xff, 0xff, 0xff, 0xff };
    OI_AVRCP_FRAME replyFrame = {
        0,
        OI_AVRCP_SUBUNIT_TYPE_UNIT,
        OI_AVRCP_SUBUNIT_ID_IGNORE,
        OI_AVRCP_OPCODE_SUBUNIT_INFO,
        NULL,
        0
    };

    OI_DBGPRINT2(("Received SUBUNIT INFO command"));
    
    replyFrame.operand = operand;
    replyFrame.operandCount = OI_ARRAYSIZE(operand);

    if (frame->operandCount == 5 && (frame->operand[0] == 0x07 || frame->operand[0] == 0xff)) {
        replyFrame.cmd = OI_AVRCP_RESPONSE_STABLE;
        replyFrame.operand[1] = OI_AVRCP_INFO_PAGE_DATA(OI_AVRCP_SUBUNIT_TYPE_PANEL, 0);
    } else {
        replyFrame.cmd = OI_AVRCP_RESPONSE_REJECTED;
    }

    return AVRCPCOMMON_Send(addr, transaction, &replyFrame);
}

static OI_STATUS Target_HandlePassThrough(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame)
{
    OI_UINT i;
    for (i = 0; i < frame->operandCount; ++i) {
        OI_DBGPRINT2(("operand[%d] = 0x%02x", i, frame->operand[i]));
    }

    /*
     * Treat vendor unique pass through commands as vendor dependent commands.
     */
    if (frame->operandCount > 0 && OI_AVRCP_OP_CODE(frame->operand[0]) == OI_AVRCP_OP_VENDOR_UNIQUE) {
        return Target_HandleVendorDependent(addr, transaction, frame);
    }
   
    if (frame->subunitType != OI_AVRCP_SUBUNIT_TYPE_PANEL || frame->subunitID != 0) {
        OI_LOG_ERROR(("Rejecting PASS THROUGH command directed at subunit <%d,%d>", frame->subunitType, frame->subunitID));
        Target_ReplyWithFailure(addr, transaction, frame, OI_AVRCP_RESPONSE_REJECTED);
    }

    if (frame->operandCount < 2 || frame->operand[1] != 0) {
        Target_ReplyWithFailure(addr, transaction, frame, OI_AVRCP_RESPONSE_NOT_IMPLEMENTED);
    } else {
        OI_UINT8 op = frame->operand[0];
        OI_DBGTRACE(("OI_AVRCP_TARGET_COMMAND_IND(addr=%:,transaction=%d,op=%d)", addr, transaction, op));
        OI_AVRCP_This->targetCb->commandInd(addr, transaction, op);
    }

    return OI_OK;
}


static void Target_AvctpRecvInd(OI_BD_ADDR *addr,
                                OI_UINT8 transaction,
                                OI_AVRCP_FRAME *frame)
{
    OI_STATUS status;
    OI_UINT8 reason;
    
    OI_DBGPRINT(("Target_AvctpRecvInd (opcode=0x%02x, cmd=0x%02x)", frame->opcode, frame->cmd));

    if (OI_AVRCP_This->targetCb == NULL) {
        OI_LOG_ERROR(("No target registered; rejecting command"));
        Target_ReplyWithFailure(addr, transaction, frame, OI_AVRCP_RESPONSE_NOT_IMPLEMENTED);
        return;
    }
    
    /* The 4-bit value of "transaction" was extracted at the AVCTP level and OI_AVRCP_MAX_TRANSACTIONS = 16 (spec)
     * so this assert here is just for sanity checking. */
    OI_ASSERT(transaction < OI_AVRCP_MAX_TRANSACTIONS);
    
    if (frame->opcode == OI_AVRCP_OPCODE_VENDOR_DEPENDENT) {
        OI_DBGPRINT2(("HandleVendorDependent"));
        status = Target_HandleVendorDependent(addr, transaction, frame);
    } else if (frame->opcode == OI_AVRCP_OPCODE_UNIT_INFO && frame->cmd == OI_AVRCP_CTYPE_STATUS) {
        status = Target_SendUnitInfoReply(addr, transaction, frame);
    } else if (frame->opcode == OI_AVRCP_OPCODE_SUBUNIT_INFO && frame->cmd == OI_AVRCP_CTYPE_STATUS) {
        status = Target_SendSubunitInfoReply(addr, transaction, frame);
#ifndef OI_CODE
    } else if (frame->opcode       == OI_AVRCP_OPCODE_PASS_THROUGH &&
               frame->cmd          == OI_AVRCP_CTYPE_CONTROL       &&
               frame->subunitType  == OI_AVRCP_SUBUNIT_TYPE_PANEL  &&
               frame->subunitID    == 0                            &&
               frame->operandCount >  1                            &&
               frame->operand[0]   == OI_AVRCP_OP_VENDOR_UNIQUE ) {
        status = Target_HandleVendorDependent(addr, transaction, frame);      
#endif /* OI_CODE */
    } else if (frame->opcode == OI_AVRCP_OPCODE_PASS_THROUGH && frame->cmd == OI_AVRCP_CTYPE_CONTROL) {
        status = Target_HandlePassThrough(addr, transaction, frame);
    } else {
        OI_LOG_ERROR(("Unknown opcode"));
        status = OI_AVRCP_NOT_IMPLEMENTED;
    }

    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("Dispatched target handler for opcode=%d, cmd=%d failed: %!", frame->opcode, frame->cmd, status));
        reason = (status == OI_AVRCP_NOT_IMPLEMENTED) ?
            OI_AVRCP_RESPONSE_NOT_IMPLEMENTED : OI_AVRCP_RESPONSE_REJECTED;
        Target_ReplyWithFailure(addr, transaction, frame, reason);
    }
}

/* Public API */

OI_STATUS OI_AVRCP_RegisterTarget(const OI_AVRCP_TARGET_CALLBACKS *cb, 
                                  OI_UINT16 categories, 
                                  OI_BYTE *companyID,
                                  const OI_SDP_STRINGS *strings)
{
    OI_STATUS status;

    OI_SDPDB_SERVICE_RECORD AvrcpTargetServiceRecord;

    OI_DBGTRACE(("OI_AVRCP_RegisterTarget(cb=0x%x,categors=%d,companyID=%@,strings=0x%x)",
                 cb, categories, companyID, 3, strings));
    OI_CHECK_INIT(AVRCP);

    ARGCHECK(cb != NULL);
    ARGCHECK(strings);

    if (OI_AVRCP_This->targetCb != NULL) {
        return OI_STATUS_ALREADY_REGISTERED;
    } 

    /* This Calloc must be made prior to accepting targetCb.   */
    /* The internal AVRCP indication of Target registration is */
    /* a non-NULL pointer in the targetCb member of "OI_AVRCP_This".    */
    OI_AVRCP_This->target = OI_Calloc(sizeof(AVRCP_TARGET));
    if (OI_AVRCP_This->target == NULL){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    OI_AVRCP_This->target->transactionTable = OI_Calloc(sizeof(AVRCP_TRANS_TABLE) * OI_AVRCP_This->maxConnections);
    if (OI_AVRCP_This->target->transactionTable == NULL){
        OI_FreeIf(&OI_AVRCP_This->target);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    OI_AVRCP_This->targetCb = cb;
    OI_AVRCP_This->Target_AvctpRecvInd = Target_AvctpRecvInd;

    if (companyID == NULL) {
        OI_AVRCP_This->target->companyID[0] = 0xff;
        OI_AVRCP_This->target->companyID[1] = 0xff;
        OI_AVRCP_This->target->companyID[2] = 0xff;
    } else {
        OI_AVRCP_This->target->companyID[0] = companyID[0];
        OI_AVRCP_This->target->companyID[1] = companyID[1];
        OI_AVRCP_This->target->companyID[2] = companyID[2];
    }

    OI_AVRCP_This->target->tgResponseFrameSize = OI_CONFIG_TABLE_GET(AVRCP)->tgResponseFrameSize;
    if((OI_AVRCP_This->target->tgResponseFrameSize <= 16) ||
       (OI_AVRCP_This->target->tgResponseFrameSize >= OI_AVRCP_MAX_FRAME_LENGTH)) {

        OI_AVRCP_This->target->tgResponseFrameSize = OI_AVRCP_MAX_FRAME_LENGTH;
    }
                           
    SupportedTargetCategories = ConstAvrcpSupportedCategories;
    SupportedTargetCategories.Value.UInt = categories;
    AvrcpTargetServiceRecord.Attributes = AvrcpTargetAttributeList;
    AvrcpTargetServiceRecord.NumAttributes = OI_ARRAYSIZE(AvrcpTargetAttributeList);
    AvrcpTargetServiceRecord.Strings = strings->attrs;
    AvrcpTargetServiceRecord.NumStrings = strings->num;

    status = OI_SDPDB_AddServiceRecord(&AvrcpTargetServiceRecord, &OI_AVRCP_This->target->sdpHandle);

    if (OI_SUCCESS(status)) {
        status = AVRCPCOMMON_Register();
    }

    if (!OI_SUCCESS(status)) {
        /* We failed to fully register */
        /* Free memory, clear targetCb, & SDP record */
        OI_SDPDB_RemoveServiceRecord(OI_AVRCP_This->target->sdpHandle);
        OI_AVRCP_This->targetCb = NULL;
        OI_FreeIf(&OI_AVRCP_This->target->transactionTable);
        OI_FreeIf(&OI_AVRCP_This->target);
    }
    
    return status;
}

OI_STATUS OI_AVRCP_DeregisterTarget( void )
{

    OI_INT i;
    
    OI_DBGTRACE(("OI_AVRCP_DeregisterTarget"));
    OI_CHECK_INIT(AVRCP);

    if (OI_AVRCP_This->target != NULL) {
        /* Cleanup Service Discovery */
        OI_SDPDB_RemoveServiceRecord(OI_AVRCP_This->target->sdpHandle);

        /* Flag AVRCP Target as deregistered */
        OI_AVRCP_This->targetCb = NULL;

        /* Free AVRCP Target memory allocation */
        for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
            OI_FreeIf(&OI_AVRCP_This->target->transactionTable[i].fragment);
        }
        OI_FreeIf(&OI_AVRCP_This->target->transactionTable);
        OI_FreeIf(&OI_AVRCP_This->target);

        /* Common deregister between Target/Controller with AVCTP */
        return AVRCPCOMMON_Deregister();
    } else {
        return OI_STATUS_NOT_REGISTERED;
    }
}

OI_STATUS OI_AVRCPTarget_SendReply(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_UINT8 op, OI_UINT8 response)
{
    OI_AVRCP_FRAME replyFrame;
    OI_BYTE operand[2];
    
    OI_DBGTRACE(("OI_AVRCPTarget_SendReply(addr=%:,transaction=%d,op=%d,response=%d)",
                 addr, transaction, op, response));
    OI_CHECK_INIT(AVRCP);

    ARGCHECK(addr != NULL);
    
    if (!OI_AVRCP_IS_RESPONSE(response)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    replyFrame.cmd = response;
    replyFrame.subunitType = OI_AVRCP_SUBUNIT_TYPE_PANEL;
    replyFrame.subunitID = OI_AVRCP_SUBUNIT_ID_0;
    replyFrame.opcode = OI_AVRCP_OPCODE_PASS_THROUGH;
    replyFrame.operand = operand;
    replyFrame.operandCount = OI_ARRAYSIZE(operand);
    operand[0] = op;
    operand[1] = 0;     /* No operation data */

    return AVRCPCOMMON_Send(addr, transaction, &replyFrame);
}

OI_STATUS OI_AVRCPTarget_SendGenericReply(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_AVRCP_FRAME *frame)
{
    OI_DBGTRACE(("OI_AVRCPTarget_SendGenericReply(addr=%:,transaction=%d,frame=0x%x)",
                 addr, transaction, frame));
    OI_CHECK_INIT(AVRCP);

    ARGCHECK(addr != NULL);
    
    if (!OI_AVRCP_IS_RESPONSE(frame->cmd)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    
    return AVRCPCOMMON_Send(addr, transaction, frame);
}

OI_STATUS OI_AVRCPTarget_GetServiceRecord(OI_UINT32 *handle)
{
    OI_DBGTRACE(("OI_AVRCPTarget_GetServiceRecord(handle=0x%x)", handle));
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(AVRCP);

    *handle = OI_AVRCP_This->target->sdpHandle;
    return OI_OK;
}
#endif /* FEATURE_BT_EXTPF_AV */

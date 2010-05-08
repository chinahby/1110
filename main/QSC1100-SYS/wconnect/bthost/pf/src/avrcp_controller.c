#ifdef FEATURE_BT_EXTPF_AV
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

#include "oi_bt_assigned_nos.h"
#include "oi_avctp.h"
#include "oi_avrcp.h"
#include "oi_avrcp_helper.h"
#include "oi_avrcp_consts.h"
#include "oi_avrcp_metadata.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_memmgr.h"
#include "oi_init_flags.h"
#include "oi_std_utils.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"


typedef struct {
    AVRCP_TRANS_TABLE *transactionTable;
    OI_UINT8 transId;
} AVRCP_TIMEOUT_ARGS;

/* SDP constants */
static const OI_DATAELEM AvrcpControllerServiceClass[] = {
    OI_ELEMENT_UUID32(OI_UUID_AV_RemoteControl)
};

static OI_DATAELEM SupportedControllerCategories;

static const OI_SDPDB_ATTRIBUTE AvrcpControllerAttributeList[] = {
    { OI_ATTRID_ServiceClassIDList,                    OI_ELEMENT_SEQ(AvrcpControllerServiceClass) },
    { OI_ATTRID_ProtocolDescriptorList,                OI_ELEMENT_SEQ(OI_ProtocolDescriptorList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,        OI_ELEMENT_SEQ(OI_ProfileDescriptorList) },
    { OI_ATTRID_SupportedFeatures,                     OI_ELEMENT_REF(SupportedControllerCategories) }
};

/* Local function prototypes */
static OI_STATUS FindFreeTransaction(OI_BD_ADDR *addr,
                                     AVRCP_TRANS_TABLE **pTransTable,
                                     OI_UINT8 *transId);

static OI_STATUS SetTimeout(AVRCP_TRANS_TABLE *transactionTable, OI_UINT8 transId, OI_INTERVAL timeout);
static void CancelTimeout(AVRCP_TRANS_TABLE *transactionTable, OI_UINT8 transId);
static void TimeoutFn(DISPATCH_ARG *arg);

/* Implementations */

void OI_AVRCPController_FlushTransactions(OI_BD_ADDR *addr, OI_STATUS reason)
{
    OI_UINT8 i;
    AVRCP_TRANS_TABLE *transactionTable;
    
    if (OI_AVRCP_This->controller == NULL) {
        return;
    }

    OI_ASSERT(OI_AVRCP_This->controllerCb != NULL);
    OI_ASSERT(OI_AVRCP_This->controllerCb->replyInd != NULL);

    transactionTable = LookupTransactionTable(addr, TRUE);

    if (NULL != transactionTable) {    
        /* Flush list of available transactions */
        for (i = 0; i < OI_AVRCP_This->maxTransactions; i++) {
            if (transactionTable->transaction[i].state != AVRCP_UNUSED) {
                OI_DBGPRINT(("Flushing transaction %d",i));
                OI_AVRCPController_CommandFailed(addr, i, reason);
            }
        }
    }
}

static OI_STATUS FindFreeTransaction(OI_BD_ADDR *addr,
                                     AVRCP_TRANS_TABLE **pTransactionTable,
                                     OI_UINT8 *transId)
{
    OI_INT i;

    AVRCP_TRANS_TABLE *transactionTable = NULL;
    
    OI_ASSERT(transId != NULL);
    
    transactionTable = LookupTransactionTable(addr, TRUE);

    if (NULL == transactionTable) {
        OI_DBGPRINT(("Transaction table not found: no address entry for %:", addr));
        return OI_STATUS_NOT_FOUND;
    }
    
    for (i = 0; i < OI_AVRCP_This->maxTransactions; i++) {
        OI_UINT8 transIndex = (i + transactionTable->nextTransaction) % OI_AVRCP_This->maxTransactions;                
        if (transactionTable->transaction[transIndex].state == AVRCP_UNUSED) {
            *pTransactionTable = transactionTable;
            transactionTable->nextTransaction = (transIndex + 1) % OI_AVRCP_This->maxTransactions;
            *transId = transIndex;
            return OI_OK;
        }
    }
    OI_DBGPRINT(("Too many outstanding transactions"));
    return OI_BUSY_FAIL;
}

void OI_AVRCPController_CommandFailed(OI_BD_ADDR *addr,
                                      OI_UINT8 transId,
                                      OI_STATUS status)
{
    AVRCP_TRANS_TABLE *transactionTable;

    transactionTable = LookupTransactionTable(addr, TRUE);

    if (NULL == transactionTable) {
        OI_DBGPRINT(("OI_AVRCPController_CommandFailed: No transaction table entry for address %:", addr));
        return;
    }

    if (OI_AVRCP_This->controllerCb != NULL) {
        OI_ASSERT(OI_AVRCP_This->controllerCb->replyInd != NULL);
        CancelTimeout(transactionTable, transId);
        OI_AVRCPCOMMON_FlushPacketByTransaction(&transactionTable->addr, transId);        

        OI_DBGTRACE(("OI_AVRCP_CONTROLLER_REPLY_IND(addr=%:,transaction=%d,status=%!)", 
                     &transactionTable->addr, transId, status));
        OI_AVRCP_This->controllerCb->replyInd(&transactionTable->addr, transId, NULL, status);
        transactionTable->transaction[transId].state = AVRCP_UNUSED;
    } else {
        OI_LOG_ERROR(("OI_AVRCPController_CommandFailed called, but no controller exists"));
    }
}


static OI_STATUS SendFrame(OI_BD_ADDR *addr, OI_UINT8 *transId, OI_AVRCP_FRAME *frame)
{
    OI_STATUS status;
    OI_UINT8 transIndex;
    AVRCP_TRANS_TABLE *transactionTable = NULL;
    OI_INTERVAL timeout;
    
    if (OI_AVRCP_This->controllerCb == NULL) {
        OI_DBGPRINT(("Attempted to send command without a registered controller"));
        return OI_FAIL;
    }
        
    status = FindFreeTransaction(addr, &transactionTable, &transIndex);
    if (!OI_SUCCESS(status)) {
        return status;
    }

    /* Choose timeout interval based on the command type */
    if (frame->opcode == OI_AVRCP_OPCODE_VENDOR_DEPENDENT) {
        if (frame->cmd == OI_AVRCP_CTYPE_CONTROL) {
            timeout = OI_AVRCP_This->controller->tMTC;
        } else {
            timeout = OI_AVRCP_This->controller->tMTP;
        }
    } else {
        timeout = OI_AVRCP_This->controller->tRCP;
    }
        
    status = SetTimeout(transactionTable, transIndex, timeout);

    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("Couldn't schedule timeout function: %!", status));
        return status;
    }

    status = AVRCPCOMMON_Send(addr, transIndex, frame);

    if (OI_SUCCESS(status)) {
        transactionTable->transaction[transIndex].state = AVRCP_AWAITING_REPLY;
        if (transId != NULL) {
            *transId = transIndex;
        }
    } else {
        CancelTimeout(transactionTable, transIndex);
    }

    return status;
}

void TimeoutFn(DISPATCH_ARG *arg)
{
    AVRCP_TIMEOUT_ARGS transactionTimeoutArgs;    
    AVRCP_TRANS_TABLE *transactionTable;
    OI_UINT8 transId;
    
    transactionTimeoutArgs = Dispatch_GetArg(arg, AVRCP_TIMEOUT_ARGS);
    transactionTable = transactionTimeoutArgs.transactionTable;
    transId = transactionTimeoutArgs.transId;
    
    OI_DBGPRINT(("Timeout on transaction %d", transId));
    transactionTable->transaction[transId].state = AVRCP_TIMED_OUT;
    transactionTable->transaction[transId].timeoutFn = 0;

    OI_AVRCPController_CommandFailed(&transactionTable->addr, transId, OI_TIMEOUT);

    transactionTable->transaction[transId].state = AVRCP_UNUSED;
}

void CancelTimeout(AVRCP_TRANS_TABLE *transactionTable,
                   OI_UINT8 transIndex)
{
    OI_DBGPRINT2(("Cancel timeout 0x%x on transaction %d\n", transactionTable->transaction[transIndex].timeoutFn, transIndex));
    (void)OI_Dispatch_CancelFunc(transactionTable->transaction[transIndex].timeoutFn);
    transactionTable->transaction[transIndex].timeoutFn = 0;
}

OI_STATUS SetTimeout(AVRCP_TRANS_TABLE *transactionTable,
                     OI_UINT8 transId,
                     OI_INTERVAL timeout)
{
    OI_STATUS status = OI_OK;
    DISPATCH_ARG arg;
    AVRCP_TIMEOUT_ARGS transactionTimeoutArgs;

    transactionTimeoutArgs.transactionTable = transactionTable;
    transactionTimeoutArgs.transId = transId;
        
    Dispatch_SetArg(arg, transactionTimeoutArgs);

    if (!OI_Dispatch_IsValidHandle(transactionTable->transaction[transId].timeoutFn)) {
        status = OI_Dispatch_RegisterFunc(&TimeoutFn, &arg, &transactionTable->transaction[transId].timeoutFn);
        OI_DBGPRINT(("Registered timeout function for transaction %d", transId));
    }

    if (!OI_SUCCESS(status)) {
        return status;
    }

    status = OI_Dispatch_SetFuncTimeout(transactionTable->transaction[transId].timeoutFn, timeout);

    if (!OI_SUCCESS(status)) {
        CancelTimeout(transactionTable, transId);
    } else {
        OI_DBGPRINT(("Set timeout value to %d", timeout));
    }
    
    return status;
}

static void Controller_AvctpRecvInd(OI_BD_ADDR *addr,
                                    OI_UINT8 transId,
                                    OI_AVRCP_FRAME *frame)
{
    AVRCP_TRANS_TABLE *transactionTable = NULL;
    OI_BOOL isInterim = (OI_AVRCP_RESPONSE_INTERIM == frame->cmd);
        
    OI_DBGPRINT(("Controller_AvctpRecvInd transaction %d", transId));

    if (isInterim) {
        OI_DBGPRINT(("Interim rsponse transaction 0x%x", transId));
    }

    if (OI_AVRCP_This->controllerCb == NULL) {
        OI_DBGPRINT(("No controller registered; ignoring reply"));
        return;
    }
    
    transactionTable = LookupTransactionTable(addr, TRUE);

    if (NULL == transactionTable) {
        OI_DBGPRINT(("Transaction table not found: no address entry for %:", addr));
        return;
    }
 
    if (transactionTable->transaction[transId].state != AVRCP_AWAITING_REPLY) {
        OI_DBGPRINT(("Received unexpected reply to transaction %d", transId));
        return;
    }
    
    CancelTimeout(transactionTable, transId);

    if (!isInterim) {
        transactionTable->transaction[transId].state = AVRCP_RECEIVED_REPLY;
    }
    
    OI_DBGTRACE(("OI_AVRCP_CONTROLLER_REPLY_IND(addr=%:,transaction=%d,reply=0x%x,status=%!)", 
                 addr, transId, frame, OI_OK));
    
    OI_DBGPRINT(("OI_AVRCP_CONTROLLER_REPLY_IND operandCount %d, operand[0] %d (%d)",
                 frame->operandCount, frame->operand[0], OI_AVRCP_OP_CODE(frame->operand[0])));    
    

    if ((frame->operandCount > 0) && (OI_AVRCP_OP_CODE(frame->operand[0]) == OI_AVRCP_OPCODE_VENDOR_DEPENDENT) && 
        (OI_AVRCP_This->controllerCb->metadataReplyInd != NULL)) {
            AVRCPMetadata_RecvInd(addr, transId, frame);
    } else {
        OI_AVRCP_This->controllerCb->replyInd(addr, transId, frame, OI_OK);
    }
    
    if (!isInterim) {
        transactionTable->transaction[transId].state = AVRCP_UNUSED;
    }
}


/* Public API */

OI_STATUS OI_AVRCP_RegisterController(const OI_AVRCP_CONTROLLER_CALLBACKS *cb, 
                                      OI_UINT16 categories,
                                      const OI_SDP_STRINGS *strings)
{
    OI_STATUS status;
    OI_INT i;
    OI_SDPDB_SERVICE_RECORD AvrcpControllerServiceRecord;

    OI_DBGTRACE(("OI_AVRCP_RegisterController(cb=0x%x,categories=%d,strings=0x%x)",
                 cb, categories, strings));
    OI_CHECK_INIT(AVRCP);

    ARGCHECK(cb != NULL);
    ARGCHECK(strings != NULL);

    if (OI_AVRCP_This->controllerCb != NULL) {
        return OI_STATUS_ALREADY_REGISTERED;
    }

    /* This Calloc must be made prior to accepting controllerCb.   */
    /* The internal AVRCP indication of Controller registration is */
    /* a non-NULL pointer in the controllerCb member of "OI_AVRCP_This".*/
    OI_AVRCP_This->controller = OI_Calloc(sizeof(AVRCP_CONTROLLER));
    if (OI_AVRCP_This->controller == NULL){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    OI_AVRCP_This->controller->transactionTable = OI_Calloc(sizeof(AVRCP_TRANS_TABLE) * OI_AVRCP_This->maxConnections);
    if (OI_AVRCP_This->controller->transactionTable == NULL){
        OI_FreeIf(&OI_AVRCP_This->controller);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
        OI_AVRCP_This->controller->transactionTable[i].transaction = OI_Calloc(sizeof(AVRCP_TRANS_TABLE_ENTRY) * OI_AVRCP_This->maxTransactions);
        if (OI_AVRCP_This->controller->transactionTable[i].transaction == NULL){
            status = OI_STATUS_OUT_OF_MEMORY;
            goto REGISTER_ERROR;
        }
        OI_AVRCP_This->controller->transactionTable[i].addr = OI_ZeroAddr;
    }

    OI_AVRCP_This->controllerCb = cb;
    OI_AVRCP_This->Controller_AvctpRecvInd = Controller_AvctpRecvInd;    

    OI_AVRCP_This->controller->tRCP = OI_CONFIG_TABLE_GET(AVRCP)->timeoutRCP;
    if (!OI_AVRCP_This->controller->tRCP) {
        OI_AVRCP_This->controller->tRCP = OI_AVRCP_TRCP * 4; /* give enough time to complete transaction */
    }
    
    OI_AVRCP_This->controller->tMTC = OI_CONFIG_TABLE_GET(AVRCP)->timeoutMTC;
    if (!OI_AVRCP_This->controller->tMTC) {
        OI_AVRCP_This->controller->tMTC = OI_AVRCP_TMTC * 4; /* give enough time to complete transaction */
    }
    
    OI_AVRCP_This->controller->tMTP = OI_CONFIG_TABLE_GET(AVRCP)->timeoutMTP;    
    if (!OI_AVRCP_This->controller->tMTP) {
        OI_AVRCP_This->controller->tMTP = OI_AVRCP_TMTP * 2; /* give enough time to complete transaction */
    }
    
    SupportedControllerCategories = ConstAvrcpSupportedCategories;
    SupportedControllerCategories.Value.UInt = categories;
    AvrcpControllerServiceRecord.Attributes = AvrcpControllerAttributeList;
    AvrcpControllerServiceRecord.NumAttributes = OI_ARRAYSIZE(AvrcpControllerAttributeList);
    AvrcpControllerServiceRecord.Strings = strings->attrs;
    AvrcpControllerServiceRecord.NumStrings = strings->num;

    status = OI_SDPDB_AddServiceRecord(&AvrcpControllerServiceRecord, &OI_AVRCP_This->controller->sdpHandle);

    if (OI_SUCCESS(status)) {
        status = AVRCPCOMMON_Register();
    }
    
    if (OI_SUCCESS(status)) {
        return OI_OK;
    }
    
REGISTER_ERROR:
    /* We failed to fully register */
    /* Free memory, clear controllerCb, & SDP record */
    OI_SDPDB_RemoveServiceRecord(OI_AVRCP_This->controller->sdpHandle);
    OI_AVRCP_This->controllerCb = NULL;
    for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
        OI_FreeIf(&OI_AVRCP_This->controller->transactionTable[i].transaction);
    }
    OI_FreeIf(&OI_AVRCP_This->controller->transactionTable);
    OI_FreeIf(&OI_AVRCP_This->controller);
    return status;
}

OI_STATUS OI_AVRCP_DeregisterController( void )
{
    OI_INT i;

    OI_DBGTRACE(("OI_AVRCP_DeregisterController"));

    OI_CHECK_INIT(AVRCP);

    if (OI_AVRCP_This->controller != NULL) {
        /* Cleanup Service Discovery */
        OI_SDPDB_RemoveServiceRecord(OI_AVRCP_This->controller->sdpHandle);

        /* Flag AVRCP Controller as deregistered */
        OI_AVRCP_This->controllerCb = NULL;

        /* Free AVRCP Controller memory allocation */
        for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
            OI_FreeIf(&OI_AVRCP_This->controller->transactionTable[i].transaction);
        }
        OI_FreeIf(&OI_AVRCP_This->controller->transactionTable);
        OI_FreeIf(&OI_AVRCP_This->controller);

        /* Common deregister between Target/Controller with AVCTP */
        return AVRCPCOMMON_Deregister();
    } else {
        return OI_STATUS_NOT_REGISTERED;
    }
}

OI_STATUS OI_AVRCPController_SendCommand(OI_BD_ADDR *addr,
                                         OI_UINT8 *transId,
                                         OI_UINT8 operation)
{
    OI_BYTE operand[2];
    OI_AVRCP_FRAME frame = {
        OI_AVRCP_CTYPE_CONTROL,
        OI_AVRCP_SUBUNIT_TYPE_PANEL,
        OI_AVRCP_SUBUNIT_ID_0,
        OI_AVRCP_OPCODE_PASS_THROUGH,
    };

    OI_DBGTRACE(("OI_AVRCPController_SendCommand(addr=%:,transaction=0x%x,operation=%d)",
                 addr, transId, operation));

    OI_CHECK_INIT(AVRCP);

    ARGCHECK(addr != NULL);
    ARGCHECK(transId != NULL);
    
    frame.operand = operand;
    frame.operandCount = OI_ARRAYSIZE(operand);
    operand[0] = operation;
    operand[1] = 0;     /* No operation data */
    
    return SendFrame(addr, transId, &frame);
}

OI_STATUS OI_AVRCPController_GetUnitInfo(OI_BD_ADDR *addr,
                                         OI_UINT8 *transId)
{
    OI_BYTE operand[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };
    OI_AVRCP_FRAME frame = {
        OI_AVRCP_CTYPE_STATUS,
        OI_AVRCP_SUBUNIT_TYPE_UNIT,
        OI_AVRCP_SUBUNIT_ID_IGNORE,
        OI_AVRCP_OPCODE_UNIT_INFO,
    };

    OI_DBGTRACE(("OI_AVRCPController_GetUnitInfo(addr=%:,transaction=0x%x)", 
                 addr, transId));

    OI_CHECK_INIT(AVRCP);

    frame.operand = operand;
    frame.operandCount = OI_ARRAYSIZE(operand);
    
    return SendFrame(addr, transId, &frame);
}

OI_STATUS OI_AVRCPController_GetSubunitInfo(OI_BD_ADDR *addr,
                                            OI_UINT8 *transId,
                                            OI_UINT8 page)
{
    OI_BYTE operand[5] = { 0x00, 0xff, 0xff, 0xff, 0xff};
    OI_AVRCP_FRAME frame = {
        OI_AVRCP_CTYPE_STATUS,
        OI_AVRCP_SUBUNIT_TYPE_UNIT,
        OI_AVRCP_SUBUNIT_ID_IGNORE,
        OI_AVRCP_OPCODE_SUBUNIT_INFO,
    };

    OI_DBGTRACE(("OI_AVRCPController_GetSubunitInfo(addr=%:,transaction=0x%x,page=%d)",
                 addr, transId, page));

    OI_CHECK_INIT(AVRCP);

    ARGCHECK(page <= OI_AVRCP_SUBUNIT_INFO_PAGE_MAX);
    
    frame.operand = operand;
    frame.operandCount = OI_ARRAYSIZE(operand);
    operand[0] = OI_AVRCP_SUBUNIT_INFO_PAGE_NUMBER(page);
    
    return SendFrame(addr, transId, &frame);
}

OI_STATUS OI_AVRCPController_SendGeneric(OI_BD_ADDR *addr,
                                         OI_UINT8 *transId,
                                         OI_AVRCP_FRAME *frame)
{
    OI_DBGTRACE(("OI_AVRCPController_SendGeneric(addr=%:,transaction=0x%x,frame=0x%x)",
                 addr, transId, frame));
    
    OI_CHECK_INIT(AVRCP);

    return SendFrame(addr, transId, frame);
}


OI_STATUS OI_AVRCPController_GetServiceRecord(OI_UINT32 *handle)
{
    OI_DBGTRACE(("OI_AVRCPController_GetServiceRecord(handle=0x%x)", handle));
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(AVRCP);

    *handle = OI_AVRCP_This->controller->sdpHandle;
    return OI_OK;
}
#endif /* FEATURE_BT_EXTPF_AV */

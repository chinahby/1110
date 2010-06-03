#ifdef FEATURE_BT_EXTPF_AV
/**
 * @file
 * @internal  
 * Audio/Video Remote Control Profile implementation
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVRCP

#include "oi_avctp.h"
#include "oi_avrcp.h"
#include "oi_l2cap.h"
#include "oi_std_utils.h"
#include "oi_memmgr.h"
#include "oi_bt_assigned_nos.h"
#include "oi_avrcp_helper.h"
#include "oi_argcheck.h"
#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

/* This symbol must be global, and be aliased to "this" */
AVRCP_THIS * OI_AVRCP_This;

OI_STATUS OI_AVRCP_Register(const OI_AVRCP_CALLBACKS *cb)
{
    OI_DBGTRACE(("OI_AVRCP_Register(cb=0x%x)", cb));

    if ( OI_INIT_FLAG_VALUE(AVRCP) ){
        return OI_STATUS_ALREADY_REGISTERED;
    }

    ARGCHECK(cb != NULL);

    OI_AVRCP_This = OI_Calloc(sizeof(AVRCP_THIS));
    if( OI_AVRCP_This == NULL ){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    OI_AVRCP_This->cb = cb;
    
    OI_AVRCP_This->maxTransactions = OI_CONFIG_TABLE_GET(AVRCP)->maxTransactions;
    if ((OI_AVRCP_This->maxTransactions < 1) || (OI_AVRCP_This->maxTransactions > AVRCP_MAX_TRANSACTIONS)) {
        OI_AVRCP_This->maxTransactions  = AVRCP_MAX_TRANSACTIONS;
    }
        
    OI_AVRCP_This->maxConnections = OI_CONFIG_TABLE_GET(AVRCP)->maxConnections;
    if ((OI_AVRCP_This->maxConnections < 1) || ((OI_AVRCP_This->maxConnections > AVRCP_MAX_CONNECTIONS))) {
        OI_AVRCP_This->maxConnections = AVRCP_MAX_CONNECTIONS;
    }
    
    OI_AVRCP_This->metadata = OI_CONFIG_TABLE_GET(AVRCP)->metadata;

    AvrcpProfileVersionInit(OI_AVRCP_This->metadata);
    
    OI_INIT_FLAG_PUT_FLAG(TRUE,AVRCP);

    return OI_OK;
}

OI_STATUS OI_AVRCP_Deregister( void )
{
    OI_DBGTRACE(("OI_AVRCP_Deregister"));

    OI_CHECK_INIT(AVRCP);

    if ( (OI_AVRCP_This->controllerCb != NULL) ||
         (OI_AVRCP_This->targetCb != NULL) ){
        return OI_STATUS_INVALID_COMMAND;
    }

    if ( (OI_AVRCP_This->packetq.head != NULL) ||
         (OI_AVRCP_This->packetq.tail != NULL) ){
        return OI_BUSY_FAIL;
    }

    OI_FreeIf(&OI_AVRCP_This);
    OI_INIT_FLAG_PUT_FLAG(FALSE,AVRCP);

    return OI_OK;
}

OI_STATUS OI_AVRCP_Connect(OI_BD_ADDR *addr, OI_L2CAP_CONNECT_PARAMS *l2cap)
{
    OI_STATUS status;
    
    OI_DBGTRACE(("OI_AVRCP_Connect(addr=%:,l2cap=0x%x)", addr, l2cap));

    OI_CHECK_INIT(AVRCP);
    
    if ((OI_AVRCP_This->numConnections + 1) <= OI_AVRCP_This->maxConnections) {
        if (OI_AVRCP_This->controller) {
            status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECT_PENDING, OI_AVRCP_This->controller->transactionTable);
        if (!OI_SUCCESS(status)) {
            return status;
        }
        }
        if (OI_AVRCP_This->target) {
            status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECT_PENDING, OI_AVRCP_This->target->transactionTable);
            if (!OI_SUCCESS(status)) {
                return status;
            }
        }
        OI_AVRCP_This->numConnections += 1;
    } else {
        OI_DBGTRACE(("OI_AVRCP_Connect(addr=%:) exceeded maximum number of connections %d",
                     addr, OI_AVRCP_This->maxConnections));
        return OI_AVRCP_TOO_MANY_CONNECTIONS;
    }    

    status = OI_AVCTP_Connect(addr, OI_UUID_AV_RemoteControl, l2cap);

    if (!OI_SUCCESS(status)) {
        if (OI_AVRCP_This->numConnections > 0 ) {
            OI_AVRCP_This->numConnections -= 1;
        }        
    }

    return status;
}

OI_STATUS OI_AVRCP_Accept(OI_BD_ADDR *addr, OI_BOOL allow)
{
    OI_STATUS  status;
    OI_DBGTRACE(("OI_AVRCP_Accept(addr=%:,allow=%B)", addr, allow));

    OI_CHECK_INIT(AVRCP);

    if (allow == TRUE) {
        if ((OI_AVRCP_This->numConnections + 1) <= OI_AVRCP_This->maxConnections) {
            
            if (OI_AVRCP_This->controller) {
                status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECT_PENDING, OI_AVRCP_This->controller->transactionTable);
                if (!OI_SUCCESS(status)) {
                    return status;
                }
            }
            if (OI_AVRCP_This->target) {
                status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECT_PENDING, OI_AVRCP_This->target->transactionTable);
            if (!OI_SUCCESS(status)) {
                return status;
            }
            }            
            OI_AVRCP_This->numConnections += 1;            
        } else {
            OI_DBGTRACE(("OI_AVRCP_Accept(addr=%:) exceeded maximum number of connections %d",
                         addr, OI_AVRCP_This->maxConnections));
            return OI_AVRCP_TOO_MANY_CONNECTIONS;
        }
    }    
    return OI_AVCTP_Accept(addr, allow);
}

OI_STATUS OI_AVRCP_Disconnect(OI_BD_ADDR *addr)
{
    OI_STATUS status;
        
    OI_DBGTRACE(("OI_AVRCP_Disconnect(addr=%:)", addr));

    OI_CHECK_INIT(AVRCP);

    status = OI_AVCTP_Disconnect(addr, OI_UUID_AV_RemoteControl);

    if (OI_SUCCESS(status)) {
        if (OI_AVRCP_This->controller) {
            status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_DISCONNECT_PENDING, OI_AVRCP_This->controller->transactionTable);
            if (!OI_SUCCESS(status)) {
                return status;
            }
        }
        if (OI_AVRCP_This->target) {
            status = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_DISCONNECT_PENDING, OI_AVRCP_This->target->transactionTable);
            if (!OI_SUCCESS(status)) {
                return status;
            }
        }
    } else {
        OI_DBGTRACE(("OI_AVRCP_Disconnect(addr=%:) failed %!", addr, status));
    }
    
    return status;
}
#endif /* FEATURE_BT_EXTPF_AV */

#ifdef FEATURE_BT_EXTPF_CTP
/**
 * @file
 *
 * This file handles CTP gateway initialization. This involves registering the 
 * correct service record and initializing the Intercom Profile module.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_CTP

#include "oi_stddefs.h"
#include "oi_bt_assigned_nos.h"
#include "oi_memmgr.h"
#include "oi_std_utils.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_init_flags.h"
#include "oi_devmgr.h"

#include "oi_tcs.h"
#include "oi_sdpdb.h"
#include "oi_ctp.h"
#include "oi_intercom.h"

static OI_UINT32 srecHandle;

/******************************************************************

  Service Record

*****************************************************************/

/** The network type
        Note: this variable is static in order that its address can be known at compile-time
            This lets us put a constant pointer into the SDP descriptor. 
*/
static OI_DATAELEM NetworkType;

/* Service class ID list */
static const OI_DATAELEM ServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_CordlessTelephony),
    OI_ELEMENT_UUID32(OI_UUID_GenericTelephony)
};

/* Protocol Descriptor List */
static const OI_DATAELEM L2CAP_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_L2CAP)
};

static const OI_DATAELEM TCS_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_TCS_BIN)
};

static const OI_DATAELEM ProtocolDescriptorList[] = {
    OI_ELEMENT_SEQ(L2CAP_Descriptor),
    OI_ELEMENT_SEQ(TCS_Descriptor)
};

/* Profile descriptor list */
static const OI_DATAELEM Profile0[] = {
    OI_ELEMENT_UUID32(OI_UUID_CordlessTelephony),
    OI_ELEMENT_UINT16(0x0100) //version 1.0
};

static const OI_DATAELEM ProfileDescriptorList[] = {
    OI_ELEMENT_SEQ(Profile0)
};

/* SDP Attribute lists */
static const OI_SDPDB_ATTRIBUTE ServiceDescription[] = {
    { OI_ATTRID_ServiceClassIDList,                   OI_ELEMENT_SEQ(ServiceClassIDList) },
    { OI_ATTRID_ProtocolDescriptorList,               OI_ELEMENT_SEQ(ProtocolDescriptorList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,       OI_ELEMENT_SEQ(ProfileDescriptorList) },
    { OI_ATTRID_External_Network,                     OI_ELEMENT_REF(NetworkType) }
};


/**
 * This function initialize a Cordless Telephony Profile gateway.
 * 
 * @param  strings      the name to advertise in the service record
 * @param  networkType  the type of network the gateway is connected to
 * @param  TCSCallbacks the callbacks to use
 */
OI_STATUS OI_CTP_GW_Register(const OI_SDP_STRINGS *strings,
                             OI_UINT8 networkType,
                             const OI_TCS_CALLBACKS *TCSCallbacks)
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;
    
    if (OI_INIT_FLAG_VALUE(CTP)) {
        return OI_STATUS_ALREADY_INITIALIZED;
    }

    srec.Attributes = ServiceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(ServiceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;
  
    /* Register with TCS. */
    result = OI_TCS_Register(OI_TCS_ROLE_GATEWAY, FALSE, TCSCallbacks);
    if (!OI_SUCCESS(result)) {
        return OI_STATUS_INITIALIZATION_FAILED;
    }

    if (networkType > OI_CTP_NETWORK_OTHER) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_SET_UINT_ELEMENT(NetworkType, networkType);

    //Register service record
    result = OI_SDPDB_AddServiceRecord(&srec, &srecHandle);
    if (!OI_SUCCESS(result)) {
        OI_TCS_Deregister();
        return OI_STATUS_INITIALIZATION_FAILED;
    }

    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_TELEPHONY);
    OI_INIT_FLAG_PUT_FLAG(TRUE, CTP);
    return OI_OK;
}

OI_STATUS OI_CTP_GW_Deregister(void)
{
    OI_STATUS result;

    if (!OI_INIT_FLAG_VALUE(CTP)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    result = OI_TCS_Deregister();
    if (!OI_SUCCESS(result)) {
        return result;
    }
    OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_TELEPHONY);
    OI_SDPDB_RemoveServiceRecord(srecHandle);
    
    OI_INIT_FLAG_PUT_FLAG(FALSE, CTP);

    return OI_OK;
}
#endif /* FEATURE_BT_EXTPF_CTP */

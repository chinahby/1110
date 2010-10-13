#ifdef FEATURE_BT_EXTPF_CTP
/**
 * @file  
 *
 * This file implements the Intercom Profile.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_INTERCOM

#include "oi_stddefs.h"
#include "oi_bt_assigned_nos.h"
#include "oi_memmgr.h"
#include "oi_std_utils.h"
#include "oi_debug.h"
#include "oi_assert.h"

#include "oi_tcs.h"
#include "oi_intercom.h"
#include "oi_sdpdb.h"

static OI_UINT32 srecHandle;

/******************************************************************

  Service Record

*****************************************************************/

/* Service class ID list */
static const OI_DATAELEM ServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_Intercom),
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
    OI_ELEMENT_UUID32(OI_UUID_Intercom),
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
};

/******************************************************************

  external functions

*****************************************************************/

/**
 * This function registers a service record for the Intercom Profile.
 *
 * @param strings  the name to register
 */
OI_STATUS OI_Intercom_RegisterServiceRecord(const OI_SDP_STRINGS *strings)
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;

    srec.Attributes = ServiceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(ServiceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;

    //Register service record
    result = OI_SDPDB_AddServiceRecord(&srec, &srecHandle);
    if (!OI_SUCCESS(result)) {
        return OI_STATUS_INITIALIZATION_FAILED;
    }
    return OI_OK;
}

OI_STATUS OI_Intercom_DeregisterServiceRecord(void)
{
    return OI_SDPDB_RemoveServiceRecord(srecHandle);
}
#endif /* FEATURE_BT_EXTPF_CTP */

#ifdef FEATURE_BT_EXTPF_AV

/**
@file 
This file implements the Advanced Audio Distribution Profile (AADP or A2DP).
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_A2DP

#include "oi_core_common.h"
#include "oi_testprint.h"
#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_dataelem.h"
#include "oi_sdpdb.h"
#include "oi_devmgr.h"

typedef struct {

    OI_UINT32 sinkSrecHandle;

    OI_UINT32 sourceSrecHandle;

} A2DP_STATE;

static A2DP_STATE *this;

/************************************************

  Source service record

************************************************/

/** the optional features supported */
static OI_DATAELEM SourceFeatures;
static OI_DATAELEM SinkFeatures;

/** service class ID list */
static const OI_DATAELEM SourceServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_AudioSource)
};

static const OI_DATAELEM SinkServiceClassIDList[] = {
    OI_ELEMENT_UUID32(OI_UUID_AudioSink)
};

/* protocol descriptor list */
static const OI_DATAELEM Protocol0[] = {
    OI_ELEMENT_UUID32(OI_UUID_L2CAP),
    OI_ELEMENT_UINT16(OI_PSM_AVDTP)
};

static const OI_DATAELEM Protocol1[] = {
    OI_ELEMENT_UUID32(OI_UUID_AVDTP),
    OI_ELEMENT_UINT16(0x0100) //version 1.0
};

static const OI_DATAELEM ProtocolDescriptorList[] = {
    OI_ELEMENT_SEQ(Protocol0),
    OI_ELEMENT_SEQ(Protocol1)
};

/* profile descriptor list */
static const OI_DATAELEM Profile0[] = {
    OI_ELEMENT_UUID32(OI_UUID_AdvancedAudioDistribution),
    OI_ELEMENT_UINT16(0x0100) //version 1.0
};

static const OI_DATAELEM ProfileDescriptorList[] = {
    OI_ELEMENT_SEQ(Profile0)
};

/** SDP attribute lists */
static const OI_SDPDB_ATTRIBUTE SourceDescription[] = {
    { OI_ATTRID_ServiceClassIDList,                      OI_ELEMENT_SEQ(SourceServiceClassIDList) },
    { OI_ATTRID_ProtocolDescriptorList,                  OI_ELEMENT_SEQ(ProtocolDescriptorList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,          OI_ELEMENT_SEQ(ProfileDescriptorList) }, //optional
    { OI_ATTRID_SupportedFeatures,                       OI_ELEMENT_REF(SourceFeatures) } //optional
};

static const OI_SDPDB_ATTRIBUTE SinkDescription[] = {
    { OI_ATTRID_ServiceClassIDList,                      OI_ELEMENT_SEQ(SinkServiceClassIDList) },
    { OI_ATTRID_ProtocolDescriptorList,                  OI_ELEMENT_SEQ(ProtocolDescriptorList) },
    { OI_ATTRID_BluetoothProfileDescriptorList,          OI_ELEMENT_SEQ(ProfileDescriptorList) }, //optional
    { OI_ATTRID_SupportedFeatures,                       OI_ELEMENT_REF(SinkFeatures) } //optional
};


static OI_STATUS internalInit(void)
{
    this = OI_Malloc(sizeof(A2DP_STATE));
    if (!this) 
	{
        return OI_STATUS_OUT_OF_MEMORY;
    }
    
    this->sinkSrecHandle = 0;
    this->sourceSrecHandle = 0;

    OI_INIT_FLAG_PUT_FLAG(TRUE, A2DP);
    return OI_OK;
}

static void internalTerminate(void)
{
    if ((0 == this->sinkSrecHandle)
        && (0 == this->sourceSrecHandle)) {
        
        OI_FreeIf(&this);
        OI_INIT_FLAG_PUT_FLAG(FALSE, A2DP);
    }
}

OI_STATUS OI_A2DP_RegisterSource(OI_UINT32 features,
                                 const OI_SDP_STRINGS *strings)
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;

    if (!OI_INIT_FLAG_VALUE(A2DP)) {
        result = internalInit();
        if (!OI_SUCCESS(result)) {
            return result;
        }
    }

    srec.Attributes = SourceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(SourceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;

    OI_SET_UINT_ELEMENT(SourceFeatures, features);
    
    result = OI_SDPDB_AddServiceRecord(&srec, &this->sourceSrecHandle);
    if (!OI_SUCCESS(result)) {
        this->sourceSrecHandle = 0;
        internalTerminate();
        return result;
    }
    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_CAPTURING);
    OI_INIT_FLAG_PUT_FLAG(TRUE, A2DP);
    return OI_OK;
}

OI_STATUS OI_A2DP_RegisterSink(OI_UINT32 features,
                               const OI_SDP_STRINGS *strings)
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;

    if (!OI_INIT_FLAG_VALUE(A2DP)) {
        result = internalInit();
        if (!OI_SUCCESS(result)) {
            return result;
        }
    }

    srec.Attributes = SinkDescription;
    srec.NumAttributes = OI_ARRAYSIZE(SinkDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;

    OI_SET_UINT_ELEMENT(SinkFeatures, features);
    
    result = OI_SDPDB_AddServiceRecord(&srec, &this->sinkSrecHandle);
    if (!OI_SUCCESS(result)) {
        this->sinkSrecHandle = 0;
        internalTerminate();
        return result;
    }
    OI_DEVMGR_RegisterServiceClass(OI_SERVICE_CLASS_RENDERING);
    OI_INIT_FLAG_PUT_FLAG(TRUE, A2DP);
    return OI_OK;
}

OI_STATUS OI_A2DP_DeregisterSource(void)
{
    if (!OI_INIT_FLAG_VALUE(A2DP)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    OI_SDPDB_RemoveServiceRecord(this->sourceSrecHandle);
    OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_CAPTURING);
    this->sourceSrecHandle = 0;
    internalTerminate();
    return OI_OK;
}

OI_STATUS OI_A2DP_DeregisterSink(void)
{
    if (!OI_INIT_FLAG_VALUE(A2DP)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    OI_SDPDB_RemoveServiceRecord(this->sinkSrecHandle);
    OI_DEVMGR_DeregisterServiceClass(OI_SERVICE_CLASS_RENDERING);
    this->sinkSrecHandle = 0;
    internalTerminate();
    return OI_OK;
}

#endif /* FEATURE_BT_EXTPF_AV */


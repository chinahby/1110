/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/syncml_server.c#1 $ 

$Log: $
*
*    #7       18 Oct  2006            PR
*  Merged OI Code v3.0.14rev1revQ
*
*    #6        29 Jul 2006            BK
* Modified to initiate obex authentication from the client.
*
*    #5        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #4        30 Aug 2004            JH
* Rolled back to #2; eliminated the use of connect policy for SD registration.
*  SD registration is now handled in bt_pf_sml_cmd_register_client().
* Modified prototype of OI_SyncML_RegisterServer() to add RFCOMM server channel
*  number as output parameter.
*
*    #3        30 Apr 2004            JH
* Changed client register command to pass in connect policy so that
*  SIO can perform SDP registration
*

===========================================================================*/
/** @file
 * @internal
 * Server side of SyncML OBEX binding for Bluetooth
 *
 * This API provides a thin layer of abstraction on top of the GenOBEX API.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_SYNCML


#include "oi_core_common.h"
#include "oi_dataelem.h"
#include "oi_syncml.h"
#include "oi_memmgr.h"
#include "oi_obexcli.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"
#include "oi_sdpdb.h"
#include "oi_argcheck.h"
#include "oi_init_flags.h"
#include "oi_syncml_common.h"

static const OI_UUID128 ServerUUID = OI_UUID_SyncMLServer;

/** the RFCOMM server channel */
static OI_DATAELEM ServerChannel ;

/* service class ID list */
static const OI_DATAELEM ServiceClassIDList[] = {
    OI_ELEMENT_UUID128(&ServerUUID)
};

/* protocol descriptor list */
static const OI_DATAELEM L2CAP_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_L2CAP)
};

static const OI_DATAELEM RFCOMM_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_RFCOMM),
    OI_ELEMENT_REF(ServerChannel)
};

static const OI_DATAELEM OBEX_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_OBEX)
};

static const OI_DATAELEM ProtocolDescriptorList[] = {
    OI_ELEMENT_SEQ(L2CAP_Descriptor),
    OI_ELEMENT_SEQ(RFCOMM_Descriptor),
    OI_ELEMENT_SEQ(OBEX_Descriptor)
};

/** SDP attribute lists */
static const OI_SDPDB_ATTRIBUTE ServiceDescription[] = {
    { OI_ATTRID_ServiceClassIDList,                   OI_ELEMENT_SEQ(ServiceClassIDList) },
    { OI_ATTRID_ProtocolDescriptorList,               OI_ELEMENT_SEQ(ProtocolDescriptorList) },
};

typedef struct{
    OI_UINT32   srec;
    OI_UINT8    channel;
}SYNCML_SERVER_THIS;

static SYNCML_SERVER_THIS * SyncML_This;

#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_SyncML_RegisterServer(const OI_SDP_STRINGS *strings,
                                   OI_BOOL authentication,
                                   OI_OBEXSRV_CB_LIST *CBList,
                                   OI_CHAR *uuid,
                                   OI_UINT8 *channel)
#endif
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;
    OI_OBEX_BYTESEQ target;
    OI_INIT_FLAG flag = OI_INIT_FLAG_VALUE(SYNCML);
            
    if( flag & SYNCML_SERVER_MASK ){
        return OI_STATUS_ALREADY_REGISTERED;
    }

    SyncML_This = OI_Malloc(sizeof(SYNCML_SERVER_THIS));
    if ( SyncML_This == NULL ){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    target.data = (OI_BYTE *)uuid;
    target.len = OI_StrLen(uuid);

    srec.Attributes =  ServiceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(ServiceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;
    
    /* Register with OBEX. */
    result = OI_OBEXSRV_RegisterServer(&target,
                                       CBList,
                                       authentication ? OI_OBEXSRV_AUTH_PASSWORD : OI_OBEXSRV_AUTH_NONE,
                                       &SyncML_This->channel,
                                       NULL);
    if (!OI_SUCCESS(result)) {
        OI_FreeIf(&SyncML_This);
        return OI_STATUS_INITIALIZATION_FAILED;
    }

    /* Register service record. */
    OI_SET_UINT_ELEMENT(ServerChannel, SyncML_This->channel);
    result = OI_SDPDB_AddServiceRecord(&srec, &SyncML_This->srec);

    if (OI_SUCCESS(result)) {
        OI_DBGPRINT2(("Registered SyncML server, RFCOMM channel: %d\n", SyncML_This->channel));
#ifndef OI_CODE
        if ( channel != NULL ){
            *channel = SyncML_This->channel;
        }
#endif
        OI_INIT_FLAG_PUT_FLAG( flag | SYNCML_SERVER_MAX, SYNCML);
    }else{
        OI_OBEXSRV_DeregisterServer(SyncML_This->channel);
        OI_FreeIf(&SyncML_This);
    }
    return result;
}


OI_STATUS OI_SyncML_DeregisterServer( void )
{
    OI_INIT_FLAG flag = OI_INIT_FLAG_VALUE(SYNCML);

    if( flag & SYNCML_SERVER_MASK ){
        OI_OBEXSRV_DeregisterServer(SyncML_This->channel);
        OI_SDPDB_RemoveServiceRecord(SyncML_This->srec);
        flag &= SYNCML_CLIENT_MASK;
        OI_INIT_FLAG_PUT_FLAG( flag, SYNCML);
        OI_FreeIf(&SyncML_This);
        return OI_OK;
    }
    return OI_STATUS_NOT_REGISTERED;
}


OI_STATUS OI_SyncML_GetServerServiceRecord(OI_UINT32 *handle)
{
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(SYNCML);

    *handle = SyncML_This->srec;
    return OI_OK;
}


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/syncml_client.c#1 $ 

$Log: $
*
*    #7        18 Oct  2006           PR
* Merged OI Code v3.0.14rev1revQ
*
*    #6        28 July 2006           BK
* Modified to initiate obex authentication from client
*
*    #5        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #4        30 Aug 2004            JH
* Rolled back to #2; eliminated the use of connect policy for SD registration.
* SD registration is now handled in bt_pf_sml_cmd_register_client().
*
*    #3        30 Apr 2004            JH
* Changed client register command to pass in connect policy so that
*  SIO can perform SDP registration
*

===========================================================================*/
/** @file
 * @internal
 * client side of SyncML OBEX binding for Bluetooth
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_SYNCML

#include "oi_core_common.h"
#include "oi_dataelem.h"
#include "oi_syncml.h"
#include "oi_obexcli.h"
#include "oi_obexspec.h"
#include "oi_obexsrv.h"
#include "oi_sdpdb.h"
#include "oi_argcheck.h"
#include "oi_modules.h"
#include "oi_init_flags.h"
#include "oi_syncml_common.h"

static const OI_UUID128 ServerUUID = OI_UUID_SyncMLClient;

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


typedef struct _SYNCML_CLIENT_THIS_{
    struct _SYNCML_CLIENT_THIS_ *   next;
    OI_UINT32                       srec;
    OI_UINT8                        channel;
}SYNCML_CLIENT_THIS;

static SYNCML_CLIENT_THIS *this;

#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_SyncML_RegisterClient10(const OI_SDP_STRINGS *strings,
                                   OI_BOOL authentication,
                                   OI_OBEXSRV_CB_LIST *CBList,
                                   OI_CHAR *uuid)
#endif
{
    return OI_SyncML_RegisterClient(strings,authentication,CBList,uuid,NULL);
}

#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_SyncML_RegisterClient(const OI_SDP_STRINGS *strings,
                                   OI_BOOL authentication,
                                   OI_OBEXSRV_CB_LIST *CBList,
                                   OI_CHAR *uuid,
                                   OI_UINT8 *channel)
#endif
{
    OI_STATUS result;
    OI_SDPDB_SERVICE_RECORD srec;
    OI_OBEX_BYTESEQ target;
    SYNCML_CLIENT_THIS *new;
    
    ARGCHECK(uuid);
    ARGCHECK(strings);
    ARGCHECK(CBList);

    if ( (OI_INIT_FLAG_VALUE(SYNCML) & SYNCML_CLIENT_MASK) >= SYNCML_CLIENT_MAX ){
        return OI_STATUS_ALREADY_REGISTERED;
    }

    new = OI_Malloc(sizeof(SYNCML_CLIENT_THIS));
    if ( new == NULL ){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    srec.Attributes =  ServiceDescription;
    srec.NumAttributes = OI_ARRAYSIZE(ServiceDescription);
    srec.Strings = strings->attrs;
    srec.NumStrings = strings->num;
    
    target.data = (OI_BYTE *)uuid;
    target.len = OI_StrLen(uuid);

    /* Register with OBEX. */
    result = OI_OBEXSRV_RegisterServer(&target,
                                       CBList,
                                       authentication ? OI_OBEXSRV_AUTH_PASSWORD : OI_OBEXSRV_AUTH_NONE,
                                       &new->channel,
                                       NULL);
    if (!OI_SUCCESS(result)) {
        OI_Free(new);
        return OI_STATUS_INITIALIZATION_FAILED;
    }

    /* Register service record. */
    OI_SET_UINT_ELEMENT(ServerChannel, new->channel);
    result = OI_SDPDB_AddServiceRecord(&srec, &new->srec);

    if (OI_SUCCESS(result)) {
        OI_DBGPRINT2(("Registered SyncML client, RFCOMM channel: %d\n", new->channel));
        new->next = this;
        this = new;
        if ( channel != NULL ){
            *channel = new->channel;
        }
        OI_INIT_FLAG_INCREMENT(SYNCML);
    }else{
        OI_OBEXSRV_DeregisterServer(new->channel);
        OI_Free(new);
    }

    return result;
}

OI_STATUS OI_SyncML_DeregisterClient(OI_UINT8 channel)
{
    OI_INT                 count;
    SYNCML_CLIENT_THIS *   mine = this;
    SYNCML_CLIENT_THIS **  prior = &this;

    count = OI_INIT_FLAG_VALUE(SYNCML) & SYNCML_CLIENT_MASK;

    while (count--){
        if ( channel == mine->channel ){
            OI_SDPDB_RemoveServiceRecord(mine->srec);
            OI_OBEXSRV_DeregisterServer(mine->channel);
            *prior = mine->next;
            OI_Free(mine);
            OI_INIT_FLAG_DECREMENT(SYNCML);
            return OI_OK;
        }
        prior = &mine->next;
        mine = mine->next;
    }
    return OI_STATUS_NOT_REGISTERED;
}

OI_STATUS OI_SyncML_GetClientServiceRecord(OI_UINT8 channel, OI_UINT32 *handle)
{
    OI_INT count;
    SYNCML_CLIENT_THIS *mine = this;

    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(SYNCML);

    for (count = OI_INIT_FLAG_VALUE(SYNCML) & SYNCML_CLIENT_MASK; count; --count, mine = mine->next){
        if ( channel == mine->channel ){
            break;
        }
    }
    if ( !count ){
        return OI_STATUS_CHANNEL_NOT_FOUND;
    }
    *handle = mine->srec;
    return OI_OK;
}


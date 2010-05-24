/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_sdp.h#1 $ 

$Log: $
*
*    #4        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #3        23 Jul 2004            JH
* Moved to version 3.0.12 revision 1
*
===========================================================================*/
/** @file 
 * @internal 
 * Basic Imaging Profile service record API
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** \addtogroup BIP_Internal */
/**@{*/

#include "oi_sdpdb.h"

typedef struct {
    OI_DATAELEM protocolDescriptor[3];
    OI_DATAELEM rfcommDescriptor[2];
    OI_DATAELEM supportedCapabilities;
    OI_DATAELEM supportedFeatures;
    OI_DATAELEM supportedFunctions;
    OI_DATAELEM totalImagingCapacity;
    OI_DATAELEM serviceId;
    OI_UUID128 serviceUUID;
    
    OI_SDPDB_ATTRIBUTE attrs[7];
    OI_SDPDB_SERVICE_RECORD serviceRecord;
    OI_UINT32 serviceRecordHandle;
} OI_BIP_SDP_INFO;

OI_STATUS OI_BIPSRV_RegisterImagingResponderServiceRecord(OI_UINT8 channel,
                                                    OI_BIPSRV_IMAGING_RESPONDER_INFO *serviceInfo,
                                                    OI_BIP_SDP_INFO **sdpInfo,
                                                    const OI_SDP_STRINGS *strings);

OI_STATUS OI_BIPSRV_RegisterObjectsServerServiceRecord(OI_UINT8 channel,
                                                 OI_UINT16 uuid,
                                                 OI_BIPSRV_OBJECTS_SERVER_INFO *serviceInfo,
                                                 OI_BIP_SDP_INFO **sdpInfo,
                                                 const OI_SDP_STRINGS *strings);

OI_STATUS OI_BIPSRV_DeregisterServiceRecord(OI_BIP_SDP_INFO **sdpInfo);

/**@}*/

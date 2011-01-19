#ifndef _OI_SDP_UTILS_H
#define _OI_SDP_UTILS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_sdp_utils.h#1 $ 

$Log: $

===========================================================================*/
/**
  @file  

  utility functions for managing dynamic attributes in an SDP database
  
*/


/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_sdpdb.h"


OI_UINT32 OI_SDP_UTILS_LookupServiceRecord(OI_UUID32 serviceUUID,
                                           OI_UINT16 *enumerator);

/*
 * Initialize a protocol descriptor list attribute for a protocol built on
 * RFCOMM.
 */
OI_STATUS OI_SDP_UTILS_BuildPDLAttribute(OI_UUID32 upperProtocol,
                                         OI_UINT8 channelNumber,
                                         OI_SDPDB_ATTRIBUTE *pdlAttribute);

/*
 * Free memory allocate for data elements in an attribute list. Memory for the
 * attribute list itself must be freed by the caller.
 */
void OI_SDP_UTILS_FreeAttributeListElements(OI_SDPDB_ATTRIBUTE *attributeList,
                                            OI_UINT16 numAttributes);

/*****************************************************************************/
#endif /* _OI_SDP_UTILS_H */

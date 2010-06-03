#ifndef _OPPCOMMON_H
#define _OPPCOMMON_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_opp_common.h#1 $ 

$Log: $

===========================================================================*/
/**
@file  
Object Push Profile common definitions and functions
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_obexspec.h"


/**
 * struct for describing a generic OBEX object. The struct may contain a
 * complete object or partial object data.
 */
typedef struct {
    OI_OBEX_UNICODE name;     /**< Unicode name of the object */
    OI_OBEX_BYTESEQ type;     /**< NULL-terminated ASCII type for the object */
    OI_UINT32 objSize;        /**< total size of the object */
    OI_OBEX_BYTESEQ objData;  /**< object data */
} OI_OPP_GENERIC_OBJECT;

OI_STATUS OPPCommon_ParseHeaders(OI_OPP_GENERIC_OBJECT *obj,
                                 OI_OBEX_HEADER_LIST *headers);



#endif /* _OPPCOMMON_H */

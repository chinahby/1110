#ifndef AEEQOSFILTER_H
#define AEEQOSFILTER_H
/*===========================================================================

FILE: AEEQoSFilter.h

SERVICES: AEE Quality of Service Filter definitions


DESCRIPTION: Base level definitions, typedefs, etc. for AEE QoS Filter

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEECLSID_QOSFILTER.BID"
#include "AEEWebOpts.h"
#include "AEENet.h"

#include "../../inc/AEEIQoSFilter.h"

#define IQOSFILTER_AddRef           IQoSFilter_AddRef
#define IQOSFILTER_Release          IQoSFilter_Release
#define IQOSFILTER_QueryInterface   IQoSFilter_QueryInterface
#define IQOSFILTER_AddOpt           IQoSFilter_AddOpt
#define IQOSFILTER_RemoveOpt        IQoSFilter_RemoveOpt
#define IQOSFILTER_RemoveOptAtIndex IQoSFilter_RemoveOptAtIndex
#define IQOSFILTER_GetOpt           IQoSFilter_GetOpt
#define IQOSFILTER_GetOptAtIndex    IQoSFilter_GetOptAtIndex

#endif  // AEEQOSFILTER_H

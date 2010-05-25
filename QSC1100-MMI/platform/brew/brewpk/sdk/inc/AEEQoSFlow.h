#ifndef AEEQOSFLOW_H
#define AEEQOSFLOW_H
/*===========================================================================

FILE: AEEQoSFlow.h

SERVICES: AEE Quality of Service Flow definitions


DESCRIPTION: Base level definitions, typedefs, etc. for AEE QoS Flow

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEECLSID_QOSFLOW.BID"
#include "AEEWebOpts.h"
#include "../../inc/AEEIQoSFlow.h"

#define IQOSFLOW_AddRef                IQoSFlow_AddRef
#define IQOSFLOW_Release               IQoSFlow_Release
#define IQOSFLOW_QueryInterface        IQoSFlow_QueryInterface
#define IQOSFLOW_AddOpt                IQoSFlow_AddOpt
#define IQOSFLOW_RemoveOpt             IQoSFlow_RemoveOpt
#define IQOSFLOW_RemoveOptAtIndex      IQoSFlow_RemoveOptAtIndex
#define IQOSFLOW_GetOpt                IQoSFlow_GetOpt
#define IQOSFLOW_GetOptAtIndex         IQoSFlow_GetOptAtIndex

#endif  // AEEQOSFLOW_H


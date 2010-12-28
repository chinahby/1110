#ifndef AEEQOSBUNDLE_H
#define AEEQOSBUNDLE_H
/*=============================================================================

FILE:          AEEQoSBundle.h

SERVICES:      BREW QoS Bundle Interfaces.

DESCRIPTION:   Interface definitions, data structures, etc. for BREW 
               QOS Bundle Interface.

===============================================================================
        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
Include Files
-------------------------------------------------------------------*/
#include "AEE.h"  // for AEECallback
#include "AEEQoSList.h"
#include "AEEQoSSession.h"
#include "../../inc/AEEIQoSBundle.h"

#define AEEIID_QOSBUNDLE AEEIID_IQoSBundle

#define IQOSBUNDLE_AddRef           IQoSBundle_AddRef
#define IQOSBUNDLE_Release          IQoSBundle_Release
#define IQOSBUNDLE_QueryInterface   IQoSBundle_QueryInterface
#define IQOSBUNDLE_SelectNetworkEx  IQoSBundle_SelectNetworkEx
#define IQOSBUNDLE_Open             IQoSBundle_Open
#define IQOSBUNDLE_GoActive         IQoSBundle_GoActive
#define IQOSBUNDLE_Close            IQoSBundle_Close
#define IQOSBUNDLE_Deactivate       IQoSBundle_Deactivate
#define IQOSBUNDLE_AddRetryCB       IQoSBundle_AddRetryCB
#define IQOSBUNDLE_GetError         IQoSBundle_GetError
#define IQOSBUNDLE_OpenEx           IQoSBundle_OpenEx

#endif // AEEQOSBUNDLE_H

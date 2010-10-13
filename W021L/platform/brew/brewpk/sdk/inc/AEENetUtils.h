#ifndef AEENETUTILS_H
#define AEENETUTILS_H
/*===========================================================================

FILE: AEENetUtils.h

SERVICES:
   BREW Network Utilities Interface

DESCRIPTION:
	Interface definitions, data structures, etc. for BREW Network Utilities Interface

PUBLIC CLASSES:
   INetUtils

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None.

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEQueryInterface.h"

#include "../../inc/AEEINetUtils.h"

#define AEEIID_INETUTILS 	AEEIID_INetUtils

#define INETUTILS_AddRef             INetUtils_AddRef
#define INETUTILS_Release            INetUtils_Release
#define INETUTILS_QueryInterface     INetUtils_QueryInterface
#define INETUTILS_GetDefaultNetwork  INetUtils_GetDefaultNetwork
#define INETUTILS_GetPDPCount        INetUtils_GetPDPCount
#define INETUTILS_GetPDPInfo         INetUtils_GetPDPInfo
#define INETUTILS_ExtFunc            INetUtils_ExtFunc

#endif    // AEENETUTILS_H

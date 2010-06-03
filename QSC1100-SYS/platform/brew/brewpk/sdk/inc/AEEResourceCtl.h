#ifndef AEERESOURCECTL_H
#define AEERESOURCECTL_H

/*============================================================================
FILE: AEEResourceCtl.h

SERVICES:  BREW Resource Acess Control Services

DESCRIPTION:
   This file defines IResourceCtl interface that allows BREW applications to
   acquire and relinquish access control over resources.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEECLSID_RESPRIV_SOUND.BID"

#include "../../inc/AEEIResourceCtl.h"
#include "../../inc/AEEResCtlSound.bid"

#define AEECLSID_RESCTL_SOUND AEECLSID_ResCtlSound
#define AEEIID_RESOURCECTL AEEIID_IResourceCtl
#define AEECLSID_TOPVISIBLECTL   0x101e530   // IResourceCtl for Top Visible
#define AEECLSID_TOPVISIBLEPRIV  0x101e52f   // Top Visible SetRelinquishControl privilege
#define IRESOURCECTL_Acquire IResourceCtl_Acquire
#define IRESOURCECTL_AddRef IResourceCtl_AddRef
#define IRESOURCECTL_CanAcquire IResourceCtl_CanAcquire
#define IRESOURCECTL_Free IResourceCtl_Free
#define IRESOURCECTL_GetStatus IResourceCtl_GetStatus
#define IRESOURCECTL_OnStatusChangeNotify IResourceCtl_OnStatusChangeNotify
#define IRESOURCECTL_QueryInterface IResourceCtl_QueryInterface
#define IRESOURCECTL_SetRelinquishCtl IResourceCtl_SetRelinquishCtl
#define IRESOURCECTL_SetRelinquishCtlAll IResourceCtl_SetRelinquishCtlAll
#define IRESOURCECTL_SetRelinquishCtlNone IResourceCtl_SetRelinquishCtlNone
#define IRESOURCECTL_Release IResourceCtl_Release


#endif // AEERESOURCECTL_H

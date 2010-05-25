#ifndef AEEGROUPIDS_H
#define AEEGROUPIDS_H
/*======================================================
FILE:  AEEGROUPIDS.h

SERVICES:  Group IDs

GENERAL DESCRIPTION:
        This file lists the pre-defined GroupIDs in BREW

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEClassIDs.h"



// Predefined Group IDs

#define AEEGROUPID_ANY_SID       (AEECLSID_GROUPS)

#define AEEGROUPID_LOW_RAM       0x0101ccc1
#define AEEGROUPID_ALARM_MGR     0x0101e3f4
#define AEEGROUPID_REGISTRY_MGR  0x0101f16b
#define AEEGROUPID_MEMORY_STATS  0x0102c804
#define AEEGROUPID_ALARM_ALL    	0x0103012e
#define AEEGROUPID_RESERVECODE_LARGE  0x0103012f

#endif   // AEEGROUPIDS_H

/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================

AEEGROUPID_ANY_SID

Description:

This group ensures that the apps in this module will be allowed to run
even if the SID of the device has changed from the original SID that was used when the app was installed.
This is mainly applicable for RUIM-based phones.

===============================================================================

AEEGROUPID_LOW_RAM

Description:

This group is a required priviledge level for apps that are allowed to register
for the AEE_SCB_LOW_RAM_CRITICAL callback.  Like the AEE_SCB_LOW_RAM callback,
an application receives this callback when available RAM is low.  See
ISHELL_OnLowRAMCritical.

===============================================================================

AEEGROUPID_ALARM_MGR

Description:

This group is a required priviledge level for apps that are allowed to suspend
alarm services via ISHELL_SuspendAlarms().

===============================================================================

AEEGROUPID_REGISTRY_MGR

Description:

This group is a required priviledge level for apps that are allowed to modify
the BREW registry at runtime via ISHELL_RegisterHandler() or 
IREGISTRY_SetHandler().

===============================================================================

AEEGROUPID_MEMORY_STATS

Description:

This group is a required priviledge level for apps that are allowed to query
for memory usage stats for other apps and also for system level memory usage via 
IHeap_GetModuleMemStats().
===============================================================================

AEEGROUPID_ALARM_ALL

Description:

This group is a required priviledge level for apps that are allowed to set, query,
list or cancel alarms for other apps.
===============================================================================

AEEGROUPID_RESERVECODE_LARGE

Description:

This group is a required priviledge level for apps that are allowed to reserve
more than 30 alarm user codes.
=============================================================================*/


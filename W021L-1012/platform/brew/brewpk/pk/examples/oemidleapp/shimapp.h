#ifndef _SHIM_APPLET_HEADER_ // {
#define _SHIM_APPLET_HEADER_
/*============================================================================

FILE:  shimapp.h

SERVICES:  
        Contains common definitions for shim design.

GENERAL DESCRIPTION:
        Common definitions and services for the BREW shim applets, the native UI
        state machine to use, and the native UI applets themselves.

REVISION HISTORY: 

============================================================================

               Copyright © 2004 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

#include "uistate.h"

#include "AEE_OEM.h"
#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEModGen.h"

// TODO: Define the number of UI Applets that are shimmed here
// This is an example that uses 3, modify according to your progress
#define  NUMBER_UI_APPLETS    (3)

// TODO: Modify if your event handler is a different signature from this
typedef ui_maj_type (*PFNUIEVENTHANDLER)(void);

// This is the lookup function to obtain a function pointer to the 
// native event handler, this implementation reference is contained in oemtransientapp.c
PFNUIEVENTHANDLER CShim_GetEventHandler(AEECLSID clsApp);

// TODO: Add the applets to shim here
// IMPORTANT!!! Idle app must be first OEM
// These must match the CLSID in the corresponding MIF files!
// These are examples only, please replace with the real files you are editing.
#define AEECLSID_IDLEAPP    (AEECLSID_OEM_APP)
#define AEECLSID_CALCAPP    (AEECLSID_OEM_APP+1)
#define AEECLSID_MAINMENU   (AEECLSID_OEM_APP+2)
// ...

// TODO: Modify values as appropriate by the applets you have added class ID for above
#define AEECLSID_FIRSTSHIM  (AEECLSID_IDLEAPP)
#define AEECLSID_LASTSHIM   (AEECLSID_MAINMENU)

// User events to control applet lifetime management
#define EVT_CLOSE_SELF     (EVT_USER+0x100)
#define EVT_LAUNCH_APP     (EVT_USER+0x101)

#endif // } _SHIM_APPLET_HEADER_
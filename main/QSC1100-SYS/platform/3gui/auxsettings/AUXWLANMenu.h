#ifndef AUXWLANMENU_H
#define AUXWLANMENU_H
/*===========================================================================

FILE: AUXWLANMenu.h

DESCRIPTION
   This file is the header file for AUXWLANMenu.c.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by AUXSettings.c and AUXMenu.h.

       Copyright © 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/07   jas     Refactoring for HDK builds
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings

===========================================================================*/

/*===========================================================================

                    INCLUDES AND VARIABLE DEFINITIONS

===========================================================================*/
#include "AUXSettings.h"
#include "AUXMenu.h"
#include "AUXWLANUtils.h"

#include "AEEStdLib.h"
#include "AEEText.h"


#define AUX_WLAN_CHANNEL_ANY   0

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/

typedef struct
{
  uint16           nDlgId;
  uint16           nSKId;           /* Soft key menu ID */
  uint16           nSKOKId;         /* "OK" button ID on soft key menu */
  uint16           nTextId;         /* Text input ID */
  int32            nWIFIOptId;      /* ID of the option being edited */
  uint16           nTextFieldSz;    /* Text field size for the option (add 1 for buffer size) */
  AEETextInputMode TextInputMode;   /* Input type, i.e. numbers/ letters */
  boolean          bWIFICommit;     /* Whether to commit to IWIFI after edit dlg */
  boolean          bIsWLANSec;      /* Whether field is a WLAN security field */
  uint16           nReturnDlgId;    /* Return dialog ID when editing complete  */
} WLANEditDlgDataType;


/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================
FUNCTION AUXRefreshWLANSettingsMenu

DESCRIPTION
  refresh WLAN Settings menu display after menu items change
  due to actions of other menus

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRefreshWLANSettingsMenu(void *pUser, void *pICurrMenu);

/*===========================================================================
FUNCTION AUXDynamicMenuDlgEventHandler

DESCRIPTION
  Handle generic dynamic menu dlg events where menu items can change
  due to actions of other menus

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXDynamicMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXWLANNetSelModeDlgEventHandler

DESCRIPTION
  Handle events for the WLAN network select mode dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean AUXWLANNetSelModeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXWLANOptEditDlgHandler

DESCRIPTION
  Handle WLAN option editing dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANOptEditDlgHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXLanDlgEventHandler

DESCRIPTION
  handle LAN radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXLanDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXConfigIpDlgEventHandler

DESCRIPTION
  handle events in the configure static IP dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXConfigIpDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXEditIpDlgEventHandler

DESCRIPTION
  handle events in the edit IP set of dialogs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXEditIpDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXApp_WLANStatusEventHandler

DESCRIPTION
  handle events in the WLAN status dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXApp_WLANStatusEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXApp_WLANSelectEventHandler

DESCRIPTION
  handle events in the Network Select dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXApp_WLANSelectEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXWLANModeDlgEventHandler

DESCRIPTION
  Handle events for the WLAN mode dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANModeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXWLANSecDlgEventHandler

DESCRIPTION
  Handles events for the WLAN Security dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANSecDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXWLANSecEditDlgHandler

DESCRIPTION
  Handle WLAN authentication settings dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANSecEditDlgHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*==========================================================================
FUNCTION AUXWLANAutoScanDlgEventHandler

DESCRIPTION
  Handle events in the WLAN Autoscan dialog, displays Searching message, sets a
  timer for Auto Scan association, pops dialog back to WLAN settings on end

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANAutoScanDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*==========================================================================
FUNCTION AUXWLANNoAssociationDlgEventHandler

DESCRIPTION
  handle events in the WLAN No Association Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANNoAssociationDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXGetWLANEditDlgTbl

DESCRIPTION
  The function is used to return WLANEditDlgTbl based on wDlgID
DEPENDENCIES
  
SIDE EFFECTS
  None
===========================================================================*/
const WLANEditDlgDataType* AUXGetWLANEditDlgTbl(uint16 wDlgID);

/*===========================================================================
FUNCTION WLANAutoScanConnectFailed

DESCRIPTION
  Timer call back for WLAN auto scan connect failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WLANAutoScanConnectFailed(AUXApp *pMe );

#endif //AUXWLANMENU_H





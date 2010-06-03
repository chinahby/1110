#ifndef AUXMENU_H
#define AUXMENU_H
/*===========================================================================

FILE: AUXMenu.h

DESCRIPTION
   This file is the header file for AUXMenu.c.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by AUXSettings.c.

       Copyright © 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sg      Support for setting the BREW Secure clock
09/24/07   snb     Provide option to exit from USB MTP mode.
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/11/06   RT      Ported support for FEATURE_DATA_MULTI_PORTS
10/31/06   RT      Initial Revision
===========================================================================*/

/*===========================================================================

                               INCLUDES

===========================================================================*/
#include "AUXSettings.h"

/*===========================================================================
FUNCTION AUXMenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXMenuDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);


#if defined (FEATURE_RUNTIME_DEVMAP)
/*===========================================================================
FUNCTION AUXPortMapDlgEventHandler

DESCRIPTION
  handle portmap menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXPortMapDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

#ifdef FEATURE_DATA_MULTI_PORTS
/*===========================================================================
FUNCTION AUXRunTimeMultiPortMapDlgEventHandler

DESCRIPTION
  handle runtime multi portmap menu dlg events

DEPENDENCIES
  InitMultiPortRuntimePortMapDlg, rdm_assign_port

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRunTimeMultiPortMapDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

#else /* ifdef FEATURE_DATA_MULTI_PORTS */

/*===========================================================================
FUNCTION AUXRunTimePortMapDlgEventHandler

DESCRIPTION
  handle runtime portmap menu dlg events

DEPENDENCIES
  InitRuntimePortMapDlg, UpdateRadiobuttonImages, rdm_assign_port

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRunTimePortMapDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif /* ifdef FEATURE_DATA_MULTI_PORTS */
#endif /* ifdef FEATURE_RUNTIME_DEVMAP */



/*===========================================================================
FUNCTION AUXBaudDlgEventHandler

DESCRIPTION
  handle DS and Data baud menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXBaudDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXMessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXMessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*=============================================================================
FUNCTION:  AUXInitMenuIcons

DESCRIPTION: Adds non-selected icons to every menu item in the
             specified menu.

PARAMETERS:
   *pm:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AUXInitMenuIcons(IMenuCtl *pm);

/*=============================================================================
FUNCTION:  AUXSetMenuIcon

DESCRIPTION:

PARAMETERS:
   *pm:
   listId:
   bSel:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AUXSetMenuIcon(IMenuCtl *pm, uint16 listId, boolean bSel);

/*===========================================================================

FUNCTION AUXSetDefaultMenuLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void AUXSetDefaultMenuLook( IShell *ps, IMenuCtl *pm);

#if defined(FEATURE_WLAN) || \
    (defined(FEATURE_RUNTIME_DEVMAP) && defined(FEATURE_USB_DEFAULT_MTP))
/*===========================================================================

FUNCTION AUXSetDefaultSoftkeyLook

DESCRIPTION

PROTOTYPE:


PARAMETERS:

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
void AUXSetDefaultSoftkeyLook(IShell *ps, IMenuCtl *pm);

/*===========================================================================
FUNCTION AUXGetDlgMenuCtlID

DESCRIPTION
  This function matches the dialog ID with a corresponding menu control ID

DEPENDENCIES
  Note: 1. This function should be called in a Menu dialog ONLY
        2. the pIActiveDialog should be test for null pointer in the calling
           function

SIDE EFFECTS
  None
===========================================================================*/
uint16 AUXGetDlgMenuCtlID(AUXApp *pMe, IDialog* pIActiveDialog);

/*===========================================================================
FUNCTION AUXUpdateSetting

DESCRIPTION
  Update menu setting static variable and NV values if the selected Item is
  NV related.
  Based on whether the change of setting took place, update the display of
  radio buttons.

DEPENDENCIES
  AUXReadSetting, AUXInitMenuIcons, AUXSetMenuIcon

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXUpdateSetting(AUXApp *pMe, uint16 wSelectedItemID,
                         IMenuCtl* pICurrentMenu);

/*===========================================================================
FUNCTION AUXStripFeatureMenuItem

DESCRIPTION
  update the menu to reflect the correspondings under the FEATURE flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void AUXStripFeatureMenuItem(AUXApp *pMe, IMenuCtl* pICurrentMenu, uint16 wCtrlID);


/*===========================================================================
FUNCTION AUXCreateMenuSelectedItemDlg

DESCRIPTION
  It obtains the current menu selection and create the next level dialog
  accordingly

DEPENDENCIES
  AUXGetDlgMenuCtlID, AUXGetNextSeletedItemDlgID

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXCreateMenuSelectedItemDlg(AUXApp *pMe);

/*===========================================================================
FUNCTION AUXRadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRadioButtonDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION AUXInitRadioButtonDlg

DESCRIPTION
  This function update the radio button menu item list based on featurization,
  update the radio button for the selected item based on NV value

DEPENDENCIES
  AUXGetDlgMenuCtlID, AUXStripFeatureMenuItem,
  AUXSetMenuIcon, AUXSetDefaultMenuLook

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXInitRadioButtonDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog);
#endif /* FEATURE_WLAN || \
          (FEATURE_RUNTIME_DEVMAP && FEATURE_USB_DEFAULT_MTP */

/*===========================================================================
FUNCTION AUXStaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  AUXInitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXStaticDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*=============================================================================
FUNCTION:  AUXGetResourcePath

DESCRIPTION: Prepares the appropriate path using canonical form depending on
 the version of BREW

PARAMETERS:
  None

RETURN VALUE:
   Valid char * if sucessful. 
   NULL: If unsucessful

COMMENTS:
  The pointer returned needs to be FREE'ed by the caller.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
char * AUXGetResourcePath(void);

/*===========================================================================
FUNCTION AUXTimeDateDlgEventHandler

DESCRIPTION
  time date edit dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXTimeDateDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);
#endif //AUXMENU_H



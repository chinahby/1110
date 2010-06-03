#ifndef NETMENU_H
#define NETMENU_H
/*===========================================================================

FILE: NetMenu.h

DESCRIPTION
   This file is the header file for NetMenu.c.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by NetApp.c.

            Copyright © 2006 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetMenu.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/06   jas     Complete the renaming of symbols formerly in CoreApp
06/18/06   joe     Initial revision
===========================================================================*/

/*===========================================================================

                               INCLUDES

===========================================================================*/
#include "NetSettings.h"
#include "NetUtils.h"

/*-------------------------------------------------------------------

                  Exported function prototypes

-------------------------------------------------------------------*/

/*===========================================================================
FUNCTION NetMenuDlgEventHandler

DESCRIPTION
  handle generic menu dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetMenuDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION NetRadioButtonDlgEventHandler

DESCRIPTION
  Handle radio button event

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetRadioButtonDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION CheckBoxDlgEventHandler

DESCRIPTION
  Handle Check Box Dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetCheckBoxDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION NetStaticDlgEventHandler

DESCRIPTION
  handle static dlg events

DEPENDENCIES
  InitStaticDlg, CreateTextEditDlg,

SIDE EFFECTS
  None
===========================================================================*/
boolean NetStaticDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*===========================================================================
FUNCTION NetMessageDlgEventHandler

DESCRIPTION
  handle message dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetMessageDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

/*-------------------------------------------------------------------

                  Exported function prototypes

-------------------------------------------------------------------*/

/*===========================================================================
FUNCTION WriteNetSetting

DESCRIPTION
  Save the setting of given item through Set methods.
  Return TRUE if the setting was changed and FALSE otherwise.

DEPENDENCIES
  Various SetXXXSetting functions

SIDE EFFECTS
  None
===========================================================================*/
boolean WriteNetSetting(CNetApp *pMe, uint16 wDlgID, uint16 wItemID);


#endif /* NETMENU_H */


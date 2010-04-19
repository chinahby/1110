#ifndef DIALERDLGMANAGER_H
#define DIALERDLGMANAGER_H

/*=============================================================================

FILE: DialerDlgManager.h

SERVICES: Dialer Applet 

GENERAL DESCRIPTION: Manages what dialogs can be displayed based on events

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDlgManager.h#3 $
$DateTime: 2007/08/24 13:42:18 $
$Author: carls $
$Change: 526599 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/22/07   cvs     Add framework for dialogs
08/14/03   BW      Integration with both ICM and ISuppSvc
07/25/03   BW      Created DialerDlgManager Header File 

=============================================================================*/

/*===========================================================================

FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION: CDialerApp_DlgManager

DESCRIPTION: Dialer Dialog Manager (based on user and notify events) 

PARAMETERS:
  *pMe: CDialerApp object pointer
  eCode: Event code
  wParam: word event parameter
  dwParam: dword event parameter

RETURN VALUE:
  boolean: TRUE if the event was handled 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_DlgManager(void *pUser, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*=============================================================================
FUNCTION: CDialerApp_GenericEventHandler

DESCRIPTION: Framework based dialog handler

PARAMETERS:
  *pUser: CDialerApp object pointer
  evt: event code
  w: word event param
  dw: dword event param

RETURN VALUE:
  boolean: Returns TRUE if event was handled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CDialerApp_GenericEventHandler(void *pUser,
                                       AEEEvent evt,
                                       uint16 w,
                                       uint32 dw);


#endif // DIALERDLGMANAGER_H

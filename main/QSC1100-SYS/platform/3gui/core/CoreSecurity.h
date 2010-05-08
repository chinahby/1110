#ifndef CORESECURITY_H
#define CORESECURITY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         C O R E   S E C U R I T Y

GENERAL DESCRIPTION

This is the header file for the Security Module of the Core application

Security_init () can be initiated in two ways:

1.  At boot, by the CORE-UI initialization routine after displaying the initial
    splash screen.
2.  Any time the CORE-UI event handler receives an ICARD_CHANGED event.


The graphical process flow is described in the "Power Up" section of the
Core App User Interface Specification document.  This document describes
the CORE-UI interaction with the 3G UI Security Module and the Security
Module interaction with the ISHELL_IDisplay and ICard interfaces

               Copyright (c) 2002-2006 by QUALCOMM INCORPORATED.
                           All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreSecurity.h#24 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/06   jas     Adding an extern to fix a compiler warning
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
12/13/04   ssc     Changed include file name case to match the Linux names 
                   of the files. Mismatches in the case break AMSS builds 
                   on Linux servers.
11/12/04   ank,cvs Dualslot changes from 6300
06/28/04   jas     Changes to support dual-display targets
06/07/04   tml     Added personalization support
01/29/04   tml     Added NEW_ICARD feature
12/22/03   tml     ICARD API changes
08/18/03   tml     Added sim initializing screen
08/06/03   tml     Dual Slot support and clean up
07/15/03   tml     Security clean up
05/14/03   tml     Code cleanup
05/03/03   tml     Externed function for NV code verification
04/02/03   kar     Code cleanup
10/28/02   tml     RTRE Security Changes
10/03/02   kar     Fixed compile warnings
10/02/02   kar     Code cleanup
08/23/02   tml     Removed EVT_ICARD_CHANGED event
08/21/02  js/tml   Support for ICARD Integration
08/13/02   kar     Core security header
===========================================================================*/

/*-------------------------------------------------------------------
Include files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEEStdLib.h"        // contain FREE function
#include "AEEAppGen.h"        // Applet helper file

#include "CoreApp_res.h"      // Core Menu Resource defines
#include "CoreApp_qvga_res.h"  // Core Menu Resource defines
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEEDB.h"            // AEE DataBase Services'
#include "AEEText.h"
#include "AEECard.h"          // ICard Services

#include "CoreApp.h"
#include "CoreUtils.h"
#include "CoreMenu.h"

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
/*===========================================================================
FUNCTION NoCardDlgEventHandler

DESCRIPTION
  This event handler will process key presses for the NO CARD and
  INVALID CARD IDialogs.  This will process both digits followed by
  the SEND key and DOWN-ARROW/SELECT to process the Emergency Call
  Soft Key.

DEPENDENCIES
  Security_init must successfully register this event handler.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean NoCardDlgEventHandler (void *pUser, AEEEvent evt,
                                             uint16 w, uint32 dw);

/*===========================================================================
FUNCTION PinVerificationEventHandler

DESCRIPTION
 This event handler will process key presses for the verify PIN1/PIN2/NV,
 enable PIN1/PIN2/NV and disable PIn1/PIN2/NV Dialogs.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean PinVerificationEventHandler (void *pUser, AEEEvent evt,
                                       uint16 w, uint32 dw);
/*===========================================================================
FUNCTION NewPinVerificationEventHandler

DESCRIPTION
  This event handler will process key presses for the NEWPIN
  IDialogs.  This will process digits followed by the SEND key.

DEPENDENCIES
  Security_init must successfully register this event handler.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean NewPinVerificationEventHandler (void *pUser, AEEEvent evt,
                                          uint16 w, uint32 dw);

/*===========================================================================
FUNCTION OldPinVerificationEventHandler

DESCRIPTION
 This event handler will process key presses for the OLD_PIN_REQUEST
 IDialog.  This will process digits followed by the SEND key.

DEPENDENCIES
  ChangePIN must successfully register this event handler.


RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean OldPinVerificationEventHandler (void *pUser, AEEEvent evt,
                                           uint16 w, uint32 dw);


/*===========================================================================
FUNCTION VerifyNVLockCode

DESCRIPTION
  Get NV Lock code and verify it with the input parameter

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean VerifyNVLockCode(AECHAR* psCode);

/*===========================================================================
FUNCTION VerifyNVCode

DESCRIPTION
  Get NV Lock code and verify it with the input parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern int VerifyNVCode(CCoreApp * pMe);

/*===========================================================================
FUNCTION GetNextPINProcessDlgID

DESCRIPTION
  This function returns the next PIN dialog ID in the pin verification,
  enabling, disabling and changing process.  


DEPENDENCIES
  None.

RETURN VALUE
  next dialog ID

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 GetNextPINProcessDlgID (CCoreApp *pMe, uint8 byPinId, uint16 wCurrentDlgID);



/*===========================================================================
FUNCTION DetermineSecurityMask

DESCRIPTION
  This function based on the card status from ICard and the configuration and
  control information to determine the security state mask
  NV_CODE_MASK                  
  PIN1_CODE_MASK                
  PIN1_SLOT2_CODE_MASK          
  EMERGENCY_MASK                
  NV_PIN1_MASK                  
  NV_PIN1_SLOT2_MASK            
  PIN1_PIN1_SLOT2_MASK          
  NV_PIN1_PIN1_SLOT2_MASK      
  NOT_READY_MASK 

DEPENDENCIES
  None.

RETURN VALUE
  uint32: 
  NV_CODE_MASK                  
  PIN1_CODE_MASK                
  PIN1_SLOT2_CODE_MASK          
  EMERGENCY_MASK                
  NV_PIN1_MASK                  
  NV_PIN1_SLOT2_MASK            
  PIN1_PIN1_SLOT2_MASK          
  NV_PIN1_PIN1_SLOT2_MASK      
  NOT_READY_MASK 

SIDE EFFECTS
  None.
===========================================================================*/
uint32 DetermineSecurityMask(CCoreApp *pMe);


/*===========================================================================
FUNCTION GetNVLockCodeStatus

DESCRIPTION
  This function set the passed in parameter based on whether the NV lock
  code has been enabled or disabled after reading from the NV item

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void GetNVLockCodeStatus(AEECardPinStatus * nv_code) ;

/*===========================================================================
FUNCTION StartPINVerificationProcess

DESCRIPTION
  This function based on the PIN lock state to see if any PIN verification
  screen dialog should be displayed or if a success/failure events should
  be fired off based on the lock state

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int StartPINVerificationProcess(CCoreApp *pMe, uint8 byPinID);

/*===========================================================================
FUNCTION CoreSecurity_EnablePIN

DESCRIPTION

  This function sets the callback function pointer for enable pin and
  call the corresponding ICard_EnablePin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
int CoreSecurity_EnablePIN(CCoreApp * pMe, uint8 byPinID);

/*===========================================================================
FUNCTION CoreSecurity_DisablePIN

DESCRIPTION

  This function sets the callback function pointer for disable pin and
  call the corresponding ICard_DisablePin function

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/EFAILED/EUNSUPPORT

SIDE EFFECTS
  None.
===========================================================================*/
int CoreSecurity_DisablePIN(CCoreApp * pMe, uint8 byPinID);

/*===========================================================================
FUNCTION SimInitializingDlgEventHandler

DESCRIPTION
  handle sim initializing static dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean SimInitializingDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw);


/*===========================================================================
FUNCTION EnterPersoKeyDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the deactivating 
 Personalization feature.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean EnterPersoKeyDlgEventHandler (void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION PersoIndicationDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the showing perso feature
 indication level

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean PersoIndicationDlgEventHandler(void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam);

/*===========================================================================
FUNCTION EnterPersoDataDlgEventHandler

DESCRIPTION
 This event handler will process key presses for the entering personalization
 data

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean EnterPersoDataDlgEventHandler(void *pUser, AEEEvent evt,
                 uint16 wParam, uint32 dwParam);


/*===========================================================================
FUNCTION CoreSecurity_IsPinVerified

DESCRIPTION
  Checks if the PIN is verified for a particular card or NV

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreSecurity_IsPinVerified(CCoreApp *pMe, uint8 byPinID);

/*===========================================================================
FUNCTION CoreSecurity_AllCardEventsReceived

DESCRIPTION
  check if the security mask for any card is not ready, if so, return FALSE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreSecurity_AllCardEventsReceived (CCoreApp * pMe );


#endif // CoreSecurity.h

#ifndef OEMKEYGUARD_H
#define OEMKEYGUARD_H

/*=============================================================================

FILE: OEMKeyguard.h
 
SERVICES:  OEM Keyguard Interface
 
GENERAL DESCRIPTION:
   The OEM Keyguard interface provides a keyguard functionality to the UI.

   The keyguard can be enabled/disabled by BREW applets by setting the 
   CFGI_KEYGUARD_ENABLED config item with the ICONFIG interface.  The current
   keyguard state may also be queried through the ICONFIG interface.

   Before the OEM layer sends a key to BREW (see OEMUITask.c) it passes the
   key to OEMKeyguard_HandleEvent().  If this function returns TRUE, the key
   is discarded and not sent to BREW.  This allows the keyguard to intercept
   key events while it is enabled.

   The keyguard also supports an autoguard that can enable the keyguard
   after a timeout period, regardless of what BREW applet is curently 
   active.  The autoguard will only enable while the phone is in the
   Idle call state and in the Online operating mode.

   While the keyguard is enabled, BREW will continue to run and function
   normally.  This can be seen quite nicely by starting the Stopwatch timer
   and waiting for the autoguard to activate.  The Stopwatch timer will
   continue counting.  
   
   When a key is pressed with the keyguard enabled, the following steps occur:

      1)  The active applet is suspended
      2)  The keyguard message is displayed.
      3a) If no further key is pressed, the message will timeout and resume
          the active applet with the keyguard enabled.
      3b) The 123 unlock code is pressed, the active applet will be resumed
          with the keyguard disabled.

    The reason the keyguard was implemented in the OEM layer instead of
    as a BREW applet was due to syncronization issues with 
    ISHELL_StartApplet().  ISHELL_StartApplet() does not start the specific
    applet immediately.  Instead it puts the applet into a 'start queue' 
    which is then started at a later time when it is convenient for BREW.  
    However when a key is pressed with keyguard enabled, the BREW keyguard 
    applet would need to be started immmediately so the OEM layer could 
    pass the key to it (otherwise the OEM layer would need to maintain a 
    key event queue).

PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/TSGBREWOEM/inc/OEMKeyguard.h#1 $
$DateTime: 2002/08/27 16:24:00 $
  $Author: kjmckay $
  $Change: 56683 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/01/02   mjv     Added file header comment
07/25/02   mjv     Ported to BREW 2.0 
07/17/02   mjv     Initial revision
 
=============================================================================*/


/*===========================================================================

                    INCLUDE FILES

===========================================================================*/


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/


/*=============================================================================
FUNCTION: OEMKeyguard_Init

DESCRIPTION:  Called to initialize the OEM Keyguard 

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMKeyguard_Init(void *pShell,void *pPhone,void *pAlert,void *ann);


/*=============================================================================
FUNCTION: OEMKeyguard_HandleEvent

DESCRIPTION:  This function should be called before sending a key event
              to BREW. 

PARAMETERS:
   evt: AEEEvent 
   wParam: 16-bit event parameter (dwParam is assumed to be zero)

RETURN VALUE:
   boolean: TRUE if the event was handled by the Keyguard and does not
            need to be forwarded to BREW

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMKeyguard_HandleEvent(AEEEvent  evt, 
                                uint16    wParam); 


/*=============================================================================
FUNCTION: OEMKeyguard_IsEnabled

DESCRIPTION:  Query if the keyguard is currently enabled.

PARAMETERS:
   None

RETURN VALUE:
   boolean:  TRUE if the keyguard is currently enabled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMKeyguard_IsEnabled(void);


/*=============================================================================
FUNCTION: OEMKeyguard_SetState

DESCRIPTION:  Enable or disable the keyguard.

PARAMETERS:
   bEnabled [in]:  TRUE if the keyguard should be enabled.
                   FALSE if the keyguard should be disabled.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMKeyguard_SetState(boolean bEnabled);

#endif /* OEMKEYGUARD_H */

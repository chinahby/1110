#ifndef __SSMANAGEMENT_DIALOG_HANDLER_H__
#define __SSMANAGEMENT_DIALOG_HANDLER_H__

/*=============================================================================
FILE: SSManagement_DialogHandler.h

SERVICES: GSM1x SS Management Dialog Handlers

GENERAL DESCRIPTION:
      SS Management dialog handlers for the UI (User interface).

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: DialogHandler_HandleEvent

DESCRIPTION: handle a BREW event by the dialog handler

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   eCode [in]: Specifies the Event sent to this applet
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
boolean DialogHandler_HandleEvent(
   struct _CGSM1xSSManagementApp *pMe,
   AEEEvent                      eCode,
   uint16                        wParam,
   uint32                        dwParam
);

/*=============================================================================
FUNCTION: DialogHandler_Initialize

DESCRIPTION: Initialize UI part.

PARAMETERS:
    pMe [in]: Pointer to the Applet structure. 

RETURN VALUE:
   None

=============================================================================*/
void DialogHandler_Initialize(struct _CGSM1xSSManagementApp *pMe);

/*=============================================================================
FUNCTION: DialogHandler_Destroy

DESCRIPTION: Destroy UI part.

PARAMETERS:
    pMe [in]: Pointer to the Applet structure. 

RETURN VALUE:
   None

=============================================================================*/
void DialogHandler_Destroy(struct _CGSM1xSSManagementApp *pMe);


#endif // end __SSMANAGEMENT_DIALOG_HANDLER_H__


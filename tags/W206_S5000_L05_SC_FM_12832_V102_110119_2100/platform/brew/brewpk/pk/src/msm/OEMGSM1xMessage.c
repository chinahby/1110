/*======================================================     
FILE:  OEMGSM1xMessage

SERVICES:  OEM GSM1x Messaging interface

GENERAL DESCRIPTION:
   Used by GSM1x intefaces to access SMS routines.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREM/ENTS:


        Copyright © 2003-2005 by QUALCOMM, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "OEMFeatures.h"

/* The following ifdef affects the whole file.*/                          
#if defined(FEATURE_GSM1x)

#include "OEMGSM1xMessage_SVC.h"
#include "AEE_OEMDispatch.h"

#include "assert.h"


/*===========================================================================

                     DEFINES

===========================================================================*/
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/

typedef enum {

   GSM1xMESSAGE_MSG_SIG,
   GSM1xMESSAGE_MSG_SMS

} OEMGSM1xMessage_MsgType;


/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                     LOCAL/STATIC DATA

===========================================================================*/

// callback data for app shutdown notifications.
//
// This data must be able to be read and written in user mode, as app shutdown
// notifications can currently only be scheduled in user mode.
static AEECallback gpShutDownCallbacks[MAX_MOSMS_OUTSTANDING];

/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
Function: OEMGSM1xMessage_Init
===========================================================================*/
void OEMGSM1xMessage_Init() 
{
   OEMGSM1xMessage_SVC_Init();
}


/*=============================================================================
Function: OEMGSM1xMessage_ReleaseSMSTransaction
===========================================================================*/
void OEMGSM1xMessage_ReleaseSMSTransaction()
{
   OEMGSM1xMessage_SVC_ReleaseSMSTransaction();
}


/*=============================================================================
Function: OEMGSM1xMessage_SendSIGMessage
===========================================================================*/
int OEMGSM1xMessage_SendSIGMessage (byte                     bearerDataLen,
                                    const byte             * bearerData,
                                    boolean                  startTransaction)
{
   return OEMGSM1xMessage_SVC_SendSIGMessage(
      bearerDataLen, bearerData, startTransaction
      );
}


/*=============================================================================
Function: OEMGSM1xMessage_SendSMSMessage
===========================================================================*/
int OEMGSM1xMessage_SendSMSMessage (boolean                  sendStatusCallback,
                                    void                   * pUserData,
                                    byte                     bearerDataLen,
                                    const byte             * bearerData,
                                    boolean                  startTransaction)
{
   return OEMGSM1xMessage_SVC_SendSMSMessage(
      sendStatusCallback, pUserData, bearerDataLen,
      bearerData, startTransaction
      );
}


/*=============================================================================
FUNCTION: OEMGSM1xMessage_AppClosedNotification

DESCRIPTION:
   Called to handle the notification that an applet, that has an outstanding 
   message, has been closed.
        
PARAMETERS:
   smsEntryHandle [in] : message handle value passed as a void*

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xMessage_AppClosedNotification(void* smsEntryHandle)
{
   OEMGSM1xMessage_SVC_AppClosedNotification((uint32)smsEntryHandle);
}


/*=============================================================================
FUNCTION: OEMGSM1xMessage_ScheduleAppClosedCallback

DESCRIPTION:
   Notification that will be called when the current applet is closed.
        
PARAMETERS:
   handle [in] : message handle value, using one based indexing.

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xMessage_ScheduleAppClosedCallback(uint32 handle)
{
   // use handle-1 as handle values currently use one based indexing (see AEEHandle.c)
   // and gpShutDownCallbacks array value uses zero based indexing.
   handle -=1;

   ASSERT(handle < MAX_MOSMS_OUTSTANDING);

   CALLBACK_Init(
      &gpShutDownCallbacks[handle], 
      OEMGSM1xMessage_AppClosedNotification, 
      (void*)handle
      );

   AEE_RegisterSystemCallback(&gpShutDownCallbacks[handle], AEE_SCB_APP_CLOSED, 0);
}


/*=============================================================================
FUNCTION: OEMGSM1xMessage_CancelAppClosedCallback

DESCRIPTION:
   Notification that will be called when the current applet is closed.
        
PARAMETERS:
   handle [in] : message handle value, using one based indexing.

RETURN VALUE:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xMessage_CancelAppClosedCallback(uint32 handle)
{
   // use handle-1 as handle values currently use one based indexing (see AEEHandle.c)
   // and gpShutDownCallbacks array value uses zero based indexing.
   handle -=1;

   ASSERT(handle < MAX_MOSMS_OUTSTANDING);

   CALLBACK_Cancel( &gpShutDownCallbacks[handle] );
}


#endif  // ifdef FEATURE_GSM1x


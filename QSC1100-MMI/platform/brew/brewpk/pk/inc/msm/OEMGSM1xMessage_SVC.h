#ifndef OEMGSM1X_MESSAGE_SVC_H
#define OEMGSM1X_MESSAGE_SVC_H
/*===========================================================================

                           OEMGSM1xMessage_SVC.h 


GENERAL DESCRIPTION
   OEM GSM1x Messaging Interface header.

EXTERNALIZED FUNCTIONS
   OEMGSM1xMessage_SVC_Init
   OEMGSM1xMessage_SVC_RerouteTapi
   OEMGSM1xMessage_SVC_SendSIGMessage
   OEMGSM1xMessage_SVC_SendSMSMessage
   OEMGSM1xMessage_SVC_ReleaseSMSTransaction
   OEMGSM1xMessage_SVC_ConfigListeners


INITIALIZATION AND SEQUENCING REQUIREMENTS
   OEMGSM1xMessage_SVC_Init needs to be called only once, and must be 
   called before using any of the OEMGSM1xMessage functions. 


Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "OEMFeatures.h"

#include "OEMGSM1xMessage.h"
#include "AEE_OEM.h"


#if defined(FEATURE_CDSMS)
     #define FEATURE_GSM1x_WMS 
#elif defined (FEATURE_GWSMS)
     #undef FEATURE_GSM1x_WMS 
     #error: We should not have FEATURE_GSM1x defined without FEATURE_CDSMS or uasms
#elif !defined(FEATURE_UASMS)
     #error Either WMS or UASMS must be defined.
#endif


#ifdef FEATURE_GSM1x_WMS
    #include "wms.h"
    #include "wmsmsg.h"
    #include "queue.h"
#else
    #include "uasms.h"
#endif


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

// Defines the maximum number of MOSMS that can be outstanding in the queue.
#define MAX_MOSMS_OUTSTANDING  10

/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

void OEMGSM1xMessage_SVC_Init(void);

// called directly from OEMTAPI_SendSMS which runs in Supervisor mode
int OEMGSM1xMessage_SVC_RerouteTapi (const char  * pszDst,
                                     const char  * pszMsg,
                                     AEECLSID      clsDst,
                                     uint32      * pdwStatus,
                                     AEECallback * pcb);

int OEMGSM1xMessage_SVC_SendSIGMessage (byte                     bearerDataLen,
                                        const byte             * bearerData,
                                        boolean                  startTransaction);

int OEMGSM1xMessage_SVC_SendSMSMessage (boolean                  sendStatusCallback,
                                        void                   * pUserData,
                                        byte                     bearerDataLen,
                                        const byte             * bearerData,
                                        boolean                  startTransaction);

void OEMGSM1xMessage_SVC_ReleaseSMSTransaction(void);

#if !defined(FEATURE_GSM1x_WMS)

// called directly from AEEPhone_InitSMS() which runs in Supervisor mode
void OEMGSM1xMessage_SVC_ConfigListeners(
       uasms_message_listener_type    message_listener_f,
       uasms_status_listener_type     status_listener_f,
       uasms_event_listener_type      event_listener_f);
#endif

void OEMGSM1xMessage_SVC_AppClosedNotification(uint32 smsEntryHandle);


#endif    // OEMGSM1X_MESSAGE_SVC_H
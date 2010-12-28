#ifndef OEMGSM1X_MESSAGE_H
#define OEMGSM1X_MESSAGE_H
/*===========================================================================

                           OEMGSM1xMessage.h 


GENERAL DESCRIPTION
   OEM GSM1x Messaging Interface header.

EXTERNALIZED FUNCTIONS
   OEMGSM1xMessage_Init
   OEMGSM1xMessage_SendSIGMessage
   OEMGSM1xMessage_SendSMSMessage
   OEMGSM1xMessage_ReleaseSMSTransaction
   OEMGSM1xMessage_GetSIGMessage


INITIALIZATION AND SEQUENCING REQUIREMENTS
   OEMGSM1xMessage_Init needs to be called only once, and must be 
   called before using any of the OEMGSM1xMessage functions. 

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE.h"
#include "AEEGSM1xSig.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

#define GSM1xMESSAGE_TELESERVICE_SIG  4104  /* GSM1X signalling */
#define GSM1xMESSAGE_TELESERVICE_SMS  4105  /* GSM1X SMS including page, and voicemail */

/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

typedef void (*PFNSMSSTATUS)(void * pUser, int status);

// OEMGSM1xSIG_SVC_ProcessMessage and OEMGSMSMS_SVC_ProcessMessage should have the 
// following signature.
// 
typedef void (*PFN_ProcessMessage) (const byte *bearerData, const uint16 length);


#define GSM1xMESSAGE_SIGNALING_MAX      223     // A GSM1x Msg with this length can
                                                // only be sent on Traffic channel.
                                                // L2 Fields          = 3 Bytes
                                                // DBM Hdr            = 4 Bytes 
                                                // SMS Type           = 1 Byte
                                                // Tele-Service TLV   = 4 Bytes
                                                // Addr TLV           = 5 Bytes
                                                // Bearer Data Hdr    = 2 Bytes
                                                // User sub-param Hdr = 2 Bytes

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

void OEMGSM1xMessage_Init(void);

int OEMGSM1xMessage_SendSIGMessage (byte                     bearerDataLen,
                                    const byte             * bearerData,
                                    boolean                  startTransaction);

int OEMGSM1xMessage_SendSMSMessage (boolean                  sendStatusCallback,
                                    void                   * pUserData,
                                    byte                     bearerDataLen,
                                    const byte             * bearerData,
                                    boolean                  startTransaction);

void OEMGSM1xMessage_ReleaseSMSTransaction(void);

boolean OEMGSM1xMessage_GetSIGMessage(AEEGSM1xSig_NotifyMessageType* message);


#endif    // OEMGSM1X_MESSAGE_H
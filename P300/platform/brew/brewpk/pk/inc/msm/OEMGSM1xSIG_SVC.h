#ifndef OEMGSM1X_SIG_SVC_H
#define OEMGSM1X_SIG_SVC_H
/*===========================================================================

                           OEMGSM1xSIG_SVC.h 


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEEGSM1xSig.h"
#include "OEMGSM1xMessage.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/


/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

void OEMGSM1xSig_SVC_Init(void);

boolean OEMGSM1xSig_SVC_GetSIGMessage(AEEGSM1xSig_NotifyMessageType* message);

int OEMGSM1xSig_SVC_SendSignalingSMS(const AEEGSM1xSig_SignalingMessageType* pSigMsg);

void OEMGSM1xSig_SVC_ProcessMessage(const byte *bearerData, const uint16 len);


#endif    // OEMGSM1X_SIG_SVC_H
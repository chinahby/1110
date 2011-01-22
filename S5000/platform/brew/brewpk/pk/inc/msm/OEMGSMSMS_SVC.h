#ifndef OEMGSMSMS_SVC_H
#define OEMGSMSMS_SVC_H
/*===========================================================================

                           OEMGSMSMS_SVC.h 


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEEGSMSMS.h"
#include "OEMGSM1xMessage.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

typedef byte GSMSMSMessage[GSM1xMESSAGE_SIGNALING_MAX];

/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

void OEMGSMSMS_SVC_Init(void);

boolean OEMGSMSMS_SVC_GetSMSMessage(GSMSMSMessage* message, byte* length);

void OEMGSMSMS_SVC_ProcessMessage(const byte *bearerData, const uint16 len);


#endif    // OEMGSMSMS_SVC_H
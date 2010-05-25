#ifndef GSM1XSMSMT_H
#define GSM1XSMSMT_H
/*===========================================================================

                            S M S M T . h      
                

GENERAL DESCRIPTION
   This file contains the prototypes for functions used to process Mobile 
   Terminated messages.

EXTERNALIZED FUNCTIONS
   GSM1xSMSMT_ProcessMTMsg
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEGSMSMS.h"
#include "GSM1xSMSMessageStore.h"

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

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSMT_MTMsgHandler

DESCRIPTION:
   Takes the data from an incoming message and converts to GSM1xSMSAppMessageStruct

PROTOTYPE:
   void GSM1xSMSMT_MTMsgHandler(SMSApp *pMe,GSMSMSRawMsg *rawMsg) 
				

PARAMETERS:
   pMe [in]: Pointer to the SMS applet structure

   rawMsg [in]: Pointer to the incoming raw message
   
RETURN VALUE:
   Returns TRUE if successful. FALSE otherwise.

COMMENTS:
 
SIDE EFFECTS:
   None
=============================================================================*/
boolean GSM1xSMSMT_MTMsgHandler(SMSApp *pMe, const GSMSMSRawMsg *rawMsg);

int GSMTime2Secs(GSMSMSTime *gsmtime);

#endif // #ifndef

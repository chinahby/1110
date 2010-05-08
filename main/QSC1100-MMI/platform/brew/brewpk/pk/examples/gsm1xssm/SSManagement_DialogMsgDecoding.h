#ifndef __SSMANAGEMENT_DIALOG_MSG_DECODING__
#define __SSMANAGEMENT_DIALOG_MSG_DECODING__

/*=============================================================================
FILE: SSManagement_DialogMsgDecoding.h

SERVICES: GSM1x SS Management Dialog Message Decoding

GENERAL DESCRIPTION:
      SS Management dialog message decoding and handling functionality

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "SSUtil.h"
#include "SSMsg.h" 

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/


// Decoded Message status
// ----------------------
#define SS_DECODING_APP_ERROR 0
#define SS_DECODING_MSG_INVALID 1
#define SS_DECODING_MSG_RESULT_SUCCESS 2
#define SS_DECODING_MSG_RESULT_REJECTED 3
#define SS_DECODING_MSG_RESULT_ERROR_CODE 4
#define SS_DECODING_MSG_RESULT_END_TRANSACTION 5
typedef int SSmsgDecodedResultStatus; /* like enum */


/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: DialogMsgDecoding_HandleResultMsg

DESCRIPTION: process and decode a result message

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   pMsg [in]: Specifies the message that needs to be handled
   pErrorCode [out]: error code value

RETURN VALUE:
   decoded message status

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
SSmsgDecodedResultStatus DialogMsgDecoding_HandleResultMsg(
    CGSM1xSSManagementApp *pMe,
    SSMsg_int_msg_type     *pMsg
    );


#endif // end __SSMANAGEMENT_DIALOG_MSG_DECODING__


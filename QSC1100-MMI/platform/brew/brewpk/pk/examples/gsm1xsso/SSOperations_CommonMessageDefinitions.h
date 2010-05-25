#ifndef __SSOPERATIONS_COMMON_MESSAGE_DEFINITIONS_H__
#define __SSOPERATIONS_COMMON_MESSAGE_DEFINITIONS_H__

/*=========================================================================
FILE: SSOperations_CommonMessageDefinitions.h

SERVICES: GSM1x SS signaling messages utilities

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSMsg.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOMessage_FreeMsg

DESCRIPTION: free the memory allocated to a message.

PARAMETERS:
   msg [in]: pointer to the message to be freed. if msg is NULL, it is ignored.

RETURN VALUE:
   None
=============================================================================*/
void SSOMessage_FreeMsg(
    SSMsg_int_msg_type *msg
    );

/*=============================================================================
FUNCTION: SSOMessage_AllocateMsg

DESCRIPTION: allocate the memory for an internal message structure.

PARAMETERS:
    msgType [in]: message type

RETURN VALUE:
    Pointer to a new SSMsg_int_msg_type structure or NULL on MALLOC failure.

COMMENTS:
    The new allocated message is initialized with the given msgType.
    When the message is no longer used, call SSOMessage_FreeMsg to release it.
=============================================================================*/
SSMsg_int_msg_type* SSOMessage_AllocateMsg(SSMsg_msg_type_type msgType);

/*=============================================================================
FUNCTION: SSOMessage_FillHeader

DESCRIPTION: fill the header of an internal GSM1x message

PARAMETERS:
    msg [out]: GSM1x internal message to fill its header
    TI  [in] : transaction id to use for setting the header

RETURN VALUE:
    None.

COMMENTS:
    pd, tiflag and tivalue are set for msg.
=============================================================================*/
void SSOMessage_FillHeader(
    SSMsg_int_msg_type *msg, 
    uint8 TI
    );

#endif  //__SS_OPERATIONS_COMMON_EVENT_DEFINITIONS_H__

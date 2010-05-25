/*=========================================================================
FILE: SSOperations_CommonMessageDefinitions.c

SERVICES: GSM1x SS signaling messages utilities

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_CommonMessageDefinitions.h"
#include "SS_CommonMacros.h"
#include "SSCallCommonDefinitions.h"

#include "AEEStdLib.h"        // AEE StdLib Services

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                        LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOMessage_FreeMsg

DESCRIPTION: free the memory allocated to a message.

PARAMETERS:
   msg [in]: pointer to the message to be freed. if msg is NULL, it is ignored.

RETURN VALUE:
   None
=============================================================================*/
void SSOMessage_FreeMsg(
    SSMsg_int_msg_type* msg
    )
{
    FREEIF(msg);
}

/*=============================================================================
FUNCTION: SSOMessage_AllocateMsg

DESCRIPTION: allocate the memory for an internal message structure.

PARAMETERS:
    msgType [in]: message type

RETURN VALUE:
    Pointer to a new SSMsg_int_msg_type structure or NULL on MALLOC failure.

COMMENTS:
    The new allocated message is zeroed and initialized with the given msgType.
    When the message is no longer used, call SSOMessage_FreeMsg to release it.
=============================================================================*/
SSMsg_int_msg_type* SSOMessage_AllocateMsg(SSMsg_msg_type_type msgType)
{
    SSMsg_int_msg_type* result = MALLOC( sizeof(SSMsg_int_msg_type) );
    if (result == NULL) return NULL;
    
    result->gen_hdr.msg_type = msgType;
    
    return result;
}

/*=============================================================================
FUNCTION: SSOMessage_FillHeader

DESCRIPTION: fill the header of an internal GSM1x message

PARAMETERS:
    msg [out]: GSM1x internal message to fill its header
    TI  [in] : transaction id to use for setting the header

RETURN VALUE:
    None.

COMMENTS:
    protocol discriminator is set to SSMSG_PD_CC. tiflag and tivalue are set
    according to the given TI.
=============================================================================*/
void SSOMessage_FillHeader(SSMsg_int_msg_type* msg, uint8 TI)
{
    assert(msg != NULL);

    msg->gen_hdr.pd = SSMSG_PD_CC;
    msg->gen_hdr.tiflag = GET_TI_FLAG(TI);
    msg->gen_hdr.tivalue = GET_TI_VALUE(TI);
}

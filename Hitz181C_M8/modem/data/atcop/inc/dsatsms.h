#ifndef DSATSMS_H
#define DSATSMS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the sms commands.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatsms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands. 


===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)

#include "dsati.h"

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATSMS_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an SMS command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

DEPENDENCIES
  DS CMD is generated.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void dsatsms_abort_cmd
(
 dsati_cms_err_e_type error /* +CMS ERROR Number */
);

/*===========================================================================

FUNCTION DSATSMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It is used in both text mode as well as PDU mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
);

/**********************EVENT HANDLER FUNCTIONS******************************/

/*===========================================================================

FUNCTION  DSATSMS_ERR_HANDLER

DESCRIPTION
  When there is an error in processing a SMS AT command, UASMS sends a command
  to handle the error condition. This function is the handler function for the
  error case.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_err_handler 
( 
 dsat_mode_enum_type mode, /* AT command mode    */
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
);

/*===========================================================================

FUNCTION  DSATSMS_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a SMS command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_abt_handler
( 
 dsat_mode_enum_type mode, /* AT command mode    */
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
);

/*=========================================================================

FUNCTION    MT_MSG_QUEUE_INIT

DESCRIPTION
  Initialize the mobile terminted message queue and the free message queue, 
  and link the message items onto the free message queue.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mt_msg_queue_init( void );




/*===========================================================================

FUNCTION  DSATSMS_INIT_SMS

DESCRIPTION
  SMS initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to UASMS.
  - Registers the UASMS message and configuration call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void dsatsms_init_sms ( void );

/*===========================================================================

FUNCTION DSATSMS_SIGNAL_HANDLER

DESCRIPTION
  This function processes the DS_AT_SMS_SIG, which indicates that an event
  has been received from WMS. Handler function for SMS events 
  (both configuration and message) from WMS

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsms_signal_handler
( 
 dsat_mode_enum_type mode
);

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
#endif /* FEATURE_DATA */

#endif /* DSATSMS_H */

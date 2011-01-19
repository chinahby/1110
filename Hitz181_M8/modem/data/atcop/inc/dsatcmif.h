#ifndef DSATCMIF_H
#define DSATCMIF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  call manager interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatvoicecmif.h_v   1.1   12 Jul 2002 10:09:44   randrew  $
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatcmif.h_v   1.0   29 Oct 2002 13:41:22   sbandaru  $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatcmif.h#1 $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/02   ar      Created module.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dsat.h"
#include "dsati.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/

/* CM client (ATCOP) identifier */
extern cm_client_id_type dsatcm_client_id;

/* CM call identifier */
extern byte dsatcm_current_call_id;

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION  DSATCMIF_GET_CM_CALL_INFO

DESCRIPTION
  This function issue query to Call Manager to get call info

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_get_cm_call_info
(
  byte cmif_call_id
);



/*===========================================================================

FUNCTION DSATCMIF_END_CALL

DESCRIPTION
  This function issues a request to CM to end a call.

DEPENDENCIES

RETURN VALUE
  TRUE:     The end request is sent, callback function
              will be called.
  FALSE:    The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
boolean dsatcmif_end_call( uint8 * call_id_ptr );


/*===========================================================================

FUNCTION DSATCMIF_ANSWER_VOICE_CALL

DESCRIPTION
  This command answer a voice call by calling cm_call_cmd_answer

DEPENDENCIES

RETURN VALUE
  TRUE : If the command succeeds.
  FALSE: If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_answer_voice_call( uint8 * call_id_ptr);


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_call_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);



/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_call_cmd_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);



/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to CM.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif ( void );

/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type            cmd,              /* Command ID            */
  cm_call_cmd_err_e_type        cmd_err           /* Command error code    */
);

#endif /* FEATURE_DATA */

#endif /* DSATCMIF_H */

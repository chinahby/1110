#ifndef MMGSDI_REFRESH_H
#define MMGSDI_REFRESH_H
/*===========================================================================


           M M G S D I   R E F R E S H  H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2007 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_refresh.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/12/07   sun      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"
#include "gsdi_exp.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_REFRESH_PROCESS_EVT

  DESCRIPTION
    This function will handle the Refresh event and set the global 
    mmgsdi_data_slot1/mmgsdi_data_slot2 if required.

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    Events will be sent to clients and client will in turn send
    new commands to MMGSDI that will move the overall refresh 
    state
===========================================================================*/
void mmgsdi_refresh_process_evt(
  const mmgsdi_event_data_type *event_ptr);


/* =============================================================================
  FUNCTION:      MMGSDI_REFRESH_MAIN

  DESCRIPTION:
    This function handles all refresh requests  - it is also the entry into the 
    refresh state machine

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support
 
  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_main(
  mmgsdi_task_cmd_type *task_cmd_ptr
);


/*===========================================================================
  FUNCTION mmgsdi_util_check_fcn_list

  DESCRIPTION:
    This function checks the fcn list for any additional files
    being read during a fcn refresh

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS:
    Additional File read flag is set if any files other than the ones
    listed in FCN command are read from the card
 ===========================================================================*/
void mmgsdi_util_check_fcn_list(uim_items_enum_type  uim_file);


/*===========================================================================
   FUNCTION mmgsdi_util_check_client_fcn_list

   DESCRIPTION:
     This function checks the fcn list for any additional files
     being read during a fcn refresh

   PARAMS:
    client id:    client id reading the file
    uim_file :    uim file name

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None

   SIDE EFFECTS:
     Additional File read flag is set if any files other than the ones
     listed in FCN command are read from the card
 ===========================================================================*/
void mmgsdi_util_check_client_fcn_list(
  mmgsdi_client_id_type       client_id,
  uim_items_enum_type         uim_file
);


/*===========================================================================
  FUNCTION mmgsdi_refresh_send_complete

  DESCRIPTION:
    This function will send refresh complete command to itself
  
  DEPENDENCIES:
     None

  LIMITATIONS:
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
 ===========================================================================*/
void mmgsdi_refresh_send_complete(mmgsdi_slot_id_enum_type slot,
                                  boolean                  pass_fail
);


/*===========================================================================
  FUNCTION mmgsdi_refresh_register

  DESCRIPTION:
    This function will let MMGSDI register with itself for refresh fcn
 
  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
   None

  SIDE EFFECTS
    None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_refresh_register(
  mmgsdi_client_id_type client_id
);


/*===========================================================================
  FUNCTION MMGSDI_REFRESH_TIMER_EXPIRY

  DESCRIPTION
    This function builds a response if an external module that MMGSDI is 
    dependent on does not respond

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
void mmgsdi_refresh_timer_expiry (
  unsigned long req_state
);

#endif /* MMGSDI_REFRESH_H */

#ifndef MMGSDI_SESSION_H
#define MMGSDI_SESSION_H
/*===========================================================================


           M M G S D I   S E S S I O N   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_session.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/10/08   nk      Added featuriztion to correlate with function definition 
                   featurization.
11/15/07   sun     Added support for NAA refresh
04/03/07   sp      Send status command before terminating the app.
12/12/06   tml     Update mmgsdi_session_build_select_aid to allow for activation
                   and deactivation of aid with the same api
08/20/06   tml     Added App Reset and 3G Session Reset supports
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   tml     Externalize mmgsdi_session_release_info.
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
07/12/05   tml     Session Management and ISIM support
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:  MMGSDI_ISIM_AUTH_RSP_TAG_ENUM_TYPE
   Indicates what type of authentication response
-----------------------------------------------------------------------------*/
typedef enum
{
  MMGSDI_ISIM_3G_AUTH_SUCCESS    = 0xDB,
  MMGSDI_ISIM_SYNC_FAIL          = 0xDC
} mmgsdi_isim_auth_rsp_tag_enum_type;


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/

#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN

   DESCRIPTION:
     This function will provide open session using the ICC protocol

   DEPENDENCIES:

   LIMITATIONS:

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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main (
  mmgsdi_task_cmd_data_type      * cmd_ptr
);



/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_MAIN_RESPONSE

   DESCRIPTION:
     This function will handle the session management response processing.  

   DEPENDENCIES:


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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_main_response (
  mmgsdi_cmd_enum_type              cmd_type,
  const mmgsdi_uim_report_rsp_type * rsp_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_RELEASE_INFO

   DESCRIPTION:
     This function will release information in regard to the session that
     has been deactivated and closed.

   DEPENDENCIES:


   LIMITATIONS:
     

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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_session_release_info (
  int32                             client_index,
  int32                             session_index
);
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_STATUS_TERMINATE

DESCRIPTION:
  This function is used to build a status command for a particular 
  application termination.

DEPENDENCIES:

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  If status command is executed succesfully, it builds up a command to 
  terminate the app also.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_status_terminate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);


#ifdef FEATURE_MMGSDI_SESSION_MANAGEMENT
/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_DEACTIVATE

DESCRIPTION:
  This function is used to build a deactivate command for a particular 
  application

DEPENDENCIES:
  The client must have a valid session open.
  Achived by calling mmgsdi_session_management_open.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_build_deactivate (
  mmgsdi_client_id_type    client_id,
  mmgsdi_slot_id_enum_type card_slot,
  mmgsdi_cmd_enum_type     orig_req_cmd,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
);


/*===========================================================================
FUNCTION:      MMGSDI_SESSION_BUILD_SELECT_AID

DESCRIPTION:
  This function will build the select AID command and put it into the
  mmgsdi command queue

DEPENDENCIES:


LIMITATIONS:
     

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

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_build_select_aid(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               orig_req_type,
  mmgsdi_cmd_enum_type               curr_req_type,
  mmgsdi_static_data_type            aid,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SESSION_ACTIVATE_APP_INFO

   DESCRIPTION:
     This function will add the app information to the session index of the 
     client. Will be used by open session and refresh

   DEPENDENCIES:


   LIMITATIONS:
     

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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_session_activate_app_info(
  mmgsdi_client_id_type         client_id,
  mmgsdi_static_data_type       app_id);
#endif /* FEATURE_MMGSDI_SESSION_MANAGEMENT */

#endif /* MMGSDI_SESSION_H */

#ifndef MMGSDI_GEN_H
#define MMGSDI_GEN_H
/*===========================================================================


           M M G S D I   I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2005 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_gen.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
10/20/08   sun     Alowed protocol to be sent as part of card reset
09/06/08   kk      Changed pkcs15 command handling
11/15/07   sun     Added support for NAA refresh
09/10/07   tml     Added MFLO Supports. 
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
03/16/06   sun     Added Auto Refresh Function
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
11/03/05   sun     Fixed refresh to a refresh command
08/18/05   pv      Initial version

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


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PUP

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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

=============================================================================*/
extern mmgsdi_return_enum_type mmgsdi_gen_process_card_pup(
  const mmgsdi_card_pup_req_type  *request_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_PDOWN

   DESCRIPTION:
     This function will send down a power down command to the UIM 
     module to power down the CARD.

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

=============================================================================*/
extern mmgsdi_return_enum_type mmgsdi_gen_process_card_pdown(
  const mmgsdi_card_pdown_req_type *request_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_GEN_PROCESS_CARD_RESET

   DESCRIPTION:
     This function shall do the procedures similar to SIM REFRESH.  There by
     sending a reset command to UIM.

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

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_reset(
  const mmgsdi_card_reset_req_type  *req_ptr);


/*=============================================================================
  FUNCTION: MMGSDI_GEN_PROCESS_GET_ATR

  DESCRIPTION:
    Function will get the ATR (Answer to Reset)from the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_get_atr (
  mmgsdi_cmd_enum_type           cmd,
  const mmgsdi_get_atr_req_type* req_ptr
);


/*===========================================================================
FUNCTION MMGSDI_GEN_PROCESS_CARD_READER_STATUS

DESCRIPTION
  This function is called to send a request to the UIM To retrieve the
  Card Reader Status.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_process_card_reader_status (
  mmgsdi_cmd_enum_type                       cmd,
  const mmgsdi_card_reader_status_req_type * req_ptr
);


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


/*===========================================================================
FUNCTION:      MMGSDI_GEN_BUILD_CARD_RESET

DESCRIPTION:
  This function is used to build a card reset command 

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
  Will result in your callback getting called everytime MMGSDI Needs to notify
  any client of any events.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_build_card_reset(
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot_id,
  mmgsdi_cmd_enum_type               curr_req_type,
  uim_protocol_type                  protocol,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
);


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
#endif /* MMGSDI_GEN_H */


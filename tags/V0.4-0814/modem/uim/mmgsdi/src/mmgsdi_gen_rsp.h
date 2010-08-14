#ifndef MMGSDI_GEN_RSP_H
#define MMGSDI_GEN_RSP_H
/*===========================================================================


           M M G S D I   S A P   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2005 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_gen_rsp.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/18/08   js      Updated featurization for Send APDU related code
11/15/08   js      Removed MFLO featurization for Send APDU related code
07/17/08   kk      Added support for PKCS 15
09/10/07   tml     Added MFLO Supports.
08/07/06   tml     Added ACL support
05/23/06   tml     BDN support
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/25/05   pv      Initial version

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_GEN_RSP

   DESCRIPTION:
     This function will process the any generic response
     Note, for BDN and ACL, this function should only come called if it
     is a failed operation, if successful operation, should be handled by
     the get_file_attribute_response and write_response accordingly

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_gen_rsp(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/*=============================================================================
  FUNCTION: MMGSDI_GEN_RSP_GET_ATR

  DESCRIPTION:
    The response to the Get ATR command.  The ATR is internally stored in
    MMGSDI and the response is built internally in MMGSDI

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_rsp_get_atr (
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


/*===========================================================================
FUNCTION MMGSDI_GEN_RSP_CARD_READER_STATUS

DESCRIPTION
  This function is called to process the response for the Card Reader Status.
  The card Status is being handled internally by MMGSDI.

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
mmgsdi_return_enum_type mmgsdi_gen_rsp_card_reader_status(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr
);


#if defined(FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */

#endif /* MMGSDI_GEN_RSP_H */


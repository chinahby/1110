#ifndef MMGSDI_UIM_ICC_H
#define MMGSDI_UIM_ICC_H
/*===========================================================================


           M M G S D I   U I M   I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2005,2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uim_icc.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/20/08   jk      Support for Generate Key API
06/12/08   ssr     Support of MMGSDI Search functionality
03/06/08   nk      Updated mmgsdi_uim_icc_select and mmgsdi_uim_icc_rehabilitate
                   to support synchronous call
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     Rehabilitate and invalidate supports
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     fixed header
08/30/05   jar     Added support for ICC Status Commands
07/26/05   sst     Added MMGSDIBT support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

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


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/



/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_SELECT

   DESCRIPTION:
     This function will format the get file attribute request in the form
     required by the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). This function is also limited to the use of EFs.


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_select (
  const mmgsdi_get_file_attr_req_type  *req_p,
  boolean                               sync_process,
  mmgsdi_get_file_attr_cnf_type       **cnf_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_transparent (
  const mmgsdi_write_req_type *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_record (
  const mmgsdi_write_req_type *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_transparent (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_record (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_ptr
);


/*===========================================================================
FUNCTION:  MMGSDI_UIM_ICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_STATUS

   DESCRIPTION
     This function is called to Queue the command to the UIM for
     a Status command.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_status(
  const mmgsdi_status_req_type        *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_REHABILITATE

   DESCRIPTION:
     This function will rehabilitate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the rehabilitate
                              or invalidate the BDN file
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_rehabilitate (
  const mmgsdi_rehabilitate_req_type *  req_ptr,
  boolean                               sync_process,
  mmgsdi_rehabilitate_cnf_type       ** cnf_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_INVALIDATE

   DESCRIPTION:
     This function will invalidate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate 
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_invalidate (
  const mmgsdi_invalidate_req_type *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_VERIFY_PIN

   DESCRIPTION:
     This function packages the VERIFY PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_verify_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_verify_pin (
  const mmgsdi_pin_operation_req_type *req_ptr,
  mmgsdi_num_retry_enum_type           retry_type
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_ENABLE_PIN
   DESCRIPTION:
     This function packages the ENABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_enable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_enable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_DISABLE_PIN
   DESCRIPTION:
     This function packages the DISABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_disable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_disable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_CHANGE_PIN
   DESCRIPTION:
     This function packages the CHANGE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_change_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_change_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_UNBLOCK_PIN
   DESCRIPTION:
     This function packages the UNBLOCK PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_unblock_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_unblock_pin (
  const mmgsdi_pin_operation_req_type       * req_ptr,
  mmgsdi_num_retry_enum_type                  retry_type
);


#ifdef FEATURE_MMGSDI_CDMA
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called for a Compute IP Authentication request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr
);
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#if defined(FEATURE_AUTH)
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_RUN_CAVE

   DESCRIPTION
     This function is called for a RUN CAVE ALGORITHM request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_GENERATE_KEY_VPM

   DESCRIPTION
     This function is called for a GENERATE KEY VPM request.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_generate_key_vpm(
  const mmgsdi_generate_key_req_type        *  req_ptr
);
#endif /* FEATURE_AUTH */
#endif /* FEATURE_MMGSDI_CDMA */


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM 
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for ICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_search_record (
  const mmgsdi_search_req_type             * req_ptr
);


#endif /* MMGSDI_UIM_ICC_H */


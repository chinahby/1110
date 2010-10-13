#ifndef MMGSDICNF_H
#define MMGSDICNF_H
/*===========================================================================


           M M G S D I   U T I L I T Y   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2005,2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_cnf.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/04/08   tml     Added sw to the cnf response header
05/05/08   sun     Added Prototype.
03/14/08   nk      Added support for mmgsdi_cnf_build_rehabilitate_data
11/15/07   sun     Added support for NAA refresh
09/27/07   tml     Lint fixes
09/10/07   tml     Added MFLO support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
05/17/07   wli     Added support for JCDMA RUIM
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     lint
04/13/06   jar     Added ONCHIP SIM Support
02/22/06   tml     Return client and session id in open and close cnf
01/04/06   tml     Updated enum name for SAP and Card powerup and down
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "uim.h"
#include "mmgsdi.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_MAP_REQ_TO_CNF_ENUM_TYPE

   DESCRIPTION:
     This function maps the request enum type to confirmation enum

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

=============================================================================*/
void mmgsdi_cnf_map_req_to_cnf_enum_type(
  mmgsdi_cmd_enum_type  req_type,
  mmgsdi_cnf_enum_type *cnf_type_ptr
);


/* =============================================================================
   FUNCTION:      mmgsdi_cnf_populate_cnf_rsp_header

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

=============================================================================*/
void mmgsdi_cnf_populate_cnf_rsp_header(
  mmgsdi_task_cnf_type              * task_cnf_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_AND_QUEUE

   DESCRIPTION:
     This function build the task command for confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS
     

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_and_queue(
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_request_header_type  * req_header_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const void                        * add_data_ptr,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_WRITE_DATA

   DESCRIPTION:
     This function build the write confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS
     

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_write_data(
  mmgsdi_write_cnf_type                   * write_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_READ_DATA

   DESCRIPTION:
     This function build the read confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS
     

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_read_data(
  mmgsdi_read_cnf_type                    * read_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_read_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_FILE_ATTR_DATA

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS
     

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_file_attr_data(
  mmgsdi_get_file_attr_cnf_type           * get_file_attr_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_request_header_type        * req_header_ptr,
  const mmgsdi_data_type                  * data_read_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_SESSION_BUILD_CLOSE_CNF

   DESCRIPTION:
     This function builds the close confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_close_data(
  mmgsdi_session_close_cnf_type *close_cnf_ptr,
  mmgsdi_return_enum_type        mmgsdi_status,
  mmgsdi_client_id_type          client_id,
  const mmgsdi_client_id_type   *session_id_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_SESSION_BUILD_OPEN_CNF

   DESCRIPTION:
     This function builds the open confirmation data
     
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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_open_data(
  mmgsdi_session_open_cnf_type  *open_cnf_ptr,
  mmgsdi_return_enum_type        mmgsdi_status,
  mmgsdi_client_id_type          client_id,
  const mmgsdi_client_id_type   *session_id_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ISIM_AUTH_DATA

   DESCRIPTION:
     This function builds the ISIM authentication confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_isim_auth_data(
  mmgsdi_isim_auth_cnf_type   *isim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_data_type      *auth_rsp_data_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the send APDU confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_send_apdu_data(
  mmgsdi_send_apdu_cnf_type               * apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * uim_rsp_data_ptr);


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PUP_DATA

   DESCRIPTION:
     This function builds the Card Power Up confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pup_data(
  mmgsdi_card_pup_cnf_type               * card_pup_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pup_options_enum_type        pup_options);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PDOWN_DATA

   DESCRIPTION:
     This function builds the Card Power Down confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pdown_data(
  mmgsdi_card_pdown_cnf_type             * card_pdown_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pdown_options_enum_type      pdown_options);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_DATA

   DESCRIPTION:
     This function builds the Status confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_status_data(
  mmgsdi_status_cnf_type            * status_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_data_type            * status_data_ptr);


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_PIN_OPERATION_DATA

   DESCRIPTION:
     This function builds the PIN Operation confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_pin_operation_data(
  mmgsdi_cmd_enum_type                req_type,
  mmgsdi_pin_operation_cnf_type     * pin_operation_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_client_req_extra_info_type * extra_info_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA

   DESCRIPTION:
     This function builds the PIN Status confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_all_pin_status_data(
  mmgsdi_get_all_pin_status_cnf_type     * get_all_pin_status_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SELECT_AID

   DESCRIPTION:
     This function builds the Select AID confirmation data

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

=============================================================================-*/
mmgsdi_return_enum_type mmgsdi_cnf_build_select_aid_data(
  mmgsdi_select_aid_cnf_type             * select_aid_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  const mmgsdi_client_req_extra_info_type *extra_param_ptr
);


#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA

   DESCRIPTION:
     This function builds the Compute IP authentication confirmation data

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Protocol is incorrect 

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_compute_ip_auth_data(
  mmgsdi_compute_ip_auth_cnf_type   *compute_ip_auth_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *compute_ip_auth_rsp_data_ptr
);
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN || FEATURE_UIM_SUPPORT_3GPD */


#ifdef FEATURE_MMGSDI_CDMA
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_CAVE_DATA

   DESCRIPTION:
     This function builds the RUN CAVE confirmation data

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    Protocol is incorrect 

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_cave_data(
  mmgsdi_run_cave_cnf_type          *run_cave_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *run_cave_rsp_data_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA

   DESCRIPTION:
     This function builds the Generate Key confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_generate_key_vpm_data(
  mmgsdi_generate_key_vpm_cnf_type  *generate_key_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *generate_key_rsp_data_ptr
);
#endif /* FEATURE_MMGSDI_CDMA */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_GET_ALL_PIN_STATUS_EVT

   DESCRIPTION:
     This function generates all the pin events as a result of the 
     get_all_pin_status confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_generate_get_all_pin_status_evt(
  void
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_EVT

   DESCRIPTION:
     This function based on the confirmation, generate the corresponding event
     as needed

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     None

   SIDE EFFECTS:

=============================================================================-*/
void mmgsdi_cnf_generate_evt(
  mmgsdi_return_enum_type    mmgsdi_status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);


#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_JCDMA_GET_CARD_INFO

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     MMGSDI_SUCCESS
     

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_jcdma_get_card_info(
  mmgsdi_jcdma_get_card_info_cnf_type     * jcdma_get_card_info_cnf_ptr
);
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_get_atr_data(
  mmgsdi_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type         mmgsdi_status,
  mmgsdi_slot_id_enum_type        slot_id
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_READER_STATUS_DATA

   DESCRIPTION:
     This function builds the SAP Card Reader Status confirmation data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_card_reader_status_data(
  mmgsdi_card_reader_status_cnf_type  * card_reader_status_cnf_ptr,
  mmgsdi_return_enum_type               mmgsdi_status);


#ifdef FEATURE_MMGSDI_MFLO
#error code not present
#endif /* FEATURE_MMGSDI_MFLO */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REFRESH_DATA

   DESCRIPTION:
     This function builds cnf for the REFRESH

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_refresh_data(
  mmgsdi_refresh_cnf_type                 * refresh_cnf_ptr,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REHABILITATE_DATA

   DESCRIPTION:
     This function build the rehabilitate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_rehabilitate_data(
  mmgsdi_rehabilitate_cnf_type                     * rehabilitate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type          * extra_param_ptr);


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEARCH_DATA

   DESCRIPTION:
     This function build the search confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS : The command processing was successful.
     MMGSDI_ERROR   : The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_search_data(
  mmgsdi_search_cnf_type                  *search_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  const mmgsdi_client_req_extra_info_type *extra_param_ptr,
  const mmgsdi_data_type                  *data_search_ptr
);


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /*FEATURE_MMGSDI_PKCS15*/

#endif /* MMGSDICNF_H */


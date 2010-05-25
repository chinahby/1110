#ifndef MMGSDI_ICC_H
#define MMGSDI_ICC_H
/*===========================================================================


           M M G S D I   I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_icc.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/06/08   kk      Changed pkcs15 command handling
08/20/08   jk      Support for Generate Key API
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     BDN, Rehabilitate and invalidate supports
04/13/06   jar     Added ONCHIP SIM SUPPORT
11/03/05   tml     Fixed header
08/30/05   jar     Added support to send Status Commands to the Card
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


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* =============================================================================
   FUNCTION:      MMGSDI_ICC_GET_FILE_ATTR

   DESCRIPTION:
     This function will provide get file attributes to files only in
     SIM/RUIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be determined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). this function is also limited to the use of EFs.


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
mmgsdi_return_enum_type mmgsdi_icc_get_file_attr(
  const mmgsdi_get_file_attr_req_type * req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_ICC_WRITE

   DESCRIPTION:
     This function will provide write access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_write (
  const mmgsdi_write_req_type      * req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_ICC_READ

   DESCRIPTION:
     This function will provide read access to any file using the ICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_read (
  const mmgsdi_read_req_type      * req_ptr
);


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to GSM 11.11 or IS820C

    TThis function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used
    to give an opportunity for a pro active SIM to indicate that the
    SIM wants to issue a SIM Application Toolkit command to the ME.

    The information is provided asynchronously in the response cnf.
    Which will contain information containing:  file ID, total memory space
    available, CHV enabled/disabled indicator, CHV status and other
    GSM/CDMA specific data (identical to SELECT).

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

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

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_status (
  mmgsdi_status_req_type      * req_ptr
);

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* =============================================================================
  FUNCTION:      MMGSDI_ICC_PROCESS_BDN

  DESCRIPTION:
    This function will enable/disable BDN by doing a rehabiliate or 
    invalidate to UIM.


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_process_bdn (
  const mmgsdi_bdn_req_type   *req_ptr
);


/* =============================================================================
  FUNCTION:      MMGSDI_ICC_REHABILITATE

  DESCRIPTION:
    This function will rehabilitate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_rehabilitate (
  const mmgsdi_rehabilitate_req_type   *req_ptr
);


/* =============================================================================
  FUNCTION:      MMGSDI_ICC_INVALIDATE

  DESCRIPTION:
    This function will invalidate the EF


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_invalidate (
  const mmgsdi_invalidate_req_type   *req_ptr
);


/* =============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations on ICC card

  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_operation should be used

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
mmgsdi_return_enum_type  mmgsdi_icc_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr
);


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_STATUS

  DESCRIPTION:
    This function will get the PIn Status from the cache or send the command to 
    the card if the PIN Status in not available
  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

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

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_pin_status (
  const mmgsdi_get_all_pin_status_req_type   * req_ptr
);


/*===========================================================================
  FUNCTION MMGSDI_ICC_BUILD_GET_PIN_STATUS

  DESCRIPTION
    This function will buid a request to the MMGSDI Task to request the PIN
    status for all relevant pins applicable to the client ID provided.
    If the client ID provided is in a session, then the PIN IDs applicable
    to the application in the session will be returned.

  DEPENDENCIES:

  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

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
mmgsdi_return_enum_type mmgsdi_icc_build_get_pin_status(void);


#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#ifdef FEATURE_MMGSDI_CDMA
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/*===========================================================================
  FUNCTION:      MMGSDI_ICC_COMPUTE_IP_AUTH

  DESCRIPTION:
    This function will allow COMPUTE IP AUTH command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_compute_ip_auth (
  const mmgsdi_compute_ip_req_type   *req_ptr
);
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#if defined(FEATURE_AUTH)
/*===========================================================================
  FUNCTION:      MMGSDI_ICC_RUN_CAVE

  DESCRIPTION:
    This function will allow RUN CAVE command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_run_cave (
  const mmgsdi_run_cave_req_type   *req_ptr
);


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GENERATE_KEY_VPM

  DESCRIPTION:
    This function will allow GENERATE KEY VPM command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_generate_key_vpm (
  const mmgsdi_generate_key_req_type   *req_ptr
);
#endif /* FEATURE_AUTH */
#endif /* FEATURE_MMGSDI_CDMA */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_SEARCH

  DESCRIPTION:
    This function will provide search access to record file using the ICC
    protocol

  DEPENDENCIES:

  LIMITATIONS:
    
  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support.
                              When the Search Type is not MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_search (
  mmgsdi_search_req_type      * req_ptr
);

#endif /* MMGSDI_ICC_H */


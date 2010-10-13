#ifndef MMGSDI_UICC_H
#define MMGSDI_UICC_H
/*===========================================================================


           M M G S D I   U I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uicc.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/06/08   kk      Changed pkcs15 command handling
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
08/07/06   tml     Added ACL support
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     BDN support
11/03/05   tml     Fixed header
02/24/05   sk      Initial version

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



/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_WRITE

  DESCRIPTION:
    This function will provide write access to any file using the UICC
    protocol

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_write should be used

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

===========================================================================--*/
mmgsdi_return_enum_type  mmgsdi_uicc_write (
  mmgsdi_write_req_type      * req_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UICC_GET_FILE_ATTR

   DESCRIPTION:
     This function will provide get file attributes to files only in
     USIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:



   LIMITATIONS:
     This function is limited to the use of UICC only. Also this function is limited
     to the use of EFs.


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_get_file_attr(
  mmgsdi_get_file_attr_req_type * req_p
);


/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_READ

  DESCRIPTION:
    This function will provide read access to any file using the UICC
    protocol

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_read should be used

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

===========================================================================--*/
mmgsdi_return_enum_type  mmgsdi_uicc_read (
  mmgsdi_read_req_type      * req_ptr
);


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_STATUS

  DESCRIPTION:
    This function will send a status command to card.  It will stay compliant
    to ETSI TS 102.221

    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    The information is provided asynchronously in the response cnf.

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
mmgsdi_return_enum_type  mmgsdi_uicc_status (
  const mmgsdi_status_req_type      * req_ptr
);

/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_PROCESS_BDN

  DESCRIPTION:
    This function will enable/disable BDN by doing an update to the EST EF

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_process_bdn (
  const mmgsdi_bdn_req_type   *req_ptr
);


/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations on UICC card

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_pin_operation should be used

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
mmgsdi_return_enum_type  mmgsdi_uicc_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr
);


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_PIN_STATUS

  DESCRIPTION:
    This function will get the pin status either from the global data if the 
    pin is already cached or from the card
  
  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_pin_status should be used.

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
mmgsdi_return_enum_type  mmgsdi_uicc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
);


/* ============================================================================
  FUNCTION:      MMGSDI_UICC_SELECT_AID

  DESCRIPTION:
    This function will select the aid provided by the client and activate the 
    app on the card

  DEPENDENCIES:
  LIMITATIONS:
    Only available on uicc.

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
mmgsdi_return_enum_type  mmgsdi_uicc_select_aid (
  const mmgsdi_select_aid_req_type      * req_ptr
);


/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_TOTAL_USIM_AIDS

  DESCRIPTION:
    This function will get the total number of aids available. The command
    is not sent down to the card.

  DEPENDENCIES:

  LIMITATIONS:
    ONLY applicable to UICC

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

===========================================================================-*/
mmgsdi_return_enum_type  mmgsdi_uicc_get_total_usim_aids(
  int32 *count_ptr
);


/*===========================================================================
FUNCTION MMGSDI_UICC_BUILD_GET_ALL_PIN_STATUS

DESCRIPTION
  This function will buid a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the client ID provided.
  If the client ID provided is in a session, then the PIN IDs applicable
  to the application in the session will be returned.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_build_get_pin_status(void);


/* ===========================================================================
  FUNCTION:      MMGSDI_UICC_GET_AVAILABLE_AIDS

  DESCRIPTION:
    This function will provide read access to any file using the UICC
    protocol

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_read should be used

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
mmgsdi_return_enum_type  mmgsdi_uicc_get_available_aids(int32 *index);


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_ACL_OPERATION

  DESCRIPTION:
    This function will enable/disable ACL by doing an update to the EST EF

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only). 

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uicc_acl_operation (
  const mmgsdi_acl_operation_req_type   *req_ptr
);


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


/*===========================================================================
  FUNCTION:      MMGSDI_UICC_SEARCH

  DESCRIPTION:
    This function will provide search access to record file using the UICC
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
                              When the Search Type is MMGSDI_ICC_SEARCH.

  SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_search (
  mmgsdi_search_req_type      * req_ptr
);
#endif /* MMGSDI_UICC_H */


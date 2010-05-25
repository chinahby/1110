#ifndef MMGSDI_UIM_UICC_H
#define MMGSDI_UIM_UICC_H
/*===========================================================================


           M M G S D I   U I M   U I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uim_uicc.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Added support for UICC Status Commands
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
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

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_transparent (
  const mmgsdi_write_req_type             * req_ptr,
  const mmgsdi_uim_additional_params_type * add_params
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_write_record (
  const mmgsdi_write_req_type             * req_ptr,
  const mmgsdi_uim_additional_params_type * add_params
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SELECT

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
     This function is limited to the use of UICC. This function is also
     limited to the use of EFs.


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
mmgsdi_return_enum_type mmgsdi_uim_uicc_select (
  mmgsdi_get_file_attr_req_type             *req_ptr,
  const mmgsdi_uim_additional_params_type   *add_params,
  boolean                                    sync_process,
  mmgsdi_get_file_attr_cnf_type            **cnf_ptr
);
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_transparent (
  const mmgsdi_read_req_type              * req_ptr,
  const mmgsdi_uim_additional_params_type * add_params,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_read_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_read_record (
  const mmgsdi_read_req_type              * req_ptr,
  const mmgsdi_uim_additional_params_type * add_params,
  boolean                                   sync_process,
  mmgsdi_read_cnf_type                    **cnf_ptr
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_OPEN_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Open Channel
     function to build a UIM Manage Channel Request to open a new
     communication channel with the Card presently inserted.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_open_channel(
  const mmgsdi_session_open_req_type *  req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_CLOSE_CHANNEL

   DESCRIPTION
     This function is called from the Channel Management Close Channel
     function to build a UIM Manage Channel Request to close a new
     communication channel with the Card presently inserted.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_close_channel(
  const mmgsdi_session_close_req_type *  req_ptr
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SESSION_DEACTIVATE

   DESCRIPTION
     This function is called for Status command.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_session_deactivate(
  const mmgsdi_status_req_type        *  req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_ISIM_AUTH

   DESCRIPTION
     This function is called for ISIM Authentication request.

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_isim_auth(
  const mmgsdi_auth_req_type        *  req_ptr
);

/*===========================================================================
FUNCTION:  MMGSDI_UIM_UICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr);



/* =============================================================================
   FUNCTION:      MMGSDI_UIM_UICC_STATUS

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
mmgsdi_return_enum_type mmgsdi_uim_uicc_status(
  const mmgsdi_status_req_type        *  req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_VERIFY_PIN

   DESCRIPTION:
     This function packages the VERIFY PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_verify_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_verify_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                  retry_type
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_ENABLE_PIN
   DESCRIPTION:
     This function packages the ENABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_enable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_enable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_DISABLE_PIN
   DESCRIPTION:
     This function packages the DISABLE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_disable_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_disable_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_CHANGE_PIN
   DESCRIPTION:
     This function packages the CHANGE PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_change_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_change_pin (
  const mmgsdi_pin_operation_req_type             * req_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_UNBLOCK_PIN
   DESCRIPTION:
     This function packages the UNBLOCK PIN  command and sends it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol access, mmgsdi_uim_icc_unblock_pin should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_unblock_pin (
  const mmgsdi_pin_operation_req_type        * req_ptr,
  mmgsdi_num_retry_enum_type                  retry_type
);


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_UICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM 
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for UICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_uicc_search_record (
  const mmgsdi_search_req_type             * req_ptr,
  const mmgsdi_uim_additional_params_type  * add_params
);


#endif /* MMGSDI_UIM_UICC_H */


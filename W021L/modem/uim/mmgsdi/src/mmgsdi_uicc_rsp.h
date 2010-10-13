#ifndef MMGSDI_UICC_RSP_H
#define MMGSDI_UICC_RSP_H
/*===========================================================================


           M M G S D I   I C C   R S P   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_uicc_rsp.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original 
                   error mmgsdi status to client
06/12/08   ssr     Support of MMGSDI Search functionality
05/16/08   sun     Fixed Warning
05/02/07   sun     Added support for Service Available and USIM Auth
04/07/07   sp      Added parsing of life cycle byte
10/26/06   tml     Externalize decode_pin_header function
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Added support to handle a Status Command Response from the 
                   Card
07/26/05   sst     Added MMGSDIBT support
07/13/05   tml     extern the parsing function
02/24/05   tml     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdilib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
/*TODO - Can we remove this?*/
#define MMGSDI_UICC_MAX_PIN_NUM 27

#define MMGSDI_UICC_LIFE_CYCLE_FILE_ACTIVATED    0x05
#define MMGSDI_UICC_LIFE_CYCLE_FILE_DEACTIVATED  0x04
#define MMGSDI_UICC_LIFE_CYCLE_BYTE_CHECK        0xF8

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_UICC_PIN_MAP_TABLE_TYPE

   DESCRIPTION:
     This structure contains the mapping table to map the MMGSDI PIN ENUMS to
     the PINS sent by the card.
-------------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_pin_enum_type   pin_enum;
  int32                  pin_val;
} mmgsdi_uicc_pin_map_table_type;

extern mmgsdi_uicc_pin_map_table_type mmgsdi_pin_map_table[MMGSDI_UICC_MAX_PIN_NUM];

/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_read (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_read_cnf_type              ** mmgsdi_cnf_ptr,
  boolean                              synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_PIN_HEADER_DATA

   DESCRIPTION:
     This function decodes the EF header information into mmgsdi file attribute
     structure

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_decode_pin_header_data (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_DECODE_FILE_HEADER

   DESCRIPTION:
     This function decodes the header information for MF, DF and EF

   DEPENDENCIES:

   LIMITATIONS:
     This function is limited to the use of UICC
     Currently, this function only supports decoding for EF

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command process is successful.
     MMGSDI_ERROR:            The Input parameter is null.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_uicc_rsp_decode_file_header (
  const mmgsdi_request_header_type           * orig_req_header_ptr,
  const mmgsdi_get_file_info_extra_info_type * orig_extra_info_ptr,
  const uint8                                * header_content_ptr,
  int32                                        header_length,
  mmgsdi_file_attributes_type                 *gsdi_format_attr_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to the get file attributes request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type  * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type    ** attr_cnf_ptr,
  boolean                             synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This function will parse write access to any file using the UICC protocol

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type *uim_rsp_ptr
);


/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   DEPENDENCIES:

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uicc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to the Status Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Operation Command

   DEPENDENCIES:


   LIMITATIONS:
     For ICC protocol status, mmgsdi_icc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_SELECT_AID

   DESCRIPTION:
     This function will build the response to the Select AID Command

   DEPENDENCIES:


   LIMITATIONS:
     NOT Applicable to ICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_select_aid(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


/* ============================================================================
   FUNCTION:      MMGSDI_UICC_RSP_SEARCH

   DESCRIPTION:
     This function will build the response to search request

   DEPENDENCIES:


   LIMITATIONS:
     

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_uicc_rsp_search (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr
);


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
#endif /* MMGSDI_UICC_RSP_H */


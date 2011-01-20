#ifndef MMGSDI_ICC_RSP_H
#define MMGSDI_ICC_RSP_H
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

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdi_icc_rsp.h#1 $$ $DateTime: 2008/12/15 12:00:08 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
09/06/08   kk      Changed pkcs15 command handling
09/06/08   sun     Reverted featurization change
08/28/08   sun     Fixed Featurization
08/26/08   jk      Fixed 1X Auth Featurization for LTK Compilation
08/20/08   jk      Support for Generate Key API
06/12/08   ssr     Support of MMGSDI Search functionality
03/08/08   nk      Added synchronous support for mmgsdi_icc_rsp_get_file_attr
                   and mmgsdi_icc_rsp_rehabitate
08/21/07   jk      Corrected function header for run cave
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
05/02/07   sun     Added support for Srv Available
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     BDN, Rehabilitate and invalidate supports
04/13/06   jar     ONCHIP SIM Support
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
08/30/05   jar     Added support to handle a ICC Status Response
07/26/05   sst     Added MMGSDIBT support
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
/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_RSP_EXTRACT_FILE_ATTR

  DESCRIPTION:
    This function is used to decode and map the Header information from the SIM
    format as define in GSM 11.11 as follows:

    byte 01:    rfu
    byte 02:    rfu
    byte 03:    file size MSB
    byte 04:    file size LSB
    byte 05:    file id MSB
    byte 06:    file id LSB
    byte 07:    type of file
              00 --> RFU
              01 --> MF
              02 --> DF
              04 --> EF
    byte 08:    RFU for Linear and Transparent Files
              bit7 = 1 Indicates Cyclic Increase allowed
    byte 09:    B8-B5 Read/Seek Permissions
              B4-B1 Update Permissions
    byte 10:    B8-B5 Increase Permissions
              B4-B1 RFU
    byte 11:    B8-B5 RehabilitatePermissions
              B4-B1 Invalidate Permissions
    byte 12:    File Status
              B8-B4 RFU
              B3 = 0 Not Readable/Writeable when invalidate
              B3 = 1 Readable/Writeable when invalidated
              B2 RFU
              B1 = 0 (Invalidated)
              B1 = 1 (Not Invalideated - Valid)
    byte 13:    Length of the following Information (Byte 14 to end]
    byte 14:    Structure of EF
              00 --> TRANSPARENT
              01 --> LINEAR FIXED
              03 --> CYCLIC
    byte 15:    Length of Record.  For cyclic and linear fixed, this denotes
              the length of the record.  For a transparent file it is 0x00.
    byte 16+:   (optional when defined)

    and it is populated into mmgsdi_file_attributes_type

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_icc_rsp_extract_file_attr (
  mmgsdi_slot_id_enum_type       slot,
  uint8 *                        sim_header_attr,
  mmgsdi_len_type                sim_header_data_len,
  mmgsdi_file_attributes_type  * attr_ptr
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This function will build the response to Get File Attribute request

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type        ** attr_cnf_ptr,
  boolean                                 synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_READ

   DESCRIPTION:
     This function will build the response to Read request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_read (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_read_cnf_type              ** mmgsdi_cnf_ptr,
  boolean                              synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

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
mmgsdi_return_enum_type  mmgsdi_icc_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/*-----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEND_APDU

   DESCRIPTION:
     This function will build the response to SEND_APDU request

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr);


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_STATUS

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

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif/* FEATURE_MMGSDI_ONCHIP_SIM */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_REHABILITATE

   DESCRIPTION:
     This function will build the response to Rehabilitate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_rehabitate (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_rehabilitate_cnf_type         ** rehab_cnf_ptr,
  boolean                                 synch
);

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_INVALIDATE

   DESCRIPTION:
     This function will build the response to Invalidate request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_invalidate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_invalidate (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);
/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This function will build the response to the PIN Operation

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_icc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_STATUS

   DESCRIPTION:
     This function will build the response to a GET ALL PIN Status Command

   DEPENDENCIES:

   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_rsp_pin_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


#ifdef FEATURE_MMGSDI_MBMS 
#error code not present
#endif /*FEATURE_MMGSDI_MBMS */


#ifdef FEATURE_MMGSDI_CDMA
#if defined(FEATURE_UIM_SUPPORT_3GPD) || defined(FEATURE_UIM_SUPPORT_HRPD_AN)
/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_compute_ip_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);
#endif /* FEATURE_UIM_SUPPORT_3GPD || FEATURE_UIM_SUPPORT_HRPD_AN */


#ifdef FEATURE_AUTH
/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_run_cave (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_GENERATE_KEY_VPM

   DESCRIPTION:
     This function will build the response to a GENERATE KEY command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_generate_key_vpm (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);
#endif /* FEATURE_AUTH */
#endif /* FEATURE_MMGSDI_CDMA */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEARCH

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

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_search (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr
);


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */
#endif /* MMGSDI_ICC_RSP_H */


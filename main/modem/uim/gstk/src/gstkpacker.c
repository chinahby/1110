/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P A C K E R


GENERAL DESCRIPTION
  The provides utility function for packing various raw data into the
  corresponding tlv format


FUNCTIONS
  gstk_packer_address_tlv
    This function populates the passed in the address tlv pointer

  gstk_packer_alpha_tlv
    This function populates the passed in alpha tlv pointer

  gstk_packer_bc_repeat_ind_tlv
    This function populates the passed in the bc repeat indicator tlv pointer

  gstk_packer_bcch_list_tlv
    This function populates the passed in bcch list tlv pointer

  gstk_packer_cause_tlv
    This function populates the passed in the cause tlv pointer

  gstk_packer_cc_req_action_tlv
    This function populates the passed in the call control requested
    action tlv pointer

  gstk_packer_ccp_tlv
    This function populates the passed in the ccp tlv pointer

  gstk_packer_cmd_details_tlv
    This function populates the passed in command_details pointer with the
    info in the command details ref table after verifying the ref id for the
    command details ref table is valid

  gstk_packer_date_time_tlv
    This function populates the passed in the date/time/timezone tlv pointer

  gstk_packer_dev_id_tlv
    This function populates the passed in device_id pointer

  gstk_packer_duration_tlv
    This function populates the passed in duration tlv pointer

  gstk_packer_evt_list_tlv
    This function populates the passed in the event list tlv pointer

  gstk_packer_help_request_tlv
    This function populates the passed in the help request tlv pointer

  gstk_packer_imei_tlv
    This function populates the passed in imei tlv pointer

  gstk_packer_item_id_tlv
    This function populates the passed in the item id tlv pointer

  gstk_packer_lang_sel_tlv
    This function populates the passed in the language selection tlv pointer

  gstk_packer_location_info_tlv
    This function populates the passed in location info tlv pointer

  gstk_packer_location_status_tlv
    This function populates the passed in the location status tlv pointer

  gstk_packer_nmr_tlv
    This function populates the passed in nmr tlv pointer

  gstk_packer_result_tlv
    This function populates the passed in result tlv pointer

  gstk_packer_sms_tpdu_tlv
    This function populates the passed in the sms tpdu tlv pointer

  gstk_packer_subaddress_tlv
    This function populates the passed in the subaddress tlv pointer

  gstk_packer_text_string_tlv
    This function populates the passed in text tlv pointer

  gstk_packer_timing_advance_tlv
    This function populates the passed in timing advance tlv pointer

  gstk_packer_transaction_id_tlv
    This function populates the passed in the transaction id tlv pointer

  gstk_packer_browser_term_cause_tlv
    This function populates the passed in browser termination cause tlv pointer

  gstk_packer_timer_id_tlv
    This function populates the passed in timer id tlv pointer

  gstk_packer_timer_value_tlv
    This function populates the passed in timer value tlv pointer

  gstk_packer_cell_broadcast_page_tlv
    This function populates the passed in cell broadcast page tlv pointer

  gstk_packer_ch_status_tlv
    This function populates the passed in channel status tlv pointer

  gstk_packer_csd_bearer_param
    This function populates the csd bearer information.

  gstk_packer_gprs_bearer_param
    This function populates the gprs bearer information.

  gstk_packer_bearer_description_tlv
    This function populates the passed in bearer description tlv pointer

  gstk_packer_buffer_size_tlv
    This function populates the passed in buffer size tlv pointer

  gstk_packer_ch_data_len_tlv
    This function populates the passed in channel data length tlv pointer

  gstk_packer_ch_data_tlv
    This function populates the passed in channel data tlv pointer

  gstk_packer_nmr_utran_tlv
    This function packs the nmr data in the nmr utran tlv pointer

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009, 2011 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE


$Header: //source/qcom/qct/modem/uim/su/baselines/sqa/gstk/rel/07H1_2/src/gstkpacker_c/rev5/gstkpacker.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   adp     Fix buffer overflow issue 
06/25/09   dd      Adding time zone information
06/20/09   dd      Enable SMS for CDMA
06/18/09   dd      Changed GSTK_MAX_RAW_SMS_LEN to 255
05/11/09   sg      Changed gstk_packer_address_tlv function to ignore '+' for
                   international numbers.
02/06/08   sk      Support for PLI UTRAN NMR
09/05/07   sk      Fixed warning
08/29/07   sk      Removed additional info conversion
08/01/07   sk      Fixed lint error
07/03/07   tml     Added support for additional info when call mod by CC
06/26/07   sk      Removed warnings.
06/24/07   sk      Fixed potential buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
09/18/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
07/03/06   sk      Added support for CCAT sms pp dl.
06/24/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control
05/21/06   sk      Fixed lint errors.
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
04/04/06   sk      Added support for PLI - Battery status,ESN, MEID, IMEISV
11/21/05   sp      Fixed lint errors
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
08/08/05   tml     fixed memcpy issue in channel data packing
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added gstk_packer_access_technology_tlv()
05/11/05   sst     Lint fixes
12/04/04  sk/tml   Channel status fix
11/23/04  sk/tml   Added SAT/USAT data support
08/19/04   tml     Added cell broadcast support
08/03/05   tml     Fixed timer value format issue.  Added error messages
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports.  Added more debug messages
09/12/03   tml     Updated the address packing function
09/03/03   tml     Assigned Command detail tag correctly
07/08/03   tml     Added check such that only populate the address and TON/NPI
                   field in address TLV if the length is greater than 0
05/18/03   tml     linted
04/21/03   tml     Added NPI/TON support and cdma sms support
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03 tml/jar   Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstkutil.h"

#include "err.h"
#include "string.h"
#include "tmc.h"





/*===========================================================================
FUNCTION gstk_packer_cmd_details_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Index location in the command_Details_ref_table
  command_rsp_type: [Input] Specifies the type of terminal response, e.g.,
                            GSTK_DISPLAY_TEXT_CNF, etc,
  command_number: [Input] Command number for the proactive command that
                          this terminal response is responding to
  command_details: [Input/Output] Pointers to which the command details
                                  to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cmd_details_tlv(
  uint32                            ref_id,
  uint32                            command_rsp_type,
  uint8                             command_number,
  gstk_command_details_tlv_type *   command_details
)
{

    MSG_HIGH("** Packing Cmd Details", 0, 0, 0);

    if(command_details == NULL)
    {
       MSG_ERROR("Cmd Detail Err: NULL", 0, 0, 0);
       return GSTK_BAD_PARAM;
    }
    /* 4 checks to verify that the ref_id is a valid value/match to the information
    ** in the command_details_ref_table
    ** - reference_id check
    ** - whether the location in the table is still valid
    ** - command number check
    ** - response type and type of command match
    */
    if (ref_id < GSTK_MAX_PROACTIVE_COMMAND) { /*ref_id within range */
        if (command_details_ref_table[ref_id].free_slot != TRUE ) { /* this location is still used*/
          if ((int)command_details_ref_table[ref_id].expected_cmd_rsp_type == command_rsp_type) {
              /* command matches -> valid ref_id */
              if (command_details_ref_table[ref_id].command_details.command_number !=
                    command_number ) {
                /* wrong match */
                MSG_ERROR("Cmd Detail Err: Command Num Mismatch", 0, 0, 0);
                return GSTK_BAD_PARAM;
              }
          }
        }
        else {
          MSG_ERROR("Cmd Detail Err: command_details_ref_table[0x%x] not in use", ref_id, 0, 0);
          return GSTK_BAD_PARAM;
        }
    }
    else {
      MSG_ERROR("Cmd Detail Err: ref_id 0x%x out of range", 0, 0, 0);
      return GSTK_BAD_PARAM;
    }

    /* populate the command_details tlv for the response */
    command_details->tag = command_details_ref_table[ref_id].command_details.tag;
    command_details->length = GSTK_COMMAND_DETAILS_LEN;
    command_details->command_number =
      command_details_ref_table[ref_id].command_details.command_number;
    command_details->type_of_command =
      command_details_ref_table[ref_id].command_details.type_of_command;
    command_details->qualifier =
      command_details_ref_table[ref_id].command_details.qualifier;
    return GSTK_SUCCESS;
} /*gstk_packer_cmd_details_tlv*/


/*===========================================================================
FUNCTION gstk_packer_dev_id_tlv

DESCRIPTION

  This function populates the passed in device_id pointer

PARAMETERS
  source: [Input] Indicates source device
  destination: [Input] Indicates target device
  device_id: [Input/Output] Pointers to which the device id to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_dev_id_tlv(
  device_identity_e                     source,
  device_identity_e                     destination,
  gstk_device_identities_tag_tlv_type * device_id
)
{

    MSG_HIGH("** Packing device ID", 0, 0, 0);

    if(device_id == NULL)
    {
       MSG_ERROR("Device Id Err: NULL", 0, 0, 0);
       return GSTK_BAD_PARAM;
    }
    device_id->tag = (int)GSTK_DEVICE_IDENTITY_TAG;
    device_id->device_tag_length = GSTK_DEVICE_IDENTITY_LEN;
    device_id->source_device = (uint8)source;
    device_id->target_device = (uint8)destination;

    return GSTK_SUCCESS;
} /* gstk_packer_dev_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_result_tlv

DESCRIPTION

  This function populates the passed in result tlv pointer

PARAMETERS
  command_result: [Input] Indicates general result
  additional_info: [Input] Indicates any additional info for the result
  result: [Input/Output] Pointers to which the result to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_result_tlv(
  gstk_general_result_enum_type         command_result,
  gstk_additional_info_ptr_type         additional_info,
  gstk_result_tag_tlv_type              * result
)
{

   MSG_HIGH("** Packing result, 0x%x", command_result, 0, 0);

   if(result == NULL)
   {
      MSG_ERROR("Result Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   /* initialize additional result pointer to null */
   result->additional_info = NULL;

   result->result_tag = (int)GSTK_RESULT_TAG;
   /* variable length result */
   result->result_tag_length = additional_info.length + 1;
   result->result = (uint8)command_result;

   result->additional_info = NULL;
   /* can be multiple additional_info */
   if ((additional_info.length != 0) && (additional_info.additional_info_ptr != NULL)) {
     result->additional_info =
      (uint8*)gstk_malloc(additional_info.length * sizeof(byte) );

     if (result->additional_info == NULL) {
       MSG_ERROR("Result Err: memory allocation fail", 0, 0, 0);
       return GSTK_MEMORY_ERROR;
     }
     memcpy(result->additional_info, 
            additional_info.additional_info_ptr, 
            additional_info.length);
   }
   return GSTK_SUCCESS;
} /* gstk_packer_result_tlv */


/*===========================================================================
FUNCTION gstk_packer_text_string_tlv

DESCRIPTION

  This function populates the passed in text tlv pointer

PARAMETERS
  dcs: [Input] Indicates the text string coding scheme
  data_len: [Input] Indicates string length
  data: [Input] Indicates the string
  string_tlv: [Input/Output] Pointers to which the text string to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_text_string_tlv(
       tags_enum_type         tag,
       uint8                  dcs,
       uint8                  data_len,
       const uint8            *data,
       gstk_string_tlv_type   *string_tlv
)
{

   gstk_status_enum_type gstk_status = GSTK_SUCCESS;

   MSG_HIGH("** Packing text string", 0, 0, 0);

   if(string_tlv == NULL)
   {
      MSG_ERROR("String TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   string_tlv->tag = (uint8)tag;
   string_tlv->length = data_len + 1;

   /* get data coding scheme */
   string_tlv->data_coding_scheme = dcs;

   if ((data_len > 0) && (data!= NULL)) {
     if (string_tlv->text != NULL) {
        memcpy(string_tlv->text,
               data,
               data_len);
     }
     else {
       MSG_ERROR("Text String Err: Null Text ptr", 0, 0, 0);
       gstk_status = GSTK_MEMORY_ERROR;
     }
   }

   return gstk_status;
} /* gstk_packer_text_string_tlv */


/*===========================================================================
FUNCTION gstk_packer_alpha_tlv

DESCRIPTION

  This function populates the passed in alpha tlv pointer

PARAMETERS
  data: [Input] Indicates the alpha text
  string_tlv: [Input/Output] Pointers to which the alpha to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_alpha_tlv(
       const gstk_string_type *data,
       gstk_alpha_tlv_type    *string_tlv
)
{

   gstk_status_enum_type gstk_status = GSTK_SUCCESS;

   MSG_HIGH("** Packing alpha", 0, 0, 0);

   if(string_tlv == NULL)
   {
      MSG_ERROR("String TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   string_tlv->tag = (int)GSTK_ALPHA_IDENTIFIER_TAG;
   string_tlv->length = data->length;

   if (data->length > 0) {
     if (string_tlv->alpha != NULL) {
        memcpy(string_tlv->alpha,
               data->text,
               data->length);
     }
     else {
       MSG_ERROR("Alpha Err: Null Text ptr", 0, 0, 0);
       gstk_status = GSTK_MEMORY_ERROR;
     }
   }

   return gstk_status;
} /* gstk_packer_alpha_tlv */


#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT 
/*===========================================================================
FUNCTION gstk_packer_cdma_location_info_tlv

DESCRIPTION

  This function populates the passed in location info tlv pointer

PARAMETERS
  data: [Input] Indicates the location info
  location_info_tlv: [Input/Output] Pointers to which the location info
                                    to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cdma_location_info_tlv(
       const gstk_cdma_location_info_type        *data,
       gstk_cdma_location_information_tlv_type   *location_info_tlv
)
{

   MSG_HIGH("** Packing location info", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(location_info_tlv == NULL)
   {
      MSG_ERROR("Location Info TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   location_info_tlv->tag = int32touint8(GSTK_LOCATION_INFORMATION_TAG);
   location_info_tlv->length = GSTK_CDMA_LOCATION_INFORMATION_LEN;

   memcpy(location_info_tlv->cdma_loc_info.mcc,
          data->mcc,
          2);
   location_info_tlv->cdma_loc_info.imsi_11_12 = 
      data->imsi_11_12;
   memcpy(location_info_tlv->cdma_loc_info.sid,
          data->sid_info.sid,
          2);
   memcpy(location_info_tlv->cdma_loc_info.nid,
          data->nid_info.nid,
          2);
   memcpy(location_info_tlv->cdma_loc_info.base_id,
          data->base_id_info.base_id,
          2);
   memcpy(location_info_tlv->cdma_loc_info.base_lat,
          data->base_lat,
          3);
   memcpy(location_info_tlv->cdma_loc_info.base_long,
          data->base_long,
          3);

   return GSTK_SUCCESS;
} /* gstk_packer_cdma_location_info_tlv */
#endif /*#ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */


/*===========================================================================
FUNCTION gstk_packer_location_info_tlv

DESCRIPTION

  This function populates the passed in location info tlv pointer

PARAMETERS
  data: [Input] Indicates the location info
  location_info_tlv: [Input/Output] Pointers to which the location info
                                    to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_location_info_tlv(
       const gstk_location_info_rsp_type    *data,
       gstk_location_information_tlv_type   *location_info_tlv
)
{

   MSG_HIGH("** Packing location info", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(location_info_tlv == NULL)
   {
      MSG_ERROR("Location Info TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   location_info_tlv->tag = (int)GSTK_LOCATION_INFORMATION_TAG;
   location_info_tlv->length = GSTK_LOCATION_INFORMATION_LEN;

   memcpy(location_info_tlv->location_info.cell_id,
          data->cell_id,
          2);
   memcpy(location_info_tlv->location_info.loc_area_code,
          data->loc_area_code,
          2);
   memcpy(location_info_tlv->location_info.mcc_and_mnc,
          data->mcc_and_mnc,
          3);

   if (data->cell_id_len == 4)
   {
     memcpy(location_info_tlv->location_info.ext_cell_id,
          &data->cell_id[2],
          2);
     location_info_tlv->length += 2;
   }

   return GSTK_SUCCESS;
} /* gstk_packer_location_info_tlv */


/*===========================================================================
FUNCTION gstk_packer_imei_tlv

DESCRIPTION

  This function populates the passed in imei tlv pointer

PARAMETERS
  data: [Input] Indicates the imei
  imei_tlv: [Input/Output] Pointers to which the imei to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_imei_tlv(
       const uint8          *data,
       gstk_imei_tlv_type   *imei_tlv
)
{
   MSG_HIGH("** Packing imei", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   if(imei_tlv == NULL)
   {
      MSG_ERROR("IMEI TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   imei_tlv->tag = (int)GSTK_IMEI_TAG;
   imei_tlv->length = GSTK_IMEI_LEN;

   memcpy(imei_tlv->imei,
          data,
          GSTK_IMEI_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_imei_tlv */


#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
/*===========================================================================
FUNCTION gstk_packer_esn_tlv

DESCRIPTION

  This function populates the passed in esn tlv pointer

PARAMETERS
  data: [Input] Indicates the esn
  esn_tlv: [Input/Output] Pointers to which the esn to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_esn_tlv(
       const uint8         *data,
       gstk_esn_tlv_type   *esn_tlv
)
{
   MSG_HIGH("** Packing esn", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   if(esn_tlv == NULL)
   {
      MSG_ERROR("ESN TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   esn_tlv->tag = (int)GSTK_ESN_TAG;
   esn_tlv->length = GSTK_ESN_LEN;

   memcpy(esn_tlv->esn,
          data,
          GSTK_ESN_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_esn_tlv */


/*===========================================================================
FUNCTION gstk_packer_meid_tlv

DESCRIPTION

  This function populates the passed in meid tlv pointer

PARAMETERS
  data: [Input] Indicates the meid
  meid_tlv: [Input/Output] Pointers to which the meid to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_meid_tlv(
       const uint8          *data,
       gstk_meid_tlv_type   *meid_tlv
)
{
   MSG_HIGH("** Packing meid", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   if(meid_tlv == NULL)
   {
      MSG_ERROR("MEID TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   meid_tlv->tag = (int)GSTK_MEID_TAG;
   meid_tlv->length = GSTK_MEID_LEN;

   memcpy(meid_tlv->meid,
          data,
          GSTK_MEID_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_meid_tlv */
#endif /* FEATURE_CCAT */


/*===========================================================================
FUNCTION gstk_packer_imeisv_tlv

DESCRIPTION

  This function populates the passed in imeisv tlv pointer

PARAMETERS
  data: [Input] Indicates the imeisv
  imeisv_tlv: [Input/Output] Pointers to which the imeisv to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_imeisv_tlv(
       const uint8            *data,
       gstk_imeisv_tlv_type   *imeisv_tlv
)
{
   MSG_HIGH("** Packing imeisv", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   if(imeisv_tlv == NULL)
   {
      MSG_ERROR("IMEISV TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   imeisv_tlv->tag = (int)GSTK_IMEISV_TAG;
   imeisv_tlv->length = GSTK_IMEI_SV_LEN;

   memcpy(imeisv_tlv->imeisv,
          data,
          GSTK_IMEI_SV_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_imeisv_tlv */
#endif /* #ifdef FEATURE_CAT_REL6 */


/*===========================================================================
FUNCTION gstk_packer_access_technology_tlv

DESCRIPTION

  This function populates the passed in access technology tlv pointer

PARAMETERS
  data                  : [Input] Indicates the iccess technology 
  iccess_technology_tlv: [Input/Output] Pointers to which the  access technology needs to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_access_technology_tlv(
  const uint8                       *data_ptr,
  gstk_access_technology_tlv_type   *access_tech_tlv
)
{
  MSG_HIGH("** Packing Access Technology", 0, 0, 0);

  if(data_ptr == NULL)
  {
    MSG_ERROR("Data_ptr is NULL", 0, 0, 0);
    return GSTK_ERROR;
  }

  if(access_tech_tlv == NULL)
  {
    MSG_ERROR("access_tech_tlv is NULL", 0, 0, 0);
    return GSTK_ERROR;
  }

  access_tech_tlv->tag    = (int)GSTK_ACCESS_TECHNOLOGY_TAG;
  access_tech_tlv->length =  GSTK_ACCESS_TECHNOLOGY_LEN;
  access_tech_tlv->access_technology = *data_ptr;
  return GSTK_SUCCESS; 
}


#ifdef FEATURE_CAT_REL6
/*===========================================================================
FUNCTION gstk_packer_battery_status_tlv

DESCRIPTION

  This function populates the passed in battery status tlv pointer

PARAMETERS
  data                  : [Input] Indicates the battery status 
  battery_status_tlv: [Input/Output] Pointers to which the battery status needs to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_battery_status_tlv(
  const uint8                    *data_ptr,
  gstk_battery_status_tlv_type   *battery_status_tlv
)
{
  MSG_HIGH("** Packing Battery Status", 0, 0, 0);

  if(data_ptr == NULL)
  {
    MSG_ERROR("Data_ptr is NULL", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }

  if(battery_status_tlv == NULL)
  {
    MSG_ERROR("battery_status_tlv is NULL", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }

  battery_status_tlv->tag            = (int)GSTK_BATTERY_STATUS_TAG;
  battery_status_tlv->length         =  GSTK_BATTERY_STATUS_LEN;
  battery_status_tlv->battery_status = *data_ptr;

  return GSTK_SUCCESS; 
}
#endif /* #ifdef FEATURE_CAT_REL6 */


/*===========================================================================
FUNCTION gstk_packer_nmr_tlv

DESCRIPTION

  This function populates the passed in nmr tlv pointer

PARAMETERS
  data: [Input] Indicates the nmr
  nmr_tlv: [Input/Output] Pointers to which the nmr to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_nmr_tlv(
       const uint8          *data,
       gstk_nmr_tlv_type    *nmr_tlv
)
{

   MSG_HIGH("** Packing nmr", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(nmr_tlv == NULL)
   {
      MSG_ERROR("NMR TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   nmr_tlv->tag = (int)GSTK_NETWORK_MEASUREMENT_RESULTS_TAG;
   nmr_tlv->length = GSTK_NMR_LEN;

   memcpy(nmr_tlv->nmr,
          data,
          GSTK_NMR_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_nmr_tlv */


/*===========================================================================
FUNCTION gstk_packer_bcch_list_tlv

DESCRIPTION

  This function populates the passed in bcch list tlv pointer

PARAMETERS
  data: [Input] Indicates the bcch list
  bcch_list_tlv: [Input/Output] Pointers to which the bcch list to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_bcch_list_tlv(
       uint8                        data_len,
       const uint8                  *data,
       gstk_bcch_ch_list_tlv_type   *bcch_list_tlv
)
{
   MSG_HIGH("** Packing bcch", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(bcch_list_tlv == NULL)
   {
      MSG_ERROR("BCCH LIST TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   bcch_list_tlv->tag = (int)GSTK_BCCH_CHANNEL_LIST_TAG;
   bcch_list_tlv->length = data_len;

   if(data_len > 0) {
     if (bcch_list_tlv->channel_list != NULL) {
       memcpy(bcch_list_tlv->channel_list,
              data,
              data_len);
       return GSTK_SUCCESS;
     }
     else {
       MSG_ERROR("BCCH List Err: Null Channel List ptr", 0, 0, 0);
       return GSTK_NULL_INPUT_PARAM;
     }
   }
   return GSTK_SUCCESS;
} /* gstk_packer_bcch_list_tlv */


#ifdef FEATURE_GSTK_NMR_UTRAN_SUPPORT
/*===========================================================================
FUNCTION gstk_packer_nmr_utran_tlv

DESCRIPTION
  This function packs the nmr data in the nmr utran tlv pointer

PARAMETERS
  length:            [Input] Length of data
  data_ptr:          [Input] Pointer from which nmr data is to be copied
  nmr_utran_tlv_ptr: [Input/Output] Pointers to which the nmr is to be 
                     populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NULL_INPUT_PARAM:  Null input param
    GSTK_BAD_PARAM:         Bad input paramter
    GSTK_SUCCESS:           Success

COMMENTS
  dest buffer length field ie nmr_utran_tlv_ptr->length is assigned 
  by the caller function

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_nmr_utran_tlv(
         uint32                        length,
         const uint8                   *data_ptr,
         gstk_nmr_utran_tlv_type       *nmr_utran_tlv_ptr
)
{
  MSG_HIGH("** Packing utran nmr", 0, 0, 0);

  if(data_ptr == NULL)
  {
     MSG_ERROR("NMR TLV Err: data_ptr NULL ptr ", 0, 0, 0);
     return GSTK_NULL_INPUT_PARAM;
  }
  if(nmr_utran_tlv_ptr == NULL)
  {
     MSG_ERROR("NMR TLV Err: nmr_utran_tlv_ptr NULL", 0, 0, 0);
     return GSTK_NULL_INPUT_PARAM;
  }
  if(length == 0 || length > 0xFF)
  {
     MSG_ERROR("NMR TLV Err: length is 0 or > 0xFF", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  /* nmr_utran_tlv_ptr->length value has already been assigned by the caller function */
  if(nmr_utran_tlv_ptr->length < length)
  {
     MSG_ERROR("NMR TLV Err: deat length is <  src length", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  nmr_utran_tlv_ptr->tag = (int)GSTK_NETWORK_MEASUREMENT_RESULTS_TAG;

  if (nmr_utran_tlv_ptr->measurement_report_ptr == NULL) 
  {
      MSG_ERROR("NMR UTRAN Err: Measurement report ptr", 0, 0, 0);
      return GSTK_NULL_INPUT_PARAM;
  }
  memcpy(nmr_utran_tlv_ptr->measurement_report_ptr,
         data_ptr,
         length);
  return GSTK_SUCCESS;
}/* gstk_packer_nmr_utran_tlv */
#endif /* FEATURE_GSTK_NMR_UTRAN_SUPPORT */


/*===========================================================================
FUNCTION gstk_packer_timing_advance_tlv

DESCRIPTION

  This function populates the passed in timing advance tlv pointer

PARAMETERS
  data: [Input] Indicates the timing advance list
  timing_adv_tlv: [Input/Output] Pointers to which the timing advance to be
                                 populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_timing_advance_tlv(
       const gstk_time_advance_rsp_type     *data,
       gstk_timing_advance_tlv_type         *timing_adv_tlv
)
{
   MSG_HIGH("** Packing timing advance", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(timing_adv_tlv == NULL)
   {
      MSG_ERROR("TIMING ADV TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   timing_adv_tlv->tag = (int)GSTK_TIMING_ADVANCE_TAG;
   timing_adv_tlv->length = GSTK_TIMING_ADVANCE_LEN;

   timing_adv_tlv->timing_advance.me_status = data->me_status;
   timing_adv_tlv->timing_advance.timing_advance = data->timing_advance;

   return GSTK_SUCCESS;

} /* gstk_packer_timing_advance_tlv */


/*===========================================================================
FUNCTION gstk_packer_lang_sel_tlv

DESCRIPTION

  This function populates the passed in the language selection tlv pointer

PARAMETERS
  data: [Input] Indicates the language selection
  imei_tlv: [Input/Output] Pointers to which the language selection to be
                           populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_lang_sel_tlv(
       const uint8          *data,
       gstk_lang_tlv_type   *lang_tlv
)
{

   MSG_HIGH("** Packing lang selection", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(lang_tlv == NULL)
   {
      MSG_ERROR("Lang TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   lang_tlv->tag = (int)GSTK_LANGUAGE_TAG;
   lang_tlv->length = GSTK_LANGUAGE_LEN;

   memcpy(lang_tlv->lang_code,
          data,
          GSTK_LANGUAGE_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_lang_sel_tlv */


/*===========================================================================
FUNCTION gstk_packer_date_time_tlv

DESCRIPTION

  This function populates the passed in the date/time/timezone tlv pointer

PARAMETERS
  data: [Input] Indicates the date, time and time zone
  time_tlv: [Input/Output] Pointers to which the time related info to be
                           populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_date_time_tlv(
       const gstk_date_time_rsp_type   *data,
       gstk_date_time_tlv_type         *time_tlv
)
{
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
   int8 ltm_off = 0;
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */

   MSG_HIGH("** Packing date time", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(time_tlv == NULL)
   {
      MSG_ERROR("Time TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   time_tlv->tag = (int)GSTK_DATE_TIME_AND_TIME_ZONE_TAG;
   time_tlv->length = GSTK_DATE_TIME_AND_TIME_ZONE_LEN;

   time_tlv->date_time.day = data->day;
   time_tlv->date_time.hour = data->hour;
   time_tlv->date_time.minute = data->minute;
   time_tlv->date_time.month = data->month;
   time_tlv->date_time.second = data->second;
   time_tlv->date_time.year = data->year;
   time_tlv->date_time.zone = data->zone;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
 /* Since the time zone is in 30 min intervals, it has to be converted to 15 min intervals
  * according to spec. Also the spec says that in the first of the two semi-octets,
  * the first bit (bit 3 of the seventh octet of TP Service Center Time stamp field
  * represents whether it is positive or negative (0 if positive, 1 if negative */
    if ((gstk_next_curr_rat == GSTK_ACCESS_TECH_CDMA) && (data->zone != 0xFF))
    {
      ltm_off = (int8)(data->zone & 0x3F);
      if (ltm_off >= 32)
      {
        ltm_off -= 64;
        ltm_off = (int8)(-(ltm_off * 2));
        time_tlv->date_time.zone = ((uint8)(ltm_off) / 10);
        time_tlv->date_time.zone += (((uint8)(ltm_off) % 10) << 4);
        time_tlv->date_time.zone |= 0x08;
      }
      else
      {
        ltm_off = (int8)(ltm_off*2);
        time_tlv->date_time.zone = ((uint8)(ltm_off) / 10);
        time_tlv->date_time.zone += (((uint8)(ltm_off) % 10) << 4);
      }
      MSG_HIGH("CDMA TP-Service-Centre-Time-Stamp is 0x%x",time_tlv->date_time.zone, 0, 0);
    }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


   return GSTK_SUCCESS;
} /* gstk_packer_date_time_tlv */


/*===========================================================================
FUNCTION gstk_packer_address_tlv

DESCRIPTION

  This function populates the passed in the address tlv pointer

PARAMETERS
  tag: [Input] Indicates: ADDRESS_TAG or SS_STRING_TAG
  data: [Input] Indicates the address
  address_tlv: [Input/Output] Pointers to which the
                              gstk_address_tag_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_address_tlv(
       tags_enum_type            tag,
       const gstk_address_type   *data,
       gstk_address_tag_tlv_type *address_tlv)
{
  int   i = 0;
  uint8 temp_ton = 0;
  uint8 lsb = 0;
  uint8 msb = 0;

  MSG_HIGH("** Packing address", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(address_tlv == NULL)
   {
      MSG_ERROR("Address TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

  address_tlv->tag = (uint8)tag;
  address_tlv->length = 0;

  if (data->length > 0) {
    temp_ton = int32touint8((int)data->TON << GSTK_TON_SHIFT_VALUE);
    address_tlv->ton = temp_ton | ((int)data->NPI & GSTK_NPI_MASK_VALUE) | 0x80 ;

    i = 0;
    /* If international number then ignore '+'*/
    if(data->address[i] == 0x2B)
    {
      if(data->TON == GSTK_INTERNATIONAL_NUM)
      {
        i++;
      }
      else
      {
        MSG_ERROR("Invalid Phone Number", 0, 0, 0);
        return GSTK_ERROR;
      }
    }

    while((i < data->length)
          &&
          (address_tlv->length < GSTK_MAX_BCD_ADDRESS_LEN))
    {
    
      if(i+1 < data->length) {
        if ((data->address[i+1] >= 0x30) && (data->address[i+1] < 0x40))
        {
          msb = ((int32touint32(data->address[i+1]-0x30) << 0x04) & 0xF0);
        }
        else {
          switch (data->address[i+1]) {
            case 0x2A: /* * */
              msb = 0xA0;
              break;
            case 0x23: /* # */
              msb = 0xB0;
              break;
            default: /* passed in as 0x01 e.g. */
              msb = (uint8)(((data->address[i+1]) << 0x04) & 0xF0);
              break;
          }
        }
        if ((data->address[i] >= 0x30) && (data->address[i] < 0x40))
        {
          lsb = (uint8)((data->address[i]-0x30) & 0x0F);
        }
        else {
          switch (data->address[i]) {
            case 0x2A: /* * */
              lsb = 0x0A;
              break;
            case 0x23: /* # */
              lsb = 0x0B;
              break;
            default: /* passed in as 0x01 e.g. */
              lsb = (uint8)((data->address[i]) & 0x0F);
              break;
          }
        }
        address_tlv->address[address_tlv->length] =
            (uint8)(msb | lsb);
      }
      else { /* last digit, pad 0x0F */
        if ((data->address[i] >= 0x30) && (data->address[i] < 0x40))
        {
          lsb = (uint8)((data->address[i]-0x30) & 0x0F);
        }
        else {
          switch (data->address[i]) {
            case 0x2A: /* * */
              lsb = 0x0A;
              break;
            case 0x23: /* # */
              lsb = 0x0B;
              break;
            default: /* passed in as 0x01 e.g. */
              lsb = (uint8)((data->address[i]) & 0x0F);
              break;
          }
        }
        address_tlv->address[address_tlv->length] =
          0xF0 | lsb;
      }
      i+=2;
      address_tlv->length++;
    }

    if (address_tlv->length == GSTK_MAX_BCD_ADDRESS_LEN)
    {
      MSG_ERROR("BCD buffer too small!", 0, 0, 0);
      gstk_util_dump_byte_array(
        "data->address",
        data->address,
        data->length);
      return GSTK_ERROR;
    }

    address_tlv->length++; /* add the TON byte */
  }
  return GSTK_SUCCESS;
} /* gstk_packer_address_tlv */


/*===========================================================================
FUNCTION gstk_packer_subaddress_tlv

DESCRIPTION

  This function populates the passed in the subaddress tlv pointer

PARAMETERS
  data: [Input] Indicates the subaddress
  subaddress_tlv: [Input/Output] Pointers to which the
                              gstk_subaddress_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_subaddress_tlv(
       const gstk_subaddress_type   *data,
       gstk_subaddress_tlv_type     *subaddress_tlv)
{

  MSG_HIGH("** Packing subaddress", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(subaddress_tlv == NULL)
   {
      MSG_ERROR("Subaddress TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

  subaddress_tlv->tag = (int)GSTK_CALLED_PARTY_SUB_ADDRESS_TAG;
  subaddress_tlv->length = data->length;
  memcpy(subaddress_tlv->subaddress, data->subaddress, data->length);
  return GSTK_SUCCESS;
} /* gstk_packer_subaddress_tlv */


/*===========================================================================
FUNCTION gstk_packer_ccp_tlv

DESCRIPTION

  This function populates the passed in the ccp tlv pointer

PARAMETERS
  data: [Input] Indicates the ccp
  ccp_tlv: [Input/Output] Pointers to which the ccp tlv info to be
                          populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_ccp_tlv(
       const gstk_cap_config_type   *data,
       gstk_cap_config_tlv_type     *ccp_tlv)
{

  MSG_HIGH("** Packing ccp", 0, 0, 0);

  if(data == NULL)
  {
     MSG_ERROR("Data Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  if(ccp_tlv == NULL)
  {
      MSG_ERROR("CCP_TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
  }

  ccp_tlv->tag = (int)GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG;
  ccp_tlv->length = data->length;
  memcpy(ccp_tlv->ccp, data->ccp, data->length);
  return GSTK_SUCCESS;
} /* gstk_packer_ccp_tlv */


/*===========================================================================
FUNCTION gstk_packer_bc_repeat_ind_tlv

DESCRIPTION

  This function populates the passed in the bc repeat indicator tlv pointer

PARAMETERS
  data: [Input] Indicates the bc repeat indicator
  ccp_tlv: [Input/Output] Pointers to which the bc repeat indicator tlv info
                          to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_bc_repeat_ind_tlv(
       gstk_bc_repeat_ind_enum_type         data,
       gstk_bc_repeat_ind_tlv_type          *bc_repeat_ind_tlv)
{

  MSG_HIGH("** Packing bc repeat ind", 0, 0, 0);

  if(bc_repeat_ind_tlv == NULL)
  {
     MSG_ERROR("BC_REPEAT_IND TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  bc_repeat_ind_tlv->tag = (int)GSTK_BC_REPEAT_INDICATOR_TAG;
  bc_repeat_ind_tlv->length = GSTK_BC_REPEAT_IND_LEN;
  bc_repeat_ind_tlv->bc_repeat_indicator = (byte)data;

  return GSTK_SUCCESS;
} /* gstk_packer_bc_repeat_ind_tlv */


/*===========================================================================
FUNCTION gstk_packer_cc_req_action_tlv

DESCRIPTION

  This function populates the passed in the call control requested
  action tlv pointer

PARAMETERS
  data: [Input] Indicates the call control requested action
  cc_req_action_tlv: [Input/Output] Pointers to which the
                                    gstk_cc_req_action_tlv_type info to be
                                    populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cc_req_action_tlv(
       const gstk_cc_req_action_type     *data,
       gstk_cc_req_action_tlv_type       *cc_req_action_tlv)
{

  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  MSG_HIGH("** Packing CC req action", 0, 0, 0);

  if(data == NULL)
  {
     MSG_ERROR("Data Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }
  if(cc_req_action_tlv == NULL)
  {
     MSG_ERROR("CC_REQ_ACTION TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  cc_req_action_tlv->tag = (int)GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG;
  cc_req_action_tlv->length = 0;

  switch(data->call_type) {
  case GSTK_VOICE:
    MSG_MED("Voice call", 0, 0, 0);
    if (data->gstk_address_string.voice_info.address.length != 0) {
      gstk_status = gstk_packer_address_tlv(GSTK_ADDRESS_TAG,
                               &data->gstk_address_string.voice_info.address,
                               &cc_req_action_tlv->gstk_address_string_tlv.address);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: Address error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->gstk_address_string_tlv.address.length + 2;
    }
    if (data->gstk_address_string.voice_info.ccp1.length != 0) {
      gstk_status = gstk_packer_ccp_tlv(&data->gstk_address_string.voice_info.ccp1,
                             &cc_req_action_tlv->ccp1);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: CCP 1 error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->ccp1.length
                                  + 2;
    }
    if (data->gstk_address_string.voice_info.ccp2.length != 0) {
      gstk_status = gstk_packer_ccp_tlv(&data->gstk_address_string.voice_info.ccp2,
                             &cc_req_action_tlv->ccp2);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: CCP 2 error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->ccp2.length
                                  + 2;
      /* bc repeat indicator is present if cpp2 is presented */
      gstk_status = gstk_packer_bc_repeat_ind_tlv(data->bc_repeat_ind,
                                       &cc_req_action_tlv->bc_repeat_ind);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: BC Repeat Ind error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->bc_repeat_ind.length
                                  + 2;
    }
    if (data->gstk_address_string.voice_info.subaddress.length != 0) {
      gstk_status = gstk_packer_subaddress_tlv(&data->gstk_address_string.voice_info.subaddress,
                                &cc_req_action_tlv->subaddress);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: Subaddress error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->subaddress.length
                                  + 2;
    }
    break;
  case GSTK_USSD:
    MSG_MED("USSD call", 0, 0, 0);
    if (data->gstk_address_string.ussd_string.length != 0) {
      /* malloc the space for ussd string */
      cc_req_action_tlv->gstk_address_string_tlv.ussd_string.text =
          gstk_malloc(data->gstk_address_string.ussd_string.length);
      if (cc_req_action_tlv->gstk_address_string_tlv.ussd_string.text != NULL) {
        gstk_status = gstk_packer_text_string_tlv(GSTK_USSD_STRING_TAG,
                                     data->gstk_address_string.ussd_string.dcs,
                                     data->gstk_address_string.ussd_string.length,
                                     data->gstk_address_string.ussd_string.text,
                                     &cc_req_action_tlv->gstk_address_string_tlv.ussd_string);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("CC Req Err: USSD Text error 0x%x", gstk_status, 0, 0);
          cc_req_action_tlv->length = 0; 
          return gstk_status;
        }
        /* length for CC request action */
        cc_req_action_tlv->length = cc_req_action_tlv->length +
                                    cc_req_action_tlv->gstk_address_string_tlv.ussd_string.length
                                    + 2;
      }
      else {
        MSG_ERROR("CC Req Err: USSD Text null Ptr", 0, 0, 0);
        return GSTK_MEMORY_ERROR;
      }
    }
    break;
  case GSTK_SS:
    MSG_MED("SS call", 0, 0,0);
    if (data->gstk_address_string.ss_string.length != 0) {
      gstk_status = gstk_packer_address_tlv(GSTK_SS_STRING_TAG,
                               &data->gstk_address_string.ss_string,
                               &cc_req_action_tlv->gstk_address_string_tlv.ss_string);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: SS String error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->gstk_address_string_tlv.ss_string.length
                                  + 2;
    }
    break;
  default:
    MSG_ERROR("CC Req Err: unkown call type", 0, 0, 0);
    return GSTK_ERROR;
  }

  /* alpha */
  if (data->alpha.length != 0) {
    /* malloc alpha space */
    cc_req_action_tlv->alpha.alpha = gstk_malloc(data->alpha.length);
    if (cc_req_action_tlv->alpha.alpha != NULL) {
      gstk_status  = gstk_packer_alpha_tlv(&data->alpha,
                               &cc_req_action_tlv->alpha);
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("CC Req Err: Alpha error 0x%x", gstk_status, 0, 0);
        cc_req_action_tlv->length = 0; 
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                    cc_req_action_tlv->alpha.length
                                    + 2;
    }
    else {
      MSG_ERROR("CC Req Err: Alpha null Ptr", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }
  }

  return GSTK_SUCCESS;
} /* gstk_packer_cc_req_action_tlv*/


/*===========================================================================
FUNCTION gstk_packer_duration_tlv

DESCRIPTION

  This function populates the passed in duration tlv pointer

PARAMETERS
  data: [Input] Indicates the duration
  duration_tlv: [Input/Output] Pointers to which the duration
                                    to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_duration_tlv(
       const gstk_duration_type    *data,
       gstk_duration_tlv_type      *duration_tlv
)
{

   MSG_HIGH("** Packing duration", 0, 0, 0);

   if(data == NULL)
   {
      MSG_ERROR("Data Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if(duration_tlv == NULL)
   {
      MSG_ERROR("Duration TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   duration_tlv->tag = (int)GSTK_DURATION_TAG;
   duration_tlv->length = GSTK_DURATION_LEN;

   duration_tlv->time_interval = data->length;
   duration_tlv->time_unit = data->units;

   return GSTK_SUCCESS;
} /* gstk_packer_duration_tlv */


/*===========================================================================
FUNCTION gstk_packer_item_id_tlv

DESCRIPTION

  This function populates the passed in the item id tlv pointer

PARAMETERS
  chosen_item_identifier: [Input] Indicates the item id
  item_id_tlv: [Input/Output] Pointers to which the item id info to be
                           populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_item_id_tlv(
       uint8                            chosen_item_identifier,
       gstk_item_identifier_tlv_type   *item_id_tlv
)
{
   MSG_HIGH("** Packing item id", 0, 0, 0);

   if(item_id_tlv == NULL)
   {
      MSG_ERROR("ITEM ID TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }
   item_id_tlv->tag = (int)GSTK_ITEM_IDENTIFIER_TAG;
   item_id_tlv->length = GSTK_ITEM_IDENTIFIER_LEN;

   item_id_tlv->identifier = chosen_item_identifier;


   return GSTK_SUCCESS;
} /* gstk_packer_item_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_help_request_tlv

DESCRIPTION

  This function populates the passed in the help request tlv pointer

PARAMETERS
  data: [Input] Indicates the help requested info
  help_tlv: [Input/Output] Pointers to which the help request related info
                           to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_help_request_tlv(
       boolean                          data,
       gstk_help_request_tlv_type       *help_tlv
)
{
   MSG_HIGH("** Packing help request", 0, 0, 0);

   if(help_tlv == NULL)
   {
      MSG_ERROR("Help TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

   if (data) {
     help_tlv->tag = (int)GSTK_HELP_REQUEST_TAG;
     help_tlv->length = GSTK_HELP_REQUEST_LEN;
   }
   else {
     help_tlv->length = 0;
   }

   return GSTK_SUCCESS;
} /* gstk_packer_help_request_tlv */


/*===========================================================================
FUNCTION gstk_packer_transaction_id_tlv

DESCRIPTION

  This function populates the passed in the transaction id tlv pointer

PARAMETERS
  data: [Input] Indicates the transaction id
  transaction_id_tlv: [Input/Output] Pointers to which the
                              gstk_transaction_id_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_transaction_id_tlv(
       const gstk_transaction_id_type   *data,
       gstk_transaction_id_tlv_type     *transaction_id_tlv)
{

  MSG_HIGH("** Packing transaction id", 0, 0, 0);

   if(data == NULL)
   {
     MSG_ERROR("Data Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
   }

   if(transaction_id_tlv == NULL)
   {
      MSG_ERROR("TRANSACTION ID TLV Err: NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
   }

  transaction_id_tlv->tag = (int)GSTK_TRANSACTION_IDENTIFIER_TAG;
  transaction_id_tlv->length = data->length;
  if(data->length > 0) {
    if(transaction_id_tlv->transaction_list != NULL) {
      memcpy(transaction_id_tlv->transaction_list, data->transaction_list, data->length);
    }
    else {
      MSG_ERROR("Trans ID Err: Trans List Null ptr", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_transaction_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_evt_list_tlv

DESCRIPTION

  This function populates the passed in the event list tlv pointer

PARAMETERS
  data: [Input] Indicates one event list
  evt_list_tlv: [Input/Output] Pointers to which the
                              gstk_evt_list_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_evt_list_tlv(
       gstk_evt_list_code_enum_type       data,
       gstk_evt_list_tlv_type             *evt_list_tlv)
{

  MSG_HIGH("** Packing evt list", 0, 0, 0);

  if(evt_list_tlv == NULL)
  {
     MSG_ERROR("EVT LIST TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  evt_list_tlv->tag = (int)GSTK_EVENT_LIST_TAG;
  evt_list_tlv->length = 1;
  /* only 1 event */
  evt_list_tlv->event_list[0] = (uint8)data;

  return GSTK_SUCCESS;
} /* gstk_packer_evt_list_tlv */


/*===========================================================================
FUNCTION gstk_packer_cause_tlv

DESCRIPTION

  This function populates the passed in the cause tlv pointer

PARAMETERS
  data: [Input] Indicates the cause
  cause_tlv: [Input/Output] Pointers to which the
                              gstk_cause_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cause_tlv(
       const gstk_cause_type   *data,
       gstk_cause_tlv_type     *cause_tlv)
{

  MSG_HIGH("** Packing cause", 0, 0, 0);

  if(data == NULL)
  {
     MSG_ERROR("Data Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  if(cause_tlv == NULL)
  {
     MSG_ERROR("CAUSE TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }
  cause_tlv->tag = (int)GSTK_CAUSE_TAG;
  cause_tlv->length = data->length;
  if(data->length > 0) {
    if(cause_tlv->cause != NULL) {
      memcpy(cause_tlv->cause, data->cause, data->length);
    }
    else {
      MSG_ERROR("Cause Err: Cause Null ptr", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_cause_tlv */


/*===========================================================================
FUNCTION gstk_packer_location_status_tlv

DESCRIPTION

  This function populates the passed in the location status tlv pointer

PARAMETERS
  data: [Input] Indicates the location status
  location_status_tlv: [Input/Output] Pointers to which the
                              gstk_location_status_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_location_status_tlv(
       gstk_location_status_enum_type         data,
       gstk_location_status_tlv_type          *location_status_tlv)
{
  MSG_HIGH("** Packing location status", 0, 0, 0);

  if(location_status_tlv == NULL)
  {
     MSG_ERROR("Location status TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }
  location_status_tlv->tag = (int)GSTK_LOCATION_STATUS_TAG;
  location_status_tlv->length = GSTK_LOCATION_STATUS_LEN;

  location_status_tlv->location_status = (uint8)data;

  return GSTK_SUCCESS;
} /* gstk_packer_location_status_tlv */


/*===========================================================================
FUNCTION gstk_packer_sms_tpdu_tlv

DESCRIPTION

  This function populates the passed in the sms tpdu tlv pointer

PARAMETERS
  is_cdma_tpdu: [Input] Indicates whether the tag should be CDMA or GSM/WCDMA
                        TPDU tag
  data: [Input] Indicates the sms tpdu
  tpdu_tlv: [Input/Output] Pointers to which the
                              gstk_sms_tpdu_tag_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_sms_tpdu_tlv(
       boolean                             is_cdma_tpdu,
       const gstk_sms_tpdu_type            *data,
       gstk_sms_tpdu_tag_tlv_type          *tpdu_tlv)
{
  MSG_HIGH("** Packing sms tpdu", 0, 0, 0);

  if(data == NULL)
  {
     MSG_ERROR("Data Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  if(tpdu_tlv == NULL)
  {
     MSG_ERROR("TPDU TLV Err: NULL", 0, 0, 0);
     return GSTK_BAD_PARAM;
  }

  if(is_cdma_tpdu) {
#if defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT) || defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
   tpdu_tlv->tag = (uint8)GSTK_CDMA_SMS_TPDU_TAG;
#else
   MSG_ERROR("cdma tpdu invalid in curr tech", 0, 0, 0);
   return GSTK_BAD_PARAM;
#endif /*FEATURE_UIM_TOOLKIT_UTK || FEATURE_CCAT || FEATURE_CDMA_800 || FEATURE_CDMA_1900*/
  }
  else
  {
   tpdu_tlv->tag = (int)GSTK_SMS_TPDU_TAG;
  }
  tpdu_tlv->length = data->length;

  gstk_memcpy(tpdu_tlv->sms_tpdu, data->tpdu, data->length, 
              GSTK_MAX_RAW_SMS_LEN, data->length);

  return GSTK_SUCCESS;
} /* gstk_packer_sms_tpdu_tlv */


/*===========================================================================
FUNCTION gstk_packer_browser_term_cause_tlv

DESCRIPTION

  This function populates the passed in browser termination cause tlv pointer

PARAMETERS
  cause: [Input] Indicates the termination cause 
  cause_tlv: [Input/Output] Pointers to which the
                              gstk_browser_term_cause_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_browser_term_cause_tlv(
         const gstk_browser_term_cause_enum_type  *cause,
         gstk_browser_term_cause_tlv_type         *cause_tlv)
{
  MSG_HIGH("** Packing browser termination cause", 0, 0, 0);

  if ((cause_tlv == NULL) || (cause == NULL)) {
    MSG_ERROR("Browser Term Cause Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  cause_tlv->tag = (int)GSTK_BROWSER_TERMINATION_CAUSE_TAG;
  cause_tlv->length = GSTK_BROWSER_TERMINATION_CAUSE_LEN;
  if ((*cause != GSTK_BROWSER_USER_TERMINATED) && (*cause != GSTK_BROWSER_ERROR_TERMINATED))
  {
    MSG_ERROR("Browser Term Cause Err: unknown cause 0x%x", *cause, 0, 0);
    return GSTK_ERROR;
  }
  cause_tlv->cause = (uint8)(*cause);
  return GSTK_SUCCESS;
} /* gstk_packer_browser_term_cause_tlv */


/*===========================================================================
FUNCTION gstk_packer_timer_id_tlv

DESCRIPTION

  This function populates the passed in timer id tlv pointer

PARAMETERS
  timer_id: [Input] Indicates the timer id 
  timer_id_tlv: [Input/Output] Pointers to which the
                              gstk_timer_id_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_timer_id_tlv(
         const uint8            *timer_id,
         gstk_timer_id_tlv_type *timer_id_tlv)
{
  MSG_HIGH("** Packing timer id", 0, 0, 0);
  if ((timer_id_tlv == NULL) || (timer_id == NULL)) {
    MSG_ERROR("Timer ID Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  timer_id_tlv->tag = (int)GSTK_TIMER_IDENTIFIER_TAG;
  timer_id_tlv->length = GSTK_TIMER_IDENTITY_LEN;
  timer_id_tlv->timer_id = (uint8)(*timer_id);
  return GSTK_SUCCESS;
} /* gstk_packer_timer_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_timer_value_tlv

DESCRIPTION

  This function populates the passed in timer value tlv pointer

PARAMETERS
  timer_value: [Input] Indicates the timer value 
  timer_value_tlv: [Input/Output] Pointers to which the
                              gstk_timer_value_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_timer_value_tlv(
         const gstk_timer_value_type *timer_value,
         gstk_timer_value_tlv_type   *timer_value_tlv)
{
  MSG_HIGH("** Packing timer value", 0, 0, 0);
  if ((timer_value == NULL) || (timer_value_tlv == NULL))
  {
    MSG_ERROR("Timer ID Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  timer_value_tlv->tag = (int)GSTK_TIMER_VALUE_TAG;
  timer_value_tlv->length = GSTK_TIMER_VALUE_LEN;

  timer_value_tlv->timer_value.hour = (timer_value->hour / 10) |
                                      ((timer_value->hour % 10) << 0x04);
  timer_value_tlv->timer_value.minute = (timer_value->minute / 10) |
                                        ((timer_value->minute % 10) << 0x04);
  timer_value_tlv->timer_value.second = (timer_value->second / 10) |
                                        ((timer_value->second % 10) << 0x04);
  return GSTK_SUCCESS;
} /* gstk_packer_timer_value_tlv */


/*===========================================================================
FUNCTION gstk_packer_cell_broadcast_page_tlv

DESCRIPTION

  This function populates the passed in cell broadcast page tlv pointer

PARAMETERS
  cell cb_page: [Input] Indicates the cell broadcast page value 
  cb_page_tlv: [Input/Output] Pointers to which the
                              gstk_cell_broadcast_page_tag_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cell_broadcast_page_tlv(
         const gstk_cb_page_type                *cb_page,
         gstk_cell_broadcast_page_tag_tlv_type  *cb_page_tlv)
{

  MSG_HIGH("** Packing cell broadcast page", 0, 0, 0);
  if ((cb_page == NULL) || (cb_page_tlv == NULL))
  {
    MSG_ERROR("Cell Broadcast Page Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  cb_page_tlv->tag = (int)GSTK_CELL_BROADCAST_PAGE_TAG;
  cb_page_tlv->length = GSTK_CELL_BROADCAST_PAGE_LEN;

  cb_page_tlv->cb_page = gstk_malloc(GSTK_CELL_BROADCAST_PAGE_LEN);

  if (cb_page_tlv->cb_page == NULL) {
    MSG_ERROR("Malloc failed for CellBroadcast Page", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }
  memcpy(cb_page_tlv->cb_page, cb_page->cb_page_data, GSTK_CELL_BROADCAST_PAGE_LEN);
  
  return GSTK_SUCCESS;
} /* gstk_packer_timer_value_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_status_tlv

DESCRIPTION

  This function populates the passed in channel status tlv pointer

PARAMETERS
  ch_status: [Input] Indicates the channel status value 
  ch_status_tlv: [Input/Output] Pointers to which the
                              gstk_ch_status_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_ch_status_tlv(
         const gstk_ch_status_type        *ch_status,
         gstk_ch_status_tlv_type          *ch_status_tlv)
{

  MSG_HIGH("** Packing Channel Status", 0, 0, 0);
  if ((ch_status == NULL) || (ch_status_tlv == NULL))
  {
    MSG_ERROR("Channel Status Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Channel Status TLV Tag */
  ch_status_tlv->tag = (int)GSTK_CHANNEL_STATUS_TAG;
  
  /* Channel Status TLV Length */
  ch_status_tlv->ch_status_length = GSTK_CHANNEL_STATUS_LEN;

  /* Populate the first byte of ch_status_tlv->ch_status with channel 
     id and link established boolean */
  if (ch_status->is_link_est) {
    ch_status_tlv->ch_status[0] = 0x80 | (((uint8)ch_status->ch_id) & 0x7);
  }
  else {
    ch_status_tlv->ch_status[0] = ((uint8)ch_status->ch_id) & 0x7;
  }

  /* Populate the second byte of ch_status_tlv->ch_status with 
     additional information */
  switch (ch_status->ch_status_additional_info) {
    case GSTK_CH_STATUS_NO_FURTHER_INFO:
      ch_status_tlv->ch_status[1] = 0;
      break;
    case GSTK_CH_STATUS_LINK_DROPPED:
      ch_status_tlv->ch_status[1] = 5;
      break;
    default:
      ch_status_tlv->ch_status[1] = 1;
      break;
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_status_tlv */


/*===========================================================================
FUNCTION gstk_packer_csd_bearer_param

DESCRIPTION
  This function populates the csd bearer information.

PARAMETERS
  gstk_csd_bearer_param_type gstk_bearer -  conatins bearer parameters
  uint8                     *csd_bearer  -  pointer holds csd bearer 
                                            information.

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_packer_csd_bearer_param(
  gstk_csd_bearer_param_type gstk_bearer, 
  uint8                     *csd_bearer)
{

  /* Allocate memory for csd bearer pointer */
  if (csd_bearer == NULL) {
    csd_bearer = gstk_malloc(GSTK_CSD_BEARER_PARAM_LEN);
    if (csd_bearer == NULL) {
      MSG_ERROR("Malloc GPRS bearer param failed", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }
  }

  /* Determine bearer speed information */
  switch (gstk_bearer.speed) {
  case GSTK_CSD_BEARER_SPEED_AUTO:
    csd_bearer[0] = 0;
    break;
  case GSTK_CSD_BEARER_SPEED_300_V21:
    csd_bearer[0] = 1;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V22:
    csd_bearer[0] = 2;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_75_V23:
    csd_bearer[0] = 3;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V22:
    csd_bearer[0] = 4;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V26:
    csd_bearer[0] = 5;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V32:
    csd_bearer[0] = 6;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V32:
    csd_bearer[0] = 7;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V34:
    csd_bearer[0] = 12;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V34:
    csd_bearer[0] = 14;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V34:
    csd_bearer[0] = 15;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V34:
    csd_bearer[0] = 16;
    break;
  case GSTK_CSD_BEARER_SPEED_33600_V34:
    csd_bearer[0] = 17;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V120:
    csd_bearer[0] = 34;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V120:
    csd_bearer[0] = 36;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V120:
    csd_bearer[0] = 38;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V120:
    csd_bearer[0] = 39;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V120:
    csd_bearer[0] = 43;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V120:
    csd_bearer[0] = 47;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V120:
    csd_bearer[0] = 48;
    break;
  case GSTK_CSD_BEARER_SPEED_38400_V120:
    csd_bearer[0] = 49;
    break;
  case GSTK_CSD_BEARER_SPEED_48000_V120:
    csd_bearer[0] = 50;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_V120:
    csd_bearer[0] = 51;
    break;
  case GSTK_CSD_BEARER_SPEED_300_V110:
    csd_bearer[0] = 65;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V110:
    csd_bearer[0] = 66;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V110_X31:
    csd_bearer[0] = 68;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V110_X31:
    csd_bearer[0] = 70;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V110_X31:
    csd_bearer[0] = 71;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V110_X31:
    csd_bearer[0] = 75;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V110_X31:
    csd_bearer[0] = 79;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V110_X31:
    csd_bearer[0] = 80;
    break;
  case GSTK_CSD_BEARER_SPEED_38400_V110_X31:
    csd_bearer[0] = 81;
    break;
  case GSTK_CSD_BEARER_SPEED_48000_V110_X31:
    csd_bearer[0] = 82;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_V110_X31:
    csd_bearer[0] = 83;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_V110_X31:
    csd_bearer[0] = 84;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT:
    csd_bearer[0] = 115;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT:
    csd_bearer[0] = 116;
    break;
  case GSTK_CSD_BEARER_SPEED_32000_PIAFS32K:
    csd_bearer[0] = 120;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_PIAFS32K:
    csd_bearer[0] = 121;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA:
    csd_bearer[0] = 130;
    break;
  case GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA:
    csd_bearer[0] = 131;
    break;
  case GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA:
    csd_bearer[0] = 132;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA:
    csd_bearer[0] = 133;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA:
    csd_bearer[0] = 134;
    break;
  default:
    MSG_ERROR("CSD_Bearer speed from Client 0x%x", gstk_bearer.speed, 0, 0);
    csd_bearer[0] = 0xFF;
    break;
  }

  /* Determine bearer name */
  switch(gstk_bearer.name) {
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ:
    csd_bearer[1] = 0;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ:
    csd_bearer[1] = 1;
    break;
  case GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI:
    csd_bearer[1] = 2;
    break;
  case GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI:
    csd_bearer[1] = 3;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI:
    csd_bearer[1] = 4;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI:
    csd_bearer[1] = 5;
    break;
  case GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI:
    csd_bearer[1] = 6;
    break;
  case GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI:
    csd_bearer[1] = 7;
    break;
  default:
    MSG_ERROR("CSD_Bearer name from Client 0x%x", gstk_bearer.name, 0, 0);
    csd_bearer[1] = 0xFF;
    break;
  }

  /* Determine bearer connection element */
  switch(gstk_bearer.connection_element) {
  case GSTK_CSD_BEARER_CE_TRANSPARENT:
    csd_bearer[2] = 0;
    break;
  case GSTK_CSD_BEARER_CE_NON_TRANSPARENT:
    csd_bearer[2] = 1;
    break;
  case GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF:
    csd_bearer[2] = 2;
    break;
  case GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF:
    csd_bearer[2] = 3;
    break;
  default:
    MSG_ERROR("CSD_Bearer connection element from Client 0x%x", gstk_bearer.connection_element, 0, 0);
    csd_bearer[2] = 0xFF;
    break;
  }
  return GSTK_SUCCESS;
}  /* gstk_packer_csd_bearer_param */


/*===========================================================================
FUNCTION gstk_packer_gprs_bearer_param

DESCRIPTION
  This function populates the gprs bearer information.

PARAMETERS
  gstk_gprs_bearer_param_type gstk_bearer - conatins bearer parameters
  uint8                     *csd_bearer  -  pointer holds csd bearer 
                                            information.

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_packer_gprs_bearer_param(
  gstk_gprs_bearer_param_type gstk_bearer, 
  uint8                       *gprs_bearer)
{
  /* Allocate memory for the gprs_bearer pointer */
  if (gprs_bearer == NULL) {
    gprs_bearer = gstk_malloc(GSTK_GPRS_BEARER_PARAM_LEN);
    if (gprs_bearer == NULL) {
      MSG_ERROR("Malloc GPRS bearer param failed", 0, 0, 0);
      return GSTK_MEMORY_ERROR;
    }
  }

  /* Fill in Bearer Description information 
  1. Precedence class 
  2. Delay class
  3. Reliability class
  4. Peak throughput class
  5. Mean throughput class
  6. Packet Data Protocol type
  */

  gprs_bearer[0] = int32touint8(gstk_bearer.precedence_cls);
  gprs_bearer[1] = int32touint8(gstk_bearer.delay_cls);
  gprs_bearer[2] = int32touint8(gstk_bearer.reliability_cls);
  gprs_bearer[3] = int32touint8(gstk_bearer.peak_throughput);
  gprs_bearer[4] = int32touint8(gstk_bearer.mean_throughput);
  switch(gstk_bearer.pkt_data_protocol) {
  case GSTK_PKT_DATA_IP:
    gprs_bearer[5] = 0x02;
    break;
  default:
    MSG_ERROR("GPRS_Bearear from Client 0x%x", gstk_bearer.pkt_data_protocol, 0, 0);
    gprs_bearer[5] = 0xFF;
    break;
  }
  return GSTK_SUCCESS;
}  /* gstk_packer_gprs_bearer_param */


/*===========================================================================
FUNCTION gstk_packer_bearer_description_tlv

DESCRIPTION

  This function populates the passed in bearer description tlv pointer

PARAMETERS
  bearer_description: [Input] Indicates the bearer description value 
  bearer_description_tlv: [Input/Output] Pointers to which the
                              gstk_bearer_description_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_bearer_description_tlv(
         const gstk_bearer_description_type  *bearer_description,
         gstk_bearer_description_tlv_type    *bearer_description_tlv)
{
  gstk_status_enum_type gstk_status = GSTK_ERROR;

  MSG_HIGH("** Packing Bearer Description", 0, 0, 0);
  if ((bearer_description == NULL) || (bearer_description_tlv == NULL))
  {
    MSG_ERROR("Bearer Description Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  bearer_description_tlv->tag = (int)GSTK_BEARER_DESCRIPTION_TAG;

  switch(bearer_description->bearer_type) {
    case GSTK_CSD:
      bearer_description_tlv->bearer_type = 0x01;
      bearer_description_tlv->bearer_description_length = GSTK_CSD_BEARER_PARAM_LEN + 1;
      bearer_description_tlv->bearer_param = gstk_malloc(bearer_description_tlv->bearer_description_length-1);
      gstk_status = gstk_packer_csd_bearer_param(bearer_description->bearer_params.csd_bearer, bearer_description_tlv->bearer_param);
      break;
    case GSTK_GPRS:
      bearer_description_tlv->bearer_type = 0x02;
      bearer_description_tlv->bearer_description_length = GSTK_GPRS_BEARER_PARAM_LEN + 1; 
      bearer_description_tlv->bearer_param = gstk_malloc(bearer_description_tlv->bearer_description_length-1);
      gstk_status = gstk_packer_gprs_bearer_param(bearer_description->bearer_params.gprs_bearer, bearer_description_tlv->bearer_param);
      break;
    case GSTK_BEARER_DEFAULT:
      bearer_description_tlv->bearer_type = 0x03;
      bearer_description_tlv->bearer_description_length = 1;
      bearer_description_tlv->bearer_param = NULL;
      break;
    default:
      MSG_ERROR("Bearer Description invalid bearer type 0x%x", bearer_description->bearer_type, 0, 0);
      gstk_status = GSTK_ERROR;
  }
  return gstk_status;
} /* gstk_packer_bearer_description_tlv */


/*===========================================================================
FUNCTION gstk_packer_buffer_size_tlv

DESCRIPTION

  This function populates the passed in buffer size tlv pointer

PARAMETERS
  buffer_size: [Input] Indicates the buffer size value 
  buffer_size_tlv: [Input/Output] Pointers to which the
                              gstk_buffer_size_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_buffer_size_tlv(
         int32                         buffer_size,
         gstk_buffer_size_tlv_type    *buffer_size_tlv)
{

  MSG_HIGH("** Packing buffer size", 0, 0, 0);
  if (buffer_size_tlv == NULL)
  {
    MSG_ERROR("Buffer size Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  buffer_size_tlv->tag = (int)GSTK_BUFFER_SIZE_TAG;

  buffer_size_tlv->buffer_size_length = GSTK_BUFFER_SIZE_LEN;
  
  buffer_size_tlv->buffer_size[0] = (int32touint32(buffer_size) >> 8) & 0xFF;
  buffer_size_tlv->buffer_size[1] = (uint8)buffer_size & 0xFF;
  return GSTK_SUCCESS;
} /* gstk_packer_buffer_size_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_data_len_tlv

DESCRIPTION

  This function populates the passed in channel data length tlv pointer

PARAMETERS
  ch_data_len: [Input] Indicates the channel data length 
  ch_data_len_tlv: [Input/Output] Pointers to which the
                              gstk_ch_data_len_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_ch_data_len_tlv(
         int32                         ch_data_len,
         gstk_ch_data_len_tlv_type    *ch_data_len_tlv)
{

  MSG_HIGH("** Packing Channel Data Length", 0, 0, 0);
  if (ch_data_len_tlv == NULL)
  {
    MSG_ERROR("Channel Data Length Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  ch_data_len_tlv->tag = (int)GSTK_CHANNEL_DATA_LEN_TAG;

  ch_data_len_tlv->ch_data_len_length = GSTK_CHANNEL_DATA_LEN_LEN;
  
  /* The max channel data length requested cannot be more than 0xFF */
  if(ch_data_len > 0xFF)
  {
    ch_data_len_tlv->ch_data_len = 0xFF;
  }
  else
  {
    ch_data_len_tlv->ch_data_len = (uint8)ch_data_len;
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_data_len_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_data_tlv

DESCRIPTION

  This function populates the passed in channel data tlv pointer

PARAMETERS
  ch_data: [Input] Indicates the channel data length 
  ch_data_tlv: [Input/Output] Pointers to which the
                              gstk_ch_data_tlv_type info to be
                              populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_ch_data_tlv(
         gstk_ch_data_type             ch_data,
         gstk_ch_data_tlv_type        *ch_data_tlv)
{

  MSG_HIGH("** Packing Channel Data", 0, 0, 0);
  if (ch_data_tlv == NULL)
  {
    MSG_ERROR("Channel Data Err: Null ptr", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  ch_data_tlv->tag = (int)GSTK_CHANNEL_DATA_TAG;

  ch_data_tlv->ch_data_length = int32touint8(ch_data.data_len);
  
  /* initialize ch_data to NULL */
  ch_data_tlv->ch_data = NULL;

  if ((ch_data.data_len > 0) && (ch_data.data != NULL)) {
    ch_data_tlv->ch_data = gstk_malloc(ch_data_tlv->ch_data_length);
    if (ch_data_tlv->ch_data != NULL) {
      memcpy(ch_data_tlv->ch_data, ch_data.data, ch_data_tlv->ch_data_length);
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_data_tlv */


/*===========================================================================
FUNCTION gstk_packer_dev_id_card_tlv

DESCRIPTION

  This function populates the data uint8 array with Device ID value info

PARAMETERS
  dev_id_orig_ptr       [Input] data pointer from which the information 
                        if to be copied
  dev_id_new_value_ptr  [Input/Output] data pointer to which the information
                        is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_dev_id_card_tlv(
  const gstk_device_id_type          *dev_id_orig_ptr,
  uint8                              *dev_id_new_value_ptr
)
{

  MSG_HIGH("** Packing device ID", 0, 0, 0);

  if((dev_id_orig_ptr == NULL) || (dev_id_new_value_ptr == NULL))
  {
    MSG_ERROR("Device Id Err: NULL", 0, 0, 0);
    MSG_ERROR("Device Id Err: orig ptr 0x%x, new ptr 0x%x", 
      dev_id_orig_ptr, dev_id_new_value_ptr, 0);
    return GSTK_BAD_PARAM;
  }
  dev_id_new_value_ptr[0] = (uint8)dev_id_orig_ptr->src;
  dev_id_new_value_ptr[1] = (uint8)dev_id_orig_ptr->dest;

  return GSTK_SUCCESS;
} /* gstk_packer_dev_id_card_tlv */


/*===========================================================================
FUNCTION gstk_packer_location_info_card_tlv

DESCRIPTION

   This function populates the data uint8 array with Location Info value 
   according to the technology

PARAMETERS
  location_info_orig_ptr        [Input] data pointer from which the information 
                                if to be copied
  location_info_new_value_ptr   [Input/Output] data pointer to which the 
                                information is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_location_info_card_tlv(
  const gstk_location_info_gen_type  *location_info_orig_ptr,
  uint8                              *location_info_new_value_ptr,
  int32                               dest_size
)
{
  #if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
  #endif
  
  size_t gstk_max_cell_id_len =  GSTK_MIN_CELL_ID_LEN;
  size_t new_dest_size        =  int32touint32(dest_size);

  MSG_HIGH("** Packing location info", 0, 0, 0);

  if((location_info_orig_ptr == NULL) || (location_info_new_value_ptr == NULL))
  {
    MSG_ERROR("Location Info Err: orig ptr 0x%x, new ptr 0x%x", 
      location_info_orig_ptr, location_info_new_value_ptr, 0);
    return GSTK_BAD_PARAM;
  }

  if(dest_size < 0)
  {
    MSG_ERROR("dest size less than 0", 0,0,0);
    return GSTK_BAD_PARAM;
  }

  if (location_info_orig_ptr->location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
  {
    gstk_memcpy(location_info_new_value_ptr, 
                location_info_orig_ptr->location_info.gsm_umts_loc_info.mcc_and_mnc, 
                GSTK_MAX_PLMN_LEN, /*copy_size */
                new_dest_size,     /* dest max size */
                sizeof(location_info_orig_ptr->location_info.gsm_umts_loc_info.mcc_and_mnc));/* src max size */

    new_dest_size = int32touint32(dest_size) - GSTK_MAX_PLMN_LEN;

    gstk_memcpy(&location_info_new_value_ptr[GSTK_MAX_PLMN_LEN], 
                location_info_orig_ptr->location_info.gsm_umts_loc_info.loc_area_code, 
                GSTK_MAX_LAC_LEN, /*copy_size */
                new_dest_size,    /* dest max size */
                sizeof(location_info_orig_ptr->location_info.gsm_umts_loc_info.loc_area_code));/* src max size */

    new_dest_size = int32touint32(dest_size) - (GSTK_MAX_PLMN_LEN + GSTK_MAX_LAC_LEN);

    #if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#error code not present
    #endif /* #if defined(FEATURE_WCDMA) && !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD) */

    gstk_memcpy(&location_info_new_value_ptr[GSTK_MAX_PLMN_LEN + GSTK_MAX_LAC_LEN], 
                location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id, 
                gstk_max_cell_id_len,    /*copy_size */
                new_dest_size,           /* dest max size */
                int32touint32(location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id_len));/* src max size */
    
    return GSTK_SUCCESS;
  }
  if (location_info_orig_ptr->location_info_tech == GSTK_CDMA_LOCATION_INFO)
  {
    return GSTK_UNSUPPORTED_COMMAND;
  }
  return GSTK_ERROR;

} /* gstk_packer_location_info_card_tlv */


/*lint -e715 pdp_context_act_param_orig_ptr not being used */
/*lint -e715 pdp_context_act_param_new_value_ptr not being used */
/*lint -e818 pdp_context_act_param_new_value_ptr not const */
/*===========================================================================
FUNCTION gstk_packer_pdp_context_act_param_card_tlv

DESCRIPTION

   This function populates the data uint8 array with PDP Context Activation
   Parameter value 

PARAMETERS
  pdp_context_act_param_orig_ptr      [Input] data pointer from which the 
                                      information if to be copied
  pdp_context_act_param_new_value_ptr [Input/Output] data pointer to which the 
                                      information is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_pdp_context_act_param_card_tlv(
  const gstk_pdp_context_act_param_type  *pdp_context_act_param_orig_ptr,
  uint8                                  *pdp_context_act_param_new_value_ptr
)
{
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#else
  MSG_ERROR("No GPRS Call Control Support", 0, 0, 0);
  return GSTK_ERROR;
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL */
  
} /* gstk_packer_pdp_context_act_param_card_tlv */
/*lint +e715 pdp_context_act_param_orig_ptr not being used */
/*lint +e715 pdp_context_act_param_new_value_ptr not being used */
/*lint +e818 pdp_context_act_param_new_value_ptr not const */


/*===========================================================================
FUNCTION gstk_packer_cmd_details_card_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Index location in the command_Details_ref_table
  command_rsp_type: [Input] Specifies the type of terminal response, e.g.,
                            GSTK_DISPLAY_TEXT_CNF, etc,
  command_number: [Input] Command number for the proactive command that
                          this terminal response is responding to
  command_details: [Input/Output] Pointers to which the command details
                                  to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_cmd_details_card_tlv(
  gstk_cmd_info_type             cmd_info,
  uint8                         *cmd_details_new_value_ptr
)
{
    int32 offset = 0;

    MSG_HIGH("** Packing Cmd Details", 0, 0, 0);

    if(cmd_details_new_value_ptr == NULL)
    {
       MSG_ERROR("Cmd Detail Err: NULL", 0, 0, 0);
       return GSTK_BAD_PARAM;
    }
    /* 4 checks to verify that the ref_id is a valid value/match to the information
    ** in the command_details_ref_table
    ** - reference_id check
    ** - whether the location in the table is still valid
    ** - command number check
    ** - response type and type of command match
    */
    if (cmd_info.ref_id < GSTK_MAX_PROACTIVE_COMMAND) { /*ref_id within range */
        if (command_details_ref_table[cmd_info.ref_id].free_slot != TRUE ) { /* this location is still used*/
          if ((int)command_details_ref_table[cmd_info.ref_id].expected_cmd_rsp_type == cmd_info.command_rsp_type) {
              /* command matches -> valid ref_id */
              if (command_details_ref_table[cmd_info.ref_id].command_details.command_number !=
                    cmd_info.command_number ) {
                /* wrong match */
                MSG_ERROR("Cmd Detail Err: Command Num Mismatch", 0, 0, 0);
                return GSTK_BAD_PARAM;
              }
          }
          else
          {
             MSG_ERROR("Cmd Detail Err: Command Rsp Mismatch", 0, 0, 0);
             return GSTK_BAD_PARAM;
          }
        }
        else {
          MSG_ERROR("Cmd Detail Err: command_details_ref_table[0x%x] not in use", cmd_info.ref_id, 0, 0);
          return GSTK_BAD_PARAM;
        }
    }
    else {
      MSG_ERROR("Cmd Detail Err: ref_id 0x%x out of range", 0, 0, 0);
      return GSTK_BAD_PARAM;
    }

    /* populate the command_details tlv for the response */

    cmd_details_new_value_ptr[offset] = 
      command_details_ref_table[cmd_info.ref_id].command_details.command_number;
    offset ++;

    cmd_details_new_value_ptr[offset] =
      command_details_ref_table[cmd_info.ref_id].command_details.type_of_command;
    offset ++;

    cmd_details_new_value_ptr[offset] =
      command_details_ref_table[cmd_info.ref_id].command_details.qualifier;
    offset ++;

    return GSTK_SUCCESS;
} /*gstk_packer_cmd_details_card_tlv*/


/*===========================================================================
FUNCTION gstk_packer_result_card_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  result_info           [Input] data pointer from which the information 
                        if to be copied
  result_new_value_ptr  [Input/Output] data pointer to which the information
                        is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_result_card_tlv(
  gstk_result_type              result_info,
  uint8                         *result_new_value_ptr
)
{
  MSG_HIGH("** Packing Result", 0, 0, 0);

  if(result_new_value_ptr == NULL)
  {
    MSG_ERROR("Result new value ptr Err: NULL", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }
  result_new_value_ptr[0] = (uint8)result_info.command_result;
  if((result_info.result_additional_info.length > 0) &&
     (result_info.result_additional_info.additional_info_ptr != NULL)
    )
  {
    memcpy(&(result_new_value_ptr[1]),result_info.result_additional_info.additional_info_ptr,
           result_info.result_additional_info.length);
  }
  return GSTK_SUCCESS;

}


/*===========================================================================
FUNCTION gstk_packer_nw_search_mode_card_tlv

DESCRIPTION

  This function populates the data uint8 array with network search mode value info

PARAMETERS
  nw_search_mode                [Input] data pointer from which the information 
                                if to be copied
  nw_search_mode_new_value_ptr  [Input/Output] data pointer to which the information
                                is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_nw_search_mode_card_tlv(
  gstk_nw_search_mode_enum_type    nw_search_mode,
  uint8                            *nw_search_mode_new_value_ptr
)
{

  MSG_HIGH("** Packing Network Search Mode", 0, 0, 0);

  if(nw_search_mode_new_value_ptr == NULL)
  {
    MSG_ERROR("nw_search_mode_new_value_ptr Err: NULL", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }
  nw_search_mode_new_value_ptr[0] = int32touint8((int32)nw_search_mode);
  return GSTK_SUCCESS;
} /* gstk_packer_nw_search_mode_card_tlv */
/*===========================================================================
FUNCTION gstk_packer_evt_list_card_tlv

DESCRIPTION

  This function populates the data uint8 array with event id value info

PARAMETERS
  evt_list_info_data       [Input] data pointer from which the information 
                           if to be copied
  evt_list_new_value_ptr   [Input/Output] data pointer to which the information
                           is to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_evt_list_card_tlv(
  gstk_evt_list_code_enum_type      evt_list_info_data,
  uint8                            *evt_list_new_value_ptr
)
{

  MSG_HIGH("** Packing Event List", 0, 0, 0);

  if(evt_list_new_value_ptr == NULL)
  {
    MSG_ERROR("evt_list_new_value_ptr Err: NULL", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }
  evt_list_new_value_ptr[0] = int32touint8((int32)evt_list_info_data);
  return GSTK_SUCCESS;
}

/*lint -e750 local macro gstk_c not being used */


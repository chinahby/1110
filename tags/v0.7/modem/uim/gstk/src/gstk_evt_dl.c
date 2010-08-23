/*===========================================================================


                       G S T K   E V T   D L   C M D

GENERAL DESCRIPTION
  This source file contains event download envelope command functions


FUNCTIONS
  gstk_cm_call_event_cb
    Call event callback function, called when a call event is happening.
    This function populate the various call related indication type and
    put the command to the GSTK command queue.

  gstk_mt_call_evt_ind
    This function builds the mt call command and sends it to UIM

  gstk_call_connected_evt_ind
    This function builds the call connected envelope command and sends it to UIM

  gstk_call_disconnected_evt_ind
    This function builds the call disconnected envelope command and sends it
    to UIM

  gstk_idle_scrn_evt_ind
    This function builds the idle screen command and sends it to UIM

  gstk_user_act_evt_ind
    This function builds the user activity command and sends it to UIM

  gstk_location_status_evt_ind
    This function builds the location status event download envelope command
    and sends it to UIM

  gstk_lang_selection_evt_ind
    This function builds the language selection event download envelope command
    and sends it to UIM

  gstk_browser_term_evt_ind
    This function builds the browser termination event download envelope command
    and sends it to UIM

  gstk_util_translate_cm_transaction_id_to_gstk_id
    This function converts the CM transaction ID into SAT/USAT format

  gstk_util_translate_cm_transaction_id_to_gstk_id_ext
    This function converts the CM transaction ID into SAT/USAT format
    
  gstk_util_translate_cm_cause_to_gstk_cause
    This function converts the CM cause into SAT/USAT format
    

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_evt_dl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/08   xz      Remove initialization of "cause" structure in 
                   gstk_cm_call_event_cb()
09/14/08   sk      Fixed lint errors
05/06/08   sk      Added support for event download env retry
02/06/08   sk      Support for PLI UTRAN NMR
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/02/06   tml     Fixed transaction ID issue and lint
05/18/06   sk      Fixed 1X warnings.
05/17/06   sk      Added support for Radio Link Failure Call disconnected evt
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related location information.
11/21/05   sk      Fixed lint errors
11/15/05   sk      Lint fixes
11/09/05   sk      Fixed Lint Errors
10/08/05   tml     Cell ID Length check
10/07/05   sk      LS fixes
10/04/05   sk      Added messages to gstk_location_status_evt_ind().
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added new fn gstk_access_tech_evt_ind()
                   for Access Technology envelope.
05/11/05   sst     Lint fixes
01/27/05   tml     Fixed potential memory leak issue
01/12/05   tml     Fixed compilation errors for 1X only target
12/20/04    sk     Added 2 new utility function 
                   gstk_util_translate_cm_cause_to_gstk_cause()
                   & gstk_util_translate_cm_transaction_id_to_gstk_id_ext()
                   Added support to handle cause value correctly on a 
                   disconnect event.
11/23/04    sk     Added SAT/USAT data support
10/01/04   tml     Fixed transaction id issue
08/16/04   tml     Added call direction fix
08/13/04   tml     Fixed source destination device ID for call connected
08/10/04   tml     Fixed transaction id 
08/10/04   tml     Added checking in CALL CONNECTED event cb to determine if
                   the envelope cmd should be sent immediately to the card or
                   not
08/03/04   tml     Fixed transaction ID issue with Call Connected and Call
                   Disconnected event download, fixed command queue issue
05/26/04   tml     Added More debug messages 
05/11/04   tml     Added timer management, lang notification, browser term
                   supports
04/16/04   tml     linted
04/02/04   tml     linted
03/01/04   tml     Merged from branch for location status env command fix
02/06/04   wli     Featurization for CDMA only build
05/16/03   tml     Linted
04/21/03   tml     Added NPI support in address
03/20/03   tml     Fixed offset for envelope responses
03/06/03   tml     Updated enum names
02/22/03   tml     Added support for transaction ID, near end and cause info
                   Event download user data fixes
02/18/03   tml     Enhanced CM related event download functions
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "gstk_envelope_cmd.h"
#include "uim.h"
#include "gstkutil.h"
#include "tmc.h"
#include "string.h"
#include "msg.h"
#include "err.h"
#define GSTK_NORMAL_RADIO_LINK_RELEASE 165
#define GSTK_MAX_NUM_OF_EVT_DL_ENV_RETRY 3

typedef enum
{
  GSTK_CALL_MSG_DIRECTION_TO_ME,
  GSTK_CALL_MSG_DIRECTION_FROM_ME,
  GSTK_CALL_MSG_DIRECTION_MAX_ENUM = 0x7FFFFFFF
}gstk_message_direction_enum_type;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

/*lint -e715 "call_info_ptr call_event not referenced . This lint error is 
  suppressed when FEATURE_GSM or FEATURE_WCDMA is not defined." */
/*===========================================================================
FUNCTION   gstk_cm_call_event_cb

DESCRIPTION
  Call event callback function, called when a call event is happening.
  This function populate the various call related indication type and
  put the command to the GSTK command queue.

PARAMETERS
  call_event: [Input] cm call event
  call_info_ptr: [Input] Pointer to the call info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_call_event_cb (
  cm_call_event_e_type         call_event,
  const cm_mm_call_info_s_type *call_info_ptr
)
{

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */

} /* gstk_cm_call_event_cb */
/*lint +e715 "call_info_ptr call_event not referenced . This lint error is 
  suppressed when FEATURE_GSM or FEATURE_WCDMA is not defined." */

/*===========================================================================
FUNCTION gstk_mt_call_evt_ind

DESCRIPTION

  This function builds the mt call command and sends it to UIM

PARAMETERS
  mt_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_mt_call_evt_ind(
        gstk_envelope_mt_evt_ind_type    * mt_evt_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_mt_evt_dl_command_type        STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   int                                         i  = 0;

   if(mt_evt_cmd == NULL)
   {
     MSG_ERROR("mt_evt_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_mt_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_MT_CALL,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.event_download_hdr.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* transaction id tag */
   if(mt_evt_cmd->mt_evt_data.transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       gstk_malloc(mt_evt_cmd->mt_evt_data.transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       MSG_ERROR("malloc failure", 0, 0, 0);
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &mt_evt_cmd->mt_evt_data.transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* address tag */
   if(mt_evt_cmd->mt_evt_data.address.length != 0) {
     gstk_status = gstk_packer_address_tlv(GSTK_ADDRESS_TAG,
                             &mt_evt_cmd->mt_evt_data.address,
                             &STK_envelope_cmd.address);
     if(gstk_status != GSTK_SUCCESS) {
       return gstk_status;
     }
   }
   if(mt_evt_cmd->mt_evt_data.subaddress.length != 0) {
     gstk_status = gstk_packer_subaddress_tlv(
          &mt_evt_cmd->mt_evt_data.subaddress,
          &STK_envelope_cmd.subaddress);
     if (gstk_status != GSTK_SUCCESS) {
         return gstk_status;
     }
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + transaction id data + 2
                        + address data + 2
                        + subaddress data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.transacton_id.length +
                             6;
   if(mt_evt_cmd->mt_evt_data.address.length != 0) {
     STK_envelope_cmd.event_download_hdr.length = STK_envelope_cmd.event_download_hdr.length +
                               STK_envelope_cmd.address.length +
                               GSTK_TAG_LENGTH_LEN;
   }
   if(mt_evt_cmd->mt_evt_data.subaddress.length != 0) {
     STK_envelope_cmd.event_download_hdr.length = STK_envelope_cmd.event_download_hdr.length +
                               STK_envelope_cmd.subaddress.length +
                               GSTK_TAG_LENGTH_LEN;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = mt_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                           + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length + 2));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length + 2);

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                     + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                    + GSTK_TAG_LENGTH_LEN);

       /* copy transaction id */
       /* tag, length */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.transacton_id,
              GSTK_TAG_LENGTH_LEN);
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       if(STK_envelope_cmd.transacton_id.length != 0) {
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.transacton_id.transaction_list,
                (int)STK_envelope_cmd.transacton_id.length);
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }

       /* copy address */
       if(STK_envelope_cmd.address.length != 0 ) { /* has address */
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.address,
              (int)(STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN));
         offset = (int)(offset + STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN);
       }

       /* copy subaddress */
       if(STK_envelope_cmd.subaddress.length != 0 ) { /* has subaddress */
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.subaddress,
              (int)(STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN));
         offset = (int)(offset + STK_envelope_cmd.subaddress.length + GSTK_TAG_LENGTH_LEN);
       }

       MSG_MED("Env: MT evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
       }

       /* free envelope mt ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }

       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)mt_evt_cmd));/*lint !e826 area too small */

       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_mt_call_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_connected_evt_ind

DESCRIPTION

  This function builds the call connected envelope command and sends it to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

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
gstk_status_enum_type gstk_call_connected_evt_ind(
        gstk_envelope_call_connected_evt_ind_type    * call_connected_evt_cmd)
{
   gstk_status_enum_type                                gstk_status = GSTK_SUCCESS;
   gstk_envelope_call_connected_evt_dl_command_type     STK_envelope_cmd;
   uim_cmd_type                                         *uim_cmd_ptr;
   int                                                  offset = 0;
   int                                                  i  = 0;

   if(call_connected_evt_cmd == NULL)
   {
     MSG_ERROR("call_connected_evt_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_call_connected_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CALL_CONNECTED,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   if(call_connected_evt_cmd->near_end) {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }
   else {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_NETWORK_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* transaction id tag */
   if(call_connected_evt_cmd->transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       gstk_malloc(call_connected_evt_cmd->transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       MSG_ERROR("malloc failure", 0, 0, 0);
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &call_connected_evt_cmd->transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + transaction id data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.transacton_id.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = call_connected_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                  + GSTK_TAG_LENGTH_LEN);

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                     + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                     + GSTK_TAG_LENGTH_LEN);

       /* copy transaction id */
       /* tag, length */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.transacton_id,
              GSTK_TAG_LENGTH_LEN);
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       if(STK_envelope_cmd.transacton_id.length != 0) {
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.transacton_id.transaction_list,
                (int)STK_envelope_cmd.transacton_id.length);
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }

       MSG_MED("Env: Call connected evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
       }

       /* free envelope call connected ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }
       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)call_connected_evt_cmd));/*lint !e826 area too small */
       
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_call_connected_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_disconnected_evt_ind

DESCRIPTION

  This function builds the call disconnected envelope command and sends it
  to UIM

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

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
gstk_status_enum_type gstk_call_disconnected_evt_ind(
        gstk_envelope_call_disconnected_evt_ind_type    * call_disconnected_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_call_disconnected_evt_dl_command_type     STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;
   int                                                     i  = 0;

   if(call_disconnected_evt_cmd == NULL)
   {
     MSG_ERROR("call_disconnected_evt_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_call_disconnected_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CALL_DISCONNECTED,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   if(call_disconnected_evt_cmd->near_end) {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }
   else {
     gstk_status = gstk_packer_dev_id_tlv(
         GSTK_NETWORK_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );
   }

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* transaction id tag */
   if(call_disconnected_evt_cmd->call_disconnected_data.transaction_id.length > 0)
   {
     STK_envelope_cmd.transacton_id.transaction_list =
       gstk_malloc(
                  call_disconnected_evt_cmd->call_disconnected_data.transaction_id.length);
     if(STK_envelope_cmd.transacton_id.transaction_list == NULL)
     {
       MSG_ERROR("malloc failure", 0, 0, 0);
       return GSTK_MEMORY_ERROR;
     }
   }
   gstk_status = gstk_packer_transaction_id_tlv(
       &call_disconnected_evt_cmd->call_disconnected_data.transaction_id,
       &STK_envelope_cmd.transacton_id);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* cause tag */
   if(call_disconnected_evt_cmd->call_disconnected_data.cause.present == TRUE) 
   {
     MSG_HIGH("Call Disconnected Cause present ",0,0,0);
     if(call_disconnected_evt_cmd->call_disconnected_data.cause.length > 0)
     {
       STK_envelope_cmd.cause.cause =
         gstk_malloc(call_disconnected_evt_cmd->call_disconnected_data.cause.length);
       if(STK_envelope_cmd.cause.cause == NULL)
       {
         MSG_ERROR("malloc failure", 0, 0, 0);
         /* free envelope call disconnected ind and tlv */
         if(STK_envelope_cmd.transacton_id.transaction_list != NULL) 
         {
           gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
         }
         return GSTK_MEMORY_ERROR;
       }
     }
     gstk_status = gstk_packer_cause_tlv(
         &call_disconnected_evt_cmd->call_disconnected_data.cause,
         &STK_envelope_cmd.cause);
     }

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = call_disconnected_evt_cmd->message_header.user_data;

       /* initialized to 3 for value portion of the TLVs 
          i.e., the final offset will be either 0 or 1 depends
          on whether the total length is > 0x7F or not */          
       offset = 3;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                     + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                     + GSTK_TAG_LENGTH_LEN);
       STK_envelope_cmd.event_download_hdr.length = 
                      STK_envelope_cmd.event_download_hdr.evt_list.length 
                     + GSTK_TAG_LENGTH_LEN;

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                   + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                   + GSTK_TAG_LENGTH_LEN);
       STK_envelope_cmd.event_download_hdr.length += 
                         STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                         + GSTK_TAG_LENGTH_LEN;

       /* copy transaction id */
       /* tag, length */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.transacton_id,
              GSTK_TAG_LENGTH_LEN);
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN);
       if(STK_envelope_cmd.transacton_id.length != 0) {
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.transacton_id.transaction_list,
                (int)STK_envelope_cmd.transacton_id.length);
         offset = (int)(offset + STK_envelope_cmd.transacton_id.length);
       }
       STK_envelope_cmd.event_download_hdr.length += 
                         STK_envelope_cmd.transacton_id.length
                         + GSTK_TAG_LENGTH_LEN;


       if(call_disconnected_evt_cmd->call_disconnected_data.cause.present == TRUE) {
         /* copy cause */
         /* tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cause.tag;
         if (STK_envelope_cmd.cause.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.event_download_hdr.length++;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cause.length;

         if(STK_envelope_cmd.cause.length != 0) 
         {
           memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                  STK_envelope_cmd.cause.cause,
                  (int)STK_envelope_cmd.cause.length);
           offset = (int)(offset + STK_envelope_cmd.cause.length);
         }

         STK_envelope_cmd.event_download_hdr.length +=
                         STK_envelope_cmd.cause.length
                         + GSTK_TAG_LENGTH_LEN;
       }

       /* populate the envelope command info */
       /* total data length:  evt id data + 2
                    + device id data + 2 (2 is for the tag and length field)
                    + transaction id data + 2
                    + cause data + 2
       */
       if(STK_envelope_cmd.event_download_hdr.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
         /* account for 0x80 */
         uim_cmd_ptr->envelope.offset = 0;
         uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.event_download_hdr.tag;
         uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.event_download_hdr.length;
         STK_envelope_cmd.event_download_hdr.length++;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                           + GSTK_TAG_LENGTH_LEN;
         
       }
       else {
         uim_cmd_ptr->envelope.offset = 1;
         uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.event_download_hdr.tag;            
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.event_download_hdr.length;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                            + GSTK_TAG_LENGTH_LEN;
       }

       MSG_MED("Env: Call disconnected evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
       }

       /* free envelope call disconnected ind and tlv */
       if(STK_envelope_cmd.transacton_id.transaction_list != NULL) {
         gstk_free(STK_envelope_cmd.transacton_id.transaction_list);
       }
       if(STK_envelope_cmd.cause.cause != NULL) {
         gstk_free(STK_envelope_cmd.cause.cause);
       }
       gstk_util_release_upper_layer_cmd_memory(
         (gstk_cmd_type*)((void*)call_disconnected_evt_cmd));/*lint !e826 area too small */

       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_call_disconnected_evt_ind*/




/*===========================================================================
FUNCTION gstk_idle_scrn_evt_ind

DESCRIPTION

  This function builds the idle screen command and sends it to UIM

PARAMETERS
  idle_scrn_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_idle_scrn_evt_ind(
  const gstk_envelope_idle_scrn_evt_ind_type * idle_scrn_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_idle_scrn_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;
   int                                           i  = 0;

   if(idle_scrn_cmd == NULL)
   {
     MSG_ERROR("idle_scrn_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_idle_scrn_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_IDLE_SCRN_AVAILABLE,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_DISPLAY_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = idle_scrn_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.evt_list,
              (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN);

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.device_id,
              (int)(STK_envelope_cmd.device_id.device_tag_length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length 
                      + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: Idle Screen 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
       }

       /* nothing to free envelope idle screen evt ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_idle_scrn_evt_ind */



/*===========================================================================
FUNCTION gstk_user_act_evt_ind

DESCRIPTION

  This function builds the user activity command and sends it to UIM

PARAMETERS
  user_act_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_user_act_evt_ind(
  const gstk_envelope_user_act_evt_ind_type * user_act_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_user_act_evt_dl_command_type    STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;
   int                                           i  = 0;

   if(user_act_cmd == NULL)
   {
     MSG_ERROR("user_act_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_user_act_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_USER_ACTIVITY,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = user_act_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.evt_list,
              (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN);
       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.device_id,
              (int)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length 
                      + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: User Activity 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
           uim_cmd_ptr->envelope.data[i]);
       }

       /* nothing to free envelope user ativity evt ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_user_act_evt_ind */

/*===========================================================================
FUNCTION gstk_process_and_store_env_cmd_for_retry

DESCRIPTION

  This function stores the event download envelope information before the
  envelope is attempted to be sent to the card at the very first time.

PARAMETERS
  data_ptr [Input] : envelope data bytes
  len      [Input] : length of envelope data
  ref_id   [Input]:  envelope reference id in the envelope reference table

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_SUCCESS  : if envelope info is stored successfully.
    GSTK_ERROR    : if there is an envelope already pending.
    GSTK_BAD_PARAM: bad parameter

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_process_and_store_env_cmd_for_retry(
  const byte *data_ptr,
  uint32      len,
  uint32      ref_id)
{
  uint32 alloc_len = 0;
  if(!data_ptr)
  {
    MSG_ERROR("data_ptr is NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }
  if(len == 0)
  {
    MSG_ERROR("len is zero",0,0,0);
    return GSTK_BAD_PARAM;
  }
  if ((ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) || 
      (ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1))) 
  {
    MSG_ERROR("Invalid env ref id",0,0,0);
    return GSTK_BAD_PARAM;
  }
  if(gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_EVT_DL_NONE)
  {
    if(gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr != NULL)
    {
      /* Should we rather free the ptr and allocate? */
      MSG_ERROR("env apdu ptr should have been NULL : env_state 0x%x", 
                 gstk_curr_envelope_info.gstk_env_state, 0, 0);
      return GSTK_ERROR;
    }
    else
    {
      alloc_len = MIN(len, UIM_MAX_CHARS);
      MSG_HIGH("Allocating len 0x%x",alloc_len,0,0);
      gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr = gstk_malloc(alloc_len);
      if(gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr == NULL )
      {
        MSG_ERROR("env data buffer NULL ptr",0,0,0);
        return GSTK_MEMORY_ERROR;
      }
      gstk_memcpy( gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr, /* dest ptr */
                   data_ptr,                                               /* src ptr */
                   len,                                                    /* copy size */
                   alloc_len,                                              /* dest len */
                   UIM_MAX_CHARS);                                         /* src len */
      gstk_curr_envelope_info.env_data_bytes.data_len      = uint32touint8(alloc_len); 
      gstk_curr_envelope_info.env_ref_id                   = ref_id;
      gstk_curr_envelope_info.retry_info.max_no_of_retries = 
        GSTK_MAX_NUM_OF_EVT_DL_ENV_RETRY;
      return GSTK_SUCCESS;
    }
  }/* if(gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_EVT_DL_NONE) */
  else
  {
    /* We should not be processing another evt dl envelope when the state
       indicates that there is one already pending */
    MSG_ERROR("Wrong env state 0x%x : Attemping to send 2nd evt dl envelope - LS to the card", 
               gstk_curr_envelope_info.gstk_env_state, 0, 0);
    return GSTK_ERROR;
  }
}/* gstk_process_and_store_env_cmd_for_retry */

/*===========================================================================
FUNCTION gstk_location_status_evt_ind

DESCRIPTION

  This function builds the location status event download envelope command
  and sends it to UIM

PARAMETERS
  locaiton_status_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

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
gstk_status_enum_type gstk_location_status_evt_ind(
  const gstk_envelope_location_statue_ind_type    * location_status_evt_cmd)
{
  gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
  gstk_envelope_location_status_evt_dl_command_type       STK_envelope_cmd;
  uim_cmd_type                                            *uim_cmd_ptr;
  int                                                     offset = 0;
  int                                                     i  = 0;
  gstk_location_info_rsp_type                             location_info;
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
  gstk_cdma_location_info_type                            cdma_location_info;
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
  gsdi_card_apps_T                                        card_apps;
  uint8                                                   app = 0x00;

  if(location_status_evt_cmd == NULL)
  {
    MSG_ERROR("location_status_evt_cmd ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  } 

  /* initialize STK_envelope_cmd, location_info */
  memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_location_status_evt_dl_command_type));
  memset(&location_info, 0, sizeof(gstk_location_info_rsp_type));

  /* Pack various TLVs */
  STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

  /* event list */
  gstk_status = gstk_packer_evt_list_tlv(
      GSTK_LOCATION_STATUS,
      &STK_envelope_cmd.event_download_hdr.evt_list );

  if (gstk_status != GSTK_SUCCESS) {
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
        GSTK_ME_DEVICE,
        GSTK_UICC_SIM_DEVICE,
        &STK_envelope_cmd.event_download_hdr.device_id );

  if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
  }

  /* location status tag */
  gstk_status = gstk_packer_location_status_tlv(
      gstk_curr_location_status,
      &STK_envelope_cmd.location_st);

  if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
  }

  /* Check for Access Technology */

  switch(location_status_evt_cmd->location_info.info_type)
  {
  case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
    MSG_HIGH("Building GSTK_CDMA_LOCATION_INFO location status envelope",0,0,0);
    if(location_status_evt_cmd->location_status == GSTK_NORMAL_SERVICE)
    {
      memset(&cdma_location_info,0x00,sizeof(cdma_location_info));
      memcpy(cdma_location_info.mcc,cdma_location_info_cache.loc_info.mcc,GSTK_MCC_LEN);
      cdma_location_info.imsi_11_12 = 
            cdma_location_info_cache.loc_info.imsi_11_12;
      memcpy(cdma_location_info.sid_info.sid,
            cdma_location_info_cache.loc_info.sid_info.sid,
            cdma_location_info_cache.loc_info.sid_info.id_len);
      cdma_location_info.sid_info.id_len = 
          cdma_location_info_cache.loc_info.sid_info.id_len;
      memcpy(cdma_location_info.nid_info.nid,
            cdma_location_info_cache.loc_info.nid_info.nid,
            cdma_location_info_cache.loc_info.nid_info.id_len);
      cdma_location_info.nid_info.id_len = 
          cdma_location_info_cache.loc_info.nid_info.id_len;
      memcpy(cdma_location_info.base_id_info.base_id,
            cdma_location_info_cache.loc_info.base_id_info.base_id,
            cdma_location_info_cache.loc_info.base_id_info.id_len);
      cdma_location_info.base_id_info.id_len = 
          cdma_location_info_cache.loc_info.base_id_info.id_len;
      memcpy(cdma_location_info.base_lat,
            cdma_location_info_cache.loc_info.base_lat,
            GSTK_BASE_LAT_LEN);
      memcpy(cdma_location_info.base_long,
            cdma_location_info_cache.loc_info.base_long,
            GSTK_BASE_LONG_LEN);
      gstk_status = gstk_packer_cdma_location_info_tlv(
        &cdma_location_info,
        &STK_envelope_cmd.location_info.cdma_location_info);
      if (gstk_status != GSTK_SUCCESS) 
      {
        return gstk_status;
      }
    }
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;

  case GSTK_GSM_UMTS_LOCATION_INFO:
    /* ONLY For GSM UMTS Location Type */
    /* location information tag */
    MSG_HIGH("Building GSTK_GSM_UMTS_LOCATION_INFO location status envelope",0,0,0);
    if(gstk_curr_location_status == GSTK_NORMAL_SERVICE) 
    {
      location_info.cell_id_len = gstk_curr_cell_id.cell_len;      
      if (gstk_curr_cell_id.cell_len == GSTK_MAX_CELL_ID_LEN) 
      {
        gsdi_get_apps_available(&card_apps);

        app = card_apps.apps_enabled & GSDI_GSM_SIM_MASK;
        if(app)
        {
          location_info.cell_id_len = GSTK_MIN_CELL_ID_LEN;
          /* Assign it to the cell_len from RRC */
        }
        else if(card_apps.apps_enabled & GSDI_USIM_APP_MASK)
        {
          location_info.cell_id_len = GSTK_MAX_CELL_ID_LEN;
          /* Assign it to the cell_len from RRC */
        }
      }

      memcpy(location_info.cell_id, gstk_curr_cell_id.cell_id,
              (uint32)location_info.cell_id_len);
      memcpy(location_info.mcc_and_mnc, gstk_curr_mcc_mnc, 3);
      memcpy(location_info.loc_area_code, gstk_curr_lac, 2);
      gstk_status = gstk_packer_location_info_tlv(
        &location_info,
        &STK_envelope_cmd.location_info.gsm_umts_location_info);

      if (gstk_status != GSTK_SUCCESS) {
        return gstk_status;
        }
      MSG_HIGH("  Envelope  Location status is %d...... ",gstk_curr_location_status ,0,0);
      MSG_HIGH("  Envelope  MNC_MCC is 0x%x 0x%x 0x%x",
          location_info.mcc_and_mnc[0],
          location_info.mcc_and_mnc[1],
          location_info.mcc_and_mnc[2]);

      MSG_HIGH(" Envelope LAC is 0x%x 0x%x",
          location_info.loc_area_code[0],
          location_info.loc_area_code[1],
          0);
      MSG_HIGH("Envelope cell id len 0x%x",location_info.cell_id_len,0,0);
      if(location_info.cell_id_len >= 2)
      {
        MSG_HIGH("Envelope Cell Id [0][1]: 0x%x 0x%x",
            location_info.cell_id[0],
            location_info.cell_id[1],
            0);
      }
      if(location_info.cell_id_len >= 4)
      {
          MSG_HIGH("Envelope Cell Id [2][3]: 0x%x 0x%x",
          location_info.cell_id[2],
          location_info.cell_id[3],         
          0);
      }
    }
    break;
  default:
    MSG_ERROR("Invalid Location Info Type",0,0,0);
    /* Need to reset pending envelope flag ? */
    return GSTK_ERROR;
  }/* End of switch location_status_evt_cmd->location_info.location_type */

  /* total data length:  evt id data + 2
                      + device id data + 2 (2 is for the tag and length field)
                      + location status data + 2
                      + location info + 2 (C)
  */
  STK_envelope_cmd.event_download_hdr.length =
                            STK_envelope_cmd.event_download_hdr.evt_list.length +
                            STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                            STK_envelope_cmd.location_st.length +
                            6;

  if(gstk_curr_location_status == GSTK_NORMAL_SERVICE) 
  {
    switch(location_status_evt_cmd->location_info.info_type)
    {
    case GSTK_GSM_UMTS_LOCATION_INFO:
      STK_envelope_cmd.event_download_hdr.length =
        STK_envelope_cmd.event_download_hdr.length +
          STK_envelope_cmd.location_info.gsm_umts_location_info.length + 2;
      break;

    case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
      STK_envelope_cmd.event_download_hdr.length =
        STK_envelope_cmd.event_download_hdr.length +
        STK_envelope_cmd.location_info.cdma_location_info.length + 2;
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;

    case GSTK_NO_SRVC_LOCATION_INFO:
    case GSTK_IMEI_INFO:
    case GSTK_NMR_BCCH_INFO:
    case GSTK_NMR_UTRAN_INFO:
    case GSTK_DATE_TIME_ZONE_INFO:
    case GSTK_LANGUAGE_SETTING_INFO:
    case GSTK_TIMING_ADVANCE_INFO:
    case GSTK_ACCESS_TECHNOLOGY_INFO:
    case GSTK_ESN_INFO:
    case GSTK_IMEISV_INFO:
    case GSTK_BATTERY_STATUS_INFO:
    case GSTK_MEID_INFO:
    case GSTK_INVALID_LOC_INFO:
    case GSTK_NETWORK_SEARCH_MODE_INFO:
      MSG_ERROR("Invalid general local info type 0x%x", 
        location_status_evt_cmd->location_info.info_type, 0, 0);
      return GSTK_ERROR;
    }
  }
  /* Send command to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
      /* set to 0 so that uim will get the data from index 0 */
      uim_cmd_ptr->envelope.offset = 0x00;

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = location_status_evt_cmd->message_header.user_data;

      /* populate the envelope command info */
      uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                      + GSTK_TAG_LENGTH_LEN;

      uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
      uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

      /* copy evt list */
      memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.event_download_hdr.evt_list,
            (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                  + GSTK_TAG_LENGTH_LEN));
      offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                  + GSTK_TAG_LENGTH_LEN);
      /* copy device */
      memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.event_download_hdr.device_id,
            (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
              + GSTK_TAG_LENGTH_LEN));
      offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN);
      /* copy location status */
      memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.location_st,
            (int)(STK_envelope_cmd.location_st.length + GSTK_TAG_LENGTH_LEN));
      offset = (int)(offset + STK_envelope_cmd.location_st.length + GSTK_TAG_LENGTH_LEN);

        /* copy location info */
      if(gstk_curr_location_status == GSTK_NORMAL_SERVICE) 
      {
        switch(location_status_evt_cmd->location_info.info_type)
        {
        case GSTK_GSM_UMTS_LOCATION_INFO:
          memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.location_info.gsm_umts_location_info,
                (int)(STK_envelope_cmd.location_info.gsm_umts_location_info.length + GSTK_TAG_LENGTH_LEN));
          offset = (int)(offset + STK_envelope_cmd.location_info.gsm_umts_location_info.length);
          break;

        case GSTK_CDMA_LOCATION_INFO:
#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
        memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.location_info.cdma_location_info,
                (int)(STK_envelope_cmd.location_info.cdma_location_info.length + GSTK_TAG_LENGTH_LEN));
          offset = (int)(offset + STK_envelope_cmd.location_info.cdma_location_info.length);
#endif /* #ifdef FEATURE_CCAT */
#endif /* #ifdef FEATURE_CAT_REL6 */
          break;

        default:
          break;
        }
      }

      MSG_HIGH("Env: location info evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
      for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
        MSG_HIGH("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i],
                  uim_cmd_ptr->envelope.data[i]);
      }
      /* Only if envelope state is GSTK_NONE, change state to RSP_FROM_CARD
         and store the env payload
      */
      if(gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        gstk_status = gstk_process_and_store_env_cmd_for_retry(
                        uim_cmd_ptr->envelope.data,
                        (uint32)uim_cmd_ptr->envelope.num_bytes,
                        uim_cmd_ptr->hdr.user_data);
        if( gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_EVT_DL_RSP_FROM_CARD;
        }
        else
        {
          MSG_HIGH("FAIL - no retry will be attempted ", 0, 0, 0);
          gstk_status = GSTK_SUCCESS;
        }
      }/* if(gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
      else
      {
        MSG_HIGH("Envelope retry not attempted to the card as flag is set to FALSE", 0, 0, 0); 
      }

      /* no free for envelope location info ind and tlv */
      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
  } /* uim_cmd_ptr != NULL */
  else {
    MSG_ERROR("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  return gstk_status;

} /* gstk_location_status_evt_ind*/

/*===========================================================================
FUNCTION gstk_lang_selection_evt_ind 

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_lang_selection_evt_ind(
  const gstk_envelope_lang_sel_evt_ind_type    * lang_select_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_language_evt_dl_command_type              STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;
   int                                                     i  = 0;

   if(lang_select_evt_cmd == NULL)
   {
     MSG_ERROR("lang_select_evt_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_language_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_LANGUAGE_SELECTION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* lang code tag */
   gstk_status = gstk_packer_lang_sel_tlv(
       lang_select_evt_cmd->lang_code.lang_code,
       &STK_envelope_cmd.language);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + lang sel data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.language.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = lang_select_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                   + GSTK_TAG_LENGTH_LEN);
       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                  + GSTK_TAG_LENGTH_LEN);
       /* copy language */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.language,
              (int)(STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: lang select evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
       }

       /* no free for envelope location info ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_lang_selection_evt_ind*/


/*===========================================================================
FUNCTION gstk_browser_term_evt_ind 

DESCRIPTION

  This function builds the browser termination event download envelope command
  and sends it to UIM

PARAMETERS
  browser_term_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_browser_term_evt_ind(
  const gstk_envelope_browser_term_evt_ind_type    * browser_term_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_browser_termination_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;
   int                                                     i  = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_browser_termination_evt_dl_command_type));
   if (browser_term_evt_cmd == NULL) {
      return GSTK_ERROR;
   }

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_BROWSER_TERMINATION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* browser termination cause tag */
   gstk_status = gstk_packer_browser_term_cause_tlv(
       &browser_term_evt_cmd->browser_termination_cause,
       &STK_envelope_cmd.cause);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + termination cause data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.cause.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = browser_term_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                   + GSTK_TAG_LENGTH_LEN);
       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                  + GSTK_TAG_LENGTH_LEN);
       /* copy cause */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.cause,
              (int)(STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: Browser Term evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
       }

       /* no free for envelope location info ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_browser_term_evt_ind*/

/*===========================================================================
FUNCTION gstk_data_avail_evt_ind 

DESCRIPTION

  This function builds the data available event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_data_avail_evt_ind(
  const gstk_envelope_data_avail_evt_ind_type *data_avail_evt_cmd
)
{
  gstk_status_enum_type                          gstk_status         = GSTK_SUCCESS;
  gstk_envelope_data_avail_evt_dl_command_type   STK_envelope_cmd;
  uim_cmd_type                                  *uim_cmd_ptr;
  int32                                          offset              = 0;
  int32                                          i                   = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_data_avail_evt_dl_command_type));
   
   if (data_avail_evt_cmd == NULL) 
   {
      return GSTK_ERROR;
   }

   /* Pack various TLVs 
   1. Event List TLV
   2. Device Id TLV
   3. Channel Status TLV
   4. Channel Data Length TLV
   */

   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* pack the event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_DATA_AVAILABLE,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) 
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   gstk_status = gstk_packer_ch_status_tlv(
     &data_avail_evt_cmd->ch_status_info,
     &STK_envelope_cmd.ch_status
     );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   gstk_status = gstk_packer_ch_data_len_tlv(
     data_avail_evt_cmd->ch_data_len,
     &STK_envelope_cmd.ch_data_len);

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* DATA LENGTH :  evt id data + 2 (2 is for the tag and length field)
                   + device id data + 2 (2 is for the tag and length field)
                   + channel status + 2 (2 is for the tag and length field)
                   + channel data len + 2 (2 is for the tag and length field)
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.ch_status.ch_status_length + 
                             STK_envelope_cmd.ch_data_len.ch_data_len_length + 
                             8;
   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data      = data_avail_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                   + GSTK_TAG_LENGTH_LEN);

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                  + GSTK_TAG_LENGTH_LEN);
       
       /* copy channel status */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
         &STK_envelope_cmd.ch_status,
              (int)(STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN);

       /* copy channel data length */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
         &STK_envelope_cmd.ch_data_len,
              (int)(STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.ch_data_len.ch_data_len_length + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: Data Avail evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) 
       {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
       }

       /* no free for envelope location info ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else 
   {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }
   return gstk_status;
}/* gstk_data_avail_evt_ind */

/*===========================================================================
FUNCTION gstk_ch_status_evt_ind 

DESCRIPTION

  This function builds the channel status event download envelope command
  and sends it to UIM

PARAMETERS
  data_avail_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_ch_status_evt_ind(
  const gstk_envelope_ch_status_evt_ind_type *ch_status_evt_cmd
)
{
  gstk_status_enum_type                          gstk_status         = GSTK_SUCCESS;
  gstk_envelope_ch_status_evt_dl_command_type    STK_envelope_cmd;
  uim_cmd_type                                  *uim_cmd_ptr;
  int32                                          offset              = 0;
  int32                                          i                   = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_ch_status_evt_dl_command_type));
   
   if (ch_status_evt_cmd == NULL) 
   {
      return GSTK_ERROR;
   }

   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* Pack various TLVs 
   1. Event List TLV
   2. Device Id TLV
   3. Channel Status TLV
   */

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_CHANNEL_STATUS,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) 
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* Channel status TLV */
   gstk_status = gstk_packer_ch_status_tlv(
     &ch_status_evt_cmd->ch_status_info,
     &STK_envelope_cmd.ch_status
     );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* DATA LENGTH :  evt id data + 2 (2 is for the tag and length field)
                   + device id data + 2 (2 is for the tag and length field)
                   + channel status + 2 (2 is for the tag and length field)
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.ch_status.ch_status_length +
                             6;
   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data      = ch_status_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                   + GSTK_TAG_LENGTH_LEN);

       /* copy device */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.event_download_hdr.device_id,
              (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                  + GSTK_TAG_LENGTH_LEN);

       /* copy channel status */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
         &STK_envelope_cmd.ch_status,
         (int)(STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.ch_status.ch_status_length + GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: Data Avail evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) 
       {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
       }

       /* no free for envelope location info ind and tlv */
       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else 
   {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }
   return gstk_status;
}/* gstk_ch_status_evt_ind */

/*===========================================================================
FUNCTION gstk_access_tech_evt_ind 

DESCRIPTION

  This function builds the access tech event download envelope command
  and sends it to UIM

PARAMETERS
  access_tech_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_access_tech_evt_ind(
  const gstk_envelope_access_tech_ind_type    *access_tech_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_access_tech_evt_dl_command_type           STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;
   int                                                     i  = 0;

   if(access_tech_evt_cmd == NULL)
   {
     MSG_ERROR("access_tech_evt_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   MSG_HIGH("In gstk_access_tech_evt_ind",0,0,0);

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_access_tech_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_ACCESS_TECH,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) 
   {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* lang code tag */
   gstk_status = gstk_packer_access_technology_tlv(
       &access_tech_evt_cmd->access_tech,
       &STK_envelope_cmd.access_tech);

   if (gstk_status != GSTK_SUCCESS) 
   {
     return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + lang sel data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.access_tech.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr == NULL ) 
   {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
   if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
   {
     /* set to 0 so that uim will get the data from index 0 */
     uim_cmd_ptr->envelope.offset = 0x00;

     /* set user_data */
     uim_cmd_ptr->hdr.user_data               = access_tech_evt_cmd->message_header.user_data;

     /* populate the envelope command info */
     uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length 
                                        + GSTK_TAG_LENGTH_LEN;

     uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
     uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

     /* copy evt list */
     memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.event_download_hdr.evt_list,
            (int)(STK_envelope_cmd.event_download_hdr.evt_list.length 
                  + GSTK_TAG_LENGTH_LEN));
     offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length 
                 + GSTK_TAG_LENGTH_LEN);
     /* copy device */
     memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.event_download_hdr.device_id,
            (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
              + GSTK_TAG_LENGTH_LEN));
     offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length 
                + GSTK_TAG_LENGTH_LEN);
     /* copy  access technology info */
     memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &STK_envelope_cmd.access_tech,
            (int)(STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN));
     offset = (int)(offset + STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN);

     MSG_MED("Env: Access Technology evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
     for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) 
     {
       MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                   uim_cmd_ptr->envelope.data[i]);
     }

     /* no free for envelope location info ind and tlv */
     MSG_HIGH("Sending Access Tech envelope to UIM",0,0,0);
     gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
   }
   return gstk_status;

} /* gstk_access_tech_evt_ind*/
/*lint -e506 lonstant value boolean */



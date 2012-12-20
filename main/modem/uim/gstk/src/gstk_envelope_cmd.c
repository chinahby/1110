/*===========================================================================


            G S T K   E N V E L O P E   C M D

GENERAL DESCRIPTION
  This source file contains functions to handle envelope commands (except
  for the event download commands which are handled in gstk_evt_dl.c


FUNCTIONS
  gstk_menu_selection_ind
    This function builds the menu selection command and sends it to UIM

  gstk_sms_pp_dl_ind
    This function builds the sms pp download command and sends it to UIM

  gstk_cell_broadcast_dl_ind
    This function builds the cell broadcast download command and sends it
    to UIM

  gstk_cc_ind
    This function builds the call control command and sends it to UIM

  gstk_mo_sms_ctrl_ind
    This function builds the MO SMS control command and sends it to UIM

  gstk_timer_expire_ind
    This function builds the timer expiration command and sends it to UIM

  gstk_process_envelope_cmd
    This function switches on the command id and call the corresponding
    envelope command ind function so that GSTK can build the corrresponding
    envelope command tlvs and send it to UIM Server

  gstk_build_postpone_envelope
    This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

  gstk_raw_envelope_ind_cleanup
    This function cleans up any allocated pointer on the heap.

  gstk_raw_envelope_ind
    This function packages the raw envelope command and sends it to UIM

INITIALIZATION AND SEQUENCING REQUIREMENTS


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009, 2011 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/uim/su/baselines/sqa/gstk/rel/07H1_2/src/gstk_envelope_cmd_c/rev2/gstk_envelope_cmd.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/11   adp     Fixed buffer over flow issue in SMS-PP DL 
01/19/09   yb      Added secondary support for cell id information when 
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
10/07/08   xz      Fixed Klockwork errors
09/14/08   sk      Fixed lint errors
08/08/08   xz      Added raw envelope command
05/06/08   sk      Added support for event download envelope retry
03/17/08   sk      Fixed klockwork errors
08/01/07   sk      Fixed lint error
07/23/07   sk      Fixed warning
07/14/07   sk      Added support to requeue sms pp envelope if there is a 
                   pending send sms tr
07/06/07   sk      Fixed typo
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
12/14/06   sk      Lint fixes
10/23/06   sk      Lint fixes
07/26/06   nk      Added check to prevent buffer overflow
07/25/06   sk      Added support to handle envelope cmds in non idle state
06/07/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mode
05/23/06   tml     GPRS Call Control support and lint
05/18/06   nk      Fixed High Lint Error:  Status defined but not used for
                   CDMA Targets.
05/18/06   sk      Fixed warnings for 1X builds
05/17/06   sk      Lint fixes
04/06/06   sk      Support for informing clients of envelope failure.
04/04/06   sk      Added new function to build location status envelope
03/29/06  sk/tml   Allow all GSM cell ID to go through for Call Control and MO
                   SMS Control
03/13/06   sk      Fixed typo error in gstk_cc_ind()
03/06/06   sk      Fixed location issue for call control and mo sms control
                   location object - obtain cell id from rrc loci table
                   for UTRAN network, mnc mcc lac info from the gstk cache.
                   Added Lint Fixes
11/09/05   sk      Fixed lint errors
09/15/05   tml     Fixed link error
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/14/05   sk      Reomoved warnings
07/13/05   sk      Added support for FEATURE_STOP_ENV_CMD_PROCESSING
                   Added new function gstk_build_access_technology_env()
05/11/05   sst     Lint fixes
11/23/04    sk     Added SAT/USAT data support
08/19/04   tml     Added cell broadcast support
08/10/04   tml     Added gstk_build_postpone_envelope
05/26/04   tml     Added More debug messages
                   Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/02/04   tml     linted
02/24/04   tml     compilation fix for CDMA only target
09/04/03   tml     Dual slot support
07/25/03   tml     ensured only valid address is being passed to the Card in
                   MO SMS Control
06/23/03   tml     removed compiler error for 1x/gsm
05/21/03   tml     linted
04/21/03   tml     Added is_cdma_sms boolean support
04/15/03   tml     Fixed MOSMS control envelope tag
03/20/03   tml     Properly featurize local info for WCDMA and GSM.
                   Fixed offset for envelope responses
03/06/03   tml     Updated enum names, Added FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "uim.h"
#include "gstkutil.h"
#include "tmc.h"
#include "string.h"
#include "msg.h"
#include "err.h"
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

#define GSTK_EVT_LIST_VAL_LEN 1
#define GSTK_UIM_MAX_CHARS  512
/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  cb_dl_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_cell_broadcast_dl_ind_cleanup(
    gstk_envelope_cell_broadcast_download_ind_type*     cb_dl_cmd,
    gstk_envelope_cell_broadcast_download_command_type* STK_envelope_cmd)
{

  if(STK_envelope_cmd == NULL)
  {
    MSG_ERROR("STK_envelope cmd ERR : NULL",0,0,0);
    return;
  }
  /* free envelope page dataind and tlv */
  /* free page tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)cb_dl_cmd));
  if(STK_envelope_cmd->cb_page.cb_page != NULL) {
    gstk_free(STK_envelope_cmd->cb_page.cb_page);
    STK_envelope_cmd->cb_page.cb_page = NULL;
  }
} /* gstk_cell_broadcast_dl_ind_cleanup */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

/*===========================================================================
FUNCTION gstk_sms_pp_dl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  sms_pp_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_sms_pp_dl_ind_cleanup(
    gstk_envelope_sms_pp_download_ind_type*           sms_pp_cmd)
{
  /* free envelope sms tpdu ind and tlv */
  /* tpdu tlv is [] so no need to free */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)sms_pp_cmd));
} /* gstk_sms_pp_dl_ind_cleanup */ /*lint !e715 */

/*===========================================================================
FUNCTION gstk_cc_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  cc_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_cc_ind_cleanup(
    gstk_envelope_cc_ind_type*     cc_cmd,
    gstk_envelope_cc_command_type* STK_envelope_cmd)
{

  if(STK_envelope_cmd == NULL)
  {
    MSG_ERROR("STK_envelope_cmd ERR:NULL",0,0,0);
    return;
  }

  if(cc_cmd == NULL)
  {
    MSG_ERROR("cc_cmd ERR:NULL",0,0,0);
    return;
  }
  /* free envelope cc ind and tlv */
  if(cc_cmd->cc_data.call_enum_type == GSTK_USSD) {
    if(STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text != NULL) {
      gstk_free(STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text);
      STK_envelope_cmd->gstk_address_string_tlv.ussd_string.text = NULL;
    }
  }
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)cc_cmd));
} /* gstk_cc_ind_cleanup */


/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  mo_sms_ctrl_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_mo_sms_ctrl_ind_cleanup(
    gstk_envelope_mo_sms_ctrl_ind_type*           mo_sms_ctrl_cmd)
{
  /* free envelope mo sms ctrl ind and tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)mo_sms_ctrl_cmd));
} /* gstk_mo_sms_ctrl_ind_cleanup */ /*lint !e715 */


/*===========================================================================
FUNCTION gstk_menu_selection_ind

DESCRIPTION

  This function builds the menu selection command and sends it to UIM

PARAMETERS
  menu_sel_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_menu_selection_ind(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_menu_selection_command_type   STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   int                                         i  = 0;

   if(menu_sel_cmd == NULL)
   {
     MSG_ERROR("menu_sel_cmd ERR : NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_menu_selection_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_MENU_SELECTION_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_KEYPAD_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Menu Sel: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
       return gstk_status;
   }

   /* item ID tag */
   gstk_status = gstk_packer_item_id_tlv(
       menu_sel_cmd->chosen_item_id,
       &STK_envelope_cmd.item_chosen);

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Menu Sel: Item ID Packing Fail 0x%x", gstk_status, 0, 0);
       return gstk_status;
   }

   /* help request tag */
   if(menu_sel_cmd->request_help) { /* user requested help */
     gstk_status = gstk_packer_help_request_tlv(
         menu_sel_cmd->request_help,
         &STK_envelope_cmd.request_help);

     if (gstk_status != GSTK_SUCCESS) {
         MSG_ERROR("In Menu Sel: Help Packing Fail 0x%x", gstk_status, 0, 0);
         return gstk_status;
     }
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + item chosen data + 2
                        + request help data + 2
   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.item_chosen.length +
                             4;
   if(menu_sel_cmd->request_help) { /* user requested help */
     STK_envelope_cmd.length = STK_envelope_cmd.length +
                               STK_envelope_cmd.request_help.length +
                               2;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
         /* required so that uim will grap the data from index 0 */
         uim_cmd_ptr->envelope.offset = 0;

         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = menu_sel_cmd->message_header.user_data;

         /* populate the envelope command info */
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

         /* copy device */
         /*STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                          &STK_envelope_cmd.device_id,
                          offset,
                          (STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN),
                          UIM_MAX_CHARS,
                          sizeof(gstk_device_identities_tag_tlv_type));
         offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN);
         /* copy item id */
         /* STK_envelope_cmd.item_chosen.length fixed in packer function */
         gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                          &STK_envelope_cmd.item_chosen,
                          offset,
                          (STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN),
                          UIM_MAX_CHARS,
                          sizeof(gstk_item_identifier_tlv_type));
         offset = (int)(offset + STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN);

         /* copy help requested */
         /* STK_envelope_cmd.request_help.length fixed in packer function */
         if(offset < (STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN) ) { /* has help */
           gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                            &STK_envelope_cmd.request_help,
                            offset,
                            (STK_envelope_cmd.request_help.length + GSTK_TAG_LENGTH_LEN),
                            UIM_MAX_CHARS,
                            sizeof(gstk_help_request_tlv_type));
           offset = (int)(offset + STK_envelope_cmd.request_help.length + GSTK_TAG_LENGTH_LEN);
         }

         MSG_MED("Env: Menu Sel 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
         for(i = 0; i< uim_cmd_ptr->envelope.num_bytes; i++) {
           MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
         }

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   /* nothing to free for menu selection ind */
   return gstk_status;

} /* gstk_menu_selection_ind */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

/*===========================================================================
FUNCTION gstk_sms_pp_dl_ind

DESCRIPTION

  This function builds the sms pp download command and sends it to UIM

PARAMETERS
  sms_pp_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_sms_pp_dl_ind(
          gstk_envelope_sms_pp_download_ind_type * sms_pp_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_sms_pp_download_command_type  STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   int                                         i  = 0;
   uint32                                      addr_length=0;

   if(sms_pp_cmd == NULL)
   {
     MSG_ERROR("sms_pp_cmd ERR : NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_sms_pp_download_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_SMS_PP_DOWNLOAD_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In SMS PP: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
       return gstk_status;
   }

   /* optional address tag */
   if(sms_pp_cmd->address.length > 0) {
     gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &sms_pp_cmd->address,
       &STK_envelope_cmd.address);

     if (gstk_status != GSTK_SUCCESS) {
         MSG_ERROR("In SMS PP: Address Packing Fail 0x%x", gstk_status, 0, 0);
         gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
         return gstk_status;
     }
   }

   /* tpdu tag */
   gstk_status = gstk_packer_sms_tpdu_tlv(
         sms_pp_cmd->is_cdma_sms_pp,
         &sms_pp_cmd->tpdu,
         &STK_envelope_cmd.tpdu);

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In SMS PP: TPDU Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
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
         uim_cmd_ptr->hdr.user_data               = sms_pp_cmd->message_header.user_data;

         /* initialized to 3 for value portion of the TLVs
            i.e., the final offset will be either 0 or 1 depends
            on whether the total length is > 0x7F or not */
         offset = 3;

         /* copy device */
         /* No Need to check STK_envelope_cmd.device_id.device_tag_length as is
            fixed length in packer function
         */
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.device_id,
                (int)(STK_envelope_cmd.device_id.device_tag_length +
                GSTK_TAG_LENGTH_LEN));
         offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);

         /* total length: + device tlv */
         STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                   GSTK_TAG_LENGTH_LEN;

         /* copy optional address */
         if(sms_pp_cmd->address.length > 0){
           addr_length = STK_envelope_cmd.address.length + GSTK_TAG_LENGTH_LEN;
           if (gstk_check_envelope_offset(uim_cmd_ptr, offset, addr_length) != GSTK_SUCCESS){
             gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
             return GSTK_MEMORY_ERROR;
           }
           memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.address,
                addr_length);
           offset = offset + uint32toint32(addr_length );
           /* total length: + address tlv */
           STK_envelope_cmd.length += uint32touint8(addr_length);
         }

         if(offset > UIM_MAX_CHARS - 1)
         {
           MSG_ERROR("Length of command exceeding Max Chars in UIM Buffer", 0, 0, 0);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_MEMORY_ERROR;
         }

         /* copy tpdu */
         /* copy tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tpdu.tag;
         if (STK_envelope_cmd.tpdu.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
            if(offset > UIM_MAX_CHARS - 1)
            {
              MSG_ERROR("Length of command exceeding Max Chars in UIM Buffer", 0, 0, 0);
              gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
              return GSTK_MEMORY_ERROR;
            }

           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
         if(offset > UIM_MAX_CHARS - 1)
         {
           MSG_ERROR("Length of command exceeding Max Chars in UIM Buffer", 0, 0, 0);
           gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
           return GSTK_MEMORY_ERROR;
         }

         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tpdu.length;

         if (gstk_check_envelope_offset(uim_cmd_ptr, offset, STK_envelope_cmd.tpdu.length) !=
                                                                   GSTK_SUCCESS){
           return GSTK_MEMORY_ERROR;
         }
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.tpdu.sms_tpdu,
                int32touint32(STK_envelope_cmd.tpdu.length));
         offset = offset + STK_envelope_cmd.tpdu.length;
         /* total length: + tpdu tlv */
         STK_envelope_cmd.length += STK_envelope_cmd.tpdu.length + GSTK_TAG_LENGTH_LEN;

         /* populate the envelope command info */
         if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
           uim_cmd_ptr->envelope.offset = 0;
           uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           STK_envelope_cmd.length++;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }
         else {
           uim_cmd_ptr->envelope.offset = 1;
           uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }

         MSG_MED("Env: SMS PP 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
         for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
           MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
         }

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_sms_pp_dl_ind_cleanup(sms_pp_cmd);
   return gstk_status;

} /* gstk_sms_pp_dl_ind */



/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind

DESCRIPTION

  This function builds the cell broadcast download command and sends it to UIM

PARAMETERS
  cb_dl_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_cell_broadcast_dl_ind(
          gstk_envelope_cell_broadcast_download_ind_type * cb_dl_cmd)
{
   gstk_status_enum_type                              gstk_status = GSTK_SUCCESS;
   gstk_envelope_cell_broadcast_download_command_type STK_envelope_cmd;
   uim_cmd_type                                      *uim_cmd_ptr;
   int                                                offset = 0;
   int                                                i  = 0;

   if(cb_dl_cmd == NULL)
   {
     MSG_ERROR("cb_dl_cmd ERR : NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_cell_broadcast_download_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_CELL_BROADCAST_DOWNLOAD_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Cell Broadcast: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   /* cb page tag */
   gstk_status = gstk_packer_cell_broadcast_page_tlv(
         &cb_dl_cmd->cb_page,
         &STK_envelope_cmd.cb_page);

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Cell Broadcast: CB Page Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
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
         uim_cmd_ptr->hdr.user_data               = cb_dl_cmd->message_header.user_data;

         /* initialized to 3 for value portion of the TLVs
            i.e., the final offset will be either 0 or 1 depends
            on whether the total length is > 0x7F or not */
         offset = 3;

         /* copy device */
         /* STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.device_id,
                (int)(STK_envelope_cmd.device_id.device_tag_length +
                GSTK_TAG_LENGTH_LEN));
         offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);

         /* total length: + device tlv */
         STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                   GSTK_TAG_LENGTH_LEN;

         /* copy cellbroadcast page */
         /* copy tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.tag;
         if (STK_envelope_cmd.cb_page.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.length;

         /* STK_envelope_cmd.cb_page.length fixed in packer function */
         memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.cb_page.cb_page,
                (int)(STK_envelope_cmd.cb_page.length));
         offset = (int)(offset + STK_envelope_cmd.cb_page.length);
         /* total length: + cb page tlv */
         STK_envelope_cmd.length += STK_envelope_cmd.cb_page.length + GSTK_TAG_LENGTH_LEN;

         /* populate the envelope command info */
         if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
           uim_cmd_ptr->envelope.offset = 0;
           uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           STK_envelope_cmd.length++;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }
         else {
           uim_cmd_ptr->envelope.offset = 1;
           uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }

         MSG_MED("Env: Cell Broadcast DL 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
         for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
           MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
         }

         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);

   return gstk_status;

} /* gstk_cell_broadcast_dl_ind */



/*===========================================================================
FUNCTION gstk_cc_ind

DESCRIPTION

  This function builds the call control command and sends it to UIM

PARAMETERS
  cc_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_cc_ind(
          gstk_envelope_cc_ind_type * cc_cmd)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  gstk_envelope_cc_command_type               STK_envelope_cmd;
  uim_cmd_type                                *uim_cmd_ptr;
  int32                                       offset = 0;
  int                                         i  = 0;
  gstk_location_info_rsp_type                 location_info;
  uint32                                       addr_length=0;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*defined(FEATURE_GSM) || defined(FEATURE_WCDMA)*/

  if(cc_cmd == NULL)
  {
    MSG_ERROR("cc_cmd ERR : NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_envelope_cmd */
  memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_cc_command_type));

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

  /* initialize location_info */
  memset(&location_info, 0, sizeof(gstk_location_info_rsp_type));

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

  if (cc_cmd->cc_data.call_enum_type == GSTK_PDP_CONTEXT_ACT)
  {
    gstk_device_id_type          dev_id;
    gstk_location_info_gen_type  gen_location_info;

    /* Send command to STK application on SIM */
    /* Send Message to UIM */
    /* get a buffer for the request */
    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr == NULL) {
      ERR("no UIM cmd buf", 0, 0, 0);
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_MEMORY_ERROR;
    }
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL)) {
      ERR("UIM cmd buf becomes null 0x%x or status != success 0x%x", uim_cmd_ptr, gstk_status, 0);
      if (uim_cmd_ptr != NULL)
      {
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        uim_cmd_ptr = NULL;
      }
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_MEMORY_ERROR;
    }
#ifdef FEATURE_GSTK_DUAL_SLOTS
    if (cc_cmd->slot_info_present) {
      uim_cmd_ptr->hdr.slot = cc_cmd->slot;
    }
#endif /*FEATURE_GSTK_DUAL_SLOTS */

    /* set user_data */
    uim_cmd_ptr->hdr.user_data               = cc_cmd->message_header.user_data;

    /* initialized to 3 for value portion of the TLVs
    i.e., the final offset will be either 0 or 1 depends
    on whether the total length is > 0x7F or not */

    offset = 3;
    /* Initialize to the beginning offset for final calculation below */
    uim_cmd_ptr->envelope.num_bytes = int32touint8(offset);

    /* copy device */
    dev_id.src = GSTK_ME_DEVICE;
    dev_id.dest = GSTK_UICC_SIM_DEVICE;
    gstk_status = gstk_util_populate_tlv(
                    GSTK_DEVICE_IDENTITY_TAG,
                    offset,
                    (gstk_cmd_to_uim_type*)&dev_id,
                    uim_cmd_ptr->envelope.data,
                    &offset);/*lint !e826 dev_id suppress area too small */

    if (gstk_status != GSTK_SUCCESS) {
      ERR ("status 0x%x for Device", gstk_status, 0, 0);
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    /* copy PDP Param */
    gstk_status = gstk_util_populate_tlv(
                    GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG,
                    offset,
                    (gstk_cmd_to_uim_type*)&cc_cmd->cc_data.gstk_address_string.pdp_context,
                    uim_cmd_ptr->envelope.data,
                    &offset);

    if (gstk_status != GSTK_SUCCESS) {
      ERR ("status 0x%x for PDP Context", gstk_status, 0, 0);
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    /* copy location info */
    gen_location_info.location_info_tech = GSTK_GSM_UMTS_LOCATION_INFO;
    memcpy(&gen_location_info.location_info.gsm_umts_loc_info,
           &location_info,
           sizeof(gstk_location_info_rsp_type));
    gstk_status = gstk_util_populate_tlv(
                    GSTK_LOCATION_INFORMATION_TAG,
                    offset,
                    (gstk_cmd_to_uim_type*)&gen_location_info,
                    uim_cmd_ptr->envelope.data,
                    &offset);/*lint !e826 gen_location_info suppress area too small */

    if (gstk_status != GSTK_SUCCESS) {
      ERR ("status 0x%x for location info", gstk_status, 0, 0);
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    gstk_status = gstk_util_populate_uim_cmd_total_data_len(
                     GSTK_CALL_CONTROL_TAG,
                     (offset - uim_cmd_ptr->envelope.num_bytes),
                     uim_cmd_ptr);

    if (gstk_status != GSTK_SUCCESS) {
      ERR ("status 0x%x for total data len", gstk_status, 0, 0);
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    MSG_MED("Env: CC 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
    for(i = 0; i< uim_cmd_ptr->envelope.num_bytes; i++) {
      MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
    }

    gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
  }
  else
  {
    /* Pack various TLVs */
    STK_envelope_cmd.tag = GSTK_CALL_CONTROL_TAG;

    /* device ID */
    gstk_status = gstk_packer_dev_id_tlv(
        GSTK_ME_DEVICE,
        GSTK_UICC_SIM_DEVICE,
        &STK_envelope_cmd.device_id );

    if (gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("In CC: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return gstk_status;
    }

    /* mandatory address/ss/ussd tag */
    switch(cc_cmd->cc_data.call_enum_type) {
    case GSTK_VOICE:
      gstk_status = gstk_packer_address_tlv(
        GSTK_ADDRESS_TAG,
        &cc_cmd->cc_data.gstk_address_string.voice.address,
        &STK_envelope_cmd.gstk_address_string_tlv.address);
      if (gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: Address Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
      }
      STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.address.length + 2;
      /* optional cap */
      if(cc_cmd->cc_data.gstk_address_string.voice.ccp1.length > 0) {
        gstk_status = gstk_packer_ccp_tlv(
          &cc_cmd->cc_data.gstk_address_string.voice.ccp1,
          &STK_envelope_cmd.ccp1);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: CCP1 Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.ccp1.length + 2;
      }
      /* optional subaddress */
      if(cc_cmd->cc_data.gstk_address_string.voice.subaddress.length > 0) {
        gstk_status = gstk_packer_subaddress_tlv(
          &cc_cmd->cc_data.gstk_address_string.voice.subaddress,
          &STK_envelope_cmd.subaddress);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: SubAddr Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.subaddress.length + 2;
      }
      /* optional cap2 */
      if(cc_cmd->cc_data.gstk_address_string.voice.ccp2.length > 0) {
        gstk_status = gstk_packer_ccp_tlv(
          &cc_cmd->cc_data.gstk_address_string.voice.ccp2,
          &STK_envelope_cmd.ccp2);
        if(gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: CCP2 Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
        }
        STK_envelope_cmd.length = STK_envelope_cmd.length + STK_envelope_cmd.ccp2.length + 2;
      }
      break;
    case GSTK_SS:
      gstk_status = gstk_packer_address_tlv(
        GSTK_SS_STRING_TAG,
        &cc_cmd->cc_data.gstk_address_string.ss_string,
        &STK_envelope_cmd.gstk_address_string_tlv.ss_string);
      if (gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: SS Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
      }
      STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.ss_string.length + 2;
      break;
    case GSTK_USSD:
      STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text =
              gstk_malloc(cc_cmd->cc_data.gstk_address_string.ussd_string.length);
      if(STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text == NULL)
      {
        MSG_ERROR("malloc failure", 0, 0, 0);
        gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
        return GSTK_MEMORY_ERROR;
      }
      gstk_status = gstk_packer_text_string_tlv(
        GSTK_USSD_STRING_TAG,
        cc_cmd->cc_data.gstk_address_string.ussd_string.dcs,
        cc_cmd->cc_data.gstk_address_string.ussd_string.length,
        cc_cmd->cc_data.gstk_address_string.ussd_string.text,
        &STK_envelope_cmd.gstk_address_string_tlv.ussd_string);
      if (gstk_status != GSTK_SUCCESS) {
          MSG_ERROR("In CC: USSD Packing Fail 0x%x", gstk_status, 0, 0);
          gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
          return gstk_status;
      }
      STK_envelope_cmd.length = STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length + 2;
      break;
    default:
      MSG_ERROR("In CC: Unknown CC type: 0x%x", cc_cmd->cc_data.call_enum_type, 0, 0);
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return GSTK_ERROR;
    }

    gstk_status = gstk_packer_location_info_tlv(
          &location_info, /* comprehension flag = 0! */
          &STK_envelope_cmd.location_info);

    if (gstk_status != GSTK_SUCCESS) {
      MSG_ERROR("In CC: Location Info Packing Fail 0x%x", gstk_status, 0, 0);
      gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
      return gstk_status;
    }

    /* Send command to STK application on SIM */
    /* Send Message to UIM */
    /* get a buffer for the request */
    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr != NULL ) {
      gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
      if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
#ifdef FEATURE_GSTK_DUAL_SLOTS
        if (cc_cmd->slot_info_present) {
          uim_cmd_ptr->hdr.slot = cc_cmd->slot;
        }
#endif /*FEATURE_GSTK_DUAL_SLOTS */

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = cc_cmd->message_header.user_data;

        /* initialized to 3 for value portion of the TLVs
        i.e., the final offset will be either 0 or 1 depends
        on whether the total length is > 0x7F or not */
        offset = 3;

        /* copy device */
        /* STK_envelope_cmd.device_id.device_tag_length fixed in packer function
           no chance of buffer overflow */
        memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.device_id,
                (int)(STK_envelope_cmd.device_id.device_tag_length
                      + GSTK_TAG_LENGTH_LEN));
        offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);
        /* total length: + device tlv */
        STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                  GSTK_TAG_LENGTH_LEN;

        /* copy address/ss/ussd */
        switch(cc_cmd->cc_data.call_enum_type) {
        case GSTK_VOICE:
          addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.address.length +
                            GSTK_TAG_LENGTH_LEN);
          if (gstk_check_envelope_offset(uim_cmd_ptr, offset, addr_length) != GSTK_SUCCESS){
            /* Error Out */
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                 &STK_envelope_cmd.gstk_address_string_tlv.address,
                 addr_length);
          offset = offset + uint32touint8(addr_length);
          /* total length: + address tlv */
          STK_envelope_cmd.length += uint32touint8(addr_length);

          if(cc_cmd->cc_data.gstk_address_string.voice.ccp1.length > 0) {
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp1.tag;
            if (STK_envelope_cmd.ccp1.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
              uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
              STK_envelope_cmd.length++;
            }
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp1.length;

            if (gstk_check_envelope_offset(uim_cmd_ptr, offset, STK_envelope_cmd.ccp1.length) !=
                                                                     GSTK_SUCCESS) {
              /* Error Out */
              return GSTK_MEMORY_ERROR;
            }
            memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                  STK_envelope_cmd.ccp1.ccp,
                  int32touint32(STK_envelope_cmd.ccp1.length));
            offset = offset + STK_envelope_cmd.ccp1.length;
            STK_envelope_cmd.length += STK_envelope_cmd.ccp1.length +
                                          GSTK_TAG_LENGTH_LEN;

          }
          if(cc_cmd->cc_data.gstk_address_string.voice.subaddress.length > 0) {
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.subaddress.tag;
            if (STK_envelope_cmd.subaddress.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
              uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
              STK_envelope_cmd.length++;
            }
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.subaddress.length;
            if (gstk_check_envelope_offset(uim_cmd_ptr, offset,
                    STK_envelope_cmd.subaddress.length) != GSTK_SUCCESS) {
                /* Error Out */
                return GSTK_MEMORY_ERROR;
            }
            memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                  STK_envelope_cmd.subaddress.subaddress,
                  int32touint32(STK_envelope_cmd.subaddress.length));
            offset = offset + STK_envelope_cmd.subaddress.length;
            STK_envelope_cmd.length += STK_envelope_cmd.subaddress.length +
                                        GSTK_TAG_LENGTH_LEN;
          }
          break;
        case GSTK_SS:
          addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.ss_string.length
                                  + GSTK_TAG_LENGTH_LEN);
          if (gstk_check_envelope_offset(uim_cmd_ptr, offset, addr_length) != GSTK_SUCCESS) {
            /* Error Out */
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.gstk_address_string_tlv.ss_string,
                addr_length);
          offset = offset + uint32toint32(addr_length);
          /* total length: + ss_string tlv */
          STK_envelope_cmd.length += uint32touint8(addr_length);
          break;
        case GSTK_USSD:
          /* tag */
          uim_cmd_ptr->envelope.data[offset++] =
                STK_envelope_cmd.gstk_address_string_tlv.ussd_string.tag;
          /* length */
          if (STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length >
                  GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
              uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
              STK_envelope_cmd.length++;
          }
          uim_cmd_ptr->envelope.data[offset++] =
              STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length;

          /* dcs */
          uim_cmd_ptr->envelope.data[offset++] =
              STK_envelope_cmd.gstk_address_string_tlv.ussd_string.data_coding_scheme;

          /* text */
          /* -1 to account for the TON byte */
          addr_length = int32touint32(STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length - 1);
          if (gstk_check_envelope_offset(uim_cmd_ptr, offset, addr_length) != GSTK_SUCCESS) {
            /* Error Out */
            gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                STK_envelope_cmd.gstk_address_string_tlv.ussd_string.text,
                addr_length);
          offset = offset + uint32toint32(addr_length);
          STK_envelope_cmd.length += STK_envelope_cmd.gstk_address_string_tlv.ussd_string.length +
                                      GSTK_TAG_LENGTH_LEN;
          break;
        default:
          break;
        }

        /* copy location info */
        /* tag */
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.tag;
        /* length */
        if (STK_envelope_cmd.location_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
          uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
          STK_envelope_cmd.length++;
        }
        uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.length;
        /* location info */
        /* STK_envelope_cmd.location_info.length fixed in packer function no
           chance for buffer overflow */
        memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                &STK_envelope_cmd.location_info.location_info,
                (int)STK_envelope_cmd.location_info.length );
        offset = (int)(offset + STK_envelope_cmd.location_info.length);
        STK_envelope_cmd.length += STK_envelope_cmd.location_info.length + GSTK_TAG_LENGTH_LEN;

        /* copy optional ccp2 */
        if(cc_cmd->cc_data.call_enum_type == GSTK_VOICE) {
          if(cc_cmd->cc_data.gstk_address_string.voice.ccp2.length > 0) {
            /* tag */
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp2.tag;
            /* length */
            if (STK_envelope_cmd.ccp2.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
              uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
              STK_envelope_cmd.length++;
            }
            uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.ccp2.length;
            /* ccp2 */
            if (gstk_check_envelope_offset(uim_cmd_ptr, offset,
                                STK_envelope_cmd.ccp2.length) != GSTK_SUCCESS) {
                /* Error Out */
                return GSTK_MEMORY_ERROR;
            }
            memcpy(&(uim_cmd_ptr->envelope.data[offset]),
                  STK_envelope_cmd.ccp2.ccp,
                  int32touint32(STK_envelope_cmd.ccp2.length));
            offset = offset + STK_envelope_cmd.ccp2.length;
            STK_envelope_cmd.length += STK_envelope_cmd.ccp2.length +
                                      GSTK_TAG_LENGTH_LEN;
          }
        }

        /* populate the envelope command info */
        /* total data length:  device id data + 2 (2 is for the tag and length field)
                      + address/ss/ussd data + 2
                      + location info data + 2
                      + ccp1/ccp2/subaddress
                      address/ss/ussd/ccp1/ccp2/subaddress length has been added ablove
        */
        if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
          /* account for 0x80 */
          uim_cmd_ptr->envelope.offset = 0;
          uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
          uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
          uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
          STK_envelope_cmd.length++;
          uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
        }
        else {
          uim_cmd_ptr->envelope.offset = 1;
          uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
          uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
          uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
        }

        MSG_MED("Env: CC 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
        for(i = 0; i< uim_cmd_ptr->envelope.num_bytes; i++) {
          MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
        }

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
      }
    } /* uim_cmd_ptr != NULL */
    else {
      ERR ("no UIM cmd buf", 0, 0, 0);
      gstk_status = GSTK_MEMORY_ERROR;
    }
  }
  gstk_cc_ind_cleanup(cc_cmd, &STK_envelope_cmd);

  return gstk_status;

} /* gstk_cc_ind */



/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_ind

DESCRIPTION

  This function builds the MO SMS control command and sends it to UIM

PARAMETERS
  mo_sms_ctrl_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_mo_sms_ctrl_ind(
          gstk_envelope_mo_sms_ctrl_ind_type * mo_sms_ctrl_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_mo_sms_ctrl_command_type      STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   int                                         i  = 0;
   gstk_location_info_rsp_type                 location_info;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)*/

   if(mo_sms_ctrl_cmd == NULL)
   {
     MSG_ERROR("mo_sms_ctrl_cmd ERR : NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_mo_sms_ctrl_command_type));

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)*/

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_MO_SHORT_MESSAGE_CONTROL_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In MO SMS Ctrl: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
       return gstk_status;
   }


   /* mandatory rp/tp address */

   gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &mo_sms_ctrl_cmd->mo_sms_ctrl_data.rp_dest_address,
       &STK_envelope_cmd.address1);
   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In MO SMS Ctrl: RP Addr Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
       return gstk_status;
   }

   gstk_status = gstk_packer_address_tlv(
       GSTK_ADDRESS_TAG,
       &mo_sms_ctrl_cmd->mo_sms_ctrl_data.tp_dest_address,
       &STK_envelope_cmd.address2);
   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In MO SMS Ctrl: TP Addr Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
       return gstk_status;
   }

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

   /* initialize location_info */
   memset(&location_info, 0, sizeof(gstk_location_info_rsp_type));

#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /*#if defined(FEATURE_GSM) || define(FEATURE_WCDMA)*/
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

   gstk_status = gstk_packer_location_info_tlv(
         &location_info, /* comprehension flag = 0! */
         &STK_envelope_cmd.location_info);

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In MO SMS Ctrl: Location Status Packing Fail 0x%x", gstk_status, 0, 0);
       gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
       return gstk_status;
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + address1 + 2
                        + address2 + 2
                        + location info data + 2

   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.address1.length +
                             STK_envelope_cmd.address2.length +
                             STK_envelope_cmd.location_info.length +
                             8;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = mo_sms_ctrl_cmd->message_header.user_data;

       /* initialized to 3 for value portion of the TLVs
          i.e., the final offset will be either 0 or 1 depends
          on whether the total length is > 0x7F or not */
       offset = 3;

       /* copy device */
       /*STK_envelope_cmd.device_id.device_tag_length is fixed in packer function,
       thus overflow not possible */
       gstk_byte_offset_memcpy(
         uim_cmd_ptr->envelope.data, 
         &STK_envelope_cmd.device_id,
         offset,
         (int)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN),  
         sizeof(uim_cmd_ptr->envelope.data),
         sizeof(STK_envelope_cmd.device_id));
       offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length + 
                GSTK_TAG_LENGTH_LEN);
       STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length + 
                                 GSTK_TAG_LENGTH_LEN;

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }

       /* copy address1 */
       gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data, 
                               &STK_envelope_cmd.address1,
                               offset,
                               GSTK_TAG_LENGTH_LEN,  
                               sizeof(uim_cmd_ptr->envelope.data),
                               sizeof(STK_envelope_cmd.address1));
       STK_envelope_cmd.length += GSTK_TAG_LENGTH_LEN;
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN );

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }
 
       /* include length for one extra byte after considering the TON byte. */
       if (STK_envelope_cmd.address1.length - 1 > 0) {
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.address1.ton;
         if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
         {
           MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
           return GSTK_ERROR;
         }
         gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data, 
                                 STK_envelope_cmd.address1.address,
                                 offset,
                                 (int)(STK_envelope_cmd.address1.length-1 ),  
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.address1.address));
         offset = (int)(offset + STK_envelope_cmd.address1.length - 1);
         STK_envelope_cmd.length += STK_envelope_cmd.address1.length;
       }

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }

       /* copy address2 */
       gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data, 
                               &STK_envelope_cmd.address2,
                               offset,
                               GSTK_TAG_LENGTH_LEN,  
                               sizeof(uim_cmd_ptr->envelope.data),
                               sizeof(STK_envelope_cmd.address2));
       STK_envelope_cmd.length += GSTK_TAG_LENGTH_LEN;
       offset = (int)(offset + GSTK_TAG_LENGTH_LEN );

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }

       if (STK_envelope_cmd.address2.length > 0) {
         if (int32touint32(offset) < sizeof(uim_cmd_ptr->envelope.data))
         {
           uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.address2.ton;
         }
         if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
         {
           MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
           gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
           uim_cmd_ptr = NULL;
           gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
           return GSTK_ERROR;
         }
         gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data, 
                                 STK_envelope_cmd.address2.address,
                                 offset,
                                 (int)(STK_envelope_cmd.address2.length-1),  
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.address2.address));
         offset = (int)(offset + STK_envelope_cmd.address2.length - 1);
         STK_envelope_cmd.length += STK_envelope_cmd.address2.length;
       }

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }
       /* copy location info */
       /* tag */
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.tag;

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }
       /* length */
       if (STK_envelope_cmd.location_info.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
         if (int32touint32(offset) < sizeof(uim_cmd_ptr->envelope.data))
         {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
       }

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.location_info.length;

       if(int32touint32(offset) >= sizeof(uim_cmd_ptr->envelope.data))
       {
         MSG_ERROR("out of bound offset 0x%x ", offset, 0, 0);
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
         gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);
         return GSTK_ERROR;
       }
       /* location info */
       /* STK_envelope_cmd.location_info.length is fixed value from packer function
          buffer overflow not possible */

       gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data, 
                               &STK_envelope_cmd.location_info.location_info,
                               offset,
                               (int)STK_envelope_cmd.location_info.length,  
                               sizeof(uim_cmd_ptr->envelope.data),
                               sizeof(STK_envelope_cmd.location_info.location_info));
       offset = (int)(offset + STK_envelope_cmd.location_info.length);
       STK_envelope_cmd.length += STK_envelope_cmd.location_info.length + GSTK_TAG_LENGTH_LEN;

       /* populate the envelope command info */
       if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
         /* account for 0x80 */
         uim_cmd_ptr->envelope.offset = 0;
         uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
         STK_envelope_cmd.length++;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
       }
       else {
         uim_cmd_ptr->envelope.offset = 1;
         uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
       }

       MSG_MED("Env: MO SMS control", 0, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
       }

       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_mo_sms_ctrl_ind_cleanup(mo_sms_ctrl_cmd);

   return gstk_status;

} /* gstk_mo_sms_ctrl_ind */

#ifdef FEATURE_CAT_REL6
/*===========================================================================
FUNCTION gstk_nw_search_mode_evt_ind

DESCRIPTION

  This function builds the network search mode command and sends it to UIM

PARAMETERS
  nw_search_mode_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_nw_search_mode_evt_ind(
  const gstk_envelope_nw_search_mode_evt_ind_type * nw_search_mode_cmd
)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   uim_cmd_type                                            *uim_cmd_ptr;
   int32                                                   offset = 0;
   int                                                     i  = 0;
   gstk_evt_list_code_enum_type                            evt_list_info_data;
   gstk_nw_search_mode_enum_type                           nw_search_mode_data;
   gstk_device_id_type                                     dev_id;

   MSG_HIGH("In gstk_nw_search_mode_evt_ind",0,0,0);

   if(nw_search_mode_cmd == NULL)
   {
     MSG_ERROR("nw_search_mode_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   nw_search_mode_data =
     (gstk_nw_search_mode_enum_type)nw_search_mode_cmd->nw_search_mode;

   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr == NULL )
   {
     ERR ("no UIM cmd buf", 0, 0, 0);
     return GSTK_MEMORY_ERROR;
   }

   gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
   if ((gstk_status != GSTK_SUCCESS) || (uim_cmd_ptr == NULL))
   {
     MSG_HIGH("ERR status is 0x%x or uim_cmd_ptr is NULL", 0, 0, 0);
     return gstk_status;
   }
   /* set to 0 so that uim will get the data from index 0 */
   uim_cmd_ptr->envelope.offset = 0x00;
   /* set user_data */
   uim_cmd_ptr->hdr.user_data   = nw_search_mode_cmd->message_header.user_data;

   /* populate the envelope command info */
   uim_cmd_ptr->envelope.num_bytes = GSTK_EVT_LIST_VAL_LEN + /* set up event list */
                                     GSTK_DEVICE_IDENTITY_LEN +
                                     GSTK_NW_SEARCH_MODE_LEN +
                                     (GSTK_TAG_LENGTH_LEN * 3) +
                                     GSTK_TAG_LENGTH_LEN;

   uim_cmd_ptr->envelope.data[offset++] = GSTK_EVENT_DOWNLOAD_TAG;

   /* total data length:  evt id data + evt list tag len+ evt list len len
                          + device id data + dev id tag len + dev id len len
                          + nw search mode data + nw search mode tag len
                          + nw search mode len len
   */
   uim_cmd_ptr->envelope.data[offset++] = GSTK_EVT_LIST_VAL_LEN + /* set up event list */
                                          GSTK_DEVICE_IDENTITY_LEN +
                                          GSTK_NW_SEARCH_MODE_LEN +
                                          (GSTK_TAG_LENGTH_LEN * 3);

   evt_list_info_data = GSTK_NW_SEARCH_MODE;

   gstk_status = gstk_util_populate_tlv(
                       GSTK_EVENT_LIST_TAG,
                       offset,
                       (gstk_cmd_to_uim_type*)&evt_list_info_data,
                       uim_cmd_ptr->envelope.data,
                       &offset); /*lint !e826 evt_list_info_data suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     ERR ("status 0x%x for Event List tag", gstk_status, 0, 0);
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }
   /* copy device */
   dev_id.src = GSTK_ME_DEVICE;
   dev_id.dest = GSTK_UICC_SIM_DEVICE;
   gstk_status = gstk_util_populate_tlv(
                   GSTK_DEVICE_IDENTITY_TAG,
                   offset,
                   (gstk_cmd_to_uim_type*)&dev_id,
                   uim_cmd_ptr->envelope.data,
                   &offset); /*lint !e826 dev_id suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     ERR("status 0x%x for Device", gstk_status, 0, 0);
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }

   /* network search mode tag */
   gstk_status = gstk_util_populate_tlv(
                   GSTK_NW_SEARCH_MODE_TAG,
                   offset,
                   (gstk_cmd_to_uim_type*)&nw_search_mode_data,
                   uim_cmd_ptr->envelope.data,
                   &offset); /*lint !e826 nw_search_mode_data suppress area too small */

   if (gstk_status != GSTK_SUCCESS)
   {
     ERR ("status 0x%x for Network search mode", gstk_status, 0, 0);
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     uim_cmd_ptr = NULL;
     return GSTK_ERROR;
   }

   MSG_MED("Env: Network Search Mode evt 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
   for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++)
   {
     MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i],
               uim_cmd_ptr->envelope.data[i]);
   }

   MSG_HIGH("Sending Network Search Mode envelope to UIM",0,0,0);
   gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
   return gstk_status;
}
#endif /* #ifdef FEATURE_CAT_REL6 */


/*===========================================================================
FUNCTION gstk_timer_expire_ind

DESCRIPTION

  This function builds the timer expire command and sends it to UIM

PARAMETERS
  timer_expire_cmd: [Input] Pointer to the envelope command to be processed

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
static gstk_status_enum_type gstk_timer_expire_ind (
  const gstk_envelope_timer_expire_ind_type * timer_expire_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_timer_expiration_command_type STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;
   int                                         i  = 0;

   if(timer_expire_cmd == NULL)
   {
     MSG_ERROR("timer_expire_cmd ERR : NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_timer_expiration_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_TIMER_EXPIRY_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Timer Expire: Device ID Packing Fail 0x%x", gstk_status, 0, 0);
       return gstk_status;
   }

   /* mandatory timer identifier */
   gstk_status = gstk_packer_timer_id_tlv(
       &timer_expire_cmd->timer_info.timer_id,
       &STK_envelope_cmd.timer_id);
   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Timer Expire: Timer ID Packing Fail 0x%x", gstk_status, 0, 0);
       return gstk_status;
   }

   /* mandatory timer value */
   gstk_status = gstk_packer_timer_value_tlv(
       &timer_expire_cmd->timer_info.time_value,
       &STK_envelope_cmd.timer_value);
   if (gstk_status != GSTK_SUCCESS) {
       MSG_ERROR("In Timer Expire: Timer Value Packing Fail 0x%x", gstk_status, 0, 0);
       return gstk_status;
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + timer_id + 2
                        + timer_value + 2

   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.timer_id.length +
                             STK_envelope_cmd.timer_value.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = timer_expire_cmd->message_header.user_data;

       /* required so that uim will grap the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy device */
       /* STK_envelope_cmd.device_id.device_tag_length fixed value from packer function
          overflow not possible */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
              &STK_envelope_cmd.device_id,
              (int)(STK_envelope_cmd.device_id.device_tag_length
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length
                      + GSTK_TAG_LENGTH_LEN);

       /* copy timer id */
       /* STK_envelope_cmd.timer_id.length fixed value from packer function,
          buffer overflow not possible */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &(STK_envelope_cmd.timer_id),
            (int)(STK_envelope_cmd.timer_id.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.timer_id.length +
                      GSTK_TAG_LENGTH_LEN);

       /* copy timer value */
       /* STK_envelope_cmd.timer_value.length fixed value from packer function,
          buffer overflow not possible */
       memcpy(&(uim_cmd_ptr->envelope.data[offset]),
            &(STK_envelope_cmd.timer_value),
            (int)(STK_envelope_cmd.timer_value.length + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.timer_value.length +
                      GSTK_TAG_LENGTH_LEN);

       MSG_MED("Env: Timer Expire: 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
       for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) {
         MSG_MED("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], uim_cmd_ptr->envelope.data[i]);
       }

       gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     }
   } /* uim_cmd_ptr != NULL */
   else {
     ERR ("no UIM cmd buf", 0, 0, 0);
     gstk_status = GSTK_MEMORY_ERROR;
   }

   /* no need to free envelope timer expire ind and tlv data */
   return gstk_status;

} /* gstk_timer_expire_ind */


/*===========================================================================
FUNCTION gstk_requeue_envelope_command

DESCRIPTION

  This function requeues the envelope command to the back of the gstk queue

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_requeue_envelope_command(
  const gstk_cmd_type *cmd_ptr)
{
  gstk_status_enum_type     gstk_status       = GSTK_SUCCESS;
  uint32                    prev_env_ref_id   = 0;
  uint32                    user_data         = 0;
  gstk_evt_cb_funct_type    gstk_sms_pp_dl_cb = NULL;
  uint32                    type_of_command   = 0;

  MSG_HIGH(" In gstk_requeue_envelope_command() ", 0, 0, 0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd NULL ptr",0,0,0);
    return GSTK_BAD_PARAM;
  }
  /* Get the type of command */
  type_of_command = cmd_ptr->general_cmd.message_header.command_id;
  prev_env_ref_id = cmd_ptr->sms_pp_envelope_ind.message_header.user_data;

  switch(type_of_command)
  {
  case GSTK_SMS_PP_DOWNLOAD_IND:
    MSG_HIGH(" Rec'd PP DL:GSTK_SMS_PP_DOWNLOAD_IND", 0, 0, 0);
    gstk_status = gstk_util_retrieve_env_cmd_info(GSTK_SMS_PP_DOWNLOAD_IND_RSP,&user_data,
                                                  &gstk_sms_pp_dl_cb,prev_env_ref_id);
    if(gstk_status == GSTK_SUCCESS)
    {
      gstk_status = gstk_send_envelope_sms_pp_dl_command(user_data,
                                                         cmd_ptr->sms_pp_envelope_ind.is_cdma_sms_pp,
                                                         &(cmd_ptr->sms_pp_envelope_ind.address),
                                                         &(cmd_ptr->sms_pp_envelope_ind.tpdu),
                                                         gstk_sms_pp_dl_cb);
    }
    break;
  default:
    break;
  }
  return gstk_status;
}


/*===========================================================================
FUNCTION gstk_process_envelope_cmd

DESCRIPTION

  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope command tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_process_envelope_cmd(
    gstk_cmd_type* cmd )
{
    gstk_status_enum_type    gstk_status            = GSTK_SUCCESS;
    uint32                   type_of_command        = 0;
    boolean                  error_env_flg          = FALSE;
    boolean                  sms_error_env_flg      = FALSE;

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR : NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    /* Get the type of command */
    type_of_command = cmd->general_cmd.message_header.command_id;

    /* Check the proactive command table for proactive command still
    ** being provessed by card
    ** If TRUE, do not send down any envelopes to the card. Inform the client
    ** that the envelope could not be processed
    */

#ifdef FEATURE_STOP_ENV_CMD_PROCESSING
    error_env_flg = gstk_check_for_pending_pro_cmd();
#endif /* FEATURE_STOP_ENV_CMD_PROCESSING */
#ifdef FEATURE_STOP_SMS_PP_ON_PENDING_SMS_TR
    sms_error_env_flg = gstk_check_for_pending_send_sms_term_resp();
#endif /* FEATURE_STOP_ENV_CMD_PROCESSING */

    switch( type_of_command ) {
    case GSTK_MENU_SELECTION_IND:
      MSG_HIGH(" Rec'd Menu Selection :GSTK_MENU_SELECTION_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_menu_selection_ind(&(cmd->menu_selection_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->menu_selection_envelope_ind.message_header.user_data,
                                                       cmd->menu_selection_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    case GSTK_SMS_PP_DOWNLOAD_IND:
      MSG_HIGH(" Rec'd PP DL:GSTK_SMS_PP_DOWNLOAD_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE && sms_error_env_flg ==  FALSE)
      {
        gstk_status = gstk_sms_pp_dl_ind(&(cmd->sms_pp_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS) || 
         (sms_error_env_flg == TRUE))
      {
        gstk_status = gstk_requeue_envelope_command(cmd);
        if(gstk_status != GSTK_SUCCESS)
        {
          MSG_HIGH("Requeue of sms pp envelope command failed",0,0,0);
          gstk_status = gstk_build_error_envelope_report(cmd->sms_pp_envelope_ind.message_header.user_data,
                                                         cmd->sms_pp_envelope_ind.message_header.command_id,
                                                         GSTK_ENVELOPE_CMD_CARD_BUSY);
        }
        else
        {
          /* Free the reference id in the envelope table for the old command */
          gstk_util_env_cmd_ref_table_free_slot(cmd->sms_pp_envelope_ind.message_header.user_data);
          /* Free the upper layer command */
          gstk_sms_pp_dl_ind_cleanup(&(cmd->sms_pp_envelope_ind));
        }
      }
      break;
    case GSTK_MT_CALL_EVT_IND:
      MSG_HIGH(" Rec'd ME Call Evt: GSTK_MT_CALL_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_mt_call_evt_ind(&(cmd->mt_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->mt_evt_envelope_ind.message_header.user_data,
                                                       cmd->mt_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CALL_CONNECTED_EVT_IND:
      MSG_HIGH(" Rec'd Call connected Evt: GSTK_CALL_CONNECTED_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_call_connected_evt_ind(&(cmd->call_connected_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->call_connected_envelope_ind.message_header.user_data,
                                                       cmd->call_connected_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CALL_DISCONNECTED_EVT_IND:
      MSG_HIGH(" Rec'd Call disconnected Evt :GSTK_CALL_DISCONNECTED_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_call_disconnected_evt_ind(&(cmd->call_disconnected_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->call_disconnected_envelope_ind.message_header.user_data,
                                                       cmd->call_disconnected_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_IDLE_SCRN_EVT_IND:
      MSG_HIGH(" Rec'd Idle Scrn Evt: GSTK_IDLE_SCRN_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_idle_scrn_evt_ind(&(cmd->idle_scrn_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->idle_scrn_evt_envelope_ind.message_header.user_data,
                                                       cmd->idle_scrn_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_LOCATION_STATUS_EVT_IND:
      MSG_HIGH(" Rec'd Location Statuc Evt:GSTK_LOCATION_STATUS_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_location_status_evt_ind(&(cmd->location_st_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->location_st_envelope_ind.message_header.user_data,
                                                       cmd->location_st_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_ACCESS_TECH_EVT_IND:
      MSG_HIGH(" Rec'd Access Technology Evt:GSTK_ACCESS_TECH_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
         gstk_status = gstk_access_tech_evt_ind(&(cmd->access_tech_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        MSG_HIGH("Building Access Tech error envelope report",0,0,0);
        gstk_status = gstk_build_error_envelope_report(cmd->access_tech_evt_envelope_ind.message_header.user_data,
                                                       cmd->access_tech_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_USER_ACTIVITY_EVT_IND:
      MSG_HIGH(" Rec'd User Activity Evt: GSTK_USER_ACTIVITY_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_user_act_evt_ind(&(cmd->user_act_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->user_act_evt_envelope_ind.message_header.user_data,
                                                       cmd->user_act_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_LANG_SELECTION_EVT_IND:
      MSG_HIGH(" Rec'd Lang Sel Evt: GSTK_LANG_SELECTION_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_lang_selection_evt_ind(&(cmd->lang_sel_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->lang_sel_evt_envelope_ind.message_header.user_data,
                                                       cmd->lang_sel_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_BROWSER_TERMINATION_EVT_IND:
      MSG_HIGH(" Rec'd Browser Term Evt: GSTK_BROWSER_TERMINATION_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_browser_term_evt_ind(&(cmd->browser_term_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->browser_term_evt_envelope_ind.message_header.user_data,
                                                       cmd->browser_term_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_DATA_AVAIL_EVT_IND:
      MSG_HIGH(" Rec'd DATA AVAIL Evt: GSTK_DATA_AVAIL_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_data_avail_evt_ind(&(cmd->data_avail_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->data_avail_evt_envelope_ind.message_header.user_data,
                                                       cmd->data_avail_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CH_STATUS_EVT_IND:
      MSG_HIGH(" Rec'd CHANNEL STATUS Evt: GSTK_CH_STATUS_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_ch_status_evt_ind(&(cmd->ch_status_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->ch_status_evt_envelope_ind.message_header.user_data,
                                                       cmd->ch_status_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_MO_SMS_CTRL_IND:
      MSG_HIGH(" Rec'd MO SMS Ctrl: GSTK_MO_SMS_CTRL_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_mo_sms_ctrl_ind(&(cmd->mo_sms_ctrl_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->mo_sms_ctrl_envelope_ind.message_header.user_data,
                                                       cmd->mo_sms_ctrl_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CC_IND:
      MSG_HIGH(" Rec'd Call ctrl: GSTK_CC_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_cc_ind(&(cmd->cc_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->cc_envelope_ind.message_header.user_data,
                                                       cmd->cc_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_TIMER_EXPIRE_IND:
      MSG_HIGH(" Rec'd Timer expire: GSTK_TIMER_EXPIRE_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_timer_expire_ind(&(cmd->timer_expire_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->timer_expire_envelope_ind.message_header.user_data,
                                                       cmd->timer_expire_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      MSG_HIGH(" Rec'd Cell Broadcast DL: GSTK_CELL_BROADCAST_DOWNLOAD_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_cell_broadcast_dl_ind(&(cmd->cell_broadcast_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->cell_broadcast_envelope_ind.message_header.user_data,
                                                       cmd->cell_broadcast_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
      break;
    case GSTK_NW_SEARCH_MODE_EVT_IND:
#ifdef FEATURE_CAT_REL6
      MSG_HIGH(" Rec'd NW Search Mode evt ind: GSTK_NW_SEARCH_MODE_EVT_IND", 0, 0, 0);
      /*lint -e774 error_env_flg always evaluates to FALSE */
      if(error_env_flg == FALSE)
      {
        gstk_status = gstk_nw_search_mode_evt_ind(&(cmd->nw_search_mode_evt_envelope_ind));
      }
      if((error_env_flg == TRUE) || (gstk_status != GSTK_SUCCESS))
      {
        gstk_status = gstk_build_error_envelope_report(cmd->nw_search_mode_evt_envelope_ind.message_header.user_data,
                                                       cmd->nw_search_mode_evt_envelope_ind.message_header.command_id,
                                                       GSTK_ENVELOPE_CMD_CARD_BUSY);
      }
#else
      MSG_HIGH("GSTK_NW_SEARCH_MODE_EVT_IND : FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      MSG_ERROR(" Rec'd unknown Envelope", 0, 0, 0);
      break;
    }
    return gstk_status;
} /* gstk_process_envelope_cmd*/


/*===========================================================================
FUNCTION   gstk_build_postpone_envelope

DESCRIPTION
  This function builds the envelope that has been postponed due to
    terminal response that is required to be sent down first

PARAMETERS
  env_cmd_index: [Input] Index into the envelope command reference table

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_build_postpone_envelope ( uint8  env_cmd_index )
{
  gstk_task_cmd_type              *task_cmd;

  MSG_HIGH("gstk_build_postpone_envelope index = 0x%x", env_cmd_index, 0, 0);
  if (env_cmd_index >= GSTK_MAX_PROACTIVE_COMMAND) {
    MSG_ERROR("Err: Env index too big", 0, 0, 0);
    return GSTK_ERROR;
  }

  if (envelope_cmd_ref_table[env_cmd_index].expected_cmd_rsp_type != GSTK_CALL_CONNECTED_EVT_IND_RSP) {
    MSG_ERROR("Err: Env not Call connected 0x%x",
               envelope_cmd_ref_table[env_cmd_index].expected_cmd_rsp_type, 0, 0);
    return GSTK_ERROR;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd != NULL ) { /* successfully obtained command buffer */
    /* build the call related envelope command download */

    task_cmd->cmd.call_connected_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.call_connected_envelope_ind.message_header.command_id = (int)GSTK_CALL_CONNECTED_EVT_IND;
    /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
    task_cmd->cmd.call_connected_envelope_ind.message_header.user_data = env_cmd_index;

    /* transaction id */
    task_cmd->cmd.call_connected_envelope_ind.transaction_id.length = 1;
    task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list =
      gstk_malloc(task_cmd->cmd.call_connected_envelope_ind.transaction_id.length);

    if (task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list == NULL) {
      MSG_ERROR("Malloc transaction id list failed", 0, 0, 0);
      /* put the queue back to the free queue */
      (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
      q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
      gstk_util_env_cmd_ref_table_free_slot(env_cmd_index);
      return GSTK_ERROR;
    }

    memcpy(task_cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list,
           ((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[env_cmd_index].user_data)->transaction_id.transaction_list,
           task_cmd->cmd.call_connected_envelope_ind.transaction_id.length);

    /* mo or mt call */
    task_cmd->cmd.call_disconnected_envelope_ind.near_end =
      ((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[env_cmd_index].user_data)->near_end;

    gstk_task_put_cmd_buf(task_cmd);
    return GSTK_SUCCESS;
  }
  else {
    ERR("No GSTK Buf", 0, 0, 0);
    gstk_util_env_cmd_ref_table_free_slot(env_cmd_index);
    return GSTK_ERROR;
  }
} /* gstk_build_postpone_envelope */

/*===========================================================================
FUNCTION   gstk_build_access_technology_envelope

DESCRIPTION
  This function builds the envelope for access technology event.

PARAMETERS
  access tech :- Type of access technology used

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
gstk_status_enum_type gstk_build_access_technology_envelope(
    gstk_access_technology_type access_tech
)
{

  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd  = NULL;
  uint32                  env_ref_id  = 0;
  uint32                  user_data   = 0;

  MSG_HIGH("Access Technology event download", 0, 0, 0);

  if ((access_tech != GSTK_ACCESS_TECH_GSM) &&
      (access_tech != GSTK_ACCESS_TECH_UTRAN) &&
      (access_tech != GSTK_ACCESS_TECH_CDMA)
     )
  {
      ERR("Unknown Access Technology type", 0, 0, 0);
      return GSTK_ERROR;
  }

  /* set user_data as the access technology value*/
  user_data = (uint32) access_tech;

  /* get the envelope refernce id */
  gstk_status =  gstk_util_save_env_cmd_info(
                    GSTK_CLIENT_ID,
                    NULL,
                    GSTK_ACCESS_TECH_EVT_IND,
                    user_data,
                    &env_ref_id);

  if(gstk_status == GSTK_SUCCESS)
  {

    /* get command buf */
    task_cmd = gstk_task_get_cmd_buf();
    if ( task_cmd == NULL )
    {
      ERR("No more gstk buf", 0, 0, 0);
      /* free the env slot */
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      return GSTK_ERROR;
    }
    /* successfully obtained command buffer */
    /* build the envelope command */
    task_cmd->cmd.access_tech_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.access_tech_evt_envelope_ind.message_header.command_id = (int)GSTK_ACCESS_TECH_EVT_IND;
    /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
    task_cmd->cmd.access_tech_evt_envelope_ind.message_header.user_data = env_ref_id;

    /* timer info */
    task_cmd->cmd.access_tech_evt_envelope_ind.access_tech = (int)access_tech;

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    MSG_HIGH("Sending Access Tech envelope to UIM",0,0,0);
    gstk_task_put_cmd_buf(task_cmd);
    return GSTK_SUCCESS;
  }
  else
  {
    return GSTK_ERROR;
  }
}
#ifdef FEATURE_CAT_REL6
/*===========================================================================
FUNCTION   gstk_build_location_status_envelope

DESCRIPTION
  This function builds the envelope for location status event.

PARAMETERS
  access tech :- Type of access technology used
  location status :- GSTK_NORMAL_SERVICE
                     GSTK_NO_SERVICE
                     GSTK_LIMITED_SERVICE

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
gstk_status_enum_type gstk_build_location_status_envelope(
    gstk_access_technology_type    access_tech,
    gstk_location_status_enum_type location_status
)
{

  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd  = NULL;
  uint32                  env_ref_id  = 0;
  uint32                  user_data   = 0;

  MSG_HIGH("Location status event download", 0, 0, 0);

  if ((access_tech != GSTK_ACCESS_TECH_GSM) &&
      (access_tech != GSTK_ACCESS_TECH_UTRAN) &&
      (access_tech != GSTK_ACCESS_TECH_CDMA)
     )
  {
      ERR("Unknown Access Technology type - Cannot build Location status envelope", 0, 0, 0);
      return GSTK_ERROR;
  }

  /* set user_data as the access technology value*/
  user_data = (uint32) access_tech;

  /* get the envelope refernce id */
  gstk_status =  gstk_util_save_env_cmd_info(
                    GSTK_CLIENT_ID,
                    NULL,
                    GSTK_LOCATION_STATUS_EVT_IND,
                    user_data,
                    &env_ref_id);

  if(gstk_status == GSTK_SUCCESS)
  {

    /* get command buf */
    task_cmd = gstk_task_get_cmd_buf();
    if ( task_cmd == NULL )
    {
      ERR("No more gstk buf", 0, 0, 0);
      /* free the env slot */
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      return GSTK_ERROR;
    }
    /* successfully obtained command buffer */
    /* build the envelope command */
    task_cmd->cmd.location_st_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    task_cmd->cmd.location_st_envelope_ind.message_header.command_id    = int32touint32((int)GSTK_LOCATION_STATUS_EVT_IND);
    /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
    task_cmd->cmd.location_st_envelope_ind.message_header.user_data     = env_ref_id;

    /* timer info */
    task_cmd->cmd.location_st_envelope_ind.location_status              = int32touint8((int)location_status);
    switch(access_tech)
    {
    case GSTK_ACCESS_TECH_GSM:
    case GSTK_ACCESS_TECH_UTRAN:
      task_cmd->cmd.location_st_envelope_ind.location_info.info_type    = GSTK_GSM_UMTS_LOCATION_INFO;
      break;
    case GSTK_ACCESS_TECH_CDMA:
      task_cmd->cmd.location_st_envelope_ind.location_info.info_type    = GSTK_CDMA_LOCATION_INFO;
      break;
    default:
      MSG_ERROR("Invalid Access technology type",0,0,0);
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      return GSTK_ERROR;
    }

    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    MSG_HIGH("Sending Location status envelope to UIM",0,0,0);
    gstk_task_put_cmd_buf(task_cmd);
    return GSTK_SUCCESS;
  }
  else
  {
    return GSTK_ERROR;
  }
}
/*===========================================================================
FUNCTION   gstk_build_nw_search_mode_change_envelope

DESCRIPTION
  This function builds the envelope for network search mode event.

PARAMETERS
  ph_nw_sel_pref :- Preferred network selection mode used.

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
gstk_status_enum_type gstk_build_nw_search_mode_change_envelope(
    gstk_nw_search_mode_enum_type  ph_nw_sel_pref
)
{

  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd  = NULL;
  uint32                  env_ref_id  = 0;
  uint32                  user_data   = 0;

  MSG_HIGH("Network Search Mode Change event download", 0, 0, 0);

  if ((ph_nw_sel_pref != GSTK_NW_SEARCH_MODE_AUTOMATIC) &&
      (ph_nw_sel_pref != GSTK_NW_SEARCH_MODE_MANUAL)
     )
  {
    ERR("Unknown Neywork Search Mode type", 0, 0, 0);
    return GSTK_ERROR;
  }

  /* No user data to return*/
  user_data = 0;

  /* get the envelope refernce id */
  gstk_status =  gstk_util_save_env_cmd_info(
                    GSTK_CLIENT_ID,
                    NULL,
                    GSTK_NW_SEARCH_MODE_EVT_IND,
                    user_data,
                    &env_ref_id);

  if(gstk_status != GSTK_SUCCESS)
  {
    return GSTK_ERROR;
  }

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd == NULL )
  {
    ERR("No more gstk buf", 0, 0, 0);
    /* free the env slot */
    gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
    return GSTK_ERROR;
  }

  /* successfully obtained command buffer */
  /* build the envelope command */
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.command_id = (int)GSTK_NW_SEARCH_MODE_EVT_IND;

  /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
  task_cmd->cmd.ph_nw_sel_pref_ind.message_header.user_data = env_ref_id;

  /* network search mode */
  task_cmd->cmd.ph_nw_sel_pref_ind.ph_nw_sel_pref = ph_nw_sel_pref;

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  MSG_HIGH("Sending Network Search Mode Change envelope to UIM",0,0,0);
  gstk_task_put_cmd_buf(task_cmd);
  return GSTK_SUCCESS;
}
#endif /* #ifdef FEATURE_CAT_REL6 */

/*===========================================================================
FUNCTION gstk_process_non_idle_state_envelope_cmd

DESCRIPTION
  This function switches on the command id and call the corresponding
  envelope command ind function so that GSTK can build the corrresponding
  envelope response to the caller

PARAMETERS
  cmd: [Input] Pointer to the envelope command types to be processed

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
gstk_status_enum_type gstk_process_non_idle_state_envelope_cmd(
  const gstk_cmd_type* cmd )
{
    gstk_status_enum_type    gstk_status        = GSTK_SUCCESS;
    uint32                   type_of_command;

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    /* Get the type of command */
    type_of_command = cmd->general_cmd.message_header.command_id;

    switch( type_of_command ) {
    case GSTK_MENU_SELECTION_IND:
      MSG_HIGH(" Rec'd Menu Selection :GSTK_MENU_SELECTION_IND", 0, 0, 0);
      gstk_status = gstk_non_idle_st_menu_selection_err_rsp(&(cmd->menu_selection_envelope_ind));
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND:
      MSG_HIGH(" Rec'd PP DL:GSTK_SMS_PP_DOWNLOAD_IND", 0, 0, 0);
      gstk_status = gstk_non_idle_st_sms_pp_err_rsp(&(cmd->sms_pp_envelope_ind));
      break;
    case GSTK_MT_CALL_EVT_IND:
    case GSTK_CALL_CONNECTED_EVT_IND:
    case GSTK_CALL_DISCONNECTED_EVT_IND:
    case GSTK_IDLE_SCRN_EVT_IND:
    case GSTK_LOCATION_STATUS_EVT_IND:
    case GSTK_USER_ACTIVITY_EVT_IND:
    case GSTK_LANG_SELECTION_EVT_IND:
    case GSTK_BROWSER_TERMINATION_EVT_IND:
    case GSTK_DATA_AVAIL_EVT_IND:
    case GSTK_CH_STATUS_EVT_IND:
    case GSTK_ACCESS_TECH_EVT_IND:
    case GSTK_NW_SEARCH_MODE_EVT_IND:
      MSG_HIGH(" Rec'd Event Download 0x%x", type_of_command, 0, 0);
      gstk_status = gstk_non_idle_st_evt_dl_err_rsp(cmd);
      break;
    case GSTK_MO_SMS_CTRL_IND:
      MSG_HIGH(" Rec'd MO SMS Ctrl: GSTK_MO_SMS_CTRL_IND", 0, 0, 0);
      gstk_status = gstk_non_idle_st_mo_sms_ctrl_err_rsp(&(cmd->mo_sms_ctrl_envelope_ind));
      break;
    case GSTK_CC_IND:
      MSG_HIGH(" Rec'd Call ctrl: GSTK_CC_IND", 0, 0, 0);
      gstk_status = gstk_non_idle_st_cc_err_rsp(&(cmd->cc_envelope_ind));
      break;
    case GSTK_TIMER_EXPIRE_IND:
      MSG_HIGH(" Rec'd Call ctrl: GSTK_TIMER_EXPIRE_IND", 0, 0, 0);
      gstk_status = gstk_non_idle_st_timer_expire_err_rsp(&(cmd->timer_expire_envelope_ind));
      break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    default:
      MSG_HIGH(" Rec'd unknown Envelope", 0, 0, 0);
      break;
    }
    return gstk_status;
} /* gstk_process_non_idle_state_envelope_cmd*/

/*===========================================================================
FUNCTION gstk_check_envelope_offset

DESCRIPTION
  This function checks the envelope offest to ensure there is no buffer overflow

PARAMETERS
  *cmd_ptr: pointer of uim_cmd_type type
  offset:   offset to be verified
  length:   length of data to be copied, value to be verified against offset

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  cmd_ptr will be freed if validation against offset fails

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_check_envelope_offset(uim_cmd_type *cmd_ptr, 
                                           int offset, uint32 length){
  if (offset < uint32toint32(GSTK_UIM_MAX_CHARS - length))
  {
    return GSTK_SUCCESS;
  }
  else{
    gstk_free(cmd_ptr);
    return GSTK_ERROR;
  }
}


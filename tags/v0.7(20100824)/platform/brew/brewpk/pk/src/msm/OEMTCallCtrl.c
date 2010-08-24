/*======================================================
FILE:  OEMTCallCtl.c

SERVICES:  Implementation of Call Control for OEMTelephone

GENERAL DESCRIPTION:
   Implements

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "err.h"

#include <string.h>

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#include "OEMTCallCtrl.h"
#include "uiutils.h"
#include "cm.h"
#include "OEMHeap.h"
#ifdef FEATURE_GSTK
#include "gstk_exp.h"
#include "gstki.h"
#endif

#ifdef FEATURE_UI_PBM
#include "pbmlib.h"
#ifdef FEATURE_UI_DUALPROC_APPS
#include "pbm_conversion.h"
#endif
#endif


#ifdef FEATURE_GSTK
static void OEMTCallCtrl_CC_Command_Cb(gstk_cmd_from_card_type *req_rsp);
static boolean OEMTCallCtrl_SendCmdToGSDI(gstk_slot_id_enum_type slot,
    gstk_call_enum_type cmd, byte *buf, int length,
    byte *subaddress,
    uint32 user_data,
    gstk_evt_cb_funct_type gstk_cc_cb );
#endif /*FEATURE_GSTK */

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_OTASP
#define STAR228 "*228"
#define STANDARD_OTASP_LENGTH 6
  /* Table specifying default hard-coded special numbers and their
  ** corresponding call type.
  **
  ** Note that for OTASP numbers the corresponding activation code
  ** is also specified.
  */
  static const struct {
      byte                     *num_str;      /* number string */
      cm_call_type_e_type      call_type;     /* associated call type */
      cm_activate_code_e_type  activate_code; /* OTASP activation code */
  } otasp_num_tbl[] = {

      /* number */   /* call type */               /* OTASP activate code */
#ifdef FEATURE_CDMA_800
      { (byte *) "*22800",    CM_CALL_TYPE_STD_OTASP,       CM_ABAND     },
      { (byte *) "*22801",    CM_CALL_TYPE_STD_OTASP,       CM_BBAND     },
#endif
#ifdef FEATURE_CDMA_1900
      { (byte *) "*22802",    CM_CALL_TYPE_STD_OTASP,       CM_ABLOCK    },
      { (byte *) "*22803",    CM_CALL_TYPE_STD_OTASP,       CM_BBLOCK    },
      { (byte *) "*22804",    CM_CALL_TYPE_STD_OTASP,       CM_CBLOCK    },
      { (byte *) "*22805",    CM_CALL_TYPE_STD_OTASP,       CM_DBLOCK    },
      { (byte *) "*22806",    CM_CALL_TYPE_STD_OTASP,       CM_EBLOCK    },
      { (byte *) "*22807",    CM_CALL_TYPE_STD_OTASP,       CM_FBLOCK    },
#endif
#ifdef FEATURE_UI_OTASP_STAR2281X
      { (byte *) "*22812",    CM_CALL_TYPE_STD_OTASP,       CM_ABLOCK    },
      { (byte *) "*22813",    CM_CALL_TYPE_STD_OTASP,       CM_BBLOCK    },
      { (byte *) "*22814",    CM_CALL_TYPE_STD_OTASP,       CM_CBLOCK    },
      { (byte *) "*22815",    CM_CALL_TYPE_STD_OTASP,       CM_DBLOCK    },
      { (byte *) "*22816",    CM_CALL_TYPE_STD_OTASP,       CM_EBLOCK    },
      { (byte *) "*22817",    CM_CALL_TYPE_STD_OTASP,       CM_FBLOCK    },
#endif
  };

#endif /* FEATURE_OTASP */

/* <EJECT> */
#ifdef FEATURE_UI_PBM
static nv_dial_type nv_emergency_numbers[PBM_NV_EMERGENCY_NUMBERS];
#endif
static void OEMTCallCtrl_Init(void)
{
#ifdef FEATURE_UI_PBM
   int i;

   for (i = 0; i < PBM_NV_EMERGENCY_NUMBERS; i++) {
      nv_emergency_numbers[i].address = (byte) (i + PBM_NV_EMERGENCY_NUMBER_START);
      (void) ui_get_nv(NV_DIAL_I, (nv_item_type*)&(nv_emergency_numbers[i]));
   } /* end of for */
#endif /* FEATURE_UI_PBM */
}
/*===========================================================================
FUNCTION   OEMT_GetCallType

DESCRIPTION
  Determine call type for a origination #

DEPENDENCIES
  None

RETURNS
  None, result will be put into call_type_ptr, otasp_code_ptr, and ecc_category

SIDE EFFECTS
  None
===========================================================================*/
static void OEMT_GetCallType(
  const cm_num_s_type     *orig_num_ptr,    // pointer to origination #
  cm_call_type_e_type     *call_type_ptr,   // associated call type, save result
  cm_activate_code_e_type *otasp_code_ptr,  // OTASP activation code(OTASP call)
  uint8                   *ecc_category     // emergency category (emergency call)
)
/*lint -esym(715,otasp_code_ptr)*/
{
#ifdef FEATURE_OTASP
  int   i;
#endif /* FEATURE_OTASP */
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
  uint8 ecc_cat = 0;
#endif

  /* If this is NOT a voice call, return now  */
  if( *call_type_ptr != CM_CALL_TYPE_VOICE ) {
    return;
  }

#ifdef FEATURE_UI_PBM
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
  if (pbm_emergency_number_cat(orig_num_ptr->buf, orig_num_ptr->len, &ecc_cat))
  {
    *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
    *ecc_category = ecc_cat;
    MSG_HIGH("found emergency number, category %d",ecc_cat,0,0);
    return;
  }
#else
  if (pbm_emergency_number(orig_num_ptr->buf, orig_num_ptr->len))
  {
    *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
    return;
  }
#endif

#endif


#ifdef FEATURE_OTASP
  /* "*228" is checked specially. */
  if ( orig_num_ptr->len == STRLEN(STAR228) &&
       ! MEMCMP(orig_num_ptr->buf, STAR228, STRLEN(STAR228)) )
  {
    *call_type_ptr  = CM_CALL_TYPE_NON_STD_OTASP;
    *otasp_code_ptr = CM_PREFERRED;
    return;
  }

  /* Consult the default hard-coded otasp numbers table to
  ** determine the originating number call type.
  */
  for( i=0; i< (int)ARR_SIZE(otasp_num_tbl); i++ )
    {
      /* Check whether one of the otasp numbers match the origination #.
      */
      if( orig_num_ptr->len >= STANDARD_OTASP_LENGTH &&
        ! MEMCMP(orig_num_ptr->buf, otasp_num_tbl[i].num_str, STANDARD_OTASP_LENGTH) )
      {
        /* If we found a match, change the call type to the corresponding,
        ** type in the table and indicate the OTASP activation code.
        */
        *call_type_ptr  = otasp_num_tbl[i].call_type;
        *otasp_code_ptr = otasp_num_tbl[i].activate_code;
        return;
      }
    } /* end of for */

    /* Not hard coded OTASP number, check NV phone book
    */
#ifdef FEATURE_UI_PBM
    for (i = 0; i < PBM_NV_EMERGENCY_NUMBERS; i++) {
      const byte* num2_buf;
      byte  num2_len;
      num2_buf = (byte *) nv_emergency_numbers[i].digits;
      num2_len = nv_emergency_numbers[i].num_digits;

      if (pbm_number_match(orig_num_ptr->buf, orig_num_ptr->len, num2_buf, num2_len)) {
        if (nv_emergency_numbers[i].status == NV_OTASP_SD_NUM 
            || nv_emergency_numbers[i].status == NV_OTASP2_SD_NUM) {
          /* If we found a match, change the call type to the corresponding,
          ** type in the table and indicate the OTASP activation code.
          */
          *call_type_ptr  = CM_CALL_TYPE_NON_STD_OTASP;
          *otasp_code_ptr = CM_PREFERRED;
          return;
        }
      } /*end if */
    } /* end of for */
#endif /* FEATURE_UI_PBM */
#endif /* FEATURE_OTASP */

}

/* <EJECT> */
/*===========================================================================
FUNCTION OEMT_CallCtrl_SetRrestrictions

DESCRIPTION
  This function sets the restriction_bitmask to the value from Core.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static byte gOEMT_SavedRestrictions;

void OEMTCallCtrl_SetRestrictions(byte restriction_bitmask)
{
  gOEMT_SavedRestrictions = restriction_bitmask;
}

/* <EJECT> */
/*===========================================================================
FUNCTION OEMT_IsRestricted

DESCRIPTION
  This function checks the restriction_bitmask that was set and returns TRUE
  if the PBM file given has the bit in the mask set which corresponds to this
  restriction.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_UI_PBM
boolean OEMTCallCtrl_IsRestricted(pbm_device_type pbm_file)
{
  switch ( pbm_file )
  {
    case PBM_EFS:
      return (0 != (gOEMT_SavedRestrictions & AEET_RESTRICT_CALLS_TO_EFS));
    case PBM_SDL:
      return (0 != (gOEMT_SavedRestrictions & AEET_RESTRICT_CALLS_TO_SPEED_DIAL_LIST));
    case PBM_ADN:
      return (0 != (gOEMT_SavedRestrictions & AEET_RESTRICT_CALLS_TO_ADN_SLOT1));
    case PBM_ADN2:
      return (0 != (gOEMT_SavedRestrictions & AEET_RESTRICT_CALLS_TO_ADN_SLOT2));
    case PBM_FDN:
#ifdef FEATURE_MMGSDI
      {
        sim_capabilities_T sim_capabilities;
        if (GSDI_SUCCESS == gsdi_get_sim_capabilities(&sim_capabilities, 0, NULL)) {
          return (sim_capabilities.fdn_enabled);
        }
      }
#endif
      return FALSE;
     
    case PBM_FDN2:
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      {
        sim_capabilities_T sim_capabilities;
        if (GSDI_SUCCESS == gsdi2_get_sim_capabilities(GSDI_SLOT_2, 
                                                       &sim_capabilities, 
                                                       0, NULL)) {
          return (sim_capabilities.fdn_enabled);
        }
      }
#endif
      return FALSE;
    default:
      return FALSE;
  }
}
#endif /* FEATURE_UI_PBM */

/* The following ENUM is used to determine if call control is required for
 * a given technology.  Call control and the SIM from which the subscription
 * is obtained are now linked.
 */
typedef enum {
  CC_NOT_REQUIRED,              /* Either not a SIM subscription or the SIM
                                   doesn't require Call Control */
  CC_REQUIRED,                  /* Call control is required for this
                                   subscription */
  CC_COMPLETED                  /* Call control was required, and has been
                                   completed. */
} OEMT_Call_Ctrl_e_type;

/* The following structure is used when issueing a call control command
 * to keep track of what is required, what has been completed, and the
 * results.
 */
typedef struct {
  OEMT_Call_Ctrl_e_type sim1_cc_required;      /* What to do about SIM 1 */
  OEMT_Call_Ctrl_e_type sim2_cc_required;      /* What to do about SIM 2 */
  cm_nc_call_control_complete_cb_f_type *cb_func;       /* Callback to use */
  void  *user_data;                             /* User data supplied. */
  cm_call_id_type call_id;                      /* Original Call ID. */
  cm_cc_status_e_type status;                   /* Determined status. */
  cm_call_type_e_type call_type;                /* Determined call type */
  cm_orig_call_control_s_type original_origination_request; /* Original orig. */
  cm_orig_call_control_s_type sim1_cc_result;   /* Result of first SIM CC */
  cm_orig_call_control_s_type sim2_cc_result;   /* Result of second SIM CC */
  cm_orig_call_control_s_type *cdma_result_ptr; /* Pointer to above. */
  cm_orig_call_control_s_type *gw_result_ptr; /* Pointer to above. */
} CC_REQUEST;

/* <EJECT> */
/*===========================================================================
FUNCTION OEMTCallCtrl_Send_CC_Command

DESCRIPTION
  Look at the request structure and determine which request to do next.
  This function will then call OEMTCallCtrl_SendCmdToGSDI to issue
  the request on the correct slot.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  request_ptr may be modified if the request fails.

===========================================================================*/
static void OEMTCallCtrl_Send_CC_Command(CC_REQUEST *request_ptr)
{
#if defined(FEATURE_GSTK)
  /* If we are required to send the command to SLOT 1. */
  if (request_ptr->sim1_cc_required == CC_REQUIRED)
  {
    if (OEMTCallCtrl_SendCmdToGSDI(GSTK_SLOT_1, GSTK_VOICE,
        request_ptr->original_origination_request.num.buf,
        request_ptr->original_origination_request.num.len,
        request_ptr->original_origination_request.num.subaddr.extension_bit?
          &request_ptr->original_origination_request.num.subaddr.num_char:NULL,
        (uint32) request_ptr, OEMTCallCtrl_CC_Command_Cb))
      return;
    else
    {
      request_ptr->sim1_cc_required = CC_NOT_REQUIRED;
      request_ptr->status = CM_CC_STATUS_ERROR;
      request_ptr->sim1_cc_result.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
    }
  }
#ifdef FEATURE_GSTK_DUAL_SLOTS
  /* Slot 1 is complete, now check if we are required to send
   * the command to SLOT 2. */
  if (request_ptr->sim2_cc_required == CC_REQUIRED)
  {
    if (OEMTCallCtrl_SendCmdToGSDI(GSTK_SLOT_2, GSTK_VOICE,
        request_ptr->original_origination_request.num.buf,
        request_ptr->original_origination_request.num.len,
        request_ptr->original_origination_request.num.subaddr.extension_bit?
          &request_ptr->original_origination_request.num.subaddr.num_char:NULL,
        (uint32) request_ptr, OEMTCallCtrl_CC_Command_Cb))
      return;
    else
    {
      request_ptr->sim2_cc_required = CC_NOT_REQUIRED;
      request_ptr->status = CM_CC_STATUS_ERROR;
      request_ptr->sim2_cc_result.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
    }
  }
#endif
#endif /* FEATURE_GSTK */

  /* If we reach here, both SLOT 1 and SLOT 2 is complete, this will
   * send the command to CM. */
  (*request_ptr->cb_func)( request_ptr->call_id,
                           request_ptr->status,
                           request_ptr->call_type,
                           request_ptr->cdma_result_ptr,
                           request_ptr->gw_result_ptr,
                           (void*) request_ptr->user_data);

  /* This sequence is complete, so free the memory. */
  OEM_Free(request_ptr);
} //void send_cc_command(CC_REQUES...

#if defined(FEATURE_GSTK)
/*===========================================================================
FUNCTION uixgsdi_ascii_to_gstk

DESCRIPTION
  The GSTK wants digits with the decimal value in each byte.  We need to
  the character string convert this array of integers.

RETURN VALUE
  None.

DEPENDENCIES
  None.
===========================================================================*/
static void OEMTCallCtrl_ascii_to_gstk(byte *buf, uint32 len, gstk_address_type *address)
{
   int i = 0;   /* Loop variable though buf array. */
   int j = 0;   /* Loop variable for addresses array of decimal bytes. */

   address->TON = GSTK_UNKNOWN_TON;
   address->NPI = GSTK_ISDN_TELEPHONY_NPI;

   if (buf[i] == '+')
   {
     address->TON = GSTK_INTERNATIONAL_NUM;
     i++;
   }

   for (; i < len; i++)
   {
     switch (buf[i])
     {
       case '0': case '1': case '2': case '3': case '4':
       case '5': case '6': case '7': case '8': case '9':
         address->address[j++] = buf[i] - '0';
         break;
       case '*':
         address->address[j++] = 0x0a;
         break;
       case '#':
         address->address[j++] = 0x0b;
         break;
       default:
         ERR("Unexpected value for buf[%d], %x", i, buf[i], 0);
         break;
     } //     switch (buf[i])...
   } //   for (j = 0, i = 0; i < len;...

   address->length = j;                     /* Save the length. */
   return;
} //void uixgsdi_ascii_to_gstk(byt...

/* <EJECT> */
/*===========================================================================

FUNCTION OEMTCallCtrl_SendCmdToGSDI

DESCRIPTION
  Take the SS command and sends the envelope command to GSTK.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean OEMTCallCtrl_SendCmdToGSDI(gstk_slot_id_enum_type slot,
    gstk_call_enum_type cmd, byte *buf, int length,
    byte *subaddress,
    uint32 user_data,
    gstk_evt_cb_funct_type gstk_cc_cb )
{
  gstk_cc_ind_type command_data;
  gstk_status_enum_type status;

  if (!gstk_slot_is_cc_required(slot))
    return FALSE;

  MEMSET(&command_data, 0, sizeof(command_data));

  command_data.call_enum_type = cmd;
  switch (cmd)
  {
    case GSTK_SS:
    {
      command_data.gstk_address_string.ss_string.TON = GSTK_RFU_TON;
      command_data.gstk_address_string.ss_string.NPI = GSTK_RFU_NPI;
      command_data.gstk_address_string.ss_string.length = length;
      MEMCPY(command_data.gstk_address_string.ss_string.address, buf, length);
      break;
    }
    case GSTK_USSD:
    {
      command_data.gstk_address_string.ussd_string.dcs = 0; /* DCS from cell broadcast. */
      command_data.gstk_address_string.ussd_string.length = length;
      command_data.gstk_address_string.ussd_string.text = buf;
      break;
    }
    case GSTK_VOICE:
    {
      OEMTCallCtrl_ascii_to_gstk(buf, length,
          &command_data.gstk_address_string.voice.address);

      if ((subaddress != NULL) &&
          (*subaddress <
           sizeof(command_data.gstk_address_string.voice.subaddress.subaddress))
         )
      {
        command_data.gstk_address_string.voice.subaddress.length = *subaddress;
        MEMCPY(command_data.gstk_address_string.voice.subaddress.subaddress,
            &subaddress[1], subaddress[0]);
      }

      command_data.gstk_address_string.voice.ccp1.length = 0;
      command_data.gstk_address_string.voice.ccp2.length = 0;
      break;
    }

    default:
      ERR("Unexpected cmd to OEMTCallCtrl_SendCmdToGSDI %d", cmd, 0, 0);
      return FALSE;
      break;
  }
  /* build the envelope command and send it to the GSTK; */
  status = gstk_slot_send_envelope_cc_command (slot, (uint32) user_data, &command_data,
                  gstk_cc_cb);
  return (GSTK_SUCCESS == status);
}

/* <EJECT> */
/*===========================================================================
FUNCTION OEMTCallCtrl_gstk_to_cm_ton

DESCRIPTION
  GSTK has their own constants for the type of number.  This function will
  map a GSTK Type of number to the CM type of number.

DEPENDENCIES
  None

RETURN VALUE
  CM Type of Number.

SIDE EFFECTS
  None

===========================================================================*/
static uint8 OEMTCallCtrl_gstk_to_cm_ton(gstk_ton_enum_type ton)
{
  switch (ton)
  {
    case GSTK_UNKNOWN_TON: return CM_TON_UNKNOWN;
    case GSTK_INTERNATIONAL_NUM: return CM_TON_INTERNATIONAL;
    case GSTK_NATIONAL_NUM: return CM_TON_NATIONAL;
    case GSTK_NETWORK_SPECIFIC_NUM: return CM_TON_NETWORK_SPECIFIC;
    case GSTK_RFU_TON: return CM_TON_RESERVED_5;
    default: return CM_TON_UNKNOWN;
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION OEMTCallCtrl_gstk_to_cm_npi

DESCRIPTION
  GSTK has their own constants for the number plan indication.  This function will
  map a GSTK number plan to the CM number plan.

DEPENDENCIES
  None

RETURN VALUE
  CM Number Plan.

SIDE EFFECTS
  None.

===========================================================================*/
static uint8 OEMTCallCtrl_gstk_to_cm_npi(gstk_npi_enum_type npi)
{
  switch (npi)
  {
    case GSTK_UNKNOWN_NPI: return CM_NPI_UNKNOWN;
    case GSTK_ISDN_TELEPHONY_NPI: return CM_NPI_ISDN;
    case GSTK_DATA_NPI: return CM_NPI_DATA;
    case GSTK_TELEX_NPI: return CM_NPI_TELEX;
    case GSTK_PRIVATE_NPI: return CM_NPI_PRIVATE;
    case GSTK_EXT_RESERVED_NPI: return CM_NPI_RESERVED_EXTENSION;
    case GSTK_RFU_NPI: return CM_NPI_UNKNOWN;
  }
  return CM_NPI_UNKNOWN;
}

/* <EJECT> */
/*===========================================================================
FUNCTION OEMTCallCtrl_CC_Command_Cb

DESCRIPTION
  When the envelope command for call control is completed, this function
  is called by GSDI/GSTK to communicate the results.   The user_data should
  be the CC_REQUEST structure we were working with.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void OEMTCallCtrl_CC_Command_Cb(gstk_cmd_from_card_type *req_rsp)
{
  /* Hacks for now. */
  CC_REQUEST *request_ptr = (CC_REQUEST *) req_rsp->hdr_cmd.user_data;
  cm_orig_call_control_s_type *sim_cc_result_ptr = NULL;

  if (request_ptr->sim1_cc_required == CC_REQUIRED)
  {
    request_ptr->sim1_cc_required = CC_COMPLETED;
    sim_cc_result_ptr = &request_ptr->sim1_cc_result;
  }
#ifdef FEATURE_GSTK_DUAL_SLOTS
  else if (request_ptr->sim2_cc_required == CC_REQUIRED)
  {
    request_ptr->sim2_cc_required = CC_COMPLETED;
    sim_cc_result_ptr = &request_ptr->sim2_cc_result;
  }
#endif
  else
  {
    ERR("Unexpected callback.  No request pending.", 0, 0, 0);
  }

  /* If the data looks good, process the response. */
  if (sim_cc_result_ptr)
  {
    /* Save the results of the CC command in the structure. */
    if (req_rsp->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS)
    {
      /* Call control can say the call is allowed, but with a modification, */
      /* the call is allowed as is, or, the call is not allowed. */
      switch(req_rsp->cmd.cc_rsp.call_ctrl_result)
      {
        case GSTK_ALLOWED_BUT_MOD:
          /* They have given us a modification to the original command. */
          switch (req_rsp->cmd.cc_rsp.cc_req_action.call_type)
          {
            case GSTK_VOICE:
              /* Apply the origination if its still an origination. */
              sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE;
              sim_cc_result_ptr->num.len = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.length;
              (void) gstk_address_to_ascii(&req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address,
                                           sim_cc_result_ptr->num.buf, CM_MAX_NUMBER_CHARS);
              sim_cc_result_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
              sim_cc_result_ptr->num.number_type = OEMTCallCtrl_gstk_to_cm_ton(req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.TON);
              sim_cc_result_ptr->num.number_plan = OEMTCallCtrl_gstk_to_cm_npi(req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address.NPI);
              sim_cc_result_ptr->num.pi = 0;
              sim_cc_result_ptr->num.si = 0;


              /* per spec, 112 is the only emergency number */
              if ((sim_cc_result_ptr->num.len == 3) &&
                (sim_cc_result_ptr->num.buf[0] == '1')  &&
                (sim_cc_result_ptr->num.buf[1] == '1')  &&
                (sim_cc_result_ptr->num.buf[2] == '2') ) {
                request_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
                sim_cc_result_ptr->num.emerg_srv_categ = 0;
#endif
              }

              break;
            case GSTK_SS:
              sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS;
              sim_cc_result_ptr->num.len = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string.length;
              /* the SS needs to be parsed, and sim_cc_result_ptr->sups_cmd needs to be filled in.  */
              /* This needs to work in ThinUI as well, so calling ISuppServ won't work here */
              (void) gstk_address_to_ascii(&req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string,
                                           sim_cc_result_ptr->num.buf, CM_MAX_NUMBER_CHARS);
              break;
            case GSTK_USSD:
              {
                int length = req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.length;

                if (length > sizeof(sim_cc_result_ptr->num.buf))
                  length = sizeof(sim_cc_result_ptr->num.buf);

                sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD;
                sim_cc_result_ptr->num.len = length;
                sim_cc_result_ptr->sups_cmd = CM_SUPS_CMD_PROCESS_USS;
                MEMCPY(sim_cc_result_ptr->num.buf,
                       req_rsp->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text,
                       length);
              }
              break;
            default:
              ERR("Unexpected call type returned by GSTK %d", req_rsp->cmd.cc_rsp.cc_req_action.call_type, 0, 0);
              break;
          }
          break;
        case GSTK_NOT_ALLOWED:
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
          break;
        case GSTK_ALLOWED_NO_MOD:
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
          break;
        default:
          /* We got an error from GSTK.  Should we allow the call or not? */
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
          ERR("Unexected response from GSTK. %d",
              req_rsp->cmd.cc_rsp.general_resp, 0, 0);
          break;
      }
    }
    else
    {
      /* We got an error from GSTK.  Should we allow the call or not? */
      ERR("GSTK returned an error. %d", req_rsp->cmd.cc_rsp.general_resp, 0, 0);
      sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    }
  }


  /* Call Send CC command to send the next or report we are done. */
  OEMTCallCtrl_Send_CC_Command(request_ptr);
}
#endif /* defined(FEATURE_GSTK)   */

/* <EJECT> */
/*===========================================================================
FUNCTION OEMTCallCtrl_NumClassification_cb

DESCRIPTION
  The new NC function that checks all information about a number, including call
  control will have the following design.

  First, check if its an emergency call.
   If Emergency, return that the call is an emergency number, and CC not required.

  Second, the UI has non-required by specification user limits, specifically:
    Limit calls to numbers in the NV phone book(s)
    Limit calls to SIM1 ADN
    Limit calls to SIM2 ADN
  If any of these limitations are set and the number does not match these numbers,
    Return the call is not allowed on either SIM.

  Third if the call is still allowed
   If either subscription uses SIM1:
     Do FDN check on SIM1
       If the call is not allowed on SIM1, mark the call as not allowed for modes whose subscription comes from SIM1.

   If either subscription uses SIM2
      Do FDN check on SIM2
         If the call is not allowed on SIM2, mark the call as not allowed for modes whose subscription comes from SIM2.

  If the call is not allowed now on either mode,
    Return the call is not allowed on either SIM.

 Fourth Call the following do_cc function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean OEMTCallCtrl_NumClassification_cb (
  cm_call_id_type                       call_id, /* call id */
  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */
  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */
  cm_call_type_e_type                   call_type,
    /* call type */
  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */
  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */
  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func
    /* function to be called when processing is complete */
#ifdef CM_API_GW_PS_CC
  , cm_gw_ps_cc_params_s_type             *gw_ps_cc_params
  /* Call control parameters for Packet data call */
#endif
)
{
  CC_REQUEST *cc_request = NULL;        /* Call control request structure. */
  boolean call_allowed_cdma = TRUE;     /* Is call still allowed on CDMA? */
  boolean call_allowed_gw = TRUE;       /* Is call still allowed on GW? */
  int i;                                /* Control variable for loops. */
  uint8 ecc_category;                   /* emergency call category */

#if defined(FEATURE_UIM) && defined(FEATURE_UI_PBM)
  /* Loop constants for looking through different sources of information. */
  cm_cc_subscription_source_e_type sub_sources[] =
    { CM_CC_SUBSCRIPTION_SOURCE_SIM_1, CM_CC_SUBSCRIPTION_SOURCE_SIM_2 };
  pbm_device_type pbm_fdns[] = { PBM_FDN, PBM_FDN2 };
#endif /* defined(FEATURE_UIM) && defined(FEATURE_UI_PBM) */

  /* Check incoming parameters. */
  if ((orig_num_ptr == NULL) || (cm_call_control_complete_cb_func == NULL))
  {
    ERR("Bad parameter from CM %x %x", orig_num_ptr,
        cm_call_control_complete_cb_func, 0);
    return FALSE;
  }

  /* This memory is freed after we issue the callback to CM. */
  if ((cc_request = (CC_REQUEST *)OEM_Malloc(sizeof(CC_REQUEST))) == NULL)
  {
    ERR("Could not allocate response structure.", 0, 0, 0);
    return FALSE;
  }
  MEMSET(cc_request, 0, sizeof(CC_REQUEST));
  cc_request->sim1_cc_required = CC_NOT_REQUIRED;
  cc_request->sim2_cc_required = CC_NOT_REQUIRED;
  cc_request->call_id = call_id;
  cc_request->user_data = user_data;
  cc_request->cb_func = cm_call_control_complete_cb_func;
  cc_request->status = CM_CC_STATUS_SUCCESS;
  cc_request->call_type = call_type;
  cc_request->cdma_result_ptr = &cc_request->original_origination_request;
  cc_request->gw_result_ptr   = &cc_request->original_origination_request;
  cc_request->original_origination_request.call_control_result =
                                                CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

  cc_request->original_origination_request.alpha.len = 0;
  MEMSET(cc_request->original_origination_request.alpha.buf,
      0, sizeof(cc_request->original_origination_request.alpha.buf));
  cc_request->original_origination_request.num = *orig_num_ptr;

  /* Get the call type.  */
  OEMT_GetCallType(orig_num_ptr, &(cc_request->call_type),
                &(cc_request->original_origination_request.activate_code),
                &ecc_category);

  /* The following check will ensure that we allow emergency calls
   * without further checking.  Also, if it is a packet data call,
   * OTASP, or any other call that typically does not have a number,
   * we will tell CM to allow this call.  CM will be responsible for
   * determining if another entity should verify call restrictions/
   * call control on data calls.
   */
  if ((cc_request->call_type != CM_CALL_TYPE_VOICE) &&
      (cc_request->call_type != CM_CALL_TYPE_CS_DATA))
  {
#ifdef CM_API_ECATEG_IN_NUM_STRUCT
    cc_request->gw_result_ptr->num.emerg_srv_categ = ecc_category;
#endif
    MSG_HIGH("skipping further call control. type: %d, cat %d",
             cc_request->call_type, ecc_category, 0);
    (*cm_call_control_complete_cb_func)(call_id,
                                      CM_CC_STATUS_SUCCESS,
                                      cc_request->call_type,
                                      cc_request->cdma_result_ptr,
                                      cc_request->gw_result_ptr,
                                      (void*) user_data);
    OEM_Free(cc_request);
    return TRUE;
  }

  if (CM_CC_SUBSCRIPTION_SOURCE_NONE == cdma_subscription_source)
  {
    call_allowed_cdma = FALSE;
    cc_request->cdma_result_ptr = NULL;
  }

  if (CM_CC_SUBSCRIPTION_SOURCE_NONE == gw_subscription_source)
  {
    call_allowed_gw = FALSE;
    cc_request->gw_result_ptr = NULL;
  }

  /* Second, the SIM has required checks of the FDN.  If the FDN is
   * enabled, these must be checked.
   * Limit calls to SIM1 FDN
   * Limit calls to SIM2 FDN
   */
#if defined(FEATURE_UIM) && defined(FEATURE_UI_PBM)
  for (i = 0; i < ARR_SIZE(sub_sources); i++)
  {
    if ((cdma_subscription_source == sub_sources[i]) ||
        (gw_subscription_source   == sub_sources[i]))
    {
      /* If FDN is enabled, make sure we check the FDN. */
      if (OEMTCallCtrl_IsRestricted(pbm_fdns[i]))
      {
        if (PBM_SUCCESS != pbm_find_number(pbm_fdns[i], orig_num_ptr->buf,
                                           orig_num_ptr->len, NULL, NULL))
        {
          if (cdma_subscription_source == sub_sources[i])
          {
            call_allowed_cdma = FALSE;
            cc_request->cdma_result_ptr = NULL;
          }
          if (gw_subscription_source == sub_sources[i])
          {
            call_allowed_gw = FALSE;
            cc_request->gw_result_ptr = NULL;
          }
        }
      }
    }
  }
  /* Now the call has passed the UIM required checks for being allowed,  */
#endif /* defined(FEATURE_UIM) && defined(FEATURE_UI_PBM) */

#ifdef FEATURE_UI_PBM
  /* Now check for the restrictions enforced by the UI itself.
   * Specifically:
   * Limit calls to numbers in the NV phone book(s)
   * Limit calls to SIM1 ADN
   * Limit calls to SIM2 ADN
   * If any of these limitations are set and the number does not match these
   * numbers, Return the call is not allowed on any subscription source.
   */
  if (call_allowed_cdma || call_allowed_gw)
  {
    boolean passes_optional_checks = FALSE;
    boolean has_optional_restriction = FALSE;
    pbm_device_type optional_list[] = {PBM_EFS, PBM_SDL, PBM_ADN, PBM_ADN2};

    for (i = 0; (!passes_optional_checks && (i < ARR_SIZE(optional_list))); i++)
    {
      if (OEMTCallCtrl_IsRestricted(optional_list[i]))
      {
        has_optional_restriction = TRUE;
        if (PBM_SUCCESS == pbm_find_number(optional_list[i], orig_num_ptr->buf,
                                           orig_num_ptr->len, NULL, NULL) )
        {
          passes_optional_checks = TRUE;
          break;
        }
      }
    }

    if (has_optional_restriction && !passes_optional_checks)
    {
      call_allowed_cdma = call_allowed_gw = FALSE;
      cc_request->cdma_result_ptr = NULL;
      cc_request->gw_result_ptr = NULL;
    }
  } //  if (call_allowed_cdma || cal...
#endif /* defined(FEATURE_UI_PBM) */

  if (!call_allowed_cdma && !call_allowed_gw)
  {
    /* The call is not allowed on either system, so say no to both. */
    cc_request->original_origination_request.call_control_result =
      CM_CC_RESULT_NOT_ALLOWED;
    (*cm_call_control_complete_cb_func)(call_id,
                                      CM_CC_STATUS_SUCCESS,
                                      cc_request->call_type,
                                      cc_request->cdma_result_ptr,
                                      cc_request->gw_result_ptr,
                                      (void*) user_data);
    OEM_Free(cc_request);
    return TRUE;
  }

#if defined(FEATURE_GSTK)
  /* If either subscription comes from SIM1, */
  if ((call_allowed_cdma && (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)) ||
      (call_allowed_gw && (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)))
  {
    MEMCPY(&cc_request->sim1_cc_result,
           &cc_request->original_origination_request,
           sizeof(cm_orig_call_control_s_type));

    if (call_allowed_gw && (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1) )
      cc_request->gw_result_ptr = &cc_request->sim1_cc_result;
    if (call_allowed_cdma && (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1) )
      cc_request->cdma_result_ptr = &cc_request->sim1_cc_result;

    /* Check if CC is required from SIM1.
     * If so, set a variable to remember that CC is required
     * for those subscriptions.
     */
    if (gstk_slot_is_cc_required(GSTK_SLOT_1))
    {
      cc_request->sim1_cc_required = CC_REQUIRED;
    }
  }
#if defined (FEATURE_GSTK_DUAL_SLOTS)
  /* If either subscription comes from SIM2, */
  if ((call_allowed_cdma && (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)) ||
      (call_allowed_gw   && (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)))
  {
    MEMCPY(&cc_request->sim2_cc_result,
           &cc_request->original_origination_request,
           sizeof(cm_orig_call_control_s_type));
    if (call_allowed_gw   && (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))
      cc_request->gw_result_ptr = &cc_request->sim2_cc_result;
    if (call_allowed_cdma && (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))
      cc_request->cdma_result_ptr = &cc_request->sim2_cc_result;

    /* Check if CC is required from SIM2.
     * If so, set a variable to remember that CC is required
     * for those subscriptions.
     */
    if (gstk_slot_is_cc_required(GSTK_SLOT_2))
    {
      cc_request->sim2_cc_required = CC_REQUIRED;
    }
  }
#endif /* defined(FEATURE_GSTK_DUAL_SLOTS) */
#endif /* defined(FEATURE_GSTK)  */

  /* Call the function that sends CC requests based on the above structure. */
  OEMTCallCtrl_Send_CC_Command(cc_request);
  return TRUE;
} //void start_call_control()...


/* <EJECT> */
/*===========================================================================
FUNCTION OEMT_CallCtrl_Preferences

DESCRIPTION
 Type for call control preferences function to be registered by a client.  CM will
 call this function after call control is complete to mediate between which modes
 to use in the case where the call is allowed on both modes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_cc_mode_pref_e_type OEMTCallCtrl_Preferences (

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */

)
{
   return CM_CC_MODE_PREF_ANY;
}

void OEMTCallCtrl_Reg(void)
{
   OEMTCallCtrl_Init();
   cm_nc_reg_with_call_control(OEMTCallCtrl_NumClassification_cb,
                               OEMTCallCtrl_Preferences);

}


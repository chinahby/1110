/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

            C H A N N E L  M A N A G E M E N T  P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the operating procedures that pertain to the
  channel management procedures as defined in the following specifications:
  7816-4.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Requrires that following events are generated prior to processing of the
  commands.
  GSDI_SIM_INIT_COMPLETED           (SLOT1)

  If the Target is a dual slotted target, then the following events, in addition
  to the event mentioned above will need to be generated before processing of
  these functions can occur on the respective slot.
  GSDI_SIM_INIT_COMPLETED_NO_PROV   (SLOT1)
  GSDI_SIM_INIT_COMPLETED_2         (SLOT2)
  GSDI_SIM_INIT_COMPLETED_NO_PROV_2 (SLOT2)

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2005 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_chnl_mgt.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   js      Added prototypes for static functions
07/07/06   sun     Removed Reference to mc.h
11/09/05   sun     Fixed lint errors
05/11/05   sst     Lint fixes
01/11/04   jar     Fix to allow 256+ Responses
11/15/04   jar     Use MMGSDI MACROS
04/28/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"
#include "task.h"
#include "intconv.h"

/* System and Interfaces */
#include "msg.h"
#include "err.h"
#include "msg.h"
#include "err.h"
#include "dsm.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
#if defined ( FEATURE_MMGSDI_GSM ) || defined ( FEATURE_MMGSDI_UMTS )
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */
/* GSDI */
#include "gsdi_exp.h"
#include "gsdi.h"
#include "gsdi_chnl_mgt.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static boolean gsdi_chnl_mgt_is_channel_id_valid(
  int32   channel_id
);

static boolean gsdi_chnl_mgt_is_slot_valid(
  gsdi_slot_id_type slot
);

static boolean gsdi_chnl_mgt_is_data_len_valid(
  int32 data_len
);

static boolean gsdi_chnl_mgt_is_data_buffer_valid(
  byte * data_buffer
);

static boolean gsdi_chnl_mgt_validate_input_params(
  void *                     req_p,
  gsdi_chnl_mgt_id_enum_type chnl_mgt_id
);


/*===========================================================================
FUNCTION GSDI_CHNL_MGT_IS_CHANNEL_ID_VALID

DESCRIPTION
  This function is used to verify that the channel ID provided is a
  valid channel.

  The only acceptable values are:
  channel_id greater than or equal to 0

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_chnl_mgt_is_channel_id_valid(
  int32   channel_id
)
{
  if (channel_id >= 0)
    return TRUE;
  else
    return FALSE;
}
/*===========================================================================
FUNCTION GSDI_CHNL_MGT_IS_SLOT_VALID

DESCRIPTION
  This function is used to verify the slot id received in the message.
  All channel management procedures require that the correct slot
  is provided in the request.

  The only acceptable values are:
  GSDI_SLOT_1 -> 0x01
  GSDI_SLOT_2 -> 0x02

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_chnl_mgt_is_slot_valid(
  gsdi_slot_id_type slot
)
{
  boolean result = FALSE;

  switch ( slot )
  {
    case GSDI_SLOT_1:
      result = TRUE;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      result = TRUE;
      break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",slot,0,0);
      break;
  }
  return result;
} /* gsdi_chnl_mgt_is_slot_valid */

/*===========================================================================
FUNCTION GSDI_CHNL_MGT_IS_DATA_LEN_VALID

DESCRIPTION
  This function is used to verify the length of the data meets the following
  requirements:

  1. Data must be greater than 0
  2. Data must be less than GSDI_MAX_DATA_BLOCK_LEN

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_len is valid for configuration
            FALSE: data_len is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_chnl_mgt_is_data_len_valid(
  int32 data_len
)
{
  if ( data_len == 0 )
    return FALSE;

  /* --------------------------------------------------------------------------
     Allow the maximum for Send and Receive.  Allow the Send portion
     to regulate the length further and trim it down to
     ------------------------------------------------------------------------*/
  if ( data_len >= GSDI_MAX_CHANNEL_DATA_RSP_LEN )
    return FALSE;

  return TRUE;
} /* gsdi_chnl_mgt_is_data_len_valid*/

/*===========================================================================
FUNCTION GSDI_CHNL_MGT_IS_DATA_BUFFER_VALID

DESCRIPTION
  This function is used to verify the buffer used is valid.

  1. Buffer Pointer Can not be NULL

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_buffer is valid for request
            FALSE: data_buffer is not valid for request

SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_chnl_mgt_is_data_buffer_valid(
  byte * data_buffer
)
{
  if ( data_buffer == NULL )
    return FALSE;

  return TRUE;
} /* gsdi_chnl_mgt_is_data_buffer_valid */

/*===========================================================================
FUNCTION GSDI_PROC_VERIFY_INPUT_PARAMS

DESCRIPTION
  Generic Function used through the Channel Management Procedures to verify
  the important parameters required to complete an OPEN,CLOSE,SEND DATA, or
  GET ATR Procedure.

DEPENDENCIES
  None

RETURN VALUE
  boolean   TRUE:  Parameters pass checking and are valid for request
            FALSE: Parameters fail checking and are not valid for request
SIDE EFFECTS
  None
===========================================================================*/
static boolean gsdi_chnl_mgt_validate_input_params(
  void *                     req_p,
  gsdi_chnl_mgt_id_enum_type chnl_mgt_id
)
{
  boolean        param_valid  = TRUE;

  if (req_p == NULL)
  {
    MSG_ERROR("Null input ptr, chnl_mgt_id = 0x%x", chnl_mgt_id, 0, 0);
    return FALSE;
  }

  switch ( chnl_mgt_id )
  {
    case GSDI_CHNL_MGT_ID_OPEN:
      /* Validate the slot ID */
      param_valid &= gsdi_chnl_mgt_is_slot_valid(
                      ((gsdi_open_channel_req_T *)req_p)->message_header.slot);

      param_valid &= gsdi_chnl_mgt_is_channel_id_valid(
                      ((gsdi_open_channel_req_T *)req_p)->channel_id);
      param_valid &= gsdi_chnl_mgt_is_channel_id_valid(
                      ((gsdi_open_channel_req_T *)req_p)->requested_channel_id);
      break;

    case GSDI_CHNL_MGT_ID_CLOSE:
      param_valid &= gsdi_chnl_mgt_is_slot_valid(
                      ((gsdi_close_channel_req_T *)req_p)->message_header.slot);

      param_valid &= gsdi_chnl_mgt_is_channel_id_valid(
                      ((gsdi_close_channel_req_T *)req_p)->channel_id);

      param_valid &= gsdi_chnl_mgt_is_channel_id_valid(
                      ((gsdi_close_channel_req_T *)req_p)->requested_channel_id);
      break;

    case GSDI_CHNL_MGT_ID_SEND_DATA:
      param_valid &= gsdi_chnl_mgt_is_slot_valid(
                      ((gsdi_send_channel_data_req_T *)req_p)->message_header.slot);

      param_valid &= gsdi_chnl_mgt_is_channel_id_valid(
                      ((gsdi_send_channel_data_req_T *)req_p)->channel_id);

      param_valid &= gsdi_chnl_mgt_is_data_len_valid(
                      ((gsdi_send_channel_data_req_T *)req_p)->apdu_len);

      param_valid &= gsdi_chnl_mgt_is_data_buffer_valid(
                      ((gsdi_send_channel_data_req_T *)req_p)->data_buffer);
      break;

    case GSDI_CHNL_MGT_ID_GET_ATR:
      param_valid &= gsdi_chnl_mgt_is_slot_valid(
                      ((gsdi_get_atr_req_T *)req_p)->message_header.slot);
      break;

    case GSDI_CHNL_MGT_ID_NONE:
    default:
      MMGSDI_DEBUG_MSG_ERROR("BAD CHNL MGT ID: %x", chnl_mgt_id,0,0);
      param_valid = FALSE;
      break;
  }
  return param_valid;
}

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_OPEN_CHANNEL

DESCRIPTION
  This is the function called from the main procedure handling function.
  This funciton will validate the important parameters.  Once the
  parameters pass the checking, it will call the GSDI - UIM Function
  to send it to the UIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_open_channel(
    gsdi_open_channel_req_T *  req_p,
    int32 *                    returned_data_len_p,
    byte *                     data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  boolean        params_valid = FALSE;

  /* --------------------------------------------------------------------------
     Verify the Message Contents to ensure corruption of
     important parameters did not occur
     -------------------------------------------------------------------------*/
  params_valid = gsdi_chnl_mgt_validate_input_params((void *)req_p,
                                                      GSDI_CHNL_MGT_ID_OPEN);

  if (params_valid == FALSE )
    return GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Ready to Build GSDI - UIM Message for a Manage Channel - Open
     -------------------------------------------------------------------------*/
  gsdi_status = gsdi_uim_server_open_channel(req_p,returned_data_len_p,data_p);

  if ( gsdi_status != GSDI_SUCCESS)
    MMGSDI_DEBUG_MSG_ERROR("OPEN CHANNEL REQ FAILURE: %x",gsdi_status,0,0);

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_CLOSE_CHANNEL

DESCRIPTION
  This is the function called from the main procedure handling function.
  This funciton will validate the important parameters.  Once the
  parameters pass the checking, it will call the GSDI - UIM Function
  to send it to the UIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_close_channel(
    gsdi_close_channel_req_T *  req_p,
    int32 *                     returned_data_len_p,
    byte *                      data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  boolean        params_valid = FALSE;

  /* --------------------------------------------------------------------------
     Verify the Message Contents to ensure corruption of
     important parameters did not occur
     -------------------------------------------------------------------------*/
  params_valid = gsdi_chnl_mgt_validate_input_params((void *)req_p,
                                                      GSDI_CHNL_MGT_ID_CLOSE);

  if (params_valid == FALSE )
    return GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Ready to Build GSDI - UIM Message for a Manage Channel - Open
     -------------------------------------------------------------------------*/
  gsdi_status = gsdi_uim_server_close_channel(req_p,returned_data_len_p,data_p);

  if ( gsdi_status != GSDI_SUCCESS)
    MMGSDI_DEBUG_MSG_ERROR("CLOSE CHANNEL REQ FAILURE: %x",gsdi_status,0,0);

  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_SEND_CHANNEL_DATA

DESCRIPTION
  This is the function called from the main procedure handling function.
  This funciton will validate the important parameters.  Once the
  parameters pass the checking, it will call the GSDI - UIM Function
  to send it to the UIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_send_channel_data(
    gsdi_send_channel_data_req_T *  req_p,
    int32 *                         returned_data_len_p,
    byte *                          data_p
)
{
  gsdi_returns_T gsdi_status  = GSDI_SUCCESS;
  boolean        params_valid = FALSE;

  /* --------------------------------------------------------------------------
     Verify the Message Contents to ensure corruption of
     important parameters did not occur
     -------------------------------------------------------------------------*/
  params_valid = gsdi_chnl_mgt_validate_input_params((void *)req_p,
                                                      GSDI_CHNL_MGT_ID_SEND_DATA);
  if (params_valid == FALSE )
    return GSDI_ERROR;

  /* --------------------------------------------------------------------------
     Ready to Build GSDI - UIM Message for a Manage Channel - Open
     -------------------------------------------------------------------------*/
  gsdi_status = gsdi_uim_server_send_channel_data(req_p,
                                                  returned_data_len_p,
                                                  data_p);

  if ( gsdi_status != GSDI_SUCCESS)
    MMGSDI_DEBUG_MSG_ERROR("SEND CHANNEL DATA REQ FAILURE: %x",gsdi_status,0,0);
  return gsdi_status;
}

/*===========================================================================
FUNCTION GSDI_PROC_CHNL_MGT_GET_ATR

DESCRIPTION
  This is the function called from the main procedure handling function.
  This funciton will validate the important parameters.  Once the
  parameters pass the checking, it will call the GSDI - UIM Function
  to send it to the UIM.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:

SIDE EFFECTS
  None
===========================================================================*/
gsdi_returns_T gsdi_proc_chnl_mgt_get_atr(
    gsdi_get_atr_req_T *  req_p,
    int32 *               returned_data_len_p,
    byte *                data_p
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  MMGSDI_RETURN_IF_NULL(req_p);
  MMGSDI_RETURN_IF_NULL(returned_data_len_p);
  MMGSDI_RETURN_IF_NULL(data_p);

  switch ( req_p->message_header.slot )
  {
  case GSDI_SLOT_1:
    *returned_data_len_p = gsdi_data.card_atr.atr_len;
    memcpy(data_p,
           gsdi_data.card_atr.atr_buffer,
           int32touint32(gsdi_data.card_atr.atr_len));
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case GSDI_SLOT_2:
    *returned_data_len_p = gsdi_data.card_atr_2.atr_len;
    memcpy(data_p,
           gsdi_data.card_atr_2.atr_buffer,
           gsdi_data.card_atr_2.atr_len);
    break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_p->message_header.slot,0,0);
    gsdi_status = GSDI_ERROR;
    break;
  }

  return gsdi_status;
}
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

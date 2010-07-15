/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   T O O L K I T   L I B R A R Y

GENERAL DESCRIPTION
  This module contains functions that process proactive commands and TLV
  buffers.

EXTERNALIZED FUNCTIONS

  uim_tk_parse_ber_tlv
    Parse a BER-TLV to get the tag and verify the length.

  uim_tk_pack_ber_tlv
    Formats a BER-TLV.  It adds a tag and the length to a VALUE field that
    has already been filled.

  uim_tk_parse_simple_tlv_from_ber_tlv
    Parses a simple TLV from a BER-TLV.  Uses the offset (of the VALUE field)
    to determine where the simple TLV resides.  The simple TLV is placed in a
    seperate (parsed) buffer.

  uim_tk_pack_simple_tlv_in_ber_tlv
    Places a simple TLV into a BER-TLV (VALUE field).  Returns the size of the
    simple TLV.

  uim_tk_parse_simple_tlv
    Parses a simple TLV from a buffer of simple TLVs.  The simple TLV is placed
    in a seperate (parse) buffer.

  uim_tk_pack_simple_tlv
    Packs a simple TLV into a buffer of simple TLVs.  Returns the size of the
    simple TLV.

  uim_tk_process_proactive_command
    Processes a proactive command received from the UIM card.

Copyright (c) 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimtk.c_v   1.4   01 May 2002 15:15:58   tsummers  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimtk.c#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/05   pv      Lint Fixes
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/17/04   ck      Made necessary changes reflect polling related definitions
05/21/03   ck      Fixed the featurization around ui_get_cmd that was
                   breaking compilation in MSM6100
05/05/03   wli     Moved ui_cmd_free_q under FEATURE_REX_DYNA_MEM_UI
05/05/03   wli     Added FEATURE_REX_DYNA_MEM_UI to ui command.
04/22/03   wli     Added GSTK feature.
02/13/03   wli     Changed feature TOOKIT TO TOOKIT_UTK.
12/20/02   ts      Fixed Lint issues
05/01/02   ts      Needed to fill out the Terminal Response length in the
                   R-UIM command for Poll Interval, More Time and Polling Off.
04/03/02   ts      Added processing for Poll Interval, More Time and Polling
                   Off.
01/09/02   ts      Include comdef.h to make compiler happy when feature is
                   disabled.
                   Fixed the ERR text for no messages on uim queue.
12/14/01   ck      Fixed some compile warnings where bytes were being assigned
                   to enum types.
10/16/01   ts      Created file.  Processes toolkit interfaces - proactive commands
                   and TLVs.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"

/*lint -esym(766,..\..\services\utils\comdef.h)
  comdef.h was previously included to get rid of compiler warning about
  no external translation unit.  The best thing to do would be
  to not compile uimcdma.c based on a rule..... but till then.....
  let us calm down the lint
*/
#if defined FEATURE_UIM_TOOLKIT_UTK && !defined FEATURE_GSTK
#include "uimtk.h"
#include "uimi.h"
#include "ui.h"
#include "msg.h"
#include "err.h"
#include "memory.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define UIM_TK_TAG_CR              0x80
#define UIM_TK_TAG_MASK            0x7F
#define UIM_TK_MULTI_BYTE_LENGTH   0x80
#define UIM_TK_TWO_BYTE_LENGTH     0x81
#define UIM_TK_ONE_BYTE_MAX_LENGTH 127
#define UIM_TK_VALUE_OFFSET        3

#define UIM_TK_COMMAND_DETAILS_SIZE 3
#define UIM_TK_DEVICE_ID_SIZE       2
#define UIM_TK_DURATION_SIZE        2
#define UIM_TK_TONE_SIZE            1
#define UIM_TK_ITEM_ID_SIZE         1
#define UIM_TK_RSP_LEN_SIZE         2
#ifndef CUST_EDITION
#define UIM_TK_LOCN_INFO_SIZE       7
#else
#define UIM_TK_LOCN_INFO_SIZE       15
#endif
#define UIM_TK_HELP_REQ_SIZE        0
#define UIM_TK_NW_MEAS_RESULTS_SIZE 16
#define UIM_TK_ICON_ID_SIZE         2
#define UIM_TK_IMMEDIATE_RSP_SIZE   0
#define UIM_TK_LANG_SIZE            2
#ifdef CUST_EDITION
#define UIM_TK_DATE_TIME_SIZE       7
#define UIM_TK_ACCESS_TECH_SIZE     1
#endif
/* These constants define the bit masks used in the indicator variables */
#define UIM_TK_COMMAND_DETAILS_BIT_IND       0x00000001
#define UIM_TK_DEVICE_ID_BIT_IND             0x00000002
#define UIM_TK_RESULT_BIT_IND                0x00000004
#define UIM_TK_DURATION_BIT_IND              0x00000008
#define UIM_TK_ALPHA_ID_BIT_IND              0x00000010
#define UIM_TK_ADDRESS_BIT_IND               0x00000020
#define UIM_TK_CAP_CONFIG_BIT_IND            0x00000040
#define UIM_TK_SUBADDRESS_BIT_IND            0x00000080
#define UIM_TK_CDMA_SMS_TPDU_BIT_IND         0x00000100
#define UIM_TK_TEXT_STRING_BIT_IND           0x00000200
#define UIM_TK_TONE_BIT_IND                  0x00000400
#define UIM_TK_ITEM_BIT_IND                  0x00000800
#define UIM_TK_ITEM_ID_BIT_IND               0x00001000
#define UIM_TK_RSP_LEN_BIT_IND               0x00002000
#define UIM_TK_FILE_LIST_BIT_IND             0x00004000
#define UIM_TK_LOCN_INFO_BIT_IND             0x00008000
#define UIM_TK_IMEI_BIT_IND                  0x00010000
#define UIM_TK_HELP_REQ_BIT_IND              0x00020000
#define UIM_TK_NW_MEAS_RESULTS_BIT_IND       0x00040000
#define UIM_TK_DEFAULT_TEXT_BIT_IND          0x00080000
#define UIM_TK_ITEMS_NEXT_ACTION_IND_BIT_IND 0x00100000
#define UIM_TK_ICON_ID_BIT_IND               0x00200000
#define UIM_TK_ICON_ID_LIST_BIT_IND          0x00400000
#define UIM_TK_IMMEDIATE_RSP_BIT_IND         0x00800000
#define UIM_TK_LANG_BIT_IND                  0x01000000

/* These constants identify the minimum set of simple TLVs for each command */
#define UIM_TK_REFRESH_MIN_TLVS (  \
  UIM_TK_COMMAND_DETAILS_BIT_IND | \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_MORE_TIME_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |  \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_POLL_INTERVAL_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |      \
  UIM_TK_DEVICE_ID_BIT_IND       |      \
  UIM_TK_DURATION_BIT_IND )

#define UIM_TK_POLLING_OFF_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |    \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_SETUP_CALL_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |   \
  UIM_TK_DEVICE_ID_BIT_IND       |   \
  UIM_TK_ADDRESS_BIT_IND )

#define UIM_TK_CDMA_SEND_SHORT_MSG_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |            \
  UIM_TK_DEVICE_ID_BIT_IND       |            \
  UIM_TK_CDMA_SMS_TPDU_BIT_IND )

#define UIM_TK_PLAY_TONE_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |  \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_DISPLAY_TEXT_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |     \
  UIM_TK_DEVICE_ID_BIT_IND       |     \
  UIM_TK_TEXT_STRING_BIT_IND )

#define UIM_TK_GET_INPUT_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |  \
  UIM_TK_DEVICE_ID_BIT_IND       |  \
  UIM_TK_TEXT_STRING_BIT_IND     |  \
  UIM_TK_RSP_LEN_BIT_IND )

#define UIM_TK_SELECT_ITEM_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |    \
  UIM_TK_DEVICE_ID_BIT_IND       |    \
  UIM_TK_ITEM_BIT_IND )

#define UIM_TK_SETUP_MENU_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |   \
  UIM_TK_DEVICE_ID_BIT_IND       |   \
  UIM_TK_ALPHA_ID_BIT_IND        |   \
  UIM_TK_ITEM_BIT_IND )

#define UIM_TK_PROVIDE_LOCAL_INFO_MIN_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |           \
  UIM_TK_DEVICE_ID_BIT_IND )

/* These constants identify the allowed set of simple TLVs for each command */
#define UIM_TK_REFRESH_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |    \
  UIM_TK_DEVICE_ID_BIT_IND       |    \
  UIM_TK_FILE_LIST_BIT_IND )

#define UIM_TK_MORE_TIME_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |      \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_POLL_INTERVAL_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |          \
  UIM_TK_DEVICE_ID_BIT_IND       |          \
  UIM_TK_DURATION_BIT_IND )

#define UIM_TK_POLLING_OFF_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |        \
  UIM_TK_DEVICE_ID_BIT_IND )

#define UIM_TK_SETUP_CALL_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |       \
  UIM_TK_DEVICE_ID_BIT_IND       |       \
  UIM_TK_ALPHA_ID_BIT_IND        |       \
  UIM_TK_ADDRESS_BIT_IND         |       \
  UIM_TK_CAP_CONFIG_BIT_IND      |       \
  UIM_TK_SUBADDRESS_BIT_IND      |       \
  UIM_TK_DURATION_BIT_IND        |       \
  UIM_TK_ICON_ID_BIT_IND )

#define UIM_TK_CDMA_SEND_SHORT_MSG_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |                \
  UIM_TK_DEVICE_ID_BIT_IND       |                \
  UIM_TK_ALPHA_ID_BIT_IND        |                \
  UIM_TK_ADDRESS_BIT_IND         |                \
  UIM_TK_CDMA_SMS_TPDU_BIT_IND   |                \
  UIM_TK_ICON_ID_BIT_IND )

#define UIM_TK_PLAY_TONE_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |      \
  UIM_TK_DEVICE_ID_BIT_IND       |      \
  UIM_TK_ALPHA_ID_BIT_IND        |      \
  UIM_TK_TONE_BIT_IND            |      \
  UIM_TK_DURATION_BIT_IND )

#define UIM_TK_DISPLAY_TEXT_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |         \
  UIM_TK_DEVICE_ID_BIT_IND       |         \
  UIM_TK_TEXT_STRING_BIT_IND     |         \
  UIM_TK_ICON_ID_BIT_IND         |         \
  UIM_TK_IMMEDIATE_RSP_BIT_IND )

#define UIM_TK_GET_INPUT_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |      \
  UIM_TK_DEVICE_ID_BIT_IND       |      \
  UIM_TK_TEXT_STRING_BIT_IND     |      \
  UIM_TK_RSP_LEN_BIT_IND         |      \
  UIM_TK_DEFAULT_TEXT_BIT_IND    |      \
  UIM_TK_ICON_ID_BIT_IND )

#define UIM_TK_SELECT_ITEM_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND       |  \
  UIM_TK_DEVICE_ID_BIT_IND             |  \
  UIM_TK_ALPHA_ID_BIT_IND              |  \
  UIM_TK_ITEM_BIT_IND                  |  \
  UIM_TK_ITEMS_NEXT_ACTION_IND_BIT_IND |  \
  UIM_TK_ITEM_ID_BIT_IND               |  \
  UIM_TK_ICON_ID_BIT_IND               |  \
  UIM_TK_ICON_ID_LIST_BIT_IND )

#define UIM_TK_SETUP_MENU_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND       | \
  UIM_TK_DEVICE_ID_BIT_IND             | \
  UIM_TK_ALPHA_ID_BIT_IND              | \
  UIM_TK_ITEM_BIT_IND                  | \
  UIM_TK_ITEMS_NEXT_ACTION_IND_BIT_IND | \
  UIM_TK_ICON_ID_BIT_IND               | \
  UIM_TK_ICON_ID_LIST_BIT_IND )

#define UIM_TK_PROVIDE_LOCAL_INFO_ALLOWED_TLVS ( \
  UIM_TK_COMMAND_DETAILS_BIT_IND |               \
  UIM_TK_DEVICE_ID_BIT_IND )

/* <EJECT> */
/*===========================================================================

MACRO UIMTK_PROCESS_TLV_BIT_IND

DESCRIPTION
  This macro sets the appropriate bits that indicate which simple TLV is in the
  BER-TLV and which requires comprehension.

PARAMETERS
  The bit indicator for the simple TLV.

DEPENDENCIES
  This macro works in conjunction with the bit indicator variables that
  indicate which simple TLV has been processed and which requires
  comprehension.

RETURN VALUE
  None

SIDE EFFECTS
  The 'processed' and 'comprehension required' bit indicator variables are
  set accordingly.
  A 'continue' instruction is executed if the simple TLV has already been
  processed.

===========================================================================*/
#define UIMTK_PROCESS_TLV_BIT_IND(simple_tlv_bit_ind)                  \
  /* Determine if this type of TLV has already been processed */       \
  if ((simple_tlv_bit_ind & processed_bit_ind) ==                      \
         simple_tlv_bit_ind)                                           \
  {                                                                    \
    /* Skip this TLV as it has already been processed */               \
    continue;                                                          \
  } /* end if - this type of TLV has already been processed */         \
  /* Include the TLV indicator to the list of processed indicators */  \
  processed_bit_ind |= simple_tlv_bit_ind;                             \
  /* Determine if comprehension is required for this TLV */            \
  if (parsed_tlv_buf.hdr.comprehension_required)                       \
  {                                                                    \
    /* Include this TLV to the comprehension required list */          \
    comprehension_required_bit_ind |= simple_tlv_bit_ind;              \
  } /* end if - comprehension is required */


/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PARSE_BER_TLV

DESCRIPTION
  This procedure parses a BER-TLV.  The function compares the amount of
  data in the buffer to the length in the BER-TLV.

DEPENDENCIES
  The function needs the size of the raw data to determine if the length
  in the TLV matches the amount of data received.

RETURN VALUE
  This function returns the status of the parse and the tag value
  of the BER_TLV.

SIDE EFFECTS
  None

===========================================================================*/
uim_tk_parse_ber_tlv_return_type uim_tk_parse_ber_tlv
(
  byte  *tlv_buff,
  word  total_size
)
{
  /* Holds the result of the parse */
  uim_tk_parse_ber_tlv_return_type return_buf;
  /* Holds the length of the BER-TLV. */
  byte tlv_size;
  /* Holds the size of the length field */
  byte length_size;

  /* Retrieve the tag field */
  return_buf.tag = (uim_tk_tag_enum_type) tlv_buff[0];
//  return_buf.comprehension_required =
//    tlv_buff[0] & UIM_TK_TAG_CR;

  /* Check if the tag is valid */
  switch (return_buf.tag)
  {
    case UIM_TK_COMMAND_DETAILS_TAG:
    case UIM_TK_DEVICE_ID_TAG:
    case UIM_TK_RESULT_TAG:
    case UIM_TK_DURATION_TAG:
    case UIM_TK_ALPHA_ID_TAG:
    case UIM_TK_ADDRESS_TAG:
    case UIM_TK_CAP_CONFIG_TAG:
    case UIM_TK_SUBADDRESS_TAG:
    case UIM_TK_CDMA_SMS_TPDU_TAG:
    case UIM_TK_TEXT_STRING_TAG:
    case UIM_TK_TONE_TAG:
    case UIM_TK_ITEM_TAG:
    case UIM_TK_ITEM_ID_TAG:
    case UIM_TK_RSP_LEN_TAG:
    case UIM_TK_FILE_LIST_TAG:
    case UIM_TK_LOCN_INFO_TAG:
    case UIM_TK_IMEI_TAG:
    case UIM_TK_HELP_REQ_TAG:
    case UIM_TK_NW_MEAS_RESULTS_TAG:
    case UIM_TK_DEFAULT_TEXT_TAG:
    case UIM_TK_ITEMS_NEXT_ACTION_IND_TAG:
    case UIM_TK_ICON_ID_TAG:
    case UIM_TK_ICON_ID_LIST_TAG:
    case UIM_TK_IMMEDIATE_RSP_TAG:
    case UIM_TK_LANG_TAG:
#ifdef CUST_EDITION	
    case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:    
    case UIM_TK_ACCESS_TECHNOLOGY_TAG:
#endif	
      {
        /* Indicate the message is not expected - not a BER-TLV
           Check the length as well.  A bad length has presidence
           over an unexpected tag. */
        return_buf.status = UIM_TK_UNEXPECTED_TAG;
      }
      break;

    /* Tag values for the BER tags */
    case  UIM_TK_PROACTIVE_SIM_COMMAND_TAG:
    case  UIM_TK_CDMA_SMS_PP_DOWNLOAD_TAG:
    case  UIM_TK_MENU_SELECTION_TAG:
      {
        /* Set the status to a valid status */
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case */
      break;

    default:
      {
        /* Indicate an undefined tag value.  Check the length as well.
           A bad length has presidence over an undefined tag value. */
        return_buf.status = UIM_TK_UNKNOWN_TAG;
      }
  } /* end switch - tag field */

  /* Retrieve the length from the BER-TLV */
  if (tlv_buff[1] & UIM_TK_MULTI_BYTE_LENGTH)
  {
    /* Check for a two byte length */
    if (tlv_buff[1] == UIM_TK_TWO_BYTE_LENGTH)
    {
      /* The size is in the second byte */
      tlv_size = tlv_buff[2];
      length_size = 2;
    }
    else /* This type of length is not supported */
    {
      /* Indicate a bad length field */
      return_buf.status = UIM_TL_LENGTH_ERROR;
      return (return_buf);
    } /* end if - a two byte length */
  }
  else /* The length is one byte */
  {
    /* The size is the first byte */
    tlv_size = tlv_buff[1];
    length_size = 1;
  } /* end if - multi-byte length */

  /* Compare the tlv length to size of the tlv buffer
     The TLV size includes the size of the length field
     and the size of the tag field. */
  if (total_size != (tlv_size + length_size + 1))
  {
    /* Indicate a length error with the TLV */
    return_buf.status = UIM_TL_LENGTH_ERROR;
  } /* end if - the TLV size does not match the buffer size */

  return( return_buf );
} /* uim_tk_parse_ber_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_BER_TLV

DESCRIPTION
  This procedure formats a BER-TLV.  The function adds a tag and length
  for a BER-TLV that has already had the value field filled out.  Since the
  length field cannot be determined beforehand, this function returns an
  offset that indicates the number of bytes (from the beginning of the
  buffer) the BER-TLV actually starts.

DEPENDENCIES
  The function needs the size of the Value field to determine the length
  of the TLV.  Also, the Value field should already been filled out with
  simple TLVs.

  This function assumes the BER-TLV is 255 bytes (total) in size.

RETURN VALUE
  This function returns the status of the pack and the tag value
  of the BER_TLV.

SIDE EFFECTS
  None

===========================================================================*/
uim_tk_pack_ber_tlv_return_type uim_tk_pack_ber_tlv
(
  byte                 *tlv_buff,  /* BER-TLV raw data buffer */
  uim_tk_tag_enum_type tag,        /* BER-TLV tag */
  byte                 value_size  /* Size of the Value field */
)
{
  /* This holds the result of the pack */
  uim_tk_pack_ber_tlv_return_type return_buf;

  /* Check if the tag is valid */
  switch (tag)
  {
    case UIM_TK_COMMAND_DETAILS_TAG:
    case UIM_TK_DEVICE_ID_TAG:
    case UIM_TK_RESULT_TAG:
    case UIM_TK_DURATION_TAG:
    case UIM_TK_ALPHA_ID_TAG:
    case UIM_TK_ADDRESS_TAG:
    case UIM_TK_CAP_CONFIG_TAG:
    case UIM_TK_SUBADDRESS_TAG:
    case UIM_TK_CDMA_SMS_TPDU_TAG:
    case UIM_TK_TEXT_STRING_TAG:
    case UIM_TK_TONE_TAG:
    case UIM_TK_ITEM_TAG:
    case UIM_TK_ITEM_ID_TAG:
    case UIM_TK_RSP_LEN_TAG:
    case UIM_TK_FILE_LIST_TAG:
    case UIM_TK_LOCN_INFO_TAG:
    case UIM_TK_IMEI_TAG:
    case UIM_TK_HELP_REQ_TAG:
    case UIM_TK_NW_MEAS_RESULTS_TAG:
    case UIM_TK_DEFAULT_TEXT_TAG:
    case UIM_TK_ITEMS_NEXT_ACTION_IND_TAG:
    case UIM_TK_ICON_ID_TAG:
    case UIM_TK_ICON_ID_LIST_TAG:
    case UIM_TK_IMMEDIATE_RSP_TAG:
    case UIM_TK_LANG_TAG:
#ifdef CUST_EDITION
    case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:    
    case UIM_TK_ACCESS_TECHNOLOGY_TAG: 
#endif	
      {
        /* Indicate the message is not expected - not a BER-TLV
           Check the length as well.  A bad length has presidence
           over an unexpected tag. */
        return_buf.status = UIM_TK_UNEXPECTED_TAG;
      }
      break;

    /* Tag values for the BER tags */
    case  UIM_TK_PROACTIVE_SIM_COMMAND_TAG:
    case  UIM_TK_CDMA_SMS_PP_DOWNLOAD_TAG:
    case  UIM_TK_MENU_SELECTION_TAG:
      {
        /* Set the status to a valid status */
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case */
      break;

    default:
      {
        /* Indicate an undefined tag value.  Check the length as well.
           A bad length has presidence over an undefined tag value. */
        return_buf.status = UIM_TK_UNKNOWN_TAG;
      }
  } /* end switch - tag field */

  /* The VALUE size is placed in third byte of the buffer regardless
     of the size of the VALUE field.  The VALUE size goes in this
     position whether indicated in one byte or two. */
  tlv_buff[2] = value_size;

  /* Determine if the VALUE size can be handled in one byte */
  if (value_size <= UIM_TK_ONE_BYTE_MAX_LENGTH)
  {
    /* Place the tag in the second byte of the buffer. */
    tlv_buff[1] = (byte)tag;

    /* Update value size to include the tag and length field size */
    return_buf.total_size = value_size + 2;

    /* The BER-TLV offset is one */
    return_buf.tlv_offset = 1;
  }
  else /* The size of the VALUE field needs 2 bytes */
  {
    /* Place the tag in the first byte of the TLV buffer. */
    tlv_buff[0] = (byte)tag;

    /* Place the first byte of the length in the second byte */
    tlv_buff[1] = UIM_TK_TWO_BYTE_LENGTH;

    /* Update value size to include the tag and length field size */
    return_buf.total_size = value_size + 3;

    /* The BER-TLV offset is zero */
    return_buf.tlv_offset = 0;
  } /* end if - the size fits in one byte */

  return( return_buf );
} /* uim_tk_pack_ber_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PARSE_SIMPLE_TLV_FROM_BER_TLV

DESCRIPTION
  This procedure parses a simple TLV from a BER-TLV.  The function looks for
  valid tag and length values.

DEPENDENCIES
  The function needs the address of the BER-TLV and the byte offset (in the
  Value field) of the start of the simple TLV.
  The offset is used to determine where to parse a simple TLV from the Value
  field of the BER TLV.
                            -----
                            | T |  BER TLV buffer
                            | L |
                            |   |
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            -----


RETURN VALUE
  This function returns the status of the parse and the length of the simple
  TLV.

SIDE EFFECTS
  This function builds a parsed simple TLV in the buffer pointed to by
  parsed_tlv_buff.

===========================================================================*/
uim_tk_proc_simple_tlv_return_type uim_tk_parse_simple_tlv_from_ber_tlv
(
  byte*            tlv_buff,       /* BER_TLV buffer pointer */
  byte             offset,         /* byte offset of simple tlv */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed tlv buffer */
)
{
  /* This holds the result of the parse */
  uim_tk_proc_simple_tlv_return_type return_buf;
  /* This holds the size of the BER_TLV */
  byte ber_tlv_size;
  /* This holds the size of the length field */
  byte length_size;

  /* Retrieve the length from the BER-TLV */
  if (tlv_buff[1] & UIM_TK_MULTI_BYTE_LENGTH)
  {
    /* Check for a two byte length */
    if (tlv_buff[1] == UIM_TK_TWO_BYTE_LENGTH)
    {
      /* The size is in the second byte */
      ber_tlv_size = tlv_buff[2];
      length_size = 2;
    }
    else /* This type of length is not supported */
    {
      /* Indicate a bad length field */
      return_buf.status = UIM_TL_LENGTH_ERROR;
      return_buf.tlv_size = 0;
      return (return_buf);
    } /* end if - a two byte length */
  }
  else /* The length is one byte */
  {
    /* The size is the first byte */
    ber_tlv_size = tlv_buff[1];
    length_size = 1;
  } /* end if - multi-byte length */

  /* Check if the offset is within the VALUE field.  Allow for the tag,
     length and value fields of the simple TLV. */
  if ((offset + 3) <= ber_tlv_size)
  {
    /* Parse the simple tlv */
    return_buf = uim_tk_parse_simple_tlv ((tlv_buff + length_size + 1),
                                          offset,
                                          parsed_tlv_buff);
    /* Determine if this is the last simple TLV in this BER-TLV */
    if ((return_buf.tlv_size + offset) == ber_tlv_size)
    {
      /* Indicate this is the last simple TLV in this BER-TLV */
      return_buf.status = UIM_TK_MSG_VALID_AND_LAST;
    }
  }
  else /* The offset is outside the BER-TLV buffer */
  {
    /* Indicate the offset is outside the BER-TLV */
    return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
    return_buf.tlv_size = 0;
  } /* end if - the offset is within the BER-TLV buffer */

  return ( return_buf );
} /* uim_tk_parse_simple_tlv_from_ber_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_SIMPLE_TLV_IN_BER_TLV

DESCRIPTION
  This procedure packs a simple TLV into a BER-TLV Value field.  The function
  checks for valid tags and lengths of the simple TLV.  It also ensures the
  simple TLV does not overwrite the BER-TLV (assumed to be 255 bytes).
  length values.

DEPENDENCIES
  The function needs the address of the BER-TLV and the byte offset (in the
  Value field) of the start of the simple TLV.
  The offset is used to determine where to pack a simple TLV into the Value
  field of the BER TLV.
                            -----
                            | T |  BER TLV buffer
                            | L |
                            |   |
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the pack and the length of the simple
  TLV.

SIDE EFFECTS
  This function fills in the simple tlv into the Value field of a BER TLV.
  The simple TLV is placed at the byte offset of the Value field of the BER
  TLV.

===========================================================================*/
uim_tk_proc_simple_tlv_return_type uim_tk_pack_simple_tlv_in_ber_tlv
(
  byte*            tlv_buff,       /* BER_TLV buffer pointer */
  byte             offset,         /* byte offset of simple tlv */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed tlv buffer */
)
{
  /* This holds the result of the parse */
  uim_tk_proc_simple_tlv_return_type return_buf;

  /* Pack the simple tlv */
  return_buf = uim_tk_pack_simple_tlv ((tlv_buff + UIM_TK_VALUE_OFFSET),
                                        offset,
                                        parsed_tlv_buff);
  return ( return_buf );
} /* uim_tk_pack_simple_tlv_in_ber_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PARSE_SIMPLE_TLV

DESCRIPTION
  This procedure parses a simple TLV.  The function looks for valid tag and
  length values.

DEPENDENCIES
  The function needs the address of the TLV buffer and the byte offset of the
  start of the simple TLV.
  The offset is used to determine where to parse a simple TLV from a buffer of
  a list of simple TLVs.
                            -----
                            |TLV|  Simple TLV within buffer (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the parse and the length of the simple
  TLV.

SIDE EFFECTS
  This function builds a parsed simple TLV in the buffer pointed to by
  parsed_tlv_buff.

===========================================================================*/
uim_tk_proc_simple_tlv_return_type uim_tk_parse_simple_tlv
(
  byte*            tlv_buff,       /* Simple TLV list buffer pointer */
  byte             offset,         /* byte offset of simple tlv within buffer */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed simple tlv buffer */
)
{
  /* This holds the result of the parse */
  uim_tk_proc_simple_tlv_return_type return_buf;
  /* This holds the size of the length field */
  byte length_size;

  /* Retrieve the length of the simple TLV */
  if (tlv_buff[offset + 1] & UIM_TK_MULTI_BYTE_LENGTH)
  {
    /* Check for a two byte length */
    if (tlv_buff[offset + 1] == UIM_TK_TWO_BYTE_LENGTH)
    {
      /* The size is in the second byte */
      return_buf.tlv_size = tlv_buff[offset + 2];
      length_size = 2;
    }
    else /* This type of length is not supported */
    {
      /* Indicate a bad length field */
      return_buf.status = UIM_TL_LENGTH_ERROR;
      return_buf.tlv_size = 0;
      return (return_buf);
    } /* end if - a two byte length */
  }
  else /* The length is one byte */
  {
    /* The size is the first byte */
    return_buf.tlv_size = tlv_buff[offset + 1];
    length_size = 1;
  } /* end if - multi-byte length */

  /* Get the tag from the TLV */
  parsed_tlv_buff->hdr.tag = (uim_tk_tag_enum_type)
                                      (tlv_buff[offset] & UIM_TK_TAG_MASK);
  parsed_tlv_buff->hdr.comprehension_required =
    tlv_buff[offset] & UIM_TK_TAG_CR;

  /* Parse the value field based on the tag */
  switch (parsed_tlv_buff->hdr.tag)
  {
    case UIM_TK_COMMAND_DETAILS_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_COMMAND_DETAILS_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->cmd_details.cmd_num  = tlv_buff[offset + 2];
          parsed_tlv_buff->cmd_details.cmd_type =
                 (uim_tk_proactive_cmd_enum_type) tlv_buff[offset + 3];
          parsed_tlv_buff->cmd_details.cmd_qual = tlv_buff[offset + 4];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_COMMAND_DETAILS_TAG */
      break;

    case UIM_TK_DEVICE_ID_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_DEVICE_ID_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->device_id.source_dev_id  =
                           (uim_tk_dev_id_type) tlv_buff[offset + 2];
          parsed_tlv_buff->device_id.dest_dev_id =
                           (uim_tk_dev_id_type) tlv_buff[offset + 3];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_DEVICE_ID_TAG */
      break;

    case UIM_TK_RESULT_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->result.result =
               (uim_tk_general_result_type) tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->result.num_bytes = return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->result.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->result.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_RESULT_TAG */
      break;

    case UIM_TK_DURATION_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_DURATION_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->duration.time_unit =
            tlv_buff[offset + 2];
          parsed_tlv_buff->duration.time_interval =
            tlv_buff[offset + 3];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_DURATION_TAG */
      break;

    case UIM_TK_ALPHA_ID_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->alpha_id.num_bytes =
          return_buf.tlv_size;
        memcpy (parsed_tlv_buff->alpha_id.data,
                &tlv_buff[offset + length_size + 1],
                parsed_tlv_buff->alpha_id.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_ALPHA_ID_TAG */
      break;

    case UIM_TK_ADDRESS_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->addr.ton_npi =
          tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->addr.num_bytes = return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->addr.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->addr.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_ADDRESS_TAG */
      break;

    case UIM_TK_CAP_CONFIG_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->cap_cfg.num_bytes = return_buf.tlv_size;
        memcpy (parsed_tlv_buff->cap_cfg.data,
                &tlv_buff[offset + length_size + 1],
                parsed_tlv_buff->cap_cfg.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_CAP_CONFIG_TAG */
      break;

    case UIM_TK_SUBADDRESS_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->subaddr.num_bytes = return_buf.tlv_size;
        memcpy (parsed_tlv_buff->subaddr.data,
                &tlv_buff[offset + length_size + 1],
                parsed_tlv_buff->subaddr.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_SUBADDRESS_TAG */
      break;

    case UIM_TK_CDMA_SMS_TPDU_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->cdma_sms.num_bytes = return_buf.tlv_size;
        memcpy (parsed_tlv_buff->cdma_sms.data,
                &tlv_buff[offset + length_size + 1],
                parsed_tlv_buff->cdma_sms.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_CDMA_SMS_TPDU_TAG */
      break;

    case UIM_TK_TEXT_STRING_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->txt.dcs = tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->txt.num_bytes = return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->txt.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->txt.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_TEXT_STRING_TAG */
      break;

    case UIM_TK_TONE_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_TONE_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->tone.tone  =
            tlv_buff[offset + 2];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_TONE_TAG */
      break;

    case UIM_TK_ITEM_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->item.item_id =
          tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->item.num_bytes = return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->item.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->item.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_ITEM_TAG */
      break;

    case UIM_TK_ITEM_ID_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_ITEM_ID_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->item_id.id  =
            tlv_buff[offset + 2];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_ITEM_ID_TAG */
      break;

    case UIM_TK_RSP_LEN_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_RSP_LEN_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->rsp_len.min_len_rsp  =
            tlv_buff[offset + 2];
          parsed_tlv_buff->rsp_len.max_len_rsp =
            tlv_buff[offset + 3];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_RSP_LEN_TAG */
      break;

    case UIM_TK_FILE_LIST_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->file_list.num_files =
          tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->file_list.num_bytes = return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->file_list.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->file_list.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_FILE_LIST_TAG */
      break;

#ifndef CUST_EDITION
    case UIM_TK_LOCN_INFO_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_LOCN_INFO_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->locn_info.mcc_mnc  =
            (dword)(tlv_buff[offset + 2] +
                   (tlv_buff[offset + 3] << 8) +
                   (tlv_buff[offset + 4] << 16));
          parsed_tlv_buff->locn_info.lac =
            (word)(tlv_buff[offset + 5] +
                   (tlv_buff[offset + 6] << 8));
          parsed_tlv_buff->locn_info.lac =
            (word)(tlv_buff[offset + 7] +
                   (tlv_buff[offset + 8] << 8));
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_LOCN_INFO_TAG */
      break;
#else
   case UIM_TK_LOCN_INFO_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_LOCN_INFO_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->locn_info.mcc = 
                (((tlv_buff[offset + 2] & 0x0f)*1000)+ 
                (((tlv_buff[offset + 2] >> 4)& 0x0f)*100) +
                ((tlv_buff[offset + 3]& 0x0f)*10) +
                ((tlv_buff[offset + 3] >> 4)& 0x0f));
                                
          parsed_tlv_buff->locn_info.imsi_11_12 = 
                (((tlv_buff[offset + 4]& 0x0f)*10) +
                ((tlv_buff[offset + 4] >> 4)& 0x0f));  
                
          parsed_tlv_buff->locn_info.sid = 
                (((tlv_buff[offset + 5] & 0x0f)*1000)+ 
                (((tlv_buff[offset + 5] >> 4)& 0x0f)*100) +
                ((tlv_buff[offset + 6]& 0x0f)*10) +
                ((tlv_buff[offset + 6] >> 4)& 0x0f));
                
          parsed_tlv_buff->locn_info.nid = 
                (((tlv_buff[offset + 7] & 0x0f)*1000)+ 
                (((tlv_buff[offset + 7] >> 4)& 0x0f)*100) +
                ((tlv_buff[offset + 8]& 0x0f)*10) +
                ((tlv_buff[offset + 8] >> 4)& 0x0f));
                
          parsed_tlv_buff->locn_info.base_id = 
                (((tlv_buff[offset + 9] & 0x0f)*1000)+ 
                (((tlv_buff[offset + 9] >> 4)& 0x0f)*100) +
                ((tlv_buff[offset + 10]& 0x0f)*10) +
                ((tlv_buff[offset + 10] >> 4)& 0x0f));
                
          parsed_tlv_buff->locn_info.base_lat = 
                (((tlv_buff[offset + 11] & 0x0f)*100000)+ 
                (((tlv_buff[offset + 11] >> 4)& 0x0f)*10000) +
                ((tlv_buff[offset + 12]& 0x0f)*1000) +
                (((tlv_buff[offset + 12] >> 4)& 0x0f)*100)+
                ((tlv_buff[offset + 13]& 0x0f)*10) +
                ((tlv_buff[offset + 13] >> 4)& 0x0f));    
                
          parsed_tlv_buff->locn_info.base_long = 
                (((tlv_buff[offset + 14] & 0x0f)*100000)+ 
                (((tlv_buff[offset + 14] >> 4)& 0x0f)*10000) +
                ((tlv_buff[offset + 15]& 0x0f)*1000) +
                (((tlv_buff[offset + 15] >> 4)& 0x0f)*100)+
                ((tlv_buff[offset + 16]& 0x0f)*10) +
                ((tlv_buff[offset + 16] >> 4)& 0x0f));            
                      
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_LOCN_INFO_TAG */
      break;
#endif //#if 0          

    case UIM_TK_IMEI_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_IMEI_SIZE)
        {
          /* Parse the Value field */
          memcpy( parsed_tlv_buff->iemi.imei,
                  &tlv_buff[offset + 2],
                  return_buf.tlv_size );
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_IMEI_TAG */
      break;

    case UIM_TK_HELP_REQ_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_HELP_REQ_SIZE)
        {
          /* There is no Value field */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_HELP_REQ_TAG */
      break;

    case UIM_TK_NW_MEAS_RESULTS_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_NW_MEAS_RESULTS_SIZE)
        {
          /* Parse the Value field */
          memcpy( parsed_tlv_buff->nw_meas_results.nw_meas_data,
                  &tlv_buff[offset + 2],
                  return_buf.tlv_size );
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_NW_MEAS_RESULTS_TAG */
      break;

    case UIM_TK_DEFAULT_TEXT_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->default_txt.dcs =
          tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->default_txt.num_bytes =
          return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->default_txt.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->default_txt.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_DEFAULT_TEXT_TAG */
      break;

    case UIM_TK_ITEMS_NEXT_ACTION_IND_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->next_action.num_bytes =
          return_buf.tlv_size;
        memcpy (parsed_tlv_buff->next_action.data,
                &tlv_buff[offset + length_size + 1],
                parsed_tlv_buff->next_action.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_ITEMS_NEXT_ACTION_IND_TAG */
      break;

    case UIM_TK_ICON_ID_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_ICON_ID_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->icon_id.icon_qual =
            tlv_buff[offset + 2];
          parsed_tlv_buff->icon_id.icon_id =
            tlv_buff[offset + 3];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_ICON_ID_TAG */
      break;

    case UIM_TK_ICON_ID_LIST_TAG:
      {
        /* Parse the Value field */
        parsed_tlv_buff->icon_id_list.icon_list_qual =
          tlv_buff[offset + length_size + 1];
        parsed_tlv_buff->icon_id_list.num_bytes =
          return_buf.tlv_size - 1;
        memcpy (parsed_tlv_buff->icon_id_list.data,
                &tlv_buff[offset + length_size + 2],
                parsed_tlv_buff->icon_id_list.num_bytes);
        return_buf.status = UIM_TK_MESSAGE_IS_VALID;
      } /* end case - UIM_TK_ICON_ID_LIST_TAG */
      break;

    case UIM_TK_IMMEDIATE_RSP_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_IMMEDIATE_RSP_SIZE)
        {
          /* There is no Value field */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_IMMEDIATE_RSP_TAG */
      break;

    case UIM_TK_LANG_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_LANG_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->lang.lang =
            (word)(tlv_buff[offset + 2] +
                   (tlv_buff[offset + 3] << 8));
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_LANG_TAG */
      break;
#ifdef CUST_EDITION
    case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_DATE_TIME_SIZE)
        {
           uint8           tmp;
           
           tmp = UTK_BCD2INT(tlv_buff[offset + 2]);
          /* Parse the Value field */
          parsed_tlv_buff->date_time.year = (tmp <= 95) ? (2000 + tmp) : (1900 + tmp);
          parsed_tlv_buff->date_time.month = UTK_BCD2INT(tlv_buff[offset + 3]);
          parsed_tlv_buff->date_time.day = UTK_BCD2INT(tlv_buff[offset + 4]);
          parsed_tlv_buff->date_time.hour = UTK_BCD2INT(tlv_buff[offset + 5]);
          parsed_tlv_buff->date_time.minute = UTK_BCD2INT(tlv_buff[offset + 6]);
          parsed_tlv_buff->date_time.second = UTK_BCD2INT(tlv_buff[offset + 7]);
          parsed_tlv_buff->date_time.zone = tlv_buff[offset + 8];                                                                      
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG */
      break;  
              
    case UIM_TK_ACCESS_TECHNOLOGY_TAG:
      {
        /* Make sure the length is valid */
        if (return_buf.tlv_size == UIM_TK_ACCESS_TECH_SIZE)
        {
          /* Parse the Value field */
          parsed_tlv_buff->access_technology.access_technology = tlv_buff[offset + 2];
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
        }
        else /* The size is not correct */
        {
          /* Indicate the problem in the return status */
          return_buf.status = UIM_TL_LENGTH_ERROR;
        } /* end if - the length is valid */
      } /* end case - UIM_TK_ACCESS_TECHNOLOGY_TAG */
      break;      

#endif
      /* Tag values for the BER tags */
    case  UIM_TK_PROACTIVE_SIM_COMMAND_TAG:
    case  UIM_TK_CDMA_SMS_PP_DOWNLOAD_TAG:
    case  UIM_TK_MENU_SELECTION_TAG:
      {
        /* Set the status to a valid status */
        return_buf.status = UIM_TK_UNEXPECTED_TAG;
      } /* end case */
      break;

    default:
      {
        /* Indicate an undefined tag value.  Check the length as well.
           A bad length has presidence over an undefined tag value. */
        return_buf.status = UIM_TK_UNKNOWN_TAG;
      }
  } /* end switch - tag field */

  /* Add the tag and length field sizes to the tlv length */
  return_buf.tlv_size += 1 + length_size;

  return return_buf;
} /* uim_tk_parse_simple_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_SIMPLE_TLV

DESCRIPTION
  This procedure packs a simple TLV into a simple buffer.  The function
  checks for valid tags and lengths of the simple TLV.  It also ensures the
  simple TLV does not overwrite the simple buffer (assumed to be 255 bytes).

DEPENDENCIES
  The function needs the address of the simple buffer and the byte offset
  of the start of the simple TLV.
  The offset is used to determine where to pack a simple TLV into a buffer
  of a list of simple TLVs.
                            -----
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the pack and the length of the simple
  TLV.

SIDE EFFECTS
  This function fills in the simple tlv into the Value field of a BER TLV.
  The simple TLV is placed at the byte offset of the Value field of the BER
  TLV.

===========================================================================*/
uim_tk_proc_simple_tlv_return_type uim_tk_pack_simple_tlv
(
  byte*            tlv_buff,       /* Simple TLV list buffer pointer */
  byte             offset,         /* byte offset of simple tlv within buffer */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed simple tlv buffer */
)
{
  /* This holds the result of the pack */
  uim_tk_proc_simple_tlv_return_type return_buf;
  /* This holds the size of the length field */
  byte length_size;
  /* This holds the tag field */
  byte local_tag;

  /* Build the tag field */
  local_tag = parsed_tlv_buff->hdr.tag;
  /* Determine if comprehension is required */
  if (parsed_tlv_buff->hdr.comprehension_required)
  {
    local_tag |= UIM_TK_TAG_CR;
  } /* end if - comprehension is required */

  /* Pack the value field based on the tag */
  switch (parsed_tlv_buff->hdr.tag)
  {
    case UIM_TK_COMMAND_DETAILS_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_COMMAND_DETAILS_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_COMMAND_DETAILS_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->cmd_details.cmd_num;
          tlv_buff[offset + 3] = parsed_tlv_buff->cmd_details.cmd_type;
          tlv_buff[offset + 4] = parsed_tlv_buff->cmd_details.cmd_qual;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_COMMAND_DETAILS_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_COMMAND_DETAILS_TAG */
      break;

    case UIM_TK_DEVICE_ID_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_DEVICE_ID_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_DEVICE_ID_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] =
            parsed_tlv_buff->device_id.source_dev_id;
          tlv_buff[offset + 3] =
            parsed_tlv_buff->device_id.dest_dev_id;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_DEVICE_ID_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_DEVICE_ID_TAG */
      break;

    case UIM_TK_RESULT_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->result.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->result.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->result.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->result.result;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->result.data,
                  parsed_tlv_buff->result.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->result.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_RESULT_TAG */
      break;

    case UIM_TK_DURATION_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_DURATION_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_DURATION_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->duration.time_unit;
          tlv_buff[offset + 3] = parsed_tlv_buff->duration.time_interval;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_DURATION_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_DURATION_TAG */
      break;

    case UIM_TK_ALPHA_ID_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->alpha_id.num_bytes)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->alpha_id.num_bytes +
             (word)length_size + 1) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->alpha_id.num_bytes;

          /* Pack the Value field */
          memcpy (&tlv_buff[offset + length_size + 1],
                  parsed_tlv_buff->alpha_id.data,
                  parsed_tlv_buff->alpha_id.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->alpha_id.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ALPHA_ID_TAG */
      break;

    case UIM_TK_ADDRESS_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->addr.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->addr.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->addr.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->addr.ton_npi;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->addr.data,
                  parsed_tlv_buff->addr.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->addr.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ADDRESS_TAG */
      break;

    case UIM_TK_CAP_CONFIG_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->cap_cfg.num_bytes)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->cap_cfg.num_bytes +
             (word)length_size + 1) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->cap_cfg.num_bytes;

          /* Pack the Value field */
          memcpy (&tlv_buff[offset + length_size + 1],
                  parsed_tlv_buff->cap_cfg.data,
                  parsed_tlv_buff->cap_cfg.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->cap_cfg.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_CAP_CONFIG_TAG */
      break;

    case UIM_TK_SUBADDRESS_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->subaddr.num_bytes)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->subaddr.num_bytes +
             (word)length_size + 1) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->subaddr.num_bytes;

          /* Pack the Value field */
          memcpy (&tlv_buff[offset + length_size + 1],
                  parsed_tlv_buff->subaddr.data,
                  parsed_tlv_buff->subaddr.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->subaddr.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_SUBADDRESS_TAG */
      break;

    case UIM_TK_CDMA_SMS_TPDU_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->cdma_sms.num_bytes)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->cdma_sms.num_bytes +
             (word)length_size + 1) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->cdma_sms.num_bytes;

          /* Pack the Value field */
          memcpy (&tlv_buff[offset + length_size + 1],
                  parsed_tlv_buff->cdma_sms.data,
                  parsed_tlv_buff->cdma_sms.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->cdma_sms.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_CDMA_SMS_TPDU_TAG */
      break;

    case UIM_TK_TEXT_STRING_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->txt.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->txt.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->txt.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] = parsed_tlv_buff->txt.dcs;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->txt.data,
                  parsed_tlv_buff->txt.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->txt.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_TEXT_STRING_TAG */
      break;

    case UIM_TK_TONE_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_TONE_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_TONE_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->tone.tone;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_TONE_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_TONE_TAG */
      break;

    case UIM_TK_ITEM_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->item.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->item.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->item.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->item.item_id;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->item.data,
                  parsed_tlv_buff->item.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->item.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ITEM_TAG */
      break;

    case UIM_TK_ITEM_ID_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_ITEM_ID_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_ITEM_ID_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->item_id.id;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_ITEM_ID_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ITEM_ID_TAG */
      break;

    case UIM_TK_RSP_LEN_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_RSP_LEN_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_RSP_LEN_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->rsp_len.min_len_rsp;
          tlv_buff[offset + 3] = parsed_tlv_buff->rsp_len.max_len_rsp;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_RSP_LEN_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_RSP_LEN_TAG */
      break;

    case UIM_TK_FILE_LIST_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->file_list.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->file_list.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->file_list.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->file_list.num_files;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->file_list.data,
                  parsed_tlv_buff->file_list.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->file_list.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_FILE_LIST_TAG */
      break;

#ifndef CUST_EDITION
    case UIM_TK_LOCN_INFO_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_LOCN_INFO_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_LOCN_INFO_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] =
            parsed_tlv_buff->locn_info.mcc_mnc & 0xff;
          tlv_buff[offset + 3] =
            (parsed_tlv_buff->locn_info.mcc_mnc << 8) & 0xff;
          tlv_buff[offset + 4] =
            (parsed_tlv_buff->locn_info.mcc_mnc << 16) & 0xff;
          tlv_buff[offset + 5] =
            parsed_tlv_buff->locn_info.lac & 0xff;
          tlv_buff[offset + 6] =
            (parsed_tlv_buff->locn_info.lac << 8) & 0xff;
          tlv_buff[offset + 7] =
            parsed_tlv_buff->locn_info.cell_id & 0xff;
          tlv_buff[offset + 8] =
            (parsed_tlv_buff->locn_info.cell_id << 8) & 0xff;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_LOCN_INFO_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_LOCN_INFO_TAG */
      break;
#else //#if 0
    case UIM_TK_LOCN_INFO_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_LOCN_INFO_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_LOCN_INFO_SIZE;
          
          /* Pack the Value field */          
          tlv_buff[offset + 2] = UTK_INT2BCD((parsed_tlv_buff->locn_info.mcc%10000)/100);
          tlv_buff[offset + 3] = UTK_INT2BCD(parsed_tlv_buff->locn_info.mcc%100);
          tlv_buff[offset + 4] = UTK_INT2BCD(parsed_tlv_buff->locn_info.imsi_11_12%100);
          tlv_buff[offset + 5] = UTK_INT2BCD((parsed_tlv_buff->locn_info.sid%10000)/100);
          tlv_buff[offset + 6] = UTK_INT2BCD(parsed_tlv_buff->locn_info.sid%100);
          tlv_buff[offset + 7] = UTK_INT2BCD((parsed_tlv_buff->locn_info.nid%10000)/100);
          tlv_buff[offset + 8] = UTK_INT2BCD(parsed_tlv_buff->locn_info.nid%100); 
          tlv_buff[offset + 9] = UTK_INT2BCD((parsed_tlv_buff->locn_info.base_id%10000)/100);
          tlv_buff[offset + 10] = UTK_INT2BCD(parsed_tlv_buff->locn_info.base_id%100);   
          tlv_buff[offset + 11] = UTK_INT2BCD((parsed_tlv_buff->locn_info.base_lat%1000000)/10000);
          tlv_buff[offset + 12] = UTK_INT2BCD((parsed_tlv_buff->locn_info.base_lat%10000)/100);  
          tlv_buff[offset + 13] = UTK_INT2BCD(parsed_tlv_buff->locn_info.base_lat%100);
          tlv_buff[offset + 14] = UTK_INT2BCD((parsed_tlv_buff->locn_info.base_long%1000000)/10000); 
          tlv_buff[offset + 15] = UTK_INT2BCD((parsed_tlv_buff->locn_info.base_long%10000)/100); 
          tlv_buff[offset + 16] = UTK_INT2BCD(parsed_tlv_buff->locn_info.base_long%100);         
                 
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_LOCN_INFO_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_LOCN_INFO_TAG */
      break;
#endif //#if 0

    case UIM_TK_IMEI_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_IMEI_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_IMEI_SIZE;

          /* Pack the Value field */
          memcpy( &tlv_buff[offset + 2],
                  parsed_tlv_buff->iemi.imei,
                  UIM_TK_IMEI_SIZE );
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_IMEI_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_IMEI_TAG */
      break;

    case UIM_TK_HELP_REQ_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_HELP_REQ_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_HELP_REQ_SIZE;

          /* There is no Value field to pack */
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_HELP_REQ_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_HELP_REQ_TAG */
      break;

    case UIM_TK_NW_MEAS_RESULTS_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_NW_MEAS_RESULTS_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_NW_MEAS_RESULTS_SIZE;

          /* Pack the Value field */
          memcpy( &tlv_buff[offset + 2],
                  parsed_tlv_buff->nw_meas_results.nw_meas_data,
                  UIM_TK_NW_MEAS_RESULTS_SIZE );
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_NW_MEAS_RESULTS_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_NW_MEAS_RESULTS_TAG */
      break;

    case UIM_TK_DEFAULT_TEXT_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->default_txt.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->default_txt.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->default_txt.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->default_txt.dcs;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->default_txt.data,
                  parsed_tlv_buff->default_txt.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->default_txt.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_DEFAULT_TEXT_TAG */
      break;

    case UIM_TK_ITEMS_NEXT_ACTION_IND_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->next_action.num_bytes)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->next_action.num_bytes +
             (word)length_size + 1) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->next_action.num_bytes;

          /* Pack the Value field */
          memcpy (&tlv_buff[offset + length_size + 1],
                  parsed_tlv_buff->next_action.data,
                  parsed_tlv_buff->next_action.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->next_action.num_bytes + length_size + 1;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ITEMS_NEXT_ACTION_IND_TAG */
      break;

    case UIM_TK_ICON_ID_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_ICON_ID_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_ICON_ID_SIZE;

          /* Pack the Value field */
          tlv_buff[offset + 2] = parsed_tlv_buff->icon_id.icon_qual;
          tlv_buff[offset + 3] = parsed_tlv_buff->icon_id.icon_id;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_ICON_ID_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ICON_ID_TAG */
      break;

    case UIM_TK_ICON_ID_LIST_TAG:
      {
        /* Determine the length size of the data */
        if ((parsed_tlv_buff->icon_id_list.num_bytes + 1)
            > UIM_TK_ONE_BYTE_MAX_LENGTH)
        {
          /* The length is represented in two bytes */
          length_size = 2;
        }
        else /* One byte represents the length */
        {
          length_size = 1;
        } /* end if - check the size of the data. */

        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + (word)parsed_tlv_buff->icon_id_list.num_bytes +
             (word)length_size + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Assume this is a two byte length.  Place the two byte indicator
            in the first byte of the length.  If the length is actually one
            byte, the next instruction will overwrite the first byte. */
          tlv_buff[offset + 1] = UIM_TK_TWO_BYTE_LENGTH;
          tlv_buff[offset + length_size] =
            parsed_tlv_buff->icon_id_list.num_bytes + 1;

          /* Pack the Value field */
          tlv_buff[offset + length_size + 1] =
            parsed_tlv_buff->icon_id_list.icon_list_qual;
          memcpy (&tlv_buff[offset + length_size + 2],
                  parsed_tlv_buff->icon_id_list.data,
                  parsed_tlv_buff->icon_id_list.num_bytes);
          /* Fill in the result buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size =
            parsed_tlv_buff->icon_id_list.num_bytes + length_size + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ICON_ID_LIST_TAG */
      break;

    case UIM_TK_IMMEDIATE_RSP_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_IMMEDIATE_RSP_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_IMMEDIATE_RSP_SIZE;

          /* There is no VALUE field to pack */
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_IMMEDIATE_RSP_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_IMMEDIATE_RSP_TAG */
      break;

    case UIM_TK_LANG_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_LANG_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_LANG_SIZE;

          /* Pack the VALUE field */
          tlv_buff[offset + 2] = parsed_tlv_buff->lang.lang & 0xff;
#ifndef CUST_EDITION
          tlv_buff[offset + 3] = (parsed_tlv_buff->lang.lang << 8) & 0xff;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_IMMEDIATE_RSP_SIZE + 2;
#else
          tlv_buff[offset + 3] = (parsed_tlv_buff->lang.lang >> 8) & 0xff;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_LANG_SIZE + 2;
#endif //#if 0
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_LANG_TAG */
      break;
#ifdef CUST_EDITION	  
    case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:
      {
      	int i;
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_DATE_TIME_SIZE + 2) <= UIM_MAX_CHARS)
        {
          uint8 tmp;
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_DATE_TIME_SIZE;

          /* Pack the VALUE field */    
          tmp = (parsed_tlv_buff->date_time.year >= 2000) ? 
                 (parsed_tlv_buff->date_time.year - 2000) : (parsed_tlv_buff->date_time.year - 1900);	                  
          tlv_buff[offset + 2] = UTK_INT2BCD(tmp);                                             
          tlv_buff[offset + 3] = UTK_INT2BCD(parsed_tlv_buff->date_time.month);
          tlv_buff[offset + 4] = UTK_INT2BCD(parsed_tlv_buff->date_time.day);
          tlv_buff[offset + 5] = UTK_INT2BCD(parsed_tlv_buff->date_time.hour);
          tlv_buff[offset + 6] = UTK_INT2BCD(parsed_tlv_buff->date_time.minute);
          tlv_buff[offset + 7] = UTK_INT2BCD(parsed_tlv_buff->date_time.second);               
          tlv_buff[offset + 8] = parsed_tlv_buff->date_time.zone;
                                                                           
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_DATE_TIME_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG */
      break;   
      
    case UIM_TK_ACCESS_TECHNOLOGY_TAG:
      {
        /* Make sure the simple TLV fits within the BER-TLV */
        if (((word)offset + UIM_TK_ACCESS_TECH_SIZE + 2) <= UIM_MAX_CHARS)
        {
          /* Set the tag of the simple TLV */
          tlv_buff[offset] = local_tag;

          /* Set the size of the simple TLV */
          tlv_buff[offset + 1] = UIM_TK_ACCESS_TECH_SIZE;

          /* Pack the VALUE field */
          tlv_buff[offset + 2] = parsed_tlv_buff->access_technology.access_technology;
          /* Fill in the return buffer */
          return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          return_buf.tlv_size = UIM_TK_ACCESS_TECH_SIZE + 2;
        }
        else /* The simple TLV is too large for this BER-TLV */
        {
          /* Indicate the offset is outside the BER-TLV */
          return_buf.status = UIM_TK_OFFSET_OUTSIDE_BUFFER;
          return_buf.tlv_size = 0;
        } /* end if - The simple TLV fits within the BER_TLV */
      } /* end case - UIM_TK_ACCESS_TECHNOLOGY_TAG */
      break;      
 #endif
      /* Tag values for the BER tags */
    case  UIM_TK_PROACTIVE_SIM_COMMAND_TAG:
    case  UIM_TK_CDMA_SMS_PP_DOWNLOAD_TAG:
    case  UIM_TK_MENU_SELECTION_TAG:
      {
        /* Set the status to a valid status */
        return_buf.status = UIM_TK_UNEXPECTED_TAG;
      } /* end case */
      break;

    default:
      {
        /* Indicate an undefined tag value.  Check the length as well.
           A bad length has presidence over an undefined tag value. */
        return_buf.status = UIM_TK_UNKNOWN_TAG;
      }
  } /* end switch - tag field */

  return return_buf;
} /* uim_tk_pack_simple_tlv */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_SEND_CMD_TO_UI                         INTERNAL FUNCTION

DESCRIPTION
  Send this proactive command to UI for processing.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  The UI is given the proactive command to process.

===========================================================================*/
void uim_tk_send_cmd_to_ui
(
  uim_rsp_buf_type *rsp_ptr
)
{
  ui_cmd_type  *ui_buf_ptr; /* pointer to buffer for ui cmd */

/* Send a msg status command to the UI task */
#ifdef FEATURE_REX_DYNA_MEM_UI
  ui_buf_ptr = ui_get_cmd();
#else
  if( (ui_buf_ptr = (ui_cmd_type*) q_get( &ui_cmd_free_q )) == NULL )
  {
    ERR("Out of UI cmd buffer", 0,0,0);
    return;
  }
  ui_buf_ptr->proactive_cmd.hdr.done_q_ptr = &ui_cmd_free_q;;
#endif /* FEATURE_REX_DYNA_MEM_UI */

  ui_buf_ptr->proactive_cmd.hdr.cmd        = UI_PROACTIVE_UIM_F;
  ui_buf_ptr->proactive_cmd.hdr.task_ptr   = NULL;
  ui_buf_ptr->proactive_cmd.hdr.sigs       = 0;

  if(rsp_ptr->cmd_rsp_size > UIM_MAX_CHARS)
  {
    ERR_FATAL("uim_tk_send_cmd_to_ui too LONG %d %d",rsp_ptr->cmd_rsp_size,rsp_ptr->rsp,0);
  }
  ui_buf_ptr->proactive_cmd.num_bytes = rsp_ptr->cmd_rsp_size;
  (void) memcpy ( ui_buf_ptr->proactive_cmd.cmd_data,
                  (void *) rsp_ptr->rsp.data, rsp_ptr->cmd_rsp_size );

  /* send command to ui */
  ui_cmd( ui_buf_ptr );
} /* proactive_cmd_report */

/* <EJECT> */
/*===========================================================================

FUNCTION PROACTIVE_CMD_REPORT                           INTERNAL FUNCTION

DESCRIPTION
  Processes the response callback for sending a proactive command response.
  Nothing is done since the UIM task does not process a failure nor a success.

DEPENDENCIES
  Cannot wait for the response since this function runs under
  the context of the UIM task.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void proactive_cmd_report
(
  uim_rpt_type *dummy
)
{
  /* Do nothing */
} /* proactive_cmd_report */

/* <EJECT> */
/*===========================================================================

FUNCTION START_TERMINAL_RESPONSE_BUFFER                     INTERNAL FUNCTION

DESCRIPTION
  Retrieves a command buffer from the UIM free queue and packs the Command
  Details, Device IDs and starts the Result TLV in the parsed_tlv_buff.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the UIM command buffer pointer.  A NULL value indicates the UIM
  free queue was empty.

SIDE EFFECTS
  The objects passed in the parameter list are changed to start a Terminal
  response for a Proactive command.

===========================================================================*/
uim_cmd_type *start_terminal_response_buffer
(
  byte *packed_offset,              /* Pointer to offset for packing */
  uim_tk_tlv_type *parsed_tlv_buf,  /* Points to parsed TLV buffer */
  uim_tk_command_details_tlv_type *cmd_details /* Points to cmd details */
)
{
  /* UIM command pointer points to UIM command buffer for sending commands
     to the UIM server. */
  uim_cmd_type *uim_cmd_ptr;
  /* This holds the return structure for processing simple TLVs */
  uim_tk_proc_simple_tlv_return_type proc_simple_tlv_return_buf;

  /* Retrieve a UIM command buffer from the free queue. */
  if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
  {
    /* Initialize the offset used to pack the simple TLVs in the Terminal
       Response */
    *packed_offset = 0;
    /* Copy the command details to the parsed buffer. */
    parsed_tlv_buf->cmd_details = *cmd_details;
    /* Build the Command Details TLV of the response */
    proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
      uim_cmd_ptr->terminal_response.data, *packed_offset, parsed_tlv_buf);

    /* Adjust the offset for the next simple TLV. */
    *packed_offset += proc_simple_tlv_return_buf.tlv_size;
    /* Build the Device Identities of the response */
    parsed_tlv_buf->hdr.comprehension_required = TRUE;
    parsed_tlv_buf->hdr.tag = UIM_TK_DEVICE_ID_TAG;
    parsed_tlv_buf->device_id.source_dev_id = UIM_TK_TERMINAL;
    parsed_tlv_buf->device_id.dest_dev_id   = UIM_TK_UIM;
    /* Pack the command details in the Terminal Response */
    proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
      uim_cmd_ptr->terminal_response.data, *packed_offset, parsed_tlv_buf);

    /* Adjust the offset for the next simple TLV. */
    *packed_offset += proc_simple_tlv_return_buf.tlv_size;
    /* Build the Result of the response */
    parsed_tlv_buf->hdr.comprehension_required = TRUE;
    parsed_tlv_buf->hdr.tag = UIM_TK_RESULT_TAG;
    parsed_tlv_buf->result.num_bytes = 0;
  } /* end if - got a buffer from the UIM free queue */

  /* Send the UIM command pointer */
  return uim_cmd_ptr;
} /* end - start_terminal_response_buffer */

/* <EJECT> */
/*===========================================================================

FUNCTION PROACTIVE_CMD_RESPONSE                           INTERNAL FUNCTION

DESCRIPTION
  Queues the terminal response on the UIM command queue.

DEPENDENCIES
  Cannot wait for the response since this function runs under
  the context of the UIM task.

RETURN VALUE
  None

SIDE EFFECTS
  The terminal response is sent back to the UIM card.

===========================================================================*/
void proactive_cmd_response
(
  uim_cmd_type *cmd_ptr       /* R-UIM command                               */
)
{
  /* Terminal Response */
  cmd_ptr->hdr.command = UIM_TERMINAL_RESPONSE_F;

  /* Indicate command completion is to be signaled:
   *    Do not signal the UIM Task
   *    Use no signal
   *    No "done" queue
   *    Status call-back function
   *    Always Report status
   */

  cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;

  cmd_ptr->hdr.cmd_hdr.sigs = 0;

  cmd_ptr->hdr.rpt_function = proactive_cmd_report;

  cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;

  cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

  /* Indicate the channel the command is to be sent */ 
  cmd_ptr->hdr.channel = UIM_DEFAULT_CHANNEL;

  /* Send the command to the R-UIM */
  uim_cmd( cmd_ptr );
} /* proactive_cmd_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PROCESS_PROACTIVE_COMMAND

DESCRIPTION
  This procedure processes a proactive command received from the UIM card.

DEPENDENCIES
  This function needs an interface from the UI to process proactive commands
  that apply to the UI.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void uim_tk_process_proactive_command
(
  uim_rsp_buf_type *rsp_ptr
)
{
  /* This holds the return value of the parse BER-TLV call */
  uim_tk_parse_ber_tlv_return_type parse_ber_tlv_return_buf;
  /* This holds the return structure for processing simple TLVs */
  uim_tk_proc_simple_tlv_return_type proc_simple_tlv_return_buf;
  /* This holds the parsed simple TLV */
  uim_tk_tlv_type parsed_tlv_buf;
  /* This holds the offset of the parsed TLVs */
  byte parsed_offset = 0;
  /* This holds the offset of the packed TLVs */
  byte packed_offset = 0;
  /* UIM command pointer points to UIM command buffer for sending commands
     to the UIM server. */
  uim_cmd_type *uim_cmd_ptr;
  /* This holds the fields for a Command Details TLV */
  uim_tk_command_details_tlv_type cmd_details = {{FALSE,0},0,0,0};
  /* This holds the fields for a Duration TLV */
  uim_tk_duration_tlv_type duration = {{FALSE,0},0,0};
  /* This is a flag to indicate when to exit the loop to parse the Proactive
     command */
  boolean exit_parse_loop = FALSE;
  /* This holds the simple TLVs processed in the Proactive command */
  uint32 processed_bit_ind = 0;
  /* This holds the TLVs that have comprehension required */
  uint32 comprehension_required_bit_ind = 0;

  /* Verify the command is a proactive command */
  parse_ber_tlv_return_buf =
    uim_tk_parse_ber_tlv (rsp_ptr->rsp.data, rsp_ptr->cmd_rsp_size);
  /* Check if the BER-TLV is a proper proactive command */
  if ((parse_ber_tlv_return_buf.status == UIM_TK_MESSAGE_IS_VALID) &&
      (parse_ber_tlv_return_buf.tag == UIM_TK_PROACTIVE_SIM_COMMAND_TAG))
  {
    /* Loop through all the simple TLVs. */
    do
    {
      /* Parse the next simple TLV */
      proc_simple_tlv_return_buf =
        uim_tk_parse_simple_tlv_from_ber_tlv(
          rsp_ptr->rsp.data,
          parsed_offset,
          &parsed_tlv_buf);
      /* Adjust the offset past the simple TLV */
      parsed_offset += proc_simple_tlv_return_buf.tlv_size;

      /* Determine the status of the parse */
      switch (proc_simple_tlv_return_buf.status)
      {
        case UIM_TK_UNEXPECTED_TAG:
          {
            /* The tag is a BER tag and not a simple tag.
               Check if the comprehension is required for this TLV */
            if (parsed_tlv_buf.hdr.comprehension_required)
            {
              /* Exit processing the Proactive command. */
              exit_parse_loop = TRUE;
            } /* end if - comprehension is required */
          } /* end case - UIM_TK_UNEXPECTED_TAG */
          break;

        case UIM_TK_UNKNOWN_TAG:
          {
            /* The tag is not recognised by this code. */
            /* Check if the comprehension is required for this TLV */
            if (parsed_tlv_buf.hdr.comprehension_required)
            {
              /* Exit processing the Proactive command. */
              exit_parse_loop = TRUE;
            } /* end if - comprehension is required */
          } /* end case - UIM_TK_UNKNOWN_TAG */
          break;

        /* Check if the simple TLV is resides outside the Proactive command */
        case UIM_TK_OFFSET_OUTSIDE_BUFFER:
          {
            /* Exit the loop and finish processing the proactive command */
            exit_parse_loop = TRUE;
          } /* end case - UIM_TK_OFFSET_OUTSIDE_BUFFER */
          break;

        case UIM_TL_LENGTH_ERROR:
          {
            /* Cannot process TLV, length type is not supported
               exit loop and return 'command data not understood by me'*/
            exit_parse_loop = TRUE;
          }
          break;

        case UIM_TK_MSG_VALID_AND_LAST:
          /* Exit the loop after processing this simple TLV */
          exit_parse_loop = TRUE;
          /* Change the status to indicate the message is valid */
          proc_simple_tlv_return_buf.status = UIM_TK_MESSAGE_IS_VALID;
          /* Execution of this case is to continue into the next case.
             Do not add any case between these and do not expect a break. */
        case UIM_TK_MESSAGE_IS_VALID:
          {
            /* Process the simple TLV according to tag */
            switch (parsed_tlv_buf.hdr.tag)
            {
              case UIM_TK_COMMAND_DETAILS_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_COMMAND_DETAILS_BIT_IND);

                  /* Save the command details fields */
                  cmd_details = parsed_tlv_buf.cmd_details;
                } /* end case - UIM_TK_COMMAND_DETAILS_TAG */
                break;

              case UIM_TK_DEVICE_ID_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_DEVICE_ID_BIT_IND);
                } /* UIM_TK_DEVICE_ID_TAG */
                break;
              case UIM_TK_RESULT_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_RESULT_BIT_IND);
                } /* UIM_TK_RESULT_TAG */
                break;
              case UIM_TK_DURATION_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_DURATION_BIT_IND);

                  /* Save the duration fields */
                  duration = parsed_tlv_buf.duration;
                } /* UIM_TK_DURATION_TAG */
                break;
              case UIM_TK_ALPHA_ID_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ALPHA_ID_BIT_IND);
                } /* UIM_TK_ALPHA_ID_TAG */
                break;
              case UIM_TK_ADDRESS_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ADDRESS_BIT_IND);
                } /* UIM_TK_ADDRESS_TAG */
                break;
              case UIM_TK_CAP_CONFIG_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_CAP_CONFIG_BIT_IND);
                } /* UIM_TK_CAP_CONFIG_TAG */
                break;
              case UIM_TK_SUBADDRESS_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_SUBADDRESS_BIT_IND);
                } /* UIM_TK_SUBADDRESS_TAG */
                break;
              case UIM_TK_CDMA_SMS_TPDU_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_CDMA_SMS_TPDU_BIT_IND);
                } /* UIM_TK_CDMA_SMS_TPDU_TAG */
                break;
              case UIM_TK_TEXT_STRING_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_TEXT_STRING_BIT_IND);
                } /* UIM_TK_TEXT_STRING_TAG */
                break;
              case UIM_TK_TONE_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_TONE_BIT_IND);
                } /* UIM_TK_TONE_TAG */
                break;
              case UIM_TK_ITEM_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ITEM_BIT_IND);
                } /* UIM_TK_ITEM_TAG */
                break;
              case UIM_TK_ITEM_ID_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ITEM_ID_BIT_IND);
                } /* UIM_TK_ITEM_ID_TAG */
                break;
              case UIM_TK_RSP_LEN_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_RSP_LEN_BIT_IND);
                } /* UIM_TK_RSP_LEN_TAG */
                break;
              case UIM_TK_FILE_LIST_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_FILE_LIST_BIT_IND);
                } /* UIM_TK_FILE_LIST_TAG */
                break;
              case UIM_TK_LOCN_INFO_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_LOCN_INFO_BIT_IND);
                } /* UIM_TK_LOCN_INFO_TAG */
                break;
              case UIM_TK_IMEI_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_IMEI_BIT_IND);
                } /* UIM_TK_IMEI_TAG */
                break;
              case UIM_TK_HELP_REQ_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_HELP_REQ_BIT_IND);
                } /* UIM_TK_HELP_REQ_TAG */
                break;
              case UIM_TK_NW_MEAS_RESULTS_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_NW_MEAS_RESULTS_BIT_IND);
                } /* UIM_TK_NW_MEAS_RESULTS_TAG */
                break;
              case UIM_TK_DEFAULT_TEXT_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_DEFAULT_TEXT_BIT_IND);
                } /* UIM_TK_DEFAULT_TEXT_TAG */
                break;
              case UIM_TK_ITEMS_NEXT_ACTION_IND_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ITEMS_NEXT_ACTION_IND_BIT_IND);
                } /* UIM_TK_ITEMS_NEXT_ACTION_IND_TAG */
                break;
              case UIM_TK_ICON_ID_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ICON_ID_BIT_IND);
                } /* UIM_TK_ICON_ID_TAG */
                break;
              case UIM_TK_ICON_ID_LIST_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_ICON_ID_LIST_BIT_IND);
                } /* UIM_TK_ICON_ID_LIST_TAG */
                break;
              case UIM_TK_IMMEDIATE_RSP_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_IMMEDIATE_RSP_BIT_IND);
                } /* UIM_TK_IMMEDIATE_RSP_TAG */
                break;
              case UIM_TK_LANG_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_LANG_BIT_IND);
                } /* UIM_TK_LANG_TAG */
                break;
#ifdef CUST_EDITION				
              case UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_LANG_BIT_IND);
                } /* UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG */
                break;              
              case UIM_TK_ACCESS_TECHNOLOGY_TAG:
                {
                  /* Process the bit indicators. */
                  UIMTK_PROCESS_TLV_BIT_IND (UIM_TK_LANG_BIT_IND);
                } /* UIM_TK_ACCESS_TECHNOLOGY_TAG */
                break;                
#endif				
              default:
                {
                  /* The tag is not recognised by this code. */
                  /* Check if the comprehension is required for this TLV */
                  if (parsed_tlv_buf.hdr.comprehension_required)
                  {
                    /* Change the parse status */
                    proc_simple_tlv_return_buf.status = UIM_TK_UNKNOWN_TAG;
                    /* Exit processing the Proactive command. */
                    exit_parse_loop = TRUE;
                  } /* end if - comprehension is required */
                } /* default*/
                break;

            } /* end switch - process TLVs according to the tag */
          } /* end case - UIM_TK_MESSAGE_IS_VALID */
          break;

        default:
          {
            /* This status is not expected. */
            MSG_ERROR ("Unrecognized status in TLV processing", 0, 0, 0);

            /* Exit processing the Proactive command. */
            exit_parse_loop = TRUE;
          }
          break;
      } /* end switch - status of the parse */
    } while ( !exit_parse_loop );

    /* Build the Result TLV based on the status of the parse */
    if (proc_simple_tlv_return_buf.status != UIM_TK_MESSAGE_IS_VALID)
    {
      /* Start the Terminal Response */
      uim_cmd_ptr = start_terminal_response_buffer
        (&packed_offset, &parsed_tlv_buf, &cmd_details);
      /* Make sure there was a UIM buffer available. */
      if (uim_cmd_ptr != NULL)
      {
        /* Set the result to indicate we do not understand the command */
        parsed_tlv_buf.result.result =
          UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;
        /* Pack the result in the Terminal Response */
        proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
          uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
        /* Adjust the offset to get the total size for the Terminal
           Response. */
        packed_offset += proc_simple_tlv_return_buf.tlv_size;

        /* Fill in the comand size */
        uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
        /* Send the Terminal Response */
        proactive_cmd_response(uim_cmd_ptr);
      }
      else /* The UIM free queue is empty */
      {
        MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
      } /* end if - check if the UIM free queue is empty */
    }
    else /* The message is valid */
    {
      /* Process the proactive command */
      switch (cmd_details.cmd_type)
      {
        case UIM_TK_REFRESH:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_REFRESH */
          break;

        case UIM_TK_MORE_TIME:
          {
            /* Start the Terminal Response */
            uim_cmd_ptr = start_terminal_response_buffer
              (&packed_offset, &parsed_tlv_buf, &cmd_details);
            /* Make sure there was a UIM buffer available. */
            if (uim_cmd_ptr != NULL)
            {
              /* Determine if the necessary TLVs are in the command. */
              if ((processed_bit_ind & UIM_TK_MORE_TIME_MIN_TLVS) !=
                  (UIM_TK_MORE_TIME_MIN_TLVS))
              {
                /* Did not get the necessary TLVs */
                parsed_tlv_buf.result.result =
                  UIM_TK_ERROR_REQD_VALUES_ARE_MISSING;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Determine if there are TLVs with comprehension required
                      that are not understood for this command */
              if (comprehension_required_bit_ind &
                       ~UIM_TK_MORE_TIME_ALLOWED_TLVS)
              {
                /* Got some comprehension required TLVs that don't belong */
                parsed_tlv_buf.result.result =
                  UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Process the command */
              {
                /* Set the result to indicate we have processed the command */
                parsed_tlv_buf.result.result =
                  UIM_TK_CMD_PERFORMED_SUCCESSFULLY;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              } /* Determine the result based on the command parameters. */
            }
            else /* The UIM free queue is empty */
            {
              MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
            } /* end if - check if the UIM free queue is empty */
          } /* end case - UIM_TK_MORE_TIME */
          break;

        case UIM_TK_POLL_INTERVAL:
          {
            /* Start the Terminal Response */
            uim_cmd_ptr = start_terminal_response_buffer
              (&packed_offset, &parsed_tlv_buf, &cmd_details);
            /* Make sure there was a UIM buffer available. */
            if (uim_cmd_ptr != NULL)
            {
              /* Determine if the necessary TLVs are in the command. */
              if ((processed_bit_ind & UIM_TK_POLL_INTERVAL_MIN_TLVS) !=
                  (UIM_TK_POLL_INTERVAL_MIN_TLVS))
              {
                /* Did not get the necessary TLVs */
                parsed_tlv_buf.result.result =
                  UIM_TK_ERROR_REQD_VALUES_ARE_MISSING;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Determine if there are TLVs with comprehension required
                      that are not understood for this command */
              if (comprehension_required_bit_ind &
                       ~UIM_TK_POLL_INTERVAL_ALLOWED_TLVS)
              {
                /* Got some comprehension required TLVs that don't belong */
                parsed_tlv_buf.result.result =
                  UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Process the command */
              {
                /* Set the result to indicate we have processed the command */
                parsed_tlv_buf.result.result =
                  UIM_TK_CMD_PERFORMED_SUCCESSFULLY;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset for the next simple TLV. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Convert the duration to milliseconds */
                switch (duration.time_unit)
                {
                  case UIM_TK_MINUTES:
                    {
                      uim_proactive_time_delay =
                        duration.time_interval * MSECS_PER_MIN;
                    } /* end case - UIM_TK_MINUTES */
                    break;

                  case UIM_TK_SECONDS:
                    {
                      uim_proactive_time_delay =
                        duration.time_interval * MSEC_PER_SEC;
                    } /* end case - UIM_TK_SECONDS */
                    break;

                  case UIM_TK_TENTHS_SECS:
                    {
                      uim_proactive_time_delay =
                        duration.time_interval * MSEC_PER_DSEC;
                    } /* end case - UIM_TK_TENTHS_SECS */
                    break;

                  default:
                    {
                      /* Got some values that are not valid */
                      parsed_tlv_buf.result.result =
                        UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;
                      /* Pack the result in the Terminal Response */
                      proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                        uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                      /* Adjust the offset to get the total size for the Terminal
                         Response. */
                      packed_offset += proc_simple_tlv_return_buf.tlv_size;

                      /* Fill in the comand size */
                      uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                      /* Send the Terminal Response */
                      proactive_cmd_response(uim_cmd_ptr);
                    }
                    return;
                } /* end switch - UIM_TK_TENTHS_SECS */

                /* We support a minimum of 1 second in 1msec increments. */
                if (uim_proactive_time_delay < UIM_MIN_POLL_DURATION)
                {
                  uim_proactive_time_delay = UIM_MIN_POLL_DURATION;
                  /* Set this duration in the response to the UIM */
                  parsed_tlv_buf.duration.time_unit = UIM_MIN_TIME_UNIT;
                  parsed_tlv_buf.duration.time_interval = UIM_MIN_TIME_INTERVAL;
                } /* end if - Polling duration is less than 1 sec. */

                /* Build the Duration TLV */
                parsed_tlv_buf.hdr.comprehension_required = TRUE;
                parsed_tlv_buf.hdr.tag = UIM_TK_DURATION_TAG;
                parsed_tlv_buf.duration.time_unit =
                  duration.time_unit;
                parsed_tlv_buf.duration.time_interval =
                  duration.time_interval;

                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Call the UIM power control function to use the new Proactive
                   polling value.  Do not affect the power on voting. */
                (void)uim_power_control( UIM_NONE, TRUE );

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;

                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              } /* Determine the result based on the command parameters. */
            }
            else /* The UIM free queue is empty */
            {
              MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
            } /* end if - check if the UIM free queue is empty */
          } /* end case - UIM_TK_POLL_INTERVAL */
          break;

        case UIM_TK_POLLING_OFF:
          {
            /* Start the Terminal Response */
            uim_cmd_ptr = start_terminal_response_buffer
              (&packed_offset, &parsed_tlv_buf, &cmd_details);
            /* Make sure there was a UIM buffer available. */
            if (uim_cmd_ptr != NULL)
            {
              /* Determine if the necessary TLVs are in the command. */
              if ((processed_bit_ind & UIM_TK_POLLING_OFF_MIN_TLVS) !=
                  (UIM_TK_POLLING_OFF_MIN_TLVS))
              {
                /* Did not get the necessary TLVs */
                parsed_tlv_buf.result.result =
                  UIM_TK_ERROR_REQD_VALUES_ARE_MISSING;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;

                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Determine if there are TLVs with comprehension required
                      that are not understood for this command */
              if (comprehension_required_bit_ind &
                       ~UIM_TK_POLLING_OFF_ALLOWED_TLVS)
              {
                /* Got some comprehension required TLVs that don't belong */
                parsed_tlv_buf.result.result =
                  UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;

                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              }
              else /* Process the command */
              {
                /* Set the proactive polling duration to the maximum allowed */
                uim_proactive_time_delay = (dword)UIM_MAX_POLL_DURATION;
                /* Call the UIM power control function to use the new Proactive
                   polling value.  Do not affect the power on voting. */
                (void)uim_power_control( UIM_NONE, TRUE );

                /* Set the result to indicate we have processed the command */
                parsed_tlv_buf.result.result =
                  UIM_TK_CMD_PERFORMED_SUCCESSFULLY;
                /* Pack the result in the Terminal Response */
                proc_simple_tlv_return_buf = uim_tk_pack_simple_tlv(
                  uim_cmd_ptr->terminal_response.data, packed_offset, &parsed_tlv_buf);
                /* Adjust the offset to get the total size for the Terminal
                   Response. */
                packed_offset += proc_simple_tlv_return_buf.tlv_size;

                /* Fill in the comand size */
                uim_cmd_ptr->terminal_response.num_bytes = packed_offset;
                /* Send the Terminal Response */
                proactive_cmd_response(uim_cmd_ptr);
              } /* Determine the result based on the command parameters. */
            }
            else /* The UIM free queue is empty */
            {
              MSG_ERROR ("No UIM command buffers on uim_free_q",0,0,0);
            } /* end if - check if the UIM free queue is empty */
          } /* end case - UIM_TK_POLLING_OFF */
          break;

        case UIM_TK_SETUP_CALL:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_SETUP_CALL */
          break;

        case UIM_TK_CDMA_SEND_SHORT_MSG:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_CDMA_SEND_SHORT_MSG */
          break;

        case UIM_TK_PLAY_TONE:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_PLAY_TONE */
          break;

        case UIM_TK_DISPLAY_TEXT:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_DISPLAY_TEXT */
          break;

        case UIM_TK_GET_INPUT:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_GET_INPUT */
          break;

        case UIM_TK_SELECT_ITEM:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_SELECT_ITEM */
          break;

        case UIM_TK_SETUP_MENU:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_SETUP_MENU */
          break;

        case UIM_TK_PROVIDE_LOCAL_INFO:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_PROVIDE_LOCAL_INFO */
          break;

        case UIM_TK_END_PROACTIVE_SESSION:
          {
            /* Send proactive command to the UI */
            uim_tk_send_cmd_to_ui (rsp_ptr);
          } /* end case - UIM_TK_END_PROACTIVE_SESSION */
          break;

        default:
          {

          } /* end case - default */
          break;
      } /* Process the proactive command */
    } /* end if - the parse status indicates the command is not valid */
  } /* end if - the Proactive command is a proper BER-TLV */
} /* uim_tk_process_proactive_command */
#endif /* FEATURE_UIM_TOOLKIT_UTK */

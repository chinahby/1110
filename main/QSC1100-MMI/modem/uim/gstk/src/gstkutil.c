/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        G S T K     U T I L I T Y


GENERAL DESCRIPTION
  This is the GSTK internal utility file.  It contains function that will
  be called within GSTK module such basic util functionalities.


FUNCTIONS
  gstk_util_cmd_details_ref_table_get_next_free_slot
    This function returns the next available slot in the
    command_details_ref_table

  gstk_util_cmd_details_ref_table_free_slot
    This function marks the index location in the command_details_ref_table
    as indicated by the ref_id as available for upcoming use

  gstk_util_env_cmd_ref_table_get_next_free_slot
    This function returns the next available slot in the
    envelope_cmd_ref_table

  gstk_util_env_cmd_ref_table_free_slot
    This function marks the index location in the envelope_cmd_ref_table
    as indicated by the ref_id as available for upcoming use

  gstk_util_save_env_cmd_info
    This function get a free slot from the envelope_cmd_ref_table, and
    populate it with the user envelope command info.

  gstk_valid_tag
    Check for proactive command tag matches

  gstk_get_uim_buf
    This function returns a pointer to heap for buffer allocation for UIM
    Server interface.

  gstk_send_cmd_to_uim_server
    This function sends the uim cmd to the uim server

  gstk_send_cmd_to_uim_free_q
    This function sends the uim cmd back to uim free queue

  gstk_util_populate_uim_buf_hdr_info
    This function gets a pointer for uim_cmd_ptr, and populates header info
    of the uim_cmd_type based on the uim_cmd_name_type

  gstk_find_length_of_length_value
    This function determines the size of the length field

  gstk_util_is_valid_client_id
    This function determines if the client_id is valid or not

  gstk_util_convert_cmd_type_to_reg_bitmask
    This function converts the command type into the registration bitmask

  gstk_util_find_client_with_registered_event
    This function goes through the gstk_client_table and see which client
    has registered for the event

  gstk_util_send_message_to_clients
    This function sends the message to the GSTK clients based on which clients
    have registered to receive that events.

  gstk_util_send_response_to_clients
    This function sends the Card Response to the GSTK clients based on which
    client registration callback function (used for Envelope Response)

  gstk_util_start_client_response_timer
    This function starts the terminal response timer for the proactive command
    that is just sent to the client

  gstk_util_populate_card_cmd_memory
    This function allocates and copies memory for the command pointer input.

  gstk_util_release_card_cmd_memory
    This function frees all the allocated memory for the command pointer input.

  gstk_util_release_upper_layer_cmd_memory
    This function frees all the allocated memory for the upper layer (U)SAT
    command response pointer input.

  gstk_util_check_evt_mask_reg_status
    This function check if the input bitmask has already been registered by
    any of the other clients.

  gstk_util_translate_qualifier_to_dcs
    This function translates the qualifier into the corresponding data
    coding scheme.

  gstk_util_build_evt_dl_ind
    This function populates the gstk_task_cmd_type based on the passed in
    event download indication

  gstk_util_check_mm_state_difference
    This function will get the cell id and determines whether there are any
    differences between the new location info/state with the old new.

  gstk_util_compare_curr_and_next_location_info
    This function will compare the previously sent location info with
    the one in the next available one

  gstk_util_pack_7_bit_chars
    This function packs ASCII chars to SMS 7 bit format

  gstk_util_unpack_7_bit_chars
    This function unpacks SMS 7 bit format to ASCII chars

  gstk_util_check_proactive_card
    Check if the card is has proactive commands capability

  gstk_util_map_mm_to_gstk_location_status
    This function maps the mm location status to gstk location status

  gstk_util_is_valid_location_info
    This function will check if the MNC, MCC, LAC, CellID and location
    status info is valid combination.

  gstk_util_determine_new_pref_from_mode_chg
    This function determines the new preferred slot from the mode change new
    slot info.

  gstk_util_handle_mode_change
    This function determine new preferred slot based on new mode from mode
    change command, and process the start toolkit or mode switch process
    accordingly

  gstk_util_has_pending_timer_envelope
    This function determines if there is any pending envelope command for
    timer expiration that needs to be sent

  gstk_util_find_first_timer_expire_command
    This function find the first pending timer expire command available.

  gstk_util_dereg_client_type
    This function dereg events for certain client type.  dereg_evt indicates
    the events to be removed from the client's list

  gstk_util_find_first_non_timer_expire_postpone_env_command
    This function find the first non timer expire postpone envelope command if
    exists.

  gstk_util_has_pending_setup_call
    This function find if there is any pending setup call proactive command

  gstk_util_decode_dcs_type
    This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
    on whether it is a cell broadcast dcs or not

  gstk_util_is_valid_channel_id_range
    This function determines if the channel id provided by the SIM is within
    valid range

  gstk_util_unpack_7_bit_chars_ext
    This function unpacks the input data from SMS7 bit format into ASCII
    and returns the actual data length without the null terminated byte.

  gstk_util_find_apn_len
    This function calculates the no of bytes to be allocated to hold
    the APN after packing. This includes the extra bytes required for
    the label separtors '.' and the terminating null byte.

  gstk_util_compareString
    This function is used to compare two strings, substrings or to
    look for the presence of a particular character in the string.

  gstk_util_extract_label_len_value
    This function is used to extract the length and the value part
    of a label and pack in the format specified in the 23.003 specs
    for APN

  gstk_util_check_for_apn_net_op_id
    This function checks the labels in the APN to determine if
    the label correspomds to an Operator ID or a Network ID.

  gstk_util_dump_byte_array
    This function dumps byte array in the format of three item per line.

  gstk_util_is_valid_data_format_req
    Check if data format type is valid

 gstk_util_is_valid_func_req
    Check if functionality type is valid

  gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type
    This function converts an STK command to a GSTK command type

  gstk_util_call_client_evt_cb
    This function call client callback based on callback type

  gstk_util_free_client_id
    This function frees up client id

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstkutil.c#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/15/09   xz      Support client id clean up of modem restart
03/02/09   xz      1) Fix typo of variable name
                   2) Check if callback is null before send msg to client
02/26/09   gg      Introduced Class B event registration
02/03/09   yb      Added secondary support for cell id information when 
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
01/26/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
01/23/09   xz      Fix the issue of failing to covert timer management 
                   cmd to event mask
11/10/08   xz      Added gstk_util_free_client_id()
11/06/08   sk      Fixed registration when wrong class is provided.
10/13/08   yb      Fixed documentation
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/08/08   sk      Fixed warnings
09/05/08   xz      Rollback change of checking client_id in gstk_util_save_env_cmd_info()
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/28/08   sk      Added fix for GSTK enhanced registration.
                   Removed gstk_util_calculate_log2l()
08/22/08   xz      Save expected envelope response of GSTK_RAW_ENVELOPE_IND
                   envelope_cmd_ref_table
                   Save client_id to envelope_cmd_ref_table in gstkutil.c
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session.
                   Fixed compiler issue for using unsigned long long with 
                   switch
08/21/08   sk      Added support for GSTK enhanced registration.
                   Added support to send end session to only those clients
                   that are involved in the current proactive session
                   Added support to differentiate the different types of
                   Provide Local Info Proactive command requests
08/14/08   sk      Fixed warning.
08/08/08   xz      1) Added support for raw terminal response and raw envelope 
                   response
                   2) function to dump byte array
05/06/08   sk      Added support for event download envelope retry
03/11/08   jar     Wrapped memfree for PLI NMR UTRAN in function
                   gstk_util_release_upper_layer_cmd_memory() to avoid an
                   erroneous memfree.
02/06/08   sk      Support for PLI UTRAN NMR
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
08/08/07   sk      Fixed warning
08/07/07   sk      Temporarily commented code that stores apdu byte transactions
                   in efs
08/01/07   sk      Fixed lint errors
07/23/07   sk      Fixed warning
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
07/14/07   sk      Fixed unpacking sms default for trailing zeroes
06/24/07   sk      Fixed potential buffer overflow
05/21/07   jk      Check to make see if UIM in recovery mode before sending
                   recovery command
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
12/01/06   sk      Corrected Feature Name
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors
10/08/06   sk      Added support for unknown unsupported proactive commands.
08/25/06   sk      Featurized uim recovery support
08/21/06   sk      Added featurization for GPRS call control
06/24/06   sk      Lint fixes, Envelope fixes for data available and
                   channel status.
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM for OFFTARGET.  Compilation Warning Fix
06/14/06   sk      Removed warnings
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support and lint
05/25/06   sk      Fixed Open Channel APN issues.
                   Added support for registration for Channel Status
                   and Data Available envelopes.
05/17/06   sk      Lint fixes
05/03/06   sk      Removed cell id check function
05/03/06   tml     Fixed compilation issue
05/03/06   tml     lint
04/04/06   sk      Added support for immediate digit response for get inkey
                   Made gstk_util_find_match_in_cmd_details_ref_table()
                   obsolete.
04/04/06   sk      Added support for PLI - battery status
04/04/06   sk      Removed obsolete function
                   gstk_util_rrc_get_serving_cell_id()
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Use GSDI protocol to determine which UIM protocol to
                   send command on
10/14/05   tml     Fixed compilation
10/14/05   tml     Fixed compilation
10/14/05   sun     Added utility function to log data
10/10/05   tml     Added utility function to check and map rat
10/08/05   tml     Valid Cell ID Check function
10/07/05   tml     Limited NO SERVICE LS event
09/30/05   sk      Added funtion gstkutil_convert_rrc_plmn_id_to_nas_plmn_id()
                   and gstkutil_update_plmn_lac_cellId_table()
07/26/05   tml     fixed DCS decoding function
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
                   Added gstk_util_find_match_in_cmd_details_ref_table()
05/25/05   sk      Removed check for Mormal Service in function
                   gstk_util_check_mm_state_difference() in order to
                   support event downloads when changing between cells with
                   limited service.
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
02/09/05   sk      Added defintion for ASCIIText_ConvertTo_GSMDefault()
                   amd also the corresponding conversion table.
02/02/05   sk      Changed gstk_util_compareString to return
                   gstk_status_enum_type.
02/02/05   sk      Added definctions for functions
                   gstk_util_find_apn_len(),
                   gstk_util_compareString(),
                   gstk_util_extract_label_len_value(),
                   gstk_util_check_for_apn_net_op_id
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Return correct number of bytes after 7 bit decoding
08/19/04   tml     Added cell broadcast support
08/18/04   tml     Added gstk_util_decode_dcs_type
08/13/04   tml     Enforce pointer to NULL after mem free
08/10/04   tml     Added gstk_util_find_first_non_timer_expire_postpone_env_command
                   and gstk_util_has_pending_setup_call functions
08/03/04   tml     Memory free for additional info pointer
06/28/04   tml     Added fake IMSI support
05/26/05   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/04/04   tml     Compilation fix
03/01/04   tml     Added location status env cmd fixes and provide lang info
                   support
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/03/03   tml     Added packing and unpacking functions
07/07/03   tml     Changed messages so that they are more meaningful
06/23/03   tml     removed compiler error for 1x/gsm
05/26/03   tml     linted
04/21/03   tml     Command qualifier fix
03/21/03   tml     Update featurization for WCDMA singlebuild
03/20/03   tml     Properly featurize local info for WCDMA and GSM as well as
                   virtual uim server
03/06/03   tml     Updated enum names, Added (FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/27/03 jar/tml   Return gstk_status_enum_type in gstk_util_populate_card_
                   cmd_memory().  Removed Call Event:  MT,CALL_DISCONNECTED,
                   CALL_CONNECTED in gstk_util_build_evt_dl_ind().
02/26/03   tml     Change gstk_util_save_env_cmd_info to take
                   gstk_evt_cb_funct_type instead of pointer to the type
02/13/03   tml     Added/Changed DM messages
02/12/03   tml     Free icon type in the release
02/11/03   tml     Get uim queue from uim_cmd_q with the correct feature
                   Return valid client id when the queried id is
                   GSTK_CLIENT_ID
02/07/03  tml/jar  Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_rsp.h"
#include "gstk_state.h"
#include "gstk_exp.h"
#include "gstk_proactive_cmd.h"

#include "gsdi_exp.h"
#include "gsdi.h"
#include "task.h"
#include "err.h"
#include "string.h"
 
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#include "sys.h"
#if defined (FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM */
#if defined (FEATURE_GSM)
#error code not present
#endif /*FEATURE_GSM */
#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

#include "tmc.h"

#ifdef FEATURE_GSDI_UIM_SERVER
#include "virtuim.h"
#endif

#ifdef FEATURE_GSTK_STATE_TESTER
#include "gstk_state_tester.h"
#endif

#include "fs.h"

#define GSTK_SMS_8_BIT_GENERAL_DATA_CODING_DCS      0x04
#define GSTK_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x08
#define GSTK_SMS_GENERAL_DATA_CODING_MASK           0xCC

#define GSTK_SMS_8_BIT_DCS                          0xF4
#define GSTK_SMS_DATA_CODING_MASK                   0xFC

#define GSTK_CBS_8_BIT_DCS                          0xF4
#define GSTK_CBS_DATA_CODING_MASK                   0xFC

#define GSTK_CBS_8_BIT_GENERAL_DATA_CODING_DCS      0x44
#define GSTK_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x48
#define GSTK_CBS_GENERAL_DATA_CODING_MASK           0xCC

#define GSTK_CBS_UCS2_7_BIT_DCS                     0x11

/* Conversion table for ASCII to SMS Default */
static char ascii_to_gsmdefault_table[] =
{
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', '!',0x22, '#', '$', '%', '&',0x27, '(', ')', '*', '+', ',', '-', '.', '/',
 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
 0x00, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
 ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}','~', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', 0x40, ' ', 0x01, 0x24, 0x03, ' ', 0x5F, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x0B, ' ', ' ', ' ', ' ', ' ', ' ', 0x60,
 ' ', ' ', ' ', ' ', 0x5B, 0x0E, 0x1C, 0x09, ' ', 0x1F, ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', 0x5D, ' ', ' ', ' ', ' ', 0x5C, ' ', ' ', ' ', ' ', ' ', 0x5E, ' ', ' ', 0x1E,
 0x7F, ' ', ' ', ' ', 0x7B, 0x0F, 0x1D, ' ', 0x04, 0x05, ' ', ' ', 0x07, ' ', ' ', ' ',
 ' ', 0x7D, 0x08, ' ', ' ', ' ', 0x7C, ' ', 0x0C, 0x06, ' ', ' ', 0x7E, ' ', ' ', ' ',
};

/*===========================================================================

                         GLOBALS FOR MODULE

===========================================================================*/

/* define Max length and max no of substrings that need to be avoided in the APN */
#define GSTK_APN_MAXLEN 6
#define GSTK_APN_MAXNO  4

/* SPEC 11.14 - Substrings to be avoided in APN Network ID */
static char gstk_apn_start_str_table[GSTK_APN_MAXNO][GSTK_APN_MAXLEN] = {"rac", "lac", "sgsn", "rnc"};
static char gstk_apn_end_str_table[GSTK_APN_MAXLEN] = ".gprs";

/* SPEC 11.14 - Count of substring to be avoided in APN Network ID */
static int gstk_start_str_cnt = 4;
static int gstk_end_str_cnt = 1;



/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_get_next_free_slot

DESCRIPTION
  This function returns the next available slot in the
  command_details_ref_table

PARAMETERS
  None

DEPENDENCIES
    None

RETURN VALUE
    reference id for the next available slot

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint32 gstk_util_cmd_details_ref_table_get_next_free_slot()
{
  uint32 i;

  for (i = 0; i< GSTK_MAX_PROACTIVE_COMMAND; i++) {
    if ( command_details_ref_table[i].free_slot == TRUE) {
        command_details_ref_table[i].free_slot= FALSE;
        return i;
    }
  }
  MSG_HIGH("no more cmd details ref free slot", 0, 0, 0);
  return GSTK_CMD_DETAILS_REF_TABLE_FULL;
}


/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_free_slot

DESCRIPTION
  This function marks the index location in the command_details_ref_table
  as indicated by the ref_id as available for upcoming use

PARAMETERS
  ref_id: [Input] index location in the command_details_ref_table to be marked
                  free

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
void gstk_util_cmd_details_ref_table_free_slot(uint32 ref_id)
{
  if (ref_id >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    /*invalid ref id*/
    MSG_ERROR("Invalid cmd details ref id (0x%x)!", ref_id, 0, 0);
    return;
  }
  /* Set the free slot boolean to TRUE, so that the slot is avaliable next time */
  command_details_ref_table[ref_id].free_slot = TRUE;
  /* Reset all the information in the table to 0 */
  command_details_ref_table[ref_id].command_details.command_number = 0;
  command_details_ref_table[ref_id].command_details.length = 0;
  command_details_ref_table[ref_id].command_details.qualifier = 0;
  command_details_ref_table[ref_id].command_details.tag = 0;
  command_details_ref_table[ref_id].command_details.type_of_command = 0;
  command_details_ref_table[ref_id].destination_dev_id = GSTK_NO_DEVICE;
  command_details_ref_table[ref_id].expected_cmd_rsp_type = GSTK_CMD_ENUM_NOT_USE;
  command_details_ref_table[ref_id].partial_comprehension = FALSE;
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  command_details_ref_table[ref_id].pending_responses = 0;
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
  /* clear the timer */
  (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));
} /* gstk_util_cmd_details_ref_table_free_slot */

/*===========================================================================
FUNCTION gstk_util_env_cmd_ref_table_get_next_free_slot

DESCRIPTION
  This function returns the next available slot in the
  envelope_cmd_ref_table

PARAMETERS
  None

DEPENDENCIES
    None

RETURN VALUE
    reference id for the next available slot

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
uint32 gstk_util_env_cmd_ref_table_get_next_free_slot() {
  uint32 i;

  for (i = 0; i< GSTK_MAX_PROACTIVE_COMMAND; i++) {
    if ( envelope_cmd_ref_table[i].free == TRUE) {
        envelope_cmd_ref_table[i].free = FALSE;
        return i;
    }
  }
  if (i >= GSTK_MAX_PROACTIVE_COMMAND) {
    MSG_HIGH("no more env ref free slot",0 , 0, 0);
    return GSTK_CMD_DETAILS_REF_TABLE_FULL;
  }
  MSG_HIGH("no more env ref free slot", 0, 0, 0);
  return GSTK_CMD_DETAILS_REF_TABLE_FULL;
} /* gstk_util_env_cmd_ref_table_get_next_free_slot */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_util_env_cmd_ref_table_free_slot

DESCRIPTION
  This function marks the index location in the envelope_cmd_ref_table
  as indicated by the ref_id as available for upcoming use

PARAMETERS
  ref_id: [Input] index location in the envelope_cmd_ref_table to be marked
                  free

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_util_env_cmd_ref_table_free_slot(uint32 ref_id) {

  /* Set the free slot boolean to TRUE, so that the slot is avaliable next time */
  envelope_cmd_ref_table[ref_id].free = TRUE;
  /* Reset all the information in the table to 0 */
  envelope_cmd_ref_table[ref_id].client_id = 0;
  if (envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_CALL_CONNECTED_EVT_IND_RSP) {
    /* free the user data */
    if (envelope_cmd_ref_table[ref_id].user_data != 0) {
      if (((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[ref_id].user_data)->transaction_id.transaction_list
            != NULL) {
        gstk_free(
          ((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[ref_id].user_data)->transaction_id.transaction_list);
        ((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[ref_id].user_data)->transaction_id.transaction_list = NULL;
      }
      gstk_free((gstk_transaction_id_direction_type*)envelope_cmd_ref_table[ref_id].user_data);
    }
  }
  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_crit_sect);
  if (envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_DATA_AVAIL_EVT_IND_RSP ||
      envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type == GSTK_CH_STATUS_EVT_IND_RSP)
  {
    gstk_bip_env_in_progress = FALSE;
  }
  rex_leave_crit_sect(&gstk_crit_sect);
  /* Leaving gstk critical section */
  envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type = GSTK_CMD_ENUM_NOT_USE; /* to indicate none */
  envelope_cmd_ref_table[ref_id].env_cb_ptr = NULL;
  envelope_cmd_ref_table[ref_id].user_data = 0;

} /* gstk_util_env_cmd_ref_table_free_slot */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_util_save_env_cmd_info

DESCRIPTION
  This function get a free slot from the envelope_cmd_ref_table, and populate
  it with the user envelope command info.

PARAMETERS
  client_id: [Input] client id for referencing the client callback if the
                     envelope command is not sent by internal modules
  env_cb: [Input] function to be called upon response from Card is received
                  for the envelope command
  env_cmd_type: [Input] envelope command type
  user_data: [Input] user data
  env_ref_id: [Input/Output] envelope reference id in the
                             envelope_cmd_ref_table

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gstk_status_enum_type gstk_util_save_env_cmd_info(
                gstk_client_id_type     client_id,
                gstk_evt_cb_funct_type  env_cb,
                gstk_cmd_enum_type      env_cmd_type,
                uint32                  user_data,
                uint32                 *env_ref_id)
{
  gstk_cmd_enum_type   env_rsp_type = GSTK_CMD_ENUM_NOT_USE;

  if(env_ref_id != NULL) {
    *env_ref_id = gstk_util_env_cmd_ref_table_get_next_free_slot();
    if (*env_ref_id != GSTK_CMD_DETAILS_REF_TABLE_FULL &&
        *env_ref_id <= GSTK_MAX_PROACTIVE_COMMAND -1) {
      /* get a valid location from the env cmd reference table */
      /* populate the table */
      envelope_cmd_ref_table[*env_ref_id].user_data = user_data;
      if(env_cb != NULL) {
        envelope_cmd_ref_table[*env_ref_id].env_cb_ptr = (gstk_evt_cb_funct_type)env_cb;
      }
      else {
        if(gstk_util_is_valid_client_id(client_id) ) {
          envelope_cmd_ref_table[*env_ref_id].env_cb_ptr =
            (gstk_evt_cb_funct_type)gstk_client_table[client_id-1].evt_cb.client_cb;
        }
        else {
          /* free the slot! */
          gstk_util_env_cmd_ref_table_free_slot(*env_ref_id);
          return GSTK_CLIENT_NOT_REGISTERED;
        }
      }
    } /* more room for the envlope command */
    else {
      return GSTK_MEMORY_ERROR;
    }
  }
  else {
    MSG_HIGH("env cmd and ref id is null", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  /* get the expected rsp type */

  switch(env_cmd_type) {
  case GSTK_MENU_SELECTION_IND:
    env_rsp_type = GSTK_MENU_IND_RSP;
    break;
  case GSTK_SMS_PP_DOWNLOAD_IND:
    env_rsp_type = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
    break;
  case GSTK_MT_CALL_EVT_IND:
    env_rsp_type = GSTK_MT_CALL_EVT_IND_RSP;
    break;
  case GSTK_CALL_CONNECTED_EVT_IND:
    env_rsp_type = GSTK_CALL_CONNECTED_EVT_IND_RSP;
    break;
  case GSTK_CALL_DISCONNECTED_EVT_IND:
    env_rsp_type = GSTK_CALL_DISCONNECTED_EVT_IND_RSP;
    break;
  case GSTK_IDLE_SCRN_EVT_IND:
    env_rsp_type = GSTK_IDLE_SCRN_EVT_IND_RSP;
    break;
  case GSTK_LOCATION_STATUS_EVT_IND:
    env_rsp_type = GSTK_LOCATION_STATUS_EVT_IND_RSP;
    break;
  case GSTK_USER_ACTIVITY_EVT_IND:
    env_rsp_type = GSTK_USER_ACTIVITY_EVT_IND_RSP;
    break;
  case GSTK_BROWSER_TERMINATION_EVT_IND:
    env_rsp_type = GSTK_BROWSER_TERMINATION_EVT_IND_RSP;
    break;
  case GSTK_LANG_SELECTION_EVT_IND:
    env_rsp_type = GSTK_LANG_SELECTION_EVT_IND_RSP;
    break;
  case GSTK_MO_SMS_CTRL_IND:
    env_rsp_type = GSTK_MO_SMS_CTRL_IND_RSP;
    break;
  case GSTK_CC_IND:
    env_rsp_type = GSTK_CC_IND_RSP;
    break;
  case GSTK_TIMER_EXPIRE_IND:
    env_rsp_type = GSTK_TIMER_EXPIRE_IND_RSP;
    break;
  case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
    env_rsp_type = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
    break;
  case GSTK_ACCESS_TECH_EVT_IND:
    env_rsp_type = GSTK_ACCESS_TECH_EVT_IND_RSP;
    break;
  case GSTK_DATA_AVAIL_EVT_IND:
    env_rsp_type = GSTK_DATA_AVAIL_EVT_IND_RSP;
    break;
  case GSTK_CH_STATUS_EVT_IND:
    env_rsp_type = GSTK_CH_STATUS_EVT_IND_RSP;
    break;
#ifdef FEATURE_CAT_REL6
  case GSTK_NW_SEARCH_MODE_EVT_IND:
    env_rsp_type = GSTK_NW_SEARCH_MODE_EVT_IND_RSP;
    break;
#endif
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
  default:
    MSG_MED("Unexpected event download type!", 0, 0, 0);
    break;
  }
  envelope_cmd_ref_table[*env_ref_id].expected_cmd_rsp_type = env_rsp_type;
  envelope_cmd_ref_table[*env_ref_id].client_id = client_id;
  return GSTK_SUCCESS;
} /* gstk_util_save_env_cmd_info */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_util_retrieve_env_cmd_info

DESCRIPTION
  This function will retrieve the envelope command info from the envelope table
  based on the envelope reference id and the command type.

PARAMETERS
  env_cmd_type:  [Input] envelope command type
  user_data_ptr: [Output] pointer to user data
  env_cb:        [Input] function to be called upon response from Card is received
                  for the envelope command
  env_ref_id:    [Input] envelope reference id in the envelope_cmd_ref_table

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
gstk_status_enum_type gstk_util_retrieve_env_cmd_info(
  gstk_cmd_enum_type      env_cmd_type,
  uint32                  *user_data_ptr,
  gstk_evt_cb_funct_type  *env_cb_ptr,
  uint32                  env_ref_id)
{
  if(user_data_ptr == NULL)
  {
    MSG_ERROR("user_data_ptr NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(env_cb_ptr == NULL)
  {
    MSG_ERROR("env_cb_ptr NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if ((env_ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) ||
      (env_ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    MSG_ERROR("Invalid env_ref_id",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(envelope_cmd_ref_table[env_ref_id].free == FALSE &&
     envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type == env_cmd_type)
  {
      *user_data_ptr = envelope_cmd_ref_table[env_ref_id].user_data ;
      if(envelope_cmd_ref_table[env_ref_id].env_cb_ptr != NULL)
      {
        *env_cb_ptr = envelope_cmd_ref_table[env_ref_id].env_cb_ptr;
      }
      else
      {
        MSG_HIGH("env cb ptr NULL",0,0,0);
      }
      return GSTK_SUCCESS;
  }
  else
  {
    MSG_ERROR("Envelope could not be found",0,0,0);
    return GSTK_ERROR;
  }
} /* gstk_util_retrieve_env_cmd_info */


/*===========================================================================
FUNCTION gstk_valid_tag

DESCRIPTION
  Check for proactive command tag matches

PARAMETERS
  tag: [Input] tag extracted from the card data buffer
  type: [Input] the type that we are trying to match the tag with

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => match
           FALSE => does not match)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_valid_tag(uint8 tag,tags_enum_type type)
{
    boolean valid = FALSE;

    if(tag == (int)type) { /* if tag of BER-TLV tag or command tag w/o CR */
        valid = TRUE;
    }
    if (!valid) { /* check if the command tag is passed in with CR */
      if (tag > 0x80) { /* tag with CR */
       tag -= 0x80;
      }
      if(tag == (int)type) {
        valid = TRUE;
      }
    }

    MSG_LOW("TAG = 0x%x, TYPE = 0x%x",tag, type, 0);

//    else if((tag & GSTK_CR_TAG_MASK) == type)
 //   {
  //      if(type != ITEMS_NEXT_ACTION_INDICATOR_TAG) /* Special case */
   //     {
    //        valid = TRUE;
     //   }
   // }

    return(valid);
} /* gstk_valid_tag*/


#if 0
/* ===========================================================================
FUNCTION gstkutil_store_tookit_data

DESCRPTION
  This function will be used to store the toolkit data.  It will store
  the TLVs for Proactive Commands, Terminal Responses, Envelope Commands,
  etc.

DEPENDENCIES
  None

RETURN VALUE
  gstk_status

============================================================================*/
static void gstkutil_store_toolkit_data(
  int32         len,
  const uint8 * data_p
)
{
  char  *             file_name_p   = "stk_tlvs.bin"; /* File Name & Handler     */
  uint8 *             file_data_p   = 0x00;           /* Pointer for data storage*/
  int32               max_file_size = 0x400;          /* Max bytes Log Supports  */
  dword               total_size    = 0x00;           /* File Size + len to save */
  uint8 *             curr_mem_p    = 0x00;
  fs_handle_type      handle;
  fs_rsp_msg_type     fs_rsp;                         /* FS Responses            */
  fs_open_xparms_type open_parms;                     /* Open Cleanup options    */
  int32               fs_open_permission;
  uint8               header[16];
  uint8               header_len = 16;
  uint8               padding[16];
  uint8               pad_len = 0;

  MSG_HIGH("LOGGING STK TLV",0,0,0);


  if ( data_p == NULL )
  {
    MSG_ERROR("Invalid data_p",0,0,0);
    return;
  }
  /* -------------------------------------------------------------------------
     Get the current size of the file in the File System and check against
     the total size supported.
     -----------------------------------------------------------------------*/
  fs_file_size(file_name_p, NULL, &fs_rsp);

  switch ( fs_rsp.file_size.status )
  {
    case FS_OKAY_S:
      MSG_HIGH("%s File Size: 0x%x",file_name_p, fs_rsp.file_size.size,0);
      fs_open_permission = (int)FS_OA_READWRITE;
      break;

    case FS_NONEXISTENT_FILE_S:
      /* Force the size to 0 bytes */
      fs_rsp.file_size.size = 0;
      fs_open_permission = (int)FS_OA_CREATE;
      break;

    default:
      MSG_ERROR("FS_OPEN : FS_OA_CREATE Failure: 0x%x",fs_rsp.file_size.status,0,0);
      return;
  }

  /* The hignhest number possible with a 32 bit signed integer would be  2,147 ,483 ,647.
     This number mod 16 would always fit in a8 bit unsigned value.
     Hence type casting
  */

  if(len%16 !=0)
  {
    pad_len =  16 - ((uint8)(len%16));
  }

  if ( (fs_rsp.file_size.size + (dword)len  + (dword)pad_len + (dword)header_len) > (dword) max_file_size )
  {
    MSG_ERROR("Log Full...Not Purging",0,0,0);
    return;
  }

  total_size = fs_rsp.file_size.size + (dword)len + (dword)header_len + (dword)pad_len;

  /* --------------------------------------------------------------------------
     Next, create a buffer to allow for the storage for the new data
     ------------------------------------------------------------------------*/
  file_data_p = gstk_malloc((uint32)total_size);

  if ( file_data_p == NULL )
  {
    MSG_ERROR("No mem for data storage",0,0,0);
    return;
  }

  curr_mem_p = file_data_p;

  /* If the total_file_size is the same as the len provided, do not, read
  ** the file system  data
  */

  /* --------------------------------------------------------------------------
     Now read back the data from the File System so that the new data can
     be appended to it.

     Open the File in EFS for Create as a New File
     Other Open File Options are:
        FS_OA_CREATE,     Create new file
        FS_OA_APPEND,     Open a file for append access
        FS_OA_READONLY,   Open a file for read-only access
        FS_OA_READWRITE,  Open a file for read-write access
        FS_OA_TRUNCATE,   Truncates file contents
        FS_OA_TEST        Reserved for file system testing
     -------------------------------------------------------------------------*/
  open_parms.create.cleanup_option   = FS_OC_CLOSE;
  open_parms.create.buffering_option = FS_OB_PROHIBIT;

  fs_open(file_name_p,       /* File Handle Provided       */
          (fs_access_option_type)fs_open_permission,/* Open File as New File      */
          &open_parms,       /* Extra Parameters...not set */
          NULL,              /* Callback ... not provied   */
          &fs_rsp);          /* Response                   */

  if (fs_rsp.open.status!=FS_OKAY_S)
  {
    MSG_ERROR("fs_open procedure failed: %x",fs_rsp.open.status,0,0);
    gstk_free(file_data_p);
    return;
  }

  /* -----------------------------------------------------------------------
     Store the Handle Locally
     ---------------------------------------------------------------------*/
  handle = fs_rsp.open.handle;

 /* ------------------------------------------------------------------------
   Read the file into buffer provided
   ----------------------------------------------------------------------*/
  fs_read( handle,                  /* file handle            */
           file_data_p  ,           /* source to store bytes  */
           (dword)fs_rsp.file_size.size,      /* # bytes to read        */
           NULL,                    /* callback_function      */
           &fs_rsp);                /* message ptr            */

  if (fs_rsp.read.status != FS_OKAY_S)
  {
    MSG_ERROR("fs_read procedure failed: %x",fs_rsp.read.status,0,0);
    gstk_free(file_data_p);
    return;
  }

  /* ------------------------------------------------------------------------
     Next, move the current pointer of the data to be read to the end of the
     data location
     ----------------------------------------------------------------------*/
  curr_mem_p = file_data_p + fs_rsp.read.count;

   /*Append the header for the new data*/
  memset(header,0xFF,header_len);
  memcpy(curr_mem_p, header, header_len);

  curr_mem_p += header_len;
  /* --------------------------------------------------------------------------
     Append the Data provided to the buffer location
     ------------------------------------------------------------------------*/
  memcpy(curr_mem_p, data_p,(uint32)len);

  /*Add the padding*/
  curr_mem_p += len;

  memset(padding, 0x0c, pad_len);

  memcpy(curr_mem_p, padding, pad_len);
  /* --------------------------------------------------------------------------
     Now that the buffer is filled.  Write it back to FS.
     ------------------------------------------------------------------------*/
  fs_write(handle,
           file_data_p,
           (uint32)total_size,
           NULL,
           &fs_rsp);

  if ( fs_rsp.write.status != FS_OKAY_S)
  {
    MSG_ERROR("WRITE Failure for STK TLV",0,0,0);
    gstk_free(file_data_p);
    return ;
  }

  /* --------------------------------------------------------------------------
     Close the file
     ------------------------------------------------------------------------*/
  fs_close(handle,       /* File Handle Provided   */
           NULL,         /* Callback Pointer       */
           &fs_rsp);     /* Response               */

  if (fs_rsp.close.status!=FS_OKAY_S)
  {
    /* We will flag the Error but will not fail the entire
    ** Procedures since the handle could not be closed
    */
    ERR("fs_close procedure failed: %x",fs_rsp.close.status,0,0);
  }

  gstk_free(file_data_p);


} /* gstkutil_store_toolkit_data */
#endif /* #if 0 */

/*===========================================================================
FUNCTION gstk_get_uim_buf

DESCRIPTION
  This function returns a pointer to heap for buffer allocation for UIM
  Server interface.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  uim_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uim_cmd_type *gstk_get_uim_buf (
  void
)
{
  uim_cmd_type *cmd;
    /* Allocate command buffer space */
    /* from TMC Heap                 */
#ifdef FEATURE_GSDI_UIM_SERVER
  cmd = (uim_cmd_type*)gstk_malloc(sizeof(uim_cmd_type));
#else
  cmd = (uim_cmd_type*)q_get(&uim_free_q);
#endif
    return cmd;

} /* gstk_get_uim_buf */


/*===========================================================================
FUNCTION gstk_send_cmd_to_uim_server

DESCRIPTION
  This function sends the uim cmd to the uim server

PARAMETERS
  uim_cmd_ptr: [Input] Pointer to uim_cmd_type to be sent to uim server

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
void gstk_send_cmd_to_uim_server (
  uim_cmd_type *cmd_ptr
)
{
#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
    uim_return_type   uim_status;
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */
    if(cmd_ptr == NULL)
    {
      MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
      return;
    }
    /* commenting code temporarily till a decision to move to EFS2 is reached */
    /*switch ( cmd_ptr->hdr.command)
    {
      case UIM_TERMINAL_RESPONSE_F:
        gstkutil_store_toolkit_data(cmd_ptr->terminal_response.num_bytes,
                                    cmd_ptr->terminal_response.data);
        break;

      case UIM_TERMINAL_PROFILE_F:
        gstkutil_store_toolkit_data(cmd_ptr->terminal_profile.num_bytes,
                                    cmd_ptr->terminal_profile.data);
        break;

      case UIM_ENVELOPE_F:
        gstkutil_store_toolkit_data(cmd_ptr->envelope.num_bytes,
                                    &cmd_ptr->envelope.data[cmd_ptr->envelope.offset]);
        break;
    default:
        break;
    }*/

    (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

#ifdef FEATURE_GSDI_UIM_SERVER
    virtuim_cmd(cmd_ptr);
#else
    /* Entering gstk critical section */
    rex_enter_crit_sect(&gstk_crit_sect);
    if(cmd_ptr->hdr.command == UIM_TERMINAL_RESPONSE_F)
    {
      gstk_proactive_cmd_in_progress = FALSE;
    }
    rex_leave_crit_sect(&gstk_crit_sect);
    /* Leaving gstk critical section */
#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
    /* gstk_gsdi_internal_sim_reset set to TRUE,indicates ongoing UIM recovery,
       but also make sure that UIM hasn't switched back to NORMAL mode in the meantime */

    if((gstk_gsdi_internal_sim_reset == TRUE) && (UIM_RECOVERY_MODE == uim_get_mode()))
    {
      uim_status = uim_recovery_cmd(cmd_ptr);
      if(uim_status != UIM_SUCCESS)
      {
        MSG_ERROR("uim_recovery_cmd  FAILURE",0,0,0);
      }
    }
    else
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */
    {
      uim_cmd(cmd_ptr);
    }
#endif /* FEATURE_GSDI_UIM_SERVER */

} /* gstk_send_cmd_to_uim_server */


/*===========================================================================
FUNCTION gstk_send_cmd_to_uim_free_q

DESCRIPTION
  This function sends the uim cmd back to uim free queue

PARAMETERS
  uim_cmd_ptr: [Input] Pointer to uim_cmd_type to be sent to uim server

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
void gstk_send_cmd_to_uim_free_q (
  uim_cmd_type *cmd_ptr
)
{
    if(cmd_ptr == NULL)
    {
      MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
      return;
    }
    (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);
#ifdef FEATURE_GSDI_UIM_SERVER
    gstk_free(cmd_ptr);
#else
    q_put(cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link);
#endif /* FEATURE_GSDI_UIM_SERVER */

} /* gstk_send_cmd_to_uim_free_qr */


/*===========================================================================
FUNCTION gstk_util_populate_uim_buf_hdr_info

DESCRIPTION
  This function gets a pointer for uim_cmd_ptr, and populates header info
  of the uim_cmd_type based on the uim_cmd_name_type

PARAMETERS
  uim_cmd_type: [Input] Indicates which type of UIM Command
  uim_cmd_ptr: [Input/Output] Pointer to uim_cmd_type, which will be sent to
                              uim server eventually

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
gstk_status_enum_type gstk_util_populate_uim_buf_hdr_info(
  uim_cmd_name_type                 uim_cmd_name,
  uim_cmd_type                      **uim_cmd_ptr_ptr )
{
    uim_cmd_type*            uim_cmd_ptr;
    gstk_status_enum_type    gstk_status = GSTK_SUCCESS;
    gsdi_returns_T           gsdi_status = GSDI_SUCCESS;
    gsdi_protocol_enum_type  gsdi_prot   = GSDI_PROTOCOL_NONE;

    if(uim_cmd_ptr_ptr == NULL)
    {
        MSG_ERROR("uim_cmd_ptr_ptr ERR:NULL", 0, 0, 0);
        return GSTK_BAD_PARAM;
    }

    uim_cmd_ptr = *uim_cmd_ptr_ptr;

    if((uim_cmd_ptr) == NULL)
    {
        MSG_MED("No Buffer from UIM", 0, 0, 0);
        return GSTK_MEMORY_ERROR;
    }

    uim_cmd_ptr->hdr.command                 = uim_cmd_name;

    uim_cmd_ptr->hdr.cmd_hdr.task_ptr        = NULL;
    uim_cmd_ptr->hdr.cmd_hdr.sigs            = 0;
#ifdef FEATURE_GSDI_UIM_SERVER
    uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr      = NULL;
#endif
        /* if not F_GSDI_UIM_SERVER, do not set the done_q_ptr so that
           uim can free the queue */
#ifndef FEATURE_GSTK_DUAL_SLOTS
     uim_cmd_ptr->hdr.slot                    = UIM_SLOT_1;
#else
     if (gstk_curr_tk_slot == GSTK_SLOT_1)
     {
            uim_cmd_ptr->hdr.slot                    = UIM_SLOT_1;
     }
     else if (gstk_curr_tk_slot == GSTK_SLOT_2)
     {
            uim_cmd_ptr->hdr.slot                    = UIM_SLOT_2;
     }
    else
    {
            MSG_ERROR("Unknown slot, %d", gstk_curr_tk_slot, 0, 0);
            uim_cmd_ptr->hdr.slot                    = UIM_SLOT_1;
    }
#endif

    /* populate the protocol based on slot */
    switch (uim_cmd_ptr->hdr.slot)
    {
    case UIM_SLOT_1:
        gsdi_status = gsdi_get_protocol(GSDI_SLOT_1, &gstk_gsdi_protocol_slot[0]);
        if (gsdi_status != GSDI_SUCCESS)
        {
          MSG_ERROR("Wrong uim slot %d", uim_cmd_ptr->hdr.slot, 0, 0);
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          *uim_cmd_ptr_ptr = NULL;
          return GSTK_ERROR;
        }
        gsdi_prot = gstk_gsdi_protocol_slot[0];
        break;

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case UIM_SLOT_2:
        gsdi_status = gsdi_get_protocol(GSDI_SLOT_2, &gstk_gsdi_protocol_slot[1]);
        if (gsdi_status != GSDI_SUCCESS)
        {
          MSG_ERROR("Wrong uim slot %d", uim_cmd_ptr->hdr.slot, 0, 0);
          gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
          *uim_cmd_ptr_ptr = NULL;
          return GSTK_ERROR;
        }
        gsdi_prot = gstk_gsdi_protocol_slot[1];
        break;
#endif /*FEATURE_GSTK_DUAL_SLOTS */

    default:
        MSG_ERROR("Wrong uim slot %d", uim_cmd_ptr->hdr.slot, 0, 0);
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        *uim_cmd_ptr_ptr = NULL;
        return GSTK_ERROR;
    }

    if (gsdi_prot == GSDI_PROTOCOL_NONE)
    {
        MSG_HIGH("bad card mode ", 0, 0, 0);
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        *uim_cmd_ptr_ptr = NULL;
        return GSTK_ERROR;
    }

    if (gsdi_prot == GSDI_PROTOCOL_GSM)
    {
        uim_cmd_ptr->hdr.protocol                = UIM_GSM;
    }
    else if ((int)gsdi_prot & (int)GSDI_PROTOCOL_UICC)
    {
        uim_cmd_ptr->hdr.protocol                = UIM_WCDMA;
    }

    switch(uim_cmd_name)
    {
    case UIM_TERMINAL_RESPONSE_F:
        uim_cmd_ptr->hdr.rpt_function            = gstk_uim_terminal_response_report;
        uim_cmd_ptr->hdr.options                 = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_ENVELOPE_F:
        uim_cmd_ptr->hdr.rpt_function            = gstk_uim_envelope_report;
        uim_cmd_ptr->hdr.options                 = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_TERMINAL_PROFILE_F:
        uim_cmd_ptr->hdr.rpt_function            = gstk_uim_terminal_profile_report;
        uim_cmd_ptr->hdr.options                 = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_POLLING_INTERVAL_F:
        uim_cmd_ptr->hdr.rpt_function            = gstk_uim_polling_report;
        uim_cmd_ptr->hdr.options                 = UIM_OPTION_ALWAYS_RPT;
        break;
#ifdef FEATURE_GSTK_DUAL_SLOTS
    case UIM_RESET_WITH_TP_F:
        uim_cmd_ptr->hdr.rpt_function            = gstk_uim_terminal_profile_reset_report;
        uim_cmd_ptr->hdr.options                 = UIM_OPTION_ALWAYS_RPT;
        break;
#endif /*EFATURE_GSTK_DUAL_SLOTS */
    default:
        MSG_MED ("Invalid UIM Cmd, %x", uim_cmd_name, 0, 0);
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        *uim_cmd_ptr_ptr = NULL;
        gstk_status = GSTK_BAD_PARAM;
        break;
    }
    return gstk_status;
}  /* gstk_util_populate_uim_buf_hdr_info */


/*===========================================================================
FUNCTION gstk_find_length_of_length_value

DESCRIPTION
  This function determines the size of the length field:
  first byte of length field [0x00, 0x7F] -> length field size == 1
  first byte of length field == 0x81 -> length field size == 2

PARAMETERS
  length_value: [Input] pointer to the length value field

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int32 gstk_find_length_of_length_value(const uint8* length_value)
{
    uint8 ret_val = 0;

    if(length_value == NULL)
    {
      MSG_ERROR("length_value ERR:NULL",0,0,0);
      return ret_val;
    }

    /*
    ** This length field can either be 0x00-0x7F or
    ** 0x81 if the 2nd byte is used
    */

    MSG_LOW(" == IN GSTK_FIND_LENGTH_OF_LENGTH_VALUE",0,0,0);
    if(length_value[0] <= 0x7F)
    {
        ret_val = 1;
        MSG_LOW(" == length_value[0] = %x ret_val = %x", length_value[0], ret_val,0);
    }
    else if(length_value[0] == 0x81)
    {
        ret_val = 2;
        MSG_LOW(" == length_value[0] = %x ret_val = %x", length_value[0], ret_val,0);
    }
    else
    {
        ERR("Wrong length field value: %x", length_value[0], 0, 0);
    }
    return(ret_val);
} /* gstk_find_length_of_length_value */


/*===========================================================================
FUNCTION gstk_util_is_valid_client_id

DESCRIPTION
  This function determines if the client_id is valid or not

PARAMETERS
  client_id: [Input] Client ID to be checked

DEPENDENCIES
  None

RETURN VALUE
  boolean -> valid ID or not

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
boolean gstk_util_is_valid_client_id(
    gstk_client_id_type  client_id )
{

  if (client_id > GSTK_MAX_CLIENT) {
    return FALSE;
  }
  if (client_id ==0) {
    return FALSE;
  }
  if (client_id == GSTK_CLIENT_ID) {
    return TRUE;
  }
  return (!gstk_client_table[client_id-1].free);
} /* gstk_util_is_valid_client_id */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

/*===========================================================================
FUNCTION gstk_util_convert_cmd_type_to_reg_bitmask

DESCRIPTION
  This function converts the command type into the registration bitmask

PARAMETERS
  cmd_type: [Input] type of command that GSTK is going to search for in the
                    client table
  cmd_bitmask_ptr:   [Input/Output] Pointer to bitmask value for the cmd_type
                                    input
  class_bitmask_ptr: [Input/Output] Pointer to bitmask value for the class to 
                                    which the cmd belongs

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NULL_INPUT_PARAM    -  Null Input Parameter
    GSTK_UNSUPPORTED_COMMAND -  Unsupported command
    GSTK_SUCCESS             -  Successful conversion of cmd type to
                                bitmask

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_convert_cmd_type_to_reg_bitmask(
   gstk_cmd_enum_type   cmd_type,
   uint64               *cmd_bitmask_ptr,
   uint32               *class_bitmask_ptr)
{
  MSG_HIGH("In gstk_util_convert_cmd_type_to_reg_bitmask", 0, 0, 0);

  if (!cmd_bitmask_ptr || !class_bitmask_ptr) 
  {
    MSG_ERROR("NULL ptr provided", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }

  switch (cmd_type) {
  case GSTK_DISPLAY_TEXT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_DISPLAY_TEXT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_GET_INKEY_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_GET_INKEY_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_GET_INPUT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_GET_INPUT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_LAUNCH_BROWSER_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  case GSTK_MORE_TIME_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_MORE_TIME_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PLAY_TONE_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PLAY_TONE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_POLL_INTERVAL_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_POLL_INTERVAL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_POLLING_OFF_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_POLLING_OFF_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_LOCAL_INFO_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_LOCAL_INFO_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_LANG_INFO_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_LANG_INFO_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_BATTERY_STATUS_REQ:
#ifdef FEATURE_CAT_REL6
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_BATTERY_STATE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
#endif /* #ifdef FEATURE_CAT_REL6 */
    break;

  case GSTK_REFRESH_REQ:
    *cmd_bitmask_ptr = GSTK_REG_REFRESH_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_RUN_AT_CMDS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_RUN_AT_CMDS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_B_CMD;
    break;

  case GSTK_SELECT_ITEM_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SELECT_ITEM_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_SMS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_SMS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_SS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_SS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_USSD_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_USSD_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_CALL_REQ:
    *cmd_bitmask_ptr = GSTK_REG_SETUP_CALL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVENT_LIST_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVENT_LIST_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_MENU_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_MENU_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_IDLE_TEXT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_DTMF_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_DTMF_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_LANG_NOTIFICATION_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_LANG_NOTIFY_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_TIMER_MANAGEMENT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_TIMER_MANAGEMENT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_OPEN_CH_REQ:
  case GSTK_CLOSE_CH_REQ:
  case GSTK_SEND_DATA_REQ:
  case GSTK_RECEIVE_DATA_REQ:
  case GSTK_GET_CH_STATUS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_DATA_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  /* Envelope Indication Responses */
  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_EVT_DOWNLOAD_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_EVT_DOWNLOAD_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  /* Profile download response */
  case GSTK_PROFILE_DL_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_PROFILE_DL_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_IDLE_MENU_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_USER_ACT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_LANG_SEL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  case GSTK_SETUP_EVT_CH_STATUS_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_CH_STATUS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  default:
    return GSTK_UNSUPPORTED_COMMAND;
  }

  return GSTK_SUCCESS;
} /*gstk_util_convert_cmd_type_to_reg_bitmask*/

/*===========================================================================
FUNCTION gstk_util_find_client_with_registered_event

DESCRIPTION
  This function goes through the gstk_client_table and the class event 
  registration table to see which client has registered for the event

PARAMETERS
  client_id_ptr:[Input] Pointer to client id
  cmd_type:     [Input] type of command that GSTK is going to search for in the
                        registration table
  format_type:  [Input] Format type (RAW/GSTK)
  cli_func_type:[Input] Client functionality type (MODEM/UI/ALL)
  
DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NO_CLIENT_REG_FOUND - no client registered for the command.
    GSTK_SUCCESS             - registered client for the specific cmd found
    GSTK_ERROR               - error in finding a registered client
    GSTK_NULL_INPUT_PARAM    - null pointer provided as input
    GSTK_BAD_PARAM           - Bad parameter passed in
    GSTK_INVALID_CLIENT_TYPE - Invalid client id found in class evt reg table

COMMENTS
  There should only be 1 client registers for each command event for a
  specific functionality (MODEM/UI/ALL) at any given time

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_find_client_with_registered_event(
    gstk_client_id_type                        *client_id_ptr,
    gstk_cmd_enum_type                          cmd_type,
    gstk_client_pro_cmd_data_format_enum_type   format_type,
    gstk_tk_functionality_enum_type             cli_func_type
)
{
  uint64                cmd_bitmask    = 0;
  uint32                class_bitmask  = 0;
  uint32                index          = 0;
#if defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT)
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

  MSG_HIGH("In gstk_util_find_client_with_registered_event", 0, 0, 0);

  if(client_id_ptr == NULL)
  {
    MSG_ERROR("client_id_ptr NULL", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }

  *client_id_ptr = GSTK_MAX_CLIENT + 1;

  if(gstk_util_convert_cmd_type_to_reg_bitmask(cmd_type, 
                                               &cmd_bitmask,
                                               &class_bitmask) != GSTK_SUCCESS)
  {
    MSG_ERROR("conversion to reg bitmask failed.", 0, 0, 0);
    return GSTK_ERROR;
  }

#if !defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT)
  
  (void)format_type;
  (void)cli_func_type;

  for (index = 1; index < GSTK_MAX_CLIENT; index++) 
  { /* 0 is GSTK */
    if((gstk_client_table[index].evt_reg_bitmask & cmd_bitmask) == cmd_bitmask) 
    {
      /* found a match */
      *client_id_ptr = (gstk_client_id_type)(index+1);
      break;
    }
  }/* End of for */
  if(*client_id_ptr == (GSTK_MAX_CLIENT +1))
  {
    return GSTK_NO_CLIENT_REG_FOUND;
  }

#else /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
  if (gstk_util_is_valid_client_id(*client_id_ptr))
  {
    return GSTK_SUCCESS;
  }
  else
  {
    MSG_ERROR("Invalid client id (0x%x) in class evt reg table!",
              *client_id_ptr, 0, 0);
    return GSTK_INVALID_CLIENT_TYPE;
  }
} /* gstk_util_find_client_with_registered_event*/

/*===========================================================================
FUNCTION gstk_util_send_message_to_clients

DESCRIPTION
  This function sends the message to the GSTK clients based on which clients
  have registered to receive that events.  Upon returning from this function,
  the message_p is free

PARAMETERS
  message_ptr: [Input] Pointer to message to be sent to clients

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
gstk_status_enum_type gstk_util_send_message_to_clients(
    gstk_cmd_from_card_type * message_ptr )
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  gstk_client_id_type   client_id   = 0;
  uint32                i           = 0;

  if(message_ptr == NULL)
  {
    MSG_ERROR("message_p ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* is this an end proactive session or toolkit switch event or mode change? */
  if ((message_ptr->hdr_cmd.command_id == GSTK_MORE_TIME_REQ)
      || (message_ptr->hdr_cmd.command_id == GSTK_PROFILE_DL_IND_RSP)
#ifdef FEATURE_GSTK_DUAL_SLOTS
       || message_ptr->hdr_cmd.command_id == GSTK_TOOLKIT_MODE_CHANGE_REQ
#endif /* FEATURE_GSTK_DUAL_SLOTS */
        )
  {
    /* send to all client */
    for (i = 1; i< GSTK_MAX_CLIENT; i++) { /* 0 is GSTK */
      if((!gstk_client_table[i].free) && 
         (gstk_client_table[i].evt_cb.client_cb != NULL))
      {
        /* client is valid and cb ptr is not NULL
         * to send msg to client, we need call different
         * cb based on client's cb type.
         */
        MSG_HIGH("Send 0x%x msg ID to client", message_ptr->hdr_cmd.command_id, 0, 0);
        gstk_util_call_client_evt_cb(i + 1, message_ptr);
      }
    }
    /* release all the memory allocation since client already copied it */
    gstk_util_release_card_cmd_memory(message_ptr);

    return GSTK_SUCCESS;
  }/* if (message_ptr->hdr_cmd.command_id == GSTK_MORE_TIME_REQ */
  if (message_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ)
  {
    /* send to relevant clients */
    for (i = 1; i< GSTK_MAX_CLIENT; i++)
    {
      if(gstk_client_table[i].free)
      {
        continue;
      }
      if((gstk_client_table[i].client_type == GSTK_UI_TYPE) && 
         (gstk_is_curr_session_ui_related == TRUE))
      {
        gstk_util_call_client_evt_cb(i+1, message_ptr);
      }
      else if(gstk_client_table[i].client_type != GSTK_UI_TYPE)
      {
        gstk_util_call_client_evt_cb(i+1, message_ptr);
      }/* if(gstk_client_table[i].client_type != GSTK_UI_TYPE) */
    }/* End of for */

    gstk_is_curr_session_ui_related = FALSE;

    return gstk_status;

  }/* if (message_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ) */

  /* find the client who has registered for this event */
  gstk_status = gstk_util_find_client_with_registered_event(
                  &client_id,
                  message_ptr->hdr_cmd.command_id,
                  GSTK_GSTK_FORMAT,
                  GSTK_HANDLE_ALL_FUNC);
  if (gstk_status != GSTK_SUCCESS)
  { 
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
        /* Error or no client registered for this event */
        MSG_HIGH("Error OR no client registered for event 0x%x", gstk_status, 0, 0);
        return gstk_status;
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
  } /* All functionality client */
  if (gstk_client_table[client_id-1].evt_cb.client_cb != NULL)
  { /* valid callback function */
    gstk_util_call_client_evt_cb(client_id, message_ptr);
    if(gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
    {
      gstk_is_curr_session_ui_related = TRUE;
    }
    /* release all the memory allocation since client already copied it */
    gstk_util_release_card_cmd_memory(message_ptr);
  }

  return gstk_status;
} /* gstk_util_send_message_to_clients */

/*===========================================================================
FUNCTION gstk_util_send_response_to_clients

DESCRIPTION
  This function sends the Card Response to the GSTK clients based on which
  client registration callback function (used for Envelope Response)

PARAMETERS
  message_p: [Input] Pointer to message to be sent to clients
  env_ref_id: [Input] Envelope ref id in the envelope_cmd_ref_table

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
gstk_status_enum_type gstk_util_send_response_to_clients(
    gstk_cmd_from_card_type * message_p, uint32 env_ref_id )
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  if (env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND ) {/* something is wrong!*/
    ERR("should never be here", 0, 0, 0);
  }
  else {
    if (envelope_cmd_ref_table[env_ref_id].env_cb_ptr != NULL) { /* valid callback function */
      (envelope_cmd_ref_table[env_ref_id].env_cb_ptr)(message_p);
      /* release all the memory allocation since client already copied it */
      gstk_util_release_card_cmd_memory(message_p);
    }
    /* free the slot */
    gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
  }

  return gstk_status;
} /* gstk_util_send_response_to_clients */



/*===========================================================================
FUNCTION gstk_util_start_client_response_timer

DESCRIPTION
  This function starts the terminal response timer for the proactive command
  that is just sent to the client

PARAMETERS
  ref_id: [Input] The timer that needs to be started

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
void gstk_util_start_client_response_timer(uint32 ref_id)
{
  /* first clear the timer */
  (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

  /* then start the timer */
  (void) rex_set_timer( &(command_details_ref_table[ref_id].tp_rpt_time),
                        GSTK_TERM_RSP_WAIT_TIME );

} /* gstk_util_start_client_response_timer */


/*===========================================================================
FUNCTION gstk_util_populate_card_cmd_memory

DESCRIPTION
  This function allocates and copies memory for the command pointer input.

PARAMETERS
  old_buf: [Input] Pointer to message to which the new gstk_cmd_from_card_type
                 will be copied from
  new_buf: [Input] Pointer to new gstk_cmd_from_card_type

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
gstk_status_enum_type gstk_util_populate_card_cmd_memory(
  const gstk_cmd_from_card_type * old_buf,
  gstk_cmd_from_card_type       * new_buf)
{
  int i;

  /* check memory of buffer pointers */
  if((old_buf == NULL) || (new_buf == NULL)) {
    return GSTK_NULL_INPUT_PARAM;
  }

  /* copy the cmd_from_card_type structure first */
  memcpy(new_buf, old_buf, sizeof(gstk_cmd_from_card_type));

  /* copy individual element within various union structure in the cmd_from_card_type
  structure if neccessary */
  switch(old_buf->hdr_cmd.command_id)
  {
  case GSTK_DISPLAY_TEXT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.display_text_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.display_text_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.display_text_pro_cmd_req.text_string.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.display_text_pro_cmd_req.text_string.length);
       if(new_buf->cmd.display_text_pro_cmd_req.text_string.text != NULL)
       {
         memcpy(new_buf->cmd.display_text_pro_cmd_req.text_string.text,
           old_buf->cmd.display_text_pro_cmd_req.text_string.text,
           old_buf->cmd.display_text_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.display_text_pro_cmd_req.icon.present) &&
        (old_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.display_text_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.display_text_pro_cmd_req.icon.size);
       if(new_buf->cmd.display_text_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.display_text_pro_cmd_req.icon.data,
           old_buf->cmd.display_text_pro_cmd_req.icon.data,
           old_buf->cmd.display_text_pro_cmd_req.icon.size);
       }
     }

     break;
  case GSTK_GET_INKEY_REQ:
       /* allocate and copy text */
     if((old_buf->cmd.get_inkey_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.get_inkey_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.get_inkey_pro_cmd_req.text_string.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.get_inkey_pro_cmd_req.text_string.length);
       if(new_buf->cmd.get_inkey_pro_cmd_req.text_string.text != NULL)
       {
         memcpy(new_buf->cmd.get_inkey_pro_cmd_req.text_string.text,
           old_buf->cmd.get_inkey_pro_cmd_req.text_string.text,
           old_buf->cmd.get_inkey_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.get_inkey_pro_cmd_req.icon.present) &&
        (old_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.get_inkey_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.get_inkey_pro_cmd_req.icon.size);
       if(new_buf->cmd.get_inkey_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.get_inkey_pro_cmd_req.icon.data,
           old_buf->cmd.get_inkey_pro_cmd_req.icon.data,
           old_buf->cmd.get_inkey_pro_cmd_req.icon.size);
       }
     }
     break;
  case GSTK_GET_INPUT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.get_input_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.get_input_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.text_string.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.text_string.length);
       if(new_buf->cmd.get_input_pro_cmd_req.text_string.text != NULL)
       {
         memcpy(new_buf->cmd.get_input_pro_cmd_req.text_string.text,
           old_buf->cmd.get_input_pro_cmd_req.text_string.text,
           old_buf->cmd.get_input_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.get_input_pro_cmd_req.icon.present) &&
        (old_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.icon.size);
       if(new_buf->cmd.get_input_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.get_input_pro_cmd_req.icon.data,
           old_buf->cmd.get_input_pro_cmd_req.icon.data,
           old_buf->cmd.get_input_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy default text */
     if((old_buf->cmd.get_input_pro_cmd_req.default_text.length > 0) &&
        (old_buf->cmd.get_input_pro_cmd_req.default_text.text != NULL)) {
       new_buf->cmd.get_input_pro_cmd_req.default_text.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.get_input_pro_cmd_req.default_text.length);
       if(new_buf->cmd.get_input_pro_cmd_req.default_text.text != NULL)
       {
         memcpy(new_buf->cmd.get_input_pro_cmd_req.default_text.text,
           old_buf->cmd.get_input_pro_cmd_req.default_text.text,
           old_buf->cmd.get_input_pro_cmd_req.default_text.length);
       }
     }
     break;
  case GSTK_PLAY_TONE_REQ:
     /* allocate and copy alpha */
     if((old_buf->cmd.play_tone_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.play_tone_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.play_tone_pro_cmd_req.alpha.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.play_tone_pro_cmd_req.alpha.length);
       if(new_buf->cmd.play_tone_pro_cmd_req.alpha.text != NULL)
       {
         memcpy(new_buf->cmd.play_tone_pro_cmd_req.alpha.text,
           old_buf->cmd.play_tone_pro_cmd_req.alpha.text,
           old_buf->cmd.play_tone_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.play_tone_pro_cmd_req.icon.present) &&
        (old_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.play_tone_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.play_tone_pro_cmd_req.icon.size);
       if(new_buf->cmd.play_tone_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.play_tone_pro_cmd_req.icon.data,
           old_buf->cmd.play_tone_pro_cmd_req.icon.data,
           old_buf->cmd.play_tone_pro_cmd_req.icon.size);
       }
     }
     break;
  case GSTK_SELECT_ITEM_REQ:
    /* allocate and copy items */
     for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) {
       if((old_buf->cmd.select_item_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)) {
         new_buf->cmd.select_item_pro_cmd_req.items[i].item_text =
           (uint8*)gstk_malloc(sizeof(uint8) *
                    old_buf->cmd.select_item_pro_cmd_req.items[i].length);
         if(new_buf->cmd.select_item_pro_cmd_req.items[i].item_text != NULL)
         {
           memcpy(new_buf->cmd.select_item_pro_cmd_req.items[i].item_text,
             old_buf->cmd.select_item_pro_cmd_req.items[i].item_text,
             old_buf->cmd.select_item_pro_cmd_req.items[i].length);
         }
       }
     }
     /* allocate and copy title */
     if((old_buf->cmd.select_item_pro_cmd_req.title.length > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.title.text != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.title.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.select_item_pro_cmd_req.title.length);
       if(new_buf->cmd.select_item_pro_cmd_req.title.text != NULL)
       {
         memcpy(new_buf->cmd.select_item_pro_cmd_req.title.text,
           old_buf->cmd.select_item_pro_cmd_req.title.text,
           old_buf->cmd.select_item_pro_cmd_req.title.length);
       }
     }
     /* allocate and copy next_action_indicator */
     if((old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list =
         (uint8*)gstk_malloc(sizeof(uint8) *
                old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
       if(new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL)
       {
         memcpy(new_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.select_item_pro_cmd_req.next_action_indicator.num_items);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.select_item_pro_cmd_req.icon.present) &&
        (old_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.select_item_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.select_item_pro_cmd_req.icon.size);
       if(new_buf->cmd.select_item_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.select_item_pro_cmd_req.icon.data,
           old_buf->cmd.select_item_pro_cmd_req.icon.data,
           old_buf->cmd.select_item_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy icon id list */
     if(old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
     {
       for(i = 0; i< old_buf->cmd.select_item_pro_cmd_req.icon_id_list.num_items; i++) {
         new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] =
           (gstk_icon_type*)gstk_malloc(sizeof(gstk_icon_type));
         if((new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL) &&
            (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL)) {

           memcpy(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i],
                 sizeof(gstk_icon_type));
           if((old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data =
               (uint8*)gstk_malloc(sizeof(uint8) *
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size);
             if(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)
             {
               memcpy(new_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->size);
             }
           }
         }
       }
     }
     break;
  case GSTK_SEND_SMS_REQ:
     /* allocate and copy sms tpdu */
     if((old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length > 0) &&
        (old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
       if(new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL)
       {
         memcpy(new_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
           old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu,
           old_buf->cmd.send_sms_pro_cmd_req.sms_tpdu.length);
       }
     }
     /* allocate and copy alpha */
     if((old_buf->cmd.send_sms_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_sms_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.alpha.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_sms_pro_cmd_req.alpha.text != NULL)
       {
         memcpy(new_buf->cmd.send_sms_pro_cmd_req.alpha.text,
           old_buf->cmd.send_sms_pro_cmd_req.alpha.text,
           old_buf->cmd.send_sms_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_sms_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_sms_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_sms_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_sms_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.send_sms_pro_cmd_req.icon.data,
           old_buf->cmd.send_sms_pro_cmd_req.icon.data,
           old_buf->cmd.send_sms_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SEND_SS_REQ:
     /* allocate and copy alpha */
     if((old_buf->cmd.send_ss_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_ss_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_ss_pro_cmd_req.alpha.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_ss_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_ss_pro_cmd_req.alpha.text != NULL)
       {
         memcpy(new_buf->cmd.send_ss_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ss_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ss_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_ss_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_ss_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_ss_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_ss_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.send_ss_pro_cmd_req.icon.data,
           old_buf->cmd.send_ss_pro_cmd_req.icon.data,
           old_buf->cmd.send_ss_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SEND_USSD_REQ:
     /* allocate and copy ussd string */
     if((old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length > 0) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length);
       if(new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL)
       {
         memcpy(new_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text,
           old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.text,
           old_buf->cmd.send_ussd_pro_cmd_req.ussd_string.length);
       }
     }
     /* allocate and copy alpha */
     if((old_buf->cmd.send_ussd_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.alpha.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_ussd_pro_cmd_req.alpha.text != NULL)
       {
         memcpy(new_buf->cmd.send_ussd_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ussd_pro_cmd_req.alpha.text,
           old_buf->cmd.send_ussd_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_ussd_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_ussd_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_ussd_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_ussd_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.send_ussd_pro_cmd_req.icon.data,
           old_buf->cmd.send_ussd_pro_cmd_req.icon.data,
           old_buf->cmd.send_ussd_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_SETUP_CALL_REQ:
     /* allocate and copy user_cnf_alpha */
     if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length > 0) &&
        (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
       if(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL)
       {
         memcpy(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_alpha.length);
       }
     }
     /* allocate and copy user_cnf_alpha */
     if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length > 0) &&
        (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
              old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
       if(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL)
       {
         memcpy(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length);
       }
     }
     /* allocate and copy user_cnf_icon */
     if((old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
        (old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
       if(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)
       {
         memcpy(new_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.user_cnf_icon.size);
       }
     }
     /* allocate and copy setup_call_display_icon */
     if((old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
        (old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) {
       new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
       if(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)
       {
         memcpy(new_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data,
           old_buf->cmd.setup_call_pro_cmd_req.setup_call_display_icon.size);
       }
     }
     break;
   case GSTK_SETUP_MENU_REQ:
         /* allocate and copy items */
     for (i=0; i< GSTK_MAX_MENU_ITEMS; i++) {
       if((old_buf->cmd.setup_menu_pro_cmd_req.items[i].length > 0) &&
          (old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)) {
         new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text =
           (uint8*)gstk_malloc(sizeof(uint8) *
                    old_buf->cmd.setup_menu_pro_cmd_req.items[i].length);
         if(new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL)
         {
           memcpy(new_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text,
             old_buf->cmd.setup_menu_pro_cmd_req.items[i].item_text,
             old_buf->cmd.setup_menu_pro_cmd_req.items[i].length);
         }
       }
     }
     /* allocate and copy title */
     if((old_buf->cmd.setup_menu_pro_cmd_req.title.length > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.title.text != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.title.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.setup_menu_pro_cmd_req.title.length);
       if(new_buf->cmd.setup_menu_pro_cmd_req.title.text != NULL)
       {
         memcpy(new_buf->cmd.setup_menu_pro_cmd_req.title.text,
           old_buf->cmd.setup_menu_pro_cmd_req.title.text,
           old_buf->cmd.setup_menu_pro_cmd_req.title.length);
       }
     }
     /* allocate and copy next_action_indicator */
     if((old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items > 0) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list =
         (uint8*)gstk_malloc(sizeof(uint8) *
                old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
       if(new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL)
       {
         memcpy(new_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list,
           old_buf->cmd.setup_menu_pro_cmd_req.next_action_indicator.num_items);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.setup_menu_pro_cmd_req.icon.present) &&
        (old_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.setup_menu_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.setup_menu_pro_cmd_req.icon.size);
       if(new_buf->cmd.setup_menu_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.setup_menu_pro_cmd_req.icon.data,
           old_buf->cmd.setup_menu_pro_cmd_req.icon.data,
           old_buf->cmd.setup_menu_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy icon id list */
     if(old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
     {
       for(i = 0; i< old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) {
         new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] =
           (gstk_icon_type*)gstk_malloc(sizeof(gstk_icon_type));
         if((new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) &&
            (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL)) {

           memcpy(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i],
                 sizeof(gstk_icon_type));
           if((old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data =
               (uint8*)gstk_malloc(sizeof(uint8) *
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size);
             if(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)
             {
               memcpy(new_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data,
                 old_buf->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->size);
             }
           }
         }
       }
     }
     break;
   case GSTK_MORE_TIME_REQ:
     break;
   case GSTK_SETUP_IDLE_TEXT_REQ:
     /* allocate and copy text */
     if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length > 0) &&
        (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL)) {
       new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length);
       if(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL)
       {
         memcpy(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
        (old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size);
       if(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data,
           old_buf->cmd.setup_idle_mode_txt_pro_cmd_req.icon.size);
       }
     }
     break;
   case GSTK_END_PROACTIVE_CMD_REQ:
    break;

   case GSTK_LAUNCH_BROWSER_REQ:

     /* allocate and copy url */
     if((old_buf->cmd.launch_browser_pro_cmd_req.url.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.url.url =
         (uint8*)gstk_malloc(sizeof(uint8) *
             old_buf->cmd.launch_browser_pro_cmd_req.url.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.url.url != NULL)
       {
         memcpy(new_buf->cmd.launch_browser_pro_cmd_req.url.url,
           old_buf->cmd.launch_browser_pro_cmd_req.url.url,
           old_buf->cmd.launch_browser_pro_cmd_req.url.length);
       }
     }
     /* allocate and copy bearer list */
     if((old_buf->cmd.launch_browser_pro_cmd_req.bearer.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list =
         (gstk_bearer_code_enum_type*)gstk_malloc(
         sizeof(gstk_bearer_code_enum_type) *
                  old_buf->cmd.launch_browser_pro_cmd_req.bearer.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL)
       {
         memcpy(new_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
           old_buf->cmd.launch_browser_pro_cmd_req.bearer.bearer_list,
           old_buf->cmd.launch_browser_pro_cmd_req.bearer.length);
       }
     }
     /* allocate and copy file */
     for (i=0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) {
       if((old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length > 0) &&
          (old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)) {
         new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths =
           (uint8*)gstk_malloc(sizeof(uint8) *
                    old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length);
         if(new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL)
         {
           memcpy(new_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths,
             old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths,
             old_buf->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].length);
         }
       }
     }
     /* allocate and copy confirm alpha */
     if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL)
       {
         memcpy(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.length);
       }
     }
     /* allocate and copy gateway text string */
     if((old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length > 0) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text =
         (uint8*)gstk_malloc(sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length);
       if(new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL)
       {
         memcpy(new_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
           old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text,
           old_buf->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
        (old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) {
       new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data =
         (uint8*)gstk_malloc(
         sizeof(uint8) * old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size);
       if(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)
       {
         memcpy(new_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data,
           old_buf->cmd.launch_browser_pro_cmd_req.user_confirm_icon.size);
       }
     }
     break;

   case GSTK_SEND_DTMF_REQ:
     /* allocate and copy alpha */
     if((old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length > 0) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL)
       {
         memcpy(new_buf->cmd.send_dtmf_pro_cmd_req.alpha.text,
           old_buf->cmd.send_dtmf_pro_cmd_req.alpha.text,
           old_buf->cmd.send_dtmf_pro_cmd_req.alpha.length);
       }
     }
     /* allocate and copy icon */
     if((old_buf->cmd.send_dtmf_pro_cmd_req.icon.present) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.icon.data =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_dtmf_pro_cmd_req.icon.size);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)
       {
         memcpy(new_buf->cmd.send_dtmf_pro_cmd_req.icon.data,
           old_buf->cmd.send_dtmf_pro_cmd_req.icon.data,
           old_buf->cmd.send_dtmf_pro_cmd_req.icon.size);
       }
     }
     /* allocate and copy dtmf */
     if((old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length > 0) &&
        (old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL)) {
       new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf =
         (uint8*)gstk_malloc(sizeof(uint8) *
            old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length);
       if(new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL)
       {
         memcpy(new_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
           old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf,
           old_buf->cmd.send_dtmf_pro_cmd_req.dtmf.length);
       }
     }
     break;

   default:
     MSG_LOW("GOT SOMETHING ELSE",0,0,0);
       break;


  }
    return GSTK_SUCCESS;

} /* gstk_util_populate_card_cmd_memory */


/*===========================================================================
FUNCTION gstk_util_release_card_cmd_memory

DESCRIPTION
  This function frees all the allocated memory for the command pointer input.

PARAMETERS
  message_p: [Input] Pointer to message to which memory location is required
                     to be freed

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  The member fields in the gstk_cmd_from_card_type pointer that client passed
  in will be set to null upon the return of this function

SEE ALSO
  None
===========================================================================*/
void gstk_util_release_card_cmd_memory(
    gstk_cmd_from_card_type * message_p )
{
  int i = 0;

  if(message_p == NULL)
  {
    MSG_ERROR("message_p ERR:NULL",0,0,0);
    return;
  }

  switch(message_p->hdr_cmd.command_id) {
  case GSTK_DISPLAY_TEXT_REQ:
    if (message_p->cmd.display_text_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.display_text_pro_cmd_req.text_string.text);
      message_p->cmd.display_text_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.display_text_pro_cmd_req.icon.present) &&
      (message_p->cmd.display_text_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.display_text_pro_cmd_req.icon.data);
      message_p->cmd.display_text_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.display_text_pro_cmd_req),
           0x00,
           sizeof(gstk_display_text_req_type));

    break;

  case GSTK_GET_INKEY_REQ:
    if (message_p->cmd.get_inkey_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.get_inkey_pro_cmd_req.text_string.text);
      message_p->cmd.get_inkey_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.get_inkey_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.get_inkey_pro_cmd_req.icon.data);
      message_p->cmd.get_inkey_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.get_inkey_pro_cmd_req),
           0x00,
           sizeof(gstk_get_inkey_req_type));
    break;

  case GSTK_GET_INPUT_REQ:
    if (message_p->cmd.get_input_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.text_string.text);
      message_p->cmd.get_input_pro_cmd_req.text_string.text = NULL;
    }
    if (message_p->cmd.get_input_pro_cmd_req.default_text.text != NULL) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.default_text.text);
      message_p->cmd.get_input_pro_cmd_req.default_text.text = NULL;
    }
    if ((message_p->cmd.get_input_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_input_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.icon.data);
      message_p->cmd.get_input_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.get_input_pro_cmd_req),
           0x00,
           sizeof(gstk_get_input_req_type));
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    if (message_p->cmd.launch_browser_pro_cmd_req.url.url != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.url.url);
      message_p->cmd.launch_browser_pro_cmd_req.url.url = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list);
      message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list = NULL;
    }
    for (i = 0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) {
      if (message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL) {
        gstk_free(message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths);
        message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths = NULL;
      }
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text);
      message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text = NULL;
    }
    if ((message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
      (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data = NULL;
    }
    memset(&(message_p->cmd.launch_browser_pro_cmd_req),
           0x00,
           sizeof(gstk_launch_browser_req_type));
    break;

  case GSTK_MORE_TIME_REQ:
    /* no dynamically allocated memory */
    break;

  case GSTK_PLAY_TONE_REQ:
    if (message_p->cmd.play_tone_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.play_tone_pro_cmd_req.alpha.text);
      message_p->cmd.play_tone_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.play_tone_pro_cmd_req.icon.present) &&
      (message_p->cmd.play_tone_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.play_tone_pro_cmd_req.icon.data);
      message_p->cmd.play_tone_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.play_tone_pro_cmd_req),
           0x00,
           sizeof(gstk_play_tone_req_type));
    break;

  case GSTK_POLL_INTERVAL_REQ:
  case GSTK_POLLING_OFF_REQ:
  case GSTK_PROVIDE_LOCAL_INFO_REQ:
    /* internal command */
    break;

  case GSTK_PROVIDE_LANG_INFO_REQ:
    break;

  case GSTK_REFRESH_REQ:
    /*  internal command */
    break;

  case GSTK_RUN_AT_CMDS_REQ:
    if (message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd != NULL) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd );
      message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd = NULL;
    }
    if (message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text );
      message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.run_at_cmd_pro_cmd_req.icon.present) &&
      (message_p->cmd.run_at_cmd_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.icon.data);
      message_p->cmd.run_at_cmd_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.run_at_cmd_pro_cmd_req),
           0x00,
           sizeof(gstk_run_at_cmd_req_type));
    break;

  case GSTK_SELECT_ITEM_REQ:
    if (message_p->cmd.select_item_pro_cmd_req.title.text != NULL) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.title.text );
      message_p->cmd.select_item_pro_cmd_req.title.text = NULL;
    }
    for (i = 0; i< message_p->cmd.select_item_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.select_item_pro_cmd_req.items[i].item_text != NULL) {
        gstk_free(message_p->cmd.select_item_pro_cmd_req.items[i].item_text );
        message_p->cmd.select_item_pro_cmd_req.items[i].item_text = NULL;
      }
    }
    if (message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< GSTK_MAX_MENU_ITEMS; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             gstk_free(
               message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           gstk_free(
             message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
        }
      }
    }
    if ((message_p->cmd.select_item_pro_cmd_req.icon.present) &&
      (message_p->cmd.select_item_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.icon.data);
      message_p->cmd.select_item_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.select_item_pro_cmd_req),
           0x00,
           sizeof(gstk_select_item_req_type));
    break;

  case GSTK_SEND_SMS_REQ:
    if (message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu);
      message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu = NULL;
    }
    if (message_p->cmd.send_sms_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.alpha.text);
      message_p->cmd.send_sms_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_sms_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_sms_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.icon.data);
      message_p->cmd.send_sms_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_sms_pro_cmd_req),
           0x00,
           sizeof(gstk_send_sms_req_type));
    break;

  case GSTK_SEND_SS_REQ:
    if (message_p->cmd.send_ss_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_ss_pro_cmd_req.alpha.text);
      message_p->cmd.send_ss_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_ss_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ss_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_ss_pro_cmd_req.icon.data);
      message_p->cmd.send_ss_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_ss_pro_cmd_req),
           0x00,
           sizeof(gstk_send_ss_req_type));
    break;

  case GSTK_SEND_USSD_REQ:
    if (message_p->cmd.send_ussd_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.alpha.text);
      message_p->cmd.send_ussd_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text);
      message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text = NULL;
    }
    if ((message_p->cmd.send_ussd_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.icon.data);
      message_p->cmd.send_ussd_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_ussd_pro_cmd_req),
           0x00,
           sizeof(gstk_send_ussd_req_type));
    break;

  case GSTK_SETUP_CALL_REQ:
    if (message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text = NULL;
    }
    if (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text = NULL;
    }
    if ((message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
      (message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data);
      message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data = NULL;
    }
    if ((message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
      (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_call_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_call_req_type));
    break;

  case GSTK_SETUP_EVENT_LIST_REQ:
    /* internal */
    break;

  case GSTK_SETUP_MENU_REQ:
    if (message_p->cmd.setup_menu_pro_cmd_req.title.text != NULL) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.title.text );
      message_p->cmd.setup_menu_pro_cmd_req.title.text = NULL;
    }
    for (i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL) {
        gstk_free(message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text );
        message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text = NULL;
      }
    }
    if (message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             gstk_free(
               message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           gstk_free(
             message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
         }
      }
    }
    if ((message_p->cmd.setup_menu_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.icon.data);
      message_p->cmd.setup_menu_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_menu_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_menu_req_type));
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    if (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_idle_mode_txt_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_idle_mode_text_req_type));
    break;

  case GSTK_SEND_DTMF_REQ:
    if (message_p->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.alpha.text);
      message_p->cmd.send_dtmf_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf);
      message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf = NULL;
    }
    if ((message_p->cmd.send_dtmf_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.icon.data);
      message_p->cmd.send_dtmf_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_dtmf_pro_cmd_req),
           0x00,
           sizeof(gstk_send_dtmf_req_type));
    break;


  case GSTK_OPEN_CH_REQ:
    if (message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address);
      message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address);
      message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address = NULL;
    }
    if ((message_p->cmd.open_ch_pro_cmd_req.icon.present) &&
      (message_p->cmd.open_ch_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.icon.data);
      message_p->cmd.open_ch_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.open_ch_pro_cmd_req),
           0x00,
           sizeof(gstk_open_ch_req_type));
    break;

  case GSTK_CLOSE_CH_REQ:
    if (message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet);
      message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet = NULL;
    }
    if ((message_p->cmd.close_ch_pro_cmd_req.icon.present) &&
      (message_p->cmd.close_ch_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.close_ch_pro_cmd_req.icon.data);
      message_p->cmd.close_ch_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.close_ch_pro_cmd_req),
           0x00,
           sizeof(gstk_close_ch_req_type));
    break;

  case GSTK_SEND_DATA_REQ:
    if (message_p->cmd.send_data_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.alpha.alphabet);
      message_p->cmd.send_data_pro_cmd_req.alpha.alphabet = NULL;
    }
    if (message_p->cmd.send_data_pro_cmd_req.ch_data.data != NULL) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.ch_data.data);
      message_p->cmd.send_data_pro_cmd_req.ch_data.data = NULL;
    }
    if ((message_p->cmd.send_data_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_data_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.icon.data);
      message_p->cmd.send_data_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_data_pro_cmd_req),
           0x00,
           sizeof(gstk_send_data_req_type));
    break;

  case GSTK_RECEIVE_DATA_REQ:
    if (message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet);
      message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet = NULL;
    }
    if ((message_p->cmd.receive_data_pro_cmd_req.icon.present) &&
      (message_p->cmd.receive_data_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.receive_data_pro_cmd_req.icon.data);
      message_p->cmd.receive_data_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.receive_data_pro_cmd_req),
           0x00,
           sizeof(gstk_receive_data_req_type));
    break;


  /* Envelope Indication Responses */
  case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
    if (message_p->cmd.sms_pp_download_rsp.response_data_present) {
      if (message_p->cmd.sms_pp_download_rsp.ack.ack != NULL) {
        gstk_free(message_p->cmd.sms_pp_download_rsp.ack.ack);
        message_p->cmd.sms_pp_download_rsp.ack.ack = NULL;
      }
    }
    break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
  case GSTK_MO_SMS_CTRL_IND_RSP:
    if (message_p->cmd.mo_sms_cc_rsp.alpha.text != NULL) {
      gstk_free(message_p->cmd.mo_sms_cc_rsp.alpha.text);
      message_p->cmd.mo_sms_cc_rsp.alpha.text = NULL;
    }
    break;

  case GSTK_CC_IND_RSP:
    if (message_p->cmd.cc_rsp.cc_req_action.length != 0) {
      if (message_p->cmd.cc_rsp.cc_req_action.call_type == GSTK_USSD) {
        if (message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
          gstk_free(message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text);
          message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text = NULL;
        }
      }
      if (message_p->cmd.cc_rsp.cc_req_action.alpha.text != NULL) {
        gstk_free(message_p->cmd.cc_rsp.cc_req_action.alpha.text);
        message_p->cmd.cc_rsp.cc_req_action.alpha.text = NULL;
      }
    }
    break;

  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
  case GSTK_MENU_IND_RSP:
  case GSTK_TIMER_EXPIRE_IND_RSP:
    /* nothing to free */
    break;

  default:
    /* do nothing */
    break;
  }

} /* gstk_util_release_card_cmd_memory*/


/*===========================================================================
FUNCTION gstk_util_release_upper_layer_cmd_memory

DESCRIPTION
  This function frees all the allocated memory for the upper layer (U)SAT
  command response pointer input.

PARAMETERS
  message_p: [Input] Pointer to message to which memory location is required
                     to be freed

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
void gstk_util_release_upper_layer_cmd_memory(
    gstk_cmd_type * message_p )
{
    uint32                        type_of_command = 0;
    gstk_additional_info_ptr_type *addi_info_ptr  = NULL;

    if(message_p == NULL)
    {
      MSG_ERROR("message_p ERR:NULL",0,0,0);
      return;
    }

    /* Get the type of command */
    type_of_command = message_p->general_cmd.message_header.command_id;

    switch( type_of_command ) {

    case GSTK_SETUP_CALL_CNF:
      if (message_p->setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_call_term_rsp_cnf.result_additional_info;
      }
      /* all the mem alloc should be due to cc type change */
      if (message_p->setup_call_term_rsp_cnf.extra_param.has_cc_type_modification) {
        if (message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
          gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text);
          message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;
        }
        /* ussd_string and text_string2 are only valid when the calltype is changed to GSTK_USSD */
        if (message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
          if (message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
            message_p->setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
          if (message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text != NULL) {
            gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text);
            message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text = NULL;
          }
        }
        if (message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
      }
      break;

    case GSTK_SEND_SS_CNF:
      if (message_p->send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_call_term_rsp_cnf.result_additional_info;
      }
      /* all the mem alloc should be due to cc type change */
      if (message_p->send_ss_term_rsp_cnf.extra_param.has_cc_type_modification) {
        if (message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
          gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text);
          message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;
        }
        /* ussd_string and text_string2 are only valid when the calltype is changed to GSTK_USSD */
        if (message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
          if (message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
            message_p->send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
          if (message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text != NULL) {
            gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text);
            message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text = NULL;
          }
        }
        if (message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
      }
      break;

    case GSTK_SEND_USSD_CNF:
      if (message_p->send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_ussd_term_rsp_cnf.result_additional_info;
      }
      /* text is not dependent on whether there is a cc type change or not */
      if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
        gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
        message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
      }
      /* the rest are dependent on whether there is a cc type change or not */
      if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification) {
        if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text != NULL) {
          gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text);
          message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text = NULL;
        }
        if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.call_type == GSTK_USSD) {
          if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ussd_string.text);
            message_p->send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
        }
        if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
      }
      break;

    case GSTK_GET_INPUT_CNF:
      if (message_p->get_input_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_input_term_rsp_cnf.result_additional_info;
      }
      if (message_p->get_input_term_rsp_cnf.data != NULL) {
        gstk_free(message_p->get_input_term_rsp_cnf.data);
        message_p->get_input_term_rsp_cnf.data = NULL;
      }
      break;

    case GSTK_PROVIDE_LOCAL_INFO_CNF:
    case GSTK_PROVIDE_LANG_INFO_CNF:
      if (message_p->provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->provide_local_info_term_rsp_cnf.result_additional_info;
      }
      if(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_type == GSTK_NMR_UTRAN_INFO) {
        if(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr){
          gstk_free(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr);
          message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr = NULL;
        }
      }
      break;

    case GSTK_SEND_SMS_CNF:
      if (message_p->send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_sms_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_LAUNCH_BROWSER_CNF:
      if (message_p->launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->launch_browser_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_PLAY_TONE_CNF:
      if (message_p->play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->play_tone_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_DISPLAY_TEXT_CNF:
      if (message_p->display_text_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->display_text_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_GET_INKEY_CNF:
      if (message_p->get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_inkey_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SELECT_ITEM_CNF:
      if (message_p->select_item_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->select_item_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_MENU_CNF:
      if (message_p->setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_menu_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_IDLE_TEXT_CNF:
      if (message_p->setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_idle_text_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_REFRESH_CNF:
      if (message_p->refresh_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->refresh_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_MORE_TIME_CNF:
      if (message_p->more_time_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->more_time_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_POLL_INTERVAL_CNF:
    case GSTK_POLLING_OFF_CNF:
      if (message_p->poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->poll_interval_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_EVENT_LIST_CNF:
      if (message_p->setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_evt_list_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SEND_DTMF_CNF:
      if (message_p->send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_dtmf_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_LANG_NOTIFICATION_CNF:
      if (message_p->lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->lang_notification_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_PROVIDE_NW_SEARCH_MODE_CNF:
#ifdef FEATURE_CAT_REL6
      if (message_p->provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->provide_local_info_term_rsp_cnf.result_additional_info;
      }
#endif
      break;

    case GSTK_TIMER_MANAGEMENT_CNF:
      if (message_p->timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->timer_manage_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_OPEN_CH_CNF:
      if (message_p->open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->open_ch_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_CLOSE_CH_CNF:
      if (message_p->close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->close_ch_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SEND_DATA_CNF:
      if (message_p->send_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_data_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_RECEIVE_DATA_CNF:
      if (message_p->receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->receive_data_term_rsp_cnf.result_additional_info;
      }
      if (message_p->receive_data_term_rsp_cnf.ch_data.data != NULL) {
        gstk_free(message_p->receive_data_term_rsp_cnf.ch_data.data);
        message_p->receive_data_term_rsp_cnf.ch_data.data = NULL;
      }
      break;

    case GSTK_GET_CH_STATUS_CNF:
      if (message_p->get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_ch_status_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_RUN_AT_CMDS_CNF: /* later */
    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:
      /* nothing to free */
        break;

        /* Envelope command */
    case GSTK_MENU_SELECTION_IND:
    case GSTK_IDLE_SCRN_EVT_IND:
    case GSTK_USER_ACTIVITY_EVT_IND:
    case GSTK_BROWSER_TERMINATION_EVT_IND:
    case GSTK_LANG_SELECTION_EVT_IND:
    case GSTK_TIMER_EXPIRE_IND:
    case GSTK_MO_SMS_CTRL_IND:
      break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    case GSTK_SMS_PP_DOWNLOAD_IND:
      if (message_p->sms_pp_envelope_ind.tpdu.tpdu != NULL) {
        gstk_free(message_p->sms_pp_envelope_ind.tpdu.tpdu);
      }
      break;
    case GSTK_MT_CALL_EVT_IND:
      if (message_p->mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list);
      }
      break;
    case GSTK_CALL_CONNECTED_EVT_IND:
      if (message_p->call_connected_envelope_ind.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->call_connected_envelope_ind.transaction_id.transaction_list);
      }
      break;
    case GSTK_CALL_DISCONNECTED_EVT_IND:
      if (message_p->call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list);
      }
      if (message_p->call_disconnected_envelope_ind.call_disconnected_data.cause.cause != NULL) {
        gstk_free(message_p->call_disconnected_envelope_ind.call_disconnected_data.cause.cause);
      }
      break;

    case GSTK_LOCATION_STATUS_EVT_IND: /* more codeeeeeee, based on MM/RR/RRC interface */
      break;

    case GSTK_CC_IND:
      if (message_p->cc_envelope_ind.cc_data.call_enum_type == GSTK_USSD) {
        if (message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text != NULL) {
          gstk_free(message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text);
          message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text = NULL;
        }
      }
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      if (message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data != NULL) {
        gstk_free(message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data);
        message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data = NULL;
      }
      break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    default:
      break;
    }
    
    if (addi_info_ptr != NULL)
    {
      gstk_free(addi_info_ptr->additional_info_ptr);
      addi_info_ptr->additional_info_ptr = NULL;
      addi_info_ptr->length = 0;
    }
} /* gstk_util_release_upper_layer_cmd_memory*/

/*===========================================================================
FUNCTION gstk_util_translate_qualifier_to_dcs

DESCRIPTION
  This function translates the qualifier into the corresponding data
  coding scheme.

PARAMETERS
  qualifier: [Input] qualifier from the (U)SAT command

DEPENDENCIES
  None

RETURN VALUE
  byte -> data coding scheme

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint8 gstk_util_translate_qualifier_to_dcs(
   uint8                     qualifier )
{
  uint8 dcs;

  if ((qualifier & 0x02) == 0x02) {
     /* UCS2 */
     dcs = 0x08;
  }
  else {
     if (qualifier & 0x08) {
       /* packed sms */
       dcs = 0x00;
     }
     else {
       /* unpacked sms */
       dcs = 0x04;
     }
  }
  return dcs;
} /* gstk_util_translate_qualifier_to_dcs */


/*===========================================================================
FUNCTION gstk_util_build_evt_dl_ind

DESCRIPTION
  This function populates the gstk_task_cmd_type based on the passed in
  event download indication

PARAMETERS
  cmd: [Input/Output] Pointer to gstk_task_cmd_type to be populated
  env_ref_id: [Input] envelope_cmd_ref_table reference id
  evt_list: [Input] event download indication type from the client

DEPENDENCIES
  None

RETURN VALUE
  byte -> data coding scheme

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_build_evt_dl_ind(
  gstk_task_cmd_type          * cmd,
  uint32                        env_ref_id,
  const gstk_evt_dl_ind_type  * evt_list)
{
  MSG_HIGH("In build evt dl indication ", 0, 0, 0);
  if(cmd != NULL && evt_list != NULL) {
    switch(evt_list->evt_type) {

    case GSTK_IDLE_SCRN_EVT_IND:
      if ((gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE] & 0x01) == 0x01) {
        cmd->cmd.idle_scrn_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.idle_scrn_evt_envelope_ind.message_header, GSTK_IDLE_SCRN_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.idle_scrn_evt_envelope_ind.message_header.user_data = env_ref_id;
      }
      else {
        MSG_ERROR("Idle Screen Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;

    case GSTK_LOCATION_STATUS_EVT_IND:
      /* internal */
      cmd->cmd.location_st_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.location_st_envelope_ind.message_header, GSTK_LOCATION_STATUS_EVT_IND);
      /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.location_st_envelope_ind.message_header.user_data = env_ref_id;
      break;

    case GSTK_USER_ACTIVITY_EVT_IND:
      if ((gstk_evt_list[GSTK_USER_ACTIVITY] & 0x01) == 0x01) {
        cmd->cmd.user_act_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.user_act_evt_envelope_ind.message_header, GSTK_USER_ACTIVITY_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.user_act_evt_envelope_ind.message_header.user_data = env_ref_id;
      }
      else {
        MSG_ERROR("User Activity Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;

    case GSTK_LANG_SELECTION_EVT_IND:
      if ((gstk_evt_list[GSTK_LANGUAGE_SELECTION] & 0x01) == 0x01) {
        cmd->cmd.lang_sel_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.lang_sel_evt_envelope_ind.message_header, GSTK_LANG_SELECTION_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.lang_sel_evt_envelope_ind.message_header.user_data = env_ref_id;
        memcpy(cmd->cmd.lang_sel_evt_envelope_ind.lang_code.lang_code,
              evt_list->evt_ind.lang_sel_evt_ind.lang_code,
              2);
      }
      else {
        MSG_ERROR("Language Sel Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;

    case GSTK_BROWSER_TERMINATION_EVT_IND:
      if ((gstk_evt_list[GSTK_BROWSER_TERMINATION] & 0x01) == 0x01) {
        if ((cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause != GSTK_BROWSER_USER_TERMINATED) &&
            (cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause != GSTK_BROWSER_ERROR_TERMINATED)) {
            MSG_ERROR("Invalid Browser Term Cause 0x%x",
              cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause, 0, 0);
            return GSTK_ERROR;
        }
        cmd->cmd.browser_term_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.browser_term_evt_envelope_ind.message_header, GSTK_BROWSER_TERMINATION_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.browser_term_evt_envelope_ind.message_header.user_data = env_ref_id;
        cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause =
                                                                    evt_list->evt_ind.browser_terminate_evt_ind;
      }
      else {
        MSG_ERROR("Browser Term Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;

    case GSTK_DATA_AVAIL_EVT_IND:
      if ((gstk_evt_list[GSTK_DATA_AVAILABLE] & 0x01) == 0x01) {
        cmd->cmd.data_avail_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.data_avail_evt_envelope_ind.message_header, GSTK_DATA_AVAIL_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.data_avail_evt_envelope_ind.message_header.user_data = env_ref_id;
        /* Channel Status Info parameter checks */
        if((evt_list->evt_ind.data_avail_evt_ind.ch_status.ch_id < 0x01) ||
           (evt_list->evt_ind.data_avail_evt_ind.ch_status.ch_id > 0x07))
        {
          MSG_ERROR("Invalid Channel Id", 0, 0, 0);
          return GSTK_ERROR;
        }
        memcpy(&(cmd->cmd.data_avail_evt_envelope_ind.ch_status_info),
          &(evt_list->evt_ind.data_avail_evt_ind.ch_status),
          sizeof(gstk_ch_status_type));
        cmd->cmd.data_avail_evt_envelope_ind.ch_data_len =
          evt_list->evt_ind.data_avail_evt_ind.ch_len;
      }
      else {
        MSG_ERROR("Data Available Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;
    case GSTK_CH_STATUS_EVT_IND:
      if ((gstk_evt_list[GSTK_CHANNEL_STATUS] & 0x01) == 0x01) {
        cmd->cmd.ch_status_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        gstk_set_command_id(&cmd->cmd.ch_status_evt_envelope_ind.message_header, GSTK_CH_STATUS_EVT_IND);
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        cmd->cmd.ch_status_evt_envelope_ind.message_header.user_data = env_ref_id;
        /* Channel Status Info parameter checks */
        if((evt_list->evt_ind.ch_status_evt_ind.ch_id < 0x01) ||
           (evt_list->evt_ind.ch_status_evt_ind.ch_id  > 0x07))
        {
          MSG_ERROR("Invalid Channel Id", 0, 0, 0);
          return GSTK_ERROR;
        }
        memcpy(&(cmd->cmd.ch_status_evt_envelope_ind.ch_status_info),
          &(evt_list->evt_ind.ch_status_evt_ind),
          sizeof(gstk_ch_status_type));
      }
      else {
        MSG_ERROR("Channel Status Evt is not set", 0, 0, 0);
        return GSTK_ERROR;
      }
      break;

    default:
      MSG_MED("Unknown EVT Download", 0, 0, 0);
      return GSTK_ERROR;
    }
    return GSTK_SUCCESS;
  }
  else { /* one of the param is null */
    return GSTK_NULL_INPUT_PARAM;
  }
} /* gstk_util_build_evt_dl_ind */


/*===========================================================================
FUNCTION gstk_util_map_mm_to_gstk_location_status

DESCRIPTION
  This function will map the location status from MM to GSTK enum type.



PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type

  location_status: [Input/Output] pointer to location status

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
gstk_status_enum_type gstk_util_map_mm_to_gstk_location_status(
  const gstk_mm_idle_state_type   * mm_idle_cmd,
  gstk_location_status_enum_type  * location_status)
{
  if(mm_idle_cmd == NULL)
  {
    MSG_ERROR("mm_idle_cmd ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(location_status == NULL)
  {
    MSG_ERROR("location_status ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  switch(mm_idle_cmd->location_state) {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service                             */
      MSG_HIGH("no service", 0, 0, 0);
      *location_status = GSTK_NO_SERVICE;
      break;
    case SYS_SRV_STATUS_LIMITED:
      /* Limited service                        */
      MSG_HIGH("limited service", 0, 0, 0);
      *location_status = GSTK_LIMITED_SERVICE;
      break;
    case SYS_SRV_STATUS_SRV:
      /* Service available                      */
      MSG_HIGH("normal service", 0, 0, 0);
      *location_status = GSTK_NORMAL_SERVICE;
      break;
    default:
      MSG_ERROR("Location Status from MM unknown %x", mm_idle_cmd->location_state, 0, 0);
      return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_util_map_mm_to_gstk_location_status */

/*===========================================================================
FUNCTION gstk_util_is_valid_location_info

DESCRIPTION
  This function will check if the MNC, MCC, LAC, CellID and location
  status info is valid combination.

PARAMETERS
  mcc_mnc: [Input]
  lac: [Input]
  cell_id: [Inputt]
  location_status: [Input]

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_valid_location_info(
  const uint8  * mcc_mnc,
  const uint8  * lac,
  const uint8  * cell_id, /*lint -esym(715, cell_id) */
                                        gstk_location_status_enum_type  location_status)
{
  uint8 gstk_bad_mcc_mnc[3];
  uint8 gstk_bad_lac[2];

  if((!mcc_mnc) || (!lac) || (!cell_id))
  {
    MSG_ERROR("LAC/MCC_MNC/CELL_ID ptr ERR:NULL",0,0,0);
    return FALSE;
  }

  memset(gstk_bad_mcc_mnc, 0xFF, 3);
  memset(gstk_bad_lac, 0xFF, 2);

  if (location_status == GSTK_NORMAL_SERVICE) {
    if (!(memcmp(gstk_bad_mcc_mnc, mcc_mnc, 3))) {
      MSG_ERROR("Normal Svc with MCC = 0xFF", 0, 0, 0);
      return FALSE;
    }
    if (!(memcmp(gstk_bad_lac, lac, 2))) {
      MSG_ERROR("Normal Svc with MCC = 0xFF", 0, 0, 0);
      return FALSE;
    }
  }
  return TRUE;
} /* gstk_util_is_valid_location_info */

/*===========================================================================
FUNCTION gstk_util_check_mm_state_difference

DESCRIPTION
  This function will get the cell id and determines whether there are any
  differences between the new location info/state with the old new.  This
  function will return cell_id as a result of the operation.

PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type
  cell_id: [Input] pointer to cell id to be populated
  location_status: [Input] location status

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_check_mm_state_difference(
  const uint8                     * mcc_mnc,
  const uint8                     * lac,
  const uint8                     * cell_id,
  const int32                       cell_len,
  gstk_location_status_enum_type    location_status)
{
#ifndef FEATURE_GSTK_FOR_NON_LTK_BUILD
  /* location_status:  New Location Status - Set by MM
  ** gstk_next_curr_location_status: Current Location Status set by GSTK (earlier)
  */
  MSG_HIGH("NEW LS: 0x%x OLD LS: 0x%x",location_status,
           gstk_next_curr_location_status,0);

  if ((mcc_mnc == NULL) ||
      (lac == NULL) ||
      (cell_id == NULL))
  {
    MSG_ERROR("Null Input pointer", 0, 0, 0);
    return FALSE;
  }

  /* compare location state */
  if(location_status != gstk_next_curr_location_status) {
    MSG_HIGH("NO MATCH LS:  NEW: 0x%x OLD: 0x%x",location_status,gstk_next_curr_location_status,0);
    return TRUE;
  }

  /* To avoid sending down multiple NO Server LS Events,
  ** check to see if status is GSTK_NO_SERVICE.  If it is
  ** there is no difference. */
  if (location_status == GSTK_NO_SERVICE) {
    MSG_HIGH("NEW: NO SVC  OLD: NO SVC - No LS EVT",0,0,0);
    return FALSE;
  }

  /* compare cell id */
  if (cell_len != gstk_next_curr_cell_id.cell_len) {
    MSG_HIGH("NO MATCH CELL ID LEN: NEW: 0x%x OLD: 0x%x",cell_len,gstk_next_curr_cell_id.cell_len,0);
    return TRUE;
  }

  /* Same cell Len */
  if (memcmp(cell_id, gstk_next_curr_cell_id.cell_id, (uint32)cell_len)) {
    MSG_HIGH("NO MATCH CELL ID",0,0,0);
    return TRUE;
  }

  /* compare mcc, mnc */
  if(memcmp(mcc_mnc, gstk_next_curr_mcc_mnc, 3)) {
    MSG_HIGH("NO MATCH MCC MNC",0,0,0);
    return TRUE; /* mcc mnc is different */
  }

  /* compare lac */
  if(memcmp(lac, gstk_next_curr_lac, 2)) {
    MSG_HIGH("NO MATCH LAC",0,0,0);
    return TRUE; /* lac is different */
  }

#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

  return FALSE;
} /* gstk_util_check_mm_state_difference */


/*===========================================================================
FUNCTION gstk_util_compare_curr_and_next_location_info

DESCRIPTION
  This function will compare the previously sent location info with
  the one in the next available one

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_compare_curr_and_next_location_info( void )
{
  /* compare location state */
  if (gstk_curr_location_status != gstk_next_curr_location_status) {
    return TRUE;
  }

  if (gstk_next_curr_location_status != GSTK_NORMAL_SERVICE) {
    /* no need to check further, no difference */
    return FALSE;
  }

  /* compare cell id */
  if(memcmp(&gstk_curr_cell_id, &gstk_next_curr_cell_id, sizeof(gstk_cell_id_type))) {
    return TRUE;
  }

  /* compare MCC, MNC */
  if(memcmp(gstk_curr_mcc_mnc, gstk_next_curr_mcc_mnc, 3))
  {
    return TRUE; /* mcc mnc is different */
  }

  /* compare lac */
  if(memcmp(gstk_curr_lac, gstk_next_curr_lac, 2)) {
    return TRUE; /* lac is different */
  }

  return FALSE;
} /* gstk_util_compare_curr_and_next_location_info */


/*===========================================================================
FUNCTION gstk_util_pack_7_bit_chars

DESCRIPTION
  This function pack the input data into SMS 7 bit format.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint16:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint16 gstk_util_pack_7_bit_chars
(
  const uint8     * in,
  uint16          in_len, //number of chars
  uint8           * out
)
{
  uint16    i=0;
  uint16    pos   = 0;
  uint8     shift = 0;

  if(in == NULL)
  {
    MSG_ERROR("in ERR:NULL",0,0,0);
    return pos;
  }

  if(out == NULL)
  {
    MSG_ERROR("out ERR:NULL",0,0,0);
    return pos;
  }

  /* pack the ASCII characters
  */

  for( pos=0;
       i < in_len;
       pos++, i++ )
  {
    /* pack the low bits */
    out[pos] = in[i] >> shift;

    if( i+1 < in_len )
    {
      /* pack the high bits using the low bits of the next character */
      out[pos] |= (uint8) (in[i+1] << (7-shift));

      shift ++;

      if( shift == 7 )
      {
        shift = 0;
        i ++;
      }
    }
  }

  /* done */
  return pos;

} /* gstk_util_pack_7_bit_chars */


/*===========================================================================
FUNCTION gstk_util_unpack_7_bit_chars

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint8:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint8 gstk_util_unpack_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,
  uint8             * out
)
{
  int      i=0;
  uint16   pos=0;
  uint16   shift = 0;

  if(in == NULL)
  {
    MSG_ERROR("In ERR:NULL",0,0,0);
    return (uint8)pos;
  }

  if(out == NULL)
  {
    MSG_ERROR("Out ERR:NULL",0,0,0);
    return (uint8)pos;
  }

  for( i=i;
       pos < in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;

    }
  }

  while((i >= 0) && (out[i] == 0x00))
  {
    i -= 1;
  }

  /* i above atarts from 1 and it is necessary to do i+1 to accomodate the
     length including the 0th location */
  return(uint8)(i+1);

} /* gstk_util_unpack_7_bit_chars() */

#ifdef FEATURE_GSTK_DUAL_SLOTS

/*===========================================================================
FUNCTION gstk_util_check_proactive_card

DESCRIPTION
  This function checks whether or not the card in the given slot is proactive.

PARAMETERS
  slot: [Input] card slot

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if the card in the given slot is proactive,
  FALSE otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_check_proactive_card (
    gstk_slot_id_enum_type slot )
{
    uint32 error_mask;
    uint32 init_mask;
    uint32 toolkit_cap_mask;
    switch (slot) {
    case GSTK_SLOT_1:
        error_mask = GSTK_SLOT1_CARD_ERROR_MASK;
        init_mask = GSTK_SLOT1_INITIALIZED_MASK;
        toolkit_cap_mask = GSTK_SLOT1_TK_CAPABLE_MASK;
        break;
    case GSTK_SLOT_2:
        error_mask = GSTK_SLOT2_CARD_ERROR_MASK;
        init_mask = GSTK_SLOT2_INITIALIZED_MASK;
        toolkit_cap_mask = GSTK_SLOT2_TK_CAPABLE_MASK;
        break;
    default:
        MSG_ERROR("Unknown slot %x", slot, 0, 0);
        return FALSE;
    }
    /* the slot has toolkit capability */
    if (((gstk_toolkit_info & error_mask) != error_mask) &&
        ((gstk_toolkit_info & init_mask) == init_mask) &&
        ((gstk_toolkit_info & toolkit_cap_mask) == toolkit_cap_mask))
    {
        return TRUE;
    }
    return FALSE;
} /* gstk_util_check_proactive_card */

/*===========================================================================
FUNCTION gstk_util_determine_new_pref_from_mode_chg

DESCRIPTION
  This function determines the new preferred slot from the mode change new
  slot info.

PARAMETERS
  new_mode: [Input] new mode that we are switching to

DEPENDENCIES
  None

RETURN VALUE
  gstk_slot_id_enum_type for the slot info as a result of the mode change
  command

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_slot_id_enum_type gstk_util_determine_new_pref_from_mode_chg(gstk_mode_enum_type new_mode)
{
    uim_slot_type uim_slot;
    /* get new slot using CM mode change info */
    switch (new_mode) {
    case GSTK_CDMA_MODE:
#ifdef FEATURE_GSTK_STATE_TESTER
        uim_slot = gstk_state_tester_get_slot_for_app();
#else
        uim_slot = uim_get_slot_for_app(UIM_CDMA_APP);
#endif /* FEATURE_GSTK_STATE_TESTER */
        break;
    case GSTK_GSM_MODE:
#ifdef FEATURE_GSTK_STATE_TESTER
        uim_slot = gstk_state_tester_get_slot_for_app();
#else
        uim_slot = uim_get_slot_for_app(UIM_GSM_APP);
#endif /*FEATURE_GSTK_STATE_TESTER */
        break;
    default:  /* all other modes */
        return GSTK_SLOT_NONE;
    }
    if (uim_slot == UIM_SLOT_1) {
        return GSTK_SLOT_1;
    }
    else if (uim_slot == UIM_SLOT_2) {
        return GSTK_SLOT_2;
    }
    else {
      return GSTK_SLOT_NONE;
    }
} /* gstk_util_determine_new_pref_from_mode_chg */


/*===========================================================================
FUNCTION gstk_util_handle_mode_change

DESCRIPTION
  This function determine new preferred slot based on new mode from mode
  change command, and process the start toolkit or mode switch process
  accordingly

PARAMETERS
  new_mode: [Input] new mode that we received from mode switch cmd

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
void gstk_util_handle_mode_change(gstk_mode_enum_type new_mode)
{
    gstk_slot_id_enum_type new_mode_chg_slot = GSTK_SLOT_NONE;
    gstk_status_enum_type  gstk_status = GSTK_SUCCESS;

    new_mode_chg_slot = gstk_util_determine_new_pref_from_mode_chg(new_mode);

    if (gstk_util_check_proactive_card(new_mode_chg_slot)) {
        /* new slot has tk capability */
        gstk_preferred_tk_slot = new_mode_chg_slot;
    }
    if (new_mode_chg_slot != GSTK_SLOT_NONE) {
        if (gstk_curr_tk_slot == GSTK_SLOT_NONE) {
            /* determine tk capability on the new slot */
            if (gstk_util_check_proactive_card(new_mode_chg_slot)) {
                /* no tp has been sent down yet */
                gstk_status = gstk_start_toolkit_session();
                if (gstk_state != GSTK_SUCCESS) {
                  MSG_HIGH("Toolkit TP unsuccessful", 0, 0, 0);
                }
            }
            else {
                /* flag an error message when
                1. the new slot is not toolkit capable
                2. curr tk == NONE => no TP sent yet => hasn't found good info on either card yet
                3. but pref tk != NONE!  */
                if (gstk_preferred_tk_slot != GSTK_SLOT_NONE) {
                  MSG_ERROR("Haven't sent down any TP and pref slot = %x != NONE", gstk_preferred_tk_slot, 0, 0);
                }
            }
        }
        else { /* something has been sent down but we are still in GSDI_SIG_WAIT */
          MSG_HIGH("A TP has been sent down b4 curr: %x, new: %x",
                          gstk_curr_tk_slot, new_mode_chg_slot, 0);

            /* sent down mode change command even if the new mode does not have tk capability,
                this will allow us to "reset" curr back to NONE */
            gstk_status = gstk_process_mode_change(new_mode_chg_slot);
            if (gstk_status != GSTK_SUCCESS) {
                MSG_MED("Mode Change ind ERR, %x", gstk_status, 0, 0);
            }
        }
    }
} /* gstk_util_handle_mode_change */


#endif /* FEATURE_GSTK_DUAL_SLOTS */

/*===========================================================================
FUNCTION gstk_util_has_pending_timer_envelope

DESCRIPTION
  This function based on the timer management table, determine if there is
  any pending timer expiration envelope that needs to be sent to the card

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_has_pending_timer_envelope(void)
{
  int i = 0;
  for (i = 0; i < GSTK_MAX_TIMER_SUPPORTED; i++) {
    if (gstk_timer_management[i].pending &&
        !gstk_timer_management[i].waiting_rsp) {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_util_has_pending_timer_envelope */



/*===========================================================================
FUNCTION gstk_util_find_first_timer_expire_command

DESCRIPTION
  This function find the first pending timer expire command available.

PARAMETERS
  None

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
uint8 gstk_util_find_first_timer_expire_command(void)
{
  uint8 i;
  for (i = 0; i < GSTK_MAX_TIMER_SUPPORTED; i++) {
    if (gstk_timer_management[i].pending &&
        !gstk_timer_management[i].waiting_rsp) {
      return i;
    }
  }
  return GSTK_MAX_TIMER_SUPPORTED;
} /* gstk_util_find_first_timer_expire_command */


/*===========================================================================
FUNCTION gstk_util_find_first_non_timer_expire_postpone_env_command

DESCRIPTION
  This function find the first non timer expire postpone envelope command if
  exists.

PARAMETERS
  None

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
uint8 gstk_util_find_first_non_timer_expire_postpone_env_command(void)
{
  uint8 i;
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    /* the location is not free */
    if(!envelope_cmd_ref_table[i].free) {
      /* if the envelope is a call connected evt */
      if (envelope_cmd_ref_table[i].expected_cmd_rsp_type ==
          GSTK_CALL_CONNECTED_EVT_IND_RSP) {
        return i;
      }
    }
  }
  return GSTK_MAX_PROACTIVE_COMMAND;
} /*gstk_util_find_first_non_timer_expire_postpone_env_command */



/*===========================================================================
FUNCTION gstk_util_has_pending_setup_call

DESCRIPTION
  This function find if there is any pending setup call proactive command

PARAMETERS
  None

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
boolean gstk_util_has_pending_setup_call(void)
{
  int i = 0;
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    /* the location is not free */
    if(!command_details_ref_table[i].free_slot) {
      /* if the envelope is a call connected evt */
      if (command_details_ref_table[i].expected_cmd_rsp_type ==
          GSTK_SETUP_CALL_CNF) {
        return TRUE;
      }
    }
  }
  return FALSE;
} /* gstk_util_has_pending_setup_call */

#if defined (FEATURE_MMGSDI_GSTK_TEST_AUTOMATION) || defined (FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP)
/*===========================================================================
FUNCTION gstk_util_dereg_client_type

DESCRIPTION
  This function dereg events for certain client type.  dereg_evt indicates
  the events to be removed from the client's list

PARAMETERS
  client_type: [Input] Specifies the client type
  dereg_evt:   [Input] Specifies the events to be deregistered

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  When the client type has no more event that it is registered for, the
  client will be removed from the gstk_client_table

SEE ALSO
  None
===========================================================================*/
void gstk_util_dereg_client_type(gstk_client_type client_type, uint32 dereg_evt)
{
  int i = 0;

  for (i = 0; i<GSTK_MAX_CLIENT; i++) {
    if (gstk_client_table[i].client_type == client_type) {
      gstk_client_table[i].evt_reg_bitmask &= ~dereg_evt;
      if (gstk_client_table[i].evt_reg_bitmask == 0) {
        /* no more registered event */
        gstk_client_table[i].free = TRUE;
        gstk_client_table[i].client_cb = NULL;
        gstk_client_table[i].client_type = GSTK_NO_TYPE;
        gstk_client_table[i].user_data = 0;
      }
    }
  }
} /* gstk_util_dereg_client_type*/
#endif /*FEATURE_MMGSDI_GSTK_TEST_AUTOMATION || FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP*/

/*===========================================================================
FUNCTION gstk_util_decode_dcs_type

DESCRIPTION
  This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
  on whether it is a cell broadcast dcs or not

PARAMETERS
  is_cb_dcs - This flag is used for differentiating if the dcs is for 
              ussd or not
  cs        - data coding scheme

DEPENDENCIES
  None

RETURN VALUE
  gstk_dcs_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_dcs_enum_type gstk_util_decode_dcs_type(boolean is_cb_dcs, uint8 dcs)
{
  if (!is_cb_dcs) {
    if ((dcs & GSTK_SMS_GENERAL_DATA_CODING_MASK) == GSTK_SMS_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_SMS_GENERAL_DATA_CODING_MASK) == GSTK_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_UCS2_8_BIT;
    }
    if ((dcs & GSTK_SMS_DATA_CODING_MASK) == GSTK_SMS_8_BIT_DCS) {
      return GSTK_DCS_8_BIT;
    }
  }
  else {
    /* Decode according to 23.038 */
    if ((dcs & GSTK_CBS_DATA_CODING_MASK) == GSTK_CBS_8_BIT_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_CBS_GENERAL_DATA_CODING_MASK) == GSTK_CBS_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_CBS_GENERAL_DATA_CODING_MASK) == GSTK_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_UCS2_8_BIT;
    }
    if ((dcs & GSTK_CBS_UCS2_7_BIT_DCS) == GSTK_CBS_UCS2_7_BIT_DCS) {
      return GSTK_DCS_UCS2_7_BIT;
    }
  }
  return GSTK_DCS_7_BIT;
}

/*===========================================================================
FUNCTION gstk_util_is_valid_channel_id_range

DESCRIPTION
  This function determines if the channel id provided by the SIM is within
  valid range

PARAMETERS
  device_id

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_valid_channel_id_range(device_identity_e device_id)
{
  if (((int)device_id > GSTK_CHANNEL_ID_BASE) && ((int)device_id <= GSTK_CHANNEL_ID_MAX_VALUE)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
} /* gstk_util_is_valid_channel_id_range */

/*===========================================================================
FUNCTION gstk_util_unpack_7_bit_chars_ext

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII
  and returns the actual data length without the null terminated byte.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  int32:length of output data length
  If this function returns negative value, it should be considered as 
  an error.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int32 gstk_util_unpack_7_bit_chars_ext
(
  const uint8       * in,
  int32             in_len,
  uint16            * out
)
{
  int      i=0;
  uint16   pos=0;
  uint16   shift = 0;

  if(in == NULL)
  {
    MSG_ERROR("in ERR:NULL",0,0,0);
    return pos;
  }

  if(out == NULL)
  {
    MSG_ERROR("out ERR:NULL",0,0,0);
    return pos;
  }

  for( i=i;
       pos < in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;

    }
  }

  return (i);

} /* gstk_util_unpack_7_bit_chars_ext() */

/*===========================================================================
FUNCTION gstk_util_find_apn_len

DESCRIPTION
  This function returns the total length of the APN. This is needed to determine
  the no of extra bytes that needs to be allocated for label separators '.'
  and the null bytes.


PARAMETERS
  uint8* apn_bytes - Byte array containing the APN
  int apn_length   - Length of the APN Byte Array
  int *cnt         - New length that includes the '.' and the nulll byte.


DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS
  GSTK_ERROR

===========================================================================*/
gstk_status_enum_type gstk_util_find_apn_len(
  const uint8 * apn_bytes,
  int           apn_length,
  int         * cnt)
{
  int i = 0;
  if(apn_bytes == NULL)
  {
    MSG_ERROR("apn_bytes ERR:NULL",0,0,0);
    return GSTK_ERROR;
  }

  if(cnt == NULL)
  {
    MSG_ERROR("cnt ERR:NULL",0,0,0);
    return GSTK_ERROR;
  }

  *cnt = 0;
  while(i < apn_length)
  {
    /* Adding one byte for the '.'
    ** No need for the label_len to be taken into account.
    */
    *cnt = *cnt + apn_bytes[i] + 1;

    /* Adding an additional byte for the label_len itself */
    i = i + apn_bytes[i] + 1;
  }
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_util_check_for_apn_net_op_id

DESCRIPTION
  This function determines whether the label is part of the APN Network Id
  or APN Operator Id.

PARAMETERS
  char *apn_bytes - APN
  int apn_len     - Length
  int pos         - Start position
  int apn_val     - GSTK_APN_NET_ID/GSTK_APN_OP_ID

DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS
  GSTK_ERROR


===========================================================================*/
gstk_status_enum_type gstk_util_check_for_apn_net_op_id(
  const char * apn_bytes,
  int          apn_len,
  int        * apn_val)
{

  unsigned int label_len = 0;
  int ind = 0;
  char tmp[5] = "gprs";
  char tmp1[4] = "mnc";
  char tmp2[4] = "mcc";
  char tmp3[5];

  if((apn_bytes == NULL) || (apn_val == NULL))
    return GSTK_ERROR;

  label_len = apn_bytes[ind];
  if(label_len < GSTK_MIN_OP_LABEL_LEN)
  {
    *apn_val = GSTK_APN_NET_ID;
    return GSTK_SUCCESS;
  }

  if(label_len < strlen(tmp1))
  {
    *apn_val = GSTK_APN_NET_ID;
    return GSTK_SUCCESS;
  }
  else
  {
    ind = ind + uint32toint32(label_len) + 1;

    if(ind > apn_len)
      return GSTK_ERROR;

    label_len = apn_bytes[ind];
    if(label_len  < strlen(tmp2))
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }
  }

  if(label_len >= GSTK_MIN_OP_LABEL_LEN && label_len <= GSTK_MAX_OP_LABEL_LEN)
  {
    ind = 0;
    label_len = apn_bytes[ind];
    ind++;
    if(ind > apn_len)
      return GSTK_ERROR;
    memset(tmp3,0x00,5*sizeof(char));

    memcpy(tmp3,&apn_bytes[ind],3);
    tmp3[3] = '\0';
     /* No match with 'mnc' */
    if(strcmp(tmp3,tmp1) != 0)
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }

    ind = ind + uint32toint32(label_len);
    if(ind > apn_len)
      return GSTK_ERROR;

    label_len =  apn_bytes[ind];
    ind++;
    memset(tmp3,0x00,5*sizeof(char));
    memcpy(tmp3,&apn_bytes[ind],3);
    tmp3[3] = '\0';

    /* No match with 'mcc' */
    if(strcmp(tmp3,tmp2) != 0)
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }

    ind = ind + uint32toint32(label_len);

    if(ind > apn_len)
      return GSTK_ERROR;

    label_len =  apn_bytes[ind];
    ind++;
    memset(tmp3,0x00,5*sizeof(char));
    memcpy(tmp3,&apn_bytes[ind],4);
    tmp3[4] = '\0';

    /* No match with 'gprs' */
    if(strcmp(tmp3,tmp) == 0)
    {
      *apn_val = GSTK_APN_OP_ID;
      return GSTK_SUCCESS;
    }
    /* Match with 'mnc' and 'mcc' and 'gprs'*/
  }
  *apn_val = GSTK_APN_NET_ID;
  return GSTK_SUCCESS;
} /* gstk_util_check_for_apn_net_op_id */

/*===========================================================================
FUNCTION gstk_util_extract_label_len_value

DESCRIPTION
  Function used by GSTK to extract label and field value from the APN

PARAMETERS
  int32          *alphabet_in     - input buffer of type uint8
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32

===========================================================================*/
gstk_status_enum_type gstk_util_extract_label_len_value(
  int32                apn,
  const uint8        * alphabet_in,
  uint16             * alphabet_out,
  int                * ind ,
  int32              * count,
  int                * iter
)
{
  int i = 0;
  int j = 0;
  int cnt = 0;
  int lb_len = 0;
  boolean match = 0;
  char tmp[5];
  int k = 0;
  uint8* temp;
  int init = 0;
  int val = 0;
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  if((alphabet_in == NULL)|| (alphabet_out == NULL) ||
    (count == NULL) || (ind == NULL) || (iter == NULL))
  {
    return GSTK_ERROR;
  }
  /*
  Extract the length portion of the label
  */
  j=0;
  lb_len = alphabet_in[j];

  memset(&(alphabet_out[*ind]), 0x00, sizeof(uint16)*int32touint32(lb_len));

  if(lb_len == 0)
  {
    *ind = *ind + alphabet_in[j] + 1;
    *iter = *iter + alphabet_in[j] + 1;
    *count = *count + alphabet_in[j] + 1;
    return GSTK_SUCCESS;
  }
  if(lb_len < 0)
  {
    return GSTK_ERROR;
  }
  /*
  If the length is greater than zero, depending on whether the APN_NET_OP
  is APN_NET/APN_OP, invoke the function to make a check for strings not to
  be used - rac, lac, sgsn,
  */
  temp = gstk_malloc(int32touint32(lb_len+1));
  if(temp == NULL)
  {
    MSG_ERROR("Mem-malloc for nw access name fail", 0, 0, 0);
    return GSTK_ERROR;
  }
  memcpy(temp,&alphabet_in[j+1],int32touint32(lb_len));
  temp[lb_len]= 0x00;

  /*
  ** APN - NETWORK IDENTIFIER
  */
  switch(apn)
  {
  case GSTK_APN_NET_ID:
    /*
    ** Make a check for strings like rac, lac, sgsn
    */
    for (cnt=0; cnt <gstk_start_str_cnt; cnt++ )
    {
      gstk_status = gstk_util_compareString((int)GSTK_SEARCH_BEGIN_STR,
                                            gstk_apn_start_str_table[cnt],
                                            (char *)temp,&match);
      if(gstk_status == GSTK_ERROR)
      {
        gstk_free(temp);
        temp = NULL;
        return GSTK_ERROR;
      }

      if(match)
      {
        gstk_free(temp);
        temp = NULL;
        MSG_ERROR("APN NET ID Cannot contain string %s",gstk_apn_start_str_table[cnt],0, 0);
        return GSTK_ERROR;
      }
    }
    for (cnt=0; cnt < gstk_end_str_cnt; cnt++ )
    {
      gstk_status = gstk_util_compareString((int)GSTK_SEARCH_END_STR,
                                            gstk_apn_end_str_table,
                                            (char *)temp,
                                            &match);
      if(gstk_status == GSTK_ERROR)
      {
        gstk_free(temp);
        temp = NULL;
        return GSTK_ERROR;
      }

      if(match)
      {
        gstk_free(temp);
        temp = NULL;
        MSG_ERROR("APN OP ID Cannot contain string %s",gstk_apn_end_str_table , 0, 0);
        return GSTK_ERROR;
      }
    }
    /* Check for the presence of a  * */
    gstk_status = gstk_util_compareString((int)GSTK_SEARCH_SINGLE_CHAR, "*", (char *)temp, &match);

    if(gstk_status == GSTK_ERROR)
    {
      gstk_free(temp);
      temp = NULL;
      return GSTK_ERROR;
    }

    if(match)
    {
      gstk_free(temp);
      temp = NULL;
      MSG_ERROR("APN NET ID Cannot contain string *",0 , 0, 0);
      return GSTK_ERROR;
    }
    /* Copy the data from input to output */
    /*label_len */
    for( i = *ind,j=1; j <= alphabet_in[0] && i <= (*ind+lb_len);j++,i ++)
    {
      alphabet_out[i] =  alphabet_in[j];
    }
    alphabet_out[i] = 0x2e;

    *ind = *ind + alphabet_in[0] + 1;
    *iter = *iter + alphabet_in[0] + 1;
    *count = *count + alphabet_in[0] + 1;

    return GSTK_SUCCESS;
    /*
    **  APN - OPERATOR IDENTIFIER
    */
  case GSTK_APN_OP_ID:
    /* Copy the data from input to output */
    /*label_len */
    while(val < 3)
    {
      memset(tmp,0x00,sizeof(char)*5);
      lb_len = alphabet_in[init];
      memcpy(tmp,&alphabet_in[init+1],3);
      tmp[3] = 0x00;
      if((strcmp(tmp,"mnc") == 0) || (strcmp(tmp,"mcc") == 0))
      {
        for( i = *ind, j = init+1; i <= (*ind+3) && j <= init+3; i ++, j++)
        {
          alphabet_out[i] =  alphabet_in[j];
        }
        for(j=*ind + GSTK_MAX_OP_LABEL_LEN, k = init+lb_len; (j> 0) && (k > init+3);k--,j--)
        {
          alphabet_out[j] = alphabet_in[k];
        }
        alphabet_out[*ind+7] = 0x2e;
        /* Add a byte for the '.' and another byte to move to the next location */
        *ind = *ind + GSTK_MAX_OP_LABEL_LEN + 1 + 1;
      }
      else /* If the string is gprs */
      {
        for( i = *ind,j=init; j <= init+lb_len && i <= (*ind+lb_len);j++,i ++)
        {
          alphabet_out[i] =  alphabet_in[j];
        }
        /* Add a byte for the label length */
        *ind = *ind + GSTK_MIN_OP_LABEL_LEN + 1;
      }
      *iter = *iter + alphabet_in[init] + 1;
      *count = *count + alphabet_in[init] + 1;
      val++;
      init = init + lb_len + 1;
    }/* end of while */
    return GSTK_SUCCESS;
  default:
    gstk_free(temp);
    temp = NULL;
    return GSTK_ERROR;
  }/* switch(apn) */
} /* gstk_util_extract_label_len_value */

/*===========================================================================
FUNCTION gstk_util_compareString

DESCRIPTION
  Generic Function used by GSTK to compare two strings/substrings

PARAMETERS
  int pos      - Start position
  char * src   - String1 to be compared.
  char * dest  - String2 to be compared.

DEPENDENCIES

RETURN VALUE
  static int32

===========================================================================*/
gstk_status_enum_type gstk_util_compareString(
    int          pos,
    const char * src,
    const char * dest,
    boolean    * match
                                              )
{
  int i = 0;
  int j = 0;
  int cnt = 0;
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  /* Case to match other strings like rac, lac, sgsn, gprs */
  if(src == NULL || dest == NULL || match == NULL)
    return GSTK_ERROR;

  if((strlen(dest) < strlen(src)) && (pos != (int)GSTK_SEARCH_SINGLE_CHAR))
  {
    *match = FALSE;
    return GSTK_SUCCESS;
  }

  switch(pos)
  {
  case GSTK_SEARCH_BEGIN_STR:
    for(i = 0; i<(int)strlen(src) && i < (int)strlen(dest); i++)
    {
      if(src[i] != dest[i])
      {
        *match = FALSE;
        gstk_status = GSTK_SUCCESS;
        break;
      }
    }
    if(j == uint32toint32(strlen(src)))
    {
      *match = TRUE;
      gstk_status = GSTK_SUCCESS;
    }
    else
    {
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  case GSTK_SEARCH_END_STR:
    cnt = 0;
    for(i = ((int)strlen(src))-1,j = ((int)strlen(dest))-1; i>=0 && j >= 0; i--, j--)
    {
      if(src[i] != dest[j])
      {
        *match = FALSE;
        gstk_status = GSTK_SUCCESS;
        break;
      }
      cnt = cnt + 1;
    }
    if(cnt == uint32toint32(strlen(src)))
    {
      *match = TRUE;
      gstk_status = GSTK_SUCCESS;
    }
    else
    {
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  case GSTK_SEARCH_SINGLE_CHAR:
    /* Case to match a '*' */
    if(strcmp(src,"*") == 0)
    {
      for(j = 0; j < (int)strlen(dest); j++)
      {
        if(dest[j] == '*')
        {
          *match = TRUE;
          gstk_status = GSTK_SUCCESS;
          break;
        }
      }
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  default:
    MSG_HIGH("INVALID Search Criteria", 0, 0, 0);
    *match = FALSE;
    gstk_status = GSTK_ERROR;
  }
  return gstk_status;
} /* gstk_util_compareString */

/*=========================================================================
FUNCTION
  ASCIIText_ConvertTo_GSMDefault

DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void ASCIIText_ConvertTo_GSMDefault (const uint8   * def,
                                     uint32          len,
                                     uint8         * gsm_default
)
{
  uint32  i = 0;
  uint32  j = 0;

  if(def == NULL)
  {
    MSG_ERROR("def ERR:NULL",0,0,0);
    return;
  }

  if(gsm_default == NULL)
  {
    MSG_ERROR("gsm_default ERR:NULL",0,0,0);
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; i < len; i ++ )
  {
    j = def[i];
    gsm_default[i] = ascii_to_gsmdefault_table[j];
  }

} /* ASCIIText_ConvertTo_GSMDefault() */

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

/*===========================================================================

FUNCTION gstk_util_is_valid_rat

DESCRIPTION
  This function determines if the rat is valid or not, and map it to GSTK
  access tech type

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean gstk_util_is_valid_rat(sys_radio_access_tech_e_type sys_rat,
                               gstk_access_technology_type *gstk_rat_ptr)
{

  if (gstk_rat_ptr == NULL) {
    MSG_ERROR("Null gstk_rat pointer", 0, 0, 0);
    return FALSE;
  }

  /* print out RAT info */
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
  switch(sys_rat)
  {
#if defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WCDMA
#error code not present
#endif /*FEATURE_GSM || FEATURE_WCDMA */

  default:
    MSG_ERROR("Unknown Rat 0x%x", sys_rat, 0, 0);
    /* Indicate not to send any LS;  However, if we are in No Service,
       the switch statement on location status below will reset the
       boolean to TRUE, so that we will still send down an envelope
       for NO SERVICE regardless of RAT */
    return FALSE;
  }
#else
  MSG_HIGH("FEATURE_GSTK_FOR_NON_LTK_BUILD defined",0,0,0);
  return FALSE;
#endif /* FEATURE_GSTK_FOR_NON_LTK_BUILD */
}/*lint !e818 "gstk_rat_ptr not const . This lint error is suppressed
  when FEATURE_GSM or FEATURE_WCDMA is not defined." */

#ifdef FEATURE_CAT_REL6
#ifdef FEATURE_CCAT
/*===========================================================================
FUNCTION gstk_util_check_cdma_loc_info_difference

DESCRIPTION
  This function will get the cell id and determines whether there are any
  differences between the new location info/state with the old new.  This
  function will return cell_id as a result of the operation.

PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type
  cell_id: [Input] pointer to cell id to be populated
  location_status: [Input] location status

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_check_cdma_loc_info_difference(
           gstk_cdma_location_info_type    cdma_loc_info,
           gstk_location_status_enum_type  location_status)
{
#ifndef FEATURE_GSTK_FOR_NON_LTK_BUILD
  /* location_status:  New Location Status - Set by MM
  ** gstk_next_curr_location_status: Current Location Status set by GSTK (earlier)
  */
  MSG_HIGH("NEW LS: 0x%x OLD LS: 0x%x",location_status,
           gstk_next_curr_location_status,0);

  /* compare location state */
  if(location_status != gstk_next_curr_location_status) {
    MSG_HIGH("NO MATCH LS:  NEW: 0x%x OLD: 0x%x",location_status,gstk_next_curr_location_status,0);
    return TRUE;
  }

  /* To avoid sending down multiple NO Server LS Events,
  ** check to see if status is GSTK_NO_SERVICE.  If it is
  ** there is no difference. */
  if (location_status == GSTK_NO_SERVICE) {
    MSG_HIGH("NEW: NO SVC  OLD: NO SVC - No LS EVT",0,0,0);
    return FALSE;
  }

  /* SID info */
  if(!memcpy(cdma_location_info_cache.loc_info.sid_info.sid,
             cdma_loc_info.sid_info.sid,cdma_loc_info.sid_info.id_len))
  {
    return TRUE;
  }

  /* NID info */
  if(!memcmp(cdma_location_info_cache.loc_info.nid_info.nid,
             cdma_loc_info.nid_info.nid,cdma_loc_info.nid_info.id_len))
  {
    return TRUE;
  }


  /* MCC info */
  if(!memcmp(cdma_location_info_cache.loc_info.mcc,
             cdma_loc_info.mcc, GSTK_MCC_LEN))
  {
    return TRUE;
  }

    /* IMSI_11_12 */
  if(cdma_location_info_cache.loc_info.imsi_11_12
      != cdma_loc_info.imsi_11_12)
  {
    return TRUE;
  }

  /* BASE_ID info */
  if(!memcmp(cdma_location_info_cache.loc_info.base_id_info.base_id,
             cdma_loc_info.base_id_info.base_id,cdma_loc_info.base_id_info.id_len))
  {
    return TRUE;
  }

  /* BASE Latitude */
  if(!memcmp(cdma_location_info_cache.loc_info.base_lat,
             cdma_loc_info.base_lat, GSTK_BASE_LAT_LEN))
  {
    return TRUE;
  }

  /* BASE Longitude */
  if(!memcmp(cdma_location_info_cache.loc_info.base_long,
             cdma_loc_info.base_lat, GSTK_BASE_LONG_LEN))
  {
    return TRUE;
  }

#endif /*#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)*/

  return FALSE;
} /* gstk_util_check_cdma_loc_info_difference */
#endif /*#ifdef FEATURE_CCAT */

/*===========================================================================
FUNCTION gstk_util_translate_inkey_qualifier_to_dcs

DESCRIPTION
  This function translates the qualifier into the corresponding data
  coding scheme.

PARAMETERS
  qualifier: [Input] qualifier from the (U)SAT command

DEPENDENCIES
  None

RETURN VALUE
  byte -> data coding scheme

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint8 gstk_util_translate_inkey_qualifier_to_dcs(
   uint8                     qualifier )
{
  uint8 dcs;

  if ((qualifier & 0x02) == 0x02) {
     /* UCS2 */
     dcs = 0x08;
  }
  else {
     if (qualifier & 0x10) {
       /* packed sms */
       dcs = 0x00;
     }
     else {
       /* unpacked sms */
       dcs = 0x04;
     }
  }
  return dcs;
} /* gstk_util_translate_inkey_qualifier_to_dcs */

/*=========================================================================
FUNCTION
  gstk_util_find_match_in_cmd_details_ref_table

DESCRIPTION
  This function is used to find a match for a request in the command details
  reference table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - if a match is found.
  FALSE  - if no match is found.

SIDE EFFECTS
  None
=========================================================================*/
boolean gstk_util_find_match_in_cmd_details_ref_table(
    uint8                            * cmd_num_ptr,
    gstk_cmd_enum_type               rsp_type
)
{
  int i = 0;
  MSG_HIGH("gstk_util_find_match_in_cmd_details_ref_table...",0,0,0);
  if(cmd_num_ptr == NULL)
  {
      MSG_HIGH("cmd_num ptr is NULL...",0,0,0);
      return FALSE;
  }

  for(i=0;i<GSTK_MAX_PROACTIVE_COMMAND;i++)
  {
    if (command_details_ref_table[i].expected_cmd_rsp_type == rsp_type)
    {
      MSG_HIGH("Found a match in the cmd_details_ref_table...",0,0,0);
      *cmd_num_ptr = command_details_ref_table[i].command_details.command_number;
      return TRUE;
    }
  }
  MSG_HIGH("No match in the cmd_details_ref_table...",0,0,0);
  return FALSE;
} /* gstk_util_find_match_in_cmd_details_ref_table */

#endif /* #ifdef FEATURE_CAT_REL6 */



/*===========================================================================
FUNCTION gstk_util_find_tlv_len

DESCRIPTION
  This function finds the Len for the simple TLV.

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_cmd_data_ptr: [Input] Original Command Data from which the length is
                     calculated from
  length_ptr:        [Input/Output] pointer to the length is returned

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
static gstk_status_enum_type gstk_util_find_tlv_len (
  tags_enum_type              tag,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  int32                      *length_ptr
)
{
  if ((orig_cmd_data_ptr == NULL) || (length_ptr == NULL))
    return GSTK_BAD_PARAM;

  *length_ptr = 0;
  switch(tag)
  {
  case GSTK_NO_TAG:
    return GSTK_SUCCESS;

  case GSTK_COMMAND_DETAILS_TAG:
    *length_ptr = GSTK_COMMAND_DETAILS_LEN;
    return GSTK_SUCCESS;

  case GSTK_DEVICE_IDENTITY_TAG:
    *length_ptr = GSTK_DEVICE_IDENTITY_LEN;
    return GSTK_SUCCESS;

  case GSTK_RESULT_TAG:
    *length_ptr = GSTK_GEN_RESULT_LEN +
      orig_cmd_data_ptr->result_info_data.result_additional_info.length;
    return GSTK_SUCCESS;

  case GSTK_EVENT_LIST_TAG:
    *length_ptr = GSTK_EVENT_LIST_LEN;
    return GSTK_SUCCESS;

  case GSTK_DURATION_TAG:
  case GSTK_ALPHA_IDENTIFIER_TAG:
  case GSTK_ADDRESS_TAG:
  case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
  case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
  case GSTK_SS_STRING_TAG:
  case GSTK_USSD_STRING_TAG:
  case GSTK_SMS_TPDU_TAG:
  case GSTK_CELL_BROADCAST_PAGE_TAG:
  case GSTK_TEXT_STRING_TAG:
  case GSTK_TONE_TAG:
  case GSTK_ITEM_TAG:
  case GSTK_ITEM_IDENTIFIER_TAG:
  case GSTK_RESPONSE_LENGTH_TAG:
  case GSTK_FILE_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_LOCATION_INFORMATION_TAG:
    if (orig_cmd_data_ptr->location_info_data.location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
    {
      *length_ptr = GSTK_LOCATION_INFORMATION_LEN +
              orig_cmd_data_ptr->location_info_data.location_info.gsm_umts_loc_info.cell_id_len
              - GSTK_MIN_CELL_ID_LEN;
      return GSTK_SUCCESS;
    }
    else if (orig_cmd_data_ptr->location_info_data.location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
    {
      *length_ptr = GSTK_CDMA_LOCATION_INFORMATION_LEN;
      return GSTK_SUCCESS;
    }
    return GSTK_ERROR;

  case GSTK_NW_SEARCH_MODE_TAG:
    *length_ptr = GSTK_NW_SEARCH_MODE_LEN;
    return GSTK_SUCCESS;

  case GSTK_IMEI_TAG:
  case GSTK_HELP_REQUEST_TAG:
  case GSTK_NETWORK_MEASUREMENT_RESULTS_TAG:
  case GSTK_DEFAULT_TEXT_TAG:
  case GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG:
  case GSTK_CAUSE_TAG:
  case GSTK_LOCATION_STATUS_TAG:
  case GSTK_TRANSACTION_IDENTIFIER_TAG:
  case GSTK_BCCH_CHANNEL_LIST_TAG:
  case GSTK_ICON_IDENTIFIER_TAG:
  case GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG:
  case GSTK_CARD_READER_STATUS_TAG:
  case GSTK_CARD_ATR_TAG:
  case GSTK_C_APDU_TAG:
  case GSTK_R_APDU_TAG:
  case GSTK_TIMER_IDENTIFIER_TAG:
  case GSTK_TIMER_VALUE_TAG:
  case GSTK_DATE_TIME_AND_TIME_ZONE_TAG:
  case GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG:
  case GSTK_AT_COMMAND_TAG:
  case GSTK_AT_RESPONSE_TAG:
  case GSTK_BC_REPEAT_INDICATOR_TAG:
  case GSTK_IMMEDIATE_RESPONSE_TAG:
  case GSTK_DTMF_STRING_TAG:
  case GSTK_LANGUAGE_TAG:
  case GSTK_TIMING_ADVANCE_TAG:
  case GSTK_AID_TAG:
  case GSTK_BROWSER_IDENTITY_TAG:
  case GSTK_URL_TAG:
  case GSTK_BEARER_TAG:
  case GSTK_PROVISIONING_REF_FILE_TAG:
  case GSTK_BROWSER_TERMINATION_CAUSE_TAG:
  case GSTK_BEARER_DESCRIPTION_TAG:
  case GSTK_CHANNEL_DATA_TAG:
  case GSTK_CHANNEL_DATA_LEN_TAG:
  case GSTK_CHANNEL_STATUS_TAG:
  case GSTK_BUFFER_SIZE_TAG:
  case GSTK_CARD_READER_IDENTIFIER_TAG:
  case GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG:
  case GSTK_OTHER_ADDRESS_TAG:
  case GSTK_ACCESS_TECHNOLOGY_TAG:
  case GSTK_ESN_TAG:
  case GSTK_NETWORK_ACCESS_NAME_TAG:
  case GSTK_CDMA_SMS_TPDU_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GPRS_CALLCONTROL */

  case GSTK_IMEISV_TAG:
  case GSTK_BATTERY_STATUS_TAG:
  case GSTK_MEID_TAG:
  case GSTK_UTRAN_MEASUREMENT_TAG:
    return GSTK_UNSUPPORTED_COMMAND;
  }
  return GSTK_ERROR;
} /* gstk_util_find_tlv_len */

/*===========================================================================
FUNCTION gstk_util_populate_tlv_value

DESCRIPTION
  This function populates the simple TLV's Value.

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_cmd_data_ptr: [Input] Original Command Data from which the info
                     is copied from
  new_data_ptr:      [Input/Output] pointer to the new buffer where the
                     data will be copied into

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
static gstk_status_enum_type gstk_util_populate_tlv_value (
  tags_enum_type              tag,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  uint8                      *new_data_ptr,
  int32                       dest_size
)
{
  MSG_HIGH("Populate TLV Value 0x%x", tag, 0, 0);

  if ((orig_cmd_data_ptr == NULL) || (new_data_ptr == NULL))
  {
    MSG_ERROR("Null Input Ptr: orig ptr 0x%x, new ptr 0x%x",
      orig_cmd_data_ptr, new_data_ptr, 0);
    return GSTK_BAD_PARAM;
  }

  switch(tag)
  {
  case GSTK_NO_TAG:
    return GSTK_SUCCESS;

  case GSTK_COMMAND_DETAILS_TAG:
    return gstk_packer_cmd_details_card_tlv(
             orig_cmd_data_ptr->cmd_info_data,
             new_data_ptr);

  case GSTK_DEVICE_IDENTITY_TAG:
    return gstk_packer_dev_id_card_tlv(
             &orig_cmd_data_ptr->dev_id_data,
             new_data_ptr);

  case GSTK_RESULT_TAG:
    return gstk_packer_result_card_tlv(
             orig_cmd_data_ptr->result_info_data,
             new_data_ptr);

  case GSTK_EVENT_LIST_TAG:
    return gstk_packer_evt_list_card_tlv(
             orig_cmd_data_ptr->evt_list_info_data,
             new_data_ptr);

  case GSTK_DURATION_TAG:
  case GSTK_ALPHA_IDENTIFIER_TAG:
  case GSTK_ADDRESS_TAG:
  case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
  case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
  case GSTK_SS_STRING_TAG:
  case GSTK_USSD_STRING_TAG:
  case GSTK_SMS_TPDU_TAG:
  case GSTK_CELL_BROADCAST_PAGE_TAG:
  case GSTK_TEXT_STRING_TAG:
  case GSTK_TONE_TAG:
  case GSTK_ITEM_TAG:
  case GSTK_ITEM_IDENTIFIER_TAG:
  case GSTK_RESPONSE_LENGTH_TAG:
  case GSTK_FILE_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_LOCATION_INFORMATION_TAG:
    return gstk_packer_location_info_card_tlv(
             &orig_cmd_data_ptr->location_info_data,
             new_data_ptr,
             dest_size);

  case GSTK_IMEI_TAG:
  case GSTK_HELP_REQUEST_TAG:
  case GSTK_NETWORK_MEASUREMENT_RESULTS_TAG:
  case GSTK_DEFAULT_TEXT_TAG:
  case GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG:
  case GSTK_CAUSE_TAG:
  case GSTK_LOCATION_STATUS_TAG:
  case GSTK_TRANSACTION_IDENTIFIER_TAG:
  case GSTK_BCCH_CHANNEL_LIST_TAG:
  case GSTK_ICON_IDENTIFIER_TAG:
  case GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG:
  case GSTK_CARD_READER_STATUS_TAG:
  case GSTK_CARD_ATR_TAG:
  case GSTK_C_APDU_TAG:
  case GSTK_R_APDU_TAG:
  case GSTK_TIMER_IDENTIFIER_TAG:
  case GSTK_TIMER_VALUE_TAG:
  case GSTK_DATE_TIME_AND_TIME_ZONE_TAG:
  case GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG:
  case GSTK_AT_COMMAND_TAG:
  case GSTK_AT_RESPONSE_TAG:
  case GSTK_BC_REPEAT_INDICATOR_TAG:
  case GSTK_IMMEDIATE_RESPONSE_TAG:
  case GSTK_DTMF_STRING_TAG:
  case GSTK_LANGUAGE_TAG:
  case GSTK_TIMING_ADVANCE_TAG:
  case GSTK_AID_TAG:
  case GSTK_BROWSER_IDENTITY_TAG:
  case GSTK_URL_TAG:
  case GSTK_BEARER_TAG:
  case GSTK_PROVISIONING_REF_FILE_TAG:
  case GSTK_BROWSER_TERMINATION_CAUSE_TAG:
  case GSTK_BEARER_DESCRIPTION_TAG:
  case GSTK_CHANNEL_DATA_TAG:
  case GSTK_CHANNEL_DATA_LEN_TAG:
  case GSTK_CHANNEL_STATUS_TAG:
  case GSTK_BUFFER_SIZE_TAG:
  case GSTK_CARD_READER_IDENTIFIER_TAG:
  case GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG:
  case GSTK_OTHER_ADDRESS_TAG:
  case GSTK_ACCESS_TECHNOLOGY_TAG:
  case GSTK_ESN_TAG:
  case GSTK_NETWORK_ACCESS_NAME_TAG:
  case GSTK_CDMA_SMS_TPDU_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GPRS_CALLCONTROL */

  case GSTK_IMEISV_TAG:
  case GSTK_BATTERY_STATUS_TAG:
  case GSTK_MEID_TAG:
  case GSTK_UTRAN_MEASUREMENT_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_NW_SEARCH_MODE_TAG:
    return gstk_packer_nw_search_mode_card_tlv(
             orig_cmd_data_ptr->nw_search_mode_data,
             new_data_ptr);

  }
  return GSTK_ERROR;
} /* gstk_util_populate_tlv_value */


/*===========================================================================
FUNCTION gstk_util_populate_tlv

DESCRIPTION
  This function populates the simple TLV.
  It takes the original offset into the passed in data buffer to which
  the simple TLV will be populated.
  It also returns the new offset as a result of populating the simple TLV

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_offset:       [Input] Original Offset into the orig_cmd_data_ptr
  orig_cmd_data_ptr: [Input] Original Command Data from which the info
                     is copied from
  new_data_ptr:      [Input/Output] pointer to the new buffer where the
                     data will be copied into
  new_offset_ptr:    [Input/Output] pointer to which the new offset will
                     be returned

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
gstk_status_enum_type gstk_util_populate_tlv (
  tags_enum_type              tag,
  int32                       orig_offset,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  uint8                      *new_data_ptr,
  int32                      *new_offset_ptr
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  int32                 length = 0;
  int32                 new_offset = orig_offset;

  MSG_HIGH("Populate TLV 0x%x", tag, 0, 0);

  if ((new_data_ptr == NULL) ||
      (new_offset_ptr == NULL) ||
      (orig_cmd_data_ptr == NULL))
  {
    MSG_ERROR("Null Input Pointers: new data=0x%x, new offset=0x%x, orig data=0x%x",
      new_data_ptr, new_offset_ptr, orig_cmd_data_ptr);
    return GSTK_BAD_PARAM;
  }

  new_data_ptr[new_offset++] = (uint8)tag;

  gstk_status = gstk_util_find_tlv_len(tag, orig_cmd_data_ptr, &length);
  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_ERROR("Populate TLV Len fail 0x%x", gstk_status, 0, 0);
    return gstk_status;
  }

  if (length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
  {
    new_data_ptr[new_offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
    new_data_ptr[new_offset++] = int32touint8(length);
  }
  else
  {
    new_data_ptr[new_offset++] = int32touint8(length);
  }

  gstk_status = gstk_util_populate_tlv_value(
                             tag,
                             orig_cmd_data_ptr,
                             &new_data_ptr[new_offset],
                              UIM_MAX_CHARS - new_offset);
  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_ERROR("Populate TLV Value fail 0%x", gstk_status, 0, 0);
    return gstk_status;
  }

  if (length > 0)
  {
    new_offset += length;
  }
  *new_offset_ptr = new_offset;
  return GSTK_SUCCESS;
} /*gstk_util_populate_tlv */


/*===========================================================================
FUNCTION gstk_util_populate_uim_cmd_total_data_len

DESCRIPTION
  This function populates the data_len for the data in the uim_cmd_type
  according to the type of command to be sent to the card.
  It accounts for the case where > 0x7F total len by adding the extra
  byte for the 0x81 byte.
  If it is an envelope command, the Envelope Tag will be filled in as well

PARAMETERS
  ber_tlv_tag:    [Input] BER TLV Tag
  total_data_len: [Input] Total number of bytes for all the simple TLVs
  uim_cmd_ptr:    [Input/Output] pointer to the uim_cmd_type

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
gstk_status_enum_type gstk_util_populate_uim_cmd_total_data_len (
  int32         ber_tlv_tag,
  int32         total_data_len,
  uim_cmd_type *uim_cmd_ptr
)
{
  MSG_ERROR("Populate UIM Cmd Total Data Len BerTLV=0x%x, Len=0x%x",
    ber_tlv_tag, total_data_len, 0);

  if (uim_cmd_ptr == NULL)
    return GSTK_BAD_PARAM;

  switch (ber_tlv_tag)
  {
  case GSTK_PROACTIVE_SIM_COMMAND_TAG:
    return GSTK_ERROR;
  case GSTK_TERMINAL_RESPONSE_TAG:
    /* to do */
    return GSTK_SUCCESS;
  case GSTK_SMS_PP_DOWNLOAD_TAG:
  case GSTK_CELL_BROADCAST_DOWNLOAD_TAG:
  case GSTK_MENU_SELECTION_TAG:
  case GSTK_CALL_CONTROL_TAG:
  case GSTK_MO_SHORT_MESSAGE_CONTROL_TAG:
  case GSTK_EVENT_DOWNLOAD_TAG:
  case GSTK_TIMER_EXPIRY_TAG:
    if (total_data_len > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
    {
      /* account for 0x80 */
      uim_cmd_ptr->envelope.offset = 0;
      uim_cmd_ptr->envelope.data[0] = int32touint8(ber_tlv_tag);
      uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
      uim_cmd_ptr->envelope.data[2] = int32touint8(total_data_len);
      uim_cmd_ptr->envelope.num_bytes = int32touint8(total_data_len + GSTK_TAG_LENGTH_LEN + 1);

    }
    else {
      uim_cmd_ptr->envelope.offset = 1;
      uim_cmd_ptr->envelope.data[1] = int32touint8(ber_tlv_tag);
      uim_cmd_ptr->envelope.data[2] = int32touint8(total_data_len);
      uim_cmd_ptr->envelope.num_bytes = int32touint8(total_data_len + GSTK_TAG_LENGTH_LEN);
    }
    return GSTK_SUCCESS;
  default:
    return GSTK_ERROR;
  }
} /* gstk_util_populate_uim_cmd_total_data_len */

/*===========================================================================
FUNCTION gstk_util_populate_term_rsp_common_tlv

DESCRIPTION
  This function populates the command details, device identities and the
  result values into the uim buffer when building the terminal response.

PARAMETERS
  orig_offset       - [Input]original offset
  *total_len_ptr    - [Input/Output] Total length ptr
  gstk_tlv          - [Input] contains command details, device id details and result values.
  *uim_cmd_ptr      - [Input/Output] uim Cmd ptr

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
gstk_status_enum_type gstk_util_populate_term_rsp_common_tlv(
  int32                         orig_offset,
  uint32                        *total_len_ptr,
  gstk_cmd_term_rsp_common_type gstk_tlv,
  byte                          *uim_cmd_ptr
)
{
  gstk_device_id_type                           dev_id;
  int32                                         offset = 0;
  gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;

  offset = orig_offset;

  MSG_HIGH("In gstk_util_populate_term_rsp_common_tlv()",0,0,0);

  if(uim_cmd_ptr == NULL)
  {
    MSG_HIGH("Bad Parameter : uim_cmd_ptr ptr is NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(total_len_ptr == NULL)
  {
    MSG_HIGH("Bad Parameter : total_len ptr is NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  MSG_HIGH("Initial total_len_ptr value is %d",*total_len_ptr,0,0);

  gstk_status = gstk_util_populate_tlv(
                  GSTK_COMMAND_DETAILS_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*)&(gstk_tlv.cmd_info_data),
                  uim_cmd_ptr,
                  &offset); /*lint !e826 cmd_info_data suppress area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_HIGH("Error in populating Command Details TLV",0,0,0);
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_COMMAND_DETAILS_LEN +
               GSTK_TAG_LENGTH_LEN;

  /* device ID */
  dev_id.src = GSTK_ME_DEVICE;
  dev_id.dest = GSTK_UICC_SIM_DEVICE;

  gstk_status = gstk_util_populate_tlv(
                  GSTK_DEVICE_IDENTITY_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*)&dev_id,
                  uim_cmd_ptr,
                  &offset);/*lint !e826 area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_HIGH("Error in populating Device Identity TLV",0,0,0);
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_DEVICE_IDENTITY_LEN +
               GSTK_TAG_LENGTH_LEN;

  /* result tag */
  gstk_status = gstk_util_populate_tlv(
                  GSTK_RESULT_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*) &(gstk_tlv.result_info_data),
                  uim_cmd_ptr,
                  &offset);/*lint !e826 area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_HIGH("Error in populating Result TLV",0,0,0);
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_GEN_RESULT_LEN +
               gstk_tlv.result_info_data.result_additional_info.length +
               GSTK_TAG_LENGTH_LEN;

  return GSTK_SUCCESS;
} /* gstk_util_populate_term_rsp_common_tlv */

/*===========================================================================
FUNCTION gstk_util_is_envelope_rsp_evt_dl

DESCRIPTION
  This function will check the command id of a particular response is
  an for and event download envelope

PARAMETERS
  command_id: command identifier

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE:   if the command is an event download envelope
    FALSE:  if the command is not an event download envelope

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_envelope_rsp_evt_dl(uint32 command_id)
{
  MSG_HIGH("In gstk_util_is_envelope_rsp_evt_dl()",0,0,0);
  switch(command_id)
  {
  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
  case GSTK_ACCESS_TECH_EVT_IND_RSP:
  case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
  case GSTK_DATA_AVAIL_EVT_IND_RSP:
    return TRUE;
  default:
    return FALSE;
  }
}/* gstk_util_is_envelope_rsp_evt_dl */

/*===========================================================================
FUNCTION gstk_util_retry_envelope_cmd

DESCRIPTION
  This function will retry sending the last unsuccessful delivered envelope 
  to the card

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  Currently this function will be called to retry only event download envelopes.
  The calling function should check the gstk envelope state to see if it is 
  set to GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE before calling this
  command to retry the event download envelope.

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_retry_envelope_cmd(void)
{
  uim_cmd_type           *uim_cmd_ptr = NULL;
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  int                     i           = 0;

  MSG_HIGH("In gstk_util_retry_envelope_cmd()",0,0,0);

  /* Clear timer before sending the envelope */
  (void)rex_clr_timer(&gstk_curr_envelope_info.gstk_env_retry_timer);

  if(gstk_curr_envelope_info.gstk_env_state != 
       GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE)
  {
    MSG_HIGH("Incorrect GSTK env state to retry envelope",0,0,0);
    return GSTK_ERROR;
  }
  if(!gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr || 
     gstk_curr_envelope_info.env_data_bytes.data_len > UIM_MAX_CHARS)
  {
    MSG_HIGH("env retry buffer ptr NULL or data len out of range",0,0,0);
    return GSTK_ERROR;
  }
  if ((gstk_curr_envelope_info.env_ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) || 
      (gstk_curr_envelope_info.env_ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1))) 
  {
    MSG_ERROR("Invalid env ref id",0,0,0);
    return GSTK_ERROR;
  }

  if(gstk_curr_envelope_info.retry_info.retry_counter < 
       gstk_curr_envelope_info.retry_info.max_no_of_retries)
  {
    uim_cmd_ptr = gstk_get_uim_buf();
    if (uim_cmd_ptr == NULL ) 
    {
      MSG_HIGH("GSTK unable to get free buffer from UIM",0,0,0);
      return GSTK_ERROR;
    }
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) 
    {
     /* set to 0x00 so that uim will get the data from index 0 */
     uim_cmd_ptr->envelope.offset    = 0x00;
     /* set user_data */
     uim_cmd_ptr->hdr.user_data      = gstk_curr_envelope_info.env_ref_id;
     /* populate the envelope command info */
     uim_cmd_ptr->envelope.num_bytes = 
       int32touint8(gstk_curr_envelope_info.env_data_bytes.data_len);
     /* copy data */
     gstk_memcpy(uim_cmd_ptr->envelope.data, 
                 gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr, 
                 int32touint32(gstk_curr_envelope_info.env_data_bytes.data_len),  
                 UIM_MAX_CHARS, 
                 int32touint32(gstk_curr_envelope_info.env_data_bytes.data_len));
     MSG_MED("Env: num bytes 0x%x", uim_cmd_ptr->envelope.num_bytes, 0, 0);
     for(i = 0; i< (uim_cmd_ptr->envelope.num_bytes); i++) 
     {
       MSG_HIGH("uim_cmd_ptr[%d] = 0x%x, %c", i, uim_cmd_ptr->envelope.data[i], 
                uim_cmd_ptr->envelope.data[i]);
     }
     gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
     gstk_curr_envelope_info.retry_info.retry_counter++;
     gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_EVT_DL_RSP_FROM_CARD;
    }/* if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))  */
  }
  else
  {
     MSG_HIGH("No of retries 0x%x have exceeded MAX 0x%x", 
             gstk_curr_envelope_info.retry_info.retry_counter, 
             gstk_curr_envelope_info.retry_info.max_no_of_retries, 0);
     return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
}/* gstk_util_retry_envelope_cmd */

/*===========================================================================
FUNCTION gstk_util_clear_retry_envelope_data

DESCRIPTION
  This function will clear the envelope retry information stored in a gstk
  global variable

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_clear_retry_envelope_data(void)
{
  MSG_HIGH("In gstk_util_clear_retry_envelope_data()",0,0,0);
  (void)rex_clr_timer(&gstk_curr_envelope_info.gstk_env_retry_timer);
  gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_EVT_DL_NONE;
  gstk_curr_envelope_info.env_ref_id     = 0;
  gstk_curr_envelope_info.retry_info.max_no_of_retries = 0;
  gstk_curr_envelope_info.retry_info.retry_counter     = 0;
  if((gstk_curr_envelope_info.env_data_bytes.data_len != 0) && 
     gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr)
  {
    gstk_free((void*)gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr);
    gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr = NULL;
    gstk_curr_envelope_info.env_data_bytes.data_len        = 0;
  }
  else
  {
    MSG_HIGH("NULL ptr or len is zero ",0,0,0);
  }
}/* gstk_util_clear_retry_envelope_data */

#ifdef FEATURE_CAT_REL6
/*===========================================================================
FUNCTION gstk_cm_ph_reg_dereg

DESCRIPTION
  This function is used to register or deregister with CM for specific
  phone events.

PARAMETERS
  evt_flg:               [Input] flg indicates where Reg or Dereg is required.
  start_ph_evt:          [Input] phone event lower range
  end_ph_evt:            [Input] phone event upper range

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
gstk_status_enum_type gstk_cm_ph_reg_dereg(
  int32             evt_flg,
  cm_ph_event_e_type start_ph_evt,
  cm_ph_event_e_type end_ph_evt
)
{
  cm_client_status_e_type             cm_reg_return_status;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;

  if (evt_flg == 0x01)
  {
    MSG_MED("... Register PH event with CM 0x%x, 0x%x",
            start_ph_evt, end_ph_evt, 0);
    cm_reg_return_status = cm_client_ph_reg(
      gstk_client_id_from_cm,               /* pointer to the client struct */
      gstk_cm_ph_event_cb,                  /* pointer to a cb function for ss events */
      CM_CLIENT_EVENT_REG,                  /* event registration */
      start_ph_evt,                         /* from this ph event */
      end_ph_evt,                           /* to this ph event */
      NULL );                        /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      MSG_ERROR("REGISTRATION UNSUCCESSFUL WITH CM", 0, 0, 0);
      gstk_status = GSTK_ERROR;
    }
  }
  else
  {
    MSG_MED("... Deregister PH event with CM 0x%x, 0x%x",
            start_ph_evt, end_ph_evt, 0);
    cm_reg_return_status = cm_client_ph_reg(
      gstk_client_id_from_cm,               /* pointer to the client struct */
      gstk_cm_ph_event_cb,                  /* pointer to a cb function for ss events */
      CM_CLIENT_EVENT_DEREG,                /* event registration */
      start_ph_evt,                         /* from this ph event */
      end_ph_evt,                           /* to this ph event */
      NULL );                        /* pointer to a cb function for errors */                               /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      MSG_ERROR("DEREGISTRATION UNSUCCESSFUL WITH CM", 0, 0, 0);
      gstk_status = GSTK_ERROR;
    }
  }
  return gstk_status;
} /* gstk_cm_ph_reg_dereg */
#endif /* #ifdef FEATURE_CAT_REl6 */

#ifndef FEATURE_NO_UIM_RECOVERY_SUPPORT
/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_free_all_slots

DESCRIPTION
  This function is only used during internal sim reset during uim recovery. It
  frees up used slots in the command details ref table after notifying clients
  that the proactive sim session has ended.

PARAMETERS
  None

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
void gstk_util_cmd_details_ref_table_free_all_slots(void)
{
  int i = 0;
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++)
  {
    if ( command_details_ref_table[i].free_slot == FALSE)
    {
      /* Set the free slot boolean to TRUE, so that the slot is avaliable next time */
      command_details_ref_table[i].free_slot = TRUE;
      /* Reset all the information in the table to 0 */
      command_details_ref_table[i].command_details.command_number  = 0;
      command_details_ref_table[i].command_details.length          = 0;
      command_details_ref_table[i].command_details.qualifier       = 0;
      command_details_ref_table[i].command_details.tag             = 0;
      command_details_ref_table[i].command_details.type_of_command = 0;
      command_details_ref_table[i].destination_dev_id              = GSTK_NO_DEVICE;
      command_details_ref_table[i].expected_cmd_rsp_type           = GSTK_CMD_ENUM_NOT_USE;
      command_details_ref_table[i].partial_comprehension           = FALSE;

      /* clear the timer */
      (void) rex_clr_timer( &(command_details_ref_table[i].tp_rpt_time));
    }
  }
} /* gstk_util_cmd_details_ref_table_free_all_slots */

/*===========================================================================
FUNCTION gstk_util_build_proactive_end_session

DESCRIPTION
  This function builds the proactive end session from gstk and sends it
  to the client. This is different than the proactive end session that
  comes from the card.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  GSTK_ERROR -   if unable to allocate buffer from the free queue
  GSTK_SUCCESS - if cmd is successfully queued to the clients.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_build_proactive_end_session(void)
{
  gstk_proactive_cmd_type cmd;

  cmd.data[GSTK_GENERATED_END_SES_INDEX] = GSTK_GENERATED_END_SES;
  return(gstk_end_proactive_session(&cmd));
} /* gstk_util_build_proactive_end_session */
#endif /* FEATURE_NO_UIM_RECOVERY_SUPPORT */

/*===========================================================================
FUNCTION gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type

DESCRIPTION
  This function converts an STK command to a GSTK command type

PARAMETERS
  stk_cmd_type: [Input]        STK command type. This confirms to the 3GPP spec
  cmd_type_ptr: [Input/Output] GSTK command type

DEPENDENCIES
  None

RETURN VALUE
  GSTK_BAD_PARAM - if input parameters are incorrect
  GSTK_SUCCESS   - if cmd is successfully queued to the clients.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
  uint8               stk_cmd_type,
  uint8               stk_cmd_qual,
  gstk_cmd_enum_type *cmd_type_ptr)
{
  MSG_HIGH("In gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type()",0,0,0);
  if(cmd_type_ptr == NULL)
  {
    MSG_ERROR("cmd_type_ptr is NULL!", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  switch(stk_cmd_type)
  {   
  case GSTK_CMD_STK_POLL_INTERVAL:
    *cmd_type_ptr = GSTK_POLL_INTERVAL_REQ;
    break;

  case GSTK_CMD_STK_POLLING_OFF:
    *cmd_type_ptr = GSTK_POLLING_OFF_REQ; 
    break;

  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
    *cmd_type_ptr = GSTK_SETUP_EVENT_LIST_REQ;
    break;

  case GSTK_CMD_STK_SET_UP_CALL:
    *cmd_type_ptr = GSTK_SETUP_CALL_REQ;
    break;

  case GSTK_CMD_STK_SEND_SS:
    *cmd_type_ptr = GSTK_SEND_SS_REQ;
    break;

  case GSTK_CMD_STK_USSD:
    *cmd_type_ptr = GSTK_SEND_USSD_REQ;
    break;

  case GSTK_CMD_STK_LAUNCH_BROWSER:
    *cmd_type_ptr = GSTK_LAUNCH_BROWSER_REQ;
    break;

  case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
    *cmd_type_ptr = GSTK_SEND_SMS_REQ;
    break;

  case GSTK_CMD_STK_PLAY_TONE:
    *cmd_type_ptr = GSTK_PLAY_TONE_REQ;
    break;

  case GSTK_CMD_STK_DISPLAY_TEXT:
    *cmd_type_ptr = GSTK_DISPLAY_TEXT_REQ;
    break;

  case GSTK_CMD_STK_GET_INKEY:
    *cmd_type_ptr = GSTK_GET_INKEY_REQ;
    break;

  case GSTK_CMD_STK_GET_INPUT:
    *cmd_type_ptr = GSTK_GET_INPUT_REQ;
    break;

  case GSTK_CMD_STK_SELECT_ITEM:
    *cmd_type_ptr = GSTK_SELECT_ITEM_REQ;
    break;

  case GSTK_CMD_STK_SET_UP_MENU:
    *cmd_type_ptr = GSTK_SETUP_MENU_REQ;
    break;

  case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
    switch (stk_cmd_qual)
    {
    case GSTK_LANGUAGE_SETTING:
      /* Change expected cmd req type for Language provide local info */
      *cmd_type_ptr = GSTK_PROVIDE_LANG_INFO_REQ;
      break;
    case GSTK_BATTERY_STATUS:
#ifdef FEATURE_CAT_REL6
      /* Change expected cmd req type for Battery Status */
      *cmd_type_ptr = GSTK_PROVIDE_BATTERY_STATUS_REQ;
#else /* #ifdef FEATURE_REL6 */
      MSG_HIGH("GSTK_BATTERY_STATUS: FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      *cmd_type_ptr =  GSTK_PROVIDE_LOCAL_INFO_REQ;
      break;  
    }/* switch (stk_cmd_qual) */
    break;

  case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
    *cmd_type_ptr = GSTK_SETUP_IDLE_TEXT_REQ;
    break;

  case GSTK_CMD_STK_REFRESH:
    *cmd_type_ptr = GSTK_REFRESH_REQ ;
    break;

  case GSTK_CMD_STK_MORE_TIME:
    *cmd_type_ptr = GSTK_MORE_TIME_REQ;
    break;

  case GSTK_CMD_STK_TIMER_MANAGEMENT:
    *cmd_type_ptr =  GSTK_TIMER_MANAGEMENT_REQ;
    break;

  case GSTK_CMD_STK_LANG_NOTIFICATION:
    *cmd_type_ptr = GSTK_LANG_NOTIFICATION_REQ;
    break;

  case GSTK_CMD_STK_SEND_DTMF:
    *cmd_type_ptr = GSTK_SEND_DTMF_REQ;
    break;

  case GSTK_CMD_STK_OPEN_CHANNEL:
    *cmd_type_ptr = GSTK_OPEN_CH_REQ;
    break;

  case GSTK_CMD_STK_CLOSE_CHANNEL:
    *cmd_type_ptr = GSTK_CLOSE_CH_REQ;
    break;

  case GSTK_CMD_STK_RECEIVE_DATA:
    *cmd_type_ptr = GSTK_RECEIVE_DATA_REQ;
    break;

  case GSTK_CMD_STK_SEND_DATA:
    *cmd_type_ptr = GSTK_SEND_DATA_REQ;
    break;

  case GSTK_CMD_STK_GET_CHANNEL_STATUS:
    *cmd_type_ptr = GSTK_GET_CH_STATUS_REQ;
    break;

  case GSTK_CMD_STK_RUN_AT_COMMAND:
  case GSTK_CMD_STK_PERFORM_CARD_APDU:
  case GSTK_CMD_STK_POWER_ON_CARD:
  case GSTK_CMD_STK_POWER_OFF_CARD:
  case GSTK_CMD_STK_GET_READER_STATUS:
    *cmd_type_ptr = GSTK_CMD_ENUM_NOT_USE;
    return GSTK_ERROR;

  case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
    *cmd_type_ptr = GSTK_END_PROACTIVE_CMD_REQ;
    break;

  default:
    MSG_ERROR("STK cmd (0x%x) is unknown!", stk_cmd_type, 0, 0);
    *cmd_type_ptr = GSTK_CMD_ENUM_NOT_USE;
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type */

/*===========================================================================
FUNCTION gstk_util_call_client_evt_cb

DESCRIPTION
  Based on the type of the client, this function packs the right format of the
  arguments and invokes the event callback function registerd by the client.

  This function is only applicable to GSTK messages. Raw proactive commands are 
  handled in gstk_process_proactive_command().

PARAMETERS
  client_id:  [Input] The ID of the client whose event call back function will
                      be invoked.
  message_ptr:[Input] The pointer to the message to be passed to the client

DEPENDENCIES
  None

RETURN VALUE
  Void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_call_client_evt_cb(
  gstk_client_id_type      client_id,
  gstk_cmd_from_card_type *message_ptr
)
{
  gstk_client_pro_cmd_reg_data_type pro_cmd_data;
  uint32                            i           = client_id - 1;

  if (message_ptr == NULL)
  {
    MSG_ERROR("null message_ptr in gstk_util_call_client_evt_cb().", 0, 0, 0);
    return;
  }

  if (!gstk_util_is_valid_client_id(client_id))
  {
    MSG_ERROR("invalid client id in gstk_util_call_client_evt_cb().", 0, 0, 0);
    return;
  }

  message_ptr->hdr_cmd.user_data = gstk_client_table[client_id-1].user_data;

  switch( gstk_client_table[i].evt_cb_type ) 
  {  
  case GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT:
    /* this function is only applicable to GSTK messages.
     * raw proactive commands are handled in gstk_process_proactive_command()
     */
    pro_cmd_data.data_format_type = GSTK_GSTK_FORMAT;
    pro_cmd_data.pro_cmd_reg_data.gstk_req_rsp_ptr = message_ptr;
    if (gstk_client_table[i].evt_cb.new_client_cb != NULL)
    {
      (gstk_client_table[i].evt_cb.new_client_cb)(&pro_cmd_data);
    }
    else
    {
      MSG_ERROR("gstk_client_table[%d].evt_cb.new_client_cb == NULL!",
                i, 0, 0);
    }
    break;  
  case GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT:
    /* client only supports GSTK format */
    if (gstk_client_table[i].evt_cb.client_cb != NULL)
    {
      (gstk_client_table[i].evt_cb.client_cb)(message_ptr);
    }
    else
    {
      MSG_ERROR("gstk_client_table[%d].evt_cb.client_cb == NULL!",
                i, 0, 0);
    }
    break;
  default:
    MSG_ERROR("Invalid evt cb type with cli %d", client_id, 0, 0);
    break;
  }
} /* gstk_util_call_client_evt_cb */

/*===========================================================================
FUNCTION gstk_util_dump_byte_array

DESCRIPTION
  This function dumps byte array in the format of three item per line.

PARAMETERS
  array_name_ptr:  [Input] The name of the array to be dumped
  byte_array_ptr:  [Input] The pointer to the byte array to be dumped
  array_size:      [Input] The size of the byte array

DEPENDENCIES
  None

RETURN VALUE
  Void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_dump_byte_array(
  const char  *array_name_ptr,
  const byte  *byte_array_ptr,
  uint32      array_size
)
{
  uint32  i       = 0;
  uint32  idx     = 0;

  if (array_name_ptr == NULL || byte_array_ptr == NULL)
  {
    MSG_ERROR("null ptr pass into gstk_util_dump_byte_array().", 0, 0, 0);
    return;
  }

  MSG_HIGH("******** dumping array %s ********",
           array_name_ptr,
           0,
           0);

  while (i < (array_size / 3))
  {
    idx = i * 3;
    MSG_HIGH("0x%x 0x%x 0x%x",
             byte_array_ptr[idx],
             byte_array_ptr[idx + 1],
             byte_array_ptr[idx + 2]);
    ++ i;
  }

  idx = i * 3;
  switch( array_size - idx )
  {
  case 3:
    /* have 3 items left in last line */
    MSG_HIGH("0x%x 0x%x 0x%x",
             byte_array_ptr[idx],
             byte_array_ptr[idx + 1],
             byte_array_ptr[idx + 2]);
    break;
  case 2:
    /* have 2 items left in last line */
    MSG_HIGH("0x%x 0x%x",
             byte_array_ptr[idx],
             byte_array_ptr[idx + 1],
             0);
    break;
  case 1:
    /* have 1 item left in last line */
    MSG_HIGH("0x%x",
             byte_array_ptr[idx],
             0,
             0);
    break;
  default:
    break;
  }

  MSG_HIGH("******** finish dumping ********", 0, 0, 0);
} /* gstk_util_dump_byte_array */

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

/*===========================================================================
FUNCTION gstk_util_free_client_id

DESCRIPTION
  This function frees up client id and all event registered by this client

PARAMETERS
  client_id: the client id to be released

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
void gstk_util_free_client_id(gstk_client_id_type client_id)
{
  if (! gstk_util_is_valid_client_id(client_id))
  {
    MSG_ERROR("Invalid client id!", 0, 0, 0);
    return;
  }

#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

  gstk_client_table[client_id-1].free               = TRUE;
  gstk_client_table[client_id-1].evt_cb_type        = GSTK_CLI_CB_NONE;
  gstk_client_table[client_id-1].evt_cb.client_cb   = NULL;
  gstk_client_table[client_id-1].evt_reg_bitmask    = 0;
  gstk_client_table[client_id-1].client_type        = GSTK_NO_TYPE;
  gstk_client_table[client_id-1].user_data          = 0;
  gstk_client_table[client_id-1].client_init_cmd_cb = NULL;
} /* gstk_util_free_client_id */


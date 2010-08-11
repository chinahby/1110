#ifndef GSTKI_H
#define GSTKI_H
/*===========================================================================


    G S T K   A N D   I N T E R N A L   M O D U L E   I N T E R F A C E S


GENERAL DESCRIPTION
  This header file is the interface definitions between GSTK and the various
  QCT internal modules



FUNCTIONS
  gstk_send_proactive_cmd
    This function allows clients to put a proactive cmd request before parsing
    onto GSTK command queue

  gstk_is_mo_sms_ctrl_required
    Utility function to check if mo sms control is required for any MO calls

  gstk_is_sms_pp_supported
    Utility function to check if sms pp control is supported.

  gstk_util_release_card_cmd_memory
    This function frees all the allocated memory for the command pointer input.

  gstk_util_populate_card_cmd_memory
    This function allocates and copies memory for the command pointer input.

  gstk_cmd
    This function allows clients to put the gstk_cmd onto the GSTK command
    queue.

  gstk_task_get_cmd_buf
    This function get the gstk_task_cmd_type from the GSTK free queue.

  gstk_task
    GSTK Main Task

  gstk_slot_is_cc_required
    Utility function to check if call control is required for any MO calls from
    the slot indicated

  gstk_util_dereg_client_type
    This function dereg events for certain client type.  dereg_evt indicates 
    the events to be removed from the client's list

  gstk_is_ussd_object_supported
    Utility function to check if the USSD object in call control is supported
    by the SIM or not.
    
  gstk_is_cell_broadcast_supported
    Utility function to check if the cell broadcast is supposed by the mobile

  gstk_test_automation_restart
    Called by test automation script via DIAG to soft restart GSTK

INITIALIZATION AND SEQUENCING REQUIREMENTS



===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2010 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/gstk/src/gstki.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/10   bd      Fixed maximum timers supported to pass GCF cases
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id 
                   when parse client TP and simplify TP download procedure
03/02/09   xz      Add client_id to gstk_client_raw_term_resp_type structure
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/02/08   xz      Change param type of gstk_test_automation_restart to int
08/22/08   xz      Add gstk_test_automation_restart()
08/21/08   sk      Added support for client id enhanced registration
08/08/08   xz      Add support for raw envelope command and raw format
                   registration
02/06/08   sk      Support for UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
02/27/07   jk      Reduce Max Timers Supported for Memory Reduction
11/08/06   tml     Added function to return the current toolkit slot
10/08/06   sk      Added support for unknown unsupported proactive commands.
07/27/06   jk      Moved definition of gstk_is_cc_required to gstk_exp for RPC
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
                   Added support for immediate digit response and variable 
                   timeout duration object for get inkey cmd.
04/04/06   sk      Added support for PLI - battery status ESN, MEID, IMEISV
10/10/05   tml     Added GSTK_ACCESS_NONE
10/08/05   tml     Added get cell ID function prototype
10/07/05   tml     Added plmn and lac in rrc cell change ind type
10/04/05   sk      Defined GSTK_MIN_CELL_ID_LEN
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
03/05/05   tml     Added tag comprehension support
01/10/05   sk      Added count to get channel status term rsp
12/08/04   sk      Changed struct gstk_get_ch_status_cnf_type
                   to support maximum GPRS channels.
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added support for get inkey UCSII input support
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Changed to use pointer for additional info, added is ussd 
                   object supported for send USSD call control
06/02/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports
06/01/04   tml     Added sw1 and sw2 in envelope rsp type
03/04/04   tml     Compilation fix
02/06/04   wli     Featurization for CDMA only environment
09/13/03   tml     Change mode_type to mode_enum_type and
                   slot_id_type to slot_id_enum_type
09/04/03   tml     Dual slot support
05/18/03   tml     linted
04/21/03   tml     Added sms pp download support check and cdma sms boolean flag
03/19/03   tml     Added data_present field in get inkey, get input and
                   select item cnf data type
03/06/03   tml     Updated enum names, Added gstk_is_cc_required,
                   gstk_is_mo_sms_ctrl_required
02/11/03   tml     Moved gstk_util_release_card_cmd_memory to this header
                   Added gstk_util_populate_card_cmd_memory
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_exp.h"
#include "cmd.h"
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */
#endif /*! GSTK_FOR_NON_LTK_BUILD */

#include "sys.h"


#define GSTK_MAX_PLMN_LEN          3
#define GSTK_MAX_LAC_LEN           2

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


#define GSTK_GET_INKEY_USER_INPUT_DATA_LEN 4
#define GSTK_MAX_DATA_SIZE                 256
#define GSTK_MAX_TP_DATA_SIZE              512

#define GSTK_IMEI_LEN                       8
#define GSTK_IMEI_SV_LEN                    10
#define GSTK_ESN_LEN                        4
#define GSTK_MEID_LEN                       8
#define GSTK_NMR_LEN                        16
#define GSTK_MAX_TIMER_SUPPORTED            8

#define GSTK_MILISECONDS_IN_AN_HOUR       3600000
#define GSTK_MILISECONDS_IN_A_MINUTE      60000
#define GSTK_MILISECONDS_IN_A_SECOND      1000
#define GSTK_MILISECONDS_IN_ONE_TENTH_SEC 100

#define GSTK_MAX_CELL_ID_LEN              4
#define GSTK_MIN_CELL_ID_LEN              2
#define GSTK_MAX_BASE_ID_LEN              2
#define GSTK_MAX_SID_LEN                  2
#define GSTK_MAX_NID_LEN                  2
#define GSTK_MCC_LEN                      2
#define GSTK_BASE_LAT_LEN                 3
#define GSTK_BASE_LONG_LEN                3
/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/

/* This variable is set to TRUE when gstk declines UIM request to do fetch. And
   it is set to FALSE when gstk receives a Proactive command. 
   It is used to check if a proactive command from card is pending or not.*/
extern boolean gstk_uim_fetch_pending;

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

/* UTRAN NMR response structure
*/
typedef struct
{
    uint32                          length;
    uint8 *                         nmr_ptr; 
}
gstk_nmr_utran_rsp_type;

/* Various Provide local information utran nmr measurement qualifiers 
** that the USIM can request from the ME
*/
typedef enum
{ 
    /* These are one byte values */
    GSTK_NMR_NONE_MEAS                     = 0x00,
    GSTK_NMR_INTRA_FREQ_MEAS               = 0x01,
    GSTK_NMR_INTER_FREQ_MEAS               = 0X02,
    GSTK_NMR_INTER_RAT_MEAS                = 0X03
}
gstk_nmr_utran_qualifier_enum_type;

/* Various UTRAN NMR processing results that RRC can return to GSTK
*/
typedef enum
{ 
    GSTK_UTRAN_NMR_RESULT_SUCCESS,
    GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN,
    GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS,
    GSTK_UTRAN_NMR_RESULT_FAIL_OTHER
} 
gstk_utran_status_enum_type;

/* Various local information that the provide local information cmd can
** request from the SIM/USIM
** Based on the parsing result from the proactive command tlv, GSTK will call
** the corresponding function in the various layers, i.e., the various layers
** involved do not need to know this enum type
*/
typedef enum
{
   /* values based on Standard */
   GSTK_LOCATION_INFO                  = 0x00,
   GSTK_IMEI                           = 0x01,
   GSTK_NMR                            = 0x02,
   GSTK_DATE_TIME_ZONE                 = 0x03,
   GSTK_LANGUAGE_SETTING               = 0x04,
   GSTK_TIMING_ADVANCE                 = 0x05,
   GSTK_ACCESS_TECHNOLOGY              = 0x06,
   GSTK_ESN                            = 0x07,
   GSTK_IMEISV                         = 0x08,
   GSTK_NETWORK_SEARCH_MODE            = 0x09,
   GSTK_BATTERY_STATUS                 = 0x0A,
   GSTK_MEID                           = 0x0B,
   GSTK_NO_LOCAL_INFO                  = 0xFF
}
gstk_local_info_enum_type;

typedef enum
{
   GSTK_ACCESS_TECH_GSM               = 0x00,
   GSTK_ACCESS_TECH_UTRAN             = 0X03,
   GSTK_ACCESS_TECH_CDMA              = 0X06,
   GSTK_ACCESS_NONE                   = 0xFF
}
gstk_access_technology_type;

typedef struct
{
   /* based on 3G 24.008 */
   uint8                        mcc_and_mnc[3];
   uint8                        loc_area_code[2];
   uint8                        cell_id[4];        /* Converted from a 16-bit word */
   int32                        cell_id_len;
}
gstk_location_info_rsp_type;

typedef struct
{
  int32             id_len;
  uint8             base_id[GSTK_MAX_BASE_ID_LEN];

} gstk_base_id_type;

typedef struct
{
  int32             id_len;
  uint8             sid[GSTK_MAX_SID_LEN];
} gstk_sid_type;

typedef struct
{
  int32             id_len;
  uint8             nid[GSTK_MAX_NID_LEN];
} gstk_nid_type;

typedef struct
{
   uint8                        mcc[GSTK_MCC_LEN];
   uint8                        imsi_11_12;
   gstk_sid_type                sid_info;
   gstk_nid_type                nid_info;        
   gstk_base_id_type            base_id_info;
   uint8                        base_lat[GSTK_BASE_LAT_LEN];
   uint8                        base_long[GSTK_BASE_LONG_LEN]; 
}
gstk_cdma_location_info_type;

typedef struct
{
   uint8                         year;
   uint8                         month;
   uint8                         day;
   uint8                         hour;
   uint8                         minute;
   uint8                         second;
   uint8                         zone;
}
gstk_date_time_rsp_type;

typedef struct
{
   uint8                        me_status;      /* 0x00 = idle state */
                                                /* 0x01 = not in idle state */
   uint8                        timing_advance;
}
gstk_time_advance_rsp_type;

 typedef enum
{
   GSTK_GSM_UMTS_LOCATION_INFO         = 0x00,
   GSTK_CDMA_LOCATION_INFO             = 0x01,
   GSTK_NO_SRVC_LOCATION_INFO          = 0x02,
   GSTK_IMEI_INFO                      = 0x03,
   GSTK_NMR_BCCH_INFO                  = 0x04,
   GSTK_NMR_UTRAN_INFO                 = 0x05,
   GSTK_DATE_TIME_ZONE_INFO            = 0x06,
   GSTK_LANGUAGE_SETTING_INFO          = 0x07,
   GSTK_TIMING_ADVANCE_INFO            = 0x08,
   GSTK_ACCESS_TECHNOLOGY_INFO         = 0x09,
   GSTK_ESN_INFO                       = 0x0A,
   GSTK_IMEISV_INFO                    = 0x0B,
   GSTK_BATTERY_STATUS_INFO            = 0x0C,
   GSTK_MEID_INFO                      = 0x0D,
   GSTK_NETWORK_SEARCH_MODE_INFO       = 0x0E,
   GSTK_INVALID_LOC_INFO               = 0xFF
}
gstk_local_info_gen_enum_type; 

typedef struct
{
  gstk_local_info_gen_enum_type       info_type;

  union{
    gstk_location_info_rsp_type   location_info;
    gstk_cdma_location_info_type  cdma_loc_info;
    uint8                         imei[GSTK_IMEI_LEN];
    uint8                         imei_sv[GSTK_IMEI_SV_LEN];
    uint8                         esn[GSTK_ESN_LEN];
    uint8                         meid[GSTK_MEID_LEN];

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */ 
    gstk_nmr_utran_rsp_type       nmr_utran;   
    gstk_date_time_rsp_type       time;
    uint8                         lang_code[2];
    gstk_time_advance_rsp_type    time_adv;
    gstk_access_technology_type   gstk_access_tech;
    gstk_battery_state_enum_type  battery_status;
    gstk_nw_search_mode_enum_type nw_search_mode;
  }info_data;
}
gstk_local_info_rsp_type;



/* ----------------------------------------------------------------------- */



typedef enum
{
    GSTK_PROACTIVE_CMD,
    GSTK_TERMINAL_RSP,
    GSTK_ENVELOPE_CMD,
    GSTK_ENVELOPE_RSP,
    GSTK_TERMINAL_PROFILE_CMD,
    GSTK_TERMINAL_PROFILE_EXT_CMD,
    GSTK_TERMINAL_PROFILE_RSP,
    GSTK_STOP_CMD, /* power off */
    GSTK_GSDI_CMD,
    GSTK_TIMER_EXP_CMD, /* command_id includes: GSTK_WATCHDOG_MSG */
    GSTK_MM_IDLE_CMD, /* mm enters idle state */
    GSTK_RRC_CELL_CHANGE_IND,
    GSTK_MODE_CHANGE_IND, /* CM mode change */
    GSTK_PH_NW_SEL_PREF_CHANGE_IND, /* Phone Network Selection Preference Change from CM */
    GSTK_CLIENT_REG_REQ_CMD, /* used by clients to reg for proactive cmds and for client id registrations */
    GSTK_MAX_CMD = 0x7FFFFFFF
}
gstk_cmd_group_enum_type;

/* location status information */
typedef enum
{
   /*  values based on standard */
   GSTK_NORMAL_SERVICE                 = 0x00,
   GSTK_LIMITED_SERVICE                = 0x01,
   GSTK_NO_SERVICE                     = 0x02
}
gstk_location_status_enum_type;




/* when command_group ==
** 1. GSTK_PROACTIVE_CMD, command_id doesn't have to be filled in by the
**    other layers
** 2. GSTK_TERMINAL_RSP, command_id is required to be filled in by GSTK utility function
**    or other layers who do not utilitize the utility function
** 3. GSTK_ENVELOPE_CMD, command_id is required to be filled in byt eh GSTK utility function
**    of by other layers who do not utilitize the utility function
** 4. GSTK_ENVELOPE_RSP, command_id filled in by GSTK based on the user_data that UIM put.
**                       the user_data is going to be the index location for the envelope
**                       command that GSTK sends to UIM server.
**                       GSTK will base on this user data and perform a lookup for the
**                       response type == GSTK_SMS_PP_DOWNLOAD_IND_RSP,GSTK_MO_SMS_CTRL_IND_RSP,
**                                     GSTK_CC_IND_RSP etc,  in the state machine processing
** 5. GSTK_TERMINAL_PROFILE_CMD, command_id is irrelevant
** 6. GSTK_TERMINAL_PROFILE_RSP, command_id = GSTK_PROFILE_DL_IND_RSP
** 7. GSTK_STOP_CMD, command_id is irrelevant
** 8. GSTK_GSDI_CMD, command_id will be the gsdi_sim_events_T
** 9. GSTK_TIMER_EXP_CMD, command_id depends on whether it is TP or TR timer
** 10. GSTK_MM_IDLE_CMD, command_id irrelevant
** 11. GSTK_RRC_CELL_CHANGE_IND
** 12. GSTK_MODE_CHANGE_IND, command_id irrelevant
*/
typedef struct
{
    gstk_cmd_group_enum_type        command_group;
    uint32                          command_id;       /* gstk_cmd_enum_type or GSTK_FSM_CMD or GSTK_ERROR_CMD */
    uint32                          user_data;        /* in communicating with UIM, GSTK will fill in user_Data which
                                                      ** UIM has to return the same info upon information received
                                                      ** from CARD */
                                                      /* user_data becomes env_ref_id in case of envelope_cmd */
}
gstk_hdr_type;


typedef struct
{
    gstk_hdr_type                  message_header;
    uint32                         length;     /* length of data */
    uint8                          data[GSTK_MAX_DATA_SIZE];
}
gstk_general_cmd_type;


typedef gstk_general_cmd_type gstk_proactive_cmd_type;


/*
** Mode change indication
*/
typedef struct
{
    gstk_hdr_type                  message_header;
    struct
    {
#ifdef FEATURE_GSTK_DUAL_SLOTS 
    gstk_mode_enum_type            new_mode;
#endif /* FEATURE_GSTK_DUAL_SLOTS */
      gstk_access_technology_type    access_technology;

      /* This structure is valid only if access_technology is CDMA */
      gstk_cdma_location_info_type   cdma_loc_info;

      /* This enum is valid only if access_technology is CDMA */
      gstk_location_status_enum_type location_status; 
    } cmd;
} 
gstk_mode_change_ind_type;

#ifdef FEATURE_CAT_REL6
typedef struct
{
  gstk_hdr_type                  message_header;
  gstk_nw_search_mode_enum_type  ph_nw_sel_pref;
}
gstk_ph_nw_sel_pref_change_ind_type;
#endif

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
}
gstk_unsupported_proactive_cmd_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type*/
}
gstk_display_text_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   boolean                            data_present;
   uint8                              returned_data_len;       /* data len for the user inputs: 1 or 0 */
   uint8                              data[GSTK_GET_INKEY_USER_INPUT_DATA_LEN]; /* user input */
   uint8                              data_coding_scheme;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type*/
   gstk_duration_type                 duration_of_exec;
}
gstk_get_inkey_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   boolean                            data_present;
   uint8                              returned_data_len;         /* data len for the user inputs */
   uint8                              *data;                     /* user inputs */
   uint8                              data_coding_scheme;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type*/
}
gstk_get_input_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;
    /* used when command_result =
            ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            LAUNCH_BROWSER_GENERIC_ERROR
                - 00 = no specific cause can be given
                  01 = bearer unavailable
                  02 = browser unavailable
                  03 = ME unable to read the provisioning data             */

}
gstk_launch_browser_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type */
}
gstk_more_time_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
}
gstk_play_tone_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_duration_type                 duration;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type */
}
gstk_poll_interval_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type 
                                                                    (not_in_speech_call if the call has been
                                                                     terminated or torn down before completion
                                                                     of this command                      */
}
gstk_send_dtmf_cnf_type;


typedef struct
{
   uint8                               hour;
   uint8                               minute;
   uint8                               second;
}
gstk_timer_value_type;

typedef struct
{
   uint8                               timer_id;  /* valid value: [0x01, 0x08] */
   gstk_timer_value_type               time_value;
}
gstk_timer_info_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   boolean                            timer_info_present;
   gstk_timer_info_type               time_info;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
}
gstk_timer_manage_cnf_type; 

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
                                                                    
   
}
gstk_lang_notification_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_local_info_rsp_type           local_info_rsp;
   uint8                              returned_data_len;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type */
}
gstk_provide_local_info_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;  /* used when command_result =
                                                                  ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                  = gstk_additional_info_code_enum_type */
}
gstk_refresh_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   //JAR
//   gstk_at_rsp_type                   at_rsp;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type */
}
gstk_run_at_cmd_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   uint8                              chosen_item_identifier;
   boolean                            data_present;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type */
}
gstk_select_item_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                                cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type          */
}
gstk_setup_evt_list_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type */
}
gstk_setup_menu_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_call_ss_rsp_extra_param_type  extra_param;
   gstk_additional_info_ptr_type      result_additional_info;
       /* used when command_result =
            ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND = gstk_additional_info_code_enum_type
            NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */

}
gstk_setup_call_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_call_ss_rsp_extra_param_type  extra_param;
   gstk_additional_info_ptr_type      result_additional_info;
    /* used when command_result =
            SUCCESSFUL command results
                - first byte = SS return operation code
                  rest =       SS return parameters
                from the return result component from the network
            ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            SS_RETURN_ERROR
                - first byte = error value given in the Facility (return result)
                               information element returned by the network
            INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */

}
gstk_send_ss_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_ussd_rsp_extra_param_type     ussd_extra_param;
   gstk_additional_info_ptr_type      result_additional_info;
     /* used when command_result =
            ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            USSD_RETURN_ERROR
                - first byte = error value given in the Facility (Return result)
                               information element returned by the network
            INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
}
gstk_send_ussd_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;
       /* used when command_result =
            ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND : gstk_additional_info_code_enum_type
            NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                - first byte = cause value of the
                               Cause information from network, bit 8 = 1
            GSTK_SMS_RP_ERROR
                - first byte = cause value in the RP-Cause element of the
                               RP-ERROR message returned by the network,
                               bit 8 = 0
            INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM
                - first byte: 0x00 = no specific cause can be given
                              0x01 = action not allowed
                              0x02 = the type of request has changed       */
}
gstk_send_sms_cnf_type;



typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info; /* used when command_result =
                                                                 ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                 = gstk_additional_info_code_enum_type*/
}
gstk_setup_idle_text_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
   boolean                            other_data_present;
   boolean                            ch_status_present;
   gstk_ch_status_type                ch_status;
   gstk_bearer_description_type       bearer_description;
   int32                              buffer_size;
}
gstk_open_ch_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
}
gstk_close_ch_cnf_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
   boolean                            data_present;
   int32                              ch_data_remain_buf_len;            
}
gstk_send_data_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
   boolean                            data_present;
   gstk_ch_data_type                  ch_data;
   int32                              ch_data_remaining_len;
}
gstk_receive_data_cnf_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             cmd_details_ref_id;
   uint8                              command_number;
   gstk_general_result_enum_type      command_result;
   gstk_additional_info_ptr_type      result_additional_info;    /* used when command_result =
                                                                    ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND
                                                                    = gstk_additional_info_code_enum_type */
   boolean                            data_present;
   int32                              ch_cnt;
   gstk_ch_status_type                ch_status[GSTK_MAX_GPRS_CHANNELS];
}
gstk_get_ch_status_cnf_type;


/* Envelope command indication type */
typedef struct
{
   gstk_hdr_type                      message_header;
   uint8                              chosen_item_id;
   /* Optional */
   boolean                            request_help;

}
gstk_envelope_menu_selection_ind_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   boolean                            is_cdma_sms_pp;
   gstk_sms_tpdu_type                 tpdu;
   /* Optional */
   gstk_address_type                  address;
}
gstk_envelope_sms_pp_download_ind_type;

/* Raw Envelope Indication type.  env_len indicates the size of the
   env_data_ptr.  env_data_ptr should contains the envelope command as defined
   in 11.14 or 31.111 */
typedef struct
{
   gstk_hdr_type                      message_header;
   uint32                             env_len;
   uint8                             *env_data_ptr;
}
gstk_raw_envelope_ind_type;

typedef struct
{
    gstk_hdr_type                     message_header;
    gstk_cb_page_type                 cb_page;
}
gstk_envelope_cell_broadcast_download_ind_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   gstk_cc_ind_type                   cc_data;
   boolean                            slot_info_present;
   gstk_slot_id_enum_type             slot;
}
gstk_envelope_cc_ind_type;



typedef struct
{
   gstk_hdr_type                      message_header;
   gstk_mo_sms_ctrl_ind_type          mo_sms_ctrl_data;
}
gstk_envelope_mo_sms_ctrl_ind_type;


typedef struct
{
   gstk_hdr_type                      message_header;
   gstk_mt_call_evt_ind_type          mt_evt_data;
}
gstk_envelope_mt_evt_ind_type;

typedef struct
{
   gstk_hdr_type                      message_header;
   gstk_transaction_id_type           transaction_id;
   boolean                            near_end;
}
gstk_envelope_call_connected_evt_ind_type;



typedef struct
{
   gstk_hdr_type                       message_header;
   gstk_call_disconnected_evt_ind_type call_disconnected_data;
   boolean                             near_end;
}
gstk_envelope_call_disconnected_evt_ind_type;


typedef struct
{
   gstk_hdr_type                        message_header;
}
gstk_envelope_idle_scrn_evt_ind_type;


typedef struct
{
   gstk_hdr_type                        message_header;
}
gstk_envelope_user_act_evt_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   uint8                                location_status;
   gstk_local_info_rsp_type             location_info;
}
gstk_envelope_location_statue_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   gstk_timer_info_type                 timer_info;
}
gstk_envelope_timer_expire_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   gstk_lang_type                       lang_code; /* 2 bytes of alpha-numeric char 
                                                 coded in SMS default 7-bit 
                                                 with bit 8 == 0 */
}
gstk_envelope_lang_sel_evt_ind_type;  


typedef struct
{
   gstk_hdr_type                        message_header;
   gstk_browser_term_cause_enum_type    browser_termination_cause;
}
gstk_envelope_browser_term_evt_ind_type;  


typedef struct
{
   gstk_hdr_type                            message_header;
   gstk_ch_status_type                      ch_status_info;
   int32                                    ch_data_len;
}
gstk_envelope_data_avail_evt_ind_type ;

typedef struct
{
   gstk_hdr_type                            message_header;
   gstk_ch_status_type                      ch_status_info;
}
gstk_envelope_ch_status_evt_ind_type;


typedef struct
{
   gstk_hdr_type                        message_header;
   uint8                                    access_tech;
}
gstk_envelope_access_tech_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   uint8                                nw_search_mode;
}gstk_envelope_nw_search_mode_evt_ind_type;

typedef struct
{
   gstk_hdr_type                        message_header;
}
gstk_report_time_msg_type;

typedef struct
{
    gstk_hdr_type                       message_header;
    boolean                             success;
}
gstk_term_profile_rsp_type;

typedef struct
{
    gstk_hdr_type                         message_header;
    gstk_general_envelope_rsp_enum_type   rsp_status;
    uint8                                 sw1;
    uint8                                 sw2;
    uint32                                length;     /* length of data */
    uint8                                 data[GSTK_MAX_DATA_SIZE];
}
gstk_envelope_resp_type;

typedef gstk_general_cmd_type gstk_error_msg_type;

typedef gstk_hdr_type gstk_power_down_type;
typedef gstk_hdr_type gstk_gsdi_evt_type;

/* differentiates term profile for sat and usat */
typedef struct
{
    gstk_hdr_type                  message_header;
    uint32                         length;     /* length of data */
    uint8                          data[GSTK_MAX_TP_DATA_SIZE];
}
gstk_terminal_profile_cmd_type;

typedef struct
{
  int32               cell_len;
  uint8               cell_id[GSTK_MAX_CELL_ID_LEN];
} gstk_cell_id_type;

typedef struct
{
   gstk_hdr_type                        message_header;
   uint8                                mcc_mnc[GSTK_MAX_PLMN_LEN];
   uint8                                lac[GSTK_MAX_LAC_LEN];
   uint8                                location_state; /* full service, limited service, no service */
   uint8                                rat; /* gsm or wcdma */
   gstk_cell_id_type                    cell_id;
}
gstk_mm_idle_state_type;


typedef struct
{
  gstk_hdr_type                        message_header;
  uint8                                mcc_mnc[GSTK_MAX_PLMN_LEN];
  uint8                                lac[GSTK_MAX_LAC_LEN];
  uint8                                location_state; /* full service, limited service, no service */
  gstk_cell_id_type                    cell_info;
}
gstk_rrc_cell_change_ind_type;

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_client_id_type                       client_id;
  uint32                                    num_of_cmds;
  gstk_toolkit_cmd_reg_info_type            *reg_info_ptr;
  gstk_reg_rsp_cmd_cb_func_type             client_cmd_cb;
  gstk_toolkit_evt_cb_func_type             client_evt_cb;  
  gstk_client_pro_cmd_data_format_enum_type data_format_type;
}
gstk_client_evt_reg_type;

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_client_type                          client_type;
  gstk_client_init_cb_func_type             client_cb_ptr;
}
gstk_client_id_reg_type;

typedef struct
{
  gstk_hdr_type                             message_header;
  gstk_client_id_type                       client_id;
  uint32                                    cmd_details_ref_id;
  gstk_generic_data_type                    raw_term_resp;
}
gstk_client_raw_term_resp_type;

/*
** GSTK_TERMINAL_RSP_CNF message
*/
typedef union{
    gstk_general_cmd_type                          general_cmd;
    gstk_proactive_cmd_type                        proactive_cmd;
    gstk_mode_change_ind_type                      mode_change_ind;
#ifdef FEATURE_CAT_REL6
    gstk_ph_nw_sel_pref_change_ind_type            ph_nw_sel_pref_ind;
#endif
    /* Terminal response confirmation type */
    gstk_unsupported_proactive_cmd_cnf_type        unknown_unsupported_term_rsp_cnf;
    gstk_display_text_cnf_type                     display_text_term_rsp_cnf;
    gstk_get_inkey_cnf_type                        get_inkey_term_rsp_cnf;
    gstk_get_input_cnf_type                        get_input_term_rsp_cnf;
    gstk_launch_browser_cnf_type                   launch_browser_term_rsp_cnf;
    gstk_more_time_cnf_type                        more_time_term_rsp_cnf;
    gstk_play_tone_cnf_type                        play_tone_term_rsp_cnf;
    gstk_poll_interval_cnf_type                    poll_interval_term_rsp_cnf;
    gstk_provide_local_info_cnf_type               provide_local_info_term_rsp_cnf;
    gstk_refresh_cnf_type                          refresh_term_rsp_cnf;
    gstk_run_at_cmd_cnf_type                       run_at_cmd_term_rsp_cnf;
    gstk_select_item_cnf_type                      select_item_term_rsp_cnf;
    gstk_send_sms_cnf_type                         send_sms_term_rsp_cnf;
    gstk_send_ss_cnf_type                          send_ss_term_rsp_cnf;
    gstk_send_ussd_cnf_type                        send_ussd_term_rsp_cnf;
    gstk_setup_call_cnf_type                       setup_call_term_rsp_cnf;
    gstk_setup_evt_list_cnf_type                   setup_evt_list_term_rsp_cnf;
    gstk_setup_menu_cnf_type                       setup_menu_term_rsp_cnf;
    gstk_setup_idle_text_cnf_type                  setup_idle_text_term_rsp_cnf;
    gstk_send_dtmf_cnf_type                        send_dtmf_term_rsp_cnf;
    gstk_timer_manage_cnf_type                     timer_manage_term_rsp_cnf;
    gstk_lang_notification_cnf_type                lang_notification_term_rsp_cnf;
    gstk_open_ch_cnf_type                          open_ch_term_rsp_cnf;
    gstk_close_ch_cnf_type                         close_ch_term_rsp_cnf;
    gstk_send_data_cnf_type                        send_data_term_rsp_cnf;
    gstk_receive_data_cnf_type                     receive_data_term_rsp_cnf;
    gstk_get_ch_status_cnf_type                    get_ch_status_term_rsp_cnf;
    /* raw client terminal response */
    gstk_client_raw_term_resp_type                 client_raw_term_resp;
    /* Envelope Command */
    gstk_envelope_menu_selection_ind_type          menu_selection_envelope_ind;
    gstk_envelope_sms_pp_download_ind_type         sms_pp_envelope_ind;
    gstk_envelope_cell_broadcast_download_ind_type cell_broadcast_envelope_ind;
    gstk_envelope_cc_ind_type                      cc_envelope_ind;
    gstk_envelope_mo_sms_ctrl_ind_type             mo_sms_ctrl_envelope_ind;
    gstk_envelope_mt_evt_ind_type                  mt_evt_envelope_ind;
    gstk_envelope_call_connected_evt_ind_type      call_connected_envelope_ind;
    gstk_envelope_call_disconnected_evt_ind_type   call_disconnected_envelope_ind;
    gstk_envelope_idle_scrn_evt_ind_type           idle_scrn_evt_envelope_ind;
    gstk_envelope_user_act_evt_ind_type            user_act_evt_envelope_ind;
// need this???????????????????????
    gstk_envelope_location_statue_ind_type         location_st_envelope_ind;

    gstk_envelope_timer_expire_ind_type            timer_expire_envelope_ind;
    gstk_envelope_browser_term_evt_ind_type        browser_term_evt_envelope_ind;
    gstk_envelope_lang_sel_evt_ind_type            lang_sel_evt_envelope_ind;
    gstk_envelope_data_avail_evt_ind_type          data_avail_evt_envelope_ind;
    gstk_envelope_ch_status_evt_ind_type           ch_status_evt_envelope_ind;
    gstk_envelope_access_tech_ind_type             access_tech_evt_envelope_ind;
#ifdef FEATURE_CAT_REL6
    gstk_envelope_nw_search_mode_evt_ind_type      nw_search_mode_evt_envelope_ind;
#endif /* #ifdef FEATURE_CAT_REL6 */
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    /* Envelope Response */
    gstk_envelope_resp_type                        envelope_response;

    /* Terminal Profile download command supports differentiating sat and usat */
    gstk_terminal_profile_cmd_type                 terminal_profile_cmd;

    /* Terminal Profile response */
    gstk_term_profile_rsp_type                     terminal_profile_rsp;

    /* gsdi event notification */
    gstk_gsdi_evt_type                             gsdi_evt;

    /* mm enter idle notification */
    gstk_mm_idle_state_type                        mm_idle_state;

    /*  rrc cell change notification */
    gstk_rrc_cell_change_ind_type                  rrc_cell_change_ind;

    /* stop sig command */
    gstk_power_down_type                           power_off;

    /* Error messages type */
    gstk_error_msg_type                            error_msg;

    /* Report Timer type */
    gstk_report_time_msg_type                      timer_msg;

    /* client id registration */
    gstk_client_id_reg_type                        client_id_reg;

    /* client event registration */ 
    gstk_client_evt_reg_type                       client_evt_reg;
}
gstk_cmd_type;




/* The common command header for all commands received by GSDI task */
typedef struct {
    cmd_hdr_type              cmd_hdr;
    gstk_cmd_type             cmd;
}
gstk_task_cmd_type;


// need this?  tingmui!!!!!!!!!!!!
/*gstki.h includes APIs between DMSS layers for the following (U)SAT related cmds:
** Polling Interval -> UIM
** Refresh          -> GSDK
** Location Info    -> MM, RR, RRC
** Location Status  -> CM
** Run AT Cmd       -> DS  (? since there is alpha tag )
*/

/* Used in Refresh cmd
** when length != 0
**      => Refresh should be performed on the USIM Application indicated
**         in the AID
** when length == 0
**      => Refresh should be performed on the current USIM App
*/
typedef struct
{
   uint8                        length;
   uint8                        *aid;
}
gstk_aid_type;

/* various refresh type, as specified in the command detail */
/* GSDI will based on this information to refresh its cache
and acknowledge other layers????????? */
typedef enum
{
   INITIALIZATION_FULL_FCN        = 0x00,
   FCN                            = 0x01,
   INITIALIZATION_FCN             = 0x02,
   INITIALIZATION                 = 0x03,
   USIM_RESET                     = 0x04,
   USIM_APPLICATION_RESET         = 0x05,
   A_3G_SESSION_RESET             = 0x06
}
gstk_refresh_enum_type;



/* Refresh */
/* GSTK <-> GSDK */
typedef struct
{
   uint8                              command_number;
   gstk_refresh_enum_type             refresh_qualifer;
   /* C */
   gstk_file_list_type                files; /* gstk_exp.h */
   /* Optional */
   gstk_aid_type                      aid;
}
gstk_refresh_req_type;


/*===========================================================================
FUNCTION gstk_util_populate_card_cmd_memory

DESCRIPTION
  This function allocates and copies memory for the command pointer input.

PARAMETERS
  old_p: [Input] Pointer to message to which the new gstk_cmd_from_card_type
                 will be copied from
  new_p: [Input] Pointer to new gstk_cmd_from_card_type

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
extern gstk_status_enum_type gstk_util_populate_card_cmd_memory(
    const gstk_cmd_from_card_type * old_p,
    gstk_cmd_from_card_type       * new_p);

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
extern void gstk_util_release_card_cmd_memory(
    gstk_cmd_from_card_type * message_p );

/*===========================================================================
FUNCTION gstk_send_proactive_cmd

DESCRIPTION

  This function allows clients to put a proactive cmd request before parsing
  onto GSTK command queue.

PARAMETERS
  data_length: [Input] Length of the data
  data: [Input] Pointer to the data buffer that contains the Proactive cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_proactive_cmd(
  uint32        data_length,                                                   
  const uint8  *data );

/*===========================================================================
FUNCTION   gstk_uim_fetch_allow

DESCRIPTION 
  Library function called by UIM to check if UIM should do fetch or not
  depending on the following two condtions:
  1. whether the next command in gstk queue BIP Envelope or not
  2. whether there is a BIP Envelope that has been sent to UIM from gstk 
     for which gstk has not received a response yet.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_uim_fetch_allow(void);

/*===========================================================================
FUNCTION gstk_is_gprs_cc_required

DESCRIPTION
  Utility function to check if call control for PDP Context Activation
  is required for any GPRS Data calls

PARAMETER
  Slot ID

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
extern boolean gstk_is_gprs_cc_required(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_is_sms_pp_supported

DESCRIPTION
  Utility function to check if SMS PP Download is allowed/supported

PARAMETER
  None

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
extern boolean gstk_is_sms_pp_supported(void);

/*===========================================================================
FUNCTION gstk_is_cell_broadcast_dl_supported

DESCRIPTION
  Utility function to check if Cell Broadcast Download is allowed/supported

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

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
extern boolean gstk_is_cell_broadcast_dl_supported(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_is_mo_sms_ctrl_required

DESCRIPTION
  Utility function to check if mo sms control is required for any MO calls

PARAMETER
  None

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
extern boolean gstk_is_mo_sms_ctrl_required(void);

/*===========================================================================
FUNCTION gstk_cmd

DESCRIPTION

  This function allows clients to put the gstk_cmd onto the GSTK command
  queue.

PARAMETER
  gstk_cmd [Input]:  Pointer of the task_cmd_type

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
extern void gstk_cmd(gstk_task_cmd_type *gstk_cmd);

/*===========================================================================
FUNCTION gstk_task_get_cmd_buf

DESCRIPTION
  This function get the gstk_task_cmd_type from the GSTK free queue.
  Calling client SHOULD NOT change information on the gstk_task_cmd_type's
  header.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_task_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_task_cmd_type *gstk_task_get_cmd_buf ( void  );

/*===========================================================================
FUNCTION gstk_task

DESCRIPTION
  GSTK Main Task

PARAMETER
  dummy [Input]:  no use!

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
extern void gstk_task(dword dummy);

/*===========================================================================
FUNCTION gstk_slot_is_cc_required

DESCRIPTION
  Utility function to check if call control is required for any MO calls

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

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
extern boolean gstk_slot_is_cc_required(gstk_slot_id_enum_type slot);

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
extern void gstk_util_dereg_client_type(gstk_client_type client_type, uint32 dereg_evt);
#endif /*FEATURE_MMGSDI_GSTK_TEST_AUTOMATION || FEATURE_GSDI_GSTK_CMD_UNIT_TESTER_APP*/

/*===========================================================================
FUNCTION gstk_is_ussd_object_supported

DESCRIPTION
  Utility function to check if the USSD object in call control is supported
  by the SIM or not.

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

DEPENDENCIES
  Client is required to check gstk_slot_is_cc_required or gstk_is_cc_required
  together with this function in order to determine if USSD call control is
  required before sending down the envelope command for USSD call control

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_is_ussd_object_supported(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_get_curr_cell_id_info

DESCRIPTION
  Function called to retrieve the Cell ID information based on the RAT
  and current PLMN Information.

PARAMETER
  Input:   sys_radio_access_type_e_type radio_access_tech : Current RAT
  Input:   uint8  * mcc_mnc_p                 : Current PLMN Information
  Input:   uint8  * lac_p                     : Current LAC  Information
  Input:   gstk_cell_id_type * cell_id_info_p : Structure to populated
  Output:  cell_id_info_p->cell_id            : cell id
  Output:  cell_id_info_p->cell_id_len        : lenght of cell id

DEPENDENCIES
  TODO

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_get_curr_cell_id_info(
  sys_radio_access_tech_e_type radio_access_tech,
  const uint8       *          mcc_mnc,
  const uint8       *          lac,
  gstk_cell_id_type *          cell_id_info_p
);

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_get_curr_tk_slot

DESCRIPTION
  Utility to return the current GSTK toolkit Slot.

PARAMETER
  None

DEPENDENCIES
  Applicable to Dual Slot functionality only

RETURN VALUE
  gstk_slot_id_enum_type
  GSTK_SLOT_1
  GSTK_SLOT_2
  GSTK_SLOT_NONE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_slot_id_enum_type gstk_get_curr_tk_slot(void);
#endif /* FEATURE_GSTK_DUAL_SLOTS */

#ifdef FEATURE_GSTK_NMR_UTRAN_SUPPORT
/*===========================================================================
FUNCTION   gstk_queue_rrc_utran_nmr_rsp

DESCRIPTION 
  Library function called by RRC to post an NMR response to GSTK for network 
  measurement results for utran.

PARAMETERS
  status:       result status from RRC
  length:       Length of data returned
  nmr_info_ptr: UTRAN NMR info returned from RRC
  data_ptr:     GSTK reference data returned back by RRC

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  Max length of bytes sent by RRC should <= 0xFF

SIDE EFFECTS
  Will queue a UTRAN NMR response to GSTK

SEE ALSO
  None
===========================================================================*/
extern void gstk_queue_rrc_utran_nmr_rsp(
  gstk_utran_status_enum_type status,
  uint32                      length, 
  const uint8               * nmr_info_ptr,
  const void                * data_ptr);
#endif /* FEATURE_GSTK_NMR_UTRAN_SUPPORT */

#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */

#endif /* GSTK_EXPORT_H */


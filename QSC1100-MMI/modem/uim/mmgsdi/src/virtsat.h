#ifndef VIRTSAT_H
#define VIRTSAT_H
/*===========================================================================

       V I R T U R A L   S A T   A P P L I C A T I O N   H E A D E R

DESCRIPTION
  This file contain data declarations for the Virtual SAT/USAT Application.

  The Virtual Sat will emulate both technologies SAT as specified in 11.14
  and USAT as specified in 31.111

  The Virtual SAT comes with 3 Apps.  The 3 Apps have various levels of
  complexity based on the Feature Defined:
  FEATURE_VIRTSAT_0_LEVEL_APPS
  FEATURE_VIRTSAT_1_LEVEL_APPS
  FEATURE_VIRTSAT_2_LEVEL_APPS



Copyright (c) 2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/virtsat.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/04   tml     Change report type to include data
05/27/03   jar     Lint Fixes. Added DOG Reporting.
11/05/02   jar     Initial Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_VIRTUAL_SAT
#include "uim.h"
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

//testing apps features
//#define FEATURE_TEST_DISPLAY_TEXT
//#define FEATURE_TEST_SETUP_CALL1
//#define FEATURE_TEST_SETUP_CALL2
//#define FEATURE_TEST_GET_INPUT_CHAR1
//#define FEATURE_TEST_GET_INPUT_DIGIT1
//#define FEATURE_TEST_GET_INPUT_DEFAULT1
//#define FEATURE_TEST_GET_INPUT_CHAR2
//#define FEATURE_TEST_GET_INPUT_DIGIT2
//#define FEATURE_TEST_GET_INPUT_DEFAULT2
//#define FEATURE_TEST_PLAY_TONE1
//#define FEATURE_TEST_PLAY_TONE2
//#define FEATURE_TEST_PLAY_TONE3
//#define FEATURE_TEST_PLAY_TONE4
//#define FEATURE_TEST_PLAY_TONE5
//#define FEATURE_TEST_PLAY_TONE6
//#define FEATURE_TEST_SMS_TPDU_ONLY
//#define FEATURE_TEST_SMS_ALL
//#define FEATURE_TEST_SMS_ALPHA_TPDU
//#define FEATURE_TEST_SMS_ADDRESS_ALPHA_ERROR
//#define FEATURE_TEST_GET_INKEY_CHAR
//#define FEATURE_TEST_GET_INKEY_YES_NO
//#define FEATURE_TEST_GET_INKEY_DIGIT
//#define FEATURE_TEST_MORE_TIME
//#define FEATURE_TEST_SETUP_IDLE_MODE_TEXT
//#define FEATURE_TEST_SS_SS_STRING_ONLY
//#define FEATURE_TEST_SS_ALL
//#define FEATURE_TEST_SS_NO_STRING_ERROR
//#define FEATURE_TEST_PROVIDE_IMEI
//#define FEATURE_TEST_PROVIDE_LOCATION
//#define FEATURE_TEST_PROVIDE_NMR
//#define FEATURE_TEST_PROVIDE_DATE
//#define FEATURE_TEST_BCCH
//#define FEATURE_TEST_LANG
//#define FEATURE_TEST_TIMING_ADV
//#define FEATURE_TEST_SETUP_EVT
//#define FEATURE_TEST_POLLING
//#define FEATURE_TEST_USSD_STRING_ONLY
//#define FEATURE_TEST_USSD_ALL
//#define FEATURE_TEST_USSD_ERROR
//#define FEATURE_TEST_LAUNCH_BROWSER_ALL
#define FEATURE_TEST_LAUNCH_BROWSER_URL_ALPHA

#define FEATURE_0_LEVEL_APPS
#define VIRTSAT_CMD_Q_SIG        0x0001
#define VIRTSAT_CMD_RSP_SIG      0x0002
#define VIRTSAT_SEND_CMD_NOW_SIG 0x0004
#define VIRTSAT_RPT_TIMER_SIG    0x0008

#define VIRTSAT_SEND_CMD_TIME    9000


#define VIRTSAT_MAX_DATA_BLOCK_LEN 256
#define VIRTSAT_MAX_APP_CMD_RSP    10

/* Envelope Command Menu Selection */
#define VIRTSAT_ENV_TAG_OFFSET        0x00
#define VIRTSAT_ENV_TOT_LEN_OFFSET        0x01
#define VIRTSAT_ENV_DEV_ID_OFFSET       0x02
#define VIRTSAT_ENV_DEV_ID_LEN_OFFSET   0x03
#define VIRTSAT_ENV_DEV_ID_SRC_OFFSET   0x04
#define VIRTSAT_ENV_DEV_ID_DES_OFFSET   0x05
#define VIRTSAT_ENV_ITEM_ID_OFFSET      0x06
#define VIRTSAT_ENV_ITEM_ID_LEN_OFFSET    0x07
#define VIRTSAT_ENV_ITEM_ID_SEL_OFFSET    0x08

#define VIRTSAT_MEUN_SEL_TAG_99       0xD3
#define VIRTSAT_MENU_SEL_DEV_ID_TAG     0x81
#define VIRTSAT_ENV_ITEM_ID_TAG       0x90
/* Envelope Command SMS-PP */
#define VIRTSAT_SMS_PP_DL_TAG_99      0xD1

/* Envelope Command Cell Broadcast */
#define  VIRTSAT_CELL_BC_DOWNLOAD_TAG_99 0xD2

/* Envelope Command Call Control */
#define VIRTSAT_CALL_CNTL_TAG_99    0xD4

#define VIRTSAT_DEVICE_IDENTITY_TAG   0x82
#define VIRTSAT_DEVICE_IDENTITY_LEN     2
#define VIRTSAT_DEVICE_KEYPAD     0x01
#define VIRTSAT_DEVICE_DISPLAY      0x02
#define VIRTSAT_DEVICE_EARPIECE     0x03
#define VIRTSAT_DEVICE_CARD_READER    0x10
#define VIRTSAT_DEVICE_CARD_READER1   0x11
#define VIRTSAT_DEVICE_CARD_READER2   0x12
#define VIRTSAT_DEVICE_CARD_READER3   0x13
#define VIRTSAT_DEVICE_CARD_READER4   0x14
#define VIRTSAT_DEVICE_CARD_READER5   0x15
#define VIRTSAT_DEVICE_CARD_READER6   0x16
#define VIRTSAT_DEVICE_CARD_READER7   0x17
#define VIRTSAT_DEVICE_CHANNEL        0x20
#define VIRTSAT_DEVICE_CHANNEL1       0x21
#define VIRTSAT_DEVICE_CHANNEL2       0x22
#define VIRTSAT_DEVICE_CHANNEL3       0x23
#define VIRTSAT_DEVICE_CHANNEL4       0x24
#define VIRTSAT_DEVICE_CHANNEL5       0x25
#define VIRTSAT_DEVICE_CHANNEL6       0x26
#define VIRTSAT_DEVICE_CHANNEL7       0x27
#define VIRTSAT_DEVICE_SIM        0x81
#define VIRTSAT_DEVICE_ME       0x82
#define VIRTSAT_DEVICE_NETWORK        0x83

typedef enum {
  /* Additional UIM Request Types */
  VIRTSAT_NO_COMMAND = 0,
  VIRTSAT_TERMINAL_PROFILE_F,
  VIRTSAT_TERMINAL_PROFILE_R,
  VIRTSAT_TERMINAL_RESPONSE_F,
  VIRTSAT_ENVELOPE_F,
  VIRTSAT_BUILD_SETUP_MENU_F,
  VIRTSAT_BUILD_SETUP_MENU_TR_R,
  VIRTSAT_BUILD_DISPLAY_TEXT_F,
  VIRTSAT_BUILD_DISPLAY_TEXT_TR_R,
  VIRTSAT_PROACTIVE_CMD_F,
  VIRTSAT_RESET_F,
  VIRTSAT_RESET_R,
  VIRTSAT_RESPONSE_R,
}virtsat_transaction_name_type;

typedef enum {
  VIRTSAT_NO_SUCH_MODE = 0,  /* No such mode */
  VIRTSAT_CDMA         = 1,  /* CDMA mode of operation */
  VIRTSAT_GSM          = 2,  /* GSM mode of operation */
  VIRTSAT_WCDMA        = 3,  /* USIM mode of operation */
  VIRTSAT_MAXIMUM_MODE = 4
}virtsat_protocol_type;

typedef enum{
  VIRTSAT_SUCCESS = 0,
  VIRTSAT_ERROR
}virtsat_result_type;

typedef struct {
  void                (*uim_rpt_function)( uim_rpt_type * );
  virtsat_result_type  virtsat_result;
  byte                 sw1;
  byte                 sw2;
} virtsat_sw1sw2_type;


typedef struct {
  virtsat_transaction_name_type transaction;
  virtsat_sw1sw2_type           status_report;
  byte                          num_bytes;
  byte                          data[255];
} virtsat_report_type;

typedef struct{
  q_link_type                   link;
  virtsat_transaction_name_type transaction;
  virtsat_protocol_type         protocol;
  void                          (*uim_rpt_function)( uim_rpt_type * );
  void                          (*virtsat_rpt_function)( virtsat_report_type * );
  virtsat_sw1sw2_type           status_report;
} virtsat_hdr_type;

typedef struct {
  virtsat_hdr_type  hdr;
  byte        num_bytes;
  byte        data[255];
}virtsat_terminal_profile_type;

typedef struct {
  virtsat_hdr_type  hdr;
  byte        num_bytes;
  byte        data[255];
}virtsat_envelope_cmd_type;


typedef struct {
  virtsat_hdr_type  hdr;
  byte        num_bytes;
  byte        data[255];
}virtsat_proactive_cmd_type;

typedef struct {
  virtsat_hdr_type  hdr;
  byte        num_bytes;
  byte        data[255];
}virtsat_terminal_response_type;

typedef union {
  virtsat_hdr_type                  hdr;
  virtsat_terminal_profile_type     terminal_profile;
  virtsat_proactive_cmd_type        proactive_cmd;
  virtsat_envelope_cmd_type         envelope_cmd;
  virtsat_terminal_response_type    terminal_response;
} virtsat_transaction_type;

typedef struct {
  byte proactive_cmd_len;
  byte proactive_cmd_buffer[100];
  byte proactive_exp_tp_rsp_len;
  byte proactive_exp_tr_buffer[50];
}virtsat_app_cmd_rsp_element_type;

typedef struct {
  byte app_id;
  virtsat_app_cmd_rsp_element_type cmd_rsp[10];
}virtsat_app_type;

typedef struct {
  byte setup_menu_len;
  byte setup_menu_data[100];
}virtsat_setup_menu_type;

typedef struct {
  byte app_id;
  uint32 proactive_cmd_len;
  byte proactive_cmd[100];
  uint32 proactive_exp_tp_rsp_len;
  byte proactive_exp_tp[50];
}virtsat_app_element_type;


typedef enum{
  VIRTUIM_APP_MODE_SAT  = 0,
  VIRTUIM_APP_MODE_USAT,
  VIRTUIM_APP_MODE_NONE = 2
}virtuim_app_mode_type;


typedef struct {
  byte    setup_menu[255];
}virtsat_main_menu_type;

typedef struct {
  byte  command_indexes[10];
  byte    terminal_rsp_indexes[10];
  byte    envelope_cmds_indexs[10];
} virtsat_app_cmd_type;

typedef struct {
  byte  response[10];
  byte    incase_fail[5];
} virtsat_terminal_rsp_type;

typedef struct {
  byte app_id;
  byte proactive_cmd_index;
  byte terminal_res_index;
  byte envelope_cmd_index;
}virtsat_app_info_type;



/*===========================================================================
FUNCTION VIRTUIM_PROCESS_TERMINAL_PROFILE

DESCRIPTION
  Will be used to parse and dump the Terminal Profile Download to the
  Diagnostics Monitor.

DEPENDENCIES
  The calling task sending the Terminal Profile download must adhere to
  3GPP TS 31.111

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
virtsat_result_type virtuim_process_terminal_profile(
  virtsat_transaction_type *virtsat_cmd_ptr
);

/*===========================================================================
FUNCTION VIRTUIM_PROCESS_TERIMAL_RESPONSE

DESCRIPTION
  Will be used to handle and check the contents of the Termain Response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
virtsat_result_type virtuim_process_terminal_response( void );

/*===========================================================================
FUNCTION VIRTUIM_PROCESS_ENVELOPE

DESCRIPTION
  Will be used to handle and check the contents of then Envelope Command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
virtsat_result_type virtuim_process_envelope( void );

/*===========================================================================
FUNCTION virtsat_main

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtsat_main
(
  dword dummy
    /* Required for REX, ignore */
    /*lint -esym(715,dummy)
    ** Have lint not complain about the ignored parameter 'dummy' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

/*===========================================================================
FUNCTION virtsat_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
extern void virtsat_cmd
(
  virtsat_transaction_type *virtsat_cmd_ptr
);

/*===========================================================================
FUNCTION virtsat_get_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtsat_transaction_type *virtsat_get_cmd_buf(void);

#endif /* FEATURE_VIRTUAL_SAT */

#endif /* VIRTSAT_H */

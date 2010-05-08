#ifndef VIRTUIM_H
#define VIRTUIM_H
/*===========================================================================

               V I R T U R A L   U I M   S E V E R   H E A D E R

DESCRIPTION
  This file contain data declarations for the Virtual UIM Server.

  The data supports both GSM SIM and 3G USIM data. Two pools of data exists.

  One is for EF data of a select number of EFs.
  uim_sim[UIM_NUM_FILES] found in virtuim.c

  The other is for EF header information of a select number of EFs.
  gsdi_sim_file_attr[UIM_NUM_FILE_ATTR] found in virtuim.c
   Note: GSM SIM EFs and 3G EFs have different header formats.


Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/gsdi/vcs/virtuim.h_v   1.6.1.0   03 Jun 2002 17:15:26   atan  $
$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/virtuim.h#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/06   tml     Added Multi UICC App support
06/24/06   jar     Added support for BDN Files for Single Slot.
01/05/06   tml     Added support for more ADN
03/24/05   tml     Added USIM support
10/27/04   sk      Changed data types from byte to uint32/int32 in
                   uim_server_read_sim and uim_server_read_attr.
08/11/04    sk     Added files for FEATURE_MMGSDI_CPHS
05/18/04   jar     Channel Management Support
05/04/04   tml     Compilation fix
04/12/04   tml     Distinguish between uim_sim and uim_sim_2 buffers
03/31/04   tml     Code clean up
03/16/03   tml     Added support for Unit Testing.
05/19/03   jar     Fix Lint Errors
05/13/03   tml     Increased UIM_NUM_FILES and UIM_NUM_FILE_ATTR for CDMA
                   SMS support
01/24/02   jar     Increase UIM_NUM_FILES to 102.
06/03/02   at      Commented out UIM_MAX_CHARS since this is picked up from
                   uimdrv.h
03/14/02   jar     Added #define UIM_TELECOM_LND_LEN  28
03/14/02   jar     Increased the values of UIM_NUM_FILES and UIM_NUM_FILE_ATTR
                   for the addition of EFs and their Headers to support PBM.
                   Added UIM_GSM_ECC_LEN, UIM_USIM_ECC_LEN, and UIM_TELECOM_LND_LEN.
02/03/02   jar     Increased UIM_NUM_FILE_ATTR to 28
01/16/02   jar     Increased UIM_MAX_IMSIS_IN_CACHE To 17
01/15/02   jar     Added functions to write items to NV and get items from
                   NV. Added a Sig VIRTUIM_NV_SIG 0xABC1 to be used with
                   NV.  Added UIM_MAX_IMSIS_IN_CACHE 16 to define Max
                   number of IMSIs.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "cmd.h"
#include "uim.h"
#include "nv.h"
#include "gsdi_exp.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

//#define UIM_MAX_CHARS   256
#define UIM_MAX_AID_SIZE 17

#define UIM_GSM_ECC_LEN      3
#define UIM_USIM_ECC_LEN     58
#define UIM_TELECOM_LND_LEN  28
#define UIM_TELECOM_ADN_LEN  28
#define UIM_TELECOM_FDN_LEN  28
#define UIM_TELECOM_BDN_LEN  28
#define UIM_USIM_ICI_LEN     28
#define UIM_USIM_OCI_LEN     28
#define UIM_ICC_ID_LEN       GSDI_ICC_IC_LEN
#define UIM_IMSI_LEN         GSDI_IMSI_LEN
#define UIM_KC_LEN           (GSDI_KC_LEN + 1) /* allow space for seqno*/
#define UIM_HPLMN_SP_LEN     GSDI_HPLMN_SP_LEN
#define UIM_ACM_MAX_LEN      GSDI_ACM_MAX_LEN
#define UIM_SST_LEN          10
#define UIM_VMWI_LEN         2
#define UIM_CFF_LEN          2
#define UIM_ONS_LEN          24
#define UIM_CSP_LEN          24
#define UIM_CPHSI_LEN        3
#define UIM_MN_LEN           24
#define UIM_UST_LEN          10
#define UIM_ACM_LEN          GSDI_ACM_LEN
#define UIM_SPN_LEN          GSDI_SPN_LEN
#define UIM_PUCT_LEN         GSDI_PUCT_LEN
#define UIM_BCCH_LEN         GSDI_BCCH_LEN
#define UIM_ACC_LEN          GSDI_ACC_LEN
#define UIM_FPLMN_LEN        GSDI_FPLMN_LEN
#define UIM_LOCI_LEN         GSDI_LOCI_LEN
#define UIM_PHASE_LEN        GSDI_PHASE_LEN
#define UIM_LOCIGPRS_LEN     GSDI_LOCIGPRS_LEN
#define UIM_SMS_LEN          GSDI_SMS_LEN
#define UIM_SMSS_LEN         GSDI_SMSS_LEN
#define UIM_SMSR_LEN         GSDI_SMSR_LEN
#define UIM_SMSP_LEN         GSDI_SMSP_LEN
#define UIM_DCK_LEN          16
#define UIM_CACHE_NETPAR_LEN        10
#define UIM_MAX_FILE_LEN            255
#define UIM_ATTR_MAX_FILE_LEN       60
/*lint -e760 */
#define UIM_MAX_CHV_DIGITS          8
/*lint +e760 */
#ifdef FEATURE_MMGSDI_CPHS
  #define UIM_NUM_FILES               194
  #define UIM_NUM_FILE_ATTR           58
#else
  #define UIM_NUM_FILES               184
  #define UIM_NUM_FILE_ATTR           52
#endif /* FEATURE_MMGSDI_CPHS */
#define UIM_NUM_FILES_2             126
#define UIM_RAND_LEN                16
#define UIM_AUTN_LEN                18
#define UIM_DEFAULT_FILE_LEN        0
#define UIM_DEFAULT_FILE_HEADER_LEN 50
#define UIM_MAX_IMSIS_IN_CACHE      17
#define VIRTUIM_NV_SIG              0xABC1  /* Return from NV */


typedef struct {
  boolean             slot_present;
  boolean             gsm_present;
  boolean             cdma_present;
  boolean             umts_present;
  gsdi_pin_status_T   pin1_status;
  boolean             tk_from_gsm;
  boolean             tk_from_cdma;
} virtuim_slot_info_type;

typedef enum {
  VIRTUIM_UMTS_APP,
  VIRTUIM_GSM_APP,
  VIRTUIM_CDMA_APP
} virtuim_app_mode;

typedef struct {
    uim_slot_type                     gsm_op_slot;
    uim_slot_type                     cdma_op_slot;
    uim_slot_type                     umts_op_slot;
    virtuim_slot_info_type            slot1;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    virtuim_slot_info_type            slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
} virtuim_card_status_type;

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#define VIRTUIM_EVENT_TESTER_RESTART_POWER_UP_SIG 0x1000000
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
#define VIRTUIM_GSTK_SIG             0x20000
#define VIRTUIM_WAIT_FOR_VIRTSAT_SIG 0x40000  /* Return from Virtual SAT */
#define VIRTUIM_SEND_SAT_CMD_SIG     0x80000  /* Send Command to GSTK */
#define VIRTUIM_ME_SUPPORTS_STRINGS  50

typedef enum {
    VIRTUIM_PROACTIVE_CMD = 0,
    VIRTUIM_MAX = 1
} virtuim_cmd_enum_type;

typedef enum {
    VIRTUIM_SUCCESS = 0,
    VIRTUIM_ERROR = 1
} virtuim_result_type;

typedef void (*virtsat_proactive_rpt_function)(
  virtuim_result_type report
);

typedef struct {
    q_link_type                     link;
    virtuim_cmd_enum_type           cmd;
    virtsat_proactive_rpt_function  virtuim_result_cb;
} virtuim_cmd_hdr_type;

typedef struct {
    virtuim_cmd_hdr_type hdr;
    byte    data_buffer[255];
    byte    data_len;
}virtuim_cmd_type;


/* Terminal Profile Download */
#define VIRTUIM_PROFILE_DOWNLOAD_OFFSET             0x00
#define VIRTUIM_PROFILE_DOWNLOAD_MASK               0x01
#define VIRTUIM_SMS_PP_DATA_DOWNLOAD_OFFSET         0x00
#define VIRTUIM_SMS_PP_DATA_DOWNLOAD_MASK           0x02
#define VIRTUIM_CELL_BROADCAST_DATA_DOWNLOAD_OFFSET 0x00
#define VIRTUIM_CELL_BROADCAST_DATA_DOWNLOAD_MASK   0x04
#define VIRTUIM_MENU_SELECTION_OFFSET               0x00
#define VIRTUIM_MENU_SELECTION_MASK                 0x08
#define VIRTUIM_SMS_PP_DATA_DL_SUPPORTED_OFFSET     0x00
#define VIRTUIM_SMS_PP_DATA_DL_SUPPORTED_MASK       0x10
#define VIRTUIM_TIMER_EXPIRATION_OFFSET             0x00
#define VIRTUIM_TIMER_EXPIRATION_MASK               0x20
#define VIRTUIM_USIM_CC_SUPPORTED_BIT_1_OFFSET      0x00
#define VIRTUIM_USIM_CC_SUPPORTED_BIT_1_MASK        0x40
#define VIRTUIM_USIM_CC_SUPPORTED_BIT_2_OFFSET      0x00
#define VIRTUIM_USIM_CC_SUPPORTED_BIT_2_MASK        0x80

#define VIRTUIM_PROACTIVE_UICC_COMMAND_TAG      0xD0

/* Proactive Commands */
typedef enum
{
    VIRTUIM_REFRESH = 0,
    VIRTUIM_MORE_TIME,
    VIRTUIM_POLL_INTERVAL,
    VIRTUIM_POLLING_OFF,
    VIRTUIM_SET_UP_EVENT_LIST,
    VIRTUIM_SET_UP_CALL,
    VIRTUIM_SEND_SS,
    VIRTUIM_SEND_USSD,
    VIRTUIM_SEND_SHORT_MESSAGE,
    VIRTUIM_SEND_DTMF,
    VIRTUIM_LAUNCH_BROWSER,
    VIRTUIM_PLAY_TONE,
    VIRTUIM_DISPLAY_TEXT,
    VIRTUIM_GET_INKEY,
    VIRTUIM_GET_INPUT,
    VIRTUIM_SELECT_ITEM,
    VIRTUIM_SETUP_MENU,
    VIRTUIM_PROVIDE_LOCAL_INFORMATION,
    VIRTUIM_TIMER_MANAGEMENT,
    VIRTUIM_SETUP_IDEL_MODEL_TEXT,
    VIRTUIM_PERFORM_CARD_APDU,
    VIRTUIM_POWER_ON_CARD,
    VIRTUIM_POWER_OFF_CARD,
    VIRTUIM_GET_READER_STATUS,
    VIRTUIM_RUN_AT_COMMAND,
    VIRTUIM_LANGUAGE_NOTIFICATION,
    VIRTUIM_OPEN_CHANNEL,
    VIRTUIM_CLOSE_CHANNEL,
    VIRTUIM_RECEIVE_DATA,
    VIRTUIM_SEND_DATA,
    VIRTUIM_GET_CHANNEL_STATUS
}virtuim_practive_command_type;


#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

typedef enum
{
  VIRTUIM_LINEAR_FIXED,
  VIRTUIM_TRANSPARENT,
  VIRTUIM_CYCLIC
}
virtuim_file_type;

/* SIM data element definitions */
typedef struct
{
   uim_items_enum_type   filename;      /* EF enum name */
   boolean               init;         /* Availability of EF */
   virtuim_file_type     file_type; /* Transparent or linear file type */
                                       /*   FALSE = transparent  */
                                       /*   TRUE = linear fixed  */
   byte                  rec_number;   /* If linear fixed, rec num of EF */
   byte                  num_records;  /* If linear fixed, total num records */
   byte                  rec_len;      /* Length of data for either tranparent */
                                       /*   or linear fixed records */
   byte                  body[UIM_MAX_FILE_LEN];  /* data */
}
uim_sim_element_T;


/* SIM header element definitions */
typedef struct
{
   uim_items_enum_type  filename;  /* EF enum name */
   byte                  rec_len;  /* Length of header data */
   byte                  body[UIM_ATTR_MAX_FILE_LEN]; /* data */
}
uim_sim_file_attr_element_T;


extern uim_sim_element_T uim_sim2[UIM_NUM_FILES_2];
extern uim_sim_element_T uim_sim[UIM_NUM_FILES];

/*===========================================================================

                                GLOBALS

============================================================================*/
//static nv_cmd_type nvi;           /* nv command buffer */


/*===========================================================================
FUNCTION test_sim_server_task

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
extern void test_sim_server_task(dword dummy);


/*===========================================================================
FUNCTION virtuim_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
extern void virtuim_cmd(uim_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION VIRTUIM_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type virtuim_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

/*===========================================================================
FUNCTION VIRTUIM_PUT_NV

DESCRIPTION
  Put an item on to the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done.
===========================================================================*/
nv_stat_enum_type virtuim_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

/*===========================================================================
FUNCTION VIRTUIM_WAIT_NV

DESCRIPTION
  Wait on NV actions.

DEPENDENCIES
  There is a command sent to NV, and this task will be signalled.  There
  are no other outstanding NV requests from this task.

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
static void virtuim_wait_nv( void );

/*===========================================================================
FUNCTION VIRTUIM_GET_EF_FROM_PATH

DESCRIPTION
   This function is used for the Virtual UIM Only.  This function will take
   a pointer to the EF PATH/FID Used in the Call.  It will resolve the PATH/FID
   assumming that the information in the EF-PBR File in Virtuim.c never
   changes.

DEPENDENCIES
   virtuim.c :  UIM_USIM_PBR    Information is assummed to not change
         UIM_TELECOM_PBR  Information is assummed to not change
         Changing the FIDs in the EF-PBR Will create undersired
         results in the operation of this function.

RETURN VALUE
   gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)
===========================================================================*/
gsdi_returns_T virtuim_get_ef_from_path( uim_cmd_type *cmd_ptr );

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
/*===========================================================================
FUNCTION virtuim_app_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_app_cmd(virtuim_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION virtuim_get_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtuim_cmd_type *virtuim_get_cmd_buf(void);

#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

/*===========================================================================
FUNCTION virtuim_set_card_status

DESCRIPTION
  Set the following global data in virtuim for card based on slot provided
  card presence, gsm df, cdma df, umts adf, pin1 disabled, gsm tk, cdma tk

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_set_card_status(gsdi_slot_id_type slot, virtuim_slot_info_type* slot_info);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION virtuim_set_operational_slot

DESCRIPTION
  Set the operational slot for the specific application in the passed in
  parameter

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_set_operational_slot(uim_app_type app, uim_slot_type slot);
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION virtuim_reset_to_power_up

DESCRIPTION
  Reset the just_powered_up parameter to TRUE so that virtuim can restart
  the power up procedure

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_reset_to_power_up (void);


/*===========================================================================
FUNCTION virtuim_restart

DESCRIPTION
  Reset the just_powered_up parameter, and populate the rest of the virtuim
  card status type data

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
extern void virtuim_restart( virtuim_card_status_type *card_status_info);


/*===========================================================================
FUNCTION uim_server_read_sim

DESCRIPTION
  Reads data from the SIM/USIM.

INPUT
  uim_slot_type slot,
  uim_items_enum_type sim_filename,
  byte          requested_data_len,
  byte          data_offset,
  byte *        number_of_records_p,
  byte *        actual_data_len_p,
  byte *        data_p

OUTPUT
  Data read from SIM/USIM to data_p

RETURN VALUE
 gsdi_returns_T  gsdi_status
===========================================================================*/
gsdi_returns_T uim_server_read_sim (
  uim_slot_type slot,
  uim_items_enum_type sim_filename,
  int32               requested_data_len,
  uint32              data_offset,
  int32              *number_of_records_p,
  int32              *actual_data_len_p,
  byte *        data_p
);


/*===========================================================================
FUNCTION uim_server_write_sim

DESCRIPTION
  Writes data to the SIM/USIM.

INPUT
  uim_slot_type slot,
  uim_items_enum_type  sim_filename,
  byte                  data_len,
  byte                  data_offset,
  byte *                data_p

OUTPUT
  data written from data_p to uim_sim_p[file_num]

RETURN VALUE
 gsdi_returns_T  gsdi_status
===========================================================================*/
gsdi_returns_T uim_server_write_sim (
  uim_slot_type        slot,
  uim_items_enum_type  sim_filename,
  byte                 data_len,
  byte                 data_offset,
  byte *               data_p
);

/*===========================================================================
FUNCTION uim_server_read_attr

DESCRIPTION
  Reads file attributes from SIM/USIM

INPUT
  uim_slot_type slot,
  uim_items_enum_type  sim_filename,
  byte *   file_header_len,
  byte *   file_header


OUTPUT
  Reads file attributes from SIM/USIM into file_header

RETURN VALUE
 gsdi_returns_T  gsdi_status
===========================================================================*/
gsdi_returns_T uim_server_read_attr (
  uim_slot_type slot,
  uim_items_enum_type  sim_filename,
  int32               *file_header_len,
  byte                *file_header
);

#endif /* VIRTUIM_H */

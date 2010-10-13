#ifndef GSDI_PERSO_ENGINE_H
#define GSDI_PERSO_ENGINE_H
/*===========================================================================


        G S D I   P E R S O N A L I Z A T I O N   E N G I N E

     S T R U C T U R E S  A N D  F U N C T I O N  P R O T O T Y P E S


DESCRIPTION
  Structure and Function prototype definitions for the personalization
  engine.  The gsdi_perso.c functions are the entry point into the
  engine.  The engine is going to retrieve all of the information required
  to allow the security library to perform the security procedures.


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso_engine.h#2 $$ $DateTime: 2009/04/01 10:26:45 $ $Author: tingl $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/26/09   tml     Fixed double defines and declarations lint errors
11/13/08   nk      Changed #elsif to #elif to fix compiler warning
09/25/08   kk      Added support for extracting unblock dck retries available
04/16/08   kk      Added support of get command on sim lock data
01/17/07   pv      Get rid of gsdi_perso_notify_clients_of_perso_event and
                   externalize gsdi_perso_engine_notify_clients_of_perso_event.
08/03/06   sun     Added new state for card error
04/28/06   sun     Added #define for MAX Length of NV Unlock Code
04/06/06   sun     Added support for RUIM Lock
03/21/06   tml     lint
03/08/06   jar     Merged in Referece Security Library
12/14/05   sun     Fixed more lint errors
11/09/05   sun     Fixed lint errors
10/03/05   sun     Pass the event from the perso engine to the security and back
05/13/05   sun     Consolidated all perso files into one file
05/05/05   sun     Added support for Perso Refresh
05/01/05   jar     Fix for NON QC Security Library Builds
04/30/05   jar     Added an IMEI Unparsed Len
04/28/05   jar     Initial release of the GSDI SIM Lock Architecutre to support
                   Plug In Proprietary Security Libraries driven by a Perso Engine.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "gsdi_exp.h"
#include "uim.h"

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

/* Offsets used when taking data_p information and populating the cnf rsp */
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_NW_STATUS_IND_OFFSET                   0x00
#define GSDI_PERSO_NS_STATUS_IND_OFFSET                   0x01
#define GSDI_PERSO_SP_STATUS_IND_OFFSET                   0x02
#define GSDI_PERSO_CP_STATUS_IND_OFFSET                   0x03
#define GSDI_PERSO_SIM_STATUS_IND_OFFSET                  0x04
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_RUIM_NW1_STATUS_IND_OFFSET  0x00
#define GSDI_PERSO_RUIM_NW2_STATUS_IND_OFFSET  0x01                
#define GSDI_PERSO_RUIM_HRPD_STATUS_IND_OFFSET 0x02                
#define GSDI_PERSO_RUIM_SP_STATUS_IND_OFFSET   0x03                
#define GSDI_PERSO_RUIM_CP_STATUS_IND_OFFSET   0x04                
#define GSDI_PERSO_RUIM_RUIM_STATUS_IND_OFFSET 0x05                
#endif /* FEATURE_PERSO_RUIM */

/* -------------------------------------------------------------------------
   Makeshift request not part of the Normal Set of Requests
   used to fake the Personalization Engine into makeing a request
   for all Personalization Feature Indicators
   -----------------------------------------------------------------------*/
#define GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS_REQ               0xFF
#define GSDI_PERSO_ENGINE_CHECK_ALL_LOCKS_REQ                    0xFE
#define GSDI_PERSO_ENGINE_CHECK_AUTOLOCKS_REQ                    0xFD
#define GSDI_PERSO_ENGINE_SIM_REFRESH_REQ                        0xFC
#define GSDI_PERSO_ENGINE_SIM_AUTO_LOCK_REQ                      0xFB

#define GSDI_PERSO_ENGINE_RUIM_CHECK_ALL_LOCKS_REQ               0xFA
#define GSDI_PERSO_ENGINE_RUIM_AUTOLOCK_REQ                      0xF9


#define GSDI_PERSO_ENGINE_MAX_CK_W_NULL 0x09

/* Max Number of Clients supported for Personalization Events */
#define GSDI_PERSO_ENGINE_MAX_REG_CLIENTS                        0x0A

/* Non Valid Index Values which indicate different statuses   */
#define GSDI_PERSO_ENGINE_MIN_TABLE_INDEX                        0x0000
#define GSDI_PERSO_ENGINE_EVENT_HANDLER_ALREADY_REGISTERD        0x00FE
#define GSDI_PERSO_ENGINE_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE 0x00FF

#define GSDI_PERSO_ENGINE_SERIAL_NUMBER_LEN    0x16
#define GSDI_PERSO_ENGINE_IMSI_LEN             0x09
#define GSDI_PERSO_ENGINE_GID1_LEN             0x64 /* 100 Bytes */
#define GSDI_PERSO_ENGINE_GID2_LEN             0x64 /* 100 Bytes */
#define GSDI_PERSO_ENGINE_CNL_LEN              0xFF /* 255 Bytes */

#define MS_PERSO_ENGINE_GSDI                   13
#define GSDI_PERSO_ENGINE_IMEI_UNPARSED_LEN    8
#define GSDI_PERSO_ENGINE_IMEI_UNPARSED_NV_LEN  9

#define GSDI_PERSO_ENGINE_NO_IND_INFO          0xFF;

/* ----------------------------------------------------------------------------
   GSDI_PERSO_ENGINE_DCK_CK_LEN
   The maximum length for a Control Key in the DCK EF
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_ENGINE_DCK_CK_LEN              0x04
#define GSDI_PERSO_BUFFER_LEN                     500


#ifdef FEATURE_PERSO_SIM
/* OTA Deperso Message Offsets
 1 -  40  Operator specific text padded with spaces to character 40.
41 -  48  Network control key
49 -  56  Network subset control key
57 -  64  SP control key
65 -  72  Corporate control key
73 -  88  IMEI
*/
#define GSDI_PERSO_OTA_DEPERSO_OP_STOP_OFFSET  39
#define GSDI_PERSO_OTA_DEPERSO_IMEI_OFFSET     72
#define GSDI_PERSO_OTA_DEPERSO_NW_CK_OFFSET    40
#define GSDI_PERSO_OTA_DEPERSO_NS_CK_OFFSET    48
#define GSDI_PERSO_OTA_DEPERSO_SP_CK_OFFSET    56
#define GSDI_PERSO_OTA_DEPERSO_CP_CK_OFFSET    64

#define GSDI_PERSO_OTA_DEPERSO_RSP_NW_OFFSET   16
#define GSDI_PERSO_OTA_DEPERSO_RSP_NS_OFFSET   17
#define GSDI_PERSO_OTA_DEPERSO_RSP_SP_OFFSET   18
#define GSDI_PERSO_OTA_DEPERSO_RSP_CP_OFFSET   19
#endif /* FEATURE_PERSO_SIM */

#define GSDI_PERSO_OTA_DEPERSO_RSP_LEN         20

/* OTA Deperso Depersonalization CK Len */
#define GSDI_PERSO_OTA_DEPERSO_CK_LEN          0x08

/* ----------------------------------------------------------------------------
   Table to hold the list of event handlers
   --------------------------------------------------------------------------*/
extern gsdi_perso_events_cb gsdi_perso_event_handler_table[GSDI_PERSO_ENGINE_MAX_REG_CLIENTS];

typedef enum {
  GSDI_PERSO_ENG_NOT_INIT_S          = 0x00000000,
  GSDI_PERSO_ENG_INIT_S,
  GSDI_PERSO_ENG_CDMA_INIT_S,
  GSDI_PERSO_ENG_INIT_CHECK_LOCKS_S,
  GSDI_PERSO_ENG_CDMA_INIT_CHECK_LOCKS_S,
  GSDI_PERSO_ENG_INIT_AUTOLOCK_S,
  GSDI_PERSO_ENG_CDMA_INIT_AUTOLOCK_S,
  GSDI_PERSO_ENG_WAIT_FOR_DEPERSO_S,
  GSDI_PERSO_ENG_CDMA_WAIT_FOR_DEPERSO_S,
  GSDI_PERSO_ENG_INIT_COMPLETED_S,
  GSDI_PERSO_ENG_INIT_ERROR_S,
  GSDI_PERSO_ENG_CARD_ERROR_S,
  GSDI_PERSO_ENG_CDMA_INIT_ERROR_S,
  GSDI_PERSO_ENG_MAX_S               = 0x7FFFFFFF
}gsdi_perso_eng_init_state_enum_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_SIM_ELE_DATA_TYPE

   DESCRIPTION:  This is the basic SIM Data Element provided to the security
                 library.

                 This structure can be used for the data elements required
                 in the SIM Personalization Procedures.

                 EF-IMSI
                 EF-CNL
                 EF-GID1
                 EF-GID2
   ------------------------------------------------------------------------*/
typedef struct {
  int32   len;
  uint8 * buffer_p;
}gsdi_perso_eng_sim_ele_data_type;


typedef struct {
  gsdi_cmdhdr_T             message_header;
}
gsdi_perso_init_proc_req_T;

typedef enum {
  GSDI_PERSO_ENGINE_ACTIVATE_FEATURE   = 0x00000000,
  GSDI_PERSO_ENGINE_DEACTIVATE_FEATURE,
  GSDI_PERSO_ENGINE_GET_FEATURE_IND,
  GSDI_PERSO_ENGINE_GET_ALL_FEATURE_INDS,
  GSDI_PERSO_ENGINE_SET_DATA_FEATURE,
  GSDI_PERSO_ENGINE_UNBLOCK_DCK,
  GSDI_PERSO_ENGINE_OTA_DERPERSO,
  GSDI_PERSO_ENGINE_INT_PROC,
  GSDI_PERSO_ENGINE_PERM_DISABLE_FEATURE_IND,
  GSDI_PERSO_ENGINE_GET_PERM_FEATURE_IND,
  GSDI_PERSO_ENGINE_GET_DCK_NUM_RETRIES,
  GSDI_PERSO_ENGINE_GET_FEATURE_KEY,
  GSDI_PERSO_ENGINE_CHECK_LOCKS_ALL,
  GSDI_PERSO_ENGINE_AUTO_LOCK,
  GSDI_PERSO_ENGINE_REFRESH,
  GSDI_PERSO_ENGINE_GET_SW_LIB_VERSION,
  GSDI_PERSO_ENGINE_VERIFY_PHONE_CODE,
  GSDI_PERSO_ENGINE_CHANGE_PHONE_CODE,
  GSDI_PERSO_ENGINE_UNBLOCK_PHONE_CODE,
  GSDI_PERSO_ENGINE_SET_PHONE_CODE,
  GSDI_PERSO_ENGINE_GET_PHONE_CODE_STATUS,
  GSDI_PERSO_ENGINE_RUIM_CHECK_LOCKS_ALL,
  GSDI_PERSO_ENGINE_RUIM_AUTO_LOCK,
  GSDI_PERSO_ENGINE_GET_FEATURE_DATA,
  GSDI_PERSO_ENGINE_GET_DCK_UNBLOCK_NUM_RETRIES,
  GSDI_PERSO_ENGINE_LT                = 0x7FFFFFFF
}gsdi_perso_engine_proc_enum_type;

#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS   5
#endif
#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS  6
#endif

#if defined(FEATURE_PERSO_SIM) && defined(FEATURE_PERSO_RUIM)
#define GSDI_PERSO_ENGINE_TOTAL_LOCKS GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS + GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS
#elif defined(FEATURE_PERSO_SIM)                                         
#define GSDI_PERSO_ENGINE_TOTAL_LOCKS GSDI_PERSO_ENGINE_SIM_NUM_OF_LOCKS
#elif defined(FEATURE_PERSO_RUIM)
#define GSDI_PERSO_ENGINE_TOTAL_LOCKS GSDI_PERSO_ENGINE_RUIM_NUM_OF_LOCKS
#endif

#define GSDI_PERSO_ENGINE_TAG_SIZE      sizeof(uint8)
#define GSDI_PERSO_ENGINE_LEN_SIZE      (sizeof(uint8) * 0x02)
#define GSDI_PERSO_ENGINE_HI_LEN        sizeof(uint8)
#define GSDI_PERSO_ENGINE_LO_LEN        sizeof(uint8)
#define GSDI_PERSO_ENGINE_TAG_CODE      0x00
#define GSDI_PERSO_ENGINE_TAG_DCK       0x01


#define GSDI_PERSO_ENGINE_TAG_FEATURE_INDS      0x01
#define GSDI_PERSO_ENGINE_TAG_PERM_FEATURE_INDS 0x02
#define GSDI_PERSO_ENGINE_TAG_SIM_LOCK_CODE     0x03
#define GSDI_PERSO_ENGINE_TAG_DEPERSO_CK        0x04
#define GSDI_PERSO_ENGINE_TAG_DCK_COUNTER       0x05
#define GSDI_PERSO_ENGINE_TAG_UNBLOCK           0x06
#define GSDI_PERSO_ENGINE_TAG_UNBLOCK_COUNTER   0x07
#define GSDI_PERSO_ENGINE_NUMBER_OF_TAGS        0x07


/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_NUM_CKS
   The maximum number of Control Keys as specified in 11.11 and 31.102.
   When a control key is not used, it will contain the default 0xFFFFFFFF
   --------------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_ENGINE_SIM_DCK_NUM_CKS             0x04
#endif
#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_ENGINE_RUIM_DCK_NUM_CKS            0x05
#endif


#define GSDI_PERSO_INT_INIT_REQ 0
#define GSDI_PERSO_SELF_INIT_REQ 1
#define GSDI_PERSO_ENGINE_INIT_REQ 2

/* IMEI Masking Digits */
#define GSDI_PERSO_ENG_IMEI_HIGH_BYTE              0xF0
#define GSDI_PERSO_ENG_IMEI_LOW_BYTE               0x0F

/* IMSI Digit Offset */
#define GSDI_PERSO_ENG_IMSI_DIGIT_1_OFFSET         0x01
#define GSDI_PERSO_ENG_IMSI_DIGIT_2_OFFSET         0x02
#define GSDI_PERSO_ENG_IMSI_DIGIT_3_OFFSET         0x02
#define GSDI_PERSO_ENG_IMSI_DIGIT_4_OFFSET         0x03
#define GSDI_PERSO_ENG_IMSI_DIGIT_5_OFFSET         0x03
#define GSDI_PERSO_ENG_IMSI_DIGITS_4_5_OFFSET      0x03
#define GSDI_PERSO_ENG_IMSI_DIGIT_6_OFFSET         0x04
#define GSDI_PERSO_ENG_IMSI_HIGH_DIGIT_MASK        GSDI_PERSO_ENG_IMEI_HIGH_BYTE
#define GSDI_PERSO_ENG_IMSI_LOW_DIGIT_MASK         GSDI_PERSO_ENG_IMEI_LOW_BYTE

#define GSDI_PERSO_MAX_MASTER_PHONE_CODE_LEN       8

/* ----------------------------------------------------------------------------
   ENUMERATION:  GSDI_PERSO_STATE_ENUM_TYPE
   DESCRIPTION:  Will indicate the different personalization states.
                 GSDI_PERSO_INIT_STATE:       State prior to running any
                                              personalization Code.  Only taken
                                              out of the NOT INIT State into the
                                              not active state after the
                                              Init data procedures are completed
                 GSDI_PERSO_NOT_ACTIVE_STATE: No personalization inds
                                              are active.  All depersonalized
                 GSDI_PERSO_AUTOLOCK_STATE:   Run the autolocking procedures
                 GSDI_PERSO_ACTIVE_SATE:      At lease one personalization
                                              indicator is active.
  --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_ENG_NOT_ACTIVE_STATE = 0x00000000,
  GSDI_PERSO_ENG_AUTOLOCK_STATE,
  GSDI_PERSO_ENG_ACTIVE_STATE,
  GSDI_PERSO_ENG_NOT_INIT_STATE,
  GSDI_PERSO_ENG_UNLOCKED_STATE,
  GSDI_PERSO_ENG_DISABLED_STATE,
  GSDI_PERSO_ENG_MAX              = 0x7FFFFFFF
} gsdi_perso_engine_state_enum_state;


/* ----------------------------------------------------------------------------
   ENUMERATION:  GSDI_PERSO_RETURN_STATUS_ENUM_TYPE
   DESCRIPTION:  Will be used as the return type for the GSDI
  --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_ENGINE_SUCCESS               = 0x00000000,
  GSDI_PERSO_ENGINE_SUCCESS_WRITE_BACK,
  GSDI_PERSO_ENGINE_ERROR,
  GSDI_PERSO_ENGINE_ERROR_WRITE_BACK,
  GSDI_PERSO_ENGINE_ERROR_GET_NV_FAILURE,
  GSDI_PERSO_ENGINE_ERROR_PUT_NV_FAILURE,
  GSDI_PERSO_ENGINE_ERROR_GET_MEMMALLOC,
  GSDI_PERSO_ENGINE_ERROR_REQ_DCK,                     /* PERSO MISMATCH */
  GSDI_PERSO_ENGINE_ERROR_REQ_MASTER_CODE,             /* DCK BLOCKED    */
  GSDI_PERSO_ENGINE_INCORRECT_PARAMS,                  /* BAD Params     */
  GSDI_PERSO_ENGINE_STATUS_MAX            = 0x7FFFFFFF
}gsdi_perso_engine_status_enum_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_ENGINE_MEM_POINTER_TYPE
   DESCRIPTION:  Will provide information pertaining to the larget memory
                 allocations that occur in the Personalization Engine.
  --------------------------------------------------------------------------*/
typedef struct {
  uint8 * memory_start;
  uint8 * memory_curr;
  uint8 * memory_end;
}gsdi_perso_engine_mem_pointer_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    gsdi_perso_eng_calc_req_mem_enum_type

   DESCRIPTION:  Used to calcuate the total memory required for a particular
                 data type sent to the security Library.  It is up to the
                 personalization engine to determine how much memory is
                 required.
   --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_ENG_CALC_ENC_DATA    = 0x0000,
  GSDI_PERSO_ENG_CALC_SIM_DATA,
  GSDI_PERSO_ENG_CALC_CLIENT_DATA,
  GSDI_PERSO_ENG_CALC_RUIM_ENC_DATA,
  GSDI_PERSO_ENG_CALC_RUIM_DATA
} gsdi_perso_eng_calc_req_mem_enum_type;


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* ---------------------------------------------------------------------------
FUNCTION:  GSDI_PERSO_INITIALIZE_DATA

DESCRIPTION:
  Will initialize the MMGSDI Perisonalization engine with the data required
  to start the Personalization Intialization Procedures.

DEPENDENCIES:
  Function must be called only after PIN1 has been verified.

LIMITATIONS:
  None

RETURN TYPE:
  gsdi_perso_engine_enum_type

-----------------------------------------------------------------------------*/
gsdi_returns_T gsdi_perso_initialize_data(
  gsdi_slot_id_type slot
);

/*===========================================================================
FUNCTION GSDI_PERSNALIZATION_PROCS

DESCRIPTION
  This function is called after the Verification procedures have occurred on
  PIN1.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_personalization_procs(
  gsdi_slot_id_type                      slot,
  gsdi_perso_eng_init_state_enum_type  * perso_engine_init_state,
  gsdi_perso_engine_state_enum_state   * perso_state,
  gsdi_perso_self_perso_state_enum_type*  self_perso_state
);
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_READ_SIM

DESCRIPTION
  This is the generic wrapper function used to read data from the SIM.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_read_sim(
  gsdi_slot_id_type                      slot,
  uim_items_enum_type                    file_name,
  void *                                 sim_data_p
);



/*===========================================================================
FUNCTION GSDI_PERSO_GET_RAND_VALUE

DESCRIPTION
  This function is used to get a Unsigned 32 Bit Integer Value.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
uint32 gsdi_get_rand_value(
void
);
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_HANDLE_REQUEST

DESCRIPTION
  This function is the main handler for all the perso requests.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_handle_request(
  gsdi_slot_id_type            slot,
  const void *                 req_p,
  byte                         msg_id,
  uint8 *                      ret_data_p,
  int32*                       ret_data_len_p,
  gsdi_perso_event_enum_type * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_PERSNALIZATION_PROCS

DESCRIPTION
  This function is called after the Verification procedures have occurred on
  PIN1.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_main(
  gsdi_slot_id_type slot,
  const void *      cmd_p,
  byte              msg_id,
  uint8 *           ret_data_p,
  int32 *           ret_data_len_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_INITIALIZE_DATA

DESCRIPTION
  This function is called after the Verification procedures have occurred on
  PIN1.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_initialize_data(
  gsdi_slot_id_type slot
);


/* ----------------------------------------------------------------------------
FUNCTION: GSDI_PERSO_ENGINE_GET_IMEI

DESCRIPTION:
  Will retrieve the IMEI number from NV.

DEPENDENCIES:
  None

LIMITATIONS:
  The memory allocated in this function will need to be released by the
  caller of this function.

RETURN TYPE:
  gsdi_returns_T

-----------------------------------------------------------------------------*/
gsdi_returns_T gsdi_perso_engine_get_imei (
  boolean parse,
  uint8 * imei_data_p,
  uint32 * imei_len_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_NOTIFY_CLIENTS_OF_PERSO_EVENT

DESCRIPTION
  This function is used to notify the clients of any Personalization
  Events.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_engine_notify_clients_of_perso_event(
  gsdi_perso_event_enum_type event,
  boolean                    additional_info_avail,
  uint32                     num_bytes,
  const uint8 *              data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_ENGINE_REGISTER_TASK

DESCRIPTION
  This function is used to process the Personalization Request from a client
  to register for Personalization Events.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T:  GSDI_INCORRECT_PARMS: Bad input Parameters
                   GSDI_ERROR          : Event Handler Table Full
                   GSDI_SUCCESS        : Successfully saved event handler
                                         - or -
                                         Duplicate Event Handler Detected
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_engine_reg_task (
  const gsdi_perso_reg_task_req_T * req_p
);

#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_SIM_SECURE_STORE

DESCRIPTION
  This function is called when file needed to be written to the secured EFS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_sim_secure_store(
  gsdi_sim_secure_store_req_T *secure_store_req,
  int32                       *ret_data_len_p);

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_SIM_SECURE_READ

DESCRIPTION
  This function is called when file needed to be read from the secured EFS

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_sim_secure_read(
  gsdi_sim_secure_read_req_T  *secure_read_req,
  int32                       *ret_data_len_p,
  uint8                       *ret_data_p);


/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_ROOT_KEY_WRITE

DESCRIPTION
  This function is called when root key is to be written to the OTP

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_root_key_write(
  gsdi_perso_root_key_write_req_T *root_key_write_req,
  uint8                           *data_p,
  int32                           *ret_data_len_p);

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_ROOT_KEY_WRITE

DESCRIPTION
  This function is called when root key is to be written to the OTP

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_root_key_read(
  gsdi_perso_root_key_read_req_T  *root_key_read_req,
  uint8                           *data_p,
  int32                           *ret_data_len_p);

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_GET_LIB_VERSION

DESCRIPTION
  This function is called when user tries to get the library version

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_get_lib_version(
  gsdi_perso_get_lib_version_req_T  *lib_version_req,
  uint8                             *data_p,
  int32                             *ret_data_len_p);

/* ============================================================================
FUNCTION:  GSDI_PERSO_ENGINE_OTP_IS_ACTIVATED

DESCRIPTION:
  Function is scalled to determine whether or not the OTP has been activated.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN_TYPE:
  boolean:         TRUE:   OTP is activated
                   FALSE:  OTP is not activated

=============================================================================*/
boolean gsdi_perso_engine_otp_is_activated(
  void
);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION GSDI_PERSO_INIT

DESCRIPTION
  This function is used to create all of the relevant Personalization Files
  used for storing information in EFS.

  This procedure is ran every time the mobile is powered up, but will not
  recreate any files already created.

  If all personalization files are not created properly or upon entering
  this function again, only some of the files exist...then an Error Fatal
  will occur.  This could be evidence of tampering.

  It will be acceptable for the file to be empty.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_init(
  int32       * returned_data_len_p,
  const uint8 * data_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_ENGINE_GET_ALL_LOCKS

DESCRIPTION
  This function will get all of the Personalization Locks and determine
  whether or not a Personalization Check is required for each lock.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_engine_get_all_locks(
  int32                      * ret_data_len_p,
  uint8                      * ret_data_p,
  gsdi_perso_event_enum_type * perso_event_p
);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
#endif /* GSDI_PERSO_ENGINE_H */

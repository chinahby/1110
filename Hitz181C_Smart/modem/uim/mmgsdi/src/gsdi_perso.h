#ifndef GSDI_PERSO_H
#define GSDI_PERSO_H
/*===========================================================================


                 G S D I   P E R S O N A L I Z A T I O N

     S T R U C T U R E S  A N D  F U N C T I O N  P R O T O T Y P E S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2004 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso.h#1 $$ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/02/04   jar     Added #defines for EF-DCK Offsets
07/17/04   jar     Added support for Checking subsets and for refresh
06/19/04 jar/tml   Added remaing support for self personalization
                   Added remaining Perso Support
06/09/04   jar     Added #defines to support populating files with information
05/22/04   jar     Initial Revision


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#if defined (FEATURE_MMGSDI_PERSONALIZATION) && \
   !defined (FEATURE_MMGSDI_PERSONALIZATION_ENGINE)
#include "gsdi_exp.h"
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Max Number of Clients supported for Personalization Events */
#define GSDI_PERSO_MAX_REG_CLIENTS                        0x0A

/* Non Valid Index Values which indicate different statuses   */
#define GSDI_PERSO_MIN_TABLE_INDEX                        0x0000
#define GSDI_PERSO_EVENT_HANDLER_ALREADY_REGISTERD        0x00FE
#define GSDI_PERSO_NO_EVENT_HANDLER_TABLE_INDEX_AVAILABLE 0x00FF

/* Offsets used when taking data_p information and populating the cnf rsp */
#define GSDI_PERSO_NW_STATUS_IND_OFFSET                   0x00
#define GSDI_PERSO_NS_STATUS_IND_OFFSET                   0x01
#define GSDI_PERSO_SP_STATUS_IND_OFFSET                   0x02
#define GSDI_PERSO_CP_STATUS_IND_OFFSET                   0x03
#define GSDI_PERSO_SIM_STATUS_IND_OFFSET                  0x04
/* Max Number of Bytes allowed from EFS when reading Feature Indicators */
#define GSDI_PERSO_MAX_FEATURE_IND_LEN                    5

/* Indicator that the feature is permanently disabled */
#define GSDI_PERSO_FEATURE_NOT_PERM_DISABLED              0x00
#define GSDI_PERSO_FEATURE_PERM_DISABLED                  0x01

/* Indicaotr that the feature is activated */
#define GSDI_PERSO_FEATURE_NOT_ACTIVATED                  0x00
#define GSDI_PERSO_FEATURE_ACTIVATED                      0x01

/* Indicator that there are no more retries remaining */
#define GSDI_PERSO_NO_MORE_CK_RETRIES                     0x00

/* Max number of retries allowed */
#define GSDI_PERSO_MAX_NUMBER_OF_RETRIES                  0x0A
#define GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES          0x20

/* Default Power Up Indicators */
#define GSDI_PERSO_NW_INDICATOR                           0x00
#define GSDI_PERSO_NS_INDICATOR                           0x00
#define GSDI_PERSO_SP_INDICATOR                           0x00
#define GSDI_PERSO_CP_INDICATOR                           0x00
#define GSDI_PERSO_SIM_INDICATOR                          0x00

/* Default Perm Disabled Indicators */
/* Default Power Up Indicators */
#define GSDI_PERSO_PERM_DIS_NW_INDICATOR                  0x00
#define GSDI_PERSO_PERM_DIS_NS_INDICATOR                  0x00
#define GSDI_PERSO_PERM_DIS_SP_INDICATOR                  0x00
#define GSDI_PERSO_PERM_DIS_CP_INDICATOR                  0x00
#define GSDI_PERSO_PERM_DIS_SIM_INDICATOR                 0x00

/* Offset of file in the array of files */
#define GSDI_PERSO_NW_INDEX                               0x00
#define GSDI_PERSO_NS_INDEX                               0x01
#define GSDI_PERSO_SP_INDEX                               0x02
#define GSDI_PERSO_CP_INDEX                               0x03
#define GSDI_PERSO_SIM_INDEX                              0x04

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
#define GSDI_PERSO_OTA_DEPERSO_RSP_LEN         20

/* OTA Deperso Depersonalization CK Len */
#define GSDI_PERSO_OTA_DEPERSO_CK_LEN          0x08

/* IMEI Masking Digits */
#define GSDI_PERSO_IMEI_HIGH_BYTE              0xF0
#define GSDI_PERSO_IMEI_LOW_BYTE               0x0F

/* IMSI Digit Offset */
#define GSDI_PERSO_IMSI_DIGIT_1_OFFSET         0x01
#define GSDI_PERSO_IMSI_DIGIT_2_OFFSET         0x02
#define GSDI_PERSO_IMSI_DIGIT_3_OFFSET         0x02
#define GSDI_PERSO_IMSI_DIGIT_4_OFFSET         0x03
#define GSDI_PERSO_IMSI_DIGIT_5_OFFSET         0x03
#define GSDI_PERSO_IMSI_DIGITS_4_5_OFFSET      0x03
#define GSDI_PERSO_IMSI_DIGIT_6_OFFSET         0x04
#define GSDI_PERSO_IMSI_HIGH_DIGIT_MASK        GSDI_PERSO_IMEI_HIGH_BYTE
#define GSDI_PERSO_IMSI_LOW_DIGIT_MASK         GSDI_PERSO_IMEI_LOW_BYTE

/* --------------------------------------------------------------------------
   GSDI_PERSO_MAX_SMS_STATUS_LEN
   Max number of bytes allowed in the SMS Status returned for the OTA
   Deperso SMS-PP
   Character  Description
    1- 16 IMEI of ME
       17 Network personalisation status
       18 Network subset personalisation status
       19 SP personalisation status
       20 Corporate personalisation status
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_SMS_STATUS_LEN      0x0014

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_NUM_CKS
   The maximum number of Control Keys as specified in 11.11 and 31.102.
   When a control key is not used, it will contain the default 0xFFFFFFFF
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_DCK_NUM_CKS             0x04

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_CK_LEN
   The maximum length for a Control Key in the DCK EF
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_DCK_CK_LEN              0x04

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_NW_CK_OFFSET
   GSDI_PERSO_DCK_NS_CK_OFFSET
   GSDI_PERSO_DCK_SP_CK_OFFSET
   GSDI_PERSO_DCK_CP_CK_OFFSET
   File Offsets for EF-DCK to retrieve the Control Keys From
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_DCK_NW_CK_OFFSET        0x00
#define GSDI_PERSO_DCK_NS_CK_OFFSET        0x04
#define GSDI_PERSO_DCK_SP_CK_OFFSET        0x08
#define GSDI_PERSO_DCK_CP_CK_OFFSET        0x0C

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_DEFAULT_EMPTY_VALUE
   When a control key is not used, it will contain the default 0xFFFFFFFF
   --------------------------------------------------------------------------*/
#define GSDI_PERSO_DCK_DEFAULT_EMPTY_VALUE 0xFF

/* --------------------------------------------------------------------------
   GSDI_PERSO_INIT_STATE
   In the Personalization Initialization State, the UE is only allowed to
   kick off the Personalization Power Up Procedures.
   ------------------------------------------------------------------------*/
typedef enum
{ GSDI_PERSO_NOT_INIT_STATE         = 0x00,
  GSDI_PERSO_INIT_STATE             = 0x01,
  GSDI_PERSO_NW_INIT_STATE          = 0x02,
  GSDI_PERSO_NW_WAIT_DEACT_STATE    = 0x03,
  GSDI_PERSO_NS_INIT_STATE          = 0x04,
  GSDI_PERSO_NS_WAIT_DEACT_STATE    = 0x05,
  GSDI_PERSO_SP_INIT_STATE          = 0x06,
  GSDI_PERSO_SP_WAIT_DEACT_STATE    = 0x07,
  GSDI_PERSO_CP_INIT_STATE          = 0x08,
  GSDI_PERSO_CP_WAIT_DEACT_STATE    = 0x09,
  GSDI_PERSO_SIM_INIT_STATE         = 0x0A,
  GSDI_PERSO_SIM_WAIT_DEACT_STATE   = 0x0B,
  GSDI_PERSO_COMPLETE_STATE         = 0x0C
}gsdi_perso_state_enum_type;

/* Enumaration to indicate whether pointer is a File or Directory */
typedef enum {
  GSDI_PERSO_FILENAME  = 0x00,
  GSDI_PERSO_DIRECTORY = 0x01
}gsdi_perso_file_enum_type;

/* Enumeration of File Names Maintained for the personalization File */
typedef enum {
  GSDI_PERSO_FILENAME_NONE      = 0x00,
  GSDI_PERSO_FEATURE_INDICATORS = 0x01
}gsdi_perso_filenames_enum_type;

typedef enum {
  GSDI_PERSO_CURRENTLY_NOT_PERSO  = 0,
  GSDI_PERSO_PERMANENTLY_NOT_PERS = 1,
  GSDI_PERSO_PERSONALIZED         = 2,
  GSDI_PERSO_IMEI_MISMATCH        = 3
}gsdi_perso_ota_deperso_status_enum_type;

/*===========================================================================

                        GLOBAL VARIABLES

===========================================================================*/
/* ----------------------------------------------------------------------------
   Table to hold the list of event handlers
   --------------------------------------------------------------------------*/
extern gsdi_perso_events_cb gsdi_perso_event_handler_table[GSDI_PERSO_MAX_REG_CLIENTS];

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_POST_PIN1

DESCRIPTION
  This function will perform the procedures required for the personalization
  procedure completed after PIN1 has been verified.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_post_pin1(
  int32 * returned_data_len_p,
  uint8 * data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_INTERNAL_INIT_PROCEDURES

DESCRIPTION
  This function will be called for each personalization procedure that occurs.
  It will be the main personalization handling function used durint the init
  procedures.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_internal_init_procedures(
  gsdi_perso_int_init_procedure_req_T * req_p,
  int32 *                               returned_data_len_p,
  uint8 *                               data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_UNBLOCK_FEATURE_IND

DESCRIPTION
  This function will be called by the GSDI Task to Handle the Unblock Feature
  Indicator Command.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_unblock_feature_ind(
  gsdi_perso_unblock_feature_ind_req_T * req_p,
  int32 *                                returned_data_len_p,
  uint8 *                                data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_PERM_DISABLE_FEATURE_IND

DESCRIPTION
  This function will be called by the GSDI Task to Handle the Permanently
  Disable Feature Indicator

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_perm_disable_feature_ind(
  gsdi_perso_perm_disabled_feature_req_T * req_p,
  int32 *                                  returned_data_len_p,
  uint8 *                                  data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_PERM_DISABLE_FEATURE_IND

DESCRIPTION
  This function will be called by the GSDI Task to Handle the Get Feature
  Keys (Control Keys).

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_get_feature_key(
  gsdi_perso_get_feature_key_req_T * req_p,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
);


/*===========================================================================
FUNCTION GSDI_PERSO_INT_INIT_HANDLER

DESCRIPTION
  This function is the callback function used to handle the response for the
  personalization procedures which in turn kicks off the next personalization
  init procedure.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void gsdi_perso_int_init_handler_cb(
  gsdi_cnf_T * gsdi_cnf_p
);

/*===========================================================================
FUNCTION GSDI_PERSO_INT_INIT_HANDLER

DESCRIPTION
  This function is the callback function used to handle the response for the
  personalization procedures which in turn kicks off the next personalization
  init procedure.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_post_pin1(
  gsdi_slot_id_type slot
);


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
gsdi_returns_T gsdi_perso_init(
  int32 * returned_data_len_p,
  uint8 * data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_REGISTER_TASK

DESCRIPTION
  This function is used to process the Personalization Request from a client
  to register for Personalization Events.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_reg_task (
  gsdi_perso_reg_task_req_T * req_p
);

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_GET_FEATURE_IND

DESCRIPTION
  This function is used to retrieve the Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
extern gsdi_returns_T gsdi_proc_perso_get_feature_ind(
  gsdi_perso_get_feature_ind_req_T * req_p,
  int32 *                           returned_data_len_p,
  uint8 *                           data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_SET_FEATURE_DATA

DESCRIPTION
  This function is used to retrieve the Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
extern gsdi_returns_T gsdi_proc_perso_set_feature_data(
  gsdi_perso_set_feature_data_req_T * req_p,
  int32 *                           returned_data_len_p,
  uint8 *                           data_p
);


/*===========================================================================
FUNCTION GSDI_PROC_PERSO_ACTIVATE_FEATURE_IND

DESCRIPTION
  This function is used to execute the Activation of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Check to see if the Feature is already Activated
  3.  See if the the Feature Contains any Data so that it can be activated

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_activate_feature_ind(
  gsdi_perso_act_feature_ind_req_T * req_p,
  int32 *                            returned_data_len_p,
  uint8 *                            data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_DEACTIVATE_FEATURE_IND

DESCRIPTION
  This function is used to execute the Deactivation of a Personalization
  Feature Indicator.

  This function will perform the following:
  1.  Check to see if the Feature is already Permanently Deactivated
  2.  Check to see if the Feature is already Deactivated

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_deactivate_feature_ind(
  gsdi_perso_deact_feature_ind_req_T * req_p,
  int32 *                              returned_data_len_p,
  uint8 *                              data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_OTA_DEPERSO

DESCRIPTION
  This function is used to perform the OTA Depersonalization Procedures as
  required to do during an SMS PP OTA Depersonalization Message

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_ota_deperso(
  gsdi_perso_ota_deperso_req_T * req_p,
  int32 *                        returned_data_len_p,
  uint8 *                        data_p
);
/* ==========================================================================

     G S D I    P E R S O N A L I Z A T I O N    E F S

            P R O T O T Y P E    F U N C T I O N S

===========================================================================*/
/*===========================================================================
FUNCTION GSDI_PROC_PERSO_REFRESH_PROCEDURES

DESCRIPTION
  This function is used to perform the refresh procedures as indicated
  in the SIM Personalization specifications:  02.22 or 22.022.
  1.  Determine if feature is active
  2.  Read EF-DCK
  3.  For each record with a valid Key, depersonalize the feature
  4.  Write back 0xFF to the Record EF-DCK just read

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_refresh_procedures(
  gsdi_slot_id_type slot,
  int32 *           returned_data_len_p,
  uint8 *           data_p
);

/*===========================================================================
FUNCTION GSDI_PROC_PERSO_GET_PERM_FEATURE_IND

DESCRIPTION
  This function is used to retrieve the Feature Indicators from Persistent
  memory which is then reported back to the client.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_proc_perso_get_perm_feature_ind(
  gsdi_perso_get_perm_feature_ind_req_T * req_p,
  int32 *                                 returned_data_len_p,
  uint8 *                                 data_p
);


/* ==========================================================================

     G S D I    P E R S O N A L I Z A T I O N    E F S

            P R O T O T Y P E    F U N C T I O N S

===========================================================================*/
/*===========================================================================
FUNCTION GSDI_PERSO_FS_GET_FEATURE_INDICATORS

DESCRIPTION
  This function is a wrapper to the FS Interface used to retrieve the
  Permanently disabled Feature Indicators.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T gsdi_perso_fs_get_feature_indicators(
  char  *                            file_name,
  int32 *                            retunred_data_len_p,
  uint8 *                            data_p
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
void gsdi_perso_notify_clients_of_perso_event(
  gsdi_perso_event_enum_type event,
  boolean                    additional_info_avail,
  int32                      num_bytes,
  uint8 *                    data_p
);

#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#endif /* GSDI_PERSO_H */

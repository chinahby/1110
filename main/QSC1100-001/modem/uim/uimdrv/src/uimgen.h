#ifndef UIMGEN_H
#define UIMGEN_H
/*===========================================================================

        E X T E R N A L   U I M  G E N E R I C   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM generic state machine.

Copyright (c) 2001,2002,2008,2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimgen.h_v   1.14   04 Oct 2002 16:46:34   tsummers  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimgen.h#2 $ $DateTime: 2009/03/25 00:48:02 $ $Author: suberoy $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/09   sun     Added support for Virtuim
09/12/08   nb      Added support to handle warnings returned by Envelope Command
08/08/08   vs      Removed featurization
08/05/08   vs      Renamed start and stop clock functions appropriately
07/17/08   vs      Added support for UIM USB UICC ICCD interface
06/03/08   rm      TA1=0x97 support
06/12/06   pv      Get rid of FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN and
                   add states for UIM_RECOVERY_COMPLETE_F
04/03/06   pv      Lint Fixes
03/09/06   pv      Added new state to clear the response cache
11/01/05   tml     Combine last sel EF with last sel Dir
10/19/05   pv      Added definition for UIM_DELAY_AFTER_ATR_ST for introducing
                   delay between the reception of the ATR and PPS request.
06/16/05   sk      Fixed UIM_CSIM_APDU_MIN_SIZE value
06/13/05   pv      Added support for new UIM commands with malloc
05/10/05   pv      BT sim access changes - Added POWER_DOWN_ST
02/03/05   jk      Changed FEATURE_UIM_RUN_TIME_ENABLE to FEATURE_UIM_RUIM 
                   around CHECK_FOR_CDMA_DF to allow CDMA to function okay
                   in 1x builds without RUN_TIME_ENABLE
01/12/05   pv      Externalized variables required for timeout logging and for
                   Driver time stamping mechanism
10/11/04   pv      Added code to parse the ATR for voltage class information
09/01/04   ck      Added support for CSIM.
05/17/04   ck      Added definitions to support for ISO 7816 streaming API.
10/17/03   ck      Added APDU_STREAM_ST to support streaming APDU.
09/19/03   ck      Changed the flag uim_atr_recd to uim_atr_pps_done.
08/14/03   ck      Added support for dual slots.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/22/03   jk      Added UTIL and WIM support
03/04/03   ck      Removed the constants used for processing a select command
02/13/03   wli     Added support to GSTK.
11/21/02   ts      Added a new state to check for the CDMA DF for RTRE.
11/21/02   ts      Added constants for default PPS.
10/29/02   ck      Externalized uim_atr_recd flag.
10/02/02   ts      Include definitions for checking TC1 (extra guardtime).
09/05/02   wli     Put parenthesis around definition of the UIM_CLOCK_STOP_ALL_BITS.
07/18/02   wli     Increased UIM_MAX_NUMBER_GEN_STATES to 4
07/09/02   ck      Modified the prototype for the function uim_stop_uim_clock()
                   to return a boolean type.
04/09/02   ck      Modified the featuization around clock stop functions to
                   include FEATURE_UIM_USIM.
03/27/02   ck      Modified the definition for 3V Tech bit for GSM and added a
                   similar one for USIM.
02/14/02   ts      Removed definitions for DFs as they are already defined in
                   uimi.h.  Explicitly include uimi.h.
01/22/02   ts      Added support for processing file characteristics.  This is
                   used to determine clock stop preferences.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
07/27/01   ts      Added constants to support processing the ATR and PPS for
                   changing operational parameters.
07/26/01   wli     Moved some definitions to uim.h
05/28/01   ck      Added support for multi mode server.
01/17/01   ts      Created module.  This module contains the generic state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"

#ifdef FEATURE_UIM
#include "comdef.h"
#include "uimi.h"
#include "uim.h"

#ifdef FEATURE_VIRTUAL_UIM_HW
#error code not present
#endif /* FEATURE_VIRTUAL_UIM_HW */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define UIM_CHV_PAD_CHAR         0xFF
#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_MAX_PPS_CHARS        6
#endif /* !FEATURE_VIRTUAL_UIM_HW */
#define UIM_PPS_DEFAULT_PPSS     0xFF
#define UIM_PPS_DEFAULT_PPS0     0
#define UIM_PPS_DEFAULT_PCK      0xFF
#define UIM_PPS_DEFAULT_REQ_SIZE 3
#define UIM_PPS_PPS1_PPS0        0x10
#define UIM_TA1_INDEX            2
#define UIM_PPS_REQ_SIZE         4
#define UIM_FI_SHIFT_OF_TA1      4
#define UIM_DI_MASK_OF_TA1       0x0F

/* Definitions to check if we need to send a PPS based on the ATR */
#define UIM_ATR_T0_BYTE          1
#define UIM_ATR_TA_PRESENT       0x10
#define UIM_ATR_TB_PRESENT       0x20
#define UIM_ATR_TC_PRESENT       0x40
#define UIM_ATR_TD_PRESENT       0x80
#define UIM_ATR_TA1_BYTE         2
#define UIM_ATR_TA2_IND          0x10
#define UIM_NO_PPS_VALUE1        0x11
#define UIM_NO_PPS_VALUE2        0x01
#define UIM_TC1_IS_0             0x00
#define UIM_TC1_IS_255           0xFF

/* Definitions to check the bits in the 8th and 5th position are set.*/
#define UIM_BIT8_CHECK           0x80
#define UIM_BIT5_CHECK           0x10

#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_P2_NEXT_RECORD_MODE  2
#define UIM_P2_PREV_RECORD_MODE  3
#define UIM_P2_ABS_RECORD_MODE   4
#endif /* !FEATURE_VIRTUAL_UIM_HW */

#if defined FEATURE_UIM_SUPPORT_CSIM
#define UIM_MAX_NUMBER_GEN_STATES 3
#else
#define UIM_MAX_NUMBER_GEN_STATES 2
#endif /* FEATURE_UIM_USIM_INERNAL_VERIFY_PIN */

#define UIM_7816_APDU_CLASS_OFFSET  0
#define UIM_7816_APDU_INSTRN_OFFSET 1
#define UIM_7816_APDU_P1_OFFSET     2
#define UIM_7816_APDU_P2_OFFSET     3
#define UIM_7816_APDU_MIN_SIZE      4
#define UIM_CASE1_7816_APDU_SIZE    4
#define UIM_CASE2_7816_APDU_SIZE    5
#define UIM_CASE2_7816_LE_OFFSET    4
#define UIM_CASE3_4_7816_LC_OFFSET  4
#define UIM_CSIM_APDU_MIN_SIZE      4

/* This is the index of the file characteristics byte in the response
   buffer of selecting the GSM directory. */
#define UIM_FILE_CHAR_INDEX      13
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_VOLTAGE_CLASS_BITS   0x70
#define UIM_5V_SIM_GSM           0x00
#define UIM_3V_SIM_GSM           0x10
#define UIM_1_8_SIM_GSM          0x20
#endif /* !FEATURE_VIRTUAL_UIM_HW */

/* Defines the supported voltages for SIM cards */
#define UIM_VOLTAGES_SUPPORTED_GSM (UIM_3V_SIM_GSM | UIM_1_8_SIM_GSM)
#define UIM_5V_UIM_UICC          0x10
#define UIM_3V_UIM_UICC          0x20
#define UIM_1_8_UIM_UICC         0x40
/* Defines the supported voltages for UICC cards */
#define UIM_VOLTAGES_SUPPORTED_UICC (UIM_3V_UIM_UICC | UIM_1_8_UIM_UICC)
#if defined( FEATURE_UIM_UICC )
#define UIM_UI_MASK              0x3F
#define UIM_UI_5V_UIM_UICC       0x01
#define UIM_UI_3V_UIM_UICC       0x02
#define UIM_UI_1_8_UIM_UICC      0x04
#endif /* FEATURE_UIM_UICC */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
#define UIM_3V_TECH_BIT_GSM      0x10
#define UIM_3V_TECH_BIT_USIM     0x20
#define UIM_CLOCK_STOP_B1        0x01
#define UIM_CLOCK_STOP_B3        0x04
#define UIM_CLOCK_STOP_B4        0x08
#define UIM_CLOCK_STOP_ALL_BITS  (UIM_CLOCK_STOP_B1 | \
                                 UIM_CLOCK_STOP_B3 | \
                                 UIM_CLOCK_STOP_B4)

/* Define how the UIM clock is to be controlled */
typedef enum {
  UIM_CLOCK_STOP_NOT_ALLOWED, /* Stopping the UIM clock is not allowed */
  UIM_CLOCK_STOP_HIGH,        /* The UIM clock can be stopped high */
  UIM_CLOCK_STOP_LOW          /* The UIM clock can be stopped low */
} uim_clock_stop_cntrl_type;

/* This is the ICCID EF file ID */
#define UIM_ICCID_EF_ID          0x2FE2
#define UIM_ICCID_OFFSET         0
#define UIM_ICCID_SIZE           10

#ifdef FEATURE_UIM_UICC

#define UIM_ADF_LEVEL            1

#define UIM_CARD_SERVICE_DATA    0x31
#define UIM_SEL_BY_FULL_DF_NAME_MASK     0x80
#define UIM_SEL_BY_PARTIAL_DF_NAME_MASK  0x60
#define UIM_DIR_OBJECT_PRESENT_MASK      0x20

#ifndef FEATURE_VIRTUAL_UIM_HW
#define UIM_FCP_TEMPLATE_TAG     0x62
#define UIM_FILE_DESCRIPTOR_TAG  0x82
#define UIM_FILE_ID_TAG          0x83
#define UIM_DF_NAME_TAG          0x84
#define UIM_PROPRIETARY_TAG      0xA5
#define UIM_SECURITY_ATTR_TAG1   0x86
#define UIM_SECURITY_ATTR_TAG2   0x8B
#define UIM_SECURITY_ATTR_TAG3   0x8C
#define UIM_SECURITY_ATTR_TAG4   0xAB
#define UIM_LIFE_CYCLE_STATUS_TAG 0x8A
#define UIM_FILE_SIZE_TAG        0x80
#define UIM_TOTAL_FILE_SIZE_TAG  0x81
#define UIM_SFI_TAG              0x88

#define UIM_UICC_TRANSPARENT_EF 0x01
#define UIM_UICC_LINEAR_EF      0x02
#define UIM_UICC_CYCLIC_EF      0x06
#define UIM_UICC_EF_STRUCT_MASK 0x07
#endif /* !FEATURE_VIRTUAL_UIM_HW */

#define UIM_SFI_NOT_USED_BR_MASK 0x80
#define UIM_SFI_NOT_USED_RR_MASK 0xF8

#define UIM_PIN1_APP1_VALUE     0x01
#define UIM_PIN1_APP2_VALUE     0x02
#define UIM_PIN1_APP3_VALUE     0x03
#define UIM_PIN1_APP4_VALUE     0x04
#define UIM_PIN1_APP5_VALUE     0x05
#define UIM_PIN1_APP6_VALUE     0x06
#define UIM_PIN1_APP7_VALUE     0x07
#define UIM_PIN1_APP8_VALUE     0x08

#define UIM_PIN2_APP1_VALUE     0x81
#define UIM_PIN2_APP2_VALUE     0x82
#define UIM_PIN2_APP3_VALUE     0x83
#define UIM_PIN2_APP4_VALUE     0x84
#define UIM_PIN2_APP5_VALUE     0x85
#define UIM_PIN2_APP6_VALUE     0x86
#define UIM_PIN2_APP7_VALUE     0x87
#define UIM_PIN2_APP8_VALUE     0x88
#define UIM_UNIVERSAL_PIN_VALUE 0x11

#endif /* FEATURE_UIM_UICC */

#define UIM_SW1_SECURITY         0x98

typedef enum {
  UIM_POWER_UP_ST,                     /* Power Up state */
  UIM_RESET_ST,                        /* Reset State */
  UIM_DELAY_AFTER_ATR_ST,              /* Delay after receiving the ATR */
  UIM_PPS_ST,                          /* PPS State */
  UIM_UPDATE_OP_PARAMS_ST,             /* Update Operational Parameters State */
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,                      /* Send IFS negotiation for IFSD */
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_CHECK_CHARACTERISTICS_ST,        /* Check clock stop preferences */
#if defined( FEATURE_UIM_RUIM )
  UIM_CHECK_FOR_CDMA_DF,               /* Select CDMA DF to determine if R-UIM
                                          card */
#endif /* FEATURE_UIM_RUIM */
  UIM_READ_ICCID_ST,                   /* Select ICCID EF */
  UIM_SELECT_ST,                       /* Select State */
  UIM_STATUS_ST,                       /* Status State */
  UIM_READ_BINARY_ST,                  /* Read Binary State */
  UIM_READ_BIN_MEM_ST,                 /* Read binary with uim allocated memory */
  UIM_UPDATE_BINARY_ST,                /* Update Binary State */
  UIM_UPDATE_BIN_MEM_ST,               /* Update binary not writing to cmd_ptr */
  UIM_READ_RECORD_ST,                  /* Read Record State */
  UIM_READ_REC_MEM_ST,                 /* Read record with uim allocated memory */
  UIM_UPDATE_RECORD_ST,                /* Update Record State */
  UIM_UPDATE_REC_MEM_ST,               /* Update binary not writing to cmd_ptr */
  UIM_INC_MEM_ST,                      /* INC with uim allocated memory */
  UIM_INCREMENT_ST,                    /* Increment State */
  UIM_SEEK_ST,                         /* Seek State */
  UIM_VERIFY_CHV1_ST,                  /* Verify CHV 1 State */
  UIM_VERIFY_CHV2_ST,                  /* Verify CHV 2 State */
  UIM_UNBLOCK_CHV1_ST,                 /* Unblock CHV 1 State */
  UIM_UNBLOCK_CHV2_ST,                 /* Unblock CHV 2 State */
  UIM_CHANGE_CHV1_ST,                  /* Change CHV 1 State */
  UIM_CHANGE_CHV2_ST,                  /* Change CHV 2 State */
  UIM_ENABLE_CHV1_ST,                  /* Enable CHV 1 State */
  UIM_DISABLE_CHV1_ST,                 /* Disable CHV 1 State */
  UIM_INVALIDATE_ST,                   /* Invalidate State */
  UIM_REHABILITATE_ST,                 /* Rehab state */
#ifdef FEATURE_UIM_TOOLKIT
  UIM_TERMINAL_PROFILE_ST,             /* Term profile State */
  UIM_ENVELOPE_ST,                     /* Envelope State */
  UIM_FETCH_ST,                        /* Fetch State */
  UIM_TERMINAL_RESPONSE_ST,            /* Term response State */
  UIM_POLLING_INTERVAL_ST,             /* Polling state */
#endif /* FEATURE_UIM_TOOLKIT */
  UIM_STREAM_APDU_ST,                  /* Stream APDU state */
  UIM_POWER_DOWN_ST,                   /* Power Down the INTERFACE */
  UIM_CLEAR_CACHE_ST,                  /* Clear the UIM cache */
  UIM_RECOVERY_COMPLETE_ST,            /* Recovery completed by GSDI */
  UIM_USB_POWER_OFF_ST,                /* Send ICCD POWER OFF to USB */
  UIM_USB_POWER_ON_ST,                 /* Send ICCD POWER ON to USB */
  UIM_USB_GET_ATR_ST,                  /* Get ATR data block from USB */
  UIM_DONE_ST                          /* Done State */
} uim_generic_state_type;


typedef struct {
  boolean         use_chv;              /* indicate if CHV should be used */
  byte            chv[UIM_MAX_CHV_DIGITS];      /* CHV digits */
} uim_chv_struct_type;

/* Contains the ADF used for internal selection */
extern uim_aid_type uim_adf_to_be_sel;

/* Boolean to indicate if ATR has been received */
extern boolean uim_atr_received;
/* Boolean to indicate if ATR has been received */
extern boolean uim_atr_pps_done;

/* Extern operational parameters for timeout logging */
extern uim_op_params_type uim_op_params_buf;
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_FORMAT_CHV

DESCRIPTION
  This procedure formats the chv recd from NV in the commands NEW_CHV and
  UNBLOCK_CHV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the value of the CHV passed in as a parameter.

===========================================================================*/

extern void uim_format_chv
(
  byte *chv   /* This points to the CHV buffer that holds the CHV digits.
                 The digits in this buffer are formatted  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_START_POWER_SAVE

DESCRIPTION
  For USB cards this function informs the USB driver that the USB interface is
  not currently needed for any ICCD transaction and hence it can be suspended.
  For legacy ISO cards this function processes a request to turn the UIM clock
  off. It uses a global control variable to determine how to control the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine
  how to control the UIM clock.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  For USB cards if all functional interfaces over the USB interface have voted
  for a suspend, the USB interface to the card will be suspended.
  For legacy ISO cards the UIM clock will either be left high or low or will be
   left on, depending on the preferences of the UIM card.

===========================================================================*/
extern boolean uim_start_power_save
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_STOP_POWER_SAVE

DESCRIPTION
  For USB cards this function sends a request to the USB driver to resume
  the USB interface as we need to communicate with the card over the ICCD
  functional interface. 
  For legacy ISO cards this function processes a request to turn the UIM clock
  back on. It uses a global control variable to determine how to control the
  clock. It also uses a global variable to determine what frequency to set
  the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine how
  to control the UIM clock and another global variable to determine the
  clock frequency.

RETURN VALUE
  None

SIDE EFFECTS
  For USB cards the USB driver will be requested to resume the interface.
  For ISO cards the UIM clock will be turned on and set to a frequency
  previously determined.

===========================================================================*/
extern void uim_stop_power_save
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
extern void uim_send_generic_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_COMMAND

DESCRIPTION
  This procedure processes a generic command sent to the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM Generic state variable amongst others.

===========================================================================*/

extern boolean uim_process_generic_command
(
  uim_cmd_type *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_generic_state.

===========================================================================*/

uim_cmd_status_type uim_generic_command_response
(
 uim_rsp_buf_type *rsp_ptr,
 uim_cmd_type     *cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the main state machine for Generic UIM command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

extern void uim_generic_command
(
  uim_cmd_type *cmd_ptr
);


/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_CMD_STATE

DESCRIPTION
  This procedure sets the UIM state and is called during custom command
  processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the state of ruim_state variable in auth task.

===========================================================================*/

extern void uim_set_cmd_state
(
  uim_generic_state_type state
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_SEL_ST_CTR

DESCRIPTION
  This procedure sets the RUIM Select state Counter and is called during
  custom command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the value of select_state_ctr variable in auth task.

===========================================================================*/

extern void uim_set_sel_st_ctr
(
  byte sel_st_ctr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_CMD_PROCESSOR

DESCRIPTION
  This procedure processes the custom commands( implemented by the customers)
  sent to the Authentication task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void uim_custom_generic_cmd_processor
(
  uim_cmd_type *cmd_ptr,
    /* Pointer to received command */
  uim_req_buf_type const *req_ptr,
    /* Pointer to request buffer */
  uim_rsp_buf_type const *rsp_ptr,
    /* Pointer to response buffer */
  uim_resp_callback_type callback_fn
  /* Callback function pointer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the state machine for the custom RUIM commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void uim_custom_generic_command
(
  uim_cmd_type const *cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern uim_cmd_status_type uim_custom_generic_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     const *cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM
  at the completion of the command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void uim_custom_process_generic_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type     const *cmd_ptr
);

/*========================================================================

FUNCTION UIM_CUSTOM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for the command that is being requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_custom_send_generic_err_rpt
(
  uim_cmd_type const *cmd_ptr                    /* the command to queue up */
);


/*========================================================================

FUNCTION UIM_PROCESSING_ENVELOPE_APDU

DESCRIPTION
  This function returns whether UIM is processing an envelope command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM processing EVVELOPE COMMAND
  FALSE: UIM not processing ENVELOPE COMMAND

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_envelope_apdu
(
  void
);


#endif /* FEATURE_UIM */
#endif /* UIMGEN_H */


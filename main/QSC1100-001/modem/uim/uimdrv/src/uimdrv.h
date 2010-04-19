#ifndef UIMDRV_H
#define UIMDRV_H
/*===========================================================================

                 U I M   D E V I C E   L A Y E R   H E A D E R

DESCRIPTION
   This is the header file for the interface layer of the UIM device driver.
   Therefore, this header file provides the complete interface to the low level
   UIM device driver.

   Copyright (c) 2001 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/uimdrv.h_v   1.12   30 Jul 2002 11:07:44   gravanas  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimdrv.h#7 $ $DateTime: 2009/04/08 09:28:42 $ $Author: nbandwar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/09   nb      Fix for Multiple selects on MF
03/25/09   sun     Added support for Virtuim
03/19/09   rm      Move platform specific cust files inclusions to custuim.h
02/26/09   yk      Fixed Compilation Error
02/25/09   yk      Bring MDM8200 drivers
01/06/09   yk      Added MSM7X27 specific cust file inclusion
12/04/08   nb      Fixed QTF compilation problem
12/03/08   nb      Fixed Compilation Error
11/19/08   vs      Allowing uim_sleep_okts_handle to be accessed as a global
11/13/08   rm      Use timer callback for command response timer to handle the
                   work waiting timer expiry immediately in the timer context.
10/27/08   as/nk   Included 6270 and 8x50 hardware specific file
10/07/08   vs      Fix to power up UIM LDO after card power down, vote TCXO off
                   permanently when IC-USB interface is selected
10/03/08   jk      Fixed warning
09/22/08   rm      Fix for parity errors in UART_DM
09/19/08   rm      Work waiting time fix for UART_DM
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
08/22/08   js      Removed featurization around MFLO APDU status types
08/08/08   vs      Fixed compile errors related to featurization
08/01/08   rm      Added a method to return the size of RX data
                   waiting in RX FIFO and packing buffer in the UART DM.
07/09/08   rm      Fix compilation error
06/18/08   jk      Added QSC11X0 specific cust file inclusion
06/11/08   jar     Added platform specific custuim inclusions
                   for 7225.
05/28/08   rm      TA1=0x97 feature support.
05/09/08   rm      Fix to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
05/02/08   rm      T=1 protocol support for LCU.
04/22/08   rm      Remove the un-necessary UIM_RX_HANDLE_CASE2_BYTES state
04/30/08   vs      Fixed Klocwork error
04/16/08   rm      Fix for 6246 to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
04/08/08   sun     Added support for OMA Bcast
10/03/08   rm      Fix for 6290 to not call PMIC MPP APIs in case of direct
                   connect to SIM or single chip solution
03/13/08   rm/jar  LCU UIM bringup in UART word mode with DM disabled
03/05/08   jar     Allow PMIC to put VREG RUIM into LPM MODE when TXCO
                   is off under feature:  FEATURE_UIM_ENABLE_VREG_UIM_LPM
10/09/07   vs/nk   Added EUIMID feature support
09/10/07   tml     Added MFLO support
10/08/07   nf      Added GPLT support
07/02/07   sun     Added support for ODD INS Class
04/17/07   sp      Featurize for LTK
09/28/06   pv      Change uim_response_byte_delay to unsigned.
07/31/06   pv      Add definition for GENERATE ASYMMETRIC KEY PAIR WIM
                   instruction to use it to avoid doine a get response
                   when 6200 is received.
06/12/06   pv      Add UIM_RX_UNKNOWN_PROC_BYTE_RCVD state
04/11/06   pv      Moved UIM_READ_STATUS macro to uimdrv.c and export
                   uim_read_status function to do the same operation.
04/03/06   pv      Lint Fixes
01/18/06   tml     added support for status word 6999
09/06/05   jk      Changes to support only allowing reading UART Status while
                   there is still power to UART
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
02/09/05   jk      Added parenthesis to UIM_READ_STATUS Macro
                   so that it evaluates properly in if condition setting.
01/12/05   jk      Added code to process Response properly for Global Platform/WIM
                   cards
01/12/05   pv      Externalized function definitions for Driver Timestamping
12/28/04   jk      Changed Streaming APDU size from 260 to 261.
12/16/04   jk      Fixed CR # 54476 - Streaming APDU size up to 260 bytes
12/08/04   pv      Raven merged to main-line
12/06/04   pv      Externalised variables required for logging timeout info
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CSIM.
08/10/04   pv      Brought in definitions for rx_state and tx_state
                   Changed the response byte delayt timeout to 150microsecs
07/18/04   pv      Brought in some definitions from driver source file
05/11/04   ts      Fixed the uim command buffer definition for accesses
                   greater than 256 bytes.
04/28/04   ts      Added a constant to support for > 256 byte writes to
                   transparent files.
01/12/04   ck      Added UIM_LOGICAL_CHANNEL_BIT_MASK
12/19/03   jk      Added interface for R-UIM 3GPD Services.
10/31/03   ts      Added constants to support accesses greater than the maximum
                   transaction size.
10/23/03   ck      Merged in changes to support 18etus between reception of a
10/10/03   ts      Removed featurization around #defs for status words.
                   Added definition for minimum guardtime for T=1.
09/02/03   ck      Removed the featurization around enum type for uim_slot_type.
08/21/03   ck      Merged in support for Saber and TCXO/5 for USIM clock.
08/14/03   ck      Merged in support for dual slots.
07/22/03   ts      Added a table that indicates the maximum BRAF for the CRCF.
06/03/03   ck      Added status type UIM_CONVENTION_ERROR.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
02/26/03   jk      Added UTIL and WIM support
01/20/03   ts      Added run-time support of 13MHz GSM clock.
12/20/02   ts      Fixed Lint issues
                   Added support for using 13MHz GSM clock.
11/26/02   ts      Determine tcxo frequency used and max UIM frequency
                   supported based on tcxo frequency.
11/21/02   ts      Added constants for a default PPS exchange.
11/13/02   djm     WCDMA PLT build w/o FEATURE_UIM does not build because
                   UIM_MAX_CHARS is referred to by other files when FEATURE_UIM
                   is not defined.  pull this section of defines in when
                   FEATURE_WCDMA_PLT is def'd
10/24/02   ts      Added reference to clock rate conversion factor table.
07/30/02   gr      Merged timetest fix that was featurized (lost in the 1.10.1.0)
05/21/02   ts      Added support for 256 bytes transfers from the UIM card.
03/18/02   ts      Moved the MSMU_OUT and MSMUW_OUT from this file to uimdrv.c
                   to avoid multi-definition problems with these macros.
01/23/02   ts      Added definitions for setting the UIM clock signal to a high
                   state and a low state through uim_update_op_params.
12/13/01   ts      Aligned some comments
12/04/01   ts      Added status word definitions.
11/13/01   ts      Added support for UIM Application Toolkit (UTK).
10/10/01   ts      Removed the SIO_NOP from the MSMU_OUT macro.
07/27/01   ts      Added test port debug macros and constants so that any part
                   of UIM server can use it.
06/26/01   ts      Changed the uim_clk_freq_type to indicate tcxo/4 and tcxo/8.
05/25/01   ck      Featurised the intruction codes for Pre-Addendum 1 codes
                   that clashed with ISO 7816 instrn codes.
05/24/01   ck      Added support for USIM.
01/15/01   ck      Created file from R-UIM driver version 1.3.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "qw.h"
#include "uim.h"

#ifdef FEATURE_UIM_DRIVER_TIME_TEST
#include "timetest.h"              /* Debug port used for timing tests     */
#endif

#if defined(T_MSM33) || defined(T_UIM_USE_BSP_DEFINES)
#include "bsp.h"                   /* Board Support Package                */
#endif

#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
#include "sleep.h"
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */

/* Make sure USIM uses UICC code */
#if defined( FEATURE_UIM_USIM )
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_VIRTUAL_UIM_HW
#error code not present
#endif/*FEATURE_VIRTUAL_UIM_HW*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Default delay based upon a 3.25MHz UIM clock and CRAF of 512 and BRAF of 8
   The baud rate is 50.781Kbps.  */
/* The default delay adds delay for 1.5 bits.  This is needed because the MSM
   interrupts the ARM half way within the first stop bit.  There are 1.5 bits
   left to wait before a response */
#define UIM_RESPONSE_BYTE_DELAY_DEFAULT 150
/* This delay works on cards that expect 6 bits delay after the procedure
   byte.  This is 7.5 bits after the procedure byte */

#define UIM_RESPONSE_BYTE_DELAY_MORE 150
/* This is the delay used before sending a byte to the UIM card directly after
   processing a byte from the card. */
extern dword uim_response_byte_delay;

#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
/* This variable holds the SLEEP Handle for voting */
extern sleep_okts_handle uim_sleep_okts_handle;
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */

#define UIM_RUIM_UIMID_MAX_SIZE    7

/* Define the slots that are supported for this build.  It could be one of
   two slots or both */
/* IMPORTANT: The enumeration and typedef for UIM slots should not be combined
   as this introduces Klocwork errors*/
enum {
  #if defined( FEATURE_UIM1 )
    UIM_DRV_SLOT1,
  #endif
  #if defined( FEATURE_UIM2 )
    UIM_DRV_SLOT2,
  #endif
  UIM_NUM_DRV_SLOTS
};
typedef uint8 uim_drv_slot_type;

#define UIM_DEFAULT_CHANNEL UIM_CHANNEL0

#define UIM_CHANNEL_MASK       3

extern uim_drv_slot_type uim_drv_slot;

#if defined(FEATURE_UIM_UART_DM)
/* This boolean enables the SW workaround for the GCF 27.11.1.5 failure in
   LCU */
extern boolean uim_par_err_workaround;
#endif /* FEATURE_UIM_UART_DM */

/* Boolean flag to indicate reselect MF */
extern boolean uim_mf_reselect;

/* Number of attempts to select MF */
extern uint32 uim_mf_reselect_count;

#if defined(FEATURE_UIM) || defined(FEATURE_WCDMA_PLT) || defined(FEATURE_GSM_PLT)
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
/* The enum type describes the voltage classes supported on the UIM interface.
   The list must start at the lowest voltage and progress top the highest voltage
   in order. */
#ifndef FEATURE_VIRTUAL_UIM_HW
typedef enum {
  UIM_VOLTAGE_1_8V,       /* Use 1.8 volts */
  UIM_VOLTAGE_3V          /* Use 3 volts */
} uim_voltage_class_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW */

/* Defines the minimum voltage supported */
#define UIM_MIN_VOLTAGE UIM_VOLTAGE_1_8V
/* Defines the maximum voltage supported */
#define UIM_MAX_VOLTAGE UIM_VOLTAGE_3V

extern uim_voltage_class_type uim_current_voltage_class[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

/*
  This is a conversion table that returns the number of bytes that follow the
  format byte.  Only the "presents of interface bytes" field is used in the
  index.  The table contains the number of bits set in the index.
 */
extern const byte uim_y_format_table[16];
/* This holds the UIM clock frequency. */
extern dword uim_clk_freq[UIM_NUM_DRV_SLOTS];
/* This type defines the allowable UIM clock control.  The controls include
   leaving the clock signal HIGH, leaving it LOW and oscillating it at defined
   frequencies.*/

typedef enum {
  UIM_CLOCK_LOW,          /* Leave the clock signal LOW                      */
  UIM_CLOCK_HIGH,         /* Leave the clock signal HIGH                     */
  UIM_CLOCK_TD8,          /* Program the clock at tcxo/8                     */
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
  UIM_CLOCK_TD5,          /* Program the clock at tcxo/5                     */
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
  UIM_CLOCK_TD4           /* Program the clock at tcxo/4                     */
} uim_clock_control_type;

/* This type gives the allowable clock frequencies.                          */
typedef enum {
  UIM_CLK_TD8,
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
  UIM_CLK_TD5,
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
  UIM_CLK_TD4,
  UIM_CLK_STOP_HIGH,
  UIM_CLK_STOP_LOW
} uim_clk_freq_type;

/* This holds the MAXIMUM value the UIM clock can be */
extern uim_clk_freq_type uim_max_freq_supported[UIM_NUM_DRV_SLOTS];


/* This type defines the transport protocol used by the UIM server */
typedef enum {
  UIM_T_0_PROTOCOL = 0x00,
  UIM_T_1_PROTOCOL = 0x01,
  UIM_NO_PROTOCOL  = 0x02
} uim_transport_protocol_type;

/* These define the UIM specific interface bytes */

#define UIM_K_FIELD          0x0F      /* In T0; Number of historical bytes */
#define UIM_Y_FIELD_SHIFT       4      /* Number of bits to shift to get the
                                           Y field in the lsb                */
#define UIM_TD_PRESENT       0x80      /* TD byte is present in following
                                           bytes                             */
#define UIM_T_FIELD          0x0F      /* In TD; transmission protocol      */

#define UIM_DEFLT_GUARDTIME_BITS 2     /* This is the default number of
                                           bits used for the guardtime.      */
#define UIM_MINIMUM_T_1_GUARDTIME 1    /* This is the minimum guardtime for
                                          the T=1 protocol */
#ifndef FEATURE_VIRTUAL_UIM_HW
typedef enum {
  UIM_DONE,                      /* Command completed without error.        */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  UIM_DATA_DOWNLOAD_ERROR,       /* An SMS-PP download error occurred       */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
  UIM_TOOLKIT_BUSY,              /* The R-UIM toolkit is unable to perform the
                                    operation because it is currently busy
                                    working on another command              */
  UIM_SUCCESS_AFTER_MULTI_TRIES, /* The command is successful; but after
                                    using the internal update retry routine.*/
  UIM_MEMORY_PROBLEM,            /* Memory problem.                         */
  UIM_NO_EF_SELECTED,            /* No EF was selected                      */
  UIM_OUT_OF_RANGE,              /* Indicates an invalid address            */
  UIM_NOT_FOUND,                 /* Indicates file ID or pattern is not
                                    found.                                  */
  UIM_INCONSISTENT_WITH_COMMAND, /* Indicates the file is inconsistent with
                                    the command.                            */
  UIM_NO_CHV_INITIALIZED,        /* No CHV was initialized to allow this
                                    command                                 */
  UIM_ACCESS_NOT_FULFILLED,      /* Indicates the access condition for the
                                    command has not been fulfilled.
                                    Unsuccessful CHV verification, at least
                                    one attempt left
                                    Unsuccessful UNBLOCK CHV verification,
                                    at least one attempt left authentication
                                    failed                                  */
  UIM_CONTRADICTION_W_CHV_STATUS,/* Command is in contradiction with the CHV
                                    status                                  */
  UIM_CONTRADICTION_W_INVALID_STATUS,/* Command is in contradiction with the
                                        invalidation status                 */
  UIM_SSD_UPDATE_SEQ_PROBLEM,    /* Problem in the sequence of the command
                                    with respect to base station challange  */
  UIM_CHV_BLOCKED,               /* Unsuccessful CHV verification, no attempt
                                    left
                                    Unsuccessful UNBLOCK CHV verification, no
                                    attempt left
                                    CHV blocked
                                    UNBLOCK CHV blocked                     */
  UIM_MAX_VALUE_REACHED,         /* Indicates the increase operation cannot be
                                    performed because the maximum value has
                                    been reached.                           */
  UIM_INCORRECT_PARAM_P3,        /* P3 is an incorrect parameter            */
  UIM_INCORRECT_PARAM_P1_2,      /* P1 or P2 is an incorrect parameter      */
  UIM_UNKNOWN_INS,               /* Unknown instruction code given in the
                                    command (APDU)                          */
  UIM_WRONG_CLASS,               /* Wrong instruction class given in the
                                    command                                 */
  UIM_PROBLEM,                   /* Technical problem with no diagnostic
                                    given                                   */
  UIM_CMD_ERR_NO_INFO,           /* No information given                    */

  /* Status byte coding - warnings */
  UIM_NO_INFO_AND_NV_UNCHANGED,  /* No information given, state of NV memory
                                    unchanged                               */
  UIM_PART_DATA_MAY_BE_CORRUPTED,/* Part of returned data may be corrupted  */
  UIM_EOF_BEFORE_READING_LE_BYTES, /* End of File/record reached before
                                      reading Le bytes                      */
  UIM_SELECTED_FILE_INVALIDATED, /* Selected file invalidated               */
  UIM_INCORRECT_FCI_FORMAT,      /* FCI not formatted according to sub clause
                                    11.1.1.3.1                              */
  UIM_NO_INFO_AND_NV_CHANGED,    /* No information given, state of NV memory
                                    changed                                 */
  UIM_FILE_FILLED_UP_BY_LAST_WRITE,/* File filled up by last write          */
  UIM_CMD_SUCCESS_WITH_X_RETRIES,  /* Command successful but after using an
                                    internal update retry routine X times.
                                    Verification failed, X retries remain   */
  UIM_MORE_DATA_AVAILABLE,         /* More Data Available */

 /* Execution Errors */
  UIM_EXEC_ERR_AND_NV_UNCHANGED, /* No information given, state of NV memory
                                    unchanged                               */
  UIM_EXEC_ERR_AND_NV_CHANGED,   /* No information given, state of NV memory
                                    changed                                 */
  UIM_EXEC_ERR_MEMORY_PROBLEM,   /* Memory Problem                          */

/* Functions in CLA not supported */
  UIM_CLA_ERR_NO_INFO,           /* No information given                    */
  UIM_LOGICAL_CHAN_NOT_SUP,      /* Logical Channel not supported           */
  UIM_SECURE_MESSAGING_NOT_SUP,  /* Secure Messaging not supported          */

/* Command not allowed */
  UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT,/* Command incompatible with file
                                           structure                        */
  UIM_SEC_STATUS_NOT_SATISFIED,  /* Security Status not satisfied           */
  UIM_AUTH_PIN_BLOCKED,          /* Authentication/PIN method blocked       */
  UIM_REF_DATA_INVALIDATED,      /* Referenced data invalidated             */
  UIM_USE_CONDNS_NOT_SATISFIED,  /* Conditions of used not satisified       */
  UIM_CMD_ERR_NO_EF_SELECTED,    /* Command not allowed                     */

/* Wrong parameters */
  UIM_INCORRECT_PARAM_IN_DATA_FIELD,/* Incorrect params in the data field   */
  UIM_FUNCTION_NOT_SUPPORTED,    /* Function not supported                  */
  UIM_FILE_NOT_FOUND,            /* File not found                          */
  UIM_RECORD_NOT_FOUND,          /* Record not found                        */
  UIM_LC_INCONSISITENT_WITH_TLV, /* Lc inconsistent with TLV structure      */
  UIM_INCORRECT_PARAMS_P1_P2,    /* Incorrect parameters P1-P2              */
  UIM_LC_INCONSISITENT_WITH_P1_P2,/* Lc inconsistent with P1-P2             */
  UIM_REF_DATA_NOT_FOUND,        /* Referenced data not found               */
  UIM_NOT_ENOUGH_MEMORY_SPACE,   /* Not enough memory space                 */

/* Application errors */
  UIM_APPLCN_SPECIFIC_AUTH_ERROR,/* Appln specific Authentication Error     */

/* Reference Problem */
  UIM_ALGORITHM_NOT_SUPPORTED,   /* Algorithm not supported */
  UIM_INVALID_KEY_CHECK_VALUE,   /* Invalid Key Check Value */

#ifdef FEATURE_UIM_WIM
  UIM_SE_NOT_SET,

#endif /* FEATURE_UIM_WIM */

#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_T_1_BLOCK_RX_FAILURE,      /* T=1 block was not received properly     */
#endif /* FEATURE_UIM_T_1_SUPPORT */

/* Not an actual status word returned */
  UIM_PARITY,                     /* Parity Error                            */
  UIM_CONVENTION_ERROR,           /* Error in convention used by the driver  */
  UIM_MAXIMUM_STATUS,
  UIM_MFLO_LOCKED_INIT_PARAM,         /* parameters already intialized */
  UIM_MFLO_APPLET_NOT_INIT,             /* parameters not initialized */
  UIM_MFLO_PUBLIC_KEY_OR_CERT_NOT_INIT, /* public key not initialized */
  UIM_MFLO_UCAST_AUTH_KEY_NOT_INIT,     /* unicast authentication key not initialized */
  UIM_MFLO_UCAST_DATA_BLOCK_NOT_INIT,   /* data blocks not recieved in order */
  UIM_MFLO_VERIFY_UCAST_FAILED          /* unicast signature did not match */

} uim_apdu_status_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW */

/*
    The following determines the clock frequency that drives the SIM glue
    logic.  This is used to determine the UIM frequency and which divide down
    to use in the SIM glue logic to get the proper UIM frequency.
*/
/* Use T_MSM33 and T_UIM_USE_BSP_DEFINES for now to allow PPG feature migration into a
   common archive*/
#define UIM_CLK_TD8_PREPROC              0
#define UIM_CLK_TD4_PREPROC              1
#define UIM_CLK_TD5_PREPROC              2
#if defined(T_QSC6270) || defined(T_MSM8650)
  #define UIM_TCXO_FREQ 19200000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD5_PREPROC
#elif defined(T_UIM_SABER_DEFS)
#error code not present
#elif defined(T_MSM33) || defined(T_UIM_USE_BSP_DEFINES)
#if (BSP_OSCILLATOR_IN == BSP_TCXO_19P2_MHZ)    /* TCXO = 19.20MHz */
  #define UIM_TCXO_FREQ 19200000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P68_MHZ) /* TCXO = 19.68MHz */
  #define UIM_TCXO_FREQ 19680000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_19P8_MHZ)  /* TCXO = 19.8MHz */
  #define UIM_TCXO_FREQ 19800000
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_13P0_MHZ)  /* TCXO = 13.0MHz */
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD4_PREPROC
  #define UIM_TCXO_FREQ 13000000
#elif (BSP_OSCILLATOR_IN == BSP_TCXO_26P0_MHZ)  /* TCXO = 26.0MHz */
  #define UIM_TCXO_FREQ 26000000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#else
#error Unsupported TCXO value
#endif  /* TCXO = 19.2 */

#else /* not T_MSM33 nor T_UIM_USE_BSP_DEFINES */

#ifdef FEATURE_TCXO1920         /* TCXO = 19.20MHz */
  #define UIM_TCXO_FREQ 19200000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#elif defined(FEATURE_TCXO1980) /* TCXO = 19.8MHz */
  #define UIM_TCXO_FREQ 19800000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#else                           /* TCXO = 19.68MHz */
  #define UIM_TCXO_FREQ 19680000
  #define UIM_MAX_FREQ_SUPPORTED_PREPROC         UIM_CLK_TD8_PREPROC
#endif  /* FEATURE_TCXO1920 */
#endif /* T_MSM33 */

/* The UIM clock rate conversion factor is the index value received from the
   UIM card in TA(1) of the ATR or in the PPS.  This index refers to the clock
   rate conversion factor and the MAX clock rate for the UIM.  The clock rate
   conversion factor helps determine the baudrate of the UIM interface                  */
typedef enum {
  UIM_CRCF_372_0,         /* Conversion factor = 372                         */
  UIM_CRCF_372_1,         /* Conversion factor = 372                         */
  UIM_CRCF_558,           /* Conversion factor = 568                         */
  UIM_CRCF_744,           /* Conversion factor = 744                         */
  UIM_CRCF_1116,          /* Conversion factor = 1116                        */
  UIM_CRCF_1488,          /* Conversion factor = 1488                        */
  UIM_CRCF_1860,          /* Conversion factor = 1860                        */
  UIM_CRCF_RFU1,          /* Reserved for future use                         */
  UIM_CRCF_RFU2,          /* Reserved for future use                         */
  UIM_CRCF_512,           /* Conversion factor = 512                         */
  UIM_CRCF_768,           /* Conversion factor = 768                         */
  UIM_CRCF_1024,          /* Conversion factor = 1024                        */
  UIM_CRCF_1536,          /* Conversion factor = 1536                        */
  UIM_CRCF_2048,          /* Conversion factor = 2048                        */
  UIM_CRCF_RFU3,          /* Reserved for future use                         */
  UIM_CRCF_RFU4,          /* Reserved for future use                         */
  UIM_CRCF_SIZE           /* Number of constants of this type                */
} uim_clock_rate_conversion_factor_type;

extern const dword crcf_values[UIM_CRCF_SIZE];

/* This type defines the maximum allowable BRAF for the given UART
   clock source, CRCF and UIM clock frequency (TD8, TD5 and TD4) */
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
#define UIM_MAX_BRAF_TD_INDEX 3
#else
#define UIM_MAX_BRAF_TD_INDEX 2
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
extern const byte UIM_MAX_BRAF[UIM_MAX_BRAF_TD_INDEX][UIM_CRCF_SIZE];


/* The UIM baud rate conversion factor is the index value received from the
   UIM card in TA(1) of the ATR or in the PPS.  This index refers to the baud
   rate adjustment factor for the UIM.  The baud rate adjustment factor helps
   determine the baudrate of the UIM interface                               */
typedef enum {
  UIM_BRAF_RFU1,          /* Reserved for future use                         */
  UIM_BRAF_1,             /* Adjustment factor = 1                           */
  UIM_BRAF_2,             /* Adjustment factor = 2                           */
  UIM_BRAF_4,             /* Adjustment factor = 4                           */
  UIM_BRAF_8,             /* Adjustment factor = 8                           */
  UIM_BRAF_16,            /* Adjustment factor = 16                          */
  UIM_BRAF_32,            /* Adjustment factor = 32                          */
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
  UIM_BRAF_64,            /* Adjustment factor = 64                          */
#else
  UIM_BRAF_RFU2,          /* Reserved for future use                         */
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
  UIM_BRAF_12,            /* Adjustment factor = 12                          */
  UIM_BRAF_20,            /* Adjustment factor = 20                          */
  UIM_BRAF_RFU3,          /* Reserved for future use                         */
  UIM_BRAF_RFU4,          /* Reserved for future use                         */
  UIM_BRAF_RFU5,          /* Reserved for future use                         */
  UIM_BRAF_RFU6,          /* Reserved for future use                         */
  UIM_BRAF_RFU7,          /* Reserved for future use                         */
  UIM_BRAF_RFU8,          /* Reserved for future use                         */
  UIM_BRAF_SIZE           /* Number of constants of this type                */
} uim_baud_rate_adjustment_factor_type;

extern const byte braf_values[UIM_BRAF_SIZE];

typedef enum {

  UIM_WAITING_FOR_T0,    /* Expecting the T0 character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_TD,    /* Expecting a TD character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_TCK,   /* Expecting the TCK character
                             after skipping the num_bytes_to_skip */
  UIM_WAITING_FOR_END    /* Expecting the last of the ATR with NO
                             TCK character expected.              */
} uim_atr_rx_mode_type;

/* These types defined the callback functions used by the UIM driver.
                                                                             */
typedef void (*uim_unsolict_resp_callback_type)( void );

#ifndef FEATURE_VIRTUAL_UIM_HW
typedef void (*uim_resp_callback_type)( void );

typedef enum {
  UIM_TRANSPARENT  = 0,            /* Transparent structure  */
  UIM_LINEAR_FIXED = 1,            /* Linear Fixed structure */
  UIM_CYCLIC       = 3,            /* Cyclic structure       */
#ifdef FEATURE_VIRTUAL_UIM_HW
#error code not present
#endif /* FEATURE_VIRTUAL_UIM_HW */
  UIM_NO_FILE_STRUCT = 0xFF        /* Indicates that no file is selected
                                      and is used for internal processing
                                      only */
} uim_file_structure_type;

typedef enum {
  RFU = 0,               /* Reserved for Future Use */
  MF = 1,                /* Master File             */
  DF = 2,                /* Dedicated File          */
  EF = 4                 /* Elementary File         */
} uim_uim_file_type;

typedef PACKED struct {
  byte           rfu1[2];         /* Reserved for Future Use (RFU) */
  byte           mem[2];          /* Amt of memory of selected directory which
                                     is not allocated to any of the DFs or EFs
                                     under the selected directory */
  byte           file_id[2];      /* File ID */
  uim_uim_file_type  file_type;   /* File Type */
  byte           rfu2[5];         /* RFU */
  byte           len_data;        /* Len of the following GSM Specific data */
} uim_sel_mf_df_rsp_type;

/* featurized it for LTK because LTK assigns 4 bytes for an enum
    where as ARM assigned 1 byte. UIM processes it as 1 byte, so need to
    change it to byte from uim enums.*/
typedef PACKED struct {
  byte                  rfu[2];        /* RFU */
  byte                  file_size[2];  /* File Size */
  byte                  file_id[2];    /* File ID */
#ifdef PLATFORM_LTK
#error code not present
#else
  uim_uim_file_type     file_type;     /* File Type */
#endif
  byte                  inc_allowed;   /* RFU/Inc Access for Cyclic File*/
  byte                  access[3];     /* Access Conditions */
  byte                  file_status;   /* File Status */
  byte                  len_data;      /* Length of following data */
#ifdef PLATFORM_LTK
#error code not present
#else
  uim_file_structure_type file_struct; /* Structure of EF */
#endif
  byte                  rec_len;       /* Length of record */
} uim_sel_ef_rsp_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW */

#ifdef FEATURE_UIM_RUIM
typedef PACKED struct {
  byte usage_ind_conf;                 /* Usage Indicator Confirmation */
} uim_store_esn_rsp_type;

#ifdef FEATURE_OTASP
typedef PACKED struct {
  byte result;                         /* Result of Operation */
} uim_commit_rsp_type;

typedef PACKED struct {
  byte result;                         /* Result of Operation */
} uim_ms_key_rsp_type;

typedef PACKED struct {
  byte result;                         /* Result of Operation */
  byte ms_result_len;                  /* Length of MS Result */
  byte ms_result[UIM_PARAM_P];         /* MS Result */
} uim_key_gen_rsp_type;

typedef PACKED struct {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
} uim_validn_rsp_type;

typedef PACKED struct {
  byte block_id;                       /* Block id */
  byte block_length;                   /* Block length */
  byte result_code;                    /* Result Code */
  byte data[UIM_MAX_CHARS - 3];       /* Data from the card */
} uim_cfg_rsp_type;

typedef PACKED struct {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
} uim_download_rsp_type;

typedef PACKED struct {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
  byte block_length;                   /* Block length */
  byte data[UIM_MAX_CHARS - 3];       /* Data from the card */
} uim_sspr_cfg_rsp_type;

typedef PACKED struct {
  byte block_id;                       /* Block id */
  byte result_code;                    /* Result Code */
  byte segment_offset[2];              /* Segment offset */
  byte segment_size;                   /* Segment size */
} uim_sspr_download_rsp_type;

typedef PACKED struct {
  byte result;                         /* Result of Operation */
  byte nam_lock_ind;                   /* NAM Lock Indication */
  byte rand_otapa[4];                  /* Rand OTAPA */
} uim_otapa_rsp_type;

#endif /* FEATURE_OTASP */
#endif /* FEATURE_UIM_RUIM */

#ifndef FEATURE_VIRTUAL_UIM_HW
typedef union {
  uim_sel_mf_df_rsp_type mf_df;       /* Response to Select MF/DF */
  uim_sel_ef_rsp_type    ef;          /* Response to Select EF */
  byte                   data[UIM_MAX_CHARS]; /* Response Data */
#ifdef FEATURE_UIM_RUIM
  uim_store_esn_rsp_type store_esn;   /* Response to Store ESN */
#ifdef FEATURE_OTASP
  uim_commit_rsp_type    commit;      /* Response to Commit */
  uim_ms_key_rsp_type    ms_key;      /* Response to MS Key Request */
  uim_key_gen_rsp_type   key_gen;     /* Response to Key Gen Request */
  uim_validn_rsp_type    validate;    /* Response to Validation Request */
  uim_cfg_rsp_type       configuration;/* Response to CFG Rsp Type */
  uim_download_rsp_type  download;    /* Response to Download Request */
  uim_sspr_cfg_rsp_type  sspr_config; /* Response to SSPR CFG Rsp Type */
  uim_sspr_download_rsp_type sspr_download; /* Response to SSPR Download Req */
  uim_otapa_rsp_type     otapa;       /* Response to OTAPA Request */
#endif /* FEATURE_OTASP */
#endif /* FEATURE_UIM_RUIM */
} uim_rsp_type;

typedef struct {
  word                 cmd_rsp_size;  /* Command Response Size */
  uim_rsp_type         rsp;           /* Response Type */
  uim_apdu_status_type cmd_status;    /* Command Status */
  byte                 sw1;           /* Status Word 1 */
  byte                 sw2;           /* Status word 2 */
} uim_rsp_buf_type;

typedef PACKED struct {
  byte  high_byte;                    /* High byte of the File to be selected */
  byte  low_byte;                     /* Low byte of the File to be selected */
} uim_sel_mf_df_ef_req_type;

typedef PACKED struct {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_verify_chv_type;

typedef PACKED struct {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* Unblock CHV */
  byte  new_chv[UIM_MAX_CHV_DIGITS];  /* New CHV to replace the old one */
} uim_unblock_chv_type;

typedef PACKED struct {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified */
  byte  new_chv[UIM_MAX_CHV_DIGITS];  /* New CHV to replace the old one */
} uim_change_chv_type;

typedef PACKED struct {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_enable_chv_type;

typedef PACKED struct {
  byte  chv[UIM_MAX_CHV_DIGITS];      /* CHV to be verified*/
} uim_disable_chv_type;

#ifdef FEATURE_UIM_RUIM
typedef PACKED struct {
  byte len_and_usage;                 /* Length of usage and Usage */
  byte esn[7];                        /* ESN */
} uim_store_esn_type;

#ifdef FEATURE_UIM_EUIMID
typedef PACKED struct {
  byte len_and_usage;                 /* Length of usage and Usage */
  byte meid[UIM_RUIM_UIMID_MAX_SIZE]; /* MEID */
} uim_store_meid_type;
#endif /* FEATURE_UIM_EUIMID */

#ifdef FEATURE_OTASP
typedef PACKED struct {
  byte randseed[UIM_RAND_SEED];       /* Random Seed for Key Generation */
  byte a_key_p_rev;                   /* A_KEY_P_REV */
  byte param_p_len;                   /* Param P Length */
  byte param_g_len;                   /* Param G Length */
  byte param_p[UIM_PARAM_P];          /* Param P */
  byte param_g[UIM_PARAM_G];          /* Param G */
} uim_ms_key_req_type;

typedef PACKED struct {
  byte bs_result_len;                 /* Length of BS Result */
  byte bs_result[UIM_PARAM_P];        /* BS Result */
} uim_key_gen_req_type;

typedef PACKED struct {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_validation_req_type;

typedef PACKED struct {
  byte block_id;                      /* Block ID of the Config Request */
} uim_config_req_type;

typedef PACKED struct {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_download_req_type;

typedef PACKED struct {
  byte block_id;                      /* Block ID of the Config Request */
  byte req_offset[2];                 /* Request Offset */
  byte req_max_size;                  /* Request Max Size */
} uim_sspr_config_req_type;

typedef PACKED struct {
  byte block_id;                      /* Block ID of the Download Request */
  byte block_len;                     /* BLock length */
  byte data[UIM_MAX_TRANS_SIZE_OUT - 2];     /* Data buffer */
} uim_sspr_download_req_type;

typedef PACKED struct {
  byte start_stop;                    /* Length of BS Result */
  dword rand_seed;                    /* Random Seed */
} uim_otapa_req_type;

#endif /* FEATURE_OTASP */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_UTIL
typedef PACKED struct {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
  byte new_reference_data[UIM_MAX_CHV_DIGITS]; /* New reference Data */
} uim_change_ref_in_chan_req_type;

typedef PACKED struct {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_verify_in_chan_req_type;

typedef PACKED struct {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_disable_verfn_in_chan_req_type;

typedef PACKED struct {
  byte verification_data[UIM_MAX_CHV_DIGITS];  /* Verification Data */
} uim_enable_verfn_in_chan_req_type;

typedef PACKED struct {
  byte unblock_data[UIM_MAX_CHV_DIGITS];        /* Unblock Data */
  byte new_ref_data[UIM_MAX_CHV_DIGITS];        /* New Reference Data */
} uim_reset_retry_in_chan_req_type;

#ifdef FEATURE_UIM_WIM
typedef PACKED struct {
  byte crdos[UIM_MAX_TRANS_SIZE_OUT];           /* CRDOs */
} uim_mse_req_type;

typedef PACKED struct {
  byte data[UIM_MAX_PSO_DATA];                  /* PSO data */
} uim_pso_req_type;

typedef PACKED struct {
  byte exp_data_len;                            /* expected data length */
} uim_ask_random_req_type;
#endif /* FEATURE_UIM_WIM */
#endif /* FEATURE_UIM_UTIL */

typedef union {
  uim_sel_mf_df_ef_req_type  mf_df_ef;          /* Select file */
  uim_verify_chv_type        verify_chv;        /* Verify CHV */
  uim_unblock_chv_type       unblk_chv;         /* Unblock CHV */
  uim_change_chv_type        change_chv;        /* Change CHV */
  uim_disable_chv_type       disable_chv;       /* Verify CHV */
  uim_enable_chv_type        enable_chv;        /* Verify CHV */
  byte                       data[UIM_MAX_TRANS_SIZE_OUT];
#ifdef FEATURE_UIM_RUIM
  uim_store_esn_type         store_esn;         /* Store ESN Type */
#ifdef FEATURE_UIM_EUIMID
  uim_store_meid_type        store_meid;        /* Store MEID Type */
#endif /* FEATURE_UIM_EUIMID */
#ifdef FEATURE_OTASP
  uim_ms_key_req_type        ms_key;            /* MS Key Request */
  uim_key_gen_req_type       key_gen;           /* Key Gen Request */
  uim_validation_req_type    validate;          /* Validation Request */
  uim_config_req_type        config_req;        /* Configuration Request */
  uim_download_req_type      download_req;      /* Download Request */
  uim_sspr_config_req_type   sspr_cfg_req;      /* SSPR Cfg Request */
  uim_sspr_download_req_type sspr_download_req; /* SSPR Download Request */
  uim_otapa_req_type         otapa_req;         /* OTAPA Request */
#endif /* FEATURE_OTASP */
#endif /* FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UTIL
  uim_verify_in_chan_req_type      verification_in_chan;/* Verify */
  uim_disable_verfn_in_chan_req_type disable_verfn_in_chan;/* Disable Verfictn */
  uim_enable_verfn_in_chan_req_type  enable_verfn_in_chan; /* Enable Verfictn */
  uim_reset_retry_in_chan_req_type   retry_in_chan; /* Reset Retry Counter */
  uim_reset_retry_in_chan_req_type reset_retry_in_chan;/* Reset Retry Counter */
  uim_change_ref_in_chan_req_type  change_ref_in_chan; /* Change Ref Data */
#ifdef FEATURE_UIM_WIM
  uim_mse_req_type           mse;               /* Manage Security Env */
  uim_pso_req_type           pso;               /* Perform Security Op */
  uim_ask_random_req_type    ask_random;        /* Ask Random */
#endif /* FEATURE_UIM_WIM */
#endif /* FEATURE_UIM_UTIL */
} uim_req_type;

typedef PACKED struct {
  byte                    uim_class;  /* UIM class of instruction           */
  uim_instrns_enum_type  instrn;      /* Instruction type                   */
  byte                    p1;         /* input parameter 1                  */
  byte                    p2;         /* input parameter 2                  */
  byte                    p3;         /* input parameter 3                  */
} uim_apdu_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW */

typedef enum {
  UIM_INSTRN_CASE_1    = 0, /* Case 1 - No data exchanged */
  UIM_INSTRN_CASE_2    = 1, /* Case 2 - Response data only */
  UIM_INSTRN_CASE_3    = 2, /* Case 3 - Command data only */
  UIM_INSTRN_CASE_4    = 3, /* Case 4 - Response and command data */
  UIM_INSTRN_IFS_BLOCK = 4  /* Used to send IFS requests to the driver */
} uim_instrn_case_type;

typedef struct {
  uim_protocol_type protocol;          /* Protocol of the APDU */
  uim_apdu_type     apdu_hdr;          /* APDU header */
  uim_req_type      req;               /* Request */
  uim_rsp_buf_type  *rsp_ptr;          /* Response pointer */
  uim_resp_callback_type rpt_function; /* call back function */
  uim_instrn_case_type instrn_case;    /* instruction case */
} uim_req_buf_type;

/* This type defines how to change the operations parameters. */
typedef struct {
  boolean change_clk_freq;           /* This indicates whether to change the UIM
                                        clock frequency                       */
  uim_clk_freq_type clock_frequency; /* This holds the selected clock
                                        frequency                             */
  boolean change_baud_rate;          /* This indicates whether to change the UIM
                                        interface baudrate                    */

  /* These parameters determine the baudrate of the UIM interface.            */
  uim_clock_rate_conversion_factor_type FI;
                                    /* Holds the UIM clock rate conversion    */
  uim_baud_rate_adjustment_factor_type  DI;
                                    /* Holds the baudrate adjustment factor   */
  boolean change_guardtime;         /* This indicates whether to change the UIM
                                       guardtime                              */
  byte guardtime_bits;              /* Number of ETUs to increase the
                                       guardtime                              */
} uim_op_params_type;

#ifndef  FEATURE_VIRTUAL_UIM_HW 
/*
  This type defines the PPS request parameters.  The parameters contain the
  PPS request as well as where to put the PPS response.
 */
typedef struct {
  byte size_of_pps_request;   /* This is the size of the PPS request          */
  byte *request_buf;          /* This points to the PPS request.              */
  uim_rsp_buf_type  *rsp_ptr; /* This points to where to place the PPS
                                 response                                     */
  uim_resp_callback_type pps_resp_callback;/* Callback for response           */
} uim_pps_req_buf_type;
#endif /* !FEATURE_VIRTUAL_UIM_HW */

/*===========================================================================

FUNCTION UIM_DEV_INIT                             EXTERNALIZED FUNCTION

DESCRIPTION
  Initialize the clock regime, configures the GPIOs, MSM UART2 serial
  interface.  It places the UIM power line low, the UIM reset line low,
  the I/O line low and the clock line low.

DEPENDENCIES
  The routine MUST be called before any other UIM driver function.  It must
  only be called once on power up of the phone.

RETURN VALUE
  None

SIDE EFFECTS
  This function leaves ALL the UIM lines low after it executes.

===========================================================================*/
extern void uim_dev_init
(
  void
);


/*===========================================================================

FUNCTION UIM_POWER_UP                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns on the power for the UIM.  It puts the I/O line in
  reception mode (stops sending break on the I/O line) and turns on the
  UIM clock.

DEPENDENCIES
  This function must be executed either after uim_dex_init or after
  uim_power_down.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is powered up and has a clock.

===========================================================================*/
extern void uim_power_up
(
#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
  uim_unsolict_resp_callback_type unsolict_resp_callback
#else
  void
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */
);


/*===========================================================================

FUNCTION UIM_RESET                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function asserts the reset line on the UIM.  Any data received by the
  phone will be flushed.  Any transfer from the phone will be terminated and
  the buffers flushed.

DEPENDENCIES
  This function must be executed after uim_dex_init.  The UIM should be powered
  up (via uim_power_up) for this function to have any effect.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver is expecting an ATR after this reset.

===========================================================================*/
extern void uim_reset
(
  uim_rsp_buf_type  *rsp_ptr,    /* Defines where to put the ATR           */
  uim_resp_callback_type atr_rx_callback /* Callback for ATR               */
);


/*===========================================================================

FUNCTION UIM_SEND_PPS                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of a PPS request to the UIM.  This
  function starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts a PPS exchange.  This exchange can only be done
  immediately after an ATR is received.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the PPS request.  Then the
  driver will expect the PPS response.

===========================================================================*/
extern void uim_send_pps
(
  uim_pps_req_buf_type const *pps_request  /* This points to the PPS request and
                                         the PPS response buffer.           */
);


/*===========================================================================

FUNCTION UIM_UPDATE_OP_PARAMS                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the operational parameters of the UIM interface.  It
  can change the baud rate of the UIM interface, change the UIM clock
  frequency or change the guardtime.

DEPENDENCIES
  This function changes the operational parameters of the UIM interface.
  Whatever changes are requested in this function shall apply as soon as
  possible.  The interface will reflect the changes.

RETURN VALUE
  None

SIDE EFFECTS
  All transfers over the UIM interface will occur at the frequency, baudrate,
  or guardtime as specified in this function call.

===========================================================================*/
extern void uim_update_op_params
(
  uim_op_params_type const *op_params,       /* Proposed operational parameters */
  uim_drv_slot_type drv_slot
);


#ifdef UIM_DRIVER_TIMESTAMP
/*===========================================================================

FUNCTION uim_StartDriverTimeStamp                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function starts the usage of the uim_rx_isr driver time stamp mechanism
  and sets the priority of the rx_isr to a high priority

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void uim_StartDriverTimeStamp
( void
);

/*===========================================================================

FUNCTION uim_StopDriverTimeStamp                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function stops the usage of the time stamping mechanism of the
  rx_isr.  Also lowers back the priority of the rx_isr back to normal
  and sets back the Watermark level to 5

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void uim_StopDriverTimeStamp( void );

#endif /* UIM_DRIVER_TIMESTAMP */

/*===========================================================================

FUNCTION UIM_SEND_COMMAND                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of an APDU to the UIM.  This function
  starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the APDU response.  Some commands solicit a GET_RESPONSE or a FETCH command
  to be sent after the APDU.

===========================================================================*/
extern void uim_send_command
(
  uim_req_buf_type *uim_req /* This points to the APDU command buffer, the
                                 command data buffer, the response buffer, the
                                 APDU status and the callback function.       */
);


#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
/*===========================================================================

FUNCTION UIM_IS_FI_DI_SUPPORTED                        EXTERNALIZED FUNCTION

DESCRIPTION
  This function tells if a {FI, DI} combination is supported by the
  platform or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern boolean uim_is_fi_di_supported
(
  uim_clock_rate_conversion_factor_type  FI, /* CRCF index */
  uim_baud_rate_adjustment_factor_type   DI /* BRAF index */
);
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION UIM_BYTES_WAITING_IN_RXFIFO                    EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the size of RX data waiting to be read
  in the UART FIFO and packing buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern int32 uim_bytes_waiting_in_rxfifo
(
  void
);
#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION UIM_POWER_DOWN                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function deactivates the UIM so it does becomes inactive and does not
  use power.  The electrical lines to the UIM are all set low in the
  appropriate order.

DEPENDENCIES
  This function inactivates the UIM.  The uim_power_up and uim_reset
  functions must be called after this function to bring the UIM back up
  to use.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is power down and inactive.

===========================================================================*/
extern void uim_power_down
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif
);

#if defined( FEATURE_UIM_T_1_SUPPORT )


/*===========================================================================

FUNCTION UIM_T_1_SEND_I_BLOCK

DESCRIPTION
  This function builds a T=1 information block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the information block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The information block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
extern void uim_t_1_send_i_block
(
  byte node_address
);


/*===========================================================================

FUNCTION UIM_T_1_SEND_R_BLOCK

DESCRIPTION
  This function builds a T=1 receive ready block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the receive ready block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The receive ready block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
extern void uim_t_1_send_r_block
(
  byte node_address,        /* Node address byte */
  byte r_block_code         /* R-block response code */
);


/*===========================================================================

FUNCTION UIM_T_1_SEND_S_BLOCK

DESCRIPTION
  This function builds a T=1 supervisory block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the supervisory block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The supervisory block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
extern void uim_t_1_send_s_block
(
  byte node_address,        /* Node address byte */
  byte s_block_code,        /* S-block code */
  byte *s_block_data,       /* S-block data */
  byte s_block_data_size    /* S-block data size */
);
#endif /* FEATURE_UIM_T_1_SUPPORT */

#endif /* FEATURE_UIM || FEATURE_WCDMA_PLT */

/*===========================================================================

MACRO
  UIM_TEST_INT_PROCESSING

DESCRIPTION
  The following macro facilitates in the timing of latencies and
  durations of the UIM ISR and command processing.

  UIM_TEST_INT_PROCESSING:  Writes the given parameter to the debug port.

PARAMETERS
  out_val
    This value is written to the debug port.

DEPENDENCIES
  Must have FEATURE_UIM_DRIVER_TIME_TEST defined for this to take
  effect.

RETURN VALUE
  None

SIDE EFFECTS
  Because the instructions (if activated) take non-zero time (i.e. invasive)
  these macros will skew the timing results.

===========================================================================*/
#ifdef FEATURE_UIM_DRIVER_TIME_TEST
  #define UIM_TEST_INT_PROCESSING(out_val)               \
    outpw( TIMETEST_PORT, out_val);

  /*
   * These values are used in the processing timing of the ISR
   */
  #define UIM_DEBUG_EXIT_UART_ISR              0x00 /*  0 */
  #define UIM_DEBUG_ENTER_UART_ISR             0x01 /*  1 */
  #define UIM_DEBUG_PROCESS_IDLE               0x02 /*  2 */
  #define UIM_DEBUG_RECEIVE_ATR                0x03 /*  3 */
  #define UIM_DEBUG_TX_BYTE                    0x04 /*  4 */
  #define UIM_DEBUG_GUARDTIME                  0x05 /*  5 */
  //#define UIM_DEBUG_SEND_PPS                   0x04 /*  4 */
  #define UIM_DEBUG_RECEIVE_PPS                0x06 /*  6 */
  //#define UIM_DEBUG_SEND_CMD_HDR               0x06 /*  6 */
  #define UIM_DEBUG_PROC_PRCDR_BYTES           0x07 /*  7 */
  #define UIM_DEBUG_SW1_NORM_END               0x08 /*  8 */
  #define UIM_DEBUG_SW1_FETCH_END              0x09 /*  9 */
  #define UIM_DEBUG_SW1_RESP_END               0x0A /* 10 */
  #define UIM_DEBUG_SW1_BUSY                   0x0B /* 11 */
  #define UIM_DEBUG_SW1_RETRIES                0x0C /* 12 */
  #define UIM_DEBUG_SW1_REFERENCE              0x0D /* 13 */
  #define UIM_DEBUG_SW1_SECURITY               0x0E /* 14 */
  #define UIM_DEBUG_SW1_P3_BAD                 0x0F /* 15 */
  #define UIM_DEBUG_SW1_P1_P2_BAD              0x10 /* 16 */
  #define UIM_DEBUG_SW1_INS_BAD                0x11 /* 17 */
  #define UIM_DEBUG_SW1_CLA_BAD                0x12 /* 18 */
  #define UIM_DEBUG_SW1_PROBLEM                0x13 /* 19 */
  //#define UIM_DEBUG_SND_CMD_DATA               0x14 /* 20 */
  #define UIM_DEBUG_PROC_RESP_DATA             0x15 /* 21 */
  #define UIM_DEBUG_RX_ERROR                   0x16 /* 22 */
  #define UIM_DEBUG_SEND_BYTE                  0x17 /* 23 */
  #define UIM_DEBUG_LAST_BYTE                  0x18 /* 24 */

#ifdef FEATURE_UIM_UICC
  #define UIM_SW1_USIM_END_RESP                0x19 /* 25 */
  #define UIM_SW1_USIM_RESEND_APDU             0x20 /* 26 */
  #define UIM_SW1_WARNINGS1                    0x21 /* 27 */
  #define UIM_SW1_WARNINGS2                    0x22 /* 28 */
  #define UIM_SW1_EXECUTION_ERR1               0x23 /* 29 */
  #define UIM_SW1_EXECUTION_ERR2               0x24 /* 30 */
  #define UIM_SW1_CLA_NOT_SUPPORTED            0x25 /* 31 */
  #define UIM_SW1_CMD_NOT_ALLOWED              0x26 /* 32 */
  #define UIM_SW1_WRONG_PARAMS                 0x27 /* 33 */
#endif /* FEATURE_UIM_UICC */

  #define UIM_DEBUG_RECEIVED_CMD               0x9000 /* 32,768 */
  #define UIM_DEBUG_SEND_CMD                   0x8100 /* 33,024 */
  #define UIM_DEBUG_NORMAL_END                 0x8200 /* 33,280 */
  #define UIM_DEBUG_ERROR_END                  0x8300 /* 33,536 */
  #define UIM_DEBUG_SEND_RESPONSE              0x8400 /* 33,792 */
  #define UIM_DEBUG_IDLE                       0x8500 /* 34,048 */
  #define UIM_DEBUG_DEV_INIT                   0x8600 /* 34,304 */
#else  /* FEATURE_UIM_DRIVER_TIME_TEST */
  #define UIM_TEST_INT_PROCESSING(out_val)
#endif /* FEATURE_UIM_DRIVER_TIME_TEST */


/* Status Register */
#define MSMU_SR_RXHUNT    0x80         /* Hunt Character Received          */
#define MSMU_SR_RXBREAK   0x40         /* Break Received                   */
#define MSMU_SR_PF_ERR    0x20         /* Parity or Framing Error          */
#define MSMU_SR_OVR_ERR   0x10         /* Overrun Error                    */
#define MSMU_SR_TXEMT     0x08         /* Transmitter Empty                */
#define MSMU_SR_TXRDY     0x04         /* Transmitter Ready                */
#define MSMU_SR_RXFULL    0x02         /* Receiver Full                    */
#define MSMU_SR_RXRDY     0x01         /* Receiver Ready                   */



/*===========================================================================

MACRO UIM_IS_UIM_CLK_ON


DESCRIPTION
  Checks to see if the UIM clock is on

DEPENDENCIES
  None

RETURN VALUE
  TRUE if Clock is on, or if FEATURE_UIM_TCXO_SLEEP is not defined
  FALSE is Clock is off (or if UIM has voted Clock off)

SIDE EFFECTS
  None

===========================================================================*/
#if defined(FEATURE_UIM_TCXO_SLEEP)
  #if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    #define UIM_IS_UIM_CLK_ON()                            \
      (((UIM_DRV_SLOT1 == uim_drv_slot) && uim_clock_is_on) ||
      ((UIM_DRV_SLOT2 == uim_drv_slot) && uim_clock_is_on_2))
  #elif !defined(FEATURE_UIM2)
    #define UIM_IS_UIM_CLK_ON() uim_clock_is_on
  #else
    #define UIM_IS_UIM_CLK_ON() uim_clock_is_on_2
  #endif /* FEATURE_UIM1 && FEATURE_UIM2 */
#else
  #define UIM_IS_UIM_CLK_ON() TRUE
#endif /* FEATURE_UIM_TCXO_SLEEP */


typedef enum {
  UIM_RX_PROCESS_IDLE,   /* Not expecting any data               */
  UIM_RX_WAIT_FOR_TXREADY, /* State to wait for the TX_READY state */
  UIM_RX_RECEIVE_ATR,    /* Expecting the ATR                    */
  UIM_RX_TX_BYTE,        /* Expecting the sample bytes of the transmitted
                             byte along with the error signal */
  UIM_RX_GUARDTIME,      /* Waiting for the guartime to pass */
  UIM_RX_LAST_TX_BYTE,   /* Expecting the last byte and either an
                             error signal or procedure byte      */
  UIM_RX_RECEIVE_PPS,    /* Expecting the PPS response           */
  UIM_RX_PROC_PRCDR_BYTES,/* Expecting procedure bytes from the UIM  */
  UIM_RX_OVERRUN,        /* UART is overrun */
  UIM_RX_SW1_NORM_END,   /* Expecting the second status word     */
  UIM_RX_SW1_FETCH_END,  /* Expecting the second status word     */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  UIM_RX_SW1_DLOAD_ERR,  /* Expecting the second status word     */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
  UIM_RX_SW1_RESP_END,   /* Expecting the second status word     */
  UIM_RX_SW1_BUSY,       /* Expecting the second status word     */
  UIM_RX_SW1_RETRIES,    /* Expecting the second status word     */
  UIM_RX_SW1_REFERENCE,  /* Expecting the second status word     */
  UIM_RX_SW1_SECURITY,   /* Expecting the second status word     */
  UIM_RX_SW1_P3_BAD,     /* Expecting the second status word     */
  UIM_RX_SW1_P1_P2_BAD,  /* Expecting the second status word     */
  UIM_RX_SW1_INS_BAD,    /* Expecting the second status word     */
  UIM_RX_SW1_CLA_BAD,    /* Expecting the second status word     */
  UIM_RX_SW1_PROBLEM,    /* Expecting the second status word     */
  UIM_RX_PROC_RESP_DATA, /* Expecting the APDU response          */
  UIM_RX_SW1_WARNINGS1,  /* Expecting the second status word     */
  UIM_RX_SW1_WARNINGS2   /* Expecting the second status word     */

#if defined( FEATURE_UIM_T_1_SUPPORT )
  ,UIM_RX_T_1_RECV_T_1_BLOCK_DM  /* Receive the T=1 block for T=1         */
  ,UIM_RX_T_1_SEND_PROLOGUE_ST   /* Send the prologue field for T=1       */
  ,UIM_RX_T_1_SEND_INFO_ST       /* Send the information field for T=1    */
  ,UIM_RX_T_1_SEND_EPILOGUE_ST   /* Send the epilogue field for T=1       */
  ,UIM_RX_T_1_RX_PROLOGUE_ST     /* Receive the prologue field for T=1    */
  ,UIM_RX_T_1_RX_INFO_ST         /* Receive the information field for T=1 */
  ,UIM_RX_T_1_RX_EPILOGUE_ST     /* Receive the epilogue field for T=1    */
#endif /* FEATURE_UIM_T_1_SUPPORT */

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
  ,UIM_RX_SW1_EXECUTION_ERR2     /* Expecting the second status word     */
  ,UIM_RX_SW1_CMD_NOT_ALLOWED    /* Expecting the second status word     */
  ,UIM_RX_SW1_WRONG_PARAMS       /* Expecting the second status word     */
  ,UIM_RX_SW1_USIM_RESEND_APDU   /* Expecting the second status word     */
  ,UIM_RX_SW1_USIM_END_RESP      /* Expecting the second status word     */

#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

#ifdef FEATURE_UIM_UICC
  ,UIM_RX_SW1_EXECUTION_ERR1     /* Expecting the second status word     */
  ,UIM_RX_SW1_CLA_NOT_SUPPORTED  /* Expecting the second status word     */
#endif /* FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_WIM
  ,UIM_RX_SW1_SE_FAIL           /* Expecting the second status word    */
#endif /* FEATURE_UIM_WIM */
  ,UIM_RX_UNKNOWN_PROC_BYTE_RCVD /* Un-known procedure byte received */

} uim_rx_state_type;

/*
   The following defines the return status of the uim_process_guardtime
   function.
 */
typedef enum {
  UIM_TX_SEND_PPS,       /* Sending the PPS bytes                */
  UIM_TX_SEND_CMD_HDR,   /* Sending the APDU command header      */
  UIM_TX_SND_CMD_DATA    /* Sending the command data bytes       */
} uim_tx_state_type;

extern uim_rx_state_type uim_rx_state[UIM_NUM_DRV_SLOTS];
extern uim_tx_state_type uim_tx_state[UIM_NUM_DRV_SLOTS];
extern byte *request_buffer_ptr[UIM_NUM_DRV_SLOTS];
extern int  num_bytes_to_send[UIM_NUM_DRV_SLOTS];
extern byte *resp_buffer_ptr[UIM_NUM_DRV_SLOTS];
extern word num_resp_data_bytes_expected[UIM_NUM_DRV_SLOTS];
extern word  num_resp_bytes_rcvd[UIM_NUM_DRV_SLOTS];
#ifdef FEATURE_UIM_UART_DM
extern int32 num_bytes_in_this_rx_transfer[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_UART_DM */

extern dword uim_read_status(void);

/*===========================================================================

FUNCTION UIM_ENABLE_VREG_LPM

DESCRIPTION
  The PMIC has the ability to put the different Voltage Regulators into
  a Low Power Mode.  The Regulator will go in and out of LPM based on
  the TCXO state of TCXO.  When in TXCO Shutdown, VREG (R)UIM is put
  into an LPM Mode. When TCXO is enabled, VREG (R)UIM is pulled out
  of LPM Mode.

  Going in an out of LPM is controlled by the PMIC.

  The UIM Drivers will need to ensure that the PMIC is notified that LPM
  mode is allowed for VREG (R)UIM.

  The UIM will not have to take VREG (R)UIM out of LPM

DEPENDENCIES
  Prior to calling this function, we must ensure from a system perspective
  that the following functions have been called.

  1) rflib_init()
  2) pm_init()

RETURN VALUE
  void - If setting to LPM Fails, not much the UIM can do about it.

SIDE EFFECTS
  After this function is executed, the VREG (R)UIM is allowed to get into
  LPM following the state of TCXO.

===========================================================================*/
void uim_enable_vreg_lpm
(
  void
);


#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */


#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

/*===========================================================================

FUNCTION UIM_CMD_RSP_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the command response timer. This call-back is called in the context of the
  high priority timer task as soon as the rex timer for the work waiting time
  expires.

  The logic in this call-back is as follows:
    - (for LCU) check if the count in the rx fifo + the packing buffer increased
        from the last check.
          - If the count increased, then re-start the wwt timer.
          - else, put driver’s uim_rx_state to IDLE, mask all interrupts, reset
            uart, set cmd_rsp_timeout signal and powerdown the sim interface

    - (for non-LCU) check if RXRDY bit is set in the status. If set,
        set cmd_rsp_timeout and return. If not set, put driver’s uim-rx-state to
        idle, mask all interrupts, reset uart, set cmd_rsp_timeout signal and
        power down the sim interface.

  NOTE: The powerdown sequence of the UIM (as in the uim_power_down() function)
        is not started within this call-back as it is a huge-piece of logic
        which might hog the CPU as this call-back is being executed in the
        context of the timer task. Hence, this call-back only signals the
        UIM task to initiate the powerdown sequence, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Power down of the SIM interface if the command response timeout expired.

===========================================================================*/
void uim_cmd_rsp_timer_expiry_cb
(
  unsigned long unused
);

#endif /* UIMDRV_H */


#ifndef _BTQSOCNVM_H_
#define _BTQSOCNVM_H_
/**
 * @file btqsocnvm.h
 *  This file implements Bluetooth Host Controller Common
 *  interface (UART, GPIO, Power, AUX_PCM)
 *
 * Copyright (c) 2007-2011 QUALCOMM Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * 
 * 
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/soccfg/rel/00.00.26/src/btqsocnvm.h#4 $
  $DateTime: 2011/01/17 23:23:57 $
  $Author: roystonr $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-10-25  tw  Added support for 4025 B3
  2009-08-07  dgh  Added support for 4025 B2.
  2009-06-02   sa  Support for Class 2 device.
  2008-11-14   sa  Support for NV automation.
  2008-10-27   sa  Added functionality to read SOC hw version register
                   (Currently this is used to differentiate BT 4020 BD B1 from 
		   BT 4020 DB B0, the cmd BT_QSOC_VS_EDL_GETAPPVER_REQ 
		   response alone cannot distinguish BT 4020 BD B1 from B1).
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-08-30   rb  Added preliminay support for 4020 BD B1.
  2008-08-20   rb  Added support to 4025 B1
  2008-07-28   sa  Added support to read Clk type,Clk sharing,in-band sleep mode, 
                   soc logging enabled, BT Wlan co-ext, BTS power mgmt mode  
				   from NV browser at run-time. 
  2008-05-27   rb  Fix compiler warnings
  2008-05-15   rb  Support for 4025 B0 Beta
  2008-05-06   rb  Added support for 4025 A0
  2008-03-13   rb  Added support for 4021 B1 and 4020 BD B0. 

  01/31/2008   bretth     Branched and renamed file from WM
  01/31/2008   ruih       Merged in bretth's latest update
  12/21/2007   damu       Initial version
===============================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif 



/*===========================================================================
                         INCLUDE FILES FOR MODULE
===========================================================================*/
#include "btqsocnvm_pf_def.h"  

/* Vendor Specific command codes */
#define BT_QSOC_EDL_CMD_OPCODE             (0xFC00)
#define BT_QSOC_NVM_ACCESS_OPCODE          (0xFC0B)

#define BT_QSOC_EDL_CMD_CODE             (0x00)
#define BT_QSOC_NVM_ACCESS_CODE          (0x0B)

/*=========================================================================*/
/*         Macros for BTS VS Download command/response types               */
/*=========================================================================*/

/* EDL Command sub-command codes */
#define BT_QSOC_VS_EDL_START_REQ         (0x00)
#define BT_QSOC_VS_EDL_DNLOAD_REQ        (0x01)
#define BT_QSOC_VS_EDL_UPLOAD_REQ        (0x02)
#define BT_QSOC_VS_EDL_GETSTATUS_REQ     (0x03)
#define BT_QSOC_VS_EDL_SETPC_REQ         (0x04)
#define BT_QSOC_VS_EDL_RESET_REQ         (0x05)
#define BT_QSOC_VS_EDL_GETAPPVER_REQ     (0x06)
#define BT_QSOC_VS_EDL_GET_DLVER_REQ     (0x07)
#define BT_QSOC_VS_EDL_VERIFY_REQ        (0x08)
#define BT_QSOC_VS_EDL_SIGNATURE_REQ     (0x09)
#define BT_QSOC_VS_EDL_POKE16_REQ        (0x0A)
#define BT_QSOC_VS_EDL_PEEK16_REQ        (0x0B)
#define BT_QSOC_VS_EDL_POKE32_REQ        (0x0C)
#define BT_QSOC_VS_EDL_PEEK32_REQ        (0x0D)
#define BT_QSOC_VS_EDL_NVMADD_REQ        (0x0E)
#define BT_QSOC_VS_EDL_CLK_BAUD_CHG_REQ  (0x0F)

/* Download Events */
#define BT_QSOC_VS_EDL_STATUS_RESP       (0x00)
#define BT_QSOC_VS_EDL_UPLOAD_RESP       (0x01)
#define BT_QSOC_VS_EDL_APPVER_RESP       (0x02)
#define BT_QSOC_VS_EDL_DLVER_RESP        (0x03)
#define BT_QSOC_VS_EDL_NVMADD_RESP       (0x0E)

/* Macros for BTS VS NVM access type (get/set)  */
#define BT_QSOC_VS_NVM_ACCESS_GET        (0x00)
#define BT_QSOC_VS_NVM_ACCESS_SET        (0x01)

/* Macros for BTS VS CMD sub-command codes */
#define BT_QSOC_VS_CMD_PAGE_CANCEL_REQ            (0x00)
#define BT_QSOC_VS_CMD_READ_RSSI_LINEAR_REQ       (0x01)
#define BT_QSOC_VS_CMD_WRITE_RSSI_THRESHOLD_REQ   (0x02)
#define BT_QSOC_VS_CMD_READ_RSSI_THRESHOLD_REQ    (0x03)
#define BT_QSOC_VS_CMD_ABSENCE_NOTIFY_REQ         (0x04)
#define BT_QSOC_VS_CMD_MAX_ABSENCES_REQ           (0x05)
#define BT_QSOC_VS_CMD_READ_EKL_REQ               (0x06)

/* Macros for BTS VS CMD Events */
#define BT_QSOC_VS_CMD_PAGE_CANCEL_RESP           (0x00)
#define BT_QSOC_VS_CMD_READ_RSSI_LINEAR_RESP      (0x01)
#define BT_QSOC_VS_CMD_WRITE_RSSI_THRESHOLD_RESP  (0x02)
#define BT_QSOC_VS_CMD_READ_RSSI_THRESHOLD_RESP   (0x03)
#define BT_QSOC_VS_CMD_ABSENCE_NOTIFY_RESP        (0x04)
#define BT_QSOC_VS_CMD_MAX_ABSENCES_RESP          (0x05)
#define BT_QSOC_VS_CMD_READ_EKL_RESP              (0x06)

#define BT_QSOC_SHUTDOWNBL_COUNT     1

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
#define BT_QSOC_R3_POKETBL_COUNT     2
#else
#define BT_QSOC_R3_POKETBL_COUNT     1
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

#define BT_QSOC_MAX_VS_POKE8_SIZE    0xF0  /* Maximum length of poke8 payload */
#define BT_QSOC_MAX_BD_ADDRESS_SIZE  0x06  /* Length of BD Address */

/* BTS version identification strings */
#define BT_QSOC_R2B_STR_ID                 "Release 3.03.0"
#define BT_QSOC_R2C_STR_ID                 "Release 3.03.1"
#define BT_QSOC_R3_STR_ID                  "Release 3.03.2"
#define BT_QSOC_R3BD_STR_ID                "Release 3.03.3"
#define BT_QSOC_R4_STR_ID                  "Release 3.03.4"
/* BD B0 and B1 Id are the same */	
#define BT_QSOC_4020BD_B0_or_B1_STR_ID     "Release 3.03.5"
#define BT_QSOC_4021_B1_STR_ID             "Release 4.03.0"
#define BT_QSOC_4025_A0_STR_ID             "Release 4.00.0"
#define BT_QSOC_4025_B0_STR_ID             "Release 4.00.1"
#define BT_QSOC_4025_B1_STR_ID             "Release 4.00.2"
#define BT_QSOC_4025_B2_STR_ID             "Release 4.00.3"
#define BT_QSOC_4025_B3_STR_ID             "Release 4.00.4"

#define   HCI_VS_NULL_CHAR (0x20)

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/
/*===========================================================================
             Tables for NVM configuration during bootup
===========================================================================*/

/* Enum type for parser error */
typedef enum
{
  BTQSOCNVM_NO_ERROR = 0,
  BTQSOCNVM_NUMBER_FORMAT_ERR,
  BTQSOCNVM_QSOC_NOT_SUPPORTED
} btqsocnvm_parser_err_type;


/* Enum type for BT QSOC FW versions. */
typedef enum
{
  BT_QSOC_R2B,
  BT_QSOC_R2C,
  BT_QSOC_R3,
  BT_QSOC_R3BD,
  BT_QSOC_4020BD_B0,
  BT_QSOC_4020BD_B1,
  BT_QSOC_R4,
  BT_QSOC_4021_B1,
  BT_QSOC_4025_A0,
  BT_QSOC_4025_B0,
  BT_QSOC_4025_B1,
  BT_QSOC_4025_B2,
  BT_QSOC_4025_B3,
  BT_QSOC_UNKNOWN,
  BT_QSOC_MAX,
  BT_QSOC_NONE = 0xFF
} bt_qsoc_enum_type;

typedef enum
{
  BT_SOC_REFCLOCK_32MHZ = 0,
  BT_SOC_REFCLOCK_19P2MHZ
} bt_soc_refclock_type;

typedef enum
{
  BT_SOC_CLOCK_SHARING_DISABLED = 0,
  BT_SOC_CLOCK_SHARING_ENABLED
} bt_soc_clock_sharing_type;

typedef enum
{
  BT_SOC_HW_INBAND_SLEEP = 0,
  BT_SOC_SW_INBAND_SLEEP
} bt_soc_inband_sleep_type;

/* Structure for storing BT QSOC related information. */
typedef const OI_UINT8 bt_qsoc_cfg_cst_element_type;
typedef OI_UINT8 bt_qsoc_cfg_element_type;

typedef enum
{
  BT_QSOC_DEV_CLASS1 = 0,
  BT_QSOC_DEV_CLASS2
} bt_qsoc_device_class_type;

/* Structure for storing BT QSOC related information. */
typedef struct
{
  bt_qsoc_cfg_cst_element_type **nvm_fixed_table_ptr;
  bt_qsoc_cfg_cst_element_type **nvm_runtime_table_ptr;
  OI_UINT16 nvm_fixed_table_size;
  OI_UINT16 nvm_runtime_table_size;
  OI_UINT8 bd_address[BT_QSOC_MAX_BD_ADDRESS_SIZE];
  bt_soc_refclock_type refclock_type;
  bt_soc_clock_sharing_type clock_sharing;
  OI_UINT8 soc_logging;
  uint8 bt_2_1_lisbon_disabled;
  bt_qsoc_device_class_type bt_qsoc_device_class;
} bt_qsoc_runtime_params_struct_type;

typedef struct
{
  OI_UINT8  vs_poke8_data_len;
  OI_UINT8  vs_poke8_data[BT_QSOC_MAX_VS_POKE8_SIZE];
} bt_qsoc_poke8_tbl_struct_type;

/* Enum type for BT QSOC Baud rates */
typedef enum
{
  BT_QSOC_BAUD_115200      = 0x0,
  BT_QSOC_BAUD_57600       = 0x1,
  BT_QSOC_BAUD_38400       = 0x2,
  BT_QSOC_BAUD_19200       = 0x3,
  BT_QSOC_BAUD_9600        = 0x4,
  BT_QSOC_BAUD_230400      = 0x5,
  BT_QSOC_BAUD_250000      = 0x6,
  BT_QSOC_BAUD_460800      = 0x7,
  BT_QSOC_BAUD_500000      = 0x8,
  BT_QSOC_BAUD_720000      = 0x9,
  BT_QSOC_BAUD_921600      = 0xA,
  BT_QSOC_BAUD_1000000     = 0xB,
  BT_QSOC_BAUD_125000      = 0xC,
  BT_QSOC_BAUD_2000000     = 0xD,
  BT_QSOC_BAUD_3000000     = 0xE,
  BT_QSOC_BAUD_4000000     = 0xF,
  BT_QSOC_BAUD_1600000     = 0x10,
  BT_QSOC_BAUD_3200000     = 0x11,
  BT_QSOC_BAUD_3500000     = 0x12,
  BT_QSOC_BAUD_Auto        = 0xFE,
  BT_QSOC_BAUD_UserDefined = 0xFF 
} bt_qsoc_enum_baud_rate;

/*=========================================================================*/
/*                  Macros for BTS UART Bit-rate                           */
/*=========================================================================*/

#ifdef FEATURE_BT_SOC_BITRATE_460800

#define BT_QSOC_BAUD_RATE  BT_QSOC_BAUD_460800

#elif defined FEATURE_BT_SOC_BITRATE_115200

#define BT_QSOC_BAUD_RATE  BT_QSOC_BAUD_115200

#else

#define BT_QSOC_BAUD_RATE  BT_QSOC_BAUD_3200000

#endif /* FEATURE_BT_SOC_BITRATE_460800 */

/*===========================================================================
                                                            Globals
===========================================================================*/
/* Fixed poke table for BTS' VS Poke8 command. The elements of this  
 * table are pretty much hardcoded.  
 */
extern const bt_qsoc_poke8_tbl_struct_type bt_qsoc_vs_poke8_tbl_r3[];

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_nvm_entries

  DESCRIPTION    This routine updates the run time nvm parameters.

  DEPENDENCIES   None.

  PARAMETERS     soc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If table found else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_update_nvm_entries
(
  bt_qsoc_enum_type soc_type,
  bt_qsoc_runtime_params_struct_type *runtime_params_ptr
);

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_init_released_nvm

  DESCRIPTION    This routine initializes the parser for nvm file.

  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If initialized successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_init_released_nvm
(
  bt_qsoc_enum_type qsoc_type,
  bt_qsoc_runtime_params_struct_type *runtime_params_ptr
);

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_close_released_nvm

  DESCRIPTION    This closes the file handle to nvm and resets the state of parser
  
  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If closed successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_close_released_nvm
(
  void
);

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_get_next_released_nvm

  DESCRIPTION    This gives the next tag which needs to be sent to SoC.
  
  DEPENDENCIES   bt_qsoc_nvm_vs_init_released_nvm must be successful.

  PARAMETERS     soc_nvm_ptr (out): pointer to tag, if return value is TRUE.
                 parser_status(out): 0 = No Error, 1 = Parse Error

  RETURN VALUE   boolean: True: If tag extracted. 
                          False: If no more tags in nvm file

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_get_next_released_nvm
(
  OI_UINT8** soc_nvm_ptr,
  OI_UINT8* parser_error
);

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_parse_supported

  DESCRIPTION    This queries whether dynamic parsing is supported for a 
                 particular SoC at the given clock speed.

  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If initialized successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_parse_supported
(
  bt_qsoc_enum_type qsoc_type, 
  bt_qsoc_runtime_params_struct_type *runtime_params_ptr
);


/* Read the nvm buffer */
boolean bt_qsoc_read_nvm_tags_from_header
(
 bt_qsoc_enum_type qsoc_type,
 bt_soc_refclock_type bt_refclock_type
);


#ifdef __cplusplus
}
#endif 
#endif /* _BTQSOCNVM_H_ */



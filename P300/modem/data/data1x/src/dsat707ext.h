#ifndef DSAT707EXT_H
#define DSAT707EXT_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode extended commands ) processor.

EXTERNALIZED FUNCTIONS

dsat707_exec_cfg_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CFG="string" command. The string
  will be stored by the MT2 and sent to the base station prior
  to dialing.

dsat707_exec_cad_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CAD? command.
  This command queries analog or digital service.

dsat707_exec_cbc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CBC? command.
  This command queries battery charge and is read-only.

dsat707_exec_cbip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CBIP? command.
  This command returns the base station's IP address and is read-only.

dsat707_exec_cdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDR command.
  This command controls whether the extended format +CDR: intermediate
  result code is transmitted by the MT2.

dsat707_exec_cds_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDS command.
  This command controls the V.42bis data compression function on
  the Um interface.

dsat707_exec_cfc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CFC=<value> command.

dsat707_exec_cdv_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV command.

dsat707_exec_chv_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV=<value> command.

dsat707_exec_chv0_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV0=<value> command.

dsat707_exec_cmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CMIP? command.
  This command returns the mobile station IP address and is read only.

dsat707_exec_cmux_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CMUX=<n> command.
  This command selects multiplex option.

dsat707_exec_cqd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CQD=<n> command.
  This command sets the command state inactivity timer.

dsat707_exec_crc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CRC=<n> command.
  This command enables/disables cellular result codes.

dsat707_exec_crm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CRM=<n> command.
  This command sets the Rm interface protocol.

dsat707_exec_csq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CSQ? command.
  Query received signal quality. Returns the signal quality measure
  <SQM> and the frame error rate <FER>.

dsat707_exec_css_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CSS? command.
  This command returns the serving system and is read only.

dsat707_exec_cta_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CTA = <value> command.
  Set/Read/Test Um packet data inactivity timer.

dsat707_exec_cxt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CXT = <value> command.
  Set Cellular extension.

dsat707_exec_eb_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+EB command.
  Break handling in error control operation. This extended-format
  compound parameter is used to control the manner of V.42 operation
  on the PSTN link (if present in the IWF).

dsat707_exec_efcs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+EFCS command.
  This extended format numeric parameter controls the use of the
  32-bit frame check sequence option in V.42 on the PSTN link.

dsat707_exec_er_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ER command.
  Error control reporting. This extended format numeric parameter
  controls whether the extended format +ER: intermediate result
  code is transmitted from the IWF over the Um interface.

dsat707_exec_es_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ES command.
  Error control selection. This extended format compound parameter
  is used to control the manner of operation if the V.42 protocol
  on the PSTN link.

dsat707_exec_esr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ESR command.
  This extended format numeric parameter controls the use of
  selective repeat option in V.42 on the PSTN link.

dsat707_exec_etbm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ETBM command.
  This extended format compound parameter controls the handling of
  data remaining in IWF buffers upon service termination.

dsat707_exec_goi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+GOI command.
  Returns null.

dsat707_exec_ilrr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ILRR command.
  Local Rate Reporting. Mobile accepts only "OFF".

dsat707_exec_ma_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MA command.
  Modulation Automode control.

dsat707_exec_mr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MR command.
  Modulation Reporting control.

dsat707_exec_ms_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MS command.
  Modulation Selection. Controls the manner of operation of
  the modulation capabilities of the IWF.

dsat707_exec_mv18r_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18R command.
  V.18 reporting control.

dsat707_exec_mv18s_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18S command.
  V.18 selection.

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707

#include "dsat.h"
#include "dsati.h"

/*===========================================================================
  FUNCTION DSAT707_EXEC_CFG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CFG="string" command. The string
  will be stored by the MT2 and sent to the base station prior
  to dialing.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_cfg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cfg_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CAD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CAD? command.
  This command queries analog or digital service.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  Returns:
  0: if no service is available.
  1: if CDMA digital service is available.
  2: if TDMA digital service is available.
  3: if Analog service is available.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cad_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CBC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CBC? command.
  This command queries battery charge and is read-only.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  Returns <BCS>, <BCL>:
  BCS:
  0: MT2 powered by battery, BCL = status.
  1: MT2 connected to external power.
  2: Battery status not available.
  3: Recognized power fault; calls inhibited.
  BCL:
  0-100 - Remaining battery capacity is 0 to 100%
===========================================================================*/
dsat_result_enum_type dsat707_exec_cbc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cbc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CBIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CBIP? command.
  This command returns the base station's IP address and is read-only.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cbip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cbip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDR command.
  This command controls whether the extended format +CDR: intermediate
  result code is transmitted by the MT2.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cdr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDS command.
  This command controls the V.42bis data compression function on
  the Um interface.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cds_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cds_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CFC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CFC=<value> command.
  Um interface fax compression:
  0 - No compression.
  1 - V.42bis compression with parameters as set by the +CDS command.
  2 - Modifies the modified read compression.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cfc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cfc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV=<value> command.
  Initiate voice call.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cdv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CHV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV=<value> command.
  Hangup voice:
  0 - Hangup voice call.
  1-255 - reserved.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_chv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CHV0_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV0=<value> command.
  Hangup voice:
  0 - Hangup voice call.
  1-255 - reserved.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_chv0_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv0_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CMIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CMIP? command.
  This command returns the mobile station IP address and is read only.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cmip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cmip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CMUX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CMUX=<n> command.
  This command selects multiplex option:
  1 - Multiplex option 1.
  2 - Multiplex option 2.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cmux_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cmux_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CQD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CQD=<n> command.
  This command sets the command state inactivity timer:
  0 - Ignored.
  1-255 - Release call after 5x<n> seconds have elapsed without
  activity.  The default <n> shall be 10, corresponding to 50 seconds.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cqd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cqd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CRC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CRC=<n> command.
  Cellular result codes:
  0 - Disable Cellular Result Codes.
  1 - Enable Cellular Result Codes.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_crc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_crc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CRM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CRM=<n> command.
  Set Rm interface protocol:
  0 - Asynchronous data or fax.
  1 - Packet data service, Relay layer Rm interface.
  2 - Packet data service, Network layer Rm interface, PPP.
  3 - Packet data service, Network layer Rm interdace, SLIP.
  4 - STU-III service.
  5-127 - Reserved for future use.
  128-255 - Reserved for manufacturer specific use.
  Default is 0.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_crm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_crm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CSQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CSQ? command.
  Query received signal quality. Returns the signal quality measure <SQM>
  and the frame error rate <FER> as follows:
  <SQM>:
  0-31 - <SQM>
  99 - <SQM> not known or is detectable.
  <FER>:
  0 - <0.01%
  1 - 0.01 to < 0.1%
  2 - 0.1  to < 0.5%
  3 - 0.5  to < 1.0%
  4 - 1.0  to < 2.0%
  5 - 2.0  to < 4.0%
  6 - 4.0  to < 8.0%
  7 - >= 8.0%
  99 - <FER> is not known or is not detectable.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_csq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_csq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CSS? command.
  This command returns the serving system and is read only.  Returns
  <Band Class>, <Band>, <SID>
  Band Class:
  C - The Mobile Station is registered with a cellular system.
  P - The Mobile Station is registered with a PCS system.
  Band:
  CA - The Mobile Station is registeres with a cellular A-band system.
  CB - The Mobile Station is registeres with a cellular B-band system.
  PA - The Mobile Station is registeres with a PCS A-band system.
  PB - The Mobile Station is registeres with a PCS B-band system.
  PC - The Mobile Station is registeres with a PCS C-band system.
  PD - The Mobile Station is registeres with a PCS D-band system.
  PE - The Mobile Station is registeres with a PCS E-band system.
  PF - The Mobile Station is registeres with a PCS F-band system.
  Z - The Mobile Station is not registered.
  SID:
  0-16383 - The mobile station is registered with the system indicated.
  99999 - The mobile station is not registered.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_css_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_css_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CTA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CTA = <value> command.
  Set/Read/Test Um packet data inactivity timer.
  0 - Traffic channel not released during inactivity periods.
  1-255 - Release traffic channel after <value> 1-second intervals have
  elapsed since last sending or receiving RLP data frames on the Um
  interface.
  20 - (default)

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cta_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cta_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CXT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CXT = <value> command.
  Cellular extension.
  0 - Do not pass unrecognized commands to IWF.
  1 - When detectomg an unrecognized command, open transport layer
  connection and pass unrecognized commands to the IWF.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cxt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
);  /* dsat707_exec_cxt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_EB_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+EB command.
  Break handling in error control operation. This extended-format
  compound parameter is used to control the manner of V.42 operation
  on the PSTN link (if present in the IWF).

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_eb_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_eb_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_EFCS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+EFCS command.
  This extended format numeric parameter controls the use of the
  32-bit frame check sequence option in V.42 on the PSTN link.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_efcs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_efcs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ER_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ER command.
  Error control reporting. This extended format numeric parameter
  controls whether the extended format +ER: intermediate result
  code is transmitted from the IWF over the Um interface.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_er_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_er_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ES_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ES command.
  Error control selection. This extended format compound parameter
  is used to control the manner of operation if the V.42 protocol
  on the PSTN link.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_es_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_es_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ESR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ESR command.
  This extended format numeric parameter controls the use of
  selective repeat option in V.42 on the PSTN link.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_esr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_esr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ETBM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ETBM command.
  This extended format compound parameter controls the handling of
  data remaining in IWF buffers upon service termination.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_etbm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_etbm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GOI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+GOI command.
  Returns null.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_goi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_goi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ILRR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ILRR command.
  Local Rate Reporting. Mobile accepts only "OFF".

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ilrr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ilrr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MA command.
  Modulation Automode control.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ma_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ma_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MR command.
  Modulation Reporting control.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_mr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MS command.
  Modulation Selection. Controls the manner of operation of
  the modulation capabilities of the IWF.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ms_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MV18R_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18R command.
  V.18 reporting control.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_mv18r_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mv18r_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MV18S_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18S command.
  V.18 selection.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_mv18s_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mv18s_cmd */

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */

#endif /* DSAT707EXT_H */

/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific fax related
  AT commands.

EXTERNALIZED FUNCTIONS

dsat707_exec_faa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FAA command.
  Adaptive answer parameter.

dsat707_exec_fap_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FAP command.
  Addressing and polling capabilities parameter.

dsat707_exec_fbo_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBO command.
  Phase-C data bit order parameter.

dsat707_exec_fbs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBS command.
  Buffer size, read only parameter.

dsat707_exec_fbu_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBU command.
  HDLC frame reporting parameter.

dsat707_exec_fcc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCC command.
  DCE capabilities parameter.

dsat707_exec_fclass_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCLASS command.
  Service class selection parameter.

dsat707_exec_fcq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCQ command.
  Copy quality checking parameter.

dsat707_exec_fcr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCR command.
  Capability to receive parameter.

dsat707_exec_fct_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCT command.
  DTE Phase-C timeout parameter.

dsat707_exec_fdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FDR command.
  Receive Phase-C data.

dsat707_exec_fea_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FEA command.
  Phase-C received EOL alignment parameter.

dsat707_exec_ffc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FFC command.
  Format conversion parameter.

dsat707_exec_fhs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FHS command.
  Call-termination status parameter.

dsat707_exec_fie_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIE command.
  Procedure interrupt enable parameter.

dsat707_exec_fip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIP command.
  Initialize fax parameters.

dsat707_exec_fis_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIS command.
  Current session negotiation parameters.

dsat707_exec_fks_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FKS command.
  Terminate session.

dsat707_exec_fli_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLI command.
  Local id string parameter (TSI or CSI).

dsat707_exec_flo_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLO command.
  Flow control select parameter.

dsat707_exec_flp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLP command.
  Indicate document to poll parameter.

dsat707_exec_fms_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FMS command.
  Minimum Phase-C speed parameter.

dsat707_exec_fnr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FNR command.
  Negotiation message reporting control parameters.

dsat707_exec_fns_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FNS command.
  Non standard frame FIF parameter.

dsat707_exec_fpa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPA command.
  Selective polling address parameter.

dsat707_exec_fpi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPI command.
  Local polling ID string parameter.

dsat707_exec_fpp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPP command.
  Packet protocol control parameter.

dsat707_exec_fpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPR command.
  Serial port rate control parameter.

dsat707_exec_fps_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPS command.
  Page status parameter.

dsat707_exec_fpw_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPW command.
  Password parameter (sending or polling).

dsat707_exec_frq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FRQ command.
  Receive quality threshold parameters.

dsat707_exec_fry_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FRY command.
  ECM retry value parameter.

dsat707_exec_fsa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FSA command.
  Subaddress parameter.

dsat707_exec_fsp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FSP command.
  Request to poll parameter.



   Copyright (c) 1995-2009 by QUALCOMM, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Featurized code under FEATURE_ASYNC_DATA_NOOP.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707fax.h"
#include "dsat707faxctab.h"
#include "dsatparm.h"
#include "msg.h"
#include "err.h"

#define TX_BUFFER_SIZE        1572  /*  should be defined from psmgr.c  */
#define RX_BUFFER_SIZE        450   /*  should be defined from dsmgr.c  */

extern int dsat707_find_cmd_name (byte *s1);

/*===========================================================================
  FUNCTION DSAT707_EXEC_FAA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FAA command.
  Adaptive answer parameter.

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
dsat_result_enum_type dsat707_exec_faa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_faa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FAP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FAP command.
  Addressing and polling capabilities parameter.

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
dsat_result_enum_type dsat707_exec_fap_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fap_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FBO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBO command.
  Phase-C data bit order parameter.

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
dsat_result_enum_type dsat707_exec_fbo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fbo_cmd */

/*==========================================================================
  FUNCTION DSAT707_EXEC_FBS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBS command.
  Buffer size, read only parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fbs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*-------------------------------------------------------------------------*/
  rb_ptr = dsatutil_itoa (TX_BUFFER_SIZE,
                          rb_ptr,
                          16);
  *rb_ptr++ = ',';
  *rb_ptr++ = ' ';
  rb_ptr = dsatutil_itoa (RX_BUFFER_SIZE,
                          rb_ptr,
                          16);

  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
} /* dsat707_exec_fbs_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_FBU_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBU command.
  HDLC frame reporting parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fbu_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fbu_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_FCC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCC command.
  DCE capabilities parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fcc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  signed int index1 = -1; 
	signed int index2 = -1;
	int i;
  byte fcc [] = "+FCC";
  byte fis [] = "+FIS";
/*-------------------------------------------------------------------------*/
  index1 = dsat707_find_cmd_name (fcc);
  index2 = dsat707_find_cmd_name (fis);

  if ((index1 == -1) || (index2 == -1))
  {
    MSG_ERROR ("FCC/FIS not found in table", 0, 0, 0);
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
    return result;
  }

  if (dsat707_fax_table[index2].compound
      != dsat707_fax_table[index1].compound)
  {
    MSG_ERROR ("FCC FIS compound values don't match", 0, 0, 0);
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
    return result;
  }

  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);

  if ((result == DSAT_OK)
       && ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    for (i = 0; i < dsat707_fax_table[index1].compound; ++i)
    {
      dsat707_fis_val[i] = dsat707_fcc_val[i];
    }
  }

  return result;
} /* dsat707_exec_fcc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCLASS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCLASS command.
  Service class selection parameter.
  0 - Class 0.
  1 - Class 1 support unavailable.
  2 - Class 2.0 fax service.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fclass_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  static const byte fclass_range[] = "0,2.0";
  static const byte fclass_0[] = "0";
  static const byte fclass_2p0[] = "2.0";
  byte *rb_ptr = res_buff_ptr->data_ptr;

  const byte *s_ptr;
/*-------------------------------------------------------------------------*/

  if ((tab_entry->attrib & FCLASS2P0) && dsat_fclass_val != 2)
  {
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
    return result;
  }

  *rb_ptr = '\0';

  if (tok_ptr->op == (NA))            /*  Name only - set to default  */
  {
    * ((dsat_num_item_type *)tab_entry->val_ptr) =
        ((dflm_type *) tab_entry->def_lim_ptr)[0].default_v;
  }
  else if (tok_ptr->op == (NA|EQ|QU)) /*  TEST command  */
  {
    s_ptr = fclass_range;
    while ( (*rb_ptr++ = *s_ptr++) != 0)
        ;
  }

  else if (tok_ptr->op == (NA|QU))    /*  READ command  */
  {
    (void) dsatutil_fmt_num_val_ex (tab_entry, rb_ptr, RESPONSE);
  }

  else if (tok_ptr->op == (NA|EQ|AR)) /*  SET command   */
  {
    if (dsatutil_strcmp_ig_sp_case(tok_ptr->arg[0], fclass_0) == 0)
    {
      * ((dsat_num_item_type *)tab_entry->val_ptr) = 0;
    }
    else if (dsatutil_strcmp_ig_sp_case(tok_ptr->arg[0], fclass_2p0) == 0)
    {
      * ((dsat_num_item_type *)tab_entry->val_ptr) = 2;
    }
    else
    {
      result = DSAT_ERROR;
      DSATI_ERROR_MSG();
    }
  }
  else                                /*  Didn't make sense - ERROR */
  {
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
  }

  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;

} /* dsat707_exec_fclass_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCQ command.
  Copy quality checking parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fcq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fcq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCR command.
  Capability to receive parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fcr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fcr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCT command.
  DTE Phase-C timeout parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fct_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fct_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FDR command.
  Receive Phase-C data.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  dsat707_fdt_seen = TRUE;
  return result;
} /* dsat707_exec_fdr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FDT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FDR command.
  Receive Phase-C data.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fdt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  dsat707_fdt_seen = TRUE;
  return result;
} /* dsat707_exec_fdr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FEA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FEA command.
  Phase-C received EOL alignment parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fea_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fea_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FFC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FFC command.
  Format conversion parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_ffc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ffc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FHS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FHS command.
  Call-termination status parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fhs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fhs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIE command.
  Procedure interrupt enable parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fie_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fie_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIP command.
  Initialize fax parameters.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIS command.
  Current session negotiation parameters.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fis_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fis_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FKS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FKS command.
  Terminate session.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fks_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  return result;
} /* dsat707_exec_fks_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLI command.
  Local id string parameter (TSI or CSI).

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fli_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fli_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLO command.
  Flow control select parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_flo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_flo_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLP command.
  Indicate document to poll parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_flp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_flp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FMS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FMS command.
  Minimum Phase-C speed parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fms_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FNR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FNR command.
  Negotiation message reporting control parameters.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fnr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fnr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FNS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FNS command.
  Non standard frame FIF parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fns_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, res_buff_ptr);
  return result;
} /* dsat707_exec_fns_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPA command.
  Selective polling address parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fpa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fpa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPI command.
  Local polling ID string parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fpi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fpi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPP command.
  Packet protocol control parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fpp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fpp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPR command.
  Serial port rate control parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fpr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPS command.
  Page status parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fps_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fps_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPW_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPW command.
  Password parameter (sending or polling).

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fpw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fpw_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FRQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FRQ command.
  Receive quality threshold parameters.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_frq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_frq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FRY_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FRY command.
  ECM retry value parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fry_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fry_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FSA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FSA command.
  Subaddress parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fsa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_fsa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FSP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FSP command.
  Request to poll parameter.

  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 return result;
} /* dsat707_exec_fsp_cmd */

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */


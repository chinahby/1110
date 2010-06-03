/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific extended
  AT commands.

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
  and executes it. It executes AT+CDV=<value> command.

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

Copyright (c) 1995-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707ext.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/08   sn      Merged lint error fix.
10/29/07   sn      Replaced banned string APIs.
06/14/06   gr/sk   Merged changes to support no SCH
03/23/03   rsl/ak  Fixes for AT+CMUX
02/18/03   rsl     Changes relating to cm end_params.
04/03/02   rsl     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "bbver.h"
#include "cm.h"
#include "db.h"
#include "dsati.h"
#include "dsat707ext.h"
#include "dsat707extctab.h"
#include "dsatparm.h"
#include "hs.h"
#include "mobile.h"
#include "msg.h"
#include "snm.h"
#include "ps_iface.h"
#include "ps_tcp.h"
#include "ds3gmgrint.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "AEEstd.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
#define RF_FM_RSSI_SPAN             250
#define CDMA_RSSI_THRESH            125
#define CDMA_RSSI_SPAN              (CDMA_RSSI_THRESH - 75)

extern cm_call_id_type   voice_call_id;
extern voice_state_type  voice_state;

LOCAL dsat_result_enum_type ds_atcop_put_cmux_arg
(
  const dsati_cmd_type     *tab_entry,  /*  Entry in parse table  */
  tokens_struct_type *tok_ptr     /*  Results of parse      */
) ;

LOCAL atoi_enum_type ds_atcop_srch_list
(
  dsat_num_item_type *val_arg_ptr,
  const list_type l,
  byte * arg
) ;

dsat_dial_val_e_type ds_atcop_proc_dial_str
(
  byte * in_ptr,
  byte  * dial_ptr,
  const byte * accept_list_ptr,
  const byte * reject_list_ptr
) ;

int ds_atcop_strchr
(
  const byte *s,
  const byte c,
  const uint16 len
) ;


/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/
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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cfg_cmd */

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
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  db_items_value_type dbi;
  dsat_result_enum_type result = DSAT_OK;
  byte  *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
  db_get (DB_NO_SVC, &dbi);
  if (dbi.no_svc)
  {
    *rb_ptr++ = '0';
  }
  else
  {
    db_get (DB_DIGITAL, &dbi);
    *rb_ptr++ = (dbi.digital ? '1' : '3');
  }
  *rb_ptr= '\0';
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
 
  return result;
} /* dsat707_exec_cad_cmd */

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
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dword temp;
  db_items_value_type dbi;
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
#if ((TG == T_G) || (TG == T_O))          /*Report power status for Gemini*/
 *rb_ptr++ = (HS_EXT_PWR_ON() ? '1' : '0');
#else                                     /* Report external power status */
 *rb_ptr++ = (ui_extpwr_present() ? '1': '0');
#endif  /* (TG == T_G) */
 *rb_ptr++ = ',';
 db_get (DB_BATTERY_LEVEL, &dbi);
 temp = dbi.battery_level * 100;

 rb_ptr = dsatutil_itoa (temp / 255, rb_ptr, 10);
 res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
 return result;
} /* dsat707_exec_cbc_cmd */

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
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
 if (mode == DSAT_CMD || mode == DSAT_ONLINE_CMD)
 {
   rb_ptr = dsatutil_itoa((word)((Tcb.conn.remote.address >> 24) & 0xFF),
                          rb_ptr,
                          10);
   *rb_ptr++ = '.';

   rb_ptr = dsatutil_itoa((word)((Tcb.conn.remote.address >> 16) & 0xFF),
                          rb_ptr,
                          10
                         );
   *rb_ptr++ = '.';

   rb_ptr = dsatutil_itoa((word)((Tcb.conn.remote.address >>  8) & 0xFF),
                          rb_ptr,
                          10
                         );
   *rb_ptr++ = '.';

   rb_ptr = dsatutil_itoa((word)((Tcb.conn.remote.address) & 0xFF),
                          rb_ptr,
                          10
                         );
 }
 else
 {
   *rb_ptr = '\0';  /*  and reset pointer     */
   /*  if no result to ouput */
 }
 res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
 return(DSAT_OK);
} /* dsat707_exec_cbip_cmd */


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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cdr_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cds_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cfc_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_CDV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV=<value> command.

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
)
{
  const byte cdv_allowed    [] = "";
  const byte cdv_reject     [] = "TP!@";
  dsat_result_enum_type result = DSAT_OK;
  dsat_dial_val_e_type digit_mode = DSAT_DIAL_EMPTY;
  boolean                       call_status;
  cm_num_s_type                 dial_num;
  cm_num_s_type                 calling_number;
  cm_cdma_orig_params_s_type    call_orig_params;
  cm_orig_alpha_s_type          alpha_buf;
  uint16                        dial_str_len;
  /*------------------------------------------------------------------------*/
  if (tok_ptr->arg[0] == NULL) 
  {
	MSG_ERROR("tok_ptr->arg[0]= NULL", 0, 0, 0);
	return DSAT_ERROR;
  }
  
  digit_mode = ds_atcop_proc_dial_str (tok_ptr->arg[0],
									   tok_ptr->arg[0], 
									   cdv_allowed, 
									   cdv_reject);
	
  if ((digit_mode == DSAT_DIAL_EMPTY) || (digit_mode == DSAT_DIAL_ERROR))
  {
	MSG_ERROR("empty or error in dial string", 0, 0, 0);
	return DSAT_ERROR;
  }
  dial_str_len = (uint16)(strlen((char *)(tok_ptr->arg[0])));

  if (voice_state.state == IDLE_STATE)
	{
	  /* check number length */
	  if (dial_str_len > CM_MAX_NUMBER_CHARS)
      {
		return DSAT_ERROR;
	  }

	  /* set dial num */
	  memset(&dial_num, 0, sizeof(cm_num_s_type));
	  cm_num_init( &dial_num );
	  if (digit_mode == DSAT_DIAL_DIGIT) 
	  {
		/* dial_str_len is <= CM_MAX_NUMBER_CHARS (64), the cast is safe */
		cm_num_fill( &dial_num, tok_ptr->arg[0], (byte) dial_str_len, CM_DIGIT_MODE_4BIT_DTMF);
	  }
	  else if (digit_mode == DSAT_DIAL_ASCII_ABC) 
	  {
	    /* dial_str_len is <= CM_MAX_NUMBER_CHARS (64), the cast is safe */
	    cm_num_fill( &dial_num, tok_ptr->arg[0], (byte) dial_str_len, CM_DIGIT_MODE_4BIT_DTMF);
	  }

	  /* We don't have calling number or alpha, 
		  just init calling number and alpha */
	  memset(&calling_number, 0, sizeof(cm_num_s_type));
	  cm_num_init(&calling_number);
	  memset(&alpha_buf, 0, sizeof(cm_orig_alpha_s_type));
	  /* set mode specific parameters */
	  memset(&call_orig_params,CM_CALL_CMD_PARAM_DEFAULT_VALUE,sizeof(call_orig_params));
	  
	  call_status = cm_mm_call_cmd_orig(dsatcmif_call_cmd_cb_func,
										NULL,
										dsatcm_client_id,
										CM_CALL_TYPE_VOICE,
										CM_SRV_TYPE_AUTOMATIC,
										&calling_number,
										&dial_num,
										&alpha_buf,
										&call_orig_params, /* cdma orig params */
										NULL, /* GW CS parameters */
										NULL, /* GW PS parameters */
										&voice_call_id
										);

	  if (call_status)
   	  {
		  dsatvoice_go_to_dialing();
		/* the command callback function will be called */
		  MSG_LOW("call status successful", 0, 0, 0);
		  result = DSAT_ASYNC_CMD;
	  }
	  else
   	  {
		/* fail it, release atcop cmd processor */
		  MSG_ERROR("call status error", 0, 0, 0);
		  result = DSAT_ERROR;
	  }
	}
	else
	{
		result = DSAT_ERROR;
	}
return result;
} /* dsat707_exec_cdv_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  if (tok_ptr->op != (NA))   /*  Has optional 0 "argument" without   */
                             /*  operator. No operators allowed      */
  {                          /*  '0' is in parse table               */
    result = DSAT_ERROR;
    DSATI_ERROR_MSG();
  }
  else if (mode ==  DSAT_CMD || mode == DSAT_ONLINE_CMD)
  {
#ifdef  FEATURE_CM_MULTIMODE
		if(dsatvoice_is_voice())
		{
			if (dsatcmif_end_call(&voice_call_id))
		 	{
				/* the command callback function will be called */
				result = DSAT_ASYNC_CMD;
			}
			else
			{
				ERR("End call request failed",0,0,0);
				result = DSAT_ERROR;
			}
		}
		else
		{
			result = DSAT_OK;
		}
#else
		cm_call_cmd_end( NULL, NULL, dsi_cm_client);
#endif /*FEATURE_CM_MULTIMODE*/
	}
	return result;
} /* dsat707_exec_chv_cmd */

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
)
{
  return(dsat707_exec_chv_cmd (mode, tab_entry, tok_ptr, rb_ptr));
} /* dsat707_exec_chv0_cmd */


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
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
  if (mode == DSAT_CMD || mode == DSAT_ONLINE_CMD)
  {
    rb_ptr = dsatutil_itoa((word)((Tcb.conn.local.address >> 24)& 0xFF),
                            rb_ptr,
                            10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)((Tcb.conn.local.address >> 16)& 0xFF),
                           rb_ptr,
                           10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)((Tcb.conn.local.address >>  8)& 0xFF),
                           rb_ptr,
                           10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)(Tcb.conn.local.address & 0xFF),
                           rb_ptr,
                           10
                          );
  }
  else
  {
    *rb_ptr = '\0';  /*  and reset pointer     */
  }
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
} /* dsat707_exec_cmip_cmd */

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
)
{
#ifdef FEATURE_DS_IS707A_CMUX
  word                  fwd_mux;
  tokens_struct_type    tmp_token_struct;
  dsat_num_item_type    old_mux_val[2];
#endif /* FEATURE_DS_IS707A_CMUX */
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  
  /*------------------------------------------------------------------------
    Get the forward multiplex option.  Save the old value, as the below
    doesn't do bounds checking for MDR mux options
  ------------------------------------------------------------------------*/   
#ifdef FEATURE_DS_IS707A_CMUX
  old_mux_val[0] = dsat707_cmux_val[0];
  old_mux_val[1] = dsat707_cmux_val[1];
#endif /* FEATURE_DS_IS707A_CMUX */

  result = dsatparm_exec_param_cmd(
                                    mode,
                                    tab_entry,
                                    tok_ptr,
                                    rb_ptr
                                  );

#ifdef FEATURE_DS_IS707A_CMUX
  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
    memcpy(&tmp_token_struct,tok_ptr,sizeof(tokens_struct_type));
    result = ds_atcop_put_cmux_arg(tab_entry, &tmp_token_struct);

    if (result == DSAT_OK)
    {
      fwd_mux = dsat707_cmux_fwd_table[(dsat707_cmux_val[0])];

      /* Set the maximum number of forward supplementals as per the
           forward multiplex option specified by the user */
      if( fwd_mux & 0x01 )
      {
        /* Odd multiplex option => rate set 1 */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                              (byte)((fwd_mux-1)/2) );
      }
      else
      {
        /* Even multiplex option => rate set 2 */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                              (byte)((fwd_mux-2)/2) );
      }
    }
  }
  
  if (result != DSAT_OK)
  {
    /*-----------------------------------------------------------------------
      Error.  Restore the old values.
    -----------------------------------------------------------------------*/
    dsat707_cmux_val[0] = old_mux_val[0];
    dsat707_cmux_val[1] = old_mux_val[1];
  }
#endif /* FEATURE_DS_IS707A_CMUX_*/
  return result;
} /* dsat707_exec_cmux_cmd */
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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cqd_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_crc_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  
#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    If CRM is less than 2, check QCMIP to determine correct behaviour.
  -------------------------------------------------------------------------*/
  if (dsat707_crm_val < 2)
  {
    /*-----------------------------------------------------------------------
      If QCMIP is greater than 0, restore original value of CRM and return 
      an error.
    -----------------------------------------------------------------------*/
    if (dsat707_qcmip_val > 0)
    {
      /*---------------------------------------------------------------------
        If QCMIP is 2, CRM had to have taken on the value of 2.
      ---------------------------------------------------------------------*/
      dsat707_crm_val = 2;
      result = DSAT_ERROR; 
    }
  }
#endif /* FEATURE_DS_MOBILE_IP */
  return result;
} /* dsat707_exec_crm_cmd */


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
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dword temp;
  db_items_value_type dbi;
  byte *rb_ptr = res_buff_ptr->data_ptr;
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  db_get (DB_NO_SVC, &dbi);
  if(dbi.no_svc)
  {
    temp = 99;                /* Return signal quality unknown */
  }
  else
  {
    db_get (DB_DIGITAL, &dbi);
    if (dbi.digital)
    {
      db_get (DB_CDMA_RSSI, &dbi);
      temp = 31 * (CDMA_RSSI_THRESH - dbi.cdma_rssi);
      temp /= CDMA_RSSI_SPAN;
    }
    else
    {
      db_get (DB_RSSI, &dbi);
      temp = 31 * dbi.rssi;
      temp /= RF_FM_RSSI_SPAN;
    }
    temp = (temp > 31 ? 31 : temp); /*  Clip to 0 - 31 */
  }
  rb_ptr = dsatutil_itoa (temp, rb_ptr, 10);

  *rb_ptr++ = ',';
  *rb_ptr++ = ' ';
  *rb_ptr++ = '9'; /*  Always return frame error rate  */
  *rb_ptr++ = '9'; /*  as unknown for now.             */
  *rb_ptr = '\0';
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
} /* dsat707_exec_csq_cmd */

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
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  db_items_value_type dbi;
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
  *rb_ptr++ = '?';
  *rb_ptr++ = ',';
  *rb_ptr++ = ' ';
  db_get (DB_SID, &dbi);
  rb_ptr = dsatutil_itoa (dbi.sid, rb_ptr, 10);
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
} /* dsat707_exec_css_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cta_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cxt_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_eb_cmd */


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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_efcs_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_er_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_es_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_esr_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_etbm_cmd */

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
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK; 
  /*------------------------------------------------------------------------*/
  rb_ptr->data_ptr[0] = '\0';
  rb_ptr->used = (word)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_goi_cmd */

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
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ilrr_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ma_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_mr_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ms_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_mv18r_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 return result;
} /* dsat707_exec_mv18s_cmd */

/*===========================================================================

FUNCTION DS_ATCOP_PUT_CMUX_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for the
  IS-707-A AT+CMUX command, and, if they are correct, stores the value(s) in
  the parameter. This function also performs special processing and error
  checking for the following cases:
  (a) If <rev> is not specified, sets <rev> = <fwd>.
  (b) Checks that <fwd> & <rev> are either both odd or both even.

  If the range or number of values is out of bounds, or if the special check
  fails, an error will be returned.

DEPENDENCIES
  Note that this function assumes the that parse table entry for AT+CMUX is
  of type LIST, and is not of type HEX.

RETURN VALUE
  DSAT_ERROR if parsing failed.  Else DSAT_OK.

SIDE EFFECTS
  The global "ds_atcop_result" will be set as a result of the processing.

===========================================================================*/

LOCAL dsat_result_enum_type ds_atcop_put_cmux_arg
(
  const dsati_cmd_type     *tab_entry,      /*  Entry in parse table        */
  tokens_struct_type *tok_ptr              /*  Command tokens from parser  */
)
{
  dsat_num_item_type scr_val [MAX_ARG];
  byte flag [MAX_ARG];
  atoi_enum_type e;
  unsigned int j;

  ASSERT( (tab_entry->attrib & HEX) == 0 );
  ASSERT( (tab_entry->attrib & LIST) != 0 );

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > tab_entry->compound)
  {
    MSG_ERROR ("DSAT_ERROR", 0, 0, 0);
    return(DSAT_ERROR);
  }

  /* If <rev> is not specified i.e. the user issued AT+CMUX = <fwd>, then  */
  /* set <rev> equal to <fwd>.                                             */
  if( tok_ptr->args_found == 1 )
  {
    tok_ptr->arg[1] = tok_ptr->arg[0];
    tok_ptr->args_found++;
  }

  /* For each argument, check if the parameter value specified is valid.   */
  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    flag [j] = 0;
    
    e = ds_atcop_srch_list (&scr_val [j], 
                            ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v,
                            tok_ptr->arg[j]);
 
    if (e == ATOI_OUT_OF_RANGE)
    {
      return(DSAT_PARAM);
    }
    else if (e == ATOI_OK)
    {
      flag [j] = 1;
    }
    /* else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset, and   */
    /* the value is not stored.                                            */
  }

  /* If <rev> is specified but <fwd> is not specified i.e. the user issued */
  /* AT+CMUX = , <rev>  then set <fwd> equal to the previous value.        */
  if( (flag[0] == 0) && (flag[1] != 0) )
  {
    scr_val[0] = *((dsat_num_item_type *)tab_entry->val_ptr);
  }

  /* Check that <fwd> and <rev> are either both odd or both even. Odd mux  */
  /* options correspond to rate set 1 and even mux options corrrespond to  */
  /* rate set 2. The hardware does not support asymmetric rate sets on the */
  /* forward and reverse links.                                            */
  if( (flag[0] != 0) || (flag[1] != 0) )  /* At least one argument was present */
  {
    
#ifdef FEATURE_DS_IS707A_CMUX
    if( ((dsat707_cmux_fwd_table[(scr_val[0])] ^ dsat707_cmux_rev_table[(scr_val[1])]) & 0x01) != 0 )
    {
      /* Forward & reverse rate sets are not equal                         */
      return(DSAT_ERROR);
    }
    else
#endif /* #ifdef FEATURE_DS_IS707A_CMUX */
    {
      /* The rate set check is ok => write the parameter value(s).         */
      for (j = 0; j < tok_ptr->args_found; ++j)
      {
        if (flag[j])
        {
          *((dsat_num_item_type *)tab_entry->val_ptr + j) = scr_val[j];
        }
      }
    }
  }
  return(DSAT_OK);
} /*  ds_atcop_put_cmux_arg  */

/*===========================================================================
FUNCTION DS_ATCOP_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  -1 is returned if no match.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL atoi_enum_type ds_atcop_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const list_type l,
  byte * arg
)
{
  int i = 0;
  byte c;
  
  while (*l[i] != NULL)
    {
    if (dsatutil_strcmp_ig_sp_case ((byte *)l[i], arg) == 0)
      {
      *val_arg_ptr = i;
      return (ATOI_OK);
      }
    ++i;
    }

  i = 0; 
  
  while ( (c = arg[i++]) != '\0')
    {
    if (c != ' ')
      {
      return (ATOI_OUT_OF_RANGE);
      }
    }
  return (ATOI_NO_ARG);
}


/*===========================================================================

FUNCTION DS_ATCOP_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES

RETURN VALUE
  Returns enum values EMPTY, DIGITS, ASCII, or ERROR as appropriate.


SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type ds_atcop_proc_dial_str
(
  byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr,         /*  Output string, null terminated  */
  const byte * accept_list_ptr, /*  MUST be upper case, null term   */
  const byte * reject_list_ptr  /*  MUST be upper case, null term   */
)
{
  byte c, cu;
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;

  while ((c = *in_ptr++) != '\0')
  {
    cu = c;
    cu = UPCASE (cu);
    if ((c >= '0' && c <= '9') || c == '#' || c == '*')
    {
      *out_ptr++ = c;
      if (result == DSAT_DIAL_EMPTY)
      {
        result = DSAT_DIAL_DIGIT;
      }
    }
    else if (ds_atcop_strchr (accept_list_ptr, cu, std_strlen((char *)accept_list_ptr)) != 0)
    {
      *out_ptr++ = c;
      result = DSAT_DIAL_ASCII_ABC;
    }
    else if (ds_atcop_strchr (reject_list_ptr, cu, std_strlen((char *)reject_list_ptr)) != 0)
    {
      result = DSAT_DIAL_ERROR;
      break;
    }
  }
  *out_ptr = '\0';
  return result;
}

/*===========================================================================

FUNCTION DS_ATCOP_STRCHR

DESCRIPTION
  Looks for a byte in a string.

DEPENDENCIES

RETURN VALUE
  Return 1 if the byte is found, else 0.


SIDE EFFECTS

===========================================================================*/
int ds_atcop_strchr
(
  const byte *s,
  const byte c,
  const uint16 len
)
{
  uint16 i;

  for(i=0; i<len && s[i] != '\0';i++)
  {
    if (s[i] == c)
    {
      return 1;
    }
    }
  return 0;
}

/*=========================================================================*/
#endif /* FEATURE_DATA_IS707 */
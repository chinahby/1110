/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 vendor specific
  AT commands.

EXTERNALIZED FUNCTIONS
dsat707_set_mdr_val
  This function writes the MDR value to NV and does any required sync
  in the phone (such as enabling/disabling various service options)

dsat707_exec_qcotc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

dsat707_exec_qcprev_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.

dsat707_exec_qcmdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.

dsat707_exec_qcscrm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.

dsat707_exec_qctrtl_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.

dsat707_exec_qcchs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  
dsat707_exec_qcqnc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.

dsat707_exec_qcso_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.

dsat707_exec_qcvad_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.

dsat707_exec_qccav_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

dsat707_exec_qcqospri_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Set desired value of QOS non-assured priority adjustment.

Copyright (c) 1995-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vend.c_v   1.14   23 Jan 2003 16:40:12   randrew  $
$Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707vend.c#6 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Fixed featurization issue.
08/29/08   bs      Cleaned up featurization for $qcmdr.
02/07/08   sn      Featurized dsat707_exec_qcmdr_cmd() under FEATURE_IS95B_MDR 
                   and rearranged FEATURE_IS2000_R_SCH. Also removed 
                   FEATURE_SPECIAL_MDR.
10/24/07   sn      Added "dsrlp.h".
06/14/06   gr/sk   Merged changes to support NO SCH
05/23/06   squ     Featurized async data
12/25/03   rsl     Changes relating to Unrec. cmds not working.
05/05/03   jd      Removed dependencies on dsiface.h, AT$QCMDR and 
                   AT$QCQOSPRI are set directly
03/05/03   rsl     Fixed at$qcvad.
01/20/03   ar      Added dialstr_modifiers to ds707_async_dial_str_cb 
                   interface.
11/21/02   rsl     Fixed merge issues with at$qcchv.
11/14/02   jd      featurized dsat707mip.h inclusion, cleanup
10/15/02   ak      Added exec_qcchs(), which is for control/hold.
09/30/02   ar      Accomodate changed interface to proc_dial_string
09/24/02   atp     Added support for QOS. (1x Release-A feature).
09/10/02   ak      Fixed bugs in exec_qcscrm() where at$qcscrm? was failing.
09/03/02   ak      In set_mdr_val, no longer write to NV.
07/12/02   ak      Function dsat707_set_mdr_val made available.  Called by
                   dsiface_set_opt.
07/03/02   ak      For QCMDR, QCQNC, QCSCRM, QCTRTL, call dsatpar_exec_param_
                   _cmd, so that input is filtered by common function.
04/03/02   rsl     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#include "dsat707mipctab.h"
#include "dstaski.h"
#include "ds707.h"
#include "ds707_so_pkt.h"
#include "ds707_so_async.h"
#include "ds707_so_g3fax.h"
#include "dsatparm.h"
#include "mc.h"
#include "msg.h"
#include "dsatact.h"
#include "dsrlp.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
  #include "ds707_async_mgr.h"
#endif /*FEATURE_ASYNC_DATA_NOOP*/

#include "dsatvoice.h"

#include "db.h"
#include "cm.h"
#include "dsatcmif.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mip.h"
#endif /* FEATURE_DS_MOBILE_IP */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION DSAT707_SET_MDR_VAL

  DESCRIPTION
  This function sets the MDR value and synchronizes the available
  service options.
  
  This function can be called from UI, if UI decides to have an interface
  to change the value.
  
  DEPENDENCIES
  None.
  
  RETURN VALUE
  Boolean indicating success/fail.
  
  SIDE EFFECTS
  None.
===========================================================================*/
boolean dsat707_set_mdr_val
(
  unsigned int    qcmdr_val
)
{
  boolean             ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  dsat707_qcmdr_val = qcmdr_val;

#ifdef FEATURE_DS_MOBILE_IP
  ret_val = dsat707_qcmip_check_mdr_val();

  /*-------------------------------------------------------------------------
    If QCMIP is greater than 0, force MDR to 3.
  -------------------------------------------------------------------------*/
  if (ret_val == FALSE)
  {
    dsat707_qcmdr_val = DS_MDR_MODE_SO33_PREF;
  }
#endif /* FEATURE_DS_MOBILE_IP */

  /*-------------------------------------------------------------------------
    Re-synchronize available service options
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  return(ret_val);
} /* dsat707_set_mdr_val() */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCOTC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCOTC command.

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
dsat_result_enum_type dsat707_exec_qcotc_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,        /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                   /*  Place to put response       */
)
{
#ifndef FEATURE_ASYNC_DATA_NOOP
  dsat_result_enum_type result = DSAT_OK;
  boolean digit_mode;
  dsat_dial_val_e_type dialstr_res;
  dsat_dial_modifier_info_type  dialstr_modifiers;
  /*-----------------------------------------------------------------*/
  if (mode == DSAT_CMD)
  {
    dialstr_res = dsat_proc_dial_str (dsat707_qcotc_val, dsat707_qcotc_val);
    memset( (void*)&dialstr_modifiers, 0, sizeof(dialstr_modifiers) );
    
    switch (dialstr_res)
    {
      case DSAT_DIAL_DIGIT:
      case DSAT_DIAL_EMPTY:         /*  OK to originate without address (?) */
        digit_mode = 0;
        ds707_async_dial_str_cb ( dsat707_qcotc_val,
                                  digit_mode,
                                  &dialstr_modifiers );
        result = DSAT_NO_RSP;
        break;

      case DSAT_DIAL_ASCII:
        digit_mode = 1;
        ds707_async_dial_str_cb ( dsat707_qcotc_val,
                                  digit_mode,
                                  &dialstr_modifiers );
        result = DSAT_NO_RSP;
        break;

      case DSAT_DIAL_ERROR:
      default:
        result = DSAT_ERROR;
        break;
    }
  }
  return result;
#else /*FEATURE_ASYNC_DATA_NOOP*/
  return DSAT_ERROR;
#endif /*FEATURE_ASYNC_DATA_NOOP*/
} /* dsat707_exec_qcotc_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPREV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPREV command.
  Returns protocol revision in use.
  1 - JSTD008
  3 - IS_95A
  4 - IS_95B
  6 - IS2000

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
dsat_result_enum_type dsat707_exec_qcprev_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*-------------------------------------------------------------------------*/
  rb_ptr = dsatutil_itoa( mc_get_p_rev_in_use(), rb_ptr, 10);
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return(DSAT_OK);
} /* dsat707_exec_qcprev_cmd */


#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMDR command.
  Set Medium Data Rate.
  0: MDR service only
  1: MDR service if available
  2: LSPD only
  3: SO 33, if available

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
dsat_result_enum_type dsat707_exec_qcmdr_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = dsatparm_exec_param_cmd(
                                    mode,
                                    tab_entry,
                                    tok_ptr,
                                    rb_ptr
                                  );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    if ( !dsat707_set_mdr_val(dsat707_qcmdr_val) )
    {
      result = DSAT_ERROR;
      DSATI_ERROR_MSG();
    }
  }

  return result;
} /* dsat707_exec_qcmdr_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */

#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSCRM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSCRM command.
  Enable/Disable mobile from SCRMing.
  0 - Mobile never SCRMs
  1 - Mobile can SCRM as needed.

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
dsat_result_enum_type dsat707_exec_qcscrm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  result = dsatparm_exec_param_cmd(
                                  mode,
                                  tab_entry,
                                  tok_ptr,
                                  rb_ptr
                                );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
      Store the new SCRM Mode setting in NV
    -----------------------------------------------------------------------*/
    ds_nv_item.data_scrm_enabled = dsat707_qcscrm_val;
    (void) dsatutil_put_nv_item(NV_DATA_SCRM_ENABLED_I, &ds_nv_item);
  }
  return result;
} /* dsat707_exec_qcscrm_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTRTL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTRTL command.
  Enable/Dsiable R_SCH throttling.
  0 - Mobile never throttles R-SCH
  1 - Mobile can throttle R-SCH as needed.

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
dsat_result_enum_type dsat707_exec_qctrtl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  result = dsatparm_exec_param_cmd(
                                  mode,
                                  tab_entry,
                                  tok_ptr,
                                  rb_ptr
                                );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
      Store the new TRTL Mode setting in NV
    -----------------------------------------------------------------------*/
    ds_nv_item.data_trtl_enabled = dsat707_qctrtl_val;
    (void) dsatutil_put_nv_item(NV_DATA_TRTL_ENABLED_I, &ds_nv_item);
  }
  return result;
} /* dsat707_exec_qctrtl_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_IS2000_R_SCH */


#ifdef FEATURE_IS2000_CHS
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHS command.
  Enable/Dsiable mobile originated control/hold.
  0 - Disable mobile-originated control/hold.
  1-255 - Enable mobile-originated control-hold.  Mobile waits for this many
          idle (both tx and rx) 20-msec frames before asking BS for
          control/hold.  

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
dsat_result_enum_type dsat707_exec_qcchs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  result = dsatparm_exec_param_cmd(
                                  mode,
                                  tab_entry,
                                  tok_ptr,
                                  rb_ptr
                                );

  return result;
} /* dsat707_exec_qcchs_cmd */
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_DS_QNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQNC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQNC command.
  Enable/Dsiable Quick Net Connect.
  0 - Disable QNC capability
  1 - Enable QNC capability

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
dsat_result_enum_type dsat707_exec_qcqnc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  result = dsatparm_exec_param_cmd(
                                  mode,
                                  tab_entry,
                                  tok_ptr,
                                  rb_ptr
                                );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
      Store the new TRTL Mode setting in NV
    -----------------------------------------------------------------------*/
    ds_nv_item.data_qnc_enabled = dsat707_qcqnc_val;
    (void) dsatutil_put_nv_item( NV_DATA_QNC_ENABLED_I, &ds_nv_item );

    /*-----------------------------------------------------------------------
     Set service options according to values read from NV
    -----------------------------------------------------------------------*/
     ds707_so_pkt_recal();
  }
  return result;
} /* dsat707_exec_qcqnc_cmd */
#endif /* FEATURE_DS_QNC */


#ifdef FEATURE_DS_CHOOSE_SO
#ifdef FEATURE_DS_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCSO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSO command.
  Set Data Service Option number.
  0 - pre-707 numbers
  1 - proprietary 707 numbers
  2 - IS-707 SO numbers

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
dsat_result_enum_type dsat707_exec_qcso_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:             */
  const dsati_cmd_type *tab_entry,        /*  Ptr to cmd in parse table    */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser   */
  dsm_item_type *rb_ptr                   /*  Place to put response        */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = dsatparm_exec_param_cmd(
                                  mode,
                                  tab_entry,
                                  tok_ptr,
                                  rb_ptr
                                );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
      Store the new SO Mode setting in NV
    -----------------------------------------------------------------------*/
    ds_nv_item.data_so_set = dsat707_qcso_val;
    (void) dsatutil_put_nv_item( NV_DATA_SO_SET_I, &ds_nv_item );

    /*-----------------------------------------------------------------------
      Enable/Disable the appropriate data service options in SNM based
      on the service option set now in use
    -----------------------------------------------------------------------*/
    ds707_so_pkt_recal();

#ifndef FEATURE_ASYNC_DATA_NOOP
    ds707_so_async_recal();
    ds707_so_g3fax_recal();
#endif /* FEATURE_ASYNC_DATA_NOOP */
  }

  return result;
} /* dsat707_exec_qcso_cmd */
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_DS_CHOOSE_SO */



/*===========================================================================
  FUNCTION DSAT707_EXEC_QCVAD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVAD command.
  Prearrangement setting. Respond to page message that has a voice service
  option with a page response that has a data service option.
  0 - Off
  1 - Fax for next call
  2 - Fax for all calls
  3 - Async. for next call
  4 - Async. for all calls

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
dsat_result_enum_type dsat707_exec_qcvad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
	db_items_value_type   dbi;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

	if (tok_ptr->op == (NA | QU)) 
  {
    db_get (DB_VOICE_AS_DATA, &dbi);
		dsat707_qcvad_val = dbi.voice_as_data;
	}

	result = dsatparm_exec_param_cmd
		(
			mode,
			tab_entry,
			tok_ptr,
			rb_ptr
			);

	if ((result == DSAT_OK) &&
			((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
     The call manager does the work of changing the VAD setting.
     Note, the cast of the qcvad_val setting is OK because the
     CM type gets its values from the DB type, qcvad_val gets
     its value from the DB.
    -----------------------------------------------------------------------*/
    cm_ph_cmd_answer_voice(NULL,
                           NULL,
                           dsatcm_client_id,
                           (cm_answer_voice_e_type)dsat707_qcvad_val,
                           600
                          );
  }
  return result;
} /* dsat707_exec_qcvad_cmd */



/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCAV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCAV command.
  Answer incoming voice call.

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
dsat_result_enum_type dsat707_exec_qccav_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  /*-----------------------------------------------------------------*/
  if (dsatvoice_is_voice())
  {
    result = dsatvoice_answer_cmd();
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qccav_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCCHV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCCHV command.
  Hangs up incoming voice call.

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
dsat_result_enum_type dsat707_exec_qcchv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  /*-----------------------------------------------------------------*/
  if(dsatvoice_is_voice())
  {
    result = dsatvoice_end_voice_call();
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_qcchv_cmd */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCQOSPRI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCQOSPRI command.
  Sets desired value of QOS non-assured priority adjustment.
  (Can range from 0 to DS707_QOS_DESIRED_PRI_MAX (0xF).)

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
dsat_result_enum_type dsat707_exec_qcqospri_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  result = dsatparm_exec_param_cmd(
                                    mode,
                                    tab_entry,
                                    tok_ptr,
                                    rb_ptr
                                  );

  if ((result == DSAT_OK) &&
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  {
    /*-----------------------------------------------------------------------
      assume range checking is done beforehand
      dsat707_qcqospri_val must be between
      DS707_QOS_DESIRED_PRI_MIN .. DS707_QOS_DESIRED_PRI_MAX
    -----------------------------------------------------------------------*/
    dsrlp_set_desired_qos_non_assur_pri( (byte) dsat707_qcqospri_val );
  }

  return result;

} /* dsat707_exec_qcqospri_cmd */
#endif /* FEATURE_IS2000_REL_A    */

#endif /* FEATURE_DATA_IS707 */



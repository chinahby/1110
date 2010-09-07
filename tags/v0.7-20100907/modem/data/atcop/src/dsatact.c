/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                ( C O M M O N   A C T I O N  
                  C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common 
  (across all modes) action commands.

EXTERNALIZED FUNCTIONS
  dsatact_exec_ampf_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (AT&F)
    command. AT&F is used to set to factory defined configuration. 

  dsatact_exec_atz_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (ATZ) 
    command. ATZ is used to reset to default configuration. 

  dsatact_exec_atd_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATD command.
    ATD command is used to dial a call (ASYNC, PACKET, VOICE) using AT 
    commands

  dsatact_exec_ata_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATA command.
    ATA command is used to answer a incoming call using AT commands

  dsatact_exec_ath_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATH command.
    ATH command is used to hangupall using AT commands.
    Will not end a voice call.

  dsatact_exec_cgmi_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMI/+GMI command.
    This command returns manufacturer information.

  dsatact_exec_cgmm_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMM/+GMM command.
    This command returns model information.

  dsatact_exec_cgmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMR/+GMR command.
    This command returns revision information.

  dsatact_exec_cgsn_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns serial number information.

  dsatact_exec_gcap_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns complete capabilities list information.

  dsatact_call_abort_handler
    This function does nothing but return a result to indicate the
    call should be aborted.

  dsatact_exec_ato_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATO command.
    ATO command is used to return from command to online data mode.
    
  dsatact_exec_ati_cmd
    This function takes the result from the command line parser
    and executes it. It executes I command.
    This command returns identification information.

  dsatact_exec_cimi_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CIMI command.
    This command returns the international mobile subscriber identy (IMSI).

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2009 by QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatact.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   bs      Added support for 1X AT Phonebook commands.
10/29/07   sn      Replaced banned string APIs.
05/23/06   squ     Featurized async data
12/25/03   rsl     Changes relating to Unrec. cmds not working.
05/01/03   sb      new return code (DSAT_ATH_CMD) when we are processing 
                   ATH command. Mainly to indicate that it is a delayed 
                   result code (from DSMGR) and we need to process the 
                   command line after we are doen executing ATH command.
04/23/03   dgy     Merged GSM and WCDMA CS Data mode-specific handlers.
04/18/03   vsk     changed to use the dsumts_rmsm interface rather than 
                   gen_rmsm 
03/12/03   ar      Scan entire dial string for I/G modifiers
02/26/03   wx      Put dialed number into last dial ph book.
02/25/03   rsl     Fixed typo., use mode instead of current_mode.
02/25/03   rsl     Execute atd only when in AT_CMD mode, otherwise return ERROR.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
02/03/03   ar      Add +ES in +GCAP list in WCDMA mode.
01/24/03   ar      Add dial string I/G/CID modifier support.
01/22/03   ar      Changed +GCAP to display +FCLASS only in GSM mode.
01/16/03   ar      AT&F now calls dsatetsipkt_reset_pdp_context_profiles()
11/22/02   sb      ATH Should not end Voice call.
11/14/02   jd      Simplified/optimized strcmp_pkt_dial_str, cleanup
11/11/02   sb      Restricted +GCAP command based on the current mode.
11/08/02   sb      Concurrent call changes.
11/08/02   sb      Return NO_CARRIER when Answer Call Back Handler is NULL.
11/07/02   ar      Incorporated code review comments
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/29/02   sb      Fixed a merge issue that occured due to Voice changes
10/28/02   sb      Opened Voice call related handlers (moved changes to APIs)
10/23/02   ar      Added FEATURE_DATA_MM wrapper around packet rmsm items 
10/09/02   sb      Populated the dial table (GSM CS, WCDMA CS, WCDMA PS )
09/18/02   ar      Added/adjusted FEATURE_DATA_WCDMA_PS wrapper for GSM build
09/10/02   ak      Compilation for 1x changes.
09/10/02   atp     Added 707 async & pkt dial cb functions to dial table. 
09/10/02   ar      Wrap GSDI interface change under FEATURE_MMGSDI_<mode>
09/09/02   ar      Move dial character validation to utility function
                   dsatutil_validate_dial_char().
09/06/02   sb      Fix atd_cmd handler bugs.
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
07/22/02   ak      made Proc_dial_string non-LOCAL
06/05/02   ar      Relocated dsatact_enter_online_data_mode from DS3GMGR
04/17/02   rc      Added stack_type as parameter to function call
                   ds3g_initiate_call().
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "AEEstd.h"

#include "bbver.h"
#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvoice.h"
#include "diag.h"
#include "ds3gmgr.h"
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "mobile.h"
#include "msg.h"

#ifdef FEATURE_DATA_MM
#include "ds3gsiolib.h"
#else
#include "ds3gsioif.h"
#endif /* FEATURE_DATA_MM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_DATA_IS707
#include "dsat707util.h"
#include "dsat707extctab.h"
#include "ds707_rmsm.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#include "ds707_async_mgr.h"
#endif /*FEATURE_ASYNC_DATA_NOOP*/

#include "snm.h"
#include "cai.h"
#endif

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*-------------------------------------
  Dial string for Packet calls.
---------------------------------------*/
LOCAL const byte dsat_pkt_orig_str[] = "*98#";

/*-------------------------------------
  Local Variables
---------------------------------------*/
/* ATCOP and PS split up support */

/* Local variable that is used to retain the */
/* call back function pointers for ATD, ATA  */
/* ATH and abort handlers for ATA and ATD    */
LOCAL dsat_dial_string_type dial_string_handler;

/* ATD Dial Table */
/* This table has different call handlers in different mode        */
/* Row is for different mode ( GSM, WCDMA, CDMA in that order)     */
/* Column is for different calls ( Circuit, Packet in that order ) */
/* Example:                                                        */
/*  { gsm_ckt_f,   gsm_pkt_f   },                                  */
/*  { wcdma_ckt_f, wcdma_pkt_f },                                  */
/*  { cdma_ckt_f,  cdma_pkt_f  }                                   */
/*                                                                 */ 
/* gsm_ckt_f will have the following prototype                     */
/* dsat_call_abort_cb_type gsm_ckt_f                               */
/* (                                                               */
/*  const byte            *dial_string,                            */
/*  boolean                digit_mode                              */
/* );                                                              */

dsat_dial_type dsati_dial_table[DSAT_MODE_MAX][DSAT_MAX_DIAL_TYPE]
= 
{

  { 
    /* GSM Circuit Call handler */
    #if defined(FEATURE_DATA_MM) && defined(FEATURE_DATA_GCSD)
    ds_ucsd_dial_string_handler
    #else
    NULL
    #endif 
    ,  
    /* GSM Packet Call handler   */
    #if (defined(FEATURE_GSM_GPRS) && defined(FEATURE_DATA_MM))
#error code not present
    #else
    NULL   
    #endif /* defined(FEATURE_GSM_GPRS) && defined(FEATURE_DATA_MM)*/
  },  
  {
    /* WCDMA Circuit Call handler */   
    #ifdef FEATURE_DATA_WCDMA_CS
#error code not present
    #else
    NULL
    #endif
    ,
    /* WCDMA Packet Call handler */
    #if (defined(FEATURE_DATA_WCDMA_PS) && defined(FEATURE_DATA_MM)) 
#error code not present
    #else
    NULL 
    #endif

  }, 
  { 
    /* CDMA Circuit Call handler */
    #if defined(FEATURE_DATA_IS707) && !defined(FEATURE_ASYNC_DATA_NOOP)
    ds707_async_dial_str_cb
    #else
    NULL
	#endif  /*defined(FEATURE_DATA_IS707) && !defined(FEATURE_ASYNC_DATA_NOOP)*/
    ,
    /*  CDMA Packet Call handler  */
    #if defined(FEATURE_DATA_IS707)
    ds707_rmsm_dial_str_cb
    #else
    NULL
    #endif
  }  
};

/*-------------------------------------
  Regional Variables
---------------------------------------*/
/* indicates the current abort handler */
dsat_call_abort_cb_type dsat_curr_abort_handler = NULL;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL void enter_online_data_mode
(
  boolean flush_tx      /* Flag to flush SIO TX fifo prior to switch */
);

boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type             cur_mode
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */


/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION DSATACT_EXEC_AMPF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&F command. 
  AT&F is used to set to factory defined configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ampf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
#ifdef FEATURE_DATA_IS707
  result = dsatact_exec_atz_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
#else
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {

    /* Initialize all AT parameters to their default values */
    dsatcmdp_init_config ();

#ifdef FEATURE_DATA_MM
    (void) ds3g_siolib_change_baud_rate((sio_bitrate_type) dsat_ipr_val, FALSE );
#else
    (void) ds3g_sio_change_baud_rate((sio_bitrate_type) dsat_ipr_val, FALSE );
#endif /* FEATURE_DATA_MM */

#if defined(FEATURE_DIAG_DS_SIO_COUPLING)
    /* Set the diag port with the new/reset baud rate */
    diag_record_dm_baudrate ( (sio_bitrate_type) dsat_qcdmr_val );
#endif /* defined(FEATURE_DIAG_DS_SIO_COUPLING) */

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
  }
  else 
  {
    result = DSAT_ERROR;
  }
#endif
  return result;
}/* dsatact_exec_ampf_cmd */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATZ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes ATZ, command. 
  ATZ is used to reset to default configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atz_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type retained_ipr_val;
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DS_IS707A_CMUX
  word  fwd_mux;  /* Forward multiplex option */
#endif /* FEATURE_DS_IS707A_CMUX */

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {
    if ( mode != DSAT_ONLINE_DATA )
    {
      /* Data Port's Baud Rate should not be changed in ATZ command */
      /* Retain the IPR value and restore it later for ATZ command  */
      retained_ipr_val = dsat_ipr_val;

      /* Initialize all AT parameters to their default values */
      dsatcmdp_init_config ();

      dsat_ipr_val = retained_ipr_val;

#if defined(FEATURE_DIAG_DS_SIO_COUPLING)
      /* Set the diag port with the new/reset baud rate */
      diag_record_dm_baudrate ( (sio_bitrate_type) dsat_qcdmr_val );
#endif /* defined(FEATURE_DIAG_DS_SIO_COUPLING) */

#ifdef FEATURE_DS_IS707A_CMUX
      /* Get the forward multiplex option */
      fwd_mux = dsat707_cmux_fwd_table[(dsat707_cmux_val[0])];

      /* Set the maximum multiplex option for service option 22 */
      if( fwd_mux & 0x01 )
      {
        /* Set the maximum number of forward supplementals as per the
           AT+CMUX default */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((fwd_mux-1)/2) );
      }
      else
      {
        /* Set the maximum number of forward supplementals as per the
                     CAI default */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((CAI_MAX_USABLE_MUX_RS1-1)/2) );
      }

      /* Set the maximum multiplex option for service option 25 */
      if( !(fwd_mux & 0x01) )
      {
        /* Set the maximum number of forward supplementals as per the
         AT+CMUX default */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((fwd_mux-2)/2) );
      }
      else
      {
      /* Set the maximum number of forward supplementals as per the
                     CAI default */
        snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((CAI_MAX_USABLE_MUX_RS2-2)/2) );
      }
#endif  /* FEATURE_DS_IS707A_CMUX */

    }

    if (  dial_string_handler.atz_cb != NULL )
    {
      dial_string_handler.atz_cb();
    }    
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_atz_cmd */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATD command.
  ATD command is used to dial a call (ASYNC, PACKET, VOICE, Direct Dial) 
  using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR          : if there was any problem in executing the command
    DSAT_NO_RSP         : if ds3g_initiate_call() is called.
    DSAT_ASYNC_CMD      : if voice call is made.

SIDE EFFECTS
  Data dial handler or voice call handler invoked to init a call.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type         result;

  dsati_dial_str_type           dial_str;
  dsati_mode_e_type             current_mode;
  dsat_dial_str_enum_type       call;
  dsat_dial_modifier_info_type  modifier_info;

/*------------------------------------------------------------------*/

  current_mode = dsatcmdp_get_current_mode();

  if (mode == DSAT_CMD) 
  {
  /* process the dial string tok_ptr->arg[0],
     Get the dial number's attribute in dial_str 
  */
    if (!proc_dial_str(tok_ptr, &dial_str, current_mode))
    {
    /* illegal dial string */
      return DSAT_ERROR;
    }

    /* error for empty dial number */
    if (dial_str.len == 0)
    {
      MSG_HIGH("Dial number empty.",0,0,0);
      return DSAT_ERROR;
    }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

    /* dial through the dial table, depending on mode, CS or PKT call
       */
    if ( dial_str.attrib & DIAL_ATTRIB_PKT )
    {
      call = DSAT_PKT_DIAL_TYPE;
    }
    else
    {
      call = DSAT_DEFAULT_DIAL_TYPE;
    }

    memset(&modifier_info, 0, sizeof(dsat_dial_modifier_info_type));
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

    if ( dsati_dial_table[current_mode][call] == NULL )
    {
      MSG_HIGH("unregistered call handler",0,0,0);
      result = DSAT_ERROR;
    }
    else
    {
      dsat_curr_abort_handler =
        dsati_dial_table[current_mode][call](dial_str.num,
                                     (dial_str.attrib & DIAL_ATTRIB_ASCII),
                                               &modifier_info);
#ifdef FEATURE_ETSI_PBM
      if ((current_mode == DSAT_MODE_GSM) ||
          (current_mode == DSAT_MODE_WCDMA))
      {
        dsatme_ld_init_record((char *) dial_str.num, -1, NULL);
      }
#endif /* FEATURE_ETSI_PBM */

      result = DSAT_NO_RSP;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_atd_cmd */


/*===========================================================================

FUNCTION DSATACT_EXEC_ATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATA command.
  ATA command is used to answer a incoming call using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  Calls ds3g_answer_call() function to answer a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  if ( mode == DSAT_CMD )
  {
#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined (FEATURE_GSM) */
    {
      if ( dial_string_handler.answer_cb == NULL )
      {
        result = DSAT_NO_CARRIER;
        MSG_HIGH("unregistered incoming_cb handler", 0,0,0);
      }
      else
      {
        result = DSAT_NO_RSP;
        dsat_curr_abort_handler = dial_string_handler.answer_cb ();

        if ( dsat_curr_abort_handler == NULL )
        {
          MSG_HIGH("unregistered ATA Abort handler",0,0,0);
        }
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_ata_cmd () */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATH command.
  ATH command is used to hangup both data and voice calls
  using AT commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ATH_CMD : if hangup pointer is invoked.

SIDE EFFECTS
  Calls ds3g_hangup_call() function to terminate a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ath_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ((tok_ptr->op == (NA|EQ|AR)) && ( *tok_ptr->arg[0] != '0'))
    {
      /* check to see if the argument is valid */
      /* for ATH only 0 is defined */
      /* any other value would cause an error */
      MSG_LOW("ATH parameter not supported: %s",*tok_ptr->arg,0,0);
      return DSAT_ERROR;
    }

    if ( dial_string_handler.hangup_cb == NULL )
    {
      result = DSAT_OK;
      MSG_HIGH("unregistered hangup call handler",0,0,0);
    }
    else
    {
      dial_string_handler.hangup_cb ();
      result = DSAT_ATH_CMD;
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_ath_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes O command.
  This command causes the DCE to return to online data state.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  None  

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ato_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ((tok_ptr->op == (NA|EQ|AR)) && ( *tok_ptr->arg[0] != '0'))
    {
      /* check to see if the argument is valid  */
      /* for ATO only 0 is defined at this time */
      /* any other value would cause an error   */
      MSG_LOW("ATO parameter not supported: %s",*tok_ptr->arg,0,0);
      return DSAT_ERROR;
    }

    /* Check mode for correct state */
    if ( DSAT_ONLINE_CMD == mode )
    {
      /* Currently in ONLINE-COMMAND mode */
      enter_online_data_mode(TRUE);
      result = DSAT_CONNECT;
    }
    else
    {
      /* Currently in COMMAND mode */
      result = DSAT_NO_CARRIER;
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatact_exec_ato_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_GMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMI/+CGMI command.
  This command returns manufacturer information.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  static char info[] = "QUALCOMM INCORPORATED";

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    (void) std_strlcpy((char*)res_buff_ptr->data_ptr, 
                       (const char*)info, 
                       res_buff_ptr->size);
    res_buff_ptr->used += (word)strlen((const char*)info);
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_gmi_cmd () */


/*===========================================================================

FUNCTION DSATACT_EXEC_GMM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMM/+CGMM command.
  This command returns model information.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmm_cmd
(
 dsat_mode_enum_type mode,               /*  AT command mode:            */
 const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
 const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
 dsm_item_type *res_buff_ptr             /*  Place to put response       */
 )
{
  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type nv_status;
  static char info[30];

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /* Query NV for model identification */
    nv_status = dsatutil_get_nv_item(NV_MOB_MODEL_I, &ds_nv_item);
    if( nv_status == NV_DONE_S )
    {
      (void) dsatutil_itoa(ds_nv_item.mob_model, (unsigned char*)info, 10);
      (void) std_strlcpy((char*)res_buff_ptr->data_ptr,
                         (const char*)info,
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }
    else
    {
      MSG_LOW("Model not found in NV", 0,0,0);

      /* Use global model variable instead */
      (void) dsatutil_itoa(mob_model, (unsigned char*)info, 10);
      (void) std_strlcpy((char*)res_buff_ptr->data_ptr,
                         (const char*)info,
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }
  }

  
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}  /* dsatact_exec_gmm_cmd () */
  


/*===========================================================================

FUNCTION DSATACT_EXEC_GMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CGMR/+GMR command.
  This command returns revision information.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmr_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    res_buff_ptr->used =
      (word)std_strlprintf ((char*)res_buff_ptr->data_ptr, res_buff_ptr->size, 
                            "%s  %d  [%s %s]", mob_sw_rev,
                            boot_block_ver[0], rel_date, rel_time);
  }
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatact_exec_gmr_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_GSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN/+CGSN command.
  This command returns serial number information.  In ETSI mode,
  the IMEI value is reported.  Otherwise, the ESN is reported.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gsn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type nv_status;
  static byte info[32];

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
    {
      /* ESN Reporting */
      nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
      if( nv_status == NV_DONE_S )
      {
        /* Convert the 32 bit unsigned number to hex */
        (void)dsatutil_itoa(ds_nv_item.esn.esn, info, 16);

        res_buff_ptr->used = (word)std_strlprintf((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size, "0x%s", 
                                                  info);
        result = DSAT_OK;
      }
      else
      {
        MSG_HIGH("Problem reading ESN from NV", 0,0,0);
        result = DSAT_ERROR;
      }
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatact_exec_gsn_cmd() */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



/*===========================================================================

FUNCTION DSATACT_EXEC_GCAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GCAP command.
  This command returns complete capabilities list information.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gcap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  #define SIZE_OF_INFO 65   /* Size of info array */
  dsat_result_enum_type result = DSAT_OK;
  static char info[48]; 
  word cnt = 0;
  const char cap_list_2000 [] = "+CIS707-A, +MS, +ES, +DS, +FCLASS";

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /*--------------------------------------
    Setup response based on operating mode
    ----------------------------------------*/
    /* Initialize response */
    (void) std_strlcpy((char*)info, (char*)"+GCAP: ", SIZE_OF_INFO);

    if (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode())
    {
      (void)std_strlprintf (info, SIZE_OF_INFO, "%s%s%s", info, 
                           (!cnt++) ? "" : ",",
                           "+CGSM");
    }
    else if (CDMA_CMD_MODE == dsatcmdp_get_operating_cmd_mode())
    {
      (void) std_strlcat(info, cap_list_2000, SIZE_OF_INFO);
    }

#ifdef FEATURE_DATA_GCSD_FAX
#error code not present
#endif /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#if defined(FEATURE_DSAT_V80) && defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_DSAT_V80) && defined(FEATURE_WCDMA) */

    (void) std_strlcpy((char*)res_buff_ptr->data_ptr, (const char*)info, 
                       res_buff_ptr->size);
    res_buff_ptr->used += (word)strlen((const char*)info);
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_gcap_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes I command.
  This command returns identification information.

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
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ati_cmd  
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  
  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /*-------------------------------------------------------
      Allocate a temporary buffer to hold composite response
    ---------------------------------------------------------*/
    dsm_item_type * temp_buff_ptr;
    if( NULL ==
        (temp_buff_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )))
    {
      ERR_FATAL( "Running out of items", 0, 0, 0);
    }

    /* Manufacturer information */
    if (DSAT_OK == dsatact_exec_gmi_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                                 temp_buff_ptr->size, "%s: %s",
                                                 "Manufacturer",
                                                 res_buff_ptr->data_ptr);
    }

    /* Model information */
    if (DSAT_OK == dsatact_exec_gmm_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                                 temp_buff_ptr->size, 
                                                 "%s\n%s: %s",
                                                 temp_buff_ptr->data_ptr,
                                                 "Model",
                                                 res_buff_ptr->data_ptr);
    }

    /* Revision information */
    if (DSAT_OK == dsatact_exec_gmr_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                                 temp_buff_ptr->size, 
                                                 "%s\n%s: %s",
                                                 temp_buff_ptr->data_ptr,
                                                 "Revision",
                                                 res_buff_ptr->data_ptr);
    }

    /* ESN/IMEI information */
    temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                               temp_buff_ptr->size, 
                                               "%s\n%s: ",
                                               temp_buff_ptr->data_ptr,
                                               (ETSI_CMD_MODE == 
                                               dsatcmdp_get_operating_cmd_mode()) ? 
                                               "IMEI" : "ESN" );
    if (DSAT_OK == dsatact_exec_gsn_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                                 temp_buff_ptr->size, 
                                                 "%s%s",
                                                 temp_buff_ptr->data_ptr,
                                                 res_buff_ptr->data_ptr);
    }

    /* Capabilities information (heading already in response buffer) */
    if (DSAT_OK == dsatact_exec_gcap_cmd (mode, parse_table, 
                                          tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)std_strlprintf((char*)temp_buff_ptr->data_ptr,
                                                 temp_buff_ptr->size, 
                                                 "%s\n%s",
                                                 temp_buff_ptr->data_ptr,
                                                 res_buff_ptr->data_ptr);
    }

    /* Flag if one DSM item not enough  (should never happen) */
    if (DSM_BUFFER_THRESHOLD < temp_buff_ptr->used)
    {
      MSG_HIGH ("Response too long for one DSM item",0,0,0);
    }
    
    /* Put composite response in result buffer */
    memcpy (res_buff_ptr->data_ptr,
            temp_buff_ptr->data_ptr, temp_buff_ptr->size);
    res_buff_ptr->used = temp_buff_ptr->used;
    (void) dsm_free_buffer( temp_buff_ptr );
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_ati_cmd() */



/*===========================================================================

FUNCTION DSATACT_CALL_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  call should be aborted.  It should be installed in the command table
  as the abort handler function for abortable AT commands which bring
  up a call.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Call Manager is requested to end voice calls.

===========================================================================*/
/* ARGSUSED */
boolean dsatact_call_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  if ( dsat_curr_abort_handler == NULL )
  {
    /* the abort handler hasn't been set and the user 
       directs to abort the call.
    */
    dsatcmdp_abortable_state = ABORTING;
  }
  else
  {
    (*dsat_curr_abort_handler)();
  }

  return TRUE;
} /* dsatact_call_abort_handler */



/*===========================================================================

FUNCTION DSAT_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register a call back functions.
  It is used to register call back funtions for ATA and ATH.

  The call back handler can be NULL. If it is not an incoming call, 
  then ATA handler will be NULL. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_register_handlers
(
  dsat_incom_answer_cb_type  call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type   call_hangup_cb  /* Hangup call back */
)
{
  /* ATA Handler */
  dial_string_handler.answer_cb = call_answer_cb; 
  /* ATH Handler */
  dial_string_handler.hangup_cb = call_hangup_cb;

  return;
} /* dsat_register_handlers */



/*===========================================================================

FUNCTION DSAT_DEREGISTER_HANDLERS

DESCRIPTION
  This function is used to deregister the call back functions.
  It resets all the registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_handlers ( void )
{
  /* ATA Handler */
  dial_string_handler.answer_cb = NULL; 
  /* ATH Handler */
  dial_string_handler.hangup_cb = NULL;

  return;
} /* dsat_deregister_handlers */


/*===========================================================================

FUNCTION DSAT_REG_ATZ_CB

DESCRIPTION
  This function is used to register a call back function for ATZ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_atz_cb
(
  dsat_atz_cb_type atz_cb  /* atz call back */
)
{
  dial_string_handler.atz_cb = atz_cb;
  return;
} /* dsat_reg_atz_cb */



/*===========================================================================

FUNCTION DSAT_INIT_CB_HANDLERS

DESCRIPTION
  Initialize all the registered call handlers to NULL
  Called from dsat_init() after every reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_cb_handlers(void)
{
  /* Reset all the CB handlers to NULL */
  memset ( &dial_string_handler, 0, sizeof(dsat_dial_string_type) );

  return;
}



/*===========================================================================

FUNCTION DSAT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsat_proc_dial_str
(
  byte * in_ptr,                /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
)
{
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;
  byte c;
  
  while (((c = *in_ptr++) != '\0') &&
         (result != DSAT_DIAL_ERROR))
  {
    dsat_dial_val_e_type vresult = dsatutil_validate_dial_char(&c);
    switch (vresult)
    {
      case DSAT_DIAL_DIGIT:
      case DSAT_DIAL_POUND:
      case DSAT_DIAL_STAR:
        *out_ptr++ = c;
        if (result == DSAT_DIAL_EMPTY)
        {
          result = DSAT_DIAL_DIGIT;
        }
        break;

      case DSAT_DIAL_ASCII_ABC:
        /* Allowed ASCII characters */
        *out_ptr++ = c;
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_ASCII_D:
      case DSAT_DIAL_STRIP:
        /* Stripped characters */
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_SEMICOLON:
        /* Do not pass semicolon to output */
        result = (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode()) ?
                  vresult : DSAT_DIAL_ERROR;
        break;

      case DSAT_DIAL_ASCII_E:
      case DSAT_DIAL_IGNORE:
        break;

      default:
        result = DSAT_DIAL_ERROR;
        break;
        
    } /* switch */
  } /* while */
  *out_ptr = '\0';
  return result;

} /* dsat_proc_dial_str () */


/*===========================================================================

FUNCTION PROC_DIAL_STR

DESCRIPTION
  Parses a dial string in atd cmd token.  The number is in 
  tok_ptr->arg[0].

  Upon function returns, tok_ptr->arg[0] contains the processed/altered
  number.  The addtional information from the parsing process are in
  dial_ptr.

  The parser looks for chars of different category, see 
  dsat_dial_val_e_type.  It stripps out chars not needed depending on
  operating mode and on whether it is for voice call.
  It set the corresponding attrib bit on when encounters special chars
  or find a packet call pattern.

  Illegal characters terminate processing.  All other characters are ignore.

DEPENDENCIES
  None

RETURN VALUE
  TRUE          Processing finished without problem.
  FALSE         There is illegal char in the dial string, 
                or the mode is not right.

SIDE EFFECTS
  None

===========================================================================*/
boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type             cur_mode
)
{
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


#ifdef FEATURE_DATA_IS707
  /*-------------------------------------------------------
    char sets in CDMA mode
    -------------------------------------------------------*/
    /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_cdma[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_cdma[]        = "ABCDTP,W@!$;";

  /* This the set of legal dial chars */
  static const byte keep_cdma[]        = "0123456789*#ABCDTP,W@!$;";

#endif /* FEATURE_DATA_IS707 */

  const byte  *       illegal_set;
  const byte  *       ascii_set;
  const byte  *       keep_set;

  /* wrt_ptr points to the destination of processing
     rd_ptr  points to the source of processing
  */
  byte  *       wrt_ptr;
  byte  *       rd_ptr;
  byte          tmp_c;

  /* eol_ptr is writable end of line pointer */
  byte  * eol_ptr;

/*------------------------------------------------------------------*/

  /* init attrib */
  dial_ptr->attrib = 0;
  dial_ptr->num    = tok_ptr->arg[0];

  ASSERT(*tok_ptr->end_of_line == '\0');

  eol_ptr = tok_ptr->end_of_line;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /* process chars one by one
     1. if encoutners non digit # * turn on DIAL_ATTRIB_ASCII
     2. strip out chars according to ETSI, CDMA, or voice */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#ifdef FEATURE_DATA_IS707

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  if (cur_mode == DSAT_MODE_CDMA)
  {
    illegal_set =       illegal_cdma;
    ascii_set   =       ascii_cdma;
    keep_set    =       keep_cdma;
  }
#endif /* FEATURE_DATA_IS707 */
  else
  {
    /* Unexpected mode, don't know how to process the dial string */
    return FALSE;
  }    

  wrt_ptr = dial_ptr->num;
  rd_ptr = dial_ptr->num;

  /* process '+' for DIAL_ATTRIB_START_PLUS attribute*/
  if (*rd_ptr == '+')
  {
    dial_ptr->attrib |= DIAL_ATTRIB_START_PLUS;
    rd_ptr++;
    wrt_ptr++;
  }

  /* Dial string scan loop */
  while (*rd_ptr != '\0')
  {
    /* In ETSI mode, check for I/G modifiers */
    if ((cur_mode == DSAT_MODE_GSM) || (cur_mode == DSAT_MODE_WCDMA))
    {
      /* Check for CUG control */
      if ('G' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_G; /* clear bit */
      }
      else if ('g' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_G; /* clear bit */
      }

      /* Check for CLIR control */
      if ('I' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_I; /* clear bit */
      }
      else if ('i' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_I; /* clear bit */
      }
    }
    
    tmp_c = UPCASE(*rd_ptr);

    /* keep chars */
    if (strchr((char *)keep_set, (char) tmp_c) != NULL)
    {
      /* see if the char is in ascii_set */
      if (strchr((char *)ascii_set, (char) tmp_c) != NULL)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_ASCII;
      }

      /* the *rd_ptr is in the keep_set */
      *wrt_ptr = *rd_ptr;
      wrt_ptr++;
      rd_ptr++;
    }
    else if (strchr((char *)illegal_set, (char) *rd_ptr) != NULL)
    {
    /* current rd_ptr points to an illegal chars */
      return FALSE;
    }
    else
    {
      /* strip this char out */
      rd_ptr++;
    }

  }

  /* null terminate the dial_str */
  *wrt_ptr = '\0';
  eol_ptr = wrt_ptr;
  dial_ptr->len = (uint16) (eol_ptr - dial_ptr->num);
    
  /*-------------------------------------------------------
     After we strip out non-dialable chars,
     See if it is a packet number       
    -------------------------------------------------------*/

  /* Check if the dial number ends with # in ETSI mode
     or starts with # in CDMA mode */
  if ( (cur_mode == DSAT_MODE_CDMA)
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
     )
  {
    /* there is a good chance this is a etsi packet data call */
    if ( dsatutil_strcmp_pkt_dial_str( dial_ptr->num, cur_mode) )
    {
      dial_ptr->attrib |= DIAL_ATTRIB_PKT;
    }
  }

  return TRUE;
} /* proc_dial_str */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */



/*===========================================================================

FUNCTION ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to initiate an asynchronous SIO/ATCOP processing mode
  change to online data mode. The flush_tx parameter is used to indicate
  whether the SIO TX fifo should be flushed prior to switching to online
  data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data 
      currently in the queue will be lost.

  The function sends either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD or 
  DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD depending on the value of flush_tx to
  3G Dsmgr to initiate the mode change.  If no TX flushing is required, the
  mode change will occur immediately, otherwise, 3G Dsmgr will schedule the 
  SIO TX fifo to be flushed.  When the flush completes, the switch to online 
  data mode will be made.

DEPENDENCIES
  Must be able to allocate a DS Task command buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void enter_online_data_mode
(
  boolean flush_tx
)
{
  ds_cmd_type * cmd_ptr;

  /* Allocate the command buffer and queue the command to the DS Task */
  if ( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    /* Need to determine the appropriate command based on whether or
    ** not the TX buffer should be flushed.
    */
    if (flush_tx)
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD;
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD;
    }
 
    MSG_LOW("Sending command %d to DSTask", cmd_ptr->hdr.cmd_id,0,0);

    /* Send the command on its way */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR_FATAL("ds_get_cmd_buf() failure", 0,0,0);
  }
} /* enter_online_data_mode() */

#endif /* FEATURE_DATA */

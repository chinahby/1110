/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   ( C O M M O N 
				
		P A R A M E T E R  C O M M A N D S )   

		P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes parameter based AT commands.
  These parameter commands are common to any mode of operation.

  
EXTERNALIZED FUNCTIONS

  dsatparm_exec_param_cmd
    This function takes the result from the command line parser
  and executes it. This function executes parameter/S register
  commands.
  
  dsatparm_exec_ipr_cmd
    This function takes the result from the command line parser
  and executes it. It executes +IPR command.
  at+ipr is used to specify the data rate at which the DCE will 
  accept the command

  dsatparm_exec_ampv_cmd
    This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.


INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001-2008 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatparm.c_v   1.4   07 Nov 2002 16:15:36   wxie  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatparm.c#5 $ $DateTime: 2008/04/15 03:49:35 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/08   bs      Fixed Lint High's.
03/28/08   bs      Fixed Klocwork errors.
02/28/08   pp      Klocwork fixes.
02/25/08   sn      Merged lint error fix.
10/29/07   sn      Changes based on dsatutil_fmt_mixed_param_val() function's 
                   new prototype.
10/29/07   sn      Replaced banned string APIs.
03/17/03   ar      Add BROKEN_RANGE attribute in default command processing.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/27/03   wx      Be strict on the number of argument and the length of 
                   string input for mixed parameter
02/26/03   ar      Skip test response if LOCAL_TEST not set for mixed param.
                   Added +CGACT response generation to &V command.
02/20/03   wx      Return (x) instead of (x-x) for test command when a
                   command's numerical range is only one value
02/12/03   rsl     In process_read_cmd and test_cmd, replaced (byte) typecast
                   to (word) typecase to avoid large numbers being cut short.
01/31/03   ar      Add +CGEQMIN to &V response.
01/22/03   ar      Restrict &V respose to commands in current operating mode.
11/13/02   ar      Restored baud rate change to IPR handler, removed unused
                   change_baud() function
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/09/02   sb      IPR should not store the baud rate into NV (fixed it)
07/12/02   ak      fixed minor compiler warning.
07/12/02   ak      Changes to support writing new IPR value into nv.
06/27/02   ar      Call command specific reporting function on MULTI_INDEX 
                   command attribute
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.


===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include <stdio.h>
#include <string.h>
#include "AEEstd.h"

#include "dsati.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "err.h"
#include "nv.h"
#include "dstaski.h"

#ifdef FEATURE_DATA_MM
#include "ds3gsiolib.h"
#else
#include "ds3gsioif.h"
#endif /* FEATURE_DATA_MM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)   
#error code not present
#endif

#ifdef FEATURE_JCDMA_1X
  #include "ds707_jcdma_m51.h"
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/
LOCAL const dsat_string_item_type string_range [] = "(20,21,23-7E)";

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL boolean process_default_cmd
(
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse tbl*/
);

LOCAL boolean process_mixed_param_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
);

LOCAL boolean process_read_cmd
(
  const dsati_cmd_type *parse_table, /* Ptr to cmd in parse table */
  dsm_item_type * buf_ptr			 /* Place to put response     */
);

LOCAL boolean process_read_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr			 /*  Place to put response     */
);

LOCAL boolean process_write_cmd
(
  const tokens_struct_type *tok_ptr, /* Cmd tokens from parser    */
  const dsati_cmd_type *parse_table  /* Ptr to cmd in parse table */
);

LOCAL boolean process_write_mixed_param_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
);

LOCAL boolean process_test_cmd
(
  const dsati_cmd_type *parse_table, /* Ptr to cmd in parse tbl */
  dsm_item_type *buff_ptr			 /* Place to put response   */
);

LOCAL boolean process_test_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr             /* Place to put response */
);

LOCAL dsm_item_type * do_settings_action
(
  const dsati_cmd_type * table,     /*  Parse Table to dump             */
  unsigned int index,               /*  Size of Table                   */
  unsigned int * char_per_line,     /*  Used to count char across calls */
  dsm_item_type * res_buff_ptr      /* Place to put response            */
);

LOCAL dsm_item_type * do_settings_dump
(
  const dsati_cmd_type * table,     /*  Parse Table to dump             */
  const unsigned int sizeof_table,  /*  Size of Table                   */
  unsigned int * char_per_line,         /*  Count of char across calls */
  dsm_item_type *res_buff_ptr           /*  Place to put response     */
);


/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATPARM_EXEC_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes parameter/S register
  commands.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NORESPONSE : set for certain commands

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
   There is no argument. Set the deafult values  
  ----------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    if ( process_default_cmd (parse_table) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*----------------------------------------------------------------
   Standard processing for a command with an argument (WRITE command):
   -----------------------------------------------------------------*/
  
  else if (tok_ptr->op == (NA|EQ|AR))
  {
    if ( process_write_cmd (tok_ptr, parse_table) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------  
   Standard processing for a READ command:
   ----------------------------------------*/
	
  else if ( tok_ptr->op == (NA|QU) )
  {
    if ( process_read_cmd (parse_table, res_buff_ptr) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------
  Standard processing for a TEST command:
  ----------------------------------------*/

  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    if ( process_test_cmd (parse_table, res_buff_ptr) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------------------
  Command error: Operator(s) found did not make sense.
  ----------------------------------------------------*/

  else
  {
    result = DSAT_ERROR;
    return result;
  }

} /*  dsatparm_exec_param_cmd */


/*===========================================================================

FUNCTION DSATPARM_EXEC_IPR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +IPR command.
  at+ipr is used to specify the data rate at which the DCE will 
  accept the command

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NORESPONSE : set for certain commands

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_ipr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    old_ipr_val;
  dsat_result_enum_type result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  old_ipr_val = dsat_ipr_val;
  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
  
  if ((result == DSAT_OK) && 
    ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  { 
#ifdef FEATURE_DATA_MM
    if (ds3g_siolib_is_baud_valid ((sio_bitrate_type)dsat_ipr_val) == FALSE)
#else
    if (sio_is_baud_valid ((sio_bitrate_type)dsat_ipr_val) == FALSE)
#endif /* FEATURE_DATA_MM */
    {
      /* Baudrate not allowed, tell user "ERROR".  Restore old IPR value. */
      result = DSAT_ERROR;
      dsat_ipr_val = old_ipr_val;
    }
    else
    {
#ifdef FEATURE_JCDMA_1X
      /*--------------------------------------------------------------------
         Note that this if statement does the function call internally, which
         will result in the baud rate being changed, if the passed in ipr_val
         is okay
       --------------------------------------------------------------------*/
      if (ds707_jcdma_chng_m513_via_at((sio_bitrate_type)dsat_ipr_val) 
          == FALSE)
      {
        result = DSAT_ERROR;
        dsat_ipr_val = old_ipr_val;
      }
      else
#endif /* FEATURE_JCDMA_1X */
      {
        /*-------------------------------------------------------------------------
          Inform SIO that the baud rate should be scheduled after sending OK to the 
          TE device.
         -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
        (void)ds3g_siolib_change_baud_rate((sio_bitrate_type) dsat_ipr_val, FALSE);
#else
        (void)ds3g_sio_change_baud_rate((sio_bitrate_type) dsat_ipr_val, FALSE);
#endif /* FEATURE_DATA_MM */
      }
    }
  }
  
  return result;

}/* dsatparm_exec_ipr_cmd */



/*===========================================================================

FUNCTION DSATPARM_EXEC_AMPV_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_ampv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type     result;
  dsm_item_type             *buff_ptr;
  dsati_at_cmd_table_entry_type *array_ptr; /*Ptr to array of cmd tbl entries */
  unsigned int              j, r;           /*Count indices*/
  unsigned int              char_per_line;  /*Used to count char across calls*/
  dsati_operating_cmd_mode_type  cur_mode;  /*Current mode*/
  
  /* Let buff_ptr point to the buffer */
  /* If the response buffer gets full, a new buffer */
  /* is allocated and then buff_ptr will point to the new buffer */
  buff_ptr = res_buff_ptr;
  *buff_ptr->data_ptr = '\0';    /*  Initialize response to null */
  char_per_line = 0;             /*  Initialize character count to zero */                    

  if ((mode == DSAT_CMD) && (tok_ptr->op == NA))
  {
    /* Separate the dump from other output */
    buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
    
    /* Loop through the Command table array and dump the commands */
    /* for the current mode.                                      */
    cur_mode = dsatcmdp_get_operating_cmd_mode();
    if(cur_mode == NUM_OPER_CMD_MODES)
	{
	  return DSAT_ERROR;
	}
    for ( r = 0; r < NUM_AT_CMD_CATEGORIES; r++ )
    {
      /* Get a pointer to array of command table entries for 
         operating mode from command table. */
      array_ptr = at_cmd_table[r][cur_mode];  
      
      if ( array_ptr != NULL )
      {
        /* An array of command table entries exists for this element 
           of the table, now look for command tables within the array. */
        for ( j = 0; array_ptr[j].table_ptr != NULL; j++ )
        {
          buff_ptr = do_settings_dump(
                                      array_ptr[j].table_ptr,
                                      *(array_ptr[j].table_size),
                                      &char_per_line,
                                      buff_ptr
                                     );

        }/* for j* */
        
      }/*array_ptr != NULL*/ 
      
    }/* NUM_AT_CMD_CATEGORIES */
    
    /*-----------------------------------------------------------------------
      Close the last line, overwriting the trailing "; 
    ------------------------------------------------------------------------*/
    /* If we hit a NULL here, its really an error from do_settings_dump! */
    if ( buff_ptr != NULL ) 
    {
      buff_ptr->data_ptr[buff_ptr->used-2] = '\0';
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  } /* mode == DSAT_CMD */
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
}/* dsatparm_exec_ampv_cmd */


/*===========================================================================

FUNCTION DSATPARM_EXEC_DTYPE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATT and ATP commands.
  ATT and ATP are used to select type of dialling (tone or pulse) 
  to be used in subsequent dials. This commands will have no effect 
  in our code

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_dtype_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*-----------------------------------------
   Processing for a command with no argument
  -------------------------------------------*/
  if (tok_ptr->op == NA)
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
}/* dsatparm_exec_dtype_cmd */



/*===========================================================================

FUNCTION PROCESS_DEFAULT_CMD

DESCRIPTION
  This function is called by dsatparm_exec_param_cmd.
  It is called when there are no aguments to the command that is being
  processed

DEPENDENCIES
  None

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets default parameter value for the command

===========================================================================*/
LOCAL boolean process_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
)
{
  int i;

  /*  S-reg illegal without '=' */
  if ( (parse_table->attrib & 
        (CONFIG | EXTENDED | STRING | SIMPLE_DEC  | MIXED_PARAM))
    == (CONFIG))
  {
    /* failure in execution of cmd*/
    return ( FALSE );
  }
  
  /*  Single letter - set to 0  */
  else if ( (parse_table->attrib 
             & (CONFIG | EXTENDED | STRING | SIMPLE_DEC | MIXED_PARAM))
            == (CONFIG | SIMPLE_DEC))
  {
    * ((dsat_num_item_type *)parse_table->val_ptr) = 0;
    /* Success in execution of cmd */
    return ( TRUE );
  }

	/*  Extended - set to default */
  else if ( (parse_table->attrib & (CONFIG | EXTENDED | STRING | MIXED_PARAM))
  == (CONFIG | EXTENDED))
  {
    uint16 range_offset = 0;
    for ( i = 0; i < parse_table->compound; ++i )
    {
      * ((dsat_num_item_type *)parse_table->val_ptr + i)
        = ((dflm_type *) parse_table->def_lim_ptr)[i+range_offset].default_v;

      /* For BROKEN_RANGE, advance to next range segment */
      if (0 != (parse_table->attrib & BROKEN_RANGE))
      {
        while (MAX_BROKEN_RANGE !=
                ((dflm_type *)parse_table->
                  def_lim_ptr)[i + range_offset].default_v)
        {
          range_offset++;
        }
      }
    }
    /* Success in execution of cmd*/
    return ( TRUE );
  }

  /*  String - set to null      */
  else if ( (parse_table->attrib & (CONFIG | EXTENDED | STRING | MIXED_PARAM))
    == (CONFIG | EXTENDED | STRING))
  {
    * ((dsat_string_item_type *)parse_table->val_ptr) = '\0';
    /* Success in execution of cmd*/
    return ( TRUE );
  }

  /* deal with mixed parameter case */
  else if ( (parse_table->attrib & (CONFIG | MIXED_PARAM | LIST))
    == (CONFIG | MIXED_PARAM | LIST) )
  {
    return process_mixed_param_default_cmd(parse_table);
  }
  
  else
  {
    return ( FALSE );
  }

}/*process_default_cmd*/


/*===========================================================================

FUNCTION PROCESS_MIXED_PARAM_DEFAULT_CMD

DESCRIPTION
  This function is called by process_default_cmd
  It is called when the argument is of MIXED_PARAM type.

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets default parameter value for the command

===========================================================================*/
LOCAL boolean process_mixed_param_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
)
{
  int index;
  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *) parse_table->val_ptr;
  
  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;

    /* For MIXED_PARAM parameters, we take action from individual
       argument.
       If the argument is a string, we set it to NULL.
       If the argument is an index into string list, we set to its default.
       If the argument is an integar with range, we set it to its default.
    */
  for (index = 0; index < parse_table->compound; index++)
  {
    /* The argument is an index into string list */
    if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | LIST) )
    {
      const def_list_type * def_list_ptr = 
        &mixed_def_list[index]->def->def_list;
      
      val_list[index].num_item = def_list_ptr->default_v;
    }

    /* The argument is an integar with range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;
      
      val_list[index].num_item = dflm_ptr->default_v;
    }

    /* the argument is a string, set it to NULL */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | STRING) )
    {
      val_list[index].string_item[0] = '\0';
    }

    /* the argument is not anticipated */
    else
    {
      ERR("Mixed parameter default cmd %d, %dth param has wrong attrib %d",
          parse_table->name, index, mixed_def_list[index]->attrib);
      return FALSE;
    }

  }

  return TRUE;
} /* process_mixed_param_default_cmd */


/*===========================================================================

FUNCTION PROCESS_READ_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called when a command with READ operation is received

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed
 
SIDE EFFECTS
  Sets response in supplied buffer for "READ" operation.
 
===========================================================================*/
LOCAL boolean process_read_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr			 /*  Place to put response     */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;
  unsigned int return_val;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  if ( (parse_table->attrib & (CONFIG | STRING | MIXED_PARAM)) == CONFIG)
  {
    /*  Extended */
    if (parse_table->attrib & EXTENDED)
    {
      return_val = dsatutil_fmt_num_val_ex (parse_table, rb_ptr, RESPONSE);
      temp_buf_ptr->used = (word)return_val + 1;
      temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
      return ( TRUE );
    }
    /*  Single letter  */
    else
    {
      return_val = dsatutil_fmt_num_val (parse_table, rb_ptr);
      temp_buf_ptr->used = (word)return_val + 1;
      temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
      return ( TRUE );
    }
  }
  /*  String */
  else if ( (parse_table->attrib & (CONFIG | STRING | MIXED_PARAM)) ==
    (CONFIG | STRING))
  {
    return_val = dsatutil_fmt_string_val (parse_table, rb_ptr, RESPONSE);
    temp_buf_ptr->used = (word)return_val + 1;
    temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
    return ( TRUE );
  }
  
  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (MIXED_PARAM | LIST))
    == (MIXED_PARAM | LIST) )
  {
    return process_read_mixed_param_cmd(parse_table, buf_ptr);
  }

  else
  {
    return ( FALSE );
  }

} /* process_read_cmd */


/*===========================================================================

FUNCTION PROCESS_READ_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_read_command
  It is called when a process_read_command process MIXED_PARAM type 
  parameters.

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed
 
SIDE EFFECTS
  Sets response in supplied buffer for "READ" operation.
 
===========================================================================*/
LOCAL boolean process_read_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr			 /*  Place to put response     */
)
{
  word written_size;

  written_size =
    (word) dsatutil_fmt_mixed_param_val (parse_table, buf_ptr, RESPONSE_ALL);
  
  if(written_size == 0)
  {
    return FALSE;
  }
  else
  {
    buf_ptr->used = (word) written_size;
  }
      
  return ( TRUE );
} /* process_read_mixed_param_cmd */


/*===========================================================================

FUNCTION PROCESS_WRITE_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called for standard processing of a command with an 
  argument (WRITE command)
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets parameter value for "SET"  command

===========================================================================*/
LOCAL boolean process_write_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
)
{
  if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM)) 
       == CONFIG)
  {
    if ( dsatutil_put_num_arg(parse_table, tok_ptr) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }
  else if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM))
            == (CONFIG | LIST) )
  {
    if ( dsatutil_put_num_arg(parse_table, tok_ptr) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }
  else if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM))
    == (CONFIG | STRING))
  {
    if ( dsatutil_put_string_arg (parse_table, tok_ptr, STORE) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }

  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (CONFIG | MIXED_PARAM | LIST))
    == (CONFIG | MIXED_PARAM | LIST) )
  {
    return process_write_mixed_param_cmd(tok_ptr, parse_table);
  }
  
  else
  {
    return ( FALSE );
  }
}/* process_write_cmd */


/*===========================================================================

FUNCTION PROCESS_WRITE_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_write_cmd.
  It is called for process_write_cmd process MIXED_PARAM type parameters.
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets parameter value for "SET"  command

===========================================================================*/
LOCAL boolean process_write_mixed_param_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
)
{
  uint8 index;
  boolean hit = FALSE;
  param_srch_enum_type srchrc;

  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *) parse_table->val_ptr;
  
  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;
  
  if(tok_ptr->args_found > parse_table->compound)
  {
    return FALSE;
  }

  for (index = 0; index < tok_ptr->args_found; index++)
  {
    byte * token_arg_ptr = tok_ptr->arg[index];

    /* the index-th parameter is of type STRING, it has no 
       def values, save the token to the val */
    if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | STRING) )
    {
      dsat_string_item_type * val_string = 
        val_list[index].string_item;
      word val_len;
      byte * use_arg = token_arg_ptr;
      byte stripped_arg[MAX_LINE_SIZE];

      /* Do nothing if null parameter */
      if (0 != *use_arg)
      {
        val_len = mixed_def_list[index]->def->string_len;
        
        /* if the argument is quoted, strip out the quotes for internal storage
         * unless quoting attribute absent. */
        if(((( mixed_def_list[index]->attrib &
              ( CONFIG | STRING | YES_QUOTE )) == ( CONFIG | STRING | YES_QUOTE )
            ) ||
            ((mixed_def_list[index]->attrib &
              ( CONFIG | STRING | NO_QUOTE )) == ( CONFIG | STRING | NO_QUOTE )
            )
           ) && ('"' == *use_arg)
          )
        {
          if(!dsatutil_strip_quotes_out(token_arg_ptr, stripped_arg, MAX_LINE_SIZE))
          {
            return FALSE;
          }
          use_arg = stripped_arg;
        }
        
        /* check if the user input is too big */
        if(strlen((char *) use_arg) > val_len)
        {
          return FALSE;
        }
        
        /* The max length of the argument string is val_len, but the space
           allocated for the string is val_len + 1 bytes */
        (void) std_strlcpy((char *) val_string, (char *) use_arg, val_len+1);
      }
    }
      
    /* the index-th parameter is a string within a string list,
       we will determine its index into the string list.  Use
       this index to set the val_ptr */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | LIST) )
    {
      dsat_num_item_type val_num;
      const def_list_type * def_list_ptr = &mixed_def_list[index]->def->def_list;
      srchrc = dsatutil_mixed_param_set_num_arg(&val_num, def_list_ptr,
                                            token_arg_ptr, 
                                                mixed_def_list[index]->attrib);
      /* Continue if parameter found */
      if (PARAM_NULL != srchrc)
      {
        if(PARAM_MISS == srchrc)
        {
          return FALSE;
        }
        val_list[index].num_item = val_num;
      }
    }
      
    /* the index-th parameter is a number with a range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {
      dsat_num_item_type * val_num_ptr = &val_list[index].num_item;
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;
      word radix;
      dsat_num_item_type arg_in_num;
      atoi_enum_type atoi_return;
        
      radix = ( (mixed_def_list[index]->attrib & HEX) ? 16 : 10);
      atoi_return = dsatutil_atoi (&arg_in_num, token_arg_ptr, radix);
        
      if (atoi_return == ATOI_OK)
      {
        /* Check to see if current parameter range is segmented */
        if (0 == (mixed_def_list[index]->attrib & BROKEN_RANGE))
        {
          /* Continuous range */
          if (arg_in_num < dflm_ptr->lower ||
              arg_in_num > dflm_ptr->upper)
          {
            ERR("The argument %d is out of range",arg_in_num,0,0);
            return FALSE;
          }
          else
          {
            *val_num_ptr = arg_in_num;
          }
        } 
        else
        {
          /* Discontinuous range so loop over each segment */
          const dflm_type *dflm_ptr_orig = dflm_ptr;
          while (MAX_BROKEN_RANGE != dflm_ptr->default_v)
          {
            if ((arg_in_num >= dflm_ptr->lower) &&
                (arg_in_num <= dflm_ptr->upper))
            {
              *val_num_ptr = arg_in_num;
              hit = TRUE;
              break;
            }
            dflm_ptr++;
            if ( (dflm_ptr - dflm_ptr_orig) >= 2)
            {
              /* Currently only CFUN command enters this loop
                It has only 2 valid broken range entries. */
              ERR("Currently this is not supported ",0,0,0);
              return FALSE;
            }
          }
          if (FALSE == hit)
          {
            ERR("The argument %d is out of range",arg_in_num,0,0);
            return FALSE;
          }
        }
      } 
      else if (atoi_return == ATOI_OUT_OF_RANGE)
      {
        ERR("Argument cannot be converted to number",0,0,0);
        return FALSE;
      }
      /*else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset  */
      /*    and the value is not stored                               */
    }

    else
    {
      ERR("Mixed parameter write cmd %d, %dth param has wrong attrib %d",
          parse_table->name, index, mixed_def_list[index]->attrib);
      return FALSE;
    }
  }
  return ( TRUE );
} /* process_write_mixed_param_cmd */


/*===========================================================================

FUNCTION PROCESS_TEST_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called when a command with TEST operation is received
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets response in supplied buffer for "TEST" operation.

===========================================================================*/
LOCAL boolean process_test_cmd 
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr			 /* Place to put response */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;
  unsigned int return_val;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  if ( (parse_table->attrib &
        (EXTENDED | CONFIG | STRING | MIXED_PARAM)) == (EXTENDED | CONFIG ))
  {
    return_val = dsatutil_fmt_num_range (parse_table, rb_ptr);
    temp_buf_ptr->used = (word)return_val + 1;
    temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
    return ( TRUE );
  }
  else if ( (parse_table->attrib & (EXTENDED | CONFIG | STRING | MIXED_PARAM)) ==
              (EXTENDED | CONFIG | STRING))
  {
    dsatutil_fmt_string_range (parse_table, rb_ptr, string_range);
    return ( TRUE );
  }

  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (MIXED_PARAM | LIST | LOCAL_TEST))
    == (MIXED_PARAM | LIST | LOCAL_TEST) )
  {
    return process_test_mixed_param_cmd(parse_table, buf_ptr);
  }

  else  /*  Command does not accept TEST  */
  {
    rb_ptr[0] = '\0';
    return ( FALSE );
  }

}/* process_test_cmd */


/*===========================================================================

FUNCTION PROCESS_TEST_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_test_cmd.
  It is called when a process_test_cmd processes MIXED_PARAM type
  parameters.
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets response in supplied buffer for "TEST" operation.

===========================================================================*/
LOCAL boolean process_test_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr             /* Place to put response */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;

  int index;
  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;
  word written_size;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  /* put cmd_name: at the beginning of the response buffer */
  written_size = (word) std_strlprintf((char *)rb_ptr, 
                                       temp_buf_ptr->size - temp_buf_ptr->used, 
                                       "%s: ", parse_table->name);
  rb_ptr += written_size;
  temp_buf_ptr->used += written_size;

  for (index = 0; index < parse_table->compound; index++)
  {
    /* for string with LOCAL_TEST and YES_QUOTE */
    if( (mixed_def_list[index]->attrib & (STRING | LIST | LOCAL_TEST | YES_QUOTE)) == 
        (STRING | LOCAL_TEST | YES_QUOTE) )
    {
      *rb_ptr = '"';
      rb_ptr++;
      temp_buf_ptr->used++;
      *rb_ptr = '"';
      rb_ptr++;
      temp_buf_ptr->used++;
      *rb_ptr = ',';
      rb_ptr++;
      temp_buf_ptr->used++;
    }

    /* for string, leave it empty */
    else if( (mixed_def_list[index]->attrib & STRING) == 
              STRING )
    {
      *rb_ptr = ',';
      rb_ptr++;
      temp_buf_ptr->used++;
    }

    /* the index-th parameter is a string within a string list, the string
       list also contains the test string */
    else if( (mixed_def_list[index]->attrib & (STRING | LIST | LOCAL_TEST)) == 
             (LIST | LOCAL_TEST) )
    {
      const dsat_string_item_type * test_string =
        mixed_def_list[index]->def->def_list.list_t;

      written_size = (word) std_strlprintf((char *)rb_ptr, 
                                     temp_buf_ptr->size - temp_buf_ptr->used, 
                                     "%s,", test_string);
      rb_ptr += written_size;
      temp_buf_ptr->used += written_size;
    }
      
    /* the index-th parameter is a number with a range, print its range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | LIST | LOCAL_TEST)) == 
             (CONFIG | LOCAL_TEST) )
    {
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;

      if(dflm_ptr->lower == dflm_ptr->upper)
      {
        written_size = (word) std_strlprintf((char *)rb_ptr, 
                                       temp_buf_ptr->size - temp_buf_ptr->used, 
                                       "(%u),", dflm_ptr->lower);
      }
      else
      {
        written_size = (word) std_strlprintf((char *)rb_ptr, 
                                      temp_buf_ptr->size - temp_buf_ptr->used, 
                                      "(%u-%u),",
                                      dflm_ptr->lower, 
                                      dflm_ptr->upper);
      }

      rb_ptr += written_size;
      temp_buf_ptr->used += written_size;
    }
    else if( (mixed_def_list[index]->attrib & LOCAL_TEST) == 0 )
    {
      /* Do nothing if LOCAL_TEST is not set */
    }
    else
    {
      ERR("Mixed parameter test cmd %d, %dth param has wrong attrib %d",
          parse_table->name, index, mixed_def_list[index]->attrib);
      return FALSE;
    }

  } /* for loop */
  /* get rid of the last ',' */
  temp_buf_ptr->used--;
      
  return ( TRUE );
}


/*===========================================================================

FUNCTION DO_SETTINGS_DUMP

DESCRIPTION
  This function is used by dsatparm_exec_ampv_cmd.
  This function gets a pointer to a AT commands table.
  It loops through the table and dumps the response in response buffer
  If the response buffer is full, then a new buffer is allocated and
  dsm_append is used to append the two buffers.

DEPENDENCIES
  None  

RETURN VALUE
  Pointer to dsm_item_type

SIDE EFFECTS
  dsm_append() is called to append multiple response buffers

===========================================================================*/
LOCAL dsm_item_type * do_settings_dump
(
  const dsati_cmd_type * table,         /*  Parse Table to dump       */
  const unsigned int sizeof_table,      /*  Size of Table             */
  unsigned int * char_per_line,         /*  Count of char across calls */
  dsm_item_type *res_buff_ptr           /*  Place to put response      */
)
{
  unsigned int i;
  dsm_item_type *cur_buff_ptr = res_buff_ptr;
  
  for (i = 0; i < sizeof_table; ++i)
  {
    /* Loop thru the table, and process all entries which have a 
       variable and whose NAME does not begin with '$'.  Proprietary 
       items are not dumped. */
    if ( (table + i)->val_ptr != NULL && (table + i)->name[0] != '$')
    {
      /* Write the formatted response in response buffer */
      if (cur_buff_ptr == NULL )
      {
        /* Some problem with DSM items, return immediately */
        break;
      }
      cur_buff_ptr = do_settings_action ( table, i, char_per_line,
                                          (dsm_item_type *)cur_buff_ptr );

    } /* if table != NULL */
  
  }/* for i */

  return cur_buff_ptr;

}/*do_settings_dump*/


/*===========================================================================

FUNCTION DO_SETTINGS_ACTION

DESCRIPTION
  This is a support function for DO_SETTINGS_DUMP.
  Takes the individual AT command, formats the response and dumps it
  in the reponse buffer.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to dsm_item_type

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsm_item_type * do_settings_action
(
  const dsati_cmd_type * table,         /* Parse Table to dump             */
  unsigned int index,                   /* Size of Table                   */
  unsigned int * char_per_line,         /* Used to count char across calls */
  dsm_item_type * res_buff_ptr          /* Place to put response           */
)
{
  unsigned int return_val = 0;
  unsigned int cnt = 0;
  unsigned int multi_index_tab_length = 0;
  unsigned int offset = 1;
  word * used_ptr;
  byte * buff_ptr;
  dsm_item_type *cur_buff_ptr = res_buff_ptr;
  dsm_item_type *prv_buff_ptr;
  word   prv_used = 0;
  
  static const struct
  {
    byte name[10];
    dsat_num_item_type (*fmt_func)( const struct dsati_cmd_struct*,
                                    dsm_item_type**,
                                    multi_index_fmt_e_type );
  } multi_index_response_fmt_table[] =
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
    "", NULL
  };

  multi_index_tab_length = sizeof(multi_index_response_fmt_table)/sizeof(multi_index_response_fmt_table[0]);

  /* Check to see if a new DSM item is required */
  if (NULL == (cur_buff_ptr =
                dsatutil_append_dsm_item(res_buff_ptr,
                                         DSM_BUFFER_THRESHOLD)))
  {
    MSG_HIGH ("Problem appending DSM item" ,0,0,0);
    return NULL;
  }
  if (res_buff_ptr != cur_buff_ptr)
  {
    /* On DSM switch, start with space to allow linefeed insert */
    cur_buff_ptr->data_ptr[cur_buff_ptr->used++] = ' '; 
  }

  
  buff_ptr = &cur_buff_ptr->data_ptr[cur_buff_ptr->used];
  used_ptr = &cur_buff_ptr->used;

  /*-------------------------------------------------------------------------
      Process the table item, using the command attribute to determine
      the format.
   ----------------------------------------------------------------------*/
  if ( (table + index)->attrib & MULTI_INDEX )
  {
    /* Multiple parameter case */

    /* Lookup command-specific response formatting function */
    while (cnt < multi_index_tab_length)
    {
      if(0 == dsatutil_strcmp_ig_sp_case(
                multi_index_response_fmt_table[cnt].name,
                (table + index)->name))
      {
        /* Call response format function */
        prv_buff_ptr = cur_buff_ptr;
        prv_used = prv_buff_ptr->used;
        return_val = multi_index_response_fmt_table[cnt].fmt_func (
                       (table + index),
                       &cur_buff_ptr,
                       MULTI_INDEX_AMPV);
        
        /* Check for DSM item change; adjust local pointers if so */
        if (prv_buff_ptr != cur_buff_ptr)
        {
          *char_per_line += prv_buff_ptr->used - prv_used;
          buff_ptr = cur_buff_ptr->data_ptr;
          used_ptr = &cur_buff_ptr->used;
          offset = 0;
        }
        break;
      }
      cnt++;
    }
  }
  else if ( (table + index)->attrib & MIXED_PARAM )
  {
    /* Mixed parameter case */
    return_val = (unsigned int)
      dsatutil_fmt_mixed_param_val (table + index, cur_buff_ptr, RESPONSE_ALL);
    
  }
  else if ( (table + index)->attrib & STRING)
  {
    /* String case */
    return_val = dsatutil_fmt_string_val(table + index,
                                         buff_ptr, RESPONSE_ALL);
  }
  else
  {
    /* Numeric case */
    return_val = dsatutil_fmt_num_val_ex(table + index,
                                         buff_ptr, RESPONSE_ALL);
  }

  if (0 == return_val)
  {
    ERR_FATAL("Parameter formatting failed",0,0,0);
  }
  
  *char_per_line += return_val;
  buff_ptr += return_val;

  /*-------------------------------------------------------------------------
      If the current item causes the output line to be longer than
      72 characters, replace the "; " preceding it with <CR><LF>
  -------------------------------------------------------------------------*/
  if (*char_per_line > 72)
  {
    *(buff_ptr-(return_val+offset)) = '\n';
    *char_per_line = return_val;
    offset = 1;
  }
  *used_ptr += (word)return_val;
  
  /*-------------------------------------------------------------------------
      Terminate the current item with "; ".
  -------------------------------------------------------------------------*/
  *buff_ptr++ = ';';
  *buff_ptr++ = ' ';
  *used_ptr += 2;
  *char_per_line += 2;
  
  return cur_buff_ptr;
}/* do_settings_action */

#endif /* FEATURE_DATA */

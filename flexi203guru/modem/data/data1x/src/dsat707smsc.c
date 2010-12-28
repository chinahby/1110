/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific sms related
  AT commands.

  The cmd is not specified in 707 spec.  We (qcom) add them to automate sms
  testing.

EXTERNALIZED FUNCTIONS

  dsat707sms_exec_qcsmp_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCSMP command.
    $QCSMP is used to set parameters for sending text sms messages.

  dsat707sms_exec_qcpms_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCPMS command.
    $QCPMS is used to select preferred memory storage for reading, 
    writing etc.

  dsat707sms_exec_qcnmi_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCNMI command.
    $QCNMI is used to select the procedure of how receiving new messages
    from the network is indicated to TE.

  dsat707sms_exec_qcmgs_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGS command.
    $QCMGS is used to send a message from TE to the network.

  dsat707sms_exec_qcmgr_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$CMGR command.
    $QCMGR is used to read a sms message.

  dsat707sms_exec_qcmgl_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGL command. 
    $QCMGL command lists all the SMS saved in the loaction.

  dsat707sms_exec_qcmss_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMSS command.
    $QCMSS is used to send a message already stored from Memory to the network.

  dsat707sms_exec_qcmgd_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGD command.
    $QCMGD is used to delete sms messages from <mem1>.

  dsat707sms_exec_qcmgw_cmd
    This function takes the result from the command line parser
    and executes it. This function executes AT$QCMGW command. 
    $QCMGW is used to store message to memory storage <mem2>

  dsat707sms_send_sms_msg
    Central place/function call for sending sms message.
    In case of actual message transmission, it is done in 2 phases. First you 
    get the actual command with all the parameters. Parameters are stored and 
    user is sent a prompt to indicate that message has to be typed. Then the 
    message is transmited. This function is used to transmit the actual message.

  is707_send_smstxt_msg
    Local function that is used to send TEXT message.

Copyright (c) 2009 by QUALCOMM Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707smsc.c#1 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $


                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/09   nc      Added support for SMS Commands 

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"

#ifdef FEATURE_CDMA_SMS

#include <stdlib.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "err.h"
#include "wms.h"
#include "string.h"

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#ifdef DEBUG_DSATCOP
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_DSATCOP */

/*===========================================================================
            Data types
===========================================================================*/

/* import variable from other files inside sms module */

/* Variable to find out what command is being processed currently */
extern sms_cmd_name_e_type      dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type       dsatsmsi_client_id;

/* Variable used in CMGL SM */
extern sms_cmd_sm_type          dsatsmsi_cmd_sm;

/* Variables needed for QCPMS */
/* remembers the mem being reported */
extern cpms_mem_status_data_s_type dsatsmsi_mem_report;

/* actual data item     */
extern nv_item_type         ds_nv_item;


/* Variable to retain destination address for send commands (text mode) */         
wms_address_s_type                send_addr;

wms_message_tag_e_type  dsat707smsi_send_stat = WMS_TAG_NONE;
wms_address_s_type   dsat707smsi_send_addr;

/* LOCAL variables */
/* Note: Following variables are kept    */
/* Global to avoid SP corruption problem */
LOCAL wms_client_message_s_type         is707_client_msg;

/* LOCAL Function Delarations */

LOCAL boolean is707_send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the txt message to be sent */
  int   len       /* Len of the txt message to be sent */
);
LOCAL wms_message_tag_e_type stat707_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 int  * all_flag  /* Ptr for "ALL",4 FLAG      */
);
 
/* FUNCTION DEFINITIONS */
/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCSMP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSMP command.
  $QCSMP is used to set parameters for sending text sms messages.

  DEPENDENCIES
  None

  RETURN VALUE
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcsmp_cmd
(
  dsat_mode_enum_type mode,           /* AT command mode:            */
  const dsati_cmd_type *tab_entry,    /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr         /* Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcsmp_cmd, op:%d", tok_ptr->op,0,0);
        );

  /*---------------------------------------
    Processing WRITE and READ command :
  -----------------------------------------*/
  if( tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA|QU) )
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      int arg_index;

      /* Try to validate the format of the valid period and 
       *  Deferred Delivery Time
       */
      for (arg_index = 1; arg_index < 4; arg_index +=2 )
      {
        /* iterate the loop for arg_index = 1 (vpf), 3 (ddtf) */
        if(tok_ptr->args_found >= (unsigned int) (arg_index + 2))
        {
          int fmt = atoi((char *)tok_ptr->arg[arg_index]);

          if(fmt == 0)
          {
            /* Absolute format */
            if ( !dsat707smsi_string_to_timestamp(
                    (char *)tok_ptr->arg[arg_index + 1], NULL) )
            {
              result = DSAT_ERROR;
            }
          }
          else if(fmt == 1)
          {
            /* Relative format */
            dsat_num_item_type rela_val;
          
            if ( ATOI_OK != 
                 dsatutil_atoi(&rela_val, tok_ptr->arg[arg_index + 1], 10) )
            {
              result = DSAT_ERROR;
            }
          
            if( rela_val >248 )
            {
              result = DSAT_ERROR;
            }
          }
          else
          {
            result = DSAT_ERROR;
          }
        }
      } /* for loop */
    } /* (tok_ptr->op == (NA|EQ|AR)) */

    if(result == DSAT_OK)
    {
      /* do the usual parameter processing */
      result = dsatparm_exec_param_cmd (  mode,
                                          tab_entry,
                                          tok_ptr,
                                          res_buff_ptr
                                          );
    }
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;
  }
  /*---------------------------------------------------
   Processing command with no args and wrong operator :
  -----------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707sms_exec_qcsmp_cmd */


/*===========================================================================

FUNCTION DSAT707SMS_EXEC_QCPMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPMS command.
  $QCPMS is used to select preferred memory storage for reading, 
  writing etc.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD: An async call has made to wms.

SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcpms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define SM_MEM 2

  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type     status = NV_DONE_S;
  dsat_num_item_type mem1_type;   /* save the old mem types */
  dsat_num_item_type mem2_type;
  dsat_num_item_type mem3_type;

  dsatsmsi_mem_report.mem3_changed = FALSE;
/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcpms_cmd, op:%d", tok_ptr->op,0,0);
        );
/* Read NV_RTRE_CONFIG_I to determine RUIM-Config */
  status = dsatutil_get_nv_item( NV_RTRE_CONFIG_I, &ds_nv_item );
  if( status != NV_DONE_S )
  {
    /* Setting to NV only*/
    ds_nv_item.rtre_config = NV_RTRE_CONFIG_NV_ONLY;
    MSG_ERROR( "Bad NV_RTRE_CONFIG_I read status %d setting default to NV ONLY", 
                                                                 status, 0, 0 );
  }

  /*----------------------------  
   Processing WRITE and READ command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == (NA|QU)) )
  {
    /* for WRITE do the usual parameter processing */
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      /* do parameter checking */

      /* mem1 is compulsory */
      if(tok_ptr->arg[0][0] == '\0')
      {
        ERR("QCPMS: mem1 is needed",0,0,0);
        return DSAT_ERROR;
      }
      mem3_type = dsat707sms_qcpms_val[CPMS_MEM3];
      mem2_type = dsat707sms_qcpms_val[CPMS_MEM2];
      mem1_type = dsat707sms_qcpms_val[CPMS_MEM1];

      result = dsatparm_exec_param_cmd ( 
                                        mode,
                                        parse_table,
                                        tok_ptr,
                                        res_buff_ptr
                                        );
      if(result != DSAT_OK)
      {
        MSG_HIGH("Erroneous input params in $QCPMS",0,0,0);
        return DSAT_ERROR;
      }

      /* In a RUIM build,in NV_Only mode Check if any of qcpms_val is set to "SM" */
      if( (ds_nv_item.rtre_config == NV_RTRE_CONFIG_NV_ONLY) && 
          ((dsat707sms_qcpms_val[CPMS_MEM3] == SM_MEM)||
           (dsat707sms_qcpms_val[CPMS_MEM2] == SM_MEM)||
           (dsat707sms_qcpms_val[CPMS_MEM1] == SM_MEM) )
        )
      {
        /* Revert the old qcpms_val */
        dsat707sms_qcpms_val[CPMS_MEM3]= mem3_type ;
        dsat707sms_qcpms_val[CPMS_MEM2]= mem2_type ;
        dsat707sms_qcpms_val[CPMS_MEM1]= mem1_type ;
        MSG_HIGH("Erroneous input params in $QCPMS in NV only mode",0,0,0);
        return DSAT_ERROR;
      }

      if(mem3_type != dsat707sms_qcpms_val[CPMS_MEM3])
      {
        dsatsmsi_mem_report.mem3_changed = TRUE;
      }
      
      dsatsmsi_mem_report.is_read = FALSE;
    }
    else /* for READ command */
    {
      dsatsmsi_mem_report.is_read = TRUE;
    }

    if ( result == DSAT_OK ) /* Set command passed */
    {
      /* get memory status for mem1, mem2, and mem3 */
      wms_status_e_type status;
      wms_memory_store_e_type mem_store;
      dsatsmsi_mem_report.mem = CPMS_MEM1;
      
      /* get memory status of msg store one at a time */
      
      /* set mem_store */
      mem_store = dsatsmsi_memory_lookup(
                (int) dsat707sms_qcpms_val[dsatsmsi_mem_report.mem], 
                dsatsmsi_mem_report.mem);

      /* call the wms function to get memory status */
      dsatsmsi_pres_cmd = SMS_CPMS;
      status = wms_cfg_get_memory_status(
                                         dsatsmsi_client_id,
                                         dsatsmsi_cmd_cb_func,
                                         &dsatsmsi_pres_cmd,
                                         mem_store,
                                         WMS_TAG_NONE);
        
      /* If the command is not accepted, the command callback will
           not be called, the mem event will not be invoked */
      if(status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        dsatsmsi_pres_cmd = SMS_NONE;
        result = DSAT_ERROR;
      }
    } /* if ( result == DSAT_OK ) */

    else
    {
      ERR("$QCPMS write command fails",0,0,0);
      result = DSAT_ERROR;
    }

  } /* ( tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA|QU) ) */

  /*---------------------------------------
   Processing command with no args :
   ----------------------------------------*/
  else if (tok_ptr->op == (NA) ) 
  {
    ERR("$QCPMS need at least one argument",0,0,0);
    result = DSAT_ERROR;
    return result;
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    char cpms_test_nv_mode[] =   "\"ME\",\"MT\"";
    char cpms_test_ruim_mode[] = "\"ME\",\"MT\",\"SM\"";

    res_buff_ptr->used = 
      (uint16) std_strlprintf((char*)res_buff_ptr->data_ptr,
                               res_buff_ptr->size,
                               "$QCPMS: %s",
                               ( ( ds_nv_item.rtre_config == NV_RTRE_CONFIG_NV_ONLY ) ?
                               (const char*)cpms_test_nv_mode:
                               (const char*)cpms_test_ruim_mode) );
    return DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    dsatsmsi_pres_cmd = SMS_NONE;
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707_exec_qcpms_cmd */


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCNMI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCNMI command.
  $QCNMI is used to select the procedure of how receiving new messages
  from the network is indicated to TE.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_CMD_ERR_RSP : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcnmi_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type mode_val;
  dsat_num_item_type mt_val;
  dsm_item_type *buff_ptr;
  unsigned int used_val;


/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcnmi_cmd, op:%d", tok_ptr->op,0,0);
        );

  buff_ptr = res_buff_ptr;

  if( tok_ptr->op == (NA|EQ|AR) )
  {

    if( tok_ptr->args_found > 3)
    {
      MSG_HIGH(" Only three parameters are accepted for $QCNMI ",0,0,0);
      return DSAT_ERROR;
    }

    if( VALID_TOKEN(0))
    {
      if ( dsatutil_atoi ( &mode_val, tok_ptr->arg[0], 10 ) )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      mode_val = dsat707sms_qcnmi_val[0];
    }

    if( VALID_TOKEN(1))
    {
      if ( dsatutil_atoi ( &mt_val, tok_ptr->arg[1], 10 ) )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      mt_val = dsat707sms_qcnmi_val[1];
    }

    if ( ( ( mt_val == 2 ) && ( mode_val != 1 ) ) ||
         ( ( mt_val == 3 ) && ( mode_val != 1 ) ) )
    {
      /* we got an error, format a +CMS ERROR */
      used_val = 
        dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_SUPPORTED);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      return result;
    }
  }
  
  result = dsatparm_exec_param_cmd ( mode,
                                      tab_entry,
                                      tok_ptr,
                                      res_buff_ptr
                                    );
  
  if ( result == DSAT_ERROR )
  {
    if ( ( tok_ptr->op == (NA|EQ|AR) )&& 
     ( tok_ptr->args_found == 1 ) && 
     ( !( VALID_TOKEN(0) ) ) )
    {
      /* We got badly formatted command */
      return result;
    }
    else
    {
      /* we got an error, format a +CMS ERROR */
      used_val = 
        dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_SUPPORTED);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      return result;
    }
  }
  else
  {
    if ( ( tok_ptr->op == (NA|EQ|AR) ) || ( tok_ptr->op == (NA) ) )
    {
      dsatsmsi_pres_cmd = SMS_CNMI;
      result = dsat707smsi_route_config ( dsat707sms_qcnmi_val[1] ); 
      if(result != DSAT_ASYNC_CMD)
      {
        dsatsmsi_pres_cmd = SMS_NONE;
      }
    }
  }

  return result;
} /* dsat707sms_exec_qcnmi_cmd */


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGS command.
  $CMGS is used to send a message from TE to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgs_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
#ifdef FEATURE_DATA_SERIALIZER
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
#endif /* FEATURE_DATA_SERIALIZER */
  dsat_result_enum_type result = DSAT_OK;

/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcmgs_cmd, op:%d", tok_ptr->op,0,0);
        );

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if(dsat707sms_qcmgf_val)
    {
      /* Retain the parameters from the this command */
      /* they will be used later when sending the message */
      /* In case of text mode, <da> is one of the parameter */
      /* We don't support pdu mode */
      /* get the send address */
      byte * toda = NULL;
      if( tok_ptr->args_found > 2 )
      {
        MSG_ERROR("More then two Args in QCMGS",0,0,0);
        return DSAT_ERROR;
      }
      /*text mode*/
      /* If user specified a <da>, store it now. It will be used later */
  
      /* Strip out the '"' from the <da> */
      if(*tok_ptr->arg[0] != '\0' )
      {
        if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0] ) )
        {
          /* We got an address without quotes */
          result = DSAT_ERROR;
        }
        if ( std_strlen((const char *)tok_ptr->arg[0]) > WMS_GW_ADDRESS_MAX )
        {
          MSG_HIGH("<oa> length greater than 20 not supported",0,0,0);
          return DSAT_ERROR;
        }

      }
      else
      {
        /* <da> is null */
        result = DSAT_ERROR;
      }
      
      /* continue processing if there is not error */
      if(result == DSAT_OK)
      {
        if ( tok_ptr->args_found == 2 )
        {
          toda = tok_ptr->arg[1];
        }
        memset( &send_addr, 0x0, sizeof(wms_address_s_type) );
        if(!dsatsmsi_str_to_addr
           (&send_addr, (byte*)tok_ptr->arg[0], toda, DSATSMS_TEXT_MODE)
           )
        {
          /* We got an error in address preocessing */
          res_buff_ptr->used = (uint16) dsatsmsi_cms_error(
                               res_buff_ptr, (int)DSAT_CMS_INVALID_TXT_PARAM);
          result =  DSAT_CMD_ERR_RSP;
        }
        else
        {
          /* Send mesasge to SIO data preprocessor to change state to */
          /* handle SMS message entry */
  #ifdef FEATURE_DATA_SERIALIZER
          dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
  #else
          dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE );
  #endif 
  
          dsatsmsi_pres_cmd = SMS_CMGS;
          result = DSAT_ASYNC_CMD;
        }
      } /* ( result == DSAT_OK ) */
    }
    else
    {
      /* PDU Mode Not Supported */
      MSG_ERROR("PDU Mode not supported in Cmd %d",dsatsmsi_pres_cmd,0,0);
      return DSAT_ERROR;
    }
  } /* ( tok_ptr->op == (NA|EQ|AR) ) */

  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707sms_exec_qcmgs_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$CMGR command.
  $CMGR is used to read a sms message.

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
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgr_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type         result;
  wms_memory_store_e_type       mem_store;
  wms_status_e_type             msg_status;
  dsat_num_item_type            index;

/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcmgr_cmd, op:%d", tok_ptr->op,0,0);
        );

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0],
                                       CPMS_MEM1);
    if( tok_ptr->args_found > 1 )
    {
      MSG_ERROR("More then one Arg in QCMGR",0,0,0);
      return DSAT_ERROR;
    }
    if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
    {
      /* We got a bad index */
      return DSAT_ERROR;
    }
    dsatsmsi_pres_cmd = SMS_CMGR;
    msg_status = wms_msg_read(dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              mem_store,
                              index
                              );

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error, format a +CMS ERROR */
      res_buff_ptr->used += (uint16) dsatsmsi_cms_error(res_buff_ptr,
                                           (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
      dsatsmsi_pres_cmd = SMS_NONE;
    }
  }

  /*---------------------------  
   Processing READ command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|QU) )
  {
    /* It is an error to query this command */
    result = DSAT_ERROR;
  }

  /*--------------------------------------
   Processing a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707sms_exec_qcmgr_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGL command. $QCMGL command lists
  all the SMS saved in the loaction.
  
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
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgl_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type result;
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  byte *data_ptr;
  dsat_string_item_type txt_tststr [] = 
  "(\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")";
  int all_flag=1;


/*-------------------------------------------------------------------------*/

  DEBUG( MSG_MED("dsat707sms_exec_qcmgl_cmd, op:%d", tok_ptr->op,0,0);
        );
        
        
   data_ptr = res_buff_ptr->data_ptr;
   
   mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0],
                                          CPMS_MEM1);
   
  /*-------------------------------------  
   Processing WRITE and DEFAULT command :
   -------------------------------------*/
  if ( ( tok_ptr->op == (NA|EQ|AR) ) || ( tok_ptr->op == (NA) ) )
  {
    mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0],
                                         CPMS_MEM1);
    dsatsmsi_pres_cmd = SMS_CMGL;
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      if( tok_ptr->args_found > 1 )
      {
        MSG_ERROR("More then one Args in QCMGL",0,0,0);
        return DSAT_ERROR;
      }
      tag = stat707_lookup(tok_ptr->arg[0], &all_flag);
      if ( !tag )
      {
        /* Bad Tag */
        /* Return an error */
        MSG_ERROR("Bad tag input in %d cmd ",dsatsmsi_pres_cmd,0,0);
        dsatsmsi_pres_cmd = SMS_NONE;
        return DSAT_ERROR;
      }
      else
      {
        dsatsmsi_cmd_sm.stat_list[0] = tag;
      }
    }
    else
    {
      /* We are in NA case */
      tag = WMS_TAG_MT_NOT_READ;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_NOT_READ;
    }
    
  
    if (!all_flag)
    {
      /* We got an ALL flag */
      dsatsmsi_cmd_sm.num_of_stat = 4;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MT_NOT_READ;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_NOT_SENT;
    }
    else
    {
      /* We are listing a single stat */
      dsatsmsi_cmd_sm.num_of_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.present_stat = 1;
    }
  
    msg_status = wms_cfg_get_message_list(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void *)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            tag
                                           );
    if(msg_status == WMS_OK_S )
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      dsatsmsi_pres_cmd = SMS_NONE;
      result = DSAT_ERROR;
    }
  
    return result;
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word)std_strlprintf((char*)data_ptr,
                                              res_buff_ptr->size,
                                              "%s: ",
                                              tok_ptr->name); 
    data_ptr += res_buff_ptr->used;
  

    res_buff_ptr->used += (word)std_strlprintf ( (char*)data_ptr,
                                      res_buff_ptr->size - res_buff_ptr->used,
                                      "%s", txt_tststr);

    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707sms_exec_qcmgl_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMSS command.
  $QCMSS is used to send a message already stored from Memory to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result; 

  /* data structures used in wms_msg_send */
  wms_send_mode_e_type send_mode = WMS_SEND_MODE_MEMORY_STORE;
  wms_status_e_type msg_status = WMS_OK_S;
  wms_memory_store_e_type mem_store;
  dsm_item_type *buff_ptr = res_buff_ptr;
  atoi_enum_type err_type = ATOI_OK;
  dsat_num_item_type index =0 ;
  
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( tok_ptr->args_found > 3 )
    {
      MSG_ERROR("More then three Args in QCMSS",0,0,0);
      return DSAT_ERROR;
    }
    /* Case when we have to just send the message from specified index */
    if( tok_ptr->args_found == 1 )
    {
      memset( &is707_client_msg, 0, sizeof(wms_client_message_s_type) );
      /* get the memory from which the message has to be picked up */
      is707_client_msg.msg_hdr.mem_store = 
        dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[1], CPMS_MEM2);
    
      /*convert the index into a suitable number */
      err_type = dsatutil_atoi(&index, tok_ptr->arg[0], 10);
      if (err_type != ATOI_OK)
      {
        return DSAT_ERROR;
      }

      is707_client_msg.msg_hdr.index = index;
    
      /* fill out rest of the enums */
      is707_client_msg.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      is707_client_msg.msg_hdr.tag = WMS_TAG_NONE;
      /* We only have index, assume SMS-SUBMIT */
      is707_client_msg.u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
      send_mode = WMS_SEND_MODE_MEMORY_STORE;

      /* finally we are done stuffing all the required parameters */
      /* send the message to WMS                                */
      dsatsmsi_pres_cmd = SMS_CMSS;
      msg_status = wms_msg_send(
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 send_mode,
                                 &is707_client_msg
                                );
      if (msg_status  != WMS_OK_S)
      {
        MSG_ERROR("Error sending the SMS",0,0,0);
        dsatsmsi_pres_cmd = SMS_NONE;
        return DSAT_ERROR;
      }
    }
    /* We have to overwrite the <da> */
    /* Read the message first        */
    else
    {
      /* Save the <da>                                                      */
      /* It will be used to replace existing <da> after we read the message */

      memset( (void *) &send_addr, 0, sizeof(wms_address_s_type));

      if( tok_ptr->args_found >= 2 )
      {
        if( VALID_TOKEN(1) )
        {
          /* Strip out the '"' from the <da> in place */
          if ( !dsatsmsi_strip_quotes( tok_ptr->arg[1], tok_ptr->arg[1] ) )
          {
            /* We got an address with out quotes */
            return DSAT_ERROR;
          }
          if ( std_strlen((const char *)tok_ptr->arg[1]) > WMS_GW_ADDRESS_MAX )
          {
            MSG_HIGH("<oa> length greater than 20 not supported",0,0,0);
            return DSAT_ERROR;
          }
        }
        else
        {
          return DSAT_ERROR;
        }
      }

      switch(tok_ptr->args_found)
      {
        /* Note: <toda> is always decimal number */
        case 2:
          if(!dsatsmsi_str_to_addr(&send_addr,tok_ptr->arg[1],
                                              NULL,DSATSMS_TEXT_MODE))
          {
            /* Bad Address */
            return DSAT_ERROR;
          }
          break;
      
        case 3:
          if (!dsatsmsi_str_to_addr( &send_addr, tok_ptr->arg[1], 
                                     tok_ptr->arg[2], DSATSMS_TEXT_MODE )
             )
          {
            /* Bad Address */
            return DSAT_ERROR;
          }
          break;

        default:
          /* we shouldn't be here */
          break;
      }
      /* <mem1> is used for reading the SMS */
      mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0],
                                          CPMS_MEM1);
    
      if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
      {
        /* We got a bad index */
        return DSAT_ERROR;
      }
      /* Read the message */
      /* once we get the message back from the WMS */
      /* Overwrite the present <da> with the new one (dsatetsismsi_send_addr) */
      dsatsmsi_pres_cmd = SMS_CMSS;
      msg_status = wms_msg_read(
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                mem_store,
                                index
                               );
    }

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      dsatsmsi_pres_cmd = SMS_NONE;
      result = DSAT_ERROR;
    }
    return result;
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsat707sms_exec_qcmss_cmd */

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGD command.
  +CMGD is used to delete sms messages from <mem1>.

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
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgd_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *rb_ptr                 /* Place to put response       */
)
{
  dsat_result_enum_type result;
  dsm_item_type *buff_ptr;
  wms_memory_store_e_type mem_store;
  wms_status_e_type msg_status;
  unsigned int used_val;
/*-------------------------------------------------------------------------*/

  DEBUG(
        MSG_MED("dsat707sms_exec_qcmgd_cmd, op:%d", tok_ptr->op,0,0);
        );

  buff_ptr = rb_ptr;
  
  mem_store = dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[0],
                                     CPMS_MEM1);
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_num_item_type index;
    if( tok_ptr->args_found == 1 )
    {
      if(dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK )
      {
        /* We received a bad index */
        return DSAT_ERROR;
      }
      dsatsmsi_pres_cmd = SMS_CMGD;
      dsatsmsi_cmd_sm.num_of_stat = 0;
      dsatsmsi_cmd_sm.present_index = 0;
      dsatsmsi_cmd_sm.num_of_indices = 0;
      dsatsmsi_cmd_sm.is_test =  FALSE;
      dsatsmsi_cmd_sm.one_index = TRUE;
      msg_status = wms_msg_delete(
                                    dsatsmsi_client_id,
                                    dsatsmsi_cmd_cb_func,
                                    (void*)&dsatsmsi_pres_cmd,
                                    mem_store,
                                    index
                                   );
      if (msg_status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        dsatsmsi_pres_cmd = SMS_NONE;
        buff_ptr->used += dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
        result = DSAT_CMD_ERR_RSP;
      }
      return result;
    }
    else if ( tok_ptr->args_found == 2 )
    {
      dsatsmsi_pres_cmd = SMS_CMGD;
      result = dsatsmsi_delete_messages(tok_ptr->arg[1]);
      if (result == DSAT_OK)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        dsatsmsi_pres_cmd = SMS_NONE;
        used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
        buff_ptr->used += (byte)used_val;
        result = DSAT_CMD_ERR_RSP;
      }
      return result;
    }
    else
    {
      /* Wrong number of arguments */
      result = DSAT_ERROR;
    }
  } /* Write command */
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    dsatsmsi_pres_cmd = SMS_CMGD;
    msg_status = wms_cfg_get_message_list(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            (void*)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            WMS_TAG_NONE
                                           );

    dsatsmsi_cmd_sm.is_test =  TRUE;
    dsatsmsi_cmd_sm.one_index = FALSE;

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error, format a +CMS ERROR */
      buff_ptr->used += dsatsmsi_cms_error(buff_ptr, 
                                  (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
      dsatsmsi_pres_cmd = SMS_NONE;
    }
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsat707sms_exec_qcmgd_cmd */ 

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_QCMGW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT$QCMGW command. 
  $QCMGW is used to store message to memory storage <mem2>

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DATA_SERIALIZER
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
#endif /* FEATURE_DATA_SERIALIZER */

  /*-------------------------
   Processing WRITE command:
   --------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( dsat707sms_qcmgf_val )
    {

      /* convert the <da> to suitable a data structure */

      /* Strip out the '"' from the <da> in place */
      if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0] ) )
      {
        return DSAT_ERROR;
      }
      if ( std_strlen((const char *)tok_ptr->arg[0]) > WMS_GW_ADDRESS_MAX )
      {
        MSG_HIGH("<oa> length greater than 20 not supported",0,0,0);
        return DSAT_ERROR;
      }
      switch ( tok_ptr->args_found )
      {
        /* no <stat> given */
        case 1:
        case 2:
        {
          byte * toa_ptr = NULL;
          if(tok_ptr->args_found == 2)
          {
            toa_ptr = tok_ptr->arg[1];
          }

          if(!dsatsmsi_str_to_addr
             (&send_addr, tok_ptr->arg[0], toa_ptr, DSATSMS_TEXT_MODE)
            )
          {
            /* We got a bad address */
            result = DSAT_CMD_ERR_RSP;
          }
          
        }
        break;

        /* <stat> given */
        case 3:
          if ( !dsatsmsi_str_to_addr ( 
                                        &send_addr, tok_ptr->arg[0],
                                        tok_ptr->arg[1],
                                        DSATSMS_TEXT_MODE
                                      ) )
          {
            /* We got a bad address */
            result = DSAT_CMD_ERR_RSP;
          }
          /* find out what type of STAT we are about to store */
          else if((dsat707smsi_send_stat = stat707_lookup(tok_ptr->arg[2],NULL))
                    == WMS_TAG_NONE ) 
          {
            /* We got a bad <stat> */
            result = DSAT_CMD_ERR_RSP;
          }
          break;
                            
        default:
          /* We shouldn't be here    */
          /* Bad number of arguments */
          result = DSAT_CMD_ERR_RSP;
          break;
      }

      if ( result == DSAT_CMD_ERR_RSP )
      {
        /* We got an error in processing PDU */
        res_buff_ptr->used = 
          (byte) dsatsmsi_cms_error (res_buff_ptr,
                                    (int)DSAT_CMS_INVALID_TXT_PARAM );
        return result;
      }

    } /* if(text mode) */
    else
    {
      /* PDU Mode Not Supported */
      return DSAT_ERROR;
    }

    /* Send mesasge to SIO data preprocessor to change state to */
    /* handle SMS message entry */
#ifdef FEATURE_DATA_SERIALIZER
    dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
#else
    dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE );
#endif 

    dsatsmsi_pres_cmd = SMS_CMGW;
    result = DSAT_ASYNC_CMD;
    return result;
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    res_buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
    return result;
  }
  /*------------------------------------ 
   Processing command with no arguments:
   -------------------------------------*/
  if ( tok_ptr->op == (NA) )
  {
    /* This is valid only in text mode */
    if( dsat707sms_qcmgf_val )
    {
      /* We didn't get a <da> from user */
      /* Set the LOCAL structure to 0   */
      memset( (void *) &dsat707smsi_send_addr, 0, sizeof(wms_address_s_type));

      /* stat is not given default it */
      dsat707smsi_send_stat = WMS_TAG_MO_NOT_SENT;
      

      /* Send mesasge to SIO data preprocessor to change state to */
      /* handle SMS message entry */
#ifdef FEATURE_DATA_SERIALIZER
      dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
#else
      dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE );
#endif 

      dsatsmsi_pres_cmd = SMS_CMGW;
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      result = DSAT_ERROR;
    }
    return result;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsat707sms_exec_qcmgw_cmd */



/*===========================================================================

FUNCTION DSAT707SMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat707sms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  boolean result = FALSE;
  if(dsat707sms_qcmgf_val)
  {
    result = is707_send_smstxt_msg ( msg, len );
  }
  else
  {
    /* PDU Mode not Supported */
    MSG_ERROR("PDU Mode not Supported",0,0,0);
    dsatsms_abort_cmd (DSAT_CMS_OP_NOT_SUPPORTED);
  }
  if(!result)
  {
    /* We got an error while processing message   */
    /* Send an ASYNC Event to tell ATCOP about it */
    dsatsms_abort_cmd (DSAT_CMS_ME_FAILURE);
  }

}/* dsat707sms_send_sms_msg */

/*************** the local functions ***************************************/

/*===========================================================================

FUNCTION IS707_SEND_SMSTXT_MSG

DESCRIPTION
  Local function that is used to send TEXT message.

DEPENDENCIES

RETURN VALUE
  Returns a boolean.
  TRUE  : If the message has been sent successfully
  FALSE : If there is an error in sending the message
SIDE EFFECTS

===========================================================================*/
LOCAL boolean is707_send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the txt message to be sent */
  int   len       /* Len of the txt message to be sent */
)
{
  wms_send_mode_e_type        send_mode;
  wms_write_mode_e_type       write_mode;
  wms_client_msg_hdr_s_type * msg_hdr_ptr = NULL;
  wms_cdma_message_s_type   * cdma_msg_ptr = NULL;
  wms_client_bd_s_type      * cdma_bd_ptr = NULL;
  wms_client_ts_data_s_type   cl_ts_data ;
  wms_status_e_type           wms_status = WMS_OK_S;
  boolean                     result = FALSE;

/*-------------------------------------------------------------------------*/

  MSG_HIGH("In is707_send_smstxt_msg : due to Command %d", 
                                  dsatsmsi_pres_cmd ,0,0 );

  send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  write_mode = WMS_WRITE_MODE_INSERT; 

  memset(&is707_client_msg, 0, sizeof(wms_client_message_s_type) );

  msg_hdr_ptr = &is707_client_msg.msg_hdr;
  msg_hdr_ptr->message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_hdr_ptr->tag = WMS_TAG_MO_NOT_SENT;
  msg_hdr_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  msg_hdr_ptr->index = (wms_message_index_type) WMS_DUMMY_MESSAGE_INDEX;

  cdma_msg_ptr = &is707_client_msg.u.cdma_message;
  cdma_msg_ptr->is_mo = TRUE;
  
  if( dsat707sms_qcsmp_val[0].num_item < 8 )
  {
    cdma_msg_ptr->teleservice = dsat707smsi_teleservice_num_to_enum(
      (char *) qcsmp_tid_valstr[dsat707sms_qcsmp_val[0].num_item]);
  }
  else
  {
    return FALSE;
  }

  /* Fill out the origination address from the first 
     argu of cmgs */
  cdma_msg_ptr->address = send_addr;

  /* we don't support subaddress for now */
  cdma_msg_ptr->subaddress.number_of_digits = 0;

  /* we don't support message ack req option for now */
  cdma_msg_ptr->is_tl_ack_requested = FALSE;

  /* this is point to point submit message, not a service message */
  cdma_msg_ptr->is_service_present = FALSE;

  /*------------------------------------------------------------------*/
  /* fill out the data field using wms_ts_encode_CDMA_bd
     according to Table 4.4.2-1. of TIA/EIA-637B */

  memset(&cl_ts_data, 0, sizeof(wms_client_ts_data_s_type));
  cl_ts_data.format = WMS_FORMAT_CDMA;
  cdma_bd_ptr = &cl_ts_data.u.cdma;
  
  /*- Mesage Identifier ----------------------------------------------*/
  cdma_bd_ptr->message_id.type = WMS_BD_TYPE_SUBMIT;
  /* Leave cdma_bd_ptr->message_id.id_number as it is, wms will fill 
     it out when sending it over the air */
#ifdef FEATURE_CDSMS_UDH
  /* don't support User Data Header in User Data Subparameter 
     for now */
  cdma_bd_ptr->message_id.udh_present = FALSE;
#endif /* FEATURE_CDSMS_UDH */
  cdma_bd_ptr->mask = WMS_MASK_BD_MSG_ID;

  /*- User Data ------------------------------------------------------*/
  cdma_bd_ptr->user_data.encoding = WMS_ENCODING_ASCII;
  /* we don't support is91ep for now */
  if(len >= WMS_CDMA_USER_DATA_MAX )
  {
    MSG_ERROR("len %d  >= WMS_CDMA_USER_DATA_MAX ", len,0,0);
    return FALSE;
  }
  txt_ptr[len] = '\0';
  cdma_bd_ptr->user_data.data_len = (uint8) len;
  cdma_bd_ptr->user_data.padding_bits = 0;
  (void) std_strlcpy((char *) cdma_bd_ptr->user_data.data, 
          (char *) txt_ptr, 
           cdma_bd_ptr->user_data.data_len + 1);
  /* using 8-bit chars number of digits is equal to the lenght in bytes  */
  cdma_bd_ptr->user_data.number_of_digits = (uint8) len;
  cdma_bd_ptr->mask |= WMS_MASK_BD_USER_DATA;

  /*- Valid Period ---------------------------------------------------*/
  if (dsat707sms_qcsmp_val[2].string_item[0] != '\0')
  {
    if(dsat707sms_qcsmp_val[1].num_item == 0)
    {
      /* absolute format */
      (void) dsat707smsi_string_to_timestamp(
               (char *) dsat707sms_qcsmp_val[2].string_item,
               &cdma_bd_ptr->validity_absolute);
      cdma_bd_ptr->mask |= WMS_MASK_BD_VALID_ABS;
    }
    else
    {
      /* relative format */
      /* fixme wms_ts_decode_relative_time has minor problem in 
         CDMA mode */
      uint8 rela_time = (uint8) atoi((char *) dsat707sms_qcsmp_val[2].string_item);
      wms_ts_decode_relative_time(rela_time, &cdma_bd_ptr->validity_relative);
      cdma_bd_ptr->mask |= WMS_MASK_BD_VALID_REL;
    }
  }

  /*- Deferred Delivery Time -----------------------------------------*/
  if (dsat707sms_qcsmp_val[4].string_item[0] != '\0')
  {
    if(dsat707sms_qcsmp_val[3].num_item == 0)
    {
      /* absolute format */
      (void) dsat707smsi_string_to_timestamp(
               (char *) dsat707sms_qcsmp_val[4].string_item,
               &cdma_bd_ptr->deferred_absolute);
      cdma_bd_ptr->mask |= WMS_MASK_BD_DEFER_ABS;
    }
    else
    {
      /* relative format */
      /* fixme wms_ts_decode_relative_time has minor problem in 
         CDMA mode */
      uint8 rela_def = (uint8) atoi((char *) dsat707sms_qcsmp_val[4].string_item);
      wms_ts_decode_relative_time(rela_def, &cdma_bd_ptr->deferred_relative);
      cdma_bd_ptr->mask |= WMS_MASK_BD_DEFER_REL;
    }
  }

  /*- Priority Indicator ---------------------------------------------*/
  cdma_bd_ptr->priority = WMS_PRIORITY_NORMAL;
  cdma_bd_ptr->mask |= WMS_MASK_BD_PRIORITY;

  /*- Privacy Indicator ----------------------------------------------*/
  cdma_bd_ptr->privacy  = WMS_PRIVACY_NORMAL;
  cdma_bd_ptr->mask |= WMS_MASK_BD_PRIVACY;

  /*- Reply Option ---------------------------------------------------*/
  cdma_bd_ptr->reply_option.user_ack_requested = FALSE;
  cdma_bd_ptr->reply_option.delivery_ack_requested = FALSE;
  cdma_bd_ptr->mask |= WMS_MASK_BD_REPLY_OPTION;

  /*- Alert on Message Delivery --------------------------------------*/
  cdma_bd_ptr->alert_mode = WMS_ALERT_MODE_ON;
  cdma_bd_ptr->mask |= WMS_MASK_BD_ALERT;

  /*- Language Indicator ---------------------------------------------*/
  cdma_bd_ptr->language = WMS_LANGUAGE_ENGLISH;
  cdma_bd_ptr->mask |= WMS_MASK_BD_LANGUAGE;

  /*- Call-Back Number -----------------------------------------------*/
  /* don't support call back number for now */

  /*- Multiple Encoding User Data ------------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  /*- Message Deposit Index ------------------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  /*- Service Category Program Results -------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  if (dsatsmsi_pres_cmd == SMS_CMGW)
  {
    msg_hdr_ptr->mem_store = 
      dsatsmsi_memory_lookup((int)dsat707sms_qcpms_val[1], CPMS_MEM2);
    if( dsat707smsi_send_stat != WMS_TAG_NONE )
    {
      msg_hdr_ptr->tag = dsat707smsi_send_stat;
    }
    else
    {
      msg_hdr_ptr->tag = WMS_TAG_MO_NOT_SENT;
    }
  }

  /* Pack the User Payload to Client TS data*/
  cdma_bd_ptr->user_data.number_of_digits = 
    wms_ts_pack_ascii((char *)txt_ptr,
                        &(cl_ts_data.u.cdma.user_data.data[0]),
                        &(cdma_bd_ptr->user_data.data_len), 
                        &(cdma_bd_ptr->user_data.padding_bits));

  /* Encode the Client Ts data to Raw Ts data */
  wms_status = wms_ts_encode(&cl_ts_data,
                             &cdma_msg_ptr->raw_ts);
  if(wms_status == WMS_OK_S)
  {
    if (dsatsmsi_pres_cmd == SMS_CMGS)
    {
      wms_status = wms_msg_send(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              send_mode,
                              &is707_client_msg
                              );
      if(wms_status == WMS_OK_S)
      {
        result = TRUE;
      }
    }
    else if (dsatsmsi_pres_cmd == SMS_CMGW)
    {
      wms_status = wms_msg_write(
                               dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               write_mode,
                               &is707_client_msg
                               );
      if(wms_status == WMS_OK_S)
      {
        result = TRUE;
      }
    }
  }

  /* Clean up the state variables */
  dsat707smsi_send_stat = WMS_TAG_NONE;
  return result;
} /* is707_send_smstxt_msg */

/*===========================================================================

FUNCTION STAT707_LOOKUP

DESCRIPTION
  Converts a string parameter to a TAG type ( wms_message_tage_e_type ).
  It also checks to see if the "ALL" wild card is being used as a stat.
  If so then it sets the pointer to the flag

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_message_tage_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL wms_message_tag_e_type stat707_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 int  * all_flag  /* Ptr for "ALL" FLAG      */
)
{
  wms_message_tag_e_type tag;
  byte tag_read[] = "\"REC READ\"";
  byte tag_unread[] = "\"REC UNREAD\"";
  byte tag_sent[] = "\"STO SENT\"";
  byte tag_unsent[] = "\"STO UNSENT\"";
  byte tag_all[] = "\"ALL\"";
  byte s[32] = {0};

  /* Check to see if the stat is wrapped in quotes */
  if( !dsatsmsi_strip_quotes ( stat_val, s ) )
  {
    return WMS_TAG_NONE;
  }

  if ( *s == '\0' )
  {
    tag = WMS_TAG_MO_NOT_SENT;
  }
  else if(!dsatutil_strcmp_ig_sp_case(stat_val,tag_unread))
  {
    tag = WMS_TAG_MT_NOT_READ;
  }
  else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_read))
  {
    tag = WMS_TAG_MT_READ;
  }
  else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_unsent))
  {
    tag = WMS_TAG_MO_NOT_SENT;
  }
  else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_sent))
  {
    tag = WMS_TAG_MO_SENT;
  }
  else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_all))
  {
    /* Valid only in CMGL */
    if ( dsatsmsi_pres_cmd == SMS_CMGL )
    {
      tag = WMS_TAG_MT_READ;
      if ( all_flag != NULL )
      {
        *all_flag = 0;
      }
    }
    else
    {
      tag = WMS_TAG_NONE;
    }
  }
  else
  {
    tag = WMS_TAG_NONE;
  }

  return tag;
}/* stat707_lookup */

#endif /* FEATURE_CDMA_SMS */


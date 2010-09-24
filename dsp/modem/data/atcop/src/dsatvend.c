/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                V E N D O R   S P E C I F I C

                A T    C O M M A N D   
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It implements execution of common 
  vendor specific AT commands.

EXTERNALIZED FUNCTIONS

  dsatvend_exec_qcclr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcclr command.
    at$qcclr command is used for clearing mobile error log.

  dsatvend_exec_qcdmg_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmg command.
    at$qcdmg is used to transition the serial port to diagnostic
    monitor.

  dsatvend_exec_qcdmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmr command.
    at$qcdmr is used to set the DM baud rate.

  dsatvend_exec_qcdom_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdom command.
    at$qcdom is used to set domain name address.

  dsatvend_exec_qcdnsp_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnsp command.
    at$qcdnsp is used to set primary DNS IP address.

  dsatvend_exec_qcdnss_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnss command.
    at$qcdnss is used to set secondary DNS IP address.

  dsatvend_exec_qcter_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcter command.
    at$qcter is used to set TE-DCE baud rate.

  dsatvend_exec_brew_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$brew command.
    at$brew is used to transition control from ATCOP to BREW COP

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.c_v   1.3   11 Oct 2002 10:27:58   sbandaru  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatvend.c#1 $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/07   sn      Replaced banned string APIs.
10/18/07   sn      Added FEATURE_DATA_SERIALIZER check for function 
                   diagcomm_sio_qcdmg().
12/06/02   ak      Removed include of nv_items.h under UNFINISHED, and
                   instead include nv.h
10/09/02   sb      ERROR check to see if it is a valid baud in $QCTER
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
12/13/01   sb      Changed the rdm_data_got_at$qcdmg() function name to 
                   rdm_data_got_atqcdmg()
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
#include "AEEstd.h"
#include "dstaski.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvend.h"
#include "msg.h"
#include "err.h"
#include "diag.h"
#include "nv.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#include "diagcomm.h"
#endif

#ifdef FEATURE_JCDMA_1X
  #include "ds707_jcdma_m51.h"
#endif

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


LOCAL void clr_err_log( void);


/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcclr command.
  at$qcclr command is used for clearing mobile error log.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcclr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
    clr_err_log();
    result = DSAT_OK;
  }
  /*--------------------------------------
  Standard processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    /*  Code at top will output                         */
    /*  the command name for at TEST command.           */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_qcclr_cmd () */


/*===========================================================================
FUNCTION CLR_ERR_LOG

DESCRIPTION
  This function clears the NV error log whenever the command AT$QCCLR is
  issued.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void clr_err_log( void)
{
  byte i;
  
  /*-----------------------------------------------------------------------
    Loop through all entries in the error log and clear them.
  -------------------------------------------------------------------------*/
  for (i=0; i < NV_MAX_ERR_LOG; i++ )
  {
    (void) err_clr_log( i);
  }

} /* clr_err_log () */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMG_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmg command.
  at$qcdmg is used to transition the serial port to diagnostic
  monitor.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the serial port to Diagnostic Monitor.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcdmg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* function return                    */
  dsat_result_enum_type result;

  /*result for diagcomm_sio_qcdmg       */
  boolean               qcdmg_ret = FALSE;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
      MSG_HIGH ("DM GO", 0, 0, 0);
      
      /*----------------------------------------------------
      Schedule a mode change to DM, but allow "OK" to be
      sent at 19200 Baud.
      ----------------------------------------------------*/
#ifdef FEATURE_RUNTIME_DEVMAP
#ifndef FEATURE_DATA_SERIALIZER
      qcdmg_ret = diagcomm_sio_qcdmg();
#endif
#endif
      if(qcdmg_ret)
      {
        result = DSAT_DO_DM;
      }
      else
      {
        result = DSAT_ERROR;
      }
  }
  /*--------------------------------------
  Processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    /*  Code at top will output                         */
    /*  the command name for at TEST command.           */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_qcdmg_cmd () */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmr command.
  at$qcdmr is used to set the DM baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdmr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  
  result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
  if ((DSAT_OK == result) && 
      ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  { 
#if defined(FEATURE_DIAG_DS_SIO_COUPLING)
    diag_record_dm_baudrate ( (sio_bitrate_type) dsat_qcdmr_val );
#endif /* defined(FEATURE_DIAG_DS_SIO_COUPLING) */
  }

  return result;
}  /* dsatvend_exec_qcdmr_cmd() */

/*  ------------------------------------------------------------------  */

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnsp command.
  at$qcdnsp is used to set primary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  char rbuffer[32];
  int ctr = 0;

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  
  if (DSAT_OK == result) 
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /*---------------------------------------------------------------
      Check for validity of the IP address & convert to internal format
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_format_ipaddr (STRING_TO_UINT, 
                                             (char*)parse_table->val_ptr, 
                                             &dsat_qcdns_val[0],
                                             FALSE))
      {
        /*---------------------------------------------------------------
        Write Primary DNS item into the NV 
        ---------------------------------------------------------------*/
        ds_nv_item.primary_dns  = dsat_qcdns_val[0];
        (void) dsi_put_nv_item( NV_PRIMARY_DNS_I, &ds_nv_item );
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA))
    {
      result = DSAT_OK;
    }
    else if (tok_ptr->op == (NA|QU))
    {
      /*---------------------------------------------------------------
      Format NV data to 4-tuple string
      ---------------------------------------------------------------*/
      memset (dsat_qcdnspri_val, 0, 16);
      if (DSAT_OK == dsatutil_format_ipaddr (UINT_TO_STRING, 
                                             (char*)dsat_qcdnspri_val,
                                             &dsat_qcdns_val[0],
                                             FALSE))
      {
        /* Use extended format on value query */
        ctr = std_strlprintf ((char*)rbuffer, sizeof(rbuffer), "%s: %s", 
                              (char*)parse_table->name, 
                              (char*)dsat_qcdnspri_val);

        (void) std_strlcpy((char*)res_buff_ptr->data_ptr, (const char*)rbuffer, 
                           res_buff_ptr->size);
        res_buff_ptr->used += (unsigned short)ctr;
        res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\0';
      
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      (void) std_strlcpy((char*)res_buff_ptr->data_ptr, 
                         (const char*)parse_table->name, 
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((const char*)parse_table->name);
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ':';
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\0';
      
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  return result;
}  /* dsatvend_exec_qcdnsp_cmd() */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnss command.
  at$qcdnss is used to set secondary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  char rbuffer[32];
  int ctr = 0;
  
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if (DSAT_OK == result)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /*---------------------------------------------------------------
      Check for validity of the IP address & convert to internal format
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_format_ipaddr (STRING_TO_UINT, 
                                             (char*)parse_table->val_ptr,
                                             &dsat_qcdns_val[1],
                                             FALSE))
      {
        /*---------------------------------------------------------------
        Write Primary DNS item into the NV 
        ---------------------------------------------------------------*/
        ds_nv_item.secondary_dns  = dsat_qcdns_val[1];
        (void) dsi_put_nv_item( NV_SECONDARY_DNS_I, &ds_nv_item );
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA))
    {
      result = DSAT_OK;
    }
    else if (tok_ptr->op == (NA|QU))
    {
      /*---------------------------------------------------------------
      Format NV data to 4-tuple string
      ---------------------------------------------------------------*/
      memset (dsat_qcdnssec_val, 0, 16);
      if (DSAT_OK == dsatutil_format_ipaddr (UINT_TO_STRING, 
                                             (char*)dsat_qcdnssec_val,
                                             &dsat_qcdns_val[1],
                                             FALSE))
      {
        /* Use extended format on value query */
        ctr = std_strlprintf ((char*)rbuffer, sizeof(rbuffer), "%s: %s", 
                              (char*)parse_table->name, 
                              (char*)dsat_qcdnssec_val);

        (void) std_strlcpy((char*)res_buff_ptr->data_ptr, (const char*)rbuffer, 
                           res_buff_ptr->size);
        res_buff_ptr->used += (unsigned short)ctr;
        res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\0';
        
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      (void) std_strlcpy((char*)res_buff_ptr->data_ptr, 
                         (const char*)parse_table->name, 
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((const char*)parse_table->name);
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ':';
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\0';
      
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  return result;
} /* dsatvend_exec_qcdnss_cmd() */



/*===========================================================================
FUNCTION DSATVEND_INIT_DNS_FROM_NV

DESCRIPTION
  This function reads Primary DNS and Secondary DNS from the NV during
  initialization. If these items were never written in NV, the values are
  made as NULL.
  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvend_init_dns_from_nv
(
  void
)
{
  nv_stat_enum_type status;
  dsat_qcdns_val[0] = 0;
  dsat_qcdns_val[1] = 0;

  /*-------------------------------------------------------------------------
    Get Primary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_PRIMARY_DNS_I, &ds_nv_item );
  if ( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
     -----------------------------------------------------------------------*/
    dsat_qcdns_val[0] = ds_nv_item.primary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    MSG_ERROR( "Bad NV read status %d for Pri DNS NV", status, 0, 0 );
  }

  /*-------------------------------------------------------------------------
    Get Secondary DNS IP addresses from NV. Look at the status returned. If
    status !=NV_DONE_S, then either NV was never written or it cannot be
    reused. Make IP addresses as NULL if NV was never written.
  -------------------------------------------------------------------------*/
  
  status = dsi_get_nv_item( NV_SECONDARY_DNS_I, &ds_nv_item );
  if( status == NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      Store number retrieved from NV.
     -----------------------------------------------------------------------*/
    dsat_qcdns_val[1] = ds_nv_item.secondary_dns;
  }
  else if ( status == NV_NOTACTIVE_S )
  {
    /* NV item not populated; use default */
  }
  else 
  {
    MSG_ERROR( "Bad NV read status %d for Sec. DNS NV", status, 0, 0 );
  }
} /* dsatvend_init_dns_from_nv( ) */



/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcter command.
  at$qcter is used to set current and default TE-DCE baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcter_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type    old_qcter_val;

  old_qcter_val = dsat_qcter_val; /* Retain old value for error checking */
  
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  
  if (DSAT_OK == result) 
  {
    if (tok_ptr->op == (NA|EQ|AR)) 
    { 
      if (sio_is_baud_valid ((sio_bitrate_type)dsat_qcter_val) == FALSE)
      {
        /* Baudrate not allowed, tell user "ERROR" */
        dsat_qcter_val = old_qcter_val;
        result = DSAT_ERROR;
      }
      else
      {
#ifdef FEATURE_JCDMA_1X
        /*--------------------------------------------------------------------
          Note that this if statement does the function call internally, which
          will result in the baud rate being changed, if the passed in 
          qcter_val is okay
        --------------------------------------------------------------------*/
        if (ds707_jcdma_chng_m513_via_at((sio_bitrate_type)dsat_qcter_val) 
                                                                    == FALSE)
        {
          dsat_qcter_val = old_qcter_val;
          result = DSAT_ERROR;
        }
        else
#endif /* FEATURE_JCDMA_1X */
        {
          /* Change current baud and default in NV */
          dsat_change_baud( (sio_bitrate_type) dsat_qcter_val, FALSE);
        }
      }  /* brackets else in sio_is_baud_valid() == FALSE */
    }
  }
  return result;
}  /* dsatvend_exec_qcter_cmd() */

#ifdef FEATURE_DSAT_BREW_SUPPORT

/*===========================================================================

FUNCTION DSATVEND_EXEC_BREW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$brew command.
    at$brew is used to transition control from ATCOP to BREW COP

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the data port to Brew.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_brew_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* function return                    */
  dsat_result_enum_type result;

  /*result for diagcomm_sio_qcdmg       */
  boolean               brew_ret = FALSE;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
    rdm_device_enum_type device;

    MSG_HIGH ("Transitioning to BREW", 0, 0, 0);
    
    /* Query the RDM for the port currently being used by Data Services */
    device = rdm_get_device(RDM_DATA_SRVC);
  
    /* Relinquish the port to BREW */
    brew_ret = rdm_assign_port ( RDM_BREW_SRVC, device, NULL );

    if ( brew_ret )
    {
      result = DSAT_DO_DM;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  /*--------------------------------------
  Processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_brew_cmd () */

#endif /* FEATURE_DSAT_BREW_SUPPORT */

#endif /* FEATURE_DATA */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/ftm/ftm_log.c#1 $
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/06    dp     Add HDET limiting loop logging for UMTS targets.
07/13/06   xw      Type casting -- compilation error fix for 7200. 
06/16/06   jac     Fixed LINT errors.
03/09/06   ka      Added FTM_LOG_TX_RX_SWEEP.
07/20/05   dp      Use weakly defined types to handle 1x code.
06/14/05   rsr     Added log support for therm and vbatt
10/12/04   jac     Mainlined ASYNC packet code.
12/15/03   dp      Port from 6100 code
11/18/03   jwh     Substitute include files msm.h and msmhwio.h.
10/17/03   ka      Dereg. callback when changing interval, fixed macro usage, 
                   removed expiration parameter.
10/14/03   ka      Added callback-based flexible logging support in DMSS or FTM.
09/16/03   wd      Updated FTM log id to LOG_FTM_VER_2_C.
09/12/03   wd      Initial revision.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "ftmicap.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_LOGGING
#include "comdef.h"
#include "target.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"  /*lint -esym(766,ctype.h)*/

#include "ftm.h"
#include "ftm_task.h"
#include "ftm_log.h"

#if defined( FEATURE_WCDMA)
#error code not present
#endif

#include "msm.h"
#include "msmhwio.h" 
#include "rfagc.h"
#include "adc.h"
#include "therm.h"
#include "nv_items.h"

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif

#include "log_codes.h"  


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#define MAX_BUFFERS 100



/* 
 *  This table defines the configuration for each log id.  Any id's that use
 *  a periodic callback should be initialized below.  There is no requirement
 *  on the log id's being in a certain order, or numbered in a continous fashion
 *  (can use log id entries of (9,2,32768,7), rather than (1,2,3,4))
 *
 *  Any log id that has a callback function or entry function initialized here needs to have the
 *  active flag initialized to FALSE.  These log id's need to be initialized
 *  thru the ftm_log_enable interface so the callback intervals can be defined
 *  and init functions called.  Functions that have NULL for the func_cb AND
 *  entry/exit functions can be set as active = TRUE here, since they require no dynamic
 *  memory allocaiton.
 */  
#if defined (T_MSM6250)
#error code not present
#endif /* T_MSM6250 */

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/




/*===========================================================================

FUNCTION     FTM_LOG_FIND_LOG_ID 

DESCRIPTION
             This function finds a particular log id in the log config table
             
DEPENDENCIES
   None.

RETURN VALUE
             pointer to table entry if log id is found. Else returns 
             NULL.

SIDE EFFECTS
   None. 

===========================================================================*/
ftm_log_config_table_entry*   ftm_log_find_log_id( ftm_log_id_type   ftm_log_id)
{
    int i;
    ftm_log_config_table_entry     *table_ptr = NULL;

    for( i = 0; i < FTM_LOG_ELEMENT_COUNT( ftm_log_config_table); i++)
    {
        if( ftm_log_config_table[i].log_id == ftm_log_id)
        {
            table_ptr = &ftm_log_config_table[i];
            break;
        }        
    }

    return( table_ptr);
    
}




/*===========================================================================

FUNCTION     FTM_IS_LOG_ENABLED

DESCRIPTION
             This function checks if a particular log id is active

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None. 

===========================================================================*/
boolean ftm_is_log_enabled( ftm_log_id_type  ftm_log_id)
{

    ftm_log_config_table_entry   *table_ptr;


    table_ptr = ftm_log_find_log_id(ftm_log_id);

    if( table_ptr == NULL)
    {
        return FALSE;
    }
    else
    {
        return table_ptr->log_config.active;
    }
}

/*===========================================================================

FUNCTION FTM_LOG_MALLOC

DESCRIPTION
   This function allocates memory for an ftm lok pkt.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None. 

===========================================================================*/
PACKED void * ftm_log_malloc (uint16 ftm_log_id, unsigned int length)
{
  log_ftm_gen_type *ftm_log_pkt = NULL;
    
  if ( ftm_is_log_enabled( (ftm_log_id_type)ftm_log_id) && log_status( LOG_FTM_VER_2_C ))  
  {

      ftm_log_pkt = (log_ftm_gen_type*) log_alloc( LOG_FTM_VER_2_C, length);
  

      if (ftm_log_pkt != NULL)
      {
          ftm_log_pkt->ftm_log_id = (word)ftm_log_id;
      }
      else
      {
          FTM_MSG_ERROR("Unable to allocate ftm log memory",0,0,0);  
      }
  }
  
  return ( ( void*) ftm_log_pkt);
} /* End ftm_log_malloc */

/*===========================================================================

FUNCTION FTM_LOG_GET_DATA_PTR

DESCRIPTION
   This function returns a ptr to the data portion of a given ftm log pkt.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
PACKED void * ftm_log_get_data_ptr ( void *log_pkt)
{
  log_ftm_gen_type *ftm_log_pkt = NULL;
    
  ftm_log_pkt = ( log_ftm_gen_type*) log_pkt;
  
  return ( ( void*) ftm_log_pkt->data);
} /* End ftm_log_get_data_ptr */

/*===========================================================================

FUNCTION FTM_LOG_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but 
  will output logged to the dm monitor. 


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_log_printf( const char *fmt, ... )
{

    char             *local_printf_buffer = NULL;
    va_list           arg_ptr;
    log_ftm_gen_type *ftm_log_pkt = NULL;


    /* check to make sure we should log first */
    if ( !log_status( LOG_FTM_VER_2_C ))   
    {
        return;
    }

    /* Allocate memory for local buffer with extra space because we don't
       yet know how big the string will be once the string formatting is 
       done
     */
    local_printf_buffer = ( char*) ftm_malloc( strlen( fmt) + 100);

    /* check that we haev memory to work with */
    if (local_printf_buffer == NULL) 
    {
        FTM_MSG_ERROR("Unable to allocate memory for printf log",0,0,0);
        return;
    }

    /* first lets format the and copy the string. */
    va_start( arg_ptr, fmt ); /*lint !e718, !e746, !e64*/
    ( void) vsprintf( local_printf_buffer, fmt, arg_ptr);
    va_end( arg_ptr);

    /* now that we know the size, allocate the log memory we need */
    ftm_log_pkt = (log_ftm_gen_type *)ftm_log_malloc( FTM_LOG_PRINTF, FTM_LOG_HEADER_SIZE +
                                                  strlen( local_printf_buffer) + 
                                                  1 /* null char */);

    /* check that we haev memory to work with */
    if (ftm_log_pkt == NULL) 
    {
        return;
    } 

    /* copy the string into the log pkt */
    memcpy((void*)ftm_log_pkt->data, (void*)local_printf_buffer, strlen (local_printf_buffer));

    /* send log */
    log_commit( ( log_type*) ftm_log_pkt );

    /* clean up memory */
    ftm_free ( ( void*) local_printf_buffer); 

} /* End ftm_log_printf */









/*===========================================================================

FUNCTION FTM_LOG_ENABLE

DESCRIPTION
   This function enables FTM logging for a particular log id.
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Allocates and initializes memory for log array structure if necessary.
   Registers callback function for given log id.

===========================================================================*/
void ftm_log_enable(ftm_log_id_type log_id, int4 interval, int num_buffer)
{


  ftm_log_config_table_entry     *log_config_ptr;

  log_config_ptr = ftm_log_find_log_id( log_id);

  if( log_config_ptr == NULL)
  {
      return;
  }


  log_config_ptr->log_config.num_buffer = ((num_buffer > 0 && num_buffer < MAX_BUFFERS) ? num_buffer : 1);

  if( log_config_ptr->log_config.active == FALSE)
  {
      /* Make sure this is a log that requires a callback function */

      if( log_config_ptr->log_config.cb_func != NULL) 
      {
           /* Enable callback for log_id */
           log_config_ptr->log_config.cb_struct = (clk_cb_type*)ftm_malloc(sizeof(clk_cb_type));
           clk_def(log_config_ptr->log_config.cb_struct);
           clk_reg(log_config_ptr->log_config.cb_struct,
                   log_config_ptr->log_config.cb_func,
                   interval,
                   interval,
                   TRUE);
      }

      /* Set log to active status */

      log_config_ptr->log_config.active = TRUE;

      /* Call the log startup function, if available */

      if( log_config_ptr->log_init_func != NULL)
      {
          log_config_ptr->log_init_func();
      }
  }
  else
  {
      /* Make sure this is a log that requires a callback function */

      if( log_config_ptr->log_config.cb_func != NULL)
      {
          /* log is already active, dereg then register w/ new interval */
          clk_dereg(log_config_ptr->log_config.cb_struct);
          clk_reg(log_config_ptr->log_config.cb_struct,
                  log_config_ptr->log_config.cb_func,
                  interval,
                  interval,
                  TRUE);
      }

  } 

} /* End ftm_log_enable */

/*===========================================================================

FUNCTION FTM_LOG_DISABLE

DESCRIPTION
   This function disables FTM logging for a particular log id.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Deregisters the callback for the given log id.
   Deallocates the log array from the heap if no logs remain active.

===========================================================================*/
void ftm_log_disable(ftm_log_id_type log_id)
{

  ftm_log_config_table_entry     *log_config_ptr;

  log_config_ptr = ftm_log_find_log_id( log_id);


  /* Return if invalid log id */
  if( log_config_ptr == NULL)
  {
      return;
  }

  /* Disable log if active */
  
  if(log_config_ptr->log_config.active)
  {
      
      if( log_config_ptr->log_config.cb_struct != NULL)
      {
          
          clk_dereg( log_config_ptr->log_config.cb_struct);
          ftm_free(log_config_ptr->log_config.cb_struct);
      
      }

      log_config_ptr->log_config.active = FALSE;


      /* Call the log's exit function if available */

      if( log_config_ptr->log_exit_func != NULL)
      {
          log_config_ptr->log_exit_func();
      }
  
  }

} /* End ftm_log_disable */

/*===========================================================================

FUNCTION FTM_LOG_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None. 

===========================================================================*/
ftm_rsp_pkt_type ftm_log_dispatch(ftm_log_req_pkt_type  *cmd_ptr)
{

  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  FTM_MSG_MED("FTM Log dispatching packet id: %d",cmd_ptr->ftm_hdr.cmd_id,0,0); /*lint !e778*/

  switch(cmd_ptr->ftm_hdr.cmd_id)
  {
  

      case FTM_LOG_ENABLE:
         
          ftm_log_enable((ftm_log_id_type)cmd_ptr->params.log_id, 
                         cmd_ptr->params.interval, 
                         cmd_ptr->params.num_buffer);
          break;
  

      case FTM_LOG_DISABLE:
    
          ftm_log_disable((ftm_log_id_type)cmd_ptr->params.log_id);
          break;
  

      default:
    
          FTM_MSG_ERROR("Unknown FTM Log Command",0,0,0);

          rsp_pkt.cmd = FTM_RSP_BAD_CMD;
          break;
  
  }

  return rsp_pkt;

} /* End ftm_log_dispatch */

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */


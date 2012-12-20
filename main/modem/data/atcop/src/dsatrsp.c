/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
          A T   C O M M A N D   R E S P O N S E  G E N E R A T O R

GENERAL DESCRIPTION
  This module provides the functionality to build, format, and send
  AT command responses to the appropriate destination; TE or PS
  protocol stack.

EXTERNALIZED FUNCTIONS
  dsat_reg_rsp_router
    Registers a function that provides AT response routing based on
    operating service mode and AT command state.  Responses are routed
    to TE or PS protocol stack.
    
  dsat_fmt_response
    Formats the response from an AT command, and from the line as
    a whole, according to the values of the ATE and ATV parameters.

  dsat_send_result_code
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0. Also, modifies ATCOP internal state variables.
    
  dsat_send_dr_result
    Sends intermediate data compression reporting result code to TE or
    PS protocol stack based on response routing provided by response
    router.
    
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatrsp_send_response
    Sends AT response based on routing function registered or to TE
    if no function is registerd.  Routes response to TE or PS protocol
    stack based on operating service mode and AT command state.
    
  dsatrsp_fmt_result_code
    Places a formatted result code in the output buffer.  The type of
    result code is identified in the input parameter.  Formatting is
    based on whether or not the mode is verbose.

  dsatrsp_set_basic_result_code
    Produces and unformatted ITU-T V.25 ter basic result code and
    returns in DSM item.

  dsatrsp_send_cell_result
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 1995-2010 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatrsp.c_v   1.3   07 Nov 2002 16:20:12   wxie  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatrsp.c#5 $ $DateTime: 2010/05/04 04:14:50 $ $Author: adubey $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/10   ad      Fixed qccpbr command .
01/02/09   bs      Added support for 1X AT Phonebook commands.
12/31/08   bs      Changed the large DSM item with small.
03/28/08   bs      Fixed Lint High's.
10/29/07   sn      Replaced banned string APIs.
05/28/03   sb      Fix for WCDMA CS Calls
05/01/03   sb      modified dsat_send_result_code() to handle ATH command.
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
02/26/03   wx      Put dialed number into last dial ph book.
07/13/02   atp     Added function for ability to register format response.
                   In dsat_fmt_result_code(), added check for registered
                   function, and if registered calls it instead of default.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "AEEstd.h"
#include "target.h"
#include "err.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsm.h"
#include "sio.h"
#ifdef FEATURE_DATA_MM
#include "ds3gsiolib.h"
#else
#include "ds3gsioif.h"
#endif /* FEATURE_DATA_MM */

#include "assert.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include <stdio.h>

#if defined(FEATURE_DATA_GCSD) || defined( FEATURE_DATA_WCDMA_CS )
#include <string.h>
#endif /* (FEATURE_DATA_GCSD) || ( FEATURE_DATA_WCDMA_CS ) */

#ifdef FEATURE_ETSI_PBM
#include "dsatetsime.h"
#endif /* FEATURE_ETSI_PBM */


/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Pointer to function defining how responses should be routed. */
LOCAL dsat_routing_func_type route_at_cmd_rsp_func_ptr = NULL;

/* Pointer to function defining how responses should be formated.          */
dsat_fmting_func_type fmt_at_cmd_rsp_func_ptr = NULL;

#ifdef FEATURE_DSAT_FLOW_CONTROL
/* Flag to indicate if tx (dsat_to_sio) watermark hit high.
   If this flag is high, ATCOP should halt current commands that
   requesting data from the subsystem
*/
boolean dsati_tx_wm_high = FALSE;

/* Function to reenter current command processing which has halted
   when tx (dsat_to_sio) watermark hit high.
*/
dsati_async_event_handler_type dsati_to_sio_low_handler = NULL;
#endif /* FEATURE_DSAT_FLOW_CONTROL */

/* Timer to delay when listing large quantities of PB entries or SMSes
   and DSM large items are close to being exhausted: this needed as USB 
   dequeues from DS3GSIOLIB WM as quickly as queued so normal flow control
   doesn't kick in, USB can acquire chain of all available DSM large items
   and never have a chance to free them as its task doesn't get a chance 
   to run while DS and WMS or PBM are busy with this. */
rex_timer_type dsati_free_item_timer;

/* dsati_free_item_timer_expired_handler() will call this on timer expiration
   if not NULL. Handlers for PB entry/SMS reads can set this function to
   reenter processing following delay to allow DSM items to be freed by USB
   task. */

dsati_async_event_handler_type dsati_handle_free_item_timer_expiry = NULL;

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/



/*-------------------------------------------------------------------------
    Prototypes for local functions:
-------------------------------------------------------------------------*/

LOCAL dsm_item_type *etsi_set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
);

LOCAL dsm_item_type *cdma_set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
);

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION DSAT_REG_RSP_ROUTER

DESCRIPTION
  This function registers a function that provides AT response routing
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.  Registered function routes AT
  command response to either SIO or PS protocol stack.  

  If service type is IS-707 related and AT operating mode is online data
  mode, responses are routed to PS protocol stack, otherwise they are
  routed to SIO.  This provides support for IS-707 command reflection in
  future releases and makes it independent of ATCOP operation.  If no
  routing function is registered, response is sent directly to SIO.

  The response routing function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_reg_rsp_router
(
  dsat_routing_func_type func_ptr       /* Function providing mode specific
                                           AT command response routing or
                                           NULL to deregister routing
                                           function. */
)
{
  /* Set the response routing function pointer to the new response
     routing function. A value of NULL deregisters the routing. */
  route_at_cmd_rsp_func_ptr = func_ptr;

} /* dsat_reg_rsp_router( ) */


/*===========================================================================

FUNCTION DSAT_REG_RSP_FMTER

DESCRIPTION
  This function registers a function that provides AT response formating
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.

  If service type is IS-707 related and AT operating mode is online data
  mode, responses to reflected AT commands have to be formated with
  EIA-617 codes. So the default fmt_response function cannot be used and
  the 707 mode-specific handler should use this function to register a
  function that does appropriate formatting.

  The response formating function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_rsp_fmter
(
  dsat_fmting_func_type func_ptr    /* Function providing mode specific
                                       AT command response formating or
                                       NULL to deregister routing function */
)
{
  /*-------------------------------------------------------------------------
    Set the response formating function pointer to the new response
    routing function. A value of NULL deregisters the routing. 
  -------------------------------------------------------------------------*/
  fmt_at_cmd_rsp_func_ptr = func_ptr;

} /* dsat_reg_rsp_fmter() */


/*===========================================================================

FUNCTION  DSATRSP_SEND_RESPONSE 

DESCRIPTION
  Sends AT response based on routing function registered or to TE
  if no function is registered.  Routing function routes response to TE
  or PS protocol stack based on operating service mode and AT command
  state.

DEPENDENCIES
  Routing is only performed if routing function is registered, otherwise
  response is sent to TE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_response
(
  dsm_item_type *rsp_ptr,                 /* Ptr to response buffer to send */
  const boolean rearm_autodetect          /* Flag to rearm SIO autodetect */
)
{
  ASSERT( rsp_ptr != NULL );
  if ( rsp_ptr == NULL )
  {
    ERR( "NULL response pointer passed in.", 0, 0, 0 );
    return;
  }
  
  if ( route_at_cmd_rsp_func_ptr != NULL )
  {
    /* Routing function is registered, so send response based on routing
       function. */
    route_at_cmd_rsp_func_ptr( rsp_ptr, dsatcmdp_at_state, rearm_autodetect );
  }
  else
  {
    /* No routing function is registered, so send response to SIO. */
#ifdef FEATURE_DATA_MM
    (void) ds3g_siolib_setup_tx( rsp_ptr, rearm_autodetect );
#else
    (void) ds3g_sio_setup_tx( rsp_ptr, rearm_autodetect );
#endif /* FEATURE_DATA_MM */
  }

} /* dsatrsp_send_response( ) */


/*===========================================================================

FUNCTION DSAT_FMT_RESPONSE

DESCRIPTION
  This function formats the response according whether or not
  the mode is verbose and whether or not the response is a result
  code or informational text.  

  The dsm item passed in as in_ptr_ptr can contain chained dsm items with
  the dsm "used" value defining the length of each item.  These chained
  items are assumed to contain a NULL terminated string, which will be
  formatted up until the dsm "used" value is reached for each buffer
  or a NULL character is encountered, whichever occurs first.  *in_ptr_ptr
  will be returned as NULL to indicate the dsm input items have been freed.

  The type of data passed in the dsm item pointed to by in_ptr_ptr is
  specified by the type_of_rsp parameter.  This controls response
  formatting and identifies if header, trailer, both header and trailer,
  or no header and trailer should be added to the response.  This allows
  formatting of partial responses, as oppossed to only supporting
  formatting of complete responses, to reduce DSM buffer usage. See enum
  type definition for details.

  The dsm item passed in as out_ptr_ptr is filled with the formatted
  response.  If there is not enough room in this dsm item, the full item
  is queued to send as a command line response and additional dsm
  items are allocated, with out_ptr_ptr pointing to the last item
  allocated.  out_ptr_ptr may be returned as NULL, if no more dsm
  buffers are available. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_fmt_response
(
  dsm_item_type **in_ptr_ptr,         /* Response to format in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in 
                                         in_ptr_ptr                        */
  boolean result_code,                /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr         /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
)
{
  byte    c;                     /* Location to store character            */ 
  byte   *src_ptr;               /* Pointer to source (input) buffer       */
  byte   *src_end_ptr;           /* Pointer to end of source data          */
  byte   *dest_ptr;              /* Pointer to destination (output) buffer */
  byte   *dest_end_ptr;          /* Pointer to end of destination buffer   */
  boolean write_data;            /* Flag to indicate write to destination  */
  boolean header;                /* Flag to write header                   */
  boolean trailer;               /* Flag to write trailer                  */
  dsm_item_type *in_ptr;         /* Pointer to input dsm item              */

  /*-------------------------------------------------------------------------
    If input pointer is NULL, there is nothing to do so exit.
  -------------------------------------------------------------------------*/
  if ( in_ptr_ptr == NULL || *in_ptr_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize pointer to input dsm item and verify output pointer is
    valid.  Exit if output pointer is invalid.
  -------------------------------------------------------------------------*/
  in_ptr = *in_ptr_ptr;
  
  /* Input item is always freed, so initialize the return value here. */
  *in_ptr_ptr = NULL;

  ASSERT( out_ptr_ptr != NULL && *out_ptr_ptr != NULL );  
  if ( out_ptr_ptr == NULL || *out_ptr_ptr == NULL )
  {
    /* Free chained input dsm items. */
    dsm_free_packet( &in_ptr );

    return;
  }
  
  /*-------------------------------------------------------------------------
    Initialize header and trailer flags.
  -------------------------------------------------------------------------*/
  switch ( type_of_rsp )
  {
  case DSAT_START_OF_RSP:
    header  = TRUE;
    trailer = FALSE;
    break;

  case DSAT_END_OF_RSP:
    header  = FALSE;
    trailer = TRUE;
    break;

  case DSAT_INTERMEDIATE_RSP:
    header = trailer = FALSE;
    break;

  case DSAT_COMPLETE_RSP:
    header = trailer = TRUE;
    break;
    
  default:
    header = trailer = TRUE;                  /* Make lint happy. */      
    ERR( "Invalid type of response.", 0, 0, 0 );
    break;
  }
  
  /*-------------------------------------------------------------------------
      Initialize source data pointers.
  -------------------------------------------------------------------------*/
  src_ptr = in_ptr->data_ptr;
  src_end_ptr = &in_ptr->data_ptr[in_ptr->used];

  /*-------------------------------------------------------------------------
    Initialize destination pointers.
  -------------------------------------------------------------------------*/
  dest_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used];
  dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
  ASSERT( dest_ptr < dest_end_ptr );

  /*-------------------------------------------------------------------------
    Format Response and write to output (destination) buffer.  Allocate
    new output buffers as necessary. Handle chained dsm items for source
    data.  
  -------------------------------------------------------------------------*/
  for ( write_data = TRUE; write_data; )
  {
    /* Check to see if we have enough room in the destination
       buffer for one more execution of this loop and the addition
       of a trailer after the loop, and if not allocate a new one.
       Worst case a two byte header, two bytes for a newline, and
       a two byte trailer could be placed in the buffer. */
    if ( dest_ptr >= (dest_end_ptr - 6) )
    {
      /* Update the number of bytes used in output buffer. */
      (*out_ptr_ptr)->used += (word) (dest_ptr -
                       &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used]);

      /* Queue formatted response. */
      dsatcmdp_queue_cmd_results( *out_ptr_ptr );      

      /* Allocate a new buffer */
      *out_ptr_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
      if ( *out_ptr_ptr == NULL )
      {
        ERR( "Running out of buffers",0,0,0 );
        if ( in_ptr != NULL )
        {
          /* Free chained input dsm items. */
          dsm_free_packet( &in_ptr );
        }
        return;
      }

      /* Initialize destination pointers. */
      dest_ptr = (*out_ptr_ptr)->data_ptr;
      dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
    }

    /* Determine if we need to write response header at beginning of
       buffer. */
    if ( header )
    {
      /* Write header if verbose mode. */
      header = FALSE;
      if ( dsat_v_val == 1 )
      {
        *dest_ptr++ = (byte) dsat_s3_val;
        *dest_ptr++ = (byte) dsat_s4_val;
      }
    }

    /* Copy a byte of source data to destination buffer. */

    /* Source is a dsm item with possible chained buffers.  The
       dsm "used" value or a NULL character determines the end of
       data in the buffer, whichever occurs first. */

    if ( src_ptr < src_end_ptr )
    {
      /* There is more data in source buffer so copy data. */
      c = *src_ptr++;
      if ( c == '\0' )
      {
        /* Assume end of source data string because of NULL termination. */
        write_data = FALSE;
        dsm_free_packet( &in_ptr );
      }
      else if ( c == '\n' )     
      {
        /* Format text for newline */
        *dest_ptr++ = (byte) dsat_s3_val;
        *dest_ptr++ = (byte) dsat_s4_val;
      }
      else
      {
        /* Copy source data to destination. */
        *dest_ptr++ = c;
      }
    }
    else
    {
      /* End of dsm source buffer reached, so free buffer and check
         for another buffer in chain. */
      in_ptr = dsm_free_buffer( in_ptr );
      if ( in_ptr != NULL )
      {
        /* Another buffer in chain, so initialize source pointers. */
        src_ptr = in_ptr->data_ptr;
        src_end_ptr = &in_ptr->data_ptr[in_ptr->used];
      }
      else
      {
        /* End of source data, no more buffers in chain. */
        write_data = FALSE;
      }
    }
    
  } /* end for( ) */

  /*-------------------------------------------------------------------------
    Add trailer to destination buffer if a trailer should be added.
    Note, there is room for a two byte trailer in the destination buffer
    because this was verified in the previous for( ) loop.
  -------------------------------------------------------------------------*/
  if ( trailer == TRUE )
  {
    /* Set up trailer: <CR> always, <LF> if verbose or re. */
    *dest_ptr++ = (byte) dsat_s3_val;
    if ( dsat_v_val == 1 || !result_code )
    {
      *dest_ptr++ = (byte) dsat_s4_val;
    }
  }

  /*-------------------------------------------------------------------------
    Update the number of bytes used in output buffer.
  -------------------------------------------------------------------------*/
  (*out_ptr_ptr)->used += (word) (dest_ptr -
        &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used]);

} /* dsat_fmt_response( ) */


/*===========================================================================

FUNCTION DSATRSP_FMT_RESULT_CODE

DESCRIPTION
  Places a formatted result code in the output buffer.  The type of
  result code is identified in the input parameter.  Formatting is
  based on whether or not the mode is verbose.  The result codes
  generated are based on mode specific result code generating
  functions.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_fmt_result_code
(
  dsat_result_enum_type result_code,   /* Result code                      */
  dsm_item_type **out_ptr_ptr          /* Location to store formatted
                                          response, output will NOT be NULL
                                          terminated                       */
)
{
  dsati_operating_cmd_mode_type op_mode;
  dsm_item_type *raw_rsp_ptr;

  /* Table of mode specific functions for generating result codes specific
     to that mode of operation. One function must be supplied for each mode
     of operation supported. */
  static struct
  {
    dsm_item_type* (*result_code_handler)( dsat_result_enum_type );
  } table[NUM_OPER_CMD_MODES] =
  {
    etsi_set_result_code,
    cdma_set_result_code 
  };

  /* Get the current operating mode. */
  op_mode = dsatcmdp_get_operating_cmd_mode( );
  
  ASSERT( op_mode < NUM_OPER_CMD_MODES );
  ASSERT( table[op_mode].result_code_handler != NULL );

  /* Generate a formatted result code if the result code handler exists. */
  if ( table[op_mode].result_code_handler != NULL )
  {
    raw_rsp_ptr = table[op_mode].result_code_handler( result_code );

    if ( fmt_at_cmd_rsp_func_ptr != NULL )
    {
      /*-------------------------------------------------------------------
        If mode-specific handler has registered a function to format
        responses, it should be invoked when in ONLINE DATA mode. This
        is needed for IS-707 async reflected AT cmds which need to format
        responses with 617 codes.
      -------------------------------------------------------------------*/
      fmt_at_cmd_rsp_func_ptr (
                                  &raw_rsp_ptr,
                                  DSAT_COMPLETE_RSP,
                                  TRUE,
                                  out_ptr_ptr,
                                  result_code
                              );
    }
    else
    {
      dsat_fmt_response(
                         &raw_rsp_ptr,
                         DSAT_COMPLETE_RSP,
                         TRUE,
                         out_ptr_ptr
                       );
    }
  }
  
} /* dsatrsp_fmt_result_code( ) */

/*===========================================================================

FUNCTION DSATRSP_SET_BASIC_RESULT_CODE

DESCRIPTION
  Produces and unformatted ITU-T V.25 ter basic result code and returns
  in DSM item.  Result code returned will be either numeric or verbose,
  based on input parameter.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted basic result code.

SIDE EFFECTS
  None
  
===========================================================================*/
dsm_item_type *dsatrsp_set_basic_result_code
(
  dsat_result_enum_type result_code,    /* Basic result code               */
  boolean               verbose         /* verbose result code or numeric  */
)
{
  int index = 0;
  dsat_result_enum_type basic_result_code;
  char *result_code_str;
  dsm_item_type *raw_rsp_ptr;
  byte *tmp_ptr;

  /* Basic result codes specified in ITU-T V.25 ter. */
  static const struct
  {
    dsat_result_enum_type code;
    char *str_ptr;
  } result_code_table[ ] =
  {
    {DSAT_OK,             "OK"}, 
    {DSAT_CONNECT,        "CONNECT"},   
    {DSAT_RING,           "RING"},   
    {DSAT_NO_CARRIER,     "NO CARRIER"},
    {DSAT_ERROR,          "ERROR"},     
    {DSAT_NO_DIALTONE,    "NO DIALTONE"},
    {DSAT_BUSY,           "BUSY"},
    {DSAT_NO_ANSWER,      "NO ANSWER"}, 
    {DSAT_MAX_BASIC,      NULL}
  };
    
  /* Search the basic result code table for a match. */
  do
  {
    basic_result_code = result_code_table[index].code;
    result_code_str = result_code_table[index++].str_ptr;
  } 
  while ( (basic_result_code != result_code) && 
          (result_code_str != NULL )
        );

  if ( result_code_str == NULL )
  {
    ERR( "Incorrect basic result code %d", result_code, 0, 0 );
    return NULL;
  }

  /* Get item for unformatted raw response. */
  raw_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
  if ( raw_rsp_ptr == NULL )
  {
    ERR( "Running out of buffers", 0, 0, 0 );
    return  NULL;
  }

  /* Generate verbose or numeric basic result code. */
  if ( verbose )
  {
    /* Generate verbose ITU-T V.25 ter result code. */
    raw_rsp_ptr->used = (word) std_strlprintf( (char*) raw_rsp_ptr->data_ptr, 
                                               raw_rsp_ptr->size, "%s",
                                               result_code_str );
  }
  else
  {
    /* Generate numeric ITU-T V.25 ter result code. */
    uint32 result_code_basic = (uint32) ((int32)basic_result_code );
    tmp_ptr = dsatutil_itoa( result_code_basic,
                             raw_rsp_ptr->data_ptr, 10 );
    raw_rsp_ptr->used = (uint16) (tmp_ptr - raw_rsp_ptr->data_ptr + 1);
  }

  return raw_rsp_ptr;

} /* dsatrsp_set_basic_result_code( ) */

/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatrsp_send_cell_result
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  dsm_item_type *fmt_rsp_ptr;

  if ( dsat_q_val == 0 )
  {
    /* Result codes are not suppressed so send result code. */

    /* Get buffer to hold formatted response. */
    fmt_rsp_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
    if ( fmt_rsp_ptr == NULL )
    {
      ERR( "Running out of items", 0, 0, 0 );
      return;
    }

    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    /* Send the formatted result code. */
    if ( fmt_rsp_ptr != NULL )
    {
      dsatrsp_send_response( fmt_rsp_ptr, FALSE );
    }
  }

} /* dsatrsp_send_cell_result( )  */


/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsat_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  /* If we are processing ATH command we are supposed to continue
     processing the command line. Check to see if we are processing 
     ATH. If so then brach out from here. This function sends the 
     result code but also continues processing the AT command line.
     Final result code is sent out after processing the command line. */
  if (dsati_processing_ath_cmd)
  {
    dsat_finished_ath_cmd (result_code);
    return; /* Need not execute rest of the function */
  }

  /* Indicate abort command processing has completed if not disabled.
     This function performs no action if abort command processing is
     not currently in progress. */
  if ( dsatcmdp_end_abort_in_send_rsp )
  {
    dsatcmdp_command_abort_complete( );
  }
  /* Format and send result code */
  dsatrsp_send_cell_result( result_code );

  /* If result code is not CONNECT, clear the regional block indications
     flag to indicate indications can now be sent to the TE. */
  if ( result_code != DSAT_CONNECT )
  {
    dsatcmdp_block_indications_flag = FALSE;
  }

#ifdef FEATURE_ETSI_PBM
  {
    dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();

    if ((current_mode == DSAT_MODE_GSM) || (current_mode == DSAT_MODE_WCDMA))
    {
      if ( result_code == DSAT_CONNECT )
      {
        dsatetsime_ld_connected();
      }
      else if (result_code == DSAT_OK || result_code == DSAT_NO_CARRIER ||
               result_code == DSAT_ERROR || result_code == DSAT_NO_DIALTONE || 
               result_code == DSAT_BUSY || result_code == DSAT_NO_ANSWER)
      {
        dsatetsime_ld_save();
      }
    }
  }
#endif /* FEATURE_ETSI_PBM */
} /* dsat_send_result_code( )  */

#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_WCDMA) )

/*===========================================================================

FUNCTION  DSAT_SEND_DR_RESULT

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_send_dr_result
(
  dsat_dr_mode_e_type compression          /*  Compression mode            */
)
{
  /* The table of return_code_text is according to
     V.25ter series V section 6.6.2
     Data Compression reporting control +DR */
  LOCAL const struct 
  {
    dsat_dr_mode_e_type return_code;
    char * return_code_text;
  } return_code_table[] = 
  {
    DSAT_DR_MODE_NONE,     "NONE",
    DSAT_DR_MODE_BOTH,     "V42B",
    DSAT_DR_MODE_RD,       "V42B RD",
    DSAT_DR_MODE_TD,       "V42B TD",
    DSAT_DR_MODE_MAX,      NULL
  };
  
  /* the service reporting is done only when +CR set to be 1 */
  if(dsat_dr_val == 1)
  {
    int index = 0;
    char * return_code_string;

    dsm_item_type *raw_rsp_ptr, *formatted_rsp_ptr;
    char * buff_ptr;

    /* search through the serv_table for serv_text */
    do
    {
      return_code_string = return_code_table[index].return_code_text;
    } while (return_code_table[index++].return_code != compression &&
             return_code_string != NULL);
  
    if(return_code_string == NULL)
    {
      ERR("Incorrect return code type %d", compression, 0, 0);
      return;
    }
  
    /* use dsm buffer to output results */
    raw_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
    if ( raw_rsp_ptr == NULL )
    {
      ERR( "Running out of buffers",0,0,0 );
      return;
    }
  
    buff_ptr = (char *) raw_rsp_ptr->data_ptr;
    (void)std_strlprintf(buff_ptr, raw_rsp_ptr->size, 
                         "+DR: %s", return_code_string);
    raw_rsp_ptr->used = (word)strlen(buff_ptr) + 1;

    formatted_rsp_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
    if ( formatted_rsp_ptr == NULL )
    {
      (void)dsm_free_buffer(raw_rsp_ptr);
      ERR( "Running out of items",0,0,0 );
      return;
    }
    
    /* format the result code and send response */
    dsat_fmt_response( &raw_rsp_ptr, DSAT_COMPLETE_RSP, TRUE,
                       &formatted_rsp_ptr );
    dsatrsp_send_response( formatted_rsp_ptr, TRUE );
  }
} /* dsat_send_dr_result( ) */
#endif /* defined(FEATURE_GSM) */



/*===========================================================================

FUNCTION ETSI_SET_RESULT_CODE

DESCRIPTION
  Produces ITU-T V.25 ter basic code or ETSI extended format result code
  if ATQ is 0 and returns in DSM item.
  
  If AT+CRC is 0, only the basic result codes are returned and extended
  RING codes are translated to the basic RING code.

  If AT+CRC is 1, the extendted format RING <type> is returned.
                  <type> is defined in Section 6.11 of 3GPP TS 27.007

  NOTE: This function should be removed to an ETSI specific file at a
        later date.  SJD_HACK
        
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing result code.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsm_item_type *etsi_set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
)
{
  dsm_item_type *raw_rsp_ptr = NULL;
  
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  
  return raw_rsp_ptr;
  
} /* etsi_set_result_code( ) */


/*===========================================================================
FUNCTION      CDMA_SET_RESULT_CODE

DESCRIPTION   
  CDMA result codes.
        
DEPENDENCIES  
  None
  
RETURN VALUE  
  Pointer to DSM item containing result code.

SIDE EFFECTS  
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsm_item_type *cdma_set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
)
{
  dsm_item_type *raw_rsp_ptr = NULL;

#if defined(FEATURE_CDMA)
  if ( dsat_q_val == 0 )
  {
    /* Result codes are not suppressed so display result code. */

    if ( result_code < DSAT_MAX_BASIC )
    {
      /* Basic result code */
      raw_rsp_ptr = dsatrsp_set_basic_result_code( result_code,
                                                   (boolean) dsat_v_val );
    }
  }
#endif /* defined(FEATURE_CDMA) */
  
  return raw_rsp_ptr;
  
} /* cdma_set_result_code( ) */


#ifdef FEATURE_DSAT_FLOW_CONTROL

/*===========================================================================

FUNCTION  DSAT_TO_SIO_LOWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits low.

  It sets dsati_tx_wm_high flag to be FALSE and inform ds task with
  DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_lowater_cb ( void )
{
  ds_cmd_type * cmd_ptr;

  dsati_tx_wm_high = FALSE;
  
  /* Send a DS_AT_TO_SIO_LOW_CMD cmd to DS task */
  if ( (cmd_ptr = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("Out of ds cmd buffer",0,0,0);
  }
  
  cmd_ptr->hdr.cmd_id = DS_AT_TO_SIO_LOW_CMD;

  ds_put_cmd(cmd_ptr);
}


/*===========================================================================

FUNCTION  DSAT_TO_SIO_HIWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits high.

  It sets dsati_tx_wm_high flag to be TRUE.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_hiwater_cb ( void )
{
  dsati_tx_wm_high = TRUE;
}


/*===========================================================================

FUNCTION  DSATI_TO_SIO_LOW_CMD_HANDLER

DESCRIPTION
  This is the handler function for DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: if the dsat_to_sio low hanlder succeeds.
  DSAT_ERROR    : if the dsat_to_sio low hanlder fails.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsati_to_sio_low_cmd_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  if(dsati_to_sio_low_handler != NULL)
  {
    result = dsati_to_sio_low_handler(mode, cmd_ptr);
  }
  return result;
}
#endif /* FEATURE_DSAT_FLOW_CONTROL */

/*===========================================================================

FUNCTION  DSATI_FREE_ITEM_TIMER_EXPIRED_HANDLER

DESCRIPTION
  This is the handler function for expiration of timer waiting for DSM large
  items to be freed by USB when listing many PB entries/SMSes.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT: if the handler succeeds.
  DSAT_ERROR    : if the handler fails.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsati_free_item_timer_expired_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  if ( (unsigned int)DSAT_TIMER_FREE_ITEM == cmd_ptr->cmd.dsat_timer_id &&
       NULL != dsati_handle_free_item_timer_expiry )
  {
    return dsati_handle_free_item_timer_expiry( mode, cmd_ptr );
  }
  else
  {
    MSG_ERROR( "Free item timer handler incorrectly called: timer ID - %d", 
               cmd_ptr->cmd.dsat_timer_id, 0, 0 );
    return DSAT_ASYNC_EVENT;
  }
} /* dsati_free_item_timer_expired_handler */

#endif /* FEATURE_DATA */

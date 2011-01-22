/*=============================================================================

        R F  D r i v e r  L o g i c a l  L a y e r  C a l l  B a c k 
                           D e f i n i t i o n

GENERAL DESCRIPTION
  This file provides the logical layer call back and event generating
  services for the RF driver. 

EXTERNALIZED FUNCTIONS
  rfllcb_init()
  rfllcb_get_event_buffer()
  rfllcb_register_events()
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  rfllcb_init() must be called prior to calling any other function
  exported by this module.

Copyright (c) 2000 - 2002  by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfllcb.c_v   1.0   01 Jul 2002 18:16:40   doerfler  $
  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfllcb.c#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/17/06   ycl     Modifications for initial MSM7600 build.
01/18/06   dp/bhas Fix bug with handling callback delay parameter.
07/01/02   sd      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"

#include "rfcom.h"
#include "rfllcb.h"
#include "clk.h"
#include "queue.h"
#include "assert.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of clock services call backs that can be active
  simultaneously.
---------------------------------------------------------------------------*/
#define MAX_NUM_CB_SUPPORTED    2

/*---------------------------------------------------------------------------
  Array of call back event structures.  Each element of the array handles
  an active clock services call back.  These are required to handle
  cases where multiple call back events may be registered simultaneously
  with the clock services.  Note: This would not be necessary if clock
  services supported passing a user data pointer to the call backs
  called by clock services.
---------------------------------------------------------------------------*/
static struct handler_struct
{
  /* Pointer to call back event structure initialized to define events
     to generate. */
  rfllcb_struct_type *cb_event_ptr;

  /* Pointer to array element of events that identifies current event
     to process.  Events are within call back event structure. */
  rfllcb_event_struct_type *current_event_ptr;

  /* Clock services call back structure */
  clk_cb_type clock_cb;
  
  /* Call back routine to be called by clock services */ 
  void (*clk_routine_ptr)( int4 );
  
} handler[MAX_NUM_CB_SUPPORTED];

/*---------------------------------------------------------------------------
  Free queue to hold buffers used to register call back event sequences.
---------------------------------------------------------------------------*/
q_type free_que;

/*---------------------------------------------------------------------------
  Items to be placed on free queue.
---------------------------------------------------------------------------*/
rfllcb_struct_type free_que_bufs[MAX_NUM_CB_SUPPORTED];

/*---------------------------------------------------------------------------
  Prototypes for local functions:
---------------------------------------------------------------------------*/
static void cb_handler0( int4 interval_ms );
static void cb_handler1( int4 interval_ms );

/*---------------------------------------------------------------------------
  Array of pointers to clock routine for initialization of handler
  structure.
---------------------------------------------------------------------------*/
void (*clk_routine_ptrs[MAX_NUM_CB_SUPPORTED])( int4 ) =
{
  cb_handler0,
  cb_handler1
};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  RFLLCB_INIT

DESCRIPTION
  Initializes the call back services.

DEPENDENCIES
  This function must be called before calling any other function
  exported by the call back services.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfllcb_init( void )
{
  struct handler_struct *handler_ptr;
  rfllcb_struct_type *buf_ptr;
  int i;

  /* Initialize the free queue. */
  (void) q_init( &free_que );
  
  /* Initialize each element of handler array and free queue. */
  for ( i = 0; i < MAX_NUM_CB_SUPPORTED; i++ )
  {
    /* Initialize link for each item to be placed on free queue,
       initialize pointer to handler element, and place each item on
       free queue. */
    buf_ptr = &free_que_bufs[i];
    (void) q_link( buf_ptr, &buf_ptr->hdr.q_link );
    q_put( &free_que, &buf_ptr->hdr.q_link );
    handler_ptr = &handler[i];    
    buf_ptr->hdr.handler_ptr = (void*) handler_ptr;

    /* Initialize the fields of each handler element. */
    handler_ptr->cb_event_ptr = NULL;
    handler_ptr->current_event_ptr = NULL;
    clk_def( &handler_ptr->clock_cb );
    handler_ptr->clk_routine_ptr = clk_routine_ptrs[i];
  }
  
} /* rfllcb_init() */

/*===========================================================================

FUNCTION  RFLLCB_GET_EVENT_BUFFER

DESCRIPTION
  Gets an event structure buffer that must be used to register a sequence
  of call back events.  Buffer is freed by making a call to
  rfllcb_register_events() with the buffer pointer as input.

DEPENDENCIES
  A buffer should only be allocated when getting ready to register events
  with a call to rfllcb_register_events().
  
RETURN VALUE
  Pointer to event structure.

SIDE EFFECTS
  An assert is thrown if no event buffers are available.

===========================================================================*/
rfllcb_struct_type* rfllcb_get_event_buffer( void )
{
  rfllcb_struct_type *rtn_ptr;

  /* Get buffer from free queue. */
  rtn_ptr = (rfllcb_struct_type*) q_get( &free_que );

  /* Throw assert if queue empty.  This implies the number of call
     backs supported must be increased. */
  ASSERT( rtn_ptr != NULL );

  /* Make sure we stop here even if ASSERTs are not enabled within
     development builds, which seems to be common practice. */
  if ( rtn_ptr == NULL )    /*lint !e774, Boolean within 'if' always evaluates to False */
  {
    ERR_FATAL("Need to increase number call backs supported.", 0, 0, 0 );
  }
  
  return rtn_ptr;
  
} /* rfllcb_get_event_buffer() */

/*===========================================================================

FUNCTION  RFLLCB_REGISTER_EVENTS 

DESCRIPTION
  Registers a sequence of call back events based on events defined in
  events structure passed in.  A sequence of call back events consists
  of a series of calls to the same call back function, with each call
  having a different event passed in.  Each call to the call back function
  occurs at the delay defined in the events structure passed in.

  The input buffer pointed to by cb_event_ptr is freed internally by
  this module after the event sequence has been generated.

DEPENDENCIES
  The event structure passed in must have been allocated with a call
  to rfllcb_get_event_buffer().
  
RETURN VALUE
  None

SIDE EFFECTS
  Frees event pointer passed in after event sequence has been generated.

===========================================================================*/
void rfllcb_register_events
(
  rfllcb_struct_type *cb_event_ptr
    /* Pointer to call back event structure. This structure must be
       initialized to define the event sequence to generate. */
)
{
  struct handler_struct *handler_ptr;
  
  /* Verify call back event pointer is valid. */
  ASSERT( cb_event_ptr != NULL );

  /* Verify call back handler pointer is valid. */
  ASSERT( cb_event_ptr->cb_handler != NULL );
  
  /* Point to current handler element. */
  handler_ptr = cb_event_ptr->hdr.handler_ptr;

  /* Verify handler pointer is valid. */
  ASSERT( handler_ptr != NULL );

  /* Verify clock services routine pointer is valid. */
  ASSERT( handler_ptr->clk_routine_ptr != NULL );
  
  /* Setup handler to point at call back event structure passed in. */
  handler_ptr->cb_event_ptr = cb_event_ptr;

  /* Setup current event pointer to point to first event in array
     of events. */
  handler_ptr->current_event_ptr = &cb_event_ptr->cb_events[0];
      
  /* Regisiter clock services routine to be called at the next event
     time rounded up to the nearest millisecond and decrement number
     of events to generate. */
  --cb_event_ptr->num_events;  
  clk_reg( &handler_ptr->clock_cb, handler_ptr->clk_routine_ptr,
           (handler_ptr->current_event_ptr->delay + 999) / 1000,
           0, FALSE );
  
} /* rfllcb_register_events() */

/*===========================================================================

FUNCTION  GENERATE_CB_EVENTS  

DESCRIPTION
  Calls call back handler each time an event expires and registers
  next event with clock services.  Note: This function is called from
  interrupt level.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Places event structure on free queue after the last event in event
  sequence has been generated.

===========================================================================*/
static void generate_cb_events
(
  struct handler_struct *handler_ptr
    /* Pointer to handler array element defining events to generate */
)
{
  rfllcb_struct_type *cb_event_ptr;

  /* Set pointer to call back event structure in handler. */
  cb_event_ptr = handler_ptr->cb_event_ptr;
  
  /* Call the call back handler for the current event. */
  cb_event_ptr->cb_handler( handler_ptr->current_event_ptr->event,
                            (void*) cb_event_ptr->user_data_ptr );

  /* Determine if there are more events to generate. */
  if ( cb_event_ptr->num_events > 0 )
  {
    /* More events need to be generated so increment pointer to the next
       event, decrement number of events remaining, and register clock
       services call back routine at time next event should occur. */
    --cb_event_ptr->num_events;
    ++handler_ptr->current_event_ptr;

    /* Regisiter clock services routine to be called at the next event
       time rounded up to the nearest millisecond. */
    clk_reg( &handler_ptr->clock_cb, handler_ptr->clk_routine_ptr,
             ((int)(handler_ptr->current_event_ptr->event) + 999) / 1000,
             0, FALSE );
  }
  else
  {
    /* Free the event buffer to indicate all events have been generated. */
    q_put( &free_que, &cb_event_ptr->hdr.q_link );
  }
  
} /* generate_cb_events() */

/*===========================================================================

FUNCTION  CB_HANDLER0
          CB_HANDLER1

DESCRIPTION
  Call back routines called by clock services when time expires.  A
  seperate call back handler is required for each call back event
  sequence currently active.  Note: These functions are called from
  interrupt level.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static void cb_handler0
(
  int4 interval_ms     /* Interval in milliseconds when call back called. */
)
{
  generate_cb_events( &handler[0] );
} /* cb_handler0() */

/* ARGSUSED */
static void cb_handler1
(
  int4 interval_ms     /* Interval in milliseconds when call back called. */
)
{
  generate_cb_events( &handler[1] );
} /* cb_handler1() */

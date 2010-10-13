#ifndef RFLLCB_H
#define RFLLCB_H

/*===========================================================================

      R F  D r i v e r  L o g i c a l  L a y e r  C a l l  B a c k 
                          H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF driver logical layer call back services.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  rfllcb_init() must be called prior to calling any other function
  exported by this module.
  
Copyright (c) 1999 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfllcb.h_v   1.0   01 Jul 2002 18:16:36   doerfler  $
$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfllcb.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#include "rfcom.h"
#include "queue.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of call back events currently supported.
---------------------------------------------------------------------------*/
#define RFLLCB_MAX_NUM_EVENTS       1

/*---------------------------------------------------------------------------
  Type definition of internal header for event generation structure.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Queue link */
  q_link_type q_link;

  /* Pointer for processing internal to this module only */
  void *handler_ptr;
  
} rfllcb_hdr_struct_type;  

/*---------------------------------------------------------------------------
  Type definition of a single event to generate when processing of a
  particular RF function.
---------------------------------------------------------------------------*/
typedef struct 
{
  /* Event */
  rfcom_cb_event_enum_type event;

  /* Time to wait in microseconds from previos event before event is
     generated. If this is the first event in the array, this is the
     delay from when the event is registered until the event generated. */
  uint32 delay;
    
} rfllcb_event_struct_type;

/*---------------------------------------------------------------------------
  Type definition of the events to generate during processing of a
  particular RF function.  The call back handler is called once for each
  event generated.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Buffer header -  The contents of this header should not be
     modified by the caller. */
  rfllcb_hdr_struct_type hdr;
  
  /* Call back handler to call for each event. */
  rfcom_cb_handler_type cb_handler;

  /* Pointer to user data that is passed to call back handler. */
  const void *user_data_ptr;
  
  /* Number of events to generate.  Defines number of events specified in
     following array of events. */
  int num_events;                       

  /* Array of events to generate with the num_events entries initialized. */
  rfllcb_event_struct_type cb_events[RFLLCB_MAX_NUM_EVENTS];

} rfllcb_struct_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern void rfllcb_init( void );

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
extern rfllcb_struct_type* rfllcb_get_event_buffer( void );

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
extern void rfllcb_register_events
(
  rfllcb_struct_type *cb_event_ptr
    /* Pointer to call back event structure. This structure must be
       initialized to define the event sequence to generate. */
);


#endif  /* RFLLCB_H */

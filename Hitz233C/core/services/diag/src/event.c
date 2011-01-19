
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service.

  An event consists of a 12 bit event ID and a timestamp.

  The requirement is that the DMSS will never drop events as long as it does
  not exceed bandwidth limitations with event reporting.  Event reporting
  has the highest priority of all unsolicited diagnostic data.

  To minimize impact of the caller's task, the event is generated and placed
  in a queue.  The DIAG task will build the final packet structure and send
  to the external device.

  Just in case an event is dropped, a "drop event" will be reported and a
  count of dropped events will be reproted as payload.

Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2000-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/event.c#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed critical lint errors.
07/13/06   as      Added RPC support for mobileview
12/12/05   tbg     Added flag to help prevent recursive event trace saving.
10/31/05    as     Fixed lint errors.
08/15/05   eav     Added a check for buffer corruption
03/09/05    as     Event config is not needed to a allocate an event.
02/18/04    as     Fixed critical lint errors.
02/10/05    as     Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
12/06/04    as     Included dog.h to prevent compiler warnings
09/01/04   eav     Added FEATURE_SAVE_DEBUG_TRACE.
08/11/04    as     Fixed corner cases for event masks.
06/11/04    gr     Added support for event masks.
09/23/03    gr     The drop handler now detects the "out of space" condition
                   more efficiently.
05/05/03    as     Event timestamp fix
12/31/02   lad     Implementation rewrite.  Previous history deleted.
07/01/99   lad     Created file.

===========================================================================*/


#include "customer.h"
#include "comdef.h"

#include <string.h>

#include "assert.h"
#include "crc.h"
#include "queue.h"
#include "qw.h"
#include "task.h"
#include "ts.h"

#include "diag.h"
#include "diagcmd.h"
#include "diagcomm.h"
#include "diagdiag.h"
#include "diagi.h"
#include "diagtune.h"
#include "event.h"
#include "dog.h"

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  #include "diagdebug.h"
  #include "err.h"
  #include "fs_public.h"
#endif


/*-------------------------------------------------------------------------
  Definitions and Declarations
-------------------------------------------------------------------------*/
/* Event reporting config indicator. */
#define EVENT_CONFIG_COMM_MASK 0x01
#define EVENT_CONFIG_LISTENER_MASK 0x02
#define EVENT_CONFIG_VALID_MASK 0x03

#ifdef PLATFORM_LTK
#error code not present
#else
  uint8 event_config = 0;
#endif

/* Pending Notify timer - used to avoid context switch during INTLOCK. */
static rex_timer_type event_pending_q_timer;
static boolean event_pending_q_pending = FALSE;
  
/* Stale timer.  If this timer expires, the current event buffer will
   be retired and a new one may be started with the next event report. */
static rex_timer_type event_stale_timer;

typedef struct
{
  diag_event_listener listener;
  void *param; /* User's parameter */
}
event_listener_entry_type;

/* This data member is used for the event listener functinality.
   The API is in diag.h. */
/*lint -save -e{785} */
static diag_searchlist_type event_listeners = {FALSE, 0, 0, 0, 0, 0, NULL};
/*lint -restore */

/* NOTE: diag_event_type and event_store_type purposely use the same
   format, except that event_store type is preceeded by a Q link
   and the event ID field has a union for internal packet formatting.
   If either types are changed, the service will not function properly. */
typedef struct
{
  q_link_type qlink;            /* To be accessed by event_q_xxx() only */

  union
  {
    unsigned int id;
    event_id_type event_id_field;
    uint16 sized_field;         /* Used to copy the event ID structure */
  }
  event_id;

  qword ts;                     /* always store full time stamp */

  event_payload_type payload;
}
event_store_type;

/* The pending queue holds events that have yet to be serviced by the listeners. */
static q_type event_pending_q;

static struct
{
  /* The length field indicates the approximate packet size of the event
     report packet.  It is calculated based on the assumption of truncated
     time stamps, since most are truncated (except the first). */
  unsigned int length;

  q_type q;
}
event_q;

#define EVENT_Q_SIZE_ADJ(xx_ev) \
  sizeof (event_trunc_type) + FPOS (event_payload_type, payload) + \
  event->payload.length;

/* The queued event to be last in the report a packet. */
static event_store_type *event_rpt_last = NULL;
/* Using the same criteria, a flush completion is detected by matching
   the event at the time the flush was issued within event_rpt_prep(). */
static event_store_type *event_flush_last = NULL;

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 event_drop_handler_malloc_cnt=0;
uint32 event_drop_handler_free_cnt=0;
uint32 event_q_alloc_malloc_cnt=0;
uint32 event_service_pending_cnt =0;
uint32 event_drain_free_cnt = 0;
uint32 event_q_get_cnt =0;
uint32 event_listener_match_and_free_cnt =0;
uint32 diag_add_event_listener_malloc_cnt = 0;
uint32 diag_add_event_listener_free_cnt = 0;

uint32 get_event_q_cnt(void)
{
  return q_cnt (&event_q.q);
}
#endif /* FEATURE_DEBUG_DIAG_HEAP */



static void event_control (uint8 config, uint8 config_mask);
static event_store_type * event_q_alloc (event_id_enum_type id, uint8 payload_length);
static void event_q_pending (event_store_type *event);
static void event_q_put (event_store_type * event);
static event_store_type * event_q_get (void);
static void event_service_listener (event_store_type *event_store);

/* Size of the event mask array, which is derived from the maximum number
 * of events.
 */
#define EVENT_MASK_SIZE (EVENT_LAST_ID/8 + 1)

#define EVENT_MASK_BIT_SET(id) \
  (event_mask[(id)/8] & (1 << ((id) & 0x07)))

/* The event mask.
 */
static unsigned char event_mask[EVENT_MASK_SIZE];

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  #define EVENT_SET_UP_HEADER(x) ((DIAG_DEBUG_EVT_BIT << 13) | x)
#endif

typedef struct{
  unsigned char mask[EVENT_MASK_SIZE]; 
}event_large_mask;

#define EVENT_LARGE_MASK(X) \
     (((event_large_mask *)X)->mask)

typedef struct
{
  uint8 payload[32]; 
} event_large_payload_type;


#define EVENT_LARGE_PAYLOAD(X) \
     (((event_large_payload_type *)X)->payload)

/*-------------------------------------------------------------------------
  Function Implementations
-------------------------------------------------------------------------*/


/*=========================================================================*/
void
event_init (void)
{
  int i;

  /* Define (initialiize) the stale and pending timers for event reporting. */
   rex_def_timer (&event_stale_timer, &diag_tcb, DIAG_EVENT_TIMER_SIG);
   rex_def_timer (&event_pending_q_timer, &diag_tcb, 
                  DIAG_EVENT_PEND_TIMER_SIG);

  event_q.length = 0;
  (void) q_init (&event_q.q);
  (void) q_init (&event_pending_q);

  /* Set all the event bits by default.
  */
  for (i = 0; i < EVENT_MASK_SIZE; i++)
    event_mask[i] = 0xff;

  diag_sleep_vote (DIAG_SLEEP_EVENT, TRUE);
}                               /* event_init */

/*===========================================================================

FUNCTION EVENT_DROP_HANDLER

DESCRIPTION
  This routine handles the case where events are dropping and the service
  is attempting to continue operation.  It attempts to allocate both a
  "dropped event event" and the requested event.  If both are not successful,
  NULL is returned, indicating the service continues to drop events,
    
DEPENDENCIES
  This must be called from INTLOCK() (within critical section).

RETURN VALUE
  A pointer to the allocated event Q item is returned.  
  NULL is returned if unsucessful.

===========================================================================*/
static event_store_type * 
event_drop_handler (uint32 drop_cnt, qword drop_ts, uint8 payload_length)
{
  event_store_type *event = NULL;
  event_store_type *drop_event = NULL;
  unsigned int event_sizes[2];

  /* Do not try to malloc memory if there is not enough space for both the
  ** allocations we do here. This is an efficiency improvement that
  ** prevents iterations through the heap to allocate memory for one
  ** event only to free it immediately because there is no memory for
  ** the second one.
  */
  event_sizes[0] = FPOS (event_store_type, payload.payload) + 
    FSIZ (event_drop_type, drop_cnt);
  event_sizes[1] = FPOS (event_store_type, payload.payload) + payload_length;
  if (!diag_alloc_check (2, event_sizes))
    return NULL;

  /* Since the event service is currently dropping, there must be space 
     for both the "dropped event" event and the next event. */
  drop_event = (event_store_type *) diag_malloc (
    FPOS (event_store_type, payload.payload) + 
    FSIZ (event_drop_type, drop_cnt));

  if (drop_event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    event_drop_handler_malloc_cnt++;
    MSG_FATAL("MALLOC event_drop_handler cnt = %d, heap used = %d  drop_event",
        event_drop_handler_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    event =
      (event_store_type *) diag_malloc (
        FPOS (event_store_type, payload.payload) + payload_length);
    
    if (event)
    {
      /* Event allocation successful.  This means dropping is over.  Queue the
        "dropped event" event and return the caller's event q buffer. */
      PACKED uint32 *drop_cnt_field;

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_malloc_cnt++;
      MSG_FATAL("MALLOC event_drop_handler cnt = %d, heapused = %d drop_event = %d",
          event_drop_handler_malloc_cnt, diag_heap_used_bytes(),EVENT_DROP_ID);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      drop_event->event_id.id = EVENT_DROP_ID;

      qw_equ (drop_event->ts, drop_ts);

      drop_event->payload.length = FSIZ (event_drop_type, drop_cnt);
      
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast */
      drop_cnt_field = (PACKED uint32 *) &drop_event->payload.payload;
/*lint -restore */
      
      *drop_cnt_field = drop_cnt;

      event_q_pending (drop_event);

      drop_event = NULL;
    }
    else
    {
      diag_free (drop_event);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_drop_handler_free_cnt++;
      MSG_FATAL("FREE event_drop_handler_free_cnt = %d, diag heap used = %d ",
          event_drop_handler_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      drop_event = NULL;
    }
  }
  
  return event;
}

/*==========================================================================

FUNCTION EVENT_Q_ALLOC

DESCRIPTION
  This routine allocates an event Q item from the DIAG heap and fills in 
  the following information common to all events:
  
  Event ID
  Time stamp
  Payload length field
  
  This routine also detects dropped events and handles the reporting of
  dropped events.
    
RETURN VALUE
  A pointer to the allocated  event from the Q is returned.  
  NULL is the Q is empty.

===========================================================================*/
static event_store_type *
event_q_alloc (event_id_enum_type id, uint8 payload_length)
{  
  event_store_type *event = NULL;
  uint32 int_sav;
  static uint32 drop_cnt = 0;
  static qword drop_ts;

  /* Verify that the event id is in the right range and that the
  ** corresponding bit is turned on in the event mask.
  */
  if ((id > EVENT_LAST_ID) || !EVENT_MASK_BIT_SET (id))
    return NULL;
  else
  {
    /* Try to allocate an event if event mask is set */
    INTLOCK_SAV (int_sav);
    if (drop_cnt > 0)
    {
      event = event_drop_handler (drop_cnt, drop_ts, payload_length);

      if (event)
      {
        drop_cnt = 0;
      }
      INTFREE_SAV (int_sav);
    }
    else
    {
      INTFREE_SAV (int_sav);

      event = (event_store_type *) diag_malloc (
        FPOS (event_store_type, payload.payload) + payload_length);
    }

    if (event)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      event_q_alloc_malloc_cnt++;
      MSG_FATAL("MALLOC event_q_cnt = %d, diag heap used = %d event_id = %d",
          event_q_alloc_malloc_cnt,diag_heap_used_bytes(),id);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      ts_get (event->ts);

      event->event_id.id = id;
      event->payload.length = payload_length;
    }
    else
    {
      /* Drop the event */
      INTLOCK ();

      if (drop_cnt == 0)
      {
        ts_get (drop_ts);
      }

      drop_cnt++;
      
      INTFREE ();
    }
  }
  
  return event;
}

/*===========================================================================

FUNCTION EVENT_Q_PENDING

DESCRIPTION
  This routine places the given event item in the "pending event" Q.  This
  also wakes the DIAG task to service the pending Q immediately.  This is
  needed to assure timely reporting of events to listeners.

===========================================================================*/
static void event_q_pending (event_store_type *event)
{
  ASSERT (event);

  q_put (&event_pending_q, q_link (event, &event->qlink));
  

#ifdef FEATURE_DEBUG_DIAG_HEAP
   MSG_FATAL("Q_PUT event_q_pending cnt = %d diag heap used = %d",
         q_cnt (&event_pending_q),  diag_heap_used_bytes(),0); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */
     
  event = NULL;

  /* The semaphore prevents multiple settings of the timer, which could 
     avoid expiration under heavy load. */
  if (!event_pending_q_pending)
  {
    /* Avoid multiple timer setting before expiration. */
    event_pending_q_pending = TRUE;

    /* Set the timer to a very small value.  This will invoke a DIAG task 
       wakeup.  Doing so eliminates re-entrance requirements since only
       the DIAG task will decide when to send events. */
    (void) rex_set_timer (&event_pending_q_timer, 1);
  }

}

/*===========================================================================

FUNCTION EVENT_SERVICE_PENDING

DESCRIPTION
  This routine services the pending Q.  The pending Q is a imtermediate
  storage Q before events are queued to be sent to the comm layer.
   
  Listeners are serviced in this step.  This routine only runs in DIAG task
  context.
  
===========================================================================*/
void event_service_pending (void)
{
  event_store_type *event = NULL;

  /* Turn off the pending Q semaphore */
  event_pending_q_pending = FALSE;
   
  event = (event_store_type *) q_get (&event_pending_q);

  while (event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    MSG_FATAL("Q_GET event_service_pending q_cnt = %d diag heap used = %d ",
        q_cnt (&event_pending_q),diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    event_service_listener (event);

    if (event_config & EVENT_CONFIG_COMM_MASK)
    {
       event_q_put (event);
    }
    else
    {
       diag_free (event);

#ifdef FEATURE_DEBUG_DIAG_HEAP
       event_service_pending_cnt++;
       MSG_FATAL("FREE event_service_pending_cnt = %d, diag heap used = %d",
           event_service_pending_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      event = NULL;
    }

    event = (event_store_type *) q_get (&event_pending_q);
  }
}

/*===========================================================================

FUNCTION EVENT_Q_PUT

DESCRIPTION
  This routine places the given event item in the event Q.  It also 
  maintains the event_q.length field.  If a set threshold is crossed,
  the stale timer is set to expire quickly so the queued events will be
  marked to be sent in the next packet.
  
===========================================================================*/
static void
event_q_put (event_store_type * event)
{
  if (event)
  {
    q_put (&event_q.q, q_link (event, &event->qlink));

#ifdef FEATURE_DEBUG_DIAG_HEAP
    MSG_FATAL("Q_PUT event_q_put event_q_cnt = %d diag heap used = %d",
        q_cnt (&event_q.q),  diag_heap_used_bytes(),0); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    /* Increment the estimated data usage in the event q, assuming
       truncated time stamps. */
    event_q.length += EVENT_Q_SIZE_ADJ (event);

    /* If a packet is not yet ready to send, see if the threshold is crossed. */
    if (event_q.length >= EVENT_RPT_PKT_SIZE)
    {
      event_stale ();
    }
  }

  return;
}

/*===========================================================================

FUNCTION EVENT_Q_GET

DESCRIPTION
  This routine retrieves an event stored in the event Q.  It also maintains
  the event_q.length field.  This field is is used as a trigger in 
  event_q_put() to detect when the queue starts to get full.
  
RETURN VALUE
  A pointer to a event from the Q is returned.  
  NULL is the Q is empty.

===========================================================================*/
static event_store_type *
event_q_get (void)
{
  event_store_type *event;

  event = (event_store_type *) q_get (&event_q.q);

  if (event)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    MSG_FATAL("Q_GET event_q_get event_q_cnt = %d diag heap used = %d ",
        q_cnt (&event_q.q),  diag_heap_used_bytes(),0); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    event_q.length -= EVENT_Q_SIZE_ADJ (event);
  }

  return event;
}

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
/*=========================================================================*/
static void
event_save_trace
(
  event_id_enum_type event_id_enum,
  evt_payload_len_type length,
  void* payload,
  event_store_type* diag_event
)
{
  static int event_recurrsion_flag = FALSE;
  static int event_save_trace_drop_count = 0;
  
  if(event_recurrsion_flag == TRUE)
  {
    /* Another event was sent while we were already processing one.
     * We must drop this event to avoid a possible infinite loop.
     */
    event_save_trace_drop_count++;
    return;
  }
   
  if (diag_debug_control_mask & DIAG_DEBUG_EVT_BIT)
  {
    int saving_ret_val = 0;
    char savetime = 0;
    event_store_type event;
    evt_length_type num_bytes_to_buffer = 0;
    diag_debug_hdr_type record_hdr = 0;
    evt_payload_len_type payload_length;
    int bytes_saved = 0;
    uint8 pretimestamp_buffer[6];
    qword ts;

    savetime = diag_debug_detail_mask & DIAG_DEBUG_SAVETIME;

    /* Retrieve tiemstamp ASAP */
    if (savetime)
    {
      INTLOCK();
      /* We don't want any additional events processed while we are getting
       * the time stamp, since those events may be from the time code itself.
       * So set our recursion flag to TRUE.
       */
      event_recurrsion_flag = TRUE;
      
      /* Set up the timestamp */
      if ( diag_event )
      {
        /* Timestamp available from created event */
        qw_equ( ts, diag_event->ts );
      }
      else
      {
        /* Need to get timestamp yourself */
        ts_get( ts );
      }          
      
      event_recurrsion_flag = FALSE;
      INTFREE();
    }        
    
    /* Create a event_id_type */
    event.event_id.event_id_field.id              = event_id_enum;
    event.event_id.event_id_field.reserved        = 0;
    event.event_id.event_id_field.time_trunc_flag = FALSE; /* full timestamp */
    if (length <= EVENT_PAY_TWO_BYTE)
    {
      event.event_id.event_id_field.payload_len = length;
    }
    else
    {
      event.event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;
    }

    /* Determine how many bytes are going to be saved to the buffer */
    num_bytes_to_buffer += sizeof(evt_id_type);

    /* Check to see if you're saving time */
    if (savetime)
    {
#ifdef FEATURE_OPTIMIZE_TRACE_RECORD_TIMESTAMPS
      num_bytes_to_buffer += sizeof(diag_debug_tstamp_delta_type);
#else
      num_bytes_to_buffer += sizeof(diag_debug_tstamp_type);   /* timestamp */
#endif
    }

    /* Check how much payload saving to buffer */
    if (length > EVENT_PAY_TWO_BYTE)
    {
      num_bytes_to_buffer += sizeof(evt_payload_len_type);
    }
    num_bytes_to_buffer += length; /* payload */

    /* Save the event header to the trace buffer */
    record_hdr = EVENT_SET_UP_HEADER(savetime);

    /* Save the length (if necessary) */
    /* Save the payload (if necessary) */
    payload_length = event.event_id.event_id_field.payload_len;

    /* Set up pre-timestamp buffer */
    memcpy(pretimestamp_buffer, &record_hdr, sizeof(diag_debug_hdr_type));
    bytes_saved += (int)sizeof(diag_debug_hdr_type);

    memcpy(pretimestamp_buffer+bytes_saved, &num_bytes_to_buffer,
                                                 sizeof(evt_length_type));
    bytes_saved += (int)sizeof(evt_length_type);

    memcpy(pretimestamp_buffer+bytes_saved, &(event.event_id.sized_field),
                                                     sizeof(evt_id_type));
    bytes_saved += (int)sizeof(evt_id_type);

  /* Save event atomically */
    INTLOCK();

  /* This is an atomic procedure, and should not be interrupted.
   * However, is one of the operations below tries to send a message,
   * we could end up in a recurrsive loop.  We will set a flag to ensure 
   * this does not happen. If the flag is already set, we will have dropped
   * the mesasge when entering the function.
   */
    
  /* set our flag */
  event_recurrsion_flag = TRUE;

/*lint -save -e{545}  Suspicious use of & */
    saving_ret_val = diag_debug_trace_to_buffer ((uint8*) &pretimestamp_buffer, bytes_saved);
/*lint -restore */

    /* Save the timestamp */
    if ( savetime )
    {
      if (saving_ret_val == DIAGDEBUG_SUCCESS)
      {
        saving_ret_val = diag_debug_trace_timestamp_to_buffer(ts);
      }
    }

    /* Save everything that should go after timestamp */
    if (saving_ret_val == DIAGDEBUG_SUCCESS)
    {
      switch(payload_length)
      {
        case EVENT_PAY_NONE:
          /* Do not save anything */
          break;
        case EVENT_PAY_ONE_BYTE:
          /* Save one byte of payload data, no payload length field */
          saving_ret_val = diag_debug_trace_to_buffer((uint8*) payload, 1);
          break;
        case EVENT_PAY_TWO_BYTE:
          /* Save two bytes of payload data, no payload length field */
          saving_ret_val = diag_debug_trace_to_buffer((uint8*) payload, 2);
          break;
        case EVENT_PAY_LENGTH:
          /* Save "length" bytes, 1 byte of payload length */
          saving_ret_val = diag_debug_trace_to_buffer((uint8*) &length,
                                          sizeof(evt_payload_len_type));
          if (saving_ret_val == DIAGDEBUG_SUCCESS)
          {
            saving_ret_val = diag_debug_trace_to_buffer((uint8*) payload, length);
          }
          break;
        default:
          MSG_HIGH("Incorrect payload length %d\n", payload_length,0,0);
          break;
      }
    }

    if (saving_ret_val == DIAGDEBUG_SUCCESS)
    {
      diag_debug_inc_trace_record_count();
    }

    /* clear flag */
    event_recurrsion_flag = FALSE;  
  
  INTFREE();
  } /* diag_debug_control_mask */
  return;
}                               /* event_save_trace */

#endif /* defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE) */
/*=========================================================================*/
void
event_report (event_id_enum_type event_id)
{
  event_store_type *event;

  event = (event_store_type *) event_q_alloc (event_id, 0);

  if (event)
  {
    event_q_pending (event);
  }

#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  event_save_trace( event_id, 0, NULL, event );
#endif

  return;
}                               /* event_report */

/*=========================================================================*/
void
event_report_payload (event_id_enum_type event_id, uint8 length, void *payload)
{
  event_store_type *event = NULL;

  if (length > 0 && payload && length <= 32)
  {
    event = (event_store_type *) event_q_alloc (event_id, length);

    if (event)
    {
      memcpy (EVENT_LARGE_PAYLOAD(event->payload.payload), payload, length);
      event_q_pending (event);
    }
  }
  else
  {
    if(length > 32)
      MSG_HIGH("Event_report_payload: Length > 32: Event ID: %d",event_id,0,0);
    event_report (event_id);
  }
#if defined (FEATURE_ERR_EXTENDED_STORE) && defined (FEATURE_SAVE_DEBUG_TRACE)
  if (length > 0 && payload)
  {
/*lint -save -e{644} */
    event_save_trace( event_id, length, payload, event);
/*lint -restore */
  }
#endif

  return;
}                               /* event_report_payload */

/*===========================================================================

FUNCTION EVENT_RPT_PREP

DESCRIPTION
  This routine performs preparation work on the items in the event Q prior
  to being sent.  This is necessary since the packet format requries a 
  length field at the beginning, and the packet is sent piece-wise.
  Therefore, the length must be calculated prior to sending the header 
  of the packet (header contains the length field).

  If listeners are enabled, each event is serviced by the listener system.
  
  Since traversal must occur, this routine also detects time truncation
  the event ID field for the final packet format.

DEPENDENCIES
  This routine must be called by the DIAG task at a TX packet boundary.
  
  This routine expects 'event_rpt_last' to be set, indicating a group
  os messages that are ready to be sent.

RETURN VALUE
  The length of the final event packet payload.  This value is used in the
  event packet header.
  
  0 is returned if not successful.

===========================================================================*/
static uint16
event_rpt_prep (void)
{
  event_store_type *event = NULL;
  unsigned int pkt_len = 0;     /* Length of 'events' member of event_rpt_type */
  uint32 time_context_hi = 0xFFFFFFFFU;  /* time context used for truncation */
  uint32 time_context_hi_new = 0xFFFFFFFFU;      /* time context used for truncation */
  uint16 time_context_lo = 0xFFFFU; /* time context used for truncation */
  uint16 time_context_lo_new = 0xFFFFU;     /* time context used for truncation */
  uint8 payload_length = 0;     /* Working var to optimize mem access */
  boolean flush_complete = FALSE;

  if (event_rpt_last)
  {
    /* To initialize, traverse the queue to determine truncated time
       stamps and packet length.  This traversal is unavoidable given
       the packet design. */

    /* First, look at the item at the head of the queue.  Since 
       'event_rpt_last' is set, it is assumed the Q has at least 1 item */
    event = q_check (&event_q.q);

    /* If flush pending, the tool mask flag must be cleared after flush completes */ 
    if (event_flush_last && event_flush_last == event)
    {
      flush_complete = TRUE;
    }

    event->event_id.event_id_field.id = event->event_id.id;

    payload_length = event->payload.length;

    /* The first event in the packet always contains a full time stamp. */
    pkt_len = sizeof (event_type) + payload_length;

    event->event_id.event_id_field.reserved = 0;

    if (payload_length <= EVENT_PAY_TWO_BYTE)
    {
      event->event_id.event_id_field.payload_len = payload_length;
    }
    else
    {
      event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;

      /* Adjust payload length to include length field */
      pkt_len += FPOS (event_payload_type, payload);
    }

    event->event_id.event_id_field.time_trunc_flag = FALSE;

    time_context_hi = qw_hi (event->ts);
    time_context_lo = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */

    /* Calculate length for this and all other events in the queue until the
       packet threshold is reached. */
    while (event && event != event_rpt_last)
    {
      event = q_next (&event_q.q, &event->qlink);

      if (event)
      {
        /* If flush pending, the tool mask flag must be cleared after flush completes */ 
        if (event_flush_last && event_flush_last == event)
        {
          flush_complete = TRUE;
        }

        event->event_id.event_id_field.id = event->event_id.id;

        event->event_id.event_id_field.reserved = 0;

        payload_length = event->payload.length;

        pkt_len += payload_length;

        if (payload_length <= EVENT_PAY_TWO_BYTE)
        {
          event->event_id.event_id_field.payload_len = payload_length;
        }
        else
        {
          event->event_id.event_id_field.payload_len = EVENT_PAY_LENGTH;

          /* Adjust payload length to include length field */
          pkt_len += FPOS (event_payload_type, payload);
        }

        /* Check time stamp relative to the last event in the queue.  Truncate
           if the context (upper 5 bytes) are the same. */
        time_context_hi_new = qw_hi (event->ts);
        time_context_lo_new = ((uint16) (qw_lo (event->ts)>>(8*2))& 0xFFFF);     /* Get MSBytes of lower TS */

        if (time_context_hi == time_context_hi_new &&
            time_context_lo == time_context_lo_new)
        {
          event->event_id.event_id_field.time_trunc_flag = TRUE;

          /* Adjust pkt_len for a truncated time stamp */
          pkt_len += sizeof (event_trunc_type);
        }
        else
        {
          event->event_id.event_id_field.time_trunc_flag = FALSE;

          time_context_hi = time_context_hi_new;
          time_context_lo = time_context_lo_new;

          /* Adjust pkt_len for a full time stamp */
          pkt_len += sizeof (event_type);
        }
      }
      else
      {
        /* 'event_rpt_last' should be found before the queue is empty. */
        MSG_ERROR ("Unexpected: event_rpt_last not in Q!", 0, 0, 0);

        event_rpt_last = NULL;  /* Start over upon the next event_q_put() */

        pkt_len = 0;
      }
    }
  }

  /* If a flush (to the external device) is complete, clear the config */
  if (flush_complete)
  {
    event_flush_last = NULL;
    event_control (0, EVENT_CONFIG_COMM_MASK);
  }

  return (uint16) pkt_len;

}

/*===========================================================================

FUNCTION EVENT_DRAIN

DESCRIPTION
  This routine builds and sends the event report packet from the event queue.
  Time truncation, CRC calculation,

  This routine periodically checks the DIAG task signals against 'mask'.
  If 'mask' is found in DIAG's task signals, the routine will exit.

DEPENDENCIES
  This routine must be called by the DIAG task at a TX packet boundary.
  If this routine returns FALSE, it must be called again prior to any other
  data being sent to the comm layer.

RETURN VALUE
  TRUE if complete (no more events to send),
  FALSE if in progress (more to send) - must call again later.

===========================================================================*/
boolean
event_drain (rex_sigs_type mask)
{
  /* Persistent data */
  static event_store_type *event_q_item_sav = NULL;
  static event_rpt_type hdr = {0, 0, {0}};
  static boolean finished_sav = TRUE;       /* return value */
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  
  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  event_store_type *event_q_item = event_q_item_sav;
  uint8 *frag = (uint8 *) send.pkt;
  uint8 *frag_last = (uint8 *) send.last;
  boolean finished = finished_sav;       /* return value */
  
  /* Local variables */
  event_type *event_full = NULL;
  event_trunc_type *event_trunc = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  (void) mask;     /* suppress compiler warning */

  /* If finished (not in progress), event_drain() is at a packet boundary. */
  if (finished)
  {
     /* Service the pending queue for good measure. */
     event_service_pending ();

    /* If packet boundary not established, check if the event queue has
       crossed its threshold.  This settles a corner case where the stale
       timer isn't serviced while trying to TX indiag_blocking_malloc(). */
    if (!event_rpt_last)
    {
      if ((rex_get_sigs (&diag_tcb) & DIAG_EVENT_TIMER_SIG) || 
          diag_panic_mode == TRUE)
      {
        /* The stale timer expired and we aren't bust sending a packet.  
           Clear the sig and prep the next packet (calling event_stale()). */
        (void) rex_clr_sigs (&diag_tcb, DIAG_EVENT_TIMER_SIG);

        /* This will set event_rpt_last. */
        event_stale ();
      }
    }

    /* If set, a packet boundary has been established - prep the packet */
    if (event_rpt_last)
    {
      hdr.cmd_code = DIAG_EVENT_REPORT_F;
      hdr.length = event_rpt_prep ();
  
      if (hdr.length > 0)
      {
        /* A report packet is now in progress */
        finished = FALSE;
        
        frag = (uint8 *) &hdr;
        frag_last = frag + FPOS (event_rpt_type, events) - 1;
  
        /* Reset send descriptor's state. */
        send.state = DIAG_SEND_STATE_START;
      }
      else
      {
        frag = NULL;
      }
    }
  }
  
  while (frag && status == DIAGBUF_READY_S)
  {

    if (diag_panic_mode) dog_kick();  /* PLM */

    send.pkt = (void *) frag;
    send.last = (void *) frag_last;

    status = diagbuf_send_pkt (&send);

    /* Last does not change - only need to update 'pkt'. */
    frag = (uint8 *) send.pkt;

    if (send.state == DIAG_SEND_STATE_COMPLETE)
    {
      frag = NULL;

      /*  The event from the queue has completed processing.  This check
          will fail for the packet header. */
      if (event_q_item)
      {
        diag_free (event_q_item);
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_drain_free_cnt++;
        MSG_FATAL("FREE event_drain_free_cnt = %d, diag heap used = %d ",
            event_drain_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        event_q_item = NULL;
      }

      if (send.terminate)
      {
        send.terminate = FALSE;

        /* Even if more events are ready, we must give up control to allow
           the packet service a chance to run.  Therefore, return "finished"
           and wait for DIAG to call again. */
        finished = TRUE;

        /* Clear the packet termination flag. */
        event_rpt_last = NULL;

        if (event_q.length >= EVENT_RPT_PKT_SIZE ||
            diag_panic_mode == TRUE)
        {
          /* This will set event_rpt_last. */
          event_stale ();
        }

        break; /* Exit function and wait to be called again. */
      } /* if send.terminate (pkt complete): finished, event_rpt_last = NULL */
    } /* if send complete: free q item, reset frag, handle terminate case */

    if (!frag)
    {
      /* If a packet is in progress, get the next event */
      event_q_item = event_q_get ();

      if (event_q_item)
      {
#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_q_get_cnt++;
        MSG_FATAL("event_q_item event_q_get_cnt = %d, diag heap used = %d ",
            event_q_get_cnt,diag_heap_used_bytes(),0);

        MSG_FATAL("EVENT ID = %d, EVENT Payload = %d ",
            (*(event_q_item)).event_id.event_id_field.id,
            (*(event_q_item)).event_id.event_id_field.payload_len,0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

        /* Reset send descriptor's state */
        send.state = DIAG_SEND_STATE_BUSY;

        /* We have a new event to process.  Check if this is the last for
           this packet. */
        if (event_q_item == event_rpt_last)
        {
          /* Set flag to terminate the packet. */
          send.terminate = TRUE;
        }

        /* Format the event from the queue to as the packet format and assign
           the packet fragment pointer to the start of the correctly formatted
           packet fragment. */

        /* First, assign the packet fragment pointer to the start of the
           payload. */
        frag = event_q_item->payload.payload;

        /* Init the last byte ptr to the size of the payload.  Adjustments 
           will be made below. */
        frag_last = frag + event_q_item->payload.length - 1;

        /* If the payload is more than 2 bytes, include the length field. */
        if (frag_last - frag >= EVENT_PAY_TWO_BYTE)
        {
          /* Adjust fragment pointer to include the length field. */
          frag -= FPOS (event_payload_type, payload);
        }

        /* Now that 'frag' is pointing to the start of the payload portion
           of the event (which follows the event ID/time structure), adjust
           'frag' to include the event itself.  This amount varies based on
           whether the time stamp is truncated or not. */

        /* Manipulate the event to match the packet format. */
        if (event_q_item->event_id.event_id_field.time_trunc_flag)
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_trunc_type);

          /* 'frag' now points to the start of the event */
          event_trunc = (event_trunc_type *) frag;

          /* Shift to capture 2nd and 3rd bytes for trunc time stamp */
          event_trunc->trunc_ts = (uint16)
            ((qw_lo (event_q_item->ts) << 8) >> (8 * 2));

          event_trunc->id = event_q_item->event_id.sized_field;

          event_trunc = NULL;     /* Done with this reference */
        }
        else
        {
          /* Adjust 'frag' and its length */
          frag -= sizeof (event_type);

          event_full = (event_type *) frag;

          /* Unless the length field is ommitted, the time stamp is already
             in place. */
          if (event_q_item->payload.length <= EVENT_PAY_TWO_BYTE)
          {
            (void) memmove ((void *) event_full->ts, (void *) event_q_item->ts, sizeof (qword));
          }
          else
          {
            /* Time stamp is already in place. No work needed here. */
          }

          /* Copy the event ID before the time stamp */
          event_full->id = event_q_item->event_id.sized_field;

          event_full = NULL;      /* Done with this reference */
        }
      }
      else
      {
        ERR_FATAL ("event_q_get returned NULL!", 0 ,0, 0);
      }
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  event_q_item_sav = event_q_item;
  send.pkt = (void *) frag;
  send.last = (void *) frag_last;
  finished_sav = finished;       /* return value */
  
  return finished;
}                               /* event_drain */

/*===========================================================================

FUNCTION EVENT_CONTROL

DESCRIPTION
  Enable/disable the event service.  Since multiple clients can configure 
  the service, a mask is used.  Only the bits in 'config' that have its
  corresponding bit in 'config_mask' set to '1' will effect the 
  configuration of the event service.
  
===========================================================================*/
static void
event_control (uint8 config, uint8 config_mask)
{
  uint8 change_mask;
   
  /* First, clear all unused bits from config mask. */
  config_mask &= EVENT_CONFIG_VALID_MASK;

  /* Next, set the bits that are masked to be set */
  change_mask = config & config_mask;
  event_config |= change_mask;

  /* Last, clear the bits that are masked to be cleared */
  change_mask = config | ~config_mask;
  event_config &= change_mask;

  if (event_config != 0)
  {
    (void) rex_set_timer (&event_stale_timer, EVENT_TIMER_LEN);
  }
  else
  {
    event_stale (); /* trigger events to be sent */
    
    (void) rex_clr_timer (&event_stale_timer);
  }
}                               /* event_control */

/*=========================================================================*/
void
event_flush (void)
{
  event_flush_last = (event_store_type *) q_last_check (&event_q.q);

  if (event_flush_last == NULL)
  {
    event_control (0, EVENT_CONFIG_COMM_MASK);
  }
}

/*===========================================================================

FUNCTION EVENT_STALE

DESCRIPTION
  This routine triggers the event service to wake up the DIAG task and send
  whatever is queued.  This is used to flush as well as to avoid stale
  events lingering in the event queues for extended periods of time.

ASSUMPTIONS
  This funciton is only called in DIAG task context.

===========================================================================*/
void
event_stale (void)
{
  if (!event_rpt_last)
  {
    event_rpt_last = (event_store_type *) q_last_check (&event_q.q);

  }

  /* Set the stale timer again to keep checking. */
  (void) rex_set_timer (&event_stale_timer, EVENT_TIMER_LEN);
  
  (void) rex_set_sigs (&diag_tcb, DIAG_TX_SIG);

  return;
}                               /* event_stale */

/*===========================================================================

FUNCTION EVENT_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.  
  
  This routine is a wrapper to call the registered listeners.
  
===========================================================================*/
static void
event_listener_notify (uint32 seq_num, void *event, diag_searchlist_node_type *node)
{
  event_listener_entry_type *entry = NULL;
  diag_event_type *event_notify = (diag_event_type *) event;
  
  if (node && event)
  {
    entry = (event_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, event_notify, entry->param);
    }    
  }
}

/*===========================================================================

FUNCTION EVENT_SERVICE_LISTENER

DESCRIPTION
  This routine is to be called once per event before the event is formatted
  for the outbound comm packet.
  
ASSUMPTIONS
  'event_store_type' and 'diag_event_type' are assumed to be formatted 
  the same, with the exception of the q_link at the beginning of the store
  type.  Changing either types could result in undesired behavior.

===========================================================================*/
static void
event_service_listener (event_store_type *event_store)
{
  if (event_store && (event_config & EVENT_CONFIG_LISTENER_MASK))
  {
    /* NOTE: diag_event_type and event_store_type purposely use the same
       format, except that event_store_type is preceeded by a Q link
       and the event ID field has a union for internal packet formatting.
       If either types are changed, the service will not function properly.
       &event_store->event_id is cast to diag_event_type * in 
       event_listener_notify(). */
    (void) diag_searchlist_search_all (event_store->event_id.id, 
                                &event_listeners, 
                                (void *) &event_store->event_id, /* cast!!! */
                                event_listener_notify);

  }
}

/*===========================================================================

FUNCTION EVENT_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each 
  node of the search until this routine returns TRUE or the search is 
  exhausted.  This routine is responsible for freeing the 'entry' which 
  was allocated in diag_add_event_listener().

===========================================================================*/
static boolean
event_listener_match_and_free (diag_searchlist_node_type *match_node, 
                               diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  event_listener_entry_type *match = NULL;
  event_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (event_listener_entry_type *) match_node->goo;
    entry = (event_listener_entry_type *) list_node->goo;

    if (match && entry)
    {
      if (match->listener == entry->listener &&
          match->param == entry->param)
      {
        found = TRUE;
        diag_free (entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_listener_match_and_free_cnt++;
        MSG_FATAL("FREE event_listener_match_and_free = %d, heap used = %d ",
            event_listener_match_and_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      }
    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_event_listener (const unsigned int event_id,
             const diag_event_listener listener, void *param)
{
  event_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  
  entry = (event_listener_entry_type *) 
    diag_malloc (sizeof (event_listener_entry_type));
  
  
#if defined (DIAG_APPS_PROC)
  diag_add_event_listener_on_modem(event_id, listener, param);
#endif

  if (entry)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_event_listener_malloc_cnt++;
    MSG_FATAL("MALLOC diag_add_event_listener_malloc = %d, heap used = %d ",
        diag_add_event_listener_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&event_listeners, event_id, entry);

    if (success)
    {
      /* Assume the critical section is initialized (success, right?). */
      rex_enter_crit_sect (&event_listeners.crit_sect);

      DIAG_SET_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);

      event_control (
        (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
        EVENT_CONFIG_LISTENER_MASK);

      rex_leave_crit_sect (&event_listeners.crit_sect);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_free ((void *) entry);
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_event_listener_free_cnt++;
      MSG_FATAL("FREE diag_add_event_listener_free_cnt = %d, heap used = %d ",
          diag_add_event_listener_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return success;
}

/*=========================================================================*/
boolean
diag_remove_event_listener (const unsigned int event_id,
                const diag_event_listener listener, void *param)
{
  event_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
  
  entry.listener = listener;
  entry.param = param;
  
  node.key = event_id;
  node.goo = (void *) &entry;

#if defined (DIAG_APPS_PROC)
  diag_remove_event_listener_on_modem(event_id, listener, param);
#endif

  success = diag_searchlist_delete (&event_listeners, node,
                                    event_listener_match_and_free);

  if (success)
  {
    /* Assume the critical section is initialized (success, right?). */
    rex_enter_crit_sect (&event_listeners.crit_sect);

    event_control (
      (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
      EVENT_CONFIG_LISTENER_MASK);

    if (event_listeners.count == 0)
    {
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);
    }

    rex_leave_crit_sect (&event_listeners.crit_sect);
  }

  return success;
}

/*===========================================================================

FUNCTION EVENT_INBOUND_PKT

DESCRIPTION
  This is the packet handler for the event service configuration packet.

===========================================================================*/
PACKED void *
event_inbound_pkt (PACKED void *req_pkt, uint16 pkt_len)
{
  event_cfg_req_type *req = (event_cfg_req_type *) req_pkt;
  event_cfg_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (event_cfg_rsp_type);
  uint8 config;
  uint8 set_mask;
  int i =0;
  boolean send_rsp = TRUE;

  (void) pkt_len; /* suppress compiler warning */

  if (req)
  {
    config = (req->enable) ? EVENT_CONFIG_COMM_MASK : 0;

    event_control (config, EVENT_CONFIG_COMM_MASK);

    rsp = (event_cfg_rsp_type *) diagpkt_alloc (DIAG_EVENT_REPORT_F, rsp_len);

    if (rsp)
    {
      rsp->length = 0;
    }

    set_mask = (config) ? 0xFF : 0;

    /* Set all the complete bytes to set_mask. Then set the
    ** bits in the last incomplete byte, if there is one.
    */
    for (i = 0; i < EVENT_MASK_SIZE; i++)
      event_mask[i] = set_mask;
  }

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }
}                               /* event_inbound_pkt */

/*
 * Packet processing functions.
 */

/*===========================================================================

FUNCTION EVENT_MASK_GET_HANDLER
DESCRIPTION
  Handles the EVENT_MASK_GET packet. Returns the current event mask.
============================================================================*/
PACKED void *
event_mask_get_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  event_mask_get_rsp_type *mask_get_rsp_ptr;
  dword rsp_len = FPOS(event_mask_get_rsp_type,mask);
  boolean send_rsp = TRUE;

  (void) req_ptr; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp_len += EVENT_MASK_SIZE;

  mask_get_rsp_ptr = (event_mask_get_rsp_type *)
                     diagpkt_alloc (DIAG_EVENT_MASK_GET_F, rsp_len);
  if (mask_get_rsp_ptr == NULL)
    return NULL;

  mask_get_rsp_ptr->error_code = 0;
  mask_get_rsp_ptr->reserved = 0;
  mask_get_rsp_ptr->numbits = EVENT_LAST_ID + 1;

  memcpy (EVENT_LARGE_MASK(mask_get_rsp_ptr->mask), event_mask, EVENT_MASK_SIZE);

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(mask_get_rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) mask_get_rsp_ptr);
  }

} /* END event_mask_get_handler */


/*===========================================================================

FUNCTION EVENT_MASK_SET_HANDLER
DESCRIPTION
  Handles the EVENT_MASK_SET packet. Sets the event mask.
============================================================================*/
PACKED void *
event_mask_set_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  event_mask_set_req_type *mask_set_req_ptr;
  event_mask_set_rsp_type *mask_set_rsp_ptr;
  uint16 numbits;
  uint8 last_byte_mask;
  dword rsp_len = FPOS(event_mask_set_rsp_type,mask);
  unsigned int mask_len = (unsigned int) pkt_len -
    FPOS (event_mask_set_req_type, mask);
  boolean send_rsp = TRUE;
  uint16 numbytes = 0;

 mask_set_req_ptr = (event_mask_set_req_type *) req_ptr;

  /* Adjust the specified number of mask bits using the number of bits
  ** in the packet and the number of bits in the mask array.
  */
  numbits = mask_set_req_ptr->numbits;
  if (numbits > mask_len * 8)
    numbits = (uint16) (mask_len * 8);
  if (numbits >= EVENT_LAST_ID)
    numbits = EVENT_LAST_ID + 1;
  numbytes = (numbits + 7)/8;

  rsp_len += EVENT_MASK_SIZE;
  mask_set_rsp_ptr = (event_mask_set_rsp_type *)
                     diagpkt_alloc (DIAG_EVENT_MASK_SET_F, rsp_len);
  if (mask_set_rsp_ptr == NULL)
    return NULL;

  mask_set_rsp_ptr->error_code = 0;
  mask_set_rsp_ptr->reserved = 0;
  mask_set_rsp_ptr->numbits = EVENT_LAST_ID + 1;

  /* Copy all the complete bytes from the input mask over. Then copy the
  ** bits in the last incomplete byte, if there is one.
  */
  memcpy (event_mask, EVENT_LARGE_MASK(mask_set_req_ptr->mask), numbits/8);

  last_byte_mask = 0xff;
  last_byte_mask <<= (numbits & 0x07);
  if (last_byte_mask != 0xff) {
/*  eq_ptr mask declared as unsigned char mask[1], used to store event_mask */
    event_mask[numbytes - 1] =
     (event_mask[numbytes - 1] & last_byte_mask) |
     (EVENT_LARGE_MASK(mask_set_req_ptr->mask)[numbytes - 1] & ~last_byte_mask);
  }

  memcpy (EVENT_LARGE_MASK(mask_set_rsp_ptr->mask), event_mask, EVENT_MASK_SIZE);

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_status())
  {
    send_rsp = FALSE;
  }

/* return response */
  if (!send_rsp)
  {
    diagpkt_free(mask_set_rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) mask_set_rsp_ptr);
  }

} /* END event_mask_set_handler */



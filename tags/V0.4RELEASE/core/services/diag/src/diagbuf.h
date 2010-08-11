#ifndef DIAGBUF_H
#define DIAGBUF_H

/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag output allocation manager.

Copyright (c) 1999-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                          Edit History

      $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagbuf.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
06/27/01   lad     diagbuf_blocking_alloc(): moved 'code' to diagpkt_alloc().
02/23/01   lad     Moved constants to diagtune.h.  Cosmetic cleanup for
                   rearchitected diag release.
11/07/00   jal     Renamed DIAGBUF_DATA_AVAIL_S to DIAGBUF_READY_S
09/01/00   pj      Increased DIAGBUF_MAX_SIZE to 10KB. Such a large buffer is
                   considered a temporary measure until the GPS SRCH code can
                   be sped up (it is currently starving the diag task, thus
                   requiring a large buffer).
08/28/00   lad     Removed use of DIAGBUF_MAX_SIZE.
06/19/00   lad     Moved some macros and prototypes from diagi.h.
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added DIAGBUF_NICE_SIZE for load balancing.  Updated API.
02/28/00   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "rex.h"
#include "diag.h"

#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
#include "diagpkt.h"

/*----------------------------------------------------------------------------
   Macro to build error packet -- Note that the commas cause the sizeof
   to be returned when this is used in an assignment, e.g.
   ret_val = ERR_PKT ( something );
----------------------------------------------------------------------------*/

#define ERR_PKT(code) diagpkt_err_rsp (code, req_ptr, pkt_len)


/* Note: Users of this iface fill in the command code themselves. */
#define diagbuf_pkt_alloc(length) diagpkt_alloc (0xFF, (unsigned int) length)

#endif /* FEATURE_DIAG_COUPLED_ARCHITECTURE */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the diag output allocation manager.

===========================================================================*/

typedef enum
{
  DIAGBUF_EMPTY_S,        /* Nothing in ring buffer or event buffer */
  DIAGBUF_READY_S,        /* Data from ring buffer queued to DSM, but ring
                             buffer still has more data to be queued--make */
                          /* sure diagbuf_drain() gets called again soon! */
  DIAGBUF_NOT_READY_S     /* Ring/event buffer has data to be queued to 
                             DSM, but we're blocked from queueing because
                             it's uncomitted, we don't have an SIO stream
                             yet, or we're flow-controlled by DSM. */
}
diagbuf_status_enum_type;

/* The send descriptor type. */
typedef enum 
{
  DIAG_SEND_STATE_START,
  DIAG_SEND_STATE_BUSY,
  DIAG_SEND_STATE_CRC1,
  DIAG_SEND_STATE_CRC2,
  DIAG_SEND_STATE_TERM,
  DIAG_SEND_STATE_COMPLETE
}
diag_send_state_enum_type;

typedef struct
{
  const void *pkt;
  const void *last; /* Address of last byte to send. */
  diag_send_state_enum_type state;
  boolean terminate;                 /* True if this fragment 
                                        terminates the packet */
#ifdef FEATURE_RSP_PRIORITY
  boolean priority;                                 
#endif /* FEATURE_RSP_PRIORITY */
}
diag_send_desc_type;


/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
  extern "C" {
#endif

/*===========================================================================

FUNCTION DIAGBUF_INIT

DESCRIPTION
  This function initializes the diag allocation buffer at startup.

============================================================================*/
void
diagbuf_init (void);

/*===========================================================================

FUNCTION DIAGBUF_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.

DEPENDENCIES
  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

RETURN VALUE
  Pointer to allocated buffer.  NULL if space is not available.

SIDE EFFECTS
  diagbuf_head may be updated.

============================================================================*/
PACKED void *
diagbuf_alloc (uint8 code, uint32 length, int nice);

/*===========================================================================

FUNCTION DIAGBUF_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagpkt_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.

============================================================================*/
void
diagbuf_shorten (PACKED void *ptr, unsigned int length);

/*===========================================================================

FUNCTION DIAGBUF_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
void
diagbuf_commit (PACKED void * ptr);

#if !defined(FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION DIAGBUF_DRAIN

DESCRIPTION
  This function drains the diag allocation buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.  It is not re-entrant!
  
  This is considered a background process for the DIAG task.  Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.
  
RETURN VALUE
  READY if more data can be sent
  NOT_READY if unable to proceed (uncommitted packet, flow control)
  EMPTY nothing to do

===========================================================================*/
diagbuf_status_enum_type
diagbuf_drain (rex_sigs_type mask);


#else /* !defined(FEATURE_DIAG_NON_STREAMING) */
/*===========================================================================

FUNCTION DIAGBUF_ENCODE_AND_SEND_PKT

DESCRIPTION
  This function takes a unencoded packet and transmits it via the comm layer.

  CRC, HDLC encoding, etc are all handled herein.  The function does not
  return until the packet is sent, or failure occurs.

RETURN VALUE
  TRUE if a packet was sent.

===========================================================================*/
boolean
diagbuf_encode_and_send_pkt (void *pkt, unsigned int length);

/*===========================================================================

FUNCTION DIAGBUF_SEND_SINGLE_BUFFERED_PKT

DESCRIPTION
  This function, used to emulate the old (diag version 7) poll/response
  protocol, sends a single packet from the buffer.  In this mode, all
  buffered packets are logs, though the contents are not referenced.

RETURN VALUE
  TRUE if the packet was sent successfully.

===========================================================================*/
boolean
diagbuf_send_single_buffered_pkt (void);

#endif /* !defined(FEATURE_DIAG_NON_STREAMING) */

/*===========================================================================

FUNCTION DIAGBUF_FLUSH

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed.

  diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

===========================================================================*/
void
diagbuf_flush (void);

/*===========================================================================

FUNCTION TYPE DIAGBUF_OUTBOUND_PKT_HANDLER_TYPE

DESCRIPTION
  This function type, if registered with a command code, is called from
  DIAG task context when the queued outbound pakcet is ready to send.

  diagbuf_drain() automatically processes packets in raw format.  However,
  some packets, such as those used in the debug message service, require
  post-processing in DIAG task context prior to sending.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

RETURN VALUE
  Number of bytes used, returned by diagbuf_send_pkt().

===========================================================================*/
typedef void (*diagbuf_outbound_pkt_handler_type) (diag_send_desc_type *desc);



/*===========================================================================

FUNCTION DIAGBUF_REGISTER_OUTBOUND_PKT_HANDLER

DESCRIPTION
  This function registers a function pointer to handle the sending of a
  packet.  diagbuf_drain() sends packets in raw format.  If processing/
  formatting needs to occur in DIAG task context prior to sending the
  packet, one can register a function pointer with the command code, which
  will be called when diagbuf_drain finds a packet with that command code.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

RETURN VALUE
  TRUE if registration was successful.

===========================================================================*/
boolean
diagbuf_register_outbound_pkt_handler (uint8 cmd_code, 
                                       diagbuf_outbound_pkt_handler_type fp);


/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION
  Sends a packet, or a fragment of a packet, described by 'desc', to the
  comm layer.
  
  This handles buffer management with the comm layer and all transport layer
  related ops, like CRC calculation HDLC encoding.

===========================================================================*/
diagbuf_status_enum_type
diagbuf_send_pkt (diag_send_desc_type *desc);
#ifdef DIAG_MP
diagbuf_status_enum_type diagbuf_send_pkt_multi_proc (
         diag_send_desc_type *desc, boolean drain_timer_expired);
#endif

#ifdef __cplusplus
  }
#endif

#endif /* DIAGBUF_H  */

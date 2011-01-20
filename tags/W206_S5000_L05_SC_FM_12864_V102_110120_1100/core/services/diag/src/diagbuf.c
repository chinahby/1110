/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Subsystem Buffer Allocation Manager

General Description
  This file contains routines that manage the diag output buffer.
  This allocation scheme is modeled as a circular buffer.

  Different types of data have different priorities.  These types and
  priorities are listed below in descending order of priority:

  1.  Event reports
  2.  Explicit diag responses
  3.  Streamed data (LOG, MSG, etc)

  Events are recorded at the head of the buffer, and will be accumulated
  until terminated (by event reporting code in event.c, etc)  A specified amount
  of space is reserved for event reporting.  If the buffer gets full (up to
  specified amount of space for events), only events will be allowed in the buffer.

  The diag task has the ability to block other tasks from accessing buffer
  space.  If diag is unable to get space, it will block other tasks (except events)
  and retry until it can get space.  Only event reports will be able to allocate space
  while diag is blocking.

Initialization and Sequencing Requirements
  diagbuf_init() must be called before using this service.

Copyright (c) 1999-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagbuf.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/08   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
02/15/07   as      Made diagbuf_tail volatile. Moved sanity checks within 
                   INTLOCKS. 
12/01/06   as      Moved diagbuf_write_overrun_check_pattern inside INTLOCK().
                   Added sanity checks in alloc and diagbuf_drain().
10/31/05   as      Fixed lint errors.
10/19/05   as      Fixed CRC update in hdlc encode.
02/15/05   as      Removed PLATFORM_LTK feature to allow data send to be split
                   into different packets.
01/12/05   as      Fixed hdlc encode bug in CRC
01/07/05   as      Modified the location of check for empty diag buffer. This 
                   fixes the invalid check for empty diag heap.
12/06/04   as      Included dog.h to prevent compiler warnings
09/21/04   as      Changed hdlc encode to fill dsm items with 128 bytes. But,
                   if the last byte is an escape character (0x7D or 0x7E),then 
                   only 127 bytes are filled in dsm item.
03/30/03   as      Added ASSERTS in functions that manipulate the diag buffer 
                   and intlocks in diagbuf wrap and move macros.
10/28/03   as      Changes to fix warnings on ads12 compiler
12/02/02   lad     Removed blocking alloc (to diagpkt.c), enhacned packet
                   bounadary processing to diagbuf_drain() to give priority
                   to diagpkt rsp packets, and added periodic sleep logic with
                   inert defaults to avoid starvation of lower priority tasks.
07/30/02   as      Updated diagbuf_get_log and diag_send_tx_packet to support
                   FEATURE_DIAG_NON_STREAMING
06/25/02   as      Changed diagbuf_internal_alloc, diagbuf_shorten, diagbuf_commit,
                   and diagbuf_drain to obtain byte allignment.
10/24/01   jal     Changed diagbuf_flush() to wait for SIO to finish
                   flushing UART before returning--guarantees last
                   response packet gets sent.
06/27/01   lad     Made private data and functions static.
                   Removed "code" from diagbuf_blocking_alloc() - this is
                   done in diagpkt_alloc().
                   Updated usage of log_config().
06/22/01   lad     Fixed diagbuf_drain to check for committed packet after wrap.
                   Changed diagbuf_buf to be aligned on 32-bit boundary.
04/17/01   lad     Modified diagbuf_alloc(), diagbuf_shorten(), and
                   diagbuf_commit() to detect buffer overrun by users.
                   Changed call to diag_do_escaping() to remove const args.
02/23/01   lad     Made changes for newly architected diagnostics service.
                   Introduced the communications layer and the use of diagtune.h
                   to configure this code.
                   Many cosmetic changes were made as well.
11/28/00   lad     Changed diagbuf_drain() to return correct status when no
                   open communications stream exists.
11/10/00   jal     Killed third call to event_drain() in diagbuf_drain().
                   It was causing an endless loop when there are no events
                   and only un-committed data in the ring buffer.  It isn't
                   needed anyways.
11/07/00   jal     Bugfixes to diagbuf_drain().  Moved handling
                   DIAG_EVENT_TIMER_SIG out of diagpkt_blocking_alloc() into
                   diag.c/diag_wait() per Larry.  Removed diag_set/clr_sigs
                   debug hack.
10/17/00   jal     Moved diagbuf_OKTX, diagbuf_start/stop_flow() to diag.c
                   Moved event_stale_timer over to event.c.  Mods to
                   support new event accumulation mechanism.
09/25/00   jal     Rewrote diagbuf_drain() to a) use the new DSM item
                   interface, and b) to use DSM items more efficiently.
                   Instead of using one item for each response packet
                   (wasteful!) we fill each item until we have nothing
                   more to send.
09/12/00   lad     Fixed load balancing check in diagbuf_alloc_internal.
08/28/00   lad     Optimized the routines in this module.
08/25/00   rc      Changed dsm_new_buffer() calls to take pool id as input
                   parameter.
07/06/00   jal     Changed KICK_WATCHDOG to DIAG_KICK_WATCHDOG to avoid
                   conflict with other (FS) VUs
06/23/00   lad     Removed obsolete diag_sleep reference.
06/19/00   lad     Added MSM5000_IRAM_FWD aroung diagbuf_alloc_internal()
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added load balancing.  Changed API to give diag_rsp_type*
                   instead of void*
03/07/00   lad     Fixed flow control for FEATURE_DIAG_NON_STREAMING.
02/28/00   lad     Created file.

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "AEEstd.h"
#include "diag.h"
#include "diagbuf.h"
#include "diagcomm.h"
#include "diagdiag.h" /* For now, this is needed to detect log overrun */
#include "diagi.h"
#include "diagpkt.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"

#include <stdio.h>
#include "assert.h"
#include "crc.h"
#include "rex.h"
#include "task.h"
#include "dog.h"

/*lint -esym(751, diagbuf_header_status_enum_type) */

/* Supress local typedef 'diagbuf_header_status_enum_type' not referenced */

/*---------------------------------------------------------------------------
  The following are indices for the ring buffer used for streaming diag
  output from the DMSS.  Each packet will be stored in the following format.
  Numbers are expressed as bytes.

  -----------------------------
  |                           |
  |  Length (2)               |  <-- Length of entire packet
  |                           |      (Data + CRC + Length field)
  -----------------------------
  |                           |
  |                           |
  |  Output Data (Length - 4) |  <-- Buffer given to caller (ptr to this)
  |                           |
  |                           |
  -----------------------------
  |                           |
  |  CRC (2)                  |  <-- CRC required for transport protocol
  |                           |
  -----------------------------

---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  The following variables are used to manage the ring buffer allocation scheme.
  The circular model is composed of a linear buffer of size 2*N where N is the
  size of the buffer.  The index to the buffer = idx % N;  This handles buffer
  wrap since head - tail will always yield the amount of allocated space.

  diagbuf_tail (T) - The end of the used buffer.  Data is drained to the SIO
                     queues from here.

  diagbuf_head (H) - The front of buffer (where writers allocate)

  With this scheme, the following calculations hold true:

  Empty condition:        H == T
  Full condition:         H - T == N
  Amnt used:              H - T

  Since 'int2' operations are atomic, the only critical section is
  moving the head.

----------------------------------------------------------------------------*/



/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*===========================================================================

MACRO DIAGBUF_EMPTY
MACRO DIAGBUF_FULL

DESCRIPTION
  Conditional macros for diag streaming buffer.

RETURN VALUE
  Conditional values.

===========================================================================*/
#define DIAGBUF_EMPTY(h, t) (h == t)



/*===========================================================================

MACRO DIAGBUF_USED
MACRO DIAGBUF_FREE

DESCRIPTION
  Status calculating macros for the diag ring buffer.

PARAMETERS
  h - head index
  t - tail index

RETURN VALUE
  Number of bytes.

===========================================================================*/
#define DIAGBUF_USED(h, t) ((h >= t) ? (h - (t)) : (h - (t) + 2*DIAGBUF_SIZE))

#define DIAGBUF_FREE(h, t) (DIAGBUF_SIZE - DIAGBUF_USED(h, t))



/*===========================================================================

MACRO DIAGBUF_CALC_IDX

DESCRIPTION
  Calculates index in DIag output buffer.

PARAMETERS
  Abstract buffer index

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

RETURN VALUE
  Index in diag ring buffer.

===========================================================================*/
#define DIAGBUF_CALC_IDX(x) ((x >= DIAGBUF_SIZE) ? (x - DIAGBUF_SIZE) : x)



/*===========================================================================

MACRO DIAGBUF_WRAP

DESCRIPTION
  Wraps abstract buffer index.

PARAMETERS
  Abstract buffer index.

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

===========================================================================*/
#define DIAGBUF_WRAP(x)                    \
do {                                       \
  INTLOCK();                               \
  x += DIAGBUF_SIZE - DIAGBUF_CALC_IDX(x); \
  if (x >= 2 * DIAGBUF_SIZE)               \
    x -= 2 * DIAGBUF_SIZE;                 \
  INTFREE();                               \
/*lint -save -e{717} */                      \
} while(0)                                 \
/*lint -restore */



/*===========================================================================

MACRO DIAGBUF_MOVE

DESCRIPTION
  Moves the buffer index.

PARAMETERS
  Buffer index.

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

===========================================================================*/
#define DIAGBUF_MOVE(i, l)   \
do {                         \
  INTLOCK();                 \
  i += l;                    \
  if (i >= 2 * DIAGBUF_SIZE) \
    i -= 2 * DIAGBUF_SIZE;   \
  INTFREE();                 \
/*lint -save -e{717} */        \
} while(0)                   \
/*lint -restore */



/*===========================================================================

MACRO DIAGBUF_NEXT_ALIGNED_BYTE

DESCRIPTION
  Moves the variable the next aligned byte

===========================================================================*/
#define DIAGBUF_NEXT_ALIGNED_BYTE(p) (((p) + 3) & ~3)


/* Diag's output buffer.  The ADS 1.0 compiler will not align a byte array on
 * 32-bit boundary.  The ARM 2.5 compiler had a setting for this, but not ADS
 * 1.0.  Therefore, we must declare it to be a array of 32-bit values and
 * assign a byte* to point to it. */
static uint32 diagbuf_buf_aligned[(DIAGBUF_SIZE + 3)/4];
static uint8 *diagbuf_buf = (uint8 *) diagbuf_buf_aligned;

/* Head and tail of circular buffer.
*/
static volatile int diagbuf_head = 0;
static volatile int diagbuf_tail = 0;

/* Flag used for software flow control.
*/
boolean diagbuf_OKTX = TRUE;

#ifndef DIAGBUF_PKT_HANDLER_TBL_CNT
/* For now, only 2 packets, DIAG_EXT_MSG_F and DIAG_LOG_F use this feature */
#define DIAGBUF_PKT_HANDLER_TBL_CNT 2
#endif

static struct {
  uint8 cmd_code;
  diagbuf_outbound_pkt_handler_type fp;
} diagbuf_outbound_pkt_handler_tbl [DIAGBUF_PKT_HANDLER_TBL_CNT];

/*lint -save -e{641} */
typedef enum
{
  DIAGBUF_UNCOMMIT_S,  /* Notify that the buffer is not commited    */
  DIAGBUF_COMMIT_S,    /* Notify that the buffer is commited        */
  DIAGBUF_WRAP_S,      /* Notify that the buffer has to be wrapped  */
  DIAGBUF_HOLE_S,      /* Notify that the buffer has a hole         */
  DIAGBUF_UNUSED_S     /* Notify that the buffer is unused          */
} diagbuf_header_status_enum_type;
/*lint -restore */

typedef struct
{
  uint8  status;  /* stores the header status */
  uint8  pad;     /* stores the pad to make it byte aligned */
  uint16 length;  /* stores the length of the buffer */
} diagbuf_header_type;

typedef struct
{
  void *dest;
  void *dest_last;
  
  /* Below: internal use only */
  uint16 crc;
} diag_hdlc_dest_type;

/* External declrations */
extern boolean diagpkt_rsp_send (void);

/* Forward prototype declarations for local routines. */
void diag_hdlc_encode (diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc);
static void diagbuf_start_cb (void);
static void diagbuf_stop_cb (void);
static void diagbuf_write_overrun_check_pattern (void *ptr);

#ifndef FEATURE_DIAG_NON_STREAMING
static diagbuf_outbound_pkt_handler_type
  diagbuf_get_custom_pkt_handler (uint8 cmd_code);
#endif

#ifdef FEATURE_REX_CORE_DUMP
  #ifndef FEATURE_L4
   extern void rex_jettison_core( void );
  #endif
   extern void rex_log_core_dump(
      uint16      line,
      const char *filename,
      const char *fmt,
      uint32      p1,
      uint32      p2,
      uint32      p3
   );
#endif


/*=========================================================================*/
void diagbuf_init (void)
{
  diagbuf_header_type *pkt_hdr; /* pointer to the first header */
  int i;

  diagbuf_head = 0;
  diagbuf_tail = 0;

  /* Setting the first status field to DIAGBUF_UNCOMMIT_S is all the
  ** initizalization needed
  */
  pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[0]);
  pkt_hdr->status = DIAGBUF_UNUSED_S;

  /* Register flow control callbacks */
  diagcomm_register_outbound_flow_ctrl(diagbuf_start_cb, diagbuf_stop_cb);

  for (i = 0; i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
  {
    diagbuf_outbound_pkt_handler_tbl[i].cmd_code = 0xFF;
    diagbuf_outbound_pkt_handler_tbl[i].fp = NULL;
  }

} /* diagbuf_init */



#ifndef MSM5000_IRAM_FWD /* Flag to use internal RAM */
/*===========================================================================

FUNCTION DIAGBUF_ALLOC_INTERNAL

DESCRIPTION
  This function allocates the specified amount of space in the diag
  output buffer.

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

============================================================================*/
static PACKED void * diagbuf_alloc_internal (
  uint32 length, /* Length of requested buffer */
  int nice    /* Niceness padding value     */
)
{
  PACKED void *ptr = NULL;  /* Pointer to return.        */
  uint32 buffer_used;          /* Length to allocate        */
  uint32  sav_var;          /* Save variable for INTLOCK */
  diagbuf_header_type *pkt_hdr;

  /* Local indices to manipulate buffer */
  int iHead;
  int iHeadIdx;
  int iTail;
  int iTailIdx;

  INTLOCK_SAV(sav_var);

  /* Sanity check*/
  if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
  }

  if (length > DIAG_MAX_TX_PKT_SIZ) {
    MSG_LOW("Attempt to alloc too much: %d", (dword) length, 0, 0);
    INTFREE_SAV(sav_var);
    return NULL;
  }

  buffer_used =
             sizeof(diagbuf_header_type) +  /* Diagbuf header    */
             (uint32)length     +  /* Number of bytes requested buffer */
             sizeof(uint16);   /* CRC                              */

  buffer_used = DIAGBUF_NEXT_ALIGNED_BYTE(buffer_used);

  ASSERT (buffer_used <= DIAGBUF_SIZE);

  /*-------------------------------------------------
    To avoid critical section for tail, read once
    and use that value.  This operation is atomic.
  -------------------------------------------------*/
  iTail = diagbuf_tail;
  iTailIdx = DIAGBUF_CALC_IDX(iTail);

  iHead = diagbuf_head;
  iHeadIdx = DIAGBUF_CALC_IDX(iHead);

  ASSERT (iHead < 2 * DIAGBUF_SIZE);
  ASSERT (iTail < 2 * DIAGBUF_SIZE);
  ASSERT (iHeadIdx < DIAGBUF_SIZE);
  ASSERT (iTailIdx < DIAGBUF_SIZE);

  /* Set a local pointer to the header */
  pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[iHeadIdx]);

  /* First check if the nice pad is available.  If OK, then alloc */
  if (DIAGBUF_BALANCE_PAD + nice < DIAGBUF_FREE(iHead, iTail)) {
    /* if not wrapped and no room here. */
    if (iTailIdx <= iHeadIdx          &&       /* Not wrapped &&  */
        iHead - iTail < DIAGBUF_SIZE  &&       /* Not full &&     */
                                               /* No room here!   */
      (uint32)iHeadIdx + buffer_used >= DIAGBUF_SIZE) {

      /* Mark header for wrap */
      pkt_hdr->status = DIAGBUF_WRAP_S;

      /* Wrap */
      DIAGBUF_WRAP(iHead);
      ASSERT ((iHead == 0) || (iHead == DIAGBUF_SIZE));

      /* Recalculate head index */
      iHeadIdx = DIAGBUF_CALC_IDX(iHead);
      ASSERT (iHeadIdx == 0);
    }

    if (buffer_used <= (unsigned int) DIAGBUF_FREE(iHead, iTail)) {
      /* Move head */
      DIAGBUF_MOVE(iHead, (int)buffer_used);

      ASSERT (iHead < 2 * DIAGBUF_SIZE);

      /* Update value of global head index */
      diagbuf_head = iHead;

      /* Sanity check */
      if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE) {
        ERR_FATAL("Ring buffer currupt!", 0, 0, 0);
      }

      /* Set header field of buffer */
      pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[iHeadIdx]);

      pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
      pkt_hdr->pad = (byte)(buffer_used - (uint32)length - sizeof(diagbuf_header_type)
          - sizeof(uint16));
      pkt_hdr->length = (uint16)length;

      /* Set 'ptr' to the byte following the header */
      ptr = pkt_hdr + 1;
    }
  } /* If requested length + nice pad is free */

  if (ptr) {
    diagbuf_write_overrun_check_pattern(pkt_hdr);
  }

  /* Sanity check*/
  if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
  }

  INTFREE_SAV(sav_var);

  return ptr;
} /* diagbuf_alloc_internal */
#endif /* !MSM5000_IRAM_FWD */



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

============================================================================*/
PACKED void * diagbuf_alloc (uint8 code, uint32 length, int nice)
{
  PACKED void *ptr = NULL; /* Pointer to return.        */

  //TODO: Move this function here.
  ptr = diagbuf_alloc_internal (length, nice);

  if (ptr)
  {
    diagpkt_set_cmd_code (ptr, code);
  }

  return ptr;
} /* diagbuf_alloc */



/*==========================================================================*/
void diagbuf_shorten (
  PACKED void *ptr,       /* Pointer returned from diagbuf_alloc() */
  unsigned int new_length /* New length of buffer                  */
)
{
  diagbuf_header_type   *pkt_hdr; /* Points to packet header */
  diagbuf_header_type   *hole; /* This will point to new header field. */

  unsigned int curr_length;
  unsigned int hole_length;

  /* Sanity check.  Make sure it is within boundaries of ring buffer */
  if ((byte *) ptr <  diagbuf_buf ||
      (byte *) ptr >= (&diagbuf_buf[DIAGBUF_SIZE] - sizeof(uint16))) {
    /* This is bad.  Incorrect usage. Report error and do nothing.*/
    MSG_LOW("Invalid packet address: ptr:0x%x, l:%d", ptr, new_length, 0);
    return;
  }

  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *) ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header */
  pkt_hdr -= 1;

  curr_length = pkt_hdr->length;

  if (new_length == 0) {
    /* Special case: shorten to 0.
     * The entire packet is just changed to be a hole
     */
    hole = pkt_hdr;

    /* length of the hole is current length + CRC + pad */
    hole->length = (uint16)DIAGBUF_NEXT_ALIGNED_BYTE (curr_length + 
        sizeof(uint16));

    ASSERT (hole->length < DIAGBUF_SIZE);

    hole->status = DIAGBUF_HOLE_S;
    hole->pad = 0;

#if !defined (FEATURE_DIAG_NON_STREAMING)
    /* Since this essentially commits the packet as a hole, notify DIAG
       that the caller is ready to TX. */
    diag_tx_notify ();
#endif

  } else {

    /* If new length is longer or the same, we do nothing. */
    if (curr_length > new_length) {
      /* Compute hole size and record new packet length */

      pkt_hdr->length = (uint16)new_length;

      /* Adjust length fields to include CRC fields */
      new_length += sizeof (uint16);
      curr_length += sizeof (uint16);

      /* Calcualte pad for the packet */
      pkt_hdr->pad = (byte)(DIAGBUF_NEXT_ALIGNED_BYTE (new_length) - new_length);

      /* Now change length fields to be the length of entire buffer used */
      new_length += pkt_hdr->pad;
      curr_length = DIAGBUF_NEXT_ALIGNED_BYTE (curr_length);

      ASSERT (curr_length < DIAGBUF_SIZE);

      hole = (diagbuf_header_type *) ptr;

      /* Move to next aligned address past the new packet length */
      hole += new_length / sizeof (*hole);

      hole_length = curr_length - new_length;

      if (hole_length >= sizeof (diagbuf_header_type)) {
        hole->status = DIAGBUF_HOLE_S;
        hole->length = (uint16)hole_length - sizeof (diagbuf_header_type);
        hole->pad = 0;
      }

      diagbuf_write_overrun_check_pattern (pkt_hdr);

    } else if (new_length > curr_length) {
      MSG_LOW("Attempted shorten %d to %d ignored.",
        curr_length, new_length, 0);
    } else {
      /* If shortening to same size, do nothing. */
    }
  }
} /* diagbuf_shorten */



#if !defined(_disable)
  extern void _disable(void);
#endif

static boolean 
diagbuf_check_overrun_pattern (diagbuf_header_type *hdr)
{
  PACKED uint16 *pattern = NULL;
  boolean success = TRUE;

  /* The 2 byte CRC field at the end of the allocated buffer is written
   * with a mask to detect buffer overrun.  Check to make sure that value
   * wasn't corrupted.
   */
  pattern = (PACKED uint16 *)(((byte *) hdr) + hdr->length +
      sizeof(diagbuf_header_type));

  if (*pattern != 0xDEAD)
  {
    uint16 line = __LINE__;
    diag_log_rsp_type *logpkt;
    char culprit[40];

    memset(culprit,0,40);
    success = FALSE;

    /* This detects a memory scribble.  There is no way to know how far
       past the allocated buffer the caller scribbled.  If this is
       determined to be a log, print the log code in lieu of the file
       name on the LCD.  Next, disable interrupts to avoid user of
       corrupt memory.  Reset diagbuf_head/tail so panic mode won't
       attempt to flush it, then enter panic mode as usual. */

    /* Disable all interrupts to avoid any more diagnostics client
       activitiy.  */
    _disable ();

    /* Reset diagbuf to avoid processing scribbled memory. */
#ifndef DIAGBUF_HEAP
    diagbuf_tail = diagbuf_head;
#endif

#ifdef FEATURE_REX_CORE_DUMP
    /* This does some logging.  Since interrupts are disabled, all
       other clients should not be able to commit their packets. */
    rex_jettison_core();
    rex_log_core_dump(line, ERR_FILE_NAME, culprit, 0,0,0 );
#endif /* FEATURE_REX_CORE_DUMP */

/*lint -save -e{740} Unusual pointer cast*/
    logpkt = (diag_log_rsp_type *) hdr + 1;
/*lint -restore */

    switch (diagpkt_get_cmd_code (logpkt))
    {
      case DIAG_LOG_F:
        /* Display that a log is the culprit in lieu of filename. */

        std_strlprintf(culprit, sizeof(culprit), "Log %04X pri:", log_get_code (logpkt->log));

        boot_clear_sl();
        err_fatal_put_log ((uint16) rex_get_pri(), culprit);
        break;

      default:

        /* This is still an error, though not from a log. */
        boot_clear_sl();
        err_fatal_put_log (line, ERR_FILE_NAME);
        break;
    }
  }

  return success;
}

#ifndef MSM5000_IRAM_FWD /* Flag to use internal RAM */
/*===========================================================================

FUNCTION DIAGBUF_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
void diagbuf_commit (
  PACKED void * ptr /* Buffer to commit */
)
{
  diagbuf_header_type *pkt_hdr; /* Pointer to the header of the buffer. */

  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *) ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header field */
  pkt_hdr -= 1;

  /* Sanity checks */
  if (ptr == NULL                                ||
      (byte*) ptr < &diagbuf_buf[sizeof(uint16)] ||
      (byte*) ptr >= &diagbuf_buf[DIAGBUF_SIZE]
     ) {
    MSG_LOW("Invalid commit address 0x%x.", (dword) ptr, 0, 0);
    return;
  }

  if (pkt_hdr->status != DIAGBUF_HOLE_S)
  {
    if (diagbuf_check_overrun_pattern (pkt_hdr))
    {
      /* Set status to "committed" */
      pkt_hdr->status = DIAGBUF_COMMIT_S;
    }
  }

#ifndef FEATURE_DIAG_NON_STREAMING
  diag_tx_notify ();
#endif

} /* diagbuf_commit */
#endif /* !MSM5000_IRAM_FWD */


/* Private functions */

/*===========================================================================

FUNCTION DIAGBUF_SIO_TX_START_CB
FUNCTION DIAGBUF_SIO_TX_STOB_CB

DESCRIPTION
  Flow control function callbacks for sending data.

  The diag task will stop/start sending as a result of these functions.

===========================================================================*/
static void diagbuf_start_cb( void )
{
  diagbuf_OKTX = TRUE;
  (void) rex_set_sigs(&diag_tcb, DIAG_TX_SIG);
}

static void diagbuf_stop_cb( void )
{
  diagbuf_OKTX = FALSE;
}



/*===========================================================================

FUNCTION DIAGBUF_WRITE_OVERRUN_CHECK_PATTERN

DESCRIPTION
  Writes the overrun check pattern in the CRC field at the end of the bufer
  pointed to by 'ptr'.  diagbuf_commit() checks this field to determine if
  buffer overrun has occured.

  The work to check this is done in diagbuf_commit() rather than a seperate
  function to save the cost of calculations and the function call.

  'ptr' points to the length field before the packet, not the beginning of
  the packet itself.

===========================================================================*/
static void diagbuf_write_overrun_check_pattern (void *ptr)
{
  unsigned int length;

  if (ptr)
  {
    length = ((diagbuf_header_type *) ptr)->length;

    /* The reserved space for the CRC is be used as a sanity check.
     * A pattern is written to those bytes in an effort to detect
     * buffer overrun.
     * The Head index is pointing to the length field.  We want to write
     * the CRC, which is 'length - sizeof(crc)' bytes ahead.
     */
    ptr = ((byte *) ptr) + length + sizeof(diagbuf_header_type);
    *((PACKED uint16 *) ptr) = 0xDEAD;
  }
} /* diagbuf_write_overrun_check_pattern */

/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION
  This function sends a packet, or a fragment of a packet, through the comm
  layer.  It handles HDLC encoding as well as flow control.

  If 'pkt' is NULL, the buffer in progress is flushed to the comm layer,
  making no assumptions about HDLC, packet termination, etc.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
/* For now, this feature is inert.  tx_sleep_time = 0 means no sleep will
occur.  It will remain in this state until proper values are established.
However, the implemenatation is tested, allowing testers to poke values to
avoid watchdog timeouts. */
unsigned int diagbuf_tx_sleep_threshold = (1024 * 8);
rex_timer_cnt_type diagbuf_tx_sleep_time = 20; //XXX 50ms worked before.

#ifndef MSM5000_IRAM_FWD /* Flag to use internal RAM */
/* If NULL, flush current buffer. */
diagbuf_status_enum_type diagbuf_send_pkt (diag_send_desc_type *desc)
{
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  unsigned int outbound_used = 0;

  
  /* Persistent data */
  static byte *outbound_sav = NULL; /* Pointer to memory allocated by the comm layer. */
  static unsigned int tx_cnt_sav = 0;
  static diag_hdlc_dest_type enc = {NULL, NULL, 0};

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  byte *outbound = outbound_sav;
  unsigned int tx_cnt = tx_cnt_sav;

  if (!(diagcomm_status() && DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S)))
  {
    /* DIAG is not connected to the comm layer.  Listeners may still be 
       active, though.  Throw away any buffer that may be lingering. */
    if (outbound != NULL)
    {
      diagcomm_send (outbound, 0);
      outbound = NULL;
    }

    /* Now discard inbound data by adjusting the descriptor to indicate
       that all data was sent.  Return status that we are ready for more.  */
    if (desc)
    {
      uint8 *ptr = (uint8 *) desc->last;
      
      ptr++;
      
      desc->pkt = (void *) ptr;
      desc->state = DIAG_SEND_STATE_COMPLETE;
      status = DIAGBUF_READY_S;
    }
  }
  else if (desc == NULL)
  {
    /* Flush the buffer to the comm layer. */
    if (outbound != NULL)
    {
      outbound_used = (uint32) enc.dest - (uint32) outbound;

      diagcomm_send (outbound, outbound_used);

      /* Note: This occurs when diag is about to give up control of the CPU.
         If tx_cnt happens to pass the threshold, the task suspension will
         occur during the next pass. */
      tx_cnt += outbound_used;

      outbound = NULL;
    }
  }
  else
  {
    while (status == DIAGBUF_READY_S && desc->state < DIAG_SEND_STATE_COMPLETE)
    {
      if (outbound == NULL && diagbuf_OKTX)
      {
        //TODO: Use a constant in diagtune.h, even though it is benign w/ SIO.
  
        /* Allocate memory from the communications layer */
        outbound = diagcomm_outbound_alloc (1000, &outbound_used);

        enc.dest = (void *) outbound;
        enc.dest_last = (void *) ((byte *) outbound + outbound_used - 1);
      }
  
      if (outbound == NULL)
      {
        /* In case this is the start state, call encode to transition the 
           state to "busy".  Some callers use this state to detect a need
           for other initialization.  Forcing state transition avoids
           multiple initialization. */
        enc.dest = enc.dest_last = NULL;
        diag_hdlc_encode (desc, &enc);

        status = DIAGBUF_NOT_READY_S;
      }
      else
      {
        diag_hdlc_encode (desc, &enc);
      
        /* After each iteration, check if outbound buffer is exhausted.  If so,
           send it and continue the loop. */
  
        /* diag_hdlc_encode() requires at least 2 bytes in 'enc'. */
        if (enc.dest >= enc.dest_last)
        {
          outbound_used = (uint32) enc.dest - (uint32) outbound;

          diagcomm_send (outbound, outbound_used);
          tx_cnt += outbound_used;
          outbound = NULL;
        }

        /* To ensure lower priority tasks are not starved by DIAG, force task
           suspension when # of TX bytes passes the threshold. */
        if (tx_cnt >= diagbuf_tx_sleep_threshold)
        {
          diag_suspend (diagbuf_tx_sleep_time);
          tx_cnt = 0;
        }
      }
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  outbound_sav = outbound;
  tx_cnt_sav = tx_cnt;
  
  return status;
}
#endif /* MSM5000_IRAM_FWD */

/*===========================================================================

FUNCTION DIAG_HDLC_ENCODE

DESCRIPTION
  This routine copies packet fragments from *src_desc to *enc.
  
  While copying, it performs step-wise CRC calculations and HDLC encoding.
  
  These descriptor structures contain memory ranges, states, etc to handle
  packetization.
  
  If enc->dest is NULL and src_desc is in the "start" state, CRC is 
  initialized and the state is transitioned to "busy".
  
DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
void diag_hdlc_encode (diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc)
{
  uint8 *dest;
  uint8 *dest_last;
  const uint8 *src;
  const uint8 *src_last;
  uint16 crc;
  byte src_byte = 0;
  diag_send_state_enum_type state;

  if (src_desc && enc)
  {
    /* Copy important parts to local variables. */
    src = src_desc->pkt;
    src_last = src_desc->last;
    state = src_desc->state;
    dest = enc->dest;
    dest_last = enc->dest_last;
    
    if (state == DIAG_SEND_STATE_START)
    {
      crc = CRC_16_L_SEED;
      state++;
    }
    else
    {
      /* Get a local copy of the CRC */
      crc = enc->crc;
    }

    /* dest or dest_last may be NULL to trigger a state transition only */
    if (dest && dest_last)
    {
      /* This condition needs to include the possiblitiy of 2 dest 
         bytes for an escaped byte */
      while (src <= src_last && dest <= dest_last)
      {
        src_byte = *src++;
  
        if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
            (src_byte == DIAG_ASYNC_ESC_CHAR))
        {
  
          /* If the escape character is not the last byte */
          if( dest != dest_last ) 
          {
            crc = CRC_16_L_STEP (crc, src_byte);
            *dest++ = DIAG_ASYNC_ESC_CHAR;
            *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
          }else
          {
            /* no space available to copy the escape character, so pack 
              it in the next one */
            src--;
            break;
          }
        }
        else
        {
          crc = CRC_16_L_STEP (crc, src_byte);
          *dest++ = src_byte;
        }
      }
  
      if (src > src_last)
      {
        if (state == DIAG_SEND_STATE_BUSY)
        {
          if (src_desc->terminate)
          {
            crc = ~crc;
            state++;
          }
          else
          {
            /* Done with fragment, no CRC/term to be done. */
            state = DIAG_SEND_STATE_COMPLETE;
          }
        }
  
        while (dest <= dest_last && state >= DIAG_SEND_STATE_CRC1 && 
               state < DIAG_SEND_STATE_TERM)
        {
          /* Encode a byte of the CRC next */
          src_byte = crc & 0xFF;
  
          /* Copied from above for runtime efficiency */
          if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
              (src_byte == DIAG_ASYNC_ESC_CHAR))
          {
  
            if( dest != dest_last ) 
            {
              *dest++ = DIAG_ASYNC_ESC_CHAR;
              *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
              /* Shift to the next CRC byte.  CRC is 0 when complete. */
              crc >>= 8;
            }else
            {
              /* no space available to copy the crc, pack it in the next one */
              break;
            }
          }
          else
          {
            /* Shift to the next CRC byte.  CRC is 0 when complete. */
            crc >>= 8;
            *dest++ = src_byte;
          }
  
          state++;
        }
  
        if (state == DIAG_SEND_STATE_TERM)
        {
          if (dest_last >= dest)
          {
            *dest++ = DIAG_ASYNC_CONTROL_CHAR;
            state++; /* Complete */
          }
        }
      }
    }
    /* Copy local variables back into the encode structure. */
    enc->dest = dest;
    enc->dest_last = dest_last;
    enc->crc = crc;
    src_desc->pkt = src;
    src_desc->last = src_last;
    src_desc->state = state;
  }

  return;
}


#if !defined (FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION DIAGBUF_PROCESS_PKT_BND

DESCRIPTION
  This function sends all diagnostic data that is given priority at a packet
  boundary.  Currently, this icludes events and response packets
  (to ack a request).

DEPENDENCIES
  This must be called at a packet boundary, from the perspective the caller.
  Once called, it must continue to be called until TRUE is returned.

RETURN VALUE
  TRUE if packet boundary processing is complete.
  FALSE if incomplete (due to flow control).  Caller must resend later.

===========================================================================*/
static boolean diagbuf_process_pkt_bnd (rex_sigs_type mask)
{
  /* This enum keeps state to ensure all pkt boundary processing is completed.
     It also ensures that state is maintained when flow control pre-empts
     processing. */
  static enum
  {
    DIAGBUF_PKT_BND_START_STATE,
    DIAGBUF_PKT_BND_RSP_STATE,  /* diagpkt rsp send in progress */
    DIAGBUF_PKT_BND_EVENT_STATE /* Event service send in progress */
  } state = DIAGBUF_PKT_BND_START_STATE;

  boolean complete = FALSE;

  /* Continue sending events and packets until all packet boundary
     processing in complete. */
  while (diagbuf_OKTX && !complete)
  {

    if (diag_panic_mode) dog_kick();  /* PLM */

    /* Service the packet service first.  If responses are delayed, 
       some tools may time out and assume a lost connection. */
    if (state == DIAGBUF_PKT_BND_START_STATE ||
        state == DIAGBUF_PKT_BND_RSP_STATE)
    {
      if (diagpkt_rsp_send () == FALSE)
      {
        state = DIAGBUF_PKT_BND_RSP_STATE;
      }
      else
      {
        /* Mark state as start so the event service will know it was not 
           interrupted. */
        state = DIAGBUF_PKT_BND_START_STATE;
      }
    }
    
    if (state == DIAGBUF_PKT_BND_START_STATE || 
        state == DIAGBUF_PKT_BND_EVENT_STATE)
    {
      /* If in the event state, the packet service TX queue is empty. */
      if (event_drain (mask) == FALSE)
      {
        state = DIAGBUF_PKT_BND_EVENT_STATE;
      }
      else if (state == DIAGBUF_PKT_BND_EVENT_STATE)
      {
        /* Finished events after previously being preempted.  Start over */
        state = DIAGBUF_PKT_BND_START_STATE;
      }
      else
      {
        /* If we came from the start state (no preemption)
           and no more events are pending, this process is complete. */
        complete = TRUE;
      }
    }

  }

  if (!complete)
  {
    if (diagbuf_OKTX)
    {
      /* Since the event service is not finished, and not flow controlled,
         it must be waiting to do more work. */
      (void) rex_set_sigs (&diag_tcb, DIAG_TX_SIG);
    }
  }

  return complete;
}

/*=========================================================================*/
diagbuf_status_enum_type diagbuf_drain (rex_sigs_type mask)
{
  rex_sigs_type  sigs;       /* Used to check if loop should be exited.      */

  static int iTailIdx;       /* Byte array index to tail of diag ring buffer */

  static diagbuf_outbound_pkt_handler_type fp = NULL; /* Customer packet processor */

  diagbuf_header_type pkt_hdr;    /* Store the header in local variable     */

  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE};
  byte *start_pos = NULL;
  static uint8 pad;               /* Stores the pad                         */

  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  /* If this fails, it typically means someone overwrote a diag buffer */
  ASSERT(diagbuf_tail >= 0 && diagbuf_tail < 2 * DIAGBUF_SIZE);

  ASSERT (rex_self () == &diag_tcb);

  /* Sanity check*/
  if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
  }

  if (!DIAG_CHECK_MASK ((int)diag_cx_state, DIAG_CX_ANY_S))
  {
    status = DIAGBUF_NOT_READY_S;
  }
  else
  {
    /* Keep sending packets until empty or flow controlled. */
    while ((diagbuf_OKTX == TRUE && status == DIAGBUF_READY_S))
    {
      /* Get the index of the ring buffer's tail */
      iTailIdx = DIAGBUF_CALC_IDX(diagbuf_tail);

      ASSERT (iTailIdx < DIAGBUF_SIZE);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        /* Process higher priority data at packet boundary. */
        if (diagbuf_process_pkt_bnd (mask) == FALSE)
        {
          status = DIAGBUF_NOT_READY_S;
        }
        else
        {
          if (DIAGBUF_EMPTY (diagbuf_head, diagbuf_tail))
          {
            status = DIAGBUF_EMPTY_S;
          }
          else
          {
            /* Prep the start of a new packet */
            pkt_hdr = *((diagbuf_header_type *) &diagbuf_buf[iTailIdx]);

            if (pkt_hdr.status == DIAGBUF_COMMIT_S)
            {
              /* Go past the header field */
              DIAGBUF_MOVE(diagbuf_tail, (int)sizeof(diagbuf_header_type));
              DIAGBUF_MOVE(iTailIdx, (int)sizeof(diagbuf_header_type));

              if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
              {
                ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
              }

              ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
              ASSERT (iTailIdx < DIAGBUF_SIZE);

              /* Initialize the send descriptor */
              send.pkt = (void *) &diagbuf_buf[iTailIdx];
              send.last = (void *) ((uint32) send.pkt + pkt_hdr.length - 1);
              send.state = DIAG_SEND_STATE_START;
              send.terminate = TRUE;

              pad = pkt_hdr.pad;

              fp = diagbuf_get_custom_pkt_handler (diagbuf_buf[iTailIdx]);

              /* Adjust for size of overrun pattern field */
              pad += sizeof (uint16);
            }
            else if (pkt_hdr.status == DIAGBUF_UNCOMMIT_S || pkt_hdr.status == DIAGBUF_UNUSED_S)
            {
              /* If panic mode, treat an uncommitted packet as a hole. */
              if (diag_panic_mode)
              {
                DIAGBUF_MOVE (diagbuf_tail, (int)(pkt_hdr.length + sizeof (diagbuf_header_type)));
                if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
                {
                  ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
                }
                ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
                continue;
              }
              else
              {
                status = DIAGBUF_NOT_READY_S;
              }
            }
            else if (pkt_hdr.status == DIAGBUF_WRAP_S)
            {
              DIAGBUF_WRAP (diagbuf_tail);
              ASSERT ((diagbuf_tail == 0) || (diagbuf_tail == DIAGBUF_SIZE));
              continue;
            }
            else if (pkt_hdr.status == DIAGBUF_HOLE_S)
            {
              DIAGBUF_MOVE (diagbuf_tail, (int)(pkt_hdr.length + sizeof (diagbuf_header_type)));
              if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
              {
                ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
              }
              ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
              continue;
            }
          }
        }
      } /* if start of new packet */

      if (status == DIAGBUF_READY_S)
      {
        start_pos = (byte *) send.pkt;

        if (fp)
        {
          fp (&send);
        }
        else
        {
          (void) diagbuf_send_pkt (&send);
        }

        if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
        {
          ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
        }
        /* Move the tail. */
        DIAGBUF_MOVE(diagbuf_tail, (int)((uint32) send.pkt - (uint32) start_pos));
        DIAGBUF_MOVE(iTailIdx, (int)((uint32) send.pkt - (uint32) start_pos));

        if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
        {
          ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
        }

        ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
        ASSERT (iTailIdx < DIAGBUF_SIZE);

        if (send.state == DIAG_SEND_STATE_COMPLETE)
        {
          fp = NULL;

          /* Packet has been exhausted.  Adjust for pad. */
          DIAGBUF_MOVE(diagbuf_tail, pad);
          DIAGBUF_MOVE(iTailIdx, pad);
 
          ASSERT (diagbuf_tail < 2 * DIAGBUF_SIZE);
          ASSERT (iTailIdx < DIAGBUF_SIZE);
        }

        /* Sanity check */
        if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
        {
          /* Panic mode will reenter this function.  Must put system in a 
             recoverable state. */
         // diagbuf_head = diagbuf_tail = 0;
          ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);

          /* Panic mode will swap in the DIAG task - must be able to return 
             to normal task operations, so exit the loop. */
/*lint -save -e{527} */
          break;
/*lint -restore */
        }
        if (DIAGBUF_EMPTY(diagbuf_head, diagbuf_tail))
        {
          status = DIAGBUF_EMPTY_S;
        }
      }

      /* Get signals */
      sigs = rex_get_sigs (&diag_tcb);

      /* RPT_TIMER_SIG?  Kick the dog */
      if (sigs & DIAG_RPT_TIMER_SIG)
      {
        diag_kick_watchdog();
      }

      /* mask sig or diag is blocking?  Break out.  */
      if (sigs & mask)
      {
        break; /* Exit loop.  Something more important has happened! */
      }
    } /* End of while() */
  }

  if (status == DIAGBUF_READY_S && diagbuf_OKTX && diagcomm_status())
  {
    /* Interrupted for some reason - signal DIAG so it will come back.
       There is no reason to terminate diagbuf_send_pkt() at this time,
       since we'll be right back! */
    (void) rex_set_sigs(&diag_tcb, DIAG_TX_SIG);
  }
  else
  {
    /* DIAG is not poised to continue TX immediately, so terminate the existing
      outbound comm buffer. */
    if(diagcomm_status())
      (void) diagbuf_send_pkt (NULL);
  }

  /* Sanity check */
  if (DIAGBUF_USED(diagbuf_head, diagbuf_tail) > DIAGBUF_SIZE)
  {
    ERR_FATAL("Ring buffer corrupt!", 0, 0, 0);
  }

  return status;

} /* diagbuf_drain */



/*=========================================================================*/
boolean diagbuf_register_outbound_pkt_handler (
  uint8 cmd_code,
  diagbuf_outbound_pkt_handler_type fp
)
{
  unsigned int i;
  boolean found = FALSE;

  if (fp)
  {
    for (i = 0; !found && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
    {
      if (diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code ||
          diagbuf_outbound_pkt_handler_tbl[i].cmd_code == 0xFF)
      {
        found = TRUE;

        diagbuf_outbound_pkt_handler_tbl[i].cmd_code = cmd_code;
        diagbuf_outbound_pkt_handler_tbl[i].fp = fp;
      }
    }
  }

  return found;
}



/*=========================================================================*/
static diagbuf_outbound_pkt_handler_type
  diagbuf_get_custom_pkt_handler (uint8 cmd_code)
{
  unsigned int i;
  diagbuf_outbound_pkt_handler_type fp = NULL;

  for (i = 0; !fp && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++)
  {
    if (diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code)
    {
      fp = diagbuf_outbound_pkt_handler_tbl[i].fp;
    }
  }

  return fp;
}
#endif /* if !defined(FEATURE_DIAG_NON_STREAMING) */

/*===========================================================================

FUNCTION DIAGBUF_FLUSH

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed.

  Diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

===========================================================================*/
void diagbuf_flush (void)
{
  /*-----------------------------------------------
    Turn off messages, logs, and event reporting.
  -----------------------------------------------*/
  msg_set_all_rt_masks (0);

  /* Listeners must still be serviced, but logs and events for the tool must
     at least stop being sent to the comm layer. */
  log_flush ();
  event_flush ();

#ifndef FEATURE_DIAG_NON_STREAMING

  /* In case there is no connection and the buffer is non-empty. */
  DIAG_SET_MASK (diag_cx_state, DIAG_CX_FLUSH_S);

  /* Go ahead and drain the ring buffer
  */
  while (diagbuf_drain (0) != DIAGBUF_EMPTY_S)
  {
    (void) diag_wait (DIAG_TX_SIG);
    (void) rex_clr_sigs (&diag_tcb, DIAG_TX_SIG);
  }

  /* After flush, the comm connection state is considered disconnected. */
  DIAG_CLR_MASK (diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

#endif /* FEATURE_DIAG_NON_STREAMING */

  /* Flush the TX stream in the comm layer. */
  diagcomm_flush_tx (NULL);

} /* diagbuf_flush */



#ifdef FEATURE_DIAG_NON_STREAMING
/*===========================================================================

FUNCTION DIAGBUF_BLOCKING_SEND_PKT

DESCRIPTION
  This function sends the given packet.  It not return until the send
  operation is complete.  If flow control occers, this routine will loop/wait
  until the packet is sent.

===========================================================================*/
boolean diagbuf_encode_and_send_pkt (void *pkt, unsigned int length)
{
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  diag_send_desc_type send;
  boolean success = FALSE;

  if (pkt)
  {
    /* Set up the descriptor. */
    send.pkt = pkt;
    send.last = (void *) ((byte *) pkt + length - 1);
    send.state = DIAG_SEND_STATE_START;
    send.terminate = TRUE;

    if (pkt)
    {
      while (send.state < DIAG_SEND_STATE_COMPLETE)
      {
        if (status != DIAGBUF_READY_S)
        {
          diag_wait (DIAG_TX_SIG);
        }

        status = diagbuf_send_pkt (&send);
      }

      /* Terminate the packet and force it to be sent. */
      diagbuf_send_pkt (NULL);
    }

    success = TRUE;
  }

  return success;
}

/*=========================================================================*/
boolean diagbuf_send_single_buffered_pkt (void)
{
  int iTailIdx;
  unsigned int pkt_length = 0;
  diagbuf_header_type *pkt_hdr;  /* local varaible to store header */
  boolean pkt_sent = FALSE;

  ASSERT (diagbuf_tail >= 0 && diagbuf_tail < 2 * DIAGBUF_SIZE);

  /*-------------------------------------------------------------
    To emulate the old V7 poll/response protocol, remove one log
    packet from the buffer and put in the response.
  -------------------------------------------------------------*/
  do
  {
    iTailIdx = DIAGBUF_CALC_IDX (diagbuf_tail);

    pkt_hdr = ((diagbuf_header_type *) &diagbuf_buf[iTailIdx]);

    if (!DIAGBUF_EMPTY (diagbuf_head, diagbuf_tail) &&
        pkt_hdr->status == DIAGBUF_WRAP_S)
    {
      DIAGBUF_WRAP(diagbuf_tail);

      continue; /* Start over */
    }

    if (!DIAGBUF_EMPTY (diagbuf_head, diagbuf_tail) &&
        pkt_hdr->status == DIAGBUF_HOLE_S)
    {
      DIAGBUF_MOVE (diagbuf_tail, pkt_hdr->length +
                    sizeof (diagbuf_header_type));

      continue; /* Start over */
    }

    if (!DIAGBUF_EMPTY (diagbuf_head, diagbuf_tail) &&
        pkt_hdr->status == DIAGBUF_COMMIT_S)
    {
      pkt_length = pkt_hdr->length;

      /* Adjust indicies */
      DIAGBUF_MOVE (diagbuf_tail, sizeof (diagbuf_header_type));

      iTailIdx  = DIAGBUF_CALC_IDX (diagbuf_tail);

      /* Send packet */
      pkt_sent = diagbuf_encode_and_send_pkt (&diagbuf_buf[iTailIdx],
                                              pkt_length);

      /* iTailIdx is re-calculated at top of loop. */
      DIAGBUF_MOVE (diagbuf_tail, pkt_length + pkt_hdr->pad +
                    sizeof (uint16));

    }
  } while (0);

  /* Since we do our own sending here, do not pass back a pointer. */
  return pkt_sent;

} /* diagbuf_send_single_buffered_pkt */

#endif /* FEATURE_DIAG_NON_STREAMING */


#ifdef DIAG_DEBUG_FS_PKT_LOG
#error code not present
#endif /* DIAG_DEBUG_FS_PKT_LOG */


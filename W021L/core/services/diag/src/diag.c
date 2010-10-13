/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  The diagnostic task supports the interface between the mobile software
  tasks and the diagnostic monitor. The diagnostic task responds to 
  packets sent by the Diagnostic Monitor in order to support software
  debugging, data logging and displaying the temporal analyzer.

Copyright (c) 1992-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diag.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/08   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
01/18/08   pc      Added DIAG_EVENT_PEND_TIMER_SIG to the busy_mask in 
                   diag_wait()
01/08/07   tbg     Added support for new err_auto_action API
05/03/06   as      Feature FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL disables
                   going into downloader on error fatal.
10/31/05   as      Fixed lint errors.
07/25/05   as      Added TRAMP_SLEEP_TIMER_TICK_ISR in diag panic handler.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/27/05   as      FEATURE_L4 turns off diag panic mode.functionality.
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
03/14/05   as      Moved diagpkt_init_manual_dispatch_table() call to diag_task
02/28/05   as      Replaced rex critical section with intlocks.
02/15/05   as      Added FEATURE_DIAG_DEBUG_6275. This temporary feature will
                   be used until Raven code is stabilized.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
01/27/05   as      Restored check for blocked diag heap in diag_malloc().
12/14/04   sl      Additionaly diag_timeout() resets the SIO stream ID if
                   FEATURE_DIAG_MULTI_PORTS is defined.
04/06/04   tbg     Added support for FEATURE_ERR_EXTEDED_STORE
09/23/03   gr      Added code to not iterate through the diag heap when out
                   of memory. Should improve efficiency in certain situations.
09/08/03   as      Changed diag_tx_notify to call rex timer only when 
                   interrupts are locked.
09/03/03   as      Removed diag_tx_notify_timer
12/12/02   lad     Added panic mode powerdown option (NV).
12/02/02   lad     Removed inactivity timer.
11/21/02   lad     Mainlined diag heap.
09/13/02   lad     Added config save to filestream mode.
07/30/02   as      Added inactivity timer to detect external device 
                   disconnect.
07/28/02   lad     Moved services initializations to diag_init().
06/21/02   lad     Added the filestream feature to capture data generated
                   prior to tool establishing connection.
                   Restructured processing of inbound data for reuse w/
                   filestream feature.
08/20/01   jal     Changes to support more Diag packets.  Handle online/
                   offline mode.  Jump to downloader added.  Added accessor
                   functions for service programming lock state.
06/27/01   lad     Changed diag_hdlc_decode to take signed length to
                   detect invalid input.
                   Fixed error in handling the RX of a packet that is too big
04/06/01   lad     TASK_ signals were replaced with DIAG_TASK_xxx signals to 
                   remove dependency on task.h.
                   Chagned max_req_pkt_len in diag_task to be 
                   DIAG_MAX_RX_PKT_SIZ which is defined in diagtune.h.
                   Changed instance of diag_start_task() to 
                   diag_ready_start_task().
                   Changed diag_do_escaping and diag_hdlc_decode to not pass 
                   ASYNC control chars as params (use constants instead).  This
                   makes the function call cheaper and optimizes the routine
                   slightly.
                   Featurized diagpkt_refresh_cache() call under 
                   FEATURE_DIAG_CACHE.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diag.h"
#include "diagbuf.h"
#include "diagcomm.h"
#include "diagi.h"
#include "diagnv.h"
#include "diagpkt.h"
#include "diagdiag.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"

#include "assert.h"
#include "crc.h"
#include "dog.h"
#include "memheap.h"
#include "nv.h"
#include "queue.h"
#include "rex.h"
#include "task.h"

#include <string.h>
#if defined (FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL)
  #include "boothw.h"
#endif

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/* Timer to wake Diagnostic task so that it can set
** its Watchdog Task Report Signal.
*/
rex_timer_type diag_rpt_timer;

/* Timer to use when a bad SPC has been received.
*/
static rex_timer_type diag_tout_timer;

/* Request packets are sent from the DM to the mobile.  Request packet is 
** reconstructed from pieces received from SIO.
*/
static uint8 diag_req_inbound[DIAG_MAX_RX_PKT_SIZ];

/* This timer is used to notify DIAG that a client has submitted data to TX.
   Using a timer instead of rex_set_sigs() prevents a context switch in case
   the client calls from within INTLOCK(). */
static rex_timer_type diag_tx_notify_timer;
static boolean diag_tx_pending = TRUE;  /* Init to TRUE until init in diag_init(). */

/* Diag state variables.
*/
static diag_security_type diag_security = DIAG_SEC_LOCKED;
static diag_sp_type diag_sp = DIAG_SPC_LOCKED;

#ifdef FEATURE_DIAG_SPC_TIMEOUT
uint16  diag_spc_tout;
#endif

/* Connectivity state.  If non-zero, something is using DIAG. */
uint32 diag_cx_state = DIAG_CX_NONE_S;

/* Definition of a constant that is the same as an unwritten security code.
*/
static const uint8 diag_no_security_code_c[] = { 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff
};

#ifndef FEATURE_DIAG_NON_STREAMING
/*----------------------------------------------------------------------------
  Panic mode information.  This mode occurs during a fatal error - it attempts
  to flush all buffered data and capture system data prior to powerdown.
----------------------------------------------------------------------------*/
boolean diag_panic_mode = FALSE;

#ifndef FEATURE_L4
static rex_sigs_type diag_panic_mode_handler (rex_sigs_type mask);
#endif
/* static boolean diag_panic_progress (void); */
static void diag_panic_end (void);

//TODO: Make something in diagtarget.h for this...

/* REX implementation things that clients normally do not access directly.
   Panic mode is a special case since the system is being halted. */
#include "armasm.h"
#include "dload.h"
#include "hw.h"
#include "tramp.h"

extern void rex_sched (void);
extern void *rex_set_cpsr (dword);

/* External declarations for panic mode */
extern void tramp_isr (void);   /* This is normally not externalized */
#endif

/*-------------------------------------------------------------------
** Dynamic Memory Support (heap)
**    Definitions and data to support rex_malloc/realloc/calloc/free
**-----------------------------------------------------------------*/
#ifndef DIAG_HEAP_SIZE
  /* The size of the REX memory buffer in bytes. */
  #define DIAG_HEAP_SIZE 2048
#endif
/* The DIAG heap in REX. */
static mem_heap_type diag_heap;

/* An upper bound on the sizes of the biggest available blocks in the
** diag heap.
*/
#define DIAG_FREEBLOCK_CACHE_SIZE 2
static unsigned int diag_freeblock_info[DIAG_FREEBLOCK_CACHE_SIZE];

/* Is the freeblock cache for the diag heap valid or not?
*/
static int diag_heap_cache_valid = 0;

/* Minimum chunk size for the diag heap.
*/
static unsigned int diag_heap_chunk_size;


/* The DIAG memory buffer used by the heap manager. */
static char diag_heap_buf[DIAG_HEAP_SIZE];

/* Idle processing table.  If fp != NULL, the function will be called when 
   the DIAG task is idle (nothing else to do). */
typedef struct
{
  diag_idle_processing_ftype fp;
  void *param;
}
diag_idle_proc_tbl_type;

diag_idle_proc_tbl_type diag_idle_proc_tbl[DIAG_IDLE_PROC_CNT];

static void diag_idle_processing (void);

extern void diagpkt_init_manual_dispatch_table (void);

#if defined (FEATURE_EFS) && !defined (FEATURE_DIAG_NON_STREAMING)
/* ----------------------------------------------------------------------------
  "Diagnostics Filestream Mode"
  
  The diagnostic task filestream mode reads a file to serve as an input 
  stream in lieu of an active comm layer.  The output of the diagnostic 
  service is buffered in diag's output buffer manager (diagbuf.c).  
  When a connection is activated via the comm layer and a valid inbound packet
  is detected, the buffered output is flushed to the comm layer,
  thus capturing all diagnostic data generated after initialization.
  
  This mode continues until one of the following conditions occur:
  
  (1) Communication is initiated via the comm layer (external device), 
      triggering the filestream mode to flush its output buffer to the
      comm layer.
  (2) The inactivity timer expires, stopping this mode and turning off all 
      diag services.

  NOTE: The diag task (diag.c) is responsible for detecting these conditions
  and calling this API appropriately.

  The input file is written by the external device during the prior instance
  of the target running.  The diag task calls diag_fs_init() at startup, 
  detecting the presence of the input file.  Once the entire input file 
  stream is processed, it is deleted.
  
Noteworthy features:
  
  This mode is enabled only if the input file is present.
  
  Once the input file is processed, it is deleted.  This is done to avoid 
  the performance loss of running the diagnostic task every boot cycle.  
  If this were ever enabled in a production phone, for example, it would 
  run the diag service every time the user turns a phone on, impacting battery
  life.  It also prevents legacy tools from failing due to lack of support
  for streaming and thie feature.
---------------------------------------------------------------------------- */
#include "fs.h"
/* Output stream buffer file open parameters */
static const fs_open_create_type diag_fs_write_open_parms = {
  FS_OC_DISCARD,
  FS_OB_ALLOW,
  FS_FA_UNRESTRICTED,
  0
};

/* Diag filestream type: contains file information as well as position for
   reading and max size for writing.
   ASSUMPTION: Writing is always done at the end of the file. */

//TODO: No longer needs to be global
typedef struct
{
  fs_file_position_type read_pos;   /* position for reading */
  fs_file_position_type write_pos;  /* position for writing */
  unsigned int max_size;        /* max file size */
  fs_handle_type handle;
  char *fname;
}
diag_fs_type;

static diag_fs_type diag_fs_config = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  FS_NULL_HANDLE,
  ".DIAGCFG"
};

static diag_fs_type diag_fs_in = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  FS_NULL_HANDLE,
  ".DIAGIN"
};

#if !defined (DIAG_FS_INBUF_MAX_SIZE)
#define DIAG_FS_INBUF_MAX_SIZE (16 * 1024)
#endif

static diag_fs_type diag_fs_inbuf = {
  0,
  0,
  DIAG_FS_INBUF_MAX_SIZE,
  FS_NULL_HANDLE,
  ".DIAGINTMP"
};

/* Prototypes for filestream mode */
static void diag_fs_init (diag_fs_type * fs);
static boolean diag_fs_inbound_mode (void *buf, unsigned int size);
static uint32 diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size);
static boolean diag_fs_write (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_inbound (diag_fs_type * fs, void *buf,
                     unsigned int size);
static void diag_fs_terminate (diag_fs_type * fs);
static void diag_fs_inbound_comm (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_config (void *req_pkt, word pkt_len);
static void diag_fs_write_config (void);
#endif /* FEATURE_EFS && !FEATURE_DIAG_NON_STREAMING */


typedef struct
{
  /* (Input) raw (HDLC encoded) stream buffer.  This is never changed
     by the packet extraction/decoding functions. */
  byte *src_ptr;

  /* (I/O) The current byte array index of src_ptr.  
     When src_idx == src_size, the src_ptr buffer is complete. */
  unsigned int src_idx;

  /* (Input) This is the number of bytes to convert from src_ptr. */
  unsigned int src_size;

  /* NOTE: Since a packet can cpan mulitple source blocks, dest_idx
     must be stored to maintain the state of the packet extraction.
     For src_ptr, this is not necessary. */

  /* (Input) The extracted packet.  This is never changed
     by the packet extraction/decoding functions. */
  byte *dest_ptr;

  /* (I/O) The array index into dest_ptr of the next unwritten byte.
     Upon successful decode of a packet, this field is the length. */
  unsigned int dest_idx;

  /* This is the number of bytes that can be stored in dest_ptr. */
  unsigned int dest_size;

  /* Used internally: Are we in the middle of an escape code?  
     This boolean is needed in order to be able to handle the 
     case where an escape code straddles two packets. */
  boolean escaping;

}
diag_hdlc_decode_type;

/* Q and Q types for diag_cmd_req () */
typedef struct
{
  q_link_type link;
  
  diag_cmd_rsp rsp_func;
  void *param;

  unsigned int length;
  byte req[DIAG_MAX_TX_PKT_SIZ];             /* sized by the 'length' field */
}
diag_req_cmd_type;

static q_type diag_req_cmd_q;

/* -------------------------------------------------------------------------
** Forward Declarations
** ------------------------------------------------------------------------- */
boolean diag_check_password (void *code_ptr);
static void diag_rx_notify (void);
static boolean diag_get_rx_pkt (uint8 * dest_ptr, unsigned int size,
                unsigned int *pkt_len);
static rex_sigs_type diag_handle_sigs (rex_sigs_type mask);
static boolean diag_extract_pkt (diag_hdlc_decode_type * hdlc);
static boolean diag_hdlc_decode (diag_hdlc_decode_type * hdlc);
static void diag_heap_init (void);

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif

#ifdef FEATURE_ERR_EXTENDED_STORE
extern void boot_generic_error_handler( void );
#endif

#ifndef FEATURE_DIAG_DEBUG_6275
#if defined (FEATURE_WCDMA) && defined (FEATURE_GSM) && defined (FEATURE_ERR_EXTENDED_STORE)
#error code not present
#endif
#endif /* FEATURE_DIAG_DEBUG_6275 */

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diag_blocking_malloc_sucess_cnt =0;
uint32 diag_malloc_sucess_cnt =0;
uint32 diag_free_cnt =0;
uint32 diag_process_rx_free_cnt = 0;
uint32 diag_searchlist_add_malloc_cnt = 0;
uint32 diag_searchlist_add_free_cnt = 0;
uint32 diag_searchlist_delete_malloc_cnt = 0;
uint32 diag_searchlist_delete_free_cnt = 0;
uint32 diag_cmd_req_cnt = 0;
uint32 diag_fs_process_inbound_free_cnt = 0;
uint32 diag_fs_process_inbound_malloc_cnt = 0;

uint32 diag_heap_used_bytes(void)
{
  return diag_heap.used_bytes;
}
#endif /* FEATURE_DEBUG_DIAG_HEAP */

/* -------------------------------------------------------------------------
** Function Implementations
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAG_RX_NOTIFY

DESCRIPTION
  This callback routine is called when data has been received and is ready
  be read by the diag task.
  
===========================================================================*/
static void
diag_rx_notify (void)
{
  (void) rex_set_sigs (&diag_tcb, DIAG_RX_SIG);
}

/* ==================================================================
FUNCTION DIAG_HEAP_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
static void
diag_heap_init (void)
{
  mem_init_heap (&diag_heap, diag_heap_buf, sizeof (diag_heap_buf), NULL);
  (void) mem_heap_set_int_lock (&diag_heap);
  diag_heap_chunk_size = mem_heap_get_chunk_size (&diag_heap);
  return;
}               /* END diag_heap_init */

/*===================================================================*/
static boolean diag_heap_block = FALSE;

void * 
diag_blocking_malloc (unsigned int num_bytes)
{
  void *buf = NULL;

  if (!diag_heap_cache_valid || diag_alloc_check (1, (unsigned int *)&num_bytes))
  {
    buf = mem_malloc (&diag_heap, num_bytes);
  }

  /* If unsuccessful, block other clients from allocating.  
     Remove block if successful. */
  diag_heap_block = (buf) ? FALSE : TRUE;

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */
  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_freeblock_info);
      diag_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_blocking_malloc_sucess_cnt++;
    MSG_FATAL(" <<DIAG BLOCKING MALLOC sucess_cnt = %d, diag heap used = %d ",
        diag_blocking_malloc_sucess_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
}

/*===================================================================*/
void * 
diag_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_heap_block)
  {
    if (!diag_heap_cache_valid || diag_alloc_check (1, (unsigned int *)&num_bytes))
    {
       buf = mem_malloc (&diag_heap, num_bytes);
    }
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */
  
  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_freeblock_info);
      diag_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_malloc_sucess_cnt++;
    MSG_FATAL(" <<DIAG MALLOC diag_malloc_sucess_cnt = %d,  heap used = %d ",
        diag_malloc_sucess_cnt, diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_malloc */


int
diag_alloc_check (unsigned int num_allocs, unsigned int *alloc_sizes )
{
  unsigned int i;
  unsigned int j;
  unsigned int cur_req_size;
  unsigned int best_match_size;
  unsigned int best_match_index;
  unsigned int freeblocks[DIAG_FREEBLOCK_CACHE_SIZE];

  ASSERT (num_allocs <= DIAG_FREEBLOCK_CACHE_SIZE);

  /* Return 1 if the freeblock cache is dirty.
  */
  if (!diag_heap_cache_valid)
    return 1;

  memcpy (freeblocks, diag_freeblock_info,
    DIAG_FREEBLOCK_CACHE_SIZE * sizeof (int));

  /* Match each requested size up with the closest available size that
  ** is greater than the requested size. Update the freeblock information
  ** after each match. If even one requested size cannot be matched up,
  ** then the heap does not contain enough space for all the requests.
  */
  for (i = 0; i < num_allocs; i++)
  {
    cur_req_size = ((alloc_sizes[i] + sizeof (mem_block_header_type)
      + diag_heap_chunk_size - 1) / diag_heap_chunk_size)
      * diag_heap_chunk_size;
    best_match_size  = 0x7fffffff;
    best_match_index = DIAG_FREEBLOCK_CACHE_SIZE;
    for (j = 0; j < DIAG_FREEBLOCK_CACHE_SIZE; j++)
    {
      if ((freeblocks[j] >= cur_req_size) &&
          (freeblocks[j] <  best_match_size))
      {
        best_match_size  = freeblocks[j];
        best_match_index = j;
      }
    }
    if (best_match_index < DIAG_FREEBLOCK_CACHE_SIZE)
      freeblocks[best_match_index] -= cur_req_size;
    else
      return 0;
  }

  return 1;
} /* END diag_alloc_check */


/*===================================================================*/
void 
diag_free(void *mem_ptr)
{
  mem_free (&diag_heap, mem_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_free_cnt++;
  MSG_FATAL(">> DIAG FREE diag_free_cnt = %d, diag heap used = %d ",
      diag_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  diag_heap_cache_valid = 0;
  return;
} /* END diag_free */

/*===================================================================*/
void 
diag_block (void)
{
  (void) rex_wait (DIAG_BLOCK_SIG);
  (void) rex_clr_sigs (&diag_tcb, DIAG_BLOCK_SIG);
  return;
} /* END diag_free */

/*===================================================================*/
void 
diag_release (void)
{
  (void) rex_set_sigs (&diag_tcb, DIAG_BLOCK_SIG);
  return;
} /* END diag_free */

/*===========================================================================

FUNCTION DIAG_INIT

DESCRIPTION
  This function initializes the SIO descriptors for the request and
  response buffers.  Initializes the msg services.  This is called prior to
  the task being started, so no assumptions about other tasks can be made.

===========================================================================*/
void
diag_init (void)
{
  int i;

  /*  Initialize watchdog report timer  */
  rex_def_timer (&diag_rpt_timer, &diag_tcb, DIAG_RPT_TIMER_SIG);

  rex_def_timer (&diag_tout_timer, &diag_tcb, DIAG_TOUT_TIMER_SIG);

  /* Timer used to notify DIAG that TX is pending from a client. */
  rex_def_timer (&diag_tx_notify_timer, &diag_tcb, DIAG_TX_SIG);

  /* reset the pending, enabling the mechanism. */
  diag_tx_pending = FALSE;

  /* In case something is already pending, go ahead and notify DIAG to TX. */
  diag_tx_notify ();

  /* If the code in the phone is not programmed, the phone defaults to 
     unlocked security. */
  if (diag_check_password (NULL))
  {
    diag_security = DIAG_SEC_UNLOCKED;
  }
  else
  {
    diag_security = DIAG_SEC_LOCKED;
  }

  /* The comm layer needs to be initialized before anything else.  This is
   * needed because diagbuf_init() sets callbacks that are initialized in
   * diagcomm_init(). */
  diagcomm_init ();

  diag_heap_init ();

  /* Register a callback to notify us when inbound data is available. */
  diagcomm_register_inbound (diag_rx_notify);

  diagbuf_init ();

  /* Init event service */
  event_init ();

  /* Init log service */
  log_init ();

  /* Init debug message service */
  msg_init ();

  for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
  {
    diag_idle_proc_tbl[i].fp = NULL;
    diag_idle_proc_tbl[i].param = NULL;
  }

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif

#ifdef FEATURE_DIAG_AUTOSTART_TASK
  rex_set_sigs (&diag_tcb, DIAG_TASK_START_SIG);
#endif

  /* Initialized the client command queue. */
  (void) q_init (&diag_req_cmd_q);

  MSG_LOW ("DIAG task initialized", 0, 0, 0);
}               /* diag_init */

/*===========================================================================

FUNCTION DIAG_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits, 
  this procedure continues to set the Watchdog report signal and reset 
  the diag_rpt_timer. 
  
  To avoid deadlock situations, this function also waits on the MC task
  sigs.  Since diag doesn't really care when power goes away, the func
  immediately ack's MC.

  Depends on previously having used the diag_rpt_timer to time when to
  set the watchdog signal.
  
===========================================================================*/
rex_sigs_type
diag_wait (rex_sigs_type mask)  /* Signals to wait for */
{
  boolean found = FALSE;
  rex_sigs_type sigs;       /* signals returned by rex_get_sigs call */
  rex_sigs_type busy_mask = mask |
#if !defined (FEATURE_DIAG_NON_STREAMING)
    DIAG_TX_SIG |
#endif
    DIAG_EVENT_TIMER_SIG | DIAG_TASK_STOP_SIG | DIAG_TASK_OFFLINE_SIG | DIAG_EVENT_PEND_TIMER_SIG;

  rex_sigs_type composite_mask = busy_mask | DIAG_RPT_TIMER_SIG;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#ifndef FEATURE_DIAG_NON_STREAMING
  #ifndef FEATURE_L4
  /* L4 does not support diag panic mode */
  if (diag_panic_mode)
  {
    sigs = diag_panic_mode_handler (mask);

    diag_kick_watchdog ();

    return sigs;
  }
  #endif /* FEATURE_L4 */
#endif
  while (!found)
  {
    sigs = rex_wait (composite_mask);

    /* Pet the dog before doing any work. */
    diag_kick_watchdog ();

    if (sigs & DIAG_TASK_STOP_SIG)
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_TASK_STOP_SIG);
      diag_shutdown ();
    }
    else if (sigs & DIAG_TASK_OFFLINE_SIG)
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_TASK_OFFLINE_SIG);
      diag_offline ();
    }
    else if (sigs & DIAG_EVENT_PEND_TIMER_SIG)
    {
     (void) rex_clr_sigs (&diag_tcb, DIAG_EVENT_PEND_TIMER_SIG);
      event_service_pending ();
    }
    else if (sigs & mask)
    {
      found = TRUE;
    }
    else if (sigs & DIAG_EVENT_TIMER_SIG)
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_EVENT_TIMER_SIG);
      event_stale ();
    }
#if !defined (FEATURE_DIAG_NON_STREAMING)
    else if (sigs & DIAG_TX_SIG)
    {
      diagbuf_status_enum_type status;

      (void) rex_clr_sigs (&diag_tcb, DIAG_TX_SIG);

      /* Clear the TX notification semaphore. */
      diag_tx_pending = FALSE;

      /* Turn off sleep while buffering. */
      diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

      status = diagbuf_drain (DIAG_TASK_STOP_SIG | mask);

      /* This function will kick the watchdog for us. */
      if (status == DIAGBUF_EMPTY_S)
      {
    diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
      }
    }
#endif /* !defined(FEATURE_DIAG_NON_STREAMING) */

    /* See if there is anything to do.  If empty, this is a quick operation. */
    diag_idle_processing ();
     
#ifndef FEATURE_DIAG_NON_STREAMING
  #ifndef FEATURE_L4
    if (diag_panic_mode)
    {
      sigs = diag_panic_mode_handler (mask);

      diag_kick_watchdog ();

      return sigs;
    }
  #endif
#endif

  }

  return (sigs);

}               /* diag_wait */

/*=========================================================================*/
void
diag_tx_notify (void)
{
#if !defined (FEATURE_DIAG_NON_STREAMING)
  if (!diag_tx_pending)
  {
    diag_tx_pending = TRUE;

  /* The timer is used because some MSG macros in DMSS?AMSS are called from
     INTLOCK. If you signal DIAG directly, it will cause a context switch 
     while in INTLOCK.
     By setting a timer, the signal won't occur until the next clock tick
     ISR. Therefore, no signal is set during the possible INTLOCK. */

    if( INTS_ARE_LOCKED() )
    {
      (void) rex_set_timer (&diag_tx_notify_timer, 1);
    }
    else
    {
      (void) rex_set_sigs (&diag_tcb, DIAG_TX_SIG);
    }

  }
#endif

  return;
}

/*===========================================================================

FUNCTION DIAG_PROCESS_RX

DESCRIPTION
  This routine processes the RX queue as well as the cmd Q from the system 
  monitor API, i.e. diag_cmd_req().
  
===========================================================================*/
static void
diag_process_rx (void)
{
  uint16 pkt_len = 0;
  diag_req_cmd_type *cmd = NULL;

  /* Disable diag sleep so packet can be processed w/out interruption */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

  /* Normal operation: process inbound data from comm layer.  Process
     packet if found. */
/*lint -save -e{545,740} Suspicious use of & Unusual pointer cast*/
  if (diag_get_rx_pkt ((uint8 *) & diag_req_inbound,
                sizeof (diag_req_inbound), (unsigned int *)&pkt_len) == TRUE)
  {
    /* If we RX good packets, we must be connected. */
    DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

    diagpkt_process_request (&diag_req_inbound, pkt_len, NULL, NULL);

#if defined (FEATURE_EFS) && !defined (FEATURE_DIAG_NON_STREAMING)
    diag_fs_process_config (&diag_req_inbound, pkt_len);
#endif
  }
/*lint -restore */

  while ((cmd = q_get (&diag_req_cmd_q)) != NULL)
  {
    diagpkt_process_request (cmd->req, (uint16) cmd->length, 
                             cmd->rsp_func, cmd->param);

    diag_free (cmd);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_process_rx_free_cnt++;
    MSG_FATAL("FREE Q_GET DiagProcessRxFree = %d, heapUsed = %d, Req_q = %d",
     diag_process_rx_free_cnt, diag_heap_used_bytes(),q_cnt (&diag_req_cmd_q));
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    cmd = NULL;
  }

#if defined (FEATURE_DIAG_NON_STREAMING)
  /* If not in legacy (non-streaming) mode, sleep will be voted in 
     diag_wait() when diagbuf_drain() completes. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
#endif

}

/*===========================================================================

FUNCTION DIAG_HANDLE_SIGS

DESCRIPTION
  Handle all of the signals that interest diag.  Watch-dog is kicked as 
  needed.
  
RETURN VALUE
  With *sigs, signals are returned.
  
===========================================================================*/
static rex_sigs_type
diag_handle_sigs (rex_sigs_type mask)
{
  boolean found = FALSE;
  rex_sigs_type sigs = 0;   /* Signals currently set for this task. */
  rex_sigs_type composite_mask = mask | DIAG_RX_SIG |
#if (!defined(FEATURE_RUNTIME_DEVMAP) && \
(defined(FEATURE_SECOND_UART) || defined(FEATURE_SERIAL_DEVICE_MAPPER)))
    DIAG_SIO_SWITCH_UART_SIG |
#endif
#ifdef FEATURE_DIAG_CACHE
    DIAG_REFRESH_CACHE_SIG |
#endif

    DIAG_COMM_OPEN_SIG        |
    DIAG_COMM_CLOSE_SIG;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while (!found)
  {
    sigs = diag_wait (composite_mask);

    if (sigs & DIAG_COMM_CLOSE_SIG)
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_COMM_CLOSE_SIG);
      diagcomm_close (NULL);
    }
    else if (sigs & DIAG_COMM_OPEN_SIG)
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_COMM_OPEN_SIG);
      (void) diagcomm_open ();
    }
#ifdef FEATURE_DIAG_CACHE
    else if (sigs & DIAG_REFRESH_CACHE_SIG)
    {
      /* The packet processing cache may have been changed. */
      (void) rex_clr_sigs (&diag_tcb, DIAG_REFRESH_CACHE_SIG);

      diagnv_refresh_cache ();
    }
#endif
    /* Only perform idle processing if there is nothing else to do. */
    else if (sigs & mask)
    {
      found = TRUE;
    }
    else if (((sigs & DIAG_RX_SIG) != 0) && (diagcomm_status()))
    {
      (void) rex_clr_sigs (&diag_tcb, DIAG_RX_SIG);

      diag_process_rx ();
    }
  }

  return sigs;
}               /* diag_handle_sigs */

/*===========================================================================

FUNCTION DIAG_GET_RX_PKT

DESCRIPTION
  This function will retrieve the next DM packet.  The incoming packets come 
  in pieces.  This function will do the necessary unescaping and combine the
  pieces to form a whole packet.

DEPENDENCIES
  This procedure can only be called when a full DM packet is available on
  queue from SIO.  Furhter, if multiple packets are enqueued, then this will
  only process the first packet.  The remaining packets are left on the queue.
  
RETURN VALUE
  Returns TRUE if full packet recd, else returns FALSE.
  
===========================================================================*/
static boolean
diag_get_rx_pkt (uint8 * dest_ptr,  /* Buffer for received packet. */
         unsigned int size, /* Maximum size for request packet. */
         unsigned int *pkt_len  /* Packet length when it is constructed. */
  )
{
  static diag_hdlc_decode_type hdlc;
  static boolean initialized = FALSE;

  /* TRUE if found a packet boundary in HDLC stream. */
  boolean pkt_bnd = FALSE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!initialized)
  {
    initialized = TRUE;
    memset (&hdlc, 0, sizeof (hdlc));
  }

  hdlc.dest_ptr = dest_ptr;
  hdlc.dest_size = size;
  *pkt_len = 0;

  do
  {

    if (hdlc.src_ptr == NULL)
    {
      hdlc.src_ptr = (byte *) diagcomm_inbound (&hdlc.src_size);
      hdlc.src_idx = 0;

      if (hdlc.src_ptr == NULL)
      {
    goto exit;
      }
    }

    pkt_bnd = diag_extract_pkt (&hdlc);

    if (pkt_bnd)
    {
      *pkt_len = hdlc.dest_idx;
      hdlc.dest_idx = 0;    /* Initialize for the next packet */
    }

    /* Free the pointer if the buffer has been completely read.
     */
    if (hdlc.src_idx == hdlc.src_size)
    {
      (void) diagcomm_inbound_complete (hdlc.src_ptr);
      hdlc.src_ptr = NULL;
    }
  }
  while (!pkt_bnd && !hdlc.src_ptr);

exit:
  if (hdlc.src_ptr == NULL)
  {
    hdlc.src_ptr = (byte *) diagcomm_inbound (&hdlc.src_size);
    hdlc.src_idx = 0;
  }

  if (hdlc.src_ptr != NULL)
  {
    /* There is more data available, signal diag to continue RXing */
    diag_rx_notify ();
  }

  return pkt_bnd;
}               /* diag_get_rx_pkt */

/*===========================================================================

FUNCTION DIAG_EXTRACT_PKT

DESCRIPTION
  This function extracts a packet from a HDLC encoded stream, and verifies 
  CRC is correct.

DEPENDENCIES
  None.
  
RETURN VALUE
  Returns TRUE if a valid packet was found.
  
===========================================================================*/
static boolean
diag_extract_pkt (diag_hdlc_decode_type * hdlc)
{
  boolean pkt_bnd = FALSE;
  /* Cyclic Redundancy Check of incoming packet. */
#ifndef PLATFORM_LTK
  word crc;
#endif

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr)
  {

    pkt_bnd = diag_hdlc_decode (hdlc);

    if (pkt_bnd == TRUE)
    {
#ifndef PLATFORM_LTK
      /* At a packet boundary, check the CRC, reset dst_idx for the next
       * packet, and return the packet (if valid). */
      crc = crc_16_l_calc (hdlc->dest_ptr, (word) ((hdlc->dest_idx - 1) * 8));

      if (crc == CRC_16_L_OK)
#else
#error code not present
#endif
      {
    hdlc->dest_idx -= DIAG_ASYNC_TAIL_SIZE;
      }
      else
      {
    MSG_LOW ("Bad RX CRC", 0, 0, 0);
    pkt_bnd = FALSE;
    hdlc->dest_idx = 0; /* Start new packet */
      }
    }
    else if (hdlc->dest_idx >= hdlc->dest_size)
    {
      /* The inbound packet is too large.  Therefore, we cannot recieve it.
       * By setting the dest_idx field to 0, the rest of the packet will be read
       * in, and the CRC will fail. */
      hdlc->dest_idx = 0;
    }
  }

  return pkt_bnd;
}

/*=========================================================================*/
void
diag_timeout (void)
{
  /* First, flush the TX buffer chain. */
  diagbuf_flush ();

#ifndef FEATURE_DIAG_SPC_TIMEOUT
  diag_suspend (DIAG_TOUT_TIME);
#else
  diag_suspend ((diag_spc_tout * 1000));
#endif

  diagcomm_flush_rx (NULL);

#ifdef FEATURE_DIAG_MULTI_PORTS
  diagcomm_reset_sio();
#endif

}               /* diag_timeout */

/*=========================================================================*/
void
diag_suspend (unsigned int duration)
{
  if (duration > 0)
  {
#ifndef FEATURE_DIAG_SPC_TIMEOUT
    (void)rex_timed_wait (DIAG_TOUT_TIMER_SIG, &diag_tout_timer, duration);
#else
    rex_autodog_enable(rex_self()->dog_report_val);
    (void) rex_timed_wait (DIAG_TOUT_TIMER_SIG, &diag_tout_timer, duration);
    rex_autodog_disable();
#endif
  }
}

/*=========================================================================*/
void
diag_pause (unsigned int duration, boolean enable_rx_processing)
{
  if (duration > 0)
  {
    (void) rex_set_timer (&diag_tout_timer, duration);

    if (enable_rx_processing)
    {
      /* Process RX packets as well. */
      (void) diag_handle_sigs (DIAG_TOUT_TIMER_SIG);
    }
    else
    {
      /* Just process TX */
      (void) diag_wait (DIAG_TOUT_TIMER_SIG);
    }
  }
}

/*===========================================================================

FUNCTION DIAG_TASK

DESCRIPTION
  This function contains the diagnostic task.  It processes diag commands
  for each request packet received from the external device (tool).

  This procedure does not return.

===========================================================================*/
void
diag_task (uint32 params)
{
  nv_err_fatal_options_type option;
  nv_stat_enum_type nvstatus = NV_FAIL_S;

#ifdef FEATURE_DIAG_SPC_TTL
  uint8 diag_spc_ttl;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

option.option = ERR_NO_ACTION;

/* Keep Lint happy */

#ifdef _lint
  params = params;
#endif

  /* Initialize task.
   */
  diag_init ();

  /* Notify that we are ready to start.  When this function returns, we
     are started. */
  diag_ready_start_task ();

  /* Forid sleep during init. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

#ifdef FEATURE_DIAG_CACHE
  /* Read the packet processing cache.
   */
  diagnv_refresh_cache ();
#endif

#if defined(FEATURE_EFS) && !defined (FEATURE_DIAG_NON_STREAMING)
  /* Process filestream mode, if applicable */
/*lint -save -e{545} Suspicious use of & */
  while (diag_fs_inbound_mode (&diag_req_inbound,
                   sizeof (diag_req_inbound)) == TRUE)
/*lint -restore */
  {
    /* While continuing to process the file stream, 
       make sure task signals are processed. */
    (void) diag_handle_sigs (0xFFFFFFFFU);
  }

  /* The powerup event cannot be sent until the input file has processed.  
     This is the only way the event service would be enabled. */
#endif

  /* Send powerup event */
  event_report (EVENT_POWERUP);

  /* Initialize the panic mode powerdown option */
  nvstatus = diag_nv_read (NV_ERR_FATAL_OPTIONS_I, (PACKED void *) &option);

  if (nvstatus == NV_DONE_S)
  {
    err_set_auto_action((err_action_type)option.option);
  }
  else
  {
    /* Nothing to do here.  Default behavior defined in err_init(). */
  }

#ifdef FEATURE_DIAG_SPC_TTL
  if ( (diag_nv_read (NV_DIAG_SPC_UNLOCK_TTL_I, &diag_spc_ttl))  == NV_DONE_S )
  {
    if (diag_spc_ttl)
    {
      diag_spc_ttl--;
      diag_set_sp_state (DIAG_SPC_UNLOCKED);
      diag_nv_write (NV_DIAG_SPC_UNLOCK_TTL_I, &diag_spc_ttl);
    }
  }
#endif

#ifdef FEATURE_DIAG_SPC_TIMEOUT
  nvstatus = diag_nv_read (NV_DIAG_SPC_TIMEOUT_I, &diag_spc_tout);

  if ( ( (nvstatus == NV_DONE_S) && (diag_spc_tout < (DIAG_TOUT_TIME / 1000)) ) ||
       (nvstatus != NV_DONE_S) )
  {
    diag_spc_tout = (DIAG_TOUT_TIME / 1000);
    diag_nv_write (NV_DIAG_SPC_TIMEOUT_I, &diag_spc_tout);
  }
#endif

  /* Vote OK to sleep for the diagnostic task. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);

#ifdef FEATURE_DIAG_MANUAL_DISPATCH_TABLE

    diagpkt_init_manual_dispatch_table ();

#endif

/*----------------------------------------------------------------------------
                           Begin task operations
----------------------------------------------------------------------------*/
  /* Get task signals */
  (void) diag_handle_sigs (0);

}               /* diag_task */

/*=========================================================================*/
boolean
diag_idle_processing_register (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean found = FALSE;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* if not yet found in the table, look for a NULL entry. */
    for (i = 0; !found && i < DIAG_IDLE_PROC_CNT; i++)
    {
      if (diag_idle_proc_tbl[i].fp == NULL)
      {
    /* Found an empty table entry. */
    found = TRUE;
    status = TRUE;

    diag_idle_proc_tbl[i].fp = fp;
    diag_idle_proc_tbl[i].param = param;
      }
    }

  }

  return status;
}

/*=========================================================================*/
boolean
diag_idle_processing_unregister (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean found = FALSE;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* Match and remove from table. */
    for (i = 0; !found && i < DIAG_IDLE_PROC_CNT; i++)
    {
      if (diag_idle_proc_tbl[i].fp == fp &&
      diag_idle_proc_tbl[i].param == param)
      {
    /* Found an empty table entry. */
    found = TRUE;
    status = TRUE;

    diag_idle_proc_tbl[i].fp = NULL;
    diag_idle_proc_tbl[i].param = NULL;
      }
    }
  }

  return status;
}

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING

DESCRIPTION
  This procedure is called when the DIAG task loop has nothing else to do,
  i,e when diag is idle.
  
  This routine traverses the list of registered functions and calls them
  in order of registration.
  
  This enables clients to regain DIAG task context from async interfaces.

===========================================================================*/
static void
diag_idle_processing (void)
{
  static boolean inprogress = FALSE;
  int i;

  if (!inprogress)
  {
    /* Prevent reentrant calls. */
    inprogress = TRUE;

    /* Call all idle processing functions that are registered. */
    for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
    {
      if (diag_idle_proc_tbl[i].fp)
      {
    diag_idle_proc_tbl[i].fp (diag_idle_proc_tbl[i].param);
      }
    }

    inprogress = FALSE;
  }

  return;
}

/*=========================================================================*/
void
diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote)
{
  /* Initial state = OK to sleep. */
  static uint32 ballot_mask = 0;
  uint32 vote_mask = 0;

  /* Assumption: the initial state is that sleep is not allowed.  
     This is established in diag_init (). */

  if (diag_svc < DIAG_SLEEP_CNT)
  {
    vote_mask = 1 << diag_svc;

    if (vote == TRUE)
    {
      /* This service is OK to sleep, 0 the bit. */
      DIAG_CLR_MASK (ballot_mask, vote_mask);

      /* If not connected, always allow sleep. */
      if (!DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) || ballot_mask == 0)
      {
    diag_allow_sleep ();
      }
    }
    else
    {
      /* This service is not OK to sleep, set the bit to 1. */
      DIAG_SET_MASK (ballot_mask, vote_mask);

      /* Must be connected to forbid sleep. */
      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) && ballot_mask != 0)
      {
    diag_forbid_sleep ();
      }
    }
  }
}

/*=========================================================================*/
diag_security_type
diag_get_security_state (void)
{
  return diag_security;
}

/*=========================================================================*/
boolean
diag_set_security_state (diag_security_type security)
{
  MSG_LOW ("Changing security state to 0x%x", security, 0, 0);

  diag_security = security;

  return TRUE;
}

/*=========================================================================*/
boolean
diag_set_sp_state (diag_sp_type sp)
{
  MSG_LOW ("Changing service programming lock state to 0x%x", sp, 0, 0);

  diag_sp = sp;

  return TRUE;
}

/*=========================================================================*/
diag_sp_type
diag_get_sp_state (void)
{
  return diag_sp;
}

/*=========================================================================*/
boolean
diag_check_password (void *code_ptr)
{
  int status;
  void *ptr;

  if (code_ptr == NULL)
  {
    ptr = (void *) diag_no_security_code_c;
  }
  else
  {
    ptr = code_ptr;
  }

  status = memcmp (ptr, chk_security_code_ptr, DIAG_PASSWORD_SIZE);

  return (status == 0) ? TRUE : FALSE;

}               /* diag_check_password */

/*=========================================================================*/
static int
diag_searchlist_insert (diag_searchlist_node_type list[], 
                        int count, int size,
                        diag_searchlist_node_type new_node)
{
  int i;

  if (list && count >= 0 && count < size)
  {
    for (i = count - 1; i >= 0 && list[i].key > new_node.key; i--)
    {
      /* Sort nodes from the end of the list until a match is found */
      list[i+1] = list[i];
    }
    
    /* 'i+1' offsets the i-- of the for loop */
    list[i+1] = new_node;

    count++;
  }

  return count;
}

/*=========================================================================*/
/* This routine performs a binary search on the given searchlist.
   
   If found, this routine returns the key.  If duplicate entries 
   exist, this routine returns one of them.
   
   If not found, this routine returns -1. */
static int 
diag_searchlist_search (unsigned int key, 
                        diag_searchlist_node_type list[],
                        int count)
{
  int low, i, high;

  if (list)
  {
    low = (-1);
    high = count;

    while (high - low > 1)
    {
      i = (high + low) / 2;

      if (key <= list[i].key)
      {
        high = i;
      }
      else
      {
        low = i;
      }
    }

    if (key != list[high].key)
    {
      /* Force an exit of the loop since 'low' is never less than -1. */
      high = (-1);
    }
    else
    {
      /* 'high' contains the matched index */
    }
  }
  else
  {
    high = (-1);
  }

  return high;
}

/*=========================================================================*/
/* returns new count.  count will be the same if failed */
static int
diag_searchlist_remove (diag_searchlist_node_type list[],
                        int count,
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp)
{
  int i, result;
  boolean found = FALSE;

  result = diag_searchlist_search (node.key, list, count);

  if (result >= 0)
  {
    /* Search before and after the found entry for the first match. */
    for (i = result; i < count && list[i].key == node.key; i++)
    {
      if (match_fp)
      {
        if (match_fp (&node, &list[i]))
        {
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
      else if (node.goo == list[i].goo)
      {
        found = TRUE;
        break; /* Avoid incrementing 'i' */
      }
    }

    if (!found)
    {
      for (i = result - 1; i >= 0 && list[i].key == node.key; i--)
      {
        if (match_fp)
        {
          if (match_fp (&node, &list[i]))
          {
            found = TRUE;
            break; /* Avoid incrementing 'i' */
          }
        }
        else if (node.goo == list[i].goo)
        {
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
    }

    if (found)
    {
      /* 'i' holds the index to the matched entry. */
      for (count--; i < count; i++)
      {
        /* Move entries up the array to replace the removed entry. */
        list[i] = list[i+1];
      }
    }
  }

  return (count);
}

/*=========================================================================*/
//NOTE: If 'notify' or 'payload' are NULL, the return value will indicate 
//existance of the key, but no action is taken to deliver the node.
boolean
diag_searchlist_search_all (unsigned int key, 
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify)
{
  int i, count; 
  int result = -1;
  uint32 seq_num = 0;
  diag_searchlist_node_type *r = NULL;
  boolean found = FALSE;

  /* !!! This function assumes the caller is not in IRQ mode since it
     is not part of the externalized API of listeners. */
  if (list && list->initialized && !rex_is_in_irq_mode ())
  {
    /* Replaced rex critical sections with intlocks, since we cannot
    ** acquire a lock on critcal section when tasks are locked.
    */

    INTLOCK();

    count = list->count;
    r = list->r;
    
    result = diag_searchlist_search (key, r, count);

    found = (result >= 0) ? TRUE : FALSE;

    INTFREE();

    /* Only notify and search for duplicates if 'notify' is specified. */
    if (found && notify && payload)
    {
       /* Increment the sequence number for each successful search. */
       seq_num = list->seq_num + 1;

      /* Search before and after the found entry for duplicate entries. */
      for (i = result; i < count && r[i].key == key; i++)
      {
        notify (seq_num, payload, &r[i]);
      }

      for (i = result - 1; i >= 0 && r[i].key == key; i--)
      {
        notify (seq_num, payload, &r[i]);
      }

      /* Save updated sequence number */
      list->seq_num = seq_num;
    }
  }

  return (found);
}

/*=========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo)
{
  int new_count = 0;
  diag_searchlist_node_type node;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  boolean success = FALSE;
  
  /* Fill in the node - this is passed by value */
  node.key = key;
  node.goo = goo;
  
  if (list && !rex_is_in_irq_mode ())
  {
    if (!list->initialized)
    {
      TASKLOCK ();
      /* This check is done twice to avoid TASKLOCK for every check 
         of initialization and multiple initialization.  
         Too bad C doesn't have constructors... */
      if (!list->initialized)
      {
        rex_init_crit_sect (&list->crit_sect);
        list->initialized = TRUE;
        list->seq_num = 0;
      }
      TASKFREE ();
    }

    /* Enter critical section to avoid corrupt list */
    rex_enter_crit_sect (&list->crit_sect);

    /* If the list doesn't have an array allocated, allocate one. */
    if (!list->r)
    {
      list->count = 0;
      list->size = DIAG_SEARCHLIST_SIZE_INCR;

      list->r = (diag_searchlist_node_type *) diag_malloc (
        (uint32)(list->size) * sizeof (diag_searchlist_node_type));

      if (!list->r)
      {
        /* reset size */
        list->size = 0;
      }
    }

    if (list->r)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_searchlist_add_malloc_cnt++;
      MSG_FATAL("MALLOC diag_searchlist_add_malloc_cnt = %d, heap used = %d ",
          diag_searchlist_add_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      if (list->count >= list->size)
      {
        /* There isn't enough room in this array to add.  Allocate
           a new one and, if successful, copy the old list. */
        new_r = (diag_searchlist_node_type *) diag_malloc (
          ((uint32)(list->size) + DIAG_SEARCHLIST_SIZE_INCR) * 
          sizeof (diag_searchlist_node_type));

        if (new_r)
        {
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_malloc_cnt++;
          MSG_FATAL("MALLOC diag_searchlist_add_malloc_cnt = %d, heap used = %d ",
              diag_searchlist_add_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          /* Only copy valid entries */
          memcpy (new_r, list->r, 
                  (uint32)(list->count) * sizeof (diag_searchlist_node_type));
 
          list->size += DIAG_SEARCHLIST_SIZE_INCR;

          /* free the old list and assign the new array to this list */
          diag_free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_process_rx_free_cnt++;
          MSG_FATAL("FREE diag_process_rx_free_cnt = %d, diag heap used = %d ",
              diag_process_rx_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          list->r = new_r;
          new_r = NULL;
        }
      }

      if (list->count < list->size)
      {
        new_count = diag_searchlist_insert (list->r, list->count, 
                                            list->size, node);

        if (new_count > list->count)
        {
          list->count = new_count;
          success = TRUE;
        }
        else if (list->count == 0)
        {
          diag_free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_process_rx_free_cnt++;
          MSG_FATAL("FREE diag_process_rx_free_cnt = %d, diag heap used = %d ",
              diag_process_rx_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
          list->size = 0;
          list->r = NULL;
        }
      }
    }
    
    rex_leave_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list, 
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp)
{
  int new_count = 0;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  boolean success = FALSE;
    
  if (list && list->initialized && !rex_is_in_irq_mode ())
  {
    /* Enter critical section to make sure the list doesn't change */
    rex_enter_crit_sect (&list->crit_sect);

    if (list->r)
    {
      new_count = diag_searchlist_remove (
        list->r, list->count, node, match_fp);

      if (new_count < list->count)
      {
        list->count = new_count;
        success = TRUE;
        
        if (new_count == 0)
        {
          diag_free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_delete_free_cnt++;
          MSG_FATAL("FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
              diag_searchlist_delete_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          list->r = NULL;
          list->size = NULL;
        }
        else if (list->size - new_count >= DIAG_SEARCHLIST_SIZE_INCR)
        {
          /* Delete crossed the increment threshold.  Alloc a smaller
             list and, if successful, copy the old list. */
          new_r = (diag_searchlist_node_type *) diag_malloc (
            ((uint32)(list->size) - DIAG_SEARCHLIST_SIZE_INCR) * 
            sizeof (diag_searchlist_node_type));

          if (new_r)
          {
#ifdef FEATURE_DEBUG_DIAG_HEAP
            diag_searchlist_delete_malloc_cnt++;
            MSG_FATAL("MALLOC diag_searchlist_delete_malloc_cnt=%d,heapUsed=%d ",
                diag_searchlist_delete_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

            /* Only copy valid entries */
            memcpy (new_r, list->r, 
                    (uint32)new_count * sizeof (diag_searchlist_node_type));

            /* free the old list and assign the new array to this list */
            diag_free (list->r);

#ifdef FEATURE_DEBUG_DIAG_HEAP
            diag_searchlist_delete_free_cnt++;
            MSG_FATAL("FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
                diag_searchlist_delete_free_cnt, diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

            list->r = new_r;
            list->size -= DIAG_SEARCHLIST_SIZE_INCR;
            new_r = NULL;
          }
        }
      }
    }
    
    rex_leave_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_cmd_req (const byte *req, unsigned int length,
              const diag_cmd_rsp rsp_cb, void *param)
{
  diag_req_cmd_type *cmd = NULL;
  boolean success = FALSE;

  if (req && length > 0 && length <= DIAG_MAX_RX_PKT_SIZ)
  {
    cmd = (diag_req_cmd_type *) diag_malloc (
      FPOS (diag_req_cmd_type, req) + length);

    if (cmd)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_cmd_req_cnt++;
      MSG_FATAL("MALLOC diag_cmd_req_cnt = %d, heap used=%d ",diag_cmd_req_cnt,
          diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      cmd->rsp_func = rsp_cb;
      cmd->param = param;
      cmd->length = length;

      memcpy (cmd->req, req, length);

      q_put (&diag_req_cmd_q, q_link (cmd, &cmd->link));

#ifdef FEATURE_DEBUG_DIAG_HEAP
      MSG_FATAL("Q_PUT diag_cmd_req diag_req_cmd_q_cnt = %d heap used = %d",
          q_cnt (&diag_req_cmd_q),  diag_heap_used_bytes(),0); 
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* DIAG checks this queue as part of its normal RX processing. */
      diag_rx_notify ();

      success = TRUE;
    }
  }

  /* The handle is the address of the command buffer */
  return success;
}

#if defined (FEATURE_EFS) && !defined (FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION DIAG_FS_INIT

DESCRIPTION
  This function initializes diag filestream mode, enabling it if applicable.

===========================================================================*/
static void
diag_fs_init (diag_fs_type * fs)
{
  fs_rsp_msg_type fs_rsp;

  if (fs && fs->handle == FS_NULL_HANDLE)
  {
    fs_open (fs->fname, FS_OA_READONLY, NULL, NULL, &fs_rsp);

    if (fs_rsp.open.status == FS_OKAY_S)
    {
      fs->handle = fs_rsp.open.handle;

      fs_tell (fs->handle, NULL, &fs_rsp);

      if (fs_rsp.tell.status == FS_OKAY_S)
      {
    fs->read_pos = fs_rsp.tell.position;
      }
      else
      {
    diag_fs_terminate (fs);
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_MODE

DESCRIPTION
  This function handles all inbound operations for the filestream mode.

===========================================================================*/
static boolean
diag_fs_inbound_mode (void *buf, unsigned int size)
{
  static boolean initialized = FALSE;
  boolean filestream_mode_active = FALSE;

  if (!initialized)
  {
    initialized = TRUE;

    /* Initialize diag input file streams. */
    diag_fs_init (&diag_fs_config);

    diag_fs_init (&diag_fs_in);
  }

  ASSERT (buf && size > 0);

  /* If filestream mode is active, process the input file, then 
     detect valid comm layer connection and buffer any inbound data.
     Once the input file is complete, the buffered inbound data will
     be processed. */
  if (diag_fs_config.handle != FS_NULL_HANDLE)
  {
    filestream_mode_active = TRUE;

    MSG_LOW ("Processing input file", 0, 0, 0);

    diag_fs_process_inbound (&diag_fs_config, buf, size);

    /* This is unnecessary if the input file is closed. */
    if (diag_fs_config.handle != FS_NULL_HANDLE)
    {
      MSG_LOW ("Attempting inbound comm buffering.", 0, 0, 0);
      diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
    }
  }
  else if (diag_fs_in.handle != FS_NULL_HANDLE)
  {
    filestream_mode_active = TRUE;

    MSG_LOW ("Processing input file", 0, 0, 0);

    diag_fs_process_inbound (&diag_fs_in, buf, size);

    /* This is unnecessary if the input file is closed. */
    if (diag_fs_in.handle != FS_NULL_HANDLE)
    {
      MSG_LOW ("Attempting inbound comm buffering.", 0, 0, 0);
      diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
    }
  }
  else if (diag_fs_inbuf.handle != FS_NULL_HANDLE)
  {
    MSG_LOW ("Input file complete", 0, 0, 0);
    filestream_mode_active = TRUE;

    if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
    {
      /* Since we have established a connection, the temporary file must
         contain at least one good packet. */
      MSG_LOW ("Processing buffered comm input", 0, 0, 0);
      diag_fs_process_inbound (&diag_fs_inbuf, buf, size);

      if (diag_fs_inbuf.handle != FS_NULL_HANDLE)
      {
    /* Some data still exists in the file, continue comm layer buffering,
       if inbound comm layer data exists.  If this state continues too long,
       the max file size of the buffer file will be reached and no more data
       will be written.  At that point, the processing of the stream can 
       "catch up" to the comm layer buffering. */
    MSG_LOW ("Attempting buffer of inbound comm", 0, 0, 0);
    diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
      }
    }
    else
    {
      /* At this point, the procesing of the original input file is
         complete and we haven't yet established a connection on the comm
         layer.  If the buffer file has data, it may or may not be useful 
         data.  Since the buffer emulates normal comm layer processing, 
         "catch up" to the comm layer and then exit filestream mode. */
      MSG_LOW ("Catch up: buffered comm input", 0, 0, 0);

      while (diag_fs_inbuf.handle != FS_NULL_HANDLE)
      {
    diag_fs_process_inbound (&diag_fs_inbuf, buf, size);
      }

      /* Input buffer processing is complete, remove the file. */
      diag_fs_terminate (&diag_fs_inbuf);
    }
  }

  return filestream_mode_active;
}

/*===========================================================================

FUNCTION DIAG_FS_TERMINATE

DESCRIPTION
  This function closes and deletes the specified file.

===========================================================================*/
static void
diag_fs_terminate (diag_fs_type * fs)
{
  fs_rsp_msg_type fs_rsp;

  if(fs)
  {
    if (fs->handle != FS_NULL_HANDLE)
    {
      /* Clean up input file */
      fs_close (fs->handle, NULL, &fs_rsp);
      fs->handle = FS_NULL_HANDLE;
    }

    /* Remove the input file. */
    fs_remove (fs->fname, NULL, &fs_rsp);

    if (fs_rsp.rmfile.status != FS_OKAY_S &&
       fs_rsp.rmfile.status != FS_NONEXISTENT_FILE_S)
    {
       ERR ("Input file delete fail! Stat: %d", fs_rsp.rmfile.status, 0, 0);
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_READ

DESCRIPTION
  This function reads inbound data from the specified filestream.

RETURN VALUE
  Number of bytes read.

===========================================================================*/
static uint32
diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size)
{
  fs_rsp_msg_type fs_rsp;
  unsigned int read_count = 0;

  if (fs && buf && fs->handle != FS_NULL_HANDLE && size > 0)
  {
    fs_seek (fs->handle, FS_SEEK_SET, fs->read_pos, NULL, &fs_rsp);

    if (fs_rsp.seek.status == FS_OKAY_S)
    {

      fs_read (fs->handle, buf, size, NULL, &fs_rsp);

      if ((fs_rsp.read.status == FS_EOF_S || fs_rsp.read.status == FS_OKAY_S)
      && fs_rsp.read.count > 0)
      {
    read_count = fs_rsp.read.count;

    fs_tell (fs->handle, NULL, &fs_rsp);

    if (fs_rsp.tell.status == FS_OKAY_S)
    {
      fs->read_pos = fs_rsp.tell.position;
    }
    else
    {
      diag_fs_terminate (fs);
    }
      }
      else
      {
    diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }

  return read_count;
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE

DESCRIPTION
  This function writes data to the file specified by 'fs'.
  
RETURN VALUE
  Boolean indicating success.
  
===========================================================================*/
static boolean
diag_fs_write (diag_fs_type * fs, void *buf, unsigned int size)
{
  fs_rsp_msg_type fs_rsp;
  boolean success = FALSE;

  if (fs && fs->handle)
  {
    /* Assumption: all writes are at the end of the file. */
    fs_seek (fs->handle, FS_SEEK_SET, fs->write_pos, NULL, &fs_rsp);

    if (fs_rsp.seek.status == FS_OKAY_S)
    {

      fs_write (fs->handle, buf, size, NULL, &fs_rsp);

      if (fs_rsp.write.status == FS_OKAY_S && fs_rsp.write.count == size)
      {
    success = TRUE;
    fs->write_pos += fs_rsp.write.count;
      }
      else
      {
    diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }
  else
  {
    diag_fs_terminate (fs);
  }

  return success;
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_INBOUND

DESCRIPTION
  This function processes inbound data from the specified file.
  
RETURN VALUE
  None.
  
===========================================================================*/
static void
diag_fs_process_inbound (diag_fs_type * fs, void *buf, unsigned int size)
{
  diag_hdlc_decode_type hdlc;
  unsigned int src_buf_size = 0;

  /* Length of packet, if boundary is found.  
     0 is returned if no packet found. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  /* Allocate a buffer from the comm layer to use as a work buffer
     for the file stream input processing. */
  src_buf_size = 256;
  hdlc.src_ptr = diag_malloc (src_buf_size);

  if (hdlc.src_ptr)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_malloc_cnt++;
    MSG_FATAL("MALLOC diag_fs_process_inbound_malloc_cnt = %d,heap used=%d ",
        diag_fs_process_inbound_malloc_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    if (fs && fs->handle != FS_NULL_HANDLE)
    {
      hdlc.dest_ptr = buf;
      hdlc.dest_size = size;

      do
      {
    /* If the src buffer has been exhausted, time to read more */
    if (hdlc.src_size == hdlc.src_idx)
    {
      hdlc.src_size = (uint32)diag_fs_read (fs, hdlc.src_ptr, src_buf_size);

      if (hdlc.src_size > 0)
      {
        hdlc.src_idx = 0;
      }
      else
      {
        diag_fs_terminate (fs);
        break;
      }
    }

    if (hdlc.src_size > hdlc.src_idx)
    {
      if (diag_extract_pkt (&hdlc) == TRUE)
      {
        /* dest_idx is the length of the decoded packet */
        diagpkt_process_request (buf, (uint16) hdlc.dest_idx, NULL, NULL);

        /* If src_ptr is not exhausted, update read_pos before exit. */
        if (hdlc.src_size > hdlc.src_idx)
        {
          fs->read_pos -= hdlc.src_size - hdlc.src_idx;
        }
        break;
      }
    }
      }
      while (fs->handle != FS_NULL_HANDLE);
    }

    diag_free (hdlc.src_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_free_cnt++;
    MSG_FATAL("FREE diag_fs_process_inbound_free_cnt = %d, heap used = %d ",
        diag_fs_process_inbound_free_cnt,diag_heap_used_bytes(),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_COMM

DESCRIPTION
  This function buffers inbound comm layer data to the file 'fs'.
  
  A temporary buffer is used to decode the inbound data and check for a 
  valid packet.
  
RETURN VALUE
  None.
  
===========================================================================*/
static void
diag_fs_inbound_comm (diag_fs_type * fs, void *buf, unsigned int size)
{
  fs_rsp_msg_type fs_rsp;
  diag_hdlc_decode_type hdlc;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  if (fs)
  {
    if (fs->handle == FS_NULL_HANDLE)
    {
      fs_nametest (fs->fname, FS_TEST_FILE, NULL, &fs_rsp);

      if (fs_rsp.nametest.status == FS_OKAY_S
      && fs_rsp.nametest.name_found == TRUE)
      {
    fs_remove (fs->fname, NULL, &fs_rsp);
      }

      fs_open (fs->fname, FS_OA_CREATE,
           (fs_open_xparms_type *) & diag_fs_write_open_parms,
           NULL, &fs_rsp);

      if (fs_rsp.open.status == FS_OKAY_S)
      {
    fs->handle = fs_rsp.open.handle;

    fs_tell (fs->handle, NULL, &fs_rsp);

    if (fs_rsp.tell.status == FS_OKAY_S)
    {
      fs->read_pos = fs_rsp.tell.position;
      fs->write_pos = fs_rsp.tell.position;
    }
      }
    }

    /* Initialize HDLC structure w/ working buffer. */
    hdlc.dest_ptr = buf;
    hdlc.dest_size = size;

    /* If the file is open, buffer to the file. */
    while (fs->handle != FS_NULL_HANDLE)
    {

      if (hdlc.src_ptr == NULL)
      {

    /* Make sure the file is not at max size yet. */
    if (fs->write_pos < fs->max_size)
    {
      /* Get a buffer from the comm layer.  If a valid packet is found, 
         declare a "connected" status and buffer the input to the file. */
      hdlc.src_ptr = diagcomm_inbound (&hdlc.src_size);
      hdlc.src_idx = 0;
    }
      }

      if (hdlc.src_ptr == NULL)
      {
    break;
      }

      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
      {
    /* We are already connected, so just buffer the data and move on. */
    (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
    diagcomm_inbound_complete (hdlc.src_ptr);
    hdlc.src_ptr = NULL;
      }
      else
      {
    /* Not connected - must look for a valid packet to connect. */
    if (hdlc.src_idx < hdlc.src_size)
    {
      if (diag_extract_pkt (&hdlc) == TRUE)
      {
        /* Valid packet means we can assume a connection. */
        DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);
      }
    }
    else
    {
      /* The input buffer has been exhausted.  Free the buffer.  The
         next iteration of the loop will grab a new one. */
      if (hdlc.dest_idx > 0)
      {
        /* This indicates that a decode is in progress.  Write the 
           buffer to the file in case it is part of the first 
           valid packet. */
        (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
      }
      else
      {
        /* Delete any accumulated data thus far and reopen.  Since
           there is no decode in progress, and diag is not connected, 
           all inbound data thus far is garbage. */
        diag_fs_terminate (fs);
      }

      /* Finished with this inbound buffer. */
      diagcomm_inbound_complete (hdlc.src_ptr);
      hdlc.src_ptr = NULL;
    }
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_CONFIG

DESCRIPTION
  This function checks if the given packet has potentially changed the
  existing diag config file.  If so, a new file is generated and writen
  to the file.
    
===========================================================================*/
static void
diag_fs_process_config (void *req_pkt, word pkt_len)
{
  (void) pkt_len; /* suppress compiler warning */

  if (req_pkt)
  {
    switch (diagpkt_get_cmd_code (req_pkt))
    {
      //TODO: For now, only event reporting is supported.
      case DIAG_EVENT_REPORT_F:
        /* Save the diagnostics service configuration for the next power cycle. */
        diag_fs_write_config ();
        break;
      default:
        break;
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE_CONFIG

DESCRIPTION
  This function saves the diagnostic services' configurations to a file to be
  read at the next power cycle.  The config packet is stored in the form of
  a raw (HDLC encoded) input stream with commands to configure the 
  appropriate services.
    
===========================================================================*/
extern uint8 event_config;  //XXX
word diag_do_escaping (const byte * src_ptr, byte * dest_ptr, word src_length,
               word dest_length, unsigned int *src_used);

static void
diag_fs_write_config (void)
{
  union
  {
    event_cfg_req_type pkt;
    byte buf[sizeof (event_cfg_req_type) + 3];
  }
  event_pkt;
  uint16 event_pkt_len = sizeof (event_cfg_req_type);
  unsigned int encoded_pkt_len = 0;

  byte buf[20];         //XXX
  word crc;
  diag_fs_type fs;
  fs_rsp_msg_type fs_rsp;

  memset (&fs, 0, sizeof (diag_fs_type));

  fs.handle = FS_NULL_HANDLE;
  fs.fname = ".DIAGCFG";
  fs.max_size = 0x8000;     /* 32K is more than enough - it will never get this big. */

  fs_nametest (fs.fname, FS_TEST_FILE, NULL, &fs_rsp);

  if (fs_rsp.nametest.status == FS_OKAY_S
      && fs_rsp.nametest.name_found == TRUE)
  {
    fs_remove (fs.fname, NULL, &fs_rsp);
  }

  fs_open (fs.fname, FS_OA_CREATE,
       (fs_open_xparms_type *) & diag_fs_write_open_parms, NULL, &fs_rsp);

  if (fs_rsp.open.status == FS_OKAY_S)
  {
    fs.handle = fs_rsp.open.handle;
  }

  if (fs.handle != FS_NULL_HANDLE)
  {
    diagpkt_set_cmd_code (&event_pkt, DIAG_EVENT_REPORT_F);

    event_pkt.pkt.enable = (event_config != 0) ? TRUE : FALSE;

    crc = crc_16_l_calc ((void *) &event_pkt,
             (uint16) (sizeof (event_cfg_req_type) * 8));

    /* Write the CRC to the end of unescaped packet.
       ** The CRC will be escaped with everything else.
     */
    event_pkt.buf[event_pkt_len++] = crc & 0x00ff;
    event_pkt.buf[event_pkt_len++] = crc >> 8;

    /* Reuse event_pkt_len to now hold size of HDLC encoded packet */
    event_pkt_len = diag_do_escaping (event_pkt.buf, buf, event_pkt_len, 20,
                      &encoded_pkt_len);

    buf[event_pkt_len++] = DIAG_ASYNC_CONTROL_CHAR;

    (void) diag_fs_write (&fs, buf, event_pkt_len);

    fs_close (fs.handle, NULL, &fs_rsp);
  }
}

#endif /* FEATURE_EFS && !FEATURE_DIAG_NON_STREAMING */

/*===========================================================================

FUNCTION DIAG_DO_ESCAPING

DESCRIPTION
  This routine provides XOR escaping functionality for the use of callers.
  Both Control character and Escape character will be escaped by substituting
  the character with the following two characters:
  
  <DIAG_ASYNC_ESC_CHAR> <src_byte ^ DIAG_ASYNC_ESC_MASK>

DEPENDENCIES
  src_ptr & dest_ptr cannot be the same nor overlapping.
  
RETURN VALUE
  Returns the number of bytes that were stored into dest_ptr.
  
  Also, via pointers, returns src_used.
    
===========================================================================*/
word 
diag_do_escaping ( 
  const byte *src_ptr, 
  /* This is the pointer that contains the data that needs to be 
  ** escaped
  */

  byte       *dest_ptr, 
  /* This is the pointer into which the escaped data should be 
  ** stored.
  */

  word        src_length, 
  /* This is the number of bytes to convert.
  */

  word        dest_length,
  /* This is the number of bytes that can be stored in dest_ptr.
  */

  unsigned int *src_used
  /* This will contain the number of values which were read from 
  ** src_ptr. 
  */
)
{
  word i;
  byte src_byte;
  word converted_length = 0;
  word check_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (dest_length >= DIAG_ESC_EXPAND_SIZE);
  ASSERT (src_ptr != NULL);
  ASSERT (dest_ptr != NULL);

  if (dest_length >= (src_length << DIAG_ESC_EXPAND_SHIFT))
  {
    /* Do not need to check for size for each character
     */
    converted_length = src_length;

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

    *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
    *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;

    /* Destination became one character 
       ** larger than projected.
     */
    converted_length++;
      }
      else
      {
    *dest_ptr++ = src_byte;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length;
  }             /* if dest_length >= src_length << 1 */
  else
  {
    check_size = dest_length - DIAG_ESC_EXPAND_SIZE;

    /* Do need to check for size.  Right now check size for 
       ** each character.  Could further optimize, which would
       ** make code more complicated but faster.
     */

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

    *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
    *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
    converted_length += DIAG_ESC_EXPAND_SIZE;
      }
      else
      {
    *dest_ptr++ = src_byte;
    converted_length++;
      }

      if (converted_length > check_size)
      {
    i--;
    break;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length - i;
  }             /* else if dest_length < src_length << 1 */

  return converted_length;
}               /* diag_do_escaping */

/*===========================================================================

FUNCTION DIAG_HDLC_DECODE

DESCRIPTION
  This routine provides XOR unescaping functionality for the use of callers.
  This routine is the inverse of do_escaping.

  When DIAG_ASYNC_ESC_CHAR is found, it is discarded and the next character
  is unescaped as follows <src_byte ^ DIAG_ASYNC_ESC_MASK>.  This character is 
  stored in dest_ptr.
  
  When an unescaped DIAG_ASYNC_CONTROL_CHAR is found, this routine will exit 
  immediately and indicate that an unescaped control character was found, in
  the parameter found_control_char.
  
  src_ptr and dest_ptr can be the same or overlapping.
  
DEPENDENCIES
  When an escape character straddles two packets, escaping will indicate this
  fact.  The caller needs to pass this parameter (unscathed) to 
  do_unescaping in order for un-escaping to be done correctly.
  
RETURN VALUE
  A boolean indicating that the control character was found.
      
===========================================================================*/
static boolean
diag_hdlc_decode (diag_hdlc_decode_type * hdlc)
{
  byte *src_ptr = NULL, *dest_ptr = NULL;
  unsigned int src_length = 0, dest_length = 0;

  unsigned int converted_length = 0;
  unsigned int i;
  byte src_byte;
  boolean pkt_bnd = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr &&
      (hdlc->src_size - hdlc->src_idx > 0) &&
      (hdlc->dest_size - hdlc->dest_idx > 0))
  {

    /* Copy paramters to local variables for optimized memory access */
    src_ptr = hdlc->src_ptr;
    src_ptr = &src_ptr[hdlc->src_idx];
    src_length = hdlc->src_size - hdlc->src_idx;

    dest_ptr = hdlc->dest_ptr;
    dest_ptr = &dest_ptr[hdlc->dest_idx];
    dest_length = hdlc->dest_size - hdlc->dest_idx;

    for (i = 0; i < src_length; i++)
    {
      src_byte = src_ptr[i];

      if (hdlc->escaping)
      {
    dest_ptr[converted_length++] = src_byte ^ DIAG_ASYNC_ESC_MASK;
    hdlc->escaping = FALSE;

      }
      else if (src_byte == DIAG_ASYNC_ESC_CHAR)
      {
    /* If DIAG_ASYNC_ESC_CHAR is found at the end of src_ptr, 
       ** there is nothing to un-escape.  So, no choice
       ** but to die
     */
    if (i == (src_length - 1))
    {
      hdlc->escaping = TRUE;
      i++;
      break;
    }
    else
    {
      dest_ptr[converted_length++] = src_ptr[++i] ^ DIAG_ASYNC_ESC_MASK;
    }
      }
      else if (src_byte == DIAG_ASYNC_CONTROL_CHAR)
      {
    /* Found control character. 
     */
    dest_ptr[converted_length++] = src_byte;
    pkt_bnd = TRUE;
    i++;
    break;
      }
      else
      {
    dest_ptr[converted_length++] = src_byte;
      }

      if (converted_length >= dest_length)
      {
    /* If more data cannot be stored into dest_ptr,
       ** exit loop
     */
    i++;
    break;
      }
    }               /* for i=0; i<src_length; i++ */

    hdlc->src_idx += i;
    hdlc->dest_idx += converted_length;
  }

  return pkt_bnd;
}               /* diag_hdlc_decode */

#if !defined(FEATURE_DIAG_NON_STREAMING)
/*===========================================================================

FUNCTION DIAG_PANIC_ISR_CLEANUP

DESCRIPTION
  This procedure removes all ISRs from tramp except those needed for DIAG to
  communicate to the external device (in particular, to flush queued data).
  
DEPENDENCIES
  Depends on DIAG being the highest priority task next to the services needed
  to send remaining data.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_IS2000
void
diag_panic_isr_cleanup (void)
{
  tramp_isr_type isr = (tramp_isr_type)0;

  /* Clear all non-essential ISRs. */
  for (isr = (tramp_isr_type)0; isr < TRAMP_ISR_CNT; isr++)
  {

    /* The following may be needed for DIAG's TX operation */
    if (!((isr == TRAMP_TICK_ISR) ||    /* SIO requires timers for flush */
      (isr == TRAMP_TICK2_ISR) ||
#ifdef FEATURE_SECOND_UART
      (isr == TRAMP_UART2_ISR) ||
#endif
#ifdef FEATURE_USB_DIAG
      (isr == TRAMP_USB_ISR) ||
#endif
      (isr == TRAMP_UART_ISR)))
    {

      tramp_set_isr (isr, NULL);
    }
  }
}
#else
void
diag_panic_isr_cleanup (void)
{
  tramp_int_num_type isr = (tramp_isr_type)0;

  /* Clear all non-essential ISRs. */
  for (isr = (tramp_isr_type)0; isr < TRAMP_NUM_INTS; isr++)
  {

    /* The following may be needed for DIAG's TX operation */
    if (!(
#ifdef FEATURE_USE_TIME_VU
      (isr == TRAMP_SLEEP_TIME_TICK_ISR) ||
#else
      (isr == SLEEP_TIMER_INT0) ||
#endif
#ifdef FEATURE_SECOND_UART
      (isr == TRAMP_UART2_ISR) ||
#endif
#ifdef FEATURE_THIRD_UART
      (isr == TRAMP_UART3_ISR) ||
#endif
#ifdef FEATURE_USB_DIAG
      (isr == USB_INT) ||
#endif
      (isr == TRAMP_UART_ISR)))
    {

      tramp_set_isr (isr, NULL);
    }
  }
}
#endif

/*===========================================================================

FUNCTION DIAG_ENTER_PANIC_MODE

DESCRIPTION
  This procedure tells the diag task to enter panic mode.  Panic mode is 
  used during a fatal error to flush all buffered data in the diagnostics
  system.  
  
  To accomplish this, the DIAG task is given highest priority.  This
  routine never relinquishes control of CPU via REX, so all other tasks
  are starved.  The DIAG task enters diag_panic_mode_handler() the next
  time it calls diag_wait().  See diag_panic_mode_handler() for details.
  
DEPENDENCIES
  Depends on DIAG being the highest priority task next to the services needed
  to send remaining data.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef FEATURE_L4
static int diag_panic_countdown = 200000;   /* arbitrary number */
#endif

void
diag_enter_panic_mode (void)
{
#if !defined(_disable)
  extern void _disable (void);
#endif

  /* Disable all interrupts.  DIAG will take over ISR servicing. */
  _disable ();

#ifndef FEATURE_L4
  if (rex_is_in_irq_mode ())
  {
      /* We need to get our of IRQ mode to allow us to be able to lock */
      /* and schedule tasks, etc.  */

#ifndef FEATURE_DIAG_DEBUG_6275
#if defined (FEATURE_WCDMA) && defined (FEATURE_GSM) && defined (FEATURE_ERR_EXTENDED_STORE)
#error code not present
#endif
#endif /* FEATURE_DIAG_DEBUG_6275 */

      /* Force the phone back to Supervisor mode so that it leaves IRQ mode */
      (void)rex_set_cpsr (PSR_Supervisor | PSR_Irq_Mask);
  }

  /* To gracefully recover from endless loop of panic mode entries */
  if (!diag_cx_state || (--diag_panic_countdown <= 0))
  {
    diag_panic_end ();
  }

  rex_task_lock();

  /* Kick the hardware watchdog since the dog task will no longer run */
  dog_kick ();

  diag_panic_mode = TRUE;

  /* Stop all diagnostic services. */
  msg_set_all_rt_masks (0);
  log_panic ();
  event_flush ();

  /* Clear all non-essential ISRs in tramp. */
  diag_panic_isr_cleanup ();

  /* DIAG is now the highest priority task, except for the underlying 
     services needed to flush diagnostics data. */
  (void) rex_task_pri (&diag_tcb, 0xFFFFFFFFU);

  while(TASKS_ARE_LOCKED())
  {
      rex_task_free();
  }

  /* If still running, this must the DIAG task.  DIAG specifically ensures 
     ERR_FATALs that could cause reentrance in panic mode are handled 
     gracefully.  Only the TX chain matters, clients don't have to worry 
     about it. */
  
  if (rex_self () == &diag_tcb)
  {
    (void)diag_panic_mode_handler (0);
  }
  else
  {
/*lint -save -e{716} */
    while (1) { dog_kick (); tramp_isr(); }
/*lint -restore */
  }
#else        
#error code not present
#endif
}

/*===========================================================================

FUNCTION DIAG_PANIC_END

DESCRIPTION
  This procedure is called when panic mode ends.  It kills the target.
    
DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  This function does not return.

===========================================================================*/
static void
diag_panic_end (void)
{
#ifdef USES_RELEASE_VERSION
  hw_reset ();
#endif
  if (err_get_auto_action() == ERR_RESET)
  {
    /* Need this option to allow automation */
    hw_reset ();
  }
#ifdef CUST_EDITION
  else if(err_get_auto_action() == ERR_NO_ACTION)
  {
    return;
  }
#endif
  else
  {
#ifdef FEATURE_ERR_EXTENDED_STORE
    /* Displays error message, and allows user to reset phone manually */
    /* No return from this function call */
    boot_generic_error_handler();
#elif defined(FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL)
    /* Loop here for ever */
    while(1)
    {
        BOOTHW_KICK_WATCHDOG();
    }

#else     
    /* Default action */
    dload_entry ();
#endif /* FEATURE_ERR_EXTENDED_STORE */ 
  }

  /* None of the above should return.  Just in case... */
  hw_power_off ();
}

#ifndef FEATURE_L4
/*===========================================================================

FUNCTION DIAG_PANIC_MODE_HANDLER

DESCRIPTION
  This procedure handles a diag_wait() call during panic mode, ignoring 
  most signals and never giving up task context.  
  
  This is a loop that maintains minimal system functionality required
  to flush DIAG's buffers to the external device prior to shutdown.  Since 
  DIAG is highest priority (or we are running in ISR context), no other tasks 
  will be able to run.  Interrupts are disabled, as is the DOG task.  Panic 
  mode must kick the hardware watchdog and service ISRs (all non-essential ISRs
  were removed when panic mode started).  While doing so, DIAG drains all of
  its buffers, skipping uncommitted buffers, to the comm layer until empty.  
  Once empty, panic mode flushes the comm layer.

  The loop decrements a countdown upon each iteration.  If the countdown 
  reaches 0, panic mode ends and the system is halted (jump to downloader)
  as usual.  This is done to hand the case of unsuccessful panic mode 
  operation due to unknown system state.
    
DEPENDENCIES
  Depends on DIAG being the highest priority task next to the services needed
  to send remaining data.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
static rex_sigs_type
diag_panic_mode_handler (rex_sigs_type mask)
{
  rex_sigs_type sigs = 0;
  static diagbuf_status_enum_type status = DIAGBUF_NOT_READY_S;

  /* If it is anything besides the TX or RX sig, return. */
  mask &= ~(DIAG_TX_SIG | DIAG_RX_SIG);

  while (diag_panic_countdown--)
  {
    /* The software watchdog will no longer run.  We must kick the h/w dog. */
    dog_kick ();

    /* Since interrupts are disabled, we must service interrupts explicitly. */
    tramp_isr ();

    /* We only care about TX and RPT timer */
    sigs = rex_get_sigs (&diag_tcb);

    /* Prevent multiple flush calls. */
    if (status != DIAGBUF_EMPTY_S)
    {
      if (sigs & DIAG_TX_SIG)
      {
        (void) rex_clr_sigs (&diag_tcb, DIAG_TX_SIG);

    status = diagbuf_drain (0);

    if (status == DIAGBUF_EMPTY_S)
    {
      /* Flush of diag buffers is now complete.  Now just flush SIO 
         and kill the target. */
      diagcomm_flush_tx (diag_panic_end);
    }
      }
      else if (sigs & mask)
      {
    break;
      }
    }
  }             /* endless loop */

  /* Countdown expired.  Go ahead and kill target. */
  if (diag_panic_countdown <= 0)
  {
    diag_panic_end ();
  }

  return sigs;
}
#endif /* !defined(FEATURE_L4)*/

#endif /* FEATURE_DIAG_NON_STREAMING */

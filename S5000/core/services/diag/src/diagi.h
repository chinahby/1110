#ifndef DIAGI_H
#define DIAGI_H
/*==========================================================================

                 Diagnostic Subsystem Internal Header File

Description
  Shared declarations and prototypes internal to the Diagnostic subsystem.

Copyright (c) 1990-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagi.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/06   as      Added win mobile featurization support.
03/28/06   pc      Changed DIAG_TX_APP_SIG from 0x00800000 to 0x00080000.
10/19/05   as      Modified diag_searchlist_type to make it 4-byte aligned.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/16/05   as      New signal for DM to communicate completion.
05/17/05   as      Added new signal for Application processor data.
10/24/01   jal     New signal for SIO to indicate the UART is flushed.
                   New signal for SIO to indicate port closure is complete.
08/20/01   jal     Supported more Diag packets.  Added NV cache typedef,
                   prototypes for diag_set_sp_state(), downloader status.
06/27/01   lad     Various non-functional changes to facilitate update of
                   log services.
04/06/01   lad     Added definitions of DIAG_TASK_xxx sigs to decouple from 
                   task.h.
                   ASYNC con<F2st definitions moved to diagcomm.h?
                   Externalized msg_nice[] and log_nice[] for streaming config 
                   support.
                   Moved prototype of diag_kick_watchdog() to diagtarget.h.
                   Added prototype of diagpkt_process_request() (moved from 
                   diagpkt.h).
                   Updated prototype for diag_do_escaping().
                   Removed prototype for diag_do_unescaping().
                   Removed references to diagpkt_refresh_cache().
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"

#include "diag.h"
#include "diagbuf.h"
#include "log.h"

#if defined(FEATURE_DIAG_NV)
#include "nv.h"
#endif
#include "rex.h"

#include <string.h>

/* -------------------------------------------------------------------------
** Diag Internal features
** ------------------------------------------------------------------------- */
/* Group all standalone features */
#if (defined(FEATURE_STANDALONE) || defined(FEATURE_STANDALONE_APPS) || \
     defined(FEATURE_STANDALONE_MODEM)) 
  #define DIAG_STANDALONE
#endif

/* Multi processor Diag */
#if defined(FEATURE_MULTIPROCESSOR) && !(defined(DIAG_STANDALONE))
#error code not present
#endif

#ifdef FEATURE_USB_ON_APPS_PROC
  /* Multi processor Apps proc is master proc */
  #if (defined(DIAG_MP) && defined(IMAGE_APPS_PROC))
    #define DIAG_REQ_FWD   /* Fowards requests to the slave proc */
    #define DIAG_APPS_PROC
  #endif
  /* Single processor or MP Apps proc (master) */
  #if !defined(DIAG_MP) || (defined(DIAG_MP) && defined(IMAGE_APPS_PROC))
    #define DIAG_NO_DSM_CHAINING
    #define DIAG_FRAMING 
  #endif
  /* Single processor or MP Modem proc (slave) */
  #if !defined(DIAG_MP) || (defined(DIAG_MP) && defined(IMAGE_MODEM_PROC))
    #define DIAG_RSP_SEND /* sends response */
  #endif
#else
  /* Multi processor Modem proc is master proc */
  #if (defined(DIAG_MP) && defined(IMAGE_MODEM_PROC))
    #define DIAG_REQ_FWD   /* Fowards requests to the slave proc */
  #endif
  /* Multi processor Apps proc is slave proc */
  #if (defined(DIAG_MP) && defined(IMAGE_APPS_PROC))
    #define DIAG_APPS_PROC
  #endif
  /* Single processor or MP Modem proc (master) */
  #if !defined(DIAG_MP) || (defined(DIAG_MP) && defined(IMAGE_MODEM_PROC))
    #define DIAG_NO_DSM_CHAINING
    #define DIAG_FRAMING 
  #endif
  /* Single processor or MP App proc (slave) */
  #if !defined(DIAG_MP) || (defined(DIAG_MP) && defined(IMAGE_APPS_PROC))
    #define DIAG_RSP_SEND /* sends response */
  #endif
#endif /* FEATURE_USB_ON_APPS_PROC */

/* Runtime Device Map port selection for diag */
#if (!defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined (FEATURE_RUNTIME_DEVMAP)) || \
      defined(FEATURE_DIAG_MP_MODEM_ONLY) || (!defined (DIAG_MP))
   #define DIAG_RUNTIME_DEVMAP
#endif

/* Error cases */
#if defined (DIAG_MP) && !defined(FEATURE_SMD)
  #error "Error: FEATURE_SND is required for DIAG_MP" 
#endif 
#if defined (FEATURE_DIAG_MP_MODEM_ONLY) &&  \
    !(defined (DIAG_MP) || defined (IMAGE_MODEM_PROC))
  #error "Error: Invalid configuration for DIAG MP"
#endif 


/* -------------------------------------------------------------------------
** Diag Task Signal Definitions
** ------------------------------------------------------------------------- */

#define DIAG_RPT_TIMER_SIG        0x00000001    /* This signal is set when the 
                           diag_rpt_timer expires      */

#define DIAG_RX_SIG               0x00000002    /* This signal is set by the 
                           Serial I/O Services when an 
                           HDLC packet is received.    */

#define DIAG_NV_CMD_SIG           0x00000004    /* This signal is specified when
                           issuing commands to the
                           nv_cmd_q of the Non-volatile
                           Database Task.  When the
                           command is completed, the
                           Non-volatile Database Task
                           sets this signal to notify
                           the Diagnostic Task.        */

#define DIAG_HS_CMD_SIG           0x00000008    /* This signal is specified when
                           issuing commands to the
                           hs_cmd_q of the handset 
                           driver.  When the command
                           is completed, the hs driver
                           sets this signal to notify
                           the Diagnostic Task.        */

#define DIAG_MC_CMD_SIG           0x00000010    /* This signal is specified when
                           issuing commands to the 
                           mc_cmd_q of the mc task.
                           When the command is
                           completed, the mc task sets
                           this signal to notify the
                           Diagnstic Task.             */

#define DIAG_SND_CMD_SIG          0x00000020    /* This signal is specified when
                           issuing commands to the snd 
                           driver. When the command is
                           completed, the snd driver 
                           sets this signal to notify
                           the diag task               */

#define DIAG_TX_SIG               0x00000080    /* This signal is set by the 
                           Serial I/O Services when a
                           packet has been sent.       */

#define DIAG_TOUT_TIMER_SIG       0x00000100    /* This signal is a timer
                           signal for timeout if bad 
                           SPC is rec'd                */

#define DIAG_UI_CMD_SIG           0x00000200    /* Signal that UI_CMD 
                           succeeded                   */

#define DIAG_REFRESH_CACHE_SIG    0x00000400    /* Signal to tell DIAG to refresh
                           any cache it may have.      */

#define DIAG_COMM_OPEN_SIG        0x00000800    /* Signal that SIO is availabe
                           to be opened for use.       */

#define DIAG_COMM_CLOSE_SIG       0x00001000    /* Signal indicating that the 
                           diag should go back to the
                           aux uart                    */
#define DIAG_SIO_SWITCH_UART_SIG  DIAG_COMM_CLOSE_SIG
                         /* Signal indicating that the 
                            diag should go back to the
                            aux uart                    */

/* 0x2000 - 0x8000 are reserved task signals.  
** In MSM builds it is defined in task.h
*/
#ifdef TASK_START_SIG
#define DIAG_TASK_START_SIG TASK_START_SIG
#else
#define DIAG_TASK_START_SIG 0x8000
#endif

#ifdef TASK_STOP_SIG
#define DIAG_TASK_STOP_SIG TASK_STOP_SIG
#else
#define DIAG_TASK_STOP_SIG 0x4000
#endif

#ifdef TASK_OFFLINE_SIG
#define DIAG_TASK_OFFLINE_SIG TASK_OFFLINE_SIG
#else
#define DIAG_TASK_OFFLINE_SIG 0x2000
#endif


#define DIAG_EVENT_TIMER_SIG      0x00010000    /* Signal to finish terminate
                           the current event reporting
                           buffer.                     */

#define DIAG_EVENT_PEND_TIMER_SIG 0x00020000    /* Signal to wake DIAG up to process
                                                   pending events.             */

#define DIAG_BLOCK_SIG            0x00040000    /* Generic block mechanism for
                                                   clients to suspend DIAG task */

#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */

#define DIAG_ASYNC_BLOCK_SIG      0x00400000    /* Signal for adding asynchronous
                           wait capabilities to diag
                           packet handlers. */

#define DIAG_TX_SLAVE_SIG           0x00080000    /* Signal to process Diag traffic
                                                   generated at Apps Processor */

#define DIAG_COMM_OPEN_FWD_SIG       0x01000000    /* Signal that SMD is availabe
                                                   to be opened for use.       */

#define DIAG_DO_NOT_USE_DM_SIG    0x80000000    /* NAND driver for DM needs signal
                                                   to communicate completion.  */


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
#ifdef FEATURE_DIAG_CACHE
/* Structure to cache NV data in memory */
typedef struct
{
  dword esn;            /* ESN */
#ifdef FEATURE_IS2000
  byte current_nam;     /* current nam index (0-3) */
  dword min1[NV_MAX_NAMS][NV_MAX_MINS]; /* All the MIN1s */
  word min2[NV_MAX_NAMS][NV_MAX_MINS];  /* All the MIN2s */
#endif              /* FEATURE_IS2000 */
}
diag_nv_cache_type;

#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
extern diag_nv_cache_type diagnv;
extern nv_item_type nitem;  /* NV Item buffer for NV interface */
#endif

#endif /* FEATURE_DIAG_CACHE */

#ifdef FEATURE_DIAG_SPC_TIMEOUT
extern uint16 diag_spc_tout;
#define MAX_DIAG_SPC_TIMEOUT 2560
#endif 

/* -------------------------------------------------------------------------
** Externalized data members
** ------------------------------------------------------------------------- */

/* Masks to indicate the operating state. */
boolean diag_set_security_state (diag_security_type security);
boolean diag_set_sp_state (diag_sp_type sp);

/* Timers.
*/
extern rex_timer_type diag_rpt_timer;   /* Watchdog report timer */

/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  Tell the sleep task that we are in a state where we can sleep.

SIDE EFFECTS
  The sleep task may take us up on our offer.

===========================================================================*/
void diag_allow_sleep (void);

/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  Tell the sleep task that we are NOT in a state where we can sleep.

===========================================================================*/
void diag_forbid_sleep (void);

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

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
rex_sigs_type diag_wait (rex_sigs_type mask);

#if defined(FEATURE_DIAG_NV)
/*===========================================================================

FUNCTION DIAG_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
===========================================================================*/
nv_stat_enum_type diag_nv_read (nv_items_enum_type item, 
                                PACKED void *buf_ptr);
#endif

/* Connection state */
#define DIAG_CX_NONE_S (0x00)
#define DIAG_CX_COMM_S (0x01)
#define DIAG_CX_CMD_S  (0x02)
#define DIAG_CX_LOG_LISTEN_S (0x04)
#define DIAG_CX_EVENT_LISTEN_S (0x08)
#define DIAG_CX_FLUSH_S (0x10) /* Used for buffer flush */
#define DIAG_CX_ANY_S (0xFFFFFFFF)

#define DIAG_SET_MASK(xx_field, xx_mask) ((xx_field) |= (xx_mask))
#define DIAG_CLR_MASK(xx_field, xx_mask) ((xx_field) &= ~(xx_mask))
#define DIAG_CHECK_MASK(xx_field, xx_mask) \
  (((xx_field) & xx_mask) ? TRUE : FALSE)

extern uint32 diag_cx_state;

/* Flag used for software flow control. */
extern boolean diagbuf_OKTX;
extern boolean diag_panic_mode;

/* These are shared in multiple diagpkt_* files.  De-couple later. */
int diagpkt_rxc_sc_good_frames (void);
int diagpkt_rxc_pc_good_frames (void);

/*===========================================================================

FUNCTION DIAG_CHECK_PASSWORD

DESCRIPTION
  This function checks the given password code, and returns TRUE if it matches.
  
  If 'code_ptr' is NULL, TRUE is returned is no code has yet been written.

===========================================================================*/
boolean diag_check_password (void *code_ptr);

typedef enum
{
  DIAG_SLEEP_DIAG = 0,
  DIAG_SLEEP_EVENT,
  DIAG_SLEEP_LOG,
  DIAG_SLEEP_MSG,
  DIAG_SLEEP_CNT
}
diag_sleep_vote_enum_type;

/*===========================================================================

FUNCTION DIAG_SLEEP_VOTE

DESCRIPTION
  This function is called to vote to allow or disallow sleep.

===========================================================================*/
void diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote);

/*===========================================================================

  Diagnostics Binary Search List (Array Implementation)

  This list implementation is optimized for efficient searches on a generic
  list.
  
  The list is implemented as an array of nodes, sorted by key and a pointer
  specified by the caller.
  
  This list has capabilities to act on duplicate entries.

===========================================================================*/

typedef struct
{
  unsigned int key;
  void *goo;
}
diag_searchlist_node_type;

typedef struct
{
  boolean initialized;          /* Must be initialized to 
                                   FALSE when allocated. */
  byte rsvd[3];                 /* Added padding to make it 4-byte aligned */
  rex_crit_sect_type crit_sect; /* May also be used by caller */
  uint32 seq_num;               /* Incremented during each search. */
  int count;
  int size;
  diag_searchlist_node_type *r; /* Array allocated w/ 'size' elements */
}
diag_searchlist_type;

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_NOTIFY

DESCRIPTION
  This function type is passed to diag_searchlist_search_all() and is 
  called with a pointer to the given payload of that search and a pointer
  to the matched node for every node that matches 'key'.
  
===========================================================================*/
typedef void (*diag_searchlist_notify) (uint32 seq_num, void *payload, 
                                        diag_searchlist_node_type *node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_MATCH

DESCRIPTION
  This function type is passed to diag_searchlist_delete() and is 
  called with a pointer to the given "match node" and the node found in
  the list.  This is used for a delete operation.  This function is called
  for every match of 'key' until this function returns TRUE, indicating a
  match.

RETURN VALUE
  TRUE is the nodes match.
  FALSE if the nodes do not match.
  
===========================================================================*/
typedef boolean (*diag_searchlist_match) (diag_searchlist_node_type *match_node,
                                          diag_searchlist_node_type *list_node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_ADD

DESCRIPTION
  This function adds the specified node, passed by value, to the given list.
  It allocates the list using the diagnostics heap, increasing the size
  of the allocated list when the current allocation is exhausted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if the add operation was successful.
  FALSE if the not successful, indicating insufficient resources in the heap.
  
===========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_DELETE

DESCRIPTION
  This function deletes one node from the specified list if it matches
  the specified node.  Every node in the list that matches 'key' will be
  dispatched to 'match_fp' until 'match_fp' returns TRUE.
  
  If a match is found, the node is removed from the list.
  
  This operation maintains sorted ordering of the list.
  
  This routine may make the list smaller.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found and the node was removed.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list, 
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_SEARCH_ALL

DESCRIPTION
  This function searches the specified list for all nodes that match 'key'.
  
  For every match, 'notify' is called with a pointer to the node and the
  specified 'payload' (unmodified).

  This operation does not alter the contents of the list.
  
  If 'notify' is NULL, the routine only checks for the existence of at 
  least 1 matching node and returns status.
  
ASSUMPTIONS
  This operation assumes the list is sorted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_search_all (unsigned int key, 
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify);

/* The following is used by the packet service to store rsp packets. */
typedef struct
{
  unsigned int pattern;     /* Pattern to check validity of committed pointers. */
  unsigned int size;        /* Size of usable buffer (diagpkt_q_type->pkt) */
  unsigned int length;      /* Size of packet */

  byte pkt[1024];               /* Sized by 'length' field. */
}
diagpkt_rsp_type;

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST 

DESCRIPTION
  This function processes an inbound request packet.
  
  If 'rsp_func' == NULL, the response is sent to the comm layer.
  If 'rsp_func' != NULL, the response packet will be dispatched to the 
  function in lieu of the comm layer, along with 'param'.
  
RETURN VALUE
  None.
  
===========================================================================*/
void diagpkt_process_request (void *req_pkt, uint16 pkt_len, 
                              diag_cmd_rsp rsp_func, void *param);

/*===========================================================================

FUNCTION EVENT_INIT

DESCRIPTION
  Initialize event buffers.

===========================================================================*/
void event_init (void);

/*=========================================================================== 

FUNCTION EVENT_SERVICE_PENDING

DESCRIPTION
   This routine services the pending events in DIAG context and places 
   them on the event Q.

===========================================================================*/
void event_service_pending (void);

/*=========================================================================== 

FUNCTION EVENT_STALE

DESCRIPTION
  Terminates event reporting buffer if space is available for a new buffer.
  If not, we are dropping events and will retry later.

SIDE EFFECTS
  This function signals the diag task and may cause a context switch.  It 
  is intended to be called from DIAG context.

===========================================================================*/
void event_stale (void);

/*=========================================================================== 

FUNCTION EVENT_DRAIN

DESCRIPTION
  Drains event reporting packet (already buffered) to the outbound diagcomm 
  data stream.  Do escaping, all that jazz.  
  
  This is basically a specialized version of diagbuf_drain().  
  
  If 'mask' is foudn in DIAG's task signals, the routine will exit promptly.
  
RETURN VALUE
  TRUE if event_drain is complete, FALSE if in progress.

SIDE EFFECTS
  Changes the needControlFlag that diagbuf_drain() uses, if neccessary.
  
===========================================================================*/
boolean event_drain (rex_sigs_type mask);

/*===========================================================================

FUNCTION EVENT_FLUSH

DESCRIPTION
  Triggers flush operation in the event service.  

===========================================================================*/
void event_flush (void);

/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize logging services.  All logs are configured to 'disabled' state.
  All entries in the "log on demand" table are erased.
  
===========================================================================*/
void log_init (void);

/*===========================================================================

FUNCTION LOG_PROCESS_EXT_LOGMASK

DESCRIPTION
  This function sets the logmask from the logmask bit field specified in the
  diag_ext_logmask_req_type packet.

  This command effects what type of entries are logged.
  
===========================================================================*/
void log_process_ext_logmask (PACKED uint8 * logmask_ptr,
                  unsigned int num_bits);

/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was 
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void log_flush (void);

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void log_panic (void);

/*===========================================================================

FUNCTION MSG_SET_LIMIT

DESCRIPTION
  Set the minimum message level.

===========================================================================*/
void msg_set_limit (uint8 msg_level_limit);

#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE
/*----------------------------------------------------------------------------
  Declaration of the 8 bytes in the check block which is the security password
----------------------------------------------------------------------------*/
//TODO: This is in diagtarget.h - get rid of it here.
extern byte const chk_security_code_ptr[];
#endif

void msg_set_all_rt_masks (uint32 mask);

#endif /* DIAGI_H */


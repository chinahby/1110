/* =======================================================================
                              zrex2rex.c
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/src/zrex2rex.c#17 $
$DateTime: 2010/09/03 07:37:49 $
$Change: 1426732 $

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "zrex.h"
#include "qw.h"
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif
#include "assert.h"
#include "qtvsystem.h"
#include "qtv_msg.h"
#ifndef FEATURE_WINCE
#include "OEMFeatures.h"

#ifdef FEATURE_BREW_3_0
#include "OEMHeap.h"
#include "AEE_OEMHeap.h"
#endif
#endif /* NOT FEATURE_WINCE */

#ifdef FEATURE_QTV_MEMORY_DEBUG
#include <stdio.h>
#endif /* FEATURE_QTV_MEMORY_DEBUG */

#ifdef TIMETEST
#include "timetest.h"
#include "qtvsystem.h"
#endif /* TIMETEST */

#ifdef FEATURE_QTV_DRM_DCF
  #include "ixipc.h"
  #include "err.h"
  #include "IxErrno.h"
  #include "ipcmacros.h"
  #include "ipcerror.h"
#endif

#define MAX_BITS_IN_SIGNAL    32
#define MAX_SYNC_EVENTS       32

#define MAX_ZTCBS_FOR_REX_THREADS  2
typedef struct {
   rex_tcb_type         *tcb_ptr;
   q_type                event_q;
   void                 *g_event_ptr;
   zrex_event_sink_type *event_sink_ptr;
} zrex_generic_event_type;


typedef struct {
   rex_tcb_type         *tcb_ptr;
   void                 *stack_ptr;
   zrex_sync_event_type *event_ptr;
   rex_timer_type        sync_event_timer;
   boolean               timer_on;
   unsigned long         sync_event_signal;
} zrex_tcb_type;


/* ======================================================================
FUNCTION:
  zrex_remove_tcb_from_event_queue

DESCRIPTION:
  Remove tcb from event queue

INPUT/OUPUT PARAMETERS:
  event_ptr
  tcb_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
static void zrex_remove_tcb_from_event_queue(
   zrex_sync_event_type *event_ptr,
   rex_tcb_type *tcb_ptr
);

/* This statically-allocated zREX TCB is used when a REX thread (not a zREX
 * thread) calls zrex_* functions.  Because it is a REX thread, it does not
 * have the resources (variables) to support zREX functionality.  When this
 * happens, we associate this statically allocated zREX TCB to the REX task
 * such that it can call the zrex_* functions.  A pointer to the statically-
 * allocated TCB is put in the ztcbs array.  Currently, we only support one
 * REX thread to access zrex_* functions.  It happens when the UI task runs
 * the Packet Video API functions.  For now, the REX thread is assumed to
 * be permanent and exists as long as the phone is on.
 */
static zrex_tcb_type   ztcbs_for_rex_threads = { 0 };
static unsigned long   num_ztcbs_for_rex_threads = 0;

static zrex_tcb_type  *ztcbs[MAX_PV_THREAD] = { NULL };
static unsigned long   num_ztcbs = 0;        // Number of zREX tcbs created
static zrex_generic_event_type generic_event_table[MAX_PV_THREAD];
#ifdef FEATURE_TEST_SYNC_EVENT
#error code not present
#endif
#ifdef FEATURE_TEST_GEM
#error code not present
#endif

/* Bit array that indicates the bits that have been associated to a sync event
 * and those that are free to be associated to a sync event.
 */
static rex_sigs_type sync_signal_bits = 0;
static unsigned long   num_sync_events = 0;  // Number of sync events created
static boolean bInitialized = FALSE;
static rex_crit_sect_type zrex_em_crit_sect;

/************************
 *
 * Functions in zrexnet.c
 *
 ************************/
/* ======================================================================
FUNCTION:
  zrex_bsd_init

DESCRIPTION:
  Wrapper for bsd_init()

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns 0 on success

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_bsd_init(void)
{
   int retVal = 0;
   default_sigs_type default_sigs;

   default_sigs.select = SELECT_TIMER_SIG;
   default_sigs.socket = SOCKET_CB_SIG;
   default_sigs.net = NET_CB_SIG;
   default_sigs.iface_ioctl = IFACE_IOCTL_SIG;
   default_sigs.dns = DNS_SIG;
   default_sigs.abort = NET_ABORT_SIG;
   default_sigs.tcp_close_timeout = TCP_CLOSE_TIMEOUT_SIG;
#ifndef FEATURE_WINCE
   retVal = bsd_init(&default_sigs, 0, NULL, 0, 0);
#else
#error code not present
#endif

   if (retVal < 0)
   {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "bsd_init() failed, ret=%d", retVal);
     return retVal;
   }

   return retVal;
}
/* ======================================================================
FUNCTION:
  zrex_bsd_shutdown

DESCRIPTION:
  Wrapper for bsd_shutdown()

INPUT/OUPUT PARAMETERS:
  ptr

RETURN VALUE:
  Returns an int

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_bsd_shutdown(void *ptr)
{
#ifndef FEATURE_WINCE
   if (bsd_shutdown(ptr) == 0)
#else
#error code not present
#endif
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"zrex_bsd_shutdown successful");
      return 0;
   }
   else
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"zrex_bsd_shutdown failed");
      return -1;
   }
}


/* ======================================================================
FUNCTION:
  zrex_bind

DESCRIPTION:
   Associates an address with the socket

INPUT/OUPUT PARAMETERS:
  s - socket with which to associate the name
  name - address to be associated with socket
  namelen - size of the address structure

RETURN VALUE:
  Returns 0 on success

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_bind(
   int              s,
      /* Socket with which to associated the name
      */
   struct sockaddr *name,
      /* Address to associated with socket
      */
   int              namelen
      /* Size of the address structure
      */
)
{
   return bind( s, name, namelen );
} /* zrex_bind() */


/* ======================================================================
FUNCTION:
  zrex_close

DESCRIPTION:
  Closes socket

INPUT/OUPUT PARAMETERS:
  s - socket to close

RETURN VALUE:
  Returns 0 on success

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_close(
   int s
      /* Socket to close
      */
)
{
   return close( s );
} /* zrex_close() */


/* ======================================================================
FUNCTION:
  zrex_connect

DESCRIPTION:
   Creates a connection to a specified destination

INPUT/OUPUT PARAMETERS:
  s - socket to use
  peername - where you want to connect
  peernamelen - size of the address structure

RETURN VALUE:
  Returns 0 on success

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_connect(
   int              s,
      /* Socket to use
      */
   struct sockaddr *peername,
      /* Where you want to connect
      */
   int              peernamelen
      /* Size of the address structure
      */
)
{
   return connect(s, peername, peernamelen);
}


/* ======================================================================
FUNCTION:
  zrex_connect

DESCRIPTION:
   Creates a connection to a specified destination

INPUT/OUPUT PARAMETERS:
  s - socket to use
  peername - where you want to connect
  peernamelen - size of the address structure

RETURN VALUE:
  Returns 0 on success

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_read(
   int   s,
      /* Socket
      */
   void *buf,
      /* Pointer to destination buffer
      */
   unsigned int len
      /* Number of bytes to read
      */
)
{
   return read(s, buf, len);
} /* zrex_read() */


/* ======================================================================
FUNCTION:
  zrex_recv

DESCRIPTION:
  Receive data on the specified TCP socket

INPUT/OUPUT PARAMETERS:
  s - socket
  buf - pointer to destination buffer
  len - number of bytes to receive
  flags - associated flags

RETURN VALUE:
  Number of bytes read

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_recv(
   int   s,
      /* Socket
      */
   void *buf,
      /* Pointer to destination buffer
      */
   int   len,
      /* Number of bytes to receive
      */
   int   flags
      /* Associated flags
      */
)
{
   return recv( s, buf, len, flags );
}


/* ======================================================================
FUNCTION:
  zrex_recvfrom

DESCRIPTION:
  Receive data on the specified UDP socket

INPUT/OUPUT PARAMETERS:
  s - socket
  buf - pointer to destination buffer
  len - number of bytes to receive
  flags - associated flags
  from - address of peer
  fromlen - size of address struct

RETURN VALUE:
  Number of bytes read

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_recvfrom(
   int              s,
      /* Socket
      */
   void            *buf,
      /* Pointer to destination buffer
      */
   int              len,
      /* Number of bytes to receive
      */
   int              flags,
      /* Associated flags
      */
   struct sockaddr *from,
      /* Address of peer
      */
   int             *fromlen
      /* Size of address structure
      */
)
{
   return recvfrom( s, buf, len, flags, from, fromlen );
} /* zrex_recvfrom() */


/* ======================================================================
FUNCTION:
  zrex_select

DESCRIPTION:
  Queries descriptors to see which are signalled.

INPUT/OUPUT PARAMETERS:
  nfds - number of descriptors
  radfds - descriptors expecting a read ready state
  writefds - descriptors expecting a write ready state
  exceptfds - descriptors on which exceptions may occur
  timeout

RETURN VALUE:
  Returns an int

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_select(
   int nfds,
      /* Number of descriptors
      */
   fd_set *readfds,
      /* Descriptors expecting a read ready state
      */
   fd_set *writefds,
      /* Descriptors expecting a write ready state
      */
   fd_set *exceptfds,
      /* Descriptors on which exceptions may occurr
      */
   struct timeval *timeout
      /* Timeout
      */
)
{
   return select( nfds, readfds, writefds, exceptfds, timeout );
} /* zrex_select() */


/* ======================================================================
FUNCTION:
  zrex_send

DESCRIPTION:
  Sends the data on the TCP socket

INPUT/OUPUT PARAMETERS:
   s
   buf
   len
   flags

RETURN VALUE:
  Number of bytes sent

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_send(
   int   s,
      /* Socket
      */
   void *buf,
      /* Source buffer
      */
   int   len,
      /* Number of bytes to write
      */
   int   flags
      /* Associated flags
      */
)
{
   return send( s, buf, len, flags );
} /* zrex_send() */


/* ======================================================================
FUNCTION:
  zrex_sendto

DESCRIPTION:
  Sends the data on the UDP socket

INPUT/OUPUT PARAMETERS:
  s
  buf
  len
  flags
  to
  tolen

RETURN VALUE:
  Number of bytes sent

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_sendto(
   int              s,
      /* Socket
      */
   void            *buf,
      /* Source buffer
      */
   int              len,
      /* Number of bytes to write
      */
   int              flags,
      /* Associated flags
      */
   struct sockaddr *to,
      /* Address where data is to be sent
      */
   int              tolen
      /* Size of the address structure
      */
)
{
   return sendto( s, buf, len, flags, to, tolen );
} /* zrex_sendto() */


/* ======================================================================
FUNCTION:
  zrex_socket

DESCRIPTION:
  Creates a socket

INPUT/OUPUT PARAMETERS:
  af
  type
  protocol

RETURN VALUE:
  Returns the socket, -1 otherwise

SIDE EFFECTS:
  None.
========================================================================== */
int zrex_socket(
   int af,
      /* Protocol family
      */
   int type,
      /* Protocol type
      */
   int protocol
      /* Which protocol
      */
)
{
   return socket( af, type, protocol );
} /* zrex_socket() */


/* ======================================================================
FUNCTION:
void sockaddr::zrex_net_abort

DESCRIPTION:
  Abort

INPUT/OUPUT PARAMETERS:
  handle

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_net_abort(zrex_handle_type handle)
{
  (void)rex_set_sigs(handle, NET_ABORT_SIG);
}


/* ======================================================================
FUNCTION:
void sockaddr::zrex_clear_net_abort

DESCRIPTION:
  Clear net abort

INPUT/OUPUT PARAMETERS:
  handle

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_clear_net_abort(zrex_handle_type handle)
{
  (void)rex_clr_sigs(handle, NET_ABORT_SIG);
}


/***************************
 *
 * Functions in zrexthread.c
 *
 ***************************/
/* ======================================================================
FUNCTION:
  zrex_create_thread

DESCRIPTION:
  Create a thread

INPUT/OUPUT PARAMETERS:
  stack_size
  priority
  thread_ptr
  param
  thread_name
  suspended

RETURN VALUE:
  Returns the handle to thread on success, else NULL

SIDE EFFECTS:
  None.
========================================================================== */
zrex_handle_type
   /* Handle to thread on success, NULL otherwise
   */
zrex_create_thread(
   zrex_stack_size_type stack_size,
      /* Requested stack size in words
      */
   zrex_priority_type   priority,
      /* Requested priority
      */
   zrex_task_func_type  thread_ptr,
      /* Thread function
      */
   unsigned long        param,
      /* Parameter to pass to thread function when it is executed
      */
   char                 *thread_name,
      /* ASCII string name for the thread, can be NULL
      */
   boolean              suspended
      /* Indicated whether or not the thread should be started
      ** in the suspended state.  When TRUE, the thread will be
      ** created but only scheduable after a call to zrex_resume_thread,
      ** FALSE indicates the thread is immediately schedulable after
      ** creation
      */
)
{
   rex_tcb_type  *tcb_ptr;
   zrex_tcb_type *ztcb_ptr;
   void          *stack_ptr;
   void          *adj_stack_ptr;
   unsigned long  i;

   if (num_ztcbs == MAX_PV_THREAD)
   {
      return NULL;
   }

   /* Allocate a zREX TCB */
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
   ztcb_ptr = (zrex_tcb_type *)
              QTV_Malloc((sizeof( zrex_tcb_type ) + 3) / 4 * 4);
#endif
   if (ztcb_ptr == NULL)
   {
      return NULL;
   }
   memset( ztcb_ptr, 0, ((sizeof( zrex_tcb_type ) + 3) / 4 * 4) );

   /* Allocate a TCB and a stack */
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
   tcb_ptr = (rex_tcb_type *)
             QTV_Malloc((sizeof( rex_tcb_type ) + 3) / 4 * 4);
#endif

   if ( tcb_ptr == NULL )
   {
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
   QTV_Free(ztcb_ptr);
#endif
      return NULL;
   }
   memset( tcb_ptr, 0, ((sizeof( rex_tcb_type ) + 3) / 4 * 4) );
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
  stack_ptr = QTV_Malloc(stack_size * 2);
#endif

   if ( stack_ptr == NULL )
   {
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
      QTV_Free(ztcb_ptr);
      QTV_Free(tcb_ptr);
#endif

      return NULL;
   }
   memset( stack_ptr, 0, stack_size * 2 );

   adj_stack_ptr = ((unsigned long)stack_ptr%4)?
      (void*)(((unsigned long)stack_ptr+(sizeof(long)) - (unsigned long)stack_ptr%4)):
      stack_ptr;

   /* Put the ZREX TCB in an empty slot in the table of ZREX TCB pointers.  Associate
    * the zREX TCB with REX TCB and do other initilization of the zREX TCB.
    */
   INTLOCK();
   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      if (ztcbs[i] == NULL)
      {
         ztcbs[i] = ztcb_ptr;
         break;
      }
   }
   INTFREE();
   ztcb_ptr->tcb_ptr = tcb_ptr;
   ztcb_ptr->stack_ptr = stack_ptr;
   rex_def_timer(&ztcb_ptr->sync_event_timer, tcb_ptr, SYNC_EVENT_TIMER_SIG);
   ztcb_ptr->timer_on = FALSE;

   num_ztcbs++;

#ifdef TIMETEST
   {
     if ((priority - QTV_BASE_PRIORITY) + TIMETEST_MP4_PV_FIRST_TASK_ID <=
         TIMETEST_MP4_PV_LAST_TASK_ID)
     {
        tcb_ptr->leds =
          (priority - QTV_BASE_PRIORITY) + TIMETEST_MP4_PV_FIRST_TASK_ID;
     }
   }
#endif /* TIMETEST */

   rex_def_task_ext(tcb_ptr,
                    (unsigned char *)adj_stack_ptr,
                    stack_size * 2,
                    priority,
                    thread_ptr,
                    param,
                    thread_name,
                    suspended);

   return tcb_ptr;
} /* zrex_create_thread() */

/* ======================================================================
FUNCTION:
  zrex_set_thread_priority

DESCRIPTION:
  Set the thread priority

INPUT/OUPUT PARAMETERS:
  handle
  priority

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_set_thread_priority(zrex_handle_type handle, zrex_priority_type priority)
{
   if (handle == NULL)
   {
       handle = rex_self();
   }
   (void)rex_task_pri(handle, priority);
}

/* ======================================================================
FUNCTION:
  zrex_get_thread_priority

DESCRIPTION:
  Get the thread priority

INPUT/OUPUT PARAMETERS:
  handle

RETURN VALUE:
  Returns the priority type

SIDE EFFECTS:
  None.
========================================================================== */
zrex_priority_type zrex_get_thread_priority(zrex_handle_type handle)
{
   if (handle == NULL)
   {
       handle = rex_self();
   }
   return ((rex_tcb_type *)handle)->pri;
}

/* ======================================================================
FUNCTION:
  zrex_kill_thread

DESCRIPTION:
  Similar to rex_kill_task. Frees up space allocated for the
  tcb and for the stack.  Removes the thread from scheduling.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_kill_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
)
{
   unsigned long i = 0, j = 0;

   /* Verify that zrex2rex has been initialized
    */
   INTLOCK();
   if( !bInitialized ) {
     zrex2rex_init();
   }
   INTFREE();

   /* Find the zREX TCB associated with the REX TCB of the task to be killed
    */
   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      if (ztcbs[i] != NULL && ztcbs[i]->tcb_ptr == handle)
      {
         break;
      }
   }

   ASSERT(i != MAX_PV_THREAD);

   /* below if condition is added to clear lint warnings */
   if (i < MAX_PV_THREAD)
   {
      if (ztcbs[i]->event_ptr != NULL)
      {
         /* Remove the task from any event queue */
         INTLOCK();
         zrex_remove_tcb_from_event_queue(ztcbs[i]->event_ptr, handle);
         INTFREE();
      }

      /* Remove the TCB from the generic event table and release posted events */
      for (j = 0; j < MAX_PV_THREAD; j++)
      {
         if (generic_event_table[j].tcb_ptr == ztcbs[i]->tcb_ptr)
         {
            break;
         }
      }
   }
   rex_enter_crit_sect(&zrex_em_crit_sect);
   if (j != MAX_PV_THREAD)
   {
      zrex_event_type *e_ptr;

      INTLOCK();
      generic_event_table[j].tcb_ptr = NULL;
      generic_event_table[j].event_sink_ptr = NULL;
      INTFREE();
      while ((e_ptr = q_get(&generic_event_table[j].event_q)) != NULL)
      {
         zrex_event_release(e_ptr);
      }
      if (generic_event_table[j].g_event_ptr != NULL)
      {
         zrex_event_release(generic_event_table[j].g_event_ptr);
      }
   }

   rex_kill_task(handle);
   rex_leave_crit_sect(&zrex_em_crit_sect);
   /* below if condition is added to clear lint warnings */
   if (i < MAX_PV_THREAD)
   {
#ifdef FEATURE_QTV_IMEM
#error code not present
#else
      QTV_Free(ztcbs[i]->tcb_ptr);
      QTV_Free(ztcbs[i]->stack_ptr);
      QTV_Free(ztcbs[i]);
   }

#endif
   /* Return task's resources */

   INTLOCK();
   /* below if condition is added to clear lint warnings */
   if (i < MAX_PV_THREAD)
   {
      ztcbs[i] = NULL;
      num_ztcbs--;
   }
   INTFREE();

   INTLOCK();
   /* de-initialize zrex2rex */
   if( bInitialized ) {
      zrex2rex_dinit();
   }
   INTFREE();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex_release_event_q

DESCRIPTION:
  Frees up all the events destined/posted to this thread on its event-q.

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_release_event_q(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
)
{
   unsigned long index=0;

   /* Verify that zrex2rex has been initialized
    */
   INTLOCK();
   if( !bInitialized ) {
     zrex2rex_init();
   }
   INTFREE();

   for (index = 0; index < MAX_PV_THREAD; index++)
   {
     if (generic_event_table[index].tcb_ptr == handle)
     {
       break;
     }
   }

   ASSERT(index != MAX_PV_THREAD);

   rex_enter_crit_sect(&zrex_em_crit_sect);
   if (index != MAX_PV_THREAD)
   {
      zrex_event_type *e_ptr;

      while ((e_ptr = q_get(&generic_event_table[index].event_q)) != NULL)
      {
         zrex_event_release(e_ptr);
      }
      if (generic_event_table[index].g_event_ptr != NULL)
      {
         zrex_event_release(generic_event_table[index].g_event_ptr);
      }
   }
   rex_leave_crit_sect(&zrex_em_crit_sect);

   INTLOCK();
   /* de-initialize zrex2rex */
   if( bInitialized ) {
      zrex2rex_dinit();
   }
   INTFREE();
   
   return;
}

/* ======================================================================
FUNCTION:
  zrex_pause_thread

DESCRIPTION:
  Pauses a thread by removing it as a candiate for scheduling.  The
  thread is not killed however and can be resumed by a call to
  zrex_resume_thread.

INPUT/OUPUT PARAMETERS:
  handle

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_pause_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
)
{
   rex_suspend_task(handle);
}


/* ======================================================================
FUNCTION:
  zrex_resume_thread

DESCRIPTION:
  Resumes execution of a previously paused or suspended thread by
  making it schedulable

INPUT/OUPUT PARAMETERS:
  handle

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_resume_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
)
{
   rex_resume_task(handle);
}


/*********************
 *
 * Functions in zrex.c
 *
 *********************/
/* ======================================================================
FUNCTION:
  zrex_sleep

DESCRIPTION:
  Blocks the current thread for a specified time. The thread becomes
  schedulable after the specified time has elapsed.

INPUT/OUPUT PARAMETERS:
  ms

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_sleep(
   unsigned long ms
)
{
  rex_sleep(ms);
} /* zrex_sleep() */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/***********************
 *
 * Functions in zrexse.c
 *
 ***********************/
/* ======================================================================
FUNCTION:
  zrex_init_sync_event

DESCRIPTION:
  Initializes an event. The caller must allocate space for the event
  by defining a variable of type zrex_event_type. Typically this
  variable will be at file or global scope.

INPUT/OUPUT PARAMETERS:
  event_ptr
  reset_is_manual
  initial_state_is_set

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void
zrex_init_sync_event(
   zrex_sync_event_type *event_ptr,
      /* Pointer to a event structure to initialize
      */
   boolean               reset_is_manual,
      /* Whether or not the event resets itself after a wait. If TRUE,
      ** the user must reset the event with a call to zrex_reset_event,
      ** otherwise the event remains in the signalled state. When FALSE,
      ** the event is reset before a return from zrex_wait_for_sync_event.
      */
   boolean               initial_state_is_set
      /* Whether or not the event is in the signalled state at init
      ** time, when TRUE, the event is immediately in the signalled state
      ** as if the user had immediately called zrex_set_sync_event,
      ** when FALSE, the event is initialized in the non signalled case
      */
)
{
   int i;
   rex_sigs_type mask = 0x00000001;

   INTLOCK();
   if ( event_ptr == NULL ||  num_sync_events == MAX_SYNC_EVENTS)
   {
      ASSERT(0);
      return;
   }

   for (i = 0; i < MAX_BITS_IN_SIGNAL; i++)
   {
      if (sync_signal_bits & mask)
      {
         mask = mask << 1;
      }
      else
      {
         break;
      }
   }
   sync_signal_bits |= mask;
   num_sync_events++;
   INTFREE();

   event_ptr->reset_type = reset_is_manual ?
              (int)ZREX_MANUAL_RESET : (int)ZREX_AUTO_RESET;
   event_ptr->state = initial_state_is_set ?
              (int)ZREX_SYNC_EVENT_IS_SET : (int)ZREX_SYNC_EVENT_IS_RESET;
   event_ptr->num_waiting_tcbs = 0;
   event_ptr->signal_mask = mask;

   return;
}


/* ======================================================================
FUNCTION:
  zrex_release_sync_event

DESCRIPTION:
  This function disassociates a signal bit from a sync event.  It is
  not part of zrexse.c but added here to allow signal bits to be
  reused once a sync event is no longer needed or goes out of
  existence as in the case of dynamically allocated sync events.

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_release_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
)
{
   uint32 i = 0;
   if (event_ptr == NULL)
   {
      ASSERT(0);
      return;
   }
   INTLOCK()
   sync_signal_bits = sync_signal_bits & ~event_ptr->signal_mask;
   if (num_sync_events > 0)
   {
     num_sync_events--;

     /* remove this sync event from all the waiting tcb list*/
     TASKLOCK();
     /* Scan the TCB array  to find the TCBs
     * having current sync event pointer and remove the sync event.
     */
     for(i = 0; i < MAX_PV_THREAD; i++)
     {
       if ( ztcbs[i] && (ztcbs[i]->event_ptr == event_ptr))
       {
         /* reset event pointer */
         ztcbs[i]->event_ptr = NULL;
       }
     }
     TASKFREE();
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "zrex_release_sync_event: Error, num_sync_events is zero");
   }
   INTFREE();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_reset_sync_event

DESCRIPTION:
  Causes the event to put into the non signalled state.  Used when
  a sync event is initialized as manual.

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_reset_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
)
{
   if ( event_ptr == NULL )
   {
      return;
   }

   INTLOCK( );
   event_ptr->state = (int)ZREX_SYNC_EVENT_IS_RESET;
   INTFREE( );

   return;
} /* END zrex_reset_sync_event */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex_release_all_sync_event

DESCRIPTION:
  This will forcebly clear all the pending sync events. This should be called
  only at the end of QTV.

INPUT/OUPUT PARAMETERS:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_release_all_sync_event(void)
{
  if( (sync_signal_bits) || (num_sync_events))
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "All SyncEvents not released(signal_bits=%d, num_events=%d)", sync_signal_bits, num_sync_events);
    sync_signal_bits = 0;
    num_sync_events = 0;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex_remove_tcb_from_event_queue

DESCRIPTION:
  Removes a task's TCB from an event queue.  This function should be called
  from within an INTLOCK()/INTFREE().

INPUT/OUPUT PARAMETERS:
  event_ptr
  tcb_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
static void zrex_remove_tcb_from_event_queue(
   zrex_sync_event_type *event_ptr,
   rex_tcb_type *tcb_ptr
)
{
   unsigned long i;

   /* Scan the TCB array of waiting tasks to find the array index containing
    * the TCB to be removed.
    */
   for (i = 0; i < event_ptr->num_waiting_tcbs; i++)
   {
      if (event_ptr->waiting_tcb_list[i] == tcb_ptr)
      {
         break;
      }
   }

   if (i == event_ptr->num_waiting_tcbs)
   {
      /* TCB not found */
      return;
   }

   /* If the timer is on, clear the task's sync event timer. */
   if (ztcbs[event_ptr->ztcb_index[i]]->timer_on)
   {
      (void)rex_clr_timer(&ztcbs[event_ptr->ztcb_index[i]]->sync_event_timer);
      ztcbs[event_ptr->ztcb_index[i]]->timer_on = FALSE;
   }

   /* Decrement the number of TCBs waiting on the event and if needed,
    * reshuffle the waiting TCB array to minimize linear search.
    */
   if (--event_ptr->num_waiting_tcbs && i != event_ptr->num_waiting_tcbs)
   {
      /* Move last array item to the vacated slot */
      event_ptr->waiting_tcb_list[i] =
         event_ptr->waiting_tcb_list[event_ptr->num_waiting_tcbs];
      event_ptr->ztcb_index[i] =
         event_ptr->ztcb_index[event_ptr->num_waiting_tcbs];
   }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_set_sync_event_internal

DESCRIPTION:
  Scan the queue for this event and signal the waiting task(s).  If the
  event is a manual reset, all waiting tasks are signalled.  If the event is
  an auto reset event, then only one waiting task, the one with then highest
  priority is signalled.

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
static void
zrex_set_sync_event_internal(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
)
{
   unsigned long i;

   /* Scan the queue for this event and signal the waiting task(s).  If the
    * event is a manual reset, all waiting tasks are signalled.  If the event is
    * an auto reset event, then only one waiting task, the one with then highest
    * priority is signalled.
    */
   if( event_ptr == NULL )
     return;

   INTLOCK();
   TASKLOCK();
   if ( event_ptr->reset_type == (int)ZREX_MANUAL_RESET )
   {
      /* Make all tasks waiting on this event schedulable. Remove all tasks from
       * wait list for this event.
       */
      for (i = 0; i < event_ptr->num_waiting_tcbs; i++)
      {
         if (ztcbs[event_ptr->ztcb_index[i]]->timer_on)
         {
            (void)rex_clr_timer(&ztcbs[event_ptr->ztcb_index[i]]->sync_event_timer);
            ztcbs[event_ptr->ztcb_index[i]]->timer_on = FALSE;
         }
         ztcbs[event_ptr->ztcb_index[i]]->sync_event_signal |=
                                                         event_ptr->signal_mask;
         (void)rex_set_sigs(event_ptr->waiting_tcb_list[i], SYNC_EVENT_SIG);
      }
      event_ptr->num_waiting_tcbs = 0;
      event_ptr->state = (int)ZREX_SYNC_EVENT_IS_SET;
   }
   else
   {
      /* Find the task with the highest priority that is waiting on this
       * event and make it schedulable.  Remove this task from the list
       * of waiting tasks.
       */
      if (event_ptr->num_waiting_tcbs > 0)
      {
         rex_tcb_type  *best_task_ptr;
         unsigned long  best_task_idx = 0;

         /* There is at least one task waiting on this event
          */
         best_task_ptr = event_ptr->waiting_tcb_list[0];
         for (i = 1; i < event_ptr->num_waiting_tcbs; i++)
         {
            if ( event_ptr->waiting_tcb_list[i]->pri > best_task_ptr->pri )
            {
               best_task_ptr = event_ptr->waiting_tcb_list[i];
               best_task_idx = i;
            }
         }

         if (ztcbs[event_ptr->ztcb_index[best_task_idx]]->timer_on)
         {
            (void)rex_clr_timer(&ztcbs[event_ptr->ztcb_index[best_task_idx]]->
                                                              sync_event_timer);
            ztcbs[event_ptr->ztcb_index[best_task_idx]]->timer_on = FALSE;
         }

         ztcbs[event_ptr->ztcb_index[best_task_idx]]->sync_event_signal |=
                                                         event_ptr->signal_mask;

         if (--event_ptr->num_waiting_tcbs &&
             best_task_idx != event_ptr->num_waiting_tcbs)
         {
            /* Move last item on list to vacated slot to shortened linear
             * search.
             */
            event_ptr->waiting_tcb_list[best_task_idx] =
               event_ptr->waiting_tcb_list[event_ptr->num_waiting_tcbs];
            event_ptr->ztcb_index[best_task_idx] =
               event_ptr->ztcb_index[event_ptr->num_waiting_tcbs];
         }

         (void)rex_set_sigs(best_task_ptr, SYNC_EVENT_SIG);
         event_ptr->state = (int)ZREX_SYNC_EVENT_IS_SET;
      }
      else
      {
         event_ptr->state = (int)ZREX_SYNC_EVENT_IS_SET;
      }
   }
   TASKFREE();
   INTFREE();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_set_sync_event

DESCRIPTION:
  Causes the event to be put into the signalled state.  Waiting
  thread(s) will become schedulable.

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void
zrex_set_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
)
{
   if ( event_ptr == NULL )
   {
      return;
   }

   /* If the event is already set, then return immediately. */
   if ( event_ptr->state == (int)ZREX_SYNC_EVENT_IS_SET )
   {
      return;
   }

   zrex_set_sync_event_internal( event_ptr );

   return;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_event_header_struct::zrex_wait_for_sync_event

DESCRIPTION:
  Blocks the caller until the associated event is placed in the
  signalled state or the timeout occurrs.

INPUT/OUPUT PARAMETERS:
  event_ptr
  milli_secs

RETURN VALUE:
  ZREX_WAIT_TMO if the timeout occurrs
  ZREX_EVENT_SET if the event is set

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
zrex_sync_event_status_type
zrex_wait_for_sync_event(
   zrex_sync_event_type *event_ptr,
      /* Handle to the event
      */
   unsigned long        milli_secs
      /* Number of milliseconds to wait for the event to be signalled.
      */
)
{
   rex_sigs_type  rex_signals;
   unsigned long  i;
   rex_tcb_type  *tcb_ptr = rex_self();
   int            dog_autokick;
   boolean        non_qtv_tcb_flag = FALSE;
   int            non_qtv_tcb_idx = 0;
   dword          int_sav; /* Interrupt lock status save */

   if ( event_ptr == NULL )
   {
      return ZREX_SYNC_EVENT_TMO;
   }

   /* If the event is set, return immediately. If the event is an auto-reset
    * event, reset it.
    */
   rex_task_lock();
   INTLOCK_SAV(int_sav);
   if ( event_ptr->state == (int)ZREX_SYNC_EVENT_IS_SET )
   {
      if ( event_ptr->reset_type == (int)ZREX_AUTO_RESET )
      {
         event_ptr->state = (int)ZREX_SYNC_EVENT_IS_RESET;
      }
      INTFREE_SAV(int_sav);
      rex_task_free();
      return ZREX_SYNC_EVENT_SET;
   }

   /* Find the associated zREX TCB */
   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      if (ztcbs[i] != NULL && ztcbs[i]->tcb_ptr == tcb_ptr)
      {
         break;
      }
   }
   if (i == MAX_PV_THREAD)
   {
      if (num_ztcbs_for_rex_threads != MAX_ZTCBS_FOR_REX_THREADS)
      {
         /* Find an empty slot in the array of zREX TCB pointers */
         for (i = 0; i < MAX_PV_THREAD; i++)
         {
            if (ztcbs[i] == NULL)
            {
               break;
            }
         }
         ASSERT(i < MAX_PV_THREAD);
         ztcbs_for_rex_threads.tcb_ptr = rex_self();
         rex_def_timer(&ztcbs_for_rex_threads.sync_event_timer,
                       ztcbs_for_rex_threads.tcb_ptr,
                       SYNC_EVENT_TIMER_SIG);
         ztcbs_for_rex_threads.timer_on = FALSE;
	 /* below if condition is added to clear lint warnings */
        if (i < MAX_PV_THREAD)
        {
            ztcbs[i] = &ztcbs_for_rex_threads;
            num_ztcbs_for_rex_threads++;
            num_ztcbs++;
            non_qtv_tcb_flag = TRUE;
            non_qtv_tcb_idx = i;
        }
      }
      else
      {
         ASSERT(0);
      }
   }

   /* This code is to convince lint that we won't access outside the bounds of
    * the ztcbs array.  This code should never be hit due to the ASSERTS in the
    * above code.
    */
   if (i >= MAX_PV_THREAD)
   {
     return ZREX_SYNC_EVENT_TMO;
   }

   if (milli_secs < ZREX_INFINITE)
   {
      (void)rex_set_timer(&ztcbs[i]->sync_event_timer, milli_secs);
      ztcbs[i]->timer_on = TRUE;
   }

   /* Put task in the event queue */
   event_ptr->waiting_tcb_list[event_ptr->num_waiting_tcbs] = tcb_ptr;
   event_ptr->ztcb_index[event_ptr->num_waiting_tcbs] = i;
   event_ptr->num_waiting_tcbs++;

   /* Store the event's address in the zTCB.  This is so that we can remove the
    * the task from the event's "waiting TCB" list when the task gets killed.
    */
   ztcbs[i]->event_ptr = event_ptr;
   rex_task_free();
   while (1)
   {
      /* A task that is subject to the watchdog timer may be executing this
       * code.  Therefore, enable the dog autokick for that task.
       */
      dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);
      rex_signals = rex_wait(SYNC_EVENT_SIG | SYNC_EVENT_TIMER_SIG);
      if (!dog_autokick)
      {
         (void)rex_autodog_disable();
      }

      if (rex_signals & SYNC_EVENT_SIG)
      {
         (void)rex_clr_sigs(rex_self(), SYNC_EVENT_SIG);
         if (ztcbs[i]->sync_event_signal & event_ptr->signal_mask)
         {
            zrex_remove_tcb_from_event_queue(event_ptr, tcb_ptr);
            ztcbs[i]->sync_event_signal &= ~event_ptr->signal_mask;
            if ( event_ptr->reset_type == (int)ZREX_AUTO_RESET )
            {
               event_ptr->state = (int)ZREX_SYNC_EVENT_IS_RESET;
            }
            INTFREE_SAV(int_sav);
            if(non_qtv_tcb_flag)
            {
              memset(&ztcbs_for_rex_threads, 0, sizeof(ztcbs_for_rex_threads));
              ztcbs[non_qtv_tcb_idx] = NULL;
              num_ztcbs_for_rex_threads--;
              num_ztcbs--;
            }
            return ZREX_SYNC_EVENT_SET;
         }
      }
      else
      {
         zrex_remove_tcb_from_event_queue(event_ptr, tcb_ptr);
         (void)rex_clr_sigs(rex_self(), SYNC_EVENT_TIMER_SIG);
         INTFREE_SAV(int_sav);
         if(non_qtv_tcb_flag)
         {
           memset(&ztcbs_for_rex_threads, 0, sizeof(ztcbs_for_rex_threads));
           ztcbs[non_qtv_tcb_idx] = NULL;
           num_ztcbs_for_rex_threads--;
           num_ztcbs--;
         }
         return ZREX_SYNC_EVENT_TMO;
      }
   }
#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/***********************
 *
 * Functions in zrexem.c
 *
 ***********************/
/* ======================================================================
FUNCTION:
  zrex_create_event_ex

DESCRIPTION:
  Creates an event

INPUT/OUPUT PARAMETERS:
  id
  size
  assoc_ptr

RETURN VALUE:
  Returns pointer to the event on success, NULL otherwise

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
zrex_event_type* zrex_create_event_ex(
   zrex_event_id_type     id,
   unsigned long          size,
   zrex_event_assoc_type *assoc_ptr,
   void                  *user_data
)
{
   zrex_event_header_type *event_hdr = NULL;

   event_hdr = (zrex_event_header_type *)QTV_Malloc(size);
   if (event_hdr != NULL)
   {
      event_hdr->id = id;
      event_hdr->size = size;
      event_hdr->assoc_ptr = assoc_ptr;
      event_hdr->isStatic = 0;
      event_hdr->isFreeToDispatch = 1;
      event_hdr->user_data = user_data;
   }
   return (zrex_event_type *) event_hdr;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex_create_static_event_ex

DESCRIPTION:
  Creates a static event. It will not be cleaned after dispatch.
  This can be used to generate an event from an APC. This will avoid heap
  operations and potential heap corruption when one APC is interrupted by
  another APC within the same thread context.

INPUT/OUPUT PARAMETERS:
  id
  size
  assoc_ptr

RETURN VALUE:
  Returns pointer to the event on success, NULL otherwise

SIDE EFFECTS:
  None.
========================================================================== */
zrex_event_type* zrex_create_static_event_ex(
   zrex_event_id_type     id,
   unsigned long          size,
   zrex_event_assoc_type *assoc_ptr,
   void                  *user_data
)
{
   zrex_event_header_type *event_hdr = NULL;

   event_hdr = (zrex_event_header_type *)QTV_Malloc(size);
   if (event_hdr != NULL)
   {
      event_hdr->id = id;
      event_hdr->size = size;
      event_hdr->assoc_ptr = assoc_ptr;
      event_hdr->isStatic = 1;
      event_hdr->isFreeToDispatch = 1;
      event_hdr->user_data = user_data;
   }
   return (zrex_event_type *) event_hdr;
}

/* ======================================================================
FUNCTION:
  zrex_dispatch_event

DESCRIPTION:
  For a thread which has set an event dispatch table, this will
  dispatch the event through the table

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_dispatch_event(
   zrex_event_type *event_ptr
)
{
   zrex_event_header_type *e_ptr = event_ptr;

   e_ptr->event_handler(event_ptr);
   zrex_event_release(event_ptr);
}


/* ======================================================================
FUNCTION:
void sockaddr::zrex_event_release

DESCRIPTION:
  Release an event

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_event_release(
   zrex_event_type *event_ptr     /* Pointer to the event */
)
{
   unsigned long i;
   zrex_event_header_type *e_ptr = event_ptr;

   rex_enter_crit_sect(&zrex_em_crit_sect);
   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      if (generic_event_table[i].g_event_ptr == event_ptr)
      {
         break;
      }
   }
   if(!e_ptr->isStatic)
   {
       QTV_Free(event_ptr);
   }
   else
   {
     e_ptr->isFreeToDispatch = 1;
   }

   if (i != MAX_PV_THREAD)
   {
      generic_event_table[i].g_event_ptr = NULL;
   }
   rex_leave_crit_sect(&zrex_em_crit_sect);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_get_next_event

DESCRIPTION:
  Gets the next event in the event queue for the currently running
  process

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  Pointer to an event, NULL otherwise

SIDE EFFECTS:
  None.
========================================================================== */
zrex_event_type* zrex_get_next_event( void )
{
   rex_tcb_type *tcb_ptr = rex_self();
   zrex_event_type *evt_ptr = NULL;
   unsigned long i = 0;
   int dog_autokick;

   #ifdef FEATURE_QTV_DRM_DCF
     #ifdef FEATURE_NATIVE_KXIPC
       rex_sigs_type p_sigs ;
       uint32 kxipc_req_sigs;
       ixipc_sig_type ixipc_ret_sigtype;
     #endif // #ifdef FEATURE_NATIVE_KXIPC
     rex_sigs_type sigType;
   #endif

   /* Verify that zrex2rex has been initialized
    */
   INTLOCK();
   if( !bInitialized ) {
      zrex2rex_init();
   }
   INTFREE();

   while (1)
   {
     dog_autokick = rex_autodog_enable(rex_self()->dog_report_val);

     #ifdef FEATURE_QTV_DRM_DCF
       #ifndef FEATURE_NATIVE_KXIPC
         sigType = rex_wait(GEM_SIG | QTV_DRM_DCF_IPC_SIG );
       #else
         sigType = GEM_SIG ;
         kxipc_req_sigs = QTV_DRM_DCF_IPC_SIG ;
         ixipc_ret_sigtype = ixipc_sig_wait(&sigType, &kxipc_req_sigs) ;
       #endif /* #ifdef FEATURE_NATIVE_KXIPC */
         #else
           (void)rex_wait(GEM_SIG);
     #endif//#ifdef FEATURE_QTV_DRM_DCF

     #ifdef FEATURE_QTV_DRM_DCF
       #ifndef FEATURE_NATIVE_KXIPC
         if(sigType & QTV_DRM_DCF_IPC_SIG)
       #else
         if(ixipc_ret_sigtype != IXIPC_REX_SIG_TYPE)
       #endif /* #ifdef FEATURE_NATIVE_KXIPC */
	 {
	   INTLOCK();
           /* de-initialize zrex2rex */
           if( bInitialized ) {
              zrex2rex_dinit();
              }
           INTFREE();
	   //IPC signal is set, return so that IPC handler will handle the IPC msg.
	   return NULL;
	 }
	 else
	 #endif//#ifdef FEATURE_QTV_DRM_DCF
	 {
           if (!dog_autokick)
           {
             (void)rex_autodog_disable();
           }

           rex_enter_crit_sect(&zrex_em_crit_sect);
           /* Find the task's event queue in the event table */
           for (i = 0; i < MAX_PV_THREAD; i++)
           {
             if (generic_event_table[i].tcb_ptr == tcb_ptr)
             {
               break;
             }
           }

           ASSERT(i != MAX_PV_THREAD);

           /* When removing an event item from the event queue, it is necessary to
           * store the event item pointer in a variable that will still be valid
           * should the sink task that removes the event item gets killed before
           * the task gets a chance to release the event.
           */
           /* below if condition is added to clear lint warnings */
           if (i < MAX_PV_THREAD)
           {
             TASKLOCK();
             evt_ptr = q_get(&generic_event_table[i].event_q);
             if (q_cnt(&generic_event_table[i].event_q) == 0)
             {
               (void)rex_clr_sigs(rex_self(), GEM_SIG);
             }
             TASKFREE();

             generic_event_table[i].g_event_ptr = evt_ptr;
           }
           rex_leave_crit_sect(&zrex_em_crit_sect);

           if (evt_ptr != NULL)
           {
             INTLOCK();
             /* de-initialize zrex2rex */
             if( bInitialized ) {
               zrex2rex_dinit();
             }
             INTFREE();
             return evt_ptr;
           }
	 }
   }//end while(1) 
}


/* ======================================================================
FUNCTION:
  zrex_post_event

DESCRIPTION:
  Asychronously puts the event into the event engine.  Returns
  immediately.  Multiple event sinks are not supported.

INPUT/OUPUT PARAMETERS:
  event_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_post_event(
   zrex_event_type *event_ptr
)
{

   zrex_event_header_type *e_ptr = event_ptr;
   zrex_event_sink_type   *event_sink_ptr;
   unsigned long i = 0;
   boolean event_posted = FALSE;

   /* Verify that zrex2rex has been initialized
    */
   INTLOCK();
   if( !bInitialized ) {
      zrex2rex_init();
   }
   INTFREE();

   if(e_ptr->isStatic)
   {
     rex_enter_crit_sect(&zrex_em_crit_sect);
     if ( e_ptr->isFreeToDispatch )
     {
       e_ptr->isFreeToDispatch = 0;
     }
     else
     {
       //event already in the event-q
       rex_leave_crit_sect(&zrex_em_crit_sect);
       return;
     }
     rex_leave_crit_sect(&zrex_em_crit_sect);
   }

   while (!event_posted && i < MAX_PV_THREAD)
   {
      INTLOCK();
      if (generic_event_table[i].tcb_ptr != NULL)
      {
         event_sink_ptr = generic_event_table[i].event_sink_ptr;
         while (event_sink_ptr->assoc_ptr != NULL)
         {
            if (event_sink_ptr->assoc_ptr == e_ptr->assoc_ptr)
            {
               e_ptr->event_handler = event_sink_ptr->func_ptr;
               q_put(&generic_event_table[i].event_q, &e_ptr->link);
               (void)rex_set_sigs(generic_event_table[i].tcb_ptr, GEM_SIG);
               event_posted = TRUE;
               break;
            }
            event_sink_ptr++;
         }
      }
      INTFREE();
      i++;
   }
   if(!event_posted && event_ptr)
   {
     zrex_event_release(event_ptr);
   }

   INTLOCK();
   /* de-initialize zrex2rex */
   if( bInitialized ) {
      zrex2rex_dinit();
   }
   INTFREE();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/* ======================================================================
FUNCTION:
  zrex_set_event_dispatch_table


DESCRIPTION:
  Creates a dispatch table for the currently running thread

INPUT/OUPUT PARAMETERS:
  table_ptr

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex_set_event_dispatch_table(
   zrex_event_sink_type *table
)
{
   unsigned long  i;
   rex_tcb_type  *tcb_ptr = rex_self();
   unsigned long  empty_slot = MAX_PV_THREAD;

   INTLOCK();
   /* Verify that zrex2rex has been initialized
    */
   if( !bInitialized ) {
      zrex2rex_init();
   }
   INTFREE();

   rex_enter_crit_sect(&zrex_em_crit_sect);
   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      if (generic_event_table[i].tcb_ptr == tcb_ptr)
      {
         break;
      }
      else if (generic_event_table[i].tcb_ptr == NULL &&
               empty_slot == MAX_PV_THREAD)
      {
         /* Record first empty slot in the table */
         empty_slot = i;
      }
   }

   if (i == MAX_PV_THREAD)
   {
      if (empty_slot != MAX_PV_THREAD)
      {
         i = empty_slot;
      }
      else
      {
         ASSERT(0);
      }
   }

   /* Add condition check to satisfy Lint.  This check should never be hit due
    * to the ASSERT in the above code
    */
   if (i < MAX_PV_THREAD)
   {
     generic_event_table[i].tcb_ptr = tcb_ptr;
     generic_event_table[i].event_sink_ptr = table;
   }
   rex_leave_crit_sect(&zrex_em_crit_sect);

   INTLOCK();
   /* de-initialize zrex2rex */
   if( bInitialized ) {
      zrex2rex_dinit();
   }
   INTFREE();
}


/*************************
 *
 * Functions in zrextime.c
 *
 *************************/
/* ======================================================================
FUNCTION:
  zrex_get_current_time_offset

DESCRIPTION:
  Get the current time offset

INPUT/OUPUT PARAMETERS:
  offset

RETURN VALUE:
  None.

SIDE EFFECTS:
The implementation will affect the implementation of
zrex_convert_time_to_systime()
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex_get_current_time_offset(
   zrex_time_offset_type *offset
)
{
   qword ms;
   qword s;

   //clk_read_ms(ms);
   /* here we use clock uptime, so even after loosing/getting RF signal, our
      relative values will not change and we don't need actual time */

   #ifdef FEATURE_CMI
#error code not present
   #else
      clk_uptime_ms(ms);
   #endif
   offset->milliseconds = qw_div(s, ms, 1000);
   offset->seconds = qw_lo(s);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex_convert_time_to_systime

DESCRIPTION:
  convert the time to system time

INPUT/OUPUT PARAMETERS:
  offset

RETURN VALUE:
  None.

SIDE EFFECTS:
The implementation is totally depends on the implementation of
zrex_get_current_time_offset()
========================================================================== */
void zrex_convert_time_to_systime(
   zrex_time_offset_type* offset
)
{
  qword up_ms, ms;
  qword s;
  // get power_on_ms

  #ifdef FEATURE_CMI
#error code not present
  #else
    clk_uptime_ms(up_ms);
    clk_read_ms(ms);
  #endif
  qw_sub(ms, ms, up_ms);
  // add back power_on_ms
  offset->milliseconds += qw_div(s, ms, 1000);
  offset->seconds += qw_lo(s);
  if (offset->milliseconds >= 1000)
  {
    // carry over
    offset->seconds += offset->milliseconds / 1000;
    offset->milliseconds = offset->milliseconds % 1000;
  }
}

/**********************
 *
 * zrex2rex.c functions
 *
 **********************/
/* ======================================================================
FUNCTION:
  zrex2rex_init

DESCRIPTION:
  Initalization function

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void zrex2rex_init(void)
{
   unsigned long i;

   rex_init_crit_sect(&zrex_em_crit_sect);

   for (i = 0; i < MAX_PV_THREAD; i++)
   {
      generic_event_table[i].tcb_ptr = NULL;
      (void)q_init(&generic_event_table[i].event_q);
      generic_event_table[i].g_event_ptr = NULL;
      generic_event_table[i].event_sink_ptr = NULL;
   }

#ifdef FEATURE_TEST_GEM
#error code not present
#endif

#ifdef FEATURE_TEST_SYNC_EVENT
#error code not present
#endif

   bInitialized = TRUE;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION:
  zrex2rex_dinit

DESCRIPTION:
  De-Initalization function

INPUT/OUPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
========================================================================== */
void zrex2rex_dinit(void)
{
/* ------
Commented out, since this code goes to all targets as common and 
below REX API is not currently ported to all active targets and baselines.
--------- */

  //rex_del_crit_sect(&zrex_em_crit_sect);
  //bInitialized = FALSE;
}



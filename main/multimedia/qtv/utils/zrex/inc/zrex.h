/* =======================================================================
                              zrex.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/zrex.h#10 $
$DateTime: 2009/12/22 03:27:02 $
$Change: 1121821 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef ZREX_H
#define ZREX_H
#ifndef FEATURE_WINCE
#ifndef FEATURE_DS_SOCKETS_BSD
   #define FEATURE_DS_SOCKETS_BSD
#endif
#endif /* NOT FEATURE_WINCE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "rex.h"
#include "queue.h"
#ifndef FEATURE_QTV_NOSTREAM
#ifndef FEATURE_WINCE
#include "dsbsd.h"
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "qtvsystem.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MAX_PV_THREAD         10
#define ZREX_INFINITE         0xFFFFFFFFUL

/* Common Signals -- can be used by any of the QTV tasks - player, streamer,
 * etc. Do not change the value of signals marked Reserved since these
 * values are specified by the task services. Except for the reserved
 * signals, the QTV common signals are in the 25, 26 , 27 and 28th bits and
 * specific to a QTV task are in 17, 18, 19, and 20th bits
 */
/***************************************************************************
********************* CAUTION CAUTION CAUTION ******************************
****************************************************************************
** When you change signals, the repercursions are beyond QTV. Please consult
** with other tasks that might be running QTV code in their context. One
** example is UI. Having these signals for example conflict with UI has
** undesirable consequences (like one task clearing another tasks signals
** inadvertantly
****************************************************************************
********************* CAUTION CAUTION CAUTION ******************************
***************************************************************************/

#ifdef T_MSM7500
#error code not present
#else
#define GEM_SIG               0x01000000  // Signal for Generic Event Model
#define SYNC_EVENT_SIG        0x02000000  // Sync event signal
#define SYNC_EVENT_TIMER_SIG  0x04000000  // sync event timer signal
#define QTV_NV_SIG            0x08000000  // signal used for NV operations
#endif
#define QTV_TASK_OFFLINE_SIG  0x00002000  // Reserved
#define QTV_TASK_STOP_SIG     0x00004000  // Reserved
#define QTV_TASK_START_SIG    0x00008000  // Reserved
#define QTV_TASK_EFS_SIG      0x40000000  // Reserved

/* Signals specific to different QTV tasks
 */

/* Streamer signals -- to be used only by the streamer task. When any signal
 * to this list, make sure that it does not conflict with the common signals
 */

#define SELECT_TIMER_SIG      0x00010000
#define DNS_SIG               0x00020000
#define NET_CB_SIG            0x00040000
#define SOCKET_CB_SIG         0x00080000
#define SLEEP_TIMER_SIG       0x00100000
#define NET_ABORT_SIG         0x00200000
#define IFACE_IOCTL_SIG       0x00400000
#define TCP_CLOSE_TIMEOUT_SIG 0x00800000

#ifdef FEATURE_QTV_DRM_DCF
 //IPC signal used by various QTV tasks to communicate with DRM suite
 #define QTV_DRM_DCF_IPC_SIG   0x00000800
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/***********************************
 *
 * #defines/typedefs from zrexbase.h
 *
 ***********************************/
typedef void* zrex_handle_type;
typedef rex_stack_size_type zrex_stack_size_type;
typedef rex_priority_type   zrex_priority_type;
typedef rex_task_func_type  zrex_task_func_type;



/***********************************
 *
 * #defines/typedefs from zrextime.h
 *
 ***********************************/
typedef struct
{
   long seconds;
   long milliseconds;
} zrex_time_offset_type;


/*********************************
 *
 * #defines/typedefs from zrexse.h
 *
 *********************************/
typedef enum {
   ZREX_SYNC_EVENT_TMO,
   ZREX_SYNC_EVENT_SET,
   ZREX_SYNC_EVENT_RESET
} zrex_sync_event_status_type;

typedef enum {
   ZREX_AUTO_RESET,
   ZREX_MANUAL_RESET
} zrex_sync_event_reset_type;

typedef enum {
   ZREX_SYNC_EVENT_IS_SET,
   ZREX_SYNC_EVENT_IS_RESET
} zrex_sync_event_state_type;

typedef struct {
   rex_tcb_type   *waiting_tcb_list[MAX_PV_THREAD];
   unsigned long   num_waiting_tcbs;
   unsigned long   ztcb_index[MAX_PV_THREAD];
   unsigned long   signal_mask;
   unsigned char   state;
   unsigned char   reset_type;
} zrex_sync_event_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/*********************************
 *
 * #defines/typedefs from zrexem.h
 *
 *********************************/
#define EVENT_POOL( xx_pool_id, xx_pool_size )

#define EVENT_SOURCE( xx_id, xx_pool_id )\
   zrex_event_assoc_type xx_id##_ZEMDATA = 0xCAFE;

#ifdef __cplusplus
#define EVENT( xx_id )\
   extern "C" zrex_event_assoc_type xx_id##_ZEMDATA;\
   typedef struct xx_id##_tag xx_id##_type;\
   struct xx_id##_tag {\
      zrex_event_header_type hdr;
#else
#define EVENT( xx_id )\
   extern zrex_event_assoc_type xx_id##_ZEMDATA;\
   typedef struct xx_id##_tag xx_id##_type;\
   struct xx_id##_tag {\
      zrex_event_header_type hdr;
#endif

#define EVENT_END\
   };

#define EVENT_TABLE( xx_table_name, xx_tcb )\
   zrex_event_sink_type xx_table_name[] = {\

#define EVENT_SINK( xx_id, xx_handler, xx_pri, xx_state  )\
   { (xx_id), (xx_handler), &xx_id##_ZEMDATA, (xx_pri) },

#define EVENT_TABLE_END\
   { 0, NULL, NULL, 0 }\
   };

#define zrex_create_event( xx_event_id, xx_user_data )\
   (xx_event_id##_type*)\
   zrex_create_event_ex(\
                          xx_event_id,\
                          sizeof( xx_event_id##_type ),\
                          &xx_event_id##_ZEMDATA,\
                          xx_user_data\
                       )

//Macro to create an event which will not be deleted after dispatching.
#define zrex_create_static_event( xx_event_id, xx_user_data )\
   (xx_event_id##_type*)\
   zrex_create_static_event_ex(\
                          xx_event_id,\
                          sizeof( xx_event_id##_type ),\
                          &xx_event_id##_ZEMDATA,\
                          xx_user_data\
                       )

typedef unsigned long zrex_event_assoc_type;
typedef void (*zrex_event_handler_type)( zrex_handle_type handle );
typedef unsigned long zrex_event_id_type;
typedef void zrex_event_type;

typedef struct zrex_event_header_struct {
   q_link_type              link;
      /* Links into the task's generic event queue */
   zrex_event_id_type       id;
      /* The associated id for this event, used for testing/debugging */
   unsigned long            size;
      /* size of the event, includes the size of this header */
   zrex_event_handler_type  event_handler;
      /* pointer to the event handler */
   zrex_event_assoc_type   *assoc_ptr;
      /* Indicate if its a static event: It will not be deleted after dispatching.
         The event creator will delete it explicitly*/
   uint8 isStatic;
   uint8 isFreeToDispatch;

   void                    *user_data;
} zrex_event_header_type;

typedef struct {
   zrex_event_id_type       id;
      /* The event id, used for testing/debugging */
   zrex_event_handler_type  func_ptr;
      /* Pointer to function which handles the event */
   zrex_event_assoc_type   *assoc_ptr;
      /* The associated event data */
   unsigned short           pri;
      /* Priority at which the event should be queued for this thread */
} zrex_event_sink_type;


/* =======================================================================
**                        Class Declarations
** ======================================================================= */
/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */
#ifdef __cplusplus
   extern "C"
   {
#endif

void zrex2rex_init(void);
void zrex2rex_dinit(void);


/**************************
 *
 * Functions from zrexnet.c
 *
 **************************/
int
   /* 0 on success
   */
zrex_bind(
   int              s,
      /* Socket with which to associated the name
      */
   struct sockaddr *name,
      /* Address to associated with socket
      */
   int              namelen
      /* Size of the address structure
      */
);

int zrex_bsd_init(void);

int zrex_bsd_shutdown(void *ptr);

int
   /* 0 on success
   */
zrex_close(
   int s
      /* Socket to close
      */
);

int
zrex_connect(
   int              s,
      /* Socket to use
      */
   struct sockaddr *peername,
      /* Where you want to connect
      */
   int              peernamelen
      /* Size of the address structure
      */
);

int
   /* Number of bytes read
   */
zrex_read(
   int   s,
      /* Socket
      */
   void *buf,
      /* Pointer to destination buffer
      */
   unsigned int len
      /* Number of bytes to read
      */
);

int
   /* Number of bytes read
   */
zrex_recv(
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
);

int
   /* Number of bytes read
   */
zrex_recvfrom(
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
);

#ifndef FEATURE_WINCE
int
zrex_select(
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
);
#endif /* NOT FEATURE_WINCE */

int
   /* Number of bytes sent
   */
zrex_send(
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
);

int
   /* Number of bytes sent
   */
zrex_sendto(
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
);

int
   /* Socket, -1 otherwise
   */
zrex_socket(
   int af,
      /* Protocol family
      */
   int type,
      /* Protocol type
      */
   int protocol
      /* Which protocol
      */
);

void zrex_net_abort(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);

void zrex_clear_net_abort(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);

/***************************
 *
 * Functions from zrextime.c
 *
 ***************************/
void zrex_get_current_time_offset(
   zrex_time_offset_type *offset
);

void zrex_convert_time_to_systime(
   zrex_time_offset_type *offset
);

/*****************************
 *
 * Functions from zrexthread.c
 *
 *****************************/
zrex_handle_type
   /* Handle to thread on success, NULL otherwise
   */
zrex_create_thread(
   zrex_stack_size_type stack_size,
      /* Requested stack size
      */
   zrex_priority_type   priority,
      /* Requested priority
      */
   rex_task_func_type   thread_ptr,
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
      ** FALSE indicates the thread is immediately scheduable after
      ** creation
      */
);

void zrex_set_thread_priority(zrex_handle_type handle, zrex_priority_type priority);

zrex_priority_type zrex_get_thread_priority(zrex_handle_type handle);

void zrex_kill_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);

void zrex_release_event_q(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);

void zrex_pause_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);

void zrex_resume_thread(
   zrex_handle_type handle
      /* Handle returned by zrex_create_thread
      */
);


/***********************
 *
 * Functions from zrex.c
 *
 ***********************/
void zrex_sleep(
   unsigned long ms
);


/*************************
 *
 * Functions from zrexse.c
 *
 *************************/
void
zrex_init_sync_event(
   zrex_sync_event_type *event_ptr,
      /* Pointer to a event structure to initialize
      */
   boolean               reset_is_manual,
      /* Whether or not the event resets itself after a wait, if TRUE,
      ** the user must reset the event with a call to zrex_reset_event,
      ** otherwise the event remains in the signalled state, when FALSE,
      ** the event automatically is reset before a return from
      ** zrex_wait_for_single_event
      */
   boolean               initial_state_is_set
      /* Whether or not the event is in the signalled state at init
      ** time, when TRUE, the event is immediately in the signalled state
      ** as if the user had immediately called zrex_set_sync_event,
      ** when FALSE, the event is initialized in the non signalled case
      */
);

void zrex_release_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
);

void zrex_reset_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
);

void zrex_set_sync_event(
   zrex_sync_event_type *event_ptr
      /* Handle to the event
      */
);

zrex_sync_event_status_type
   /* ZREX_WAIT_TMO if the timeout occurrs
   ** ZREX_EVENT_SET if the event is set
   */
zrex_wait_for_sync_event(
   zrex_sync_event_type *event_ptr,
      /* Handle to the event
      */
   unsigned long        milli_secs
      /* Number of milliseconds to wait for the event to be signalled.
      */
);

/* this will forcebly release all sync events. This should be called only
   when QTV is exiting. Any remaining SYNC events will be lost */
void zrex_release_all_sync_event(void);

/*************************
 *
 * Functions from zrexem.c
 *
 *************************/
zrex_event_type*
   /* Pointer to the event on success, NULL otherwise
   */
zrex_create_event_ex(
   zrex_event_id_type     id,
      /* ID for the event
      */
   unsigned long          size,
      /* Size of the event
      */
   zrex_event_assoc_type *assoc_ptr,
      /* Source/Sink association data
      */
   void                  *user_data
);

zrex_event_type*
   /* Pointer to the static event on success, NULL otherwise
   */
zrex_create_static_event_ex(
   zrex_event_id_type     id,
      /* ID for the event
      */
   unsigned long          size,
      /* Size of the event
      */
   zrex_event_assoc_type *assoc_ptr,
      /* Source/Sink association data
      */
   void                  *user_data
);

void zrex_dispatch_event(
   zrex_event_type *event_ptr
      /* Pointer to an event
      */
);

void zrex_event_release(
   zrex_event_type *event_ptr     /* Pointer to the event */
);

zrex_event_type*
   /* Pointer to an event, NULL otherwise
   */
zrex_get_next_event( void );

void zrex_post_event(
   zrex_event_type *event_ptr
      /* Pointer to event
      */
);

void zrex_set_event_dispatch_table(
   zrex_event_sink_type table_ptr[]
      /* Array of event_sinks
      */
);

#ifdef __cplusplus
   }
#endif

#endif /* ZREX_H */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ I F A C E . C


GENERAL DESCRIPTION
  This is the implementation of the PS interface "member functions".  This
  provides the interface between the IP layer (including IP routing between
  interfaces) and the device specific IP transport mechanism, such as RLC,
  PPP/RLP or 802.3.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  In order to use the functions defined in this module it is important that
  the interface data structure be "created" using the create() function.

Copyright (c) 2002-2011 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface.c_v   1.27   24 Feb 2003 10:55:32   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface.c#3 $ $DateTime: 2011/02/24 23:31:53 $ $Author: msankar $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/25/11    ms     Ported support for MOBILE_IP_DEREG feature.
06/17/08    pp     Fixed RVCT compiler warnings.
07/29/03    ss     Changed ps_iface_sdb_status_handler() to call the upper
                   layer handler through a function pointer
07/08/03    ss     Added call to dssocki_sdb_status_handler() for SDB notif.
07/07/03    atp    Added ps_iface_sdb_status_handler() as part of SDB
                   enhancements to pass status notification to application.
05/05/03    aku    Replaced ps_iface_ioctl_name_enum_type with 
                   dss_iface_ioctl_name_enum_type in ps_iface_get_handle()
05/02/03    usb    Removed instance param from ps_iface_create(), instances
                   are now assigned automatically to the ifaces in 
                   sequential order. Resetting DNS info when the iface
                   goes DOWN.
03/19/03    aku    Set the ip address to zero in ps_iface_get_addr() if the
                   type field is being set to IP_ADDR_INVALID.
02/21/03    usb    Fixed bug in ps_iface_get_handle() where it was not 
                   returning NULL if no matching iface is found, removed 
                   TASKLOCK around call to tx_fn() for bridge iface.
02/13/03    usb    Fixed event buffer allocation to avoid memory leak and
                   dsm item corruption, fixed ipfltr_updated_ind() to 
                   calculate new filter count correctly.
02/12/03    aku    Added support for flow control of non system sockets.
02/11/03    usb    changed ps_iface_ipfltr_updated_ind() to take num filters
                   actually added and deleted to calculate cur value.
02/07/03    usb    Added IP filter info to iface_cb, added event 
                   notification for IFACE_IPFLTR_UPDATED_EV.
12/24/02    aku    Added ps_iface_get_handle() and support for 
                   global_iface_array.
12/06/02    mvl    Fixed comiler warnings.
11/25/02    usb    Added q_type to ps_ifacei_event_buf_type to store the 
                   queue on which the event buf is actually registered.  
                   Enhanced error handling in event callback functions.
11/19/02    jd     Backed out routeable callbacks only on if state change
11/15/02    mvl    Added physical link statistics.
11/14/02    jd/usb Fixed teardown to return SUCCESS if no teardown fn reg'd
                   Call iface routeable event callbacks only on iface state 
                   change 
11/14/02    jd     Allow ps_iface_get_addr in routeable (for MIP)
11/12/02    mvl    Will report IP address changes in ROUTABLE state also.
10/31/02    mvl    Wrapped all relevant functions completely in
                   TASKLOCK()s.  Removed the delete() call.
10/25/02    usb    Flow is only disabled during phys link up cmd if we 
                   are coming out of dormancy, not for fresh originations.
                   Also fixed a bug in tx_cmd().
10/08/02    mvl    Setting the kind field of the IP packet in
                   ps_iface_input(), and also forwarding to the bridged
                   interface if bridging is enabled.
10/01/02    mvl    Added gateway address to the v4_net_info.
09/17/02    mvl    Added cookie to iface_cb, calling route_flush when an
                   interface is disabled.  cback_reg() now returns success if
                   a callback is already registered.  Cleaning up the
                   net_info when interface goes down.  More tasklock fixes.
                   Added the bridge processing call.
09/06/02    mvl    fixed some bugs in new memory handling for cback events.
09/04/02    mvl    changed memory handling for event callback buffers.
09/03/02    mvl    Missing handler for bring down commands will not fail, but
                   will return success.  When physical link is in coming_up
                   or going_down state the interface has flow disabled.
                   fixed bug where state was being set to transient value
                   before ensuring that it was possible to do so (i.e. there
                   was a handler for it).  Only posting the IP addr changed
                   event in the up state.  Put TASKLOCK()s around many
                   functions.  Re-ordered functions.
08/30/02    om     Added ACL registration in pf_iface_create().
08/13/02    mvl    updated ps_iface state macros for name change
07/30/02    usb    Various Fixes: DOWN event was being posted in 
                   routeable_ind() fn; tear_down_cmd() and 
                   phys_link_down_cmd() were changing
                   state after calling the callback; Added an ASSERT if 
                   phys_link_down_cmd() is called and iface is DOWN or 
                   DISABLED. 
07/24/02    mvl    Replaced ASSERT() in ps_iface_input() with MSG_HIGH() as
                   this case is possible under normal operation.
07/16/02    usb    Hack to receive IP packets int the stack until we have 
                   the new routing module. 
07/15/02    mvl    More IP addressing functionality.
07/11/02    mvl    Moved PS_IFACE_IS_VALID() to header file.  Also added
                   support for global event callbacks.  Updates for new PPP
                   architecture.
07/01/02    mvl    added ps_iface_set_ip_addr()
05/14/02    mvl    initial compile and checkin
03/15/02   na/mvl  created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA
#include "ps_acl.h"
#include "dserrno.h"
#include "err.h"
#include "msg.h"
#include "ps_iface.h"
#include "ps_ip.h"
#include "ps_route.h"
#include "rex.h"
#include "ds_flow_control.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
int ps_ifacei_default_tx_cmd
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr,
  void               *tx_cmd_info
);


void ps_ifacei_invoke_event_cbacks
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info
);


int ps_ifacei_set_local_addr
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr
);

void ps_ifacei_addr_alloc_cback
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define DEFAULT_DATA_UNIT 1500   /* used for MRU and MTU                   */

/*---------------------------------------------------------------------------
  This macro is intended to be used only before return statement in functions
  that have been wrapped in tasklock/taskfree calls
---------------------------------------------------------------------------*/
#define PS_BRANCH_TASKFREE() {TASKFREE()

/*---------------------------------------------------------------------------
  GLOBAL_EVENT_Q_ARRAY - this is used to store callbacks that are intended to
  be called when events happen onANY interface.
  GLOBAL_EVENT_Q_INITIALIZED - has the global queue been initialized?

  NOTE: these are separate (not in a struct) to insure that the
        initialization variables are set correctly.
---------------------------------------------------------------------------*/
q_type  global_event_q_array[IFACE_EVENT_MAX]; /* global event cback array */
boolean global_event_q_initialized = FALSE;    /* has above been initied   */
 
/*-------------------------------------------------------------------------
  GLOBAL_IFACE_PTR_ARRAY - Array of pointers to all of the the created
    interfaces on the system.  The semantics of this array are that empty
    slots have a value of 0, any other values indicate the slot points to an
    iface.
  GLOBAL_IFACE_PTR_ARRAY_INDEX - the next open slot in the array
---------------------------------------------------------------------------*/
 ps_iface_type *global_iface_ptr_array[MAX_SYSTEM_IFACES] = {0,0,0,0};
 uint8 global_iface_ptr_array_index = 0; 

/*===========================================================================
TYPEDEF PS_IFACEI_EVENT_BUF_TYPE

DESCRIPTION
  This is the type that is placed on the event callback queues.  It is only
  used internally and so should not be referenced outside of this file.
===========================================================================*/
typedef struct
{
  q_link_type               link;
  q_type                   *event_q_ptr;
  dsm_item_type            *this_item;
  ps_iface_event_cback_type event_cback_f_ptr;
  void                     *user_data_ptr;
} ps_ifacei_event_buf_type;

#define EVENT_BUF_SIZE sizeof(ps_ifacei_event_buf_type)
#define EVENT_BUF_ITEM_SIZE DSM_DS_SMALL_ITEM_SIZ
#define EVENT_BUF_ITEM_POOL DSM_DS_POOL_SIZE(EVENT_BUF_ITEM_SIZE)
#define EVENT_BUF_ITEM_KIND 0xEB

/*-------------------------------------------------------------------------
  Ptr to memory to be used for storing event buffers.
---------------------------------------------------------------------------*/
LOCAL dsm_item_type     *curr_alloc_ptr = NULL;

 
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS
             ---------------------------------------------------
                               CONFIGURATION

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACE_CREATE()

DESCRIPTION
    This will memset()s the private data struct to 0, initializes the
    "this_ptr" to point to the interface control block itself, initializes
    the queues and the TX ptr to the default value.  It will also add the
    interface to the global interface pointer array - this will ASSERT() if
    there are not enough interface slots available.

    NOTE: the memset() sets the iface_state to DISABLED.

PARAMETERS
    this_iface_ptr: ptr to interface control block on which to operate on.
    name:     the name of the interface (like CDMA_SN_IFACE)
    instance: the instance of this interface - generally starts with 0
    this_iface_acl_ptr: pointer to the acl to include the defailt ACL in the
              creation of the IFACE.

RETURN VALUE
  -1  - error in iface creation
  >=0 - Else instance number of the newly created iface

DEPENDENCIES
  The interface being created MUST NOT be freed before this function returns

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_create
(
  ps_iface_type          *this_iface_ptr,
  ps_iface_name_enum_type name,
  acl_type               *this_iface_acl_ptr
)
{
  static ps_iface_cookie_type global_cookie = 1;
  int loop;
  int instance = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(global_iface_ptr_array_index != MAX_SYSTEM_IFACES);
  TASKLOCK();
  if(this_iface_ptr == NULL ||
     this_iface_ptr->private.this != NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

  /*-------------------------------------------------------------------------
    Find the highest instance number used so far for the iface of this type. 
    The instance of the newly created iface should be one higher than that.
  -------------------------------------------------------------------------*/
  for(loop=global_iface_ptr_array_index-1; loop>=0; loop--)
  {
    /*-----------------------------------------------------------------------
      Since ifaces are added sequentially and they are never deleted, we
      traverse the array of iface bakwards and as soon as we get an iface 
      matching "name", its instance number is the highest among all ifaces 
      of that type.
    -----------------------------------------------------------------------*/
    if(global_iface_ptr_array[loop]->name == name)
    {
      instance = global_iface_ptr_array[loop]->instance+1;
      break;
    }
  }

  MSG_HIGH("Creating iface(0x%x) %d:%d", this_iface_ptr, name, instance);

  /*-------------------------------------------------------------------------
    reset the private info and initialize all of the queue strucutures
  -------------------------------------------------------------------------*/
  memset(&(this_iface_ptr->private), 0, sizeof(this_iface_ptr->private));
  for(loop = 0; loop < IFACE_EVENT_MAX; loop++)
  {
    q_init((this_iface_ptr->private.event_q_array) + loop);
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */
  }

  /*-------------------------------------------------------------------------
    Initialize the IP filter queue and by default set the flag to not
    process the filters at iface level.
  -------------------------------------------------------------------------*/
  q_init(&(this_iface_ptr->private.ip_filter_info.filter_q));
  this_iface_ptr->private.ip_filter_info.process_filter = FALSE;

  /*-------------------------------------------------------------------------
    Insert the iface into the global iface_ptr array.
  -------------------------------------------------------------------------*/
  global_iface_ptr_array[global_iface_ptr_array_index] = this_iface_ptr;
  global_iface_ptr_array_index += 1;

  /*-------------------------------------------------------------------------
    Set the this ptr, assign the name and instance, set the tx function to
    the default value, disallow the dormant cleanup and set the peer address
    function to NULL.
  -------------------------------------------------------------------------*/
  this_iface_ptr->private.this            = this_iface_ptr;
  this_iface_ptr->name                    = name;
  this_iface_ptr->instance                = instance;
  this_iface_ptr->private.tx_cmd          = ps_ifacei_default_tx_cmd;
  this_iface_ptr->graceful_dormant_close  = FALSE;

  /*-------------------------------------------------------------------------
    Assign the cookie, and increment the global counter.
  -------------------------------------------------------------------------*/
  this_iface_ptr->private.cookie = global_cookie;
  global_cookie += 1;

  /*-------------------------------------------------------------------------
    Set the MRU and MTU in the v4 net info to the default values, set the
    getway and DNS addresses to 0.
  -------------------------------------------------------------------------*/
  this_iface_ptr->v4_net_info.mtu = DEFAULT_DATA_UNIT;
  this_iface_ptr->v4_net_info.mru = DEFAULT_DATA_UNIT;
  this_iface_ptr->v4_net_info.gateway = 0;
  this_iface_ptr->v4_net_info.primary_dns = 0;
  this_iface_ptr->v4_net_info.secondary_dns = 0;

  /*-------------------------------------------------------------------------
    intialize statistics
  -------------------------------------------------------------------------*/
  memset(&this_iface_ptr->stats, 0, sizeof(ps_iface_stats_type));

  /*-----------------------------------------------------------------------
    Store the mode handler's ACL for this interface
  -------------------------------------------------------------------------*/
  if(this_iface_acl_ptr)
  {
    this_iface_acl_ptr->if_ptr = (void*) this_iface_ptr;
    route_acl_add(this_iface_acl_ptr, ROUTE_ACL_ANY_PRIORITY);
    // Error handling ??
  }
  TASKFREE();
  
  return instance;
} /* ps_iface_create() */



#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */


/*===========================================================================
FUNCTION PS_IFACE_ALLOC_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for ps_iface
  event callbacks.

PARAMETERS
  event_cback_f_ptr: the callback to be registered
  user_data_ptr: data to be called with the callback.

RETURN VALUE
  NULL: if allocation failed
  ptr to memory being allocated.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void *ps_iface_alloc_event_cback_buf
(
  ps_iface_event_cback_type event_cback_f_ptr,
  void                     *user_data_ptr
)
{
  ps_ifacei_event_buf_type *buf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event_cback_f_ptr == NULL)
  {
    return NULL;
  }

  if(EVENT_BUF_SIZE > EVENT_BUF_ITEM_SIZE)
  {
    ASSERT(0);
    return NULL;
  }

  TASKLOCK();  
  /*-------------------------------------------------------------------------
    if the current allocation pointer is null then a new DSM item is needed
    to provide the buffer back to the caller, so grab one.  If the DSM
    allocation fails, return NULL.
  -------------------------------------------------------------------------*/
  if(curr_alloc_ptr == NULL)
  {
    curr_alloc_ptr = dsm_new_buffer(EVENT_BUF_ITEM_POOL);
    if(curr_alloc_ptr == NULL)
    {
      PS_BRANCH_TASKFREE();
      return NULL;
    }

    /*-----------------------------------------------------------------------
      Reference field sould be set to number of event buffers using it, since
      dsm_new_buffer() increments the reference by one regardless, reset it.
    -----------------------------------------------------------------------*/
    curr_alloc_ptr->references--;

  } /* if(no item) */

  /*-------------------------------------------------------------------------
    Initialize the event buffer
  -------------------------------------------------------------------------*/
  buf_ptr = (ps_ifacei_event_buf_type*)(curr_alloc_ptr->data_ptr +
                                        curr_alloc_ptr->used);
  q_link(buf_ptr, &(buf_ptr->link));
  buf_ptr->this_item         = curr_alloc_ptr;
  buf_ptr->event_cback_f_ptr = event_cback_f_ptr;
  buf_ptr->user_data_ptr     = user_data_ptr;
  buf_ptr->event_q_ptr       = NULL;

  /*-------------------------------------------------------------------------
    do the book keeping with the DSM item.  NOTE this assumes that the
    EVENT_BUF_SIZE allows these things to be packed - i.e. are 4 byte aligned
  -------------------------------------------------------------------------*/
  curr_alloc_ptr->references += 1;
  curr_alloc_ptr->used       += EVENT_BUF_SIZE;
  curr_alloc_ptr->kind       =  EVENT_BUF_ITEM_KIND;

  /*-------------------------------------------------------------------------
    make sure that there is enough room to insert another buffer in here, if
    not reset the curr_alloc_ptr to NULL.
  -------------------------------------------------------------------------*/
  if((curr_alloc_ptr->size - curr_alloc_ptr->used) < EVENT_BUF_SIZE)
  {
    curr_alloc_ptr = NULL;
  }  
  TASKFREE();

  return buf_ptr;

} /* ps_iface_alloc_event_cback_buf() */



/*===========================================================================
FUNCTION PS_IFACE_FREE_EVENT_CBACK_BUF()

DESCRIPTION
  This function is used to allocate the memory used to register for ps_iface
  event callbacks.

PARAMETERS
  buf: the previously allocated buffer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_free_event_cback_buf
(
  void *buf
)
{
  dsm_item_type *item_ptr;
  uint8         *buf_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(buf == NULL)
  {
    ASSERT(0);
    return;
  }

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in:
    1) not already on a queue
    2) buffer must be in the data section of the containing DSM item
    3) the kind field is set correctly
    4) the item pointer in the first slot points to the containing item
  -------------------------------------------------------------------------*/
  item_ptr = (((ps_ifacei_event_buf_type*)buf)->this_item);  
  if(item_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  buf_data = (uint8*)buf;
  if(Q_ALREADY_QUEUED(&(((ps_ifacei_event_buf_type*)buf)->link))         ||
     item_ptr->kind != EVENT_BUF_ITEM_KIND                               ||
     buf_data < item_ptr->data_ptr                                       ||
     buf_data > (item_ptr->data_ptr + item_ptr->size - EVENT_BUF_SIZE)   ||
     ((ps_ifacei_event_buf_type*)item_ptr->data_ptr)->this_item != item_ptr)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Invalidate the buffer so that the caller cannot use it without
    explicitly allcating it again. 
  -------------------------------------------------------------------------*/
  ((ps_ifacei_event_buf_type*)buf)->event_cback_f_ptr = NULL;

  /*-------------------------------------------------------------------------
    Since the reference count is automatically incremented on every alloc, it
    suffices to simply call free_buffer() - if this was the last event buffer
    using this dsm item, then the dsm item will be freed automatically. If 
    the item being freed is same as the one we are hanging on to, discard it.
  -------------------------------------------------------------------------*/
  if(item_ptr == curr_alloc_ptr && curr_alloc_ptr->references == 1)
  {
    curr_alloc_ptr = NULL;
  }
  TASKFREE();

  dsm_free_buffer(item_ptr);

} /* ps_iface_free_event_cback_buf() */



/*===========================================================================
FUNCTION PS_IFACE_EVENT_CBACK_REG()

DESCRIPTION
  Registers callbacks for events on an interface.  Whenever an event occurs,
  the callback for that event is called.  If the event is true when
  registration happens the callback is called immediately.  This will fail if
  either the interface is invalid, or the buffer to queue (with callback and
  user data pointer) is NULL.  A callback will only be queued if the event
  has not previously been queued.

  Memory is owned by the caller and it is put into the queue for that
  event.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. Should not be NULL.

RETURN VALUE
  0: on success (include
 -1: on failure (any of the passed in parameters is bad)

DEPENDENCIES
  The link field must have been initialized with q_link().

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_event_cback_reg
(
  ps_iface_type           *this_iface_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
)
{
  ps_iface_event_info_u_type event_info;
  ps_ifacei_event_buf_type *buf_ptr;
  dsm_item_type *item_ptr;
  uint8 *buf_data;
  int    loop;
  int    ret_val = 0;
  boolean need_to_invoke_cback = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(buf == NULL ||
     event >= IFACE_EVENT_MAX)
  {
    ASSERT(0);
    return -1;
  }

  buf_ptr = (ps_ifacei_event_buf_type*)buf;

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in:
    1) buffer must be in the data section of the containing DSM item
    2) the kind field is set correctly
    3) the item pointer in the first slot points to the containing item
  -------------------------------------------------------------------------*/
  item_ptr = (dsm_item_type*)(buf_ptr->this_item);
  if(item_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

  buf_data = (uint8*)buf;
  if(item_ptr->kind != EVENT_BUF_ITEM_KIND                               ||
     buf_data < item_ptr->data_ptr                                       ||
     buf_data > (item_ptr->data_ptr + item_ptr->size - EVENT_BUF_SIZE)   ||
     ((ps_ifacei_event_buf_type*)item_ptr->data_ptr)->this_item != item_ptr)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

  /*-------------------------------------------------------------------------
    make sure that this is not already queued - if not mark it as such
  -------------------------------------------------------------------------*/
  if(Q_ALREADY_QUEUED(&(buf_ptr->link)))
  {
    MSG_HIGH("Not re-queuing buf 0x%x for event %d",
             buf,
             event, 0);
    PS_BRANCH_TASKFREE();
    return 0;
  }

  /*-------------------------------------------------------------------------
    As the contents of the buffer are supposed to be set by ps_iface
    functions it should NEVER have a NULL callback
  -------------------------------------------------------------------------*/
  if(buf_ptr->event_cback_f_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

  /*-------------------------------------------------------------------------
    is this for the global queue?
  -------------------------------------------------------------------------*/
  if(this_iface_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      If the global queue is not initialized, then initialize it.
    -----------------------------------------------------------------------*/
    if(global_event_q_initialized == FALSE)
    {
      global_event_q_initialized = TRUE;
      for(loop = 0; loop < IFACE_EVENT_MAX; loop++)
      {
        q_init(global_event_q_array + loop);
      }
    } /* if(not inited) */

    buf_ptr->event_q_ptr = global_event_q_array + event;
    q_put(global_event_q_array + event, &(buf_ptr->link));

  } /* if(global queue) */

  /*-------------------------------------------------------------------------
    if not the global queue then make sure that the interface is valid
  -------------------------------------------------------------------------*/
  else if(PS_IFACE_IS_VALID(this_iface_ptr) == TRUE)
  {
    buf_ptr->event_q_ptr = (this_iface_ptr->private.event_q_array) + event;
    q_put((this_iface_ptr->private.event_q_array) + event, &(buf_ptr->link));

    /*-----------------------------------------------------------------------
      check if the event is true - if so call the callback immediately
    -----------------------------------------------------------------------*/
    switch(event)
    {
      /*---------------------------------------------------------------------
                              IFACE_ENABLED_EV
      ---------------------------------------------------------------------*/
    case IFACE_ENABLED_EV:
      if(this_iface_ptr->private.state != IFACE_DISABLED)
      {
        event_info.state = this_iface_ptr->private.state;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                              IFACE_DISABLED_EV
      ---------------------------------------------------------------------*/
    case IFACE_DISABLED_EV:
      if(this_iface_ptr->private.state == IFACE_DISABLED)
      {
        event_info.state = IFACE_DISABLED;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                                IFACE_DOWN_EV
      ---------------------------------------------------------------------*/
    case IFACE_DOWN_EV:
      if(this_iface_ptr->private.state == IFACE_DOWN)
      {
        event_info.state = IFACE_DOWN;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                              IFACE_ROUTEABLE_EV
      ---------------------------------------------------------------------*/
    case IFACE_ROUTEABLE_EV:
      if(this_iface_ptr->private.state == IFACE_ROUTEABLE)
      {
        event_info.state = IFACE_ROUTEABLE;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                                 IFACE_UP_EV
      ---------------------------------------------------------------------*/
    case IFACE_UP_EV:
      if(this_iface_ptr->private.state == IFACE_UP)
      {
        event_info.state = IFACE_UP;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                           IFACE_PHYS_LINK_DOWN_EV
      ---------------------------------------------------------------------*/
    case IFACE_PHYS_LINK_DOWN_EV:
      if(this_iface_ptr->private.phys_link_state == PHYS_LINK_DOWN)
      {
        event_info.link_state = PHYS_LINK_DOWN;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                            IFACE_PHYS_LINK_UP_EV
      ---------------------------------------------------------------------*/
    case IFACE_PHYS_LINK_UP_EV:
      if(this_iface_ptr->private.phys_link_state == PHYS_LINK_UP)
      {
        event_info.link_state = PHYS_LINK_UP;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                            IFACE_FLOW_ENABLED_EV
      ---------------------------------------------------------------------*/
    case IFACE_FLOW_ENABLED_EV:
      if(this_iface_ptr->private.tx_flow_mask == 0)
      {
        event_info.flow_mask = 0;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                           IFACE_FLOW_DISABLED_EV
      ---------------------------------------------------------------------*/
    case IFACE_FLOW_DISABLED_EV:
      if(this_iface_ptr->private.tx_flow_mask != 0)
      {
        event_info.flow_mask = this_iface_ptr->private.tx_flow_mask;
        need_to_invoke_cback = TRUE;
      }
      break;

      /*---------------------------------------------------------------------
                           IFACE_IPFLTR_UPDATED_EV
                           
        Don't call this event callback during registration.  
      ---------------------------------------------------------------------*/
    case IFACE_IPFLTR_UPDATED_EV:
      break;

    /*---------------------------------------------------------------------
                         IFACE_DELETE_EV and default

       It does not make sence to call the delete event here because the if
       interface has been deleted, then it is not in use and can't have
       callback called on/by it.
     ---------------------------------------------------------------------*/
    case IFACE_DELETE_EV:
      ASSERT(0);

    default:
      /* do nothing */;

    } /* switch(event) */

    /*-----------------------------------------------------------------------
      If the callback needs to be called, do so
    -----------------------------------------------------------------------*/
    if(need_to_invoke_cback)
    {
      buf_ptr->event_cback_f_ptr(this_iface_ptr,
                                 event,
                                 event_info,
                                 buf_ptr->user_data_ptr);
    }
  } /* if(iface is valid) */
  else
  {
    ret_val = -1;
  }
  TASKFREE();

  return ret_val;

} /* ps_iface_event_cback_reg() */



/*===========================================================================
FUNCTION PS_IFACE_EVENT_CBACK_DEREG()

DESCRIPTION
  DeRegisters callbacks for a specific event on an interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  event:  Event that triggers the callbacks
  buf:    Buffer that holds the callback pointer. This buffer is removed
          from the queue of callbacks. Should not be NULL.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_event_cback_dereg
(
  ps_iface_type           *this_iface_ptr,
  ps_iface_event_enum_type event,
  void                    *buf
)
{
  q_type   *working_q_ptr = NULL;
  ps_ifacei_event_buf_type *buf_ptr;
  dsm_item_type *item_ptr;
  uint8 *buf_data;
  boolean quit = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event >= IFACE_EVENT_MAX ||
     buf == NULL)
  {
    ASSERT(0);
    return;
  }

  buf_ptr = (ps_ifacei_event_buf_type*)buf;

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in:
    1) buffer must be in the data section of the containing DSM item
    2) the kind field is set correctly
    3) the item pointer in the first slot points to the containing item
  -------------------------------------------------------------------------*/
  item_ptr = (dsm_item_type*)(buf_ptr->this_item);
  if(item_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  buf_data = (uint8*)buf;
  if(item_ptr->kind != EVENT_BUF_ITEM_KIND                               ||
     buf_data < item_ptr->data_ptr                                       ||
     buf_data > (item_ptr->data_ptr + item_ptr->size - EVENT_BUF_SIZE)   ||
     ((ps_ifacei_event_buf_type*)item_ptr->data_ptr)->this_item != item_ptr)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    make sure that this event is on a queue - if not mark it as such
  -------------------------------------------------------------------------*/
  if(Q_ALREADY_QUEUED(&(buf_ptr->link)) == FALSE)
  {
    MSG_MED("Buf 0x%x for event %d not on a queue",
            buf,
            event, 0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    As the contents of the buffer are supposed to be set by ps_iface
    functions it should NEVER have a NULL callback
  -------------------------------------------------------------------------*/
  if(buf_ptr->event_cback_f_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    If this registration is for global callbacks, then use the global
    variable as the working pointer, otherwise use the q from the iface
  -------------------------------------------------------------------------*/
  if(this_iface_ptr == NULL)
  {
    if(global_event_q_initialized == FALSE)
    {
      ASSERT(0);
      MSG_HIGH("global event q not inited!", 0, 0, 0);
      quit = TRUE;
    }
    working_q_ptr = global_event_q_array + event;
  }
  else if(PS_IFACE_IS_VALID(this_iface_ptr) == TRUE)
  {
    working_q_ptr = (this_iface_ptr->private.event_q_array) + event;
  }
  else
  {
    quit = TRUE;
  }

  /*-------------------------------------------------------------------------
    Make sure the event buf is either on an event queue of the specified 
    iface or on the gloabl event queue
  -------------------------------------------------------------------------*/
  if(working_q_ptr != buf_ptr->event_q_ptr)
  {
    ASSERT(0);
    MSG_ERROR("Buf 0x%x (ev %d) not reg 0x%x iface",
              buf, event, this_iface_ptr);
    quit = TRUE;
  }

  /*-------------------------------------------------------------------------
    make sure we have a valid queue to remove from - if so use the queue
    function to remove the item from the queue.
  -------------------------------------------------------------------------*/
  if(quit == FALSE)
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete(working_q_ptr, &(buf_ptr->link));
#else
    q_delete(&(buf_ptr->link));
#endif
  }
  TASKFREE();

  return;

} /* ps_iface_event_cback_dereg() */



/*===========================================================================
FUNCTION PS_IFACE_GET_HANDLE()

DESCRIPTION
  This function returns a ps_iface handle based on the interface name 
  and instance passed in.  If no such interfac exists, a value of NULL is 
  returned.

PARAMETERS
  Pointer to struct containing name and instance information.

RETURN VALUE
  NULL for failure
  ps_iface handle on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type *ps_iface_get_handle
(
  dss_iface_ioctl_name_enum_type name,
  uint32                         instance
)
{
  ps_iface_type *iface_ptr=NULL;
  int iface_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Search the global iface array to see if there is an iface with the same
    name & instance pair as passed in, if not return NULL.
  -------------------------------------------------------------------------*/
  for(iface_index=0; iface_index<MAX_SYSTEM_IFACES; iface_index++)
  {
    /*-----------------------------------------------------------------------
      Since ifaces are added sequentially and they are never deleted, as 
      soon as we get a NULL ptr there is no more iface pointers in the list.
    -----------------------------------------------------------------------*/
    if(global_iface_ptr_array[iface_index] == NULL)
    {
      break;
    }

    if(global_iface_ptr_array[iface_index]->name == name &&
       global_iface_ptr_array[iface_index]->instance == instance)
    {
      MSG_LOW("Found matching ps_iface 0x%x",iface_ptr,0,0);
      iface_ptr = global_iface_ptr_array[iface_index];
      break;
    }      
  }

  return iface_ptr;

} /* ps_iface_get_handle() */



/*===========================================================================
FUNCTION PS_IFACE_SET_BRIDGE()

DESCRIPTION
  This function will set the bridge pointer and inform the address server
  the bridging mode.  If the bridged_iface_ptr is NULL this will "un-bridge"
  the interface.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  bridged_iface_ptr: ptr to interface to bridge to.

RETURN VALUE
  None

DEPENDENCIES
  This should only ever be called by a client from a SINGLE task context.

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_bridge
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  MSG_MED("bridging 0x%x(%d) to 0x%x",
          this_iface_ptr,
          ps_iface_state(this_iface_ptr),
          target_iface_ptr);

  /*-------------------------------------------------------------------------
    Set the bridge pointer to the value passed in, and call the bridge
    processing function if there is one.
  -------------------------------------------------------------------------*/
  this_iface_ptr->private.tx_bridge_ptr = target_iface_ptr;

  if(this_iface_ptr->bridge_proc_f_ptr != NULL)
  {
    this_iface_ptr->bridge_proc_f_ptr(this_iface_ptr,
                                      target_iface_ptr,
                                      this_iface_ptr->client_data_ptr);
  }

  TASKFREE();

} /* ps_iface_set_bridge() */



/*===========================================================================
FUNCTION PS_IFACE_SET_TX_FUNCTION()

DESCRIPTION
  This function is used to set the TX function.  Unsetting the function is
  done by passing in NULL for the tx_cmd parameter.

PARAMETERS
  this_iface_ptr:  interface on which to operate
  tx_cmd:          Tx command being set
  tx_cmd_info:     Info to pass to registered cmd handler

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_tx_function
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_tx_cmd_f_ptr_type tx_cmd,
  void                      *tx_cmd_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    if the tx_cmd parameter is NULL then register the default command
  -------------------------------------------------------------------------*/
  else if(tx_cmd == NULL)
  {
    this_iface_ptr->private.tx_cmd = ps_ifacei_default_tx_cmd;
  }

  /*-------------------------------------------------------------------------
    Register the command
  -------------------------------------------------------------------------*/
  else
  {
    this_iface_ptr->private.tx_cmd = tx_cmd;
  }

  /*-------------------------------------------------------------------------
    Set the command info to whatever was passed in.
  -------------------------------------------------------------------------*/
  this_iface_ptr->private.tx_cmd_info = tx_cmd_info;
  TASKFREE();

} /* ps_iface_set_tx_function() */



/*===========================================================================
FUNCTION PS_IFACE_ENABLE_FLOW()

DESCRIPTION
  Interface user enables flow on the interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&event_info, 0, sizeof(event_info));

  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    store the previous flow mask in the event info variable, and remove the
    client mask from the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = this_iface_ptr->private.tx_flow_mask;
  DS_FLOW_CTRL_SET_MASK(DS_FLOW_ENABLE,
                        this_iface_ptr->private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being enabled
  -------------------------------------------------------------------------*/
  if(PS_IFACE_FLOW_ENABLED(this_iface_ptr) &&
     event_info.flow_mask != DS_FLOW_IS_ENABLED)
  {
    MSG_MED("client 0x%x enabling flow, mask 0x%x",
            flow_mask,
            this_iface_ptr->private.tx_flow_mask, 0);
    ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                  IFACE_FLOW_ENABLED_EV,
                                  event_info);
  }
  TASKFREE();

} /* ps_iface_enable_flow() */



/*===========================================================================
FUNCTION PS_IFACE_DISABLE_FLOW()

DESCRIPTION
  client disables flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disable_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
)
{
  ps_iface_event_info_u_type event_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&event_info, 0, sizeof(event_info));

  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    store the previous flow mask in the event info variable, and insert the
    client mask into the tx_flow_mask.
  -------------------------------------------------------------------------*/
  event_info.flow_mask = this_iface_ptr->private.tx_flow_mask;
  DS_FLOW_CTRL_SET_MASK(DS_FLOW_DISABLE,
                        this_iface_ptr->private.tx_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being disabled
  -------------------------------------------------------------------------*/
  if(event_info.flow_mask == DS_FLOW_IS_ENABLED &&
     this_iface_ptr->private.tx_flow_mask != DS_FLOW_IS_ENABLED)
  {
    MSG_MED("client 0x%x disabling flow mask 0x%x",
            flow_mask,
            this_iface_ptr->private.tx_flow_mask, 0);
    ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                  IFACE_FLOW_DISABLED_EV,
                                  event_info);
  }
  TASKFREE();

} /* ps_iface_disable_flow() */




/*===========================================================================
FUNCTION PS_IFACE_ENABLE_NON_SYS_SOCKETS_FLOW()

DESCRIPTION
  Interface user enables flow for non system sockets using the interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_non_sys_sockets_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Store the previous flow mask in the event info variable, and remove the
    client mask from the tx_flow_mask.
  -------------------------------------------------------------------------*/
  DS_FLOW_CTRL_SET_MASK(DS_FLOW_ENABLE,
                        this_iface_ptr->private.non_sys_sockets_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being enabled
  -------------------------------------------------------------------------*/
  if(PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED(this_iface_ptr))
  {
    MSG_MED("0x%x enabling non sys sock flow",
            flow_mask,0, 0);
  }
  TASKFREE();

} /* ps_iface_enable_non_sys_sockets_flow() */



/*===========================================================================
FUNCTION PS_IFACE_DISABLE_NON_SYS_SOCKETS_FLOW()

DESCRIPTION
  client disables flow for non system sockets on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  flow_mask: bit mask that identifies the caller.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disable_non_sys_sockets_flow
(
  ps_iface_type *this_iface_ptr,
  uint32         flow_mask
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Insert the client mask into the tx_flow_mask.
  -------------------------------------------------------------------------*/
  DS_FLOW_CTRL_SET_MASK(DS_FLOW_DISABLE,
                        this_iface_ptr->private.non_sys_sockets_flow_mask,
                        flow_mask);

  /*-------------------------------------------------------------------------
    only call the callback on the transition to the flow being disabled
  -------------------------------------------------------------------------*/
  if(this_iface_ptr->private.non_sys_sockets_flow_mask != DS_FLOW_IS_ENABLED)
  {
    MSG_MED("0x%x disabling non sys sockets flow",
            flow_mask,0,0);
    MSG_MED("Non sys sockets mask 0x%x",
            this_iface_ptr->private.non_sys_sockets_flow_mask, 0, 0);
  }
  TASKFREE();

} /* ps_iface_disable_non_sys_sockets_flow() */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                       IP ADDRESS INTERFACE FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_IFACE_SET_ADDR()

DESCRIPTION
  This function is used to set the local address - this should only be called
  by the addr_event() function - DO NOT TELL the address server - the caller
  is responsible for that.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr: address to set.

RETURN VALUE
  0 if sucessful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_set_addr
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE ||
     ip_addr_ptr == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

  switch(ip_addr_ptr->type)
  {
  case IP_ADDR_INVALID:
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

  case IPV4_ADDR:
    /*-----------------------------------------------------------------------
      if the address has changed and we are in the UP state - call the
      address changed callbacks
    -----------------------------------------------------------------------*/
    if(this_iface_ptr->private.ipv4_addr != ip_addr_ptr->addr.v4 &&
       (ps_iface_state(this_iface_ptr) == IFACE_UP ||
        ps_iface_state(this_iface_ptr) == IFACE_ROUTEABLE))
    {
      event_info.ip_addr.addr.v4 = this_iface_ptr->private.ipv4_addr;
      this_iface_ptr->private.ipv4_addr = ip_addr_ptr->addr.v4;
      event_info.ip_addr.type = IPV4_ADDR;
      ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                    IFACE_ADDR_CHANGED_EV,
                                    event_info);
    }
    /*-----------------------------------------------------------------------
      Assign the the address to the interface
    -----------------------------------------------------------------------*/
    this_iface_ptr->private.ipv4_addr = ip_addr_ptr->addr.v4;
    break;

  default:
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  } /* switch(address type) */
  TASKFREE();

  return 0;

} /* ps_iface_set_addr() */



/*===========================================================================
FUNCTION PS_IFACE_GET_ADDR()

DESCRIPTION
  This function is used to get the local address of the passed in interface.
  If on interface type is specified in the ip_addr_ptr, either v4 or v6 will
  be returned depending on which one is set (v4 will always be returned if it
  is set).

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_addr
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE ||
     ip_addr_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    The address is only valid if the state of the interface is UP - in all
    other cases return invalid address.
  -------------------------------------------------------------------------*/
  switch(this_iface_ptr->private.state)
  {
    case IFACE_DISABLED:
    case IFACE_DOWN:
    case IFACE_COMING_UP:
    case IFACE_GOING_DOWN:
      /*---------------------------------------------------------------------
        Return the invalid address, the address is invalid in this state
      ---------------------------------------------------------------------*/
      ip_addr_ptr->type = IP_ADDR_INVALID;
      ip_addr_ptr->addr.v4 = 0;
      break;

    case IFACE_ROUTEABLE:
    case IFACE_UP:
      /*---------------------------------------------------------------------
        Get the address.
      ---------------------------------------------------------------------*/
      ps_iface_get_cached_addr(this_iface_ptr, ip_addr_ptr);
      break;

  default:
    ASSERT(0);
  } /* switch(iface state) */

} /* ps_iface_get_addr() */



/*===========================================================================
FUNCTION PS_IFACE_GET_CACHED_ADDR()

DESCRIPTION
  This function will return the address that is stored in the requested IP
  address field of the interface regardless of the state.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be  will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_cached_addr
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  switch(ip_addr_ptr->type)
  {
  case IP_ANY_ADDR:
    /*-----------------------------------------------------------------------
      If the caller didn't specify an address type, the v4 address will be
      returned if set otherwise the v6 address will be returned if it is set
    -----------------------------------------------------------------------*/
    if(this_iface_ptr->private.ipv4_addr != 0)
    {
      ip_addr_ptr->type = IPV4_ADDR;
      ip_addr_ptr->addr.v4 = this_iface_ptr->private.ipv4_addr;
      break;
    }
    /* fall through */

  case IPV6_ADDR:
    /*-----------------------------------------------------------------------
      return the invalid address, as we don't currently support v6
    -----------------------------------------------------------------------*/
    ip_addr_ptr->type = IP_ADDR_INVALID;
    break;

  case IPV4_ADDR:
    /*-----------------------------------------------------------------------
      return the v4 address
    -----------------------------------------------------------------------*/
    ip_addr_ptr->addr.v4 = this_iface_ptr->private.ipv4_addr;
    break;

  default:
    ASSERT(0);

  } /* switch(requested address type) */

  TASKFREE();

} /* ps_iface_get_cached_addr() */




/*===========================================================================
FUNCTION PS_IFACE_SET_PEER_DATA()

DESCRIPTION
  This function is used by the link layer associated with an interface to set
  its peer information associated with the passed in interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  addr_cmd_f_ptr: function pointer to function for retrieving peer IP address
                  information.
  link_info: any link specific information that needs to be passed to the CB

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_set_peer_data
(
  ps_iface_type                 *this_iface_ptr,
  ps_iface_addr_cmd_f_ptr_type   get_addr_cmd_f_ptr,
  void                          *link_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Store the passed in peer information
  -------------------------------------------------------------------------*/
  this_iface_ptr->private.peer_data.get_addr_cmd_f_ptr = get_addr_cmd_f_ptr;
  this_iface_ptr->private.peer_data.link_info          = link_info;
  TASKFREE();

} /* ps_iface_set_peer_data() */



/*===========================================================================
FUNCTION PS_IFACE_GET_PEER_ADDR()

DESCRIPTION
  This function will return the address of the interfaces peer (e.g. the
  GGSN, or the PDSN) if this is a point-to-point link.  If the interface
  isn't point-to-point the invalid address will be returned.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_get_peer_addr
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE ||
     ip_addr_ptr                       == NULL)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    If the peer_address function is registered call it, otherwise return the
   invalid address.
   -----------------------------------------------------------------------*/
  if(this_iface_ptr->private.peer_data.get_addr_cmd_f_ptr != NULL)
  {
    this_iface_ptr->private.peer_data.get_addr_cmd_f_ptr(
      this_iface_ptr,
      ip_addr_ptr,
      this_iface_ptr->private.peer_data.link_info);
  }
  else
  {
    ip_addr_ptr->type = IP_ADDR_INVALID;
  }
  TASKFREE();

} /* ps_iface_get_peer_addr() */



#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                  COMMANDS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_IFACE_BRING_UP_CMD()

DESCRIPTION
  This function is used to bring up an iface.  Will call the registered
  callback if the interface is in the down state.  If it is in the up state,
  it returns success.  If the interface is in the coming state or down it
  return DS_EWOULDBLOCK.  Finally if it is in the going down state, it
  returns DS_ECLOSEINPROGRESS.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno:       value to return error codes
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_bring_up_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
)
{
  int ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    *ps_errno = DS_EINVAL;
    return -1;
  }

  MSG_MED("bringing up iface %d:%d in state %d",
          this_iface_ptr->name,
          this_iface_ptr->instance,
          ps_iface_state(this_iface_ptr));

  /*-------------------------------------------------------------------------
    take action based on the current state.
  -------------------------------------------------------------------------*/
  switch(this_iface_ptr->private.state)
  {
  case IFACE_UP:
    /*-----------------------------------------------------------------------
      IFACE_UP: nothing to do, as we are already up.
    -----------------------------------------------------------------------*/
    ret_val = 0;
    break;

  case IFACE_DOWN:
    // should we have default values for DNS?  If so then copy them to the
    // net info DNS slots here here.
    if(this_iface_ptr->bring_up_cmd_f_ptr != NULL)
    {
      this_iface_ptr->private.state = IFACE_COMING_UP;
      if(this_iface_ptr->bring_up_cmd_f_ptr(this_iface_ptr,
                                            client_data_ptr) < 0)
      {
        *ps_errno = DS_ENETNONET;
        ps_iface_down_ind(this_iface_ptr);
      }
      else
      {
        *ps_errno = DS_EWOULDBLOCK;
      }
    }
    else
    {
      *ps_errno = DS_EOPNOTSUPP;
    }
    ret_val = -1;
    break;

  case IFACE_COMING_UP:
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  case IFACE_GOING_DOWN:
    *ps_errno = DS_ENETCLOSEINPROGRESS;
    ret_val = -1;
    break;

  case IFACE_ROUTEABLE:
    MSG_MED("Interface is routeable, cannot be brought up!",0,0,0);
    *ps_errno = DS_ENETDOWN;
    ret_val = -1;
    break;

  default:
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */

  TASKFREE();

  return ret_val;

} /* ps_iface_bring_up_cmd() */



/*===========================================================================
FUNCTION PS_IFACE_TEAR_DOWN_CMD()

DESCRIPTION
  This function is used to stop an iface.  It

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
)
{
  int ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    *ps_errno = DS_EINVAL;
    return -1;
  }

  MSG_MED ("Tear down if %x (state = %d)", 
           this_iface_ptr, this_iface_ptr->private.state, 0);

  /*-------------------------------------------------------------------------
    take action based on the current state
  -------------------------------------------------------------------------*/
  switch(this_iface_ptr->private.state)
  {
  case IFACE_DOWN:
    ret_val = 0;
    break;

  case IFACE_ROUTEABLE:
  case IFACE_UP:
  case IFACE_COMING_UP:
    if(this_iface_ptr->tear_down_cmd_f_ptr == NULL)
    {
      MSG_ERROR("No tear down command",0,0,0);
      ret_val = 0;
      break;
    }
    this_iface_ptr->private.state = IFACE_GOING_DOWN;
    this_iface_ptr->tear_down_cmd_f_ptr(this_iface_ptr, client_data_ptr);
    /* fall through */

  case IFACE_GOING_DOWN:
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  default:
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */

  TASKFREE();

  return ret_val;

} /* ps_iface_tear_down_cmd() */



/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_CMD()

DESCRIPTION
  This function is used to bring up the physical link on an iface, but it
  will only do so if the interface is not in the down or disabled state.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_phys_link_up_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
)
{
  int ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    If the interface is down or disabled this is not a meaningful command
  -------------------------------------------------------------------------*/
  if(ps_iface_state(this_iface_ptr) == IFACE_DOWN ||
     ps_iface_state(this_iface_ptr) == IFACE_DISABLED)
  {
    MSG_HIGH("iface(0x%x) in invalid state %d",
             this_iface_ptr,
             ps_iface_state(this_iface_ptr), 0);
    *ps_errno = DS_EINVAL; // MVL: enotallowed; needs to be added?
    return -1;
  }

  /*-------------------------------------------------------------------------
    take action depending on the physical link state
  -------------------------------------------------------------------------*/
  switch(this_iface_ptr->private.phys_link_state)
  {
  case PHYS_LINK_UP:
    /*-----------------------------------------------------------------------
      Physical link is up: return success
    -----------------------------------------------------------------------*/
    ret_val = 0;
    break;

  case PHYS_LINK_COMING_UP:
    /*-----------------------------------------------------------------------
      Physical link is coming up: return would block
    -----------------------------------------------------------------------*/
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  case PHYS_LINK_DOWN:
    /*-----------------------------------------------------------------------
      Physical link is down: if the bring up hanler is registered set the
      state to coming up and call it.
    -----------------------------------------------------------------------*/
    if(this_iface_ptr->phys_link_up_cmd_f_ptr != NULL)
    {
      this_iface_ptr->private.phys_link_state = PHYS_LINK_COMING_UP;
      
      if(ps_iface_state(this_iface_ptr) == IFACE_UP ||
         ps_iface_state(this_iface_ptr) == IFACE_ROUTEABLE)
      {      
        ps_iface_disable_flow(this_iface_ptr, DS_FLOW_PS_IFACE_MASK);
      }

      if(this_iface_ptr->phys_link_up_cmd_f_ptr(this_iface_ptr,
                                                client_data_ptr) < 0)
      {
        /*-------------------------------------------------------------------
          can't bring up the network for some reason, so return net no net
          and set the state to down
        -------------------------------------------------------------------*/
        *ps_errno = DS_ENETNONET;
        ps_iface_phys_link_down_ind(this_iface_ptr);
      }
      else
      {
        /*-------------------------------------------------------------------
          bring up is in progress so return would block
        -------------------------------------------------------------------*/
        *ps_errno = DS_EWOULDBLOCK;
      }
    } /* if(f_ptr registered) */

    /*-----------------------------------------------------------------------
      The handler is not registered, so return option not supported
    -----------------------------------------------------------------------*/
    else
    {
      *ps_errno = DS_EOPNOTSUPP;
    }
    ret_val = -1;
    break;

  case PHYS_LINK_GOING_DOWN:
    /*-----------------------------------------------------------------------
      Physical link is going down: return net close in progress
    -----------------------------------------------------------------------*/
    *ps_errno = DS_ENETCLOSEINPROGRESS;
    ret_val = -1;
    break;

  default:
    /*-----------------------------------------------------------------------
      This should NEVER happen!
    -----------------------------------------------------------------------*/
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */
  TASKFREE();

  return ret_val;

} /* ps_iface_phys_link_up_cmd() */



/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_CMD()

DESCRIPTION
  This function is used to tear down the physical link of an iface, but it
  only does so if the interface is not down or disabled.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_phys_link_down_cmd
(
  ps_iface_type *this_iface_ptr,
  int16         *ps_errno,
  void          *client_data_ptr
)
{
  int ret_val = -1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*-------------------------------------------------------------------------
    If the interface is down or disabled this is not a meaningful command
  -------------------------------------------------------------------------*/
  if(ps_iface_state(this_iface_ptr) == IFACE_DOWN ||
     ps_iface_state(this_iface_ptr) == IFACE_DISABLED)
  {
    MSG_ERROR("iface(0x%x) in invalid state %d to bring down",
              this_iface_ptr,
              ps_iface_state(this_iface_ptr), 0);
    ASSERT(0);
    *ps_errno = DS_EINVAL; // MVL: enotallowed; needs to be added?
    PS_BRANCH_TASKFREE();
    return -1;
  }

  switch(this_iface_ptr->private.phys_link_state)
  {
  case PHYS_LINK_DOWN:
    ret_val = 0;
    break;

  case PHYS_LINK_UP:
  case PHYS_LINK_COMING_UP:
    if(this_iface_ptr->phys_link_down_cmd_f_ptr != NULL)
    {
      this_iface_ptr->private.phys_link_state = PHYS_LINK_GOING_DOWN;
      ps_iface_disable_flow(this_iface_ptr, DS_FLOW_PS_IFACE_MASK);
      this_iface_ptr->phys_link_down_cmd_f_ptr(this_iface_ptr,
                                               client_data_ptr);
    }
    else
    {
      ret_val = 0;
    }
    /* fall through */

  case PHYS_LINK_GOING_DOWN:
    *ps_errno = DS_EWOULDBLOCK;
    ret_val = -1;
    break;

  default:
    ASSERT(0);
    *ps_errno = DS_EOPNOTSUPP;
    ret_val = -1;
    break;

  } /* switch(state) */
  TASKFREE();

  return ret_val;

} /* ps_iface_phys_link_down_cmd() */



/*===========================================================================
FUNCTION PS_IFACE_TX_CMD()

DESCRIPTION
  This function will transmit to the iface specified - it checks if the state
  is UP or ROUTEABLE, if it isn't then the interface must be a route on demand
  interface, if it is not, then the default tx functions is called (which
  frees the memory).

  Note: this function does not pay attention to the flow control state of the
        interface.  The client callback must do this.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received for tx
  meta_info_ref_ptr: ref to meta info ptr (data related to pkt)

RETURN VALUE
  0: if the packet was transmitted
 -1: if not

DEPENDENCIES
  None

SIDE EFFECTS
  The memory that was passed in will be freed by one of the tx functions.
===========================================================================*/
int ps_iface_tx_cmd
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr
)
{
  ps_iface_tx_cmd_f_ptr_type tx_cmd;
  void                       *tx_cmd_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return -1;
  }

  tx_cmd = this_iface_ptr->private.tx_cmd;
  tx_cmd_info = this_iface_ptr->private.tx_cmd_info;

  /*-------------------------------------------------------------------------
    If the interface is ROUTEABLE or UP call the TX function
  -------------------------------------------------------------------------*/
  if(ps_iface_state(this_iface_ptr) == IFACE_UP       ||
     ps_iface_state(this_iface_ptr) == IFACE_ROUTEABLE ||
     ps_iface_state(this_iface_ptr) == IFACE_GOING_DOWN
    )
  {
    PS_BRANCH_TASKFREE();
    return tx_cmd(this_iface_ptr,
                  pkt_ref_ptr,
                  meta_info_ref_ptr,
                  tx_cmd_info);
  }

  /*-------------------------------------------------------------------------
    Otherwise if the interface supports route on demand call the TX function
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

  /*-------------------------------------------------------------------------
    otherwise call the default TX function which will free all of the memory
  -------------------------------------------------------------------------*/
  TASKFREE();
  return ps_ifacei_default_tx_cmd(this_iface_ptr,
                                      pkt_ref_ptr,
                                      meta_info_ref_ptr,
                                      NULL);
} /* ps_iface_tx_cmd() */



/*===========================================================================
FUNCTION PS_IFACE_INPUT()

DESCRIPTION
   Interface client gives the protocol stack a packet to transmit.

PARAMETERS
  this_iface_ptr:    ptr to interface control block on which to operate on.
  pkt_ref_ptr:       ref to dsm item ptr received
  meta_info_ref_ptr: ref to meta info ptr (data related to pkt)

RETURN VALUE
  0: data was transfered
 -1: data was not transferred, but memory was freed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_input
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr
)
{
  ps_iface_type      *bridge_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    MSG_HIGH("rcvd data on invalid iface(0x%x) freeing!",
             this_iface_ptr, 0, 0);
    dsm_free_packet(pkt_ref_ptr);
    PS_META_INFO_FREE(meta_info_ref_ptr);
    PS_BRANCH_TASKFREE();
    return -1;
  }

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

  /*-------------------------------------------------------------------------
    Set the kind field so that if forwarded the type of the packet is known.
    If the bridge pointer is set, then forward it to the bridged interface.
  -------------------------------------------------------------------------*/
  (*pkt_ref_ptr)->kind = DSM_PS_IP_COMPRESS;

  if(ps_iface_bridge_iface(this_iface_ptr) != NULL)
  {
    bridge_iface_ptr = ps_iface_bridge_iface(this_iface_ptr);
    PS_BRANCH_TASKFREE();
    return ps_iface_tx_cmd(bridge_iface_ptr, pkt_ref_ptr, NULL);
  }

  /*-------------------------------------------------------------------------
    If you make it here then it suffices to call ip_input as it will handle
    the local check, and the bridging.
  -------------------------------------------------------------------------*/
  TASKFREE();
  return ip_input( this_iface_ptr, *pkt_ref_ptr, NULL, FALSE );

} /* ps_iface_input() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                                 INDICATIONS

  These are intended to only be called by the interface clents/owners.  While
  there is no way to enforce this, having someone else call the indications
  is not supported behavior.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACE_ENABLE_IND()

DESCRIPTION
  The given interface has been enabled.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_enable_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  MSG_MED("Enabling interface %d:%d",
          this_iface_ptr->name,
          this_iface_ptr->instance, 0);

  event_info.state = this_iface_ptr->private.state;
  switch(event_info.state)
  {
  case IFACE_DISABLED:
    this_iface_ptr->private.state = IFACE_DOWN;
    break;

  case IFACE_DOWN:
  case IFACE_COMING_UP:
  case IFACE_ROUTEABLE:
  case IFACE_UP:
  case IFACE_GOING_DOWN:
    MSG_MED("Interface %d:%d already enabled",
            this_iface_ptr->name,
            this_iface_ptr->instance, 0);

  } /* switch(state) */

  /*-------------------------------------------------------------------------
    Call callbacks always - event if state hasn't changed.  This should be
    moved into the if() above if callbacks are only executed on state changes
  -------------------------------------------------------------------------*/
  ps_ifacei_invoke_event_cbacks(this_iface_ptr,IFACE_ENABLED_EV,event_info);
  TASKFREE();

} /* ps_iface_enable_ind() */



/*===========================================================================
FUNCTION PS_IFACE_DISABLED_IND()

DESCRIPTION
  The given interface has been disabled, if the interface is in any state
  other than DOWN then force the state DOWN first.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_disabled_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  MSG_MED("disabling iface %d:%d in state %d",
          this_iface_ptr->name,
          this_iface_ptr->instance,
          ps_iface_state(this_iface_ptr));

  /*-------------------------------------------------------------------------
    copy the current state information into the event_info varible, then
    process based on this state.
  -------------------------------------------------------------------------*/
  event_info.state = ps_iface_state(this_iface_ptr);
  switch(event_info.state)
  {
  case IFACE_ROUTEABLE:
  case IFACE_UP:
  case IFACE_COMING_UP:
  case IFACE_GOING_DOWN:
    /*-----------------------------------------------------------------------
      If we are in any state other than down we want to call the down
      indication - then change the state for the interface.
    -----------------------------------------------------------------------*/
    ps_iface_down_ind(this_iface_ptr);
    /* fall through */

  case IFACE_DOWN:
    this_iface_ptr->private.state = IFACE_DISABLED;
    /* fall thru */

  case IFACE_DISABLED:
    ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                  IFACE_DISABLED_EV,
                                  event_info);
    break;

  default:
    ASSERT(0);
  } /* switch(state) */

  TASKFREE();

} /* ps_iface_disabled_ind() */



/*===========================================================================
FUNCTION PS_IFACE_DOWN_IND()

DESCRIPTION
  The given interface has been stopped.   Call callbacks.  We set the address
  to zero (this allows the address server to OK the use of this address by
  someone else) and inform the address server.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  switch(ps_iface_state(this_iface_ptr))
  {
  case IFACE_DOWN:      
  case IFACE_COMING_UP: 
  case IFACE_ROUTEABLE: 
  case IFACE_UP:        
  case IFACE_GOING_DOWN:
    /*-----------------------------------------------------------------------
      reset all of the IP configuration
    -----------------------------------------------------------------------*/
    this_iface_ptr->private.ipv4_addr = 0;
    this_iface_ptr->v4_net_info.mtu = DEFAULT_DATA_UNIT;
    this_iface_ptr->v4_net_info.mru = DEFAULT_DATA_UNIT;
    this_iface_ptr->v4_net_info.gateway = 0;
    this_iface_ptr->v4_net_info.primary_dns = 0;
    this_iface_ptr->v4_net_info.secondary_dns = 0;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

   /*-----------------------------------------------------------------------
     Store the previous state, change the current state to DOWN and call the
     callbacks.
    -----------------------------------------------------------------------*/
    event_info.state = this_iface_ptr->private.state;
    this_iface_ptr->private.state = IFACE_DOWN;
    ps_ifacei_invoke_event_cbacks(this_iface_ptr, IFACE_DOWN_EV, event_info);

    break;

  case IFACE_DISABLED:  
    MSG_ERROR("Cannot move disabled iface %d:%d to down",
              this_iface_ptr->name,
              this_iface_ptr->instance, 0);

    break;

  default:
    ASSERT(0);
  } /* switch(iface_state) */

  TASKFREE();

} /* ps_iface_down_ind() */



/*===========================================================================
FUNCTION PS_IFACE_ROUTEABLE_IND()

DESCRIPTION
  The given interface has been set to routeable. The interface needs to be in
  the UP state before this indication is called.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
// do a switch on all states: if disabled fail
void ps_iface_routeable_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Store the old state, set the new state to routeable and call the callbacks
  -------------------------------------------------------------------------*/
  event_info.state = this_iface_ptr->private.state;
  this_iface_ptr->private.state = IFACE_ROUTEABLE;
  MSG_MED ("Calling event cbacks",0,0,0);
  ps_ifacei_invoke_event_cbacks(this_iface_ptr, 
                                IFACE_ROUTEABLE_EV, 
                                event_info);
  TASKFREE();

} /* ps_iface_routeable_ind() */



/*===========================================================================
FUNCTION PS_IFACE_UP_IND()

DESCRIPTION
  The given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
// do a switch on all states: if disabled fail
void ps_iface_up_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    store the previous state, change the current state to UP and call the
    callbacks.
  -------------------------------------------------------------------------*/
  event_info.state = this_iface_ptr->private.state;
  this_iface_ptr->private.state = IFACE_UP;
  ps_ifacei_invoke_event_cbacks(this_iface_ptr, IFACE_UP_EV, event_info);
  TASKFREE();

} /* ps_iface_up_ind() */



/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_IND()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
// do a switch on all phys link states
void ps_iface_phys_link_down_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Enable the flow that was disabled in coming_up/going_down
  -------------------------------------------------------------------------*/
  ps_iface_enable_flow(this_iface_ptr, DS_FLOW_PS_IFACE_MASK);

  /*-------------------------------------------------------------------------
    store the previous state, change the current link state to DOWN and call
    the callbacks.
  -------------------------------------------------------------------------*/
  event_info.link_state = this_iface_ptr->private.phys_link_state;
  this_iface_ptr->private.phys_link_state = PHYS_LINK_DOWN;
  ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                IFACE_PHYS_LINK_DOWN_EV,
                                event_info);
  TASKFREE();

} /* ps_iface_phys_link_down_ind() */



/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_IND()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
// do a switch on all phys link states
void ps_iface_phys_link_up_ind
(
  ps_iface_type *this_iface_ptr
)
{
  ps_iface_event_info_u_type event_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  TASKLOCK();
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    This indication should not be called if the interface is down or disabled
  -------------------------------------------------------------------------*/
  if(ps_iface_state(this_iface_ptr) == IFACE_DOWN ||
     ps_iface_state(this_iface_ptr) == IFACE_DISABLED)
  {
    ASSERT(0);
    PS_BRANCH_TASKFREE();
    return;
  }

  /*-------------------------------------------------------------------------
    Enable the flow that was disabled in coming_up/going_down
  -------------------------------------------------------------------------*/
  ps_iface_enable_flow(this_iface_ptr, DS_FLOW_PS_IFACE_MASK);

  /*-------------------------------------------------------------------------
    store the previous state, change the current link state to UP and call
    the callbacks.
  -------------------------------------------------------------------------*/
  event_info.link_state = this_iface_ptr->private.phys_link_state;
  this_iface_ptr->private.phys_link_state = PHYS_LINK_UP;
  ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                IFACE_PHYS_LINK_UP_EV,
                                event_info);
  TASKFREE();

} /* ps_iface_phys_link_up_ind() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_UPDATED_IND()

DESCRIPTION
  This fucntion is called to indicate that the number of IP filters 
  instaled in this iface have changed. Appropriate event callback
  is posted.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  n_added: number of new filters added
  n_deleted: number of existing filters deleted

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_ipfltr_updated_ind
(
  ps_iface_type *this_iface_ptr,
  uint8 n_added,
  uint8 n_deleted
)
{
  ps_iface_event_info_u_type event_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    return;
  }

  TASKLOCK();
  /*-------------------------------------------------------------------------
    Call the event callback passing the currently installed num ip filters
  -------------------------------------------------------------------------*/

  if(n_deleted > this_iface_ptr->private.ip_filter_info.n_filters)
  {
    MSG_ERROR("Deleting %d filters, total %d", 
              n_deleted, 
              this_iface_ptr->private.ip_filter_info.n_filters,
              0);
    ASSERT(0);
  }
  else
  {
    event_info.n_filters = this_iface_ptr->private.ip_filter_info.n_filters;
    this_iface_ptr->private.ip_filter_info.n_filters -= n_deleted; 
    this_iface_ptr->private.ip_filter_info.n_filters += n_added;

    MSG_MED("if 0x%x, n_filters %d -> %d", 
            this_iface_ptr, 
            event_info.n_filters,
            this_iface_ptr->private.ip_filter_info.n_filters);

    ps_ifacei_invoke_event_cbacks(this_iface_ptr,
                                  IFACE_IPFLTR_UPDATED_EV,
                                  event_info);
  }
  TASKFREE();

} /* ps_iface_ipfltr_updated_ind() */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PS_IFACEI_INVOKE_EVENT_CBACKS()

DESCRIPTION
  This function will invoke all of the event callbacks for a given interface
  and event.  It will also call the global callbacks for said event, if any.

PARAMETERS
  this_iface_ptr: ptr to the interface on which we are operating
  event: the event for which callbacks need to be called
  event_info: event information that needs to be passed into callback.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_invoke_event_cbacks
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info
)
{
  ps_ifacei_event_buf_type *event_buf_ptr;
  q_type *working_q_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    return;
  }

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

  /*-------------------------------------------------------------------------
    Start with the local callbacks
  -------------------------------------------------------------------------*/
  working_q_ptr = (this_iface_ptr->private.event_q_array) + event;

  for(;;)
  {
    /*-----------------------------------------------------------------------
      traverse the queue of callbacks for this event
    -----------------------------------------------------------------------*/
    event_buf_ptr = q_check(working_q_ptr);

    while(event_buf_ptr != NULL)
    {
      /*---------------------------------------------------------------------
        Call the callback
      ---------------------------------------------------------------------*/
      event_buf_ptr->event_cback_f_ptr(this_iface_ptr,
                                       event,
                                       event_info,
                                       event_buf_ptr->user_data_ptr);
      /*---------------------------------------------------------------------
        Get the next element in the queue
      ---------------------------------------------------------------------*/
      event_buf_ptr = q_next(working_q_ptr, &(event_buf_ptr->link));

    } /* while(items in queue) */

    /*-----------------------------------------------------------------------
      If the working_q_ptr is set to qlobal_event_q_array then we are done,
      otherwise we need to call those callbacks - but only if the global
      event q has been initialized.
    -----------------------------------------------------------------------*/
    if(working_q_ptr == global_event_q_array + event ||
       global_event_q_initialized == FALSE)
    {
      break;
    }
    else
    {
      working_q_ptr = global_event_q_array + event;
    }
  } /* for(all iface and global callbacks) */

} /* ps_ifacei_invoke_event_cbacks() */



/*===========================================================================
FUNCTION PS_IFACEI_DEFAULT_TX_CMD()

DESCRIPTION
  This function is the default TX function.  It is assigned to the tx
  function pointer when no explicit TX function is registered.

  It frees the data passed in.

PARAMETERS
  this_iface_ptr:    interface on which this is being called
  pkt_ref_ptr:       ref to dsm item ptr for Tx - freed
  meta_info_ref_ptr: ref to meta info ptr - also freed.
  tx_cmd_info:       ignored

RETURN VALUE
  -1: indicating error

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ifacei_default_tx_cmd
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_ref_ptr,
  ps_meta_info_type **meta_info_ref_ptr,
  void               *tx_cmd_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    free the meta information, and the dsm packet
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(meta_info_ref_ptr);
  dsm_free_packet(pkt_ref_ptr);

  return -1;

} /* ps_ifacei_default_tx_cmd() */


/*===========================================================================
FUNCTION PS_IFACE_SDB_STATUS_HANDLER()

DESCRIPTION
  This fucntion is used to convey status of SDB transmission from
  mode-handler to the ps_iface. ps_iface, in turn, calls a handler function
  registered with it by the sockets layer.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate.
  sock_id:        Socket through which SDB had been sent.
  cm_info_ptr:    ptr to an struct that contains status of SDB transmission.
                  The ptr to struct is typecast to void* by the mode handler
                  before calling this function so that ps_iface is not aware
                  of what is being passed. The socket layer typecasts the 
                  ptr to the right type when notifying the application.


RETURN VALUE
   0 success
  -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_sdb_status_handler
(
  ps_iface_type *this_iface_ptr,
  sint15        sock_id,
  void*         cm_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(PS_IFACE_IS_VALID(this_iface_ptr) == FALSE)
  {
    ASSERT(0);
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Call the function registered by the socket layer to handle the SDB status 
  -------------------------------------------------------------------------*/
  if (this_iface_ptr->private.sdb_status_handler_sock_fcn != NULL)
  {
    MSG_LOW("Calling handler for iface=0x%x, func=0x%x", 
            this_iface_ptr, 
            this_iface_ptr->private.sdb_status_handler_sock_fcn, 
            0);
    this_iface_ptr->private.sdb_status_handler_sock_fcn(
      sock_id, 
      (void *) cm_info_ptr); 
  }

  return 0;
} /* ps_iface_sdb_status_handler() */


#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */
#endif /* FEATURE_DATA */

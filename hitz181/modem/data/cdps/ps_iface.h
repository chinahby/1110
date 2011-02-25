#ifndef PS_IFACE_H
#define PS_IFACE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ I F A C E . H

DESCRIPTION
  Header file defining all of the data types and the interface control block
  for the ps_iface architecture.

HOW THE INTERFACE IS TO BE USED:
  There are three types of functions here:
  configuration (e.g. ps_iface_event_cback_reg())
  commands      (e.g. ps_iface_bring_up_cmd())
  indications   (e.g. ps_iface_up_ind())

  COMMANDS can be called by anyone: the intention is that they are to be used
  to request something of an interface (such as bringing it up).
  INDICATIONS and CONFIGURATION functions are intended to be called
  exclusively by the client/owner of the interface, so for example a mode
  specific handler should be the only person calling the up indication - and
  this should be done when the interface is up (e.g. for UMTS the PDP context
  is established, and PPP is established - for PDP Type PPP).

DEPENDENCIES
  None of these functions should EVER be called from Interrupt context!

EXTERNALIZED FUNCTIONS

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface.h_v   1.18   12 Feb 2003 19:35:58   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/29/03    ss     Added a field/macro for sdb ack status handler function. 
07/07/03    atp    Added ps_iface_sdb_status_handler() as part of SDB
                   enhancements to pass status notification to application.
05/05/03    aku    Moved MAX_SYSTEM_IFACES definition to ps_iface_defs.h and 
                   replaced ps_iface_ioctl_name_enum_type with 
                   dss_iface_ioctl_name_enum_type in ps_iface_get_handle().
05/02/03    usb    Removed instance param from ps_iface_create().
04/11/03    ak     Increased number of possible PS_IFACES for GPRS+1X builds.
02/10/03    aku    Added support for flow control of non system sockets.
02/12/03    aku    Changed errnos to use ones in defined in dserrno.h in 
                   dss_iface_ioctl() 
02/11/03    usb    Added macro to access num filters in the iface, changed
                   ps_iface_ipfltr_updated_ind() prototye
02/07/03    usb    Added IP filter info to iface_cb, added event 
                   notification for IFACE_IPFLTR_UPDATED_EV.
01/17/03    atp    Added extra argument ifaceptr to ps_iface_ioctl_f_ptr_type
12/24/02    aku    Added ps_iface_get_handle(). Moved 
                   ps_iface_event_enum_type to ps_iface_defs.h. Added support
                   for global_iface_array.
11/15/02    mvl    Added physical link statistics.
11/10/02    aku    Added support for ps_iface_ioctl()
11/01/02    mvl    moved some common elements to ps_iface_defs.h file.
09/17/02    mvl    Added cookie to iface_cb.  Added the bridge processing
                   call.
09/04/02    mvl    changed memory handling for event callback buffers.
09/03/02    mvl    general cleanup.  Added prototypes for alloc and free_buf.
08/13/02    mvl    updated ps_iface state macros for name change
07/27/02  mvl/om   changed ip_addr.h include and added address macro.
07/15/02    mvl    More IP addressing functionality.
07/11/02    mvl    Added the PS_IFACE_IS_VALID() macro from C file.  Updates
                   for new PPP architecture.
07/01/02    mvl    Added ps_iface_get_flow_mask() to return ifaces flow mask.
05/14/02    mvl    initial compile and checkin
03/15/02   na/mvl  created file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "dsm.h"
#include "ds_flow_control.h"
#include "ps_acl.h"
#include "ps_ip_addr.h"
#include "ps_meta_info.h"
#include "queue.h"
#include "ps_iface_defs.h"
#include "dss_iface_ioctl.h"

/*===========================================================================
                            FORWARD DECLARATIONS
===========================================================================*/
typedef struct ps_iface_s ps_iface_type;

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  GLOBAL_IFACE_PTR_ARRAY - Array of pointers to all of the the created
    interfaces on the system.  The semantics of this array are that empty
    slots have a value of 0, any other values indicate the slot points to an
    iface.
---------------------------------------------------------------------------*/
extern ps_iface_type *global_iface_ptr_array[MAX_SYSTEM_IFACES];

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_COOKIE_TYPE

DESCRIPTION
  type for the cookie that is used to identify an interface.
---------------------------------------------------------------------------*/
typedef uint16 ps_iface_cookie_type;


/*===========================================================================
                               CMD F_PTR TYPES
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_CMD_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_iface_cmd_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  void *client_data
);

/*===========================================================================
TYPEDEF PS_IFACE_IOCTL_F_PTR_TYPE()

DESCRIPTION
  This function performs various operations on the given ps interface. 
  Typically, these operations are to get or set a value.

DEPENDENCIES
  None.

PARAMETERS

  ps_iface_type*            - Pointer to ps_iface
  
  dss_iface_ioctl_type      - The operation name
  
  void*                     - Pointer to operation specific structure
  

  sint15*                   - Error code returned in case of failure (Error 
                              values are those defined in dserrno.h)

                              DS_EBADF - Returned by dss_iface_ioctl() if the
                              specified id_ptr is invalid (i.e. id_ptr does 
                              not map to a valid ps_iface_ptr).
                            
                              
                              DS_EINVAL - Returned by dss_iface_ioctl() when 
                              the specified IOCTL does not belong to the 
                              common set of IOCTLs and there is no IOCTL mode
                              handler registered for the specified interface.
                              
                              DS_EOPNOTSUPP - Returned by the lower level 
                              IOCTL mode handler when specified IOCTL is not 
                              supported by the interface. For instance, this 
                              would be returned by interfaces that do not 
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the 
                              implementation is mode specific, for example, 
                              GO_DORMANT).
                              
                              DS_EFAULT - This error code is returned if the 
                              specified arguments for the IOCTL are correct 
                              but dss_iface_ioctl() or a mode handler 
                              encounters an error while executing the IOCTL..
                              For instance, if the 1X interface cannot 
                              "GO_DORMANT" it would return this error. 
                              
                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.
                              
RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
typedef int (*ps_iface_ioctl_f_ptr_type)
( 
  ps_iface_type            *this_iface_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
);


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_BRIDGE_PROC_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interfaces command functions.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  client_data:    pointer to any client data that needs to be passed in

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef int (*ps_iface_bridge_proc_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void *client_data
);


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_TX_CMD_F_PTR_TYPE

DESCRIPTION
  Definition for the txmit cmd function of an interface.

  ALL MUST tx_cmds run in the context of PS task.

  Once this function is called - the pkt_chain_ptr & meta_info_ptr is the
  responsibility of the interface client and must be eventually freed by the
  interface client.  That is, the tx function MUST free the memory that is
  passed to it (the pkt_chain_ptr, and meta_info_ptr).

PARAMS
  this_iface_ptr:   Ptr to interface that this pkt was received on - and to
                 operate on.
  pkt_chain_ptr: Ptr to chain of dsm items that hold the payload that needs
                 to be transmitted.
  meta_info_ptr: Ptr to dsm item that contains the meta info for this pkt.
  tx_cmd_info:   info to pass to registered cmd handler

RETURN VALUE
  0 on success
 -1 on failure
---------------------------------------------------------------------------*/
typedef int (*ps_iface_tx_cmd_f_ptr_type)
(
  ps_iface_type      *this_iface_ptr,
  dsm_item_type     **pkt_chain_ptr,
  ps_meta_info_type **meta_info_ref_ptr,
  void               *tx_cmd_info
);


/*===========================================================================
                             EVENT RELATED STUFF
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_INFO_U_TYPE

DESCRIPTION
  Data structure that is used to pass event specific information into the
  callback functions that are registered for the events.

MEMBERS
  state: This will be set when the interface state change(*) callback is
    called and will contain the previous state the interface was in.
    (*) This applies to the IFACE_DOWN_EV and IFACE_UP_EV events.
  link_state: This will be set when the physical link state change(*)
    callback is called and will contain the previous state the link was in.
    (*) this applies to the IFACE_PHYS_LINK_DOWN_EV and IFACE_PHYS_LINK_UP_EV
  flow_mask: This will be set when the flow state(*) is changed, that is,
    flow is enabled, or disabled.  It will contain the previous flow_mask.
    (*) This applies to the IFACE_FLOW_ENABLED_EV and IFACE_FLOW_DISABLED_EV
---------------------------------------------------------------------------*/
typedef union
{
  ps_iface_state_enum_type state;
  phys_link_state_type     link_state;
  uint32                   flow_mask;
  ip_addr_type             ip_addr;
  uint8                    n_filters;
} ps_iface_event_info_u_type;


/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_EVENT_CBACK_TYPE

DESCRIPTION
  Defines for the event callbacks. There can be any number of events per
  interface and the memory is owned by the clients.

PARAMS
  this_iface_ptr: Ptr to interface control block on which to operate on.
  event: the event being registered for
  event_info: information specific to the event: for details look at typedef
  user_data_ptr: data to pass back to the user

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_iface_event_cback_type)
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

/*===========================================================================
                            ADDRESS RELATED STUFF
===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_ADDR_CMD_F_PTR_TYPE

DESCRIPTION
  Definitition of the function for an interface to get the peer information.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    ptr the IP address that is being operated on.
  info:           whatever client information the call needs

  NOTE: when this function type is being used for the revoke/release
        callbacks the IP address passed by reference is a variable that is ON
        THE STACK of the calling function, and thus needs to be copied if it
        needs to persist.

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_iface_addr_cmd_f_ptr_type)
(
  ps_iface_type *this_iface_ptr,
  ip_addr_type  *ip_addr,
  void          *info
);


#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */


/*===========================================================================
                           INTERFACE CONTROL BLOCK

TYPEDEF PS_IFACE_TYPE

DESCRIPTION
  The interface control block.
  Each interface has an instance of this data structure.
  Interface can be dynamically created and destroyed and can have
  multiple instances (PDP-IP-0, PDP-IP-1, ...)
  The output of each iface always goes to another iface. The IP stack is also
  initialized as an interface.
  ALL REGISTERED CMDs & INDICATIONs must be under TASKLOCKs & INTLOCKs.
===========================================================================*/
struct ps_iface_s
{
  /*-------------------------------------------------------------------------
    The private information that should only ever be changed through ps_iface
    functions.  The data in here should NOT changed by the clients hand.

    NOTE: This MUST be the first element as the q link needs to be the first
          element in the structure.
  -------------------------------------------------------------------------*/
  struct iface_private
  {
    q_link_type    link;               /* link pointer - MUST BE FIRST     */
    q_type         event_q_array[IFACE_EVENT_MAX]; /* event callback array */
    ps_iface_type *this;               /* if NULL the block is invalid     */
    ps_iface_type *tx_bridge_ptr;      /* pointer to iface we are bridging */
    ps_iface_tx_cmd_f_ptr_type tx_cmd; /* set via accessor function        */
    void          *tx_cmd_info;        /* information to use with tx_cmd   */
    uint32         tx_flow_mask;       /* if !=0, iface is flow_ctl        */
    uint32         non_sys_sockets_flow_mask; /* if !=0, non_sys sockets are
                                                 flow controlled           */
    uint32         ipv4_addr;          /* this interfaces IP address       */

    ps_iface_cookie_type  cookie;      /* cookie identifying an instance   */
    ps_iface_state_enum_type state;    /* interface state                  */
    phys_link_state_type phys_link_state; /* physical link state           */

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

    /*-----------------------------------------------------------------------
      The following information will be set by link layers to contain peer
      information, like PDSN or GGSN addresses.
    -----------------------------------------------------------------------*/
    struct
    {
      ps_iface_addr_cmd_f_ptr_type   get_addr_cmd_f_ptr;
      void                          *link_info;
    } peer_data;

    struct
    {
      q_type filter_q;
      uint8 n_filters;
      boolean process_filter;
    } ip_filter_info;

    void (*sdb_status_handler_sock_fcn) (sint15 sockfd, void* cm_info_ptr);
    
  } private;

  /*-------------------------------------------------------------------------
    Fields that identify the interface. Can be changed by clients.
  -------------------------------------------------------------------------*/
  uint16                        group_mask;
  ps_iface_name_enum_type       name;
  uint8                         instance;

  /*-------------------------------------------------------------------------
    This flag defines the call termination policy for an interface.  If set
    to TRUE the interface should be closed gracefully, even if this means
    re-establishing a dormant physical link.  The default value of this flag
    is set to FALSE.
    -------------------------------------------------------------------------*/
  boolean                 graceful_dormant_close;

  /*-------------------------------------------------------------------------
    void pointer which the client can use to store whatever information they
    desire.
  -------------------------------------------------------------------------*/
  void                   *client_data_ptr;

  /*-------------------------------------------------------------------------
    IP info fields - struct defined in ps_ip_addr.h
  -------------------------------------------------------------------------*/
  ip_v4_net_info_type     v4_net_info;

  /*-------------------------------------------------------------------------
    Statistics
  -------------------------------------------------------------------------*/
  ps_iface_stats_type     stats;

  /*-------------------------------------------------------------------------
    Commands - called from PS->Interface client
  -------------------------------------------------------------------------*/
  ps_iface_cmd_f_ptr_type bring_up_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type tear_down_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type phys_link_down_cmd_f_ptr;
  ps_iface_cmd_f_ptr_type phys_link_up_cmd_f_ptr;

  /*-------------------------------------------------------------------------
    bridge command handler
  -------------------------------------------------------------------------*/
  ps_iface_bridge_proc_f_ptr_type bridge_proc_f_ptr;

  /*-------------------------------------------------------------------------
    iface ioctl function pointer.  
  -------------------------------------------------------------------------*/
  ps_iface_ioctl_f_ptr_type      iface_ioctl_f_ptr;

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */
};


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                              MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
MACRO PS_IFACE_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the iface pointer passed in
  is valid.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  TRUE: if iface_ptr is valid
  FALSE: otherwise.
===========================================================================*/
#define PS_IFACE_IS_VALID( iface_ptr )          \
  (((iface_ptr) != NULL) &&                     \
   (iface_ptr->private.this == (iface_ptr)))

/*===========================================================================
MACRO PS_IFACE_STATE()

DESCRIPTION
  This macro returns the state of the interface that was passed in.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  the state of the passed in interface.
===========================================================================*/
#define ps_iface_state( iface_ptr )             \
  (iface_ptr)->private.state

/*===========================================================================
MACRO PS_IFACE_PHYS_STATE()

DESCRIPTION
  This macro returns the state of the interface that was passed in.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  the state of the passed in interface.
===========================================================================*/
#define ps_iface_phys_link_state( iface_ptr )   \
  (iface_ptr)->private.phys_link_state


/*===========================================================================
MACRO PS_IFACE_GET_COOKIE()

DESCRIPTION
  This macro returns the interface cookie, which is unique per interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  the cookie for the passed in interface
===========================================================================*/
#define ps_iface_get_cookie( iface_ptr )        \
  (iface_ptr)->private.cookie

/*===========================================================================
MACRO PS_IFACE_IP_V4_ADDR()

DESCRIPTION
  This macro returns the IPv4 address assigne to the interface.

PARAMETERS
  iface_ptr: pointer to the interface in question.

RETURN VALUE
  IP address, 0 if interface pointer is NULL
===========================================================================*/
#define PS_IFACE_IP_V4_ADDR( iface_ptr )                                \
          (iface_ptr ? (iface_ptr->private.ipv4_addr) : 0x00000000 )

/*===========================================================================
MACRO PS_IFACE_BRIDGE_IFACE()

DESCRIPTION
  This macro will call the transmit of the interface that is passed in.

PARAMETERS
  iface_ptr: pointer to the interface this is bridged to.

RETURN VALUE
  NULL: if not set
  the address of the interface if it is set.
===========================================================================*/
#define ps_iface_bridge_iface( iface_ptr )      \
  (iface_ptr)->private.tx_bridge_ptr


/*===========================================================================
MACRO PS_IFACE_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether the interface is has flow disabled.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  TRUE: if flow is disabled
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_FLOW_ENABLED( iface_ptr )                      \
  ((iface_ptr)->private.tx_flow_mask == DS_FLOW_IS_ENABLED)

/*===========================================================================
MACRO PS_IFACE_GET_FLOW_MASK()

DESCRIPTION
  This macro returns the flow mask of the passed in interface.

PARAMETERS
  iface_ptr: pointer to the interface whose flow mask is requested.

RETURN VALUE
  The flow mask of the interface in question

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_get_flow_mask( iface_ptr )     \
  ((iface_ptr)->private.tx_flow_mask)


/*===========================================================================
MACRO PS_IFACE_REG_SDB_STATUS_HANDLER()

DESCRIPTION
  This macro registers the specified fnc_ptr as the handler for the sdb 
  status callback.

PARAMETERS
  iface_ptr: pointer to the interface whose flow mask is requested.
  fnc_ptr  : pointer to the function to be registered

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_reg_sdb_status_handler( iface_ptr, fcn_ptr )     \
  ((iface_ptr)->private.sdb_status_handler_sock_fcn = fcn_ptr)


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
);

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
);

/*===========================================================================
MACRO PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED()

DESCRIPTION
  This macro returns whether the non system sockets on this interface has 
  flow disabled.

PARAMETERS
  iface_ptr: pointer to the interface whose state is requested.

RETURN VALUE
  TRUE: if flow is disabled
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_NON_SYS_SOCKETS_FLOW_ENABLED( iface_ptr )  \
  ((iface_ptr)->private.non_sys_sockets_flow_mask == DS_FLOW_IS_ENABLED)

/*===========================================================================
MACRO PS_IFACE_GET_NON_SYS_SOCKETS_FLOW_MASK()

DESCRIPTION
  This macro returns the flow mask for non system sockets of the passed in 
  interface.

PARAMETERS
  iface_ptr: pointer to the interface whose flow mask is requested.

RETURN VALUE
  The flow mask of the interface in question

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_get_non_sys_sockets_flow_mask( iface_ptr )     \
  ((iface_ptr)->private.non_sys_sockets_mask)


/*===========================================================================
MACRO PS_IFACE_HANDLE_FLOW_CTL()

DESCRIPTION
  This macro will free a packet if the interface passed in is flow
  controlled, and will return -1

PARAMETERS
  iface_ptr: pointer to the interface on which to TX.
  pkt_ptr_ptr:  dsm item that was being Txed
  meta_info_ref_ptr: meta infor for the packet being TXed
===========================================================================*/
#define PS_IFACE_HANDLE_FLOW_CTL(iface_ptr, pkt_ptr_ptr, meta_info_ref_ptr) \
  if(iface_ptr->private.tx_flow_mask != 0)                                  \
  {                                                                         \
    PS_META_INFO_FREE(meta_info_ref_ptr);                                   \
    dsm_free_packet(pkt_chain_ptr);                                         \
                                                                            \
    return -1;                                                              \
  }


/*===========================================================================
MACRO PS_IFACE_NUM_FILTERS()

DESCRIPTION
  This macro returns the current number of IP filters in the iface.

PARAMETERS
  iface_ptr: pointer to the interface on which operation is requested.

RETURN VALUE
  As described above.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_NUM_FILTERS( iface_ptr )                      \
  ((iface_ptr)->private.ip_filter_info.n_filters)

#ifdef FEATURE_DATA_PS_UNSUPPORTED
#error code not present
#endif /* FEATURE_DATA_PS_UNSUPPORTED */

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION PS_IFACE_CREATE()

DESCRIPTION
    This will memset()s the private data struct to 0, initializes the
    "this_ptr" to point to the interface control block itself, initializes
    the queues and the TX ptr to the default value.

    NOTE: the memset() sets the iface_state to DISABLED.

PARAMETERS
    this_iface_ptr: ptr to interface control block on which to operate on.
    name: the name of the interface (like CDMA_SN_IFACE)
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
);

/*===========================================================================
FUNCTION PS_IFACE_DELETE()

DESCRIPTION
  It will flush all routes for this interface.  Then will memset() the
  entire data structure.

  NOTE: This should be called AFTER the delete_ind() has been called -
  otherwise it will be impossible to call the callbacks (they will no longer
  be linked to the iface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  This function MUST be TASKLOCK()ed

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_delete
(
  ps_iface_type *this_iface_ptr
);


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
);


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
);


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
  0: on success
 -1: on failure

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
);


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
);


/*===========================================================================
FUNCTION PS_IFACE_GET_HANDLE()

DESCRIPTION
  This function returns a ps_iface handle based on the interface name and 
  instance passed in.  If no such interface exists, a value of NULL is 
  returned.

PARAMETERS
  Pointer to struct containing name and instance information.

RETURN VALUE
  ps_iface handle on success
  NULL on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type *ps_iface_get_handle
(
  dss_iface_ioctl_name_enum_type name,
  uint32                         instance
);

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
  ps_iface_type *bridged_iface_ptr
);


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
);

/*===========================================================================
MACRO PS_IFACE_UNSET_TX_FUNCTION()

DESCRIPTION
  This macro is simply a shortcut for clearing the tx function.  It simply
  calls the set command with the NULL as the argument.

PARAMETERS
  this_iface_ptr:  interface on which to operate

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_unset_tx_function( this_iface_ptr) \
  ps_iface_set_tx_function((this_iface_ptr), NULL, NULL)


/*===========================================================================
FUNCTION PS_IFACE_ENABLE_FLOW()

DESCRIPTION
  Interface user enables flow on the interface.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
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
);

/*===========================================================================
FUNCTION PS_IFACE_DISABLE_FLOW()

DESCRIPTION
  client disables flow on the interface.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.
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
);


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
);

/*===========================================================================
FUNCTION PS_IFACE_GET_ADDR()

DESCRIPTION
  This function is used to get the local address of the passed in interface.
  If on interface type is specified in the ip_addr_ptr, either v4 or v6 will
  be returned depending on which one is set (v4 will always be returned if it
  is set).

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be

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
);

/*===========================================================================
FUNCTION PS_IFACE_GET_CACHED_ADDR()

DESCRIPTION
  This function will return the address that is stored in the requested IP
  address field of the interface regardless of the state.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be

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
);

/*===========================================================================
FUNCTION PS_IFACE_SET_PEER_DATA()

DESCRIPTION
  This function is used by the link layer associated with an interface to set
  its peer information  associated with the passed in interface.

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
  ps_iface_type                 *ps_iface_ptr,
  ps_iface_addr_cmd_f_ptr_type   get_addr_cmd_f_ptr,
  void                          *link_info
);

/*===========================================================================
FUNCTION PS_IFACE_RESET_PEER_DATA()

DESCRIPTION
  This function is used by the link layer associated with an interface to
  reset the peer information associated with the passed in interface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define ps_iface_reset_peer_data(iface)         \
  ps_iface_set_peer_data((iface), NULL, NULL);


/*===========================================================================
FUNCTION PS_IFACE_GET_PEER_ADDR()

DESCRIPTION
  This function will return the address of the interfaces peer (e.g. the
  GGSN, or the PDSN) if this is a point-to-point link.  If the interface
  isn't point-to-point the invalid address will be returned.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

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
);

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
  returne ewouldblock.  Finally if it is in the going down state, it returns
  closeinprogress.

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
);


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
);

/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_CMD()

DESCRIPTION
  This function is used to bring up the physical link on an iface.
  // more detail here
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
);

/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_CMD()

DESCRIPTION
  This function is used to tear down the physical link of an iface.
  // more detail here
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
);


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
);


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
);



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
  this_iface_ptr: Ptr to interface control block on which to operate on.

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
);


/*===========================================================================
FUNCTION PS_IFACE_DISABLED_IND()

DESCRIPTION
  The given interface has been disabled.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

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
);


/*===========================================================================
FUNCTION PS_IFACE_DOWN_IND()

DESCRIPTION
  The given interface has been stopped.   Call callbacks.
  This also sets the physical link state to DOWN.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

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
);


/*===========================================================================
FUNCTION PS_IFACE_ROUTEABLE_IND()

DESCRIPTION
  The given interface has been set to routeable. The interface needs to be in
  the UP state before this indication is called.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_routeable_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_UP_IND()

DESCRIPTION
  The given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_up_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_DOWN_IND()

DESCRIPTION
  The physical link of the given interface is down (707 dormancy).  Call
  callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_phys_link_down_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_PHYS_LINK_UP_IND()

DESCRIPTION
  The physical link of the given interface is up.  Call callbacks.

  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_phys_link_up_ind
(
  ps_iface_type *this_iface_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_DELETED_IND()

DESCRIPTION
  The given interface has been deleted.  Call the callbacks.

  NOTE: This needs to be called before the

  THIS FUNCTION SHOULD NOT BE CALLED IN AN ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_deleted_ind
(
  ps_iface_type *this_iface_ptr
);


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
);

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
  int16         sock_id,
  void*         cm_info_ptr
);

#endif /* FEATURE_DATA */

#endif /* PS_IFACE_H */


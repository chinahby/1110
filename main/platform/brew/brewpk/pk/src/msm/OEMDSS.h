/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMDSS.h

SERVICES:  OEM DSS backwards compatibility layer

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for newer DSS functions that do not exist in older distributions

        Copyright © 2003-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
===========================================================================*/

#ifndef OEMDSS_H
#define OEMDSS_H

#include "dssocket.h"           // for struct sockaddr, etc.

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(DSS_VERSION) || (DSS_VERSION>=20 && DSS_VERSION <=29)
#define OEMDSS_NETLIB2_NOT_SUPPORTED
#endif //DSS_VERSION

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 5100/NT3503.
  ===========================================================================
  ===========================================================================*/
#if !defined(OEM_PROVIDES_SERVER_SOCKETS)

#define DS_ENOPROTOOPT 131 /* The option is unknown at the level indicated */
#define DS_EMSGSIZE       130           /* The message is too large.       */
#define DS_EMSGTRUNC      134  /* msg truncated, supplied buffer too small */

/*---------------------------------------------------------------------------
                      Asynchronous Socket Events
---------------------------------------------------------------------------*/
#define DS_ACCEPT_EVENT 0x08        /* associated with an accetable socket */


/*===========================================================================

FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  sucessful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is recieved and there
  are no free queue slots the new connection is rejected
  (ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indiates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be 
  reduced to the maximum (see DSS_SOMAXCONN).

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections. 
  DS_EINVAL           Socket already open, closed, unbound or not one 
                      you can listen on. 

SIDE EFFECTS 
  For TCP, initiates passive open for new connections.

===========================================================================*/
extern sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffect the queue size is
  maintained (ie. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block 
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not of type SOCK_STREAM 
  DS_EINVAL           Socket is not listening. 
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETDOWN         network subsystem unavailable
  DS_NOMEM            not enough memory to establish backlog connections. 

SIDE EFFECTS 

  The head backlog item from the queue of the listening socket is
  removed from that queue.

===========================================================================*/
extern sint15 dss_accept 
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *remoteaddr,                       /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
);

#endif // !defined(OEM_PROVIDES_SERVER_SOCKETS)

/*===========================================================================
  ===========================================================================
  definitions for compilation with CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE OEM 
  feature or with DMSS releases prior to 6050/DP3110.
  ===========================================================================
  ===========================================================================*/
#if defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE) || (!defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250))
#ifndef CUST_EDITION
   #define TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE

   extern void OEMDSS_OnCallStatus(void);
   extern void OEMDSS_FreeTAPI(void *p);
   extern void OEMDSS_SetupTAPI(void);
#endif
#endif // defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE) || ...

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6050/DP3110.
  ===========================================================================
  ===========================================================================*/
#if !defined(AEE_SIMULATOR) && !defined(T_MSM6050) && !defined(T_MSM6250)

enum
{
  DSS_SO_SILENT_CLOSE =  2,        /* bool: close() call causes conn reset */
};

/*===========================================================================

FUNCTION DSS_SETSOCKOPT

DESCRIPTION
  Set the options associated with a socket. This fuction expects the 
  following parameters:
  
DEPENDENCIES
  None. 
  
PARAMETERS  
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level. 
  int optname,      -     Option name. 
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.  
  sint15 *errno     -     Error condition value.     


RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                       /* error condition value          */
);

/*===========================================================================
FUNCTION DSS_GETSOCKOPT

DESCRIPTION
  Return an option associated with a socket. This fuction expects the 
  following parameters:
  
DEPENDENCIES
  None. 

PARAMETERS  
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level. 
  int optname,      -     Option name. 
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.  
  sint15 *errno     -     Error condition value.     

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of 
  the buffer pointed to by optval, and modified on return to indicate the  
  actual  size  of the value returned. On error, return DSS_ERROR and places 
  the error condition value in *errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value  
  DS_EFAULT               Invalid buffer or argument  

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */                  
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
);

#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6050)



/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6100/30505.
  ===========================================================================
  ===========================================================================*/
#if !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)


#include "psglobal.h"           // for ip_addr_type
#include "ip.h"                 // for ip4a

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_NAME_ENUM_TYPE

DESCRIPTION
  List of interface names and groups
---------------------------------------------------------------------------*/
typedef enum
{
  /* Group Names - Note: this is a mask which uses the bottom 15 bits */
  ANY_IFACE_GROUP    = 0x0000,
  ANY_DEFAULT_GROUP  = 0x0001,
  WWAN_GROUP         = 0x0002,
  RM_GROUP           = 0x0004,
  
  /* Interface Names - Note: these values are sequential */
  IFACE_MASK         = 0x8000,
  CDMA_SN_IFACE      = 0x8001,
  CDMA_AN_IFACE      = 0x8002,
  UMTS_IFACE         = 0x8003,
  SIO_IFACE          = 0x8004,
  LO_IFACE           = 0x8800

} ps_iface_name_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_STATE_ENUM_TYPE

DESCRIPTION
  Enum for states of an interface.
---------------------------------------------------------------------------*/
typedef enum
{
  IFACE_DISABLED   = 0,
  IFACE_DOWN       = 1,
  IFACE_COMING_UP  = 2,
  IFACE_ROUTEABLE  = 3,          /* packet can be routed to this interface */
  IFACE_UP         = 4,          /* data can originate over this interface */
  IFACE_GOING_DOWN = 5
} ps_iface_state_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF PHYS_LINK_STATE_TYPE

DESCRIPTION
  Physical link state enum - what is the physical link doing
---------------------------------------------------------------------------*/
typedef enum
{
  PHYS_LINK_DOWN       = 0,
  PHYS_LINK_COMING_UP  = 1,
  PHYS_LINK_UP         = 2,
  PHYS_LINK_GOING_DOWN = 3

} phys_link_state_type;

typedef struct
{
  ps_iface_name_enum_type  name;
  uint32                   instance;
} dss_iface_name_and_instance_type;

typedef enum
{
  DSS_APP_ID  = 0,
  DSS_SOCK_FD = 1
} dss_id_type;

typedef struct
{
  dss_id_type type;
  union
  {
    sint15 app_id;
    sint15 sockfd;
  } info;
} dss_id_info_type;

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                      REGIONAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Definition of various interface control operations. Please do NOT change 
  the values of the existing enumerated types when adding new ones (to ensure
  that object code released with previous versions of the code do not break).
  Each of the following control operations have associated tyepdef (with 
  corresponding names) that are passed in or returned for get/set operations. 
  Note that all GET operations are assigned even numbers and SET operations 
  are assigned odd numbers. Also ensure that symetric operations are 
  grouped.  
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------
  
                              COMMON IOCTLS
                              
    The following operations are common to all interfaces. 
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MIN                     = -1,
  DSS_IFACE_IOCTL_GET_IPV4_ADDR           = 0, /* Get IPV4 addr of iface   */
  DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR  = 2, /* Get Primary DNS (IPV4) addr 
                                                                  of iface */
  DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR  = 4, /* Get Secondary DNS (IPV4) 
                                                             addr of iface */
  DSS_IFACE_IOCTL_GET_MTU                 = 6, /* Get MTU of iface         */
  DSS_IFACE_IOCTL_GET_STATE               = 8, /* Get state of iface       */
  DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE     = 10,/* Get physical link state  */
  DSS_IFACE_IOCTL_REG_EVENT_CB            = 12,/* Register callback for 
                                                  events. Note that app can 
                                                  have only one callback per 
                                                  interface for each event */
  DSS_IFACE_IOCTL_DEREG_EVENT_CB          = 13,/* Deregister event callback*/
  DSS_IFACE_IOCTL_GET_ALL_IFACES          = 14,/* Get all enabled ifaces. 
                                                  Note for that this IOCTL,
                                                  a NULL id_ptr has to be 
                                                  passed as a parameter    */   
  DSS_IFACE_IOCTL_REG_IP_FILTER           = 15,/* Register IP filter       */
  DSS_IFACE_IOCTL_DEREG_IP_FILTER         = 16,/* Deregister IP filter     */


  /*-------------------------------------------------------------------------
    The following operations are common, but implementation is specific to
    a particular iface. Some may not even be supported by a particular iface. 
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_GO_ACTIVE               = 101,/* Go active from dormant  */
  DSS_IFACE_IOCTL_GO_NULL                 = 105,/* Go NULL                 */
  

  /*-------------------------------------------------------------------------
    IOCTLs which are interface specific should be enumerated below this.
    Note that the enuerated values from 0 - 199 are reserved for the common
    IOCTLs. All interface specific IOCTLs should use values greater than 199. 
    It is recommended that each interface have a certain range of values 
    reserved. This would ensure that all the interface specific operations
    are grouped together. For intance, CDMA2000 could use value b/w 200 and
    399, WCDMA 400 to 599 etc.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
                                
                                707 IOCTLS
                                
    The following operations are specific to 707 (1x) interface. Start 200.
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_707_GET_MDR             = 200,/* 1x QCMDR value          */
  DSS_IFACE_IOCTL_707_SET_MDR             = 201,/* 1x QCMDR value          */
  DSS_IFACE_IOCTL_707_GET_DORM_TIMER      = 202,/* 1x dorm timer value     */
  DSS_IFACE_IOCTL_707_SET_DORM_TIMER      = 203,/* 1x dorm timer value     */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK= 204,/* 1x RLP curr NAK policy  */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK= 205,/* 1x RLP curr NAK policy  */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK = 206,/* 1x RLP  default NAK     */
  DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK = 207,/* 1x RLP  default NAK     */
  DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK = 208,/* 1x RLP  negotiated NAK  */
  DSS_IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI  = 210,/* 1x QOS nonassured prio  */
  DSS_IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI  = 211,/* 1x QOS nonassured prio  */
  DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY   = 212,/* 1x SDB possible query   */
  DSS_IFACE_IOCTL_707_ENABLE_HOLDDOWN     = 215,/* 1x enable/disable
                                                   holddown timer          */

  /*-------------------------------------------------------------------------

                              UMTS IOCTLS
                                
    The following operations are specific to UMTS interface.Start from 300.
  -------------------------------------------------------------------------*/


  /*-------------------------------------------------------------------------

                              MORE COMMON STUFF
                              
  -------------------------------------------------------------------------*/
  DSS_IFACE_IOCTL_MAX,
  DSS_IFACE_IOCTL_NAME_FORCE_32_BIT       = 0x7FFFFFFF /* Force 32bit enum */
} dss_iface_ioctl_type;

/*---------------------------------------------------------------------------
                                
                                COMMON DATA STRUCTURES
                                
              The following data structs are for common ioctls.
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Definition of iface names.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_IOCTL_CDMA_SN,
  DSS_IFACE_IOCTL_CDMA_AN,
  DSS_IFACE_IOCTL_UMTS,
  DSS_IFACE_IOCTL_SIO,
  DSS_IFACE_IOCTL_IFACE_FORCE_32_BIT = 0x7FFFFFFF /* Force 32bit enum type */
} dss_iface_ioctl_name_enum_type;

/*---------------------------------------------------------------------------
  Definition of identifying struct.  
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_ioctl_name_enum_type  name;
  uint32                          instance;
} dss_iface_ioctl_id_type;

/*---------------------------------------------------------------------------
  Definition of iface events.
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_IOCTL_MIN_EV,
  DSS_IFACE_IOCTL_DOWN_EV,
  DSS_IFACE_IOCTL_UP_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV,
  DSS_IFACE_IOCTL_PHYS_LINK_UP_EV,
  DSS_IFACE_IOCTL_ADDR_CHANGED_EV,
  DSS_IFACE_IOCTL_EVENT_MAX,
  DSS_IFACE_IOCTL_EVENT_FORCE_32_BIT = 0x7FFFFFFF /* Force 32bit enum type */
} dss_iface_ioctl_event_enum_type;

/*---------------------------------------------------------------------------
  Data type used for Phys link State (DSS_IFACE_IOCTL_PHYS_LINK_STATE)
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_state_type;

/*---------------------------------------------------------------------------
  Data types used for registering/deregistering for event callbacks 
  (DSS_IFACE_IOCTL_REG_EVENT_CB/DSS_IFACE_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_DOWN_EV and DSS_IFACE_IOCTL_UP_EV
---------------------------------------------------------------------------*/
typedef ps_iface_state_enum_type dss_iface_ioctl_event_info_type;

/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV and 
  DSS_IFACE_IOCTL_PHYS_LINK_UP_EV
---------------------------------------------------------------------------*/
typedef phys_link_state_type dss_iface_ioctl_phys_link_event_info_type;

/*---------------------------------------------------------------------------
  Event info for DSS_IFACE_IOCTL_ADDR_CHANGED_EV
---------------------------------------------------------------------------*/
typedef ip_addr_type dss_iface_ioctl_addr_change_event_info_type;

typedef union
{
  dss_iface_ioctl_event_info_type             iface_state_info;
  dss_iface_ioctl_phys_link_event_info_type   phys_link_state_info;
  dss_iface_ioctl_addr_change_event_info_type addr_change_info;

// Note that in the AMSS, dss_iface_ioctl_priv_addr_info_type is actually a member of 
// dss_iface_ioctl_event_info_union_type. Despite this fact, in order to supply 
// backward compatibility for all DSS versions, we don't define it as a member of the union, 
// but rather cast the union to dss_iface_ioctl_priv_addr_info_type in the OEM code.

//#if ((!defined DSS_VERSION) || (DSS_VERSION<1400))
//   dss_iface_ioctl_priv_addr_info_type priv_ipv6_addr;
//#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1400))
} dss_iface_ioctl_event_info_union_type;

/*---------------------------------------------------------------------------
  Typedef for event callback function
  (DSS_IFACE_IOCTL_REG_EVENT_CB/ DSS_IFACE_IOCTL_DEREG_EVENT_CB)
---------------------------------------------------------------------------*/
typedef void (*dss_iface_ioctl_event_cb)
(
  dss_iface_ioctl_event_enum_type          event,
  dss_iface_ioctl_event_info_union_type    event_info,
  void                                     *user_data,
  sint15                                   app_id,
  ps_iface_name_enum_type                  name,
  uint32                                   instance 
);

/*---------------------------------------------------------------------------
  Typedef for struct used with DSS_IFACE_IOCTL_REG_EVENT_CB and
  DSS_IFACE_IOCTL_DEREG_EVENT_CB. Note that the application must specify a 
  valid APP_ID when registering/deregistering for event callbacks.
---------------------------------------------------------------------------*/
typedef struct 
{
  dss_iface_ioctl_event_cb        event_cb;
  dss_iface_ioctl_event_enum_type event;
  void                            *user_data_ptr;
  sint15                          app_id;
} dss_iface_ioctl_ev_cb_type;


/*---------------------------------------------------------------------------
  Data type used for DSS_IFACE_IOCTL_GO_ACTIVE and DSS_IFACE_IOCTL_GO_NULL 
  (Note that apps do not have to define an arg of this type.
   They can just pass in Null).
---------------------------------------------------------------------------*/
typedef void dss_iface_ioctl_null_arg_type;

/*---------------------------------------------------------------------------
  Data type used for 707 QCMDR value
  (DSS_IFACE_IOCTL_707_GET/SET_MDR)
---------------------------------------------------------------------------*/
typedef unsigned int dss_iface_ioctl_707_mdr_type;

/*---------------------------------------------------------------------------
  Data type used for 707 RLP3 NAK policies (current, default, negotiated)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_CURR_NAK)
  (DSS_IFACE_IOCTL_707_GET/SET_RLP_ALL_DEF_NAK)
  (DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK)
  IOCTL_707_MAX_NAK_ROUNDS should be same as DSRLP_MAX_NAK_ROUNDS. Duplicate
  definition to avoid exposing dsrlp.h  by including here.
---------------------------------------------------------------------------*/
#define IOCTL_707_MAX_NAK_ROUNDS 3

typedef struct
{
  byte rscb_index;
  byte nak_rounds_fwd;
  byte naks_per_round_fwd[IOCTL_707_MAX_NAK_ROUNDS];
  byte nak_rounds_rev;
  byte naks_per_round_rev[IOCTL_707_MAX_NAK_ROUNDS];
} dss_iface_ioctl_707_rlp_opt_type;

/*--------------------------------------------------------------------------
  Data type used for 707 SDB support query.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 flags;       /* Same flag as used in socket write to specify SDB
                         (Eg: MSG_USE_SDB_REACH, MSG_USE_SDB_ACH, etc)     */
  boolean can_do_sdb; /* On return from ioctl, this boolean field will say
                         whether SDB supported or not                      */
} dss_iface_ioctl_707_sdb_support_query_type;

#define DS_ESHUTDOWN      136 /* can't send after shutting down write-half */

/*---------------------------------------------------------------------------
  The values for the 'how' argument of the shutdown() call. Specifies what
  action to perform:
---------------------------------------------------------------------------*/
enum
{
  DSS_SHUT_RD   = 0,                          /* disallow further receives */
  DSS_SHUT_WR   = 1,                             /* disallow further sends */
  DSS_SHUT_RDWR = 2          /* disallow further receives as well as sends */
};

enum
{
  DSS_SO_RCVBUF       =  3,        /* set the receive window size          */
};

enum
{
  DSS_SOL_SOCKET = 2,               /* socket level                        */
};

/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the local address assigned to the specified socket.
  
DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  addr      -  local address currently assigned to the socket 
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it 
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket 
  address and the address length in addr and addrlen parameters, resp.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory 
                          location

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getsockname
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.
  
DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket 
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it 
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer 
  address and the address length in addr and addrlen parameters, resp.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory 
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_getpeername
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
);

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID()

DESCRIPTION
  This function is used to get the interface name and instance of a specific
  interface. The interface can be specified using either an app_id or a 
  sockfd. Note that the interface would be the one that the app/socket is
  currently using.

DEPENDENCIES
  None.

PARAMETERS
  dss_id_info_type id - Specifies whether an app_id or sockfd is being 
    as an identifier.
  dss_iface_ioctl_id_type *iface_id - Output parameter which contains the 
    name and instance of the iface associated with specified id.
 
RETURN VALUE
  0 on success.
  -1 on failure.

SIDE EFFECTS
  None.
===========================================================================*/
extern int dss_get_iface_id
(
  dss_id_info_type id,
  dss_iface_ioctl_id_type *iface_id 
);

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
  This function dstermines the ps_iface_ptr associated with the passed in
  identifier (App ID). It then calls ps_iface_ioctl().

DEPENDENCIES
  None.

PARAMETERS

  dss_iface_ioctl_id_type   - Interface ID on which the specified operations 
                              is to be performed
                          
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
                              "GET_MDR").
                              
                              DS_EFAULT - This error code is returned if the 
                              specified arguments for the IOCTL are incorrect 
                              or if dss_iface_ioctl() or a mode handler 
                              encounters an error while executing the IOCTL.                            
                              
                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
extern int dss_iface_ioctl
( 
  dss_iface_ioctl_id_type  *id_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *dss_errno
);

/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:
   
  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subseuqnet calls to both recv and send functions
  
DEPENDENCIES
  None. 

PARAMETERS  
  sockfd    -  socket file descriptor
  how       -  action to be performed
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
);

#define IP_ADDR_V4(a) a
#define IP_ADDR_IS_VALID(a) TRUE

#else // !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250) 

#define IP_ADDR_V4(a) a.addr.v4
#define IP_ADDR_IS_VALID(a) ((a).type != IP_ADDR_INVALID)

#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250) 



/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6100/4040 and with DMSS release 6050/3320.  DSS_MAX_SYS_APPS was 
  defined in 6100 somewhere between releases 30505 and 4040.  PF_INET6 was defined 
  somewhere between 6050/3110 and 6050/3320 and was defined in 6100 somewhere 
  between releases 30505 and 4040.
  ===========================================================================
  ===========================================================================*/
#if !defined(DSS_MAX_SYS_APPS) && !defined(PF_INET6)

enum 
{
  DSS_SO_LINGER       =  4,        /* linger on close                      */
};

#endif // !defined(DSS_MAX_SYS_APPS) && !defined(PF_INET6)

#if !defined(DSS_MAX_SYS_APPS) 

/*---------------------------------------------------------------------------
  The data type used for SO_LINGER socket option. Note that l_linger is in
  ms, not in seconds as on UNIX systems
---------------------------------------------------------------------------*/
typedef struct
{
  int l_onoff;                                     /* linger active or not */
  int l_linger;                   /* how many milli-secondss to linger for */
} dss_so_linger_type;

#endif // !defined(DSS_MAX_SYS_APPS)

#if !defined(MAX_SYSTEM_IFACES)

/*---------------------------------------------------------------------------
  MAX_SYSTEM_IFACES - the maximum number of interfaces in the system.
  2 - CDMA SN IFACE (1 for pkt, 1 for async)
  1 - CDMA AN IFACE  
  3 - UMTS IFACE
  2 - SIO IFACE (1 for CDMA, 1 for UMTS)  
  1 - LO IFACE  
  1 - IPSEC IFACE
---------------------------------------------------------------------------*/
#define MAX_SYSTEM_IFACES 10

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GET_ALL_IFACES parameter type
  -  (Note for this IOCTL, a NULL id_ptr has to be passed as a parameter).
---------------------------------------------------------------------------*/
typedef struct
{
  unsigned int            number_of_ifaces;
  dss_iface_ioctl_id_type ifaces[MAX_SYSTEM_IFACES];
} dss_iface_ioctl_all_ifaces_type;

#endif // !defined(MAX_SYSTEM_IFACES)



/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6100/5015.  DSS_IFACE_GET_ID3 was defined in 6100 somewhere
  between releases 4040 and 5015.
  ===========================================================================
  ===========================================================================*/
#if !defined(DSS_IFACE_GET_ID3)

#define OEMDSS_NO_IFACE_GET_ID3_DEFINITION

/*---------------------------------------------------------------------------
  opaque interface id
---------------------------------------------------------------------------*/

typedef uint32 dss_iface_id_type;

/*---------------------------------------------------------------------------
  Invalid iface_id: does not identify any valid iface
---------------------------------------------------------------------------*/
#define DSS_IFACE_INVALID_ID  0

#define DSS_IFACE_GET_ID3(iface_name, iface_inst, phys_link_inst)  \
  ( (((uint32)iface_name & 0x0000FFFF) << 16) |                    \
    (((uint32)iface_inst & 0x000000FF) << 8) |                     \
    ((uint32)phys_link_inst & 0x000000FF) )

static __inline dss_iface_id_type oemdss_get_iface_id(sint15 appid)
{
   static dss_iface_ioctl_id_type iface_ioctl_id;
   dss_id_info_type id;

   id.type = DSS_APP_ID;
   id.info.app_id = appid;

   if (DSS_SUCCESS != dss_get_iface_id(id, &iface_ioctl_id)) {
      return DSS_IFACE_INVALID_ID;
   }

   return DSS_IFACE_GET_ID3(iface_ioctl_id.name, iface_ioctl_id.instance, 0);
}

/*---------------------------------------------------------------------------
DSS_IFACE_IOCTL_GET_ALL_IFACES parameter type
-  (Note for this IOCTL, a NULL id_ptr has to be passed as a parameter).
---------------------------------------------------------------------------*/
typedef struct
{
   unsigned int      number_of_ifaces;
   dss_iface_id_type ifaces[MAX_SYSTEM_IFACES];
} oemdss_iface_ioctl_all_ifaces_type;

#else  // !defined(DSS_IFACE_GET_ID3)

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with later variations of
  dss_get_iface_id().
  ===========================================================================
  ===========================================================================*/
#if !defined(DSS_GET_IFACE_ID_VERS) || (DSS_GET_IFACE_ID_VERS < 3)

static __inline dss_iface_id_type oemdss_get_iface_id(sint15 appid)
{
   dss_id_info_type id;

   id.type = DSS_APP_ID;
   id.info.app_id = appid;

   return dss_get_iface_id(id);
}

#else // !defined(DSS_GET_IFACE_ID_VERS) || (DSS_GET_IFACE_ID_VERS < 3)

#define oemdss_get_iface_id dss_get_iface_id

#endif // !defined(DSS_GET_IFACE_ID_VERS) || (DSS_GET_IFACE_ID_VERS < 3)

#define oemdss_iface_ioctl_all_ifaces_type dss_iface_ioctl_all_ifaces_type

#endif // !defined(DSS_IFACE_GET_ID3)

int oemdss_iface_ioctl(dss_iface_id_type iface_id,
                       dss_iface_ioctl_type ioctl_name,
                       void *argval_ptr,
                       sint15 *dss_errno);

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6100/5150.  DSS_MIN_RCVBUF was defined in 6100 somewhere
  between releases 5015 and 5150.
  ===========================================================================
  ===========================================================================*/
#if !defined(DSS_MIN_RCVBUF)

/*---------------------------------------------------------------------------
  Range values for various socket options
---------------------------------------------------------------------------*/
#define DSS_MIN_RCVBUF (536)
#define DSS_MAX_RCVBUF (18 * 1024)
#define DSS_MIN_SNDBUF (536)
#define DSS_MAX_SNDBUF (18 * 1024)

enum 
{
  DSS_SO_SNDBUF       =  5,        /* set/get the sndbuf queue size        */
  DSS_SO_SDB_ACK_CB   =  7,        /* call a cb upon recv'ing SDB data ack */
  DSS_TCP_NODELAY     =  8,        /* Disable Nagle's algorithm            */
  DSS_SO_KEEPALIVE    =  9,        /* Send keepalive probes?               */
};

enum
{
  DSS_IPPROTO_TCP = 3,               /* TCP protocol level                  */
};

#endif // !defined(DSS_MIN_RCVBUF)

#if !defined(DSS_VERSION) || (DSS_VERSION < 30)
enum 
{
  DSS_TCP_MAXSEG      =  6,        /* set/get the TCP maximum segment size*/
};
#endif // !defined(DSS_VERSION) || (DSS_VERSION < 30)

#if !defined(MSG_EXPEDITE)

/*---------------------------------------------------------------------------
  Flags passed to dss_sendto() call for SDB. Non-SDB related flags are 
  defined in dssocket.h. Any time a SDB related flags is added here, OR it 
  with the existing flags in MSG_VALID_BITS macro definition in dssocki.c 
  so that the reserved bit mask can be properly built.

  SDB flags will cause short data burst to be used if the mobile is not on
  traffic. Don't set any bit if one doesn't want SDB and traffic is to be 
  brought up. Set both bits if one doesn't care which channel is used for
  SDB transmission, else set the appropriate bit.
---------------------------------------------------------------------------*/
#define MSG_EXPEDITE      0x00000001   /* use ACH/REACH/TCH                */
#define MSG_FAST_EXPEDITE 0x00000002   /* use     REACH/TCH                */

/*---------------------------------------------------------------------------
  Enum to indicate the SDB ack status code to the application. The pkt mgr
  receives the status from CM and maps it to the this enum to be returned
  to the application. Changes to CM/Pkt mgr may necessitate modifying this
  enum definition.
---------------------------------------------------------------------------*/
typedef enum dss_sdb_ack_status_enum_type
{
  DSS_SDB_ACK_NONE = -1,                         /* completed successfully */
  DSS_SDB_ACK_OK = 0,                            /* completed successfully */
  DSS_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT = 1,      /* hold orig retry timeout */
  DSS_SDB_ACK_HOLD_ORIG = 2,   /* cannot process because hold orig is true */
  DSS_SDB_ACK_NO_SRV = 3,                                    /* no service */
  DSS_SDB_ACK_ABORT = 4,                                          /* abort */
  DSS_SDB_ACK_NOT_ALLOWED_IN_AMPS = 5,       /* cannot send in analog mode */
  DSS_SDB_ACK_NOT_ALLOWED_IN_HDR = 6,           /* cannot send in HDR call */
  DSS_SDB_ACK_L2_ACK_FAILURE = 7,              /* failure receiving L2 ack */
  DSS_SDB_ACK_OUT_OF_RESOURCES = 8,          /* e.g., out of memory buffer */
  DSS_SDB_ACK_ACCESS_TOO_LARGE = 9,   /* msg too large to be sent over acc */
  DSS_SDB_ACK_DTC_TOO_LARGE = 10,     /* msg too large to be sent over DTC */
  DSS_SDB_ACK_OTHER = 11,          /* any status response other than above */
  DSS_SDB_ACK_ACCT_BLOCK = 12,   /* Access blocked based on service option */
  DSS_SDB_ACK_FORCE_32_BIT = 0x7FFFFFFF     /* Force the enum to be 32-bit */
} dss_sdb_ack_status_enum_type;

/*---------------------------------------------------------------------------
  The structure containing information about the sdb ack status. This info 
  is returned to the socket application in the sdb ack callback if the sdb 
  ack callback socket option is set.
---------------------------------------------------------------------------*/
typedef struct dss_sdb_ack_status_info_type
{
  uint32                       overflow;
  dss_sdb_ack_status_enum_type status;
} dss_sdb_ack_status_info_type;

#endif // !defined(MSG_EXPEDITE)

#if (defined DSS_VERSION) && ((DSS_VERSION >= 20) && (DSS_VERSION <=29))
#define OLD_DS_NET_POLICY_SUPPORTED
#define DSS_GET_APP_PROFILE_ID
#endif // (defined DSS_VERSION) && ((DSS_VERSION >= 20) && DSS_VERSION <=29))

#if (defined DSS_VERSION) && (((DSS_VERSION >= 1705) && (DSS_VERSION < 1800)) || (DSS_VERSION >= 1900))
#define DSS_GET_APP_PROFILE_ID
#endif // (defined DSS_VERSION) && (((DSS_VERSION >= 1705) && (DSS_VERSION < 1800)) || (DSS_VERSION >= 1900))

/*---------------------------------------------------------------------------
Backward compatibility definitions for SDB Ack 
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION <1601))
enum 
{
   DSS_SDB_ACK_L3_ACK_FAILURE = 0xbad000       /* failure receiving L3 ack */
};
#endif // ((!defined DSS_VERSION) || (DSS_VERSION <1601))


/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DMSS releases
  prior to 6500/3030.  DSS_GET_IFACE_ID_BY_POLICY_VERS was defined in 6500 by
  release 3030.
  ===========================================================================
  ===========================================================================*/
#if (!defined(DSS_GET_IFACE_ID_BY_POLICY_VERS) && !defined(OLD_DS_NET_POLICY_SUPPORTED))

/*---------------------------------------------------------------------------
  Definition of iface names
---------------------------------------------------------------------------*/
typedef enum
{
  DSS_IFACE_CDMA_SN      = CDMA_SN_IFACE,
  DSS_IFACE_CDMA_AN      = CDMA_AN_IFACE,
#ifdef FEATURE_DATA_BCMCS
  DSS_IFACE_CDMA_BCAST   = CDMA_BCAST_IFACE,
#endif
  DSS_IFACE_UMTS         = UMTS_IFACE,
  DSS_IFACE_SIO          = SIO_IFACE,
  DSS_IFACE_LO           = LO_IFACE,
  DSS_IFACE_WWAN         = WWAN_GROUP,
  DSS_IFACE_ANY_DEFAULT  = ANY_DEFAULT_GROUP,
  DSS_IFACE_ANY          = ANY_IFACE_GROUP,
  DSS_IFACE_RM           = RM_GROUP
} dss_iface_name_enum_type;

typedef enum
{
  DSS_IFACE_ID = 0,
  DSS_IFACE_NAME = 1
} dss_iface_id_kind_enum_type;

/*---------------------------------------------------------------------------
  dss_iface_id_type will be treated as opaque and typedef'ed to int
  16 bits for iface name, 8 bits for instance and 8 for phys link
  the user will get the iface_id by supplying iface and instance to a 
  function that will return dss_iface_id_type. an attempt to get the
  iface_id for "ANY" iface will return error.
---------------------------------------------------------------------------*/

typedef struct
{
  dss_iface_id_kind_enum_type kind;
  union
  {
    dss_iface_id_type id;
    dss_iface_name_enum_type name;
  } info;
} dss_iface_type;

/*---------------------------------------------------------------------------
                       Network policy data structures.
---------------------------------------------------------------------------*/

typedef enum
{
  DSS_IFACE_POLICY_SPECIFIED = 0,  
  DSS_IFACE_POLICY_UP_ONLY   = 1,
  DSS_IFACE_POLICY_UP_SPEC   = 2 
} dss_iface_policy_flags_enum_type;

typedef struct
{
  dss_iface_policy_flags_enum_type policy_flag; /* Desired policy behavior */ 
  dss_iface_type                   iface;                 /* Desired IFACE */
  boolean                          ipsec_disabled;  /* Is IPSEC disabled ? */
  int                              family;   /* ipv4 or ipv6 or don't care */
  boolean                          is_routeable; /* Is interface routeable?*/
  struct 
  {
    int pdp_profile_num;
  } umts;                              /* UMTS specific policy information */
  struct 
  {
    int cookie;
  } dss_netpolicy_private;         /* Private cookie for internal purposes */
} dss_net_policy_info_type;

static __inline void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
)
{
}

static __inline sint15 dss_set_app_net_policy
(
  sint15 appid,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
)
{
   return DSS_SUCCESS;
}
#endif // !defined(DSS_GET_IFACE_ID_BY_POLICY_VERS) && !defined(OLD_DS_NET_POLICY_SUPPORTED))

#if (!defined(DSS_GET_APP_PROFILE_ID))
static __inline int dss_get_app_profile_id
(
  uint32 app_type
)
{ 
   return DSS_ERROR;
}
#endif // (!defined(DSS_GET_APP_PROFILE_ID))


/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 50
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<50))

enum 
{
  DSS_TCP_DELAYED_ACK =  12,       /* Enable delayed ack                   */
  DSS_TCP_SACK        =  13,       /* Enable SACK                          */
  DSS_TCP_TIME_STAMP  =  14,       /* Enable TCP time stamp option         */
};

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<50))

#if ((!defined DSS_VERSION) || (DSS_VERSION<50))
// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_GOING_DOWN_EV = 0xbad6 /* garbage value */ ,
   DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV = 0xbad7 /* garbage value */ ,
   DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV = 0xbad8 /* garbage value */
};
#endif // ((defined DSS_VERSION) && (DSS_VERSION<50))

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 800
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<800))

enum 
{
  /* Register for a Mobile Terminated event callback */
  DSS_IFACE_IOCTL_MT_REG_CB = DSS_IFACE_IOCTL_MAX,
  /* Deregister for a Mobile Terminated event callback */
  DSS_IFACE_IOCTL_MT_DEREG_CB = DSS_IFACE_IOCTL_MAX
};

/*---------------------------------------------------------------------------
  Mobile Terminated handle type.
---------------------------------------------------------------------------*/
typedef uint32 dss_iface_ioctl_mt_handle_type;

/*---------------------------------------------------------------------------
  Register callback through dss_iface_ioctl(DSS_IFACE_IOCTL_MT_REG_CB)
---------------------------------------------------------------------------*/
typedef struct 
{
  sint15                                app_id;
  dss_iface_ioctl_event_cb              event_cb;
  void                                 *user_data_ptr;
  dss_iface_ioctl_mt_handle_type       *handle;
} dss_iface_ioctl_mt_reg_cb_type; 

/*---------------------------------------------------------------------------
  De-register callback through dss_iface_ioctl(DSS_IFACE_IOCTL_MT_DEREG_CB)
---------------------------------------------------------------------------*/
typedef struct 
{
  sint15                                app_id;
  dss_iface_ioctl_mt_handle_type        handle;
} dss_iface_ioctl_mt_dereg_cb_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<800))

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 1306 and later than 1399
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1306) || \
    ((DSS_VERSION>=1400) && (DSS_VERSION<1804))   || \
    ((DSS_VERSION>=1900) && (DSS_VERSION<2000)))
#define SLIP_IFACE     0xbad4 /* garbage value */
#define DSS_IFACE_SLIP SLIP_IFACE
#endif
/*===========================================================================
   ===========================================================================
   The definitions below allow OEMSock.c to compile with DSS_VERSION values
   that do not support the feature.
   ===========================================================================
   ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1903)|| \
      ((DSS_VERSION>=2000) && (DSS_VERSION<2100)))
#define UW_FMC_IFACE     0xbada /* garbage value */
#define DSS_IFACE_UW_FMC UW_FMC_IFACE
#endif
       	 
/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 900
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<900))
#define WLAN_IFACE     0xbad1 /* garbage value */
#define DSS_IFACE_WLAN WLAN_IFACE 

enum 
{
   DSS_IP_TOS          =  18,       /* Type of Service                      */
   DSS_IPV6_TCLASS     =  19,       /* Traffic Class for V6 sockets         */
};

enum
{
   DSS_IPPROTO_IPV6 = 4,              /* IPV6 protocol level               */
};

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<900))

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 901
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<901))

enum 
{
  /* Get the HW Address of the iface */
  DSS_IFACE_IOCTL_GET_HW_ADDR             = 24,
};

/*---------------------------------------------------------------------------
  Argument to be passed with DSS_IFACE_IOCTL_GET_HW_ADDR to get the HW
  address of the iface.
---------------------------------------------------------------------------*/
typedef struct dss_iface_ioctl_hw_addr_s
{
  uint8    hw_addr_len;          /* size of the buffer provided */
  uint8 *  hw_addr;              /* ptr to buffer to store HW addr */
} dss_iface_ioctl_hw_addr_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<901))

#if ((!defined DSS_VERSION) || (DSS_VERSION<=20))

/*---------------------------------------------------------------------------
 Definitions used for sockaddr_storage structure padding
---------------------------------------------------------------------------*/
#define _SS_MAXSIZE    32
#define _SS_ALIGNSIZE  (sizeof(int64))
#define _SS_PADSIZE    ((_SS_MAXSIZE) - (2 * (_SS_ALIGNSIZE)))

/*---------------------------------------------------------------------------
                    Sockaddr storage structure
---------------------------------------------------------------------------*/

struct sockaddr_storage
{
 uint16       ss_family;             /* address family                     */
 int64       _ss_align;              /* field to force alignment           */
 char        _ss_pad[_SS_PADSIZE];   /* Padding                            */
};

#ifndef AF_UNSPEC
#define AF_UNSPEC 1
#endif

#if !defined(DSS_IFACE_IOCTL_GET_IP_ADDR)
#define DSS_IFACE_IOCTL_GET_IP_ADDR DSS_IFACE_IOCTL_GET_IPV4_ADDR
#endif

#if !defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

/*===========================================================================
   Create fallback inplementation for OEMNet_GetDefaultNetwork() when
   DSS_GET_IFACE_ID_BY_POLICY_VERS is not defined
===========================================================================*/

extern int16 OEMDSS_GetDefaultNetwork
(
   int *pnNetwork
);

extern int16 OEMDSS_GetIfaceIdBeforePppopen
(
   dss_iface_id_type* iface_id
);

#endif //!defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

#endif //!defined DSS_VERSION) || (DSS_VERSION<=20)

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with:
  DSS_VERSION prior to 902 or 
  DSS_VERSION prior to 1100 but later than major version 09
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || \
   (DSS_VERSION<902) || \
   ((DSS_VERSION>=1000)&&(DSS_VERSION<1100)))                                

/*---------------------------------------------------------------------------
  Network down reason.
---------------------------------------------------------------------------*/
typedef enum 
{
   DSS_NET_DOWN_REASON_NOT_SPECIFIED = 0x000bad00,
   DSS_NET_DOWN_REASON_UMTS_REATTACH_REQ,
   DSS_NET_DOWN_REASON_MAX = 0x00badff
} dss_net_down_reason_type;

/*===========================================================================
FUNCTION DSS_LAST_NETDOWNREASON()

DESCRIPTION
  This function provides an interface to the applications for retrieving the
  reason for the network interface going down.

DEPENDENCIES
  None.

PARAMETERS  
  appid     -  application id calling this function.
  reason    -  network down reason.
  dss_errno -  error number.


RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS and places the 
  network down reason in reason. Otherwise, returns DSS_ERROR and places 
  the error number in dss_errno. 

  dss_errno Values
  ---------------
  DS_EBADAPP               invalid application ID specified

SIDE EFFECTS
  None.
===========================================================================*/
sint15
dss_last_netdownreason
(
  sint15                      appid,                /* Application id      */
  dss_net_down_reason_type  * reason,               /* network down reason */
  sint15                    * dss_errno             /* error number        */
);
#endif // ((!defined DSS_VERSION) || (DSS_VERSION<902) || ((DSS_VERSION>=1000)&&(DSS_VERSION<1100))) 

#if ((!defined DSS_VERSION) || (DSS_VERSION<1100))
// dss_net_down_reason_type 
enum {
   DSS_NET_DOWN_REASON_CLOSE_IN_PROGRESS = 0x000bad02,
   DSS_NET_DOWN_REASON_NW_INITIATED_TERMINATION
};
#endif //((!defined DSS_VERSION) || (DSS_VERSION<1100))

#if ((!defined DSS_VERSION) || (DSS_VERSION<1200))
// dss_net_down_reason_type 
enum {
      DSS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES = 0x000bad04,
      DSS_NET_DOWN_REASON_UNKNOWN_APN,
      DSS_NET_DOWN_REASON_UNKNOWN_PDP,
      DSS_NET_DOWN_REASON_AUTH_FAILED,
      DSS_NET_DOWN_REASON_GGSN_REJECT,
      DSS_NET_DOWN_REASON_ACTIVATION_REJECT,
      DSS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED,
      DSS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED,
      DSS_NET_DOWN_REASON_OPTION_TEMP_OOO,
      DSS_NET_DOWN_REASON_NSAPI_ALREADY_USED,
      DSS_NET_DOWN_REASON_REGULAR_DEACTIVATION,
      DSS_NET_DOWN_REASON_QOS_NOT_ACCEPTED,
      DSS_NET_DOWN_REASON_NETWORK_FAILURE,
      DSS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR,
      DSS_NET_DOWN_REASON_TFT_SYNTAX_ERROR,
      DSS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT,
      DSS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR,
      DSS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR,
      DSS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT,
      DSS_NET_DOWN_REASON_INVALID_TRANSACTION_ID,
      DSS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC,
      DSS_NET_DOWN_REASON_INVALID_MANDATORY_INFO,
      DSS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED,
      DSS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE,
      DSS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT,
      DSS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR,
      DSS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE,
      DSS_NET_DOWN_REASON_PROTOCOL_ERROR
};
#endif //((!defined DSS_VERSION) || (DSS_VERSION<1200))


// The following net down reasons are defined for DSS Versions:
// 1401 (6280 4.x) , 1501 (6260 2.4, 7200 3.x), 1600 and higher
#if ( (!defined DSS_VERSION) || (DSS_VERSION<=1400) || (1500 == DSS_VERSION) )
// dss_net_down_reason_type 
enum {
  DSS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE  = 0x000bad20,
  DSS_NET_DOWN_REASON_INTERNAL_MIN,
  DSS_NET_DOWN_REASON_INTERNAL_ERROR,
  DSS_NET_DOWN_REASON_INTERNAL_CALL_ENDED,                                      
  DSS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE,
  DSS_NET_DOWN_REASON_INTERNAL_MAX,
};

#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<=1400) || (1500 == DSS_VERSION) )

// The following net down reasons are defined for DSS Versions:
// 1504-1599 and 1702 and higher.
#if ( (!defined DSS_VERSION) || (DSS_VERSION<1504) || ((DSS_VERSION>1599)&&(DSS_VERSION<1702)) )
// dss_net_down_reason_type 
enum {
   DSS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING  = 0x000bad26, 
   DSS_NET_DOWN_REASON_LLC_SNDCP_FAILURE,
   DSS_NET_DOWN_REASON_APN_TYPE_CONFLICT 
};

#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<1504) || ((DSS_VERSION>1599)&&(DSS_VERSION<1702)) )

// The following net down reasons are defined for DSS Versions:
// 1705-1799, 1901 and higher.
#if ((!defined DSS_VERSION) || ((DSS_VERSION < 1705) || (((DSS_VERSION >= 1800) && (DSS_VERSION < 1802)) || ((DSS_VERSION == 1900)))))
// dss_net_down_reason_type
enum {
   DSS_NET_DOWN_REASON_EAP_MIN = 0x000bad30,
   DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS,
   DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS,
   DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES,
   DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH,
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH,
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH,
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS,
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED,
   DSS_NET_DOWN_REASON_EAP_SUCCESS, 
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE, 
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE, 
   DSS_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED,
   DSS_NET_DOWN_REASON_EAP_MAX,
   DSS_NET_DOWN_REASON_IPSEC_MIN,
   DSS_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE,
   DSS_NET_DOWN_REASON_IPSEC_AUTH_FAILED,
   DSS_NET_DOWN_REASON_IPSEC_CERT_INVALID,
   DSS_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR,
   DSS_NET_DOWN_REASON_IPSEC_MAX
};
#endif // ((!defined DSS_VERSION) || ((DSS_VERSION < 1705) || ((DSS_VERSION == 1800) || ((DSS_VERSION == 1900)))))

#if ((!defined DSS_VERSION) || (DSS_VERSION<1000))

int oemdss_qos_iface_ioctl
( 
   dss_iface_id_type        iface_id,
   dss_iface_ioctl_type     ioctl_name,
   void                     *argval_ptr,
   sint15                   *dss_errno
);

enum 
{
  /* Get QOS staus */
  DSS_IFACE_IOCTL_QOS_GET_STATUS          = 0x40000014,
  /* Bring flow out of dormancy */
  DSS_IFACE_IOCTL_QOS_RESUME              = 0x40000015,
  /* Suspend a flow */
  DSS_IFACE_IOCTL_QOS_SUSPEND             = 0x40000016,
};

/*---------------------------------------------------------------------------
  QOS handle type.
---------------------------------------------------------------------------*/
typedef dss_iface_id_type dss_qos_handle_type;

typedef enum
{
  QOS_STATE_INVALID = 0x00,
  QOS_UNAVAILABLE   = 0x01,
  QOS_ACTIVATING    = 0x02,
  QOS_AVAILABLE     = 0x04,
  QOS_SUSPENDING    = 0x08,
  QOS_DEACTIVATED   = 0x10,
  QOS_RELEASING     = 0x20
} dss_iface_ioctl_qos_status_type;

/*---------------------------------------------------------------------------
  Data type used for getting QOS status
  (DSS_IFACE_IOCTL_QOS_GET_STATUS)
---------------------------------------------------------------------------*/
typedef struct 
{
  dss_qos_handle_type              handle;       /* Handle to QOS instance */

  /* OUTPUT param */
  dss_iface_ioctl_qos_status_type  qos_status;   /* status of QOS instance */
} dss_iface_ioctl_qos_get_status_type;

/*---------------------------------------------------------------------------
  Data type used for resuming the QOS parameters 
  (DSS_IFACE_IOCTL_QOS_RESUME)
---------------------------------------------------------------------------*/
typedef struct 
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
} dss_iface_ioctl_qos_resume_type;

/*---------------------------------------------------------------------------
  Data type used for suspending the QOS parameters 
  (DSS_IFACE_IOCTL_QOS_SUSPEND)
---------------------------------------------------------------------------*/
typedef struct 
{
  dss_qos_handle_type                   handle;  /* Handle to QOS instance */
} dss_iface_ioctl_qos_suspend_type;
#else //((!defined DSS_VERSION) || (DSS_VERSION<1000))

#define oemdss_qos_iface_ioctl dss_iface_ioctl

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1000))

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 1003
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1003))

// -----------------------
// FILE: dss_iface_ioctl.h
// -----------------------

// dss_iface_ioctl_type
enum {
   DSS_IFACE_IOCTL_MCAST_JOIN   = 0xbad1, /* garbage values */
   DSS_IFACE_IOCTL_MCAST_LEAVE
};

typedef uint32 dss_iface_ioctl_mcast_handle_type;

/*---------------------------------------------------------------------------
Join a Multicast group address and port
---------------------------------------------------------------------------*/
typedef struct 
{
   ip_addr_type               ip_addr;          /* Multicast IP address   */
   uint16                     port;             /* Multicast Port         */
   void                       *mcast_param_ptr; /* Multicast input params */ 
   
   dss_iface_ioctl_event_cb   event_cb;         /* Callback for mcast ev  */
   void                       *user_data_ptr;   /* App specific data, 
   passed in cback        */
   sint15                     app_id;           /* appid                  */
   
   /* Output Paramters */
   uint32                     handle;           /* Multicast handle       */
} dss_iface_ioctl_mcast_join_type;

/*---------------------------------------------------------------------------
Leave a Multicast group using handle assigned by the mode handler
---------------------------------------------------------------------------*/
typedef struct
{
   uint32                          handle;
   sint15                          app_id;
} dss_iface_ioctl_mcast_leave_type;  


// -------------------------
// FILES: ps_iface_defs.h
//        dss_socket_defs.h
// -------------------------
// ps_iface_name_enum_type
#define FLO_IFACE     0xbad2 /* garbage value */
// dss_iface_name_enum_type
#define DSS_IFACE_FLO FLO_IFACE 

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1003))

/*===========================================================================
           DS version compatibility for BCMCS info codes
===========================================================================*/

#if ( (!defined DSS_VERSION) || (DSS_VERSION<1003) )

// On these versions no BCMCS info codes were defined so
// we need to define them all here instead.
typedef enum 
{
   DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED = 0,
      
   /* BCMCS info codes */
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 0xbad1, /* garbage values */
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT,
   DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_FLOWS_REACHED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG,
   DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED,
   DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE

   /* Any additional info code, ie. DVBH */
} dss_iface_ioctl_mcast_info_code_enum_type;

// Basic set of BCMCS info codes introduced in 1003.
// For later versions (as specified below) define only
// those codes not yet introduced.
#elif ( (DSS_VERSION>=1003 && DSS_VERSION<1004) ||   \
		(DSS_VERSION>=1100 && DSS_VERSION<1106) ||	  \
		(DSS_VERSION>=1200 && DSS_VERSION<1203)	     \
      )  

enum {
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE = 0xbad1, // garbage values
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG,
   DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED,
   DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE
};

// Four new info codes introduced in versions 1004, 1106 and 1203.
// No need to define them here for these versions and later.
// In addition, at those versions AN_REJECT was deleted so we 
// define it here again for these versions and later.
#elif ( (DSS_VERSION>=1004 && DSS_VERSION<1006) ||   \
		(DSS_VERSION>=1106 && DSS_VERSION<1107) ||	  \
		(DSS_VERSION>=1203 && DSS_VERSION<1206)	     \
      )  

enum {
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT, = 0xbad1, // garbage values
   DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE
};


// SYS_UNAVAILABLE code introduced in versions 1006, 1107 and 1206.
// No need to define it here for these versions and later.
#elif ( (DSS_VERSION>=1006 && DSS_VERSION<1100) ||   \
		(DSS_VERSION>=1107 && DSS_VERSION<1200) ||	  \
		(DSS_VERSION>=1206)	                          \
      )  

enum {
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT = 0xbad1 /* garbage values */
};

#endif

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with AMSS that 
  does not support BCMCS_DB_UPDATE
  ===========================================================================
  ===========================================================================*/

#if ((defined(DSS_VERSION) && DSS_VERSION >= 1003 && DSS_VERSION < 1100) || \
     (defined(DSS_VERSION) && DSS_VERSION >= 1101))

#ifdef FEATURE_BCMCS
   #define OEM_PROVIDES_BCMCS_DB_UPDATE
#endif

#else

// -----------------------
// FILE: dsbcmcs_defs.h
// -----------------------

/*---------------------------------------------------------------------------
  The BCMCS flow id type. This can be different for 1X and HDR
---------------------------------------------------------------------------*/
//VR - move this to MC files before checkin
typedef uint32 dsbcmcs_hdr_flow_id_type;

/*---------------------------------------------------------------------------
  BCDB zone type - it will be a SUBNET for HDR or a PZID for 1X. In the 
  future it can be expanded to support other technologies like UMTS, etc. 

  When requesting for the link layer information for an {IP, port},
  the calling module must specify the zone since the information may  be 
  different for different zones.
---------------------------------------------------------------------------*/

// hdraddr_type definition from:
// "API for Updating BCMCS Flow Mapping Information (80-V4186-1 A)"
#define HDRADDR_LENGTH_IN_BYTES 16
typedef unsigned char hdraddr_type[HDRADDR_LENGTH_IN_BYTES];

typedef enum
{
  DSBCMCS_ZONE_1X   = 0,
  DSBCMCS_ZONE_HDR  = 1,

  DSBCMCS_ZONE_MAX
} dsbcmcs_zone_enum_type;

typedef struct
{
  dsbcmcs_zone_enum_type type;

  union
  {
    hdraddr_type subnet;
    /*-----------------------------------------------------------------------
       Expand here for 1X, UMTS, etc. 
       Making this an union since the "zone" may not be 32-bits for other
       technolgies.
    -----------------------------------------------------------------------*/
  } zone;

} dsbcmcs_zone_type; 

/*---------------------------------------------------------------------------
  Define the BCMCS flow id to be an union so that it can be expandable in the 
  future 
---------------------------------------------------------------------------*/
typedef union
{
  dsbcmcs_hdr_flow_id_type hdr_flow_id;
  /*-------------------------------------------------------------------------
    Add more fields here later if 1X/UMTS have different flow_id types.
    Make this an union since this struct will be used by other modules,
    so if this is an union the other modules' code doesn't need to change
    later.
  -------------------------------------------------------------------------*/
} dsbcmcs_flow_id_type;

/*---------------------------------------------------------------------------
  BCMCS framing type
---------------------------------------------------------------------------*/
typedef enum
{
  DSBCMCS_FRAMING_SEGMENT = 0,    /* Segment based framing                 */
  DSBCMCS_FRAMING_HDLC    = 1,    /* HDLC-like framing                     */

  DSBCMCS_FRAMING_MAX
} dsbcmcs_framing_enum_type;

/*---------------------------------------------------------------------------
  The upper layer protocol obtained after deframing the BCMCS packet.
---------------------------------------------------------------------------*/
typedef enum
{
  DSBCMCS_PROTOCOL_PPP  = 0,  /*the packet is PPP encapsulated (there are one
                             or two protocol bytes preceding the IP packet)*/
  DSBCMCS_PROTOCOL_IPv4 = 1, /*the packet obtained after deframing is an IPv4 
                             packet without any further encapsulation.     */
  DSBCMCS_PROTOCOL_IPv6 = 2, /*the packet obtained after deframing is an IPv6 
                             packet without any further encapsulation.     */

  /* Add other protocols in the future if needed */
  BCMCS_PROTOCOL_MAX
} dsbcmcs_protocol_enum_type;


// -----------------------
// FILE: ps_iface_ioctl.h
// -----------------------

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_BCMCS_DB_UPDATE

  Fields are
    overwrite    : Overwrite an entry if current one is found.

    Please see the BCMCS documentation for the remaining descriptions
    of the flow specific parameters.
---------------------------------------------------------------------------*/

typedef struct
{
  dsbcmcs_zone_type zone;
  ip_addr_type      multicast_ip;
  uint32            program_id;
  uint8             program_id_len;
  uint8             flow_discrim_len;
  uint8             flow_discrim;
  uint16            port;
  dsbcmcs_framing_enum_type  framing;
  dsbcmcs_protocol_enum_type protocol;
  uint8   crc_len;
  uint8   flow_format;
  uint32  flow_id; // may become dsbcmcs_flow_id_type (union)
  // Flow id length was added in dss versions 1703, 1801 and later.
  uint8   flow_id_len;
  boolean overwrite;
} ps_iface_ioctl_bcmcs_db_update_type;


// -----------------------
// FILE: dss_iface_ioctl.h
// -----------------------

typedef ps_iface_ioctl_bcmcs_db_update_type dss_iface_ioctl_bcmcs_db_update_type;

enum {
   DSS_IFACE_IOCTL_BCMCS_DB_UPDATE = 0xbad4
};

#endif


/*===========================================================================
           DS version compatibility for DVB-H definition
===========================================================================*/
#if ( (!defined DSS_VERSION) || (DSS_VERSION<1207) )
// ps_iface_name_enum_type
#define DVBH_IFACE     0xbad3 /* garbage value */
// dss_iface_name_enum_type
#define DSS_IFACE_DVBH DVBH_IFACE
#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1207))

#if (!defined(DSS_VERSION) || (DSS_VERSION < 50))
typedef uint8 qos_spec_field_mask_type;
typedef void ip_flow_spec_type;
typedef void ip_flow_type;
typedef uint32 ipflow_field_mask_type;
#endif

/*---------------------------------------------------------------------------
  Functions used for QOS Auxiliary Flows   
---------------------------------------------------------------------------*/
// If QoS Auxiliary Flows is supported, adds QOS_MASK_RX_AUXILIARY_FLOWS to the
// given QoS spec field mask. Otherwise, does nothing.
void     OEMDSS_SetRxAuxFlowMask(qos_spec_field_mask_type* pFieldMask);
// If QoS Auxiliary Flows is supported, adds QOS_MASK_TX_AUXILIARY_FLOWS to the
// given QoS spec field mask. Otherwise, does nothing.
void     OEMDSS_SetTxAuxFlowMask(qos_spec_field_mask_type* pFieldMask);
// Returns whether or not the QOS_MASK_RX_AUXILIARY_FLOWS is set in the given
// QoS spec field mask.
boolean  OEMDSS_IsRxAuxFlowMask(qos_spec_field_mask_type field_mask);
// Returns whether or not the QOS_MASK_TX_AUXILIARY_FLOWS is set in the given
// QoS spec field mask.
boolean  OEMDSS_IsTxAuxFlowMask(qos_spec_field_mask_type field_mask);
// If QoS Auxiliary Flows is supported, sets the number of auxiliary flows to 
// the given value. Otherwise, does nothing.
void     OEMDSS_SetNumAuxFlows(ip_flow_spec_type* pFlowSpec, uint8 uNumAuxFlows);
// If QoS Auxiliary Flows is supported, returns the number of auxiliary flows 
// in the given flow spec. Otherwise, returns 0.
uint8    OEMDSS_GetNumAuxFlows(ip_flow_spec_type* pFlowSpec);
// If QoS Auxiliary Flows is supported, returns the pointer to the flow 
// in the requested index. Otherwise, returns NULL.
ip_flow_type* OEMDSS_GetAuxFlow(ip_flow_spec_type* pFlowSpec, uint8 uIndex);
// If QoS Auxiliary Flows is supported, allocates memory for the Auxiliary Flow
// List. Otherwise, does nothing.
uint16   OEMDSS_MallocAuxFlowList(ip_flow_spec_type* pFlowSpec, uint8 uNumFlows);
// If QoS Auxiliary Flows is supported, frees the allocated memory for the 
// auxiliary flow list and updates the number of auxiliary flows to 0. 
// Otherwise, does nothing.
void     OEMDSS_FreeAuxFlowList(ip_flow_spec_type* pFlowSpec);
// If QoS Auxiliary Flows is supported, returns the error mask for a flow in 
// the auxiliary flow list, according to the requested index. 
// Otherwise, returns 0.
ipflow_field_mask_type OEMDSS_GetAuxFlowErrMask(ip_flow_spec_type* pFlowSpec, uint8 uIndex);   

#ifdef OEM_PROVIDES_QOS

#if (!defined DSS_VERSION) || (DSS_VERSION < 1100)
#define DSS_IFACE_IOCTL_QOS_REQUEST_EX 25
#define DSS_IFACE_IOCTL_QOS_RELEASE_EX 0x40000017
#define DSS_IFACE_IOCTL_QOS_SUSPEND_EX 0x40000018
#define DSS_IFACE_IOCTL_QOS_RESUME_EX  0x40000019

typedef enum
{
   DSS_IFACE_IOCTL_QOS_REQUEST_OP,
   DSS_IFACE_IOCTL_QOS_CONFIGURE_OP
} dss_iface_ioctl_qos_request_ex_opcode_enum_type;


/*---------------------------------------------------------------------------
Data type used for specifying the QOS parameters when app requests QOS or
to configure QOS
(DSS_IFACE_IOCTL_QOS_REQUEST_EX)
---------------------------------------------------------------------------*/
typedef struct
{
   sint15                     app_id;                  /* Client's App Id   */
   dss_iface_ioctl_qos_request_ex_opcode_enum_type opcode; 
   /* Request type      */
   uint8                      num_qos_specs;           /* Num QOS requests  */
   uint8                      qos_control_flags;       /* Extra QOS options -
                                                        not used currently   */
   qos_spec_type            * qos_specs_ptr;          /* QOS specifications */
   dss_iface_ioctl_event_cb   cback_fn;  /* QOS event notification callback */
   void                     * user_data; /* User data passed to callback    */

   /* OUTPUT Parameter */
   dss_qos_handle_type      * handles_ptr; /* Handles to the QOS instances,
                                          use for all subsequent IOCTLs
                                          on these QOS instances          */
} dss_iface_ioctl_qos_request_ex_type;

/*---------------------------------------------------------------------------
Data type used for suspending/resuming and releasing multiple QOS instances
(DSS_IFACE_IOCTL_QOS_RELEASE_EX)
(DSS_IFACE_IOCTL_QOS_SUSPEND_EX)
(DSS_IFACE_IOCTL_QOS_RESUME_EX)
---------------------------------------------------------------------------*/
typedef struct dss_iface_ioctl_qos_release_s
{
   uint8                  num_handles;                        /* Num flows passed    */
   dss_qos_handle_type  * handles_ptr;
} dss_iface_ioctl_qos_release_ex_type;

typedef struct dss_iface_ioctl_qos_release_s dss_iface_ioctl_qos_suspend_ex_type;
typedef struct dss_iface_ioctl_qos_release_s dss_iface_ioctl_qos_resume_ex_type;

#endif // (!defined DSS_VERSION) || (DSS_VERSION < 1100)

#endif // #ifdef OEM_PROVIDES_QOS

/*---------------------------------------------------------------------------
   QoS Aware/Unaware notification 
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1007))
// dss_iface_ioctl_extended_info_code_enum_type:
enum 
{
   DSS_IFACE_IOCTL_EIC_QOS_AWARE_SYSTEM = 0xbad3, /* garbage value */ 
   DSS_IFACE_IOCTL_EIC_QOS_UNAWARE_SYSTEM
};
// dss_iface_ioctl_event_enum_type:
enum
{   
   DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV = 0xbad3, /* garbage value */
   DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV  
};
#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1007))

#if ( !defined DSS_VERSION || \
      ((DSS_VERSION >= 1100) && (DSS_VERSION < 1105)) || \
      ((DSS_VERSION >= 1200) && (DSS_VERSION < 1202)) )
// Support both - new and old info code names. 
// The OEM always refers to the new names. In case the old info code names are 
// defined, rename them to the new names.
#define DSS_IFACE_IOCTL_EIC_QOS_AWARE_SYSTEM   DSS_IFACE_IOCTL_EIC_QOS_HANDOFF_TO_QOS_AWARE_SYSTEM  
#define DSS_IFACE_IOCTL_EIC_QOS_UNAWARE_SYSTEM DSS_IFACE_IOCTL_EIC_QOS_HANDOFF_TO_QOS_UNAWARE_SYSTEM
#endif // ( !defined DSS_VERSION || ((DSS_VERSION >= 1100) && (DSS_VERSION < 1105)) || ((DSS_VERSION >= 1200) && (DSS_VERSION < 1202)) )

#if ((!defined DSS_VERSION) || \
     (DSS_VERSION<1106) || \
     (DSS_VERSION>=1200 && DSS_VERSION<1203))
// dss_iface_ioctl_extended_info_code_enum_type:
enum 
{
   DSS_IFACE_IOCTL_EIC_QOS_REJECTED_OPERATION = 0xbad5, /* garbage value */ 
   DSS_IFACE_IOCTL_EIC_QOS_TIMED_OUT_OPERATION,
   DSS_IFACE_IOCTL_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED
};
#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1106) || (DSS_VERSION>=1200 && DSS_VERSION<1203))

#if ( (!defined DSS_VERSION) || \
      (DSS_VERSION<1007) || \
      ((DSS_VERSION>=1100) && (DSS_VERSION<1103)) )
// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM = 0xbad3 /* garbage value */
};
typedef boolean dss_iface_ioctl_on_qos_aware_system_type;
#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<1007) || ((DSS_VERSION>=1100) && (DSS_VERSION<1103)) )

#if ( (!defined DSS_VERSION) || \
      (DSS_VERSION<1007) || \
      ((DSS_VERSION>=1100) && (DSS_VERSION<1104)) )
#define DS_EQOSUNAWARE    150         /* Mobile is in a QOS Unaware System */
#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<1007) || ((DSS_VERSION>=1100) && (DSS_VERSION<1104)) )

/*---------------------------------------------------------------------------
Functions used for WLAN QOS Flow options: 
   User Priority,
   Min Service Interval,
   Max Service Interval,
   Inactivity  Interval
And for the QoS Flow options:
   Nominal SDU Size
   CDMA Flow Priority
---------------------------------------------------------------------------*/
// If the specified QoS Flow options are supported and if the options are set, 
// translates them from OEM representation to AMSS representation.
// If the specified QoS Flow options are not supported does nothing, 
// returns AEE_NET_SUCCESS.
uint16 OEMDSS_TranslateOEMFlowOptToAMSS(oem_ip_flow_type* pOemFlow, ip_flow_type* pAMSSFlow);
// If the specified QoS Flow option are supported and if the options are set, 
// translates them from AMSS representation to OEM representation.
// If the specified QoS Flow options are not supported does nothing, 
// returns AEE_NET_SUCCESS.
uint16 OEMDSS_TranslateAMSSFlowOptToOEM(ip_flow_type* pAMSSFlow, oem_ip_flow_type* pOemFlow);

/*---------------------------------------------------------------------------
Functions used for CDMA data session profile id   
---------------------------------------------------------------------------*/
void OEMDSS_SetCDMAProfileId(int nNetwork, dss_net_policy_info_type* policy_info);

#if ( (!defined DSS_VERSION) || \
      (DSS_VERSION<1106)     || \
      (DSS_VERSION>=1200 && DSS_VERSION<1204) )

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES = 0xbad8 /* garbage value */
};

/*---------------------------------------------------------------------------
  Argument to be passed with PS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES
---------------------------------------------------------------------------*/
#define MAX_NETWORK_SUPPORTED_PROFILE_COUNT 255

typedef struct
{
  uint8  profile_count;
  uint16 profile_value[MAX_NETWORK_SUPPORTED_PROFILE_COUNT];
} ps_iface_ioctl_get_network_supported_qos_profiles_type;

/*---------------------------------------------------------------------------
  Data type used for getting the network supported QoS profiles
  (DSS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES)
---------------------------------------------------------------------------*/
typedef ps_iface_ioctl_get_network_supported_qos_profiles_type
   dss_iface_ioctl_get_network_supported_qos_profiles_type;

#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<1106) || (DSS_VERSION>=1200 && DSS_VERSION<1204) )

#if ( (!defined DSS_VERSION) || (DSS_VERSION<1303 ) )
// dss_iface_ioctl_event_enum_type:
enum
{   
   DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV = 0xbad5, /* garbage value */
};
#endif // #if ( (!defined DSS_VERSION) || (DSS_VERSION<1303 ) )

/*---------------------------------------------------------------------------
// Backward compatibility definitions for feature "IOCTL to query P-CSCF/domain"
---------------------------------------------------------------------------*/
#if ( (!defined DSS_VERSION) || \
      (DSS_VERSION<1300) )      

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_GET_SIP_SERV_ADDR = 0xbad9, /* garbage value */
   DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES = 0xbad10 /* garbage value */
};


// DSS_IFACE_IOCTL_GET_SIP_SERV_ADDRS parameter type
//    List of SIP Server Addresses.
typedef struct
{
  uint32            count;
  ip_addr_type     *addr_array;
} dss_iface_ioctl_sip_serv_addr_info_type;

// DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES parameter type
//    List of SIP Server Domain Names.
#define PS_IFACE_MAX_DOMAIN_NAME_SIZE    256
#define DSS_IFACE_MAX_DOMAIN_NAME_SIZE  PS_IFACE_MAX_DOMAIN_NAME_SIZE

// Domain name type. Used for SIP server domain names.
typedef struct
{
  char domain_name[PS_IFACE_MAX_DOMAIN_NAME_SIZE];
} ps_iface_domain_name_type;

typedef ps_iface_domain_name_type   dss_iface_ioctl_domain_name_type;

typedef struct
{
  uint32                             count;
  dss_iface_ioctl_domain_name_type  *name_array;
} dss_iface_ioctl_sip_serv_domain_name_info_type;

#endif // ( (!defined DSS_VERSION) || (DSS_VERSION<1300) )

/*---------------------------------------------------------------------------
Backward compatibility definitions for the Bearer Technology related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1100))

// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV = 0xbad11, /* garbage value */ 
};

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY = 0xbad12, /* garbage value */
};

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_BEARER_TECHNOLOGY_TYPE

DESCRIPTION
  The type that is used to define the bearer technology type.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_IFACE_NETWORK_MIN = 0,
  PS_IFACE_NETWORK_CDMA,
  PS_IFACE_NETWORK_UMTS,
  PS_IFACE_NETWORK_WLAN,
  PS_IFACE_NETWORK_MAX
}ps_iface_network_type;

typedef struct
{
  uint32  rat_mask;
  #define PS_IFACE_CDMA_1X               0x01
  #define PS_IFACE_CDMA_EVDO_REV0        0x02
  #define PS_IFACE_CDMA_EVDO_REVA        0x04
}ps_iface_network_cdma_type;

typedef struct
{
  uint32  rat_mask;
  #define PS_IFACE_UMTS_WCDMA             0x01
  #define PS_IFACE_UMTS_GPRS              0x02
  #define PS_IFACE_UMTS_HSDPA             0x04
  #define PS_IFACE_UMTS_HSUPA             0x08
  #define PS_IFACE_UMTS_EDGE              0x10
}ps_iface_network_umts_type;

typedef struct
{
  ps_iface_network_type  current_network;
  union
  {
    ps_iface_network_cdma_type      cdma_type;
    ps_iface_network_umts_type      umts_type;
  }data;
}ps_iface_bearer_technology_type;

typedef ps_iface_bearer_technology_type  ps_iface_ioctl_bearer_tech_type;
typedef ps_iface_ioctl_bearer_tech_type  dss_iface_ioctl_bearer_tech_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1100))

/*-------------------------------------------------------------------------------------
Backward compatibility definitions for the IOCTL enum of IPv6 Privacy Address extension
-------------------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1500)) 

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR = 0xbad16, /* garbage value */ 
};

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1500))

/*------------------------------------------------------------------------
Other backward compatibility definitions of IPv6 Privacy Address extension
------------------------------------------------------------------------*/

#if ((!defined DSS_VERSION) || (DSS_VERSION<1500) || (!defined FEATURE_DATA_PS_IPV6))

// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV  = 0xbad13, /* garbage value */
   DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV = 0xbad14, /* garbage value */
   DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV    = 0xbad15, /* garbage value */ 
};

/*---------------------------------------------------------------------------
TYPEDEF DSS_IFACE_IOCTL_IPV6_IID_PARAMS_TYPE

DESCRIPTION
  Parameters for generating specific types of privacy addresses.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean is_unique;              /* Request a unique address or shareable */
} dss_iface_ioctl_ipv6_iid_params_type;

/*---------------------------------------------------------------------------
  DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR parameter type
---------------------------------------------------------------------------*/
typedef struct
{
  sint15                               dss_nethandle;
  dss_iface_ioctl_ipv6_iid_params_type iid_params;
  dss_iface_ioctl_event_cb             event_cb;         /* event callback */
  void                                *user_data_ptr;/* User data to cback */

  /* OUTPUT PARAMETER */
  ip_addr_type                         *ip_addr;
} dss_iface_ioctl_priv_ipv6_addr_type;

typedef struct
{
   ip_addr_type ip_addr;  
   boolean      is_unique;
} dss_iface_ioctl_priv_addr_info_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1500) || (!defined FEATURE_DATA_PS_IPV6))

#if (!defined DSS_VERSION || DSS_VERSION < 1100)
// dss_iface_ioctl_type for retrieve the name of the associated interface
enum 
{
   DSS_IFACE_IOCTL_GET_IFACE_NAME = 0xbad6 /* garbage value */ 
};

/*---------------------------------------------------------------------------
 DSS_IFACE_IOCTL_GET_IFACE_NAME parameter type 
 - Iface name
---------------------------------------------------------------------------*/
typedef int dss_iface_ioctl_iface_name_type;

#endif // (!defined DSS_VERSION || DSS_VERSION < 1100)

#ifdef OEM_PROVIDES_QOS
/*---------------------------------------------------------------------------
Backward compatibility definitions for "Query QoS During Any State" feature
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1300))

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2 = 0xbad17, /* garbage value */ 
};

/*---------------------------------------------------------------------------
  Data type used for retrieving the QOS flow spec
  (DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2)
---------------------------------------------------------------------------*/
typedef struct
{
  dss_qos_handle_type               handle;       /* Handle to QOS instance */

  /* OUTPUT Parameters */
  ip_flow_type                      rx_flow;      /* Rx Flow spec           */
  ip_flow_type                      tx_flow;      /* Tx Flow spec           */
  dss_iface_ioctl_qos_status_type   qos_status;   /* status of QOS instance */
} dss_iface_ioctl_qos_get_granted_flow_spec2_type;

#endif // #if ((!defined DSS_VERSION) || (DSS_VERSION<1300))

#endif // OEM_PROVIDES_QOS

/*---------------------------------------------------------------------------
   Backward compatibility definitions for DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1100))
#define OEM_DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC
#else
#define OEM_DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC
#endif // #if ((!defined DSS_VERSION) || (DSS_VERSION<1100))

/*===========================================================================
===========================================================================
The definitions below allow OEMSock.c to compile with DSS_VERSION
prior to 1400
===========================================================================
===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1400))

enum 
{
   DSS_SO_REUSEADDR = 28 /* Enable Socket Reuse */      
};

#endif // #if ((!defined DSS_VERSION) || (DSS_VERSION<1400))

/*===========================================================================
  ===========================================================================
  The definitions below allow OEMSock.c to compile with DSS_VERSION
  prior to 1600
  ===========================================================================
  ===========================================================================*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1600))

enum 
{
   DSS_SO_DISABLE_FLOW_FWDING = 29 /* Disables routing data on best effort */   
};

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1600))

#ifdef OEMDSS_NETLIB2_NOT_SUPPORTED 
/*===========================================================================

MACRO DSS_CLOSE_NETLIB2()

DESCRIPTION
This macro makes a dss_close_netlib2() call available corresponding to
dss_open_netlib2() for the sake of uniformity. Since dss_close_netlib2()
is no different from dss_close_netlib(), it simply calls the latter with
the passed parameters.

DEPENDENCIES
None.

RETURN VALUE
Same as dss_close_netlib()

SIDE EFFECTS
===========================================================================*/
#define dss_close_netlib2(appid, errno) dss_close_netlib(appid, errno)

typedef void (*dss_net_cb_fcn)
(
   sint15            appid,     /* Application id */
   dss_iface_id_type iface_id,  /* Interfcae id structure */
   sint15            dss_errno, /* type of network error, ENETISCONN, ENETNONET.*/
   void*             net_cb_user_data /* Call back User data  */
);

typedef void (*dss_sock_cb_fcn)
(
   sint15 appid,      /* Application id */
   sint15 sockfd,     /* socket descriptor */
   uint32 event_mask, /* Event occurred */
   void * sock_cb_user_data /* User data specified during registration */
);


sint15 dss_open_netlib2
(
   dss_net_cb_fcn net_cb,    /* network callback function */
   void *  net_cb_user_data, /* User data for network call back */ 
   dss_sock_cb_fcn sock_cb,  /* socket callback function */
   void * sock_cb_user_data, /* User data for socket call back */ 
   dss_net_policy_info_type * policy_info_ptr, /* Network policy info */ 
   sint15 *dss_errno                           /* error condition value */
);
#endif // OEMDSS_NETLIB2_NOT_SUPPORTED 

/*---------------------------------------------------------------------------
Backward compatibility definitions for the Outage related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1600))

// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV = 0xbad19, /* garbage value */ 
};

typedef struct
{
  uint32 time_to_outage;
  uint32 duration;
} dss_iface_ioctl_outage_notification_event_info_type;

#endif //((!defined DSS_VERSION) || (DSS_VERSION<1600)))

/*---------------------------------------------------------------------------
Backward compatibility definitions for the QoS Profiles Changed related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1600))

// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV = 0xbad1a, /* garbage value */ 
};

#endif //((!defined DSS_VERSION) || (DSS_VERSION<1600)))

/*---------------------------------------------------------------------------
Backward compatibility definitions for the IPv6 Prefix Update related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1400) || (!defined FEATURE_DATA_PS_IPV6))

// dss_iface_ioctl_event_enum_type:
enum 
{
   DSS_IFACE_IOCTL_PREFIX_UPDATE_EV = 0xbad18, /* garbage value */ 
};

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1400) || (!defined FEATURE_DATA_PS_IPV6))

/*---------------------------------------------------------------------------
Additional backward compatibility definitions for the IPv6 Prefix Update related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1500))

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_ADDR_STATE_ENUM_TYPE

DESCRIPTION
   State of the IPv6 address.
---------------------------------------------------------------------------*/
typedef enum
{
   // No address has been created. An invalid entry.
   IPV6_ADDR_STATE_INVALID    = 0,
   // This address is pending DAD verification
   IPV6_ADDR_STATE_TENTATIVE  = 1,
   // This address has been verified (DAD/local) and is ready to be allocated
   IPV6_ADDR_STATE_UNASSIGNED = 2,
   // This address has been assigned.
   IPV6_ADDR_STATE_VALID      = 3,
   // This address has been deprecated
   IPV6_ADDR_STATE_DEPRECATED = 4
} ps_iface_ipv6_addr_state_enum_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1500))

/*---------------------------------------------------------------------------
More backward compatibility definitions for the IPv6 Prefix Update related info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION<1701))

/*---------------------------------------------------------------------------
  Data type used to return all the v6 prefixes of an interface.
  (DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES)
---------------------------------------------------------------------------*/

// dss_iface_ioctl_type:
enum 
{
   DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES = 0xbad19, /* garbage value */ 
};

#ifdef FEATURE_DATA_PS_IPV6
typedef ps_iface_ipv6_addr_state_enum_type 
  dss_iface_ioctl_ipv6_addr_state_enum_type;

typedef struct
{
  struct in6_addr                            prefix;
  dss_iface_ioctl_ipv6_addr_state_enum_type  prefix_state;
  uint8                                      prefix_len;
} dss_iface_ioctl_prefix_info_type;

typedef struct 
{
  /* OUTPUT param */
  dss_iface_ioctl_prefix_info_type *prefix_info_ptr; /* Ptr to client prefix array */

  /* This is an input/output param that specifies the # of addresses
   for which space was allocated by the application and the actual # of 
   addresses  returned.                                                    */ 
   uint8         num_prefixes;

} dss_iface_ioctl_get_all_v6_prefixes_type;
#endif //FEATURE_DATA_PS_IPV6

#endif // ((!defined DSS_VERSION) || (DSS_VERSION<1701))

#if (!defined DSS_VERSION) || (DSS_VERSION < 50)
enum
{
   DSS_BCMCS_JOIN  = 15,
   DSS_BCMCS_LEAVE = 16
};
#endif // (!defined DSS_VERSION) || (DSS_VERSION < 50)

// Backward compatibility definitions for new multicast semantics
#if ((!defined DSS_VERSION) || (DSS_VERSION < 1703 || DSS_VERSION == 1800))
// dss_iface_ioctl_type
enum {
   DSS_IFACE_IOCTL_MCAST_JOIN_EX   = 0xbad1, /* garbage values */
   DSS_IFACE_IOCTL_MCAST_LEAVE_EX,
   DSS_IFACE_IOCTL_MCAST_REGISTER_EX
};

/*---------------------------------------------------------------------------
Maximum number of flows requested in a MCAST_JOIN_EX ioctl call
----------------------------------------------------------------------------*/
#define DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL (25)

/*---------------------------------------------------------------------------
Multicast request flags for register/configure request per flow
----------------------------------------------------------------------------*/
typedef enum
{
   DSS_IFACE_IOCTL_MCAST_REG_SETUP_ALLOWED   = 0xbad10,
   DSS_IFACE_IOCTL_MCAST_REG_SETUP_NOT_ALLOWED
} dss_iface_ioctl_mcast_join_ex_req_flags_enum_type;

/*---------------------------------------------------------------------------
Join a number of Multicast group addresses and ports
----------------------------------------------------------------------------*/
typedef struct 
{
   ip_addr_type               ip_addr[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast IP address     */
   uint16                     port[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast Port           */
   void                       *mcast_param_ptr[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast input params   */
   uint32                     num_flows;         /* Num flow join requests   */
   void                       *user_data_ptr;    /* App specific data,
                                                 passed in cback          */
   int16                      dss_nethandle;            /* dss_nethandle            */
#define dss_nethandle        app_id
   dss_iface_ioctl_mcast_join_ex_req_flags_enum_type
      mcast_request_flags[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Configure/Register request */
   dss_iface_ioctl_event_cb   event_cb;          /* Callback for mcast ev    */
   uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast handle         */
}dss_iface_ioctl_mcast_join_ex_type;

/*---------------------------------------------------------------------------
Leave a number of Multicast groups using handles assigned by the mode handler
----------------------------------------------------------------------------*/
typedef struct
{
   uint8                      num_flows;         /* Num flow leave requests  */
   uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast handle         */
   int16                      dss_nethandle;            /* dss_nethandle            */
} dss_iface_ioctl_mcast_leave_ex_type;

/*---------------------------------------------------------------------------
Trigger registration of multiple multicast flows using assigned handles 
---------------------------------------------------------------------------*/
typedef struct
{
   uint8                      num_flows;    /* Num flow register requests  */
   uint32                     handle[DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL];
   /* Multicast handle            */
   int16                      dss_nethandle;            /* dss_nethandle            */
} dss_iface_ioctl_mcast_register_ex_type;

// New event
enum {
   DSS_IFACE_IOCTL_MCAST_STATUS_EV
};

// New info codes
enum {
   DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE = 0x7fffffff,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MONITORED = 0xbad100,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_CANCELLED,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNABLE_TO_MONITOR, 
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_TIMEOUT,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_LOST, 
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_SYS_UNAVAILABLE,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNAVAILABLE,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_NO_MAPPING,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_REQUESTED,
   DSS_IFACE_IOCTL_MCAST_BCMCS2p0_MAX_FLOWS_REACHED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS, 
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_REQUEST_SENT, 
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS,
   DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_SUCCESS, 
   DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_FAILED,
   DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_DEREGISTERED
};
#endif // ((!defined DSS_VERSION) || (DSS_VERSION < 1703 || DSS_VERSION == 1800))
/* The version check for BCMCS Database flow id length feature 
   (if this expression evaluates to TRUE, this feature is supported) */
#define DSS_VER_FLOW_ID_LEN (defined (DSS_VERSION) && \
                            (DSS_VERSION >= 1703) && (DSS_VERSION != 1800) )

/*---------------------------------------------------------------------------
Backward compatibility definitions for the HAT info
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION < 1704) || ((DSS_VERSION >= 1800) && (DSS_VERSION < 1900)))

enum 
{
   DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER = 0xbad1, /* garbage values */
   DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER
};

typedef int dss_iface_ioctl_707_hat_type;

#endif // ((!defined DSS_VERSION) || (DSS_VERSION < 1704) || ((DSS_VERSION >= 1800) && (DSS_VERSION < 1900)))

/*---------------------------------------------------------------------------
Backward compatibility definitions for the 3GPP_ANY iface
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || (DSS_VERSION < 1705))
enum 
{
   DSS_IFACE_3GPP_ANY     = 0xbad5,
   DSS_IFACE_3GPP2_ANY    = 0xbad6
};
#endif // ((!defined DSS_VERSION) || (DSS_VERSION < 1705))

/*---------------------------------------------------------------------------
Backward compatibility definitions for the IWLAN iface
---------------------------------------------------------------------------*/
#if ((!defined DSS_VERSION) || ((DSS_VERSION < 1705) || (((DSS_VERSION >= 1800) && (DSS_VERSION < 1899)) || ((DSS_VERSION == 1900)))))
enum 
{
   DSS_IFACE_IWLAN_3GPP     = 0xbad7,
   DSS_IFACE_IWLAN_3GPP2    = 0xbad8
};
#endif // ((!defined DSS_VERSION) || ((DSS_VERSION < 1705) || (((DSS_VERSION >= 1800) && (DSS_VERSION < 1899)) || ((DSS_VERSION == 1900)))))

#if defined(__cplusplus)
}
#endif
#endif /* OEMDSS_H */

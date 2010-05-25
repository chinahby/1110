#ifndef DS_SOCKI_H
#define DS_SOCKI_H

/*===========================================================================

          S O C K E T S  I N T E R N A L  H E A D E R  F I L E
                   
DESCRIPTION

 The Data Services Internal Socket Header File. Contains shared variables and 
 enums, as well as declarations for functions for each protocol.


Copyright (c) 1998-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocki.h_v   1.15   13 Feb 2003 12:20:14   ssinghai  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssocki.h#3 $ $DateTime: 2008/07/03 12:28:16 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/07/08    pp     OMH 1b Profile Arbitration development.
11/05/03    sv     Added tcp sndcnt field to SCB.
07/08/03    ss     Made changes to support SDB ack callback socket option.
07/06/03    sv     Added sndbuf_size filed to the socket control block. Changed
                   the declaration of is_writeable to return int. Changed
                   the declaration of dssocki_copy_buf_to_dsm to accept
                   maximum bytes to copy.
05/27/03    om     Added ipsec_hdr_size to SCB, changed recvmsg and sendmsg
                   to optionally take payload in DSM item chains.
05/04/03    ss     Added declarations to support so_linger socket option
04/25/03    aku    Added app_specified_iface_ptr to acb_type.
04/23/03    ss     Added options_mask and other fields for option values to 
                   the scb, added macros to set, reset and check sockopts
04/14/03    sv     Added dssocki_sendmsg, dssocki_recvmsg and dssocki_msghdr
                   declarations. Changed protocol specific read functions to
                   accept iov and iovcount instead of buffer and bufferlen.
04/10/03    ss     Added dssocki_is_addr_in_use() and PORTOFFSET
02/13/03    ss     Added state_flags to scb to support socket shutdown  
02/12/03    om     Added address cache to SCB for UDP routing.
02/10/03    aku    Removed unused dssocki_non_sys_sockets_flow_controlled().
02/07/03    usb    Added support for adding/deleting IP filters to iface
12/24/02    aku    Added support for reg/dereg event callbacks using IOCTL.
12/22/02    aku    Removed iface_up_cback and ifacw_down_cback from acb_type.
                   Removed dssocki_iface_cb_type. Added 
                   dssocki_process_iface_up_ev_cback() and 
                   dssocki_process_iface_down_ev_cback().
11/28/02    mvl    Added the SO_RECVBUF mask for the socket options.
11/26/02    usb    Increased number of iface_cb buffers to accomodate 
                   multiple events that each app can get.
11/14/02    jd     Added dssocki_mip_iface_in_use to see if MIP iface is
                   available
11/05/02    mvl    Added support for changing default window size.
10/30/02    ss     Added BIND routine in the caller function enum
10/30/02    rsl    Added extern to dssocki_iface_ev_cback().
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
09/24/02    om     Added dssocki_generate_meta_info() and its flags.
09/12/02    aku    Reverting change to dssocki_common_write_check() function
                   signature as using pointer to struct degrades performance
09/11/02    aku    Changed dssocki_common_write_check() to take pointer to
                   struct instead of multiple individual arguments.
09/06/02    aku    Fixed bug in registering for iface events. 
09/05/02    aku    Updated for ps_iface cback buf interface.
08/30/02    om     Added routing cache to SCB.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/24/02    aku    app_iface_state is now used only if FEATURE_DATA_MM is
                   not defined. 
07/14/02    aku    Added support for FEATURE_DATA_MM
07/10/02    na     Changed prototype of common write check to fit new MM 
                   architecture.
05/02/02    mvl    Modified the signature of the write function in the
                   protocol specific function table to accept a meta info.
                   Also added boolean to common_write_check() which indicates
                   whether to re-orig if dormant.
02/22/02    pjb    Added accept and listen
02/11/02    ss     Added is_writeable() in dss_fcn_ptr_table.
01/15/01    aku    Added prototype for 
                   dssocki_non_sys_sockets_flow_controlled()
11/11/01    aku    Changed boolean ppp_active in acb_type to iface_state type
                   Removed redundant dssocki_is_socket_app_active()
                   Removed dssocki_num_active_apps() (Not used)
                   Moved dssocki_is_ppp_in_use() to dssinternal.h
                   Added acb_ptr arg to dssocki_socket_event_occurred()
11/09/01    aku    Added prototype for dssocki_is_socket_app_active()
11/09/01    ss     Changed DSS_LONG_CLOSE_INTERVAL from 1 to 2 sec. This
                   interval should be sufficient for the peer to send a
                   FIN after sending an ack for FIN.
10/11/01    mt     Add dss_errno argument to connect_setup function pointer. 
08/27/01    aku    Removing net_event_active flag from acb_type struct. No 
                   longer used.
04/06/01    js     Added lower and upper range in the definition of socket
                   option type. 
04/06/01    js     Modified dssocki_socket_event_check() to support socket
                   option DSS_SO_ZERO_ADDR_OK.
04/06/01    js     Added prototype for function dssocki_common_write_check()
                   which contains the generic steps for a socket write 
                   operation. 
03/29/01    js     Added inclusion for dssinternal.h
03/27/01    mt     change the second argument type of the read function
                   pointer in struct dss_fcn_ptr_table.
03/27/01    mvl    changed all instances of errno to dss_errno - removed
                   definitions of base and max sockfd values (now in
                   socket.h).
03/27/01    js     Added extra parameter to the definition of write() in
                   dss_fcn_ptr_table. This extra parameter points to the a
                   structure holding the destination information of the
                   write operation. (The destination info in scb_ptr maybe
                   stale)
03/27/01    js     Added icb (ICMP Control Block) to the definition of
                   protocol_ctl_block_type.
02/14/01    js     Added definitions to support socket options. 
02/08/01    yll    Added read_dsm_chain in the dss_fcn_ptr_table.
11/27/00    na     Cleanup. Packed acb_type and struct scb_s better to save
                   RAM.
10/30/00    snn    Removed define for SOCKFDBASE( 0x1000)  and APPIDBASE 
                   (0x2000) because global variables will be used in their
                   place.
                   Renamed MAX_SOCKS to DSS_MAX_SOCKS. and MAX_APPS 
                   to DSS_MAX_APPS.
                   Renamed scb_type as scb_s, to follow naming convention.
                   Rearranged elements in some structures to pack the data
                   properly.
10/10/00    snn    Moved MAX_SOCKS and MAX_APPS from here to dssocket.h for
                   cleaner interface, because they are updated by customers.
09/03/00    snn    Made changes to support multiple applications.
08/03/00    mvl/js Added a function pointer to the socket function pointer
                   table definition to support protocol specific bind(). 
07/27/00    mvl    Backed out the MAX_APPS change as there was no support for
                   multiple applications using the library.
05/11/00    mvl    Modified MAX_SOCKS to support multiple sockets and
                   MAX_APPS to support as many applications as there are
                   sockets.
08/11/99    ak/rc  Added abort to fcn ptr table, to take care of closing
                   socket when no network.
04/21/99    hcg    Updated parameter to dssocki_send_to_ps(), to conform to 
                   new kind field sizing in dsm_item_type.
04/05/99    hcg    Incorporated code review changes and comments.  Removed 
                   external declaration of dssocki_convert_ntoh() as its 
                   functionality is now provided through macros.
10/06/98    na/hcg Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_DS_SOCKETS
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "psglobal.h"
#include "netuser.h"
#include "dssocket.h"
#include "dssinternal.h"
#include "ps_iface.h"
#include "ps_tcp.h"
#include "ps_ip.h"
#include "ps_svc.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Socket allocation constants.
---------------------------------------------------------------------------*/
#define SOCKAVAIL            0                   /* socket is free for use */
#define SOCKFDMAX       0x1FFF                    /* socket descriptor max */
#define APPAVAIL             0              /* application is free for use */
#define APPIDMAX        0x2FFF                    /* socket descriptor max */
#define PORTBASE        0x8000                      /* ephemeral port base */
#define PORTOFFSET      0x0FFF                    /* ephemeral port offset */

/*---------------------------------------------------------------------------
  Socket Cleanup Timer Intervals
---------------------------------------------------------------------------*/
#define DSS_SHORT_CLOSE_INTERVAL   30       /* TCP closed cleanup interval */
#define DSS_LONG_CLOSE_INTERVAL  2000    /* TCP time wait cleanup interval */

/*---------------------------------------------------------------------------
  Socket Half Open Interval
---------------------------------------------------------------------------*/
#define DSS_HALF_OPEN_INTERVAL 15000          /* Max time to ack a syn/ack */

/*---------------------------------------------------------------------------
  Data types for option values. When an entry for a specified option is 
  referenced from the options table, the dss_sockopt_data_type field of the 
  entry reveals the option's data type. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSSI_SO_BOOL            = 0,               /* boolean type               */
  DSSI_SO_INT             = 1,               /* integer type               */
  DSSI_SO_LINGER          = 2,               /* linger type                */
  DSSI_SO_SDB_ACK_CB_TYPE = 3                /* sdb ack callback type      */
} dssocki_sockopt_data_type;

/*---------------------------------------------------------------------------
  Encapsulation of socket option. Fields such as option name, level and
  option value type are not to be changed once initilzed. 
---------------------------------------------------------------------------*/
typedef struct
{
  int optname;                            /* option name                   */
  dssocki_sockopt_data_type optvaltype;   /* option type                   */
  dss_sockopt_levels_type level;          /* option level                  */
  uint32 optlen;                          /* legth of the option value     */
  int initval;                            /* initial option value          */
  int minval;                             /* optval lower bound            */
  int maxval;                             /* optval upper bound            */
} dssocki_sockopt_type;

/*---------------------------------------------------------------------------
  Enum for socket state.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_NULL = 0,       /* empty state                                   */
  DSSOCKI_INIT = 1,       /* TCP socket has been created                   */
  DSSOCKI_LISTEN = 2,     /* TCP server socket listening                   */
  DSSOCKI_OPENING = 3,    /* TCP is attempting a connection                */
  DSSOCKI_OPEN = 4,       /* TCP conn established, DGRAMS open for writing */
  DSSOCKI_CLOSING = 5,    /* TCP issued a close                            */
  DSSOCKI_WAIT_FOR_CLOSE = 6, /* TCP close completed                       */
  DSSOCKI_CLOSED = 7      /* connection or network failure                 */
} dssocki_state_enum_type;

/*---------------------------------------------------------------------------
  Enum for state flags. Note that the flags are ORed together. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_CANTSENDMORE = 0x1,  /* output operations are disallowed on sock */
  DSSOCKI_CANTRECVMORE = 0x2,   /* input operations are disallowed on sock */
  DSSOCKI_LINGER       = 0x4                    /* the socket is lingering */
} dssocki_state_flags_enum_type;

/*---------------------------------------------------------------------------
  Enum for type of caller function
---------------------------------------------------------------------------*/
typedef enum 
{
  DSSOCKI_CONNECT_ROUTINE  =  0,
  DSSOCKI_BIND_ROUTINE     =  1,
  DSSOCKI_LISTEN_ROUTINE   =  2,
  DSSOCKI_ACCEPT_ROUTINE   =  3,
  DSSOCKI_CLOSE_ROUTINE    =  4,
  DSSOCKI_INPUT_ROUTINE    =  5, 
  DSSOCKI_OUTPUT_ROUTINE   =  6, 
  DSSOCKI_SHUTDOWN_ROUTINE =  7 
} dssocki_caller_enum_type;

/*---------------------------------------------------------------------------
  Enum for Application control flags
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_NET_UP_EVENT_WAIT = 0x1,
} dssocki_app_ctrl_flags_enum_type;

/*---------------------------------------------------------------------------
  Application control block structure.
---------------------------------------------------------------------------*/
typedef struct
{
  void (*net_callback_fcn)(void *);          /* network event callback fcn */
  void (*socket_callback_fcn)(void *);        /* socket event callback fcn */
  sint15 app_id;                     /* ACB's corresponding application ID */
  boolean notified_cb_flag;             /* Socket callback has been called */
  ps_iface_type *ps_iface_ptr;           /* Ptr to iface for this socket   */
  q_type event_q_array[DSS_IFACE_IOCTL_EVENT_MAX];/* event callbacks array */
  ps_iface_type *app_specified_iface_ptr; /* App specified interface       */
  acl_policy_info_type acl_netpolicy_info; /* network policy */
  uint16               flags;              /* application control flags   */
} acb_type;

/*---------------------------------------------------------------------------
  This is the type that is placed on the event callback queues.  It is only
  used internally and so should not be referenced outside of this file.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type               link;
  ps_iface_type             *iface_ptr;
  dss_iface_ioctl_event_cb  f_ptr;
  void                      *user_info_ptr;
  dsm_item_type             *dsm_ptr;
} dssocki_event_buf_type;


/*---------------------------------------------------------------------------
  protocol control block union type - contains pointers to protocol 
  specific control blocks
---------------------------------------------------------------------------*/
typedef union 
{
  struct icmp_cb *icb;                                             /* ICMP */
  struct tcb *tcb;                                                  /* TCP */
  struct udp_cb *ucb;                                               /* UDP */
  struct raw_ip *rcb;                                            /* Raw IP */
  void *p;                                                       /* dunno? */
} protocol_ctl_block_type;

/*---------------------------------------------------------------------------
  socket control block structure.  
---------------------------------------------------------------------------*/
typedef struct scb_s
{
  acb_type *acb_ptr;          /* Ptr to application ctl block for this scb */
  struct dss_fcn_ptr_table *fcn_ptr_table;  /* protocol function ptr table */
  protocol_ctl_block_type protocol_ctl_blk;      /* protocol control block */
  sint31 requested_event_mask;       /* Events the socket is interested in */
  sint31 event_happened_mask;                 /* Events that have occured  */
  struct socket localname;                                    /* host name */
  struct socket servname;                                   /* server name */  
  struct scb_s *so_head;                  /* back pointer to accept socket */
  struct scb_s *so_partial;    /* queue of unconnected/partial connections */
  struct scb_s *so_estab;               /* queue of acceptable connections */
  sint15 so_qlimit;                                 /* max length of queue */
  sint15 closed_state_reason;      /* reason socket went into CLOSED state */
  sint15 sockfd;                  /* SCB's corresponding socket descriptor */  
  dssocki_state_enum_type socket_state;             /* state of the socket */
  uint16 state_flags;            /* flags associated with the socket_state */
  byte family;                                           /* address family */
  byte type;                                                /* socket type */
  byte protocol;                                          /* protocol type */
  boolean data_available;                 /* Something to read -or- accept */
  ps_iface_type *routing_cache;        /* Routing cache for transmit iface */
  ip_addr_type  address_cache;         /* Address cache for UDP routing    */
  ps_iface_type *fi_map[MAX_SYSTEM_IFACES]; /* sock-iface ip fltr maping   */
  uint32 options_mask;                      /* bit-mask for socket options */
  uint32 rcvbuf;      /* maximum number of bytes allowed to be in the rcvq */
  uint32 sndbuf;      /* maximum number of bytes allowed to be in the sndq */
  uint16 ipsec_hdr_size;        /* Max. header size increment due to IPSec */
  uint8  ip_ttl;               /* the TTL value of the outgoing IP packets */
  int    so_linger;            /* time to linger for, in ms, while closing */
  int    linger_timer;                    /* timer id for the linger timer */
  acb_type *linger_acb_ptr;              /* ptr to the acb while lingering */
  dss_so_sdb_ack_cb_fcn sdb_ack_cb_fcn;             /* sdb ack cb function */
  void* sdb_ack_user_data; /* the user data to be passed to sdb ack cb fcn */
  uint32 sndcnt;                                         /* tcp send count */
} scb_type;

/*---------------------------------------------------------------------------
  Message header structure for the common I/o functions.
---------------------------------------------------------------------------*/
struct dssocki_msghdr
{
  void               * msg_name;            /* protocol address            */
  uint16               msg_namelen;         /* size of protocol address    */
  struct iovec       * msg_iov;             /* scatter/gather array        */
  uint16               msg_iovlen;          /* # elements in msg_iov       */
  void               * msg_control;         /* ancillary data              */
  uint16               msg_controllen;      /* length of ancillary data    */
  int                  msg_flags;           /* flags returned by recvmsg   */
};

/*---------------------------------------------------------------------------
  protocol specific function pointer table - contains function pointers
  to "common" socket functions, that are implemented differently, depending
  on the protocol being used.
---------------------------------------------------------------------------*/
struct dss_fcn_ptr_table 
{
  int type;                                                 /* Socket type */
  sint15 (*socket)(struct scb_s *scb_ptr, sint15 *dss_errno);/* alloc sckt */
  sint15 (*connect_setup)(struct scb_s *scb_ptr, sint15 *dss_errno);
                                                      /* set-up connection */
  sint15 (*connect)(struct scb_s *scb_ptr); /* actually perform connection */
  sint15 (*listen_setup) (struct scb_s *scb_ptr,         
              sint15 backlog,
              sint15 * dss_errno);           /* set-up sockets */
  sint15 (*listen) (struct scb_s *scb_ptr);    /* perform the passive open */
  sint15 (*accept) (struct scb_s *scb_ptr,      
            struct sockaddr * remote_addr,
            uint16 * addr_len,
            sint15 * dss_errno);        /* accept a new connection */
  sint15 (*write)(protocol_ctl_block_type pcb,
                  struct sockaddr_in *ta_ptr,
                  dsm_item_type *dsm_item,
                  ps_meta_info_type *meta_info_ptr);/* byte output routine */
  sint15 (*read)( struct scb_s* scb_ptr, struct iovec * iov,   
                  uint16 iovcount, struct sockaddr_in *fromaddr, 
                  sint15* dss_errno);                /* byte input routine */
  sint15 (*read_dsm_chain)( struct scb_s* scb_ptr,   
                            dsm_item_type **item_ptr,   
                            struct sockaddr_in *fromaddr, 
                            sint15* dss_errno);  /* dsm item input routine */
  sint15 (*shutdown)( struct scb_s* scb_ptr,   
                      dsm_item_type *item_ptr,   
                      sint15* dss_errno);       /* socket shutdown routine */
  sint15 (*close)(struct scb_s *scb_ptr, sint15 *dss_errno); /* cls/cln con*/
  sint15 (*abort)(struct scb_s *scb_ptr, sint15 *dss_errno);/* abrt/cln con*/
  void (*bind)(struct scb_s *scb_ptr, struct sockaddr_in *la); /* bind sock*/ 
  int (*is_writeable)(protocol_ctl_block_type pcb);
};

/*---------------------------------------------------------------------------
  protocol specific function pointer table - array of function tables,
  on a per protocol basis.
---------------------------------------------------------------------------*/
extern struct dss_fcn_ptr_table dssock_fcn_ptr_table[];

extern struct scb_s scb_array[DSS_MAX_SOCKS]; /* socket ctl block array    */

extern acb_type dssocki_acb_array[DSS_MAX_APPS];

extern const dssocki_sockopt_type dss_sock_opts_default[];
/*---------------------------------------------------------------------------
  Application Identifier Base.                            
   NOTE THAT THE  APPIDBASE_MIN_VAL SHOULD NOT EQUAL ZERO.  
   Note, these should be within range of appidbase type.
---------------------------------------------------------------------------*/
#define APPIDBASE_MIN_VAL  100
#define APPIDBASE_MAX_VAL  1000 


/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO      DSSOCKI_SOCKET_CANTRECVMORE()

DESCRIPTION  This macro updates the state_flags of the given scb to 
             indicate that the socket has been shut down for the read-half

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modifies state_flags in the scb
===========================================================================*/
#define DSSOCKI_SOCKET_CANTRECVMORE(scb_ptr)        \
{                                                   \
  INTLOCK();                                        \
  scb_ptr->state_flags |= DSSOCKI_CANTRECVMORE;     \
  INTFREE();                                        \
}

/*===========================================================================
MACRO      DSSOCKI_SOCKET_CANTSENDMORE()

DESCRIPTION  This macro updates the state_flags of the given scb to 
             indicate that the socket has been shut down for the write-half

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modifies state_flags in the scb
===========================================================================*/
#define DSSOCKI_SOCKET_CANTSENDMORE(scb_ptr)        \
{                                                   \
  INTLOCK();                                        \
  scb_ptr->state_flags |= DSSOCKI_CANTSENDMORE;     \
  INTFREE();                                        \
}

/*===========================================================================
MACRO         IS_SOCKOPT_SET()

DESCRIPTION   Check if the specified socket option is set,

DEPENDENCIES  None

RETURN VALUE  True, if the option is set
              False, otherwise

SIDE EFFECTS  None
===========================================================================*/
#define IS_SOCKOPT_SET(options_mask, optname) \
   ((options_mask & (1 << optname))? TRUE : FALSE)


/*===========================================================================
MACRO         SOCKOPT_SET()

DESCRIPTION   Set the specified socket option,

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define SOCKOPT_SET(options_mask, optname) \
   (options_mask |= (1 << optname))


/*===========================================================================
MACRO         SOCKOPT_RESET()

DESCRIPTION   Reset the specified socket option

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define SOCKOPT_RESET(options_mask, optname) \
   (options_mask &= ~(1 << optname))

#ifdef FEATURE_DATA_PS_ARBITRATION_MGR
/*===========================================================================
MACRO         DSSOCKI_IS_APP_CTRL_FLAG_SET

DESCRIPTION   Check if the specified control flag is set,

DEPENDENCIES  None

RETURN VALUE  True, if the option is set
              False, otherwise

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_IS_APP_CTRL_FLAG_SET(acb_ptr, optname) \
   (((acb_ptr)->flags & (optname))? TRUE : FALSE)

/*===========================================================================
MACRO         DSSOCKI_SET_APP_CTRL_FLAG

DESCRIPTION   Set the specified control flag,

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_SET_APP_CTRL_FLAG(acb_ptr, optname) \
   ((acb_ptr)->flags |= (optname))

/*===========================================================================
MACRO          DSSOCKI_RESET_APP_CTRL_FLAG

DESCRIPTION   Reset the specified control flag.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_RESET_APP_CTRL_FLAG(acb_ptr, optname) \
   ((acb_ptr)->flags &= ~( optname))
#endif /* FEATURE_DATA_PS_ARBITRATION_MGR */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSSOCKI_ALLOCSCB

DESCRIPTION
  Allocates socket control block from SCB array.  Calculates socket
  descriptor and sets it in the SCB.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern struct scb_s *dssocki_allocscb
(
  void
);

/*===========================================================================
FUNCTION DSSOCKI_INIT_SCB()

DESCRIPTION
  Initializes the given scb
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If the initialization was successful
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean
dssocki_init_scb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_FREESCB

DESCRIPTION
  Frees a socket control block in the SCB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_freescb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCKFDTOSCB

DESCRIPTION
  Given a socket descriptor, returns a socket control block from SCB array.  

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.

===========================================================================*/
extern struct scb_s *dssocki_sockfdtoscb
(
  int sockfd                                          /* socket descriptor */
);

/*===========================================================================

FUNCTION DSSOCKI_ALLOCACB

DESCRIPTION
  Allocates application control block from ACB array.  Assigns application
  ID and sets the network and socket callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern acb_type *dssocki_allocacb
(
  void
);

/*===========================================================================

FUNCTION DSSOCKI_FREEACB

DESCRIPTION
  Frees an application control block in the ACB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_freeacb
(
  acb_type *acb_ptr          /* ptr to application control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_APPIDTOACB

DESCRIPTION
  Given an application ID, returns an application control block from ACB 
  array.  

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.

SIDE EFFECTS
  None.

===========================================================================*/
extern acb_type *dssocki_appidtoacb
(
  sint15 app_id                                          /* application ID */
);

/*===========================================================================

FUNCTION DSSOCKI_GETACBCOUNT

DESCRIPTION
    Gets the current number of allocated application control blocks in the 
    system.

DEPENDENCIES
  None.

RETURN VALUE
  Returns number of allocated applications in ACB array.

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssocki_getacbcount
(
  void
);

/*===========================================================================
FUNCTION DSSOCKI_IS_ADDR_IN_USE()

DESCRIPTION
  Determines if the specified address, in network byte order, is in use for 
  the given protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - The address is in use
  FALSE - The address is not in use

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dssocki_is_addr_in_use 
( 
  byte proto, 
  struct sockaddr_in* laddr 
);

/*===========================================================================

FUNCTION DSSOCKI_GETEMPHEM_PORT

DESCRIPTION
  Returns an ephemeral port, for the specified protocol, between PORTBASE and 
  PORTBASE+PORTOFFSET after checking that it is not in use by another socket.

DEPENDENCIES
  None.

RETURN VALUE
  Port number: in network byte order, not in use by another socket
  0: In case there is no ephemeral port available

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 dssocki_getephem_port
(
  byte proto       /* protocol for which ephemeral port is to be generated */
);

/*===========================================================================

FUNCTION DSSOCKI_FCNPTR_SETUP

DESCRIPTION
  Sets up the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Function pointer table is assinged.

===========================================================================*/
extern sint15 dssocki_fcnptr_setup
(
  struct scb_s *sock_ptr,                /* socket control block structure */ 
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSSOCKI_SEND_TO_PS

DESCRIPTION
  Sends the application's dsm_item data to the PS task, which will send it 
  out using the protocol for the given socket.
  
  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.               
===========================================================================*/
extern void dssocki_send_to_ps
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type* item_ptr,                    /* Ptr to payload to be sent */
  struct sockaddr_in* ta_ptr,    /* Ptr to structure holding the dest info */
  byte kind                                 /* Type of item to be enqueued */
);

/*===========================================================================

FUNCTION DSSOCKI_COPY_BUF_TO_DSM

DESCRIPTION
  Copies application data buffer vector to data services memory pool.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to dsm_item_type

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssocki_copy_buf_to_dsm
(
  struct iovec *iov,      /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 max_bytes,                     /* maximum number of bytes to copy */
  dsm_item_type **buf_ptr_ptr          /* pointer to dsm item to copy into */
);

/*===========================================================================
FUNCTION DSSOCKI_GENERATE_META_INFO()

DESCRIPTION
  Generates PS meta info packet for TX flags and socket opetions
  set and attaches it to the packet.

DEPENDENCIES
  Packet head pointer has to include all headers above IP.

RETURN VALUE
  DSS_SUCCESS or DSS_ERROR (dss_errno contains cause).

SIDE EFFECTS
  None.
===========================================================================*/
int dssocki_generate_meta_info
(
  dsm_item_type*  pkt_head_ptr,
  scb_type*       scb_ptr,
  uint32          tx_flags,
  sint15*         dss_errno
);


/*===========================================================================

FUNCTION DSSOCKI_SET_SOCK_STATE

DESCRIPTION
  This function sets the socket state. It can be NULL, INIT, OPENING, OPEN, 
  CLOSING, WAIT_FOR_CLOSE, or CLOSED.
  
  NULL: Is a pseudo-state, that is set prior to initial socket
           creation.  A NULL socket cannot be accessed by any API
           function.

  INIT: Is specific to TCP, and is set when a TCP socket is first
           created, through a call to socket().  Note this is an
           invalid state for UDP sockets.

  LISTEN: This socket it listening.  This is specific to TCP, as such
           is an invalid state for UDP.  accept() may be called on
           this socket.
  
  OPENING: the socket is not writable and read() and write() would
           return DS_EWOULDBLOCK.
           A connected socket is in the OPENING state after dss_connect() 
           has been called and before the socket is completly connected.
           A connectionless socket is never in this state.
  
  OPEN: This means the socket is writable if there is enough memory
           available.  A connected socket is in the OPEN state when
           TCP is in the ESTABLISHED state.  A connectionless socket
           is in OPEN state in the dss_socket() call.
  
  CLOSED:  means the socket is writable and readable  and read() and write() 
           would return DS_ECONNABORTED, DS_ECONNRESET, DS_IPADDRCHANGED or
           DS_ENETDOWN. A connected socket is in the CLOSED state after it 
           lost its connection to the server or the network connectivity was
           lost any time after the dss_connect() was called. 
           A connectionless socket is in this state, only when the network
           subsystem has experienced a failure.
  
  CLOSING: means that a TCP socket has issued a close(), but the TCP close
           is still in progress.  The initial call to close() for TCP 
           sockets would return DS_EWOULDBLOCK.  This corresponds to the 
           active close, and the socket is waiting for TCP to fully 
           negotiate the close.
           This state is invalid for connectionless sockets.

  WAIT_FOR_CLOSE: means that TCP has fully closed, or the TCP close
           timer has expired.  In this state, the application may call
           close() again to clean up the socket and free it for
           re-use.  This state is invalid for connectionless sockets.

  This should be called whenever the state changes and is the only function
  which modifies the socket_state member of the scb_s.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_set_sock_state
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dssocki_state_enum_type new_state                        /* Socket state */
);

/*===========================================================================

FUNCTION DSSOCKI_NET_EVENT_NOTIFY

DESCRIPTION
  This function notifies the application that the network event of interest
  has occurred. If the passed parameter is NULL then it notifies all the 
  opened   applications. If passed parameter is a valid one, it notifies
  only that application.
  
DEPENDENCIES 
    It may be a good practice to check that the curr_acb_ptr being passed 
    is a valid one by the function calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_net_event_notify
(
    acb_type* curr_acb_ptr /* ACB to be notiifed */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_CHECK()

DESCRIPTION
  This function performs a real-time check, to determine if any of the socket
  events of interest have occurred.  If so, it returns the event mask of the'
  events that have occurred.
  
DEPENDENCIES
  None.

RETURN VALUE
  sint31 event mask of the socket events that have occurred.

SIDE EFFECTS
  None.

===========================================================================*/
sint31 dssocki_socket_event_check
( 
  struct scb_s* scb_ptr,          /* ptr to socket control block structure */
  sint31 requested_event_mask        /* Events the socket is interested in */
);

/*===========================================================================

FUNCTION DSSOCKI_NOTIFY_APP

DESCRIPTION
  This function calls the application callback function to alert the
  application that a socket event has occurred.  Note that the application
  callback will not be called if the application has already been notified.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The application callback function is invoked.

===========================================================================*/
extern void dssocki_notify_app
( 
  acb_type *acb_ptr                    /* ptr to application control block */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_OCCURRED

DESCRIPTION
  This function notifies all interested sockets that a socket event 
  has occurred.  If a scb_ptr is specified, this function notifies only that 
  socket. If scb_ptr is NULL and acb_ptr is specified, this function notifies
  all sockets for that app. If both scb_ptr and acb_ptr are specified, the 
  function assumes it should provide notification for all sockets.  The 
  function then loops through the socket control block array and checks each socket for events
  that may have occurred.  The application is notified upon any events
  having occurred.  If a valid scb_ptr is specified, the function checks
  that particular socket for any events that may have occurred.  Again, 
  the application is notified if any of the events that it is interested
  in, have occurred. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_socket_event_occurred
( 
  struct scb_s    *scb_ptr,                 /* ptr to socket control block */
  acb_type *acb_ptr                         /* ptr to app control block    */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCK_SET_STATE_CLOSED

DESCRIPTION
  This function sets the socket state to DSSOCKI_CLOSED, however, only if 
  the current socket state is set to DSSOCKI_OPEN or DSSOCKI_OPENING.  If a 
  NULL parameter is passed in, it will change all DSSOCKI_OPEN or 
  DSSOCKI_OPENING sockets to DSSOCKI_CLOSED.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets socket state to DSSOCKI_CLOSED.
===========================================================================*/
extern void  dssocki_sock_set_state_closed
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_SET_CLOSED_STATE_REASON

DESCRIPTION
  Sets the closed_state_reason field in the SCB with the appropriate
  reason for the socket transitioning into the CLOSED state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the closed_state_reason field.
===========================================================================*/
extern void  dssocki_set_closed_state_reason
(
  struct scb_s *scb_ptr,          /* ptr to socket control block structure */
  sint15   reason                /* reason for socket tranistion to CLOSED */
);

/*===========================================================================

FUNCTION DSSOCKI_GET_ERR_VAL

DESCRIPTION
  Gets the appropriate error value based on current socket state and 
  type of operation (connect(), close(), read(), write()).  This function
  follows the TCP/UDP socket state machines.  

DEPENDENCIES
  None.

RETURN VALUE
  Error condition.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dssocki_get_err_val
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  dssocki_caller_enum_type type,   /* type of function that is calling this
                                      function.                            */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSSOCKI_INDEXTOSCB

DESCRIPTION
  Convert the index of scb_array to the address.
  
  This function will take the index of the scb_array as the input. If the 
  sockfd is valid for this index in the scb_array, i.e. Not SOCKAVAIL, then 
  we return the address of that element of array. 
  If the sockfd equals SOCKAVAIL, then we return a NULL.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.

===========================================================================*/
extern struct scb_s *dssocki_indextoscb (int i);


/*===========================================================================

FUNCTION DSSOCKI_IFACE_CHECK

DESCRIPTION
  This function contains procedure that's generic to dss_sendto(), 
  dss_write(), dss_write_dsm_chain() and dss_connect().
  
  This function figures out which interface to use and checks for validity
  conditions on that interface.

DEPENDENCIES
  None.

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise. 
  Error condition value is returned in the pointer to dss_errno, 

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor specified. 
  DS_ENETDOWN         network is closed. 
  DS_EWOULDBLOCK      not ready to write yet. 
  
SIDE EFFECTS
  None.

===========================================================================*/
sint15 dssocki_iface_check
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  boolean        re_orig,        /* should this cause dormant re-orig?     */
  boolean        check_flow_ctl,         /* TRUE=iface flow ctl is checked */ 
  sint15       *dss_errno                      /*   error condition value. */
);

/*===========================================================================

FUNCTION DSSOCKI_COMMON_WRITE_CHECK

DESCRIPTION
  This function contains procedure that's generic to dss_sendto(), 
  dss_write(), dss_write_dsm_chain().

DEPENDENCIES
  None.

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise. 
  Error condition value is returned in the pointer to dss_errno, 
  a pointer to socket control block is returned. 

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor specified. 
  DS_ENETDOWN         network is closed. 
  DS_EWOULDBLOCK      not ready to write yet. 
  
SIDE EFFECTS
  None.

===========================================================================*/
sint15 dssocki_common_write_check
(
  sint15       sockfd,                                /* socket descriptor */
  struct scb_s **scb_p_ptr,      /* pointer to scb, can be NULL initially. */
  boolean        re_orig,        /* should this cause dormant re-orig?     */
  boolean        check_flow_ctl,         /* TRUE=iface flow ctl is checked */ 
  sint15       *dss_errno                      /*   error condition value. */
);

/*===========================================================================
FUNCTION      DSSOCKI_IFACE_EV_CBACK()

DESCRIPTION   Called when the IFACE goes up/down or if the IP address changes. 
              This posts a command to PS and is processed in PS task context.

DEPENDENCIES

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void dssocki_iface_ev_cback
(
  ps_iface_type                *iface_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_UP_EV_CBACK()

DESCRIPTION   Called when the IFACE goes up. This calls the
              net_callback_fcn() for the specified application.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_iface_up_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_DOWN_EV_CBACK()

DESCRIPTION   Called when the IFACE goes down. This calls the
              net_callback_fcn() for the specified application.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_iface_down_ev_cback 
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PHYS_LINK_UP_EV_CBACK()

DESCRIPTION   Called when the PHYS_LINK goes up. This calls any app 
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_phys_link_up_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PHYS_LINK_DOWN_EV_CBACK()

DESCRIPTION   Called when the PHYS_LINK goes down. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_phys_link_down_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_IS_IFACE_IN_USE()

DESCRIPTION  This function determines if any other app is using the interface
             used by the specified app.

DEPENDENCIES  None

RETURN VALUE  boolean - TRUE if atleast one other app is using iface.
                        FALSE if no other app is using iface.

SIDE EFFECTS  None.
===========================================================================*/
boolean dssocki_is_iface_in_use
(
  acb_type                *acb_ptr
);

/*===========================================================================

FUNCTION DSSOCKI_REG_EV()

DESCRIPTION
  This function registers an application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If registration is successful.
  FALSE     If registration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_reg_ev
(
  sint15                    app_id,
  ps_iface_event_enum_type  event,
  ps_iface_type             *iface_ptr,
  dss_iface_ioctl_event_cb  f_ptr,
  void                      *user_info_ptr
);


/*===========================================================================

FUNCTION DSSOCKI_DEREG_EV()

DESCRIPTION
  This function deregisters the application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_dereg_ev
(
  sint15                    app_id,
  ps_iface_event_enum_type  event,
  ps_iface_type             *iface_ptr
);


/*===========================================================================
FUNCTION      DSSOCKI_INVOKE_APP_EV_CBACKS()

DESCRIPTION   This function invokes callbacks registered by the app (using
              the IOCTL interface) for the specified event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_invoke_app_ev_cbacks
(
  acb_type                        *acb_ptr,
  ps_iface_type                   *iface_ptr,
  dss_iface_ioctl_event_enum_type event
);


/*===========================================================================

FUNCTION DSSOCKI_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified iface. The filters are
  passed down to the iface to be added to iface control block while a mapping
  of the iface is added to SCB of the specified socket. This mapping is used 
  to deregister these filters when the socket is closed. 

DEPENDENCIES
  None.

RETURN VALUE
  Num filters added successfully, or
  -1 for error (sets dss_errno appropriately)

SIDE EFFECTS
  None.

===========================================================================*/
int dssocki_reg_ip_filter
(
  sint15                sockfd,
  ps_iface_type         *iface_ptr,
  ip_filter_enum_type   filter_type,
  uint8                 num_filters,
  void                  *filters,
  sint15                *dss_errno
);


/*===========================================================================

FUNCTION DSSOCKI_DEREG_IP_FILTER()

DESCRIPTION
  This function deregisters all the IP filters previosuly added to an iface
  associated with the specified socket. The filters are deleted from the 
  iface control block and the iface is removed from the filter map.
  
DEPENDENCIES
  None.

RETURN VALUE
  0 for success, -1 for error (sets dss_errno appropriately)
  
SIDE EFFECTS
  None.

===========================================================================*/
int dssocki_dereg_ip_filter
(
  sint15                sockfd,
  ps_iface_type         *iface_ptr,
  sint15                *dss_errno
);

/*===========================================================================
FUNCTION DSSOCKI_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output 
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport 
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less 
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required 
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once  
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/

sint15 dssocki_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dssocki_msghdr * msg,     /* Header containing data and dest addr */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSSOCKI_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input 
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers 
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dssocki_msghdr * msg,      /* Message header for filling in data  */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_LINGERING_CLOSE()

DESCRIPTION
  This function is called to when a lingering socket is closed. It brings
  down the interface if no other application is using it and frees up the 
  scb and the associated acb (a lingering socket's acb is allocated 
  internally when the socket enters the lingering state)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees up the acb and the scb, passed as a parameter
===========================================================================*/
extern void
dssocki_process_lingering_close
(
  scb_type** scb_ptr_ptr
);

#ifdef FEATURE_PS_UNSUPPORTED
#ifdef FEATURE_DS_MOBILE_IP

/*===========================================================================
FUNCTION      DSSOCKI_MIP_IFACE_IN_USE()

DESCRIPTION  This function determines if any other app is using the interface
             used by the specified app.

DEPENDENCIES  None

RETURN VALUE  boolean - TRUE if atleast one other app is using iface.
                        FALSE if no other app is using iface.

SIDE EFFECTS  None.
===========================================================================*/
extern boolean dssocki_mip_iface_in_use
(
  ps_iface_type *  go_iface
);
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_PS_UNSUPPORTED */

#endif /* FEATURE_DS_SOCKETS */

#endif /* DS_SOCKI_H */

/*===========================================================================

        D O M A I N  N A M E  S E R V I C E S  F U N C T I O N S
                   
DESCRIPTION

 The Domain Name Services mobile API File. Contains basic API functions for
 DNS Lookup.
 
EXTERNALIZED FUNCTIONS

  dss_getipnodebyname()
    POSIX IPv6 compliant function returns the resolved IP address in 
    dss_hostent structure given a host name.  Reference RFC 2553.

  dss_getipnodebyaddr()
    POSIX IPv6 compliant.  Returns resolved hostname in dss_hostent structure
    given a host IP address. Reference RFC 2553.

  dss_freehostent()
    Frees the dss_hostent structure returned by dss_getipnodebyname() and
    dss_getipnodebyaddr().  Reference RFC 2553.

  dss_gethostaddrs()
    Obtain host IP address by host name from the DNS server.  Legacy API
    function, deprecated.

  dss_inet_aton()
    Given a string with an IPv4 address in dot format returns the host
    address in network order.

  dss_inet_ntoa()
    Given an IPv4 address returns it in dot string format.

Copyright (c) 2000-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssdns.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/09    kk     Added NULL pointer checks in dnsi_cache_find()
                   and dnsi_cache_timeout_cb()
06/24/08   pp      Fixed RVCT compiler warnings.
04/03/08   pp      Klocwork fixes.
10/29/07   sn      Replaced banned string APIs.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
08/18/03   ifk     Fixed bug where we were adding expanded domain name length
                   to message offset while parsing a domain name message.
05/01/03   ifk     Rewrote parts of the query processing layer to save query
                   information as a queue of RRs instead of as a dss_hostent.
                   Added a cache layer.  Added support for AAAA and CNAME
                   records.
03/27/03   ifk     Added dns_get_test_server_addr(),
                   dns_set_test_server_addr(), dnsi_get_server_addr() and
                   supporting data structures for DNS server address
                   management.
01/08/03   ifk     Added check for negative length in dss_getipnodebyaddr().
                   Check in dss_inet_aton() for addresses bigger then 32 
                   bits.
12/12/02   ifk     Changed dss_errno to int16*.  Fixed a bug where the 
                   network linger timer was getting started even though
                   network bring up failed.  Fixed issue where DS_ETRYAGAIN
                   was returned even though we were not out of resources.
                   If aliases list is empty in dss_hostent return a single
                   NULL entry instead of setting the list to NULL.  Fixed a
                   bug where if the network went down the memory for the 
                   query would be freed.
12/10/02   ifk     dss_getipnodebyname() takes addresses in network order.
                   Fixed bug where dsm item was not being freed correctly in
                   dnsi_sock_write()
11/25/02   ifk     Fixed misc bugs, implemented dss_inet_ntoa() with snprintf
                   added macro for q_delete(), removed extraneous asserts.
11/06/02   ifk     Made dss_errno last argument in getipnode functions.
11/01/02   ifk     Rewrote.
04/17/02   rc      Wrapped code in !FEATURE_DATA and !FEATURE_DATA_WCDMA_PS 
                   as appropriate so builds for all targets.
03/13/02   aku     Added support for new iface_state APP_IFACE_OPEN_PENDING
01/11/02   ss      Changed the state machine to fix some bugs, make the 
                   implementation cleaner and to support net up/down events
12/14/01   ss      Explicitly set the app_iface_state to open if the network
                   subsystem is open
09/25/01   mt      Call the app callback function with the correct app id.
08/23/01   aku     Renamed dss_getifacestatus as dss_get_iface_status
07/20/01   mt      Add dss_gethostbyname().
09/08/00   ttl     Added cache table and refined the function structures.
08/25/00   rc      Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to 
                   return pool id based on pool size.
05/22/00   ttl     Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
#include <stdio.h>                 /* for snprintf used in dss_inet_ntoa() */
#include <string.h>
#include "AEEstd.h"

#include "dssocket.h"
#include "dssdns.h"
#include "pstimer.h"
#include "err.h"
#include "msg.h"
#include "ran.h"

#ifdef FEATURE_DATA_MM
#include "ps_svc.h"
#include "ps_utils.h"
#include "dss_iface_ioctl.h"
#else
#include "psmisc.h"
#include "ps.h"
#endif /* FEATURE_DATA_MM */
#include "assert.h"



/*===========================================================================
                             MACRO DEFINITIONS
===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR                                               
#define Q_DELETE( qptr, linkptr )        q_delete( (qptr), (linkptr) );
#else                                                                       
#define Q_DELETE( qptr, linkptr )        q_delete( (linkptr) );
#endif /* FEATURE_Q_NO_SELF_QPTR */                                         

#define DNS_LOOKUP_PORT            53   /* TCP/UDP port for DNS server     */
#define DNS_TIMEOUT_INTERVAL     3000   /* DNS timeout interval in ms      */
#define DNS_QUERY_TBL_ENTRIES    DSS_DNS_MAX_QUERIES
                                        /* num of query entries            */
#define DNS_SOCK_TBL_ENTRIES     DSS_DNS_MAX_QUERIES
                                        /* num of socket entries           */
#define DNS_NET_TBL_ENTRIES      1      /* num of network entries          */
#define DNS_INADDR_STR_LEN         19   /* max length of IP addr string    */
#define DNS_MSG_HEADER_SIZE        12   /* DNS message header size         */
#define DNS_INADDR_ARPA_STR  ".in-addr.arpa"
                                        /* String used in pointer queries  */
#define DNS_CACHE_TBL_ENTRIES       5   /* Entries in cache table          */
#define DNS_CACHE_TTL_MIN_THRESHOLD 1   /* Minimum ttl in sec to cache     */
#define DNS_CACHE_TTL_MAX_THRESHOLD 300 /* Maximum ttl in sec to cache     */

#define DNS_MIN_MSG_ID  1
#define DNS_MAX_MSG_ID  0x7FFF

/*---------------------------------------------------------------------------
  Bit positions in the parameter field for DNS message header.
  The parameter field is shown below with the LSb(least significant bit)
  labeled 15.  Each bit is indicated as a +.
 
  MSb                                         LSb
  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +
 
  bit 0 (MSb) : operation bit. 0 = Query, 1 = Response
      1-4     : Query type, 0 = standard, 1 = inverse, 2 and 3 obsolete
      5       : set if answer authoritative
      6       : set if message truncated
      7       : set if recursion desired
      8       : set if recursion available
      9-11    : reserved
      12-15   : response type. 0 = no error, 1 = format error in query
                               2 = server failure, 3 = name doesn't exist
---------------------------------------------------------------------------*/
#define DNS_MSG_PARAM_OPERATION   (0x8000) /* operation field              */
#define DNS_MSG_PARAM_REPLY       (0x8000) /* operation field - reply msg  */
#define DNS_MSG_PARAM_QUERY_TYPE  (0x7800) /* query type field             */
#define DNS_MSG_PARAM_TYPE_PTR    (0x0800) /* inverse query type           */
#define DNS_MSG_PARAM_AUTH_ANSWER (0x0400) /* authoritative answer         */
#define DNS_MSG_PARAM_TRUNCATED   (0x0200) /* message truncated            */
#define DNS_MSG_PARAM_RECUR_REQ   (0x0100) /* recursion desired            */
#define DNS_MSG_PARAM_RECUR_AVAIL (0x0080) /* recursion available          */
#define DNS_MSG_PARAM_RESP_TYPE   (0x000f) /* response type                */


/*===========================================================================
                          EXTERNAL FUNCTION PROTOTYPES
===========================================================================*/
extern sint15 dss_write_dsm_chain
(
  sint15          sockfd,            /* socket descriptor                  */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15         *dss_errno          /* error condition value              */
);

extern sint15 dss_read_dsm_chain
(   
  sint15           sockfd,            /* socket descriptor                 */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15          *dss_errno          /* error condition value             */
);


/*===========================================================================
                            LOCAL TYPE DEFINITIONS
===========================================================================*/
/* function to notify upper layer of events */
typedef void (*dns_notify_f_type)
(
  int                    event,        /* The event that occured           */
  int32                  data,         /* Data associated with the event   */
  void                  *cb_voidptr    /* Registered cookie by upper layer */
);


#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*---------------------------------------------------------------------------
  These defnitions are used to support dss_getipaddr() as wrapper around the
  dss_getipnodebyname() function.
---------------------------------------------------------------------------*/
#define DNS_WRAPPER_TBL_ENTRIES    2    /* table entries for the old API   */

/* wrapper table data structure */
typedef struct dns_wrapper_s_type
{
  /* q link */
  q_link_type                link;
  /* query */
  uint8                      name[DSS_DNS_NAME_LEN];
  /* error code */
  int16                      error;
  /* resolved ip address */
  uint32                     ipaddr;
  dss_gethostaddrs_cb_f_type cb_f;
  /* Callback function */
} dns_wrapper_s_type;

static dns_wrapper_s_type dns_wrapper_tbl[DNS_WRAPPER_TBL_ENTRIES];

/* qs' for wrapper table entries, provide mutual exclusion and simplify
 * searching for an entry.  Also act as states for the wrapper state
 * machine.
 */
static q_type dns_wrapper_q_free;
static q_type dns_wrapper_q_pending;
static q_type dns_wrapper_q_valid;
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


/*---------------------------------------------------------------------------
  The socket management subsystem definitions. 
---------------------------------------------------------------------------*/
/* socket layer events notified to query resolution layer */
typedef enum dns_sock_event_e_type
{
  DNS_SOCK_SOCKUP_EVENT      = 0,
  DNS_SOCK_SOCKUP_FAIL_EVENT = 1,
  DNS_SOCK_SOCKDOWN_EVENT    = 2,
  DNS_SOCK_READ_EVENT        = 3,
  DNS_SOCK_WRITE_EVENT       = 4
} dns_sock_event_e_type;

/* socket state enumeration */
typedef enum dns_sock_state_e_type
{
  DNS_SOCK_MIN_STATE     = 0,
  DNS_SOCK_FREE_STATE    = DNS_SOCK_MIN_STATE,
  DNS_SOCK_OPENING_STATE = 1,
  DNS_SOCK_OPEN_STATE    = 2,
  DNS_SOCK_CLOSING_STATE = 3,
  DNS_SOCK_MAX_STATE                   /* This should be the last value    */
} dns_sock_state_e_type;


/* socket management data structure */
typedef struct dns_sock_s_type {
  q_link_type            link;          /* q link                 */
  dns_sock_state_e_type  state;         /* State of the socket    */
  int32                  fd;            /* Socket file descriptor */
  int32                  type;          /* the socket type        */
  int32                  family;        /* protocol family        */
  dns_notify_f_type      notify_f;      /* upper layer notify func*/
  void                  *notify_voidptr;/* notify cb data         */
} dns_sock_s_type;

static dns_sock_s_type   dns_sock_tbl[DNS_SOCK_TBL_ENTRIES];

/* qs' for socket table entries, provide mutual exclusion and simplify
 * searching for an entry.
 */
static q_type dns_sock_q[DNS_SOCK_MAX_STATE];

/* Callback function used by the DSS socket layer to inform application
 * of socket events
 */
typedef void (*dns_dss_cb_f_type) (void *);

/* Application ID used held by the DNS subsystem */ 
int32 dns_app_id = DSS_ERROR;

/* Application ID used by the socket layer */ 
int32 dns_sock_app_id = DSS_ERROR;

/* PS timer used by socket layer */ 
int32 dns_sock_pstimer = DSS_ERROR;

/*---------------------------------------------------------------------------
  DNS message format definitions.  RFC 1035.
---------------------------------------------------------------------------*/
/* format of DNS message header */
typedef struct dns_msg_header_s_type
{
  uint16        id;                        /* message ID                   */
  uint16        parameter;                 /* ops requested, resp codes    */
  uint16        num_questions;             /* num of questions in message  */
  uint16        num_answers;               /* num of answers in message    */
  uint16        num_authority;             /* num of authority sections    */
  uint16        num_additional;            /* num of extra info sections   */
} dns_msg_header_s_type;

/* types of resource records */
typedef enum dns_msg_record_e_type
{
  TYPE_A     =   1,                        /* Host address                 */
  TYPE_NS    =   2,                        /* Name server                  */
  TYPE_MD    =   3,                        /* Mail destination (obsolete)  */
  TYPE_MF    =   4,                        /* Mail forwarder (obsolete)    */
  TYPE_CNAME =   5,                        /* Canonical name               */
  TYPE_SOA   =   6,                        /* Start of Authority           */
  TYPE_MB    =   7,                        /* Mailbox name (experimental)  */
  TYPE_MG    =   8,                        /* Mail group member (exper)    */
  TYPE_MR    =   9,                        /* Mail rename name (exper)     */
  TYPE_NULL  =  10,                        /* Null (experimental)          */
  TYPE_WKS   =  11,                        /* Well-known sockets           */
  TYPE_PTR   =  12,                        /* Pointer record               */
  TYPE_HINFO =  13,                        /* Host information             */
  TYPE_MINFO =  14,                        /* Mailbox information (exper)  */
  TYPE_MX    =  15,                        /* Mail exchanger               */
  TYPE_TXT   =  16,                        /* Text strings                 */
  TYPE_AAAA  =  28,                        /* IPv6 host address            */
  TYPE_ANY   = 255                         /* Matches any type             */
} dns_msg_record_e_type;

/* types of objects */
typedef enum dns_msg_class_e_type
{
  CLASS_IN = 1,                            /* Internet class               */
  CLASS_CS = 2,                            /* CSNET class (obsolete)       */
  CLASS_CH = 3,                            /* CHAOS class                  */
  CLASS_HS = 4                             /* Hesiod                       */
} dns_msg_class_e_type;

/* format of DNS question section */
typedef struct dns_msg_question_s_type
{
  char     *query;                         /* domain name query            */
  uint16    type;                          /* type of resource record      */
  uint16    class;                         /* object class eg IN: internet */
} dns_msg_question_s_type;

/* format of DNS answer section */
typedef struct dns_msg_rr_s_type
{
  char    *owner;                          /* owner of this RR             */
  uint16   type;                           /* type of resource record      */
  uint16   class;                          /* class of object              */
  uint32   ttl;                            /* time the mapping is valid    */
  uint16   rr_length;                      /* resource record data length  */
  char    *rr;                             /* resource record data         */
} dns_msg_rr_s_type;


/*---------------------------------------------------------------------------
  The query management subsystem definitions. 
---------------------------------------------------------------------------*/
/* query resolution state */
typedef enum dns_query_state_e_type
{
  DNS_QUERY_MIN_STATE          = 0,
  DNS_QUERY_FREE_STATE         = DNS_QUERY_MIN_STATE,
  DNS_QUERY_OPENING_STATE      = 1,
  DNS_QUERY_OPEN_SEND_SUBSTATE = 2,
  DNS_QUERY_OPEN_WAIT_SUBSTATE = 3,
  DNS_QUERY_VALID_STATE        = 4,
  DNS_QUERY_MAX_STATE                  /* This should be the last value    */
} dns_query_state_e_type;

typedef enum dns_query_ret_e_type
{
  DNS_DATA_STRUCTURE_DSS_HOSTENT = 1
} dns_query_ret_e_type;

/* query resolution data structure */
typedef struct dns_query_s_type {
  q_link_type             link;         /* q link                */
  dns_query_state_e_type  state;        /* state of query        */
  uint16                  id;           /* Query ID to be sent   */
  dns_msg_question_s_type question;     /* Caonical question     */
  int32                   pstimer;      /* pstimer for timeout   */
  int32                   server_index; /* server being queried  */
  int32                   flags;        /* API func flags param  */
  int32                   sock_index;   /* socket table entry    */
  int32                   sock_type;    /* socket type (UDP/TCP) */
  int32                   error;        /* error                 */
  int32                   af;           /* Address family        */
  dss_dns_cb_f_type       cb;           /* API callback function */
  dns_query_ret_e_type    ret_type;     /* Type of data returned */
  void                   *ret;          /* Pointer to return data*/
  void                   *cb_voidptr;   /* callback function data*/
  dsm_item_type          *dsm_req;      /* dsm item for req msg  */
  dsm_item_type          *dsm_rep;      /* dsm item for rep msg  */
} dns_query_s_type;

static dns_query_s_type  dns_query_tbl[DNS_QUERY_TBL_ENTRIES];

/* q for query table entries, provide mutual exclusion and simplify
 * searching for an entry.  Also act as states for the query state
 * machine.
 */
static q_type dns_query_q[DNS_QUERY_MAX_STATE];


/*---------------------------------------------------------------------------
  The network subsystem definitions.
---------------------------------------------------------------------------*/
/* network state enumeration */
typedef enum dns_net_state_e_type
{
  DNS_NET_MIN_STATE     = 0,           /* place holder for smallest state  */
  DNS_NET_FREE_STATE    = DNS_NET_MIN_STATE,
  DNS_NET_OPENING_STATE = 1,
  DNS_NET_OPEN_STATE    = 2,
  DNS_NET_CLOSING_STATE = 3,
  DNS_NET_MAX_STATE                    /* This should be the last value    */
} dns_net_state_e_type;

/* network events indicated to socket layer */
typedef enum dns_net_event_e_type
{
  DNS_NET_NETUP_EVENT      = 0,
  DNS_NET_NETUP_FAIL_EVENT = 1,
  DNS_NET_NETDOWN_EVENT    = 2
} dns_net_event_e_type;

/* network management data structure */
typedef struct dns_net_s_type
{
  q_link_type              link;          /* q link for network qs */
  dns_net_state_e_type     state;         /* state of network entry*/
  int16                    app_id;        /* application ID        */
  dns_notify_f_type        notify_f;      /* upper layer cb func   */
  void                    *notify_voidptr;/* upper layer cb data   */
} dns_net_s_type;

static dns_net_s_type dns_net_tbl[DNS_NET_TBL_ENTRIES];

/* qs for network table entries, provide mutual exclusion and simplify
 * searching for an entry.
 */
static q_type dns_net_q[DNS_NET_MAX_STATE];


/*---------------------------------------------------------------------------
  Server address management definitions.
---------------------------------------------------------------------------*/
/* The primary and secondary DNS servers enum */
typedef enum dns_server_addr_type_e_type
{
  DNS_SERVER_ADDR_INVALID  = 0,
  DNS_SERVER_ADDR_IPV4     = 1,
  DNS_SERVER_ADDR_IPV6     = 2
} dns_server_addr_type_e_type;

/* The primary and secondary DNS servers enum */
typedef enum dns_server_type_e_type
{
  DNS_SERVER_PRIMARY   = 0,
  DNS_SERVER_SECONDARY = 1,
  DNS_MAX_SERVERS      = 2             /* DNS servers to retry             */
} dns_server_type_e_type;

/* DNS test server address type */
typedef struct dns_test_server_s_type
{
  dns_server_addr_type_e_type type;    /* type of address                  */
  union 
  {
    uint32 v4;                         /* IPv4 address                     */
  } addr;
} dns_test_server_s_type;

/* Array containing DNS test server addresses */
static dns_test_server_s_type dns_test_server_tbl[DNS_MAX_SERVERS];

/*---------------------------------------------------------------------------
  DNS cache definitions
---------------------------------------------------------------------------*/
/* Storage for a cached DNS RR */
typedef struct dns_cache_entry_s_type
{
  q_link_type        link;
  dns_msg_rr_s_type  rr;
} dns_cache_entry_s_type;

/* The cache queue constitutes a one bucket hash table */
static q_type dns_cache_q;

/* Variable that acts to prevent simultaneous access to the DNS cache */
static int dns_cache_lock;

/* PS timer to free cache entries */
static int32 dns_cache_timer;

/*---------------------------------------------------------------------------
  DNS memory subsystem definitions

  DNS allocated memory in chunks with a chunk header prepended to the
  allocated memory.  The chunks maybe part of a larger allocated container
  like DSM in which case the container field points to the DSM header
  which is used for allocation/free with the previous and next header
  pointers NULL.
---------------------------------------------------------------------------*/
/* Types of DNS memory chunk */
typedef enum dns_mem_type_s_type
{
  DNS_MEM_TYPE_NULL = 0,          /* Invalid chunk type value              */
  DNS_MEM_TYPE_DSM  = 1           /* Chunk belongs to a DSM item           */
} dns_mem_type_s_type;

/* The DNS memroy chunk header */
typedef struct dns_mem_header_s_type
{
  dns_mem_type_s_type            type;        /* Type of memory chunk      */
  struct dns_mem_header_s_type  *next_header; /* Next chunk header         */
  struct dns_mem_header_s_type **prev_header;
                        /* Previous chunk's pointer pointing to this chunk */
  void                          *container;
                        /* Set if chunk is part of a DSM like container    */
} dns_mem_header_s_type;

/* Global variable pointing to the DNS memory pool */
static dns_mem_header_s_type *dns_mem_pool;


/*===========================================================================
                           Forward Declarations
===========================================================================*/
/*---------------------------------------------------------------------------
  Socket layer functions
---------------------------------------------------------------------------*/
static int32 dnsi_sock_socket
(
  int32      app_id,                   /* Application id                   */
  int32      family,                   /* Protocol family                  */
  int32      type                      /* Type of socket                   */
);

static int32 dnsi_sock_close
(
  int32 sock_fd                        /* socket handle to close           */
);

static int32 dnsi_sock_register_event
(
  int32 sock_index,                    /* index in socket table            */
  int32 event_mask                     /* events to register for           */
);

static int32 dnsi_sock_async_select
(
  int32 sock_fd,                       /* socket descriptor                */
  int32 event_mask                     /* events to register for           */
);

static int32 dnsi_sock_getnextevent
(
  int32 app_id,                        /* Application id                   */
  int32 sock_fd                        /* Socket descriptor                */
);

static int32 dnsi_sock_write_dsm_chain
(
  int32           sock_fd,             /* socket to write to               */
  dsm_item_type **dsm_chain            /* DSM chain pointer                */
);

static int32 dnsi_sock_read_dsm_chain
(
  int32           sock_fd,             /* socket to read from              */
  dsm_item_type  *dsm_chain            /* DSM chain pointer                */
);

static int32 dnsi_sock_sendto
(
  int32            sock_fd,            /* send socket                      */
  char            *buffer,             /* buffer to write out              */
  int32            buffer_len,         /* data length to write out         */
  struct sockaddr *address,            /* address of server                */
  int32            address_len,        /* length of address field          */
  int32            *error              /* Error to be returned             */
);

static int32 dnsi_sock_recvfrom
(
  int32            sock_fd,            /* socket to write to               */
  char            *buffer,             /* buffer to write out              */
  int32            buffer_len          /* data length to write out         */
);

static int32 dnsi_sock_alloc
(
  int32              family,           /* Protocol family                  */
  int32              type,             /* Type of socket                   */
  dns_notify_f_type  notify_func,      /* Upper layer notify func          */
  void              *notify_voidptr    /* Upper layer specified data       */
);

static void dnsi_sock_dealloc
(
  int32 sock_index                     /* Socket index to dealloc          */
);

static void dnsi_sock_notify
(
  dns_net_event_e_type net_event,      /* The network event                */
  int32                event_data,     /* Data associated with net event   */
  void                *cb_voidptr      /* sockets registered data          */
);

static int32 dnsi_sock_write
(
  int32             sock_index,        /* Write socket index               */
  dsm_item_type   **dsmptr,            /* DSM item to send                 */
  struct sockaddr  *address,           /* Server address for UDP           */
  int32             address_len,       /* Length of address parameter      */
  int32            *errno              /* Error to be returned             */
);

static dsm_item_type *dnsi_sock_read
(
  int32 sock_index                     /* Socket index                     */
);

static void dnsi_sock_cb
(
  void *appid                          /* Application id                   */
);

#ifdef FEATURE_DATA_MM
static
#endif /* ifdef FEATURE_DATA_MM */
void dnsi_sock_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *app_id             /* Application ID                   */
);

static void dnsi_sock_timeout_cb
(
  void *pindex                         /* Not used                         */
);

/*---------------------------------------------------------------------------
  Network layer functions
---------------------------------------------------------------------------*/
static int32 dnsi_net_dss_open_netlib
(
  dns_dss_cb_f_type  net_cb,           /* network callback function        */
  dns_dss_cb_f_type  sock_cb           /* socket callback function         */
);

/* Comment out this function for now */
#if 0
static int32 dnsi_net_dss_close_netlib
(
  int16 appid                          /* application ID                   */
);
#endif /* if 0 */

static int32 dnsi_net_dss_pppopen
(
  int16 appid                          /* application ID                   */
);

static int32 dnsi_net_dss_pppclose
(
  int16 appid                          /* application ID                   */
);

static void dnsi_net_handle_enetnonet
(
  int16 app_id                         /* application ID                   */
);

static void dnsi_net_handle_enetisconn
(
  int16 app_id                         /* application ID                   */
);

static int32 dnsi_net_get_appid
(
  dns_dss_cb_f_type  sock_cb_func,     /* DSS sockets callback             */
  dns_notify_f_type  notify_func,      /* notify upper layer of net events */
  void              *notify_voidptr    /* upper layer registered data      */
);

static int32 dnsi_net_release_appid
(
  uint32  app_id                       /* application ID                   */
);

static void dnsi_net_cb
(
  void *appid                          /* application id                   */
);

#ifdef FEATURE_DATA_MM
static
#endif /* FEATURE_DATA_MM */
void dnsi_net_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *appid              /* application id                   */
);

/*---------------------------------------------------------------------------
  Query layer functions
---------------------------------------------------------------------------*/
static void *dnsi_query_resolve
(
  dns_msg_question_s_type *asked_ques, /* The asked DNS query              */
  dns_query_ret_e_type     ret_type,   /* The data type to be returned     */
  int32                    af,         /* address family requested         */
  int32                    flags,      /* options                          */
  void                    *voidptr,    /* application data                 */
  dss_dns_cb_f_type        callback,   /* callback function pointer        */
  int16                   *dss_errno   /* error code                       */
);

static void dnsi_query_recv
(
  dns_query_s_type  *pquery,           /* Pointer to query entry           */
  dsm_item_type    **dsmptr            /* DSM item containing DNS message  */
);

static void dnsi_query_send
(
  dns_query_s_type  *pquery            /* Query table entry                */
);

static void dnsi_query_notify
(
  dns_sock_event_e_type  event,        /* The lower layer event            */
  int32                  event_data,   /* Data associated with event       */
  void                  *cb_voidptr    /* Query layer registered cb data   */
);

static void dnsi_query_timeout_cb
(
  void *pindex                         /* Index in the query table         */
);

static void dnsi_query_msg_hdr_put
(
  dsm_item_type         **dsmptr,      /* DSM chain pointer                */
  dns_msg_header_s_type  *phdr         /* Filled DNS message header        */
);

static int32 dnsi_query_msg_hdr_get
(
  dsm_item_type         *dsmptr,       /* Pointer to DSM item chain        */
  dns_msg_header_s_type *phdr          /* DNS message header structure     */
);

static int16 dnsi_query_msg_question_put
(
  dsm_item_type           **dsmptr,    /* DSM chain to which question added*/
  dns_msg_question_s_type  *pquestion  /* Domain name question             */
);

static int32 dnsi_query_msg_question_get
(
  dns_query_s_type *pquery,            /* Query table entry                */
  dsm_item_type    *dsmptr,            /* DSM item with the message        */
  int32             offset             /* Offset into the DSM item         */
);

static int16 dnsi_query_msg_answer_get
(
  dns_query_s_type      *pquery,       /* Query table entry                */
  dsm_item_type         *dsmptr,       /* DSM item containing DNS message  */
  int16                  offset,       /* Offset into the DSM item         */
  dns_msg_rr_s_type     *prr           /* Resource record memory           */
);

static int16 dnsi_query_msg_req_build
(
  dns_query_s_type *pquery,            /* Query table entry                */
  uint16            parameter,         /* Parameter field in DNS message   */
  int32            *error_code         /* The error code of any error      */
);

static int16 dnsi_query_msg_rep_parse
(
  dns_query_s_type *pquery,               /* Pointer to query entry        */
  dsm_item_type    *dsmptr                /* DSM item carrying DNS message */
);

static int16 dnsi_query_canonical
(
  dns_msg_question_s_type *asked,      /* Query asked                      */
  dns_msg_question_s_type *formed,     /* Query in canonical form          */
  int32                    af,         /* Address family                   */
  dns_query_ret_e_type     ret_type,   /* Type of return value in ret      */
  void                   **ret,        /* Return value if answer found     */
  int16                   *dss_errno   /* The error value in case of error */
);

static int16 dnsi_query_domain_name_expand
(
  dsm_item_type    *dsmptr,            /* DSM item with the message        */
  uint16            offset,            /* Offset into the DNS item         */
  char             *buf,               /* Buffer for storing the DNS name  */
  int32             buflen,            /* Length of buffer                 */
  uint32           *namelen            /* Length of name                   */
);

static void dnsi_query_start
(
  void *query_index                    /* Index in the query table         */
);

#ifdef FEATURE_DATA_MM
static
#endif /* ifdef FEATURE_DATA_MM */
void dnsi_query_start_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void *query_index                    /* Application id                   */
);

#ifndef FEATURE_DATA_MM
void dnsi_query_stop_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void *query_index                    /* Application id                   */
);
#endif /* ifdef FEATURE_DATA_MM */

static int dnsi_query_hostent_alloc
(
  struct dss_hostent      **phostent,  /* dss_hostent to be allocated      */
  dns_msg_question_s_type  *question,  /* The DNS query information        */
  q_type                   *rr_q,      /* Queue containing answer RRs      */
  int32                     af,        /* Address family                   */
  int16                    *dss_errno  /* Error code                       */
);

static void dnsi_query_hostent_free
(
  struct dss_hostent **phostent        /* The dss_hostent to be freed      */
);

#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*---------------------------------------------------------------------------
  Wrapper layer functions
---------------------------------------------------------------------------*/
static void dnsi_wrapper_cb
(
  struct dss_hostent *phostent,        /* dss_hostent pointer with mapping */
  void               *cb_voidptr,      /* application defined data         */
  int16               dss_errno        /* error code                       */
);
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */

/*---------------------------------------------------------------------------
  Misc functions
---------------------------------------------------------------------------*/
static void* dnsi_memalloc
(
  int32             size               /* Size of memory to be allocated   */
);

static void dnsi_memfree
(
  char            **pmem               /* Memory to free                   */
);

int dns_set_test_server_addr
(
  void                   *addr,        /* Parameter containing IP address  */
  int                     addr_len,    /* Size of memory pointed by addr   */
  int                     af,          /* Address family of IP address     */
  dns_server_type_e_type  server_type  /* Primary or secondary server      */
);

int dns_get_test_server_addr
(
  void                   *addr,        /* Parameter to return IP address in*/
  int                     addr_len,    /* Size of memory pointed by addr   */
  int                     af,          /* Address family of IP address     */
  dns_server_type_e_type  server_type  /* Primary or secondary server      */
);

static int dnsi_get_server_addr
(
  dns_server_type_e_type  server_index,/* Primary or secondary DNS server  */
  int                     sock_index,  /* Socket index for this request    */
  int                     af,          /* Address family of the IP address */
  void                   *addr,        /* Memory for returned address      */
  int                     addr_len     /* Length of addr parameter         */
);

/*---------------------------------------------------------------------------
  Cache subsystem
---------------------------------------------------------------------------*/
static void dnsi_cache_init
(
  void                            /* Void                                  */
);

static void dnsi_cache_entry_free
(
  dns_cache_entry_s_type **pentry /* Cache entry to be freed               */
);

static int dnsi_cache_find
(
  void   *query,                  /* The query to resolve                  */
  uint16  class,                  /* The class of query                    */
  uint16  type,                   /* The type of query                     */
  q_type *cache_q,                /* Cache bucket to get RRs from          */
  q_type *rr_q                    /* Queued RRs to be returned on cache hit*/
);

static void dnsi_cache_add
(
  q_type *rr_q                    /* Queue containing resource records     */
);

static int dnsi_cache_lookup
(
  dns_msg_question_s_type   *question, /* Canonical DNS query              */
  int32                      af,       /* Address family                   */
  dns_query_ret_e_type       ret_type, /* Data structure to return         */
  void                     **ret,      /* Ptr to data structure returned   */
  int16                     *dss_errno /* Error code                       */
);

static void dnsi_cache_timeout_cb
(
  void *pindex                         /* Index in the query table         */
);


/*===========================================================================
                       External Function Definitions
===========================================================================*/
/*===========================================================================

FUNCTION  DSS_DNS_INIT

DESCRIPTION
  Initialize the DNS subsystem.
  
DEPENDENCIES
  None.

RETURN VALUE
  None. 

SIDE EFFECTS
  None.

===========================================================================*/
void dss_dns_init
(
  void
)
{
  dns_sock_s_type    *psock    = dns_sock_tbl;   /* Sock table entry       */
  dns_query_s_type   *pquery   = dns_query_tbl;  /* Query table entry      */
  dns_net_s_type     *pnet     = dns_net_tbl;    /* Network table entry    */
  int32               index    = 0;              /* Loop variable          */
#ifdef FEATURE_DSS_DNS_DEPRECATED_API
  dns_wrapper_s_type *pwrapper = dns_wrapper_tbl;/* Wrapper table entry    */
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Initialize the socket data structures.
  -----------------------------------------------------------------------*/
  for( index = DNS_SOCK_MIN_STATE; index < DNS_SOCK_MAX_STATE; index++ )
  {
    q_init(&dns_sock_q[index]);
  }

  for(; psock < (dns_sock_tbl + DNS_SOCK_TBL_ENTRIES); psock++)
  {
    q_link( psock, &psock->link ); 
    q_put( &dns_sock_q[DNS_SOCK_FREE_STATE], &psock->link ); 
  }

  dns_sock_pstimer = ps_timer_alloc( dnsi_sock_timeout_cb, NULL );
  if( PS_TIMER_FAILURE == dns_sock_pstimer )
  {
    ERR( "Failed to acquire timer", 0, 0, 0 );
    dns_sock_pstimer = DSS_ERROR;
  }

  /*-----------------------------------------------------------------------
    Initialize the query data structures.
  -----------------------------------------------------------------------*/
  for( index = DNS_QUERY_MIN_STATE; index < DNS_QUERY_MAX_STATE; index++ )
  {
    q_init(&dns_query_q[index]);
  }

  for(; pquery < (dns_query_tbl + DNS_QUERY_TBL_ENTRIES); pquery++)
  {
    q_link( pquery, &pquery->link ); 
    q_put( &dns_query_q[DNS_QUERY_FREE_STATE], &pquery->link ); 
  }

  /*-----------------------------------------------------------------------
    Initialize the network data structures.
  -----------------------------------------------------------------------*/
  for( index = DNS_NET_MIN_STATE; index < DNS_NET_MAX_STATE; index++ )
  {
    q_init(&dns_net_q[index]);
  }

  for(; pnet < (dns_net_tbl + DNS_NET_TBL_ENTRIES); pnet++)
  {
    q_link( pnet, &pnet->link ); 
    q_put( &dns_net_q[DNS_NET_FREE_STATE], &pnet->link ); 
  }

#ifdef FEATURE_DSS_DNS_DEPRECATED_API
  /*-----------------------------------------------------------------------
    Initialize the wrapper data structures.
  -----------------------------------------------------------------------*/
  q_init( &dns_wrapper_q_free );
  q_init( &dns_wrapper_q_pending );
  q_init( &dns_wrapper_q_valid );

  for(; pwrapper < (dns_wrapper_tbl + DNS_WRAPPER_TBL_ENTRIES); pwrapper++)
  {
    q_link( pwrapper, &pwrapper->link); 
    q_put( &dns_wrapper_q_free, &pwrapper->link); 
  }
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */

  /*-----------------------------------------------------------------------
    Acquire an application ID for the DNS system.
  -----------------------------------------------------------------------*/
  if( DSS_ERROR == dnsi_net_dss_open_netlib( dnsi_net_cb, dnsi_sock_cb ) )
  {
    ERR( "Could not get application ID", 0, 0, 0 );
  }

  /*-----------------------------------------------------------------------
    Register DNS command handlers with the PS task.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ps_set_cmd_handler( PS_DNS_NET_CB_CMD, dnsi_net_cb_cmd_handler );
  ps_set_cmd_handler( PS_DNS_SOCK_CB_CMD, dnsi_sock_cb_cmd_handler );
  ps_set_cmd_handler( PS_DNS_START_CMD, dnsi_query_start_cmd_handler );
#endif

  /*-----------------------------------------------------------------------
    Initialize test server address table
  -----------------------------------------------------------------------*/
  memset( dns_test_server_tbl,
          0,
          sizeof(dns_test_server_s_type) * DNS_MAX_SERVERS
       	);

  /*-----------------------------------------------------------------------
    Initialize the cache
  -----------------------------------------------------------------------*/
  dnsi_cache_init();

  /*-----------------------------------------------------------------------
    Initialize the memory subsystem
  -----------------------------------------------------------------------*/
  dns_mem_pool = NULL;

  return;
} /* dss_dns_init() */


#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*===========================================================================

FUNCTION  DSS_INIT_DNS_CACHE_TBL

DESCRIPTION
  Deprecated function.  Used to initialize the DNS cache table now simply
  calls dss_dns_init().
  
DEPENDENCIES
  None.

RETURN VALUE
  None. 

SIDE EFFECTS
  None.

===========================================================================*/
void dss_init_dns_cache_tbl
(
  void
)
{
  dss_dns_init();
} /* dss_init_dns_cache_tbl() */
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


/*===========================================================================

                           API FUNCTIONS

===========================================================================*/
/*===========================================================================

FUNCTION DSS_GETIPNODEBYNAME

DESCRIPTION
  This function performs a DNS resolution on the passed domain name.  It
  returns the resolved mapping in a dss_hostent structure.  In case the
  domain name is a dotted decimal ipv4 or dotted decimal / colon seperated
  ipv6 address it is copied to the dss_hostent structure and returned.  If
  a mapping is not found the return value is NULL and the appropriate error
  code is set in dss_errno.  In case the dss_errno value is DS_EWOULDBLOCK
  the resolver was unable to locate the dns mapping in its local cache (if
  one exists) and would query the DNS servers and return the mapping by
  invoking the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

PARAMETERS
  name              string with name of an ipv4 or ipv6 host or dotted 
                    decimal ipv4 or colon seperated/dotted decimal ipv6 host
  af                address family of name.  AF_INET or AF_INET6
  flags             options - none currently supported.
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a dss_hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values. 

  dss_errno Values
  ----------------
  DS_EHOSTNOTFOUND    domain name asked is not known.
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter 
  DS_EOPNOTSUPP       invalid value for the flags parameter
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          domain name is malformed
  DS_EWOULDBLOCK      DNS servers being queried would invoke the callback
                      with the answer 

SIDE EFFECTS
  None.

===========================================================================*/
struct dss_hostent *dss_getipnodebyname
(
  char             *name,                  /* query domain name            */
  int32             af,                    /* address family desired       */
  int32             flags,                 /* options, currently set to 0  */
  dss_dns_cb_f_type callback,              /* callback to return mapping   */
  void             *cb_voidptr,            /* application supplied data    */
  int16            *dss_errno              /* error, if return is NULL     */
)
{
  struct dss_hostent      *phostent = NULL;
  dns_msg_question_s_type  question;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify the parameters are correct.
  -------------------------------------------------------------------------*/
  if( NULL    == name                   ||
      0       == strlen(name)           ||
      NULL    == dss_errno              ||
      (AF_INET != af && AF_INET6 != af) ||
      0       != flags
    )
  {
    if( NULL != dss_errno )
    {
      *dss_errno = ( AF_INET != af && AF_INET6 != af ) ? DS_EAFNOSUPPORT :
	           ((0 != flags)     ? DS_EOPNOTSUPP   : DS_EFAULT);
    }
    ERR( "dss_getipnodebyname(): invalid function parameters", 0, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Setup data structures for a call to dnsi_query_resolve(), call the
      resolve function.
    -----------------------------------------------------------------------*/
    question.query = name;
    question.type  = (AF_INET == af ) ? TYPE_A : TYPE_AAAA;
    question.class = CLASS_IN;
    phostent = dnsi_query_resolve( &question,
                                   DNS_DATA_STRUCTURE_DSS_HOSTENT,
                                   af,
                                   flags,
                                   cb_voidptr,
                                   callback,
                                   dss_errno
			         );
  }

  MSG_LOW( "dss_getipnodebyname():  Returning hostent:%x, dss_errno: %d",
	   phostent,
           (NULL != dss_errno) ? *dss_errno : 0,
	   0
	 );
  return phostent;
} /* dss_getipnodebyname() */


/*===========================================================================

FUNCTION DSS_GETIPNODEBYADDR

DESCRIPTION
  This function performs reverse DNS resolutions on the passed host address
  returning the resolved mapping in a dss_hostent structure.  If a mapping
  is not found the return value is NULL and the appropriate error code is
  set in dss_errno.  In case the dss_errno value is DS_EWOULDBLOCK the
  resolver was unable to locate the dns mapping in its local cache (if one
  exists) and would query the DNS servers returning the mapping by invoking
  the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

  At present support for IPv6 is not added.

PARAMETERS
  addr              based on the value of 'af' this will be either a
                    'struct in_addr' (for af == AF_INET)  or 'struct
		    in6_addr' (for af == AF_INET6)
  len               length of the addr field
  af                address family of addr AF_INET or AF_INET6
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values. 

  dss_errno Values
  ----------------
  DS_EHOSTNOTFOUND    the specified host is unknown
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter 
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          query is malformed
  DS_EWOULDBLOCK      query being processed would invoke the callback
                      with the answer 

SIDE EFFECTS
  None.

===========================================================================*/
struct dss_hostent *dss_getipnodebyaddr
(
  void              *addr,                 /* query domain name            */
  int32              len,                  /* length of the query field    */
  int32              af,                   /* address family queried       */
  dss_dns_cb_f_type  callback,             /* callback to return mapping   */
  void              *cb_voidptr,           /* application supplied data    */
  int16             *dss_errno             /* error, if return is NULL     */
)
{
  struct dss_hostent      *phostent = NULL;
  dns_msg_question_s_type  question;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check the function parameters.
  -------------------------------------------------------------------------*/
  if ( NULL    == addr                   ||
       NULL    == dss_errno              ||
       (AF_INET != af && AF_INET6 != af) ||
       sizeof(struct in_addr) > len      ||
       0 > len
     )
  {
    if( NULL != dss_errno )
    {
      *dss_errno = ( AF_INET != af ) ? DS_EAFNOSUPPORT : DS_EFAULT;
    }
    ERR( "dss_getipnodebyaddr(): invalid function parameters", 0, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Setup data structures for a call to dnsi_query_resolve(), call the
      resolve function.
    -----------------------------------------------------------------------*/
    question.query = addr;
    question.type  = TYPE_PTR;
    question.class = CLASS_IN;
    phostent = dnsi_query_resolve( &question,
                                   DNS_DATA_STRUCTURE_DSS_HOSTENT,
                                   af,
                                   0,
                                   cb_voidptr,
                                   callback,
                                   dss_errno
			         );
  }

  return phostent;
} /* dss_getipnodebyaddr() */


/*===========================================================================

FUNCTION DSS_FREEHOSTENT

DESCRIPTION
  This function is called to free the dss_hostent structure returned by
  dss_getipnodebyname() or dss_getipnodebyaddr() functions.

PARAMETERS
  phostent    pointer to dss_hostent structure to be freed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Frees memory associated with the passed dss_hostent structure.

===========================================================================*/
void dss_freehostent
(
  struct dss_hostent *phostent         /* Pointer to dss_hostent to free   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Free memory associated with the hostent.
  -------------------------------------------------------------------------*/
  if( NULL != phostent )
  {
    dnsi_query_hostent_free( &phostent );
  }

  return;
} /* dss_freehostent() */


#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*===========================================================================

FUNCTION DSS_GETHOSTADDRS

DESCRIPTION

  This function is deprecated in favor of dss_getipnodebyname().

  This function returns the IP address for a query domain name.  It sends a
  request to the DNS server to get the mapping, returning DS_EWOULDBLOCK to
  the caller. The user application can call this function again once it is
  informed by the given callback function to obtain the IP address that
  was requested.

Parameters passed:
  Domain Name:        string containing the domain name for which we need
                      to get the IP address. Example, www.qualcomm.com
  
  IP ADDRESS:         IP address for the querying domain name. If answer is
                      not available, this field will be 0.
  
  Callback function:  This function is called by the resolver after obtaining
                      the IP address.


DEPENDENCIES
  Size of the hostname:         This function assumes that the maximum size
                                of the host name is DSS_DNS_NAME_LEN bytes
				including the <CR>.
  
RETURN VALUE
  DSS_SUCCESS    If mapping was successful.  The ip_addr field will have the
                 the resolved IP address.
  Error number   In case of failure the return value will be the error code
                 and ip_addr will be set to 0.

SIDE EFFECTS
  None
  

===========================================================================*/
int16 dss_gethostaddrs
( 
  char                       *domain_name,  /* Domain name to resolve      */
  uint32                     *ip_addr,      /* IP address mapped to name   */
  dss_gethostaddrs_cb_f_type  cb_func       /* Callback function           */
) 
{ 
  int16               dss_errno = DSS_SUCCESS;
  dns_wrapper_s_type *pwrapper  = NULL;
  struct dss_hostent *phostent  = NULL;
  uint32 domain_len = strlen(domain_name);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Check the parameters.
  -----------------------------------------------------------------------*/
  if ( NULL             == domain_name         ||
       DSS_DNS_NAME_LEN <= domain_len ||
       NULL == ip_addr
     )
  {
    ERR( "dss_gethostaddrs(): Invalid parameters",0,0,0);
    dss_errno = DS_EFAULT;
  }
  else 
  {
    MSG_LOW("dss_gethostaddrs(): called with name %s", domain_name, 0, 0);

    /*---------------------------------------------------------------------
      Check if we already have a mapping.
    ---------------------------------------------------------------------*/
    pwrapper = (dns_wrapper_s_type *) q_check( &dns_wrapper_q_valid );

    while( NULL != pwrapper )
    {
      if( 0 == strcmp((char*) (pwrapper->name), domain_name) )
      {
        /* Found the resolved query */
        Q_DELETE( &dns_wrapper_q_valid, &pwrapper->link );
        *ip_addr  = pwrapper->ipaddr;
        *ip_addr  = dss_ntohl( *ip_addr );
        dss_errno = pwrapper->error;

        /* Free the wrapper data structure */
        memset(pwrapper, 0, sizeof(dns_wrapper_s_type));
        q_put( &dns_wrapper_q_free, &pwrapper->link );
        return dss_errno;
      }

      pwrapper = q_next( &dns_wrapper_q_valid, &pwrapper->link );
    }

    /*-----------------------------------------------------------------------
      New query.  Allocate a wrapper table entry.
    -----------------------------------------------------------------------*/
    if( NULL == (pwrapper = q_get( &dns_wrapper_q_free )))
    {
      dss_errno = DS_ETRYAGAIN;
    }
    else
    {
      memcpy( pwrapper->name,
              domain_name,
              domain_len + 1
            );
      pwrapper->cb_f  = cb_func;
      pwrapper->error = DS_EWOULDBLOCK;
      q_put( &dns_wrapper_q_pending, &pwrapper->link );
      phostent = dss_getipnodebyname( domain_name,
                                      AF_INET,
                                      0,
                                      (NULL == cb_func) ? NULL :
                                       (dss_dns_cb_f_type) dnsi_wrapper_cb,
                                      (void *)(pwrapper - dns_wrapper_tbl),
                                      &dss_errno
                                    );
      /*---------------------------------------------------------------------
        If mapping returned or an error occured, handle it
      ---------------------------------------------------------------------*/
      if( NULL != phostent || DS_EWOULDBLOCK != dss_errno )
      {
        if( NULL != phostent )
        {
          /*-------------------------------------------------------------------
            Obtained a mapping.  Return in ip_addr, free structures
          -------------------------------------------------------------------*/
          *ip_addr = dss_ntohl(*(uint32*)(phostent->h_addr_list[0]));
          dss_freehostent( phostent );
          dss_errno = DSS_SUCCESS;
        }
        /*---------------------------------------------------------------------
          Free the wrapper data structure 
        ---------------------------------------------------------------------*/
        Q_DELETE( &dns_wrapper_q_pending, &pwrapper->link );
        memset(pwrapper, 0, sizeof(dns_wrapper_s_type));
        q_put( &dns_wrapper_q_free, &pwrapper->link );
      }
    }
  } 

  MSG_LOW("dss_gethostaddrs():  returning code %d\n", dss_errno,0,0);
  return dss_errno;
} /* dss_gethostaddrs() */
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


/*===========================================================================

FUNCTION  DSS_INET_ATON

DESCRIPTION
  Convert internet address from dotted string to network ordered struct
  in_addr. 
  
DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR    For failure 
  DSS_SUCCESS  For success

  The IP address is returned in struct in_addr *addr

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_inet_aton
(
  const  char    *cp,
  struct in_addr *addr
)
{
  enum 
  {
    BASE_OCT = 8,
    BASE_DEC = 10,
    BASE_HEX = 16
  }         base        = BASE_DEC;
  int32     index       = 0;
  int32     number      = 0;
  uint32    value       = 0;
  uint32    ipaddr      = 0;
  uint32    check_value = 0;
  uint8    *presult     = (uint8 *) &ipaddr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if( NULL == cp || NULL == addr || 0 == strlen(cp) )
   {
      return DSS_ERROR;
   }

  /*-------------------------------------------------------------------------
    The ASCII string containing the address could be in one of 4 forms:
    i  ) a.b.c.d - each of a,b,c and d is one octet of IPv4 address
    ii ) a.b.c   - a and b are most significant octets, c is remaining 16
                   bits of address.
    iii) a.b     - a is most significant octet while b is remaining 24 bits
    iv ) a       - a is the complete address
   
    Each of a,b,c and d can be a decimal number, an octal number or a hex 
    number.
  -------------------------------------------------------------------------*/
  for(index = 0; index < sizeof(struct in_addr); index++)
  {
   /*------------------------------------------------------------------------
     Determine the base of the number.  If number starts with 0x or 0X then
     base is 16, if the number starts with 0 the next charachter being
     neither x nor X then base 8 else base 10. 
    -----------------------------------------------------------------------*/
    base  = BASE_DEC;
    value = 0;
    if( '0' == *cp )
    {
      /*---------------------------------------------------------------------
        Either a hex or an octal number follows or the number is zero
      ---------------------------------------------------------------------*/
      cp++;
      base = ( 'X' == *cp || 'x' == *cp ) ?  BASE_HEX : BASE_OCT;
      if( BASE_HEX == base )
      {
        /*-------------------------------------------------------------------
          Skip the 'x' or 'X'.
        -------------------------------------------------------------------*/
        cp++;
      }
    }

   /*------------------------------------------------------------------------
     Find the rest of the number.
    -----------------------------------------------------------------------*/
    while( '.' != *cp && 0 != *cp )
    {
      number = *cp - '0';
      switch(base)
      {
        case BASE_DEC:
        case BASE_OCT:
          if(number < 0 || number > ((int32)base - 1))
          {
            /*---------------------------------------------------------------
              Illegal number
            ---------------------------------------------------------------*/
            return DSS_ERROR;
          }
          break;

        case BASE_HEX:
          /*-----------------------------------------------------------------
            Convert to decimal.
          -----------------------------------------------------------------*/
          if(number < 0 || number > 9)
          {
            number = *cp - 'a' + 10;
            if (number < 10 || number > 15)
            {
              number = *cp - 'A' + 10;
              if(number < 10 || number > 15)
              {
                return DSS_ERROR;
              }
            }
          }
          break;

        default:
          ASSERT( 0 );
          return DSS_ERROR;
      } /* switch */

      cp++;

      /*---------------------------------------------------------------------
        Verify that the number can be contained in a 32 bit quantity.
      ---------------------------------------------------------------------*/
      if( ((long long) value * base) + number > 0xffffffff )
      {
        return DSS_ERROR;
      }

      value = (value * (int32)base) + number;
    } /* while */

    /*-----------------------------------------------------------------------
      Each '.' seperated number should be at most 0xff.
    -----------------------------------------------------------------------*/
     if(value > 0xff && *cp != 0)
     {
       return DSS_ERROR;
     }
     else
     {
       if( 0 != *cp )
       {
         *presult++ = value;
       }
       else
       {
         /*------------------------------------------------------------------
           Verify that the last number is in the correct range. 
         ------------------------------------------------------------------*/
         switch( index )
         {
           case 0:
             check_value = 0xffffffff;
             break;

           case 1:
	     check_value = 0xffffff;
	     break;

           case 2:
             check_value = 0xffff;
             break;

           case 3:
             check_value = 0xff;
             break;

           default:
             return DSS_ERROR;
         } /* switch */

	 if( value > check_value )
         {
           return DSS_ERROR;
         }

         ipaddr |= dss_htonl(value);
       }
     }

    /*-----------------------------------------------------------------------
      Break if this was the last number.
    -----------------------------------------------------------------------*/
    if(*cp++ == 0)
    {
      break;
    }
  } /* for */

  addr->s_addr = ipaddr;

  return DSS_SUCCESS;
} /* dss_inet_aton() */


/*===========================================================================

FUNCTION  DSS_INET_NTOA

DESCRIPTION
  Convert network order internet address to dotted string form.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS   If successful
  DSS_ERROR     If an error occured

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_inet_ntoa
(
  const struct in_addr  in,        /* IPv4 address to be converted         */
  uint8                *buf,       /* Buffer to hold the converted address */
  int32                 buflen     /* Length of buffer                     */
)
{
  uint8 *paddr  = (uint8*) &in.s_addr;
  int32  retval = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == buf || 0 == buflen )
  {
    retval = DSS_ERROR;
  }
  else
  {
    if( -1 == snprintf( (char*)buf,
                        buflen, "%d.%d.%d.%d",
                        paddr[0],
                        paddr[1],
                        paddr[2],
                        paddr[3] )
      )
    {
      retval = DSS_ERROR;
    }
  }

  return retval;
} /* dss_inet_ntoa() */


/*===========================================================================
                       Internal Function Definitions
===========================================================================*/
#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*===========================================================================

FUNCTION DNSI_WRAPPER_CB

DESCRIPTION
  This function server as the wrapper callback to the new API functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_wrapper_cb
(
  struct dss_hostent *phostent,        /* dss_hostent pointer with mapping */
  void               *cb_voidptr,      /* application defined data         */
  int16               dss_errno        /* error code                       */
)
{
  dns_wrapper_s_type *pwrapper = NULL; /* Wrapper table entry              */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Acquire and delete the wrapper entry from the pending q.
  ------------------------------------------------------------------------*/
  pwrapper = &dns_wrapper_tbl[(int)cb_voidptr];
  Q_DELETE( &dns_wrapper_q_pending, &pwrapper->link );

  /*------------------------------------------------------------------------
    Copy ip address and error code, put the entry in valid q and call the
    callback.
  ------------------------------------------------------------------------*/
  if( NULL != phostent )
  {
    memcpy( &pwrapper->ipaddr,
            phostent->h_addr_list[0],
            sizeof(struct in_addr)
          );
    dss_freehostent( phostent );
    pwrapper->error = DSS_SUCCESS;
  }
  else
  {
    pwrapper->error    = dss_errno;
  }

  q_put( &dns_wrapper_q_valid, &pwrapper->link );

  pwrapper->cb_f();

  return;
} /* dnsi_wrapper_cb() */
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


/*===========================================================================

FUNCTION DNS_GET_TEST_SERVER_ADDR

DESCRIPTION
  This function returns a primary or secondary DNS server IP address, in 
  network byte order, that is used to test DNS.

DEPENDENCIES
  The address parameter must be of size struct in_addr.  Later when support
  for IPv6 is added it can also be of size struct in6_addr.

RETURN VALUE
  DSS_SUCCESS if a valid server address being returned in addr parameter
  DSS_ERROR   if an error occured

SIDE EFFECTS
  None.

===========================================================================*/
int dns_get_test_server_addr
(
  void                   *addr,        /* Parameter to return IP address in*/
  int                     addr_len,    /* Size of memory pointed by addr   */
  int                     af,          /* Address family of IP address     */
  dns_server_type_e_type  server_type  /* Primary or secondary server      */
)
{
  int retval = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameters
  -------------------------------------------------------------------------*/
  if( NULL                   == addr        ||
      sizeof(struct in_addr) != addr_len    ||
      AF_INET                != af          ||
      (DNS_SERVER_PRIMARY    != server_type &&
       DNS_SERVER_SECONDARY  != server_type)
    )
  {
    retval = DSS_ERROR;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If an address is set copy it to the address parameter.  For now only
      supports IPv4
    -----------------------------------------------------------------------*/
    switch( af )
    {
      case AF_INET:
        if( DNS_SERVER_ADDR_IPV4 == dns_test_server_tbl[server_type].type &&
            sizeof(struct in_addr) == addr_len
          )
        {
          INTLOCK();
          memcpy( addr,
                  &dns_test_server_tbl[server_type].addr.v4,
                  sizeof(struct in_addr)
                );
          INTFREE();
          break;
        }

      default:
        retval = DSS_ERROR;
        break;
     }
  }

  return retval;
} /* dns_get_test_server_addr() */


/*===========================================================================

FUNCTION DNS_SET_TEST_SERVER_ADDR

DESCRIPTION
  This function is called with a primary or secondary DNS server address
  and is used to set the corresponding test server.

DEPENDENCIES
  For now only supports IPv4.  Address should be in network order.

RETURN VALUE
  DSS_SUCCESS if a valid server address was set
  DSS_ERROR   if an error occured

SIDE EFFECTS
  Sets the primary or secondary DNS test server if successful.

===========================================================================*/
int dns_set_test_server_addr
(
  void                   *addr,        /* Parameter containing IP address  */
  int                     addr_len,    /* Size of memory pointed by addr   */
  int                     af,          /* Address family of IP address     */
  dns_server_type_e_type  server_type  /* Primary or secondary server      */
)
{
  int retval = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameters
  -------------------------------------------------------------------------*/
  if( NULL                   == addr        ||
      sizeof(struct in_addr) != addr_len    ||
      AF_INET                != af          ||
      (DNS_SERVER_PRIMARY    != server_type &&
       DNS_SERVER_SECONDARY  != server_type)
    )
  {
    retval = DSS_ERROR;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If a valid address is passed copy it to the DNS test server array.  The
      tasklock is because this function can be called from other task
      contexts.  For now only supports IPv4
    -----------------------------------------------------------------------*/
    switch( af )
    {
      case AF_INET:
        INTLOCK();
        dns_test_server_tbl[server_type].type = DNS_SERVER_ADDR_IPV4;
        memcpy( &dns_test_server_tbl[server_type].addr.v4,
                addr,
                sizeof(struct in_addr)
              );
        INTFREE();
        break;

      default:
        retval = DSS_ERROR;
        break;
    } /* switch */
  }

  return retval;
} /* dnsi_set_test_server_addr() */


/*===========================================================================

FUNCTION DNSI_GET_SERVER_ADDR

DESCRIPTION
  This function is called to acquire a primary or secondary DNS server
  address.

DEPENDENCIES
  None

RETURN VALUE
  DSS_SUCCESS if a valid server address being returned in addr parameter
  DSS_ERROR   if an error occured

SIDE EFFECTS
  None.

===========================================================================*/
static int dnsi_get_server_addr
(
  dns_server_type_e_type  server_index,/* Primary or secondary DNS server  */
  int                     sock_index,  /* Socket index for this request    */
  int                     af,          /* Address family of the IP address */
  void                   *addr,        /* Memory for returned address      */
  int                     addr_len     /* Length of addr parameter         */
)
{
  int                      retval = DSS_SUCCESS;
#ifdef FEATURE_DATA_MM
  dss_id_info_type         id;
  dss_iface_ioctl_id_type  iface;
  int16                    dss_errno;
  dss_iface_ioctl_type     ioctl_name;
  ip_addr_type             ipaddr;
#endif /* FEATURE_DATA_MM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameters
  -------------------------------------------------------------------------*/
  if( NULL                   == addr         ||
      sizeof(struct in_addr) != addr_len     ||
      AF_INET                != af           ||
      (DNS_SERVER_PRIMARY    != server_index &&
       DNS_SERVER_SECONDARY  != server_index)
    )
  {
    retval = DSS_ERROR;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If a test server is set, return that.
    -----------------------------------------------------------------------*/
    retval = dns_get_test_server_addr( addr,
                                       addr_len,
                                       af,
                                       server_index
                                     );

    /*-----------------------------------------------------------------------
      If we did not get a DNS server from a test server being set, try to
      acquire the server with our associated interface.  This is only for
      MM_DATA code at the moment.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
    if( DSS_ERROR == retval )
    {
      /*---------------------------------------------------------------------
        For now this is mapping the interface based on application ID.  This
        can change if we use TCP sockets.
      ---------------------------------------------------------------------*/
      id.type        = DSS_APP_ID;
      id.info.app_id = dns_app_id;
      retval         = dss_get_iface_id( id, &iface );

      /*---------------------------------------------------------------------
        The ioctl is called only if we were successful in getting an
       	interface.
      ---------------------------------------------------------------------*/
      if( DSS_SUCCESS == retval )
      {
        switch( af )
        {
          case AF_INET:
            /*---------------------------------------------------------------
              Call dss_ioctl to get DNS server address.  If the ioctl is
              successful and returns a valid address then copy it to addr.
            ---------------------------------------------------------------*/
            ioctl_name = (DNS_SERVER_PRIMARY == server_index)     ?
                           DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR :
                           DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR;
            retval     = dss_iface_ioctl( &iface,
                                          ioctl_name,
                                          &ipaddr,
                                          &dss_errno
                                        );
            if( DSS_SUCCESS == retval        &&
                IP_ANY_ADDR != ipaddr.type   &&
                0           != ipaddr.addr.v4
              )
            {
              memcpy( addr, &ipaddr.addr.v4, sizeof( struct in_addr ) );
              break;
            }

          default:
            retval = DSS_ERROR;
            break;
        } /* switch */
      }
    }
#else

    /*-----------------------------------------------------------------------
      If we still have not acquired a server address try to get it from NV
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == retval && AF_INET == af )
    {
      memcpy( addr,
#ifdef FEATURE_DATA
              &dsat_qcdns_val[server_index],
#else
              &ds_atcop_qcdns_val[server_index],
#endif /* FEATURE_DATA */
              sizeof( struct in_addr )
            );
      retval = ( 0 == *(uint32*)addr ) ? DSS_ERROR : DSS_SUCCESS;
      *(uint32*)addr = htonl( *(uint32*)addr );
    }
#endif /* FEATURE_DATA_MM */
  }

  return retval;
} /* dnsi_get_server_addr() */


/*===========================================================================

                            DNS SOCKET SUBSYSTEM

===========================================================================*/
/*===========================================================================

FUNCTION DNSI_SOCK_SOCKET

DESCRIPTION
  Wrapper around dss_socket(), handles the error cases.

DEPENDENCIES
  Assume family is AF_INET.

RETURN VALUE
  In case of error return DSS_ERROR otherwise return the socket descriptor.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_socket
(
  int32      app_id,                   /* Application id                   */
  int32      family,                   /* Protocol family                  */
  int32      type                      /* Type of socket                   */
)
{
  int32 retval    = DSS_SUCCESS;       /* Return value                     */
  int16 dss_errno = DSS_SUCCESS;       /* dss_socket() error parameter     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the socket. 
  -------------------------------------------------------------------------*/
  retval = dss_socket( app_id,
                       family,
                       type,
                       (type == SOCK_DGRAM) ?
                       IPPROTO_UDP    :
                       IPPROTO_TCP,
                       &dss_errno
                     );

  /*-------------------------------------------------------------------------
    Handle errors if any.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == retval )
  {
    switch( dss_errno )
    {
      case DS_EAFNOSUPPORT:
      case DS_EBADAPP:
      case DS_EPROTONOSUPPORT:
      case DS_ESOCKNOSUPPORT:
        ERR( "dss_socket() error code: %d", dss_errno, 0, 0 );
        break;

      case DS_EMFILE:
        ERR( "No more sockets of type: %d", type, 0, 0 );
        break;

      default:
        ERR( "Unknown error code: %d for dss_socket()", dss_errno, 0, 0 );
        break;
    } /* switch */
  }

  return retval;
} /* dnsi_sock_socket() */


/*===========================================================================

FUNCTION DNSI_SOCK_CLOSE

DESCRIPTION
  Wrapper around dss_close(), handles error cases.

DEPENDENCIES
  None

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR   if failed.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_close
(
  int32 sock_fd                        /* socket handle to close           */
)
{
  int32 retval    = DSS_SUCCESS;       /* Return value                     */
  int16 dss_errno = DSS_SUCCESS;       /* dss_close() error parameter      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Close the socket, handle errors if any.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == (retval = dss_close( sock_fd, &dss_errno )) )
  {
    switch( dss_errno )
    {
      case DS_EWOULDBLOCK:
        dnsi_sock_async_select( sock_fd, DS_CLOSE_EVENT );
        break;

      case DS_EBADF:
        ERR( "dss_close returned bad sock error: %d", sock_fd, 0, 0 );
        ASSERT( 0 );
        break;

      default:
        ERR( "dss_close returned error: %d", dss_errno, 0, 0 );
        break;
    } /* switch dss_errno */
  }

  return retval;
} /* dnsi_sock_close() */


/*===========================================================================

FUNCTION DNSI_SOCK_REGISTER_EVENT

DESCRIPTION
  Wrapper around dnsi_sock_async_select(), takes the socket index returned
  by dnsi_sock_alloc() rather then the socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR   if failed.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_register_event
(
  int32 sock_index,                    /* index in socket table            */
  int32 event_mask                     /* events to register for           */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Call async select.
  -------------------------------------------------------------------------*/
  return dnsi_sock_async_select( dns_sock_tbl[sock_index].fd, event_mask );
} /* dnsi_sock_register_event() */


/*===========================================================================

FUNCTION DNSI_SOCK_ASYNC_SELECT

DESCRIPTION
  Wrapper around dss_async_select(), handles errors.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR   if failed.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_async_select
(
  int32 sock_fd,                       /* socket descriptor                */
  int32 event_mask                     /* events to register for           */
)
{
  int32 retval    = DSS_SUCCESS;       /* Return value                     */
  int16 dss_errno = DSS_SUCCESS;       /* error parameter                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Call async select, handle errors if any.
  -------------------------------------------------------------------------*/
  retval = dss_async_select( sock_fd, event_mask, &dss_errno );

  if( DSS_ERROR == retval )
  {
    switch( dss_errno )
    {
      case DS_EBADF:
        ERR( "invalid socket descriptor %d", sock_fd, 0, 0 );
	ASSERT( 0 );
        break;

      default:
        ERR( "dss_async_select() error %d", dss_errno, 0, 0 );
        break;
    } /* switch */
  }

  return retval;
} /* dnsi_sock_async_select() */


/*===========================================================================

FUNCTION DNSI_SOCK_GETNEXTEVENT

DESCRIPTION
  Wrapper around dss_getnextevent(), handles errors.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR   if failed.
  event mask if successful.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_getnextevent
(
  int32 app_id,                        /* Application id                   */
  int32 sock_fd                        /* Socket descriptor                */
)
{
  int32 retval    = DSS_SUCCESS;       /* Return value                     */
  int16 dss_errno = DSS_SUCCESS;       /* error parameter                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Call dss_getnextevent(), handle errors if any.
  -------------------------------------------------------------------------*/
  retval = dss_getnextevent( app_id, (int16*)&sock_fd, &dss_errno );
  MSG_LOW( "Socket: %d, event: 0x%x", sock_fd, retval, 0 );

  /*-------------------------------------------------------------------------
    Handle errors if any.
  -------------------------------------------------------------------------*/
  if( 0 == retval )
  {
    retval = DSS_ERROR;
  }
  else
  {
    if( DSS_ERROR == retval )
    {
      switch( dss_errno )
      {
        case DS_EBADF:
          ERR( "Bad socket descriptor: %d", sock_fd, 0, 0 );
          ASSERT( 0 );
          break;

        case DS_EBADAPP:
          ERR( "Bad application	ID: %d", app_id, 0, 0 );
          ASSERT( 0 );
          break;

        default:
          ERR( "Unexpected error: %d", dss_errno, 0, 0 );
          break;
      } /* switch */
    }
  }

  return retval;
} /* dnsi_sock_getnextevent() */


/*===========================================================================

FUNCTION DNSI_SOCK_WRITE_DSM_CHAIN

DESCRIPTION
  Wrapper around dss_write_dsm_chain, handles error cases.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on successfully writing the buffer.
  DSS_ERROR   in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_write_dsm_chain
(
  int32           sock_fd,             /* socket to write to               */
  dsm_item_type **dsm_chain            /* DSM chain pointer                */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* write chain error parameter      */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Write the DSM chain, handle errors if any.
  -------------------------------------------------------------------------*/
  retval = dss_write_dsm_chain( sock_fd, dsm_chain, &dss_errno );

  if( DSS_ERROR == retval )
  {
    switch( dss_errno )
    {
      case DS_ENETDOWN:
        /*-------------------------------------------------------------------
          This is handled in network callback.
        -------------------------------------------------------------------*/
        ERR( "Network down, error: %d", dss_errno, 0, 0 );
        break;

      case DS_EFAULT:
      case DS_EADDRREQ:
        ERR( "Bad arguments, error: %d", dss_errno, 0, 0 );
        break;


      case DS_ENOTCONN:
        /*-------------------------------------------------------------------
          This is handled in socket callback.
        -------------------------------------------------------------------*/
        ERR( "socket not connected", 0, 0, 0 );
        break;

      case DS_ECONNRESET:
      case DS_ECONNABORTED:
      case DS_EIPADDRCHANGED:
        /*-------------------------------------------------------------------
          This is handled in socket callback.
        -------------------------------------------------------------------*/
        ERR( "TCP socket closed, error: %d", dss_errno, 0, 0 );
        break;

      case DS_EWOULDBLOCK:
        /*-------------------------------------------------------------------
          Register for a write event and return.
        -------------------------------------------------------------------*/
        dnsi_sock_async_select( sock_fd, DS_WRITE_EVENT );
        break;

      case DS_EBADF:
        ERR( "Bad socket: %d", sock_fd, 0, 0 );
        break;

      case DS_EOPNOTSUPP:
      case DS_EPIPE:
      default:
        ERR( "dss_write_dsm_chain() return %d", dss_errno, 0, 0 );
        dsm_free_packet(dsm_chain);
        break;
    } /* switch */
  } 

  return retval;
} /* dnsi_sock_write_dsm_chain() */


/*===========================================================================

FUNCTION DNSI_SOCK_READ_DSM_CHAIN

DESCRIPTION
  Wrapper around dss_read_dsm_chain(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on successfully reading.
  DSS_ERROR   in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_read_dsm_chain
(
  int32           sock_fd,             /* socket to read from              */
  dsm_item_type  *dsm_chain            /* DSM chain pointer                */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* dss_read_dsm_chain() parameter   */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Read dsm chain from socket, handle errors.
   ------------------------------------------------------------------------*/
  retval = dss_read_dsm_chain( sock_fd, &dsm_chain, &dss_errno );

  if( DSS_ERROR == retval )
  {
    switch( dss_errno )
    {
      case DS_EWOULDBLOCK:
        /*-------------------------------------------------------------------
          Register for a read event and return.
        -------------------------------------------------------------------*/
        dnsi_sock_async_select( sock_fd, DS_READ_EVENT );
        break;

      case DS_ENETDOWN:
        /*-------------------------------------------------------------------
          This will be handled in the dss network callback.
        -------------------------------------------------------------------*/
        break;

      case DS_ENOTCONN:
      case DS_ECONNRESET:
      case DS_ECONNABORTED:
      case DS_EIPADDRCHANGED:
        /*-------------------------------------------------------------------
          The query layer will be informed in the dss socket callback.
        -------------------------------------------------------------------*/
        ERR( "dss_read_dsm_chain() error %d", dss_errno, 0, 0 );
        break;

      case DS_EBADF:
        ERR( "Bad socket: %d", sock_fd, 0, 0 );
        break;

      case DS_EPIPE:
      case DS_EINVAL:
      case DS_EFAULT:
      case DS_EADDRREQ:
      default:
        ERR("dss_read_dsm_chain() error %d", dss_errno, 0, 0 );
        break;
    }
  }

  return retval;
} /* dnsi_sock_read_dsm_chain() */


/*===========================================================================

FUNCTION DNSI_SOCK_SENDTO

DESCRIPTION
  Wrapper around dss_sendto(), handles error cases.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on successfully writing the buffer.
  DSS_ERROR   in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_sendto
(
  int32            sock_fd,            /* send socket                      */
  char            *buffer,             /* buffer to write out              */
  int32            buffer_len,         /* data length to write out         */
  struct sockaddr *address,            /* address of server                */
  int32            address_len,        /* length of address field          */
  int32            *error              /* Error to be returned             */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* dss_sendto() error parameter     */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Send the buffer, handle errors if any.
  -------------------------------------------------------------------------*/
  retval = dss_sendto( sock_fd,
                       buffer,
                       buffer_len,
                       0,
                       address,
                       sizeof(struct sockaddr),
                       &dss_errno
                     );

  if( DSS_ERROR == retval )
  {
    if( NULL != error )
    {
      *error = dss_errno;
    }

    switch( dss_errno )
    {
      case DS_EADDRREQ:
      case DS_EFAULT:
        ERR( "Invalid parameter error: %d", dss_errno, 0, 0 );
        break;

      case DS_EWOULDBLOCK:
        /*-------------------------------------------------------------------
          Register for a write event and return.
        -------------------------------------------------------------------*/
        dnsi_sock_async_select( sock_fd, DS_WRITE_EVENT );
        break;

      case DS_EMSGSIZE:
        /*-------------------------------------------------------------------
          Insufficient buffer space.
        -------------------------------------------------------------------*/
        ERR( "Insufficient buffer space", 0, 0, 0 );
        break;

      case DS_ENETDOWN:
        /*-------------------------------------------------------------------
          Handle in network callback.
        -------------------------------------------------------------------*/
        ERR( "Network error, net down", 0, 0, 0 );
        break;

      case DS_EBADF:
        ERR( "Bad socket: %d", sock_fd, 0, 0 );
        break;

      case DS_EOPNOTSUPP:
      case DS_EAFNOSUPPORT:
      default:
        ERR( "dss_sendto() returned error: %d", dss_errno, 0, 0 );
        break;
    } /* switch */
  }

  return retval;
} /* dnsi_sock_sendto() */


/*===========================================================================

FUNCTION DNSI_SOCK_RECVFROM

DESCRIPTION
  Wrapper around dss_recvfrom(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  bytes received or DSS_ERROR in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_recvfrom
(
  int32            sock_fd,            /* socket to write to               */
  char            *buffer,             /* buffer to write out              */
  int32            buffer_len          /* data length to write out         */
)
{
  int16           dss_errno = DSS_SUCCESS;/* parameter to dss_recvfrom()   */
  int32           retval    = DSS_SUCCESS;/* return value                  */
  int16           addr_len  = 0;          /* dss_recvfrom() parameter      */
  struct sockaddr addr;                   /* address of remote machine     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize variables.
  -------------------------------------------------------------------------*/
  memset(&addr, 0, sizeof(struct sockaddr));
  addr_len = sizeof(struct sockaddr);

  /*-------------------------------------------------------------------------
    Read datagram and handle errors. 
  -------------------------------------------------------------------------*/
  retval = dss_recvfrom( sock_fd, 
                         buffer, 
                         buffer_len, 
                         0,  
                         &addr,  
                         (uint16*)&addr_len, 
                         &dss_errno
                       );

  if( DSS_ERROR == retval )
  {
    switch( dss_errno )
    {
      case DS_EWOULDBLOCK:
        /*-------------------------------------------------------------------
          In case of DS_EWOULDBLOCK register for READ event and return.
        -------------------------------------------------------------------*/
        dnsi_sock_async_select( sock_fd, DS_READ_EVENT );
        break;

      case DS_EMSGSIZE:
        /*-------------------------------------------------------------------
          Insufficient buffer space.
        -------------------------------------------------------------------*/
        ERR( "Insufficient buffer space", 0, 0, 0 );
        break;

      case DS_ENETDOWN:
        /*-------------------------------------------------------------------
          This will be handled in network callback.
        -------------------------------------------------------------------*/
        ERR( "dss_recvfrom() error: %d", dss_errno, 0, 0 );
        break;

      case DS_EBADF:
        ERR( "Bad socket: %d", sock_fd, 0, 0 );
        break;

      case DS_EAFNOSUPPORT:
      case DS_EFAULT:
      case DS_EOPNOTSUPP:
      case DS_EADDRREQ:
      default:
        ERR("dss_recvfrom() error %d", dss_errno, 0, 0 );
        break;
    } /* switch */
  }

  return retval;
} /* dnsi_sock_recvfrom() */


/*===========================================================================

FUNCTION DNSI_SOCK_ALLOC

DESCRIPTION
  This function is called by the query resolution layer to allocate a socket
  entry in the socket table.  The domain and the type of socket desired
  are passed as arguments.  The function acquires an application ID and 
  starts the lower layers.  The upper layer is informed when the socket is
  open via the query notify callback.

DEPENDENCIES
  Assumed family is AF_INET.

RETURN VALUE
  In case of error return DSS_ERROR, otherwise return DSS_SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_alloc
(
  int32              family,           /* Protocol family                  */
  int32              type,             /* Type of socket                   */
  dns_notify_f_type  notify_func,      /* Upper layer notify func          */
  void              *notify_voidptr    /* Upper layer specified data       */
)
{
  int32            retval = DSS_ERROR; /* Return value                     */
  dns_sock_s_type *psock  = NULL;      /* Socket table entry               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( AF_INET == family );
  ASSERT( SOCK_DGRAM == type || SOCK_STREAM == type );

  /*-------------------------------------------------------------------------
    Cancels timer if we are timing out the network layer.
  -------------------------------------------------------------------------*/
  ps_timer_cancel( dns_sock_pstimer );

  /*-------------------------------------------------------------------------
    Acquire a socket entry.
  -------------------------------------------------------------------------*/
  if( NULL == (psock = q_get( &dns_sock_q[DNS_SOCK_FREE_STATE] )) )
  {
    ERR( "Could not get a socket entry", 0, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Fill in socket entry.  Put in the opening state.
    -----------------------------------------------------------------------*/
    psock->type           = type;
    psock->family         = family;
    psock->notify_f       = notify_func;
    psock->notify_voidptr = notify_voidptr;
    psock->state          = DNS_SOCK_OPENING_STATE;
    q_put( &dns_sock_q[DNS_SOCK_OPENING_STATE], &psock->link );

    /*-----------------------------------------------------------------------
      If don't have an appid acquire one.  If a socket is already in the
      OPENING state then do not call dnsi_net_get_appid again.
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == dns_sock_app_id )
    {
      if( 1 >= q_cnt( &dns_sock_q[DNS_SOCK_OPENING_STATE] ) )
      {
        retval = dnsi_net_get_appid( dnsi_sock_cb,
                                     (dns_notify_f_type) dnsi_sock_notify,
                                     (void *)(psock - dns_sock_tbl)
                                   );
        if( DSS_ERROR == retval )
        {
          /*-------------------------------------------------------------------
            Remove socket from opening q and put in free q
          -------------------------------------------------------------------*/
          Q_DELETE ( &dns_sock_q[psock->state], &psock->link );
          memset( psock, 0, sizeof(struct dns_sock_s_type) );
          q_put( &dns_sock_q[DNS_SOCK_FREE_STATE], &psock->link );
        }
      }
      else
      {
        retval = DSS_SUCCESS;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Already have an application id, get the socket and notify the upper
        layer.  This is achieved by call to dnsi_sock_notify() spoofing a
        net_up event.
      ---------------------------------------------------------------------*/
      retval = DSS_SUCCESS;
      dnsi_sock_notify( DNS_NET_NETUP_EVENT,
                        dns_sock_app_id,
                        (void*)(psock - dns_sock_tbl)
                      );
    }
  }

  return retval;
} /* dnsi_sock_alloc() */


/*===========================================================================

FUNCTION DNSI_SOCK_DEALLOC

DESCRIPTION
  Called with the dnsi_sock_alloc() returned socket index to release the
  socket entry.  The socket is closed and if there are no more sockets open
  a timer is started to release the application ID.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_sock_dealloc
(
  int32 sock_index                     /* Socket index to dealloc          */
)
{
  dns_sock_s_type *psock  = &dns_sock_tbl[sock_index]; /* ptr to sock entry*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Dequeue the socket from the queue.
  -------------------------------------------------------------------------*/
  Q_DELETE( &dns_sock_q[psock->state], &psock->link );

  /*-------------------------------------------------------------------------
    Socket can be deallocated only in the open state by the query layer,
    other sockets code can however call socket deallocate to use common code.
  -------------------------------------------------------------------------*/
  switch( psock->state )
  {
    case DNS_SOCK_OPEN_STATE:
      /*---------------------------------------------------------------------
        Call close on the socket.  If DSS_ERROR returned put socket entry in
        closing state.
      ---------------------------------------------------------------------*/
      psock->notify_f = NULL;
      psock->state = DNS_SOCK_CLOSING_STATE;
      q_put( &dns_sock_q[psock->state], &psock->link );

      if( DSS_ERROR == dnsi_sock_close( psock->fd ) )
      {
        return;
      }
      Q_DELETE( &dns_sock_q[psock->state], &psock->link );
      break;

    case DNS_SOCK_OPENING_STATE:
    case DNS_SOCK_CLOSING_STATE:
      break;

    case DNS_SOCK_FREE_STATE:
    default:
      ERR( "dealloc on invalid socket state: %d", psock->state, 0, 0 );
      ASSERT( 0 );
      break;
  } /* switch */

  /*-------------------------------------------------------------------------
    Free socket entry.
  -------------------------------------------------------------------------*/
  memset(psock, 0, sizeof(dns_sock_s_type));
  q_put(&dns_sock_q[DNS_SOCK_FREE_STATE], &psock->link);

  /*-------------------------------------------------------------------------
    If this was the last socket and we have an app id at the sockets layer,
    release it.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR != dns_sock_app_id &&
      DNS_SOCK_TBL_ENTRIES == q_cnt( &dns_sock_q[DNS_SOCK_FREE_STATE] )
    )
  {
    if( PS_TIMER_FAILURE == ps_timer_start( dns_sock_pstimer,
                                            DNS_TIMEOUT_INTERVAL )
      )
    {
      MSG_ERROR( "Timer start failed for: %d", dns_sock_pstimer, 0, 0 );
      dnsi_sock_timeout_cb( 0 );
    }
  }

  return;
} /* dnsi_sock_dealloc() */


/*===========================================================================

FUNCTION DNSI_SOCK_NOTIFY

DESCRIPTION
  Called by the network management layer to notify the sockets layer of a
  network event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_sock_notify
(
  dns_net_event_e_type net_event,      /* The network event                */
  int32                event_data,     /* Data associated with net event   */
  void                *cb_voidptr      /* sockets registered data          */
)
{
  dns_sock_s_type *psock  = NULL;      /* Ptr to socket entry              */
  int              index  = 0;         /* Loop variable                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Handle the network event.
  -------------------------------------------------------------------------*/
  switch( net_event )
  {
    case DNS_NET_NETUP_EVENT:
      /*---------------------------------------------------------------------
        Bring up all sockets in the opening state.  The event_data contains
        the application id.
      ---------------------------------------------------------------------*/
      TASKLOCK();
      dns_sock_app_id = event_data;
      TASKFREE();

      psock = q_check( &dns_sock_q[DNS_SOCK_OPENING_STATE] );
      while( NULL != psock )
      {
        /*---------------------------------------------------------------------
          Acquire a socket.  If we fail to get a socket and there are no more
          sockets open, release the application ID.
        ---------------------------------------------------------------------*/
        psock->fd = dnsi_sock_socket( event_data, psock->family, psock->type );

        if( DSS_ERROR == psock->fd )
        {
          /*-------------------------------------------------------------------
            Inform the query layer that the socket could not be opened.  Free
            socket entry.  If this is the last socket entry, release app_id.
          -------------------------------------------------------------------*/
          psock->notify_f( DNS_SOCK_SOCKUP_FAIL_EVENT,
                           0,
                           psock->notify_voidptr
                         );

          dnsi_sock_dealloc( psock - dns_sock_tbl );
        }
        else
        {
          /*-------------------------------------------------------------------
            Inform the query layer that the socket is open.  Put the socket
            entry in the open q.
            TODO:  handle the TCP socket connect
          -------------------------------------------------------------------*/
          Q_DELETE( &dns_sock_q[psock->state], &psock->link );
          psock->state = DNS_SOCK_OPEN_STATE;
          q_put( &dns_sock_q[psock->state], &psock->link );

          psock->notify_f( DNS_SOCK_SOCKUP_EVENT,
                           psock - dns_sock_tbl,
                           psock->notify_voidptr
                         );
        }
        psock = q_check( &dns_sock_q[DNS_SOCK_OPENING_STATE] );
      }
      break;

    case DNS_NET_NETDOWN_EVENT:
    case DNS_NET_NETUP_FAIL_EVENT:
      /*---------------------------------------------------------------------
        Bring down all sockets in the OPEN and OPENING states
      ---------------------------------------------------------------------*/
      for( index = DNS_SOCK_OPENING_STATE;
           index <= DNS_SOCK_OPEN_STATE;
           index++
         )
      {
        while( NULL != (psock = q_check( &dns_sock_q[index] )) )
        {
          /*-----------------------------------------------------------------
            Inform the query layer that the socket is down / could not be
            brought up.  Free socket resources.
          -----------------------------------------------------------------*/
          psock->notify_f( (DNS_NET_NETDOWN_EVENT == net_event) ?
                             DNS_SOCK_SOCKDOWN_EVENT            :
                             DNS_SOCK_SOCKUP_FAIL_EVENT,
                           (DNS_NET_NETDOWN_EVENT == net_event) ?
                             psock - dns_sock_tbl               :
                             0,
                           psock->notify_voidptr
                         );

          dnsi_sock_dealloc( psock - dns_sock_tbl );
        } /* while() */
      } /* for */
      break;

    default:
      ERR( "Invalid event: %d for socket: %d", net_event, cb_voidptr, 0 );
      ASSERT( 0 );
      break;
  } /* switch */

  return;
} /* dnsi_sock_notify() */


/*===========================================================================

FUNCTION DNSI_SOCK_WRITE

DESCRIPTION
  This function writes the dsm buffer to the UDP/TCP socket passed in.
  The dsm item is freed before the function returns.

DEPENDENCIES
  The UDP case assumes only one dsm item in the packet chain.
  The socket is already dequeued.

RETURN VALUE
  DSS_SUCCESS on successfully writing the buffer.
  DSS_ERROR   in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_sock_write
(
  int32             sock_index,        /* Write socket index               */
  dsm_item_type   **dsmptr,            /* DSM item to send                 */
  struct sockaddr  *address,           /* Server address for UDP           */
  int32             address_len,       /* Length of address parameter      */
  int32            *error              /* Error to be returned             */
)
{
  dns_sock_s_type *psock   = NULL;     /* Pointer to socket entry          */
  int32            retval  = DSS_ERROR;/* Return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != dsmptr && NULL != *dsmptr );
  psock   = &dns_sock_tbl[sock_index];

  /*-------------------------------------------------------------------------
    Send message using dss_sendto() for UDP and dss_write_dsm_chain() for
    TCP.
  -------------------------------------------------------------------------*/
  if( DNS_SOCK_OPEN_STATE == psock->state )
  {
    switch( psock->type )
    {
      case SOCK_DGRAM:
        ASSERT( NULL != address );

        if( NULL != (*dsmptr)->pkt_ptr )
        {
          ERR( "dss_sendto doesn't support dsm chains", 0, 0, 0 );
        }
        else
        {
          retval = dnsi_sock_sendto( psock->fd,
                                     (char*) (*dsmptr)->data_ptr,
                                     (*dsmptr)->used,
                                     address,
                                     address_len,
                                     error
                                   );
          retval = (DSS_ERROR != retval) ? DSS_SUCCESS : DSS_ERROR;
        }

        dsm_free_packet( dsmptr );
        break;

      case SOCK_STREAM:
        retval = dnsi_sock_write_dsm_chain( psock->fd, dsmptr );
      break;

      default:
        ERR( "Invalid socket type: %d", psock->type, 0, 0 );
        ASSERT( 0 );
        dsm_free_packet( dsmptr );
        break;
    } /* switch */
  }
  else
  {
    dsm_free_packet( dsmptr );
  }

  return retval;
} /* dnsi_sock_write() */


/*===========================================================================

FUNCTION DNSI_SOCK_READ

DESCRIPTION
  This function reads a dsm buffer from the TCP/UDP socket passed in.

DEPENDENCIES
  The UDP case assumes only one dsm item in the packet chain.

RETURN VALUE
  Pointer to the DSM item with the message if successfully received.
  NULL   in case of failure.

SIDE EFFECTS
  None.

===========================================================================*/
static dsm_item_type *dnsi_sock_read
(
  int32 sock_index                     /* Socket index                     */
)
{
  dns_sock_s_type *psock  = NULL;      /* Pointer to socket entry          */
  dsm_item_type   *dsmptr = NULL;      /* Pointer to DSM item for message  */
  int32            retval = 0;         /* Value returned by function call  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Acquire socket entry and allocates memory for the message.
  -------------------------------------------------------------------------*/
  psock = &dns_sock_tbl[sock_index];

  if( NULL == (dsmptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL )) )
  {
    ERR( "Couldn't get a large item", 0, 0, 0 );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Receive message using dss_recvfrom() for UDP and dss_read_dsm_chain()
      for TCP.  Receiving data from socket. 
    -----------------------------------------------------------------------*/
    switch( psock->type )
    {
      case SOCK_DGRAM:
        retval = dnsi_sock_recvfrom( psock->fd,
                                     (char*) dsmptr->data_ptr,
                                     dsmptr->size
                                   );
        dsmptr->used = ( DSS_ERROR == retval ) ? dsmptr->used : retval;
        break;

      case SOCK_STREAM:
        retval = dnsi_sock_read_dsm_chain( psock->fd, dsmptr );
        break;

      default:
        ERR( "Invalid socket type: %d", psock->type, 0, 0 );
        ASSERT( 0 );
        break;
    } /* switch */

    /*-----------------------------------------------------------------------
      If an error occured free the buffer.
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == retval )
    {
      dsm_free_packet( &dsmptr );
    }
  }

  return dsmptr;
} /* dnsi_sock_read() */


/*===========================================================================

FUNCTION DNSI_SOCK_CB

DESCRIPTION
  Callback function for socket events.  Send the PS_DNS_SOCK_CB_CMD to 
  PS task with the application ID as the user specified data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_sock_cb
(
  void *appid                          /* Application id                   */
)
{
#ifndef FEATURE_DATA_MM
  ps_cmd_type cmd_item;
#endif /* ifndef FEATURE_DATA_MM */
  int32       app_id  = *(int16 *)appid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ps_send_cmd( PS_DNS_SOCK_CB_CMD, (void *) app_id );   
#else
  cmd_item.cmd_name         = PS_DNS_SOCK_CB_CMD;
  cmd_item.cmd_arg.dns_data = (void *) app_id;
  ps_cmd( &cmd_item );
#endif /* ifdef FEATURE_DATA_MM */

  return;
} /* dnsi_sock_cb() */


/*===========================================================================

FUNCTION DNSI_SOCK_CB_CMD_HANDLER

DESCRIPTION
  Callback cmd handler function for socket events, handles the
  PS_DNS_SOCK_CB_CMD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DATA_MM
static
#endif /* ifdef FEATURE_DATA_MM */
void dnsi_sock_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *app_id             /* Application ID                   */
)
{
  dns_sock_s_type *psock      = NULL;  /* Pointer to sock table entry      */
  dns_sock_s_type *psock_temp = NULL;  /* Pointer to sock table entry      */
  int32            event      = 0;     /* socket event                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT( PS_DNS_SOCK_CB_CMD == cmd );

  /*-------------------------------------------------------------------------
    Check the open sockets for events posted on them
  -------------------------------------------------------------------------*/
  psock = (dns_sock_s_type *) q_check( &dns_sock_q[DNS_SOCK_OPEN_STATE] );

  while( NULL != psock )
  {
    event = dnsi_sock_getnextevent( dns_sock_app_id, psock->fd );

    /*-----------------------------------------------------------------------
      If dnsi_sock_getnextevent() returned DSS_ERROR skip to next entry.
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == event )
    {
      psock = q_next( &dns_sock_q[DNS_SOCK_OPEN_STATE], &psock->link );
      continue;
    }

    /*-----------------------------------------------------------------------
      Check if this is the close event.
    -----------------------------------------------------------------------*/
    if( event & DS_CLOSE_EVENT )
    {
      /*---------------------------------------------------------------------
        Close the socket and notify the query layer.  Get the next socket
        entry.
      ---------------------------------------------------------------------*/
      dnsi_sock_close( psock->fd );
      psock->notify_f( DNS_SOCK_SOCKDOWN_EVENT, 0, psock->notify_voidptr );
      psock_temp = q_next( &dns_sock_q[DNS_SOCK_OPEN_STATE], &psock->link );

      /*---------------------------------------------------------------------
        Free the socket entry.
      ---------------------------------------------------------------------*/
      dnsi_sock_dealloc( psock - dns_sock_tbl );

      psock = psock_temp;
      continue;
    }

    /*-----------------------------------------------------------------------
      Check if this is the write event.
    -----------------------------------------------------------------------*/
    if( event & DS_WRITE_EVENT )
    {
      /*---------------------------------------------------------------------
        Notify the upper layer.
      ---------------------------------------------------------------------*/
      psock->notify_f( DNS_SOCK_WRITE_EVENT,
                       psock - dns_sock_tbl,
                       psock->notify_voidptr
                     );
    }

    /*-----------------------------------------------------------------------
      Check if this is the read event.
    -----------------------------------------------------------------------*/
    if( event & DS_READ_EVENT )
    {
      /*---------------------------------------------------------------------
        Notify the upper layer.
      ---------------------------------------------------------------------*/
      psock->notify_f( DNS_SOCK_READ_EVENT,
                       psock - dns_sock_tbl,
                       psock->notify_voidptr
                     );

    }

    /*-----------------------------------------------------------------------
      The socket may have been closed in which case get a fresh entry from
      the open socket q.
    -----------------------------------------------------------------------*/
    if( DNS_SOCK_OPEN_STATE == psock->state )
    {
      psock = q_next( &dns_sock_q[DNS_SOCK_OPEN_STATE], &psock->link );
    }
    else
    {
      psock = (dns_sock_s_type *)q_check( &dns_sock_q[DNS_SOCK_OPEN_STATE] );
    }
  } /* while */

  /*-------------------------------------------------------------------------
    Check the closing sockets for events posted on them
  -------------------------------------------------------------------------*/
  psock = (dns_sock_s_type *) q_check( &dns_sock_q[DNS_SOCK_CLOSING_STATE] );

  while( NULL != psock )
  {
    event = dnsi_sock_getnextevent( dns_sock_app_id, psock->fd );

    /*-----------------------------------------------------------------------
      If dnsi_sock_getnextevent() returned DSS_ERROR skip to next entry.
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == event )
    {
      psock= q_next( &dns_sock_q[DNS_SOCK_CLOSING_STATE], &psock->link );
      continue;
    }

    /*-----------------------------------------------------------------------
      Check if this is the close event.
    -----------------------------------------------------------------------*/
    if( event & DS_CLOSE_EVENT )
    {
      /*---------------------------------------------------------------------
        Close the socket and notify the query layer.  Get the next socket
        entry.
      ---------------------------------------------------------------------*/
      dnsi_sock_close( psock->fd );
      psock_temp = q_next(&dns_sock_q[DNS_SOCK_CLOSING_STATE], &psock->link);

      /*---------------------------------------------------------------------
        Free the socket entry.
      ---------------------------------------------------------------------*/
      dnsi_sock_dealloc( psock - dns_sock_tbl );

      psock = psock_temp;
      continue;
    }
    /*-----------------------------------------------------------------------
      The socket may have been closed in which case get a fresh entry from
      the open socket q.
    -----------------------------------------------------------------------*/
    if( DNS_SOCK_CLOSING_STATE == psock->state )
    {
      psock = q_next( &dns_sock_q[DNS_SOCK_CLOSING_STATE], &psock->link );
    }
    else
    {
      psock = (dns_sock_s_type *)
                   q_check( &dns_sock_q[DNS_SOCK_CLOSING_STATE] );
    }
  } /* while */

  return;
} /* dnsi_sock_cb_cmd_handler() */


/*===========================================================================

FUNCTION DNSI_SOCK_TIMEOUT_CB

DESCRIPTION
  Callback function for handling socket timeout.

DEPENDENCIES
  The network entry for this event to occur should be in the closing state.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_sock_timeout_cb
(
  void *pindex                         /* Not used                         */
)
{
  int32 dns_sock_old_app_id;           /* Previous application ID          */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Check if there are any open/opening sockets.  Else release network.
  -------------------------------------------------------------------------*/
  if( DNS_SOCK_TBL_ENTRIES == q_cnt( &dns_sock_q[DNS_SOCK_FREE_STATE] ) )
  {
    TASKLOCK();
    dns_sock_old_app_id = dns_sock_app_id;
    dns_sock_app_id     = DSS_ERROR;
    TASKFREE();

    /*-----------------------------------------------------------------------
      Cancel and free the timer
    -----------------------------------------------------------------------*/
    if( PS_TIMER_FAILURE == ps_timer_cancel( dns_sock_pstimer ) )
    {
      MSG_ERROR( "timer cancel failed for %d", dns_sock_pstimer, 0, 0 );
    }

    /*-------------------------------------------------------------------------
      Close the network
    -------------------------------------------------------------------------*/
    if( DSS_ERROR != dns_sock_old_app_id )
    {
      dnsi_net_release_appid ( dns_sock_old_app_id );
    }
  }

  return;
} /* dnsi_sock_timeout_cb() */


/*===========================================================================

                            DNS NET SUBSYSTEM

===========================================================================*/
/*===========================================================================

FUNCTION DNSI_NET_DSS_OPEN_NETLIB

DESCRIPTION
  Wrapper function for dss_open_netlib(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  successful.
  DSS_ERROR    error occured.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_net_dss_open_netlib
(
  dns_dss_cb_f_type  net_cb,           /* network callback function        */
  dns_dss_cb_f_type  sock_cb           /* socket callback function         */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* parameter to dss_open_netlib()   */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if we already hold an application ID.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR != dns_app_id )
  {
    retval = dns_app_id;
  }
  else
  {
    /*-----------------------------------------------------------------------
      call dss_open_netlib(), handle errors.
    -----------------------------------------------------------------------*/
    if( DSS_ERROR == (retval = dss_open_netlib( net_cb,
                                                sock_cb,
                                                &dss_errno))
      )
    {
      switch( dss_errno )
      {
        case DS_EMAPP:
          ERR( "Out of application IDs", 0, 0, 0 );
          break;

        default:
          ERR( "Unknown error: %d", dss_errno, 0, 0 );
          break;
      }
    }

    /*-----------------------------------------------------------------------
      Set the global DNS application ID
    -----------------------------------------------------------------------*/
    TASKLOCK();
    dns_app_id = retval;
    TASKFREE();
  }

  return retval;
} /* dnsi_net_dss_open_netlib() */


/* Comment out this function for now */
#if 0
/*===========================================================================

FUNCTION DNSI_NET_DSS_CLOSE_NETLIB

DESCRIPTION
  Wrapper function for dss_close_netlib(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  successful.
  DSS_ERROR    error occured.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_net_dss_close_netlib
(
  int16 appid                          /* application ID                   */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* parameter to dss_close_netlib()  */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    call dss_close_netlib(), handle errors.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == (retval = dss_close_netlib( appid, &dss_errno)) )
  {
    switch( dss_errno )
    {
      case DS_SOCKEXIST:
      case DS_ENETEXIST:
        /*-------------------------------------------------------------------
          this should never occur, we are hopelessly confused
        -------------------------------------------------------------------*/
        ERR_FATAL( "Sockets exist / network exists: %d ", dss_errno, 0, 0 );
        break;

      case DS_EBADAPP:
        ERR( "Bad appid: %d", appid, 0, 0 );
        ASSERT( 0 );
        break;

      default:
        ERR( "Unknown error: %d", dss_errno, 0, 0 );
        break;
    }
  }

  return retval;
} /* dnsi_net_dss_close_netlib() */
#endif /* if 0 */


/*===========================================================================

FUNCTION DNSI_NET_DSS_PPPOPEN

DESCRIPTION
  Wrapper function for dss_pppopen(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  successful.
  DSS_ERROR    error occured.

SIDE EFFECTS
  None

===========================================================================*/
static int32 dnsi_net_dss_pppopen
(
  int16 appid                          /* application ID                   */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* parameter to dss_pppopen()       */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    call dss_pppopen(), handle errors.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == (retval = dss_pppopen( appid, &dss_errno )) )
  {
    switch( dss_errno )
    {
      case DS_EWOULDBLOCK:
        retval = DSS_SUCCESS;
        break;

      case DS_ENETCLOSEINPROGRESS:
        ERR( "Net close in progress", 0, 0, 0 );
	break;

      case DS_EOPNOTSUPP:
        ERR( "Mobile in laptop call", 0, 0, 0 );
        break;

      case DS_EBADAPP:
        ERR( "Bad appid: %d", appid, 0, 0 );
        ASSERT( 0 );
        break;

      default:
        ERR( "Unexpected error: %d", dss_errno, 0, 0 );
        break;
    }
  }

  return retval;
} /* dnsi_net_dss_pppopen() */


/*===========================================================================

FUNCTION DNSI_NET_DSS_PPPCLOSE

DESCRIPTION
  Wrapper function for dss_pppclose(), handles error conditions.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  successful.
  DSS_ERROR    error occured.

SIDE EFFECTS
  None

===========================================================================*/
static int32 dnsi_net_dss_pppclose
(
  int16 appid                          /* application ID                   */
)
{
  int16 dss_errno = DSS_SUCCESS;       /* parameter to dss_pppclose()      */
  int32 retval    = DSS_SUCCESS;       /* return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    call dss_pppclose(), handle errors.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == (retval = dss_pppclose( appid, &dss_errno )) )
  {
    switch( dss_errno )
    {
      case DS_EWOULDBLOCK:
      case DS_ENETCLOSEINPROGRESS:
        retval = DSS_SUCCESS;
        break;

      case DS_EBADAPP:
        ERR( "Bad appid: %d", appid, 0, 0 );
        ASSERT( 0 );
        break;

      default:
        ERR( "Unexpected error: %d", dss_errno, 0, 0 );
        break;
    }
  }

  return retval;
} /* dnsi_net_dss_pppclose() */


/*===========================================================================

FUNCTION DNSI_NET_HANDLE_ENETNONET

DESCRIPTION
  Handles DS_ENETNONET network event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_net_handle_enetnonet
(
  int16 app_id                         /* application ID                   */
)
{
  dns_net_s_type       *pnet  = NULL;  /* pointer to network table entry   */
  dns_net_state_e_type  index = DNS_NET_MIN_STATE; /* loop variable        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Action needs to be taken if the network entry is in open, opening or
    closing queues. 
  -------------------------------------------------------------------------*/
  for( index = DNS_NET_MIN_STATE; index < DNS_NET_MAX_STATE; index++ )
  {
    pnet = q_check( &dns_net_q[index]);

    while( NULL != pnet )
    {
      if( pnet->app_id == app_id )
      {
        switch( index )
        {
          case DNS_NET_OPEN_STATE:
            /*---------------------------------------------------------------
              Notify sockets layer that the network went down.  The sockets
              layer will call dnsi_net_release_appid().
            ---------------------------------------------------------------*/
            pnet->notify_f( DNS_NET_NETDOWN_EVENT,
                            app_id,
                            pnet->notify_voidptr
                          );
	    break;

          case DNS_NET_OPENING_STATE:
          case DNS_NET_CLOSING_STATE:
            /*---------------------------------------------------------------
              For entry in opening state, notify sockets layer that we
              failed to bring up the network, close netlib and free entry.
              For entry in closing state close netlib and free entry.
            ---------------------------------------------------------------*/
            if( DNS_NET_OPENING_STATE == index )
            {
              pnet->notify_f( DNS_NET_NETUP_FAIL_EVENT,
                              0,
                              pnet->notify_voidptr
                            );
            }

            Q_DELETE( &dns_net_q[index], &pnet->link );

            // dnsi_net_dss_close_netlib( pnet->app_id );
            memset( pnet, 0, sizeof(dns_net_s_type) );
            q_put( &dns_net_q[DNS_NET_FREE_STATE], &pnet->link );
	    break;

          default:
            ASSERT( 0 );
            break;
        } /* switch */

        return;
      }

      pnet = q_next( &dns_net_q[index], &pnet->link );
    } /* while */
  } /* for */

  return;
} /* dnsi_net_handle_enetnonet() */


/*===========================================================================

FUNCTION DNSI_NET_HANDLE_ENETISCONN

DESCRIPTION
  Handles DS_ENETISCONN network event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_net_handle_enetisconn
(
  int16 app_id                         /* application ID                   */
)
{
  dns_net_s_type *pnet = NULL;         /* pointer to network table entry   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    The network up event affects only the network entries in the opening
    state.  Find the entry corresponding to this application id in the
    opening q.  If the entry is located, notify sockets layer and
    transition to open state.
  -------------------------------------------------------------------------*/
  pnet = q_check( &dns_net_q[DNS_NET_OPENING_STATE] );

  while( NULL != pnet )
  {
    if( pnet->app_id == app_id )
    {
      Q_DELETE( &dns_net_q[DNS_NET_OPENING_STATE], &pnet->link );
      pnet->state = DNS_NET_OPEN_STATE;
      q_put( &dns_net_q[pnet->state], &pnet->link );
      pnet->notify_f( DNS_NET_NETUP_EVENT,
                      pnet->app_id,
                      pnet->notify_voidptr
                    );
      break;
    }

    pnet = q_next( &dns_net_q[DNS_NET_OPENING_STATE], &pnet->link );
  } /* while */

  return;
} /* dnsi_net_handle_enetisconn() */


/*===========================================================================

FUNCTION DNSI_NET_GET_APPID

DESCRIPTION
  This function is called by the upper layers to acquire an app id and to
  bring up the network.  The upper layer registers notifiction and socket
  callback functions.  The notify function is used by the network layer to
  inform the upper layer of network events while the socket callback is
  registerd with netlib and is used to asynchronously inform the socket
  management layer of socket events.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  successful.
  DSS_ERROR    error occured.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_net_get_appid
(
  dns_dss_cb_f_type  sock_cb_func,     /* DSS sockets callback             */
  dns_notify_f_type  notify_func,      /* notify upper layer of net events */
  void              *notify_voidptr    /* upper layer registered data      */
)
{
  int32           retval = DSS_SUCCESS; /* return value                    */
  dns_net_s_type *pnet   = NULL;        /* network table entry pointer     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != notify_func  );
  ASSERT( NULL != sock_cb_func );

  /*-------------------------------------------------------------------------
    Allocate a network table entry from the free q.  Fill it and attempt to
    bring up the network
  -------------------------------------------------------------------------*/
  if( NULL == (pnet = q_get( &dns_net_q[DNS_NET_FREE_STATE] )) )
  {
    ERR( "Out of network table entries", 0, 0, 0 );
    retval = DSS_ERROR;
  }
  else
  {
    pnet->state          = DNS_NET_OPENING_STATE;
    pnet->notify_f       = notify_func;
    pnet->notify_voidptr = notify_voidptr;

    /*-----------------------------------------------------------------------
      open netlib, acquire application ID.
    -----------------------------------------------------------------------*/
    pnet->app_id = dnsi_net_dss_open_netlib( dnsi_net_cb, sock_cb_func );

    if( DSS_ERROR == pnet->app_id )
    {
      retval = DSS_ERROR;
      memset( pnet, 0, sizeof(dns_net_s_type) );
      q_put( &dns_net_q[DNS_NET_FREE_STATE], &pnet->link );
    }
    else
    {
      /*---------------------------------------------------------------------
        bring up the network.  The network up event occurs in the network
	callback.
      ---------------------------------------------------------------------*/
      q_put( &dns_net_q[pnet->state], &pnet->link );

      if( DSS_ERROR == (retval = dnsi_net_dss_pppopen( pnet->app_id )) )
      {
        // dnsi_net_dss_close_netlib( pnet->app_id );
        Q_DELETE( &dns_net_q[pnet->state], &pnet->link );
        memset( pnet, 0, sizeof(dns_net_s_type) );
        q_put( &dns_net_q[DNS_NET_FREE_STATE], &pnet->link );
      }
    }
  }

  return retval;
} /* dnsi_net_get_appid() */


/*===========================================================================

FUNCTION DNSI_NET_RELEASE_APPID

DESCRIPTION
  This function is called to release an application id acquired earlier
  by a call to dnsi_net_get_appid().

DEPENDENCIES
  The network entry corresponding to app id must be in open state.

RETURN VALUE
  DSS_ERROR   if unsuccessful.
  DSS_SUCCESS if successful.

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_net_release_appid
(
  uint32  app_id                       /* application ID                   */
)
{
  int32           retval = DSS_SUCCESS;/* return value                     */
  dns_net_s_type *pnet   = NULL;       /* pointer to net table entry       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find the network entry corresponding to this application ID.  An app id
    can only be released if the network entry is in the open state ASSERT
    otherwise.
  -------------------------------------------------------------------------*/
  pnet = (dns_net_s_type*) q_check( &dns_net_q[DNS_NET_OPEN_STATE] );

  while( NULL != pnet )
  {
    if( pnet->app_id == app_id )
    {
      Q_DELETE( &dns_net_q[DNS_NET_OPEN_STATE], &pnet->link );
      break;
    }

    pnet = q_next( &dns_net_q[DNS_NET_OPEN_STATE], &pnet->link );
  }

  /*-------------------------------------------------------------------------
    Assert if network entry not found.
  -------------------------------------------------------------------------*/
  if( NULL == pnet )
  {
    ERR( "Appid release called on non-open network", 0, 0, 0 );
    ASSERT( 0 );
    retval = DSS_ERROR;
  }
  else
  {
      pnet->notify_f = NULL;
      pnet->state    = DNS_NET_CLOSING_STATE;
      q_put( &dns_net_q[pnet->state], &pnet->link );

      /*---------------------------------------------------------------------
        Close the network
      ---------------------------------------------------------------------*/
      if( DSS_ERROR == dnsi_net_dss_pppclose( pnet->app_id ) )
      {
        Q_DELETE( &dns_net_q[pnet->state], &pnet->link );
        // dnsi_net_dss_close_netlib( pnet->app_id );
        memset( pnet, 0, sizeof(dns_net_s_type) );
        q_put( &dns_net_q[DNS_NET_FREE_STATE], &pnet->link );
      }
  }

  return retval;
} /* dnsi_net_release_appid() */


/*===========================================================================

FUNCTION DNSI_NET_CB

DESCRIPTION
  Callback function for network events.  Send the PS_DNS_NET_CB_CMD to 
  PS task with the application ID as the user specified data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_net_cb
(
  void *appid                          /* application id                   */
)
{
#ifndef FEATURE_DATA_MM
  ps_cmd_type cmd_item;
#endif /* ifndef FEATURE_DATA_MM */
  int32       app_id  = *(int16 *)appid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ps_send_cmd( PS_DNS_NET_CB_CMD, (void *) app_id );   
#else
  cmd_item.cmd_name         = PS_DNS_NET_CB_CMD;
  cmd_item.cmd_arg.dns_data = (void *)app_id;
  ps_cmd( &cmd_item );
#endif /* ifdef FEATURE_DATA_MM */

  return;
} /* dnsi_net_cb() */


/*===========================================================================

FUNCTION DNSI_NET_CB_CMD_HANDLER

DESCRIPTION
  Callback cmd handler function for network events, handles the
  PS_DNS_NET_CB_CMD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DATA_MM
static
#endif /* FEATURE_DATA_MM */
void dnsi_net_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *appid              /* application id                   */
)
{
  int16             netstatus = 0;    /* variable to hold network status   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT( PS_DNS_NET_CB_CMD == cmd );

  /*-------------------------------------------------------------------------
    Obtain network status for the passed application ID.
  -------------------------------------------------------------------------*/
  dss_netstatus( (int) appid, &netstatus );

  switch( netstatus )
  {
    case DS_ENETNONET:
      /*---------------------------------------------------------------------
        Handle the network down event.
      ---------------------------------------------------------------------*/
      dnsi_net_handle_enetnonet( (int) appid );
      break;

    case DS_ENETISCONN:
      /*---------------------------------------------------------------------
        Handle the network up event.
      ---------------------------------------------------------------------*/
      dnsi_net_handle_enetisconn( (int) appid );
      break;

    case DS_ENETCLOSEINPROGRESS:
    case DS_ENETINPROGRESS:
      break;

    case DS_EBADAPP:
      ERR( "Bad application id: %d", (int) appid, 0, 0 );
      ASSERT( 0 );
      break;

    default:
      /*---------------------------------------------------------------------
        Unexpected network status.
      ---------------------------------------------------------------------*/
      ERR( "Unexpected net status: %d", netstatus, 0, 0 );
      break;
  }

  return;
} /* dnsi_net_cb() */


/*===========================================================================

                          DNS QUERY SUBSYSTEM

===========================================================================*/
/*===========================================================================

FUNCTION DNSI_QUERY_RESOLVE

DESCRIPTION
  The main entry point into the DNS resolver code.

  Use the information provided to verify and form a DNS query.  If the
  query cannot be answered locally acquire a query table entry and queue
  a PS command to start query processing.

  Looking for the answer locally occurs in application task context.

DEPENDENCIES
  Only getting SOCK_DGRAM for now.

RETURN VALUE
  The return value is a pointer which if non-NULL contains the mapping.
  The data type of the returned structure is passed in as the parameter
  ret_type.

  If the mapping wasn't locally found or an error occured resulting in a
  return value of NULL then the dss_errno parameter contains a suitable
  error code.

SIDE EFFECTS
  Starts query processing if a valid domain name question is asked and a 
  mapping couldn't be locally found.

===========================================================================*/
static void *dnsi_query_resolve
(
  dns_msg_question_s_type *asked_ques, /* The asked DNS query              */
  dns_query_ret_e_type     ret_type,   /* The data type to be returned     */
  int32                    af,         /* address family requested         */
  int32                    flags,      /* options                          */
  void                    *voidptr,    /* application data                 */
  dss_dns_cb_f_type        callback,   /* callback function pointer        */
  int16                   *dss_errno   /* error code                       */
)
{
  dns_query_s_type        *pquery      = NULL;
  void                    *ret         = NULL;
  dns_msg_question_s_type  formed_ques;
  int                      retval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the query asked is valid and form a standard domain name 
    query using the passed information.  In case of an error return a NULL
    with dss_errno set to the error code.
  -------------------------------------------------------------------------*/
  formed_ques.query = NULL;
  retval = dnsi_query_canonical( asked_ques,
                                 &formed_ques,
                                 af,
                                 ret_type,
                                 &ret,
                                 dss_errno );

  if( DSS_ERROR == retval || NULL != ret )
  {
    if( NULL != formed_ques.query )
    {
      dnsi_memfree( &formed_ques.query );
    }
    return ret;
  }

  /*-------------------------------------------------------------------------
    Lookup the cache.
  -------------------------------------------------------------------------*/
  if( DSS_SUCCESS == dnsi_cache_lookup( &formed_ques,
                                        af,
                                        ret_type,
                                        &ret,
                                        dss_errno ) )
  {
    if( NULL != formed_ques.query )
    {
      dnsi_memfree( &formed_ques.query );
    }
    return ret;
  }

  /* If cache lookup fails resolve the query */

  /*-------------------------------------------------------------------------
    Check if callback is NULL, if it is return DS_EFAULT.
  -------------------------------------------------------------------------*/
  if( NULL == callback )
  {
    *dss_errno = DS_EFAULT;
    if( NULL == formed_ques.query )
    {
      dnsi_memfree( &formed_ques.query );
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      Get a query table entry.  If we couldn't get one return DS_ETRYAGAIN else
      fill the query table entry and start the lower layers.
    -------------------------------------------------------------------------*/
    pquery = q_get( &dns_query_q[DNS_QUERY_FREE_STATE] );

    if( NULL == pquery )
    {
      *dss_errno = DS_ETRYAGAIN;
      if( NULL != formed_ques.query )
      {
        dnsi_memfree( &formed_ques.query );
      }
    }
    else
    {
      /*-----------------------------------------------------------------------
        Fill query resolution structure.  No IPv6 support now.
      -----------------------------------------------------------------------*/
      pquery->af             = af;
      pquery->question.query = formed_ques.query;
      pquery->question.class = formed_ques.class;
      pquery->question.type  = formed_ques.type;
      pquery->sock_type      = SOCK_DGRAM;
      pquery->sock_index     = DSS_ERROR;
      pquery->flags          = flags;
      pquery->cb             = callback;
      pquery->id             = (uint16)ran_dist( ran_next(),
                                                 DNS_MIN_MSG_ID,
                                                 DNS_MAX_MSG_ID );

      pquery->cb_voidptr     = voidptr;
      pquery->ret_type       = ret_type;
      pquery->pstimer        = ps_timer_alloc( dnsi_query_timeout_cb,
                                               (void*)( pquery - 
                                                        dns_query_tbl)
                                             );
      pquery->state          = DNS_QUERY_OPENING_STATE;
      *dss_errno             = DS_EWOULDBLOCK;

      q_put( &dns_query_q[pquery->state], &pquery->link );
      dnsi_query_start( (void*) (pquery - dns_query_tbl) );
    }
  }

  return NULL;
} /* dnsi_query_resolve() */


/*===========================================================================

FUNCTION DNSI_QUERY_RECV

DESCRIPTION
  Receive the DNS reply message, parse it.

DEPENDENCIES
  Query entry is already dequeued.  The entry should not be re-queued here.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_recv
(
  dns_query_s_type  *pquery,           /* Pointer to query entry           */
  dsm_item_type    **dsmptr            /* DSM item containing DNS message  */
)
{
  uint16            msg_len = 0;       /* DNS message length for TCP       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != dsmptr && NULL != *dsmptr );
  ASSERT( NULL != pquery );

  /*-------------------------------------------------------------------------
    Process the message if the query is in the wait substate.
  -------------------------------------------------------------------------*/
  if( DNS_QUERY_OPEN_WAIT_SUBSTATE == pquery->state )
  {
    /*-----------------------------------------------------------------------
      Form the message based on the type of socket.
    -----------------------------------------------------------------------*/
    switch( pquery->sock_type )
    {
      case SOCK_DGRAM:
        dnsi_query_msg_rep_parse( pquery, *dsmptr );
        dsm_free_packet( dsmptr );
        break;

      case SOCK_STREAM:
        /*-------------------------------------------------------------------
          Add the dsm item to the DSM chain already read.
        -------------------------------------------------------------------*/
        dsm_append( &pquery->dsm_rep, dsmptr );

        /*-------------------------------------------------------------------
          For a TCP message the first two bytes are the message length.
        -------------------------------------------------------------------*/
        if( 0 == dsm_extract( pquery->dsm_rep, 0, &msg_len, sizeof(uint16) ) )
        {
          break;
        }

        /*-------------------------------------------------------------------
          If we have the complete message then parse it.
        -------------------------------------------------------------------*/
        while( (pquery->dsm_rep)->used >= msg_len )
        {
          dsm_pullup( &pquery->dsm_rep, NULL, sizeof(uint16) );
          dnsi_query_msg_rep_parse( pquery, pquery->dsm_rep );

          /*-----------------------------------------------------------------
            Remove the message from the chain.
          -----------------------------------------------------------------*/
          dsm_pullup( &pquery->dsm_rep, NULL, msg_len );

          /*-----------------------------------------------------------------
            If we have portion of the next message get its length.
          -----------------------------------------------------------------*/
          if( 0 == dsm_extract( pquery->dsm_rep,
                                0,
                                &msg_len,
                                sizeof(uint16) )
            )
          {
            break;
          }
        }
        break;

      default:
        ERR( "Invalid socket type: %d", pquery->sock_type, 0, 0 );
        ASSERT( 0 );
        break;
    }
  }
  else
  {
    dsm_free_packet( dsmptr );
  }

  return;
} /* dnsi_query_recv() */


/*===========================================================================

FUNCTION DNSI_QUERY_SEND

DESCRIPTION
  Send queries on the associated socket index.

DEPENDENCIES
  Query entry is already dequeued.  The entry should not be re-queued here.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_send
(
  dns_query_s_type  *pquery            /* Query table entry                */
)
{
  int32               retval = 0;      /* Return value                     */
  struct sockaddr     addr;            /* DNS server address               */
  struct sockaddr_in *paddr  = NULL;   /* pointer to DNS server address    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Send a query only if we are in the send substate.
  -------------------------------------------------------------------------*/
  if( DNS_QUERY_OPEN_SEND_SUBSTATE == pquery->state )
  {
    /*-----------------------------------------------------------------------
      Prepare DNS lookup query packet.  The message is placed in the dsm_req
      field of pquery.
    -----------------------------------------------------------------------*/
    retval = dnsi_query_msg_req_build( pquery,
                                       DNS_MSG_PARAM_RECUR_REQ,
                                       &pquery->error
                                     );

    if( DSS_SUCCESS == retval )
    {
      /*---------------------------------------------------------------------
        Acquire DNS server address for UDP socket.
      ---------------------------------------------------------------------*/
      if( SOCK_DGRAM == pquery->sock_type )
      {
        paddr                  = (struct sockaddr_in*) &addr;
        if( DSS_ERROR == dnsi_get_server_addr
                                 (
                                  (dns_server_type_e_type)pquery->server_index,
                                  pquery->sock_index,
                                  AF_INET,
                                  &paddr->sin_addr,
                                  sizeof( paddr->sin_addr )
                                 )
          )
        {
          pquery->error = DS_ENORECOVERY;
          pquery->state = DNS_QUERY_VALID_STATE;
          return;
        }
        paddr->sin_family      = AF_INET;
        paddr->sin_port        = dss_htons( DNS_LOOKUP_PORT );
      }

      /*---------------------------------------------------------------------
        Send query message, start timer and change to wait substate.
      ---------------------------------------------------------------------*/
      retval = dnsi_sock_write( pquery->sock_index,
                                &pquery->dsm_req,
                                (struct sockaddr*) paddr,
                                sizeof(addr),
                                &pquery->error
                              );

      if( DSS_SUCCESS == retval )
      {
        pquery->state = DNS_QUERY_OPEN_WAIT_SUBSTATE;
        ps_timer_start( pquery->pstimer, DNS_TIMEOUT_INTERVAL );
      }
      else
      {
        if( DS_EWOULDBLOCK != pquery->error )
        {
          pquery->state = DNS_QUERY_VALID_STATE;
        }
      }
    }
    else
    {
      pquery->state = DNS_QUERY_VALID_STATE;
    }
      
  }

  return;
} /* dnsi_query_send() */


/*===========================================================================

FUNCTION DNSI_QUERY_NOTIFY

DESCRIPTION
  Query layer notify function, called by the lower layer to inform the query
  management layer of events.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_notify
(
  dns_sock_event_e_type  event,        /* The lower layer event            */
  int32                  event_data,   /* Data associated with event       */
  void                  *cb_voidptr    /* Query layer registered cb data   */
)
{
  dns_query_s_type *pquery   = NULL;   /* Query table entry pointer        */
  dsm_item_type    *dsmptr   = NULL;   /* DSM item pointer                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the query table entry for this event.
  -------------------------------------------------------------------------*/
  pquery = &dns_query_tbl[(int)cb_voidptr];

  /*-------------------------------------------------------------------------
    If state is VALID or FREE return.  If state is valid and socket went
    down then free the socket index
  -------------------------------------------------------------------------*/
  if( DNS_QUERY_FREE_STATE == pquery->state || 
      DNS_QUERY_VALID_STATE == pquery->state )
  {
    if( DNS_SOCK_SOCKDOWN_EVENT == event )
    {
      TASKLOCK();
      pquery->sock_index = DSS_ERROR;
      TASKFREE();
    }
    return;
  }

  Q_DELETE( &dns_query_q[pquery->state], &pquery->link );

  /*-------------------------------------------------------------------------
    Process the event.
  -------------------------------------------------------------------------*/
  switch( event )
  {
    case DNS_SOCK_SOCKUP_EVENT:
      /*---------------------------------------------------------------------
        Socket is up, event_data being the socket index.  Change to send
        substate and wait for socket write callback.
      ---------------------------------------------------------------------*/
      if( DNS_QUERY_OPENING_STATE == pquery->state )
      {
        pquery->sock_index = event_data;
        pquery->state      = DNS_QUERY_OPEN_SEND_SUBSTATE;
      }
      else
      {
        /*-------------------------------------------------------------------
          we are hopelessly confused
        -------------------------------------------------------------------*/
        ASSERT(0);
      }
      break;

    case DNS_SOCK_READ_EVENT:
      /*---------------------------------------------------------------------
        Read the message into a dsm item.  Call message parse function to
        parse the message.  event_data is the socket index.  A DSM item is
        allocated by dnsi_sock_read().
      ---------------------------------------------------------------------*/
      if( NULL != (dsmptr = dnsi_sock_read( event_data )) )
      {
        dnsi_query_recv( pquery, &dsmptr );
      }
      break;

    case DNS_SOCK_WRITE_EVENT:
      /*---------------------------------------------------------------------
        Call the query send function.
      ---------------------------------------------------------------------*/
      dnsi_query_send( pquery );
      break;

    case DNS_SOCK_SOCKUP_FAIL_EVENT:
    case DNS_SOCK_SOCKDOWN_EVENT:
      /*---------------------------------------------------------------------
        Callback with DS_ETRYAGAIN set state to valid and clear the cached
        socket index.
      ---------------------------------------------------------------------*/
      pquery->error      = DS_ETRYAGAIN;
      pquery->state      = DNS_QUERY_VALID_STATE;
      pquery->sock_index = DSS_ERROR;
      break;

    default:
      ERR_FATAL( "Unknown socket event", 0, 0, 0 );
      break;
  }

  /*-------------------------------------------------------------------------
    Enqueue the query entry.
  -------------------------------------------------------------------------*/
  q_put( &dns_query_q[pquery->state], &pquery->link );

  switch( pquery->state )
  {
    case DNS_QUERY_VALID_STATE:
      pquery->cb( pquery->ret, pquery->cb_voidptr, pquery->error );
      /*---------------------------------------------------------------------
        If the socket index is DSS_ERROR then free the entry here else start
       	a timer to dealloc the socket.
      ---------------------------------------------------------------------*/
      if( DSS_ERROR == pquery->sock_index )
      {
        ps_timer_free( pquery->pstimer );
        if( NULL != pquery->question.query )
        {
          dnsi_memfree( &(pquery->question.query) );
        }
        memset( pquery, 0, sizeof(dns_query_s_type) );
        q_put( &dns_query_q[pquery->state], &pquery->link );
      }
      else
      {
        ps_timer_start( pquery->pstimer, DNS_TIMEOUT_INTERVAL );
      }
      break;

    case DNS_QUERY_OPEN_SEND_SUBSTATE:
      dnsi_sock_register_event( pquery->sock_index, DS_WRITE_EVENT );
      break;

    case DNS_QUERY_OPEN_WAIT_SUBSTATE:
      dnsi_sock_register_event( pquery->sock_index, DS_READ_EVENT );
      break;

    default:
      break;
  } /* switch */
  
  return;
} /* dnsi_query_notify() */


/*===========================================================================

FUNCTION DNSI_QUERY_TIMEOUT_CB

DESCRIPTION
  Callback function for handling query layer timeout.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_timeout_cb
(
  void *pindex                         /* Index in the query table         */
)
{
  dns_query_s_type *pquery = NULL;     /* Query table entry                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MSG_LOW("dnsi_query_timeout_cb(): called on entry: %d ",(int)pindex, 0, 0);

  /*-------------------------------------------------------------------------
    Acquire the query table entry.
  -------------------------------------------------------------------------*/
  pquery = &dns_query_tbl[(int)pindex];
  Q_DELETE( &dns_query_q[pquery->state], &pquery->link );

  /*-------------------------------------------------------------------------
    Cancel timer and process based on query state.
  -------------------------------------------------------------------------*/
  ps_timer_cancel( pquery->pstimer );

  switch( pquery->state )
  {
    case DNS_QUERY_OPEN_WAIT_SUBSTATE:
      /*---------------------------------------------------------------------
        Change to send substate if more servers available else could not
        resolve.
      ---------------------------------------------------------------------*/
      if( DNS_MAX_SERVERS > ++(pquery->server_index) )
      {
        pquery->state = DNS_QUERY_OPEN_SEND_SUBSTATE;
        q_put( &dns_query_q[pquery->state], &pquery->link );
        dnsi_sock_register_event( pquery->sock_index, DS_WRITE_EVENT );
      }
      else
      {
        pquery->state = DNS_QUERY_VALID_STATE;
        pquery->cb( NULL, pquery->cb_voidptr, DS_ETRYAGAIN ); 
        /*-------------------------------------------------------------------
          Start the timer to dealloc the socket.
        -------------------------------------------------------------------*/
        ps_timer_start( pquery->pstimer, DNS_TIMEOUT_INTERVAL );
        q_put( &dns_query_q[pquery->state], &pquery->link );
      }
      break;

    case DNS_QUERY_VALID_STATE:
      /*---------------------------------------------------------------------
        Timer was started to deallocate the socket.
      ---------------------------------------------------------------------*/
      if( DSS_ERROR != pquery->sock_index )
      {
        dnsi_sock_dealloc( pquery->sock_index );
      }
      pquery->sock_index = DSS_ERROR;

      /*---------------------------------------------------------------------
        Free query table entry.
      ---------------------------------------------------------------------*/
      ps_timer_free( pquery->pstimer );
      if( NULL != pquery->question.query )
      {
        dnsi_memfree( &(pquery->question.query) );
      }
      memset( pquery, 0, sizeof(dns_query_s_type) );
      q_put( &dns_query_q[pquery->state], &pquery->link );
      break;

    default:
      q_put( &dns_query_q[pquery->state], &pquery->link );
      break;
  } /* switch */

  return;
} /* dnsi_query_timeout_cb() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_HDR_PUT

DESCRIPTION
  Add DNS message header to the passed dsm item chain.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_msg_hdr_put
(
  dsm_item_type         **dsmptr,      /* DSM chain pointer                */
  dns_msg_header_s_type  *phdr         /* Filled DNS message header        */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Add the message ID field.
  -------------------------------------------------------------------------*/
  phdr->id = dss_htons( phdr->id ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->id,
                     sizeof(phdr->id),
                     DSM_DS_LARGE_ITEM_POOL
                   );

  /*-------------------------------------------------------------------------
    Add the parameter field.
  -------------------------------------------------------------------------*/
  phdr->parameter = dss_htons( phdr->parameter ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->parameter,
                     sizeof(phdr->parameter),
                     DSM_DS_LARGE_ITEM_POOL
                   );
  
  /*-------------------------------------------------------------------------
    Add the number of questions field.
  -------------------------------------------------------------------------*/
  phdr->num_questions = dss_htons( phdr->num_questions ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->num_questions,
                     sizeof(phdr->num_questions),
                     DSM_DS_LARGE_ITEM_POOL
                   );

  /*-------------------------------------------------------------------------
    Add the number of answers field.
  -------------------------------------------------------------------------*/
  phdr->num_answers = dss_htons( phdr->num_answers ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->num_answers,
                     sizeof(phdr->num_answers),
                     DSM_DS_LARGE_ITEM_POOL
                   );

  /*-------------------------------------------------------------------------
    Add the number of authority records field.
  -------------------------------------------------------------------------*/
  phdr->num_authority = dss_htons( phdr->num_authority ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->num_authority,
                     sizeof(phdr->num_authority),
                     DSM_DS_LARGE_ITEM_POOL
                   );

  /*-------------------------------------------------------------------------
    Add the number of additional records field.
  -------------------------------------------------------------------------*/
  phdr->num_additional = dss_htons( phdr->num_additional ); 
  phdr->id = dss_htons( phdr->id ); 
  dsm_pushdown_tail( dsmptr,
                     &phdr->num_additional,
                     sizeof(phdr->num_additional),
                     DSM_DS_LARGE_ITEM_POOL
                   );

  return;
} /* dnsi_query_msg_hdr_put() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_HDR_GET

DESCRIPTION
  Acquire the DNS message header from the dsm item.

DEPENDENCIES
  None.

RETURN VALUE
  offset into the dsm item

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_query_msg_hdr_get
(
  dsm_item_type         *dsmptr,       /* Pointer to DSM item chain        */
  dns_msg_header_s_type *phdr          /* DNS message header structure     */
)
{
  int32 offset = 0;                    /* Offset into the DSM item         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Extract the message ID field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->id,
                         sizeof(phdr->id)
                       );
  phdr->id = dss_ntohs(phdr->id);

  /*-------------------------------------------------------------------------
    Extract the parameter field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->parameter,
                         sizeof(phdr->parameter)
                       );
  phdr->parameter = dss_ntohs(phdr->parameter);

  /*-------------------------------------------------------------------------
    Extract the number of questions field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->num_questions,
                         sizeof(phdr->num_questions)
                       );
  phdr->num_questions = dss_ntohs(phdr->num_questions);

  /*-------------------------------------------------------------------------
    Extract the number of resource records field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->num_answers,
                         sizeof(phdr->num_answers)
                       );
  phdr->num_answers = dss_ntohs(phdr->num_answers);

  /*-------------------------------------------------------------------------
    Extract the number of authority sections field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->num_authority,
                         sizeof(phdr->num_authority)
                       );
  phdr->num_authority = dss_ntohs(phdr->num_authority);

  /*-------------------------------------------------------------------------
    Extract the number of additional sections field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &phdr->num_additional,
                         sizeof(phdr->num_additional)
                       );
  phdr->num_additional = dss_ntohs( phdr->num_additional );
  
  return offset;
} /* dnsi_query_msg_hdr_get() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_QUESTION_PUT

DESCRIPTION
  Add the question passed to the dsm item chain.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  If successful.
  DSS_ERROR    If failed.

SIDE EFFECTS
  None.

===========================================================================*/
static int16 dnsi_query_msg_question_put
(
  dsm_item_type           **dsmptr,    /* DSM chain to which question added*/
  dns_msg_question_s_type  *pquestion  /* Domain name question             */
)
{
  char *plabel_prev = pquestion->query;/* Pointer to beginning of label    */
  char *plabel      = pquestion->query;/* Pointer to label                 */
  uint8 label_len   = 0;               /* Length of label                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(pquestion->type)
  {
    case TYPE_A:
    case TYPE_AAAA:
    case TYPE_PTR:
      /*---------------------------------------------------------------------
        The question is in the form of a sequence of labels:
       
        1 byte            : label length
        label length bytes: label
       
        where each label is the portion of the domain name seperated by the
        '.' charachter
      ---------------------------------------------------------------------*/
      do
      {
        if( '.' == *plabel || 0 == *plabel )
        {
          dsm_pushdown_tail( dsmptr,
                             &label_len,
                             sizeof(uint8),
                             DSM_DS_LARGE_ITEM_POOL
                           );
          dsm_pushdown_tail( dsmptr,
                             plabel_prev,
                             label_len,
                             DSM_DS_LARGE_ITEM_POOL
                           );
          label_len   = 0;
          plabel_prev = (*plabel != 0) ? ++plabel : plabel;
	}
        else
        {
          label_len++;
          plabel++;
        }
      } while( 0 != *plabel_prev );

      /*---------------------------------------------------------------------
        Add final terminating zero
      ---------------------------------------------------------------------*/
      label_len = 0;
      dsm_pushdown_tail( dsmptr,
                         &label_len,
                         sizeof(uint8),
                         DSM_DS_LARGE_ITEM_POOL
                       );
      break;

    default:
      ERR( "Question type not supported", 0, 0, 0 );
      return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Add query type and query class.
  -------------------------------------------------------------------------*/
  pquestion->type = dss_htons( pquestion->type );
  dsm_pushdown_tail( dsmptr,
                     &pquestion->type,
                     sizeof(pquestion->type),
                     DSM_DS_LARGE_ITEM_POOL
                   );
  pquestion->type = dss_ntohs( pquestion->type );

  pquestion->class = dss_htons( pquestion->class );
  dsm_pushdown_tail( dsmptr,
                     &pquestion->class,
                     sizeof(pquestion->class),
                     DSM_DS_LARGE_ITEM_POOL
                   );
  pquestion->class = dss_ntohs( pquestion->class );

  return DSS_SUCCESS;
} /* dnsi_query_msg_question_put() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_QUESTION_GET

DESCRIPTION
  Act on the question which starts at offset passed in the DSM chain.  Return
  the offset in the DSM item after the question field.

DEPENDENCIES
  None.

RETURN VALUE
  offset into the dsm item after the question

SIDE EFFECTS
  None.

===========================================================================*/
static int32 dnsi_query_msg_question_get
(
  dns_query_s_type        *pquery,            /* Query table entry                */
  dsm_item_type           *dsmptr,            /* DSM item with the message        */
  int32                    offset             /* Offset into the DSM item         */
)
{
  uint32                   buflen     = 0;    /* Length of buffer             */
  dns_msg_question_s_type  question;   /* Question section buffer          */
  uint32                   query_server_str_len; /* Length of server string */
  uint32                   response_server_str_len;/* Length of server string*/
  int                      name_mismatch = 0; /* indicate name mismatch */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get the query domain name.  
  -------------------------------------------------------------------------*/
  dnsi_query_domain_name_expand( dsmptr, offset, NULL, 0, &buflen );
  if( NULL == (question.query = dnsi_memalloc( buflen )) )
  {
    ERR("Failed to allocate question query", 0, 0, 0);
    pquery->state = DNS_QUERY_VALID_STATE;
    pquery->error = DS_ENOMEM;
    offset = DSS_ERROR;
    goto bail;
  }
  memset (question.query, 0, buflen);
  if( DSS_ERROR == (offset = dnsi_query_domain_name_expand( dsmptr,
                                                            offset,
                                                            question.query,
                                                            buflen,
                                                            NULL )))
     
  {
    pquery->state = DNS_QUERY_VALID_STATE;
    pquery->error = DS_ENORECOVERY;
    offset = DSS_ERROR;
    goto bail;
  }
  
  /*Handle the case where server add dot (.) at the end of the name*/
  query_server_str_len = std_strlen(pquery->question.query);
  response_server_str_len = std_strlen(question.query);
  if (( response_server_str_len == query_server_str_len+1 )&&
      ( response_server_str_len != 0 )&&
      ( question.query[response_server_str_len-1] == '.' ))
  {
    /*Compare up to (excluding) the '.' at the end.*/
    name_mismatch = std_strnicmp(question.query, 
                                 pquery->question.query,
                                 query_server_str_len);
    
  }
  else
  {
    name_mismatch = std_stricmp(question.query, 
                                pquery->question.query);
  }
  if ( 0 != name_mismatch )
  {
    ERR( "Ignoring DNS response due to query mismatch %d %d", 
          question.query, 
          pquery->question.query,
          0 );
    /*Don't change state in order to continue wait for a valid response*/ 
    dnsi_sock_register_event( pquery->sock_index, DS_READ_EVENT );
    offset = DSS_ERROR;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Get the query type and class.  
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &question.type,
                         sizeof(question.type)
                       );
  question.type = dss_ntohs( question.type );

  if ( question.type != pquery->question.type )
  {
    ERR( "Ignoring DNS response due to question type mismatch %d %d", 
          question.type, pquery->question.type, 0 );
    dnsi_sock_register_event( pquery->sock_index, DS_READ_EVENT );
    offset = DSS_ERROR;
    goto bail;
 }

  offset += dsm_extract( dsmptr,
                         offset,
                         &question.class,
                         sizeof(question.class)
                       );
  question.class = dss_ntohs( question.class );
  if ( question.class != pquery->question.class )
  {
    ERR( "Ignoring DNS response due to class mismatch %d %d", 
          question.class, pquery->question.class, 0 );
    dnsi_sock_register_event( pquery->sock_index, DS_READ_EVENT );
    offset = DSS_ERROR;
    goto bail;
  }

bail:
  if ( NULL != question.query)
  {
    dnsi_memfree(& question.query );
  }
  return offset;
} /* dnsi_query_msg_question_get() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_ANSWER_GET

DESCRIPTION
  Parses one answer record of the DNS message.

DEPENDENCIES
  None.

RETURN VALUE
  offset into the dsm buffer
  DSS_ERROR if an error occured

SIDE EFFECTS
  Allocates memory for the owner and resource record data fields of an RR.
  In case of error the query state is changed to DNS_QUERY_VALID_STATE
  and sets the error code.

===========================================================================*/
static int16 dnsi_query_msg_answer_get
(
  dns_query_s_type      *pquery,       /* Query table entry                */
  dsm_item_type         *dsmptr,       /* DSM item containing DNS message  */
  int16                  offset,       /* Offset into the DSM item         */
  dns_msg_rr_s_type     *prr           /* Resource record memory           */
)
{
  uint32                buflen     = 0;    /* Length of buffer             */
  int                   retval     = DSS_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the domain name for which this record is the answer.
  -------------------------------------------------------------------------*/
  dnsi_query_domain_name_expand( dsmptr, offset, NULL, 0, &buflen );
  if( NULL == (prr->owner = dnsi_memalloc( buflen )) )
  {
    pquery->state = DNS_QUERY_VALID_STATE;
    pquery->error = DS_ENOMEM;
    return DSS_ERROR;
  }

  if( DSS_ERROR == (offset = dnsi_query_domain_name_expand( dsmptr,
                                                            offset,
                                                            prr->owner,
                                                            buflen,
                                                            NULL )
                   )
    )
  {
    pquery->state = DNS_QUERY_VALID_STATE;
    pquery->error = DS_ENORECOVERY;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Get the two byte type.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr, offset, &prr->type,  sizeof(prr->type) );
  prr->type = dss_ntohs( prr->type );

  /*-------------------------------------------------------------------------
    Get the two byte class
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr, offset, &prr->class, sizeof(prr->class) );
  prr->class = dss_ntohs( prr->class );

  /*-------------------------------------------------------------------------
    Get the four byte TTL field
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr, offset, &prr->ttl,   sizeof(prr->ttl) );
  prr->ttl = dss_ntohl( prr->ttl );

  /*-------------------------------------------------------------------------
    Get the two byte record length field.
  -------------------------------------------------------------------------*/
  offset += dsm_extract( dsmptr,
                         offset,
                         &prr->rr_length,
                         sizeof(prr->rr_length)
                       );
  prr->rr_length = dss_ntohs( prr->rr_length );

  /*-------------------------------------------------------------------------
    Process answer based on record type.
  -------------------------------------------------------------------------*/
  switch( prr->type )
  {
    case TYPE_A:
    case TYPE_AAAA:
      /*---------------------------------------------------------------------
        Allocate memory for IP address, copy the address.
      ---------------------------------------------------------------------*/
      if( NULL == (prr->rr = dnsi_memalloc( prr->rr_length)) )
      {
        pquery->state = DNS_QUERY_VALID_STATE;
        pquery->error = DS_ENOMEM;
        retval = DSS_ERROR;
      }
      else
      {
        offset += dsm_extract( dsmptr, offset, prr->rr, prr->rr_length );
      }
      break;

    case TYPE_PTR:
      /*---------------------------------------------------------------------
        The answer is the domain name.  Find size of RR data taking into
        account compressed names.  Adjust RR length accordingly
      ---------------------------------------------------------------------*/
      dnsi_query_domain_name_expand( dsmptr, offset, NULL, 0, &buflen );
      prr->rr_length = buflen;

      if( NULL == (prr->rr = dnsi_memalloc( buflen )) )
      {
        pquery->state = DNS_QUERY_VALID_STATE;
        pquery->error = DS_ENOMEM;
        retval = DSS_ERROR;
      }
      else
      {
        /*-------------------------------------------------------------------
          Get the mapping.
        -------------------------------------------------------------------*/
        if( DSS_ERROR == (offset = dnsi_query_domain_name_expand( dsmptr,
                                                                  offset,
                                                                  prr->rr,
                                                                  buflen,
                                                                  NULL ))
          )
        {
          pquery->state = DNS_QUERY_VALID_STATE;
          pquery->error = DS_ENORECOVERY;
          retval = DSS_ERROR;
        }
      }
      break;

    case TYPE_CNAME:
      /*---------------------------------------------------------------------
        Find length of domain name, allocate memory and expand the name.
      ---------------------------------------------------------------------*/
      dnsi_query_domain_name_expand( dsmptr, offset, NULL, 0, &buflen );
      prr->rr_length = buflen;

      if( NULL == (prr->rr = dnsi_memalloc( buflen )) )
      {
        pquery->state = DNS_QUERY_VALID_STATE;
        pquery->error = DS_ENOMEM;
        retval = DSS_ERROR;
        break;
      }

      /*---------------------------------------------------------------------
        Get the mapping.
      ---------------------------------------------------------------------*/
      if( DSS_ERROR == (offset = dnsi_query_domain_name_expand( dsmptr,
                                                                offset,
                                                                prr->rr,
                                                                buflen,
                                                                NULL ))
        )
      {
        pquery->state = DNS_QUERY_VALID_STATE;
        pquery->error = DS_ENORECOVERY;
        retval = DSS_ERROR;
      }
      break;

    case TYPE_MB:
    case TYPE_MG:
    case TYPE_MR:
    case TYPE_NS:
    case TYPE_HINFO:
    case TYPE_MX:
    case TYPE_SOA:
    case TYPE_TXT:
    default:
      offset += prr->rr_length;
      break;
  } /* switch */

  return ( DSS_ERROR == retval ) ? retval : offset;
} /* dnsi_query_msg_answer_get() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_REQ_BUILD

DESCRIPTION
  Makes a DNS look-up packet.

DEPENDENCIES
  Query entry is already dequeued.  The entry should not be re-queued here.

RETURN VALUE
  DSS_SUCCESS  If formed a message.
  DSS_ERROR    If failed to form a dns query.  The error code is placed
               in the error_code parameter.

SIDE EFFECTS
  None.

===========================================================================*/
static int16 dnsi_query_msg_req_build
(
  dns_query_s_type *pquery,            /* Query table entry                */
  uint16            parameter,         /* Parameter field in DNS message   */
  int32            *error_code         /* The error code of any error      */
)
{
  dsm_item_type           *dsmptr = pquery->dsm_req; /* DSM item for msg   */
  dns_msg_header_s_type    msghdr;                   /* DNS msg header     */
  int16                    retval = DSS_SUCCESS;     /* Return value       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
    Asks for a large dsm item to buffer the prepared DNS lookup query
    packet.  Preallocate 4 bytes in the beginning for TCP.
  ---------------------------------------------------------------------*/
  if ( NULL == dsmptr && 
       NULL == (dsmptr = dsm_offset_new_buffer( DSM_DS_LARGE_ITEM_POOL, 4 ))
     )
  {
    MSG_HIGH("Could not acquire a dsm item",0,0,0);
    *error_code = DS_ENOMEM;
    retval      = DSS_ERROR;
  }
  else
  {
    pquery->dsm_req = dsmptr;
    /*-------------------------------------------------------------------------
      Fill in DNS message header.
    -------------------------------------------------------------------------*/
    msghdr.id             = pquery->id;
    msghdr.parameter      = parameter;
    msghdr.num_questions  = 1;
    msghdr.num_answers    = 0;
    msghdr.num_authority  = 0;
    msghdr.num_additional = 0;
    dnsi_query_msg_hdr_put( &dsmptr, &msghdr );

    /*-------------------------------------------------------------------------
      Make the DNS lookup query -- Domain name in the query format.
    -------------------------------------------------------------------------*/
    retval = dnsi_query_msg_question_put( &dsmptr, &pquery->question );

    if( retval == DSS_ERROR )
    { 
      dsm_free_packet( &pquery->dsm_req );
    }
  }

  return retval;
} /* dnsi_query_msg_req_build() */


/*===========================================================================

FUNCTION DNSI_QUERY_MSG_REP_PARSE

DESCRIPTION
  Parse the DNS lookup response message received from the DNS server.

DEPENDENCIES
  query entry is dequeued before calling the function.

RETURN VALUE
  DSS_SUCCESS  If successfully parsed.
  DSS_ERROR    If error occured.

SIDE EFFECTS
  The answer RRs may be cached.

===========================================================================*/
static int16 dnsi_query_msg_rep_parse
(
  dns_query_s_type *pquery,               /* Pointer to query entry        */
  dsm_item_type    *dsmptr                /* DSM item carrying DNS message */
)
{
  int16                    offset    = 0; /* Offset in the dsm chain       */
  dns_msg_header_s_type    msghdr;        /* Buffer for DNS header         */
  dns_cache_entry_s_type *pentry;
  q_type                  answer_q;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == dsmptr || DNS_MSG_HEADER_SIZE  > dsm_length_packet( dsmptr ) )
  {
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Get the message header.  Verify it is a reply and check return code:
      return code = parameter & 0xf
      0 for no error
      1 for format error in query
      2 for server failure
      3 for name doesn't exist
  -------------------------------------------------------------------------*/
  offset = dnsi_query_msg_hdr_get(dsmptr, &msghdr);
  if ( msghdr.id != pquery->id )
  {  
    /* Mismatch of ID wait for correct answer */
    ERR("Mismatch of Query ID wait for correct answer", 0, 0, 0);
    dnsi_sock_register_event( pquery->sock_index, DS_READ_EVENT );
    return DSS_ERROR;
  }

  if(0 == (msghdr.parameter & DNS_MSG_PARAM_OPERATION) )
  {
    return DSS_ERROR;
  }

  switch( msghdr.parameter & DNS_MSG_PARAM_RESP_TYPE ) 
  {
    case 0:  /* No error */
      pquery->error = DSS_SUCCESS;
      break;

    case 1:  /* Format error in query */
      pquery->state = DNS_QUERY_VALID_STATE;
      pquery->error = DS_NAMEERR;
      return DSS_ERROR;

    case 3:  /* Host not found */
      pquery->error = DS_EHOSTNOTFOUND;
      pquery->state = DNS_QUERY_VALID_STATE;
      return DSS_ERROR;

    case 2:   /* Server failure */
    default:
      pquery->state = ( DNS_MAX_SERVERS > ++pquery->server_index ) ? 
                        DNS_QUERY_OPEN_SEND_SUBSTATE               :
                        DNS_QUERY_VALID_STATE;
      if( DNS_QUERY_VALID_STATE == pquery->state )
      {
        pquery->error = DS_ENORECOVERY;
      }	
      return DSS_ERROR;
  } /* switch */
  /*-------------------------------------------------------------------------
    Check the question section. 
  -------------------------------------------------------------------------*/
  for( ; msghdr.num_questions > 0; msghdr.num_questions-- )
  {
    offset = dnsi_query_msg_question_get( pquery, dsmptr, offset );
    if ( DSS_ERROR == offset )
    {
      ERR( "Failed to get question from DNS response", 0, 0, 0);
      return DSS_ERROR;
    }
  }

  /*-------------------------------------------------------------------------
    Parse the answer portion and acquire the mappings.
  -------------------------------------------------------------------------*/
  q_init( &answer_q );

  for( ; 0 < msghdr.num_answers && DNS_QUERY_VALID_STATE != pquery->state;
         msghdr.num_answers-- )
  {
    if( NULL == (pentry = dnsi_memalloc( sizeof(dns_cache_entry_s_type) )) )
    {
      pquery->state = DNS_QUERY_VALID_STATE;
      pquery->error = DS_ENOMEM;
    }
    else
    {
      offset = dnsi_query_msg_answer_get( pquery,
                                          dsmptr,
                                          offset,
                                          &pentry->rr);
      q_put( &answer_q, &pentry->link );
    }
  } /* for */

  /*-------------------------------------------------------------------------
    If query state is VALID then an error occured.  Otherwise fill the data
    structure to be returned and cache the RRs
  -------------------------------------------------------------------------*/
  if( DNS_QUERY_VALID_STATE != pquery->state )
  {
    switch( pquery->ret_type )
    {
      case DNS_DATA_STRUCTURE_DSS_HOSTENT:
        dnsi_query_hostent_alloc( (struct dss_hostent**)&pquery->ret, 
                                  &pquery->question,
                                  &answer_q,
                                  pquery->af,
                                  (int16*)&pquery->error );
        break;

      default:
        ASSERT( 0 );
        break;
    } /* switch */

    /*-----------------------------------------------------------------------
      If a cache is present then try adding the RRs to the cache.
    -----------------------------------------------------------------------*/
    dnsi_cache_add( &answer_q );
  }

  /*-------------------------------------------------------------------------
    Free memory associated with RRs
  -------------------------------------------------------------------------*/
  while( NULL != (pentry = q_get( &answer_q )) )
  {
    dnsi_cache_entry_free( &pentry );
  } /* while */

  pquery->state = DNS_QUERY_VALID_STATE;

  return DSS_SUCCESS;
} /* dnsi_query_msg_rep_parse() */


/*===========================================================================

FUNCTION DNSI_QUERY_CANONICAL

DESCRIPTION
  Verify that the query passed in is well-formed.  Form the canonical query.
  If the query can be answered from the information available then fill in
  the appropriate structure passing it back as a non-NULL ret.

  At present only handles IPv4 addresses, TYPE_A and TYPE_PTR queries.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS  If no errors.
  DSS_ERROR    Otherwise.  The error code is placed in dss_errno.

SIDE EFFECTS
  Allocated memory for the formed query.

===========================================================================*/
static int16 dnsi_query_canonical
(
  dns_msg_question_s_type *asked,      /* Query asked                      */
  dns_msg_question_s_type *formed,     /* Query in canonical form          */
  int32                    af,         /* Address family                   */
  dns_query_ret_e_type     ret_type,   /* Type of return value in ret      */
  void                   **ret,        /* Return value if answer found     */
  int16                   *dss_errno   /* The error value in case of error */
)
{
  int16               retval = DSS_SUCCESS; /* Return value                */
  int                 buflen = 0;           /* Question buffer length      */
  struct in_addr      ip_addr;              /* IPv4 address                */
  struct dss_hostent *phostent = NULL;      /* Ptr to struct dss_hostent   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  formed->class = asked->class;
  formed->type  = asked->type;

  /*-------------------------------------------------------------------------
    Process based on the type of query.  Find the memroy to be allocated for
    the domain name question.  Form the query in canonical form.
  -------------------------------------------------------------------------*/
  switch( asked->type )
  {
    case TYPE_A:
    case TYPE_AAAA:
      buflen = strlen( asked->query ) + 1;
      if( NULL == (formed->query = dnsi_memalloc( buflen )) )
      {
        *dss_errno = DS_ENOMEM;
        retval     = DSS_ERROR;
        break;
      }
      else
      {
        /*-------------------------------------------------------------------
          Copy the domain name.
        -------------------------------------------------------------------*/
        memcpy( formed->query, asked->query, buflen );

        /*-------------------------------------------------------------------
          Check if a mapping can be returned based on the passed information.
          This case occurs if the asked domain name is an IP address in
          dotted quad format.  Set dss_errno to something other then 
          DS_EWOULDBLOCK.  Form a data structure to be returned of the
          correct type.
        -------------------------------------------------------------------*/
        if( AF_INET == af &&
            DSS_SUCCESS == dss_inet_aton( formed->query, &ip_addr ) )
        {
          /*-----------------------------------------------------------------
            Fill in the correct data type to be returned.
          -----------------------------------------------------------------*/
          switch( ret_type )
          {
            case DNS_DATA_STRUCTURE_DSS_HOSTENT:
              /*-------------------------------------------------------------
                Allocate memory for struct dss_hostent, a single NULL alias,
                two entries for the address list including the NULL and
                memory for the address.  In case of failure return DS_ENOMEM
              -------------------------------------------------------------*/
              phostent = dnsi_memalloc( sizeof( struct dss_hostent ) );
              if( NULL == phostent )
              {
                *dss_errno = DS_ENOMEM;
                break;
              }

              phostent->h_aliases = dnsi_memalloc( sizeof(char *) );
              if( NULL == phostent->h_aliases )
              {
                *dss_errno = DS_ENOMEM;
                dnsi_query_hostent_free( &phostent );
                break;
              }
              phostent->h_aliases[0] = NULL;

              phostent->h_addr_list = dnsi_memalloc( 2 * sizeof(char *) );
              if( NULL == phostent->h_addr_list )
              {
                *dss_errno = DS_ENOMEM;
                dnsi_query_hostent_free( &phostent );
                break;
              }

              phostent->h_addr_list[0] =
                                   dnsi_memalloc( sizeof( struct in_addr ) );
              if( NULL == phostent->h_addr_list[0] )
              {
                *dss_errno = DS_ENOMEM;
                dnsi_query_hostent_free( &phostent );
                break;
              }
              memcpy( phostent->h_addr_list[0],
                      &ip_addr,
                      sizeof( struct in_addr ) );
              phostent->h_addr_list[1] = NULL;

              phostent->h_name = dnsi_memalloc( strlen(asked->query) + 1 );
              if( NULL == phostent->h_name )
              {
                *dss_errno = DS_ENOMEM;
                dnsi_query_hostent_free( &phostent );
                break;
              }
              memcpy( phostent->h_name,
                      asked->query,
                      strlen( asked->query ) + 1 );
              phostent->h_addrtype = af;
              phostent->h_length   = sizeof( struct in_addr );
              *ret = phostent;
              break;

            default:
              ASSERT( 0 );
              break;
          } /* switch */
	}
      }
      break;

    case TYPE_PTR:
      buflen = DNS_INADDR_STR_LEN + strlen( DNS_INADDR_ARPA_STR );

      if( NULL == (formed->query = dnsi_memalloc( buflen )) )
      {
        *dss_errno = DS_ENOMEM;
        retval     = DSS_ERROR;
      }
      else
      {
        ASSERT( AF_INET == af );
        memcpy( &ip_addr, asked->query, sizeof(struct in_addr) );

        /*-------------------------------------------------------------------
          A pointer query is of the form d.c.b.a.in-addr.arpa, where
          a.b.c.d is the IP addres.
        -------------------------------------------------------------------*/
        ip_addr.s_addr   = ((ip_addr.s_addr & 0xff)       << 24) |
                           ((ip_addr.s_addr & 0xff00)     << 8)  |
                           ((ip_addr.s_addr & 0xff0000)   >> 8)  |
                           ((ip_addr.s_addr & 0xff000000) >> 24);

        if( DSS_ERROR == dss_inet_ntoa( ip_addr,
                                        (uint8 *)formed->query,
                                        DNS_INADDR_STR_LEN )
          )
        {
          *dss_errno = DS_NAMEERR;
          retval     = DSS_ERROR;
        }
        else
        {
          std_strlcpy( &(formed->query[strlen(formed->query)]),
                       DNS_INADDR_ARPA_STR, 
                       buflen-strlen(formed->query));
        }
      }
      break;

    default:
      ASSERT( 0 );
      *dss_errno = DS_EFAULT;
      retval     = DSS_ERROR;
      break;
  } /* switch */

  return retval;
} /* dnsi_query_canonical() */


/*===========================================================================

FUNCTION DNSI_QUERY_DOMAIN_NAME_EXPAND

DESCRIPTION
  Passed a DSM chain containing the DNS message and the offset where the
  domain name starts.  Expands the domain name copying into the passed
  buffer.  If the buffer parameter is passed as NULL simply returns the
  offset into the DSM chain after the domain name.  It also returns the 
  actual length of the name in the namelen parameter.

DEPENDENCIES
  None.

RETURN VALUE
  offset into the dsm item after the domain name
  DSS_ERROR on error
  

SIDE EFFECTS
  None.

===========================================================================*/
static int16 dnsi_query_domain_name_expand
(
  dsm_item_type    *dsmptr,            /* DSM item with the message        */
  uint16            offset,            /* Offset into the DNS item         */
  char             *buf,               /* Buffer for storing the DNS name  */
  int32             buflen,            /* Length of buffer                 */
  uint32           *namelen            /* Length of name                   */
)
{
  uint16  label_len = 0;               /* Length of domain name label      */
  int16   retval    = 0;               /* Return value                     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    If namelen is non-NULL set it to zero.
  -------------------------------------------------------------------------*/
  if( NULL != namelen )
  {
    *namelen = 0;
  }

  /*-------------------------------------------------------------------------
    Loop over the domain name till have a valid name.
  -------------------------------------------------------------------------*/
  for(;;)
  {
    /*-----------------------------------------------------------------------
      Get the label length; a label length of zero indicates end of name
    -----------------------------------------------------------------------*/
    dsm_peek_byte( dsmptr, offset, (uint8*)&label_len );

    if( 0 == label_len )
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Check for pointer.  A label length is a pointer if the most significant
      two bits are one.  The remaining fourteen bits are the pointer.
    -----------------------------------------------------------------------*/
    if( 0xc0 == (label_len & 0xc0) )
    {
      /*---------------------------------------------------------------------
        Save offset past the domain name.  We need to save offset at this
        point because we are taking the label pointer.
      ---------------------------------------------------------------------*/
      if( 0 == retval )
      {
        retval = offset + sizeof(uint16);
      }

      dsm_extract( dsmptr, offset, &offset, sizeof(uint16) );
      offset = dss_ntohs( offset );
      offset &= ~0xc000;
    }
    else
    {
      offset++;
      /*---------------------------------------------------------------------
        If buffer is non NULL copy the label to it after checking for space.
        The number two is for the terminating '.' and 0.
      ---------------------------------------------------------------------*/
      if( NULL != buf )
      {
        if( 0 > (buflen - label_len - 2) )
        {
          retval = DSS_ERROR;
          break;
        }
        dsm_extract( dsmptr, offset, buf, label_len );
        buf    += label_len;
        buflen -= label_len + 1;
        *buf++  = '.';
      }
      offset += label_len;

      /*---------------------------------------------------------------------
        If namelen is non NULL add the label length to the total name length.
        The second addition of 1 is for the '.'
      ---------------------------------------------------------------------*/
      if( NULL != namelen )
      {
        *namelen += label_len;
        (*namelen)++;
      }
    }
  } /* for */

  /*-------------------------------------------------------------------------
    Add the terminating 0. 
  -------------------------------------------------------------------------*/
  if( NULL != buf && 0 < buflen )
  {
    *buf  = 0;
  }

  /*-------------------------------------------------------------------------
    Add the terminating 0 to the namelen. 
  -------------------------------------------------------------------------*/
  if( NULL != namelen )
  {
    (*namelen)++;
  }

  /*-------------------------------------------------------------------------
    Obtain offset past the domain name if it isn't already set.
  -------------------------------------------------------------------------*/
  if( 0 == retval )
  {
    retval = offset + sizeof(uint8);
  }

  return retval;
} /* dnsi_query_domain_name_expand() */


/*===========================================================================

FUNCTION DNSI_QUERY_START

DESCRIPTION
  Queues a PS_DNS_START_CMD to the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dnsi_query_start
(
  void *query_index                    /* Index in the query table         */
)
{
#ifndef FEATURE_DATA_MM
  ps_cmd_type cmd_item;
#endif /* ifndef FEATURE_DATA_MM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Queue a command to PS task.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ps_send_cmd( PS_DNS_START_CMD, query_index );
#else
  cmd_item.cmd_name         = PS_DNS_START_CMD;
  cmd_item.cmd_arg.dns_data = query_index;
  ps_cmd( &cmd_item );
#endif /* ifdef FEATURE_DATA_MM */

  return;
} /* dnsi_query_start() */


/*===========================================================================

FUNCTION DNSI_QUERY_START_CMD_HANDLER

DESCRIPTION
  Handles PS_DNS_START_CMD.  Calls dnsi_sock_alloc() to acquire a socket.

DEPENDENCIES
  Query is in the opening state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DATA_MM
static
#endif /* ifdef FEATURE_DATA_MM */
void dnsi_query_start_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *query_index        /* Application id                   */
)
{
  dns_query_s_type *pquery = NULL;     /* Query table entry                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( PS_DNS_START_CMD == cmd );
  /*-------------------------------------------------------------------------
    Acquire the query table entry and call dnsi_sock_alloc().
  -------------------------------------------------------------------------*/
  pquery = &dns_query_tbl[(int)query_index];

  ASSERT( DNS_QUERY_OPENING_STATE == pquery->state );

  if( DSS_ERROR == dnsi_sock_alloc( AF_INET,
                                    pquery->sock_type,
                                    (dns_notify_f_type) dnsi_query_notify,
                                    query_index
                                  )
    )
  {
    /*-----------------------------------------------------------------------
      Failed to acquire a socket, return DS_ETRYAGAIN as an error.
    -----------------------------------------------------------------------*/
    pquery->cb( NULL, pquery->cb_voidptr, DS_ETRYAGAIN );

    Q_DELETE( &dns_query_q[pquery->state], &pquery->link );

    /*-----------------------------------------------------------------------
      Free query table entry.
    -----------------------------------------------------------------------*/
    ps_timer_free( pquery->pstimer );
    if( NULL != pquery->question.query )
    {
      dnsi_memfree( &(pquery->question.query) );
    }
    memset( pquery, 0, sizeof(dns_query_s_type) );
    q_put( &dns_query_q[DNS_QUERY_FREE_STATE], &pquery->link );
  }

  return;
} /* dnsi_query_start_cmd_handler() */


/*===========================================================================

FUNCTION DNSI_QUERY_STOP_CMD_HANDLER

DESCRIPTION
  Handles the PS_DNS_STOP_CMD, frees the query table entry and its associated
  data structures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef FEATURE_DATA_MM
void dnsi_query_stop_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *query_index        /* Application id                   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return;
} /* dnsi_query_stop_cmd_handler() */
#endif /* ifndef FEATURE_DATA_MM */


/*===========================================================================

FUNCTION DNSI_QUERY_HOSTENT_ALLOC

DESCRIPTION
  This function dynamically allocates memory for a struct dss_hostent
  and uses the information passed in, including the queue of resource
  records, to fill in the allocated dss_hostent which is then passed
  back to the application and must be freed by a call to dss_freehostent().

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR   in case of an error, dss_errno is appropriately set.
  DSS_SUCCESS if successful.  The hostent is returned in phostent and
              the passed RRs are in the rr_q.
  
SIDE EFFECTS
  Allocates memory for the hostent and member elements.  This memory must
  be freed by a call to dss_freehostent().

===========================================================================*/
static int dnsi_query_hostent_alloc
(
  struct dss_hostent      **phostent,  /* dss_hostent to be allocated      */
  dns_msg_question_s_type  *question,  /* The DNS query information        */
  q_type                   *rr_q,      /* Queue containing answer RRs      */
  int32                     af,        /* Address family                   */
  int16                    *dss_errno  /* Error code                       */
)
{
  dns_cache_entry_s_type  *pentry;
  char                   **pchar;
  int                      num_aliases = 0;
  int                      num_address = 0;
  int                      retval      = DSS_SUCCESS;
  char                     buffer[DNS_INADDR_STR_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate memory for the hostent.  In case of failure return with
    DSS_ERROR and the error code set to DS_ENOMEM.
  -------------------------------------------------------------------------*/
  if( NULL == (*phostent = dnsi_memalloc( sizeof(struct dss_hostent) )) )
  {
    *dss_errno = DS_ENOMEM;
    return DSS_ERROR;
  }

  memset( *phostent, 0, sizeof( struct dss_hostent ) );

  /*-------------------------------------------------------------------------
    Loop through the RR queue finding space needed for the alias and
    address array
  -------------------------------------------------------------------------*/
  pentry = q_check( rr_q );

  while( NULL != pentry )
  {
    switch( pentry->rr.type )
    {
      case TYPE_PTR:
      case TYPE_CNAME:
        num_aliases++;
        break;
      
      case TYPE_A:
      case TYPE_AAAA:
        /*-------------------------------------------------------------------
          A dss_hostent cannot mix addresses.  Pass along only those
          addresses which are of the right type
        -------------------------------------------------------------------*/
        if( (AF_INET == af && TYPE_A == pentry->rr.type)   ||
            (AF_INET6 == af && TYPE_AAAA == pentry->rr.type) )
        {
          num_address++;
        }
        break;

      default:
        break;
    } /* switch */

    pentry = q_next( rr_q, &pentry->link );
  } /* while */

  /*-------------------------------------------------------------------------
    If query was of pointer type then allocate room for the asked address.
  -------------------------------------------------------------------------*/
  if( TYPE_PTR == question->type )
  {
    num_address++;
  }

  /*-------------------------------------------------------------------------
    If query was of type A or AAAA type and no addresses are returned then
    return an error.
  -------------------------------------------------------------------------*/
  if( (TYPE_A == question->type || TYPE_AAAA == question->type) &&
      0 == num_address )
  {
    *dss_errno = DS_EHOSTNOTFOUND;
    dnsi_memfree( (char**) phostent );
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Allocate memory for the aliases and address list
  -------------------------------------------------------------------------*/
  (*phostent)->h_aliases =
                        dnsi_memalloc( (num_aliases + 1) * sizeof( char *) );
  (*phostent)->h_addr_list =
                        dnsi_memalloc( (num_address + 1) * sizeof( char *) );

  if( NULL == (*phostent)->h_aliases || NULL == (*phostent)->h_addr_list )
  {
    *dss_errno = DS_ENOMEM;
    dnsi_memfree( (char**) phostent );
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Memset aliases and address array to zero
  -------------------------------------------------------------------------*/
  memset( (*phostent)->h_aliases, 0, sizeof(char*) * (num_aliases + 1) );
  memset( (*phostent)->h_addr_list, 0, sizeof(char*) * (num_address + 1 ) );
  num_aliases = 0;
  num_address = 0;

  /*-------------------------------------------------------------------------
    For the case of a pointer query, fill in the asked address.  The query
    is in canonical form i.e. "address.in-addr.arpa"
  -------------------------------------------------------------------------*/
  if( TYPE_PTR == question->type )
  {
    uint32 copy_len;
    copy_len = strlen( question->query ) - strlen( DNS_INADDR_ARPA_STR ) + 1;
    if ( DNS_INADDR_STR_LEN >= copy_len )
    {
      memcpy( buffer,
              question->query,
              copy_len
             );
    }
    else
    {
      /* Not enough memory in buffer to copy! */
      *dss_errno = DS_ENOMEM;
      dnsi_query_hostent_free( phostent );
      return DSS_ERROR;
    }
	
    (*phostent)->h_addr_list[num_address] =
                    dnsi_memalloc( sizeof(struct in_addr) );
    if( NULL == (*phostent)->h_addr_list[num_address] )
    {
      *dss_errno = DS_ENOMEM;
      dnsi_query_hostent_free( phostent );
      return DSS_ERROR;
    }
    dss_inet_aton( buffer,
                   (struct in_addr *)(*phostent)->h_addr_list[num_address] );
    *(uint32*)(*phostent)->h_addr_list[num_address] =
                dss_htonl( *(uint32*)(*phostent)->h_addr_list[num_address] );
    num_address++;
  }

  /*-------------------------------------------------------------------------
    Fill the addrtype and address length fields.
  -------------------------------------------------------------------------*/
  (*phostent)->h_addrtype = af;
  (*phostent)->h_length   = ( AF_INET == af )           ?
                              sizeof( struct in_addr )  :
                              sizeof( struct in6_addr );

  /*-------------------------------------------------------------------------
    Fill the h_name field
  -------------------------------------------------------------------------*/
  if( TYPE_PTR != question->type && NULL == (*phostent)->h_name )
  {
    (*phostent)->h_name = dnsi_memalloc( strlen(question->query) + 1 );
    if( NULL == (*phostent)->h_name )
    {
      *dss_errno = DS_ENOMEM;
      dnsi_query_hostent_free( phostent );
      return DSS_ERROR;
    }
    memcpy( (*phostent)->h_name,
            question->query,
            strlen(question->query) + 1 );
  }

  /*-------------------------------------------------------------------------
    Traverse the queue and fill up the hostent.
  -------------------------------------------------------------------------*/
  pentry = q_check( rr_q );

  while( NULL != pentry )
  {
    switch(pentry->rr.type)
    {
      case TYPE_CNAME:
        /*-------------------------------------------------------------------
          If the h_name field is already filled then move that to the
          aliases and copy the CNAME to h_name field.
        -------------------------------------------------------------------*/
        if( NULL != (*phostent)->h_name )
        {
          (*phostent)->h_aliases[num_aliases++] = (*phostent)->h_name;
        }
        /*-------------------------------------------------------------------
          The RR already has converted the data to a string while the length
          field has the string length including the terminating 0
        -------------------------------------------------------------------*/
        (*phostent)->h_name = dnsi_memalloc( pentry->rr.rr_length );
        if( NULL == (*phostent)->h_name )
        {
          *dss_errno = DS_ENOMEM;
          dnsi_query_hostent_free( phostent );
          return DSS_ERROR;
        }
        memcpy( (*phostent)->h_name,
                pentry->rr.rr,
                pentry->rr.rr_length );
        break;

      case TYPE_PTR:
        pchar = ( NULL == (*phostent)->h_name )  ?
                  &(*phostent)->h_name           :
                  &(*phostent)->h_aliases[num_aliases++];
        if( NULL == (*pchar = dnsi_memalloc( pentry->rr.rr_length )) )
        {
          *dss_errno = DS_ENOMEM;
          dnsi_query_hostent_free( phostent );
          return DSS_ERROR;
        }
        memcpy( *pchar, pentry->rr.rr, pentry->rr.rr_length ); 
        break;

      case TYPE_A:
      case TYPE_AAAA:
        if( (AF_INET == af && TYPE_A == pentry->rr.type)   ||
            (AF_INET6 == af && TYPE_AAAA == pentry->rr.type) )
        {
          (*phostent)->h_addr_list[num_address] =
                         dnsi_memalloc( pentry->rr.rr_length );
          if( NULL == (*phostent)->h_addr_list[num_address] )
          {
            *dss_errno = DS_ENOMEM;
            dnsi_query_hostent_free( phostent );
            return DSS_ERROR;
          }
          memcpy( (*phostent)->h_addr_list[num_address],
                  pentry->rr.rr,
                  pentry->rr.rr_length );
          num_address++;
        }
        break;
    } /* switch */

    /* Get the next RR entry */
    pentry = q_next( rr_q, &pentry->link );
  } /* while */

  return retval;
} /* dnsi_query_hostent_alloc */


/*===========================================================================

FUNCTION DNSI_QUERY_HOSTENT_FREE

DESCRIPTION
  Frees the hostent and associated memory

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  Frees the memory allocated to a hostent.

===========================================================================*/
static void dnsi_query_hostent_free
(
  struct dss_hostent **phostent        /* The dss_hostent to be freed      */
)
{
  int index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Free the aliases list
  -------------------------------------------------------------------------*/
  if( NULL != (*phostent)->h_aliases )
  {
    index = 0;
    while( NULL != (*phostent)->h_aliases[index] )
    {
      dnsi_memfree( &((*phostent)->h_aliases[index++]) );
    } /* while */
    dnsi_memfree( (char **) &((*phostent)->h_aliases) );
  }

  /*-------------------------------------------------------------------------
    Free the address list
  -------------------------------------------------------------------------*/
  if( NULL != (*phostent)->h_addr_list )
  {
    index = 0;
    while( NULL != (*phostent)->h_addr_list[index] )
    {
      dnsi_memfree( &((*phostent)->h_addr_list[index++]) );
    } /* while */
    dnsi_memfree( (char**)&((*phostent)->h_addr_list) );
  }

  if( NULL != (*phostent)->h_name )
  {
    dnsi_memfree( &((*phostent)->h_name) );
  }

  /*-------------------------------------------------------------------------
    Free the hostent
  -------------------------------------------------------------------------*/
  dnsi_memfree( (char **)phostent );

  return;
} /* dnsi_query_hostent_free() */


/*===========================================================================

                            DNS CACHE SUBSYSTEM

===========================================================================*/
/*===========================================================================

FUNCTION DNSI_CACHE_INIT

DESCRIPTION
  Initializes the DNS cache.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Initializes the DNS cache data structures.

===========================================================================*/
static void dnsi_cache_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the cache data structures
  -------------------------------------------------------------------------*/
  dns_cache_lock  = FALSE;
  dns_cache_timer = DSS_ERROR;
  q_init( &dns_cache_q );
  
  return;
} /* dnsi_cache_init() */


/*===========================================================================

FUNCTION DNSI_CACHE_ENTRY_FREE

DESCRIPTION
  Frees memory associated with cache entry passed in.

DEPENDENCIES
  The entry must have already been dequeued.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees memory associated with a cache entry.

===========================================================================*/
static void dnsi_cache_entry_free
(
  dns_cache_entry_s_type **pentry /* Cache entry to be freed               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Free memory associated with the owner field
  -------------------------------------------------------------------------*/
  if( NULL != (*pentry)->rr.owner )
  {
    dnsi_memfree( &(*pentry)->rr.owner );
  }

  /*-------------------------------------------------------------------------
    Free memory associated with the RR data field
  -------------------------------------------------------------------------*/
  if( NULL != (*pentry)->rr.rr )
  {
    dnsi_memfree( &(*pentry)->rr.rr );
  }

  /*-------------------------------------------------------------------------
    Free memory associated with the cache entry
  -------------------------------------------------------------------------*/
  dnsi_memfree( (char**)pentry );

  *pentry = NULL;

  return;
} /* dnsi_cache_entry_free() */


/*===========================================================================

FUNCTION DNSI_CACHE_FIND

DESCRIPTION
  Find a DNS name mapping in the cache.

DEPENDENCIES
  Assumes exclusive lock on the cache

RETURN VALUE
  DSS_SUCCESS  If found the mapping in the cache.  All matching resource
               records are returned as a linked list in the rr_q parameter
               if non-NULL.  If rr_q is NULL simply returns a success on
               a cache hit.
  DSS_ERROR    If didn't find the mapping or an error occured

SIDE EFFECTS
  If successful returns queue of RRs in the rr_q parameter.  Frees any
  expired cache entries.

===========================================================================*/
static int dnsi_cache_find
(
  void   *query,                  /* The query to resolve                  */
  uint16  class,                  /* The class of query                    */
  uint16  type,                   /* The type of query                     */
  q_type *cache_q,                /* Cache bucket to get RRs from          */
  q_type *rr_q                    /* Queued RRs to be returned on cache hit*/
)
{
  dns_cache_entry_s_type *pentry;
  int                     retval = DSS_ERROR;
  int                     count  = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Scan through the cache finding all cache entries matching the requested
    domain.
  -------------------------------------------------------------------------*/
  count = q_cnt( cache_q );

  while( count-- > 0 )
  {
    if ( ( pentry = q_get( cache_q ) ) == NULL )
	{
	  MSG_ERROR("Empty or corrupt DNS cache",0,0,0);
	  return retval;
	}
    /*-----------------------------------------------------------------------
      Check the timestamp to see if this entry needs to be released
    -----------------------------------------------------------------------*/
    if( msclock() > pentry->rr.ttl )
    {
      dnsi_cache_entry_free( &pentry );
    }
    else  /* cache entry is valid */
    {
      /*---------------------------------------------------------------------
        Compare the type, class fields and domain
      ---------------------------------------------------------------------*/
      if( pentry->rr.type  == type                               &&
          pentry->rr.class == class                              &&
          0 == strncmp( pentry->rr.owner, query, strlen(query) ) &&
          strlen(pentry->rr.owner) - strlen(query) <= 1
    	)
      {
        retval = DSS_SUCCESS;
        /*-------------------------------------------------------------------
          A match is found.  Queue this RR to the passed in queue.  Return
          success if passed queue is NULL.
        -------------------------------------------------------------------*/
        if( NULL == rr_q )
        {
          break;
        }
        q_put( rr_q, &pentry->link );
      }
      else
      {
        q_put( cache_q, &pentry->link );
      }
    }
  } /* while */
  
  return retval;
} /* dnsi_cache_find() */


/*===========================================================================

FUNCTION DNSI_CACHE_ADD

DESCRIPTION
  This function is passed a pointer to a queue containing RRs to be
  added to the cache.

DEPENDENCIES
  Uses the dns_cache_lock to obtain exclusive access to the cache.  If
  unable to acquire the lock returns with a DSS_ERROR.

RETURN VALUE
  None.
  
SIDE EFFECTS
  The passed RRs may be cached.

===========================================================================*/
static void dnsi_cache_add
(
  q_type *rr_q                    /* Queue containing resource records     */
)
{
  dns_cache_entry_s_type *pentry    = NULL;
  int                     do_cache  = TRUE;
  uint32                  ttl_value = DNS_CACHE_TTL_MAX_THRESHOLD;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Try acquiring the cache lock.  If we fail then free the queue and return
  -------------------------------------------------------------------------*/
  TASKLOCK();
  if( TRUE == dns_cache_lock )
  {
    do_cache = FALSE;
  }
  else
  {
    dns_cache_lock = TRUE;
  }
  TASKFREE();

  /*-------------------------------------------------------------------------
    The queued entries are added to the cache if:
    i)   there is enough room in the cache to add them
    ii)  the query doesn't already exist in the cache
    iii) the ttl exceeds the threshold ttl
  -------------------------------------------------------------------------*/
  if( TRUE == do_cache &&
      q_cnt(rr_q) + q_cnt(&dns_cache_q) <= DNS_CACHE_TBL_ENTRIES
    )
  {
    pentry = q_check( rr_q );
    while( NULL != pentry )
    {
      if( pentry->rr.ttl <= DNS_CACHE_TTL_MIN_THRESHOLD ||
          DSS_SUCCESS == dnsi_cache_find( pentry->rr.owner,
                                          pentry->rr.class,
                                          pentry->rr.type,
                                          &dns_cache_q,
                                          NULL )
        )
      {
        do_cache = FALSE;
        break;
      }

      if( ttl_value > pentry->rr.ttl )
      {
        ttl_value = pentry->rr.ttl;
      }
      pentry = q_next( rr_q, &pentry->link );
    } /* while */


    /*-------------------------------------------------------------------------
      Add RRs to cache if do_cache is true at this point.  Start the timer to
      free the RRs
    -------------------------------------------------------------------------*/
    if( TRUE == do_cache )
    {
      ttl_value *= 1000;         /* Convert ttl to milli seconds */
      ttl_value += msclock();

      while( NULL != (pentry = q_get( rr_q )) )
      {
        pentry->rr.ttl = ttl_value;
        q_put( &dns_cache_q, &pentry->link );
      } /* while */

      /*-----------------------------------------------------------------------
        Restart cache timer to free cache entries.
      -----------------------------------------------------------------------*/
      if( DSS_ERROR == dns_cache_timer )
      {
        dns_cache_timer = ps_timer_alloc( dnsi_cache_timeout_cb, NULL );
      }

      if( PS_TIMER_FAILURE != dns_cache_timer )
      {
        ps_timer_cancel( dns_cache_timer );
        ps_timer_start( dns_cache_timer, DNS_CACHE_TTL_MAX_THRESHOLD * 1000 );
      }
    }

    /*-----------------------------------------------------------------------
      Release cache lock
    -----------------------------------------------------------------------*/
    TASKLOCK();
    dns_cache_lock = FALSE;
    TASKFREE();
  }
  
  return;
} /* dnsi_cache_add() */


/*===========================================================================

FUNCTION DNSI_CACHE_LOOKUP

DESCRIPTION
  This function is passed a query to lookup and a data structure type to
  return if a cache hit occurs.  It searches the cache and in case of
  success returns the filled data structure back.

DEPENDENCIES
  Uses the dns_cache_lock to obtain exclusive access to the cache.  If
  unable to acquire the lock returns with a DSS_ERROR.

RETURN VALUE
  DSS_SUCCESS if successful with the relevant filled data structure
              returned as the ret parameter
  DSS_ERROR   if an error occured.  The error code is placed in dss_errno.
  
SIDE EFFECTS
  Frees expired cache entries.  Copies and returns the answer if a cache
  hit occurs.

===========================================================================*/
static int dnsi_cache_lookup
(
  dns_msg_question_s_type   *question, /* Canonical DNS query              */
  int32                      af,       /* Address family                   */
  dns_query_ret_e_type       ret_type, /* Data structure to return         */
  void                     **ret,      /* Ptr to data structure returned   */
  int16                     *dss_errno /* Error code                       */
)
{
  int                     retval  = DSS_SUCCESS;
  dns_cache_entry_s_type *pentry  = NULL;
  q_type                  cache_q;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Try acquiring the cache lock.  If we fail then return else proceed.
  -------------------------------------------------------------------------*/
  TASKLOCK();
  if( TRUE == dns_cache_lock )
  {
    retval = DSS_ERROR;
  }
  else
  {
    dns_cache_lock = TRUE;
  }
  TASKFREE();

  if( DSS_ERROR != retval )
  {
    q_init( &cache_q );

    /*-----------------------------------------------------------------------
      Search the cache for this query
    -----------------------------------------------------------------------*/
    retval = dnsi_cache_find( question->query,
                              question->class,
                              question->type,
                              &dns_cache_q,
                              &cache_q);
    if( DSS_SUCCESS == retval )
    {
      /*---------------------------------------------------------------------
        Fill in the relevant data structure
      ---------------------------------------------------------------------*/
      switch( ret_type )
      {
        case DNS_DATA_STRUCTURE_DSS_HOSTENT:
          dnsi_query_hostent_alloc( (struct dss_hostent**)ret,
                                    question,
                                    &cache_q,
                                    af,
                                    dss_errno
                                  );
          break;

        default:
          ASSERT( 0 );
          break;
      } /* switch */

      /*---------------------------------------------------------------------
        Put the cache entries back in the cache queue
      ---------------------------------------------------------------------*/
      while( NULL != (pentry = q_get( &cache_q )) )
      {
        q_put( &dns_cache_q, &pentry->link );
      }
    }
    /*-----------------------------------------------------------------------
      Release cache lock
    -----------------------------------------------------------------------*/
    TASKLOCK();
    dns_cache_lock = FALSE;
    TASKFREE();
  }


  return retval;
} /* dnsi_cache_lookup() */


/*===========================================================================

FUNCTION DNSI_CACHE_TIMEOUT_CB

DESCRIPTION
  Callback function for handling cache layer timeout.

DEPENDENCIES
  Uses the dns_cache_lock to obtain exclusive access to the cache.  If
  unable to acquire the lock then restart the timer.

RETURN VALUE
  None

SIDE EFFECTS
  Removes expired DNS cache entries.

===========================================================================*/
static void dnsi_cache_timeout_cb
(
  void *pindex                         /* Index in the query table         */
)
{
  int                     count;
  int                     retval = DSS_SUCCESS;
  dns_cache_entry_s_type *pentry;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MSG_LOW( "dnsi_cache_timeout_cb(): called", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Cancel timer.
  -------------------------------------------------------------------------*/
  ps_timer_cancel( dns_cache_timer );

  /*-------------------------------------------------------------------------
    Try acquiring the cache lock.  If we fail then free the queue and return
  -------------------------------------------------------------------------*/
  TASKLOCK();
  if( TRUE != dns_cache_lock )
  {
    dns_cache_lock = TRUE;
  }
  else
  {
    retval = DSS_ERROR;
  }
  TASKFREE();

  if( DSS_SUCCESS == retval )
  {
    /*-----------------------------------------------------------------------
      Scan through the cache finding all cache entries that have expired.
    -----------------------------------------------------------------------*/
    count = q_cnt( &dns_cache_q );

    while( count-- > 0 )
    {
      if ( ( pentry = q_get( &dns_cache_q ) ) != NULL )
	  {
        /*---------------------------------------------------------------------
          Check the timestamp to see if this entry needs to be released
        ---------------------------------------------------------------------*/
        if( msclock() > pentry->rr.ttl )
        {
          dnsi_cache_entry_free( &pentry );
        }
        else  /* cache entry is valid */
        {
          q_put( &dns_cache_q, &pentry->link );
        }
      }
    } /* while */
  
    if( 0 < q_cnt( &dns_cache_q ) )
    {
      retval = DSS_ERROR;
    }
    else
    {
      ps_timer_free( dns_cache_timer );
      TASKLOCK();
      dns_cache_timer = DSS_ERROR;
      TASKFREE();
    }

    /*-----------------------------------------------------------------------
      Release cache lock
    -----------------------------------------------------------------------*/
    TASKLOCK();
    dns_cache_lock = FALSE;
    TASKFREE();
  }

  /*-------------------------------------------------------------------------
    If there were still RRs left or we couldn't acquire the cache lock then
    restart the timer.
  -------------------------------------------------------------------------*/
  if( DSS_ERROR == retval )
  {
    ps_timer_start( dns_cache_timer, DNS_CACHE_TTL_MAX_THRESHOLD * 1000 );
  }

  return;
} /* dnsi_query_timeout_cb() */


/*===========================================================================

                            DNS MEMORY SUBSYSTEM

===========================================================================*/
/*===========================================================================

FUNCTION  DNSI_MEMALLOC

DESCRIPTION
  The function is passed the amount of memory required.  If it finds a
  chunck of memory of suitable size to service the request it returns that
  otherwise it returns a NULL.

  This function may be called from tasks other then PS and therefore must
  be thread safe.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to memory block if successful. 
  NULL if could not get memory.

SIDE EFFECTS
  May allocate a large DSM item.  The DSM item is not freed until all memory
  allocated from it is freed.

===========================================================================*/
static void* dnsi_memalloc
(
  int32 size                           /* Size of memory to be allocated   */
)
{
  char                      *dataptr  = NULL; /* Ptr to memory allocated   */
  dsm_item_type             *dsmptr   = NULL; /* Pointer to a DSM item     */
  dns_mem_header_s_type     *phdr     = NULL;
  int                        total_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify parameter
  -------------------------------------------------------------------------*/
  ASSERT( size > 0 );

  /*-------------------------------------------------------------------------
    The size of memory chunk to be allocated includes the requested size
    plus the chunk header.  Allocate enough memory to take care of alignment
    issues.
  -------------------------------------------------------------------------*/
  total_size = size + sizeof( dns_mem_header_s_type );
  total_size = (sizeof(int32) - 1 + total_size) & (~(sizeof(int32) - 1)); 

  /*-------------------------------------------------------------------------
    Find if we can service this memory request with already allocated memory
  -------------------------------------------------------------------------*/
  TASKLOCK();
  phdr = dns_mem_pool;
  TASKFREE();

  while( NULL != phdr )
  {
    switch( phdr->type )
    {
      case DNS_MEM_TYPE_DSM:
        /*-------------------------------------------------------------------
          The memory chunk is a DSM item.  Get to the DSM header.
        -------------------------------------------------------------------*/
        dsmptr = phdr->container;

        TASKLOCK();
        if( dsmptr->size - dsmptr->used >= total_size )
        {
          phdr = (dns_mem_header_s_type *)(dsmptr->data_ptr + dsmptr->used);

          /*-----------------------------------------------------------------
            Increment the DSM app_field to indicate another chunk allocated,
            adjust the used field.
          -----------------------------------------------------------------*/
          dsmptr->used += total_size;
          dsmptr->app_field++;

          /*-----------------------------------------------------------------
            Fill the chunk header.
          -----------------------------------------------------------------*/
          phdr->type        = DNS_MEM_TYPE_DSM;
          phdr->next_header = NULL;
          phdr->prev_header = NULL;
          phdr->container   = dsmptr;

          /*-----------------------------------------------------------------
            Make the dataptr point after the chunk header, NULL phdr to exit
            the outer while loop.
          -----------------------------------------------------------------*/
          dataptr = (char *)++phdr;
          phdr = NULL;
        }
        else
        {
          phdr = phdr->next_header;
        }
        TASKFREE();
        break;

      default:
        ASSERT( 0 );
        phdr = phdr->next_header;
        break;
    } /* switch */
  } /* while */

  /*-------------------------------------------------------------------------
    If we couldn't service the request with existing resources allocate a
    new DSM item / memory chunk.
  -------------------------------------------------------------------------*/
  if( NULL == dataptr )
  {
    dsmptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
    if( NULL !=  dsmptr ) 
    {
      /*-------------------------------------------------------------------
        Increment use count.
      -------------------------------------------------------------------*/
      dsmptr->app_field = 0;

      if( dsmptr->size - dsmptr->used >= total_size )
      {
        /*-------------------------------------------------------------------
          Fill the chunk header.  Make the dataptr point after the chunk
          header.  Increment the DSM app_field to indicate another chunk
          allocated, adjust the used field.  This being the first chunk
          header in the DSM item is linked in the dns_mem_pool as the first
          item.
        -------------------------------------------------------------------*/
        phdr = (dns_mem_header_s_type*) (dsmptr->data_ptr + dsmptr->used);
        dsmptr->used     += total_size;
        dsmptr->app_field++;
        phdr->type        = DNS_MEM_TYPE_DSM;
        phdr->container   = dsmptr;
        TASKLOCK();
        phdr->next_header = dns_mem_pool;
        phdr->prev_header = &dns_mem_pool;
        if( NULL != dns_mem_pool )
        {
          dns_mem_pool->prev_header = &phdr->next_header;
        }
        dns_mem_pool      = phdr;
        dataptr           = (char *)++phdr;
        TASKFREE();
      }
      else
      {
        /*-------------------------------------------------------------------
          The memory request is too large for our resources
        -------------------------------------------------------------------*/
        dsm_free_buffer( dsmptr );
      }
    }
  }

  return dataptr;
} /* dnsi_memalloc() */


/*===========================================================================

FUNCTION  DNSI_MEMFREE

DESCRIPTION
  Free memory allocated by dnsi_memalloc() and sets the passed memory
  pointer to NULL.
  
DEPENDENCIES
  The memory chunk passed to be freed must have been allocated by
  dnsi_memalloc().

RETURN VALUE
  None.

SIDE EFFECTS
  May free the memory chunk/DSM item which contains the chunk to be freed.
  Sets the passed in pointer to memory to NULL.

===========================================================================*/
static void dnsi_memfree
(
  char **pmem                     /* Memory to free                        */
)
{
  dsm_item_type         *dsmptr = NULL;/* Pointer to a DSM item            */
  dns_mem_header_s_type *phdr   = NULL;/* Pointer to memory chunk header   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Verify parameters
  ------------------------------------------------------------------------*/
  ASSERT( NULL != pmem  );
  ASSERT( NULL != *pmem );

  /*-----------------------------------------------------------------------
    Get the memory chunk header.
  -----------------------------------------------------------------------*/
  phdr = (dns_mem_header_s_type*) *pmem;
  phdr--;
  switch( phdr->type )
  {
    case DNS_MEM_TYPE_DSM:
      /*---------------------------------------------------------------------
        Memory chunk is from a DSM item.  Decrement the use count and free
       	the item if use count ges to zero.
      ---------------------------------------------------------------------*/
      dsmptr = phdr->container;
      TASKLOCK();
      --(dsmptr->app_field);
      TASKFREE();
      if( 0 == dsmptr->app_field )
      {
        TASKLOCK();
        /*-------------------------------------------------------------------
          Get the first memory chunk header in the DSM item which points to
          the next and previous memory chunks.
        -------------------------------------------------------------------*/
        phdr = (dns_mem_header_s_type *)dsmptr->data_ptr;
        *(phdr->prev_header) = phdr->next_header;
        if( NULL != phdr->next_header )
        {
          phdr->next_header->prev_header = phdr->prev_header;
        }
        TASKFREE();
        dsm_free_packet( &dsmptr );
      }
      break;

    default:
      ASSERT( 0 );
      break;
  }

  *pmem = NULL;

  return;
} /* dnsi_memfree() */

#endif  /* FEATURE_DS_SOCKETS */

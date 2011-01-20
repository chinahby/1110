#ifndef _DSSDNS_H
#define _DSSDNS_H
/*===========================================================================

   D A T A   S E R V I C E S   S O C K E T   A P I   H E A D E R   F I L E
                   
DESCRIPTION

 The Data Services Socket Header File. Contains shared variables and enums, 
 as well as declarations for functions.

Copyright (c) 2000, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssdns.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/02   ifk     Changed dss_errno parameters to be int16*.  Added comments
11/25/02   ifk     Fixed comment in dss_getipnodebyaddr(); DS_EFAULT is the
                   error returned if argument incorrect.
11/06/02   ifk     Made dss_errno the last argument in ipnode functions.
11/01/02   ifk     rewrote for the new DNS.
01/11/02   ss      Moved external function declarations from dssocket.h to
                   this file. Removed resolver() declaration.
09/08/00   ttl     Added cache table and refined the function structures.
05/22/00   ttl     Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
#include "dssocket.h"

#ifndef FEATURE_DATA_MM
/*---------------------------------------------------------------------------
  Name of structure ps_cmd_name_type changed in MM DATA to ps_cmd_enum_type
---------------------------------------------------------------------------*/
#define ps_cmd_enum_type ps_cmd_name_type
#include "ps.h"
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Number of simultaneous queries supported, number of PS timers needed.
---------------------------------------------------------------------------*/
#define DSS_DNS_MAX_QUERIES 5
#define DSS_DNS_PS_TIMER    (DSS_DNS_MAX_QUERIES + 1)

/*---------------------------------------------------------------------------
  The host entry structure                     
---------------------------------------------------------------------------*/
typedef struct dss_hostent
{
  char  *h_name;                       /* Official host name               */
  char **h_aliases;                    /* NULL-terminated alias list       */
  int    h_addrtype;                   /* host address type                */
  int    h_length;                     /* length of address                */
  char **h_addr_list;                  /* NULL-terminated address list     */
} dss_hostent;


/*===========================================================================

                      PUBLIC FUNCTION TYPEDEFS

===========================================================================*/
/*===========================================================================

FUNCTION TYPEDEF DSS_DNS_CB_F_TYPE 

DESCRIPTION
  Typedef for the dss_getipnodebyname() and dss_getipnodebyaddr() callback
  function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*dss_dns_cb_f_type)
(
  struct dss_hostent *phostent,            /* Official host name           */
  void               *cb_voidptr,          /* application passed pointer   */
  int16               dss_errno            /* error code if phostent NULL  */
);


#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/*===========================================================================

FUNCTION TYPEDEF DSS_GETHOSTADDRS_CB_F_TYPE

DESCRIPTION
  Typedefs for the dss_gethostaddrs() callback function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*dss_gethostaddrs_cb_f_type)
(
  void
);
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
extern void dss_dns_init
(
   void
);


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

  At present support for IPv6 is not added.

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

  dss_errno values
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
extern struct dss_hostent *dss_getipnodebyname
(
  char             *name,                  /* query domain name            */
  int32             af,                    /* address family desired       */
  int32             flags,                 /* options, currently set to 0  */
  dss_dns_cb_f_type callback,              /* callback to return mapping   */
  void             *cb_voidptr,            /* application supplied data    */
  int16            *dss_errno              /* error, if return is NULL     */
);


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

  dss_errno values
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
extern struct dss_hostent *dss_getipnodebyaddr
(
  void              *addr,                 /* query domain name            */
  int32              len,                  /* length of the query field    */
  int32              af,                   /* address family queried       */
  dss_dns_cb_f_type  callback,             /* callback to return mapping   */
  void              *cb_voidptr,           /* application supplied data    */
  int16             *dss_errno             /* error, if return is NULL     */
);


/*===========================================================================

FUNCTION DSS_FREEHOSTENT

DESCRIPTION
  This function is called to free the dss_hostent structure returned by
  dss_getipnodebyname() or dss_getipnodebyaddr() functions.  It also frees
  all associated data structures used to resolve the query.

PARAMETERS
  phostent    pointer to dss_hostent structure to be freed

DEPENDENCIES
  The query entry to be freed should be in the valid q

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dss_freehostent
(
  struct dss_hostent *phostent         /* Pointer to dss_hostent to free   */
);


/*===========================================================================

FUNCTION DSS_INET_ATON

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
extern int32 dss_inet_aton
(
  const  char    *cp,                  /* IPv4 addr in dotted quad string  */
  struct in_addr *addr                 /* Returned IPv4 address            */
);


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
extern int32 dss_inet_ntoa
(
  const struct in_addr  in,        /* IPv4 address to be converted         */
  uint8                *buf,       /* Buffer to hold the converted address */
  int32                 buflen     /* Length of buffer                     */
);


#ifdef FEATURE_DSS_DNS_DEPRECATED_API
/* maximum dns name length passed to deprecated API functions */
#define DSS_DNS_NAME_LEN  128
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
);


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
     i                   not available, this field will be 0.

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
extern int16 dss_gethostaddrs
(
  char                       *domain_name, /* Domain name to resolve       */
  uint32                     *ip_addr,     /* IP address mapped to name    */
  dss_gethostaddrs_cb_f_type  cb_func      /* Callback function            */
);
#endif /* FEATURE_DSS_DNS_DEPRECATED_API */


#ifndef FEATURE_DATA_MM
/*===========================================================================

FUNCTION DNSI_SOCK_CB_CMD_HANDLER

DESCRIPTION
  Callback cmd handler function for socket events, handles the
  PS_DNS_SOCK_CB_CMD.  This function declaration will be removed from the 
  header file with the new MM DATA architecture.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dnsi_sock_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *app_id             /* Application ID                   */
);


/*===========================================================================

FUNCTION DNSI_NET_CB_CMD_HANDLER

DESCRIPTION
  Callback cmd handler function for network events, handles the
  PS_DNS_NET_CB_CMD.  This function definition will be removed from the
  header file with the new MM DATA architecture.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dnsi_net_cb_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *appid              /* application id                   */
);


/*===========================================================================

FUNCTION DNSI_QUERY_START_CMD_HANDLER

DESCRIPTION
  Handles PS_DNS_START_CMD.  Calls dnsi_sock_alloc() to acquire a socket.
  This function prototype will be removed with the MM data architecture.

DEPENDENCIES
  Query is in the opening state.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dnsi_query_start_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *query_index        /* Application id                   */
);


/*===========================================================================

FUNCTION DNSI_QUERY_STOP_CMD_HANDLER

DESCRIPTION
  Handles the PS_DNS_STOP_CMD, frees the query table entry and its associated
  data structures.  This function prototype will be removed with the MM data
  architecture.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dnsi_query_stop_cmd_handler
(
  ps_cmd_enum_type  cmd,               /* PS command                       */
  void             *query_index        /* Application id                   */
);
#endif /* ifndef FEATURE_DATA_MM */

#endif  /* FEATURE_DS_SOCKETS */
#endif  /* _DSSDNS_H */

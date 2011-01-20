#ifndef DS_DSSTCP_H
#define DS_DSSTCP_H

#ifdef FEATURE_DS_SOCKETS
/*===========================================================================

     D A T A  S E R V I C E S  S O C K E T  T C P  H E A D E R  F I L E
                   
DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the 
 functions used by the sockets library to access TCP socket function calls.
 
EXTERNALIZED FUNCTIONS

  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect_setup()
    Sets up specific parameters for TCP sockets.  Called prior 
    dsstcp_connect(), it sets up the appropriate state of the socket
    so that the correct error response will be generated as needed.

  dsstcp_connect()
    Connects a TCP socket.

  dsstcp_listen_setup()
    Allocates child sockets for the backlog of tcp connections for
    passive tcp open.  A message is sent to PS for each of the child
    sockets to open it in passive mode.

  dsstcp_listen()
    Does the passive open for listener socket child sockets. 

  dsstcp_accept()
    Removes and return the a child socket from a listener sockets
    ready queue. This will also try to refill the half open
    connections queue.

  dsstcp_write()
    TCP specific write() function.  Sends the dsm memory item out
    the TCP transport.

  dsstcp_read()
    TCP specific read() function.  Reads specified number of bytes from
    the TCP transport.

  dsstcp_read_dsm_chain()
    TCP specific read_dsm_chain() function.  Reads a DSM item chain from
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and 
    associated resources.

  dsstcp_abort()
    TCP specific abort() function. Aborts the TCP connection and cleans-up.
    This is usually called when we are dormant.
    

  dsstcp_get_conn_error()
    Gets the appropriate error code for connection errors.

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection. 

  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then send a 
    reset to the peer, and then close down sockets and notify the user.       
    
  dsstcp_bind()
    This is the TCP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

Copyright (c) 1998-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.h_v   1.2   13 Feb 2003 12:21:28   ssinghai  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dsstcp.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/03   sv      Modified dsstcp_is_writeable to return integer.
04/15/03   sv      Modified dsstcp_read to accept iov and iovcount as 
                   arguments instead of buffer and bufferlen.
02/13/03   ss      Added dsstcp_shutdown()
12/22/02   aku     Changed dsstcp_new_ip_upcall() signature to take 
                   ps_iface_ptr as argument instead of IP addresses.
05/02/02   mvl     File clean up, added meta_info_ptr to write() prototype
02/22/02   pjb     Added accept and listen
02/12/02   ss      Added prototype for dsstcp_cleanup_all_reseq().
02/11/02   ss      Added prototype for dsstcp_is_writeable().
10/11/01   mt      Add dss_errno argument to dsstcp_connect_setup().
03/27/01   mt      Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01   js      Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with the dss_fcn_ptr_table format.
02/08/01   yll     Added dsstcp_read_dsm_chain().
11/10/00   snn     Changed scb_type to scb_s
10/10/00   na      Removed tcp_ptr which is not longer used. Added prototype
                   for dsstcp_init_tcb_array().
08/03/00   mvl/js  Added prototype for dsstcp_bind() function.
03/22/00   rc/na   Added prototype for function dsstcp_new_ip_upcall().
09/22/99   rc      Incorporated code review changes and comments.
08/11/99   rc/ak   Extern for tcp abort function.
12/14/98   na/hcg  Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "dssocki.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.
  
  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS
  
  On error, return DSS_ERROR and places the error condition value in *errno.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_socket
(
  struct scb_s* scb_ptr,         /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
);

/*===========================================================================
FUNCTION DSSTCP_CONNECT_SETUP()

DESCRIPTION
  This function sets up specific parameters for TCP sockets, and should
  be called prior to making a call to the TCP connect function. Specifically
  it sets the appropriate state of the socket, within the context of the
  calling application, so that the correct error responses will be generated
  as needed.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_connect_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                    /* address of error condition value */
);

/*===========================================================================
FUNCTION DSSTCP_CONNECT()

DESCRIPTION
  This function connects a TCP socket and should only be called when 
  PPP/traffic channel is established.
    
  This function is called from the context of the PS  task.

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_connect
(
  struct scb_s* scb_ptr      /* Ptr to socket control block for the socket */
);

/*===========================================================================
FUNCTION DSSTCP_LISTEN_SETUP()

DESCRIPTION
  This starts a passive open on a tcp socket. 
  This is called from the context of the application. 
 // PJB Fill in from .c file....
DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.
 // PJB Fill in from .c file....
RETURN VALUE 
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_listen_setup
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 backlog,	
  sint15 * dss_errno
);


/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack
 // PJB Fill in from .c file....
DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.
 // PJB Fill in from .c file....
RETURN VALUE 
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_listen
(
  struct scb_s* scb_ptr     /* Ptr to socket control block for the socket */
);


/*===========================================================================
FUNCTION DSSTCP_ACCEPT()

DESCRIPTION
  This is called from the context of the application to accept a new
  connection.  

DEPENDENCIES

  The scb that this is called with should be in the LISTEN state, and
  the ACCEPT_EVENT should have been posted.
 // PJB Fill in from .c file....
RETURN VALUE
  The file descriptor of the new connection is returned on success. 
 // PJB Fill in from .c file....

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_accept
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct sockaddr* remote_addr,
  uint16 * addrlen,
  sint15 * dss_errno
);


/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_write
(
  protocol_ctl_block_type pcb,             /* protocol control block       */
  struct sockaddr_in *ta_ptr,              /* destination socket info      */
  dsm_item_type *item_ptr,                 /* ptr to head of dsm pkt chain */
  ps_meta_info_type *meta_info_ptr         /* ptr to meta info             */
);

/*===========================================================================
FUNCTION DSSTCP_READ()

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.
  
  This function does no sanity checking. The sanity checking should be done
  in dss_read().
  
  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of 
      bytes specified.
      
  If there are no bytes to be read in the rcvq, the return value is 
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_read
(
  struct scb_s * scb_ptr,    /* Ptr to socket control block for the socket */
  struct iovec * iov,         /* scattered buffer to which to copy data    */
  uint16 iovlen,               /* length of iovec array                    */
  struct sockaddr_in *fromaddr,           /* destination address - ignored */
  sint15 *errno                                   /* error condition value */
);

/*===========================================================================
FUNCTION DSSTCP_READ_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.
  
  This function does no sanity checking. The sanity checking should be done
  in dss_read_dsm_chain().
  
  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read from the DSM item chain.

  If there are no items to be read in the rcvq, the return value is 
  DSS_ERROR and DS_EWOULDBLOCK is returned in errno.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_read_dsm_chain
(
  struct scb_s* scb_ptr,         /* Ptr to socket control block for the socket */
  dsm_item_type **item_ptr,         /* Ptr to DSM item chain to be read in */
  struct sockaddr_in *fromaddr,           /* destination address - ignored */
  sint15 *errno                                   /* error condition value */
);
/*===========================================================================
FUNCTION DSSTCP_CLOSE()

DESCRIPTION
  Initiates the active close for TCP.  If the connection is already closed,
  cleans up the TCP socket and associated resources.
                         
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Initiates active close for TCP connections.
===========================================================================*/
extern sint15 dsstcp_close
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
);

/*===========================================================================
FUNCTION DSSTCP_ABORT()

DESCRIPTION
  Aborts the TCP connection and cleans-up. This is usually called when we are
  dormant.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  SCB is freed up and the pointer is no longer valid.
===========================================================================*/
extern sint15 dsstcp_abort
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno              /* Error condition value                      */
);


/*===========================================================================
FUNCTION DSSTCP_GET_CONN_ERROR()

DESCRIPTION
  Gets the appropriate error code from the closed_state_reason.  Depending
  on the type of call (connect(), or read()/write()), different error
  values can be returned.

DEPENDENCIES
  None.

RETURN VALUE
  Error condition for connection state.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_get_conn_error
(
  struct scb_s *scb_ptr,                    /* ptr to socket control block */
  dssocki_caller_enum_type type             /* type of the caller function */
);


/*===========================================================================
FUNCTION DSSTCP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a 
  valid connection.
                         
DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
extern struct tcb *dsstcp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
);


/*===========================================================================
FUNCTION DSSTCP_LOOKUP_SERV()

DESCRIPTION
  Given a connection structure, looks up in the tcb to see if this is a 
  valid server waiting for new connections.
                         
DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCB if valid connection.  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
extern struct tcb *dsstcp_lookup_serv
(
  struct connection *conn                    /* connection to be looked up */
);


/*===========================================================================
FUNCTION DSSTCP_NEW_IP_UPCALL()

DESCRIPTION
  This function will be invoked by PPPIPCP when a new IP address is 
  negotiated with the Base Station.  If that is the case, then 
  send a reset to the peer, and then close down sockets and notify 
  the user.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void dsstcp_new_ip_upcall
(                                                                            
  ps_iface_type *iface_ptr
);


/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.
                         
DEPENDENCIES
  None.

RETURN VALUE
  None. 

SIDE EFFECTS
  None.
===========================================================================*/
extern void dsstcp_bind
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct sockaddr_in *la     /* Ptr to the structure contains the local 
                                address.                                   */
);

/*===========================================================================
FUNCTION DSSTCP_IS_WRITEABLE()

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern int dsstcp_is_writeable
(
  protocol_ctl_block_type pcb                    /* protocol control block */
);
                        
/*===========================================================================
FUNCTION DSSTCP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for tcp sockets.

DEPENDENCIES
  None.

PARAMETERS
  scb_ptr - pointer to scb which needs to be shut down
  item_ptr - a dsm item containing information about what kind of shutdown 
             needs to be done (read, write or both)
  dss_errno - error number returned in case of error

RETURN VALUE
  On success, returns DSS_SUCCESS otherwise returns DSS_ERROR and places
  the error number in dss_errno.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dsstcp_shutdown
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type*  item_ptr,  /* dsm item containing the call specific info */
  sint15* dss_errno                                    /* dss error number */
);

/*===========================================================================
FUNCTION DSSTCP_INIT_TCB_ARRAY()

DESCRIPTION
  This function initializes the array of tcbs to zero.
  
  This function should be called at startup.
  
  This function is called from the context of the PS task.
  

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void dsstcp_init_tcb_array
(
  void
);
                        
/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This 
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_cleanup_all_reseq
(
  void
);

#endif /* FEATURE_DS_SOCKETS */

#endif /* DS_DSSTCP_H */

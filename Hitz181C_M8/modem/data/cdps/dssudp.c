/*===========================================================================

                           D S S U D P . C
                   
DESCRIPTION

 The Data Services sockets UDP interface file. This contains all the 
 functions used by the sockets library to access UDP socket function calls.

EXTERNALIZED FUNCTIONS

  dssudp_socket()
    UDP specific socket() function.  Sets up the UDP socket and creates
    the UDP control block.

  dssudp_write()
    UDP specific write() function.  Sends specified number of bytes out
    the UDP datagram transport.

  dssudp_read()
    UDP specific read() function.  Reads specified number of bytes from
    the UDP datagram transport.

  dssudp_close()
    UDP specific close() function.  Simply cleans up socket control
    block and makes the socket available for re-use.

  dssudp_bind()
    This is the UDP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dssudp_connect_setup()
    This is the UDP specific connect function. It fills in the peers ipaddr
    and binds the socket to a particular interface.

  dssudp_shutdown()
    This is the UDP specific shutdown function. It clears the receive queue
    of the socket and generates a socket event.

  dssudp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.  

  dssudp_process_rx_pkt()
    Processes incoming UDP packet to see which socket it belongs to.

Copyright (c) 1998-2008 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssudp.c_v   1.1   08 Oct 2002 23:16:52   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssudp.c#2 $ $DateTime: 2008/02/26 05:58:00 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/08    sn     Fixed lint errors.
10/18/07    sn     Replaced DSM_SMALL_ITEM_LESS_THAN_TRANSPORT_FEW and 
                   DSM_LARGE_ITEM_LESS_THAN_TRANSPORT_FEW macros with 
                   DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS macro. Pulled 
                   in this change from tip.
05/17/04    sv     Modified dssudp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call
07/17/03    ss     Decremented the rcvcnt in dssudp_read() while discarding 
                   data due to the supplied buffer being too small
05/28/03    ss     Changed code to handle rcvq as a dsm item chain instead of 
                   queue. Added support for rcvbuf socket option.
05/27/03    om     Added dssudp_read_dsm_chain().
05/13/03    ss     Removed an unused variable declaration from dssudp_close()
05/08/03    ss     Added dssudpi_free_rcvq() and INTLOCK'ed queue accesses
05/05/03    ss     Protected q_put() and q_get() within INTLOCK's
04/15/03    sv     Added dssudp_connect_setup and dssudp_shutdown functions.
                   Modified dssudp_read to accept iov and iocount instead of
                   buffer and bufferlen as arguments.
04/15/03    sv     Modified dssudp_process_rx_pkt to support connected udp
                   sockets and to accept ucb_ptr and dsm_item as arguments.
                   Added dssudp_lookup_conn function.
04/10/03    ss     Modified dssudp_write() to change the parameter to 
                   getephem_port()
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/21/02    na     Fixed comment
07/15/02    na     Added dssudp_process_rx_pkt() (used to be ps_process_udp
                   in old psmgr.c)
07/14/02    na     Fixed ip_addr_type to support FEATURE_DATA_MM.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/08/02    ss     Modified dssudp_read() to truncate the datagram if the 
                   supplied buffer is too small.
12/18/01    rsl    Updated tracer field.
07/23/01    pjb    Return type of dsm_pushdown changed.  
06/27/01    na     Replaced DSSUDPI_MAX_UDP_SOCKS with DSS_MAX_UDP_SOCKS 
                   making the usage consistent.
03/27/01    mt     Changed type of second argument in dssudp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dssudp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. This avoids using the
                   destination information in scb, which may be stale.
02/14/01    js     Modified call to udp_output() to support DSS_IP_TTL. 
11/10/00    snn    Renamed scb_type as scb_s
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.                  
08/03/00    mvl/js Added function dssudp_bind() for the protocol specific
                   assignment of local.conn.port. 
06/07/00    rc     Removed ovld of link ptr in dsm items.
05/11/00    mvl    Modified DSSUDPI_MAX_UDP_SOCKS to support multiple UDP
                   sockets (the current number is 4). 
12/23/99    na     Changed q_inserts to use the ovld of link ptr in dsm items.
04/09/99    hcg    Added FEATURE_Q_NO_SELF_QPTR to support queue services memory 
                   optimization.
04/08/99    hcg    Added host to network byte conversion for dssudp_read() to
                   convert the incoming IP address to network byte ordering.
04/05/99    hcg    Incorporated code review changes and comments.  Changed 
                   some of the diagnostic message levels.  Modified use of 
                   INTLOCK()/INTFREE() macros to ensure protection of access
                   to global data. 
12/09/98   na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
#include "assert.h"
#include "msg.h"
#include "psglobal.h"
#include "netuser.h"
#include "dssocket.h"
#include "dssocki.h"
#include "ps.h"
#include "dsm.h"
#include "ps_udp.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_meta_info.h"


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
===========================================================================*/

/*---------------------------------------------------------------------------
  Array of UDP control blocks.
---------------------------------------------------------------------------*/
struct udp_cb ucb_array[DSS_MAX_UDP_SOCKS];


/*===========================================================================

                    FORWARD  DECLARATIONS FOR MODULE
===========================================================================*/
LOCAL struct udp_cb *dssudpi_create_ucb(struct scb_s *scb_ptr);
LOCAL void           dssudpi_free_ucb(struct udp_cb* ucb_ptr);

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSUDPI_CREATE_UCB()

DESCRIPTION
  This function creates a new UDP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL struct udp_cb *dssudpi_create_ucb
(
  struct scb_s *scb_ptr                        /* Ptr to socket ctl block. */
)
{
  int index;                                                 /* loop index */
  struct udp_cb* ucb_ptr = NULL;               /* Ptr to UCB to be created */
  dword int_sav;         /* local variable used for saving interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    loop through array to find available UCB - break when one is available.
    Access to the global UCB array is protected through the use of 
    INTLOCK_SAV()/INTFREE_SAV().
   ------------------------------------------------------------------------*/
  for (index=0; index< DSS_MAX_UDP_SOCKS; index++)
  {
    INTLOCK_SAV(int_sav);
    if (ucb_array[index].scb_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        Assign ucb_ptr and memset the ucb to 0
      ---------------------------------------------------------------------*/
      ucb_ptr = &ucb_array[index];
      memset( ucb_ptr, 0, sizeof( struct udp_cb));
      ucb_ptr->scb_ptr = scb_ptr;
      INTFREE_SAV(int_sav);
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  /*-------------------------------------------------------------------------
    If no UDP control block is available, return NULL.
   ------------------------------------------------------------------------*/
  if (index == DSS_MAX_UDP_SOCKS)
  {
    return (NULL);
  }

  return (ucb_ptr);

} /* dssudpi_create_ucb() */

/*===========================================================================
FUNCTION DSSUDPI_FREE_UCB()

DESCRIPTION
  This function frees UDP control block, making it available for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void dssudpi_free_ucb
(
  struct udp_cb* ucb_ptr                         /* Ptr to UCB to be freed */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (ucb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Free the ucb_ptr and set everything to NULL
   ------------------------------------------------------------------------*/
  memset( ucb_ptr, 0, sizeof( struct udp_cb));

} /* dssudpi_free_ucb() */

/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSSUDP_SOCKET()

DESCRIPTION
  This function sets up the UDP socket and creates the UDP control block.
  
  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS
  
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EMFILE           too many descriptors open.  A socket is already open or 
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_socket
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check to see that protocol matches socket type.
   ------------------------------------------------------------------------*/
  if ( (scb_ptr->protocol != IPPROTO_UDP) && (scb_ptr->protocol != NULL) )
  {
    *errno = DS_EPROTOTYPE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Create UDP control block.
   ------------------------------------------------------------------------*/
  if ( (ucb_ptr = dssudpi_create_ucb( scb_ptr)) == NULL)
  {
    MSG_HIGH(" Could not create a UCB", 0, 0, 0);
    *errno = DS_EMFILE;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set socket state to OPEN
   ------------------------------------------------------------------------*/
  dssocki_set_sock_state(scb_ptr, DSSOCKI_OPEN);

  /*-------------------------------------------------------------------------
    Associate the ucb with the scb through the pcb ptr in the scb
   ------------------------------------------------------------------------*/
  scb_ptr->protocol_ctl_blk.ucb = ucb_ptr;

  return (DSS_SUCCESS);
   
} /* dssudp_socket() */

/*===========================================================================
FUNCTION DSSUDP_WRITE()

DESCRIPTION
  Sends dsm memory item out to UDP using udp_output found in udp.c.
  
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes written.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_write
(
  protocol_ctl_block_type pcb,             /* protocol control block       */
  struct sockaddr_in *ta_ptr,              /* destination socket info      */
  dsm_item_type *item_ptr,                 /* ptr to head of dsm pkt chain */
  ps_meta_info_type *meta_info_ptr         /* ptr to meta info             */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  uint32        *dest_ip_ptr;                    /* Ptr to dest IP address */
  struct scb_s  *scb_ptr;                       /* Ptr to socket ctl block */
  udp_hdr_type  udp_hdr;                                     /* UDP header */
  sint15        length;                       /* length of payload written */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup the socket and UDP control block pointers.
   ------------------------------------------------------------------------*/
  ucb_ptr = pcb.ucb;
  scb_ptr = ucb_ptr->scb_ptr;
  dest_ip_ptr = &( scb_ptr->servname.address);

  /*-------------------------------------------------------------------------
    Use destination information from dsm_item chain. If not possible.
    use the information in scb (may be stale).
  -------------------------------------------------------------------------*/
  if (ta_ptr != NULL && ta_ptr->sin_addr.s_addr != 0)
  {
    dest_ip_ptr = &(ta_ptr->sin_addr.s_addr);
  }
  else
  {
    dest_ip_ptr = &( scb_ptr->servname.address);
  }

  /*-------------------------------------------------------------------------
    If socket is not already attached to a local address , assign ephemeral 
    port.  Local IP address is assigned in udp_output().
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.port == 0)
  {
    scb_ptr->localname.port = dssocki_getephem_port(scb_ptr->protocol);
    scb_ptr->localname.port = dss_ntohs(scb_ptr->localname.port);
    ucb_ptr->conn.local.port = scb_ptr->localname.port;
  }
  else
  {
    ucb_ptr->conn.local.port    =  scb_ptr->localname.port;
  }

  /*-------------------------------------------------------------------------
    Fill in the known UDP header fields, except the checksum.
  -------------------------------------------------------------------------*/
  udp_hdr.source = ucb_ptr->conn.local.port;

  /*-------------------------------------------------------------------------
    Use the non-zero port number from dsm_item chain. If not possible.
    use the information in scb (may be stale).
  -------------------------------------------------------------------------*/
  if (( ta_ptr != NULL ) && ( ta_ptr->sin_port != 0 ))
  {
    udp_hdr.dest = ta_ptr->sin_port;
  }
  else
  {
    udp_hdr.dest = scb_ptr->servname.port;
  }
  length         = (sint15) dsm_length_packet(item_ptr);
  udp_hdr.length = (word) ((uint16) length + sizeof(udp_hdr_type));

  /*-------------------------------------------------------------------------
    Call udp_output, which will make up the UDP header and send the packet to
    IP.
   ------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK
  
  udp_output(dest_ip_ptr, &udp_hdr, item_ptr, ucb_ptr, meta_info_ptr);

  /*-------------------------------------------------------------------------
    Return number of bytes sent out.
   ------------------------------------------------------------------------*/
  return (length);

} /* dssudp_write() */

/*===========================================================================
FUNCTION DSSUDP_READ()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data in fromaddr.
  This function assumes that fromaddr is not NULL. 

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct iovec * iov,     /* scattered user buffer from where to copy data */
  uint16 iovcount,                                /* lenght of iovec array */
  struct sockaddr_in *fromaddr,                     /* destination address */
  sint15 *errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  uint16 bytes_read =0;                    /* # of bytes to read from rcvq */
  uint16 bytes_requested =0;  /* # of bytes to requested in each iteration */
  uint16 payload_len;                             /* Length of UDP payload */
  uint16 remaining_len;   /* remaining length of the datagram to be copied */
  uint16 read_cnt=0;                  /* # of bytes read in each iteration */
  uint16 cnt=0;                                /* tmp # of bytes retrieved */
  int i;                                          /* Local loop variable   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Ensure that the "from address" is not NULL.  This function fills in this
    structure, thus, needs to ASSERT against dereferencing a NULL pointer.
   ------------------------------------------------------------------------*/
  ASSERT( fromaddr != NULL);

  /*-------------------------------------------------------------------------
    Set the socket and UDP control block pointers, and set the family to
    AF_INET.
  -------------------------------------------------------------------------*/
  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
  fromaddr->sin_family = AF_INET;

  /*-------------------------------------------------------------------------
    Check if there is anything in the UDP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if (ucb_ptr->rcvcnt == 0)
  {
    *errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Extract the payload length, server's IP address, and port number 
    and store in fromaddr.  ASSERT that bytes to read, are equal to the
    number of bytes pulled up.
  -------------------------------------------------------------------------*/
  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &payload_len, sizeof(payload_len));
  INTFREE();

  ASSERT( cnt == sizeof(payload_len) );

  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &(fromaddr->sin_port), 
                   sizeof(fromaddr->sin_port));
  INTFREE();

  ASSERT( cnt == sizeof(fromaddr->sin_port) );

  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &(fromaddr->sin_addr), 
                   sizeof(fromaddr->sin_addr));
  INTFREE();

  ASSERT( cnt == sizeof(fromaddr->sin_addr) );

  remaining_len = MIN(payload_len, DSS_READ_MAX_BYTES);

  for(i=0; i < iovcount && remaining_len > 0; i++)
  { 
    /*-----------------------------------------------------------------------
      Extract the number of bytes which the application wants to read.
    -----------------------------------------------------------------------*/
    bytes_requested = MIN(remaining_len, iov[i].iov_len);
    if(bytes_requested > 0)
    {
      INTLOCK();
      read_cnt = dsm_pullup( &(ucb_ptr->rcvq), 
                             iov[i].iov_base, 
                             bytes_requested );
      ucb_ptr->rcvcnt -= read_cnt;
      INTFREE();

      ASSERT(read_cnt == bytes_requested);
      remaining_len   -= read_cnt;
      payload_len     -= read_cnt;
      bytes_read      += read_cnt;
    }
  }

  /*-------------------------------------------------------------------------
    If the supplied buffer is not large enough to hold the datagram, discard
    the remainder of the datagram and set errno.
  -------------------------------------------------------------------------*/
  if (payload_len > 0)
  {
    INTLOCK();
    read_cnt = dsm_pullup( &(ucb_ptr->rcvq), NULL, payload_len );
    ucb_ptr->rcvcnt -= read_cnt;
    INTFREE();

    ASSERT(read_cnt == payload_len);
    MSG_MED("Truncating %d bytes, sockfd=%d, supplied buffer too small", 
              payload_len, scb_ptr->sockfd, 0);
    *errno = DS_EMSGTRUNC;
  }

  /*-------------------------------------------------------------------------
    Check if there are any remaining UDP packets in the receive queue. Set
    the data_available flag to FALSE, if there are no remaining UDP packets.  
    Protect the access to the queue through INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  INTLOCK();
  if ( ucb_ptr->rcvcnt == 0 )
  {
    scb_ptr->data_available = FALSE;
  }
  INTFREE();

  /*-------------------------------------------------------------------------
    Convert the server IP address into the Network byte order. 
    Note, only the IP address is in host order, the port number is not.
  -------------------------------------------------------------------------*/
  (fromaddr->sin_addr).s_addr = dss_htonl( (fromaddr->sin_addr).s_addr);

  /*-------------------------------------------------------------------------
    Return the number of bytes read from the buffer.
  -------------------------------------------------------------------------*/
  return ( (sint15) bytes_read); 

} /* dssudp_read() */

/*===========================================================================
FUNCTION DSSUDP_READ_DSM_CHAIN()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data in fromaddr.
  This function assumes that fromaddr is not NULL. 

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_read_dsm_chain
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type** msg_ptr,
  struct sockaddr_in *fromaddr,                     /* destination address */
  sint15 *errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
  uint16 payload_len;                             /* Length of UDP payload */
  uint16 cnt=0;                                /* tmp # of bytes retrieved */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Ensure that the "from address" is not NULL.  This function fills in this
    structure, thus, needs to ASSERT against dereferencing a NULL pointer.
   ------------------------------------------------------------------------*/
  ASSERT( fromaddr != NULL);

  /*-------------------------------------------------------------------------
    Ensure that the dsm item is not NULL.
   ------------------------------------------------------------------------*/
  ASSERT( msg_ptr != NULL);

  /*-------------------------------------------------------------------------
    Set the socket and UDP control block pointers, and set the family to
    AF_INET.
   ------------------------------------------------------------------------*/
  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
  fromaddr->sin_family = AF_INET;

  /*-------------------------------------------------------------------------
    Check if there is anything in the UDP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if ( ucb_ptr->rcvcnt == 0 )
  {
    *errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Extract the payload length, server's IP address, and port number 
    and store in fromaddr.  ASSERT that bytes to read, are equal to the
    number of bytes pulled up.
  -------------------------------------------------------------------------*/
  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &payload_len, sizeof(payload_len));
  INTFREE();

  ASSERT( cnt == sizeof(payload_len) );

  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &(fromaddr->sin_port), 
                   sizeof(fromaddr->sin_port));
  INTFREE();

  ASSERT( cnt == sizeof(fromaddr->sin_port) );

  INTLOCK();
  cnt = dsm_pullup(&(ucb_ptr->rcvq), &(fromaddr->sin_addr), 
                   sizeof(fromaddr->sin_addr));
  INTFREE();

  ASSERT( cnt == sizeof(fromaddr->sin_addr) );

  /*-------------------------------------------------------------------------
    Extract the number of bytes which the application wants to read.
  -------------------------------------------------------------------------*/
  INTLOCK();

  *msg_ptr = ucb_ptr->rcvq;
  ucb_ptr->rcvcnt -= payload_len;

  /*-------------------------------------------------------------------------
    Check if there are any remaining UDP packets in the receive queue. Set
    the readable flag to FALSE, if there are no remaining UDP packets.  
    Access to global SCB array item is protected through 
    INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  if ( ucb_ptr->rcvcnt == 0 )
  {
    scb_ptr->data_available = FALSE;
  }

  INTFREE();

  /*-------------------------------------------------------------------------
    Convert the server IP address into the Network byte order. 
    Note, only the IP address is in host order, the port number is not.
  -------------------------------------------------------------------------*/
  (fromaddr->sin_addr).s_addr =  dss_htonl( (fromaddr->sin_addr).s_addr);

  /*-------------------------------------------------------------------------
    Return the number of bytes read from the buffer.
  -------------------------------------------------------------------------*/
  return (payload_len); 

} /* dssudp_read_dsm_chain() */

/*===========================================================================
FUNCTION DSSUDP_CLOSE()

DESCRIPTION
  This function is the UDP specific close() function.  It simply sets the
  socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.
                         
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Socket state is set to NULL.  Socket is freed and made available for
  re-use.
===========================================================================*/
sint15 dssudp_close
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *errno                                   /* error condition value */
)
{
  struct udp_cb *ucb_ptr;                    /* UDP protocol control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( errno != NULL );
  *errno = DSS_SUCCESS;

  ucb_ptr = (struct udp_cb *) scb_ptr->protocol_ctl_blk.ucb;

  /*-------------------------------------------------------------------------
    Free the receive queue, ucb and free the scb
  -------------------------------------------------------------------------*/
  INTLOCK();
  dsm_free_packet(&(ucb_ptr->rcvq));
  dssudpi_free_ucb(ucb_ptr);
  dssocki_freescb(scb_ptr);
  INTFREE();

  return (DSS_SUCCESS);

} /* dssudp_close() */

/*===========================================================================
FUNCTION DSSUDP_BIND()

DESCRIPTION
  This is the UDP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.
                         
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_bind
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct sockaddr_in *la     /* Ptr to the structure contains the local 
                                address.                                   */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set the conn.local.port valie to the port number passed in
  -------------------------------------------------------------------------*/
  scb_ptr->protocol_ctl_blk.ucb->conn.local.port = dss_ntohs(la->sin_port);
}


/*===========================================================================
FUNCTION DSSUDP_CONNECT_SETUP()

DESCRIPTION
  This function connects a udp socket by filling up the peer address and port
  in the udp control block. 

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.
  On failure returns DSS_ERROR

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssudp_connect_setup
(
  struct scb_s* scb_ptr,      /* Ptr to socket control block for the socket */
  sint15 *errno                    /* address of error condition value      */ 
)
{
  struct udp_cb* ucb_ptr;                      /* Ptr. to UDP control block */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - -*/
  MSG_MED(" Connecting UDP ", 0, 0, 0);
  
  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (scb_ptr == NULL) 
  {
    *errno = DS_EFAULT;
    return (DSS_ERROR);
  }
  
  /*-------------------------------------------------------------------------
    create the UDP control block (UCB).
  -------------------------------------------------------------------------*/
  ucb_ptr = scb_ptr->protocol_ctl_blk.ucb;
  ASSERT(ucb_ptr != NULL);  

  /*--------------------------------------------------------------------------
    if server address is null disconnect the socket.
  --------------------------------------------------------------------------*/
  if((scb_ptr->servname.address == 0) && (scb_ptr->servname.port == 0))
  {
    ucb_ptr->conn.remote.address = 0;
    ucb_ptr->conn.remote.port= 0;
    ucb_ptr->connected = FALSE;
    scb_ptr->localname.address = INADDR_ANY;
    return DSS_SUCCESS;  
  }

  /*-------------------------------------------------------------------------
    If address not bound to socket, assign it the local IP. Till we support
    binding to a particular interface we keep binding to new local ipaddress
    for each connect call.
  -------------------------------------------------------------------------*/
  ucb_ptr->conn.local.address = 
    PS_IFACE_IP_V4_ADDR(scb_ptr->routing_cache);
  scb_ptr->localname.address  = ucb_ptr->conn.local.address;
  /*-------------------------------------------------------------------------
    If socket is not already attached to a local address port, assign 
    ephemeral port.
  -------------------------------------------------------------------------*/
  if (scb_ptr->localname.port == 0)
  {
    scb_ptr->localname.port = dssocki_getephem_port(scb_ptr->protocol);
    scb_ptr->localname.port = dss_ntohs(scb_ptr->localname.port);
  }

  ucb_ptr->conn.local.port    = scb_ptr->localname.port;

  ucb_ptr->conn.remote  = scb_ptr->servname;
  ucb_ptr->connected    = TRUE;
  
  return(DSS_SUCCESS);

} /* dssudp_connect_setup() */

/*===========================================================================
FUNCTION DSSUDP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for udp sockets.

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
sint15 dssudp_shutdown
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dsm_item_type*  item_ptr,  /* dsm item containing the call specific info */
  sint15* dss_errno                                    /* dss error number */
)
{
  uint16 how;
  struct udp_cb *ucb_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( dss_errno != NULL );
  *dss_errno = DSS_SUCCESS;

  ucb_ptr = (struct udp_cb *) scb_ptr->protocol_ctl_blk.ucb;

  if ( dsm_pullup( &item_ptr, &how, sizeof(how)) != sizeof(how) )
  {
    MSG_ERROR("Error while pulling up, discarding packet", 0, 0, 0);
    dsm_free_packet(&item_ptr);
    return DSS_ERROR;
  }
 
  /*-----------------------------------------------------------------------
    Free the DSM packet since we have extracted the necessary information
    from it and have no more use of it.
  -----------------------------------------------------------------------*/
  dsm_free_packet(&item_ptr);

  /*-----------------------------------------------------------------------
    If the read-half needs to be shut down, clean-up the receive and the 
    resequencing queues. Note that shut down of read-half does not 
    result in any communication with the peer.
  -----------------------------------------------------------------------*/
  if ( how == DSS_SHUT_RD || how == DSS_SHUT_RDWR  ) 
  {
    /*---------------------------------------------------------------------
      Free the receive queue for the UDP control block.
    ---------------------------------------------------------------------*/
    INTLOCK();
    dsm_free_packet(&(ucb_ptr->rcvq));
    INTFREE();
  }
  /*-----------------------------------------------------------------------
    Inform the application of the socket event that the read-half/
    write-half is closed.
  -----------------------------------------------------------------------*/
  dssocki_socket_event_occurred( scb_ptr, NULL);

  return (DSS_SUCCESS);

} /* dssudp_shutdown() */


/*===========================================================================
FUNCTION DSSUDP_LOOKUP_CONN()

DESCRIPTION
  Given a connection structure, looks up in the ucb's to see if there is a 
  socket expecting packet from that source address.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP_CB if a socket is expecting the packet.
  If not, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct udp_cb *dssudp_lookup_conn
(
  struct connection *conn                    /* connection to be looked up */
)
{
  struct udp_cb *ucb_ptr = NULL;             /* UDP protocol control block */
  uint16            i;                       /* loop counter               */
  dword int_sav;         /* local variable used for saving interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Look up the connection and look at IP address and port numbers for
    both source and destination.  
  -------------------------------------------------------------------------*/
  for( i=0; i< DSS_MAX_UDP_SOCKS; i++)
  {
    INTLOCK_SAV(int_sav);
    if( ucb_array[i].scb_ptr != NULL)
    {
      ucb_ptr = &ucb_array[i];
      if ( (ucb_ptr->conn.local.address == conn->local.address ||
	    ucb_ptr->conn.local.address == INADDR_ANY) &&
	   (ucb_ptr->conn.local.port    == conn->local.port) &&
	   (ucb_ptr->connected == FALSE ||
	   (ucb_ptr->conn.remote.address == conn->remote.address &&
	    ucb_ptr->conn.remote.port == conn->remote.port )))
      {
	INTFREE_SAV(int_sav);
        return( ucb_ptr);
      }
    } /* if valid tcb */
    INTFREE_SAV(int_sav);
  } /* for */

  /*-------------------------------------------------------------------------
      Connection not found.
  -------------------------------------------------------------------------*/
  return( NULL);

} /* dssudp_lookup_conn() */

/*===========================================================================
FUNCTION DSSUDP_PROCESS_RX_PKT()

DESCRIPTION
  This function processes the passed UDP packet. 
  
  For socket calls, this function stores the packet in the UDP receive queue
  for the socket, and informs the application if it is waiting for the READ
  event.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_process_rx_pkt
( 
  struct udp_cb * ucb_ptr,                   /* UDP protocol control block */
  dsm_item_type * item_ptr,            /* Contains payload and socket info */
  uint32 udp_payload_len                         /* length of the item_ptr */
)
{
  struct scb_s * scb_ptr      = NULL;           /* Ptr to socket ctl block */
/*- - - - - - - - -  - - - - - - - - - - - - - - - - - -  - - - - - - - - -*/

  if ( ucb_ptr == NULL || ucb_ptr->scb_ptr == NULL )
  {
    MSG_HIGH("Dropping pkt, null control block", 0, 0, 0);   
    dsm_free_packet( &item_ptr);
    return;
  }
   
  scb_ptr = ucb_ptr->scb_ptr;

  if ((ucb_ptr->rcvcnt + udp_payload_len) > scb_ptr->rcvbuf)
  {
    MSG_HIGH("Dropping pkt for sock%d, buffer full", scb_ptr->sockfd, 0, 0);   
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if there's still memory in the dsm pool, this will prevent
    the receive queue on the icmp socket from becoming a bottleneck.
  -------------------------------------------------------------------------*/
  if (DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL)&&
      DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL))
  {
    MSG_HIGH("Dropping pkt for sock%d, no memory", scb_ptr->sockfd, 0, 0);   
    dsm_free_packet( &item_ptr);
    return;
  }

  if (scb_ptr->state_flags & DSSOCKI_CANTRECVMORE)
  {
    MSG_HIGH("Dropping pkt for sock%d, cantrecvmore", scb_ptr->sockfd, 0, 0);   
    dsm_free_packet( &item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Append the data item to the rcvq and set the data_available flag
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_UDP_RCVQ;
#endif //FEATURE_DSM_MEM_CHK

  /*-----------------------------------------------------------------------
    Note that the rcvcnt reflects only the length of the udp payload and 
    not the overhead information (source address, port and the payload 
    length) present in the dsm packet
  -----------------------------------------------------------------------*/
  INTLOCK();
  dsm_append(&(ucb_ptr->rcvq), &item_ptr);
  ucb_ptr->rcvcnt += udp_payload_len;
  scb_ptr->data_available = TRUE;
  INTFREE();
    
  dssocki_socket_event_occurred( scb_ptr, NULL);

  return;
} /* dssudp_process_rx_pkt() */

#endif /* FEATURE_DS_SOCKETS */

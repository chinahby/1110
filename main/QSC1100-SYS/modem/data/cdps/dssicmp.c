/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I C M P  F I L E
                   
DESCRIPTION

  The Data Services sockets ICMP interface file. This file contains all the 
  functions used by the sockets library to access ICMP socket function calls.


EXTERNALIZED FUNCTIONS
  
  dssicmp_socket()
    This function creates an ICMP socket. NOTE: This function is cannot be
    mapped by the protocol specific function pointer. To create an ICMP
    socket, this function must be invoked directly (instead of through
    dss_socket()). 
    
  dssicmp_read()
    Reads 'nbytes' bytes into the buffer from the ICMP transport.  Fills in
    address structure with values from who sent the data in fromaddr.
    This function asserts that fromaddr is not NULL. This function is the 
    ICMP specific dss_recvfrom()

  dssicmp_write()
    Send DSM item out to ICMP (write to ICMP socket). The actual write is
    performed in icmp_output() in icmp.c This function is the ICMP
    specific dss_sendto()
  
  dssicmp_close()
    This function is the ICMP specific dss_close() function.  It simply sets 
    the socket state to DSSOCKI_NULL and frees up the socket control block 
    and related data structures.
  
  dssicmp_ps_input()
    When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or 
    TIMESTAMP, the ICMP message will be handled by this fuction in the
    ps context. NOTE: the packet is not released in this should an 
    error occured. Instead DSS_ERROR is returned and the packet will 
    be freed by icmp_input(). 

Copyright (c) 2001-2002 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssicmp.c_v   1.2   26 Nov 2002 08:50:16   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssicmp.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/26/04    vp     Fixed the freeing of INTLOCK in dssicmp_ps_input when the 
                   function exits because of errors.
04/15/03    sv     Modified dssicmp_read to accept iov and iovcount instead
                   of buffer and bufferlen as arguments.
11/25/02    usb    Added routing cache for ICMP sockets
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/14/02    na     Fixed ip_addr_type to support FEATURE_DATA_MM.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
12/18/01    rsl    Updated tracer field.
11/21/01    aku    dssocki_socket_event_occurred() takes additional acb_ptr
                   arg
06/28/01    mvl    Modified msg levels to reduce extra debug chatter
05/10/01    na     Added check to see if scb ptr is valid before 
                   dereferencing in dssicmp_ps_input
03/27/01    mt     Changed instances of errno to dss_errno.  Changed second
                   parameter type of dssicmp_read() from byte * to void *.
03/21/01    js     Created module. 
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
#include "ps_icmp.h"
#include "ps_ip.h"
#include "memory.h"
#include "err.h"


/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Array of ICMP control blocks.
---------------------------------------------------------------------------*/
struct icmp_cb icmp_cb_array[DSS_MAX_ICMP_SOCKS];



/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION DSSICMPI_CREATE_ICMP_CB()

DESCRIPTION
  This function creates an ICMP control block upon the creation of an ICMP
  socket. 

DEPENDENCIES
  None. 

RETURN VALUE
  Returns a pointer to the ICMP control block; NULL if an error occured. 

SIDE EFFECTS
  None. 

===========================================================================*/
LOCAL struct icmp_cb *dssicmpi_create_icmp_cb
(
  struct scb_s *scb_ptr                        /* Ptr to socket ctl block. */
)
{
  int              index;               /* loop index                      */
  struct icmp_cb * icmp_cb_ptr = NULL;  /* Ptr to CB to be created         */
  uint32           int_sav;             /* used for saving interrupt state */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find a free CB. exit the loop when one comes up.
    INTLOCK because we don't want anyone to toch the icmp_cb_array. 
  -------------------------------------------------------------------------*/
  for (index = 0; index < DSS_MAX_ICMP_SOCKS; index++)
  {
    INTLOCK_SAV(int_sav);
    if (icmp_cb_array[index].scb_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        Assign icmp_cb_ptr.
      ---------------------------------------------------------------------*/
      icmp_cb_ptr = &icmp_cb_array[index];
      memset( icmp_cb_ptr, 0, sizeof( struct icmp_cb));
      icmp_cb_ptr->scb_ptr = scb_ptr;
      INTFREE_SAV(int_sav);
      break;
    }
    else
    {
      INTFREE_SAV(int_sav);
    }
  }

  /*-------------------------------------------------------------------------
    If all ICMP sockets are in use, return NULL.
   ------------------------------------------------------------------------*/
  if (index == DSS_MAX_ICMP_SOCKS)
  {
    return (NULL);
  }

  /*-------------------------------------------------------------------------
    Initialize values.
   ------------------------------------------------------------------------*/

  return (icmp_cb_ptr);

} /* dssicmpi_create_icmp_cb() */

/*===========================================================================

FUNCTION DSSICMPI_FREE_ICMP_CB()

DESCRIPTION
  This function releases an ICMP control block. It is called when an ICMP
  socket is being closed. 
  
DEPENDENCIES
  None. 

RETURN VALUE
  None. 

SIDE EFFECTS
  None. 
  
===========================================================================*/
LOCAL void dssicmpi_free_icmp_cb
(
  struct icmp_cb *icmp_cb_ptr     /* pointer to the ICMP cb to be released */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (icmp_cb_ptr != NULL);

  /*-------------------------------------------------------------------------
    Free the ucb_ptr and set everything to NULL
   ------------------------------------------------------------------------*/
  memset( icmp_cb_ptr, 0, sizeof( struct icmp_cb));

} /* dssicmpi_free_icmp_cb() */


/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSSICMP_SOCKET()

DESCRIPTION
  This function creates an ICMP socket. NOTE: This function is cannot be
  mapped by the protocol specific function pointer. To create an ICMP
  socket, this function must be invoked directly (instead of through
  dss_socket()). 

DEPENDENCIES
  None. 
    
PARAMETERS
  sint15 app_id     -  identification of the application requesting the 
                       creation of an ICMP socket      
  uint8  type       -  ICMP socket type
  uint8  code       -  ICMP socket code
  sint15 *dss_errno -  error condition value 

RETURN VALUE
  Errno Values
  ------------
  DS_EBADAPP          invalid application ID  
  DS_EMFILE           too many descriptors open.  A socket is already open or 
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.
  

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssicmp_socket
(
  sint15 app_id,                                      
  uint8  type,
  uint8  code,
  sint15 *dss_errno                               /* error condition value */
)
{
  struct scb_s *    scb_ptr;        /* Ptr to created socket control block */
  struct icmp_cb *  icmp_cb_ptr;
  sint15            temp_result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL) 
  {
    ASSERT(0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Allocate a socket control block. 
  -------------------------------------------------------------------------*/
  if ( (scb_ptr = dssocki_allocscb()) == NULL)
  {
    *dss_errno = DS_EMFILE;
    MSG_HIGH("Socket not allocated in dss_socket() call",0 ,0, 0);
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Fill in various fields of the socket control block. 
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Find application control blcok based on the app_id. Return DSS_ERROR if 
    invalid application ID. 
  -------------------------------------------------------------------------*/
  if ( (scb_ptr->acb_ptr = dssocki_appidtoacb(app_id)) == NULL )
  {
    dssocki_freescb(scb_ptr);
    *dss_errno = DS_EBADAPP;
    return (DSS_ERROR);
  }

  scb_ptr->type = SOCK_ICMP;  

  scb_ptr->protocol = IPPROTO_ICMP;

  /*-------------------------------------------------------------------------
    Set up protocol specific function pointer table
  -------------------------------------------------------------------------*/
  temp_result = dssocki_fcnptr_setup(scb_ptr, dss_errno);
  if (temp_result == DSS_ERROR)
  {
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  } 

  /*-------------------------------------------------------------------------
    Set up the socket's routing cache (avoiding later lookups in the
    ACB if the application already did early routing).
  -------------------------------------------------------------------------*/
  if ( scb_ptr->acb_ptr )
  {
    scb_ptr->routing_cache = scb_ptr->acb_ptr->ps_iface_ptr;
  }
  
  MSG_HIGH("Socket(): Setting tx cache to 0x%x",
           scb_ptr->routing_cache, 0, 0 );

  /*-------------------------------------------------------------------------
    protocol socket-specific creation process 
  -------------------------------------------------------------------------*/
  if ((icmp_cb_ptr = dssicmpi_create_icmp_cb(scb_ptr)) == NULL ) 
  {
    *dss_errno = DS_EMFILE;
    dssocki_freescb(scb_ptr);
    return (DSS_ERROR);
  }

  dssocki_set_sock_state(scb_ptr, DSSOCKI_OPEN);

 /*-------------------------------------------------------------------------
    write type and code into the icmp control block 
  -------------------------------------------------------------------------*/
  icmp_cb_ptr->type = type;
  icmp_cb_ptr->code = code;

  /*-------------------------------------------------------------------------
    Initialize the receive queue and set it in the socket control block.
  ------------------------------------------------------------------------*/
  (void) q_init ( &(icmp_cb_ptr->rcvq) );
  scb_ptr->protocol_ctl_blk.icb = icmp_cb_ptr;

  ASSERT (  *dss_errno == DSS_SUCCESS );

  /*-------------------------------------------------------------------------
    Return valid socket descriptor . Done!
  ------------------------------------------------------------------------*/
  MSG_LOW("ICMP SOCKET CREATED!", 0, 0, 0);

  return (scb_ptr->sockfd);
} /* dssicmp_socket() */


/*===========================================================================

FUNCTION DSSICMP_WRITE()

DESCRIPTION
  Send DSM item out to ICMP (write to ICMP socket). The actual write is
  performed in icmp_output() in icmp.c This function is the ICMP
  specific dss_sendto()

DEPENDENCIES
  None. 

PARAMETERS  
  protocol_ctl_block_type pcb       -   protocol control block 
  struct sockaddr_in      *ta_ptr   -   destination socket info 
  dsm_item_type           *item_ptr -  ptr to head of dsm memory pool items 

RETURN VALUE
  Length of item written. 

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssicmp_write
(
  protocol_ctl_block_type pcb,                   /* protocol control block */
  struct sockaddr_in      *ta_ptr,              /* destination socket info */
  dsm_item_type           *item_ptr,/*ptr to head of dsm memory pool items */
  ps_meta_info_type *meta_info_ptr              /* ptr to meta info        */
)
{
  struct icmp_cb *  icmp_cb_ptr;
  uint32  *dest_ip_ptr;                          /* Ptr to dest IP address */
  struct scb_s *    scb_ptr;
  struct icmp       icmp_hdr;
  uint16            length;                                    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW("In dssicmp_write (@ %p)", dssicmp_write, 0, 0);

  /*-------------------------------------------------------------------------
    Since the meta infomation is not used, free the item
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(&meta_info_ptr);

  /*-------------------------------------------------------------------------
    Setup the socket and ICMP control block pointers.
  ------------------------------------------------------------------------*/
  icmp_cb_ptr = pcb.icb;
  scb_ptr = icmp_cb_ptr->scb_ptr;

  /*-------------------------------------------------------------------------
    Use destination information from dsm_item chain. If not possible. 
    use the information in scb (may be stale).                        
  ------------------------------------------------------------------------*/
  if (ta_ptr != NULL) 
  {
    dest_ip_ptr = &(ta_ptr->sin_addr.s_addr);
    MSG_LOW("Using parameter ta_ptr", 0, 0, 0);
  }
  else
  {
    dest_ip_ptr = &(scb_ptr->servname.address); 
    MSG_LOW("Using the server addr in SCB", 0, 0, 0);
  }
  MSG_LOW("destination IP address: %x", *dest_ip_ptr, 0, 0);

  /*-------------------------------------------------------------------------
    Fill in all the known header fields, but not the checksum.
  ------------------------------------------------------------------------*/
  icmp_hdr.info.type = icmp_cb_ptr->type;
  icmp_hdr.info.code = icmp_cb_ptr->code;

  MSG_LOW("ICMP header successfully assembled", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Collect length information and pass it down to icmp_output(). 
  ------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Call icmp_output, which will make up the icmp header and send the 
    packet to IP.
  -------------------------------------------------------------------------*/
  length = dsm_length_packet(item_ptr);

  MSG_LOW("calling icmp_output(%d bytes), item length = %d", 
          length, 
          item_ptr->used, 
          0);
#ifdef FEATURE_DSM_MEM_CHK
  item_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK
  icmp_output( dest_ip_ptr, &icmp_hdr, item_ptr, icmp_cb_ptr, 0);

  /*-------------------------------------------------------------------------
    Return number of bytes sent out.
  ------------------------------------------------------------------------*/
  return (length);
}/* dssicmp_write() */



/*===========================================================================

FUNCTION DSSICMP_READ()

DESCRIPTION
  Reads 'nbytes' bytes into the buffer from the ICMP transport.  Fills in
  address structure with values from who sent the data in fromaddr.
  This function asserts that fromaddr is not NULL. This function is the 
  ICMP specific dss_recvfrom()

DEPENDENCIES
  None. 

PARAMETERS
  struct scb_s* scb_ptr      -  Ptr to socket control block for the socket
  void   *buffer             -  user buffer from which to copy the data 
  uint16 nbytes              -  number of bytes app wants to read
  struct sockaddr_in *fromaddr  -  source address 
  sint15 *dss_errno          -  error condition value 

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  Errno Values
  ------------
  DS_EWOULDBLOCK      operation would block

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssicmp_read
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  struct iovec * iov,         /* user buffer from which to copy the data   */
  uint16         iovcount,     /* length of the iovec array                */
  struct sockaddr_in *fromaddr,                          /* source address */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct icmp_cb *icmp_cb_ptr;              /* ICMP protocol control block */
  dsm_item_type *item_ptr;         /* ptr to head of dsm memory pool items */
  uint16 cnt=0;                                /* tmp # of bytes retrieved */
  uint16 bytes_read =0;                    /* # of bytes to read from rcvq */
  uint16 read_cnt=0;              /* # of bytes read in each iteration     */
  uint16 payload_len=0;                        /* packet length            */
  int    i;                                  /* local loop index           */
  uint16 bytes_requested;        /* # of bytes requested in each iteration */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_LOW("In dssicmp_read()", 0, 0, 0);
  /*-------------------------------------------------------------------------
    Ensure that the "from address" is not NULL.  This function fills in this
    structure, thus, needs to ASSERT against dereferencing a NULL pointer.
  ------------------------------------------------------------------------*/
  ASSERT( fromaddr != NULL);

  /*-------------------------------------------------------------------------
    Set the socket and ICMP control block pointers, and set the family to
    AF_INET.
  -------------------------------------------------------------------------*/
  icmp_cb_ptr = scb_ptr->protocol_ctl_blk.icb;
  fromaddr->sin_family = AF_INET;
  MSG_LOW("ICMP CB pointer successfully set", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Check if there is anything in the ICMP receive queue. If not, return
    DS_EWOULDBLOCK.
  -------------------------------------------------------------------------*/
  if ( (item_ptr = (dsm_item_type *) q_get( (&(icmp_cb_ptr->rcvq)))) == NULL)
  {
    MSG_LOW("Nothing on ICMP revq", 0, 0, 0);
    *dss_errno = DS_EWOULDBLOCK;
    return (DSS_ERROR);
  }
  MSG_LOW("There is indeed something on the ICMP rcvq", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Extract the payload length, server's IP address and store in fromaddr.  
    ASSERT that bytes to read, are equal to the number of bytes pulled up.
    The port field will be undefined in the fromaddr structure returned to
    the application. 
  -------------------------------------------------------------------------*/
  cnt = dsm_pullup(&item_ptr, &(fromaddr->sin_addr), 
                   sizeof(fromaddr->sin_addr));
  ASSERT( cnt == sizeof(fromaddr->sin_addr) );
  MSG_MED("extracted server's IP %x", fromaddr->sin_addr.s_addr, 0, 0);

  /*-------------------------------------------------------------------------
    Extract the number of bytes which the application wants to read.
  -------------------------------------------------------------------------*/
  payload_len = dsm_length_packet( item_ptr );
  for(i=0;i< iovcount && payload_len > 0 ; i++)
  { 
    /*-----------------------------------------------------------------------
      Extract the number of bytes which the application wants to read.
      -----------------------------------------------------------------------*/
    bytes_requested = MIN( payload_len, iov[i].iov_len);
    if(bytes_requested > 0)
    {
      read_cnt = dsm_pullup( &(item_ptr), iov[i].iov_base, bytes_requested);
    }

    /*-----------------------------------------------------------------------
      ASSERT that we read the expected number of bytes from the buffer.
      -----------------------------------------------------------------------*/
    ASSERT(read_cnt == bytes_requested);
    payload_len -= read_cnt;
    bytes_read += read_cnt;
    read_cnt = 0;
  }
 

  if (payload_len > 0 ) 
  {
    ERR("User provided buffer is smaller than received datagram (%d bytes)",
	bytes_read + payload_len, 0, 0);
  }

  MSG_LOW("Successfully read nbytes of data in DSSICMP_READ", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Free the packet in dsm buffer. 
  -------------------------------------------------------------------------*/
  dsm_free_packet ( &item_ptr );
  MSG_LOW("Packet is successfully freed in DSSICMP_READ", 0, 0, 0);
  
  /*-------------------------------------------------------------------------
    Check if there are any remaining ICMP packets in the receive queue. Set
    the readable flag to FALSE, if there are no remaining ICMP packets.  
    Access to global SCB array item is protected through 
    INTLOCK()/INTFREE().
  -------------------------------------------------------------------------*/
  if ( (item_ptr = (dsm_item_type *) q_check( &(icmp_cb_ptr->rcvq))) == NULL)
  {
    INTLOCK();
    scb_ptr->data_available = FALSE;
    INTFREE();
  }

  /*-------------------------------------------------------------------------
    Convert the server IP address into the Network byte order. 
    Note, only the IP address is in host order, the port number is not.
  -------------------------------------------------------------------------*/
  (fromaddr->sin_addr).s_addr =  dss_htonl( (fromaddr->sin_addr).s_addr);

  /*-------------------------------------------------------------------------
    Return the number of bytes read from the buffer.
  -------------------------------------------------------------------------*/
  MSG_LOW("DONE ICMPREAD", 0, 0, 0);
  return ( (sint15) bytes_read); 

} /* dssudp_read() */


/*===========================================================================

FUNCTION DSSICMP_CLOSE()

DESCRIPTION
  This function is the ICMP specific dss_close() function.  It simply sets 
  the socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise. 

SIDE EFFECTS
  None. 
  
===========================================================================*/
extern sint15 dssicmp_close
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  sint15 *dss_errno                               /* error condition value */
)
{
  struct icmp_cb *icmp_cb_ptr;              /* ICMP protocol control block */
  dsm_item_type *item_ptr;                           /* temporary item ptr */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
    if (dss_errno == NULL) 
    {
      ASSERT(0);
      return(DSS_ERROR);
    }
    *dss_errno = DSS_SUCCESS;

    icmp_cb_ptr = (struct icmp_cb *) scb_ptr->protocol_ctl_blk.icb;

    /*-------------------------------------------------------------------------
      Free the receive queue for the ICMP control block.
     ------------------------------------------------------------------------*/
    while(( item_ptr = (dsm_item_type *) q_get( &(icmp_cb_ptr->rcvq))) != NULL)
    {
      dsm_free_packet(&item_ptr);
    }

    /*-------------------------------------------------------------------------
      Free the ICMP control block.
     ------------------------------------------------------------------------*/
    dssicmpi_free_icmp_cb(icmp_cb_ptr);

    /*-------------------------------------------------------------------------
      Free the socket control block.
    ------------------------------------------------------------------------*/
    dssocki_freescb(scb_ptr);

    return (DSS_SUCCESS);
} /* dssicmp_close () */



/*===========================================================================
FUNCTION DSSICMP_PS_INPUT()

DESCRIPTION
  When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or 
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context. NOTE: the packet is not released in this should an 
  error occured. Instead DSS_ERROR is returned and the packet will 
  be freed by icmp_input(). 
                         
DEPENDENCIES
  None. 

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise. 

SIDE EFFECTS
  None. 
  
===========================================================================*/
extern sint15 dssicmp_ps_input
( 
  struct icmp           *icmp_hdr,               /* ptr to the icmp header */
  dsm_item_type         *pkt_ptr,          /* ptr to the arriving ICMP pkt */
  ip4a                  *ip_src_addr                     /* source address */
)
{
  struct scb_s *  scb_ptr;                  /* Ptr to socket control block */
  int             index;                                     /* loop index */
  uint32          int_sav;       /* temporary variable for interrupt state */
  boolean         success;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Looping through the array of open ICMP socket control blocks and find
    whose type/code filed matches that of icmp_hdr. 
  -------------------------------------------------------------------------*/
  success = FALSE; 
  for (index = 0; index < DSS_MAX_ICMP_SOCKS; index++)
  {
    INTLOCK_SAV(int_sav);
    /*----------------------------------------------------------------------- 
      Only the sockets in OPEN state are being considered. 
    -----------------------------------------------------------------------*/
    scb_ptr = icmp_cb_array[index].scb_ptr;
    if(( scb_ptr != NULL) && (scb_ptr->socket_state == DSSOCKI_OPEN))
    {
      /*---------------------------------------------------------------------
        Check for matching type filed. NOTE: !! By default, code filed is
        not filtered. 
      ---------------------------------------------------------------------*/
      if ( icmp_cb_array[index].type == icmp_hdr->info.type ) 
      {
        /*--------------------------------------------------------------------
          Check if there's still memory in the dsm pool, this will prevent
          the receive queue on the icmp socket from becoming a bottleneck. 
        --------------------------------------------------------------------*/
        if (((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) == TRUE) ||
            ((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL) == TRUE) )))
        {
          INTFREE_SAV(int_sav);
          MSG_HIGH("Low in memory: Discarding the ICMP packet", 0,0,0);
          break;
        }

        /*--------------------------------------------------------------------
          Push the IP source address, on the item ptr. This informationis 
          required by the application layer.
        --------------------------------------------------------------------*/
        if (dsm_pushdown( &pkt_ptr, 
                          ip_src_addr, 
                          4,
                          DSM_DS_SMALL_ITEM_POOL) == FALSE)
        {
          INTFREE_SAV(int_sav);
          ERR("Out of Memory!",0,0,0);
          break;
        }

        /*--------------------------------------------------------------------
          Put the packet on queue. 
        --------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
	pkt_ptr->tracer = DSM_ICMP_RCVQ;
#endif //FEATURE_DSM_MEM_CHK

	q_put( &(icmp_cb_array[index].rcvq), &(pkt_ptr->link) );
        
        /*-------------------------------------------------------------------
          There are bytes in the data payload, so indicate that a socket 
          event has occurred.  
        -------------------------------------------------------------------*/
        icmp_cb_array[index].scb_ptr->data_available = TRUE;
        INTFREE_SAV(int_sav);

        dssocki_socket_event_occurred( icmp_cb_array[index].scb_ptr, NULL );


        success = TRUE;
        break;
      } /* if (matched type) */
      else
      {
        INTFREE_SAV(int_sav);
      }
    } /* if (OPEN socket) */
    else
    {
      INTFREE_SAV(int_sav);
    }
  } /* for (all ICMP sockets) */

  /*-------------------------------------------------------------------------
    Process return value. 
  -------------------------------------------------------------------------*/
  if (success == 1) 
  {
    return (DSS_SUCCESS);
  }
  else
  {
    return (DSS_ERROR);
  }
}/* dssicmp_ps_input () */

#endif /* FEATURE_DS_SOCKETS */

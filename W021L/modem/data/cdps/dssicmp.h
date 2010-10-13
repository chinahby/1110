#ifndef DSSICMP_H
#define DSSICMP_H
/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I C M P  F I L E
                   
DESCRIPTION

 The Data Services sockets ICMP interface file. This contains all the 
 functions used by the sockets library to access ICMP socket function calls.

EXTERNALIZED FUNCTIONS

  dssicmp_socket()
    This function creates an ICMP socket. NOTE: This function is cannot be
    mapped by the protocol specific function pointer. To create an ICMP
    socket, this function must be invoked directly (instead of through
    dss_socket()). 

  dssicmp_write()
    Send DSM item out to ICMP (write to ICMP socket). The actual write is
    performed in icmp_output() in icmp.c This function is the ICMP
    specific dss_sendto()

  dssicmp_read()
    Reads 'nbytes' bytes into the buffer from the ICMP transport.  Fills in
    address structure with values from who sent the data in fromaddr.
    This function asserts that fromaddr is not NULL. This function is the 
    ICMP specific dss_recvfrom()

  dssicmp_close()
    This function is the ICMP specific dss_close() function.  It simply sets 
    the socket state to DSSOCKI_NULL and frees up the socket control block 
    and related data structures.
  
  dssicmp_ps_input ()
    When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or 
    TIMESTAMP, the ICMP message will be handled by this fuction in the
    ps context. 

    

    
Copyright (c) 2001-2002 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/15/03    sv     Modified dssicmp_read to accept iov and iovcount as
                   arguments instead of buffer and bufferlen.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
05/02/02    mvl    File clean up, added meta_info_ptr to write()
03/27/01    mt     Changed instances of errno to dss_errno.  Changed second
                   parameter type of dssicmp_read() from byte * to void *.
03/21/01    js     Created module. 
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "ps_icmp.h"
#include "dssocki.h"

#ifdef FEATURE_DS_SOCKETS

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS
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
  dss_errno Values
  ----------------
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
  uint8 type,
  uint8 code,
  sint15 *dss_errno                               /* error condition value */
);

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
);

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

  dss_errno Values
  ----------------
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
);

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
);



/*===========================================================================
FUNCTION DSSICMP_PS_INPUT()

DESCRIPTION
  When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or 
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context. 
                         
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
  dsm_item_type         *pkt_ptr,               /* ptr to the arriving pkt */
  ip4a                  *ip_src_addr                     /* source address */
);


#endif /* FEATURE_DS_SOCKETS */

#endif /* DSSICMP_H */


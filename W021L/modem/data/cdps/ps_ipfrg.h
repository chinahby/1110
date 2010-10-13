#ifndef PS_IPFRG_H
#define PS_IPFRG_H
/*===========================================================================

                       I P   F R A G M E N T A T I O N

                               R O U T I N E S

DESCRIPTION

  Copyright (c) 1995-2003 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ipfrg.h_v   1.1   12 Feb 2003 14:37:26   mlioy  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ipfrg.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/11/03    mvl    Added include for IP header as it is needed.
07/28/02    om     Moved fragmentation routines into separate file.
===========================================================================*/


/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "ps_iphdr.h"

/*===========================================================================

                            DEFINITIONS AND TYPES

===========================================================================*/
/*---------------------------------------------------------------------------
  The number of elements in the reassembly structure. This value gives the
  total number of fragmented-datagrams that can be handled at a given time.
  
  Note that, if more than 2 datagrams may be fragmented at a given time, this
  should be increased.
---------------------------------------------------------------------------*/
#define NUM_DATA_FRAG     2

/*---------------------------------------------------------------------------
  ip_reasm_type is a structure used for storing the information about the 
  datagram and its fragments during the reassembling of the ip fragments.
  This structure contains:
  
    Queue to store the IP datagrams. Each of the queue will be used for a
    single datagram. This is a queue of the dsm_item_types. Each of the
    elements of this queue will be a fragment received. The fragments are 
    stored in the queue and after receiving all the fragments, they are made
    into a single element.
    
    IP Header is used to store the IP header that should be passed to the 
    upper layers. Note that each of the fragments will have the IP header,
    but standard (RFC) mentions that only the IP header of the first fragment
    i.e. offset = 0, should contain all the options. So, we need to pass only
    the header of the first offset to upper layers.
    
    in_use flag is used to indicate that the current element of the 
    reassembly structure/array is being used. 
    
    datagram_len gives the total length of the datagram length i.e. sum of 
    all the fragments that will be received. The value to be stored here is
    obtained by calculating the length from the last offset ( value of the
    last offset + length of the last fragment).
    
    Rcvd_bytes is used to calculate the number of the bytes received in the
    fragments for a particular datagram.
---------------------------------------------------------------------------*/
struct ip_reasm_type 
{
  q_type *fraglist_q;       /* Points to the datagrams                     */
  struct ip  ip;            /* Extracted IP header                         */
  boolean   in_use;         /* current element is used                     */
  uint16     datagram_len;  /* length of datagram. All fragments           */
  uint16     rcvd_bytes;    /* Bytes received till now                     */
};


/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

extern struct iplink Iplink[];

/*---------------------------------------------------------------------------
  ip_reasm_array is used for accessing the datagram information.
---------------------------------------------------------------------------*/
extern struct ip_reasm_type ip_reasm_array[NUM_DATA_FRAG];


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION IP_FRAGMENT_HANDLER()

DESCRIPTION
  Process IP datagram fragments. If datagram is complete, returns 
  address of the datagram else returns NULL.

DEPENDENCIES

RETURN VALUE
If datagram is complete, returns address of the datagram else returns NULL.

SIDE EFFECTS
===========================================================================*/
struct dsm_item_s *ip_fragment_handler
(
  struct ip  *ip,
  struct dsm_item_s *bp 
);

/*===========================================================================
FUNCTION IP_LOOKUP_REASM()

DESCRIPTION
  This function will look for the element in the re-assembly descriptor 
  array, that contains the same identifier, source, destination and protocol
  as the incoming packet. If a match for the parameters exist, then the 
  address of the array element is returned. If match is not found then NULL 
  is returned.

DEPENDENCIES
  None.

RETURN VALUE
  If a match for the parameters exist, then the address of the array element 
  is returned. If match is not found then NULL is returned.

SIDE EFFECTS
  None
===========================================================================*/
struct ip_reasm_type *ip_lookup_reasm
(
  struct ip *ip
);

/*===========================================================================
FUNCTION IP_CREATE_REASM()

DESCRIPTION
  This function will create a reassembly descriptor and it will be used to 
  store the information of each IP packet.

DEPENDENCIES
  

RETURN VALUE
  The address of the free element of the reassembly structure.

SIDE EFFECTS
  None
===========================================================================*/
struct ip_reasm_type *ip_create_reasm
(
  struct ip *ip
);

/*===========================================================================
FUNCTION IP_FREE_REASM()

DESCRIPTION
  This function will free a reassembly descriptor and it will be used to 
  store the information of each IP packet.

DEPENDENCIES

RETURN VALUE
 
SIDE EFFECTS
  None
===========================================================================*/
void ip_free_reasm
(
  struct ip_reasm_type *rp
);

/*===========================================================================
FUNCTION IP_REASM_INIT()

DESCRIPTION
  This function will initialize the reassembly data structures.

DEPENDENCIES

RETURN VALUE
 
SIDE EFFECTS
  None
===========================================================================*/
void ip_reasm_init( void );

#endif  /* PS_IPFRG_H */

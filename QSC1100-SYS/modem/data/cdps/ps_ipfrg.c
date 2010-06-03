/*===========================================================================

                    I P   F R A G M E N T A T I O N

                            R O U T I N E S 

DESCRIPTION

   Copyright (c) 1995-2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ipfrg.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ipfrg.c#2 $ $DateTime: 2008/02/28 05:25:49 $ $Author: praveenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/08   pp      Fixed lint critical errors.
07/28/02   om      Moved fragmentation routines into separate file.

===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */

#include "msg.h"
#include "err.h"
#include "assert.h"

#include "dsm.h"
#include "ps_iface.h"
#include "ps_tcp.h"

#include "ps_ip.h"
#include "ps_iputil.h"
#include "ps_iphdr.h"
#include "ps_ipfrg.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  ip_reasm_type is a structure used for storing the information about the 
  datagram and its fragments during the reassembling of the ip fragments.
  This structure contains:
  
    Queue to store the IP datagrams.     
    IP Header is used to store the IP header that should be passed to the 
              upper layers. 
    in_use flag 
    datagram_len 
    Rcvd_bytes
    
  NUM_DATA_FRAG The number of elements in the reassembly structure. 
  This value gives the total number of fragmented-datagrams that can be 
  handled at a given time.
---------------------------------------------------------------------------*/
struct ip_reasm_type ip_reasm_array[NUM_DATA_FRAG];

/*---------------------------------------------------------------------------
  The queue that holds the reassembly fragments.
---------------------------------------------------------------------------*/
q_type re_asm_q[NUM_DATA_FRAG];


/*===========================================================================

                    PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IP_FRAGMENT_HANDLER()

DESCRIPTION
  Process IP datagram fragments. If datagram is complete, returns address
  of the datagram else returns NULL.
  
  Each datagram is stored in a queue. On receiving the fragments of the 
  datagram, they are appended to the queue. Parallely we can handle 
  NUM_DATA_FRAG number of datagrams.
  
  After receiving each fragment, check the number of bytes received in this
  datagram to the total number of data bytes in the datagram. If all the
  expected bytes are received, then pull them out of the queue and append
  into a single item. Pass the single item to the next layer.
   

DEPENDENCIES
   WE ASSUME THAT OVERLAP DOES NOT OCCUR between fragments. Overlaps
   cannot be handled.

RETURN VALUE
  If datagram is complete, returns address of the datagram else returns NULL.

SIDE EFFECTS

===========================================================================*/
struct dsm_item_s *ip_fragment_handler
(
  struct ip *ip,              /* IP header                                 */
  struct dsm_item_s *bp       /* The fragment itself                       */
)
{
  /*-------------------------------------------------------------------------
    Pointer to reassembly descriptor 
  -------------------------------------------------------------------------*/
  struct ip_reasm_type *rp;         

  /*-------------------------------------------------------------------------
    Pointers to data item. They are used for storing intermittent results
    of traversing through the queue.
  -------------------------------------------------------------------------*/
  dsm_item_type *lastfrag_link,*nextfrag_link, *auxfrag_ptr;

  /*-------------------------------------------------------------------------
    Contains the offset of the fragment.
  -------------------------------------------------------------------------*/
  uint16 next_offset;
    
  /*-------------------------------------------------------------------------
    Contains the length of the data in the fragment. The Maximum length is
    2^16 - (20bytes).
  -------------------------------------------------------------------------*/
  uint16 frag_len;            
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

//    ASSERT(&bp == NULL);
//    ASSERT(&ip == NULL);

  /*-------------------------------------------------------------------------
    Obtain the reassembly_pointer from the look up function. This function 
    will check for the id, source, destination and protocol and returns the 
    reassembly_pointer. 
      
    rp is now pointing to the fragments of the datagram we are currently 
    interested in. 
      
    if we received a new datagram, then rp is NULL and it will be assigned 
    a value in ip_create_reasm. Note that each datagram is uniquely 
    identified with the source, destination, protocol and identifier.
     
    If new entry cannot be obtained, then thrash existing oldest one or 
    return a NULL so that upper layers decide. In current implementation
    we thrash oldest one. In future, we may allocate dynamically and return
    a NULL.

  -------------------------------------------------------------------------*/
        
  if((rp = ip_lookup_reasm(ip)) == NULL)
  {
    if((rp = ip_create_reasm(ip)) == NULL)
    {
    /*-----------------------------------------------------------------------
      Currently code will never come here but in future when we have
      dynamic allocation of reassembly descriptors, this code will be 
      useful.
      No space for descriptor, drop fragment 
      
      Append following code in future:
      
      MSG_HIGH("Donot have enough memory for IP fragments",0,0,0);
      ipReasmFails++;
      dsm_free_buffer(bp);
    -----------------------------------------------------------------------*/
      return NULL;
    }
  }

  /*-------------------------------------------------------------------------
    Update the statistics
  -------------------------------------------------------------------------*/
  ipReasmReqds++;

  /*-------------------------------------------------------------------------
    frag_len is the length of the data received in current IP fragment. 
    Update the total received bytes, rp->rcvd_bytes by adding length of 
    new fragment.
    Note that rp->rcvd_bytes is initialized to zero every time we create
    a new descriptor.
  -------------------------------------------------------------------------*/
  frag_len = ip->length - (IPLEN + ip->optlen);
  rp->rcvd_bytes += frag_len;

  MSG_HIGH("Received %d bytes in current Fragment", frag_len,0,0);

  /*-------------------------------------------------------------------------
    If this is the last fragment of the data gram, then the total length of 
    the data gram can be obtained by adding the offset with length of the 
    data. Since, ip->flags.mf equals zero only if it is last fragment 
    (otherwise, control does not come this far). So, use this information 
    and fill up the total datagram length.

    Note that the rp->datagram_len should only be written with a Non-zero 
    number ONLY if "more fragments" flag == 0. This is because of some 
    checking that we do below. Also, note that everytime the datagram 
    descriptor is freed, this variable should be made as zero. 
  -------------------------------------------------------------------------*/
  if(ip->flags.mf == 0)
     rp->datagram_len = ip->offset + frag_len;

  /*-------------------------------------------------------------------------
    Store the offset by copying it to the data area of the dsm_item. 
    Since the ip->offset is 13bits, the sizeof(ip->offset) operator should 
    give 2 bytes. 
    Note that, the length of the dsm_item is now modified by 2 so using it 
    to calculate the size of the length of the fragment, will be incorrect.
  -------------------------------------------------------------------------*/
  if (dsm_pushdown( &bp, (void*)&(ip->offset),sizeof(ip->offset), 
                           DSM_DS_SMALL_ITEM_POOL ) <
      sizeof(ip->offset))
  {
    /*-----------------------------------------------------------------------
      Free up the incoming fragment and abort reassembly   
    -----------------------------------------------------------------------*/
    ERR("Running out of buffer",0,0,0);
    dsm_free_packet( &bp );
    return NULL; 
  }

  /*-------------------------------------------------------------------------
    Now locate the fragments before and after the current data fragment we 
    received so that we can insert the new fragment in between them. This 
    is obtained by finding the offset of the current data fragment and 
    comparing with the offsets of the fragments received earlier.
      
    lastfrag_link will point to the fragment before the new (now 
    received data) fragment while nextfrag_link points to fragment after
    the lastfrag_link.
      
    At the end of the following loop, the current fragment will be inserted
    between the fragments.
  -------------------------------------------------------------------------*/
  lastfrag_link = NULL;
  for(nextfrag_link = (dsm_item_type*)q_check(rp->fraglist_q);
      nextfrag_link != NULL;
      nextfrag_link = 
           (dsm_item_type*)q_next(rp->fraglist_q,&nextfrag_link->link)
     )
  {
    /*-----------------------------------------------------------------------
      data_ptr + 0 contains the Offset.
      Use auxfrag_ptr to avoid dsm_extract from changing any pointers.
      
      Check the offset stored and current offset.
      If ( Value of stored offset > value of received offset)
         We received fragment that should be appended before the stored
         packet.
         
      If (Value of stored offset < value of received offset)
         We received a fragment that should be appended below (may not be
         immediately below) the stored fragment.
         
      Do the above check until, we find where the new fragment fits.
         
    -----------------------------------------------------------------------*/
    auxfrag_ptr = nextfrag_link;
    dsm_extract((dsm_item_type*)auxfrag_ptr,0,
                 &next_offset,
                 sizeof(ip->offset));
    if( next_offset > ip->offset)
        break;
    lastfrag_link = nextfrag_link;
  } /* End of the For loop                                                 */


  /*-------------------------------------------------------------------------
    Lastfrag now points to the fragment before the new fragment received;  
    nextfrag points at the next fragment of the new fragment. Insert the 
    new fragment between the lastfrag and nextfrag.
      
    If this is the first fragment append it at the beginning of the queue 
    else append it in between. Note that q_insert cannot be used to insert
    in an empty queue or at the end of the queue, so use q_put().
  -------------------------------------------------------------------------*/
  if(nextfrag_link == NULL)
  {
    q_put( rp->fraglist_q,         /* Ptr to the queue               */ 
           &(bp->link)             /* Ptr to link of item to insert  */
         );
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR             
    q_insert( rp->fraglist_q,      /* Ptr to the queue               */ 
              &(bp->link),         /* Ptr to link of item to insert  */
              &nextfrag_link->link /* Insert before this Ptr         */
            );
#else
    q_insert( &(bp->link),         /* Ptr to link of item to insert  */
              &nextfrag_link->link /* Insert before this Ptr         */
            );
#endif
  }
  /*-------------------------------------------------------------------------
    Check if all the fragments are received or not. This is known if the 
    total length of the datagram is same as the received  number of bytes. 
    Note that WE ASSUME THAT OVERLAP DOES NOT OCCUR.
      
    The datagram_len will be non-zero only if the last fragment of the 
    datagram is received. 
      
    If all the fragments are received, then append them all into a single 
    packet chain and pass it on to the upper layers.
  -------------------------------------------------------------------------*/
  if((rp->datagram_len != 0)&&(rp->datagram_len==rp->rcvd_bytes))
    {
      /*---------------------------------------------------------------------
        If we received more bytes than what we expect to receive, so overlap
        occured. The current implementation of the code does not expect 
        overlaps to occur. So we donot handle it.
      ---------------------------------------------------------------------*/
      if(rp->datagram_len < rp->rcvd_bytes)
      {
         MSG_ERROR("More bytes received than expected in IP Reassembly", 
                    0,0,0);
         // ASSERT(0);
      }
      else
      {
         MSG_HIGH("Recvd all IP Fragments & %d bytes ", rp->datagram_len,0,0);
      }
      
      
      /*---------------------------------------------------------------------
        Read the data/payload from the fragments and append all of them into 
        a single packet chain.
        
        Remove the offset from the top of the payload for all of the 
        fragments.
      ---------------------------------------------------------------------*/
        lastfrag_link = (dsm_item_type*)q_get(rp->fraglist_q);
       
      /*---------------------------------------------------------------------
        Extract the offset that is stored in the payload and thrash it off, 
        because it is not needed now.
        Obtain the offset that is stored in the first link of the queue.
      ---------------------------------------------------------------------*/
        dsm_pullup(((dsm_item_type**)&lastfrag_link), 
                     NULL, 
                     sizeof(ip->offset)
                  );

      /*---------------------------------------------------------------------
        Pull out each element from the queue and append it to the initial
        data item. 
      ---------------------------------------------------------------------*/
        for(nextfrag_link = (dsm_item_type*)q_get(rp->fraglist_q);
            nextfrag_link != NULL;
            nextfrag_link = (dsm_item_type*)q_get(rp->fraglist_q))
        {
           /*----------------------------------------------------------------
             Remove the offset from the top of the payload and thrash it off.
           ----------------------------------------------------------------*/
             dsm_pullup(&nextfrag_link, NULL, sizeof(ip->offset));

           /*----------------------------------------------------------------
             Append everything to the first element of the queue. So, make 
             the first parameter of dsm_append  equal to first element of the 
             data item and the second element varies as we traverse the queue.
           ----------------------------------------------------------------*/
             dsm_append( &lastfrag_link, &nextfrag_link);
        }

                  
        /*-------------------------------------------------------------------
          We need to provide the upper layers the IP header only from the 
          first frame because, the other fragment IP headers may not have 
          the entire information about options. So, copy the first header
          into the IP.
              
          Since the ip->length contains only the length of last received 
          fragment, tell IP the entire length of the datagram.
          ASSERT(dsm_item_length()== rp->datagram_len;
        -------------------------------------------------------------------*/
        *ip = rp->ip;
        ip->length = rp->datagram_len + (IPLEN + ip->optlen);
          
        /*-------------------------------------------------------------------
          Free the resources
        -------------------------------------------------------------------*/
        ip_free_reasm(rp);
        ip->offset = 0;
        ip->flags.mf = 0;

        /*-------------------------------------------------------------------
          Update the statistics. 
        -------------------------------------------------------------------*/
        ipReasmOKs++;

        /*-------------------------------------------------------------------
          We've gotten a complete datagram now, so pass it on. Now the entire 
          fragment is only in the first element of the queue i.e. 
          lastfrag_link
        -------------------------------------------------------------------*/
        return lastfrag_link;
    } 
    else
    {
        /*-------------------------------------------------------------------
          Still some more fragments to come. So wait.
        -------------------------------------------------------------------*/
        return NULL;
    }
         
} /* ip_fragment_handler() */


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
)
{

  uint16 I;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Loop through the reassembly array and find out if the id, source,
    destination and protocol match with any of the entries in the array.
  -------------------------------------------------------------------------*/
  for(I = 0; I < NUM_DATA_FRAG ; I++ )
  {
     if(ip->id       == ip_reasm_array[I].ip.id     && 
        ip->source   == ip_reasm_array[I].ip.source && 
        ip->dest     == ip_reasm_array[I].ip.dest   && 
        ip->protocol == ip_reasm_array[I].ip.protocol )
     {
        /*-------------------------------------------------------------------
          We need to provide the upper layers the IP header only from the 
          first frame because, the other fragment IP headers may not have the
          entire information. If we received the IP fragment with frame 
          offset == 0, it implies this is the first fragment. So copy the 
          ip information.
        -------------------------------------------------------------------*/
        if(ip->offset == 0) 
        {
           ip_reasm_array[I].ip = *ip;
        }

        /*-------------------------------------------------------------------
          If we already have the information about the datagram to which this
          incoming fragment belongs to, return the address of the element
          of the array. The calling function will only manipulate with that
          address.
        -------------------------------------------------------------------*/
        return ((struct ip_reasm_type*)(ip_reasm_array + I));

     }

  }
  /*-------------------------------------------------------------------------
    We did not find any match with existing fragments. This fragment may
    belong to a new datagram. Return NULL so that a new location may be
    created for this.
  -------------------------------------------------------------------------*/
  return NULL;

} /* ip_lookup_reasm()                                                     */


/*===========================================================================

FUNCTION IP_CREATE_REASM()

DESCRIPTION
  This function will create a reassembly descriptor and it will be used to 
  store the information of each IP packet.
  If the array does not have any free descriptor then we discard the oldest
  one and add the new datagram.
  
DEPENDENCIES
  

RETURN VALUE
  The address of the free element of the reassembly structure.

SIDE EFFECTS
  None
===========================================================================*/
struct ip_reasm_type *ip_create_reasm
(
  struct ip *ip
)
{
  /*-------------------------------------------------------------------------
    free_reasm_element is used to access the array that contains the 
    reassembly descriptor.
  -------------------------------------------------------------------------*/    
  int free_reasm_element;
  
  /*-------------------------------------------------------------------------
    found is used to indicate if existing array has any free elements to be
    used for the datagram. If nothing is found then found is made FALSE.
  -------------------------------------------------------------------------*/    
  boolean found = FALSE;    

  /*-------------------------------------------------------------------------
    re_asm_entry_ptr is used to access the elements of the queue.
    re_asm_q_ptr is used to access the queue.
  -------------------------------------------------------------------------*/
  dsm_item_type *re_asm_entry_ptr;  
  q_type        *re_asm_q_ptr;       
  
  /*-------------------------------------------------------------------------
    oldest_datagram is used as a pointer to the the oldest element of the
    reassembly descriptor. This variable contains values from 0 to 
    NUM_DATA_FRAG -1. Based on this number, we find out which of the 
    elements is oldest and replace that.
    
    This has been made static because, its value is needed every time this 
    function is called.
  -------------------------------------------------------------------------*/
  static int oldest_datagram = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        
  /*-------------------------------------------------------------------------
    Check if any elements are unused in the array. If they are unused, then, 
    free_reasm_element will be offset to element of the array.
  -------------------------------------------------------------------------*/
  for(free_reasm_element = 0; 
      free_reasm_element < NUM_DATA_FRAG; 
      free_reasm_element++)
  {
      if(ip_reasm_array[free_reasm_element].in_use == FALSE)
      {
        /*-------------------------------------------------------------------
        Found the free element in ip_reasm_array. 
        -------------------------------------------------------------------*/
        found = TRUE;
        break; 
      }
  } /* For loop                                                            */
  
  /*-------------------------------------------------------------------------
    If all the elements are already being used then free_reasm_element will 
    be equal to NUM_DATA_FRAG. 
  -------------------------------------------------------------------------*/
  if(!found )
    {
      /*---------------------------------------------------------------------
         Control comes here if no free items are available in the array. So, 
         discard the oldest packet to make way for newer packet. Copy the 
         newer packet into older location and return the address of the last 
         element of this array.
      ---------------------------------------------------------------------*/
//    ASSERT(0);  If it hits this, increase size of array.
      free_reasm_element = oldest_datagram;
      oldest_datagram++;
      
      /*---------------------------------------------------------------------
        If the oldest_datagram is greater than number of the elements in 
        the array, then reset to the first one.
      ---------------------------------------------------------------------*/
      if(oldest_datagram >= NUM_DATA_FRAG) 
      {
         oldest_datagram = 0;
      }

    } /* if (!found)                                                       */

    /*-----------------------------------------------------------------------
      Clean up all the data packets that are in the queue.
    -----------------------------------------------------------------------*/
    /* Sanity check */
    if(free_reasm_element >= NUM_DATA_FRAG)
    {
      return NULL;
    }
    re_asm_q_ptr  = ip_reasm_array[free_reasm_element].fraglist_q;
    while ((re_asm_entry_ptr = (dsm_item_type *)q_get(re_asm_q_ptr)) != NULL)
    {
      dsm_free_packet( &re_asm_entry_ptr);
    }

    /*-----------------------------------------------------------------------
      The queue should be initialized 
    -----------------------------------------------------------------------*/
    q_init(re_asm_q_ptr);

    /*-----------------------------------------------------------------------
      Copy the ip->source, ip->dest, ip->id and ip->protocol.
      Note that we need only source, destination, id, protocol, optlen and
      offset from the IP fields for handling the fragments. But while 
      copying the structure, we copy the entire ip header because, we need
      to give the upper layers complete IP header from the first fragment
      only.
        
      Make the in_use field as TRUE to indicate that this element of the
      array is being used. This should be made as FALSE after all the 
      fragments are handled OR when the call is brought down. 
        
      Initialize the datagram_len and rcvd_bytes to 0. They will be filled
      up as fragments are handled.
    -----------------------------------------------------------------------*/
    ip_reasm_array[free_reasm_element].ip           = *ip;
    ip_reasm_array[free_reasm_element].in_use       = TRUE;
    ip_reasm_array[free_reasm_element].datagram_len = 0;
    ip_reasm_array[free_reasm_element].rcvd_bytes   = 0;

    /*-----------------------------------------------------------------------
      Return the address of the array's element. Note that all the 
      operations, by the calling function, will be done at the returned 
      address only.
    -----------------------------------------------------------------------*/
    return ((struct ip_reasm_type*)(ip_reasm_array + free_reasm_element));

} /* ip_create_reasm()                                                     */


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
)
{
    
  /*-------------------------------------------------------------------------
    re_asm_entry_ptr is used to access the elements of the queue.
    re_asm_q_ptr is used to access the queue.
  -------------------------------------------------------------------------*/
  dsm_item_type *re_asm_entry_ptr;  
  q_type        *re_asm_q_ptr;      
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Free up the data items from the queue element. Note that the entire
    packet has to be freed.
  -------------------------------------------------------------------------*/
  re_asm_q_ptr  = rp->fraglist_q;
  // HACK - Check if re_asm_q_ptr can be NULL
  if(re_asm_q_ptr != NULL)
  {
    while((re_asm_entry_ptr = (dsm_item_type *)q_get(re_asm_q_ptr)) != NULL)
    {
      dsm_free_packet( &re_asm_entry_ptr);
    }
  }
      
  /*-------------------------------------------------------------------------
    in_use should be made FALSE, because this element of the array is no
    longer being used by any datagram.
    The datagram_len should be initialized to zero, because, this field is
    used in other functions, to confirm if entire packet has been received.
  -------------------------------------------------------------------------*/
  rp->in_use        = FALSE;
  rp->datagram_len  = 0;

} /* ip_free_reasm()                                                       */


/*===========================================================================

FUNCTION IP_REASM_INIT()

DESCRIPTION
  This function will initialize the reassembly data structures.

DEPENDENCIES
    None

RETURN VALUE
    None
 
SIDE EFFECTS
  None
===========================================================================*/
void ip_reasm_init
(
  void 
)
{
   int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initialize the pointers to the queues for each of the elements of the
    array.
    Make the in_use flag to FALSE, so that it can be used next time by a new
    fragment.
    Make the datagram_len to zero. This is necessary because in other 
    functions, this flag is used to check if all the expected fragments are 
    received.
    Initialize the rcvd_bytes to zero, to inform we have not yet received 
    any data.
  -------------------------------------------------------------------------*/

  for (i=0; i< NUM_DATA_FRAG; i++ ) 
  {
    ip_reasm_array[i].fraglist_q = ((q_type*)(re_asm_q + i));
    q_init(ip_reasm_array[i].fraglist_q);
      
    ip_reasm_array[i].in_use       = FALSE;
    ip_reasm_array[i].datagram_len = 0;
    ip_reasm_array[i].rcvd_bytes   = 0;
  }
  
} /* ip_reasm_init() */

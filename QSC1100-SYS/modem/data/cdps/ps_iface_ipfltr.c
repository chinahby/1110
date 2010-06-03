/*===========================================================================

                      P S _ I F A C E _ I P F L T R . C
                   
DESCRIPTION
  This file provides functions to use IP filters for inbound iface specific
  processing of IP packets received on that iface.  

EXTERNALIZED FUNCTIONS

  PS_IFACE_IPFLTR_ADD()
    adds filters to the iface
  
  PS_IFACE_IPFLTR_DELETE() 
    deletes filters from the iface
  
  PS_IFACE_IPFLTR_EXECUTE() 
    executes filter for the processing inbound IP pkts

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_iface_ipfltr.c_v   1.2   13 Feb 2003 14:15:40   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_iface_ipfltr.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/13/03    usb    Not calling ipfltr_delete() while adding filters, instead 
                   moved delete code in ipfltr_add() to avoid duplicate event 
                   generation.
02/11/03    usb    Modified MSG logs, passing num filters actually 
                   added and deleted to ps_iface_ipfltr_updated_ind()
01/28/03    usb    File created.
===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR THE MODULE
                       
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#include "dsm.h"
#include "queue.h"
#include "internet.h"
#include "rex.h"

#include "ps_iface_ipfltr.h"
#include "ps_aclrules.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Data definitions used for storing filters in iface control block
---------------------------------------------------------------------------*/

typedef struct
{
  q_link_type         link;
  dsm_item_type       *this_item_ptr;
  ip_filter_enum_type filter_type;

  union
  {
    ip_filter_type     fi_default;
    acl_fptr_type      fi_acl;
  } filter;

  sint15              sockfd;
} ps_iface_ipfilteri_type;


#define FILTER_BUF_SIZE sizeof(ps_iface_ipfilteri_type)
#define FILTER_BUF_ITEM_SIZE DSM_DS_SMALL_ITEM_SIZ
#define FILTER_BUF_ITEM_POOL DSM_DS_POOL_SIZE(FILTER_BUF_ITEM_SIZE)
#define FILTER_BUF_ITEM_KIND 0xCC


/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_ALLOC_BUF()

DESCRIPTION
  This function allocates a memory buffer to coantain the IP filter being
  added to the iface.
  
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to the allocated buffer, NULL if the buffer could not be allocated

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_ipfilteri_type *ps_iface_ipfltri_alloc_buf(void)
{

  dsm_item_type *mem_ptr = NULL;
  ps_iface_ipfilteri_type *buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(FILTER_BUF_SIZE > FILTER_BUF_ITEM_SIZE)
  {
    ERR("dsm buf size %d, req size %d, too small", 
        FILTER_BUF_ITEM_SIZE, FILTER_BUF_SIZE, 0);
    ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Try to grab a dsm item to place the buffer into. We always use a new
    dsm item for each buffer since the buffer size is not 4 byte aligned, 
    this can cause data abort if multiple buffers are packed in the same 
    item. If the DSM allocation fails, return NULL.
  -------------------------------------------------------------------------*/
  mem_ptr = dsm_new_buffer(FILTER_BUF_ITEM_POOL);
  if(mem_ptr == NULL)
  {
    MSG_ERROR("Unable to allocate filter buffer", 0, 0, 0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Initialize the event buffer and do the book keeping with dsm item
  -------------------------------------------------------------------------*/
  buf_ptr = (ps_iface_ipfilteri_type*) mem_ptr->data_ptr;
  q_link(buf_ptr, &(buf_ptr->link));
  buf_ptr->this_item_ptr = mem_ptr;
  mem_ptr->kind = FILTER_BUF_ITEM_KIND;
  mem_ptr->used = FILTER_BUF_SIZE;

  return buf_ptr;
} /* ps_iface_ipfltri_alloc_buf() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_FREE_BUF()

DESCRIPTION
  This function frees up the memory buffer coantaining the IP 
  filter in the iface.

DEPENDENCIES
  The buffer should have been created using ps_iface_ipfltri_alloc_buf()
  and it should not be on a queue.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltri_free_buf
(
  ps_iface_ipfilteri_type *buf_ptr     /* ptr to the buffer being freed up */
)
{
  dsm_item_type *mem_ptr;
  uint8         *buf_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(buf_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Validate the buffer that is passed in:
    1) not already on a queue
    2) buffer must be in the data section of the containing DSM item
    3) the kind field is set correctly    
    4) the item pointer in the buffer points to the containing item
  -------------------------------------------------------------------------*/
  mem_ptr = buf_ptr->this_item_ptr;
  if(mem_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  buf_data = (uint8*)buf_ptr;
  if(Q_ALREADY_QUEUED(&buf_ptr->link)                                   ||
     mem_ptr->kind != FILTER_BUF_ITEM_KIND                              ||
     buf_data < mem_ptr->data_ptr                                       ||
     buf_data > (mem_ptr->data_ptr + mem_ptr->size - FILTER_BUF_SIZE)   ||
     ((ps_iface_ipfilteri_type*)mem_ptr->data_ptr)->this_item_ptr != mem_ptr)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Invalidate the buffer so that the caller cannot use it without
    explicitly allcating it again and free it.
  -------------------------------------------------------------------------*/
  buf_ptr->this_item_ptr = NULL;
  dsm_free_buffer(mem_ptr);

} /* ps_iface_ipfltri_free_buf() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTRI_PROCESS_DEFAULT()

DESCRIPTION
  This function deletes the IP rules from the rule table for the specified 
  socket

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for a successful match
  FALSE for no match
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_iface_ipfltri_process_default
(
  ip_filter_type       *fltr_ptr,        /* ptr to filter to process       */
  ip_filter_info_type  *info_ptr         /* ptr to info to apply filter on */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(fltr_ptr == NULL || info_ptr == NULL)
  {
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Empty filter is not allowed
  -------------------------------------------------------------------------*/
  if(fltr_ptr->field_mask == 0)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Now perform the actual filter provcessing. If any field specified in the
    filter does not match we fail the processing. For protocol specific 
    fields the filter is only applied if protocol in the info_ptr is same
    as specified in the filter.
  -------------------------------------------------------------------------*/
  if(fltr_ptr->field_mask & IPFLTR_IP_SRC_ADDR_MASK &&
     fltr_ptr->ip_hdr.v4.src_addr != info_ptr->ip_hdr.v4.source)
  {
    return FALSE;
  }

  if(fltr_ptr->field_mask & IPFLTR_IP_DST_ADDR_MASK &&
     fltr_ptr->ip_hdr.v4.dst_addr != info_ptr->ip_hdr.v4.dest)
  {
    return FALSE;
  }

  if(fltr_ptr->field_mask & IPFLTR_TCP_SRC_PORT_MASK &&
     (info_ptr->ip_hdr.v4.protocol != TCP_PTCL ||
     fltr_ptr->prot_hdr.tcp.src_port != info_ptr->ptcl_info.tcp.src_port))
  {
    return FALSE;
  }

  if(fltr_ptr->field_mask & IPFLTR_TCP_DST_PORT_MASK &&
     (info_ptr->ip_hdr.v4.protocol != TCP_PTCL ||
     fltr_ptr->prot_hdr.tcp.dst_port != info_ptr->ptcl_info.tcp.dst_port))
  {
    return FALSE;
  }

  if(fltr_ptr->field_mask & IPFLTR_UDP_SRC_PORT_MASK &&
     (info_ptr->ip_hdr.v4.protocol != UDP_PTCL ||
     fltr_ptr->prot_hdr.udp.src_port != info_ptr->ptcl_info.udp.src_port))
  {
    return FALSE;
  }

  if (fltr_ptr->field_mask & IPFLTR_UDP_DST_PORT_MASK &&
      (info_ptr->ip_hdr.v4.protocol != UDP_PTCL ||
      fltr_ptr->prot_hdr.udp.dst_port != info_ptr->ptcl_info.udp.dst_port))
  {
    return FALSE;
  }

  if (fltr_ptr->field_mask & IPFLTR_ICMP_MSG_CODE_MASK &&
      (info_ptr->ip_hdr.v4.protocol != ICMP_PTCL ||
      fltr_ptr->prot_hdr.icmp.code != info_ptr->ptcl_info.icmp.code))
  {
    return FALSE;
  }

  if (fltr_ptr->field_mask & IPFLTR_ICMP_MSG_TYPE_MASK &&
      (info_ptr->ip_hdr.v4.protocol != ICMP_PTCL ||
      fltr_ptr->prot_hdr.icmp.type != info_ptr->ptcl_info.icmp.type))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If we reach here, all the specified rules macthed
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ps_iface_ipfltri_process_default() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_ADD()

DESCRIPTION
  This function adds IP filter rules to the specified iface. The filter is 
  tied to a socket, all packets passign through the filter are passed up to 
  that socket. Any existing filter for the socket is deleted before adding 
  the new filter.

DEPENDENCIES
  Caller is responsible for adding a reference to this iface to SCB so that 
  filter could be deleted when the socket is closed. 

RETURN VALUE
  Number of filters successfully added, 0 if none could be added

SIDE EFFECTS
  None.

===========================================================================*/
int ps_iface_ipfltr_add
( 
  ps_iface_type        *iface_ptr,
  ip_filter_enum_type  filter_type,
  uint8                num_filters,
  void                 *fi_ptr_arr,
  sint15               sockfd
)
{
  ps_iface_ipfilteri_type *filter_buf_ptr, *tmp_buf_ptr;
  q_type *q_ptr;
  uint8 n_deleted = 0;                     /* to count num filters deleted */
  uint8 n_added = 0;                       /* to count num filters added   */
  uint8 loop;
  boolean done = FALSE;
  uint32 *tmp_ptr_arr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("Adding %d fltrs,sock %d,if 0x%x", num_filters, sockfd, iface_ptr);

  if(PS_IFACE_IS_VALID(iface_ptr) == FALSE)
  {
    ASSERT(0);
    return 0;
  }

  if (fi_ptr_arr == NULL)
  {
    MSG_ERROR("No filter specified", 0, 0, 0);
    return 0;
  }

  q_ptr = &(iface_ptr->private.ip_filter_info.filter_q);

  TASKLOCK();
  /*-------------------------------------------------------------------------
    First remove all the existing filter rules for the specified socket from
    this iface
  -------------------------------------------------------------------------*/
  filter_buf_ptr = q_check(q_ptr);
  
  while(filter_buf_ptr != NULL)
  {
    tmp_buf_ptr = filter_buf_ptr;
    filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link));
    
    if(tmp_buf_ptr->sockfd == sockfd)
    {
      /*---------------------------------------------------------------------
        Delete this rule
      ---------------------------------------------------------------------*/
      q_delete(q_ptr, &(tmp_buf_ptr->link));    
      ps_iface_ipfltri_free_buf(tmp_buf_ptr);
      n_deleted++;
    }
  }

  /*-------------------------------------------------------------------------
    Now add the new filters to this iface
  -------------------------------------------------------------------------*/
  for(loop=0; loop<num_filters; loop++)
  {
    if ((filter_buf_ptr = ps_iface_ipfltri_alloc_buf()) == NULL)
    {
      break;
    }

    switch(filter_type)
    {
      case IP_INBOUND_FILTER_DEFAULT_TYPE:        
        filter_buf_ptr->filter.fi_default = 
          *((ip_filter_type *)fi_ptr_arr + loop); 
        break;

      case IP_INBOUND_FILTER_ACL_TYPE:
        tmp_ptr_arr = (uint32 *)fi_ptr_arr;
        filter_buf_ptr->filter.fi_acl = (acl_fptr_type)tmp_ptr_arr[loop];
        break;

      default:
        ERR("Invalid filter type %d", filter_buf_ptr->filter_type, 0, 0);
        ps_iface_ipfltri_free_buf(filter_buf_ptr);
        done = TRUE;
        break;
    }
    if(done)
    {
      break;
    }

    filter_buf_ptr->filter_type = filter_type;
    filter_buf_ptr->sockfd = sockfd;
    q_put(q_ptr, &(filter_buf_ptr->link));
    n_added++;
  }
  
  ps_iface_ipfltr_updated_ind(iface_ptr, n_added, n_deleted);
  TASKFREE();

  return (int)n_added;
} /* ps_iface_ipfltr_add() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_DELETE()

DESCRIPTION
  This function deletes all the existing IP filter rules for a socket from 
  the specified iface.
  
DEPENDENCIES
  Caller is responsible for deleting the reference to this iface from SCB.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ps_iface_ipfltr_delete
( 
  ps_iface_type        *iface_ptr,
  sint15               sockfd
)
{
  q_type *q_ptr;
  uint8 n_deleted = 0;                     /* to count num filters deleted */
  ps_iface_ipfilteri_type *curr_buf_ptr, *tmp_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_MED("Deleting fltrs,sock %d,if 0x%x", sockfd, iface_ptr, 0);

  if(PS_IFACE_IS_VALID(iface_ptr) == FALSE)
  {
    ASSERT(0);
    return;
  }

  q_ptr = &(iface_ptr->private.ip_filter_info.filter_q);

  TASKLOCK();
  curr_buf_ptr = q_check(q_ptr);
  
  while(curr_buf_ptr != NULL)
  {
    tmp_buf_ptr = curr_buf_ptr;
    curr_buf_ptr = q_next(q_ptr, &(curr_buf_ptr->link));
    
    if(tmp_buf_ptr->sockfd == sockfd)
    {
      /*---------------------------------------------------------------------
        Delete this rule
      ---------------------------------------------------------------------*/
      q_delete(q_ptr, &(tmp_buf_ptr->link));    
      ps_iface_ipfltri_free_buf(tmp_buf_ptr);
      n_deleted++;
    }
  }
  ps_iface_ipfltr_updated_ind(iface_ptr, 0, n_deleted);
  TASKFREE();

} /* ps_iface_ipfltr_delete() */


/*===========================================================================
FUNCTION PS_IFACE_IPFLTR_EXECUTE()

DESCRIPTION
  This function executes all the filters in an iface to see if any of 
  those passes criteria specified in the information passed in. Processing 
  is done until the first match is found and socket descriptor associated 
  with that filter is returned. For DEFAULT filter type, each field set in
  the filter is compared against the input info using a fixed set of rules. 
  For ACL type filters, the ACL function is called which can contain more
  complex and different types of rules.

DEPENDENCIES
  None
  
RETURN VALUE
  0 for no match, 
  sockfd of the matching socket if a successful match is found

SIDE EFFECTS
  None.

===========================================================================*/
sint15 ps_iface_ipfltr_execute
( 
  ps_iface_type        *iface_ptr,
  ip_filter_info_type  *ip_filter_info_ptr
)
{
  q_type *q_ptr;
  ps_iface_ipfilteri_type *filter_buf_ptr;
  uint8 loop;
  sint15 result = 0;
  boolean done = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(PS_IFACE_IS_VALID(iface_ptr) == FALSE)
  {
    ASSERT(0);
    return 0;
  }

  if (ip_filter_info_ptr == NULL)
  {
    MSG_ERROR("Null filter_info specified", 0, 0, 0);    
    ASSERT(0);
    return 0;
  }
  
  q_ptr = &(iface_ptr->private.ip_filter_info.filter_q);

  TASKLOCK();
  filter_buf_ptr = q_check(q_ptr);

  for(loop=0; loop<PS_IFACE_NUM_FILTERS(iface_ptr); loop++)
  {
    /*-----------------------------------------------------------------------
      if num_filters is non zero filter_buf_ptr cannot be NULL.
    -----------------------------------------------------------------------*/
    if(filter_buf_ptr == NULL)
    {
      ASSERT(0);
      break;
    }

    switch(filter_buf_ptr->filter_type)
    {
      case IP_INBOUND_FILTER_DEFAULT_TYPE:
        /*-------------------------------------------------------------------
          For default filter, process the filter info to see if rquired 
          fields match.
        -------------------------------------------------------------------*/
        
        if(ps_iface_ipfltri_process_default(
             &(filter_buf_ptr->filter.fi_default),
             ip_filter_info_ptr) == TRUE)
        {
          done = TRUE;
          result = filter_buf_ptr->sockfd;
          MSG_HIGH("Fltr passed,iface=0x%x,sock=%d",iface_ptr, result, 0);
        }
        break;

      case IP_INBOUND_FILTER_ACL_TYPE:
        /*-------------------------------------------------------------------
          Call the ACL processing function, note that policy info is not
          required for inbound ACL processing
        -------------------------------------------------------------------*/
        if(filter_buf_ptr->filter.fi_acl(ip_filter_info_ptr, 
                                        NULL, 
                                        iface_ptr) > ACL_DEFAULT_CLASS)
        {
          done = TRUE;
          result = filter_buf_ptr->sockfd;
          MSG_HIGH("Fltr passed,iface=0x%x,sock=%d",iface_ptr, result, 0);
        }
        break;

      default:
        MSG_ERROR("Invalid filter type %d", 
                  filter_buf_ptr->filter_type, 0, 0);
        ASSERT(0);
        break;
    }

    if(done)
    {
      break;
    }

    filter_buf_ptr = q_next(q_ptr, &(filter_buf_ptr->link));
  }
  TASKFREE();

  return result;

} /* ps_iface_ipfltr_execute() */

#endif /* FEATURE_DATA */



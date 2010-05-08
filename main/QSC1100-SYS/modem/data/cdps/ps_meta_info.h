#ifndef PS_META_INFO_H
#define PS_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ M E T A _ I N F O . H

GENERAL DESCRIPTION
  This is a header file that contains the definition of the meta information
  that is passed down from the sockets layer all the way down to the
  interface.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

     Copyright (c)2002-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_meta_info.h_v   1.2   12 Feb 2003 20:35:38   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_meta_info.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/06/03    om     Added ipsec_required flag.
04/24/03    mvl    Added some missing parentheses in macros.
02/12/03    om     Added policy-info-is-valid flag and UDP routing info.
09/24/02    om     Added socket options mask.
04/01/02    mvl    created file.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "assert.h"
#include "comdef.h"

#include "dsm.h"
#include "ps_acl.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define META_INFO_KIND   1

/*---------------------------------------------------------------------------
TYPEDEF PS_META_INFO_TYPE

DESCRIPTION
  This is used to pass meta information related to a packet through the
  stack. It will be pushed down into the dsm item (meta_info_ptr) that is
  passed with the tx_cmd.
  -- this should also be stored in the SCB.
---------------------------------------------------------------------------*/
typedef struct
{
  acl_policy_info_type  policy_info;    /* policy info to use              */
  dsm_item_type        *this_dsm_item;  /* ptr to dsm item this data is in */
  void                 *routing_cache;  /* Rt'g cache for UDP pkts         */
  uint32                tx_flags;       /* flags as defined in dssocket.h  */
  uint32                sock_opts;      /* sock options as in dssocki.h    */
  int16                 sockfd;         /* handle of generating socket     */
  uint8                 sock_ip_ttl;    /* value for TTL option            */
  boolean               ipsec_required; /* Packet requires ESP processing  */
  boolean               policy_info_is_valid; /* above contents are valid  */

} ps_meta_info_type;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
MACRO PS_META_INFO_VALID()

DESCRIPTION
  This macro validates that this is a valid meta info item.  It does so by
  ensuring that the kind field of the DSM item is set to META_INFO_KIND

PARAMETERS
  mi_ptr: pointer to the meta info item being checked

RETURN VALUE
  TRUE if the item is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_META_INFO_VALID(mi_ptr)                      \
  ((mi_ptr)->this_dsm_item->kind == META_INFO_KIND)

/*===========================================================================
MACRO PS_META_INFO_GET()

DESCRIPTION
  This macro allocates the dsm item that is used for the meta info.

  Note: the _SET_META_TRACER() helper macro is there because it is not
        possible to have an #ifdef within a #define

PARAMETERS
  mi_ptr: pointer that will be have the ps_meta_info memory assigned to it.

RETURN VALUE
  mi_ptr will be NULL on failure, or point to the meta_info data

DEPENDENCIES
  None

SIDE EFFECTS
  A dsm item is allocated.
===========================================================================*/
#define PS_META_INFO_GET( mi_ptr )                                      \
  ASSERT(sizeof(ps_meta_info_type) <= DSM_DS_LARGE_ITEM_SIZ);           \
  {                                                                     \
    dsm_item_type *tmp = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL);        \
    if(tmp == NULL)                                                     \
    {                                                                   \
      mi_ptr = NULL;                                                    \
    }                                                                   \
    else                                                                \
    {                                                                   \
      tmp->used = sizeof(ps_meta_info_type);                            \
      tmp->kind = META_INFO_KIND;                                       \
      _SET_META_TRACER(tmp);                                            \
      mi_ptr = (ps_meta_info_type*)tmp->data_ptr;                       \
      mi_ptr->this_dsm_item = tmp;                                      \
    }                                                                   \
  }

#ifdef FEATURE_DSM_MEM_CHK
  #define _SET_META_TRACER(item_ptr) ((item_ptr)->tracer = DSM_PS_META_INFO)
#else
  #define _SET_META_TRACER(item_ptr)
#endif


/*===========================================================================
MACRO PS_META_INFO_FREE()

DESCRIPTION
  This macro frees the dsm item that is used for the meta info.

PARAMETERS
  mi_ptr_ptr: pointer to a ps_meta_info_type pointer.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_META_INFO_FREE(mi_ptr_ptr)                   \
  if((mi_ptr_ptr) != NULL && *(mi_ptr_ptr) != NULL)     \
  {                                                     \
    dsm_item_type *meta_dsm_item_ptr;                   \
    meta_dsm_item_ptr = (*(mi_ptr_ptr))->this_dsm_item; \
    dsm_free_packet(&meta_dsm_item_ptr);                \
    *(mi_ptr_ptr) = NULL;                               \
  }

#endif /* PS_META_INFO_H */

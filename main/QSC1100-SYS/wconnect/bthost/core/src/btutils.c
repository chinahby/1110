/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         B L U E T O O T H    U T I L I T Y    M O D U L E

GENERAL DESCRIPTION
  This module contains the Bluetooth utility functions

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btutils.c_v   1.3   12 Feb 2002 14:17:58   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btutils.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btutils.c_v  $
*
 *   Rev 1.4    16 Jun 2006            DSN
 * Added featurization to support HostController SoC solution.
 * 
 *    Rev 1.3   12 Feb 2002 14:17:58   waynelee
 * Made some common bt utility funcs more lean and efficient.
 * Added some new utility funcs.
 * 
 *    Rev 1.2   26 Mar 2001 11:43:02   waynelee
 * Added support for new broadcast device area.
 * 
 *    Rev 1.1   13 Mar 2001 11:12:10   sramacha
 * Featurized setting dsm_ptr->tracer value so that we can compile without
 * FEATURE_DSM_MEM_CHK being defined.
 * 
 *    Rev 1.0   29 Jan 2001 17:10:42   propach
 * Initial revision.
 * 
 *    Rev 1.5   21 Dec 2000 17:49:00   waynelee
 * Removed T_CORE_EMUL.  Added bt_set_pkt_tracer().
 * 
 *    Rev 1.4   21 Sep 2000 12:07:38   waynelee
 * fixed auth/encrypt; fixed inquiry; added scan modes; other hs fixes
 * 
 *    Rev 1.3   30 Aug 2000 09:43:06   waynelee
 * Made changes for new BT_MSG macros
 * 
 *    Rev 1.2   29 Aug 2000 08:26:40   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.1   19 Jul 2000 17:28:26   propach
 * Update BT API and featurization with cleanup.
 *
 *    Rev 1.0   04 Jul 2000 19:24:16   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "bthc.h"
#ifndef FEATURE_BT_SOC
#include "btqdspq.h"
#endif /* FEATURE_BT_SOC */
#include "btutils.h"


#define BT_MSG_LAYER  BT_MSG_GN   /* necessary for btmsg.h */

#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_dw2lap

DESCRIPTION

===========================================================================*/
void bt_dw2lap
(
  dword         val,
  bt_lap_type*  lap_ptr
)
{

  byte   i;
  byte*  byte_ptr = (byte*) lap_ptr;

  for ( i = 0; i < sizeof( bt_lap_type ); i++ )
  {
    byte_ptr[i] = (byte) (val >> (8*i));
  }

}


/*===========================================================================

FUNCTION
  bt_lap2dw

DESCRIPTION

===========================================================================*/
dword bt_lap2dw
(
  bt_lap_type  lap
)
{

  return( (dword) (lap[0]) |
          (dword) (lap[1] << 8) |
          (dword) (lap[2] << 16) );

}
#endif /* FEATURE_BT_SOC */


/*===========================================================================

FUNCTION
  bt_w2ba

DESCRIPTION

===========================================================================*/
void bt_w2ba
(
  word   w,
  byte*  ba
)
{

  ba[0] = (byte) (w);
  ba[1] = (byte) (w >> 8);

}


#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_ba2w

DESCRIPTION

===========================================================================*/
word bt_ba2w
(
  byte*  ba
)
{

  return( (word) ba[0] | (((word) ba[1]) << 8) );

}


/*===========================================================================

FUNCTION
  bt_ba2ch

DESCRIPTION

===========================================================================*/
word bt_ba2ch
(
  byte*  ba
)
{

  return( ((word) ba[0] | (((word) ba[1]) << 8)) & 0xFFF );

}


/*===========================================================================

FUNCTION
  bt_dw2ba

DESCRIPTION

===========================================================================*/
void bt_dw2ba
(
  dword  dw,
  byte*  ba
)
{

  ba[0] = (byte) (dw);
  ba[1] = (byte) (dw >> 8);
  ba[2] = (byte) (dw >> 16);
  ba[3] = (byte) (dw >> 24);

}


/*===========================================================================

FUNCTION
  bt_ba2dw

DESCRIPTION

===========================================================================*/
dword bt_ba2dw
(
  byte*  ba
)
{

  return( (dword) ba[0] | (((dword) ba[1]) << 8) |
          (((dword) ba[2]) << 16) | (((dword) ba[3]) << 24) );

}


/*===========================================================================

FUNCTION
  bt_aa2idx

DESCRIPTION

===========================================================================*/
byte bt_aa2idx
(
  word  am_addr
)
{

  byte i;

  for ( i = 0; i < BT_MAX_BT_DEV; i++ )
  {
    if ( *bt_lm_remote_unit[i].attributes.am_addr_ptr == am_addr )
    {
      // found it
      break;
    }
  }

  return( i );

}


/*===========================================================================

FUNCTION
  bt_ba2idx

DESCRIPTION

===========================================================================*/
byte bt_ba2idx
(
  bt_bd_addr_type*  arm_bd_addr_ptr
)
{
  uint8                         idx;
  bt_lm_remote_unit_info_type*  rui_ptr;
  bt_lm_conn_info_type*         acl_ci_ptr;

  for ( idx = 0; idx < BT_MAX_BT_DEV; idx++ )
  {
    rui_ptr    =  &bt_lm_remote_unit[ idx ];
    acl_ci_ptr =  &rui_ptr->connection[ BT_LM_ACL_CONN_IDX ];

    if ( ( acl_ci_ptr->state != BT_LM_CONN_DOWN ) &&
         ( bt_arm_qdsp_memcmp( (uint8*) (arm_bd_addr_ptr),
                               (uint8*) rui_ptr->attributes.bd_addr_ptr,
                                sizeof( bt_bd_addr_type ) ) == 0 ) )
    {
      /* found it */
      break;
    }
  }

  return( idx );
}


/*===========================================================================

FUNCTION
  bt_connh2idx

DESCRIPTION

===========================================================================*/
uint8 bt_connh2idx
(
  word                    conn_hndl,
  bt_lm_conn_info_type**  ci_ptr_ptr
)
{
  uint8 ci_idx;
  uint8 dev_idx;

  dev_idx = BT_LM_GET_DEV_IDX_FROM_CONN_HNDL( conn_hndl );
  ci_idx  = BT_LM_GET_CI_IDX_FROM_CONN_HNLD( conn_hndl );
  
  if ( bt_lm_remote_unit[ dev_idx ].connection[ ci_idx ].conn_hndl ==
       conn_hndl )
  {
    if ( ci_ptr_ptr != NULL )
    {
      *ci_ptr_ptr = &bt_lm_remote_unit[ dev_idx ].connection[ ci_idx ];
    }
  }
  else
  {
    dev_idx = BT_MAX_BT_DEV;
  }

  return dev_idx;
}

/*===========================================================================

FUNCTION
  bt_connh2rui

DESCRIPTION

===========================================================================*/
bt_lm_remote_unit_info_type* bt_connh2rui
(
  word                    conn_hndl,
  bt_lm_conn_info_type**  ci_ptr_ptr
)
{
  uint8                         ci_idx;
  uint8                         dev_idx;
  bt_lm_remote_unit_info_type*  rui_ptr;

  dev_idx = BT_LM_GET_DEV_IDX_FROM_CONN_HNDL( conn_hndl );
  ci_idx  = BT_LM_GET_CI_IDX_FROM_CONN_HNLD( conn_hndl );

  rui_ptr = &bt_lm_remote_unit[ dev_idx ];  

  if ( rui_ptr->connection[ ci_idx ].conn_hndl == conn_hndl )
  {
    if ( ci_ptr_ptr != NULL )
    {
      *ci_ptr_ptr = &rui_ptr->connection[ ci_idx ];
    }
  }
  else
  {
    rui_ptr = NULL;
  }

  return rui_ptr;
}


/*===========================================================================

FUNCTION
  bt_hci_to_arm_memcpy

DESCRIPTION

===========================================================================*/
void bt_hci_to_arm_memcpy
(
  byte*   dest_ptr,
  byte*   src_ptr,
  uint32  count
)
{

  while ( count != 0 )
  {
    *dest_ptr++ = *src_ptr++;
    count--;
  }

}


/*===========================================================================

FUNCTION
  bt_arm_qdsp_memcmp

DESCRIPTION

===========================================================================*/
int bt_arm_qdsp_memcmp
(
  byte*   arm_ptr,
  byte*   dsp_ptr,
  uint32  count
)
{

  int     ok = 0;
  uint16  temp;

  while ( count != 0 )
  {
    temp = *((uint16*)(dsp_ptr));
    if ( count )
    {
      if ( *arm_ptr++ != (temp & 0x00FF) )
      {
        ok = 1;
      }
      count--;
    }
    if ( count )
    {
      if ( *arm_ptr++ != ((temp & 0xFF00)>> 8) )
      {
        ok = 1;
      }
      count--;
    }
    dsp_ptr += sizeof( uint16 );
  }

  return( ok );

}


/*===========================================================================

FUNCTION
  bt_a2q_memcpy

DESCRIPTION

===========================================================================*/
void bt_a2q_memcpy
(
  byte*   dest_ptr,
  byte*   src_ptr,
  uint32  count
)
{

  uint16 temp;

  while ( count != 0 )
  {
    temp = *((uint16 *)(dest_ptr));
    if ( count )
    {
      temp = (temp & 0xFF00) | *src_ptr++;
      count--;
    }
    if ( count )
    {
      temp = (temp & 0x00FF) | ((*src_ptr++) << 8);
      count--;
    }
    *((uint16 *)(dest_ptr)) = temp;
    dest_ptr += sizeof( uint16 );
  }

}


/*===========================================================================

FUNCTION
  bt_q2a_memcpy

DESCRIPTION

===========================================================================*/
void bt_q2a_memcpy
(
  byte*   dest_ptr,
  byte*   src_ptr,
  uint32  count
)
{

  uint16 temp;

  while ( count != 0 )
  {
    temp = *((uint16 *)(src_ptr));
    if ( count )
    {
      *dest_ptr++ = (temp & 0xFF);
      count--;
    }
    if ( count )
    {
      *dest_ptr++ = ((temp & 0xFF00) >> 8);
      count--;
    }
    src_ptr += sizeof( uint16 );
  }

}

/*===========================================================================

FUNCTION
  bt_a32_2_2q16s

DESCRIPTION
  Writes a uint32 value into 2 qdsp words

===========================================================================*/
void bt_a32_2_2q16s
(
  uint16*  dst_ptr,
  uint32   value
)
{
  dst_ptr[ 0 ] = value & 0xFFFF;
  dst_ptr[ 1 ] = ( value >> 16 ) & 0xFFFF;
}


/*===========================================================================

FUNCTION
  bt_ascii_to_bd_addr

DESCRIPTION

===========================================================================*/
void bt_ascii_to_bd_addr
(
  char*             str,
  bt_bd_addr_type*  ba_ptr
)
{

  byte    i, p = sizeof( bt_bd_addr_type ) * 2;
  uint16  j;
  char    c;

  for ( i = 0, j = 0; i < sizeof( bt_bd_addr_type ); i++ )
  {
    p -= 2;
    c = str[p];
    j = ( ( c >= '0' ) && ( c <= '9' ) ) ? c - '0' : c - 'A' + 10;
    j <<= 4;
    c = str[p+1];
    j |= ( ( c >= '0' ) && ( c <= '9' ) ) ? c - '0' : c - 'A' + 10;
    ba_ptr->bd_addr_bytes[i] = j;
  }

}


/*===========================================================================

FUNCTION
  bt_bd_addr_to_ascii

DESCRIPTION

===========================================================================*/
void bt_bd_addr_to_ascii
(
  char*             str,
  bt_bd_addr_type*  ba_ptr
)
{

  byte    i, p = sizeof( bt_bd_addr_type ) * 2;
  uint16  j, k;

  for ( i = 0; i < sizeof( bt_bd_addr_type ); i++ )
  {
    j = (uint16) ba_ptr->bd_addr_bytes[i];
    k = j & 0x000F;
    str[--p] = ( k <= 9 ) ? k + '0' : k - 10 + 'A';
    k = ( j >> 4 ) & 0x000F;
    str[--p] = ( k <= 9 ) ? k + '0' : k - 10 + 'A';
  }

}

/*===========================================================================

FUNCTION
  bt_set_pkt_tracer

DESCRIPTION
  Sets the tracer value for all of the dsm buffers in the packet chain
  to the specified value.

===========================================================================*/
void bt_set_pkt_tracer
(
  dsm_item_type*  dsm_ptr,     /* the dsm packet chain head  */
  uint16          tracer_val   /* the tracer value to set to */
)
{
  while( dsm_ptr != NULL )
  {
#ifdef FEATURE_DSM_MEM_CHK
      dsm_ptr->tracer = tracer_val;
#endif //FEATURE_DSM_MEM_CHK
      dsm_ptr = dsm_ptr->pkt_ptr;
  }
}

#endif /* FEATURE_BT_SOC */

#endif /* FEATURE_BT */


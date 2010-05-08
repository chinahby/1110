#ifndef _BTUTILS_H
#define _BTUTILS_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

   B L U E T O O T H    U T I L I T Y    M O D U L E

GENERAL DESCRIPTION
  This module contains Bluetooth utility function definitions

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btutils.h_v   1.2   12 Feb 2002 14:18:00   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btutils.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btutils.h_v  $
 *
 *    #8       01 Nov 2007             AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *    #7        27 Jun 2007            PN
 * Includes "AEEstd.h" unconditionally.
 *
 *    #6        15 Jun 2007            PN
 * Replaced "AEEStd.h" with "AEEstd.h".
 *
 *    #5        21 Feb 2007            PN
 * Fixed compile error for MSM6500.
 *
 *    #4        16 Feb 2007            PN
 * Added safe string macros.
 * 
 *    Rev 1.3   27 Jun 2006            dsn
 * Added featurization for SOC solution.
 *
 *    Rev 1.2   12 Feb 2002 14:18:00   waynelee
 * Added some new utility funcs.
 * 
 *    Rev 1.1   31 May 2001 14:46:34   waynelee
 * Added BT_A2Q_W32 macro.
 * 
 *    Rev 1.0   29 Jan 2001 17:10:46   propach
 * Initial revision.
 * 
 *    Rev 1.4   21 Dec 2000 17:49:24   waynelee
 * Added BT_SET_PKT_TRACER() macro if BT_DSM_MEM_CHK defined.
 * 
 *    Rev 1.3   03 Nov 2000 09:08:42   waynelee
 * Added periodic logging on state/status/statistics for QXDM.
 * 
 *    Rev 1.2   21 Sep 2000 12:07:42   waynelee
 * fixed auth/encrypt; fixed inquiry; added scan modes; other hs fixes
 * 
 *    Rev 1.1   17 Aug 2000 13:47:38   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.0   04 Jul 2000 19:24:20   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_BT_SOC 
#include "btlm.h"
#endif /* FEATURE_BT_SOC */
#include "bti.h"
#include "dsm.h"
#include "AEEstd.h"
/*===========================================================================

          FUNCTION DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for functions, local and global.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        External Functions                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_dw2lap

DESCRIPTION

===========================================================================*/
extern void bt_dw2lap (dword val, bt_lap_type *lap_ptr);

/*===========================================================================

FUNCTION
  bt_lap2dw

DESCRIPTION

===========================================================================*/
extern dword bt_lap2dw (bt_lap_type lap);

/*===========================================================================

FUNCTION
  bt_w2ba

DESCRIPTION

===========================================================================*/
extern void bt_w2ba (word w, byte *ba);

#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_ba2w

DESCRIPTION

===========================================================================*/
extern word bt_ba2w (byte *ba);

/*===========================================================================

FUNCTION
  bt_ba2ch

DESCRIPTION

===========================================================================*/
extern word bt_ba2ch (byte *ba);

/*===========================================================================

FUNCTION
  bt_ba2idx

DESCRIPTION

===========================================================================*/
extern byte bt_ba2idx (bt_bd_addr_type* arm_bd_addr_ptr);

/*===========================================================================

FUNCTION
  bt_aa2idx

DESCRIPTION

===========================================================================*/
extern byte bt_aa2idx (word am_addr);

/*===========================================================================

FUNCTION
  bt_dw2ba

DESCRIPTION

===========================================================================*/
extern void bt_dw2ba (dword dw, byte *ba);

/*===========================================================================

FUNCTION
  bt_ba2dw

DESCRIPTION

===========================================================================*/
extern dword bt_ba2dw (byte *ba);

/*===========================================================================

FUNCTION
  bt_connh2idx

DESCRIPTION

===========================================================================*/
extern byte bt_connh2idx (word conn_hndl, bt_lm_conn_info_type** ptr_ptr);

/*===========================================================================

FUNCTION
  bt_connh2rui

DESCRIPTION

===========================================================================*/
extern bt_lm_remote_unit_info_type* bt_connh2rui
(
  word                    conn_hndl,
  bt_lm_conn_info_type**  ci_ptr_ptr
);

/*===========================================================================

FUNCTION
  bt_a2q_memcpy

DESCRIPTION

===========================================================================*/
extern void bt_a2q_memcpy( byte * dest_ptr, byte * src_ptr, uint32 count);

/*===========================================================================

FUNCTION
  bt_q2a_memcpy

DESCRIPTION

===========================================================================*/
extern void bt_q2a_memcpy( byte * dest_ptr, byte * src_ptr, uint32 count);

/*===========================================================================

FUNCTION
  bt_hci_to_arm_memcpy

DESCRIPTION

===========================================================================*/
extern void bt_hci_to_arm_memcpy( byte * dest_ptr, byte * src_ptr, uint32 count);

/*===========================================================================

FUNCTION
  bt_arm_qdsp_memcmp

DESCRIPTION

===========================================================================*/
extern int bt_arm_qdsp_memcmp( byte * dest_ptr, byte * src_ptr, uint32 count);

/*===========================================================================

FUNCTION
  bt_hc_send_packet_to_host

DESCRIPTION

===========================================================================*/
extern boolean bt_hc_send_packet_to_host( byte uart, byte *src_ptr, word num_bytes );

/*===========================================================================

FUNCTION
  bt_a32_2_2q16s

DESCRIPTION
  Writes a uint32 value into 2 qdsp words

===========================================================================*/
extern void bt_a32_2_2q16s
(
  uint16*  dst_ptr,
  uint32   value
);

/*===========================================================================

FUNCTION
  bt_ascii_to_bd_addr

DESCRIPTION
  Convert an ASCII representation of a BD_ADDR to bt_bd_addr_type

DEPENDENCIES
  The ASCII BD_ADDR is ordered from MSB to LSB of the actul BD_ADDR

===========================================================================*/
extern void bt_ascii_to_bd_addr( char*, bt_bd_addr_type* );

/*===========================================================================

FUNCTION
  bt_bd_addr_to_ascii

DESCRIPTION
  Convert a BD_ADDR of type bt_bd_addr_type to ASCII

SIDE EFFECTS
  The ASCII BD_ADDR is ordered from MSB to LSB of the actual BD_ADDR

===========================================================================*/
extern void bt_bd_addr_to_ascii( char*, bt_bd_addr_type* );

/*===========================================================================

FUNCTION
  bt_set_pkt_tracer

DESCRIPTION
  Sets the tracer value for all of the dsm buffers in the packet chain
  to the specified value.

===========================================================================*/
extern void bt_set_pkt_tracer
(
  dsm_item_type*  dsm_ptr,     /* the dsm packet chain head  */
  uint16          tracer_val   /* the tracer value to set to */
);

/*===========================================================================

MACRO 
  BT_Q2A_W32

DESCRIPTION
  Takes in a pointer to qdsp memory and returns the 32-bit value.

PARAMETERS
  qdsp_ptr  - pointer to qdsp memory.

===========================================================================*/
#define BT_Q2A_W32( qdsp_ptr ) \
  (uint32) ( ( *( (uint16*) qdsp_ptr + 0 ) << 16 ) | \
             ( *( (uint16*) qdsp_ptr + 1 ) << 0  ) )

/*===========================================================================

MACRO 
  BT_Q2A_W32

DESCRIPTION
  Takes in a pointer to qdsp memory and returns the 32-bit value.

PARAMETERS
  qdsp_ptr  - pointer to qdsp memory.

===========================================================================*/
#define BT_A2Q_W32( qdsp_ptr, value )                                   \
{                                                                       \
  *( (uint16*) qdsp_ptr + 0 ) = ( (uint16) ( value >> 16 ) & 0xFFFF );  \
  *( (uint16*) qdsp_ptr + 1 ) = ( (uint16) ( value ) & 0xFFFF );        \
}
#endif /* FEATURE_BT_SOC */

/*===========================================================================

MACRO 
  BT_SET_PKT_TRACER

DESCRIPTION
  If the BT_DSM_MEM_CHK feature is enabled, calls the appropriate function
  to set tracer value to all of the dsm buffers in the packet chain.

PARAMETERS

===========================================================================*/
#ifdef BT_DSM_MEM_CHK
#define BT_SET_PKT_TRACER( dsm_ptr, tracer_val ) \
  bt_set_pkt_tracer( dsm_ptr, tracer_val )
#else
#define BT_SET_PKT_TRACER( dsm_ptr, tracer_val )
#endif

/*===========================================================================

MACROS
  BT_SAFE_STRCPY
  BT_SAFE_STRFORMAT
  

DESCRIPTION
  Safe string APIs.

===========================================================================*/
#ifdef AEESTD_H
#define BT_STRCPY      std_strlcpy
#define BT_STRFORMAT   std_strlprintf
#define BT_VSTRFORMAT  std_vstrlprintf
#define BT_STRCMP      std_strcmp
#else
#define BT_STRCPY      strncpy
#define BT_STRFORMAT   snprintf
#define BT_VSTRFORMAT  vsnprintf
#define BT_STRCMP      strcmp
#endif

#endif /* FEATURE_BT */

#endif /* _BTUTILS_H */


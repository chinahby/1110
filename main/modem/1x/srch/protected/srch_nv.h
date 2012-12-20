#ifndef SRCH_NV_H
#define SRCH_NV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S E A R C H   T A S K  --  S R C H _ N V


GENERAL DESCRIPTION
   This file contains the NV access interface for srch.

EXTERNALIZED FUNCTIONS
   srch_nv_read_nv_items

  Copyright (c) 2005, 2006, 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_nv.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/09   mca     Moved the QLIC enable/disable to the RX Diversity NV item
11/07/08   pa      Fixed #define for header
11/06/08   pk      Fixes for Q6 compiler
02/16/07   sst     Add QLIC support
04/03/06   awj     Added translation for TRM_RX_BEST and TRM_RX_BEST_POSSIBLE
10/25/05   awj     Migrated here from srch.c
===========================================================================*/

/*--------------------------------------------------------------------------*
 *                  Elements in the srch_dbg_mask_nv_i                      *
 *--------------------------------------------------------------------------*/

/* srch_dbg_mask_nv_i is item 3632, 64 bits */

/* Bits 0:1
   Chain to use when TRM_RX_ANY is selected */
#define SRCH_NV_RX_ANY_CHAIN_TRANS_MASK        0x3
#define SRCH_NV_RX_ANY_CHAIN_TRANS_SHIFT       0

/* Bits 2:3
   Chain to use when TRM_RX_BEST is selected */
#define SRCH_NV_RX_BEST_CHAIN_TRANS_MASK       0x3
#define SRCH_NV_RX_BEST_CHAIN_TRANS_SHIFT      2

/* Bits 4:5
   Chain to use when TRM_RX_BEST_POSSIBLE is selected */
#define SRCH_NV_RX_BEST_POS_CHAIN_TRANS_MASK   0x3
#define SRCH_NV_RX_BEST_POS_CHAIN_TRANS_SHIFT  4

#define SRCH_NV_RX_DEFAULT                     0x0
#define SRCH_NV_RX_0                           0x1
#define SRCH_NV_RX_1                           0x2

/*--------------------------------------------------------------------------*
 *                          Typedefs and Macros                             *
 *--------------------------------------------------------------------------*/

typedef uint32  srch_nv_dbg_mask_type;  /* element from NV debug item type */

/*===========================================================================

FUNCTION       SRCH_NV_READ_NV_ITEMS

DESCRIPTION    This function reads the RX_DIVERSITY NV item

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_nv_read_nv_items( void );

#endif


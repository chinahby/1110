#ifndef BOOT_CLK_SPEED_H
#define BOOT_CLK_SPEED_H

/*===========================================================================

                       B o o t  C l o c k  S p e e d
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and type definitions for the
  clock speed data structures and definitions that are common between
  different images.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/shared/boot_clk_speed.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

typedef enum
{
  BOOT_CLK_SOURCE_TCXO=0,
  BOOT_CLK_SOURCE_PLL_384,
  BOOT_CLK_SOURCE_PBL_MAX, /* 2 */
  BOOT_CLK_SOURCE_PLL_192,
  BOOT_CLK_SOURCE_ENUM_SIZE = 0x7FFFFFFF
}boot_clk_source_type;

/* Clock speed definition structure */
typedef struct boot_clk_speed_type
{
  boot_clk_source_type boot_clk_source;
  uint32 arm9;
  uint32 ahb;
  uint32 ebi1;
  uint32 ebi2;
} boot_clk_speed_type;


#endif /* BOOT_CLK_SPEED_H */

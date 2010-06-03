#ifndef _UTILS_H
#define _UTILS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_std_utils.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
 This file contains utilities that are commonly found in stdlib libraries.
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_bt_spec.h"
#include "oi_memmgr.h"
#include "oi_utils.h"

extern const OI_BD_ADDR OI_ZeroAddr;

void OI_StrToUpper(OI_CHAR *str);

/** Convert a character to upper case. */
#define OI_toupper(c) ( ((c) >= 'a') && ((c) <= 'z') ? ((c) - 32) : (c) )

/**
 * Compare two BD_ADDRs
 * SAME_BD_ADDR - Boolean: TRUE if they are the same address
 */

#define SAME_BD_ADDR(x, y)      (0 == OI_MemCmp((x),(y),OI_BD_ADDR_BYTE_SIZE) )

/**
 * This macro initializes a BD_ADDR. This currently zeros all bytes.
 */

#define INIT_BD_ADDR(addr)      OI_MemZero((addr), OI_BD_ADDR_BYTE_SIZE)

#define OI_IS_ZERO_ADDR(addr)  SAME_BD_ADDR((addr), &OI_ZeroAddr) 

//Max/min

#define OI_MAX(a, b) ((a) < (b) ? b : a )
#define OI_MIN(a, b) ((a) > (b) ? b : a )

/*****************************************************************************/
#endif /* _UTILS_H */

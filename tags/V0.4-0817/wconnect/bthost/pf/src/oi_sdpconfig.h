#ifndef _OI_SDPCONFIG_H
#define _OI_SDPCONFIG_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_sdpconfig.h#1 $ 

$Log: $

===========================================================================*/
/** @file
 * macros for computing attribute IDs for each language
 *
 * These may be modified to support other languages as needed. The
 * default language must be 0x100.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define BASE_ENGLISH(id)   ((id) + 0x100)  /* 0x0100 is the default for the primary language. */
#define BASE_FRANCAIS(id)  ((id) + 0x110)
#define BASE_DEUTSCH(id)   ((id) + 0x120)
#define BASE_JAPANESE(id)  ((id) + 0x130)

#endif

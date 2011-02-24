#ifndef __MIBOOT_H__
#define __MIBOOT_H__
/*===========================================================================

                Multi-Image Boot Block Definitions

DESCRIPTION
  This header file provides definitions of which blocks contain which
  Boot Information and Images.

  Copyright (c) 2004-2007 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/mibib/MSM_MIBIB.02.00/miboot.h#9 $ $DateTime: 2008/01/25 16:39:26 $ $Author: rtilak $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/07   rt      Add support for Secure Boot 2.0
02/28/07   rt      Add support for NOR partitioning
10/30/04   drh     Add support security modes
08/30/04   drh     Created.

===========================================================================*/

#include "mibib.h"

#ifndef MIBIB_VERSION
  #error MIBIB Version not defined
#elif (MIBIB_VERSION == 3)
  #include "mioemsbl.h"
  #include "miqcsbl.h"
#elif (MIBIB_VERSION == 4)
  /* Definitions of magic numbers used in the first page of each
   * of the FSBL and OSBL blocks.  We must mark these blocks
   * with some identifier because the DBL does not have enough
   * knowledge to know how to find bad blocks.
   */
  #define MI_FSBL_MAGIC1         0x6FC123DF
  #define MI_FSBL_MAGIC2         0x60FDEFC7

  #define MI_OSBL_MAGIC1         0x6CBA1CFD
  #define MI_OSBL_MAGIC2         0x68D2CBE9
#else
  #error Unknown MIBIB Version
#endif


/*------------------------------------------------------------
 *
 *  Multi-Image Boot Block Definitions
 *
 * ---------------------------------------------------------*/
 
 /* Definitions of which blocks are used for what purposes.  These
  * definitions are for NON-TRUSTED boot on older MSMs and  for TRUSTED
  * boot on newer MSMs. These offsets are used as the base values for
  * allocating blocks.
  */

#if (MIBIB_VERSION == 3)
  #define MI_BLOCK_PBL         0x00
  #define MI_BLOCK_QCSBLHD     0x01
  #define MI_BLOCK_BBTB        0x02
  #define MI_BLOCK_MIBIB_1     0x03
  #define MI_BLOCK_MIBIB_2     0x04
  #define MI_BLOCK_LAST        0x05
#elif (MIBIB_VERSION == 4)
  /* Block offsets for Secure Boot Architecture 2.0 */ 
  #define MI_BLOCK_DBL         0x00
  #define MI_BLOCK_BBTB        0x01
  #define MI_BLOCK_MIBIB       0x02
  #define MI_BLOCK_LAST        0x03
#endif

#define MI_BOOT_SEC_MODE_NON_TRUSTED 0x00
#define MI_BOOT_SEC_MODE_TRUSTED     0x01

/* Block offsets for NOR multi-image boot architecture */ 
#define MI_BLOCK_BOOT_VECT     0x00
#define MI_BLOCK_PARTI_TBL     0x01
#define MI_BLOCK_QCSBL         0x02


#endif /* __MIBOOT_H__ */

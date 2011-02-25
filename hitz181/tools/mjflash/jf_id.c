/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      MSM specific Identification Block

GENERAL DESCRIPTION
  This module contains a block of strings that is the ID block for the
  binary

EXTERNALIZED DATA
  id_tbl
    The data structure.

  Copyright (c) 2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/jflash/JFLASH.03.00/jf_id.c#2 $ $DateTime: 2008/02/22 20:25:02 $ $Author: opham $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/08   op      Initial version
===========================================================================*/


//--------------------------------------------------------------------------
// Global Data Declarations
//--------------------------------------------------------------------------

/* Table of strings that show up in the binary.  The only way to have them
 * appear in the binary in the same order as they are declared are to
 * have them in an array of pointers to strings (or an array of strings).
 * If they are declared separately like they used to be, then they will
 * invariably be rearranged by the compile/link process and may not make
 * sense in the binary.
 *
 * These strings are obtained from the binary using "strings jflash.elf".
 */
const char *id_tbl[] = {
  "  QUALCOMM MJFLASH Identification Block\n"
  "    Compiled " __DATE__ " " __TIME__ "\n"

  /* Update this version string every time MJNAND is released */
  "    Version == JFLASH 03.00.00\n"
  "    JFLASH.ELF for QSC1100 SURF\n"
  "    Copyright (c) 2008 by Qualcomm, Incorporated. All Rights Reserved.\n"
  "  End of ID Block\n"
};



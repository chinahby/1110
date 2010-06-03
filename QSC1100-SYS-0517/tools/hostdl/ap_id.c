/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      MSM specific Identification Block

GENERAL DESCRIPTION
  This module contains a block of strings that is the ID block for the
  binary

EXTERNALIZED DATA
  id_tbl
    The data structure.

  Copyright (c)  2008 Qualcomm Incorporated. 
   All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/tools/hostdl/HOSTDL.14.00/ap_id.c#1 $ $DateTime: 2008/01/21 21:01:41 $ $Author: opham $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/MSM6275/ap_id.c#9 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/08   op      Initial version

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
 * There also must be code to reference this array and that is in
 * packet_init().  If that code is removed, then these strings will not
 * appear in the binary.  The compiler/linker is sometimes too smart.
 *
 * These strings are obtained from the binary using "strings armprg.hex".
 */


/* The ap_flash.min file included by the makefile defines this.  */
#ifndef BIN_NAME
#define BIN_NAME "APRG6055.HEX"
#endif

#ifdef USE_UART_ONLY
#define TRANSPORT "UART only" 
#else
#define TRANSPORT "UART and USB" 
#endif



const char *id_tbl[] = {
  "  QUALCOMM ARMPRG Identification Block\n"
  "    Compiled " __DATE__ " " __TIME__ "\n"

  /* Update this version string every time ARMPRG is released */
  "    Version == 14.00.00\n"
  "    " BIN_NAME " for QSC1100 ONLY - " TRANSPORT "\n"
  "      Multi-Image Boot Support only\n"
  "    Copyright (c) 2004-2007 by QUALCOMM, Incorporated. All Rights Reserved.\n"
#if defined(NOISY) || defined(TNOISY)
#error code not present
#endif
  "  End of ID Block\n"
};



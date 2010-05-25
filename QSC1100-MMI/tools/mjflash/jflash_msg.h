#ifndef __JFLASH_MSG_H__
#define __JFLASH_MSG_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

GENERAL DESCRIPTION

  This program is essentially a plug-in for Lauterbach JTAG ICD TRACE32
  Debugger FLASH command. The programming can be achieved in two different 
  modes: emulator controlled or target controlled. The emulator controlled
  programming needs no program running on the target, but is slow
  (typically about 1 to 5 KBytes/sec). The target controlled programming 
  requires a user designed FLASH programming routine (and hence this plug-in)
  to be available on the target. This plug-in is called by TRACE32 with 
  appropriate parameters to program FLASH devices. The advantage of this
  method is the higher programming speed and more flexibility in the 
  implementation of the programming algorithm.

  To use the plug-in, see the PRACTICE file JNAND.CMM.  It not only uses
  the built-in FLASH.* commands, but also pokes the parameter buffer with
  extended command codes that we need in order to do initialization and
  finalization.
  
  In order for the Debugger to get control back after the plug-in completes,  
  a breakpoint is assumed to be in place after the last instruction
  executed in the plug-in.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Must place a breakpoint right after the last instruction in the plug-in
  in order for the Debugger to get back control.

  Copyright (c) 2006-2008 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jflash_msg.h#4 $ $DateTime: 2008/06/09 12:32:26 $ $Author: c_surven $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/08   sv      Fix RVCT compiler warnings
09/22/07   op      change BUILD_FOR_INTEL by BUILD_FOR_X86
02/28/07   rt      Make compilable with GCC
12/06/06   rt      Create initial version from JFLASH 2.1
===========================================================================*/

/* -----------------------------------------------------------------------
** Include files for debug print output
** ----------------------------------------------------------------------- */

/* ABSOLUTELY do not bring in <stdio.h> or it will attempt to link
 * in code that will cause the link to fail on target.  May be OK
 * when compiling for off target unit test */
#if defined(BUILD_FOR_X86)
  #include <stdio.h>
#else
  #if defined(NOISY) || defined(TNOISY)
#error code not present
  #endif /* NOISY || TNOISY */
#endif /* UNIX_UNIT_TEST */


/* -----------------------------------------------------------------------
** Macros for debug print output
** ----------------------------------------------------------------------- */
#ifdef NOISY
#error code not present
#else
  #define DPRINTF(x) /**/
#endif /* NOISY */

#ifdef TNOISY
#error code not present
#else
  #define TPRINTF(level, x) /**/
#endif /* NOISY */


#endif /* __JFLASH_MSG_H__ */


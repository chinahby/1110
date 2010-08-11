#ifndef __FLASH_MSG_H__
#define __FLASH_MSG_H__

/**********************************************************************
 * flash_msg.h
 *
 * Flash message header. All flash driver client specific includes are
 * derived from this file. Also all flash driver .c files make sure to 
 * include this file as the last file in the include tree to override any
 * flash driver client specific macros and flags.
 *
 * Copyright (c) 2006-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_msg.h#29 $ $DateTime: 2009/03/13 11:02:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-05   jm      Revised to use Babylon
2009-03-05   mm      Fix warning explicit type is missing 
2009-02-27   mm      Simplified the CLK_BUSY_WAIT Macro
2009-02-20   mm      Simulating FLASH_CLK_BUSY_WAIT
2008-11-17   jz      Added macro FLASH_CLK_BUSY_WAIT to avoid include of clk.h
2008-07-10   jz      Added support for QDSP6
2008-05-30   mh      Add support for WM7
2008-03-26   rt      Cleanup of unused message macros
2008-02-22   mm      Updated featurization for QFIT
2008-02-12   mh      Fix compiler error on WM builds
2008-01-12   rt      Key off FLASH_SINGLE_THREADED based on external features
2007-11-27   rt      Use boot architecture independent macros for SBL
2007-11-26   rt      Featurize to compile with GCC
2007-11-21   dp      Updated usage of FLASH_COMPILETIME_FN_INIT
2007-10-30   dp      Add FEATURE_FLASH_ENABLE_TIMETEST2 dependency and 
                     FLASH_TT_OTP_READ flag
2007-09-24   op      Add macro which return a pointer to open partition function
2006-08-16   dp      Integrating quartz kernal ifdefs
2006-02-06   dp      Created
===========================================================================*/

#include "comdef.h"

/* Note that for WM mutual excusion is enusred at the FMD layer. Thus at this
 * layer WM can be treated as a single threaded client */
#ifndef FLASH_SINGLE_THREADED
  #if defined(BUILD_JFLASH) || defined(BUILD_JNAND) || \
      defined(BUILD_ARMPRG) || defined(BUILD_NANDPRG) || \
      defined(BUILD_HOSTDL) ||  defined(FEATURE_IG_EFS_EXT_SERVER) || \
      defined(FEATURE_WINCE_BOOTLOADER) || defined(BOOT_LOADER) || \
      defined(BUILD_BOOT_CHAIN) || defined(FLASH_NAND_SINGLE_THREADED) || \
      defined(FEATURE_WINCE) 
    #define FLASH_SINGLE_THREADED
  #endif
#endif

#ifdef FEATURE_IG_EFS_EXT_SERVER
  #include "efs2_ext_server_stub.h"
  #include "msmhwio.h"
  #include "msmhwioreg.h"
#endif

/*
 * NOTE:  The flash drivers are called by the file system code and the
 *  flash programming tools.  It is never appropriate to call ERR_FATAL
 *  macro directly.
 *  In the flash programming tools, ERR_FATAL is not defined at all.  In
 *  AMSS, even though ERR_FATAL is defined, it makes calls that try to log
 *  to the file system and if we just got a fatal error by being called
 *  by the file system code, then we don't want to try to use the file
 *  system to log the error.  So the wrapper macro turns off logging and
 *  then displays the error message in the normal fashion.
 *  The version of this fatal error macro for flash tools merely goes
 *  into an infinite loop.
 */
#if defined(BUILD_BOOT_CHAIN)
  #include BOOT_CUSTSBL_H
  #include BOOT_SBL_H
  #define DPRINTF(x)
  #define TPRINTF(level, x)
  #undef FEATURE_TIMETEST2
#elif defined(BUILD_JNAND)
  #include "jnand_msm.h"
  #include "jnand_msg.h"
  #define FLASH_ERR_FATAL(x,a,b,c) do { KICK_WATCHDOG(); } while (1)
  #undef FEATURE_TIMETEST2
#elif defined(BUILD_NANDPRG)
  #include "ap_armprg.h"
  #include "ap_msm.h"
  #undef MSG_HIGH
  #define MSG_HIGH(x,a,b,c) /**/
  #define FLASH_ERR_FATAL(x,a,b,c) do { KICK_WATCHDOG(); } while (1)
  #undef FEATURE_TIMETEST2
#elif defined (BUILD_JFLASH)
  #include "jfi.h"
  #include "jflash_msg.h"
  #undef MSG_HIGH
  #define MSG_HIGH(x,a,b,c) /**/
  #define FLASH_ERR_FATAL(x,a,b,c) do { KICK_WATCHDOG(); } while (1)
  #undef FEATURE_TIMETEST2
#elif defined (BUILD_ARMPRG)
  #include "ap_armprg.h"
  #include "ap_msm.h"
  #include "ap_flash.h"
  #undef MSG_HIGH
  #define MSG_HIGH(x,a,b,c) /**/
  #define FLASH_ERR_FATAL(x,a,b,c) do { KICK_WATCHDOG(); } while (1)
  #undef FEATURE_TIMETEST2
#elif defined (QUARTZ_BSP)
  #include <stdio.h>
  #include <assert.h>
  #undef FLASH_ERR_FATAL
  #define FLASH_ERR_FATAL(fmt,a,b,c) printf("ERR_FATAL"); assert(0)
  #define DPRINTF(x)
  #define TPRINTF(level, x)
  #undef FEATURE_TIMETEST2
#elif defined (FEATURE_WINCE)
#error code not present
#else
  #include "msg.h"
  #include "err.h"
  #define FLASH_ERR_FATAL(fmt,a,b,c) \
    do { \
      err_disable_fs_log (); \
      ERR_FATAL (fmt, a, b, c); \
    } while (0)
  #define DPRINTF(x)
  #define TPRINTF(level, x)
#endif

/* Simulate FLASH_CLK_BUSY_WAIT. NOTE: This will not provide accurate 
 * delay as its just a simulated version.
 */
#define FLASH_CLK_BUSY_WAIT(pause_time_us) \
  do\
  { \
    volatile int i = pause_time_us * 50; \
    if(i) \
    { \
      while (i--); \
    }\
   }while(0)
  
/* Clients that do not need open partition functions should
 * define their dependency flags so that these functions are
 * not mapped */
#if defined(BOOT_LOADER) && !defined(BUILD_JNAND) && \
    !defined(BUILD_NANDPRG) && !defined(BUILD_JNAND_WITH_OEMSBL) && \
    !defined(APPSBL) && !defined(OEMSBL) 
  #define FLASH_COMPILETIME_FN_INIT(fn)      NULL
#else
  #define FLASH_COMPILETIME_FN_INIT(fn)     fn
#endif

#ifdef FEATURE_IG_EFS_EXT_SERVER 
static void *memcpy (long *, const long *, unsigned long);
extern dword msm6550_int_cntl_safe_read( dword address, dword mask );
#endif /* FEATURE_IG_EFS_EXT_SERVER */

#ifdef IMAGE_QDSP6_PROC
  extern void* q_memcpy(void * dest, const void * src, size_t size);
#endif

#if (!defined(BOOT_LOADER) && !defined(FEATURE_WINCE_BOOTLOADER) && \
     !defined(BUILD_JNAND) && !defined(BUILD_NANDPRG) && defined(FEATURE_BABYLON))
    
  #include "customer.h"
  #include "bab_public.h"

  extern int flash_bab_enable;

  /* Flash Babylon ID values */
  #define FLASH_BAB_ERASE_BLOCK                0xA1
  #define FLASH_BAB_READ_MPAGES                0xA2
  #define FLASH_BAB_READ_PAGE                  0xA3
  #define FLASH_BAB_READ_PAGE_AND_UDATA        0xA4
  #define FLASH_BAB_READ_SPARE_BYTES           0xA5
  #define FLASH_BAB_READ_SPARE_UDATA           0xA6
  #define FLASH_BAB_WRITE_PAGE                 0xA7
  #define FLASH_BAB_WRITE_PAGE_AND_UDATA       0xA8
  #define FLASH_BAB_MEMCPY                     0xA9
  #define FLASH_BAB_DM_TRANSFER                0xAA
  #define FLASH_BAB_DM_CALLBACK                0xAB

  /* logging macros */
  #define FLASH_BAB_START(id) \
    if (flash_bab_enable) { bab_marker_start (BAB_STREAM_FLASH, (id), 0xff); }

  #define FLASH_BAB_END(id) \
    if (flash_bab_enable) { bab_marker_end (BAB_STREAM_FLASH, (id), 0xff); }

  #define FLASH_BAB_START_DM(id,cmd) \
    if (flash_bab_enable) { bab_marker_start (BAB_STREAM_FLASH, (id), cmd); }

  #define FLASH_BAB_END_DM(id,cmd) \
    if (flash_bab_enable) { bab_marker_end (BAB_STREAM_FLASH, (id), cmd); }
  
  #define FLASH_BAB_EVENT_DM(id) \
    if (flash_bab_enable) { bab_single_event (BAB_STREAM_FLASH, (id), 0); }

  #define FLASH_BAB_PORT_INIT()  _bab_port_init()

  #ifdef IMAGE_QDSP6_PROC
    #define FLASH_MEMCPY(dest, src, len) \
    { \
     FLASH_BAB_START(FLASH_BAB_MEMCPY); \
     (void)q_memcpy((dest), (src), (len)); \
     FLASH_BAB_END(FLASH_BAB_MEMCPY); \
    }
  #else
    #define FLASH_MEMCPY(dest, src, len) \
    { \
     FLASH_BAB_START(FLASH_BAB_MEMCPY); \
     (void)memcpy((dest), (src), (len)); \
     FLASH_BAB_END(FLASH_BAB_MEMCPY); \
    }
  #endif 
#else
  #define FLASH_BAB_START(x)       (void)0
  #define FLASH_BAB_END(x)         (void)0
  #define FLASH_BAB_START_DM(x,y)  (void)0
  #define FLASH_BAB_END_DM(x,y)    (void)0
  #define FLASH_BAB_EVENT_DM(x)    (void)0
  #define FLASH_BAB_PORT_INIT()    (void)0

  #ifdef IMAGE_QDSP6_PROC
  #define FLASH_MEMCPY(dest, src, length) \
    (void)q_memcpy(dest, src, length)
  #else
  #define FLASH_MEMCPY(dest, src, length) \
    (void)memcpy(dest, src, length)
#endif
#endif

#endif /* __FLASH_MSG_H__ */


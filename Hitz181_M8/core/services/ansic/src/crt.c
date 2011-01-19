/* =========================================================================

DESCRIPTION
   C Runtime Library Definitions and Declarations

   Defines and declares the supported subset of the ANSI CRT

Copyright (c) 2002 - 2003 by QUALCOMM Incorporated.  All Rights Reserved.

============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/crt.c_v   1.0   23 May 2002 22:32:40   rajeevg  $
$Header: //source/qcom/qct/core/services/ansic/main/latest/src/crt.c#2 $ $DateTime: 2008/05/01 10:08:52 $ $Author: cgross $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
04/13/07   cwg     Fixed Banned API/Lint errors
09/15/05   tjh     New qmalloc library for FEATURE_L4 builds
06/03/05    jc     Added __aeabi_atexit() for RVCT 2.1 builds.
05/13/04    sj     Featurized stdlib for L4 builds.
06/02/04    gr     Added Thin UI support.
01/27/04    gr     Added inclusion of AEE_OEMHeap.h for BREW 3.0.
08/27/03    gr     Corrected the signature of calloc.
08/25/03    gr     Added calloc.
08/22/03   jkl     Updated malloc definition.
06/11/03    lz     Added FEATURE_BREW in addition to FEATURE_BREW_LITE.
02/19/03    gr     Added malloc, free and realloc
02/06/03   kar     Fixed compile warning
05/22/02    gr     Removed zrex prefix from several symbols.
09/04/01    gr     Deleted _sys_exit stub.
01/19/00   jct     Support for using msg_xxx functions for printf
09/30/00   jct     Created
============================================================================ */

/* ------------------------------------------------------------------------
** Required CRT Variables for Win32 Environments
** ------------------------------------------------------------------------ */
#ifdef T_WINNT
#error code not present
#endif

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifdef FEATURE_MSP
#error code not present
#else
   #include "target.h"
   #include "customer.h"
#endif
#include "comdef.h"
#include "msg.h"
#include "err.h"
#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
   #include "OEMHeap.h"
   #include "OEMFeatures.h"
#if defined (FEATURE_BREW_3_0)
   #include "AEE_OEMHeap.h"
#endif
#endif

#if defined (FEATURE_UI_CORE_REMOVED) || defined (FEATURE_L4)
   #include "tmc.h"
#endif

#include "AEEstd.h"
#include "AEEVaList.h"

#if defined (FEATURE_L4)
#error code not present
#endif

#define CRT_MAX_ONEXIT_FUNCS 64

#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

#ifndef T_WINNT /* MSP or MSM */

struct __FILE
{
   unsigned long handle;
} __FILE;

FILE __stdin;
FILE __stdout;
FILE __stderr;

int rename ( const char *old_name, const char *new_name) { return 0; }
int remove ( const char *filename ) { return 0; }
void __user_initial_stackheap ( ) { return; }
void _ttywrch ( ) { return; }
int fputc( int c, FILE *stream ) { return 0; }

#endif /* !T_WINNT */

/* Msximum size of printf output buffer */
#define PRINTF_BUF_MAX_SIZE	256

char printf_buffer[ PRINTF_BUF_MAX_SIZE ];

/* ==================================================================
FUNCTION PRINTF
DESCRIPTION
  ANSI style printf
===================================================================== */
#ifndef T_WINNT /* MSP or MSM */
int _printf(const char * format, ...)
#else
#error code not present
#endif
{
   AEEVaList      args;
   msg_store_type msg_store;
   int            n;

   AEEVA_START( args, format );
   n = std_vstrlprintf( printf_buffer, PRINTF_BUF_MAX_SIZE, format, args );
   AEEVA_END( args );
#if (MSG_LEVEL!=MSG_LVL_NONE)
   (void) msg_file; /*avoid unused variable warning*/
#endif
#ifdef FEATURE_MSP
#error code not present
#else
   msg_store.level    = MSG_LVL_MED;
#endif
   msg_store.file_ptr = "ZX CRT";
   msg_store.line     = 0;
   msg_store.xx_fmt   = printf_buffer;
   msg_put( &msg_store, 0,0,0 );

   return n;
} /* END printf */


/* ==================================================================
FUNCTION FPRINTF
DESCRIPTION
  ANSI style fprintf
===================================================================== */
#ifndef T_WINNT /* MSP or MSM */
int _fprintf( FILE *stream, const char * format, ...)
#else
#error code not present
#endif
{
#ifdef  T_WINNT
#error code not present
#else
   // making fprintf doing the same thing as printf for thor
   AEEVaList        args;
   msg_store_type msg_store;
   int            n;

   AEEVA_START( args, format );
   n = std_vstrlprintf( printf_buffer, PRINTF_BUF_MAX_SIZE, format, args );
   AEEVA_END( args );
#if (MSG_LEVEL!=MSG_LVL_NONE)
   (void) msg_file; /*avoid unused variable warning*/
#endif
#ifdef FEATURE_MSP
#error code not present
#else
   msg_store.level    = MSG_LVL_MED;
#endif
   msg_store.file_ptr = "ZX CRT";
   msg_store.line     = 0;
   msg_store.xx_fmt   = printf_buffer;
   msg_put( &msg_store, 0,0,0 );

   return n;
#endif // T_WINNT

} /* END fprintf */


/* ==================================================================
FUNCTION SPRINTF
DESCRIPTION
  ANSI style sprintf
===================================================================== */
int sprintf(char *destination, const char * format, ...)
{
   AEEVaList      args;
   int            n;

   AEEVA_START( args, format );
   n = std_vstrlprintf( destination, PRINTF_BUF_MAX_SIZE, format, args );
   AEEVA_END( args );

   return n;
} /* END sprintf */


#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

#if !defined(FEATURE_L4) || !defined(FEATURE_MULTIPROCESSOR)
/* ==================================================================
FUNCTION MALLOC
DESCRIPTION
  Allocates memory from a heap. Just calls OEM_Malloc for now.
===================================================================== */
void *malloc(
   unsigned int size
)
{
#if defined (FEATURE_L4)
#error code not present
#else
#ifdef FEATURE_UI_CORE_REMOVED
   return tmc_malloc( size );
#else
#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
   return OEM_Malloc( size );
#else
   ERR_FATAL ("malloc not implemented", 0, 0, 0);
   return NULL;
#endif /* defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW) */
#endif /* defined FEATURE_UI_CORE_REMOVED */
#endif /* defined FEATURE_L4 */
} /* END malloc */

/* ==================================================================
FUNCTION CALLOC
DESCRIPTION
  Allocates memory from a heap. Just calls OEM_Malloc for now.
===================================================================== */
void *calloc(
   unsigned int num,
   unsigned int size
)
{
   unsigned int num_bytes = num * size;
   void *mem_ptr = NULL;

#if defined (FEATURE_L4)
#error code not present
#else
#ifdef FEATURE_UI_CORE_REMOVED
   mem_ptr = tmc_malloc( num_bytes );
#else
#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
   mem_ptr = OEM_Malloc( num_bytes );
#else
   ERR_FATAL ("calloc not implemented", 0, 0, 0);
#endif /* defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW) */
#endif /* defined FEATURE_UI_CORE_REMOVED */
#endif /* defined FEATURE_L4 */

   if (mem_ptr)
      memset( mem_ptr, 0x00, num_bytes );
   return mem_ptr;
} /* END calloc */

/* ==================================================================
FUNCTION FREE
DESCRIPTION
  Frees allocated memory. Just calls OEM_Free for now.
===================================================================== */
void free(
   void *ptr
)
{
#if defined (FEATURE_L4)
#error code not present
#else
#ifdef FEATURE_UI_CORE_REMOVED
   tmc_free( ptr );
#else
#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
   OEM_Free( ptr );
#else
   ERR_FATAL ("free not implemented", 0, 0, 0);
#endif /* defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW) */
#endif /* defined FEATURE_UI_CORE_REMOVED */
#endif /* defined FEATURE_L4 */
} /* END free */

/* ==================================================================
FUNCTION REALLOC
DESCRIPTION
  Reallocates memory. Just calls OEM_Realloc for now.
===================================================================== */
void *realloc(
   void  *ptr,
   unsigned int size
)
{
#ifdef FEATURE_UI_CORE_REMOVED
   #ifdef FEATURE_L4
#error code not present
   #else
     return tmc_realloc( ptr, size );
   #endif
#else
#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
   #ifdef FEATURE_L4
#error code not present
   #else
     return OEM_Realloc( ptr, size );
   #endif
#else
   ERR_FATAL ("realloc not implemented", 0, 0, 0);
   return NULL;
#endif /* defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW) */
#endif /* defined FEATURE_UI_CORE_REMOVED */
} /* END realloc */

/* ==================================================================
FUNCTION __aeabi_atexit
DESCRIPTION
  Registers a C++ destructor to be called at program exit.
  The return value should be non-zero to indicate success.

  This function is a feature of the ARM RVCT compiler, and was
  not present in the ARM ADS compiler.  This function is called
  from initialization code for each statically declared C++
  object that has a destructor.  The initialization code runs
  early in the boot process, called from the ARM library run-time
  initialization function.

  The ARM library implementation of __aeabi_atexit() calls malloc(),
  but the heap has not been initialized at this point in the boot process.
  Consequently, we must replace the library version __aeabi_atexit().
  Since AMSS does not exit, __aeabi_atexit() can be a simple stub.
===================================================================== */
/* If ARM compiler is version 2.1.0 (RVCT) or greater */
#ifdef __ARMCC_VERSION
#if (__ARMCC_VERSION >= 210000)
int __aeabi_atexit(
   void* object,
   void (*destroyer)(void*),
   void* dso_handle
)
{
   return 1;
}
#endif  /* __ARMCC_VERSION >= 200000 */
#endif  /* __ARMCC_VERSION */

#endif /* !defined(FEATURE_L4) || !defined(FEATURE_MULTIPROCESSOR) */

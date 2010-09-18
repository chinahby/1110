/*======================================================
FILE:  AEEComdef.h

SERVICES:  Definitions of Basic data structures used in AEE.

GENERAL DESCRIPTION:
	Base level data structure definitions, typedefs, etc. for AEE

Copyright (c) 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

/* guard against re-inclusion */
#if !defined(AEECOMDEF_H)
#define AEECOMDEF_H

/* GCC packed works a little differently */
#if !defined(GCCPACKED)
// For the GCC compiler
# if defined(__GNUC__)
#  define GCCPACKED  __attribute__ ((packed))
# else  //__GNUC__
#  define GCCPACKED 
# endif //__GNUC__
#endif // ifndef GCCPACKED

/* Make sure exports are only done when building for Windows */
#if defined(AEE_SIMULATOR)
# ifdef AEE_OEM_EXPORTS
#  define AEE_EXPORTS __declspec(dllexport)
# else
#  define AEE_EXPORTS __declspec(dllimport)
# endif // AEE_OEM_EXPORTS
#else
# define AEE_EXPORTS
#endif // AEE_SIMULATOR

/* Make sure exports are only done when building for Windows */
#if defined(AEE_SIMULATOR)
# ifdef BREWSTATICEXTENSION_EXPORTS
#  define BREWSTATICEXTENSION_API __declspec(dllexport)
# else
#  define BREWSTATICEXTENSION_API 
# endif //BREWSTATICEXTENSION_EXPORTS
#else 
# define  BREWSTATICEXTENSION_API 
#endif // AEE_SIMULATOR

#if !defined(MAX_UINT8)
#define MAX_UINT8 0xff
#endif

#if !defined(MAX_INT8)
#define MAX_INT8 0x7f
#endif

/* =========================================================================
   guard against handset comdef.h's, because they define most of the below
   unconditionally
   ========================================================================= */

/* =========================================================================
             "S T A N D A R D"    D E C L A R A T I O N S

DEFINED TYPES

       Name      Definition
       -------   --------------------------------------------------------
       byte      8  bit unsigned value
       word      16 bit unsigned value 
       dword     32 bit unsigned value

       uint1     byte
       uint2     word
       uint4     dword

       uint8     8  bit unsigned value
       uint16    16 bit unsigned value
       uint32    32 bit unsigned value
       uint64    64 bit unsigned value

       int8      8  bit signed value
       int16     16 bit signed value 
       int32     32 bit signed value
       int 64    64 bit signed value

       sint31    32 bit signed value 
       sint15    16 bit signed value
       sint7     8  bit signed value

       int1      8  bit signed value
       int2      16 bit signed value
       int4      32 bit signed value

       boolean   8 bit boolean value

DEFINED CONSTANTS

       Name      Definition
       -------   --------------------------------------------------------
       TRUE      Asserted boolean condition (Logical 1)
       FALSE     Deasserted boolean condition (Logical 0)
 
       ON        Asserted condition
       OFF       Deasserted condition

       NULL      Pointer to nothing

       PACKED    Used to indicate structures which should use packed
                 alignment 

       INLINE    Used to inline functions for compilers which support this

===========================================================================*/
/* Standard declarations */
#include "../../inc/AEEStdDef.h"

#if !defined(COMDEF_H)

#ifndef _WORD_DEFINED
typedef  unsigned short    word;         /* Unsigned 16 bit value type. */
#define _WORD_DEFINED
#endif

#ifndef _DWORD_DEFINED
typedef  unsigned long     dword;        /* Unsigned 32 bit value type. */
#define _DWORD_DEFINED
#endif

#ifndef _UINT1_DEFINED
typedef  unsigned char     uint1;        /* Unsigned 8  bit value type. */
#define _UINT1_DEFINED
#endif

#ifndef _UINT2_DEFINED
typedef  unsigned short    uint2;        /* Unsigned 16 bit value type. */
#define _UINT2_DEFINED
#endif

#ifndef _UINT4_DEFINED
typedef  unsigned long     uint4;        /* Unsigned 32 bit value type. */
#define _UINT4_DEFINED
#endif

#ifndef _INT1_DEFINED
typedef  signed char       int1;         /* Signed 8  bit value type. */
#define _INT1_DEFINED
#endif

#ifndef _INT2_DEFINED
typedef  signed short      int2;         /* Signed 16 bit value type. */
#define _INT2_DEFINED
#endif

#ifndef _INT4_DEFINED
typedef  long int          int4;         /* Signed 32 bit value type. */
#define _INT4_DEFINED
#endif

#ifndef _SINT31_DEFINED
typedef  signed long       sint31;       /* Signed 32 bit value */
#define _SINT31_DEFINED
#endif

#ifndef _SINT15_DEFINED
typedef  signed short      sint15;       /* Signed 16 bit value */
#define _SINT15_DEFINED
#endif

#ifndef _SINT7_DEFINED
typedef  signed char       sint7;        /* Signed 8  bit value */
#define _SINT7_DEFINED
#endif

/*===========================================================================

                            Data Declarations

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#ifndef ON
#define  ON   1    /* On value. */
#endif
#ifndef OFF
#define  OFF  0    /* Off value. */
#endif

#ifndef NULL
  #define NULL  0
#endif

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */


/* ---------------------------------------------------------------------
** Compiler Keyword Macros
** --------------------------------------------------------------------- */ 

#ifdef __ARMCC_VERSION
  #define PACKED __packed
#else
  #define PACKED
#endif

/* ----------------------------------------------------------------------
**                          STANDARD MACROS
** ---------------------------------------------------------------------- */

#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#ifndef ABS
   #define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

#if !defined(ARR_SIZE)
   #define ARR_SIZE(a)  (sizeof((a))/sizeof((a[0])))
#endif /* #if !defined(ARR_SIZE) */

#endif /* #if !defined(COMDEF_H) */

/* 
   common stuff used after platform defs 
*/

typedef void *(*PFNREALLOC)(void *pv, uint32 nSize);


// NOTE: The following type is used for declaration of bit-fields. This
// is done so that comparisons to booleans works correctly in both Win and ARM
// environments.
//
#ifndef _FLG_DEFINED
typedef unsigned           flg;
#define _FLG_DEFINED
#endif

typedef void (*PFNNOTIFY)(void *);

#ifndef PSTR /* implies null-terminated string */
typedef  char * PSTR;
#endif

#ifndef PCSTR /* implies null-terminated string */
typedef  const char * PCSTR;
#endif

#endif /* #if !defined(AEECOMDEF_H) */

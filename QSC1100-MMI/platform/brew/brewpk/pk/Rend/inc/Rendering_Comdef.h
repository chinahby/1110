//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_Comdef.h
//Author:       AnyLook
//Version:      1.0
//Date:         2008/07/31
//Description:
//    Rendering Include file
//Others: 
//Function List: 
//<Name>        <desc>
//Simple        
//History: 
//<author>      <time>      <version >  <desc>
// AnyLook    2009/01/09       1.0       build this moudle  
//**************************************************************

#if !defined(RENDERINGCOMDEF_H)
#define RENDERINGCOMDEF_H


// GCC packed works a little differently 
#if !defined(GCCPACKED)
// For the GCC compiler
# if defined(__GNUC__)
#  define GCCPACKED  __attribute__ ((packed))
# else  //__GNUC__
#  define GCCPACKED 
# endif //__GNUC__
#endif // ifndef GCCPACKED

// Make sure exports are only done when building for Windows 
#if defined(WIN32)
    #define xOS_EXPORTS __declspec(dllexport)
#else
    #define xOS_EXPORTS
#endif // WIN32

#ifdef __GNUC__
#define INLINE __inline__
#else
#if defined(_MSC_VER) || defined(__BORLANDC__) || \
    defined(__DMC__) || defined(__SC__) || \
    defined(__WATCOMC__) || defined(__LCC__) || \
    defined(__DECC) || defined(__EABI__) || defined(__CC_ARM)
#ifndef INLINE
#define INLINE	__inline
#endif
#else
#if !defined(__MRC__) && !defined(_SGI_SOURCE)
#ifndef INLINE
#define INLINE inline
#endif
#endif // Not a funky compiler
#endif // Visual C++
#endif // GNU C

#if !defined(MAX_UINT8)
#define MAX_UINT8 0xff
#endif

#if !defined(MAX_INT8)
#define MAX_INT8 0x7f
#endif


// =========================================================================
//   guard against handset comdef.h's, because they define most of the below
//  unconditionally
//   ========================================================================

// =========================================================================
//             "S T A N D A R D"    D E C L A R A T I O N S
//
//DEFINED TYPES
//
//       Name      Definition
//       -------   --------------------------------------------------------
//       byte      8  bit unsigned value
//       word      16 bit unsigned value 
//       dword     32 bit unsigned value
//
//       uint1     byte
//       uint2     word
//       uint4     dword
//
//       uint8     8  bit unsigned value
//       uint16    16 bit unsigned value
//       uint32    32 bit unsigned value
//       uint64    64 bit unsigned value
//
//       int8      8  bit signed value
//       int16     16 bit signed value 
//       int32     32 bit signed value
//       int 64    64 bit signed value
//
//       sint31    32 bit signed value 
//       sint15    16 bit signed value
//       sint7     8  bit signed value
//
//       int1      8  bit signed value
//       int2      16 bit signed value
//       int4      32 bit signed value
//
//       boolean   8 bit boolean value
//
//DEFINED CONSTANTS
//
//       Name      Definition
//       -------   --------------------------------------------------------
//       TRUE      Asserted boolean condition (Logical 1)
//       FALSE     Deasserted boolean condition (Logical 0)
// 
//       ON        Asserted condition
//       OFF       Deasserted condition
//
//       NULL      Pointer to nothing
//
//       PACKED    Used to indicate structures which should use packed
//                 alignment 
//
//       INLINE    Used to inline functions for compilers which support this

//===========================================================================
// Standard declarations 
#include "Rendering_StdDef.h"

#if !defined(COMDEF_H)

#ifndef _WORD_DEFINED
typedef  unsigned short    word;         // Unsigned 16 bit value type. 
#define _WORD_DEFINED
#endif

#ifndef _DWORD_DEFINED
typedef  unsigned long     dword;       // Unsigned 32 bit value type. 
#define _DWORD_DEFINED
#endif

#ifndef _UINT1_DEFINED
typedef  unsigned char     uint1;       //Unsigned 8  bit value type. 
#define _UINT1_DEFINED
#endif

#ifndef _UINT2_DEFINED
typedef  unsigned short    uint2;       //Unsigned 16 bit value type. 
#define _UINT2_DEFINED
#endif

#ifndef _UINT4_DEFINED
typedef  unsigned long     uint4;        // Unsigned 32 bit value type. 
#define _UINT4_DEFINED
#endif

#ifndef _INT1_DEFINED
typedef  signed char       int1;         //Signed 8  bit value type. 
#define _INT1_DEFINED
#endif

#ifndef _INT2_DEFINED
typedef  signed short      int2;         // Signed 16 bit value type. 
#define _INT2_DEFINED
#endif

#ifndef _INT4_DEFINED
typedef  long int          int4;         // Signed 32 bit value type. 
#define _INT4_DEFINED
#endif

#ifndef _SINT31_DEFINED
typedef  signed long       sint32;      // Signed 32 bit value. 
#define _SINT31_DEFINED
#endif

#ifndef _SINT15_DEFINED
typedef  signed short      sint16;       // Signed 16 bit value 
#define _SINT15_DEFINED
#endif

#ifndef _SINT7_DEFINED
typedef  signed char       sint8;       //Signed 8  bit value 
#define _SINT7_DEFINED
#endif
//
//===========================================================================
//
//                            Data Declarations
//
//===========================================================================

// ------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------ 
#ifndef ON
#define  ON   1    // On value. 
#endif
#ifndef OFF
#define  OFF  0    //Off value. 
#endif

#ifndef NULL
  #define NULL  0
#endif

// -----------------------------------------------------------------------
// Standard Types
//----------------------------------------------------------------------- 


//---------------------------------------------------------------------
// Compiler Keyword Macros
//---------------------------------------------------------------------  

#ifdef __ARMCC_VERSION
  #define PACKED __packed
#else
  #define PACKED
#endif

//----------------------------------------------------------------------
//                          STANDARD MACROS
// ---------------------------------------------------------------------

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
#endif // #if !defined(ARR_SIZE) 

#endif // #if !defined(COMDEF_H) 

// NOTE: The following type is used for declaration of bit-fields. This
// is done so that comparisons to booleans works correctly in both Win and ARM
// environments.

#ifndef _FLG_DEFINED
typedef unsigned           flg;
#define _FLG_DEFINED
#endif

#ifndef PSTR // implies null-terminated string 
typedef  char * PSTR;
#endif

#ifndef PCSTR //implies null-terminated string 
typedef  const char * PCSTR;
#endif

#endif // #if !defined(RENDERINGCOMDEF_H) 

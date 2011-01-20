/*
  ========================================================================

  FILE:  AEEComUtil.h
  
  SERVICES: 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEECOMUTIL_H__
#define __AEECOMUTIL_H__

#include <stdarg.h>
#include "AEEComdef.h"

// clean casting macro
#ifndef CAST
#define CAST(t, exp)	((t)(void*)(exp))
#endif

// Replacement for legacy ADS/Windows va_list * type for use in VTable
// API functions. It is similar but slightly different from AEEOldVaList
// found in BREW 3.1.5+. This locks the API so that ADS- and Windows-
// compiled code should be both source and binary compatible, while
// RVCT-compiled Widgets will at least be *binary* compatible (meaning
// client code compiled with RVCT must replace va_list * with VaListPtrType)
#if defined(__arm)
typedef int *(*VaListPtrType)[1];
#else
typedef va_list *VaListPtrType;
#endif

// Helper functions to convert to/from va_list and VaListPtrType
static __inline VaListPtrType VaListPtr_From_va_list(va_list *args) {
#if (__ARMCC_VERSION >= 200000) && (!defined (__APCS_ADSABI))
   /* to maintain binary compatibility with our ABI */
   return CAST(VaListPtrType, &((*args).__ap));
#else /* ADS 1.2, or RVCT with adsabi, Windows or anything else */
   return CAST(VaListPtrType, args);
#endif
}

static __inline void VaListPtr_To_va_list(VaListPtrType pArgs, va_list *args) {
#if (__ARMCC_VERSION >= 200000) && !defined (__APCS_ADSABI)
   (*args).__ap = (void *)*((void **)pArgs);
#elif defined(__arm)
   /* ADS 1.2, or RVCT with adsabi, which uses array-of-1-pointer type */
   **args = **pArgs;
#else /* everything else, assuming simple stack pointer */
   *args = (va_list)*pArgs;
#endif
}


// Signed to signed.

static __inline int8    int16toint8(int16 n16) {
   return (int8)n16;
}

static __inline int8    int32toint8(int32 n32) {
   return (int8)n32;
}

#if 0
static __inline int8    int64toint8(int64 n64) {
   return (int8)n64;
}
#endif

static __inline int16   int32toint16(int32 n32) {
   return (int16)n32;
}

#if 0
static __inline int16   int64toint16(int64 n64) {
   return (int16)n64;
}
#endif

#if 0
static __inline int32   int64toint32(int64 n64) {
   return (int32)n64;
}
#endif

// Signed to unsigned.

static __inline uint8   int8touint8(int8 n8) {
   return (uint8)n8;
}

static __inline uint8   int16touint8(int16 n16) {
   return (uint8)n16;
}

static __inline uint8   int32touint8(int32 n32) {
   return (uint8)n32;
}

#if 0
static __inline uint8   int64touint8(int64 n64) {
   return (uint8)n64;
}
#endif

static __inline uint16  int16touint16(int16 n16) {
   return (uint16)n16;
}

static __inline uint16  int32touint16(int32 n32) {
   return (uint16)n32;
}

#if 0
static __inline uint16  int64touint16(int64 n64) {
   return (uint16)n64;
}
#endif

static __inline uint32  int32touint32(int32 n32) {
   return (uint32)n32;
}

#if 0
static __inline uint32  int64touint32(int64 n64) {
   return (uint32)n64;
}
#endif

#if 0
static __inline uint64  int64touint64(int64 n64) {
   return (uint64)n64;
}
#endif

// Unsigned to signed.

static __inline int8    uint8toint8(uint8 u8) {
   return (int8)u8;
}

static __inline int8    uint16toint8(uint16 u16) {
   return (int8)u16;
}

static __inline int8    uint32toint8(uint32 u32) {
   return (int8)u32;
}

#if 0
static __inline int8    uint64toint8(uint64 u64) {
   return (int8)u64;
}
#endif

static __inline int16   uint16toint16(uint16 u16) {
   return (int16)u16;
}

static __inline int16   uint32toint16(uint32 u32) {
   return (int16)u32;
}

#if 0
static __inline int16   uint64toint16(uint64 u64) {
   return (int16)u64;
}
#endif

static __inline int32   uint32toint32(uint32 u32) {
   return (int32)u32;
}

#if 0
static __inline int32   uint64toint32(uint64 u64) {
   return (int32)u64;
}
#endif

#if 0
static __inline int64   uint64toint64(uint64 u64) {
   return (int64)u64;
}
#endif

// Unsigned to unsigned.

static __inline uint8   uint16touint8(uint16 u16) {
   return (uint8)u16;
}

static __inline uint8   uint32touint8(uint32 u32) {
   return (uint8)u32;
}

#if 0
static __inline uint8   uint64touint8(uint64 u64) {
   return (uint8)u64;
}
#endif

static __inline uint16  uint32touint16(uint32 u32) {
   return (uint16)u32;
}

#if 0
static __inline uint16  uint64touint16(uint64 u64) {
   return (uint16)u64;
}
#endif

#if 0
static __inline uint32  uint64touint32(uint64 u64) {
   return (uint32)u64;
}
#endif

// Custodial Pointer
#define CUSTODIALPTR_SEM(func,pos)	/*lint -sem(func,custodial(pos)) */
#define CUSTODIALPTR			/*lint -e429 */

#endif /* __AEECOMUTIL_H__ */

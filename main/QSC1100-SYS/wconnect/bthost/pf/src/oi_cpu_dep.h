#ifndef _OI_CPU_DEP_H
#define _OI_CPU_DEP_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_cpu_dep.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 * This file contains definitions for characteristics of the target CPU and
 * compiler, including primitive data types and endianness. 
 * 
 * This file defines the byte order and primitive data types for various 
 * CPU families. The preprocessor symbol 'CPU' must be defined to be an 
 * appropriate value or this header will generate a compile-time error.
 *
 * @note The documentation for this header file uses the x86 family of processors 
 * as an illustrative example for CPU/compiler-dependent data type definitions. 
 * Go to the source code of this header file to see the details of primitive type 
 * definitions for each platform.
 *
 * Additional information is available in the @ref data_types_docpage section.
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** @name Definitions indicating family of target OI_CPU_TYPE
 *  @{
 */

#ifndef OI_CODE
  /* <PG> Inclded to get OI_CPU_TYPE defintion and keep lint happy */
  #include "btpfi.h"
#endif

#define OI_CPU_X86         1 /**< x86 processor family */ 
#define OI_CPU_ARM         2 /**< ARM processor family */
#define OI_CPU_ARC         3 /**< ARC processor family */
#define OI_CPU_SH3         4 /**< Hitachi SH-3 processor family */
#define OI_CPU_H8          5 /**< Hitachi H8 processor family */
#define OI_CPU_MIPS        6 /**< MIPS processor family */
#define OI_CPU_SPARC       7 /**< SPARC processor family */
#define OI_CPU_M68000      8 /**< Motorola M68000 processor family */
#define OI_CPU_PPC         9 /**< PowerPC (PPC) processor family */
#define OI_CPU_SH4_7750   10 /**< Hitachi SH7750 series in SH-4 processor family */
#define OI_CPU_SH2        11 /**< Hitachi SH-2 processor family */
#define OI_CPU_ARM7_LEND  12 /**< ARM7, little-endian */
#define OI_CPU_ARM7_BEND  13 /**< ARM7, big-endian */
#define OI_CPU_GDM1202    14 /**< GCT GDM1202 */
#define OI_CPU_ARC_LEND   15 /**< ARC processor family */
#define OI_CPU_ARC_BEND   16 /**< ARC processor family */
#define OI_CPU_M30833F    17 /**< Mitsubishi M308 processor family */
#define OI_CPU_CR16C      18 /**< National Semiconductor 16 bit processor family */
#define OI_CPU_M64111     19 /**< Renesas M64111 processor (M32R family) */

#ifndef OI_CPU_TYPE
    #error "OI_CPU_TYPE type not defined"
#endif

/**@}*/


/** @name Definitions indicating byte-wise endianness of target CPU
 *  @{
 */

#define OI_BIG_ENDIAN_BYTE_ORDER    0  /**< Multiple-byte values are stored in memory beginning with the most significant byte at the lowest address.  */ 
#define OI_LITTLE_ENDIAN_BYTE_ORDER 1  /**< Multiple-byte values are stored in memory beginning with the least significant byte at the lowest address. */

/**@}*/


/** @name  CPU/compiler-independent primitive data type definitions
 *  @{
 */

typedef int             OI_BOOL;  /**< Boolean values use native integer data type for target CPU. */ 
typedef int             OI_INT;   /**< Integer values use native integer data type for target CPU. */
typedef unsigned int    OI_UINT;  /**< Unsigned integer values use native unsigned integer data type for target CPU. */
typedef unsigned char   OI_BYTE;  /**< Raw bytes type uses native character data type for target CPU. */

/**@}*/



/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_X86
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH3
#error code not present
#endif
/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH2
#error code not present
#endif
/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_H8
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_MIPS
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SPARC
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M68000
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_PPC
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_SH4_7750
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM7_LEND
#define OI_CPU_BYTE_ORDER OI_LITTLE_ENDIAN_BYTE_ORDER

/** @name   little-endian CPU/compiler-dependent primitive data type definitions for the ARM7 processor family 
 *  @{
 */

typedef signed char     OI_INT8;   /**< 8-bit signed integer values use native signed character data type for ARM7 processor. */
typedef signed short    OI_INT16;  /**< 16-bit signed integer values use native signed short integer data type for ARM7 processor. */
typedef signed long     OI_INT32;  /**< 32-bit signed integer values use native signed long integer data type for ARM7 processor. */
typedef unsigned char   OI_UINT8;  /**< 8-bit unsigned integer values use native unsigned character data type for ARM7 processor. */
typedef unsigned short  OI_UINT16; /**< 16-bit unsigned integer values use native unsigned short integer data type for ARM7 processor. */
typedef unsigned long   OI_UINT32; /**< 32-bit unsigned integer values use native unsigned long integer data type for ARM7 processor. */

/**@}*/

#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARM7_BEND
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_GDM1202
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC_LEND
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_ARC_BEND
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M30833F
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_CR16C
#error code not present
#endif

/*********************************************************************************/

#if OI_CPU_TYPE==OI_CPU_M64111
#error code not present
#endif

/*********************************************************************************/


#ifndef OI_CPU_BYTE_ORDER
    #error "Byte order (endian-ness) not defined"
#endif


/*********************************************************************************/

#endif /* _OI_CPU_DEP_H */


#ifndef BOOT_COMDEF_H
#define BOOT_COMDEF_H

/*===========================================================================

                          B o o t  L o a d e r
                 C o m m o n  D e f i n t i o n  F i l e

DESCRIPTION
  This header file contains common declarations and definitions used
  throughout the boot software.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_comdef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/08   MJS     Merged 6xxx and 7xxx versions.
04/11/05   SJD     Changed to only define LOCAL if it isn't already defined.
08/25/04   SJD     Initial revision.

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#ifndef BUILD_BOOT_CHAIN
#include "comdef.h"
#else /* BUILD_BOOT_CHAIN */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Constants
---------------------------------------------------------------------------*/

#define TRUE   1   /* Boolean true value */
#define FALSE  0   /* Boolean false value */

#define  ON   1    /* On value */
#define  OFF  0    /* Off value */

#ifdef _lint
  #define NULL 0   
#endif

#ifndef NULL
  #define NULL  0
#endif

/*---------------------------------------------------------------------------
  Standard Types
---------------------------------------------------------------------------*/

/* The following definitions are the same accross platforms.  The first
   group are the sanctioned types.  */

#ifdef BOOT_IMEM_32BIT_ACCESS
typedef  unsigned int       boolean;     /* Boolean value type. */
#else /* BOOT_IMEM_32BIT_ACCESS */
typedef  unsigned char      boolean;     /* Boolean value type. */
#endif /* BOOT_IMEM_32BIT_ACCESS */

typedef  unsigned long long uint64;      /* Unsigned 64 bit value */
#ifndef FEATURE_DSP //add by yangdecai 09-25
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
#else
#ifndef   uint32
#define   uint32        unsigned long int
#endif /* uint32 */

#ifndef   uint16
#define   uint16        unsigned short
#endif /* uint16 */

#endif
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */

typedef  signed long int    int32;       /* Signed 32 bit value */
typedef  signed short       int16;       /* Signed 16 bit value */
typedef  signed char        int8;        /* Signed 8  bit value */

/* This group are the deprecated types.  Their use should be
   discontinued and new code should use the types above. */

typedef  unsigned char     byte;         /* Unsigned 8  bit value type. */
typedef  unsigned short    word;         /* Unsinged 16 bit value type. */
typedef  unsigned long     dword;        /* Unsigned 32 bit value type. */

typedef  unsigned char     uint1;        /* Unsigned 8  bit value type. */
typedef  unsigned short    uint2;        /* Unsigned 16 bit value type. */
typedef  unsigned long     uint4;        /* Unsigned 32 bit value type. */

typedef  signed char       int1;         /* Signed 8  bit value type. */
typedef  signed short      int2;         /* Signed 16 bit value type. */
typedef  long int          int4;         /* Signed 32 bit value type. */

/*---------------------------------------------------------------------------
  Local definition
---------------------------------------------------------------------------*/

#ifndef LOCAL
#define LOCAL static 
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif
#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#endif /* BUILD_BOOT_CHAIN */

#endif /* BOOT_COMDEF_H */


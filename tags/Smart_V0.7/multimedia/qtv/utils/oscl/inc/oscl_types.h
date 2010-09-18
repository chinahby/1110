#ifndef OSCL_TYPES_H
#define OSCL_TYPES_H
/* =======================================================================
                               oscl_types.h
DESCRIPTION
  This file contains basic type definitions for common use across platforms.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_types.h#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
extern "C"
{
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"
}

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifndef __cplusplus
#ifndef bool
//! The bool type is mapped to an integer.
typedef int bool;
#endif

#else // __cplusplus

//! The NULL_TERM_CHAR is used to terminate c-style strings.
const char NULL_TERM_CHAR = '\0';

#endif

#define BYTE_ORDER_LITTLE_ENDIAN
#if( !defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(BYTE_ORDER_BIG_ENDIAN) )
# error "Must specify valid endianness"
#endif /* !LITTLE_ENDIAN && !BIG_ENDIAN */

//! mbchar is multi-byte char with null termination.
#ifndef mbchar
typedef char mbchar;
#endif

//! OSCL_CHAR is used for regular (8-bit) characters
typedef char OSCL_CHAR;

// The definition of the MemoryFragment will probably
// be OS-dependant since the goal is to allow this data
// structure to be passed directly to I/O routines that take
// scatter/gather arrays.
#if defined(PV_OS_UNIX)
#include <unistd.h>
struct MemoryFragment {
  void *ptr;
  size_t len;
};

#else
struct MemoryFragment {
  uint32 len;
  void *ptr;
};
#endif


//! oscl_status_t defines return codes for oscl functions
/* OSCL Return Code Definitions */
typedef enum
{
  SUCCESS,
  FAIL,
  ETIMEOUT,
  ECONNREF,
  EINVALIDADDRESS,
  ENOPORT,
  EBADSOCK,
  ECANTBIND,
  ECANTLISTEN,
  EINDEXOUTOFBOUND,
  EMEMORY
} oscl_status_t;

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef octet
typedef uint8 octet;
#endif

/* Use PV_OS_VERSION to specify the OS version */

#if !defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(BYTE_ORDER_BIG_ENDIAN)
#error must define either BYTE_ORDER_LITTLE_ENDIAN or BYTE_ORDER_BIG_ENDIAN
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  SAMPLE_FUNC

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */

#endif

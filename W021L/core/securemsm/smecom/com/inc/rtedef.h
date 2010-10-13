#ifndef RTEDEF_H
#define RTEDEF_H
/** @file rtedef.h
*/

/*===========================================================================

DESCRIPTION
  This header file contains general types and macros that are of use
  to all modules.  

DEFINED TYPES

       Name      Definition
       -------   --------------------------------------------------------
       uint8     8  bit unsigned value
       uint16    16 bit unsigned value
       uint32    32 bit unsigned value
       uint64    64 bit unsigned value

       int8      8  bit signed value
       int16     16 bit signed value
       int32     32 bit signed value
       int64     64 bit signed value

       boolean   8 bit boolean value

DEFINED CONSTANTS

       Name      Definition
       -------   --------------------------------------------------------
       TRUE      Asserted boolean condition (Logical 1)
       FALSE     Deasserted boolean condition (Logical 0)

       NULL      Pointer to nothing

       PACKED    Used to indicate structures which should use packed
                 alignment

       INLINE    Used to inline functions for compilers which support this

Copyright (c) 2005      by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/rtedef.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/06    rv     Fixed compilation errors
05/03/05   jct     Recreated from comdef.h

===========================================================================*/
#include "customer.h"
#define __CPLUSPLUS_GUARD_BEGIN  extern "C" {
#define __CPLUSPLUS_GUARD_END    }

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif
/** @brief Standard Types
*/
typedef void* rte_opaque_address_type; ///{ Opaque address

/** @brief Quick and dirty status enum

    This enumeration defines the possible status codes that can be returned
    from a method conforming to the conventions of the aIDunno interface.
    Normally, aIDunno-style methods should return a status code, and send
    any results of the function call back to the caller through out
    parameters.
*/
enum rte_result_type
{
  RTE_OK,             ///< Operation succeeded
  RTE_FAIL,           ///< General failure
  RTE_NO_INTERFACE,   ///< Requested interface not found
  RTE_NULL_POINTER,   ///< NULL pointer provided as an out parameter
  RTE_NO_MEMORY,      ///< Memory allocation failure
  RTE_UNAVAILABLE,    ///< Requested service is unavailable
  RTE_UNSUPPORTED     ///< Requested service is not supported
};

#ifdef __cplusplus
	// Namespaces
	#ifdef __arm
      #define NAMESPACE( ns )
   #else
      #define NAMESPACE( ns ) using namespace ns;
   #endif

	// Dynamic exports
   #ifdef WIN32
      #define RTE_EXPORT __declspec(dllexport)
   #else
      #define RTE_EXPORT 
   #endif
#endif

// CORBA typedefs pulled from idl4 for non-ARM targets
#ifndef __arm
typedef int CORBA_TypeCode;
typedef short CORBA_short;
typedef int CORBA_long;
typedef long long CORBA_long_long;
typedef unsigned short CORBA_unsigned_short;
typedef unsigned int CORBA_unsigned_long;
typedef unsigned long long CORBA_unsigned_long_long;
typedef float CORBA_float;
typedef double CORBA_double;
typedef long double CORBA_long_double;
typedef unsigned char CORBA_unsigned_char;
typedef char CORBA_char;
typedef short CORBA_wchar;
typedef unsigned char CORBA_boolean;
typedef unsigned char CORBA_octet;
typedef struct CORBA_any { CORBA_TypeCode _type;void *_value; } CORBA_any;
typedef unsigned int CORBA_enum; /* always 32bit, even on 64bit platforms */
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1   ///{ Boolean true value
#define FALSE  0   ///{ Boolean false value

/* ---------------------------------------------------------------------
** Compiler Keyword Macros
** --------------------------------------------------------------------- */
#define INLINE __inline

#if (defined T_WINNT) || (defined TARGET_OS_SOLARIS)
#error code not present
#else

#if (defined T_QSC60X5) || (defined T_MSM6280)
#ifndef _INT64_DEFINED
  typedef signed long long int64;
  #define _INT64_DEFINED
#endif /* !_INT64_DEFINED */
#ifndef _UINT64_DEFINED
  typedef unsigned long long uint64;
  #define _UINT64_DEFINED
#endif /* !_UINT64_DEFINED */
 #else  /* T_QSC60X5 || T_MSM6280 */
#error code not present
#endif  /* T_QSC60X5 || T_MSM6280 */
#define CDECL
#define PACKED __packed

#endif /* T_WINNT || TARGET_OS_SOLARIS */

#ifdef _lint
#define __packed
#endif

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif
#endif // RTEDEF_H


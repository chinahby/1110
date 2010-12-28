#ifndef QTVSYSTEM_H
#define QTVSYSTEM_H
/* =======================================================================
                               qtvsystem.h
DESCRIPTION
   This file provides definitions for the QTV Tasks.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/system/main/latest/inc/qtvsystem.h#1 $
$DateTime: 2008/05/08 11:03:24 $
$Change: 656211 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifdef __cplusplus
extern "C"
{
#endif
#include <comdef.h>  // common type definitions
#include <rex.h>
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "err.h" 
#include "AEEstd.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*********************************
*
* Task Priorities
*
**********************************/
#ifdef TIMETEST
// Minimum QTV Priority (not including the decoder thread)
// This value is used by timetest builds to determine
// which LEDs to use for each QTV thread
#define QTV_BASE_PRIORITY QTVParserPriority
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
extern rex_priority_type QTVDSPTaskPriority;
extern rex_priority_type QTVPlayerPriority;
extern rex_priority_type QTVAudioPriority;
extern rex_priority_type QTVStreamerPriority;
extern rex_priority_type QTVRendererPriority;
extern rex_priority_type QTVDecoderPriority;
extern int32 qtv_dummy_var;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
extern rex_priority_type QTVDownloaderPriority;
#endif

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
extern rex_priority_type QTVTimedTextPriority;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

extern rex_priority_type QTVParserPriority;
extern rex_priority_type QTVBcastXmitPriority;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/*************************************************************************
 * The following macro definitions are the only dynamic memory allocation
 * routines that are to be used in Qtv.
 ************************************************************************* */
  
/* =======================================================================
MACRO QTV_Malloc

ARGS 
  n - the number of bytes to allocate

DESCRIPTION:
  This macro calls qtv_malloc in order to allocate memory from the heap and
  track memory allocations for debugging purposes.  A pointer to the allocated
  memory is returned.
========================================================================== */
#define QTV_Malloc(n)       qtv_malloc((n), __FILE__, __LINE__)

/* =======================================================================
MACRO QTV_Realloc

ARGS
  p - original pointer returned by QTV_Malloc
  n - number of bytes to reallocate

DESCRIPTION:
  This macro calls qtv_realloc in order to allocate memory from the heap and
  track memory allocations for debugging purposes.  A pointer to the allocated
  memory is returned.
========================================================================== */
#define QTV_Realloc(p,n)    qtv_realloc((p), (n), __FILE__, __LINE__)

/* =======================================================================
MACRO QTV_Free

ARGS 
  p - pointer to memory which will be free'd

DESCRIPTION:
  This macro calls qtv_free in order to free memory from the heap and
  track memory allocations for debugging purposes.
========================================================================== */
#define QTV_Free(p)         qtv_free((p), __FILE__, __LINE__)

/* =======================================================================
MACRO QTV_New

ARGS 
  t - type of object to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct an
  object of the requested type.  Then qtv_new is called in order to track memory
  allocations for debugging purposes.  A pointer to the newly constructed object
  is returned.
========================================================================== */
#define QTV_New(t)          ((t*)qtv_new(new t, sizeof(t), __FILE__, __LINE__))
  
/* =======================================================================
MACRO QTV_New_Args

ARGS 
  t - type of object to allocate and construct
  a - list of arguments to pass to constructor.  MUST BE SURROUNDED BY
      PARENTHESES.

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct an
  object of the requested type passing the arguments specified.  Then qtv_new is
  called in order to track memory allocations for debugging purposes.  A pointer
  to the newly constructed object is returned.
========================================================================== */
#define QTV_New_Args(t,a)   ((t*)qtv_new(new t##a, sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO QTV_New_Array

ARGS
  t - type of object to allocate and construct
  n - number of objects to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct the
  specified number of object of the requested type.  Then qtv_new is called in
  order to track memory allocations for debugging purposes.  A pointer to the
  newly constructed array of objects is returned.
========================================================================== */
#define QTV_New_Array(t,n)  ((t*)qtv_new(new t[n], (n)*sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO QTV_New_Array_Args

ARGS
  t - type of object to allocate and construct
  a - list of arguments to pass to constructor.  MUST BE SURROUNDED BY
      PARENTHESES.
  n - number of objects to allocate and construct

DESCRIPTION:
  This macro calls the C++ 'new' operator in order to allocate and construct the
  specified number of object of the requested type with the specified
  parameters.  Then qtv_new is called in order to track memory allocations for
  debugging purposes.  A pointer to the newly constructed array of objects is
  returned.
========================================================================== */
#define QTV_New_Array_Args(t,a,n)  ((t*)qtv_new(new t##a##[n], (n)*sizeof(t), __FILE__, __LINE__))

/* =======================================================================
MACRO QTV_Delete

ARGS 
  p - pointer to object which will be destructed and free'd

DESCRIPTION:
  This macro calls the C++ 'delete' operator in order to deconstruct and free
  the specified object.  Then qtv_delete is called in order to track memory
  allocations for debugging purposes.
========================================================================== */
#define QTV_Delete(p)       qtv_delete((p), __FILE__, __LINE__);delete(p)
  
/* =======================================================================
MACRO QTV_Delete_Array

ARGS 
  p - pointer to object which will be destructed and free'd

DESCRIPTION:
  This macro calls the C++ 'delete[]' operator in order to deconstruct and free
  the specified object.  Then qtv_delete is called in order to track memory
  allocations for debugging purposes.
========================================================================== */
#define QTV_Delete_Array(p)  qtv_delete((p), __FILE__, __LINE__);delete[](p)
  
/* =======================================================================
MACRO QTV_NULL_PTR_CHECK

ARGS
  ptr - the pointer
  retval - return value in case of NULL

DESCRIPTION:
  This macro checks the ptr value against NULL.  If ptr is NULL, then it
  returns retval.  If the function has return type of void, then use RETURN_VOID.
========================================================================== */
#define RETURN_VOID /* null macro */
#define QTV_NULL_PTR_CHECK(ptr, retval)                          \
  if( NULL == (ptr) )                                            \
  {                                                              \
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,            \
      "Unexpected Null Ptr");                                    \
    return retval;                                               \
  }
/* =======================================================================
MACRO BOUNDS_CHECK_FOR_PTR(

ARGS
  X - Pointer which is going for bound check
  Y - Pointer is maximum limit of X pointer
  Z - value which is going to increment in pointer X
  retval - value assign to retval

DESCRIPTION:
  This macro checks if pointer X going beyond pointer Y + length Z
========================================================================== */

#define BOUNDS_CHECK_FOR_PTR(X, Y, Z, retval)                    \
 if( ((X) >= ((Y) + sizeof(Y) - (Z))))                           \
 {                                                               \
   return retval;                                                \
 } 
  
  /* =======================================================================

MACRO QTV_USE_ARG1

ARGS 
  arg1 - function argument parameter
  arg2 - function argument parameter
  

DESCRIPTION:
  This macro assign the function formal arguments to dummy variables for just to 
  fix compiler warning.

========================================================================== */
#define QTV_USE_ARG1(arg1)           qtv_dummy_var = (int32)arg1;

#define QTV_USE_ARG2(arg1,arg2)      qtv_dummy_var = (int32)arg1;\
                                     qtv_dummy_var = (int32)arg2;

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
  qtv_malloc

DESCRIPTION
  Allocate memory from the heap.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Malloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  none

RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* qtv_malloc(uint32 numBytes, const char *file, uint16 line);

#ifdef FEATURE_QTV_IMEM 
#error code not present
#endif

/* ======================================================================
FUNCTION 
  qtv_realloc

DESCRIPTION
  Reallocate memory of a different size from the heap.  If possible, the memory
  will be in the same location.  Otherwise the old memory will be free'd and a
  new chunk allocated.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Realloc() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  qtv_realloc will not update any of the byte counters (total, current, or max)
  unless FEATURE_QTV_MEMORY_LEAK_DEBUG is defined.
  
RETURN VALUE
  Returns a pointer to numBytes of memory or NULL if unable to allocate memory.

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void* qtv_realloc(void *pOldMemory, uint32  numBytes, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  qtv_free

DESCRIPTION
  Free memory back to the heap to be reused later.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Free() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void  qtv_free(void* pMemory, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  qtv_new

DESCRIPTION
  Allocate memory from the heap.  This function replaces the standard C++ 'new'
  operator in order to track memory usage by Qtv.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_New() To be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void *qtv_new(void* pMemory, uint32 numBytes, const char *file, uint16 line);

/* ======================================================================
FUNCTION 
  qtv_delete

DESCRIPTION
  Free memory back to the heap to be reused later.  This function replaces the
  standard C++ 'delete' operator in order to track memory usage by Qtv.

  This function should not be called directly from Qtv code.  Instead, call
  QTV_Delete() to be independent of FEATURE_QTV_MEMORY_DEBUG.

DEPENDENCIES
  This memory must have been allocated by a call to QTV_Malloc.

RETURN VALUE
  none

SIDE EFFECTS
  modifies tables to track memory leaks
  
========================================================================== */
void qtv_delete(void* pMemory, const char *file, uint16 line);
  
#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */
 
/* ======================================================================
FUNCTION
  QTV_GetTotalBytesAllocated

DESCRIPTION
  Return the total number of bytes allocated via calls to qtv_malloc and
  qtv_new.  Basically a sum total of all bytes allocated.

DEPENDENCIES
  none 

RETURN VALUE
  Total bytes allocated via QTV_Malloc and QTV_New.

SIDE EFFECTS
  none
========================================================================== */
uint32 QTV_GetTotalBytesAllocated(void);

/* ======================================================================
FUNCTION
  QTV_ClearBytesAllocated

DESCRIPTION
  Clear the total number of bytes allocated via calls to qtv_malloc and
  qtv_new.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
uint32 QTV_ClearBytesAllocated(void);

#ifdef FEATURE_QTV_MEMORY_LEAK_DEBUG
#error code not present
#endif /* FEATURE_QTV_MEMORY_LEAK_DEBUG */

#ifdef FEATURE_QTV_ASSERT
/* ======================================================================
FUNCTION
  QTV_AssertFailed

DESCRIPTION
  Empty function to place breakpoints in during de-bugging. Function is
  called before the program is halted / fatal message logged.

DEPENDENCIES
  none 

RETURN VALUE
  none

SIDE EFFECTS
  none
========================================================================== */
void QTV_AssertFailed(void);
#endif /* FEATURE_QTV_ASSERT */
  
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef FEATURE_QTV_ASSERT
volatile extern int qtv_assert_level;

/* place a breakpoint in QTV_AssertFailed for easy debugging. */
#define QTV_ASSERT(a)  \
        { \
          /*lint -save -e506 */ \
          if ((a) == FALSE) \
          /*lint -restore */ \
          { \
            QTV_AssertFailed(); \              
            if(qtv_assert_level == 0) \
            { \
               ERR_FATAL( "Assertion ", 0, 0, 0 ); \
            } \
            else \
            { \
               MSG_FATAL( "Assertion ", 0, 0, 0 ); \
            } \
          } \
        }
#else  /* FEATURE_QTV_ASSERT */
/* to completly compile the define out */
#define QTV_ASSERT(a) { }
#endif /* FEATURE_QTV_ASSERT */
  


#endif /* QTVSYSTEM_H */


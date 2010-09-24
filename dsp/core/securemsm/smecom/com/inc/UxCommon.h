#ifndef UXCOMMON_H
#define UXCOMMON_H

/*===========================================================================

                           H E A D E R  F I L E

FILE:  UxCommon.h

DESCRIPTION

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/UxCommon.h#3 $
  $DateTime: 2008/03/26 19:45:34 $
  $Author: hvimadal $  

===========================================================================*/


/*===========================================================================
                           
                                INCLUDE FILES

============================================================================*/

#include "comdef.h"
#include "stdlib.h"
#include "customer.h"
#ifdef __UXMALLOC_DEBUG__
  #include "tracker.h"
#endif

#ifdef __cplusplus
  extern "C"
  {
#endif

typedef enum
{
  UX_ALLOC_LOCAL,
  UX_ALLOC_SHARED
} UxAllocE;

typedef void (*IxContentAckFuncType) (uint32, uint32);

/*===========================================================================
FUNCTION UxMallocLocal

DESCRIPTION
  Allocates memory from a local heap.
===========================================================================*/
void *UxMallocLocal (size_t size);

/*===========================================================================
FUNCTION UxReallocLocal

DESCRIPTION
  Reallocates memory from a heap. The choice of heap depends on which PD this
  call is made from and whether it's a shared allocation. 
===========================================================================*/
void *UxReallocLocal (void *ptr, size_t size);

/*===========================================================================
FUNCTION UxMallocShared

DESCRIPTION
  Allocates memory from a shared heap.
===========================================================================*/
void *UxMallocShared (size_t size);

#ifdef __UXMALLOC_DEBUG__

/*===========================================================================
MACRO UxMallocDebug

DESCRIPTION
  Debug version of UxMalloc.
===========================================================================*/
#define UxMalloc(size, memType) \
  UxMallocDebug ((size), (memType), __FILE__, __LINE__)

/*===========================================================================
MACRO UxReallocDebug

DESCRIPTION
  Debug version of UxRealloc.
===========================================================================*/
#define UxRealloc(ptr, size) \
  UxReallocDebug ((ptr), (size), __FILE__, __LINE__)

/*===========================================================================
FUNCTION UxMallocDebug

DESCRIPTION
  Allocates memory from a heap. The choice of heap depends on which PD this
  call is made from and whether it's a shared allocation. 
===========================================================================*/
void *UxMallocDebug (size_t size, UxAllocE memType,
  const char *pFileName, unsigned int lineNumber);

/*===========================================================================
FUNCTION UxReallocDebug

DESCRIPTION
  Reallocates memory from a heap. The choice of heap depends on which PD this
  call is made from and whether it's a shared allocation. 
===========================================================================*/
void *UxReallocDebug (void *ptr, size_t size,
  const char *pFileName, unsigned int lineNumber);

#else

/*===========================================================================
FUNCTION UxMalloc

DESCRIPTION
  Allocates memory from a heap. The choice of heap depends on which PD this
  call is made from and whether it's a shared allocation. 
===========================================================================*/
void *UxMalloc (size_t size, UxAllocE memType);

/*===========================================================================
FUNCTION UxRealloc

DESCRIPTION
  Reallocates memory from a heap. The choice of heap depends on which PD this
  call is made from and whether it's a shared allocation. 
===========================================================================*/
void *UxRealloc (void *ptr, size_t size);

#endif /* __UXMALLOC_DEBUG __ */

/*===========================================================================
FUNCTION UxFree

DESCRIPTION
  Frees memory allocated by UxMalloc or UxRealloc (or the debug versions
  of these functions).
===========================================================================*/
void UxFree (void *ptr);

/* Macros for safe handling of malloc-ed memory. */
#define UX_SAFE_DELETE(p)     { if (NULL != p) { delete (p); (p) = NULL; } }
#define UX_SAFE_FREE(p)       { if (p) { UxFree((void*)p); (p) = 0; } }

#ifdef __cplusplus
  } /* extern "C" */
#endif

#endif /* UXCOMMON_H */

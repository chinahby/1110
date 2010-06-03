#ifndef __SEC_HEAP_H__
#define __SEC_HEAP_H__

/*===========================================================================

                              D R M P D   H E A P

                             H E A D E R  F I L E

sec_heap.h

GENERAL DESCRIPTION

  Functions to allocate and free memory from the sec heap.

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/sec_heap.h#1 $
  $DateTime: 2007/08/01 12:14:54 $
  $Author: jmiera $

===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "customer.h"
#include "comdef.h"

#define SEC_HEAP_SIZE (1 << 20)

/*==============================================================
* FUNCTION:  sec_heap_init()
*
* Initializes the sec heap. This function must be called before
* any allocations can be made from the heap.
==============================================================*/
void sec_heap_init (void *buf_ptr) ;

/*==============================================================
* FUNCTION:  sec_calloc()
*
* Allocates zero-initialized memory from the sec heap. Returns NULL
* on failure.
==============================================================*/
void *sec_calloc (unsigned int num, unsigned int size);

/*==============================================================
* FUNCTION:  sec_malloc()
*
* Allocates memory from the sec heap. Returns NULL on failure.
==============================================================*/
void *sec_malloc (unsigned int size);

/*==============================================================
* FUNCTION:  sec_realloc()
*
* Reallocates memory from the sec heap. Returns NULL on failure.
==============================================================*/
void *sec_realloc (void *ptr, unsigned int size);

/*==============================================================
* FUNCTION:  sec_free()
*
* Frees memory allocated from the sec heap.
==============================================================*/
void sec_free (void *ptr);

/*==============================================================
* FUNCTION:  sec_heap_ptr_is_valid()
*
* Returns 1 if the pointer passed in is in the SEC heap and 0 if
* it is not.
==============================================================*/
int sec_heap_ptr_is_valid (void *ptr);

/*==============================================================
* FUNCTION:  sec_heap_init_done()
*
* Returns 1 if the sec heap has been initialized and 0 if it is not.
==============================================================*/
int sec_heap_init_done (void);

#ifdef __cplusplus
}
#endif

#endif /* __SEC_HEAP_H__ */


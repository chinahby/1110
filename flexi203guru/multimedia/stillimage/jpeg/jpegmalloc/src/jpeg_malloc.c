
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                      E N C O D E R  AND  D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to keep track of memory usage in the 
  process of JPEG encoding and decoding. It also provides a mechanism
  to detect any memory leaks present in the JPEG module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002-2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegmalloc/main/latest/src/jpeg_malloc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/06   vma     Initial version

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "jpeg_malloc.h"        
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <stdlib.h>
#include "jpeg.h"
#include "jpegi.h"
#include "jpeg_mrk.h"
#include <memory.h>             /* Memory manipulation routines            */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/***********************************************************************/
/*                 JPEG Decoder Functions Prototype                    */
/***********************************************************************/

#ifdef JPEG_MALLOC_LOGGING
static memory_usage_logs jpeg_memory_logs;
#endif

/*===========================================================================

FUNCTION      JPEG_MALLOC

DESCRIPTION
              When used in debug mode, do malloc and keep track
              of memory allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef JPEG_MALLOC_LOGGING

/*lint -save -e123 -e578 */
void *jpeg_malloc (uint32 size, jpeg_file_id_type jpeg_file_id, uint32 linenumber)
{
  uint32 i;
  void *ptr = malloc(size);
  if (ptr)
  {
    jpeg_memory_logs.numInfoBlocks++;
    for (i=0; i< JPEG_MEMORY_LOG_COUNT; i++)
    {
      if (jpeg_memory_logs.usageInfo[i].mallocPtrs == 0)
      {
        jpeg_memory_logs.usageInfo[i].mallocPtrs    = ptr;
        jpeg_memory_logs.usageInfo[i].mallocSizes   = size;
        jpeg_memory_logs.totalSize              += size;
        jpeg_memory_logs.usageInfo[i].jpeg_file_id  = jpeg_file_id;
        jpeg_memory_logs.usageInfo[i].mallocline    = linenumber;
        jpeg_memory_logs.usageInfo[i].freeline      = 0;

        if (jpeg_memory_logs.totalSize > jpeg_memory_logs.maxSize)
          jpeg_memory_logs.maxSize = jpeg_memory_logs.totalSize;

        MSG_HIGH("JPEG MALLOC: (# malloc-ed) %d (total size) %d (max size) %d", 
                 jpeg_memory_logs.numInfoBlocks, jpeg_memory_logs.totalSize, jpeg_memory_logs.maxSize);

        break;
      }
    }
    if (i == JPEG_MEMORY_LOG_COUNT)
    {
      MSG_ERROR("JPEG MALLOC: 50+ un-freed mallocs, possible memory leaks! (# malloc-ed) %d \
                (total size) %d (max size) %d", 
                jpeg_memory_logs.numInfoBlocks, jpeg_memory_logs.totalSize, jpeg_memory_logs.maxSize);
    }
  }
  return ptr;
}
/*lint -restore */


/*===========================================================================

FUNCTION      JPEG_FREE

DESCRIPTION
              When used in debug mode, do free and keep track
              of memory de-allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void jpeg_free(void *ptr, uint32 linenumber)
{
  uint32 i;
  boolean found_ptr = FALSE;

  if (ptr)
  {    
    jpeg_memory_logs.numInfoBlocks--;
    for (i=0; i< JPEG_MEMORY_LOG_COUNT; i++)
    {
      if (jpeg_memory_logs.usageInfo[i].mallocPtrs == ptr)
      {
        found_ptr = TRUE;
        jpeg_memory_logs.usageInfo[i].mallocPtrs = NULL;
        jpeg_memory_logs.usageInfo[i].freeline   = linenumber;
        jpeg_memory_logs.totalSize              -= jpeg_memory_logs.usageInfo[i].mallocSizes;
        MSG_HIGH("JPEG FREE:   (# malloc-ed) %d (total size) %d (max size) %d", 
                 jpeg_memory_logs.numInfoBlocks, jpeg_memory_logs.totalSize, jpeg_memory_logs.maxSize);

        break;
      }
    }
    free(ptr);
    if (found_ptr == FALSE)
    {
      /* It is possible that malloc may not be logged because all
       * entries are used. */
      MSG_ERROR("JPEG FREE: freeing unrecorded malloc block; (# malloc-ed) %d (total size) %d (max size) %d", 
                jpeg_memory_logs.numInfoBlocks, jpeg_memory_logs.totalSize, jpeg_memory_logs.maxSize);
    }
  }
}

#endif /* JPEG_MALLOC_LOGGING */



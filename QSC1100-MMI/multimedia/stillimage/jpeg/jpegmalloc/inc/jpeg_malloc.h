#ifndef JPEG_MALLOC_H
#define JPEG_MALLOC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
               D E C O D E R   H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains the definitions necessary to interface to the
  JPEG malloc logger. Modules using the logger should add an identification
  to the jpeg_file_id_type enum in this file and define jpeg_file_id in the 
  .c file. For example the following lines show up in exif.c:
  
  #include "jpeg_malloc.h"
  #define jpeg_file_id EXIF

  Replace all malloc and free into JPEG_MALLOC and JPEG_FREE respectively.

  Define JPEG_MALLOC_LOGGING to enable this feature.

Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegmalloc/main/latest/inc/jpeg_malloc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/06   vma     Initial version

===========================================================================*/
#include "comdef.h"             /* customer specific definitions           */
#include "malloc_mgr.h"

#define JPEG_MALLOC(size) MM_MALLOC(size, JPEG_GROUP, __FILE__, __LINE__)
#define JPEG_FREE(ptr) MM_FREE(ptr, __FILE__, __LINE__)

#ifdef JPEG_MALLOC_LOGGING
#define JPEG_MEMORY_LOG_COUNT      50

/* 
   Each file in the jpeg module will have on top a #define of jpeg_file_id
   to the following enum values correspondingly to reflect the location of
   the malloc/free in the file
*/
typedef enum
{
  MALLOC_EXIF,
  MALLOC_JPEG,
  MALLOC_JPEGD,
  MALLOC_JPEGE,
  MALLOC_LPM
} jpeg_file_id_type;

typedef struct
{
  /* Size of the malloc in bytes */
  uint32             mallocSizes;
  /* Returned pointer of the malloc */
  void              *mallocPtrs;
  /* Specify in which file the malloc was located */
  jpeg_file_id_type  jpeg_file_id;
  /* The line number of the file jpeg_file_id in which the malloc was called*/
  uint32             mallocline;
  /* The line number of the file jpeg_file_id in which the free was called */
  uint32             freeline;

} jpeg_memory_usage_info_struct;

typedef struct
{
  /* the number of un-freed mallocs recorded so far*/
  uint32                        numInfoBlocks;
  /* Current total of the sizes of all malloc-ed blocks*/
  uint32                        totalSize;
  /* The high water mark of total size */
  uint32                        maxSize;

  jpeg_memory_usage_info_struct usageInfo[JPEG_MEMORY_LOG_COUNT];

} memory_usage_logs;

/*===========================================================================

FUNCTION      JPEG_MALLOC & JPEG_FREE

DESCRIPTION
              When used in debug mode, do malloc and free and keep track
              of memory allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void *jpeg_malloc (uint32 size, jpeg_file_id_type jpeg_file_id, uint32 linenumber);
void jpeg_free(void *ptr, uint32 linenumber);

#endif /* JPEG_MALLOC_LOGGING */

#endif /* JPEG_MALLOC_H */


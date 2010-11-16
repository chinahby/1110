/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     V I D E O   B U F F E R S

GENERAL DESCRIPTION
  This module contains video buffers software.  This is a frame buffer management
  system that allows allocation/deallocation of frame buffers and buffering of
  pending video frames for post processing.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  VideoBuffersAllocate
    This function initializes the appropriate number of buffers to a buffer pool for 
    future allocation.
  VideoBuffersDeallocate
    This function returns the memory consumed in the VideoBuffersAllocate routine to
    the heap.
  VideoBuffersGetNextAvailableBuffer
    This function allocates a buffer from the Video Buffers Pool
  VideoBufferReturnBufferToPool
    This function deallocates a buffer, returning it to the Video Buffers Pool
  VideoBuffersEnqueueUse
    This function tracks the given YCbCr Address pair as being currently held for
    post-processing.
  VideoBuffersDequeueUse
    This function takes the YCbCr address that was first in and releases it from tracking,
    thus indicating the post-processing of this frame is complete.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/video_buffers.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/06   Nag     wrapper added for malloc, free and realloc routines
03/29/06   jdas    Removed lint error
09/09/05   dw      Added VideoBuffersReturnEncodeBufferCount
                   Modified VideoBuffersAllocate to enable buffer count
02/22/05   wyh     Added VideoBuffersIsBufferValid()
01/28/05   drl     Fix memory leak.
12/10/04   drl     Added Error Asserts for error conditions.
12/09/04   drl     Added VideoBuffersReset.
11/29/04   drl     Added VideoBufferPeakUse.
10/21/04   drl     Creation.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "video_buffers.h"
#include "assert.h"
#include "qcamraw.h"
#include "camcorder_util.h"
#include <stdlib.h>


#ifdef FEATURE_CAMCORDER_MALLOC

static camcorderutil_malloc_context_type *vbuf_malloc_context= NULL;
  #define camcorder_malloc(x)\
    camcorderutil_malloc(vbuf_malloc_context, x, \
                         __func__,__LINE__)
  #define camcorder_free(x) { static msg_const_type m; \
                        camcorderutil_free(vbuf_malloc_context,\
                                          __LINE__,__func__,&m, x); \
                      }
#else
  #define camcorder_malloc(x)    malloc(x)
  #define camcorder_free(x)      free(x)
#endif


/* <EJECT> */
/*===========================================================================

                        PUBLIC FUNCTIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION VideoBuffersAllocate

DESCRIPTION
  This procedure pre-allocates all buffers that will be subsequently be managed
  by VideoBuffersGetNextAvailableBuffer routine.

DEPENDENCIES
  None

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  Large quantities of memory are allocated from the system heap.

===========================================================================*/
VideoBuffersStatus VideoBuffersAllocate (VideoBufferHandle *handle, int listSize, 
                                         int encodeListSize, int bufferSize)
{
  int i,j;
  unsigned int list_size = (unsigned int)listSize;
#ifdef FEATURE_CAMCORDER_MALLOC  
  static msg_const_type m;
#endif

  /* Expect a non-null handle */
  if (handle == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }
#ifdef FEATURE_CAMCORDER_MALLOC

vbuf_malloc_context = camcorderutil_malloc_init(__MODULE__,__LINE__,&m);

#endif
  handle->pool.bufferList = (byte **) camcorder_malloc (sizeof (byte *) * list_size);

  /* Check allocation of buffer list. */
  if (handle->pool.bufferList == NULL)
  {
    QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");
    return VIDEO_BUFFERS_FAILURE;
  }

  handle->pool.in_use     = (boolean *) camcorder_malloc (sizeof (boolean) * list_size);

  if (handle->pool.in_use == NULL)
  {
    camcorder_free (handle->pool.bufferList);
#ifdef FEATURE_CAMCORDER_MALLOC
     camcorderutil_malloc_terminate(vbuf_malloc_context,
                                         __LINE__,__func__,&m);
    vbuf_malloc_context = NULL;

#endif
	QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");


    return VIDEO_BUFFERS_FAILURE;
  }

  handle->pool.freePool = (VideoBuffersYCbCrAddressPairsType *) camcorder_malloc (sizeof (VideoBuffersYCbCrAddressPairsType) * list_size);

  if (handle->pool.freePool == NULL)
  {
    camcorder_free (handle->pool.bufferList);
    camcorder_free (handle->pool.in_use);
#ifdef FEATURE_CAMCORDER_MALLOC
     camcorderutil_malloc_terminate(vbuf_malloc_context,
                                         __LINE__,__func__,&m);
    vbuf_malloc_context = NULL;

#endif
	QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");


    return VIDEO_BUFFERS_FAILURE;
  }

  handle->inUse.list = (VideoBuffersYCbCrAddressPairsType *) 
                          camcorder_malloc (sizeof (VideoBuffersYCbCrAddressPairsType) * list_size);

  if (handle->inUse.list == NULL)
  {
    camcorder_free (handle->pool.bufferList);
    camcorder_free (handle->pool.in_use);
    camcorder_free (handle->pool.freePool);
#ifdef FEATURE_CAMCORDER_MALLOC
     camcorderutil_malloc_terminate(vbuf_malloc_context,
                                         __LINE__,__func__,&m);
    vbuf_malloc_context = NULL;

#endif
	QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");


    return VIDEO_BUFFERS_FAILURE;
  }

  for (i = 0; i < listSize; i++)
  {
    handle->pool.bufferList [i] = (byte *) camcorder_malloc (sizeof (byte) * (unsigned int)bufferSize);
    handle->pool.in_use [i]     = FALSE;

    /* Check allocation of buffer */
    if (handle->pool.bufferList [i] == NULL)
    {
      for (j = 0; j < i; j++)
      {
        camcorder_free (handle->pool.bufferList [j]);
      }
      camcorder_free (handle->pool.bufferList);
      camcorder_free (handle->inUse.list);
      camcorder_free (handle->pool.in_use);
      camcorder_free (handle->pool.freePool);
      handle->pool.bufferList = NULL;
      handle->pool.in_use     = NULL;
      handle->pool.freePool   = NULL;
#ifdef FEATURE_CAMCORDER_MALLOC
     camcorderutil_malloc_terminate(vbuf_malloc_context,
                                         __LINE__,__func__,&m);
    vbuf_malloc_context = NULL;

#endif
	  QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");


      return VIDEO_BUFFERS_FAILURE;

    }
  }

  handle->listSize       = listSize;
  handle->encodeListSize = encodeListSize;

  handle->inUse.head     = 0;
  handle->inUse.tail     = 0;
  handle->inUse.empty    = TRUE;

  return VIDEO_BUFFERS_SUCCESS;
  
}

/*===========================================================================

FUNCTION VideoBuffersIsBufferValid 

DESCRIPTION
  This procedure checks if a buffer is in the list of allocated buffers
  for a videoBufferHandle structure.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None

===========================================================================*/
VideoBuffersStatus VideoBuffersIsBufferValid(
  const VideoBufferHandle *handle, const byte *buffer
  )
{
  int i;
  for(i=0; i<handle->listSize; i++)
  {
    if ( handle->pool.bufferList[i] == buffer )
      return VIDEO_BUFFERS_SUCCESS;
  }
  QTV_MSG(QTVDIAG_DEBUG,"YY Said : VIDEO_BUFFERS_FAILURE");
  return VIDEO_BUFFERS_FAILURE;
}

/*===========================================================================

FUNCTION VideoBuffersReset 

DESCRIPTION
  This procedure resets the buffer pool and in-use queue.  This is used in 
  an error situation, where VideoBuffersAllocate has been called already
  and the buffering mechanism needs to be reset without deallocating and
  then reallocating buffers.  Assumes that VideBuffersAllcoate has been
  called successfully.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None

===========================================================================*/
VideoBuffersStatus VideoBuffersReset (VideoBufferHandle *handle)
{
  int i;

  if (handle->pool.in_use == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  for (i = 0; i < handle->listSize; i++)
  {
    handle->pool.in_use [i] = FALSE;
  }

  handle->inUse.head  = 0;
  handle->inUse.tail  = 0;
  handle->inUse.empty = TRUE;

  return VIDEO_BUFFERS_SUCCESS;
}


/*===========================================================================

FUNCTION VideoBuffersDeallocate

DESCRIPTION
  This procedure returns the pre-allocated buffers to the system heap.
  Assumes that the handle was previously used in a call to VideoBuffersAllocate 
  with a successful return value.  If the handle has not been used in a successful
  VideoBuffersAllocate call, then this routine will return VIDEO_BUFFERS_FAILURE.

DEPENDENCIES
  None

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  Large quantities of memory are returned to the system heap.

===========================================================================*/
VideoBuffersStatus VideoBuffersDeallocate (VideoBufferHandle *handle)
{
  int i;
#ifdef FEATURE_CAMCORDER_MALLOC  
  static msg_const_type m;
#endif
  /* Expect a non-null handle */
  if (handle == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->pool.bufferList == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  for (i = 0; i < handle->listSize; i++)
  {
    camcorder_free (handle->pool.bufferList [i]);
  }

  camcorder_free (handle->pool.bufferList);
  camcorder_free (handle->pool.freePool);
  camcorder_free (handle->inUse.list);
  camcorder_free (handle->pool.in_use);
  handle->pool.bufferList = NULL;
  handle->pool.freePool   = NULL;
  handle->pool.in_use     = NULL;
#ifdef FEATURE_CAMCORDER_MALLOC
     camcorderutil_malloc_terminate(vbuf_malloc_context,
                                         __LINE__,__func__,&m);
    vbuf_malloc_context = NULL;

#endif


  return VIDEO_BUFFERS_SUCCESS;

}

/*===========================================================================

FUNCTION VideoBuffersGetNextAvailableBuffer

DESCRIPTION
  This procedure allocates a buffer that was pre-allocated in the VideoBuffersAllocate
  procedure.  If there are no more buffers available, or VideoBuffersAllocate was
  not called prior to this call with an indication of SUCCESS, then this procedure
  will return a value of NULL.

DEPENDENCIES
  Requires VideoBuffersAllocate to be called prior.

RETURN VALUE
  byte * - pointer to buffer.

SIDE EFFECTS
   None

NOTE
   For output1 processing can be called from ISR.

===========================================================================*/
VideoBuffersYCbCrAddressPairsType *VideoBuffersGetNextAvailableBuffer (const VideoBufferHandle *handle)
{
  int buffer_index;

  /* Expect a non-null handle */
  if (handle == NULL)
  {
    ASSERT (0);
    return NULL;
  }

  /* Expect VideoBuffersAllocate to be called before this routine */
  if (handle->pool.freePool == NULL)
  {
    ASSERT (0);
    return NULL;
  }

  if (handle->pool.in_use == NULL)
  {
    ASSERT (0);
    return NULL;
  }

  for (buffer_index = 0; buffer_index < handle->listSize; buffer_index++)
  {
    if (!handle->pool.in_use [buffer_index])
    {
      handle->pool.in_use [buffer_index] = TRUE;
      return &handle->pool.freePool [buffer_index];
    }
  }

  ASSERT (0);
  return NULL;
}

/*===========================================================================

FUNCTION VideoBufferReturnBufferToPool

DESCRIPTION
  This procedure returns a buffer to the free pool. Assumes that VideoBuffersAllocate
  was called prior to this call with an indication of SUCCESS, otherwise this procedure
  will return a FAILURE indication.

DEPENDENCIES
  Requires VideoBuffersAllocate to be called prior.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
   None

===========================================================================*/
VideoBuffersStatus VideoBufferReturnBufferToPool (const VideoBufferHandle *handle, 
                                                  VideoBuffersYCbCrAddressPairsType buffer)
{
  int buffer_index;

  /* Expect a non-null handle */
  if (handle == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  /* Expect VideoBuffersAllocate to be called before this routine */
  if (handle->pool.freePool == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  /* Expect VideoBuffersAllocate to be called before this routine */
  if (handle->pool.in_use == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  for (buffer_index = 0; buffer_index < handle->listSize; buffer_index++)
  {
    if (handle->pool.freePool [buffer_index].Y_Address == buffer.Y_Address)
    {
      handle->pool.in_use [buffer_index] = FALSE;
      return VIDEO_BUFFERS_SUCCESS;
    }
  }

  return VIDEO_BUFFERS_FAILURE;

}

/*===========================================================================

FUNCTION VideoBuffersEnqueueUse

DESCRIPTION
  This procedure enqueues YCbCr Address pairs into a queue of frame buffers.
  Assumes that VideoBufferAllocate was called prior with a success indication.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
   None.

===========================================================================*/
VideoBuffersStatus VideoBufferEnqueueUse (VideoBufferHandle                *handle, 
                                          VideoBuffersYCbCrAddressPairsType addresses)
{
  if (handle == NULL) 
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.list == NULL)
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  if ((handle->inUse.empty == FALSE) && (handle->inUse.head == handle->inUse.tail))
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  handle->inUse.list [handle->inUse.tail] = addresses;
  handle->inUse.tail  = (handle->inUse.tail + 1) % handle->listSize;
  handle->inUse.empty = FALSE;

  return VIDEO_BUFFERS_SUCCESS;
}

/*===========================================================================

FUNCTION VideoBufferUseQueueSize

DESCRIPTION
   Provide the current number of entries in the use queue.

DEPENDENCIES
  None.

RETURN VALUE
  int - Size of use queue.

SIDE EFFECTS
   None.

===========================================================================*/
int VideoBufferUseQueueSize (const VideoBufferHandle *handle)
{
  if (handle == NULL) return 0;

  if (handle->inUse.empty) return 0;

  if (handle->inUse.head >= handle->inUse.tail)
  {
    return ((handle->listSize - handle->inUse.head) + handle->inUse.tail);
  }
  else
  {
    return (handle->inUse.tail - handle->inUse.head);
  }
}


/*===========================================================================

FUNCTION VideoBuffersDequeueUse

DESCRIPTION
  This procedure dequeues the YCbCr Address pair that was previously enequeued
  using the VideoBuffersEnqueueUse procedure. Assumes that the handle was previously 
  used in a call to VideoBuffersAllocate with a successful return value.  If the handle 
  has not been used in a successful VideoBuffersAllocate call, then this routine will 
  return VIDEO_BUFFERS_FAILURE.  Also assumes that the queue is not empty, in which case
  VIDEO_BUFFERS_FAILURE can also be returned.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None.

===========================================================================*/
VideoBuffersStatus VideoBufferDequeueUse (VideoBufferHandle *handle,
                                          VideoBuffersYCbCrAddressPairsType *addresses)
{
  if (handle == NULL) 
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.list == NULL)
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.empty == TRUE)
  {
    ASSERT (0);
    return VIDEO_BUFFERS_FAILURE;
  }

  *addresses = handle->inUse.list [handle->inUse.head];

  handle->inUse.head = (handle->inUse.head + 1) % handle->listSize;
  if (handle->inUse.head == handle->inUse.tail)
  {
    handle->inUse.empty = TRUE;
  }

  return VIDEO_BUFFERS_SUCCESS;

}

/*===========================================================================

FUNCTION VideoBuffersPeakUse

DESCRIPTION
  This procedure returns the YCbCr Address pair that was previously enequeued
  using the VideoBuffersEnqueueUse procedure without removing it from the queue. 
  Assumes that the handle was previously used in a call to VideoBuffersAllocate 
  with a successful return value.  If the handle has not been used in a 
  successful VideoBuffersAllocate call, then this routine will 
  return VIDEO_BUFFERS_FAILURE.  Also assumes that the queue is not empty, in 
  which case VIDEO_BUFFERS_FAILURE can also be returned.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None.

===========================================================================*/
VideoBuffersStatus VideoBufferPeakUse (const VideoBufferHandle *handle,
                                       VideoBuffersYCbCrAddressPairsType *addresses)
{
  if (handle == NULL) 
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.list == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.empty == TRUE)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  *addresses = handle->inUse.list [handle->inUse.head];

  return VIDEO_BUFFERS_SUCCESS;

}

/* <EJECT> */
/*===========================================================================

FUNCTION VideoBuffersReturnEncodeBufferCount

DESCRIPTION
  This procedure returns the no. of buffers that are used solely by for 
  encoding. Assumes that the handle was previously used in a call to 
  VideoBuffersAllocate with a successful return value.  If the handle has 
  not been used in a successful VideoBuffersAllocate call, then this routine 
  will return VIDEO_BUFFERS_FAILURE. Buffers in queue can be empty.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None.

===========================================================================*/
VideoBuffersStatus VideoBuffersReturnEncodeBufferCount (const VideoBufferHandle *handle, 
                                                        int               *bufferCount)
{
  if (handle == NULL) 
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  if (handle->inUse.list == NULL)
  {
    return VIDEO_BUFFERS_FAILURE;
  }

  *bufferCount = handle->encodeListSize;

  return VIDEO_BUFFERS_SUCCESS;
}


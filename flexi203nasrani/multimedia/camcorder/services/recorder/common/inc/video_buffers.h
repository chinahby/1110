#ifndef VIDEO_BUFFERS
#define VIDEO_BUFFERS
/*===========================================================================

                       V I D E O   B U F F E R S
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  software modules to interface with the video buffers module.  The video
  buffers modules manages video buffers allocation/deallocation as well
  as the set of frame buffers that are currently "in use" (buffered) for
  ARM level processing.

Copyright(c) 2004 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/video_buffers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/06   jdas    Modified few functions to remove lint error
09/09/05   dw      Added VideoBuffersReturnEncodeBufferCount
                   Modified VideoBuffersAllocate to enable buffer count
02/22/05   wyh     Added VideoBuffersIsBufferValid()
 1/19/05   drl     Changed time value in VideoBuffersYCbCrAddressPairsType
                   to a single elelment rather than an array.
12/09/04   drl     Added VideoBufferReset.
11/29/04   drl     Added timestamp support.
10/28/04   drl     Added VideoBufferPeakUse.
10/19/04   drl     Creation.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ipl.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum VideoBuffersStatusEnum
{
  VIDEO_BUFFERS_SUCCESS,
  VIDEO_BUFFERS_FAILURE
} VideoBuffersStatus;

/* Address Pairs used to define a YCbCr Frame.  These addresses are nested inside */
/* one buffer.                                                                    */
typedef struct VideoBuffersYCbCrAddressPairsStruct
{
  uint32 *Y_Address;      /* Luma Address   */
  uint32 *CbCr_Address;   /* Chroma Address */
  uint32  time;           /* Absolute time that the frame arrived */
  ipl_col_for_type 
          image_format;   /* image format : line-packed of mb-packed */
} VideoBuffersYCbCrAddressPairsType;

/* Pool of allocatable buffers */
typedef struct VideoBuffersPoolQueueStruct
{
  byte                              **bufferList;   /* Pointer to list of available buffers  */
  VideoBuffersYCbCrAddressPairsType  *freePool;
  boolean                            *in_use;       /* array of in-use flags for each buffer */
} VideoBuffersPoolQueueType;

/* Queue of YCbCr frames that are currently buffered together. */
typedef struct VideoBuffersInUseQueueStruct
{
  VideoBuffersYCbCrAddressPairsType *list;  /* Array of YCbCr address pairs            */
  int                                head;  /* Pointer to the front of the queue       */
  int                                tail;  /* Pointer to the tail of the queue        */
  boolean                            empty; /* Flag indicating that the queue is empty.*/
} VideoBuffersInUseQueueType;

/* Overall handle which holds all VideoBuffer info */
typedef struct VideoBufferHandleStruct
{
  int                        listSize;       /* number of buffers                */
  int                        encodeListSize; /* no. of buffers used for encoding */
  VideoBuffersPoolQueueType  pool;           /* Allocation Pool                  */
  VideoBuffersInUseQueueType inUse;          /* FIFO list of in use Buffers      */
} VideoBufferHandle;


/* <EJECT> */
/*===========================================================================

FUNCTION VideoBuffersAllocate 

DESCRIPTION
  This procedure pre-allocates the specified number of buffers each of the 
  specified size from the heap.  These buffers are then managed using the
  VideoBuffersGetNextAvailableBuffer and VideoBufferReturnBufferToPool procedures.

  If all buffers cannot be allocated, then all successful allocations prior to the
  failure are returned to the heap and VIDEO_BUFFERS_FAILURE is returned.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  Large quantities of memory are allocated from the system heap.

===========================================================================*/
VideoBuffersStatus VideoBuffersAllocate (VideoBufferHandle *handle, 
                                         int                listSize, 
                                         int                encodeListSize,
                                         int                bufferSize);

/* <EJECT> */
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
  );

/* <EJECT> */
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
VideoBuffersStatus VideoBuffersReset (VideoBufferHandle *handle);


/* <EJECT> */
/*===========================================================================

FUNCTION VideoBuffersDeallocate

DESCRIPTION
  This procedure returns the pre-allocated buffers to the system heap.
  Assumes that the handle was previously used in a call to VideoBuffersAllocate 
  with a successful return value.  If the handle has not been used in a successful
  VideoBuffersAllocate call, then this routine will return VIDEO_BUFFERS_FAILURE.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  Large quantities of memory are returned to the system heap.

===========================================================================*/
VideoBuffersStatus VideoBuffersDeallocate (VideoBufferHandle *handle);

/* <EJECT> */
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

===========================================================================*/
VideoBuffersYCbCrAddressPairsType *VideoBuffersGetNextAvailableBuffer (const VideoBufferHandle *handle);

/* <EJECT> */
/*===========================================================================

FUNCTION VideoBufferReturnBufferToPool

DESCRIPTION
  This procedure returns a buffer to the free pool. Assumes that VideoBuffersAllocate
  was called prior to this call with an indication of SUCCESS, otherwise this procedure
  will return a FAILURE indication.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_BUFFERS_SUCCESS if success,
  VIDEO_BUFFERS_FAILURE if failed.

SIDE EFFECTS
  None.

===========================================================================*/
VideoBuffersStatus VideoBufferReturnBufferToPool (const VideoBufferHandle *handle, 
                                                  VideoBuffersYCbCrAddressPairsType buffer);

/* <EJECT> */
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
                                          VideoBuffersYCbCrAddressPairsType addresses);

/* <EJECT> */
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
int VideoBufferUseQueueSize (const VideoBufferHandle *handle);

/* <EJECT> */
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
VideoBuffersStatus VideoBufferDequeueUse (VideoBufferHandle                 *handle, 
                                          VideoBuffersYCbCrAddressPairsType *addresses);

/* <EJECT> */
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
VideoBuffersStatus VideoBufferPeakUse (const VideoBufferHandle                 *handle, 
                                       VideoBuffersYCbCrAddressPairsType *addresses);


/* <EJECT> */
/*===========================================================================

FUNCTION VideoBuffersReturnEncodeBufferCount

DESCRIPTION
  This procedure returns the no. of buffers that are used solely by backend
  for encoding. Assumes that the handle was previously used in a call to 
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
                                                        int               *bufferCount);

#endif /* VIDEO_BUFFERS */

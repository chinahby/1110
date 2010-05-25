/* =======================================================================
                              QtvMediaSliceBuffer.cpp
DESCRIPTION
  This module contains the implementation for the Qtv Media Track List 
  class. 

EXTERNALIZED FUNCTIONS
  1. Constructor & Destructor
  2. Insert
  3. CopyOrDropTillOffset
  4. Delete
  5. Peek
  6. operator[]
  7. FindADTSSyncMarker
  9. GetADTSSamplingFrequencyFromIndex


INITIALIZATION AND SEQUENCING REQUIREMENTS
  -NA-
  
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/qtvmediaslicebuffer.cpp#7 $
$DateTime: 2008/05/08 08:09:22 $
$Change: 656074 $

========================================================================== */

/*===========================================================================
  Includes
===========================================================================*/
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"
#include "qcplayer.h"
#include "QtvMediaSliceBuffer.h"
#include "QCUtils.h"



/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::QtvMediaSliceBuffer (Constructor)

DESCRIPTION:
   Slice Buffer is made up of audio samples
   Assumption is that this slice buffer contains the audio samples sequentially
   Initializes all the member variables. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
QtvMediaSliceBuffer::QtvMediaSliceBuffer()
{
  QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "Executing the Media Slice Buffer constructor ");
  m_nMax        = 0;
  for(int i=0;i<QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS;i++)
  {
    InitSliceBufferEntry(i);
  }
}
/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::~QtvMediaSliceBuffer 

DESCRIPTION:
   Destructor - Frees up the audio PES sample copies available.

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
QtvMediaSliceBuffer::~QtvMediaSliceBuffer(void)
{
  // delete all the active entries in the array
  QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "Executing the Media Slice Buffer destructor (%d buffers present) ",
               m_nMax);
  while(m_nMax)
  {
    DeleteSlice();
  }
}
/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::CanInsert 

DESCRIPTION:
  Returns if space available inside the slice buffer. 

RETURN VALUE:
  true/false 
          
SIDE EFFECTS:
  None.
===========================================================================*/  
bool QtvMediaSliceBuffer::CanInsert()
{
  return (m_nMax < QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS); 
}

/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::Insert 

DESCRIPTION:
   Inserts the audio PES sample passed into the slice buffer along with the timestamp value. 

RETURN VALUE:
  true/false depending on whether the insert was successful. Max size of an ADTS frame can 
  not exceed 772. Hence there is no way we will queue up more than 30 buffers at a time. 
          
SIDE EFFECTS:
  None.
===========================================================================*/  
bool QtvMediaSliceBuffer::Insert(unsigned char* pSlice, unsigned int nTimestamp, unsigned int nSize)
{
  // Assumption is that this is a sequential array , Hence insert boils down to very simple 
  if(m_nMax >= QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_FATAL,
                 "QtvMediaSliceBuffer::Insert sample overflow error !!!!");
    QTV_ASSERT(0);
    // This may not happen since we expect coulple of samples 
    // only queued at any point of time. 
    return false;
  }

  unsigned char * ptr =(unsigned char *)QTV_Malloc(nSize);
  if(ptr == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_FATAL,
                 "QtvMediaSliceBuffer::Insert Malloc Failed Dropping data !!!!");
    // This may not happen since we expect malloc's to succeed
    return false;
  }
  memcpy(ptr,pSlice,nSize);
  m_aSlice[m_nMax]     = ptr;
  m_aOrigSlice[m_nMax] = ptr; 
  m_aTimestamp[m_nMax] = nTimestamp;
  // m_aBound[m_nMax] = nSize if m_nMax is zero 
  // Previous size of the array is in m_aBound[m_nMax-1]
  // Last entry contains the size of the entire array
  // Otherwise set it to M_aBound[m_nMax-1] + nSize
  m_aBound[m_nMax]     = nSize;
  if(m_nMax > 0)
  {
    m_aBound[m_nMax] += m_aBound[m_nMax-1];
  }
  // Finally increment the max count field
  m_nMax++;
  QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "QtvMediaSliceBuffer - Inserted %d: %p, %d",
               m_nMax-1,ptr,nSize);
  return true; 
}

            
/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::CopyOrDropTillOffset 

DESCRIPTION:
   Copies or Drops till the specified offset from the very beginning of the slice buffer.  User of this routine
   has to make sure that sufficient space available in the destination pointer when copy flag is set to 
   true. 

RETURN VALUE:
  true/false depending of the validity of the specified offset.
          
SIDE EFFECTS:
  None.
===========================================================================*/        
bool QtvMediaSliceBuffer::CopyOrDropTillOffset(unsigned int nOffset, 
                                                unsigned char *pDst, 
                                                bool bCopy)
{

  // Assumption here is that size check for the destination buffer is made 
  // before calling this function. 
  // Also every copy/drop is assumed to be aligned with the buffer start 
  unsigned int nFromSlice=0,nToSlice=0,nToIndex=0,i=0;
  // You are supposed to delete offset +1 bytes or copy into the destination buffer
  unsigned int nSizeDeleted = nOffset+1; 
  bool bRet=false; 

  bRet = Peek(nOffset , nToIndex, nToSlice); 
  if(!bRet)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                 "QtvMediaSliceBuffer::Copy failed due to invalid offset(%d)",
                 nOffset);
    return false; 
  }

  // Go through the for loop and take care of the complete slices
  for(i=nFromSlice; i < nToSlice; i++)
  {
    // need to copy the entire slices
    if(bCopy)
    {
      QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
        "CopyOrDrop: Copied slie %d:(%p,%d)",
        i,m_aSlice[0],m_aBound[0]);
      memcpy(pDst,m_aSlice[0],m_aBound[0]);
      pDst += m_aBound[0];
    }
    DeleteSlice();
  }

  // Even if we started with the complex case we have now turned the problem
  // simple
  nSizeDeleted = nToIndex + 1;
  if(bCopy)
  {
    QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
        "CopyOrDrop: Copied slie %d:(%p,%d)",
        nToSlice,m_aSlice[0],m_aBound[0]);
    // Inclusive copy 
    memcpy(pDst,m_aSlice[0],nSizeDeleted);
  }
  if(nSizeDeleted == m_aBound[0])
  {
    // entire slice has been consumed. hence issue delete. 
    DeleteSlice();
  }
  else 
  {
    m_aSlice[0] += nSizeDeleted; 
    // Go through and update the bounds for the entire array
    for(i=0;i<m_nMax;i++)
    {
      m_aBound[i] -= nSizeDeleted;
    }
    QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                  "CopyOrDrop: First slice adjusted to %d from %d",
                  m_aBound[0],m_aBound[0]+nSizeDeleted);
  }
  return true; 
}
/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::Delete 

DESCRIPTION:
   Delete the first slice within the buffer.  Slice buffer is a kind of queue. deletion will always 
   performed at the first slice. At times more than one slices could be deleted. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
unsigned int QtvMediaSliceBuffer::DeleteSlice()
{

  if(m_nMax == 0)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
    "QtvMediaSliceBuffer::DeleteSlice Ignored due to empty slice buffer");
    return 0;
  }
  
  unsigned int nSizeDeleted = m_aBound[0]; // Size getting deleted
  unsigned char *ptr        = m_aOrigSlice[0];     
  unsigned int i;


  // Print the address of buffer freed. This can be used to 
  // verify that allocated buffers are freed exactly. 
  QTV_MSG_PRIO2(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_MED,
               "QtvMediaSliceBuffer::DeleteSlice Slice Buffer %p,size %d",
               ptr,nSizeDeleted);
  QTV_Free(ptr);
 
  for(i=0;i<(m_nMax-1);i++)
  {
    m_aSlice[i]        = m_aSlice[i+1];
    m_aOrigSlice[i]    = m_aOrigSlice[i+1];
    m_aTimestamp[i]    = m_aTimestamp[i+1];
    m_aBound[i]        = m_aBound[i+1] - nSizeDeleted;
  }
  
  // Re-Initialize the deleted entry in the array. 
  InitSliceBufferEntry(i);

  // Finally update the  number slices present
  m_nMax--;

  return nSizeDeleted;
}

/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::Peek

DESCRIPTION:
   Peeks at any valid offset and returns the index and slice values. 

RETURN VALUE:
  true/false depending on the validity of the offset passed.
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaSliceBuffer::Peek(unsigned int offset,
                                unsigned int &nIndex, 
                                unsigned int &nSlice)
{
  unsigned int i;

  /* Take care of the Buffer empty case */
  if(m_nMax == 0)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST, QTVDIAG_PRIO_LOW,
                 "Peek: Slice Buffer empty ");
    return false; 
  }

  /* You can return the negative result quickly */
  if(offset >= m_aBound[m_nMax-1])
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "Peek: Invalid offset passed (%d); returning failure ",offset);
    return false; 
  }
  // Since we have eliminated the negative case, search should be successful. 
  for(i=0;i<m_nMax;i++)
  {
    if(offset < m_aBound[i])
    {
      // We have found the offset 
      // index is same as offset when the element peeked present in the first slice. 
      // Otherwise remove it from the previous cumulative element count
      nIndex = offset; 
      if(i>0)
      {
        nIndex -= m_aBound[i-1]; 
      }
      nSlice = i; 
      break;
    }
  }
  QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "Peek sliceBuf[%d,%d] =  %d",
               nSlice,nIndex,m_aSlice[nSlice][nIndex]);
  return true;
}

/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::Operator[]

DESCRIPTION:
   Peeks at any valid offset and returns the value within it. 

RETURN VALUE:
  Positive/Negaitve value based on the validity of the offset. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
sint31 QtvMediaSliceBuffer::operator[](unsigned int offset)
{
  sint31 nVal=-1; // Negative value to indicate the failure
  unsigned int i=0;
  unsigned int nSlice=0; 

  if(Peek(offset,i,nSlice))
  {
    nVal = m_aSlice[nSlice][i];
  }
  
  return nVal; 
}

/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::DropCurrentSlice

DESCRIPTION:
   Drops the currently working slice. This is invoked to flush the discontinuity. 

RETURN VALUE:
  Returns the number of bytes dropped. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
int QtvMediaSliceBuffer::DropCurrentSlice(unsigned int offset)
{
  int nVal=-1; // Negative value to indicate the failure
  unsigned int i=0;
  unsigned int nSlice=0; 

  if(Peek(offset,i,nSlice))
  {
    nVal = m_aBound[nSlice];
    QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
        "Dropped %d bytes due to observed discontinuity",
        nVal);
    (void)CopyOrDropTillOffset(nVal-1,NULL,false);
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
        "DropCurrentSlice called with Invalid Offset; Nothing dropped");
    nVal=0;
  }
  
  return nVal; 
}

// Helper functions to aid in PES audio sample parsing 
// Find Sync Marker 
/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::FindADTSSyncMarker

DESCRIPTION:
   Locks on the syncword within the slice buffer. 

RETURN VALUE:
  true/false depending on whether sync is found within the buffer.
  bPacketPending flag is set to true when the sync work split across the PES
  slice boundary. bDiscontinuity output flag is set to true when any of the data
  is dropped. If the sync is found, first two bytes of the buffer alighned to the 
  sync word. Otherwise all the data dropped and discontinuity flag raised. 
          
SIDE EFFECTS:
  None.
===========================================================================*/
bool QtvMediaSliceBuffer::FindADTSSyncMarker(bool &bPacketPending,
                                         bool &bDiscontinuity)
{
  bool bNoMatch = true;  /* Initial state */
  unsigned char *ptr;

  bPacketPending = false; 
  bDiscontinuity = false; 
  /* Take care of the Buffer empty case */
  if(m_nMax == 0) 
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST, QTVDIAG_PRIO_LOW,
                 "Peek: Slice Buffer empty ; Underrun Returned");
    return false; 
  }

  // m_aBound[m_nMax-1] points to the size of the array
  // If there are less than 2 bytes there is not point in attempting to 
  // find itself. Return failure immediately. 
  if(m_aBound[m_nMax-1] < 2)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERIC_BCAST, QTVDIAG_PRIO_LOW,
                 "Peek: Slice Buffer has one character; Underrun again; ");
    return false; 
  }

  // offset runs from 0..m_aBound[m_nMax-1]
  unsigned int i=0,j=0,k=0,offset=0,size=0;
  for(i=0;i<m_nMax;i++)
  {

    // Size is m_aBound[i] when i=0
    // Size is m_aBound[i] - m_aBound[i-1] when i>0
    size = m_aBound[i];
    if(i>0)
    {
      size -= m_aBound[i-1];
    }
    ptr = m_aSlice[i];
    // Pointer value can never be zero 
    if(!ptr)
    {
      QTV_ASSERT(0);
      return false;
    }
    QTV_MSG_PRIO3(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
                 "FindADTSSyncMarker: slice = %d, ptr = %x , size = %d",
                 i,ptr,size);

    // J runs from 0..(size-1)
    j=0;
    for(;j<size;j++,offset++)
    {
      /* Execute the state machine */
      if(bNoMatch)
      { 
        /* First state transitions */
        if(ptr[j] == QTV_MEDIA_ADTS_HIGH_SYNC_MARKER_VALUE)
        {
          /* Match found ; Move to second state */
          bNoMatch     = false; 
          /* Store the slice value in k */
          k = i; 
        }
      }
      else
      {
        /* Second State Transition */
        if((ptr[j] & QTV_MEDIA_ADTS_LOW_SYNC_MARKER_MASK) == QTV_MEDIA_ADTS_LOW_SYNC_MARKER_VALUE)
        {
          /* Sync Found */
          //offset point to the place where the low sync marker found
          //offset-1 definitely exists and points to the high sync marker 
          //In most cases offset will be equal to one ; In this case there is 
          //no drop. 
          if(offset >= 2)
          {
            CopyOrDropTillOffset(offset-2,NULL,false);
            QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_HIGH,
                         "FindADTSSyncMarker: Detected Discontinuity and dropping %d bytes",
                         offset-1);
            bDiscontinuity = true; 
          }

          // k has the slice where the high sync marker match found. 
          if(i>k)
          {
            //Sync Marker crosses the PES Sample boundary
            bPacketPending=true;
            QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_ERROR,
                         "FindADTSSyncMarker: Detected split in the sync marker",
                         offset-1);
          }
          return true;
        }
        /* If the higher nibble doesnt match then jump to first state 
           anyway since you can not have match for the high sync marker
           value anyway */
        bNoMatch = true; 
      }
    }
  }
  /* Sync Marker not found , Ignore all the bytes*/
  CopyOrDropTillOffset(m_aBound[m_nMax-1]-1,NULL,false);
  /* Set Discontinuity to true since we are dropping bytes here */
  bDiscontinuity = true; 
  return false; 
}



/*===========================================================================
FUNCTION:
  QtvMediaSliceBuffer::InitSliceBufferEntry

DESCRIPTION:
   Initializes the slice buffer entry whose index is passed. 

RETURN VALUE:
  None.
          
SIDE EFFECTS:
  None.
===========================================================================*/
void QtvMediaSliceBuffer::InitSliceBufferEntry(int nIndex)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERIC_BCAST,QTVDIAG_PRIO_LOW,
               "[Re]Initializing Slice Buffer Entry #%d",nIndex);
  /* Below if condition is added to fix klocwork warnings */
  if(nIndex > 0 && nIndex < QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS)
  {
    m_aSlice[nIndex]     = NULL;
    m_aOrigSlice[nIndex] = NULL;
    m_aBound[nIndex]     = 0;
    m_aTimestamp[nIndex] = QTV_MEDIA_INVALID_TIMESTAMP;
  }
  return;
}

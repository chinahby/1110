#ifndef QTV_MEDIA_SLICE_BUFFER_H
#define QTV_MEDIA_SLICE_BUFFER_H
/* =======================================================================

                          QtvMediaSliceBuffer.h

DESCRIPTION

  Definition of the QTV Media slice buffer  class.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/qtvmediaslicebuffer.h#7 $ 
$DateTime: 2008/05/08 08:09:22 $
$Change: 656074 $ 

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "QtvMediaAACHeaderDefns.h"


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Impractical to reach this maximum value ; We should bound the frame length somehow*/
#define QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS              30 
#define QTV_MEDIA_INVALID_TIMESTAMP            0xffffffff

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
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */
/* ======================================================================
CLASS: QtvMediaSliceBuffer

DESCRIPTION: Contains the AUDIO PES Sample Buffer. This buffer is used to parse and peel individual ADTS
frames out of the PES samples pulled from the media source. 

PARAMETERS:

RETURN VALUE:

=========================================================================*/
class QtvMediaSliceBuffer
{
  // Contains one slice buffer or audio sample
  // This value could be modified especially when we modify the first slice 
  // by returning the partial or complete ADTS frame. 
  unsigned char *m_aSlice[QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS];

  // Contains the copy of the original pointer 
  // Used to make the free logic simple.
  unsigned char *m_aOrigSlice[QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS];

  // Timestamp associated with each of the audio sample
  unsigned int   m_aTimestamp[QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS];

  // m_aBound holds the cumulative number of elements upto any index 
  // Special case : m_aBound[m_nMax-1] holds the size of the entire slice buffer
  unsigned int   m_aBound[QTV_MEDIA_MAX_NO_OF_SLICE_BUFFERS];

  // Number of slices or PES audio samples in the buffer. MAX any time could be 20. 
  unsigned int   m_nMax; 

public: 
  // Constructor Method
  QtvMediaSliceBuffer(void);

  // Destructor Method 
  ~QtvMediaSliceBuffer(void);

  // Insert new sample into the array 
  bool Insert(unsigned char *pSliceBuf, unsigned int nTimestamp, unsigned int size);

  // Drops or Copies the data upto the specified offset(inclusive) in the slice buffer. 
  bool CopyOrDropTillOffset(unsigned int nTo, unsigned char *pDst, bool bCopy);

  // assumption is that we have to delete the top one only 
  unsigned int DeleteSlice(void);

  // Operator function to accesss two dimension slice buffer as an array of single dimension
  sint31 operator[](unsigned int offset);

  // Drop the current slice due to observed discontinuity
  int DropCurrentSlice(unsigned int offset);
  
  // Return false/true depending on whether the offset is invalid or not 
  // Return the slice and index values associated with the particular offset passed as input 
  bool Peek(unsigned int offset, unsigned int &nSlice,unsigned int &nIndex);

  // Locate the ADTS Sync marker within the bitstream. 
  bool FindADTSSyncMarker(bool &bPacketPending, bool &bDiscontinuity);



  // Check whether the slice buffer has space or not
  bool CanInsert();

  // Initialize or Re-Initialize the slice buffer entry
  void InitSliceBufferEntry(int nIndex);

};
#endif /* QTV_MEDIA_SLICE_BUFFER_H */



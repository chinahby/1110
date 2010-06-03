#ifndef AEEENCODESPEC_H
#define AEEENCODESPEC_H

/*============================================================================
FILE:        AEEEncodeSpec.h

SERVICES:  BREW MultiMedia Utility Services

DESCRIPTION:
   This file defines image encoding spec for IMediaUtil interface. 

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004 - 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/oem/imagepro/rel/3.1.5/latest/inc/AEEEncodeSpec.h#2 $
 $DateTime: 2008/07/02 01:43:45 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/30/06  NC     Updated documentation.
 03/20/06  NC     Fixed a typo.

===============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
#include "AEEMedia.h"
/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
// Input information required to encode the image.
// This structure is casted from pEncodeSpec.
// wMainInColor and wThumbInColor is the input image color format and casted from ipl_col_for_type.
// If the image does not include a thumbnail, the thumbnail info are all 0.
// The pHeaderBuf is for exif info from the new decoder later.
typedef struct ImageEncodeSpec
{
  boolean         bQuality;     // flag for selecting nQuality or nMaxFileSize
  union{
    int32         nQuality;     // encoding quality in percentage
    int32         nMaxFileSize; // max. encoding file size
  }size;
  uint16          wMainWidth;   // main image width
  uint16          wMainHigh;    // main image high
  uint16          wMainInColor; // main image color format
  uint16          wThumbWidth;  // thumbnail image width
  uint16          wThumbHigh;   // thumbnail image high
  uint16          wThumbInColor; // thumbnail image color format
  void *          pHeaderBuf;   // buffer for file header information. may be NULL now
} ImageEncodeSpec;

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
ImageEncodeSpec

Description:
This structure contains information required by PNG/JPEG encoder.

Definition:
  typedef struct
  {
    boolean         bQuality;     
    union{
      int32         nQuality;     
      int32         nMaxFileSize; 
    }size;
    uint16          wMainWidth;   
    uint16          wMainHigh;    
    uint16          wMainInColor; 
    uint16          wThumbWidth;  
    uint16          wThumbHigh;   
    uint16          wThumbInColor;
    void *          pHeaderBuf;   
  } ImageEncodeSpec;

Members:
  bQuality:         flag for selecting nQuality or nMaxFileSize
  nQuality:         encoding quality in percentage (1 - 100)
  nMaxFileSize:     max. encoding file size in KB

  wMainWidth:       main image width
  wMainHigh:        main image high
  wMainInColor:     main image color format
  wThumbWidth:      thumbnail image width, could be 0 if no thumbnail.
  wThumbHigh:       thumbnail image high, could be 0 if no thumbnail.
  wThumbInColor:    thumbnail image color format, could be 0 if no thumbnail.
  pHeaderBuf:       buffer for file header information.

Comments:
  This data struct could be assigned to pEncodeSpec in the AEEMediaEncodeInfo.

See Also:
  AEEMediaEncodeInfo.
=============================================================================*/
#endif //AEEENCODESPEC_H





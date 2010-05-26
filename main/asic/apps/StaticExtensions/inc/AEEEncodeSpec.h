#ifndef AEEENCODESPEC_H
#define AEEENCODESPEC_H

/*============================================================================
FILE:        AEEEncodeSpec.h

SERVICES:  BREW MultiMedia Utility Services

DESCRIPTION:
   This file defines image encoding spec for IMediaUtil interface. 

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
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
  uint16          wMainHight;   // main image hight
  uint16          wMainInColor; // main image color format
  uint16          wThumbWidth;  // thumbnail image width
  uint16          wThumbHigh;   // thumbnail image hight
  uint16          wThumbInColor; // thumbnail image color format
  void *          pHeaderBuf;   // buffer for file header information. may be NULL now
} ImageEncodeSpec;

#endif //AEEENCODESPEC_H
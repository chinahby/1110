#ifndef AEEIMAGEEXTFUNC_H
#define AEEIMAGEEXTFUNC_H
/*======================================================
FILE:  AEEImageExtFunc.h

SERVICES:  IIMAGE_ExtFunc()

GENERAL DESCRIPTION:
   Mechanism for extending IImage.

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEImage.h"
#ifdef FEATURE_GEOTAGGING
#include "AEEPosDet.h"
#endif /* FEATURE_GEOTAGGING */

#if !defined(IPARM_EXTFUNC)

#define IPARM_EXTFUNC 13 // extended parameters

// structure for IPARM_EXTFUNC
typedef struct {
   AEECLSID id;
   void    *pBuf;
   int      nBufSize;
} AEEImageExtFunc;

static __inline int IIMAGE_ExtFunc(IImage *me, AEECLSID id, void *pBuf, int nBufSize)
{
   AEEImageExtFunc parm;
   int nReturn = EUNSUPPORTED;

   parm.id = id;
   parm.pBuf = pBuf;
   parm.nBufSize = nBufSize;

   IIMAGE_SetParm(me, IPARM_EXTFUNC, (int)&parm, (int)&nReturn);
   return nReturn;
}

#endif //!defined(IPARM_EXTFUNC)

#ifdef FEATURE_JPEGD_RESIZE
#if !defined(JPEGD_RESIZE)

#define JPEGD_RESIZE 14 // JPEGD Resize functionality

typedef struct {
  uint32     resizeImageWidth;  // request for resize image width
  uint32     resizeImageHeight; // request for resize image height
  boolean    aspectRatioFlag;   // preserve/eliminate aspect ratio
  boolean    jpegdResizeFlag;   // enable/disable resize feature
} jpegdResizeInfo;

#endif //!defined(JPEGD_RESIZE)
#endif //FEATURE_JPEGD_RESIZE

#ifdef FEATURE_GEOTAGGING
#define ADD_TAGS 15

typedef struct {
  char m_sCurrentFileName[80]; // current image Filename to be passed to OEM
  AEEGPSInfo *gps_info; // gps info to add sent to OEM 
} GeoTagInfo;
#endif /* FEATURE_GEOTAGGING */
/*=============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================
jpegdResizeInfo

Description:
This structure specifies the resize information required to configure
the JPEG decoder

Definition:
  typedef struct {
    uint32     resizeImageWidth;
    uint32     resizeImageHeight;
    boolean    aspectRatioFlag;
    boolean    jpegdResizeFlag;
  } jpegdResizeInfo;

Members:
  resizeImageWidth:  request for resize image width
  resizeImageHeight: request for resize image height
  aspectRatioFlag:   flag to preserve/eliminate aspect ratio
  jpegdResizeFlag:   flag to enable/disable JPEG decoder resize feature

Comments:
  None.

See Also:
  IIMAGE_ExtFunc()

=============================================================================
Refer to AEEImage.h for other document.
===========================================================================*/
#endif //AEEIMAGEEXTFUNC_H

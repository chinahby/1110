#ifndef AEEIMAGEINFO_H
#define AEEIMAGEINFO_H
/*======================================================
FILE:  AEEImageInfo.h

SERVICES:  AEEImageInfo struct definition

GENERAL DESCRIPTION:
   Defines BREW AEEImageInfo type.

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEStdDef.h"

typedef struct _AEEImageInfo
{
   uint16   cx;
   uint16   cy;
   uint16   nColors;
   boolean  bAnimated;
   uint16   cxFrame;
} AEEImageInfo;


/*=======================================================================
AEEImageInfo

Description:
This structure gets the information about an image

Definition:
typedef struct _AEEImageInfo
{
   uint16 cx;
   uint16 cy;
   uint16 nColors;
   boolean bAnimated;
   uint16 cxFrame;
} AEEImageInfo;

Members:

cx : The width of the image (in pixels)
cy : The height of the image (in pixels)
nColors : The number of colors in the image, or 0 for images with more than 65535 colors
bAnimated : TRUE, if the image contains animation
cxFrame : If the image is divided into frames, this member indicates the width of
          each frame

Comments:
None

See Also:
None

=======================================================================*/
#endif //AEEIMAGEINFO_H


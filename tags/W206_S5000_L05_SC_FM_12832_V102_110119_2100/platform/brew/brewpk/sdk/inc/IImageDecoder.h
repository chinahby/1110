#ifndef  IIMAGEDECODER_H
#define  IIMAGEDECODER_H
/*=================================================================================
FILE:          IImageDecoder.h

SERVICES:      IImageDecoder interface

DESCRIPTION:   

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

Copyright © 2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEBitmap.h"

//*********************************************************************************
//
// IImageDecoder Interface
//
//*********************************************************************************

#define AEEIID_IMAGEDECODER 0x01026e20

typedef struct IImageDecoder IImageDecoder;

#define INHERIT_IImageDecoder(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*GetBitmap)(iname *po, IBitmap **ppiBitmap); \
   int  (*GetRop)(iname *po)


AEEINTERFACE(IImageDecoder)
{
   INHERIT_IImageDecoder(IImageDecoder);
};


// Access macros for IImageDecoder
#define  IIMAGEDECODER_AddRef(p)                AEEGETPVTBL(p,IImageDecoder)->AddRef(p)
#define  IIMAGEDECODER_Release(p)               AEEGETPVTBL(p,IImageDecoder)->Release(p)
#define  IIMAGEDECODER_QueryInterface(p,i,p2)   AEEGETPVTBL(p,IImageDecoder)->QueryInterface((p),(i),(p2))
#define  IIMAGEDECODER_GetBitmap(p,b)           AEEGETPVTBL(p,IImageDecoder)->GetBitmap((p),(b))
#define  IIMAGEDECODER_GetRop(p)                AEEGETPVTBL(p,IImageDecoder)->GetRop(p)




/*============================================================================
   INTERFACE DOCUMENTATION
==============================================================================

IImageDecoder Interface

Description:
   This interface is used to retrieve an IBitmap from an image decoder.
   Typically, classes that export the IImageDecoder interface will also export
   another interface that is used to feed encoded data to the decoder.
   Currently, the IForceFeed interface exists for this purpose.

   The IImageDecoder interface may be queried from an object with
   AEEIID_IMAGEDECODER.

   The following header file is required:~
   IImageDecoder.h

   The following table lists image decoders that may be present on the device.
   Each of these decoders implements IImageDecoder as its default interface
   and also implements the IForceFeed interface.

===pre>
   Image type   Class ID               BID file
   ----------   --------------------   ---------------
   BMP          AEECLSID_BMPDECODER    BMPDecoder.bid
   JPEG         AEECLSID_JPEGDECODER   JPEGDecoder.bid
   PNG          AEECLSID_PNGDECODER    PNGDecoder.bid
===/pre>

==============================================================================
Function: IIMAGEDECODER_AddRef()

Description:
	This function is inherited from IQI_AddRef().
	
==============================================================================
Function: IIMAGEDECODER_Release()

Description:
	This function is inherited from IQI_Release().

==============================================================================
Function: IIMAGEDECODER_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().
	
==============================================================================
Function: IIMAGEDECODER_GetBitmap()

Description:
   This function is used to retrieve the decoded bitmap.

Prototype:
   int IIMAGEDECODER_GetBitmap(IImageDecoder *po, IBitmap **ppiBitmap);

Parameters:
   po          [in]: Pointer to IImageDecoder interface.
   ppiBitmap  [out]: Decoded image..

Return Value:
   SUCCESS: if function executed correctly.~
   EFAILED: if no image has been decoded.

Comments:
   The IBitmap that is returned is typically an IDIB.  You should use
   IBITMAP_QueryInterface() to retrieve the IDIB pointer.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.2.

See Also:
   IBitmap, IDIB

==============================================================================
Function: IIMAGEDECODER_GetRop()

Description:
   This function returns the default raster op to be used when blitting this
   bitmap.

Prototype:
   int IIMAGEDECODER_GetRop(IImageDecoder *po);

Parameters:
   po          [in]: Pointer to IImageDecoder interface.

Return Value:
   Default raster op for this image.

Comments:
   This will return AEE_RO_COPY for opaque images, and either
   AEE_RO_TRANSPARENT or AEE_RO_BLEND for transparent images, depending on the
   transparency type.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.2.

See Also:
   AEERasterOp

============================================================================*/


#endif /* IIMAGEDECODER_H */


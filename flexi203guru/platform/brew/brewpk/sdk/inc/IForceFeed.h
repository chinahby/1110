#ifndef  IFORCEFEED_H
#define  IFORCEFEED_H
/*=================================================================================
FILE:          IForceFeed.h

SERVICES:      IForceFeed interface

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

//*********************************************************************************
//
// IForceFeed Interface
//
//*********************************************************************************

#define AEEIID_FORCEFEED 0x0101eb0b

typedef struct IForceFeed IForceFeed;

#define INHERIT_IForceFeed(iname) \
   INHERIT_IQueryInterface(iname); \
   int      (*Write)(iname *po, void *pBuf, int cb); \
   void     (*Reset)(iname *po)


AEEINTERFACE(IForceFeed)
{
   INHERIT_IForceFeed(IForceFeed);
};


// Access macros for IForceFeed
#define  IFORCEFEED_AddRef(p)                         AEEGETPVTBL(p,IForceFeed)->AddRef(p)
#define  IFORCEFEED_Release(p)                        AEEGETPVTBL(p,IForceFeed)->Release(p)
#define  IFORCEFEED_QueryInterface(p,i,p2)            AEEGETPVTBL(p,IForceFeed)->QueryInterface(p,i,p2)
#define  IFORCEFEED_Write(p,b,c)                      AEEGETPVTBL(p,IForceFeed)->Write(p,b,c)
#define  IFORCEFEED_Reset(p)                          AEEGETPVTBL(p,IForceFeed)->Reset(p)




/*============================================================================
   INTERFACE DOCUMENTATION
==============================================================================

IForceFeed Interface

Description:
   This interface is used for feeding data to an object.  The image decoders
   are examples of classes that use this interface.  (See IImageDecoder.)
   
   The IForceFeed interface may be queried from an object with
   AEEIID_FORCEFEED.

   The following header file is required:~
   IForceFeed.h

==============================================================================
Function: IFORCEFEED_AddRef()

Description:
	This function is inherited from IQI_AddRef().

==============================================================================
Function: IFORCEFEED_Release()

Description:
	This function is inherited from IQI_Release().

==============================================================================
Function: IFORCEFEED_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().

==============================================================================
Function: IFORCEFEED_Write()

Description:
   This function is used to write data to the object.  The data written is
   considered a continuation of data written with previous calls to this
   function, since the last call to IFORCEFEED_Reset().

Prototype:
   int IFORCEFEED_Write(IForceFeed *po, void *pBuf, int cb);

Parameters:
   po          [in]: Pointer to IForceFeed interface.
   pBuf        [in]: Pointer to buffer containing data to write, or NULL to
                     signify the end of the stream of data.
   cb          [in]: Number of bytes of data to write.

Return Value:
   SUCCESS: if function executed correctly.
   
   Implementation specific error codes.

Comments:
   The error code returned may be due to an internal error, such as not
   enough memory, or due to the data being in the wrong format.  For
   instance, a PNG image decoder would return an error code here if
   the data was not a PNG image.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.2.

See Also:
   IFORCEFEED_Reset()

==============================================================================
Function: IFORCEFEED_Reset()

Description:
   This function is used to prepare the object to accept a new stream of data.

Prototype:
   void IFORCEFEED_Reset(IForceFeed *po);

Parameters:
   po          [in]: Pointer to IForceFeed interface.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.2.

See Also:
   IFORCEFEED_Write()

============================================================================*/


#endif /* IFORCEFEED_H */


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

/*
===============================================================================

IIMAGE_ExtFunc()

Description:
   This function provides an extension mechanism for IImage.  It is
   implemented as an inline function on top of IIMAGE_SetParm(), using the
   IPARM_EXTFUNC parameter ID.  IMAGE_ExtFunc() provides a mechanism for
   return values and unique IDs.

   In order to add ExtFunc support to an IImage implementation, you must
   handle IPARM_EXTFUNC in IIMAGE_SetParm().  Here is an example:

===pre>   
   static int MyImage_ExtFunc(IImage *po, AEECLSID id, void *pBuf, int nBufSize)
   {
      switch (id) {
      case AEECLSID_MYEXTENSION:
         // your extension code goes here
         return SUCCESS;
      default:
         return EUNSUPPORTED;
      }
   }

   static void MyImage_SetParm(IImage *po, int nParm, int n1, int n2)
   {
      switch(nParm){
         .
         .
         .
      case IPARM_EXTFUNC:
         *(int *)n2 = MyImage_ExtFunc(po,
                                       ((AEEImageExtFunc *)n1)->id,
                                       ((AEEImageExtFunc *)n1)->pBuf,
                                       ((AEEImageExtFunc *)n1)->nBufSize);
         break;
      }
   }
===/pre>   

   Note that the class ID used should be generated with the BREW Extranet Class
   ID Generator, and used only for this purpose.  This convention eliminates
   the possibility of ID collisions.

Prototype:
   int IIMAGE_ExtFunc(IImage *pif, AEECLSID id, void *pBuf, int nBufSize);

Parameters:
   pif:  Pointer to IFoo interface
   id:   Unique ID that determines what this function will do

Return Value:
   SUCCESS - successful execution
   EUNSUPPORTED - ExtFunc or ID is not supported
   Particular IDs may be defined to return other error codes.

See Also:
   IIMAGE_SetParm()

===============================================================================
*/

#endif //AEEIMAGEEXTFUNC_H

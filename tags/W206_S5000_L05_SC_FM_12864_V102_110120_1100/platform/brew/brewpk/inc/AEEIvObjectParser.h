#ifndef AEEIVOBJECTPARSER_H
#define AEEIVOBJECTPARSER_H
/*==============================================================================

File:
   AEEIvObjectParser.h

Services:
   Provides parser related functionality to vCard or vCalendar

General Description:
   Parses or generates a data stream consisting of either vCard or vCalendar

Public Classes And Static Functions:
   IvObjectParser

INITIALIZATION AND SEQUENCING REQUIREMENTS:

      (c) COPYRIGHT 2007 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/
#include "AEEIvObject.h"

#define AEEIID_IvObjectParser 0x0105f45d 

#define INHERIT_IvObjectParser(iname)                                         \
   INHERIT_IQI(iname);                                                        \
   int (*GetNumberOfParsedObjects)(iname        *po,                          \
                                   int          *pnNumObjects,                \
                                   AEECLSID  *pnObjectType);                  \
   int (*GetObjectAtIndex)(iname *po, int nIndex, IvObject **ppIvObject);     \
   int (*GetBufferFromIvObject)(iname    *po,                                 \
                                IvObject *pIvObject,                          \
                                char     *pBuf,                               \
                                int      nBufSize,                            \
                                int      *pnBufSizeReq)


AEEINTERFACE_DEFINE(IvObjectParser);

static __inline uint32 IvObjectParser_AddRef(IvObjectParser *pIvObjectParser)
{
   return AEEGETPVTBL(pIvObjectParser, IvObjectParser)->AddRef(pIvObjectParser);
}


static __inline uint32 IvObjectParser_Release(IvObjectParser *pIvObjectParser)
{
   return AEEGETPVTBL(pIvObjectParser,IvObjectParser)->Release(pIvObjectParser);
}

static __inline int IvObjectParser_QueryInterface(IvObjectParser *pIvObjectParser,
                                                  AEEIID         iid,
                                                  void           **ppo)
     
{
   return AEEGETPVTBL(pIvObjectParser, IvObjectParser)->QueryInterface(pIvObjectParser,
                                                                       iid,
                                                                       ppo);
         
}

static __inline int IvObjectParser_GetNumberOfParsedObjects(IvObjectParser *pIvObjectParser,
                                                            int            *pnNumObjects,
                                                            AEECLSID       *pnObjectType)
{
   return AEEGETPVTBL(pIvObjectParser,IvObjectParser)->GetNumberOfParsedObjects(pIvObjectParser,
                                                                                pnNumObjects,
                                                                                pnObjectType);
}

static __inline int IvObjectParser_GetObjectAtIndex(IvObjectParser *pIvObjectParser,
                                                    int            nIndex,
                                                    IvObject       **ppIvObject)
{
   return AEEGETPVTBL(pIvObjectParser, IvObjectParser)->GetObjectAtIndex(pIvObjectParser,
                                                                         nIndex,
                                                                         ppIvObject);
}

static __inline int IvObjectParser_GetBufferFromIvObject(IvObjectParser *pIvObjectParser,
                                                         IvObject       *pIvObject,
                                                         char           *pBuf,
                                                         int            nBufSize,
                                                         int            *pnBufSizeReq)
{
   return AEEGETPVTBL(pIvObjectParser, IvObjectParser)->GetBufferFromIvObject(pIvObjectParser,
                                                                              pIvObject,
                                                                              pBuf,
                                                                              nBufSize,
                                                                              pnBufSizeReq);
}



/*
===============================================================================
   INTERFACE   DOCUMENTATION
===============================================================================
IvObjectParser Interface

Description:

   This interface provides parsing functionality for a vCard or
   vCaleandar/iCalendar object

===============================================================================
IvObjectParser_AddRef

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvObjectParser_Release()

===============================================================================
IvObjectParser_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IvObjectParser_AddRef()

===============================================================================
IvObjectParser_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

===============================================================================
IvObjectParser_GetNumberOfParsedObjects

Description:
   This function will let the user know the number of IvObjects created while
   parsing

   We can a group of vCards or vCalendar buffers passed in one datastream

Prototype:
   int IvObjectParser_GetNumberOfParsedObjects(IvObjectParser *pIvObjectParser,
                                               int            *pnNumObjects,
                                               AEECLSID       *pnObjectType);

Parameters:
   pIvObjectParser   :  Pointer to the IvObjectParser interface
   pnNumObjects      :  Upon return holds number of objects created
   pnObjectType      :  Will let the user know of what kind of buffer has been
                        parsed

Return Value:
   AEE_SUCCESS    :  If we can get number of objects
   AEE_EBADPARM   :  If either pnNumObjects or pnObjectType is NULL

Comments:
   None

See Also:
   None

===============================================================================
IvObjectParser_GetObjectAtIndex

Description:
   Returns a newly instantiated IvObject.

   The Passed in IvObject has to be of type pnObjType in
   IvObjectParser_GetParsedStatus
   
Prototype:
   int IvObjectParser_GetObjectAtIndex(IvObjectParser *pIvObjectParser,
                                       int            nIndex,
                                       IvObject       **ppIvObject);

Parameters:
   pIvObjectParser : Pointer to the IvObjectParser interface
   nIndex          : Index at which users want to retrieve the object, the index
                     is zero based
   ppIvObject      : Pointer to the IvObject that has to created

 Return Value:
   AEE_SUCCESS  : If we are able to initialise the object correctly
   AEE_EBADPARM : If either dwIndex or ppIvObject is invalid

Comments:
   None
   
See Also:
   None

===============================================================================
IvObjectParser_GetBufferFromIvObject

Description:
   Returns the data buffer generated from passed in IvObject

Prototype:
   int IvObjectParser_GetBufferFromIvObject(IvObjectParser *pIvObjectParser,
                                            IvObject       *pIvObject,
                                            char           *pBuf,
                                            int            nBufSize,
                                            int            *pnBufSizeReq);

Parameters:
   pIvObjectParser : Pointer to the IvObjectParser interface
   pIvObject       : Pointer to the IvObject interface for which the buffer has
                     to be generated
   pBuf            : Pointer to the generated buffer
   nBufSize        : Size of the pBuf
   pnBufSizeReq    : Required bufsize to hold the data stream


Return Value:
   AEE_SUCCESS    : If we are able to generate the buffer
   AEE_EBADPARM   : If either pIvObject or pBuf is NULL
   AEE_ENOMEMORY  : If there is no enough memory to generate the data stream

Comments:
   None

See Also:
   None

===============================================================================
*/


#endif  //AEEIVOBJECTPARSER_H

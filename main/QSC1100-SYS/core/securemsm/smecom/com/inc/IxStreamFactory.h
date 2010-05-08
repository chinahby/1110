#ifndef __IXSTREAMFACTORY_H__
#define __IXSTREAMFACTORY_H__

/*===========================================================================


                                     I X  S T R E A M  F A C T O R Y        
                                          H E A D E R  F I L E

FILE:  IxStreamFactory.h

GENERAL DESCRIPTION

  This header file defines the IxStreamFactory class

  This is the class used to create a particular type of stream such as file stream
  or socketstream or http stream based on the uri being passed to it.

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxStreamFactory.h#3 $
  $DateTime: 2007/11/19 18:32:07 $
  $Author: rajeevg $

===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include "IxErrno.h"
#include "UxURI.h"
#include "IxStream.h"

/*------------------------------------------------------------------------
                               DATA TYPES
------------------------------------------------------------------------*/

#define IXSTREAM_FACTORY_MAX_STREAM_TYPES 16

typedef IxStream * (*IxStreamFactoryNewCbT) (void);

typedef struct
{
  int                   streamType;
  char                 *pURIScheme;
  IxStreamFactoryNewCbT newCb;
} IxStreamFactoryTypeMapT;

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxStreamFactory
{
/*------------------------------------------------------------------------
                               PUBLIC TYPES
 ------------------------------------------------------------------------*/
   

  public:

/*------------------------------------------------------------------------
                               PUBLIC METHODS
 ------------------------------------------------------------------------*/

/*==============================================================
FUNCTION:  IxStreamFactory::IxStreamFactory

DESCRIPTION:

Constructor to create an instance of this stream factory class

INPUT/OUTPUT PARAMETERS: None

RETURN VALUE: None 

SIDE EFFECTS: None
==============================================================*/

  IxStreamFactory();


/*==============================================================
FUNCTION:  IxStreamFactory::~IxStreamFactory

DESCRIPTION:

Destructor to do cleanup operations

INPUT/OUTPUT PARAMETERS: None

RETURN VALUE: None 

SIDE EFFECTS: None
==============================================================*/

  ~IxStreamFactory();


/*==============================================================
FUNCTION:  IxStreamFactory::CreateStream

DESCRIPTION:

Create a particular type of stream based on the URI passed in. The
scheme specified in the URI is used to determine whether to create
a filestream, bufstream, etc. When the operation completes, the
specified callback function is called with the result of the operation.

This method is asynchronous

INPUT/OUTPUT PARAMETERS:

  UxURI*               pUri     -> pointer to the populated UxURI object
  IxStream**           ppStream -> pointer to the created stream object
  IxStreamCbT          pfn      -> callback function
  IxStreamOpenRspT*    pOpenRsp -> argument to callback function
  IxStreamAccessRights access   -> access rights

RETURN VALUE: E_SUCCESS or E_WOULD_BLOCK if successful,
  error code otherwise

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType CreateStream(UxURI *pURI, 
    IxStream **ppStream, 
    IxStreamCbT pfn = NULL, 
    IxStreamOpenRspT *pOpenRsp = NULL,
    IxStreamAccessRights access = IX_STRM_READWRITE);

/*==============================================================
FUNCTION:  IxStreamFactory::CreateStream

DESCRIPTION:

Create a particular type of stream based on the URI passed in. The
scheme specified in the URI is used to determine whether to create
a filestream, bufstream, etc.

INPUT/OUTPUT PARAMETERS:

  UxURI *pUri         -> pointer to the populated UxURI object
  IxStream **ppStream -> pointer to the created stream object

RETURN VALUE: E_SUCCESS if successful, error code otherwise

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType CreateStream(UxURI *pURI, 
    IxStream **ppStream, 
    IxStreamAccessRights access = IX_STRM_READWRITE);


/*==============================================================
FUNCTION:  IxStreamFactory::CreateStream - IxStreamBuf version

DESCRIPTION:

Create a bufstream based on the allocation size passed in.

INPUT/OUTPUT PARAMETERS:

  uint32               allocsize -> the size of the IxStreamBuf to allocate
  IxStream**           ppStream  -> pointer to the created stream object
  IxStreamCbT          pfn       -> callback function
  IxStreamOpenRspT*    pOpenRsp  -> argument to callback function
  IxStreamAccessRights access    -> access rights

RETURN VALUE: E_SUCCESS or E_WOULD_BLOCK if successful,
  error code otherwise

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType CreateStream(uint32 allocsize, 
    IxStream **ppStream,
    IxStreamCbT pfn = NULL,
    IxStreamOpenRspT *pOpenRsp = NULL,
    IxStreamAccessRights access = IX_STRM_READWRITE);


/*==============================================================
FUNCTION:  IxStreamFactory::CreateStream - Shell version

DESCRIPTION:

Creates an IxStream object based on the type of IxSream specified by
the enumerated types.

INPUT/OUTPUT PARAMETERS:

  IxStreamTypeT        streamType -> enumerated IxStream type
  IxStream**           ppStream   -> pointer to the created stream object

RETURN VALUE: E_SUCCESS on success, other error code if not.

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType CreateStream (int streamType, IxStream **ppStream);


/*==============================================================
FUNCTION:  IxStreamFactory::CreateStream - Shell version 2

DESCRIPTION:

Creates an IxStream object based on the Stream metadata passed.

INPUT/OUTPUT PARAMETERS:

  uint8*        pStreamStateData -> pointer to stream state data
  IxStream**    ppStream         -> pointer to the created stream object

RETURN VALUE: E_SUCCESS on success, other error code if not.

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType CreateStream (uint8 *pStreamStateData, IxStream **ppStream);

/*==============================================================
FUNCTION:  IxStreamFactory::RegisterStreamType

DESCRIPTION:

Registers a new type of IxStream object with the factory. Allows the
factory to create an object of this type.

INPUT/OUTPUT PARAMETERS:
  int                   streamType -> stream type to be registered
  IxStreamFactoryNewCbT pfn        -> callback function

RETURN VALUE: E_SUCCESS on success, other error code on failure.

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType RegisterStreamType (int streamType,
    IxStreamFactoryNewCbT pfn);

/*==============================================================
FUNCTION:  IxStreamFactory::RegisterURIScheme

DESCRIPTION:

Registers a URI scheme with the factory by associating a stream type
with the URI scheme. Allows the factory to decide which type of stream
to create for given a URI scheme.

INPUT/OUTPUT PARAMETERS:
  const char *   pURIScheme -> URI scheme
  int            streamType -> stream type to be registered

RETURN VALUE: E_SUCCESS on success, other error code on failure.

SIDE EFFECTS: None
==============================================================*/

  IxErrnoType RegisterURIScheme (const char *pURIScheme,
    int streamType);

/*------------------------------------------------------------------------
                              PRIVATE METHODS
 ------------------------------------------------------------------------*/

private:

  IxErrnoType CreateStreamInternal (UxURI *pURI,
    IxStream **ppStream,
    IxStreamCbT pfn,
    IxStreamOpenRspT *pOpenRsp,
    IxStreamAccessRights access);

  IxStreamFactoryTypeMapT __typeMap[IXSTREAM_FACTORY_MAX_STREAM_TYPES];
};

#endif


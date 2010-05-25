#ifndef __IXSTREAMUTILS_H__
#define __IXSTREAMUTILS_H__

/*===========================================================================

                              I X  S T R E A M  U T I L S

                                  H E A D E R  F I L E

FILE:  IxStreamUtils.h


GENERAL DESCRIPTION

  Stream-related utility functions.

PUBLIC CLASSES AND STATIC FUNCTIONS
  
INITIALIZATION & SEQUENCING REQUIREMENTS
  
  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxStreamUtils.h#2 $
  $DateTime: 2008/05/07 17:39:52 $
  $Author: amundi $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
2006/11/01  ws     Add IxStreamBufCreateFromSource
2006/10/25  gr     Added C API.
2006/08/08  gr     Made RecreateStream a synchronous function.
2006/07/27  gr     Added RecreateStream function. Updated comments to reflect
                   reality.
2006/06/16 ssm     Move to IxCallback
2006/01/09  gr     Initial version
===========================================================================*/


/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include "IxStream.h"

#ifndef FEATURE_DRM_NO_BREW

#include "AEESource.h"
#include "AEEIContentHandler.h"

/*
** XXX: Remove this inclusion. Only here because some source files are not
** including all the headers they should.
*/
#ifdef __cplusplus
#include "IxStreamFile.h"
#endif

/*------------------------------------------------------------------------
                               PUBLIC FUNCTIONS
 ------------------------------------------------------------------------*/

#ifdef __cplusplus

/*==============================================================
* FUNCTION:  IxStreamCreateSource()

DESCRIPTION:
  Create a source object from a stream.

INPUT/OUTPUT PARAMETERS:
  @param  IxStream*    pStream  - pointer to stream object
  @param  ISource**    ppSource - pointer to created source object
  @param  IxStreamCbT  pCb      - callback function
  @param  IxCallback*  pRsp     - input to callback function

RETURN VALUE:
  Error Code. E_SUCCESS or E_AGAIN is returned if there is no error. If
  E_AGAIN is returned, the specified callback is invoked when the
  operation completes.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType IxStreamCreateSource (IxStream *pStream, ISource **ppSource,
  IxStreamCbT pCb, IxCallback *pRsp);

/*==============================================================
* FUNCTION:  IxStreamCreateFromSource()

DESCRIPTION:
  Create a stream object from a source object.

INPUT/OUTPUT PARAMETERS:
  @param  ISource*    pSource  - pointer to source object
  @param  IxStream**  ppStream - pointer to created stream object

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType IxStreamCreateFromSource (ISource *pSource, IxStream **ppStream);

/*==============================================================
* FUNCTION:  IxStreamBufCreateFromSource()

DESCRIPTION:
  Create a stream buf object from a source object.

  This function will attempt to read all all the data available from
  the source and store it as an IxStreamBuf.  This IxStreamBuf is then
  returned here.

INPUT/OUTPUT PARAMETERS:
  @param  ISource*    pSource  - pointer to source object
  @param  IxStream**  ppStream - pointer to created stream buf object

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType IxStreamBufCreateFromSource (ISource *pSource, IxStream **pStream);

/*==============================================================
* FUNCTION:  IxStreamRecreateStream()

DESCRIPTION:
  Create a stream object from a source object that itself has been created
  from another stream object.

INPUT/OUTPUT PARAMETERS:
  @param  ISource*    pSource   - pointer to source object
  @param  IxStream**  ppStream  - pointer to created stream object

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType IxStreamRecreateStream (ISource *pSource, IxStream **ppStream);

///*==============================================================
//* FUNCTION:  IxStreamSetContentPlayer()
//
//DESCRIPTION:
//  Save the content player pointer in the ISource
//
//INPUT/OUTPUT PARAMETERS:
//  @param  ISource*          pSource   - pointer to source object
//  @param  IxContentPlayerClient*  pContentPlayer  - pointer to content player client object
//
//RETURN VALUE:
//  Error Code. E_SUCCESS is returned if there is no error.
//
//SIDE EFFECTS: None
//==============================================================*/
//IxErrnoType IxStreamSetContentPlayer (ISource *pSource, IxContentPlayer *pContentPlayer);

/*==============================================================

* FUNCTION:  IxStreamSetContentPlayer()

DESCRIPTION:
  Save the content player pointer in the ISource

INPUT/OUTPUT PARAMETERS:
  @param  ISource*          pSource   - pointer to source object
  @param  IxContentPlayerClient*  pContentPlayer  - pointer to content player client object

RETURN VALUE:
  Error Code. E_SUCCESS is returned if there is no error.

SIDE EFFECTS: None
==============================================================*/
IxErrnoType IxStreamSetISourceHandler (ISource *pSource, ISourceHandler *pISourceHandler);

#endif /* __cplusplus */

/*------------------------------------------------------------------------
                               C WRAPPERS
 ------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

IxErrnoType ix_stream_create_source (void *pStream, ISource **ppSource,
  IxStreamCbT pCb, void *pRsp);

IxErrnoType ix_stream_create_from_source (ISource *pSource, void **pStream);

IxErrnoType ix_stream_recreate_stream (ISource *pSource, void **ppStream);

IxErrnoType ix_set_i_source_handler (ISource *pSource, ISourceHandler *pISourceHandler);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //FEATURE_DRM_NO_BREW

#endif /* IXSTREAM_UTILS_H */


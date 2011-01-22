#ifndef AEEMEDIAUTIL_H
#define AEEMEDIAUTIL_H

/*============================================================================
FILE:        AEEMediaUtil.h

SERVICES:  BREW MultiMedia Utility Services

DESCRIPTION:
   This file defines IMediaUtil interface. IMediaUtil provides utility functions,
   used along with IMediaUtil and IImage interfaces, to BREW multimedia applications.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEMedia.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
// Define interface pointer
typedef struct IMediaUtil        IMediaUtil;

//
// Input information required to encode new media.
// This structure is filled by caller.
//
typedef struct AEEMediaEncodeInfo
{
   AEEMediaData * pmdDest;    // Destination media data. Must remain valid thrroghout a transaction.
   AEEMediaData * pmdList;    // List of source media data
   int16          nCount;     // Number of sources
   void *         pEncodeSpec;// Encoding specifications. Depends on source and destination.
   uint32         dwSpecSize; // Size of pEncodeSpec
} AEEMediaEncodeInfo;

//
// Resulting information after the completion of encode operation.
// This structure is filled by BREW.
//
typedef struct AEEMediaEncodeResult
{
   int16          nResult;    // Result of encode operation. SUCCESS or error code
   AEECLSID       clsMedia;   // Destination object class ID
   AEEMediaData * pmdDest;    // Destination media data. Same pointer specified in AEEMediaEncodeInfo
   void *         pEncodeSpec;// Resulting encoded specifications. Memory alloc'd and freed by BREW
   uint32         dwSpecSize; // Size of pEncodeSpec
} AEEMediaEncodeResult;

typedef struct AEEMediaCreateInfo
{
   uint32            dwStructSize;  // Size of this struct

   int               nState;        // Desired media state: MM_STATE_IDLE or MM_STATE_READY

   int               nCount;        // Number of elements in the pmdList and pnCapsList
   AEEMediaDataEx   *pmdList;       // List of media data structures
   int              *pnCapsList;    // Corresponding list of enabled caps
} AEEMediaCreateInfo;

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//********************************************************************************************************************************
//
// IMediaUtil Interface
//
//********************************************************************************************************************************

AEEINTERFACE(IMediaUtil)
{
   INHERIT_IQueryInterface(IMediaUtil);

   int   (*CreateMedia)(IMediaUtil * po, AEEMediaData * pmd, IMedia ** ppm);
   int   (*EncodeMedia)(IMediaUtil * po, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb);
   int   (*CreateMediaEx)(IMediaUtil * po, AEEMediaCreateInfo * pcmi, IMedia ** ppm);
};

#define IMEDIAUTIL_AddRef(p)                             AEEGETPVTBL(p, IMediaUtil)->AddRef(p)
#define IMEDIAUTIL_Release(p)                            AEEGETPVTBL(p, IMediaUtil)->Release(p)
#define IMEDIAUTIL_QueryInterface(p, i, p2)              AEEGETPVTBL(p, IMediaUtil)->QueryInterface(p, i, p2)

#define IMEDIAUTIL_CreateMedia(p, pmd, ppm)              AEEGETPVTBL(p, IMediaUtil)->CreateMedia(p, pmd, ppm)
#define IMEDIAUTIL_CreateMediaEx(p, pcmi, ppm)           AEEGETPVTBL(p, IMediaUtil)->CreateMediaEx(p, pcmi, ppm)

#define IMEDIAUTIL_EncodeMedia(p, per, cls, pei, pcb)    AEEGETPVTBL(p, IMediaUtil)->EncodeMedia(p, per, cls, pei, pcb)
#define IMEDIAUTIL_EncodePMD(p, per, pei, pcb)           AEEGETPVTBL(p, IMediaUtil)->EncodeMedia(p, per, AEECLSID_MEDIAPMD, pei, pcb)
#define IMEDIAUTIL_EncodeJPEG(p, per, pei, pcb)          AEEGETPVTBL(p, IMediaUtil)->EncodeMedia(p, per, AEECLSID_JPEG, pei, pcb)

#ifdef __cplusplus
extern "C" {
#endif
//
// NOTE: These functions are DEPRECATED and are provided for backward compatibility only.
//       Use IMediaUtil methods instead.
//
extern int AEEMediaUtil_CreateMedia(IShell * ps, AEEMediaData * pmd, IMedia ** ppm);
extern int AEEMediaUtil_FindClass(IShell * ps, AEEMediaData * pmd, AEECLSID * pCls);
#ifdef __cplusplus
}
#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEMediaEncodeInfo

Description:
This structure specifies the input information required to encode new media.
It is structure filled by the caller.

Definition:
   typedef struct AEEMediaEncodeInfo
   {
      AEEMediaData * pmdDest;
      AEEMediaData * pmdList;
      int16          nCount;
      void *         pEncodeSpec;
      uint32         dwSpecSize;
   } AEEMediaEncodeInfo;

Members:
   pmdDest:       Destination media data. Must remain valid thrroghout a transaction.
   pmdList:       List of source media data
   nCount:        Number of sources
   pEncodeSpec:   Encoding specifications. Depends on source and destination.
   dwSpecSize:    Size of pEncodeSpec

Comments:
pEncodeSpec depends on the destination media type as well as (in many cases)
on source media. For example, to encode a particular destination media (like MPEG4), it may
require that you specify timing information to process one or more input media.

See Also:
   IMEDIAUTIL_EncodeMedia()

=======================================================================

AEEMediaEncodeResult

Description:
This is the resulting information after the completion of the encode operation.
This structure is filled by BREW.

Definition:
   typedef struct AEEMediaEncodeResult
   {
      int16          nResult;
      AEECLSID       clsMedia;
      AEEMediaData * pmdDest;
      void *         pEncodeSpec;
      uint32         dwSpecSize;
   } AEEMediaEncodeResult;

Members:
   nResult:       Result of encode operation. SUCCESS or error code
   clsMedia:      Destination object class ID
   pmdDest:       Destination media data. Same pointer specified in AEEMediaEncodeInfo.
   pEncodeSpec:   Resulting encoded specifications. Memory alloc'd and freed by BREW
   dwSpecSize:    Size of pEncodeSpec

Comments:
The encoding result is returned via nResult. pEncodeSpec depends on the destination media.

This structure remains valid within the callback function and you need to make a copy
of it. DO NOT free this memory, as it is owned by BREW.

See Also:
   IMEDIAUTIL_EncodeMedia()

=======================================================================

AEEMediaCreateInfo

Description:
This structure specifies the data necessary to create IMedia-based object.

Definition:
   typedef struct AEEMediaCreateInfo
   {
      uint32            dwStructSize; 

      int16             nState;

      int16             nCount;       
      AEEMediaDataEx   *pmdList;      
      int              *pnCapsList;   
   } AEEMediaCreateInfo;

Members:
   dwStructSize:  Size of this struct
   nState:        Desired media state: MM_STATE_IDLE or MM_STATE_READY
   nCount:    Number of elements in the pmdList/pnCapsList
   pmdList:       List of media data structures
   pnCapsList:    Corresponding list of enabled caps

Comments:
   Each element of pnCapsList corresponds to an element in same position
   in pmdList.

   You can specify one or more AEEMediaDataEx and also specify which capabilities
   are enabled for each of them. For example, you can specify two MPEG4 files and
   play video from first and audio from the second.

   nReqState specifies the desired state of IMedia object when the function returns. 
   If the desired state is MM_STATE_IDLE, then the function will not perform 
   IMEDIA_SetMediaData() on the created IMedia. The user needs to set the media data. 
   If the desired state is MM_STATE_READY, then the function will set the specified media
   data and puts the media in ready state.

See Also:
   IMEDIAUTIL_CreateMediaEx()

=====================================================================
  INTERFACE DOCUMENTATION
=====================================================================
IMediaUtil Interface

This interface provides multimedia utility services. BREW applications, using IMedia or
IMedia-based classes, can take advantage of these services.

IMediaUtil helps in

 -    Creating IMedia-based object based on the input media info

 -    Encoding new media

The capability to encode new media largely depends on the device multimedia capability.


The following header file is required:~
IMediaUtil.h

=============================================================================

IMEDIAUTIL_AddRef()

IMEDIA_AddRef()

Description:
    This function is inherited from IBASE_AddRef().


=============================================================================

IMEDIAUTIL_Release()

Description:
    This function is inherited from IBASE_Release().


=============================================================================

IMEDIAUTIL_QueryInterface()

Description:
    This function can be used to:
    (1) Get a pointer to an interface or data based on the input class ID.
    (2) Query an extended version of the IMediaUtil-derived class.
    (3) Support version compatibility.

Prototype:
    int IMEDIAUTIL_QueryInterface
    (
    IMediaUtil * pIMediaUtil,
    AEECLSID clsReq,
    void ** ppo
    )

Parameters:
    pIMediaUtil  :  [in] :  Pointer to IMediaUtil Interface.
    clsReq  :  [in] :  A globally unique id to identify the entity (interface or
data) that you are trying to query.
    ppo :  [out] :  Pointer to the interface or data that you want to retrieve.
If the value passed back is NULL, the interface or data that you query are not available.

Return Value:
    Return SUCCESS on success, otherwise returns error code.

Comments:
    If ppo is back a NULL pointer, the interface or data that you query is not available.

Side Effects:
    If an interface is retrieved, then this function increments its reference count.
If a data structure is retrieved, then a pointer to the internal structure is given
and user should not free it.

Version:
    Introduced BREW Client 2.1

See Also:
    None
=============================================================================

IMEDIAUTIL_CreateMedia()

Description:
    Given AEEMediaData, this function analyzes media data and creates an IMedia-based
object (IMedia Interface object).

Prototype:
    int IMEDIAUTIL_CreateMedia
    (
    IMediaUtil * pIMediaUtil,
    AEEMediaData * pmd,
    IMedia ** ppm
    )

Parameters:
    pIMediaUtil  :  [in] :  Pointer to IMediaUtil Interface.
    pmd  :  [in] :  Media data info
    ppm  :  [out] :  IMedia object pointer returned to caller

Return Value:
    SUCCESS: IMedia object successfully created
    EBADPARM: Input parameter(s) is wrong
    EUNSUPPORTED: Class not found
    MM_EBADMEDIADATA: Media data is not correct
    MM_ENOMEDIAMEMORY:No memory to create IMedia object
    ENEEDMORE: Need more data to create IMedia object.
    For MMD_ISOURCE, call IPEEK_Peekable(). (See Comments).
    MM_EPEEK: Error while peeking for the data. (See Comments)
    MM_ENOTENOUGHDATA: Aborted due to insufficient data
    EFAILED: General failure

Comments:
    If pmd->clsData is MMD_FILE_NAME, then:

    (1) The file extension is checked to see if any IMedia class is registered for the
extension.

    (2) If not, the file is opened and file contents are read. Using ISHELL_DetectType(),
the MIME of the media data is determined and IMedia class is found from Shell registry.

    If pmd->clsData is MMD_BUFFER, then:

    (1) The buffer contents are analyzed using ISHELL_DetectType() to determine
the MIME of the media data is determined and IMedia class is found from Shell registry.

    If pmd->clsData is MMD_ISOURCE, then

    (1) The caller needs to set pmd->pData to IPeek *.

    (2) This function peeks for the data to see if enough data is available. The
buffer contents are analyzed using ISHELL_DetectType(), the MIME of the media data
is determined and IMedia class is found from Shell registry.

    (3) If enough data is not there, then it calls IPEEK_Peek() requesting required
number of bytes. If IPEEK_Peek() returns IPEEK_WAIT, then it returns ENEEDMORE.
In response to this, caller needs to call IPEEK_Peekable(). After the data becomes
available, caller should call this function again to create IMedia object.

    If IMedia class is found, then IMedia object is created and media data is set.
This puts IMedia object in ready state.

Version:
    Introduced BREW Client 2.1

See Also:
    ISHELL_DetectType()
    AEEMediaData

=============================================================================

IMEDIAUTIL_EncodeMedia()

Description:
    This function encodes a new media of specified format based on encode specifications
and one or more input media. It also returns the encode results.

Prototype:
    int IMEDIAUTIL_EncodeMedia(IMediaUtil * pIMediaUtil, AEEMediaEncodeResult *
per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb);

Parameters:
    pIMediaUtil :[in]: Pointer to IMediaUtil Interface
    per :[out]: Resulting encoding info
    clsDest :[in]: Destination encode class like AEECLSID_PMD (for PMD), AEECLSID_JPEG
(for JPEG), etc.
    pei :[in]: Input encode specifications
    pcb :[in]: Callback for asynchronous notifications

Return Value:
    SUCCESS: Command accepted
    EBADPARM: Invalid input parameter(s)
    EUNSUPPORTED: Class not found
    ENOMEMORY: Insufficient memory
    EFAILED: General failure

Comments:
    You can cancel the operation by canceling the callback. This ensures that you
won't receive the callback but
    may not ensure that the operation is aborted.
    The 'per' parameter and 'pei->pmdDest' must remain valid throught the transaction.
'per' is
    filled by IMediaUtil and can be examined in the callback function.

Version:
    Introduced BREW Client 2.1

See Also:
    None

=============================================================================

IMEDIAUTIL_EncodePMD()

Description:
    This function encodes a media of the PMD format based on encode specifications and
one or more input media (typically JPEG and QCP). It also returns the encode results.

Prototype:
    int IMEDIAUTIL_EncodePMD
    (
    IMediaUtil * pIMediaUtil,
    AEEMediaEncodeResult * per,
    AEEMediaEncodeInfo * pei,
    AEECallback * pcb);

Parameters:
    pIMediaUtil  :  [in] :  Pointer to IMediaUtil Interface
    per  :  [out] :  Resulting encoding info
    pei  :  [in] :  Input encode specifications
    pcb  :  [in] :  Callback for asynchronous notifications

Return Value:
    SUCCESS: Command accepted
    EBADPARM: Invalid input parameter(s)
    EUNSUPPORTED: Class not found
    ENOMEMORY: Insufficient memory
    EFAILED: General failure

Comments:
    You can cancel the operation by canceling the callback. This ensures that you
won't receive the callback but may not ensure that the operation is aborted.
    The 'per' parameter and 'pei->pmdDest' must remain valid throught the transaction.
'per' is filled by IMediaUtil and can be examined in the callback function.

Version:
    Introduced BREW Client 2.1

See Also:
    AEEMediaEncodeResult
    AEEMediaEncodeInfo

=============================================================================

IMEDIAUTIL_EncodeJPEG()

Description:
    This function encodes a media of JPEG format based on encode specifications
and one or more input media. It also returns the encode results.

Prototype:
    int IMEDIAUTIL_EncodeJPEG
    (
    IMediaUtil * pIMediaUtil,
    AEEMediaEncodeResult * per,
    AEEMediaEncodeInfo * pei,
    AEECallback * pcb
    )

Parameters:
    pIMediaUtil :[in]: Pointer to IMediaUtil Interface
    per :[out]: Resulting encoding info
    pei :[in]: Input encode specifications
    pcb :[in]: Callback for asynchronous notifications

Return Value:
    SUCCESS: Command accepted
    EBADPARM: Invalid input parameter(s)
    EUNSUPPORTED: Class not found
    ENOMEMORY: Insufficient memory
    EFAILED: General failure

Comments:
    You can cancel the operation by canceling the callback. This ensures that you
won't receive the callback but may not ensure that the operation is aborted.
    The 'per' parameter and 'pei->pmdDest' must remain valid throught the transaction.
'per' is filled by IMediaUtil and can be examined in the callback function.

Version:
    Introduced BREW Client 2.1

See Also:
    None

=============================================================================

IMEDIAUTIL_CreateMediaEx()

Description:
   This function provides more flexibility than IMEDIAUTIL_CreateMedia() to
   specify information required to create IMedia-based object.

Prototype:
   int   IMEDIAUTIL_CreateMediaEx(IMediaUtil * po, AEEMediaCreateInfo *pcmi, IMedia ** ppm);

Parameters:
   pIMediaUtil [in]: Pointer to IMediaUtil Interface.
   pcmi [in]: Pointer to structure containg create info
   ppm [out]:        IMedia object pointer returned to caller

Return Value:
    SUCCESS: IMedia object successfully created
    EBADPARM: Input parameter(s) is wrong
    EUNSUPPORTED: Class not found
    MM_EBADMEDIADATA: Media data is not correct
    MM_ENOMEDIAMEMORY:No memory to create IMedia object
    ENEEDMORE: Need more data to create IMedia object.

    For MMD_ISOURCE, call IPEEK_Peekable(). (See Comments).

    MM_EPEEK: Error while peeking for the data. (See Comments)
    MM_ENOTENOUGHDATA: Aborted due to insufficient data
    EFAILED: General failure

Comments:
   Please see the documentation of AEEMediaCreateInfo

   Please see documentation of IMEDIAUTIL_CreateMedia().

Version:
    Introduced BREW Client 3.1.2

See Also:
   AEEMediaCreateInfo
   IMEDIAUTIL_CreateMedia()

============================================================================= */
#endif // AEEMEDIAUTIL_H

#ifndef AEEMEDIADLS_H
#define AEEMEDIADLS_H

/*============================================================================
FILE: AEEMediaDLS.h

SERVICES:  BREW Downloadable Sounds (DLS) Services

DESCRIPTION:
   This file defines an abstract interface for BREW DLS which is used
   in conjunction with IMedia interface.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
//===================================================================
//      Include Files
//===================================================================

#include "AEEMedia.h"

//*********************************************************************
//
// IDLS interface definition
//
//*********************************************************************
//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================
#define AEEIID_MEDIADLS       0x010301ad
#define AEEIID_DLSLINKER      0x010301af

//===================================================================
//    Interface Definition
//===================================================================
#define INHERIT_IDLS(iname)\
   INHERIT_IQueryInterface(iname);\
   int (*Load)(iname *po, AEEMediaData *pmd, AEECallback *pcb, int32 *pnRet);\
   int (*Unload)(iname *po, AEECallback *pcb, int32 *pnRet);\
   int (*IsLoaded)(iname *po, boolean *pbLoaded);\
   int (*IsGlobal)(iname *po, boolean *pbGlobal)

AEEINTERFACE_DEFINE(IDLS);

//===============================================================================
//    API Definition
//===============================================================================
static __inline uint32 IDLS_AddRef(IDLS *me)
{
   return AEEGETPVTBL(me,IDLS)->AddRef(me);
}

static __inline uint32 IDLS_Release(IDLS *me)
{
   return AEEGETPVTBL(me,IDLS)->Release(me);
}

static __inline uint32 IDLS_QueryInterface(IDLS *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IDLS)->QueryInterface(me, cls, ppo);
}

static __inline int IDLS_Load(IDLS *me, AEEMediaData *pmd, AEECallback *pcb, int32 *pnRet)
{
   return AEEGETPVTBL(me,IDLS)->Load(me, pmd, pcb, pnRet);
}

static __inline int IDLS_Unload(IDLS *me, AEECallback *pcb, int32 *pnRet)
{
   return AEEGETPVTBL(me,IDLS)->Unload(me, pcb, pnRet);
}

static __inline int IDLS_IsLoaded(IDLS *me, boolean *pbLoaded)
{
   return AEEGETPVTBL(me,IDLS)->IsLoaded(me, pbLoaded);
}

static __inline int IDLS_IsGlobal(IDLS *me, boolean *pbGlobal)
{
   return AEEGETPVTBL(me,IDLS)->IsGlobal(me, pbGlobal);
}

//*********************************************************************
//
// IDLSLinker interface definition
//
//*********************************************************************
//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================

//===================================================================
//    Interface Definition
//===================================================================
#define INHERIT_IDLSLinker(iname)\
   INHERIT_IQueryInterface(iname);\
   int (*Link)(iname *po, IDLS *pDLS);\
   int (*Unlink)(iname *po, IDLS *pDLS);\
   int (*GetDLSList)(iname *po, IDLS **ppList, int *pnCount)

AEEINTERFACE_DEFINE(IDLSLinker);


//===============================================================================
//    API Definition
//===============================================================================
static __inline uint32 IDLSLinker_AddRef(IDLSLinker *me)
{
   return AEEGETPVTBL(me,IDLSLinker)->AddRef(me);
}

static __inline uint32 IDLSLinker_Release(IDLSLinker *me)
{
   return AEEGETPVTBL(me,IDLSLinker)->Release(me);
}

static __inline uint32 IDLSLinker_QueryInterface(IDLSLinker *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IDLSLinker)->QueryInterface(me, cls, ppo);
}

static __inline int IDLSLinker_Link(IDLSLinker *me, IDLS *pDLS)
{
   return AEEGETPVTBL(me,IDLSLinker)->Link(me, pDLS);
}

static __inline int IDLSLinker_Unlink(IDLSLinker *me, IDLS *pDLS)
{
   return AEEGETPVTBL(me,IDLSLinker)->Unlink(me, pDLS);
}

static __inline int IDLSLinker_UnlinkAll(IDLSLinker *me)
{
   return AEEGETPVTBL(me,IDLSLinker)->Unlink(me, NULL);
}

static __inline int IDLSLinker_GetDLSList(IDLSLinker *me, IDLS **ppList, int *pnCount)
{
   return AEEGETPVTBL(me,IDLSLinker)->GetDLSList(me, ppList, pnCount);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=====================================================================
  INTERFACES DOCUMENTATION
=====================================================================

IDLS Interface

Description: 
   IDLS interface represents one Downloadable Sounds (DLS) media. IDLS enables
   BREW applications to load a DLS content, which can be applied to one or more
   supported media content (like MIDI). IDLSLinker interface allows a DLS
   object to be associated with an IMedia object, which represents a media content.
   One DLS object can be associated with multiple IMedia objects.


The following header files are required:
AEEMediaDLS.h


=============================================================================

IDLS_AddRef()

Description:
    This function is inherited from IBASE_AddRef().

=============================================================================

IDLS_Release()

Description:
    This function is inherited from IBASE_Release().
	
=============================================================================

IDLS_QueryInterface()

Description:
   This function can be used to
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the IDLS-derived class
   (3) support version compatibility

Prototype:
   int IDLS_QueryInterface(IDLS * po, AEECLSID clsReq, void ** ppo);

Parameters:
   po [in]: Pointer to IDLS interface.

   clsReq [in]: A globally unique id to identify the entity (interface or data) that we
                are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.  If the value
             passed back is NULL, the interface or data that we query are not available.

Return Value:
   SUCCESS: on success, ~
   Otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects:
   If an interface is retrieved, then this function increments its reference count.
   If a data structure is retrieved, then a pointer to the internal structure is
   given and user should not free it.

Version:
    Introduced BREW SDK 3.1.4

See Also:
    None
=============================================================================

IDLS_Load()

Description:
    This function loads the DLS media content.
Prototype:
    int IDLS_Load
    (
    IDLS * pIDLS,
    AEEMediaData * pmd,
    AEECallback * pcb,
    int32 * pnRet
    )

Parameters:
    pIDLS  :  Pointer to the IDLS interface object.
    pmd    :  Pointer to the AEEMediaData.
    pcb    :  Pointer to the callback function.
    pnRet  :  Pointer to the return value.  This value needs to be preserved and
              used during the callback.
    
        SUCCESS - The DLS content was loaded succesfully.
        Error code, if otherwise -

Return Value:
   
        SUCCESS       : The DLS load command is accepted.
        EBADPARM      : Bad parm.
        EUNSUPPORTED  : Operation not supported by the class.
        EFAILED       : Operation Failed.
        ENOMEMORY     : Out of application heap.

Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    IDLS_Unload

=============================================================================

IDLS_Unload()

Description:
    This function unloads the DLS media content.
    
Prototype:
    int IDLS_Unload
    (
    IDLS * pIDLS,
    AEECallback * pcb,
    int32 * pnRet
    )

Parameters:
    pIDLS  :  Pointer to the IDLS interface object.
    pcb    :  Pointer to the callback function.
    pnRet  :  Pointer to the return value. This value needs to be preserved and
              used during the callback.

        SUCCESS- The DLS media content was loaded succesfully.
        Error code, if otherwise-

Return Value:
    
        EBADPARM      : Bad parm.
        EUNSUPPORTED  : Operation not supported by the class.
        EFAILED       : Operation Failed.
        ENOMEMORY     : Out of application heap.

Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    IDLS_Load
=============================================================================

IDLS_IsLoaded()

Description:
    This function checks if the DLS content is currently loaded or not.
Prototype:
    int IDLS_IsLoaded
    (
    IDLS * pIDLS,
    boolean * pbLoaded
    )

Parameters:
    pIDLS    :  Pointer to the IDLS interface object.
    pbLoaded :  Pointer to the return value.  
               TRUE - DLS content is loaded.
               FALSE - DLS content is not loaded.

Return Value:
    
        EBADPARM      : Bad parm.
        EUNSUPPORTED  : Operation not supported by the class.
        EFAILED       : Operation Failed.
        ENOMEMORY     : Out of application heap.

Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    None
    
=============================================================================

IDLS_IsGlobal()

Description:
    This function indicates whether linking of DLS to specific media is supported.
    If not supported, then DLS applies globally to all active media.

Prototype:
    int IDLS_IsGlobal
    (
    IDLS * pIDLS,
    boolean * pbGlobal
    )

Parameters:
    pIDLS  :  Pointer to the IDLS interface object.
    pbGlobal :  Pointer to the return value.
        TRUE - Linking of DLS with specific media is not supported. DLS applies globally 
               to all relevant active media.
        FALSE- Linking of DLS with specific media is supported.  

Return Value:

        EBADPARM      : Bad parm.
        EUNSUPPORTED  : Operation not supported by the class.
        EFAILED       : Operation Failed.
        ENOMEMORY     : Out of application heap.
Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    None
    
=============================================================================*/

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=====================================================================
  INTERFACES DOCUMENTATION
=====================================================================

IDLSLinker Interface

Description: 
   IDLSLinker interface enables BREW applications to link DLS content(s) to an IMedia
   object which is representing a media content (like MIDI). IDLSLinker object 
   pointer can be obtained by calling IMEDIA_QueryInterface() with the class ID 
   AEECLSID_MEDIADLSLINKER.

   Please note that the relationship between IDLS and IMedia is many to many,.i.e,
   (1) You can create an IDLS object, load a DLS and link it to multiple IMedia objects
       using corresponding IDLSLinker
   (2) You can link, using IDLSLinker, an IMedia object to multiple IDLS objects


The following header files are required:
AEEMediaDLS.h


=============================================================================

IDLSLinker_AddRef()

Description:
    This function is inherited from IBASE_AddRef().


=============================================================================

IDLSLinker_Release()

Description:
    This function is inherited from IBASE_Release().

=============================================================================

IDLSLinker_QueryInterface()

Description:
   This function can be used to
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the IDLS-derived class
   (3) support version compatibility

Prototype:
   int IDLSLinker_QueryInterface(IDLSLinker * po, AEECLSID clsReq, void ** ppo);

Parameters:
   po [in]: Pointer to IDLSLinker interface.

   clsReq [in]: A globally unique id to identify the entity (interface or data) that we
                are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.  If the value
             passed back is NULL, the interface or data that we query are not available.

Return Value:
   SUCCESS: on success, ~
   Otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects:
   If an interface is retrieved, then this function increments its reference count.
   If a data structure is retrieved, then a pointer to the internal structure is
   given and user should not free it.

Version:
    Introduced BREW SDK 3.1.4

See Also:
    None
=============================================================================

IDLSLinker_Link()

Description:
    This function links the IDLS object that is associated with media and IDLSLinker object.  
    IDLSLinker object pointer can be obtained by calling IMEDIA_QueryInterface() with the class 
    ID AEECLSID_MEDIADLSLINKER.

Prototype:
    int IDLSLinker_Link
    (
    IDLSLinker * pIDLSLinker,
    IDLS * pIDLS
    )

Parameters:
    pIDLSLinker  :  Pointer to the IDLSLinker interface object.
    pIDLS  :  Pointer to the IDLS interface object.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:
    
    EBADPARM      : Bad parm.
    EUNSUPPORTED  : Operation not supported by the class.
    EFAILED       : Operation Failed.

Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    IDLSLinker_Unlink

=============================================================================

IDLSLinker_Unlink()

Description:
    This function unlinks the IDLS object that is associated with Media and IDLSLinker object.
Prototype:
    int IDLSLinker_Unlink
    (
    IDLSLinker * pIDLSLinker,
    IDLS * pIDLS
    )

Parameters:
    pIDLSLinker  :  Pointer to the IDLSLinker interface object.
    pIDLS :  Pointer to the IDLS interface object.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:
    
    EBADPARM      : Bad parm.
    EUNSUPPORTED  : Operation not supported by the class.
    EFAILED       : Operation Failed.
    
Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    IDLSLinker_Link
=============================================================================

IDLSLinker_UnlinkAll()

Description:
    This function unlinks all the current linked IDLS objects with the IDLSLinkerObject.
Prototype:
    int IDLSLinker_UnlinkAll
    (
    IDLSLinker * pIDLSLinker,
    )

Parameters:
    pIDLSLinker  :  Pointer to the IDLSLinker interface object.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:
    

    EBADPARM      : Bad parm
    EUNSUPPORTED  : Operation not supported by the class.
    EFAILED       : Operation Failed.
    
Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    IDLSLinker_Unlink
    
=============================================================================

IDLSLinker_GetDLSList()

Description:
    This function the list of the IDLS objects that are currently linked
    with the IDLS linker object.

Prototype:
    int IDLSLinker_GetDLSList
    (
    IDLSLinker * pIDLSLinker,
    IDLS ** ppList,
    int *pnCount
    )

Parameters:
    pIDLSLinker  :  Pointer to the IDLSLinker Interface object.
    ppList :  Pointer to the list of IDLS objects.
    pnCount : Pointer to the count of the IDLS objects.

    
   Please note on the usage of this function.
   
   (1) First call the IDLSLinker_GetDLSList with *pnCount = initial value or 0 and **ppList = NULL.
   (2) On return of the function the *pnCount contains the number of elements more than the 
      initial value in the list.
   (3) Allocate memory for the *pnCount number of elements.
   (4) Call IDLSLinker_GetDLSList by passing in the pointer to the pointer of the memory region allocated
       in the form of **pList parameter.
    (5) On return of the function call the allocated memory contains the IDLS objects information.   
       

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:
    
    EBADPARM      : Bad parm.
    EUNSUPPORTED  : Operation not supported by the class.
    EFAILED       : Operation Failed.
    
Comments:
    None

Version:
    Introduced BREW SDK 3.1.4

See Also:
    None
    
=============================================================================

============================================================================= */
#endif // AEEMEDIADLS_H

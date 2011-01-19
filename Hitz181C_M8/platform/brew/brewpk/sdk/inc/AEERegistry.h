#ifndef AEEREGISTRY_H
#define AEEREGISTRY_H
/*=====================================================================
FILE:  AEERegistry.h

SERVICES:  BREW Registry Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for BREW Registry

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
Copyright © 1999-2003,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================================*/

#include "AEE.h"

//********************************************************************************************************************************
//
// IRegistry Interface
//
//********************************************************************************************************************************


//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _IRegistry IRegistry;

typedef struct _AEERegItem
{
   AEECLSID       clsType;
   AEECLSID       clsHandler;
   const char *   pszMime;
   flg            bActive:1;     // Class is the default handler...
   flg            bRestricted:1; // Class is not available to app...
} AEERegItem;

typedef boolean (*PFNREGENUM)(void * pUser, AEERegItem * pi);

QINTERFACE(IRegistry)
{
   DECLARE_IBASE(IRegistry)

   AEECLSID    (*GetHandler)(IRegistry * po,AEECLSID clsType, const char * pszMime);
   int         (*SetHandler)(IRegistry * po,AEECLSID clsType, const char * pszMime,AEECLSID cls);
   void        (*Enum)(IRegistry * po,PFNREGENUM pfn, void * pUser, boolean bAll);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IREGISTRY_AddRef(p)                        GET_PVTBL(p,IRegistry)->AddRef((p))
#define IREGISTRY_Release(p)                       GET_PVTBL(p,IRegistry)->Release((p))
#define IREGISTRY_GetHandler(p,ct,psz)             GET_PVTBL(p,IRegistry)->GetHandler((p),(ct),(psz))
#define IREGISTRY_SetHandler(p,ct,psz,ch)          GET_PVTBL(p,IRegistry)->SetHandler((p),(ct),(psz),(ch))
#define IREGISTRY_Enum(p,pfn,pu,b)                 GET_PVTBL(p,IRegistry)->Enum((p),(pfn),(pu),(b))

#endif   // AEEREGISTRY_H


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

PFNREGENUM

Description:
   This is the prototype of the function that can be used to enumerate 
   over the registry.

Definition:
   typedef boolean (*PFNREGENUM)(void * pUser, AEERegItem * pi);

Members:
   None

Comments:
   If the function return FALSE the enumeration is stopped.

Side Effects:
   None

Version:
   2.1

See Also:
   AEERegItem
   IRegistery_Enum

=======================================================================

AEERegItem

Description:
   AEERegItem contains the registry information for each registry item.   

Definition:
   typedef struct _AEERegItem
   {
      AEECLSID       clsType;
      AEECLSID       clsHandler;
      const char *   pszMime;
      flg            bActive:1;     
      flg            bRestricted:1; 
   } AEERegItem;
   
Members:

   clsType : Base class or interface id
   clsHandler : Hanlder class id 
   pszMime : Mime type
   bActive : True if this is the current handler for this type 
   bRestricted : True if the application does not have permissions to
   create this class.

Comments:
   None

Side Effects:
   None

Version:
   2.1

See Also:
   IRegistery_Enum
   PFNREGENUM

=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IRegistry

Description:

The IRegistry is used to get, set, and enumerate handlers.  Currently
the only class that implements the IRegistry interface is 
AEECLSID_REGISTRY.

Handlers are registered when BREW is initialized.  There is no guaranteed 
order of how handlers are registered, except that MIFs will be registered 
after the built-in handlers.  IREGISTRY_GetHandler will always return the 
currently active handler.  If IREGISTRY_Enum is called with bAll set to 
FALSE, then only the active handlers are enumerated.  If bAll is set to 
TRUE, then all the handlers present in MIFs are enumearted as well.

=======================================================================
IREGISTRY_AddRef()

Description:
    This function is inherited from IBASE_AddRef().
	
Comments:
   None

Side Effects:
   None

Version:
   2.1

See Also:
	IBASE_AddRef()

=======================================================================
IREGISTRY_Release()

Description:
    This function is inherited from IBASE_Release().
	
Comments:
   None

Side Effects:
   None

Version:
   2.1

See Also:
	IBASE_Release()

=======================================================================
IREGISTRY_GetHandler

Description:
    This function retrieves the currently active registered handler for
    the base class and mime type.

Prototype:
   AEECLSID IREGISTRY_GetHandler(IRegistry * po,AEECLSID clsType, const char * pszMime);

Parameters:
   po:         Pointer to IRegistry interface.
   clsType:    Base class or interface for the registered handler
   pszMime:    Mime type to be queried.  

Return Value:
   AEECLSID:  Class id of the registered handler, or 0 if no registered 
   handler is found for that mime type and base class.

Comments:
   None

Side Effects:
   None

Version:
   2.1

See Also:
   None

=======================================================================

IREGISTRY_SetHandler

Description:
    This function is used register a handler for the mime type and 
    base class.  If successfull the hanlder becomes the active handler.

Prototype:
   int IREGISTRY_SetHandler(IRegistry * po,AEECLSID clsType, const char * pszMime, AEECLSID cls);

Parameters:
   po:         Pointer to IRegistry interface.
   clsType:    Base class or interface for the handler
   pszMime:    Mime type to be queried.  
   cls:        The class id of the handler to be registered.

Return Value:
   SUCCESS:    If the handler was registered successfully.
   EFAILED:    If unsuccessful.
   EPRIVLEVEL: If the application does not have the priviliges to set
               this handler.
   EBADCLASS:  If the class is unsupported.
   EALREADY:   If the class ia already registered and active.

Comments:
   None

Side Effects:
   None

Version:
   2.1

See Also:
   None

=======================================================================

IREGISTRY_Enum

Description:
    This function is used to enumerate the registry.  

Prototype:
   void IREGISTRY_Enum(IRegistry * po,PFNREGENUM pfn, void * pUser, boolean bAll);

Parameters:
   po:      Pointer to IRegistry interface.
   pfn:     Callback function to be called for each item.
   pUser:   User context pointer for the callback function.
   bAll:    If FALSE only the active handlers are enumerated.  If TRUE
            active handlers plus all the handlers present in MIFs.

Return Value:
	None

Comments:
   This is a synchronous function, which returns when the enumeration 
   is over.  The enumeration is stopped when all the elements have been
   enumerated or PFNREGENUM returns FALSE.

Side Effects:
   None

Version:
   2.1

See Also:
   PFNREGENUM 

=======================================================================*/


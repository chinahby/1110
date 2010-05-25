/*
  ============================================================================

FILE:  OEMRegistry.h

SERVICES:  
       OEM Registry related definitions.

GENERAL DESCRIPTION:

REVISION HISTORY: 

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef OEMREGISTRY_H
#define OEMREGISTRY_H 

#include "AEEShell.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

//********************************************************************************************************************************
//
//          AEE_OEM Interface
//
//********************************************************************************************************************************
/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      AEE-OEM Functions
-------------------------------------------------------------------*/
//
// OEM Type (MIME) Detection based on data in the buffer or name of the object.
//
extern int OEMRegistry_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME);

//
// Enhanced reverse registry query
//

typedef struct
{
   uint32      id;               // Item ID (DLITEMID)
   AECHAR *    pszAppName;       // App name
   int         nNameSize;        // Name size (max to fill)
   uint32      dwInstallTime;    // Install date/time
   uint32      dwLastUsedTime;   // Date/time last used
   AEEAppInfo  ai;               // App Info
} AEEClassInfo;

typedef boolean (*PFNREGCB)(void * pUser, AEECLSID clsType, const char * pszMimeType, AEECLSID clsHandler, AEEClassInfo * pci);

extern int  AEE_GetClassInfo(AEECLSID cls,AEEClassInfo * phi);
extern int  AEE_EnumRegHandlers(AEECLSID clsType, const char * pszMimeType,PFNREGCB pfn, void * pUser, AEEClassInfo * pci);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
OEMRegistry Interface
======================================================================= 

Function: OEMRegistry_DetectType()

Description: 
   Given data in a buffer or the name of an object, this function detects 
   the MIME type. 
   
   This function is typically used to get the handler associated with the data type. 
   For example, if the data represents standard MIDI format, then this 
   function returns the MIME "audio/mid". Using the MIME type, you can query 
   Shell registry to obtain the handler (Class ID) of type AEECLSID_MEDIA.

Prototype:

   int OEMRegistry_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME);

Parameters:
   cpBuf [in]:        Buffer containing the data whose type needs to be determined
   pdwSize [in/out]: On input: Size of data in pBuf, unless pBuf is NULL, then ignored
                     On output: number of additional data bytes needed to perform type detection
   cpszName [in]:    Name of the object whose type needs to be determined (may be null, if unknown).
   pcpszMIME [out]:  MIME string returned to caller, on return, filled with a pointer to a constant string (do not free)

Return Value: 
   SUCCESS:    Data type is detected and MIME is returned
   ENOTYPE:    There is no type associated with this data
   EBADPARM:   Wrong input data (parameter(s))
   ENEEDMORE:  Need more data to perform type detection. *pdwSize contains the the required
               number of additional bytes.
   EUNSUPPORTED: Type detection for the specified input is not supported

Comments:
   pBuf takes precedence over pszName. If both of them are specified, then first pBuf is used for type detection
   followed by pszName. 
   
   If the function returns ENEEDMORE, then *pdwSize is filled with the required additional bytes to carry out the
   operation. Call this function again with (original dwSize + *pdwSize) bytes.

   To determine the maximum number of bytes required to enable type detection, you can call 
      if (ENEEDMORE == ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL))
      {
         // dwReqSize contains the max bytes needed for type detection
      }
  
   IMPORTANT NOTE TO OEMs: 
   (1) Do not modify the existing type detection code
   (2) Add your new type detection functions and you may order them accordingly.

Side Effects: 
   None

See Also:
   ISHELL_DetectType(), ISHELL_GetHandler(), ISHELL_CreateInstance()

=======================================================================

Function: AEE_GetClassInfo()

Description:
   This helper function is provided for OEMs to gain easy access to 
extended information regarding BREW classes.  Although the data can be obtained
through the use of other interfaces (ISHELL_QueryClass and IDOWNLOAD), this
function is provided for more ready access to these parameters.

Prototype:
   int AEE_GetClassInfo(AEECLSID cls, AEEClassInfo * phi)

Parameters:
   None

Return Value:
   0 on SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================
Function: AEE_EnumRegHandlers()

Description:

   This helper function provides OEMs with the ability to:

- Enumerate handlers for a specified type/mime type;
- Enumerate all handlers for a specified handler type;
- Enumerate all handlers for all types;

   Example:
===pre>
   static void EnumTest(Me * pMe)
   {
      AEEClassInfo   ci;
      AECHAR         szName[32];

      MEMSET(&ci,0,sizeof(AEEHandlerInfo));
      ci.pszAppName = szName;
      ci.nNameSize = sizeof(szName);

      AEE_EnumRegHandlers(pMe,AEECLSID_APP,"application/foo",EnumCB,pMe,&ci);
   }

   static boolean EnumCB(void * pcxt,AEECLSID clsType,const char * pszMime,AEECLSID cls,AEEClassInfo * pci)
   {
      Me * pMe = (void *)pcxt;

      AddToMyMenu(pMe,pci);
      return(TRUE);
   }
===/pre>

Prototype:
   int AEE_EnumRegHandlers(AEECLSID clsType,const char * pszMimeType,PFNREGCB pfn,void * pUser,AEEClassInfo * pci)

Parameters:
   clsType:       Type of handlers (example: AEECLSID_APP, AEECLSID_VIEW, etc.)
   pszMimeType:   Mime type or extension
   pfn:           Pointer to callback for each entry
   pUser:         User data pointer for callback
   pci:           Pointer to AEEClassInfo structure to fill for each entry

Return Value:
   0 on SUCCESS

Comments:
   Enumeration will stop if handler returns FALSE

Side Effects:
   None

See Also:
   None

========================================================================*/


#endif /* #ifndef OEMREGISTRY_H */

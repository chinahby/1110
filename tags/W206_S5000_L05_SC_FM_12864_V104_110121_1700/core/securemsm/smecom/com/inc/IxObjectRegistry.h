#ifndef __IXOBJECTREGISTRY_H__
#define __IXOBJECTREGISTRY_H__

/*===========================================================================

                       I X  O B J E C T  R E G I S T R Y

                            H E A D E R  F I L E

FILE:  IxObjectRegistry.h

GENERAL DESCRIPTION

  This header file defines the interface to IxObjectRegistry.

  This interface has methods for registering an object and for
  deregistering an object.

 PUBLIC CLASSES AND STATIC FUNCTIONS:

 INITIALIZATION & SEQUENCING REQUIREMENTS:


  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxObjectRegistry.h#2 $
  $DateTime: 2008/04/07 18:38:18 $
  $Author: rajeevg $

===========================================================================*/


/*===========================================================================

               INCLUDE FILES

============================================================================*/

#include "IxErrno.h"

/*------------------------------------------------------------------------
                              DATA TYPES
------------------------------------------------------------------------*/

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

class IxObjectMap
{
   friend class IxObjectRegistry;

   private:
      
      IxObjectMap ()
      {
         m_handle  = 0;
         m_pObject = 0;
      }

      unsigned int m_handle;
      void        *m_pObject;
};

/* The MAX_OBJECTS constant below is used to prevent runaway registration
** of objects.
*/
#define IX_OBJECT_REGISTRY_MAX_OBJECTS 2048

#define IX_OBJECT_REGISTRY_DEFAULT_MAP_SIZE 32

class IxObjectRegistry
{

/*------------------------------------------------------------------------
                                  PUBLIC METHODS
 ------------------------------------------------------------------------*/

   public:

      /*==============================================================
      FUNCTION:  IxObjectRegistry::Instance()

      DESCRIPTION:
         Returns a reference to the singleton IxObjectRegistry object.

      INPUT/OUTPUT PARAMETERS:

      RETURN VALUE:
         Reference to IxObjectRegistry object.

      SIDE EFFECTS:
      ==============================================================*/
      static IxObjectRegistry* Instance ();

      /*==============================================================
      FUNCTION:  IxObjectRegistry::Register()

      DESCRIPTION:
         Register a specified object. If registration succeeds, a unique
         handle is returned using the pHandle parameter.

      INPUT/OUTPUT PARAMETERS:
         @param  void*         pObject - pointer to the path string
         @param  unsigned int* pHandle - pointer to returned handle

      RETURN VALUE:
         Error Code. E_SUCCESS is returned if there is no error.

      SIDE EFFECTS:
      ==============================================================*/
      IxErrnoType Register (void *pObject, unsigned int *pHandle);

      /*==============================================================
      FUNCTION:  IxObjectRegistry::UnRegister()

      DESCRIPTION:
         Unregister a specified handle.

      INPUT/OUTPUT PARAMETERS:
         @param  unsigned int handle - handle to unregister

      RETURN VALUE:
         Error Code. E_SUCCESS is returned if there is no error.

      SIDE EFFECTS:
      ==============================================================*/
      IxErrnoType UnRegister (unsigned int handle);

      /*==============================================================
      FUNCTION:  IxObjectRegistry::IsRegistered()

      DESCRIPTION:
         Checks if a specified handle is registered or not.

      INPUT/OUTPUT PARAMETERS:
         @param  unsigned int handle - handle to check

      RETURN VALUE:
         TRUE if the specified handle is registered, FALSE otherwise.

      SIDE EFFECTS:
      ==============================================================*/
      bool IsRegistered (unsigned int handle);

      /*==============================================================
      FUNCTION:  IxObjectRegistry::GetObject()

      DESCRIPTION:
         Returns a pointer to an object that matches the handle passed
         in.

      INPUT/OUTPUT PARAMETERS:
         @param  unsigned int handle - handle to check

      RETURN VALUE:
         A pointer to the object whose handle matches the input handle,
         or NULL if there is no match.

      SIDE EFFECTS:
      ==============================================================*/
      void * GetObject (unsigned int handle);

      /*==============================================================
      FUNCTION:  IxObjectRegistry::Clear()

      DESCRIPTION:
         Clears all registry information and frees all allocated memory.

      INPUT/OUTPUT PARAMETERS:

      RETURN VALUE:
         Error Code. E_SUCCESS is returned if there is no error.

      SIDE EFFECTS:
      ==============================================================*/
      IxErrnoType Clear();

   private:

      IxObjectMap *m_pMap;

      unsigned int m_mapSize;

      unsigned int m_nextHandle;

      unsigned int m_nRegisteredObjs;

      /* Prevent direct construction, copy construction and assignment.
      */
      IxObjectRegistry ()
      {
         m_nextHandle      = 1;
         m_pMap            = 0;
         m_mapSize         = 0;
         m_nRegisteredObjs = 0;
      }

      IxObjectRegistry (const IxObjectRegistry &);

      IxObjectRegistry& operator = (const IxObjectRegistry &);

      ~IxObjectRegistry () {}
};

#endif /* __IXOBJECTREGISTRY_H__ */

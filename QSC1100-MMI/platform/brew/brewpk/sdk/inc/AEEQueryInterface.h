/*=============================================================================
Copyright © 2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#ifndef AEEQUERYINTERFACE_H
#define AEEQUERYINTERFACE_H /* #ifndef AEEQUERYINTERFACE_H */

#include "AEEInterface.h"
#include "AEEComdef.h"

//***********************************************************************************************
//
//  IQueryInterface Base Class - This class is exposed as a base
//                                   extension mechanism, this should be added
//                                   to IBase, eventually, prolly
//
//***********************************************************************************************

// ### Deprecated ### - Use INHERIT_IQueryInterface from AEEInterface.h
#define DECLARE_IQIBASE(iname) \
   DECLARE_IBASE(iname)\
   int (*QueryInterface)(iname *, AEECLSID, void **)

#include "../../inc/AEEIQI.h"

#define AEEIID_QUERYINTERFACE AEEIID_IQI
#define INHERIT_IQueryInterface INHERIT_IQI
#define IQueryInterface IQI
#define IQueryInterfaceVtbl IQIVtbl

#define IQI_SELF(p,idReq,pp,idImp) (*(pp)=((((idReq)==(idImp))||((idReq)==AEEIID_IQI))?(p):0))

/*
=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

IQueryInterface Interface

Description:
   IQueryInterface is an alias for the IQI interface.

See Also:
   IQI

=======================================================================
MACROS DOCUMENTATION
=======================================================================

IQI_SELF()

Description:
   This is a helper macro for those implementing an object with a single interface.
===pre>
Prototype:
   IQI_SELF(p,idReq,pp,idImp) (*(pp)=((((idReq)==(idImp))||((idReq)==AEEIID_IQI))?(p):0))

===/pre>
Parameters:
  p: the object

  idReq: the interface ID passed to QueryInterface; the requested interface ID

  pp: output interface pointer

  idImpl: the interface ID of the interface (other than IQI) that
           the object implements

Return Value: 
	p if the object is either idReq or idImpl

Comments:
   Use it like so:
===pre>
   int ISomething_QueryInterface(ISomething *me, AEEIID idReq, void **ppo)
   {
      if (NULL != IQI_SELF(me, idReq, ppo, AEEIID_ISomething)) {
         IQI_AddRef(*ppo);
         return AEE_SUCCESS;
      } else {
         return AEE_ECLASSNOTSUPPORT;
      }
   }

===/pre>
Side Effects:
   ppo is set to p or NULL

See Also:
   IQI_QueryInterface()

=======================================================================
*/
#endif /* #ifndef AEEQUERYINTERFACE_H */

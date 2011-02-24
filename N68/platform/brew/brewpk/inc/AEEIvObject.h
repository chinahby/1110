#ifndef AEEIVOBJECT_H 
#define AEEIVOBJECT_H 
/*==============================================================================

File:
   AEEIvObject.h

Services:
   Object abstract base class used by vCard-vCal properties, IvCalObject and 
   vCard interfaces.

General Description:
   This file contains abstract base class required by, IvProperty and
   IvCard and IvCalObject interfaces.

Public Classes And Static Functions:
   IvObject

INITIALIZATION AND SEQUENCING REQUIREMENTS:

      (c) COPYRIGHT 2007 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/
#include "AEEIQI.h"

#define AEEIID_IvObject 0x010627be

/*=============================================================================
IvOBJECT INTERFACE
==============================================================================*/

#define INHERIT_IvObject(iname)                                              \
   INHERIT_IQI(iname);                                                       \
   int (*GetObjectType)(iname *po, AEECLSID *pnObjType)

AEEINTERFACE_DEFINE(IvObject);

static __inline uint32 IvObject_AddRef(IvObject *pIvObject)
{
   return AEEGETPVTBL(pIvObject,IvObject)->AddRef(pIvObject);
}

static __inline uint32 IvObject_Release(IvObject *pIvObject)
{
   return AEEGETPVTBL(pIvObject,IvObject)->Release(pIvObject);
}


static __inline int IvObject_QueryInterface(IvObject  *pIvObject, 
                                            AEEIID    id,
                                            void      **ppo)
{
   return AEEGETPVTBL(pIvObject,IvObject)->QueryInterface(pIvObject, 
                                                          id,
                                                          ppo);
}


static __inline int IvObject_GetObjectType(IvObject   *pIvObject,
                                           AEECLSID   *pnObjType)
{
   return AEEGETPVTBL(pIvObject, IvObject)->GetObjectType(pIvObject,pnObjType);
}

/*
===============================================================================
      INTERFACE DOCUMENTATION
===============================================================================
IvObject Interface

   IvObject interface represents an abstract base class from which 
   IvProperty, IvParm, IvCard and IvCalObject interfaces would be derived.


   The specifications for vCard and vCalendar supported by the interfaces, are
   here.

   http://www.imc.org/pdi/vcard-21.doc
   http://www.imc.org/pdi/vcal-10.doc
   http://www.ietf.org/rfc/rfc2445.txt
   http://www.ietf.org/rfc/rfc2426.txt


   Interface to parse the data stream containing vCard or vCalendar parameter 
   objects.
   
   IvParm - Interface to  IvParm Parameters object.

Hierarchy :
===pre>
--- Note : Fields in the ( ) are optional ---

(I)
          VCARD DATASTRAEM
                |
                |
      -------------------------------------------------...
      |             |          |           |          |
    vCard1      (vCard2)   (vCard3)    (vCard4)   ..and so on...
                   |
                   |
          --------------------------------------- ...
          |           |          |          |
        Property1   Property2  Property3 ...and so on...
                      |
                      |
        -----------------------------------------------------------
        |           |         |       |             |             |
    (Group Name)  Name     (Parm1)  (Parm2)   ..and so on..     Value
                            |
                        ---------
                        |        |
                      (Name)     Value



(II)

          VCALENDAR DATASTRAEM
                |
                |
      ------------------------------------------...
      |             |          |             |
    vCalendar1 (vCalendar2) (vCalendar3)  ..and so on...
                   |
                   |-------------
                   |            |
                   |       ----------------------------------------------...
                   |       |           |          |           |          |
                   |     (vEvent1)  (vToDo1)   (vToDo2)    (vEvent2)   ..and so on...
                   |         |         |
                   |         |         |
          ------------------------------
          |           |                |
        Property1   Property2  ..and so on...
                      |
                      |
        -----------------------------------------------------------
        |           |         |       |             |             |
    (Group Name)  Name     (Parm1)  (Parm2)   ..and so on..     Value
                     |
                 ---------
                 |        |
              (Name)     Value
 ===/pre>  
===============================================================================

IvObject_AddRef

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvObject_Release()

===============================================================================
IvObject_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
    IvObject_AddRef()

===============================================================================
IvObject_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

================================================================================
IvObject_GetObjectType

Description:
   This function returns the AEECLSID of the current object type

Prototype:
   int IvObject_GetObjectType(IvObject    *pIvObject,
                              AEECLSID    *pnObjType);

Parameters:
   pIvObject   : Pointer to the IvObject interface
   pnObjType   : Upon return, holds the class id of the object

Return Value:
   AEE_SUCCESS   : If we are able to get the class id of the object correctly
   AEE_EBADPARM  : If pnObjType is NULL

Comments:
   None

Side Effects:
   None

================================================================================
*/
#endif //AEEIVOBJECT_H 

#ifndef AEEIVCALOBJECT_H
#define AEEIVCALOBJECT_H
/*==============================================================================
FILE: 
   AEEIvCalObject.h

SERVICES:
   vCalendar Parser related services.

GENERAL DESCRIPTION:
   This interface provides support for vCalendar generic parser/generator for
   vCalendar data streams.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IvCalObject

INITIALIZATION AND SEQUENCING REQUIREMENTS:
      
          COPYRIGHT © 2007 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
==============================================================================*/
#include "AEEIvProperty.h"

#define AEEIID_IvCalObject   0x0105ccc5

#define AEEVCALVERSION_1_0    1
#define AEEVCALVERSION_2_0    2

typedef uint32 AEEVCalVersion;

/*=============================================================================
IvCalObject INTERFACE
==============================================================================*/

#define INHERIT_IvCalObject(iname)                                            \
   INHERIT_IvObject(iname);                                                   \
   int (*AddProperty)(iname *po, IvProperty *pIvProperty);                    \
   int (*RemoveProperty)(iname *po, IvProperty *pIvProperty);                 \
   int (*GetNumberOfProperties)(iname         *po,                            \
                                AEEVProperty  nPropId,                        \
                                int           *pnNumProperties);              \
   int (*GetPropertyAtIndex)(iname           *po,                             \
                             AEEVProperty    nPropId,                         \
                             int             nIndex,                          \
                             IvProperty      **ppIvProperty);                 \
   int (*GetNumberOfObjects)(iname       *po,                                 \
                             AEECLSID    nObjType,                            \
                             int         *pnNumObjects);                      \
   int (*GetObjectAtIndex)(iname        *po,                                  \
                           int          nIndex,                               \
                           AEECLSID   nObjType,                               \
                           IvCalObject  **ppIvCalObject);                     \
   int (*AddObject)(iname *po, IvCalObject *pIvCalObject);                    \
   int (*RemoveObject)(iname *po, IvCalObject *pIvCalObject);                 \
   int (*SetVersion)(iname *po, AEEVCalVersion nCalVersion)


AEEINTERFACE_DEFINE(IvCalObject);

static __inline uint32 IvCalObject_AddRef(IvCalObject *pIvCalObject)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->AddRef(pIvCalObject);
}

static __inline uint32 IvCalObject_Release(IvCalObject *pIvCalObject)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->Release(pIvCalObject);
}

static __inline int IvCalObject_QueryInterface(IvCalObject *pIvCalObject,
                                               AEEIID      iid,
                                               void        **ppo)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->QueryInterface(pIvCalObject,
                                                                 iid,
                                                                 ppo);
}

static __inline int IvCalObject_GetObjectId(IvCalObject *pIvCalObject, 
                                            AEECLSID    *pObjectId)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->GetObjectType(pIvCalObject,
                                                                pObjectId);
}

static __inline int IvCalObject_AddProperty(IvCalObject  *pIvCalObject,
                                            IvProperty   *pIvProperty)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->AddProperty(pIvCalObject,
                                                              pIvProperty);
}

static __inline int IvCalObject_RemoveProperty(IvCalObject *pIvCalObject,
                                               IvProperty  *pIvProperty)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->RemoveProperty(pIvCalObject, 
                                                                 pIvProperty);
}

static __inline int IvCalObject_GetNumberOfProperties(IvCalObject   *pIvCalObject,
                                                      AEEVProperty  nPropId,
                                                      int           *pnNumProps)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->GetNumberOfProperties(pIvCalObject,
                                                                        nPropId,
                                                                        pnNumProps);
}


static __inline int IvCalObject_GetPropertyAtIndex(IvCalObject   *pIvCalObject,
                                                   AEEVProperty  nPropId,
                                                   int           nIndex,
                                                   IvProperty    **ppIvProperty)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->GetPropertyAtIndex(pIvCalObject,
                                                                     nPropId,
                                                                     nIndex,
                                                                     ppIvProperty);
}

static __inline int IvCalObject_GetNumberOfObjects(IvCalObject  *pIvCalObject,
                                                    AEECLSID    nObjType,
                                                    int         *pnNumObjects)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->GetNumberOfObjects(pIvCalObject,
                                                                     nObjType,
                                                                     pnNumObjects);
}

static __inline int IvCalObject_GetObjectAtIndex(IvCalObject  *pIvCalObject,
                                                 int          nIndex,
                                                 AEECLSID     nObjType,
                                                 IvCalObject  **ppIvCalObject)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->GetObjectAtIndex(pIvCalObject,
                                                                   nIndex,
                                                                   nObjType,
                                                                   ppIvCalObject);
}

static __inline int IvCalObject_AddObject(IvCalObject *pIvCalObject,
                                          IvCalObject *pIvCalObj)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->AddObject(pIvCalObject,
                                                            pIvCalObj);
}

static __inline int IvCalObject_RemoveObject(IvCalObject *pIvCalObject,
                                             IvCalObject *pIvCalObj)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->RemoveObject(pIvCalObject,
                                                               pIvCalObj);
}

static __inline int IvCalObject_SetVersion(IvCalObject      *pIvCalObject,
                                           AEEVCalVersion   nCalVersion)
{
   return AEEGETPVTBL(pIvCalObject, IvCalObject)->SetVersion(pIvCalObject,
                                                             nCalVersion);
}


static __inline IvObject *IvCalObject_To_IvObject(IvCalObject *pIvCalObject) 
{
   return (IvObject*) (void *) pIvCalObject;
}

/*
============================================================================
 
                Data Structure Documentation
============================================================================
AEEVCalVersion

Description:
   Specifies the calendar version

Definition:
   
   #define AEEVCALVERSION_1_0    1
   #define AEEVCALVERSION_2_0    2

   typedef uint32 AEEVCalVersion;

Members:

   AEEVCALVERSION_1_0   :  The given calendar is of 1.0 version (vCal)
   AEEVCALVERSION_2_0   :  The given calendar is of 2.0 version (iCal)

Comments:
   Every calendar object either vCal or iCal must include the version info,
   if not set we don't parse the calendar object, just return a failure

See Also:
   None

============================================================================
            INTERFACE DOCUMENTATION
============================================================================
IvCalObject Interface
   
   IvCalObject interface provides support for parsing/generating calendar
   items. The versions that are supported are, 2.0(iCalendar, rfc2445) and 
   version 1.0(vCalendar, published by versit consortium). The specs can
   be found at,

   http://www.imc.org/pdi/vcal-10.doc 
   
   http://www.ietf.org/rfc/rfc2445.txt

   It is recommended to set the version property when applications create an
   IvCalobject, irrespective of the component type, it would help in
   validating the attached properties.

   If we have a simple buffer as follows,
   ===pre>
      
      BEGIN:VCALENDAR
      PRODID:-//Foo Bar//EN
      VERSION:2.0
      METHOD:PUBLISH
      BEGIN:VEVENT
      ORGANIZER:MAILTO:anyone@anywhere.com
      DTSTART:20071003T063000Z
      DTEND:20071003T070000Z
      SEQUENCE:0
      UID:040000008200E00074C5B7101A82E00800000000100C47396304C8010000000000000
       00010000000430E90713A99A345BD3C8FF5EBBFC655
      DTSTAMP:20071003T090837Z
      DESCRIPTION:1. Test the IvCalObject interface
      SUMMARY:Tasks
      PRIORITY:5
      CLASS:PUBLIC
      BEGIN:VALARM
      TRIGGER:-PT15M
      ACTION:DISPLAY
      DESCRIPTION:Reminder
      END:VALARM
      END:VEVENT
      END:VCALENDAR
===/pre>
   Once parsing is done we get a IvCalObject interface pIvCalObject, the
   various operations he can perform are,

   Note: For our use case we assume we always get the return value are
         AEE_SUCCESS and pointers passed in or allocated are valid
   

===pre>
   void Sample_vCalObject_Usage1(IvCalObject *pIvCalObject)
   {
      int nResult = AEE_SUCCESS;
      int nNumObjs = 0;
      AEECLSID nObjecId = 0;
      IvCalObject *pIvCalEventObject = NULL;
      IvCalObject *pIvCalAlarmObject = NULL;
      int nNumProps = 0;

      nResult = IvCalObject_GetObjectId(pIvCalObject, &nObjecId);
      //on return holds nObjecId=AEECLSID_vCalendar
      
      nResult = IvCalObject_GetNumberOfObjects(pIvCalObject,AEECLSID_vEvent,&nNumObjs);
      //on return nNumObjs=1

      nResult = IvCalObject_GetObjectAtIndex(pIvCalObject,
                                             0,
                                             AEECLSID_vEvent,
                                             &pIvCalEventObject);
      //on return holds the pointer to event IvCalObject

      nResult = IvCalObject_GetObjectId(pIvCalEventObject, &nObjecId);
      //on return nObjecId=AEECLSID_vEvent

      nResult = IvCalObject_GetNumberOfObjects(pIvCalEventObject,
                                               AEECLSID_vAlarm,
                                               &nNumObjs);
      //on return nNumObjs=1

      nResult = IvCalObject_GetObjectAtIndex(pIvCalEventObject,
                                             0,
                                             AEECLSID_vEvent,
                                             &pIvCalAlarmObject);
      //on return pIvCalAlarmObject points to vAlarm object of vEvent

      nResult = IvCalObject_GetObjectId(pIvCalAlarmObject,&nObjecId);
      //on return nObjecId=AEECLSID_vAlarm

      nResult = IvCalObject_GetNumberOfProperties(pIvCalAlarmObject,
                                                  0,
                                                  &nNumProps);
      //on return nNumProps=3

      RELEASEIF(pIvCalEventObject);
      RELEASEIF(pIvCalAlarmObject);

   }   
===/pre>

============================================================================
IvCalObject_AddRef

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvCalObject_Release()

============================================================================
IvCalObject_Release

Description:
   This function is inherited from IQI_Release().

See Also:
   IvCalObject_AddRef()

============================================================================
IvCalObject_QueryInterface

Description:
   This function is inherited from IQI_QueryInterface(). 

============================================================================
IvCalObject_GetObjectId

Description:
   This functions returns the current object AEECLSID

Prototype:
   int IvCalObject_GetObjectId(IvCalObject *pIvCalObject,
                               AEECLSID    *pObjectId); 

Parameters:
   pIvCalObject  :  Pointer to the IvCalObject interface
   pObjectId     :  The object Id.

Return Value:
   AEE_SUCCESS  : If the interface was able to process the buffer     
   AEE_EBADPARM : If pObjectId is NULL

Comments:
   None

Side Effects:
   None

============================================================================
IvCalObject_AddProperty

Description:
   Adds a property to IvCalObject

Prototype:
   int IvCalObject_AddProperty(IvCalObject  *pIvCalObeject,
                               IvProperty   *pIvProperty);
Parameters:
   pIvCalObeject  :  Interface pointer
   pIvProperty    :  The property that has to be added

Return Value:
   AEE_SUCCESS   :  If we are successful in adding the property
   AEE_EBADPARM  :  If pIvProperty is invalid
   AEE_EFAILED   :  Otherwise

Comments:
  The application should set first the VERSION property on the component
  to better validate the grammar. The API returns EBADPARM in case the property
  specified is invalid. For e.g. DTSTART can't occur more than once if 
  user tries to again insert DTSTART we return EBADPARM.

Side Effects:
   The added property reference count is incremented

============================================================================
IvCalObject_RemoveProperty 

Description:
   Removes the given IvProperty object

Prototype:
   int IvCalObject_RemoveProperty(IvCalObject *pIvCalObject,
                                  IvProperty  *pIvProperty);

Parameters:
   pIvCalObject :  Pointer to the interface
   pIvProperty  :  Property that has to be removed

Return Value:
   AEE_SUCCESS   :  If we are successful in removing the property
   AEE_EBADPARM  :  If pIvProperty is invalid

Comments:
  None

Side Effects:
   The IvProperty object's reference count is decremented and removed
   from the object's list of properties

============================================================================
IvCalObject_GetNumberOfProperties

Description:
   Returns the number of properties 

   Passing nPropId=0, will tell total number of properties assosciated with
   this object

Prototype:
   int IvCalObject_GetNumberOfProperties(IvCalObject   *pIvCalObject,
                                         AEEVProperty  nPropId,
                                         int           *pnNumProps);

Parameters:
   pIvCalObject   :  Pointer to the interface object
   nPropId        :  Property Id, for which we want to check the number
                     of occurrences
   pnNumProps     :  Upon successful return contains the number of
                     properties for that particular property id

Return Value:
   Returns the AEE return value

   AEE_SUCCESS    :  If we are able to successfully return the count
   AEE_EBADPARM   :  If either nPropId is invalid or pnNumProps is NULL

Comments:
   In calendar we may have some properties repeat more than once. This
   function is useful in knowing such number of properties
   
   ATTENDEE;CN="anybody,anyone";ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:anybody@anywhere.com
   
   ATTENDEE;CN="nobody,noone";ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:nobody@anywhere.com
   
   ATTENDEE;CN="someone,somewhere";ROLE=REQ-PARTICIPANT;RSVP=TRUE:MAILTO:someone@anywhere.com

   For the above example, if the user queries with the AEEVPROPERTY_ATTENDEE
   upon return pnNumProps would be 3

Side Effects:
   None

============================================================================
IvCalObject_GetPropertyAtIndex

Description:
   Get the IvProperty object at the specified index

   Passing nPropId=0, will retrieve the IvProperty at the given index

Prototype:
   int IvCalObject_GetPropertyAtIndex(IvCalObject     *pIvCalObject,
                                      AEEVProperty    nPropId,
                                      int             nIndex,
                                      IvProperty      **ppIvProperty);
                                   
Parameters:
   pIvCalObject   :  Pointer to the interface object
   nPropId        :  Property Id, for which we want to retrieve the object
   nIndex         :  Index at which the value has to be retirieved, the given
                     nIndex is zero based
   ppIvProperty   :  Pointer to the Property object, where the returned object
                     would be stored

Return Value:
   AEE_SUCCESS   :  If we are SUCCESS in retrieving the object
   AEE_EBADPARM  :  If either nPropId or nIndex is invalid or ppIvProperty is
                    NULL
   AEE_EFAILED   :  Otherwise

Commets:
   If there are three properties for ATTENDEE, if the nIndex is 1 then he
   would get the 2nd IvProperty object associated with property. The nIndex
   will be a zero based index.

See Also:
   IvCalObject_GetNumPropertiesById, IvCalObject_GetNumPropertiesByName

Side Effects:
   The returned property object reference count is incremented

============================================================================
IvCalObject_GetNumberOfObjects

Description:
   Returns the specified number of objects attached to this IvCalObj for the
   specified object id

Prototype:
   int  IvCalObject_GetNumberOfObjects(IvCalObject *pIvCalObject,
                                       AEECLSID    ObjId,
                                       int         *pnNumObjects);

Parameters:
   pIvCalObject  :  Pointer to the interface object
   ObjId         :  The Object AEECLSID which user wants to know
   pnNumObjects  :  Contains the number of objects

Return value:
   AEE_SUCCESS  :  If we can return the count correctly
   AEE_EBADPARM :  If pnNumObjects is NULL
   AEE_EFAILED  :  Otherwise

Comments:
   None

Side Effects:
   None

============================================================================
IvCalObject_GetObjectAtIndex

Description:
   Returns the IvCalObj at specified index for that class id

Prototype:
   int IvCalObject_GetObjectAtIndex(IvCalObject  *pIvCalObject,
                                    int          nIndex,
                                    AEECLSID     ObjId,
                                    IvCalObject  **ppIvCalObject);

Parameters:
   pIvCalObject   :  Pointer to the interface object
   nIndex         :  Index at which the object has to be retrieved from, the
                     nIndex is zero based
   ObjId          :  The Object Id that has to be retrieved
   ppIvCalObject  :  To hold the returned object

Return Value:
   AEE_SUCCESS   :  If we are successful in retrieving the object
   AEE_EBADPARM  :  If either nIndex or ppIvCalObject is NULL
   AEE_EFAILED   :  Otherwise

Comments:
   None

Side Effects:
   The returned object reference count is incremented

============================================================================
IvCalObject_AddObject

Description:
   Adds the passed in Object

Prototype:
   int IvCalObject_AddObject(IvCalObject  *pIvCalObject,
                             IvCalObject  *pIvCalObj);

Parameters:
   pIvCalObject   :  The Interface pointer
   pIvCalObj      :  Object that has to be added

Return Value:
   AEE_SUCCESS       :  If we are successful in adding the object
   AEE_EBADPARM      :  If pIvCalObj is NULL
   AEE_ENOTALLOWED   :  If the you can't add this type, pIvCalObj to the
                        current pIvCalObject. User can't add vEvent type
                        to vToDo type.
   AEE_EFAILED       :  Otherwise

Comments:
   None

Side Effects:
   The added object's reference count is incremented

============================================================================
IvCalObject_RemoveObject 

Description:
   Removes the given object

Prototype:
   int IvCalObject_RemoveObject(IvCalObject *pIvCalObject,
                                IvCalObject *pIvCalObj);

Parameters:
   pIvCalObject   :  Pointer to the interface
   pIvCalObj      :  Object that has to be removed

Return Value:
   AEE_SUCCESS    :  If we are successful in adding the object
   AEE_EBADPARM   :  If pIvCalObj is NULL
   AEE_EFAILED    :  Otherwise

Comments:
  None

Side Effects:
   The removed object's reference count is decremented and removed from the
   list of objects assosciated with this IvCalObject

============================================================================
IvCalObject_SetVersion

Description:
   Set's the version of the object
   
Prototype:
  int IvCalObject_SetVersion(IvCalObject      *pIvCalObject,
                             AEEVCalVersion   nCalVersion);
   
Parameters:
   pIvCalObject   :  Pointer to the interface object
   nCalVersion    :  Version to be set

Comments:
   When a component is created, app should set the version property on that
   component to better validate the properties being added.

   The default version would be set to version AEEVCALVERSION_2_0
   
Side Effects:
   None

============================================================================
 */

#endif //AEEIVCALOBJECT_H


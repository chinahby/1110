#ifndef AEEIVCALSTORE_H
#define AEEIVCALSTORE_H
/*==============================================================================

File: 
   AEEIvCalStore.h

Services:     
   Provides services for storing and retrieving Events/Todo's/Journal/TimeZone
   component items of a vCalendar or iCalendar

Description:  
   IvCalStore Interface General Definitions.

Public Classes And Static Functions:
   IvCalStore

INITIALIZATION AND SEQUENCING REQUIREMENTS:

      Copyright © 2007 QUALCOMM Incorporated.
             All Rights Reserved.
          QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "AEEIvCalObject.h"
#include "AEEISignal.h"

#define AEEIID_IvCalStore      0x0105f4ad

/*==============================================================================
 IvCalStore Interface
==============================================================================*/

#define INHERIT_IvCalStore(iname)                                           \
   INHERIT_IQI(iname);                                                      \
   int (*AddObject)(iname        *po,                                       \
                    IvCalObject  *pIvCalObject,                             \
                    AEEVProperty *pnAEEPropertyOID);                        \
   int (*UpdateObject)(iname        *po,                                    \
                       IvCalObject  *pIvCalObject);                         \
   int (*GetObjectByOID)(iname           *po,                               \
                         AEEVProperty    nAEEPropertyOID,                   \
                         IvCalObject     **ppIvCalObject);                  \
   int (*DeleteObjectByOID)(iname         *po,                              \
                            AEEVProperty  nAEEPropertyOID);                 \
   int (*GetObjectOIDs)(iname              *po,                             \
                        AEECLSID           ObjId,                           \
                        AEEVProperty       *pnAEEPropertyOIDs,              \
                        int                nSize,                           \
                        int                *pnReqSize);                     \
   int (*GetObjectOIDByUID)(iname         *po,                              \
                            char          *pszUID,                          \
                            AEEVProperty  *pnAEEVPropertyOID);              \
   int (*LockCalendar)(iname*   po);                                        \
   int (*UnlockCalendar)(iname  *po);                                       \
   int (*IsCalendarLocked)(iname *po, boolean *bIsLocked);                  \
   int (*OnAdd)(iname *po, ISignal *pISignal);                              \
   int (*OnUpdate)(iname *po, ISignal *pISignal);                           \
   int (*OnDelete)(iname *po, ISignal *pISignal);                           \
   int (*OnLock)(iname *po, ISignal *pISignal)


AEEINTERFACE_DEFINE(IvCalStore);

static __inline uint32 IvCalStore_AddRef(IvCalStore *pIvCalStore)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->AddRef(pIvCalStore);
}

static __inline uint32 IvCalStore_Release(IvCalStore *pIvCalStore)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->Release(pIvCalStore);
}

static __inline int IvCalStore_QueryInterface(IvCalStore *pIvCalStore, 
                                              AEEIID     iid,
                                              void       **ppo)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->QueryInterface(pIvCalStore,
                                                              iid,
                                                              ppo);
}

static __inline int IvCalStore_AddObject(IvCalStore    *pIvCalStore,
                                         IvCalObject   *pIvCalObject,
                                         AEEVProperty  *pnAEEPropertyOID)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->AddObject(pIvCalStore,
                                                         pIvCalObject,
                                                         pnAEEPropertyOID);

}

static __inline int IvCalStore_UpdateObject(IvCalStore   *pIvCalStore,                                             
                                            IvCalObject  *pObject)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->UpdateObject(pIvCalStore,                                                                                                                                             pObject);
}

static __inline int IvCalStore_GetObjectByOID(IvCalStore    *pIvCalStore,
                                              AEEVProperty  nAEEPropertyOID,                                           
                                              IvCalObject   **ppObj)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->GetObjectByOID(pIvCalStore,
                                                              nAEEPropertyOID,
                                                              ppObj);
}

static __inline int IvCalStore_DeleteObjectByOID(IvCalStore    *pIvCalStore,                                                 
                                                 AEEVProperty  nAEEPropertyOID)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->DeleteObjectByOID(pIvCalStore,
                                                                 nAEEPropertyOID);
}


static __inline int IvCalStore_GetObjectOIDs(IvCalStore      *pIvCalStore, 
                                             AEECLSID        ObjId,                                              
                                             AEEVProperty    *pnAEEPropertyOIDs,                                             
                                             int             nSize,
                                             int             *pnReqSize)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->GetObjectOIDs(pIvCalStore, 
                                                             ObjId, 
                                                             pnAEEPropertyOIDs,                                                             
                                                             nSize,
                                                             pnReqSize);
}

static __inline int IvCalStore_GetObjectOIDByUID(IvCalStore    *pIvCalStore, 
                                                 char          *pszUID,
                                                 AEEVProperty  *pnAEEVPropertyOID)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->GetObjectOIDByUID(pIvCalStore, 
                                                                 pszUID,
                                                                 pnAEEVPropertyOID);
}

static __inline int IvCalStore_LockCalendar(IvCalStore  *pIvCalStore)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->LockCalendar(pIvCalStore);
}

static __inline int IvCalStore_UnlockCalendar(IvCalStore    *pIvCalStore)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->UnlockCalendar(pIvCalStore);
}

static __inline int IvCalStore_IsCalendarLocked(IvCalStore    *pIvCalStore,
                                                boolean       *bIsLocked)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->IsCalendarLocked(pIvCalStore,
                                                                bIsLocked);
}

static __inline int IvCalStore_OnAdd(IvCalStore    *pIvCalStore,
                                     ISignal       *pISignal)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->OnAdd(pIvCalStore,
                                                     pISignal);
}
static __inline int IvCalStore_OnUpdate(IvCalStore    *pIvCalStore,
                                        ISignal       *pISignal)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->OnUpdate(pIvCalStore,
                                                        pISignal);
}

static __inline int IvCalStore_OnDelete(IvCalStore    *pIvCalStore,
                                        ISignal       *pISignal)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->OnDelete(pIvCalStore,
                                                        pISignal);
}

static __inline int IvCalStore_OnLock(IvCalStore    *pIvCalStore,
                                      ISignal       *pISignal)
{
   return AEEGETPVTBL(pIvCalStore,IvCalStore)->OnLock(pIvCalStore,
                                                      pISignal);
}

/*===========================================================================
  INTERFACES DOCUMENTATION
=============================================================================
IvCalStore Interface

Description:   
   This interface provides the ability to interact with the vCalStore data 
   store consisting of IvCalObjects

   This interface offers the capability to:   
   -    Create new / Update an Event/vTodo/vJournal in the calendar 
        data store.
   -    Delete Event/vTodo/vJournal based by Object ID in calendar 
        data store.
   -    Get Event/vTodo/vJournal IDs.   

   The IvCalStore distinguishes between two Identifiers 
   
   1. OID or Object ID :- This ID is used by IvCalStore to identify a full 
      component (Event, Todo, Journal etc) record or data in the store.
      Users can get the whole component with the help of this Object ID.

   2. UID or Unique ID :- This is specific to the vCalendar. A vCalendar 
      component might contain a UID property. This UID specifies to the
      vCalendar UID property.

Comments:
   None

Side Effects:
   None
=============================================================================
IvCalStore_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvCalStore_Release()

===============================================================================
IvCalStore_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IvCalStore_AddRef()

===============================================================================
IvCalStore_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================
IvCalStore_AddObject

Description:
   This function creates/adds a Event,Todo,Journal component record 
   in the IvCalStore data store of type specified in the pObject.
   An Event, Todo etc may be associated with some time zone. This time zone
   object should be attached to the Event, Todo object by user so that the API 
   can store time zone information corresponding to an Event. If the user 
   doesn't attach the time zone information along with event, the timezone
   information would not be stored.
   The record number or Object ID (OID) would be returned back to the user.   

Prototype:
   int IvCalStore_AddObject(IvCalStore   *pIvCalStore,                            
                            IvCalObject  *pObject,
                            AEEVProperty *pnAEEPropertyOID);

Parameters:
   pIvCalStore      : [in]  The IvCalStore Interface pointer.    
   pObject          : [in]  Pointer to an object to be saved.
   pnAEEPropertyOID : [out] The returned OID of record added.
  

Return Value:
   AEE_SUCCESS    :  If the object stored successfully.
   AEE_EBADPARM   :  If invalid parameters are passed.
   AEE_ENOMEMORY  :  If there is not enough memory available
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  Otherwise.

Comments:
   App can add an event, todo, journal component object to calendar
   store. If app receives an entire calendar object, it has to get the 
   individual vEvent, vToDo , vJournal, vTimeZone objects from the calendar
   object and add them one by one using this API.
   
   There can be a time zone associated with a specific event. App has to 
   attach the time zone component to that event object and provide it
   to calendar store. If the event has a time zone component attached ,
   store would store timezone otherwise not.
   
   AEEPRIVID_vCalStoreWrite privilege is required to Add some calendar 
   information in calendar store.

    
Side Effects:
   None
=============================================================================
IvCalStore_UpdateObject

Description:
   This function Updates a calendar Record in the IvCalStore data store of 
   type specified in the pObject.     
    
Prototype:
   int IvCalStore_UpdateObject(IvCalStore *pIvCalStore,IvCalObject *pObject);

Parameters:
   pIvCalStore : [in] The IvCalStore Interface pointer.   
   pObject     : [in] Pointer to an object to be updated.
  

Return Value:
   AEE_SUCCESS    :  If the object is updated successfully.
   AEE_EBADPARM   :  If invalid parameters are passed.
   AEE_ENOMEMORY  :  If there is not enough memory available to do 
                     the operation.
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  Otherwise.

Comments:
   In case the user wants to add new properties or components and update then
   the object pObject should contain all the properties and components which
   were already present plus the new properties and components. 
  
   Apps can register signals with IvCalStore_OnUpdate() API to receive 
   notifications for any updates in the database. 
   App using this API needs to add the OID property to the object 
   being updated. 
   
   AEEPRIVID_vCalStoreWrite privilege is required to Update some calendar 
   information in calendar store.

See Also:
   IvCalStore_OnUpdate

=============================================================================
IvCalStore_DeleteObjectByOID

Description:
   This function deletes an object from the calendar data store based on the 
   passed calendar record Object ID.

   This Object ID is different from the UID in a calendar component. OID 
   represents the database object ID for an event, todo, journal etc.

Prototype:
   int IvCalStore_DeleteObjectByOID(IvCalStore    *pIvCalStore,                                                 
                                    AEEVProperty  nAEEPropertyOID)

Parameters:
   pIvCalStore     : [in] The IvCalStore Interface pointer. 
   nAEEPropertyOID : [in] The IvCalStore ObjectId to be deleted.

Return Value:
   AEE_SUCCESS    :  If deletion of a record is successful.
   AEE_EBADPARM   :  If invalid parameters are passed.
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  Otherwise.

Comments:
   Apps can register signals with IvCalStore_OnDelete() API to receive 
   notifications for any deletions from the database. 
   AEEPRIVID_vCalStoreWrite privilege is required to Delete some calendar 
   information in calendar store.
 
See Also:
   IvCalStore_OnDelete

=============================================================================
IvCalStore_GetObjectByOID

Description:
   This function retrieves the Event, Todo, Journal object given its OID. 
   An event may have a time zone information. While adding/storing the event
   if the time zone component object is attached to the event it would be 
   stored and retrieved back by this API.

Prototype:
   int IvCalStore_GetObjectByOID(IvCalStore     *pIvCalStore,
                                 AEEVProperty   nAEEPropertyOID,                               
                                 IvCalObject    **ppObj);

Parameters:
   pIvCalStore      : [in] The IvCalStore Interface pointer.
   nAEEPropertyOID  : [in] The IvCalStore Object ID to be retrieved.  
   ppObj            : [out]Holds pointer to the returned object

Return Value:
   AEE_SUCCESS    :  If the object is retrieved
   AEE_EBADPARM   :  If invalid parameters are passed.
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  Otherwise.

Comments:
   AEEPRIVID_vCalStoreRead privilege is required to Read/Get some calendar 
   information in calendar store.

Side Effects:
   None

=============================================================================
IvCalStore_GetObjectOIDs

Description:
   This function gets all the OIDs for a particular component type.
   If the component type is specified as 0 then all the OIDs in the
   calendar database would be returned back.

Prototype:
   int IvCalStore_GetObjectOIDs(IvCalStore        *pIvCalStore, 
                                AEECLSID          ObjId, 
                                AEEVProperty      *pnAEEPropertyOIDs,
                                int               nSize,
                                int               *pnReqSize);

Parameters:
   pIvCalStore        : [in]     Pointer to the IvCalStore Interface object
   ObjId              : [in]     Object type
   pnAEEPropertyOIDs  : [in/out] List of OIDs returned
   nSize              : [in]     Size of the list (number of elements)
   pnReqSize          : [in/out] On return will contain the required number
                                 of elements (OIDs) 

Return Value:
   AEE_SUCCESS    :  If the OIDs are retrieved successfully.
   AEE_EBADPARM   :  If invalid parameters are passed.
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  Otherwise.


Comments:
   In Order to get all OIDs in the DB, specify ObjId as 0.

   This API would return the number of elements (OIDs) to be returned
   if pnAEEPropertyOIDs is NULL and not the size in bytes.   
   User would first call the API with pnAEEPropertyOIDs as NULL. This 
   would provide the number of elements in pnReqSize. 
   Suppose the number of elements returned are 3, then user would 
   allocate memory for 3 uint32 values and again call the API with
   pnAEEPropertyOIDs. User would call the API second time with allocated
   memory and nSize would specify the number of OIDs (same as returned
   from the first call) to be filled in pnAEEPropertyOIDs.
   AEEPRIVID_vCalStoreRead privilege is required to get the OIDs
   information from calendar store.

Side Effects:
  None

=============================================================================
IvCalStore_GetObjectOIDByUID

Description:
   This function gets the OID for a particular component type 
   according to the specified UID string.

Prototype:
   int IvCalStore_GetObjectOIDByUID(IvCalStore    *pIvCalStore, 
                                    char          *pszUID, 
                                    AEEVProperty  *pnAEEPropertyOID)

Parameters: 
   pIvCalStore      : [in] Pointer to the IvCalStore Interface object.
   pszUID           : [in] Character string for UID in a calendar.
   pnAEEPropertyOID : [out]Object ID for this UID.   

Return Value:
   AEE_SUCCESS    :  If the OID is retrieved successfully.
   AEE_EBADPARM   :  If either pszUID is NULL
   AEE_ENOTALLOWED:  If database is locked.
   AEE_EPRIVLEVEL :  If no proper privileges.
   AEE_EFAILED    :  If we fail to retrieve 

Comments:
   AEEPRIVID_vCalStoreRead privilege is required to get the OID
   information from calendar store.

Side Effects:
   None

=============================================================================
IvCalStore_LockCalendar

Description:
   This function is used to Lock the calendar. 

Prototype:
   int IvCalStore_LockCalendar(IvCalStore  *pIvCalStore);

Parameters:
   pIvCalStore  : [in] Pointer to the IvCalStore Interface object.   

Return Value:
   AEE_SUCCESS    :  If the calendar is locked successfully.  
   AEE_EPRIVLEVEL :  If the app doesn't have enough privileges to lock

Comments:
   Only an app with privilege level AEEPRIVID_vCalStoreLock would be able
   to lock the calendar. Once the calendar is locked no other app with 
   less priv level would be able to access the calendar database.

See Also:
   IvCalStore_UnlockCalendar

=============================================================================
IvCalStore_UnlockCalendar

Description:
   This function is used to Unlock the calendar.

Prototype:
   int IvCalStore_UnlockCalendar(IvCalStore  *pIvCalStore);

Parameters:
   pIvCalStore  : [in] Pointer to the IvCalStore Interface object.   

Return Value:
   AEE_SUCCESS    :  If the calendar is unlocked successfully.  
   AEE_EPRIVLEVEL :  If the app don't have enough privileges to unlock

Comments:
   Only an app with privilege level AEEPRIVID_vCalStoreLock would be able
   to unlock the calendar.

See Also:
   IvCalStore_LockCalendar

=============================================================================
IvCalStore_IsCalendarLocked

Description:
   This function is used to query whether the calendar is locked or unlocked

Prototype:
   int IvCalStore_IsCalendarLocked(IvCalStore    *pIvCalStore,
                                   boolean       *bIsLocked);

Parameters:
   pIvCalStore  : [in] Pointer to the IvCalStore Interface object.   
   bIsLocked    : [out]TRUE if calendar locked, FALSE if not locked.

Return Value:
   AEE_SUCCESS    :  If we can retrieve the info
   AEE_EBADPARM   :  If bIsLocked is NULL

Comments:
   None

Side Effects:
   None

===============================================================================
IvCalStore_OnAdd

Description:
   This function is used to get notifications for Add events on the
   calendar store

Prototype:
   int IvCalStore_OnAdd(IvCalStore *pIvCalStore,ISignal *pISignal);

Parameters:
   pIvCalStore  : [in] Pointer to the IvCalStore Interface object.   
   pISignal     : [in] Pointer to the ISignal object that's set when a new 
                       object has been added in the database

Return Value:
   AEE_SUCCESS    :  If the signal for Add is added successfully.  
   AEE_EBADPARM   :  If pISignal is NULL

Comments:
   Apps would register ISignal through this API to get notifications
   for any additions in the calendar database.

Side Effects:
   None

===============================================================================
IvCalStore_OnUpdate

Description:
   This function is used to get notifications for Update events on the
   calendar store

Prototype:
   int IvCalStore_OnUpdate(IvCalStore *pIvCalStore,ISignal *pISignal);

Parameters:
   pIvCalStore  : [in] Pointer to the IvCalStore Interface object.   
   pISignal     : [in] Pointer to the ISignal object that's set when any object
                       in the database is updated

Return Value:
   AEE_SUCCESS    :  If the signal for Update is added successfully.  
   AEE_EBADPARM   :  If pISignal is NULL

Comments:
   Apps would register ISignal through this API to get notifications
   for any updates in the calendar database.

Side Effects:
   None

===============================================================================
IvCalStore_OnDelete

Description:
   This function is used to get notifications for delete events on the
   calendar store

Prototype:
   int IvCalStore_OnDelete(IvCalStore *pIvCalStore,ISignal *pISignal);

Parameters:
   pIvCalStore : [in] Pointer to the IvCalStore Interface object.   
   pISignal    : [in] Pointer to the ISignal object that's set when any object 
                      in the database is deleted

Return Value:
   AEE_SUCCESS    :  If the signal for Delete is added successfully.  
   AEE_EBADPARM   :  If pISignal is NULL

Comments:
   Apps would register ISignal through this API to get notifications
   for any deletions in the calendar database.

Side Effects:
   None

================================================================================
IvCalStore_OnLock

Description:
   This function is used to get notifications for Lock events on the
   calendar store. When the database is locked, notifications would be 
   provided to the apps.

Prototype:
   int IvCalStore_OnLock(IvCalStore *pIvCalStore,ISignal *pISignal);

Parameters:
   pIvCalStore : [in] Pointer to the IvCalStore Interface object.   
   pISignal    : [in] Pointer to the ISignal object that's set when the database 
                      is locked

Return Value:
   AEE_SUCCESS    :  If the signal for lock is added successfully.  
   AEE_EBADPARM   :  If pISignal is NULL

Comments:
   Apps would register ISignal through this API to get notifications
   if the calendar database gets locked.

Side Effects:
   None

==============================================================================*/
#endif //AEEIVCALSTORE_H

        

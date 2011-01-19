/*===========================================================================

FILE: 
   AEEvCalStore.c

SERVICES: 
   Calendar Store APIs

DESCRIPTION
   This file contains a class that implements the IvCalStore interface.
   IvCalStore interface provides user Apps to access the calendar 
   entries stored on the mobile device.

PUBLIC CLASSES:  
   N/A


   	   Copyright © 2007-2008 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
=============================================================================*/

/*=============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================*/
#include "AEEStdlib.h"
#include "AEEIvCalStore.h"
#include "AEEIvCalStoreUtil.h"

/*=============================================================================

                      PUBLIC DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                         CLASS DEFINITIONS

=============================================================================*/

/*=============================================================================
                    FUNCTION PROTOTYPES

=============================================================================*/

uint32  CvCalStore_AddRef(IvCalStore* po);
uint32  CvCalStore_Release(IvCalStore* po);
int     CvCalStore_QueryInterface(IvCalStore *po, 
                                  AEECLSID   iid,
                                  void       **ppo );

int  CvCalStore_AddObject(IvCalStore   *pIvCalStore,                         
                          IvCalObject  *pObject,
                          AEEVProperty *pnAEEPropertyOID);
int  CvCalStore_UpdateObject(IvCalStore  *pIvCalStore,                              
                             IvCalObject *pObject);
int  CvCalStore_GetObjectByOID(IvCalStore   *pIvCalStore,
                               AEEVProperty dwOID,
                               IvCalObject  **ppObj);
int CvCalStore_DeleteObjectByOID(IvCalStore *pMe,                                  
                                 uint32     dwOID);
int  CvCalStore_GetObjectOIDs(IvCalStore   *pIvCalStore, 
                              AEECLSID     RecType,
                              AEEVProperty *pnAEEPropertyOIDs,                              
                              int          dwsize,
                              int          *pdwReqSize);
int CvCalStore_GetOIDByUID(IvCalStore    *pMe, 
                           char          *pszUID,                                                      
                           AEEVProperty  *pnOID);
int CvCalStore_LockCalendar(IvCalStore *pIvCalStore);
int CvCalStore_UnLockCalendar(IvCalStore *pIvCalStore);
int CvCalStore_IsCalendarLocked(IvCalStore *pMe, boolean *bLocked);
int  CvCalStore_OnAdd(IvCalStore *pIvCalStore, 
                      ISignal    *pISignal);
int  CvCalStore_OnUpdate(IvCalStore *pIvCalStore, 
                         ISignal    *pISignal);
int  CvCalStore_OnDelete(IvCalStore *pIvCalStore,                              
                         ISignal    *pISignal);
int  CvCalStore_OnLock(IvCalStore *pIvCalStore,                              
                       ISignal    *pISignal);
void IvCalStore_Dtor(IvCalStore *pMe);
int IvCalStore_New(IShell *piShell,AEECLSID cls, IModule *pIModule,void **pp);
int IvCalStore_HandleAddObject(IvCalStore   *pMe,
                               IvCalObject  *pObject,
                               AEECLSID     RecType,
                               AEEVProperty *pnAEEPropertyOID);
int IvCalStore_HandleAdd(IvCalStore   *pIvCalStore,
                         IvCalObject  *pObject,
                         AEECLSID     RecType,
                         AEEVProperty *pnAEEPropertyOID,
                         AEEVProperty dwUpdateObjectID);
int IvCalStore_DeleteObject(IvCalStore *pIvCalStore,
                            uint32     dwUID);
int IvCalStore_HandleUpdate(IvCalStore   *pIvCalStore,
                            IvCalObject  *pObject,
                            AEECLSID     RecType );
int IvCalStore_GetAlarm(IvCalStore *pMe,
                        uint32      dwOID,                            
                        IvCalObject *pObj);
int IvCalStore_AddSingleProp(IvCalStore  *pMe,
                             IvCalObject *pObject,
                             AEECLSID    RecType,
                             int         nNumProp,
                             uint32      *pdwRecNum,
                             uint32      dwUpdateObjectID);
int IvCalStore_HandleSingleProp(IvCalStore *pMe,
                                IvProperty *pIvProperty,
                                uint32     dwUID,
                                uint32     dwOID,
                                AEECLSID   objType,
                                boolean    bDayLight,
                                uint32     dwRecNum);
int IvCalStore_HandleAlarm(IvCalStore  *pMe,
                           IvCalObject *pIvObject,
                           uint32      dwUID);
int IvCalStore_GetParmString(IvParm *pIvParm, AECHAR **pwszMainStr);
int IvCalStore_AddPropParms(IvCalStore *pMe,
                            IvProperty *pIvProperty,
                            uint32     dwUID);
int IvCalStore_HandleMultiProp(IvCalStore *pMe,
                               IvProperty *pIvProperty,
                               uint32     dwUID,
                               boolean    bAlarmProp,
                               uint32     dwRecNum);
int IvCalStore_HandleGetObject(  IvCalStore  *pMe,
                                 uint32      dwOID,                            
                                 IvCalObject **pObj);
int IvCalStore_AddProperty(IvCalStore   *pMe,                                                     
                           IvCalObject  *pIvObj,
                           AEEVProperty dwProp,
                           void         *pdwVal,
                           uint32       nSize,
                           uint32       dwOID,
                           boolean      bSingle,
                           AEECLSID     objType);
int IvCalStore_GetMultipleProp(IvCalStore  *pMe,
                               IvCalObject *pObject,
                               uint32      dwOID,
                               AEECLSID    objType);
int IvCalStore_AddTimeZone(IvCalStore  *pMe,
                           IvCalObject *pObject,
                           uint32      dwOID);
int IvCalStore_GetTimeZone(IvCalStore  *pMe,
                           uint32      dwOID,
                           IvCalObject *pObject);
int IvCalStore_GetTimeZoneParms(IvCalStore    *pMe,
                                IvProperty    *pIvProperty,
                                AEEVProperty  dwProp,
                                uint32        dwRecNum,
                                boolean       bDayLight);
int IvCalStore_GetAndStoreParms(IvCalStore *pMe,
                                uint32      dwParmID,
                                AECHAR      *pwszParmVal,
                                IvProperty  *pIvProperty);

extern int IvCalStore_GetAlarmComponentFromProp(IvCalStore      *pMe,
                                                IvProperty      *pIvProperty,
                                                IvCalObject     **ppIvCalObject);
extern boolean IvCalStore_CheckForValueW(AECHAR *pwszParmValues);
/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/
 
/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */
/*=============================================================================
Function:
   AEEClsCreateInstance

Prototype:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,
                            IModule * pIModule,void ** ppObj)

Parameters:
   ClsId:    [in]: Specifies the ClassID of the applet which is being loaded
   pIShell:  [in]: Contains pointer to the IShell object. 
   pIModule: [in]: Contains pointer to the IModule object to the 
                   current module to which this app belongs
   ppObj:    [out]: On return, *ppObj must point to a valid IApplet structure. 
   Allocation of memory for this structure and initializing the base data 
   members is done by AEEApplet_New().

Return Value:
   AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() 
                invocation was successful 
   AEE_EFAILED: If the app does not need to be loaded or if errors occurred 
                in AEEApplet_New(). If this function returns FALSE, the app
                will not be loaded.
   AEE_ECLASSNOTSUPPORT: Specified class unsupported

Comments:  
   None

Side Effects: 
   None

===========================================================================*/

#if defined(AEE_STATIC)
  int32 AEEIvCalStore_CreateInstance(AEECLSID ClsId, IShell *pIShell, 
                                     IModule *pIModule, void **ppObj)
#else
  int AEEClsCreateInstance(AEECLSID  ClsId, 
													              IShell 	  *pIShell,
													              IModule 	 *pIModule,
													              void 		   **ppObj)
#endif
{
   int nResult = AEE_EFAILED ; 

   *ppObj = NULL;

   switch( ClsId)
   {
   case AEECLSID_vCalStore :
      nResult = IvCalStore_New(pIShell, ClsId, pIModule, (void**)ppObj);		      
      break;    
   default:
      nResult = AEE_ECLASSNOTSUPPORT;
   }

   return nResult;
}
/*===========================================================================

Function:  
   CvCalStore_AddRef()

Description:
		 This function increases the reference count for the IvCalStore object.

Prototype:
   uint32  CvCalStore_AddRef(IvCalStore* po)

Parameters:
   po: Pointer to IvCalStore interface whose reference count is to be 
       incremented

Return Value:
   The updated reference count

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
uint32  CvCalStore_AddRef(IvCalStore* po)
{   
   return ++(((IvCalStore *)po)->m_nRefs);
}

/*===========================================================================

Function:  
   CvCalStore_Release()

Description:
   This function decreases the reference count for the IvCalStore object. If
   the reference count reaches zero, it does the required cleanup.

Prototype:
	  uint32  CvCalStore_Release(IvCalStore* po)

Parameters:
   po: Pointer to the IvCalStore interface whose reference count needs to be
       decremented.

Return Value:
   The updated reference count

Comments: 
   None

Side Effects:
   None

==============================================================================*/
uint32  CvCalStore_Release(IvCalStore* po)
{  
   IvCalStore *pMe = (IvCalStore*)po;
   if (0 != --pMe->m_nRefs){
      return pMe->m_nRefs;
   }

   IvCalStore_Dtor(po); 

   FREEIF(po);

   return 0;
}
/*===========================================================================

Function:  
   IvCalStore_Dtor()

Description:
   This function frees all the memory allocated for the object.

Prototype:
	  void IvCalStore_Dtor(IvCalStore *pMe);

Parameters:
   pMe: Pointer to the IvCalStore interface 

Return Value:
   Void

Comments: 
   None

Side Effects:
   None

==============================================================================*/
void IvCalStore_Dtor(IvCalStore *pMe)
{   
   RELEASEIF(pMe->pIDeleteSignalBus);

   RELEASEIF(pMe->pIUpdateSignalBus);

   RELEASEIF(pMe->pIAddSignalBus);

   RELEASEIF(pMe->pILockSignalBus);

   RELEASEIF(pMe->pISQL);  

   RELEASEIF(pMe->pIModule);  

   RELEASEIF(pMe->pIShell);
   
}

/*===========================================================================
Function:  
   CvCalStore_QueryInterface()

Description:
		 This function gives the requested interface if supported.

Prototype:
	  int     CvCalStore_QueryInterface(IvCalStore  *po,
                                     AEECLSID    iid,
                                     void        **ppo)

Parameters:
   po:            [in] : Pointer to the IvCalStore interface 
   iid            [in] : The interface ID.
   ppo            [out]: Pointer to resulting interface instance.

Return Value:
   AEE_SUCCESS   :   If the pointer is initialised.
   AEE_ECLASSNOTSUPPORT :   If the interface ID isn't valid one.   

Comments:
   None

Side Effects: 
   None

==============================================================================*/
int  CvCalStore_QueryInterface(IvCalStore* po, 
                               AEECLSID iid,
                               void  **ppo )
{
   int nResult = AEE_ECLASSNOTSUPPORT;
   *ppo = NULL;

   if (AEECLSID_vCalStore == iid) {
      *ppo = po;
      (void)CvCalStore_AddRef(*ppo);
      nResult = AEE_SUCCESS;
   }
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_New()

Description:
		 This function creates and instance of the object specified by classid.

Prototype:
	  int IvCalStore_New(IShell    *piShell,
                      AEECLSID  cls, 
                      IModule   *pIModule,
                      void      **pp);

Parameters:
   piShell        [in]: IShell pointer 
   cls            [in]: Class ID.
   pIModule       [in]: Module pointer.
   pp             [in]: Object to be returned.

Return Value:
   Returns whether instance of the object has been created properly or not.

Comments: 
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_New(IShell *piShell,AEECLSID cls, IModule *pIModule,void **pp)
{
   int nResult = AEE_EFAILED, nSize = 0;
   IvCalStore *me = NULL;
   IvCalStoreVtbl *pModFuncs = NULL;
   
   do{     
      
      nSize = sizeof(IvCalStore);
      me = (IvCalStore*)MALLOC(nSize + sizeof(VTBL(IvCalStore)));
      if(NULL == me){
         nResult = AEE_ENOMEMORY;
         break;
      }
      
      pModFuncs = (IvCalStoreVtbl*)(void *)((byte *)me + nSize); 
      pModFuncs->AddRef = CvCalStore_AddRef;
      pModFuncs->Release = CvCalStore_Release;
      pModFuncs->QueryInterface = CvCalStore_QueryInterface;
      pModFuncs->AddObject = CvCalStore_AddObject;
      pModFuncs->UpdateObject = CvCalStore_UpdateObject;
      pModFuncs->GetObjectByOID = CvCalStore_GetObjectByOID;
      pModFuncs->DeleteObjectByOID = CvCalStore_DeleteObjectByOID;
      pModFuncs->GetObjectOIDs = CvCalStore_GetObjectOIDs;
      pModFuncs->GetObjectOIDByUID = CvCalStore_GetOIDByUID;
      pModFuncs->LockCalendar = CvCalStore_LockCalendar;
      pModFuncs->UnlockCalendar = CvCalStore_UnLockCalendar;
      pModFuncs->IsCalendarLocked = CvCalStore_IsCalendarLocked;
      pModFuncs->OnAdd = CvCalStore_OnAdd;
      pModFuncs->OnUpdate = CvCalStore_OnUpdate;
      pModFuncs->OnDelete = CvCalStore_OnDelete;
      pModFuncs->OnLock = CvCalStore_OnLock;

      AEEINITVTBL(me, IModule, *pModFuncs);           

      me->m_nRefs = 1;
      me->pIShell = piShell;
      me->pIModule = pIModule;
      ISHELL_AddRef(piShell); 

      if(me->pIModule){
         IMODULE_AddRef(me->pIModule);
      }

      //Open Calendar.db
      nResult = IvCalStore_OpenCalendarDB(me);
      BREAKIF(nResult);      

      //Create instances for SignalBuses
      nResult = ISHELL_CreateInstance(piShell, AEECLSID_SignalBus,
                                     (void**)&me->pIUpdateSignalBus);
      BREAKIF(nResult);

      nResult = ISHELL_CreateInstance(piShell, AEECLSID_SignalBus,
                                     (void**)&me->pIDeleteSignalBus);
      BREAKIF(nResult);

      nResult = ISHELL_CreateInstance(piShell, AEECLSID_SignalBus,
                                     (void**)&me->pIAddSignalBus);
      BREAKIF(nResult); 

      nResult = ISHELL_CreateInstance(piShell, AEECLSID_SignalBus,
                                     (void**)&me->pILockSignalBus);
      BREAKIF(nResult);
      *pp = (void*)me;

   }while(0);
   if((AEE_SUCCESS != nResult) && (NULL != me)){
      IvCalStore_Dtor(me);
      FREEIF(me);
   }

   return nResult;
}

/*===========================================================================
Function:  
   CvCalStore_AddObject()

Description:
		 This function adds an object in the database.

Prototype:
	  int  CvCalStore_AddObject(IvCalStore   *pIvCalStore, 
                             IvCalObject  *pObject
                             AEEVProperty *pnAEEPropertyOID)

Parameters:
   pIvCalStore      [in]    : Pointer to the IvCalStore interface    
   pObject          [out]   : Pointer to resulting record.
   pnAEEPropertyOID [in/out]: Pointer to Object ID returned.

Return Value:
   Returns whether the Addition in the DB was successful
   or not.

Comments:  
   None

Side Effects:
   None

==============================================================================*/
int  CvCalStore_AddObject(IvCalStore   *pIvCalStore,                          
                          IvCalObject  *pObject,
                          AEEVProperty *pnAEEPropertyOID)
{
   int nResult = AEE_EFAILED;
   AEECLSID objType = AEEVOBJECT_MAX;      
   boolean bLocked = FALSE;

   if(NULL == pnAEEPropertyOID || NULL == pObject){
      return AEE_EBADPARM;
   }

   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pIvCalStore,&bLocked);
   if(AEE_SUCCESS == nResult){
      //if DB is locked the app can't perform the add operation.
      //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
      //then try for addition.
      if(bLocked){
         return AEE_ENOTALLOWED;
      }
   }else{
      return AEE_EFAILED;
   }
   
   //Check for app privileges.App should have write privilege to 
   //add a record in the database.
   if (!ISHELL_CheckPrivLevel(pIvCalStore->pIShell, 
                              AEEPRIVID_vCalStoreWrite,
                              TRUE)) {
      return AEE_EPRIVLEVEL;
   }   
   
   nResult = IvCalObject_GetObjectId(pObject, &objType);
   if(AEE_SUCCESS != nResult){
      return nResult;
   }

   switch(objType) {
      case AEECLSID_vEvent :
         {                                   
            nResult = IvCalStore_HandleAddObject(pIvCalStore,
                                                 pObject,
                                                 AEECLSID_vEvent,
                                                 pnAEEPropertyOID);
            
            break;
         }
      case AEECLSID_vToDo:
         {                        
            nResult = IvCalStore_HandleAddObject(pIvCalStore, 
                                                 pObject,
                                                 AEECLSID_vToDo,
                                                 pnAEEPropertyOID);
            
            break;
         }
      
      case AEECLSID_vJournal :
      {                             
         nResult = IvCalStore_HandleAddObject(pIvCalStore,
                                              pObject, 
                                              AEECLSID_vJournal,
                                              pnAEEPropertyOID);
         
         break;
      }           
      default:
         {            
            return AEE_EBADPARM;
         }

   }
   
   if(AEE_SUCCESS == nResult){      
      //notify apps through signal that a record has been added      
      nResult = ISignalBus_Strobe(pIvCalStore->pIAddSignalBus);
   }

   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_HandleAddObject()

Description:
		 This function adds an object in the database in a transaction.

Prototype:
	  int IvCalStore_HandleAddObject(IvCalStore   *pMe,
                                  IvCalObject  *pObject,
                                  AEECLSID     RecType,
                                  AEEVProperty *pnAEEPropertyOID)

Parameters:
   pMe              [in]    : Pointer to the IvCalStore interface 
   pObject          [in]    : Pointer to object to be added
   RecType          [in]    : Record type
   pnAEEPropertyOID [in/out]: Pointer to Object ID returned.   

Return Value:
   Returns whether the Addition in the DB was successfull
   or not.

Comments:  
   None

Side Effects:
   None

==============================================================================*/
int IvCalStore_HandleAddObject(IvCalStore   *pMe,
                               IvCalObject  *pObject,
                               AEECLSID     RecType,
                               AEEVProperty *pnAEEPropertyOID)
{
   int nResult = AEE_EFAILED;
   do{
      nResult = ISQL_Exec ( pMe->pISQL, "BEGIN TRANSACTION", NULL, NULL, NULL );
      BREAKIF(nResult);

      nResult = IvCalStore_HandleAdd(pMe,
                                     pObject,
                                     RecType,
                                     pnAEEPropertyOID,
                                     0);
      if(AEE_SUCCESS != nResult){
         (void)ISQL_Exec ( pMe->pISQL, "ROLLBACK TRANSACTION", NULL, NULL, NULL );         
      } else {                
         (void)ISQL_Exec ( pMe->pISQL, "END TRANSACTION", NULL, NULL, NULL ) ;        
      }      

   }while(0);
   
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleAdd()

Description:
   This function adds an Event, ToDo , Journal object in the database.

Prototype:
	  int IvCalStore_HandleAdd(IvCalStore     *pIvCalStore,
                            IvCalObject    *pObject,
                            AEEvObjectType RecType,
                            AEEVProperty   *pnAEEPropertyOID,
                            AEEVProperty   dwUpdateObjectID);

Parameters:
   pIvCalStore      [in]: Pointer to the IvCalStore interface 
   pObject          [in]: Object to be added
   RecType          [in]: type of record to be added
   pnAEEPropertyOID [out]:Record Number to be returned.
   dwUpdateObjectID [in]: Object ID when updating a record

Return Value:
   Returns whether the Addition in the DB was successful
   or not.

Comments: 
   This function first stores all the properties for a component in the MAIN 
   table which can occur only once in the component. This provides the MAIN 
   table RowId for the record inserted. Once this is done all the properties
   which occur more than once and their parameters are stored in PROP and PARM 
   tables. All the parameters for properties which occur only once are also 
   stored in the PARM table.
   All the ATTENDEE and Recurrence rules are also added to respective tables.
   If the Alarm sub component is present in the object it would be added 
   to the Alarm table.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleAdd(IvCalStore     *pIvCalStore,
                         IvCalObject    *pObject,
                         AEECLSID       RecType,
                         AEEVProperty   *pnAEEPropertyOID,
                         AEEVProperty   dwUpdateObjectID)
{
   int nResult = AEE_EFAILED, nNumProp = 0, nCount = 0, nAlarm = 0, 
       nTimeZone = 0;   
   IvCalObject *pAlarm = NULL;
   IvCalObject *pTimeZone = NULL;
   uint32 dwOID = 0, dwRecNum = 0;         
   IvCalStore *pMe = (IvCalStore*)pIvCalStore;
   IvProperty *pIvProperty = NULL;   
      
   if(NULL == pnAEEPropertyOID || NULL == pObject){
      return AEE_EBADPARM;
   } 
   do{         
      nResult = IvCalObject_GetNumberOfProperties(pObject, 0, &nNumProp);         
      BREAKIF(nResult);
      //First add all the properties that can occur once to get the record
      //number or OID for this record from MAIN table.
      //We would then add other properties and their parameters later
      nResult = IvCalStore_AddSingleProp(pMe,
                                         pObject,
                                         RecType,
                                         nNumProp,
                                         &dwRecNum,
                                         dwUpdateObjectID);
      BREAKIF(nResult);
      if(0 == dwUpdateObjectID){
         dwOID = dwRecNum;
      }else{
         dwOID = dwUpdateObjectID;
      }

      //Now add all the properties and their parameters which can occur
      //more than once in a component.We also add the parameters for the 
      //properties which we have added the property values before/above
      for(nCount =0; nCount < nNumProp; nCount++){
         nResult = IvCalObject_GetPropertyAtIndex(pObject, 
                                                  0, 
                                                  nCount,
                                                  &pIvProperty);         
         if(AEE_SUCCESS != nResult || NULL == pIvProperty){
            nResult = AEE_EFAILED;
            break;
         }
         nResult = IvCalStore_AddPropParms(pMe, pIvProperty, dwOID);        
         if((AEE_SUCCESS != nResult)&&(SQL_DONE != nResult)){
            nResult = AEE_EFAILED;
            break;
         }else {
            nResult = AEE_SUCCESS;
         }  
         RELEASEIF(pIvProperty);
      }
      BREAKIF(nResult);

      //Check if any alarm sub component is present. If yes add that too
      if((AEECLSID_vEvent == RecType) || (AEECLSID_vToDo == RecType)){
         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfObjects(pObject, 
                                                     AEECLSID_vAlarm, 
                                                     &nAlarm);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nAlarm; nCount++){
               nResult = IvCalObject_GetObjectAtIndex(pObject,
                                                      nCount,
                                                      AEECLSID_vAlarm,
                                                      &pAlarm);

               BREAKIF(nResult);
               if(NULL != pAlarm){
                  nResult = IvCalStore_HandleAlarm(pMe,pAlarm,dwOID);
                  RELEASEIF(pAlarm);                  
               }else {
                  nResult = AEE_EFAILED;
               }
               BREAKIF(nResult);
            }
         }

         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfProperties(pObject, 
                                                        AEEVPROPERTY_AALARM, 
                                                        &nAlarm);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nAlarm; nCount++){
               IvProperty *pIvAAlarmProperty = NULL;
               nResult = IvCalObject_GetPropertyAtIndex(pObject, 
                                                         AEEVPROPERTY_AALARM, 
                                                         nCount,
                                                         &pIvAAlarmProperty);
               BREAKIF(nResult);
               if(NULL == pIvAAlarmProperty){
                  nResult = AEE_EFAILED;
                  break;
               }
               nResult = IvCalStore_GetAlarmComponentFromProp(pMe,
                                                              pIvAAlarmProperty,
                                                              &pAlarm);
               RELEASEIF(pIvAAlarmProperty);
               BREAKIF(nResult);
               nResult = IvCalStore_HandleAlarm(pMe,pAlarm,dwOID);
               RELEASEIF(pAlarm);            
               BREAKIF(nResult);
            }
         }

         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfProperties(pObject,
                                                        AEEVPROPERTY_DALARM, 
                                                        &nAlarm);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nAlarm; nCount++){
               IvProperty *pIvAAlarmProperty = NULL;
               nResult = IvCalObject_GetPropertyAtIndex(pObject, 
                                                        AEEVPROPERTY_DALARM, 
                                                        nCount,
                                                        &pIvAAlarmProperty);
               BREAKIF(nResult);
               if(NULL == pIvAAlarmProperty){
                  nResult = AEE_EFAILED;
                  break;
               }
               nResult = IvCalStore_GetAlarmComponentFromProp(pMe,
                                                              pIvAAlarmProperty,
                                                              &pAlarm);
               RELEASEIF(pIvAAlarmProperty);
               BREAKIF(nResult);
               nResult = IvCalStore_HandleAlarm(pMe,pAlarm,dwOID);
               RELEASEIF(pAlarm);            
               BREAKIF(nResult);
            }
         }

         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfProperties(pObject, 
                                                        AEEVPROPERTY_MALARM, 
                                                        &nAlarm);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nAlarm; nCount++){
               IvProperty *pIvAAlarmProperty = NULL;
               nResult = IvCalObject_GetPropertyAtIndex(pObject, 
                                                        AEEVPROPERTY_MALARM, 
                                                        nCount,
                                                        &pIvAAlarmProperty);
               BREAKIF(nResult);
               if(NULL == pIvAAlarmProperty){
                  nResult = AEE_EFAILED;
                  break;
               }
               nResult = IvCalStore_GetAlarmComponentFromProp(pMe,
                                                              pIvAAlarmProperty,
                                                              &pAlarm);
               RELEASEIF(pIvAAlarmProperty);
               BREAKIF(nResult);
               nResult = IvCalStore_HandleAlarm(pMe,pAlarm,dwOID);
               RELEASEIF(pAlarm);            
               BREAKIF(nResult);
            }
         }

         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfProperties(pObject, 
                                                        AEEVPROPERTY_PALARM, 
                                                        &nAlarm);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nAlarm; nCount++){
               IvProperty *pIvAAlarmProperty = NULL;
               nResult = IvCalObject_GetPropertyAtIndex(pObject, 
                                                        AEEVPROPERTY_PALARM, 
                                                        nCount,
                                                        &pIvAAlarmProperty);
               BREAKIF(nResult);
               if(NULL == pIvAAlarmProperty){
                  nResult = AEE_EFAILED;
                  break;
               }
               nResult = IvCalStore_GetAlarmComponentFromProp(pMe,
                                                              pIvAAlarmProperty,
                                                              &pAlarm);
               RELEASEIF(pIvAAlarmProperty);
               BREAKIF(nResult);
               nResult = IvCalStore_HandleAlarm(pMe,pAlarm,dwOID);
               RELEASEIF(pAlarm);            
               BREAKIF(nResult);
            }
         }
         //Check whether this component is associated with some time zone 
         //component. if yes add the time zone component to the database
         if(AEE_SUCCESS == nResult){
            nResult = IvCalObject_GetNumberOfObjects(pObject, 
                                                     AEECLSID_vTimeZone,
                                                     &nTimeZone);
            BREAKIF(nResult);
            for(nCount = 0; nCount < nTimeZone; nCount++){
               nResult = IvCalObject_GetObjectAtIndex(pObject,
                                                      nCount,
                                                      AEECLSID_vTimeZone,
                                                      &pTimeZone);

               BREAKIF(nResult);
               nResult = IvCalStore_AddTimeZone(pMe,pTimeZone,dwOID);
               RELEASEIF(pTimeZone);
               BREAKIF(nResult);
            }
         }
      }
     
      if(AEE_SUCCESS == nResult){                         
         *pnAEEPropertyOID = dwOID;            
      }     
      
   }while(0); 
   RELEASEIF(pIvProperty);
   RELEASEIF(pAlarm);
   RELEASEIF(pTimeZone);   
   return nResult;

}
/*===========================================================================
Function:  
   IvCalStore_AddSingleProp()

Description:
		 This function adds all the single time occuring properties
   present in the object in the database  .

Prototype:
	  int IvCalStore_AddSingleProp(IvCalStore  *pMe,
                                IvCalObject *pObject,
                                AEECLSID    RecType,
                                int         nNumProp,
                                uint32      *pdwRecNum,
                                uint32      dwUpdateObjectID)

Parameters:
   pMe              [in]    : Pointer to the IvCalStore interface 
   pObject          [in]    : Pointer to object provided for addition.
   RecType          [in]    : Record type.
   nNumProp         [in]    : Number of properties in the object
   pdwRecNum        [in/out]: Record number of the added record returned.
   dwUpdateObjectID [in]    : Object ID used when updating a record.

Return Value:
   Returns whether the Addition in the DB for those properties which
   occur once was successfull or not.

Comments:  
   None

Side Effects:
   None

==============================================================================*/
int IvCalStore_AddSingleProp(IvCalStore  *pMe,
                             IvCalObject *pObject,
                             AEECLSID    RecType,
                             int         nNumProp,
                             uint32      *pdwRecNum,
                             uint32      dwUpdateObjectID)
{
   int nResult = AEE_EFAILED, nCount = 0, nSize = 0;
   uint32 dwProp = 0;
   char  *pszUIDStr = NULL, *pszTransp = NULL,
          *pszURL = NULL, *pszLocation = NULL, *pszDesc = NULL,
          *pszSummary = NULL, *pszClass = NULL, *pszTZID = NULL,
          *pszDaylight = NULL, *pszDuration = NULL, *pszCreated = NULL,
          *pszCompleted = NULL, *pszDue = NULL, *pszDtEnd = NULL,
          *pszLastModified = NULL, *pszDtStart = NULL, *pszRecurId = NULL,
          *pszGeo = NULL, *pszDtStamp = NULL, *pszStatus = NULL;
   char *pszStrMain = "INSERT INTO "VCALSTORE_MAIN_TBL
      " (Transp,Sequence,LastModified,DateEnd,Class,DateStart,"
      "DateTime,Description,Duration,Geo,Location,Priority,"
      "Status,Summary,RNum,URL,TZID,RecurID,DateCompleted,DateCreated,"
      "Due,PercentComplete,Type,UID,TimeZone,DayLight"
      ") VALUES"
      "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

   char *pszStrUpdateMain = "INSERT INTO "VCALSTORE_MAIN_TBL
      " (Transp,Sequence,LastModified,DateEnd,Class,DateStart,"
      "DateTime,Description,Duration,Geo,Location,Priority,"
      "Status,Summary,RNum,URL,TZID,RecurID,DateCompleted,DateCreated,"
      "Due,PercentComplete,Type,UID,TimeZone,DayLight,RecordNumber"
      ") VALUES"
      "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
   ISQLStmt *pStmt = NULL;
   IvProperty *pIvProperty = NULL, *pIvPropertyTemp = NULL;
   if((NULL == pObject) || (NULL == pdwRecNum)){
      return AEE_EBADPARM;
   }

   do{
      if(0 == dwUpdateObjectID){
         nResult = ISQL_Prepare( pMe->pISQL, pszStrMain, -1, &pStmt, NULL );
      }else{
         nResult = ISQL_Prepare( pMe->pISQL, pszStrUpdateMain, -1, &pStmt, NULL );
      }
      //BREAKIF(nResult);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = ISQLSTMT_BindInt(pStmt, 23, (int)RecType);
      //BREAKIF(nResult);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }


      if(0 != dwUpdateObjectID){
         nResult = ISQLSTMT_BindInt(pStmt, 27, (int)dwUpdateObjectID);
         BREAKIF(nResult);
      }

      for(nCount =0; nCount < nNumProp; nCount++){
         nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                                  0,
                                                  nCount,
                                                  &pIvProperty);
         BREAKIF(nResult);
         if(NULL == pIvProperty){
            nResult = AEE_EFAILED;
            break;
         }
         nResult = IvProperty_GetId(pIvProperty, &dwProp);
         BREAKIF(nResult);
         switch(dwProp) 
         {         
         case AEEVPROPERTY_TRANSP:
            {               
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pszTransp = (char*)MALLOC(nSize);
               if(NULL == pszTransp){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszTransp,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 1, pszTransp, -1, NULL);               
               break;
            }
         case AEEVPROPERTY_SEQUENCE:
            {
               uint32 Value = 0;           
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               TRUE,
                                               (uint32*)&Value, 
                                               sizeof(uint32),
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindInt(pStmt, 2, (int)Value);               
               break;
            }

         case AEEVPROPERTY_LASTMODIFIED:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszLastModified = (char*)MALLOC(nSize);
               if(NULL == pszLastModified){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszLastModified,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 3, pszLastModified, -1, NULL);
               break;
            }
         case AEEVPROPERTY_DTEND:
            {
               
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszDtEnd = (char*)MALLOC(nSize);
               if(NULL == pszDtEnd){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDtEnd,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 4, pszDtEnd, -1, NULL);
               break;
            }
         case AEEVPROPERTY_CLASS:               
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);
               pszClass = (char*)MALLOC(nSize);
               if(NULL == pszClass){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE,
                                               pszClass,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 5, pszClass, -1, NULL);               
               break;  
            }
         case AEEVPROPERTY_DTSTART:
            {                                                                    
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszDtStart = (char*)MALLOC(nSize);
               if(NULL == pszDtStart){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDtStart,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 6, pszDtStart, -1, NULL);
               break;
            }  
         case AEEVPROPERTY_DTSTAMP:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszDtStamp = (char*)MALLOC(nSize);
               if(NULL == pszDtStamp){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDtStamp,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 7, pszDtStamp, -1, NULL);
               break;
            }
         case AEEVPROPERTY_DESCRIPTION:
            {               
               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pszDesc = (char*)MALLOC(nSize);
               if(NULL == pszDesc){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDesc,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 8, pszDesc, -1, NULL);               
               break;

            }
         case AEEVPROPERTY_DURATION:
            {               
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult); 
               pszDuration = (char*)MALLOC(nSize);
               if(NULL == pszDuration){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDuration,
                                               nSize, 
                                               &nSize);
               nResult = ISQLSTMT_BindText(pStmt, 9, pszDuration, -1, NULL);               
               break;                  
            }
         case AEEVPROPERTY_GEO:
            { 
               AEEVGeo Geo = {0};
               int nReqSize = 0;
               //Parse the Geo property first
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               TRUE,
                                               &Geo,
                                               sizeof(AEEVGeo), 
                                               &nReqSize);
               BREAKIF(nResult);
               nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                               AEECLSID_vProperty, 
                                               (void**)&pIvPropertyTemp);
               BREAKIF(nResult);
               //Init a temporary property to get the iCal 2.0 version
               //for the property
               nResult = IvProperty_InitProperty(pIvPropertyTemp,
                                                 dwProp,
                                                 NULL,
                                                 TRUE,
                                                 &Geo,
                                                 sizeof(AEEVGeo),
                                                 NULL);
               BREAKIF(nResult);

               //parse value for the same temporary property to get
               //iCal 2.0 Geo property format.
               nResult = IvProperty_ParseValue(pIvPropertyTemp, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszGeo = (char*)MALLOC(nSize);
               if(NULL == pszGeo){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvPropertyTemp, 
                                               FALSE,
                                               pszGeo,
                                               nSize, 
                                               &nSize);
               RELEASEIF(pIvPropertyTemp);
               BREAKIF(nResult);
               
               nResult = ISQLSTMT_BindText(pStmt, 10, pszGeo, -1, NULL);
               break;
            }
         case AEEVPROPERTY_LOCATION:
            {                           
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL, 
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pszLocation = (char*)MALLOC(nSize);
               if(NULL == pszLocation){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszLocation,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 11, pszLocation, -1, NULL);               
               break;

            }
         case AEEVPROPERTY_PRIORITY:
            {
               uint32 Value = 0;           
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               TRUE,
                                               (uint32*)&Value,
                                               sizeof(uint32),
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindInt(pStmt, 12, (int)Value);               
               break;
            }
         case AEEVPROPERTY_STATUS:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL, 
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pszStatus = (char*)MALLOC(nSize);
               if(NULL == pszStatus){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszStatus,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 13, pszStatus, -1, NULL);               
               break;

            }
         case AEEVPROPERTY_SUMMARY:
            {

               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE, 
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pszSummary = (char*)MALLOC(nSize);
               if(NULL == pszSummary){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszSummary,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 14, pszSummary, -1, NULL);               
               break;             
            }                     
         case AEEVPROPERTY_RNUM:
            {
               uint32 Value = 0;           
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               TRUE,
                                              (uint32*)&Value, 
                                              sizeof(uint32),
                                              &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindInt(pStmt, 15, (int)Value);               
               break;

            }            
         case AEEVPROPERTY_URL: 
            {                              
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);
               pszURL = (char*)MALLOC(nSize);
               if(NULL == pszURL){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE,
                                               pszURL,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 16, pszURL, -1, NULL);               
               break;
            }                 
         case AEEVPROPERTY_TZID:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszTZID = (char*)MALLOC(nSize);
               if(NULL == pszTZID){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszTZID,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 17, pszTZID, -1, NULL);
               break;
            }
         case AEEVPROPERTY_RECUR_ID:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszRecurId = (char*)MALLOC(nSize);
               if(NULL == pszRecurId){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszRecurId,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 18, pszRecurId, -1, NULL);
               break;
            }     
         case AEEVPROPERTY_DCOMPLETED:
            {                                
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszCompleted = (char*)MALLOC(nSize);
               if(NULL == pszCompleted){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszCompleted,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 19, pszCompleted, -1, NULL);
               break;

            }
         case AEEVPROPERTY_DCREATED:
         case AEEVPROPERTY_CREATED:
            {               
               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE,
                                               NULL, 
                                               0,
                                               &nSize);
               BREAKIF(nResult);               
               pszCreated = (char*)MALLOC(nSize);
               if(NULL == pszCreated){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszCreated,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 20, pszCreated, -1, NULL); 
               break;

            }
         case AEEVPROPERTY_DUE:
            {
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);               
               pszDue = (char*)MALLOC(nSize);
               if(NULL == pszDue){
                  nResult = AEE_ENOMEMORY;
                  break;
               }                          
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDue,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 21, pszDue, -1, NULL);
               break;

            }
         case AEEVPROPERTY_PERCENT:
            {
               uint32 Value = 0;           
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               (uint32*)&Value, 
                                               sizeof(uint32),
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindInt(pStmt, 22, (int)Value);               
               break;

            }
         case AEEVPROPERTY_UID:
            {                    
               nResult = IvProperty_ParseValue(pIvProperty,
                                               FALSE,
                                               NULL,
                                               0, 
                                               &nSize);
               BREAKIF(nResult);
               pszUIDStr = (char*)MALLOC(nSize);
               if(NULL == pszUIDStr){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszUIDStr,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 24, pszUIDStr, -1, NULL);               
               break;
            }                                                                      
         case AEEVPROPERTY_TIMEZONE:
            {               
               int32 nTimeZone = 0;

               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               &nTimeZone, 
                                               sizeof(int32), 
                                               &nSize);
               BREAKIF(nResult);
               
               nResult = ISQLSTMT_BindInt(pStmt, 25, (int)nTimeZone);               
               break;
            }         
         case AEEVPROPERTY_DAYLIGHT:
            {
               nResult = IvProperty_ParseValue(pIvProperty, FALSE, NULL, 0, &nSize);
               BREAKIF(nResult);
               pszDaylight = (char*)MALLOC(nSize);
               if(NULL == pszDaylight){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty, 
                                               FALSE,
                                               pszDaylight,
                                               nSize, 
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISQLSTMT_BindText(pStmt, 26, pszDaylight, -1, NULL); 
               break;
            }                                                                       
         default :
            {            
               break;
            }
         }
         RELEASEIF(pIvProperty);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
      }      
      BREAKIF(nResult);      
      nResult = ISQLSTMT_Step(pStmt);
      if( SQL_DONE != nResult) { 
         nResult = AEE_EFAILED;
         break;
      }else{
         nResult = AEE_SUCCESS;
      }
      *pdwRecNum = (uint32)ISQL_GetLastRowId(pMe->pISQL);                       

   }while(0);
   RELEASEIF(pStmt);
   RELEASEIF(pIvProperty);
   RELEASEIF(pIvPropertyTemp);
   FREEIF(pszCreated);
   FREEIF(pszCompleted);
   FREEIF(pszDtStamp);
   FREEIF(pszRecurId);
   FREEIF(pszDue);
   FREEIF(pszGeo);
   FREEIF(pszDtStart);
   FREEIF(pszLastModified);
   FREEIF(pszDtEnd);
   FREEIF(pszDaylight);
   FREEIF(pszDuration);           
   FREEIF(pszUIDStr);
   FREEIF(pszTransp);
   FREEIF(pszTZID);
   FREEIF(pszURL);
   FREEIF(pszLocation);
   FREEIF(pszDesc);
   FREEIF(pszSummary);
   FREEIF(pszClass);
   FREEIF(pszStatus);
   return nResult;
}
/*===========================================================================
Function:  
   CvCalStore_UpdateObject()

Description:
		 This function updates an object in the database.

Prototype:
	  int CvCalStore_UpdateObject)(IvCalStore *pIvCalStore,                                 
                                IvCalObject *pObject);

Parameters:
   pIvCalStore    [in]: Pointer to the IvCalStore interface    
   pObject        [out]:Pointer to object to be updated.

Return Value:
   Returns whether the updation in the DB was successful
   or not.

Comments:  
   None

Side Effects:
   None

==============================================================================*/
int  CvCalStore_UpdateObject(IvCalStore *pIvCalStore,                              
                             IvCalObject *pObject)
{
   int nResult = AEE_EFAILED;
   AEECLSID objType = AEEVOBJECT_MAX;      
   boolean bLocked = FALSE;  

   if(NULL == pObject){
      return AEE_EBADPARM;
   }

   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pIvCalStore,&bLocked);
   if(AEE_SUCCESS == nResult){
   //if DB is locked the app can't perform the update operation.
   //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
   //then try for addition.
      if(bLocked){
         return AEE_ENOTALLOWED;
      }
   }else{
      return AEE_EFAILED;
   }

   //First check for app privileges.
   if (!ISHELL_CheckPrivLevel(pIvCalStore->pIShell, 
                              AEEPRIVID_vCalStoreWrite, 
                              TRUE)) {
      return AEE_EPRIVLEVEL;
   }
   
   nResult = IvCalObject_GetObjectId(pObject, &objType);
   if(AEE_SUCCESS != nResult){
      return nResult;
   }

   switch(objType) {
      case AEECLSID_vEvent :
         {                                 
            nResult = IvCalStore_HandleUpdate(pIvCalStore,
                                              (IvCalObject*)pObject, 
                                              AEECLSID_vEvent);            
            break;
         }
      case AEECLSID_vToDo:
         {                       
            nResult = IvCalStore_HandleUpdate(pIvCalStore, 
                                              (IvCalObject*)pObject, 
                                              AEECLSID_vToDo);            
            break;
         }      
      case AEECLSID_vJournal :
      {                             
         nResult = IvCalStore_HandleUpdate(pIvCalStore, 
                                           (IvCalObject*)pObject, 
                                           AEECLSID_vJournal );         
         break;
      }
      default:
         {            
            return AEE_EBADPARM;
         }

   }
   if(AEE_SUCCESS == nResult){
      nResult = ISignalBus_Strobe(pIvCalStore->pIUpdateSignalBus);
   }
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleUpdate()

Description:
		 This function updates an object in the database  .

Prototype:
	  int IvCalStore_HandleUpdate(IvCalStore *pIvCalStore,
                               IvCalObject* pObject,
                               AEEvObjectType RecType)
                           

Parameters:
   pIvCalStore    [in]: Pointer to the IvCalStore interface 
   pObject        [in]: Pointer to pEvent
   RecType        [in]: Record object type

Return Value:
   Returns whether the Addition in the DB was successful
   or not.

Comments:  
   This function updates the records in all the tables if necessary for a 
   new updated object provided. It first makes all the fields in the 
   MAIN table NULL and then updates the same with the new values. It also 
   deletes all the concerned records in other tables and adds the new 
   updated records in the respective tables.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleUpdate(IvCalStore   *pIvCalStore,
                            IvCalObject  *pObject,
                            AEECLSID     RecType )                          
{
   int nResult = AEE_EFAILED, nNumProp = 0, nSize = 0;      
   uint32 dwOID = 0, dwUpdateObjectID = 0;      
   IvCalStore *pMe = (IvCalStore*)pIvCalStore;
   IvProperty *pIvProperty = NULL;                 

   if(NULL == pObject){
      return AEE_EBADPARM;
   }
   do{      
            
      nResult = IvCalObject_GetNumberOfProperties(pObject, 0, &nNumProp); 
      nResult |= IvCalObject_GetPropertyAtIndex(pObject,
                                                AEEVPROPERTY_OID,
                                                0, 
                                                &pIvProperty);
      BREAKIF(nResult); 
      if(NULL != pIvProperty){

         nResult = IvProperty_ParseValue(pIvProperty,
                                          TRUE,
                                          &dwOID,
                                          sizeof(uint32),
                                          &nSize);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult); 
      }else {
         nResult = AEE_EFAILED;
         break;
      }

      nResult = ISQL_Exec ( pMe->pISQL, "BEGIN TRANSACTION", NULL, NULL, NULL );
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = IvCalStore_DeleteObject(pIvCalStore,dwOID);
      BREAKIF(nResult);
      

      nResult = IvCalStore_HandleAdd(pIvCalStore, 
                                     pObject, 
                                     RecType, 
                                     &dwUpdateObjectID,
                                     dwOID);                                                 

      if (AEE_SUCCESS != nResult){
         (void)ISQL_Exec ( pMe->pISQL, 
                           "ROLLBACK TRANSACTION", 
                           NULL,
                           NULL,
                           NULL );      
      } else {
         (void)ISQL_Exec ( pMe->pISQL, 
                           "END TRANSACTION", 
                           NULL,
                           NULL,
                           NULL );
      }      

   }while(0);   
      
     
   return nResult;

}
/*===========================================================================
Function:  
   CvCalStore_GetObjectByOID()

Description:
		 This function gets and object from the database according to the
   OID specified.

Prototype:
	  int  CvCalStore_GetObjectByOID(IvCalStore *pIvCalStore,
                                  AEEVProperty oid,
                                  IvCalObject **pObj);
Parameters:
   pIvCalStore    [in]: Pointer to the IvCalStore interface 
   dwOID          [in]: OID of the entity.    
   ppObj          [out]: Returned object.

Return Value:
   Returns whether the object has been found successfully or not.

Comments:
   None

Side Effects:
   None

==============================================================================*/
int  CvCalStore_GetObjectByOID(IvCalStore    *pIvCalStore,
                               AEEVProperty  dwOID,
                               IvCalObject   **ppObj)
{
   int nResult = AEE_EFAILED;          
   boolean bLocked = FALSE;

   if(NULL == ppObj){
      return AEE_EBADPARM;
   }

   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pIvCalStore,&bLocked);
   if(AEE_SUCCESS == nResult){
   //if DB is locked the app can't perform the add operation.
   //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
   //then try for addition.
   if(bLocked){
      return AEE_ENOTALLOWED;
   }
   }else{
      return AEE_EFAILED;
   }

   //First check for app privileges.
   if (!ISHELL_CheckPrivLevel(pIvCalStore->pIShell, 
                              AEEPRIVID_vCalStoreRead, 
                              TRUE)) {
      return AEE_EPRIVLEVEL;
   }
   
   do{            
      nResult = IvCalStore_HandleGetObject( pIvCalStore,
                                            dwOID,
                                            (IvCalObject**)ppObj);

      
   }while(0);  
   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_HandleGetObject()

Description:
		 This function gets an object from the store.

Prototype:
   int IvCalStore_HandleGetObject(IvCalStore     *pMe,
                                  uint32         dwOID,                            
                                  IvCalObject    **pObj);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   dwOID          [in]:  Object ID
   pObj           [out]: Object to be returned

Return Value:
   Returns whether the object has been recovered/received from 
   DB successfully or not.

Comments:
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleGetObject(  IvCalStore  *pMe,
                                 uint32      dwOID,                            
                                 IvCalObject **pObj)
{
   int nResult = AEE_EFAILED, /*nRnum = 0,*/ nSeq = 0;
   uint32 dwPrio = 0, dwPercComp = 0; 
   AEECLSID  objType = 0;
   IvCalObject *pObject = NULL;    
   char *pszStr = "SELECT Type FROM "VCALSTORE_MAIN_TBL
               " WHERE RecordNumber = ?";
   ISQLStmt *pStmt = NULL;   
   int nTimeZone = 0;
   char *pszTransp = NULL, *pszClass = NULL, *pszURL = NULL,
          *pszSummary = NULL, *pszStatus = NULL, *pszLocation = NULL,
          *pszUID = NULL, *pszDesc = NULL, *pszTZID = NULL,
          *pszDaylight = NULL, *pszLastModified = NULL, *pszDateCompleted = NULL,
          *pszDateCreated = NULL, *pszDateEnd = NULL, *pszDuration = NULL,
          *pszGeo = NULL, *pszRecurId = NULL, *pszDtStart = NULL,
          *pszDue = NULL, *pszDateTime = NULL;           
   char *pszStrObject="SELECT RNum,Transp,Sequence,LastModified,"
                  "DateCompleted,DateCreated,DateEnd,TimeZone,URL,RecurId,Duration,"
                  "DateStart,Due,Summary,Status,Priority,PercentComplete,"
                  "Location,Geo,Description,Class,DateTime,UID,TZID,DayLight FROM "
                  VCALSTORE_MAIN_TBL" WHERE RecordNumber = ?";     

   if(NULL == pObj){
      return AEE_EBADPARM;
   }
   
   do{
      //First get the object type from database.
      nResult = ISQL_Prepare(pMe->pISQL, pszStr, -1, &pStmt, NULL );
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_ROW == nResult){
         nResult = ISQLSTMT_GetColumnInt(pStmt, 0, (int*)&objType);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
      }else{
         nResult = AEE_SUCCESS;
         *pObj = NULL;
         break;
      }

      RELEASEIF(pStmt);

      if(AEECLSID_vEvent == objType){
         nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                         AEECLSID_vEvent, 
                                         (void**)&pObject);         
      }else if(AEECLSID_vToDo == objType){
         nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                         AEECLSID_vToDo, 
                                         (void**)&pObject);        
      }else if(AEECLSID_vJournal == objType){
         nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                         AEECLSID_vJournal, 
                                         (void**)&pObject);         
      }
      BREAKIF(nResult);

      nResult = ISQL_Prepare(pMe->pISQL,pszStrObject, -1 ,&pStmt,NULL);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      (void)ISQLSTMT_BindInt(pStmt,1,(int)dwOID);

      while(SQL_ROW == ISQLSTMT_Step(pStmt)){                 
         /*nResult = ISQLSTMT_ColumnSize(pStmt,0);
         if(0 != nResult){
            nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nRnum);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_RNUM,
                                             &nRnum,
                                             sizeof(uint32),
                                             dwOID,
                                             TRUE,
                                             objType); 
            BREAKIF(nResult);            
         }*/
         
         nResult = ISQLSTMT_GetColumnText(pStmt,1,&pszTransp);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszTransp){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_TRANSP,
                                             pszTransp,
                                             STRLEN(pszTransp),
                                             dwOID,
                                             TRUE,
                                             objType); 
            BREAKIF(nResult);
         }                          

         nResult = ISQLSTMT_ColumnSize(pStmt,2);
         if(0 != nResult){
            nResult = ISQLSTMT_GetColumnInt(pStmt,2,&nSeq);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_SEQUENCE,
                                             &nSeq,
                                             sizeof(uint32),
                                             dwOID,
                                             TRUE,
                                             objType);

            BREAKIF(nResult);
         }
                 
         nResult = ISQLSTMT_GetColumnText(pStmt, 3, &pszLastModified);            
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszLastModified){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_LASTMODIFIED,
                                             pszLastModified,
                                             STRLEN(pszLastModified),
                                             dwOID,
                                             TRUE,
                                             objType);            
            BREAKIF(nResult);
         }
                  
         nResult = ISQLSTMT_GetColumnText(pStmt,4,&pszDateCompleted);  
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszDateCompleted){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DCOMPLETED,
                                             pszDateCompleted,
                                             STRLEN(pszDateCompleted),
                                             dwOID,
                                             TRUE,
                                             objType);

            BREAKIF(nResult);
         }
                          
         nResult = ISQLSTMT_GetColumnText(pStmt, 5, &pszDateCreated); 
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszDateCreated){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DCREATED,
                                             pszDateCreated,
                                             STRLEN(pszDateCreated),
                                             dwOID,
                                             TRUE,
                                             objType); 
            if(AEE_EUNSUPPORTED == nResult){
               nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_CREATED,
                                             pszDateCreated,
                                             STRLEN(pszDateCreated),
                                             dwOID,
                                             TRUE,
                                             objType); 

            }
            BREAKIF(nResult);
         }
                         
         nResult = ISQLSTMT_GetColumnText(pStmt, 6, &pszDateEnd); 
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszDateEnd){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DTEND,
                                             pszDateEnd,
                                             STRLEN(pszDateEnd),
                                             dwOID,
                                             TRUE,
                                             objType);            
            BREAKIF(nResult);
         }

         nResult = ISQLSTMT_ColumnSize(pStmt,7);
         if(0 != nResult){            
            nResult = ISQLSTMT_GetColumnInt(pStmt,7,&nTimeZone); 
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_TIMEZONE,
                                             &nTimeZone,
                                             sizeof(int32),
                                             dwOID,
                                             TRUE,
                                             objType);                       
            BREAKIF(nResult);
         }         
         
         nResult = ISQLSTMT_GetColumnText(pStmt,8,&pszURL);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszURL){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_URL,
                                             pszURL,
                                             STRLEN(pszURL),
                                             dwOID,
                                             TRUE,
                                             objType);
            BREAKIF(nResult);
         }
                          
         nResult = ISQLSTMT_GetColumnText(pStmt, 9, &pszRecurId); 
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszRecurId){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_RECUR_ID,
                                             pszRecurId,
                                             STRLEN(pszRecurId),
                                             dwOID,
                                             TRUE,
                                             objType);   

            BREAKIF(nResult);
         }
                  
         nResult = ISQLSTMT_GetColumnText(pStmt,10,&pszDuration);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         if(NULL != pszDuration){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DURATION,
                                             pszDuration,
                                             STRLEN(pszDuration),
                                             dwOID,
                                             TRUE,
                                             objType);                                   

            BREAKIF(nResult);
         }
         
            
         nResult = ISQLSTMT_GetColumnText(pStmt, 11, &pszDtStart);                                    
         BREAKIF(nResult);
         if(NULL != pszDtStart){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DTSTART,
                                             pszDtStart,
                                             STRLEN(pszDtStart),
                                             dwOID,
                                             TRUE,
                                             objType);  
            
            BREAKIF(nResult);
         }
                     
         nResult = ISQLSTMT_GetColumnText(pStmt, 12, &pszDue);             
         BREAKIF(nResult);
         if(NULL != pszDue){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DUE,
                                             pszDue,
                                             STRLEN(pszDue),
                                             dwOID,
                                             TRUE,
                                             objType);  
            
            BREAKIF(nResult);
         }         
         
         nResult = ISQLSTMT_GetColumnText(pStmt,13,&pszSummary);                       
         BREAKIF(nResult);
         if(NULL != pszSummary){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_SUMMARY,
                                             pszSummary,
                                             STRLEN(pszSummary),
                                             dwOID,
                                             TRUE,
                                             objType);                        
            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_GetColumnText(pStmt,14,&pszStatus);                      
         BREAKIF(nResult);
         if(NULL != pszStatus){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_STATUS,
                                             pszStatus,
                                             STRLEN(pszStatus),
                                             dwOID,
                                             TRUE,
                                             objType);            
            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_ColumnSize(pStmt,15);
         if(0 != nResult){
            nResult = ISQLSTMT_GetColumnInt(pStmt,15,(int*)&dwPrio);            
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_PRIORITY,
                                             &dwPrio,
                                             sizeof(uint32),
                                             dwOID,
                                             TRUE,
                                             objType);            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_ColumnSize(pStmt,16);
         if(0 != nResult){
            nResult = ISQLSTMT_GetColumnInt(pStmt,16,(int*)&dwPercComp);            
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_PERCENT,
                                             &dwPercComp,
                                             sizeof(uint32),
                                             dwOID,
                                             TRUE,
                                             objType);
            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_GetColumnText(pStmt,17,&pszLocation);
         BREAKIF(nResult);
         if(NULL != pszLocation){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_LOCATION,
                                             pszLocation,
                                             STRLEN(pszLocation),
                                             dwOID,
                                             TRUE,
                                             objType);

           
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_GetColumnText(pStmt,18,&pszGeo);
         BREAKIF(nResult);
         if(NULL != pszGeo){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_GEO,
                                             pszGeo,
                                             STRLEN(pszGeo),
                                             dwOID,
                                             TRUE,
                                             objType);

            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_GetColumnText(pStmt,19,&pszDesc);            
         BREAKIF(nResult);
         if(NULL != pszDesc){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DESCRIPTION,
                                             pszDesc,
                                             STRLEN(pszDesc),
                                             dwOID,
                                             TRUE,
                                             objType);            
            
            BREAKIF(nResult);
         }
                  
         nResult = ISQLSTMT_GetColumnText(pStmt,20,&pszClass);                      
         BREAKIF(nResult);
         if(NULL != pszClass){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_CLASS,
                                             pszClass,
                                             STRLEN(pszClass),
                                             dwOID,
                                             TRUE,
                                             objType);

            
            BREAKIF(nResult);
         }
                  
         nResult = ISQLSTMT_GetColumnText(pStmt, 21, &pszDateTime);                                  
         BREAKIF(nResult);
         if(NULL != pszDateTime){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DTSTAMP,
                                             pszDateTime,
                                             STRLEN(pszDateTime),
                                             dwOID,
                                             TRUE,
                                             objType);  
            
            BREAKIF(nResult);
         }                                                             
         
         nResult = ISQLSTMT_GetColumnText(pStmt,22,&pszUID);            
         BREAKIF(nResult);
         if(NULL != pszUID){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_UID,
                                             pszUID,
                                             STRLEN(pszUID),
                                             dwOID,
                                             TRUE,
                                             objType);
            
            BREAKIF(nResult);
         }   
         
         nResult = ISQLSTMT_GetColumnText(pStmt,23,&pszTZID);            
         BREAKIF(nResult);
         if(NULL != pszTZID){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_TZID,
                                             pszTZID,
                                             STRLEN(pszTZID),
                                             dwOID,
                                             TRUE,
                                             objType);                        
            BREAKIF(nResult);
         }  

         
         nResult = ISQLSTMT_GetColumnText(pStmt,24,&pszDaylight); 
         BREAKIF(nResult);
         if(NULL != pszDaylight){
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pObject,
                                             AEEVPROPERTY_DAYLIGHT,
                                             pszDaylight,
                                             STRLEN(pszDaylight),
                                             dwOID,
                                             TRUE,
                                             objType);                                   
            BREAKIF(nResult);
         }
         
      }          

      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         RELEASEIF(pObject);
         break;
      }
      RELEASEIF(pStmt);
      //Get all the properties which can occur multiple times
      nResult = IvCalStore_GetMultipleProp(pMe,pObject,dwOID,objType); 
      BREAKIF(nResult);                          
      
      nResult = IvCalStore_GetAlarm(pMe,
                                    dwOID,
                                    (IvCalObject*)pObject);
      
      BREAKIF(nResult);
      nResult = IvCalStore_GetTimeZone(pMe,
                                       dwOID,
                                       (IvCalObject*)pObject);


      
      if(AEE_SUCCESS == nResult){          
         *pObj = pObject;
      } else {
         nResult = AEE_EFAILED;
         RELEASEIF(pObject); 
         *pObj = NULL;

      }
   }while(0);
   
   RELEASEIF(pStmt);
   return nResult;
}

/*===========================================================================
Function:  
   CvCalStore_DeleteObjectByOID()

Description:
		 This function deletes the object specified by OID.

Prototype:
	  int CvCalStore_DeleteObjectByOID(IvCalStore *pMe,                                
                                    uint32     dwOID)

Parameters:
   pMe            [in]: Pointer to the IvCalStore interface    
   dwOID          [in]: OID of the record to be deleted.

Return Value:
   Deletes the records for a specific component type.

Comments:  
   None

Side Effects:
   None

==============================================================================*/
int CvCalStore_DeleteObjectByOID(IvCalStore *pMe,                                
                                 uint32     dwOID)
{
   int nResult = AEE_EFAILED;
   
   boolean bLocked = FALSE; 
   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pMe,&bLocked);
   if(AEE_SUCCESS == nResult){
      //if DB is locked the app can't perform the add operation.
      //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
      //then try for addition.
      if(bLocked){
         return AEE_ENOTALLOWED;
      }
   }else{
      return AEE_EFAILED;
   }
   //Check for app privileges.
   if (!ISHELL_CheckPrivLevel(pMe->pIShell, AEEPRIVID_vCalStoreWrite, TRUE)) {
      return AEE_EPRIVLEVEL;
   }
   
   do { 
      nResult = ISQL_Exec ( pMe->pISQL, "BEGIN TRANSACTION", NULL, NULL, NULL ) ;
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      //delete the records from the database
      nResult = IvCalStore_DeleteObject(pMe, dwOID);
      if(AEE_SUCCESS == nResult){
         (void)ISQL_Exec ( pMe->pISQL, "END TRANSACTION", NULL, NULL, NULL ) ;         
         nResult = ISignalBus_Strobe(pMe->pIDeleteSignalBus);
      } else {
         (void)ISQL_Exec ( pMe->pISQL, "ROLLBACK TRANSACTION", NULL, NULL, NULL ); 
      }           
   }while (0); 
   if(AEE_SUCCESS != nResult){
      nResult = AEE_EFAILED;
   }
     
   return nResult;
}

/*===========================================================================
Function:  
   CvCalStore_OnAdd()

Description:
		 This function adds/registers the signal provided by the user to the
   Add signal bus.

Prototype:
	  int  CvCalStore_OnAdd(IvCalStore *pIvCalStore, 
                         ISignal    *pISignal);

Parameters:
   pIvCalStore        [in]: Pointer to IvCalStore Object
   pISignal           [in]: Signal to be added for registration.   

Return Value:
   AEE_SUCCESS : If the signal for Add was added successfully.
   AEE_EFAILED : Otherwise

Comments: 
   None

Side Effects: 
   None

==============================================================================*/

int  CvCalStore_OnAdd(IvCalStore *pIvCalStore, 
                      ISignal    *pISignal)
{
   if(NULL == pISignal){
      return AEE_EBADPARM;
   }

   return ISignalBus_Add(pIvCalStore->pIAddSignalBus, pISignal);

}
/*===========================================================================
Function:  
   CvCalStore_OnUpdate()

Description:
		 This function adds/registers the signal provided by the user to the
   Update signal bus.

Prototype:
	  int  CvCalStore_OnUpdate(IvCalStore *pIvCalStore, 
                            ISignal    *pISignal);

Parameters:
   pIvCalStore        [in]: Pointer to IvCalStore Object
   pISignal           [in]: Signal to be added for registration.   

Return Value:
   AEE_SUCCESS : If the signal for Update was added successfully.
   AEE_EFAILED : Otherwise

Comments: 
   None

Side Effects: 
   None

==============================================================================*/

int  CvCalStore_OnUpdate(IvCalStore *pIvCalStore, 
                         ISignal    *pISignal)
{
   if(NULL == pISignal){
      return AEE_EBADPARM;
   }

   return ISignalBus_Add(pIvCalStore->pIUpdateSignalBus, pISignal);

}
/*===========================================================================
Function:  
   CvCalStore_OnDelete()

Description:
		 This function adds/registers the signal provided by the user to the
   Delete signal bus.

Prototype:
	  int  CvCalStore_OnDelete(IvCalStore *pIvCalStore, 
                            ISignal    *pISignal);

Parameters:
   pIvCalStore        [in]: Pointer to IvCalStore Object
   pISignal           [in]: Signal to be added for registration.   

Return Value:
   AEE_SUCCESS : If the signal for Delete was added successfully.
   AEE_EFAILED : Otherwise

Comments: 
   None

Side Effects: 
   None

==============================================================================*/
int  CvCalStore_OnDelete(IvCalStore *pIvCalStore,                              
                         ISignal    *pISignal)
{
   if(NULL == pISignal){
      return AEE_EBADPARM;
   }
   return ISignalBus_Add(pIvCalStore->pIDeleteSignalBus, pISignal);
}
/*===========================================================================
Function:  
   CvCalStore_OnLock()

Description:
		 This function adds/registers the signal provided by the user to the
   Lock signal bus.

Prototype:
	  int  CvCalStore_OnLock(IvCalStore *pIvCalStore, 
                          ISignal    *pISignal);

Parameters:
   pIvCalStore        [in]: Pointer to IvCalStore Object
   pISignal           [in]: Signal to be added for registration.   

Return Value:
   AEE_SUCCESS : If the signal for lock was added successfully.
   AEE_EFAILED : Otherwise

Comments: 
   None

Side Effects: 
   None

==============================================================================*/
int  CvCalStore_OnLock(IvCalStore *pIvCalStore,                              
                       ISignal    *pISignal)
{
   if(NULL == pISignal){
      return AEE_EBADPARM;
   }
   return ISignalBus_Add(pIvCalStore->pILockSignalBus, pISignal);
}
/*===========================================================================
Function:  
   IvCalStore_AddTimeZoneParms()

Description:
   This function adds the parameters for multi time occuring property.

Prototype:
   int IvCalStore_AddTimeZoneParms(IvCalStore   *pMe, 
                                   IvProperty   *pIvProperty,                                     
                                   uint32       nRecNumProp,
                                   uint32       dwOID);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvProperty     [in]: The pointer to property.
   nRecNumProp     [in]: Record number from Prop table for
                         timezone  
   dwOID           [in]: OID of record from MAIN table

Return Value:
   AEE_SUCCESS   :   If the addition of parms to DB is successful.  
   AEE_EFAILED   :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddTimeZoneParms(IvCalStore   *pMe, 
                                IvProperty   *pIvProperty, 
                                uint32       nRecNumProp,
                                uint32       dwOID)
{
   int nNumParms = 0, nResult = AEE_EFAILED, nCount = 0;
   char *pszStrParm = "INSERT INTO "VCALSTORE_TZPARAM_TBL
      "(RecNumTZProp,ParmID,ParmVal,PropID,UID) VALUES"
      "(?,?,?,?,?)";        
   AECHAR *pwszTempParmVal = NULL;
   ISQLStmt *pStmt = NULL;
   IvParm *pIvParm = NULL;
   uint32 dwPropID = 0;
   if(NULL == pIvProperty){
      return AEE_EBADPARM;
   }
   do{
      //Check for parameters for the property and if params are present 
      //add them to PARM table.
      nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms);
      BREAKIF(nResult);
      nResult = IvProperty_GetId(pIvProperty,&dwPropID);
      if(0 < nNumParms){         
         nResult = ISQL_Prepare(pMe->pISQL, pszStrParm, -1, &pStmt, NULL);         
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         for(nCount = 0; nCount < nNumParms; nCount++){            
            int nParmId = 0;                       
            nResult = IvProperty_GetParmAtIndex(pIvProperty, 
                                                0, 
                                                nCount,
                                                (IvParm**)&pIvParm);
            BREAKIF(nResult);
            nResult = IvParm_GetId(pIvParm, (AEEVProperty*)&nParmId);  
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 1, nRecNumProp);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 2, nParmId);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }

            nResult = IvCalStore_GetParmString(pIvParm,&pwszTempParmVal);
            if(NULL != pwszTempParmVal){               
               nResult = ISQLSTMT_BindWText(pStmt, 3, pwszTempParmVal,-1,NULL);               
            }else {
               nResult = ISQLSTMT_BindNull(pStmt,3);
            }
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt,4, (int)dwPropID);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 5, (int)dwOID);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_Step(pStmt);
            if( SQL_DONE != nResult) {               
               FREEIF(pwszTempParmVal);
               RELEASEIF(pIvParm);                         
               nResult = AEE_EFAILED;               
               break;
            }              
            FREEIF(pwszTempParmVal);
            RELEASEIF(pIvParm);
            (void) ISQLSTMT_Reset(pStmt);
         }
         RELEASEIF(pStmt);
         if((AEE_SUCCESS != nResult)&&(SQL_DONE != nResult)){
            nResult = AEE_EFAILED;
            break;
         }else{
            nResult = AEE_SUCCESS;
         }
      }

   }while(0);
   FREEIF(pwszTempParmVal);
   RELEASEIF(pIvParm);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_AddTZMultiProp()

Description:
   This function stores the timezone properties which occur more than once.

Prototype:
   int IvCalStore_AddTZMultiProp(IvCalStore   *pMe,
                                 AEEVProperty dwPropID,
                                 uint32       dwDaylightRecNum,
                                 IvProperty   *pIvProperty,
                                 uint32       dwOID,
                                 boolean      bTZorDaylight);

Parameters:
   pMe:             [in]: Pointer to the IvCalStore interface 
   dwPropID         [in]: Property ID   
   dwDaylightRecNum [in]: Record Number from Daylight table
   pIvProperty      [in]: Pointer to property
   dwOID            [in]: OID of record from MAIN table.
   bTZorDaylight    [in]: Whether from Timezone table or Daylight

Return Value:
   AEE_SUCCESS       :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddTZMultiProp(IvCalStore   *pMe,
                              AEEVProperty dwPropID,
                              uint32       dwDaylightRecNum,
                              IvProperty   *pIvProperty,
                              uint32       dwOID,
                              boolean      bTZorDaylight)
{
   int nResult = AEE_EFAILED, nSize = 0;
   char *pszPropStr = "INSERT INTO "VCALSTORE_TZPROP_TBL
                   "(PropID,PropVal,RecNumDayLight,UID) VALUES (?,?,?,?)";
   char *pszPropStrTZ = "INSERT INTO "VCALSTORE_TZPROP_TBL
                   "(PropID,PropVal,RecNumTimeZone,UID) VALUES (?,?,?,?)";
   ISQLStmt *pStmt = NULL;
   char *pszProp = NULL, *pszPropName = NULL;
   uint32 dwRecNumProp = 0;
   if(NULL == pIvProperty){
      return AEE_EBADPARM;
   }
   do{
      if(bTZorDaylight){
         nResult = ISQL_Prepare(pMe->pISQL, pszPropStrTZ, -1, &pStmt, NULL);
      }else{
         nResult = ISQL_Prepare(pMe->pISQL, pszPropStr, -1, &pStmt, NULL);
      }  
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwPropID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = IvProperty_ParseValue(pIvProperty,
                                       FALSE,
                                       NULL,
                                       0,
                                       &nSize);
      pszProp = (char*)MALLOC(nSize);
      if(NULL == pszProp){
         nResult = AEE_ENOMEMORY;
         break;
      }
      nResult = IvProperty_ParseValue(pIvProperty,
                                       FALSE,
                                       pszProp,
                                       nSize,
                                       &nSize);
      BREAKIF(nResult);

      if( AEEVPROPERTY_EXTENDED == dwPropID ){
         char *pszTemp = NULL;
         nResult = IvProperty_GetPropertyName(pIvProperty,
                                              NULL,
                                              nSize,
                                              &nSize);
         BREAKIF(nResult);
         pszPropName = (char*)MALLOC(nSize);
         if(NULL == pszPropName){
            nResult = AEE_ENOMEMORY;
            break;
         }
         nResult = IvProperty_GetPropertyName(pIvProperty,
                                              pszPropName,
                                              nSize,
                                              &nSize);
         BREAKIF(nResult);

         pszTemp = pszProp;
         pszProp = IvCalStore_AppendStr(pszPropName, pszProp, ':');
         FREEIF(pszTemp);
         if(NULL == pszProp){
            nResult = AEE_ENOMEMORY;
            break;
         }         
      }
      nResult = ISQLSTMT_BindText(pStmt, 2, pszProp,-1,NULL);      
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 3, (int)dwDaylightRecNum);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 4, (int)dwOID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
                        
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){
         nResult = AEE_EFAILED;
         break;
      }else{
         nResult = AEE_SUCCESS;
      }
      RELEASEIF(pStmt);
      dwRecNumProp = (uint32)ISQL_GetLastRowId(pMe->pISQL);  
      
      nResult = IvCalStore_AddTimeZoneParms(pMe,
                                            pIvProperty,
                                            dwRecNumProp,
                                            dwOID);


   }while(0);   
   FREEIF(pszProp); 
   FREEIF(pszPropName);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleMultiTimeZoneProp()

Description:
   This function stores the timezone properties that can occur more than
   once.

Prototype:
   int  IvCalStore_HandleMultiTimeZoneProp(IvCalStore  *pMe,
                                           IvCalObject *pSubComponents,
                                           uint32      dwDaylightRecNum,
                                           uint32      dwOID);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pSubComponents  [in]: The pointer to daylight or standard timezone 
                         sub component object.   
   dwDaylightRecNum[in]: Record Number from Daylight table
   dwOID           [in]: OID of the record from MAIN table
   

Return Value:
   AEE_SUCCESS   :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleMultiTimeZoneProp(IvCalStore  *pMe,
                                       IvCalObject *pSubComponents,
                                       uint32      dwDaylightRecNum,
                                       uint32      dwOID)
{
   int nResult = AEE_EFAILED, nNumOfProps = 0, nCount = 0 ;   
   IvProperty *pIvProperty = NULL;
   do{
      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_COMMENT,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_COMMENT,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_COMMENT,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);                                                
         }
         BREAKIF(nResult);
      }      

      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_RRULE,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_RRULE,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_RRULE,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
                                                
         }
         BREAKIF(nResult);
      }

      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_TZNAME,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_TZNAME,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_TZNAME,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
                                                
         }
         BREAKIF(nResult);
      }

      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_EXTENDED,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_EXTENDED,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_EXTENDED,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
                                                
         }
         BREAKIF(nResult);
      }

      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_RDATE,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_RDATE,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_RDATE,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);            
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
                                                
         }
         BREAKIF(nResult);
      }

      nResult = IvCalObject_GetNumberOfProperties(pSubComponents,
                                                  AEEVPROPERTY_EXDATE,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                                     AEEVPROPERTY_EXDATE,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult); 
            if(NULL == pIvProperty){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_AddTZMultiProp(pMe,
                                                AEEVPROPERTY_EXDATE,
                                                dwDaylightRecNum,
                                                pIvProperty,
                                                dwOID,
                                                FALSE);            
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
                                                
         }
         BREAKIF(nResult);
      }

   }while(0);
   RELEASEIF(pIvProperty);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_AddDayLightParams()

Description:
   This function stores the daylight and standard timezone component
   parameters in timezone param table.

Prototype:
   int IvCalStore_AddDayLightParams(IvCalStore  *pMe,
                                    IvCalobject *pSubComponents,
                                    uint32      dwOID,
                                    uint32      dwDayLightRecNum);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pSubComponents  [in]: The pointer to daylight or standard timezone 
                         sub component object.      
   dwOID           [in]: OID of the record from MAIN table
   dwDayLightRecNum[in]: DayLight table record number

Return Value:
   AEE_SUCCESS   :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddDayLightParams(IvCalStore  *pMe,
                                 IvCalObject *pSubComponents,
                                 uint32      dwOID,
                                 uint32      dwDayLightRecNum)
{
   int nResult  = AEE_EFAILED;
   IvProperty *pIvProperty = NULL;
   do{
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_DTSTART,
                                               0,
                                               &pIvProperty);      
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp( pMe,
                                                pIvProperty,
                                                dwDayLightRecNum,
                                                dwOID,
                                                AEECLSID_vTimeZone,
                                                TRUE,
                                                0);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_TZOFFSETFROM,
                                               0,
                                               &pIvProperty);      
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp(pMe,
                                               pIvProperty,
                                               dwDayLightRecNum,
                                               dwOID,
                                               AEECLSID_vTimeZone,
                                               TRUE,
                                               0);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_TZOFFSETTO,
                                               0,
                                               &pIvProperty);      
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp(pMe,
                                               pIvProperty,
                                               dwDayLightRecNum,
                                               dwOID,
                                               AEECLSID_vTimeZone,
                                               TRUE,
                                               0);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }

   }while(0);
   RELEASEIF(pIvProperty);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_AddStandardDaylight()

Description:
   This function stores the daylight and standard timezone component.

Prototype:
   int  IvCalStore_AddStandardDaylight(IvCalStore  *pMe,
                                       IvCalObject *pSubComponents,
                                       uint32      dwRecNum,
                                       AEECLSID    dwType,
                                       uint32      dwOID);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pSubComponents  [in]: The pointer to daylight or standard timezone 
                         sub component object.   
   dwRecNum        [in]: Record Number from Main table
   dwType          [in]: Whether standard or daylight
   dwOID           [in]: OID of the record from MAIN table

Return Value:
   AEE_SUCCESS   :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int  IvCalStore_AddStandardDaylight(IvCalStore  *pMe,
                                    IvCalObject *pSubComponents,
                                    uint32      dwRecNum,
                                    AEECLSID    dwType,
                                    uint32      dwOID)
{
   int nResult = AEE_EFAILED, nSize = 0;
   char *pszSubComponent = "INSERT INTO "VCALSTORE_DAYLIGHT_TBL
                           "(Type,DateStart,TZOffsetFrom,TZOffsetTo,"
                           "RecNumTimeZone,UID)"
                           " VALUES (?,?,?,?,?,?)";
   char *pszDtStart = NULL;
   IvProperty *pIvProperty = NULL;   
   ISQLStmt *pStmt = NULL;
   int32 nTZOffset = 0;    
   uint32 dwDaylightRecNum = 0;

   do{
      nResult = ISQL_Prepare(pMe->pISQL, pszSubComponent, -1, &pStmt, NULL);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      //Get the DTSTART property
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_DTSTART,
                                               0,
                                               &pIvProperty);      
      BREAKIF(nResult); 
      if(NULL == pIvProperty){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt,1,(int)dwType);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      //Parse/get the DTSTART property value 
      nResult = IvProperty_ParseValue( pIvProperty, 
                                       FALSE,
                                       NULL,
                                       0, 
                                       &nSize);
      BREAKIF(nResult);               
      pszDtStart = (char*)MALLOC(nSize);
      if(NULL == pszDtStart){
         nResult = AEE_ENOMEMORY;
         break;
      }                          
      nResult = IvProperty_ParseValue( pIvProperty, 
                                       FALSE,
                                       pszDtStart,
                                       nSize, 
                                       &nSize);
      RELEASEIF(pIvProperty);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindText(pStmt, 2, pszDtStart, -1, NULL);            
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      
      //Get the TZOFFSETFROM property
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_TZOFFSETFROM,
                                               0,
                                               &pIvProperty);  
      BREAKIF(nResult);
      if(NULL == pIvProperty){
         nResult = AEE_EFAILED;
         break;
      }

      //Get the TZOFFSETFROM property value
      nResult = IvProperty_ParseValue(pIvProperty,
                                      TRUE,
                                      &nTZOffset,
                                      sizeof(int32),
                                      &nSize);
      RELEASEIF(pIvProperty);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt,3,nTZOffset);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nTZOffset = 0;
      //Get the TZOFFSETTO property
      nResult = IvCalObject_GetPropertyAtIndex(pSubComponents,
                                               AEEVPROPERTY_TZOFFSETTO,
                                               0,
                                               &pIvProperty);  
      BREAKIF(nResult);
      if(NULL == pIvProperty){
         nResult = AEE_EFAILED;
         break;
      }
      //Get the TZOFFSETTO property value
      nResult = IvProperty_ParseValue(pIvProperty,
                                      TRUE,
                                      &nTZOffset,
                                      sizeof(int32),
                                      &nSize);
      RELEASEIF(pIvProperty);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 4, nTZOffset);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 5, (int)dwRecNum);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 6, (int)dwOID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      RELEASEIF(pStmt);
      ///see from here
      dwDaylightRecNum = (int)ISQL_GetLastRowId(pMe->pISQL); 

      //Add the parameters related to the above added properties
      nResult = IvCalStore_AddDayLightParams(pMe,
                                             pSubComponents,
                                             dwOID,
                                             dwDaylightRecNum);
      BREAKIF(nResult);

      //Add all the properties which can occur more than once.                 
      nResult = IvCalStore_HandleMultiTimeZoneProp(pMe,
                                                   pSubComponents,
                                                   dwDaylightRecNum,
                                                   dwOID);
      BREAKIF(nResult);
                                               
   }while(0);
   FREEIF(pszDtStart);
   RELEASEIF(pIvProperty);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalObject_AddTimeZoneParams()

Description:
   This function stores the timezone related parameters in Timezone parm
   table.

Prototype:
   int IvCalObject_AddTimeZoneParams(IvCalStore  *pMe,
                                     IvCalObject *pObject,
                                     uint32      dwOID,
                                     uint32      dwTimeZoneRecNum);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pObject         [in]: The pointer to timezone object.  
   dwOID           [in]: Object ID
   dwTimeZoneRecNum[in]: Record Number from TimeZone table.


Return Value:
   AEE_SUCCESS   :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalObject_AddTimeZoneParams(IvCalStore  *pMe,
                                  IvCalObject *pObject,
                                  uint32      dwOID,
                                  uint32      dwTimeZoneRecNum)
{
   int nResult = AEE_EFAILED;
   IvProperty *pIvProperty = NULL;
   do{
      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_TZID,
                                               0,
                                               &pIvProperty);
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp(pMe,
                                                pIvProperty,
                                                dwTimeZoneRecNum,
                                                dwOID,
                                                AEECLSID_vTimeZone,
                                                FALSE,
                                                0);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
      
      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_TZURL,
                                               0,
                                               &pIvProperty);
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp(pMe,
                                               pIvProperty,
                                               dwTimeZoneRecNum,
                                               dwOID,
                                               AEECLSID_vTimeZone,
                                               FALSE,
                                               0);
      }
      RELEASEIF(pIvProperty);
      BREAKIF(nResult);
      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_LASTMODIFIED,
                                               0,
                                               &pIvProperty);
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvCalStore_HandleSingleProp(pMe,
                                               pIvProperty,
                                               dwTimeZoneRecNum,
                                               dwOID,
                                               AEECLSID_vTimeZone,
                                               FALSE,
                                               0);
      }      
      BREAKIF(nResult);
      
   }while(0);
   RELEASEIF(pIvProperty);
   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_AddTimeZone()

Description:
   This function stores the timezone component.

Prototype:
   int IvCalStore_AddTimeZone(IvCalStore  *pMe,
                              IvCalObject *pObject);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pObject         [in]: The pointer to timezone object.   

Return Value:
   AEE_SUCCESS   :   If the addition is successful.
   AEE_ENOMEMORY     :   If no memory
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddTimeZone(IvCalStore  *pMe,
                           IvCalObject *pObject,
                           uint32      dwOID)
{
   int nResult = AEE_EFAILED, nCount = 0, nSize = 0, nNumOfProps = 0;
   char *pszTimeZone = "INSERT INTO "VCALSTORE_TIMEZONE_TBL
                       "(TZID,TZURL,LastModified,UID) VALUES (?,?,?,?)";
   ISQLStmt *pStmt = NULL;
   IvCalObject *pSubComponents = NULL;
   IvProperty *pIvProperty = NULL;
   char *pszTZID = NULL, *pszTZURL = NULL, *pszLastModified = NULL;
   uint32 dwRecNum = 0;      
   int nNumObjects = 0;
   if(NULL == pObject){
      return AEE_EBADPARM;
   }
   do{      
      nResult = ISQL_Prepare(pMe->pISQL,pszTimeZone,-1,&pStmt,NULL);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = ISQLSTMT_BindInt(pStmt,4,(int)dwOID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }

      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_TZID,
                                               0,
                                               &pIvProperty);
      BREAKIF(nResult);
      if(NULL != pIvProperty){
         nResult = IvProperty_ParseValue(pIvProperty, 
                                         FALSE, 
                                         NULL, 
                                         0, 
                                         &nSize);
         BREAKIF(nResult);
         pszTZID = (char*)MALLOC(nSize);
         if(NULL == pszTZID){
            nResult = AEE_ENOMEMORY;
            break;
         }
         nResult = IvProperty_ParseValue( pIvProperty, 
                                          FALSE,
                                          pszTZID,
                                          nSize, 
                                          &nSize);
         BREAKIF(nResult);
         nResult = ISQLSTMT_BindText(pStmt, 1, pszTZID, -1, NULL);
         RELEASEIF(pIvProperty);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
      }
      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_TZURL,
                                               0,
                                               &pIvProperty);
      BREAKIF(nResult);
      if(NULL != pIvProperty ){
         nResult = IvProperty_ParseValue(pIvProperty, 
                                         FALSE,
                                         NULL, 
                                         0, 
                                         &nSize);
         BREAKIF(nResult);
         pszTZURL = (char*)MALLOC(nSize);
         if(NULL == pszTZURL){
            nResult = AEE_ENOMEMORY;
            break;
         }
         nResult = IvProperty_ParseValue( pIvProperty, 
                                          FALSE,
                                          pszTZURL,
                                          nSize, 
                                          &nSize);
         BREAKIF(nResult);
         nResult = ISQLSTMT_BindText(pStmt, 2, pszTZURL, -1, NULL); 
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }      
      nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                               AEEVPROPERTY_LASTMODIFIED,
                                               0,
                                               &pIvProperty);      
      if(NULL != pIvProperty && AEE_SUCCESS == nResult){         
         nResult = IvProperty_ParseValue(pIvProperty, 
                                         FALSE,
                                         NULL, 
                                         0, 
                                         &nSize);
         BREAKIF(nResult);
         pszLastModified = (char*)MALLOC(nSize);
         if(NULL == pszLastModified){
            nResult = AEE_ENOMEMORY;
            break;
         }
         nResult = IvProperty_ParseValue( pIvProperty, 
                                          FALSE,
                                          pszLastModified,
                                          nSize, 
                                          &nSize);
         BREAKIF(nResult);
         nResult = ISQLSTMT_BindText(pStmt, 3, pszLastModified, -1, NULL); 
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
      
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){
         nResult = AEE_EFAILED;
         break;
      }   
      RELEASEIF(pStmt);
      dwRecNum = (uint32)ISQL_GetLastRowId(pMe->pISQL);

      nResult = IvCalObject_GetNumberOfProperties(pObject,
                                                  AEEVPROPERTY_EXTENDED,
                                                  &nNumOfProps);
      BREAKIF(nResult);
      if(0 < nNumOfProps){
         for (nCount = 0; nCount < nNumOfProps; nCount++ ){
            nResult = IvCalObject_GetPropertyAtIndex(pObject,
                                                     AEEVPROPERTY_EXTENDED,
                                                     nCount,
                                                     &pIvProperty);
            BREAKIF(nResult);
            if(NULL != pIvProperty){
               nResult = IvCalStore_AddTZMultiProp(pMe,
                                                   AEEVPROPERTY_EXTENDED,
                                                   dwRecNum,
                                                   pIvProperty,
                                                   dwOID,
                                                   TRUE);
               RELEASEIF(pIvProperty);
               BREAKIF(nResult);
            }else{
               nResult = AEE_EFAILED;
               break;
            }
            
                                                
         }
         BREAKIF(nResult);
      }


      nResult = IvCalObject_AddTimeZoneParams(pMe, pObject, dwOID, dwRecNum);
      BREAKIF(nResult);
      nResult = IvCalObject_GetNumberOfObjects(pObject,
                                               AEECLSID_vStandard,
                                               &nNumObjects);
      //Check for any daylight or standard sub component.if present
      //add them to the Daylight table.

      if(0 < nNumObjects){
         for (nCount = 0; nCount < nNumObjects; nCount++ ){
            nResult = IvCalObject_GetObjectAtIndex(pObject, 
                                                   nCount, 
                                                   AEECLSID_vStandard,
                                                   &pSubComponents);
            BREAKIF(nResult);
            nResult = IvCalStore_AddStandardDaylight(pMe,
                                                      pSubComponents,
                                                      dwRecNum,
                                                      AEECLSID_vStandard,
                                                      dwOID);            
            RELEASEIF(pSubComponents);
            BREAKIF(nResult);
         }
      }
      BREAKIF(nResult);

      nNumObjects = 0;
      nResult = IvCalObject_GetNumberOfObjects(pObject,
                                               AEECLSID_vDaylight,
                                               &nNumObjects);
      if(0 < nNumObjects){
         for (nCount = 0; nCount < nNumObjects; nCount++ ){
            nResult = IvCalObject_GetObjectAtIndex(pObject, 
                                                   nCount, 
                                                   AEECLSID_vDaylight,
                                                   &pSubComponents);
            BREAKIF(nResult);
            nResult = IvCalStore_AddStandardDaylight(pMe,
                                                     pSubComponents,
                                                     dwRecNum,
                                                     AEECLSID_vDaylight,
                                                     dwOID);
            RELEASEIF(pSubComponents);
            BREAKIF(nResult);
         }
      }      

   }while(0);
   if(AEE_SUCCESS != nResult){
      nResult = AEE_EFAILED;   
   }
   FREEIF(pszTZID);
   FREEIF(pszTZURL);
   FREEIF(pszLastModified);
   RELEASEIF(pIvProperty);
   RELEASEIF(pStmt);
   return nResult;

}

/*===========================================================================
Function:  
   IvCalStore_AddMultiPropParams()

Description:
   This function adds the parameters for multi time occuring property.

Prototype:
   int IvCalStore_AddMultiPropParams(IvCalStore   *pMe, 
                                     IvProperty   *pIvProperty,
                                     boolean      bAlarmProp,
                                     uint32       UID,
                                     AEEVProperty nProp,
                                     int          nRecNumProp);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvProperty     [in]: The pointer to property.
   bAlarmProp      [in]: Whether the property is for alarm component
   UID             [in]: unique ID for the record
   nProp           [in]: Property ID
   nRecNumProp     [in]: Record Number of the property table


Return Value:
   AEE_SUCCESS   :   If the addition of parms to DB is successful.  
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddMultiPropParams(IvCalStore   *pMe, 
                                  IvProperty   *pIvProperty,
                                  boolean      bAlarmProp,
                                  uint32       UID,
                                  AEEVProperty nProp,
                                  int          nRecNumProp)
{
   int nNumParms = 0, nResult = AEE_EFAILED, nCount = 0;
   char *pszStrParm = "INSERT INTO "VCALSTORE_PARAM_TBL
      "(UID,PropID,ParmID,ParmVal,RecNumProp) VALUES"
      "(?,?,?,?,?)";     
   char *pszStrAlarmParm = "INSERT INTO "VCALSTORE_ALARMPARAM_TBL
      "(AlarmOID,PropID,ParmID,ParmVal,RecNumProp) VALUES"
      "(?,?,?,?,?)";
   AECHAR *pwszTempParmVal = NULL;
   ISQLStmt *pStmt = NULL;
   IvParm *pIvParm = NULL;
   do{
      //Check for parameters for the property and if params are present 
      //add them to PARM table.
      nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms);
      BREAKIF(nResult);
      if(0 < nNumParms){
         if(FALSE == bAlarmProp){
            nResult = ISQL_Prepare(pMe->pISQL, pszStrParm, -1, &pStmt, NULL);
         }else{
            nResult = ISQL_Prepare(pMe->pISQL, pszStrAlarmParm, -1, &pStmt, NULL);
         }
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         for(nCount = 0; nCount < nNumParms; nCount++){            
            int nParmId = 0;                       
            nResult = IvProperty_GetParmAtIndex(pIvProperty, 0, nCount,(IvParm**)&pIvParm);
            BREAKIF(nResult);
            nResult = IvParm_GetId(pIvParm, (AEEVParameter*)&nParmId);  
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 1, (int)UID);
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 2, nProp);
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 3, nParmId);
            BREAKIF(nResult);            

            nResult = IvCalStore_GetParmString(pIvParm,&pwszTempParmVal);
            if(NULL != pwszTempParmVal){               
               nResult = ISQLSTMT_BindWText(pStmt, 4, pwszTempParmVal,-1,NULL);               
            }else {
               nResult = ISQLSTMT_BindNull(pStmt,4);
            }
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt,5,nRecNumProp);
            if(AEE_SUCCESS != nResult){
               FREEIF(pwszTempParmVal);
               RELEASEIF(pIvParm);
               break;
            }            
            nResult = ISQLSTMT_Step(pStmt);
            if( SQL_DONE != nResult) {               
               FREEIF(pwszTempParmVal);
               RELEASEIF(pIvParm);                         
               nResult = AEE_EFAILED;               
               break;
            }              
            FREEIF(pwszTempParmVal);
            RELEASEIF(pIvParm);
            (void)ISQLSTMT_Reset(pStmt);
         }
         RELEASEIF(pStmt);
         if((AEE_SUCCESS != nResult)&&(SQL_DONE != nResult)){
            nResult = AEE_EFAILED;
            RELEASEIF(pIvParm);
            RELEASEIF(pStmt);
            break;
         }else{
            nResult = AEE_SUCCESS;
         }
      }
   }while(0);
   RELEASEIF(pIvParm);
   FREEIF(pwszTempParmVal);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleMultiProp()

Description:
   This function handles inserting the properties in PROP table
   which can occur more than once in a calendar component.

Prototype:
	 int IvCalStore_HandleMultiProp(IvCalStore *pMe,
                                 IvProperty *pIvProperty,
                                 uint32 UID,
                                 boolean bAlarmProp,
                                 uint32  dwRecNum);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvProperty     [in]: The pointer to property.
   UID             [in]: UID  
   bAlarmProp      [in]: Whether Alarm Extended property
                         If TRUE it is alarm property otherwise
                         Event, ToDo or journal property.

Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   SQL returned ERROR in case of failure

Comments:  
   This function adds properties which can occur more than once in a 
   component. These properties are added in PROP and PARM tables.
   The Properties are added in the PROP table and for each property 
   multiple params can be added in PARM table.   

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleMultiProp(IvCalStore *pMe,
                               IvProperty *pIvProperty,
                               uint32     UID,
                               boolean    bAlarmProp,
                               uint32     dwRecNum)
{   
   char *pszStrProp = "INSERT INTO "VCALSTORE_PROP_TBL
                      "(UID,PropID,PropVal) VALUES"
                      "(?,?,?)";   
   char *pszStrAlarmProp = "INSERT INTO "VCALSTORE_ALARMPROP_TBL
                           "(AlarmOID,PropID,PropVal,RecNumAlarmTable) VALUES"
                           "(?,?,?,?)";   
   ISQLStmt *pStmt = NULL;
   int nResult = AEE_EFAILED, nProp = 0, nSize = 0;
   int nRecNumProp = 0;           
   char *pszProp = NULL, *pszPropName = NULL;      

   if(NULL == pIvProperty){     
      return AEE_EBADPARM;
   }
   do{
      nResult = IvProperty_GetId(pIvProperty, (AEEVProperty*)&nProp);
      BREAKIF(nResult);
      if(FALSE == bAlarmProp){
         nResult = ISQL_Prepare(pMe->pISQL, pszStrProp, -1, &pStmt, NULL);         
      }else{
         nResult = ISQL_Prepare(pMe->pISQL, pszStrAlarmProp, -1, &pStmt, NULL);
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         nResult = ISQLSTMT_BindInt(pStmt, 4, (int)dwRecNum);
      }
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)UID);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 2, nProp);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = IvProperty_ParseValue(pIvProperty,
                                      FALSE,
                                      NULL,
                                      0,
                                      &nSize);
      BREAKIF(nResult);      
      pszProp = (char*)MALLOC(nSize);
      if(NULL == pszProp){
         nResult = AEE_ENOMEMORY;
         break;
      }
      nResult = IvProperty_ParseValue(pIvProperty,
                                       FALSE,
                                       pszProp,
                                       nSize,
                                       &nSize);
      BREAKIF(nResult);

      if( AEEVPROPERTY_EXTENDED == nProp ){
         char *pszTemp = NULL;
         nResult = IvProperty_GetPropertyName(pIvProperty,
                                              NULL,
                                              nSize,
                                              &nSize);
         BREAKIF(nResult);
         pszPropName = (char*)MALLOC(nSize);
         if(NULL == pszPropName){
            nResult = AEE_ENOMEMORY;
            break;
         }
         nResult = IvProperty_GetPropertyName(pIvProperty,
                                              pszPropName,
                                              nSize,
                                              &nSize);
         BREAKIF(nResult);
         pszTemp = pszProp;
         pszProp = IvCalStore_AppendStr(pszPropName, pszProp, ':');
         FREEIF(pszTemp);
         if(NULL == pszProp){
            nResult = AEE_ENOMEMORY;
            break;
         }
         FREEIF(pszTemp);

      }
      nResult = ISQLSTMT_BindText(pStmt, 3, pszProp,-1,NULL);
      if(AEE_SUCCESS != nResult){
         nResult = AEE_EFAILED;
         break;
      }
                          
      nResult = ISQLSTMT_Step(pStmt);
      if( SQL_DONE != nResult) {
         nResult = AEE_EFAILED;
         break;
      }           
      FREEIF(pszProp);
      RELEASEIF(pStmt);
      nRecNumProp = (int)ISQL_GetLastRowId(pMe->pISQL);      

      //Check for parameters for the property and if params are present 
      //add them to PARM table.
      
      nResult = IvCalStore_AddMultiPropParams(pMe, 
                                              pIvProperty,
                                              bAlarmProp,
                                              UID,
                                              nProp,
                                              nRecNumProp);
                                              

      
     
   }while(0);
   
   FREEIF(pszProp);
   FREEIF(pszPropName);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleSingleProp()

Description:
   This function handles inserting the properties in PROP table
   which can occur once in a calendar component.

Prototype:
   int IvCalStore_HandleSingleProp(IvCalStore *pMe,
                                   IvProperty *pIvProperty,
                                   uint32     UID,
                                   uint32     dwOID,
                                   AEECLSID   objType,
                                   boolean    bDayLight,
                                   uint32     dwRecNum);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvProperty     [in]: The pointer to property.
   UID             [in]: recordnumber for a particular table 
   dwOID           [in]: Object ID for the event, todo etc record
   objType         [in]: object type
   bDayLight       [in]: Whether daylight or time zone table
   dwRecNum        [in]: Record number from Alarm table

Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   SQL returned ERROR in case of failure

Comments:  
   This function stores all the parameters in PARM table for a property 
   which occurs once in a component. The property value would already be 
   stored in MAIN table.

Side Effects: 
   None

==============================================================================*/

int IvCalStore_HandleSingleProp(IvCalStore *pMe,
                                IvProperty *pIvProperty,
                                uint32     UID,
                                uint32     dwOID,
                                AEECLSID   objType,
                                boolean    bDayLight,
                                uint32     dwRecNum)
{
   char *pszStrParm = "INSERT INTO "VCALSTORE_PARAM_TBL
                      "(UID,PropID,ParmID,ParmVal) VALUES"
                      "(?,?,?,?)";   
   char *pszStrAlarmParm = "INSERT INTO "VCALSTORE_ALARMPARAM_TBL
                           "(AlarmOID,PropID,ParmID,ParmVal,RecNumAlarmTable) VALUES"
                           "(?,?,?,?,?)";
   char *pszStrTimeZoneParm = "INSERT INTO "VCALSTORE_TZPARAM_TBL
                           "(RecNumTZTable,PropID,ParmID,ParmVal,UID) VALUES"
                           "(?,?,?,?,?)";
   char *pszStrDaylightParm = "INSERT INTO "VCALSTORE_TZPARAM_TBL
                           "(RecNumDayLightTable,PropID,ParmID,ParmVal,UID) VALUES"
                           "(?,?,?,?,?)";
   int nNumParms = 0, nCount = 0;
   IvParm *pIvParm = NULL;
   ISQLStmt *pStmt = NULL;
   int nResult = AEE_EFAILED, nProp = 0;  
   AECHAR *pwszTempParmVal = NULL;
   if(NULL == pIvProperty){
      return AEE_EBADPARM;
   }
   do{
      nResult = IvProperty_GetId(pIvProperty, (AEEVProperty*)&nProp);
      BREAKIF(nResult);
      nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms); 
      BREAKIF(nResult);
      if(0 < nNumParms){
         //Check if Alarm parameters
         if(AEECLSID_vAlarm == objType){
            nResult = ISQL_Prepare(pMe->pISQL, pszStrAlarmParm, -1, &pStmt, NULL);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 5, (int)dwRecNum);
            //Check if time zone related parameters
         }else if(AEECLSID_vTimeZone == objType){
            //if time zone related parameters , check for daylight or timezone 
            //table property parameters
            if(bDayLight){
               nResult = ISQL_Prepare(pMe->pISQL, pszStrDaylightParm, -1, &pStmt, NULL);
            }else {
               nResult = ISQL_Prepare(pMe->pISQL, pszStrTimeZoneParm, -1, &pStmt, NULL);
            }
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 5, (int)dwOID);
         }else {
            nResult = ISQL_Prepare(pMe->pISQL, pszStrParm, -1, &pStmt, NULL);
         }
         if(AEE_SUCCESS != nResult){
            nResult = AEE_EFAILED;
            break;
         }
         for(nCount = 0; nCount < nNumParms; nCount++){            
            int nParmId = 0;                  
            nResult = IvProperty_GetParmAtIndex(pIvProperty, 
                                                0, 
                                                nCount,
                                                (IvParm**)&pIvParm);
            BREAKIF(nResult);
            nResult = IvParm_GetId(pIvParm, (AEEVProperty*)&nParmId); 
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 1, (int)UID);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 2, nProp);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = ISQLSTMT_BindInt(pStmt, 3, nParmId);
            if(AEE_SUCCESS != nResult){
               nResult = AEE_EFAILED;
               break;
            }
            nResult = IvCalStore_GetParmString(pIvParm,&pwszTempParmVal);
            BREAKIF(nResult);                        
            if(NULL != pwszTempParmVal){               
               nResult = ISQLSTMT_BindWText(pStmt, 4, pwszTempParmVal,-1,NULL);               
            }else {
               nResult = ISQLSTMT_BindNull(pStmt,4);
            }            
            if(SUCCESS != nResult){                              
               break;
            }                
            nResult = ISQLSTMT_Step(pStmt);
            if( SQL_DONE != nResult) {
               nResult = AEE_EFAILED;                           
               RELEASEIF(pStmt);
               break;
            } 
            FREEIF(pwszTempParmVal);
            (void) ISQLSTMT_Reset(pStmt);
            RELEASEIF(pIvParm);
         } 
         if((AEE_SUCCESS != nResult)&&(SQL_DONE != nResult)){            
            RELEASEIF(pIvParm);
            break;
         } else {
            nResult = AEE_SUCCESS;
         }
      }else {
         nResult = AEE_SUCCESS;
      }
   }while(0);
   FREEIF(pwszTempParmVal);
   RELEASEIF(pIvParm);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_AddSingleAlarmParms()

Description:
   This function handles inserting the alarm parms in ALARM PARM table
  

Prototype:
	int IvCalStore_AddSingleAlarmParms(IvCalStore  *pMe,
                                      IvCalObject *pIvObject,
                                      uint32      dwRecNum,
                                      uint32      UID);
Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvObject       [in]: The pointer to alarm component object.
   dwRecNum        [in]: Record number 
   UID             [in]: Unique record number from Main table
   

Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   SQL returned ERROR in case of failure

Comments:  
   This function adds the parms for Alarm subcomponent in an Event,
   Todo or Journal.

Side Effects: 
   None

==============================================================================*/

int IvCalStore_AddSingleAlarmParms(IvCalStore  *pMe,
                                   IvCalObject *pIvObject,
                                   uint32      dwRecNum,
                                   uint32      UID)
{
   IvProperty *pIvProperty = NULL;   
   int nResult = AEE_EFAILED, nCount = 0, nNumProp = 0, nNumParms = 0;
   uint32 dwProp = 0;
   do{
      nResult = IvCalObject_GetNumberOfProperties(pIvObject, 0, &nNumProp);  
      BREAKIF(nResult);
      for(nCount =0; nCount < nNumProp; nCount++){         
         nResult = IvCalObject_GetPropertyAtIndex(pIvObject, 0, nCount,&pIvProperty);         
         BREAKIF(nResult);
         if(NULL == pIvProperty){
            nResult = AEE_EFAILED;
            break;
         }
         nResult = IvProperty_GetId(pIvProperty, &dwProp);
         BREAKIF(nResult);
         switch(dwProp){
            case AEEVPROPERTY_SUMMARY:
               {                  
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                                        0,
                                                        &nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe, 
                                                           pIvProperty, 
                                                           UID,
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           FALSE,
                                                           dwRecNum);
                  }
                  break;             
               }
            case AEEVPROPERTY_DESCRIPTION:
               {               
                  
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                                        0,
                                                        &nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe, 
                                                           pIvProperty,
                                                           UID,
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           FALSE,
                                                           dwRecNum);
                  }

                  break;

               }                           
            case AEEVPROPERTY_ACTION:
               {
                  
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                                        0,
                                                        &nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe, 
                                                           pIvProperty,
                                                           UID,
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           FALSE,
                                                           dwRecNum);
                  }

                  break;               
               }

            case AEEVPROPERTY_DURATION:
               {                                                   
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                                        0,
                                                        &nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe,
                                                           pIvProperty, 
                                                           UID,
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           TRUE,
                                                           dwRecNum);
                  }
                  break;                 
               }

            case AEEVPROPERTY_REPEAT:
               {
                                    
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                                        0,
                                                        &nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe,
                                                           pIvProperty, 
                                                           UID,
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           TRUE,
                                                           dwRecNum);
                  }
                  break;
           

               }
            case AEEVPROPERTY_TRIGGER:
               
               {                                    
                  nResult = IvProperty_GetNumberOfParms(pIvProperty,0,&nNumParms);
                  BREAKIF(nResult);
                  if(0 < nNumParms){
                     nResult = IvCalStore_HandleSingleProp(pMe,
                                                           pIvProperty, 
                                                           UID, 
                                                           0,
                                                           AEECLSID_vAlarm,
                                                           FALSE,
                                                           dwRecNum);
                  }
              
                  break;
               }  
            case AEEVPROPERTY_EXTENDED:
            case AEEVPROPERTY_ATTENDEE:
            case AEEVPROPERTY_ATTACH:
               {
                  nResult = IvCalStore_HandleMultiProp(pMe,pIvProperty,UID,TRUE,dwRecNum);
                  break;

               }
         }
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
    }while(0);
    RELEASEIF(pIvProperty);
    return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_HandleAlarm()

Description:
		 This function handles inserting the alarm properties in ALARM table
  

Prototype:
	  int IvCalStore_HandleAlarm(IvCalStore  *pMe,
                              IvCalObject *pIvCalObject,
                              uint32      UID);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvCalObject    [in]: The pointer to alarm component object.
   UID             [in]: UID
   

Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   SQL returned ERROR in case of failure

Comments:  
   This function adds the properties for Alarm subcomponent in an Event,
   Todo or Journal.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_HandleAlarm(IvCalStore  *pMe,
                           IvCalObject *pIvObject,
                           uint32      UID)
{
   char *pszStrAlarm = "INSERT INTO "VCALSTORE_ALARM_TBL
      " (Summary,Description,Action,Duration,Repeat,Trigger,"
      "UID,Snooze) VALUES"
      "(?,?,?,?,?,?,?,?)"; 
   char *pszSummary = NULL, *pszAction = NULL, *pszDesc = NULL;
   ISQLStmt *pStmt = NULL;
   uint32 dwRecNumAlarm = 0;
   char *pszDuration = NULL, *pszTriggerStr = NULL;
   IvProperty *pIvProperty = NULL;   
   int nResult = AEE_EFAILED, nCount = 0, nNumProp = 0, nSize = 0;  
   uint32 dwProp = 0; 
   if(NULL == pIvObject){
      return AEE_EBADPARM;
   }
   do{
      nResult = IvCalObject_GetNumberOfProperties(pIvObject, 0, &nNumProp);  
      BREAKIF(nResult);
      nResult = ISQL_Prepare( pMe->pISQL, pszStrAlarm, -1, &pStmt, NULL );
      if(SQL_OK != nResult){
         nResult = AEE_EFAILED;
         break;
      }
      nResult = ISQLSTMT_BindInt(pStmt, 7, (int)UID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindNull(pStmt, 8);
      BREAKIF(nResult);
      for(nCount =0; nCount < nNumProp; nCount++){         
         nResult = IvCalObject_GetPropertyAtIndex(pIvObject, 0, nCount,&pIvProperty);         
         BREAKIF(nResult);
         if(NULL == pIvProperty){
            nResult = AEE_EFAILED;
            break;
         }
         nResult = IvProperty_GetId(pIvProperty, &dwProp);
         BREAKIF(nResult);
         switch(dwProp){
            case AEEVPROPERTY_SUMMARY:
               {
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  NULL,
                                                  0,
                                                  &nSize);
                  BREAKIF(nResult);
                  pszSummary = (char*)MALLOC(nSize);
                  if(NULL == pszSummary){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  pszSummary,
                                                  nSize, 
                                                  &nSize);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindText(pStmt, 1, pszSummary, -1, NULL);                   
                  
                  break;             
               }
            case AEEVPROPERTY_DESCRIPTION:
               {               
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  NULL, 
                                                  0,
                                                  &nSize);
                  BREAKIF(nResult);
                  pszDesc = (char*)MALLOC(nSize);
                  if(NULL == pszDesc){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                   FALSE,
                                                   pszDesc,
                                                   nSize, 
                                                   &nSize);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindText(pStmt, 2, pszDesc, -1, NULL); 
                  break;
                  
               }                           
            case AEEVPROPERTY_ACTION:
               {

                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  NULL, 
                                                  0,
                                                  &nSize);
                  BREAKIF(nResult);
                  pszAction = (char*)MALLOC(nSize);
                  if(NULL == pszAction){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                   FALSE,
                                                   pszAction,
                                                   nSize, 
                                                   &nSize);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindText(pStmt, 3, pszAction, -1, NULL);
                  break;
                              
               }

            case AEEVPROPERTY_DURATION:
               {                                 
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  NULL, 
                                                  0,
                                                  &nSize);
                  BREAKIF(nResult);
                  pszDuration = (char*)MALLOC(nSize);
                  if(NULL == pszDuration){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                   FALSE,
                                                   pszDuration,
                                                   nSize, 
                                                   &nSize);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindText(pStmt, 4, pszDuration, -1, NULL);
                  break;
                                 
               }

            case AEEVPROPERTY_REPEAT:
               {
                  
                  uint32 Value = 0;           
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                   TRUE,
                                                   (uint32*)&Value, 
                                                   sizeof(uint32),
                                                   &nSize);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindInt(pStmt, 5, (int)Value);   
                  break;                

               }
            case AEEVPROPERTY_TRIGGER:
               
               {                  
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                  FALSE,
                                                  NULL, 
                                                  0,
                                                  &nSize);
                  BREAKIF(nResult);
                  pszTriggerStr = (char*)MALLOC(nSize);
                  if(NULL == pszTriggerStr){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  nResult = IvProperty_ParseValue(pIvProperty, 
                                                   FALSE,
                                                   pszTriggerStr,
                                                   nSize, 
                                                   &nSize);
                  BREAKIF(nResult);
                                    
                  nResult = ISQLSTMT_BindText(pStmt, 6, pszTriggerStr,-1,NULL);
                  break;                
                  
               }                                      
         }
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }
      dwRecNumAlarm = (uint32)ISQL_GetLastRowId(pMe->pISQL);
      nResult = IvCalStore_AddSingleAlarmParms(pMe, pIvObject,dwRecNumAlarm, UID);

   }while(0);
  
   FREEIF(pszDuration);   
   FREEIF(pszTriggerStr);
   FREEIF(pszSummary);
   FREEIF(pszDesc);   
   FREEIF(pszAction);   
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetParmString()

Description:
		 This function gets the values for the param in the form of comma
   separated string.
  

Prototype:
	  int IvCalStore_GetParmString(IvParm *pIvParm, AECHAR **pwszMainStr);

Parameters:
   pIvParm:        [in]: Pointer to the IvParm interface 
   pwszMainStr     [in]: The string to be returned.   

Return Value:
   AEE_SUCCESS   : If the values are returned successfully.
   AEE_EFAILED   : Otherwise.

Comments:  
   This function is used to get a string format of all the parameter values.
   A parameter can have multiple values. These values would be stored in a 
   comma separated string in the database.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetParmString(IvParm *pIvParm, AECHAR **pwszMainStr)
{
   int nResult = AEE_EFAILED;
   uint32 dwValue = 0;
   int nReqSize = 0;
   int nNumValues = 0,  nCount = 0; 
   AECHAR *pwszValues = NULL, *pwszTempStr = NULL, *pwszParmName = NULL;
   AEEVParameter dwParmId = 0;
   char *pszParmName = NULL;         
   AECHAR *pwszTempParmName = NULL;
   
   if(NULL == pwszMainStr || NULL == pIvParm){
      return AEE_EBADPARM;
   }     
   do{
      //Get the number of values for this parameter
      nResult = IvParm_GetNumberOfValues(pIvParm, &nNumValues);
      BREAKIF(nResult);
      //for each value check whether it is uint32 value or string value
      //and concatenate all the values and store it as a string.
      for(nCount = 0;nCount < nNumValues;nCount++){                           
         nResult = IvParm_ParseValue(pIvParm,
                                     TRUE,
                                     nCount,
                                     &dwValue,
                                     NULL,
                                     sizeof(uint32),
                                     (int*)&nReqSize);
         BREAKIF(nResult);
         //if the parm value is string dwValue would be returned as 0
         if(0 == dwValue){
            //pwszValues = (AECHAR*)MALLOC(nReqSize*sizeof(AECHAR));
            pwszValues = (AECHAR*)MALLOC(nReqSize);
            if(NULL == pwszValues){
               nResult = AEE_ENOMEMORY;
               break;
            }
            nResult = IvParm_ParseValue(pIvParm,
                                        TRUE,
                                        nCount,
                                        &dwValue,
                                        pwszValues,
                                        nReqSize,
                                        &nReqSize);
            BREAKIF(nResult);
            if(NULL == *pwszMainStr ){
               *pwszMainStr = IvCalStore_WStrnDup(pwszValues,WSTRLEN(pwszValues));
               if(NULL == *pwszMainStr){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               //pwszTempStr = *pwszMainStr;
            }else{               
               pwszTempStr = *pwszMainStr;
               *pwszMainStr = IvCalStore_AppendWStr(*pwszMainStr,pwszValues,';');                              
               if(NULL == *pwszMainStr){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               FREEIF(pwszTempStr);
               //pwszTempStr = *pwszMainStr;
            }            
            FREEIF(pwszValues);

         }else{
            //The value is a number 
            AECHAR *pwszIntToStr = NULL;
            pwszIntToStr = IvCalStore_IntToAECHARString(dwValue);
            if(NULL == pwszIntToStr){
               nResult = AEE_ENOMEMORY;
               break;
            }
            if(NULL == *pwszMainStr ){                              
               *pwszMainStr = IvCalStore_WStrnDup(pwszIntToStr,
                                            WSTRLEN(pwszIntToStr));
               FREEIF(pwszIntToStr);
               if(NULL == *pwszMainStr){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               //pwszTempStr = *pwszMainStr;
            }else{                  
               pwszTempStr = *pwszMainStr;
               *pwszMainStr = IvCalStore_AppendWStr(*pwszMainStr, pwszIntToStr, ';');
               FREEIF(pwszIntToStr);
               if(NULL == *pwszMainStr){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               FREEIF(pwszTempStr);
               //pwszTempStr = *pwszMainStr;
            }
            FREEIF(pwszIntToStr);            
         } 
         
      }

      if(AEE_SUCCESS != nResult){
         break;
      }
      
      nResult = IvParm_GetId(pIvParm, &dwParmId);
      BREAKIF(nResult);
      if(AEEVPARM_EXTENDED == dwParmId){
         
         nResult = IvParm_GetName(pIvParm,
                                  NULL,
                                  0,
                                  &nReqSize);
         BREAKIF(nResult);

         pszParmName = (char*)MALLOC(nReqSize);
         if(NULL == pszParmName){
            nResult = AEE_ENOMEMORY;
            break;
         }

         nResult = IvParm_GetName(pIvParm,
                                  pszParmName,
                                  nReqSize,
                                  &nReqSize);
         BREAKIF(nResult);

         pwszParmName = (AECHAR*)MALLOC((nReqSize)*sizeof(AECHAR));
         if(NULL == pwszParmName){
            FREEIF(pszParmName);
            nResult = AEE_ENOMEMORY;
            break;
         }
         STRTOWSTR(pszParmName, pwszParmName, nReqSize*sizeof(AECHAR));
         pwszTempParmName = *pwszMainStr;
         *pwszMainStr = IvCalStore_AppendWStr(pwszParmName, *pwszMainStr, '=');
         FREEIF(pwszParmName);
         
         if(NULL == *pwszMainStr){
            nResult = AEE_ENOMEMORY;
            break;           
         }         
      }      
      
   }while(0);   

   FREEIF(pszParmName);
   FREEIF(pwszTempParmName);
   FREEIF(pwszValues);
   FREEIF(pwszTempStr);
   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_AddPropParms()

Description:
   This function handles adding the property and params for different 
   properties.

Prototype:
	  int IvCalStore_AddPropParms(IvCalStore *pMe,
                               IvProperty *pIvProperty,
                               uint32     dwUID);

Parameters:
   pMe:            [in]: Pointer to the IvCalStore interface 
   pIvProperty     [in]: Pointer to property.
   dwUID           [in]: UID of the record.   
  
Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   AEE_EFAILED       :   otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddPropParms(IvCalStore *pMe,
                            IvProperty *pIvProperty,
                            uint32     dwOID)
{
   int nResult = AEE_EFAILED, nSize = 0, nNumParms = 0, nCount = 0;
   IvProperty *pIvPropertyTemp = NULL;
   IvParm *pIvParm = NULL;
   AEEVDateTimePeriod *pDateTimePeriod = NULL;
   AEEVRRule *pRRule = NULL;
   uint32 dwProp = 0;
   do{                   
         nResult = IvProperty_GetId(pIvProperty, &dwProp);
         BREAKIF(nResult);
         switch(dwProp) 
         {
         case AEEVPROPERTY_RDATE:
            {         
               //Parse the value for RDATE 
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pDateTimePeriod = (AEEVDateTimePeriod*)MALLOC(nSize);
               if(NULL == pDateTimePeriod){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               pDateTimePeriod,
                                               nSize,
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                               AEECLSID_vProperty, 
                                               (void**)&pIvPropertyTemp);
               BREAKIF(nResult);
               //Initialize a temporary property so that we can 
               //get the RDATE property values in iCal 2.0 format
               nResult = IvProperty_InitProperty(pIvPropertyTemp,
                                                 dwProp,
                                                 NULL,
                                                 TRUE,
                                                 pDateTimePeriod,
                                                 nSize,
                                                 NULL);
               FREEIF(pDateTimePeriod);
               BREAKIF(nResult);
               //Add all the parameters to the temporary property
               nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms);
               BREAKIF(nResult);
               for ( nCount = 0; nCount < nNumParms; nCount++ ){
                  nResult = IvProperty_GetParmAtIndex(pIvProperty,
                                                      0,
                                                      nCount,
                                                      (IvParm**)&pIvParm);
                  BREAKIF(nResult);
                  nResult = IvProperty_AddParm(pIvPropertyTemp, pIvParm);
                  RELEASEIF(pIvParm);
                  BREAKIF(nResult);
               }
               BREAKIF(nResult);
               //Add the RDATE property to the database
               nResult = IvCalStore_HandleMultiProp(pMe, pIvPropertyTemp, dwOID, FALSE,0);


            }
            break;
         case AEEVPROPERTY_RRULE: 
            {
               //Parse the value for RRULE
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               pRRule = (AEEVRRule *)MALLOC(nSize);
               if(NULL == pRRule){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               pRRule,
                                               nSize,
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                               AEECLSID_vProperty, 
                                               (void**)&pIvPropertyTemp);
               BREAKIF(nResult);
               //Initialize a temporary property so that we can 
               //get the RRULE property values in iCal 2.0 format
               nResult = IvProperty_InitProperty(pIvPropertyTemp,
                                                 dwProp,
                                                 NULL,
                                                 TRUE,
                                                 pRRule,
                                                 nSize,
                                                 NULL);
               FREEIF(pRRule);
               BREAKIF(nResult);
               //Add all the parameters to the temporary property
               nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms);
               BREAKIF(nResult);
               for ( nCount = 0; nCount < nNumParms; nCount++ ){
                  nResult = IvProperty_GetParmAtIndex(pIvProperty,
                                                      0,
                                                      nCount,
                                                      (IvParm**)&pIvParm);
                  BREAKIF(nResult);
                  nResult = IvProperty_AddParm(pIvPropertyTemp, pIvParm);
                  RELEASEIF(pIvParm);
                  BREAKIF(nResult);
               }
               BREAKIF(nResult);
               //Add the RRule property to the database
               nResult = IvCalStore_HandleMultiProp(pMe, pIvPropertyTemp, dwOID, FALSE,0);

            }
            break;
            //These are the properties which can occur more than once 
            //in an event. we store them in separate PROP table 
         case AEEVPROPERTY_XRULE: 
            {
               //Parse the value for RRULE
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               NULL,
                                               0,
                                               &nSize);
               BREAKIF(nResult);
               BREAKIF(nResult);
               pRRule = (AEEVRRule *)MALLOC(nSize);
               if(NULL == pRRule){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               nResult = IvProperty_ParseValue(pIvProperty,
                                               TRUE,
                                               pRRule,
                                               nSize,
                                               &nSize);
               BREAKIF(nResult);
               nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                               AEECLSID_vProperty, 
                                               (void**)&pIvPropertyTemp);
               BREAKIF(nResult);
               //Initialize a temporary property so that we can 
               //get the RRULE property values in iCal 2.0 format
               nResult = IvProperty_InitProperty(pIvPropertyTemp,
                                                 AEEVPROPERTY_EXRULE,
                                                 NULL,
                                                 TRUE,
                                                 pRRule,
                                                 nSize,
                                                 NULL);
               FREEIF(pRRule);
               BREAKIF(nResult);
               //Add all the parameters to the temporary property
               nResult = IvProperty_GetNumberOfParms(pIvProperty, 0, &nNumParms);
               BREAKIF(nResult);
               for ( nCount = 0; nCount < nNumParms; nCount++ ){
                  nResult = IvProperty_GetParmAtIndex(pIvProperty,
                                                      0,
                                                      nCount,
                                                      (IvParm**)&pIvParm);
                  BREAKIF(nResult);
                  nResult = IvProperty_AddParm(pIvPropertyTemp, pIvParm);
                  RELEASEIF(pIvParm);
                  BREAKIF(nResult);
               }
               BREAKIF(nResult);
               //Add the RRule property to the database
               nResult = IvCalStore_HandleMultiProp(pMe, pIvPropertyTemp, dwOID, FALSE,0);
            }
            break;
         case AEEVPROPERTY_CATEGORIES:          
         case AEEVPROPERTY_RELATED: 
         case AEEVPROPERTY_RESOURCES:
         case AEEVPROPERTY_CONTACT:
         case AEEVPROPERTY_COMMENT:
         case AEEVPROPERTY_ATTACH:
         case AEEVPROPERTY_EXDATE:   
         case AEEVPROPERTY_EXRULE:  
         case AEEVPROPERTY_EXTENDED:
         case AEEVPROPERTY_RSTATUS:
         case AEEVPROPERTY_ATTENDEE: 
         case AEEVPROPERTY_ORGANIZER:
           
            {               
               nResult = IvCalStore_HandleMultiProp(pMe, pIvProperty, dwOID, FALSE,0);
               break;
            }
         
         //These are the properties which occur only once (if any) in the event
         // we have already stored the values for all these properties in MAIN
         //table. Now we just store the parameters for these properties in
         //PARAM table.
         case AEEVPROPERTY_URL:                    
         case AEEVPROPERTY_LOCATION:            
         case AEEVPROPERTY_DESCRIPTION:
         case AEEVPROPERTY_SUMMARY: 
         case AEEVPROPERTY_DURATION:
         case AEEVPROPERTY_CLASS:
         case AEEVPROPERTY_DCREATED:
         case AEEVPROPERTY_CREATED:
         case AEEVPROPERTY_DCOMPLETED:
         case AEEVPROPERTY_DUE:
         case AEEVPROPERTY_DTEND:
         case AEEVPROPERTY_LASTMODIFIED:
         case AEEVPROPERTY_DTSTART:
         case AEEVPROPERTY_GEO:
         case AEEVPROPERTY_TIMEZONE:
         case AEEVPROPERTY_RNUM:
         case AEEVPROPERTY_PRIORITY:
         case AEEVPROPERTY_SEQUENCE:
         case AEEVPROPERTY_TRANSP: 
         case AEEVPROPERTY_STATUS:
         case AEEVPROPERTY_PERCENT:
         case AEEVPROPERTY_RECUR_ID:
         case AEEVPROPERTY_DTSTAMP:                
         case AEEVPROPERTY_UID:                
         case AEEVPROPERTY_TZID:               
            {               
               nResult = IvCalStore_HandleSingleProp(pMe,
                                                     pIvProperty,
                                                     dwOID,
                                                     0,
                                                     0,
                                                     FALSE,
                                                     0);
               break;
            }                                    
         default :
            {            
               break;
            }
         }
         if(AEE_SUCCESS != nResult){
            break;
         }
   }while(0);

   FREEIF(pDateTimePeriod);
   FREEIF(pRRule);
   RELEASEIF(pIvParm);
   RELEASEIF(pIvPropertyTemp);   

   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_DeleteAlarm()

Description:
		 This function deletes all the alarm related records in the Alarm
   tables.

Prototype:
	  int IvCalStore_DeleteAlarm(IvCalStore *pIvCalStore,
                              uint32     dwUID);

Parameters:
   pIvCalStore    [in]: Pointer to the IvCalStore interface 
   dwUID          [in]: Record number to be updated.
   
  
Return Value:
   AEE_SUCCESS   :   If the deletion is successfull.
   AEE_EFAILED   :   Otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_DeleteAlarm(IvCalStore *pIvCalStore,
                           uint32     dwUID)
{
   int nResult = AEE_EFAILED;
   char *pszDelAlarm = "DELETE FROM "VCALSTORE_ALARM_TBL
      " WHERE UID = ?";
   char *pszDelAlarmProp = "DELETE FROM "VCALSTORE_ALARMPROP_TBL
      " WHERE AlarmOID = ?";
   char *pszDelAlarmParm = "DELETE FROM "VCALSTORE_ALARMPARAM_TBL
      " WHERE AlarmOID = ?";
   ISQLStmt *pStmt = NULL;
   do{
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelAlarm, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){ 
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }
      RELEASEIF(pStmt);

      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelAlarmProp, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){ 
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }
      RELEASEIF(pStmt);

      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelAlarmParm, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){ 
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }

   }while(0);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_DeleteTimeZone()

Description:
		 This function deletes all the time zone related records in the TimeZone 
   tables.

Prototype:
	  int IvCalStore_DeleteTimeZone(IvCalStore *pIvCalStore,
                                 uint32     dwUID);

Parameters:
   pIvCalStore    [in]: Pointer to the IvCalStore interface 
   dwUID          [in]: Record number to be updated.
   
  
Return Value:
   AEE_SUCCESS   :   If the deletion is successfull.
   AEE_EFAILED   :   Otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_DeleteTimeZone(IvCalStore *pIvCalStore,
                              uint32     dwUID)
{
   char *pszDelTimeZone = "DELETE FROM "VCALSTORE_TIMEZONE_TBL
                          " WHERE UID = ?";
   char *pszDelDaylight = "DELETE FROM "VCALSTORE_DAYLIGHT_TBL
                          " WHERE UID = ?";
   char *pszDelTimeZoneProp = "DELETE FROM "VCALSTORE_TZPROP_TBL
                              " WHERE UID = ?";
   char *pszDelTimeZoneParam = "DELETE FROM "VCALSTORE_TZPARAM_TBL
                              " WHERE UID = ?";
   
   ISQLStmt *pStmt = NULL;
   int nResult = AEE_EFAILED;
   do{
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelTimeZone, -1, &pStmt, NULL);
      BREAKIF(nResult);    
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){          
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
      RELEASEIF(pStmt);
      
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelDaylight, -1, &pStmt, NULL);
      BREAKIF(nResult);    
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){          
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
      RELEASEIF(pStmt);

      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelTimeZoneProp, -1, &pStmt, NULL);
      BREAKIF(nResult);    
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){          
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
      RELEASEIF(pStmt);

      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelTimeZoneParam, -1, &pStmt, NULL);
      BREAKIF(nResult);    
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){          
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }          

   }while(0);
   RELEASEIF(pStmt);
   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_DeleteObject()

Description:
		 This function makes all the fields in Main table to NULL during updation
   of the record. It also deletes records for the OID from all the other tables
   so that the tables can be updated with the new ones.

Prototype:
	  int IvCalStore_DeleteObject(IvCalStore *pIvCalStore,
                                    uint32     dwUID);

Parameters:
   pIvCalStore:    [in]: Pointer to the IvCalStore interface 
   dwUID          [in]: Record number to be updated.
   
  
Return Value:
   AEE_SUCCESS   :   If the insertion is successfull.
   SQL returned ERROR in case of failure

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_DeleteObject(IvCalStore *pIvCalStore,
                            uint32     dwUID)
{
   int nResult = AEE_EFAILED;        
   char *pszStr = "DELETE FROM "VCALSTORE_MAIN_TBL
                  " WHERE RecordNumber = ?";
   char *pszDelProp = "DELETE FROM "VCALSTORE_PROP_TBL
                      " WHERE UID = ?";
   char *pszDelParm = "DELETE FROM "VCALSTORE_PARAM_TBL
                      " WHERE UID = ?";   
   
   ISQLStmt *pStmt = NULL;   

   do{         
      //delete records from the PROP table
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelProp, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){ 
         nResult = AEE_EFAILED;         
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
      RELEASEIF(pStmt);
      //delete records from the PARAM table
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszDelParm, -1, &pStmt, NULL);
      BREAKIF(nResult);    
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){          
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
      RELEASEIF(pStmt);                
      
      //Delete the time zone related records
      nResult = IvCalStore_DeleteTimeZone(pIvCalStore,
                                          dwUID);
      BREAKIF(nResult);
      //delete the Alarm related records
      nResult = IvCalStore_DeleteAlarm(pIvCalStore,
                                       dwUID);
      BREAKIF(nResult);      
      //delete record from the main table.
      nResult = ISQL_Prepare(pIvCalStore->pISQL, pszStr, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwUID);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if(SQL_DONE != nResult){ 
         nResult = AEE_EFAILED;
         break;
      }else {
         nResult = AEE_SUCCESS;
      }      
   }while(0);
   
   RELEASEIF(pStmt);
   return nResult;
}

/*===========================================================================
Function:  
   IvCalStore_AddProperty()

Description:
		 This function adds a property to the TODO.

Prototype:
	  int IvCalStore_AddProperty(IvCalStore *pMe,                                                     
                              IvCalObject *pIvObj,
                              AEEVProperty dwProp,
                              void* pdwVal,
                              uint32 nSize,
                              uint32 dwOID,
                              boolean bSingle,
                              AEEvObjectType objType);
                           
Parameters:
   pMe            [in]: Pointer to the IvCalStore interface 
   pIvObj         [in]: Pointer to object
   dwProp         [in]: Property id
   pdwVal         [in]: Value of property
   nSize          [in]: Size of the value.
   dwOID          [in]: OID of the object.
   bSingle        [in]: Whether property occurs single times or multiple
                        times
   objType        [in]: Object type

Return Value:
   Gets the parameters for the property.

Comments: 
   This function adds a property to a component object. This takes care of 
   adding parameters for the property stored in PARM table.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_AddProperty(IvCalStore   *pMe,                                                     
                           IvCalObject  *pIvObj,
                           AEEVProperty dwProp,
                           void         *pdwVal,
                           uint32       nSize,
                           uint32       dwOID,
                           boolean      bSingle,
                           AEECLSID     objType)
{
   int nResult = AEE_EFAILED;
   IvProperty *pIvProperty = NULL;   
   IvCalObject *pIvCalObject = NULL;
   IvParm *pIvParm = NULL;
   uint32 dwParmID = 0;  
   AECHAR *pwszParmVal = NULL;
   ISQLStmt *pStmt = NULL;
   char *pszStrParam = "SELECT ParmID, ParmVal FROM "VCALSTORE_PARAM_TBL
                       " WHERE UID = ? AND PropID = ?";
   char *pszStrMultiPropParam = "SELECT ParmID, ParmVal FROM "
                                VCALSTORE_PARAM_TBL" WHERE "
                                "RecNumProp = ? AND PropID = ?";

   char *pszStrAlarmParam = "SELECT ParmID, ParmVal FROM "
                            VCALSTORE_ALARMPARAM_TBL" WHERE "
                            "RecNumAlarmTable = ? AND PropID = ? ";
   char *pszStrAlarmMultiPropParam = "SELECT ParmID, ParmVal FROM "
                                     VCALSTORE_ALARMPARAM_TBL
                                     " WHERE RecNumProp = ? AND PropID = ?";
                                     

   if(NULL == pIvObj){
      return AEE_EBADPARM;
   }
   do{   
      /*if(AEEVPROPERTY_RNUM == dwProp){
         return AEE_SUCCESS;
      }*/
      pIvCalObject = (IvCalObject*)pIvObj;
      nResult = ISHELL_CreateInstance( pMe->pIShell,
                                       AEECLSID_vProperty,
                                       (void**)&pIvProperty);
      BREAKIF(nResult);      
      if((AEEVPROPERTY_TIMEZONE == dwProp) || 
         (AEEVPROPERTY_PRIORITY == dwProp) || (AEEVPROPERTY_SEQUENCE == dwProp) || 
         (AEEVPROPERTY_PERCENT == dwProp) || (AEEVPROPERTY_TZOFFSETFROM == dwProp) ||
         (AEEVPROPERTY_TZOFFSETTO == dwProp) || (AEEVPROPERTY_REPEAT == dwProp)){
            nResult = IvProperty_InitProperty(pIvProperty, 
                                             dwProp,
                                             NULL, 
                                             TRUE,
                                             pdwVal,
                                             nSize,
                                             NULL);            
         }else if (AEEVPROPERTY_EXTENDED == dwProp){
            char *pszPropName = NULL, *pszNameValueSep = NULL, *pszTemp = NULL;
            int nLen = 0;  
            pszTemp = (char*)pdwVal;
            pszNameValueSep = STRCHR(pszTemp,':');
            if(NULL == pszNameValueSep){
               nResult = AEE_EFAILED;
               break;
            }
            nLen = pszNameValueSep - pszTemp + 1;
            pszPropName = (char*)MALLOC(nLen);
            if(NULL == pszPropName){
               nResult = AEE_ENOMEMORY;
               break;
            }
            STRLCPY((char*)pszPropName, (char*)pszTemp, nLen); 
            pszNameValueSep++;

            if(NULL != pszPropName && NULL != pszNameValueSep){
               nResult = IvProperty_InitProperty(pIvProperty, 
                                                dwProp,
                                                pszPropName,
                                                FALSE,
                                                pszNameValueSep,
                                                STRLEN(pszNameValueSep),
                                                NULL);
            } else {
               nResult = AEE_EFAILED;
               break;
            }
            FREEIF(pszPropName);
            BREAKIF(nResult);
         }else {
            nResult = IvProperty_InitProperty(pIvProperty, 
                                             dwProp,
                                             NULL,
                                             FALSE,
                                             pdwVal,
                                             nSize,
                                             NULL);
         }
      BREAKIF(nResult);
      if(AEECLSID_vAlarm != objType){
         if(TRUE == bSingle){
            nResult = ISQL_Prepare(pMe->pISQL, pszStrParam, -1 , &pStmt, NULL);
         }else{
            nResult = ISQL_Prepare(pMe->pISQL, pszStrMultiPropParam, -1 , &pStmt, NULL);
         }
      }else {
         if(TRUE == bSingle){
            nResult = ISQL_Prepare(pMe->pISQL, pszStrAlarmParam, -1 , &pStmt, NULL);
         }else{
            nResult = ISQL_Prepare(pMe->pISQL, pszStrAlarmMultiPropParam, -1 , &pStmt, NULL);
         }
      }
      BREAKIF(nResult);
      (void)ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      (void)ISQLSTMT_BindInt(pStmt, 2, (int)dwProp);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){ 
         pwszParmVal = NULL;
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,(int*)&dwParmID);
         BREAKIF(nResult);
         /*if((AEEVPROPERTY_LOCATION == dwProp ||
            AEEVPROPERTY_SUMMARY == dwProp || 
            AEEVPROPERTY_ATTENDEE == dwProp)&&((AEEVPARM_ENCODING == dwParmID)||
            (AEEVPARM_TYPE == dwParmID) || (AEEVPARM_VALUE == dwParmID) ||
            (AEEVPARM_LANG == dwParmID) || (AEEVPARM_EXPECT == dwParmID) ||
            (AEEVPARM_STATUS == dwParmID))){
               continue;
            }*/
            nResult = ISQLSTMT_GetColumnWText(pStmt,1,&pwszParmVal);
            BREAKIF(nResult);
            if(NULL != pwszParmVal){
               nResult = IvCalStore_GetAndStoreParms(pMe,
                                                      dwParmID,
                                                      pwszParmVal,
                                                      pIvProperty);            
            }   
            
      }                        
      if(AEE_SUCCESS == nResult){         
         nResult = IvCalObject_AddProperty(pIvCalObject,pIvProperty);
      }      
   }while(0);
   RELEASEIF(pIvParm);
   RELEASEIF(pIvProperty);
   RELEASEIF(pStmt);
   //FREEIF(pwszParmVal);

   return nResult;
   

}

/*===========================================================================
Function:  
   IvCalStore_GetAndStoreParms()

Description:
		 This function gets all the values for a parameter for a particular 
   property.

Prototype:
	  int IvCalStore_GetAndStoreParms(IvCalStore *pMe,
                                   uint32      dwParmID,
                                   AECHAR      *pwszParmVal,
                                   IvProperty  *pIvProperty);
                           
Parameters:
   pMe            [in]: Pointer to the IvCalStore interface 
   dwParmID       [in]: Parm Id
   pszParmVal     [in]: Comma  separated values for parameter
   pIvProperty    [in]: Property to be initialized with the parameter
Return Value:
   Gets the parameters for the attendee property.

Comments: 
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetAndStoreParms(IvCalStore *pMe,
                                uint32      dwParmID,
                                AECHAR      *pszParmVal,
                                IvProperty  *pIvProperty)

{
   int nResult = AEE_EFAILED;
   AECHAR *pTempStr1 = NULL, *pTempStr2 = NULL, *pszPropName = NULL;
   IvParm *pIvParm = NULL;
   AECHAR *pszParmValues = NULL;
   boolean bFirstTime = TRUE;
   AECHAR *pwszParmVal = NULL; 
   AECHAR wsSep[] = {'\0'};
   if(NULL == pszParmVal || NULL == pIvProperty){
      return AEE_EBADPARM;
   }   

   do{
      if(AEEVPARM_EXTENDED == dwParmID){
         AECHAR *pszNameValueSep = NULL, *pszTemp = NULL;
         int nLen = 0; 
         AECHAR wszEqualSep[] = {'='};
         pszTemp = (AECHAR*)pszParmVal;
         pszNameValueSep = WSTRCHR(pszTemp,wszEqualSep[0]);
         nLen = (pszNameValueSep - pszTemp) + 1;
         pszPropName = (AECHAR*)MALLOC(nLen * sizeof(AECHAR));
         if(NULL == pszPropName){
            nResult = AEE_ENOMEMORY;
            break;
         }
         WSTRLCPY((AECHAR*)pszPropName, (AECHAR*)pszTemp, nLen); 
         pszNameValueSep++;
         pTempStr1 = pTempStr2 = pszNameValueSep;

      }else{
         pTempStr1 = pTempStr2 = pszParmVal;
      }
      nResult = ISHELL_CreateInstance( pMe->pIShell,
                                       AEECLSID_vParm,
                                       (void**)&pIvParm);
      if(AEE_SUCCESS != nResult){                        
         break;
      }
      while((NULL != pTempStr1)&&(wsSep[0] != *pTempStr1)){
         int nLen = 0;
         char *propName = NULL;         
         boolean bIsValue = FALSE;
         AECHAR wszCommaSep[] = {';'};
         char *pszVal = NULL;                        
         pTempStr1 = WSTRCHR(pTempStr1,wszCommaSep[0]);
         if(NULL == pTempStr1){                        
            nLen = WSTRLEN(pTempStr2);
         }else{
            nLen = (pTempStr1 - pTempStr2);
         }
         pszParmValues = (AECHAR*)MALLOC((nLen + 1)*sizeof(AECHAR));
         if(NULL == pszParmValues){
            nResult = AEE_ENOMEMORY;
            break;
         }
         MEMSET(pszParmValues, 0, (nLen + 1)*sizeof(AECHAR));
         WSTRLCPY(pszParmValues, pTempStr2, nLen + 1);   
         
         bIsValue = IvCalStore_CheckForValueW(pszParmValues);
         if(bIsValue){
            pszVal = (char*)MALLOC((WSTRLEN(pszParmValues)+1)*sizeof(AECHAR));
            if(NULL == pszVal){               
               nResult = AEE_ENOMEMORY;
               break;
            }
            pszVal = WSTRTOSTR( pszParmValues, pszVal, (WSTRLEN(pszParmValues)+1)*sizeof(AECHAR));
            if(bFirstTime){
               if(AEEVPARM_EXTENDED == dwParmID){                  
                  propName = (char*)MALLOC((WSTRLEN(pszPropName)+1)*sizeof(AECHAR));
                  if(NULL == propName){
                     nResult = AEE_ENOMEMORY;
                     FREEIF(pszVal);
                     break;
                  }
                  propName = WSTRTOSTR(pszPropName, propName, (WSTRLEN(pszPropName)+1)*sizeof(AECHAR));
                  
                  nResult = IvParm_InitParm(pIvParm,
                                            dwParmID,
                                            propName,
                                            (uint32)ATOI(pszVal),
                                            NULL);   
                  FREEIF(propName);
                  FREEIF(pszVal);
               }else{                  
                  nResult = IvParm_InitParm(pIvParm,
                                            dwParmID,
                                            NULL,
                                            (uint32)ATOI(pszVal),
                                            NULL); 
                  FREEIF(pszVal);
               }
               bFirstTime = FALSE;
            }else {
               nResult = IvParm_AddValue(pIvParm,                                         
                                         (uint32)ATOI(pszVal),
                                         NULL);
               FREEIF(pszVal);
            }
         }else{
            
            if(bFirstTime){               
               if(AEEVPARM_EXTENDED == dwParmID){
                  propName = (char*)MALLOC((WSTRLEN(pszPropName)+1)*sizeof(AECHAR));
                  if(NULL == propName){
                     nResult = AEE_ENOMEMORY;
                     break;
                  }
                  propName = WSTRTOSTR(pszPropName, propName, (WSTRLEN(pszPropName)+1)*sizeof(AECHAR));
                  nResult = IvParm_InitParm( pIvParm,
                                             dwParmID,
                                             propName,
                                             0,
                                             pszParmValues); 
                  FREEIF(propName);
               }else{                  
                  nResult = IvParm_InitParm( pIvParm,
                                             dwParmID,
                                             NULL,
                                             0,
                                             pszParmValues);
               }
               bFirstTime = FALSE;
            }else{
               nResult = IvParm_AddValue(pIvParm,                                         
                                         0,
                                         pszParmValues);
            }         
            FREEIF(pwszParmVal);

         }
         FREEIF(pszParmValues);                        
         BREAKIF(nResult);
         if(NULL == pTempStr1){
            break;
         }
         pTempStr1++;
         pTempStr2 = pTempStr1;
      }
      BREAKIF(nResult);
      nResult = IvProperty_AddParm(pIvProperty, pIvParm); 
   }while(0);
   RELEASEIF(pIvParm);
   FREEIF(pszPropName);
   FREEIF(pwszParmVal);
   FREEIF(pszParmValues);
   return nResult;
}

/*===========================================================================

Function:  
   IvCalStore_Lock()

Description:
		 This function Locks the calendar database if proper privileges are 
   available.

Prototype:
   int IvCalStore_Lock(IvCalStore *pMe);

Parameters:
   pMe: Pointer to IvCalStore interface 

Return Value:
   AEE_SUCCESS     : If the calendar is locked successfully
   AEE_EPRIVLEVEL  : If the user doens't have proper privilege
   AEE_EFAILED     : otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_Lock(IvCalStore *pMe)
{
   int nResult = AEE_EFAILED;   
   char *pszUpdate = "UPDATE LOCK SET Locked = 1";
   ISQLStmt *pStmt = NULL;

   do{      
      nResult = ISQL_Prepare(pMe->pISQL, pszUpdate, -1, &pStmt, NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_Step(pStmt);
      if( SQL_DONE != nResult) { 
         nResult = AEE_EFAILED;
         break;
      }else{
         nResult = AEE_SUCCESS;
      }      

   }while(0);
   
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================

Function:  
   CvCalStore_LockCalendar()

Description:
		 This function Locks the calendar database if proper privileges are 
   available.

Prototype:
   int CvCalStore_LockCalendar(IvCalStore *pMe);

Parameters:
   pMe: Pointer to IvCalStore interface 

Return Value:
   AEE_SUCCESS : If the calendar is locked successfully
   AEE_EPRIVLEVEL  : If the user doens't have proper privilege
   AEE_EFAILED     : otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int CvCalStore_LockCalendar(IvCalStore *pMe)
{
   int nResult = AEE_EFAILED; 
   if (!ISHELL_CheckPrivLevel(pMe->pIShell, AEEPRIVID_vCalStoreLock, TRUE)) {
         return AEE_EPRIVLEVEL;
      }
   do{           
      nResult = IvCalStore_Lock(pMe);      
      if(AEE_SUCCESS == nResult){      
         //notify apps through signal that a record has been added
         nResult = ISignalBus_Strobe(pMe->pILockSignalBus);
      }

   }while(0);


   return nResult;
}
/*===========================================================================

Function:  
   IvCalStore_UnLock()

Description:
		 This function UnLocks the calendar database if proper privileges are 
   available.

Prototype:
   int IvCalStore_UnLock(IvCalStore *pMe);

Parameters:
   pMe: Pointer to IvCalStore interface 

Return Value:
   AEE_SUCCESS : If the calendar is locked successfully
   AEE_EPRIVLEVEL  : If the user doens't have proper privilege
   AEE_EFAILED     : otherwise

Comments:  
   It does all the necessary database operations required for unlocking 
   the database.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_UnLock(IvCalStore *pMe)
{
   int nResult = AEE_EFAILED;   
   char *pszUpdate = "UPDATE LOCK SET Locked = 0";
   ISQLStmt *pStmt = NULL;

   do{
      nResult = ISQL_Prepare(pMe->pISQL, pszUpdate, -1, &pStmt, NULL);
      BREAKIF(nResult);

      nResult = ISQLSTMT_Step(pStmt);
      if( SQL_DONE != nResult) { 
         nResult = AEE_EFAILED;
         break;
      }else{
         nResult = AEE_SUCCESS;
      }      

   }while(0);

   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================

Function:  
   CvCalStore_UnLockCalendar()

Description:
		 This function UnLocks the calendar database if proper privileges are 
   available.

Prototype:
   int CvCalStore_UnLockCalendar(IvCalStore *pMe);

Parameters:
   pMe: Pointer to IvCalStore interface 

Return Value:
   AEE_SUCCESS : If the calendar is locked successfully
   AEE_EPRIVLEVEL  : If the user doens't have proper privilege
   AEE_EFAILED     : otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int CvCalStore_UnLockCalendar(IvCalStore *pMe)
{
   int nResult = AEE_EFAILED;
   
   do{
      if (!ISHELL_CheckPrivLevel(pMe->pIShell, AEEPRIVID_vCalStoreLock, TRUE)) {
         return AEE_EPRIVLEVEL;
      }
      nResult = IvCalStore_UnLock(pMe);
      
   }while(0);
   
   return nResult;   
}
/*===========================================================================

Function:  
   IvCalStore_IsCalendarLocked()

Description:
		 This function checks whether the calendar database is locked or unlocked.

Prototype:
   int IvCalStore_IsCalendarLocked(IvCalStore *pMe,
                                   boolean    *bLocked);

Parameters:
   pMe     :   Pointer to IvCalStore interface 
   bLocked :   Whether the DB is locked or unlocked.
               TRUE - Locked
               FALSE - Unlocked

Return Value:
   AEE_SUCCESS : If the lock/unlock information is retrieved successfully   
   AEE_EFAILED     : otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_CheckCalendarLocked(IvCalStore *pMe,
                                   boolean    *bLocked)
{
   int nResult = AEE_EFAILED, nLock = -1;
   char *pszLock = "SELECT Locked FROM LOCK";    
   ISQLStmt *pStmt = NULL;
   if(NULL == bLocked){
      return AEE_EBADPARM;
   }
   do{
      nResult = ISQL_Prepare(pMe->pISQL, pszLock, -1, &pStmt, NULL);
      BREAKIF(nResult);

      nResult = ISQLSTMT_Step(pStmt);
      if( SQL_ROW != nResult) { 
         nResult = AEE_EFAILED;
         break;         
      }
      nResult = ISQLSTMT_GetColumnInt(pStmt, 0, &nLock);
      BREAKIF(nResult);

      nResult = AEE_SUCCESS;
      if(nLock){
         *bLocked = TRUE;
      }else{
         *bLocked = FALSE;
      }            
   }while(0);

   RELEASEIF(pStmt);

   return nResult;
}
/*===========================================================================

Function:  
   CvCalStore_IsCalendarLocked()

Description:
		 This function checks whether the calendar database is locked or unlocked.

Prototype:
   int CvCalStore_IsCalendarLocked(IvCalStore *pMe, boolean *bLocked);

Parameters:
   pMe     :   Pointer to IvCalStore interface 
   bLocked :   Whether the DB is locked or unlocked.
               TRUE - Locked
               FALSE - Unlocked

Return Value:
   AEE_SUCCESS : If the lock/unlock information is retrieved successfully   
   AEE_EFAILED     : otherwise

Comments:  
   None

Side Effects: 
   None

==============================================================================*/
int CvCalStore_IsCalendarLocked(IvCalStore *pMe, boolean *bLocked)
{
   int nResult = AEE_EFAILED;  

   if(NULL == bLocked){
      return AEE_EBADPARM;
   }
   do{
      nResult = IvCalStore_CheckCalendarLocked(pMe, bLocked);     
               
   }while(0);     

   return nResult;   
}

/*===========================================================================
Function:  
   IvCalStore_GetParms()

Description:
		 This function gets parameters for a particular property.

Prototype:
   int IvCalStore_GetParms(IvCalStore *pMe,
                           IvProperty *pIvProperty,
                           uint32     dwProp,
                           uint32     dwOID,
                           boolean    bIsTimeZone);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   pIvProperty    [in]:  Pointer to property
   dwProp         [in]:  Property ID
   dwOID          [in]:  Object ID
   bIsTimeZone    [in]:  Is timeZone if TRUE the property is 
                         for TimeZone otherwise not.
   
Return Value:
   AEE_SUCCES  : If the parameters are added successfully
   AEE_EFAILED : Otherwise.

Comments:
   None

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetParms(IvCalStore *pMe,
                        IvProperty *pIvProperty,
                        uint32     dwProp,
                        uint32     dwOID,
                        boolean    bIsTimeZone)
{
   char *pszStrParam = "SELECT ParmID, ParmVal FROM "VCALSTORE_PARAM_TBL" WHERE "
                       "RecNumProp = ? AND PropID = ?";
   char *pszStrTZParam = "SELECT ParmID, ParmVal FROM "VCALSTORE_TZPARAM_TBL
                         " WHERE "
                         "RecNumTZProp = ? AND PropID = ?";
   int nResult = AEE_EFAILED;
   uint32 dwParmID = 0;   
   ISQLStmt *pStmt = NULL;
   do{
      if(FALSE == bIsTimeZone){
         nResult = ISQL_Prepare(pMe->pISQL,pszStrParam, -1 ,&pStmt,NULL);
      }else{
         nResult = ISQL_Prepare(pMe->pISQL,pszStrTZParam, -1 ,&pStmt,NULL);
      }
      BREAKIF(nResult);
      (void)ISQLSTMT_BindInt(pStmt,1,(int)dwOID);
      (void)ISQLSTMT_BindInt(pStmt,2,(int)dwProp);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){
         AECHAR *pszParmVal = NULL;
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,(int*)&dwParmID);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnWText(pStmt,1,&pszParmVal);
         BREAKIF(nResult);
         if(NULL != pszParmVal){
            nResult = IvCalStore_GetAndStoreParms(pMe,
                                                   dwParmID,
                                                   pszParmVal,
                                                   pIvProperty);
            
            BREAKIF(nResult);
         }                  
      } 
      if(SQL_DONE == nResult){
         nResult = AEE_SUCCESS;
      }
      
   }while(0);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetExtendedProp()

Description:
		 This function gets extended property for alarm object from the store.

Prototype:
   int IvCalStore_GetExtendedProp(IvCalStore  *pMe,
                                  IvCalObject *pIvAlarm,
                                  uint32      dwOID,
                                  AEECLSID    objType);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   pIvAlarm       [in]:  Pointer to Alarm object
   dwOID          [in]:  Object ID
   objType        [out]: Object type for alarm
   
Return Value:
   AEE_SUCCESS : If the extended property is retrieved.
   AEE_EFAILED : Otherwise

Comments:
   This function gets all the properties and parameters related to a 
   specific VALARM sub component.

Side Effects: 
   None

==============================================================================*/

int IvCalStore_GetExtendedProp(IvCalStore  *pMe,
                               IvCalObject *pIvAlarm,
                               uint32      dwOID,
                               AEECLSID    objType)
{
   int nResult = AEE_EFAILED;
   char *pszStrObjectProp = "SELECT PropID, PropVal, RecordNumber FROM "
                         VCALSTORE_ALARMPROP_TBL
                         " WHERE RecNumAlarmTable = ?";
   ISQLStmt *pStmt = NULL;
   uint32 dwProp = 0, dwRecNumProp = 0;
   do{
      nResult = ISQL_Prepare(pMe->pISQL,pszStrObjectProp, -1 ,&pStmt,NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      BREAKIF(nResult);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){         
         char *pszPropVal = NULL;         
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,(int*)&dwProp); 
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt,2,(int*)&dwRecNumProp);
         BREAKIF(nResult);
         nResult = ISQLSTMT_ColumnSize(pStmt,1);
         if(0 != nResult){
                    
            switch(dwProp){               
               case AEEVPROPERTY_EXTENDED:                  
               case AEEVPROPERTY_ATTENDEE:
               case AEEVPROPERTY_ATTACH:
                  {
                     nResult = ISQLSTMT_GetColumnText(pStmt,1,&(pszPropVal));
                     if(NULL != pszPropVal){                 
                        nResult = IvCalStore_AddProperty(pMe,
                                                         (IvCalObject*)pIvAlarm,
                                                         dwProp,
                                                         pszPropVal,
                                                         STRLEN(pszPropVal),
                                                         dwRecNumProp,
                                                         FALSE,
                                                         objType);
                     }


                     break;
                     

                  }                                 
               default:
                  {
                     nResult = AEE_EBADPARM;
                     break;
                  }

            }                   
            
            BREAKIF(nResult);
         }
      }      
   }while(0);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetTZMultiProp()

Description:
		 This function gets multiple times occuring properties for 
   time zone component.

Prototype:
   int IvCalStore_GetTZMultiProp(IvCalStore  *pMe,
                                 IvCalObject *pIvDaylight,
                                 int         nRecNum,
                                 uint32      dwOID,
                                 boolean     bTZOrDaylight);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface
   pIvDaylight    [in]:  Pointer to time zone daylight or standard
                         sub component object
   nRecNum        [in]:  Record number from Daylight table
   dwOID          [in]:  Object ID 
   bTZOrDaylight  [in]:  Whether TZ table or Daylight

   
Return Value:
   Gets the time zone property and parameters which occur multiple times 
   in daylight and standard sub components.

Comments:
   This function gets all the properties and parameters related to a 
   specific VTIMEZONE standard and daylight sub component which occur
   multiple times.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetTZMultiProp(IvCalStore  *pMe,
                              IvCalObject *pIvDaylight,
                              int         nRecNum,
                              uint32      dwOID,
                              boolean     bTZOrDaylight)
{
   int nResult = AEE_EFAILED;
   char *pszTZMultiProp = "SELECT PropID, PropVal,"
                          "RecordNumber FROM "VCALSTORE_TZPROP_TBL
                          " WHERE RecNumDayLight = ?";
   char *pszTZMultiPropTZTable = "SELECT PropID, PropVal,"
                          "RecordNumber FROM "VCALSTORE_TZPROP_TBL
                          " WHERE RecNumTimeZone = ?";
   char *pszPropVal = NULL;   
   int nRecNumProp = 0;
   uint32 dwPropID = 0;
   ISQLStmt *pStmt = NULL;
   IvProperty *pIvProperty = NULL;
   do{
      if(bTZOrDaylight){
         nResult = ISQL_Prepare(pMe->pISQL,pszTZMultiPropTZTable,-1,&pStmt,NULL);
      }else{
         nResult = ISQL_Prepare(pMe->pISQL,pszTZMultiProp,-1,&pStmt,NULL);
      }
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, nRecNum);
      BREAKIF(nResult);
      while ( SQL_ROW == ISQLSTMT_Step(pStmt)){
         nResult = ISHELL_CreateInstance(pMe->pIShell,
                                         AEECLSID_vProperty,
                                         (void**)&pIvProperty);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt, 0, (int*)&dwPropID);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt, 2, (int*)&nRecNumProp);
         BREAKIF(nResult);         
         nResult = ISQLSTMT_ColumnSize(pStmt,1);
         if(0 < nResult){            
            nResult = ISQLSTMT_GetColumnText(pStmt, 1, &pszPropVal);
            BREAKIF(nResult);
            if (AEEVPROPERTY_EXTENDED == dwPropID){
               char *pszPropName = NULL, *pszNameValueSep = NULL, *pszTemp = NULL;
               int nLen = 0;  
               pszTemp = (char*)pszPropVal;
               pszNameValueSep = STRCHR(pszTemp,':');
               if(NULL == pszNameValueSep){
                  nResult = AEE_EFAILED;
                  break;
               }
               nLen = pszNameValueSep - pszTemp + 1;
               pszPropName = (char*)MALLOC(nLen);
               if(NULL == pszPropName){
                  nResult = AEE_ENOMEMORY;
                  break;
               }
               STRLCPY((char*)pszPropName, (char*)pszTemp, nLen); 
               pszNameValueSep++;
               if(NULL != pszPropName && NULL != pszNameValueSep){

                  nResult = IvProperty_InitProperty(pIvProperty, 
                                                   dwPropID,
                                                   pszPropName,
                                                   FALSE,
                                                   pszNameValueSep,
                                                   STRLEN(pszNameValueSep),
                                                   NULL);
               } else {
                  nResult = AEE_EFAILED;
                  break;
               }
               FREEIF(pszPropName);
               
            }else {
               nResult = IvProperty_InitProperty(pIvProperty,
                                                   dwPropID,
                                                   NULL,
                                                   FALSE,
                                                   pszPropVal,
                                                   STRLEN(pszPropVal),
                                                   NULL);
            }
            
            BREAKIF(nResult);
         }                          
         nResult = IvCalStore_GetParms(pMe,
                                       pIvProperty,
                                       dwPropID,
                                       (uint32)nRecNumProp,
                                       TRUE);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvDaylight,pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
      }

   }while(0);
   
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetDaylightStandard()

Description:
		 This function gets daylight/standard sub components from 
   time zone component.

Prototype:
   int IvCalStore_GetDaylightStandard(IvCalStore *pMe,
                                      IvCalObject *pIvTimeZone,
                                      uint32      dwRecNum,
                                      uint32      dwOID);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface
   pIvTimeZone    [in]:  Pointer to time zone object
   dwRecNum       [in]:  Record number from Time zone table
   dwOID          [in]:  Object ID   
   
Return Value:
   Gets the time zone property and parameters for daylight and standard
   sun components.

Comments:
   This function gets all the properties and parameters related to a 
   specific VTIMEZONE standard and daylight sub component.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetDaylightStandard(IvCalStore *pMe,
                                   IvCalObject *pIvTimeZone,
                                   uint32      dwRecNum,
                                   uint32      dwOID)
{
   int nResult = AEE_EFAILED, nType = 0, nRecNum = 0;
   int32 nTZOffset1 = 0, nTZOffset2 = 0;   
   char *pszGetDaylight = "SELECT Type, DateStart, RecordNumber, "
                          " TZOffsetFrom, TZOffsetTo FROM "
                          VCALSTORE_DAYLIGHT_TBL" WHERE RecNumTimeZone = ?";
   char *pszDateStart = NULL;
   ISQLStmt *pStmt = NULL;
   IvCalObject *pIvDaylight = NULL;   
   IvProperty *pIvProperty = NULL;
   do{
      nResult = ISQL_Prepare(pMe->pISQL,pszGetDaylight,-1,&pStmt,NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwRecNum);
      BREAKIF(nResult);
      while (SQL_ROW == ISQLSTMT_Step(pStmt)){         
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nType);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt,2,&nRecNum);
         BREAKIF(nResult);
         if(AEECLSID_vDaylight == nType){
            nResult = ISHELL_CreateInstance(pMe->pIShell,
                                            AEECLSID_vDaylight,
                                            (void**)&pIvDaylight);            
         }else{
            nResult = ISHELL_CreateInstance(pMe->pIShell,
                                            AEECLSID_vStandard,
                                            (void**)&pIvDaylight); 

         }
         BREAKIF(nResult);
         nResult = ISQLSTMT_ColumnSize(pStmt, 1);
         if(0 < nResult){
                   
            nResult = ISQLSTMT_GetColumnText(pStmt, 1, &pszDateStart);
            BREAKIF(nResult);
            nResult = ISHELL_CreateInstance( pMe->pIShell,
                                             AEECLSID_vProperty,
                                             (void**)&pIvProperty);
            BREAKIF(nResult);
            nResult = IvProperty_InitProperty(pIvProperty,
                                              AEEVPROPERTY_DTSTART,
                                              NULL,
                                              FALSE,
                                              pszDateStart,
                                              STRLEN(pszDateStart),
                                              NULL); 
            
            BREAKIF(nResult);
            nResult = IvCalStore_GetTimeZoneParms(pMe,
                                                  pIvProperty,
                                                  AEEVPROPERTY_DTSTART,
                                                  (uint32)nRecNum,
                                                  TRUE);
            BREAKIF(nResult);
            nResult = IvCalObject_AddProperty(pIvDaylight, pIvProperty);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
         }
         nResult = ISQLSTMT_GetColumnInt(pStmt, 3, (int*)&nTZOffset1);
         BREAKIF(nResult);         
         nResult = ISHELL_CreateInstance(pMe->pIShell,
                                          AEECLSID_vProperty,
                                          (void**)&pIvProperty);
         BREAKIF(nResult);
         nResult = IvProperty_InitProperty(pIvProperty,
                                          AEEVPROPERTY_TZOFFSETFROM,
                                          NULL,
                                          TRUE,
                                          &nTZOffset1,
                                          sizeof(int32),
                                          NULL);         
         BREAKIF(nResult);
         nResult = IvCalStore_GetTimeZoneParms( pMe,
                                                pIvProperty,
                                                AEEVPROPERTY_TZOFFSETFROM,
                                                (uint32)nRecNum,
                                                TRUE);
         BREAKIF(nResult);
         nResult = IvCalObject_AddProperty(pIvDaylight, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         nTZOffset2 = 0;
         nResult = ISQLSTMT_GetColumnInt(pStmt,4,(int*)&nTZOffset2);
         BREAKIF(nResult);         
         nResult = ISHELL_CreateInstance(pMe->pIShell,
                                         AEECLSID_vProperty,
                                         (void**)&pIvProperty);
         BREAKIF(nResult);
         nResult = IvProperty_InitProperty(pIvProperty,
                                          AEEVPROPERTY_TZOFFSETTO,
                                          NULL,
                                          TRUE,
                                          &nTZOffset2,
                                          sizeof(int32),
                                          NULL);         
         BREAKIF(nResult);
         nResult = IvCalStore_GetTimeZoneParms( pMe,
                                                pIvProperty,
                                                AEEVPROPERTY_TZOFFSETTO,
                                                (uint32)nRecNum,
                                                TRUE);
         BREAKIF(nResult);
         nResult = IvCalObject_AddProperty(pIvDaylight,pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);         
         nResult = IvCalStore_GetTZMultiProp(pMe,
                                             pIvDaylight,
                                             nRecNum,
                                             dwOID,
                                             FALSE);
         BREAKIF(nResult);
         
         nResult = IvCalObject_AddObject(pIvTimeZone,pIvDaylight);
         BREAKIF(nResult);
         RELEASEIF(pIvDaylight);
      }

   }while(0);
   
   RELEASEIF(pIvProperty);
   RELEASEIF(pIvDaylight);
   RELEASEIF(pStmt);

   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetTimeZoneParms()

Description:
		 This function gets time zone component parameters from timezone 
   param table.

Prototype:
   int IvCalStore_GetTimeZoneParms(IvCalStore    *pMe,
                                   IvProperty    *pIvProperty,
                                   AEEVProperty  dwProp,
                                   uint32        dwRecNum,
                                   boolean       bDayLight);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   pIvProperty    [in]:  Property for which params to be retrieved
   dwProp         [in]:  Property ID
   dwRecNum       [in]:  Record Number from Timezone or DayLight tables
   bDayLight      [in]:  Whether DayLight or not.
   
Return Value:
   Gets the time zone parameters.

Comments:
   This function gets parameters related to a 
   specific VTIMEZONE sub component.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetTimeZoneParms(IvCalStore    *pMe,
                                IvProperty    *pIvProperty,
                                AEEVProperty  dwProp,
                                uint32        dwRecNum,
                                boolean       bDayLight)
{
   int nResult = AEE_EFAILED;
   ISQLStmt *pStmt = NULL;
   IvParm *pIvParm = NULL;
   uint32 dwParmID = 0;
   char *pszStrTZParam = "SELECT ParmID, ParmVal FROM "VCALSTORE_TZPARAM_TBL
                       " WHERE RecNumTZTable = ? AND PropID = ?"; 
   char *pszStrDayLightParam = "SELECT ParmID, ParmVal FROM "
                               VCALSTORE_TZPARAM_TBL
                              " WHERE RecNumDayLightTable = ? AND PropID = ?"; 
   do{
      if(!bDayLight){
         nResult = ISQL_Prepare(pMe->pISQL,pszStrTZParam,-1,&pStmt,NULL);
      }else{
         nResult = ISQL_Prepare(pMe->pISQL,pszStrDayLightParam,-1,&pStmt,NULL);
      }
      BREAKIF(nResult);
      
      (void)ISQLSTMT_BindInt(pStmt, 1, (int)dwRecNum);
      (void)ISQLSTMT_BindInt(pStmt, 2, (int)dwProp);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){
         AECHAR *pszParmVal = NULL;
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,(int*)&dwParmID);
         BREAKIF(nResult);
         
         nResult = ISQLSTMT_GetColumnWText(pStmt,1,&pszParmVal);
         BREAKIF(nResult);
         if(NULL != pszParmVal){
            nResult = IvCalStore_GetAndStoreParms(pMe,
                                                   dwParmID,
                                                   pszParmVal,
                                                   pIvProperty);
            BREAKIF(nResult);
         }  
      }
                                    

   }while(0);
   RELEASEIF(pIvParm);   
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetTimeZone()

Description:
		 This function gets time zone component.

Prototype:
   int IvCalStore_GetTimeZone(IvCalStore  *pMe,
                              uint32      dwOID,
                              IvCalObject *pObject);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   dwOID          [in]:  Object ID
   pObj           [out]: Object for which Time Zone to be returned
   
Return Value:
   Gets the time zone property and parameters.

Comments:
   This function gets all the properties and parameters related to a 
   specific VTIMEZONE sub component.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetTimeZone(IvCalStore  *pMe,
                           uint32      dwOID,
                           IvCalObject *pObject)
{
   int nResult = AEE_EFAILED;
   char *pszTimezone = "SELECT RecordNumber, TZID, TZURL, LastModified "
                       "FROM "VCALSTORE_TIMEZONE_TBL
                       " WHERE UID = ?";
   IvCalObject *pIvTimeZone = NULL;
   char *pszTZID = NULL, *pszTZURL = NULL, *pszLastModified = NULL;
   uint32 dwRecNum = 0;
   ISQLStmt *pStmt = NULL;   
   IvProperty *pIvProperty = NULL;
   do{
      nResult = ISQL_Prepare(pMe->pISQL,pszTimezone,-1,&pStmt,NULL);
      BREAKIF(nResult);      
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      BREAKIF(nResult);
      while (SQL_ROW == ISQLSTMT_Step(pStmt)){         
         nResult = ISHELL_CreateInstance(pMe->pIShell,
                                         AEECLSID_vTimeZone,
                                         (void**)&pIvTimeZone);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt, 0, (int*)&dwRecNum);
         BREAKIF(nResult);
         nResult = ISQLSTMT_ColumnSize(pStmt,1);
         if(0 < nResult){            
            nResult = ISQLSTMT_GetColumnText(pStmt,1, &pszTZID);
            BREAKIF(nResult);
            nResult = ISHELL_CreateInstance(pMe->pIShell,
                                            AEECLSID_vProperty,
                                            (void**)&pIvProperty);
            BREAKIF(nResult);
            nResult = IvProperty_InitProperty(pIvProperty,
                                              AEEVPROPERTY_TZID,
                                              NULL,
                                              FALSE,
                                              pszTZID,
                                              STRLEN(pszTZID),
                                              NULL);
            
            BREAKIF(nResult);
            nResult = IvCalStore_GetTimeZoneParms(pMe,
                                                  pIvProperty,
                                                  AEEVPROPERTY_TZID,
                                                  dwRecNum,
                                                  FALSE);
            BREAKIF(nResult);
            nResult = IvCalObject_AddProperty(pIvTimeZone,pIvProperty);
            BREAKIF(nResult);            
            RELEASEIF(pIvProperty);            
         }
         nResult = ISQLSTMT_ColumnSize(pStmt,2);
         if(0 < nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,2, &pszTZURL);
            BREAKIF(nResult);
            nResult = ISHELL_CreateInstance(pMe->pIShell,
                                            AEECLSID_vProperty,
                                            (void**)&pIvProperty);
            BREAKIF(nResult);
            nResult = IvProperty_InitProperty(pIvProperty,
                                              AEEVPROPERTY_TZURL,
                                              NULL,
                                              FALSE,
                                              pszTZURL,
                                              STRLEN(pszTZURL),
                                              NULL);
            BREAKIF(nResult);
            nResult = IvCalStore_GetTimeZoneParms(pMe,
                                                  pIvProperty,
                                                  AEEVPROPERTY_TZURL,
                                                  dwRecNum,
                                                  FALSE);
            BREAKIF(nResult);
            nResult = IvCalObject_AddProperty(pIvTimeZone,pIvProperty);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
            
            
         }
         nResult = ISQLSTMT_ColumnSize(pStmt,3);
         if(0 < nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,3, &pszLastModified);
            BREAKIF(nResult);
            nResult = ISHELL_CreateInstance(pMe->pIShell,
                                            AEECLSID_vProperty,
                                            (void**)&pIvProperty);
            BREAKIF(nResult);
            nResult = IvProperty_InitProperty(pIvProperty,
                                              AEEVPROPERTY_LASTMODIFIED,
                                              NULL,
                                              FALSE,
                                              pszLastModified,
                                              STRLEN(pszLastModified),
                                              NULL);
            BREAKIF(nResult);
            nResult = IvCalStore_GetTimeZoneParms(pMe,
                                                  pIvProperty,
                                                  AEEVPROPERTY_LASTMODIFIED,
                                                  dwRecNum,
                                                  FALSE);
            BREAKIF(nResult);
            nResult = IvCalObject_AddProperty(pIvTimeZone,pIvProperty);
            RELEASEIF(pIvProperty);
            BREAKIF(nResult);
            
            
         }          
         nResult = IvCalStore_GetTZMultiProp(pMe,
                                             pIvTimeZone,
                                             dwRecNum,
                                             dwOID,
                                             TRUE);
         BREAKIF(nResult);

         nResult = IvCalStore_GetDaylightStandard(pMe,
                                                  pIvTimeZone,
                                                  dwRecNum,
                                                  dwOID);
         BREAKIF(nResult);
         
         nResult = IvCalObject_AddObject(pObject, pIvTimeZone);
         RELEASEIF(pIvTimeZone);
         BREAKIF(nResult);                                                  
      }

   }while(0);
   
   RELEASEIF(pIvTimeZone);
   RELEASEIF(pIvProperty);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetAlarm()

Description:
		 This function gets alarm objects from the store for an Event, Todo or
   Journal.

Prototype:
   int IvCalStore_GetAlarm(IvCalStore     *pMe,
                           uint32         dwOID,                            
                           IvCalObject       **pObj);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   dwOID          [in]:  Object ID
   pObj           [out]: Object to be returned
   
Return Value:
   Gets the alarm property and parameters for the Alarm object.

Comments:
   This function gets all the properties and parameters related to a 
   specific VALARM sub component.

Side Effects: 
   None

==============================================================================*/
int IvCalStore_GetAlarm(IvCalStore *pMe,
                        uint32      dwOID,                            
                        IvCalObject *pObj)
{
   int nResult = AEE_EFAILED, nRepeat = 0;
   IvCalObject *pIvAlarm = NULL;
   char *pszStrAlarm = "SELECT Summary,Description,"
                       "Action,Duration,Repeat,Trigger,RecordNumber FROM "
                       VCALSTORE_ALARM_TBL" WHERE UID = ?";
   char   *pszSummary = NULL, *pszAction = NULL,
          *pszDesc = NULL, *pszDuration = NULL,
          *pszTrigger = NULL;
   
   ISQLStmt *pStmt = NULL;  
   uint32 dwRecNum = 0;
   AEECLSID objType = AEECLSID_vAlarm;   
   if(NULL == pObj){
      return AEE_EBADPARM;
   }
   do{      
      nResult = ISQL_Prepare(pMe->pISQL,pszStrAlarm, -1 ,&pStmt,NULL);
      BREAKIF(nResult); 
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){
         nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                          AEECLSID_vAlarm, 
                                          (void**)&pIvAlarm);
         BREAKIF(nResult);           
         nResult = ISQLSTMT_GetColumnInt(pStmt, 6, (int*)&dwRecNum);
         BREAKIF(nResult);
         nResult = ISQLSTMT_ColumnSize(pStmt,0);         
         if(0 != nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,0,&pszSummary); 
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_SUMMARY,
                                             pszSummary,
                                             STRLEN(pszSummary),
                                             dwRecNum,
                                             TRUE,
                                             objType);
            
            BREAKIF(nResult);
         }
         nResult = ISQLSTMT_ColumnSize(pStmt,1);         
         if(0 != nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,1,&pszDesc); 
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_DESCRIPTION,
                                             pszDesc,
                                             STRLEN(pszDesc),
                                             dwRecNum,
                                             TRUE,
                                             objType);
            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_ColumnSize(pStmt,2);         
         if(0 != nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,2,&pszAction); 
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_ACTION,
                                             pszAction,
                                             STRLEN(pszAction),
                                             dwRecNum,
                                             TRUE,
                                             objType);
            
            BREAKIF(nResult);
         }
         
         nResult = ISQLSTMT_ColumnSize(pStmt,3);
         if(0 != nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,3,&pszDuration);
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_DURATION,
                                             pszDuration,
                                             STRLEN(pszDuration),
                                             dwRecNum,
                                             TRUE,
                                             objType);
            

            
            BREAKIF(nResult);
         }
         nResult = ISQLSTMT_ColumnSize(pStmt,4);
         if(0 != nResult){
            nResult = ISQLSTMT_GetColumnInt(pStmt,4,(int*)&nRepeat);
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_REPEAT,
                                             &nRepeat,
                                             sizeof(uint32),
                                             dwRecNum,
                                             TRUE,
                                             objType); 

            BREAKIF(nResult);            
         }
         nResult = ISQLSTMT_ColumnSize(pStmt,5);
         if(0 != nResult){
            
            nResult = ISQLSTMT_GetColumnText(pStmt,5,&pszTrigger);
            BREAKIF(nResult);
            nResult = IvCalStore_AddProperty(pMe,
                                             (IvCalObject*)pIvAlarm,
                                             AEEVPROPERTY_TRIGGER,
                                             pszTrigger,
                                             STRLEN(pszTrigger),
                                             dwRecNum,
                                             TRUE,
                                             objType);            

            
            BREAKIF(nResult);
         }    
         //Get all objects which occur more than once for Alarm object
         nResult = IvCalStore_GetExtendedProp(pMe, pIvAlarm, dwRecNum, objType);    
         if(AEE_SUCCESS != nResult){
            break;
         } 
         nResult = IvCalObject_AddObject(pObj,pIvAlarm);         
         RELEASEIF(pIvAlarm);
         
         BREAKIF(nResult);
      }//while
      if(AEE_SUCCESS != nResult){
         break;
      }
      
      
   }while(0);   
   
   RELEASEIF(pIvAlarm);
   RELEASEIF(pStmt);
   return nResult;

}
/*===========================================================================
Function:  
   IvCalStore_GetMultipleProp()

Description:
		 This function gets the multiple occuring properties from store.

Prototype:
   int IvCalStore_GetMultipleProp(IvCalStore  *pMe,
                                  IvCalObject *pObject,
                                  uint32      dwOID,
                                  AEECLSID    objType);
                           

Parameters:
   pMe            [in]:  Pointer to the IvCalStore interface 
   pObject        [in]:  pointer to component object
   dwOID          [in]:  Object ID   
   objType        [in]:  Object type
Return Value:
   Gets the parameters for the property.

Comments:
   None

Side Effects: 
   None

==============================================================================*/

int IvCalStore_GetMultipleProp(IvCalStore  *pMe,
                               IvCalObject *pObject,
                               uint32      dwOID,
                               AEECLSID    objType)
{
   int nResult = AEE_EFAILED, nSize = 0;
   uint32 dwProp = 0, dwRecNumProp = 0;
   char *pszStrObjectProp = "SELECT PropID, PropVal, RecordNumber FROM "
                            VCALSTORE_PROP_TBL" WHERE UID = ?";
   char *pszPropVal = NULL;
   ISQLStmt *pStmt = NULL;
   do{
      nResult = ISQL_Prepare(pMe->pISQL,pszStrObjectProp, -1 ,&pStmt,NULL);
      BREAKIF(nResult);
      nResult = ISQLSTMT_BindInt(pStmt, 1, (int)dwOID);
      BREAKIF(nResult);
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){
         pszPropVal = NULL;
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,(int*)&dwProp);
         BREAKIF(nResult);
         nResult = ISQLSTMT_GetColumnInt(pStmt,2,(int*)&dwRecNumProp);
         BREAKIF(nResult);
         nSize = ISQLSTMT_ColumnSize(pStmt,1);
         if(0 != nSize){
                       
            switch(dwProp){               
               case AEEVPROPERTY_ATTACH:
               case AEEVPROPERTY_COMMENT:
               case AEEVPROPERTY_CONTACT:
               case AEEVPROPERTY_CATEGORIES:
               case AEEVPROPERTY_RSTATUS:
               case AEEVPROPERTY_RELATED:
               case AEEVPROPERTY_RESOURCES:
               case AEEVPROPERTY_EXTENDED:
               case AEEVPROPERTY_EXDATE:
               case AEEVPROPERTY_RDATE:
               case AEEVPROPERTY_EXRULE:
               case AEEVPROPERTY_RRULE:
               case AEEVPROPERTY_ATTENDEE:
               case AEEVPROPERTY_ORGANIZER:
                  {
                     nResult = ISQLSTMT_GetColumnText(pStmt,1,&(pszPropVal));
                    
                     nResult = IvCalStore_AddProperty(pMe,
                                                      (IvCalObject*)pObject,
                                                      dwProp,
                                                      pszPropVal,
                                                      STRLEN(pszPropVal),
                                                      dwRecNumProp,
                                                      FALSE,
                                                      objType);
                     break;

                  }                                                   
               default:
                  {
                     break;
                  }

            }                   
            
            BREAKIF(nResult);
         }
      }      
      
   }while(0);
   
   RELEASEIF(pStmt);
   return nResult;
}


/*===========================================================================
Function:  CvCalStore_GetObjectOIDs()

Description:
   This function deletes the object specified by UID  .

Prototype:
	  int CvCalStore_GetObjectOIDs( IvCalStore     *pIvCalStore, 
                                 AEEvObjectType RecType,
                                 AEEVProperty   *pnAEEPropertyOIDs,
                                 int            nSize,
                                 int            *pnReqSize);

Parameters:
   pIvCalStore       [in]    :  Pointer to the IvCalStore interface 
   RecType           [in]    :  The record type.   
   pnAEEPropertyOIDs [out]   :  OIDS returned
   nSize             [in]    :  Size of the UID buffer
   pnReqSize         [in/out]:  Required size
Return Value:
   Gets the number of records for a specific component type.

Comments:
   None

Side Effects: 
   None

==============================================================================*/
int CvCalStore_GetObjectOIDs( IvCalStore       *pIvCalStore, 
                              AEECLSID         RecType,
                              AEEVProperty     *pnAEEPropertyOIDs,                                                            
                              int              nSize,
                              int              *pnReqSize)
{
   int nResult = AEE_EFAILED;       
   AEEVProperty *pnListOfOIDs = pnAEEPropertyOIDs;
   int nCount = 0, nActualSize = 0;
   boolean bLocked = FALSE;
   IvCalStore *pMe = pIvCalStore;   
             
   const char *pszObjectCount = "SELECT COUNT(*) FROM "VCALSTORE_MAIN_TBL
                            " WHERE Type = ?";
          
   const char *pszObjectUID = "SELECT RecordNumber"
                              " FROM "VCALSTORE_MAIN_TBL
                              " WHERE Type = ?";
   
   //If RecType = 0 return total number of records
   //in the table for specified RecType.
   
   const char *pszFullObjectCount = "SELECT COUNT(*) FROM "VCALSTORE_MAIN_TBL;

   const char *pszFullObjectUID = "SELECT RecordNumber "
                                  "FROM "VCALSTORE_MAIN_TBL;                           
   ISQLStmt *pStmt = NULL;
   if(NULL == pnReqSize){
      return AEE_EBADPARM;
   }

   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pIvCalStore,&bLocked);
   if(AEE_SUCCESS == nResult){
      //if DB is locked the app can't perform the add operation.
      //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
      //then try for addition.
      if(bLocked){
         return AEE_ENOTALLOWED;
      }
   }else{
      return AEE_EFAILED;
   }

   //First check for app privileges.
   if (!ISHELL_CheckPrivLevel(pIvCalStore->pIShell, 
                              AEEPRIVID_vCalStoreRead, 
                              TRUE)) {
      return AEE_EPRIVLEVEL;
   }
   

   do{
      //if all record OIDs are to be retrieved
      if(0 == RecType){
         //if the OIDs list pointer is NULL we return only the 
         //number of elements which would be returned
         if(NULL == pnAEEPropertyOIDs){
            nResult = ISQL_Prepare(pMe->pISQL,pszFullObjectCount,-1,&pStmt,NULL);
            BREAKIF(nResult);
            nResult = ISQLSTMT_Step(pStmt);
            if(SQL_ROW != nResult){
               *pnReqSize = 0;
               break;
            }else{
               nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
               BREAKIF(nResult);
               *pnReqSize = nCount;
               break;
            }
         }else{
            //if the OIDs list pointer is not NULL we return the 
            //OIDs            
            nResult = ISQL_Prepare(pMe->pISQL,pszFullObjectUID,-1,&pStmt,NULL);
            BREAKIF(nResult);
            while(SQL_ROW == ISQLSTMT_Step(pStmt)){
               if(nSize > nActualSize){
                  nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
                  BREAKIF(nResult);
                  *pnListOfOIDs++ = (uint32)nCount;
                  nActualSize++;
               }else{
                  nResult = AEE_ENEEDMORE;
                  RELEASEIF(pStmt);
                  //Get the required size and return it to the user with error
                  //AEE_ENEEDMORE
                  nResult = ISQL_Prepare(pMe->pISQL,pszFullObjectCount,-1,&pStmt,NULL);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_Step(pStmt);
                  if(SQL_ROW != nResult){
                     *pnReqSize = 0;                     
                  }else{
                     nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
                     BREAKIF(nResult);
                     *pnReqSize = nCount;                     
                  }
                  break;
               }
            }
            BREAKIF(nResult);
         }
      }else{
         //if the OIDs list pointer is NULL we return only the 
         //number of bytes required
         if(NULL == pnAEEPropertyOIDs){
            nResult = ISQL_Prepare(pMe->pISQL,pszObjectCount,-1,&pStmt,NULL);
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 1, (int)RecType);
            BREAKIF(nResult);
            nResult = ISQLSTMT_Step(pStmt);
            if(SQL_ROW != nResult){
               *pnReqSize = 0;
               break;
            }else{
               nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
               BREAKIF(nResult);
               *pnReqSize = nCount;
               break;
            }
         }else{
            //if the OIDs list pointer is not NULL we return the 
            //OIDs
            nResult = ISQL_Prepare(pMe->pISQL,pszObjectUID,-1,&pStmt,NULL);
            BREAKIF(nResult);
            nResult = ISQLSTMT_BindInt(pStmt, 1, (int)RecType);
            BREAKIF(nResult);
            while(SQL_ROW == ISQLSTMT_Step(pStmt)){
               if(nSize > nActualSize){
                  nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
                  BREAKIF(nResult);
                  *pnListOfOIDs++ = (uint32)nCount;
                  nActualSize++;
               }else{
                  nResult = AEE_ENEEDMORE;
                  RELEASEIF(pStmt);
                  //Get the required size and return it to the user with error
                  //AEE_ENEEDMORE
                  nResult = ISQL_Prepare(pMe->pISQL,pszObjectCount,-1,&pStmt,NULL);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_BindInt(pStmt, 1, (int)RecType);
                  BREAKIF(nResult);
                  nResult = ISQLSTMT_Step(pStmt);
                  if(SQL_ROW != nResult){
                     *pnReqSize = 0;
                     break;
                  }else{
                     nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nCount);
                     BREAKIF(nResult);
                     *pnReqSize = nCount;                     
                  }
                  break;
               }               
            }
            BREAKIF(nResult);
         }
      }
   }while (0);
   RELEASEIF( pStmt);   
   return nResult;
}
/*===========================================================================
Function:  
   IvCalStore_GetOIDByUID()

Description:
   This function gets Object ID from UID.

Prototype:
   int IvCalStore_GetOIDByUID(IvCalStore    *pMe, 
                              char          *pszUID,                              
                              AEEVProperty  *pnOID);

Parameters:
   pMe            [in] : IvCalStore pointer
   pszUID         [in] : UID in string format.    
   pnOID          [out]: Object IDs for the UID.
   

Return Value:
   Returns whether the OID is retrieved successfully or not.
   It does all the SQL database related queries and returns back
   the OID for the UID provided.

Comments: 
   None

Side Effects:
   None

==============================================================================*/
int IvCalStore_GetOIDByUID(IvCalStore    *pMe, 
                           char          *pszUID,                              
                           AEEVProperty  *pnOID)
{
   int nResult = AEE_EFAILED; 
   int nOID = 0;
   char *pszStr = "SELECT RecordNumber FROM "VCALSTORE_MAIN_TBL
                  " WHERE UID = ?";
   ISQLStmt *pStmt = NULL;
   if(NULL == pszUID || NULL == pnOID){
      return AEE_EBADPARM;
   }
   do{
      *pnOID = 0;
      nResult = ISQL_Prepare(pMe->pISQL, pszStr, -1, &pStmt, NULL);
      BREAKIF(nResult);

      nResult = ISQLSTMT_BindText(pStmt,1,pszUID,-1,NULL);
      BREAKIF(nResult);
      //We would return only the first OID if there are more than
      //one OIDs for a particular UID. This won't be the case 
      //as UIDs are unique throughout the calendar database.
      while(SQL_ROW == ISQLSTMT_Step(pStmt)){            
         nResult = ISQLSTMT_GetColumnInt(pStmt,0,&nOID);
         *pnOID = nOID;
         nResult = AEE_SUCCESS;
         break;
      }       
   }while(0);
   RELEASEIF(pStmt);
   return nResult;
}
/*===========================================================================
Function:  
   CvCalStore_GetOIDByUID()

Description:
   This function gets Object ID from UID.

Prototype:
   int CvCalStore_GetOIDByUID(IvCalStore    *pMe, 
                              char          *pszUID,                              
                              AEEVProperty  *pnOID)

Parameters:
   pMe            [in] : IvCalStore pointer
   pszUID         [in] : UID in string format.    
   pnOID          [out]: Object IDs for the UID.
   

Return Value:
   Returns whether the OID is retrieved successfully or not.

Comments: 
   None

Side Effects:
   None

==============================================================================*/
int CvCalStore_GetOIDByUID(IvCalStore   *pMe, 
                           char         *pszUID,                            
                           AEEVProperty *pnOID)
{
   int nResult = AEE_EFAILED;       
   boolean bLocked = FALSE;
   if(NULL == pszUID || NULL == pnOID){
      return AEE_EBADPARM;
   }
   
   //First check for app privileges.
   if (!ISHELL_CheckPrivLevel(pMe->pIShell, AEEPRIVID_vCalStoreRead, TRUE)) {
      return AEE_EPRIVLEVEL;
   }
   //Check whether the database is locked or not.
   nResult = IvCalStore_IsCalendarLocked(pMe,&bLocked);
   //if DB is locked the app can't perform the add operation.
   //it has to first unlock the DB with IvCalStore_UnLockCalendar() API and
   //then try for addition.
   if(AEE_SUCCESS == nResult){
      if(bLocked){
         return AEE_ENOTALLOWED;
      }
   }else{
      return AEE_EFAILED;
   }
   
   do{
      nResult = IvCalStore_GetOIDByUID(pMe,
                                       pszUID,
                                       pnOID);
      
   }while(0);

   return nResult;
}

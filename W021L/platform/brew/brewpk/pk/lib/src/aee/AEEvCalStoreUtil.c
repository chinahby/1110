/*===========================================================================

FILE: 
   AEEvCalStoreUtil.c

SERVICES: 
   Calendar Store utility functions

DESCRIPTION
   This file contains a utility functions for IvCalStore interface.

PUBLIC CLASSES:  
   N/A


   	   Copyright © 2007-2008 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEStdlib.h"
#include "AEEIvCalStore.h"
#include "AEESQL.h"
#include "AEECLSID_SQLMGR.bid"
#include "AEEIvCalStoreUtil.h"
#include "AEEIvProperty.h"
#include "AEEFile.h"

/*=============================================================================
                    FUNCTION PROTOTYPES

=============================================================================*/

static int AEEV_InitPropertyGivenIdValue(IShell             *pMe,                                         
                                         IvProperty         **ppIvProperty,
                                         AEEVProperty       nPropId,
                                         void               *pValue);
static int IvCalStore_ValidateMailbox( IvCalStore *pMe, boolean *pbPresent );
/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */

/*=============================================================================
Function: 
   IvCalStore_OpenCalendarDB

Description:   
   Opens the calendar file for edit.

Prototype : 
   int IvCalStore_OpenCalendarDB( IvCalStore *pMe );

Parameters:
   *pMe    [in]:  Newly created IvCalStore interface
  
Return Value:
   int:  BREW result code

Comments:
   None

Side Effects:
   None

=============================================================================*/

int IvCalStore_OpenCalendarDB( IvCalStore *pMe )
{

   int nResult = AEE_EFAILED;
   ISQLMgr  *pSQLMgr = NULL;
   boolean bPresent = FALSE;
   ISQLStmt *pStmt = NULL;
   char *pLockStr = "SELECT Locked FROM LOCK";
   char *pInsertLock = "INSERT INTO LOCK (Locked) VALUES (0)";   
  
   do {
      if(NULL == pMe){
         nResult = AEE_EFAILED;
      }
      nResult = ISHELL_CreateInstance(
                            pMe->pIShell, AEECLSID_SQLMGR, (void**)&pSQLMgr);

      if (AEE_SUCCESS != nResult) {
         break;
      }
      nResult = IvCalStore_ValidateMailbox(pMe, &bPresent);
      BREAKIF(nResult);    

      nResult = ISQLMGR_Open( pSQLMgr, CALENDAR, &pMe->pISQL , 0 );
      
      if ( AEE_SUCCESS != nResult ) {
         RELEASEIF(pMe->pISQL);
         break;
      }
      //Enabling BREW_reserved_fs_bytes PRAGMA with 5000,
      //to make sure successful deletion even in case of less FREE EFS.
      (void)ISQL_Exec(pMe->pISQL, "PRAGMA BREW_reserved_fs_bytes=5000",
                      NULL, NULL, NULL);
      if(!bPresent){
         
         nResult = ISQL_Exec(pMe->pISQL,"CREATE TABLE MAIN (DayLight TEXT, TimeZone NUMERIC, Type NUMERIC, RNum NUMERIC, Transp NUMERIC, Sequence NUMERIC, LastModified NUMERIC, DateCompleted NUMERIC, DateCreated NUMERIC, DateEnd NUMERIC, TZID TEXT, URL TEXT, RecurId TEXT, Organizer TEXT, Duration NUMERIC, DateStart NUMERIC, Due NUMERIC, Summary TEXT, Status TEXT, Priority NUMERIC, PercentComplete NUMERIC, Location TEXT, Geo TEXT, Description TEXT, Class TEXT, DateTime NUMERIC, UID TEXT, RecordNumber INTEGER PRIMARY KEY);"
                                        "CREATE TABLE ALARM (Summary TEXT, Description TEXT, RecordNumber INTEGER PRIMARY KEY, Action TEXT, AudioContent TEXT, Duration NUMERIC, Repeat NUMERIC, Snooze NUMERIC, Trigger NUMERIC, Type NUMERIC, UID NUMERIC);"
                                        "CREATE TABLE DAYLIGHT (RecNumTimeZone NUMERIC, Type NUMERIC, DateStart NUMERIC, RecordNumber INTEGER PRIMARY KEY, TZOffsetFrom TEXT, TZOffsetTo TEXT, UID NUMERIC);"
                                        "CREATE TABLE LOCK (Locked NUMERIC);"
                                        "CREATE TABLE TIMEZONEPARAM (RecNumTZTable NUMERIC, RecNumDayLightTable NUMERIC, PropID NUMERIC, UID NUMERIC, ParmID NUMERIC, ParmVal WTEXT, RecNumTZProp NUMERIC, RecordNumber INTEGER PRIMARY KEY);"
                                        "CREATE TABLE TIMEZONEPROP (RecNumTimeZone NUMERIC, UID NUMERIC, PropID NUMERIC, PropVal TEXT, RecNumDayLight NUMERIC, RecordNumber INTEGER PRIMARY KEY);"
                                        "CREATE TABLE TIMEZONE (LastModified TEXT, RecordNumber INTEGER PRIMARY KEY, TZID TEXT, TZURL TEXT, UID NUMERIC);"
                                        "CREATE TABLE PARAM (RecNumProp NUMERIC, ParmID NUMERIC, ParmVal WTEXT, PropID NUMERIC, RecordNumber INTEGER PRIMARY KEY, UID NUMERIC);"
                                        "CREATE TABLE PROP (PropVal TEXT, PropID NUMERIC, UID NUMERIC, RecordNumber INTEGER PRIMARY KEY);"
                                        "CREATE TABLE ALARMPARAM (RecNumProp NUMERIC, AlarmOID NUMERIC, ParmID NUMERIC, ParmVal WTEXT, PropID NUMERIC, RecNumAlarmTable NUMERIC, RecordNumber INTEGERPRIMARY KEY);"
                                        "CREATE TABLE ALARMPROP (AlarmOID NUMERIC, PropID NUMERIC, PropVal TEXT,RecNumAlarmTable NUMERIC, RecordNumber INTEGER PRIMARY KEY);",NULL,NULL,NULL);
         BREAKIF(nResult);

      }
      nResult = ISQL_Prepare(pMe->pISQL,pLockStr,-1,&pStmt,NULL);
      BREAKIF(nResult);
      if(SQL_ROW != ISQLSTMT_Step(pStmt)){
         RELEASEIF(pStmt);
         nResult = ISQL_Prepare(pMe->pISQL,pInsertLock,-1,&pStmt,NULL);
         BREAKIF(nResult);
         if(SQL_DONE != ISQLSTMT_Step(pStmt)){
            nResult = AEE_EFAILED;
         }         
      }else{
         nResult = AEE_SUCCESS;
      }
            
   } while( 0);
   RELEASEIF(pStmt);
   RELEASEIF( pSQLMgr );   
   return nResult;
}
/*=============================================================================
Function: 
   IvCalStore_GetNumberOfDigits

Description: 
   Gets number of digits in a int value

Prototype:
   int IvCalStore_GetNumberOfDigits(int nValue);

Parameters:
   nValue       : integer value.
               
Return Value:
   Number of digits in the int value.
 
=============================================================================*/
int IvCalStore_GetNumberOfDigits(int nValue)
{
   int nCount = 1;
   while(nValue/10){
      nValue = nValue/10;
      nCount++;
   }
   return nCount;
}
/*=============================================================================
Function: 
   IvCalStore_IntToString()

Description: 
   Converts integer to string

Prototype:
   int IvCalStore_IntToString(int32 nValue);

Parameters:
   nValue       : integer value.
                 
ReTurn Value:
   String representation of integer
 
=============================================================================*/
char *IvCalStore_IntToString(int32 nValue)
{
   char *pszStr = NULL;
   uint32 nCount = 0;

   nCount = (uint32)IvCalStore_GetNumberOfDigits(nValue);   
   pszStr = (char*) MALLOC(++nCount);
   
   if(NULL == pszStr){
      return NULL;
   }

   SNPRINTF(pszStr, nCount, "%d", nValue);

   return pszStr;
}
/*=============================================================================
Function: 
   IvCalStore_IntToAECHARString

Description: 
   Converts integer to AECHAR string

Prototype:
   static AECHAR *IvCalStore_IntToAECHARString(int32 nValue);

Parameters:
   nValue       : integer value.
                 
ReTurn Value:
   String representation
 
=============================================================================*/
AECHAR *IvCalStore_IntToAECHARString(int32 nValue)
{
   char *pszStr = NULL;
   AECHAR *pwszStr = NULL;

   pszStr = IvCalStore_IntToString(nValue);
   if(NULL == pszStr){
      return NULL;
   }

   pwszStr = (AECHAR*)MALLOC((STRLEN(pszStr)+1)*sizeof(AECHAR));
   if(NULL == pwszStr){
      FREEIF(pszStr);
      return NULL;      
   }

   pwszStr = STRTOWSTR(pszStr, pwszStr,(int)(STRLEN(pszStr)+1)* (int)((sizeof(AECHAR))));

   FREE_VAR(pszStr);

   return pwszStr;
}
/*=============================================================================
Function: 
   IvCalStore_StrnDup()

Description: 
   This function is similar to standard C strndup. Copies at
   most n characters.

Parameters:
   str : The string to copied.
   n   : Number of bytes to be duped.

Return Value: 
   Pointer to the newly allocated string, if succeeds else
   NULL.

Comments:
   None

Side effects: 
   The pointer returned must be FREE'd at later stage.

See Also:

=============================================================================*/
char *IvCalStore_StrnDup(const char *str, uint32 n)
{
   char *p = NULL;

   if(NULL == str){
      return NULL;
   }

   p=(char *)MALLOC(n+1);
   if(NULL == p){
      return NULL;
   }

   p[n]=0; 
   return MEMCPY(p,str,n);


}
/*=============================================================================
Function: 
   IvCalStore_WStrnDup()

Description: 
   This function is similar to standard C strndup. Copies at
   most n AECHAR characters.

Parameters:
   str : The string to copied.
   n   : Number of bytes to be duped.

Return Value: 
   Pointer to the newly allocated string, if succeeds else
   NULL.

Comments:
   None

Side Effects: 
   The pointer returned must be FREE'd at later stage.

See Also:

=============================================================================*/
AECHAR *IvCalStore_WStrnDup(const AECHAR *str, uint32 n)
{
   AECHAR *p = NULL;

   if(NULL == str){
      return NULL;
   }

   p=(AECHAR *)MALLOC((n+1)*sizeof(AECHAR));
   if(NULL == p){
      return NULL;
   }

   p[n]=0; 
   return MEMCPY(p,str,n*sizeof(AECHAR));


}
/*=============================================================================
Function: 
   IvCalStore_AppendStr()

Description:
   This would append the two strings by allocating a new chunk of memory with
   the provided delimiter.

Parameters:
   pszStr1 [in]: The first string
   pszStr2 [in]: The second string
   d       [in]: The Delimiter with which the strings have to be appended.

Return Value:
   Returns the appended string if succeeds by allocating a new chunk of 
   memory else NULL.

Comments: 
   This returns a newly allocated chunk of memory. Which must
   be freed at later point.

See Also:

=============================================================================*/
char *IvCalStore_AppendStr(const char *pszStr1,const char *pszStr2, int d)
{
   unsigned int nLenStr1=0, nLenStr2=0;
   char *pStr= NULL,*p = NULL;
   uint32 nLen=0;


   if (NULL == pszStr1 || NULL == pszStr2 || AEEV_SEPARATOR_LIMIT < d) {
      return NULL;
   }

   nLenStr1 = STRLEN(pszStr1);
   nLenStr2=  STRLEN(pszStr2);

   nLen = (nLenStr1+nLenStr2+2);
   /*  1 for Delimiter and the other for NULL char */

   pStr= (char *)MALLOC((dword)nLen);

   if (NULL == pStr) {
      return NULL;
   }

   MEMSET(pStr,0,nLen);
   pStr[nLen-1]=0;

   p =pStr;
   MEMCPY(pStr,pszStr1,nLenStr1);
   pStr+= (nLenStr1);
   // first take the current location and copy 'd' to it and then 
   //increment the ptr.
   *pStr++=(char)d; 

   MEMCPY(pStr,pszStr2,nLenStr2);

   return p;
}
/*=============================================================================
Function: 
   IvCalStore_AppendWStr()

Description:
   This would append the two AECHAR strings by allocating a new chunk of memory 
   with the provided delimiter.

Parameters:
   pszStr1 [in]: The first string
   pszStr2 [in]: The second string
   d       [in]: The Delimiter with which the strings have to be appended.

Return Value:
   Returns the appended string if succeeds by allocating a new chunk of 
   memory else NULL.

Comments: 
   This returns a newly allocated chunk of memory. Which must
   be freed at later point.

See Also:

=============================================================================*/
AECHAR *IvCalStore_AppendWStr( AECHAR *pszStr1, AECHAR *pszStr2, int d)
{
   int nLenStr1=0, nLenStr2=0;
   AECHAR *pStr= NULL,*p = NULL;
   uint32 nLen=0;

   if ( NULL == pszStr1 || NULL == pszStr2 || 256 < d ) {
      return NULL;
   }

   nLenStr1 = WSTRLEN(pszStr1);
   nLenStr2 = WSTRLEN(pszStr2);
   
   nLen = (uint32) (nLenStr1+nLenStr2+2);
   /*  1 for Delimiter and the other for NULL char */

   pStr= (AECHAR *)MALLOC((dword)nLen * sizeof(AECHAR));

   if (NULL == pStr) {
      return NULL;
   }

   MEMSET((char*)pStr,0,nLen * sizeof(AECHAR));
   pStr[nLen-1]=0;

   p =pStr;

   MEMCPY((char*)pStr,
          (char*)pszStr1,
          ((size_t)(nLenStr1 * ((int) (sizeof(AECHAR)))))  );

   pStr+= (nLenStr1);
   
   // first take the current location and copy 'd' to it and then 
   //increment the ptr.
   *pStr++=(AECHAR)d; 

   MEMCPY((char*)pStr,
          (char*)pszStr2,
          nLenStr2 * sizeof(AECHAR));

   return p;
}

/*===========================================================================
Function:  
   IvCalStore_CheckForValueW()

Description:
		 This function checks whether the string contains only numbers or not.

Prototype:
	  boolean IvCalStore_CheckForValueW(char *pszParmValues);

Parameters:
   pszParmValues        [in]: String to be checked for numbers    
   
Return Value:
   TRUE if the string contains only numbers otherwise FALSE.

Comments: 
   None

Side Effects: 
   None

==============================================================================*/
boolean IvCalStore_CheckForValueW(AECHAR *pszParmValues)
{
   AECHAR *pszValues = pszParmValues;
   int  nLen = 0, nCount = 0;
   AECHAR wszDigits[] = {'0','9'};
   boolean bIsValue = TRUE;
   if(NULL == pszParmValues){
      return 0;
   }
   nLen = WSTRLEN(pszParmValues);

   for(nCount = 0; nCount < nLen ; nCount++){
      if((wszDigits[0] > pszValues[nCount]) || (wszDigits[1] < pszValues[nCount])){
         bIsValue = 0;
         break;
      }      
   }
   return bIsValue;

}
/*=============================================================================
Function IvCalStore_GetAlarmComponentFromProp

Description:
   Converts the vCal 1.0 specific Alarm property into iCal 2.0 alarm component.
   
Prototype:
   int IvCalStore_GetAlarmComponentFromProp(IvCalStore      *pMe,
                                            IvProperty      *pIvProperty,
                                            IvCalObject     **ppIvCalObject);
   
Parameters:
   pMe               [in] : Pointer to IvCalStore interface
   pIvProperty       [in] : Pointer to the property object.
   ppIvCalObject [in/out] : Holds the Pointer to alarm component.

Return Value:
   AEE_SUCCESS     : If iCal 2.0 specific alarm component is generated.
   AEE_EBADPARM    : If pIvProperty or pIvCalObject  is NULL.
   AEE_EFAILED     : Otherwise.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

=============================================================================*/
int IvCalStore_GetAlarmComponentFromProp(IvCalStore      *pMe,
                                         IvProperty      *pIvProperty,
                                         IvCalObject     **ppIvCalObject)
{
   int nResult = AEE_SUCCESS;
   void *pProperty = NULL;
   void *pValue = NULL;
   IvCalObject *pIvCalObject = NULL;
   char pszAudio[] = "AUDIO";
   char pszEmail[] = "EMAIL";
   char pszProcedure[] = "PROCEDURE";
   char pszDisplay[] = "DISPLAY";
   int  nPropSize = 0;
   int  nPropSizeReq = 0;
   AEEVProperty nPropId = 0;
   AEEVAlarm *pAlarm = NULL;
   

   if(NULL == pMe || NULL == ppIvCalObject ){
      return AEE_EBADPARM;
   }

   do{

      /* Parse the vCal 1.0 ALARM property Value */
      nResult = IvProperty_ParseValue(pIvProperty, 
                                      TRUE,
                                      NULL, 
                                      0, 
                                      &nPropSizeReq);
      BREAKIF(nResult);

      if(nPropSizeReq <= 0){
         nResult = AEE_EBADPARM;
         break;
      }

      nPropSize = nPropSizeReq;
      pProperty = MALLOC(nPropSize);
      if(NULL == pProperty){
         nResult = AEE_ENOMEMORY;
         break;
      }

      nResult = IvProperty_ParseValue(pIvProperty, 
                                      TRUE, 
                                      pProperty,
                                      nPropSize,
                                      &nPropSizeReq);
      BREAKIF(nResult);
      
      pAlarm = (AEEVAlarm *)pProperty;

      nResult = IvProperty_GetId(pIvProperty, &nPropId);
      BREAKIF(nResult);

      switch(nPropId){
         case AEEVPROPERTY_AALARM:
            pValue = (void *)pszAudio;
            break;
            
         case AEEVPROPERTY_DALARM:
            pValue = (void *)pszDisplay;
            break;

         case AEEVPROPERTY_MALARM:
            pValue = (void *)pszEmail;
            break;

         case AEEVPROPERTY_PALARM:
            pValue = (void *)pszProcedure;
            break;

         default:
            nResult = AEE_EBADPARM;
            break;
      }
      BREAKIF(nResult);


      /* Create Alarm Component */
      nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                      AEECLSID_vAlarm, 
                                      (void **)&pIvCalObject);
      BREAKIF(nResult);

      nResult = IvCalObject_SetVersion(pIvCalObject, VCAL_VER_20);
      BREAKIF(nResult);

      /* Init with Action property */
      nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                              (IvProperty **)&pIvProperty,
                                              AEEVPROPERTY_ACTION,
                                              pValue);
      BREAKIF(nResult);

      nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
      RELEASEIF(pIvProperty);
      BREAKIF(nResult);

      pValue = NULL;

      /*Init with Trggier property */
      if(NULL != pAlarm->pszTrigger){
         pValue = (void *)pAlarm->pszTrigger;

         nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                 (IvProperty **)&pIvProperty,
                                                 AEEVPROPERTY_TRIGGER,
                                                 pValue);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         pIvProperty = NULL;
         pValue = NULL;
      }

        
      /* Init with Duaration Property */
      if(NULL != pAlarm->pszDuration){
         pValue = (void *)pAlarm->pszDuration;

         nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                 (IvProperty **)&pIvProperty,
                                                 AEEVPROPERTY_DURATION,
                                                 pValue);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         pIvProperty = NULL;
         pValue = NULL;
      }

      if(pAlarm->nRpt_Cnt){
         pValue = (void *)&pAlarm->nRpt_Cnt;
         nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                 (IvProperty **)&pIvProperty,
                                                 AEEVPROPERTY_REPEAT,
                                                 pValue);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         pIvProperty = NULL;
         pValue = NULL;
      }

      if(NULL != pAlarm->pszStr1){
         pValue = (void *)pAlarm->pszStr1;

         switch(nPropId){
            case AEEVPROPERTY_AALARM:
               nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                       (IvProperty **)&pIvProperty,
                                                       AEEVPROPERTY_ATTACH,
                                                       pValue);
               break;

            case AEEVPROPERTY_DALARM:
               nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                       (IvProperty **)&pIvProperty,
                                                       AEEVPROPERTY_DESCRIPTION,
                                                       pValue);
               break;

            
            case AEEVPROPERTY_PALARM:
               nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                       (IvProperty **)&pIvProperty,
                                                       AEEVPROPERTY_ATTACH, pValue);
               break;

            case AEEVPROPERTY_MALARM:
               nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                       (IvProperty **)&pIvProperty,
                                                       AEEVPROPERTY_ATTENDEE,
                                                       pValue);
               break;
         }

         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         pIvProperty = NULL;
         pValue = NULL;
      }

      if(NULL != pAlarm->pszStr2){
         if(nPropId != AEEVPROPERTY_MALARM){
            break;
         }

         pValue = (void *)pAlarm->pszStr2;
         nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                 (IvProperty **)&pIvProperty,
                                                 AEEVPROPERTY_SUMMARY,
                                                 pValue);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);
         pIvProperty = NULL;

         nResult = AEEV_InitPropertyGivenIdValue(pMe->pIShell,
                                                 (IvProperty **)&pIvProperty,
                                                 AEEVPROPERTY_DESCRIPTION,
                                                 pValue);
         BREAKIF(nResult);

         nResult = IvCalObject_AddProperty(pIvCalObject, pIvProperty);
         RELEASEIF(pIvProperty);
         BREAKIF(nResult);

         pValue = NULL;
      }

   }while(0);

   if(AEE_SUCCESS != nResult){
      RELEASEIF(pIvCalObject);      
   }
   FREEIF(pProperty);
   RELEASEIF(pIvProperty);
   *ppIvCalObject = pIvCalObject;

   return nResult;

}

/*=============================================================================
Function: AEEV_InitPropertyGivenIdValue

Description:
   Init the property given property id and value.
   
Prototype:
   AEEV_InitPropertyGivenIdValue(IShell         *ps
                                 IvProperty     **ppIvProperty,
                                 AEEVProperty   nPropId,
                                 void           *pValue);
   
Parameters:
   ps              [in]    : IShell pointer.
   ppIvProperty    [in/out]: Holds a Pointer to the returned IvProperty object.
   nPropId         [in]    : Property Id.
   pValue          [in]    : Property Value.

Return Value:
   AEE_SUCCESS     : If successful in creating property and init 
                     the property with given Id & value.
   AEE_EBADPARM    : If ppIvProperty or pValue are NULL.
                     If nPropdId is invalid.
   AEE_EFAILED     : Otherwise.

Comments:
   nPropdId should be one of AEEVPROPERTY_ACTION, AEEVPROPERTY_TRIGGER, 
   AEEVPROERTY_DURATION,
   AEEVPROPERTY_

Side Effects:
   None.

See Also:
   None.

=============================================================================*/
int AEEV_InitPropertyGivenIdValue(IShell             *ps,
                                  IvProperty         **ppIvProperty,
                                  AEEVProperty       nPropId,
                                  void               *pValue)
{
   int nResult = AEE_EFAILED;
   IvProperty *pIvProperty = NULL;
   *ppIvProperty = NULL;

   do{
      nResult = ISHELL_CreateInstance(ps, AEECLSID_vProperty, (void **)&pIvProperty);
      BREAKIF(nResult);

      switch(nPropId){

         case AEEVPROPERTY_ATTACH:
         case AEEVPROPERTY_ACTION:
         case AEEVPROPERTY_DURATION:
         case AEEVPROPERTY_TRIGGER:
         case AEEVPROPERTY_DESCRIPTION:
         case AEEVPROPERTY_ATTENDEE:
         case AEEVPROPERTY_SUMMARY:{

            nResult = IvProperty_InitProperty(pIvProperty,
                                              nPropId,
                                              NULL,
                                              FALSE,
                                              pValue,
                                              STRLEN((char *)pValue),
                                              NULL );
            break;
            }

         case AEEVPROPERTY_REPEAT:{

            nResult = IvProperty_InitProperty(pIvProperty,
                                              nPropId,
                                              NULL,
                                              TRUE,
                                              pValue,
                                              sizeof(uint32),
                                              NULL );
            break;
                                  }

         default:
            {
               nResult = AEE_EFAILED;
               break;   
            }
      }

   }while(0);

   if(nResult == AEE_SUCCESS){
      *ppIvProperty = pIvProperty;
   }
      
   return nResult;
}

/*=============================================================================
Function: 
   IvCalStore_ValidateMailbox

Description:
   Validate the calendar database.
   
Prototype:
   static int IvCalStore_ValidateMailbox( IvCalStore *pMe );
   
Parameters:
   pMe              [in]    : Pointer to IvCalStore.
   pszFromFileName  [in/out]: From file name.
   pszToFileName    [in]    : To file name.   

Return Value:
   AEE_SUCCESS     : If successful in copying the file
   AEE_EBADPARM    : If parameters are not proper
   AEE_EFAILED     : Otherwise.

Comments:
   None

Side Effects:
   None.

See Also:
   None.

=============================================================================*/
static int IvCalStore_ValidateMailbox( IvCalStore *pMe, boolean *pbPresent )
{

   int nResult = AEE_EFAILED;
   IFileMgr *pFileMgr = NULL;      
   
   do {

      //Lets get some info on the file.
      nResult = ISHELL_CreateInstance(pMe->pIShell, 
                                       AEECLSID_FILEMGR, 
                                       (void**)&pFileMgr);

      if ( AEE_SUCCESS != nResult || NULL == pFileMgr) {
         nResult = AEE_EFAILED;
         break;
      }
     
      if( AEE_SUCCESS != IFILEMGR_Test( pFileMgr, CALENDAR)) {      
         // The mailbox file does not exist, 
         // attempt to create the database on the fly                  
         *pbPresent = FALSE;         
      } else {         
         *pbPresent = TRUE;
      }
      nResult = AEE_SUCCESS;


   }while( 0 );
   
   RELEASEIF( pFileMgr);

   return nResult;
}

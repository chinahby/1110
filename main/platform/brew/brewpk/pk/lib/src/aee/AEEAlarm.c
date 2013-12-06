/*============================================================================

FILE: AEEAlarm.c

SERVICES: The IAlarm object manages alarms for other applets.

DESCRIPTION:
   This object provides the same functionality as ISHELL_SetAlarm() and
   ISHELL_CancelAlarm().  It also allows all the active alarms to be 
   suspended and then later resumed, specifically for when the phone is in LPM.

   Hopefully eventually the native ISHELL alarms will support this feature
   and this entire object can be removed.

   THIS OBJECT ASSUMES THAT THE PHONE HAS VALID TIME.  DO NOT INVOKE ANY
   METHODS IN THIS OBJECT IF THIS IS NOT THE CASE.  IT IS OK, HOWEVER, TO 
   CREATE THE OBJECT WHILE THE PHONE DOES NOT YET HAVE VALID TIME.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IAlarm

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
============================================================================*/

/*============================================================================

  $Header: //tsg/UI/main/AEE/src/AEEAlarm.c#5 $
$DateTime: 2002/08/15 15:08:09 $
  $Author: mvines $
  $Change: 54873 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
03/31/2004   huxin   for more accurately, changed the SetAlarm parameter from min to seconds.
08/14/2002   mjv     IAlarm is now simply a BREW class, instead of 
                     an applet.
07/19/2002   mjv     Use AEE_SetAppContext() so IIDLE_SetTimer() is no longer
                     required.
06/13/2002   mjv     Include customer.h instead of OEMFeatures.h 
05/09/2002   mjv     Ported to BREW 1.2
04/25/2002   mjv     Use ERR() macro instead of MSG_ERROR(). 
                     Modified alarm saving code to avoid using 
                     IFILEMGR_Remove() because it seems to cause some weird
                     behaviour occasionally (the remove fails with a
                     'operation not supported' error!)
02/28/2002   mjv     Add a reference to the AEEAlarm module when creating
                     an AEEAlarm object.
02/22/2002   mjv     Created AEEAlarm module
02/14/2002   mjv     Use IIDLE_SetTimer() instead of ISHELL_SetTimer() 
01/24/2002   mjv     Renamed file to AEEAlarm.c, use AEEVector.h instead 
                     of IVector.h 
12/12/2001   mjv     Initial revision

============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEFile.h"

#include "OEMClassIds.h"

#include "AEEAlarm.h"
#include "AEEVector.h"
#ifndef WIN32
#include "ERR.h"
#include "db.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "OEMHeap.h"
#include "AEEAnnunciator.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


////
// EFS file that is used to store the alarm info
//#define ALARM_EFS_FILE  "alarms.dat"


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef PACKED struct _AlarmInfo {
   AEECLSID cls;
   uint16   nUserCode;
   uint32   nExpireMin; // The minute when the timer will expire.  
} AlarmInfo;


typedef struct _CAlarm
{
   DECLARE_VTBL(IAlarm)

   uint32    m_nRefs;
   IShell   *m_pShell;
   IVector  *m_alarms;  // vector of AlarmInfo pointers
   boolean   m_alarmsActive;
} CAlarm;


/*===========================================================================

                    GLOBAL DATA

===========================================================================*/


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

//--
// IAlarm Interface methods 
//--
static uint32     CAlarm_AddRef(IAlarm *p);
static uint32     CAlarm_Release(IAlarm *p);
static int        CAlarm_SetAlarm(IAlarm *p, 
                                       AEECLSID     cls,
                                       uint16       nUserCode,
                                       uint32       nMins);
static int        CAlarm_CancelAlarm(IAlarm *p, 
                                          AEECLSID     cls,
                                          uint16       nUserCode);
static void       CAlarm_SuspendAlarms(IAlarm *p);
static void       CAlarm_ResumeAlarms(IAlarm *p);


//--
// Static methods 
//--
static void       CAlarm_FreeAlarmDataVector(CAlarm *pMe);
static void       CAlarm_LoadAlarmData(CAlarm *pMe);
static void       CAlarm_SaveAlarmData(CAlarm *pMe);
static AlarmInfo *CAlarm_FindAlarm(CAlarm *pMe,
                                        AEECLSID cls,
                                        uint16   nUserCode,
                                        uint32  *idx);
static void       CAlarm_ScheduleAlarms(CAlarm *pMe,uint16 nUserCode);
static void       CAlarm_TimerCB(void *pUser);
static void       CAlarm_RemAllAlarm(IAlarm *p);


/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

static const VTBL(IAlarm) gAlarmMethods =
{
   CAlarm_AddRef,
   CAlarm_Release,
   CAlarm_SetAlarm,
   CAlarm_CancelAlarm,
   CAlarm_SuspendAlarms,
   CAlarm_ResumeAlarms,
   CAlarm_RemAllAlarm
};

static CAlarm gAlarm;   


/*=============================================================================
FUNCTION: AEEAlarm_New

DESCRIPTION: Creates a new IAlarm interface

PARAMETERS:
	*ps [in]: IShell pointer
   cls [in]: Class to create
*ppif [out]: IAlarm interface double pointer

RETURN VALUE:
int: SUCCESS if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEAlarm_New(IShell *ps, AEECLSID cls, void **ppif)
{
   *ppif = NULL;

   if (cls != AEECLSID_UIALARM) {
      return ECLASSNOTSUPPORT;
   }

   if (0 == gAlarm.m_nRefs) {
      MEMSET(&gAlarm,
             0,
             sizeof(gAlarm));

      INIT_VTBL(&gAlarm,
                IAlarm,
                gAlarmMethods);

      if (SUCCESS != ISHELL_CreateInstance(ps,
                                           AEECLSID_VECTOR,
                                           (void**)&gAlarm.m_alarms)) {
         return EFAILED;
      }
      
      gAlarm.m_pShell = ps;
      ISHELL_AddRef(ps);
      
      CAlarm_LoadAlarmData(&gAlarm);

      // NOTE:
      //   We are adding an extra reference so once the class has been
      //   loaded it will never be unloaded.  
      //
      gAlarm.m_nRefs = 1;
   }
   else
   {
      ++gAlarm.m_nRefs;
   }

   *ppif = (void *) &gAlarm;
   return AEE_SUCCESS;
}


/*=============================================================================
FUNCTION: CAlarm_AddRef

DESCRIPTION: Add a reference to this object

PARAMETERS:
   *p: IAlarm interface pointer

RETURN VALUE:
uint32: the new reference count

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  CAlarm_AddRef(IAlarm *p)
{
   register CAlarm *pMe = (CAlarm *)p;

   return ++pMe->m_nRefs;
}


/*=============================================================================
FUNCTION: CAlarm_Release

DESCRIPTION: Remove a reference from this object

PARAMETERS:
   *p: IAlarm interface pointer

RETURN VALUE:
uint32: the new reference count

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32  CAlarm_Release(IAlarm *p)
{
   register CAlarm *pMe = (CAlarm *)p;

   if (0 == pMe->m_nRefs) {
      return 0;
   } 

   if(--pMe->m_nRefs) {
      return pMe->m_nRefs;
   }

   if (pMe->m_alarmsActive) {
      CAlarm_SuspendAlarms(p);
   }
   
   CAlarm_FreeAlarmDataVector(pMe);
   IVector_Release(pMe->m_alarms);
   ISHELL_Release(pMe->m_pShell);
   return 0;
}

/*=============================================================================
FUNCTION:  CAlarm_SetAlarm
 
DESCRIPTION: Allows the caller to set a long-term alarm.
 
PARAMETERS:
   *p: IAlarm interface pointer
   cls: Applet class to call when the alarm expires
   nUserCode: 16-bit code passed to the applet
   nMins: Number of minutes to set the alarm from the current time
 
RETURN VALUE:
   SUCCESS  if operation is successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CAlarm_SetAlarm(IAlarm *p, 
                                AEECLSID     cls,
                                uint16       nUserCode,
                                uint32       nMins)
{
   CAlarm *pMe = (CAlarm*)p;
   uint32       idx;
   AlarmInfo   *pai;
   DBGPRINTF( "CAlarm_SetAlarm  cls=0x%x nUserCode = %d--nMins=%d",cls,nUserCode,nMins);
   MSG_FATAL("pMe->m_alarmsActive=====%d",pMe->m_alarmsActive,0,0);
   if (FALSE == pMe->m_alarmsActive) {
      return EFAILED;
   }

   pai = CAlarm_FindAlarm(pMe, cls, nUserCode, &idx);
   if (NULL == pai) {
      pai = OEM_Malloc(sizeof(*pai));
      if (NULL == pai) {
         return ENOMEMORY;
      }

      idx = IVector_Size(pMe->m_alarms);

      if (IVector_AddElement(pMe->m_alarms, (void*)pai) != SUCCESS) {
         OEM_Free((void*)pai);
         return ENOMEMORY;
      }
   } 

   pai->cls = cls;
   pai->nUserCode = nUserCode;
   //for more accurately, less than 1 seconds.
   {
      uint32 i;
   
      for (i = 0; i < IVector_Size(pMe->m_alarms); i++) 
      {
         AlarmInfo *oldpai = IVector_ElementAt(pMe->m_alarms, i);
         if(oldpai->nExpireMin == (nMins + GET_SECONDS()))
        {
            if((oldpai->cls == AEECLSID_ALARMCLOCK)&& (cls == AEECLSID_SCHEDULEAPP))
            {
                nMins = nMins + 60;
                break;                
            }

            if((oldpai->cls == AEECLSID_SCHEDULEAPP)&& (cls == AEECLSID_ALARMCLOCK))
            {
                nMins = nMins - 60;                
                break;
            }
        }
      }
   }  
   
   pai->nExpireMin = nMins +
                     GET_SECONDS();
   MSG_FATAL("pai->nExpireMin==%d,nMins==%d,GET_SECONDS()===%d",pai->nExpireMin,nMins,GET_SECONDS());
//                     GET_SECONDS() / 60;

   CAlarm_SaveAlarmData(pMe);

   CAlarm_ScheduleAlarms(pMe,nUserCode);

   return SUCCESS;
}



/*=============================================================================
FUNCTION:  CAlarm_CancelAlarm
 
DESCRIPTION: Cancels an alarm previously set via IAlarm_SetAlarm()
 
PARAMETERS:
   *p: IAlarm interface pointer
   cls: Applet class of alarm to cancel
   nUserCode: 16-bit user code of alarm to cancel
 
RETURN VALUE:
   SUCCESS  if alarm is found and canceled
   EFAILED  if alarm can not be found or canceled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int CAlarm_CancelAlarm(IAlarm *p, 
                                   AEECLSID     cls,
                                   uint16       nUserCode)
{
   CAlarm *pMe = (CAlarm*)p;
   uint32       idx;
   AlarmInfo   *pai;

   pai = CAlarm_FindAlarm(pMe, cls, nUserCode, &idx);
   if (NULL == pai) {
      // Alarm doesn't exist
      return EFAILED;
   }

   if (pMe->m_alarmsActive) {
      void *pOldContext = AEE_EnterAppContext(NULL);

      (void) ISHELL_CancelTimer(pMe->m_pShell,
                                CAlarm_TimerCB,
                                NULL);

      AEE_LeaveAppContext(pOldContext);
   }

   (void) IVector_RemoveElementAt(pMe->m_alarms, idx);

   OEM_Free((void*)pai);

   CAlarm_SaveAlarmData(pMe);

   CAlarm_ScheduleAlarms(pMe,nUserCode);

   return SUCCESS;
}


/*=============================================================================
FUNCTION:  CAlarm_SuspendAlarms
 
DESCRIPTION: Suspends all active alarms
 
PARAMETERS:
   *p: IAlarm interface pointer 
 
RETURN VALUE:
   None

COMMENTS: 
   This method may be called when are no active alarms.

SIDE EFFECTS:

SEE ALSO:
   CAlarm_ResumeAlarms

=============================================================================*/
static void CAlarm_SuspendAlarms(IAlarm *p)
{
   CAlarm *pMe = (CAlarm*)p;

   pMe->m_alarmsActive = FALSE;

   CAlarm_ScheduleAlarms(pMe,0);
}


/*=============================================================================
FUNCTION:  CAlarm_ResumeAlarms
 
DESCRIPTION:  Resumes the alarms previously suspended by 
              CAlarm_SuspendAlarms()
 
PARAMETERS:
   *p: IRefIAlarm interface pointer
 
RETURN VALUE:
   None

COMMENTS:
   This method may be called when are no alarms to resume. 

SIDE EFFECTS:

SEE ALSO:
   CAlarm_SuspendAlarms

=============================================================================*/
static void CAlarm_ResumeAlarms(IAlarm *p)
{
   CAlarm *pMe = (CAlarm*)p;

   pMe->m_alarmsActive = TRUE;

   CAlarm_ScheduleAlarms(pMe,0);
}


/*=============================================================================
FUNCTION:  CAlarm_FreeAlarmDataVector
 
DESCRIPTION: Releases all elements of the CAlarm::m_alarms vector
 
PARAMETERS:
   *pMe: CIdle object pointer
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CAlarm_FreeAlarmDataVector(CAlarm *pMe)
{
   uint32 i;
   
   for (i = 0; i < IVector_Size(pMe->m_alarms); i++) {
      AlarmInfo *ai = IVector_ElementAt(pMe->m_alarms, i);

      OEM_Free((void*)ai);
   }
   IVector_RemoveAllElements(pMe->m_alarms);
}


/*=============================================================================
FUNCTION:  CAlarm_LoadAlarmData
 
DESCRIPTION: Loads alarm data from EFS
 
PARAMETERS:
   *pMe: CAlarm object pointer
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CAlarm_LoadAlarmData(CAlarm *pMe)
{
   IFile     *f;
    AlarmInfo  ai,
             *pai;
    int32      bytesRead;
    IFileMgr *pFileMgr;
    if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_FILEMGR, (void**)&pFileMgr)) 
    {
         return;
    }
    
    CAlarm_FreeAlarmDataVector(pMe);

    f = IFILEMGR_OpenFile(pFileMgr, ALARM_EFS_FILE, _OFM_READ);

    if (f != NULL)
    {

        DBGPRINTF( ";----------------------------------------");
        DBGPRINTF( ";CAlarm_LoadAlarmData");
        for (;;)
        {
            bytesRead = IFILE_Read(f, (void*)&ai, sizeof(ai));

            if (bytesRead != (int32)sizeof(ai))
            {
                break;
            }

            pai = OEM_Malloc(sizeof(*pai));
            if (NULL == pai)
            {
                ERR("Out of memory", 0, 0, 0);
                break;
            }

            *pai = ai;

            DBGPRINTF( ";********cls=0x%x, user=%d, expire=%d", ai.cls, ai.nUserCode, ai.nExpireMin);
            if (IVector_AddElement(pMe->m_alarms, (void*)pai) != SUCCESS)
            {
                OEM_Free((void*)pai);
                ERR("Out of memory", 0, 0, 0);
                break;
            }
        }

      IFILE_Release(f);
   }
   IFILEMGR_Release(pFileMgr);
}


/*=============================================================================
FUNCTION:  CAlarm_SaveAlarmData
 
DESCRIPTION: Saves alarm data to EFS
 
PARAMETERS:
   *pMe: CAlarm object pointer
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CAlarm_SaveAlarmData(CAlarm *pMe)
{
   IFile  *f;
   uint32  i;
   uint32  bytesWritten;
   IFileMgr *pFileMgr;
   if (SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_FILEMGR, (void**)&pFileMgr)) 
   {
      return;
   }
   
   f = IFILEMGR_OpenFile(pFileMgr, 
                         ALARM_EFS_FILE, 
                         _OFM_READWRITE);

   if (NULL == f) {
      f = IFILEMGR_OpenFile(pFileMgr,
                            ALARM_EFS_FILE,
                            _OFM_CREATE);

      if (NULL == f) {
          DBGPRINTF( ";************************");
          DBGPRINTF("Unable to open/create: %s" ALARM_EFS_FILE);
          DBGPRINTF( ";************************");
          IFILEMGR_Release(pFileMgr);
         return;
      }
   }
   
   (void) IFILE_Truncate(f, 0);
   (void) IFILE_Seek(f, _SEEK_START, 0);
   
   for (i = 0; i < IVector_Size(pMe->m_alarms); i++) {
      AlarmInfo *pai = IVector_ElementAt(pMe->m_alarms, i);
      

      bytesWritten = IFILE_Write(f, pai, sizeof(*pai));

      if (bytesWritten != sizeof(*pai)) {
         ERR("Error writing to file: " ALARM_EFS_FILE, 0, 0, 0);
         break;
      }
   }

   IFILE_Release(f);
   IFILEMGR_Release(pFileMgr);
}


/*=============================================================================
FUNCTION:  CAlarm_FindAlarm
 
DESCRIPTION: Searchs the current alarms for an alarm with the specified 
             class id and user code
 
PARAMETERS:
   pMe: CAlarm object pointer
   cls: Class id of the alarm 
   nUserCode: User code of the alarm
   *idx: returns the index of the alarm in CAlarm::m_alarms
 
RETURN VALUE:
AlarmInfo: NULL if the alarm was not found
           pointer to the alarm info if it was found

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static AlarmInfo *CAlarm_FindAlarm(CAlarm *pMe,
                                        AEECLSID cls,
                                        uint16   nUserCode,
                                        uint32  *idx)
{
   uint32 i;

   for (i = 0; i < IVector_Size(pMe->m_alarms); i++) {
      AlarmInfo *pai = IVector_ElementAt(pMe->m_alarms, i);

      if ( (pai->cls == cls) && (pai->nUserCode == nUserCode) ) {
         *idx = i;
         return pai;
      }
   }
   return NULL;
}



/*=============================================================================
FUNCTION:  CAlarm_ScheduleAlarms
 
DESCRIPTION: Determines if any alarms have expired and if so notifies the
             appropriate applet.  Then the next alarm timer is scheduled.
 
PARAMETERS:
   *pMe: CAlarm object pointer
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CAlarm_ScheduleAlarms(CAlarm *pMe,uint16 nUserCode)
{
    uint32   i;
    uint32   nCurrSecs;
    uint32   nCurrMin;
    uint32   nNextTimerMS;
    uint32   nNextMin;
    boolean  bAlarmsChanged;
    void    *pOldContext;
    // liyz add for test @131205
    boolean bUserSetALARM = FALSE;
	//boolean  bAlarmSalesTracker = FALSE;
    
    pOldContext = AEE_EnterAppContext(NULL);
    (void) ISHELL_CancelTimer(pMe->m_pShell, CAlarm_TimerCB, NULL);
    AEE_LeaveAppContext(pOldContext);

    if (FALSE == pMe->m_alarmsActive) 
    {
        IAnnunciator *pIAnn;
        if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
        {
            IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF/*ANNUN_STATE_OFF*/);
            IANNUNCIATOR_Release(pIAnn);
        }
        return;
    }  

    nCurrSecs = GET_SECONDS();
    //for more accurately, less than 1 seconds.
    nCurrMin = nCurrSecs;
    //   nCurrMin = nCurrSecs / 60;

    bAlarmsChanged = FALSE;
    nNextMin = 0xFFFFFFFF;
    i = 0;
    
	bUserSetALARM = FALSE; // liyz add for test @131205
    while (i < IVector_Size(pMe->m_alarms)) 
    {
        AlarmInfo *pai = IVector_ElementAt(pMe->m_alarms, i);

        /*
		if((pai->nUserCode == 21) &&(IVector_Size(pMe->m_alarms) ==1))  
		{
			bAlarmSalesTracker = TRUE;
			nUserCode = 21;
		}       
        */
        // liyz add for test @131205
        if(pai->nUserCode <= 20)
        {
           bUserSetALARM = TRUE;
        }
        DBGPRINTF( "pai->nExpireMin = %d nCurrMin = %d---pai->nUserCode=%d",pai->nExpireMin,nCurrMin,pai->nUserCode);
        DBGPRINTF( "i = %d ---IVector_Size(pMe->m_alarms = %d",i,IVector_Size(pMe->m_alarms)); 
        if (pai->nExpireMin <= nCurrMin) 
        {
            pOldContext = AEE_EnterAppContext(NULL);
			{
            	ISHELL_PostEventEx(pMe->m_pShell, 0, pai->cls, EVT_ALARM, pai->nUserCode, 0);
			}
            DBGPRINTF( ";alarm expired class = 0x%x id = %d",pai->cls,pai->nUserCode);
            AEE_LeaveAppContext(pOldContext);

            (void) IVector_RemoveElementAt(pMe->m_alarms, i);
            OEM_Free((void*)pai);

            bAlarmsChanged = TRUE;
        } 
        else 
        {
            if (pai->nExpireMin < nNextMin)
            {
                nNextMin = pai->nExpireMin;
            }
            i++;
        }
    }

    if (0xFFFFFFFF == nNextMin) 
    {  // no alarms
        IAnnunciator *pIAnn;
        if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
        {
            IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF/*ANNUN_STATE_OFF*/);
            IANNUNCIATOR_Release(pIAnn);
        }
    }
    else 
    {
        nNextMin -= nCurrMin;
        //for more accurately, less than 1 seconds.
        //修改调度时间，将重排时间从12小时改为10分钟
        if (nNextMin > (10*60))
        {
            // Don't set the timer for longer than 12 hours.  Not sure why,
            // but the BREW Alarms do this so there must be some reason...
            nNextMin = (10*60);
        }
#if 0 // jyj TBD.
        if (nNextMin > (12*60)) {
        // Don't set the timer for longer than 12 hours.  Not sure why,
        // but the BREW Alarms do this so there must be some reason...
        nNextMin = (12*60);
        }
#endif
        //for more accurately, less than 1 seconds.
        nNextTimerMS = 1000 * (nNextMin + 1);// - (nCurrSecs % 60 ));
        //      nNextTimerMS = 1000 * ( ( 60 - (nCurrSecs % 60 ) ) + ( (nNextMin-1) * 60 ) );

        pOldContext = AEE_EnterAppContext(NULL);
        (void) ISHELL_SetTimer(pMe->m_pShell, (int32) nNextTimerMS, CAlarm_TimerCB, pMe);
        AEE_LeaveAppContext(pOldContext);
		MSG_FATAL("nUserCode============%d",nUserCode,0,0);
        
        /*
        if (nUserCode != 21) //PERMID   21     
        {
            IAnnunciator *pIAnn;
            if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
            {
                IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_ON/);
                IANNUNCIATOR_Release(pIAnn);
            }
        }       
		else if (bAlarmSalesTracker)
		{
			IAnnunciator *pIAnn;
            if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
            {
                IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF);
                IANNUNCIATOR_Release(pIAnn);
            }

            bAlarmSalesTracker = FALSE;
		}       
        */
#if 0 // liyz modify for test @131205
        if ((nUserCode == 22) || (nUserCode == 23) || (nUserCode == 24))    //SOUND_BO_ALARM /SALE_TRACKER_ALARM / ESN_TRACKER_ALARM
        {
            IAnnunciator *pIAnn;
            if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
            {
                IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF);
                IANNUNCIATOR_Release(pIAnn);
            }
        }
        else
        {
            IAnnunciator *pIAnn;
            if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
            {
                IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_ON/*ANNUN_STATE_OFF*/);
                IANNUNCIATOR_Release(pIAnn);
            }
        }
#else
     if (!bUserSetALARM)	//SOUND_BO_ALARM /SALE_TRACKER_ALARM / ESN_TRACKER_ALARM
	 {
		 IAnnunciator *pIAnn;
		 if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
		 {
			 IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_OFF);
			 IANNUNCIATOR_Release(pIAnn);
		 }
	 }
	 else
	 {
		 IAnnunciator *pIAnn;
		 if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_ANNUNCIATOR, (void**)&pIAnn)) 
		 {
			 IANNUNCIATOR_SetField(pIAnn, ANNUN_FIELD_ALARM, ANNUN_STATE_ALARM_ON/*ANNUN_STATE_OFF*/);
			 IANNUNCIATOR_Release(pIAnn);
		 }
	 }
#endif
    }

    if (bAlarmsChanged) 
    {
        CAlarm_SaveAlarmData(pMe);
    }
}

static void       CAlarm_RemAllAlarm(IAlarm *p)
{
	CAlarm *pMe = (CAlarm*)p;
	(void) IVector_RemoveAllElements(pMe->m_alarms);
}

/*=============================================================================
FUNCTION:  CAlarm_TimerCB
 
DESCRIPTION: Alarm timer callback function
 
PARAMETERS:
   *pUser: CAlarm object pointer 
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CAlarm_TimerCB(void *pUser)
{
   register CAlarm *pMe = (CAlarm *)pUser;

   CAlarm_ScheduleAlarms(pMe,0);
}

#ifndef AEEIVCALSTOREUTIL_H
#define AEEIVCALSTOREUTIL_H
/*==============================================================================

File: 
   AEEIvCalStoreUtil.h

Services:     
   Provides utility services and functionalities for IvCalStore interface

Description:  
   IvCalStore utility functionalities.

Public Classes And Static Functions:
   IvCalStore

INITIALIZATION AND SEQUENCING REQUIREMENTS:

      Copyright © 2007 QUALCOMM Incorporated.
             All Rights Reserved.
          QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "AEESQL.h"
#include "AEEStdErr.h"
#include "AEECLSID_SQLMGR.bid"
#include "AEEvEvent.bid"
#include "AEEvToDo.bid"
#include "AEEvJournal.bid"
#include "AEEModGen.h"
#include "AEEvTimeZone.bid"//This file might not be required

#include "AEEvProperty.bid"
#include "AEEvParm.bid"
#include "AEEvCalStore.bid"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "AEEvAlarm.bid"
#include "AEEvStandard.bid"
#include "AEEvDaylight.bid"

#define CALENDAR "fs:/shared/Calendar.db"
#define VCALSTORE_DEFAULT_CALENDAR "fs:/shared/default.db"

#define VCALSTORE_MAIN_TBL "MAIN"
#define VCALSTORE_TZ_TBL "TIMEZONE"
#define VCALSTORE_ALARM_TBL "ALARM"
#define VCALSTORE_RECUR_TBL "RECURRENCE"
#define VCALSTORE_PROP_TBL "PROP"
#define VCALSTORE_PARAM_TBL "PARAM"
#define VCALSTORE_DAYLIGHT_TBL "DAYLIGHT"
#define VCALSTORE_ALARMPARAM_TBL "ALARMPARAM"
#define VCALSTORE_ALARMPROP_TBL "ALARMPROP"
#define VCALSTORE_TIMEZONE_TBL "TIMEZONE"
#define VCALSTORE_TZPROP_TBL "TIMEZONEPROP"
#define VCALSTORE_TZPARAM_TBL "TIMEZONEPARAM"
#define VCALSTORE_DATETIME_TBL "DATETIMEPERIOD"
#define AEEV_SEPARATOR_LIMIT 256
#define AEEV_INVALID_NUM  0x7FFFFFFF
#define AEEV_SEMICOLON  ';'

#ifndef BREAKIF
#define BREAKIF(nRetVal) { \
  if(SUCCESS != (nRetVal)) { \
  break; } \
}
#endif

struct IvCalStore {

   const AEEVTBL(IvCalStore) * pvt;

   uint32 m_nRefs;

   IShell *pIShell; 

   IModule *pIModule;

   ISQL *pISQL;       

   ISignalBus *pIUpdateSignalBus;

   ISignalBus *pIDeleteSignalBus;

   ISignalBus *pIAddSignalBus;

   ISignalBus *pILockSignalBus;
};


#ifndef RELEASEIF
#define RELEASEIF(p)          if (p) { IBASE_Release((IBase*)(void*)(p)); (p) = NULL; }
#endif //RELEASEIF

#ifndef FREE_VAR
#define FREE_VAR(p)           {FREE(p);  (p) = NULL;}
#endif /* FREE_VAR */

#define BREAKIF_NULLPTR(p,n,e,s) {\
                                if(NULL == p) \
                                {\
                                  DBGPRINTF_FATAL(s);\
                                  n = e; \
                                  break; \
                                }\
                              }

#define AEEVPROPERTY_MAX 0x7FFFFFFF
#define AEEVCATEGORY_MAX 0x7FFFFFFF
#define AEEVOBJECT_MAX 0x7FFFFFFF
#define AEEVPARM_MAX 0x7FFFFFFF
#define AEEVSTATUS_MAX 0x7FFFFFFF

#define VCAL_VER_20  0x00000002
/* Week In Month */
#define AEEVWEEK_FIRST     0x00000001
#define AEEVWEEK_SECOND    0x00000002
#define AEEVWEEK_THIRD     0x00000003
#define AEEVWEEK_FOURTH    0x00000004
#define AEEVWEEK_FIFTH     0x00000005
#define AEEVWEEK_LAST      0x00000006
#define AEEVWEEK_LSECOND   0x00000007   // Second last
#define AEEVWEEK_LTHIRD    0x00000008   // Third last
#define AEEVWEEK_LFOURTH   0x00000009   // Fourth last
#define AEEVWEEK_LFIFTH    0x0000000a   // Fifth last

extern int IvCalStore_OpenCalendarDB( IvCalStore *pMe );
extern char *IvCalStore_IntToString(int32 nValue);
extern char *IvCalStore_AppendStr(const char *pszStr1,const char *pszStr2, int d);
extern char *IvCalStore_StrnDup(const char *str, uint32 n);
extern AECHAR *IvCalStore_WStrnDup(const AECHAR *str, uint32 n);
extern AECHAR *IvCalStore_AppendWStr( AECHAR *pszStr1, AECHAR *pszStr2, int d);
extern AECHAR *IvCalStore_IntToAECHARString(int32 nValue);

#endif // AEEIVCALSTORE_H

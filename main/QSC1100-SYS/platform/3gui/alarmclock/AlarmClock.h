#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H
/*=============================================================================

FILE: AlarmClock.h

SERVICES: QCT UI AlarmClock App

GENERAL DESCRIPTION:


(c) COPYRIGHT 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   clm     Initial revision.
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/
#include "customer.h"

/* Brew libraries */
#include "OEMFeatures.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEESysClock.h"
#include "AEEAlarmMgr.h"
#include "AEEFile.h"
#include "AEEAnnunciator.h"
#include "AEEAlert.h"

#include "AEE_OEM.h"

#include "AEEVector.h"
#include "alarmclock.brh"

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/
// Clock alarm states
typedef enum {
   ALARM_STATE_INIT = 0,
   ALARM_STATE_READY,
   ALARM_STATE_RUNNING,
   ALARM_STATE_EXPIRED,
   ALARM_STATE_SNOOZE,  //This state is defined, but it is not fully implemented
   ALARM_STATE_INVALID
} AlarmState;

// List of available repeating types for recurring alarms

#define CLOCK_ONE_TIME        ((uint8)0x0)
#define CLOCK_EVERY_MONDAY    ((uint8)0x1)
#define CLOCK_EVERY_TUESDAY   ((uint8)0x2)
#define CLOCK_EVERY_WEDNESDAY ((uint8)0x4)
#define CLOCK_EVERY_THURSDAY  ((uint8)0x8)
#define CLOCK_EVERY_FRIDAY    ((uint8)0x10)
#define CLOCK_EVERY_SATURDAY  ((uint8)0x20)
#define CLOCK_EVERY_SUNDAY    ((uint8)0x40) 
#define CLOCK_EVERY_DAY       ((uint8)0x7f)

typedef uint8 RepeatType;

// Struct to store relevent data for an alarm
typedef struct {  
   // Current state of the alarm
   AlarmState    state;      
   
   // User code used by IAlarmMgr to identify this alarm among other alarms 
   uint16        wCode;

   // Alarm expiration time 
   uint32        uExpirationTime;
   
   // Last alarm exipration time
   uint32        uLastExpirationTime;

   // Whether the alarm or snooze has expired
   boolean       bExpired;

   // Whether the alarm exipres before the phone
   // acquires valid system time
   boolean       bExpiredBeforeValidTime;

   // Whether the corresponding SHELL alarm is pending
   boolean       bAlarmActive;
  
   // Value encoding alarm recurrence information.
   RepeatType    recurrence;       

   // Associated ringer ID to play when the alarm expires 
   AECHAR       *pRingerID;            
  
   // Optional alarm description
   AECHAR       *pDescription;  
} Alarm;

typedef struct
{
  // First element of this structure must be AEEApplet.
  AEEApplet     a;

  // Whether app has been started.
  boolean       bAppIsRunning;

  // Alarm manager
  IAlarmMgr    *pAlarmMgr;

  // Vector model storing all the alarms
  IVector      *pAlarmList; 
   
  // File manager
  IFileMgr     *pFileMgr;

  // IAnnuciator
  IAnnunciator *pIAnn;

  // IALERT
  IALERT       *pIAlert;

  // Is system time valid?  
  boolean       bTimeValid;   

  // Time in seconds since epoch time adjusted to local time
  // when the phone acquires valid system time
  uint32        uSysTimeAcquiredTime;   
   
  // Snooze duration in seconds
  uint32        uSnoozeDuration;

  // What is the screen size?
  uint16        wCxScreen;

  // Alarm item currently selected from the main list.
  uint16       wAlarmSelIdx;

  // Alarm that has most recently expired
  uint16       wAlarmExpiredIdx;
} AlarmClockApp;

#endif /* ALARMCLOCK_H */

#ifndef AEEALARM_H
#define AEEALARM_H
#error AEEALARM_H
/*======================================================
FILE:  AEEALARM.h

SERVICES:  Interface to the Real Time Clock alarm.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for IALARM. Note that this interface is
   primarily for the annunciator to query for RTC alarm status and get event
   notifications. Applications and extensions should use ISHELL_SetAlarm, and
   ISHELL_CancelAlarm to manipulate BREW alarms.

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================
                        INCLUDES
===========================================================================*/

#include "AEE.h"

#include "ALARM.bid"
#include "ALARM_MODEL.BID"

/*===========================================================================

                        DEFINES AND CONSTANTS

===========================================================================*/

//
// IAlarm Model Events
// 

// Used to indicate that an alarm is set. dwParam is not used.
#define IALARM_EVT_ALARM_SET 0x0001

// Used to indicate that an alarm is cleared. dwParam is not used.
#define IALARM_EVT_ALARM_CLEARED 0x0002

/*===========================================================================

                        PUBLIC DATA TYPES

===========================================================================*/

/*===========================================================================

                        IAlarm Interface

===========================================================================*/

typedef struct _IAlarm  IAlarm;

QINTERFACE(IAlarm)
{
   INHERIT_IQueryInterface(IAlarm);

   boolean (*AlarmIsActive)(IAlarm * po);

};

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

static __inline int IALARM_AddRef(IAlarm * po) 
{
   return AEEGETPVTBL((po),IAlarm)->AddRef(po);
}

static __inline int IALARM_Release(IAlarm * po) 
{
   return AEEGETPVTBL((po),IAlarm)->Release(po);
}

static __inline int IALARM_QueryInterface(IAlarm *po, AEECLSID iid, void **ppo)
{
   return AEEGETPVTBL((po),IAlarm)->QueryInterface(po,iid,ppo);
}

/*===========================================================================

FUNCTION IALARM_AlarmIsActive

DESCRIPTION
  This interface returns TRUE if an alarm is set.

DEPENDENCIES
  None

PROTOTYPE
  boolean IALARM_AlarmIsActive
  (
    IAlarm *               pIAlarm
  )

PARAMETERS
  pIAlarm                 [In]- Pointer to IAlarm object

RETURN VALUE
  TRUE         At least one or more alarm is set.
  FALSE        No alarms are set.

COMMENTS 

SIDE EFFECTS
  None
===========================================================================*/
static __inline boolean IALARM_AlarmIsActive(IAlarm * po) 
{
   return AEEGETPVTBL((po),IAlarm)->AlarmIsActive(po);
}

#endif   // AEEALARM_H

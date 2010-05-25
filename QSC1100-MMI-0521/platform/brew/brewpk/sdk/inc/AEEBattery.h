#ifndef AEEBATTERY_H
#define AEEBATTERY_H

/*=============================================================================

FILE: AEEBattery.h
 
SERVICES:  IBattery interface definition
 
GENERAL DESCRIPTION:
   The IBattery interface provides battery related information

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IBattery, IBatteryNotifier
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None
 
Copyright (c) 2002,2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "AEE.h"
#include "AEEBATTERY.bid"
#include "AEEBATTERYNOTIFIER.bid"

/*=============================================================================

                    DEFINITIONS AND CONSTANTS

=============================================================================*/
//
// IBatteryNotifier EVT_NOTIFY events
// 

#define NMASK_BATTERY_STATUS_CHANGE                   0x0001
#define NMASK_BATTERY_LEVEL_CHANGE                    0x0002
#define NMASK_BATTERY_CHARGERSTATUS_CHANGE            0x0004
#define NMASK_BATTERY_EXTPWR_CHANGE                   0x0008

typedef struct IBatteryNotifier IBatteryNotifier;

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

#define  AEEBATTERY_CHARGERSTATUS_UNKNOWN             0
#define  AEEBATTERY_CHARGERSTATUS_NO_BATT             1
#define  AEEBATTERY_CHARGERSTATUS_UNKNOWN_BATT        2
#define  AEEBATTERY_CHARGERSTATUS_DEAD_BATT           3
#define  AEEBATTERY_CHARGERSTATUS_NO_CHARGE           4
#define  AEEBATTERY_CHARGERSTATUS_NO_CHARGER          5
#define  AEEBATTERY_CHARGERSTATUS_CANNOT_CHARGE       6
#define  AEEBATTERY_CHARGERSTATUS_INIT                7
#define  AEEBATTERY_CHARGERSTATUS_CHARGING            8
#define  AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE        9
#define  AEEBATTERY_CHARGERSTATUS_OVERVOLTAGE         10
#define  AEEBATTERY_CHARGERSTATUS_MAX                 0xFFFFFFFF

typedef int AEEBatteryChargerStatus;

#define  AEEBATTERY_STATUS_UNKNOWN                    0
#define  AEEBATTERY_STATUS_POWERDOWN                  1
#define  AEEBATTERY_STATUS_LOW                        2
#define  AEEBATTERY_STATUS_NORMAL                     3
#define  AEEBATTERY_STATUS_MAX                        0xFFFFFFFF

typedef int AEEBatteryStatus; 


#define  AEEBATTERY_ITEM_STATUS                       0
#define  AEEBATTERY_ITEM_LEVEL                        1
#define  AEEBATTERY_ITEM_CHARGERSTATUS                2
#define  AEEBATTERY_ITEM_EXTPWR                       3
#define  AEEBATTERY_ITEM_LAST                         4
#define  AEEBATTERY_ITEM_MAX                          0xFFFFFFFF

typedef int AEEBatteryItemID;

// Define IBattery model events
#define EVT_MDL_BATTERY_STATUS_CHANGE                   (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_BATTERY_LEVEL_CHANGE                    (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_BATTERY_CHARGERSTATUS_CHANGE            (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_BATTERY_EXTPWR_CHANGE                   (EVT_MDL_GENERIC_CHANGE + 4)

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

// Define IBattery interface
typedef struct IBattery IBattery;

AEEINTERFACE(IBattery)
{
   INHERIT_IQueryInterface(IBattery);

   int (*GetBatteryItem)(IBattery *p, uint32 nItemID, uint32 *pdwData);
};

static __inline uint32 IBATTERY_AddRef(IBattery *p) 
{
   return AEEGETPVTBL((p),IBattery)->AddRef(p);
}

static __inline uint32 IBATTERY_Release(IBattery *p) 
{
   return AEEGETPVTBL((p),IBattery)->Release(p);
}

static __inline int IBATTERY_QueryInterface(IBattery *p, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL((p),IBattery)->QueryInterface(p,id,ppo);
}

static __inline int IBATTERY_GetBatteryStatus(IBattery *p, AEEBatteryStatus *pBatteryStatus)
{
   return AEEGETPVTBL(p,IBattery)->GetBatteryItem(p, AEEBATTERY_ITEM_STATUS, (uint32*)pBatteryStatus);
}

static __inline int IBATTERY_GetBatteryLevel(IBattery *p, uint32 * pdwData)
{
   return AEEGETPVTBL(p,IBattery)->GetBatteryItem(p, AEEBATTERY_ITEM_LEVEL, (uint32*)pdwData);
}

static __inline int IBATTERY_GetBatteryChargerStatus(IBattery *p, AEEBatteryChargerStatus *pBatteryChargerStatus)
{
   return AEEGETPVTBL(p,IBattery)->GetBatteryItem(p, AEEBATTERY_ITEM_CHARGERSTATUS, (uint32*)pBatteryChargerStatus);
}

static __inline int IBATTERY_IsExternalPowerPresent(IBattery *p, boolean *pbData)
{
   uint32 dwData = 0;
   int nErr;

   if (!pbData)
   {
	   return EBADPARM;
   }

   nErr = AEEGETPVTBL(p,IBattery)->GetBatteryItem(p, AEEBATTERY_ITEM_EXTPWR, &dwData);

   if (nErr == SUCCESS)
   {
      *pbData = (dwData ? TRUE : FALSE);
   }

   return nErr;
}

#define GETBATTERYSCALE(dwLevel)       (uint16)(dwLevel >> 16)
#define GETBATTERYLEVEL(dwLevel)       (uint16)(dwLevel & 0x0000ffff)
/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================

AEEBatteryChargerStatus

Description:
   Specifies the battery charger status

Definition:
typedef enum {
   AEEBATTERY_CHARGERSTATUS_UNKNOWN,
   AEEBATTERY_CHARGERSTATUS_NO_BATT,
   AEEBATTERY_CHARGERSTATUS_UNKNOWN_BATT,
   AEEBATTERY_CHARGERSTATUS_DEAD_BATT,
   AEEBATTERY_CHARGERSTATUS_NO_CHARGE,
   AEEBATTERY_CHARGERSTATUS_NO_CHARGER,
   AEEBATTERY_CHARGERSTATUS_CANNOT_CHARGE,
   AEEBATTERY_CHARGERSTATUS_INIT,
   AEEBATTERY_CHARGERSTATUS_CHARGING,
   AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE,
   AEEBATTERY_CHARGERSTATUS_OVERVOLTAGE,
   AEEBATTERY_CHARGERSTATUS_MAX = 0xFFFFFFFF
} AEEBatteryChargerStatus;

Members:
   AEEBATTERY_CHARGERSTATUS_UNKNOWN: Charger status not known
   AEEBATTERY_CHARGERSTATUS_NO_BATT: Charger couldn't detect a battery
   AEEBATTERY_CHARGERSTATUS_UNKNOWN_BATT: Charger found an unrecognized battery
   AEEBATTERY_CHARGERSTATUS_DEAD_BATT: Charger found a dead battery
   AEEBATTERY_CHARGERSTATUS_NO_CHARGE: Nothing's wrong. Just haven't started
   AEEBATTERY_CHARGERSTATUS_NO_CHARGER: Charger isn't working now
   AEEBATTERY_CHARGERSTATUS_CANNOT_CHARGE: Cannot charge for the moment
   AEEBATTERY_CHARGERSTATUS_INIT: Initialized state
   AEEBATTERY_CHARGERSTATUS_CHARGING: Battery is being charged
   AEEBATTERY_CHARGERSTATUS_FULLY_CHARGE: Battery is fully charged
   AEEBATTERY_CHARGERSTATUS_OVERVOLTAGE: Voltage input too high for phone

Comments:
   None

See Also:
   None

===============================================================================

AEEBatteryStatus

Description:
   Specifies the battery status

Definition:
typedef enum {
   AEEBATTERY_STATUS_UNKNOWN,
   AEEBATTERY_STATUS_POWERDOWN,
   AEEBATTERY_STATUS_LOW,
   AEEBATTERY_STATUS_NORMAL,
   AEEBATTERY_STATUS_MAX = 0xFFFFFFFF
} AEEBatteryStatus; 

Members:
   AEEBATTERY_STATUS_UNKNOWN: Battery status is unknown
   AEEBATTERY_STATUS_POWERDOWN: Phone must be powered down
   AEEBATTERY_STATUS_LOW: Battery is low
   AEEBATTERY_STATUS_NORMAL: Battery is normal

Comments:
   None

See Also:
   None

===============================================================================

IBattery model events

Description:
   Events provided to listeners registered with IBattery IModel
   interface.

Definition:

#define EVT_MDL_BATTERY_STATUS_CHANGE                   (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_BATTERY_LEVEL_CHANGE                    (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_BATTERY_CHARGERSTATUS_CHANGE            (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_BATTERY_EXTPWR_CHANGE                   (EVT_MDL_GENERIC_CHANGE + 4)

Members:

EVT_MDL_BATTERY_STATUS_CHANGE       : Event sent when battery status changes. dwParam of 
                                      the ModelEvent struct contains the new battery status.

EVT_MDL_BATTERY_LEVEL_CHANGE        : Event sent when battery level changes. dwParam of 
                                      the ModelEvent struct contains the new battery level.

EVT_MDL_BATTERY_CHARGERSTATUS_CHANGE: Event sent when battery charger status changes. 
                                      dwParam of the ModelEvent struct contains the new 
                                      battery charger status.

EVT_MDL_BATTERY_EXTPWR_CHANGE       : Event sent when external power state changes. dwParam
                                      of the ModelEvent struct contains TRUE if external 
                                      power is currently present and FALSE if external 
                                      power is currently not present.

Comments:
   None

See Also:
   None

===============================================================================
   MACROS DOCUMENTATION
===============================================================================

GETBATTERYSCALE()

Description:
   This macro retrieves battery level scale from 32 bit battery level value
provided by IBATTERY_GetBatteryLevel().

Definition:
   #define GETBATTERYSCALE(dwLevel)       (uint16)(dwLevel >> 16)

Parameters:
   dwLevel: 32 bit battery level value provided by IBATTERY_GetBatteryLevel()

Evaluation Value:
   16 bit battery level scale

Comments:
   None

Side Effects:
   None

See Also:
   IBATTERY_GetBatteryLevel
===============================================================================

GETBATTERYLEVEL()

Description:
   This macro retrieves battery level from 32 bit battery level value
provided by IBATTERY_GetBatteryLevel().

Definition:

#define GETBATTERYLEVEL(dwLevel)       (uint16)(dwLevel & 0x0000ffff)

Parameters:
   dwLevel: 32 bit battery level value provided by IBATTERY_GetBatteryLevel()

Evaluation Value:
   16 bit battery level

Comments:
   None

Side Effects:
   None

See Also:
   IBATTERY_GetBatteryLevel

===============================================================================
  INTERFACES   DOCUMENTATION
===============================================================================

Interface Name: IBatteryNotifier

Description:
  IBatteryNotifier is the notifier class with which applications can register
  for battery notifications in following ways:

  1. Using application MIF
  2. Using ISHELL_RegisterNotify()

  Applications are not required and can't create an instance of IBatteryNotifier.

Following are the notification masks supported by IBatteryNotifier:

#define NMASK_BATTERY_STATUS_CHANGE                   0x0001

 An EVT_NOTIFY event with NMASK_BATTERY_STATUS_CHANGE mask is sent when there is a 
 change in the battery status as reported by IBATTERY_GetBatteryStatus().

 ((AEENotify*)dwParam)->pData contains the new battery status.

 Note: A battery status of AEEBATTERY_STATUS_POWERDOWN may occur even when external 
       power is present, if external power fails to provide sufficient power to 
       maintain a voice call.

#define NMASK_BATTERY_LEVEL_CHANGE                    0x0002

 An EVT_NOTIFY event with NMASK_BATTERY_LEVEL_CHANGE mask is sent when there is a 
 change in the battery level as reported by IBATTERY_GetBatteryLevel().

 ((AEENotify*)dwParam)->pData contains the new battery level.

#define NMASK_BATTERY_CHARGERSTATUS_CHANGE            0x0004

 An EVT_NOTIFY event with NMASK_BATTERY_CHARGERSTATUS_CHANGE mask is sent when there 
 is a change in the battery charger status as reported by IBATTERY_GetBatteryChargerStatus().

 ((AEENotify*)dwParam)->pData contains the new battery charger status.

#define NMASK_BATTERY_EXTPWR_CHANGE                   0x0008

 An EVT_NOTIFY event with NMASK_BATTERY_EXTPWR_CHANGE mask is sent when there is 
 a change in the external power state.

 ((AEENotify*)dwParam)->pData is TRUE if external power is currently present and
 is FALSE if external power is currently not present.

===============================================================================
  INTERFACES   DOCUMENTATION
===============================================================================

Interface Name: IBattery

Description:
   IBattery interface lets application get battery related information.

The IBattery interface instance is obtained via ISHELL_CreateInstance()
mechanism.

IBattery also supports IModel interfcae. Client can access IModel
interface by calling IBATTERY_QueryInterface() with AEEIID_MODEL and 
register listeners using IMODEL_AddListener.

===============================================================================

IBATTERY_AddRef()

Description:
   This method increments the reference count of the IBattery Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IBATTERY_AddRef(IBattery * po)

Parameters:

   po [in]: Pointer to the IBattery Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   IBATTERY_Release()

===============================================================================

IBATTERY_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IBATTERY_Release(IBattery * po)

Parameters:

   po [in]: Pointer to the IBATTERY Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   IBATTERY_AddRef()

===============================================================================

IBATTERY_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int IBATTERY_QueryInterface(IBattery *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to IBattery interface.

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:

   Return SUCCESS on success,
   ECLASSNOTSUPPORT if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

===============================================================================

IBATTERY_GetBatteryStatus

Description:  
   This method queries the current battery status

Prototype:
   int IBATTERY_GetBatteryStatus(IBattery *p, AEEBatteryStatus *pBatteryStatus)

Parameters:
   p [in]: Pointer to IBattery interface
   pBatteryStatus [in/out]: Place holder for battery status

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   None

Side Effects:
   None

===============================================================================

IBATTERY_GetBatteryLevel

Description:  
   This method queries the current battery level. Most significant 16 bits give 
battery level scale whereas least significant 16 bits will give battery level.

Prototype:
   int IBATTERY_GetBatteryLevel(IBattery *p, uint32 * pdwData)

Parameters:
   p [in]: Pointer to IBattery interface
   pdwData [in/out]: Place holder for battery level

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   Use GETBATTERYSCALE() and GETBATTERYLEVEL() to get battery level scale and
battery level.

Side Effects:
   None

===============================================================================

IBATTERY_GetBatteryChargerStatus

Description:  
   This method queries the current battery charger status

Prototype:
   int IBATTERY_GetBatteryChargerStatus(IBattery *p, AEEBatteryChargerStatus *pBatteryChargerStatus)

Parameters:
   p [in]: Pointer to IBattery interface
   pBatteryChargerStatus [in/out]: Place holder for battery charger status

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   None

Side Effects:
   None

===============================================================================

IBATTERY_IsExternalPowerPresent

Description:  
   This method queries if external power is present

Prototype:
   int IBATTERY_IsExternalPowerPresent(IBattery *p, boolean *pbData)

Parameters:
   p [in]: Pointer to IBattery interface
   pbData [in/out]: Place holder for information about external power

Return Value:
   SUCCESS on success,
   Error code otherwise

Comments:
   None

Side Effects:
   None

=============================================================================*/
#endif //AEEBATTERY_H

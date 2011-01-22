#ifndef AEEBATT_H
#define AEEBATT_H

/*=============================================================================

FILE: AEEBatt.h
 
SERVICES:  IBatt interface definition
 
GENERAL DESCRIPTION:
   The IBatt interface provides battery related information

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IBatt,IBattNotifier
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None
 
(c) COPYRIGHT 2002,2004 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEBatt.h#4 $
$DateTime: 2005/05/25 09:10:39 $
  $Author: johns $
  $Change: 200670 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/24/05   jas     Fixing include file case for Linux builds.
11/18/04   sun     Added support for USB Charging
02/25/04   ram     Added initial revision.
02/12/02   mjv     Removed IBATT_GetBattLevel() 
02/09/02   mjv     Updated for 2GUI. 
10/23/02   jcb     Added state AEECHG_STATUS_OVERVOLTAGE for charger  
                   over-voltage.
05/07/02   mjv     Initial revision
 
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"

#include "BATT.BID"
#include "BATT_NOTIFIER.BID"
#include "BATT_MODEL.BID"

#define PARAM_NOT_REF(x)
/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


//
//
// IBattNotifier EVT_NOTIFY events
// 
//

// This event is sent when there is a change in the external power state.
//
// dwParam:  TRUE if external power is currently present
//           FALSE if external power is currently not present
#define NMASK_BATTNOTIFIER_EXTPWR_CHANGE             0x0001 

// This event is sent whenever the battery status changes, as reported by
// IBATT_GetBattStatus()
//
// Note: A battery status of AEEBATTSTATUS_POWERDOWN may occur even
//       when external power is present, as some wimpy wall worts do 
//       not provide sufficient power to maintain a voice call.
//
// dwParam contains the new AEEBattStatus value
#define NMASK_BATTNOTIFIER_BATTSTATUS_CHANGE         0x0002 


// This event is sent whenever the battery level changes, as reported by
// IBATT_GetBattStatus()
//
// dwParam contains the new AEEBattLevel value
#define NMASK_BATTNOTIFIER_BATTLEVEL_CHANGE          0x0004 

// This event is sent when there is change in the charger state.
//
// dwParam contains a value from the AEEChargerStatus enum which
// indicates the new charger status.
#define NMASK_BATTNOTIFIER_CHARGERSTATUS_CHANGE      0x0008 


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef enum {
   /* 
    * Battery annunciator is typically hidden for these states:
    */
   AEECHG_STATUS_NO_BATT,          /* Charger couldn't detect a battery      */
   AEECHG_STATUS_UNKNOWN_BATT,     /* Charger found an unrecognized battery  */
   AEECHG_STATUS_DEAD_BATT,        /* Charger found a dead battery.          */
   AEECHG_STATUS_NO_CHARGE,        /* Nothing's wrong.  Just haven't started */
   AEECHG_STATUS_NO_CHARGER,       /* Charger isn't working now              */
   AEECHG_STATUS_CANNOT_CHARGE,    /* Cannot charge for the moment           */
   AEECHG_STATUS_INIT,             /* Initialized state                      */

   /*
    * Battery annunciator is typically animated for this state:
    */
   AEECHG_STATUS_CHARGING,         /* Battery is being charged               */

   /*
    * Full battery level annunciator is typically displayed for this state:
    */
   AEECHG_STATUS_FULLY_CHARGE,     /* Battery is fully charged               */

   /*
    * Current battery level annunciator is typically displayed for this state:
    */
   AEECHG_STATUS_OVERVOLTAGE ,      /* Voltage input too high for phone       */
   AEECHG_STATUS_NO_CHARGE_NO_BATT /*Charger is weak and no Battery present*/
} AEEChargerStatus;



typedef struct {
   // The current battery level bounded by (0..numlevels-1).  The value 0xFF 
   // may also be used if no battery is present.
   byte  level;      
   
   // Maximum number of battery levels supported by the OEM.
   byte  numlevels;  
} AEEBattLevel;


////
// Battery status reported to the AEE layer
typedef enum {
   AEEBATTSTATUS_POWERDOWN,  // Phone must be powered down
   AEEBATTSTATUS_LOW,        // Battery is low
   AEEBATTSTATUS_NORMAL      // Battery is normal
} AEEBattStatus; 


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/
////
// IBatt interface.
#define INHERIT_IBatt(iname)                                              \
   INHERIT_IQueryInterface(iname);                                        \
   AEEBattStatus        (*GetBattStatus)(iname *p, AEEBattLevel *pLevel); \
   AEEChargerStatus     (*GetChargerStatus)(iname *p);                    \
   boolean              (*GetExternalPower)(iname *p)                     \


AEEINTERFACE_DEFINE(IBatt);
   

static __inline int IBATT_AddRef(IBatt *p) 
{
   return AEEGETPVTBL((p),IBatt)->AddRef(p);
}

static __inline int IBATT_Release(IBatt *p) 
{
   return AEEGETPVTBL((p),IBatt)->Release(p);
}

/*=============================================================================
FUNCTION: IBATT_QueryInterface

DESCRIPTION:  

PARAMETERS:
    *p:  IBatt interface pointer
    id:
  *ppo:

RETURN VALUE:
   int: SUCCESS if successful

COMMENTS:
   IBatt supports the AEEIID_MODEL interface, which may be used as an
   alternative to EVT_NOTIFY to listen for IBatt events (the ModelEvent
   structure is currently undefined -- it will be NULL).

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline int IBATT_QueryInterface(IBatt     *p, 
                                         AEECLSID   id, 
                                         void     **ppo)
{
   return AEEGETPVTBL((p),IBatt)->QueryInterface(p,id,ppo);
}


/*=============================================================================
FUNCTION: IBATT_GetBattStatus

DESCRIPTION:  Returns the current battery status

PARAMETERS:
  *p       [in]:  IBatt interface pointer
  *pLevel [out]:  Returns the current battery level

RETURN VALUE:
   AEEBattStatus:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline AEEBattStatus IBATT_GetBattStatus(IBatt        *p, 
                                                  AEEBattLevel *pLevel)
{
   return AEEGETPVTBL(p,IBatt)->GetBattStatus(p, pLevel);
}


/*=============================================================================
FUNCTION: IBATT_GetChargerStatus

DESCRIPTION:  Returns the current charger state

PARAMETERS:
   *p:  IBatt interface pointer

RETURN VALUE:
   AEEChargerStatus:  current charger state

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline AEEChargerStatus IBATT_GetChargerStatus(IBatt *p)
{
   return AEEGETPVTBL(p,IBatt)->GetChargerStatus(p);
}


/*=============================================================================
FUNCTION: IBATT_GetExternalPower

DESCRIPTION:  Checks if there is currently external power available

PARAMETERS:
   *p:  IBatt interface pointer

RETURN VALUE:
   boolean:  TRUE if external power is available

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline boolean IBATT_GetExternalPower(IBatt *p)
{
   return AEEGETPVTBL(p,IBatt)->GetExternalPower(p);
}


#endif /*AEEBATT_H*/

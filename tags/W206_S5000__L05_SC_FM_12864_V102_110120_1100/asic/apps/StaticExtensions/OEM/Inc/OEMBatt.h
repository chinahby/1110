#ifndef OEMBATT_H
#define OEMBATT_H

/*=============================================================================

FILE: OEMBatt.h
 
SERVICES: OEM Battery Interface 
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2002,2004 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMBatt.h#1 $
$DateTime: 2004/02/25 15:34:38 $
  $Author: rsomani $
  $Change: 95147 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/25/04   ram     Added initial revision.
02/09/02   mjv     Updated for 2GUI. 
09/13/02   mjv     Added return value to AEE_BatteryLevelChange()
06/03/02   mjv     Initial revision
 
=============================================================================*/

 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEEBatt.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                       AEE BATTERY FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: AEEBatt_New

DESCRIPTION: Creates a new IBatt or IBattNotifier interface

PARAMETERS:
   piShell:  IShell interface
       cls:  Class ID of the interface to create
    **ppif:  Newly created interface

RETURN VALUE:
   int:  SUCCESS if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEBatt_New(IShell *piShell, AEECLSID cls, void **ppif);


/*=============================================================================
FUNCTION: AEEBatt_ExternalPowerChange 

DESCRIPTION:  This function must be called by the OEM layer when external
              power is lost or acquired.

PARAMETERS:
   bExt: TRUE if external power is available

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_ExternalPowerChange(boolean bExt);


/*=============================================================================
FUNCTION: AEEBatt_ChargerStateChange

DESCRIPTION:  This function must be called by the OEM layer when it detects
              a change in the battery charger state.

PARAMETERS:
   state: New charger state

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_ChargerStateChange(AEEChargerStatus state);


/*=============================================================================
FUNCTION: AEEBatt_BatteryLevelChange

DESCRIPTION: This function must be called by the OEM layer to report a change
             in battery level to AEE.  

PARAMETERS:
    newLevel:  The new battery level

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_BatteryLevelChange(AEEBattLevel newLevel);


/*=============================================================================
FUNCTION: AEEBatt_BatteryStatusChange

DESCRIPTION: This function must be called by the OEM layer to report a change
             in battery status to AEE.  

PARAMETERS:
   newStatus:  The new battery status

RETURN VALUE:
   None

COMMENTS:
   Until the first time AEEBatt_BatteryLevel() is invoked by the OEM, AEE should
   assume the battery status is OEMBATT_LVL_NORMAL.  Thus if the battery
   is low upon phone startup, the OEM will immediately call this function.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void AEEBatt_BatteryStatusChange(AEEBattStatus newStatus);


/*===========================================================================

                       OEM BATTERY FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: OEMBatt_Init

DESCRIPTION:
   Initialize the OEM Battery layer.  This must be called before the 
   AEE layer uses any other OEM Battery functions.

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMBatt_Init(void);

void OEMBatt_Destory(void);

/*=============================================================================
FUNCTION: OEMBatt_GetBattLevel

DESCRIPTION:  Returns the current battery level 

PARAMETERS:
   None

RETURN VALUE:
   AEEBattLevel:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEBattLevel OEMBatt_GetBattLevel(void);


/*=============================================================================
FUNCTION: OEMBatt_GetBattStatus

DESCRIPTION:  Returns the current battery status

PARAMETERS:
   None

RETURN VALUE:
   AEEBattStatus:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEBattStatus OEMBatt_GetBattStatus(void);


/*=============================================================================
FUNCTION: OEMBatt_GetChargerState

DESCRIPTION:
  Returns the state of the battery charger.

PARAMETERS:
   None

RETURN VALUE:  
   AEEChargerStatusType:  current charger status

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEEChargerStatus OEMBatt_GetChargerState(void);


/*=============================================================================
FUNCTION: OEMBatt_GetExternalPower

DESCRIPTION: Returns the state of external power

PARAMETERS:
   None

RETURN VALUE:
   boolean:  TRUE  if external power is attached
             FALSE if external power is not attached.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMBatt_GetExternalPower(void);


/*===========================================================================

                    STRIDE INTEGRATION

===========================================================================*/


#ifdef _SCL 
#include "SUIDList.h"

// Get the base SUID for AEEBatt
#define SUID_BASE  (SUID_AEEBATT_START)

#pragma scl_func(SUID_BASE+0,AEEBatt_ExternalPowerChange)
#pragma scl_func(SUID_BASE+1,AEEBatt_ChargerStateChange)
#pragma scl_func(SUID_BASE+2,AEEBatt_BatteryLevelChange)
#pragma scl_func(SUID_BASE+3,AEEBatt_BatteryStatusChange)

#pragma scl_func(SUID_BASE+4,OEMBatt_Init)
#pragma scl_func(SUID_BASE+5,OEMBatt_GetBattLevel)
#pragma scl_func(SUID_BASE+6,OEMBatt_GetBattStatus)
#pragma scl_func(SUID_BASE+7,OEMBatt_GetChargerState)
#pragma scl_func(SUID_BASE+8,OEMBatt_GetExternalPower)

// Ensure we haven't overrun our allocated SUID region
#if (SUID_BASE + 8) > SUID_AEEBATT_END
#error SUID_AEEBATT_END is too small
#endif

#undef SUID_BASE

#endif /* _SCL */



#endif /* OEMBATT_H */

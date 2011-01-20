#ifndef AEEJOYSTICK_H
#define AEEJOYSTICK_H

/*============================================================================
FILE:	AEEJoystick.h

SERVICES:  BREW Joystick Services

DESCRIPTION:
   This file defines the IJoystick interface that controls the device joystick.
   
   When created,the IJoystick interface enables the generation of joystick
   (X,Y) coordinates, which are received by the application as BREW joystick
   events (recommended) or through a polling read functionality (provided
   for versions of BREW earlier than 3.1.4).
   
   The IJoystick interface further provides the capability for controlling the
   sample rate and inactivity timeout configuration parameters to the 
   joystick device.   
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#define AEECLSID_IJOYSTICK1    	0x01021c2b
#define AEECLSID_IJOYSTICK2    	0x01021dac
#define AEECLSID_IJOYSTICK     	AEECLSID_IJOYSTICK1

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"    

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

typedef struct IJoystick  IJoystick;

//
// Joystick Control Parms
//
// Set/Get IJoystick parms using IJOYSTICK_SetParm()/IJOYSTICK_GetParm().
//
#define JOY_PARM_BASE           1                       // Base used by IJoystick

#define JOY_PARM_SAMPLEFREQ     (JOY_PARM_BASE)         // [set] p1 = sampling freq, p2 ignored
#define JOY_PARM_INACTIVITY     (JOY_PARM_BASE + 1)     // [set] p1 = inactivity count in ms, p2 ignored
#define JOY_PARM_SF_IC          (JOY_PARM_BASE + 2)     // [set] p1 = sampling freq, p2 = inactivity count in ms]

#define JOY_PARM_MAXCOUNT       (JOY_PARM_BASE + 3)     // [get] returns max allowed inactivity count (ms)
#define JOY_PARM_MAXFREQ        (JOY_PARM_BASE + 4)     // [get] returns max allowed sampling freq (Hz)

// JOY_ID_PRIMARY and JOY_ID_SECONDARY are not used. These are maintained for backward compatibility with
// BREW versions prior to BREW 3.1.5.
#define JOY_ID_PRIMARY		      0			              // Unused
#define JOY_ID_SECONDARY	      1			              // Unused

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//********************************************************************************************************************************
//
// IJoystick Interface
//
//********************************************************************************************************************************
AEEINTERFACE(IJoystick)
{
   INHERIT_IQueryInterface(IJoystick);

   int         (*SetParm)(IJoystick * po, int16 nParmID, int32 p1, int32 p2);
   int         (*GetParm)(IJoystick * po, int16 nParmID, int32 * pP1);
   int         (*Read)(IJoystick * po, int16 * px, int16 * py);
};

#define IJOYSTICK_AddRef(p)                       AEEGETPVTBL(p, IJoystick)->AddRef(p)
#define IJOYSTICK_Release(p)                      AEEGETPVTBL(p, IJoystick)->Release(p)
#define IJOYSTICK_QueryInterface(p, i, p2)        AEEGETPVTBL(p, IJoystick)->QueryInterface(p, i, p2)

#define IJOYSTICK_SetParm(p, pn, p1, p2)          AEEGETPVTBL(p, IJoystick)->SetParm(p, pn, p1, p2)
#define IJOYSTICK_GetParm(p, pn, p1, p2)          AEEGETPVTBL(p, IJoystick)->GetParm(p, pn, p1)
#define IJOYSTICK_Read(p, px, py)                 AEEGETPVTBL(p, IJoystick)->Read(p, px, py)

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IJoystick Interface

   This file defines the IJoystick interface that controls the device joystick.
   
   When created,the IJoystick interface enables the generation of joystick
   (X,Y) coordinates, which are received by the application as BREW joystick
   events (recommended) or through a polling read functionality (provided
   for versions of BREW earlier than 3.1.4).
   
   The IJoystick interface further provides the capability for controlling the
   sample rate and inactivity timeout configuration parameters to the 
   joystick device.   

The following header file is required:~
AEEJoystick.h

===========================================================================

IJOYSTICK_AddRef()

Description:
   This function is inherited from IBASE_AddRef(). 

See Also:
   IJOYSTICK_Release()

=============================================================================

IJOYSTICK_Release()

Description:
   This function is inherited from IBASE_Release(). 

See Also:
   IJOYSTICK_AddRef()

=============================================================================

IJOYSTICK_QueryInterface()

Description:
   This function can be used to
    -    Get a pointer to an interface or data based on the input class ID
    -    Query an extended version of the IJoystick-derived class
    -    Support version compatibility

Prototype:
   int IJOYSTICK_QueryInterface
   (
   IJoystick * pIJoystick, 
   AEECLSID clsReq, 
   void ** ppo
   )

Parameters:
   pIJoystick:  [in]:  Pointer to IJoystick Interface.
   clsReq:  [in]:  A globally unique id to identify the entity (interface or
data) that we are trying to query.
      (AEECLSID_IJOYSTICK1, AEECLSID_IJOYSTICK2, AEECLSID_IJOYSTICK)
   ppo:  [out]:  Pointer to the interface or data that we want to retrieve. If
the value passed back is NULL, the interface or data that we query are not available.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not available.

Side Effects:
   If an interface is retrieved, then this function increments its reference count.

   If a data structure is retrieved, then a pointer to the internal structure is
given and user should not free it.

Version:
   Introduced BREW Client 3.1.4

See Also:
   None

=============================================================================

IJOYSTICK_SetParm()

Description:
   This function sets the joystick control parameters.

Prototype:
   int IJOYSTICK_SetParm
   (
   IJoystick * pIJoystick,  
   int16 nParmID,
   int32 p1,
   int32 p2
   )

Parameters:
   pIJoystick:  Pointer to IJoystick Interface.     
   nParmID: Command parameter that controls what will be set by this function.
   p1:  Depends upon nParmID.
   p2:  Depends upon nParmID.

Return Value:
   SUCCESS: Successful. Operation is completed.
   EFAILED: General failure

Comments:
   None:  

Version:
   Introduced BREW Client 3.1.4
   
See Also:

=============================================================================

IJOYSTICK_GetParm()

Description:
   This function gets the joystick control parameters.

Prototype:
   int IJOYSTICK_SetParm
   (
   IJoystick * pIJoystick,  
   int16 nParmID,
   int32 * pP1
   )

Parameters:
   pIJoystick:  Pointer to IJoystick Interface.     
   nParmID: Command parameter that controls what will be set by this function.
   pP1:  Depends upon nParmID.
  
Return Value:
   SUCCESS: Successful. Operation is completed.
   EFAILED: General failure

Comments:
   None  

Version:
   Introduced BREW Client 3.1.4
   
See Also:

=============================================================================
IJOYSTICK_Read()

Description:
   This function reads signed values of the current joystick x, y positions.

Prototype:
   int IJOYSTICK_Read
   (
   IJoystick * pIJoystick,  
   int16 * px,
   int16 * py
   )

Parameters:
   pIJoystick:  Pointer to IJoystick Interface.     
   px:  Pointer to the signed 16 bit x position value.
   py:  Pointer to the signed 16 bit y position value.

Return Value:
   SUCCESS: Successful. Operation is completed.  
   EFAILED: General failure.

Comments:  
   In versions of BREW 3.1 and higher this function may be ignored in 
   favor of handling the EVT_JOYSTICK_POS directly.  All earlier versions 
   of BREW must use this function to obtain x, y values.
   
Version:
   Introduced BREW Client 3.1.4  

See Also:
    
=============================================================================
============================================================================= */

#endif // AEEJOYSTICK_H


#ifndef OEMJOYSTICK_H
#define OEMJOYSTICK_H
/*============================================================================
FILE:        OEMJoystick.h

SERVICES:  BREW Joystick Services

DESCRIPTION:
   This file is contains defines for OEMJoystick.c

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEE_OEMComdef.h"
#include "AEEJoystick.h"

//-------------------------------------------------------------------
//      Macros
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//     Type Declarations
//-------------------------------------------------------------------
#define JOY_DEFAULT_SAMPLEFREQ   (int8)30
#define JOY_DEFAULT_INACTIVITY   (int16)0

#define JOY_MAX_SAMPLEFREQ   0x0000003f
#define JOY_MAX_INACTIVITY   0x0000ffff

//---------------------------------------------------------------------------
//    AEE-OEM Function Declarations
//---------------------------------------------------------------------------
extern void    OEMJoystick_Init(void);
extern int     OEMJoystick_New(AEECLSID cls);
extern void    OEMJoystick_Delete(AEECLSID cls);
                                                                                   
extern int     OEMJoystick_SetParm(AEECLSID cls, int16 nParmID, int32 p1, int32 p2);
extern uint32  OEMJoystick_GetParm(AEECLSID cls, int16 nParmID, uint32* nErr);
extern uint32  OEMJoystick_Read(AEECLSID cls);

//---------------------------------------------------------------------------
//    User Mode AEE-OEM Function Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// AEE Functions used in OEM Layer.
// AEEJoystick_Init(): OEM ModTable entry. Called, during BREW initialization,
//                   in system mode.
// AEEJoystick_New():  OEM ModTable entry. Called, during ISHELL_CreateInstance(),
//                   in user mode.
//---------------------------------------------------------------------------
extern void    AEEJoystick_Init(IShell * ps);
extern int     AEEJoystick_New(IShell * ps, AEECLSID cls, void **ppif);

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
OEMJoystick Interface

   IJoystick implementation in AEE layer uses the interface defined in
   this file to perform all the IJoystick operations.

=============================================================================

OEMJoystick_Init()

Description:
   This function allows OEMs to perform one-time initialization for the joystick
   when BREW starts up.

Prototype:
   void OEMJoystick_Init(void);

Parameters:
   None.

Return Value:
   None

Comments:
   None

See Also:
   None
=============================================================================

OEMJoystick_New()

Description:
   This function creates the OEM joystick object that is refered by AEE IJoystick
   object.

Prototype:
   int OEMJoystick_New(AEECLSID cls);

Parameters:
   cls: Class ID    
   
Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   
See Also:

=============================================================================

OEMJoystick_Delete()

Description:
   This function stops the joystick.

Prototype:
   void OEMJoystick_Delete(AEECLSID cls);

Parameters:   
   cls: class ID

Return Value:
   SUCCESS if successful
   Error code if error

Comments:                     

See Also:       

=============================================================================

OEMJoystick_SetParm()

Description:
   This function sets the joystick control parameters.

Prototype:
   int OEMJoystick_SetParm(AEECLSID cls, int16 nParmID, int32 p1, int32 p2);

Parameters:
   cls: class ID
   nParmID: Command ID.
   p1: Controlled by nParmID.
   p2: Controlled by nParmID.

Return Value:
   SUCCESS:    successful. Operation is completed. 
   EUNSUPPORTED: parm not supported by the class
   EFAILED: joystick driver failed in setting the parameter(s)

Comments:          

See Also:

=============================================================================

OEMJoystick_GetParm()

Description:
   This function gets the joystick control parameters.

Prototype:
   uint32 OEMJoystick_GetParm(AEECLSID cls, int16 nParmID, uint32* nErr);

Parameters:             
   cls: class ID
   nParmID: Command to control the fetched parameters. 
   nErr: Error return value

Return Value:
   The value requested by nParmID.

Comments:          

See Also:


=============================================================================

OEMJoystick_Read()

Description:
   This function reads the joystick x,y signed postition values.

Prototype:
   uint32 OEMJoystick_Read(AEECLSID cls);

Parameters:        
   cls: class ID
   
Return Value:
   x in upper 16 bits, y in lower 16 bits

Comments:          

See Also:
   None

=============================================================================

  INTERFACES   DOCUMENTATION
=======================================================================
AEEJoystick Interface

=======================================================================

AEEJoystick_Init()

Description:
   OEM Mod table entry for IJoystick initialization.

Prototype:
   void AEEJoystick_Init(IShell * ps);

Parameters:
   ps: pointer to IShell

Return Value:
   None.

Comments:
   None

See Also:
  OEMJoystick Interface Documentation
========================================================================*/

#endif // #ifndef OEMJOYSTICK_H

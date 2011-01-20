#ifndef AEEIMediaAudio3D_H
#define AEEIMediaAudio3D_H

/*============================================================================
FILE: AEEIMediaAudio3D.h

SERVICES:  BREW Media Audio Special Effects Services

DESCRIPTION:
   This file defines 3D positional sound interface for BREW Media Audio Special Effects.

PUBLIC CLASSES: 
   IMediaAudio3D implementation

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
//===================================================================
//      Include Files
//===================================================================
#include "AEEIQI.h"
#include "AEEMediaAudioDef.h"

//==========================================================================
//   Interface ID definition
//==========================================================================
#define AEEIID_IMediaAudio3D    0x01042c46

//==========================================================================
//   Type definitions
//==========================================================================

//
// Polar position specification structure
//
typedef struct AEEPolarPosition {
   int32 nAzimuth;    //[Mandatory] Degrees from the -z-axis towards the +x-axis.
   int32 nRange;      //[Mandatory] Millimeters.
   int32 nElevation;  //[Mandatory] Degrees from the xz-plane towards the +y-axis.
} AEEPolarPosition;

//
// Rolloff specification structure
//
typedef struct AEEMediaRollOff {
    int32   nMinDistance;    //[Mandatory] Millimeters. Sound sources closer than this are played at maximum volume.
    int32   nMaxDistance;    //[Mandatory] Millimeters. Sound sources further than this are muted.
    int32   nRollOffFactor;  //[Mandatory] In thousandths. 1000 (1.0) is normal rolloff and 0 is no rolloff.
    boolean bMuteAfterMax;   //[Mandatory] Distance gain behavior for distances beyond nMaxDistance
    uint8   nReserved1;      // Reserved byte
    uint16  nReserved2;      // Reserved word
} AEEMediaRollOff;

//
// IMediaAudio3D Control Parameters
//
#define MM_A3D_PARM_POSITION               1  // [Get/Set] Sound source position
#define MM_A3D_PARM_POLAR_POSITION         2  // [Get/Set] Sound source Polar position
#define MM_A3D_PARM_MODE                   3  // [Get/Set] Sound processing mode
#define MM_A3D_PARM_VOLUME                 4  // [Get/Set] Sound source volume
#define MM_A3D_PARM_VELOCITY               5  // [Get/Set] Sound source velocity
#define MM_A3D_PARM_ROLL_OFF               6  // [Get/Set] Sound rolloff

//
// 3D processing modes
//
#define MM_A3D_MODE_DISABLE                0  // (Default) Disable 3D processing
#define MM_A3D_MODE_NORMAL                 1  // Normal mode
#define MM_A3D_MODE_LISTENER_RELATIVE      2  // All parameters are relative to listener

//===================================================================
//    IMediaAudio3D Interface Definition
//===================================================================
#define INHERIT_IMediaAudio3D(iname)\
   INHERIT_IQI(iname);\
   int (*GetParm)(iname *po, int32 nParmID, void *p1, void *p2);\
   int (*SetParm)(iname *po, int32 nParmID, void *p1, void *p2)

AEEINTERFACE_DEFINE(IMediaAudio3D);

static __inline uint32 IMediaAudio3D_AddRef(IMediaAudio3D *me)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->AddRef(me);
}

static __inline uint32 IMediaAudio3D_Release(IMediaAudio3D *me)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->Release(me);
}

static __inline int IMediaAudio3D_QueryInterface(IMediaAudio3D *me, AEEIID cls, void** ppo)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->QueryInterface(me, cls, ppo);
}

static __inline int IMediaAudio3D_GetParm(IMediaAudio3D *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, nParmID, p1, p2);
}

static __inline int IMediaAudio3D_SetParm(IMediaAudio3D *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, nParmID, p1, p2);
}

static __inline int IMediaAudio3D_SetPosition(IMediaAudio3D *me, AEEVector *pPos, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_POSITION, (void *)pPos, (void *)dwDuration);
}

static __inline int IMediaAudio3D_GetPosition(IMediaAudio3D *me, AEEVector *pPos)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_POSITION, (void *)pPos, 0);
}

static __inline int IMediaAudio3D_SetPolarPosition(IMediaAudio3D *me, AEEPolarPosition *pPolarPos, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_POLAR_POSITION, (void *)pPolarPos, (void *)dwDuration);
}

static __inline int IMediaAudio3D_GetPolarPosition(IMediaAudio3D *me, AEEPolarPosition *pPolarPos)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_POLAR_POSITION, (void *)pPolarPos, 0);
}

static __inline int IMediaAudio3D_SetMode(IMediaAudio3D *me, int nMode)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_MODE, (void *)nMode, 0);
}

static __inline int IMediaAudio3D_GetMode(IMediaAudio3D *me, int *pnMode)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_MODE, (void *)pnMode, 0);
}

static __inline int IMediaAudio3D_SetVolume(IMediaAudio3D *me, int32 nVolume, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_VOLUME, (void *)nVolume, (void *)dwDuration);
}

static __inline int IMediaAudio3D_GetVolume(IMediaAudio3D *me, int32 *pnVolume, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_VOLUME, (void *)pnVolume, pRange);
}

static __inline int IMediaAudio3D_SetVelocity(IMediaAudio3D *me, AEEVector *pVelocity, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_VELOCITY, (void *)pVelocity, (void *)dwDuration);
}

static __inline int IMediaAudio3D_GetVelocity(IMediaAudio3D *me, AEEVector *pVelocity, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_VELOCITY, (void *)pVelocity, pRange);
}

static __inline int IMediaAudio3D_SetRollOff(IMediaAudio3D *me, AEEMediaRollOff *prf, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->SetParm(me, MM_A3D_PARM_ROLL_OFF, (void *)prf, (void *)dwDuration);
}

static __inline int IMediaAudio3D_GetRollOff(IMediaAudio3D *me, AEEMediaRollOff *prf, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudio3D)->GetParm(me, MM_A3D_PARM_ROLL_OFF, (void *)prf, pRange);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEEPolarPosition

Description: 
This structure specifies position in Polar coordinates.

Definition:
   typedef struct AEEPolarPosition {
      int32 nAzimuth;
      int32 nRange;
      int32 nElevation;
   } AEEPolarPosition;


Members:
   nAzimuth:    [Mandatory] Degrees from the -z-axis towards the +x-axis.
                            If the listener is at the default position with default orientation, then
                            0 is straight ahead and 90 is to the right.
   nRange:      [Mandatory] Millimeters.
   nElevation:  [Mandatory] Degrees from the xz-plane towards the +y-axis.
                            If the listener is at the default position with default orientation, then
                             90 is directly above the listener, and 
                            -90 is directly below the listener.
Comments:
   None.

See Also:
   IMediaAudio3D_SetPolarPosition(),
   IMediaAudio3D_GetPolarPosition()

=============================================================================

AEEMediaRollOff

Description: 
This structure specifies rolloff characteristics.

Definition:
   typedef struct AEEMediaRollOff {
    int32   nMinDistance;
    int32   nMaxDistance;
    int32   nRollOffFactor;
    boolean bMuteAfterMax;
    uint8   nReserved1;
    uint16  nReserved2;
   } AEEMediaRollOff;


Members:
   nMinDistance:          [Mandatory] Millimeters. Sound sources closer than this are played at maximum volume.
   nMaxDistance:          [Mandatory] Millimeters. Sound sources further than this are muted.
   nRollOffFactor:        [Mandatory] In thousandths. 1000 (1.0) is normal rolloff and 0 is no rolloff.
   boolean bMuteAfterMax: [Mandatory] Distance gain behavior for distances beyond nMaxDistance.
                                      true if the source is silent beyond the maximum distance, nMaxDistance,
                                      or false if beyond nMaxDistance the sound source gain is held at the same constant
                                      level as at nMaxDistance.
   uint8   nReserved1:                Reserved byte, do not use
   uint16  nReserved2:                Reserved word, do not use

Comments:
   None.

See Also:
   IMediaAudio3D_SetRollOff(),
   IMediaAudio3D_GetRollOff()

=============================================================================
=============================================================================

IMediaAudio3D Control Parameters

Description:
These parameters(MM_A3D_PARM_XXX) allow setting/getting of 3D audio parameters. 
They are used in IMediaAudio3D_SetParm() and IMediaAudio3D_GetParm() APIs.
===pre>
MM_A3D_PARM_POSITION
Set:  Sets the sound source position in Cartesian coordinates.
      p1 = AEEVector *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current positon
      p1 = AEEVector *

MM_A3D_PARM_POLAR_POSITION
Set:  Sets the sound source position in polar coordinates.
      p1 = AEEPolarPosition *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current polar position.
      p1 = AEEPolarPosition *

MM_A3D_PARM_MODE
Set:  Sets the source source method of how audio parameters are processed.
      p1 = MM_A3D_MODE_XXX
Get:  Gets the current mode
      p1 = pointer to int value => MM_A3D_MODE_XXX

MM_A3D_PARM_VOLUME
Set:  Sets the sound source volume.
      p1 = int32 value => millibels of attenuation
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current volume.
      p1 = pointer to int32 value => millibels of attenuation
      p2 = AEEAudioParmRange *

MM_A3D_PARM_VELOCITY
Set:  Sets the sound source rate of motion.
      p1 = AEEVector *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current velocity.
      p1 = AEEVector *
      p2 = AEEAudioParmRange *
IMediaAudio3D
MM_A3D_PARM_ROLL_OFF
Set:  Sets the sound source rolloff.
      p1 = AEEMediaRollOff *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current rolloff.
      p1 = AEEMediaRollOff *
      p2 = AEEAudioParmRange *
===/pre>
Comments: 
   The changes will take effect immediately if a duration value is not supported for a parameter.

=============================================================================
    INTERFACE DOCUMENTATION
=============================================================================

IMediaAudio3D Interface

Description: 
   This interface represents the 3D sound sources which can be added to 
   audio environment, i.e., IMediaAudioEnv object.
   This interface is obtained via IMedia_QueryInterface(AEEIID_IMedioAudio3D) on an IMedia object
   representing a media (audio) content.
   This interface allows applications to set 3D positional parameters for the sound source.

=============================================================================

IMediaAudio3D_GetParm()

Description:
    This function gets the control parameters.

Prototype:
    int IMediaAudio_GetParm
    (
    IMediaAudio3D *pIMediaAudio,
    int16   nParmID, 
    void  * p1, 
    void  * p2,
    )

Parameters:
    pIMediaAudio: [in]:   Pointer to IMediaAudio3D Interface.
    nParmID:      [in]:   Parameter ID
    p1:           [out]:  Depends on nParmID
    p2:           [out]:  Depends on nParmID

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Parameter not supported by the class
    EFAILED:      General failure

Comments:
    None

See Also:
    IMediaAudio3D_SetParm()

=============================================================================

IMediaAudio3D_SetParm()

Description:
    This function sets the control parameters.

Prototype:
    int IMediaAudio_SetParm
    (
    IMediaAudio3D *pIMediaAudio,
    int16 nParmID,
    int32 p1,
    int32 p2,
    )

Parameters:
    pIMediaAudio: [in]:  Pointer to IMediaAudio3D Interface.
    nParmID:      [in]:  Parameter ID
    p1            [in]:  Depends on nParmID
    p2            [in]:  Depends on nParmID
 
Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Parameter not supported by the class
    EFAILED:      General failure

Comments:
    None

See Also:
    IMediaAudio3D_GetParm()
=============================================================================

IMediaAudio3D_GetMode()

Description:
    This function gets the current 3D processing mode.

Prototype:
    int IMediaAudio3D_GetMode
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    int *pnMode
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    pnMode:         [out]:  Pointer to current mode value, MM_A3D_MODE_XXX

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    pnMode value can be one of the following:
    MM_A3D_MODE_DISABLE :          (Default) Disable 3D processing
    MM_A3D_MODE_NORMAL :           Normal mode
    MM_A3D_MODE_LISTENER_RELATIVE: All parameters are relative to listener
    
See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetMode()

=============================================================================

IMediaAudio3D_GetPolarPosition()

Description:
    This function gets the current position in polar coordinates.

Prototype:
    int IMediaAudio3D_GetPolarPosition
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEPolarPosition *pPolarPos
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    pPolarPos:      [out]:  Pointer to structure describing polar position

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetPolarPosition()

=============================================================================

IMediaAudio3D_GetPosition()

Description:
    This function gets the current position in cartesian coordinates.

Prototype:
    int IMediaAudio3D_GetPosition
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEVector *pPos
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    pPos:           [out]:  Pointer to vector describing the position in three dimensional Cartesian coordinates.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetPosition()

=============================================================================

IMediaAudio3D_GetRollOff()

Description:
    This function queries how volume will change with distance.

Prototype:
    int IMediaAudio3D_GetRollOff
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEMediaRollOff *prf,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    prf:            [out]:  Pointer to structure describing rolloff
    pRange:         [out]:  Pointer to structure describing the valid range
                            of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Sound sources that are closer than the minimum distance are played at
    maximum volume, sounds further than the maximum distance are muted, and
    sounds in between are attenuated according to a combination of minimum
    distance and scale. A scale value of 1000 is equivalent to normal rolloff, 
    0 is no rolloff, less than 1000 is less than normal rolloff, and more
    than 1000 is more than normal rolloff.

    If prf->bMuteAfterMax == true, then the sound source is silent beyond the maximum distance, nMaxDistance,
    otherwise, the sound source gain is held at a constant level beyond nMaxDistance.
    
See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetRollOff()

=============================================================================

IMediaAudio3D_GetVelocity()

Description:
    This function gets the current velocity.

Prototype:
    int IMediaAudio3D_GetVelocity
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEVector *pVelocity,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    pVelocity:      [out]:  Pointer to vector describing velocity in 
                            millimeters per second along the x, y, and z axes.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetVelocity()

=============================================================================

IMediaAudio3D_GetVolume()

Description:
    This function gets the sound volume, ranging from 0 (full amplitude) 
    to -9600 (-96.0 dB representing negative infinity).

Prototype:
    int IMediaAudio3D_GetVolume
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    int32 *pnVolume,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudio3D: [in]:   Pointer to IMediaAudio3D Interface.
    pnVolume:       [out]:  Pointer to current value of the volume in millibels.
    pRange:         [out]:  Pointer to structure describing the valid range
                            of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudio3D_GetParm(),
    IMediaAudio3D_SetVolume()

=============================================================================

IMediaAudio3D_SetMode()

Description:
    This function sets the 3D processing mode.

Prototype:
    int IMediaAudio3D_SetMode
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    int nMode
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    nMode:          [in]:  New mode value, MM_A3D_MODE_XXX

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    nMode setting takes effect only when the associated media has been added to the environment.
    Otherwise nMode setting is treated as MM_A3D_MODE_DISABLE.
  
    nMode value can be one of the following:
    MM_A3D_MODE_DISABLE :          (Default) Disable 3D processing
    MM_A3D_MODE_NORMAL :           Normal mode
    MM_A3D_MODE_LISTENER_RELATIVE: All parameters are relative to listener
  
See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetMode(),
    IMediaAUDIOENV_AddMedia()

=============================================================================

IMediaAudio3D_SetPolarPosition()

Description:
    This function sets the position of an object in polar coordinates.

Prototype:
    int IMediaAudio3D_SetPolarPosition
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEPolarPosition *pPolarPos,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    pPolarPos:      [in]:  Pointer to structure describing polar position
    dwDuration:     [in]:  Change occurs smoothly over this amount of time (milliseconds).
    
Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetPolarPosition()

=============================================================================

IMediaAudio3D_SetPosition()

Description:
    This function sets the position of an object in cartesian coordinates.

Prototype:
    int IMediaAudio3D_SetPosition
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEVector *pPos,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    pPos:           [in]:  Pointer to vector describing the position in three dimensional Cartesian coordinates.
    dwDuration:     [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetPosition()

=============================================================================

IMediaAudio3D_SetRollOff()

Description:
    This function specifies how volume will change with distance.

Prototype:
    int IMediaAudio3D_SetRollOff
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEMediaRollOff *prf,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    prf:            [in]:  Pointer to structure describing rolloff
    dwDuration:     [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Sound sources that are closer than the minimum distance are played at
    maximum volume, sounds further than the maximum distance are muted, and
    sounds in between are attenuated according to a combination of minimum
    distance and scale. A scale value of 1000 is equivalent to normal rolloff, 
    0 is no rolloff, less than 1000 is less than normal rolloff, and more
    than 1000 is more than normal rolloff.

    If prf->bMuteAfterMax == true, then the sound source is silent beyond the maximum distance, nMaxDistance,
    otherwise, the sound source gain is held at a constant level beyond nMaxDistance.
    
See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetRollOff()

=============================================================================

IMediaAudio3D_SetVelocity()

Description:
    This function sets the velocity.

Prototype:
    int IMediaAudio3D_SetVelocity
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    AEEVector *pVelocity,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    pVelocity:      [in]:  Pointer to vector describing velocity in 
                           millimeters per second along the x, y, and z axes.
    dwDuration:     [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.
    
See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetVelocity()

=============================================================================

IMediaAudio3D_SetVolume()

Description:
    This function sets the sound source volume, ranging from 0 (full amplitude) 
    to -9600 (-96.0 dB representing negative infinity).

Prototype:
    int IMediaAudio3D_SetVolume
    ( 
    IMediaAudio3D *pIMediaAudio3D,
    int32 *pnVolume,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudio3D: [in]:  Pointer to IMediaAudio3D Interface.
    nVolume:        [in]:  New value of the volume in millibels.
    dwDuration:     [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudio3D_SetParm(),
    IMediaAudio3D_GetVolume()

============================================================================= */

#endif // AEEIMediaAudio3D_H

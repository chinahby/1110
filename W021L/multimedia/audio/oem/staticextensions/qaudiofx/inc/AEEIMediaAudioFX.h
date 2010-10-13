#ifndef AEEIMediaAudioFX_H
#define AEEIMediaAudioFX_H

/*============================================================================
FILE: AEEIMediaAudioFX.h

SERVICES:  BREW Media Audio Special Effects Services

DESCRIPTION:
   This file defines Sound Reverberations interface for BREW Media Audio Special Effects.
   
PUBLIC CLASSES: 
   IMediaAudioFX implementation

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
//   Interface ID definition.
//==========================================================================
#define AEEIID_IMediaAudioFX    0x01042c47

//==========================================================================
//   Type definitions
//==========================================================================
//
// IMediaAudioFX Control Parameters
//

// Reverb parameters
#define MM_AFX_PARM_REVERB_GAIN              0x100  // [Get/Set] Reverb gain
#define MM_AFX_PARM_REVERB_PRESET            0x101  // [Get/Set] Reverb envrionment preset
#define MM_AFX_PARM_REVERB_DECAY_TIME        0x102  // [Get/Set] Reverb decay time

#define MM_AFX_PARM_REVERB_RESERVED1         0X103  // Resevered
#define MM_AFX_PARM_REVERB_RESERVED2         0X104  // Resevered
#define MM_AFX_PARM_REVERB_DAMPING_FACTOR    0x105  // [Get/Set] Damping factor

//
// Reverb presets
//
#define MM_REVERB_ENV_PRESET_NONE            0        
#define MM_REVERB_ENV_PRESET_ROOM            1
#define MM_REVERB_ENV_PRESET_BATHROOM        2
#define MM_REVERB_ENV_PRESET_CONCERTHALL     3
#define MM_REVERB_ENV_PRESET_CAVE            4
#define MM_REVERB_ENV_PRESET_ARENA           5
#define MM_REVERB_ENV_PRESET_FOREST          6
#define MM_REVERB_ENV_PRESET_CITY            7
#define MM_REVERB_ENV_PRESET_MOUNTAINS       8
#define MM_REVERB_ENV_PRESET_UNDERWATER      9
#define MM_REVERB_ENV_PRESET_AUDITORIUM      10
#define MM_REVERB_ENV_PRESET_ALLEY           11
#define MM_REVERB_ENV_PRESET_HALLWAY         12
#define MM_REVERB_ENV_PRESET_HANGAR          13
#define MM_REVERB_ENV_PRESET_LIVINGROOM      14
#define MM_REVERB_ENV_PRESET_SMALLROOM       15
#define MM_REVERB_ENV_PRESET_MEDIUMROOM      16
#define MM_REVERB_ENV_PRESET_LARGEROOM       17
#define MM_REVERB_ENV_PRESET_MEDIUMHALL      18
#define MM_REVERB_ENV_PRESET_LARGEHALL       19
#define MM_REVERB_ENV_PRESET_PLATE           20

//===================================================================
//    IMediaAudioFX Interface Definition
//===================================================================
#define INHERIT_IMediaAudioFX(iname)\
   INHERIT_IQI(iname);\
   int (*GetParm)(iname *po, int32 nParmID, void *p1, void *p2);\
   int (*SetParm)(iname *po, int32 nParmID, void *p1, void *p2)


AEEINTERFACE_DEFINE(IMediaAudioFX);

static __inline uint32 IMediaAudioFX_AddRef(IMediaAudioFX *me)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->AddRef(me);
}

static __inline uint32 IMediaAudioFX_Release(IMediaAudioFX *me)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->Release(me);
}

static __inline int IMediaAudioFX_QueryInterface(IMediaAudioFX *me, AEEIID cls, void** ppo)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->QueryInterface(me, cls, ppo);
}

static __inline int IMediaAudioFX_GetParm(IMediaAudioFX *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->GetParm(me, nParmID, p1, p2);
}

static __inline int IMediaAudioFX_SetParm(IMediaAudioFX *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->SetParm(me, nParmID, p1, p2);
}

static __inline int IMediaAudioFX_SetReverbGain(IMediaAudioFX *me, int32 nGain)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->SetParm(me, MM_AFX_PARM_REVERB_GAIN, (void *)nGain, 0);
}

static __inline int IMediaAudioFX_GetReverbGain(IMediaAudioFX *me, int32 *pnGain, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->GetParm(me, MM_AFX_PARM_REVERB_GAIN, (void *)pnGain, (void *)pRange);
}

static __inline int IMediaAudioFX_SetReverbPreset(IMediaAudioFX *me, int32 nPreset)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->SetParm(me, MM_AFX_PARM_REVERB_PRESET, (void *)nPreset, 0);
}

static __inline int IMediaAudioFX_GetReverbPreset(IMediaAudioFX *me, int32 *pnPreset)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->GetParm(me, MM_AFX_PARM_REVERB_PRESET, (void *)pnPreset, 0);
}

static __inline int IMediaAudioFX_SetReverbDecayTime(IMediaAudioFX *me, uint32 dwTimeMS)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->SetParm(me, MM_AFX_PARM_REVERB_DECAY_TIME, (void *)dwTimeMS, 0);
}

static __inline int IMediaAudioFX_GetReverbDecayTime(IMediaAudioFX *me, uint32 *pdwTimeMS, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->GetParm(me, MM_AFX_PARM_REVERB_DECAY_TIME, (void *)pdwTimeMS, (void *)pRange);
}

static __inline int IMediaAudioFX_SetReverbDampingFactor(IMediaAudioFX *me, uint32 dwDamping)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->SetParm(me, MM_AFX_PARM_REVERB_DAMPING_FACTOR, (void *)dwDamping, 0);
}

static __inline int IMediaAudioFX_GetReverbDampingFactor(IMediaAudioFX *me, uint32 *pdwDamping, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudioFX)->GetParm(me, MM_AFX_PARM_REVERB_DAMPING_FACTOR, (void *)pdwDamping, (void *)pRange);
}

/*=============================================================================
=============================================================================== 

IMediaAudioFX Control Parameters

Description:
These parameters(MM_AFX_PARM_XXX) allow setting/getting of effect parameters. 
They are used in IMediaAudioFX_SetParm() and IMediaAudioFX_GetParm() APIs.
===pre>
MM_AFX_PARM_REVERB_GAIN
Set:  Sets the reverb gain.
      p1 = int32 value => millibels of attenuation
Get:  Gets the current reverb gain setting
      p1 = pointer to int32 value
      p2 = pointer to AEEAudioParmRange

MM_AFX_PARM_REVERB_PRESET
Set:  Sets the preset characteristics for reverb.
      p1 = MM_REVERB_ENV_PRESET_XXX
Get:  Gets the current reverb preset setting
      p1 = pointer to int32 value => MM_REVERB_ENV_PRESET_XXX

MM_AFX_PARM_REVERB_DECAY_TIME
Set:  Sets the reverb decay time.
      p1 = uint32 => decay time
Get:  Gets the current reverb decay time setting
      p1 = pointer to uint32 value
      p2 = AEEAudioParmRange *

MM_AFX_PARM_REVERB_DAMPING_FACTOR 
Set:  Sets the reverb damping factor.
      p1 = uint32 => damping factor
Get:  Gets the current reverb damping factor setting
      p1 = pointer to uint32 value
      p2 = AEEAudioParmRange *
===/pre>

===================================================================== 
    INTERFACE DOCUMENTATION
===================================================================== 

IMediaAudioFX Interface

Description:
   This interface allows application to control reverb related audio effects in the environment. 
   Application can obtain this interface via IMediaAudioENV_QueryInterface(AEEIID_IMediaAudioFX).

   Application can manipulate reverb related audio effects on individual sound sources using IMediaAudioFX
   which can be obtained via IMEDIA_QueryInterface(AEEIID_IMediaAudioFX).

=============================================================================
IMediaAudioFX_GetParm()

Description:
    This function gets the control parameters.

Prototype:
    int IMediaAudioFX_GetParm
    (
    IMediaAudioFX *pIMediaAudio,
    int16   nParmID, 
    void  * p1, 
    void  * p2,
    )

Parameters:
    pIMediaAudio: [in]:   Pointer to IMediaAudioFX Interface.
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
    IMediaAudioFX_SetParm()

=============================================================================

IMediaAudioFX_SetParm()

Description:
    This function sets the control parameters.

Prototype:
    int IMediaAudioFX_SetParm
    (
    IMediaAudioFX *pIMediaAudio,
    int16 nParmID,
    int32 p1,
    int32 p2,

    )

Parameters:
    pIMediaAudio: [in]:  Pointer to IMediaAudioFX Interface.
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
    IMediaAudioFX_GetParm()
=============================================================================

IMediaAudioFX_GetReverbGain()

Description:
    This function gets the overall volume of the reverb, ranging 
    from 0 (full amplitude) to -9600 (-96.0 dB representing negative infinity).

Prototype:
    int IMediaAudioFX_GetReverbGain
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    int32 *pnGain,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudioFX: [in]:   Pointer to IMediaAudioFX Interface.
    pnGain:         [out]:  Pointer to volume in millibels.  
    pRange:         [out]:  Pointer to structure describing the valid range
                            of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset and 
    applies to all objects in the environment.

    The per object reverb volume is -9600 by default.

See Also:
    IMediaAudioFX_GetParm(),
    IMediaAudioFX_SetReverbGain()

=============================================================================

IMediaAudioFX_GetReverbDampingFactor()

Description:
    This function gets the relative decay time of high frequency vs. low 
    frequency content in the environment.

Prototype:
    int IMediaAudioFX_GetReverbDampingFactor
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    uint32 *pdwDamping,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudioFX: [in]:   Pointer to IMediaAudioFX Interface.
    pdwDamping:     [out]:  Pointer to reverberation damping factor.  
                            This parameter ranges from 1 to 1000000, 
                            where 1000, i.e 1.0 ratio, applies equal weighting 
                            to both high and low frequencies.
    pRange:         [out]:  Pointer to structure describing the valid range
                            of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    A low damping factor means that high frequencies decay very quickly,
    whereas a high damping factor implies the reverse. 
    The *pdwDamping parameter specifies 1000 times the desired reverberation damping factor.
    
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset().

See Also:
    IMediaAudioFX_GetParm(),
    IMediaAudioFX_SetReverbDampingFactor()

=============================================================================

IMediaAudioFX_GetReverbDecayTime()

Description:
    This function gets the amount of time, in milliseconds, that 
    the reverberation will decay in the environment until it reaches 
    -60 dB of the original impulse in.

Prototype:
    int IMediaAudioFX_GetReverbDecayTime
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    uint32 *pdwTimeMS,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudioFX: [in]:   Pointer to IMediaAudioFX Interface.
    pdwTimeMS:      [out]:  Pointer to decay time (milliseconds).  
    pRange:         [out]:  Pointer to structure describing the valid range
                            of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Decay time is a measure of how "live" the room is. The larger 
    the decay time, the longer the echoes will sound. 
    
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset.

See Also:
    IMediaAudioFX_GetParm(),
    IMediaAudioFX_SetReverbDecayTime()

=============================================================================

IMediaAudioFX_GetReverbPreset()

Description:
    This function gets the reverberation environment preset. 

Prototype:
    int IMediaAudioFX_GetReverbPreset
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    int32 *pnPreset
    )

Parameters:
    pIMediaAudioFX: [in]:   Pointer to IMediaAudioFX Interface.
    pnPreset:       [out]:  Pointer to environment preset MM_REVERB_ENV_PRESET_XXX  

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudioFX_GetParm(),
    IMediaAudioFX_SetReverbPreset()

=============================================================================

IMediaAudioFX_SetReverbGain()

Description:
    This function sets the overall volume of the reverb, ranging 
    from 0 (full amplitude) to -9600 (-96.0 dB representing negative infinity).

Prototype:
    int IMediaAudioFX_SetReverbGain
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    int32 nGain
    )

Parameters:
    pIMediaAudioFX: [in]:  Pointer to IMediaAudioFX Interface.
    nGain:          [in]:  Volume in millibels.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset and 
    applies to all objects in the environment.

    The per object reverb volume is set to -9600 by default.

    pnGain setting for a media object takes effect only when the associated media has been added to the environment.
    Otherwise pnGain setting is treated as -9600.


See Also:
    IMediaAudioFX_SetParm(),
    IMediaAudioFX_GetReverbGain()

=============================================================================

IMediaAudioFX_SetReverbDampingFactor()

Description:
    This function sets the relative decay time of high frequency 
    vs. low frequency content.

Prototype:
    int IMediaAudioFX_SetReverbDampingFactor
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    uint32 dwDamping
    )

Parameters:
    pIMediaAudioFX: [in]:  Pointer to IMediaAudioFX Interface.
    dwDamping:      [in]:  Reverberation damping factor.  This 
                           parameter ranges from 1 to 1000000, where 
                           1000, i.e 1.0 ratio, applies equal weighting 
                           to both high and low frequencies.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    A low damping factor means that high frequencies decay very quickly, 
    whereas a high damping factor implies the reverse.
    The dwDamping parameter specifies 1000 times the desired reverberation damping factor.
    
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset.

See Also:
    IMediaAudioFX_SetParm(),
    IMediaAudioFX_GetReverbDampingFactor()

=============================================================================

IMediaAudioFX_SetReverbDecayTime()

Description:
    This function sets the amount of time, in milliseconds, that 
    the reverberation will decay in the environment until it reaches 
    -60 dB of the original impulse.

Prototype:
    int IMediaAudioFX_SetReverbDecayTime
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    uint32 dwTimeMS
    )

Parameters:
    pIMediaAudioFX: [in]:  Pointer to IMediaAudioFX Interface.
    dwTimeMS:       [in]:  Decay time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Decay time is a measure of how "live" the room is. The larger 
    the decay time, the longer the echoes will sound. 
    
    This parameter is automatically initialized by IMediaAudioFX_SetReverbPreset.

See Also:
    IMediaAudioFX_SetParm(),
    IMediaAudioFX_GetReverbDecayTime()

=============================================================================

IMediaAudioFX_SetReverbPreset()

Description:
    This function sets the reverberation environment preset.

Prototype:
    int IMediaAudioFX_SetReverbPreset
    ( 
    IMediaAudioFX *pIMediaAudioFX,
    int32 nPreset
    )

Parameters:
    pIMediaAudioFX: [in]:  Pointer to IMediaAudioFX Interface.
    nPreset:        [in]:  Environment setting MM_REVERB_ENV_PRESET_XXX

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Sets the reverberation environment to the type of room in which 
    the listener is located.

See Also:
    IMediaAudioFX_SetParm(),
    IMediaAudioFX_GetReverbPreset()

============================================================================= */

#endif // AEEIMediaAudioFX_H

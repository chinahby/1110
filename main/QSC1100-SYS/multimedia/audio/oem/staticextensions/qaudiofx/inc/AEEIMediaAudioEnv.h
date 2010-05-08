#ifndef AEEIMediaAudioEnv_H
#define AEEIMediaAudioEnv_H

/*============================================================================
FILE: AEEIMediaAudioEnv.h

SERVICES:  BREW Media Audio Special Effects Services

DESCRIPTION:
   This file defines Audio Environment interface for BREW Media Audio Special Effects.

PUBLIC CLASSES: 
   IMediaAudioEnv implementation

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
#include "AEEIMedia.h"

//==========================================================================
//   Interface ID definition
//==========================================================================
#define AEEIID_IMediaAudioEnv   0x0103fdeb

//==========================================================================
//   Type definitions
//==========================================================================

//
// Orientation specification structure
//
typedef struct AEEMediaOrientation {
    AEEVector ForwardVector;  //[Mandatory] Vector describing the forward direction.
    AEEVector UpVector;       //[Mandatory] Vector describing the direction up from the top of the listener's head.
} AEEMediaOrientation;


//
// IMediaAudioEnv Control Parameters
//
#define MM_AENV_PARM_ENABLE                     1  // [Get/Set] Enable audio processing
#define MM_AENV_PARM_OUTPUT_DEVICE              2  // [Get/Set] Output Device
#define MM_AENV_PARM_LISTENER_POSITION          3  // [Get/Set] Listener position
#define MM_AENV_PARM_LISTENER_VELOCITY          4  // [Get/Set] Listener velocity
#define MM_AENV_PARM_LISTENER_ORIENTATION       5  // [Get/Set] Listener orientation

//
// Enable mask for audio environment effects
//
#define MM_AENV_ENABLE_NONE                     0x00000000  // (Default) All audio processing off
#define MM_AENV_ENABLE_3D                       0x00000001  // 3D processing on
#define MM_AENV_ENABLE_REVERB                   0x00000002  // Reverb processing on
#define MM_AENV_ENABLE_ALL                      0xFFffFFff  // All processing on

//===================================================================
//    IMediaAudioEnv Interface Definition
//===================================================================
#define INHERIT_IMediaAudioEnv(iname)\
   INHERIT_IQI(iname);\
   int (*GetParm)(iname *po, int32 nParmID, void *p1, void *p2);\
   int (*SetParm)(iname *po, int32 nParmID, void *p1, void *p2);\
   int (*AddMedia)(iname *po, IMedia * pIMedia);\
   int (*RemoveMedia)(iname *po, IMedia * pIMedia);\
   int (*GetMediaList)(iname *po, IMedia **ppIMediaList, uint16 * pnCount);\
   int (*EnableDeferCommit)(iname *po, boolean *pbNew, boolean *pbOld);\
   int (*Commit)(iname *po, AEECallback *pcb, int32 *pnRet)

AEEINTERFACE_DEFINE(IMediaAudioEnv);

static __inline uint32 IMediaAudioEnv_AddRef(IMediaAudioEnv *me)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->AddRef(me);
}

static __inline uint32 IMediaAudioEnv_Release(IMediaAudioEnv *me)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->Release(me);
}

static __inline int IMediaAudioEnv_QueryInterface(IMediaAudioEnv *me, AEEIID cls, void** ppo)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->QueryInterface(me, cls, ppo);
}

static __inline int IMediaAudioEnv_GetParm(IMediaAudioEnv *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetParm(me, nParmID, p1, p2) ;
}

static __inline int IMediaAudioEnv_SetParm(IMediaAudioEnv *me, int32 nParmID, void *p1, void *p2)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, nParmID, p1, p2);
}

static __inline int IMediaAudioEnv_AddMedia(IMediaAudioEnv *me, IMedia * pIMedia)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->AddMedia(me, pIMedia);
}

static __inline int IMediaAudioEnv_RemoveMedia(IMediaAudioEnv *me, IMedia * pIMedia)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->RemoveMedia(me, pIMedia);
}

static __inline int IMediaAudioEnv_GetMediaList(IMediaAudioEnv *me, IMedia **ppIMediaList, uint16 * pnCount)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetMediaList(me, ppIMediaList, pnCount);
}

static __inline int IMediaAudioEnv_EnableDeferCommit(IMediaAudioEnv *me, boolean *pbNew, boolean *pbOld)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->EnableDeferCommit(me, pbNew, pbOld);
}

static __inline int IMediaAudioEnv_Commit(IMediaAudioEnv *me, AEECallback *pcb, int32 *pnRet)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->Commit(me, pcb, pnRet);
}

static __inline int IMediaAudioEnv_Enable(IMediaAudioEnv *me, uint32 *pdwNew, uint32 *pdwOld)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, MM_AENV_PARM_ENABLE, (void *)pdwNew, (void *)pdwOld);
}

static __inline int IMediaAudioEnv_SetOutputDevice(IMediaAudioEnv *me, AEESoundDevice eDevice)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, MM_AENV_PARM_OUTPUT_DEVICE, (void *)(int32)eDevice, 0);
}

static __inline int IMediaAudioEnv_GetOutputDevice(IMediaAudioEnv *me, AEESoundDevice *peDevice)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetParm(me, MM_AENV_PARM_OUTPUT_DEVICE, (void *)peDevice, 0);
}

static __inline int IMediaAudioEnv_SetListenerPosition(IMediaAudioEnv *me, AEEVector *pPos, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, MM_AENV_PARM_LISTENER_POSITION, (void *)pPos, (void *)dwDuration);
}

static __inline int IMediaAudioEnv_GetListenerPosition(IMediaAudioEnv *me, AEEVector *pPos)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetParm(me, MM_AENV_PARM_LISTENER_POSITION, (void *)pPos, 0);
}

static __inline int IMediaAudioEnv_SetListenerVelocity(IMediaAudioEnv *me, AEEVector *pVelocity)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, MM_AENV_PARM_LISTENER_VELOCITY, (void *)pVelocity, 0);
}

static __inline int IMediaAudioEnv_GetListenerVelocity(IMediaAudioEnv *me, AEEVector *pVelocity, AEEAudioParmRange *pRange)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetParm(me, MM_AENV_PARM_LISTENER_VELOCITY, (void *)pVelocity, pRange);
}

static __inline int IMediaAudioEnv_SetListenerOrientation(IMediaAudioEnv *me, AEEMediaOrientation *pmo, uint32 dwDuration)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->SetParm(me, MM_AENV_PARM_LISTENER_ORIENTATION, (void *)pmo, (void *)dwDuration);
}

static __inline int IMediaAudioEnv_GetListenerOrientation(IMediaAudioEnv *me, AEEMediaOrientation *pmo)
{
   return AEEGETPVTBL(me,IMediaAudioEnv)->GetParm(me, MM_AENV_PARM_LISTENER_ORIENTATION, (void *)pmo, 0);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEEMediaOrientation

Description: 
This structure specifies 2 vectors to define orientation.

Definition:
   typedef struct AEEMediaOrientation {
       AEEVector ForwardVector;
       AEEVector UpVector;
   } AEEMediaOrientation;


Members:
   ForwardVector:  [Mandatory] Vector describing the forward direction.
   UpVector:       [Mandatory] Vector describing the direction up from the top of the listener's head.

Comments:
   The default ForwardVector is (0, 0, -1) for the listener.
   The default UpVector is (0, 1, 0) for the listener.

See Also:
   IMediaAudioEnv_SetListenerOrientation(),
   IMediaAudioEnv_GetListenerOrientation()

=============================================================================
=============================================================================

IMediaAudioEnv Control Parameters

Description:
These parameters(MM_AENV_PARM_XXX) allow setting/getting of environment parameters. 
They are used in IMediaAudioEnv_SetParm() and IMediaAudioEnv_GetParm() APIs.
===pre>
MM_AENV_PARM_ENABLE
Set:  Enables/disables different types of audio processing.
      p1 = pointer to combined MM_AENV_ENABLE_XXX values.  
      p2 = pointer to uint32 to receive old value.

MM_AENV_PARM_OUTPUT_DEVICE
Set:  Sets the output device.
      p1 = AEESoundDevice 
Get:  Gets the current output device
      p1 = pointer to AEESoundDevice

MM_AENV_PARM_LISTENER_POSITION
Set:  Sets the listener position in Cartesian coordinates.
      p1 = AEEVector *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current listener position.
      p1 = AEEVector *

MM_AENV_PARM_LISTENER_VELOCITY
Set:  Sets the listener velocity.
      p1 = AEEVector *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current listener velocity
      p1 = AEEVector *
      p2 = AEEAudioParmRange *

MM_AENV_PARM_LISTENER_ORIENTATION
Set:  Sets the listener orientation.
      p1 = AEEMediaOrientation *
      p2 = uint32 value => duration over which the change takes place
Get:  Gets the current listener orientation.
      p1 = AEEMediaOrientation *
===/pre>
Comments:
   The changes will take effect immediately if a duration value is not supported for a parameter.

=============================================================================
    INTERFACE DOCUMENTATION
=============================================================================

IMediaAudioEnv Interface

Description: 
   This interface represents the audio environment. 

   It allows applications to:
   - Add/Remove audio (IMedia) objects to/from the environment
   - Set/Get listener related parameters
 
=============================================================================

IMediaAudioEnv_GetParm()

Description:
    This function gets the control parameters.

Prototype:
    int IMediaAudioEnv_GetParm
    (
    IMediaAudioEnv *pIMediaAudio,
    int16   nParmID, 
    void  * p1, 
    void  * p2,
    )

Parameters:
    pIMediaAudio: [in]:   Pointer to IMediaAudioEnv Interface.
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
    IMediaAudioEnv_SetParm()

=============================================================================

IMediaAudioEnv_SetParm()

Description:
    This function sets the control parameters.

Prototype:
    int IMediaAudioEnv_SetParm
    (
    IMediaAudioEnv *pIMediaAudio,
    int16 nParmID,
    int32 p1,
    int32 p2,
    )

Parameters:
    pIMediaAudio: [in]:  Pointer to IMediaAudioEnv Interface.
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
    IMediaAudioEnv_GetParm()

=============================================================================
IMediaAudioEnv_AddMedia()

Description:
    This function associates an IMedia object with a sound environment.

Prototype:
    int IMediaAudioEnv_AddMedia
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    IMedia *pIMedia
    )

Parameters:
    pIMediaAudioEnv: [in]: Pointer to IMediaAudioEnv Interface.
    pIMedia:         [in]: The IMedia object to add to the sound environment.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    EBADSTATE:    Operation not allowed in the current state
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    In order for an IMedia object to produce any sound effects, 
    it must be added to the sound environment,
    the sound environment must be enabled for processing other than MM_AENV_ENABLE_NONE and
    the IMedia 3D audio mode must be set to other than MM_A3D_MODE_DISABLE.

See Also:
    IMediaAudioEnv_RemoveMedia(),
    IMediaAudioEnv_GetMediaList()

=============================================================================

IMediaAudioEnv_Commit()

Description:
    This function commits attached IMediaAudioEnv, IMediaAudioFX and environment settings
    if deferred commit is enabled.

Prototype:
    int IMediaAudioEnv_Commit
    ( 
    IMediaAudioEnv *pIMediaAudio,
    AEECallback *pcb, 
    int32 *pnRet
    )

Parameters: 
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pcb:             [in]:   Function to call when done. This callback
                             cannot be cancelled.
    pnRet:           [out]:  When the callback is called, the result of
                             the commit will be in this variable. This
                             pointer must remain valid until after the
                             callback is called.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    This call allows changes to attached IMediaAudioEnv, IMediaAudioFX and 
    environment settings to be committed at the same time. Without deferred commit, 
    each change happens at the time the corresponding function is called.

See Also:
    IMediaAudioEnv_SetParm(),
    IMediaAudioEnv_EnableDeferCommit()

=============================================================================

IMediaAudioEnv_Enable()

Description:
    This function enables or disables audio environment effects.

Prototype:
    int IMediaAudioEnv_Enable
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    uint32 *pdwNew, 
    uint32 *pdwOld
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pdwNew:          [in]:   Combination of MM_AENV_ENABLE_XXX values. 
                             Pass NULL to leave unchanged.
    pdwOld:          [out]:  Previous value. Pass NULL to ignore previous value.
    
Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    Audio FX (i.e., Reverb) processing is independent of Audio 3D processing.
    Reverb works even if 3D audio is disabled and vice versa.
    By default both 3D and Reverb processing are disabled.
    When enabling both 3D and Reverb processing, use 
    *pdwNew == (MM_AENV_ENABLE_3D | MM_AENV_ENABLE_REVERB).

    Behavior of MM_AENV_ENABLE_3D and MM_AENV_ENABLE_REVERB:

    If 3D is disabled and reverb is enabled, then BREW will process only 
    reverb related commands and all reverb settings take effect. 
    3D related commands will have no effect.
 
    If 3D is enabled and reverb is disabled, then BREW will process only
    3D related commands and all 3D settings take effect.
    Reverb related commands will have no effect.

See Also:
    IMediaAudioEnv_SetParm()

=============================================================================

IMediaAudioEnv_EnableDeferCommit()

Description:
    This function enables or disables deferred commit of changes to settings.

Prototype:
    int IMediaAudioEnv_EnableDeferCommit
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    boolean *pbNew, 
    boolean *pbOld
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pbNew:           [in]:   TRUE/FALSE => Enable/Disable deferred commit.
                             Pass NULL to leave unchanged.
    pbOld:           [out]:  Previous value. Pass NULL to ignore previous value.
    
Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    If deferred commit is enabled, changes to settings require a call to
    IMediaAudioEnv_Commit() to take effect.
    By default all settings take effect immediately.

See Also:
    IMediaAudioEnv_Commit()

=============================================================================

IMediaAudioEnv_GetListenerOrientation()

Description:
    This function gets the orientation of the listener.

Prototype:
    int IMediaAudioEnv_GetListenerOrientation
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEMediaOrientation *pmo
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pmo:             [out]:  Pointer to structure describing the forward
                             and up directions of the listener.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudioEnv_GetParm(),
    IMediaAudioEnv_SetListenerOrientation()

=============================================================================

IMediaAudioEnv_GetListenerPosition()

Description:
    This function gets the listener position.

Prototype:
    int IMediaAudioEnv_GetListenerPosition
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEVector *pPos
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pPos:            [out]:  Pointer to vector describing listener position
                             in three dimensional Cartesian coordinates.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None

See Also:
    IMediaAudioEnv_GetParm(),
    IMediaAudioEnv_SetListenerPosition()

=============================================================================

IMediaAudioEnv_GetListenerVelocity()

Description:
    This function gets the listener velocity in three dimensional
    Cartesian coordinates.

Prototype:
    int IMEDIAUDIOAENV_GetListenerVelocity
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEVector *pVelocity,
    AEEAudioParmRange *pRange
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    pVelocity:       [out]:  Pointer to vector describing listener velocity
                             where the x, y, and z fields are the components
                             of the motion along those axes in millimeters/second.
    pRange:          [out]:  Pointer to structure describing the valid range
                             of values. Pass NULL to ignore range information.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    The radial velocity relative to the listener is calculated from
    the listener position and velocity and the sound source position 
    and velocity. The radial velocity is, in turn, used to calculate the Doppler effect.

See Also:
    IMediaAudioEnv_GetParm(),
    IMediaAudioEnv_SetListenerVelocity()

=============================================================================

IMediaAudioEnv_GetMediaList()

Description:
    This function lists IMedia objects attached to the sound environment. 

Prototype:
    int IMediaAudioEnv_GetMediaList
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    IMedia **ppIMediaList,
    uint16 *pnCount
    )

Parameters:
    pIMediaAudioEnv: [in]:     Pointer to IMediaAudioEnv Interface.
    ppIMediaList:    [out]:    Buffer to be filled with the list of IMedia 
                               pointers. Pass NULL to find out only how many
                               items are in the list.
    pnCount:         [in/out]: On input, the number of IMedia pointers 
                               that can be held in ppIMediaList. On output,
                               the number of IMedia pointers copied into 
                               ppIMediaList if it is not NULL. If ppIMediaList
                               is NULL, the number of items the list needs to
                               hold.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudioEnv_AddMedia(),
    IMediaAudioEnv_RemoveMedia()

=============================================================================

IMediaAudioEnv_GetOutputDevice()

Description:
    This function gets the current output device for the environment.

Prototype:
    int IMediaAudioEnv_GetOutputDevice
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEESoundDevice *peDevice
    )

Parameters:
    pIMediaAudioEnv: [in]:   Pointer to IMediaAudioEnv Interface.
    peDevice:        [out]:  Pointer to current device value.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudioEnv_GetParm(),
    IMediaAudioEnv_SetOutputDevice()

=============================================================================

IMediaAudioEnv_RemoveMedia()

Description:
    This function disassociates an IMedia object from the sound environment.

Prototype:
    int IMediaAudioEnv_RemoveMedia
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    IMedia *pIMedia
    )

Parameters:
    pIMediaAudioEnv: [in]:  Pointer to IMediaAudioEnv Interface.
    pIMedia:         [in]:  The IMedia object to remove from the sound environment.


Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    When an IMedia object is removed from the sound environment it's associated
    sound effects will get suppressed.

See Also:
    IMediaAudioEnv_AddMedia(),
    IMediaAudioEnv_GetMediaList()

=============================================================================

IMediaAudioEnv_SetListenerOrientation()

Description:
    This function sets the orientation of the listener.

Prototype:
    int IMediaAudioEnv_SetListenerOrientation
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEMediaOrientation *pmo,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudioEnv: [in]:  Pointer to IMediaAudioEnv Interface.
    pmo:             [in]:  Pointer to structure describing the forward and up directions of the listener.
    dwDuration:      [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None

See Also:
    IMediaAudioEnv_SetParm(),
    IMediaAudioEnv_GetListenerOrientation()

=============================================================================

IMediaAudioEnv_SetListenerPosition()

Description:
    This function sets the listener position.

Prototype:
    int IMediaAudioEnv_SetListenerPosition
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEVector *pPos,
    uint32 dwDuration
    )

Parameters:
    pIMediaAudioEnv: [in]:  Pointer to IMediaAudioEnv Interface.
    pPos:            [in]:  Vector describing listener position in three dimensional Cartesian coordinates.
    dwDuration:      [in]:  Change occurs smoothly over this amount of time (milliseconds).

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None

See Also:
    IMediaAudioEnv_SetParm(),
    IMediaAudioEnv_GetListenerPosition()

=============================================================================

IMediaAudioEnv_SetListenerVelocity()

Description:
    This function sets the listener velocity in three dimensional 
    Cartesian coordinates.

Prototype:
    int IMediaAudioEnv_SetListenerVelocity
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEEVector *pVelocity
   )

Parameters:
    pIMediaAudioEnv: [in]:  Pointer to IMediaAudioEnv Interface.
    pVelocity:       [in]:  Vector describing listener velocity where the x, y, and z fields 
                            are the components of the motion along those axes in millimeters/second.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    The radial velocity relative to the listener is calculated from 
    the listener position and velocity and the sound source position 
    and velocity. The radial velocity is, in turn, used to calculate the Doppler effect.

See Also:
    IMediaAudioEnv_SetParm(),
    IMediaAudioEnv_GetListenerVelocity()

=============================================================================

IMediaAudioEnv_SetOutputDevice()

Description:
    This function sets the output device for the environment.

Prototype:
    int IMediaAudioEnv_SetOutputDevice
    ( 
    IMediaAudioEnv *pIMediaAudioEnv,
    AEESoundDevice eDevice
    )

Parameters:
    pIMediaAudioEnv: [in]:  Pointer to IMediaAudioEnv Interface.
    eDevice:         [in]:  Device value.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter
    ENOMEMORY:    Not enough memory
    EUNSUPPORTED: Request not supported by the class
    EFAILED:      General failure

Comments:
    None.

See Also:
    IMediaAudioEnv_SetParm(),
    IMediaAudioEnv_GetOutputDevice()

============================================================================= */

#endif // AEEIMediaAudioEnv_H

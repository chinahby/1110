#ifndef AEESOUNDCTL_H
#define AEESOUNDCTL_H

/*============================================================================
FILE:	AEESoundCtl.h

SERVICES:  BREW Sound Control Services

DESCRIPTION:
   This file defines ISoundCtl interface that controls the sound device.
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEError.h"
/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

#define AEE_SOUNDCTL_BASE                                 100           // Base used by ISoundCtl

#define AEE_SOUNDCTL_SUCCESS                              0             // Command was accepted  
#define AEE_SOUNDCTL_FAILURE                              1             // Request is rejected because it is erroneous 
#define AEE_SOUNDCTL_UNKNOWN                              2             // Unknown status 
#define AEE_SOUNDCTL_UNSUPPORTED                          EUNSUPPORTED  // API is not supported

// commands
#define AEE_SOUNDCTL_CMD_BASE                             (AEE_SOUNDCTL_BASE + 100)
#define AEE_SOUNDCTL_CMD_ENABLE_DISABLE_EQUALIZER         (AEE_SOUNDCTL_CMD_BASE + 1)  
#define AEE_SOUNDCTL_CMD_SET_EQUALIZER                    (AEE_SOUNDCTL_CMD_BASE + 2) 
#define AEE_SOUNDCTL_CMD_SET_EQUALIZER_GAIN               (AEE_SOUNDCTL_CMD_BASE + 3)
#define AEE_SOUNDCTL_CMD_ENABLE_DISABLE_AUDIO_EXPANDER    (AEE_SOUNDCTL_CMD_BASE + 4)
#define AEE_SOUNDCTL_CMD_SET_AUDIO_EXPANDER               (AEE_SOUNDCTL_CMD_BASE + 5)
#define AEE_SOUNDCTL_CMD_ENABLE_DISABLE_SPECTRUM_ANALYZER (AEE_SOUNDCTL_CMD_BASE + 6)
#define AEE_SOUNDCTL_CMD_SET_SPECTRUM_ANALYZER            (AEE_SOUNDCTL_CMD_BASE + 7)
#define AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA           (AEE_SOUNDCTL_CMD_BASE + 8)
#define AEE_SOUNDCTL_CMD_ENABLE_DISABLE_ANGLEDMIXING      (AEE_SOUNDCTL_CMD_BASE + 9)
#define AEE_SOUNDCTL_CMD_SET_ANGLEDMIXING                 (AEE_SOUNDCTL_CMD_BASE + 10)

//Equalizer filter boost type
#define AEE_SOUNDCTL_EQUALIZER_BASE_BOOST                 1  // Base Boost
#define AEE_SOUNDCTL_EQUALIZER_TREBLE_BOOST               2  // Treble Boost
#define AEE_SOUNDCTL_EQUALIZER_BAND_BOOST                 3  // Band Boost

// Audio expander mode type
#define AEE_SOUNDCTL_EXPANDER_HEADPHONE                   0  // Head phone 
#define AEE_SOUNDCTL_EXPANDER_SPEAKER_FRONT               1  // Speaker, front firing 
#define AEE_SOUNDCTL_EXPANDER_SPEAKER_SIDE                2  // Speaker, side firing  
#define AEE_SOUNDCTL_EXPANDER_SPEAKER_DESKTOP             3  // Speaker, desktop

//AngledMixing class type
#define AEE_SOUNDCTL_CLASS_VOCODER                        0  //Voice calls
#define AEE_SOUNDCTL_CLASS_AUDIO                          1  //Music, Alerts, etc playback
                                                              
// Parameter type
#define AEE_SOUNDCTL_PARM_BASE                            (AEE_SOUNDCTL_BASE + 200)      // Parms base used by ISoundCtl
#define AEE_SOUNDCTL_PARM_EQUALIZER_NUM_BAND_MAX          (AEE_SOUNDCTL_PARM_BASE + 1)
#define AEE_SOUNDCTL_PARM_SPECTRUM_DATA_PERIOD            (AEE_SOUNDCTL_PARM_BASE + 10) 
#define AEE_SOUNDCTL_PARM_SPECTRUM_DATA_MAX               (AEE_SOUNDCTL_PARM_BASE + 11) 


// Equalizer filter type
typedef struct AEESoundCtlEqualizerFilter
{
  int16                                   nGain;        // Filter band initial Gain (dB) 
                                                        // Can change in steps of 1 dB only 
                                                        // Range is +12 dB to -12 dB 
  uint16                                  nFreq;        // Filter band center frequency  
  uint32                                  dwType;       // Equalizer filter boost type 
                                                        // Filter band type 
  uint16                                  nQF;          // Filter band quality factor 
                                                        // Expressed as q-8 number. 
  uint32                                  dwStructSize; // Size of structure                                                 
}AEESoundCtlEqualizerFilter;

//Audio expander parameter type
typedef struct AEESoundCtlAudioExpander
{
  uint32                          dwMode;          // Audio expander mode type
  uint16                          nGain;           // Linear gain for Expander input per channel
                                                   // range from 0 to 0x7FFF 
                                                   // while 0x7FFF stands for unit gain (0dB)
                                                   // and 0x4027 for -6dB.
  uint16                          nSpread;         // Amount of expansion for Audio 3D expansion 
                                                   // algorithm. range from 0 to 0x7FFF while 0x7FFF
                                                   // corresponding to 1 
  int16                           nGeometry;       // Delay for Audio 3D Expander algorithm. Range
                                                   // could be from -32 to 32.
                                                   // While >=0: delay in direct path.
                                                   //        <0: delay in filter path.
  uint32                          dwStructSize;    // Size of structure                                                    
}AEESoundCtlAudioExpander;

//AEEAngledMixing parameter type
typedef struct AEESoundCtlAngledMixing
{
    uint8                        nClass;        // Class for which the parameters should be applied
                                                // 
    uint16                       nGain;         // Background attenuation for audio source
                                                // range from 0 to 0x7FFFF
    uint16                       nAngle;        // Absolute target azimuth angle for audio source
                                                // range from 0 to 0x167
    uint16                       nTime;         // Transition time to new Gain and Angle for audio source
                                                // range from 0 to 0x7FFF
    uint32                       dwStructSize;  // Size of structure                                               
}AEESoundCtlAngledMixing;

// AEESoundCtlNotify structure is returned via AEECallback
typedef struct ISoundCtl  ISoundCtl;
typedef struct AEESoundCtlNotify
{
   ISoundCtl*     pSoundCtl;         // Pointer to sound control object
   int16          nCmd;              // Command code
   int16          nStatus;           // Status code
   void*          pData;             // Pointer to data buffer
   uint32         dwDataSize;        // Size of pData
} AEESoundCtlNotify;

typedef void (*PFNSOUNDCTLNOTIFY)(void* pUser, AEESoundCtlNotify* pNotify);
               
//********************************************************************************************************************************
//
// ISoundCtl Interface
//
//********************************************************************************************************************************
AEEINTERFACE(ISoundCtl)
{
   INHERIT_IQueryInterface(ISoundCtl);

   int        (*RegisterNotify)(ISoundCtl* po, PFNSOUNDCTLNOTIFY pfnNotify, const void* pUser);

   //Retrieve parameters
   int        (*GetSoundCtlParm)(ISoundCtl * po, int nParamID, int32 * pP1, int32 * pP2);
   //Equalizer functions
   int        (*EnableEqualizer)(ISoundCtl* po, boolean bEnable);
   int        (*SetEqualizer)(ISoundCtl* po, AEESoundCtlEqualizerFilter* eqtype, uint32 bands);
   int        (*SetEqualizerGain)(ISoundCtl* po, uint16 band, int16 nGain);
   //Audio Expander functions
   int        (*EnableAudioExpander)(ISoundCtl* po, boolean bEnable);
   int        (*SetAudioExpander)(ISoundCtl* po, AEESoundCtlAudioExpander* aepType);
   //AngledMixing functions
   int        (*EnableAngledMixing)(ISoundCtl* po, boolean bEnable);
   int        (*SetAngledMixing)(ISoundCtl* po, AEESoundCtlAngledMixing* aepType, uint16 nParams);
   //Spectrum analyzer functions
   int        (*EnableSpectrumAnalyzer)(ISoundCtl* po, boolean bEnable);  
   int        (*SetSpectrumAnalyzer)(ISoundCtl* po, uint16 update_period);
 };
 
#define ISOUNDCTL_AddRef(p)                       AEEGETPVTBL(p, ISoundCtl)->AddRef(p)
#define ISOUNDCTL_Release(p)                      AEEGETPVTBL(p, ISoundCtl)->Release(p)
#define ISOUNDCTL_QueryInterface(p, i, p2)        AEEGETPVTBL(p, ISoundCtl)->QueryInterface(p, i, p2)

#define ISOUNDCTL_RegisterNotify(p, pfn, pu)      AEEGETPVTBL(p, ISoundCtl)->RegisterNotify(p, pfn, pu)

//Retrieve parameters
#define ISOUNDCTL_GetParm(p, c, pp1, pp2)         AEEGETPVTBL(p, ISoundCtl)->GetSoundCtlParm(p, c, pp1, pp2)

// Equalizer functions
#define ISOUNDCTL_EnableEqualizer(p, en)          AEEGETPVTBL(p, ISoundCtl)->EnableEqualizer(p, en)
#define ISOUNDCTL_SetEqualizer(p, eq, bd)         AEEGETPVTBL(p, ISoundCtl)->SetEqualizer(p, eq, bd)
#define ISOUNDCTL_SetEqualizerGain(p, bd, ga)     AEEGETPVTBL(p, ISoundCtl)->SetEqualizerGain(p, bd, ga)

//Audio Expander functions
#define ISOUNDCTL_EnableAudioExpander(p, en)      AEEGETPVTBL(p, ISoundCtl)->EnableAudioExpander(p, en)
#define ISOUNDCTL_SetAudioExpander(p, aep)        AEEGETPVTBL(p, ISoundCtl)->SetAudioExpander(p, aep)

//AngledMixing functions
#define ISOUNDCTL_EnableAngledMixing(p, en)          AEEGETPVTBL(p, ISoundCtl)->EnableAngledMixing(p, en)
#define ISOUNDCTL_SetAngledMixing(p, aep, np)       AEEGETPVTBL(p, ISoundCtl)->SetAngledMixing(p, aep, np)

// Spectrum analyzer functions
#define ISOUNDCTL_EnableSpectrumAnalyzer(p, en)   AEEGETPVTBL(p, ISoundCtl)->EnableSpectrumAnalyzer(p, en)   
#define ISOUNDCTL_SetSpectrumAnalyzer(p, pe)      AEEGETPVTBL(p, ISoundCtl)->SetSpectrumAnalyzer(p, pe)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

Status:
AEE_SOUNDCTL_UNKNOWN : Unknown status
AEE_SOUNDCTL_SUCCESS : Request is accepted
AEE_SOUNDCTL_FAILURE : Error occurred with this request
AEE_SOUNDCTL_UNSUPPORTED : Not supported


Commands:
AEE_SOUNDCTL_CMD_ENABLE_DISABLE_EQUALIZER : Enable/Disable equalizer
AEE_SOUNDCTL_CMD_SET_EQUALIZER : Set Equalizer
AEE_SOUNDCTL_CMD_SET_EQUALIZER_GAIN : Set gain to equalizer on a single band
AEE_SOUNDCTL_CMD_ENABLE_DISABLE_AUDIO_EXPANDER : Enable/Disable audio expander
AEE_SOUNDCTL_CMD_SET_AUDIO_EXPANDER : Set audio expander
AEE_SOUNDCTL_CMD_ENABLE_DISABLE_SPECTRUM_ANALYZER : Enable/Disable spectrum analyzer
AEE_SOUNDCTL_CMD_SET_SPECTRUM_ANALYZER : set spectrum analyzer
AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA : spectrum analyzer data
AEE_SOUNDCTL_CMD_ENABLE_DISABLE_ANGLEDMIXING : Enable/Disable audio qensemble
AEE_SOUNDCTL_CMD_SET_ANGLEDMIXING : Set audio qensemble

Equalizer filter boost type:
AEE_SOUNDCTL_EQUALIZER_BASE_BOOST : Base Boost
AEE_SOUNDCTL_EQUALIZER_TREBLE_BOOST : Treble Boost
AEE_SOUNDCTL_EQUALIZER_BAND_BOOST : Band Boost

Audio expander mode type:
AEE_SOUNDCTL_EXPANDER_HEADPHONE : Head phone 
AEE_SOUNDCTL_EXPANDER_SPEAKER_FRONT : Speaker, front firing
AEE_SOUNDCTL_EXPANDER_SPEAKER_SIDE : Speaker, side firing
AEE_SOUNDCTL_EXPANDER_SPEAKER_DESKTOP : Speaker, desktop

Audio Class type:
AEE_SOUNDCTL_CLASS_VOCODER : Vocoder class type
AEE_SOUNDCTL_CLASS_AUDIO: Audio class type including Music playback, Alerts, Keybeeps, etc.

=============================================================================

AEESoundCtlEqualizerFilter

Description:
AEESoundCtlEqualizerFilter is used to specify the equalizer configuration 
on a selected band.

typedef struct AEESoundCtlEqualizerFilter
{
  int16         nGain;
  uint16        nFreq;
  uint32        dwType;
  uint16        nQF;
  uint32        dwStructSize;
}AEESoundCtlEqualizerFilter;


Members:
nGain : Filter band initial gain (dB)  it can be changed in steps of 1 dB within 
        the range of +12 dB to -12 dB
nFreq : Filter band center frequency 
dwType : Equalizer filter boost type
nQF : Filter band quality factor, it is expressed as q-8 number.
dwStructSize : size of structure

Comments:
None

See Also:
AEE_SOUNDCTL_EQUALIZER_BASE_BOOST 
AEE_SOUNDCTL_EQUALIZER_TREBLE_BOOST
AEE_SOUNDCTL_EQUALIZER_BAND_BOOST

=============================================================================

AEESoundCtlAudioExpander

Description:
AEESoundCtlAudioExpander is used to specify the audio expander tuning parameters

typedef struct AEESoundCtlAudioExpander
{
  uint32          dwMode;
  uint16          nGain;
  uint16          nSpread;
  int16           nGeometry;
  uint32          dwStructSize;
}AEESoundCtlAudioExpander;


Members:
dwMode : Expander mode enum type
nGain : Linear gain for expander input per channel, range from 0 to 0x7FFF, 
       while 0x7FFF stands for unit gain (0dB) and 0x4027 for -6dB. 
nSpread : Amount of expansion for Audio 3D expansion, range from 0 to 0x7FFF, 
       while 0x7FFF corresponding to 1.
nGeometry : Delay for Audio 3D Expander. Range from -32 to 32 
       >=0: delay in direct path, 
       < 0: delay in filter path. 
dwStructSize : Size of structure 

Comments:
None

See Also:
AEE_SOUNDCTL_EXPANDER_HEADPHONE 
AEE_SOUNDCTL_EXPANDER_SPEAKER_FRONT 
AEE_SOUNDCTL_EXPANDER_SPEAKER_SIDE  
AEE_SOUNDCTL_EXPANDER_SPEAKER_DESKTOP

=============================================================================

AEESoundCtlAngledMixing

Description:
AEESoundCtlAngledMixing is used to control the pseudo-positioning(angle) 
effect of sound sources.

typedef struct AEESoundCtlAngledMixing
{
  uint8           nClass;
  uint16          nGain;
  uint16          nAngle;
  uint16          nTime;
  uint32          dwStructSize;
}AEESoundCtlAngledMixing;


Members:
nClass : The Class for which the parameters are applicable. There are two classes
         one for Voice calls and the other Audio playback including Music playback, 
         alerts, keybeeps, etc.
nGain : Gain when Audio source is in the background, 
        range from 0 to 0x7FFF. 
nAngle : Azimuth angle in degrees for audio source to move to Front,
         range from 0 to 0x167. 
nTime : Time duration in milliseconds for audio source to move from current angle 
        to target angle. 
        range from 0 to 0x7FFF.
dwStructSize : Size of structure 

Comments:
None

See Also:
AEE_SOUNDCTL_CLASS_VOCODER
AEE_SOUNDCTL_CLASS_AUDIO

=============================================================================

AEESoundCtlNotify

Description:
AEESoundCtlNotify contains information of an event generated by ISoundCtl object. 
It is sent via the registered callback function.

typedef struct AEESoundCtlNotify
{
   ISoundCtl*     pSoundCtl;
   int16          nCmd;
   int16          nStatus;
   void*          pData;
   uint32         dwDataSize;
} AEESoundCtlNotify;

Members:
pCam : ISoundCtl object originating this callback
nCmd : Command code. SOUNDCTL_CMD_XXX
nStatus : Status code. AEE_SOUNDCTL_XXX
pData : Context-based data
dwDataSize : Context-based data size

Comments:
None

See Also:
ISOUNDCTL_RegisterNotify

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

ISoundCtl Interface

Description:
   ISoundCtl interface provides services to control sound device. The controlled 
   features include equalizer, spectrum analyzer, audio expander and angled mixing.
   Functions in the ISoundCtl include:

   ISOUNDCTL_AddRef()~
   ISOUNDCTL_Release()~
   ISOUNDCTL_QueryInterface()~
   ISOUNDCTL_RegisterNotify()~
   ISOUNDCTL_GetParm()~
   ISOUNDCTL_EnableEqualizer()~
   ISOUNDCTL_SetEqualizer()~
   ISOUNDCTL_SetEqualizerGain()~
   ISOUNDCTL_EnableAudioExpander()~
   ISOUNDCTL_SetAudioExpander()~
   ISOUNDCTL_EnableAngledMixing()~
   ISOUNDCTL_SetAngledMixing()~
   ISOUNDCTL_EnableSpectrumAnalyzer()~
   ISOUNDCTL_SetSpectrumAnalyzer()

======================================================================= 

ISOUNDCTL_AddRef()

Description:
This function increase the reference count of the ISoundCtl Interface object. This
allows the object to be shared by multiple callers. The object is freed when the
reference count reaches 0 (zero). ISOUNDCTL_Release() .

Prototype:
uint32 ISOUNDCTL_AddRef(ISoundCtl * pISoundCtl)

Parameters:

pISoundCtl : Pointer to the ISoundCtl Interface object

Return Value:
Returns the incremented reference count for the object. A valid object returns a
positive reference count.

Comments:
None

Side Effects:
None

See Also:
ISOUNDCTL_Release()

=============================================================================

ISOUNDCTL_Release()

Description:
This function decrements the reference count of an object. The object is freed from
memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
uint32 ISOUNDCTL_Release(ISoundCtl * pISoundCtl)

Parameters:

pISoundCtl : Pointer to the ISoundCtl Interface object

Return Value:
Reference count : Decremented reference count for the object
0 (zero) : If the object has been freed and is no longer valid


Comments:
None

Side Effects:
None


See Also:
ISOUNDCTL_AddRef()

=============================================================================

Function:  ISOUNDCTL_QueryInterface()

Description:
   This function can be used to 
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the ISOUNDCTL-derived class
   (3) support version compatibility

Prototype:
   int ISOUNDCTL_QueryInterface (ISoundCtl * pISoundCtl, AEECLSID clsReq,
                                  void ** ppo)

Parameters:
   po [in]: Pointer to ISoundCtl interface.

   clsReq [in]: A globally unique id to identify the entity (interface or
                data) that we are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.
             If the value passed back is NULL, the interface or data that
             we query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects: 
   If an interface is retrieved, then this function increments its reference
   count.  If a data structure is retrieved, then a pointer to the internal
   structure is given and user should not free it.

See Also: 
None.
   
=============================================================================   
   
ISOUNDCTL_RegisterNotify()

Description:
This function registers the callback function.

Prototype:
void ISOUNDCTL_RegisterNotify(ISoundCtl * pISoundCtl, PFNSOUNDCTLNOTIFY pfn, const void * pUser)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
pfn : Status callback function pointer (can be NULL, if no callback is required)
pUser : User data for unique correlation/identification of the transaction. This
piece of information is not retrieved or processed by ISoundCtl and can be NULL if
no identifying data is required. This same data pointer is passed back to the client
along with callback status to correlate the transactions.

Return Value:
SUCCESS, if successful
EFAILED: Operation failed


Comments:
None

Side Effects:
None

See Also:
None

=============================================================================

ISOUNDCTL_GetParm()

Description:
This function gets the parameters from the sound device

Prototype:
int ISOUNDCTL_GetParm(ISoundCtl * pISoundCtl, int nParamID, int32 * pP1, int32 * pP2)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
nParamID : AEE_SOUNDCTL_PARM_XXX
pP1 : Depends on nParamID
pP2 : Depends on nParamID

Return Value:
AEE_SOUNDCTL_SUCCESS, if successful
AEE_SOUNDCTL_UNSUPPORTED, Parm not supported by the class
AEE_SOUNDCTL_FAILURE, FEATURE is not available

Comments:
No status returned via registered callback.

Following table gives the list of ISoundCtl supported get parameters:
===pre>

nParmID                                  Desc                       pP1                              pP2
------------------------------------------------------------------------------------------------------------------------------
AEE_SOUNDCTL_PARM_EQUALIZER_NUM_BAND_MAX max. equalizer bands       max number of bands can be set
AEE_SOUNDCTL_PARM_SPECTRUM_DATA_PERIOD   spectrum data period       min. period can be requested     max. period can be requested
AEE_SOUNDCTL_PARM_SPECTRUM_DATA_MAX      max. value spectrum data   value         

===/pre>

Side Effects:
None

See Also:
ISOUNDCTL_SetSpectrumAnalyzer()

=============================================================================

ISOUNDCTL_EnableEqualizer()

Description:
This function enables or disables equalizer.

Prototype:
void ISOUNDCTL_EnableEqualizer(ISoundCtl * pISoundCtl, boolean bEnable)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
bEnable : TRUE for enable, FALSE for disable

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
None

Side Effects:
callback returns AEE_SOUNDCTL_CMD_ENABLE_DISABLE_EQUALIZER as command code

See Also:
ISOUNDCTL_SetEqualizer()
ISOUNDCTL_SetEqualizerGain()

=============================================================================

ISOUNDCTL_SetEqualizer()

Description:
This function sets parameters for equalizer. 

Prototype:
void ISOUNDCTL_SetEqualizer(ISoundCtl * pISoundCtl, AEESoundCtlEqualizerFilter* eqtype, uint32 bands)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
eqtype : Equalizer filter type
band : number of bands

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
This function does not enable Equalizer, ISOUNDCTL_EnableEqualizer must be called to 
enable the equalizer. For setting multiple filters, the first one should be a bass 
boost filter and the last one should be a treble boost filter. All of the filters in 
between the first and the last filters must use band boost/cut filters

Side Effects:
callback returns AEE_SOUNDCTL_CMD_SET_EQUALIZER as command code

See Also:
AEESoundCtlEqualizer
ISOUNDCTL_EnableEqualizer()
   
=============================================================================

ISOUNDCTL_SetEqualizerGain()

Description:
This function adjusts the gain value on a single band.

Prototype:
void ISOUNDCTL_SetEqualizerGain(ISoundCtl* pISoundCtl, uint16 band, int16 nGain)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
band : Zero-based numerical index of the band for which the gain must be changed 
nGain : New gain value for this band 

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
This function does not enable Equalizer, ISOUNDCTL_EnableEqualizer must be called to 
enable the equalizer.

Side Effects:
callback returns AEE_SOUNDCTL_CMD_SET_EQUALIZER_GAIN as command code

See Also:
AEESoundCtlNotify
ISOUNDCTL_EnableEqualizer()

=============================================================================

ISOUNDCTL_EnableAudioExpander()

Description:
This function enables or disables the audio expander.

Prototype:
void ISOUNDCTL_EnableAudioExpander(ISoundCtl* pISoundCtl, boolean bEnable)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
bEnable : TRUE for enable, FALSE for disable

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
None

Side Effects:
callback returns AEE_SOUNDCTL_CMD_ENABLE_DISABLE_AUDIO_EXPANDER as command code

See Also:
AEESoundCtlNotify
ISOUNDCTL_SetAudioExpander()

=============================================================================

ISOUNDCTL_SetAudioExpander()

Description:
This function sets parameters for audio expander.

Prototype:
void ISOUNDCTL_SetAudioExpander(ISoundCtl* po, AEESoundCtlAudioExpander* aepType)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
aepType : Audio expander type

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
This function does not enable AudioExpander, ISOUNDCTL_EnableAudioExpander must
be called to enable the AudioExpander.

Side Effects:
callback returns AEE_SOUNDCTL_CMD_SET_AUDIO_EXPANDER as command code

See Also:
AEESoundCtlNotify
AEESoundCtlAudioExpander
ISOUNDCTL_EnableAudioExpander()


=============================================================================

ISOUNDCTL_EnableAngledMixing()

Description:
This function enables or disables the audio AngledMixing.

Prototype:
void ISOUNDCTL_EnableAngledMixing(ISoundCtl* pISoundCtl, boolean bEnable)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
bEnable : TRUE for enable, FALSE for disable

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found or feature not supported 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
None

Side Effects:
callback returns AEE_SOUNDCTL_CMD_ENABLE_DISABLE_ANGLEDMIXING as command code

See Also:
AEESoundCtlNotify
ISOUNDCTL_SetAngledMixing()

=============================================================================

ISOUNDCTL_SetAngledMixing()

Description:
This function sets parameters for audio AngledMixing.

Prototype:
void ISOUNDCTL_SetAngledMixing(ISoundCtl* po, AEESoundCtlAngledMixing* aepType, uint16 nParam)

Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
aepType : Array of AngledMixing parameters for different classes of Audio source
nParam : Number of AngledMixing parameters being set

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found or feature not supported
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
This function does not enable AngledMixing, ISOUNDCTL_EnableAngledMixing must
be called to enable the AngledMixing. It is recommended that the ISOUNDCTL_SetAngledMixing() 
is called at least once before ISOUNDCTL_EnableAngledMixing() is called.

Side Effects:
callback returns AEE_SOUNDCTL_CMD_SET_ANGLEDMIXING as command code

See Also:
AEESoundCtlNotify
AEESoundCtlAngledMixing
ISOUNDCTL_EnableAngledMixing()

=============================================================================

ISOUNDCTL_EnableSpectrumAnalyzer()

Description:
This function enables or disables the spectrum analyzer.

Prototype:
void ISOUNDCTL_EnableSpectrumAnalyzer(ISoundCtl* pISoundCtl, boolean bEnable)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
bEnable : TRUE for enable, FALSE for disable

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
None

Side Effects:
callback returns AEE_SOUNDCTL_CMD_ENABLE_DISABLE_SPECTRUM_ANALYZER as command code

See Also:
AEESoundCtlNotify
ISOUNDCTL_SetSpectrumAnalyzer()

=============================================================================

ISOUNDCTL_SetSpectrumAnalyzer()

Description:
This function sets the callback interval period for spectrum analyzer

Prototype:
void ISOUNDCTL_SetSpectrumAnalyzer(ISoundCtl* ISoundCtl, uint16 update_period)


Parameters:

pISoundCtl : Pointer to ISoundCtl Interface object
update_period : Interval to receive spectrum analyzer data based on 10ms.  

Return Value:
AEE_SOUNDCTL_SUCCESS: Command accepted 
AEE_SOUNDCTL_UNSUPPORTED: Class not found 
AEE_SOUNDCTL_FAILURE: Operation failed

Comments:
update_period should be within the allowed spectrum data period. Use 
ISOUNDCTL_GetParm() to retrieve min and max value.

Side Effects:
Spectrum Analyzer will be enabled by calling this function, 
callback returns AEE_SOUNDCTL_CMD_SET_SPECTRUM_ANALYZER as command code

See Also:
AEESoundCtlNotify
ISOUNDCTL_EnableSpectrumAnalyzer()
ISOUNDCTL_GetParm()

==============================================================================*/

#endif // AEESOUNDCTL_H


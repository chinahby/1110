#ifndef AEESOUNDPLAYER_H
#define AEESOUNDPLAYER_H
/*============================================================================
FILE: AEESoundPlayer.h

SERVICES:  Sound Playing Services

DESCRIPTION:
   This file provides definitions for the MP3 and MIDI interfaces
   available to application developers.  This header file must be included
   by all MP3 and MIDI applications.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

#define AEE_ID3_TAG_LENGTH        30   // ID3 tag standard

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEESound.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// Status type is returned in callback functions to indicate events
// and return data to client.
enum {
   AEE_SOUNDPLAYER_UNKNOWN,            // Unknown status
   AEE_SOUNDPLAYER_SUCCESS,            // Request is accepted
   AEE_SOUNDPLAYER_REWIND,             // Playback is currently rewinding
   AEE_SOUNDPLAYER_FFORWARD,           // Playback is currently fast forwarding
   AEE_SOUNDPLAYER_PAUSE,              // Playback is currently paused
   AEE_SOUNDPLAYER_RESUME,             // Playback has resumed
   AEE_SOUNDPLAYER_TEMPO,              // Playback tempo changed
   AEE_SOUNDPLAYER_TUNE,               // Playback tune changed
   AEE_SOUNDPLAYER_PAN,                // Playback stereo pan changed
   AEE_SOUNDPLAYER_AUDIO_SPEC,         // Audio spec information
   AEE_SOUNDPLAYER_TICK_UPDATE,        // One second update during playback
   AEE_SOUNDPLAYER_DATA_ACCESS_DELAY,  // Playback is being delayed by data access
   AEE_SOUNDPLAYER_ABORTED,            // Command was aborted
   AEE_SOUNDPLAYER_REPEAT,             // Sound repeating
   AEE_SOUNDPLAYER_DONE,               // Command has been carried out
   AEE_SOUNDPLAYER_FAILURE,            // Error occured with this request
   AEE_SOUNDPLAYER_LAST
};
#ifdef WIN32
typedef unsigned AEESoundPlayerStatus;
#else
typedef int8 AEESoundPlayerStatus;
#endif

// Enumerated type to indicate the source where ISoundPlayer will find the
// data or file to be played.
enum {
   SDT_NONE,
   SDT_FILE,         // specified data is file name
   SDT_BUFFER,       // specified data is raw buffer
   SDT_VOICEPROMPT
};
#ifdef WIN32
typedef unsigned AEESoundPlayerInput;
#else
typedef int8 AEESoundPlayerInput;
#endif

enum {
   AEE_SOUNDPLAYER_SOURCE_UNKNOWN,  // An invalid type
   AEE_SOUNDPLAYER_SOURCE_MEM,      // Designates that data is found in memory
   AEE_SOUNDPLAYER_SOURCE_FILE,     // Designates that data is found in the file
                                    // Embedded File System (EFS)
   AEE_SOUNDPLAYER_SOURCE_LAST
};
#ifdef WIN32
typedef unsigned AEESoundPlayerSource;
#else
typedef int8 AEESoundPlayerSource;
#endif

/* This is an enumerated type that describes the format of the audio file */
enum {
   AEE_SOUNDPLAYER_FILE_UNKNOWN, // Unknown format
   AEE_SOUNDPLAYER_FILE_MIDI,    // MIDI
   AEE_SOUNDPLAYER_FILE_MP3,     // MP3
   AEE_SOUNDPLAYER_FILE_QCP,     // QCELP
   AEE_SOUNDPLAYER_FILE_MMF,     // MMF
   AEE_SOUNDPLAYER_FILE_PHR,     // Phrase
   AEE_SOUNDPLAYER_FILE_IMY,     // IMelody
   AEE_SOUNDPLAYER_FILE_ADPCM,   // ADPCM Wav
   AEE_SOUNDPLAYER_FILE_PCM,     // Linear Wav
   AEE_SOUNDPLAYER_FILE_AAC,     // AAC
   AEE_SOUNDPLAYER_FILE_AMR,     // AMR
   AEE_SOUNDPLAYER_FILE_PMD,     // PMD(CMX)
   AEE_SOUNDPLAYER_FILE_MPEG4,   // MPEG4
   AEE_SOUNDPLAYER_FILE_3GP,     // 3GP
   AEE_SOUNDPLAYER_FILE_3G2,     // 3G2
   AEE_SOUNDPLAYER_FILE_WMA,     // WMA
   AEE_SOUNDPLAYER_FILE_WMV,     // WMV
   AEE_SOUNDPLAYER_FILE_ASF,     // WMV
   AEE_SOUNDPLAYER_FILE_RA,      // RA
   AEE_SOUNDPLAYER_FILE_RM,      // RM
   AEE_SOUNDPLAYER_FILE_MOV,     // MOV
   AEE_SOUNDPLAYER_FILE_QT,      // QT
   AEE_SOUNDPLAYER_FILE_AVI,     // AVI
   AEE_SOUNDPLAYER_FILE_AIFF,    // AIFF
   AEE_SOUNDPLAYER_FILE_LAST  = 0x7F
};
#ifdef WIN32
typedef unsigned AEESoundPlayerFile;
#else
typedef int8 AEESoundPlayerFile;
#endif

// MIDI file specification
typedef struct
{
   AEESoundPlayerFile     fType;  // Format of audio file
} AEESoundPlayerMIDISpec;

// MPEG version enum type
enum {
   AEE_SOUNDPLAYER_MP3_VERSION_25,      // MPEG version 2.5
   AEE_SOUNDPLAYER_MP3_VERSION_RESERVED,// Reserved
   AEE_SOUNDPLAYER_MP3_VERSION_2,       // MPEG version 2.0
   AEE_SOUNDPLAYER_MP3_VERSION_1,       // MPEG version 1.0
   AEE_SOUNDPLAYER_MP3_VERSION_UNKNOWN  // Unable to determine version
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3Version;
#else
typedef int8 AEESoundPlayerMP3Version;
#endif

// MPEG layer info
enum {
   AEE_SOUNDPLAYER_LAYER_RESERVED, // Reserved
   AEE_SOUNDPLAYER_LAYER_3,        // MPEG layer 3 compression
   AEE_SOUNDPLAYER_LAYER_2,        // MPEG layer 2 compression
   AEE_SOUNDPLAYER_LAYER_1,        // MPEG layer 1 compression
   AEE_SOUNDPLAYER_LAYER_UNKNOWN   // Unable to determine layer information
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3Layer;
#else
typedef int8 AEESoundPlayerMP3Layer;
#endif

// MP3 bitrate enum type
enum {
   // Free bitrate (determined by software)
   AEE_SOUNDPLAYER_MP3_BITRATE_FREE = 0,

    // Fixed bitrates
   AEE_SOUNDPLAYER_MP3_BITRATE_8K   = 8,
   AEE_SOUNDPLAYER_MP3_BITRATE_16K  = 16,
   AEE_SOUNDPLAYER_MP3_BITRATE_24K  = 24,
   AEE_SOUNDPLAYER_MP3_BITRATE_32K  = 32,
   AEE_SOUNDPLAYER_MP3_BITRATE_40K  = 40,
   AEE_SOUNDPLAYER_MP3_BITRATE_48K  = 48,
   AEE_SOUNDPLAYER_MP3_BITRATE_56K  = 56,
   AEE_SOUNDPLAYER_MP3_BITRATE_64K  = 64,
   AEE_SOUNDPLAYER_MP3_BITRATE_80K  = 80,
   AEE_SOUNDPLAYER_MP3_BITRATE_96K  = 96,
   AEE_SOUNDPLAYER_MP3_BITRATE_112K = 112,
   AEE_SOUNDPLAYER_MP3_BITRATE_128K = 128,
   AEE_SOUNDPLAYER_MP3_BITRATE_144K = 144,
   AEE_SOUNDPLAYER_MP3_BITRATE_160K = 160,
   AEE_SOUNDPLAYER_MP3_BITRATE_176K = 176,
   AEE_SOUNDPLAYER_MP3_BITRATE_192K = 192,
   AEE_SOUNDPLAYER_MP3_BITRATE_224K = 224,
   AEE_SOUNDPLAYER_MP3_BITRATE_256K = 256,
   AEE_SOUNDPLAYER_MP3_BITRATE_288K = 288,
   AEE_SOUNDPLAYER_MP3_BITRATE_320K = 320,
   AEE_SOUNDPLAYER_MP3_BITRATE_352K = 352,
   AEE_SOUNDPLAYER_MP3_BITRATE_384K = 384,
   AEE_SOUNDPLAYER_MP3_BITRATE_416K = 416,
   AEE_SOUNDPLAYER_MP3_BITRATE_448K = 448,

   // Variable bitrate (Changes each frame)
   AEE_SOUNDPLAYER_MP3_BITRATE_VAR  = 500,

   // Unable to determine bitrate information
   AEE_SOUNDPLAYER_MP3_BITRATE_UNK  = 501
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3BitRate;
#else
typedef int16 AEESoundPlayerMP3BitRate;
#endif

// MP3 Sample rate enum type
enum {
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_UNKNOWN,  // Unknown rate
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_8000 = 1, // 8k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_11025,    // 11.025k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_12000,    // 12k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_16000,    // 16k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_22050,    // 22.050k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_24000,    // 24k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_32000,    // 32k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_44100,    // 44.1k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_48000,    // 48k
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_LAST
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3SampleRate;
#else
typedef int8 AEESoundPlayerMP3SampleRate;
#endif

// MP3 channel
enum {
   AEE_SOUNDPLAYER_MP3_CHANNEL_STEREO,       // Stereo data
   AEE_SOUNDPLAYER_MP3_CHANNEL_JOINT_STEREO, // Joint Stereo data
   AEE_SOUNDPLAYER_MP3_CHANNEL_DUAL,         // Dual channel (stereo) data
   AEE_SOUNDPLAYER_MP3_CHANNEL_SINGLE        // Single channel (mono) data
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3Channel;
#else
typedef int8 AEESoundPlayerMP3Channel;
#endif

// MP3 extension
enum {
   // For Layer 1 and 2 files
   AEE_SOUNDPLAYER_MP3_EXT_BAND_4_31  = 0, // Channel extension info, 4-31
   AEE_SOUNDPLAYER_MP3_EXT_BAND_8_31  = 1, // Channel extension info, 8-31
   AEE_SOUNDPLAYER_MP3_EXT_BAND_12_31 = 2, // Channel extension info, 12-31
   AEE_SOUNDPLAYER_MP3_EXT_BAND_16_31 = 3, // Channel extension info, 16-31

   // For Layer 3 files
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_OFF = 0, // Intensity stereo off, MS off
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_OFF  = 1, // Intensify stereo on, MS off
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_ON  = 2, // Intensity stereo off, MS on
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_ON   = 3  // Intensity stereo on, MS on
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3Extension;
#else
typedef int8 AEESoundPlayerMP3Extension;
#endif

// MP3 emphasis
enum {
   AEE_SOUNDPLAYER_MP3_EMPHASIS_NONE,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_50_15_MS,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_RESERVED,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_CCITT_J17
};
#ifdef WIN32
typedef unsigned AEESoundPlayerMP3Emphasis;
#else
typedef int8 AEESoundPlayerMP3Emphasis;
#endif

// MP3 spec
typedef struct
{
   // More fields can be added here, such as copyright,
   AEESoundPlayerFile            fType;        // Format of audio file
   AEESoundPlayerMP3Version      version;      // MPEG version
   AEESoundPlayerMP3Layer        layer;        // MPEG layer description
   boolean                       crcFlag;      // True if CRC protection
   AEESoundPlayerMP3BitRate      bitrate;      // Bit rate
   AEESoundPlayerMP3SampleRate   sampleRate;   // Sampling rate
   AEESoundPlayerMP3Channel      channel;      // Channel mode
   AEESoundPlayerMP3Extension    extension;    // Only when JOINT_STEREO
   boolean                       copyrightFlag;// True if copyrighted
   boolean                       originalFlag; // True if original
   AEESoundPlayerMP3Emphasis     emphasis;     // Audio emphasis
   char                          title[AEE_ID3_TAG_LENGTH];    // Song title
   char                          artist[AEE_ID3_TAG_LENGTH];   // Song performer
   char                          album[AEE_ID3_TAG_LENGTH];    // Album with the song
   char                          year[4];                      // Year Album released
   char                          comment[AEE_ID3_TAG_LENGTH];  // Text comment
   uint8                         genre;                        // ID3 genre tag
} AEESoundPlayerMP3Spec;

// AEE_SOUNDPLYER_AUDIO_SPEC play callback data
typedef union
{
   AEESoundPlayerFile      fType;
   AEESoundPlayerMIDISpec  MIDISpec;
   AEESoundPlayerMP3Spec   MP3Spec;
} AEESoundPlayerAudioSpec;

enum {
   AEE_SOUNDPLAYER_STATUS_CB, // for SoundPlayer status
   AEE_SOUNDPLAYER_PLAY_CB,   // for Play status
   AEE_SOUNDPLAYER_TIME_CB,   // for get time status
   AEE_SOUNDPLAYER_SOUND_CB,  // for sound status
   AEE_SOUNDPLAYER_VOLUME_CB  // for get volume status
};
#ifdef WIN32
typedef unsigned AEESoundPlayerCmd;
#else
typedef int8 AEESoundPlayerCmd;
#endif

// This type is a union of all the structures/values that are returned via the
// audio format play callback function.
typedef union
{
   uint32                  dwElapsedTime;
   uint32                  dwTotalTime;
   uint32                  dwTempo;
   uint32                  dwTune;
   uint32                  dwPan;
   uint16                  wVolume;
   AEESoundPlayerAudioSpec spSpec;
} AEESoundPlayerCmdData;

typedef struct
{
   byte *                pbData;
   uint16                wNumFrames;
} AEEVoicePrompt;

typedef struct
{
   AEESoundPlayerInput  eInput;
   void *               pData;
   uint32               dwSize;
} AEESoundPlayerInfo;

typedef void (*PFNSOUNDPLAYERSTATUS)(void * pUser, AEESoundPlayerCmd  eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam);

// Define interface pointers
typedef struct _ISoundPlayer  ISoundPlayer;

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
// ISoundPlayer Interface
//
//********************************************************************************************************************************

QINTERFACE(ISoundPlayer)
{
   DECLARE_IBASE(ISoundPlayer)

   void     (*RegisterNotify)(ISoundPlayer * po,PFNSOUNDPLAYERSTATUS pfn, void * pUser);
   void     (*Set)(ISoundPlayer * po, AEESoundPlayerInput t, void * pData);
   void     (*Play)(ISoundPlayer * po);
   void     (*Stop)(ISoundPlayer * po);
   void     (*Rewind)(ISoundPlayer * po,uint32 dwTime);
   void     (*FastForward)(ISoundPlayer * po,uint32 dwTime);
   void     (*Pause)(ISoundPlayer * po);
   void     (*Resume)(ISoundPlayer * po);
   void     (*SetTempo)(ISoundPlayer * po,uint32 dwTempoFactor);
   void     (*SetTune)(ISoundPlayer * po,int8 nStep);
   void     (*SetVolume)(ISoundPlayer * po, uint16 wVolume);
   void     (*GetVolume)(ISoundPlayer * po);
   void     (*GetTotalTime)(ISoundPlayer * po);
   void     (*SetSoundDevice)(ISoundPlayer * po,AEESoundDevice eDevice, AEESoundMuteCtl eEarMute, AEESoundMuteCtl eMicMute );
   void     (*SetStream)(ISoundPlayer * po, IAStream * ps);
// BREW 1.1.x.x API:
   int      (*SetInfo)(ISoundPlayer * po, AEESoundPlayerInfo * pInfo);
   int      (*GetInfo)(ISoundPlayer * po, AEESoundPlayerInfo * pInfo);
};

#define ISOUNDPLAYER_AddRef(p)                     GET_PVTBL(p, ISoundPlayer)->AddRef(p)
#define ISOUNDPLAYER_Release(p)                    GET_PVTBL(p, ISoundPlayer)->Release(p)
#define ISOUNDPLAYER_RegisterNotify(p, f, pd)      GET_PVTBL(p, ISoundPlayer)->RegisterNotify(p, f, pd)
#define ISOUNDPLAYER_Set(p, t, pd)                 GET_PVTBL(p, ISoundPlayer)->Set(p, t, pd)
#define ISOUNDPLAYER_Play(p)                       GET_PVTBL(p, ISoundPlayer)->Play(p)
#define ISOUNDPLAYER_Stop(p)                       GET_PVTBL(p, ISoundPlayer)->Stop(p)
#define ISOUNDPLAYER_Rewind(p, t)                  GET_PVTBL(p, ISoundPlayer)->Rewind(p, t)
#define ISOUNDPLAYER_FastForward(p, t)             GET_PVTBL(p, ISoundPlayer)->FastForward(p, t)
#define ISOUNDPLAYER_Pause(p)                      GET_PVTBL(p, ISoundPlayer)->Pause(p)
#define ISOUNDPLAYER_Resume(p)                     GET_PVTBL(p, ISoundPlayer)->Resume(p)
#define ISOUNDPLAYER_SetTempo(p, f)                GET_PVTBL(p, ISoundPlayer)->SetTempo(p, f)
#define ISOUNDPLAYER_SetTune(p, s)                 GET_PVTBL(p, ISoundPlayer)->SetTune(p, s)
#define ISOUNDPLAYER_GetTotalTime(p)               GET_PVTBL(p, ISoundPlayer)->GetTotalTime(p)

#define ISOUNDPLAYER_SetSoundDevice(p, d, e, m)    GET_PVTBL(p, ISoundPlayer)->SetSoundDevice(p, d, e, m)
#define ISOUNDPLAYER_SetVolume(p, v)               GET_PVTBL(p, ISoundPlayer)->SetVolume(p, v)
#define ISOUNDPLAYER_GetVolume(p)                  GET_PVTBL(p, ISoundPlayer)->GetVolume(p)

#define ISOUNDPLAYER_SetStream(p,ps)               GET_PVTBL(p, ISoundPlayer)->SetStream(p,ps)

#define ISOUNDPLAYER_SetInfo(p, pi)                GET_PVTBL(p, ISoundPlayer)->SetInfo(p, pi)
#define ISOUNDPLAYER_GetInfo(p, pi)                GET_PVTBL(p, ISoundPlayer)->GetInfo(p, pi)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEESoundPlayerAudioSpec

Description:
AEESoundPlayerAudioSpec indicates the audio specifications. It is used with the
AEESOUNDPLAYER_AUDIO_SPEC playback callback.

Definition:
typedef union
{
   AEESoundPlayerFile fType;
   AEESoundPlayerMIDISpec MIDISpec;
   AEESoundPlayerMP3Spec MP3Spec;
} AEESoundPlayerAudioSpec;

Members:
fType : Audio File Type
MIDISpec : MIDI specifications if file type is MIDI
MP3Spec : MP3 specifications if file type is MP3

Comments:
None

See Also:
AEESoundPlayerFile ,
AEESoundPlayerMIDISpec ,
AEESoundPlayerMP3Spec

=============================================================================

AEESoundPlayerCmd

Description:
AEESoundPlayerCmd specifies the callback type used by ISoundPlayer to send events
and data to application.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_STATUS_CB,
   AEE_SOUNDPLAYER_PLAY_CB,
   AEE_SOUNDPLAYER_TIME_CB,
   AEE_SOUNDPLAYER_SOUND_CB,
   AEE_SOUNDPLAYER_VOLUME_CB
} AEESoundPlayerCmd;

Members:

AEE_SOUNDPLAYER_STATUS_CB : SoundPlayer status callback
AEE_SOUNDPLAYER_PLAY_CB : Playback callback
AEE_SOUNDPLAYER_TIME_CB : Get time callback
AEE_SOUNDPLAYER_SOUND_CB : Sound callback
AEE_SOUNDPLAYER_VOLUME_CB : Get volume callback


Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerCmdData

Description:
AEESoundPlayerCmdData specifies the data sent through callback to application.

Definition:
typedef union
{
   uint32 dwElapsedTime;
   uint32 dwTotalTime;
   uint32 dwTempo;
   uint32 dwTune;
   uint32 dwPan;
   uint16 wVolume;
   AEESoundPlayerAudioSpec spSpec;
} AEESoundPlayerCmdData;

Members:

dwElapsedTime : Elapsed time sent through playback callback for Pause, Resume, Rewind,
                FastForward
dwTotalTime : Total Time sent through get time callback
dwTempo : Tempo factor sent through playback callback for SetTempo
dwTune : Tune factor sent through playback callback for SetTune
dwPan : Not used
wVolume : Indicates volume of the device, method pair. It ranges from 0 (zero) to
          AEE_MAX_VOLUME
spSpec : Audio specs sent through playback callback for Play

Comments:
None

See Also:
AEESoundPlayerAudioSpec

=============================================================================

AEESoundPlayerFile

Description:
AEESoundPlayerFile indicates the type of file being played.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_FILE_UNKNOWN,
   AEE_SOUNDPLAYER_FILE_MIDI,
   AEE_SOUNDPLAYER_FILE_MP3,
   AEE_SOUNDPLAYER_FILE_QCP,
   AEE_SOUNDPLAYER_FILE_MMF,
   AEE_SOUNDPLAYER_FILE_PHR,
   AEE_SOUNDPLAYER_FILE_IMY,
   AEE_SOUNDPLAYER_FILE_ADPCM,
   AEE_SOUNDPLAYER_FILE_PCM,
   AEE_SOUNDPLAYER_FILE_AAC,
   AEE_SOUNDPLAYER_FILE_AMR,
   AEE_SOUNDPLAYER_FILE_PMD,
   AEE_SOUNDPLAYER_FILE_MPEG4,
   AEE_SOUNDPLAYER_FILE_3GP,
   AEE_SOUNDPLAYER_FILE_3G2,
   AEE_SOUNDPLAYER_FILE_WMA,
   AEE_SOUNDPLAYER_FILE_WMV,
   AEE_SOUNDPLAYER_FILE_ASF,
   AEE_SOUNDPLAYER_FILE_RA,
   AEE_SOUNDPLAYER_FILE_RM,
   AEE_SOUNDPLAYER_FILE_MOV,
   AEE_SOUNDPLAYER_FILE_QT,
   AEE_SOUNDPLAYER_FILE_AVI,
   AEE_SOUNDPLAYER_FILE_AIFF,
   AEE_SOUNDPLAYER_LAST = 0x7F
} AEESoundPlayerFile;

Members:
AEE_SOUNDPLAYER_FILE_UNKNOWN:    Unknown format
AEE_SOUNDPLAYER_FILE_MIDI:       MIDI
AEE_SOUNDPLAYER_FILE_MP3:        MP3
AEE_SOUNDPLAYER_FILE_QCP:        QCELP
AEE_SOUNDPLAYER_FILE_MMF:        MMF
AEE_SOUNDPLAYER_FILE_PHR:        Phrase
AEE_SOUNDPLAYER_FILE_IMY:        IMelody
AEE_SOUNDPLAYER_FILE_ADPCM:      ADPCM Wav
AEE_SOUNDPLAYER_FILE_PCM:        Linear Wav
AEE_SOUNDPLAYER_FILE_AAC:        AAC
AEE_SOUNDPLAYER_FILE_AMR:        AMR
AEE_SOUNDPLAYER_FILE_PMD:        PMD(CMX)
AEE_SOUNDPLAYER_FILE_MPEG4:      MPEG4
AEE_SOUNDPLAYER_FILE_3GP:        3GP
AEE_SOUNDPLAYER_FILE_3G2:        3G2
AEE_SOUNDPLAYER_FILE_WMA:        WMA
AEE_SOUNDPLAYER_FILE_WMV:        WMV
AEE_SOUNDPLAYER_FILE_ASF:        WMV
AEE_SOUNDPLAYER_FILE_RA:         RA
AEE_SOUNDPLAYER_FILE_RM:         RM
AEE_SOUNDPLAYER_FILE_MOV:        MOV
AEE_SOUNDPLAYER_FILE_QT:         QT
AEE_SOUNDPLAYER_FILE_AVI:        AVI
AEE_SOUNDPLAYER_FILE_AIFF:       AIFF
AEE_SOUNDPLAYER_LAST:            Reserved
Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerInput

Description:
AEESoundPlayerInput indicates the source where ISoundPlayer finds the data or file
to be played.

Definition:
typedef enum {
   SDT_NONE,
   SDT_FILE,
   SDT_BUFFER,
   SDT_VOICEPROMPT
} AEESoundPlayerInput;

Members:
SDT_NONE : Source unknown (reserved)
SDT_FILE : Specified data is file name
SDT_BUFFER : Specified data is raw buffer (reserved)
SDT_VOICEPROMPT : Not used

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMIDISpec

Description:
AEESoundPlayerMIDISpec indicates the audio specifications of the MIDI file type.
It is used with the AEESoundPlayerAudioSpec playback callback.

Definition:
typedef struct
{
   AEESoundPlayerFile fType;
} AEESoundPlayerMIDISpec;

Members:
fType : Format of audio file

Comments:
None

See Also:
AEESoundPlayerFile

=============================================================================

AEESoundPlayerMP3BitRate

Description:
AEESoundPlayerMP3BitRate specifies the MP3 bit rate.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_BITRATE_FREE = 0,
   AEE_SOUNDPLAYER_MP3_BITRATE_8K = 8,
   AEE_SOUNDPLAYER_MP3_BITRATE_16K = 16,
   AEE_SOUNDPLAYER_MP3_BITRATE_24K = 24,
   AEE_SOUNDPLAYER_MP3_BITRATE_32K = 32,
   AEE_SOUNDPLAYER_MP3_BITRATE_40K = 40,
   AEE_SOUNDPLAYER_MP3_BITRATE_48K = 48,
   AEE_SOUNDPLAYER_MP3_BITRATE_56K = 56,
   AEE_SOUNDPLAYER_MP3_BITRATE_64K = 64,
   AEE_SOUNDPLAYER_MP3_BITRATE_80K = 80,
   AEE_SOUNDPLAYER_MP3_BITRATE_96K = 96,
   AEE_SOUNDPLAYER_MP3_BITRATE_112K = 112,
   AEE_SOUNDPLAYER_MP3_BITRATE_128K = 128,
   AEE_SOUNDPLAYER_MP3_BITRATE_144K = 144,
   AEE_SOUNDPLAYER_MP3_BITRATE_160K = 160,
   AEE_SOUNDPLAYER_MP3_BITRATE_176K = 176,
   AEE_SOUNDPLAYER_MP3_BITRATE_192K = 192,
   AEE_SOUNDPLAYER_MP3_BITRATE_224K = 224,
   AEE_SOUNDPLAYER_MP3_BITRATE_256K = 256,
   AEE_SOUNDPLAYER_MP3_BITRATE_288K = 288,
   AEE_SOUNDPLAYER_MP3_BITRATE_320K = 320,
   AEE_SOUNDPLAYER_MP3_BITRATE_352K = 352,
   AEE_SOUNDPLAYER_MP3_BITRATE_384K = 384,
   AEE_SOUNDPLAYER_MP3_BITRATE_416K = 416,
   AEE_SOUNDPLAYER_MP3_BITRATE_448K = 448,
   AEE_SOUNDPLAYER_MP3_BITRATE_VAR = 500,
   AEE_SOUNDPLAYER_MP3_BITRATE_UNK = 501
} AEESoundPlayerMP3BitRate;

Members:
This is Free bit rate (determined by software):
AEE_SOUNDPLAYER_MP3_BITRATE_FREE

The following are Fixed bit rates:
AEE_SOUNDPLAYER_MP3_BITRATE_8K
AEE_SOUNDPLAYER_MP3_BITRATE_16K
AEE_SOUNDPLAYER_MP3_BITRATE_24K
AEE_SOUNDPLAYER_MP3_BITRATE_32K
AEE_SOUNDPLAYER_MP3_BITRATE_40K
AEE_SOUNDPLAYER_MP3_BITRATE_48K
AEE_SOUNDPLAYER_MP3_BITRATE_56K
AEE_SOUNDPLAYER_MP3_BITRATE_64K
AEE_SOUNDPLAYER_MP3_BITRATE_80K
AEE_SOUNDPLAYER_MP3_BITRATE_96K
AEE_SOUNDPLAYER_MP3_BITRATE_112K
AEE_SOUNDPLAYER_MP3_BITRATE_128K
AEE_SOUNDPLAYER_MP3_BITRATE_144K
AEE_SOUNDPLAYER_MP3_BITRATE_160K 
AEE_SOUNDPLAYER_MP3_BITRATE_176K
AEE_SOUNDPLAYER_MP3_BITRATE_192K
AEE_SOUNDPLAYER_MP3_BITRATE_224K
AEE_SOUNDPLAYER_MP3_BITRATE_256K
AEE_SOUNDPLAYER_MP3_BITRATE_288K
AEE_SOUNDPLAYER_MP3_BITRATE_320K
AEE_SOUNDPLAYER_MP3_BITRATE_352K
AEE_SOUNDPLAYER_MP3_BITRATE_384K
AEE_SOUNDPLAYER_MP3_BITRATE_416K
AEE_SOUNDPLAYER_MP3_BITRATE_448K

This is Variable bit rate (Changes each frame)
AEE_SOUNDPLAYER_MP3_BITRATE_VAR

Unable to determine bit-rate information
AEE_SOUNDPLAYER_MP3_BITRATE_UNK

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3Channel

Description:
AEESoundPlayerMP3Channel specifies the MP3 channel.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_CHANNEL_STEREO
   AEE_SOUNDPLAYER_MP3_CHANNEL_JOINT_STEREO
   AEE_SOUNDPLAYER_MP3_CHANNEL_DUAL
   AEE_SOUNDPLAYER_MP3_CHANNEL_SINGLE
} AEESoundPlayerMP3Channel;

Members:
AEE_SOUNDPLAYER_MP3_CHANNEL_STEREO : Stereo data
AEE_SOUNDPLAYER_MP3_CHANNEL_JOINT_STEREO : Joint Stereo data
AEE_SOUNDPLAYER_MP3_CHANNEL_DUAL : Dual channel (stereo) data
AEE_SOUNDPLAYER_MP3_CHANNEL_SINGLE : Single channel (mono) data

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3Emphasis

Description:
AEESoundPlayerMP3Emphasis specifies MP3 emphasis flag.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_EMPHASIS_NONE,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_50_15_MS,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_RESERVED,
   AEE_SOUNDPLAYER_MP3_EMPHASIS_CCITT_J17
} AEESoundPlayerMP3Emphasis;

Members:
AEE_SOUNDPLAYER_MP3_EMPHASIS_NONE : Invalid flag
AEE_SOUNDPLAYER_MP3_EMPHASIS_50_15_MS : 50_15_MS
AEE_SOUNDPLAYER_MP3_EMPHASIS_RESERVED : Reserved
AEE_SOUNDPLAYER_MP3_EMPHASIS_CCITT_J17 : CCITT J17

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3Extension

Description:
AEESoundPlayerMP3Extension specifies MP3 extension for layers 1-3.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_EXT_BAND_4_31 = 0,
   AEE_SOUNDPLAYER_MP3_EXT_BAND_8_31 = 1,
   AEE_SOUNDPLAYER_MP3_EXT_BAND_12_31 = 2,
   AEE_SOUNDPLAYER_MP3_EXT_BAND_16_31 = 3,
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_OFF = 0,
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_OFF = 1,
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_ON = 2,
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_ON = 3
} AEESoundPlayerMP3Extension;

Members:
   For Layer 1 and 2 files
   AEE_SOUNDPLAYER_MP3_EXT_BAND_4_31  :  Channel extension info, 4-31.
   AEE_SOUNDPLAYER_MP3_EXT_BAND_8_31  :  Channel extension info, 8-31.
   AEE_SOUNDPLAYER_MP3_EXT_BAND_12_31  :  Channel extension info, 12-31.
   AEE_SOUNDPLAYER_MP3_EXT_BAND_16_31  :  Channel extension info, 16-31.

   For Layer 3 files
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_OFF :  Intensity stereo off, MS off.
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_OFF  :  Intensify stereo on, MS off.
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_OFF_MS_ON  :  Intensity stereo off, MS on.
   AEE_SOUNDPLAYER_MP3_EXT_INTENSITY_ON_MS_ON  :  Intensity stereo on, MS on.

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3Layer

Description:
AEESoundPlayerMP3Layer specifies the MPEG layer information.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_LAYER_RESERVED,
   AEE_SOUNDPLAYER_LAYER_3,
   AEE_SOUNDPLAYER_LAYER_2,
   AEE_SOUNDPLAYER_LAYER_1,
   AEE_SOUNDPLAYER_LAYER_UNKNOWN
} AEESoundPlayerMP3Layer;

Members:

AEE_SOUNDPLAYER_LAYER_RESERVED : Reserved
AEE_SOUNDPLAYER_LAYER_3 : MPEG layer 3 compression
AEE_SOUNDPLAYER_LAYER_2 : MPEG layer 2 compression
AEE_SOUNDPLAYER_LAYER_1 : MPEG layer 1 compression
AEE_SOUNDPLAYER_LAYER_UNKNOWN : Unable to determine layer information


Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3SampleRate

Description:
AEESoundPlayerMP3SampleRate specifies MP3 sample rate.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_UNKNOWN,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_8000 = 1,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_11025,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_12000,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_16000,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_22050,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_24000,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_32000,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_44100,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_48000,
   AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_LAST
} AEESoundPlayerMP3SampleRate;

Members:

AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_UNKNOWN : Unknown rate
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_8000 : 8k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_11025 : 11.025k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_12000 : 12k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_16000 : 16k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_22050 : 22.050k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_24000 : 24k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_32000 : 32k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_44100 : 44.1k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_48000 : 48k
AEE_SOUNDPLAYER_MP3_SAMPLE_RATE_LAST : Reserved


Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerMP3Spec

Description:
AEESoundPlayerMP3Spec indicates the audio specifications of MP3 file type. It is
used with the AEESOUNDPLAYER_AUDIO_SPEC playback callback.

Definition:
typedef struct
{
   AEESoundPlayerFile fType;
   AEESoundPlayerMP3Version version;
   AEESoundPlayerMP3Layer layer;
   boolean crcFlag;
   AEESoundPlayerMP3BitRate bitrate;
   AEESoundPlayerMP3SampleRate sampleRate;
   AEESoundPlayerMP3Channel channel;
   AEESoundPlayerMP3Extension extension;
   boolean copyrightFlag;
   boolean originalFlag;
   AEESoundPlayerMP3Emphasis emphasis;
   char title[AEE_ID3_TAG_LENGTH];
   char artist[AEE_ID3_TAG_LENGTH];
   char album[AEE_ID3_TAG_LENGTH];
   char year[4];
   char comment[AEE_ID3_TAG_LENGTH];
   uint8 genre;
} AEESoundPlayerMP3Spec;

Members:
fType : Format of audio file
version : MPEG version
layer : MPEG layer description
crcFlag : True if CRC protection
bitrate : Bit rate
sampleRate : Sampling rate
channel : Channel mode
extension : Used Only when channel is AEE_SOUNDPLAYER_MP3_CHANNEL_JOINT_STEREO
copyrightFlag : True if copyrighted
originalFlag : True if original
emphasis : Audio emphasis
title : Song title
artist : Song performer
album : Album with the song
year : Year Album released
comment : Text comment
genre : ID3 genre tag

Comments:
None

See Also:
AEESoundPlayerFile ,
AEESoundPlayerMP3Version ,
AEESoundPlayerMP3Layer ,
AEESoundPlayerMP3BitRate ,
AEESoundPlayerMP3SampleRate ,
AEESoundPlayerMP3Channel ,
AEESoundPlayerMP3Extension ,
AEESoundPlayerMP3Emphasis

=============================================================================

AEESoundPlayerMP3Version

Description:
AEESoundPlayerMP3Version specifies the MPEG version types.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_MP3_VERSION_25,
   AEE_SOUNDPLAYER_MP3_VERSION_RESERVED,
   AEE_SOUNDPLAYER_MP3_VERSION_2,
   AEE_SOUNDPLAYER_MP3_VERSION_1,
   AEE_SOUNDPLAYER_MP3_VERSION_UNKNOWN
} AEESoundPlayerMP3Version;

Members:

AEE_SOUNDPLAYER_MP3_VERSION_25 : MPEG version 2.5
AEE_SOUNDPLAYER_MP3_VERSION_RESERVED : Reserved
AEE_SOUNDPLAYER_MP3_VERSION_2 : MPEG version 2.0
AEE_SOUNDPLAYER_MP3_VERSION_1 : MPEG version 1.0
AEE_SOUNDPLAYER_MP3_VERSION_UNKNOWN : Unable to determine version

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerSource

Description:
AEESoundPlayerSource used internally by the ISoundPlayer Interface .

Definition:
typedef enum {
   AEE_SOUNDPLAYER_SOURCE_UNKNOWN, // An invalid type
   AEE_SOUNDPLAYER_SOURCE_MEM, // Designates that data is found in memory
   AEE_SOUNDPLAYER_SOURCE_FILE, // Designates that data is found in the Embedded File System (EFS)
   AEE_SOUNDPLAYER_SOURCE_LAST
} AEESoundPlayerSource;

Members:
AEE_SOUNDPLAYER_SOURCE_UNKNOWN : Invalid type
AEE_SOUNDPLAYER_SOURCE_MEM : Data is found in the memory
AEE_SOUNDPLAYER_SOURCE_FILE : Data is found in the Embedded File System (EFS)
AEE_SOUNDPLAYER_SOURCE_LAST : Reserved

Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerStatus

Description:
AEESoundPlayerStatus is returned in callback functions to indicate ISoundPlayer
events and return data to application.

Definition:
typedef enum {
   AEE_SOUNDPLAYER_UNKNOWN,
   AEE_SOUNDPLAYER_SUCCESS,
   AEE_SOUNDPLAYER_REWIND,
   AEE_SOUNDPLAYER_FFORWARD,
   AEE_SOUNDPLAYER_PAUSE,
   AEE_SOUNDPLAYER_RESUME,
   AEE_SOUNDPLAYER_TEMPO,
   AEE_SOUNDPLAYER_TUNE,
   AEE_SOUNDPLAYER_PAN,
   AEE_SOUNDPLAYER_AUDIO_SPEC,
   AEE_SOUNDPLAYER_TICK_UPDATE,
   AEE_SOUNDPLAYER_DATA_ACCESS_DELAY,
   AEE_SOUNDPLAYER_ABORTED,
   AEE_SOUNDPLAYER_REPEAT,
   AEE_SOUNDPLAYER_DONE,
   AEE_SOUNDPLAYER_FAILURE,
   AEE_SOUNDPLAYER_LAST
} AEESoundPlayerStatus;

Members:

AEE_SOUNDPLAYER_UNKNOWN : Unknown status
AEE_SOUNDPLAYER_SUCCESS : Request is accepted
AEE_SOUNDPLAYER_REWIND : Playback is currently rewinding
AEE_SOUNDPLAYER_FFORWARD : Playback is currently fast forwarding
AEE_SOUNDPLAYER_PAUSE : Playback is currently paused
AEE_SOUNDPLAYER_RESUME : Playback has resumed
AEE_SOUNDPLAYER_TEMPO : Playback tempo changed
AEE_SOUNDPLAYER_TUNE : Playback tune changed
AEE_SOUNDPLAYER_PAN : Playback stereo pan changed
AEE_SOUNDPLAYER_AUDIO_SPEC : Audio spec information
AEE_SOUNDPLAYER_TICK_UPDATE : One second update during playback
AEE_SOUNDPLAYER_DATA_ACCESS_DELAY : Playback is being delayed by data access
AEE_SOUNDPLAYER_ABORTED : Command was aborted
AEE_SOUNDPLAYER_REPEAT : Sound repeating
AEE_SOUNDPLAYER_DONE : Command has been carried out
AEE_SOUNDPLAYER_FAILURE : Error occurred with this request
AEE_SOUNDPLAYER_LAST :  Reserved.

Comments:
    None

See Also:
    None

================================================================================

PFNSOUNDPLAYERSTATUS

Description:
PFNSOUNDPLAYERSTATUS is the type specification for the ISoundPlayer callback function
that application must register with ISoundPlayer. ISoundPlayer sends all the events
and data to application via the registered callback function.

Definition:
typedef void ( * PFNSOUNDPLAYERSTATUS)
(
   void * pUser,
   AEESoundPlayerCmd eCBType,
   AEESoundPlayerStatus eSPStatus,
   uint32 dwParam
);

Members:
pUser : Application specified data pointer
eCBType : Type of callback
eSPStatus : Status within the callback type
dwParam : Pointer to AEESoundPlayerCmdData, if any. NULL otherwise.

Comments:
None

See Also:
AEESoundPlayerCmd ,
AEESoundPlayerStatus

=====================================================================
AEEVoicePrompt

Description:
AEEVoicePrompt is not implemented.

Definition:
typedef struct
{
byte * pbData;
uint16 wNumFrames;
} AEEVoicePrompt;

Members:

pbData : Not implemented
wNumFrames : Not implemented


Comments:
None

See Also:
None

=============================================================================

AEESoundPlayerInfo

Description:
    AEESoundPlayerInfo specifies the audio source. It is used to set the audio source
of ISoundPlayer using ISOUNDPLAYER_SetInfo().

Definition:
    typedef structure {
    AEESoundPlayerInput eInput;
    void * pData;
    uint32 dwSize;
    } AEESoundPlayerInfo;

Members:
    eInput :  Audio source (input) type. It can be file or buffer.
    pData :  Pointer to the file name or buffer based on the input type.
    dwSize :  Size of the data in the buffer. Valid only for buffer input type.


Comments:
    The parameter pData is owned by the application. ISoundPlayer does not allocate
or deallocate pData.

See Also:
    AEESoundPlayerInput

=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

ISoundPlayer Interface

The ISoundPlayer Interface has been deprecated, use the IMedia Interface.

ISoundPlayer interface provides audio multimedia services. ISoundPlayer supports
MIDI, MP3 and QCP (QUALCOMM PureVoice or QCELP) formats. For more details on QCP
format and the associated BREW SDK tools, please refer to BREW SDK User's Guide.

Creating ISoundPlayer Instance

You can create ISoundPlayer instance in two ways:

1.   Using ISHELL_CreateInstance() with AEECLSID_SOUNDPLAYER as ClassID.

2.   Using ISHELL_GetHandler() with AEEHandlerType set to HTYPE_SOUND and MIME string
set to snd/midi for MIDI, snd/mp3 for MP3, or snd/qcp for QCP formats. Call ISHELL_CreateInstance()
with the returned ClassID.

ISoundPlayer Input Types

You can specify the audio source,(input type and data), in AEESoundPlayerInfo structure
and set it using ISOUNDPLAYER_SetInfo(). The ISoundPlayer Interface can play using
a memory buffer, SDT_BUFFER or file, SDT_FILE. Playback while streaming is not supported,
but you can associate an IAStream with ISoundPlayer using ISOUNDPLAYER_SetStream().
ISoundPlayer reads the entire data from the stream into a buffer, and plays the
buffer.

ISOUNDPLAYER_GetInfo() returns the current audio source if a pointer to AEESoundPlayerInfo
is given.

NOTE: ISoundPlayer does not set the audio source if it is busy. ISoundPlayer is
busy if it is playing, executing a command, or streaming input data. To find if
ISoundPlayer is busy, you call: int ISOUNDPLAYER_GetInfo(pISoundPlayer, NULL); It
returns EIDLE if ISoundPlayer is idle or EITEMBUSY if it is busy.

NOTE: ISOUNDPLAYER_Set() is deprecated. Instead use ISOUNDPLAYER_SetInfo() which
provides better access to ISoundPlayer.

Playback Control

ISoundPlayer provides the following playback control functions:


ISOUNDPLAYER_Play() 
 To play the MIDI/MP3/QCP file
 
ISOUNDPLAYER_Stop() 
 To stop the playback
 
ISOUNDPLAYER_Rewind() 
 To rewind playback for n milliseconds. This command is not supported for QCP format.
 
ISOUNDPLAYER_FastForward() 
 To fast forward playback for n milliseconds This command is not supported for QCP
format.
 
ISOUNDPLAYER_Pause() 
 To pause the playback, This command is not supported for QCP format.
 
ISOUNDPLAYER_Resume() 
 To resume the playback This command is not supported for QCP format.
 
ISOUNDPLAYER_SetTempo() 
 To set playback tempo for MIDI. Not applicable for MP3 and QCP.
 
ISOUNDPLAYER_SetTune() 
 To set the pitch level, in half step increments, of the current MIDI playback.
Not applicable for MP3 and QCP
 


ISOUNDPLAYER_Play() gets precedence over ISOUNDPLAYER_GetTotalTime(). There can
be only one active ISOUNDPLAYER_Play(). You need to issue a stop before you can
call another ISOUNDPLAYER_Play(). Only one of the commands from #4 -#9 (in the table
above) can be active at any time. ISoundPlayer sends AEE_SOUNDPLAYER_FAILURE if
a command does not succeed.

Callback Notification

Most of the calls to ISoundPlayer API are asynchronous in nature. If application
is to receive status and events from ISoundPlayer, the application must register
a callback notification function using ISOUNDPLAYER_RegisterNotify(). ISoundPlayer
sends status and events to the user application through this registered callback
function.

There are five types of callback functions invoked by the ISoundPlayer Interface.

AEE_SOUNDPLAYER_PLAY_CB

AEE_SOUNDPLAYER_STATUS_CB

AEE_SOUNDPLAYER_TIME_CB

AEE_SOUNDPLAYER_SOUND_CB

AEE_SOUNDPLAYER_VOLUME_CB

Each callback type, in turn, contains a set of status indications and data that
are valid only for callbacks of that type.

For example, for AEE_SOUNDPLAYER_PLAY_CB, the status type can be:

AEE_SOUNDPLAYER_SUCCESS

AEE_SOUNDPLAYER_PAUSE

AEE_SOUNDPLAYER_RESUME

and so forth

Any Data is returned in a callback through the dwParam parameter that points to
an AEESoundPlayerCmdData structure if data is present. Otherwise the dwParam is
NULL.

During playback, every second the applet is notified of AEE_SOUNDPLAYER_TICK_ UPDATE
status by AEE_SOUNDPLAYER_PLAY_CB. This can be used by the applet to update the
progress of playback.

After rewind, fast forward, pause, or resume, the dwParam parameter of AEE_SOUNDPLAYER_PLAY_CB
callback function's dwParam points to the current position in milliseconds of the
playback. Again, applets can use this to update the playback progress. These mechanisms
are demonstrated in the sample MIDI-MP3 Player applet.

It is not always required to register a callback function. Choose not to get any
ISoundPlayer events by calling ISOUNDPLAYER_RegisterNotify() as follows:

ISOUNDPLAYER_RegisterNotify(pISoundPlayer, NULL, NULL);

Audio Device And Volume Control

ISoundPlayer internally uses an instance of ISound for setting the audio device
parameters (ISOUNDPLAYER_SetSoundDevice()) and for setting/getting the volume of
the output (ISOUNDPLAYER_SetVolume() and ISOUNDPLAYER_GetVolume()). You do not need
to create an ISound instance for these operations.

To use the services of ISoundPlayer Interface

1.   Create the ISoundPlayer object by calling ISHELL_CreateInstance() with AEECLSID_SOUNDPLAYER
as ClassID.

2.   Call ISOUNDPLAYER_RegisterNotify() to register the ISoundPlayer callback function.
The ISoundPlayer Interface always uses this callback function to notify the applet
of any events or status changes.

3.   Set the source of the MIDI/MP3/QCP media (file/buffer using the ISOUNDPLAYER_SetInfo()
function. Be sure to check if it is successful.

4.   In the applet event handler, call any of the playback control functions like
ISOUNDPLAYER_GetTotalTime(), ISOUNDPLAYER_Play().

5.   Before exiting, always:

a.   Un-register the callback function by calling

     ISOUNDPLAYER_RegisterNotify(pISoundPlayer, NULL, NULL)

b.   Call ISOUNDPLAYER_Stop() if playback is active.


The following header file is required:

AEESoundPlayer.h

=============================================================================

ISOUNDPLAYER_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    ISOUNDPLAYER_Release()

=============================================================================

ISOUNDPLAYER_FastForward()

Description:
    This function issues a command to fast-forward the current MIDI/MP3 playback.

Prototype:
    void ISOUNDPLAYER_FastForward
    (
    ISoundPlayer * pISoundPlayer, 
    uint32 dwTime
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    dwTime  :  Amount of time, in milliseconds, to fast-forward the current playback.

Return Value:
    None

Comments:
    This command has no effect on QCP format

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful and accepted the fast-forward command.~
    AEE_SOUNDPLAYER_FAILURE 
     Failed and did not accept the fast-forward command. Possibly, playback is not
active or another command is active.~
*

    Fast-forwarding past the end of the file ends the current audio playback and
triggers a AEE_SOUNDPLAYER_DONE playback callback. This API triggers AEE_SOUNDPLAYER_FFORWARD
playback callback if playback is active. When the fast-forward function is completed,
AEE_SOUNDPLAYER_RESUME playback callback if the audio file has not ended.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    None

=============================================================================

ISOUNDPLAYER_GetInfo()

Description:
    This function gets either the current audio source of ISoundPlayer or the state
of ISoundPlayer. The status of the function is returned immediately.

Prototype:
    int ISOUNDPLAYER_GetInfo
    (
    ISoundPlayer * pISoundPlayer,
    AEESoundPlayerInfo * pInfo
    )

Parameters:
    pISoundPlayer  :  [in] :  Pointer to the ISoundPlayer Interface object.
    pInfo  :  [out] :  Audio source of ISoundPlayer.

Return Value:
    AEE_SUCCESS
     Audio source is returned successfully.~
    Error code
     If pInfo is not NULL or if pInfo is NULL, then it returns one of the following:
~
     EITEMBUSY
     ISoundPlayer is busy if it is currently playing, executing a command, or streaming
input data.~
     EIDLE
     ISoundPlayer is idle.~
*

Comments:
    Application must allocate memory for pInfo.

Version:
   Introduced BREW Client 1.1
   Deprecated BREW Client 2.0

See Also:
    AEESoundPlayerInfo
    ISOUNDPLAYER_SetInfo()

=============================================================================

ISOUNDPLAYER_GetTotalTime()

Description:
    This function issues a command to request the calculation of the total playback
time of the specified MP3/MIDI input source.

Prototype:
    void ISOUNDPLAYER_GetTotalTime(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    Only one play or get-total-time command executes at a time. A play command overrides
any previous play or get-total-time command. However, a get-total-time command does
not interrupt a play command.
    This command has no effect on QCP format.

Side Effects:
    The result of the operation is sent to the client by the AEE_SOUNDPLAYER_TIME_CB
callback function pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful getting the total playback time~
    AEE_SOUNDPLAYER_FAILURE 
     Failed getting the total playback time. Possibly, playback is active or another
get-total-time command is active.~
    AEE_SOUNDPLAYER_DONE 
     Successful getting the total playback time. The dwParam parameter to the callback
function points to AEESoundPlayerCmdData’s dwTotalTime parameter contains the current
time in milliseconds.~
    AEE_SOUNDPLAYER_ABORTED 
     Command is aborted by the ISOUNDPLAYER_Play()~
*

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    None

=============================================================================

ISOUNDPLAYER_GetVolume()

Description:
    This function gets the volume for the current playback device and method.

Prototype:
    void ISOUNDPLAYER_GetVolume(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    The volume range is from 0 (zero, lowest) to AEE_MAX_VOLUME (highest).

Side Effects:
    The result of the operation is sent to the client by the AEE_SOUND_VOLUME_CB
callback function pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS
     Successful getting volume value. The dwParam points to AEESoundPlayerCmdData
in which the wVolume contains the current volume. ~
    AEE_SOUNDPLAYER_FAILURE
     Failed to get volume value.~
*

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also
    AEESoundCmd
    ISOUNDPLAYER_SetVolume()

=============================================================================

ISOUNDPLAYER_Pause()

Description:
    This function issues a command to pause the current MIDI/MP3 playback.

Prototype:
    void ISOUNDPLAYER_Pause(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    This command has no effect on QCP format.

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful and accepted the pause command.~
    AEE_SOUNDPLAYER_FAILURE 
     Failed and did not accept the pause command. Possibly, playback is not active
or another command is active.~
*
    This routine triggers a AEE_SOUNDPLAYER_PAUSE playback callback if playback
is active.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    None

=============================================================================

ISOUNDPLAYER_Play()

Description:
    This function issues a command to play a MIDI/MP3/QCP audio from the input source
specified in the call to ISOUNDPLAYER_Set().

Prototype:
    void ISOUNDPLAYER_Play(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    Only one play or get-total-time command is executed at a time. A play command
overrides any previous get-total-time command. However, a get-total-time command
does not interrupt a play command. If a playback is currently active, then a new
play command is not be accepted.

Side Effects:
    The registered callback function, with AEE_SOUNDPLAYER_PLAY_CB, is triggered
multiple times during the playback to indicate the status of the current playback
transaction. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Play command was accepted and begins playback.~
    AEE_SOUNDPLAYER_REWIND 
     Playback is rewinding.~
    AEE_SOUNDPLAYER_FFORWARD 
     Playback is fast-forwarding.~
    AEE_SOUNDPLAYER_PAUSE 
     Playback is paused.~
    AEE_SOUNDPLAYER_RESUME 
     Playback is resuming after a pause, rewind, fast-forward process.~
    AEE_SOUNDPLAYER_TEMPO 
     Playback tempo factor has been updated.~
    AEE_SOUNDPLAYER_TUNE 
     Playback tune factor has been updated.~
    AEE_SOUNDPLAYER_AUDIO_SPEC 
     Relevant file specs for MIDI/MP3 are provided. In BREW Simulator, only file
type is returned. This callback does not happen for QCP format. ~
    AEE_SOUNDPLAYER_TICK_UPDATE 
     Periodic tick update every second.~
    AEE_SOUNDPLAYER_DATA_ACCESS_DELAY
     Data access delay is causing the playback to be temporarily delayed.~
    AEE_SOUNDPLAYER_ABORTED 
     Playback was aborted.~
    AEE_SOUNDPLAYER_DONE 
     Playback is finished.~
    AEE_SOUNDPLAYER_FAILURE 
     Play command was not accepted.Possibly there was an error processing the audio
input or another playback is active.~
*

    The following information is passed along in the callback function's dwParam
for AEE_SOUNDPLAYER_AUDIO_SPEC status:
~
    If MIDI/CMX 
     Pointer to the AEESoundPlayerMIDISpec structure.~
    If MP3 
     Pointer to the AEESoundPlayerMP3Spec structure.~
*

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_Set()

=============================================================================

ISOUNDPLAYER_RegisterNotify()

Description:
    This function registers the status event callback function.

Prototype:
    void ISOUNDPLAYER_RegisterNotify
    (
    ISoundPlayer * pISoundPlayer,
    PFNSOUNDPLAYERSTATUS pfn,
    void * pUser
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    pfn  :  Pointer to the user callback (NULL to deregister).
    pUser  :  User data for unique correlation/identification of the transaction.
This piece of information is not retrieved or processed by ISoundPlayer and can
be NULL if no identifying data is required. This same data pointer is passed back
to the client along with status callback to correlate the transactions.

Return Value:
    None

Comments:
    The following functions and their callback status types are listed here:
~
    ISOUNDPLAYER_Play 
     AEE_SOUNDPLAYER_PLAY_CB~
    ISOUNDPLAYER_Stop 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_Rewind 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_FastForward 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_Pause 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_Resume 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_SetTempo 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_SetTune 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_SetVolume 
     AEE_SOUNDPLAYER_STATUS_CB~
    ISOUNDPLAYER_GetVolume 
     AEE_SOUNDPLAYER_VOLUME_CB~
    ISOUNDPLAYER_GetTotalTime 
     AEE_SOUNDPLAYER_TIME_CB~
    ISOUNDPLAYER_SetSoundDevice 
     AEE_SOUNDPLAYER_STATUS_CB~
*

    The above list also indicates what types of status are sent back through the
callback functions. The status type determines the way the data portion of the status
is interpreted. The status indicates the status of the operations or the result
of the query in case of get-operations. See each of the function descriptions to
find out the effects of this registered callback function.
    NOTE: Applications can choose not to register the callback by issuing the following
call:
===pre>
    ISOUNDPLAYER_RegisterNotify(pISoundPlayer, NULL, NULL);
===/pre>
Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    PFNSOUNDPLAYERSTATUS

=============================================================================

ISOUNDPLAYER_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_AddRef()

=============================================================================

ISOUNDPLAYER_Resume()

Description:
    This function issues a command to resume the current MIDI/MP3 playback.

Prototype:
    void ISOUNDPLAYER_Resume(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    Playback must have been paused with the ISOUNDPLAYER_Pause() function.

    If playback is not currently paused by the ISOUNDPLAYER_Pause() function, this
function call has no effect.

    This command has no effect on QCP format.

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful and accepted the resume command~
    AEE_SOUNDPLAYER_FAILURE 
     Failed and did not accept the resume command. Possibly, playback is not active
or another command is active.~
*

    This routine triggers a AEE_SOUNDPLAYER_RESUME playback callback if playback
is active.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_Pause()

=============================================================================

ISOUNDPLAYER_Rewind()

Description:
    This function issues a command to rewind the current MIDI/MP3 playback.

Prototype:
    void ISOUNDPLAYER_Rewind(ISoundPlayer * pISoundPlayer, uint32 dwTime)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    dwTime  :  Amount of time, in milliseconds, to rewind the current playback.

Return Value:
    None

Comments:
    This command has no effect on QCP format.

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful and accepted the rewind command.~
    AEE_SOUNDPLAYER_FAILURE 
     Failed and did not accept the rewind command. Possibly, playback is not active
or another command is active.~
*

    Rewinding past the beginning of the audio file, restarts playback from the beginning.
This routine triggers a AEE_SOUNDPLAYER_REWIND playback callback if playback is
active. When the rewind function is completed, a AEE_SOUNDPLAYER_RESUME playback
callback is triggered.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    None

=============================================================================

ISOUNDPLAYER_Set()
    NOTE: This function is deprecated. Use ISOUNDPLAYER_SetInfo() instead.

Description:
    This function frees the memory associated with previous input data buffer, if
any, and sets the current input data buffer as the source of the MIDI/MP3/QCP audio.
    NOTE: The SDT_VOICEPROMPT input source is not supported in the current version
of BREW.

Prototype:
    void ISOUNDPLAYER_Set
    (
    ISoundPlayer * pISoundPlayer,
    AEESoundPlayerInput t,
    void * pData
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    t  :  Source type of pData. Supported values include:
     -    SDT_FILE: Specified data is file name.
     -    SDT_BUFFER: Specified data is raw buffer.
    pData  :  Pointer to the data of specified type.

Return Value:
    None

Comments:
    NOTE: Only SDT_FILE is supported in the BREW Simulator.

Side Effects:
    If the input type is SDT_FILE, ISoundPlayer copies the file name from pData
into its local buffer. It is the application's responsibility to free the memory
allocated for pData.

    If the input type is SDT_BUFFER, ISoundPlayer does not make its own copy, but
uses the pData pointer to the application-allocated buffer. ISoundPlayer frees this
memory after another ISOUNDPLAYER_Set() or last ISOUNDPLAYER_Release(). The application
should not free this memory.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 1.1

See Also:
    AEESoundPlayerInput

=============================================================================

ISOUNDPLAYER_SetInfo()

Description:
    This function either sets or resets the audio source of ISoundPlayer. The status
of the function is returned immediately.

Prototype:
    int ISOUNDPLAYER_SetInfo
    (
    ISoundPlayer * pISoundPlayer,
    AEESoundPlayerInfo * pInfo
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    pInfo  :  Audio source of ISoundPlayer.

Return Value:
    AEE_SUCCESS, if the audio source is set, if pInfo is not NULL or if the audio
source is reset successfully, if pInfo is NULL. ~
    Error Codes:
~
    EITEMBUSY, if ISoundPlayer is busy. ISoundPlayer is busy if it is currently
playing, executing a command, or streaming input data.~
    EBADPARM, if the function encounters a bad parameter (like setting pInfo ->
dwSize to zero).~
    EUNSUPPORTED, if the input type is not supported in the current version of BREW.~
*

Comments:
    If pInfo is NULL, a successful execution of this function causes ISoundPlayer
to release the memory it allocated for the previous audio source. It is application’s
responsibility to free the memory allocated for pInfo -> pData.

Side Effects:
    If the audio source is set successfully, memory allocated by ISoundPlayer for
previous audio source is released. If the input type is SDT_FILE, ISoundPlayer copies
the file name from pInfo -> pData into its local buffer.

Version:
   Introduced BREW Client 1.1
   Deprecated BREW Client 2.0

See Also:
    AEESoundPlayerInfo
    ISOUNDPLAYER_GetInfo()

=============================================================================

ISOUNDPLAYER_SetSoundDevice()

Description:
    This function commands the device to connect to a specific audio device. There
is always one, and only one, audio device connected at any particular time.

Prototype:
    void ISOUNDPLAYER_SetSoundDevice
    (
    ISoundPlayer * pISoundPlayer,
    AEESoundDevice eDevice,
    AEESoundMuteCtl eEarMute,
    AEESoundMuteCtl eMicMute
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    eDevice :  Audio device to be connected.
    eEarMute  :  Mute or unmute the ear piece.
    eMicMute  :  Mute or unmute the microphone.

Return Value:
    None

Comments:
    NOTE: This function is not supported in the BREW Simulator in the current version
of BREW, but it is available on the BREW-enabled devices.

Side Effects:
    The result of the operation is sent to the client through the AEE_SOUNDPLAYER_SOUND_CB
callback function pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful connecting to the new sound device~
    AEE_SOUNDPLAYER_FAILURE 
     Failed connecting to the new sound device. Possibly another command is active.
*

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    AEESoundMuteCtl
    AEESoundDevice

=============================================================================

ISOUNDPLAYER_SetStream()

Description:
    This function allows an IAStream Interface to be associated with an ISoundPlayer
instance to allow audio source data to be streamed from IAStream (like file or socket).

Prototype:
    void ISOUNDPLAYER_SetStream
    (
    ISoundPlayer * pISoundPlayer, 
    IAStream * ps
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object that receives
the streaming audio input.
    ps :  Pointer to an instance of a class that implements the IAStream Interface
(for example, the IFile Interface or the ISocket Interface).

Return Value:
    None

Comments:
    ISOUNDPLAYER_SetStream() only supports network stream from TCP socket but not
UDP socket.

    ISoundPlayer reads the whole stream into a buffer and internally uses SDT_BUFFER
as input type.

Side Effects:
    If ISoundPlayer is not busy, then memory allocated (by ISoundPlayer) for previous
audio source is released.

    The function asynchronously triggers a AEE_SOUNDPLAYER_STATUS callback. The
status list includes:
~
    AEE_SOUNDPLAYER_SUCCESS
     Audio source data streaming successfully completed.~
    AEE_SOUNDPLAYER_FAILURE
     Audio source data streaming failed. Reasons can be ISoundPlayer is busy or
streaming error.~
*

    If streaming is successful, then internally ISoundPlayer allocates memory for
the buffer and sets the audio source type as SDT_BUFFER. The application need not
worry about this memory.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    IASTREAM_Read()
    IASTREAM_Readable()

=============================================================================

ISOUNDPLAYER_SetTempo()

Description:
    This function issues a command to modify the playback tempo of the current MIDI
playback as a percentage of the normal playback tempo.

Prototype:
    void ISOUNDPLAYER_SetTempo
    (
    ISoundPlayer * pISoundPlayer,
    uint32 dwTempoFactor
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    dwTempoFactor  :  This parameter is used to modify the playback tempo as a percentage
of the normal playback tempo. Possibly, playback is not active or another command
is active. A dwTempoFactor of 50 means half of the normal playback tempo. The maximum
supported tempo factor is 500% or five times of the normal playback tempo. The minimum
tempo factor is 1% of the normal playback tempo. The tempo factor is initially 100%
when the ISOUNDPLAYER_Play() function is called. 

Return Value:
    None

Comments:
    NOTE: This function is not supported in the BREW Simulator in the current version
of BREW, but is supported on the target device.
    This function is not applicable to MP3 and QCP formats.

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successfully set a new tempo value~
    AEE_SOUNDPLAYER_FAILURE 
     Failed to set a new tempo value; old value remains~
*

    This routine triggers a AEE_SOUNDPLAYER_TEMPO playback callback if playback
is active.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_Play()

=============================================================================

ISOUNDPLAYER_SetTune()

Description:
    This function issues a command to modify the tune (pitch level) of the current
MIDI playback in half-step increments.
    NOTE: This function is not supported in the BREW Simulator in the current version
of BREW, but is supported on the BREW-enabled devices.

Prototype:
    void ISOUNDPLAYER_SetTune(ISoundPlayer * pISoundPlayer, int8 nStep)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    nStep  :  This parameter is used to modify the tune of the playback in half-step
increments.
===pre>   
     0 indicates normal playback. 
    -1 indicates 1 half-step lower than the normal playback. 
    +1 indicates 1 half-step higher than the normal playback. 
    -12 indicates 12 half-steps, or 1 octave lower than the normal playback. 
    +12 indicates 12 half-steps, or 1 octave higher than the normal playback
===/pre>

Return Value:
    None

Comments:
    For each playback nStep is reset to 0.

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successfully set a new tune value~
    AEE_SOUNDPLAYER_FAILURE 
     Failed to set a new tune value; old value remains~
*

    This routine triggers a AEE_SOUNDPLAYER_TUNE playback callback if playback is
active.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_Play()

=============================================================================

ISOUNDPLAYER_SetVolume()

Description:
    This function sets the volume for the current playback device.

Prototype:
    void ISOUNDPLAYER_SetVolume
    (
    ISoundPlayer * pISoundPlayer, 
    uint16 wVolume
    )

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.
    wVolume  :  New volume level for device/method.

Return Value:
    None

Comments:
    The volume range is from 0 (zero, lowest) to AEE_MAX_VOLUME (highest).

Side Effects:
    The result of the operation is sent to the client through the AEE_SOUNDPLAYER_SOUND_CB
callback function pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successfully set a new volume value~
    AEE_SOUNDPLAYER_FAILURE 
     Failed to set a new volume value; old value remains~
*

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    ISOUNDPLAYER_GetVolume()

=============================================================================

ISOUNDPLAYER_Stop()

Description:
    This function issues a command to stop the current MIDI/MP3/QCP playback.

Prototype:
    void ISOUNDPLAYER_Stop(ISoundPlayer * pISoundPlayer)

Parameters:
    pISoundPlayer  :  Pointer to the ISoundPlayer Interface object.

Return Value:
    None

Comments:
    None

Side Effects:
    The result of the operation is sent to the client through the callback function
pointer. The status can be one of the following values:
~
    AEE_SOUNDPLAYER_SUCCESS 
     Successful and accepted the stop command.~
    AEE_SOUNDPLAYER_FAILURE 
     Failed and did not accept the stop command.~
*

    If this operation is successful, the function also triggers another event to
be sent to the client with the AEE_SOUNDPLAYER_DONE or MM_STATUS_ABORTED value for
the playback transaction.

Version:
   Introduced BREW Client 1.0
   Deprecated BREW Client 2.0

See Also:
    None

==================================================================*/

#endif // AEESOUNDPLAYER_H

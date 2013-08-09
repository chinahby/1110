#ifndef AEEISOUND_H
#define AEEISOUND_H
/*============================================================================
FILE: AEESound.h

SERVICES:  Basic Sound Services

DESCRIPTION:
   This file provides definitions for basic sound interfaces
   available to application developers.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2002,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#include "AEEIBase.h"
#include "AEEIResourceCtl.h"

#define AEEIID_ISound              AEECLSID_Sound 

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#ifdef FEATURE_VERSION_K212
#define AEE_MAX_VOLUME            80   // Volume from 0 (silence) to AEE_MAX_VOLUME
#else
#define AEE_MAX_VOLUME            100  // Volume from 0 (silence) to AEE_MAX_VOLUME
#endif

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
#ifdef CUST_EDITION
typedef enum {
   OEMSOUND_MUTE_VOL = 0,
   OEMSOUND_1ST_VOL,
   OEMSOUND_2ND_VOL,
   OEMSOUND_3RD_VOL,
   OEMSOUND_4TH_VOL,
   OEMSOUND_5TH_VOL,
   OEMSOUND_ESCALATING_VOL
} OEMSound_Volume_Type;
#define OEMSOUND_AVG_VOL OEMSOUND_2ND_VOL
#define OEMSOUND_MAX_VOL OEMSOUND_5TH_VOL
#define OEMSOUND_MAX_RING_VOL OEMSOUND_ESCALATING_VOL
#endif

enum {
   AEE_SOUND_UNKNOWN       = 0,
   AEE_SOUND_SUCCESS       = 1,
   AEE_SOUND_PLAY_DONE     = 2,
   AEE_SOUND_FAILURE       = 3,
   AEE_SOUND_RESOURCE_BUSY = 4,

   AEE_SOUND_LAST          = 0x7f
};

#ifdef _WIN32
typedef unsigned AEESoundStatus;
#else
typedef int8 AEESoundStatus;
#endif

enum {
   AEE_SOUND_DEVICE_UNKNOWN = 0, // Unknown device
   AEE_SOUND_DEVICE_HANDSET = 1, // Handset
   AEE_SOUND_DEVICE_HFK     = 2, // Hands Free Kit (HFK)
   AEE_SOUND_DEVICE_HEADSET = 3, // Mono Headset

   AEE_SOUND_DEVICE_AHFK    = 4, // Analog HFK
   AEE_SOUND_DEVICE_SDAC    = 5, // Stereo DAC

   AEE_SOUND_DEVICE_TTY_HFK      = 6, // TTY HFK
   AEE_SOUND_DEVICE_TTY_HEADSET  = 7, // TTY Headset

   AEE_SOUND_DEVICE_CURRENT      = 8,   // Currently selected device
   AEE_SOUND_DEVICE_SPEAKER      = 9,   // Built-In Speaker device
   AEE_SOUND_DEVICE_BT_HEADSET   = 10,  // BlueTooth headset
   AEE_SOUND_DEVICE_STEREO_HEADSET     = 11, // Stereo headset
   AEE_SOUND_DEVICE_USB                = 12, // USB
   AEE_SOUND_DEVICE_BT_STEREO_HEADSET  = 13, // BlueTooth stereo headset
   AEE_SOUND_DEVICE_TTY_HCO            = 14, //forward link = tty data, reverse link = voice
   AEE_SOUND_DEVICE_TTY_VCO            = 15, //forward link = voice, reverse link = tty data
   AEE_SOUND_DEVICE_TTY_FULL           = 16, //tty data both directions.
#ifdef CUST_EDITION	   
#if defined( FEATURE_SOUND_DEVICE_HEADSET_SPEAKER)
   AEE_SOUND_DEVICE_HEADSET_SPEAKER	   = 17,
#endif
#endif /*CUST_EDITION*/
   AEE_SOUND_DEVICE_RESERVED3_START    = 89, // Reserved range #3
#ifdef FEATURE_FM_OEM
   AEE_SOUND_DEVICE_HEADSET_FM,
   AEE_SOUND_DEVICE_SPEAKER_FM,
#endif
   AEE_SOUND_DEVICE_RESERVED3_END      = 104,
   AEE_SOUND_DEVICE_RESERVED2_START    = 105, // Reserved range #2
   AEE_SOUND_DEVICE_RESERVED2_END      = 112,
   AEE_SOUND_DEVICE_RESERVED1_START    = 113, // Reserved range #1
   AEE_SOUND_DEVICE_RESERVED1_END      = 122,
   AEE_SOUND_DEVICE_LAST = 0x7f    // Reserved
};

#ifdef _WIN32
typedef unsigned AEESoundDevice;
#else
typedef int8 AEESoundDevice;
#endif

enum {
   AEE_SOUND_METHOD_UNKNOWN   = 0,    // Unknown method
   AEE_SOUND_METHOD_VOICE     = 1,    // Use the device's voice generator
   AEE_SOUND_METHOD_BEEP      = 2,    // Use the device's keybeep generator
   AEE_SOUND_METHOD_MESSAGE   = 3,    // Use the device's keybeep generator
   AEE_SOUND_METHOD_RING      = 4,    // Use the device's ring generator
   AEE_SOUND_METHOD_CLICK     = 5,    // Use the device's click generator
   AEE_SOUND_METHOD_MIDI      = 6,    // Use the device's Midi generator
   AEE_SOUND_METHOD_AUX       = 7,    // Use the device's auxiliary generator if avail
   AEE_SOUND_METHOD_RESERVED1_START = 113,
   AEE_SOUND_METHOD_RESERVED1_END   = 122,
   AEE_SOUND_METHOD_LAST      = 0x7f  // Reserved
};

#ifdef _WIN32
typedef unsigned AEESoundMethod;
#else
typedef int8 AEESoundMethod;
#endif

enum {
   AEE_SOUND_APATH_LOCAL = 0,
   AEE_SOUND_APATH_TX    = 1,
   AEE_SOUND_APATH_BOTH  = 2,
   AEE_SOUND_APATH_MUTE  = 3,
   AEE_SOUND_APATH_LAST = 0x7f
};
#ifdef _WIN32
typedef unsigned AEESoundAPath;
#else
typedef int8 AEESoundAPath;
#endif

enum {
   AEE_SOUND_MUTECTL_UNMUTED = 0,
   AEE_SOUND_MUTECTL_MUTED   = 1,
   AEE_SOUND_MUTECTL_LAST    = 0x7f
};

#ifdef _WIN32
typedef unsigned AEESoundMuteCtl;
#else
typedef int8 AEESoundMuteCtl;
#endif

enum {
   AEE_TONE_FIRST,
   AEE_TONE_0,                       /* DTMF for 0 key                                                           */
   AEE_TONE_1,                       /* DTMF for 1 key                                                           */
   AEE_TONE_2,                       /* DTMF for 2 key                                                           */
   AEE_TONE_3,                       /* DTMF for 3 key                                                           */
   AEE_TONE_4,                       /* DTMF for 4 key                                                           */
   AEE_TONE_5,                       /* DTMF for 5 key                                                           */
   AEE_TONE_6,                       /* DTMF for 6 key                                                           */
   AEE_TONE_7,                       /* DTMF for 7 key                                                           */
   AEE_TONE_8,                       /* DTMF for 8 key                                                           */
   AEE_TONE_9,                       /* DTMF for 9 key                                                           */
   AEE_TONE_A,                       /* DTMF for A key                                                           */
   AEE_TONE_B,                       /* DTMF for B key                                                           */
   AEE_TONE_C,                       /* DTMF for C key                                                           */
   AEE_TONE_D,                       /* DTMF for D key                                                           */
   AEE_TONE_POUND,                   /* DTMF for # key                                                           */
   AEE_TONE_STAR,                    /* DTMF for * key                                                           */
   AEE_TONE_CTRL,                    /* Tone for a control key                                                   */
   AEE_TONE_2ND,                     /* Tone for secondary function on a key                                     */
   AEE_TONE_WARN,                    /* Warning tone (e.g. overwriting user phone# slot)                         */
   AEE_TONE_ERR,                     /* Tone to indicate an error                                                */
   AEE_TONE_TIME,                    /* Time marker tone                                                         */
   AEE_TONE_RING_A,                  /* 1st Ringer tone                                                          */
   AEE_TONE_RING_B,                  /* 2nd Ringer tone                                                          */
   AEE_TONE_RING_C,                  /* 3rd Ringer tone                                                          */
   AEE_TONE_RING_D,                  /* 4th Ringer tone                                                          */
   AEE_TONE_RING_A4,                 /*  440.0 Hz  -Piano Notes-                                                 */
   AEE_TONE_RING_AS4,                /*  466.1 Hz                                                                */
   AEE_TONE_RING_B4,                 /*  493.8 Hz                                                                */
   AEE_TONE_RING_C4,                 /*  523.2 Hz                                                                */
   AEE_TONE_RING_CS4,                /*  554.3 Hz                                                                */
   AEE_TONE_RING_D4,                 /*  587.3 Hz                                                                */
   AEE_TONE_RING_DS4,                /*  622.2 Hz                                                                */
   AEE_TONE_RING_E4,                 /*  659.2 Hz                                                                */
   AEE_TONE_RING_F4,                 /*  698.5 Hz                                                                */
   AEE_TONE_RING_FS4,                /*  739.9 Hz                                                                */
   AEE_TONE_RING_G4,                 /*  784.0 Hz                                                                */
   AEE_TONE_RING_GS4,                /*  830.6 Hz                                                                */
   AEE_TONE_RING_A5,                 /*  880.0 Hz                                                                */
   AEE_TONE_RING_AS5,                /*  932.2 Hz                                                                */
   AEE_TONE_RING_B5,                 /*  987.7 Hz                                                                */
   AEE_TONE_RING_C5,                 /* 1046.5 Hz                                                                */
   AEE_TONE_RING_CS5,                /* 1108.7 Hz                                                                */
   AEE_TONE_RING_D5,                 /* 1174.6 Hz                                                                */
   AEE_TONE_RING_DS5,                /* 1244.3 Hz                                                                */
   AEE_TONE_RING_E5,                 /* 1318.5 Hz                                                                */
   AEE_TONE_RING_F5,                 /* 1397.0 Hz                                                                */
   AEE_TONE_RING_FS5,                /* 1479.9 Hz                                                                */
   AEE_TONE_RING_G5,                 /* 1568.0 Hz                                                                */
   AEE_TONE_RING_GS5,                /* 1661.2 Hz                                                                */
   AEE_TONE_RING_A6,                 /* 1760.0 Hz                                                                */
   AEE_TONE_RING_AS6,                /* 1864.7 Hz                                                                */
   AEE_TONE_RING_B6,                 /* 1975.5 Hz                                                                */
   AEE_TONE_RING_C6,                 /* 2093.1 Hz                                                                */
   AEE_TONE_RING_CS6,                /* 2217.4 Hz                                                                */
   AEE_TONE_RING_D6,                 /* 2349.3 Hz                                                                */
   AEE_TONE_RING_DS6,                /* 2489.1 Hz                                                                */
   AEE_TONE_RING_E6,                 /* 2637.0 Hz                                                                */
   AEE_TONE_RING_F6,                 /* 2793.7 Hz                                                                */
   AEE_TONE_RING_FS6,                /* 2959.9 Hz                                                                */
   AEE_TONE_RING_G6,                 /* 3135.9 Hz                                                                */
   AEE_TONE_RING_GS6,                /* 3322.4 Hz                                                                */
   AEE_TONE_RING_A7,                 /* 3520.0 Hz                                                                */
   AEE_TONE_RBACK,                   /* Ring back (audible ring)                                                 */
   AEE_TONE_BUSY,                    /* Busy tone                                                                */
   AEE_TONE_INTERCEPT_A,             /* First tone of an intercept                                               */
   AEE_TONE_INTERCEPT_B,             /* Second tone of an intercept                                              */
   AEE_TONE_REORDER_TONE,            /* Reorder                                                                  */
   AEE_TONE_PWRUP,                   /* Power-up tone                                                            */
   AEE_TONE_OFF_HOOK_TONE,           /* Off-hook tone, IS-95 (CAI 7.7.5.5)                                       */
   AEE_TONE_CALL_WT_TONE,            /* Call-waiting tone                                                        */
   AEE_TONE_DIAL_TONE_TONE,          /* Dial tone                                                                */
   AEE_TONE_ANSWER_TONE,             /* Answer tone                                                              */
   AEE_TONE_HIGH_PITCH_A,            /* 1st High pitch for IS-54B alerting                                       */
   AEE_TONE_HIGH_PITCH_B,            /* 2nd High pitch for IS-54B alerting                                       */
   AEE_TONE_MED_PITCH_A,             /* 1st Medium pitch for IS-54B alerting                                     */
   AEE_TONE_MED_PITCH_B,             /* 2nd Medium pitch for IS-54B alerting                                     */
   AEE_TONE_LOW_PITCH_A,             /* 1st Low pitch for IS-54B alerting                                        */
   AEE_TONE_LOW_PITCH_B,             /* 2nd Low pitch for IS-54B alerting                                        */
   AEE_TONE_TEST_ON,                 /* Test tone on                                                             */
   AEE_TONE_MSG_WAITING,             /* Message Waiting Tone                                                     */
   AEE_TONE_PIP_TONE_TONE,           /* Used for Pip-Pip-Pip-Pip (Vocoder) Tone                                  */
   AEE_TONE_SPC_DT_INDIA,            /* Used for India's Special Dial Tone                                       */
   AEE_TONE_SIGNAL_INDIA,            /* Used in Various India Signalling Tones                                   */
   AEE_TONE_DT_TONE_INDIA,           /* Used for India's Normal Dial Tone (and others)                           */
   AEE_TONE_DT_TONE_BRAZIL,          /* Used for Brazil's Dial Tone                                              */
   AEE_TONE_DT_DTACO_TONE,           /* Used for DTACO's single tone (350Hz, 350Hz)                              */
                                     // FEATURE_IDS || definedFEATURE_PHONE_VR_HFK                               
   AEE_TONE_HFK_TONE1,               /* These two tones used for Voice Activation and                            */
   AEE_TONE_HFK_TONE2,               /* Incoming Call Answer in phone VR-HFK                                     */
                                                                                                                 
   AEE_TONE_SILENCE,                 /* Control tone - SND_SILENCE                                               */
   AEE_TONE_PTT_ALERT_ARRIVAL,       /* Played when there is an incoming alert                                   */                         
   AEE_TONE_PTT_CALL_ARRIVAL,        /* Played when there is an incoming PTT call                                */
   AEE_TONE_PTT_FAIL,                /* Played when there is a generic PTT failure                               */
   AEE_TONE_PTT_FLOOR_DENY,          /* Played when a user is denied floor request in a PTT call                 */
   AEE_TONE_PTT_FLOOR_GRANT,         /* Played when a user is granted floor request in a PTT call                */
   AEE_TONE_PTT_FLOOR_INTERRUPTION,  /* Played when a user is interrupted by high-priority user in a PTT call    */
   AEE_TONE_PTT_ADD_MEMBER,          /* Played when adding member to a PTT call                                  */
   AEE_TONE_PTT_SUCCESS,             /* Played when there is a generic PTT success                               */
   AEE_TONE_PTT_TALK_ANNOUNCE,       /* Played when other user grabs the floor in a PTT call                     */
   AEE_TONE_PTT_FLOOR_AVAILABLE,     /* Played when other talker releases the floor in a PTT call                */

   AEE_TONE_RESERVED_1,              // Reserved
   AEE_TONE_RESERVED_2,              // Reserved
   AEE_TONE_RESERVED_3,              // Reserved
   AEE_TONE_RESERVED_4,              // Reserved

   AEE_TONE_LAST,                   // Deprecated
   AEE_TONE_FEEDBACK_0 = AEE_TONE_LAST, // Feedback (e.g. haptic) for 0 key
   AEE_TONE_FEEDBACK_1,             // Feedback (e.g. haptic) for 1 key
   AEE_TONE_FEEDBACK_2,             // Feedback (e.g. haptic) for 2 key
   AEE_TONE_FEEDBACK_3,             // Feedback (e.g. haptic) for 3 key
   AEE_TONE_FEEDBACK_4,             // Feedback (e.g. haptic) for 4 key
   AEE_TONE_FEEDBACK_5,             // Feedback (e.g. haptic) for 5 key
   AEE_TONE_FEEDBACK_6,             // Feedback (e.g. haptic) for 6 key
   AEE_TONE_FEEDBACK_7,             // Feedback (e.g. haptic) for 7 key
   AEE_TONE_FEEDBACK_8,             // Feedback (e.g. haptic) for 8 key
   AEE_TONE_FEEDBACK_9,             // Feedback (e.g. haptic) for 9 key
   AEE_TONE_FEEDBACK_SELECT,        // Feedback (e.g. haptic) for SELECT key
   AEE_TONE_FEEDBACK_CLR,           // Feedback (e.g. haptic) for CLR key
   AEE_TONE_FEEDBACK_POUND,         // Feedback (e.g. haptic) for # key
   AEE_TONE_FEEDBACK_STAR,          // Feedback (e.g. haptic) for * key
   AEE_TONE_FEEDBACK_UP,            // Feedback (e.g. haptic) for UP key
   AEE_TONE_FEEDBACK_DOWN,          // Feedback (e.g. haptic) for DOWN key
   AEE_TONE_FEEDBACK_LEFT,          // Feedback (e.g. haptic) for LEFT key
   AEE_TONE_FEEDBACK_RIGHT,         // Feedback (e.g. haptic) for RIGHT key
   AEE_TONE_FEEDBACK_PLAY,          // Feedback (e.g. haptic) for Play/Pause key
   AEE_TONE_FEEDBACK_FFWD,          // Feedback (e.g. haptic) for Forward key
   AEE_TONE_FEEDBACK_RWD,           // Feedback (e.g. haptic) for Rewind key
   AEE_TONE_FEEDBACK_STOP           // Feedback (e.g. haptic) for Stop key
};

#ifdef _WIN32
typedef unsigned AEESoundTone;
#else
typedef int8 AEESoundTone;
#endif

// Reserved tones.
#define AEESOUND_TONE_BEEP_ALERT       AEE_TONE_RESERVED_1
#define AEESOUND_TONE_BEEP_REMINDER    AEE_TONE_RESERVED_2
#define AEESOUND_TONE_BEEP_MSG         AEE_TONE_RESERVED_3
#define AEESOUND_TONE_BEEP_ERROR       AEE_TONE_RESERVED_4

enum {
   AEE_SOUND_STATUS_CB = 0,
   AEE_SOUND_VOLUME_CB = 1
};

#ifdef _WIN32
typedef unsigned AEESoundCmd;
#else
typedef int8 AEESoundCmd;
#endif

typedef union
{
   uint16 wVolume;
   uint16 wPlayIndex;
} AEESoundCmdData;

typedef struct _AEESoundToneData
{
   AEESoundTone   eTone;
   uint16         wDuration;
} AEESoundToneData;

typedef struct _AEESoundInfo
{
   AEESoundDevice    eDevice;
   AEESoundMethod    eMethod;
   AEESoundAPath     eAPath;
   AEESoundMuteCtl   eEarMuteCtl;
   AEESoundMuteCtl   eMicMuteCtl;
} AEESoundInfo;

// ISound callback function type
typedef void (*PFNSOUNDSTATUS)
(
   void*          pUser,
   AEESoundCmd    eCBType,
   AEESoundStatus eSPStatus,
   uint32         dwParam
);


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
// ISound Interface
//
//********************************************************************************************************************************

#define INHERIT_ISound(iname) \
   INHERIT_IBase(iname); \
   void     (*RegisterNotify)(iname* po, PFNSOUNDSTATUS pfn, const void* pUser); \
   int      (*Set)(iname* po, const AEESoundInfo* pSoundInfo); \
   void     (*Get)(iname* po, AEESoundInfo* pSoundInfo);\
   void     (*SetDevice)(iname* po);\
   void     (*PlayTone)(iname* po, AEESoundToneData toneData);\
   void     (*PlayToneList)(iname* po, AEESoundToneData* pToneData, uint16 wDataLen);\
   void     (*PlayFreqTone)(iname* po, uint16 wHiFreq, uint16 wLoFreq, uint16 wDuration);\
   void     (*StopTone)(iname* po);\
   void     (*Vibrate)(iname* po, uint16 wDuration);\
   void     (*StopVibrate)(iname* po);\
   void     (*SetVolume)(iname* po, uint16 wVolume);\
   void     (*GetVolume)(iname* po);\
   int      (*GetResourceCtl)(iname * po, IResourceCtl ** ppo) 

AEEINTERFACE_DEFINE(ISound);


// Access Macros

static __inline uint32 ISound_AddRef(ISound *p)
{
   return AEEGETPVTBL(p, ISound)->AddRef(p);
}
static __inline uint32 ISound_Release(ISound *p)
{
   return AEEGETPVTBL(p, ISound)->Release(p);
}
static __inline void ISound_RegisterNotify(ISound *p, PFNSOUNDSTATUS f, const void *u)
{
   AEEGETPVTBL(p, ISound)->RegisterNotify(p, f, u);
}
static __inline int ISound_Set(ISound *p, const AEESoundInfo *s)
{
   return AEEGETPVTBL(p, ISound)->Set(p, s);
}
static __inline void ISound_Get(ISound *p, AEESoundInfo *s)
{
   AEEGETPVTBL(p, ISound)->Get(p, s);
}
static __inline void ISound_SetDevice(ISound *p)
{
   AEEGETPVTBL(p, ISound)->SetDevice(p);
}
static __inline void ISound_PlayTone(ISound *p, AEESoundToneData t)
{
   AEEGETPVTBL(p, ISound)->PlayTone(p, t);
}
static __inline void ISound_PlayToneList(ISound *p, AEESoundToneData *t, uint16 d)
{
   AEEGETPVTBL(p, ISound)->PlayToneList(p, t, d);
}
static __inline void ISound_PlayFreqTone(ISound *p, uint16 h, uint16 l, uint16 d)
{
   AEEGETPVTBL(p, ISound)->PlayFreqTone(p, h, l, d);
}
static __inline void ISound_StopTone(ISound *p)
{
   AEEGETPVTBL(p, ISound)->StopTone(p);
}
static __inline void ISound_Vibrate(ISound *p, uint16 d)
{
   AEEGETPVTBL(p, ISound)->Vibrate(p, d);
}
static __inline void ISound_StopVibrate(ISound *p)
{
   AEEGETPVTBL(p, ISound)->StopVibrate(p);
}
static __inline void ISound_SetVolume(ISound *p, uint16 v)
{
   AEEGETPVTBL(p, ISound)->SetVolume(p, v);
}
static __inline void ISound_GetVolume(ISound *p)
{
   AEEGETPVTBL(p, ISound)->GetVolume(p);
}
static __inline int ISound_GetResourceCtl(ISound *p, IResourceCtl **po)
{
   return AEEGETPVTBL(p, ISound)->GetResourceCtl(p, po);
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEESoundAPath

Description:
AEESoundAPath is used to indicate whether or not ISound Interface should transmit
the tone over-the-air in a call

Definition:
typedef enum {
   AEE_SOUND_APATH_LOCAL,
   AEE_SOUND_APATH_TX,
   AEE_SOUND_APATH_BOTH,
   AEE_SOUND_APATH_MUTE,
   AEE_SOUND_APATH_LAST
} AEESoundAPath;

Members:

AEE_SOUND_APATH_LOCAL : DTMF on local audio
AEE_SOUND_APATH_TX : Transmit DTMFs
AEE_SOUND_APATH_BOTH : Transmit and sound DTMFs locally
AEE_SOUND_APATH_MUTE : Mute DTMFs
AEE_SOUND_APATH_LAST : Reserved


Comments:
None

See Also:
None

=============================================================================

AEESoundCmd

Description:
AEESoundCmd specifies the callback type used by ISound to send events and data to
the application

Definition:
typedef enum {
   AEE_SOUND_STATUS_CB,
   AEE_SOUND_VOLUME_CB
} AEESoundCmd;

Members:

AEE_SOUND_STATUS_CB : ISound status callback
AEE_SOUND_VOLUME_CB : Get volume callback


Comments:
None

See Also:
None

=============================================================================

AEESoundCmdData

Description:
AEESoundCmdData specifies the data sent through callback to application.

Definition:
typedef union
{
   uint16 wVolume;
   uint16 wPlayIndex;
} AEESoundCmdData;

Members:

wVolume : Volume sent through volume callback
wPlayIndex : Current tone to be played. This is sent through status callback for
PlayToneList

Comments:
None

See Also:
None

=============================================================================

AEESoundDevice

Description:
AEESoundDevice specifies the device selected and used by ISound Interface .

Definition:
typedef enum {
   AEE_SOUND_DEVICE_UNKNOWN,
   AEE_SOUND_DEVICE_HANDSET,
   AEE_SOUND_DEVICE_HFK,
   AEE_SOUND_DEVICE_HEADSET,
   AEE_SOUND_DEVICE_AHFK,
   AEE_SOUND_DEVICE_SDAC,
   AEE_SOUND_DEVICE_TTY_HFK,
   AEE_SOUND_DEVICE_TTY_HEADSET,
   AEE_SOUND_DEVICE_CURRENT,
   AEE_SOUND_DEVICE_SPEAKER,     
   AEE_SOUND_DEVICE_BT_HEADSET,  
   AEE_SOUND_DEVICE_STEREO_HEADSET,
   AEE_SOUND_DEVICE_USB,         
   AEE_SOUND_DEVICE_BT_STEREO_HEADSET,  // BlueTooth stereo headset
   AEE_SOUND_DEVICE_TTY_HCO,     //forward link = tty data, reverse link = voice
   AEE_SOUND_DEVICE_TTY_VCO,     //forward link = voice, reverse link = tty data
   AEE_SOUND_DEVICE_TTY_FULL,    //tty data both directions.
   AEE_SOUND_DEVICE_RESERVED3_START    = 89, // Reserved range #3
   AEE_SOUND_DEVICE_RESERVED3_END      = 104,
   AEE_SOUND_DEVICE_RESERVED2_START    = 105, // Reserved range #2
   AEE_SOUND_DEVICE_RESERVED2_END      = 112,
   AEE_SOUND_DEVICE_RESERVED1_START    = 113, // Reserved range #1
   AEE_SOUND_DEVICE_RESERVED1_END      = 122,
   AEE_SOUND_DEVICE_LAST
} AEESoundDevice;

Members:

AEE_SOUND_DEVICE_UNKNOWN : Unknown device
AEE_SOUND_DEVICE_HANDSET : Handset
AEE_SOUND_DEVICE_HFK : Hands Free Kit (HFK)
AEE_SOUND_DEVICE_HEADSET : Mono Headset
AEE_SOUND_DEVICE_AHFK : Analog HFK
AEE_SOUND_DEVICE_SDAC : Stereo DAC
AEE_SOUND_DEVICE_TTY_HFK : TTY HFK
AEE_SOUND_DEVICE_TTY_HEADSET : TTY Headset
AEE_SOUND_DEVICE_CURRENT : Currently selected device
AEE_SOUND_DEVICE_SPEAKER: Built-In Speaker device
AEE_SOUND_DEVICE_BT_HEADSET: BlueTooth headset
AEE_SOUND_DEVICE_STEREO_HEADSET: Stereo headset
AEE_SOUND_DEVICE_USB: USB
AEE_SOUND_DEVICE_BT_STEREO_HEADSET: BlueTooth stereo headset
AEE_SOUND_DEVICE_TTY_HCO: forward link = tty data, reverse link = voice
AEE_SOUND_DEVICE_TTY_VCO: forward link = voice, reverse link = tty data
AEE_SOUND_DEVICE_TTY_FULL: tty data both directions.
AEE_SOUND_DEVICE_RESERVED3_START: reserved device range #3 start
AEE_SOUND_DEVICE_RESERVED3_END: reserved device range #3 end (inclusive)
AEE_SOUND_DEVICE_RESERVED2_START: reserved device range #2 start
AEE_SOUND_DEVICE_RESERVED2_END: reserved device range #2 end (inclusive)
AEE_SOUND_DEVICE_RESERVED1_START: reserved device range #1 start
AEE_SOUND_DEVICE_RESERVED1_END: reserved device range #1 end (inclusive)
AEE_SOUND_DEVICE_LAST: Reserved


Comments:
None

See Also:
None

=============================================================================

AEESoundInfo

Description:
AEESoundInfo specifies the ISound attributes that are used by ISound for all its
operations.

Definition:
typedef struct
{
   AEESoundDevice eDevice;
   AEESoundMethod eMethod;
   AEESoundAPath eAPath;
   AEESoundMuteCtl eEarMuteCtl;
   AEESoundMuteCtl eMicMuteCtl;
} AEESoundInfo;

Members:
eDevice : Device used
eMethod : Method used
eAPath : Over-the-air audio path
eEarMuteCtl : Earpiece mute control
eMicMuteCtl : Microphone mute control

Comments:
None

See Also:
AEESoundDevice , AEESoundMethod ,
AEESoundAPath , AEESoundMuteCtl

=============================================================================

AEESoundMethod

Description:
AEESoundMethod specifies the method used by ISound Interface .

Definition:
typedef enum {
   AEE_SOUND_METHOD_UNKNOWN,
   AEE_SOUND_METHOD_VOICE,
   AEE_SOUND_METHOD_BEEP,
   AEE_SOUND_METHOD_MESSAGE,
   AEE_SOUND_METHOD_RING,
   AEE_SOUND_METHOD_CLICK,
   AEE_SOUND_METHOD_MIDI,
   AEE_SOUND_METHOD_AUX,
   AEE_SOUND_METHOD_RESERVED1_START = 113,
   AEE_SOUND_METHOD_RESERVED1_END   = 122,
   AEE_SOUND_METHOD_LAST
} AEESoundMethod;

Members:
AEE_SOUND_METHOD_UNKNOWN : Unknown method
AEE_SOUND_METHOD_VOICE : Use the device's voice generator
AEE_SOUND_METHOD_BEEP : Use the device's keybeep generator
AEE_SOUND_METHOD_MESSAGE : Use the device's keybeep generator
AEE_SOUND_METHOD_RING : Use the device's ring generator
AEE_SOUND_METHOD_CLICK : Use the device's click generator
AEE_SOUND_METHOD_MIDI : Use the device's Midi generator
AEE_SOUND_METHOD_AUX : Use the device's auxiliary generator if avail
AEE_SOUND_METHOD_RESERVED1_START : reserved method range start
AEE_SOUND_METHOD_RESERVED1_END  : reserved method range end (inclusive) 
AEE_SOUND_METHOD_LAST : Reserved

Comments:
None

See Also:
None

=============================================================================

AEESoundMuteCtl

Description:
AEESoundMuteCtl is used to control ear piece and microphone muting.

Definition:

enum {
   AEE_SOUND_MUTECTL_UNMUTED = 0,
   AEE_SOUND_MUTECTL_MUTED   = 1,
   AEE_SOUND_MUTECTL_LAST    = 0x7f
};

Members:
AEE_SOUND_MUTECTL_UNMUTED : Audio path is not muted
AEE_SOUND_MUTECTL_MUTED : Audio path is muted
AEE_SOUND_MUTECTL_LAST : Reserved

Comments:
None

See Also:
None

=============================================================================

AEESoundStatus

Description:
AEESoundStatus is returned in callback functions to indicate ISound events and return
data to the application.

Definition:
typedef enum {
   AEE_SOUND_UNKNOWN,
   AEE_SOUND_SUCCESS,
   AEE_SOUND_PLAY_DONE,
   AEE_SOUND_FAILURE,
   AEE_SOUND_RESOURCE_BUSY,
   AEE_SOUND_LAST
} AEESoundStatus;

Members:

AEE_SOUND_UNKNOWN : Unknown status
AEE_SOUND_SUCCESS : Request is accepted
AEE_SOUND_PLAY_DONE : Playback of the tone is either completed or overridden by
                      another tone
AEE_SOUND_FAILURE : Error occurred with this request
AEE_SOUND_RESOURCE_BUSY: Sound resource is being held by other ISound interface in the
                         same application or background application.
AEE_SOUND_LAST : Reserved


Comments:
None

See Also:
None

=============================================================================

AEESoundTone

Description:
AEESoundTone specifies the tone identifiers which are used to play tone by ISound.

Definition:
typedef enum {
   AEE_TONE_FIRST,
   AEE_TONE_0,
   AEE_TONE_1,
   AEE_TONE_2,
   AEE_TONE_3,
   AEE_TONE_4,
   AEE_TONE_5,
   AEE_TONE_6,
   AEE_TONE_7,
   AEE_TONE_8,
   AEE_TONE_9,
   AEE_TONE_A,
   AEE_TONE_B,
   AEE_TONE_C,
   AEE_TONE_D,
   AEE_TONE_POUND,
   AEE_TONE_STAR,
   AEE_TONE_CTRL,
   AEE_TONE_2ND,
   AEE_TONE_WARN,
   AEE_TONE_ERR,
   AEE_TONE_TIME,
   AEE_TONE_RING_A,
   AEE_TONE_RING_B,
   AEE_TONE_RING_C,
   AEE_TONE_RING_D,
   AEE_TONE_RING_A4,
   AEE_TONE_RING_AS4,
   AEE_TONE_RING_B4,
   AEE_TONE_RING_C4,
   AEE_TONE_RING_CS4,
   AEE_TONE_RING_D4,
   AEE_TONE_RING_DS4,
   AEE_TONE_RING_E4,
   AEE_TONE_RING_F4,
   AEE_TONE_RING_FS4,
   AEE_TONE_RING_G4,
   AEE_TONE_RING_GS4,
   AEE_TONE_RING_A5,
   AEE_TONE_RING_AS5,
   AEE_TONE_RING_B5,
   AEE_TONE_RING_C5,
   AEE_TONE_RING_CS5,
   AEE_TONE_RING_D5,
   AEE_TONE_RING_DS5,
   AEE_TONE_RING_E5,
   AEE_TONE_RING_F5,
   AEE_TONE_RING_FS5,
   AEE_TONE_RING_G5,
   AEE_TONE_RING_GS5,
   AEE_TONE_RING_A6,
   AEE_TONE_RING_AS6,
   AEE_TONE_RING_B6,
   AEE_TONE_RING_C6,
   AEE_TONE_RING_CS6,
   AEE_TONE_RING_D6,
   AEE_TONE_RING_DS6,
   AEE_TONE_RING_E6,
   AEE_TONE_RING_F6,
   AEE_TONE_RING_FS6,
   AEE_TONE_RING_G6,
   AEE_TONE_RING_GS6,
   AEE_TONE_RING_A7,
   AEE_TONE_RBACK,
   AEE_TONE_BUSY,
   AEE_TONE_INTERCEPT_A,
   AEE_TONE_INTERCEPT_B,
   AEE_TONE_REORDER_TONE,
   AEE_TONE_PWRUP,
   AEE_TONE_OFF_HOOK_TONE,
   AEE_TONE_CALL_WT_TONE,
   AEE_TONE_DIAL_TONE_TONE,
   AEE_TONE_ANSWER_TONE,
   AEE_TONE_HIGH_PITCH_A,
   AEE_TONE_HIGH_PITCH_B,
   AEE_TONE_MED_PITCH_A,
   AEE_TONE_MED_PITCH_B,
   AEE_TONE_LOW_PITCH_A,
   AEE_TONE_LOW_PITCH_B,
   AEE_TONE_TEST_ON,
   AEE_TONE_MSG_WAITING,
   AEE_TONE_PIP_TONE_TONE,
   AEE_TONE_SPC_DT_INDIA,
   AEE_TONE_SIGNAL_INDIA,
   AEE_TONE_DT_TONE_INDIA,
   AEE_TONE_DT_TONE_BRAZIL,
   AEE_TONE_DT_DTACO_TONE,
   AEE_TONE_HFK_TONE1,
   AEE_TONE_HFK_TONE2,
   AEE_TONE_SILENCE,
   AEE_TONE_PTT_ALERT_ARRIVAL,                               
   AEE_TONE_PTT_CALL_ARRIVAL,        
   AEE_TONE_PTT_FAIL,                
   AEE_TONE_PTT_FLOOR_DENY,          
   AEE_TONE_PTT_FLOOR_GRANT,         
   AEE_TONE_PTT_FLOOR_INTERRUPTION,  
   AEE_TONE_PTT_ADD_MEMBER,          
   AEE_TONE_PTT_SUCCESS,             
   AEE_TONE_PTT_TALK_ANNOUNCE,       
   AEE_TONE_PTT_FLOOR_AVAILABLE,           
   AEE_TONE_RESERVED_1,
   AEE_TONE_RESERVED_2,
   AEE_TONE_RESERVED_3,
   AEE_TONE_RESERVED_4,
   AEE_TONE_LAST,                  
   AEE_TONE_FEEDBACK_0 = AEE_TONE_LAST,            
   AEE_TONE_FEEDBACK_1,              
   AEE_TONE_FEEDBACK_2,                
   AEE_TONE_FEEDBACK_3,                  
   AEE_TONE_FEEDBACK_4,                    
   AEE_TONE_FEEDBACK_5,                      
   AEE_TONE_FEEDBACK_6,                        
   AEE_TONE_FEEDBACK_7,                          
   AEE_TONE_FEEDBACK_8,                            
   AEE_TONE_FEEDBACK_9,                              
   AEE_TONE_FEEDBACK_SELECT,                           
   AEE_TONE_FEEDBACK_CLR,                                
   AEE_TONE_FEEDBACK_POUND,                                          
   AEE_TONE_FEEDBACK_STAR,                                             
   AEE_TONE_FEEDBACK_UP,                                                 
   AEE_TONE_FEEDBACK_DOWN,                                                 
   AEE_TONE_FEEDBACK_LEFT,                                                   
   AEE_TONE_FEEDBACK_RIGHT,                                                    
   AEE_TONE_FEEDBACK_PLAY,                                                             
   AEE_TONE_FEEDBACK_FFWD,                                                                     
   AEE_TONE_FEEDBACK_RWD,                                                                        
   AEE_TONE_FEEDBACK_STOP,                                                                         
} AEESoundTone;


Members:

AEE_TONE_FIRST : Reserved
AEE_TONE_0 : DTMF for 0 key
AEE_TONE_1 : DTMF for 1 key
AEE_TONE_2 : DTMF for 2 key
AEE_TONE_3 : DTMF for 3 key
AEE_TONE_4 : DTMF for 4 key
AEE_TONE_5 : DTMF for 5 key
AEE_TONE_6 : DTMF for 6 key
AEE_TONE_7 : DTMF for 7 key
AEE_TONE_8 : DTMF for 8 key
AEE_TONE_9 : DTMF for 9 key
AEE_TONE_A : DTMF for A key
AEE_TONE_B : DTMF for B key
AEE_TONE_C : DTMF for C key
AEE_TONE_D : DTMF for D key
AEE_TONE_POUND : DTMF for # key
AEE_TONE_STAR : DTMF for * key
AEE_TONE_CTRL : Tone for a control key
AEE_TONE_2ND : Tone for secondary function on a key
AEE_TONE_WARN : Warning tone (for example, overwriting user phone# slot)
AEE_TONE_ERR : Tone to indicate an error
AEE_TONE_TIME : Time marker tone
AEE_TONE_RING_A : 1st Ringer tone
AEE_TONE_RING_B : 2nd Ringer tone
AEE_TONE_RING_C : 3rd Ringer tone
AEE_TONE_RING_D : 4th Ringer tone
AEE_TONE_RING_A4 : 440.0 Hz -Piano Notes-
AEE_TONE_RING_AS4 : 466.1 Hz
AEE_TONE_RING_B4 : 493.8 Hz
AEE_TONE_RING_C4 : 523.2 Hz
AEE_TONE_RING_CS4 : 554.3 Hz
AEE_TONE_RING_D4 : 587.3 Hz
AEE_TONE_RING_DS4 : 622.2 Hz
AEE_TONE_RING_E4 : 659.2 Hz
AEE_TONE_RING_F4 : 698.5 Hz
AEE_TONE_RING_FS4 : 739.9 Hz
AEE_TONE_RING_G4 : 784.0 Hz
AEE_TONE_RING_GS4 : 830.6 Hz
AEE_TONE_RING_A5 : 880.0 Hz
AEE_TONE_RING_AS5 : 932.2 Hz
AEE_TONE_RING_B5 : 987.7 Hz
AEE_TONE_RING_C5 : 1046.5 Hz
AEE_TONE_RING_CS5 : 1108.7 Hz
AEE_TONE_RING_D5 : 1174.6 Hz
AEE_TONE_RING_DS5 : 1244.3 Hz
AEE_TONE_RING_E5 : 1318.5 Hz
AEE_TONE_RING_F5 : 1397.0 Hz
AEE_TONE_RING_FS5 : 1479.9 Hz
AEE_TONE_RING_G5 : 1568.0 Hz
AEE_TONE_RING_GS5 : 1661.2 Hz
AEE_TONE_RING_A6 : 1760.0 Hz
AEE_TONE_RING_AS6 : 1864.7 Hz
AEE_TONE_RING_B6 : 1975.5 Hz
AEE_TONE_RING_C6 : 2093.1 Hz
AEE_TONE_RING_CS6 : 2217.4 Hz
AEE_TONE_RING_D6 : 2349.3 Hz
AEE_TONE_RING_DS6 : 2489.1 Hz
AEE_TONE_RING_E6 : 2637.0 Hz
AEE_TONE_RING_F6 : 2793.7 Hz
AEE_TONE_RING_FS6 : 2959.9 Hz
AEE_TONE_RING_G6 : 3135.9 Hz
AEE_TONE_RING_GS6 : 3322.4 Hz
AEE_TONE_RING_A7 : 3520.0 Hz
AEE_TONE_RBACK : Ring back (audible ring)
AEE_TONE_BUSY : Busy tone
AEE_TONE_INTERCEPT_A : First tone of an intercept
AEE_TONE_INTERCEPT_B : Second tone of an intercept
AEE_TONE_REORDER_TONE : Reorder
AEE_TONE_PWRUP : Power-up tone
AEE_TONE_OFF_HOOK_TONE : Off-hook tone, IS-95 (CAI 7.7.5.5)
AEE_TONE_CALL_WT_TONE : Call-waiting tone
AEE_TONE_DIAL_TONE_TONE : Dial tone
AEE_TONE_ANSWER_TONE : Answer tone
AEE_TONE_HIGH_PITCH_A : 1st High pitch for IS-54B alerting
AEE_TONE_HIGH_PITCH_B : 2nd High pitch for IS-54B alerting
AEE_TONE_MED_PITCH_A : 1st Medium pitch for IS-54B alerting
AEE_TONE_MED_PITCH_B : 2nd Medium pitch for IS-54B alerting
AEE_TONE_LOW_PITCH_A : 1st Low pitch for IS-54B alerting
AEE_TONE_LOW_PITCH_B : 2nd Low pitch for IS-54B alerting
AEE_TONE_TEST_ON : Test tone on
AEE_TONE_MSG_WAITING : Message Waiting Tone
AEE_TONE_PIP_TONE_TONE : Used for Pip-Pip-Pip-Pip (Vocoder) Tone
AEE_TONE_SPC_DT_INDIA : Used for India's Special Dial Tone
AEE_TONE_SIGNAL_INDIA : Used in Various India Signalling Tones
AEE_TONE_DT_TONE_INDIA : Used for India's Normal Dial Tone (and others)
AEE_TONE_DT_TONE_BRAZIL : Used for Brazil's Dial Tone
AEE_TONE_DT_DTACO_TONE : Used for DTACO's single tone (350Hz, 350Hz)
AEE_TONE_HFK_TONE1 : These two tones used for Voice Activation and
AEE_TONE_HFK_TONE2 : Incoming Call Answer in phone VR-HFK
AEE_TONE_SILENCE  :  Control tone - SND_SILENCE.
AEE_TONE_PTT_ALERT_ARRIVAL: Played when there is an incoming alert                         
AEE_TONE_PTT_CALL_ARRIVAL: Played when there is an incoming PTT call                              
AEE_TONE_PTT_FAIL: Played when there is a generic PTT failure                             
AEE_TONE_PTT_FLOOR_DENY: Played when a user is denied floor request in a PTT call               
AEE_TONE_PTT_FLOOR_GRANT: Played when a user is granted floor request in a PTT call              
AEE_TONE_PTT_FLOOR_INTERRUPTION: Played when a user is interrupted by high-priority user in a PTT call  
AEE_TONE_PTT_ADD_MEMBER: Played when adding member to a PTT call                                
AEE_TONE_PTT_SUCCESS: Played when there is a generic PTT success                             
AEE_TONE_PTT_TALK_ANNOUNCE: Played when other user grabs the floor in a PTT call                   
AEE_TONE_PTT_FLOOR_AVAILABLE: Played when other talker releases the floor in a PTT call
AEE_TONE_RESERVED_1: Reserved
AEE_TONE_RESERVED_2: Reserved
AEE_TONE_RESERVED_3: Reserved
AEE_TONE_RESERVED_4: Reserved
AEE_TONE_LAST : Deprecated
AEE_TONE_FEEDBACK_0:Feedback (e.g. haptic) for 0 key
AEE_TONE_FEEDBACK_1:Feedback (e.g. haptic) for 1 key
AEE_TONE_FEEDBACK_2:Feedback (e.g. haptic) for 2 key
AEE_TONE_FEEDBACK_3:Feedback (e.g. haptic) for 3 key
AEE_TONE_FEEDBACK_4:Feedback (e.g. haptic) for 4 key
AEE_TONE_FEEDBACK_5:Feedback (e.g. haptic) for 5 key
AEE_TONE_FEEDBACK_6:Feedback (e.g. haptic) for 6 key
AEE_TONE_FEEDBACK_7:Feedback (e.g. haptic) for 7 key
AEE_TONE_FEEDBACK_8:Feedback (e.g. haptic) for 8 key
AEE_TONE_FEEDBACK_9:Feedback (e.g. haptic) for 9 key
AEE_TONE_FEEDBACK_SELECT:Feedback (e.g. haptic) for SELECT key
AEE_TONE_FEEDBACK_CLR:Feedback (e.g. haptic) for CLR key
AEE_TONE_FEEDBACK_POUND:Feedback (e.g. haptic) for # key
AEE_TONE_FEEDBACK_STAR:Feedback (e.g. haptic) for * key
AEE_TONE_FEEDBACK_UP:Feedback (e.g. haptic) for UP key
AEE_TONE_FEEDBACK_DOWN:Feedback (e.g. haptic) for DOWN key
AEE_TONE_FEEDBACK_LEFT:Feedback (e.g. haptic) for LEFT key
AEE_TONE_FEEDBACK_RIGHT:Feedback (e.g. haptic) for RIGHT key
AEE_TONE_FEEDBACK_PLAY:Feedback (e.g. haptic) for Play/Pause key
AEE_TONE_FEEDBACK_FFWD:Feedback (e.g. haptic) for Forward key
AEE_TONE_FEEDBACK_RWD:Feedback (e.g. haptic) for Rewind key
AEE_TONE_FEEDBACK_STOP:Feedback (e.g. haptic) for Stop key


Comments:
None

See Also:
None

=============================================================================

AEESoundToneData

Description:
AEESoundToneData specifies tone id and duration to play the tone using the ISound_PlayTone()
and ISound_PlayToneList() .

Definition:
typedef struct
{
   AEESoundTone eTone;
   uint16 wDuration;
} AEESoundToneData ;

Members:
eTone : Tone ID
wDuration : Duration in milliseconds

Comments:
None

See Also:
AEESoundTone

=======================================================================

PFNSOUNDSTATUS

Description:
PFNSOUNDSTATUS is the type definition of the ISound callback function that applications
must register with ISound . ISound sends all the events and data to the applications
via the registered callback function.

Definition:
typedef void ( * PFNSOUNDSTATUS)
(
   void * pUser,
   AEESoundCmd eCBType,
   AEESoundStatus eSPStatus,
   uint32 dwParam
);

Members:
pUser : Application specified data pointer
eCBType : Type of callback
eSPStatus : Status within the callback type
dwParam : Pointer to AEESoundCmdData , if any. NULL otherwise.

Comments:
None

See Also:
AEESoundCmd ,
AEESoundPlayerStatus
AEESoundCmdData

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISound

Description:
The ISound Interface provides the basic sound services. These services enable playing
of beeps, rings, vibrations, various tones and list of tones. ISound defines a set
of tones that can be played for a period of time or continuously until explicitly
stopped. The tone identifiers can be put in a list and all the tones in the list
can be played using a single function call. The ISound Interface also provides functions
to get and set the volume.

To use the ISound Interface:
Create an instance of the ISound Interface with ClassID: AEECLSID_Sound

Register the status callback function using the ISound_RegisterNotify() . ISound
always communicates with applets through this callback function. The Callback function
carries information regarding user data, status callback types ( AEE_SOUND_STATUS_CB,
AEE_SOUND_VOLUME_CB ) and status (AEE_SOUND_SUCCESS , AEE_SOUND_PLAY_DONE , AEE_SOUND_FAILURE)
Data, if any, is returned in the callback through the dwParam parameter which
points to AEESoundPlayerCmdData . Otherwise dwParam is NULL. It is not always required
to register a callback function; applet developer can choose not to get any ISound
events as follows:
ISound_RegisterNotify(pISound, NULL, NULL);

Set ISound parameters using the ISound_Set() . This step is optional and ISound
assumes default values for the parameters if it is not performed.

After these steps, ISound is ready for service. the ISound function description for
more details on each function.

Tone-Database:
On the target device, device manufacturers can map elements of the AEESoundTone
type to the corresponding IDs in the tone database. For example, AEE_TONE_0 maps
to SND_0 on an MSM-based device.

===========================================================================
ISound_AddRef()

Description:
This function increments the reference count of the ISound Interface object. This
allows the object to be shared by multiple callers. The object is freed when the
reference count reaches 0 (zero). ISound_Release() .

Prototype:
uint32 ISound_AddRef(ISound * pISound)

Parameters:

pISound : Pointer to the ISound Interface object

Return Value:
Returns the incremented reference count for the object. A valid object returns a
positive reference count.

Comments:
None

Side Effects:
None

Version:
   Introduced BREW Client 1.0

See Also:
   ISound_Release()

=============================================================================


ISound_Get()

Description:
This function gets the device attributes of ISound Interface object.

Prototype:
void ISound_Get(ISound * pISound, const AEESoundInfo * pSoundInfo)

Parameters:

pISound : [in] : Pointer to ISound Interface object
pSoundInfo : [out] : Structure containing ISound device attributes

Return Value:
None

Comments:
ISound_Set() function description for details on AEESoundInfo

Side Effects:
None

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ISound_GetVolume()

Description:
This function retrieves the current volume used for the device/method pair.

Prototype:
void ISound_GetVolume(ISound * pISound)

Parameters:

pISound : Pointer to ISound Interface object

Return Value:
None

Comments:
None

Side Effects:
The volume level is sent back to the client via pointer to the AEESoundCmdData structure in the
callback function. The result includes the status as well as the current volume level. The status can
be one of the following values:

AEE_SOUND_SUCCESS:   Successful retrieving the current volume level. The dwParam of the callback function
                     is a pointer to the AEESoundCmdData structure which contains the volume.

AEE_SOUND_FAILURE:   Failed to retrieve the current volume level and the volume level is reset to 0.

The volume set is scaled to max supported levels for the (device, method) pair. So, the returned volume
corresponds to one of the current levels scaled back to BREW max (AEE_MAX_VOLUME) and may not represent 
the exact numeric value set before.

Version:
   Introduced BREW Client 1.0
See Also:
   None

=============================================================================

ISound_PlayFreqTone()

Description:
This function plays a tone given a high frequency and a low frequency for the specified
amount of time.

Prototype:
void ISound_PlayFreqTone(ISound * pISound, uint16 wHiFreq, uint16 wLoFreq, uint16
wDuration)

Parameters:

pISound : Pointer to ISound Interface object
wHiFreq : Higher frequency of the DTMF pair
wLoFreq : Lower frequency of the DTMF pair
wDuration : Tone play duration in milliseconds

Return Value:
None

Comments:
If the duration is set to 0 (zero), the tone plays until the ISound_StopTone() call
is issued. This function is not supported in the BREW Simulator but it is supported
on the target device.

Side Effects:
The result of the operation is sent to the client via the callback function pointer.
Also, when the operation is done, it notifies the client through the same callback
function pointer.
The status can be one of the following values:

AEE_SOUND_SUCCESS:   Successful playing the tone
AEE_SOUND_FAILURE:   Failed playing the tone
AEE_SOUND_PLAY_DONE: Done playing the tone or replaced by another tone

Version:
   Introduced BREW Client 1.0

See Also:
   None

=============================================================================

ISound_PlayTone()

Description:
This function plays a tone given a tone ID for the specified amount of time.

Prototype:
void ISound_PlayTone(ISound * pISound, AEESoundToneData toneData)

Parameters:

pISound : Pointer to ISound Interface object
toneData : Structure containing a ToneID and the duration in milliseconds for which
that tone is to be played

Return Value:
None

Comments:
If the duration is set to 0 (zero) for any tone in the list, the tone plays until
the ISound_StopTone() call is issued.

Side Effects:
The result of the operation is sent to the client via the callback function pointer.
Also, when the operation is done, it notifies the client through the same callback
function pointer.
The status can be one of the following values:

AEE_SOUND_SUCCESS:   Successful playing the tone
AEE_SOUND_FAILURE:   Failed to play the tone
AEE_SOUND_PLAY_DONE: Done playing the tone or replaced by another tone

Version:
   Introduced BREW Client 1.0
See Also:
   None

=============================================================================

ISound_PlayToneList()

Description:
This function plays a list of tones, given a tone ID for the specified amount of
time for each tone.

Prototype:
void ISound_PlayToneList(ISound * pISound, AEESoundToneData * pToneData, uint16
wDataLen)

Parameters:

pISound : Pointer to ISound Interface object
pToneData : List(Array) of AEESoundToneData structures, each containing ToneID and
duration in milliseconds to be played for that tone
wDataLen : Number of tones in the pToneData list

Return Value:
None.

Comments:
Call ISound_StopTone() to end the playing of tone list. If the duration is set to
0 (zero) for any tone in the list, the tone plays until the ISound_StopTone() call
is issued.

Side Effects:
The result of the operation is sent to the client, for each tone in the tone list,
via the callback function pointer. Also, when the operation is done, it notifies
the client through the same callback function pointer. The status can be one of
the following values:

AEE_SOUND_SUCCESS:   Received for each successful playing of tone in the tone list
AEE_SOUND_FAILURE:   Failed playing the tone in the tone list and tone list playback ends
AEE_SOUND_PLAY_DONE: Received for each tone, in the tone list, after the tone playback is complete or
                     replaced by another tone

Version:
   Introduced BREW Client 1.0
See Also:
   None

=============================================================================
ISound_GetResourceCtl()

Description:
   This function returns a pointer to resource control object associated with
   this interface for accessing the sound resource. By default, ISound interface
   uses the associated resource control object to acquire the sound resource in
   the ISound APIs. Applications can use this resource control object to manage
   the sound resource differently, for example an application can use it to hold
   the sound resource for a certain period of time.

Prototype:
   int ISound_GetResourceCtl(ISound * po, IResourceCtl * ppo);

Parameters:
   po [in]:  Pointer to ISound interface.
   ppo[out]: Returned resource control interface object. Filled by this method

Return Value:
   SUCCESS: Valid resource control interface object returned
   EFAILED: Failed to get the resource control interface object

Comments:
   None

Side Effects:
   If the resource control interface is retrieved, then this function increments its
   reference count. User should invoke IResourceCtl_Release() when done.

Version:
   Introduced BREW SDK 3.1

See Also:
   None

=============================================================================
ISound_RegisterNotify()

Description:
This function registers the status callback function.

Prototype:
void ISound_RegisterNotify(ISound * pISound, PFNSOUNDSTATUS pfn, const void * pUser)


Parameters:

pISound : Pointer to ISound Interface object
pfn : Status callback function pointer (can be NULL, if no callback is required)
pUser : User data for unique correlation/identification of the transaction. This
piece of information is not retrieved or processed by ISound and can be NULL if
no identifying data is required. This same data pointer is passed back to the client
along with callback status to correlate the transactions.

Return Value:
None

Comments:
The following functions and their callback status types are listed here:

ISound_PlayTone      AEE_SOUND_STATUS_CB
ISound_PlayToneList  AEE_SOUND_STATUS_CB
ISound_PlayFreqTone  AEE_SOUND_STATUS_CB
ISound_StopTone      AEE_SOUND_STATUS_CB
ISound_Vibrate       AEE_SOUND_STATUS_CB
ISound_StopVibrate   AEE_SOUND_STATUS_CB
ISound_GetVolume     AEE_SOUND_VOLUME_CB
ISound_SetVolume     AEE_SOUND_STATUS_CB

The above list also indicates what types of status are sent out through the callback
functions. The status types determine the way the data portion of the status is
interpreted. The status indicates the status of the operations or the result of
the query in case of get-operations. each of the function descriptions to find out
the effects of this registered callback function.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0
See Also:
   None

=============================================================================

ISound_Release()

Description:
This function decrements the reference count of an object. The object is freed from
memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
uint32 ISound_Release(ISound * pISound)

Parameters:

pISound : Pointer to the ISound Interface object

Return Value:

reference count : Decremented reference count for the object
0 (zero) : If the object has been freed and is no longer valid


Comments:
None

Side Effects:
None

Version:
Introduced BREW Client 1.0

See Also:
ISound_AddRef()

=============================================================================

ISound_Set()

Description:
This function sets the device attributes of ISound Interface object. The device
attributes include Device, Method, APath, Ear-piece Mute Control and Mic Mute Control.

Prototype:
int ISound_Set(ISound * pISound, const AEESoundInfo * pSoundInfo)

Parameters:

pISound : Pointer to ISound Interface object
pSoundInfo : Structure containing ISound device attributes

Return Value:

SUCCESS : ISound device attributes are set
EBADPARM : One or more AEESoundInfo parameters are wrong


Comments:
The device is not connected until you call ISound_SetDevice() .

Side Effects:
If SUCCESS is NOT returned, then ISound Interface retains the previous values. If
it is created first time it has the default values.

Version:
Introduced BREW Client 1.0

See Also:
AEESoundInfo

=============================================================================

ISound_SetDevice()

Description:
This function commands ISound to connect to a specific audio device. There is always
one, and only one, audio device connected at any particular time. The audio device
is specified by the call to the ISound_Set() function.

Prototype:
void ISound_SetDevice(ISound * pISound)

Parameters:

pISound : Pointer to ISound Interface object

Return Value:
None

Comments:
This function is not supported in BREW Simulator but it is supported on the target
device.

Side Effects:
The result of the operation is sent to the client via the callback function pointer.
The status can be one of the following values:

AEE_SOUND_SUCCESS:   Successful connecting to the new sound device
AEE_SOUND_FAILURE:   Failed to connect to the new sound device

Version:
Introduced BREW Client 1.0

See Also:
ISound_Set()

=============================================================================

ISound_SetVolume()

Description:
This function sets the volume to be used for the device/method pair specified in
the call to ISound_Set() .

Prototype:
void ISound_SetVolume(ISound * pISound, uint16 wVolume)

Parameters:

pISound : Pointer to ISound Interface object
wVolume : New volume level for device/method

Return Value:
None

Comments:
The range of volume is from 0 (zero, lowest) to AEE_MAX_VOLUME (highest). In the
BREW Simulator, the volume increases linearly. This may not be the case on target
device.

Side Effects:
The result of the operation is sent to the client via the callback function pointer.
The result can be one of the following:

AEE_SOUND_SUCCESS:   Successful setting the new volume level
AEE_SOUND_FAILURE:   Failed setting the new volume level; the old level still prevails

Version:
Introduced BREW Client 1.0

Version:
Introduced BREW Client 1.0

See Also:
ISound_Set()

=============================================================================

ISound_StopTone()

Description:
This function stops playing current tone or ends the playback of tone list.

Prototype:
void ISound_StopTone(ISound * pISound)

Parameters:

pISound : Pointer to ISound Interface object

Return Value:
None.

Comments:
In BREW Simulator, this function stops the vibration tone, if vibration is active.

Side Effects:
The result of the operation is sent to the client, via the callback function pointer.
Also, when the operation is done, it notifies the client through the same callback
function pointer. The status can be one of the following values:

AEE_SOUND_SUCCESS:   Successful and accepted the stop command. If playback of a tone or tone list is
                     active, then it also generates AEE_SOUND_PLAY_DONE callback
AEE_SOUND_FAILURE:   Failed and did not accept the stop command

Version:
Introduced BREW Client 1.0

See Also:
ISound_Vibrate()

=============================================================================

ISound_StopVibrate()

Description:
This stops the current vibration.

Prototype:
void ISound_StopVibrate(ISound * pISound)

Parameters:

pISound : Pointer to ISound Interface object

Return Value:
None

Comments:
In the BREW Simulator, this function is the same as calling ISound_StopTone() . As
a result, it may stop an ongoing ISound_PlayTone() playback.

Side Effects:
This function does not cause any callback.

Version:
Introduced BREW Client 1.0

See Also:
ISound_Vibrate()

=============================================================================

ISound_Vibrate()

Description:
This function causes the device to vibrate for the specified amount or until ISound_StopVibrate()
is called.

Prototype:
void ISound_Vibrate(ISound * pISound, uint16 w6Duration)

Parameters:

pISound : Pointer to the public ISound object
wDuration : Duration of vibration in milliseconds

Return Value:
None

Comments:
In the BREW Simulator, this function is the same as calling ISound_PlayTone() with
a sample tone. As a result, it may stop ongoing ISound_PlayTone() playback or vice
versa.

Side Effects:
This function does not cause any callback.

Version:
Introduced BREW Client 1.0

See Also:
ISound_StopVibrate()

==============================================================================*/

#endif // AEEISOUND_H

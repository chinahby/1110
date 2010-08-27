#ifndef AEEIMEDIA_H
#define AEEIMEDIA_H

/*============================================================================
FILE: AEEIMedia.h

SERVICES:  BREW MultiMedia Services

DESCRIPTION:
   This file defines an abstract interface for BREW MultiMedia format. All
   media formats like MIDI, MP3, QCP must be derived from this class.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999,2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
//==========================================================================
//   Defines
//==========================================================================
#define AEEIID_IMedia   0x01005500     // This is AEECLSID_MEDIA

//==========================================================================
//    Include Files
//==========================================================================
#include "AEEISound.h"
#include "AEEIBitmap.h"
#include "AEEISource.h" /* for AEEIID_ISource */

//==========================================================================
//   Type definitions
//==========================================================================
//
// Command codes
//
//
#define MM_CMD_BASE           1        // Command base
#define MM_CMD_USER_BASE      0x100    // Command base for derived class

#define MM_CMD_SETMEDIAPARM   (MM_CMD_BASE + 1)    // SetMediaParm()
#define MM_CMD_GETMEDIAPARM   (MM_CMD_BASE + 2)    // GetMediaParm()
#define MM_CMD_PLAY           (MM_CMD_BASE + 3)    // Play()
#define MM_CMD_RECORD         (MM_CMD_BASE + 4)    // Record()
#define MM_CMD_GETTOTALTIME   (MM_CMD_BASE + 5)    // GetTotalTime()

//
// Status codes
//
//
#define MM_STATUS_BASE              1     // Status
#define MM_STATUS_USER_BASE         0x100 // Status base for derived class

#define MM_STATUS_START             (MM_STATUS_BASE)        // [Play/Rec] Playback/Recording has started
#define MM_STATUS_DONE              (MM_STATUS_BASE + 1)    // [All cmd] Command completed
#define MM_STATUS_ABORT             (MM_STATUS_BASE + 2)    // [All cmd] Command aborted
#define MM_STATUS_MEDIA_SPEC        (MM_STATUS_BASE + 3)    // [Play/Rec] Media specifications
#define MM_STATUS_TICK_UPDATE       (MM_STATUS_BASE + 4)    // [Play/Rec] Playback/Recording tick update
#define MM_STATUS_DATA_IO_DELAY     (MM_STATUS_BASE + 5)    // [Play/Rec] Playback/Recording being delayed by data i/o access
#define MM_STATUS_SEEK              (MM_STATUS_BASE + 6)    // [Play/Rec] Playback/Recording seek done
#define MM_STATUS_SEEK_FAIL         (MM_STATUS_BASE + 7)    // [Play/Rec] Playback/Recording seek cmd failed
#define MM_STATUS_PAUSE             (MM_STATUS_BASE + 8)    // [Play/Rec] Playback/Recording paused
#define MM_STATUS_PAUSE_FAIL        (MM_STATUS_BASE + 9)    // [Play/Rec] Playback/Recording pause cmd failed
#define MM_STATUS_RESUME            (MM_STATUS_BASE + 10)   // [Play/Rec] Playback/Recording resumed
#define MM_STATUS_RESUME_FAIL       (MM_STATUS_BASE + 11)   // [Play/Rec] Playback/Recording resume cmd failed
#define MM_STATUS_REPEAT            (MM_STATUS_BASE + 12)   // [Play] Playback repeating
#define MM_STATUS_SPACE_WARNING     (MM_STATUS_BASE + 13)   // [Rec] Memory available to store recording running low
#define MM_STATUS_SPACE_ERROR       (MM_STATUS_BASE + 14)   // [Rec] Memory unavailable to store recording; recording ended
#define MM_STATUS_UNKNOWN           (MM_STATUS_BASE + 15)   // [All cmd] Unknown status
#define MM_STATUS_FRAME             (MM_STATUS_BASE + 16)   // [Play/Rec] Frame callback. Call GetMediaParm(MM_PARM_FRAME) to get the frame.
#define MM_STATUS_SYNC              (MM_STATUS_BASE + 17)   // [Play/Rec] Sync indication. Allows external objects to sync with the media playback
#define MM_STATUS_AUDIOSYNC         (MM_STATUS_BASE + 17)   // [Play/Rec] AV Sync indication. Users can sync their audio/video playback

#define MM_STATUS_RESERVED_MEDIAPLAYER_1     (MM_STATUS_BASE + 0xA0) // Reserved range from 0xA0-0xBF for IMediaPlayer
#define MM_STATUS_RESERVED_MEDIAPLAYER_32    (MM_STATUS_BASE + 0xBF)  
#define MM_STATUS_RESERVED_MEDIARECORDER_1   (MM_STATUS_BASE + 0xC0) // Reserved range from 0xA0-0xBF for IMediaRecorder
#define MM_STATUS_RESERVED_MEDIARECORDER_16  (MM_STATUS_BASE + 0xCF)  

#define MM_STATUS_RESERVED_BROWSER_1  (MM_STATUS_BASE + 0xEF)  // Reserved range from 0xF0
#define MM_STATUS_RESERVED_BROWSER_16 (MM_STATUS_BASE + 0xFE)  // to 0xFF

//
// Media Control Parms
//
// Set/Get parms using SetMediaParm()/GetMediaParm().
// Setting/Getting media parms may result in async callback events. If
// this happens, then function will return MM_PENDIG status.
// In the callback, AEEMediaCmdNotify struct contains the following info:
//    nCmd     = MM_CMD_SETMEDIAPARM/MM_CMD_GETMEDIAPARM
//    nSubCmd  = MM_PARM_XXXX (defined below)
//    nStatus  = MM_STATUS_DONE/MM_STATUS_ABORT
//    pCmdData = <context sensitive data> for MM_STATUS_DONE / NULL for MM_STATUS_ABORT
//    dwSize   = size of pCmdData for MM_STATUS_DONE / 0 for MM_STATUS_ABORT
//
#define MM_PARM_BASE             1        // Parms base
#define MM_PARM_USER_BASE        0x100    // Parms base for derived class

#define MM_PARM_MEDIA_DATA       (MM_PARM_BASE)       // [Get/Set] Media data, p1 = AEEMediaData *
#define MM_PARM_AUDIO_DEVICE     (MM_PARM_BASE + 1)   // [Get/Set] Audio device, p1 = AEESoundDevice
#define MM_PARM_AUDIO_PATH       (MM_PARM_BASE + 2)   // [Get/Set] Audio path, p1 = MM_APATH_XXX,
                                                      //           (Ringer only)p2 = SilenceTimerMS (1000 ms default), silence in between playback. = 0, means no repeat.
#define MM_PARM_VOLUME           (MM_PARM_BASE + 3)   // [Get/Set] Volume ctl, p1 = volume (0 - AEE_MAX_VOLUME)
#define MM_PARM_MUTE             (MM_PARM_BASE + 4)   // [Get/Set] Mute Ctl, p1 = 1 to mute, 0 to unmute.

#define MM_PARM_TEMPO            (MM_PARM_BASE + 5)   // [Get/Set] Tempo ctl, p1 = tempo factor. Percent: 100 (default).
#define MM_PARM_TUNE             (MM_PARM_BASE + 6)   // [Get/Set] Tune ctl, p1 = tune factor. Values: 0x34, 0x3f, 0x40(default), 0x41, 0x4c
#define MM_PARM_PAN              (MM_PARM_BASE + 7)   // [Get/Set] Tune ctl, p1 = pan factor (0 - MM_MAX_PAN).

#define MM_PARM_TICK_TIME        (MM_PARM_BASE + 8)   // [Get/Set] Periodicity of MM_TICK_UPDATE event,
                                                      //           p1 = 1000 (default); = 0 no event sent.
#define MM_PARM_RECT             (MM_PARM_BASE + 9)   // [Get/Set]. Video/Image and Text rects. p1 = Dest Video/Image AEERect *, p2 = Dest Text AEERect *
#define MM_PARM_PLAY_REPEAT      (MM_PARM_BASE + 10)  // [Get/Set] Play repeat count,
                                                      //           p1 = 1 play once (default); = 0 play forever; = n>1 play n times
#define MM_PARM_POS              (MM_PARM_BASE + 11)  // [Get] Current play/rec position (including direction). p1[In] = seek mode, p1[out] = Cur pos in ms, p2 = Cur pos in seek mode
                                                      // [Set] Set play/rec start/stop pos: p1 = [seek mode (msb)] | [0/1 => start/stop (lsb)], p2 = start/stop pos
#define MM_PARM_CLSID            (MM_PARM_BASE + 12)  // [Get] Class ID, p1 = Class ID
#define MM_PARM_CAPS             (MM_PARM_BASE + 13)  // [Get] Capabilities. p1 = ORed MM_CAPS_XXX, p2 = Media specific caps struct *
#define MM_PARM_ENABLE           (MM_PARM_BASE + 14)  // [Get/Set] Enable/Disable capabilities. p1 = ORed MM_CAPS_XXX
#define MM_PARM_CHANNEL_SHARE    (MM_PARM_BASE + 15)  // [Get/Set] Enable/Disable sharing the channel for playback
#define MM_PARM_FRAME            (MM_PARM_BASE + 16)  // [Get] Returns current frame as IBitmap. pi = IBitmap *
                                                      // [Set] Enables/Disables MM_STATUS_FRAME callback. p1 = TRUE/FALSE => Enable/Disable. Default: Disabled
#define MM_PARM_RESERVED_1       (MM_PARM_BASE + 17)  // Reserved -- used in AEEMediaDecrypt.h
#define MM_PARM_RESERVED_2       (MM_PARM_BASE + 18)  // Reserved -- used in AEEMediaDecrypt.h

#define MM_PARM_SEEK_CAPS        (MM_PARM_BASE + 19)  // [Get] Get seek caps -- the supported seek modes. p1 = list of supported modes, p2 = byte count
#define MM_PARM_RATE             (MM_PARM_BASE + 20)  // [Get/Set] Set play speed. p1= play rate: 16-bit LSB is numerator, 16-bit MSB is denominator
#define MM_PARM_PLAY_TYPE        (MM_PARM_BASE + 21)  // [Get/Set] Set play type. p1 = MM_PLAY_TYPE_XXX
#define MM_PARM_AUDIOSYNC        (MM_PARM_BASE + 22)  // [Get/Set] Set AudioSync callback info. p1=MM_AUDIOSYNC_MODE_XXX, p2=Callback TimeMS/SampleRate
#define MM_PARM_NOTES            (MM_PARM_BASE + 23)  // [Get/Set] Set the number of simultaneously voiced notes required by this media object. 
                                                      //           p1=uint32, specifies the number of notes. 

#define MM_PARM_RESERVED_MEDIAPLAYER_1    (MM_PARM_BASE + 0xA0)  // Reserved range from 0xA0-0xBF for IMediaPlayer
#define MM_PARM_RESERVED_MEDIAPLAYER_32   (MM_PARM_BASE + 0xBF)  

#define MM_PARM_RESERVED_BROWSER_1  (MM_STATUS_BASE + 0xEF) // Reserved range from 0xF0
#define MM_PARM_RESERVED_BROWSER_16 (MM_STATUS_BASE + 0xFE) // to 0xFF

//
// Media States
//
#define MM_STATE_BASE            1        // States base
#define MM_STATE_USER_BASE       0x100    // States base used by derived class

#define MM_STATE_IDLE            (MM_STATE_BASE)      // Initial state. Media needs to acquire AEEMediaData
#define MM_STATE_READY           (MM_STATE_BASE + 1)  // Ready state. Media acquired AEEMediaData
#define MM_STATE_PLAY            (MM_STATE_BASE + 2)  // Playback in progress
#define MM_STATE_RECORD          (MM_STATE_BASE + 3)  // Recording in progress
#define MM_STATE_PLAY_PAUSE      (MM_STATE_BASE + 4)  // Playback paused
#define MM_STATE_RECORD_PAUSE    (MM_STATE_BASE + 5)  // Recording paused

//
// Media Audio Path
// This specifies destination(s) for playback or source for recording.
//
#define MM_APATH_BASE            1        // AudioPath base
#define MM_APATH_USER_BASE       0x100    // AudioPath base used by derived class

#define MM_APATH_LOCAL           (MM_APATH_BASE)      // Playback to: ear-piece, speaker, etc. Record from: Mic
#define MM_APATH_LOCAL_RINGER    (MM_APATH_BASE + 1)  // Playback to: Ringer
#define MM_APATH_REMOTE          (MM_APATH_BASE + 2)  // Playback to: ReverseLink(over-the-air). Record from: ForwardLink.
#define MM_APATH_BOTH            (MM_APATH_BASE + 3)  // Playback [to]: Both MM_APATH_LOCAL and MM_APATH_TX.

#define MM_MAX_PAN               128   // Range: 0(Left) < MM_MAX_PAN/2(Balanced - default) < MM_MAX_PAN(Right)

//
// Media capabilities
//
#define MM_CAPS_NONE             0x00000000
#define MM_CAPS_AUDIO            0x00000001  // Media can have audio component
#define MM_CAPS_VIDEO            0x00000002  // Media can have video component
#define MM_CAPS_TEXT             0x00000004  // Media can have text component
#define MM_CAPS_SYNC             0x00000008  // Media allows external entities to sync with media playback

// Note: In Seek() API,
// - Lower 4 bits of eSeek refers to seek reference (MM_SEEK_START,..)
// - Upper 4 bits of eSeek refers to seek mode (MM_SEEK_MODE,..)
enum {
   MM_SEEK_START = 0,
   MM_SEEK_END,
   MM_SEEK_CURRENT
};
#ifdef _WIN32
typedef unsigned AEEMediaSeek;
#else
typedef int8 AEEMediaSeek;
#endif

#define MM_SEEK_MODE_TIME     0x00
#define MM_SEEK_MODE_FRAME    0x10
#define MM_SEEK_MODE_CHAPTER  0x20

//
// Play type
//
#define MM_PLAY_TYPE_BASE            1        // Base 
#define MM_PLAY_TYPE_USER_BASE       0x100    // Base used by derived class

#define MM_PLAY_TYPE_NORMAL      (MM_PLAY_TYPE_BASE)  // Normal media playback
#define MM_PLAY_TYPE_RINGER      (MM_PLAY_TYPE_BASE + 1)  // Media playback as ringer
#define MM_PLAY_TYPE_REMINDER    (MM_PLAY_TYPE_BASE + 2)  // Media playback as reminder

//
// AudioSync callback type.
//
#define MM_AUDIOSYNC_MODE_BASE         1        // Base 
#define MM_AUDIOSYNC_MODE_USER_BASE    0x100    // Base used by derived class

#define MM_AUDIOSYNC_MODE_TIME         (MM_AUDIOSYNC_MODE_BASE)       // AudioSync callback based on time
#define MM_AUDIOSYNC_MODE_SAMPLE       (MM_AUDIOSYNC_MODE_BASE + 1)   // AudioSync callback based on samples

// 
// AudioSync info.
// When MM_STATUS_AUDIOSYNC callback occurs, AEEMediaCmdNotify::pData points to this structure.
//
typedef struct AEEMediaAudioSync
{
   uint64   ullTimeStampMS;   // Time stamp in ms of when samples are rendered
   uint64   ullSamples;       // Number of samples rendered so far
} AEEMediaAudioSync;


typedef struct IMedia   IMedia;

// AEEMediaCmdNotify structure is returned via registered callback function to client.
// Value of pCmdData depends on combination of (clsMedia, nCmd, nSubCmd, nStatus).
// For IMedia class, it can be one of the following:
//    uint32      dwElapsedTime;       // [MM_CMD_PLAY/MM_CMD_REC, MM_STATUS_TICK_UPDATE] Milliseconds
//    uint32      dwTotalTime;         // [MM_CMD_GETTOTALTIME, MM_STATUS_DONE] Milliseconds
//    uint16      nTempo;              // [MM_CMD_SETMEDIAPARM, MM_STATUS_TEMPO] Percent: 100 (default)
//    uint16      wTune;               // [MM_CMD_SETMEDIAPARM, MM_STATUS_TUNE] Values: 0x34, 0x3f, 0x40, 0x41, 0x4c
//    uint16      nPan;                // [MM_CMD_SETMEDIAPARM, MM_STATUS_PAN] Range: 0(Left) < 64(Balanced - default) < 128(Right)
//    uint16      wVolume;             // [MM_CMD_GETMEDIAPARM, nSubCmd=MM_PARM_VOLUME, MM_STATUS_VOLUME] 0 - AEE_MAX_VOLUME (100)
typedef struct AEEMediaCmdNotify
{
   AEECLSID    clsMedia;   // CLSID of IMedia concrete class
   IMedia *    pIMedia;    // Pointer to IMedia

   int         nCmd;       // Command code
   int         nSubCmd;    // Sub command code, if any or 0
   int         nStatus;    // Status code
   void *      pCmdData;   // Contains one of the values listed above
   uint32      dwSize;     // Size of pCmdData
} AEEMediaCmdNotify;

typedef void (*PFNMEDIANOTIFY)(void * pUser, AEEMediaCmdNotify * pCmdNotify);


//-------------------------------------------------------------------
// AEEMediaData related
//-------------------------------------------------------------------
//
// Source/sink type
//
#define MMD_BASE              0
#define MMD_FILE_NAME         (MMD_BASE)        // pData = File name only
#define MMD_BUFFER            (MMD_BASE + 1)    // pData = Memory buffer containing entire data
#define MMD_ISOURCE           AEEIID_ISource    // pData = ISource * provides real-time streaming data

//
// This struct defines source/sink type, context sensitive data
// associated with media data.
// Following table gives details of context sensitive data for pre-defined
// source/sink types:
// clsData           Mode        pData          dwSize
// ------------------------------------------------------------
// MMD_FILE_NAME     Read/Write  File name      0
// MMD_BUFFER:       Read/Write  Buffer ptr     data size or 0
// MMD_ISOURCE:      Read only   ISource *      data size or 0
//
// Note: (1) For playback, cls can be set to CLSID of any ISource-based class with
//           pData set to the corresponding interface pointer.
//       (2) For recording, only MMD_FILE_NAME/MMD_BUFFER types are allowed. Existing
//           file or memory buffer will be overwritten.
//
typedef struct AEEMediaData
{
   AEECLSID    clsData;    // Type of data source/sink
   void *      pData;      // Context sensitive data (see table above)
   uint32      dwSize;     // Context sensitive data (see table above)
} AEEMediaData;

typedef struct AEEMediaDataEx
{
   AEECLSID    clsData;       // Type of data source/sink
   void *      pData;         // Context sensitive data (see table above)
   uint32      dwSize;        // Size of context sensitive data (see table above)

   uint32      dwStructSize;  // Size of this structure
   uint32      dwCaps;        // Enabled caps. Zero means all caps enabled
   unsigned    bRaw:1;        // TRUE if data is in raw format
   void *      pSpec;         // [Typically applies to raw format] Media spec struct/info
   uint32      dwSpecSize;    // Size of media spec
   uint32      dwBufferSize;  // [Applies to streaming only] Size of streaming buffer to be 
                              // maintained by IMedia object. Zero means use default
} AEEMediaDataEx;

//
// DEPRECATED DEFINES: DO NOT USE THE FOLLOWING DEFINES.
//
#define MM_FMT_MIDI           AEECLSID_MEDIAMIDI      // Standard MIDI Format (SMF)
#define MM_FMT_MP3            AEECLSID_MEDIAMP3       // MP3
#define MM_FMT_QCP            AEECLSID_MEDIAQCP       // QCP(PureVoice)
#define MM_FMT_PMD            AEECLSID_MEDIAPMD       // PMD(WebAudio)
#define MM_FMT_MIDIOUTMSG     AEECLSID_MEDIAMIDIOUTMSG// MIDI messages (Real-Time MIDI)
#define MM_FMT_MIDIOUTQCP     AEECLSID_MEDIAMIDIOUTQCP// QCP(PureVoice) in foreground thru MIDI device
#define MM_FMT_MPEG4          AEECLSID_MEDIAMPEG4     // MPEG4
#define MM_FMT_MMF            AEECLSID_MEDIAMMF       // Yahama MMF

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
// IMedia Interface
//
//********************************************************************************************************************************

#define INHERIT_IMedia(iname) \
   INHERIT_IQI(iname);\
   \
   int               (*RegisterNotify)(iname * po, PFNMEDIANOTIFY pfnNotify, void * pUser);\
   \
   int               (*SetMediaParm)(iname * po, int nParamID, int32 p1, int32 p2);\
   int               (*GetMediaParm)(iname * po, int nParamID, int32 * pP1, int32 * pP2);\
   \
   int               (*Play)(iname * po);\
   int               (*Record)(iname * po);\
   \
   int               (*Stop)(iname * po);\
   int               (*Seek)(iname * po, AEEMediaSeek eSeek, int32 lSeekValue);\
   int               (*Pause)(iname * po);\
   int               (*Resume)(iname * po);\
   \
   int               (*GetTotalTime)(iname * po);\
   \
   int               (*GetState)(iname * po, boolean * pbStateChanging)

AEEVTBL_DEFINE(IMedia);

static __inline uint32 IMedia_AddRef(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->AddRef(p);
}
static __inline uint32 IMedia_Release(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Release(p);
}
static __inline int IMedia_QueryInterface(IMedia *p, AEEIID i, void **p2)
{
   return AEEGETPVTBL(p, IMedia)->QueryInterface(p, i, p2);
}

static __inline int IMedia_RegisterNotify(IMedia *p, PFNMEDIANOTIFY f, void *pd)
{
   return AEEGETPVTBL(p, IMedia)->RegisterNotify(p, f, pd);
}

static __inline int IMedia_SetMediaParm(IMedia *p, int c, int32 p1, int32 p2)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm(p, c, p1, p2);
}
static __inline int IMedia_GetMediaParm(IMedia *p, int c, int32 *pp1, int32 *pp2)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm(p, c, pp1, pp2);
}

static __inline int IMedia_Play(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Play(p);
}
static __inline int IMedia_Record(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Record(p);
}
static __inline int IMedia_Stop(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Stop(p);
}

static __inline int IMedia_Seek(IMedia *p, AEEMediaSeek s, int32 t)
{
   return AEEGETPVTBL(p, IMedia)->Seek(p, s, t);
}
static __inline int IMedia_Rewind(IMedia *p, int32 t)
{
   return AEEGETPVTBL(p, IMedia)->Seek(p, MM_SEEK_CURRENT, (t)*(-1));
}
static __inline int IMedia_FastForward(IMedia *p, int32 t)
{
   return AEEGETPVTBL(p, IMedia)->Seek(p, MM_SEEK_CURRENT, t);
}
static __inline int IMedia_SeekFrame(IMedia *p, AEEMediaSeek s, int32 t)
{
   return AEEGETPVTBL(p, IMedia)->Seek(p, MM_SEEK_MODE_FRAME | s, t);
}

static __inline int IMedia_Pause(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Pause(p);
}
static __inline int IMedia_Resume(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->Resume(p);
}

static __inline int IMedia_GetTotalTime(IMedia *p)
{
   return AEEGETPVTBL(p, IMedia)->GetTotalTime(p);
}

static __inline int IMedia_GetState(IMedia *p, boolean *pb)
{
   return AEEGETPVTBL(p, IMedia)->GetState(p, pb);
}

// Commonly used API to set/get IMedia parm
static __inline int IMedia_SetMediaDataEx(IMedia *p, AEEMediaDataEx * pmd, int32 n)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_MEDIA_DATA, (int32)(pmd), (int32)(n));
}
static __inline int IMedia_SetMediaData(IMedia *p, AEEMediaData * pmd)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_MEDIA_DATA, (int32)(pmd), (int32)(0));
}
static __inline int IMedia_GetMediaData(IMedia *p,  AEEMediaData * pmd)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_MEDIA_DATA, (int32 *)(pmd), NULL);
}

static __inline int IMedia_SetAudioDevice(IMedia *p, AEESoundDevice eDevice)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_AUDIO_DEVICE, (int32)(eDevice), 0);
}

static __inline int IMedia_SetAudioPath(IMedia * p, int nAPath, uint32 dwMS)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_AUDIO_PATH, (int32)(nAPath), (int32)(dwMS));
}
static __inline int IMedia_SetMuteCtl(IMedia * p, boolean bMute)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_MUTE, (int32)(bMute), 0);
}

static __inline int IMedia_SetVolume(IMedia * p, uint16 wVolume)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_VOLUME, (int32)(wVolume), 0);
}

static __inline int IMedia_GetVolume(IMedia *p, uint16 * pwVol)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_VOLUME, (int32 *)(pwVol), NULL);
}

static __inline int IMedia_SetPan(IMedia * p, uint16 wPan)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_PAN, (int32)(wPan), 0);
}
static __inline int IMedia_SetTickTime(IMedia * p, uint32 dwTickMS)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_TICK_TIME, (int32)(dwTickMS), 0);
}

static __inline int IMedia_SetRect(IMedia *p, AEERect* prImage, AEERect* prText)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_RECT, (int32)(prImage), (int32)(prText));
}

static __inline int IMedia_GetRect(IMedia *p, AEERect *prImage, AEERect *prText)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_RECT, (int32 *)(prImage), (int32 *)(prText));
}

static __inline int IMedia_GetClassID(IMedia *p, AEECLSID * pCls)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_CLSID, (int32 *)(pCls), 0);
}

static __inline int IMedia_EnableChannelShare(IMedia * p, boolean bEnable)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_CHANNEL_SHARE, (int32)(bEnable), 0);
}
static __inline int IMedia_IsChannelShare(IMedia * p, boolean * pb)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_CHANNEL_SHARE, (int32 *)(pb), NULL);
}
static __inline int IMedia_EnableFrameCallback(IMedia * p, boolean b)
{
   return AEEGETPVTBL(p, IMedia)->SetMediaParm((IMedia *)(p), MM_PARM_FRAME, (int32)(b), 0);
}
static __inline int IMedia_GetFrame(IMedia * p, IBitmap ** ppFrame)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_FRAME, (int32 *)(ppFrame), NULL);
}
static __inline int IMedia_IsFrameCallback(IMedia * p, boolean * pb)
{
   return AEEGETPVTBL(p, IMedia)->GetMediaParm((IMedia *)(p), MM_PARM_FRAME, NULL, (int32 *)(pb));
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEMediaCmdNotify

Description:
This structure is contains information regarding the event
returned by IMedia via application-registered callback.

Definition:
   typedef struct
   {
      AEECLSID    clsMedia;
      IMedia *    pIMedia;

      int         nCmd;
      int         nSubCmd;
      int         nStatus;
      void *      pCmdData;
      uint32      dwSize;
   } AEEMediaCmdNotify;

Members:
      clsMedia:   CLSID of IMedia concrete class
      pIMedia:    Pointer to IMedia
      nCmd:       Command code
      nSubCmd:    Sub command code, if any or 0
      nStatus:    Status code
      pCmdData:   Contains one of the values listed above
      dwSize:     Size of pCmdData

Comments:
The following table gives the possible events that contain commands and sub-commands,
status, and context-sensitive data.
===pre>
  nCmd               nSubCmd        nStatus                    pData [optional]
---------------------------------------------------------------------------------
MM_CMD_SETMEDIAPARM  MM_PARM_XXX    MM_STATUS_DONE             <Depends on parm>
                                    MM_STATUS_ABORT

MM_CMD_GETMEDIAPARM  MM_PARM_XXX    MM_STATUS_DONE             <Depends on parm>
                                    MM_STATUS_ABORT

MM_CMD_PLAY          0              MM_STATUS_START
                                    MM_STATUS_DONE
                                    MM_STATUS_ABORT
                                    MM_STATUS_MEDIA_SPEC       [Ptr to MediaSpec]
                                    MM_STATUS_TICK_UPDATE
                                    MM_STATUS_DATA_IO_DELAY    Elapsed Time in MS
                                    MM_STATUS_SEEK
                                    MM_STATUS_SEEK_FAIL
                                    MM_STATUS_PAUSE            Elapsed Time in MS
                                    MM_STATUS_PAUSE_FAIL
                                    MM_STATUS_RESUME           Elapsed Time in MS
                                    MM_STATUS_RESUME_FAIL
                                    MM_STATUS_REPEAT           [Elapsed Time in MS]
                                    MM_STATUS_FRAME

MM_CMD_RECORD        0              MM_STATUS_START
                                    MM_STATUS_DONE
                                    MM_STATUS_ABORT
                                    MM_STATUS_MEDIA_SPEC       [Ptr to MediaSpec]
                                    MM_STATUS_TICK_UPDATE
                                    MM_STATUS_DATA_IO_DELAY    Elapsed Time in MS
                                    MM_STATUS_SEEK
                                    MM_STATUS_SEEK_FAIL
                                    MM_STATUS_PAUSE            Elapsed Time in MS
                                    MM_STATUS_PAUSE_FAIL
                                    MM_STATUS_RESUME           Elapsed Time in MS
                                    MM_STATUS_RESUME_FAIL
                                    MM_STATUS_FRAME
                                    MM_STATUS_SPACE_WARNING
                                    MM_STATUS_SPACE_ERROR


MM_CMD_GETTOTALTIME   0             MM_STATUS_DONE             Total Time in MS
                                    MM_STATUS_ABORT
===/pre>

See Also:
   PFNMEDIANOTIFY,
   IMedia_SetMediaParm()
   IMedia_GetMediaParm()
   IMedia_Play()
   IMedia_Record()
   IMedia_GetTotalTime()

=============================================================================

AEEMediaData

Description:
This struct defines media data source/sink type, and context-sensitive data
associated with media data type.

Definition:
   typedef struct
   {
      AEECLSID             clsData;    // Type of media data type
      void *               pData;      // Context sensitive data (see table above)
      uint32               dwSize;     // Context sensitive data (see table above)
   } AEEMediaData;

Members:
   clsData:  Type of media data
   pData     Context sensitive data
   dwSize    Context sensitive data

Comments:

The following table gives details of context-sensitive data for predefined
media data types [Read/Write means Playback/Record].

===pre>

clsData           Mode        pData          dwSize
------------------------------------------------------------
MMD_FILE_NAME     Read/Write  File name      0
MMD_BUFFER:       Read/Write  Buffer ptr     data size or 0
MMD_ISOURCE:      Read only   ISource *      data size or 0

===/pre>

Note:
(1) For playback, cls can be set to CLSID of any ISource-based class with
pData set to the corresponding interface pointer.

(2) For recording, only MMD_FILE_NAME/MMD_BUFFER types are allowed. Existing
file or memory buffer will be overwritten.

See Also:
   None.
=============================================================================

AEEMediaDataEx

Description:
This struct is derived from AEEMediaData and enhances the media data specification.

Definition:
   typedef struct AEEMediaDataEx
   {
      AEECLSID    clsData;    
      void *      pData;      
      uint32      dwSize;     

      uint32      dwStructSize; 
      uint32      dwCaps;       
      unsigned    bRaw:1;       
      void *      pSpec;        
      uint32      dwSpecSize;   
      uint32      dwBufferSize; 
                                
   } AEEMediaDataEx;

Members:
   clsData:  Type of media data
   pData:    Context sensitive data
   dwSize:   Context sensitive data

   dwStructSize:  Size of this structure
   dwCaps:        Enabled caps. Zero means all caps enabled
   bRaw:          TRUE if data is in raw format
   pSpec:         Media spec struct/info. Typically applies to raw format 
   dwSpecSize:    Size of media spec
   dwBufferSize:  Applies to streaming only. Size of streaming buffer to be maintained 
                  by IMedia object. Zero means use default

Comments:
   See AEEMediaData documentation first and then read the following.

   This structure allows users to 
   (1) Enable certain capabilities of media source by setting: dwCaps
       For example, play only video in a MPEG4 file.
   (2) Specify streaming info by setting: pSpec, dwSpecSize, dwBufferSize
       For example, use AEEMediaWaveSpec to specify raw PCM specs
   (3) Playback "raw" media data by setting: bRaw
       For example, raw QCP (13k/EVRC) data or raw PCM data.

See Also:
   IMedia_SetMediaDataEx()

=============================================================================

PFNMEDIANOTIFY

Description:
PFNMEDIANOTIFY is the type specification for callback function that user must
register with the IMedia object. IMedia object sends all the events
and data to user via the registered callback function.

Definition:
   typedef void (*PFNMEDIANOTIFY)(void * pUser, AEEMediaCmdNotify * pCmdNotify);

Members:
   pUser:      User-specified data pointer
   pCmdNotify: Callback event-specific information

Comments:
   None.

See Also:
   AEEMediaCmdNotify

=============================================================================

AEEMediaSeek

Description:
This enum specifies the seek reference in IMedia_Seek() API.

Definition:
   typedef enum {
      MM_SEEK_START = 0,
      MM_SEEK_END,
      MM_SEEK_CURRENT
   } AEEMediaSeek;


Members:
   MM_SEEK_START:    Seek from the beginning of media
   MM_SEEK_END:      Seek from the end of media
   MM_SEEK_CURRENT:  Seek from the current position of the media

Comments:
   IMedia_Rewind() and IMedia_FastForward() use MM_SEEK_CURRENT.

See Also:
   IMedia_Seek(), IMedia_Rewind(), IMedia_FastForward()

=============================================================================
AEEMediaAudioSync

Description:
This structure specifies the audio sync information to the app. When MM_STATUS_AUDIOSYNC callback 
occurs, AEEMediaCmdNotify::pData points to this structure.

Definition:
   typedef struct AEEMediaAudioSync
   {
      uint64   ullTimeStampMS;   // Time stamp in ms of when samples are rendered
      uint64   ullSamples;       // Number of samples rendered so far
   } AEEMediaAudioSync;

Members:
   ullTimeStampMS: Time stamp in ms of when samples are rendered
   ullSamples:     Number of samples rendered so far

Comments:
   This callback usually occurs after IMedia_Play() is started on streaming of raw media.

See Also:
   IMedia_Play()
   IMedia_SetMediaDataEx()
   AEEMediaDataEx
=============================================================================
Multimedia Status Codes (MM_STATUS_XXX)

Description:
The following table gives the possible events that contain commands and sub-commands,
status, and context-sensitive data.
===pre>
  nCmd               nSubCmd        nStatus                    pData [optional]
---------------------------------------------------------------------------------
MM_CMD_SETMEDIAPARM  MM_PARM_XXX    MM_STATUS_DONE             <Depends on parm>
                                    MM_STATUS_ABORT

MM_CMD_GETMEDIAPARM  MM_PARM_XXX    MM_STATUS_DONE             <Depends on parm>
                                    MM_STATUS_ABORT

MM_CMD_PLAY          0              MM_STATUS_START
                                    MM_STATUS_DONE
                                    MM_STATUS_ABORT
                                    MM_STATUS_MEDIA_SPEC       [Ptr to MediaSpec]
                                    MM_STATUS_TICK_UPDATE
                                    MM_STATUS_DATA_IO_DELAY    Elapsed Time in MS
                                    MM_STATUS_SEEK
                                    MM_STATUS_SEEK_FAIL
                                    MM_STATUS_PAUSE            Elapsed Time in MS
                                    MM_STATUS_PAUSE_FAIL
                                    MM_STATUS_RESUME           Elapsed Time in MS
                                    MM_STATUS_RESUME_FAIL
                                    MM_STATUS_REPEAT           [Elapsed Time in MS]
                                    MM_STATUS_FRAME

MM_CMD_RECORD        0              MM_STATUS_START
                                    MM_STATUS_DONE
                                    MM_STATUS_ABORT
                                    MM_STATUS_MEDIA_SPEC       [Ptr to MediaSpec]
                                    MM_STATUS_TICK_UPDATE
                                    MM_STATUS_DATA_IO_DELAY    Elapsed Time in MS
                                    MM_STATUS_SEEK
                                    MM_STATUS_SEEK_FAIL
                                    MM_STATUS_PAUSE            Elapsed Time in MS
                                    MM_STATUS_PAUSE_FAIL
                                    MM_STATUS_RESUME           Elapsed Time in MS
                                    MM_STATUS_RESUME_FAIL
                                    MM_STATUS_FRAME
                                    MM_STATUS_SPACE_WARNING
                                    MM_STATUS_SPACE_ERROR


MM_CMD_GETTOTALTIME   0             MM_STATUS_DONE             Total Time in MS
                                    MM_STATUS_ABORT
===/pre>

See Also:
   PFNMEDIANOTIFY,
   IMedia_SetMediaParm()
   IMedia_GetMediaParm()
   IMedia_Play()
   IMedia_Record()
   IMedia_GetTotalTime()
   IMEDIA1_SetMediaParm()
   IMEDIA1_GetMediaParm()
   IMEDIA1_Play()
   IMEDIA1_Record()
   IMEDIA1_GetTotalTime()

=============================================================================
Multimedia Parameters (MM_PARM_XXX)

Description:
Following table gives the list of IMedia supported get/set parameters:
===pre>

nParmID                 Op       Desc                       p1                            p2
-----------------------------------------------------------------------------------------------
MM_PARM_MEDIA_DATA      Get/Set  Media data                 AEEMediaData *
MM_PARM_AUDIO_DEVICE    Get/Set  Audio device               AEESoundDevice
MM_PARM_AUDIO_PATH      Get/Set  Media path                 MM_APATH_XXX                  SilenceTimerMS. For ringer path pnly: (1000 ms default), silence in between playback. = 0, means no repeat.

MM_PARM_VOLUME          Get/Set  Volume ctl                 Volume(0-AEE_MAX_VOLUME)
MM_PARM_MUTE            Get/Set  Mute Ctl                   1 to mute, 0 to unmute.

MM_PARM_TEMPO           Get/Set  Tempo ctl                  Tempo factor (percent)
MM_PARM_TUNE            Get/Set  Tune ctl                   Tune factor
MM_PARM_PAN             Get/Set  Pan ctl                    Pan factor.
                                                            Range: [0(Left) < 64(Balanced - default) < 128(Right)]
MM_PARM_TICK_TIME       Get/Set  Periodicity of             1000 (default); = 0 no event sent
                                 MM_TICK_UPDATE event
MM_PARM_RECT            Get/Set  Video/Image and            Dest Video/Image AEERect *    Dest Text AEERect *
                                 Text rectangles
MM_PARM_PLAY_REPEAT     Get/Set  Play repeat count          1=play once (default); = 0 play forever; = n>1 play n times
MM_PARM_POS             See description of MM_PARM_POS below
MM_PARM_CLSID           Get      IMedia class ID            ClassID
MM_PARM_CAPS            Get      IMedia class capabilities  ORed MM_CAPS_XXX              IMedia specific caps struct *
MM_PARM_ENABLE          Get/Set  Enable/Disable IMedia      ORed MM_CAPS_XXX
                                 capabilities
MM_PARM_CHANNEL_SHARE   Get/Set  Enable/Disable sharing     TRUE/FALSE =>Enable/Disable
                                 the channel for playback
MM_PARM_FRAME           Get      Returns current frame as   IBitmap *                     TRUE/FALSE=>Enable/Disable
                                 IBitmap *
                        Set      Enables/Disables           TRUE/FALSE=>Enable/Disable
                                 MM_STATUS_FRAME callback   [Default: Disabled]

MM_PARM_DECRYPT_CAPS    Get      Returns supported          Ptr to ORed MM_DECRYPT_CAPS_XXX     Zero-terminated list
                                 decryption capabilities                                        of decryption formats
                                 & decryption formats for                                       (MM_DECRYPT_FMT_XXX)
                                 the media
MM_PARM_DECRYPT_INFO    Set/Get  Provides decryption info   MM_DECRYPT_CAPS_XXX                 AEEMediaDecryptInfo *
                                 (key) to enable media
                                 content decryption
MM_PARM_SEEK_CAPS       Get      Get seek caps, i.e.,       List of supported modes             List size (bytes)
                                 supported seek modes       (Can be NULL to get size in p2)
===/pre>

MM_PARM_POS
===pre>
Set:  Set play/rec start/stop pos and implicitly play direction. Direction is determined by values of start and stop.
      start < stop => forward direction
      stop < start => reverse direction
      p1 = 16-bit LSB:0/1 for start/stop
           16-bit MSB: Seek mode (MM_SEEK_MODE_XXX), 
      p2 = dwPos: start/stop pos. -1 indicates last value.
Get:  Current play/rec position.
      Input:   p1 = Seek mode (MM_SEEK_MODE_XXX)
      Output:  p1 = Cur pos in ms
               p2 = Cur pos in seek mode
===/pre>
MM_PARM_RATE
===pre>
Set:  Sets play speed and direction.
      p1 = Play rate: dwPlayRate
      dwPlayRate format: Lower 16 bits is Numerator. 
                         Upper 16 bits is Denominator. Zero denominator is treated as 1.
Get:  Gets play speed and direction
      p2 = *pdwPlayRate
===/pre>

MM_PARM_PLAY_TYPE
===pre>
Set:  Sets the play type which specifies if the playback is as a ringer, alert, reminder, etc.
      p1 = Play type: MM_PLAY_TYPE_XXX
Get:  Gets the current play type
      p2 = MM_PLAY_TYPE_XXX
===/pre>

MM_PARM_AUDIOSYNC
===pre>
Set: Set AudioSync callback info. 
     p1 = uint32. Specifies the callback type, MM_AUDIOSYNC_MODE_XXX
     p2 = uint32. Specifies the info required for the above callback, TimeMS/SampleRate
Get  Gets the current AudioSync callback info.
     p1 = MM_AUDIOSYNC_MODE_XXX
     p2 = TimeMS/SampleRate
===/pre>
  
MM_PARM_NOTES
===pre>
Set:  Set the number of simultaneously voiced notes required by this media object. 
      p1 = uint32, specifies the number of notes.

      This parameter applies to formats with synthesized playback, such as MIDI, MMF, etc.
      When multiple MIDI media objects are played simultaneously, the synthesizer will 
      determine the best possible playback quality that accommodates all the notes required 
      for that set of objects.  A larger number of notes may possibly require a lower 
      quality for some of the synthesized notes. 

      Setting this to zero will select the highest quality playback mode.
      If not set, the system will accommodate as many notes as possible.
      This parameter can be set in READY state only.
Get:  Gets the current value of the number of simultaneously voiced notes for this 
      media object.
      p1 = uint32 *, current value
===/pre>

Side Effects:
This function may return MM_PENDING return value. In this case, the results
are returned in the registered callback. (See AEEMediaNotify struct documentation).

Version:
    Introduced BREW SDK 2.0

See Also:
   IMedia_GetMediaParm()
   IMedia_SetMediaParm()
   IMEDIA1_GetMediaParm()
   IMEDIA1_SetMediaParm()

=============================================================================

=====================================================================
 INTERFACE DOCUMENTATION
=====================================================================

IMedia Interface

Description:
The IMedia Interface class represents an abstract base class for all BREW multimedia
objects. It represents one type of multimedia format or a format played thorough
a specific device. For example, there can be an IMediaMIDI object that performs MIDI
playback and an IMediaMPEG4 object that renders MPEG4 content. The BREW multimedia
objects can contain a combination of audio, video, and karaoke text. The IMedia Interface
provides facilities to play back, record, and perform play/record control functions
like seek, stop, pause, resume, and so forth.

Note: IMedia-based object means IMedia Interface object, hereafter.
See IMedia-based classes.
===H2>
Shell Registry:
===/H2>

IMedia Interface objects have a base class ID of AEECLSID_MEDIA. You need to use
this class ID to query the Shell registry for one of the IMedia-based classes handling
a given MIME. Typically, IMedia Interface handlers are registered with MIME types:
"audio/" and "video/". Refer to AEEMIMETypes.h for a listing of IMedia MIME types.

===H2>
Creating IMedia Interface Objects
===/H2>

IMedia Interface objects can be created in one of the two ways:
~
1.   Using the class ID of the IMedia-based class, if the class ID is known, or

2.   Using IMEDIAUTIL_CreateMedia() utility function (provided in AEEMediaUtil.c),
that analyzes the media data, detects the media data type (which is its MIME), gets
the registered class ID for that MIME (with AEECLSID_MEDIA as base class) and creates
the IMedia Interface object.
*

In case (1), the IMedia Interface object is in Idle state. The IMedia Interface object cannot
be used in Idle state. You need to set media data using IMedia_SetMediaData() which,
if successful, puts IMedia Interface object in Ready state.

In case (2), IMEDIAUTIL_CreateMedia() puts IMedia Interface object in Ready state.

===H2>
Callback Events:
===/H2>

Most of the operations in IMedia Interface are asynchronous. You need to register
a callback notification function using IMedia_RegisterNotify(), if you want to receive
the events from IMedia Interface object. This step is optional. You may register
one callback function with several IMedia objects. You can correlate using either
the IMedia pointer or class ID returned in the callback data.

===H2>
IMedia State Machine:
===/H2>

IMedia maintains the following basic state machine. Implementers of IMedia can extend
the state machine.
===pre>

                  Idle
                    |
                    | IMedia_SetMediaData()
                    V
                  Ready
                    |
                    V
                -----------------------
               |                       |
 IMedia_Play() |                       | IMedia_Record()
               V                       V
              Play                   Record
               | ^                    ^  |
 IMedia_Pause()| |   IMedia_Resume()  |  | IMedia_Pause()
               V |                    |  V
           Play_Pause            Record_Pause

===/pre>
===H2>
IMedia State Machine Notes:
===/H2>

(1) IMedia_SetMediaData() sets the media data. Media data is not loaded on the device
yet.

(1) IMedia_Play() or IMedia_Record() loads the media data, allocates required device
multimedia resources and begins playback/recording.

(2) IMedia_Stop() stops current playback/recording, releases all the device multimedia
resources and the takes IMedia to Ready state from any playback/recording states.

===H2>
Deleting IMedia Object:
===/H2>

Preferably, you should release IMedia object. But this is not enforced by BREW.
When the application is unloaded, BREW stops any ongoing playback/recording and
cleans up IMedia object along with any associated device multimedia resources.

===H2>
Simultaneous Playback/Recording:
===/H2>

BREW does not enforce any rules for simultaneous playbacks or recordings. It depends
on the implementers of IMedia-based classes and also on the device capabilities.
For example, QUALCOMM MSM5100 based devices usually support one MIDI/QCP background
playback along with four simultaneous QCP playbacks through MIDI out device. Only
requirement BREW specifies is that if an IMedia API is not supported, then the API
shall return EUNSUPPORTED.

===H2>
Handling of application suspend and resume events:
===/H2>

If an application gets EVT_APP_SUSPEND, then it is preferable to do an IMedia_Stop()
and also release any resources associated with IMedia object. You need not do IMedia_Release()
but we recommend that you release the object and recreate it (along with associated
resources) when you get EVT_APP_RESUME.

===H2>
Play Options: Seek Mode, Start/Stop position, Play rate
===/H2>
IMedia allows three seek modes, viz, Seek based on 
(1) Time in milliseconds (MM_SEEK_MODE_TIME)
(2) Frame number (MM_SEEK_MODE_FRAME)
(3) Chapter number (MM_SEEK_MODE_CHAPTER)
IMedia_Seek() can seek in any of the specified modes, if supported by the media.

MM_PARM_SEEK_CAPS returns the list of supported seek modes for the media.

MM_PARM_POS allows user to set the start and stop positions of media playback/record. You can specify
the direction of play as forward or reverse -- the values of start and stop are used to determine the direction. 
Forward direction is default. The units of position depends on the specified seek mode. 
For example, if the media supports time and frame seek modes, you can start at frame #9 and stop at time 1300 ms.

MM_PARM_RATE allows user to set the play rate. Rate of 1 means normal speed in forward
direction. You can specify 0.5 for half speed, 2 for double speed, etc. 

===H2>
Sync external entity with media playback
===/H2>
MM_STATUS_SYNC, if supported, is sent by the media to enable synchronization of external
entities with media playback.

For example, if you've a media file with video track, you can send MM_STATUS_SYNC
at various points in time and allow the app to play music to sync with the image(s).

IMedia-based classes
===pre>

Format
 Class ID
 Class Name
 BREW MIME
 File Extension
 API
 Comments

MIDI
 AEECLSID_MEDIAMIDI
 IMediaMIDI
 audio/mid
 mid
 -
 -

MP3
 AEECLSID_MEDIAMP3
 IMediaMP3
 audio/mp3
 mp3
 -
 -

QCP
 AEECLSID_MEDIAQCP
 IMediaQCP
 audio/qcp
 qcp
 IMEDIAQCP_SetFormat()IMEDIAQCP_GetFormat()
 [CMX-based devices only] Can enable MM_PARM_CHANNEL_SHARE. Plays via MIDIOut device. Multiple
media files can be played.

QCP (thru MIDIOutDevice)
 AEECLSID_MEDIAMIDIOUTQCP
 IMediaMIDIOutQCP
 audio/qcf
 qcf
 -
 [CMX-based devices only] Special class to play QCP via MIDIOut device

MIDIMsg
 AEECLSID_MEDIAMIDIOUTMSG
 IMediaMIDIOutMsg
 -
 -
 IMEDIAMIDIOUTMSG_SetMIDIMsg()
 Sends MIDI message in buffer to the MIDIOut device

PMD (also called CMX)
 AEECLSID_MEDIAPMD
 IMediaPMD
 video/pmd
 pmd
 -
 -

MPEG4
 AEECLSID_MEDIAMPEG4
 IMediaMPEG4
 video/mp4
 mp4
 -
 -

MMF (SMAF)
 AEECLSID_MEDIAMMF
 IMediaMMF
 audio/mmf
 mmf
 -
 -

Phrase
 AEECLSID_MEDIAPAPHR
 IMediaPhr
 audio/spf
 spf
 -
 -

IMA-ADPCM
 AEECLSID_MEDIAADPCM
 IMediaADPCM
 audio/wav
 wav
 -
 [CMX-based devices only] Can enable MM_PARM_CHANNEL_SHARE.Multiple media files
can be played.

AAC
 AEECLSID_MEDIAAAC
 IMediaAAC
 audio/aac
 aac
 -
 -

IMelody
 AEECLSID_MEDIAIMELODY
 -
 audio/imy
 imy
 -
 -

===/pre>



The following header files are required:~
AEEMedia.h~
AEEMediaFormats.h


=============================================================================

IMedia_AddRef()

Description:
    This function is inherited from IBase_AddRef().


=============================================================================

IMedia_Release()

Description:
    This function is inherited from IBase_Release().

=============================================================================

IMedia_QueryInterface()

Description:
   This function can be used to
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the IMedia-derived class
   (3) support version compatibility

Prototype:
   int IMedia_QueryInterface(IMedia * po, AEECLSID clsReq, void ** ppo);

Parameters:
   po [in]: Pointer to IMedia interface.

   clsReq [in]: A globally unique id to identify the entity (interface or data) that we
                are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.  If the value
             passed back is NULL, the interface or data that we query are not available.

Return Value:
   SUCCESS: on success, ~
   Otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects:
   If an interface is retrieved, then this function increments its reference count.
   If a data structure is retrieved, then a pointer to the internal structure is
   given and user should not free it.

Version:
    Introduced BREW SDK 2.0

See Also:
    None
=============================================================================

IMedia_RegisterNotify()

Description:
    This function registers a callback notification function with IMedia Interface
object. IMedia Interface reports asynchronous events thorough this callback.

Prototype:
    int IMedia_RegisterNotify
    (
    IMedia * pIMedia,
    PFNMEDIANOTIFY pfnNotify,
    void * pUser
    )

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    pfnNotify :  Pointer to the callback function.
    pUser :  User data to be used when calling pfnNotify()

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~.
    EBADSTATE: Error - IMedia Interface is not in Ready state.

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    PFNMEDIANOTIFY

=============================================================================

IMedia_SetMediaParm()

Description:
    This function sets specified the IMedia Interface object parameters.

Prototype:
    int IMedia_SetMediaParm
    (
    IMedia * pIMedia,
    int nParamID,
    int32 p1,
    int32 p2
    )

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    nParmID :  MM_PARM_XXX
    p1 :  Depends on parm
    p2 :  Depends on parm

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
Following table gives the list of IMedia supported get/set parameters:
===pre>

nParmID                 Op       Desc                       p1                            p2
-----------------------------------------------------------------------------------------------
MM_PARM_MEDIA_DATA      Get/Set  Media data                 AEEMediaData *
MM_PARM_AUDIO_DEVICE    Get/Set  Audio device               AEESoundDevice
MM_PARM_AUDIO_PATH      Get/Set  Media path                 MM_APATH_XXX                  SilenceTimerMS. For ringer path pnly: (1000 ms default), silence in between playback. = 0, means no repeat.

MM_PARM_VOLUME          Get/Set  Volume ctl                 Volume(0-AEE_MAX_VOLUME)
MM_PARM_MUTE            Get/Set  Mute Ctl                   1 to mute, 0 to unmute.

MM_PARM_TEMPO           Get/Set  Tempo ctl                  Tempo factor (percent)
MM_PARM_TUNE            Get/Set  Tune ctl                   Tune factor
MM_PARM_PAN             Get/Set  Pan ctl                    Pan factor.
                                                            Range: [0(Left) < 64(Balanced - default) < 128(Right)]
MM_PARM_TICK_TIME       Get/Set  Periodicity of             1000 (default); = 0 no event sent
                                 MM_TICK_UPDATE event
MM_PARM_RECT            Get/Set  Video/Image and            Dest Video/Image AEERect *    Dest Text AEERect *
                                 Text rectangles
MM_PARM_PLAY_REPEAT     Get/Set  Play repeat count          1=play once (default); = 0 play forever; = n>1 play n times
MM_PARM_POS             See description of MM_PARM_POS below
MM_PARM_CLSID           Get      IMedia class ID            ClassID
MM_PARM_CAPS            Get      IMedia class capabilities  ORed MM_CAPS_XXX              IMedia specific caps struct *
MM_PARM_ENABLE          Get/Set  Enable/Disable IMedia      ORed MM_CAPS_XXX
                                 capabilities
MM_PARM_CHANNEL_SHARE   Get/Set  Enable/Disable sharing     TRUE/FALSE=>Enable/Disable
                                 the channel for playback
MM_PARM_FRAME           Get      Returns current frame as   IBitmap *                     TRUE/FALSE=>Enable/Disable
                                 IBitmap *
                        Set      Enables/Disables           TRUE/FALSE=>Enable/Disable
                                 MM_STATUS_FRAME callback   [Default: Disabled]

MM_PARM_DECRYPT_CAPS    Get      Returns supported          Ptr to ORed MM_DECRYPT_CAPS_XXX     Zero-terminated list
                                 decryption capabilities                                        of decryption formats
                                 & decryption formats for                                       (MM_DECRYPT_FMT_XXX)
                                 the media
MM_PARM_DECRYPT_INFO    Set/Get  Provides decryption info   MM_DECRYPT_CAPS_XXX                 AEEMediaDecryptInfo *
                                 (key) to enable media
                                 content decryption
MM_PARM_SEEK_CAPS       Get      Get seek caps, i.e.,       List of supported modes             List size (bytes)
                                 supported seek modes       (Can be NULL to get size in p2)
===/pre>

MM_PARM_POS
===pre>
Set:  Set play/rec start/stop pos and implicitly play direction. Direction is determined by values of start and stop.
      start < stop => forward direction
      stop < start => reverse direction
      p1 = 16-bit LSB:0/1 for start/stop
           16-bit MSB: Seek mode (MM_SEEK_MODE_XXX), 
      p2 = dwPos: start/stop pos. -1 indicates last value.
Get:  Current play/rec position.
      Input:   p1 = Seek mode (MM_SEEK_MODE_XXX)
      Output:  p1 = Cur pos in ms
               p2 = Cur pos in seek mode
===/pre>
MM_PARM_RATE
===pre>
Set:  Sets play speed and direction.
      p1 = Play rate: dwPlayRate
      dwPlayRate format: Lower 16 bits is Numerator. 
                         Upper 16 bits is Denominator. Zero denominator is treated as 1.
Get:  Gets play speed and direction
      p2 = *pdwPlayRate
===/pre>

MM_PARM_PLAY_TYPE
===pre>
Set:  Sets the play type which specifies if the playback is as a ringer, alert, reminder, etc.
      p1 = Play type: MM_PLAY_TYPE_XXX
Get:  Gets the current play type
      p2 = MM_PLAY_TYPE_XXX
===/pre>

MM_PARM_AUDIOSYNC
===pre>
Set: Set AudioSync callback info. 
     p1 = uint32. Specifies the callback type, MM_AUDIOSYNC_MODE_XXX
     p2 = uint32. Specifies the info required for the above callback, TimeMS/SampleRate
Get  Gets the current AudioSync callback info.
     p1 = MM_AUDIOSYNC_MODE_XXX
     p2 = TimeMS/SampleRate
===/pre>
  
MM_PARM_NOTES
===pre>
Set:  Set the number of simultaneously voiced notes required by this media object. 
      p1 = uint32, specifies the number of notes.

      This parameter applies to formats with synthesized playback, such as MIDI, MMF, etc.
      When multiple MIDI media objects are played simultaneously, the synthesizer will 
      determine the best possible playback quality that accommodates all the notes required 
      for that set of objects.  A larger number of notes may possibly require a lower 
      quality for some of the synthesized notes. 

      Setting this to zero will select the highest quality playback mode.
      If not set, the system will accommodate as many notes as possible.
      This parameter can be set in READY state only.
Get:  Gets the current value of the number of simultaneously voiced notes for this 
      media object.
      p1 = uint32 *, current value
===/pre>

Side Effects:
This function may return MM_PENDING return value. In this case, the results
are returned in the registered callback. (See AEEMediaNotify struct documentation).

Version:
    Introduced BREW SDK 2.0

See Also:
   AEEMediaData
   IMedia_GetMediaParm()

=============================================================================

IMedia_GetMediaParm()

Description:
    This function gets the parameters set using IMedia_SetMediaParm()

Prototype:
    int IMedia_GetMediaParm
    (
    IMedia * pIMedia,
    int nParamID,
    int32 * pP1,
    int32 * pP2
    )

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    nParmID :  MM_PARM_XXX
    pP1 :  Depends on parm
    pP2 :  Depends on parm

Return Value:
    SUCCESS: if successful
    MM_PENDING: Value returned in the registered callback
    Error code, if otherwise:~
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
The table in "comments section of IMedia_SetMediaParm()" documentation gives the list of
IMedia supported gettable parameters.

Side Effects:
    This function may return MM_PENDING return value. In this case, the results
are returned in the registered callback. See the AEEMediaCmdNotify documentation.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetMediaParm()
    AEEMediaCmdNotify

=============================================================================

IMedia_Play()

Description:
    This function starts the playback of IMedia Interface object. It loads the media
data, allocates required device multimedia resources and begins playback.

Prototype:
    int IMedia_Play(IMedia * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_EBADSTREAM: Bad stream
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot play in the current state
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The playback events are returned in the registered callback. See the AEEMediaCmdNotify
and the AEEMediaData documentation.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_FastForward()
    IMedia_Pause()
    IMedia_Record()
    IMedia_Resume()
    IMedia_Rewind()
    IMedia_Seek()
    IMedia_Stop()
    AEEMediaCmdNotify
=============================================================================

IMedia_Record()

Description:
    This function starts the recording of IMedia Interface object. It loads the
media data, allocates required device multimedia resources and begins recording.

Prototype:
    int IMedia_Record(IMedia * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_EBADSTREAM: Bad stream
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot record in the current state
    EITEMBUSY: IMedia is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The recording events are returned in the registered callback, See AEEMediaCmdNotify
and AEEMediaData documentation.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_FastForward()
    IMedia_Pause()
    IMedia_Play()
    IMedia_Resume()
    IMedia_Rewind()
    IMedia_Seek()
    IMedia_Stop()
    AEEMediaCmdNotify
    AEEMediaData
=============================================================================

IMedia_Stop()

Description:
    This function stops the playback/recording of IMedia Interface object. It releases
all the device multimedia resources and the takes IMedia Interface to Ready state
from any playback/recording states.

Prototype:
    int IMedia_Stop(IMedia * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADSTATE: Cannot stop in the current state

Comments:
    The recording events are returned in the registered callback. See AEEMediaCmdNotify
documentation. This function results in MM_STATUS_DONE / MM_STATUS_ABORTED status.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_Play()
    IMedia_Record()
=============================================================================

IMedia_Seek()

Description:
    This function seeks the media position based on the eSeek and lTimeMS params.
The media is left in the current state.

Prototype:
    int IMedia_Seek(IMedia * pIMedia, AEEMediaSeek eSeek, int32 lTimeMS)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    eSeek :  Seek reference
    lTimeMS :  Time in milliseconds

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot seek in the current state
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_SEEK or MM_SEEK_FAIL status.
If media playback continues after seek, MM_STATUS_RESUME is returned.

Version:
    Introduced BREW SDK 2.0

See Also:
    AEEMediaSeek
    IMedia_FastForward()
    IMedia_Rewind()
    IMedia_Play()
=============================================================================
IMedia_SeekFrame()

Description:
This function seeks the media pos based on the seek reference and number of frames.
Typically, this API is used for video formats to seek based on frames.
The media is left in the current state.

Prototype:
   int IMedia_SeekFrame(IMedia * po, AEEMediaSeek eSeek, int32 lFrames);

Parameters:
   po : Pointer to the IMedia Interface object
   eSeek: Seek reference
   lFrames: Number of frames. Negative indicates reverse direction

Return Value:
   SUCCESS:    Successful
   EBADPARM:   Bad parm in stream
   EBADSTATE:  Cannot seek in the current state
   EITEMBUSY:  IMedia is currently busy. Try after some time.
   EUNSUPPORTED: API not supported by the class

Comments:

Side Effects:
The events are returned in the registered callback.
(See PFNMEDIANOTIFY struct documentation).

This function results in MM_STATUS_SEEK or MM_SEEK_FAIL status.
If media playback continues after seek, MM_STATUS_RESUME is returned.

Version:
    Introduced BREW Client 3.1

See Also:
   AEEMediaSeek
   IMedia_Seek()
   IMedia_Rewind()
   IMedia_FastForward()
   IMedia_Play()
=============================================================================

IMedia_Rewind()

Description:
    This function rewinds lTimeMS milliseconds from the current position. The media
is left in the current state.

Prototype:
    int IMedia_Rewind(IMedia * pIMedia, int32 lTimeMS)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    lTimeMS :  Time in milliseconds

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot rewind in the current state
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_SEEK or MM_SEEK_FAIL status.
If media playback continues after seek, MM_STATUS_RESUME is returned.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_FastForward()
    IMedia_Seek()
    IMedia_Play()
    AEEMediaCmdNotify

=============================================================================

IMedia_FastForward()

Description:
    This function fast forwards lTimeMS milliseconds from the current position.
The media is left in the current state.

Prototype:
    int IMedia_FastForward(IMedia * pIMedia, int32 lTimeMS)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    lTimeMS :  Time in milliseconds

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream.
    EBADSTATE: Cannot seek in the current state.
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class.

Comments:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_SEEK or MM_SEEK_FAIL status.
If media playback continues after seek, MM_STATUS_RESUME is returned.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_Seek()
    IMedia_Rewind()
    IMedia_Play()
    AEEMediaCmdNotify

=============================================================================

IMedia_Pause()

Description:
    This function pauses the media and returns the current position in the MM_STATUS_PAUSE
callback.

Prototype:
    int IMedia_Pause(IMedia * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot pause in the current state
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_PAUSE or MM_PAUSE_FAIL status.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_Play()
    IMedia_Resume()
    IMedia_Record()
    AEEMediaCmdNotify
=============================================================================

IMedia_Resume()

Description:
    This function resumes the media from the current position and returns the current
position in the MM_STATUS_RESUME callback.

Prototype:
    int IMedia_Resume(IMedia * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream
    EBADSTATE: Cannot resume in the current state
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class

Comments:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_RESUME or MM_RESUME_FAIL status.

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_Play()
    IMedia_Pause()
    IMedia_Record()
    AEEMediaCmdNotify

=============================================================================

IMedia_GetTotalTime()

Description:
    This function returns the total playback time of the IMedia Interface object.

Prototype:
    int IMedia_GetTotalTime(iname * pIMedia)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    EBADPARM: Bad parm in stream.
    EBADSTATE: IMedia Interface is not in Ready state.
    EITEMBUSY: IMedia Interface is currently busy. Try after some time.
    EUNSUPPORTED: API not supported by the class.

Comments:
    IMedia_GetTotalTime() only works in the READY state

Side Effects:
    The events are returned in the registered callback. See the AEEMediaCmdNotify
documentation. This function results in MM_STATUS_DONE or MM_RESUME_ABORT status.

Version:
    Introduced BREW SDK 2.0

See Also:
    AEEMediaCmdNotify

=============================================================================

IMedia_GetState()

Description:
    This function returns the current state of IMedia Interface and also indicates
if the IMedia Interface object is currently in state transition.

Prototype:
    int IMedia_GetState(iname * pIMedia, boolean * pbStateChanging)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    pbStateChanging :  TRUE means IMedia Interface is currently busy with state
transition.

Return Value:
    MM_STATE_XXX

Comments:
    None

Side Effects:
    If IMedia Interface is currently is in state transition, then most of IMedia
functions fail and return EBADSTATE.

Version:
    Introduced BREW SDK 2.0

See Also:
    None

=============================================================================

IMedia_SetMediaData()

Description:
    This function sets the media data and puts the media in Ready state.

Prototype:
    int IMedia_SetMediaData(IMedia * pIMedia, AEEMediaData * pmd)

Parameters:
    pIMedia :  Pointer to the IMedia Interface object
    pmd :  Media data

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_GetMediaData()
    IMedia_GetMediaParm()
    IMedia_SetMediaParm()
=============================================================================

IMedia_SetMediaDataEx()

Description:
This function sets a list of media data and puts the media in Ready state.

Prototype:
   int IMedia_SetMediaDataEx(IMedia * po, AEEMediaDataEx * pmdList, int nCount);

Parameters:
   po : Pointer to the IMedia Interface object
   pmdList: List of media content
   nCount: Number of elements in pmdList

Return Value:
   SUCCESS:    Successful
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm set not supported by the class

Comments:
   This command allows users to set a list of media sources while allowing the flexibility
   to enable certain capabilities for each media source. For example, you can specify two
   MPEG4 files and do playback of video track from one file and audio track from second
   file (Note: In this case, the underlying media player may do timing syncronization).

   For a single media source, this command can be used as follows:

      IMedia_SetMediaDataEx(pMedia, pOneAEEMediaDataEx, 1);

   which provides following advantages to user

   (1) Specify streaming info
   (2) Playback "raw" media data. For example, raw QCP (13k/EVRC) data (i.e., QCP data with no headers, etc.)
   (3) Specify media specifications (typically for #2, the raw media data playback). For example, if
       it is PCM playback, then use AEEMediaWaveSpec to specify the PCM info.

Side Effects:
   None.

See Also:
   IMedia_GetMediaData()
   IMedia_SetMediaParm(), IMedia_GetMediaParm()

=============================================================================

IMedia_GetMediaData()

Description:
    This function gets the media data.

Prototype:
    int IMedia_GetMediaData(IMedia * pIMedia, AEEMediaData * pmd)

Parameters:
    pIMedia :  [in]  :  Pointer to the IMedia Interface object
    pmd :  [out] :  Media data

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_GetMediaParm()
    IMedia_SetMediaData()
    IMedia_SetMediaParm()

=============================================================================

IMedia_SetAudioDevice()

Description:
    This function sets the audio output device.

Prototype:
    int IMedia_SetAudioDevice(IMedia * pIMedia, AEESoundDevice eDevice)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    eDevice :  Target audio device

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback.
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetAudioPath()
    IMedia_SetMuteCtl()
    IMedia_SetVolume()
    IMedia_GetVolume()
    IMedia_SetPan()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()

=============================================================================

IMedia_SetAudioPath()

Description:
    This function sets the audio path for playback/recording.

Prototype:
    int IMedia_SetAudioPath(IMedia * pIMedia, int nAPath uint32 dwMS);

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    nAPath :  Audio path
    dwMS :  Pause, in milliseconds, between rings. Applies to MM_APATH_LOCAL_RINGER
            only

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
   Audio path can take following values:~
~
   MM_APATH_LOCAL:          Playback to: ear-piece, speaker, etc. Record from: Mic~
   MM_APATH_LOCAL_RINGER:   Playback to: Ringer~
   MM_APATH_REMOTE:         Playback to: ReverseLink(over-the-air). Record from: ForwardLink.~
   MM_APATH_BOTH:           Playback [to]: Both MM_APATH_LOCAL and MM_APATH_TX.~
*

Side Effects:
   None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetMuteCtl()
    IMedia_SetVolume()
    IMedia_GetVolume()
    IMedia_SetPan()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()
    IMedia_SetAudioDevice()

=============================================================================

IMedia_SetMuteCtl()

Description:
    This function mutes / unmutes the current audio path.

Prototype:
    int IMedia_SetMuteCtl(IMedia * pIMedia, boolean bMute)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    bMute :  TRUE means mute
    Otherwise do not mute.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetAudioPath()
    IMedia_SetVolume()
    IMedia_GetVolume()
    IMedia_SetPan()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()
    IMedia_SetAudioDevice()

=============================================================================

IMedia_SetVolume()

Description:
    This function sets the volume of the current playback / recording.

Prototype:
    int IMedia_SetVolume(IMedia * pIMedia, uint16 wVolume)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    wVolume :  Volume. Range: 0 - AEE_MAX_VOLUME

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetAudioDevice()
    IMedia_SetAudioPath()
    IMedia_SetMuteCtl()
    IMedia_GetVolume()
    IMedia_SetPan()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()
=============================================================================

IMedia_GetVolume()

Description:
    This function retrieves the volume of the current playback / recording.

Prototype:
    int IMedia_GetVolume(IMedia * pIMedia, uint16 * pwVolume)

Parameters:
    pIMedia :  [in]  :  Pointer to the IMedia Interface Interface object
    pwVolume :  [out] :  Pointer to volume.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetAudioPath()
    IMedia_SetMuteCtl()
    IMedia_SetVolume()
    IMedia_SetPan()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()
    IMedia_SetAudioDevice()

=============================================================================

IMedia_SetPan()

Description:
    This function sets the pan factor of the current playback.

Prototype:
    int IMedia_SetPan(IMedia * pIMedia, uint16 wPan)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    wPan :  Pan factor. Range:
    0(zero) means (Left).
    MM_MAX_PAN/2 means (Balanced - default).
    MM_MAX_PAN means (Right).

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_SetAudioPath()
    IMedia_SetMuteCtl()
    IMedia_SetVolume()
    IMedia_GetVolume()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()
    IMedia_SetAudioDevice()

=============================================================================

IMedia_SetTickTime()

Description:
    This function sets periodicity of MM_TICK_UPDATE event sent to the application
during
    playback/recording.

Prototype:
    int IMedia_SetTickTime(IMedia * pIMedia, uint32 wTickMS)

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    dwTickMS :  Tick period. = 1000 (default); = 0 no event sent.

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_Play()
    IMedia_Record()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()

=============================================================================

IMedia_SetRect()

Description:
    This function sets the destination image rectangle and text rectangle. If the
media data contains video/image info, then it is displayed in the specified image
rectangle. If media data contains embedded text info, then the text is displayed
in the text rectangle.

Prototype:
    int IMedia_SetRect
    (
    IMedia * pIMedia,
    AEERect * prImage,
    AEERect * prText
    )

Parameters:
    pIMedia  :  Pointer to the IMedia Interface Interface object
    prImage :  Image rectangle pointer
    prText :  Text rectangle pointer

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    AEERect
    IMedia_Play()
    IMedia_Record()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()

=============================================================================

IMedia_GetRect()

Description:
    This function retrieves the destination image rectangle and text rectangle.

Prototype:
    int IMedia_GetRect
    (
    IMedia * pIMedia,
    AEERect * prImage,
    AEERect * prText
    )

Parameters:
    pIMedia :  [in]  :  Pointer to the IMedia Interface object
    prImage :  [out] :  Image rectangle pointer
    prText :  [out] :  Text rectangle pointer

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    AEERect
    IMedia_Play()
    IMedia_Record()
    IMedia_SetMediaParm()
    IMedia_GetMediaParm()

=============================================================================

IMedia_GetClassID()

Description:
    This function retrieves the class ID of IMedia Interface object.

Prototype:
    int IMedia_GetClassID(IMedia * pIMedia, AEECLSID * pCls)

Parameters:
    pIMedia  :  [in]  :  Pointer to the IMedia Interface object
    pCls :  [out] :  Class ID of IMedia object

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    None

Version:
    Introduced BREW SDK 2.0

See Also:
    IMedia_GetMediaParm()

=============================================================================

IMedia_EnableChannelShare()

Description:
   Enable/Disable exclusive playback of the media.

Prototype:
   int IMedia_EnableChannelShare(IMedia * po, boolean bEnable);

Parameters:
   po :        Pointer to the IMedia Interface object
   bEnable:    TRUE/FALSE => Enable/Disable channel share

Return Value:
   SUCCESS:    Successful
   Error code, if otherwise:~
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm set not supported by the class

Comments:
   If enabled, then multiple IMedia objects can be played simultaneously.
   Its implementation is device and media dependent.

Side Effects:

Version:
    Introduced BREW Client 3.1

See Also:
   IMedia_IsChannelShare()

=============================================================================

IMedia_IsChannelShare()

Description:
   Retrieves the current channel share setting.

Prototype:
   int IMedia_IsChannelShare(IMedia * po, boolean * pbEnable);

Parameters:
   po :        Pointer to the IMedia Interface object
   pbEnable:   Pointer to boolean indicating TRUE/FALSE => Enable/Disable frame callback.

Return Value:
   SUCCESS:    Successful
   Error code, if otherwise:~    
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm get not supported by the class

Comments:
   None

Side Effects:

Version:
    Introduced BREW Client 3.1

See Also:
   IMedia_EnableChannelShare()

=============================================================================

IMedia_EnableFrameCallback()

Description:
   Enable/Disable frame playback of the media.

Prototype:
   int IMedia_EnableFrameCallback(IMedia * po, boolean bEnable);

Parameters:
   po :        Pointer to the IMedia Interface object
   bEnable:    TRUE/FALSE => Enable/Disable frame callback

Return Value:
   SUCCESS:    If successful
   Error code, if otherwise:~
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm set not supported by the class

Comments:
   This API applies to media having video content.

   Frame callback is disabled by default. If enabled, then IMedia object delivers
   MM_STATUS_FRAME callback for each media frame decoded. In response to this
   callback, user needs to call IMedia_GetFrame() to retrieve the IBitmap
   representing the frame.

Side Effects:

Version:
    Introduced BREW Client 3.1

See Also:
   IMedia_GetFrame()
   IMedia_IsFrameCallback()

=============================================================================

IMedia_IsFrameCallback()

Description:
   Retrieves the current frame callback enable/disable setting.

Prototype:
   int IMedia_IsFrameCallback(IMedia * po, boolean * pbEnable);

Parameters:
   po :        Pointer to the IMedia Interface object
   pbEnable:   Pointer to boolean indicating TRUE/FALSE => Enable/Disable frame callback.

Return Value:
   SUCCESS:    if successful
   Error code, if otherwise:~
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm get not supported by the class

Comments:
   None

Side Effects:

Version:
    Introduced BREW Client 3.1

See Also:
   IMedia_EnableFrameCallback()
   IMedia_GetFrame()

=============================================================================

IMedia_GetFrame()

Description:
   Retrieves the current frame as IBitmap pointer.

Prototype:
   int IMedia_GetFrame(IMedia * po, IBitmap ** ppFrame);

Parameters:
   po :     Pointer to the IMedia Interface object
   ppFrame: Frame as IBitmap

Return Value:
   SUCCESS:    if successful
   Error code, if otherwise:~
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm get not supported by the class

Comments:
   User calls this API in response to MM_STATUS_FRAME callback. User can blt the bitmap or
   use it for post-processing.

Side Effects:

Version:
    Introduced BREW Client 3.1

See Also:
   IMedia_EnableFrameCallback()
   IMedia_IsFrameCallback()

============================================================================= */
#endif // AEEIMEDIA_H

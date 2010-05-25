#ifndef AEEMEDIAFORMATS_H
#define AEEMEDIAFORMATS_H

/*============================================================================
FILE:	AEEMediaFormats.h

SERVICES:  Pre-defined BREW MultiMedia Formats.

DESCRIPTION:
   This file defines the following interfaces derived from IMedia class:
   (1) IMediaMIDI:   plays Standard MIDI Format (SMF)
   (2) IMediaMP3:    plays MP3 Format
   (3) IMediaQCP:    plays and records QCP (PureVoice) Format
   (4) IMediaMIDIOutMsg:plays MIDI messages
   (5) IMediaMIDIOutQCP:  plays QCP (PureVoice) files in the foreground using MIDI device
   (6) IMediaPMD:    plays PMD Format that includes synchronized audio and video

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

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEMedia.h"

// Define interface pointers
typedef struct _IMediaMIDI          IMediaMIDI;
typedef struct _IMediaMP3           IMediaMP3;
typedef struct _IMediaQCP           IMediaQCP;
typedef struct _IMediaMIDIOutMsg    IMediaMIDIOutMsg;
typedef struct _IMediaMIDIOutQCP    IMediaMIDIOutQCP;
typedef struct _IMediaPMD           IMediaPMD;
typedef struct _IMediaMPEG4         IMediaMPEG4;
typedef struct _IMediaMMF           IMediaMMF;
typedef struct _IMediaPhr           IMediaPhr;
typedef struct _IMediaADPCM         IMediaADPCM;
typedef struct _IMediaAAC           IMediaAAC;

//********************************************************************************************************************************
//
// IMediaMIDI Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMIDI Type Declarations
-------------------------------------------------------------------*/
typedef struct AEEMediaMIDISpec
{
   byte     nFormat;    // SMF format: 0, 1 or 2
   uint16   nTracks;    // Number of tracks in the SMF
   int16    nDivision;  // Timing info 
} AEEMediaMIDISpec;

//-------------------------------------------------------------------
// IMediaMIDI Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaMIDI)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaMP3 Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMP3 Type Declarations
-------------------------------------------------------------------*/
#define MM_MP3_ID3_TAG_LENGTH       30

//
// MP3 version info
//
#define MM_MP3_VER_25               0     // MPEG version 2.5
#define MM_MP3_VER_RESERVED         1     // Reserved   
#define MM_MP3_VER_2                2     // MPEG version 2.0
#define MM_MP3_VER_1                3     // MPEG version 1.0
#define MM_MP3_VER_UNKNOWN          4

//
// MP3 layer: compression alogorithm
//
#define MM_MP3_LAYER_RESERVED       0  // Reserved
#define MM_MP3_LAYER_3              1  // MPEG Layer 3 compression
#define MM_MP3_LAYER_2              2  // MPEG Layer 2 compression
#define MM_MP3_LAYER_1              3  // MPEG Layer 1 compression
#define MM_MP3_LAYER_UNKNOWN        4

//
// MP3 bit rate: amount of info compressed in each frame.
// Indicates the level of quality.
//
#define MM_MP3_BITRATE_FREE         0
#define MM_MP3_BITRATE_VAR          500   // Variable bitrate (Changes each frame)
#define MM_MP3_BITRATE_UNKNOWN      501   // Unable to determine bitrate information

//
// MP3 sample rate: data rate of compressed data
//
#define MM_MP3_SAMPLE_RATE_UNKNOWN        0xFFFFffff

//
// MP3 channel info: number of audio channels embedded
//
#define MM_MP3_CH_STEREO            0
#define MM_MP3_CH_JOINT_STEREO      1
#define MM_MP3_CH_DUAL              2
#define MM_MP3_CH_SINGLE            3
#define MM_MP3_CH_UNKNOWN           0xFF

//
// MP3 extension info: type of joint stereo channel data.
// Applies to MM_MP3_CH_JOINT_STEREO only
//
// For Layer 1 & 2 files
#define MM_MP3_EXT_BAND_4_31              0  // Channel extension info, 4-31   
#define MM_MP3_EXT_BAND_8_31              1  //                         8-31
#define MM_MP3_EXT_BAND_12_31             2  //                        12-31   
#define MM_MP3_EXT_BAND_16_31             3  //                        16-31   
// For Layer 3 files
#define MM_MP3_EXT_INTENSITY_OFF_MS_OFF   0  // Intensity stereo off, MS off   
#define MM_MP3_EXT_INTENSITY_ON_MS_OFF    1  //                  on      off   
#define MM_MP3_EXT_INTENSITY_OFF_MS_ON    2  //                  off     on    
#define MM_MP3_EXT_INTENSITY_ON_MS_ON     3  //                  on      on  
#define MM_MP3_EXT_UNKNOWN                0xFF

//
// MP3 Emphasis: type of audio filter applied
//
#define MM_MP3_EMPHASIS_NONE              0  // Emphasis flag
#define MM_MP3_EMPHASIS_50_15_MS          1
#define MM_MP3_EMPHASIS_RESERVED          2
#define MM_MP3_EMPHASIS_CCITT_J17         3
#define MM_MP3_EMPHASIS_UNKNOWN           0xFF

typedef struct AEEMediaMP3Spec
{
   int         nVersion;                        // MPEG version
   byte        nLayer;                          // MPEG layer compression: 1, 2 or 3
   boolean     bCRCFlag;                        // True if CRC protection
   uint16      wBitRate;                        // Bit rate [Kilo bits]
   uint32      dwSampleRate;                    // Sampling rate [Kilo bits]
   byte        nChannel;                        // Channel
   byte        nExtension;                      // Only when JOINT_STEREO
   boolean     bCopyrightFlag;                  // True if copyrighted
   boolean     bOriginalFlag;                   // True if original
   byte        nEmphasis;                       // Audio emphasis
   char        szTitle[MM_MP3_ID3_TAG_LENGTH];  // Song title
   char        szArtist[MM_MP3_ID3_TAG_LENGTH]; // Song performer
   char        szAlbum[MM_MP3_ID3_TAG_LENGTH];  // Album with the song
   char        szYear[4];                       // Year Album released
   char        szComment[MM_MP3_ID3_TAG_LENGTH];// Text comment
   byte        nGenre;                          // ID3 genre tag
} AEEMediaMP3Spec;

//-------------------------------------------------------------------
// IMediaMP3 Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaMP3)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaQCP Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaQCP Type Declarations
-------------------------------------------------------------------*/
//
// QCP format type. This is the desired format specified during
// recording using MM_PARM_QCP_FORMAT parm type. 
// Note that this may be overridden based on the current DSP configuration.
//
#define MM_QCP_FORMAT_FIXED_FULL_13K   0     // QCP format, Fixed-full rate, 13K vocoder
#define MM_QCP_FORMAT_VAR_FULL_13K     1     // QCP format, Variable rate, full rate max, 13K
#define MM_QCP_FORMAT_EVRC             2     // QCP format, Variable rate, full rate max, EVRC
#define MM_QCP_FORMAT_VAR_HALF_13K     3     // QCP format, Varibale rate, half rate max, 13K
#define MM_QCP_FORMAT_FIXED_HALF_13K   4     // QCP format, Fixed half rate, 13K vocoder
#define MM_QCP_FORMAT_FIXED_FULL_EVRC  5     // QCP format, Fixed-full rate, EVRC vocoder
#define MM_QCP_FORMAT_VAR_FULL_EVRC    MM_QCP_FORMAT_EVRC

#define MM_QCP_FORMAT_FIXED_4_75_AMR   6     // QCP format, Fixed- 4.75 kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_5_15_AMR   7     // QCP format, Fixed- 5.15 kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_5_9_AMR    8     // QCP format, Fixed- 5.9  kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_6_7_AMR    9     // QCP format, Fixed- 6.7  kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_7_4_AMR    10    // QCP format, Fixed- 7.4  kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_7_95_AMR   11    // QCP format, Fixed- 7.95 kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_10_2_AMR   12    // QCP format, Fixed-10.2  kbit/s, AMR
#define MM_QCP_FORMAT_FIXED_12_2_AMR   13    // QCP format, Fixed-12.2  kbit/s, AMR

#define MM_AMR_FORMAT_FIXED_4_75_AMR   14    // AMR format, Fixed- 4.75 kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_5_15_AMR   15    // AMR format, Fixed- 5.15 kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_5_9_AMR    16    // AMR format, Fixed- 5.9  kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_6_7_AMR    17    // AMR format, Fixed- 6.7  kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_7_4_AMR    18    // AMR format, Fixed- 7.4  kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_7_95_AMR   19    // AMR format, Fixed- 7.95 kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_10_2_AMR   20    // AMR format, Fixed-10.2  kbit/s, AMR
#define MM_AMR_FORMAT_FIXED_12_2_AMR   21    // AMR format, Fixed-12.2  kbit/s, AMR
  
#define MM_QCP_FORMAT_VAR_FULL_4GV_NB     22 // QCP format, Variable rate, full rate max, 4GV-NB
#define MM_QCP_FORMAT_FIXED_FULL_4GV_NB   23 // QCP format, Fixed rate, full rate max, 4GV-NB

#define MM_QCP_FORMAT_FIXED_FULL_EFR      24 // QCP format, Fixed full rate, EFR
#define MM_QCP_FORMAT_FIXED_FULL_FR       25 // QCP format, Fixed full rate, FR
#define MM_QCP_FORMAT_FIXED_FULL_HR       26 // QCP format, Fixed full rate, HR

#define MM_QCP_FORMAT_UNKNOWN          0xFF

#define MM_PARM_QCP_FORMAT             (MM_PARM_USER_BASE)  // [Set/Get] Record parm. p1=MM_QCP_FORMAT_XXX. 
                                                            // Default: MM_QCP_FORMAT_FIXED_FULL_13K

//-------------------------------------------------------------------
// IMediaQCP Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaQCP)
{
   INHERIT_IMedia(IMedia);
};

#define IMEDIAQCP_SetFormat(po, nFormat)     IMEDIA_SetMediaParm((IMedia *)po, MM_PARM_QCP_FORMAT, (int32)nFormat, 0)
#define IMEDIAQCP_GetFormat(po, pnFormat)    IMEDIA_GetMediaParm((IMedia *)po, MM_PARM_QCP_FORMAT, (int32 *)pnFormat, NULL)

//********************************************************************************************************************************
//
// IMediaMIDIOutMsg Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMIDIOutMsg Type Declarations
-------------------------------------------------------------------*/
//
// MIDI messages are sent thru this command
//
#define MM_PARM_MIDI_MSG   (MM_PARM_USER_BASE)  // [Set] MIDI msg. p1 = uint8 *, p2 = dwSize.

//-------------------------------------------------------------------
// IMediaMIDIOutMsg Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaMIDIOutMsg)
{
   INHERIT_IMedia(IMedia);
};

#define IMEDIAMIDIOUTMSG_SetMIDIMsg(po, pMsg, dwSize) IMEDIA_SetMediaParm((IMedia *)po, MM_PARM_MIDI_MSG, (int32)pMsg, (int32)dwSize)

//********************************************************************************************************************************
//
// IMediaMIDIOutQCP Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMIDIOutQCP Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// IMediaMIDIOutQCP Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaMIDIOutQCP)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaPMD Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaPMD Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// IMediaPMD Interface Definitions - C Style
//-------------------------------------------------------------------
QINTERFACE(IMediaPMD)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaMPEG4 Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMPEG4 Type Declarations
-------------------------------------------------------------------*/
#define MM_STATUS_REPOSITIONING  (MM_STATUS_USER_BASE + 0)    // Repositioning allowed

//-------------------------------------------------------------------
// IMediaMPEG4 Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaMPEG4)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaMMF Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaMMF Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// IMediaMMF Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaMMF)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaPhr Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaPhr Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// IMediaPhr Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaPhr)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaADPCM Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct AEEMediaWaveSpec
{
   // Mandatory info:
   uint16   wSize;            // Size of this struct.
   AEECLSID clsMedia;         // Class ID of the media format
   uint16   wChannels;        // Number of channels. One for mono, two for stereo.
   uint32   dwSamplesPerSec;  // Channel sample rate in samples per second(Hertz). Typical values: 8kHz, 16kHz, 32kHz, etc.
   uint16   wBitsPerSample;   // Number of bits per sample
   flg      bUnsigned:1;      // TRUE if sample is interpreted as unsigned

   // Additional info:
   uint32   dwBytesPerSec;    // Required average data transfer rate in bytes per second
   uint16   wBlockSize;       // Minumum atomic unit data size: Num of bytes for n-channels
   uint32   dwExtra;          // Extra info or pointer to extra info specific to media. Zero if none.
} AEEMediaWaveSpec;

//-------------------------------------------------------------------
// IMA-ADPCM format: IMediaADPCM Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaADPCM)
{
   INHERIT_IMedia(IMedia);
};

//********************************************************************************************************************************
//
// IMediaAAC Interface
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      IMediaAAC Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// IMediaAAC Interface Definitions - C Style
//-------------------------------------------------------------------

QINTERFACE(IMediaAAC)
{
   INHERIT_IMedia(IMedia);
};

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
AEEMediaMIDISpec

Description: MIDI format specification

Definition:
   typedef struct AEEMediaMIDISpec
   {
      byte     nFormat;
      uint16   nTracks;
      int16    nDivision;
   } AEEMediaMIDISpec;

Members:
   nFormat:    SMF format: 0, 1 or 2
   nTracks:    Number of tracks in the SMF
   nDivision:  Timing info 
 
Comments:
   This structure is optionally returned in MM_MEDIA_SPEC callback by
   IMedia object handling MIDI format.

See Also:
   IMEDIA_Play()

======================================================================= 
AEEMediaMP3Spec

Description: MP3 format specification

Definition:
   typedef struct AEEMediaMP3Spec
   {
      int         nVersion;
      byte        nLayer;
      boolean     bCRCFlag;
      uint16      wBitRate;
      uint32      dwSampleRate;
      byte        nChannel;
      byte        nExtension;
      boolean     bCopyrightFlag;
      boolean     bOriginalFlag;
      byte        nEmphasis;
      char        szTitle[MM_MP3_ID3_TAG_LENGTH];
      char        szArtist[MM_MP3_ID3_TAG_LENGTH];
      char        szAlbum[MM_MP3_ID3_TAG_LENGTH];
      char        szYear[4];
      char        szComment[MM_MP3_ID3_TAG_LENGTH];
      byte        nGenre;
   } AEEMediaMP3Spec;

Members:
      nVersion:      MPEG version
      nLayer:        MPEG layer compression: 1, 2 or 3
      bCRCFlag:      True if CRC protection
      wBitRate:      Bit rate [Kilo bits]
      dwSampleRate:  Sampling rate [Kilo bits]
      nChannel:      Channel
      nExtension:    Only when JOINT_STEREO
      bCopyrightFlag:True if copyrighted
      bOriginalFlag: True if original
      nEmphasis:     Audio emphasis
      szTitle:       Song title
      szArtist:      Song performer
      szAlbum:       Album with the song
      szYear:        Year Album released
      szComment:     Text comment
      nGenre:        ID3 genre tag
 
Comments:
   This structure is optionally returned in MM_MEDIA_SPEC callback by
   IMedia object handling MP3 format.

See Also:
   IMEDIA_Play()

======================================================================= 
AEEMediaWaveSpec

Description: 
ADPCM or wave format specification.

Definition:
   typedef struct AEEMediaWaveSpec
   {
      uint16   wSize;            
      AEECLSID clsMedia;         
      uint16   wChannels;        
      uint32   dwSamplesPerSec;  
      uint32   dwBytesPerSec;    
      uint16   wBlockSize;       
      uint16   wBitsPerSample;   
      uint32   dwExtra;          
   } AEEMediaWaveSpec;

Members:
   wSize:            Size of this struct.
   clsMedia:         Class ID of the media format
   wChannels:        Number of channels. One for mono, two for stereo.
   dwSamplesPerSec:  Channel sample rate in samples per second(Hertz). Typical values: 8kHz, 16kHz, 32kHz, etc.
   dwBytesPerSec:    Required average data transfer rate in bytes per second
   wBlockSize:       Minumum atomic unit data size: Num of bytes for n-channels
   wBitsPerSample:   Number of bits per sample
   dwExtra:          Extra info or pointer to extra info specific to media. Zero if none.
 
Comments:
   This structure is optionally returned in MM_MEDIA_SPEC callback by
   IMedia object handling ADPCM or wave format.

See Also:
   IMEDIA_Play()



===================================================================== 
  INTERFACE DOCUMENTATION
===================================================================== 

IMedia Formats Interface
===pre>
   IMediaMIDI:    Handles playback of the standard MIDI format.
   IMediaMP3:     Handles playback of the MP3 format.
   IMediaQCP:     Handles playback and recording of the QCP (PureVoice) format.

   IMediaMIDIOutQCP: Handles playback of the QCP format through the MIDI out device.
   IMediaMIDIOutMsg: Renders MIDI messages directly to the MIDI out device.

   IMediaPMD: Handles playback of the PMD (WebAudio) format that contains audio, 
              video, and karaoke text.
===/pre>

Description: 
   Typically, you do not need to use the API of these interfaces directly but use 
   IMedia API instead. Use the API of these interfaces only if you need to do 
   something specific to that interface.

=============================================================================

   IMediaMIDI
   Handles playback of the standard MIDI format.
   See Also:
   IMedia Formats
==============================================================================
   IMediaMP3
   Handles playback of the MP3 format.
   See Also:
   IMedia Formats
==============================================================================
   IMediaQCP
   Handles playback and recording of the QCP (PureVoice) format.
   See Also:
   IMedia Formats
==============================================================================
   IMediaMIDIOutQCP
   Handles playback of the QCP format through the MIDI out device.
      See Also:
   IMedia Formats
==============================================================================
   IMediaMIDIOutMsg
   Renders MIDI messages directly to the MIDI out device.
   See Also:
   IMedia Formats
==============================================================================
   IMediaPMD
   Handles playback of the PMD (WebAudio) format that contains audio,
              video, and karaoke text.
   See Also:
   IMedia Formats
==============================================================================

IMEDIAQCP_SetFormat()

Description:
    This function sets the QCP format and is mainly used for recording.

Prototype:
    int IMEDIAQCP_SetFormat(IMedia * pIMedia, int nFormat);

Parameters:
    pIMedia: Pointer to the IMedia Interface object
    nFormat: Format of QCP

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    nFormat can take the following values:
~
   MM_QCP_FORMAT_FIXED_FULL_13K   : QCP format, Fixed-full rate, 13K vocoder~
   MM_QCP_FORMAT_VAR_FULL_13K     : QCP format, Variable rate, full rate max, 13K~
   MM_QCP_FORMAT_EVRC             : QCP format, Variable rate, full rate max, EVRC~
   MM_QCP_FORMAT_VAR_HALF_13K     : QCP format, Varibale rate, half rate max, 13K~
   MM_QCP_FORMAT_FIXED_HALF_13K   : QCP format, Fixed half rate, 13K vocoder~
   MM_QCP_FORMAT_FIXED_FULL_EVRC  : QCP format, Fixed-full rate, EVRC vocoder~
   MM_QCP_FORMAT_VAR_FULL_EVRC    : Same as MM_QCP_FORMAT_EVRC

   MM_QCP_FORMAT_FIXED_4_75_AMR   : QCP format, Fixed- 4.75 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_5_15_AMR   : QCP format, Fixed- 5.15 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_5_9_AMR    : QCP format, Fixed- 5.9  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_6_7_AMR    : QCP format, Fixed- 6.7  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_7_4_AMR    : QCP format, Fixed- 7.4  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_7_95_AMR   : QCP format, Fixed- 7.95 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_10_2_AMR   : QCP format, Fixed-10.2  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_12_2_AMR   : QCP format, Fixed-12.2  kbit/s, AMR~

   MM_AMR_FORMAT_FIXED_4_75_AMR   : AMR format, Fixed- 4.75 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_5_15_AMR   : AMR format, Fixed- 5.15 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_5_9_AMR    : AMR format, Fixed- 5.9  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_6_7_AMR    : AMR format, Fixed- 6.7  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_7_4_AMR    : AMR format, Fixed- 7.4  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_7_95_AMR   : AMR format, Fixed- 7.95 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_10_2_AMR   : AMR format, Fixed-10.2  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_12_2_AMR   : AMR format, Fixed-12.2  kbit/s, AMR~

   MM_QCP_FORMAT_VAR_FULL_4GV_NB  : QCP format, Variable rate, full rate max, 4GV-NB~
   MM_QCP_FORMAT_FIXED_FULL_4GV_NB: QCP format, Fixed rate, full rate max, 4GV-NB~

   MM_QCP_FORMAT_FIXED_FULL_EFR   : QCP format, Fixed full rate, EFR~
   MM_QCP_FORMAT_FIXED_FULL_FR    : QCP format, Fixed full rate, FR~
   MM_QCP_FORMAT_FIXED_FULL_HR    : QCP format, Fixed full rate, HR~

   MM_QCP_FORMAT_UNKNOWN:           Unknown format~
*

If you specify, 
   MM_PARM_QCP_FORMAT_XXX_13K, then the recorded format is QCP with QCELP content(.qcp file) 
   MM_PARM_QCP_FORMAT_XXX_EVRC, then the recorded format is QCP with EVRC content(.qcp file) 
   MM_PARM_QCP_FORMAT_XXX_AMR, then the recorded format is QCP with AMR content (.qcp file) 
   MM_PARM_AMR_FORMAT_XXX, then the recorded format is AMR (.amr file) 

Version:
    Introduced BREW Client 2.0

See Also:
    IMEDIAQCP_GetFormat()
    Return to the List of Functions

=============================================================================

IMEDIAQCP_GetFormat()

Description: 
   This function retrieves the QCP format.

Prototype:
   int IMEDIAQCP_GetFormat(IMedia * po, int * pnFormat);

Parameters:
   po :        Pointer to the IMedia Interface object
   pnFormat [out]:    Pointer to format of QCP

Return Value:
   SUCCESS:    Successful
   Error code, if otherwise:~
   MM_PENDING: Value returned in the registered callback
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Parm not supported by the class

Comments:
   *pnFormat can take the following values:
~
   MM_QCP_FORMAT_FIXED_FULL_13K   : QCP format, Fixed-full rate, 13K vocoder~
   MM_QCP_FORMAT_VAR_FULL_13K     : QCP format, Variable rate, full rate max, 13K~
   MM_QCP_FORMAT_EVRC             : QCP format, Variable rate, full rate max, EVRC~
   MM_QCP_FORMAT_VAR_HALF_13K     : QCP format, Varibale rate, half rate max, 13K~
   MM_QCP_FORMAT_FIXED_HALF_13K   : QCP format, Fixed half rate, 13K vocoder~
   MM_QCP_FORMAT_FIXED_FULL_EVRC  : QCP format, Fixed-full rate, EVRC vocoder~
   MM_QCP_FORMAT_VAR_FULL_EVRC    : Same as MM_QCP_FORMAT_EVRC

   MM_QCP_FORMAT_FIXED_4_75_AMR   : QCP format, Fixed- 4.75 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_5_15_AMR   : QCP format, Fixed- 5.15 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_5_9_AMR    : QCP format, Fixed- 5.9  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_6_7_AMR    : QCP format, Fixed- 6.7  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_7_4_AMR    : QCP format, Fixed- 7.4  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_7_95_AMR   : QCP format, Fixed- 7.95 kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_10_2_AMR   : QCP format, Fixed-10.2  kbit/s, AMR~
   MM_QCP_FORMAT_FIXED_12_2_AMR   : QCP format, Fixed-12.2  kbit/s, AMR~

   MM_AMR_FORMAT_FIXED_4_75_AMR   : AMR format, Fixed- 4.75 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_5_15_AMR   : AMR format, Fixed- 5.15 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_5_9_AMR    : AMR format, Fixed- 5.9  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_6_7_AMR    : AMR format, Fixed- 6.7  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_7_4_AMR    : AMR format, Fixed- 7.4  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_7_95_AMR   : AMR format, Fixed- 7.95 kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_10_2_AMR   : AMR format, Fixed-10.2  kbit/s, AMR~
   MM_AMR_FORMAT_FIXED_12_2_AMR   : AMR format, Fixed-12.2  kbit/s, AMR~

   MM_QCP_FORMAT_VAR_FULL_4GV_NB  : QCP format, Variable rate, full rate max, 4GV-NB~
   MM_QCP_FORMAT_FIXED_FULL_4GV_NB: QCP format, Fixed rate, full rate max, 4GV-NB~

   MM_QCP_FORMAT_FIXED_FULL_EFR   : QCP format, Fixed full rate, EFR~
   MM_QCP_FORMAT_FIXED_FULL_FR    : QCP format, Fixed full rate, FR~
   MM_QCP_FORMAT_FIXED_FULL_HR    : QCP format, Fixed full rate, HR~

   MM_QCP_FORMAT_UNKNOWN:           Unknown format~
*

Side Effects: 

Version:
    Introduced BREW Client 2.0

See Also:
   IMEDIAQCP_GetFormat()
   IMEDIA_GetMediaParm()
   IMEDIA_SetMediaParm()

=============================================================================
IMEDIAMIDIOUTMSG_SetMIDIMsg()

Description:
    This function sets the buffer containing MIDI messages as media data.

Prototype:
    int IMEDIAMIDIOUTMSG_SetMIDIMsg
    (
    IMedia * pIMedia, 
    uint8 * pMsgBuf, 
    uint32 dwSize
    )

Parameters:
    pIMedia  :  Pointer to the IMedia Interface object
    pMsgBuf :  Buffer containing MIDI messages
    dwSize :  Size of the buffer

Return Value:
    SUCCESS: if successful
    Error code, if otherwise:~
    MM_PENDING: Value returned in the registered callback
    EBADPARM: Bad parm
    EBADSTATE: Cannot get parm in the current state
    EUNSUPPORTED: Parm not supported by the class

Comments:
    This API must be called before you begin the playback.

Version:
    Introduced BREW Client 2.0

See Also:
   IMEDIA_GetMediaParm()
   IMEDIA_SetMediaParm()
===================================================================== 

============================================================================= */
#endif // AEEMEDIAFORMATS_H

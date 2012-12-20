#ifndef AEEMEDIAMPEG4_H
#define AEEMEDIAMPEG4_H

/*============================================================================
FILE:   AEEMediaMPEG4.h

SERVICES:  BREW Movie File Services

DESCRIPTION:
   This file defines the IMediaMPEG4 interface that is used to read and write
   audio and video stream data in a movie file.
   
PUBLIC CLASSES:  IMediaMPEG4

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*!
  @file AEEMediaMPEG4.h
  
  @brief IMediaMPEG4 API

  IMediaMPEG4 is a sub-class extension of IMedia.

 */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/staticextensions/main/latest/inc/aeemediampeg4.h#17 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $


========================================================================== */

/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEMedia.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//
// Status codes
//
// Status code is returned via registered callback function to 
// indicate event status and to return data to client.
// AEEMediaNotify::nStatus sent via callback function contains 
// the following status codes.
//

#define REMOVED_CARRIER_NAMES
#define MM_MPEG4_QTV_MAX_LANGUAGE_BYTES    4

// MP4 only status
#define MM_MP4_STATUS_3GPP_TTEXT        (MM_STATUS_USER_BASE + 1)   // Next 3GPP Timed Text is READY to display
#define MM_MP4_STATUS_TELOP_TEXT        (MM_STATUS_USER_BASE + 2)   // Next Telop Text is READY to display

// Some presently MP4, BUT POTENTIALLY MEDIA COMMON STATUS
#define MM_MP4_STATUS_BUFFER_UPDATE     (MM_STATUS_USER_BASE + 3)   // indicate Buffering
#define MM_MP4_STATUS_CONNECTION_UPDATE (MM_STATUS_USER_BASE + 4)   // streaming Setup in progress indication 
#define MM_MP4_STATUS_PB_READY          (MM_STATUS_USER_BASE + 5)   // indicate moving to Playback Ready State 
#define MM_MP4_STATUS_OPEN_COMPLETE     (MM_STATUS_USER_BASE + 6)   // indicate open file complete
#define MM_MP4_STATUS_NO_SYNC_FRAME     (MM_STATUS_USER_BASE + 7)   // no sync frame error (seek may fail)
#define MM_MP4_STATUS_RTSP              (MM_STATUS_USER_BASE + 8)   // RTSP status code
#define MM_MP4_STATUS_RESTARTING_VIDEO  (MM_STATUS_USER_BASE + 9)   // restarting video in loop mode
#define MM_MP4_STATUS_RESTARTING_AUDIO  (MM_STATUS_USER_BASE +10)   // restarting audio in loop mode
#define MM_MP4_STATUS_RESTARTING_TEXT   (MM_STATUS_USER_BASE +11)   // restarting text in loop mode
#define MM_MP4_STATUS_PS_BUFFER_UPDATE  (MM_STATUS_USER_BASE +12)   // pseudo stream is in buffering state
#define MM_MP4_STATUS_UNHANDLED_SDP     (MM_STATUS_USER_BASE +13)   // unhandled sdp line
#define MM_MP4_STATUS_UNHANDLED_RTSP    (MM_STATUS_USER_BASE +14)   // unhandled rtsp header
#define MM_MP4_STATUS_AUDIO_ABORT       (MM_STATUS_USER_BASE +15)   // Error in audio track
#define MM_MP4_STATUS_VIDEO_ABORT       (MM_STATUS_USER_BASE +16)   // Error in video track
#define MM_MP4_STATUS_MIDI_AVAILABLE    (MM_STATUS_USER_BASE +17)   // MIDI data is available for user
#define MM_MP4_STATUS_PLAYING           (MM_STATUS_USER_BASE +18)   // Qtv is playing after reposition or start of play
#define MM_MP4_STATUS_FILE_SYS_LIMIT    (MM_STATUS_USER_BASE+19)    // File sys limit is approaching during stream recording
#define MM_MP4_STATUS_FILE_SYS_FULL       (MM_STATUS_USER_BASE+20)  // File sys is FULL
#define MM_MP4_STATUS_FILE_ALREADY_EXISTS (MM_STATUS_USER_BASE+21)  // File already Exists, Cannot overwrite
#define MM_MP4_STATUS_FILE_OW_FAILURE     (MM_STATUS_USER_BASE+22)  // Failed to overwrite a file
#define MM_MP4_STATUS_RECORDER_FAILED     (MM_STATUS_USER_BASE+23)  // Stream recorder failed
#define MM_MP4_STATUS_RECORD_COMPLETED    (MM_STATUS_USER_BASE+24)  // Stream record completed

#define MM_MP4_STATUS_VIDEO_MEDIA_END     (MM_STATUS_USER_BASE+25)  // video media ends, but still video maybe playing
#define MM_MP4_STATUS_AUDIO_MEDIA_END     (MM_STATUS_USER_BASE+26)  // audio media ends, but still audio maybe playing
#define MM_MP4_STATUS_TEXT_MEDIA_END      (MM_STATUS_USER_BASE+27)  // text media ends, but still text maybe playing

#define MM_MP4_STATUS_PSEUDO_PAUSE        (MM_STATUS_USER_BASE+28) // While Pseudo Streaming not enough data so PSEUDO PAUSE
#define MM_MP4_STATUS_PSEUDO_RESUME       (MM_STATUS_USER_BASE+29) // While Pseudo Streaming enough data to resume

#define MM_MP4_STATUS_HTTP_PAUSE          (MM_STATUS_USER_BASE+30) // While HTTP Streaming not enough data so HTTP PAUSE
#define MM_MP4_STATUS_HTTP_RESUME         (MM_STATUS_USER_BASE+31) // While HTTP Streaming enough data to resume
#define MM_MP4_STATUS_RECORDED_CLIP_DURATION ( MM_STATUS_USER_BASE + 32)  // gives recorded clip duration, record_duration = *((uint32 *)pCmdNotify->pCmdData)
#define MM_MP4_STATUS_TS_LAYER_PRESENT  (MM_STATUS_USER_BASE+33) // Add for dectection of TS LAYER
#define MM_MP4_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED  (MM_STATUS_USER_BASE+34) // video resolution not supported
#define MM_MP4_STATUS_INFO               (MM_STATUS_USER_BASE+35)  // Status information passed the app 
                                                                   
#define MM_MP4_STATUS_DL_STOPPED          (MM_STATUS_USER_BASE+36) // HTTP Download stopped
#define MM_MP4_STATUS_DL_COMPLETE         (MM_STATUS_USER_BASE+37) // HTTP Downloading completed                                                                   
#define MM_MP4_STATUS_DL_DATA_AVAILABLE   (MM_STATUS_USER_BASE+38) // Downloaded Data available
#define MM_MP4_STATUS_PLAYLIST_CLIP_TRANSITION (MM_STATUS_USER_BASE+39)  // Clip transition
#define MM_MP4_STATUS_PLAYLIST_SKIP_COMPLETE (MM_STATUS_USER_BASE+40)  // Succeeded in skipping to a different clip
#define MM_MP4_STATUS_PLAYLIST_SKIP_FAILED (MM_STATUS_USER_BASE+41)  // Failed to skip to a different clip

#define MM_MP4_STATUS_DRM_AUTHORIZATION_ERROR          (MM_STATUS_USER_BASE+42)  // Authorization error
#define MM_MP4_STATUS_DRM_DEVICE_REGISTRATION_ERROR    (MM_STATUS_USER_BASE+43)  // Device registration error
#define MM_MP4_STATUS_DRM_RENTAL_COUNT_EXPIRED         (MM_STATUS_USER_BASE+44)  // Rental expired
#define MM_MP4_STATUS_DRM_PLAYBACK_GENERAL_ERROR       (MM_STATUS_USER_BASE+45)  // General DRM error
#define MM_MP4_STATUS_CONSUME_RENTAL_VIEW_CONFIRMATION (MM_STATUS_USER_BASE+46)  // Confirmation needed before consuming play count
#define MM_MP4_STATUS_PAUSED_SUSPENDED                 (MM_STATUS_USER_BASE+47)  // Qtv is suspended due to incoming call
#define MM_MP4_STATUS_GENERIC_TTEXT                    (MM_STATUS_USER_BASE+48) 		
#define MM_MP4_STATUS_FCS_DONE                         (MM_STATUS_USER_BASE+49)  // FCS playback done status for the current clip	
#define MM_MP4_STATUS_FCS_SWITCHING                    (MM_STATUS_USER_BASE+50) // FCS switching to new clip is in progress																	 // 
#define MM_MP4_STATUS_CLIPINFO_DIMENSION_READY	       (MM_STATUS_USER_BASE+51) //Valid Height and width of clip
#define MM_MP4_STATUS_TRACKLIST_CODEC_CHECK_DONE       (MM_STATUS_USER_BASE+52)  // tracklist check for supporting the mime type
#define MM_MP4_STATUS_RECONNECT_SUCCESS                (MM_STATUS_USER_BASE+53)   // reconnect succeeded 
#define MM_MP4_STATUS_RECONNECT_FAILED                 (MM_STATUS_USER_BASE+54)   // reconnect failed
#define MM_MP4_STATUS_RECONNECT_IN_PROGRESS            (MM_STATUS_USER_BASE+55)   // reconnect is in progress
#define MM_MP4_STATUS_FCS_SWITCH_SUPPORTED             (MM_STATUS_USER_BASE+56)   //FCS switching supported
#define MM_MP4_STATUS_RECONNECTING_USING_TCP_INTERLEAVE (MM_STATUS_USER_BASE+57)  //reconnecting using tcp interleaving																		 
//
// MP4 only parameters 
//
#define MM_MP4_PARM_ROTATION            (MM_PARM_USER_BASE + 1)    // Rotation parameter
#define MM_MP4_PARM_SCALING             (MM_PARM_USER_BASE + 2)    // Scaling parameter
#define MM_MP4_PARM_TELOP_SUB_STR       (MM_PARM_USER_BASE + 3)   // Gets the current telop's string.

//
// Some presently MP4, BUT POTENTIALLY MEDIA COMMON parameters
//
#define MM_MP4_PARM_BUFFER_UPDATE       (MM_PARM_USER_BASE + 4)  // Indicate Buffering
#define MM_MP4_PARM_CONNECT_UPDATE      (MM_PARM_USER_BASE + 5)  // Indicate Connection Setup to user
#define MM_MP4_PARM_PB_READY            (MM_PARM_USER_BASE + 6)  // Indicate PB Ready
#define MM_MP4_PARM_MEDIA_SPEC          (MM_PARM_USER_BASE + 7)  // Retrieves the MediaSpec

//
// Sub status to indicate that stream requires authentication 
//
#define MM_MP4_STREAM_AUTHENTICATE      (MM_PARM_USER_BASE + 8)   // Sub status to indicate that stream requires authentication 

// Set separate audio and video files 
//   Use this instead of MM_PARM_MEDIA_DATA to use different audio and video 
//   file names.  Param 1 == video, Param 2 == audio, 
//   To disable video, set Param 1 to NULL
//   To disable audio, set Param 2 to NULL 
#define MM_MP4_AUDIO_VIDEO_MEDIA_DATA   (MM_PARM_USER_BASE + 9) // Param 1 == video, Param 2 == audio file name

// ex: IMEDIA_GetMediaParm( pme->m_pMedia,
//                          MM_MP4_PARM_TRACK_TIME_SCALE,
//                          (int32 *)VIDEO_TRACK,
//                          (int32*)&timescale ) 
#define MM_MP4_PARM_TRACK_TIME_SCALE    (MM_PARM_USER_BASE + 10)  // gives track's time scale

// Sets the override value of audio format. If this is set, Audio
//   format for DSP is chosen by this value and audio format of
//   MP4 file is ignored.
//   Param 1 == audio type class ID
#define MM_MP4_PARAM_AUDIO_OVERRIDE     (MM_PARM_USER_BASE + 11)

// Enable/disable RTSP status notification to the BREW App
#define MM_MP4_PARM_RTSP_STATUS_NOTIFY  (MM_PARM_USER_BASE + 12)

// p1 has value true/false
#define MM_MP4_PARM_LOOP_TRACK          (MM_PARM_USER_BASE + 13)

// p1 = *playback time structure
#define MM_MP4_PARAM_PLAYBACK_POS       (MM_PARM_USER_BASE + 14)

#define MMD_HTTP_PUSH_BUFFER_PLAYBACK   (MMD_BASE + 2)
#define MMD_HTTP_PULL_BUFFER_PLAYBACK   (MMD_BASE + 3)
#define MMD_BUFFER_FRAGMENT             (MMD_HTTP_PUSH_BUFFER_PLAYBACK)

#define MM_MP4_PARAM_PSBUF_WRITE_OFFSET (MM_PARM_USER_BASE + 15)
#define MM_MP4_PARM_PS_BUFFER_UPDATE    (MM_PARM_USER_BASE + 16) 

#define MM_MP4_SCALING_UPDATE           (MM_PARM_USER_BASE + 17)

// p1= pointer to AEEMFDecryptKey struct, p2=size
// see below for this definition
//#define MM_MP4_PARM_DECRYPT_KEY         (MM_PARM_USER_BASE + 18)

//p1 = pointer to AEEStreamRecordInfo structure. 
#define MM_MP4_PARM_RTSP_STREAM_CONVERT  (MM_PARM_USER_BASE + 19) // Start RTSP stream recording

// get midi data size, p1 = pointer to uint32 for size
#define MM_MP4_PARM_MIDI_SIZE           (MM_PARM_USER_BASE + 20)

// get midi data, p1 = buf pointer,
// p2 = pointer to uint32 has size, will have bytes copied
#define MM_MP4_PARM_MIDI_DATA           (MM_PARM_USER_BASE + 21)

#define MM_MP4_PARM_PB_ABORT            (MM_PARM_USER_BASE + 22)  // Indicate Playback Abort

//Stop RTSP stream recording
#define MM_MP4_PARM_RTSP_STOP_STR_CONVERT  (MM_PARM_USER_BASE + 23)

// Set Network QoS Policy.
// p1 = pointer to AEENetPolicyInfo structure
// NetPolicy info will take effect only when it is set before 
// moving Mpeg4 IMEDIA object to "Ready" state.
//Ex:IMEDIA_SetMediaParm(pMedia,
//                      MM_MP4_PARM_NET_QOS_POLICY,
//                      (int32)(pmd->qosInfo),
//                      0);
//
// Move Mpeg4 IMEDIA object to "Ready" state
// IMEDIA_SetMediaData((IMedia *)(p),(int32)(pmd))
//
#define MM_MP4_PARM_NET_QOS_POLICY      (MM_PARM_USER_BASE + 24)

// get link data size, p1 = pointer to uint32 for size
#define MM_MP4_PARM_LINK_SIZE           (MM_PARM_USER_BASE + 25)

// get link data, p1 = buf pointer,
// p2 = pointer to uint32 has size, will have bytes copied
#define MM_MP4_PARM_LINK_DATA           (MM_PARM_USER_BASE + 26)

//’ftyp’ atom
// To get 'ftyp' data size:
// IMEDIA_GetMediaParm() parameters:
// p1 = uint32 pointer, will have size of the atom.
//      0 if atom not present in the media content.
//
// To get 'ftyp' data:
// IMEDIA_GetMediaParm() parameters:
// p1 = buf pointer,
// p2 = uint32 pointer, will have size of the bytes 
//      copied in the buffer.
#define MM_MP4_PARM_ATOM_FTYP_SIZE           (MM_PARM_USER_BASE + 28)
#define MM_MP4_PARM_ATOM_FTYP_DATA           (MM_PARM_USER_BASE + 29) // Get p1 = buf pointer, p2 = uint32 pointer, will have size of the bytes copied in the buffer.


//’dcmd’ atom
#define MM_MP4_PARM_ATOM_DCMD_SIZE           (MM_PARM_USER_BASE + 30)
#define MM_MP4_PARM_ATOM_DCMD_DATA           (MM_PARM_USER_BASE + 31)

//’cprt’ atom
#define MM_MP4_PARM_ATOM_UDTA_CPRT_SIZE      (MM_PARM_USER_BASE + 32)
#define MM_MP4_PARM_ATOM_UDTA_CPRT_DATA      (MM_PARM_USER_BASE + 33)

//’auth’ atom
#define MM_MP4_PARM_ATOM_UDTA_AUTH_SIZE      (MM_PARM_USER_BASE + 34)
#define MM_MP4_PARM_ATOM_UDTA_AUTH_DATA      (MM_PARM_USER_BASE + 35)

//’titl’ atom
#define MM_MP4_PARM_ATOM_UDTA_TITL_SIZE      (MM_PARM_USER_BASE + 36)
#define MM_MP4_PARM_ATOM_UDTA_TITL_DATA      (MM_PARM_USER_BASE + 37)

//’dscp’ atom
#define MM_MP4_PARM_ATOM_UDTA_DSCP_SIZE      (MM_PARM_USER_BASE + 38)
#define MM_MP4_PARM_ATOM_UDTA_DSCP_DATA      (MM_PARM_USER_BASE + 39)

//Text track origin-x
#define MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_SIZE  (MM_PARM_USER_BASE + 40)
#define MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_DATA  (MM_PARM_USER_BASE + 41)

//Text track origin-y
#define MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_SIZE  (MM_PARM_USER_BASE + 42)
#define MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_DATA  (MM_PARM_USER_BASE + 43)

//Text track width
#define MM_MP4_PARM_TEXT_TKHD_WIDTH_SIZE     (MM_PARM_USER_BASE + 44)
#define MM_MP4_PARM_TEXT_TKHD_WIDTH_DATA     (MM_PARM_USER_BASE + 45)

//Text track height
#define MM_MP4_PARM_TEXT_TKHD_HEIGHT_SIZE    (MM_PARM_USER_BASE + 46)
#define MM_MP4_PARM_TEXT_TKHD_HEIGHT_DATA    (MM_PARM_USER_BASE + 47)

//Progressive Download: Startup and Buffering time
#define MM_MP4_PARM_HTTP_SETUP_TIME     (MM_PARM_USER_BASE + 48)

// Parameter to modify the point at which QTV streaming starts rebuffering 
#define MM_MP4_PARAM_MODIFY_BUFFER_LOWMARK (MM_PARM_USER_BASE + 49)

// parameter to set QTV streaming UDP data port range
// p1 = begin port, p2 = end port
#define MM_MP4_PARAM_SET_DATA_PORT_RANGE  (MM_PARM_USER_BASE + 50)

// Parameter for copying downloladed clip in a buffer to a file
#define MM_MP4_PARAM_COPY_BUFFER_TO_FILE   (MM_PARM_USER_BASE + 51)

// [Get] Video Codec Type, P1 = (AEEMediaMP4CodecType *)
#define MM_MP4_PARM_VIDEO_CODEC             (MM_PARM_USER_BASE + 52)

// [Get] Audio Codec Type, P1 = (AEEMediaMP4CodecType *)
#define MM_MP4_PARM_AUDIO_CODEC             (MM_PARM_USER_BASE + 53)

// [Get] Video Frame Rate , P1 = (float *)
#define MM_MP4_PARM_VIDEO_FRAME_RATE        (MM_PARM_USER_BASE + 54)

// [Get] Video Bit Rate, P1 = (unsigned long *)
#define MM_MP4_PARM_VIDEO_AVERAGE_BIT_RATE  (MM_PARM_USER_BASE + 56)

// [Get] Audio Bit Rate, P1 = (unsigned long *)
#define MM_MP4_PARM_AUDIO_AVERAGE_BIT_RATE  (MM_PARM_USER_BASE + 57)

// Fast track download progress update
#define MM_STATUS_DOWNLOAD_TICK_UPDATE     (MM_PARM_USER_BASE + 58)

// [Get] Extended Frame Info (AEEMediaExtFrameInfoHeader *)
#define MM_MP4_PARM_FRAME_EXT               (MM_PARM_USER_BASE + 59)

#define MM_MP4_PARM_USER_AGENT              (MM_PARM_USER_BASE + 60)

// Fasttrack Dir path to save downloaded file
#define MM_MP4_PARAM_SET_PVX_SAVE_OPTION    (MM_PARM_USER_BASE + 61)

// Fasttrack Preroll
#define MM_MP4_PARAM_SET_PVX_PREROLL        (MM_PARM_USER_BASE + 62)

#define MM_MP4_PARM_TOTAL_RX_DATA_RATE      (MM_PARM_USER_BASE + 63)

// HTTP Download Free the buffer 
#define MM_MP4_PARAM_HTTP_FREE_DOWNLOAD_BUFFER (MM_PARM_USER_BASE + 64)

// DRM Callback registering
// Callback to get data size
#define MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA_SIZE (MM_PARM_USER_BASE + 65)
// Callback to get data buffer                           
#define MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA (MM_PARM_USER_BASE + 66)
// Callback to figure if MIME is supported or not                              
#define MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_SUPPORTED_TYPE (MM_PARM_USER_BASE + 67)


// HTTP Download Dir path to save downloaded file
#define MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION  (MM_PARM_USER_BASE + 68)

// HTTP Download Data fetch if Encrypted
#define MM_MP4_PARAM_HTTP_GET_ENCRYPTED_DATA (MM_PARM_USER_BASE + 69)

//'rtng' atom
#define MM_MP4_PARM_ATOM_UDTA_RTNG_SIZE      (MM_PARM_USER_BASE + 70)
#define MM_MP4_PARM_ATOM_UDTA_RTNG_DATA      (MM_PARM_USER_BASE + 71)

//'perf' atom
#define MM_MP4_PARM_ATOM_UDTA_PERF_SIZE      (MM_PARM_USER_BASE + 72)
#define MM_MP4_PARM_ATOM_UDTA_PERF_DATA      (MM_PARM_USER_BASE + 73)

//'clsf' atom
#define MM_MP4_PARM_ATOM_UDTA_CLSF_SIZE      (MM_PARM_USER_BASE + 74)
#define MM_MP4_PARM_ATOM_UDTA_CLSF_DATA      (MM_PARM_USER_BASE + 75)

//'gnre' atom
#define MM_MP4_PARM_ATOM_UDTA_GNRE_SIZE      (MM_PARM_USER_BASE + 76)
#define MM_MP4_PARM_ATOM_UDTA_GNRE_DATA      (MM_PARM_USER_BASE + 77)

//'kywd' atom
#define MM_MP4_PARM_ATOM_UDTA_KYWD_SIZE      (MM_PARM_USER_BASE + 78)
#define MM_MP4_PARM_ATOM_UDTA_KYWD_DATA      (MM_PARM_USER_BASE + 79)

//'loci' atom
#define MM_MP4_PARM_ATOM_UDTA_LOCI_SIZE      (MM_PARM_USER_BASE + 80)
#define MM_MP4_PARM_ATOM_UDTA_LOCI_DATA      (MM_PARM_USER_BASE + 81)


// Arbitrary scaling parameter
#define MM_MP4_PARM_ASCALING                 (MM_PARM_USER_BASE + 82)

// Priority for multiple instances, should only be set before IMEDIA_SetMediaData
#define MM_MP4_PARM_PRIORITY                 (MM_PARM_USER_BASE + 83)

// p1 - Track bitmask may be audio(MM_QTV_AUDIO_TRACK), text(MM_QTV_TEXT_TRACK) or both.
// p2 - 1 to mute, 0 to unmute
#define MM_MP4_PARM_MUTE_TRACK               (MM_PARM_USER_BASE + 84)

// This method can be called only when the player state is PB_READY.
// p1 - Track bitmask may combine MM_QTV_AUDIO_TRACK, MM_QTV_TEXT_TRACK & MM_QTV_VIDEO_TRACK
// p2 - AEETrackID * with requested track IDs                                                                     
#define MM_MP4_PARM_SELECT_PB                (MM_PARM_USER_BASE + 85)

// Read # of tracks or track info, depending on which pointer is allocated
// p1 = uint32*, p2 = AEETrackList *
#define MM_MP4_PARM_READ_TRACK               (MM_PARM_USER_BASE + 86)

// Absolute time repositioning, only to be used during pause, p1 msec
#define MM_MP4_PARM_REPOSITION               (MM_PARM_USER_BASE + 87)

// FF/RW # of Iframes, only to be set while not playing, p1 # sync frames
// IMEDIA_PLAY must be called after this parm is set to play ff/rw by Iframe
#define MM_MP4_PARM_PLAY_IFRAME_MODE         (MM_PARM_USER_BASE + 88)

//'meta' atom
#define MM_MP4_PARM_ATOM_UDTA_META_SIZE      (MM_PARM_USER_BASE + 89)
#define MM_MP4_PARM_ATOM_UDTA_META_DATA      (MM_PARM_USER_BASE + 90)

// Playback speed setting
// Specifies the speed for the next play command
#define MM_MP4_PARM_PB_SPEED                  (MM_PARM_USER_BASE + 91)

// Play live option
// Converts the next play command from 'resume (from TSB)' to 'play live'
// Remove this parm as it was a temporary solution for an SBA.  Now we have a
// longer term solution already in place.  However, we can never reuse +92 due
// to backward/forward compatibility.
// #define MM_MP4_PARM_RESUME_LIVE              (MM_PARM_USER_BASE + 92) 

// Dual mono output selection
// Specifies the dual mono channel to speaker mapping
#define MM_MP4_PARM_DUAL_MONO                (MM_PARM_USER_BASE + 93)

// Start position
// Specifies the 'start from' time for the first play command
#define MM_MP4_PARM_START_FROM_POS           (MM_PARM_USER_BASE + 94)

// Register the decryption callback using QtvPlayer::RegisterDRMDecryptMethod
// OEM layer should provide a DRM decryption method to allow encrypted WM file
// be sent to Qtv for playback
#define MM_MP4_PARM_DRM_DECRYPTION          (MM_PARM_USER_BASE + 95)           

#define MM_MP4_PARM_FCS_PROBE              (MM_PARM_USER_BASE + 96) 
#define MM_MP4_PARM_FCS_SWITCH             (MM_PARM_USER_BASE + 97) 

// p1= pointer to AEEMFDecryptKey struct (QTV threats it as void pointer), p2=size
// SKA - special case, please do not change
#define MM_MP4_PARM_DECRYPT_KEY         (MM_PARM_USER_BASE + 200)

// FEATURE_QTV_BCAST_FLO
// p1 = Ptr to AEEMMpeg4MediaInfo (QTV treats it as void pointer)
// p2 = size of struct
#define MM_MP4_PARM_MEDIA_INFO          (MM_PARM_USER_BASE + 201)
                                                                   
//FEATURE_QTV_ENCRYPTED_STREAMS: IPSEC
#define MM_MP4_PARM_DESCRAMBLER_HANDLER (MM_PARM_USER_BASE + 202)
                                                                   
// [Get] Video Frame Rate , P1 = (double *)
#define MM_MP4_PARM_VIDEO_FRAME_RATE_EX (MM_PARM_USER_BASE + 203)

//Used in GetMediaParm to retrieve device registration code
//p1 = Ptr to retrieve the code and 
//p2 = Indicate MAX size allocated. 
//When P1 is NULL, Code length is returned in P2
#define MM_MP4_PARM_DRM_REG_CODE            (MM_PARM_BASE + 204)  


// Callback registering for PULL mode replay
// Callback to get data size
#define MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE (MM_PARM_USER_BASE + 205)
// Callback to get data buffer                           
#define MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA      (MM_PARM_USER_BASE + 206)


/* these values are only for baclward compatibility. Now uses should use the
   new values in AEEMediaMpeg4.h */
#define MM_MPEG4_KDDI_MAX_TEXT_COLORS   MM_MPEG4_MAX_TEXT_COLORS
#define MM_MPEG4_3GPP_MAX_FONT_LEN      MM_MPEG4_MAX_FONT_LEN
#define MM_MPEG4_KDDI_FORMAT_TYPE       MM_MPEG4_UUID_FORMAT_TYPE
#define AEEMediaMPEG4KDDISpec           AEEMediaMPEG4UUIDSpec
#define KDDISpec                        UUIDSpec

/*-------------------------------------------------------------------
            Macros
-------------------------------------------------------------------*/
#define IMEDIA_GetMediaSpec(p, ppSpec, pSize)   \
  IMEDIA_GetMediaParm((IMedia *)(p), MM_MP4_PARM_MEDIA_SPEC, (int32 *)(ppSpec), (int32 *)(pSize))

#define IMEDIAMPEG4_GetFrameExt(p, pb, pe)   \
  IMEDIA_GetMediaParm((IMedia *)(p), MM_MP4_PARM_FRAME_EXT, (int32 *)(pb), (int32 *)(pe))

/* IMEDIA wrapper for setting QoS network policy information */
#define IMEDIA_SetQoSMediaData(p,pmd) \
        IMEDIA_SetMediaParm((IMedia *)(p), MM_PARM_MEDIA_DATA, (int32)(pmd), 0)

#define IMEDIA_GetPlaybackPositions(p, pPlaybackPos) \
          IMEDIA_GetMediaParm((IMedia *)(p), \
                               MM_MP4_PARAM_PLAYBACK_POS,\
                               (int32*)(pPlaybackPos),\
                               NULL)

#define IMEDIA_SetPseudoStreamMediaData(p, pmd)   \
 IMEDIA_SetMediaParm((IMedia *)(p), MM_MP4_PARAM_PSBUF_WRITE_OFFSET, (int32)(pmd), 0)


#define IMEDIA_SetHTTPStreamMediaData(p, pmd)   \
 IMEDIA_SetMediaParm((IMedia *)(p), MM_MP4_PARM_HTTP_SETUP_TIME, (int32)(pmd), 0)


/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/

#define MM_MPEG4_VOLUME_STEP                       25

#define MM_MPEG4_MAX_TITLE_BYTES                   50
#define MM_MPEG4_MAX_AUTHOR_BYTES                  50
#define MM_MPEG4_MAX_COPYRIGHT_BYTES              128
#define MM_MPEG4_MAX_RATING_BYTES                 128
#define MM_MPEG4_MAX_DESCRIPTION_BYTES            256
#define MM_MPEG4_MAX_OWNER_BYTES                  128
#define MM_MPEG4_MAX_VERSION_BYTES                128
#define MM_MPEG4_MAX_URN_BYTES                    1024
#define MM_MPEG4_MAX_CREATIONDATE_BYTES           128
#define MM_MPEG4_MAX_DEVICE_NAME_BYTES              9
#define MM_MPEG4_MAX_MODEL_NAME_BYTES               9
#define MM_MPEG4_MAX_ENCODER_INFO_BYTES             9
#define MM_MPEG4_MAX_MUX_INFO_BYTES                 9
#define MM_MPEG4_MAX_GPS_LATITUDE                   3
#define MM_MPEG4_MAX_GPS_LONGITUDE                  3
#define MM_MPEG4_MAX_GPS_TIME                       3
#define MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES          9
#define MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES  64
#define MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES    64
#define MM_MPEG4_MAX_GPS_DATE_BYTES                12
#define MM_MPEG4_MAX_ALBUM_BYTES                   50

#define MM_MPEG4_MAX_TEXT_COLORS                    4
#define MM_MPEG4_MAX_FONT_LEN                      32

#define MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES      64
#define MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES 256
#define MM_DEFAULT_TICK_INTERVAL                  500 /* in milliseconds */

// added to compile with BREW 3.0 will have to remove it after change
// is added to AEEMedia.h

#ifndef FEATURE_BREW_3_0
#define MM_SEEK_ACCESS_POINT 3
#endif

#define MM_QTV_VIDEO_TRACK 1
#define MM_QTV_AUDIO_TRACK 2
#define MM_QTV_TEXT_TRACK 4

/*!
  @brief MUTE defines for MP
*/
#define MM_MP4_TRACK_MUTE     0x01
#define MM_MP4_TRACK_UNMUTE   0x02

/*! 
  @brief MPEG4RotationType Enumeration
*/
typedef enum AEEMediaMPEG4RotationType
{
  MM_MPEG4_NO_ROTATION=0,     //!< MPEG4 is not rotated
  MM_MPEG4_90_CW_ROTATION,    //!< MPEG4 rotated 90 degrees clockwise
  MM_MPEG4_90_CCW_ROTATION,   //!< MPEG4 rotated 90 degrees counter clockwise
  MM_MPEG4_180_ROTATION       //!< MPEG4 rotated 180 degrees counter clockwise
} AEEMediaMPEG4RotationType;

/*!
  @brief MPEG4TrakType Enumeration
*/ 
typedef enum AEEMediaMPEG4TrackType
{
  VIDEO_TRACK,
  AUDIO_TRACK,
  TEXT_TRACK
} AEEMediaMPEG4TrackType;

/*!
  @brief MPEG4FileFormatType Enumeration
*/ 
typedef enum AEEMediaMPEG4FileFormatType
{
  MM_MPEG4_UNKNOWN_FORMAT_TYPE=0,
  MM_MPEG4_STD_FORMAT_TYPE,
  MM_MPEG4_UUID_FORMAT_TYPE,
  MM_PVX_FORMAT_TYPE,
  MM_DIVX_DRM_FORMAT_TYPE
} AEEMediaMPEG4FileFormatType;

/*!
  @brief StreamType Enumeration
*/ 
typedef enum AEEStreamType
{
  MM_MEDIA_NON_STREAMING = 0,        //!< local clip
  MM_MEDIA_STREAMING,                //!< VOD streaming
  MM_MEDIA_LIVE_STREAMING,           //!< Live streaming
  MM_MEDIA_PV_SERVER_SIDE_PLAYLIST,  //!< Streaming PV server-side playlist
  MM_MEDIA_WM_SERVER_SIDE_PLAYLIST,  //!< Streaming WM server-side playlist
  MM_MEDIA_STREAMING_BCAST_ISDBT,    //!< BCAST_ISDBT streaming
  MM_MEDIA_BUFFERED_LIVE_STREAMING   //!< Live stream via Time-Shift Buffer (DVR)
} AEEStreamType;

/*!
  @brief MPEG4ScalingType Enumeration
*/ 
typedef enum AEEMediaMPEG4ScalingType
{
  MM_MPEG4_2P5X_ZOOM=3,        //!< 2.5X scaling
  MM_MPEG4_2X_ZOOM=2,          //!< 2X scaling
  MM_MPEG4_1P25X_ZOOM=1,       //!< 1.25X scaling
  MM_MPEG4_NO_SCALING=0,       //!< 1X scaling
  MM_MPEG4_0P25X_SHRINK=(-1),  //!< 0.25X scaling
  MM_MPEG4_2X_SHRINK=(-2),     //!< 0.5X scaling
  MM_MPEG4_0P75X_SHRINK=(-3),  //!< 0.75X scaling
  MM_MPEG4_ASCALE=(-4)
} AEEMediaMPEG4ScalingType;

typedef enum AEEMediaMPEG4Type
{
   MM_MPEG4_AUDIO_ONLY=0
  ,MM_MPEG4_VIDEO_ONLY
  ,MM_MPEG4_AUDIO_VIDEO
  ,MM_MPEG4_VIDEO_STILL_IMAGE
  ,MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE
  ,MM_MPEG4_VIDEO_TEXT
  ,MM_MPEG4_AUDIO_TEXT
  ,MM_MPEG4_AUDIO_VIDEO_TEXT
  ,MM_MPEG4_VIDEO_STILL_IMAGE_TEXT
  ,MM_MPEG4_AUDIO_VIDEO_STILL_IMAGE_TEXT
  ,MM_MPEG4_TEXT
  ,MM_MPEG4_INVALID
} AEEMediaMPEG4Type;

typedef enum AEEMediaMP4CodecType
{
   MM_MPEG4_EVRC_CODEC=0
  ,MM_MPEG4_QCELP_CODEC
  ,MM_MPEG4_AAC_CODEC
  ,MM_MPEG4_GSM_AMR_CODEC
  ,MM_MPEG4_MPEG4_CODEC
  ,MM_MPEG4_H263_CODEC
  ,MM_MPEG4_DIVX311_CODEC 
  ,MM_MPEG4_STILL_IMAGE_CODEC
  ,MM_MPEG4_TIMED_TEXT_CODEC
  ,MM_MPEG4_MP3_CODEC
  ,MM_MPEG4_H264_CODEC
  ,MM_MPEG4_WMV1_CODEC
  ,MM_MPEG4_WMV2_CODEC
  ,MM_MPEG4_WMV3_CODEC
  ,MM_MPEG4_WMA_CODEC
  ,MM_MPEG4_AMR_WB_CODEC
  ,MM_MPEG4_AMR_WB_PLUS_CODEC
  ,MM_MPEG4_WMA_PRO_CODEC
  ,MM_MPEG4_WMA_PRO_PLUS_CODEC
  ,MM_MPEG4_UNKNOWN_CODEC 
  ,MM_MPEG4_COOK_CODEC
  ,MM_MPEG4_SIPR_CODEC
  ,MM_MPEG4_RV30_CODEC
  ,MM_MPEG4_RV40_CODEC
  ,MM_MPEG4_EVRC_NB_CODEC
  ,MM_MPEG4_EVRC_WB_CODEC
  ,MM_AVI_AC3_CODEC
  ,MM_PCM_CODEC
} AEEMediaMP4CodecType;


typedef enum AEEQtvTrackType
{
    /* Track Types */
   TRACK_UNKNOWN          /* Unknown track type  */
  ,TRACK_AUDIO_EVRC       /* EVRC audio          */
  ,TRACK_AUDIO_QCELP      /* QCELP audio         */
  ,TRACK_AUDIO_AMR        /* AMR audio           */
  ,TRACK_AUDIO_MP4A_LATM  /* AAC audio           */
  ,TRACK_VIDEO_MP4V_ES    /* MPEG4 video         */
  ,TRACK_VIDEO_H263_2000  /* H.263 video         */
  ,TRACK_VIDEO_H264       /* H.264 video         */
  ,TRACK_3GPP_TIMED_TEXT  /* 3GPP Timed Text     */
  ,TRACK_GENERIC_TEXT
  ,TRACK_WM_AUDIO         /* Windows Media Audio */
  ,TRACK_WM1_VIDEO        /* Windows Media Video 7 */
  ,TRACK_WM2_VIDEO        /* Windows Media Video 8 */
  ,TRACK_WM3_VIDEO        /* Windows Media Video 9 */
  ,TRACK_VIDEO_OSCAR      /* OSCAR video         */
    /* Real Media Supported Track Types */
  ,TRACK_REAL_AUDIO_G2_MONO
  ,TRACK_REAL_AUDIO_G2_DUAL_MONO
  ,TRACK_REAL_AUDIO_8_STEREO
  ,TRACK_REAL_VIDEO_8
  ,TRACK_REAL_VIDEO_9
  ,TRACK_AUDIO_AMR_WB     /* AMR-WB audio        */
  ,TRACK_AUDIO_AMR_WB_PLUS /* AMR-WB+ audio       */
  ,TRACK_REAL_VIDEO_10     
} AEEQtvTrackType;

  typedef enum AEEQtvEncryptionType
  {
    /* type of encryption */
    ENCRYPT_NONE = 0,
    ENCRYPT_OMA_DRM_V2,
    ENCRYPT_WM_DRM,
    ENCRYPT_UNSUPPORTED
  } AEEQtvEncryptionType;

   /* AAC channel information */
  typedef enum AEEChannelConfig
  {
    AUDIO_CHANNEL_UNKNOWN = 0, 
    AUDIO_CHANNEL_MONO,         /* Single channel */
    AUDIO_CHANNEL_DUAL,         /* Stereo */ 
    AUDIO_CHANNEL_TRIPLE,       /* 3 channels (UNSUPPORTED) */  
    AUDIO_CHANNEL_QUAD,         /* 4 channels (UNSUPPORTED) */ 
    AUDIO_CHANNEL_QUINTUPLE,    /* 5 channels (UNSUPPORTED */ 
    AUDIO_CHANNEL_SEXTUPLE,     /* 5+1 channels (UNSUPPORTED */ 
    AUDIO_CHANNEL_OCTUPLE,      /* 7+1 channels (UNSUPPORTED) */ 
    AUDIO_CHANNEL_DUAL_MONO,    /* Dual mono */ 
    AUDIO_CHANNEL_UNSUPPORTED
  } AEEChannelConfig;

  /* AAC codec information */
  typedef struct AEEQtvAACInfo
  {
    uint32             nSamplingFreq;   /* Sampling frequency */
    AEEChannelConfig  eChannelConfig;  /* Channel configuration */
  }AEEQtvAACInfo;

  /* Codec specific information */
  typedef union AEECodecInfo
  {
    AEEQtvAACInfo aacInfo;  /* AAC codec specific information */
  }AEECodecInfo;

typedef enum AEEMediaPvxPlaybackControl
{
     MM_NO_PLAYBACK = 0
    ,MM_PLAYBACK_AFTER_DOWNLOAD
    ,MM_PLAYBACK_ASAP
    ,MM_PLAYBACK_RESERVE
}AEEMediaPvxPlaybackControl;

typedef enum AEEMediaEndOfClipAction
{
     MM_NO_END_OF_CLIP_ACTION = 0
    ,MM_END_OF_CLIP_CLOSE_APP
    ,MM_END_OF_CLIP_PLAY_NEXT_URL
    ,MM_END_OF_CLIP_LAUNCH_URL
}AEEMediaEndOfClipAction;

typedef enum AEEMediaHTTPDownloadAction
{
    MM_MPEG4_APP_TO_HANDLE_CONTENT = 0
   ,MM_MPEG4_QTV_TO_HANDLE_CONTENT = 1
   ,MM_MPEG4_QTV_TO_STOP = 2
}AEEMediaHTTPDLAction;

typedef enum AEEMediaInstancePriority
{
   MM_QTV_PRIORITY_INVALID = 0
  ,MM_QTV_PRIORITY_1
  ,MM_QTV_PRIORITY_HIGHEST = MM_QTV_PRIORITY_1
  ,MM_QTV_PRIORITY_2
  ,MM_QTV_PRIORITY_3
  ,MM_QTV_PRIORITY_LOWEST  = MM_QTV_PRIORITY_3
  ,MM_QTV_PRIORITY_DEFAULT = MM_QTV_PRIORITY_LOWEST
  ,MM_QTV_PRIORITY_COUNT
}AEEMediaInstancePriority;

typedef enum {
  MM_QTV_PLAYBACK_SPEED_NORMAL = 0,
  MM_QTV_PLAYBACK_SPEED_1P3X   = 1
} AEEMediaPlaybackSpeedType;

typedef enum AEEMediaDualMonoOutputType
{
  MM_DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R = 0, /* main channel to left and right */
  MM_DUAL_MONO_OUTPUT_SUB_TO_L_AND_R,      /* sub channel to left and right */ 
  MM_DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R,  /* main channel to left, sub channel to right */
  MM_DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R   /* sub channel to left, main channel to right */
}AEEMediaDualMonoOutputType;

typedef struct AEEMediaMPEG4StdSpec
{
  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
  AECHAR              szRating[MM_MPEG4_MAX_RATING_BYTES];
  AECHAR              szDescription[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  AECHAR              szOwner[MM_MPEG4_MAX_OWNER_BYTES];
  AECHAR              szVersion[MM_MPEG4_MAX_VERSION_BYTES];
  AECHAR              szCreationDate[MM_MPEG4_MAX_CREATIONDATE_BYTES];
  AECHAR              szArtist[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szSTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szAlbum[MM_MPEG4_MAX_ALBUM_BYTES];
  AECHAR              szInfo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  AECHAR              szLaunchURL[MM_MPEG4_MAX_URN_BYTES];
  AEEMediaEndOfClipAction szEndofClipAction;
} AEEMediaMPEG4StdSpec;

typedef struct AEEMediaMPEG4UUIDSpec
{
  uint32              dwCopyProhibitionFlag;
  uint32              dwValidityEffectiveDate; 
  uint32              dwValidityPeriod; 
  uint32              dwNumberOfAllowedPlayBacks; 

  AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
  AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
  AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
  AECHAR              szMemo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  uint32              dwGenerationDLLVersion;

  uint32              dwEditFlags; 
  uint32              dwRecordingMode; 
  uint32              dwRecordingDate;

  AECHAR              szDeviceName[MM_MPEG4_MAX_DEVICE_NAME_BYTES]; 
  AECHAR              szModelName[MM_MPEG4_MAX_MODEL_NAME_BYTES]; 
  AECHAR              szEncoderInformation[MM_MPEG4_MAX_ENCODER_INFO_BYTES]; 
  AECHAR              szMuxInformation[MM_MPEG4_MAX_MUX_INFO_BYTES]; 

  uint16              wGPSByteOrder;
  uint32              dwGPSVersionID;
  uint32              dwGPSLatitudeRef;
  uint32              dwGPSLongitudeRef;
  uint32              dwGPSAltitudeRef;
  uint64              GPSLatitudeArray[MM_MPEG4_MAX_GPS_LATITUDE];
  uint64              GPSLongitudeArray[MM_MPEG4_MAX_GPS_LONGITUDE];
  uint64              dwGPSAltitude;
  uint64              GPSTimeArray[MM_MPEG4_MAX_GPS_TIME];
  uint64              ddwGPSExtensionMapScaleInfo;
  AECHAR              szGPSSurveyData[MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES];
  AECHAR              szGPSPositioningMethod[MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES];
  AECHAR              szGPSPositioningName[MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES];
  AECHAR              szGPSDate[MM_MPEG4_MAX_GPS_DATE_BYTES];

} AEEMediaMPEG4UUIDSpec;

typedef struct AEEMediaMPEG4PvxSpec
{
    AEEMediaMPEG4StdSpec       StdSpec;
    boolean                    bPurgeDowloadedFile;
    boolean                    bAllowUserPlayback;
    AEEMediaPvxPlaybackControl downloadPlaybackControl; 
} AEEMediaMPEG4PvxSpec;

typedef struct AEEMediaMPEG4DivXDRMSpec
{  
  uint16          nUseLimit;	  
  uint16          nUseCounter;
  uint8           nCgmsaSignal;
  uint8           nAcptbSignal;
	uint8           nDigitalProtectionSignal;
}AEEMediaMPEG4DivXDRMSpec;

typedef struct AEEMediaDataSetupTime
{
  uint32 dwStartupTime;   /*!< min playback time required for startup */
  uint32 dwBufferingTime; /*!< min playback time required for restart after buffering */
}AEEMediaDataSetupTime;

typedef union AEEMediaMPEG4SpecEx
{
  AEEMediaMPEG4StdSpec        StdSpec;
  AEEMediaMPEG4UUIDSpec       UUIDSpec;
  AEEMediaMPEG4PvxSpec        PvxSpec;
  AEEMediaMPEG4DivXDRMSpec    DivXDrmSpec;
}AEEMediaMPEG4SpecEx;

/*!
  @brief Clip information
*/
typedef struct AEEMediaMPEG4Spec
{
  uint32                      dwDuration;           //!< clip duration in ms
  uint32                      dwSize;               //!< file size
  AEEStreamType               streamtype;           //!< Only applicable to streaming clips
  AEEMediaMPEG4Type           mpeg4Type;            //!< MP4 content type
  AEEMediaMP4CodecType        audioType;            //!< MP4 Audio Codec type
  boolean                     bRepositioningAllowed;
  boolean                     bAudioOnlyPlaybackAllowed;
  boolean                     bVideoOnlyPlaybackAllowed;
  uint32                      dwHeight;           //!< height of the video frame
  uint32                      dwWidth;            //!< width of video frame

  AEEMediaMPEG4FileFormatType eFormatType;
  AEEMediaMPEG4SpecEx         ExtendedInfo;
} AEEMediaMPEG4Spec;

/*!
  @brief 3GPP timed text font info
*/
typedef struct AEETTFontInfo 
{
  uint16    nID;
  char      Name[MM_MPEG4_MAX_FONT_LEN];
} AEETTFontInfo;

typedef struct AEE3GPPTimedText
{
  AECHAR      * pszText;
  uint32        dwDuration;
  uint32        dwDisplayFlags;
  int32         dwHorzJustification;
  int32         dwVertJustification;
  int32         dwBoxTop;
  int32         dwBoxLeft;
  int32         dwBoxBottom;
  int32         dwBoxRight;
  uint32        dwStartChar;
  uint32        dwEndChar;
  uint32        dwFontID;
  uint32        dwFontStyleFlags;
  uint32        dwFontSize;
  uint8         BackgroundColorRGBA[MM_MPEG4_MAX_TEXT_COLORS];
  uint8         TextColorRGBA[MM_MPEG4_MAX_TEXT_COLORS];
  uint32        dwFontListSize;
  AEETTFontInfo *pFontList; /*!< list indexed from 0 to dwFontListSize-1 */
  uint32        dwSampleModifiersSize;
  uint8         *pSampleModifiersBuffer;

} AEE3GPPTimedText;

typedef struct AEE3GenericText
{
	uint8        *GenericTextBuffer;
	uint32        GenericTextSize;

} AEEGenericText;

/*!
  @brief RTSPStatus structure
*/ 
typedef struct AEERTSPStatus
{
  int32 statusCode;
  char  statusString[MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES];
} AEERTSPStatus;

/*!
  @brief Unhandled status string
*/ 
typedef struct AEEUnhandledSDP_RTSP_Status
{
  char  statusString[MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES];
} AEEUnhandledSDP_RTSP_Status;

/*! @brief QoS structure for choosing PDP context */
typedef struct AEENetPolicyInfo
{
  uint32  size;                /*!< Size of this total structure in bytes */
  boolean isSet;               /*!< TRUE, if the information in this structure is valid */
  boolean isQoSEnabled;        /*!< TRUE, if QoS is enabled */
  uint8   primaryPDPProfileNo; /*!< PDP profile Number */
}AEENetPolicyInfo;

/*! @brief Stream recording parameters */
typedef struct AEEStreamRecordInfo
{
  char * filename;   /*!< File name along with path */
  boolean overwrite; /*!< TRUE, if it is OK to overwrite an existing file */
  uint8 mode;        /*!< Bit mask to select tracks to be recoreded.  May be
                      * ingored by QtvPlayer.  Refer to QtvPlayer API for more
                      * details. */
  uint32 duration;   /*!< 0, If no limit is enforced.  May be ingored by
                      * QtvPlayer.  Refer to QtvPlayer API for more details. */
} AEEStreamRecordInfo;


typedef struct AEETrackList {
  uint32              nTrackID;
  AEEQtvTrackType     TrackType;
  AEEMediaMPEG4Type   MediaType;
  uint32              bmTrackState;
  AEECodecInfo        codecInfo;    /* codec information */
  AEEQtvEncryptionType     encryptionType;
  char                language[MM_MPEG4_QTV_MAX_LANGUAGE_BYTES];
} AEETrackList;

typedef struct AEETrackID
{
  uint32 nAudioTrackID;
  uint32 nVideoTrackID; 
  uint32 nTextTrackID;
} AEETrackID;

/*---------------------------------------------------------------------------
  Generic interface to SetMediaInfo
---------------------------------------------------------------------------*/

/* QTV Media info types, including network */
typedef enum {
  AEEM_MEDIA_BCAST_FLO = 0,
  AEEM_MEDIA_NUM       = 1
} AEEMMpeg4MediaType;


/* QTV Media info struct */
typedef struct {
  AEEMMpeg4MediaType type; /*!< Identifies the contents of of info below */
  uint32             size; /*!< # of bytes pointed to by info, for versioning */
  byte               info[1]; /*!< Var length media info, identified by type */
} AEEMMpeg4MediaInfo;

/*---------------------------------------------------------------------------
  Generic interface to GetExtFrameInfo
---------------------------------------------------------------------------*/
typedef enum AEEExtFrmInfoType
{
  FLO_OSCAR=0      /*!< The extended frame info type that identifies the sender 
                      in AEEMediaExtFrameInfoHeader structure */
} AEEExtFrmInfoType;

/*---------------------------------------------------------------------------
  Extended frame info structure that is filled up by the decoder and passed
  all the way up for consumption by the UI. Note that this structure is 
  malloc'ed and freed by the decoder. All that the UI has to do is to read 
  this data along with the frame info it's already reading. Finally when the 
  frame buffer is released, the extended frame buffer is released as well.
---------------------------------------------------------------------------*/
typedef struct 
{
  /*! Size of this structure. This is needed in order to enable the end user to
   * allocate/copy this data into it's own memory. This size is set by the
   * originator of the data and is equal to the total size of data sent, which
   * includes the header (AEEMediaExtFrameInfoHeader) and any additional codec
   * specific information.
   */
  uint32 size;

  /*! Identifies the type of system/codec that we are dealing with. Currently it
   * is always BCAST/OSCAR. However we could make this a generic frame info
   * mechanism. Then each system/codec can set it's type
   * accordingly
   */
  AEEExtFrmInfoType type;

} AEEMediaExtFrameInfoHeader;


typedef struct AEETelopTextSubString
{
  boolean     bLineFeed;
  boolean     bUnderline;
  boolean     bReversal;
  boolean     bLinking;
  int32       dwFontColor;
  int32       dwTextSampleSize;  //!< in bytes
  int32       dwLinkSize;
  char        *pTextSubString;
  char        *pLinkValue;
} AEETelopTextSubString;

typedef struct AEETelopTextElement
{
  int32       dwTelopSize;
  int32       dwBeginTime;
  int32       dwDuration;
  int32       dwNumberOfSubStrings;
  boolean     bWrapFlag;
  int32       dwBackgroundColor;
  int32       dwDefaultFontColor;
} AEETelopTextElement;


typedef struct AEEMediaDataBufferChunk
{
  void* *pData; /*!< Pointer to the beginning of a buffer chunk */
  uint32 dwSize; /*!< Size of this chunk in bytes */
}AEEMediaDataBufferChunk;


typedef struct AEEMediaPlaybackPos
{
   uint32      dwVideoPos;    //!< video position in ms
   uint32      dwAudioPos;    //!< audio position in ms
   uint32      dwTextPos;     //!< text position in ms
   uint32      dwPlaybackPos; //!< max of the three positions in ms
} AEEMediaPlaybackPos;

/* The following are not part of BREW.They are for reference use only.
*/

/*
Structure for QoS media. This is not part of BREW and should not be used anywhere
in a BREW app.
*/
typedef struct AEEMediaDataWithQoS
{
   AEECLSID    clsData;    //!< Type of data source/sink
   void *      pData;      //!< Context sensitive data (see table above)
   uint32      dwSize;     //!< Context sensitive data (see table above)
   AEENetPolicyInfo*  qosInfo; //!< Pointer to Net Policy information 

} AEEMediaDataWithQoS; 


//AEEFetchBufferedDataSizeT Function Prototype Callback Function
// pUserData, user's data
// nDataSize, data size
// pbEndOfData,  this should be TRUE if entire data available in the buffer
typedef void (*AEEFetchBufferedDataSizeT)(void* pUserData, uint32* nDataSize, boolean* pbEndOfData);

//AEEFetchBufferedDataT Function Prototype Callback Function
// pUserData, user's data
// dataBuf, to be filled with data
// readSize, size of the buffer
// readPos, read pos
typedef uint32 (*AEEFetchBufferedDataT)(void* pUserData, void* pDataBuf, uint32 nReadSize, uint32 nReadPos);


/* Reference function for implementing media with QoS settings */
int AEEMediaUtil_CreateQoSMedia(IShell * ps,
                                AEEMediaDataWithQoS * pmd,
                                IMedia ** ppm);


int AEEMediaUtil_CreateMediaEx( IShell * ps,
                                AEEMediaDataEx * pmdList,
                                int nListCount,
                                IMedia ** ppm );

int AEEMediaUtil_CreatePseudoMediaEx(IShell * ps, AEEMediaDataEx * pmd, IMedia ** ppm);   

/*! @brief DRM decryption function type */
typedef boolean (*AEEMediaDRMDecryptMethodT)
(
  void      *pEncryptedDataBuf,   /*!< pointer to encrypted data buffer, which has to be decrypted */
  void      *pDecryptedDataBuf,   /*!< pointer to destination buffer to copy decrypted data,
                                   *   OEM is resposible for copying the decrypted data  */
  uint32    wEncryptedDataSize,   /*!< encrypted data buffer size */
  uint32    *pDecryptedDataSize,  /*!< pointer to decrypted data buffer size */
  void      *pClientData          /*!< pointer to client data provided by OEM when registering callback */
);

/*=========================================================================================
  DATA STRUCTURE DOCUMENTATION
===========================================================================================
AEEMediaMPEG4StdSpec  
  
Description:
This structure contains information for the standard specifications for a MPEG4 clip.

Definition:
  typedef struct AEEMediaMPEG4StdSpec
  {
    AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
    AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
    AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
    AECHAR              szRating[MM_MPEG4_MAX_RATING_BYTES];
    AECHAR              szDescription[MM_MPEG4_MAX_DESCRIPTION_BYTES];
    AECHAR              szOwner[MM_MPEG4_MAX_OWNER_BYTES];
    AECHAR              szVersion[MM_MPEG4_MAX_VERSION_BYTES];
    AECHAR              szCreationDate[MM_MPEG4_MAX_CREATIONDATE_BYTES];
    AECHAR              szArtist[MM_MPEG4_MAX_AUTHOR_BYTES];
    AECHAR              szSTitle[MM_MPEG4_MAX_TITLE_BYTES];
    AECHAR              szAlbum[MM_MPEG4_MAX_ALBUM_BYTES];
    AECHAR              szInfo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
  } AEEMediaMPEG4StdSpec;

Members:
  szTitle, szAuthor, szCopyright, szRating, szDescription, szOwner, szVersion, szCreationDate,
  szArtist, szSTitle, szAlbum, szInfo are all char arrays contain strings representing given
  infomation for a clip about the title, author, copyright, etc.

Comments:
  Every MPEG4 must have these fields in it.  Even though they exist some of them may still be
  NULL.

See Also:
  AEEMediaMPEG4SpecEx
  AEEMediaMPEG4UUIDSpec

=============================================================================================
AEEMediaMPEG4DivXDRMSpec

Description:
Following structure defines DivX Clip specific DRM information.

Definition:
typedef struct AEEMediaMPEG4DivXDRMSpec
{  
  uint16          nUseLimit;	  
  uint16          nUseCounter;
  uint8           nCgmsaSignal;
  uint8           nAcptbSignal;
  uint8           nDigitalProtectionSignal;
}AEEMediaMPEG4DivXDRMSpec;

Members:  
  nUseLimit	               - The counter to specify maximum number of times this clip can be played.
  nUseCounter              - Current playback count for the given clip.
                             Clip can be played only 'nUseLimit' times. User has to renew the count once it expires.
  nCgmsaSignal             - External signal protection. Copy protection mechanism for analog television signals.
                         
  nAcptbSignal             - External signal protection. If set and if device does not have MacroVision,
                             TvOut should be disabled.
  nDigitalProtectionSignal - External signal protection for digital output. When set,
                             content can only be output over secure digital outputs approved by DivX or
                             analog video outputs that are constrained to maximum 480p resolution.

Comments:
  Once the DIVX DRM is enabled, clip specific information will be available after QTV reports PB_READY.
=============================================================================================
AEEMediaMPEG4UUIDSpec

Description:
This structure is an extension to the basic AEEMediaMPEG4StdSpec that contains extra information
given for all clips having UUID atoms including GPS information.

Definition:
  typedef struct AEEMediaMPEG4UUDISpec
  {
    uint32              dwCopyProhibitionFlag;
    uint32              dwValidityEffectiveDate; 
    uint32              dwValidityPeriod; 
    uint32              dwNumberOfAllowedPlayBacks; 
  
    AECHAR              szTitle[MM_MPEG4_MAX_TITLE_BYTES];
    AECHAR              szCopyright[MM_MPEG4_MAX_COPYRIGHT_BYTES];
    AECHAR              szAuthor[MM_MPEG4_MAX_AUTHOR_BYTES];
    AECHAR              szMemo[MM_MPEG4_MAX_DESCRIPTION_BYTES];
    uint32              dwGenerationDLLVersion;
  
    uint32              dwEditFlags; 
    uint32              dwRecordingMode; 
    uint32              dwRecordingDate;

    AECHAR              szDeviceName[MM_MPEG4_MAX_DEVICE_NAME_BYTES]; 
    AECHAR              szModelName[MM_MPEG4_MAX_MODEL_NAME_BYTES]; 
    AECHAR              szEncoderInformation[MM_MPEG4_MAX_ENCODER_INFO_BYTES]; 
    AECHAR              szMuxInformation[MM_MPEG4_MAX_MUX_INFO_BYTES]; 

    uint16              wGPSByteOrder;
    uint32              dwGPSVersionID;
    uint32              dwGPSLatitudeRef;
    uint32              dwGPSLongitudeRef;
    uint32              dwGPSAltitudeRef;
    uint64              GPSLatitudeArray[MM_MPEG4_MAX_GPS_LATITUDE];
    uint64              GPSLongitudeArray[MM_MPEG4_MAX_GPS_LONGITUDE];
    uint64              dwGPSAltitude;
    uint64              GPSTimeArray[MM_MPEG4_MAX_GPS_TIME];
    uint64              ddwGPSExtensionMapScaleInfo;
    AECHAR              szGPSSurveyData[MM_MPEG4_MAX_GPS_SURVEY_DATA_BYTES];
    AECHAR              szGPSPositioningMethod[MM_MPEG4_MAX_GPS_POSITIONING_METHOD_BYTES];
    AECHAR              szGPSPositioningName[MM_MPEG4_MAX_GPS_POSITIONING_NAME_BYTES];
    AECHAR              szGPSDate[MM_MPEG4_MAX_GPS_DATE_BYTES];
  } AEEMediaMPEG4UUIDSpec;

Members:
  dwCopyProhibitionFlag:      Any bit in this field is set if clip is copy protected
  dwValidityEffectiveDate:    Validity Start date from CopyGaurdAtom
  dwValidityPeriod:        Validity period from the Start Date from CopyGuardAtom
  dwNumberOfAllowedPlayBacks:    Number of times clip is allowed to be played back
  szTitle:            Char array with title of clip
  szCopyright:          Char array with copyright string
  szAuthor:            Char array with author string
  szMemo:              Char array with any extra notes or info
  dwGenerationDLLVersion:      Version of the DLL used
  dwEditFlags:          Edit flags from Movie Mail Atom
  dwRecordingMode:        Recording mode of Movie Mail Atom
  dwRecordingDate:        Recording date for Movie Mail Atom
  szDeviceName:          Char array with encoding device name from Encoder Information Atom
  szModelName:          Char array with encoder model name from Encoder Information Atom
  szEncoderInformation:      Char array with encoder information from Encoder Information Atom
  szMuxInformation:        Char array with encoding multiplexer information
  wGPSByteOrder:          GPS byte order from GPS Atom
  dwGPSVersionID:          Version ID from GPS Atom
  dwGPSLatitudeRef:        GPS Latitude reference from GPS Atom
  dwGPSLongitudeRef:        GPS Longitude reference from GPS Atom
  dwGPSAltitudeRef:        GPS Altitude reference from GPS Atom
  GPSLatitudeArray:        64-bit array for GPS latitude from GPS Atom
  GPSLongitudeArray:        64-bit array for GPS longitude from GPS Atom
  dwGPSAltitude:          64-bit GPS Altitude from GPS Atom
  GPSTimeArray:          64-bit time array from GPS Atom
  ddwGPSExtensionMapScaleInfo:  Extension Map Scale information from GPS Atom
  szGPSSurveyData:        Char array with GPS Survey data from GPS Atom
  szGPSPositioningMethod:      Char array with GPS positioning method from GPS Atom
  szGPSPositioningName:      Char array with GPS positioning name from GPS Atom
  szGPSDate:            Char array with GPS date from GPS Atom

Comments:

See Also:
  AEEMediaMPEG4SpecEx

=============================================================================================
AEEMediaMPEG4SpecEx

Description:
This is the extended spec which contains the standard and UUID spec's described above.

Definition:
  typedef union AEEMediaMPEG4SpecEx
  {
    AEEMediaMPEG4StdSpec        StdSpec;
    AEEMediaMPEG4UUIDSpec       UUIDSpec;
  } AEEMediaMPEG4SpecEx;

Members:
  StdSpec:    Standard specification, see above
  UUIDSpec:    UUID user atom specifications, see above
  
Comments:

See Also:

=============================================================================================
AEEMediaMPEG4Spec

Description:
This is the information that pertains only to the video and auto information for a clip.

Definition:
  typedef struct AEEMediaMPEG4Spec
  {
    uint32                      dwDuration;           
    uint32                      dwSize;             
    AEEStreamType               streamtype;       
    AEEMediaMPEG4Type           mpeg4Type;      
    AEEMediaMP4CodecType        audioType;
    boolean                     bRepositioningAllowed;
    boolean                     bAudioOnlyPlaybackAllowed;
    boolean                     bVideoOnlyPlaybackAllowed;
    uint32                      dwHeight;
    uint32                      dwWidth;
    AEEMediaMPEG4FileFormatType eFormatType;
    AEEMediaMPEG4SpecEx         ExtendedInfo;
  } AEEMediaMPEG4Spec;

Members:
  dwDuration:            Duration of the clip in ms
  dwSize:              Size of the file in bytes
  streamtype:            Stream type only used for streaming clips
  mpeg4Type:            MP4 content type
  audioType:            MP4 Audio codec type
  bRepositioningAllowed:      True if reposition in clip is allowed
  bAudioOnlyPlaybackAllowed:    True if only audio and no video
  bVideoOnlyPlaybackAllowed:    True if only video and no audio
  dwHeight:            Height of video frame
  dwWidth:            Width of video frame
  eFormatType:          UNKNOWN, STD, or UUID format type
  ExtendedInfo:          Extra STD and UUID info structure, see above

Comments:
  This is the basic video and audio information that is needed, if other details are needed
  dig into the STD or UUID structures
  
See Also:
  AEEMediaMPEG4SpecEx

=============================================================================================
AEETTFontInfo 

Description:
This is the font information structure that stores the name of the font and associated ID 
used for storing font information in an array

Definition:
  typedef struct AEETTFontInfo 
  {
    uint16    nID;
    char      Name[MM_MPEG4_MAX_FONT_LEN];
  } AEETTFontInfo;

Members:
  nID:    ID of the font for the given name
  Name:    A char array representing the name of the font

Comments:

See Also:
  AEE3GPPTimedText

=============================================================================================
AEE3GPPTimedText

Description:
This structure is used to hold all the information need for displaying and keeping track of a
timed text area.

Definition:
  typedef struct AEE3GPPTimedText
  {
    AECHAR      * pszText;
    uint32        dwDuration;
    uint32        dwDisplayFlags;
    int32         dwHorzJustification;
    int32         dwVertJustification;
    int32         dwBoxTop;
    int32         dwBoxLeft;
    int32         dwBoxBottom;
    int32         dwBoxRight;
    uint32        dwStartChar;
    uint32        dwEndChar;
    uint32        dwFontID;
    uint32        dwFontStyleFlags;
    uint32        dwFontSize;
    uint8         BackgroundColorRGBA[MM_MPEG4_MAX_TEXT_COLORS];
    uint8         TextColorRGBA[MM_MPEG4_MAX_TEXT_COLORS];
    uint32        dwFontListSize;
    AEETTFontInfo *pFontList;      list indexed from 0 to dwFontListSize-1
    uint32        dwSampleModifiersSize;
    uint8         *pSampleModifiersBuffer;
  } AEE3GPPTimedText;

Members:
  pszText:          Pointer to an text string that is displayed
  dwDuration:          The duration which the text is displayed for in ms
  dwDisplayFlags:        Flags to modify how text displayed
  dwHorzJustification:    Horizontal justification
  dwVertJustification:    Vertical Justification
  dwBoxTop:          Top edge of text box
  dwBoxLeft:          Left edge of text box
    dwBoxBottom:        Bottom edge of text box
  dwBoxRight:          Right edge of text box
  dwStartChar:        First character index from szText
  dwEndChar:          Last character index from szText  
  dwFontID:          ID of the font to use for text
  dwFontStyleFlags:      Style of font, bold, italics, etc
  dwFontSize:          Size of the font to use
  BackgroundColorRGBA:    Array containing the background color information
  TextColorRGBA:        Array containing the text color information
  dwFontListSize:        Number of elements in the font list
  pFontList:          A pointer to an array of font elements
  dwSampleModifiersSize:    Size of the sample modifiers buffer
  pSampleModifiersBuffer:    Pointer to the sample modifiers buffer data

Comments:

See Also:
  AEETTFontInfo 

=============================================================================================
AEERTSPStatus

Description:
Status structure contains a code and a string

Definition:
  typedef struct AEERTSPStatus
  {
    int32 statusCode;
    char  statusString[MM_MPEG4_MAX_RTSP_STATUS_STRING_BYTES];
  } AEERTSPStatus;
  
Members:
  statusCode:    Code number of the status
  statusString:  String describing the status

Comments:

See Also:

=============================================================================================
AEEUnhandledSDP_RTSP_Status

Description:
Unhandled status has no code, only a string.

Definition:
  typedef struct AEEUnhandledSDP_RTSP_Status
  {
     char  statusString[MM_MPEG4_MAX_UNHANDLED_SDP_RTSP_STRING_BYTES];
  } AEEUnhandledSDP_RTSP_Status;
  
Members:
  statusString:  String describing the unhandled status
  
Comments:

See Also:

=============================================================================================
AEEMediaDRMDecryptMethodT

Description:
DRM decryption callback function.

Definition:
typedef boolean (*AEEMediaDRMDecryptMethodT)
(
    void      *pEncryptedDataBuf,   
    void      *pDecryptedDataBuf,                                       
    uint32    wEncryptedDataSize,  
    uint32    *pDecryptedDataSize,                                   
    void      *pClientData          
);
  
Members:
    pEncryptedDataBuf: pointer to encrypted data buffer which has to be decrypted 
    pDecryptedDataBuf: pointer to destination buffer to copy decrypted data
    wEncryptedDataSize: encrypted data buffer size
    pDecryptedDataSize: pointer to decrypted data buffer size
    pClientData: pointer to client data provided by OEM when registering callback

Comments:
This function is registered by setting MM_MP4_PARM_DRM_DECRYPTION.
Note the following when this function is called by QTV:
(1) Memory is allocated and owned by QTV. User (OEM) need NOT allocate or free any memory
(2) Entire encrypted data,in pEncryptedDataBuf, must be decrypted into pDecryptedDataBuf.
    Fragmented decryption is not allowed
(3) QTV uses the decrypted data as soon as the function returns
(4) Decryption latency affects playback performance

See Also:
MM_MP4_PARM_DRM_DECRYPTION
IMEDIA_SetMediaParm()

=============================================================================================
AEEFetchBufferedDataSizeT

Description:
PULL mode replay prototype callback function for retrieving data size

Definition:
typedef void (*AEEFetchBufferedDataSizeT)(void* pUserData, uint32* nDataSize, boolean* pbEndOfData);

 
Members:
    pUserData: pointer to user's private data
    nDataSize: data size
    pbEndOfData: set to true if nDataSize indicates file size

Comments:
This function is registered by setting MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE.

See Also:
MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE
IMEDIA_SetMediaParm()

=============================================================================================
AEEFetchBufferedDataT

Description:
PULL mode replay prototype callback function for retrieving data

Definition:
typedef uint32 (*AEEFetchBufferedDataT)(void* pUserData, void* pDataBuf, uint32 nReadSize, uint32 nReadPos);

 
Members:
    pUserData: pointer to user's private data
    pDataBuf: point to data buffer
    nReadSize: requested read size
    nReadPos: starting position

Comments:
This function is registered by setting MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA.

See Also:
MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA
IMEDIA_SetMediaParm()

=============================================================================================

MP4 Control Parameters (MM_MP4_PARM_XXX)

Description:
These parameters allow setting/getting of MP4 media parameters.  Use IMEDIA_GetMediaData and 
IMEDIA_SetMediaData to get and set media parameters.

MM_MP4_PARM_ROTATION
Set:  Sets the rotational value of current clip
      p1 = AEEMediaData *
      p2 = int32  (rotational value)
Get:  Gets the current media data's rotation value.
      p1 = AEEMediaData *
      p2 = int32  * (rotational value)

MM_MP4_PARM_SCALING
Set:  Sets the scaling value of current clip
      p1 = AEEMediaData *
      p2 = int32  (scaling value)
Get:  Gets the current media data's scaling value.
      p1 = AEEMediaData *
      p2 = int32  * (scaling value)

MM_MP4_PARM_TELOP_SUB_STR
Get:  Gets the current media data's telop sub string.
      p1 = AEEMediaData *
      p2 = int32  * (pointer to substring)
    p3 = telop substring index

MM_MP4_PARM_RTSP_STREAM_CONVERT
Set:  Start RTSP stream recording
      p1 = pointer to AEEStreamRecordInfo structure. 

MM_MP4_PARM_RTSP_STOP_STR_CONVERT
Set:  Stop RTSP stream recording

MM_MP4_PARM_NET_QOS_POLICY
Set:  Move Mpeg4 IMEDIA object to "Ready" state
      p1 = pointer to AEENetPolicyInfo

MM_MP4_PARM_DRM_DECRYPTION  
Set:  Registers the DRM decryption callback function. QTV will call this function when 
      it needs to decrypt the data during playback as and when needed. 
      This is synchronous function call. QTV passes ENCRYPTED data and expect DECRYPTED data 
      back in buffer when function returns.
      p1 = pointer to OEM layer DRM decryption callback function
      p2 = pointer to client data provided by OEM when registering callback

MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE
Set:  Register callback function for PULL mode replay to retrieve data size
      p1 = point to callback function
      p2 = point to client data

MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA
Set:  Register callback function for PULL mode replay to retrieve data
      p1 = point to callback function
      p2 = point to client data

MM_MP4_PARM_BUFFER_UPDATE       - Indicate Buffering
MM_MP4_PARM_CONNECT_UPDATE      - Indicate Connection Setup to user
MM_MP4_PARM_PB_READY            - Indicate PB Ready
MM_MP4_PARM_MEDIA_SPEC          - Retrieves the MediaSpec
MM_MP4_STREAM_AUTHENTICATE      - Authenticate the stream when enabled 
MM_MP4_AUDIO_VIDEO_MEDIA_DATA   - all audio video media data
MM_MP4_PARM_TRACK_TIME_SCALE    - Track time scale amount
MM_MP4_PARAM_AUDIO_OVERRIDE     - Overide audio parameters
MM_MP4_PARM_RTSP_STATUS_NOTIFY  - RTSP status notify event
MM_MP4_PARM_LOOP_TRACK          - p1 has value true/false
MM_MP4_PARAM_PLAYBACK_POS       - p1 = *playback time structure
MM_MP4_PARAM_PSBUF_WRITE_OFFSET - Set/Get PSBUF offset for writing
MM_MP4_PARM_PS_BUFFER_UPDATE    - Update the PS buffer
MM_MP4_PARM_DECRYPT_KEY         - p1= pointer to AEEMFDecryptKey struct, p2 = size
MM_MP4_SCALING_UPDATE           - Indicate to update after scaling
MM_MP4_PARM_RTSP_STREAM_RECORD  - enable stream recording
MM_MP4_PARM_MIDI_SIZE           - get midi data size, p1(output) = * uint32
MM_MP4_PARM_MIDI_DATA           - get midi data, p1 = buf pointer, p2(input/output) = * uint32 size
MM_MP4_PARM_LINK_SIZE           - get link data size, p1(output) = * uint32
MM_MP4_PARM_LINK_DATA           - get link data, p1 = buf pointer, p2(input/output) = * uint32 size
MM_MP4_PARAM_SET_DATA_PORT_RANGE - set QTV UDP data port range, p1=begin port, p2=end port
MM_MP4_PARM_VIDEO_CODEC          - get Video Codec Type, p1(output) = (AEEMediaMP4CodecType *), valid after MediaSpec is available
MM_MP4_PARM_AUDIO_CODEC          - get Audio Codec Type, P1(output) = (AEEMediaMP4CodecType *), valid after MediaSpec is available
MM_MP4_PARM_VIDEO_FRAME_RATE     - get Video Frame Rate, p1(output) = (* float), valid after MediaSpec is available
MM_MP4_PARM_VIDEO_FRAME_RATE_EX  - get Video Frame Rate, p1(output) = (* double), valid after MediaSpec is available
MM_MP4_PARM_VIDEO_AVERAGE_BIT_RATE - get Video Bit Rate, p1(output) = (* unsigned long), valid after MediaSpec is available
MM_MP4_PARM_AUDIO_AVERAGE_BIT_RATE - get Audio Bit Rate, p1(output) = (* unsigned long), valid after MediaSpec is available
MM_MP4_PARM_DRM_REG_CODE           - retrieve registration code,   p1(input/output)= (*char),p2(*int) = Specifying size allocated for p1.
MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA_SIZE - registers DRM callback for OEM to retrieve data size 
MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA - registers DRM callback for OEM to decrypt data 
MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_SUPPORTED_TYPE - registers callback to determine if MIME is supported or not
      

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

IMEDIA_GetMediaSpec()

Description:
   
   This macro is used to get a pointer to a specific media specification.

Prototype:
   int IMEDIA_GetMediaSpec(IMedia * p, int32 * ppSpec, int32 * pSize);
  
Parameters:
     p [in]:        Pointer to the IMedia Interface object
     ppSpec [in]:   Pointer to an integer to store spec info
     pSize [in]:    Pointer to an integer to store the size in

Return Value:

   SUCCESS:    Successful.
   EBADSTATE:  Error - IMedia is not in Ready state.

Comments: 
   None.

Side Effects: 
   None

See Also:

======================================================================
*/
#endif // AEEMEDIAMPEG4_H

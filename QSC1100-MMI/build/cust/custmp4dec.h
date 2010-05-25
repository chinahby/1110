#ifndef CUSTMP4_H
#define CUSTMP4_H
/*===========================================================================

            " M S M 6 0 5 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target using MSM6050 ARM Processor.
  This is a Trimode build with IS2000.
  This particular build supports EFS, MPEG4 and AAC

  Copyright (c) 2001,2002   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc60x0/build/cust/6020/custmp4dec.h#1 $ $DateTime: 2006/07/26 19:06:20 $ $Author: c_bmorra $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/03   enj     Removing dead features
08/29/03   sjy     Defined feature for temporal scalability, removed
                   FEATURE_MP4_TS_DSP_INTERFACE
07/25/02    ny     Moved the definition of FEATURE_AAC from custcmx.h to custmp4.h
07/14/02    ny     Initial creation
===========================================================================*/

/* Enable MPEG4 decoder
*/
#define FEATURE_MP4_DECODER

/* Enable usage of the whole 4M of flash
*/
#define FEATURE_EFS_MPEG4_FLASH

/* BREWapi MPEG4 feature definitions
*/
#define   FEATURE_APP_MPEG4

/* BREWapi Multiple File Playback feature definition
*/
#define   FEATURE_APP_MEDIA_MULTIPLE_FILE

/*  Enable 1/8 VGA Support 
 */
#define FEATURE_MP4_ONEEIGHTH_VGA

/* 1/4 VGA (QVGA) Support.  Disabled by default.  May be enabled by OEMs.
*/
//#define FEATURE_MP4_ONEQUARTER_VGA

/* Enable 240x192 support
 */
#define FEATURE_MP4_240X192_PIXELS
 
/* Enable AAC frame markers
 */
#define FEATURE_MP4_AAC_FRAME_MARKER

/*
 * This is example code for use by OEM's.
 *
 * Enable the following feature to have the MediaPlayer request MM_STATUS_FRAME
 * callbacks from the IMEDIA layer.  Once these are enabled, MediaPlayer is in
 * charge of displaying the video frames to the screen.  As such, it can modify
 * each frame before it is displayed.  (i.e. overlays, clipping, watermarking,
 * etc.)
 */
//#define FEATURE_APP_MPEG4_FRAME_CB

/* Enable zrex-rex translation to support the Packet Video implementation
*/
#define FEATURE_ZREX2REX

/* Enable Qtv memory debugging.  This feature turns on a QTVDIAG_DEBUG message
 * for every new/delete stating the file, line, number of bytes, and pointer.
 */
#define FEATURE_QTV_MEMORY_DEBUG

/* Enable port mapping traversal algorithm to allow streaming to external
 * RealServers outside of the network's firewall
 */
#define FEATURE_MP4_FIREWALL_PASS_THROUGH_RN

/* Force the MediaPlayer to use the MMC card for it's media content
 */
#ifndef T_FFA
# define FEATURE_MP4_USE_MMC
#endif

/* Enable workaround for PV non-spec-compliance.  This workaround is in the
 * MPEG4 Video Encoder Engine.
 */
#define FEATURE_VIDEO_ENG_QTV_INTEROP

/* Enable looping of independent a/v tracks
 */
#define FEATURE_QTV_LOOP_TRACK

/* Enable 3GPP timed text and KDDI User-Meta-Data support.
 */
#define FEATURE_MP4_3GPP_TIMED_TEXT

/* Enable KDDI User-Meta-Data support.
 */
#define FEATURE_MP4_KDDI_META_DATA

/* Enable KDDI Telop Timed Text Support
 */
#define FEATURE_MP4_KDDI_TELOP_TEXT

#if defined(FEATURE_MP4_KDDI_TELOP_TEXT) && !defined(FEATURE_MP4_3GPP_TIMED_TEXT)
#error FEATURE_MP4_KDDI_TELOP_TEXT needs FEATURE_MP4_3GPP_TIMED_TEXT
#endif

/* Enable single frame step (forward and backward) support.
 */
#define FEATURE_MP4_STEP_FRAME

/* Enable frame transformation (rotation and scaling) support.
 */
#define FEATURE_MP4_FRAME_TRANSFORMATIONS

/* Enable independently selecting audio and/or video track for playback.
 */
#define FEATURE_MP4_AUDIO_VIDEO_SELECTION

/* Enable MP4 AMR (GSM audio) support
 * Enables AMR audio playback when embedded within
 * an MPEG-4 video file.
 */
#define FEATURE_MP4_AMR

/* Enable MP4 MP3 (Mpeg 1/2, layer 3 audio) support
 * Enables MP3 audio playback when embedded within
 * an MPEG-4 video file.
 */
#define FEATURE_MP4_MP3

/* Enable the video encoder application.
*/
/* #define FEATURE_APP_QCAMCORDER */

/* Enable video encoding.
*/
/* #define FEATURE_VIDEO_ENCODE */

/* Enable the MPEG4 encoder.
*/
/* #define FEATURE_MP4_ENCODER */

/* Enable the QDSP image supporting the MPEG4 encoder with 13k QCELP.
*/
/* #define FEATURE_MP4_ENC_13K */

/* Enable the QDSP image supporting the MPEG4 encoder with EVRC
*/
/* #define FEATURE_MP4_ENC_EVRC */

/* Enables 3GPP2 AMR audio encoding within an MPEG-4 video file.
*/
/* #define FEATURE_MP4_ENC_AMR */

/* Enables RFC 3267 AMR file format support.
*/
#define FEATURE_AUDFMT_AMR

/* Enables the IMovieFile extension to BREW, which can be used to read
 * and write movie files in the MP4 file format and related file formats.
 * It is necessary for certain application features, such as "audio dubbing"
 * for Qcamcorder.  FEATURE_VIDEO_AUDIO_DUBBING is a prerequisite (it must
 * also be enabled in order to enable this feature).
*/
#define FEATURE_BREW_IMOVIEFILE
/* Enables a special "offline" mode of the Qcamcorder video engine,
 * which allows an application to write a movie file, in the MP4 file
 * format or related file formats, without actually using the video 
 * encoding hardware.  Instead, previously-recorded video and audio
 * are copied from input files to form a new movie file "offline".
 * This is how the "audio dubbing" feature of Qcamcorder is accomplished.
*/
/* #define FEATURE_VIDEO_AUDIO_DUBBING */

/* Enable QTV 2.0 messaging system.
 *
 * This flag should be defined only for builds containing MSG 2.0
 * (MSM6100 and later).
 */ 
#define FEATURE_QTV_MSG_2_0

/* Enable LCD contrast control
 */
#define FEATURE_MP4_LCD_CONTRAST

/* Enable support for playback from memory buffer */
#define FEATURE_MP4_BUFFER_PLAYBACK

/* Enable support for file fragmentation*/
#define FEATURE_FILE_FRAGMENTATION

#ifdef FEATURE_FILE_FRAGMENTATION
# define FEATURE_QTV_INTER_FRAG_REPOS
#endif /* FEATURE_FILE_FRAGMENTATION */

/* Enable support for DIAG CMD */
#define FEATURE_QTV_DIAG_CMD

#ifdef T_QSC6020
#define FEATURE_QTV_VIDEO_DISABLE

#ifdef FEATURE_QTV_VIDEO_DISABLE
#undef FEATURE_MP4_FRAME_TRANSFORMATIONS
#endif
#endif /* T_QSC6020 */
/*Enable support for QTV Video Editing */
#define FEATURE_QTV_VIDEO_EDITING

/*Enable support for Video Editing in Qcamcorder */
/* #define FEATURE_QCAMCORDER_VIDEO_EDITING */
/* #define FEATURE_QCAMCORDER_CUT_N_MERGE */

/*Enable support for Telop Text in Qcamcorder */
/* #define FEATURE_QCAMCORDER_TELOP */
/* #define FEATURE_QCAMCORDER_POST_EDIT */

/* temporary for featurization in camera.c */
/* #define FEATURE_VIDEO_ENCODE_TIMED_TEXT */

/*Enable support for video overlay*/
/* #define FEATURE_VIDEO_ENCODE_OVERLAY */

/* Enable transcoder feature */                             
/* #define FEATURE_MP4_TRANSCODER */
# define FEATURE_VOC_PCM_INTERFACE
# define FEATURE_AMR_VOCODER


/***** NEW FEATURES IN Qtv4.3 *****/

/* Enable SKT Media On Demand (MOD) support
 */
#define FEATURE_QTV_SKT_MOD

#define FEATURE_QTV_MFDRM

/* Temporal scalability support.
 */
/*#define FEATURE_MP4_TEMPORAL_SCALABILITY*/

/* Enable all streaming related Feature only when FEATURE_QTV_STREAMING 
   is enabled */
#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /*FEATURE_QTV_STREAMING */

#define FEATURE_QTV_IPL_SCALING

/**** End NEW FEATURES IN Qtv4.3 *****/

/* Enable support for the new ARM-QDSP Interface, Rev C */
#define FEATURE_VIDEO_PLAYER_INTERFACE_REV_C

/* Enable GRP : in grp.h #if !defined FEATURE_LTK && defined FEATURE_GRP &&
 * defined DISP_DEVICE_16BPP defines GRP_ENABLED
 */
/* #define FEATURE_GRP */

/* #define FEATURE_QCAMCORDER_CUT_N_MERGE */
/* #define FEATURE_QCAMCORDER_POST_EDIT */

#define FEATURE_AAC_PLUS

#ifdef FEATURE_AAC_PLUS
  #define FEATURE_MP4_AAC_PLUS
  #define FEATURE_QTV_PP_ON_ARM
#endif

/* Enable DSP based AV Sync */
#define FEATURE_QTV_CMX_AV_SYNC_BYTES

/* If this feature is turned ON and the video frame is bigger than the display
 * screen, then we will scale down the video frame to fit in the display screen.
 */
#ifndef FEATURE_QTV_VIDEO_DISABLE
#define FEATURE_SMALL_DISPLAY_SCREEN
#endif /* FEATURE_QTV_VIDEO_DISABLE */

#endif /* CUSTMP4_H */


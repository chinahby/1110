#ifndef CUSTMP4_H
#define CUSTMP4_H
/*===========================================================================

            " M S M 6 0 5 0 -  S U R F "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target using MSM6275 ARM Processor.

  Copyright (c) 2005   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/qtv/build/cust/rel/ULC/1.0/custmp4.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/07   gdp     On Sateesh’s suggestion we enable #define FEATURE_FILE_FRAGMENTATION
09/10/07   gdp     On Sateesh’s suggestion we disable FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
08/24/07   gdp     Added BCMCS Support.
04/26/07   gopi    Moved feature of camcorder into #ifndef 6055
01/19/05   wh      Added overlay and video engine frame processing api.
01/18/05   dw      Enabled FEATURE_VIDEO_ENCODE_OVERLAY for camcorder
08/04/04   drl     Enabled FEATURE_VIDEO_ENCODE for camcorder bring-up.
09/04/03   enj     Removing dead features
08/29/03   sjy     Defined feature for temporal scalability, removed
                   FEATURE_MP4_TS_DSP_INTERFACE
07/25/02    ny     Moved the definition of FEATURE_AAC from custcmx.h to custmp4.h
07/14/02    ny     Initial creation
===========================================================================*/

/*************************************************************************************
        IMPORTANT NOTES:
        -----------------
    1. The audio/video  functionality has been featurized and set to ON by default,
       through the cust files. If Licensee does not intend to use this functionality
       it must be manually set to OFF.
    2. A licensee that has to turn OFF H.264 and/or WindowsMedia functionality,
       should NOT modify the files custh264.h and/or custwm.h.
       In order to turn off these features, he has to modify the file
       msm6250.builds in the follwing manner:
            - to disable H.264 functionality,        delete the line "h264   make"
            - to disable WindowsMedia functionality, delete the line "wm     make"
*************************************************************************************/



/* Enable MPEG4 decoder
*/
/* This audio/video  functionality has been featurized and set to ON by
** default. If Licensee does not intend to use this functionality it
** must be manually set to OFF
*/
#define FEATURE_MP4_DECODER


/* BREWapi MPEG4 feature definitions
*/
#define   FEATURE_APP_MPEG4

/* BREWapi Multiple File Playback feature definition
*/
#define   FEATURE_APP_MEDIA_MULTIPLE_FILE

/*  Enable 1/8 VGA Support
 */
#define FEATURE_MP4_ONEEIGHTH_VGA


/* Enable AAC frame markers
 */
#define FEATURE_MP4_AAC_FRAME_MARKER

/************************/
/* For ULC Bring up Support*/ 
/************************/
#define FEATURE_32BIT_ALIGNMENT 
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

/* Enable the Sleep watchdog reporting fix */
/*#define FEATURE_MP4_MONITOR_SLEEP_DOG_REPORT */

/* Enable port mapping traversal algorithm to allow streaming to external
 * RealServers outside of the network's firewall
 */
/*#define FEATURE_MP4_FIREWALL_PASS_THROUGH_RN */


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

/* Enable frame transformation (rotation and scaling) support.
 * NOTE: No longer supported in DSP in 6550--part of MDP hardware
 */
/*  #define FEATURE_MP4_FRAME_TRANSFORMATIONS */

/* Enable support for temporal scalability
 */
#define FEATURE_MP4_TEMPORAL_SCALABILITY

/* Enable MP4 AMR (GSM audio) support
 * Enables AMR audio playback when embedded within
 * an MPEG-4 video file.
 */
#define FEATURE_MP4_AMR

/* Enable MP4 MP3 (Mpeg 1/2, layer 3 audio) support
 * Enables MP3 audio playback when embedded within
 * an MPEG-4 video file.
 */
/* This audio/video  functionality has been featurized and set to ON by
** default. If Licensee does not intend to use this functionality it
** must be manually set to OFF
*/

/* Enables RFC 3267 AMR file format support.
*/
#define FEATURE_AUDFMT_AMR



/* Enable QTV 3.0 messaging system.
 */
#define FEATURE_QTV_MSG_3_0

/* Enable LCD contrast control
 */
#define FEATURE_MP4_LCD_CONTRAST


/* This audio/video  functionality has been featurized and set to ON by
** default. If Licensee does not intend to use this functionality it
** must be manually set to OFF
*/
#define FEATURE_AAC_PLUS

#define FEATURE_ENHANCED_AAC_PLUS

/* Enable support for DIAG CMD */
#define FEATURE_QTV_DIAG_CMD

/*Enable support for QTV Video Editing */
#define FEATURE_QTV_VIDEO_EDITING



#define FEATURE_DISABLE_QVGA_DECODE 

/*
 * Enable Heap cache
 */
#define FEATURE_MEMUTILS_NEW_DELETE
#define FEATURE_MEMUTILS_NEW_DELETE_HEAP_CACHE
#define FEATURE_MEMUTILS_HEAP_CACHE_ENHANCED_DEBUGGING

#define FEATURE_FILE_FRAGMENTATION 
/* Qtv 4.4 Features */
#if defined (FEATURE_FILE_FRAGMENTATION)
     #define FEATURE_QTV_INTER_FRAG_REPOS
     #if defined (FEATURE_QTV_INTER_FRAG_REPOS)
         #define FEATURE_QTV_RANDOM_ACCESS_REPOS
     #endif  /*FEATURE_QTV_INTER_FRAG_REPOS */
     #define FEATURE_QTV_PSEUDO_STREAM
#endif  /* FEATURE_FILE_FRAGMENTATION */

/* Features for SKT */
#define FEATURE_QTV_SKT_MOD
#define FEATURE_QTV_SKT_MOD_MIDI



/* Enable DSP interface */
#define FEATURE_VIDEO_PLAYER_INTERFACE_REV_C

/* Enable DSP interface for 6260*/
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
 #define FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
#endif

/* Enable MDP support */
//#define FEATURE_QTV_MDP

/* Enable MDP transformations */
//#define FEATURE_QTV_MDP_TRANSFORMATIONS

/* Ascaling */
//#define FEATURE_QTV_MDP_ASCALE

#if (defined T_QSC6065) || (defined T_QSC6075) || (defined T_QSC6085) || (defined T_QSC1100) 
#ifdef BOOT_MODE_NAND

/* Enable Video Eng Frame-Processing API for special effects */
#ifdef USES_CAMCORDER
#define FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#endif /* USES_CAMCORDER */

#endif /* BOOT_MODE_NAND */
#endif /* T_QSC6055 || T_QSC6075 || T_QSC6085*/


/* Enable CMX A/V synch call back support */
#undef FEATURE_QTV_CMX_AV_SYNC
#ifdef FEATURE_QTV_CMX_AV_SYNC
#error code not present
#endif

/* Enable enhanced CMX A/V synch call back support, in which
   the DSP is providing (for WMA decoding) feedback of the number
   of bytes processed, in addition to the number of samples.
   This feature and the FEATURE_QTV_CMX_AV_SYNC are mutually exclusive
*/
#define FEATURE_QTV_CMX_AV_SYNC_BYTES
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
    /* Clock drift compensation is turned off due to 
    introduction of the CMX_AV_SYNC feature */
    #undef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
#endif




/* Enable option to play clips with H264 video + AMR audio */
#define FEATURE_QTV_H264_AMR




#define FEATURE_QTV_STATIC_TASKS

#define FEATURE_QTV_STATIC_DLAGENT_TASK
#define FEATURE_QTV_STATIC_DLDSPSVC_TASK
#define FEATURE_QTV_DECODER_INVALIDATE_CACHE

//Gemsea Add
#define FEATURE_QTV_IPL_SCALING
#endif /* CUSTMP4_H */



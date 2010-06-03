#ifndef CUSTCAMCORDER_H
#define CUSTCAMCORDER_H
/*===========================================================================

            " M S M C A M C O R D E R "   H E A D E R   F I L E

DESCRIPTION
  Configuration for SURF Target for camcorder.
  This particular build supports Camcorder related features.

  Copyright (c) 2001,2002   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camcorder/build/1110/cust/main/latest/custcamcorder.h#4 $ $DateTime: 2008/09/24 02:43:42 $ $Author: akanakan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/08   ps     Added Featurization of Timestamp special effects support.
11/14/07  jdas   DCVS changes for SC2X target
10/22/07   sanal   Disabled DVS feature.
04/11/07   gopi    Enable Camcorder features for sc2x.
08/08/06   jdas    Added and enabled a new feature FEATURE_VIDEO_CLIP_INFO
06/14/06   Shahed  Enabled MB/Frame variance for H.264.
03/23/05   cl      Enable FEATURE_VIDEO_MULTI_UUID
12/20/05   Shahed  Added and enabled a new feature DETAILED_SPACE_OUT_MSG.
11/02/05   dlr     Enable
10/03/05   cl      Enable FEATURE_VIDEO_ENCODE_DEBUG
06/08/05   drl     Enable FEATURE_VIDEO_ENCODE_BITRATE and
                   FEATURE_VIDEO_ENCODE_VDEC_SUPPORT
06/01/05   drl     Enable FEATURE_VIDEO_ENCODE_RAW
05/25/05   drl     Creation
===========================================================================*/

/* Enable the video encoder application.
*/
#define FEATURE_APP_QCAMCORDER
/* Enable video encoding.
*/
#define FEATURE_VIDEO_ENCODE
/* Enable the MPEG4 encoder.
*/
#define FEATURE_MP4_ENCODER

/* Enable Video Encode transition effects
*/
#define FEATURE_VIDENC_TRANSITION_EFFECTS

/*Enable support for Telop Text in Qcamcorder */
#define FEATURE_QCAMCORDER_TELOP

/* Enable Frame Overlay feature in Qcamcorder */
#define FEATURE_VIDEO_ENCODE_OVERLAY_UI

/* Eliminate obsolete profiles */
#define FEATURE_CAMCORDER_DELETE_LEGACY_PROFILES

/* Enable Video Eng Frame-Processing API for special effects */
#define FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

/* Enable Video Encode Bitrate */
#define FEATURE_VIDEO_ENCODE_BITRATE

/* Enable Video Encode VDEC Support */
#define FEATURE_VIDEO_ENCODE_VDEC_SUPPORT

/* Enable Video Encode Debug */
#define FEATURE_VIDEO_ENCODE_DEBUG 

#define FEATURE_MP4E_MULTI_VFE

// This feature enables detailed space out warnings/error msgs for camcorder
// Caution: MUST be enabled for 6550, 6275 targets
#define DETAILED_SPACE_OUT_MSG

/* Enable support of setting multiple uuid to a video clip */
/* This feature requires Brew version 3.1.5 and up */
#define FEATURE_VIDEO_MULTI_UUID

/* Enable QVGA recording with 7.5 fps */
#define FEATURE_QVGA_LOW_FPS_ONLY

/* enable fad-in and fade-out on start/stop cycles
*/
#define FEATURE_VIDEO_ENCODE_FADING

/* the following feature is defined to enable Dynamic
 * clock voting scheme in camcorder
*/
#define FEATURE_CAMCORDER_DCVS


#endif /* CUSTCAMCORDER_H */


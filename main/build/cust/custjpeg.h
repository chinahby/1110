/*===========================================================================

            " C u s t - J P E G"   H E A D E R   F I L E

DESCRIPTION
  Configuration for JPEG Features.

  Copyright (c) 2004 - 2005 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/build/cust/qsc1110/main/latest/inc/custjpeg.h#6 $ $DateTime: 2009/01/07 20:56:30 $ $Author: kdivvela $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/09   kdiv    Moved FEATURE_PNG_DECODER from custcmx.h
04/07/08   KC      Moved FEATURE_QCT_JPEG_DECODER from OEMFeatures.h
05/23/08   chai    Removed Geo tagging feature
01/29/06   arv     Added JPEG Feature
12/08/06   vma     Added newline character to the end of file to fix compilation warning
11/22/06   arv     Added feature for JPEG Decoder Resize
05/25/06   arv     Added feature for ARM JPEG Decoder
03/17/06   dl      Added precompiler definition for cache lines and 
                   video resources.
08/17/05   pdo     Added PNG encoder feature
08/03/05   pdo     Added #define
07/05/05   pdo     Initial revision.

===========================================================================*/

/*---------------------------------------------------------------------------
                            JPEG  Services
---------------------------------------------------------------------------*/
#ifdef FEATURE_CAMERA
#define FEATURE_JPEG_ENCODER
#endif

#ifndef FEATURE_USES_LOWMEM
#define FEATURE_JPEG_DECODER
#define FEATURE_QCT_JPEG_DECODER // Use the QCT provided JPEG decoder.
#endif

#ifdef FEATURE_JPEG_ENCODER

/* MSM6550/6275/6800 use encoder rev2 */
#define FEATURE_JPEG_ENCODER_REV2
#endif

#ifndef 
#ifdef FEATURE_JPEG_DECODER
/* MSM6550/6275/6800 use decoder rev2 */
#define FEATURE_JPEG_DECODER_REV2

#endif


/*---------------------------------------------------------------------------
                            PNG  Services
---------------------------------------------------------------------------*/
#ifndef FEATURE_IPL_NO_CAMERA
/* Enables the PNG image encoder software.
*/
#define FEATURE_PNG_ENCODER
#endif
/* Enables the PNG image decoder software. Moved here from custcmx.h
*/
#ifndef FEATURE_PNG_DECODER
  #define FEATURE_PNG_DECODER
#endif


/*---------------------------------------------------------------------------
                       JPEG precompiler definition
---------------------------------------------------------------------------*/

/* This definition allows jpeg module to clear selective cache lines */
/* instead of entire cache. */
#define JPEG_DATA_CACHE_LINES
#ifdef FEATURE_CAMERA
/* This definition allows jpeg module to use video resources for jpeg */
#define JPEG_VIDEO_RESOURCES
#endif

/*---------------------------------------------------------------------------
                      JPEG ARM Decoder Support
---------------------------------------------------------------------------*/
#define FEATURE_JPEG_ARM_STANDALONE_DECODER

/*---------------------------------------------------------------------------
                      JPEG Decoder Resize Feature Support
---------------------------------------------------------------------------*/
#ifndef FEATURE_IPL_NO_CAMERA
 #define FEATURE_JPEGD_RESIZE 
#endif
/*---------------------------------------------------------------------------
                      Print Image Matching Support
---------------------------------------------------------------------------*/
#define FEATURE_EXIF_PIM3


/*---------------------------------------------------------------------------
                      String Library Support
---------------------------------------------------------------------------*/
#define JPEG_USES_LIBSTD

/*---------------------------------------------------------------------------
                      JPEG QDSP ID
---------------------------------------------------------------------------*/
#define JPEG_USES_QDSP_JPEGID








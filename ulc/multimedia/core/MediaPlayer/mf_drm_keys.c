
/*=============================================================================
  FILE: mf_drm_keys.c

  SERVICES: DRM key
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright @2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/MediaPlayer/mf_drm_keys.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/05   jrr     Added includes for target.h and customer.h, more FEATURE #ifdefs

===========================================================================*/


/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */

#include "AEEFile.h"
#include "mf_drm_keys.h"
#include "AEEStdLib.h"

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_APP_MPEG4
#include "OEMMediaMPEG4.h"
#include "AEEMediaMPEG4.h"
#endif

#define KEY_TAG_LENGTH 8
#define FILE_NAME 512
#define BLOCK_SIZE 512

#ifdef FEATURE_APP_MPEG4
#if MM_MP4_PARM_DECRYPT_KEY != 456
#error MM_MP4_PARM_DECRYPT_KEY defined in AEEMediaMPEG4.h should be equal to 456
#endif
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

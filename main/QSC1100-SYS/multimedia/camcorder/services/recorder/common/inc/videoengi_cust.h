#ifndef VIDEOENGI_CUST_H
#define VIDEOENGI_CUST_H
/*===========================================================================

                  V I D E O   E N C O D E   E N G I N E
         H E A D E R   F I L E   F O R   C U S T O M I Z A T I O N

DESCRIPTION
  This header file contains internal definitions to customize the video 
  encode engine module.

Copyright(c) 2003-2005 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/videoengi_cust.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/06   dhaval  Added LOW, MED and HIGH value for VIDEO_ENG_LIMIT_NEAR_THRESHOLD
                   and VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD.
07/18/05   jk      Added VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH.
                   Changed VIDEO_ENG_OPTIMAL_CHUNK_LENGTH from ms to seconds.
06/14/05   jk      Changed feature name to FEATURE_VIDEO_ENG_DEBUG.
05/31/05   jk      Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/***************************************************************************/
/*                                                                         */
/***************************************************************************/

/* This is the desired interlace rate, in milliseconds, when recording both
** video and audio to a movie file.
** This is used only if both video and audio are present.  Chunks will be
** written out on whichever condition comes first, whether the interlace rate 
** is met or the VIDEO_ENG_OPTIMAL_CHUNK_SIZE amount of video data is ready.
** This may affect the stream buffer size and chunk table size.
*/
#define VIDEO_ENG_DESIRED_INTERLACE_RATE     3000

/* This is the desired minimum length in seconds of each chunk of media data
** to write to the output at once.
** Media data chunks will be written out if this amount of video data is 
** ready or if interlace period has elapsed (when both video and audio 
** streams are present), whichever comes first.
** This may affect the video/audio stream buffer size and video/audio chunk 
** table size.
*/
#define VIDEO_ENG_OPTIMAL_CHUNK_LENGTH       3

/* This is the estimated length in seconds of each chunk of audio data,
** if each chunk is VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE bytes in size.
*/
#define VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH 3

/* These are the number of seconds warning that the engine will attempt to
** give the client in two stages before the recording limit, either running
** out of sample table memory or filesystem space, is reached during
** recording.
*/
#define VIDEO_ENG_LIMIT_NEAR_THRESHOLD       10
#define VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD   3

/* This is the size to make each fragment of fragmented 3G2 (KDDI Movie File
** Format Specification for cdma2000 1x/1xEV-DO v1.1.0) files, in seconds.
** This may affect the video/audio sample table size and the video/audio chunk
** table size if fragment is enabled.
*/
#define VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE     15

/* This is the maximum length of files, in seconds if fragment is disabled.
** This has no impact if fragment is enabled.
** This may affect the video/audio sample/chunk table size and the table 
** stores table size if fragment is disabled.
*/
#define VIDEO_ENG_MAX_STREAM_LENGTH          3600

/* If this is defined, fixed video/audio stream buffer size and fixed 
** video/audio sample/chunk table size are used regardless of other base 
** parameters such as frame rate and bit rate.
** Those fixed parameters should be set large enough for the worst case.
** If this is not defined, video/audio stream buffer size and video/audio
** sample/chunk table size are determined based on other parameters such 
** as frame rate and bit rate.
*/
//#define FEATURE_VIDEO_ENG_FIXED_PARAMETERS

/* If this is defined, video encode engine prints the value of the macro
** functions.
*/
//#define FEATURE_VIDEO_ENG_DEBUG

/* Low threshold value for space limit threshold value.
 * It will show space warning twice over UI while running out of sample
 * table memory and filesyste space.
 * 
*/
#define VIDEO_ENG_LIMIT_NEAR_THRESHOLD_LOW       5
#define VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_LOW   1

/* Threshold value high then LOW and default.
 * It will show space warning twice over UI while running out of sample
 * table memory and filesyste space.
 * 
*/
#define VIDEO_ENG_LIMIT_NEAR_THRESHOLD_MED       20
#define VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_MED   6

/* Highest space limit threshold value.
 * It will show space warning twice over UI while running out of sample
 * table memory and filesyste space.
 * 
*/
#define VIDEO_ENG_LIMIT_NEAR_THRESHOLD_HIGH       40
#define VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD_HIGH   12

#endif /* VIDEOENGI_CUST_H */

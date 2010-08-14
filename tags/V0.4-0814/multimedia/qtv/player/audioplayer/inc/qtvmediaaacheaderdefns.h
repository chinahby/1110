#ifndef QTV_MEDIA_AAC_HEADER_DEFNS_H
#define QTV_MEDIA_AAC_HEADER_DEFNS_H
/* =======================================================================

                          QtvMediaAACHeaderDefns.h

DESCRIPTION

 Definitions for mask, offset ans shift details about the AAC ADTS header.
 Each AAC raw data block or block of 2 to 4 raw data blocks grouped as a 
 ADTS(Audio Data Transport Stream) packet by adding a header in front of it. 
 The format of ADTS header is as follows:
 
 Fixed header (These fields don't change from frame to frame):
   Sync Marker (12): set to 0xFFF
    ID (1): {0: MPEG-4, 1: MPEG-2}
    Layer (2): {'00'}
    Protection Absent (1): {'1'}
    Profile (2): {'00': Main ,'01': Low Complexity, 
                      '10':Scalable Sampling Rate}
    Sampling Frequency Index (4): {0: 96000, 1: 88200,
                                   2: 64000, 3: 48000,
                                   4: 44100, 5: 32000,
                                   6: 24000, 7: 22050,
                                   8: 16000, 9: 12000,
                                   a: 11025, b:  8000}
    private_bit (1): {'0'}
    channel_configuation (3): No of Speakers 
    original/copy (1): {0:Copyright Absent,
                        1:Copyright Present}
    home (1): {0: copy , 1: Original}
     
  Variable header (These fields change from frame to frame):
    copyright_identification_bit (1): one of 72 bit copyright info
    copyright_identification_start (1): {0: Identification bit is not present as first,
                                         1: Identification bit is present as first}
    Frame length (13): Length of the frame including header and error checks
    adts_buffer_fullness (11): Dont Care
    Number of raw data blocks in frame (2)
  Optional header (only if protection_absent == 0):
    crc_check (16)
 

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/inc/qtvmediaaacheaderdefns.h#1 $ 
$DateTime: 2008/05/19 02:58:01 $
$Change: 661882 $ 

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */



/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */




#define QTV_MEDIA_AAC_SAMPLES_PER_FRAME 1024
#define QTV_MEDIA_NUM_MS_IN_SEC 1000
#define QTV_MEDIA_ADTS_INVALID_TIMESTAMP       0xFFFFFFFF
/* ADTS Sync Marker value: 0xFFF */
/* High Sync Marker */
#define QTV_MEDIA_ADTS_HIGH_SYNC_MARKER_MASK   0xFF
#define QTV_MEDIA_ADTS_HIGH_SYNC_MARKER_VALUE  0xFF

/* Low Sync Marker */
#define QTV_MEDIA_ADTS_LOW_SYNC_MARKER_MASK    0xF0
#define QTV_MEDIA_ADTS_LOW_SYNC_MARKER_VALUE   0xF0


/* Minimum length of the ADTS header. */

#define QTV_MEDIA_ADTS_MINIMUM_HEADER_SIZE        7 
#define QTV_MEDIA_ADTS_MAXIMUM_HEADER_SIZE        9

/* Frame length */
#define QTV_MEDIA_ADTS_FRAME_LENGTH_OFFSET        3 

/* 2 least significant bits of byte at offset 3
 * 8 bits of byte at offset 4 and
 * 3 most significant bits of byte at offset 5
 */
#define QTV_MEDIA_ADTS_HIGH_FRAME_LENGTH_MASK        0x03
#define QTV_MEDIA_ADTS_HIGH_FRAME_LENGTH_SHIFT         11
#define QTV_MEDIA_ADTS_MID_FRAME_LENGTH_MASK         0xFF
#define QTV_MEDIA_ADTS_MID_FRAME_LENGTH_SHIFT           3
#define QTV_MEDIA_ADTS_LOW_FRAME_LENGTH_MASK         0xE0
#define QTV_MEDIA_ADTS_LOW_FRAME_LENGTH_SHIFT           5

// Actually AAC frame length is always less than 800
#define QTV_MEDIA_ADTS_MAX_AAC_ADTS_FRAME_LENGTH     1024

/* Id: MPEG-2 and MPEG-4 */
#define QTV_MEDIA_ADTS_MPEG_ID_OFFSET                   1
#define QTV_MEDIA_ADTS_MPEG_ID_MASK                  0x08
#define QTV_MEDIA_ADTS_MPEG_ID_SHIFT                    3

/* Layer */
#define QTV_MEDIA_ADTS_LAYER_OFFSET                     1
#define QTV_MEDIA_ADTS_LAYER_MASK                    0x06
#define QTV_MEDIA_ADTS_LAYER_SHIFT                      1
#define QTV_MEDIA_ADTS_LAYER_VAL                     0x00

/* Protection Absent */
#define QTV_MEDIA_ADTS_PROTECTION_ABSENT_OFFSET         1
#define QTV_MEDIA_ADTS_PROTECTION_ABSENT_MASK        0x01

/* Profile Id */
#define QTV_MEDIA_ADTS_PROFILE_ID_OFFSET                2
#define QTV_MEDIA_ADTS_PROFILE_ID_MASK               0xC0
#define QTV_MEDIA_ADTS_PROFILE_ID_SHIFT                 6

#define QTV_MEDIA_ADTS_PROFILE_ID_SCALABLE_SAMPLING_RATE               2

/* Sampling frequency index */
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_OFFSET       2
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_MASK      0x3C
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_INDEX_SHIFT        2

/* Channel Count */
#define QTV_MEDIA_ADTS_CHANNEL_COUNT_OFFSET             2 

/* 1 least significant bit of byte at offset 2
 * 2 most significant bits of byte at offset 3 
 */
#define QTV_MEDIA_ADTS_HIGH_CHANNEL_COUNT_MASK        0x01
#define QTV_MEDIA_ADTS_HIGH_CHANNEL_COUNT_SHIFT          2
#define QTV_MEDIA_ADTS_LOW_CHANNEL_COUNT_MASK         0xC0
#define QTV_MEDIA_ADTS_LOW_CHANNEL_COUNT_SHIFT           6

/* Sampling Frequency index values present within ADTS Header */
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_96KHZ  0x0
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_88KHZ  0x1
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_64KHZ  0x2
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_48KHZ  0x3
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_44KHZ  0x4
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_32KHZ  0x5
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_24KHZ  0x6
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_22KHZ  0x7
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_16KHZ  0x8
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_12KHZ  0x9
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_11KHZ  0xa
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_ID_8KHZ   0xb

/* Sampling Frequency values associated with the indexes */
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_8KHZ   8000
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_16KHZ  16000

#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_12KHZ  12000
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_24KHZ  24000

#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_32KHZ  32000
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_64KHZ  64000

#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_48KHZ  48000
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_96KHZ  96000


#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_11KHZ  11050
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_22KHZ  22050
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_44KHZ  44100
#define QTV_MEDIA_ADTS_SAMPLING_FREQ_VALUE_88KHZ  88200


#define QTV_MEDIA_AAC_ADTS_AUDIO_OBJECT_TYPE_LEFT_SHIFT   3
#define QTV_MEDIA_AAC_ADTS_HIGH_SF_INDEX_MASK          0x0E
#define QTV_MEDIA_AAC_ADTS_HIGH_SF_INDEX_RIGHT_SHIFT      1
#define QTV_MEDIA_AAC_ADTS_LOW_SF_INDEX_MASK            0x1
#define QTV_MEDIA_AAC_ADTS_LOW_SF_INDEX_LEFT_SHIFT        7
#define QTV_MEDIA_AAC_ADTS_CC_INDEX_MASK               0x0F
#define QTV_MEDIA_AAC_ADTS_CC_INDEX_LEFT_SHIFT            3

// Valid sample frequency indexes are 0x0 through 0xb 
#define QTV_MEDIA_AAC_ADTS_INVALID_SF_INDEX            0x7f

#define QTV_MEDIA_AAC_ADTS_INVALID_SAMPLING_FREQUENCY     0

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */


#endif

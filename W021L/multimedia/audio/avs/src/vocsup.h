#ifndef VOCSUP_H
#define VOCSUP_H
/*===========================================================================

            V O C O D E R    S U P P O R T     H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the vocoder support
  functions.                                                     

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  vocsup_enc_format_to_rate  
    Use this routine to translate the encoder format byte to the equivalent 
    voc_rate_type.
  vocsup_dec_format_to_rate  
    Use this routine to translate the decoder format byte to the equivalent 
    voc_rate_type.
  vocsup_rate_to_size    
    Use this routine to determine the length of the Decoder or Encoder packet
    based on the rate of the packet.
  vocsup_compute_freq
    This function returns the vocoder tone value for a given frequency
    to be played by the vocoder.
  vocsup_pb_q_size       
    This function returns number of entries in the specified voice play back 
    queue.
  vocsup_pb_get_hdr
    This function returns the pointer to the current entry of the specified 
    vocoder play back queue.
  vocsup_pb_get_frame    
    This function is called to get the current frame to output in a voice
    play back session.
  vocsup_pb_add_entry    
    This function adds one entry to the specified voice play back queue.
  vocsup_pb_clear_q      
    This function clears the specified voice play back queue.
  voc_bytes_to_frames
    Given the specified vocoder and vocoder frame format, determine how many 
    COMPLETE vocoder frames in the given format will fit into the number of bytes
    specified.  In the case of variable size formats, out_min_frames and 
    out_max_frames may be set to different values.
  voc_count_frames
    Given the specified vocoder and vocoder frame format, examine the buffer of 
    vocoder frames passed in and determine how many COMPLETE vocoder frames are 
    present, up to the number of frames specified in inout_frame_count.  Return 
    the number of COMPLETE vocoder frames and how many bytes those frames 
    occupy.  Always assume that the first byte of the input buffer begins a new 
    vocoder frame. 
  vocsup_rec_clear_buffers
    This function clears the recording buffer queue.
  vocsup_rec_put_buf
    This function accepts a buffer to be used for recording.
  voc_rec_free_buf
    This function cleans up the recording buffers.
  vocsup_rec_frame
    This function records one voice frame. This function copies the voice frame 
    to the record buffer without disturbing the frame itself.
  vocsup_rec_start
    This function starts the recording process at the frame data level.
  vocsup_rec_stop
    This function stops voice recording.  This function will return the pointer
    of the last buffer in the recording session.
  vocsup_rec_change_config
    This function is called when the vocoder driver detects that the
    configuration has changed while an in-call recording session is in 
    progress.
    This function updates the configuration specified in the header of the 
    recorded buffers from this point on.
  voc_get_shared_mem_ptr
    This function returns a pointer to the shared RAM area between the vocoder
    driver and the VR engine.  The vocoder shared RAM area is limited to two
    buffers used in voice recording.  The purpose of sharing RAM is to eliminate
    RAM usage for the recording buffers.  By sharing RAM, voice recording (for
    voice memo only at this point) and voice recognition can not be running 
    concurrently.
  vocsup_validate_frame_control_byte
    Given the input vocoder capability, check if the input frame control byte is
    valid.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1998 - 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/vocsup.h_v   1.0   06 Oct 2001 12:57:56   ropalsky  $
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocsup.h#2 $ $DateTime: 2008/12/30 06:20:17 $ $Author: subhashj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
11/20/06    az     Fixed the CR: 85787 and warning issues for MM API.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
12/08/04   ymc     Externalized vocsup_native_header_to_gsm_mode, 
                   vocsup_native_header_to_gsm_rate, 
                   and vocsup_gsm_to_native_header.
           ymc     Updated Copyright.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
10/10/03    aw     Added MSMAUD_VOC_AMR_COMMON featurization.
04/17/03    sm     Added defines for AMR file format support.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
11/18/02    st     Featurized GSM/WCDMA specific code; modified
                   vocsup_rate_to_size() function prototype.
11/06/02    sm     Added QCP AMR support.
07/09/02    sm     Merged to perforce tip.
            sm     Removed old tone generation constants.
09/03/01    st     Redesigned vocoder packet recording feature to: accept
                   recording buffers from client instead of declaring buffers
                   locally, declare and handle vocoder packet buffers.
04/04/01    st     Added Native QCP frame byte definitions.
11/27/00    sm     Added stereo DAC DTMF tables.
07/27/00    ro     Remove size parameter from vocsup_rec_frame().
05/30/00    st     Added support for DTMF frequency calculation for 22kHz 
                   sampling rate.
01/26/00    tc     Added VR Mode A support.
10/25/99   spf     Replaced VOC_API_VERSION with T_MSM31.
10/14/99   spf     Replaced FEATURE_QDSP2 with VOC_API_VERSION.
07/30/99    st     Added vocoder values for DTMF frequencies for Pink Panther.
07/21/99    vl     Implemented voice answering machine feature (playback on
                   reverse link and both links, record forward link).  Created
                   vocsup_enc_format_to_rate () and vocsup_dec_format_to_rate ()
                   so playback can accomodate erasure frames, blank frames and 
                   full rate error frames.  Added frame format byte defines.
04/15/99    vl     Added VSP_MAX_REC_INTERVAL_MS.
04/07/99    vl     Removed externs for 3 vocsup functions due to name change,
                   added #include customer.h.
03/04/99    st     Added vocsup_compute_freq() function prototype; added 
                   defines for vocoder tone values, also added 
                   vocsup_freq_value_type typedef, these are used for 
                   calculating the vocoder value of a tone from its frequency 
                   value.
03/04/99    vl     Implemented Voice Record to support Voice Memo (see 
                   FEATURE_VOICE_RECORD) plus misc clean up.
12/04/98    vl     Added one more parameter to vocsup_pb_get_frame ().
                   Created function vocsup_pb_get_hdr ().
08/31/98    vl     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "msmaud.h"     /* Target specific audio features                  */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
/*---------------------------------------------------------------------------

  Vocoder tones

---------------------------------------------------------------------------*/
/*
**    Vocoder tone = 16384.0 * cos( 2.0 * Pi * frequency / sample rate ) + 0.5
**
**    2*Pi radians is 1 cycle, 8000  or 22000 is the sample rate of the Vocoder,
**    16384 is for scaling the number for fixed point arithmatic, the
**    0.5 is for rounding.  All values wrap to 0 at 16384.
*/
#define VOC_100_HZ_8K     16333
#define VOC_150_HZ_8K     16270
#define VOC_200_HZ_8K     16182
#define VOC_250_HZ_8K     16069
#define VOC_300_HZ_8K     15931
#define VOC_350_HZ_8K     15769
#define VOC_400_HZ_8K     15582
#define VOC_450_HZ_8K     15371
#define VOC_500_HZ_8K     15137
#define VOC_550_HZ_8K     14879
#define VOC_600_HZ_8K     14598
#define VOC_650_HZ_8K     14295
#define VOC_700_HZ_8K     13970
#define VOC_750_HZ_8K     13623
#define VOC_800_HZ_8K     13255
#define VOC_850_HZ_8K     12867
#define VOC_900_HZ_8K     12458
#define VOC_950_HZ_8K     12031
#define VOC_1000_HZ_8K    11585
#define VOC_1050_HZ_8K    11121
#define VOC_1100_HZ_8K    10641
#define VOC_1150_HZ_8K    10143
#define VOC_1200_HZ_8K    9630
#define VOC_1250_HZ_8K    9102
#define VOC_1300_HZ_8K    8561
#define VOC_1350_HZ_8K    8006
#define VOC_1400_HZ_8K    7438
#define VOC_1450_HZ_8K    6859
#define VOC_1500_HZ_8K    6270
#define VOC_1550_HZ_8K    5671
#define VOC_1600_HZ_8K    5063
#define VOC_1650_HZ_8K    4447
#define VOC_1700_HZ_8K    3825
#define VOC_1750_HZ_8K    3196
#define VOC_1800_HZ_8K    2563
#define VOC_1850_HZ_8K    1926
#define VOC_1900_HZ_8K    1285
#define VOC_1950_HZ_8K    643
#define VOC_2000_HZ_8K    1
#define VOC_2050_HZ_8K    64893
#define VOC_2100_HZ_8K    64251
#define VOC_2150_HZ_8K    63610
#define VOC_2200_HZ_8K    62973
#define VOC_2250_HZ_8K    62340
#define VOC_2300_HZ_8K    61711
#define VOC_2350_HZ_8K    61089
#define VOC_2400_HZ_8K    60473
#define VOC_2450_HZ_8K    59865
#define VOC_2500_HZ_8K    59266
#define VOC_2550_HZ_8K    58677
#define VOC_2600_HZ_8K    58098
#define VOC_2650_HZ_8K    57530
#define VOC_2700_HZ_8K    56975
#define VOC_2750_HZ_8K    56434
#define VOC_2800_HZ_8K    55906
#define VOC_2850_HZ_8K    55393
#define VOC_2900_HZ_8K    54895
#define VOC_2950_HZ_8K    54415
#define VOC_3000_HZ_8K    53951
#define VOC_3050_HZ_8K    53505
#define VOC_3100_HZ_8K    53078
#define VOC_3150_HZ_8K    52669
#define VOC_3200_HZ_8K    52281
#define VOC_3250_HZ_8K    51913
#define VOC_3300_HZ_8K    51566
#define VOC_3350_HZ_8K    51241
#define VOC_3400_HZ_8K    50938
#define VOC_3450_HZ_8K    50657
#define VOC_3500_HZ_8K    50399
#define VOC_3550_HZ_8K    50165
#define VOC_3600_HZ_8K    49954
#define VOC_3650_HZ_8K    49767
#define VOC_3700_HZ_8K    49605
#define VOC_3750_HZ_8K    49467
#define VOC_3800_HZ_8K    49354
#define VOC_3850_HZ_8K    49266
#define VOC_3900_HZ_8K    49203
#define VOC_3950_HZ_8K    49165
#define VOC_4000_HZ_8K    49152

#define VOC_100_HZ_22K    16377
#define VOC_150_HZ_22K    16369
#define VOC_200_HZ_22K    16357
#define VOC_250_HZ_22K    16342
#define VOC_300_HZ_22K    16324
#define VOC_350_HZ_22K    16302
#define VOC_400_HZ_22K    16277
#define VOC_450_HZ_22K    16249
#define VOC_500_HZ_22K    16217
#define VOC_550_HZ_22K    16182
#define VOC_600_HZ_22K    16144
#define VOC_650_HZ_22K    16102
#define VOC_700_HZ_22K    16058
#define VOC_750_HZ_22K    16010
#define VOC_800_HZ_22K    15958
#define VOC_850_HZ_22K    15904
#define VOC_900_HZ_22K    15846
#define VOC_950_HZ_22K    15785
#define VOC_1000_HZ_22K   15720
#define VOC_1050_HZ_22K   15653
#define VOC_1100_HZ_22K   15582
#define VOC_1150_HZ_22K   15508
#define VOC_1200_HZ_22K   15431
#define VOC_1250_HZ_22K   15351
#define VOC_1300_HZ_22K   15268
#define VOC_1350_HZ_22K   15181
#define VOC_1400_HZ_22K   15092
#define VOC_1450_HZ_22K   14999
#define VOC_1500_HZ_22K   14903
#define VOC_1550_HZ_22K   14805
#define VOC_1600_HZ_22K   14703
#define VOC_1650_HZ_22K   14598
#define VOC_1700_HZ_22K   14491
#define VOC_1750_HZ_22K   14380
#define VOC_1800_HZ_22K   14266
#define VOC_1850_HZ_22K   14150
#define VOC_1900_HZ_22K   14030
#define VOC_1950_HZ_22K   13908
#define VOC_2000_HZ_22K   13783
#define VOC_2050_HZ_22K   13655
#define VOC_2100_HZ_22K   13525
#define VOC_2150_HZ_22K   13391
#define VOC_2200_HZ_22K   13255
#define VOC_2250_HZ_22K   13116
#define VOC_2300_HZ_22K   12975
#define VOC_2350_HZ_22K   12830
#define VOC_2400_HZ_22K   12684
#define VOC_2450_HZ_22K   12534
#define VOC_2500_HZ_22K   12382
#define VOC_2550_HZ_22K   12228
#define VOC_2600_HZ_22K   12071
#define VOC_2650_HZ_22K   11911
#define VOC_2700_HZ_22K   11749
#define VOC_2750_HZ_22K   11585
#define VOC_2800_HZ_22K   11419
#define VOC_2850_HZ_22K   11250
#define VOC_2900_HZ_22K   11078
#define VOC_2950_HZ_22K   10905
#define VOC_3000_HZ_22K   10729
#define VOC_3050_HZ_22K   10551
#define VOC_3100_HZ_22K   10371
#define VOC_3150_HZ_22K   10189
#define VOC_3200_HZ_22K   10005
#define VOC_3250_HZ_22K   9819
#define VOC_3300_HZ_22K   9630
#define VOC_3350_HZ_22K   9440
#define VOC_3400_HZ_22K   9248
#define VOC_3450_HZ_22K   9054
#define VOC_3500_HZ_22K   8858
#define VOC_3550_HZ_22K   8660
#define VOC_3600_HZ_22K   8461
#define VOC_3650_HZ_22K   8259
#define VOC_3700_HZ_22K   8057
#define VOC_3750_HZ_22K   7852
#define VOC_3800_HZ_22K   7646
#define VOC_3850_HZ_22K   7438
#define VOC_3900_HZ_22K   7229
#define VOC_3950_HZ_22K   7018
#define VOC_4000_HZ_22K   6806

#define VOC_UNUSED      0      /* Invalid/Unused Frequency for Vocoder */

/*---------------------------------------------------------------------------
                       VOCODER FRAME FORMAT BYTE 
                       
Masks used to process Vocoder encoder/decoder format byte.
---------------------------------------------------------------------------*/

/* Mask used to validate frame format byte .  
**    Bit 7:  Set to 1 if 13K packet data, 0 if 8K packet data (EVRC)
**    Bit 6:  Should always be 0 (reserved)
*/
#define VSP_VALID_PLAYBACK_FORMAT_M  0xC0      
#define VSP_13K_V                    0x80           /* Use 13K vocoder     */
#define VSP_8K_V                     0x00           /* Use 8K vocoder      */
                                                                         
/* Mask used to get rate information from encoder/decoder frame format byte.   
*/                                                                       
#define VSP_FMT_RATE_M               0x03           /* Frame rate          */
                                                                         
/* Masks for decoder frame format byte.                                 
*/                                                                       
#define VSP_DEC_FULL_LIKELY_M   0x10    /* Flag full-rate-likely frames    */
#define VSP_DEC_FULL_LIKELY_V   0x10    /* Frame is a full-rate-likely     */
#define VSP_DEC_ERASE_M         0x08    /* Flag erased frame               */
#define VSP_DEC_ERASE_V         0x08    /* Frame is an erasure             */
#define VSP_DEC_BLANK_M         0x04    /* Flag blanked frame              */
#define VSP_DEC_BLANK_V         0x04    /* Frame is blanked                */

/* Frame format byte values for Native QCP format
*/
#define VSP_NATIVE_QCP_FULL     0x04    /* Full rate                       */
#define VSP_NATIVE_QCP_HALF     0x03    /* Half rate                       */
#define VSP_NATIVE_QCP_QUARTER  0x02    /* Quarter rate                    */
#define VSP_NATIVE_QCP_EIGHTH   0x01    /* Eighth rate                     */
#define VSP_NATIVE_QCP_BLANK    0x00    /* Blank frame                     */
#define VSP_NATIVE_QCP_ERASURE  0x0E    /* Erasure frame                   */

#if defined(MSMAUD_VOC_AMR_COMMON)
#define VSP_NATIVE_AMR_RATE_M     0x0F
#define VSP_NATIVE_AMR_RATE_475   0x01
#define VSP_NATIVE_AMR_RATE_515   0x02
#define VSP_NATIVE_AMR_RATE_590   0x03
#define VSP_NATIVE_AMR_RATE_670   0x04
#define VSP_NATIVE_AMR_RATE_740   0x05
#define VSP_NATIVE_AMR_RATE_795   0x06
#define VSP_NATIVE_AMR_RATE_1020  0x07
#define VSP_NATIVE_AMR_RATE_1220  0x08

/* When the VSP_NATIVE_XXX makes it to a QCP frame, it is considered converted
** from a TX or RX frame mode to a value listed below.
*/
#define VSP_NATIVE_AMR_MODE_M          0xF0
#define VSP_NATIVE_AMR_MODE_SHIFT      4
#define VSP_NATIVE_AMR_SPEECH_GOOD     0x00  /* good speech frame             */
#define VSP_NATIVE_AMR_SPEECH_DEGRADED 0x10  /* degraded speech frame         */
#define VSP_NATIVE_AMR_ONSET           0x20  /* onset                         */
#define VSP_NATIVE_AMR_SPEECH_BAD      0x30  /* bad speech frame              */
#define VSP_NATIVE_AMR_SID_FIRST       0x40  /* first silence descriptor      */
#define VSP_NATIVE_AMR_SID_UPDATE      0x50  /* successive silence descriptor */
#define VSP_NATIVE_AMR_SID_BAD         0x60  /* bad silence descriptor frame  */
#define VSP_NATIVE_AMR_NO_DATA         0x70  /* no data                       */
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#ifdef FEATURE_AUDFMT_AMR
/* Frame types for 4-bit frame type as in 3GPP TS 26.101 v3.2.0, Sec.4.1.1 */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0475         0    /* 4.75 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0515         1    /* 5.15 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0590         2    /* 5.90 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0670         3    /* 6.70 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0740         4    /* 7.40 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_0795         5    /* 7.95 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_1020         6    /* 10.2 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_1220         7    /* 12.2 kbit /s    */
#define VSP_AUDFMT_AMR_FRAME_TYPE_AMR_SID      8    /* SID frame       */
/* Frame types 9-11 are not supported */
/* Frame types 12-14 are reserved for future use */
#define VSP_AUDFMT_AMR_FRAME_TYPE_NO_DATA      15   /* No data         */

/* Defines to extract frame type and quality from frame header. */
#define VSP_AUDFMT_AMR_FRAME_TYPE_MASK     0x78
#define VSP_AUDFMT_AMR_FRAME_TYPE_SHIFT    3
#define VSP_AUDFMT_AMR_FRAME_QUALITY_MASK  0x04
#define VSP_AUDFMT_AMR_FRAME_QUALITY_SHIFT 2
  
#endif /* FEATURE_AUDFMT_AMR */

/* AMR-WB File */
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_0660          0
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_0885          1
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_1265          2
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_1425          3
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_1585          4
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_1825          5
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_1985          6
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_2305          7
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_2385          8
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_SID           9
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_SPEECH_LOST   14
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_NO_DATA       15

/* Defines to extract frame type and quality from frame header. */
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_MASK     0x78
#define VSP_AUDFMT_AMR_WB_FRAME_TYPE_SHIFT    3
#define VSP_AUDFMT_AMR_WB_FRAME_QUALITY_MASK  0x04
#define VSP_AUDFMT_AMR_WB_FRAME_QUALITY_SHIFT 2

#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */

/*---------------------------------------------------------------------------
                        VOCODER DATA BIT COUNTS

The following constants specify the number of bits used per vocoder frame
at the various data rates.
---------------------------------------------------------------------------*/

#define  VOC_8_BITS  16    /* Data rate 1/8:  16 bits or  2 bytes. */
#define  VOC_4_BITS  40    /* Data rate 1/4:  40 bits or  5 bytes. */
#define  VOC_2_BITS  80    /* Data rate 1/2:  80 bits or 10 bytes. */
#define  VOC_1_BITS  171   /* Data rate 1  : 171 bits or 22 bytes. */

#define  VOC_BITS_SIZ  ((VOC_1_BITS + 5) / 8)

#define  VOC13K_8_BITS  20    /* Data rate 1/8:  20 bits or  3 bytes. */
#define  VOC13K_4_BITS  54    /* Data rate 1/4:  40 bits or  6 bytes. */
#define  VOC13K_2_BITS  124   /* Data rate 1/2: 124 bits or 15 bytes. */
#define  VOC13K_1_BITS  266   /* Data rate 1  : 266 bits or 34 bytes. */

#define  VOC13K_BITS_SIZ  ((VOC13K_1_BITS + 6) / 8)
  /* Specifies the number of bytes required to hold the vocoder bits at
     the highest data rates. */

#if defined(MSMAUD_VOC_AMR_COMMON)
#define  VOCAMR_475_BITS    42+53     /* Data rate 4.75  95 bits or 12 bytes. */
#define  VOCAMR_515_BITS    49+54     /* Data rate 5.15 103 bits or 13 bytes. */
#define  VOCAMR_590_BITS    55+63     /* Data rate 5.90 118 bits or 15 bytes. */
#define  VOCAMR_670_BITS    58+76     /* Data rate 6.70 134 bits or 17 bytes. */
#define  VOCAMR_740_BITS    61+87     /* Data rate 7.40 148 bits or 19 bytes. */
#define  VOCAMR_795_BITS    75+84     /* Data rate 7.95 159 bits or 20 bytes. */
#define  VOCAMR_1020_BITS   65+99+40  /* Data rate 10.2 204 bits or 26 bytes. */
#define  VOCAMR_1220_BITS   81+103+60 /* Data rate 12.2 244 bits or 31 bytes. */
#define  VOCAMR_GSM_FR_BITS 266       /* Data rate FR   266 bits or 34 bytes. */
#define  VOCAMR_SID_BITS    39        /* Data rate SID   39 bits or  5 bytes  */

#define  VOCAMR_BITS_SIZ  ((VOCAMR_1220_BITS + 7) / 8)
#define  VOCGSM_BITS_SIZ  ((VOCAMR_GSM_FR_BITS + 7) / 8)
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#ifdef FEATURE_VOICE_PLAYBACK

/* The vocoder driver maintains two voice play back queues, one for the 
** forward link and one for the reverse link.  Both queues have the same 
** number of entries.
*/
#define VSP_PBQ_MAX 3                /* Max voice play back queue entries  */

#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
/*===========================================================================

                 FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION VOCSUP_ENC_FORMAT_TO_RATE

DESCRIPTION
  Use this routine to translate the encoder format byte to the equivalent 
  voc_rate_type.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_rate_type vocsup_enc_format_to_rate (byte format);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_DEC_FORMAT_TO_RATE

DESCRIPTION
  Use this routine to translate the decoder format byte to the equivalent 
  voc_rate_type.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_rate_type vocsup_dec_format_to_rate (byte format);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_RATE_TO_SIZE

DESCRIPTION
  Use this routine to determine the length of the Decoder or Encoder packet
  based on the rate of the packet.

DEPENDENCIES
  None

RETURN VALUE
  The length in bytes of the packet not including the format byte.

SIDE EFFECTS
  None

===========================================================================*/
extern word vocsup_rate_to_size(byte rate, voc_capability_type so);

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR_COMMON)
/*===========================================================================

FUNCTION VOCSUP_AMR_TO_NATIVE_HEADER

DESCRIPTION
  Use this routine to translate the AMR voc_rate_type to native QCP format byte.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 vocsup_amr_to_native_header(
  voc_rate_type rate,
  voc_gsm_frame_type mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_AMR_NATIVE_HEADER_TO_AMR_RATE

DESCRIPTION
  Use this routine to translate the native QCP header byte to the equivalent 
  voc_rate_type.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_rate_type vocsup_native_header_to_amr_rate(
  byte header
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_AMR_NATIVE_HEADER_TO_AMR_MODE

DESCRIPTION
  Use this routine to translate the native QCP header byte to the equivalent 
  voc_gsm_frame_type.

DEPENDENCIES
  None

RETURN VALUE
  voc_gsm_frame_type value.

SIDE EFFECTS
  None

===========================================================================*/
extern voc_gsm_frame_type vocsup_native_header_to_amr_mode(
  byte header
);
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

/* <EJECT> */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_compute_freq

DESCRIPTION
  This function returns the vocoder tone value for a given frequency
  to be played by the vocoder.

DEPENDENCIES
  Requires vocsup_freq_values[].

RETURN VALUE
  Returns the a word containing the vocoder tone value.

SIDE EFFECTS
  None.
===========================================================================*/
extern word vocsup_compute_freq
(
  word frequency,
    /* Frequency in Hz for which vocoder tone values are to be computed */
  voc_sample_rate_type sample_rate
);
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION vocsup_pb_get_frame

DESCRIPTION
  This function is called to get the current frame to output in a voice
  play back session.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Pointer to frame data
  Frame rate
  Pointer to header
  Queue status

SIDE EFFECTS
  Update vsp_pbq_fwd or vsp_pbq_rev queue.

===========================================================================*/
typedef enum
{
  SAME_PROMPT,    
    /* normal return, got next frame of current prompt                     */          
  NEXT_PROMPT,    
    /* got last frame of current prompt, advance to next prompt in queue   */
  QUEUE_EMPTY
    /* got last frame of current prompt, no next prompt to play            */        
} vsp_get_frame_type;

extern vsp_get_frame_type vocsup_pb_get_frame 
  (
    byte *fr_rate,                           /* Frame rate                 */
    byte **fr_data,                          /* Ptr to frame data          */
    voc_pb_header_type **pb_hdr_ptr,         /* Ptr to current play header */
    voc_pb_dir_type play_direction           /* Play direction             */
  );

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_pb_q_size

DESCRIPTION
  This function returns number of entries in the specified play back queue.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Number of queued entries in vsp_pbq_fwd or vsp_pbq_rev queue.

SIDE EFFECTS
  None

===========================================================================*/

extern int vocsup_pb_q_size
  (
    /* Play direction */
    voc_pb_dir_type           play_direction
  );

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_pb_get_hdr

DESCRIPTION
  This function returns the pointer to the current entry of the specified 
  vocoder play back queue.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Pointer to current queue entry in vsp_pbq_fwd or vsp_pbq_rev queue.
  
SIDE EFFECTS
  None

===========================================================================*/

extern voc_pb_header_type *vocsup_pb_get_hdr 
  (
    /* Play direction */
    voc_pb_dir_type           play_direction
  );

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_pb_add_entry

DESCRIPTION
  This function adds one entry to the appropriate vocoder play back queue.  
  The vocoder driver maintains two queues, one for the forward link and one for
  the reverse link.  The play back queues are implemented as ring buffers.
 
  Each entry contains a voice utterance header.  The header contains information
  used by the vocoder driver to play a voice utterance.  

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Update vsp_pbq_fwd or vsp_pbq_rev queue.

  Disable interrupts while queue insert pointer is updated.  This variable
  is accessed in an ISR.

===========================================================================*/

extern void vocsup_pb_add_entry
  (
    /* Pointer to voice utterance header */
    voc_pb_header_type *voc_pb_header_ptr,

    /* Play direction */
    voc_pb_dir_type    play_direction
  );

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_pb_clear_q

DESCRIPTION
  This function clears the voice play back queues.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Clear vsp_pbq_fwd or vsp_pbq_rev queue or both.

===========================================================================*/

extern void vocsup_pb_clear_q 
  (
    /* Play direction */
    voc_pb_dir_type    play_direction
  );

#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
/*===========================================================================

FUNCTION vocsup_rec_clear_buffers

DESCRIPTION
  This function clears the recording buffer queue.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vocsup_rec_clear_buffers (void);

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_get_frame_buf

DESCRIPTION
  This function retrieves a buffer in order to save a recorded vocoder
  packet.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 *vocsup_rec_get_frame_buf ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_put_buf

DESCRIPTION
  This function accepts a buffer to be used for memo recording.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vocsup_rec_put_buf (
  uint8  *buffer,
  uint32 num_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_free_buf

DESCRIPTION
  This function resets recording buffers.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vocsup_rec_free_buf (
  voc_rec_result_type *rec_result
);

/*===========================================================================

FUNCTION vocsup_rec_reset_frm_count

DESCRIPTION
  This function resets the data request time stamp as needed.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vocsup_rec_reset_frm_count (void);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_FRAME

DESCRIPTION
  This function records one voice frame. This function copies the voice frame 
  to the record buffer without disturbing the frame itself.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Ptr to record result structure.

SIDE EFFECTS
  Update record buffer info.

===========================================================================*/
extern void vocsup_rec_frame (
  voc_rec_type        dir,
  byte                packet_format, 
  byte                *data_ptr,
  voc_rec_result_type *rec_result
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_START

DESCRIPTION
  This function starts the recording process at the frame data level.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Update record info.

===========================================================================*/
extern void vocsup_rec_start (
  /* Recording parameters. */
  voc_rec_parm_type          *voc_rec_parm
);

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_SYNC_UP

DESCRIPTION
  This function will return all buffered recorded frames in the current
  recording session.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Ptr to record result structure.

SIDE EFFECTS
  Update record buffer info.

===========================================================================*/
extern void vocsup_rec_sync_up ( voc_rec_result_type *rec_result );

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_CHANGE_CONFIG

DESCRIPTION
  This function is called when the vocoder driver detects that the
  configuration has changed while an in-call recording session is in progress.
  This function updates the configuration specified in the header of the 
  recorded buffers from this point on.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Update record info.

===========================================================================*/

extern void vocsup_rec_change_config (voc_capability_type new_config);


/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_DO_AV_SYNC

DESCRIPTION
  This function sends the recorded frame(s) information back to the 
	application. 

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void vocsup_rec_do_av_sync(void);


#endif /* FEATURE_VOICE_RECORD */

#endif /* VOCSUP_H */


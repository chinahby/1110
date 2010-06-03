/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Vocoder Support")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           V O C O D E R    S U P P O R T

GENERAL DESCRIPTION

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

Copyright (c) 1998 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocsup.c#6 $ $DateTime: 2009/01/07 03:08:46 $ $Author: subhashj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
08/29/08    av     Fixed lint errors.
08/12/08    sj     Fixed klokwork errors
06/17/08    sj     Made changes to function vocsup_rec_do_av_sync()
                   in order to avoid compiler specific code generation.
                   Issue found when comipling with RVCT compiler.
10/26/07    hs     Added EVW file format support
04/20/07    at     Added EVRC-B file format recording support.
04/02/07   ayu     Fixed AMR Recording and playback issue. 
03/21/07    at     Added EVRC-B file format support.
03/15/07    kan    Added 4GV-WB Code 
01/23/07    kk     Fixed issues related to adding 44k sample rate.
12/04/06   suk     Fixed 6260 ThinUI build compilation error.
11/10/06    az     Fixed the CR: 85787 and warnings for MM API support.
11/09/06    az     Merged the code for MM API changes.
09/19/06    kan    Modified the algo for SID First detection in AMR (A26101)
09/08/06    pl     Added 44K tone sample to tone_values_table.
02/03/06    sm     Added 4GV VOIP support
11/08/05    sm     audfmt_amr_header_info() returns SID_UPDATE now.
                   changed amr_rate_to_size() to support SPEECH_BAD/SID_BAD
08/25/05    hz     Fixed a typo in vocsup_rec_frame.
08/15/05   ymc     Modified code to define bad AMR frame size as 0 length.
                   CR#73065.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
04/28/05   ymc     Added QCP file support for 4GV vocoder.
           ymc     Fixed AMR frame size calculation in vocsup_rate_to_size().
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
01/30/04    sm     Only write SID_FIRST header in AMR file now.
10/10/03    aw     Added MSMAUD_VOC_AMR_COMMON featurization.
04/17/03    sm     Added AMR file format support.
03/12/03    st     Modified code to use 32-bit variable for INTLOCK_SAV().
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
11/18/02    st     Modified QCP playback/recording code to centralize format
                   specific differences; featurized GSM/WCDMA specific code.
11/06/02    sm     Added support for QCP AMR files.
08/13/02    st     Modified level of debug messages; removed featurization
                   of 11.025kHz and 12kHz in tone_values_table[].
03/12/02    st     Unfeaturized tone_values_table[] because it now is used for
                   wideband codec as well.
12/12/01    aw     Merged with MSM_AVS.01.05.17.
11/10/01    st     Removed obsolete reference to 80186 processor.
10/09/01    st     Modified code to record an eighth rate NULL in place of
                   erasures and blank frames.
09/16/01    st     Modified values in tone_values_table to not have 0 as any 
                   entry in order to compensate for a DTMF generator bug where
                   if the initial value is 0, no tone is played.
09/03/01    st     Redesigned vocoder packet recording feature to: accept
                   recording buffers from client instead of declaring buffers
                   locally, declare one recording header each for Rx and Tx
                   path, support for recording into QCP format, declare and
                   handle vocoder packet buffers, remove support for 
                   VOC_PB_FIX_LEN_FIX_RATE and VOC_PB_VAR_LEN_VAR_RATE format
                   recording, handle any recording callback interval setting,
                   remove mem_to_qcp support.
04/04/01    st     Added support for playback of Native QCP vocoder frames;
                   added featurization for FEATURE_MIDI_OUT feature; lint
                   updates.
12/13/00    st     Modified code to use correct parameter to index frequency
                   tables in vocsup_compute_freq() (replaced sample_rate
                   with sample_rate_index).
12/13/00    st     Fixed definition of VOC_FREQ_NUM_VALUES, this was causing
                   large tables to generated in ROM.
12/10/00    st     Added proper featurization to tone_values_table.
12/05/00   jkl     Replaced VOC_SAMPLE_RATE_48000 to VOC_SAMPLE_RATE_UNKNOWN-1.
12/04/00    sm     Added stereo DAC DTMF tables.
08/10/00    vl     Fixed bug in vocsup_rec_frame () which generated invalid 
                   erasure packets by calling incorrect function.
07/27/00    ro     Make 13k frame sizes compatible with QCP format.
                   Remove size parameter from vocsup_rec_frame().
                   Support for FEATURE_VM_MEM_TO_QCP.
07/14/00   spf     Removed #include <memory.h>.  Lint updates.
05/30/00    st     Added support for DTMF frequency calculation for 22kHz
                   sampling rate.
05/24/00    ro     Removed support for VOC_PB_DIR_DUPLICATE (CR13557).
01/26/00    tc     Added VR Mode A support.
07/29/99    vl     Removed VOC_ROM.
07/28/99    vl     Reset vsp_curr_pb_hdr when prompt is done playing.
07/23/99    vl     Removed reference to q_ptr.
07/21/99    vl     Implemented voice answering machine feature (playback on
                   reverse link and both links, record forward link).  Created
                   vocsup_enc_format_to_rate () and vocsup_dec_format_to_rate ()
                   so playback can accomodate erasure frames, blank frames and 
                   full rate error frames.  Fixed boundary condition bug in 
                   voc_count_frames ().  Made necessary changes to remove 
                   #include voc_core.h
05/19/99    vl     Added vocsup_validate_frame_control_byte ().  Call this
                   function in vocsup_pb_get_frame () when playing back voice
                   prompts and in voc_count_frames () when counting frames
                   for voice memo.
04/15/99    vl     Changed size and number of recording buffers, added code 
                   to report recording error only once.
04/07/99    vl     Changed 3 function names from vocsup_ to voc_ to avoid
                   translation.  Added support for shared buffer with VR 
                   engine.  Added #include customer.h.
03/04/99    st     Added vocsup_compute_freq() and required defines/tables.
03/04/99    vl     Implemented Voice Record to support Voice Memo (see 
                   FEATURE_VOICE_RECORD) plus misc clean up.
12/05/98    vl     Changed play back queue format and queue access software.
                   Added one more parameter to vocsup_pb_get_frame ().
                   Created function vocsup_pb_get_hdr ().
12/04/98    sm     Support selectable ROM keyword for backward 
                   compatibility.
08/31/98    vl     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "msmaud.h"             /* Target-specific audio features          */
#include "voc.h"                /* Vocoder interface definition            */
#include "vocsup.h"             /* Vocoder Support Routines                */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#include "assert.h"             /* Definitions for ASSERT                  */
#ifdef FEATURE_PHONE_VR
#include "vrengine.h"
#endif /* FEATURE_PHONE_VR */
#include "amrsup.h"
#ifdef FEATURE_MM_REC
#include "clk.h"
#endif /* FEATURE_MM_REC */
#include <string.h>             /* String/Memory manipulation routines     */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                 VOCODER VOICE PLAY BACK LOOK UP TABLE

---------------------------------------------------------------------------*/
/* Convert hardware rates to external rates (same as vc_rates [] in vocm2.c)
*/
LOCAL const voc_rate_type vocsup_rate_table[] = {
  VOC_1_RATE,                             /* Full rate    */
  VOC_2_RATE,                             /* Half rate    */
  VOC_4_RATE,                             /* Quarter rate */
  VOC_8_RATE                              /* Eighth rate  */
};

/* Type describing the size of a frame, indexed by rate. 
*/
LOCAL const byte vocsup_frame_sizes_8k [] = {
  0,                                      /* Blank        */
  ( (VOC_8_BITS + 7) / 8 ),               /* Eighth rate  */
  ( (VOC_4_BITS + 7) / 8 ),               /* Quarter rate */
  ( (VOC_2_BITS + 7) / 8 ),               /* Half rate    */
  ( (VOC_1_BITS + 7) / 8 ),               /* Full rate    */
  0,                                      /* Erasure      */
  ( (VOC_1_BITS + 7) / 8 )                /* Full-likely  */
};

LOCAL const byte vocsup_frame_sizes_13k [] = {
  0,             /* Blank        */
  3,             /* Eighth rate  */
  7,             /* Quarter rate */
  16,            /* Half rate    */
  34,            /* Full rate    */
  0,             /* Erasure      */
  34             /* Full-likely  */
};

#if defined(MSMAUD_VOC_AMR_COMMON)
/* Type describing the size of a frame, indexed by rate.
*/
LOCAL const byte vocsup_frame_sizes_amr [] = {
  ( (VOCAMR_475_BITS + 7) / 8 ),          /* 4.75 kbit /s  */
  ( (VOCAMR_515_BITS + 7) / 8 ),          /* 5.15 kbit /s  */
  ( (VOCAMR_590_BITS + 7) / 8 ),          /* 5.90 kbit /s  */
  ( (VOCAMR_670_BITS + 7) / 8 ),          /* 6.70 kbit /s  */
  ( (VOCAMR_740_BITS + 7) / 8 ),          /* 7.40 kbit /s  */
  ( (VOCAMR_795_BITS + 7) / 8 ),          /* 7.95 kbit /s  */
  ( (VOCAMR_1020_BITS + 7) / 8 ),         /* 10.20 kbit /s */
  ( (VOCAMR_1220_BITS + 7) / 8 )          /* 12.20 kbit /s */
};
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */

#ifdef FEATURE_AUDFMT_AMR
/* Tables to convert VOC to AMRSUP enums & vice versa
*/
LOCAL amrsup_frame_type voc_to_amrsup_frame_table[VOC_GSM_FRAME_MAX] =
{
  AMRSUP_FRAME_TYPE_MAX,     /* VOC_GSM_SID */
  AMRSUP_FRAME_TYPE_MAX,     /* VOC_GSM_SPEECH_GOOD */
  AMRSUP_FRAME_TYPE_MAX,     /* VOC_GSM_BFI */
  AMRSUP_FRAME_TYPE_MAX,     /* VOC_GSM_INVALID_SID */
  
  AMRSUP_SPEECH_GOOD,        /* VOC_AMR_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED,    /* VOC_AMR_SPEECH_DEGRADED */
  AMRSUP_ONSET,              /* VOC_AMR_ONSET */
  AMRSUP_SPEECH_BAD,         /* VOC_AMR_SPEECH_BAD */
  AMRSUP_SID_FIRST,          /* VOC_AMR_SID_FIRST */
  AMRSUP_SID_UPDATE,         /* VOC_AMR_SID_UPDATE */
  AMRSUP_SID_BAD,            /* VOC_AMR_SID_BAD */
  AMRSUP_NO_DATA             /* VOC_AMR_NO_DATA */
};
#define VOC_TO_AMRSUP_FRAME(x) voc_to_amrsup_frame_table[(x)]

LOCAL voc_gsm_frame_type amrsup_to_voc_amr_frame_table[AMRSUP_FRAME_TYPE_MAX] =
{
  VOC_AMR_SPEECH_GOOD,     /* AMRSUP_SPEECH_GOOD */
  VOC_AMR_SPEECH_DEGRADED, /* AMRSUP_SPEECH_DEGRADED */
  VOC_AMR_ONSET,           /* AMRSUP_ONSET */ 
  VOC_AMR_SPEECH_BAD,      /* AMRSUP_SPEECH_BAD */
  VOC_AMR_SID_FIRST,       /* AMRSUP_SID_FIRST */
  VOC_AMR_SID_UPDATE,      /* AMRSUP_SID_UPDATE */
  VOC_AMR_SID_BAD,         /* AMRSUP_SID_BAD */
  VOC_AMR_NO_DATA          /* AMRSUP_NO_DATA */
};
#define AMRSUP_TO_VOC_AMR_FRAME(x) amrsup_to_voc_amr_frame_table[(x)]

LOCAL amrsup_mode_type voc_to_amrsup_mode_table[VOC_GSM_RATE_MAX] =
{
  AMRSUP_MODE_0475,       /* VOC_AMR_RATE_475 */
  AMRSUP_MODE_0515,       /* VOC_AMR_RATE_515 */
  AMRSUP_MODE_0590,       /* VOC_AMR_RATE_590 */
  AMRSUP_MODE_0670,       /* VOC_AMR_RATE_670 */
  AMRSUP_MODE_0740,       /* VOC_AMR_RATE_740 */
  AMRSUP_MODE_0795,       /* VOC_AMR_RATE_795 */
  AMRSUP_MODE_1020,       /* VOC_AMR_RATE_1020 */
  AMRSUP_MODE_1220,       /* VOC_AMR_RATE_1220 */
  AMRSUP_MODE_MAX,        /* VOC_GSM_FULL_RATE */
  AMRSUP_MODE_MAX         /* VOC_GSM_HALF_RATE */
};
#define VOC_TO_AMRSUP_MODE(x) voc_to_amrsup_mode_table[(x)]

LOCAL voc_gsm_frame_rate_type amrsup_to_voc_amr_mode_table[AMRSUP_MODE_MAX] =
{
  VOC_AMR_RATE_475,        /* AMRSUP_MODE_0475 */
  VOC_AMR_RATE_515,        /* AMRSUP_MODE_0515 */
  VOC_AMR_RATE_590,        /* AMRSUP_MODE_0590 */
  VOC_AMR_RATE_670,        /* AMRSUP_MODE_0670 */
  VOC_AMR_RATE_740,        /* AMRSUP_MODE_0740 */
  VOC_AMR_RATE_795,        /* AMRSUP_MODE_0795 */
  VOC_AMR_RATE_1020,       /* AMRSUP_MODE_1020 */
  VOC_AMR_RATE_1220        /* AMRSUP_MODE_1220 */
};
#define AMRSUP_TO_VOC_AMR_MODE(x) amrsup_to_voc_amr_mode_table[(x)]

#endif  /* FEATURE_AUDFMT_AMR */

LOCAL voc_rate_type amrsup_to_voc_amr_wb_mode_table[AMRSUP_MODE_MAX] =
{
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_MAX,        /* VOC_AMR_WB_RATE_MAX */
  VOC_AMR_WB_RATE_0660,    /* 6.60 kbit /s                          */
  VOC_AMR_WB_RATE_0885,    /* 8.85 kbit /s                          */
  VOC_AMR_WB_RATE_1265,    /* 12.65 kbit /s                         */
  VOC_AMR_WB_RATE_1425,    /* 14.25 kbit /s                         */
  VOC_AMR_WB_RATE_1585,    /* 15.85 kbit /s                         */
  VOC_AMR_WB_RATE_1825,    /* 18.25 kbit /s                         */
  VOC_AMR_WB_RATE_1985,    /* 19.85 kbit /s                         */
  VOC_AMR_WB_RATE_2305,    /* 23.05 kbit /s                         */
  VOC_AMR_WB_RATE_2385    /* 23.85 kbit /s                         */
};
#define AMRSUP_TO_VOC_AMR_WB_MODE(x) amrsup_to_voc_amr_wb_mode_table[(x)]


#ifdef FEATURE_VOICE_PLAYBACK
/* Erasure packet.
*/
LOCAL byte vocsup_erasure_packet[2] =
{
  0xff, 0xff
};
#endif

/* Maximum and Minimum frequencies that the vocoder can generate.
*/
#define VOC_FREQ_MIN 100             /* minimum vocoder freq at 100  Hz */
#define VOC_FREQ_MAX 4000            /* maximum vocoder freq at 4000 Hz */

#define VOC_FREQ_STEP     50    /* Interval between successive freqs    */

#define VOC_FREQ_NUM_VALUES (((VOC_FREQ_MAX - VOC_FREQ_MIN) / VOC_FREQ_STEP) +1)

/* This table contains precomputed vocoder tone values, along with the
** frequency associated with the value, for use with linear interpolation.
** The index into this table is computed by the following equation:
**         index = floor((freq-VOC_FREQ_MIN) / VOC_FREQ_STEP)
** Interpolation then takes the index and index+1 values from this table
** to find the gradient and compute a frequency's vocoder tone value.
** The table is based on sample rate, and frequency.
*/
const uint16 tone_values_table[VOC_SAMPLE_RATE_UNKNOWN - 1]
                              [VOC_FREQ_NUM_VALUES] = {
  /* 8k sampling rate */
  {16333, 16270, 16182, 16069, 15931, 15769, 15582, 15371, 15137, 14879, 14598,
   14295, 13970, 13623, 13255, 12867, 12458, 12031, 11585, 11121, 10641, 10143,
    9630,  9102,  8561,  8006,  7438,  6859,  6270,  5671,  5063,  4447,  3825,
    3196,  2563,  1926,  1285,   643,     1, 64893, 64251, 63610, 62973, 62340,
   61711, 61089, 60473, 59865, 59266, 58677, 58098, 57530, 56975, 56434, 55906,
   55393, 54895, 54415, 53951, 53505, 53078, 52669, 52281, 51913, 51566, 51241,
   50938, 50657, 50399, 50165, 49954, 49767, 49605, 49467, 49354, 49266, 49203,
   49165, 49152},
  /* 11k sampling rate */
  {16357, 16324, 16278, 16218, 16145, 16059, 15960, 15848, 15723, 15586, 15435,
   15273, 15097, 14910, 14710, 14499, 14276, 14041, 13795, 13537, 13269, 12989,
   12700, 12400, 12089, 11769, 11440, 11101, 10753, 10397, 10032,  9659,  9278,
    8889,  8493,  8091,  7682,  7266,  6845,  6418,  5986,  5549,  5107,  4662,
    4212,  3759,  3304,  2845,  2384,  1921,  1457,   991,   525,    58, 65128,
   64661, 64195, 63731, 63267, 62806, 62347, 61890, 61437, 60986, 60540, 60097,
   59659, 59226, 58797, 58375, 57958, 57547, 57143, 56745, 56355, 55972, 55597,
   55230, 54871},
  /* 12k sampling rate */
  {16362, 16333, 16294, 16244, 16182, 16110, 16026, 15931, 15826, 15709, 15582,
   15444, 15296, 15137, 14968, 14788, 14598, 14399, 14189, 13970, 13741, 13502,
   13255, 12998, 12733, 12458, 12176, 11885, 11585, 11278, 10963, 10641, 10311,
    9974,  9630,  9280,  8923,  8561,  8192,  7818,  7438,  7053,  6664,  6270,
    5872,  5469,  5063,  4653,  4240,  3825,  3406,  2986,  2563,  2139,  1713,
    1285,   857,   429,     1, 65107, 64679, 64251, 63823, 63397, 62973, 62550,
   62130, 61711, 61296, 60883, 60473, 60067, 59664, 59266, 58872, 58483, 58098,
   57718, 57344},
  /* 16k sampling rate */
  {16371, 16356, 16333, 16305, 16270, 16229, 16182, 16129, 16069, 16003, 15931,
   15853, 15769, 15679, 15582, 15480, 15371, 15257, 15137, 15011, 14879, 14741,
   14598, 14449, 14295, 14135, 13970, 13799, 13623, 13441, 13255, 13063, 12867,
   12665, 12458, 12247, 12031, 11810, 11585, 11356, 11121, 10883, 10641, 10394,
   10143,  9889,  9630,  9368,  9102,  8833,  8561,  8285,  8006,  7723,  7438,
    7150,  6859,  6566,  6270,  5971,  5671,  5368,  5063,  4756,  4447,  4137,
    3825,  3511,  3196,  2880,  2563,  2245,  1926,  1606,  1285,   965,   643,
     322,     0},
  /* 22k sampling rate */
  {16377, 16369, 16357, 16342, 16324, 16303, 16278, 16249, 16218, 16183, 16145,
   16104, 16059, 16011, 15960, 15906, 15848, 15787, 15723, 15656, 15586, 15512,
   15435, 15356, 15273, 15187, 15097, 15005, 14910, 14812, 14710, 14606, 14499,
   14389, 14276, 14160, 14041, 13919, 13795, 13667, 13537, 13404, 13269, 13130,
   12989, 12846, 12700, 12551, 12400, 12246, 12089, 11931, 11769, 11606, 11440,
   11272, 11101, 10928, 10753, 10576, 10397, 10215, 10032,  9846,  9659,  9469,
    9278,  9084,  8889,  8692,  8493,  8293,  8091,  7887,  7682,  7475,  7266,
    7056,  6845},
  /* 24k sampling rate */
  {16378, 16371, 16362, 16349, 16333, 16315, 16294, 16270, 16244, 16214, 16182,
   16147, 16110, 16069, 16026, 15980, 15931, 15880, 15826, 15769, 15709, 15647,
   15582, 15515, 15444, 15371, 15296, 15218, 15137, 15053, 14968, 14879, 14788,
   14694, 14598, 14500, 14399, 14295, 14189, 14081, 13970, 13856, 13741, 13623,
   13502, 13380, 13255, 13128, 12998, 12867, 12733, 12597, 12458, 12318, 12176,
   12031, 11885, 11736, 11585, 11433, 11278, 11121, 10963, 10803, 10641, 10477,
   10311, 10143,  9974,  9803,  9630,  9456,  9280,  9102,  8923,  8743,  8561,
    8377,  8192},
  /* 32k sampling rate */
  {16381, 16377, 16371, 16364, 16356, 16345, 16333, 16320, 16305, 16289, 16270,
   16251, 16229, 16207, 16182, 16156, 16129, 16100, 16069, 16037, 16003, 15968,
   15931, 15893, 15853, 15812, 15769, 15724, 15679, 15631, 15582, 15532, 15480,
   15426, 15371, 15315, 15257, 15198, 15137, 15075, 15011, 14946, 14879, 14811,
   14741, 14671, 14598, 14525, 14449, 14373, 14295, 14216, 14135, 14053, 13970,
   13885, 13799, 13712, 13623, 13533, 13441, 13349, 13255, 13160, 13063, 12966,
   12867, 12766, 12665, 12562, 12458, 12353, 12247, 12140, 12031, 11921, 11810,
   11698, 11585},
   /* 44.1k sampling rate */
  {16382, 16380, 16377, 16374, 16369, 16364, 16357, 16350, 16342, 16334, 16324,
   16314, 16303, 16291, 16278, 16264, 16249, 16234, 16218, 16201, 16183, 16165,
   16145, 16125, 16104, 16082, 16059, 16036, 16011, 15986, 15960, 15933, 15906,
   15877, 15848, 15818, 15787, 15756, 15723, 15690, 15656, 15621, 15586, 15549,
   15512, 15474, 15435, 15396, 15356, 15315, 15273, 15230, 15187, 15142, 15097,
   15052, 15005, 14958, 14910, 14861, 14812, 14761, 14710, 14659, 14606, 14553,
   14499, 14444, 14389, 14333, 14276, 14218, 14160, 14101, 14041, 13980, 13919,
   13857, 13795},
  /* 48k sampling rate */
  {16383, 16381, 16378, 16375, 16371, 16367, 16362, 16356, 16349, 16342, 16333,
   16325, 16315, 16305, 16294, 16283, 16270, 16257, 16244, 16229, 16214, 16199,
   16182, 16165, 16147, 16129, 16110, 16090, 16069, 16048, 16026, 16003, 15980,
   15956, 15931, 15906, 15880, 15853, 15826, 15798, 15769, 15739, 15709, 15679,
   15647, 15615, 15582, 15549, 15515, 15480, 15444, 15408, 15371, 15334, 15296,
   15257, 15218, 15178, 15137, 15095, 15053, 15011, 14968, 14924, 14879, 14834,
   14788, 14741, 14694, 14647, 14598, 14549, 14500, 14449, 14399, 14347, 14295,
   14242, 14189}
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
   /* 44k sampling rate */
   ,{16382, 16380, 16377, 16374, 16369, 16364, 16357, 16350, 16342, 16333, 16324,
   16313, 16302, 16290, 16277, 16263, 16249, 16233, 16217, 16200, 16182, 16164,
   16144, 16124, 16102, 16080, 16058, 16034, 16010, 15984, 15958, 15931, 15904,
   15875, 15846, 15816, 15785, 15753, 15720, 15687, 15653, 15618, 15582, 15546,
   15508, 15470, 15431, 15391, 15351, 15310, 15268, 15225, 15181, 15137, 15092,
   15046, 14999, 14952, 14903, 14854, 14805, 14754, 14703, 14651, 14598, 14545,
   14491, 14436, 14380, 14323, 14266, 14208, 14150, 14090, 14030, 13970, 13908,
   13846, 13783}
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
};

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*---------------------------------------------------------------------------
                 VOCODER VOICE PLAY BACK QUEUE 

The following variables and definitions are used by the vocoder driver 
to manage two voice play back queues, one for the forward link and one for the
reverse link.

---------------------------------------------------------------------------*/

/* Each entry of the play back queues is a copy of the header, not the header
** itself because the frame number is decremented as the utterance is being 
** played, thus the frame number is 0 by the time the utterance is finished.
** Since the multi play counts on the fact that the utterance can be replayed,
** this problem is solved by creating the vsp_pb_header_type struct.  We are
** essentially queueing a copy of the header and keeping with it the pointer
** to the real header.  This structure is local to vocsup.
*/
typedef struct 
{
  voc_pb_header_type pbhdr;
  voc_pb_header_type *pbptr;

} vsp_pb_header_type;

typedef struct 
{
  int insert_idx;
    /* Voice play back queue insert index */
  int remove_idx;
    /* Voice play back queue remove index */
  int size;
    /* Number of entries in queue */        
  vsp_pb_header_type queue [VSP_PBQ_MAX];
    /* Voice play back queue (ring buffer) */

} vsp_pbq_type;

LOCAL vsp_pbq_type vsp_pbq_fwd;
LOCAL vsp_pbq_type vsp_pbq_rev;

/* Address of utterance header currently playing.
*/
LOCAL voc_pb_header_type *vsp_curr_pb_hdr = (voc_pb_header_type *) NULL;
#endif /* FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#ifdef FEATURE_PHONE_VR 
typedef union 
{
  /* Voice Recognition shared memory area.
  */
  vre_data_buffer_type vre_shared_buf;
} vsp_vre_shared_type;
   
/* Array of buffers used in voice recording.
*/
LOCAL vsp_vre_shared_type vsp_shared_mem;

#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_RECORD
/* Call back interval for voice recording in frames (20 ms frame).
*/
LOCAL word vsp_interval_frm = 0;
LOCAL word vsp_current_frm  = 0;

/* Reverse (tx) and forward (rx) link record buffer headers.
*/
LOCAL voc_rec_header_type vsp_rec_tx_hdr;
LOCAL voc_rec_header_type vsp_rec_rx_hdr;

/* Recording parameters. 
*/
LOCAL voc_rec_parm_type   vsp_rec_parm;

/* Flag to indicate error has been reported.
*/
LOCAL boolean             vsp_error_reported = FALSE;

/* Recoded bytes threshold to indicate whether buffer needs to be replied
 */
LOCAL uint32              vsp_rec_req_length = 0;

#ifdef FEATURE_MM_REC
/* How long - in terms of milisecond - of one speach frame during recording */
#define VSP_REC_FRAME_INTERVAL 20

/* How many samples every frame */
#define VSP_REC_FRAME_SAMPLES  160

/* Recoded frame count to indicate whether buffer needs to be replied
   Every how many frames should turn in recoded buffer
 */
LOCAL uint32              vsp_rec_data_req_frm_cnt  = 0;

/* Counter to keep track how many frames recorded in this session.
 */
LOCAL uint32              vsp_rec_data_req_cnt1 = 0;
LOCAL uint32              vsp_rec_data_req_cnt2 = 0;

/* Flag to indicate whether this is the beginning of record or RESUME after 
 * PASUE */
LOCAL boolean             vsp_rec_av_sync_stamp = FALSE;

/* Frame index recoded
 */
LOCAL uint32              vsp_rec_data_frame_index = 0;
#endif /* FEATURE_MM_REC */

/* The following structures are used as the recording buffer queue. The
** vocoder driver depends on the client to supply buffers for recording.
*/
#define VSP_REC_BUF_MAX 5
typedef struct {
  uint8  *buffer;       /* Pointer to recording buffer       */
  uint32 num_bytes;     /* Size of recording buffer in bytes */ 
} vsp_rec_buf_struct_type;

typedef struct {
  vsp_rec_buf_struct_type rec_buf[VSP_REC_BUF_MAX];  /* Recording buffers */
  uint32                  number;           /* Number of buffers in queue */
  uint32                  index;                /* Current index of queue */
} vsp_rec_buf_array_type;

vsp_rec_buf_array_type vsp_rec_buf_array;

/* The following structures and defines are used to buffer up recorded
** frames from the QDSP driver layer.
*/
#define VSP_REC_FRAME_NUM_BUFFERS 6
typedef struct {
  uint32   num_in_use;
  uint32   index;
  uint8    packet_buf[VSP_REC_FRAME_NUM_BUFFERS][34];
} vsp_rec_frame_buf_type;

vsp_rec_frame_buf_type vsp_rec_frame_buf = {0,0};
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
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
voc_rate_type vocsup_enc_format_to_rate (byte format)
{

  /* Figure out the rate from the format byte */
  return (vocsup_rate_table [format & VSP_FMT_RATE_M]);
} /* end vocsup_enc_format_to_rate */

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
voc_rate_type vocsup_dec_format_to_rate (byte format)
{

  /* Figure out the rate from the format byte */
  if ((format & VSP_DEC_FULL_LIKELY_M) == VSP_DEC_FULL_LIKELY_V)
  {
    return (VOC_ERR_RATE);
  }
  else if ((format & VSP_DEC_ERASE_M) == VSP_DEC_ERASE_V)
  {
    return (VOC_ERASURE);
  }
  else if ((format & VSP_DEC_BLANK_M) == VSP_DEC_BLANK_V)
  {
    return (VOC_0_RATE);
  }
  else 
  {
    return (vocsup_rate_table [format & VSP_FMT_RATE_M]);
  }
} /* end vocsup_dec_format_to_rate */

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_VOC_TO_NATIVE_FORMAT

DESCRIPTION
  Use this routine to translate the vocoder format into the native QCP
  format.

DEPENDENCIES
  None

RETURN VALUE
  Native format value.

SIDE EFFECTS
  None

===========================================================================*/
byte vocsup_voc_to_native_format (
  byte                format,
  voc_capability_type so
) {
  byte ret_val;

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(so == VOC_CAP_AMR) {
    ret_val = format;
  } else
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
  {
    switch (format) {
      case VOC_0_RATE:
        ret_val = VSP_NATIVE_QCP_BLANK;
        break;

      case VOC_8_RATE:
        ret_val = VSP_NATIVE_QCP_EIGHTH;
        break;

      case VOC_4_RATE:
        ret_val = VSP_NATIVE_QCP_QUARTER;
        break;

      case VOC_2_RATE:
        ret_val = VSP_NATIVE_QCP_HALF;
        break;

      case VOC_1_RATE:
        ret_val = VSP_NATIVE_QCP_FULL;
        break;

      case VOC_ERASURE:
        ret_val = VSP_NATIVE_QCP_ERASURE;
        break;

      default:
        ret_val = VSP_NATIVE_QCP_ERASURE;
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_NATIVE_TO_VOC_FORMAT

DESCRIPTION
  Use this routine to translate the native QCP format byte to the equivalent 
  vocoder driver format.

DEPENDENCIES
  None

RETURN VALUE
  vocoder frame value.

SIDE EFFECTS
  None

===========================================================================*/
byte vocsup_native_to_voc_format (
  byte                format,
  voc_capability_type so
) {
  voc_rate_type ret_val;

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(so == VOC_CAP_AMR) {
    ret_val = format;
  } else 
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
  {
    switch (format) {
      case VSP_NATIVE_QCP_BLANK:
        ret_val = VOC_0_RATE;
        break;

      case VSP_NATIVE_QCP_EIGHTH:
        ret_val = VOC_8_RATE;
        break;

      case VSP_NATIVE_QCP_QUARTER:
        ret_val = VOC_4_RATE;
        break;

      case VSP_NATIVE_QCP_HALF:
        ret_val = VOC_2_RATE;
        break;

      case VSP_NATIVE_QCP_FULL:
        ret_val = VOC_1_RATE;
        break;

      case VSP_NATIVE_QCP_ERASURE:
        ret_val = VOC_ERASURE;
        break;

      default:
        ret_val = VOC_ERR_RATE;
        break;
    }
  }

  return(ret_val);
}

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
uint8 vocsup_amr_to_native_header (voc_rate_type rate, voc_gsm_frame_type mode)
{
  uint8 ret_val;

  /* Convert rate byte */
  switch (rate) {
    case VOC_AMR_RATE_475:
      ret_val = VSP_NATIVE_AMR_RATE_475;
      break;
    case VOC_AMR_RATE_515:
      ret_val = VSP_NATIVE_AMR_RATE_515;
      break;
    case VOC_AMR_RATE_590:
      ret_val = VSP_NATIVE_AMR_RATE_590;
      break;
    case VOC_AMR_RATE_670:
      ret_val = VSP_NATIVE_AMR_RATE_670;
      break;
    case VOC_AMR_RATE_740:
      ret_val = VSP_NATIVE_AMR_RATE_740;
      break;
    case VOC_AMR_RATE_795:
      ret_val = VSP_NATIVE_AMR_RATE_795;
      break;
    case VOC_AMR_RATE_1020:
      ret_val = VSP_NATIVE_AMR_RATE_1020;
      break;
    case VOC_AMR_RATE_1220:
      ret_val = VSP_NATIVE_AMR_RATE_1220;
      break;

    default:
      ret_val = VSP_NATIVE_AMR_RATE_1220;
      break;
  }

  /* Convert mode byte */
  switch (mode) {
    case VOC_AMR_SPEECH_GOOD:
      ret_val |= VSP_NATIVE_AMR_SPEECH_GOOD;
      break;
    case VOC_AMR_SPEECH_DEGRADED:
      ret_val |= VSP_NATIVE_AMR_SPEECH_DEGRADED;
      break;
    case VOC_AMR_ONSET:
      ret_val |= VSP_NATIVE_AMR_ONSET;
      break;
    case VOC_AMR_SPEECH_BAD:
      ret_val |= VSP_NATIVE_AMR_SPEECH_BAD;
      break;
    case VOC_AMR_SID_FIRST:
      ret_val |= VSP_NATIVE_AMR_SID_FIRST;
      break;
    case VOC_AMR_SID_UPDATE:
      ret_val |= VSP_NATIVE_AMR_SID_UPDATE;
      break;
    case VOC_AMR_SID_BAD:
      ret_val |= VSP_NATIVE_AMR_SID_BAD;
      break;
    case VOC_AMR_NO_DATA:
      ret_val |= VSP_NATIVE_AMR_NO_DATA;
      break;

    default:
      ret_val |= VSP_NATIVE_AMR_NO_DATA;
      break;
  }

  return(ret_val);
}

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
voc_rate_type vocsup_native_header_to_amr_rate (byte header)
{
  voc_rate_type ret_val;

  switch (header & VSP_NATIVE_AMR_RATE_M) {
    case VSP_NATIVE_AMR_RATE_475:
      ret_val = VOC_AMR_RATE_475;
      break;
    case VSP_NATIVE_AMR_RATE_515:
      ret_val = VOC_AMR_RATE_515;
      break;
    case VSP_NATIVE_AMR_RATE_590:
      ret_val = VOC_AMR_RATE_590;
      break;
    case VSP_NATIVE_AMR_RATE_670:
      ret_val = VOC_AMR_RATE_670;
      break;
    case VSP_NATIVE_AMR_RATE_740:
      ret_val = VOC_AMR_RATE_740;
      break;
    case VSP_NATIVE_AMR_RATE_795:
      ret_val = VOC_AMR_RATE_795;
      break;
    case VSP_NATIVE_AMR_RATE_1020:
      ret_val = VOC_AMR_RATE_1020;
      break;
    case VSP_NATIVE_AMR_RATE_1220:
      ret_val = VOC_AMR_RATE_1220;
      break;
  
    default:
      ret_val = VOC_AMR_RATE_1220;
      break;
  }

  return(ret_val);
}

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
voc_gsm_frame_type vocsup_native_header_to_amr_mode (byte header)
{
  voc_gsm_frame_type ret_val;

  switch (header & VSP_NATIVE_AMR_MODE_M) {
    case VSP_NATIVE_AMR_SPEECH_GOOD:
      ret_val = VOC_AMR_SPEECH_GOOD;
      break;
    case VSP_NATIVE_AMR_SPEECH_DEGRADED:
      ret_val = VOC_AMR_SPEECH_DEGRADED;
      break;
    case VSP_NATIVE_AMR_ONSET:
      ret_val = VOC_AMR_ONSET;
      break;
    case VSP_NATIVE_AMR_SPEECH_BAD:
      ret_val = VOC_AMR_SPEECH_BAD;
      break;
    case VSP_NATIVE_AMR_SID_FIRST:
      ret_val = VOC_AMR_SID_FIRST;
      break;
    case VSP_NATIVE_AMR_SID_UPDATE:
      ret_val = VOC_AMR_SID_UPDATE;
      break;
    case VSP_NATIVE_AMR_SID_BAD:
      ret_val = VOC_AMR_SID_BAD;
      break;
    case VSP_NATIVE_AMR_NO_DATA:
      ret_val = VOC_AMR_NO_DATA;
      break;
  
    default:
      ret_val = VOC_AMR_NO_DATA;
      break;
  }

  return(ret_val);
}
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

/* <EJECT> */
#ifdef FEATURE_AUDFMT_AMR
/*===========================================================================

FUNCTION VOCSUP_BUILD_AUDFMT_AMR_HEADER

DESCRIPTION
  Use this routine to translate the AMR voc_rate_type to native QCP format byte.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
uint8 vocsup_build_audfmt_amr_header (
  voc_rate_type       rate,
  voc_gsm_frame_type  mode
) {
  uint8 ret_val;
  boolean quality = TRUE; /* Assume good quality */

  /* Convert mode byte */
  switch (mode) {
    case VOC_AMR_SPEECH_DEGRADED:
    case VOC_AMR_SPEECH_BAD:
      quality = FALSE;
      /* Fall through */
    case VOC_AMR_ONSET:
    case VOC_AMR_SPEECH_GOOD:
      /* Convert rate byte */
      switch (rate) {
        case VOC_AMR_RATE_475:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0475;
          break;
        case VOC_AMR_RATE_515:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0515;
          break;
        case VOC_AMR_RATE_590:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0590;
          break;
        case VOC_AMR_RATE_670:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0670;
          break;
        case VOC_AMR_RATE_740:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0740;
          break;
        case VOC_AMR_RATE_795:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_0795;
          break;
        case VOC_AMR_RATE_1020:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_1020;
          break;
        case VOC_AMR_RATE_1220:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_1220;
          break;

        default:
          ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_NO_DATA;
          break;
      }
      break;

    case VOC_AMR_SID_BAD:
      quality = FALSE;
      /* Fall through */
    case VOC_AMR_SID_FIRST:
    case VOC_AMR_SID_UPDATE:
      ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_AMR_SID;
      break;
    
    case VOC_AMR_NO_DATA:
    default:
      ret_val = VSP_AUDFMT_AMR_FRAME_TYPE_NO_DATA;
      break;
  }

  /* Build AUDFMT AMR header */
  ret_val <<= VSP_AUDFMT_AMR_FRAME_TYPE_SHIFT;
  if(quality == TRUE) {
    ret_val |= VSP_AUDFMT_AMR_FRAME_QUALITY_MASK;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_extract_audfmt_amr_header_info

DESCRIPTION
  Use this routine to translate the AMR header byte to the equivalent 
  voc_rate_type and voc_gsm_frame_type.

DEPENDENCIES
  None

RETURN VALUE
  voc_rate_type value.

SIDE EFFECTS
  None

===========================================================================*/
void vocsup_extract_audfmt_amr_header_info(
  byte                *voc_packet,
  voc_rate_type       *amr_rate,
  voc_gsm_frame_type  *amr_ftype
) {
  byte amr_ft_sid;
  boolean quality;
  byte header = *voc_packet; 
  byte amr_ft = ((header & VSP_AUDFMT_AMR_FRAME_TYPE_MASK) >>
                                               VSP_AUDFMT_AMR_FRAME_TYPE_SHIFT);

  /* Set quality flag */
  if((header & VSP_AUDFMT_AMR_FRAME_QUALITY_MASK) ==
                                            VSP_AUDFMT_AMR_FRAME_QUALITY_MASK) {
    quality = TRUE;
  } else {
    quality = FALSE;
  }

  /* Check the AMR frame type nibble */
  switch (amr_ft) {
    case VSP_AUDFMT_AMR_FRAME_TYPE_0475:
      *amr_rate = VOC_AMR_RATE_475;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_0515:
      *amr_rate = VOC_AMR_RATE_515;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_0590:
      *amr_rate = VOC_AMR_RATE_590;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_0670:
      *amr_rate = VOC_AMR_RATE_670;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_0740:
      *amr_rate = VOC_AMR_RATE_740;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_0795:
      *amr_rate = VOC_AMR_RATE_795;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_1020:
      *amr_rate = VOC_AMR_RATE_1020;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_1220:
      *amr_rate = VOC_AMR_RATE_1220;
      break;
    case VSP_AUDFMT_AMR_FRAME_TYPE_AMR_SID:
      amr_ft_sid = (((voc_packet[5]<<1)&0x04)|((voc_packet[5]>>1)&0x02)
                                               |((voc_packet[5]>>3)&0x01)) ;
      switch(amr_ft_sid){
        case VSP_AUDFMT_AMR_FRAME_TYPE_0475:
          *amr_rate = VOC_AMR_RATE_475;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_0515:
          *amr_rate = VOC_AMR_RATE_515;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_0590:
          *amr_rate = VOC_AMR_RATE_590;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_0670:
          *amr_rate = VOC_AMR_RATE_670;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_0740:
          *amr_rate = VOC_AMR_RATE_740;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_0795:
          *amr_rate = VOC_AMR_RATE_795;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_1020:
          *amr_rate = VOC_AMR_RATE_1020;
          break;
        case VSP_AUDFMT_AMR_FRAME_TYPE_1220:
        default:  
          *amr_rate = VOC_AMR_RATE_1220;
          break;
      }
      break;
    default:
      *amr_rate = VOC_AMR_RATE_1220;
      break;
  }

  /* Generate the ftype value from the quality + ft nibble */
  if(amr_ft <= VSP_AUDFMT_AMR_FRAME_TYPE_1220) {
    /* Speech frame, check quality */
    if(quality == TRUE) {
      *amr_ftype = VOC_AMR_SPEECH_GOOD;
    } else {
      *amr_ftype = VOC_AMR_SPEECH_BAD;
    }
  } else {
    switch(amr_ft) {
      case VSP_AUDFMT_AMR_FRAME_TYPE_AMR_SID:
        if(quality == TRUE) {
          if (voc_packet[5] & 0x10)  /*check the bit 8+35 in voc_packet  */
            *amr_ftype = VOC_AMR_SID_UPDATE;
          else
            *amr_ftype = VOC_AMR_SID_FIRST;
        } else {
          *amr_ftype = VOC_AMR_SID_BAD;
        }
        break;
      case VSP_AUDFMT_AMR_FRAME_TYPE_NO_DATA:
      default:
        *amr_ftype = VOC_AMR_NO_DATA;
        break;
    }
  }
}
#endif /* FEATURE_AUDFMT_AMR */

#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */

/* <EJECT> */
#if defined(MSMAUD_VOC_AMR_COMMON)
/*===========================================================================

FUNCTION VOCSUP_AMR_RATE_TO_SIZE

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
word vocsup_amr_rate_to_size(
  voc_gsm_frame_type   mode, 
  voc_rate_type        rate,
  voc_capability_type  so
)
{
  word size=0;


  if (so != VOC_CAP_AMR)
  {
    MSG_ERROR("Non-AMR service option %d", so, 0, 0);
    return 0;
  }

  /* Avoid out of bound array access. we support 8 different AMR frame rates */
  if(rate >=  (sizeof(vocsup_frame_sizes_amr) / sizeof(vocsup_frame_sizes_amr[0])))
  {
	MSG_ERROR("Invalid vocoder framerate: %d", rate, 0, 0);
	return 0;
  }

  switch (mode)
  {
    case VOC_AMR_SPEECH_GOOD :
    case VOC_AMR_SPEECH_DEGRADED :
    case VOC_AMR_ONSET :
    case VOC_AMR_SPEECH_BAD :
      size = vocsup_frame_sizes_amr[rate];
      break;

    case VOC_AMR_SID_FIRST :
    case VOC_AMR_SID_UPDATE :
    case VOC_AMR_SID_BAD :
      size = (VOCAMR_SID_BITS + 7) / 8;
      break;

    case VOC_AMR_NO_DATA :
      size = 0;
      break;

    default :
      size = 0;
      MSG_ERROR("Unknown AMR frame type %x", mode, 0, 0);
      break;
  }


  return size;
}
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#ifdef MSMAUD_VOC_AMR_WB_COMMON
/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_amr_to_voc_amr_wb_ftype

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_amr_wb_frame_type vocsup_amrsup_to_voc_amrwb_ftype(amrsup_frame_type amr_ftype)
{
  voc_amr_wb_frame_type ret_val;

  switch(amr_ftype)
  {
    case AMRSUP_SPEECH_GOOD:             /* Good speech frame              */
      ret_val = VOC_AMR_WB_SPEECH_GOOD;
      break;
    case AMRSUP_SPEECH_DEGRADED:         /* Speech degraded                */
      ret_val = VOC_AMR_WB_SPEECH_DEGRADED;
      break;
    case AMRSUP_ONSET:                   /* onset                          */
      ret_val = VOC_AMR_WB_ONSET;
      break;
    case AMRSUP_SPEECH_BAD:              /* Corrupt speech frame (bad CRC) */
      ret_val = VOC_AMR_WB_SPEECH_BAD;
      break;
    case AMRSUP_SID_FIRST:               /* First silence descriptor       */
      ret_val = VOC_AMR_WB_SID_FIRST;
      break;
    case AMRSUP_SID_UPDATE:              /* Comfort noise frame            */
      ret_val = VOC_AMR_WB_SID_UPDATE;
      break;
    case AMRSUP_SID_BAD:                 /* Corrupt SID frame (bad CRC)    */
      ret_val = VOC_AMR_WB_SID_BAD;
      break;
    case AMRSUP_NO_DATA:                 /* Nothing to transmit            */
      ret_val = VOC_AMR_WB_NO_DATA;
      break;
    case AMRSUP_SPEECH_LOST:             /* Lost speech in downlink        */
      ret_val = VOC_AMR_WB_SPEECH_LOST;
      break;
    default:
      MSG_ERROR("Invalid AMR-WB frametype = %d", (uint8)amr_ftype, 0, 0);
      ret_val = VOC_AMR_WB_FRAME_MAX;
      break;
  }

  return (ret_val);
}
#endif /* MSMAUD_VOC_AMR_WB_COMMON */

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
word vocsup_rate_to_size (
  byte rate,
  voc_capability_type so
) {
#if defined(MSMAUD_VOC_AMR_COMMON)
  voc_gsm_frame_type   mode;
  voc_rate_type        amr_rate;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

  if (so == VOC_CAP_IS733)
  {
	/* Avoid out of bound array access */
	if(rate >=  (sizeof(vocsup_frame_sizes_13k) / sizeof(vocsup_frame_sizes_13k[0])))
	{
	  MSG_ERROR("Invalid vocoder framerate: %d", rate, 0, 0);
	  return 0;
	}
    return(vocsup_frame_sizes_13k[rate]);

  } 
  else if (so == VOC_CAP_IS127)
  {
	/* Avoid out of bound array access */
	if(rate >=  (sizeof(vocsup_frame_sizes_8k) / sizeof(vocsup_frame_sizes_8k[0])))
	{ 
	  MSG_ERROR("Invalid vocoder framerate: %d", rate, 0, 0);
	  return 0;
	}
    return(vocsup_frame_sizes_8k[rate]);
  }
#if defined(MSMAUD_VOC_4GV) || defined(MSMAUD_VOC_4GV_WB)
  else if (so == VOC_CAP_4GV_NB || so == VOC_CAP_4GV_NB_VOIP 
                                || so == VOC_CAP_4GV_WB)
  {
	/* Avoid out of bound array access */
	if(rate >=  (sizeof(vocsup_frame_sizes_8k) / sizeof(vocsup_frame_sizes_8k[0])))
	{ 
	  MSG_ERROR("Invalid vocoder framerate: %d", rate, 0, 0);
	  return 0;
	}
    return(vocsup_frame_sizes_8k[rate]);
  }
#endif  /* MSMAUD_VOC_4GV || MSMAUD_VOC_4GV_WB*/
#if defined(MSMAUD_VOC_AMR_COMMON)
  else if (so == VOC_CAP_AMR) {
    mode = vocsup_native_header_to_amr_mode(rate);
    amr_rate = vocsup_native_header_to_amr_rate(rate);
    return vocsup_amr_rate_to_size(mode, amr_rate, so);
  }
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
  return(0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_compute_freq

DESCRIPTION
  This function returns the vocoder tone value for a given frequency
  to be played by the vocoder.

DEPENDENCIES
  Requires tone_values_table[][].

RETURN VALUE
  Returns the a word containing the vocoder tone value.

SIDE EFFECTS
  None.
===========================================================================*/
word vocsup_compute_freq
(
  word frequency,
    /* Frequency in Hz for which vocoder tone values are to be computed */
  voc_sample_rate_type sample_rate
    /* Sampling rate that the vocoder is working in                     */
)
{
  word voc_tone_val;         /* Computed result for a vocoder tone  */
  word tlower_freq;          /* Closest known freq to given freq    */
  word tindex;               /* Index into precomputed values table */
  word xdiff, ydiff;         /* Intermediates needed for computation*/
  word sample_rate_index;    /* Index into tone_values_table        */
  int last_entry;            /* last entry used in cache            */
  int second_last_entry;     /* Always inverse of last_entry        */

  /* Last cache table    */
  static int last_entry_table[VOC_SAMPLE_RATE_UNKNOWN-1]; 
  /* Frequency cache     */
  static word freq_cache[VOC_SAMPLE_RATE_UNKNOWN-1][2];  
  /* Freq Voc_Value cache*/
  static word val_cache[VOC_SAMPLE_RATE_UNKNOWN-1][2];   

  sample_rate_index = (word) (sample_rate - VOC_SAMPLE_RATE_8000);

  /* Avoid out of bound array access */
  if(sample_rate_index > VOC_SAMPLE_RATE_UNKNOWN-2)
  {
	MSG_ERROR("Invalid sample rate index: %d", sample_rate_index, 0, 0);
	return 0;
  }

  last_entry = last_entry_table[sample_rate_index];
                                          /*Toggle second last entry*/
  second_last_entry = (last_entry == 0)? 1 : 0;

  /* Check if the given frequency is out of range for the vocoder   */
  if (frequency < VOC_FREQ_MIN ) {
    ERR("Freq: %d -- Min Freq is %d Hz",frequency,VOC_FREQ_MIN,0);
    frequency = VOC_FREQ_MIN;/* If freq is too low then set to min  */
  } else if (frequency > VOC_FREQ_MAX ) {
    ERR("Freq: %d -- Max Freq is %d Hz",frequency,VOC_FREQ_MAX,0);
    frequency = VOC_FREQ_MAX;/* If freq is too high then set to max */
  }

  /* Check the cache to see if frequency was recently used          */
  if (frequency == freq_cache[sample_rate_index][last_entry]) {
    voc_tone_val = val_cache[sample_rate_index][last_entry];
  } else if (frequency == freq_cache[sample_rate_index][second_last_entry]) {
    voc_tone_val = val_cache[sample_rate_index][second_last_entry];
    /* Update last_entry to the entry that was found in cache incase
    ** this value is required again.
    */
    last_entry_table[sample_rate_index] = second_last_entry;
  } else {
    /* Frequency was not cached, therefore, compute from table. */
    /* Find the closest (lower) adjacent frequency from the known frequencies*/
    tindex = (frequency - VOC_FREQ_MIN) / VOC_FREQ_STEP;
    tlower_freq = (word) (tindex * VOC_FREQ_STEP + VOC_FREQ_MIN);
    xdiff = frequency - tlower_freq;
    voc_tone_val = tone_values_table[sample_rate_index][tindex];
    if (xdiff != 0 ) {
      /* For all other values compute value by interpolation                 */
      /*lint -e661 : tindex+1 will never go out of tone_values_table         */
      ydiff = voc_tone_val - tone_values_table[sample_rate_index][tindex+1];
      /*lint +e661 */
      voc_tone_val = (word) (voc_tone_val-((ydiff*xdiff)/VOC_FREQ_STEP));
    }
    /* Update the cache since a value had to be computed                     */
    /* Update the last used entry*/
    last_entry_table[sample_rate_index] = second_last_entry;
     /* Update the last computed frequency*/
    freq_cache[sample_rate_index][last_entry] = frequency;
    /* Update the freq's value          */
    val_cache[sample_rate_index][last_entry] = voc_tone_val;
  }
  MSG_LOW("Freq:%d voc_value:%d",frequency,voc_tone_val,0);
  return (voc_tone_val);
} /* end of vocsup_compute_freq */

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION VOCSUP_VALIDATE_FRAME_CONTROL_BYTE

DESCRIPTION
  Given the input vocoder capability, check if the input frame control byte is
  valid.

  This function should only be used for voice playback since a blank frame is
  considered invalid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if frame control byte is OK.
  FALSE if frame control byte is bad.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean vocsup_validate_frame_control_byte 
  (byte fcb, voc_capability_type so) 

{
  boolean ret_val = TRUE;

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(so != VOC_CAP_AMR)
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
  {
    /* Validate against a set of bits.
    */
    fcb &= VSP_VALID_PLAYBACK_FORMAT_M;

    /* Make sure 13K vocoder capability does not process a frame control byte 
    ** with 8K format and vice versa.
    */
    if (((so == VOC_CAP_IS733) && (fcb != VSP_13K_V))  ||
        ((so =! VOC_CAP_IS733) && (fcb != VSP_8K_V)))
    {
      ret_val = FALSE;
    }
  }
  return (ret_val);
} /* end vocsup_validate_frame_control_byte */

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

int vocsup_pb_q_size
  (
    /* Play direction */
    voc_pb_dir_type           play_direction
  )
{
  if (play_direction == VOC_PB_DIR_FORWARD)
  {
    return (vsp_pbq_fwd.size);
  }
  else if (play_direction == VOC_PB_DIR_REVERSE)
  {
    return (vsp_pbq_rev.size);
  }
  else if (play_direction == VOC_PB_DIR_BOTH)
  {
    return (vsp_pbq_fwd.size + vsp_pbq_rev.size);
  }

  return (0);    /* Should not get here */

} /* end vocsup_pb_q_size */

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

voc_pb_header_type *vocsup_pb_get_hdr
  (
    /* Play direction */
    voc_pb_dir_type           play_direction
  )
{
  /* Get ptr to utterance currently playing. */
  if (play_direction == VOC_PB_DIR_BOTH)
  {

    /* The current pb header will only be set once playback has started. */
    if (vsp_curr_pb_hdr != (voc_pb_header_type *) NULL) 
    {
      return (vsp_curr_pb_hdr);
    }
    /* If playback has not started, pick one from the queues.
    */
    else if (vsp_pbq_fwd.size > 0)
    {
      return (vsp_pbq_fwd.queue [vsp_pbq_fwd.remove_idx].pbptr);
    }
    else if (vsp_pbq_rev.size > 0)
    {
      return (vsp_pbq_rev.queue [vsp_pbq_rev.remove_idx].pbptr);
    }
  }

  /* Get ptr to utterance in the forward link playback queue only. */
  else if ((play_direction == VOC_PB_DIR_FORWARD) &&
           (vsp_pbq_fwd.size > 0))
  {
    return (vsp_pbq_fwd.queue [vsp_pbq_fwd.remove_idx].pbptr);
  }

  /* Get ptr to utterance in the reverse link playback queue only. */
  else if ((play_direction == VOC_PB_DIR_REVERSE) &&
           (vsp_pbq_rev.size > 0))
  {
    return (vsp_pbq_rev.queue [vsp_pbq_rev.remove_idx].pbptr);
  }

  /* Return NULL if no utterance is found. */
  return ((voc_pb_header_type *) NULL);    

} /* end vocsup_pb_get_hdr */

/* <EJECT> */
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
  Update vsp_pbq_fwd or vsp_pbq_rev.

===========================================================================*/

vsp_get_frame_type vocsup_pb_get_frame 
  (
    byte *fr_rate,                           /* Frame rate                 */
    byte **fr_data,                          /* Ptr to frame data          */
    voc_pb_header_type **pb_hdr_ptr,         /* Ptr to current play header */
    voc_pb_dir_type play_direction           /* Play direction             */
  )
{
  word                frame_length;
  word                control_byte_length = 0;
  boolean             fcb_error = FALSE;
  byte                frame_rate = VOC_ERASURE;
  voc_pb_header_type  *curr_entry_ptr;
#ifdef FEATURE_AUDFMT_AMR
  voc_rate_type       amr_rate;
  voc_gsm_frame_type  amr_ftype;
#endif /* FEATURE_AUDFMT_AMR */

#if defined(FEATURE_AUDFMT_AMR) || (defined(MSMAUD_VOC_AMR_WB_COMMON) && \
    defined(FEATURE_AMR_IF1))
  static uint8        voc_packet[AMRSUP_VOC_FRAME_BYTES];
#endif /* FEATURE_AUDFMT_AMR || (MSMAUD_VOC_AMR_WB_COMMON && FEATURE_AMR_IF1) */

#if defined(MSMAUD_VOC_AMR_WB_COMMON) && defined(FEATURE_AMR_IF1)
  voc_rate_type              voc_amr_wb_mode;
  amrsup_frame_type          frame_type; /* frame_type */
  amrsup_mode_type           amr_mode;    /* amr_mode */
  amrsup_if1_frame_info_type if1_frame_info;
  int                        num_bytes_decoded = 0;
  voc_amr_wb_frame_type      voc_amr_wb_ftype;
#endif  /* MSMAUD_VOC_AMR_WB_COMMON && FEATURE_AMR_IF1 */

  if (play_direction == VOC_PB_DIR_FORWARD)
  {
    /* Points to current entry in the forward link vocoder play back queue. */
    curr_entry_ptr = &(vsp_pbq_fwd.queue [vsp_pbq_fwd.remove_idx].pbhdr);
   
    /* Return address of current entry. */
    *pb_hdr_ptr = vsp_pbq_fwd.queue [vsp_pbq_fwd.remove_idx].pbptr;
  }
  else
  {
    /* Points to current entry in the reverse link vocoder play back queue. */
    curr_entry_ptr = &(vsp_pbq_rev.queue [vsp_pbq_rev.remove_idx].pbhdr);
   
    /* Return address of current entry. */
    *pb_hdr_ptr = vsp_pbq_rev.queue [vsp_pbq_rev.remove_idx].pbptr;
  }

  /* Update local copy of the address.
  */
  vsp_curr_pb_hdr = *pb_hdr_ptr;

  switch (curr_entry_ptr->pb_len_rate_var)
  {

    /* If fixed length-fixed rate (FLFR), the vocoder driver assumes 
    ** responsibility of building the frame control bytes to save flash memory 
    ** (1 byte per packet).  All FLFR packets are the same length.  The vocoder 
    ** driver outputs the frame control byte plus the same number of voice data 
    ** bytes every frame.
    */
    case VOC_PB_FIX_LEN_FIX_RATE:
  
      *fr_data   = curr_entry_ptr->pb_data_ptr;
      *fr_rate   = curr_entry_ptr->pb_rate;
      frame_rate = curr_entry_ptr->pb_rate; /* used to update data ptr */
      control_byte_length = 0; /* frm control byte not in voice prompt data */
      break; /* end case VOC_PB_FIX_LEN_FIX_RATE */
  
    /* If fixed length-variable rate (FLVR), the vocoder driver uses the frame 
    ** control bytes embedded in the voice data.  All FLVR packets are the same 
    ** length.  Packet length is derived from packet rate (see pb_rate below) 
    ** and voice mode specified in the header.  Actual rate in the frame 
    ** control byte may be equal to or less than packet rate.  Lower rate 
    ** packets' (shorter packets') unused bytes are padded to the same length 
    ** as packets with rate specified in the header.  The vocoder driver only 
    ** outputs legitimate bytes.
    */
    case VOC_PB_FIX_LEN_VAR_RATE:
  
      /* Validate frame control byte first.
      */
      if (vocsup_validate_frame_control_byte (
              *(curr_entry_ptr->pb_data_ptr), 
              curr_entry_ptr->pb_voc_capability) == FALSE)
      {
        fcb_error = TRUE;
        MSG_HIGH ("GetFrame, Invalid FCB, data=%x,%x,%x",
                *(curr_entry_ptr->pb_data_ptr), 
                *(curr_entry_ptr->pb_data_ptr + 1),
                *(curr_entry_ptr->pb_data_ptr + 2));
      }
      else 
      {
        *fr_rate   = vocsup_dec_format_to_rate (*(curr_entry_ptr->pb_data_ptr));
        *fr_data   = curr_entry_ptr->pb_data_ptr + 1;
        frame_rate = curr_entry_ptr->pb_rate; /* used to update data ptr */
        control_byte_length = 1; /* the control byte is embedded in the data */
      }
      break; /* end case VOC_PB_FIX_LEN_VAR_RATE */
  
    /* If variable length-variable rate (VLVR), the vocoder driver uses the 
    ** frame control bytes embedded in the voice data.  All VLVR packets are of
    ** variable length.  The vocoder driver outputs the correct number of bytes  
    ** using the rate and voice mode information in the frame control byte.      
    */ 
    case VOC_PB_VAR_LEN_VAR_RATE:
  
      /* Validate frame control byte first.
      */
      if (vocsup_validate_frame_control_byte (
              *(curr_entry_ptr->pb_data_ptr), 
              curr_entry_ptr->pb_voc_capability ) == FALSE)
      {
        fcb_error = TRUE;
        MSG_HIGH ("GetFrame, Invalid FCB, data=%x,%x,%x",
                *(curr_entry_ptr->pb_data_ptr), 
                *(curr_entry_ptr->pb_data_ptr + 1),
                *(curr_entry_ptr->pb_data_ptr + 2));
      }
      else 
      {
        *fr_rate   = vocsup_dec_format_to_rate (*(curr_entry_ptr->pb_data_ptr));
        *fr_data   = curr_entry_ptr->pb_data_ptr + 1;
        frame_rate = *fr_rate; /* used to update data ptr */
        control_byte_length = 1; /* the control byte is embedded in the data */
      }
      break; /* end case VOC_PB_VAR_LEN_VAR_RATE */
  
    case VOC_PB_NATIVE_QCP:
      *fr_rate   = vocsup_native_to_voc_format( *(curr_entry_ptr->pb_data_ptr),
                                       curr_entry_ptr->pb_voc_capability);
      frame_rate = *fr_rate; /* used to update data ptr */
      *fr_data   = curr_entry_ptr->pb_data_ptr + 1;
      control_byte_length = 1; /* the control byte is embedded in the data */
      break; /* end case VOC_PB_NATIVE_QCP */

#ifdef FEATURE_AUDFMT_EVB
    case VOC_PB_EVB:
#ifdef FEATURE_AUDFMT_EVW
    case VOC_PB_EVW:
#endif /* FEATURE_AUDFMT_EVW*/
      *fr_rate = *(curr_entry_ptr->pb_data_ptr);
      frame_rate = *fr_rate;
      *fr_data   = curr_entry_ptr->pb_data_ptr + 1;
      control_byte_length = 1; /* the control byte is embedded in the data */
      break;
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_AMR
    case VOC_PB_AMR:
      /* Convert QCP-AMR header */
      vocsup_extract_audfmt_amr_header_info(
                                        (byte *)(curr_entry_ptr->pb_data_ptr),
                                        &amr_rate,
                                        &amr_ftype);
	  /* Avoid out of bound array access */
	  if(amr_ftype >= VOC_GSM_FRAME_MAX)
	  {
		MSG_ERROR("Invalid AMR frametype: %d", amr_ftype, 0, 0);
		return 0;
	  }

	  /* Avoid out of bound array access */
	  if(amr_rate >= VOC_GSM_RATE_MAX)
	  {
		MSG_ERROR("Invalid AMR framerate: %d", amr_rate, 0, 0);
		return 0;
	  }

      /* Convert AMR formatted packet to QCP-AMR style */
      amrsup_audfmt_de_framing(voc_packet,
                               VOC_TO_AMRSUP_FRAME(amr_ftype),
                               VOC_TO_AMRSUP_MODE(amr_rate),
                               (uint8*)(curr_entry_ptr->pb_data_ptr+1));

      /* Point rate byte to AMR header */
      *fr_rate = vocsup_amr_to_native_header (amr_rate, amr_ftype);
      frame_rate = *fr_rate;
      /* Point data to AMR buffer */
      *fr_data = (byte*)voc_packet;
      /* 1 control byte here.. used to skip forward to the next buffer */
      control_byte_length = 1; /* the control byte is embedded in the data */
      break; /* end case VOC_PB_AMR */
#endif /* FEATURE_AUDFMT_AMR */

  case VOC_PB_AWB:
#if defined(MSMAUD_VOC_AMR_WB_COMMON) && defined(FEATURE_AMR_IF1)
    /* Now that we have the whole frame, we transcode it: IF1 to 
    ** raw DSP format
    */
    /* extract the frame type index from the frame header byte */
    if1_frame_info.frame_type_index = (amrsup_frame_type_index_type)((*(curr_entry_ptr->pb_data_ptr) & \
                       VSP_AUDFMT_AMR_WB_FRAME_TYPE_MASK) >> VSP_AUDFMT_AMR_WB_FRAME_TYPE_SHIFT);

    /* Set quality flag */
    if((*(curr_entry_ptr->pb_data_ptr) & VSP_AUDFMT_AMR_WB_FRAME_QUALITY_MASK) ==
                                              VSP_AUDFMT_AMR_WB_FRAME_QUALITY_MASK) {
      if1_frame_info.fqi = TRUE;
    } else {
      if1_frame_info.fqi = FALSE;
    }
    if1_frame_info.amr_type = AMRSUP_CODEC_AMR_WB;

    num_bytes_decoded =\
    amrsup_if1_de_framing(voc_packet,
                          &frame_type,
                          &amr_mode,
                          (uint8*)(curr_entry_ptr->pb_data_ptr+1),
                          &if1_frame_info);

    voc_amr_wb_mode = AMRSUP_TO_VOC_AMR_WB_MODE(amr_mode);
    voc_amr_wb_ftype = vocsup_amrsup_to_voc_amrwb_ftype(frame_type);

    /* Point rate byte to AMR-WB header */
    *fr_rate = ((((uint8)voc_amr_wb_ftype << 4) & 0xf0) | ((uint8)voc_amr_wb_mode & 0x0f));
    frame_rate = *fr_rate;
    /* Point data to AMR-WB buffer */
    *fr_data = (byte*)voc_packet;
    /* 1 control byte here.. used to skip forward to the next buffer */
    control_byte_length = 1; /* the control byte is embedded in the data */

#endif  /* MSMAUD_VOC_AMR_WB_COMMON && FEATURE_AMR_IF1 */

    break; /* end case VOC_PB_AMR */

    default:
      break; /* end default case */

  } /* end switch (curr_entry_ptr->pb_len_rate_var) */
  
  /* Make sure frame control byte is valid to avoid playing bad voice frames.  
  ** If frame control byte is bad, return an erasure packet and skip to end 
  ** of prompt.
  */
  if (fcb_error == TRUE)
  {
    MSG_HIGH ("Invalid playback frame control byte",0,0,0);
    *fr_rate = VOC_ERASURE;
    *fr_data = vocsup_erasure_packet;
    curr_entry_ptr->pb_num_frames = 0;
  }
  else
  {
#if defined(MSMAUD_VOC_AMR_WB_COMMON) && defined(FEATURE_AMR_IF1)
    if (curr_entry_ptr->pb_voc_capability == VOC_CAP_AMR_WB) {
      frame_length = (word)num_bytes_decoded;  
    }
    else     
#endif  /* MSMAUD_VOC_AMR_WB_COMMON && FEATURE_AMR_IF1 */
    {
    frame_length = 
          vocsup_rate_to_size (frame_rate, curr_entry_ptr->pb_voc_capability);
    }
    frame_length += control_byte_length;
    curr_entry_ptr->pb_data_ptr += frame_length;
    curr_entry_ptr->pb_num_frames--;
  }

  /* Check for end of entry */
  if (curr_entry_ptr->pb_num_frames == 0)
  {
    /* Reset local copy of the current playback header address.
    */
    vsp_curr_pb_hdr = (voc_pb_header_type *) NULL;
   
    if (play_direction == VOC_PB_DIR_FORWARD)
    {
      vsp_pbq_fwd.remove_idx++;
      if (vsp_pbq_fwd.remove_idx == VSP_PBQ_MAX)
      {
        vsp_pbq_fwd.remove_idx = 0;
      }
      /* Done with 1 queue entry so subtract 1 entry */
      vsp_pbq_fwd.size--;  /* Number of entries in queue */
     
      if (vsp_pbq_fwd.size > 0)
      {
        /* Just finished the last frame in the current prompt, play the
        ** next prompt in the queue in the next interrupt.
        */
        return (NEXT_PROMPT);
      }
      else
      {
        /* Just finished the last frame in the current prompt, no more 
        ** prompt to play.
        */
        return (QUEUE_EMPTY);
      }
    } /* of if (play_direction == VOC_PB_DIR_FORWARD) */
    else
    {
      vsp_pbq_rev.remove_idx++;
      if (vsp_pbq_rev.remove_idx == VSP_PBQ_MAX)
      {
        vsp_pbq_rev.remove_idx = 0;
      }
      /* Done with 1 queue entry so subtract 1 entry */
      vsp_pbq_rev.size--;  /* Number of entries in queue */
     
      if (vsp_pbq_rev.size > 0)
      {
        /* Just finished the last frame in the current prompt, play the
        ** next prompt in the queue in the next interrupt.
        */
        return (NEXT_PROMPT);
      }
      else
      {
        /* Just finished the last frame in the current prompt, no more 
        ** prompt to play.
        */
        return (QUEUE_EMPTY);
      }
    } /* end if (play_direction == VOC_PB_DIR_FORWARD) */
  } /* of if (curr_entry_ptr->pb_num_frames == 0) */
  else
  {
    /* The current prompt is not over yet.
    */
    return (SAME_PROMPT);
  } /* end if (curr_entry_ptr->pb_num_frames == 0) */ 

} /* end vocsup_pb_get_frame */

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

void vocsup_pb_add_entry
  (
    /* Pointer to voice utterance header */
    voc_pb_header_type *voc_pb_header_ptr,

    /* Play direction */
    voc_pb_dir_type    play_direction
  )
{
  /* interrupt context holder */
  uint32 isave;

  /* No interrupts while we manipulate the queue since the queue is used in
  ** an ISR.
  */
  INTLOCK_SAV (isave); /* Disable interrupts and save PSW */

  if (play_direction == VOC_PB_DIR_FORWARD)
  {
    /* Each entry of the play back queue is a copy of the header, not the header
    ** itself because the frame number is decremented as the utterance is being 
    ** played, thus the frame number is 0 by the time the utterance is finished.
    ** Since the multi play counts on the fact that the utterance can be 
    ** replayed, this problem is solved by creating the vsp_pb_header_type 
    ** struct.  We are essentially queueing a copy of the header and keeping with
    ** it the pointer to the real header.
    */
    vsp_pbq_fwd.queue [vsp_pbq_fwd.insert_idx].pbhdr = *voc_pb_header_ptr;
    vsp_pbq_fwd.queue [vsp_pbq_fwd.insert_idx++].pbptr = voc_pb_header_ptr;
   
    if (vsp_pbq_fwd.insert_idx == VSP_PBQ_MAX)
    {
      vsp_pbq_fwd.insert_idx = 0;
    }
    vsp_pbq_fwd.size++;  /* Number of entries in queue */
  }
  else
  {
    vsp_pbq_rev.queue [vsp_pbq_rev.insert_idx].pbhdr = *voc_pb_header_ptr;
    vsp_pbq_rev.queue [vsp_pbq_rev.insert_idx++].pbptr = voc_pb_header_ptr;
   
    if (vsp_pbq_rev.insert_idx == VSP_PBQ_MAX)
    {
      vsp_pbq_rev.insert_idx = 0;
    }
    vsp_pbq_rev.size++;  /* Number of entries in queue */
  }
  /* Interrupts OK again */
  INTFREE_SAV (isave); /* Restore interrupts (PSW) */

  /* Display debug information outside INTLOCK */
  if (play_direction == VOC_PB_DIR_FORWARD)
  {
    MSG_MED ("Add to fwd play queue, entries =%d",vsp_pbq_fwd.size,0,0);
  }
  else
  {
    MSG_MED ("Add to rev play queue, entries =%d",vsp_pbq_rev.size,0,0);
  }
} /* end vocsup_pb_add_entry */

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

void vocsup_pb_clear_q 
  (
    /* Play direction */
    voc_pb_dir_type    play_direction
  )
{
  if (play_direction == VOC_PB_DIR_FORWARD)
  {
    vsp_pbq_fwd.insert_idx = 0;  /* Forward link play back queue insert idx */
    vsp_pbq_fwd.remove_idx = 0;  /* Forward link play back queue remove idx */
    vsp_pbq_fwd.size = 0;        /* Number of entries in queue */
  }
  else if (play_direction == VOC_PB_DIR_REVERSE)
  {
    vsp_pbq_rev.insert_idx = 0;  /* Reverse link play back queue insert idx */
    vsp_pbq_rev.remove_idx = 0;  /* Reverse link play back queue remove idx */
    vsp_pbq_rev.size = 0;        /* Number of entries in queue */
  } 
  else
  {
    vsp_pbq_fwd.insert_idx = 0;  /* Forward link play back queue insert idx */
    vsp_pbq_fwd.remove_idx = 0;  /* Forward link play back queue remove idx */
    vsp_pbq_fwd.size = 0;        /* Number of entries in queue */
    vsp_pbq_rev.insert_idx = 0;  /* Reverse link play back queue insert idx */
    vsp_pbq_rev.remove_idx = 0;  /* Reverse link play back queue remove idx */
    vsp_pbq_rev.size = 0;        /* Number of entries in queue */
  }

  /* Reset local copy of the current playback header address.  This is done 
  ** when the prompt finishes playing but should also be done here in case 
  ** playback is aborted.
  */
  vsp_curr_pb_hdr = (voc_pb_header_type *) NULL;
 
} /* end vocsup_pb_clear_q */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_BYTES_TO_FRAMES

DESCRIPTION
  Given the specified vocoder and vocoder frame format, determine how many 
  COMPLETE vocoder frames in the given format will fit into the number of bytes
  specified.  In the case of variable size formats, out_min_frames and 
  out_max_frames may be set to different values.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Maximum number of frames
  Minimum number of frames

SIDE EFFECTS
  None

===========================================================================*/

void voc_bytes_to_frames
  (
    voc_capability_type vocoder,       /* Option to configure vocoder      */
    voc_pb_len_rate_var_type format,   /* Play back format                 */
    voc_rate_type max_rate,            /* Max vocoder rate to consider     */
    dword size_in_bytes,               /* Size of buffer                   */
    dword *out_min_frames,             /* Min number of frames possible    */
    dword *out_max_frames              /* Max number of frames possible    */
  ) 
{
  word max_frm_size;      /* Frame size for default min rate of 1/8th      */
  word min_frm_size;      /* Frame size for input max rate                 */

  /* Get size of each frame without the control byte. */
#if defined(MSMAUD_VOC_AMR_COMMON)
  if(vocoder == VOC_CAP_AMR) {
    max_frm_size = vocsup_amr_rate_to_size(VOC_AMR_SPEECH_GOOD,
                                           max_rate, vocoder);
    min_frm_size = vocsup_amr_rate_to_size(VOC_AMR_SPEECH_GOOD,
                                           VOC_AMR_RATE_475, vocoder);
  } else 
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
  {
    max_frm_size = vocsup_rate_to_size (max_rate, vocoder);
    min_frm_size = vocsup_rate_to_size (VOC_8_RATE, vocoder);
  }

  /* Switch depending on the frame format. */
  switch (format)
  {
  
    /* If fixed length-fixed rate, the frame control byte is not saved
    ** with the data so do nothing.
    */
    case VOC_PB_FIX_LEN_FIX_RATE:

      break; /* end case VOC_PB_FIX_LEN_FIX_RATE */
  
    /* If variable length-variable rate, the frame control byte is 
    ** stored along with the data so increase the frame size by 1. 
    */ 
    case VOC_PB_VAR_LEN_VAR_RATE:
    case VOC_PB_NATIVE_QCP:

      min_frm_size += 1;
      max_frm_size += 1;
      break; /* end case VOC_PB_VAR_LEN_VAR_RATE */
  
    /* fixed length-variable rate format is not supported for voice memo.
    */ 
    case VOC_PB_FIX_LEN_VAR_RATE:
      
      ERR ("FIX_LEN_VAR_RATE format not supported!!!!", 0, 0, 0);
      break; /* end case VOC_PB_FIX_LEN_VAR_RATE */
  
    default:

      ERR_FATAL ("Illegal frame format!!!!", 0, 0, 0);
      break; /* end default case */
  
  } /* end switch (format) */

  *out_min_frames = size_in_bytes / (dword) max_frm_size;
  *out_max_frames = size_in_bytes / (dword) min_frm_size;

} /* end voc_bytes_to_frames */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_COUNT_FRAMES

DESCRIPTION
  Given the specified vocoder and vocoder frame format, examine the buffer of 
  vocoder frames passed in and determine how many COMPLETE vocoder frames are 
  present, up to the number of frames specified in inout_frame_count.  Return 
  the number of COMPLETE vocoder frames and how many bytes those frames 
  occupy.  Always assume that the first byte of the input buffer begins a new 
  vocoder frame. 

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Status of request
  Number of complete vocoder frames
  Number of bytes those frames occupy

SIDE EFFECTS
  None

===========================================================================*/

voc_status_type voc_count_frames
  (
    voc_capability_type vocoder,       /* Option to configure vocoder      */ 
    voc_pb_len_rate_var_type format,   /* Play back format                 */ 
    voc_rate_type max_rate,            /* Max vocoder rate to consider     */ 
    const void *in_buffer,             /* Pointer to input buffer          */ 
    dword in_buffer_bytes,             /* Size of input buffer             */ 
    dword *out_frame_bytes,            /* Number of bytes occupied         */ 
    dword *inout_frame_count           /* Number of complete frames        */
  )

{
  word                      frame_length = 0;
  voc_rate_type             frame_rate = VOC_ERASURE;
  dword                     frame_count = 0L;
  dword                     frame_bytes = 0L;
  boolean                   done = FALSE;
  byte                      *data_ptr;  
  byte                      *end_ptr;
  voc_status_type           ret_val = VOC_DONE_S;

  ASSERT (in_buffer != NULL);

  /* Switch depending on the frame format. */
  switch (format)
  {

    case VOC_PB_FIX_LEN_FIX_RATE:
  
      /* Get size of each frame without the control byte. */
      frame_length = vocsup_rate_to_size (max_rate, vocoder);

      /* All frames are the same size, so a simple divide will do. */
      frame_count = in_buffer_bytes / (dword) frame_length;

      /* If input frame count is greater than what's in the buffer, return the
      ** number of frames in the buffer.
      */
      if (*inout_frame_count > frame_count)
      {
        *inout_frame_count = frame_count;
      }
      *out_frame_bytes = frame_length * (*inout_frame_count);
      break; /* end case VOC_PB_FIX_LEN_FIX_RATE */
  
    case VOC_PB_VAR_LEN_VAR_RATE:
  
      /* Use this pointer to traverse down the buffer. */
      data_ptr = (byte *) in_buffer;
      end_ptr  = (byte *) in_buffer + (in_buffer_bytes - 1);

      while (!done)
      {
        /* Make sure frame control byte is valid to avoid playing bad voice
        ** frames.  If frame control byte is bad, return error code and do not
        ** include bad frame in the returned parameters.
        */
        if (vocsup_validate_frame_control_byte (*data_ptr, vocoder) == FALSE)
        {
          MSG_HIGH ("CountFrame, Invalid FCB, data=%x,%x,%x",
                     *data_ptr, *(data_ptr + 1), *(data_ptr + 2));
          done = TRUE;
          ret_val = VOC_CMD_FAILED_S;
        }
        else
        {
          /* Get size of each frame without the control byte. */
          frame_rate = vocsup_dec_format_to_rate (*data_ptr);
          frame_length = vocsup_rate_to_size (frame_rate, vocoder);
          
          /* Move data pointer to the next control byte, account for the 
          ** control byte embedded in the data.
          */
          data_ptr = data_ptr + frame_length + 1;
          
          /* Increment frame counter. */
          frame_count++;
        
          /* Increment number of bytes contained in the accounted frames. */
          frame_bytes = frame_bytes + (frame_length + 1);
      
          /* Done if go past end of buffer or reach desired number of frames. */
          if ((data_ptr > end_ptr) || (frame_count == *inout_frame_count))
          {
            done = TRUE;
          }
        }
      } /* end while loop */

      *inout_frame_count = frame_count;
      *out_frame_bytes = frame_bytes;

      break; /* end case VOC_PB_VAR_LEN_VAR_RATE */
  
    case VOC_PB_FIX_LEN_VAR_RATE:
      
      /* fixed length-variable rate format is not supported for voice memo.
      */ 
      ERR ("FIX_LEN_VAR_RATE format not supported!!!!", 0, 0, 0);
      break; /* end case VOC_PB_FIX_LEN_VAR_RATE */
  
    default:

      ERR_FATAL ("Illegal frame format!!!!", 0, 0, 0);
      break; /* end default case */
  
  } /* end switch (format) */

  return (ret_val);

} /* end voc_count_frames */

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

void vocsup_rec_clear_buffers (void)
{
  memset((uint8 *)(&vsp_rec_buf_array), 0, sizeof(vsp_rec_buf_array_type));
} /* end vocsup_rec_clear_buffers */

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_get_frame_buf

DESCRIPTION
  This function returns a buffer to hold a recorded frame until the
  vocoder driver gets a chance to process it.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 *vocsup_rec_get_frame_buf ( void )
{
  uint8 *ret_val = NULL;

  if(vsp_rec_frame_buf.num_in_use < VSP_REC_FRAME_NUM_BUFFERS) {
    ret_val = vsp_rec_frame_buf.packet_buf[vsp_rec_frame_buf.index];
    vsp_rec_frame_buf.num_in_use++;
    vsp_rec_frame_buf.index++;
    if(vsp_rec_frame_buf.index == VSP_REC_FRAME_NUM_BUFFERS) {
      vsp_rec_frame_buf.index = 0;
    }
  } else {
    MSG_ERROR("Ran out of frame buffers!", 0,0,0);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_free_frame_buf

DESCRIPTION
  This function frees a frame buffer.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vocsup_rec_free_frame_buf ( uint8 *buf )
{
  uint32 index;

  if(vsp_rec_frame_buf.num_in_use == 0) {
    MSG_ERROR("Freeing unallocated buffer!",0,0,0);
  } else {
    INTLOCK();
    if(vsp_rec_frame_buf.num_in_use > vsp_rec_frame_buf.index) {
      index = VSP_REC_FRAME_NUM_BUFFERS - 
                    (vsp_rec_frame_buf.num_in_use - vsp_rec_frame_buf.index);
    } else {
      index = vsp_rec_frame_buf.index - vsp_rec_frame_buf.num_in_use;
    }
    INTFREE();
    if(buf != vsp_rec_frame_buf.packet_buf[index]) {
      MSG_ERROR("Freeing buffer out of order!",0,0,0);
    } else {
      INTLOCK();
      vsp_rec_frame_buf.num_in_use--;
      INTFREE();
    }
  }
}

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
void vocsup_rec_put_buf (
  uint8  *buffer,
  uint32 num_bytes
) {
  uint32 entry;
  if(vsp_rec_buf_array.number < VSP_REC_BUF_MAX) {
    INTLOCK();
    entry = vsp_rec_buf_array.index + vsp_rec_buf_array.number;
    if(entry >= VSP_REC_BUF_MAX) {
      entry -= VSP_REC_BUF_MAX;
    }
    vsp_rec_buf_array.rec_buf[entry].buffer    = buffer;
    vsp_rec_buf_array.rec_buf[entry].num_bytes = num_bytes;
    vsp_rec_buf_array.number++;
    INTFREE();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_get_buf

DESCRIPTION
  Get a buffer from the record buffer queue.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vocsup_rec_get_buf (
  uint32 *num_bytes,
  uint8  **buffer
) {
  if(vsp_rec_buf_array.number != 0) {
    *buffer    = vsp_rec_buf_array.rec_buf[vsp_rec_buf_array.index].buffer;
    *num_bytes = vsp_rec_buf_array.rec_buf[vsp_rec_buf_array.index].num_bytes;

    INTLOCK();
    vsp_rec_buf_array.index++;
    if(vsp_rec_buf_array.index == VSP_REC_BUF_MAX) {
      vsp_rec_buf_array.index = 0;
    }
    vsp_rec_buf_array.number--;
    INTFREE();
  } else {
    *buffer    = NULL;
    *num_bytes = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vocsup_rec_reset_hdr

DESCRIPTION
  This function resets a recording header.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vocsup_rec_reset_hdr (
  voc_rec_header_type *hdr
) {
  if(hdr != NULL) {
    hdr->rec_data_ptr    = NULL;
    hdr->rec_buffer_size = 0;
    hdr->rec_num_frames  = 0;
    hdr->rec_length      = 0;
  }
} /* end vocsup_rec_reset_hdr */

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
void vocsup_rec_free_buf (
  voc_rec_result_type *rec_result
) {
  vocsup_rec_reset_hdr(rec_result->fwd_ptr);
  vocsup_rec_reset_hdr(rec_result->rev_ptr);
}

#ifdef FEATURE_MM_REC
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
void vocsup_rec_reset_frm_count (void)
{
  if (vsp_rec_parm.sync_cb_func != NULL) {
    vsp_rec_data_req_cnt1 = 0;
    vsp_rec_data_req_cnt2 = 0;
	vsp_rec_av_sync_stamp = FALSE;
  }
}

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
void vocsup_rec_do_av_sync (void)
{
  voc_rec_av_report_type av_report;
  qword                  time_stamp;

  clk_uptime_ms(time_stamp);

  av_report.num_samples = (vsp_rec_data_req_cnt2 - vsp_rec_data_req_cnt1) * 
                          VSP_REC_FRAME_SAMPLES;

  // *(uint64 *)(av_report.time_stamp) = *(uint64 *)time_stamp;
  av_report.time_stamp[0] = time_stamp[0];
  av_report.time_stamp[1] = time_stamp[1];

  av_report.lost_samples = 0;
  av_report.frame_index  = vsp_rec_data_frame_index;

  if (vsp_rec_parm.sync_cb_func != NULL) {
  vsp_rec_parm.sync_cb_func(&av_report,
                            vsp_rec_parm.client_data);
  }

  if (vsp_rec_data_req_cnt2 == (uint32)(-1)) {
    vsp_rec_data_req_cnt2 = 0;
  }
  
  if (vsp_rec_data_frame_index == (uint32)(-1)) {
    vsp_rec_data_frame_index = 0;
  }

  vsp_rec_data_req_cnt1 = vsp_rec_data_req_cnt2;
}
#endif /* FEATURE_MM_REC */

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_FRAME

DESCRIPTION
  This function records one voice frame.  It's called from 
  voc_rec_frame_processing in order to record in or out of a call.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Record result.

SIDE EFFECTS
  Update record buffer info.

===========================================================================*/
void vocsup_rec_frame (
  voc_rec_type        dir,
  byte                packet_format, 
  byte                *data_ptr,
  voc_rec_result_type *rec_result
) {
  voc_rec_header_type *vsp_curr_rec_hdr = NULL;
  byte                rate;
  word                size;
  byte                *curr_rec_data_ptr; /* Crnt loc in rec buf to write. */
  byte                *frame_ptr;
  static const uint8  null_frame[] = {0xff, 0xff, 0xff};
#ifdef FEATURE_AUDFMT_AMR
  voc_rate_type       amr_rate;
  voc_gsm_frame_type  amr_ftype;
#endif /* FEATURE_AUDFMT_AMR */
#ifdef FEATURE_MM_REC
  boolean rec_data = FALSE;
#endif /* FEATURE_MM_REC */

  /* Initialize return structure. */
  rec_result->status  = VOC_REC_STAT_NOT_READY;
  rec_result->fwd_ptr = (voc_rec_header_type *) NULL;
  rec_result->rev_ptr = (voc_rec_header_type *) NULL;

  /* If an error has been reported, just set status to not ready to avoid 
  ** calling back multiple times to report the same error.  
  */
  if (vsp_error_reported == FALSE) {
    if(dir == VOC_REC_FORWARD) {
      vsp_curr_rec_hdr = &vsp_rec_rx_hdr;
    } else if(dir == VOC_REC_REVERSE) {
      vsp_curr_rec_hdr = &vsp_rec_tx_hdr;
    } else {
      rec_result->status = VOC_REC_STAT_ERROR;
      vsp_error_reported = TRUE;
    }

    /* If a new buffer is needed. */
    if ((rec_result->status != VOC_REC_STAT_ERROR) &&
        (vsp_curr_rec_hdr->rec_data_ptr == NULL))
    {
      /* Get a buffer. */
      vocsup_rec_get_buf( &vsp_curr_rec_hdr->rec_buffer_size, 
                          &vsp_curr_rec_hdr->rec_data_ptr );

      /* Run out of buffer to record so report the error. */
      if((vsp_curr_rec_hdr->rec_data_ptr == NULL) || 
         (vsp_curr_rec_hdr->rec_buffer_size == 0)) {
        /* Set status to error. */
        rec_result->status = VOC_REC_STAT_BUFFER_ERROR;

        /* Indicate error has been reported. */
        vsp_error_reported = TRUE;
      }
    }

    if (rec_result->status == VOC_REC_STAT_NOT_READY) {
      /* Points to current location in recording buffer. */
      curr_rec_data_ptr = 
               vsp_curr_rec_hdr->rec_data_ptr + vsp_curr_rec_hdr->rec_length;

      /* Switch depending on the frame format. */
      switch (vsp_rec_parm.settings.frame_format) {
        case VOC_PB_NATIVE_QCP:
          rate = packet_format;
          if(((rate == VOC_ERASURE) || (rate == VOC_0_RATE) ||
              (rate == VOC_ERR_RATE))
#if defined(MSMAUD_VOC_AMR_COMMON)
             && (vsp_rec_parm.settings.capability != VOC_CAP_AMR)
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
            ) {
            /* Insert 1/8th rate NULL in place of erasures and blank frames */
            frame_ptr = (uint8 *) null_frame;
            rate      = VOC_8_RATE;
          } else {
            frame_ptr = data_ptr;
          }
          size = vocsup_rate_to_size(rate, vsp_rec_parm.settings.capability);

          *curr_rec_data_ptr++ = vocsup_voc_to_native_format (rate,
                                        vsp_rec_parm.settings.capability);

          memcpy(curr_rec_data_ptr, frame_ptr, size);
          vsp_curr_rec_hdr->rec_length = vsp_curr_rec_hdr->rec_length + 
                                         size + 1;
          vsp_curr_rec_hdr->rec_num_frames++;
          break;

#ifdef FEATURE_AUDFMT_AMR
        case VOC_PB_AMR:
          /* for AMR, packet_format is already converted to a native header */
          rate = packet_format;
          frame_ptr = data_ptr;
          size = vocsup_rate_to_size(rate, vsp_rec_parm.settings.capability);

          amr_ftype = vocsup_native_header_to_amr_mode(rate);
          amr_rate = vocsup_native_header_to_amr_rate(rate);
          *curr_rec_data_ptr++ = vocsup_build_audfmt_amr_header(amr_rate,
                                                                amr_ftype);

          /* Reorganize data to match AMR file format */
          amrsup_audfmt_framing(frame_ptr,
                                VOC_TO_AMRSUP_FRAME(amr_ftype),
                                VOC_TO_AMRSUP_MODE(amr_rate),
                                curr_rec_data_ptr);

          vsp_curr_rec_hdr->rec_length = vsp_curr_rec_hdr->rec_length + 
                                         size + 1;
          vsp_curr_rec_hdr->rec_num_frames++;
          break;
#endif /* FEATURE_AUDFMT_AMR */

#ifdef FEATURE_AUDFMT_EVB
      case VOC_PB_EVB:
#ifdef FEATURE_AUDFMT_EVW
      case VOC_PB_EVW:
#endif /* FEATURE_AUDFMT_EVW */
          rate = packet_format;
          frame_ptr = data_ptr;
          size = vocsup_rate_to_size(rate, vsp_rec_parm.settings.capability);

          /* Write frame header */
          *curr_rec_data_ptr++ = rate;

          /* Write frame data */
          memcpy(curr_rec_data_ptr, frame_ptr, size);
          vsp_curr_rec_hdr->rec_length = vsp_curr_rec_hdr->rec_length + 
                                         size + 1;
          vsp_curr_rec_hdr->rec_num_frames++;
          break;
#endif /* FEATURE_AUDFMT_EVB */

        case VOC_PB_FIX_LEN_FIX_RATE:
        case VOC_PB_VAR_LEN_VAR_RATE:
        case VOC_PB_FIX_LEN_VAR_RATE:
          ERR ("Format not supported!!!!", 0, 0, 0);
          break;
      
        default:
    
          ERR_FATAL ("Illegal frame format!!!!", 0, 0, 0);
          break; /* end default case */
      } /* end switch (vsp_rec_parm.settings.frame_format) */
      
      #ifdef FEATURE_MM_REC
      if (vsp_rec_parm.sync_cb_func != NULL) {

        if ((vsp_rec_data_req_cnt1 == 0)     &&
            (vsp_rec_data_req_cnt2 == 0)     &&
            (vsp_rec_av_sync_stamp == FALSE) &&
            (vsp_rec_parm.sync_cb_func != NULL)) {/*first report as timestamp*/
          
          vsp_rec_data_req_cnt2++;
          vsp_rec_data_frame_index++;

        vocsup_rec_do_av_sync();
          rec_data = TRUE;
          vsp_rec_av_sync_stamp = TRUE;
        } else if (vsp_rec_parm.sync_cb_func != NULL) {

          vsp_rec_data_req_cnt2++;
          vsp_rec_data_frame_index++;

          if ((vsp_rec_data_req_cnt2 - vsp_rec_data_req_cnt1) >= 
              vsp_rec_data_req_frm_cnt) {

            vocsup_rec_do_av_sync();
            rec_data = TRUE;
		  }
		 }

        if (rec_data == TRUE) {

          /* Fill out return struct. */
          if(dir == VOC_REC_FORWARD) {
            rec_result->fwd_ptr = vsp_curr_rec_hdr;
          } else {
            rec_result->rev_ptr = vsp_curr_rec_hdr;
          }
          rec_result->status  = VOC_REC_STAT_DATA;

          vsp_rec_data_req_cnt1 = vsp_rec_data_req_cnt2;
        }
      }
#endif /* FEATURE_MM_REC */
      if ((vsp_curr_rec_hdr->rec_buffer_size - vsp_curr_rec_hdr->rec_length) < 
          36) {
        /* Fill out return struct. */
        if(dir == VOC_REC_FORWARD) {
          rec_result->fwd_ptr = vsp_curr_rec_hdr;
        } else {
          rec_result->rev_ptr = vsp_curr_rec_hdr;
        }
#ifdef FEATURE_MM_REC
        if (vsp_rec_parm.sync_cb_func != NULL){

          vocsup_rec_do_av_sync();
        }
#endif /* FEATURE_MM_REC */
        rec_result->status  = VOC_REC_STAT_DATA;
      }

      /* Check if record interval has been reached.
      */
      if((vsp_interval_frm > 0) &&
         ((vsp_rec_parm.rec_type != VOC_REC_BOTH) || 
          (dir == VOC_REC_FORWARD)) && 
         (++vsp_current_frm == vsp_interval_frm)) {
        rec_result->status = VOC_REC_STAT_INT_TIME;

        /* Reset record interval */
        vsp_current_frm = 0;
      }
    } /* end if (rec_result->status != VOC_REC_STAT_ERROR) */
  } /* end if (vsp_error_reported == FALSE) */

  vocsup_rec_free_frame_buf(data_ptr);

} /* end vocsup_rec_frame */

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
void vocsup_rec_start (
  /* Recording parameters. */
  voc_rec_parm_type          *voc_rec_parm
) {
  /* Save call back interval (in 20 ms frames).
  */
  vsp_interval_frm = (voc_rec_parm->rec_interval_ms / 20);

  /* Start the frame count.
  */
  vsp_current_frm = 0;

  /* Initialize current header ptr */
  vocsup_rec_reset_hdr(&vsp_rec_tx_hdr);
  vocsup_rec_reset_hdr(&vsp_rec_rx_hdr);

  /* Save recording parameters.
  */
  vsp_rec_parm = *voc_rec_parm;

  /* Initialize error report flag. 
  */
  vsp_error_reported = FALSE;

  #ifdef FEATURE_MM_REC
  if ((vsp_rec_parm.sync_cb_func != NULL) &&
      (vsp_rec_parm.data_req_ms >= VSP_REC_FRAME_INTERVAL)) {

    /* in unit of 20 millisecond */
    vsp_rec_data_req_frm_cnt    = (vsp_rec_parm.data_req_ms / 
                                   VSP_REC_FRAME_INTERVAL);
    vsp_rec_data_req_cnt1       = 0;
    vsp_rec_data_req_cnt2       = 0;
    vsp_rec_data_frame_index    = 0;
    vsp_rec_av_sync_stamp       = FALSE;
  }
#endif /* FEATURE_MM_REC */
} /* end vocsup_rec_start */

/* <EJECT> */
/*===========================================================================

FUNCTION VOCSUP_REC_SYNC_UP

DESCRIPTION
  This function will return the pointer of the current buffer in the
  recording session.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Ptr to record result structure.

SIDE EFFECTS
  Update record buffer info.

===========================================================================*/
void vocsup_rec_sync_up (
  voc_rec_result_type *rec_result
) {
  rec_result->fwd_ptr = (voc_rec_header_type *) NULL;
  rec_result->rev_ptr = (voc_rec_header_type *) NULL;

  if (vsp_rec_rx_hdr.rec_data_ptr != NULL) {
    rec_result->fwd_ptr = &vsp_rec_rx_hdr;
  }

  if (vsp_rec_tx_hdr.rec_data_ptr != NULL) {
    rec_result->rev_ptr = &vsp_rec_tx_hdr;
  }

#ifdef FEATURE_MM_REC
  if (vsp_rec_parm.sync_cb_func != NULL) {
    vocsup_rec_do_av_sync();
  }
#endif /* FEATURE_MM_REC */

  rec_result->status  = VOC_REC_STAT_DATA;

  /* Reset record interval */
  vsp_current_frm = 0;
}
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION voc_get_shared_mem_ptr

DESCRIPTION
  This function returns a pointer to the shared RAM area between the vocoder
  driver and the VR engine.  The vocoder shared RAM area is limited to two
  buffers used in voice recording.  The purpose of sharing RAM is to eliminate
  RAM usage for the recording buffers.  By sharing RAM, voice recording (for
  voice memo only at this point) and voice recognition can not be running 
  concurrently.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  Pointer to shared RAM area.

SIDE EFFECTS
  None

===========================================================================*/

void *voc_get_shared_mem_ptr (void)
{
  return (&vsp_shared_mem);
}
#endif /* FEATURE_PHONE_VR */

/* VOCSUP.C */

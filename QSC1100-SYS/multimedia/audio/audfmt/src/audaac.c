/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audaac Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   A A C   S E R V I C E S 

GENERAL DESCRIPTION
  This module contains AudAAC services software.

EXTERNALIZED FUNCTIONS
  audaac_format_play
    Use this function to play streaming PV AAC.
  audaac_parse_head
    Parses the aac header for relevant infomation.
  audaac_get_data_cb_req
    Callback function which handles the response to a Data request.
  audaac_get_sample_rate
    Returns the sampling rate of the current aac song.
  audaac_flush
    Flushes the buffers and restarts playback.
  audaac_fforward
    Moves the AAC playback forward ms milliseconds.
  audaac_rewind
    Moves the AAC playback backward ms milliseconds.
  audaac_reset
    Resets internal AAC buffers after a song is unloaded.
  audaac_get_length
    Returns the total duration of the selected AAC file.
  audaac_get_song_config
    Returns song-specific information to optimize AAC playback.
  audaac_adec_adjust
    Adjusts adec buffers to reflect their newly used data.
  audaac_parse
    Parses one event of the AAC file.  Prepares buffers for data transfer.
  audaac_age
    Checks to see if the buffers are ready for use.

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES
  ISO/IEC 13818-7:1997(E)
  ISO/IEC 14496-3:2001(E)
  
Copyright(c) 2002 - 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac.c#3 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/17/09    kk     Fixed CR 182369 - Small(less than 1 sec)AAC file not played.
07/07/08    kk     Fixed  CR 148758 CMX play back crashes after get_specs.
04/16/08    hs     Fixed to support higher sampling rate
04/03/08    kk     Fixed CR 141303 - Problems in ffwarding/rewiding when 
                   playing special AAC file.
02/27/08    kk     Fixed  CR 130620 - Media Player can not handle the specific
                   AAC file in pause/resume
07/27/07    kk     Fixed CR 124514 - Audio: Error Fatal dog BCDB 0 crash 
                   observed during FF and Rew .
05/21/07    az     Fixed the CR: 115681 Crash observed while playing AAC file 
                   through automation. 
05/17/07    kk     Modified fix for CR 114230.
05/11/07    hs     Fixed CR 117907 - crash and memory leak
05/09/07    kk     Fixed CRCR 114230 - Media Player doesn't recognize
                   a very short (32 ms) AAC clip 
03/13/07    az     Fixed the warning in audaac_calc_byte_offset ()
03/05/07    az     Fixed the CR: 112175 memory leak when playing MP3(ID3v2) and
                   AAC files.
02/27/07    kk     Added support for AAC BSAC audio
11/09/06    kk     Fixed CR 104372 - CMX: During Auto play for one 
                   particular AAC clip file doesn't play when selected.
10/09/06    kk     fixed,sending wrong sampling freq index for AAC files
08/15/06    sg     Modified to fix CR99003 where the parsers where not
                   accounting for partial buffers and failed sync reads.
08/01/06    hz     Fixed a mis-merging bug which causes VBR FFWD and 
                   continous playback error.
07/28/06    hz     Added support for AAC ADTS/VBR song length.
07/20/05    sp     Added support for AAC LOAS.
07/19/05    pl     Modified audaac_parse_head() to initialize error status to 
                   AUDMAIN_STATUS_MAX meaning that no error has been found.
07/08/05    pl     Modified audaac_do_file_spec() to set supported operation
                   bit mask.
05/24/05    hz     Modified to adapt AAC recording change. Added ADTS format.
05/16/05    sg     Modified to not return elapsed time greater than file
                   duration if seek is attempted beyond the file (CR63283).
            ac     Revised audaac_parse_head to include padding information for 
                   ADIF files (CR54442).
04/23/05    ac     Revised the audaac_find_sync() to do the following:
                   Include a boolean argument for the first buffer. 
                   Algo to parse second buffer was revisted to fix bugs 
03/28/05    aw     Modified code to ignore Target Head for streaming playback
                   CR#58007.
11/19/04    sg     Modified to check if frame_length is valid. Modified to 
                   check for non-zero bitrate before it is used in length
                   calculations.
09/07/04    sg     Modified audaac_find_sync() to scan an additional buffer if
                   there's insufficient data to find a 2nd sync word in the
                   first buffer.
08/27/04    sg     Added additional checks to find_sync() so that AAC files
                   are detected correctly.
08/11/04    sg     Modified to use macros when copying ID3 tag strings.
04/23/04    lg     Modified code to support Animated AAC Ringers.
04/23/04    sg     Modified to fail sync if data is invalid or not supported.
                   Modified to reject fforward/rewind if song_len is unknown.
03/05/04    sg     Modified audaac_calc_byte_offset() to handle overflows.
11/23/03    sg     Modified song length calculation to prevent overflow errors.
10/01/03    sg     Added additional restrictions for epConfig.
09/19/03    sg     Modified to support additional parameters required by the
                   DSP. Added support for a 'psuedo-raw' mode.
08/11/03    sg     Allow playback of streams when num channels is unknown.
03/13/03    sg     Added support to play files which have a bunch of zeroes at
                   the beginning of the song.
02/04/03    sg     Modified to support ID=0 in ADTS header.
07/25/02    sg     Added support for ADIF format.
06/18/02    sg     Reduced MSG level to HIGH for invalid file message.
06/17/02    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "customer.h"           /* Customer specific definitions           */
#ifdef FEATURE_AAC
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */

#include "audmain.h"            /* Audio Format Data definitions and protos*/
#include "audaac.h"             /* AAC typedefs and prototypes             */
#include "audaaci.h"            /* AAC internal typedefs and prototypes    */
#include "audmpeg.h"            /* MPEG typedefs and prototypes            */
#include "snd.h"
#include "qw.h"                 /* Quadword typedefs and prototypes       */

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDAAC General Defines
**---------------------------------------------------------------------------
*/
/* This structure contains all the information about a AAC file
** that is being parsed.
*/
audmpeg_parse_ctl_type audaac_parse_ctl;

/* aac_info contains all relevant information about the currently playing AAC
** file, extracted from the header.
*/
aac_info_type aac_info;

/* Each AAC frame contains sync data and unchanging header data which can be
** searched for, to more accurately locate the next sync.
*/
byte aac_frame_data[4];
boolean aac_frame_data_found = FALSE;

uint16 aac_frame_length = 0;

uint16 aac_adif_hdr_bit_index;

/* samplerate_table[sampling_frequency_index] */
const snd_sample_rate_type aac_samplerate_table[] = {
  SND_SAMPLE_RATE_96000,
  SND_SAMPLE_RATE_88200,
  SND_SAMPLE_RATE_64000,
  SND_SAMPLE_RATE_48000,
  SND_SAMPLE_RATE_44100,
  SND_SAMPLE_RATE_32000,
  SND_SAMPLE_RATE_24000,
  SND_SAMPLE_RATE_22050,
  SND_SAMPLE_RATE_16000,
  SND_SAMPLE_RATE_12000,
  SND_SAMPLE_RATE_11025,
  SND_SAMPLE_RATE_8000,

  SND_SAMPLE_RATE_UNKNOWN,  /* Reserved Sampling frequency */
  SND_SAMPLE_RATE_UNKNOWN,  /* Reserved Sampling frequency */
  SND_SAMPLE_RATE_UNKNOWN,  /* Reserved Sampling frequency */
  SND_SAMPLE_RATE_UNKNOWN   /* Reserved Sampling frequency */
};

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudAAC Macro definitions
**---------------------------------------------------------------------------
*/

#define IS_ADIF(x) \
   (\
      (x)[0] == 'A' && \
      (x)[1] == 'D' && \
      (x)[2] == 'I' && \
      (x)[3] == 'F'    \
   )
     
#define IS_SYNC(x) \
     (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF6) == 0xF0)
     
#define IS_VALID_FRAME(x) \
   (\
      (x)[0] == aac_frame_data[0] && \
      (x)[1] == aac_frame_data[1] && \
      (x)[2] == aac_frame_data[2] && \
      ( (x)[3] & 0xF0 ) == aac_frame_data[3] \
   )
#ifdef FEATURE_AAC_FORMAT_LOAS
   
#define IS_LOAS(x) \
      (((byte *) (x))[0] == 0x56 && (((byte *)(x))[1] & 0xE0) == 0xE0)

#define LOAS_IS_AUD_OBJ_SUPPORTED(x) \
        ((x == 2) || (x == 4) || (x == 5) || (x == 17))

#define LOAS_IS_SFI_SUPPORTED(x) ((x >= 3) && (x <= 0x0B))

/* c is channel config and o is Audio object type */
#define LOAS_IS_CHANNEL_CONFIG_SUPPORTED(c, o) \
        (((c <= 2) && ((o == 2) || (o == 4) || (o == 5))) || \
         (((c == 1) || (c == 2)) && (o == 17)))

#define LOAS_IS_EXT_SFI_SUPPORTED(x)  ((x >= 0x03) && (x <= 0x08))

/* Extension Flag is e and Audio object type is o */
#define LOAS_IS_EXT_FLG_SUPPORTED(e,o) \
        ((((o == 2) || (o == 4) || (o == 5)) && (e == 0)) || \
         ((o == 17) && (e == 1)))

#define LOAS_GA_SPECIFIC_CONFIG(o) \
        (((o != 5) && (o >=1 ) && (o <= 7)) || \
         ((o != 18) && (o >= 17) && (o <= 23)))

#endif /* FEATURE_AAC_FORMAT_LOAS */
   
#define AAC_EOF_PADDING_BUFFER_SIZE 1500
   
/* <EJECT> */
/*---------------------------------------------------------------------------
** AudAAC Internal Function definitions
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audaac_extract_bits

DESCRIPTION
  This function extracts the required number of bits from the input buffer.
  Note that this function is limited to extracting 32 bits at a time.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates aac_adif_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_extract_bits(
  byte   *input,
  byte   num_bits_reqd,
  void   *out_buf
)
{
  uint32 output = 0;
  uint32 value = 0;
  uint32 byte_index;
  byte   bit_index;
  byte   bits_avail_in_byte;
  byte   num_to_copy;
  byte   mask;
  byte   num_remaining = num_bits_reqd;

  while(num_remaining) {
    byte_index = aac_adif_hdr_bit_index / 8;
    bit_index  = aac_adif_hdr_bit_index % 8;

    bits_avail_in_byte = 8 - bit_index;
    num_to_copy = MIN(bits_avail_in_byte, num_remaining);

    mask = ~(0xff << bits_avail_in_byte);

    value = input[byte_index] & mask;
    value = value >> (bits_avail_in_byte - num_to_copy);

    aac_adif_hdr_bit_index += num_to_copy;
    num_remaining -= num_to_copy;

    output = (output << num_to_copy) | value;
  }

  if(num_bits_reqd <= 8) {
    uint8 *buf8 = out_buf;
    *buf8 = (uint8)output;
  }
  else if(num_bits_reqd <= 16) {
    uint16 *buf16 = out_buf;
    *buf16 = (uint16)output;
  }
  else if(num_bits_reqd <= 32) {
    uint32 *buf32 = out_buf;
    *buf32 = (uint32)output;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_extract_adif_header

DESCRIPTION
  This function extracts the ADIF header from the start of data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audaac_extract_adif_header(
  byte            *data
)
{
  uint8  buf8;
  uint32 buf32;
  byte   num_pfe, num_fce, num_sce, num_bce, num_lfe, num_ade, num_vce;
  byte   pfe_index;
  byte   i;

  /* We already parsed the 'ADIF' keyword. Skip it. */
  aac_adif_hdr_bit_index = 32;

  aac_info.hdr.layer = SND_MPEG2_LAYER_AAC;

  audaac_extract_bits(data, AAC_COPYRIGHT_PRESENT_SIZE, &buf8);

  if(buf8) {
    /* Copyright present; Just discard it for now */
    aac_adif_hdr_bit_index += 72;
  }

  audaac_extract_bits(data, AAC_ORIGINAL_COPY_SIZE,
                      &aac_info.hdr.original_flag);

  audaac_extract_bits(data, AAC_HOME_SIZE, &aac_info.hdr.home_flag);

  audaac_extract_bits(data, AAC_BITSTREAM_TYPE_SIZE,
                      &aac_info.hdr.variable_bitrate);

  audaac_extract_bits(data, AAC_BITRATE_SIZE, &aac_info.hdr.bit_rate);

  audaac_extract_bits(data, AAC_NUM_PFE_SIZE, &num_pfe);

  for(pfe_index=0; pfe_index<num_pfe+1; pfe_index++) {
    if(!aac_info.hdr.variable_bitrate) {
      /* Discard */
      audaac_extract_bits(data, AAC_BUFFER_FULLNESS_SIZE, &buf32);
    }

    /* Extract Program Config Element */

    /* Discard element instance tag */
    audaac_extract_bits(data, AAC_ELEMENT_INSTANCE_TAG_SIZE, &buf8);

    audaac_extract_bits(data, AAC_PROFILE_SIZE, &aac_info.config.audio_object);
    if (aac_info.config.audio_object == 1)
    {
      /* We'll use the MPEG4 object type instead of the MPEG2 profile, since
       * the DSP prefers it that way */
      aac_info.config.audio_object = SND_MP4_AUDIO_OBJECT_AAC_LC;
    }
    else
    {
      aac_info.config.audio_object = SND_MP4_AUDIO_OBJECT_UNKNOWN;
    }

    audaac_extract_bits(data, AAC_SAMPLING_FREQ_INDEX_SIZE, &buf8);
    aac_info.config.sample_freq_index = buf8;
    aac_info.hdr.samplerate = aac_samplerate_table[buf8];

    audaac_extract_bits(data, AAC_NUM_FRONT_CHANNEL_ELEMENTS_SIZE, &num_fce);

    audaac_extract_bits(data, AAC_NUM_SIDE_CHANNEL_ELEMENTS_SIZE, &num_sce);

    audaac_extract_bits(data, AAC_NUM_BACK_CHANNEL_ELEMENTS_SIZE, &num_bce);

    audaac_extract_bits(data, AAC_NUM_LFE_CHANNEL_ELEMENTS_SIZE, &num_lfe);

    audaac_extract_bits(data, AAC_NUM_ASSOC_DATA_ELEMENTS_SIZE, &num_ade);

    audaac_extract_bits(data, AAC_NUM_VALID_CC_ELEMENTS_SIZE, &num_vce);

    audaac_extract_bits(data, AAC_MONO_MIXDOWN_PRESENT_SIZE, &buf8);
    if(buf8) {
      audaac_extract_bits(data, AAC_MONO_MIXDOWN_ELEMENT_SIZE, &buf8);
    }

    audaac_extract_bits(data, AAC_STEREO_MIXDOWN_PRESENT_SIZE, &buf8);
    if(buf8) {
      audaac_extract_bits(data, AAC_STEREO_MIXDOWN_ELEMENT_SIZE, &buf8);
    }

    audaac_extract_bits(data, AAC_MATRIX_MIXDOWN_PRESENT_SIZE, &buf8);
    if(buf8) {
      audaac_extract_bits(data, AAC_MATRIX_MIXDOWN_SIZE, &buf8);
    }

    for(i=0; i<num_fce; i++) {
      audaac_extract_bits(data, AAC_FCE_SIZE, &buf8);
    }

    for(i=0; i<num_sce; i++) {
      audaac_extract_bits(data, AAC_SCE_SIZE, &buf8);
    }

    for(i=0; i<num_bce; i++) {
      audaac_extract_bits(data, AAC_BCE_SIZE, &buf8);
    }

    for(i=0; i<num_lfe; i++) {
      audaac_extract_bits(data, AAC_LFE_SIZE, &buf8);
    }

    for(i=0; i<num_ade; i++) {
      audaac_extract_bits(data, AAC_ADE_SIZE, &buf8);
    }

    for(i=0; i<num_vce; i++) {
      audaac_extract_bits(data, AAC_VCE_SIZE, &buf8);
    }

    /* byte_alignment() */
    buf8 = (byte)(aac_adif_hdr_bit_index % 8);
    if(buf8) {
      aac_adif_hdr_bit_index += 8 - buf8;
    }

    /* get comment_field_bytes */
    buf8 = data[aac_adif_hdr_bit_index/8];

    aac_adif_hdr_bit_index += AAC_COMMENT_FIELD_BYTES_SIZE;

    /* Skip the comment */
    aac_adif_hdr_bit_index += buf8 * AAC_COMMENT_FIELD_DATA_SIZE;
  }

  /* byte_alignment() */
  buf8 = (byte)(aac_adif_hdr_bit_index % 8);
  if(buf8) {
    aac_adif_hdr_bit_index += 8 - buf8;
  }

  audaac_parse_ctl.first_sync += aac_adif_hdr_bit_index/8;

  aac_info.config.channel_configuration = num_fce + num_sce + num_bce +
                                          num_lfe + num_ade + num_vce;

  aac_frame_data_found = TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_extract_loas_header

DESCRIPTION
  This function extracts the LOAS header from the start of data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void audaac_extract_loas_header(
  byte            *data
)
{
#ifdef FEATURE_AAC_FORMAT_LOAS
  uint32      value    = 0;
  uint8       obj_type = 0;
  uint8       ext_flag = 0;

  uint8       num_fc   = 0;
  uint8       num_sc   = 0;
  uint8       num_bc   = 0;
  uint8       num_lfe  = 0;
  uint8       num_ade  = 0;
  uint8       num_vce  = 0;

  uint16      num_bits_to_skip = 0;

  aac_header_type *hdr_ptr = &aac_info.hdr;

  aac_frame_data_found = FALSE;
  /* Length is in the bits 11 to 23 from left in the bitstream */

  aac_adif_hdr_bit_index = 11;
  audaac_extract_bits(data, 13, &aac_frame_length);

  /* useSameStreamMux */
  audaac_extract_bits(data, 1, &value);

  if(value)
  {
    return;
  }
  /* Audio mux version */
  audaac_extract_bits(data, 1, &value);
  if (value)
  {
    /* Unsupported format */
    return;
  }
  /* allStreamsSameTimeFraming */
  audaac_extract_bits(data, 1, &value);
  if (!value)
  {
    /* Unsupported format */
    return;
  }
  /* numSubFrames */
  audaac_extract_bits(data, 6, &value);
  if (value)
  {
    /* Unsupported format */
    return;
  }
  /* numProgram */
  audaac_extract_bits(data, 4, &value);
  if (value)
  {
    /* Unsupported format */
    return;
  }
  /* numLayer */
  audaac_extract_bits(data, 3, &value);
  if (value)
  {
    /* Unsupported format */
    return;
  }

  /* Audio specific config */
  /* audioObjectType */
  audaac_extract_bits(data, 5, &value);
  if (!LOAS_IS_AUD_OBJ_SUPPORTED(value))
  {
    /* Unsupported format */
    return;
  }
  aac_info.config.audio_object = value;

  /* SFI */
  audaac_extract_bits(data, 4, &value);
  if (!LOAS_IS_SFI_SUPPORTED(value))
  {
    /* Unsupported format */
    return;
  }
  aac_info.config.sample_freq_index = value;

  hdr_ptr->samplerate = aac_samplerate_table[value];
  MSG_ERROR("LOAS Sample rate = %d \n", hdr_ptr->samplerate, 0,0);
  hdr_ptr->bit_rate = 
    (8 * aac_frame_length *
     audmpeg_samplerate_values_table[hdr_ptr->samplerate]) /
    AAC_SAMPLES_PER_DATA_BLOCK;

  /* Channel config */
  audaac_extract_bits(data, 4, &value);
  if (!LOAS_IS_CHANNEL_CONFIG_SUPPORTED(value, aac_info.config.audio_object))
  {
    /* Unsupported format */
    return;
  }
  aac_info.config.channel_configuration = value;

  if (aac_info.config.audio_object == 5)
  {
    /* Extension SFI */
    audaac_extract_bits(data, 4, &value);
    if (!LOAS_IS_EXT_SFI_SUPPORTED(value))
    {
      /* Unsupported format */
      return;
    }
    /* Audio object_type */
    audaac_extract_bits(data, 5, &value);
    if (!LOAS_IS_AUD_OBJ_SUPPORTED(value))
    {
      /* Unsupported format */
      return;
    }
    aac_info.config.audio_object = value;
  }
  obj_type = aac_info.config.audio_object;

  if (LOAS_GA_SPECIFIC_CONFIG(obj_type))
  {
    /* framelengthFlag */
    audaac_extract_bits(data, 1, &value);
    if (value)
    {
      /* Unsupported format */
      return;
    }

    /* dependsOnCoreCoder */
    audaac_extract_bits(data, 1, &value);
    if (value)
    {
      /* Unsupported format */
      return;
    }

    /* extensionFlag */
    audaac_extract_bits(data, 1, &ext_flag);
    if (!LOAS_IS_EXT_FLG_SUPPORTED(ext_flag,obj_type) )
    {
      /* Unsupported format */
      return;
    }
    if (!aac_info.config.channel_configuration)
    {
      /* Skip 10 bits */
      audaac_extract_bits(data, 10, &value);

      audaac_extract_bits(data, 4, &num_fc);
      audaac_extract_bits(data, 4, &num_sc);
      audaac_extract_bits(data, 4, &num_bc);
      audaac_extract_bits(data, 2, &num_lfe);
      audaac_extract_bits(data, 3, &num_ade);
      audaac_extract_bits(data, 4, &num_vce);

      /* mono_mixdown_present */
      audaac_extract_bits(data, 1, &value);
      if (value) {
        /* mono_mixdown_element_number */
        audaac_extract_bits(data, 4, &value);
      }

      /* stereo_mixdown_present */
      audaac_extract_bits(data, 1, &value);
      if (value) {
        /* stereo_mixdown_element_number */
        audaac_extract_bits(data, 4, &value);
      }

      /* matrix_mixdown_idx_present */
      audaac_extract_bits(data, 1, &value);
      if (value) {
        /* matrix_mixdown_idx and presudo_surround_enable */
        audaac_extract_bits(data, 3, &value);
      }
      num_bits_to_skip = (num_fc * 5) + (num_sc * 5) + (num_bc * 5) + 
                         (num_lfe * 4) + (num_ade * 4) + (num_vce * 5);
      while (num_bits_to_skip != 0){
        if (num_bits_to_skip > 32) {
          audaac_extract_bits(data, 32, &value);
          num_bits_to_skip -= 32;
        } else {
          audaac_extract_bits(data, num_bits_to_skip, &value);
          num_bits_to_skip = 0;
        }
      }

     if (aac_adif_hdr_bit_index & 0x07)
     {
       aac_adif_hdr_bit_index +=  (8 - (aac_adif_hdr_bit_index & 0x07));
     }
      /* comment_field_bytes */
      audaac_extract_bits(data, 8, &value);

      num_bits_to_skip = value * 8;

      while (num_bits_to_skip != 0){
        if (num_bits_to_skip > 32) {
          audaac_extract_bits(data, 32, &value);
          num_bits_to_skip -= 32;
        } else {
          audaac_extract_bits(data, num_bits_to_skip, &value);
          num_bits_to_skip = 0;
        }
      }
    }

    if (ext_flag)
    {
      if (((obj_type == 17) || (obj_type == 19) || 
           (obj_type == 20) || (obj_type == 23)))
      {
        audaac_extract_bits(data, 1, &value);
        aac_info.config.aac_section_data_resilience_flag = value;

        audaac_extract_bits(data, 1, &value);
        aac_info.config.aac_scalefactor_data_resilience_flag = value;

        audaac_extract_bits(data, 1, &value);
        aac_info.config.aac_spectral_data_resilience_flag = value;
      }
      /* extensionFlag3 */
      audaac_extract_bits(data, 1, &value);
    }
  }
  if ((obj_type != 18) && (obj_type >= 17) && (obj_type <= 27))
  {
    /* epConfig */
    audaac_extract_bits(data, 2, &value);
    if (value)
    {
      /* Unsupported format */
      return;
    }
    aac_info.config.ep_config = value;
  }
  /* Back in StreamMuxConfig */
  /* framelengthType */
  audaac_extract_bits(data, 3, &value);
  if (value)
  {
    /* Unsupported format */
    return;
  }
  aac_frame_data_found = TRUE;

#endif /* FEATURE_AAC_FORMAT_LOAS */
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_extract_adts_header

DESCRIPTION
  This function extracts the ADTS header from the start of data.Parses the 
  first six bytes of the passed data 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void audaac_extract_adts_header(
  byte            *data
)
{
  aac_header_type *hdr_ptr = &aac_info.hdr;

  /*
  hdr_ptr->version = (snd_aac_id_enum_type)
    ((data[AACHDR_VERSION_OFS] & AACHDR_VERSION_M) >> AACHDR_VERSION_SHIFT);
    */

  hdr_ptr->layer = (snd_mpeg_layer_enum_type)
    ((data[AACHDR_LAYER_OFS] & AACHDR_LAYER_M) >> AACHDR_LAYER_SHIFT);

  hdr_ptr->crc_flag = (boolean)
    ((data[AACHDR_CRC_OFS] & AACHDR_CRC_M) >> AACHDR_CRC_SHIFT);

  aac_info.config.audio_object = (snd_mp4_audio_object_enum_type)
    ((data[AACHDR_PROFILE_OFS] & AACHDR_PROFILE_M) >> AACHDR_PROFILE_SHIFT);

  if (aac_info.config.audio_object == 1)
  {
    /* We'll use the MPEG4 object type instead of the MPEG2 profile, since
     * the DSP prefers it that way */
    aac_info.config.audio_object = SND_MP4_AUDIO_OBJECT_AAC_LC;
  }
  else
  {
    aac_info.config.audio_object = SND_MP4_AUDIO_OBJECT_UNKNOWN;
  }

  aac_info.config.sample_freq_index = (byte)
    ((data[AACHDR_SAMPLERATE_OFS] & AACHDR_SAMPLERATE_M) >>
      AACHDR_SAMPLERATE_SHIFT);

  hdr_ptr->samplerate = aac_samplerate_table[aac_info.config.sample_freq_index];

  hdr_ptr->private_flag = (boolean)
    ((data[AACHDR_PRIVATE_OFS] & AACHDR_PRIVATE_M) >> AACHDR_PRIVATE_SHIFT);

  aac_info.config.channel_configuration = (snd_aac_channel_enum_type)
                  ( ( (data[2] & 0x01) << 2 ) | ( (data[3] & 0xC0) >> 6 ) );

  hdr_ptr->original_flag = (boolean)
    ((data[AACHDR_ORIGINAL_OFS] & AACHDR_ORIGINAL_M) >> AACHDR_ORIGINAL_SHIFT);

  hdr_ptr->home_flag = (boolean)
    ((data[AACHDR_HOME_OFS] & AACHDR_HOME_M) >> AACHDR_HOME_SHIFT);

  /* Don't really use this 
  hdr_ptr->copyright_bit = (byte)
    ((data[AACHDR_COPYRIGHT_BIT_OFS] & AACHDR_COPYRIGHT_BIT_M) >>
      AACHDR_COPYRIGHT_BIT_SHIFT);

  hdr_ptr->copyright_start = (byte)
    ((data[AACHDR_COPYRIGHT_START_OFS] & AACHDR_COPYRIGHT_START_M) >>
      AACHDR_COPYRIGHT_START_SHIFT);
  */

  aac_frame_length = (uint16)( (data[5] & 0xE0) >> 5 );
  aac_frame_length = aac_frame_length | ( (uint16)data[4] << 3 );
  aac_frame_length = aac_frame_length | ( (uint16)(data[3] & 0x03) << 11 );

  if (aac_frame_length) {
    /*
    hdr_ptr->num_raw_data_blocks = (byte)
      ((data[AACHDR_NUM_DATA_BLOCKS_OFS] & AACHDR_NUM_DATA_BLOCKS_M) >>
        AACHDR_NUM_DATA_BLOCKS_SHIFT);
    */

    /* aac_frame_data holds the unchanging area of the aac frame header that
    ** can be scanned for to find the next valid frame.  The sync is included.
    */
    aac_frame_data[0] = 0xFF;
    aac_frame_data[1] = data[1];
    aac_frame_data[2] = data[2];
    aac_frame_data[3] = data[3] & 0xF0;

    hdr_ptr->bit_rate = (8 * aac_frame_length *
                         audmpeg_samplerate_values_table[hdr_ptr->samplerate]) /
                         AAC_SAMPLES_PER_DATA_BLOCK;

    aac_frame_data_found = TRUE;
  }
  else {
    aac_frame_data_found = FALSE;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_supported_aac

DESCRIPTION
  This function checks the header of the AAC file to verify it is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the file is supported.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audaac_supported_aac()
{
  aac_header_type             *hdr_ptr       = &aac_info.hdr;
  audmain_aac_config_type     *config        = &aac_info.config;

  /* No need to check (hdr_ptr->layer == SND_MPEG2_LAYER_AAC) here:-
  ** already done in IS_SYNC() */

  return(
          ( ((config->aac_format == AUDMAIN_AAC_DATA_FORMAT_ADIF ||
              config->aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) &&
              config->audio_object == SND_MP4_AUDIO_OBJECT_AAC_LC
            ) ||
            (config->aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS &&
              ( config->audio_object == SND_MP4_AUDIO_OBJECT_AAC_LC ||
                config->audio_object == SND_MP4_AUDIO_OBJECT_AAC_LTP ||
                config->audio_object == SND_MP4_AUDIO_OBJECT_ER_AAC_LC
              )
            )
#ifdef FEATURE_AVS_BSAC
            ||  ( (config->aac_format == AUDMAIN_AAC_DATA_FORMAT_PSUEDO_RAW) &&
                 (config->audio_object == SND_MP4_AUDIO_OBJECT_BSAC)
                )
#endif /* FEATURE_AVS_BSAC */
          ) &&
          (hdr_ptr->samplerate >= SND_SAMPLE_RATE_8000) &&
          (hdr_ptr->samplerate <= SND_SAMPLE_RATE_96000) &&
          (config->channel_configuration <= SND_AAC_CHANNEL_DUAL) &&
          ( config->audio_object < SND_MP4_AUDIO_OBJECT_ER_AAC_LC ||
#ifdef FEATURE_AVS_BSAC
            config->audio_object == SND_MP4_AUDIO_OBJECT_BSAC ||
#endif /* FEATURE_AVS_BSAC */
          ( config->audio_object == SND_MP4_AUDIO_OBJECT_ER_AAC_LC &&
              config->ep_config == 0
            )
          )
        );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_calc_song_len_ms

DESCRIPTION
  This function calculates the length of the song in milliseconds.

DEPENDENCIES
  None.

RETURN VALUE
  length of the song in milliseconds

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audaac_calc_song_len_ms(
  aac_header_type  *hdr_ptr,
  uint32           song_len_bytes
)
{
  if (hdr_ptr->bit_rate) {
    song_len_bytes = song_len_bytes - (audaac_parse_ctl.first_sync +
           (aac_info.id3_tag.v1_tag_found ? AUDMPEG_ID3V1_TAG_LEN : 0) );

    if (hdr_ptr->bit_rate < 1000) {
      /* This arrangement of math operations takes care of underflow errors */
      return(((song_len_bytes*8)/(hdr_ptr->bit_rate)) * 1000);
    }
    else {
      /* This arrangement of math operations takes care of overflow errors */
      return((song_len_bytes*8)/(hdr_ptr->bit_rate/1000));
    }
  }
  return 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_do_file_spec

DESCRIPTION
  This function builds a sound file specifications buffer and transmits the
  data to the CMX layer through the playback callback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audaac_do_file_spec (
  snd_af_access_type          *handle
)
{
  snd_aac_spec_type snd_spec;

  snd_spec.file_type     = SND_CMX_AF_FILE_AAC;

  snd_spec.layer         = aac_info.hdr.layer;
  snd_spec.crc_f         = aac_info.hdr.crc_flag;
  snd_spec.audio_object  = aac_info.config.audio_object;
  snd_spec.sample_rate   = aac_info.hdr.samplerate;
  snd_spec.private_f     = aac_info.hdr.private_flag;
  snd_spec.channel       = aac_info.config.channel_configuration;
  snd_spec.original_f    = aac_info.hdr.original_flag;
  snd_spec.home_f        = aac_info.hdr.home_flag;

  snd_spec.bit_rate      = aac_info.hdr.bit_rate;

  if(aac_info.id3_tag.v1_tag_found) {
    audmpeg_id3v1_tag_type *id3_ptr = &aac_info.id3_tag.v1_tag;

    memcpy(snd_spec.title, (const char * PACKED)id3_ptr->title,
           AUDMPEG_ID3_TITLE_LENGTH);
    memcpy(snd_spec.artist, (const char * PACKED)id3_ptr->artist,
           AUDMPEG_ID3_ARTIST_LENGTH);
    memcpy(snd_spec.album, (const char * PACKED)id3_ptr->album,
           AUDMPEG_ID3_ALBUM_LENGTH);
    memcpy(snd_spec.year, (const char * PACKED)id3_ptr->year,
           AUDMPEG_ID3_YEAR_LENGTH);
    memcpy(snd_spec.comment, (char * PACKED)id3_ptr->comment,
           AUDMPEG_ID3_COMMENT_LENGTH);
    snd_spec.genre = id3_ptr->genre;
  } else {
    memset(snd_spec.title, 0, AUDMPEG_ID3_TITLE_LENGTH);
    memset(snd_spec.artist, 0, AUDMPEG_ID3_ARTIST_LENGTH);
    memset(snd_spec.album, 0, AUDMPEG_ID3_ALBUM_LENGTH);
    memset(snd_spec.year, 0, AUDMPEG_ID3_YEAR_LENGTH);
    memset(snd_spec.comment, 0, AUDMPEG_ID3_COMMENT_LENGTH);
    snd_spec.genre = 0;
  }
  if (aac_info.song_len_ms > 0) {
    switch (aac_info.config.aac_format) {
    case AUDMAIN_AAC_DATA_FORMAT_ADIF:
      snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_GETTIME;
      break;
      
    case AUDMAIN_AAC_DATA_FORMAT_ADTS:
    case AUDMAIN_AAC_DATA_FORMAT_RAW:  
    case AUDMAIN_AAC_DATA_FORMAT_PSUEDO_RAW:
    case AUDMAIN_AAC_DATA_FORMAT_LOAS:
      snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_NONE;
      break;
      
    default:
      MSG_ERROR("Invalid AAC format", 0,0,0);
      snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_NONE;
      break;
    }
  } else {
    /* Cannot seek and get time without knowing the length */
    snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_NONE;
  }

  audmain_do_play_cb( handle, SND_CMX_AF_AUDIO_SPEC, 0, (void *)&snd_spec );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_calc_byte_offset

DESCRIPTION
  This function calculates the byte offset within an AAC file given the AAC
  header information and the number of milliseconds to move in the file.

DEPENDENCIES
  aac_info must be initialized.

RETURN VALUE
  A byte offset into the file is returned, which can be used for forward or
  backward movement.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audaac_calc_byte_offset(
  int ms,                 /* Milliseconds to calculate for byte movement   */
  int song_pos_ms         /* Current position in MS of song                */
)
{
  int new_pos_ms;         /* Destination position in the song in MS        */
  qword  qw1 = {0, 0};
  qword  qw2 = {0, 0};
  uint32 num_frame_played = 0;
  uint32 seek_frame_num   = 0;
  uint32 target_frame_num = 0;
  uint32 ret_val = 0;

  /* this is required for small files to work fine*/
  if (!audaac_parse_ctl.file_pad_req) {
    /* reduce the file_len to the actual size */
    audaac_parse_ctl.file_len -= audaac_parse_ctl.file_pad_length;
    /* set for EOF padding */
    audaac_parse_ctl.file_pad_req = TRUE;
  }

  if (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) {

    /* This is REWIND operation, we need to calculate where to restart.
     * First, find out which frame is being playback right now */
    num_frame_played = 
      (song_pos_ms / 1000) * 
       (audmpeg_samplerate_values_table[aac_info.hdr.samplerate] / 1024);

    /* How many frames need to be rewind */
    seek_frame_num   = 
      (abs(ms) *
       (audmpeg_samplerate_values_table[aac_info.hdr.samplerate] / 1000)) / 
       1024;

    if (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_REWIND) {

      MSG_MED("ADTS rrwnd, song_pos_ms = %d, ms = %d, frame_played = %d",
             song_pos_ms, abs(ms), num_frame_played); 
 
      MSG_MED("ADTS rrwnd, seek_frame_num = %d, frame_num = %d",
              seek_frame_num,audaac_parse_ctl.vbr_parse.frame_num,0); 

      audaac_parse_ctl.ops &= ~AUDMPEG_PROC_OPS_PROCESSING_REWIND; 

      if (seek_frame_num >= num_frame_played) {
        target_frame_num = 0;
      } else {
        target_frame_num  = num_frame_played - seek_frame_num; 
      }

      MSG_MED("ADTS rrwnd target_frame_num = %d", target_frame_num,0,0);

      if ((target_frame_num == 0) &&
          (audaac_parse_ctl.vbr_parse.frame_pos)) {

        /* RRewind to the beginning of file */
        ret_val = audaac_parse_ctl.first_sync;
        
        /* begin logging frame information again */
        audaac_parse_ctl.vbr_parse.rrwnd_num = 0;
        audaac_parse_ctl.vbr_parse.frame_num = 0;
        audaac_parse_ctl.vbr_parse.vbr_recorded = audaac_parse_ctl.first_sync;

      } else if ((audaac_parse_ctl.vbr_parse.frame_num > target_frame_num) &&
                 (audaac_parse_ctl.vbr_parse.frame_pos[target_frame_num - 1])) {

        ret_val = audaac_parse_ctl.vbr_parse.frame_pos[target_frame_num - 1];

        audaac_parse_ctl.vbr_parse.rrwnd_num = 
          (audaac_parse_ctl.vbr_parse.frame_num - target_frame_num + 1);
      }

    } else if (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD) {

      MSG_MED("ADTS ffwd, song_pos_ms = %d, ms = %d, frame_played = %d",
             song_pos_ms, abs(ms), num_frame_played); 
 
      MSG_MED("ADTS ffwd, seek_frame_num = %d, frame_num = %d",
              seek_frame_num,audaac_parse_ctl.vbr_parse.frame_num,0); 

      target_frame_num = num_frame_played + seek_frame_num;

      MSG_MED("ADTS ffwd target_frame_num = %d", target_frame_num,0,0);

      if (target_frame_num <= audaac_parse_ctl.vbr_parse.frame_num) {

        audaac_parse_ctl.ops &= ~AUDMPEG_PROC_OPS_PROCESSING_FFWD;

        audaac_parse_ctl.vbr_parse.rrwnd_num = 
            (audaac_parse_ctl.vbr_parse.frame_num - target_frame_num);
        audaac_parse_ctl.vbr_parse.ffwd_num = 0;
        ret_val = audaac_parse_ctl.vbr_parse.frame_pos[target_frame_num - 1];

      } else {

       /* we need to iterate more and find out the target frame num. */
        audaac_parse_ctl.vbr_parse.ffwd_num =
          (target_frame_num - audaac_parse_ctl.vbr_parse.frame_num);
        audaac_parse_ctl.vbr_parse.rrwnd_num = 0;
        ret_val = 
          audaac_parse_ctl.vbr_parse.frame_pos 
                              [audaac_parse_ctl.vbr_parse.frame_num - 1];
      }
    }
  } else {
    /* Figure out where in the song we will move to.
    */
    new_pos_ms = song_pos_ms + ms;
    if(new_pos_ms < 0) {
      /* Start at the beginning */
      new_pos_ms = 0;
    }
    if(new_pos_ms > aac_info.song_len_ms) {
      /* Move to the end */
      return audaac_parse_ctl.file_len;
    }
  
    qw_set(qw1, 0, new_pos_ms);
    qw_mul(qw2, qw1, aac_info.hdr.bit_rate);
    qw_div(qw1, qw2, 8*1000);
  
    if (qw_hi(qw1)) {
      ret_val = audaac_parse_ctl.file_len;
    }
    else {
      ret_val = qw_lo(qw1);
    }
  
    if (ret_val > audaac_parse_ctl.file_len) {
      /* This condition may occur due to round-off errors */
      ret_val = audaac_parse_ctl.file_len;
    }
  }
  MSG_MED("audaac_calc_byte_offset ret_val = %d", ret_val,0,0);
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_find_sync

DESCRIPTION
  This function calculates the byte offset of the sync bits within an AAC
  file given the AAC buffer and the number of bytes contained within the
  buffer.

DEPENDENCIES
  The buffer must be at least 6 bytes long.

RETURN VALUE
  A byte offset into the buffer is returned, or an offset equal to the
  length of the buffer is returned if the sync is not found.

SIDE EFFECTS
  None.

===========================================================================*/
int audaac_find_sync(
  byte     **buffer_ptr_ptr,   /* Buffer to search through */
  uint32   buf_len,
  boolean  first_buffer
)
{
  uint32  min_len = 6;
  uint32  index = 0;
  boolean found_sync = FALSE;
  uint8   *buf2_p = NULL;
  uint32  next_frame_start;
  uint32  next_buf_max = 0;
  int32   next_frame_start_in_buf2 = 0;
  byte    *buffer = *buffer_ptr_ptr;
  uint32  next_buf_start = aac_info.id3_tag.v2_tag_length;
  /* Need a minimum number of bytes in the buffer to have a sync
  */
  if(buf_len < min_len) {
    return(buf_len);
  }

  /* Look through the buffer to find the sync bytes.
  */
  while (index <= (buf_len - min_len)) {
    if(IS_ADIF(buffer+index)) {
      /* ADIF data */
      aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_RAW;
      audaac_extract_adif_header(buffer+index);
      if(audaac_supported_aac()) {
        index = index + audaac_parse_ctl.first_sync;
        aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADIF;
        found_sync = TRUE;
        break;
      }
      else {
        found_sync = FALSE;
        break;
      }
    }
    else if(IS_SYNC(buffer+index)) {
      if (aac_frame_data_found) {
        /* We are trying to seek to a new location - reuse the sync info we
         * already have */
        if(IS_VALID_FRAME(buffer+index)) {
          aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
          found_sync = TRUE;
          break;
        }
      }
      else {
        audaac_extract_adts_header(buffer+index);
        if((aac_frame_data_found == TRUE) &&
           (aac_frame_length <= AAC_MAX_FRAME_SIZE) && audaac_supported_aac()) {
          /* Check if the remaining buffer size is large enough to
           * look for another sync */
          /* check for single frame files */
          if ((aac_frame_length == buf_len) && 
              ((buf_len + aac_info.id3_tag.v2_tag_length) == 
                                  audaac_parse_ctl.file_len)) {
            MSG_HIGH("File has one frame of length = %d",aac_frame_length,0,0); 
            aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
            found_sync = TRUE;
            break;
          }

          next_frame_start = index + aac_frame_length;
          if ((next_frame_start + min_len) < (buf_len - (min_len - 1))){
            if (IS_SYNC(buffer + next_frame_start) &&
                IS_VALID_FRAME(buffer + next_frame_start)) {
              aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
              found_sync = TRUE;
              break;
            } else if(IS_ID3V1_TAG(buffer + next_frame_start)) {
              MSG_HIGH("File has ID3V1 tag and one valid AAC frame ",0,0,0); 
              aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
              found_sync = TRUE;
              break;
            }
          }
          else {
            next_buf_start += (first_buffer==TRUE) ? buf_len - min_len
                               : index+1;
            next_buf_max = MIN(audaac_parse_ctl.file_len,
                               (buf_len + next_buf_start));
            /* Calculations for fetching buffer 2. adjusting next_frame_start
             * to absolute location in the file */
            next_frame_start += aac_info.id3_tag.v2_tag_length;
                               
            if (((next_frame_start + min_len) < next_buf_max) &&
                (buf2_p == NULL)) {

              if (first_buffer == FALSE ) {
                audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle,
                                   SND_CMX_AF_FREE_BUF, 0, buffer);
                /*adjust the index back to start of the buffer */
                 *buffer_ptr_ptr = NULL ;  
              }   
              if(audmain_get_sync_data(audaac_parse_ctl.audmpeg_handle,
                 next_buf_start, &buf2_p, &buf_len) != AUDMAIN_STATUS_SUCCESS) {
                found_sync = FALSE;
                break;
              }
              if (buf_len == 0) {
                found_sync = FALSE;
                break;
              }
              next_frame_start_in_buf2 = next_frame_start - next_buf_start;
              if (next_frame_start_in_buf2 < 0) {
                MSG_ERROR("Internal buffer error", 0, 0, 0);
                break;
              }
              if (( next_frame_start_in_buf2 < (buf_len - min_len)) &&  
                  IS_SYNC(buf2_p+next_frame_start_in_buf2) &&
                  IS_VALID_FRAME(buf2_p+next_frame_start_in_buf2)) {
                aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
                found_sync = TRUE;
                break;
              }
            }
            /* else false positive */
          }
        }
        /* Clear info, since we hit a false positive */
        aac_frame_data_found = FALSE;
      }
    }
#ifdef FEATURE_AAC_FORMAT_LOAS
    else if (IS_LOAS(buffer+index))
    {
      MSG_LOW("LOAS Sync found \n",0,0,0);
      if (!aac_frame_data_found) {
        aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_LOAS;
        audaac_extract_loas_header(buffer+index);
        if (aac_frame_data_found) {
          found_sync = TRUE;
          break;
        }
      } else {
        found_sync = TRUE;
        break;
      }
    }
#endif /* FEATURE_AAC_FORMAT_LOAS */
    index++;
  }

  if (buf2_p != NULL) {
    audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle,
                       SND_CMX_AF_FREE_BUF, 0, (void *)buf2_p);
  }

  if(found_sync == TRUE) {
    return(index);
  }
  return(buf_len);
}

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDAAC External Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audaac_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
boolean audaac_get_data_cb_req (
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
)
{
  uint32 parsed_size;
  uint32 temp_buf_size;
  uint8  *temp_buf = NULL;
  audmpeg_vbr_parse_type *vbr_parse;

  *data = buf;

  if (buf != NULL) {

    /* Check if you have recieved the correct buffer(buf_2) for the 
     * fast forward and Gettime functions */
    if ((((audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_GETTIME) &&
         (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_GETVBR)) ||
	    ((audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK) &&
         (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD))) && 
        (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS)) {
    
      if (&(audaac_parse_ctl.track_buf_1) == data) {
        MSG_HIGH("Got wroung buffer (buf_1)- discard ",0,0,0);
        audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle, 
                            SND_CMX_AF_FREE_BUF, 0, 
                          (void *)audaac_parse_ctl.track_buf_1);

        return (TRUE); 
      }
    }
	  
    /* First section is specifically for get_time
     * operation */
    if ((audaac_parse_ctl.ops & 
         AUDMPEG_PROC_OPS_PROCESSING_GETTIME) &&
        (audaac_parse_ctl.ops & 
         AUDMPEG_PROC_OPS_PROCESSING_GETVBR)) {
      
      audaac_parse_ctl.vbr_parse.total_read_len += length; 
      
      parsed_size = 
        audaac_adts_parse_frame(buf,
                                length,
                                FALSE);

      if (parsed_size > length) {
        audaac_parse_ctl.vbr_parse.total_read_len += 
        (parsed_size - length);
      } else if (parsed_size != 0) {
        audaac_parse_ctl.vbr_parse.total_read_len -= 
        (length - parsed_size);
      }
      
      if (audaac_parse_ctl.vbr_parse.total_read_len >=
          audaac_parse_ctl.file_len) {
  
        /* AAC one frame has 1024 samples, based on the clip frame rate and 
         * frame number, it is possible to calculate the total song length */
        aac_info.song_len_ms = 
          ((audaac_parse_ctl.vbr_parse.frame_num) * 1024) / 
          (audmpeg_samplerate_values_table[aac_info.hdr.samplerate] / 1000);
  
        MSG_MED("Song length is %d ms",aac_info.song_len_ms,0,0);
  
        audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_NONE;
  
        audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle, 
                           SND_CMX_AF_FREE_BUF, 
                           0, 
                           (void *)audaac_parse_ctl.track_buf_2);
  
        audmain_audfmt_do_time();
  
      } else  {
  
        audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle,
                           SND_CMX_AF_FREE_BUF, 
                           0, 
                           (void *)(audaac_parse_ctl.track_buf_2));
  
        audaac_parse_ctl.track_2_start = 
          audaac_parse_ctl.vbr_parse.total_read_len; 
        
        audmain_get_data(audaac_parse_ctl.audmpeg_handle, 
                         audaac_parse_ctl.track_2_start,
                         &(audaac_parse_ctl.track_buf_2));
      }
  
      return (TRUE);
    }
  
    /* During playback, three things need to be handled,
     * 1. recording the parsed frame offset to first_sync;
     * 2. dealing with rewind;
     * 3. dealing with forward.
     */
    if ((audaac_parse_ctl.ops & 
         AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK) && 
        (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS)) {
  
      vbr_parse  = &(audaac_parse_ctl.vbr_parse); 
  
      if (vbr_parse->failure != TRUE) {
  
        MSG_MED(" data_cb_req - leftover_size = %d, new_buf_len = %d "
                                      ,vbr_parse->leftover_size,length,0);
        if ((vbr_parse->leftover_size > 0) &&
            (vbr_parse->leftover != NULL) && (!(audaac_parse_ctl.ops & 
            AUDMPEG_PROC_OPS_PROCESSING_FFWD))) {
  
          if((vbr_parse->leftover_size + length) <= AUDAAC_ADTS_MIN_HEADER_LEN){
            /* FFworard approached the end of file */
            vbr_parse->ffwd_num   = 0;
            audaac_parse_ctl.file_location = audaac_parse_ctl.file_len;
            MSG_HIGH("Reached the end of file while FF",0,0,0);  
            /* free & clear the leftover bytes */
            free(vbr_parse->leftover); 
            vbr_parse->leftover = NULL;
            vbr_parse->leftover_size = 0;

          } else if((temp_buf = malloc(vbr_parse->leftover_size + length)) != NULL) {  
            memcpy(temp_buf, vbr_parse->leftover,vbr_parse->leftover_size); 
                            
            memcpy(temp_buf+vbr_parse->leftover_size,
                   buf,
                   length); 
            
            temp_buf_size = vbr_parse->leftover_size + length;
            free(vbr_parse->leftover); 
            vbr_parse->leftover = NULL;
            vbr_parse->leftover_size = 0;
   
            audaac_adts_parse_frame(temp_buf,
                                    temp_buf_size,
                                    TRUE);
            free(temp_buf);
          } else {
  
            vbr_parse->failure = TRUE;
            free(vbr_parse->leftover); 
            vbr_parse->leftover = NULL;
            vbr_parse->leftover_size = 0;
          }
  
        } else {
          /* Free the leftover bytes if in FForward */  
          if ((vbr_parse->leftover_size > 0) &&
                 (vbr_parse->leftover != NULL)) {
            free(vbr_parse->leftover); 
            vbr_parse->leftover = NULL;
            vbr_parse->leftover_size = 0; 
          }           
            audaac_adts_parse_frame(buf,
                                    length,
                                    TRUE);
        }

        /* FForward target frame number has not been found yet */
        if (audaac_parse_ctl.ops & 
            AUDMPEG_PROC_OPS_PROCESSING_FFWD) {
        
          if (audaac_parse_ctl.vbr_parse.ffwd_num == 0) {

            audaac_parse_ctl.ops  &= ~AUDMPEG_PROC_OPS_PROCESSING_FFWD;

            audmain_vbr_check_ffwd((void *)audaac_parse_ctl.audmpeg_handle, 
                                   audaac_parse_ctl.vbr_parse.state_type,
                                   buf,
                                   audaac_parse_ctl.vbr_parse.adjusted_ms,
                                   audaac_parse_ctl.vbr_parse.elapsed_ms,
                                   audaac_parse_ctl.vbr_parse.ms_forward);

            MSG_HIGH("FFWD is done, frame_num = %d",
                     audaac_parse_ctl.vbr_parse.frame_num,0,0);

            audaac_parse_ctl.vbr_parse.adjusted_ms = 0;
            audaac_parse_ctl.vbr_parse.elapsed_ms  = 0;
            audaac_parse_ctl.vbr_parse.ms_forward  = 0;

            audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle,
                               SND_CMX_AF_FREE_BUF, 
                               0, 
                               (void *)buf);

            audaac_parse_ctl.track_2_start = audaac_parse_ctl.file_location;

          } else {

            audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle, 
                               SND_CMX_AF_FREE_BUF, 
                               0, 
                               (void *)buf);

            audaac_parse_ctl.track_2_start = 
              audaac_parse_ctl.vbr_parse.frame_pos
                                  [audaac_parse_ctl.vbr_parse.frame_num - 1];

            audmain_get_data(audaac_parse_ctl.audmpeg_handle, 
                             audaac_parse_ctl.track_2_start,
                             &(audaac_parse_ctl.track_buf_2));

            MSG_HIGH("FFWD is continuing, frame_num = %d",
                     audaac_parse_ctl.vbr_parse.frame_num,0,0);
          }

          return (TRUE);
        } 
      }
    } /* during playback */
  }

  return audmpeg_get_data_cb_req(&audaac_parse_ctl, buf, length);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_format_play

DESCRIPTION
  Use this function to play streaming PV AAC.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audaac_format_play (
  snd_aac_format_spec_type    *input_spec,
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
) {
   aac_header_type             *aac_hdr_ptr   = &aac_info.hdr;
   audmain_aac_config_type     *config        = &aac_info.config;

   if(input_spec->codec_type.file_type != SND_CMX_AF_FILE_AAC) {
     MSG_LOW("Not an AAC file",0,0,0);
     return(AUDMAIN_STATUS_ERROR);
   }

   /* Reset internal parameters */
   audaac_reset();

   aac_hdr_ptr->layer            = SND_MPEG2_LAYER_AAC;
   aac_hdr_ptr->crc_flag         = FALSE;
   aac_hdr_ptr->samplerate       = input_spec->sample_rate;
   aac_hdr_ptr->private_flag     = FALSE;
   aac_hdr_ptr->original_flag    = FALSE;
   aac_hdr_ptr->home_flag        = FALSE;
   aac_hdr_ptr->bit_rate         = input_spec->bit_rate;

   config->aac_format            = input_spec->format;
   config->audio_object          = input_spec->audio_object;
   config->channel_configuration = input_spec->channel;
   config->ep_config             = input_spec->ep_config;

   config->aac_section_data_resilience_flag =
     input_spec->aac_section_data_resilience_flag;

   config->aac_scalefactor_data_resilience_flag =
     input_spec->aac_scalefactor_data_resilience_flag;

   config->aac_spectral_data_resilience_flag =
     input_spec->aac_spectral_data_resilience_flag;

   switch (input_spec->sample_rate) {
     case SND_SAMPLE_RATE_8000:
       config->sample_freq_index = 11;
       break;
     case SND_SAMPLE_RATE_11025:
       config->sample_freq_index = 10;
       break;
     case SND_SAMPLE_RATE_12000:
       config->sample_freq_index = 9;
       break;
     case SND_SAMPLE_RATE_16000:
       config->sample_freq_index = 8;
       break;
     case SND_SAMPLE_RATE_22050:
       config->sample_freq_index = 7;
       break;
     case SND_SAMPLE_RATE_24000:
       config->sample_freq_index = 6;
       break;
     case SND_SAMPLE_RATE_32000:
       config->sample_freq_index = 5;
       break;
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
     case SND_SAMPLE_RATE_44000:
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
     case SND_SAMPLE_RATE_44100:
       config->sample_freq_index = 4;
       break;
     case SND_SAMPLE_RATE_48000:
       config->sample_freq_index = 3;
       break;
     case SND_SAMPLE_RATE_64000:
       config->sample_freq_index = 2;
       break;
     case SND_SAMPLE_RATE_88200:
       config->sample_freq_index = 1;
       break;
     case SND_SAMPLE_RATE_96000:
       config->sample_freq_index = 0;
       break;
     default:
       MSG_ERROR("Unsupported Sampling Frequency in AAC file",0,0,0);
       return(AUDMAIN_STATUS_ERROR);
       break;
   }

   if(!audaac_supported_aac()) {
     MSG_ERROR("Unsupported features used in AAC file",0,0,0);
     return(AUDMAIN_STATUS_ERROR);
   }

   /* Clear the resync func, so that any attempt to resync will fail */
   audaac_parse_ctl.find_sync_func = NULL;

   /* Setup the buffers */
   audmpeg_setup_streaming(buffer, handle, parse_ctl, &audaac_parse_ctl);

   /* Allow the setup of the adec_buffer_factor */
   aac_frame_data_found = FALSE;

   MSG_MED("Streaming AAC ready to go", 0, 0, 0);
   return(AUDMAIN_STATUS_SUCCESS);
} /* end audaac_format_play */

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_parse_head

DESCRIPTION
  This function parses the aac header for relevant infomation and initializes
  the data structures necessary to play the file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audaac_parse_head (
  uint8                      *buffer,
  snd_af_access_type         *handle,
  audmain_parse_ctl_type     *parse_ctl,
  audmain_play_param_type    *param
) {

  audaac_reset();

  if (param->get_specs) {
    audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_GETSPEC;
  } else if (param->calc_time) {
    audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_GETTIME;
  } else {
    audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK;
  }

  if (!audmpeg_find_first_frame(buffer, handle, parse_ctl,
                                &audaac_parse_ctl, &aac_info.id3_tag)) {
    MSG_LOW("Not an AAC file",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  }

  if (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_RAW) {
    /* Don't pass the header info to the DSP. Advance the buffer to
     * point to the start of the raw data.
     */
    audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, 
                       (void *)audaac_parse_ctl.track_buf_1);

    audaac_parse_ctl.track_1_start = audaac_parse_ctl.first_sync;
    if(audmain_get_data(handle, audaac_parse_ctl.track_1_start,
                   &(audaac_parse_ctl.track_buf_1)) != AUDMAIN_STATUS_SUCCESS) {
      return(AUDMAIN_STATUS_FAILURE);
    }
    audaac_parse_ctl.file_location = audaac_parse_ctl.track_1_start +
                                     audaac_parse_ctl.track_1_length;

    /* Setup adec buffers */
    parse_ctl->current_buf.adec_cmd.adec_buf1 = audaac_parse_ctl.track_buf_1;
    parse_ctl->current_buf.adec_cmd.adec_num_buf1=
                                              audaac_parse_ctl.track_1_length;

    /* Clear the resync func, so that any attempt to resync will fail */
    audaac_parse_ctl.find_sync_func = NULL;
    
  }
    /* Mark for Padding 1500 bytes at the end of the file: This is required to
   * ensure that we force the DSP to decode any remaining portion of the ADIF/ADTS
     * stream, which it may not otherwise decode since the amount of remaining
     * data may not have been sufficient to trigger the decoder
     */
    audaac_parse_ctl.file_pad_req = TRUE;
  audaac_parse_ctl.file_pad_length = AAC_EOF_PADDING_BUFFER_SIZE;  

  /* Locate the ID3V1 tag and extract it, if available */
  audmpeg_load_id3v1_tag(handle, &audaac_parse_ctl, &aac_info.id3_tag);

  /* Calculate the song length, if a file size is known */
  if ((audaac_parse_ctl.file_len  != 0) &&
      (aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS)) {

    aac_info.song_len_ms = audaac_calc_song_len_ms(&aac_info.hdr,
                                                   audaac_parse_ctl.file_len);

  } 

  if ((audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK) &&
      (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS)) {

    /* Start to record the VBR frame index and location infomation */
    memset(&(audaac_parse_ctl.vbr_parse), 0, sizeof(audmpeg_vbr_parse_type));

    audaac_parse_ctl.vbr_parse.prealloc_index = 1;
    audaac_parse_ctl.vbr_parse.frame_pos  =  
      malloc(sizeof(uint32) * 
             (AUDMPEG_VBR_PRE_ALLOC_MEM * 
              audaac_parse_ctl.vbr_parse.prealloc_index));

    if (audaac_parse_ctl.vbr_parse.frame_pos != NULL) {
    audaac_parse_ctl.vbr_parse.vbr_recorded = audaac_parse_ctl.first_sync;
    audaac_adts_parse_frame(audaac_parse_ctl.track_buf_1,
                            (audaac_parse_ctl.file_location -
                             audaac_parse_ctl.track_1_start),
                            TRUE);
    } else { 
      audaac_parse_ctl.vbr_parse.failure = TRUE;
      MSG_ERROR("AAC ADTS, buffer pre-allocation failed",0,0,0);
    }

  }

  if ((audaac_parse_ctl.file_len == 0 || aac_info.song_len_ms == 0) &&
      ((aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS))) {
    aac_info.song_len_ms = 0;
    MSG_ERROR("Unable to calculate song length.  FF/REW Disabled.",0,0,0);
  }

  /* Create the SND/CMX file spec table, and return the data to 
   * the CMX layer */
  audaac_do_file_spec(handle);

  if (!(param->calc_time) && !(param->get_specs)) {
    /* This is essentially for playback */
    audmpeg_setup_track2(handle, parse_ctl, &audaac_parse_ctl);

    audaac_parse_ctl.error_status = AUDMAIN_STATUS_MAX;
    MSG_MED("AAC ready to go", 0, 0, 0);
  }

  if ((audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_GETTIME) &&
      (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS)) {
    /* For VBR and get_time or playback operation case, we don't need to 
     * look into the tag, since the whole frame number has to be found. 
     * 
     * Important: Before playback operation, get_time has to be invoked
     * to get the correct playback length for ADTS/VBR clips.
     * */

    audaac_parse_ctl.ops |= AUDMPEG_PROC_OPS_PROCESSING_GETVBR;
  
    audaac_parse_ctl.vbr_parse.total_read_len  =
      audaac_parse_ctl.first_sync;
  
    audaac_parse_ctl.vbr_parse.just_read_len   = 0;
    audaac_parse_ctl.vbr_parse.frame_num       = 0;
    audaac_parse_ctl.vbr_parse.vbr_recorded    = 
       audaac_parse_ctl.first_sync;
    audmain_do_play_cb(handle, 
                       SND_CMX_AF_FREE_BUF, 
                       0, 
                       (void *)audaac_parse_ctl.track_buf_2);

    audmain_set_read_sync(FALSE);
    
    audaac_parse_ctl.track_2_start = 
      audaac_parse_ctl.first_sync;
    
    audmain_get_data(handle, 
                     audaac_parse_ctl.track_2_start,
                     &(audaac_parse_ctl.track_buf_2));
  }

  return(AUDMAIN_STATUS_SUCCESS);
} /* end audaac_parse_head */

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current aac song.

DEPENDENCIES
  aac_info

RETURN VALUE
  sample rate of the last aac file parsed

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audaac_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
)
{
  return(aac_info.hdr.samplerate);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_flush (
  audmain_parse_ctl_type   *parse_ctl
)
{
  audmpeg_parse_ctl_type *aac_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_ERROR;
  uint32 dummy1 = 0;
  uint32 dummy2 = 0;
  
  if(parse_ctl != NULL) {
    aac_ptr = (audmpeg_parse_ctl_type*)parse_ctl->tracks;

    if (aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS) {
      aac_ptr->file_location = audaac_parse_ctl.first_sync;
    }
    aac_ptr->resync_state  = AUDMPEG_RESYNC_START;
    audmpeg_age(parse_ctl, &dummy1, &dummy2);
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_fforward

DESCRIPTION
  This function moves the AAC playback forward ms milliseconds.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds forward.

===========================================================================*/
audmain_status_type audaac_fforward (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
)
{
  audmpeg_parse_ctl_type *aac_ptr;
  uint32                 temp_ms_forward;
  
  temp_ms_forward = ms_forward;

  if(parse_ctl != NULL && temp_ms_forward > 0) {
    aac_ptr = (audmpeg_parse_ctl_type*)parse_ctl->tracks;

    if ((aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS &&
         aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_LOAS) ||
        ((aac_info.song_len_ms == 0) &&
         (aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS))) {
      return AUDMAIN_STATUS_FAILURE;
    }
    else {
      if (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) {

        /* If it is in the middle of fforwarding, forward time needs 
         * to be accumlated.
         */
        if (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD) {
          temp_ms_forward += audaac_parse_ctl.vbr_parse.ms_forward;
        } else {
          audaac_parse_ctl.ops |= AUDMPEG_PROC_OPS_PROCESSING_FFWD;
        }

        *elapsed_ms = (*elapsed_ms / 1000) * 1000;
      }

      aac_ptr->file_location = 
        audaac_calc_byte_offset(temp_ms_forward, *elapsed_ms);
    }

    aac_ptr->resync_state = AUDMPEG_RESYNC_START;
    if (aac_ptr->file_location >= audaac_parse_ctl.file_len) {

      *adjusted_ms = *elapsed_ms = aac_info.song_len_ms;

    } else if ((aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) && 
               (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD)) {

      /* This is "jump searching" category, we need to look further
       * to find out the target frame.
       */
      parse_ctl->status_flag |= AUDMAIN_MASK_FF_PAUSE; 

      /* Store all the time information for calculation and print
       * on ui screen.
       */
      audaac_parse_ctl.vbr_parse.state_type  = (void *)parse_ctl;
      audaac_parse_ctl.vbr_parse.adjusted_ms = *adjusted_ms;
      audaac_parse_ctl.vbr_parse.elapsed_ms  = *elapsed_ms;
      audaac_parse_ctl.vbr_parse.ms_forward  = temp_ms_forward;

      audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle,
                         SND_CMX_AF_FREE_BUF, 
                         0, 
                         audaac_parse_ctl.track_buf_2);

      audaac_parse_ctl.track_2_start = aac_ptr->file_location; 

      audmain_get_data(audaac_parse_ctl.audmpeg_handle, 
                       audaac_parse_ctl.track_2_start,
                       &(audaac_parse_ctl.track_buf_2));

    } else {

      *adjusted_ms = *elapsed_ms = *elapsed_ms + temp_ms_forward;
    }
  }

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rewind

DESCRIPTION
  This function moves the AAC playback backward ms milliseconds.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds backward.

===========================================================================*/
audmain_status_type audaac_rewind (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_rewind,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
)
{
  audmpeg_parse_ctl_type *aac_ptr;
  
  if(parse_ctl != NULL && ms_rewind > 0) {
    aac_ptr = (audmpeg_parse_ctl_type *) parse_ctl->tracks;

    if ((aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS &&
         aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_LOAS) ||
        ((aac_info.song_len_ms == 0) &&
         (aac_info.config.aac_format != AUDMAIN_AAC_DATA_FORMAT_ADTS))) {
      return AUDMAIN_STATUS_FAILURE;
    }
    else {
      if (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) {

        /* For ADTS VBR clip, rewind is very straightforward. Since we
         * have recorded every played frame's offset to first_sync till
         * now, just jump back to the target frame and get it from file.
         *
         * first thing to do is to stop any FForward 
         * operation if it is in the middle of FForwarding. 
         */
        if (audaac_parse_ctl.ops | AUDMPEG_PROC_OPS_PROCESSING_FFWD) {

          audaac_parse_ctl.ops &= ~AUDMPEG_PROC_OPS_PROCESSING_FFWD;
        }
        
        /* Then second set the Rewind bitmask. This is used in data callback
         * processing.
         */
        audaac_parse_ctl.ops |= AUDMPEG_PROC_OPS_PROCESSING_REWIND;
        /* This process avoid the wround up to make more precise calculation */  
        *elapsed_ms = (*elapsed_ms / 1000) * 1000;
      }

      aac_ptr->file_location = 
        audaac_calc_byte_offset(-ms_rewind, *elapsed_ms); 
    }

    aac_ptr->resync_state = AUDMPEG_RESYNC_START;
    if(ms_rewind >= *elapsed_ms) {
      *adjusted_ms = *elapsed_ms = 0;
    } else {
      *adjusted_ms = *elapsed_ms = *elapsed_ms - ms_rewind;
    }
  }

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_reset

DESCRIPTION
  This function resets internal AAC buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  aac_info, aac_frame_data_found

===========================================================================*/
void audaac_reset ()
{
  audaac_cleanup_vbr();
  audmpeg_reset(&audaac_parse_ctl);

  (void) memset(&aac_info, 0, sizeof(aac_info));

  audaac_parse_ctl.find_sync_func = audaac_find_sync;

  aac_adif_hdr_bit_index = 0;
  aac_frame_data_found = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_length

DESCRIPTION
  This function returns the total duration of the selected AAC file.

DEPENDENCIES
  aac_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_get_length(
  audmain_parse_ctl_type *parse_ptr,
  uint32 *elapsed_ms
)
{
  if (audaac_parse_ctl.ops & 
      AUDMPEG_PROC_OPS_PROCESSING_GETVBR) {

    return (AUDMAIN_STATUS_PENDING);
  }

  *elapsed_ms = aac_info.song_len_ms;
  return(AUDMAIN_STATUS_DONE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_song_config

DESCRIPTION
  Returns song-specific information which is used to configure AAC playback on
  the DSP.

DEPENDENCIES
  aac_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_get_song_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *song_config
)
{
  static const byte aac_frame_mask[] = {0xFF, 0xFF};
  uint32 tmp_bf;
  aac_header_type             *aac_hdr_ptr   = &aac_info.hdr;
  audmain_aac_config_type     *config        = &aac_info.config;

  if(parse_ctl != NULL) {
    song_config->aac_config = *config;

    if( aac_hdr_ptr->samplerate == SND_SAMPLE_RATE_NONE ||
        aac_hdr_ptr->samplerate == SND_SAMPLE_RATE_MAX){
      return(AUDMAIN_STATUS_ERROR);
    }

    /* 1024/16 * 2 * bitrate/samplerate */
    tmp_bf = 128 * aac_hdr_ptr->bit_rate;
    song_config->aac_config.buffer_factor = 
    (uint16)(tmp_bf/audmpeg_samplerate_values_table[aac_hdr_ptr->samplerate]);

    if(aac_frame_data_found) {
      
      /* If we found the header, copy the non-changing header info and mask */
      song_config->mask_size = sizeof(aac_frame_mask);
      memcpy(song_config->mask, aac_frame_mask, song_config->mask_size);

      song_config->header[0] = aac_frame_data[1] << 4;
      song_config->header[0] = song_config->header[0] | (aac_frame_data[2]>>4);
      song_config->header[1] = aac_frame_data[2] << 4;
      song_config->header[1] = song_config->header[1] | (aac_frame_data[3]>>4);
      
    } else {
      /* Clear out the mask and header info */
      song_config->mask_size = 0;
      memset(song_config->mask, 0, sizeof(song_config->mask));
      memset(song_config->header, 0, sizeof(song_config->header));
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
)
{
  return audmpeg_adec_adjust(parse_ctl, bytes_used);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_parse

DESCRIPTION
  This function parses one event of the AAC file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audaac_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
)
{
  return audmpeg_parse(parse_ctl, cmd_buf, time_remain);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
) 
{
  audmpeg_age(parse_ctl, elapsed_time, adjusted_time);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_animaac_config

DESCRIPTION
  Returns song-specific information which is used to configure Animated AAC 

DEPENDENCIES
  aac_info

RETURN VALUE
  TRUE - Format correct
  FALSE - Wrong format

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_get_animaac_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *song_config,
  snd_sample_rate_type        *sample_rate,
  byte                        *data
)
{
  boolean ret_val = FALSE;

  if( IS_SYNC( data)){
    audaac_reset();
    audaac_extract_adts_header(data);
    if ( aac_frame_data_found == TRUE ) {
     aac_info.config.aac_format = AUDMAIN_AAC_DATA_FORMAT_ADTS;
     audaac_get_song_config(parse_ctl, song_config);
     *sample_rate = audaac_get_sample_rate(parse_ctl);
     ret_val = TRUE;
    }
  }

  audaac_reset();
  return ret_val;
}

/*===========================================================================

FUNCTION audaac_adts_parse_frame

DESCRIPTION
  Parsed a buffer to get the number of ADTS frame number.


DEPENDENCIES
  None.

RETURN VALUE
  Length of the buffer parsed.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 audaac_adts_parse_frame (
  uint8   *buf,
  uint32  buf_len,
  boolean playback
)
{
  uint8  *buf_ptr;
  uint32 frame_len;
  audmpeg_vbr_parse_type *vbr_parse;
  uint32 sync_error;

  buf_ptr    = buf;
  frame_len  = 0;
  sync_error = 0;
  vbr_parse  = &(audaac_parse_ctl.vbr_parse); 

  while ((buf_ptr - buf) < buf_len) {
     /* check if this buffer is a junk with no sync which can 
       * possible create infinite loop while fforward */
     if ((audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD) &&
          (((sync_error + AUDAAC_ADTS_MIN_HEADER_LEN)-1) == buf_len)) {
       MSG_MED(" parse_frame- sync_error = %d",sync_error,0,0);
       vbr_parse->ffwd_num   = 0;
       audaac_parse_ctl.file_location =
               vbr_parse->frame_pos[vbr_parse->frame_num - 1] + buf_len; 
       MSG_MED(" RETURN parse_frame End Of FF - %d"
                                  ,audaac_parse_ctl.file_location,0,0);
       return (audaac_parse_ctl.file_location);
     }
    if ((buf_len - (buf_ptr - buf)) < AUDAAC_ADTS_MIN_HEADER_LEN) {

      /* leftover bytes can not even provide whole frame information,
       * but we need to keep it, it maybe just partial of a 
       * frame header.
       */
      if ((playback) &&
          (((vbr_parse->rrwnd_num == 0) &&
            ((vbr_parse->ffwd_num == 0))))) {

        if ((vbr_parse->leftover = malloc(buf_len - (buf_ptr - buf))) 
            != NULL) {
          vbr_parse->leftover_size = (buf_len - (buf_ptr - buf));
          memcpy(vbr_parse->leftover, buf_ptr, vbr_parse->leftover_size); 
          MSG_MED(" parse_frame- Leftover_size = %d"
                                       ,vbr_parse->leftover_size,0,0);    
        } else {

          vbr_parse->leftover_size = 0;
          vbr_parse->failure       = TRUE;
        }
      } else if ((vbr_parse->ffwd_num > 0) &&
          (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD)&&
           ((audaac_parse_ctl.file_len - 
	    vbr_parse->frame_pos[vbr_parse->frame_num - 1]) < (frame_len *2))) {
        vbr_parse->ffwd_num   = 0;
        audaac_parse_ctl.file_location =
           vbr_parse->frame_pos[vbr_parse->frame_num - 1] + 
                                          (buf_len - (buf_ptr - buf)); 
        MSG_MED(" RETURN parse_frame End Of FF - %d"
                                  ,audaac_parse_ctl.file_location,0,0);
        return (audaac_parse_ctl.file_location);           

      }

      return (buf_ptr - buf);
    }

    if (IS_SYNC(buf_ptr) &&
        IS_VALID_FRAME(buf_ptr)) {
  
      /* Good, a frame is found. For not playback operation, we don't
       * have to record the frame offset, just count the frame number.
       */
       /* clear the sync_error as we have found a sync */
       sync_error = 0;

      frame_len = (uint16)((buf_ptr[5] & 0xE0) >> 5);
      frame_len = frame_len | ((uint16)buf_ptr[4] << 3);
      frame_len = frame_len | ((uint16)(buf_ptr[3] & 0x03) << 11);

      if (!playback) {
        vbr_parse->frame_num += 1;
        vbr_parse->vbr_recorded += frame_len;
        MSG_MED(" Notplayback-  framenum = %d, frame_pos = %d "
                   ,vbr_parse->frame_num,vbr_parse->vbr_recorded,0);
      }

      if (playback) {

        /* Now, this section deals with the playback situation. Couple
         * of contraints have to be taken into consideration.
         */
        if ((vbr_parse->rrwnd_num == 0) &&
            (vbr_parse->ffwd_num  == 0)) {

          /* Normally playback, just record the frame offset */
          vbr_parse->frame_num += 1;

          if ((vbr_parse->frame_num / AUDMPEG_VBR_PRE_ALLOC_MEM) ==
              audaac_parse_ctl.vbr_parse.prealloc_index) {

            audaac_parse_ctl.vbr_parse.prealloc_index += 1;

            vbr_parse->frame_pos = 
              realloc(vbr_parse->frame_pos, 
                      (sizeof(uint32) * 
                       (AUDMPEG_VBR_PRE_ALLOC_MEM * 
                        audaac_parse_ctl.vbr_parse.prealloc_index)));
          }

          if (vbr_parse->frame_pos != NULL) {

            vbr_parse->vbr_recorded += frame_len;
  
            vbr_parse->frame_pos[vbr_parse->frame_num - 1] = 
              vbr_parse->vbr_recorded;
	    MSG_MED(" parse_frame-  framenum = %d, frame_pos = %d "
                       ,vbr_parse->frame_num,vbr_parse->vbr_recorded,0);
  
          } else { 
           vbr_parse->failure = TRUE;
          }
        } else if (vbr_parse->rrwnd_num > 0) {

          /* This is repeated frames during rewind operation, no need
           * to record it 
           */
          vbr_parse->rrwnd_num -= 1;

        } else if (vbr_parse->ffwd_num > 0) {

          /* This is "Jumping search" for forward operation, the frame
           * offset is recorded till we found the target frame.
           */

          if (audaac_parse_ctl.ops & AUDMPEG_PROC_OPS_PROCESSING_FFWD) {

            vbr_parse->ffwd_num -= 1;
            vbr_parse->frame_num += 1;
  
            if ((vbr_parse->frame_num / AUDMPEG_VBR_PRE_ALLOC_MEM) == 
                audaac_parse_ctl.vbr_parse.prealloc_index) {
            
              audaac_parse_ctl.vbr_parse.prealloc_index += 1;
  
              vbr_parse->frame_pos = 
                realloc(vbr_parse->frame_pos, 
                        (sizeof(uint32) * 
                         (AUDMPEG_VBR_PRE_ALLOC_MEM * 
                          audaac_parse_ctl.vbr_parse.prealloc_index)));
            }
  
            if (vbr_parse->frame_pos != NULL) {
  
              /* record this frame offset */ 
              vbr_parse->vbr_recorded += frame_len;
            
              vbr_parse->frame_pos[vbr_parse->frame_num - 1] = 
                vbr_parse->vbr_recorded;
            
            } else {
             vbr_parse->failure = TRUE;
            }
            if ((audaac_parse_ctl.file_len - 
                 vbr_parse->frame_pos[vbr_parse->frame_num - 1]) <
                (frame_len *2)) {
              /* FFworard approaching to the end of file */
              vbr_parse->ffwd_num   = 0;
              vbr_parse->ms_forward = 0;
              audaac_parse_ctl.file_location = audaac_parse_ctl.file_len;
	      MSG_MED(" RETURN parse_frame EOF - %d"
                                    ,audaac_parse_ctl.file_location,0,0);
              return (audaac_parse_ctl.file_location);
  
            } else if (vbr_parse->ffwd_num == 0) {
  
              audaac_parse_ctl.file_location =
              vbr_parse->frame_pos[vbr_parse->frame_num - 1]; 
              MSG_MED(" RETURN parse_frame End Of FF - %d"
                                 ,audaac_parse_ctl.file_location,0,0);
              return (audaac_parse_ctl.file_location);
            }
          } /* in ffworard searching */
        } /* if searching target forward frame */
      } /* if (playback) */

      if (((buf_ptr - buf) + frame_len) > buf_len) {
 
        return ((buf_ptr - buf) + frame_len);
      }

      buf_ptr += frame_len;

    } else {
      sync_error++;
      buf_ptr++;
    }
  } /* while */

  return (buf_len - (buf_ptr - buf));
}

/*===========================================================================

FUNCTION audaac_cleanup_vbr

DESCRIPTION
  If it is ADTS file playback, check and clean up the vbr_parse block.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_cleanup_vbr (void)
{
  if (aac_info.config.aac_format == AUDMAIN_AAC_DATA_FORMAT_ADTS) { 

    if (audaac_parse_ctl.vbr_parse.frame_pos) { 

      free(audaac_parse_ctl.vbr_parse.frame_pos);
      audaac_parse_ctl.vbr_parse.frame_pos = NULL;
    }

    audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_NONE;
    memset(&(audaac_parse_ctl.vbr_parse), 0, sizeof(audmpeg_vbr_parse_type));
  }
}

/*===========================================================================

FUNCTION audaac_check_ops_in_progress

DESCRIPTION
  Check whether there is any operation in-progress when do parse_head.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_check_ops_in_progress (void)
{

  return (audaac_parse_ctl.ops != AUDMPEG_PROC_OPS_NONE);
}

/*===========================================================================

FUNCTION audaac_abort_calc_time

DESCRIPTION
  This function abort the current get_time operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void audaac_abort_calc_time (void)
{
  audaac_parse_ctl.ops = AUDMPEG_PROC_OPS_NONE;

  audmain_do_play_cb(audaac_parse_ctl.audmpeg_handle, 
                     SND_CMX_AF_FREE_BUF, 
                     0, 
                     (void *)audaac_parse_ctl.track_buf_2);
}
#endif /* FEATURE_AAC */


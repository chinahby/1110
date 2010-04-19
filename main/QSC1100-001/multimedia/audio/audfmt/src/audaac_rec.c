// $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_rec.c#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $
/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audaac_enc Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    A U D I O   F O R M A T s   A A C   E N C O D E R   S E R V I C E S 

GENERAL DESCRIPTION
  This module contains AudAAC Encoder services software.

EXTERNALIZED FUNCTIONS
  audaac_rec_get_buf
    This function will get the availabe buffer for encoded data.
  audaac_rec_frame
    Record AAC encoded packet into ADTS/ADIF/RAW format.
  audaac_rec_start
    Start a AAC record session.
  audaac_rec_stop
    Stop a AAC record session.
  audaac_rec_pause
    Pause a AAC record session.
  audaac_rec_buf
    This function accepts a buffer from CMX for AAC recording.

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES
  ISO/IEC 13818-7:1997(E)
  ISO/IEC 14496-3:2001(E)
  
Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_rec.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/05    hz     Get rid of a compilation warning in audaac_rec_frame_raw. 
05/20/05    hz     Second revision for release.
03/08/05    hz     First revision for release.
                  
                 
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"          /* Target specific definitions                 */
#include "comdef.h"          /* Customer specific definitions               */
#include "customer.h"        /* Customer specific definitions               */
#ifdef FEATURE_AAC_REC
#include "msg.h"             /* Message logging/reporting services          */
#include "err.h"             /* Error logging/reporting services            */
#include <memory.h>          /* Memory manipulation routines                */

#include "audmain.h"         /* Audio Format Data definitions and protos    */
#include "audaaci.h"         /* AAC Recoder internal typedefs and prototypes*/
#include "audaac_rec.h"      /* AAC Recoder typedefs and prototypes         */
#include "audaac_reci.h"     /* AAC Recoder internal typedefs and prototypes*/
#include "audmpeg.h"         /* MPEG typedefs and prototypes                */
#include "snd.h"             /* Sound driver                                */
#include "qw.h"              /* Quadword typedefs and prototypes            */
#include <stdlib.h>

#include "clk.h"

/*---------------------------------------------------------------------------
** AUDAAC ENCODER General Defines
**---------------------------------------------------------------------------
*/

/* These structures contain all the information about a file
** that is being recorded.
*/
static audaac_rec_ctl_type audaac_mm_rec_ctl;

extern const  snd_sample_rate_type aac_samplerate_table[];

/* samplerate_table[sampling_frequency_index] */
const uint32 aac_samplerate_val [] = {
  96000, 88200, 64000, 48000, 44100, 32000, 
  24000, 22050, 16000, 12000, 11025, 8000
};

/*---------------------------------------------------------------------------
** AudAAC Internal Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audaac_rec_data

DESCRIPTION
  This function sends a buffer through callback to higher layer for recording
  encoded data/frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_data (void)
{
  audaac_rec_ctl_type         *ctl_ptr;
  snd_mm_rec_data_struct_type data;

  ctl_ptr  = &audaac_mm_rec_ctl; 

  data.rec_data_buf  = ctl_ptr->current_buf;
  data.rec_data_num  = ctl_ptr->index_current_buf;
  data.rec_data_pos  = ctl_ptr->running_index;
  data.client_data   = (const void*) ctl_ptr;
  
  ctl_ptr->audaac_rec_bytes   += ctl_ptr->index_current_buf;
  
  audmain_mm_rec_status(SND_CMX_MM_REC_DATA, 
                        &data, 
                        ctl_ptr->cb_func,
                        ctl_ptr->client_data);
  
  ctl_ptr->running_index  += ctl_ptr->index_current_buf;
  
  if (ctl_ptr->buffer_available) {

    ctl_ptr->buffer_available   = FALSE;
  
    ctl_ptr->current_buf        = ctl_ptr->reserve_buf;
    ctl_ptr->index_current_buf  = ctl_ptr->index_resv_buf;
    ctl_ptr->size_current_buf   = ctl_ptr->size_reserve_buf;
  
    ctl_ptr->reserve_buf        = NULL;
    ctl_ptr->size_reserve_buf   = 0;
  } else {

    ctl_ptr->current_buf = NULL; 
    MSG_MED("audaac_rec_data buffer underflow", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION audaac_rec_frame_raw

DESCRIPTION
  This function records a frame of data in raw format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_do_avsync(voc_aac_record_data_type *frame_info)
{
  snd_av_report_type    av_report;
  qword                 time_stamp;
  audaac_rec_ctl_type   *ctl_ptr;
  boolean               do_sync;

  ctl_ptr = &audaac_mm_rec_ctl; 
  do_sync = FALSE;

  clk_uptime_ms(time_stamp);

  if (frame_info->lost_frame1 != 0) {
    /* oops, frame lost happens */
    
    av_report.num_samples  = 0;
    *(uint64 *)(av_report.time_stamp) = *(uint64 *)time_stamp;
    av_report.lost_samples = frame_info->lost_frame1 * 1024;
    
    do_sync = TRUE;
  
    ctl_ptr->sync_frames = frame_info->sync_frame1;
  } else if ((frame_info->lost_frame1  == 0) && 
             (frame_info->buf1_avsync == TRUE)) {
    /* A/V sync sample threshold reached */
  
    av_report.num_samples  = frame_info->samples_rpt1;
  
    *(uint64 *)(av_report.time_stamp) = *(uint64 *)time_stamp;
    av_report.frame_index  = frame_info->sync_frame1;
    
    do_sync = TRUE;

    ctl_ptr->sync_frames = frame_info->sync_frame1;
  } else if (frame_info->lost_frame2 != 0) {
    /* oops, frame lost happens */
      
    clk_uptime_ms(time_stamp);
    av_report.num_samples  = 0;
    *(uint64 *)(av_report.time_stamp) = *(uint64 *)time_stamp;
    av_report.lost_samples = frame_info->lost_frame1 * 1024;
    
    do_sync = TRUE;

    ctl_ptr->sync_frames = frame_info->sync_frame2;
  } else if ((frame_info->lost_frame2 == 0) &&  
             (frame_info->buf2_avsync == TRUE)) {
    /* A/V sync sample threshold reached */
    
    av_report.num_samples  = frame_info->samples_rpt2;
    
    *(uint64 *)(av_report.time_stamp) = *(uint64 *)time_stamp;
    av_report.frame_index  = frame_info->sync_frame1;
    
    do_sync = TRUE;

    ctl_ptr->sync_frames = frame_info->sync_frame2;
  }

  if (do_sync) {

    ctl_ptr->sync_cb_func(&av_report,
                          ctl_ptr->client_data);
  }
}

/*===========================================================================

FUNCTION audaac_rec_frame_raw

DESCRIPTION
  This function records a frame of data in raw format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_frame_raw (
  voc_aac_record_data_type *frame_info
)
{
  audaac_rec_ctl_type   *ctl_ptr;
  uint8                 index;
  uint8                 *buf_ptr;

  MSG_MED("audaac_rec_frame_raw entering", 0, 0, 0);

  ctl_ptr = &audaac_mm_rec_ctl; 

  if ((ctl_ptr->status & AUDAAC_MASK_ENABLED) &&
      !(ctl_ptr->status & AUDAAC_MASK_PAUSED)) {

    if (ctl_ptr->current_buf != NULL) {

      if (frame_info->buf1_flush == TRUE) {
        /* buf1 always points to current buffer */

        for (index = 0; index < frame_info->buf1_frame_num; index++) {

          buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);

          ctl_ptr->index_current_buf += (
                  frame_info->buf1_frame_info[index].data_length);

          ctl_ptr->audaac_rec_num_frames++;
        } /* for */

        audaac_rec_data();

        if (ctl_ptr->sync_cb_func != NULL) {
           
          if (frame_info->buf1_avsync) {
        
            audaac_rec_do_avsync(frame_info);
	      } /* ctl_ptr->sync_cb_func != NULL */
        } /* if (ctl_ptr->sync_cb_func != NULL) */

        if (frame_info->buf2_frame_num > 0) {
          for (index = 0; index < frame_info->buf2_frame_num; index++) {
            /* buf2 now points to current buffer */ 
            buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);
          
            ctl_ptr->index_current_buf += (
                    frame_info->buf2_frame_info[index].data_length);

            ctl_ptr->audaac_rec_num_frames++;
          } /* for */

          if (ctl_ptr->sync_cb_func != NULL) {

            if (frame_info->buf2_avsync) {

              audaac_rec_data();
              audaac_rec_do_avsync(frame_info);
            }
          } /* if (ctl_ptr->sync_cb_func != NULL) */
        } /* if (frame_info->buf2 != NULL) */
      } else {

        for (index = 0; index < frame_info->buf1_frame_num; index++) {

          buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);

          ctl_ptr->index_current_buf += (
                  frame_info->buf1_frame_info[index].data_length);

          ctl_ptr->audaac_rec_num_frames++;
        } /* for */
      }
    } /* if (ctl_ptr->current_buf != NULL) */
  } /* if in enable mode */

  return;
}

/*===========================================================================

FUNCTION audaac_rec_frame_adts

DESCRIPTION
  This function records a frame of data in adts format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_frame_adts (
  voc_aac_record_data_type *frame_info
)
{
  audaac_rec_ctl_type   *ctl_ptr;
  uint8                 index;
  uint8                 *buf_ptr;

  MSG_MED("audaac_rec_frame_adts entering", 0, 0, 0);

  ctl_ptr = &audaac_mm_rec_ctl; 

  if ((ctl_ptr->status & AUDAAC_MASK_ENABLED) &&
      !(ctl_ptr->status & AUDAAC_MASK_PAUSED)) {

    if (ctl_ptr->current_buf != NULL) {

      if (frame_info->buf1_flush == TRUE) {
        /* buf1 always points to current buffer */

        for (index = 0; index < frame_info->buf1_frame_num; index++) {

          buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);

          audaac_rec_install_adts_header_variable(
                  frame_info->buf1_frame_info[index].data_length +
                  AUDAAC_MAX_ADTS_HEADER_LENGTH);

          memcpy(buf_ptr,
                 ctl_ptr->audaac_header,
                 AUDAAC_MAX_ADTS_HEADER_LENGTH);

          ctl_ptr->index_current_buf += (
                  frame_info->buf1_frame_info[index].data_length +
                  AUDAAC_MAX_ADTS_HEADER_LENGTH);

          ctl_ptr->audaac_rec_num_frames++;
        } /* for */

        audaac_rec_data();

        if (ctl_ptr->sync_cb_func != NULL) {
           
          if (frame_info->buf1_avsync) {
        
            audaac_rec_do_avsync(frame_info);
	      } /* ctl_ptr->sync_cb_func != NULL */
        } /* if (ctl_ptr->sync_cb_func != NULL) */

        if (frame_info->buf2_frame_num > 0) {
          for (index = 0; index < frame_info->buf2_frame_num; index++) {
            /* buf2 now points to current buffer */ 
            buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);
          
            audaac_rec_install_adts_header_variable(
                    frame_info->buf2_frame_info[index].data_length +
                    AUDAAC_MAX_ADTS_HEADER_LENGTH);
          
            memcpy(buf_ptr,
                   ctl_ptr->audaac_header,
                   AUDAAC_MAX_ADTS_HEADER_LENGTH);
          
            ctl_ptr->index_current_buf += (
                    frame_info->buf2_frame_info[index].data_length +
                    AUDAAC_MAX_ADTS_HEADER_LENGTH);

            ctl_ptr->audaac_rec_num_frames++;
          } /* for */

          if (ctl_ptr->sync_cb_func != NULL) {
             
            if (frame_info->buf2_avsync) {

              audaac_rec_data();
              audaac_rec_do_avsync(frame_info);
            }
          } /* if (ctl_ptr->sync_cb_func != NULL) */
        } /* if (frame_info->buf2 != NULL) */
      } else {

        for (index = 0; index < frame_info->buf1_frame_num; index++) {

          buf_ptr = &(ctl_ptr->current_buf[ctl_ptr->index_current_buf]);

          audaac_rec_install_adts_header_variable(
                  frame_info->buf1_frame_info[index].data_length +
                  AUDAAC_MAX_ADTS_HEADER_LENGTH);

          memcpy(buf_ptr,
                 ctl_ptr->audaac_header,
                 AUDAAC_MAX_ADTS_HEADER_LENGTH);

          ctl_ptr->index_current_buf += (
                  frame_info->buf1_frame_info[index].data_length +
                  AUDAAC_MAX_ADTS_HEADER_LENGTH);

          ctl_ptr->audaac_rec_num_frames++;
        } /* for */
      }
    } /* if (ctl_ptr->current_buf != NULL) */
  } /* if in enable mode */

  return;
}

/*===========================================================================

FUNCTION audaac_rec_frame

DESCRIPTION
  This function records aac encoded frame(s) of data into buffer(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_frame (
  voc_aac_record_data_type *frame_info
)
{
  if (audaac_mm_rec_ctl.format == SND_AAC_DATA_FORMAT_ADTS) {

    audaac_rec_frame_adts(frame_info);

  } else if (audaac_mm_rec_ctl.format == SND_AAC_DATA_FORMAT_ADIF ||
             audaac_mm_rec_ctl.format == SND_AAC_DATA_FORMAT_RAW) {

    audaac_rec_frame_raw(frame_info);
  }
}

/*===========================================================================

FUNCTION audaac_rec_get_buf

DESCRIPTION
  This function will get the availabe buffer for encoded data.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  TRUE  - There is available buffer
  FALSE - There is no available buffer

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_rec_get_buf (
  voc_aac_record_buf_type *buf_type
)
{

  if (audaac_mm_rec_ctl.current_buf == NULL) {
    return FALSE;
  }

  buf_type->buf1       = &(audaac_mm_rec_ctl.current_buf[
                           audaac_mm_rec_ctl.index_current_buf]);
  buf_type->buf1_space = audaac_mm_rec_ctl.size_current_buf - 
                         audaac_mm_rec_ctl.index_current_buf; 

  if (audaac_mm_rec_ctl.buffer_available) {
    buf_type->buf2       = audaac_mm_rec_ctl.reserve_buf;
    buf_type->buf2_space = audaac_mm_rec_ctl.size_reserve_buf;
  } else {
    buf_type->buf2 = NULL;
  }

  if (audaac_mm_rec_ctl.format == SND_AAC_DATA_FORMAT_ADTS) {
    buf_type->hdr_offset = AUDAAC_MAX_ADTS_HEADER_LENGTH;
  } else {
    buf_type->hdr_offset = 0;
  }

  if (audaac_mm_rec_ctl.sync_cb_func != NULL) {
    buf_type->sync_samples = audaac_mm_rec_ctl.sync_samples;
    buf_type->sync_frames  = audaac_mm_rec_ctl.sync_frames;
  } else {
    buf_type->sync_samples = 0;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION audaac_rec_install_bits

DESCRIPTION
  This function is for setup the AAC file and frame headers.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None.

SIDE EFFECTS
  Updates audaac_hdr_bit_index.
===========================================================================*/
LOCAL void audaac_rec_install_bits 
(
  uint8   *input,
  byte    num_bits_reqd,
  uint32  value,
  uint16  *hdr_bit_index
)
{
  uint32 byte_index;
  byte   bit_index;
  byte   bits_avail_in_byte;
  byte   num_to_copy;
  byte   byte_to_copy;

  byte   num_remaining = num_bits_reqd;
  uint8  bit_mask;
  
  bit_mask = 0xFF;

  while (num_remaining) {

    byte_index = (*hdr_bit_index) >> 3;
    bit_index  = (*hdr_bit_index) &  0x07;

    bits_avail_in_byte = 8 - bit_index;

    num_to_copy = MIN(bits_avail_in_byte, num_remaining);

    byte_to_copy = ((uint8)((value >> (num_remaining - num_to_copy)) & 0xFF) <<
                    (bits_avail_in_byte - num_to_copy));

    input[byte_index] &= ((uint8)(bit_mask << bits_avail_in_byte));
    input[byte_index] |= byte_to_copy;

    *hdr_bit_index += num_to_copy;

    num_remaining -= num_to_copy;
  } /* while (num_remaining) */
} /* audaac_rec_install_bits */


/*===========================================================================

FUNCTION audaac_rec_install_adif_header

DESCRIPTION
  This routine sets up the adif file header.  

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None.

SIDE EFFECTS
  Updates audaac_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adif_header 
(
  uint8      sample_index,
  uint32     bit_rate
)
{
  uint8   buf8;
  uint32  value;
  uint32  dummy = 0;
  byte    num_pfe, num_fce, num_sce, num_bce; 
  byte    num_lfe, num_ade, num_vce, num_com;
  byte    pfe_index;
  byte    i;
  boolean variable_bit_rate = FALSE;

  audaac_rec_ctl_type         *ctl_ptr;
  ctl_ptr  = &audaac_mm_rec_ctl; 

  num_pfe = num_fce = num_sce = num_bce = 
  num_lfe = num_ade = num_vce = num_com = 0;

  /* Store Header Id "ADIF" first */
  memcpy(&(ctl_ptr->audaac_header[0]), "ADIF", sizeof(uint32));

  ctl_ptr->audaac_hdr_bit_index = 32;

  /* copyright_id_present field, 1 bit */
  value = 0;
  audaac_rec_install_bits(ctl_ptr->audaac_header, 
                          AAC_COPYRIGHT_PRESENT_SIZE, 
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  if (value) {
    /* Copyright present, 72 bits; skip it for now,
     * just install dummy value */
    audaac_rec_install_bits(ctl_ptr->audaac_header, 
                            72,
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));
  }

  /* original_copy field, 1 bit */
  value = 1;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_ORIGINAL_COPY_SIZE,
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* home field, 1 bit */
  value = 1;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_HOME_SIZE, 
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* bitstream_type = 1, varibable bit rate, 1 bit */
  value = 0;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_BITSTREAM_TYPE_SIZE,
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* bit_rate field, 23 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_BITRATE_SIZE, 
                          bit_rate,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* num_program_config_elements, 4 bits */
  num_pfe = 0;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_NUM_PFE_SIZE, 
                          (uint32)num_pfe,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* below is to install program_config_elements field,
   * for now only one element is supported */
  for (pfe_index=0; pfe_index < num_pfe+1; pfe_index++) {

     dummy = 0xFFFFF; 

     if (!variable_bit_rate) { /* impossible, put dummy value for now */
       audaac_rec_install_bits(ctl_ptr->audaac_header, 
                               AAC_BUFFER_FULLNESS_SIZE, 
                               dummy,
                               &(ctl_ptr->audaac_hdr_bit_index));

     }

    dummy = 0;

    /* element_instance_tag field, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_ELEMENT_INSTANCE_TAG_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* object_type, 2 bits, AAC LC is supported */
    value = 1;
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_PROFILE_SIZE,  /* object type */
                            value,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* sampling_frequency_index, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_SAMPLING_FREQ_INDEX_SIZE, 
                            (uint32)sample_index,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_front_channel_elements, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_FRONT_CHANNEL_ELEMENTS_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_side_channel_elements, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_SIDE_CHANNEL_ELEMENTS_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_back_channel_elements, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_BACK_CHANNEL_ELEMENTS_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_lfe_channel_elements, 2 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_LFE_CHANNEL_ELEMENTS_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_assoc_data_elements, 3 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_ASSOC_DATA_ELEMENTS_SIZE, 
                            num_ade,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* num_valid_cc_elements, 4 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_NUM_VALID_CC_ELEMENTS_SIZE, 
                            num_vce,
                            &(ctl_ptr->audaac_hdr_bit_index));

    /* mono_mixdown_present, 1 bits */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_MONO_MIXDOWN_PRESENT_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    if (dummy) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_MONO_MIXDOWN_ELEMENT_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }
   
    /* stereo_mixdown_present */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_STEREO_MIXDOWN_PRESENT_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    if (dummy) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_STEREO_MIXDOWN_ELEMENT_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    /* matrix_mixdown_idx_present, 1 bit */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_MATRIX_MIXDOWN_PRESENT_SIZE, 
                            dummy,
                            &(ctl_ptr->audaac_hdr_bit_index));

    if (dummy) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_MATRIX_MIXDOWN_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    for (i=0; i<num_fce; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_FCE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }
    
    for (i=0; i<num_sce; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_SCE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    for (i=0; i<num_bce; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_BCE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    for (i=0; i<num_lfe; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_LFE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    for (i=0; i<num_ade; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_ADE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    for (i=0; i<num_vce; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_VCE_SIZE, 
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    /* byte_alignment() */
    buf8 = (byte)((ctl_ptr->audaac_hdr_bit_index) & (0x07));
    if (buf8) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              buf8,
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }

    /* comment_field_bytes, 8 bits,
     * skip the comment section */
    audaac_rec_install_bits(ctl_ptr->audaac_header,
                            AAC_COMMENT_FIELD_BYTES_SIZE,
                            num_com,
                            &(ctl_ptr->audaac_hdr_bit_index));

    for (i=0; i<num_com; i++) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              AAC_COMMENT_FIELD_DATA_SIZE,
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
    }
  } /* for (pfe_index=0; pfe_index < num_pfe+1; pfe_index++) */

  /* byte_alignment() */
  buf8 = (byte)((ctl_ptr->audaac_hdr_bit_index) & (0x07) );
  if (buf8) {
      audaac_rec_install_bits(ctl_ptr->audaac_header,
                              buf8,
                              dummy,
                              &(ctl_ptr->audaac_hdr_bit_index));
  }
} /* audaac_rec_install_adif_header */ 


/*===========================================================================

FUNCTION audaac_rec_install_adts_header_fixed

DESCRIPTION
  This routine sets up the fixed part of adts frame header.
  
DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None.

SIDE EFFECTS
  Updates audaac_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adts_header_fixed (
  uint8   sample_index,
  uint8   channel_config
)
{
  uint32  value;

  audaac_rec_ctl_type *ctl_ptr;

  ctl_ptr  = &audaac_mm_rec_ctl; 

  /* Store Sync word first */
  ctl_ptr->audaac_header[0] = 0xFF;
  ctl_ptr->audaac_header[1] = 0xF0;

  ctl_ptr->audaac_hdr_bit_index = 12;

  /* ID field, 1 bit */
  value = 1;
  audaac_rec_install_bits(ctl_ptr->audaac_header, 
                          1, 
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* Layer field, 2 bits */
  value = 0;
  audaac_rec_install_bits(ctl_ptr->audaac_header, 
                          AACHDR_LAYER_SIZE,
                          value, 
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* Protection_absent field, 1 bit */
  value = 1;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AACHDR_CRC_SIZE,
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* profile_ObjectType field, 2 bit */
  value = 1;
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_PROFILE_SIZE,
                          value,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* sampling_frequency_index field, 4 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_SAMPLING_FREQ_INDEX_SIZE,
                          (uint32)sample_index,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* pravate_bit field, 1 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          1, 
                          0,
                          &(ctl_ptr->audaac_hdr_bit_index));

  /* channel_configuration field, 3 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          3,
                          channel_config,
                          &(ctl_ptr->audaac_hdr_bit_index));


  /* original/copy field, 1 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_ORIGINAL_COPY_SIZE,
                          0,
                          &(ctl_ptr->audaac_hdr_bit_index));


  /* home field, 1 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AAC_HOME_SIZE,
                          0,
                          &(ctl_ptr->audaac_hdr_bit_index));

} /* audaac_rec_install_adts_header_fixed */ 

/*===========================================================================

FUNCTION audaac_rec_install_adts_header_variable

DESCRIPTION
  This routine sets up the variable part of adts frame header.
  
DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None.

SIDE EFFECTS
  Updates audaac_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adts_header_variable (uint16  byte_num)
{
  uint16  bit_index;

  audaac_rec_ctl_type *ctl_ptr;

  ctl_ptr  = &audaac_mm_rec_ctl; 

  bit_index = ctl_ptr->audaac_hdr_bit_index;

  bit_index += 2;

  /* aac_frame_length field, 13 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          AUDAAC_ADTS_FRAME_LENGTH_SIZE,
                          byte_num,
                          &bit_index);

  /* adts_buffer_fullness field, 11 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          11,
                          0x7FF,
                          &bit_index);

  /* number_of_raw_data_blocks_in_frame, 2 bits */
  audaac_rec_install_bits(ctl_ptr->audaac_header,
                          2,
                          0,
                          &bit_index);

} /* audaac_rec_install_adts_header_variable */ 

/*===========================================================================

FUNCTION audaac_rec

DESCRIPTION
  This function starts a AAC recording session.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_rec_start (
  snd_mm_rec_aac_para_type    *rec_param,   /* Record parameters              */
  snd_mm_rec_cmd_struct_type  *buf,         /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func,      /* Callback function to CMX       */
  const void                  *client_data  /* Client data pointer            */
)
{
  snd_mm_rec_data_struct_type  data;
  snd_aac_channel_enum_type    chan_config;
  boolean                      ret_val; 
  audaac_rec_ctl_type          *ctl_ptr = NULL;
  uint32                       bit_rate = 0;
  uint8                        index_cnt;

  ctl_ptr   = &audaac_mm_rec_ctl;
  index_cnt = 0;
  ret_val   = TRUE;

  ctl_ptr->audaac_rec_bytes       = 0;
  ctl_ptr->audaac_rec_num_frames  = 0;

  chan_config         = rec_param->chan;

  while (index_cnt < AUDAAC_SAMPLE_INDEX_MAX) {

    if (rec_param->sample_rate == aac_samplerate_table[index_cnt]) {
      break;
    }

    index_cnt++;    
  }

  if (index_cnt == AUDAAC_SAMPLE_INDEX_MAX) {
    return (NULL);
  } else {
    ctl_ptr->sample_rate       = rec_param->sample_rate; 
    ctl_ptr->sample_rate_index = index_cnt;
  }

  switch (rec_param->aac_file_format) {
  case SND_AAC_DATA_FORMAT_ADIF: {

     memset(ctl_ptr->audaac_header, 0, AUDAAC_MAX_ADIF_HEADER_LENGTH);
     ctl_ptr->audaac_hdr_bit_index = 0;

     /*---------------------------------------------------------
      * write dummy ADIF file header first, it will be re-written
      * at stop when the recording session finishes
      *--------------------------------------------------------*/
      audaac_rec_install_adif_header(ctl_ptr->sample_rate_index,
                                     bit_rate);
    
      if ((ctl_ptr != NULL) && 
          !(ctl_ptr->status & AUDAAC_MASK_ENABLED)) {
    
        ctl_ptr->audaac_rec_head_bytes = (ctl_ptr->audaac_hdr_bit_index / 8); 
    
        data.rec_data_buf  = (uint8 *)ctl_ptr->audaac_header; 
        data.rec_data_num  = ctl_ptr->audaac_rec_head_bytes;
        data.rec_data_pos  = 0;
        data.buf_cb_func   = NULL;
        data.client_data   = NULL;
    
        cb_func(SND_CMX_MM_REC_HEADER_DATA, 
                &data, 
                client_data);
      }
  } /* case SND_MM_FORMAT_ADIF: */
      break;   

  case SND_AAC_DATA_FORMAT_ADTS: {

     memset(ctl_ptr->audaac_header, 0, AUDAAC_MAX_ADTS_HEADER_LENGTH);
     ctl_ptr->audaac_hdr_bit_index = 0;
     ctl_ptr->audaac_rec_head_bytes  = 0;

     /*---------------------------------------------------------
      * Prepare ADTS frame headers (Fixed)
      *--------------------------------------------------------*/
     if (chan_config == SND_AAC_CHANNEL_MONO) {

       audaac_rec_install_adts_header_fixed(ctl_ptr->sample_rate_index,
                                            (uint8)chan_config);

     } else if (chan_config == SND_AAC_CHANNEL_DUAL) {

       audaac_rec_install_adts_header_fixed(ctl_ptr->sample_rate_index,
                                            (uint8)chan_config);

     } else {

       ret_val = FALSE;
     }
  } /* case SND_MM_FORMAT_ADTS: */
      break;   

  case SND_AAC_DATA_FORMAT_RAW: 
  case SND_AAC_DATA_FORMAT_PSUEDO_RAW: {
     /*---------------------------------------------------------
      * Prepare frame headers (Fixed and Variable) 
      *--------------------------------------------------------*/
     ctl_ptr->audaac_rec_head_bytes  = 0;
  } /* case SND_MM_FORMAT_RAW_AAC: */
      break;   

  default:
      ret_val = FALSE;
      break;
  }

  if (ret_val != FALSE) {
    ctl_ptr->running_index      = ctl_ptr->audaac_rec_head_bytes;
    ctl_ptr->format             = rec_param->aac_file_format;
    ctl_ptr->channel            = rec_param->chan;
    ctl_ptr->cb_func            = cb_func;
    ctl_ptr->client_data        = client_data;
    ctl_ptr->sync_cb_func       = rec_param->sync_cb_func;

    if (ctl_ptr->sync_cb_func != NULL) {
      ctl_ptr->sync_samples = 
          ((aac_samplerate_val[ctl_ptr->sample_rate_index]) *
           rec_param->sync_ms) / 1000;
    }

    ctl_ptr->current_buf        = buf->buffer1;
    ctl_ptr->buffer_available   = TRUE;
    ctl_ptr->size_current_buf   = buf->num_buf1;
    ctl_ptr->index_current_buf  = 0;
    ctl_ptr->reserve_buf        = buf->buffer2;
    ctl_ptr->size_reserve_buf   = buf->num_buf2;
    ctl_ptr->index_resv_buf     = 0;
  }

  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rec_stop

DESCRIPTION
  This function stops a AAC recording session.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_rec_stop (void)
{
  snd_mm_rec_data_struct_type  rec_data;
  audmain_status_type          ret_val  = AUDMAIN_STATUS_FAILURE;
  audaac_rec_ctl_type          *ctl_ptr = NULL;
  uint32                       bit_rate = 0; 

  ctl_ptr = &audaac_mm_rec_ctl;
  ret_val  = AUDMAIN_STATUS_FAILURE;

  if ((ctl_ptr != NULL) && 
      (ctl_ptr->status & AUDAAC_MASK_ENABLED)) {

    ctl_ptr->status &= ~AUDAAC_MASK_ENABLED;

    if (ctl_ptr->cb_func != NULL) {

      rec_data.buf_cb_func = NULL;
      rec_data.client_data = NULL;

      if (ctl_ptr->index_current_buf != 0) {

        rec_data.rec_data_buf   = ctl_ptr->current_buf;
        rec_data.rec_data_num   = ctl_ptr->index_current_buf;
        rec_data.rec_data_pos   = ctl_ptr->running_index;

        ctl_ptr->audaac_rec_bytes   += ctl_ptr->index_current_buf;

        ctl_ptr->cb_func(SND_CMX_MM_REC_DATA, 
                         &rec_data, 
                         ctl_ptr->client_data);

        ctl_ptr->running_index += ctl_ptr->index_current_buf;

        ctl_ptr->index_current_buf = 0;

      } /* if (ctl_ptr->index_current_buf != 0) */

      if (ctl_ptr->format == SND_AAC_DATA_FORMAT_ADIF) {

        if (ctl_ptr->audaac_rec_num_frames > 0) {

          bit_rate = (uint32)((ctl_ptr->audaac_rec_bytes * 8 *
                               aac_samplerate_val[ctl_ptr->sample_rate_index]) /
                               (1024 * ctl_ptr->audaac_rec_num_frames));
        } /* if (ctl_ptr->audaac_rec_num_frames > 0) */
        
        audaac_rec_install_adif_header((byte)ctl_ptr->sample_rate_index,
                                       bit_rate);
 
        ctl_ptr->audaac_rec_head_bytes = (ctl_ptr->audaac_hdr_bit_index / 8); 
        
        rec_data.rec_data_buf  = (uint8 *)ctl_ptr->audaac_header; 
        rec_data.rec_data_num  = ctl_ptr->audaac_rec_head_bytes;
        rec_data.rec_data_pos  = 0;
        rec_data.buf_cb_func   = NULL;
        rec_data.client_data   = NULL;
        
        ctl_ptr->cb_func(SND_CMX_MM_REC_HEADER_DATA, 
                         &rec_data, 
                         ctl_ptr->client_data);

      } /* if (format == AACREC_ADIF_FILE_FORMAT) */

      ctl_ptr->cb_func(SND_CMX_MM_REC_DONE,
                        NULL,
                        ctl_ptr->client_data);

      ret_val = AUDMAIN_STATUS_SUCCESS;

    } /* if (ctl_ptr->cb_func != NULL) */
  }

  return(ret_val);
}


/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rec_pause

DESCRIPTION
  This function pauses/resumes a AAC recording session.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_rec_pause (
  boolean               pause
)
{
  audmain_status_type ret_val  = AUDMAIN_STATUS_FAILURE;
  audaac_rec_ctl_type *ctl_ptr = NULL;

  ctl_ptr = &audaac_mm_rec_ctl;

  if ((ctl_ptr != NULL) && 
      (ctl_ptr->status & AUDAAC_MASK_ENABLED)) {

    if (ctl_ptr->cb_func != NULL) {

      if (pause) {

        if (!(ctl_ptr->status & AUDAAC_MASK_PAUSED)) {

          ctl_ptr->cb_func(SND_CMX_MM_REC_PAUSED, 
                           NULL, 
                           ctl_ptr->client_data);

        }

      } else {

        if (ctl_ptr->status & AUDAAC_MASK_PAUSED) {

          ctl_ptr->cb_func(SND_CMX_MM_REC_RESUMED, 
                           NULL, 
                           ctl_ptr->client_data);

        }
      }
    } /* if (ctl_ptr->cb_func != NULL) */

    if (pause) {

      ctl_ptr->status |= AUDAAC_MASK_PAUSED;

    } else {

      ctl_ptr->status &= ~AUDAAC_MASK_PAUSED;

    }

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_AAC_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_send_status (
  snd_af_data_cmd_enum status
)
{
  switch (status) {
    case SND_CMX_MM_REC_INTERRUPTED: {

      if ((audaac_mm_rec_ctl.status & AUDAAC_MASK_ENABLED) &&
          !(audaac_mm_rec_ctl.status & AUDAAC_MASK_INTERRUPTED) &&
          (audaac_mm_rec_ctl.cb_func != NULL)) {

         audaac_mm_rec_ctl.status |= AUDAAC_MASK_INTERRUPTED;

         audaac_mm_rec_ctl.cb_func(status, 
                                   NULL,
                                   audaac_mm_rec_ctl.client_data);
      }
    } /* case SND_CMX_MM_REC_INTERRUPTED */
      break;

    case SND_CMX_MM_REC_INT_RESUME: {

      if((audaac_mm_rec_ctl.status & AUDAAC_MASK_ENABLED) &&
         (audaac_mm_rec_ctl.status & AUDAAC_MASK_INTERRUPTED) &&
         (audaac_mm_rec_ctl.cb_func != NULL)) {

        audaac_mm_rec_ctl.status &= ~AUDAAC_MASK_INTERRUPTED;

        audaac_mm_rec_ctl.cb_func(status, 
                                  NULL,
                                  audaac_mm_rec_ctl.client_data);
      }
    } /* case SND_CMX_MM_REC_INT_RESUME */
      break;

    case SND_CMX_MM_REC_ENABLED: {

        audaac_mm_rec_ctl.status |= AUDAAC_MASK_ENABLED;

        audaac_mm_rec_ctl.cb_func(status, 
                                  NULL,
                                  audaac_mm_rec_ctl.client_data);
    }
      break; 

    default:
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rec_buf

DESCRIPTION
  This function accepts a buffer from CMX for AAC recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_rec_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
)
{
  audaac_rec_ctl_type *ctl_ptr = (audaac_rec_ctl_type *) client_data;

  if ((ctl_ptr != NULL) && 
      (ctl_ptr->status & AUDAAC_MASK_ENABLED)) {

    if (ctl_ptr->current_buf == NULL) {

      ctl_ptr->current_buf       = buffer;
      ctl_ptr->size_current_buf  = num_bytes;
      ctl_ptr->index_current_buf = 0;

    } else if (ctl_ptr->reserve_buf == NULL) {

      ctl_ptr->reserve_buf       = buffer;
      ctl_ptr->size_reserve_buf  = num_bytes;

    }

    ctl_ptr->buffer_available  = TRUE;
  }
} /* audaac_rec_buf */
#endif /* FEATURE_AAC_REC */

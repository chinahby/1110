// $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_reci.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $
#ifndef AUDAACI_REC_H
#define AUDAACI_REC_H
/*===========================================================================

 A U D F M T   S E R V I C E S   A A C   R E C O D E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all internal definitions necessary to interface
  MPEG-2 AAC files recording with AUDFMT Services.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac_reci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/05    hz     Second revision for release.
03/08/05    hz     First revision for release.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#ifdef FEATURE_AAC_REC
#include "audmpeg.h"   /* Data common to all MPEG audio file types        */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* hardcode maximum ADIF file header length (512 bits) for now           */
#define AUDAAC_MAX_ADIF_HEADER_LENGTH 64 

/* maximum ADTS frame header length                */
#define AUDAAC_MAX_ADTS_HEADER_LENGTH 7 

/* ADTS variable frame header, frame length field  */
#define AUDAAC_ADTS_FRAME_LENGTH_SIZE    13                                     

/* Sample index count                              */
#define AUDAAC_SAMPLE_INDEX_MAX       12 

/*===========================================================================

                       AAC HEADER STRUCTURE DECLARATIONS

===========================================================================*/
/* This structure contains all information for MM recording session
*/
typedef struct {
  uint32                      status;            /* Status of MM recording   */
  uint32                      running_index;     /* Current index of file    */
  snd_aac_data_format_enum_type format;          /* AAC file format          */
  snd_aac_channel_enum_type   channel;           /* Stereo/Mono channel      */
  snd_mm_rec_cb_func_ptr_type cb_func;           /* Client callback function */
  const void                  *client_data;      /* Client data ptr          */
  snd_sample_rate_type        sample_rate;       /* Sample rate enum value   */
  uint8                       sample_rate_index; /* Sample rate index value  */
  snd_mm_rec_av_sync_cb_ptr   sync_cb_func; /* A/V sync callback function    */
  uint32                      sync_samples; /* Time(ms) to do A/V sync report*/
  uint32                      sync_frames;  /* A/V sync last reported frame  */

  /*
   * fields for recording buffer information
   */
  uint8                       *current_buf;      /* Current MM record buff   */
  boolean                     buffer_available;  /* If there is an empty buf */
  uint32                      size_current_buf;  /* Size of current buffer   */
  uint32                      index_current_buf; /* Index into current buff  */
  uint8                       *reserve_buf;      /* Reserve recording buffer */
  uint32                      size_reserve_buf;  /* Size of reserve buffer   */
  uint32                      index_resv_buf;    /* Index into reserve buff  */

  /*
   * fields for recording frame information
   */
  uint32  audaac_rec_bytes;                      /* Number of bytes recorded  */
  uint32  audaac_rec_num_frames;                 /* Number of frames recorded */
  uint32  audaac_rec_head_bytes;                 /* Number of bytes in header */
  uint8   audaac_header[AUDAAC_MAX_ADIF_HEADER_LENGTH]; 
                                                 /* AAC header information    */
  uint16  audaac_hdr_bit_index;                  /* Header bit index          */
} audaac_rec_ctl_type;

/* This enum type contains bitmask values for status of PCM recording
*/
typedef enum {
  AUDAAC_MASK_ENABLED       = 0x0001, /* MM recording is enabled          */
  AUDAAC_MASK_FRAME_DROPPED = 0x0002, /* MM recording frame dropped state */
  AUDAAC_MASK_INTERRUPTED   = 0x0004, /* MM recording interrupted state   */
  AUDAAC_MASK_PAUSED        = 0x0008  /* MM recording paused state        */
} audaac_status_mask_enum_type;


/*---------------------------------------------------------------------------
** Audaac Internal Function Prototypes
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audaac_rec_install_adif_header

DESCRIPTION
  This routine sets up the adif file header.  
  
DEPENDENCIES
  FEATURE_AAC_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates aac_adif_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adif_header(
  uint8  sample_index,
  uint32 bit_rate
);

/*===========================================================================

FUNCTION audaac_rec_install_bits

DESCRIPTION
  This function is for setup the AAC file and frame headers.
  
DEPENDENCIES
  FEATURE_AAC_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates aac_adif_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_bits (
  byte   *input,
  byte   num_bits_reqd,
  uint32 value,
  uint16 *hdr_bit_index
);

/*===========================================================================

FUNCTION audaac_rec_install_adts_header_fixed

DESCRIPTION
  This routine sets up the fixed part of adts frame header.
  
DEPENDENCIES
  FEATURE_AAC_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates aac_adts_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adts_header_fixed (
  uint8  sample_index,
  uint8  channel_config
);

/*===========================================================================

FUNCTION audaac_rec_install_adts_header_variable

DESCRIPTION
  This routine sets up the variable part of adts frame header.
  
DEPENDENCIES
  FEATURE_AAC_REC is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates aac_adts_hdr_bit_index.

===========================================================================*/
LOCAL void audaac_rec_install_adts_header_variable (
  uint16  byte_num
);

#endif /* FEATURE_AAC_REC */
#endif /* AUDAACI_REC_H */

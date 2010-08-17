#ifndef AUDIOSBCENC_H
#define AUDIOSBCENC_H

/*===========================================================================
 B L U E  T O O T H  S B C  E N C O D E R  H E A D E R  F I L E

DESCRIPTION
  This header file contains the definitions that are derived from
  the A2DP specification.

  This is the only SBC header file that needs to be included by the BT drivers.

REFERENCES

  Advanced Audio Distribution Profile Specification, Version 1.0

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/codec/sbc/main/latest/src/audiosbcenc.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/07   pc      Added ONCRPC Meta-comments for remoting
09/18/05   sp      Added support for DSP based SBC encoder
08/29/05   sp      Added P4 header.
5/27/05    sp      Initial version 

===========================================================================*/
#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
/* The data structure used for the SBC codec-specific data */

#define AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO   0x01 
#define AUDIOSBCENC_CHANNEL_MODE_STEREO         0x02 
#define AUDIOSBCENC_CHANNEL_MODE_DUAL_CHANNEL   0x04 
#define AUDIOSBCENC_CHANNEL_MODE_MONO           0x08

#define AUDIOSBCENC_SAMPLING_FREQUENCY_48000    0x10
#define AUDIOSBCENC_SAMPLING_FREQUENCY_44100    0x20
#define AUDIOSBCENC_SAMPLING_FREQUENCY_32000    0x40
#define AUDIOSBCENC_SAMPLING_FREQUENCY_16000    0x80

#define AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS  0x01
#define AUDIOSBCENC_ALLOCATION_METHOD_SNR       0x02

#define AUDIOSBCENC_SUBBANDS_8                  0x04
#define AUDIOSBCENC_SUBBANDS_4                  0x08

#define AUDIOSBCENC_BLOCK_LENGTH_16             0x10
#define AUDIOSBCENC_BLOCK_LENGTH_12             0x20
#define AUDIOSBCENC_BLOCK_LENGTH_8              0x40
#define AUDIOSBCENC_BLOCK_LENGTH_4              0x80

typedef PACKED struct {
  /* Though PACKED key word is not required in the unions below, the compiler
  requires it */

  /* Octet 0: bits 4-7: Sampling Frequency */
  /* Octet 0: bits 0-3: Channel Mode */
  PACKED union {
    uint8 sampling_frequency;
    uint8 channel_mode;
  } octet0;

  /* Octet 1: bits 4-7: Block Length */
  /* Octet 1: bits 2-3: Number of Subbands */
  /* Octet 1: bits 0-1: Allocation Method */
  PACKED union {
    uint8 block_length;
    uint8 subbands;
    uint8 allocation_method;
  } octet1;

  uint8 min_bitpool;
  uint8 max_bitpool;
} audiosbcenc_data_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD audiosbcenc_data_type.octet0 DISC _OBJ_.octet0.sampling_frequency */
/*~ FIELD audiosbcenc_data_type.octet1 DISC _OBJ_.octet1.block_length */
/*~ PARTIAL audiosbcenc_data_type.octet0 */
/*~ PARTIAL audiosbcenc_data_type.octet1 */
/*~ DEFAULT audiosbcenc_data_type.octet0.sampling_frequency */
/*~ DEFAULT audiosbcenc_data_type.octet1.block_length */
#endif /* FEATURE_HTORPC_METACOMMENTS */
#endif /* FEATURE_SBC_CODEC */
#endif /* AUDIOSBCENC_H */

#ifndef SNDGEN_H
#define SNDGEN_H
/*===========================================================================

                S O U N D   T A S K    H E A D E R    F I L E
                   H W   G E N E R A T O R   S U P P O R T

DESCRIPTION
  This header files contains definitions for all of the voice and tone
  generators that the Sound Server supports.

Copyright (c) 1999-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/sndgen.h_v   1.1   12 Mar 2002 21:42:30   stam  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndgen.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/08   mas     Made level_data of snd_gen_voc_cal_type and snd_gen_ring_cal_type 
                   read and writable when AUDIO_CALIBRATION enabled.
                   Updated Copyright
04/21/05   act     Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/12/02    st     Modified snd_gen_level_voc_type to remove codec_rx_gain.
04/04/01    st     Modified code to support new FEATURE_MIDI_OUT feature.
04/13/00    st     Added SND_GEN_MIDI generator.
10/04/99   jkl     Modified for dbm functionality purposes.
09/09/99   jkl     Modifications after code review. 
08/17/99   ro/jkl  created file

===========================================================================*/
 
#include "comdef.h"     /* Definitions for byte, word, etc.       */
#include "target.h"     /* Definitons of FEATURE                  */
#include "snddev.h"     /* Definitions of device, methods         */
#include "voc.h"        /* Vocoder interface definition           */

/* <EJECT> */

/* To build a version for use in setting volume levels, define this symbol.
** When defined, the volume tables will be placed in RAM where values can
** be modified using the DM.
*/
#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
#define CAL_MEMORY
#else
#define CAL_MEMORY   const     /* Normally, tables are in ROM  */
#endif

/* ==========================================================================
**
**                           D E F I N I T I O N S
**  
** =========================================================================*/

/*  All of the hardware voice and tone generators that the Sound Server
**  Supports.
*/
typedef enum {
  SND_GEN_NONE = 0,     /* No sound generator (silence)                      */
  SND_GEN_VOC,          /* Vocoder generated tones                           */
  SND_GEN_RING,         /* Ringer generated tone (can use vocoder for FM TX) */
#ifdef FEATURE_CLICK
#error code not present
#endif
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
  SND_GEN_MIDI,         /* Midi Generator (uses vocoder)                     */
#endif
  SND_GEN_AUX           /* Auxiliary on/off generator                        */
} snd_gen_type;

/* Base type for calibrating volume levels for a generator
*/
typedef struct {
  snd_gen_type  generator;          /* what type of generator is this for  */
  uint16        num_levels;         /* Number of volume levels 0..n        */
  uint16        num_scale;          /* Number of scale levels 0..n         */
  /* generator-specific calibration data goes here */
} snd_gen_cal_type;

/* Volume table for the vocoder
*/
typedef struct {
  sint15 rx_volume_db;   /* Vocoder RX Volume [dB*100]          */
  sint15 dtmf_volume_db; /* Vocoder DTMF Volume [dB*100]        */
  uint8  pad_value;      /* Codec Pad Byte Value (off-chip only)*/
                         /* Calibration type for codec volume (any pcm path)*/
} snd_gen_level_voc_type;    

typedef struct {
  snd_gen_type  generator;          /* What generator does this apply to   */
  uint16        num_levels;         /* Number of volume levels 0..n        */
  uint16        num_scale;          /* Number of scale levels 0..n         */
  
  /* vocoder/device specific calibration data */
  voc_codec_type pcm_path;            /* The PCM path the the vocoder should */
                                      /* use.                                */
  uint16         warmup_time_ms;      /* How many miliseconds for the device */
                                      /* to get ready to play audio after    */
                                      /* it gets switched on                 */
  uint8          pad_mask;            /* Mask of pad bits to control         */
  CAL_MEMORY snd_gen_level_voc_type
                 *level_data;         /* Pointer to the Volume table         */
} snd_gen_voc_cal_type;
 
 
/* Calibration type for ringer volume
*/
typedef struct {
  snd_gen_type   generator;          /* What generator does this apply to   */
  uint16         num_levels;         /* Number of volume levels 0..n        */
  uint16         num_scale;          /* Number of scale levels 0..n         */
  
  /* ringer-specific calibration data */
  CAL_MEMORY uint16   *level_data;        /* Ringer volume                        */
} snd_gen_ring_cal_type;

/* Calibration type for click generator
*/
typedef struct {
  snd_gen_type   generator;          /* What generator does this apply to   */
  uint16         num_levels;         /* Number of volume levels 0..n        */
  uint16         num_scale;          /* Number of scale levels 0..n         */
} snd_gen_click_cal_type;

/* Calibration type for auxilliary generator
*/
typedef struct {
  snd_gen_type   generator;          /* What generator does this apply to   */
  uint16         num_levels;         /* Number of volume levels 0..n        */
  uint16         num_scale;          /* Number of scale levels 0..n         */
} snd_gen_aux_cal_type;

#endif /* SNDGEN_H */


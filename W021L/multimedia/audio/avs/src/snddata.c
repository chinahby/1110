/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Calibration Database")
*/ 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S O U N D   C A L I B R A T I O N   D A T A B A S E
                    A C C E S S   F U N C T I O N S
                       
GENERAL DESCRIPTION
  Provides access to the sound configuration and calibration data.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  snd_data_set_volume_override
    Set flag to determine if override is necessary
  snd_data_get_volume
    Get the volume for a device and method
  snd_data_set_volume
    Get the volume for a device and method
  snd_data_get_generator
    Get the audio generator for a particular [device,method]
  snd_data_get_pcm_path
    Get the PCM audio path for a particular [device,method]
  snd_data_get_num_levels
    Get the number of volume levels for this [device,method]
  snd_data_get_warmup_time
    Get the warm up time necessary for this [device, method] to get ready
  snd_data_get_pad_mask
    Get the pad mask for the internal CODEC gain registers
  snd_data_get_ringer_volume
    Get the ringer volume in dbm for this [device,method]
  snd_data_get_rx_volume
    Get the vocoder rx volume in dbm for this [device,method]
  snd_data_get_dtmf_volume
    Get the vocoder dtmf volume in dbm for this [device,method]
  snd_data_get_pad_value
    Get the pad value for the external CODEC 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999-2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/snddata.c_v   1.1   12 Mar 2002 21:42:08   stam  $
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/snddata.c#4 $ $DateTime: 2009/06/02 23:17:14 $ $Author: bsingh $

when       who     what, where, why
--------   ---     --------------------------------------------- 
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
02/23/07    ay     Fixed compiler warnings.
02/09/07   act     Modified code to, at last, cap the volume index into
                   volume tables. Volume levels set beyond the maximum will
                   now be capped to the maximum volume level set in cal
                   tables.
04/21/05   act     Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
07/23/02    st     Modified code to return a set rx volume setting for
                   snd_data_override mode.
03/12/02    st     Modified code to support DEVICE override for volume instead
                   of METHOD override; removed snd_data_get_codec_rx_gain().
04/04/01    st     Modified code to support new FEATURE_MIDI_OUT feature.
07/15/00    st     Use 8bit value for volume levels; lint updates.
06/11/00   spf     Updated references.
05/18/00    st     Added support for SND_METHOD_MIDI and SND_DEVICE_SDAC.
03/06/00    st     Removed FEATURE_ENHANCED_RINGS from scaling; modified
                   volume override mechanism to simply return maximum volume.
11/29/99   jkl     Removed initialization of snd_data_device_volume.
10/04/99   jkl     Updated for dbm adjustments.
09/09/99   jkl     Included CODEC RX gain.
08/17/99  ro/jkl   Created file.

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions              */
#include "target.h"             /* Target specific definitions                */
#include "comdef.h"             /* Definitions for byte, word, etc.           */
#include "snddev.h"             /* Definitions for audio devices and methods  */
#include "sndgen.h"             /* Definitions of generators                  */
#include "sndcal.h"             /* Interface definition for snd cal data      */
#include "snddata.h"            /* Interface definition for snd data data     */
#include "msg.h"                /* Messages services                          */
/* <EJECT> */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* Set RX volume override value to 1.6dB */
#define SND_DATA_RX_VOL_OVERRIDE_VALUE 160

/* Type to hold volume level for each device and method
*/
typedef uint8 snd_data_volume_type[SND_DEVICE_MAX][SND_METHOD_MAX];

/* ===========================================================================
**
**                       S T A T E   V A R I A B L E S
**  
** =========================================================================*/

snd_data_volume_type snd_data_device_volume;  /* Volume for [device,method] */

uint16  snd_data_scale    = 0;        /* volume scaling                     */
boolean snd_data_override = FALSE;    /* TRUE if override is demanded       */

/* <EJECT> */

/* ===========================================================================
**
**                            F U N C T I O N S
**  
** =========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
/*===========================================================================

FUNCTION snd_data_set_volume_override

DESCRIPTION
  Set flag to determine if volume override is necessary

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  snd_data_override.

===========================================================================*/
void snd_data_set_volume_override (
  boolean override
)
{
  /* If True, then override is required */
  snd_data_override = override;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/*===========================================================================

FUNCTION snd_data_get_volume

DESCRIPTION
  Get the volume for this device x method

DEPENDENCIES
  None.

RETURN VALUE
  The volume level for this device x method

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint8 snd_data_get_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL;
  uint8                volume;

  /* Mechanism so that different devices can share volume settings
  */
  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  volume = snd_data_device_volume[audio_ptr->device_vol][method];
#ifdef SND_OLD_VOLUME_UNITS
  /* For Gemini-like UI, convert from old fixed levels - 0x1f, 0x6f, 0xaf, 0xff
  ** Also support levels 0, 1, ..., max.
  */
  if (volume >=  0x1f) {
    volume = SND_UI_VOL_TO_INDEX(volume);
    volume++;
  }
#endif
  return (volume);
} /* snd_data_get_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
    
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_set_volume

DESCRIPTION
  Set the volume for a device x method

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  snd_data_device_volume[].

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void snd_data_set_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  uint8             volume     /* volume desired        */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  /* Mechanism so that different devices can share volume settings
  */
  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  snd_data_device_volume[audio_ptr->device_vol][method] = volume;
  return;
} /* snd_data_set_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
    
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_generator

DESCRIPTION
  Get the audio generator for a particular [device,method]

DEPENDENCIES
  None.

RETURN VALUE
  The generator used for this [device, method]

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
snd_gen_type snd_data_get_generator (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (audio_ptr->generator);
} /* snd_data_get_generator */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
    
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_pcm_path

DESCRIPTION
  Get the PCM audio path for a particular [device,method]

DEPENDENCIES
  None.

RETURN VALUE
  the CODEC used for this [device, method]

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_codec_type snd_data_get_pcm_path (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  if ((audio_ptr->generator == SND_GEN_VOC)
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
      || (audio_ptr->generator == SND_GEN_MIDI)
#endif
     ) {
    return(((snd_gen_voc_cal_type *)audio_ptr->gen_cal)->pcm_path);
  } else {
    return(VOC_CODEC_NONE);
  }
} /* snd_data_get_pcm_path */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
    
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_num_levels

DESCRIPTION
  Get the maximum volume level for this [device,method]

DEPENDENCIES
  None.

RETURN VALUE
  The maximum volume level

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint16 snd_data_get_num_levels (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (audio_ptr->gen_cal->num_levels);
} /* snd_data_get_num_levels */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_num_scale

DESCRIPTION
  Get the number of volume scaling levels for this [device,method]

DEPENDENCIES
  None.

RETURN VALUE
  The maximum level of scaling

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint16 snd_data_get_num_scale (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (audio_ptr->gen_cal->num_scale);
} /* snd_data_get_num_scale */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_set_scale

DESCRIPTION
  Set the scaling factor 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  snd_data_scale

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void snd_data_set_scale (
  uint16            set_scale
)
{
  snd_data_scale = set_scale;
} /* snd_data_set_scale */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_scaled_volume

DESCRIPTION
  Get the volume for a device x method associated with the scaling.
  If the max_scale = 0, it means that the scaling is disabled

DEPENDENCIES
  snd_data_scale

RETURN VALUE
  The volume level after scaling

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint16 snd_data_get_scaled_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume before scale   */             
)
{
  uint16            max_scale;

  max_scale = snd_data_get_num_scale( device, method );
  
  if (snd_data_scale >= max_scale) {
    MSG_ERROR("Scale level is %d, max allowed = %d",snd_data_scale,max_scale,0);
    snd_data_scale = max_scale - 1;
  }

  if (max_scale == 0 || volume == 0) {
    return (volume);
  } else {
    /* Casting to uint16 to get rid of lint warning. volume * max_scale
    ** will never exceed a 16-bit number.
    */
    return ((uint16) ((volume * max_scale) - snd_data_scale));
  }
} /* snd_data_get_scaled_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
    
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_warmup_time

DESCRIPTION
  Get the number of ms for the device to become ready to use

DEPENDENCIES
  None.

RETURN VALUE
  The warm up time value

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint16 snd_data_get_warmup_time (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (((snd_gen_voc_cal_type *) audio_ptr->gen_cal)->warmup_time_ms);
} /* snd_data_get_warmup_time */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_pad_mask

DESCRIPTION
  Get the pad mask for the internal CODEC

DEPENDENCIES
  None.

RETURN VALUE
  The pad mask 

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint8 snd_data_get_pad_mask (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (((snd_gen_voc_cal_type *)audio_ptr->gen_cal)->pad_mask);
} /* snd_data_get_pad_mask */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */


/*===========================================================================

FUNCTION snd_data_get_gen_cal

DESCRIPTION
  Get a pointer to the generator-specific level calibration table for
  this  [device,method]

DEPENDENCIES
  None.

RETURN VALUE
  The pointer to the generator calibration data
 
SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
snd_gen_cal_type *snd_data_get_gen_cal (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
)
{
  snd_cal_control_type *audio_ptr = NULL; /* Pointer to control data       */

  (void) snd_cal_get_audio_control(device,method,&audio_ptr);
  return (audio_ptr->gen_cal);
} /* snd_data_get_gen_cal */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_ringer_volume

DESCRIPTION
  Get the ringer volume

DEPENDENCIES
  None.

RETURN VALUE
  The ringer volume

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint16 snd_data_get_ringer_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
)
{
  snd_gen_cal_type  *gen_cal;  /* pointer to calibration table */
  uint16            max_index;
  uint16            index;     /* volume table index           */

  index   = snd_data_get_scaled_volume( device, method, volume );
  gen_cal = snd_data_get_gen_cal( device, method );
  max_index = (uint16) ( ((snd_gen_ring_cal_type *) gen_cal)->num_levels 
                       * ((snd_gen_ring_cal_type *) gen_cal)->num_scale );

  if(index > max_index) {
    index = max_index;
  }

  return (((snd_gen_ring_cal_type *) gen_cal)->level_data[index]);
} /* snd_data_get_ringer_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION snd_data_get_midi_volume

DESCRIPTION
  Get the midi volume

DEPENDENCIES
  None.

RETURN VALUE
  The midi volume

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
sint15 snd_data_get_midi_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
)
{
  snd_gen_voc_cal_type  *gen_cal;  /* pointer to calibration table */
  uint16            index;     /* volume table index           */
  uint16            max_index;
  sint15            rx_vol;

  index   = snd_data_get_scaled_volume( device, method, volume );
  gen_cal = (snd_gen_voc_cal_type *)snd_data_get_gen_cal( device, method );

  max_index = (uint16) (gen_cal->num_levels * gen_cal->num_scale);

  if(index > max_index) {
    index = max_index;
  }
  rx_vol = gen_cal->level_data[index].rx_volume_db;

  return rx_vol;
} /* snd_data_get_midi_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT) */
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_rx_volume

DESCRIPTION
  Get the vocoder rx volume

DEPENDENCIES
  None.

RETURN VALUE
  The rx volume in dbm

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
sint15 snd_data_get_rx_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
)
{
  snd_gen_voc_cal_type  *gen_cal;  /* pointer to calibration table */
  uint16            index;         /* volume table index           */
  uint16            max_index;
  sint15            rx_vol;

  /* If override is requested, use set volume level */
  if((snd_data_override) && (method == SND_METHOD_VOICE) && (volume != 0)) {
    rx_vol = SND_DATA_RX_VOL_OVERRIDE_VALUE;
  } else {
    gen_cal = (snd_gen_voc_cal_type *)snd_data_get_gen_cal( device, method );

    index  = snd_data_get_scaled_volume( device, method, volume );

    max_index = (uint16) (gen_cal->num_levels * gen_cal->num_scale);
    if(index > max_index) {
      index = max_index;
    }
    rx_vol = gen_cal->level_data[index].rx_volume_db;
  }

  return (rx_vol);
} /* snd_data_get_rx_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_dtmf_volume

DESCRIPTION
  Get the vocoder dtmf volume

DEPENDENCIES
  None.

RETURN VALUE
  The dtmf volume in dbm

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
sint15 snd_data_get_dtmf_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
)
{
  snd_gen_voc_cal_type  *gen_cal;  /* pointer to calibration table */
  uint16            max_index;
  uint16            index;     /* volume table index           */

  gen_cal = (snd_gen_voc_cal_type *)snd_data_get_gen_cal( device, method );

  max_index = (uint16) (gen_cal->num_levels * gen_cal->num_scale);

  /* If override is requested, use max volume level */
  if((snd_data_override) && (method == SND_METHOD_VOICE) && (volume != 0)) {
    /* Casting to uint16 to get rid of lint warning. num_levels * num_scale
    ** will never exceed a 16-bit number.
    */
    index = max_index;
  } else {
    index = snd_data_get_scaled_volume( device, method, volume );
    if(index > max_index) {
      index = max_index;
    }
  }

  return (((snd_gen_voc_cal_type *)gen_cal)->level_data[index].dtmf_volume_db);
} /* snd_data_get_dtmf_volume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_pad_value

DESCRIPTION
  Get the CODEC pad value

DEPENDENCIES
  None.

RETURN VALUE
  The pad value for the external CODEC

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint8 snd_data_get_pad_value (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
)
{
  snd_gen_cal_type  *gen_cal;  /* pointer to calibration table */
  uint16            index;     /* volume table index           */
  uint16            max_index;

  gen_cal = snd_data_get_gen_cal( device, method );
    
  max_index = (uint16) (gen_cal->num_levels * gen_cal->num_scale);

  /* If override is requested, use max volume level */
  if((snd_data_override) && (method == SND_METHOD_VOICE) && (volume != 0)) {
    /* Casting to uint16 to get rid of lint warning. num_levels * num_scale
    ** will never exceed a 16-bit number.
    */
    /* Casting to uint16 to get rid of lint warning. num_levels * num_scale
    ** will never exceed a 16-bit number.
    */
    index = max_index;
  } else {
    index = snd_data_get_scaled_volume( device, method, volume );
    if(index > max_index) {
      index = max_index;
    }
  }

  return (((snd_gen_voc_cal_type *) gen_cal)->level_data[index].pad_value);
} /* snd_data_get_pad_value */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */

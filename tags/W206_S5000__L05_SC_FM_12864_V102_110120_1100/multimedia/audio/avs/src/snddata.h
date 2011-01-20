#ifndef SNDDATA_H
#define SNDDATA_H
/*===========================================================================

          S O U N D   C A L I B R A T I O N   D A T A B A S E
        A C C E S S   F U N C T I O N S  H E A D E R    F I L E 
                   

DESCRIPTION
  This header files contains definitions for all of the voice and tone
  generators that the Sound Server supports.

Copyright (c) 1999-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/snddata.h_v   1.1   12 Mar 2002 21:42:16   stam  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/snddata.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/05   act     Reverted name of voc_codec_id_type to voc_codec_type.
04/01/05   act     Changed use of specific codec IDs to generic codec types
                   where applicable.
03/12/02    st     Removed snd_data_get_codec_rx_gain() prototype.
04/04/01    st     Modified code to support new FEATURE_MIDI_OUT feature.
02/28/01    st     Removed targetization.
07/15/00    st     Use 8bit value for volume levels; lint updates.
04/13/00    st     Added snd_data_get_midi_volume function prototype.
03/06/00    st     Removed FEATURE_ENHANCED_RINGS from scaling.
10/04/99   jkl     Updated for dbm adjustments.
08/17/99  jkl/ro   Created file.

===========================================================================*/
 
#include "comdef.h"     /* Definitions for byte, word, etc.       */
#include "target.h"     /* Target Specific Defintions             */
#include "snddev.h"     /* Definitions of device, methods         */
#include "sndgen.h"     /* Definitions of generator               */
#include "voc.h"        /* Vocoder interface definition           */

/* <EJECT> */

/* ==========================================================================
**
**                           D E F I N I T I O N S
**  
** =========================================================================*/
/* Target uses Gemini-like UI volume levels. 
*/
/* This define is necessary to support old volume units. If volume levels used
** are consecutive and increasing from 0 then this define is not necessary.
*/
#define SND_OLD_VOLUME_UNITS

#ifdef SND_OLD_VOLUME_UNITS
/* This Macro converts a UI Volume into a sndhw_ctl_type Index.         */

#define SND_UI_VOL_TO_INDEX(vol)        ((vol>>6) & 0x3)
#endif 

/* ===========================================================================
**
**                 F U N C T I O N   D E F I N I T I O N S
**  
** =========================================================================*/
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
extern void snd_data_set_volume_override ( boolean override );
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
uint8 snd_data_get_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
    
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
void snd_data_set_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  uint8             volume
);
    
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
extern snd_gen_type snd_data_get_generator (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
    
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
extern voc_codec_type snd_data_get_pcm_path (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
    
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
extern uint16 snd_data_get_num_levels (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);

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
extern uint16 snd_data_get_num_scale (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
/*===========================================================================

FUNCTION snd_data_set_scale

DESCRIPTION
  Set the scaling factor

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  snd_data_scale.

===========================================================================*/
extern void snd_data_set_scale (
  uint16            set_scale
);
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
extern uint16 snd_data_get_warmup_time (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
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
extern uint8 snd_data_get_pad_mask (
  snd_device_type   device,    /* device                */
  snd_method_type   method     /* method                */
);
/* <EJECT> */

/*===========================================================================

FUNCTION snd_data_get_ringer_volume

DESCRIPTION
  Get the ringer volume

DEPENDENCIES
  None.

RETURN VALUE
  The ringer volume in dbm

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 snd_data_get_ringer_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
);

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
extern sint15 snd_data_get_midi_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
);
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
extern sint15 snd_data_get_rx_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
);

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
extern sint15 snd_data_get_dtmf_volume (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
);

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
extern uint8 snd_data_get_pad_value (
  snd_device_type   device,    /* device                */
  snd_method_type   method,    /* method                */
  byte              volume     /* volume level          */
);


#endif /* SNDDATA_H */


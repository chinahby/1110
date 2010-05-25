#ifndef SNDCAL_H
#define SNDCAL_H
/*===========================================================================

             S O U N D   C A L I B R A T I O N   D A T A B A S E
    S T O R A G E   A N D   R E T R I E V A L   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to access the
  raw data contained in the audio calibration database.
  
Copyright (c) 1999-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/sndcal.h_v   1.1   12 Mar 2002 21:42:00   stam  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndcal.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   mas     Renamed AUDIO_CALIBRATION to FEATURE_AVS_DYNAMIC_CALIBRATION
                   Updated Copyright
03/12/02    st     Modified snd_cal_control_type to support doing DEVICE
                   override for volume instead of METHOD override.
10/04/99   jkl     Included AUDIO_CALIBRATION for Calibration Test.
08/17/99  ro/jkl   Created file.

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.          */
#include "snddev.h"             /* Definitions for audio devices and methods */
#include "sndgen.h"             /* Definitions for HW sound generators       */
#include "voc.h"                /* Vocoder interface definition              */

/* <EJECT> */

/* ===========================================================================
**
**                           D E F I N I T I O N S
**  
** =========================================================================*/
/* To build a version for use in setting volume levels, define this symbol.
** When defined, the volume tables will be placed in RAM where values can
** be modified using the DM.
*/
#ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
#define VOL_MEMORY
#else
#define VOL_MEMORY   const     /* Normally, tables are in ROM  */
#endif

/* Function return values
*/
typedef enum {
  SND_CAL_SUCCESS,
  SND_CAL_FAILED
} snd_cal_return_type;


/* <EJECT> */

/* ===========================================================================
**
**                      D A T A   S T R U C T U R E S
**  
** =========================================================================*/

/* Main calibration type for device and method. Determines how voice or
** sounds will be played to a specific generator.
*/
typedef struct {
  snd_device_type    device_vol; /* Indicates whether this device/method  */
                                 /* pair should use the volume setting    */
                                 /* of method from another device         */
  snd_gen_type       generator;  /* The sound generator to use to play    */
                                 /* the requested sound                   */
  snd_gen_cal_type   *gen_cal;   /* Generator and Volume-level            */
                                 /* specific calibration values           */
} snd_cal_control_type;

/* Data structure to contain all calibration data for one audio device
*/
typedef snd_cal_control_type 
        snd_cal_device_calibration_type[SND_METHOD_MAX];

/* ===========================================================================
**
**                 F U N C T I O N   D E F I N I T I O N S
**  
** =========================================================================*/
/*===========================================================================

FUNCTION snd_cal_get_audio_control

DESCRIPTION
  Get the pointer to the audio calibration data according to this
  [device, method]
  
DEPENDENCIES
  None.

RETURN VALUE
  SND_CAL_SUCCESS - data retrieved successfully
  SND_CAL_FAILED  - could not get data

SIDE EFFECTS
  None.

===========================================================================*/
extern snd_cal_return_type snd_cal_get_audio_control (
  snd_device_type      device,         /* device                             */
  snd_method_type      method,         /* method                             */
  snd_cal_control_type **audio_config  /* pointer of pointer to audio_config */
);

/* <EJECT> */


#endif /* SNDCAL_H */

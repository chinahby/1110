#ifndef AUDADSP_MIXERSTATE_H
#define AUDADSP_MIXERSTATE_H
/*===========================================================================

                   Q D S P 4   M I X E R    D R I V E R
           S T A T E   C O N T R O L L E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions for functions and data used by the
  QDSP2 Vocoder Driver State Machine.

REFERENCES
  None.

Copyright (c) 1999 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/audadsp_mixer.h#2 $ $DateTime: 2008/07/31 05:20:27 $ $Author: skray $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/08   skr     Changes to release VOCODER & ADEC separately.
01/07/08    az     Included the in call midi and the new in call architecture.
11/02/07    az     Included the support for the in call wma.
12/11/06    sp     Fixed dsp crash during vocoder handoff.
09/01/06    hz     Added FEATURE_AVS_INCALL_MIXER_ADEC_PB protection.
08/03/06    hz     Initial Version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"           /* Customer feature specific              */
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "rex.h"                /* definition of REX data types           */
#include "voc.h"                /* Vocoder interface definition           */
#include "voci.h"               /* Internal Vocoder Task Definitions      */
#include "qdsp.h"               /* QDSP Services                          */

#include "audadsp_vocoder.h"
#include "audadsp_adec.h"

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S
               
===========================================================================*/

/* ENUMs to describe the entity state of the mixer.
 */
typedef enum {
  AUDADSP_MIXER_STATE_NONE       = 0,
  AUDADSP_MIXER_STATE_VOC        = (0x0001 << 1),
  AUDADSP_MIXER_STATE_ADEC       = (0x0001 << 2),
  AUDADSP_MIXER_STATE_VOC_ADEC   = (0x0001 << 3),
  AUDADSP_MIXER_STATE_MAX                = (0x0001 << 7)
} audadsp_mixer_state_entity_state_type;

/* mixer control block.
 */ 
typedef struct {
  audadsp_mixer_state_entity_state_type entity_state;
} audadsp_mixer_state_control_type;

/* Return value of each of entities when state
 * transition happens.
 */
typedef enum {
  AUDADSP_MIXER_RETURN_NONE       = 0,

  AUDADSP_MIXER_RETURN_RESET      = 0x0001 << 1,
  AUDADSP_MIXER_RETURN_DONE       = 0x0001 << 2, 
  AUDADSP_MIXER_RETURN_CONTINUE   = 0x0001 << 3, 
  AUDADSP_MIXER_RETURN_IMMEDIATE  = 0x0001 << 4,
  AUDADSP_MIXER_RETURN_ERROR      = 0x0001 << 5,

  AUDADSP_MIXER_RETURN_MAX        = 0x0001 << 6
} audadsp_mixer_entity_return_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S                
  
===========================================================================*/
/*===========================================================================

FUNCTION audadsp_mixer_adec_ready

DESCRIPTION
  This is to check the adec state.

DEPENDENCIES
  None.  

RETURN VALUE
  TRUE  -- for command sending successful.
  FALSE -- for command sending failed.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean audadsp_mixer_adec_ready(void);

/*===========================================================================

FUNCTION audadsp_mixer_state_adec_cfg_cmd

DESCRIPTION
  Sends the audio config command.

DEPENDENCIES
  None.  

RETURN VALUE
    
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean audadsp_mixer_state_adec_cfg_cmd(void);


/*===========================================================================

FUNCTION audadsp_mixer_state_voc_cfg_cmd

DESCRIPTION
  Sends the vocoder config command.

DEPENDENCIES
  None.  

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean audadsp_mixer_state_voc_cfg_cmd(void);

/*===========================================================================

FUNCTION audadsp_mixer_state_mixer_codec_cfg_cmd

DESCRIPTION
  Sends the mixer codec config command.

DEPENDENCIES
  None.  

RETURN VALUE
  TRUE  -- for command sending successful.
  FALSE -- for command sending failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean audadsp_mixer_state_mixer_codec_cfg_cmd (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_init

DESCRIPTION
  First virtual state after DSP IMAGE is loaded. DSP itself does not have
  this state.

DEPENDENCIES
  voc_state_data information.

RETURN VALUE
  Return values for QDSP2 State Control Functions.

SIDE EFFECTS
  NONE.
===========================================================================*/
voc_state_return_type audadsp_mixer_state_init (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_idle_entry

DESCRIPTION
  Wait for the QDSP2 to reach IDLE state.  When QDSP is in IDLE, inform
  users, and transition the driver to IDLE state. If the QDSP does not reach
  IDLE within the allowed time, then perform error recovery.
 
DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
voc_state_return_type audadsp_mixer_state_idle_entry (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_idle

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to IDLE state.  If
  the vocoder is released, turn off the clocks;

  The majority done in this function is for vocoder init state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  NONE.
===========================================================================*/
voc_state_return_type audadsp_mixer_state_idle (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_active_entry

DESCRIPTION
  When the DSP to arrives in VOCODER state, set the volume, inform users
  of the state change, and transition state machine to VOCODER state and
  ADEC state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
voc_state_return_type audadsp_mixer_state_active_entry (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_active

DESCRIPTION
  When the DSP entities arrive in VOCODER and ADEC state, set the volume, inform 
  users of the state change, and stays there until something changed - may resend
  mixer codec config command.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
voc_state_return_type audadsp_mixer_state_active (void);

/*===========================================================================

FUNCTION audadsp_mixer_state_active_go_idle

DESCRIPTION
  The entry point to driver the mixer to goes into idle state. It could be
  from INIT state or ACTIVE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
voc_state_return_type audadsp_mixer_state_active_go_idle (void);

/*===========================================================================

FUNCTION audadsp_mixer_voc_state_control

DESCRIPTION
  Mixer uses this function to driver vocoder entity state transition.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  NONE.
===========================================================================*/
extern audadsp_mixer_entity_return_type audadsp_vocoder_state_control (void);

/*===========================================================================

FUNCTION audadsp_mixer_adec_state_control

DESCRIPTION
  Mixer uses this function to driver audio entity state transition.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  NONE

===========================================================================*/
extern audadsp_mixer_entity_return_type audadsp_adec_state_control (void);

/*===========================================================================

FUNCTION: audadsp_mixer_state_get_mixer_mode  

DESCRIPTION
  This function gets the vocoder+audio mixing state currently configured. 

DEPENDENCIES
  voc_state_data information.

RETURN VALUE
  Audio+Vocoder mixer mode description.

SIDE EFFECTS
  None.
===========================================================================*/
extern qdsp_mixer_mode_type audadsp_mixer_state_get_mixer_mode (
  audadsp_mixer_state_entity_state_type *entity_state
);

/*===========================================================================

FUNCTION: audadsp_adec_state_qtunes_config  

DESCRIPTION
  This function gets the wma configuration before sending to the DSP. 

DEPENDENCIES
  voc_state_data information.

RETURN VALUE
  Audio+Vocoder mixer mode description.

SIDE EFFECTS
  None.
===========================================================================*/
void audadsp_adec_state_qtunes_config (void);

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB ||
          FEATURE_AVS_INCALL_MIXER_CMX */
#endif /* AUDADSP_MIXERSTATE_H */


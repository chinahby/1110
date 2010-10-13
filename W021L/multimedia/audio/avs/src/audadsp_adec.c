/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP Device Driver")
*/ 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 4   M I X E R   A U D I O   D R I V E R
                       S T A T E   C O N T R O L L E R

GENERAL DESCRIPTION
  Controls the state of the QDSP Vocoder DSP.  The QDSP Maintains the
  following internal states:
                 
      INIT <----> ADEC_ENTRY <-----> ADEC

  This module, via the voc_state_control function, drives the QDSP through
  the required state transitions and monitors the results.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  voc_state_get_state_data
    Returns a pointer to the state control structure.
  voc_state_get_inform_val
    Returns the current state, converted to user value.
  voc_state_control
    The QDSP State controller.
  voc_state_control_init
    Initializes the state controller.
  voc_state_enable_clocks
    Enables vocoder and codec clocks.
  voc_state_disable_clocks
    Disables vocoder and codec clocks.
  voc_state_set_codec_cal
    Sets the calibration for the codec specified.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2009 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/audadsp_adec.c#5 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when       who     what, where, why
--------   ---    -------------------------------------------------------------
10/14/09   yp      CMI changes for SCMM 
06/30/09   kk      Fixed CR 181984 - Incall(MO & MT):After playing MP3 clip,
                   First AAC clip plays but with no audio in Continuous mode,  
01/19/09   skr     Send Adec config in audadsp_adec_state_qtunes_init() for WMA
                   playback.
05/09/08   skr     changes to prevent codec config being sent during the mixer 
                   exit sequence    
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
01/07/08    az     Included the in call cmx and in call audio architecture.
12/18/07    sj     Removed fetaure FEATURE_MULTIMEDIA_CODEC_GAIN from 
                   custavs.h
                   so call to voc_state_write_codec_gain() function
                   is no longer featurized now.
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/08/07    az     Fixed the merging issues.
11/02/07    az     Included the support for the in call WMA.
10/26/07    az     Included the In call AAC support.
10/15/07    az     Included support for the crystal speech for in call audio.
12/11/06    sp     Fixed dsp crash during vocoder handoff.
09/01/06    hz     Added FEATURE_AVS_INCALL_MIXER_ADEC_PB protection.
08/03/06    hz     Initial Version.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "rex.h"                /* definition of REX data types            */
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif 
#ifdef FEATURE_CMI_MM
#error code not present
#endif       
/* Clock prototypes                        */    
#include "task.h"               /* Task definitions and prototypes         */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#include "hw.h"                 /* Hardware specific definitions           */

#if defined(T_SLEEP) || defined(FEATURE_SLEEP_TASK)
#include "sleep.h"              /* Sleep signals                           */
#endif /* T_SLEEP || FEATURE_SLEEP_TASK */

#include "msmaud.h"             /* MSMAudio features                       */
#include "voc.h"                /* Vocoder interface definition            */
#include "vocsup.h"             /* Vocoder Support Functions               */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions  */
#include "vocstate.h"           /* Vocoder State Controller Definitions    */
#include "qdspcmd.h"            /* QDSP Driver Command Layer definitions   */
#include "qdspdown.h"           /* QDSP download module                    */
#include "qdspext.h"            /* QDSP firmware image definitions         */
#include "clkregim.h"           /* For clk_regime_enable/disable.          */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
#include "audadsp_mixer.h"      /*                                         */
#include "audadsp_adec.h"       /*                                         */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
** Vocoder driver State Control Types
**-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
** Vocoder driver State Control Data
**-----------------------------------------------------------------------*/

#define AUDADSP_ADEC_SLEEP_CMD_SIZE  2
#define AUDADSP_ADEC_SLEEP_CMD_HDR   0x0002 

audadsp_adec_state_ctl_type audadsp_adec_ctl;
static boolean audadsp_adec_acquired = FALSE;

/*-------------------------------------------------------------------------
** ADSP Error recovery vars
**-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
/*===========================================================================

FUNCTION audadsp_adec_state_qtunes_init_entry

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to QTUNES_IDLE state. 
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qtunes_init_entry (void)
{
  
  if (qdsp_cmd_get_adec_state() != QDSP_STATE_INIT) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (audadsp_adec_ctl.retry_counter < 100) {

      MSG_HIGH("MIXER+ADEC adecState INIT transition retry", 0,0,0);
      audadsp_adec_ctl.retry_counter++;
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("MIXER+ADEC firmware adecState failed to reach INIT", 0,0,0);
      audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
      return(AUDADSP_MIXER_RETURN_ERROR);
    }
  }

  audadsp_adec_ctl.retry_counter = 0;
  audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT;

  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_adec_state_qtunes_init_entry */
   
/*===========================================================================

FUNCTION audadsp_adec_state_qtunes_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to QTUNES_IDLE state. 
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qtunes_init (void)
{
  voc_state_control_type   *state;

  state = voc_state_get_state_data();
  MSG_LOW("audadsp_adec_acquired: %d, state->tunes_config_pending :%d",
                    audadsp_adec_acquired,state->tunes_config_pending,0);
  MSG_LOW("state->config: %d, state->config_pending :%d",
                                  state->config,state->config_pending,0);

  /* update config with config_pending to send correct config to DSP */
  if(state->config != state->config_pending){
     state->config = state->config_pending;
  }

  if (audadsp_adec_acquired == TRUE) {

    state->tunes_config_pending = FALSE;
#ifdef FEATURE_QTUNES
    if(state->config == VOC_CAP_QTUNES) {
      MSG_MED("MIXER AUDIO INIT: MP3",0,0,0);
      /* send DSP configuration information */
      qdsp_cmd_set_mp3_config((qdsp_cmd_mp3_config_type*)&(state->mp3_config));
    }
#endif
#ifdef FEATURE_QTUNES_AAC
    else if(state->config == VOC_CAP_QTUNES_AAC) {
      MSG_MED("MIXER AUDIO INIT: AAC",0,0,0);
      qdsp_cmd_set_aac_config((qdsp_cmd_aac_config_type*)&(state->aac_config));
    }
#endif
#ifdef FEATURE_WMA
    else if(state->config == VOC_CAP_QTUNES_WMA) {
      MSG_MED("MIXER AUDIO INIT: WMA",0,0,0);
      audadsp_adec_state_qtunes_config ();
    }
#endif
    else {
      MSG_MED("MIXER AUDIO INIT: Incompatible Audio",0,0,0);
    }

    /* Setup audio format pro-processing calibration blocks */
    voc_set_audfmt_pp_cal();

    /* Issue mixer codec configuration command */
    if (audadsp_mixer_state_adec_cfg_cmd() == FALSE) {

      if (audadsp_adec_ctl.retry_counter < 100) {

        MSG_HIGH("mixer_adec_config retry", 0,0,0);
        audadsp_adec_ctl.retry_counter++;
        return(AUDADSP_MIXER_RETURN_CONTINUE);
      } else {
        
        /* If a retry has already been attempted, transition to ERROR state */
        MSG_ERROR("failed to issue mixer_adec_config after MAX attempts", 
                  0,0,0);
        audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
        return(AUDADSP_MIXER_RETURN_ERROR);
      }
    }
    state->exit_mixer_state =FALSE;

    audadsp_adec_ctl.retry_counter = 0;
    audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_ACTIVE_ENTRY;
    /* Transition to the Qtunes active entry state */
    MSG_HIGH("Mixer going to MIXER TUNES ACTIVE ENTRY",0,0,0);
  }

  return (AUDADSP_MIXER_RETURN_CONTINUE);
} /* audadsp_adec_state_qtunes_init */

/*===========================================================================

FUNCTION audadsp_adec_state_qtunes_entry

DESCRIPTION
  Wait for the QDSP2 to reach QTUNES state.  When QDSP2 is in QTUNES, inform
  users, and transition the driver to QTUNES state. If the QDSP2 does not
  reach QTUNES within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qtunes_entry (void)
{
  voc_state_control_type     *state;

  state = voc_state_get_state_data();

  if (qdsp_cmd_get_adec_state() != QDSP_STATE_TUNES) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (audadsp_adec_ctl.retry_counter < 100) {

      MSG_HIGH("MIXER+ADEC adecState transition retry", 0,0,0);
      audadsp_adec_ctl.retry_counter++;
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("Firmware failed to reach VOCODER+ADEC adecState", 0,0,0);
      audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
      return(AUDADSP_MIXER_RETURN_ERROR);
    }
  }

  audadsp_adec_ctl.retry_counter = 0;
     	#if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
            clk_busy_wait(3000); 		
 	#else
#error code not present
	#endif

  /* Configure ADEC interrupts and activate the buffer initialization */
  (void) qdsp_cmd_adec_config(voc_adec_isr);
  audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_ACTIVE;

  MSG_HIGH("Entered QTUNES state",0,0,0);

  /* Inform users of transition to QTUNES state */
  voc_inform(VOC_INFORM_MIXER_ADEC);

  /* Setup audio format pro-processing blocks
  */
  voc_set_audfmt_pp_config();

#ifdef FEATURE_MULTIMEDIA_CODEC_GAIN
  /* Write codec gain */
  (void)voc_state_write_codec_gain();
#endif /* FEATURE_MULTIMEDIA_CODEC_GAIN */

  return (AUDADSP_MIXER_RETURN_CONTINUE);
} /* audadsp_adec_state_qtunes_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION audadsp_adec_state_qtunes

DESCRIPTION
  Holding pattern for Qtunes functions.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qtunes (void)
{
  uint16 sleep_cmd[2] = {AUDADSP_ADEC_SLEEP_CMD_SIZE, 
                         AUDADSP_ADEC_SLEEP_CMD_HDR};

  voc_state_control_type     *state;

  qdsp_cmd_status_type     result;

  state = voc_state_get_state_data();
  
  /* Clear any previous error condition */
  voc_state_clear_fw_err_cnt();

  if ((audadsp_adec_acquired == FALSE) ||
      (state->tunes_config_pending == TRUE)) {

    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);
    
#if defined(FEATURE_ARM_POST_PROCESSING) || \
    defined (FEATURE_AVS_WB_PCM_RX_INTERFACE)
    qdsp_cmd_register_pp_cb(NULL, QDSP_CMD_PP_TYPE_NONE);
    voc_state_set_pp_status(VOC_PP_STATUS_DISABLED);
#endif /* (FEATURE_ARM_POST_PROCESSING) || (FEATURE_AVS_WB_PCM_RX_INTERFACE) */

    result = qdsp_adec_command(sleep_cmd);

    if (result == QDSP_CMD_SUCCESS) {
      audadsp_adec_ctl.state  = AUDADSP_ADEC_STATE_INIT;
      audadsp_adec_ctl.retry_counter = 0;
      qdsp_write(QDSP_adecDataHostStickyInt, 0);
      state->exit_mixer_state = TRUE;

    } else {

      MSG_HIGH("Sleep command retrying",0,0,0);
      audadsp_adec_ctl.retry_counter++;
      return (AUDADSP_MIXER_RETURN_CONTINUE);

      if (state->retry_counter == 100) {
        MSG_ERROR( "Failed to issue SLEEP command", 0,0,0 );
        audadsp_adec_ctl.state  = AUDADSP_ADEC_STATE_INIT_ENTRY;
        return (AUDADSP_MIXER_RETURN_ERROR);
      }
    }
  } else {

    /* Configure ADEC interrupts and activate the buffer initialization */
    qdsp_cmd_set_adec_int_cb(voc_adec_isr);
    if (state->config != state->config_pending) {

      state->config = state->config_pending;
      voc_inform(VOC_INFORM_MIXER_ADEC);
    }
  }

  return (AUDADSP_MIXER_RETURN_DONE);
} /* audadsp_adec_state_qtunes */
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */
     
#ifdef FEATURE_AVS_INCALL_MIXER_CMX
/*===========================================================================

FUNCTION audadsp_adec_state_qsynth_init_entry

DESCRIPTION
  
DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qsynth_init_entry (void)
{
    
  if (qdsp_cmd_get_adec_state() != QDSP_STATE_INIT) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (audadsp_adec_ctl.retry_counter < 100) {

      MSG_HIGH("MIXER+ADEC adecState INIT transition retry", 0,0,0);
      audadsp_adec_ctl.retry_counter++;
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("MIXER+ADEC firmware adecState failed to reach INIT", 0,0,0);
      audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
      return(AUDADSP_MIXER_RETURN_ERROR);
    }
  }

  audadsp_adec_ctl.retry_counter = 0;
  audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT;

  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_adec_state_qtunes_init */

/*===========================================================================

FUNCTION audadsp_adec_state_qsynth_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to QSYNTH_IDLE state. 
  If the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qsynth_init (void)
{

  voc_state_control_type   *state;

  state = voc_state_get_state_data();

  if (audadsp_adec_acquired == TRUE) {

    /* Set QSynth hybrid mode */
    qdsp_cmd_set_hybrid_mode(state->hybrid_mode_pending);
    state->hybrid_mode = state->hybrid_mode_pending;

    /* Set QSynth volume level mode */
    qdsp_cmd_set_qsynth_vol_mode(
                            (uint16)state->qsynth_vol_mode_pending);
    state->qsynth_vol_mode = state->qsynth_vol_mode_pending;

    /* Set QSynth DLS configuration */
    qdsp_cmd_set_dls_config(voc_data_get_dls_config());

    /* Setup audio format pro-processing calibration blocks */
    voc_set_audfmt_pp_cal();

    /* Issue mixer codec configuration command */
    if (audadsp_mixer_state_adec_cfg_cmd() == FALSE) {

      if (audadsp_adec_ctl.retry_counter < 100) {

        MSG_HIGH("mixer_adec_config retry", 0,0,0);
        audadsp_adec_ctl.retry_counter++;
        return(AUDADSP_MIXER_RETURN_CONTINUE);
      } else {
        
        /* If a retry has already been attempted, transition to ERROR state */
        MSG_ERROR("failed to issue mixer_adec_config after MAX attempts", 
                  0,0,0);
        audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
        return(AUDADSP_MIXER_RETURN_ERROR);
      }
    }
    state->exit_mixer_state = FALSE;

    audadsp_adec_ctl.retry_counter = 0;
    audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_ACTIVE_ENTRY;
    /* Transition to the QSYNTH CONFIG state */
    MSG_HIGH("Mixer going to MIXER SYNTH ACTIVE ENTRY",0,0,0);
  }

  return (AUDADSP_MIXER_RETURN_CONTINUE);
} /* audadsp_adec_state_qtunes_init */

/*===========================================================================

FUNCTION audadsp_adec_state_qsynth_entry

DESCRIPTION
  Wait for the QDSP2 to reach QSYNTH state.  When QDSP2 is in QSYNTH, inform
  users, and transition the driver to QSYNTH state. If the QDSP2 does not
  reach QSYNTH within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qsynth_entry (void)
{


  if (qdsp_cmd_get_adec_state() != QDSP_STATE_SYNTH) {
    /* If QDSP failed to reach ADEC+VOCODER state. Attempt retry, max 1s */
    if (audadsp_adec_ctl.retry_counter < 100) {

      MSG_HIGH("MIXER adecState ACTIVE transition retry", 0,0,0);
      audadsp_adec_ctl.retry_counter++;
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("Firmware adectState failed to reach ACTIVE", 0,0,0);
      audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_INIT_ENTRY;
      return(AUDADSP_MIXER_RETURN_ERROR);
    }
  }

  audadsp_adec_ctl.retry_counter = 0;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
  (void) qdsp_cmd_set_adec_timer_cb(voc_set_adec_cmd_retry_timer);
  (void) qdsp_cmd_adec_config(voc_adec_isr);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
  
  audadsp_adec_ctl.state = AUDADSP_ADEC_STATE_ACTIVE;

  MSG_HIGH("Entered INCALL QSYNTH state",0,0,0);

  /* Inform users of transition to QSYNTH state */
  voc_inform(VOC_INFORM_QSYNTH);

  /* Setup audio format pro-processing blocks
  */
  voc_set_audfmt_pp_config();

#ifdef FEATURE_MULTIMEDIA_CODEC_GAIN
  /* Write codec gain */
  (void)voc_state_write_codec_gain();
#endif /* FEATURE_MULTIMEDIA_CODEC_GAIN */

  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_adec_state_qsynth_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION audadsp_adec_state_qsynth

DESCRIPTION
  Holding pattern for QSYNTH functions.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_adec_state_qsynth (void)
{
  uint16 sleep_cmd[2] = {AUDADSP_ADEC_SLEEP_CMD_SIZE, 
                         AUDADSP_ADEC_SLEEP_CMD_HDR};

  voc_state_control_type     *state;

  qdsp_cmd_status_type     result;

  state = voc_state_get_state_data();
  
  if ((audadsp_adec_acquired == FALSE) ||
      (state->hybrid_mode != state->hybrid_mode_pending) ||
      (state->qsynth_vol_mode != state->qsynth_vol_mode_pending)) {

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
#ifdef FEATURE_GASYNTH_INTERFACE
    (void) qdsp_cmd_set_adec_timer_cb(NULL);
#endif /* FEATURE_GASYNTH_INTERFACE */
    /* Disable audio decoder functions */
    (void) qdsp_cmd_adec_config(NULL);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
    /* Disable audio decoder functions */

    qdsp_cmd_qsynth_cmd_dereg();

    result = qdsp_adec_command(sleep_cmd);

    if (result == QDSP_CMD_SUCCESS) {
      audadsp_adec_ctl.state  = AUDADSP_ADEC_STATE_INIT_ENTRY;
      state->exit_mixer_state = TRUE;
      audadsp_adec_ctl.retry_counter = 0;

      qdsp_write(QDSP_adecDataHostStickyInt, 0);

      return (AUDADSP_MIXER_RETURN_CONTINUE);

    } else {

      MSG_HIGH("MIXER qsynth sleep command retrying",0,0,0);
      audadsp_adec_ctl.retry_counter++;

      if (state->retry_counter == 100) {
        MSG_ERROR( "Failed to issue SLEEP command to MIXER synth", 0,0,0 );
        audadsp_adec_ctl.state  = AUDADSP_ADEC_STATE_INIT_ENTRY;
        return (AUDADSP_MIXER_RETURN_ERROR);
      }
    }
  }
                              
  return (AUDADSP_MIXER_RETURN_DONE);
} /* audadsp_adec_state_qtunes */
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */

/*===========================================================================

FUNCTION audadsp_adec_state_control

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
audadsp_mixer_entity_return_type audadsp_adec_state_control (void)
{
  audadsp_mixer_entity_return_type ret_val = AUDADSP_MIXER_RETURN_ERROR;

#ifdef FEATURE_AVS_INCALL_MIXER_ADEC_PB
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
  if (voc_state_get_state_data()->current_image == QDSP_IMAGE_VOC_COMBO_ICA) {
  switch (audadsp_adec_ctl.state) {
      case AUDADSP_ADEC_STATE_INIT_ENTRY:
      MSG_HIGH("AUDADSP_ADEC_STATE_INIT_ENTRY",0,0,0);
        return (audadsp_adec_state_qtunes_init_entry());
        break;

    case AUDADSP_ADEC_STATE_INIT:
      MSG_HIGH("AUDADSP_ADEC_STATE_INIT",0,0,0);
      return (audadsp_adec_state_qtunes_init()); 
      break;

    case AUDADSP_ADEC_STATE_ACTIVE_ENTRY:
      MSG_HIGH("AUDADSP_ADEC_STATE_ACTIVE_ENTRY",0,0,0);
      return (audadsp_adec_state_qtunes_entry()); 
      break;

    case AUDADSP_ADEC_STATE_ACTIVE:
      MSG_HIGH("AUDADSP_ADEC_STATE_ACTIVE",0,0,0);
      return (audadsp_adec_state_qtunes()); 
      break;
  }
  } 
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED*/
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB */

#ifdef FEATURE_AVS_INCALL_MIXER_CMX
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
  if (voc_state_get_state_data()->current_image == QDSP_IMAGE_VOC_COMBO_SYNTH) {
    switch (audadsp_adec_ctl.state) {
      case AUDADSP_ADEC_STATE_INIT_ENTRY:
        ret_val = audadsp_adec_state_qsynth_init_entry();
        break;

      case AUDADSP_ADEC_STATE_INIT:
        ret_val = audadsp_adec_state_qsynth_init(); 
        break;

      case AUDADSP_ADEC_STATE_ACTIVE_ENTRY:
        ret_val = (audadsp_adec_state_qsynth_entry()); 
        break;

      case AUDADSP_ADEC_STATE_ACTIVE:
        ret_val = (audadsp_adec_state_qsynth()); 
        break;
    }
  }
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED*/
#endif /* FEATURE_AVS_INCALL_MIXER_CMX */

  /* Hiting here means something wrong happens */
  return (ret_val);
}

/*===========================================================================

FUNCTION audadsp_adec_get_state

DESCRIPTION
  This function is used to get Audio state. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
audadsp_adec_state_type audadsp_adec_get_state() {
  return (audadsp_adec_ctl.state);
}

/*===========================================================================

FUNCTION audadsp_adec_acquire

DESCRIPTION
  This function acquires or disacquire in-call audio playback module. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void audadsp_adec_acquire
(
  boolean acq_flag
) {
  audadsp_adec_acquired = acq_flag;
  MSG_LOW("audadsp_adec_acquire = %d",acq_flag,0,0);
}

/*===========================================================================

FUNCTION audadsp_adec_state_qtunes_config

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void audadsp_adec_state_qtunes_config (void) 
{
  voc_state_control_type     *state;
#ifdef FEATURE_WMA
  qdsp_cmd_adec_config_type adec_config;

  state = voc_state_get_state_data();
  if((state->current_image == QDSP_IMAGE_WM) 
#ifdef QDSP_IMAGE_VOC_COMBO_ICA_DEFINED
    || (state->current_image == QDSP_IMAGE_VOC_COMBO_ICA) 
#endif /* QDSP_IMAGE_VOC_COMBO_ICA_DEFINED */
    ) {
    adec_config.adec_type        = 0;
    adec_config.subcommand       = 0;
    adec_config.command_length   = 0;
    adec_config.version          = state->wma_config.version;
    adec_config.channels         = state->wma_config.channels;
    adec_config.bytes_per_second = state->wma_config.bytes_per_second;
    adec_config.sample_rate      = state->wma_config.sample_rate;
    adec_config.encoder_options  = state->wma_config.encoder_options;
    adec_config.virtual_pkt_len  = state->wma_config.virtual_pkt_len;

    qdsp_cmd_adec_config_cmd(&adec_config);

    }
#endif
}

#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB || 
          FEATURE_AVS_INCALL_MIXER_CMX */

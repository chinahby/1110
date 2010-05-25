/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP Device Driver")
*/ 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 4   M I X E R   V O C O D E R   D R I V E R
                       S T A T E   C O N T R O L L E R

GENERAL DESCRIPTION
  Controls the state of the QDSP Vocoder DSP.  The QDSP Maintains the
  following internal states:
                 
      RESET ---> IMAGE ---> INIT <----> IDLE <-----> VOCODER

  This module, via the voc_state_control function, drives the QDSP through
  the required state transitions and monitors the results.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  audadsp_vocoder_state_init
    Returns a pointer to the state control structure.
  audadsp_vocoder_state_idle_entry
    Wait for the QDSP2 to reach IDLE state.
  audadsp_vocoder_state_idle
    Stay in IDLE state until the vocoder is configured or released.
  audadsp_vocoder_state_vocoder_entry
    Transition state machine to VOCODER state.
  audadsp_vocoder_state_vocoder
    Monitor the current configuration.
  audadsp_vocoder_state_control
    Mixer uses this function to driver vocoder entity state transition. 

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/audadsp_vocoder.c#3 $ $DateTime: 2009/03/10 09:37:19 $ $Author: subhashj $

when       who     what, where, why
--------   ---    -------------------------------------------------------------
03/10/09   sj      Added slowtalk feature support
05/09/08   skr     changes to prevent codec config being sent during the mixer 
                   exit sequence    
03/17/08  ymc/anb  Modified codec gain ctrl api to control gain changes.
                   Fix for SC2X codec pop issue through MSMAUD_PCM_DC_OFFSET feature.
01/07/08    az     Included the in call midi and the new in call architecture.
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/02/07    az     Included the support for the in call wma.
10/26/07    az     Included the support for the in call AAC.
10/15/07    az     Included support for the crystal speech for in call audio.
12/07/06   ymc     Fixed AUX PCM clock loss due to DSP image swap.
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
#include "clk.h"                /* Clock prototypes                        */
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
#include "qdspcmd.h"            /* QDSP Driver Command Layer definitions  */
#include "qdspdown.h"           /* QDSP download module                   */
#include "qdspext.h"            /* QDSP firmware image definitions         */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
#include "audadsp_mixer.h"         /*                                         */
#include "audadsp_vocoder.h"      /*                                         */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

#define SLEEP_CMD_SIZE               2
#define SLEEP_CMD_HDR                0x0002 /* Command type */

/*-------------------------------------------------------------------------
** Vocoder driver State Control Types
**-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
** Vocoder driver State Control Data
**-----------------------------------------------------------------------*/
audadsp_vocoder_state_ctl_type audadsp_vocoder_ctl;
static boolean audadsp_vocoder_acquired = FALSE;
extern qdsp_cmd_status_type qdsp2_command( uint16* );
/*-------------------------------------------------------------------------
** ADSP Error recovery vars
**-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION audadsp_vocoder_state_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to IDLE state.  If
  the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_init_entry (void)
{
  /* If QDSP failed to reach INIT state, attempt retry */
  if (qdsp_cmd_get_state() != QDSP_STATE_INIT) {
    if (audadsp_vocoder_ctl.retry_counter < 100 ) {
      MSG_HIGH( "MIXER VOCODER INIT state transition retry", 0,0,0 );
      audadsp_vocoder_ctl.retry_counter++;
      return (AUDADSP_MIXER_RETURN_CONTINUE);
    } else {
      /* Transition to ERROR state if retry failed */
      audadsp_vocoder_ctl.retry_counter = 0;
      MSG_ERROR( "QDSP failed to reach MIXER VOCODER INIT state", 0,0,0 );
      return (AUDADSP_MIXER_RETURN_ERROR);
    }
  }

  audadsp_vocoder_ctl.retry_counter = 0;
  audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT;

  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_vocoder_state_init */

/*=========================================================================

FUNCTION audadsp_vocoder_state_init

DESCRIPTION
  If the vocoder is acquired, turn it on and send it to IDLE state.  If
  the vocoder is released, turn off the clocks.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_init (void)
{
  voc_state_control_type   *state;
  state = voc_state_get_state_data();

  if (audadsp_vocoder_acquired == TRUE) {
    if (audadsp_mixer_state_voc_cfg_cmd() == FALSE) {
      if (audadsp_vocoder_ctl.retry_counter < 100 ) {
        MSG_HIGH( "MIXER VOCODER CONFIG command retry", 0,0,0 );
        audadsp_vocoder_ctl.retry_counter++;
        return (AUDADSP_MIXER_RETURN_CONTINUE);
      } else {
        /* Transition to ERROR state if retry failed */
        audadsp_vocoder_ctl.retry_counter = 0;
        audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
        MSG_ERROR( "MIXER VOCODER CONFIG command failed", 0,0,0 );
        return (AUDADSP_MIXER_RETURN_ERROR);
      }
    }
    state->exit_mixer_state = FALSE;
    /* Transition to the MIXER IDLE ENTRY state */
    MSG_HIGH("MIXER Vocoder going to IDLE ENTRY",0,0,0);
    audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_IDLE_ENTRY;
  }
  audadsp_vocoder_ctl.retry_counter = 0;
  return (AUDADSP_MIXER_RETURN_CONTINUE);
} /* audadsp_vocoder_state_init */

/*===========================================================================

FUNCTION audadsp_vocoder_state_idle_entry

DESCRIPTION
  Wait for the QDSP2 to reach IDLE state.  When QDSP2 is in IDLE, inform
  users, and transition the driver to IDLE state. If the QDSP2 does not reach
  IDLE within the allowed time, then perform error recovery.

DEPENDENCIES

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_idle_entry (void)
{
  voc_state_control_type   *state;

  state = voc_state_get_state_data();

  if (qdsp_cmd_get_state() != QDSP_STATE_IDLE) {
    /* If QDSP failed to reach IDLE state, attempt retry */
    if (audadsp_vocoder_ctl.retry_counter < 100) {
      MSG_HIGH("MIXER VOC IDLE state transition retry",0,0,0);
      audadsp_vocoder_ctl.retry_counter++;
      return (AUDADSP_MIXER_RETURN_CONTINUE);
    } else {
      audadsp_vocoder_ctl.retry_counter = 0;
      audadsp_vocoder_ctl.state         = AUDADSP_VOCODER_STATE_INIT_ENTRY;

      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("QDSP failed to reach MIXER VOC IDLE state",0,0,0);
      return (AUDADSP_MIXER_RETURN_ERROR);
    }
  }
   
  audadsp_vocoder_ctl.retry_counter = 0;

  /* Issue sample slipping command */
  if (qdsp_cmd_sample_slip_mode (QDSP_SAMPLE_SLIP_MANUAL_V)
      != QDSP_CMD_SUCCESS) {
   
     MSG_ERROR("Sample Slip command failed",0,0,0);
     audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
     return (AUDADSP_MIXER_RETURN_ERROR);
  }
  
  /* Initialize the audio front end */
  /* Should mixer initialize afe now */
  if (voc_state_init_afe() != VOC_STATE_SUCCESS) {
   
     MSG_ERROR("Failed to initialize afe", 0, 0, 0);
     audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
     return (AUDADSP_MIXER_RETURN_ERROR);
  }

  /* Disable loopback tests */
  if (state->test_mode != VOC_TEST_NONE) {
   
#ifdef FEATURE_UP_LOOPBACK_TEST
    qdsp_cmd_up_packet_loopback(FALSE);
#endif
    qdsp_cmd_loopback_test( QDSP_LOOPBACK_INIT );
    state->test_mode = VOC_TEST_NONE;
  }

  MSG_HIGH("Entered MIXER VOCODER IDLE state",0,0,0);
  
  /* Inform users of transition to IDLE state */
  voc_inform(VOC_INFORM_MIXER_VOC_IDLE);

  /* Going to idle state */
  audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_IDLE;

  /* Check if audio subsystem is active then just return continue instead of immediate as vocoder 
     needs to be bring up independently and the current architecture doesn't support the 
     vocoder and audio systems independently. As of now the code is commented and will be 
     uncommented once the new in call changes are checked in */

 /* if (state->mixer_audio_active_flag) {
       return(AUDADSP_MIXER_RETURN_CONTINUE);
  }*/
  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_vocoder_state_idle_entry */

/*===========================================================================

FUNCTION audadsp_vocoder_state_idle

DESCRIPTION
  Stay in IDLE state until the vocoder is configured or released. If the
  vocoder is configured, transition to the correct state. If the vocoder is
  released transition to the SLEEP state.

DEPENDENCIES
  None

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None
  
===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_idle (void)
{
  qdsp_cmd_status_type     result;
  qdsp_cmd_voc_timing_type *timing_ptr;
  uint16                   slip_mode_qdsp;
  voc_state_control_type   *state;

  word sleep_command[2]={SLEEP_CMD_SIZE, SLEEP_CMD_HDR};
  state = voc_state_get_state_data();

  if (state->codec_pending == TRUE) {
    voc_retry_codec_select();
  }

  if (state->test_mode_pending != state->test_mode) {
   
    state->test_mode = state->test_mode_pending;

    voc_state_write_codec_gain();

    if (state->test_mode == VOC_TEST_AUDIO_LOOP) {
     
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));
      qdsp_cmd_loopback_test( QDSP_LB_AUDIO_AUD_V );

    } else {

      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
      qdsp_cmd_loopback_test( QDSP_LOOPBACK_INIT );
    }
  }

  if (audadsp_vocoder_acquired == TRUE) {

#ifdef FEATURE_VOC_PCM_INTERFACE
    if (state->pcm_int_mode_pending && !state->pcm_int_mode) {
      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL));

    } else if (!state->pcm_int_mode_pending &&
               state->pcm_int_mode &&
               state->test_mode != VOC_TEST_AUDIO_LOOP) {

      qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_MUTE_VOL));
    }

    voc_state_set_pcm_modes();
#endif /* FEATURE_VOC_PCM_INTERFACE */

    if ((state->config_mask != 0) &&
        (state->config_standby == FALSE)) {

      switch (state->config) {
#ifdef FEATURE_QTUNES        
        case VOC_CAP_QTUNES:
#endif
#ifdef FEATURE_QTUNES_AAC
        case VOC_CAP_QTUNES_AAC:
#endif
#ifdef FEATURE_WMA
        case VOC_CAP_QTUNES_WMA: 
#endif           
        case VOC_CAP_QSYNTH:
        case VOC_CAP_IS733:
        case VOC_CAP_IS127:
#if defined(MSMAUD_VOC_AMR)
        case VOC_CAP_AMR:
#endif /* defined(MSMAUD_VOC_AMR) */
#if defined(MSMAUD_VOC_EFR)
#error code not present
#endif /* defined(MSMAUD_VOC_EFR) */
#if defined(MSMAUD_VOC_FR)
#error code not present
#endif /* defined(MSMAUD_VOC_FR) */
#if defined(MSMAUD_VOC_HR)
        case VOC_CAP_GSM_HR:
#endif /* defined(MSMAUD_VOC_HR) */
        {
#ifdef FEATURE_VOICE_PLAYBACK
          /* Decide what to do with current voice service activities */ 
          voc_vr_check_so_change ();
#endif
          if (state->slip_mode_pending == VOC_STATE_SLIP_MAN) {

            slip_mode_qdsp = QDSP_SAMPLE_SLIP_MANUAL_V;
          } else {

            slip_mode_qdsp = QDSP_SAMPLE_SLIP_AUTO_V;
          }

          state->slip_mode = state->slip_mode_pending;

          if (qdsp_cmd_sample_slip_mode(slip_mode_qdsp) != QDSP_CMD_SUCCESS) {

            MSG_ERROR("Sample Slip command failed",0,0,0);

            audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
            return (AUDADSP_MIXER_RETURN_ERROR);
          }
          
          timing_ptr = voc_data_get_timing(state->config);
          if (qdsp_cmd_voc_timing(timing_ptr) != QDSP_CMD_SUCCESS) {

            MSG_ERROR("MIXER Vocoder Timing command failed",0,0,0);

            audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
            return (AUDADSP_MIXER_RETURN_ERROR);
          }

          if (voc_state_voc_config(state->config)!= VOC_STATE_SUCCESS) {
            /* Giving a second chance. Wait for 5 ms */
            (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);

            if (voc_state_voc_config(state->config) != VOC_STATE_SUCCESS ) {

              MSG_ERROR("MIXER Vocoder Configuration command failed",0,0,0);
              audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
              return (AUDADSP_MIXER_RETURN_ERROR);
            }
          }

          /* If the sample slip mode is manual, then issue a manual frame
           * reference command to cause the QDSP to transition to
           * VOCODER state.
           */
          if (state->slip_mode == VOC_STATE_SLIP_MAN) {
            if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {

              /* Giving a second chance. Wait for 5 ms */
              (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 5);

              if (qdsp_cmd_voc_frame_reference() != QDSP_CMD_SUCCESS) {
                MSG_ERROR("MIXER Vocoder Frame Reference command failed",0,0,0);

                audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
                return(AUDADSP_MIXER_RETURN_ERROR);
              }
            }
          }
        }
          break;

        default:
          break;
      }

      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_ACTIVE_ENTRY;
      MSG_HIGH("Going to MIXER VOCODER ACTIVE state",0,0,0);
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {
      /* Vocoder is not configured, just stay in IDLE */
      return (AUDADSP_MIXER_RETURN_DONE);
    }
  } else {
    /* set that slowtalk STATIC parameters are not updated,
       so next time when voice call starts, it will be updated
    */
    voc_data_set_slowtalk_static_params_status(
                    VOC_SLOWTALK_STATIC_PARAMS_NOT_UPDATED);

    /* Go to sleep */
    voc_state_adie_stall();
    result = qdsp2_command(sleep_command);

    if (result == QDSP_CMD_SUCCESS) {
      MSG_HIGH("Going to MIXER VOCODER INIT state",0,0,0);
      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT;
      state->exit_mixer_state = TRUE;
      return(AUDADSP_MIXER_RETURN_CONTINUE);
    } else {
      MSG_ERROR("Sleep command failed",0,0,0);
      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
      return(AUDADSP_MIXER_RETURN_ERROR);
    }

    return(AUDADSP_MIXER_RETURN_CONTINUE);
  }
} /* audadsp_vocoder_state_idle */

/*===========================================================================

FUNCTION audadsp_vocoder_state_vocoder_entry

DESCRIPTION
  When the DSP to arrives in VOCODER state, set the volume, inform users
  of the state change, and transition state machine to VOCODER state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  Volume is set (unmuted).

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_vocoder_entry (void)
{
  voc_state_control_type   *state;

  state = voc_state_get_state_data();

  if (qdsp_cmd_get_state() != QDSP_STATE_VOCODER) {
    /* If QDSP failed to reach VOCODER state. Attempt retry, max 1s */
    if (audadsp_vocoder_ctl.retry_counter < 100) {
      MSG_HIGH("MIXER VOCODER state transition retry", 0,0,0);
      audadsp_vocoder_ctl.retry_counter++;
      return (AUDADSP_MIXER_RETURN_CONTINUE);
    } else {
      /* If a retry has already been attempted, transition to ERROR state */
      MSG_ERROR("QDSP failed to reach VOCODER state", 0,0,0);
      audadsp_vocoder_ctl.retry_counter = 0;
      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
      return (AUDADSP_MIXER_RETURN_ERROR);
    }
  }
    
  audadsp_vocoder_ctl.retry_counter = 0;

  MSG_HIGH("Entered MIXER VOCODER state",0,0,0);
   
  /* Reset config flag */
  state->config_flag  = FALSE;
  audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_ACTIVE; 

#ifdef FEATURE_VOC_DTMF_DETECT
  /* Enable DTMF detection */
  voc_dtmf_detect_enable();
#endif /* FEATURE_VOC_DTMF_DETECT */

  /* Inform users of transition to VOCODER state */
  voc_inform(VOC_INFORM_MIXER_VOCODER);

  /* Issue volume control command */
  if (qdsp_cmd_vol_ctrl(voc_data_get_volume(VOC_DATA_NORM_VOL))
       != QDSP_CMD_SUCCESS) {
   
     MSG_ERROR("Failed to set volume", 0, 0, 0);

     audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
     return (AUDADSP_MIXER_RETURN_ERROR);
  }
#if defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_GSM) */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

  /* now we are in VOCODER state so update the slowtalk configuration */
  voc_update_slowtalk_config();

 /* Check if audio subsystem is active then just return continue instead of immediate as vocoder 
    needs to be bring up independently and the current architecture doesn't support the 
    vocoder and audio systems independently. As of now the code is commented and will be 
    uncommented once the new in call changes are checked in */

/* if (state->mixer_audio_active_flag) {
      return (AUDADSP_MIXER_RETURN_CONTINUE);
   }*/
  return (AUDADSP_MIXER_RETURN_IMMEDIATE);
} /* audadsp_vocoder_state_vocoder_entry */

/*===========================================================================

FUNCTION audadsp_vocoder_state_vocoder

DESCRIPTION
  Monitor the current configuration. If the configuration changes or is
  cleared, command the DSP to transition to IDLE state.

DEPENDENCIES
  None.

RETURN VALUE
  VOC_STATE_SUCCESS  - No action required
  VOC_STATE_CONTINUE - Need to execute state machine again

SIDE EFFECTS
  None.

===========================================================================*/
audadsp_mixer_entity_return_type audadsp_vocoder_state_vocoder (void)
{
  uint16          slip_mode_qdsp;
  voc_state_control_type   *state;

  state = voc_state_get_state_data();
  
  /* Update the sample slip mode
   */
  if (state->slip_mode_pending != state->slip_mode) {
    if (state->slip_mode_pending == VOC_STATE_SLIP_MAN) {
      slip_mode_qdsp = QDSP_SAMPLE_SLIP_MANUAL_V;
    } else {
      slip_mode_qdsp = QDSP_SAMPLE_SLIP_AUTO_V;
    }

    state->slip_mode = state->slip_mode_pending;

    MSG_HIGH("Setting Sample Slip mode = %d",state->slip_mode,0,0);

    if (qdsp_cmd_sample_slip_mode(slip_mode_qdsp) != QDSP_CMD_SUCCESS) {
      MSG_ERROR("Sample Slip command failed",0,0,0);

     audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
     return (AUDADSP_MIXER_RETURN_ERROR);
    }
  }

#ifdef FEATURE_VOC_PCM_INTERFACE
  voc_state_set_pcm_modes();
#endif /* FEATURE_VOC_PCM_INTERFACE */

  /* Determine whether the vocoder should stay in VOCODER state
  ** or return to IDLE.
  */

  if (audadsp_vocoder_acquired == FALSE ||
      state->codec_pending == TRUE  
#ifdef FEATURE_BT_AG
      || (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING)
#endif
     ) {

    if (voc_state_go_to_idle() == VOC_STATE_SUCCESS) {
      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_IDLE_ENTRY;
      /* state->mixer_vocoder_active_flag = FALSE; */
      state->exit_mixer_state = TRUE;
      if (state->codec_pending == TRUE) {
        voc_retry_codec_select();
      }
    } else {
      MSG_ERROR("MIXER VOCODER->IDLE failed",0,0,0);
      audadsp_vocoder_ctl.state = AUDADSP_VOCODER_STATE_INIT_ENTRY;
      /* state->mixer_vocoder_active_flag = FALSE; */
      return (AUDADSP_MIXER_RETURN_ERROR);
    }

    return (AUDADSP_MIXER_RETURN_CONTINUE);
  } else {
#ifdef FEATURE_BT_AG
    voc_check_bt(VOC_BT_OP_AG_NORMAL);
#endif /* FEATURE_BT_AG */
    return (AUDADSP_MIXER_RETURN_DONE);
  }
} /* audadsp_vocoder_state_vocoder */

/*===========================================================================

FUNCTION audadsp_vocoder_state_control

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
audadsp_mixer_entity_return_type audadsp_vocoder_state_control (void)
{
  switch (audadsp_vocoder_ctl.state) {
    case AUDADSP_VOCODER_STATE_INIT_ENTRY:
      return (audadsp_vocoder_state_init_entry());
      break;

    case AUDADSP_VOCODER_STATE_INIT:
      return (audadsp_vocoder_state_init());
      break;

    case AUDADSP_VOCODER_STATE_IDLE_ENTRY:
      return (audadsp_vocoder_state_idle_entry()); 
      break;

    case AUDADSP_VOCODER_STATE_IDLE:
      return (audadsp_vocoder_state_idle()); 
      break;

    case AUDADSP_VOCODER_STATE_ACTIVE_ENTRY:
      return (audadsp_vocoder_state_vocoder_entry()); 
      break;

    case AUDADSP_VOCODER_STATE_ACTIVE:
      return (audadsp_vocoder_state_vocoder()); 
      break;
  }

  /* Hiting here means something wrong happens */
  return (AUDADSP_MIXER_RETURN_NONE);
}

/*===========================================================================

FUNCTION audadsp_vocoder_acquire

DESCRIPTION
  This function acquires or disacquire in-call audio playback module. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void audadsp_voc_acquire
(
  boolean acq_flag
) {
  audadsp_vocoder_acquired = acq_flag;
}
#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB || 
          FEATURE_AVS_INCALL_MIXER_CMX */


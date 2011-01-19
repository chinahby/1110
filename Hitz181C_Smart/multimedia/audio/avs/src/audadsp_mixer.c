/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QDSP Device Driver")
*/ 
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P 4   A U D A D S P   M I X E R   D R I V E R
                       S T A T E   C O N T R O L L E R

GENERAL DESCRIPTION
  Controls the state of the QDSP Vocoder DSP.  The QDSP Maintains the
  following internal states:
                 
      RESET ---> IMAGE ---> INIT <----> IDLE <-----> ACTIVE

  This module, via the voc_state_control function, drives the QDSP through
  the required state transitions and monitors the results.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  audadsp_mixer_state_get_mixer_mode
    Gets the vocoder+audio mixing state currently configured. 
  audadsp_mixer_state_init
    First virtual state after DSP IMAGE is loaded
  audadsp_mixer_state_idle_entry
    Wait for the QDSP2 to reach IDLE state.
  audadsp_mixer_state_idle
    The majority done in this function is for vocoder init state.
  audadsp_mixer_state_active_entry
    Transition state machine to VOCODER state and ADEC state.
  audadsp_mixer_state_active
    Stays there until something changed 
  audadsp_mixer_state_check_mixer_codec_cfg_cmd
    Sends the mixer codec config command.
  audadsp_mixer_state_active_go_idle
    The entry point to driver the mixer to goes into idle state

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/audadsp_mixer.c#12 $ $DateTime: 2009/10/15 05:43:22 $ $Author: kmodak $

when       who     what, where, why
--------   ---    -------------------------------------------------------------
10/08/09   knm     Integrated new marimba codec driver
10/06/09   sj      Added SCMM specific changes
07/31/09   rm      snd_expire gets called soon whenever present image does not
                   support its module in dsp. relaxing sound expire by 100ms.
                   [CR 189921 - Sound mute issue (VOC_CMD_BUSY_S)]
07/30/09   kkm     AAC playback is not resumed after voice call end if mixing
                   is enabled. Eclocks are turned off before mixer idle command,
                   modifed the code to enable eclock until idle command is issued
06/30/09   kk      Fixed CR 181984 - Incall(MO & MT):After playing MP3 clip,
                   First AAC clip plays but with no audio in Continuous mode,  
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
01/29/09    az     Fixed the CR: 170332 vocoder handoff is not working for 
                   InCall Audio scenario.
09/23/08   skr     check for module_status before qdsp_enable in 
                   audadsp_mixer_state_idle
08/26/08   skr     Qensemble default config sent for first time.
07/16/08   skr     changes to send 0x501 cmd to bring Audio to foreground 
                   (90 deg) without changing to the default gain.
07/11/08   skr     changes to release VOCODER & ADEC state machine separately.
05/12/08   skr     implemented review comments.
05/09/08   skr     changes to prevent codec config being sent during the mixer 
                   exit sequence    
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
02/29/08   anb     Fixed a compiler warning.
02/28/08    az     Corrected the messages for in call audio.
02/08/08    vs     Fixed compile error if QEnsemble feature not defined.
02/07/08    kk     Added support for Qensemble phase 2 interface changes.
01/28/08    az     Fixed the CR:136655 Incall MIDI: Incall Midi is not working 
                   if we interrupt play back in between.
01/07/08    az     Included the in call midi and the new in call architecture.
11/15/07    az     Featurized the code for In call to avoid compilation for
                   other targets.
11/14/07    hs     Added QEnsemble support
11/08/07    az     Fixed the merging issues.
11/02/07    az     Included the support for the in call wma.
10/26/07    az     Included the In call AAC support.
08/30/07   wfd     Place F3 messages for BT_AG inside #ifdef FEATURE_BT_AG.
04/14/07   sud     Added support for independent control of Tx and Rx codec
                   clock for Saturn codec.
12/11/06    sp     Fixed a compiler error.
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
#endif         /* Clock prototypes                        */

#ifdef FEATURE_CMI_MM
#error code not present
#endif                        
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
#include "clkregim.h"           /* For clk_regime_enable/disable.          */

#ifdef FEATURE_EXTERNAL_SDAC
#include "sdac.h"               /* External Stereo DAC prototypes          */
#include "bsp.h"                /* Board support package                   */
#endif /* FEATURE_EXTERNAL_SDAC */

#ifdef MSMAUD_ADIE_CODEC
#ifndef MSMAUD_SCMM
#include "adie.h"               /* Analog die definitions                  */
#endif /* MSMAUD_SCMM */
#endif /* MSMAUD_ADIE_CODEC */

#ifdef FEATURE_TTY_UMTS
#error code not present
#endif /* FEATURE_TTY_UMTS */

#if defined(FEATURE_AVS_INCALL_MIXER_ADEC_PB) || \
    defined(FEATURE_AVS_INCALL_MIXER_CMX)
#include "audadsp_mixer.h"      /*                                         */
#include "audadsp_adec.h"       /*                                         */
#include "audadsp_vocoder.h"    /*                                         */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/
#define AUDADSP_MIXER_IDLE_CMD_SIZE       2
#define AUDADSP_MIXER_STATE_IDLE_CMD_HDR  0x901

/*-------------------------------------------------------------------------
** MIXER driver State Control Types
**-----------------------------------------------------------------------*/
audadsp_mixer_state_control_type audadsp_mixer_state_ctl;

/*-------------------------------------------------------------------------
** Vocoder driver State Control Data
**-----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
** ADSP Error recovery vars
**-----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
   This enum type is defined to indicate whether to acquire/disacquire audio module & voice module.
 *------------------------------------------------------------------------*/
typedef enum {
  AUDADSP_MIXER_RELSE_NONE = 0x0,
  AUDADSP_MIXER_RELSE_SND_ONLY = 0x1,
  AUDADSP_MIXER_RELSE_VOC_ONLY = 0x2,
  AUDADSP_MIXER_RELSE_ALL = (AUDADSP_MIXER_RELSE_SND_ONLY |
                             AUDADSP_MIXER_RELSE_VOC_ONLY)
} audadsp_mixer_release_mode_enum_type;
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
/*-----------------------------------------------------------------------*/


/* <EJECT> */
/*===========================================================================

                 F U N C T I O N   D E F I N I T I O N S

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/

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
qdsp_mixer_mode_type audadsp_mixer_state_get_mixer_mode (
  audadsp_mixer_state_entity_state_type *entity_state
)
{
  voc_state_control_type  *state;
  qdsp_mixer_mode_type    mode;

  state = voc_state_get_state_data();

  mode = QDSP_CMD_MIXER_MODE_NONE;

#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
  if (state->current_image == QDSP_IMAGE_VOC_COMBO_SYNTH) {
    state->audfmt_mix_mode = VOC_ADEC_MIX_MODE_RX;
    }
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */

  if ((state->audfmt_mix_mode == VOC_ADEC_MIX_MODE_RX) &&
      (state->num_higher_priority_apps == 0)) {

    if (((state->acquire_mask & ((word) 1 << VOC_ACQ_CDMA)) &&
         (state->config_mask  & ((word) 1 << VOC_ACQ_CDMA)))
#ifdef FEATURE_MVS
        || ((state->acquire_mask & ((word) 1 << VOC_ACQ_MVS)) &&
            (state->config_mask  & ((word) 1 << VOC_ACQ_MVS))))
#endif
    {
      /* During voice call, check adec play back */

      switch (voc_get_desired_config(VOC_ACQ_SND)) {
#ifdef FEATURE_QTUNES
        case VOC_CAP_QTUNES:
#endif
#ifdef FEATURE_QTUNES_AAC
        case VOC_CAP_QTUNES_AAC:
#endif
#ifdef FEATURE_WMA
        case VOC_CAP_QTUNES_WMA: 
#endif
#ifdef FEATURE_QSYNTH_COMMON
        case VOC_CAP_QSYNTH:
#endif /* FEATURE_QSYNTH_COMMON */
        /* There is a voice call (either CDMA or UMTS/GSM) existing
         * and QTUNES is to be configured */
          mode = QDSP_CMD_MIXER_MODE_VOC_ADEC;
          break;

        /* TODO: Need to consider future requirement to be able to
         * play QCP+MP3/AAC simutaneously */
        default:
          mode = QDSP_CMD_MIXER_MODE_VOC_ONLY;
          MSG_HIGH(" Value of mode in SND %d",mode,0,0);
          break;
      }

    } else if ((state->acquire_mask & ((word) 1 << VOC_ACQ_SND)) &&
               (state->config_mask  & ((word) 1 << VOC_ACQ_SND))) {
        /* During ADEC playback, check voice call */

        switch (voc_get_desired_config(VOC_ACQ_CDMA)) { 
          case VOC_CAP_IS733:
          case VOC_CAP_IS127:
            mode = QDSP_CMD_MIXER_MODE_VOC_ADEC;
            break;

          default:
            mode = QDSP_CMD_MIXER_MODE_ADEC_ONLY;
            break;
        }

#ifdef FEATURE_MVS
        /* During ADEC playback, check voice call */
        switch (voc_get_desired_config(VOC_ACQ_MVS)) { 
          case VOC_CAP_AMR:
          case VOC_CAP_GSM_FR:
          case VOC_CAP_GSM_EFR:
          case VOC_CAP_GSM_HR:
            mode = QDSP_CMD_MIXER_MODE_VOC_ADEC;
            break;

          default:
            mode = QDSP_CMD_MIXER_MODE_ADEC_ONLY;
            MSG_HIGH(" Value of mode in MVS %d",mode,0,0);
            break;
        }
#endif /* FEATURE_MVS */
    }
  }

  /* Setup mixer internal state control block based on the required mode */
  switch (mode) {
    case QDSP_CMD_MIXER_MODE_VOC_ONLY:
      *entity_state = AUDADSP_MIXER_STATE_VOC;
      break;

    case QDSP_CMD_MIXER_MODE_ADEC_ONLY:
      *entity_state = AUDADSP_MIXER_STATE_ADEC;
      break;

    case QDSP_CMD_MIXER_MODE_VOC_ADEC:
      *entity_state = AUDADSP_MIXER_STATE_VOC_ADEC;
      break;

    default: 
      *entity_state = AUDADSP_MIXER_STATE_NONE;
      MSG_HIGH("No audio and voice modules acquired %d",mode,0,0);
      break;
  }

  return (mode);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

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
voc_state_return_type audadsp_mixer_state_init (void)
{
  voc_state_control_type *state;

  audadsp_mixer_state_entity_state_type entity_state;

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

  state = voc_state_get_state_data();
  state->retry_counter = 0;

  (void)audadsp_mixer_state_get_mixer_mode(&entity_state);

  if (entity_state != AUDADSP_MIXER_STATE_NONE) {

    /* Transition to the AUDADSP_MIXER IDLE state */
    state->state  = VOC_STATE_MIXER_IDLE_ENTRY;
    MSG_HIGH("MIXER going to AUDADSP_MIXER_IDLE_ENTRY state",0,0,0);

  } else { /* Vocoder is not acquired */
    
    /* Shut down clocks and codecs */
    voc_state_shutdown();
    
    /* No more activity, going to reset, mixer image needs to be 
     * swapped out */
    MSG_HIGH("MIXER going to RESET",0,0,0);
    state->state  = VOC_STATE_RESET;
  
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }

  return (VOC_STATE_CONTINUE_IMMEDIATE);
} /* audadsp_mixer_state_init */

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
voc_state_return_type audadsp_mixer_state_idle_entry (void)
{
  voc_state_control_type    *state;
  sint15                    dsp_mixer_state;

  state = voc_state_get_state_data();

  (void)qdsp_cmd_get_mixer_state(&dsp_mixer_state);

  if (dsp_mixer_state != QDSP_STATE_MIXER_IDLE) {

    if (state->retry_counter < 100 ) {
      MSG_HIGH("MIXER IDLE state transition retry", 0,0,0);
      state->retry_counter++;
      return (VOC_STATE_CONTINUE);
    } else {
      /* Transition to ERROR state if retry failed */
      MSG_ERROR( "QDSP failed to reach mixer IDLE state", 0,0,0 );
      state->state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  /* Whenever MIXER is in IDLE_ENTRY state, we should make sure that the 
   * entities of VOCODER and ADEC are both at INIT state. 
   *
   * This is what this section does.
   */

  state->retry_counter = 0;

  state->state = VOC_STATE_MIXER_IDLE;
  
  MSG_HIGH("Entered MIXER_IDLE state",0,0,0);
  
  /* Inform users of transition to MIXER IDLE state */
  voc_inform(VOC_INFORM_MIXER_IDLE);

  /* Going to MIXER IDLE state */
  return (VOC_STATE_CONTINUE_IMMEDIATE);
} /* audadsp_mixer_state_idle_entry */

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
voc_state_return_type audadsp_mixer_state_idle (void)
{
  audadsp_mixer_state_entity_state_type entity_state;

  qdsp_mixer_mode_type          mixer_mode;
                                
#ifdef FEATURE_BT_AG            
  boolean                       bt_status = FALSE;
#endif
  qdsp_module_type              module_select;

  voc_state_control_type        *state;
  voc_state_type	        module_select_status;

  state = voc_state_get_state_data();

  /* Update config with config_pending */
  state->config = state->config_pending;

  module_select_status =  voc_state_get_high_module(&module_select);
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 state->current_image,state->module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  mixer_mode = audadsp_mixer_state_get_mixer_mode(&entity_state);

   state->module_reqd = module_select;
  MSG_MED("Mixer idle- current_qdsps_app:%d, module_reqd:%d, module_status:%d ",
                   state->current_qdsps_app,state->module_reqd,state->module_status);

  if (!(voc_state_check_ica_mixer_module(state->module_reqd))) {
      /* We should not call qdsp_enable if the module_status is 
        VOC_STATE_MOD_STATUS_DISABLE_PENDING */
      if (state->module_status != VOC_STATE_MOD_STATUS_DISABLE_PENDING){

          MSG_HIGH("qdsp_enable(%d,%d)",
                          state->current_qdsps_app, module_select,0);

        state->module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;

        qdsp_enable (state->current_qdsps_app, module_select);
        (void) rex_wait(VOC_CHECK_STATE_SIG);
      }

    MSG_HIGH("Mixer going to RESET",0,0,0);
    state->state  = VOC_STATE_RESET;
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  }else{

      /* We should not call qdsp_enable if the module_status is 
        VOC_STATE_MOD_STATUS_DISABLE_PENDING */
      if (state->module_status != VOC_STATE_MOD_STATUS_DISABLE_PENDING){

          MSG_HIGH("qdsp_enable(%d,%d)",
                          state->current_qdsps_app, module_select,0);

        state->module_status = VOC_STATE_MOD_STATUS_ENABLE_REQUESTED;

        qdsp_enable (state->current_qdsps_app, module_select);
        (void) rex_wait(VOC_CHECK_STATE_SIG);
      }else {
        MSG_HIGH("Mixer idle- waiting for VOC_STATE_MOD_STATUS_DISABLE",0,0,0);
        return (VOC_STATE_CONTINUE);
      }
  }

  if (voc_state_get_clocks_on() == FALSE) {
    /* Enable all vocoder and codec clocks */
    voc_state_enable_clocks();
    voc_state_set_clocks_on(TRUE);
  }

  /* Allow QDSP time to run */  
   
#if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
  clk_busy_wait(200);    		
#else
#error code not present
#endif

#ifdef MSMAUD_ADIE_CODEC
  /* Wake up the Analog Die if the codec in use is the internal codec. */
  (void) voc_state_adie_wakeup ();
#endif /* MSMAUD_ADIE_CODEC */

  if (entity_state != audadsp_mixer_state_ctl.entity_state) { 
    audadsp_mixer_state_ctl.entity_state = entity_state;
  }

#ifdef FEATURE_BT_AG
  if (voc_get_bt_state() == VOC_BT_STATE_AG_CODEC_CONFIG_PENDING) {

    bt_status = voc_complete_bt_ag();

    if (!bt_status) {
     
      /* BT state could not be transitioned to AG ENABLED due to 
      DSP not accepting a codec config */
      if (state->retry_counter < 100 ) {
        MSG_HIGH("MIXER IDLE: BT state transition retry",0,0,0 );
        state->retry_counter++;
        return (VOC_STATE_CONTINUE);
      } else {

        /* If a retry has already been attempted, go to ERROR state */
        MSG_ERROR("MIXER: BT state failed to reach AG ENABLED state",0,0,0 );
        state->state = VOC_STATE_ERROR_RECOVERY;
        return (VOC_STATE_CONTINUE);
      }
    } else {
     
      state->retry_counter = 0;
    }
  } else if (!voc_check_bt(VOC_BT_OP_AG_NORMAL)) {
    return(VOC_STATE_SUCCESS);
  } else 
#endif /* FEATURE_BT_AG */

    state->tunes_config_pending = FALSE;
/* Need to configure the mp3,aac and wma here as it is the requirement from the
   dsp to be done in idle state */
#ifdef FEATURE_QTUNES
    if(state->config == VOC_CAP_QTUNES) {
      MSG_MED("MIXER IDLE: MP3",0,0,0);
      /* send DSP configuration information */
      qdsp_cmd_set_mp3_config((qdsp_cmd_mp3_config_type*)&(state->mp3_config));
    }
#endif /* FEATURE_QTUNES */ 
#ifdef FEATURE_QTUNES_AAC
    else if(state->config == VOC_CAP_QTUNES_AAC) {
      MSG_MED("MIXER IDLE: AAC",0,0,0);
      qdsp_cmd_set_aac_config((qdsp_cmd_aac_config_type*)&(state->aac_config));
    }
#endif
#ifdef FEATURE_WMA
    else if(state->config == VOC_CAP_QTUNES_WMA) {
      MSG_MED("MIXER IDLE: WMA",0,0,0);
      audadsp_adec_state_qtunes_config ();
      #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)
      clk_busy_wait(3000);   		
      #else
#error code not present
      #endif
    }
#endif
    else {
      MSG_MED("MIXER IDLE: Incompatible Audio",0,0,0);
    }

  if (audadsp_mixer_state_mixer_codec_cfg_cmd() == FALSE) {
    MSG_ERROR("Mixer Codec Config command failed",0,0,0);
    state->state = VOC_STATE_ERROR_RECOVERY;
    return(VOC_STATE_CONTINUE);
  }

  /* After mixer codec configuration command is issued, 
   * 1. MIXER state is supposed to go to ACTIVE;
   * 2. MIXER Vocoder state is supposed to go to IDLE;
   * 3. MIXER ADEC state is supposed to go to ACTIVE. */

  /* Transition to the MIXER ACTIVE state */
  MSG_HIGH("MIXER going to ACTIVE ENTRY",0,0,0);

  state->state = VOC_STATE_MIXER_ACTIVE_ENTRY;

  /* Since the mixer codec config command has been sent,
   * mixer needs to driver vocoder and adec entities further.
   */
  return (VOC_STATE_CONTINUE);
} /* audadsp_mixer_state_idle */

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
voc_state_return_type audadsp_mixer_state_active_entry (void)
{
  voc_state_control_type  *state;
  sint15                  mixer_dsp_state;
#ifdef FEATURE_BT_AG
  voc_bt_state_enum_type  bt_state;
#endif /* FEATURE_BT_AG */

  state = voc_state_get_state_data();

  (void)qdsp_cmd_get_mixer_state(&mixer_dsp_state);

  if (mixer_dsp_state != QDSP_STATE_MIXER_ACTIVE) {

    if (state->retry_counter < 100) {
      MSG_HIGH("MIXER ACTIVE state transition retry", 0,0,0);
      state->retry_counter++;
      return (VOC_STATE_CONTINUE);
    } else {
      /* Transition to ERROR state if retry failed */
      MSG_ERROR( "QDSP failed to reach MIXER ACTIVE state", 0,0,0 );
      state->state = VOC_STATE_ERROR_RECOVERY;
      return(VOC_STATE_CONTINUE);
    }
  }

  state->retry_counter = 0;

  /* Now we need to set up the PCM clock */
#ifdef FEATURE_BT_AG
  bt_state = voc_get_bt_state(); 

  MSG_HIGH("bt_state = %d",bt_state,0,0);
#endif /* FEATURE_BT_AG */

  /* setup PCM clock based on the mixing mode */
  switch (audadsp_mixer_state_ctl.entity_state) {
    /* Need to revisit this switch case */

    case AUDADSP_MIXER_STATE_VOC:
      /* Vocoder only mode, PCM clock stays at 8KHz. */
      #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
      #else
      voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
      #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
      break;

    case AUDADSP_MIXER_STATE_ADEC:
      /* Don't do anything here, since voc_state_do_adie_wakeup has already
       * the corresponding PCM clock according to the clip's sampling 
       * frequency */
      break;

    case AUDADSP_MIXER_STATE_VOC_ADEC:
      /* VOC+ADEC mixing mode, if BT link is connected, PCM clock needs to stay
       * at 8KHz, otherwise it has to be bumped up to 48KHz. */
#ifdef FEATURE_BT_AG
      if ((bt_state >= VOC_BT_STATE_AG_SCO_LINK_PENDING) && 
          (bt_state <= VOC_BT_STATE_AG_ENABLED)) {

        // Enable the ecodec clock to increase the count and disable in the idle state
        // Firmware requires eclock to be on for decoder to come to init state
        voc_state_enable_eclock();
        MSG_HIGH("BlueTooth link existing",0,0,0);
        /* voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000); */

      } else if (bt_state == VOC_BT_STATE_DISABLED) {

        MSG_HIGH("BlueTooth link is not existing",0,0,0);
        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
      } else 
#endif /* FEATURE_BT_AG */
      {
        #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
        #else
        voc_state_set_icodec_clk(VOC_SAMPLE_RATE_48000);
        #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */
      }
      break;

    default:
      break;
  }

  /* Turn analog part of Adie */
  #ifdef MSMAUD_SCMM
#error code not present
  #endif /* MSMAUD_SCMM */

  MSG_HIGH("Entered MIXER ACTIVE state",0,0,0);
   
  /* Reset config flag */
  state->config_flag = FALSE;
  state->state       = VOC_STATE_MIXER_ACTIVE;
   
  return (VOC_STATE_CONTINUE_IMMEDIATE);
} /* audadsp_mixer_state_active_entry */

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
voc_state_return_type audadsp_mixer_state_active (void)
{
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif
  qdsp_module_type              module_select;
  voc_state_type		module_select_status;
  voc_state_control_type        *state;
  qdsp_mixer_mode_type          mixer_mode;
  audadsp_mixer_state_entity_state_type entity_state;
  audadsp_mixer_entity_return_type audvoc_ret = AUDADSP_MIXER_RETURN_NONE;
  audadsp_mixer_release_mode_enum_type relse_mode = AUDADSP_MIXER_RELSE_NONE;
  voc_capability_type            voc_cap = VOC_CAP_NONE;
  boolean                        mixer_idle = TRUE;

  module_select_status =  voc_state_get_high_module(&module_select);
  state = voc_state_get_state_data();
  switch(module_select_status)
  {
    case VOC_CMD_FAILED_S:
      break;
    case VOC_CMD_BUSY_S:
      {
        MSG_HIGH("Module not supported on current image = %d, "
                 "Continuing with Module = %d",
                 state->current_image,state->module_reqd,0);
        voc_inform(VOC_INFORM_MODULE_UNFETCHED);
      }
      break;
    default:
      ;
  }
  

  if ((state->module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING) ||
      (state->config_mask == 0)) {
    relse_mode = AUDADSP_MIXER_RELSE_ALL;
  } else if (state->module_reqd == module_select) {
    relse_mode = AUDADSP_MIXER_RELSE_NONE;
  } else {
    if (voc_state_check_ica_mixer_module(module_select) == TRUE) {
      if (state->snd_config != voc_get_desired_config(VOC_ACQ_SND)) {
#ifdef FEATURE_QSYNTH_COMMON
        if((state->snd_config == VOC_CAP_QSYNTH) ||
           (voc_get_desired_config(VOC_ACQ_SND) == VOC_CAP_QSYNTH)) {
          relse_mode = AUDADSP_MIXER_RELSE_ALL;
        } else
#endif /* FEATURE_QSYNTH_COMMON */
        {
          relse_mode |= AUDADSP_MIXER_RELSE_SND_ONLY;
          mixer_idle = FALSE;
  }
    }
      if ((state->acquire_mask & ((word) 1 << VOC_ACQ_CDMA)) &&
          (state->config_mask  & ((word) 1 << VOC_ACQ_CDMA))) {
        voc_cap = voc_get_desired_config(VOC_ACQ_CDMA);
      }
#ifdef FEATURE_MVS
      else if ((state->acquire_mask & ((word) 1 << VOC_ACQ_MVS)) &&
               (state->config_mask  & ((word) 1 << VOC_ACQ_MVS))) {
        voc_cap = voc_get_desired_config(VOC_ACQ_MVS);
      }
#endif /* FEATURE_MVS */
      if (state->voc_config != voc_cap) {
        relse_mode |= AUDADSP_MIXER_RELSE_VOC_ONLY;
        mixer_idle = FALSE;
  }

    } else {
       if (state->config_pending == state->snd_config){
         /* Voice call has ended so we will release the Vocoder First */
         MSG_HIGH("Voice call has ended",0,0,0);
         relse_mode |=AUDADSP_MIXER_RELSE_VOC_ONLY; 
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
         /* After VOCODER has reached the INIT state we release the ADEC */
         if (qdsp_cmd_get_state() == QDSP_STATE_INIT){
           MSG_HIGH("Music ended after Voice call has ended",0,0,0);
         relse_mode |= AUDADSP_MIXER_RELSE_SND_ONLY;
         }
       }
       else if(state->config_pending == state->voc_config){
         MSG_HIGH("Music has ended ",0,0,0);
         relse_mode |=AUDADSP_MIXER_RELSE_SND_ONLY;
         /* After ADEC has reached the INIT state we release the VOCODER */
         if (qdsp_cmd_get_adec_state() == QDSP_STATE_INIT){
          MSG_HIGH("Voice call ended after Music has ended",0,0,0);
          relse_mode |= AUDADSP_MIXER_RELSE_VOC_ONLY;
          }
       }
       else{
         relse_mode = AUDADSP_MIXER_RELSE_ALL;
       }
    }
  }

  switch(relse_mode) {
    case AUDADSP_MIXER_RELSE_NONE:
      MSG_HIGH("AUDADSP_MIXER_RELSE_NONE",0,0,0);
      if (state->module_status == VOC_STATE_MOD_STATUS_ENABLED) {
      mixer_mode = audadsp_mixer_state_get_mixer_mode(&entity_state);
      if ((mixer_mode & QDSP_CMD_MIXER_MODE_VOC_ONLY) != 0) {
        MSG_MED("QDSP_CMD_MIXER_MODE_VOC_ONLY",0,0,0);
        audadsp_voc_acquire(TRUE);
        audvoc_ret |= audadsp_vocoder_state_control();
        }

      if ((mixer_mode & QDSP_CMD_MIXER_MODE_ADEC_ONLY) != 0) {
        MSG_MED("QDSP_CMD_MIXER_MODE_ADEC_ONLY",0,0,0);
        audadsp_adec_acquire(TRUE);
        audvoc_ret |= audadsp_adec_state_control();
        }
      }
      break;

    case AUDADSP_MIXER_RELSE_SND_ONLY:
      MSG_HIGH("AUDADSP_MIXER_RELSE_SND_ONLY",0,0,0);
      audadsp_adec_acquire(FALSE);
      audvoc_ret = audadsp_adec_state_control();

      if(!(audvoc_ret & AUDADSP_MIXER_RETURN_ERROR) &&
         (qdsp_cmd_get_adec_state() == QDSP_STATE_INIT)) {
        state->module_reqd = module_select;
        qdsp_enable (state->current_qdsps_app, module_select);
        (void) rex_wait(VOC_CHECK_STATE_SIG);
        MSG_MED(" Release only SND %d",0,0,0);
        return (VOC_STATE_CONTINUE);
      }
      break;

    case AUDADSP_MIXER_RELSE_VOC_ONLY:
      MSG_HIGH("AUDADSP_MIXER_RELSE_VOC_ONLY",0,0,0);
      audadsp_voc_acquire(FALSE);
      audvoc_ret = audadsp_vocoder_state_control();

      if(!(audvoc_ret & AUDADSP_MIXER_RETURN_ERROR) &&
         (qdsp_cmd_get_state() == QDSP_STATE_INIT)) {
        state->module_reqd = module_select;
        qdsp_enable (state->current_qdsps_app, module_select);
        (void) rex_wait(VOC_CHECK_STATE_SIG);
        MSG_MED(" Release only VOC %d",0,0,0);
        return (VOC_STATE_CONTINUE);
      }
      break;

    case AUDADSP_MIXER_RELSE_ALL:
      MSG_HIGH("AUDADSP_MIXER_RELSE_ALL",0,0,0);  
      /* In Qensemble scenario we wait for interrupt fron the DSP until the 
        Audio has moved to foreground or File plaback has ended before 
        releasing the ADEC */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
      audadsp_voc_acquire(FALSE);
      audvoc_ret |= audadsp_vocoder_state_control();

      if ((!(
#ifdef QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED
          (state->current_image == QDSP_IMAGE_VOC_COMBO_SYNTH) &&
#endif /* QDSP_IMAGE_VOC_COMBO_SYNTH_DEFINED */
#ifdef FEATURE_QSYNTH_COMMON
          (voc_get_desired_config(VOC_ACQ_SND) == VOC_CAP_QSYNTH) &&
          (state->config_pending == VOC_CAP_QSYNTH)
#endif /* FEATURE_QSYNTH_COMMON */
          )) ||
          (state->module_status == VOC_STATE_MOD_STATUS_DISABLE_PENDING)) {
          MSG_MED("VOC_STATE_MOD_STATUS_DISABLE_PENDING",0,0,0);
          audadsp_adec_acquire(FALSE);
          audvoc_ret |= audadsp_adec_state_control();

          if (!(audvoc_ret & AUDADSP_MIXER_RETURN_ERROR) &&
              (qdsp_cmd_get_adec_state() == QDSP_STATE_INIT) &&
              (qdsp_cmd_get_state() == QDSP_STATE_INIT)) {
            if (mixer_idle == TRUE) {
              return(audadsp_mixer_state_active_go_idle());
            }
          }
        }
      break;
  }

  if ((audvoc_ret & AUDADSP_MIXER_RETURN_ERROR)) {
    /* Need to clean up voc state machine and adec state machine */ 
    state->state = VOC_STATE_ERROR_RECOVERY;
    return (VOC_STATE_CONTINUE);
  } else if ((audvoc_ret & AUDADSP_MIXER_RETURN_IMMEDIATE)) {
    return (VOC_STATE_CONTINUE_IMMEDIATE);
  } else if (audvoc_ret & AUDADSP_MIXER_RETURN_CONTINUE){
    return (VOC_STATE_CONTINUE);
  }
  
  /* Inform users of transition to MIXER ACTIVE state */
  voc_inform(VOC_INFORM_MIXER_ACTIVE);

#ifdef FEATURE_BT_AG
  voc_check_bt(VOC_BT_OP_AG_NORMAL);
#endif /* FEATURE_BT_AG */

/* QEnsemble config command issued */
#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

  return (VOC_STATE_SUCCESS);
} /* audadsp_mixer_state_active */

/*===========================================================================

FUNCTION audadsp_mixer_state_get_codec_cfg

DESCRIPTION
  To get codec config.

DEPENDENCIES
  None.  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audadsp_mixer_state_get_codec_cfg
(
  qdsp_cmd_codec_config_type **codec_config_ptr
) { 
  voc_codec_type             pcm_path;
  voc_codec_class_type       codec_type;
  voc_state_control_type     *state;

  state = voc_state_get_state_data();

  /* Get the codec configuration data */
  voc_data_get_codec(&pcm_path);
  voc_data_get_codec_class(&codec_type);

  /* Reset the configuration for this type of codec */
  voc_state_set_codec_cal(pcm_path);

  /* This is for the future purpose for the SDAC support */
#ifdef FEATURE_EXTERNAL_SDAC
  if (codec_type == VOC_CODEC_CLASS_EXTERNAL_STEREO) {

    voc_set_clk_ctrl_timer(FALSE, VOC_CLK_CTRL_STEREO_DAC);
    voc_state_config_external_sdac(voc_data_get_sample_rate());
  } else {

    voc_state_config_external_sdac(VOC_SAMPLE_RATE_NONE);
  }
#endif

  /* Enable registered codec function */
  voc_enable_codec_func();

#ifdef FEATURE_BT_AG
  if (((codec_type == VOC_CODEC_CLASS_BLUETOOTH)
#ifdef FEATURE_BT_AG_LOCAL_AUDIO
#error code not present
#endif /* FEATURE_BT_AG_LOCAL_AUDIO */
      ) && (voc_get_bt_state() == VOC_BT_STATE_DISABLED)) {

    voc_data_get_codec_config(state->current_image,
                              VOC_CODEC_DEFAULT, codec_config_ptr);
  } else
#endif /* FEATURE_BT_AG */
  {
    voc_data_get_codec_config(state->current_image, 
                              pcm_path,
                              codec_config_ptr);
  }
}

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
boolean audadsp_mixer_state_mixer_codec_cfg_cmd ()
{
  qdsp_cmd_codec_config_type *codec_config_ptr = NULL;
  qdsp_cmd_status_type       result;
  voc_state_control_type     *state;

  state = voc_state_get_state_data();
  audadsp_mixer_state_get_codec_cfg(&codec_config_ptr);

  /* Issue mixer codec configuration command */
  result = qdsp_cmd_mixer_codec_config(QDSP_CMD_MIXER_MODE_CODEC_ONLY, 
                                       codec_config_ptr);
  if (result != QDSP_CMD_SUCCESS) {
    return (FALSE);
  }
  /* Mixer codec config command moves the mixer from IDLE to ACTIVE state
   * ie Control enters the Mixer state */
  state->exit_mixer_state = FALSE;

  return (TRUE);
}

/*===========================================================================

FUNCTION audadsp_mixer_state_adec_cfg_cmd

DESCRIPTION
  Sends the audio config command.

DEPENDENCIES
  None.  

RETURN VALUE
  TRUE  -- for command sending successful.
  FALSE -- for command sending failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean audadsp_mixer_state_adec_cfg_cmd ()
{
  qdsp_cmd_codec_config_type *codec_config_ptr = NULL;
  qdsp_cmd_status_type       result;

  audadsp_mixer_state_get_codec_cfg(&codec_config_ptr);

  /* Issue mixer codec configuration command */
  result = qdsp_cmd_mixer_adec_config(codec_config_ptr);

  if (result != QDSP_CMD_SUCCESS) {
    return (FALSE);
  }

  return (TRUE);
}

/*===========================================================================

FUNCTION audadsp_mixer_state_voc_cfg_cmd

DESCRIPTION
  Sends the vocoder config command.

DEPENDENCIES
  None.  

RETURN VALUE
  TRUE  -- for command sending successful.
  FALSE -- for command sending failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean audadsp_mixer_state_voc_cfg_cmd ()
{
  qdsp_cmd_codec_config_type *codec_config_ptr = NULL;
  qdsp_cmd_status_type       result;

  audadsp_mixer_state_get_codec_cfg(&codec_config_ptr);

  /* Issue mixer codec configuration command */
  result = qdsp_cmd_mixer_voc_config(codec_config_ptr);
  if (result != QDSP_CMD_SUCCESS) {
    return (FALSE);
  }

  return (TRUE);
}

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
voc_state_return_type audadsp_mixer_state_active_go_idle (void)
{
  voc_state_return_type ret_val;

  /* Local buffer for the command */
  uint16 idle_command[2]={AUDADSP_MIXER_IDLE_CMD_SIZE, 
                          AUDADSP_MIXER_STATE_IDLE_CMD_HDR};

  voc_state_control_type        *state;

  state      = voc_state_get_state_data();

  /* Reset codec clock and jump back to IDLE state */
  #ifdef FEATURE_AVS_CODEC_CLK_SRC_MGMT
#error code not present
  #else
  voc_state_set_icodec_clk(VOC_SAMPLE_RATE_8000);
  #endif  /* FEATURE_AVS_CODEC_CLK_SRC_MGMT */

  /* Allow the clock to stablize */
  (void) rex_timed_wait(VOC_STATE_GP_SIG, &voc_state_gp_timer, 20);

  if (qdsp_mixer_command(idle_command) != QDSP_CMD_SUCCESS) {
    if (state->retry_counter < 100) {
      MSG_ERROR("Failed to issue MIXER IDLE command",0,0,0);
      state->retry_counter++;
    } else {
      MSG_ERROR("MIXER transition to IDLE failed",0,0,0);
      state->state = VOC_STATE_ERROR_RECOVERY;
    }
    ret_val = VOC_STATE_CONTINUE;
  } else {

    MSG_HIGH("MIXER ACTIVE state Going to IDLE state", 0, 0, 0);

    qdsp_write(QDSP_adecDataHostStickyInt, 0);

    // Disable eclock, if eclock is already disabled then this call will
    // be dummy else it will disable eclock
    voc_state_disable_eclock();
    voc_state_adie_stall();

    state->state  = VOC_STATE_MIXER_IDLE_ENTRY;
    ret_val = VOC_STATE_CONTINUE_IMMEDIATE;
  }

  return (ret_val);
}

/*===========================================================================

FUNCTION audadsp_mixer_adec_ready

DESCRIPTION
  This is to check the adec state.

DEPENDENCIES
  None.

RETURN VALUE
 
SIDE EFFECTS
 
===========================================================================*/

boolean audadsp_mixer_adec_ready() {
  return ((audadsp_adec_get_state() == AUDADSP_ADEC_STATE_ACTIVE)?TRUE:FALSE);
}

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */

#endif /* FEATURE_AVS_INCALL_MIXER_ADEC_PB  ||
          FEATURE_AVS_INCALL_MIXER_CMX */

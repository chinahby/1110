#ifndef SNDIHW_H
#define SNDIHW_H
/*===========================================================================

       S O U N D   I N T E R N A L   H A R D W A R E   R O U T I N E S

DESCRIPTION
  The file SNDIHW.H defines the "generic level hardware interface".  The
  actual routines that drive the hardware are "hidden" in the target
  specific routines SNDHWG1.C (all 1st Generation Phones), SNDHWG2.C
  (all 2nd Generation Phones) and so on.  These routines are for the
  exclusive use of the Sound Driver (SND.C).

Copyright(c) 1995 - 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/sndihw.h_v   1.0   06 Oct 2001 12:56:50   ropalsky  $
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndihw.h#11 $ $DateTime: 2009/10/21 01:52:26 $ $Author: kkmajeti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/20/09   kkm     Added function sndhw_get_audfmt_need_config to provide audfmt 
                   configuration status.
04/20/09    rm     CR 168030, handling voc_state_get_high_module
01/19/09   anb     Added sndhw_set_qtv_cb_func so that QTV can be informed
                   about mod/image swap event.Fixes Cr 163807(SR-1133583)
01/19/09    dp     CR 168251 Removed feturization for FEATURE_BT_QSOC
12/26/08    rm     Added RVE API changes
10/22/08    dp     FM over BT AG changes
09/15/08   knm     Added support for Aux Line In(FM) volume control
06/20/08   knm     Added sndhw_set_aux_line_in API to configure AuxPGA Line In 
                   path.
12/19/07    hs     Added support ISDB-T dual mono output control
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/08/07  anb/wfd  Added aux_pcm_clk to be enabled/disabled from other tasks like BT (CR 122106).
11/20/06    sp     Added support for OEMSBL Audio.
11/09/06    az     Merged the code for MM API changes.
07/31/06    sp     Fixed some merge issues in AV Sync feature and
                   some issues in SCO rework feature 
07/27/06   anb     Added the FEATURE_AVS_BT_SCO_REWORK which enables the AVS to send
                   messages to the BT driver
03/27/06    ak     Added support for sidetone control.
01/30/06    sp     Added support for I2S SBC interface.
09/18/05    sp     Added support for DSP based SBC encoder.
09/14/05    sk     Re featurized code so that FEATURE_ARM_POST_PROCESSING is 
                   split into RX interface and RX and TX interface. Enabling
                   FEATURE_ARM_POST_PROCESSING enables both RX and TX. Enabling
                   FEATURE_AVS_WB_PCM_RX_INTERFACE enables only RX.
08/31/05    sm     VOX support
08/30/05    sp     Added function prototype for sndhw_bt_a2dp_suspend(). 
                   Audiosbc will call this function to suspend a2dp link.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/19/05    pl     Modified prototype of sndhw_audfmt_adec_data.
07/13/05    sp     Added function prototype for sndhw_free_sbc_buffer to 
                   eliminate a compiler warning.
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for MultiMedia/AAC record functionality.
08/11/04    st     Updated sndhw_audfmt_cmd() function prototype.
04/22/04    lg     Added support for Animated AAC Ringers.
03/22/04    st     Added sndhw_pcm_record() and sndhw_pcm_record_stop()
                   function prototypes.
                   Updated sndhw_audfmt_adec_chan_pause() and 
                   sndhw_audfmt_adec_pause() function prototypes; removed
                   sndhw_set_seq_vol() function prototype.
01/26/04    lg     Modiied code to support multi_sequencer.
12/02/03    sg     Added support for linear PCM playback.
11/24/03    aw     Added sndhw_midi_out_path_ready() function prototypes.
10/16/03    aw     Updated sndhw_set_adec_mode() function prototype.
03/10/03    st     Added sndhw_audfmt_adec_pause() for FEATURE_ADPCM.
02/28/03    st     Added sndhw_ec_ctl() and sndhw_ns_ctl() function prototypes.
02/19/03    st     Refeaturized function prototypes for FEATURE_ADPCM and
                   FEATURE_QSYNTH_ADPCM.
11/19/02    st     Added sndhw_audfmt_synth_mvol() and sndhw_audfmt_adec_mvol()
                   function prototypes.
08/13/02  aw/st    Added sndhw_set_adec_mode() function prototype.
07/11/02    yl     Modified the prototype for sndhw_qcp_record().
10/01/01    st     Modified prototypes for sndhw_audfmt_adec_chan_vol(),
                   sndhw_audfmt_adec_chan_pan(), sndhw_audftm_adec_data(),
                   sndhw_audfmt_adec_flush() and sndhw_audfmt_adec_reset()
                   to support multiple ADEC channels.
09/03/01    st     Added QCP recording function prototypes.
06/18/01  jkl/ym   Added FEATURE_ACP.
04/26/01    st     Updated sndhw_audfmt_adec_data() function prototype.
04/04/01    st     Added sndhw_audfmt_adec_reset(), sndhw_midi_out(), 
                   sndhw_midi_out_msg() and sndhw_audfmt_stereo_pan()
                   prototypes; lint updates.
12/04/00    sm     Added adec_chan_vol(), adec_chan_pan() protos.
          st/sm    Added MIDI, AUDMAIN, ADEC support.
09/25/00    sm     Added sndhw_bt_ag_init().
04/13/00    st     Added sndhw_audfmt_cmd() and sndhw_audfmt_cleanup() 
                   function prototypes.
02/10/00    st     Removed sndhw_voc_acquire_needed() and sndhw_allow_sleeping()
                   function prototypes.
11/15/99    st     Removed unnecessary function prototypes.
09/09/99  jkl/st   Removed SND_AUX_ALERT. Added AGC/AVC control functions.
08/24/99    st     Added sndhw_click() function prototype.
08/17/99    st     Re-designed according to the New Sound Calibration Files
06/30/99  sm/RPW   Integrated AGC feature with VR/VM features.
03/22/99    st     Added snd_rx_mute_ctl() prototype.
03/04/99    st     Removed ringer and vibrator function prototypes (moved to 
                   ring.h), renamed remaining ringer functions with prefix 
                   sndhw_ instead of sndrng_.
01/13/99    st     Added DTMF_CONST and VOL_CONST defines to compile volume
                   and DTMF tables into ROM for ARM targets; added function 
                   prototypes for voice prompting support; moved VOL_MEMORY
                   and VOL_CONST definitions to sndi.h.
12/04/98    sm     Support selectable ROM keyword for backward compatibility.
10/08/98    st     HFK-VR mainlining: modified sndhw_extcom_ctl() funtion 
                   prototype; added funtion prototypes sndhw_extcom_protocol(),
                   sndhw_hfk_phs(), sndhw_hfk_aec() sndhw_hfk_vr_tone().
10/08/98    st     Added sndhw_voc_change_path_set() prototype.
06/22/98    st     Added sndhw_adsp_tone() prototype.
01/16/98    ro     Added sndhw_extcom_ctl().
01/12/98    ro     Added mute parameter to sndhw_stereo_mute.
                   Update copyright.
12/08/97    ha     Added sndrng_click() prototype. 
06/17/97    ro     No longer need Q phone fix for DTMF_MEMORY
                   (Q is a target - T_Q - now)
04/29/97    ro     Added sndhw_hfk_ctl() for support of
                   TGP carkit - "NGP Plus".
04/11/97    ro     Don't need sndhw_voc_acquire to be available to others.
                   Fix definition of DTMF_MEMORY for "Q" phone.
                   Changed sndhw_rxfm_mute_ctl(). to sndhw_rx_mute_ctl().
                   Update dependencies.
03/28/97    ro     Support for TGP. Change sndhw_stop_ring changed to
                   sndrng_stop_ring. Change sndhw_ring to sndrng_ring.
                   Remove prototypes for sndhw_st_mode, sndhw_lpf_mode,
                   sndhw_wbd_mode, sndhw_1270_tone, sndhw_1270_stop_tone,
                   sndhw_splt_ctl.  Only T_G defines ringer table in RAM,
                   Remove T_T and T_MD. Remove extern definition of
                   sndhw_dtmf. Use FEATURE_VIBRATOR.
03/12/97   jjn     Added (TG==T_MD) for module in conditional compile statements
02/12/97    ro     Added sndhw_vibrator_ctl().
01/07/97    ro     Added SND_TUNE_VOLUME to support setting volume levels.
                   Add sndhw_path_timer_ctl.
                   Change interface to sndhw_voc_tone.
                   Change interface to sndhw_ring.
                   Add snd_setup_voice_path
                   Add snd_get_tone_volume
                   Change interface to sndhw_set_apath_out, and
                   sndhw_set_apath_in.
10/14/96    ro     Added (TG==T_T) cases to all #if's that have (TG==T_G)
                   for initial support of TGP build.
07/22/96   fkm     Minor Documentation Tweeks
03/19/96   fkm     Updates Per Code Review
02/16/96   fkm     Tweeks to comments and documentation
02/12/96   fkm     Lint and Documentation Updates.
12/13/95   fkm     Lint Nits
11/30/95   fkm     sndhw_dtmf[] can now move to RAM for some targets
10/11/95   fkm     Defintion for sndhw_voc_acquire() added to this level.
08/22/95   fkm     Added Defintion for sndhw_stereo_mute()
07/06/95   fkm     Documentation Updates Per Code Review
04/24/95   fkm     Changes to Support Gemini.
01/17/95   fkm     Created file.

===========================================================================*/

#include "comdef.h"
#include "snd.h"
#include "sndi.h"   /* pick up SND_ROM definition */
#include "sndgen.h"
#include "voc.h" /*For getting the BT enum type*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_register_aux_ctl

DESCRIPTION
  This function will register the function provided by the client
  to use in the external generator action

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
/* Define the function pointer for controlling the auxilliary generator  */
/* In this case, turn the vibrator on and off                            */
extern void sndhw_register_aux_ctl( snd_aux_func_ptr_type aux_ctl_fn_ptr);
/* <EJECT> */
/*===========================================================================
FUNCTION snd_setup_voice_path

DESCRIPTION
  Setup the state of an audio output path. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
extern void snd_setup_voice_path
( 
  snd_device_type       device,      /* Setting for physical path */
  snd_mute_control_type mute,        /* Mute setting              */
  snd_pstate_type       *path_state  /* Output: path state        */
);

/* <EJECT> */
/*===========================================================================
 FUNCTION sndhw_voc_module_fetched

 DESCRIPTION
	 Inform upper layer that the Vocoder is unable to support current requested
	 module. Expires snd_index for which voc_config
	 was requested.

 DEPENDENCIES
	 sndhw_curr_index
	 sndhw_curr_source
	 sndhw_curr_config

 RETURN VALUE
	 None

 SIDE_EFFECTS
 	 sndhw_current_index
	 sndhw_current_source
	 sndhw_current_config

 ===========================================================================*/
 extern void sndhw_voc_module_fetched(void);


/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_voc_change_path_set

DESCRIPTION
  This routine does a path set and possibly signals the IDS task after the
  vocoder state has changed to VOC_ACQ_RE_INIT_S.

DEPENDENCIES
  extcom_task_ptr
  extcom_sigs
  sndhw_ext_com_state

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_ext_com_state

===========================================================================*/
extern void sndhw_voc_change_path_set( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_is_path_ready

DESCRIPTION
  Returns the path status.

DEPENDENCIES
  snd_path_is_ready

RETURN VALUE
  SND_PATH_NOT_READY  - if path is not ready
  SND_PATH_VOC_READY  - if vocoder is acquired but path is not yet ready
  SND_PATH_READY      - if path is ready

SIDE EFFECTS
  None.

===========================================================================*/
extern snd_path_ready_type sndhw_is_path_ready( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_path_timer_ctl

DESCRIPTION
  Mark paths ready after a fixed delay period.

DEPENDENCIES
  SND_CAR_KIT_ON_TIME
  SND_EARPIECE_ON_TIME
  snd_path_timer

RETURN VALUE
  None

SIDE EFFECTS
  snd_path_is_ready

===========================================================================*/
extern void sndhw_path_timer_ctl( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_stereo_mute

DESCRIPTION
  Attempt to mute/unmute the Car Stereo as requested by Regional Flags.
  This assumes of course that we are connected to a Car Kit that supports
  stereo mute.

DEPENDENCIES
  sndhw_ext_com_state
  sndhw_hfk_protocol

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_stereo_mute_s
  extcom_stereo_mute_s

===========================================================================*/
extern void sndhw_stereo_mute( boolean mute );
/* <EJECT> */
#ifdef SND_HFK_PLUS
#error code not present
#endif /* SND_HFK_PLUS */
/* <EJECT> */
#ifdef SND_EXTCOM
#error code not present
#endif /* SND_EXTCOM */
/* <EJECT> */
/*===========================================================================

FUNCTION snd_rx_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the audio input from the FM receive path.
  The parameter allows multiple requesters to request muting and for the
  audio path to unmute only when all requesters have un-muted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void snd_rx_mute_ctl
(
  snd_mute_type mask,
    /* mask of audio mute request associated with caller */

  boolean mute_audio
    /* True = mute audio, False = un-mute.               */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_txfm_mute_ctl

DESCRIPTION
  Temporary mute/unmute of the audio output to the FM transmit path.
  
  Note:  this does not mute DTMF output to the FM transmit path, use
  snd_dtmf_tx_mute_ctl() to mute DTMF output.

DEPENDENCIES
  Multiple Mute Sources is supported at snd_txfm_mute_ctl() level.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_txfm_mute_ctl
(
  boolean mute_audio
    /* True = mute audio, False = un-mute.                              */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_dtmf_tx_mute_ctl

DESCRIPTION
  Control mute/unmute of the DTMF output to the FM transmit path.

DEPENDENCIES
  snd_dtmf_tx_mute_ctl() has additional code to allow multiple source
     muting.

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================*/
extern void sndhw_dtmf_tx_mute_ctl
(
  boolean mute_audio
    /* True = mute audio, False = un-mute. */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_gen_tone

DESCRIPTION
  This routine is called by the Generic Sound Task Level to actually begin
  the generation of a tone.

DEPENDENCIES
  snd_ring_busy
  snd_voc_busy
  sndhw_aux_busy
  sndhw_adsp_busy
  sndhw_hfk_protocol
  sndhw_hfk_vr_tone_ready

RETURN VALUE
  TRUE  - if tone is on hold
  FALSE - if tone is playing

SIDE EFFECTS
  snd_ring_volume

===========================================================================*/
extern boolean sndhw_gen_tone
(
  tone_status_type *tone_status_ptr
);

#if defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC)
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_bt_init

DESCRIPTION
  Initialization for Bluetooth Audio Gateway communication.

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_bt_init( void );

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_free_sbc_buffer

DESCRIPTION
  Frees the SBC PP buffer
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_free_sbc_buffer( uint16 index );


#endif /* FEATURE_BT_AG || FEATURE_SBC_CODEC */

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_init

DESCRIPTION
  Generic Low Level Hardware Initialization for Sound Functions and chips.

DEPENDENCIES
  The Timers and Queues in snd_init() must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_init( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_filters_ctl

DESCRIPTION
  Some targets allow different types of "Filtering" to be done by Sound.
  For instance, most data calls need VOXing turned off.

DEPENDENCIES
  snd_current_filters

RETURN VALUE
  None

SIDE EFFECTS
  snd_current_filters

===========================================================================*/
extern void sndhw_filters_ctl
(
  snd_filters_type filters
    /* Type of Filtering Current Desired */
);
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_set_int_volume

DESCRIPTION
  Set the volume for a given device and method in the database.

DEPENDENCIES
  none

RETURN VALUE
  TRUE  - if a new volume setting is made.
  FALSE - if no volume setting was made.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean sndhw_set_int_volume (
  snd_device_type device,    /* device */
  snd_method_type method,    /* method */
  byte            volume     /* volume */
);
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_set_volume

DESCRIPTION
  Set the output volume according to the current output path.  This includes
  muting if the output is muted.

DEPENDENCIES
  snd_crnt_apath_out

RETURN VALUE
  None

SIDE EFFECTS
  snd_voc_dtmf_volume

===========================================================================*/
extern void sndhw_set_volume( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ec_ctl

DESCRIPTION
  Controls EC (Echo Canceller).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_ec_ctl (
  snd_ec_ns_mode_type   ec_mode
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ns_ctl

DESCRIPTION
  Controls NS (Noise Suppressor).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_ns_ctl (
  snd_ec_ns_mode_type   ns_mode
);

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_vox_ctl

DESCRIPTION
  Controls VOX (half/full duplex voice mode).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_vox_ctl (
  snd_vox_mode_type   vox_mode
);

/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_aud_process_ctl

DESCRIPTION
  Controls whether ESEC (Ear-Seal Echo Cancellation) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If aud-in or aud-out is muted, the associated switches are set to mute.
  Sets snd_vox_mode.

===========================================================================*/
extern void sndhw_aud_process_ctl( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_sidetone_ctl

DESCRIPTION
  Controls sidetone.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_sidetone_ctl (
  snd_sidetone_mode_type   sidetone_mode
);

/* <EJECT> */
#ifdef FEATURE_AUDIO_AGC
/*===========================================================================

REGIONAL FUNCTION sndhw_avc_ctl

DESCRIPTION
  Controls whether AVC (automatic volume control) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_avc_on

===========================================================================*/
extern void sndhw_avc_ctl(boolean avc_on);
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_agc_ctl

DESCRIPTION
  Controls whether AGC (automatic gain control) is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_agc_on

===========================================================================*/
extern void sndhw_agc_ctl(boolean agc_on);
#endif /* FEATURE_AUDIO_AGC */


/* <EJECT> */
/*==========================================================================  

REGIONAL FUNCTION sndhw_smartsnd_ctl

DESCRIPTION
  Controls AGC/AVC/RVE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_smartsnd_ctl
  

==========================================================================*/
extern void sndhw_smartsnd_ctl(snd_smartsnd_mode_type smartsnd);



/* <EJECT>*/
#ifdef FEATURE_AUDIO_AGC
/*==========================================================================
 FUNCTION sndhw_smartsnd_aud_agc_process_ctl

 DESCRIPTION
   Controls AGC,AVC enabling.

 DEPENDENCIES
   sndhw_smartsnd_status

 RETURN VALUE
   none

 SIDE EFFECTS
   none
  
 ==========================================================================*/

extern void sndhw_smartsnd_aud_agc_process_ctl( void );
#endif /*FEATURE_AUDIO_AGC*/



/* <EJECT>*/
/*==========================================================================
 FUNCTION  sndhw_smartsnd_aud_rve_process_ctl

 DESCRIPTION
   Controls RVE enabling.

 DEPENDENCIES
   sndhw_smartsnd_status

 RETURN VALUE
   none

 SIDE EFFECTS
   none
 
 ==========================================================================*/

extern void sndhw_smartsnd_aud_rve_process_ctl( void );



/* <EJECT>*/
/*==========================================================================

 FUNCTION sndhw_smartsnd_aud_process_ctl


 DESCRIPTION
   Controls whether ESEC (Ear-Seal Echo Cancellation) is enabled or not.
   Also controls RVE and AGC.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   If aud-in or aud-out is muted, the associated switches are set to mute.
   Sets snd_vox_mode.

 ===========================================================================*/  

extern void sndhw_smartsnd_aud_process_ctl( void );


/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_set_apath_in

DESCRIPTION
  Change input paths

DEPENDENCIES
  The Sound Task must have been started in order to initialize the hardware.
  snd_analog_mode
  snd_tx_muted

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_set_apath_in
(
  snd_mute_control_type   mute
    /* audio input path mute state */
);
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_set_apath_out

DESCRIPTION
  Change output paths

DEPENDENCIES
  The Sound Task must have been started in order to initialize the hardware.
  Cannot be called from interrupt context, cannot be called from any task
  other than the sound task.

RETURN VALUE
  None

SIDE EFFECTS
  snd_crnt_path_out
  snd_voc_acquired

===========================================================================*/
extern void sndhw_set_apath_out
(
    snd_pstate_type *apath_out
    /* audio output path to use */
);
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_set_tone_path_out

DESCRIPTION
  Change output paths depending on the generator.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_tone_playing
  sndhw_keep_voc_acquired
  snd_override_timer
  sndhw_unmute_flag
  snd_unmute_timer

===========================================================================*/
extern void sndhw_set_tone_path_out (
  snd_pstate_type *tone_path_out,
  snd_pstate_type *voice_path_out
);
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_overrride_done

DESCRIPTION
  This function signals the end of the override timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_keep_voc_acquired
  snd_override_timer

===========================================================================*/
extern void sndhw_override_done( void );
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_unmute_done

DESCRIPTION
  This function signals the end of the unmute timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_unmute_flag
  snd_unmute_timer

===========================================================================*/
extern void sndhw_mute_done( void );
/* <EJECT> */
/*===========================================================================

REGIONAL FUNCTION sndhw_restore_voice_path

DESCRIPTION
  This function restores the voice path after tones are finished playing.

DEPENDENCIES
  sndhw_keep_voc_acquired
  snd_override_timer
  sndhw_unmute_flag
  snd_unmute_timer

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_tone_playing
  sndhw_unmute_flag

===========================================================================*/
extern void sndhw_restore_voice_path (
  snd_pstate_type      *path_out,
  snd_mute_control_type path_in_mute
);
/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION sndhw_voc_vp_enc

DESCRIPTION
  Enqueue as many voice prompts onto the vocoder VP queue.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Modifies snd_prompt_q and snd_prompt_ptr_q.

===========================================================================*/
LOCAL void sndhw_voc_vp_enq(void);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_voc_vp

DESCRIPTION
  Send a command to the vocoder to have it start playing a voice prompt.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Sets snd_voc_busy to indicate the state of the Vocoder.

===========================================================================*/
LOCAL void sndhw_voc_vp
(
  tone_status_type *tone_status_ptr
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_cleanup_vp

DESCRIPTION
  Calls vocoder driver voice prompt clean up function and mutes rx path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_cleanup_vp(void);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_abort_vp

DESCRIPTION
  Sends vocoder voice prompt driver signal to end voice playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_abort_vp(void);

/**********   P R O M P T   P T R   Q U E U E   F U N C T I O N S   ********/
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_free_all

DESCRIPTION
  This function frees all entries in the internal prompt ptr queue and resets
  the current prompt ptr queue to the internal one.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_prompt_ptr_q
  sndhw_prompt_ptr_q_list

===========================================================================*/
extern void sndhw_ppq_free_all( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_pop_single

DESCRIPTION
  This function will free a single entry in the internal prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty or not internal prompt queue being used.
  SND_PPQ_PQ_SUCCESS if enqueue is not empty.

SIDE EFFECTS
  sndhw_prompt_ptr_q

===========================================================================*/
LOCAL snd_ppq_pq_return_type sndhw_ppq_pop_single( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_set_list

DESCRIPTION
  This function sets the current list of voice prompts to use.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_SUCCESS

SIDE EFFECTS
  sndhw_prompt_ptr_q_list

===========================================================================*/
extern snd_ppq_pq_return_type sndhw_ppq_set_list (
  voc_pb_header_type **prompt_list
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_enqueue

DESCRIPTION
  This function enqueues a single pointer onto the internal prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is full.
  SND_PPQ_PQ_SUCCESS if able to queue entry.

SIDE EFFECTS
  sndhw_prompt_ptr_q

===========================================================================*/
extern snd_ppq_pq_return_type sndhw_ppq_enqueue (
  voc_pb_header_type *vp,     /* Pointer to voice prompt to enqueue        */
  const void         *client_data,
  word               *idx     /* Returns index into queue that VP was NQed */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_get_ptr

DESCRIPTION
  This function returns the specified entry from the current prompt ptr queue.

DEPENDENCIES
  sndhw_prompt_ptr_q

RETURN VALUE
  Entry in prompt ptr queue that was specified.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL snd_ppq_pq_return_type sndhw_ppq_get_ptr (
  int                index,          /* Index into queue to get VP pointer */
  voc_pb_header_type **vp,
  const void         **client_data
);
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_ppq_internal_q

DESCRIPTION
  This function checks if the internal prompt ptr queue is current.

DEPENDENCIES
  sndhw_prompt_ptr_q_list

RETURN VALUE
  TRUE if the internal prompt ptr queue is current
  FALSE if the internal prompt ptr queue is not current

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_ppq_internal_q( void );
/* <EJECT> */
/**************   P R O M P T   Q U E U E   F U N C T I O N S   ************/
/*===========================================================================

FUNCTION sndhw_pq_free_all

DESCRIPTION
  This function frees all the entries in the intermediate prompt queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
extern void sndhw_pq_free_all( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_pq_enqueue

DESCRIPTION
  This function enqueues an entry onto the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is full.
  SND_PPQ_PQ_SUCCESS if enqueued successfully.

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
extern snd_ppq_pq_return_type sndhw_pq_enqueue( word index );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_pq_empty

DESCRIPTION
  This function checks if the intermediate prompt queue is empty.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  TRUE if the intermediate prompt queue is empty.
  FALSE if the intermediate prompt queue is not empty.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean sndhw_pq_empty( void );
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_pq_peek

DESCRIPTION
  This function returns the top entry in the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty.
  SND_PPQ_PQ_SUCCESS if queue is not empty.
  Top entry in intermediate prompt queue.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL snd_ppq_pq_return_type sndhw_pq_peek( int *index );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_pq_dequeue

DESCRIPTION
  This function dequeues the top entry in the intermediate prompt queue.

DEPENDENCIES
  sndhw_prompt_q

RETURN VALUE
  SND_PPQ_PQ_FAIL if queue is empty.
  SND_PPQ_PQ_SUCCESS if queue is not empty.

SIDE EFFECTS
  sndhw_prompt_q

===========================================================================*/
LOCAL snd_ppq_pq_return_type sndhw_pq_dequeue( void );

#endif /* FEATURE_VOICE_PROMPT */

/* <EJECT> */
#ifdef FEATURE_AUDIO_FORMAT
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION sndhw_audfmt_cmd

DESCRIPTION
  Send midi commands to the vocoder with zero volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_audfmt_cmd ( audmain_cmd_buf_type *cmd_buf );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_synth_mvol

DESCRIPTION
  This function sets the synth master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_synth_mvol (
  uint16 vol
);
#endif /* FEATURE_MIDI */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION sndhw_audfmt_adec_mvol

DESCRIPTION
  This function sets the ADEC master volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_mvol (
  uint16 vol
);
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_vol

DESCRIPTION
  Modifies the ADEC channel volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 volume
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_pan

DESCRIPTION
  Modifies the ADEC channel panning.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
);

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_audfmt_adec_chan_pause

DESCRIPTION
  Transfers the MIDI command buffer to the QDSP for processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION sndhw_set_adec_mode

DESCRIPTION
  This function setup the QSynth audio decoder mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_set_adec_mode( snd_set_adec_mode_enum_type mode );
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_cleanup

DESCRIPTION
  Unconfigures vocoder after midi is finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_audfmt_cleanup ( void );

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION sndhw_audfmt_adec_data

DESCRIPTION
  Moves data from Audfmt/Sound task to audio decoder playback buffers in QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type sndhw_audfmt_adec_data(
  snd_adec_mode_type adec_mode,
  voc_adec_cmd_type  *adec_cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_adec_reset

DESCRIPTION
  Resets audio decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_audfmt_adec_reset ( uint32 reset_mask );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_adec_pause

DESCRIPTION
  Pauses audio decoder playback and interrupts in the qdsp.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_audfmt_adec_pause (
  boolean pause
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_adec_int_pending

DESCRIPTION
  This function checks for a pending ADEC interrupt.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if an interrupt is pending.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_int_pending(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_stereo_pan

DESCRIPTION
  Issues a stereo pan command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_stereo_pan (
  uint8 pan_factor
);
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_ADEC
/*===========================================================================

FUNCTION sndhw_audfmt_adec_flush

DESCRIPTION
  Resets audio decoder playback buffers and internal states in the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_audfmt_adec_flush ( uint16 flush_mask );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_audfmt_adec_pcm_stopped

DESCRIPTION
  This function checks if PCM output has stopped.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PCM output has stopped.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_audfmt_adec_pcm_stopped(void);

#endif /* FEATURE_ADEC */

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION sndhw_qcp_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_qcp_record (
  snd_qcp_format_type      format,
  snd_qcp_dir_type         link,
  snd_mm_qcp_rec_para_type *rec_param,
  voc_rec_cb_type          cb_func,
  const void               *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_qcp_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_qcp_record_stop ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_qcp_record_pause

DESCRIPTION
  This function sends a command to the vocoder driver to pause
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_qcp_record_pause ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_qcp_record_resume

DESCRIPTION
  This function sends a command to the vocoder driver to resume
  recording QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_qcp_record_resume ( void );
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION sndhw_pcm_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_pcm_record (
  snd_pcm_rec_dir_type           link,
  voc_pcm_client_output_fn_type  cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_pcm_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_pcm_record_stop (
  snd_pcm_rec_dir_type link
);
#endif /* FEATURE_PCM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION sndhw_midi_out

DESCRIPTION
  This function enables the MIDI device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type sndhw_midi_out(
#ifdef FEATURE_MIDI_OUT_QCP
  voc_adec_cb_func_struct adec_cb,
  void                    *adec_client_data,
#endif /* FEATURE_MIDI_OUT_QCP */
  snd_midi_out_type       enable
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_midi_out_msg

DESCRIPTION
  This function sends midi commands to the midi device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern snd_status_type sndhw_midi_out_msg (
  uint16 num_commands,
  uint16 num_words,
  uint16 *buf
);

/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_midi_out_path_ready

DESCRIPTION
  This function return the status of midi out path.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The path is ready.
  FLASE - The path is not ready.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_midi_out_path_ready(void);

#endif /* FEATURE_MIDI_OUT */

/* <EJECT>*/
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION sndhw_aac_set_song_config

DESCRIPTION
  Sets the DSP configuration for AAC

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_aac_set_song_config(
  voc_aac_config_type* config
  );

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_aac_mpeg_header_config

DESCRIPTION
  Sets the MPEG header configuration for AAC

DEPENDENCIES
  AAC DSP Image must be loaded already.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_aac_set_mpeg_header(
  snd_audfmt_mpeg_header_config* config
  );

#endif

/* <EJECT>*/
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION sndhw_get_audfmt_need_config

DESCRIPTION
  Get the status whether audfmt configuration is needed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Audfmt configuration is not done
  FALSE - Audfmt configuration is done

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_get_audfmt_need_config();
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
/*===========================================================================

FUNCTION sndhw_set_sample_rate

DESCRIPTION
  Send sample rate command to the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_set_sample_rate (
  snd_sample_rate_type sample_rate
);
#endif

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION sndhw_mm_record

DESCRIPTION
  This function sends a command to the vocoder driver to start
  recording MultiMedia data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_mm_record (
  snd_mm_rec_aud_type       *rec_param,   /* Record parameters              */
  voc_mm_rec_cb_fn_type     cb_func
); 

/*===========================================================================

FUNCTION sndhw_mm_record_stop

DESCRIPTION
  This function sends a command to the vocoder driver to stop
  recording MultiMedia data.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - success  
  FALSE - faulure

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_mm_record_stop (void);

/*===========================================================================

FUNCTION sndhw_mm_record_data

DESCRIPTION
  Get the recorded data from QDSP to snd layer. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sndhw_mm_record_data(
  snd_mm_rec_format_type      mm_type,
  audmain_mm_record_buf_type  *buf,
  audmain_mm_record_data_type *frame_info
);
#endif /* FEATURE_MM_REC */

#if defined FEATURE_SBC_CODEC || defined FEATURE_SBC_DSP_CODEC || \
    defined FEATURE_AVS_WB_PCM_RX_INTERFACE
/*===========================================================================

FUNCTION sndhw_free_wb_pcm_rx_interface_buffer

DESCRIPTION
  Frees the wb_pcm_rx_interface_buffer 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_free_wb_pcm_rx_interface_buffer( uint16 index );

/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_bt_a2dp_suspend

DESCRIPTION
  Suspend the BT A2DP connection 
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_bt_a2dp_suspend(void);
#endif /* FEATURE_SBC_CODEC || defined FEATURE_AVS_WB_PCM_RX_INTERFACE */ 

#ifdef FEATURE_SBC_DSP_CODEC
/*===========================================================================

FUNCTION sndhw_sbc_configure

DESCRIPTION
  This function sets the SBC configuration information with the audio drivers.  
  When a DSP image that supports SBC gets loaded onto ADSP audio driver will
  configure the SBC encoder in DSP with these parameters.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_sbc_configure(uint32 blocks, uint32 mode, uint32 alloc);

/*===========================================================================

FUNCTION sndhw_sbc_set_bitrate

DESCRIPTION
  Sets SBC bitrate to the DSP
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_sbc_set_bitrate(uint32 bitrate);
#endif

/* <EJECT>*/
#ifdef FEATURE_AVS_I2SSBC
/*===========================================================================

FUNCTION sndhw_i2ssbc_enable

DESCRIPTION
  This function acquires the vocoder to start I2S SBC encoding.

DEPENDENCIES
  The current codec must be able to support BT A2DP
  i.e. VOC_CODEC_BT_A2DP or VOC_BT_A2DP_SCO

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_status_type sndhw_i2ssbc_enable(
  snd_sample_rate_type               sample_rate,
  snd_i2ssbc_cb_func_ptr_type        cb_func
);
/* <EJECT>*/
/*===========================================================================

FUNCTION sndhw_i2ssbc_disable

DESCRIPTION
  This function releases the vocoder

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_i2ssbc_disable(void);
#endif /* FEATURE_AVS_I2SSBC */
/*===========================================================================

FUNCTION sndhw_set_qtv_cb_func 

DESCRIPTION
  This function is used to inform the audio drivers about the QTV
  callback to be called by audio drivers to inform QTV about the
  QDSP_MOD_DISABLE evt when the image swap is happening from audio
  only image to audio/video image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_qtv_cb_func(voc_notify_qtv_cb_func_ptr_type cb_func);

#if (defined (FEATURE_BT_AG) || defined (FEATURE_SBC_CODEC) || \
    defined (FEATURE_SBC_DSP_CODEC) )&& defined (FEATURE_AVS_BT_SCO_REWORK)
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_set_bt_cb 

DESCRIPTION
  This function is used to inform the audio drivers of the BT callback fucntion
  to be called by audio drivers to inform BT about the type of sound (whether
  the sound is supported over BT SCO, BT A2DP etc.). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_bt_cb_func(voc_bt_cb_func_ptr_type cb_func);
/*===========================================================================

FUNCTION sndhw_cleanup_tone

DESCRIPTION
  Calls the BT functions to let BT know that we have finished playing a tone
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void sndhw_cleanup_tone(void);
/*===========================================================================

FUNCTION sndhw_get_bt_playback_other_start 

DESCRIPTION
  This function is used to get the status of the other playback event 
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean sndhw_get_bt_playback_other_start(void);
/*===========================================================================

FUNCTION sndhw_set_bt_playback_other_start 

DESCRIPTION
  This function is used to set the status of the other playback event 
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_bt_playback_other_start(boolean bt_other_play_evt_status);
/*===========================================================================

FUNCTION sndhw_get_bt_playback_dtmf_start 

DESCRIPTION
  This function is used to get the status of the dtmf playback event 
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
boolean sndhw_get_bt_playback_dtmf_start(void);
/*===========================================================================
FUNCTION sndhw_set_bt_playback_dtmf_start 

DESCRIPTION
  This function is used to set the status of the dtmf playback event 
  that is sent to BT driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

==========================================================================*/
void sndhw_set_bt_playback_dtmf_start(boolean bt_dtmf_play_evt_status);

#endif /* FEATURE_BT_AG|FEATURE_SBC_CODEC|FEATURE_SBC_DSP_CODEC & AVS_BT_SCO_REWORK*/


#ifdef FEATURE_OEMSBL_AUDIBLE_TONE
/*===========================================================================

FUNCTION sndhw_set_oemsbl_cb

DESCRIPTION
  This function is called by OEMSBL code to set a call back. This callback
  will be used by vocoder drivers to let OEMSBL know when the DSP is shutdown. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_oemsbl_cb(snd_oemsbl_cb_func_ptr_type func_ptr);
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION sndhw_enable_eclock

DESCRIPTION
  This function will enable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void sndhw_enable_eclock(void);
/*===========================================================================

FUNCTION sndhw_disable_eclock

DESCRIPTION
  This function will disable (external)auxpcm clock.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A task switch might occurr

===========================================================================*/
void sndhw_disable_eclock(void);


#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*===========================================================================

FUNCTION sndhw_audfmt_adjust_sample

DESCRIPTION
  Add/Drop one audio sample.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_adjust_sample(boolean add_sample);
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION sndhw_audfmt_dual_mono
DESCRIPTION
  Set dual mono mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_dual_mono(snd_af_dual_mono_mode_enum_type  mode);

/*===========================================================================

FUNCTION sndhw_audfmt_sbr_mode

DESCRIPTION
  Set SBR mode.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean sndhw_audfmt_sbr_mode(snd_af_sbr_mode_enum_type   sbr_mode);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* FM over BT AG */
/*===========================================================================

FUNCTION sndhw_set_fm_over_bt

DESCRIPTION
  This function performs FM over BT related functionality 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_fm_over_bt (
  snd_aux_line_in_mode_type aux_mode
);

/*===========================================================================

FUNCTION sndhw_set_aux_line_in

DESCRIPTION
  Set AuxPGA Line In Path.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_aux_line_in (
  snd_aux_line_in_mode_type aux_mode
);

/*===========================================================================

FUNCTION sndhw_check_aux_line_in

DESCRIPTION
  Return aux line in status. 
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Aux Line In path is active.
  FALSE - Aux Line In path is not active. 

SIDE EFFECTS
  None.

===========================================================================*/
boolean sndhw_check_aux_line_in(
  void
);

/*===========================================================================

FUNCTION sndhw_set_aux_line_in_vol

DESCRIPTION
  Set the aux line in volume

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sndhw_set_aux_line_in_vol (uint8 volume);

#endif /* SNDIHW_H */


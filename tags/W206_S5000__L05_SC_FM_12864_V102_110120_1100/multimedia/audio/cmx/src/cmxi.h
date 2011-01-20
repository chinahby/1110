#ifndef CMXI_H
#define CMXI_H
/*===========================================================================

                  C M X   S E R V I C E S   I N T E R N A L   
                           H E A D E R   F I L E

DESCRIPTION
  This header file contains all the internal definitions to CMX services.

REFERENCES
  CMX Services ISOD: 80-24875-12 X2

Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/cmx/main/latest/src/cmxi.h#4 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/09   skr     Changes to introduce new API cmx_audfmt_avsync_playback()
                   that will send the type of ABORT to the client and retain
                   backward compatibilty of the cmx_aufmt_playcodec_av_sync()
06/31/08   vsud    modified cmx_packet_type to rmove poly mode def outside 
				   webaudio feature.
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
12/19/07    hs     Added support ISDB-T dual mono output control
12/12/07    sk     Increased CMX_MIN_BUF_SIZE to 156.CR134423.                   
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
07/19/07    st     Increased the number of CMX packets from 40 to 60, CR116861;
                   modified code to use statically defined variables for
                   audio specification callback processing.
04/01/07   st/pl   Modified code to properly abort on-going xmf operation 
                   upon receiving of new commands.
06/27/06    pl     Added 3d disable/enable on environment and per object support.
                   Added environment reset and source reset support.
06/26/06    pl     Added support for doppler and reverb.
06/08/06    lg     Added new status and a new SVG cleanup function(CR89340)
03/24/06    lg     Added support for SVGT 1.2
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device; added support for repeat
                   playback mode for audio decoder files to MIDI device.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
11/15/05    st     Added CMX_AF_STATUS_MASK_PLAY_ABORTED to
                   cmx_af_status_mask_enum_type, CR82257.
10/21/05    st     Modified code to include client buffer request under
                   FEATURE_SAF and FEATURE_SVG; modified code to include
                   rec_tx_handle and rec_rx_handle in cmx_af_handle_type
                   under FEATURE_QCP.
09/18/05    st     Added support for QAudioFX: 3D positional commands and query
                   interface.
08/24/05   ayu     Modified code to delete recorded QCP files containing only
                   header information, for CR62770.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/19/05    pl     Defined cmx_af_stop_enum_type to further differentiate the 
                   reason why playback is stopped. 
06/15/05    st     Added support for SVG commands that are embedded within
                   c-MIDI file.
05/24/05    hz     Added support for MultiMedia AAC record functionality.
04/19/05    lg     Modified code to correct SVG client frame generation
04/05/05    lg     Modified to code to allow copy of SVG event_data to CMX 
03/25/05   ac/sg   Added cmx_audfmt_play_codec_av_sync() API to support A/V
                   synchronization.
03/17/05    lg     Added support for SVG.
03/10/05    st     Added support for MFi LED control.
02/05/05    st     Modifed code to support XMF and DLS file processing;
                   modified cmx_af_handle_type to use a bitmask instead of
                   a bunch of boolean parameters.
10/12/04    st     Modified code to remove requirement that client executes
                   before CMX task for data buffer request commands.
04/23/04    lg     Chnaged saf_buffer to a pointer-to-pointer for allowing 
                   dynamic memory allocation of cmx_saf_frame_buf
04/16/04    st     Added support for animation frame positioning.
04/08/04    st     Removed JPEG encoder software, this is no longer supported
                   through CMX layer.
03/22/04    st     Added support for PCM recording; added support for data
                   read commands of variable size for PCM playback support.
                   Added cmx_midi_out_qcp_pause_chan_processing() function
                   prototype; added CMX_MIDI_OUT_QCP_PAUSE_CHAN and
                   CMX_MIDI_OUT_QCP_RESUME_CHAN commands.
                   cmx_midi_out_qcp_pause_chan_processing() function prototype.
                   Modified code to support requesting memory buffers from
                   client; removed wa_reset parameter from cmx_af_cb_packet_type
                   and cmx_audfmt_play_done_processing() because it is no
                   longer necessary.
03/02/04    sg     Increased CMX_MIN_BUF_SIZE to 80 to meet camcorder
                   constraints.
01/26/04    lg     Added support for tune, pan & volume adjustments for 
                   multi-sequencer.
01/26/04    aw     Added support to get file specs (similar to get_time).
01/21/04   lg/aw   Added support for SMAF led/vibrator/graphics info events.
12/02/03    sg     Added support for linear PCM playback.
10/16/03   aw/st   Modified code to support multi-sequence and SMAF.
02/19/03   aw/st   Added CMX_MIDI_OUT_QCP_RESERVE_CHANNEL and 
                   cmx_midi_out_qcp_reserve_channel_processing() function
                   prototype to support playback of ADPCM file with
                   midi_out_qcp() API.
11/05/02    sg     Added support for iMelody format.
10/28/02   aw/st   Modified code to support processing get time callback
                   command in CMX context;
08/13/02    yl     Modified prototype of cmx_qcp_record_processing().  
                   Add a new field in cmx_qcp_record_packet_type and a new 
                   entry in cmx_cmd_type. 
08/10/02   aw/st   Modified code to add an audio deocder type parameter to
                   the MIDI out open command.
07/25/02   sg/st   Added support for MP4 and AAC.
                   Added cmx_audfmt_play_codec(). Added support for flushing.
                   Added support for audio postcard API.
06/11/02    st     Modified code to consolidate all the common image processing
                   routines in the CMX module.
03/22/02    sa     Added support for JPEG decode and encode.
11/08/01    st     Increased CMX_NUM_PACKETS from 20 to 40.
10/11/01    aw     Modified code to support graphics task.
10/01/01    st     Modified code for MIDI device audio decoder to support
                   multiple channels; added pause, resume, volume and panning
                   functions for MIDI device ADEC;
                   modified cmx_af_aud_spec_packet_type to use a pointer to
                   cmx_af_audio_spec_type in order to reduce RAM usage.
09/16/01    st     Added CMX_QCP_SET_CNFG to cmx_cmd_type.
09/03/01    st     Added code to support QCP recording and support functions
                   (set/get information).
04/04/01    st     Updated function prototypes with LOCAL macro.
                   Added support for SAF (simple animation format); added
                   support for direct MIDI device output interface; added
                   support for client controlled data source; removed scaling
                   support from PNG decoder.
02/28/01    st     Added support for audio format preparse function; lint 
                   updates.
12/04/00    sm     Added cmx_audfmt_pan_processing(), pan command.
11/27/00  st/sm    Featurized EFS, MP3 support.  Removed RESET_MINOR command.
07/25/00    st     Added support for starting position indicator in the
                   cmx_audfmt_play() and cmx_audfmt_ringer() functions.
07/15/00    st     Added support for LED, audio spec, tempo and tune functions;
                   added CMX_AF_CMD_RESET_MINOR to the reset command.
06/13/00    st     Added code to handle the returning of non-PNG images
                   in WebAudio files to the client; modified code to handle
                   text positioning.
05/18/00    st     Featurized WebAudio and PNG decoder specific code; added PNG
                   specification and text commands; changed adv_start to
                   seek_pause.
05/09/00    st     Added PNG decoder module commands and prototypes.
04/28/00    st     Added CMX_AF_CMD_ADV_START; added cmx_png_decode_processing()
                   and cmx_audfmt_adv_start_processing() prototypes;
                   added cmx_png_decode_cb_func_ptr_type to cmx_af_handle_type.
04/13/00    st     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#ifdef FEATURE_CMX
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Definitions for queue services                  */
#include "cmx.h"        /* CMX services typedefs and prototypes            */
#include "snd.h"        /* Sound typedefs and prototypes                   */

#ifdef FEATURE_CMX_IMAGE
#include "graph.h"      /* Graphics Task for PNG, SAF, and JPEG            */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_EFS
#include "fs.h"         /* File system typedefs and prototypes             */
#endif

#ifdef FEATURE_SVG
#include "rex.h"        /* For SVG Frame timers                            */
#include "svg.h"        /* SVG decoder                                     */
#endif /* FEATURE_SVG */

/* <EJECT> */
/*---------------------------------------------------------------------------
** CMX General Defines
**---------------------------------------------------------------------------
*/

/* This signal is set when an item is put on the cmx_cmd_q.
*/
#define VS_CMX_CMD_Q_SIG               0x0020

#ifdef FEATURE_SAF
/* This signal is set when the SAF frame timer expires.
*/
#define VS_CMX_SAF_FRAME_SIG           0x0040
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
#define VS_CMX_SVG_FRAME_SIG          0x0100
#define CMX_SVG_MAX_STRING_LENGTH     256
#endif /* FEATURE_SVG */

/* CMX Command Queue Available Packets.
*/
#define CMX_NUM_PACKETS 60               /* # of Free Packets Allocated     */

typedef enum {

#ifdef FEATURE_AUDIO_FORMAT
  CMX_AF_CMD_AVSYNC_PLAYBACK, /* Play data whose format is specified 
                                 with ABORT type returned through callback */
  CMX_AF_CMD_PLAY_CODEC,      /* Play data whose format is specified    */
  CMX_AF_CMD_PLAY,            /* Determine data format and play         */
  CMX_AF_CMD_RINGER,          /* Midi Ringer command                    */
  CMX_AF_CMD_STOP,            /* Stop CMX playback command              */
  CMX_AF_CMD_REWIND,          /* Rewind CMX playback command            */
  CMX_AF_CMD_FFORWARD,        /* Fast forward CMX playback command      */
  CMX_AF_CMD_PAUSE,           /* Pause CMX playback command             */
  CMX_AF_CMD_SEEK_PAUSE,      /* Prepare to receive rewind/ff command   */
  CMX_AF_CMD_RESUME,          /* Resume CMX playback command            */
  CMX_AF_CMD_FLUSH,           /* Discard buffers and restart playing    */
  CMX_AF_CMD_TEMPO,           /* CMX playback tempo command             */
  CMX_AF_CMD_TUNE,            /* CMX playback tune command              */
  CMX_AF_CMD_PAN,             /* CMX playback stereo pan command        */

  CMX_AF_CMD_SEQ_STOP,        /* CMX stop sequence command              */
  CMX_AF_CMD_SEQ_PAUSE,       /* CMX pause sequence command             */
  CMX_AF_CMD_SEQ_RESUME,      /* CMX resume sequence command            */
  CMX_AF_CMD_SEQ_FFORWARD,    /* CMX fast-forward sequence command      */
  CMX_AF_CMD_SEQ_REWIND,      /* CMX rewind sequence command            */
  CMX_AF_CMD_SEQ_TEMPO,       /* CMX change sequence tempo command      */
  CMX_AF_CMD_SEQ_TUNE,        /* CMX change sequence tune command       */
  CMX_AF_CMD_SEQ_PAN,         /* CMX change sequence pan command        */
  CMX_AF_CMD_SEQ_VOL,         /* CMX change sequence volume command     */
               
  CMX_AF_CMD_GET_TIME,        /* Get total time of CMX file command     */
  CMX_AF_CMD_GET_TIME_ABORT,  /* Abort CMX get time command             */
  CMX_AF_CMD_GET_TIME_CB,     /* CMX get time callback command          */
  CMX_AF_CMD_GET_SPEC,        /* Get audio specifications command       */
  CMX_AF_CMD_GET_SPEC_CB,     /* Get audio specifications callback cmd  */
  CMX_AF_CMD_READ,            /* CMX read data command                  */
  CMX_AF_CMD_READ_VAR,        /* CMX read variable data command         */
  CMX_AF_CMD_GET_DLS,         /* CMX get DLS waveform command           */
  CMX_AF_CMD_READ_RSP,        /* CMX read response command              */
  CMX_AF_CMD_CB,              /* CMX sound server play callback command */
  CMX_AF_CMD_PLAY_DONE_CB,    /* CMX playback done command              */
  CMX_AF_CMD_SUPPORT,         /* CMX playback support command           */
  CMX_AF_CMD_AUDIO_SPEC,      /* CMX audio spec command                 */
  CMX_AF_CMD_PREPARSE,        /* CMX preparse command                   */

#ifdef FEATURE_WEBAUDIO
  CMX_AF_CMD_PIC_INIT,        /* CMX playback picture init command      */
  CMX_AF_CMD_PIC_DATA,        /* CMX playback picture data command      */
  CMX_AF_CMD_PIC_CTL,         /* CMX playback picture control command   */
#ifdef FEATURE_EFS
  CMX_AF_CMD_PIC_READ,        /* CMX playback picture read command      */
#endif /* FEATURE_EFS */
  CMX_AF_CMD_TEXT_CTL,        /* CMX playback text command              */
  CMX_AF_CMD_ANIM,            /* CMX playback anim control              */
  CMX_AF_CMD_ANIM_FRAME,      /* CMX playback anim Frame                */
  CMX_AF_CMD_ANIM_FRAME_ID,   /* CMX playback anim Frame ID             */
#ifdef FEATURE_SMAF
  CMX_AF_CMD_SMAF_GRAPH_INFO, /* SMAF_playback graphics track info      */
  CMX_AF_CMD_SMAF_LED_CTL,    /* SMAF playback LED control              */
  CMX_AF_CMD_SMAF_VIB_CTL,    /* SMAF playback VIB control              */
#endif /* FEATURE_SMAF */
  CMX_AF_CMD_MFI_LED_CTL,     /* MFi playback LED control               */
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
  CMX_AF_CMD_LED_CTL,         /* CMX playback LED control               */
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
  CMX_AF_CMD_BACKLIGHT_CTL,   /* CMX playback Backlight control         */
#endif
#ifdef FEATURE_AUDFMT_HYBRID_MODE 
  CMX_AF_CMD_SET_POLY_MODE,    /* CMX set poly mode command             */
  CMX_AF_CMD_GET_POLY_MODE,    /* CMX get poly mode command             */
  CMX_AF_CMD_POLY_MODE_CB,     /* CMX set poly mode cb command          */
  CMX_AF_CMD_GET_POLY_MODE_CB, /* CMX get poly mode cb command          */
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
#ifdef FEATURE_QCP
  CMX_QCP_GET_DATA_SIZE,      /* QCP get data size command              */
  CMX_QCP_GET_FRAMES,         /* QCP get frames command                 */
  CMX_QCP_GET_CNFG,           /* QCP get cnfg command                   */
  CMX_QCP_SET_CNFG,           /* QCP set cnfg command                   */
  CMX_QCP_GET_LABL,           /* QCP get label command                  */
  CMX_QCP_SET_LABL,           /* QCP set label command                  */
  CMX_QCP_GET_TEXT,           /* QCP get text command                   */
  CMX_QCP_SET_TEXT,           /* QCP set text command                   */
  CMX_QCP_FUNC_DONE,          /* QCP get/set function done command      */
  CMX_QCP_FUNC_FAIL,          /* QCP get/set function failed command    */
  CMX_QCP_CMD_REC,            /* QCP record command                     */
  CMX_QCP_CMD_REC_STOP,       /* QCP record stop command                */
  CMX_QCP_CMD_REC_AUTO_STOP,  /* QCP record auto stop command           */
  CMX_QCP_CMD_REC_PAUSE,      /* QCP record pause command               */
  CMX_QCP_CMD_REC_RESUME,     /* QCP record resume command              */
  CMX_QCP_CMD_REC_DATA,       /* QCP record data command                */
  CMX_QCP_CMD_REC_GET_BUF,    /* QCP record get buffer command          */
  CMX_QCP_CMD_REC_DONE,       /* QCP record done command                */
  CMX_QCP_CMD_REC_TIME,       /* QCP record time update                 */
  CMX_QCP_CMD_REC_PAUSE_S,    /* QCP record paused status               */
  CMX_QCP_CMD_REC_RESUME_S,   /* QCP record resumed status              */
  CMX_QCP_CMD_REC_EFS_DONE,   /* QCP recording ran out of space         */
  CMX_QCP_CMD_REC_ERROR,      /* QCP recording error encountered        */
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
  CMX_PCM_CMD_REC,            /* PCM record command                     */
  CMX_PCM_CMD_REC_STOP,       /* PCM record stop command                */
  CMX_PCM_CMD_REC_PAUSE,      /* PCM record pause command               */
  CMX_PCM_CMD_REC_RESUME,     /* PCM record resume command              */
  CMX_PCM_CMD_REC_STATUS,     /* PCM record status callback command     */
  CMX_PCM_CMD_REC_DONE,       /* PCM record finished command            */
  CMX_PCM_CMD_REC_HEADER_DATA,/* PCM record header data command         */
  CMX_PCM_CMD_REC_DATA,       /* PCM record data command                */
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
  CMX_MM_CMD_REC,            /* MM record command                     */
  CMX_MM_CMD_REC_STOP,       /* MM record stop command                */
  CMX_MM_CMD_REC_PAUSE,      /* MM record pause command               */
  CMX_MM_CMD_REC_RESUME,     /* MM record resume command              */
  CMX_MM_CMD_REC_STATUS,     /* MM record status callback command     */
  CMX_MM_CMD_REC_DONE,       /* MM record finished command            */
  CMX_MM_CMD_REC_HEADER_DATA,/* MM record header data command         */
  CMX_MM_CMD_REC_DATA,       /* MM record data command                */
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
  CMX_CMD_CLIENT_BUF_REQ,     /* CMX buffer request command             */
  CMX_CMD_CLIENT_BUF_REQ_CB,  /* CMX client buffer request callback cmd */
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

#ifdef FEATURE_CMX_IMAGE
  CMX_IMAGE_CMD_CONTINUE,     /* Continue CMX decode image command      */
  CMX_IMAGE_CMD_READ,         /* Read image data command                */
  CMX_IMAGE_CMD_DO_CB,        /* Do image callback command              */
#ifdef FEATURE_WEBAUDIO
  CMX_IMAGE_CMD_BUF_REQ,      /* Image buffer request command           */
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_PNG_DECODER
  CMX_PNG_CMD_DECODE,         /* CMX decode PNG image command           */
  CMX_PNG_CMD_ABORT,          /* Abort CMX decode PNG image command     */
  CMX_PNG_CMD_SPEC,           /* Get PNG image specifications command   */
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
  CMX_JPEG_CMD_DECODE,        /* CMX decode JPEG image command          */
  CMX_JPEG_CMD_SPEC,          /* Get JPEG image specifications command  */
  CMX_JPEG_CMD_ABORT_DECODE,  /* Abort JPEG decode image command        */
#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_SAF
  CMX_SAF_CMD_DECODE,         /* CMX decode SAF image command           */
  CMX_SAF_CMD_DECODE_CONT,    /* CMX continue decode SAF image command  */
  CMX_SAF_CMD_ABORT,          /* Abort CMX decode SAF image command     */
  CMX_SAF_CMD_FRAME_ID,       /* SAF frame ID command                   */
  CMX_SAF_CMD_PAUSE,          /* Pause CMX decode SAF image command     */
  CMX_SAF_CMD_RESUME,         /* Resume CMX decode SAF image command    */
  CMX_SAF_CMD_INFO,           /* Get SAF image specifications command   */
  CMX_SAF_CMD_INFO_CONT,      /* Continue get SAF spec command          */
  CMX_SAF_CMD_DO_FRAME_CB,    /* Issue frame command callback command   */
  CMX_SAF_CMD_FRAME_CONT,     /* Continue SAF frame command             */
  CMX_SAF_CMD_FRAME_DONE,     /* SAF frame done, return to client       */
#endif /* FEATURE_SAF */

#ifdef FEATURE_MIDI_OUT
  CMX_MIDI_OUT_OPEN,            /* Open MIDI output device                */
  CMX_MIDI_OUT_CLOSE,           /* Close MIDI output device               */
  CMX_MIDI_OUT_MSG,             /* Issue MIDI messages to MIDI device     */
#ifdef FEATURE_MIDI_OUT_QCP
  CMX_MIDI_OUT_QCP,             /* Play QCP file to MIDI output device    */
  CMX_MIDI_OUT_ADEC_START_CHAN, /* Start ADEC file to MIDI output device  */
  CMX_MIDI_OUT_QCP_STOP,        /* Stop QCP file to MIDI output device    */
  CMX_MIDI_OUT_QCP_PAUSE,       /* Pauses QCP files MIDI output device    */
  CMX_MIDI_OUT_QCP_RESUME,      /* Resumes QCP files to MIDI output dev   */
  CMX_MIDI_OUT_QCP_PAUSE_CHAN,  /* Pauses QCP file MIDI output device     */
  CMX_MIDI_OUT_QCP_RESUME_CHAN, /* Resumes QCP file to MIDI output dev    */
  CMX_MIDI_OUT_QCP_PAN,         /* Modify QCP panning playing to MIDI dev */
  CMX_MIDI_OUT_QCP_VOLUME,      /* Modify QCP volume playing to MIDI dev  */
  CMX_MIDI_OUT_QCP_RESET,       /* Reset QCP file                         */
#ifdef FEATURE_WAVE_PB
  CMX_MIDI_OUT_QCP_RESERVE_CHANNEL,
                              /* Reserve MIDI_OUT_QCP channel for ADPCM */
#endif /* FEATURE_WAVE_PB */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_QAUDIOFX
  CMX_CMD_3D_POS_ENABLE,            /* Enable 3D positional command      */
  CMX_CMD_3D_POS_RESET,             /* Reset 3D environment              */
  CMX_CMD_3D_POS_SET_LSNR_POS,      /* Set listener position command     */
  CMX_CMD_3D_POS_SET_LSNR_ORIENT,   /* Set listener orientation command  */
  CMX_CMD_3D_POS_SET_LSNR_VEL,      /* Set listener velocity command     */
  CMX_CMD_3D_REVERB_ENABLE,         /* Enable Environment Reverb command */
  CMX_CMD_3D_REVERB_DISABLE,        /* Disable Reverb command            */
  CMX_CMD_3D_REVERB_SET_PRESET,     /* Set Reverb Decay command    */
  CMX_CMD_3D_REVERB_SET_LEVEL,      /* Set Reverb Decay command    */
  CMX_CMD_3D_REVERB_SET_TIME,       /* Set Reverb Decay command    */
  CMX_CMD_3D_REVERB_SET_RATIO,  /* Set Reverb ratio command    */
  CMX_CMD_3D_POS_SRC_ENABLE,        /* Enable/disable source 3D effect   */
  CMX_CMD_3D_POS_SRC_RESET,         /* Reset source 3D                   */
  CMX_CMD_3D_POS_SET_SRC_VOL,       /* Set source volume command         */
  CMX_CMD_3D_POS_SET_SRC_VOL_ATTEN, /* Set source volume attenuation cmd */
  CMX_CMD_3D_POS_SET_SRC_POS,       /* Set source position command       */
  CMX_CMD_3D_POS_SET_SRC_VEL,       /* Set source velocity command       */
  CMX_CMD_3D_REVERB_SET_SRC_VOL,    /* Set source Reverb volume command */
  CMX_CMD_3D_POS_QUERY_ENV,         /* Query environment variable command*/
  CMX_CMD_3D_POS_QUERY_SRC,         /* Query source variable command     */
  CMX_CMD_3D_POS_DO_CB,             /* 3D positional command callback    */
  CMX_CMD_3D_POS_DO_QUERY_CB,       /* 3D positional query callback      */
#endif /* FEATURE_QAUDIOFX */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  CMX_ADEC_READ,              /* Do QCP data read                       */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_AUDIO_POSTCARD
  CMX_CMD_CREATE_AUDIOP,      /* Create audio postcard command          */
  CMX_CMD_COMPLETE_AUDIOP,    /* Complete audio postcard creation cmd   */
#endif /* FEATURE_AUDIO_POSTCARD */

#ifdef FEATURE_DLS
  CMX_AF_CMD_LOAD_DLS,        /* Load DLS file command                  */
  CMX_AF_CMD_UNLOAD_DLS,      /* Unload DLS command                     */
  CMX_AF_CMD_DLS_CB,          /* DLS callback command                   */
#endif /* FEATURE_DLS */

#ifdef FEATURE_XMF
  CMX_XMF_DATA_CB,            /* Data request callback for XMF          */
  CMX_XMF_CONTINUE_PROCESS,   /* Continue processing XMF command        */
#endif /* FEATURE_XMF */

#ifdef FEATURE_SVG
  CMX_SVG_CMD_DECODE,         /* CMX decode SVG image command           */
  CMX_SVG_CMD_ABORT,          /* Abort CMX decode SVG image command     */
  CMX_SVG_CMD_FRAME,          /* SVG frame ID command                   */
  CMX_SVG_CMD_SEEK,           /* SVG seek command                       */
  CMX_SVG_CMD_PAUSE,          /* Pause CMX decode SVG image command     */
  CMX_SVG_CMD_RESUME,         /* Resume CMX decode SVG image command    */
  CMX_SVG_CMD_REWIND,         /* Rewinds CMX decode SVG image command   */
  CMX_SVG_CMD_FFORWARD,       /* Fast-forwards CMX decode SVG command   */
  CMX_SVG_CMD_INFO,           /* Get SVG image specifications command   */
  CMX_SVG_CMD_START_PLAY,     /* SVG frame done, return to client       */
  CMX_SVG_CMD_RESET,          /* Reset CMX decode SVG image command     */
  CMX_SVG_CMD_EVENT,          /* SVG CMX Event processing               */
  CMX_SVG_CMD_REQ,            /* SVG external file open request         */
  CMX_SVG_CMD_REQ_DATA,       /* SVG external file read request         */
  CMX_SVG_CMD_REQ_CB,         /* SVG external file read callback        */
  CMX_SVG_CMD_SET_VIEWPORT,   /* sets the Viewport for SVG decoder      */
  CMX_SVG_CMD_FREE,           /* Frees an SVG session                   */
#ifdef FEATURE_SVGDOM
  CMX_SVG_CMD_DOM,            /* SVG DOM call                           */
#endif //FEATURE_SVGDOM
#ifdef FEATURE_WEBAUDIO
  CMX_CMD_SVG_FRAME_BUFFER,   /* Processes a data buffer for SVG frames */
  CMX_CMD_SVG_FRAME_DONE,     /* SVG frame done command                 */
#endif /* FEATURE_WEBAUDIO */
#endif /*FEATURE_SVG */
#ifdef FEATURE_AUDIO_PCR_SUPPORT
  CMX_AF_CMD_ADD_SAMPLE,      /* Add one frame sample, for QTV clock sync  */
  CMX_AF_CMD_DROP_SAMPLE,     /* Drop one frame sample, for QTV clock sync */
#endif /* FEATURE_AUDIO_PCR_SUPPORT */
  CMX_AF_CMD_INFO_UPDATE,       /* Codec info update callback                */
  CMX_AF_CMD_CHANNEL_DUAL_MONO, /* Dual Mono Channel Mode command            */
  CMX_AF_CMD_SBR_MODE,          /* SBR Mode command                          */
  CMX_CMD_INVALID = 0xFFFFFFFF  /* (Leave Last) Invalid Command - Causes Error */
} cmx_cmd_type;

typedef union {
  uint8              *buf;      /* Pointer to CMX file in RAM/ROM         */
#ifdef FEATURE_EFS
  fs_handle_type     file;      /* EFS file handle of CMX file            */
#endif
  cmx_client_req_func_ptr_type
                     data_fn;   /* Data access function for data server   */
} cmx_af_access_type;

/* CMX uses this type to retain information on the type of CMX file that 
** is currently being processed.
*/
typedef struct {
  cmx_source_type      source;    /* Source of the CMX file                 */
  uint32               cmx_len;
  cmx_client_data_enum_type 
                       client_cfg;/* Configuration for data thru data servr */
  cmx_af_access_type   handle;    /* Access for the CMX file                */
  cmx_af_access_type   support;   /* Access for playback support functions, */
} cmx_af_source_info_type;

#ifdef FEATURE_AUDIO_FORMAT
#define CMX_MIN_BUF_SIZE  156

/* Structure of CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf01_type;

/* Structure of 2*CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[2*CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf02_type;

/* Structure of 4*CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[4*CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf04_type;

/* Structure of 8*CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[8*CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf08_type;

/* Structure of 16*CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[16*CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf16_type;

/* Structure of 32*CMX_MIN_BUF_SIZE byte buffers */
PACKED typedef struct {
  uint8 buf[32*CMX_MIN_BUF_SIZE];
} cmx_audfmt_buf32_type;

/* Largest buffer size for use with midi type files */
#define CMX_AF_MAX_BUFFER_SIZE   (CMX_MIN_BUF_SIZE*32)

/* Union of the various size buffers into a 3200 byte structure. */
PACKED typedef union {
  cmx_audfmt_buf32_type buf32[2];   /* 2  - 1600 byte buffers  */
  cmx_audfmt_buf16_type buf16[4];   /* 4  - 800  byte buffers  */
  cmx_audfmt_buf08_type buf08[8];   /* 8  - 400  byte buffers  */
  cmx_audfmt_buf04_type buf04[16];  /* 16 - 200  byte buffers  */
  cmx_audfmt_buf02_type buf02[32];  /* 32 - 100  byte buffers  */
  cmx_audfmt_buf01_type buf01[64];  /* 64 - 50   byte buffers  */
} cmx_audfmt_var_buf_type;

/* Enumerated type for each type/size of buffer supported */
typedef enum {
  CMX_AUDFMT_BUF_32,      /* 1600 byte buffer          */
  CMX_AUDFMT_BUF_16,      /* 800  byte buffer          */
  CMX_AUDFMT_BUF_08,      /* 400  byte buffer          */
  CMX_AUDFMT_BUF_04,      /* 200  byte buffer          */
  CMX_AUDFMT_BUF_02,      /* 100  byte buffer          */
  CMX_AUDFMT_BUF_01,      /* 50   byte buffer          */
  CMX_AUDFMT_BUF_INVALID  /* Invalid buffer type/size  */
} cmx_audfmt_buf_enum_type;

typedef struct {
  cmx_audfmt_buf_enum_type buf_type;         /* Buffer size to use           */
  uint32                   in_use_bit_mask;  /* Bit mask of buffers that are */
  uint32                   in_use_bit_mask2; /* in use (maximum 64 buffers)  */
  cmx_audfmt_var_buf_type  buf;              /* Buffer structure             */
} cmx_audfmt_buf_struct;
#endif /* FEATURE_AUDIO_FORMAT */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
/* This structure contains information on buffer requests from clients.
*/
typedef struct {
  uint8 *buffer;       /* Buffer received from client              */
  void  *pending_cmd;  /* Command to issue when buffer is received */
  void  *handle;       /* Handle of originating buffer request     */
} cmx_client_buf_type;
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

#ifdef FEATURE_SVG
/* This enum contains states of SVG decoder 
*/
typedef enum{
  CMX_SVG_STATUS_PREPARING,   /* Loading content                    */
  CMX_SVG_STATUS_PLAYING,     /* Playing animations, between frames */
  CMX_SVG_STATUS_RENDERING,   /* Decoder is Rendering a frame       */
  CMX_SVG_STATUS_PENDING,     /* Frame Command pending              */
  CMX_SVG_STATUS_PAUSED,      /* Decoder is Paused                  */
  CMX_SVG_STATUS_STOPPED,     /* Decoder is stopped                 */
  CMX_SVG_STATUS_INVALID
}cmx_svg_status_type;

/* This structure contains all the info about SVG file playback */
typedef struct{
  uint32     width;                                 /* Display width          */
  uint32     height;                                /* Display width          */
  uint16     dpi;                                   /* Pixels per inch        */
  char      directory[FS_FILENAME_MAX_LENGTH_P];   /* External requests      */
  uint32     delay;                                /* delay between frames    */
  uint32     bk_color;                             /* Background color        */
  uint32     tr_color;                             /* Transparent color       */
  uint8*     mem_buffer;                           /* Memory Buffer           */
  void*      svg_handle;                          /* SVG handle               */
  boolean    transparent: 1;                          /* Transparency feature    */
  cmx_color_scheme_enum_type color_scheme: 3;             /* Color Scheme             */
  cmx_svg_decode_mode_enum_type   cmx_svg_decode_mode: 2; /* Mode                */
  boolean    progressive: 1;                       /* Progressive rendering allowed */
  boolean    auto_stop: 1;                       /* Progressive rendering allowed */
  cmx_svg_status_type status;                       /* Player status          */

  uint8*     frame_buf;
  uint32     frame_bufsize;
  uint32     frame_time;                          /* Time info for next frame */
  boolean    frame_buf_busy;                      /* true = Frame buffer not consumed by client yet */

#ifdef FEATURE_SVGDOM
  cmx_svgdom_handle_event_type event_handler;     /* Client Event Handler */
  void*                        event_handler_data; /* Data for event Handler */
#endif /* FEATURE_SVGDOM */
  rex_timer_type frame_timer;

  rex_timer_type actual_time;

} cmx_svg_data_type;

/* This strut contains info about External file requests made by SVG
*/
typedef struct{
  uint8 type;           /* 0- Req from file, 1-Non-req from file, FF- client */
  void* handle;                                   /* SVG handle               */
  svg_cmx_request_cb_type cb_func;
  void* cb_data;
  char filename[FS_FILENAME_MAX_LENGTH_P];        /* External file name */
  svg_cmx_get_request_cb_type next_cb;

  union{
    struct{
      uint8** buf_ptr;
      uint32* num_bytes;
    }client;            /* URL processed by client                           */
#ifdef FEATURE_EFS
    struct{
      fs_handle_type fs_handle;
      uint32 file_size;
    }efs;               /* External file in EFS                              */
#endif
  }data;
} cmx_svg_req_type;
#endif /* FEATURE_SVG */

/* This mask type is used to indicate various status for cmx_af_handle_type
*/
typedef enum {
  CMX_AF_STATUS_MASK_DISABLE_PENDING = 0x0001, /* Indicates file is disabling */
  CMX_AF_STATUS_MASK_USE_CLIENT_BUF  = 0x0002, /* Flag for client buffers     */
  CMX_AF_STATUS_MASK_WARNING_SENT    = 0x0004, /* Space warning sent flag     */
  CMX_AF_STATUS_MASK_DLS_LOADED      = 0x0008, /* DLS file loaded for handle  */
  CMX_AF_STATUS_MASK_XMF_RECOGNIZED  = 0x0010, /* File is XMF format          */
  CMX_AF_STATUS_MASK_TX_RECORDED     = 0x0020, /* Tx audio content recorded   */
  CMX_AF_STATUS_MASK_RX_RECORDED     = 0x0040, /* Rx audio content recorded   */
  CMX_AF_STATUS_MASK_PLAY_ABORTED    = 0x0080, /* Stop issued by client       */
  CMX_AF_STATUS_MASK_DISABLED        = 0x0100, /* Disabled the handle but not cleanedup yet  */
  CMX_AF_STATUS_MASK_XMF_LOADING_DLS = 0x0200  /* XMF playbackin process of   */
                                               /* loading DLS instruments     */
} cmx_af_status_mask_enum_type;

/* CMX descriptor type that retains information on the service currently
** being performed by CMX, and the necessary client information to do 
** callbacks.
*/
typedef struct {
  uint32                    status_mask;      /* Status mask for this handle */
  cmx_cmd_type              cmd;              /* Service being performed     */
#if defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC)
  cmx_pcm_rec_link_type     link;             /* Direction of PCM recording  */
#endif /* FEATURE_PCM_REC || FEATURE_MM_REC */
#if defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC) || defined(FEATURE_QCP)
#ifdef FEATURE_EFS
  cmx_handle_type           *rec_tx_handle;   /* Tx dest. storage handle     */
  cmx_handle_type           *rec_rx_handle;   /* Rx dest. storage handle     */
#endif /* FEATURE_EFS */
#endif /* FEATURE_PCM_REC || FEATURE_MM_REC || FEATURE_QCP */
#ifdef FEATURE_MM_REC
  cmx_mm_rec_format_type    format;           /* MultiMedia record file type */
#endif /* FEATURE_MM_REC */
  cmx_af_source_info_type   source_info;      /* Source of CMX file          */
#ifdef FEATURE_AUDIO_FORMAT
  cmx_af_callback_enum_type cb_type;          /* Callback mode               */
  cmx_audfmt_buf_struct     *buffer;          /* Buffers for this playback   */
  union {
    snd_audfmt_sound_type   audfmt_sound;     /* Sound structs for playback  */
    snd_format_sound_type   format_sound;     /* Sound structs for playback  */
  } sound;
#endif
  union {
#ifdef FEATURE_AUDIO_FORMAT
    cmx_audfmt_play_cb_func_ptr_type play;    /* Playback callback function  */
    cmx_audfmt_time_cb_func_ptr_type time;    /* Get time callback function  */
    cmx_audfmt_spec_cb_func_ptr_type spec;    /* Get Audio Spec callback func*/
    cmx_cb_func_ptr_type             ringer;  /* Ringer callback function    */
    cmx_cb_func_ptr_type             play_codec; /* Codec play callback func */
    cmx_audfmt_play_cb_func_ptr_type av_sync_playback;
                                         /* av sync Codec play callback func */
#endif
#ifdef FEATURE_CMX_IMAGE
    cmx_image_cb_func_ptr_type       image;   /* PNG decoder/SAF callback fn */
#endif
#ifdef FEATURE_MIDI_OUT_QCP
    cmx_midi_out_cb_func_ptr_type    midi_out_qcp; /* MIDI out QCP cb func   */
#endif /* FEATURE_MIDI_OUT_QCP */
    cmx_cb_func_ptr_type             base;    /* Base callback function      */
  } cb_func;
#ifdef FEATURE_WEBAUDIO
  uint32                    content_mask;     /* Mask of WA cmds issued      */
  cmx_client_buf_type       text_buffer;      /* Text buffer for text data   */
  cmx_client_buf_type       pict_buffer;      /* Buffer for image cmds       */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_SAF
  cmx_client_buf_type       saf_buffer;       /* Frame buffer for SAF cmds   */
  uint8                     *saf_obj_buf;     /* Buffer for anim objects     */
  uint32                    saf_obj_buf_size; /* Object buffer size          */
#endif /* FEATURE_SAF */
#if defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC)
  uint8                     *buf1;            /* PCM/MM recording data buffer*/
  uint8                     *buf2;            /* PCM/MM recording data buffer*/
#endif /* FEATURE_PCM_REC || FEATURE_MM_REC */
#ifdef FEATURE_AUDIO_POSTCARD
  cmx_handle_type           *file_desc;       /* Output file descriptor      */ 
  uint32                    cur_length;       /* Current length of output    */
#endif /* FEATURE_AUDIO_POSTCARD */
#ifdef FEATURE_XMF
  uint8                     *data_buf;        /* XMF data buffer pointer     */
#endif /* FEATURE_XMF */
  void                      *gen_handle;      /* multipurpose generic handle */
#ifdef FEATURE_SVG
  cmx_svg_data_type         *svg_data;        /* SVG data for file playback  */
  cmx_svg_req_type          svg_req;          /* SVG Request info            */
  cmx_client_buf_type       svg_buffer;       /* Frame buffer for SVG cmds   */
#endif /* FEATURE_SVG */
#ifdef FEATURE_AUDFMT_HYBRID_MODE
  cmx_audfmt_poly_mode_type *client_poly_ptr; /* Client's poly mode ptr      */
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  cmx_af_codec_info_update_cb_ptr_type codec_update_cb; /* Codec info update 
                                                            callback func    */
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */
  void                      *client_data;     /* Client data                 */
} cmx_af_handle_type;

/* Header for CMX commands
*/
typedef struct
{
  q_link_type         link;            /* Command queue link               */
  cmx_cmd_type        cmd;             /* CMX command to perform           */

} cmx_hdr_type;

/* Base packet type for CMX commands that use standard CB function and have
** no parameters.
*/
typedef struct
{
  cmx_hdr_type           hdr;          /* Command Header            */
  cmx_cb_func_ptr_type   cb_func;      /* Callback function pointer */
  void                   *client_data; /* Client data               */
} cmx_base_packet_type;

#ifdef FEATURE_AUDIO_FORMAT
/* CMX get/read data command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;           /* Command Header                     */
  snd_af_data_cmd_enum  cmd;           /* Type of read command               */
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service  */
  uint32                position;      /* Position to get data, offset from  */
                                       /* beginning of CMX file              */
  snd_af_read_cb_func_type
                        cb_func;       /* Callback function pointer          */
  void                  *client_data;  /* Client data                        */
  uint32                buffer_size;   /* Buffer size for read request       */
  uint32                offset;        /* Offset within buffer to start data */
} cmx_af_read_packet_type;

/* CMX get DLS command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  uint32                length;        /* Length of DLS waveform in bytes   */
  uint32                position;      /* Position of DLS, offset from      */
                                       /* beginning of CMX file             */
  snd_af_get_dls_cb_func_type
                        cb_func;       /* Callback function pointer         */
  void                  *client_data;  /* Client data                       */
} cmx_af_get_dls_packet_type;

/* CMX EFS read response command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;           /* Command Header                */
  uint32                position;      /* Position that was read        */
  uint32                length;        /* Length that was read          */
  uint8                 *buff;         /* Buffer containing data        */
  snd_af_read_cb_func_type
                        cb_func;       /* Sound callback function       */
  void                  *client_data;  /* Client data                   */
} cmx_af_read_rsp_packet_type;

/* CMX play command for user specified data format
*/
/* Define internal structure to accommodate original and extended 
   playbac codec API */
typedef union {
  cmx_af_codec_type      codec_type;   /* Generic codec information      */
  cmx_codec_spec_type    spec_original;
  cmx_codec_spec_ex_type spec_extended;
} cmx_internal_codec_spec_type;

typedef union {
  cmx_av_sync_type       av_sync_original;
  cmx_av_sync_ex_type    av_sync_extended;
} cmx_internal_av_sync_type;

typedef enum {
  CMX_INTERFACE_ORIGINAL,
  CMX_INTERFACE_EXTENDED
} cmx_interface_enum_type;

typedef struct
{ 
  cmx_hdr_type                 hdr;             /* Command Header            */
  cmx_handle_type              *handle;         /* Descriptor of CMX file    */
  cmx_internal_codec_spec_type codec_spec;      /* File format details       */
  cmx_internal_av_sync_type    av_sync;         /* audio video sync details  */           
  cmx_cb_func_ptr_type         cb_func;         /* Callback function pointer */
  cmx_audfmt_play_cb_func_ptr_type av_sync_cb_func; 
                                        /* av sync callback Function pointer */ 
  void                         *client_data;    /* Client data               */
  cmx_interface_enum_type      interface_src;
} cmx_af_play_codec_packet_type;


/* This structure contains information on playback parameters
*/
typedef struct {
  boolean                     multiseq_flag;  /* Multisequencer flag    */
  cmx_af_callback_enum_type   cb_type;        /* Client CB state        */
  cmx_audfmt_start_enum_type  start_position; /* Start mode of file     */
  cmx_audfmt_repeat_enum_type repeat_state;   /* Repeat state of play   */
  uint8                       repeat_count;   /* Repeat count           */
  uint16                      repeat_ms;      /* Time between repeats   */
} cmx_af_play_param_type;

/* CMX play command packet
*/
typedef struct
{
  cmx_hdr_type                     hdr;          /* Command Header         */
  cmx_handle_type                  *handle;      /* Descriptor of CMX file */
  cmx_af_play_param_type           param;        /* Playback paramaters    */
  cmx_audfmt_play_cb_func_ptr_type cb_func;      /* Callback function      */
  void                             *client_data; /* Client data            */
} cmx_af_play_packet_type;

/* CMX ringer command packet
*/
typedef struct
{
  cmx_hdr_type           hdr;            /* Command Header                  */
  cmx_handle_type        *handle;        /* Descriptor of CMX file          */
  uint16                 repeat_timer;   /* Time (in ms) of silence between */
                                         /* repeating of CMX file (0 for no */
                                         /* repeating).                     */
  cmx_audfmt_start_enum_type
                         start_position; /* Start position of audio file    */
  cmx_cb_func_ptr_type   cb_func;        /* Callback function pointer       */
  void                   *client_data;   /* Client data                     */
} cmx_af_ringer_packet_type;

/* CMX get time command packet
*/
typedef struct
{
  cmx_hdr_type           hdr;            /* Command Header            */
  cmx_handle_type        *handle;        /* Descriptor of CMX file    */
  cmx_audfmt_time_cb_func_ptr_type
                         cb_func;        /* Callback function pointer */
  void                   *client_data;   /* Client data               */
} cmx_af_time_packet_type;

/* CMX rewind/fast-forward command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;            /* Command Header                 */
  uint32                time_ms;        /* Time to rewind/ff CMX playback */
  cmx_cb_func_ptr_type  cb_func;        /* Callback function pointer      */
  void                  *client_data;   /* Client data                    */
} cmx_af_play_ctl_packet_type;

/* CMX sequence command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;            /* Command header                 */
  void                  *sequence;      /* Sequence handle                */
  uint32                num;            /* Parameter of command           */
  cmx_cb_func_ptr_type  cb_func;        /* Callback function pointer      */
  void                  *client_data;   /* Client data                    */
} cmx_af_sequence_packet_type;

/* Sound server callback command packet
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header               */
  void                  *client_data;  /* Client data                  */
  snd_status_type       status;        /* Sound status                 */
} cmx_af_cb_packet_type;

/* Sound command/status command packet, used for playback support functions
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                    */
  snd_af_data_cmd_enum  command;       /* Command/status from sound server  */
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  uint32                position;      /* Generic position                  */
  uint32                number;        /* Generic number                    */
} cmx_af_support_packet_type;

/* Character set command packet 
*/
typedef struct {
  cmx_hdr_type           hdr;         /* Command Header                   */
  cmx_af_handle_type     *handle;     /* Descriptor of current service    */
  cmx_af_file_enum_type  file_type;   /* Format of audio file             */
  cmx_af_audio_spec_type *audio_spec; /* Character set to be used         */
} cmx_af_aud_spec_packet_type;

/* Audio format get time callback command packet
*/
typedef struct {
  cmx_hdr_type             hdr;         /* Command Header                   */
  cmx_af_handle_type       *handle;     /* Descriptor of current service    */
  snd_status_type          status;      /* Sound status                     */
  uint32                   time_ms;     /* Elapsed time in milliseconds     */
} cmx_af_get_time_cb_packet_type;

/* Audio format get audio file format callback command packet
*/
typedef struct {
  cmx_hdr_type             hdr;         /* Command Header                   */
  cmx_af_handle_type       *handle;     /* Descriptor of current service    */
  snd_status_type          status;      /* Sound status                     */
} cmx_af_get_spec_cb_packet_type;

/* Audio format preparse command packet
*/
typedef struct {
  cmx_hdr_type                  hdr;           /* Command Header            */
  void                          *client_data;  /* Client data               */
  snd_af_preparse_func_type     preparse_func; /* Preparse function pointer */
} cmx_af_preparse_packet_type;


typedef struct {
  cmx_hdr_type                     hdr;          /* Command Header          */
  cmx_handle_type                  *handle;      /* Descriptor of CMX file  */
  cmx_audfmt_spec_cb_func_ptr_type cb_func;      /* Client callback func    */
  void                             *client_data; /* Client callback data    */
} cmx_af_get_spec_packet_type;
  
#ifdef FEATURE_WEBAUDIO
/* New picture inititalization command packet, used for playback images
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  cmx_af_pic_speed_enum_type
                        draw_speed;    /* Draw speed                        */
  cmx_af_pos_enum_type  xpos_type;     /* Position of image in the X plane  */
  cmx_af_pos_enum_type  ypos_type;     /* Position of image in the Y plane  */
  uint8                 xpos_percent;  /* % offset of image in X plane      */
  uint8                 ypos_percent;  /* % offset of image in Y plane      */
} cmx_af_pic_init_packet_type;

/* Picture data command packet, used for playback images
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_af_pic_enum_type  pic_type;      /* Format of image                   */
  uint8                 *buffer;       /* Data buffer containing image data */
  uint32                buf_len;       /* Size of data buffer               */
  cmx_af_handle_type    *handle;       /* Descriptor of current service     */
  uint32                position;      /* Position in file to find rest of  */
                                       /* image data not in data buffer     */
  uint32                remain_bytes;  /* Size of remaining image data      */
} cmx_af_pic_data_packet_type;

/* Picture control command packet, used for playback images
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                   */
  cmx_af_pic_status_enum_type
                        command;       /* Type of picture control          */
  cmx_af_handle_type    *handle;       /* Descriptor of current service    */
  cmx_af_pic_speed_enum_type
                        speed;         /* Speed of clear, if clear control */
} cmx_af_pic_ctl_packet_type;

/* Text control command packet, used for playback text
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                   */
  cmx_af_text_ctl_enum_type
                        command;       /* Type of text command             */
  uint8                 *buffer;       /* Data buffer containing text data */
  uint32                buf_len;       /* Size of data buffer              */
  cmx_af_handle_type    *handle;       /* Descriptor of current service    */       
  uint32                position;      /* Position in file to find rest of */
                                       /* text data not in data buffer     */
  uint32                remain_bytes;  /* Size of remaining text data      */
  cmx_af_pos_enum_type  xpos_type;     /* Position of image in the X plane */
  cmx_af_pos_enum_type  ypos_type;     /* Position of image in the Y plane */
} cmx_af_text_ctl_packet_type;

/* Animation data command packet, used for playback animation
*/
typedef struct {
  cmx_hdr_type           hdr;           /* Command Header                    */
  cmx_af_anim_enum_type  type;
  uint8                  *buffer;       /* Data buffer containing image data */
  uint32                 buf_len;       /* Size of data buffer               */
  cmx_af_handle_type     *handle;       /* Descriptor of current service     */
  uint32                 position;      /* Position in file to find rest of  */
                                        /* image data not in data buffer     */
  uint32                 remain_bytes;  /* Size of remaining image data      */
  cmx_af_pos_struct_type frame_pos;     /* Position of animation frame       */
} cmx_af_anim_data_packet_type;

#ifdef FEATURE_SMAF
/* SMAF Graphics track information command packet, used for playback animation
*/
typedef struct {
  cmx_hdr_type             hdr;           /* Command Header                 */
  cmx_af_handle_type       *handle;       /* Descriptor of current services */
  cmx_smaf_graph_info_type info;          /* Graphics track information     */
} cmx_smaf_graph_info_packet_type;

/* SMAF LED/Vibrator control command packet, used for playback LED/Vibratior 
** events
*/
typedef struct {
  cmx_hdr_type              hdr;          /* Command Header                 */
  cmx_af_handle_type        *handle;      /* Descriptor of current services */
  cmx_smaf_led_ctl_status_enum_type  led_ctl; /* LED state                      */
  cmx_smaf_vib_ctl_status_enum_type  vib_ctl; /* VIB state                      */
} cmx_smaf_led_vib_ctl_packet_type;
#endif /* FEATURE_SMAF */

/* MFi LED control command packet
*/
typedef struct {
  cmx_hdr_type        hdr;        /* Command Header                   */
  cmx_af_handle_type  *handle;    /* Descriptor of current service    */
  uint32              mask;       /* LED state                        */
} cmx_mfi_led_ctl_packet_type;
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
/* LED control command packet
*/
typedef struct {
  cmx_hdr_type             hdr;        /* Command Header                   */
  cmx_af_handle_type       *handle;    /* Descriptor of current service    */       
  cmx_af_led_ctl_enum_type led_ctl;    /* LED state                        */
} cmx_af_led_ctl_packet_type;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
/* Backlight control command packet
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                   */
  cmx_af_handle_type    *handle;       /* Descriptor of current service    */       
  boolean               backlight_ctl; /* Backlight state TRUE=ON          */
} cmx_af_blight_ctl_packet_type;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_QCP
#define MIN_AUTO_STOP_VAL 1000 /* Min value for auto stop silence time (ms) */

/* QCP general function get/set command packet
*/
typedef struct {
  cmx_hdr_type         hdr;             /* Command Header                */
  cmx_handle_type      *handle;         /* Descriptor of CMX file        */
  uint8                *buffer;         /* Buffer for data               */
  uint32               length;          /* Length of data to write       */
  uint32               *num_bytes;      /* Parameter for size gets       */
  cmx_cb_func_ptr_type cb_func;         /* Callback function pointer     */
  void                 *client_data;    /* Client data                   */
} cmx_qcp_func_packet_type;

/* QCP function get/set done command packet
*/
typedef struct {
  cmx_hdr_type         hdr;             /* Command Header                */
  cmx_af_handle_type   *handle;         /* Descriptor of CMX file        */
  uint8                *buffer;         /* QCP buffer read               */
  uint32               num_bytes;       /* QCP length read               */
} cmx_qcp_func_cb_packet_type;

/* QCP record packet type, used to start recording a QCP file
*/
typedef struct {
  cmx_hdr_type             hdr;          /* Command Header                 */
  cmx_handle_type          *tx_handle;   /* Handle for Tx data             */
  cmx_handle_type          *rx_handle;   /* Handle for Rx data             */
  cmx_mm_rec_qcp_para_type rec_param;    /* Recording parameters           */ 
  cmx_cb_func_ptr_type     cb_func;      /* Callback function pointer      */
  const void               *client_data; /* Client data                    */
} cmx_qcp_record_packet_type;

/* Enumerated type to indicate the direction of the recorded QCP data
*/
typedef enum {
  CMX_QCP_DIR_REVERSE,   /* Reverse (Tx path) link */
  CMX_QCP_DIR_FORWARD,   /* Forward (Rx path) link */
  CMX_QCP_DIR_INVALID
} cmx_qcp_dir_type;

/* This enum type is used to describe the type of QCP data, in the buffer,
** that is to be recorded.
*/
typedef enum {
  CMX_QCP_REC_DATA_BUFFER_UNCATAGORIZED = 0, /* uncategorized data type     */
  CMX_QCP_REC_DATA_BUFFER_RECDATA,           /* recorded sound data         */
  CMX_QCP_REC_DATA_BUFFER_MAX
} cmx_qcp_rec_data_buffer_enum_type;

/* QCP recorded data packet type
*/
typedef struct {
  cmx_hdr_type        hdr;        /* Command Header                    */
  cmx_af_handle_type  *handle;    /* Descriptor of current service     */
  cmx_qcp_dir_type    link;       /* Path that data is recorded from   */
  cmx_qcp_rec_data_buffer_enum_type
                      content;    /* Type of data being recorded       */
  uint8               *buffer;    /* Buffer with recorded data         */
  uint32              num_bytes;  /* Number of bytes in buffer         */
  uint32              position;   /* Position the data belongs in file */
} cmx_qcp_rec_data_packet_type;

/* Recording buffer request from sound server command packet
*/ 
typedef struct {
  cmx_hdr_type             hdr;      /* Command Header                     */
  cmx_af_handle_type       *handle;  /* Descriptor of current service      */
  snd_qcp_buf_cb_func_type cb_func;  /* Callback function to return buffer */
} cmx_qcp_rec_buf_packet_type;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM_REC
/* PCM record start packet type
*/
typedef struct {
  cmx_hdr_type          hdr;          /* Command Header                    */
  cmx_handle_type       *handle;      /* Descriptor of current service     */
  cmx_pcm_rec_para_type rec_param;    /* Record parameters                 */
  cmx_cb_func_ptr_type  cb_func;      /* Callback function pointer         */
  const void            *client_data; /* Client data                       */
} cmx_pcm_rec_packet_type;

/* PCM record command packet type
*/
typedef struct {
  cmx_hdr_type          hdr;          /* Command Header                    */
  cmx_pcm_rec_link_type link;         /* Record link                       */
  cmx_cb_func_ptr_type  cb_func;      /* Callback function pointer         */
  const void            *client_data; /* Client data                       */
} cmx_pcm_rec_cmd_packet_type;

/* PCM record data packet type
*/
typedef struct {
  cmx_hdr_type              hdr;          /* Command Header                   */
  cmx_af_handle_type        *handle;      /* Descriptor of current service    */
  uint8                     *buffer;      /* Buffer of recorded data          */
  uint32                    num_bytes;    /* Number of bytes of recorded data */
  uint32                    position;     /* Position in file to write data   */
  snd_pcm_buf_cb_func_type  buf_cb;       /* Cb func to call to return buffer */
  const void                *client_data; /* Client data for buffer CB func   */
} cmx_pcm_rec_data_packet_type;

/* PCM record status packet type
*/
typedef struct {
  cmx_hdr_type          hdr;          /* Command Header                    */
  cmx_status_type       status;       /* Status of PCM processing          */
  cmx_af_handle_type    *handle;      /* Descriptor of current service     */
} cmx_pcm_rec_status_packet_type;
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/* 
 * MultiMedia record packet type, used to start recording a MultiMedia file
 */
typedef struct {
  cmx_hdr_type            hdr;             /* Command Header                 */
  cmx_handle_type         *mm_handle;      /* Descriptor of current service  */ 
  cmx_mm_rec_param_type   rec_param;       /* Recording parameters           */ 
  cmx_cb_func_ptr_type    cb_func;         /* Callback function pointer      */
  const void              *client_data;    /* Client data                    */
} cmx_mm_record_packet_type;

/* 
 * MultiMedia record command packet type
 */
typedef struct {
  cmx_hdr_type            hdr;             /* Command Header                 */
  cmx_mm_rec_link_type    link;            /* Direction to record            */
  cmx_cb_func_ptr_type    cb_func;         /* Callback function pointer      */
  const void              *client_data;    /* Client data                    */
} cmx_mm_rec_cmd_packet_type;

/* 
 * MultiMedia recorded data packet type
 */
typedef struct {
  cmx_hdr_type            hdr;             /* Command Header                 */
  cmx_af_handle_type      *handle;         /* Descriptor of current service  */
  uint8                   *buffer;         /* Buffer with recorded data      */
  uint32                  num_bytes;       /* Number of bytes in buffer      */
  uint32                  position;        /* Position the data inside file  */
  snd_mm_buf_cb_func_type buf_cb;
  const void              *client_data;
} cmx_mm_rec_data_packet_type;

/* 
 * MultiMedia recording buffer request from sound server command packet
 */ 
typedef struct {
  cmx_hdr_type             hdr;            /* Command Header                 */
  cmx_af_handle_type       *handle;        /* Descriptor of current service  */
  snd_mm_buf_cb_func_type  cb_func;        /* Callback function to return buf*/
} cmx_mm_rec_buf_packet_type;

/* 
 * MultiMedia recording buffer request from sound server command packet
 */ 
typedef struct {
  cmx_hdr_type          hdr;               /* Command Header                 */
  cmx_status_type       status;            /* Status of PCM processing       */
  cmx_af_handle_type    *handle;           /* Descriptor of current service  */
} cmx_mm_rec_status_packet_type;
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
typedef struct {
  cmx_hdr_type           hdr;        /* Command Header */
  cmx_af_handle_type     *handle;    /* Descriptor of current service */
  cmx_af_codec_info_type codec_info; /* Codec information */
} cmx_af_codec_info_update_packet_type;

typedef struct {
  cmx_hdr_type                    hdr;     /* Command Header */
  cmx_af_dual_mono_mode_enum_type mode;    /* Dual Mono Channel Mode */
  cmx_cb_func_ptr_type       cb_func;      /* Callback function pointer */
  void                       *client_data; /* Client data               */
} cmx_af_dual_mono_mode_packet_type;

typedef struct {
  cmx_hdr_type               hdr;          /* Command Header */
  cmx_af_sbr_mode_enum_type  mode;         /* SBR Mode */
  cmx_cb_func_ptr_type       cb_func;      /* Callback function pointer */
  void                       *client_data; /* Client data               */
} cmx_af_sbr_mode_packet_type;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_SVG 
typedef struct{
  q_link_type              link;
  cmx_svg_decode_data_type svg_decode;
} cmx_svg_decode_pk_type;

#ifdef FEATURE_SVGDOM
typedef struct{
  q_link_type              link;
  uint16                   str[CMX_SVG_MAX_STRING_LENGTH]; //16-bit strings
  cmx_af_handle_type       *handle;
  cmx_svgdom_api_enum_type api;
}cmx_svgdom_strpool_type;
#endif /* FEATURE_SVGDOM */

#endif /* FEATURE_SVG */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
/* Buffer request command packet
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  cmx_client_buf_type   *buf_handle;   /* Buffer pointer handle             */
  uint32                num_bytes;     /* Number of bytes of buffer request */
} cmx_client_buf_req_pkt_type;

/* Buffer request callback command packet
*/
typedef struct {
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_client_buf_type   *buf_handle;   /* Buffer pointer handle             */
  uint8                 *buffer;       /* Buffer returned from client       */
} cmx_client_buf_req_cb_pkt_type;
#endif  /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

#ifdef FEATURE_CMX_IMAGE
typedef enum {
#ifdef FEATURE_PNG_DECODER
  CMX_IMAGE_TYPE_PNG,
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_SAF
  CMX_IMAGE_TYPE_SAF,
#endif /* FEATURE_SAF */
#ifdef FEATURE_JPEG_DECODER
  CMX_IMAGE_TYPE_JPEG,
#endif /* FEATURE_JPEG_DECODER */
#ifdef FEATURE_SVG
  CMX_IMAGE_TYPE_SVG,
#endif /* FEATURE_SVG */
  CMX_IMAGE_TYPE_INVALID
} cmx_image_enum_type;

typedef union {
#ifdef FEATURE_PNG_DECODER
  png_cmx_command_cb_func_type      png_cb;
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_SAF
  saf_cmx_command_cb_func_ptr_type  saf_cb;
#endif /* FEATURE_SAF */
#ifdef FEATURE_JPEG_DECODER
  jpeg_cmx_command_cb_func_type     jpeg_cb;
#endif /* FEATURE_JPEG_DECODER */
#ifdef FEATURE_SVG
  svg_decoder_cb_type               svg_cb;
#endif /* FEATURE_SVG */
} cmx_image_command_cb_type;

/* PNG decoder/SAF/JPEG decoder command packet
*/
typedef struct
{
  cmx_hdr_type               hdr;            /* Command Header               */
  cmx_handle_type            *handle;        /* Descriptor of CMX file       */
#ifdef FEATURE_SAF
  cmx_saf_ctl_type           saf_ctl;        /* SAF frame control            */
#endif /* FEATURE_SAF */
#ifdef FEATURE_SVG
  cmx_svg_decode_pk_type   *svg_decode;    /* SVG decode command           */
#endif /* FEATURE_SVG */
  cmx_image_cb_func_ptr_type cb_func;        /* Callback function pointer    */
  void                       *client_data;   /* Client data                  */
} cmx_image_decode_packet_type;

/* PNG/SAF/JPEG callback command packet
*/
typedef struct
{
  cmx_hdr_type              hdr;           /* Command Header                */
  cmx_image_enum_type       image_type;
  cmx_image_command_cb_type cb_func;       /* Callback function pointer     */
  void                      *client_data;  /* Client data                   */
} cmx_image_cb_packet_type;

/* CMX PNG/SAF/JPEG get/read data command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_image_enum_type   image_type;
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  uint8                 *buffer;       /* Buffer to read data into          */
  uint32                num_bytes;     /* Number of bytes to read           */
  uint32                position;      /* Position to get data, offset from */
                                       /* beginning of PNG file             */
  cmx_image_command_cb_type cb_func;       /* Callback function pointer     */
  void                      *client_data;  /* Client data                   */
} cmx_image_read_packet_type;

#ifdef FEATURE_WEBAUDIO
/* CMX PNG/SAF/JPEG get buffer command packet
*/
typedef struct
{
  cmx_hdr_type          hdr;           /* Command Header                    */
  cmx_image_enum_type   image_type;
  cmx_af_handle_type    *handle;       /* CMX descriptor of current service */
  uint8                 **buf_handle;  /* Buffer handle to set              */
  uint32                num_bytes;     /* Number of bytes of buffer         */

  cmx_image_command_cb_type cb_func;       /* Callback function pointer     */
  void                      *client_data;  /* Client data                   */
} cmx_image_buf_req_packet_type;
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_SAF
/* SAF continue command packet */
typedef struct
{
  cmx_hdr_type        hdr;            /* Command Header               */
  cmx_af_handle_type  *handle;        /* Descriptor of CMX file       */
  cmx_saf_ctl_type    saf_ctl;        /* SAF frame control            */
} cmx_saf_cont_packet_type;

/* SAF frame ID command packet */
typedef struct {
  cmx_hdr_type          hdr;            /* Command Header                */
  uint32                frame_id;       /* Frame ID                      */
  uint8                 *frame_buffer;  /* Frame buffer                  */
  cmx_cb_func_ptr_type  cb_func;        /* Callback function pointer     */
  void                  *client_data;   /* Client data                   */
} cmx_saf_frame_id_packet_type;

/* SAF frame done command packet */
typedef struct
{
  cmx_hdr_type          hdr;        /* Command Header                    */
  cmx_af_handle_type    *handle;    /* Descriptor of current service     */
  uint8                 *buffer;    /* Buffer containing frame data      */
  uint32                num_bytes;  /* Number of bytes in the buffer     */
  uint32                delay;      /* Delay until next frame            */
  boolean               last_frame; /* Last frame flag                   */
} cmx_saf_frame_done_packet_type;

/* This enum is used to keep track of the state of the SAF decoder
*/
typedef enum {
  CMX_SAF_STATUS_STOPPED,    /* SAF is not currently active     */
  CMX_SAF_STATUS_PREPARING,  /* SAF is decoding images          */
  CMX_SAF_STATUS_PLAYING,    /* SAF is ready to accept commands */
  CMX_SAF_STATUS_PAUSED,     /* SAF is paused                   */
  CMX_SAF_STATUS_INVALID
} cmx_saf_status_type;
#endif /* FEATURE_SAF */

#ifdef FEATURE_MIDI_OUT
/* Structure used for MIDI_OUT_OPEN commands */
typedef struct {
  cmx_hdr_type            hdr;                   /* Command Header            */
  cmx_cb_func_ptr_type    cb_func;               /* Callback function pointer */
  void                    *client_data;          /* Client data               */
  cmx_adec_mode_enum_type adec_mode;             /* CMX QSynth ADEC type      */
} cmx_midi_out_open_packet_type;

/* Structure used for MIDI_OUT commands */
typedef struct
{
  cmx_hdr_type                  hdr;            /* Command Header            */
  cmx_handle_type               *handle;        /* Descriptor of CMX file    */
  cmx_cb_func_ptr_type          cb_func;        /* Callback function pointer */
  void                          *client_data;   /* Client data               */
} cmx_midi_out_msg_packet_type;

#ifdef FEATURE_MIDI_OUT_QCP
/* This structure is used to manage the MIDI device audio decoder channels.
** There will be one of these ID structures per channel.
*/
typedef struct {
  q_link_type        link;       /* Link for queue services                  */
  uint32             channel;    /* Channel number of audio decoder          */
  cmx_af_handle_type *af_handle; /* Handle of CMX file for the audio decoder */
} cmx_midi_out_qcp_handle_type;

/* Structure used for MIDI_OUT QCP commands */
typedef struct
{
  cmx_hdr_type                  hdr;              /* Command Header          */
  cmx_handle_type               *handle;          /* Descriptor of CMX file  */
  cmx_af_handle_type            *af_handle;       /* Descriptor of QCP file  */
  cmx_midi_out_adec_param_type  adec_param;       /* Parameters for playback */
  cmx_midi_out_qcp_handle_type  *midi_out_handle; /* ID of QCP to act on     */
  cmx_midi_out_cb_func_ptr_type qcp_cb_func;      /* QCP cb function pointer */
  cmx_cb_func_ptr_type          cb_func;          /* Cb function pointer     */
  void                          *client_data;     /* Client data             */
} cmx_midi_out_qcp_packet_type;
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
typedef struct 
{
  cmx_hdr_type  hdr; /* Command Header                       */
  cmx_audfmt_poly_mode_type mode;
  cmx_cb_func_ptr_type  cb_func;      /* Cb function pointer                  */
  void                  *client_data; /* Client data                          */
} cmx_af_poly_mode_packet_type;

typedef struct 
{
  cmx_hdr_type  hdr; /* Command Header                       */
  cmx_audfmt_poly_mode_type *mode;
  cmx_cb_func_ptr_type  cb_func;      /* Cb function pointer                  */
  void                  *client_data; /* Client data                          */
} cmx_af_get_poly_mode_packet_type;

typedef struct
{
  cmx_hdr_type       hdr;          /* Command Header */
  cmx_status_type    pm_status;    /* status of poly mode cmd */
  cmx_af_handle_type  *handle;      /* CMX handle             */
} cmx_af_set_poly_mode_cb_packet_type;

typedef struct
{
  cmx_hdr_type               hdr;          /* Command Header */
  cmx_status_type            pm_status;    /* status of poly mode cmd */
  cmx_af_handle_type         *handle;      /* CMX handle */
  cmx_audfmt_poly_mode_type  mode;         /* poly mode returned by snd */
} cmx_af_get_poly_mode_cb_packet_type;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_QAUDIOFX
/* 3D positional command packet type */
typedef struct
{
  cmx_hdr_type          hdr;          /* Command Header                       */
  cmx_3d_pos_param_type param;        /* Parameters for 3D positional command */
  cmx_cb_func_ptr_type  cb_func;      /* Cb function pointer                  */
  void                  *client_data; /* Client data                          */
} cmx_3d_pos_packet_type;

/* 3D positional query command packet type */
typedef struct
{
  cmx_hdr_type               hdr;          /* Command Header              */
  cmx_3d_pos_param_enum_type param;        /* Parameter being queried     */
  void                       *id;          /* ID for source being queried */
  cmx_3d_pos_get_cb_ptr_type cb_func;      /* Cb function pointer         */
  void                       *client_data; /* Client data                 */
} cmx_3d_pos_query_packet_type;

/* Structure containing 3D positional callback types */
typedef struct {
  union {
    cmx_3d_pos_get_cb_ptr_type get_cb;  /* Query callback function pointer */
    cmx_cb_func_ptr_type       base_cb; /* Base callback function pointer  */
  } cb_func;
  void *client_data;                    /* Client data                     */
} cmx_3d_pos_cb_struct_type;

/* 3D positional callback command type */
typedef struct
{
  cmx_hdr_type              hdr;         /* Command Header                */
  cmx_status_type           status;      /* Status of callback            */
  cmx_3d_pos_cb_struct_type *cb_struct;  /* Callback function information */
} cmx_3d_pos_cb_packet_type;

/* 3D positional query callback command type */
typedef struct
{
  cmx_hdr_type                  hdr;        /* Command Header                */
  cmx_status_type               status;     /* Status of callback            */
  cmx_3d_pos_param_enum_type    param;      /* Parameter being queried       */
  cmx_3d_pos_param_setting_type setting;    /* Result of query               */
  cmx_3d_pos_cb_struct_type     *cb_struct; /* Callback function information */
} cmx_3d_pos_query_cb_packet_type;
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDIO_POSTCARD
/* Audio postcard create command packet */
typedef struct
{
  cmx_hdr_type         hdr;              /* Command Header            */
  cmx_handle_type      *jpeg;            /* Descriptor of JPEG file   */
  cmx_handle_type      *qcp;             /* Descriptor of QCP file    */
  cmx_handle_type      *output;          /* Descriptor of output file */
  cmx_cb_func_ptr_type cb_func;          /* Cb function pointer       */
  void                 *client_data;     /* Client data               */
} cmx_create_audiop_packet_type;

/* Audio postcard complete command packet */
typedef struct
{
  cmx_hdr_type         hdr;              /* Command Header               */
  cmx_status_type      status;           /* Status of QCP processing     */
  cmx_af_handle_type   *handle;          /* Descriptor of QCP file       */
  uint32               num_bytes;        /* Number of bytes of QCP data  */
  uint32               position;         /* Position of QCP data in file */
} cmx_complete_audiop_packet_type;
#endif /* FEATURE_AUDIO_POSTCARD */

#ifdef FEATURE_DLS
/* DLS load/unload command packet */
typedef struct
{
  cmx_hdr_type         hdr;              /* Command Header            */
  cmx_handle_type      *handle;          /* Descriptor of DLS file    */
  cmx_cb_func_ptr_type cb_func;          /* Cb function pointer       */
  void                 *client_data;     /* Client data               */
} cmx_audfmt_dls_packet_type;
#endif /* FEATURE_DLS */

#ifdef FEATURE_XMF
/* Status type for commands between XMF and CMX modules */
typedef enum {
  CMX_XMF_SUCCESS,       /* Command was successful */
  CMX_XMF_BUFF_PENDING,  /* Buffer pending status  */
  CMX_XMF_DONE,          /* Command done           */
  CMX_XMF_FAILURE        /* Command failed         */
} cmx_xmf_status_type;

/* XMF processing command packet */
typedef struct
{
  cmx_hdr_type        hdr;        /* Command Header            */
  cmx_xmf_status_type status;     /* Status of XMF command     */
  cmx_af_handle_type  *handle;    /* XMF handle                */
  uint32              num_bytes;  /* Number of bytes           */
} cmx_audfmt_xmf_packet_type;
#endif /* FEATURE_XMF */

#ifdef FEATURE_SVG
/* Base packet type for SVG commands 
** no parameters.
*/
typedef struct
{
  cmx_hdr_type           hdr;          /* Command Header            */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  cmx_cb_func_ptr_type   cb_func;      /* Callback function pointer */
  void                   *client_data; /* Client data               */
} cmx_svg_base_packet_type;

/* This struct is used for storing info for frame cmd
*/
typedef struct{
  cmx_hdr_type         hdr;       /* Command Header               */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  uint32               time_ms;
  cmx_cb_func_ptr_type callback_ptr;
  void                 *client_data;
}cmx_svg_frame_packet_type;

/* This struct is used for storing info for Event cmd
*/
typedef struct{
  cmx_hdr_type         hdr;       /* Command Header               */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  cmx_svg_event_enum_type   event_type; 
  cmx_svg_event_type        event_data;
  uint32                    num_bytes;
  uint32                    time_ms;
}cmx_svg_event_packet_type;

/* This struct is used for storing info for Rewind, F-Forward cmd
*/
typedef struct{
  cmx_hdr_type         hdr;       /* Command Header               */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  uint32                    time_ms;
  cmx_cb_func_ptr_type      callback_ptr;
  void*                     client_data;
}cmx_svg_seek_packet_type;

/* This struct is used for storing info on continue cmd 
*/
typedef struct
{
  cmx_hdr_type         hdr;       /* Command Header                          */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  cmx_client_buf_type  buffer;    /* Frame buffer for SVG cmds               */
  uint8                request;   /* Request made: 0-none, 1-frame           */
} cmx_svg_cont_packet_type;

/* This struct is used for storing info on External Request cmd
*/
typedef struct
{
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  char                filename[FS_FILENAME_MAX_LENGTH_P];
  uint8               type;
  svg_cmx_get_request_cb_type next_cb;
  svg_cmx_request_cb_type  cb_func;
  void*                    cb_data;
  uint8**             buf_ptr;
  uint32*             num_bytes;
}cmx_svg_req_pk_type;

typedef struct{
  cmx_hdr_type         hdr;
  cmx_svg_req_pk_type  *svg_req;
  //cmx_svg_req_pk_type  svg_req;
}cmx_svg_req_packet_type;

/* This struct is used for storing info on External request get data cmd
*/
typedef struct
{
  cmx_hdr_type         hdr;
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  uint8*               buffer;
  uint32               num_bytes;
  uint32               position;
  svg_cmx_request_cb_type  cb_func;
  void*                    cb_data;
}cmx_svg_req_data_packet_type;

/* This struct is used for storing request cb cmd
*/
typedef struct
{
  cmx_hdr_type         hdr;
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  uint8**              buffer;
  uint32               num_bytes;
}cmx_svg_req_cb_packet_type;

/* This struct is used for storing info of Set Viewport cmd
*/
typedef struct
{
  cmx_hdr_type         hdr;
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  uint32               width;
  uint32               height;
  cmx_cb_func_ptr_type   callback_ptr;
  void                  *client_data;
}cmx_svg_viewport_packet_type;

#ifdef FEATURE_SVGDOM
typedef struct{
  cmx_hdr_type         hdr;       /* Command Header               */
  cmx_af_handle_type* handle;          /* CMX handle for multi-file playback */
  cmx_svgdom_api_enum_type  type;
  cmx_svgdom_para_type         data;
}cmx_svgdom_packet_type;
#endif /* FEATURE_SVGDOM */

#ifdef FEATURE_WEBAUDIO
/* This command packet is used to process a frame buffer returned for SVG
** processing.
*/
typedef struct {
  cmx_hdr_type        hdr;       /* Command Header                    */
  cmx_af_handle_type  *handle;   /* Descriptor of current service     */
} cmx_svg_frame_buffer_packet_type;

/* This command packet is used to return an SVG frame to client */
typedef struct {
  cmx_hdr_type        hdr;            /* Command Header                */
  cmx_af_handle_type  *handle;        /* Descriptor of current service */
  uint8               *frame_buffer;  /* Buffer containing frame data  */
  uint32              frame_size;     /* Size of frame data            */
} cmx_svg_frame_done_packet_type;
#endif /* FEATURE_WEBAUDIO */

#endif /* FEATURE_SVG */

typedef union {
  cmx_hdr_type                   hdr;
  cmx_base_packet_type           base;               /* Base CMX packet type  */
#ifdef FEATURE_AUDIO_FORMAT
  cmx_af_read_packet_type        audfmt_read;        /* CMX data read         */
  cmx_af_get_dls_packet_type     audfmt_get_dls;     /* CMX get DLS           */
  cmx_af_read_rsp_packet_type    audfmt_read_rsp;    /* CMX read response     */
  cmx_af_play_codec_packet_type  audfmt_play_codec;  /* CMX codec play        */
  cmx_af_play_packet_type        audfmt_play;        /* CMX play              */
  cmx_af_ringer_packet_type      audfmt_ringer;      /* Midi Ringer           */
  cmx_af_time_packet_type        audfmt_time;        /* CMX get time          */
  cmx_af_play_ctl_packet_type    audfmt_play_ctl;    /* CMX play control      */
  cmx_af_sequence_packet_type    audfmt_seq_cmd;     /* CMX sequence command  */
  cmx_af_cb_packet_type          audfmt_cb;          /* CMX sound server CB   */
  cmx_af_support_packet_type     audfmt_support;     /* CMX playback support  */
  cmx_af_aud_spec_packet_type    audfmt_audio_spec;  /* CMX audio specs       */
  cmx_af_get_time_cb_packet_type audfmt_get_time_cb; /* CMX get time CB       */
  cmx_af_get_spec_cb_packet_type audfmt_get_spec_cb; /* CMX get file type CB  */
  cmx_af_preparse_packet_type    audfmt_preparse;    /* CMX preparse command  */
  cmx_af_get_spec_packet_type    audfmt_get_spec;    /* CMX get audio spec    */
  #ifdef FEATURE_AUDFMT_HYBRID_MODE
  cmx_af_poly_mode_packet_type   audfmt_poly_mode;   /* AUDFMT poly mode cmd  */
  cmx_af_get_poly_mode_packet_type audfmt_get_poly_mode; /* get poly mode cmd */
  cmx_af_set_poly_mode_cb_packet_type audfmt_poly_mode_cb; /* Poly mode CB    */
  cmx_af_get_poly_mode_cb_packet_type audfmt_get_pm_cb; /* Poly mode query */
#endif /* FEAUTRE_AUDFMT_HYBRID_MODE */
#ifdef FEATURE_WEBAUDIO
  cmx_af_pic_init_packet_type    audfmt_pic_init;    /* CMX picture init      */
  cmx_af_pic_data_packet_type    audfmt_pic_data;    /* CMX picture data      */
  cmx_af_pic_ctl_packet_type     audfmt_pic_ctl;     /* CMX picture control   */
  cmx_af_text_ctl_packet_type    audfmt_text_ctl;    /* CMX text control      */
  cmx_af_anim_data_packet_type   audfmt_anim_data;   /* CMX animation command */
#ifdef FEATURE_SMAF
  cmx_smaf_graph_info_packet_type audfmt_graph_info;   /* SMAF graphic info   */
  cmx_smaf_led_vib_ctl_packet_type audfmt_smaf_led_vib_ctl;/* SMAF LED/Vib ctrl   */
#endif /* FEATURE_SMAF */
  cmx_mfi_led_ctl_packet_type    audfmt_mfi_led_ctl; /* MFi LED control       */
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
  cmx_af_led_ctl_packet_type   audfmt_led_ctl;   /* CMX LED control      */
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
  cmx_af_blight_ctl_packet_type audfmt_blight_ctl;/* CMX Backlight control*/
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_QCP
  cmx_qcp_func_packet_type       qcp_func;           /* QCP function command  */
  cmx_qcp_func_cb_packet_type    qcp_func_cb;        /* QCP func cb command   */
  cmx_qcp_record_packet_type     qcp_record;         /* QCP record command    */
  cmx_qcp_rec_data_packet_type   qcp_rec_data;       /* QCP rec data command  */
  cmx_qcp_rec_buf_packet_type    qcp_rec_get_buf;    /* QCP rec buf request   */
#endif /* FEATURE_QCP */
#ifdef FEATURE_PCM_REC
  cmx_pcm_rec_packet_type        pcm_record;     /* PCM record start command  */
  cmx_pcm_rec_cmd_packet_type    pcm_rec_cmd;    /* PCM record command        */
  cmx_pcm_rec_data_packet_type   pcm_rec_data;   /* PCM record data command   */
  cmx_pcm_rec_status_packet_type pcm_rec_status; /* PCM record status command */
#endif /* FEATURE_PCM_REC */
#ifdef FEATURE_MM_REC
  cmx_mm_record_packet_type     mm_record;      /* MM record command    */
  cmx_mm_rec_cmd_packet_type    mm_rec_cmd;     /* MM record command    */
  cmx_mm_rec_data_packet_type   mm_rec_data;    /* MM rec data command  */
  cmx_mm_rec_buf_packet_type    mm_rec_get_buf; /* MM rec buf request   */
  cmx_mm_rec_status_packet_type mm_rec_status;  /* MM record status command */
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
  cmx_client_buf_req_pkt_type    client_buf_req;     /* CMX buffer request    */
  cmx_client_buf_req_cb_pkt_type client_buf_req_cb;  /* CMX buffer request CB */
#endif  /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */
#ifdef FEATURE_CMX_IMAGE
  cmx_image_decode_packet_type   image_decode;   /* CMX PNG/SAF/JPEG decoder  */
  cmx_image_cb_packet_type       image_cb;       /* CMX PNG/SAF/JPEG callback */
  cmx_image_read_packet_type     image_read;     /* CMX PNG/SAF/JPEG read     */
#ifdef FEATURE_WEBAUDIO
  cmx_image_buf_req_packet_type  image_buf_req;  /* Image buffer request cmd  */
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */
#ifdef FEATURE_SAF
  cmx_saf_cont_packet_type        saf_cont;       /* SAF continue command     */
  cmx_saf_frame_id_packet_type    saf_frame_id;   /* SAF frame ID command     */
  cmx_saf_frame_done_packet_type  saf_frame_done; /* SAF frame done           */
#endif /* FEATURE_SAF */
#ifdef FEATURE_MIDI_OUT
  cmx_midi_out_open_packet_type   midi_out_open; /* CMX MIDI_OUT_OPEN command */
  cmx_midi_out_msg_packet_type    midi_out;      /* CMX MIDI_OUT command      */
#ifdef FEATURE_MIDI_OUT_QCP
  cmx_midi_out_qcp_packet_type    midi_out_qcp;  /* CMX MIDI_OUT QCP cmd      */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */
#ifdef FEATURE_QAUDIOFX
  cmx_3d_pos_packet_type          cmd_3d_pos;      /* 3D positional command   */
  cmx_3d_pos_query_packet_type    query_3d_pos;    /* 3D positional query cmd */
  cmx_3d_pos_cb_packet_type       cb_3d_pos;       /* 3D positional callback  */
  cmx_3d_pos_query_cb_packet_type query_cb_3d_pos; /* 3D pos query callback   */
#endif /* FEATURE_QAUDIOFX */
#ifdef FEATURE_AUDIO_POSTCARD
  cmx_create_audiop_packet_type   create_audiop;   /* Create audio PC cmd     */
  cmx_complete_audiop_packet_type complete_audiop; /* Complete audio PC cmd   */
#endif /* FEATURE_AUDIO_POSTCARD */
#ifdef FEATURE_DLS
  cmx_audfmt_dls_packet_type      audfmt_dls;  /* DLS load/unload command     */
#endif /* FEATURE_DLS */
#ifdef FEATURE_XMF
  cmx_audfmt_xmf_packet_type      xmf_cmd;     /* XMF continue processing cmd */
#endif /* FEATURE_XMF */
#ifdef FEATURE_SVG
  cmx_svg_frame_packet_type       svg_frame;      /* SVG Frane command      */
  cmx_svg_event_packet_type       svg_event;      /* SVG Event command      */
  cmx_svg_seek_packet_type        svg_seek;       /* SVG rewind/fforward    */
  cmx_svg_cont_packet_type        svg_cont;       /* SVG buffer requests    */
  cmx_svg_req_packet_type         svg_req;        /* SVG file open request  */
  cmx_svg_req_data_packet_type    svg_req_data;   /* SVG file read request  */
  cmx_svg_req_cb_packet_type      svg_req_cb;     /* SVG file request cb    */
  cmx_svg_viewport_packet_type    svg_viewport;   /* SVG set Viewport cmd   */
  cmx_svg_base_packet_type        svg_base;
#ifdef FEATURE_SVGDOM
  cmx_svgdom_packet_type          svgdom_data;   /* SVG DOM call           */
#endif /* FEATURE_SVGDOM */
#ifdef FEATURE_WEBAUDIO
  cmx_svg_frame_buffer_packet_type svg_frame_buffer; /* SVG frame buffer cmd */
  cmx_svg_frame_done_packet_type   svg_frame_done;   /* SVG frame done cmd   */
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_SVG */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
  cmx_af_codec_info_update_packet_type codec_update;   /* Codec Update */
  cmx_af_dual_mono_mode_packet_type    dual_mono_mode; /* Dual Mono Mode cmd */
  cmx_af_sbr_mode_packet_type          sbr_mode;       /* SBR Mode cmd */
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */
} cmx_packet_type;

#ifdef FEATURE_AUDIO_FORMAT
/* Enumerated type to indicate the current action in CMX */
typedef enum {
  CMX_AF_GET_TIME,         /* get time active                     */
  CMX_AF_PLAY,             /* midi/WebAudio playback active       */
  CMX_AF_RINGER,           /* midi ringer active                  */
  CMX_AF_QCP_REC,          /* recording QCP data active           */
  CMX_AF_AUDIOP,           /* creating audio postcard             */
  CMX_AF_PLAY_MULTI,       /* midi/WebAudio multi playback active */
  CMX_AF_GET_SPEC,         /* get audio specifications            */
  CMX_AF_MM_REC,           /* recording MM data active            */
  CMX_AF_NONE              /* no playback active                  */
} cmx_af_active_type;
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_MIDI_OUT
/* Enum type used to indicate status of MIDI output device */
typedef enum {
  CMX_MIDI_OUT_DISABLE,    /* Disable MIDI output device    */
  CMX_MIDI_OUT_ENABLE,     /* Enable MIDI output device     */
  CMX_MIDI_OUT_INVALID     /* Invalid indicator             */
} cmx_midi_out_type;
#endif /* FEATURE_MIDI_OUT */

/* Enum type used to indicate the cause of playback stop */
typedef enum {
  CMX_AF_STOP_NONE,         /* Stop command is not issued                  */
  CMX_AF_STOP_CLIENT_ABORT, /* Client requests the playback to stop        */
  CMX_AF_STOP_PREEMPTED,    /* The sound is prempted by a new sound with same 
                               priority */
  CMX_AF_STOP_MAX
} cmx_af_stop_enum_type;  

/* <EJECT> */
/*---------------------------------------------------------------------------
** CMX function prototypes
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_audfmt_callback_processing

DESCRIPTION
  This function takes the appropriate action for sound server callbacks
  for audio format sounds.

DEPENDENCIES
  cmx_af_stopped

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped

===========================================================================*/
LOCAL void cmx_audfmt_callback_processing(
  void            *client_data,
  snd_status_type status
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_done_processing

DESCRIPTION
  This function processes a play done status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_play_done_processing (
  snd_status_type status,
  void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_read_processing

DESCRIPTION
  This function issues a command to EFS to read from a midi/WebAudio file.

DEPENDENCIES
  cmx_af_buf_info
  cmx_af_buf_type_in_use
  cmx_af_efs_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_efs_cb_array

===========================================================================*/
LOCAL void cmx_audfmt_read_processing (
  cmx_af_handle_type       *handle,
  uint32                   position,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_read_var_processing

DESCRIPTION
  This function issues a command to read a variable amount of data from a file.

DEPENDENCIES
  cmx_af_buf_info
  cmx_af_data_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_data_cb_array

===========================================================================*/
LOCAL void cmx_audfmt_read_var_processing (
  snd_af_data_cmd_enum     cmd,
  cmx_af_handle_type       *handle,
  uint32                   position,
  uint32                   buffer_size,
  uint32                   offset,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_dls_processing

DESCRIPTION
  This function issues a command to get DLS waveform from file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_dls_processing (
  cmx_af_handle_type          *handle,
  uint32                      length,
  uint32                      position,
  void                        *client_data,
  snd_af_get_dls_cb_func_type cb_func
);
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_support_processing

DESCRIPTION
  This function performs the appropriate WebAudio support.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_support_processing (
  snd_af_data_cmd_enum   command,
  cmx_af_handle_type     *handle,
  uint32                 position,
  uint32                 num
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_audio_spec_processing

DESCRIPTION
  This function calls back client with the character set that is used with
  the text commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_audio_spec_processing (
  cmx_af_handle_type     *handle,
  cmx_af_file_enum_type  file_type,
  cmx_af_audio_spec_type *audio_spec
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_cb_processing

DESCRIPTION
  This function processes the get time command callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=======================================================================*/
LOCAL void cmx_audfmt_get_time_cb_processing
(
  cmx_af_handle_type *handle,
  snd_status_type    status,
  uint32             time_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_spec_cb_processing

DESCRIPTION
  This function processes the get time command callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_get_spec_cb_processing (
  cmx_af_handle_type   *handle,
  snd_status_type      status
);

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_audfmt_text_ctl_processing

DESCRIPTION
  This function gets the complete text string and returns it to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_text_ctl_processing (
  cmx_af_text_ctl_enum_type  command,
  uint8                      *buffer,
  uint32                     buf_len,
  cmx_af_handle_type         *handle,
  uint32                     position,
  uint32                     remain_bytes,
  cmx_af_pos_enum_type       xpos_type,
  cmx_af_pos_enum_type       ypos_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_init_processing

DESCRIPTION
  This function returns the setup information of the current picture to
  the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_pic_init_processing (
  cmx_af_handle_type           *handle,
  cmx_af_pic_speed_enum_type   draw_speed,
  cmx_af_pos_enum_type         xpos_type,
  cmx_af_pos_enum_type         ypos_type,
  uint8                        xpos_percent,
  uint8                        ypos_percent
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_data_processing

DESCRIPTION
  This function sets up the appropriate mechanism to pass picture data
  back to the client.

DEPENDENCIES
  cmx_af_png_scale

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_pic_data_processing (
  cmx_af_pic_enum_type pic_type,
  uint8                *buffer,
  uint32               buf_len,
  cmx_af_handle_type   *handle,
  uint32               position,
  uint32               remain_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pic_ctl_processing

DESCRIPTION
  This function sends a picture control command back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_pic_ctl_processing (
  cmx_af_pic_status_enum_type  command,
  cmx_af_handle_type           *handle,
  cmx_af_pic_speed_enum_type   speed
);

/* <EJECT> */
#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION cmx_af_pic_read_processing

DESCRIPTION
  This function performs the appropriate action upon receiving a picture data
  read callback.

DEPENDENCIES
  cmx_pic_remain
  cmx_pic_position
  cmx_pic_handle
  cmx_pic_type
  cmx_picture_buffer

RETURN VALUE
  None

SIDE EFFECTS
  cmx_pic_remain
  cmx_pic_position

===========================================================================*/
LOCAL void cmx_af_pic_read_processing( void );
#endif /* FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_data_processing

DESCRIPTION
  This function sets up the appropriate mechanism to initiate SAF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_anim_data_processing (
  cmx_af_anim_enum_type   type,
  uint8                   *buffer,
  uint32                  buf_len,
  cmx_af_handle_type      *handle,
  uint32                  position,
  uint32                  remain_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_frame_processing

DESCRIPTION
  This function issues a frame command to the SAF module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_anim_frame_processing (
  cmx_af_anim_enum_type   type,
  uint8                   *buffer,
  uint32                  buf_len,
  cmx_af_handle_type      *handle,
  uint32                  position,
  uint32                  remain_bytes,
  cmx_af_pos_struct_type  *frame_pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_anim_frame_id_processing

DESCRIPTION
  This function issues a frame ID command to the SAF module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_anim_frame_id_processing (
  cmx_af_anim_enum_type   type,
  cmx_af_handle_type      *handle,
  uint32                  frame_id,
  cmx_af_pos_struct_type  *frame_pos
);

/* <EJECT> */
#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION cmx_audfmt_smaf_graph_info_processing

DESCRIPTION
  This function calls back the client to indicate graphics track chunk 
  information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_smaf_graph_info_processing (
  cmx_af_handle_type       *handle,
  cmx_smaf_graph_info_type graph_info
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_smaf_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_smaf_led_ctl_processing (
  cmx_af_handle_type     *handle,
  cmx_smaf_led_ctl_status_enum_type smaf_led_ctl_status
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_smaf_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate vibrator control 
  information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_smaf_vib_ctl_processing (
  cmx_af_handle_type     *handle,
  cmx_smaf_vib_ctl_status_enum_type smaf_vib_ctl_status
);
#endif /* FEATURE_SMAF */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_mfi_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information
  specific to MFi.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_mfi_led_ctl_processing (
  cmx_af_handle_type *handle,
  uint32             mask
);
#ifdef FEATURE_SVG
/*===========================================================================

FUNCTION cmx_audfmt_svg_cleanup

DESCRIPTION
  This function cleans up SVG components in a WebAudio playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_audfmt_svg_cleanup (
  cmx_af_handle_type *handle
);
#endif /* FEATURE_SVG */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
/*===========================================================================

FUNCTION cmx_audfmt_led_ctl_processing

DESCRIPTION
  This function calls back the client to indicate LED control information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_led_ctl_processing (
  cmx_af_handle_type         *handle,
  cmx_af_led_ctl_enum_type   led_ctl
);
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_IMELODY
/*===========================================================================

FUNCTION cmx_audfmt_blight_ctl_processing

DESCRIPTION
  This function calls back the client to indicate Backlight control information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_blight_ctl_processing (
  cmx_af_handle_type         *handle,
  boolean                    backlight_ctl
);
#endif /* FEATURE_IMELODY */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_play_callback

DESCRIPTION
  This function is used as a callback to request/send data from the
  midi/WebAudio file.

DEPENDENCIES
  cmx_af_buf_info

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_buf_type_in_use
  cmx_skip_snd_cb

===========================================================================*/
LOCAL void cmx_af_play_callback (
  snd_af_data_cmd_enum  command,
  void                  *file,
  uint32                num,
  void                  *data
);

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_set_func_cb

DESCRIPTION
  This function is the callback function for QCP set cnfg, labl and 
  text command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_qcp_set_func_cb (
  snd_af_data_cmd_enum  command,
  void                  *data,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_func_done

DESCRIPTION
  This function cleans up after a QCP get/set comamnd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_func_done (
  cmx_status_type      status,
  cmx_af_handle_type   *handle,
  uint8                *buffer,
  uint32               num_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_get_buf_processing

DESCRIPTION
  This function allocates a buffer for QCP recording and returns it to
  the requestor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_qcp_rec_get_buf_processing (
  cmx_af_handle_type       *handle,
  snd_qcp_buf_cb_func_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_data_processing

DESCRIPTION
  This function saves the recorded QCP data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_qcp_rec_data_processing (
  cmx_af_handle_type                *handle,
  uint8                             *buffer,
  uint32                             num_bytes,
  uint32                             position,
  cmx_qcp_dir_type                   link,
  cmx_qcp_rec_data_buffer_enum_type  content
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_done_processing

DESCRIPTION
  This function cleans up the QCP recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_qcp_rec_done_processing (
  cmx_cmd_type             cmd,
  cmx_af_handle_type       *handle_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_rec_status_processing

DESCRIPTION
  This function returns recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_qcp_rec_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record_data_processing

DESCRIPTION
  This function records the buffer of PCM data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_pcm_record_data_processing (
  cmx_cmd_type              cmd,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  snd_pcm_buf_cb_func_type  buf_cb,
  const void                *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_done_processing

DESCRIPTION
  This function cleans up the PCM recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_pcm_record_done_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_status_processing

DESCRIPTION
  This function returns recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_pcm_record_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION cmx_mm_record_processing

DESCRIPTION
  This function starts recording MultiMedia data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_mm_record_processing (
  cmx_handle_type           *handle,
  cmx_mm_rec_param_type     *rec_param,
  cmx_cb_func_ptr_type      cb_func,
  const void                *client_data
); 

/*===========================================================================

FUNCTION cmx_mm_record_stop_processing

DESCRIPTION
  This function stops a MultiMedia recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_mm_record_stop_processing (
  cmx_mm_rec_link_type   link,            /* Direction to record            */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_pause_processing

DESCRIPTION
  This function pauses a MultiMedia recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_mm_record_pause_processing (
  cmx_mm_rec_link_type   link,            /* Direction to record            */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_resume_processing

DESCRIPTION
  This function resumes MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_mm_record_resume_processing (
  cmx_mm_rec_link_type   link,            /* Direction to record            */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_data_processing

DESCRIPTION
  This function records the buffer of MultiMedia data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_mm_record_data_processing (
  cmx_cmd_type              cmd,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  snd_mm_buf_cb_func_type   buf_cb,
  const void                *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_done_processing

DESCRIPTION
  This function cleans up the MultiMedia recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_mm_record_done_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);

/*===========================================================================

FUNCTION cmx_mm_record_status_processing

DESCRIPTION
  This function returns MultiMedia recording status to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_mm_record_status_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);

/*===========================================================================

FUNCTION cmx_mm_record_done_processing

DESCRIPTION
  This function cleans up the MultiMedia recording state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_mm_record_done_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);
#endif /* FEATURE_MM_REC */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_codec_processing

DESCRIPTION
  This function plays audio data whose format must be specified by the user and
  has AV Sync capability.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped
  cmx_af_png_scale

===========================================================================*/
LOCAL void cmx_audfmt_play_codec_processing
(
  cmx_handle_type                  *handle,
  cmx_internal_codec_spec_type     *codec_spec,
  cmx_internal_av_sync_type        *av_sync, 
  cmx_cb_func_ptr_type             cb_func,
  void                             *client_data,
  cmx_interface_enum_type          interface_src
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_avsync_playback_processing

DESCRIPTION
  This function plays audio data whose format must be specified by the user and
  has AV Sync capability.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped
  cmx_af_png_scale

===========================================================================*/
LOCAL void cmx_audfmt_avsync_playback_processing
(
  cmx_handle_type                  *handle,
  cmx_internal_codec_spec_type     *codec_spec,
  cmx_internal_av_sync_type        *av_sync, 
  cmx_audfmt_play_cb_func_ptr_type  cb_func,
  void                             *client_data,
  cmx_interface_enum_type          interface_src
);


/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_processing

DESCRIPTION
  This function plays audio data, whose format is determined by CMX, by parsing
  the data.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped
  cmx_af_png_scale

===========================================================================*/
LOCAL void cmx_audfmt_play_processing
(
  cmx_handle_type                  *handle,
  cmx_af_play_param_type           *param,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_ringer_processing

DESCRIPTION
  This function plays a midi ringer.


DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped
  cmx_af_png_scale

===========================================================================*/
LOCAL void cmx_audfmt_ringer_processing
(
  cmx_handle_type             *handle,
  uint16                      repeat_timer,
  cmx_audfmt_start_enum_type  start_position,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_stop_processing

DESCRIPTION
  This function stops midi/WebAudio playback.


DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_playing
  cmx_af_stopped

===========================================================================*/
LOCAL void cmx_audfmt_stop_processing (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_rewind_processing

DESCRIPTION
  This function rewinds midi/WebAudio playback the indicated number of
  milliseconds.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_rewind_processing
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_fforward_processing

DESCRIPTION
  This function fast forwards midi/WebAudio playback the indicated number
  of milliseconds.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_fforward_processing
(
  uint32               time_ms,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pause_processing

DESCRIPTION
  This function pauses/resumes midi/WebAudio playback.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_pause_processing
(
  boolean              pause,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tempo_processing

DESCRIPTION
  This function modifies the midi/WebAudio playback tempo.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_tempo_processing
(
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_flush_processing

DESCRIPTION
  This functions discards the current data buffers and restarts playing,
  by requesting the CMX client for more data.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_flush_processing
(
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tune_processing

DESCRIPTION
  This function modifies the midi/WebAudio playback tune.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_tune_processing
(
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pan_processing

DESCRIPTION
  This function modifies the playback stereo pan setting.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_pan_processing
(
  uint32               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seek_pause_processing

DESCRIPTION
  This function prepares for a rewind/fast-forward command for
  midi/WebAudio playback.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seek_pause_processing
(
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_stop_processing

DESCRIPTION
  This function stops the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_stop_processing
(
  void                 *sequence,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_pause_processing

DESCRIPTION
  This function pauses/resumes the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_pause_processing
(
  void                 *sequence,
  boolean              pause_flag,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_fforward_processing

DESCRIPTION
  This function fast-forwards the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_fforward_processing
(
  void                 *sequence,
  uint32               ms_forward,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_rewind_processing

DESCRIPTION
  This function rewinds the indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_rewind_processing
(
  void                 *sequence,
  uint32               ms_rewind,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_tempo_processing

DESCRIPTION
  This function modifies the playback tempo of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_tempo_processing
(
  void                 *sequence,
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_tune_processing

DESCRIPTION
  This function modifies the playback tune of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_tune_processing
(
  void                 *sequence,
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_pan_processing

DESCRIPTION
  This function modifies the playback pan of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_pan_processing
(
  void                 *sequence,
  uint16               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seq_vol_processing

DESCRIPTION
  This function modifies the playback vol of indicated sequence.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_seq_vol_processing
(
  void                 *sequence,
  uint16               vol_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_processing

DESCRIPTION
  This function gets the playback time of the indicated midi/WebAudio file.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_get_time_processing
(
  cmx_handle_type                   *handle,
  cmx_audfmt_time_cb_func_ptr_type  cb_func,
  void                              *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_abort_processing

DESCRIPTION
  This function aborts the midi/WebAudio get time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_get_time_abort_processing ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_spec_processing

DESCRIPTION
  This function gets the specifications of a audio file.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_audfmt_get_spec_processing
(
  cmx_handle_type                  *handle,
  cmx_audfmt_spec_cb_func_ptr_type cb_func,
  void                             *client_data
);

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_get_func_processing

DESCRIPTION
  This function retrieves the requested information from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_get_func_processing (
  cmx_cmd_type           cmd,
  cmx_handle_type        *handle,
  uint32                 *length,
  uint8                  *buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_func_processing

DESCRIPTION
  This function sets the cnfg, labl or text chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_set_func_processing (
  cmx_cmd_type           cmd,
  cmx_handle_type        *handle,
  uint32                 length,
  uint8                  *buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_processing

DESCRIPTION
  This function starts recording QCP data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_record_processing (
  cmx_handle_type          *tx_handle,
  cmx_handle_type          *rx_handle,
  cmx_mm_rec_qcp_para_type *rec_param,
  cmx_cb_func_ptr_type     cb_func,
  const void               *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_stop_processing

DESCRIPTION
  This function stops a QCP recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_record_stop_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_pause_processing

DESCRIPTION
  This function pauses QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_record_pause_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_resume_processing

DESCRIPTION
  This function resumes QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_qcp_record_resume_processing (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record_processing

DESCRIPTION
  This function starts recording PCM data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_pcm_record_processing (
  cmx_handle_type        *handle,
  cmx_pcm_rec_para_type  *rec_param,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_stop_processing

DESCRIPTION
  This function stops a PCM recording session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_pcm_record_stop_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_pause_processing

DESCRIPTION
  This function pauses PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_pcm_record_pause_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_resume_processing

DESCRIPTION
  This function resumes PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_pcm_record_resume_processing (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);
#endif /* FEATURE_PCM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)       
/*===========================================================================

FUNCTION cmx_client_buf_req_cb_processing

DESCRIPTION
  This function processes buffer request callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_client_buf_req_cb_processing (
  uint8               *buffer,
  cmx_client_buf_type *buf_handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_client_buf_req_processing

DESCRIPTION
  This function will issue a buffer request command to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_client_buf_req_processing (
  cmx_af_handle_type  *handle,
  cmx_client_buf_type *buf_handle,
  uint32              num_bytes
);
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

/* <EJECT> */
#ifdef FEATURE_CMX_IMAGE
/*===========================================================================

FUNCTION cmx_image_cb_processing

DESCRIPTION
  This function determines when the PNG decoder may continue decoding the
  current image.

DEPENDENCIES
  cmx_image_data_cb_array
  cmx_af_data_cb_array

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_image_data_cb_array
  cmx_png_cont_index

===========================================================================*/
LOCAL void cmx_image_cb_processing (
  cmx_image_enum_type       image_type,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_image_continue_processing

DESCRIPTION
  This function determines when the PNG decoder may continue decoding the
  current image.

DEPENDENCIES
  cmx_png_read_cb_func_ptr
  cmx_af_efs_cb_array

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_png_read_cb_func_ptr
  cmx_png_read_cb_data
  cmx_png_cont_index

===========================================================================*/
LOCAL void cmx_image_continue_processing (
  cmx_image_enum_type       image_type,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
);
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_image_read_processing

DESCRIPTION
  This function gets the requested data from the indicated PNG file.

DEPENDENCIES
  cmx_png_buf_bytes
  cmx_png_buffer
  cmx_png_offset

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_png_read_cb_func_ptr
  cmx_png_read_cb_data

===========================================================================*/
LOCAL void cmx_image_read_processing (
  cmx_image_enum_type       image_type,
  cmx_af_handle_type        *handle,
  uint8                     *buffer,
  uint32                    num_bytes,
  uint32                    position,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
);

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_image_buf_req_processing

DESCRIPTION
  This function will request a buffer from the client for image processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_image_buf_req_processing (
  cmx_image_enum_type       image_type,
  cmx_af_handle_type        *handle,
  uint8                     **buf_handle,
  uint32                    num_bytes,
  cmx_image_command_cb_type cb_func,
  void                      *client_data
);
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX_IMAGE */

/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION cmx_png_decode_processing

DESCRIPTION
  This function starts the PNG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_png_decode_processing (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_abort_processing

DESCRIPTION
  This function aborts the PNG decoder.


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_png_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_spec_processing

DESCRIPTION
  This function gets the specifications of a PNG file.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_png_spec_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION cmx_jpeg_decode_processing

DESCRIPTION
  This function starts the JPEG decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_jpeg_decode_processing (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/*===========================================================================

FUNCTION cmx_jpeg_spec_processing

DESCRIPTION
  This function gets the specifications of a JPEG file.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_jpeg_spec_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_decode_abort_processing

DESCRIPTION
  This function aborts the JPEG decoder.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_jpeg_decode_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);
#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_cleanup

DESCRIPTION
  This function cleans up a SAF decode session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_saf_cleanup (
  cmx_af_handle_type *handle_ptr,
  cmx_status_type    status
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_done_processing

DESCRIPTION
  This function is used to handle SAF frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_saf_frame_done_processing (
  cmx_af_handle_type    *handle_ptr,
  uint8                 *buffer,
  uint32                num_bytes,
  uint32                delay,
  boolean               last_frame
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_cont_processing

DESCRIPTION
  This function continues with frame processing if everything is setup
  properly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_saf_frame_cont_processing (
  cmx_af_handle_type *handle_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_decode_cont_processing

DESCRIPTION
  This function continues with starting SAF decode session.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_saf_offset
  cmx_saf_buf_bytes

===========================================================================*/
LOCAL void cmx_saf_decode_cont_processing (
  cmx_af_handle_type *handle_ptr,
  cmx_saf_ctl_type   saf_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_decode_processing

DESCRIPTION
  This function starts the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_png_offset
  cmx_png_buf_bytes

===========================================================================*/
LOCAL void cmx_saf_decode_processing (
  cmx_handle_type              *handle,
  cmx_saf_ctl_type             saf_ctl,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_abort_processing

DESCRIPTION
  This function aborts the SAF decoder.


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_saf_abort_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_id_cmd_processing

DESCRIPTION
  This function aborts the SAF decoder.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_saf_frame_id_cmd_processing
(
  uint32                 frame_id,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_pause_processing

DESCRIPTION
  This function pauses the SAF decoder.


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_saf_pause_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_resume_processing

DESCRIPTION
  This function resumes the SAF decoder.


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_saf_resume_processing
(
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_info_cont_processing

DESCRIPTION
  This function continues with SAF get info command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_saf_info_cont_processing (
  cmx_af_handle_type *handle_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_info_processing

DESCRIPTION
  This function gets the specifications of a SAF file.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
LOCAL void cmx_saf_info_processing
(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);
#endif /* FEATURE_SAF */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION cmx_adec_read_processing

DESCRIPTION
  This function issues a command to read from a QCP file.

DEPENDENCIES
  cmx_af_efs_cb_array

RETURN VALUE
  None

SIDE EFFECTS
  cmx_af_efs_cb_array

===========================================================================*/
LOCAL void cmx_adec_read_processing (
  cmx_af_handle_type       *handle,
  uint32                   position,
  void                     *client_data,
  snd_af_read_cb_func_type cb_func
);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION cmx_midi_out_processing

DESCRIPTION
  This function issues a command to enable/disable midi API.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_processing
(
  cmx_midi_out_type       enable,
  cmx_cb_func_ptr_type    cb_func,
  cmx_adec_mode_enum_type adec_mode,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_msg_processing

DESCRIPTION
  This function issues midi commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_msg_processing
(
  cmx_handle_type        *handle,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION cmx_midi_out_qcp_reset_processing

DESCRIPTION
  This function issues a command to reset QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_midi_out_qcp_reset_processing (
  cmx_af_handle_type       *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_processing

DESCRIPTION
  This function plays a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_processing
(
  cmx_handle_type               *handle,
  cmx_midi_out_adec_param_type  *param,
  cmx_midi_out_cb_func_ptr_type cb_func,
  void                          *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec_start_chan_processing

DESCRIPTION
  This function starts playback of the indicated audio file that is in
  delayed start mode to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_adec_start_chan_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_stop_processing

DESCRIPTION
  This function stops a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_stop_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_processing

DESCRIPTION
  This function pauses/resumes QCP files to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_pause_processing
(
  boolean                      pause,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_chan_processing

DESCRIPTION
  This function pauses/resumes QCP files to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_pause_chan_processing
(
  boolean                      pause,
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pan_processing

DESCRIPTION
  This function modifies the panning of a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_pan_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  sint15                       pan,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_vol_processing

DESCRIPTION
  This function modifies the volume of a QCP file to MIDI device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_vol_processing
(
  cmx_midi_out_qcp_handle_type *midi_out_handle,
  uint16                       volume,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
#ifdef FEATURE_ADPCM
/*===========================================================================

FUNCTION cmx_midi_out_qcp_reserve_channel_processing

DESCRIPTION
  This function reserves channel 0 of MIDI_OUT_QCP for ADPCM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_midi_out_qcp_reserve_channel_processing (void);
#endif /* FEATURE_ADPCM */
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION cmx_3d_pos_enable_cmd

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_enable_cmd (
  cmx_3d_pos_mode_enum_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_enable_processing

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_enable_processing (
  cmx_3d_pos_mode_enum_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);

/*===========================================================================

FUNCTION cmx_3d_pos_reset_processing

DESCRIPTION
  This function processes a 3D environment reset command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_reset_processing (
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_lsnr_pos_processing

DESCRIPTION
  This function processes a 3D positional listener position command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_pos_processing (
  cmx_3d_pos_lsnr_pos_cmd_type *lsnr_pos,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_lsnr_orient_processing

DESCRIPTION
  This function processes a 3D positional listener orientation command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_orient_processing (
  cmx_3d_pos_lsnr_orient_cmd_type *lsnr_orient,
  cmx_cb_func_ptr_type            cb_func,
  void                            *client_data
);

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_3d_pos_lsnr_vel_processing

DESCRIPTION
  This function processes a 3D positional listener velocity command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_lsnr_vel_processing (
  cmx_3d_pos_lsnr_vel_cmd_type *lsnr_vel,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_processing

DESCRIPTION
  This function processes a 3D Enable Reverb command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_enable_processing (
  boolean                      enable,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_cmd_processing

DESCRIPTION
  This function processes a 3D Set Decay Ratio Reverb command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_cmd_processing (
  cmx_cmd_type                 cmd,
  cmx_3d_pos_param_type       *reverb_param,
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_disable_reverb_processing

DESCRIPTION
  This function processes a 3D Disable Reverb command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_disable_reverb_processing (
  cmx_cb_func_ptr_type         cb_func,
  void                         *client_data
);

/* <EJECT> */

/*===========================================================================

FUNCTION cmx_3d_pos_src_enable_processing

DESCRIPTION
  This function processes a 3D positional enable/disable command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_enable_processing (
  cmx_3d_pos_src_enable_cmd_type *src_mode,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
);

/*===========================================================================

FUNCTION cmx_3d_pos_src_reset_processing

DESCRIPTION
  This function processes a sourcd 3D reset command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_reset_processing (
  cmx_3d_pos_src_reset_cmd_type *src_reset,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
);
/*===========================================================================

FUNCTION cmx_3d_pos_src_vol_processing

DESCRIPTION
  This function processes a 3D positional source volume command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vol_processing (
  cmx_3d_pos_src_volume_cmd_type *src_vol,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_src_vol_atten_processing

DESCRIPTION
  This function processes a 3D positional source volume attenuation command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vol_atten_processing (
  cmx_3d_pos_src_vol_atten_cmd_type *src_vol_atten,
  cmx_cb_func_ptr_type              cb_func,
  void                              *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_src_pos_processing

DESCRIPTION
  This function processes a 3D positional source position command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_pos_processing (
  cmx_3d_pos_src_pos_cmd_type *src_pos,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================
FUNCTION cmx_3d_pos_src_vel_processing

DESCRIPTION
  This function processes a 3D positional source velocity command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_src_vel_processing (
  cmx_3d_pos_src_vel_cmd_type *src_vel,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_reverb_src_vol_processing

DESCRIPTION
  This function processes a 3D positional source Reverb volume command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_reverb_src_vol_processing (
  cmx_3d_reverb_src_vol_cmd_type *src_reverb_vol,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_env_processing

DESCRIPTION
  This function processes a 3D positional environment query command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_env_processing (
  cmx_3d_pos_param_enum_type param,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_src_processing

DESCRIPTION
  This function processes a 3D positional source query command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_src_processing (
  cmx_3d_pos_param_enum_type param,
  void                       *id,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_cb_processing

DESCRIPTION
  This function processes a 3D positional command callback command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_cb_processing (
  cmx_status_type           status,
  cmx_3d_pos_cb_struct_type *cb_struct
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_query_cb_processing

DESCRIPTION
  This function processes a 3D positional query callback command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_3d_pos_query_cb_processing (
  cmx_status_type               status,
  cmx_3d_pos_param_enum_type    param,
  cmx_3d_pos_param_setting_type *setting,
  cmx_3d_pos_cb_struct_type     *cb_struct
);
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
#ifdef FEATURE_AUDIO_POSTCARD
/*===========================================================================

FUNCTION cmx_complete_audiop_processing

DESCRIPTION
  This function completes an audio postcard creation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_complete_audiop_processing
(
  cmx_status_type      status,
  cmx_af_handle_type   *handle,
  uint32               num_bytes,
  uint32               position
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_create_audiop_processing

DESCRIPTION
  This function creates an audio postcard.

DEPENDENCIES
  cmx_af_active

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_af_active

===========================================================================*/
LOCAL void cmx_create_audiop_processing
(
  cmx_handle_type      *jpeg,
  cmx_handle_type      *qcp,
  cmx_handle_type      *output,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);
#endif /* FEATURE_AUDIO_POSTCARD */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmx_af_dls_callback_processing

DESCRIPTION
  This function processes callbacks for DLS processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmx_af_dls_callback_processing (
  snd_af_data_cmd_enum  command,
  cmx_af_handle_type    *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_load_dls_processing

DESCRIPTION
  This function issues commands to load DLS file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_audfmt_load_dls_processing (
  cmx_handle_type       *handle,
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_unload_dls_processing

DESCRIPTION
  This function issues commands to unload DLS resources.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void cmx_audfmt_unload_dls_processing (
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);
#endif /* FEATURE_DLS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_init

DESCRIPTION
  This procedure initializes the CMX command queues.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet

===========================================================================*/
extern void cmx_init(void);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_cmd_parser

DESCRIPTION
  This procedure processes requests received by cmx_cmd().  See cmxi.h for
  a description of the actual CMX packets.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  cmx_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_cmd_q

===========================================================================*/
extern void cmx_cmd_parser(void);

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_frame

DESCRIPTION
  This procedure gets the next SAF frame.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  cmx_saf_frame_timer

===========================================================================*/
extern void cmx_saf_frame(void);
#endif /* FEATURE_SAF */

/* <EJECT> */
#ifdef FEATURE_XMF
/*===========================================================================

FUNCTION cmx_xmf_recognized

DESCRIPTION
  This function performs the necessary setup for a positive XMF file
  recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_recognized (
  void *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_unrecognized

DESCRIPTION
  This function indicates that the XMF module does not recognize the file as
  an XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_unrecognized (
  void *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_free_buff

DESCRIPTION
  This function frees the indicated buffer that was being used by the XMF
  module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_free_buff (
  void  *handle,
  uint8 *buffer
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_get_buff

DESCRIPTION
  This function allocates a buffer to be used by XMF module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 *cmx_xmf_get_buff (
  void   *handle,
  uint32 *length
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_client_read_cb_processing

DESCRIPTION
  This function handles a client data request callback for XMF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_client_read_cb_processing (
  cmx_af_handle_type  *handle,
  cmx_xmf_status_type status,
  uint32              len
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_get_data

DESCRIPTION
  This function reads the indicated data from the XMF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_get_data (
  void   *handle,
  uint8  *buffer,
  uint32 position,
  uint32 *num_bytes,
  void   *state
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_continue_processing

DESCRIPTION
  This function continues XMF processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_continue_processing (
  cmx_af_handle_type *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_continue

DESCRIPTION
  This function queues a command to continue XMF processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_continue (
  void *handle,
  void *state
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_failure

DESCRIPTION
  This function fails the XMF file processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_failure (
  void *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_terminate

DESCRIPTION
  This function terminates the XMF file processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_xmf_terminate (
  cmx_af_handle_type    *handle,
  cmx_af_stop_enum_type stop_reason
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_xmf_load_and_play

DESCRIPTION
  This function loads and/or plays the DLS/MIDI file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
cmx_xmf_status_type cmx_xmf_load_and_play (
  void   *handle,
  uint32 dls_position,
  uint32 smf_position,
  void   *state
);
#endif /* FEATURE_XMF */

/* <EJECT> */
#ifdef FEATURE_SVG
/*===========================================================================

FUNCTION cmx_svg_callback

DESCRIPTION
  This function is used as a callback for the SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_callback (
  svg_cmx_status_type status,
  void*               handle,
  void*               server_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_decode_processing

DESCRIPTION
  This function starts playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_decode_processing(
  cmx_handle_type*                 handle,
  cmx_svg_decode_data_type*         svg_decode,
  cmx_image_cb_func_ptr_type       callback_ptr,
  void*                            client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_abort_processing

DESCRIPTION
  This function aborts playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_abort_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_pause_processing

DESCRIPTION
  This function pauses the playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_pause_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_resume_processing

DESCRIPTION
  This function resumes playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_resume_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_reset_processing

DESCRIPTION
  This function resets the Pan and Zoom factors of a SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_reset_processing
(
  void*                  handle,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_set_viewport_processing

DESCRIPTION
  This function sets the size of viewport of SVG Decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_set_viewport_processing
(
  void*                  handle,
  uint32                 width,
  uint32                 height,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_rewind_processing

DESCRIPTION
  This function rewinds playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_rewind_processing
(
  void*                  handle,
  uint32                 time_ms,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_fforward_processing

DESCRIPTION
  This function fast-forwards playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_fforward_processing
(
  void*                  handle,
  uint32                 time_ms,
  cmx_cb_func_ptr_type   callback_ptr,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_info_processing

DESCRIPTION
  This function reads data from files for external file requests made by 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_info_processing(
  cmx_handle_type*                 handle,
  cmx_svg_decode_data_type*        svg_decode,
  cmx_image_cb_func_ptr_type       callback_ptr,
  void*                            client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_cleanup

DESCRIPTION
  This function frees all resources used by SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_svg_cleanup( 
  cmx_af_handle_type* handle_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_frame_processing

DESCRIPTION
  This function processes Frame commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_svg_frame_processing( 
  cmx_af_handle_type*  handle_ptr,
  uint32               frame_time, /*Rendering time of frame */
  boolean              seek,
  cmx_cb_func_ptr_type callback_ptr,
  void*                client_data
);
 
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_timer_cb

DESCRIPTION
  This function is a callback for frame timer in CMX controlled mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmx_svg_timer_cb( unsigned long counter);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_play_processing

DESCRIPTION
  This function requests frames from SVG decoder after all the required callbacks
  are received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_play_processing(
  cmx_af_handle_type*    handle_ptr,
  cmx_svg_cont_packet_type  *svg_cont
) ;

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_request_processing

DESCRIPTION
  This function opens handle for external file requests made by SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_request_processing( 
  void* handle, 
  cmx_svg_req_pk_type  *svg_req
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_data_processing

DESCRIPTION
  This function reads data from files for external file requests made by 
  SVG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svg_req_data_processing(
  void*           handle,
  uint8*          buffer,
  uint32          num_bytes,
  uint32          position
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_cb_processing

DESCRIPTION
  This function is used as a callback by clients for returning external file
  requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in Graph context
===========================================================================*/
void cmx_svg_req_cb_processing(
  void* handle, 
  uint8** buffer,            /* Buffer with URL data in one chunk */
  uint32 num_bytes
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_free

DESCRIPTION
  This function frees the SVG session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in CMX context                      
===========================================================================*/
void cmx_svg_free( 
  void*                  handle
); 

#ifdef FEATURE_SVGDOM
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_processing

DESCRIPTION
  This function issues a DOM command to SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_processing(
  void* handle,
  cmx_svgdom_api_enum_type type,
  cmx_svgdom_para_type* dom_para
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_free_strings

DESCRIPTION
  This function issues a DOM command to SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_free_strings(
  cmx_af_handle_type* handle_ptr,
  cmx_svgdom_api_enum_type api
);
#endif /* FEATURE_SVGDOM */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION cmx_af_svg_frame_cmd_processing

DESCRIPTION
  This function processes an SVG frame command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_cmd_processing (
  cmx_af_handle_type     *handle_ptr,
  uint32                 frame_id,
  cmx_af_pos_struct_type *frame_pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_svg_frame_buffer_processing

DESCRIPTION
  This function sets up a memory buffer to be used as an SVG frame buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_buffer_processing (
  cmx_af_handle_type *handle_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_af_svg_frame_done_processing

DESCRIPTION
  This function processes a frame for SVG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_af_svg_frame_done_processing (
  cmx_af_handle_type *handle_ptr,
  uint8              *frame_buffer,
  uint32             frame_size
);
#endif /* FEATURE_WEBAUDIO */
#endif /* FEATURE_SVG */

#ifdef FEATURE_AUDFMT_HYBRID_MODE 
/*==========================================================================
 FUNCTION cmx_af_multi_poly_mode_cb_processing

 DESCRPTION
   This function is call back function for the lower layer to pass back the 
   status of hybrid mode command

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_poly_mode_cb_processing(
  cmx_status_type status,
  cmx_af_handle_type *handle
);

/*==========================================================================
 FUNCTION cmx_af_multi_get_poly_mode_cb_processing

 DESCRPTION
   This function is call back function for the lower layer to pass back the 
   current hybrid mode.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_get_poly_mode_cb_processing(
  cmx_status_type status,
  cmx_audfmt_poly_mode_type mode,
  cmx_af_handle_type *handle
);

/*==========================================================================
 FUNCTION cmx_af_multi_poly_mode_processing

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_multi_poly_mode_processing (
  cmx_audfmt_poly_mode_type mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);

/*==========================================================================
 FUNCTION cmx_af_get_multi_poly_mode_processing

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_af_get_multi_poly_mode_processing (
  cmx_audfmt_poly_mode_type *mode,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_AUDIO_PCR_SUPPORT
/*==========================================================================
 FUNCTION cmx_af_adjust_sample_processing

 DESCRPTION
   This function add or drop one sample during audio playback.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_adjust_sample_processing(
  boolean                   Add_Sample,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);
#endif /* FEATURE_AUDIO_PCR_SUPPORT */

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*==========================================================================
 FUNCTION cmx_af_codec_info_update_processing

 DESCRPTION
   This function updates client of cmx with new
   codec information.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_codec_info_update_processing(
  cmx_af_codec_info_type *codec_info,
  cmx_af_handle_type     *handle
);

/*==========================================================================
 FUNCTION cmx_af_dual_mono_mode_processing

 DESCRPTION
   This function sets dual mono channel mode

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_dual_mono_mode_processing(
  cmx_af_dual_mono_mode_enum_type dual_mono_mode,
  cmx_cb_func_ptr_type            cb_func,
  void                            *client_data
);

/*==========================================================================
 FUNCTION cmx_af_sbr_mode_processing

 DESCRPTION
   This function sets sbr mode

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
==========================================================================*/
void cmx_af_sbr_mode_processing(
  cmx_af_sbr_mode_enum_type      sbr_mode,
  cmx_cb_func_ptr_type           cb_func,
  void                           *client_data
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

#endif /* FEATURE_CMX */
#endif /* CMXI_H */

#ifndef SNDTEST_H
#define SNDTEST_H
/*===========================================================================

                 S O U N D   T E S T   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary for the Sound driver to
  be tested using the Diagnostic Monitor. The fields according to the test 
  table can be modified in order to activate all possible sound commands.

REFERENCES
  MSM3100 User's Manual.

Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndtest.h#7 $ $DateTime: 2009/03/10 09:37:19 $ $Author: subhashj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/09    sj      Added slowtalk feature support
12/26/08    rm     RVE API changes
11/17/08   knm     Added flag for aux_line_in in snd_test_block.
10/22/08    dp     FM over BT AG changes
04/15/08    sj     Added sndtest code support for overriding any codec type 
                   with other codec. Added few interface variables in
                   snd_test_block structure to support this using sndtest 
                   code.
11/23/07   vsud    Removed define which was inlcuded by mistake.
 11/22/07  vsud    Removed banned-api sprintf and replaced with std_strlprintf.
11/14/07    hs     Added QEnsemble support
10/26/07    hs     Added EVW file format support
04/20/07    at     Added EVB file format recording test support.
09/05/06    ay     Added AMR-WB+ test support.
                   Updated copyright text.
04/03/06    ak     Added support to test sidetone control.
03/16/06   anb     Modified to support sbc re-configuration given the encoded params
                   CR 78581,87334 
01/30/06    sp     Added support for I2S SBC interface.
10/04/05    sp     Added support for testing SBC DSP Codec
05/27/05    sp     Added support for BT A2DP profile and ARM based SBC codec.
05/24/05    hz     Added support for AAC recording test.
03/18/05   act     Added direct-to-register ADIE control.
02/15/05   act     Added GPIO control block.
12/03/04    sm     Added FEATURE_AUDIO_CALIBRATION
01/30/04    sg     Feature-ized with SND_TEST_CODE so that test code can be
                   enabled by defining SND_TEST_CODE here.
12/08/03    sg     Modified to support changes in the vocoder PCM interface.
05/13/03   ymc     Added d2e_loop.
           ymc     Removed WVS support.
04/17/03    sg     Added PCM interface test code.
09/18/02   ymc     Added FEATURE_MVS.
09/06/02    sg     Added TTY activity status test code.
07/24/01   ymc     Updated Copyright.
                   Used Row 9 of snd_test_block for WVS test functions.
07/14/00   spf     Removed #include "memory.h".
10/26/99   jkl     Added PCM Pad value to write full range.
10/14/99   jkl     Added tests to dfm, audio loop back and TX rate limit.
10/04/99   jkl     Added test to voc_put_pcmpad().
08/17/99   jkl     Created from sndi.h
===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc. */
#include "target.h"     /* Target specific definitions      */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */

#ifdef SND_TEST_CODE

#include "msg.h"        /* Make sure MSG_LEVEL is defined   */
#include "voc.h"        /* Definition  of voc_pb_header_type*/
#include "snd.h"        /* Definitions of filters           */
#include "sndi.h"       /* Definitions of snd op commands   */
#include "sndihw.h"     /* Definitions of snd hw functions  */
#include "sdb.h"        /* Definitions of sdb functions     */ 
#include "AEEstd.h"  /* Definitions of standard functions*/ 

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
 #include "audiosbcenc.h" /* Defintions for audiosbcenc */
#endif
/* <EJECT> */
/*===========================================================================

              SOUND TASK REX SIGNAL DEFINITIONS

===========================================================================*/

/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/* Target requires AVBAP  */
#if ((TG==T_O) || (TG==T_T) || (TG==T_Q2)) 
#define SND_AVBAP_SUPPORTED
#endif

#define PACKET_VERSION_1  "00.00.00"        /* Vocoder Packet Version 1*/

/*===========================================================================

                      REGIONAL FUNCTION DECLARATIONS

===========================================================================*/
typedef struct {
/* Row 1 - Path control */
    byte init_flag;             /* If ZERO we need initialization       */
    byte device;                /* Device - see snddev.h                */
    byte omute;                 /* Output path mute 0-unmute, 1-mute    */
    byte imute;                 /* Input path mute 0-unmute, 1-mute     */
/* Row 2 - Muting */
    byte tx_mute;               /* TXFM Mute = 1 or Unmute = 0          */
    byte rx_mute;               /* Mute = 1 or Unmute = 0 Rx Voice      */
    byte stereo_mute;           /* Mute = 1 or Unmute = 0 car stereo    */
    byte filters;               /* VOICE = 0 or DATA = 1                */
/* Row 3 - Volume */
    byte set_vol;               /* Set volume command                   */
    byte vol_dev;               /* Device for the volume command        */
    byte vol_method;            /* Method for the volume command        */
    byte vol_volume;            /* Volume                               */
/* Row 4 - Play Tone */
    byte tone;                  /* Generate a tone                      */
    byte freq;                  /* Generate tone [freq_high, freq_low]  */
    byte smp_tone;              /* Generate a sample tone               */
    byte smp_tone_vol;          /* Sample tone volume                   */  
/* Row 5 - Tone with freq pair */
    word freq_high;             /* High frequency in Hz of DTMF to play */
    word freq_low;              /* Low frequency in Hz of DTMF to play  */
/* Row 6 - Play Sounds */
    byte sound;                 /* Generate a sound from list of tones  */
    byte sound_flex;            /* Generate a flexible sound from list  */
    byte smp_sound;             /* Generate a sample sound from list    */
    byte smp_sound_vol;         /* Sample Sound volume                  */
/* Row 7 - Additional Test Functions */
    byte snd_id;                /* Sounds from database                 */
    byte smp_snd_id;            /* Sample Sounds from database          */
    byte priority;              /* Priority of the sound                */
    byte stop_last;             /* Stop Last Tone/Multi/Sound           */
/* Row 8 - Sound parameters */
    byte method;                /* method - see snddev.h                */
    byte dtmf_path;             /* Path for sounds                      */
    word duration;              /* Tone duration [ms]                   */
/* Row 9 - MVS or WVS Test */
    byte mvs_loop;              /* Configure MVS loopback type          */
    byte mvs_enable;            /* Activate or de-activate MVS          */
    byte row93;                 /* Unused                               */
    byte mvs_mode;              /* Configure MVS mode                   */
/* Row 10 - External Communications Test */
    byte row101;                 /* 1: Enable comm's, 0: Disable comm's */
    byte tty_activity;           /* Register for TTY activity reports   */
    byte row103;                 /* 0: Trigger pad byte read            */
    byte row104;                 /* Pad byte value                      */
/* Row 11 - VBAP Register Write Test */
    byte row111;                 /* setup i2c port                      */
    byte row112;                 /* write to i2c port                   */
    byte row113;                 /* register                            */
    byte row114;                 /* value                               */
/* Row 12 - VBAP Register Read Test  */
    byte d2e_loop;              /* Decoder to Encoder Loop test         */
    byte row122;                /* read i2c port                        */
    byte row123;                /* register                             */
    byte row124;                /* value                                */
/* Row 13 - QDSP2 driver tests */
    byte config_fm;             /* Config to FM                         */
    byte config_evrc;           /* Config to EVRC                       */
    byte config_13k;            /* Config to 13K                        */
    byte tx_rate;               /* tx_rate                              */
/* Row 14 - QDSP2 driver tests */
    byte pkt_int_loop;          /* Packet internal Loop test            */
    byte dfm_loop;              /* DFM Loop test                        */
    byte pkt_loop;              /* Packet Loop test                     */
    byte audio_loop;            /* Audio Loop test                      */
/* Row 15 - QDSP2 driver tests */
    byte pcm_loop;              /* PCM Loop test                        */
    byte micro_loop;            /* uP Packet Loop test                  */
    byte pcm_pad;               /* Activate PCM pad function            */
    byte pcm_pad_val;           /* PCM pad value                        */
/* Row 16 - PCM Interface tests */
    byte pcm_rx_inp;            /* Feed PCM file data to RX PCM Input   */
    byte pcm_rx_out;            /* Enable and log RX PCM Output         */
    byte pcm_tx_inp;            /* Feed PCM file data to TX PCM Input   */
    byte pcm_tx_out;            /* Enable and log TX PCM Output         */
/* Row 17 - PCM Interface tests */
    byte pcm_int_loop;          /* PCM Interface loopback tests         */
    byte row172;                /*                                      */
    byte row173;                /*                                      */
    byte row174;                /*                                      */

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */
/* Row 25 */ 
    byte aac_rec;
    byte adif_test;
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
    byte audiosbc_write_frame;
    byte audiosbc_write_pcm;
#endif /* FEATURE_SBC_CODEC */
/* Row 26 */
#ifdef FEATURE_AVS_I2SSBC
    byte i2ssbc;
#endif
    byte st_ctl;
#if defined(FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST) && \
    defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
    byte amr_wb_test;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC_TEST && FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_AUDFMT_EVB
    byte evb_rec_op;
/* Row 27 */
    byte evb_rec_format;
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
    byte evw_rec_op;
    byte evw_rec_format;
#endif /* FEATURE_AUDFMT_EVW */

#ifdef FEATURE_AUDIO_QENSEMBLE
#error code not present
#endif /* FEATURE_AUDIO_QENSEMBLE */
    char voc_pac_version[9];     /* Vocoder packet version */

#ifdef SND_TEST_CODEC_OVERRIDE
	byte codec_to_be_override; /* which codec to override */
	byte codec_override;       /* to which codec we need to override */
/* Row 29 */
	byte codec_override_flag;  /* if true then use above to variables to 
								  override codec else if false then disable
	                              earlier codec override for 
								  codec codec_to_be_override */
#endif // SND_TEST_CODEC_OVERRIDE
    byte  smartsnd;			   /* parameter to control agc, rve, avc */

    boolean snd_device_usb_flag; /* flag to set the device as USB */
    boolean aux_line_in_on;      /* flag used to switch on FM     */
    boolean aux_line_in_off;     /* flag used to switch off FM    */

/* Row 30 */
/* FM over BT AG */
#ifdef FEATURE_BT_AG
  byte fm_over_bt_ag;
#endif

  byte    slowtalk_enable_flag;  /* 1 = enable, 0 = disable */
  byte    slowtalk_expansion_ratio; /* 0, 1, 2, 3, 4, 5 */

} snd_test_block_type;

#ifdef FEATURE_AUDIO_CALIBRATION
#error code not present
#endif /* FEATURE_AUDIO_CALIBRATION */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION snd_test_code

DESCRIPTION
  This function provides hardware test code when compiled into a non-production
  version of the system.  This function is hardware dependent.

DEPENDENCIES
  If SND_TEST_CODE is defined, then this routine watches for value poked into
  memory from the Diagnostic Monitor or the In-Circuit emulator and actives 
  the hardware as indicated by the flags.

RETURN VALUE
  None

SIDE EFFECTS
  Values in snd_test_block are clear after they are executed.

===========================================================================*/
extern void snd_test_code
(
  word *change_mask
    /* Pointer to "main loop" Change Flags */
);
/* <EJECT> */
#endif /* SND_TEST_CODE */

#endif /* SNDTEST_H */


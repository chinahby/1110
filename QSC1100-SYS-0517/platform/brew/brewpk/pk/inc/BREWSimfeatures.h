#ifndef BREWSIMFEATURES_H
#define BREWSIMFEATURES_H
/*===========================================================================
FILE:  BREWSimfeatures.h

SERVICES:  Feature Definitions header file for SDK

DESCRIPTION
  This header file provides definitions of the Features used in the SDK or Simulator 

   	   Copyright © 2000-2005 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

//
// The following defines are used to selectively compile the OEM_XXXX.c 
// routines.  If these flags are DEFINED, then we will compile the OEM_XXXX.c
// versions provided as samples.
//

#if !defined(OEMMODTABLE)
	#define OEMMODTABLE
#endif

#if !defined(OEMDB)
	#define OEMDB               // Use OEMDB.c
#endif

#if !defined(OEMSTRINGFUNCS)
	#define OEMSTRINGFUNCS      // Use OEMStringFuncs.c
#endif 

#if !defined(OEMHEAP)
	#define OEMHEAP             // Use OEMHeap.c
#endif 

#if !defined(OEMDISP)
	#define OEMDISP             // Use OEMDisp.c
#endif 

#if !defined(OEMTEXT)
	#define OEMTEXT             // Use OEMText.c
#endif 

/*---------------------------------------------------------------------------
                            CMX  Services
---------------------------------------------------------------------------*/
#define FEATURE_AUDIO_FORMAT
#define FEATURE_MIDI
#define FEATURE_MP3
#define FEATURE_QCP
//#define FEATURE_WEBAUDIO
#define FEATURE_MIDI_OUT
#define FEATURE_MIDI_OUT_QCP
#define FEATURE_SAF
#define FEATURE_ADEC
#define FEATURE_NEW_SYNTH
#define FEATURE_EFS
#define FEATURE_EFS_HOTPLUG
#define FEATURE_CMX
#define FEATURE_DEFAULT_VIEW_APP
#define TG  T_G            
#define TARG_H "target.h"  

// CMX 3.0 defines for BREW...
#define FEATURE_BREW_CMX_V3
#if defined(FEATURE_BREW_CMX_V3)
#define FEATURE_WAVE_PB
#define FEATURE_ADPCM
#define FEATURE_PCM
#define FEATURE_AAC
#define FEATURE_MPEG_AUDIO
#define FEATURE_CMX_IMAGE
#define FEATURE_AUDIO_POSTCARD
#define FEATURE_CMX_RAW_PLAYBACK
#define FEATURE_CMX_LINEAR_PCM
#endif // defined FEATURE_BREW_CMX_3X

#define FEATURE_BREW_CAMERA
#if defined(FEATURE_BREW_CAMERA)
#define FEATURE_CAMERA
#define FEATURE_VIDEO_ENGINE
#define FEATURE_VIDEO_ENCODE
//disable PNG and JPEG encoding for camera simulaton by uncommenting the following
//#define FEATURE_BREW_CAMERA_ENCODE_NO_PNG_JPEG
#endif

#define FEATURE_DEFAULT_VIEW_APP  // Use ViewApp.c as default view applet.

#define FEATURE_DS_SOCKETS
#define FEATURE_DATA
#define FEATURE_DATA_PS_IPV6
#define FEATURE_HDR_BCMCS_2p0
#define FEATURE_DATA_MM
#define FEATURE_DATA_WCDMA_PS

#define FEATURE_BCMCS
#define FEATURE_DATA_BCMCS
#define FEATURE_DTV_DVBH

#define FEATURE_CM

#define FEATURE_MMC

#define FEATURE_BREW_BATTERY

//#define FEATURE_BREW_JOYSTICK

/*---------------------------------------------------------------------------
                                 AEE                               
---------------------------------------------------------------------------*/

/* Enable QCOM extensions
*/
#ifndef FEATURE_QCOM // Launcher defines this with /D flag
  #define FEATURE_QCOM
#endif /* FEATURE_QCOM */

#define FEATURE_UI_DBCS

#define FEATURE_ADDRBOOK

//This is required so that AEEDownload.c will get compiled
//#define FEATURE_APP_MSHOP

/* Enable Calculator
*/
//#define FEATURE_APP_CALC

/* Enable Calendar
*/
//#define FEATURE_APP_CALENDAR

/* Enable Alarmclock, etc.
*/
//#define FEATURE_APP_ALARMCLOCK

/* Enable the MemoPad, etc.
*/
//#define FEATURE_APP_MEMOPAD

/* Enable the Countdown timer
*/
//#define FEATURE_APP_COUNTDOWN

/* Enable the stop watch timer
*/
//#define FEATURE_APP_STOPWATCH

/* Enable 4.1 Browser
*/
//#undef FEATURE_APP_UP4BROWSER

/*
** If neither browser is defined,
** enable the 3.1 browser
*/

#if !defined(FEATURE_APP_UP4BROWSER) && !defined(FEATURE_APP_UPBROWSER)
/* Enable 3.1 Browser
*/
//#define FEATURE_APP_UPBROWSER
#endif

#ifdef FEATURE_APP_UP4BROWSER
/* Enable static UP cache for 4.1 browser
*/
#define FEATURE_APP_UP4BROWSER_STATIC_CACHE

/* Enable packet reassembly for 4.1 browser
*/
#define FEATURE_DS_IP_REASSEMBLY
#endif

/* Enable new Launcher app
*/
//#define FEATURE_APP_LAUNCHER

/* Enable configurable launcher
*/
//#define FEATURE_QCOM_CONFIG_LAUNCHER

/* Enable App Manager */
//#define FEATURE_APP_MANAGER

/* Enable Unzip of streams */
#define FEATURE_UNZIP

/* Enable PNG Viewer */
#define FEATURE_BREW_PNG_DECODE

/* Enable BCI Decode */
#define FEATURE_BCI_DECODE

/* Enable JPEG Decode */
//#define FEATURE_JPEG_DECODER

/* Enable BREW Fonts */
#define FEATURE_BREW_FONTS

/* Enable App Frame */
#define FEATURE_BREW_APPFRAME

/* Enable IBitmap2 stuff */
#define FEATURE_BREW_BITMAPDEV

/* Enable Position Determination services */
#define  FEATURE_POSITION_DETERMINATION

/* Enable GPS Services */
#define	FEATURE_GPSONE

/* Enable Multimedia services */
#define FEATURE_BREW_MULTIMEDIA

#if defined(FEATURE_BREW_MULTIMEDIA)
#define OEMOBJECTMGR
#endif //FEATURE_BREW_MULTIMEDIA

//Enable SIO services
#define FEATURE_BREW_SIO

#define FEATURE_BREW_SEED

#if !defined(FEATURE_REAL_HEAP)
	#define FEATURE_REAL_HEAP
#endif 

/* Enable GIF Decode */
#if !defined(FEATURE_GIF_DECODE)
	//#define FEATURE_GIF_DECODE
#endif

/* Now perform certain Sanity Checks on the various options and combinations
** of option.  Note that this list is probably NOT exhaustive, but just
** catches the obvious stuff.
*/

#if defined( FEATURE_UI_ENHANCED_PRIVACY_SET ) && defined( FEATURE_VOICE_PRIVACY_SWITCH)
#error defined( FEATURE_UI_ENHANCED_PRIVACY_SET ) && defined( FEATURE_VOICE_PRIVACY_SWITCH)
#endif

#if defined(FEATURE_UASMS) != defined(FEATURE_UI_SMS)
#error defined(FEATURE_UASMS) != defined(FEATURE_UI_SMS)
#endif

#if defined(FEATURE_UI_TIME_YES) && defined(FEATURE_ACP)
#error defined(FEATURE_UI_TIME_YES) && defined(FEATURE_ACP)
#endif

#if defined(FEATURE_DS_AMPS) && !defined(FEATURE_ACP)
#error defined(FEATURE_DS_AMPS) && !defined(FEATURE_ACP)
#endif

#if defined(FEATURE_UI_FORCE_CALL) && !defined(FEATURE_ACP)
#error defined(FEATURE_UI_FORCE_CALL) && !defined(FEATURE_ACP)
#endif

#if defined(FEATURE_UI_SEND_PIN) && !defined(FEATURE_ACP)
#error defined(FEATURE_UI_SEND_PIN) && !defined(FEATURE_ACP)
#endif

#if defined(FEATURE_UI_IMSI_ALL_0_OK) && defined(FEATURE_OTASP)
#error defined(FEATURE_UI_IMSI_ALL_0_OK) && defined(FEATURE_OTASP)
#endif

#if (defined(FEATURE_OTASP_OTAPA) && !defined(FEATURE_OTASP)) || \
    (defined(FEATURE_OTASP_OTAPA) && defined(FEATURE_NSOTASP)) || \
    (defined(FEATURE_NSOTASP) && !defined(FEATURE_OTASP))
#error Wrong feature combination of OTASP, NSOTASP and OTASP_OTAPA
#endif

#if (defined(FEATURE_OTASP_HOLD_CMD_IN_COMMIT) && !defined(FEATURE_OTASP)) || \
    (defined(FEATURE_OTASP_HOLD_CMD_IN_COMMIT) && defined(FEATURE_NSOTASP)) 
#error Wrong feature combination of OTASP, NSOTASP and OTASP_HOLD_CMD_IN_COMMIT
#endif

#if defined(FEATURE_PREFERRED_ROAMING) && \
    !defined(FEATURE_CDMA_1900) && !defined(FEATURE_CDMA_800)
#error !defined(FEATURE_CDMA_1900) && !defined(FEATURE_CDMA_800)
#endif

#if !defined(FEATURE_PRL_FORCE_MODE) && ( defined(FEATURE_CDMA_800) || defined(FEATURE_ACP) )
#error !defined(FEATURE_PRL_FORCE_MODE) && ( defined(FEATURE_CDMA_800) || defined(FEATURE_ACP) )
#endif

/* Required & Invalid features for Tri-mode */
#if defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)

#if !defined(FEATURE_IS95B)
#error !defined(FEATURE_IS95B) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_SSPR_800)
#error !defined(FEATURE_SSPR_800) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if defined(FEATURE_NSOTASP)
#error defined(FEATURE_NSOTASP) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_PREFERRED_ROAMING)
#error !defined(FEATURE_PREFERRED_ROAMING) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_SSPR_ENHANCEMENTS)
#error !defined(FEATURE_SSPR_ENHANCEMENTS) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_IS683A_PRL)
#error !defined(FEATURE_IS683A_PRL) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_PRL_FORCE_MODE)
#error !defined(FEATURE_PRL_FORCE_MODE) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_HOME_ONLY)
#error !defined(FEATURE_HOME_ONLY) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_MAX_ACCESS_FALLBACK)
#error !defined(FEATURE_MAX_ACCESS_FALLBACK) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if !defined(FEATURE_CLASS_1_IMSI)
#error !defined(FEATURE_CLASS_1_IMSI) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#if defined(FEATURE_AVAIL_HOME_SIDE_PREF)
#error defined(FEATURE_AVAIL_HOME_SIDE_PREF) && defined(FEATURE_CDMA_800) && defined(FEATURE_CDMA_1900)
#endif

#endif /* defined(FEATURE_CDMA_800 && defined FEATURE_CDMA_1900 - Tri-mode */

// Enable the following definition to support DebugApp
#define FEATURE_AEE_DEBUGSET
//For the merged PK/SDK simulator
#ifdef FEATURE_COMMERCIAL_SIMULATOR
#define FEATURE_NO_BKEY
#define FEATURE_NO_SIGCHECK
#define FEATURE_NO_COOKIES
#endif

#define FEATURE_JCDMA_2

// For QWERTY keypad support
#define FEATURE_EXTENDED_KEYS

//#define FEATURE_KEYS_CONFIG

// FEATURE_NO_HAT is used to disable the HAT feature (Hysteresis Activation Timer)
//#define FEATURE_NO_HAT

#endif /* BREWSIMFEATURES_H */


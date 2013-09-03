#ifndef OEMFEATURES_H
#define OEMFEATURES_H
/*======================================================
FILE:  OEMFeatures.h

SERVICES:  AEE Features

GENERAL DESCRIPTION:
   Temporary file.  Compiler options for AEE

        Copyright ?1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#if defined AEE_SIMULATOR
#include "BREWSimfeatures.h"  // Simulator Feature Definitions
#else
#if !defined(_ARM_ASM_)
#include "comdef.h"
#endif
#if defined(CUST_H) || defined(CUSTFILE) || defined(CUSTH)
#include "target.h"
#include "customer.h"         //Handset Feature Definitions
#endif
#endif
#include "BREWVersion.h"
#include "mmidispsize.h"
//#define FEATURE_PEKTEST
#ifndef FEATURE_VERSION_K202
#ifndef FEATURE_VERSION_K212_12832
#define FEATURE_GREYBIT
#endif
#endif
#define FEATURE_BREW_3_0

//
// The following defines are used to selectively compile the OEM_XXXX.c
// routines.  If these flags are DEFINED, then we will compile the OEM_XXXX.c
// versions provided as samples.
//
#if !defined(OEMDB)
   #define OEMDB               // Use OEMDB.c
#endif

#if !defined(OEMAPPFUNCS)
   #define OEMAPPFUNCS         // Use OEMAppFuncs.c
#endif //

#if !defined(OEMCONFIG)
   #define OEMCONFIG           // Use OEMConfig.c
#endif //

#if !defined(OEMSTRINGFUNCS)
   #define OEMSTRINGFUNCS      // Use OEMStringFuncs.c
#endif

#if !defined(OEMHEAP)
   #define OEMHEAP             // Use OEMHeap.c
#endif

#if !defined(OEMDISP)
   #define OEMDISP             // Use OEMDisp.c
#endif

#ifndef FEATURE_MANGO_BREW
#if !defined(OEMTEXT)
   #define OEMTEXT             // Use OEMText.c
#endif
#endif

#if !defined(OEMSMS)
   //#define OEMSMS              // Use OEMSMS.c
#endif

#if !defined(OEMWMS)
#define OEMWMS                // Use OEMWMS.c
#endif

#if !defined (FEATURE_WCDMA) && !defined (FEATURE_GSM) \
    && !defined(FEATURE_CDSMS)
#define OEMPHONE
#define OEMSMS                // Use OEMSMS.c
#endif /* !FEATURE_WCDMA && !FEATURE_GSM && !FEATURE_CDSMS */

#ifndef FEATURE_MANGO_BREW
#ifndef FEATURE_USES_LOWMEM
#if !defined(OEMPRIVACY)
   #define OEMPRIVACY            // Use OEMPrivacy.c
#endif /*FEATURE_W021_LOW_MEMORY*/
#endif

#if !defined(OEMRINGERS)
   #define OEMRINGERS            // Use OEMRingers.c
#endif
#endif
#if !defined(FEATURE_OEMSVC)
   #define FEATURE_OEMSVC          // Use OEMSVC.c
#endif

#if !defined(OEMMODTABLE)
   #define OEMMODTABLE           // Use OEMModTable.c
#endif

#if !defined( AEE_SIMULATOR)
#ifndef FEATURE_MANGO_BREW
#if defined FEATURE_RUIM_PHONEBOOK
//#define FEATURE_IRUIM
#define FEATURE_ADDRBOOK_RUIM
#endif
#endif
#define FEATURE_ICARD         // ICARD Extension
#endif

#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_CALLHISTORY)
#define FEATURE_CALLHISTORY   // Use OEMCallHistory.c
#endif
#endif

// ZiCorp Simplified Chinese Stroke mode
//#define FEATURE_ZICORP_STROKE // Use OEMIMM.c and OEMIMUI.c
// ZiCorp Simplified Chinese Pinyin mode
//#define FEATURE_ZICORP_PINYIN // Use OEMIMM.c and OEMIMUI.c

// These enable the default language to be specified in
// the text control when AEE_TM_EZTEXT is given
// This is not the full integration of the specified language
// To do that a new mode would need to be added for the multitap
// in that language.
// Optionally you would support the AEE_TM_*_R mode to let the user
// specify the language rapid mode needed.
//#define FEATURE_ZICORP_EZI_EN // Enables eZi English
//#define FEATURE_ZICORP_EZI_TH // Enables eZi Thai
//#define FEATURE_ZICORP_EZI_IT // Enables eZi Italian
//#define FEATURE_ZICORP_EZI_FR // Enables eZi French
//#define FEATURE_ZICORP_EZI_BP // Enables eZi Brasilian Portuguese
//#define FEATURE_ZICORP_EZI_PR // Enables eZi Portuguese
//#define FEATURE_ZICORP_EZI_ES // Enables eZi Spanish

// No need to modify this block, it depends on the features above
#if defined(FEATURE_ZICORP_EZI_EN) || defined(FEATURE_ZICORP_EZI_TH) \
    || defined(FEATURE_ZICORP_EZI_IT) || defined(FEATURE_ZICORP_EZI_FR) \
    || defined(FEATURE_ZICORP_EZI_BP) || defined(FEATURE_ZICORP_EZI_PR) \
    || defined(FEATURE_ZICORP_EZI_ES)
#define FEATURE_ZICORP_EZITEXT // ZiCorp eZiText needed
#endif

// No need to modify this block, it depends on the features above
#if defined(FEATURE_ZICORP_STROKE) || defined(FEATURE_ZICORP_PINYIN) // {
#define FEATURE_ZICORP_CHINESE   // A ZiCorp Chinese method is selected
#ifndef FEATURE_BREW_IME // {
#define FEATURE_BREW_IME         // IME support for Chinese mode
#endif // } !FEATURE_BREW_IME
#endif // } FEATURE_ZICORP_STROKE || FEATURE_ZICORP_PINYIN

// Applications supported
#ifndef FEATURE_MANGO_BREW
#ifdef USES_BREW_APPMGR
#define FEATURE_APP_MANAGER
#endif
#endif

#ifndef FEATURE_USES_LOWMEM
#define FEATURE_USES_ADVANCEAPI
#endif

#ifdef FEATURE_UI_CORE

#define FEATURE_APP_UI     /* UI Core App */
#ifndef FEATURE_MANGO_BREW
#define FEATURE_ADDRBOOK    /* BREW Address Book */
#define FEATURE_IVECTOR     /* IVector Interface */

#define FEATURE_ANNUNCIATOR

#ifndef FEATURE_USES_LOWMEM
#ifndef FEATURE_BREW_DOWNLOAD
#define FEATURE_BREW_DOWNLOAD
#endif
#endif

#endif

#ifndef WIN32
#if defined(FEATURE_CAMERA) || defined(FEATURE_DSP)
#define FEATURE_ICAMERA    /* ICAMERA Extension */
#define FEATURE_BREW_CAMERA
#endif /* FEATURE_CAMERA */
#endif

#ifndef FEATURE_MANGO_BREW
#ifdef FEATURE_JOYST_TASK
#define FEATURE_IJOYSTICK
#define FEATURE_BREW_JOYSTICK
#endif /* FEATURE_JOYSTICK */
#endif
#endif /* FEATURE_UI_CORE */

#ifdef FEATURE_PHONE_VR
#define FEATURE_IVR
#endif

// You must include "ViewApp.c" in the build for this to be enabled.

//#define FEATURE_DEFAULT_VIEW_APP  // Use ViewApp.c as default view applet.
#ifndef FEATURE_MANGO_BREW
#ifndef FEATURE_USES_LOWMEM
#define FEATURE_GIF_DECODE
#define FEATURE_BCI_DECODE
#endif
#define FEATURE_BREW_PNG_DECODE
#endif

//#define FEATURE_JPEG_DECODER
//#define FEATURE_QCT_JPEG_DECODER

#ifdef FEATURE_MOBILE_SUITE
#define FEATURE_IMIC
#endif  /* FEATURE_MOBILE_SUITE */

#if defined(FEATURE_DYNAMIC_OEM_HEAP)
#ifdef OEM_STATIC_HEAP_SIZE
#undef OEM_STATIC_HEAP_SIZE
#endif
#else /* FEATURE_DYNAMIC_OEM_HEAP */
#if defined(OEMHEAP) && !defined(OEM_STATIC_HEAP_SIZE) && !defined(AEE_SIMULATOR)
#if defined(FEATURE_CAMERA) && defined(FEATURE_CAMERA_VGA)
#if defined(SHADOW_MODE)
#define OEM_STATIC_HEAP_SIZE  0x1F0000 //0x240000  // 2.25MB for VGA (640x480)
#else /* defined(SHADOW_MODE) */
#define OEM_STATIC_HEAP_SIZE  0x1F0000  // 3MB for VGA (640x480)
#endif /* defined(SHADOW_MODE) */
#elif defined(FEATURE_CAMERA)
#define OEM_STATIC_HEAP_SIZE  0x1F0000  // 1.25MB for CIF (352x288)
#elif defined(FEATURE_JAVA)
#define OEM_STATIC_HEAP_SIZE  0x1F0000  // 1.25MB for Java
#elif defined(FEATURE_APP_MPEG4)
#define OEM_STATIC_HEAP_SIZE  0x140000  // 1.25MB
#else
#define OEM_STATIC_HEAP_SIZE  0x140000  // 1.25MB
#endif // FEATURE_APP_MPEG4
#endif
#endif /* FEATURE_DYNAMIC_OEM_HEAP */

#ifdef FEATURE_ENABLE_BREW_HEAPTRACKER
#define FEATURE_BREW_HEAP_TRACKER
#define FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
#define FEATURE_BREW_HEAP_INIT_MEM_ON_FREE
#define FEATURE_BREW_HEAP_OVERWRITE_DETECT
#endif  // ENABLE_BREW_HEAPTRACKER


//
// Linked AEE Libraries...
//
#ifndef FEATURE_MANGO_BREW
#define AEEMENU_LIB     1
#define AEECONTROLS_LIB 1
#define AEEDB_LIB       1

#ifdef FEATURE_USES_ADVANCEAPI
#define FEATURE_BREW_AEENET
#define FEATURE_BREW_AEETHREAD
#define FEATURE_BREW_AEEFIFO
#define FEATURE_BREW_PROVISION
#define FEATURE_BREW_LICENSE
#define FEATURE_BREW_AEEAPPHISTORY
#define FEATURE_BREW_AEEAPPLETCTL
#endif
//
// Feature Libraries
//
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_VOICE_MEMO)
   #define FEATURE_SOUNDPLAYER  1
#endif
#ifdef FEATURE_USES_ADVANCEAPI
#ifdef FEATURE_APP_MANAGER
#define FEATURE_BTIL       1
#endif

#define FEATURE_GRAPHICS   1
#endif
#endif

#define FEATURE_BREW_SOUND 1

#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
#define FEATURE_BREW_MULTIMEDIA
#define FEATURE_CMX_RAW_PLAYBACK
#define FEATURE_CMX_AV_SYNC
#ifdef FEATURE_PCM
#define FEATURE_CMX_LINEAR_PCM
#endif /* FEATURE_PCM */
#endif // defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)

#if defined(FEATURE_BREW_MULTIMEDIA) || defined(FEATURE_GPSONE)
#define OEMOBJECTMGR
#endif // defined(FEATURE_BREW_MULTIMEDIA) || defined(FEATURE_GPSONE)
#if defined(FEATURE_VERSION_W208S) || defined(FEATURE_PEKTEST)
#else
//Disable for QSC1110
//#ifndef FEATURE_LOW_MEMORY_USAGE
#ifdef FEATURE_USES_ADVANCEAPI
#define FEATURE_HTMLVIEWER
#define FEATURE_UNZIP
#endif
//#endif
#endif

#if (defined(FEATURE_PDAPI) || defined(FEATURE_GPSONE)) && defined(FEATURE_GPSAPP)
#define FEATURE_POSITION_DETERMINATION
#define FEATURE_IPOSDETEXT
#endif /* FEATURE_PDAPI || FEATURE_GPSONE */

//Disable for QSC1110
//#define FEATURE_BREW_WEB
#ifndef FEATURE_GREYBIT
#ifndef FEATURE_MANGO_BREW
//#define FEATURE_BREW_FONTS
#endif
#endif
#define FEATURE_BREW_TELEPHONE
#ifndef FEATURE_MANGO_BREW
#define FEATURE_BREW_APPFRAME
#endif
#define FEATURE_BREW_BITMAPDEV

#define FEATURE_BREW_TAPI

//Disable for QSC1110 mainstream
#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_LOW_MEMORY_USAGE)
#define FEATURE_IVOCODER
#endif
#endif

#ifdef FEATURE_USES_ADVANCEAPI
#define FEATURE_BREW_SECURITY       // turn on hashes, ciphers and random numbers
#define FEATURE_BREW_RSA            // Turn on RSA; needed for sig verify, SSL & IX509Chain
#define FEATURE_BREW_SEED
#if defined(FEATURE_BREW_SECURITY)
   #define FEATURE_BREW_DES3DES     // define this to turn on DES and 3DES in BREW
#endif
  #define FEATURE_BREW_ROOTCERTS // Turn on WebOpts based access to root certs
#endif

//Disable for QSC1110
//#define FEATURE_BREW_SSL            // turn on BREW SSL
//#define FEATURE_BREW_SSLROOTCERTS   // turn on BREW SSL Root Certificates
#ifndef FEATURE_MANGO_BREW
#ifdef FEATURE_USES_ADVANCEAPI
#define FEATURE_BREW_ROOTCERTS // Turn on WebOpts based access to root certs
#endif
#endif

#ifndef FEATURE_MANGO_BREW
#ifndef FEATURE_LOW_MEMORY_USAGE
#define FEATURE_BREW_RAMCACHE
#define FEATURE_BREW_SPRITE
#endif
#endif
//Disable for QSC1110
//#define  FEATURE_LOGGER_FILE


//Disable for QSC1110
//#ifdef FEATURE_BTIL
//#define  FEATURE_LOGGER_BTIL
//#endif

//Disable for QSC1110
//#define  FEATURE_LOGGER_DMSS
//#if defined(FEATURE_LOGGER_FILE) || defined(FEATURE_LOGGER_SERIAL) || defined(FEATURE_LOGGER_DMSS) || defined(FEATURE_LOGGER_WIN) || defined(FEATURE_LOGGER_BTIL)
//#define  FEATURE_LOGGER
//#endif

#ifdef FEATURE_USES_ADVANCEAPI
  #define FEATURE_BREW_KEYMAKER // Used by BREW's Mutual Auth for BREW Keys and Credentials
#endif

#define FEATURE_WMS

// Enable Top Visible Application Priorities
#ifndef FEATURE_MANGO_BREW
#define FEATURE_TOPVISIBLE
#endif
// Include the reference Toucan 16-bit display driver
#define FEATURE_DISP_TOUCAN

#define BREW_OFFSET_X  0
#ifdef FEATURE_UIONE_HDK
#define BREW_OFFSET_Y  30
#else
#define BREW_OFFSET_Y  40
#endif

// Include Overlay feature
#if defined(FEATURE_MDP)
#define FEATURE_OVERLAY
#endif

// Features to deal with changes in DMSS functions and structures
#define FEATURE_BREW_CMX_V3
#define FEATURE_BREW_COMPLETE_SYSPARAMS
//#define FEATURE_BREWAPI_SPECIFIC

#if defined(FEATURE_GPSONE) || defined (FEATURE_PDAPI)
// Mods for PDSM API.
#define FEATURE_PDSM_API_R2
#endif

//Enable the following definition to include GSM1x support.
//#define FEATURE_GSM1x

//Disable for QSC1110
//#if !defined(FEATURE_LOW_MEMORY_USAGE)
// Do not enable BREW SIO for small footprint targets
//#define FEATURE_BREW_SIO
//#define FEATURE_BREW_SIO_NEW_RDM
//#endif

#if defined(FEATURE_BT)
#define FEATURE_IBT
#endif /* FEATURE_BT */

#define FEATURE_BREW_SMS 1
#define FEATURE_BREW_BROADCAST_SMS
#define FEATURE_BREW_RESCTL

// Disabled image scaling support for now
#ifndef FEATURE_PEKTEST
#ifndef FEATURE_USES_LOWMEM
#define FEATURE_BREW_SCALE       // turn on image scaling in BREW viewers
#endif
#endif

#define FEATURE_DEV_NOTIFIER
#ifndef FEATURE_MANGO_BREW
#ifdef FEATURE_USES_ADVANCEAPI
#define FEATURE_BREW_NATIVENOTIFIER

#define FEATURE_BREW_NETMTPDNOTIFIER
#endif
#endif
#define FEATURE_BACKLIGHT

// Disabled BREW Debugger
//#ifndef FEATURE_BREW_DEBUGGER // turn on BREW Debugger capability
//#define FEATURE_BREW_DEBUGGER
//#endif

// Disabled QOS for now
//#define FEATURE_BREW_QOS

// Disabled codesig and fota features for the SC1X for now
//#define FEATURE_CODESIG      // required for ifota
//#define FEATURE_IFOTA        // ifota feature is turned on.
//#define REFERENCE_IFOTA_IMPL // turn on reference ifota implementation.

#define FEATURE_SECURE_CLOCK

// Enable the following definition if the handset uses uixsnd.c to implement
// Sound APIs
#define FEATURE_UIXSND_IMPLEMENTATION

//#define FEATURE_OEMSOUND_NATIVE_IF

//Disable for QSC1110
//#if defined(FEATURE_USB_BREW_EXTENDED_DEVICE)
//   #define FEATURE_BREW_USBDRIVER
//#endif
/* Checks for invalid feature combos */

#if MIN_BREW_VERSIONEx(3,1,4) && defined(FEATURE_BRIDLE)
#error MIN_BREW_VERSIONEx(3,1,4) && defined(FEATURE_BRIDLE)
#endif

#if MIN_BREW_VERSIONEx(3,1,4) && defined(FEATURE_BRIDLE_CAMERA)
#error MIN_BREW_VERSIONEx(3,1,4) && defined(FEATURE_BRIDLE_CAMERA)
#endif

// Enables IBATTERY interface
#if defined (FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK)
#define FEATURE_BREW_BATTERY
#endif

//Enable the following to support IFLIP interface
//#define FEATURE_IFLIP

// Enable the following definition to support DebugApp
#ifndef FEATURE_MANGO_BREW
#define FEATURE_AEE_DEBUGSET
#endif

// Enable the following definition to forcibly close all TCP sockets
// when the data network closes.
// Some platforms do not close TCP connections when the network goes down
// and so the OEM layer must close these connections. Enable this feature
// on those targets.
// This feature should be disabled for platforms which close the TCP
// connections when the network goes down.
#define FEATURE_CLOSE_TCP_SOCKETS_ON_NETWORK_CLOSE

// Enable the following definition to close PPP when the device is out of
// network coverage.
#ifndef FEATURE_MANGO_BREW
#ifndef FEATURE_WLAN
#ifndef FEATURE_USES_LOWMEM
  #define CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE
#endif
#endif //FEATURE_WLAN
#endif
// Disable vixenUI
#define FEATURE_NO_OEMCALLCTRL

#ifdef CUST_EDITION

#define FEATURE_IRUIM

#define FEATURE_BREW_EXTENDED_MODTABLE

#if !defined( AEE_SIMULATOR)
    #define FEATURE_APP_DIALER
#ifndef FEATURE_USES_LOWMEM	
    #define FEATURE_ICM
#endif	
    #undef FEATURE_APP_RECENTCALLS
    #define FEATURE_CALLLIST
#endif

#ifndef FEATURE_USES_LOWMEM
#define FEATURE_APP_SVCPRG
#endif

#if !defined( FEATURE_GRAPHICS)
//#define FEATURE_GRAPHICS
#endif//#if !defined( FEATURE_GRAPHICS)
#if !defined( FEATURE_BREW_PNG_DECODE)
#define FEATURE_BREW_PNG_DECODE
#endif//#if !defined( FEATURE_BREW_PNG_DECODE)
#if !defined( FEATURE_GAME_TETRIS)
//#define FEATURE_GAME_TETRIS
#endif//#if !defined( FEATURE_GAME_TETRIS)

#undef FEATRUE_SUPPORT_G_SENSOR
#ifdef CUST_EDITION

#if !defined(FEATRUE_SUPPORT_G_SENSOR)
#ifndef FEATURE_USES_LOWMEM
#ifndef FEATURE_VERSION_K212
#define FEATURE_GAME_BRICKATTACK
#define FEATURE_GAME_BLACKJACK
#endif
#endif
#endif
#endif /*CUST_EDITION*/

#define FEATURE_CUSTOMIZED_MENU_STYLE

#define FEATURE_UIALARM
#define FEATURE_APP_UTK
#ifdef USES_CAMERA
#define FEATURE_APP_CAMERA
#endif

#define FEATURE_CUSTOMIZED_STATIC_STYLE
//#define FEATURE_CONTROL_BG_USE_IMAGE
//#define FEATURE_COLOR_BAR_USE_IMAGE

#if !defined(AEE_SIMULATOR)
#ifndef FEATURE_USES_LOWMEM
	#define FEATURE_APP_FLDDBG
#endif /*FEATURE_W021_LOW_MEMORY*/
	#define FEATURE_CONTACT_APP
#endif
#define FEATURE_FUNCS_BOTTOM_BAR
#define FEATURE_FUNCS_TITLE_BAR
//#define FEATURE_FUNCS_THEME
#define FEATURE_MENUTITLE_AUTOSCROLL
/*define OEMCFGI FEATURE*/
#ifndef FEATURE_USES_LOWMEM
#define FEATRUE_AUTO_POWER
#endif
#define FEATURE_TIME_DATA_SETTING
#define FEATURE_KEYGUARD
#ifndef FEATURE_USES_LOWMEM
#define FEATURE_RECORDER
#define FEATURE_DOUBLE_SIM_CARD
#endif/*FEATURE_W021_LOW_MEMORY*/

//#define FEATURE_SET_AUTOKEYGUARD
//#define FEATURE_ANIMATION_POWERUPDOWN
//#define FEATURE_SCREEN_SAVE

//wangliang modify!
//#define FEATURE_RESERVEDMSG
#ifndef FEATURE_VERSION_CITYCELL
#define FEATURE_LANGUAGE
#endif
#ifdef FEATURE_SUPPORT_BT_BCM
#define FEATURE_SUPPORT_BT_APP
#endif //FEATURE_SUPPORT_BT_BCM

#ifndef CUST_EDITION
#define FEATURE_SUPPORT_BUIW
#endif

#undef FETURE_HW_ON

#ifndef WIN32
#if !defined(FEATURE_CARRIER_TFL)
#ifndef CUST_EDITION
#define FEATURE_SUPPORT_WAP_APP
#endif /*CUST_EDITION*/
#endif
#if !defined(FEATURE_CARRIER_CHINA_VERTU) && !defined(FEATURE_CARRIER_TAIWAN_APBW)
#define FEATURE_SUPPORT_BREW_MOBILESHOP
#endif
#endif//WIN32

#ifndef FEATURE_USES_LOWMEM
#define FEATURE_APP_MEDIAGALLERY
#define FEATURE_APP_MUSICPLAYER
#define FEATURE_APP_QUICKTEST
#define FEATURE_APP_GAME
#define FEATURE_APP_APPLICATION
#define FEATURE_APP_MULTIMEDIA
#define FEATURE_CONVERTER
#define FEATURE_APP_WORLDTIME
#define FEATURE_APP_STOPWATCH
#define FEATURE_APP_TIMERAPP
#define FEATURE_APP_SCHEDULE
#define FEATURE_APP_CALC
#if defined FEATURE_APP_MPEG4
#define FEATURE_VIDEOPLAYER
#endif
#if !defined(FEATURE_CARRIER_CHINA_VERTU)
#define  FEATURE_APP_CALENDAR
#endif
#endif/*FEATURE_W021_LOW_MEMORY*/

#define FEATURE_AUTOEXIT_AFTER_BLDISABLE
#ifdef FEATRUE_SUPPORT_G_SENSOR
#define FEATURE_SPORTS_APP
#endif
//#define FEATURE_SPORTS_APP


#ifdef FEATURE_APP_MUSICPLAYER
#define FEATURE_MUSICPLAYER_RANDOM
#define FEATURE_MUSICPLAYER_LIST_INDEX_DISPLAY
#endif
//#define FEATURE_CONVERTER
#if (defined(FEATURE_CARRIER_TAIWAN_APBW) )\
    ||(defined(FEATURE_CARRIER_THAILAND_CAT) )\
    ||(defined(FEATURE_CARRIER_THAILAND_HUTCH) )\
    ||(defined(FEATURE_CARRIER_ISRAEL_PELEPHONE) )\
    ||(defined(FEATURE_CARRIER_INDONESIA) )\
    ||(defined (FEATURE_CARRIER_SUDAN_SUDATEL))
    #undef FEATRUE_SET_IP_NUMBER
#endif
#define FEATRUE_SET_ANN_FULL_SCREEN
//#define FEATURE_MENU_STYLE

//#define FEATURE_CAMERAAPP_FUN_FRAME
#define FEATURE_VIBRATOR

//wangliang close!  2010-06-05
//#define FEATURE_PLANEMODE

#define FEATURE_BREW_ANNUNCIATOR_CONTROL

#ifdef FEATURE_CDG2_TEST
    #define FEATURE_OEM_DEBUG /*this feature only for output oem message from QXDM*/
#endif
#ifndef FEATURE_VERSION_K212
#define FEATURE_POWERDOWN_ALARM
#endif
#define FEATURE_SCROLLBAR_USE_STYLE


#if !defined(FEATURE_CARRIER_CHINA_VERTU)
//#define FEATURE_RANDOM_MENU_COLOR
#endif

#ifdef FEATURE_APP_SCHEDULE
#define FEATURE_CALENDAR_USE_STYLE
#endif

#define FEATURE_EDITABLE_RECORD
#undef FEATURE_APP_RECORDER
#define FEATURE_SUPPORT_VCARD
#ifdef FEATURE_SUPPORT_WAP_APP

#ifdef FEATURE_CARRIER_CHINA_TELCOM 
#define FEATURE_CALL_E_HALL_BY_WAP
#define FEATURE_ESURFING_BESTTONE_WAP
#define FEATURE_ESURFING_SUPPORT_VNET
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/

#endif

#ifdef FEATURE_CARRIER_CHINA_TELCOM
#define FEATURE_ESURFING_SUPPORT_DOWNLOAD
#define FEATURE_ESURFING_SUPPORT_UTK
#define FEATURE_ESURFING_SUPPORT_ITV
#define FEATURE_ESURFING_SUPPORT_OFFICE
#define FEATURE_ESURFING_SUPPORT_MEGAEYE
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/
#ifdef FEATURE_CARRIER_CHINA_TELCOM
#define FEATURE_SAVE_ONLY_NAME
#define FEATURE_CHECK_SAME_NAME
#define FEATURE_GIVEUP_EDIT
#endif
//#define     FEATURE_PEKTEST

//wlh add for LCD touch
//#define FEATURE_LCD_TOUCH_ENABLE

//wlh 20090405 add 
#ifdef USES_REND
#define FEATURE_RANDOM_MENU_REND
#endif
#endif/*CUST_EDITION*/

#ifndef FEATURE_MANGO_BREW
#define FEATURE_BREW_ANNUNCIATOR_CONTROL
#endif
#define FEATURE_BREW_DETECT_MP3
#define FEATURE_BREW_DETECT_AAC
#endif   // OEMFEATURES_H


#ifndef OEMCFGI_H
#define OEMCFGI_H
/*===========================================================================

              O E M   C F G I

DESCRIPTION
  Definitions to support access to OEM configuration items.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   This file should not be included directly.  It is included by
   OEMConfig.h, which is included by AEEConfig.h


Copyright (c) 2001-2007     by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMCFGI.h#63 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/03/08   yb      Added CFGI_TOOLKIT_CS_PS_PARALLEL
02/13/08   sam     Add include for CustomOEMConfigItems.h
02/06/08   sam     Remove CFGI_TTY and add it to CutomOEMConfigItems.
09/28/07   clm     Add CFGI_NITZ_NW_INFO_MCC and CFGI_NITZ_NW_INFO_MNC items.
02/05/07   ypmw    Fix the compiler errors for the changes of sound dev 
                   attach variables
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
12/07/06   mv/jas  Removing CFGI_FACTORY_TEST_MODE, which is unused.
10/19/06   jas     Mainlining uiOne HDK changes
09/18/06   cvs     Integrate brew 4.0 changes from BTG.
09/14/06   jas     New item for availability of Manual PLMN selection
06/28/06   jas     Correcting featurization to work on 6275 with BREW 3.1.4
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
06/07/06   JD      Removed two config items, implemented through AEEConfig
06/13/06   JD      Added CFGI_VOICEPRIVACY
03/31/06   ac      Added support for QIPCALL debug info
02/21/06   cvs     Add support for persistent game mode
12/01/05   rmd     Added support to Band Class 14.
11/21/05   jas     New item for disabling background images
11/15/05   jas     New item for disabling display during calls
10/26/05   RI      Added CFGI_NITZ_NW_INFO.
10/10/05   PMD     Added CFGI_SMS_RETRY_INTERVAL Item.
09/22/05   cvs     Support for Stereo USB Carkit
08/29/05   RI      Enable auto update of user time using MM info
07/15/05   AT      Added CFGI item display_duration and back_light_intensity.
06/20/05   bmg     Added CFGI item for IntelliCeiver state
06/10/05   cvs     Support for Smart USB Carkit
06/07/05   sun     Added support for CCBS
05/24/05   jas     Fixing include file case for Linux builds.
04/19/05   sun     Added new Item for Manual PLMN Selection
11/17/04   sun     Added new CFGI Item for Beep Length
11/04/04   PMD     Added SMS GCF Flag CFGI Item.
10/30/04   bhas    Debug Screen support for UMTS targets
10/04/04   sun     Removed CFGI Item for CPHS
09/27/04   sun     Added new CFGI Item to support CPHS
09/08/04   dyc     Expand band enum from bc0 to bc13
08/16/04   NNG     Support for voice debug.
06/07/04   sun     Added support for LCD Display
05/17/04   ak      Added HDR related functions
04/15/04   PA      Added CFGI_LAST_RX_DATA_COUNT, CFGI_LAST_TX_DATA_COUNT,
                   CFGI_TOTAL_RX_DATA_COUNT and CFGI_TOTAL_TX_DATA_COUNT
04/14/04   SUN     Added new CFGI item CFGI_VR & CFGI_TTY
03/08/04   PMD     Added Support for WMS Application Mode and seperate
                   Voicemails for CDMA and GSM / WCDMA (GW)
02/14/04   PMD     Added SMS Retry Period Config Item
02/10/04   SUN     Added new CFGI item for CDMA/HDR diversity control
02/10/04   SUN     Added support for CFGI_AMR
01/28/04   RAM     Added new CFGI item for SpeakerPhone.
01/26/04   PMD     Added SMS Segmentation Message Reference
01/26/04   PMD     Added SMS Segmentation Settings
11/26/03   SUN     Added new CFGI item for CUG
11/23/03   SUN     Added new CFGI items for devices
11/05/03   RA      Added PA,MSM,PMIC,CHARGER,BATTERY thermistor readings
11/04/03   PMD     Added CDMA and GW Default Template Settings for SMS
10/31/03   PMD     Added IConfig Items for SMS Cell Broadcast Settings
                   Added IConfig Items for SMS Alert Settings
08/20/03   ADM     Added CFGI_LASTCALL_TIMER, CFGI_AIR_TIMER,CFGI_ROAM_TIMER.
                   CFGI_LIFE_TIMER to retrieve time information for the
                   RecentCalls app
08/19/03   SUN     Added Support for IMEI
08/05/03   PMD     Added Mode preference (CDMA / GW) NV Item for SMS to be
                   accessed using IConfig
09/16/02   sg      Added a number of CFGI items for Debug Applet
09/13/02   mjv     Added Dynamic R-UIM support
08/29/02   sg      Added CFGI_TIME_FORMAT
08/27/02   mjv     Added CFGI_FDN_ENABLED, CFGI_RECENT_MT_CALL_TIMER,
                   CFGI_RECENT_MO_CALL_TIMER, CFGI_MINUTE_ALERT,
                   CFGI_ANYKEY_ANSWER, and CFGI_HEADSET_AUTOANSWER.
                   Removed CFGI_LAST_CALL_TIMER.
08/08/02   sg      Featurized Smart Sound
07/30/02   mjv     Added CFGI_VOICEMAIL_NUMBER
07/19/02   mjv     Added CFGI_KEYGUARD_ENABLED
07/19/02   mjv     Added CFGI_OTKSLCODE
07/02/02   mjv     Added CFGI_IMSI_11_12, CFGI_IMSI_S, CFGI_MODE_PREF,
                   CFGI_AMPS_HOME_SID, CFGI_AMPS_FIRSTCHP, CFGI_AMPS_REG_TYPE,
                   CFGI_DATA_QNC_ENABLED, CFGI_DATA_DIALSTRING,
                   CFGI_SMS_IS41_WORKAROUND and CFGI_SMS_MO_ENCODING.
                   Renamed CFGI_COUNTRY_CODE to CFGI_IMSI_MCC.
                   Removed CFGI_MODEL_ID.
06/14/02   mjv     Added new CFGI items for the Service Programming applet,
                   added better documentation for existing items.
06/04/02   lnr     Fixed defects 384 & 402. Make the maximum SMS message
                   payload and the SMS MO Channel selection as configurable
                   items.
06/02/02   mjv     Removed a number of CFGI items that are not used.
                   Featurized CFGI_LOCKCODE with FEATURE_UIM.
05/29/02   mjv     Added CFGI_DEBUG_* config items
02/01/02   mjv     Removed CFGI_LAST_CALL.
12/05/01   jcb     Created file. Moved OEM AEEConfigItem's here.

===========================================================================*/

#include "BREWVersion.h"

#if MIN_BREW_VERSIONEx(3,1,2)
#ifndef AEECONFIG_H
#include "AEEConfig.h"
//#error OEMCFGI.h must not be included directly, include AEEConfig.h instead
#endif
#else
#ifndef OEMCONFIG_H
#error OEMCFGI.h must not be included directly, include OEMConfig.h instead
#endif
#endif

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include "oemnvint.h"  // include the config values


#include "CustomOEMConfigItems.h" // include CustomOEMConfigItems define


//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

enum /* OEM AEEConfigItem Values */
{
   //////////////////////////////////////////////////////////////////////
   // Setting Menu
   //////////////////////////////////////////////////////////////////////

   CFGI_SILENCEALL = CFGI_FIRST_OEM, // type=byte
   CFGI_AUTOKEYGUARD,                // type=byte
   CFGI_RECENT_MT_CALL_TIMER,        // type=uint32
   CFGI_RECENT_MO_CALL_TIMER,        // type=uint32
   CFGI_ALL_CALL_TIMER,              // type=uint32
   CFGI_BROWSER_CALL_TIMER,          // type=uint32
   CFGI_HEADSET_RNG,                 // type=byte
   CFGI_ANYKEY_ANSWER,               // type=boolean
   CFGI_HEADSET_AUTOANSWER,          // type=byte
   CFGI_MISSED_CALL_ALERT,           // type=byte
   CFGI_ROAMING_ALERT,               // type=byte
   CFGI_INTERNET_ALERT,              // type=byte
   CFGI_MINUTE_ALERT,                // type=byte
   CFGI_GAME_MODE,                   // type=boolean
   CFGI_GAME_MODE_PERSIST,           // type=boolean


   ////////////////////////////////////////////////////////////////
   // Sound Menu
   ////////////////////////////////////////////////////////////////

   CFGI_RINGER_VOL,                  // type=byte
   CFGI_EAR_VOL,                     // type=byte
   CFGI_BEEP_VOL,                    // type=byte
   CFGI_ALERT_TYPE,                  // type=byte
#ifdef FEATURE_SMART_SOUND
   CFGI_SMART_SOUND,                 // type=boolean
#endif
   CFGI_KEYBEEP_SOUND,               // type=byte
   CFGI_KEYTONE_LENGTH,              // type=keyToneLength

   // CFGI_RINGER_TYPE should only used by OEMRinger.c
   // The valid values are:
   // {SND_ALERT,SND_EFS_RINGER_1,SND_RING_[123456789],SND_ALERT_[12345]}
   CFGI_RINGER_TYPE,                 // type=byte,

   // IMEI
   CFGI_IMEI,                       // type = oemnvi_imei_type
   CFGI_CUG,                        // type = OEMNVCugInfo
   CFGI_VR,                         // type = boolean

    //AMR
    CFGI_AMR,                        // type=boolean
    CFGI_LCD,
    CFGI_LCD_TIMER,                  //display_duration type uint8
    CFGI_CCBS,

    ////////////////////////////////////////////////////////////////
    // Data Call Counters
    ////////////////////////////////////////////////////////////////

    CFGI_LAST_RX_DATA_COUNT,       // type=uint32
    CFGI_LAST_TX_DATA_COUNT,       // type=uint32
    CFGI_TOTAL_RX_DATA_COUNT,      // type=uint32
    CFGI_TOTAL_TX_DATA_COUNT,      // type=uint32
    CFGI_BEEP_LENGTH,              // type=uint32

    //CDMA Rx Diversity Control
    CFGI_CDMA_RX_DIVERSITY_CTL,

    //HDR Rx Diversity Control
    CFGI_HDR_RX_DIVERSITY_CTL,

   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////

   CFGI_PHONE_NUMBER,  // type=AECHAR *, len <= OEMNV_PHONENUMBER_MAXLEN

   ////////////////////////////////////////////////////////////////
   // Display Menu
   ////////////////////////////////////////////////////////////////

   CFGI_BACK_LIGHT_INTENSITY, // type=uint32
   CFGI_LANGUAGE_SELECTION,  // type=byte
   CFGI_BANNER,              // type=AECHAR *, size <= OEMNV_BANNER_MAXLEN
   CFGI_MENU_FORMAT,         // type=byte
   CFGI_TIME_FORMAT,         // type=byte
   CFGI_MANUAL_PLMN_SEL,     // type=boolean    
#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
   CFGI_AUTO_TIME_ENABLE,    // type = boolean
   CFGI_NITZ_NW_INFO,        // type = AECHAR, len <= OEMNV_MAX_NW_NAME_CHARS
   CFGI_NITZ_NW_INFO_MCC,    // type=uint32
   CFGI_NITZ_NW_INFO_MNC,    // type=uint32
#endif 

   ////////////////////////////////////////////////////////////////
   // Toolkit
   ////////////////////////////////////////////////////////////////
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
   CFGI_TOOLKIT_CS_PS_PARALLEL,  // type = boolean 
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////

   CFGI_SMS_SERVICE_OPTION,          // type=byte
   CFGI_SMS_TIMESTAMP,               // type=byte   
   CFGI_SMS_IS41_WORKAROUND,         // type=byte
   CFGI_PPP_AUTH,					 // type=byte
   CFGI_SMS_MO_ENCODING,             // type=byte
   CFGI_SMS_MODE,                    // type=byte
   CFGI_SMS_APP,                     // type=byte
   CFGI_SMS_CB_AUTO_DELETE,          // type=byte
   CFGI_SMS_CB_SILENT_ALERT,         // type=byte
   CFGI_SMS_CB_LANG_FILTER,          // type=byte
   CFGI_SMS_ALERT_DISPLAY,           // type=byte
   CFGI_SMS_ALERT_SOUND,             // type=byte
   CFGI_SMS_ALERT_VOLUME,            // type=byte
   CFGI_SMS_CDMA_DEFAULT_TEMPLATE,   // type=byte
   CFGI_SMS_GW_DEFAULT_TEMPLATE,     // type=byte
   CFGI_SMS_SEGMENT_SETTING,         // type=byte
   CFGI_SMS_SEGMENT_REF_NUM,         // type=uint16
   CFGI_SMS_RETRY_PERIOD,            // type=uint16
   CFGI_SMS_RETRY_INTERVAL,          // type=uint16
   CFGI_SMS_GCF_FLAG,                // type=boolean
   CFGI_VOICEMAIL_NUMBER,            // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN
   CFGI_VOICEMAIL_NUMBER_CDMA,       // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN
   CFGI_VOICEMAIL_NUMBER_GW,         // type=AECHAR *,
                                     // size <= OEMNV_VOICEMAIL_MAXLEN


   /////////////////////////////////////////////////////////////////
   // Recent Calls App
   /////////////////////////////////////////////////////////////////

   CFGI_LASTCALL_TIMER,              // Last Call Time in seconds (type = uint32)
   CFGI_AIR_TIMER,                   // Total home air time in seconds (type = uint32)
   CFGI_ROAM_TIMER,                  // Total roaming time in seconds (type = uint32)
   CFGI_LIFE_TIMER,                  // Total time in seconds (type = uint32)

   ////////////////////////////////////////////////////////////////
   // Service Programming
   ////////////////////////////////////////////////////////////////

   CFGI_SECCODE,          // type=AECHAR *, len == OEMNV_SECCODE_LENGTH
#ifdef FEATURE_ENABLE_OTKSL
   CFGI_OTKSLCODE,        // type=AECHAR *, len == OEMNV_SECCODE_LENGTH
#endif
   CFGI_ESN,              // type=uint32 (read-only)
   CFGI_SLOTINDEX,        // type=byte
   CFGI_BUILD_VERSION,    // type=AECHAR * (ver_dir variable from mobile.h)
   CFGI_BUILD_TIME,       // type=AECHAR * (ver_dir variable from mobile.h)
                          // read-only
   CFGI_RFCAL_VERSION,    // type=AECHAR *, len==OEMNV_RFCALVER_LEN(read-only)
   CFGI_RFCAL_DATE,       // type=dword (read-only)
   CFGI_HOME_SIDNID_LIST, // type=array of AEEConfigSIDNIDPairTypes
                          //      with OEMNV_HOME_SIDNID_SIZE
                          //      elements.
   CFGI_LOCK_SIDNID_LIST, // type=array of AEEConfigSIDNIDPairTypes
                          //      with OEMNV_LOCK_SIDNID_ARRSIZE
                          //      elements.
   CFGI_IMSI_MCC,         // type=uint16 (MCC encoded as decimal)   (read-only)
   CFGI_IMSI_11_12,       // type=uint16 (MNC encoded as decimal)   (read-only)
   CFGI_IMSI_S,           // type=AECHAR *, len==OEMNV_IMSI_S_LENGTH(read-only)
   CFGI_PRL_ENABLED,      // type=boolean
   CFGI_PRI_CH_A,         // type=uint16
   CFGI_PRI_CH_B,         // type=uint16
   CFGI_SEC_CH_A,         // type=uint16
   CFGI_SEC_CH_B,         // type=uint16
   CFGI_AOC,              // type=byte   (read-only)
   CFGI_HOME_SID_REG,     // type=boolean
   CFGI_FORN_SID_REG,     // type=boolean
   CFGI_FORN_NID_REG,     // type=boolean

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

   CFGI_DATA_QNC_ENABLED, // type=boolean
   CFGI_DATA_DIALSTRING,  // type=AECHAR *, len <= OEMNV_DATA_DIALSTRING_MAXLEN

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   CFGI_RTRE_CONFIGURATION,	 // nv_rtre_configuration_type
#endif


   CFGI_BREW_CID,     // type=unit32, carrier ID
   CFGI_BREW_PID,     // type=unit32, platform ID
   CFGI_BREW_BKEY,    // type=AECHAR *, size <= DL_BKEY_SIZE
   CFGI_BREW_AKEY,    // type=AECHAR *, size <= DL_AKEY_SIZE
   CFGI_BREW_SERVER,  // type=AECHAR *, size <= DL_MAX_SERVER
   CFGI_BREW_FLAGS,   // type=uint16, 16-bit download related flags
   CFGI_BREW_AUTH,    // type=byte, download auth policy
   CFGI_BREW_PRIVP,   // type=byte, privacy policy

   ////////////////////////////////////////////////////////////////
   // Security Menu
   ////////////////////////////////////////////////////////////////

   CFGI_LOCKCODE,         // type=AECHAR *, len <= OEMNV_LOCKCODE_MAXLEN

#ifdef FEATURE_FDN
   CFGI_FDN_ENABLED,      // type=boolean
#endif /* FEATURE_FDN */

   CFGI_HEADSET,        // type=boolean
   CFGI_STEREO_HEADSET, // type=boolean
   CFGI_USB_HFK_ATTACHED,   // type=boolean
   CFGI_STEREO_USB_HFK_ATTACHED,   // type=boolean
   CFGI_EXT_PWR,        // type=boolean
#ifdef FEATURE_VOICE_FIXED_PATTERN_DEBUG
#error code not present
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */
   CFGI_DISABLE_IN_CALL_DISP, // type=boolean
   CFGI_DISABLE_BG_IMAGE, // type=boolean
   /* Whether the UI should provide Manual PLMN selection. */
   CFGI_MANUAL_PLMN_SEL_ALLOWED,  // type = boolean

   ////////////////////////////////////////////////////////////////
   //
   // State Items
   //
   // The following CFGI items are used by the UI to request information
   // that is currently not available through any existing BREW interfaces.
   //
   // Unless otherwise indicated, state items are read-only.
   //
   // The caller must not assume that these items are actually
   // supported by the OEM layer.  If return value from ICONFIG_GetItem()
   // is not AEE_SUCCESS, the Applet should take appropriate action
   // like using default value instead.
   //
   ////////////////////////////////////////////////////////////////
   CFGI_FIRST_STATE_ITEM = 0x8000,   // Move the 'state items' far away
                                     // from the normal config items

   CFGI_DEVICE, //type = uint32

   // Debug Menu state items
   CFGI_DEBUG_CHANNEL,    // type=uint16
   CFGI_DEBUG_PILOT_PN,   // type=uint16
   CFGI_DEBUG_TX_AGC,     // type=uint16
   CFGI_DEBUG_TX_AGC_IN_DBM,// type=int16
   CFGI_DEBUG_RX_AGC,     // type=byte
   CFGI_DEBUG_RX_AGC_IN_DBM,// type=int16
   CFGI_DEBUG_ECIO,
   CFGI_DEBUG_SID,        // type=uint16
   CFGI_DEBUG_NID,        // type=uint16
   CFGI_DEBUG_TX_ADJUST,  // type=byte
   CFGI_DEBUG_HDET,       // type=byte
   CFGI_DEBUG_VBATT,      // type=byte
   CFGI_DEBUG_ERRLOG,     // type=OEMErrLOgType
   CFGI_DEBUG_LNA_STATE,  // type=byte
   CFGI_DEBUG_PA_STATE,   // type=byte
   CFGI_DEBUG_RATCHET_STATE,// type=byte

#if defined(FEATURE_WCDMA)
#error code not present
#endif

#ifdef T_MSM6500
    CFGI_DEBUG_RF_CARD_NV_ID,
    CFGI_DEBUG_THERM_BATT,
    CFGI_DEBUG_THERM_MSM,
    CFGI_DEBUG_THERM_PA,
    CFGI_DEBUG_THERM_PMIC,
    CFGI_DEBUG_THERM_CHRGR,
#endif

   // Items required for power calculation in Debug Menu
   CFGI_CDMA_DYNAMIC_RANGE, // type=unit16
   CFGI_CDMA_MIN_RX_RSSI,   // type=uint16

#ifdef FEATURE_QIPCALL
   CFGI_QIPCALL_DEBUG_EVDO_IP_ADDR,
   CFGI_QIPCALL_DEBUG_EVDO_SIP_REG_STATUS,
   CFGI_QIPCALL_DEBUG_WLAN_IP_ADDR,
   CFGI_QIPCALL_DEBUG_WLAN_SIP_REG_STATUS,
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

   CFGI_DEBUG_THERM,      // type=uint16
   CFGI_DEBUG_BAND,       // type=byte
   //
   // TRUE if the keyguard is currently enabled.
   //
   // This item is not read-only. The keyguard can be enabled/disabled by
   // setting this item to TRUE/FALSE.
   //
   CFGI_KEYGUARD_ENABLED, // type=boolean

#ifdef FEATURE_INTELLICEIVER
   CFGI_DEBUG_INTELLICEIVER_STATE,
#endif /* FEATURE_INTELLICEIVER */
   CFGI_BREW_DL_FLAGS,
   
   CFGI_LAST_OEM_ITEM
};

enum
{
#ifdef T_MSM6500
  CFGI_DEBUG_BAND_BC0 = 0,
  CFGI_DEBUG_BAND_BC1,
  CFGI_DEBUG_BAND_BC2,
  CFGI_DEBUG_BAND_BC3,
  CFGI_DEBUG_BAND_BC4,
  CFGI_DEBUG_BAND_BC5,
  CFGI_DEBUG_BAND_BC6,
  CFGI_DEBUG_BAND_BC7,
  CFGI_DEBUG_BAND_BC8,
  CFGI_DEBUG_BAND_BC9,
  CFGI_DEBUG_BAND_BC10,
  CFGI_DEBUG_BAND_BC11,
  CFGI_DEBUG_BAND_BC12,
  CFGI_DEBUG_BAND_BC13,
  CFGI_DEBUG_BAND_BC14,
#endif //T_MSM6500
  CFGI_DEBUG_BAND_AMPS,
  CFGI_DEBUG_BAND_CDMA,
  CFGI_DEBUG_BAND_PCS,
  CFGI_DEBUG_BAND_GPS,
  CFGI_DEBUG_BAND_SLEEP,
  CFGI_DEBUG_BAND_LAST,
  CFGI_DEBUG_BAND_GSM,
  CFGI_DEBUG_BAND_WCDMA
};

#ifdef FEATURE_FLEXI_STATIC_BREW_APP
typedef enum 
{
	STATIC_BREW_APP_FLEXI_NASRANI,
	STATIC_BREW_APP_FLEXI_MUSLIM,
	STATIC_BREW_APP_FLEXI_PORTAL,
	STATIC_BREW_APP_DEFAULT	
}STATIC_BREW_APP_e;
#endif

#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
typedef enum 
{
	STATIC_BREW_APP_SMARTFREN_FACEBOOK,
	STATIC_BREW_APP_SMARTFREN_MNEWS,
	STATIC_BREW_APP_SMARTFREN_MKARAOKE,
	STATIC_BREW_APP_SMARTFREN_SATU,
	STATIC_BREW_APP_SMARTFREN_TWITTER,
	STATIC_BREW_APP_SMARTFREN_SFM,
	STATIC_BREW_APP_SMARTFREN_MSHOP,
	STATIC_BREW_APP_SMARTFREN_MDIRECTORY,
	STATIC_BREW_APP_SMARTFREN_FGEN,
	STATIC_BREW_APP_SMARTFREN_MOKA,
	STATIC_BREW_APP_DEFAULT	
}STATIC_BREW_APP_e;
#endif
/*===========================================================================

                     FUNCTION PROTOTYPES

============================================================================*/

boolean   OEM_GetCachedLCDSetting(void);

/*==============================================================================
函数：
       GetRepeatRawNumber
       
说明：
       得到raw　number.用于把连续重复的号码记录为一条记录。

参数：
      
返回值：
       无。

备注：:

==============================================================================*/
int GetRepeatRawNumber(AECHAR * number,AECHAR * rawnumber);

void OEM_RestoreFactorySetting( void );

boolean OEM_IsEmergency_Number(char *pNumber,int len);
#endif // !OEMCFGI_H


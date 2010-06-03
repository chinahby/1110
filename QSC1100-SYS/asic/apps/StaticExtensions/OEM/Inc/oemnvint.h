#ifndef OEMNVINT_H
#define OEMNVINT_H

/*=============================================================================

FILE: oemnvint.h

SERVICES:  Valid values for CFGI items

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   This file should not be included directly.  It is included by OEMCFGI.h

(c) COPYRIGHT 2002-2006 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/oemnvint.h#19 $
$DateTime: 2008/02/13 14:31:16 $
  $Author: carls $
  $Change: 609141 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/12/08   SAM     Typecasting OEMTTY to be uint32
10/19/06   JAS     Fixing backlight values to be seconds-based.
                   Removing VOICEPRIVACY values, now defined elsewhere
07/12/06   JAS     Adding explicit start value for the OEMTTY enum
06/14/06   JD      Removed defines for old SMS channel selection
06/13/06   JD      Added defines for VOICEPRIVACY
10/10/05   PMD     Added OEMNV_SMS_RETRY_INTERVAL define for 5 seconds
09/22/05   cvs     Support for Stereo USB Carkit
09/08/05   PMD     Added OEMNV_SMS_RETRY_PERIOD define for 240 seconds
06/10/05   cvs     Support for Smart USB Carkit
06/07/05   sun     Added support for CCBS
11/17/04   SUN     Added new enum for Beep Length
04/14/04   SUN     Added support for CFGI_TTY
03/08/04   PMD     Added Support for WMS Application Mode
01/28/03   RAM     Added mask for SpeakerPhone.
11/23/03   SUN     Added mask for CFGI Items
10/31/03   PMD     Added IConfig Items for SMS Cell Broadcast Settings
                   Added IConfig Items for SMS Alert Settings
08/05/03   PMD     Added Mode preferences (CDMA/GSM) for SMS
09/16/02   sg      Added max length of RF CAL Version string and an Error Log
                   structure for Debug Applet
09/13/02   mjv     Added Dynamic R-UIM support
09/11/02   mjv     CR #602: Removed the 'Digial Pref' and 'Analog Pref'
                   mode preference.
08/29/02   sg      Added the TIMEFORM values
08/27/02   mjv     Added values for CFGI_HEADSET_AUTOANSWER and
                   CFGI_ANYKEY_ANSWER
07/30/02   mjv     Added OEMNV_VOICEMAIL_MAXLEN
07/02/02   mjv     Added values relating to CFGI_SMS_MO_ENCODING,
                   CFGI_SMS_IS41_WORKAROUND, CFGI_IMSI_S, CFGI_MODE_PREF,
                   CFGI_AMPS_REG_TYPE, and CFGI_DATA_DIALSTRING.
06/14/02   mjv     Added values for the new Service Programming CFGI items,
                   better documented the existing items.
06/04/02   lnr     Fixed defects 384 & 402. Make the maximum SMS message
                   payload and the SMS MO Channel selection as configurable
                   items.
06/02/02   mjv     Removed OEMNV_PINLOCK_ON and OEMNV_PINLOCK_OFF
04/17/02   mjv     Swapped values of OEMNV_SMARTSOUND_DISABLE and
                   OEMNV_SMARTSOUND_ENABLE.  Their values are now 0 and 1
                   respectively.
01/23/02   mjv     Added edit history

=============================================================================*/

#ifndef OEMCFGI_H
#error oemnvint.h must not be included directly, include OEMConfig.h instead
#endif

/************************************************************************/
/*                                                                      */
/*  Setting Menu                                                        */
/*                                                                      */
/************************************************************************/

/////////////////////////////////////////////////////////////////////////
// Silence All -- CFGI_SILENCEALL
/////////////////////////////////////////////////////////////////////////
#define OEMNV_SA_NORMAL              0
#define OEMNV_SA_VIBONLY             1
#define OEMNV_SA_LITONLY             2

/////////////////////////////////////////////////////////////////////////
// Auto Key Guard -- CFGI_AUTOKEYGUARD
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_AKG_OFF              0
#define   OEMNV_AKG_ON               1
#define   OEMNV_AKG_30SEC            2
#define   OEMNV_AKG_2MIN             3
#define   OEMNV_AKG_5MIN             4

/////////////////////////////////////////////////////////////////////////
// BackLight On/Off with External Power -- CFGI_EXTPWR_BK_LIGHT
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_EXTPWR_BL_OFF         0
#define   OEMNV_EXTPWR_BL_ON          1


/////////////////////////////////////////////////////////////////////////
// Headset Ring  -- CFGI_HEADSET_RNG
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_HEADSET_RNG_OFF       0
#define   OEMNV_HEADSET_RNG_ON        1

/////////////////////////////////////////////////////////////////////////
// Anykey Answer  -- CFGI_ANYKEY_ANSWER
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_ANYKEY_ANSWER_OFF       0
#define   OEMNV_ANYKEY_ANSWER_ON        1

/////////////////////////////////////////////////////////////////////////
// Headset Autoanswer  -- CFGI_HEADSET_AUTOANSWER
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_HEADSET_AUTOANSWER_OFF       0
#define   OEMNV_HEADSET_AUTOANSWER_2SEC      2
#define   OEMNV_HEADSET_AUTOANSWER_5SEC      5
#define   OEMNV_HEADSET_AUTOANSWER_10SEC    10


/////////////////////////////////////////////////////////////////////////
// Alert -- CFGI_MISSED_CALL_ALERT, CFGI_ROAMING_ALERT,
//          CFGI_SVC_ALERT, CFGI_INTERNET_ALERT, CFGI_MINUTE_ALERT
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_ALERT_DISABLE         0
#define   OEMNV_ALERT_ENABLE          1

/************************************************************************/
/*                                                                      */
/*  Security Menu                                                      */
/*                                                                      */
/************************************************************************/

/////////////////////////////////////////////////////////////////////////
// Security Menu Software Based Lock Code -- CFGI_LOCKCODE
/////////////////////////////////////////////////////////////////////////

// maximum number of AECHARS in the lock code (including a NULL word)
#define OEMNV_LOCKCODE_MAXLEN 5

/***********************************************************************/
/*                                                                     */
/*  Sound Menu                                                       */
/*                                                                     */
/***********************************************************************/
/////////////////////////////////////////////////////////////////////////
// Volume -- CFGI_RINGER_VOL,CFGI_EAR_VOL,CFGI_BEEP_VOL
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_VOLUME_OFF         0
#define   OEMNV_VOLUME_LOW         1
#define   OEMNV_VOLUME_MID         2
#define   OEMNV_VOLUME_HIGH        3
#define   OEMNV_VOLUME_ESCALATING  4  // only valid for CFGI_RINGER_VOL

/////////////////////////////////////////////////////////////////////////
// Key Tone Length -- CFGI_KEYTONE_LENGTH
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_KEYTONE_NORMAL    0
#define   OEMNV_KEYTONE_LONG     1

/////////////////////////////////////////////////////////////////////////
// Key Beep Sound -- CFGI_KEYBEEP_SOUND
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_KEYBEEP_TONE     0
#define   OEMNV_KEYBEEP_CLICK    1

/////////////////////////////////////////////////////////////////////////
// Smart Sound -- CFGI_SMART_SOUND
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_SMARTSOUND_ENABLE   1
#define   OEMNV_SMARTSOUND_DISABLE  0

/////////////////////////////////////////////////////////////////////////
// Alert Type -- CFGI_ALERT_TYPE
/////////////////////////////////////////////////////////////////////////

#define OEMNV_ALERTTYPE_OFF         0
#define OEMNV_ALERTTYPE_RINGER      1
#define OEMNV_ALERTTYPE_VIB         2
#define OEMNV_ALERTTYPE_VIBRINGER   3

/***********************************************************************/
/*                                                                     */
/*  Display Menu                                                       */
/*                                                                     */
/***********************************************************************/
/////////////////////////////////////////////////////////////////////////
// Backlighting -- CFGI_BACK_LIGHT
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_BL_OFF    0
#define   OEMNV_BL_10S     10
#define   OEMNV_BL_30S     30

/////////////////////////////////////////////////////////////////////////
// Menu Format -- CFGI_MENU_FORMAT
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_MENUFORM_LARGE    0
#define   OEMNV_MENUFORM_SMALL    1

/////////////////////////////////////////////////////////////////////////
// Time Format -- CFGI_TIME_FORMAT
/////////////////////////////////////////////////////////////////////////
#define   OEMNV_TIMEFORM_AMPM    0
#define   OEMNV_TIMEFORM_24HR    1

/////////////////////////////////////////////////////////////////////////
// Banner -- CFGI_BNNER
/////////////////////////////////////////////////////////////////////////

// maximum number of AECHARS in the banner (including the NULL word)
#define OEMNV_BANNER_MAXLEN  13

/************************************************************************/
/*                                                                      */
/*  SMS Settings                                                        */
/*                                                                      */
/************************************************************************/

/////////////////////////////////////////////////////////////////////////
// Maximum Message Payload -- CFGI_SMS_MAX_MSG_PAYLOAD
/////////////////////////////////////////////////////////////////////////
#define OEMNV_SMS_MAX_MSG_PAYLOAD      180

/////////////////////////////////////////////////////////////////////////
// SMS Service Option (to send a mobile originated SMS message) --
// CFGI_SMS_SERVICE_OPTION
/////////////////////////////////////////////////////////////////////////
#define OEMNV_SMS_SO_NONE           0
#define OEMNV_SMS_SO_SO6            1
#define OEMNV_SMS_SO_SO14           2

/////////////////////////////////////////////////////////////////////////
// SMS MT Timestamp Ajust -- CFGI_SMS_TIMESTAMP
/////////////////////////////////////////////////////////////////////////
#define OEMNV_SMS_TIMESTAMP_ADJUST        0
#define OEMNV_SMS_TIMESTAMP_ASRECEIVED    1

/////////////////////////////////////////////////////////////////////////
// SMS IS-41 Workaround -- CFGI_SMS_IS41_WORKAROUND
/////////////////////////////////////////////////////////////////////////

// Mobile station will originate SMS to an e-mail address using the IS41
// workaround solution.
#define OEMNV_SMS_EMAIL_ADDRESSING_IS41      0

// Mobile station will originate SMS to an e-mail address using
// IS637-A format.
#define OEMNV_SMS_EMAIL_ADDRESSING_STANDARD  1

/////////////////////////////////////////////////////////////////////////
// SMS MO Encoding -- CFGI_SMS_MO_ENCODING
/////////////////////////////////////////////////////////////////////////

// Encode Mobile Originated messages in 7-bit ASCII
#define OEMNV_SMS_MO_ENCODING_7BIT           0

// Encode Mobile Originated messages in UNICODE
#define OEMNV_SMS_MO_ENCODING_UNICODE        1

// Encode mode to CDMA
#define OEMNV_SMS_MODE_CDMA                  0

// Encode mode to GSM / UMTS (GW)
#define OEMNV_SMS_MODE_GW                    1

// WMS Application User Mode
#define OEMNV_SMS_APP_USER                   0

// WMS Application Test Mode
#define OEMNV_SMS_APP_TEST                   1

// Disable Cell Broadcast Auto Delete
#define OEMNV_SMS_CB_DISABLE_AUTO_DELETE     0

// Enable Cell Broadcast Auto Delete
#define OEMNV_SMS_CB_ENABLE_AUTO_DELETE      1

// Disable Cell Broadcast Silent Alert
#define OEMNV_SMS_CB_DISABLE_SILENT_ALERT    0

// Enable Cell Broadcast Silent Alert
#define OEMNV_SMS_CB_ENABLE_SILENT_ALERT     1

// Disable Cell Broadcast Language Filter
#define OEMNV_SMS_CB_DISABLE_LANG_FILTER     0

// Enable Cell Broadcast Language Filter
#define OEMNV_SMS_CB_ENABLE_LANG_FILTER      1

// Disable SMS Display Alert
#define OEMNV_SMS_ALERT_DISPLAY_DISABLE      0

// Enable SMS Display Alert
#define OEMNV_SMS_ALERT_DISPLAY_ENABLE       1

// Disable SMS Sound Alert
#define OEMNV_SMS_ALERT_SOUND_DISABLE        0

// Enable SMS Sound Alert
#define OEMNV_SMS_ALERT_SOUND_ENABLE         1

// SMS Alert Volume Level 0 (Mute)
#define OEMNV_SMS_ALERT_VOLUME_0             0

// SMS Alert Volume Level 1
#define OEMNV_SMS_ALERT_VOLUME_1             1

// SMS Alert Volume Level 2
#define OEMNV_SMS_ALERT_VOLUME_2             2

// SMS Alert Volume Level 3
#define OEMNV_SMS_ALERT_VOLUME_3             3

// SMS Alert Volume Level 4
#define OEMNV_SMS_ALERT_VOLUME_4             4

// Retry Period
#define OEMNV_SMS_RETRY_PERIOD             240

// Retry Interval
#define OEMNV_SMS_RETRY_INTERVAL             5

/////////////////////////////////////////////////////////////////////////
// Voice Mail Number -- CFGI_VOICEMAIL_NUMBER
/////////////////////////////////////////////////////////////////////////

// Maximum number of AECHARS in the voice mail number
// (including the NULL word)
#define OEMNV_VOICEMAIL_MAXLEN              32

/************************************************************************/
/*                                                                      */
/*  Service Programming                                                 */
/*                                                                      */
/************************************************************************/
/////////////////////////////////////////////////////////////////////////
// Phone Number -- CFGI_PHONE_NUMBER
/////////////////////////////////////////////////////////////////////////

// Maximum number of AECHARS in the phone number (including the NULL word)
#define OEMNV_PHONENUMBER_MAXLEN 16

/////////////////////////////////////////////////////////////////////////
// Service Programming Code -- CFGI_SECCODE
/////////////////////////////////////////////////////////////////////////

// Number of AECHARS in the SPC (including the NULL word)
#define OEMNV_SECCODE_LENGTH  7


/////////////////////////////////////////////////////////////////////////
// SysMenu Code -- CFGI_RFCAL_VERSION & CFGI_RFCAL_DATE
/////////////////////////////////////////////////////////////////////////

// The max string size of RF CAL Version
#define OEMNV_RFCALVER_LEN  9


/////////////////////////////////////////////////////////////////////////
// IMSI_S -- CFGI_IMSI_S
/////////////////////////////////////////////////////////////////////////

// Number of AECHARS in IMSI_S (including the NULL word)
#define OEMNV_IMSI_S_LENGTH  11

/////////////////////////////////////////////////////////////////////////
// "Home" SID+NID pairs for CDMA acquisition -- CFGI_HOME_SIDNID_LIST
/////////////////////////////////////////////////////////////////////////

// Number of AEEConfigSIDNIDPairTypes array elements
#define OEMNV_HOME_SIDNID_ARRSIZE 20

typedef PACKED struct {
  uint16 sid;
  uint16 nid;
} AEEConfigSIDNIDPairType;


/////////////////////////////////////////////////////////////////////////
// SID+NID pairs to be locked out from CDMA acquisition --
// CFGI_LOCK_SIDNID_LIST
/////////////////////////////////////////////////////////////////////////

// Number of AEEConfigSIDNIDPairTypes array elements
#define OEMNV_LOCK_SIDNID_ARRSIZE 10

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

/////////////////////////////////////////////////////////////////////////
// Dial string for originating packet calls -- CFGI_DATA_DIALSTRING
/////////////////////////////////////////////////////////////////////////

// Maximum length in AECHARs of the data dial string (including the NULL word).
#define OEMNV_DATA_DIALSTRING_MAXLEN  17

/////////////////////////////////////////////////////////////////////////
// SysMenu - The following structure is required to fetch the error logs
/////////////////////////////////////////////////////////////////////////

typedef struct _OEMErrLogType {
   byte err_num;         // n-th error
   byte err_count;       // # of times the error has occurred
   byte file_name[8];    // file name in which it occurred
   word line_num;        // line number where error hit
   boolean fatal;        // whether fatal
} OEMErrLogType;


#define OEMNV_STEREO_HEADSET_MASK              0x00000001
#define OEMNV_HEADSET_MASK                     0x00000002
#define OEMNV_ANALOG_HFK_MASK                  0x00000004
#define OEMNV_USB_HFK_ATTACHED_MASK            0x00000008
#define OEMNV_USB_HFK_ROUTE_AUDIO_MASK         0x00000010
#define OEMNV_STEREO_USB_HFK_ATTACHED_MASK     0x00000020
#define OEMNV_SDAC_MASK                        0x00000040
#define OEMNV_EXT_PWR_MASK                     0x00000080
#define OEMNV_SPEAKERPHONE_MASK                0x00000100


typedef uint32 OEMTTY;

#define OEMNV_TTY_FULL  0
#define OEMNV_TTY_TALK  1
#define OEMNV_TTY_HEAR  2
#define OEMNV_TTY_OFF   3

typedef enum{
  OEMNV_DTMFOFF=1,
  OEMNV_DTMFCONT,
  OEMNV_DTMFBURST
}OEMDTMF;

typedef enum
{
  OEM_CCBS_ACCEPT,
  OEM_CCBS_REJECT,
  OEM_CCBS_HOLD_N_ACCEPT,
  OEM_CCBS_RELEASE_N_ACCEPT,
  OEM_CCBS_NONE
}OEMCCBS;
#endif  /* OEMNVINT_H */


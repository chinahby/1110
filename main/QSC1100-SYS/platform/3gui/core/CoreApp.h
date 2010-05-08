#ifndef COREAPP_H
#define COREAPP_H
/*=============================================================================

FILE: CoreApp.h

SERVICES: QCT UI Core BREW Application.

GENERAL DESCRIPTION:
.h file for CoreApp.

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
This should be the startup application automatically launched by BREW.

(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreApp.h#151 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/07   cvs     Fix mem leak in MMOverlay BG Image code.  Remove Overlay2 BG img code
08/03/07   cvs     Featurize secondary display code
03/13/07   ypmw    Add 5 sec option for LCD display and backlight
03/07/07   jas     Reimplement background images with overlays.
                   Support VGA background images.
02/23/07   jas     Refactoring for HDK builds
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/11/06   RT      Moved m_nCurrSrvc and m_pRdmDeviceList to the AUXSettings applet
12/06/06   RT      Added prototype for CheckWLANAutoScanNewAssociation
11/29/06   ypmw    Fix wlan compiler warnings
11/21/06   RT      CoreApp Refactoring: Added prototypes for functions needed in CoreService
11/17/06   ak      Display Protocol Revision for HDR
11/10/06   jmb     Added support for IMMOverlay management of BREW bitmap
11/09/06   ypmw    Fix the WIFI Security setting errors
11/01/06   ypmw    Add SSID for start adhoc network
10/19/06   jas     Migrating config items to CustomOEMConfigItems
10/17/06   alb     Remove m_bySendSubscriptionFail from the cCoreApp object.
10/09/06   ypmw    Add multi-port for data services
09/14/06   jas     Removing unused JCDMA-related CCoreApp members
08/08/06   joe     NetSetting related changes.
07/20/06   sj      Mainlining 6500 (5.3) Coreapp changes
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
04/28/04   cvs     Fix text corruption
04/04/06   jas     Adding support for font selection
01/26/06   jks     Add SYS_MODE_GW macro
12/16/05   jks     Added security support for WLAN manual scans
12/05/05   jks     Added function prototype for DBtoLevel, added support for 
                   WLAN Network Scan Mode, removed support for IP Version
11/21/05   jas     Support for disabling background images
11/07/05   ak      Do not allow phone operations for blocked pin case
10/20/05   sun     Removed support for BT-SAP
10/12/05   RI      Added EVT_LCD_ON event.
09/30/05   cvs     Fix EVT_CORE_SET_TEXT_WHEN_APP_SUSPENDED when idle dialog 
                   is not the current dialog.
09/27/05   sun     Added new Callback and cmdStatus for CSP
09/22/05   cvs     Support for Stereo USB Carkit
09/05/05   RI      Added ISysClock member to pMe
08/31/05   sun     Added support for BT-SAP
08/08/05   aps     Fixed NV volume level lookup for sound devices
07/25/05   sun     Added ISTK member to pMe
07/05/05   ak      Merged Meid Support from Jaguar
06/14/05   ak      Read Network Name from ONS files
06/10/05   cvs     Support for Smart USB Carkit
05/19/05   aps     removed CCoreApp->m_bSilenceAll
05/09/05   ak      Using the packed string Decoding
04/20/05   JAS     Added WLAN subscription available mask
04/15/05   JAS     Reworked WLAN security menus
04/08/05   ak      Added Checks for buffer overflow in PNN/OPL Files
03/18/05   JAS     Support for WLAN Status screen
03/17/05   ak      Changes to Support Async Callback in CARD
03/15/05   cvs     Support for updating time on screen less frequently
03/10/05   ak      Added CPHS EONS Support
02/22/05   jas     Tweaking WLAN menbers in pMe
02/16/05   sun     Added support for Enhanced Network Selection
02/14/05   cvs     Fix race condition where subscriptions could be sent
                   before phone went online, and then weren't resent.
01/25/05   sun     Allow other Apps to Change Backlight setting via CoreApp
12/16/04   jas     Parenthesizing macros
12/09/04   sun     Added new LCD off timer - 10Sec
12/07/04   sun     Added new LCD off timer  - 30 Sec
12/06/04   BW      Added LAN menu and related submenu
11/22/04   sun     Added support for USB Charging
11/12/04   ank,cvs Dualslot changes from 6300
11/11/04   sun     Added support for LCD off timers
10/06/04   sun     Fixed Manual PLMN Search screens
09/22/04   cvs     Add Contrast menu, lint fixes
09/16/04   PA      Added new boolean to check if we have started the
                   initialization sequence for phone info available.
08/02/04   PA      Display PLMN List again only for network selection error.
07/18/04   sun     Added broadcast to idle menu while the app is suspended
06/28/04   jas     Changes to support dual-display targets
06/24/04   sun     Send Separate Event to Change Mode
06/07/04   tml     Added personalizaton support
06/07/04   sun     Added support for Manual PLMN Selection and  LCD Display
05/13/04   sun     Added LPM Changes
04/21/04   sun     Fixed Compile Error
04/14/04   sun     Added new menu item to set VR
03/19/04   sun     Added changes for DTMF
02/25/04   ram     Integration with IBatt Interface.
02/24/04   rd      Moved CLIPBOARD_NAME_MAX_LENGTH definition to appdiag.h
02/20/04   NNG     Added support for volume up/down keys.
02/20/04   sun     Enable voice privacy settings through diag
02/12/04   ram     Added support for Hybrid RSSI display.
01/29/04   tml     Added NEW_ICARD feature
01/29/04   ram     Replace AVK_SPEAKER with EVT_SPEAKERPHONE_TOGGLE
12/22/03   tml     ICARD API change
01/07/04   ram     Add Multimedia Volume setting
11/25/03   ram     Add broadcast text to idle menu and CUG Menu support
11/20/03   ram     Added HDR_ABORT_PPP support and fixed annunciators.
11/04/03   tml     Added Pref Slot support
11/03/03   ram     Added PLMN selection menu.
09/24/03   kar     Event cleanup
09/23/03   kar     Added task offline event
09/21/03   tml     ICM support
08/06/03   tml     Added dual slot support
08/06/03   sg      Added support for STEREO_HEADSET device.
08/05/03   kar     Set time changes
07/25/03   bpw     Removed dialer state variable
07/18/03   bpw     Added event for dialer to orginate call using SK UI.
07/16/03   tml     enable/disable pin1/pin2 cleanup
07/15/03   tml     Security clean up
06/10/03   kar     Include AEESuppsvc.h only for GW targets
06/03/03   AT      Adding in Supplementary Services.
05/19/03   kar     Code cleanup
05/16/03   wli     Added GSM 1X support
05/13/03   kar     Support for call origination from other apps
05/08/03   kar     System menu changes
04/14/03   kar     Code cleanup
04/10/03   kar     Added mask for RTRE config sent
04/01/03   kar     Code cleanup
03/11/03   pa      Added support for bad battery event
02/27/03   tml     Removed EVT_CAT_SS_REQUEST and EVT_CAT_USSD_REQUEST
02/27/03   tml     Added support for USAT Send SS and Send USSD
02/10/03   kar     Added support for numedit dialog
01/31/03   ic      Added Extras -> Call Info and Settings -> Network dialogs.
01/30/03   kar     FTM changes
01/28/03   tml     GSM/WCDMA compiler fixes
01/28/03   kar     Changes for AppComFunc API
01/21/03   kar     Added new event when phone info is ready
12/31/02   kar     Added variable to store time string
12/04/02   tml     Added comments to indicate UIM_STATUS_MASK is temporary
12/03/02   tml     Added one extra mask for STATUS command from UIM for the
                   byPhInfo member and add UIM_STATUS_MASK
11/15/02   tml     RTRE changes with CM event
11/08/02   kar     Added events for external devices
10/25/02   tml     RTRE changes
10/08/02   tml     code review changes
10/07/02   kar     Got rid of update BAR mode
10/02/02   kar     Code cleanup
10/01/02   kar     Added new call events
09/19/02   tml     Removed Corefeatures.h
09/17/02    lz     Added AEECLSID_CORE_STARTAPP, EVT_USER_EXIT, and m_bExit
                   in CCoreApp.
09/17/02   tml     Added GoToUpdateBAR variable
09/17/02   kar     Support for FTM
09/16/02   AAA     Added Multimedia Ringer support
09/11/02   kar     Code cleanup
08/26/02   sun     Added EVT_USER_WMS_CFG_CDMA_READY
08/26/02   sun     Added support for WMS
08/22/02   kar     Renamed variables to comply with guidelines
08/21/02  js/tml   Support for ICARD integration
08/15/02   kar     Reorganized system events to allow room for growth
08/13/02   kar     Core application header
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

/* Brew libraries */
#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEAnnunciator.h"
#include "AEECard.h"
#include "AEE_OEM.h"
#include "AEEMedia.h"         // AEE Multimedia Services
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "OEMCFGI.h"
#include "CustomOEMConfigItems.h"
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#include "AEESuppSvc.h"
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#endif

#include "AEECM.h"
#include "uixsnd.h"

#if (defined (FEATURE_HDR_SESSION_TIED_TO_PPP) || defined (TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP))
#include "dssocket.h"
#include "ds707_extif.h"
#define HDR_ABORT_PPP_TIMER 20000
#endif

#include "AEEBatt.h"
#include "AEETime.h"
#include "AEEDisp.h"
#include "AEESTK.h"
#include "disp.h"

#ifdef FEATURE_OVERLAY
#include "IOverlay.h"
#endif
#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif

#ifdef FEATURE_USRSYSCLOCK
#include "AEEUsrSysClock.h"
#endif // FEATURE_USRSYSCLOCK

#include "CoreApp.BID"
#include "CoreUserEvents.h"

/* No SK UI support */
#define CoreApp_IsSKUI()         0
#define CLIPBOARD_NAME_MAX_LENGTH 15

#define COREAPP_LCD_OFF_TIME_5_SEC  (5*1000)
#define COREAPP_LCD_OFF_TIME_10_SEC  (10*1000)
#define COREAPP_LCD_OFF_TIME_30_SEC  (30*1000)
#define COREAPP_LCD_OFF_TIME_2_MIN  (2 * 60* 1000)
#define COREAPP_LCD_OFF_TIME_10_MIN (10 * 60* 1000)

/* Dialog timers */
#define COREAPP_CONNECTED_DISPLAY_TIME   (2*1000)
#define COREAPP_NO_WLAN_AP_FOUND_TIME   (5*1000)
#define COREAPP_WLAN_AUTOSCAN_NO_ASSOCIATION_TIME   (2*1000)
/*=============================================================================

                       CONSTANTS

=============================================================================*/

#ifndef COREAPPTIMEOUT
#define COREAPPTIMEOUT 60000  /* Time out value for bringing the program back to
                                 Idle screen */
#endif
#define MM_RINGER_MAX_FILES         15


/* Security mask
0000 0000 0000 0000 0000 0000 0000 0000
 7     6    5    4    3    2    1    0 nibble

 0 - 2 nibbles => Slot 1 related info
 3 - 5 nibbles => Slot 2 related info
 7 nibble => NV/PIN1/PIN1(Slot2) Verified or not

 Within each slot bytes (e.g., 0-2 bytes)
 0 nibble => Operational slot info      0 0 0 0
                                        | | |___ GSM in slot 1
                                        | |_____ CDMA in slot 1
                                        |_______ UMTS in slot 1

 1 nibble => Application bitmask info   direct mapping to GSDI apps_enabled
           allow => AEECARD_NO_APP_MASK    0x00
                    AEECARD_TELECOM_MASK   0x01
                    AEECARD_GSM_SIM_MASK   0x02
                    AEECARD_USIM_APP_MASK  0x04
                    AEECARD_RUIM_APP_MASK  0x08
           limitation : EMV, DCS1800 and other potential apps are not supported
 2 nibble => Slot present and operational info valid?
                                      0 0 0 0
                                          | |___ Slot 1 present
                                          |_____ Operational slot info valid
*/
/* NV Code and PIN masks */
#define COREAPP_NV_CODE_MASK                  0x10000000  /* NV code verified */
#define COREAPP_PIN1_CODE_MASK                0x20000000  /* PIN code verified */
#define COREAPP_PIN1_SLOT2_CODE_MASK          0x40000000  /* PIN code for slot 2 verified */

#define COREAPP_EMERGENCY_MASK                0x80000000  /* emergency mask */
#define COREAPP_NV_PIN1_MASK                  0x30000000  /* NV code and PIN1 code */
#define COREAPP_NV_PIN1_SLOT2_MASK            0x50000000  /* NV code and pin1 slot2 code */
#define COREAPP_PIN1_PIN1_SLOT2_MASK          0x60000000  /* pin1, pin1 slot2 codes */
#define COREAPP_NV_PIN1_PIN1_SLOT2_MASK       0x70000000  /* NV, PIN1, PIN1 slot2 codes */
#define COREAPP_NOT_READY_MASK                0xF0000000  /* not ready yet */
#define COREAPP_NO_CODE_MASK                  0x00000000  /* no pin1 is required */

/* Slot 1 masks */
#define COREAPP_SLOT1_PRESENT_MASK            0x00000100
#define COREAPP_SLOT1_OPERATIONAL_VALID_MASK  0x00000200
#define COREAPP_SLOT1_APP_MASK                0x000000F0
#define COREAPP_SLOT1_APP_GSM_MASK            0x00000020
#define COREAPP_SLOT1_APP_USIM_MASK           0x00000040
#define COREAPP_SLOT1_APP_RUIM_MASK           0x00000080
#define COREAPP_SLOT1_CDMA_OPERATIONAL_MASK   0x00000002
#define COREAPP_SLOT1_GSM_OPERATIONAL_MASK    0x00000001
#define COREAPP_SLOT1_UMTS_OPERATIONAL_MASK   0x00000004
#define COREAPP_SLOT1_INFO_MASK               0x20000FFF

/* Slot 2 masks */
#define COREAPP_SLOT2_PRESENT_MASK            0x00100000
#define COREAPP_SLOT2_OPERATIONAL_VALID_MASK  0x00200000
#define COREAPP_SLOT2_APP_MASK                0x000F0000
#define COREAPP_SLOT2_APP_GSM_MASK            0x00020000
#define COREAPP_SLOT2_APP_USIM_MASK           0x00040000
#define COREAPP_SLOT2_APP_RUIM_MASK           0x00080000
#define COREAPP_SLOT2_CDMA_OPERATIONAL_MASK   0x00002000
#define COREAPP_SLOT2_GSM_OPERATIONAL_MASK    0x00001000
#define COREAPP_SLOT2_UMTS_OPERATIONAL_MASK   0x00004000
#define COREAPP_SLOT2_INFO_MASK               0x40FFF000
/* what subscription has been sent
   0x1 = GSM/UMTS
   0x2 = CDMA */
#define COREAPP_GW_SUBS_AVAIL_MASK            0x1
#define COREAPP_CDMA_SUBS_AVAIL_MASK          0x2
#define COREAPP_BOTH_SUBS_AVAIL_MASK          0x3
#define COREAPP_WLAN_SUBS_AVAIL_MASK          0x4
#define COREAPP_NOT_READY_SUBSCRIPTION_MASK   0x8
#define COREAPP_CARD1                         0x1
#define COREAPP_CARD2                         0x2
#define COREAPP_NO_CARD                       0x3

#define COREAPP_PENDING_SLOT_CHG1_MASK             0x05
#define COREAPP_PENDING_SLOT_CHG1_MASK_VALID_MASK  0x01
#define COREAPP_PENDING_SLOT_CHG1_REINIT1_MASK     0x04
#define COREAPP_PENDING_SLOT_CHG2_MASK             0x50
#define COREAPP_PENDING_SLOT_CHG2_REINIT2_MASK     0x40
#define COREAPP_PENDING_SLOT_CHG2_MASK_VALID_MASK  0x10

#define COREAPP_PLMN_MODE_BIT      0x80
#define COREAPP_CPHS_VAS_OFFSET    0xC0 /*Value Added Service*/

/* Types of addresses for LAN */
#define IPADDRESS   0
#define SUBNETMASK  1
#define GATEWAY     2
#define DNS         3
#define IP_BUFFER_SIZE      16      /* size of the IP string */


#ifdef  FEATURE_CPHS_EONS

/* Length values of the various fields in Corresponding SIM files */
#define COREAPP_NETWORK_PLMN_LEN            0x03
#define COREAPP_MAX_SPN_NAME_LEN            18
#define COREAPP_MAX_OPL_LIST_LEN            50
#define COREAPP_MAX_NETWORK_LONGNAME_LEN    40
#define COREAPP_MAX_NETWORK_SHORTNAME_LEN   20
#define COREAPP_MAX_PNN_ARRAY_SIZE          20
#define COREAPP_INVALID_NUM_RECS            -1
#define COREAPP_MAX_CPHS_OPNAME_LEN         50

/* Information Element Identifiers */
#define COREAPP_NETWORK_LONG_NAME_IEI       0x43
#define COREAPP_NETWORK_SHORT_NAME_IEI      0x45

/* Tag for Service Prov Desc Information */
#define COREAPP_NETWORK_SPDI_TAG            0x80


/* Country Initial and Coding Scheme Mask */
#define COREAPP_NETWORK_CI_MASK             0x80
#define COREAPP_NETWORK_CS_MASK             0x70
#define COREAPP_SPARE_BITS_MASK             0x07
#define COREAPP_GSM_DFLT_CODING_MASK        0x7F

#define COREAPP_FILE_ID_HIGH_BYTE           0x6F
#define COREAPP_FILE_ID_LOW_BYTE_PNN        0xC5
#define COREAPP_FILE_ID_LOW_BYTE_OPL        0xC6

#define COREAPP_PLMN_WILDCARD_CHAR          0x0D
#endif /* #ifdef  FEATURE_CPHS_EONS */

#ifdef FEATURE_MEID_SUPPORT
//Used to check if an ESN is a Pseudo ESN (i.e. MSB is 0x80)
#define COREAPP_PSEUDO_ESN_UBYTE            0x80
#endif /* #ifdef FEATURE_MEID_SUPPORT */

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/
#ifdef  FEATURE_CPHS_EONS
typedef struct
{
  uint8 m_displayCond;
  char  m_operatorName[COREAPP_MAX_SPN_NAME_LEN];
  AEECallback   m_spnCallback;
  AEECardGenericCmdStatus *m_pCardCmdStatus;
}CCoreAppSPNInfo;


typedef struct
{
  uint16 numPLMNs;
  AEECMPLMNID         *pPLMNIds;
  AEECallback   m_spdiCallback;
  AEECardGenericCmdStatus *m_pCardCmdStatus;
}CCoreAppSPDescInfo;


typedef struct
{
  AEECMPLMNID         PLMNId;
  AEECMLACID          lac1;
  AEECMLACID          lac2;
  byte                pnnRecNum;
}CCoreAppPLMNInfo;


typedef struct
{
  uint16 numPLMNs;
  uint16 numRecsRead;
  boolean bIsOPLInfoAvailable;
  CCoreAppPLMNInfo         plmnInfo[COREAPP_MAX_OPL_LIST_LEN];
  AEECardFileAttribStatus  m_FileAttrib;
  AEECallback              m_oplCallback;
  AEECardGenericCmdStatus  *m_pCardCmdStatus;
}CCoreAppOPLInfo;


typedef struct
{

  byte    szLongNetworkName[COREAPP_MAX_NETWORK_LONGNAME_LEN];
  uint8   longNameLen;
  byte    szShortNetworkName[COREAPP_MAX_NETWORK_SHORTNAME_LEN];
  uint8   shortNameLen;
  boolean bIsAddCI; //Country Initial
  uint16  codingScheme; // GSM 7 bit Default or UCS2
  uint8   spareBitsLongName;
  uint8   spareBitsShortName;
}CCoreAppPNNRecord;


typedef struct
{
  uint16 numRecsRead;
  boolean bIsPNNInfoAvailable;
  CCoreAppPNNRecord       m_pnnArray[COREAPP_MAX_PNN_ARRAY_SIZE];
  AEECallback             m_pnnCallback;
  AEECardFileAttribStatus  m_FileAttrib;
  AEECardGenericCmdStatus *m_pCardCmdStatus;
}CCoreAppPNNInfo;


typedef struct
{
  uint8 m_NameLen;
  char  m_operatorName[COREAPP_MAX_CPHS_OPNAME_LEN];
  AEECallback   m_cphsCallback;
  AEECardGenericCmdStatus *m_pCardCmdStatus;
}CCoreAppCPHSONSInfo;

#endif /* FEATURE_CPHS_EONS */

typedef struct
{
  /*First element of this structure must be AEEApplet. */
  AEEApplet a ;

  IImage *  m_pSplashImage;
  boolean   m_bSuspended;           /* Are we currently suspended? */
  uint16    m_cxScreen, m_cyScreen; /* What is the screen size? */
  IAnnunciator * m_pIAnn;           /* pointer to annunciator */
  IBitmap   * m_pIBitmap;           /* bitmap for primary display */
#ifdef FEATURE_SECONDARY_DISPLAY
  IDisplay  * m_pIDisp2;            /* pointer to secondary display */
  IBitmap   * m_pIBitmap2;          /* bitmap for secondary display */
  IStatic   * m_pTimeDate;          /* Time/date string for 2ndary display */
#endif
#ifdef FEATURE_NEW_ICARD
  ICard *   m_pICard;               /* pointer to icard interface */
#else
    ICARD *   m_pICard;               /* pointer to icard interface */
#endif /*FEATURE_NEW_ICARD */
  ICM   *   m_pICM;
  IConfig * m_pIConfig;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  ISUPPSVC * m_pISUPPSVC;              /* pointer to ISuppSvc interface */
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
  IDiag    *m_pIDiag;               /* pointer to IDIAG object */
#endif
#ifdef FEATURE_USRSYSCLOCK
  IUsrSysClock *m_pIUsrSysClock;    /* pointer to the IUSRSYSCLOCK object */
#endif // FEATURE_USRSYSCLOCK
  /**************************
        menu settings
  **************************/
  /* Each variable has its corresponding Get/Set methods so that other files
  can access them */

  /* Sound */
  byte      m_byRingerVolume;       /* current Ringer Volume level? */
  byte      m_byKeyBeepVolume;      /* current Keybeep Volume level? */
  byte      m_byEarpieceVolume;     /* current Earpiece Volume level? */
  byte      m_byMessageVolume;      /* current Msg alert Volume level? */
  uisnd_ringer_type m_RingerType;   /* current ringer type? */
  uint8     m_byBeepLength;          /* current key beep length */
#ifdef FEATURE_USRSYSCLOCK
  boolean   m_bNetworkTime;         /* Enable user time to be set based on the MM info */
#endif // FEATURE_USRSYSCLOCK
  boolean   m_bSmartSound;          /* AGC on? */
  boolean   m_bServiceAlert;        /* Service Alert on? */
  boolean   m_bMute;                /* Is the phone muted? */
  uint32    m_byTTY;                /* Is TTY on/talk/hear/off? */
  boolean   m_bMMRinger;
  boolean   m_bMMVolumeSetting;
  char *    m_pszMMRingerFile ;     /* MM ringer filename */
  char *    m_pszFileArray[MM_RINGER_MAX_FILES];
  int       m_nNumFiles;            // Number of files read into m_szFileArray
  AEEMediaData m_sMd;               // Media data
  IMedia *  m_pIMedia;              // Pointer to IMedia

  /* Display */
  byte      m_byBacklight;          /* current Backlight duration? */
  byte      m_byBklightIntensity;   /* current Backlight Intensity level? */
  uint32    m_nLcdTimer;
  AECHAR *  m_pszBanner;            /* current banner text */
  AECHAR *  m_pszTime;              /* Time string */
  AECHAR *  m_pszLastTime;          /* Time string displayed last time */
  AECHAR *  m_pszTimeDate;          /* Time/date string */
  AECHAR *  m_pszIdleMenuText;      /* Broadcast message string*/
  AECHAR *  m_pszDialString;        /* Dial string */
  boolean   m_bAutoHyphen;          /* Is Auto Hyphen on? */
  uint32  m_ss_request_id;

  /* Security */
  byte      m_wRestrict;           /* Is the phone restricted? */

  /* Extras */
  boolean   m_bAutoAnswer;          /* Auto Answer on? */
  byte      m_byAutoRetry;          /* Auto Retry duration? */
  boolean   m_bOneTouch;            /* One Touch dial on? */
  byte      m_byPrivacy;            /* standard or enhanced privacy mode */
  uint16    m_nNWSelectionMode;     /* Network Selection Mode */
  AEECMPhError m_PhError;           /* Error in ICM_GetNetworks */
  AEECMDetailedPLMNList *m_pDetailedPLMNList; /* The PLMN List returned by ICM_GetNetorks*/
  AEECallback m_nCallback;         /*GetNetworks callback.*/
  /* Key guard */
  byte      m_byKeyguard;           /* keyguard activation duration? */
  int       m_nKeyguardUnlockCodePos;  /* State of keycode code entry */

  /* Message display */
  uint16    m_MessageID;           /* Message ID to be displayed */
  AECHAR *  m_pszMsgString;        /* Message String to be displayed */


  /*********************************
      Phone hardware variables
  *********************************/
  boolean   m_bPwr;
  IBatt     *m_pIBatt;

  /* Other variables */
  boolean   m_bDemoAlert;                 /* is demo sound playing? */
  boolean   m_bUserPwrDown;               /* User Initiated powerdown ? */
  boolean   m_bExit;                      /* AEE is about to exit? */

  /***********************************
          For Security Module
   **********************************/
  AEECallback   m_sCallback;
  AEECallback   m_cspCallback;
#ifdef FEATURE_NEW_ICARD
  AEECardPinCode    m_sPIN;
  AEECardPinCode    m_sPUK;
#else
  AEEPINCode    m_sPIN;
  AEEPINCode    m_sPUK;
  AEEPINCbData  m_sPin_rsp_data;
  AEEGenericCbData m_sGeneric_rsp_data;
#endif /*FEATURE_NEW_ICARD */
  uint8         m_nCardStatus;
  uint8         m_sPIN1_lock_state;
  uint8         m_sPIN2_lock_state;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint8         m_nCardStatus_slot2;
  uint8         m_sPIN1_slot2_lock_state;
  uint8         m_sPIN2_slot2_lock_state;
  uint8         m_byGSMUserSlotPref;
  uint8         m_byCDMAUserSlotPref;  
  boolean       m_bSlotChangePending;  
  boolean       m_bUserMadeFirstSelSub;
  boolean       m_bTestAutomationMode;
#endif
  uint8         m_sNV_lock_state;
  boolean       m_bSecurityTimer;
  uint32        m_rtreConfig;
  uint32        m_rtreControl;
  uint32        m_dwSecurityMask;
  boolean       m_bConfigSent;
  boolean       m_bManualNWSelItem;
  boolean       mbReadFiles;
  uint8         m_bySubscriptionSent;  /* what subscription has been sent
                                          0x1 = GSM
                                          0x2 = CDMA */
  AEECardGenericCmdStatus *m_pCardCmdStatus;
  AEECardGenericCmdStatus *m_pCSPCardCmdStatus;
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  uint8         m_nPersoStatus;
  AEECardGetPersoFeatureCmdStatus m_sPersoFeatureInd;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  uint8         m_nPersoStatus_slot2;
  AEECardGetPersoFeatureCmdStatus m_sPersoFeatureInd_slot2;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

#ifdef FEATURE_CPHS_EONS
  CCoreAppSPNInfo         m_SPNInfo;
  CCoreAppSPDescInfo      m_SPDInfo;
  CCoreAppOPLInfo         m_OpListInfo;
  CCoreAppPNNInfo         m_PNNInfo;
  CCoreAppCPHSONSInfo     m_CPHSOpNameInfo;
  AEECMPLMNID             m_hplmn;
#endif /* FEATURE_CPHS_EONS */

  boolean       m_bPinVerified;

  /***********************************
         PH and SS INFO  Settings
   **********************************/
  AEECMPhInfo m_phInfo;
  AEECMSSInfo m_ssInfo;
#ifdef FEATURE_HDR
  boolean m_bHDRDisplay;
#endif

  // Battery related.
  unsigned int m_nlastIcon; // For the animating battery icon while charging
  boolean m_bLowBatt;
  boolean m_bVR;

  boolean m_bLPM;
  boolean m_bExtPwr;
  boolean m_bLPMTimer;

  boolean m_bNetSelErr;
  boolean m_bNetSel;
  AEECMDetailedPLMNList *m_pCopyDetailedPLMNList; /* The PLMN List returned by ICM_GetNetorks*/
  boolean setMode;
  boolean m_bProvisioned; 
  /***********************************
        Contrast Win related
  **********************************/
  uint16    m_byDispContrast;     /* display contrast level */
  uint16    m_byContrastMin;      /* max contrast from display driver */
  uint16    m_byContrastMax;      /* min contrast from display driver */
  uint16    m_byContrastDefault;  /* default contrast from display driver */
  ISTK      *m_pISTK;
  IDialog *m_pIdleDialog;

#if defined(FEATURE_MMOVERLAY)
   IImage     *m_pBgImage;
   boolean     m_bBgImageRendered;  /* Whether the bg image has been rendered
                                     * into th m_pBgImage buffer.
                                     */
   boolean     m_bBgImageDisplayed; /* Whether bg image has been displayed
                                     * to the screen.
                                     */
   IMMOverlay *m_pFgImgOverlay;
   IMMOverlay *m_pBgImgOverlay;
#endif /* FEATURE_MMOVERLAY */

} CCoreApp;

/* Define screen size values. */
#define COREAPP_QVGA_DIM1 240
#define COREAPP_QVGA_DIM2 320
#define COREAPP_VGA_DIM1  480
#define COREAPP_VGA_DIM2  640

/* Definitions for screen resolutions.  Note that a display may have the
 * indicated resolution in either landscape or portrait orientation.
 * COREAPP_VGA_RES means that the display is at least VGA (640x480)
 * COREAPP_QVGA_RES means that the display is at least QVGA (320x240) but
 *   not VGA.
 * COREAPP_SUB_QVGA_RES means that the screen is smaller than QVGA.
 */
#define COREAPP_VGA_RES         0x00000001
#define COREAPP_QVGA_RES        0x00000002
#define COREAPP_SUB_QVGA_RES    0x00000003

extern char *gpszResourceFile;

/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION: EndLPMMode

DESCRIPTION:
  Ends Low Power mode

SIDE EFFECTS:
  None.

=============================================================================*/
void EndLPMMode(CCoreApp *pMe, boolean changeMode);

/*===========================================================================
FUNCTION: CoreApp_Invoke_DlgEventHandler

DESCRIPTION: 
  Invokes the DlgEventHandler corresponding to the Dlg ID.

SIDE EFFECTS:
  None
===========================================================================*/
boolean CoreApp_InvokeDlgEventHandler(uint16 wDlgID, CCoreApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*=============================================================================
FUNCTION: ValidateCardsAndChangeState

DESCRIPTION:
    This function is used called everytime we receive a card event
  and it validates the cards and depending on the card events and
  pin states makes a transition to the right state

SIDE EFFECTS:
  None.

=============================================================================*/
void ValidateCardsAndChangeState (CCoreApp *pMe,boolean bAllowByPass);


/*=============================================================================
FUNCTION: CoreApp_SetOvImg

DESCRIPTION:
  This function is to register/enable overlay images.

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_SetOvImg(CCoreApp * pMe);

#if defined(FEATURE_ENHANCED_OVERLAY) || defined(FEATURE_MMOVERLAY)
/*=============================================================================
FUNCTION: CoreApp_ToggleBgImage

DESCRIPTION:
  This function enables or disables the background image.

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_ToggleBgImage(CCoreApp * pMe, boolean bTurnOn);
#endif /* FEATURE_ENHANCED_OVERLAY || FEATURE_MMOVERLAY */

#endif /* COREAPP_H */


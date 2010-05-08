/*===========================================================================

FILE: AUXWLANMenu.c

DESCRIPTION
   This file contains the AUXWLAN Configurations applet's menu related functions.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   Functions in this file is being called by AUXApp.c.

       Copyright   2007-2008 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/08   sg      Fix Kernel Panic when END key is pressed during AUXSettings
03/26/08   cvs     Remove deprecated functions
10/01/07   cvs     Remove AEECMPhInfo from stack
06/12/07   ypmw    Fix the start adhoc problem that network select mode switched
                   to automatic mode mistakingly
03/30/07   ypmw    Fix the manual scan selection problem
03/27/07   ypmw    Fix the compiler warning
03/26/07   ypmw    Add EAP authentication scheme for automatic wlan mode
03/23/07   ypmw    Fix crash on WLAN mode select when hitting EXIT button
01/31/07   ypmw    Fix removing security options
01/29/07   ypmw    Fix compiler and lint warnings
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings

===========================================================================*/


/*===============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AUXWLANMenu.h"

#define MAX_STR_SIZE 50

/* Number of msec to wait between updates to the WLAN status screen */
#define AUX_WLAN_STATUS_UPDATE_TIME 2000

/* Number of statistics lines to display */
#define MAX_NUM_WLAN_STATS 4

/* Size of text control for WLAN Status screen */
#define AUX_WLAN_STATUS_NUM_CHARS (MAX_NUM_WLAN_STATS * MAX_STR_SIZE)

#define AUX_MIN_VAL(a, b) (((a) <= (b)) ? (a) : (b))

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/

/* The AUXWLANMenu_SecFieldEditStruct contains values used in the initialization
 * and processing of dialogs in which the user edits specific WLAN
 * security fields (e.g. 64-bit WEP key 1)
 */

typedef struct
{
  uint16  nStrResId;
  int32   nWIFIOptId;
  uint16  nMenuItemId;
  boolean bIsNumeric;
} AUXWLANMenu_SecFieldEditStruct;

/* The following value in the nStrResId field indicates the end of
 * the security field edit data table.
 */
#define AUXAPP_SEC_FIELD_EDIT_END 0xFFFF

static const AUXWLANMenu_SecFieldEditStruct WEP64EditData[] =
{
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
  { IDS_WLAN_SEC_WEP_KEY_ID, AEEWIFI_OPT_WEP_64BIT_KEY_ID,
      IDL_WLAN_SEC_WEP64_KEY_ID, TRUE },
  { IDS_WLAN_SEC_WEP_KEY1, AEEWIFI_OPT_WEP_64BIT_KEY_0,
      IDL_WLAN_SEC_WEP64_KEY1, FALSE },
  { IDS_WLAN_SEC_WEP_KEY2, AEEWIFI_OPT_WEP_64BIT_KEY_1,
      IDL_WLAN_SEC_WEP64_KEY2, FALSE },
  { IDS_WLAN_SEC_WEP_KEY3, AEEWIFI_OPT_WEP_64BIT_KEY_2,
      IDL_WLAN_SEC_WEP64_KEY3, FALSE },
  { IDS_WLAN_SEC_WEP_KEY4, AEEWIFI_OPT_WEP_64BIT_KEY_3,
      IDL_WLAN_SEC_WEP64_KEY4, FALSE },
#else
  { IDS_WLAN_SEC_WEP_KEY_ID, AEEWIFI_OPT_WEP_KEY_ID,
      IDL_WLAN_SEC_WEP64_KEY_ID, TRUE },
  { IDS_WLAN_SEC_WEP_KEY1, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP64_KEY1, FALSE },
  { IDS_WLAN_SEC_WEP_KEY2, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP64_KEY2, FALSE },
  { IDS_WLAN_SEC_WEP_KEY3, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP64_KEY3, FALSE },
  { IDS_WLAN_SEC_WEP_KEY4, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP64_KEY4, FALSE },
#endif
  { AUXAPP_SEC_FIELD_EDIT_END, 0, 0, FALSE },
};

static const AUXWLANMenu_SecFieldEditStruct WEP128EditData[] =
{
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
  { IDS_WLAN_SEC_WEP_KEY_ID, AEEWIFI_OPT_WEP_128BIT_KEY_ID,
      IDL_WLAN_SEC_WEP128_KEY_ID, TRUE },
  { IDS_WLAN_SEC_WEP_KEY1, AEEWIFI_OPT_WEP_128BIT_KEY_0,
      IDL_WLAN_SEC_WEP128_KEY1, FALSE },
  { IDS_WLAN_SEC_WEP_KEY2, AEEWIFI_OPT_WEP_128BIT_KEY_1,
      IDL_WLAN_SEC_WEP128_KEY2, FALSE },
  { IDS_WLAN_SEC_WEP_KEY3, AEEWIFI_OPT_WEP_128BIT_KEY_2,
      IDL_WLAN_SEC_WEP128_KEY3, FALSE },
  { IDS_WLAN_SEC_WEP_KEY4, AEEWIFI_OPT_WEP_128BIT_KEY_3,
      IDL_WLAN_SEC_WEP128_KEY4, FALSE },
#else
  { IDS_WLAN_SEC_WEP_KEY_ID, AEEWIFI_OPT_WEP_KEY_ID,
      IDL_WLAN_SEC_WEP128_KEY_ID, TRUE },
  { IDS_WLAN_SEC_WEP_KEY1, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP128_KEY1, FALSE },
  { IDS_WLAN_SEC_WEP_KEY2, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP128_KEY2, FALSE },
  { IDS_WLAN_SEC_WEP_KEY3, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP128_KEY3, FALSE },
  { IDS_WLAN_SEC_WEP_KEY4, AEEWIFI_OPT_WEP_KEY,
      IDL_WLAN_SEC_WEP128_KEY4, FALSE },
#endif
  { AUXAPP_SEC_FIELD_EDIT_END, 0, 0, FALSE },
};

static const AUXWLANMenu_SecFieldEditStruct WPAPSKEditData[] =
{
  { IDS_PASSWORD, AEEWIFI_OPT_8021X_PASSWORD,
      IDL_WLAN_SEC_WPA_PSK_PASSWORD, FALSE },
  { AUXAPP_SEC_FIELD_EDIT_END, 0, 0, FALSE },
};

static const AUXWLANMenu_SecFieldEditStruct WPATLSCertEditData[] =
{
  { IDS_USER_ID, AEEWIFI_OPT_8021X_ID,
      IDL_WLAN_SEC_WPA_TLS_CERT_USERID, FALSE },
  { AUXAPP_SEC_FIELD_EDIT_END, 0, 0, FALSE },
};

static const AUXWLANMenu_SecFieldEditStruct WPAMD5EditData[] =
{
  { IDS_USER_ID, AEEWIFI_OPT_8021X_ID,
      IDL_WLAN_SEC_WPA_MD5_USERID, FALSE },
  { IDS_PASSWORD, AEEWIFI_OPT_8021X_PASSWORD,
      IDL_WLAN_SEC_WPA_MD5_PASSWORD, FALSE },
  { AUXAPP_SEC_FIELD_EDIT_END, 0, 0, FALSE },
};

/* The AUXWLANMenu_SecEditStruct contains values used in the initialization and
 * processing of dialogs in which the user selects WLAN security fields to
 * edit (e.g. 64-bit WEP security options).
 */
typedef struct
{
  uint16                           nDlgId;
  uint16                           nSKId;        /* Soft key menu ID */
  uint16                           nEditMenuId;  /* Selection menu */
  uint16                           nSKSave;      /* Save button on SK menu */
  uint16                           nSKCancel;    /* Cancel button on SK menu */
  uint32                           nEncrypType;
  uint32                           nAuthType;
  uint16                           nTitleResID;
  const AUXWLANMenu_SecFieldEditStruct  *pSecFieldData;
} AUXWLANMenu_SecEditStruct;

static const AUXWLANMenu_SecEditStruct WLANSecEditData[] =
{
  { IDD_WLAN_SEC_WEP64, IDC_SK_WLAN_SEC_WEP64, IDC_WLAN_SEC_WEP64,
     IDL_WLAN_SEC_WEP64_SAVE, IDL_WLAN_SEC_WEP64_CANCEL,
     AEEWIFI_ENCRYPTION_WEP_64bit, AEEWIFI_AUTHENTICATION_WEP,
     IDS_WLAN_SEC_WEP64_SECURITY, WEP64EditData },

  { IDD_WLAN_SEC_WEP128, IDC_SK_WLAN_SEC_WEP128, IDC_WLAN_SEC_WEP128,
     IDL_WLAN_SEC_WEP128_SAVE, IDL_WLAN_SEC_WEP128_CANCEL,
    AEEWIFI_ENCRYPTION_WEP_128bit, AEEWIFI_AUTHENTICATION_WEP,
    IDS_WLAN_SEC_WEP128_SECURITY, WEP128EditData },

  { IDD_WLAN_SEC_WPA_PSK, IDC_SK_WLAN_SEC_WPA_PSK, IDC_MENU_WLAN_SEC_WPA_PSK,
    IDL_WLAN_SEC_WPA_PSK_SAVE, IDL_WLAN_SEC_WPA_PSK_CANCEL,
    AEEWIFI_ENCRYPTION_TKIP, AEEWIFI_AUTHENTICATION_8021X_PASSWORD,
    IDS_WLAN_SEC_WPA_PSK, WPAPSKEditData },

  { IDD_WLAN_SEC_WPA_TLS_CERT, IDC_SK_WLAN_SEC_WPA_TLS_CERT, IDC_MENU_WLAN_SEC_WPA_TLS_CERT,
    IDL_WLAN_SEC_WPA_TLS_CERT_SAVE, IDL_WLAN_SEC_WPA_TLS_CERT_CANCEL,
    AEEWIFI_ENCRYPTION_TKIP, AEEWIFI_AUTHENTICATION_8021X_TLS,
    IDS_WLAN_SEC_WPA_TLS_CERT, WPATLSCertEditData },

  { IDD_WLAN_SEC_WPA_MD5, IDC_SK_WLAN_SEC_WPA_MD5, IDC_MENU_WLAN_SEC_WPA_MD5,
    IDL_WLAN_SEC_WPA_MD5_SAVE, IDL_WLAN_SEC_WPA_MD5_CANCEL,
    AEEWIFI_ENCRYPTION_TKIP, AEEWIFI_AUTHENTICATION_8021X_MD5,
    IDS_WLAN_SEC_WPA_MD5, WPAMD5EditData }
};

/* WLAN Channel values
 * CHANNEL_BUFFER_SIZE is the UI buffer text size
 * AUX_WLAN_CHANNEL_ANY is the value used in the UI for "any channel";
 *   manipulating this value is easier than manipulating the AEEWIFI
 *   value for "any".
 */
#define AUX_CHANNEL_BUFFER_SIZE     2
#define AUX_WORD_BUFFER_SIZE        10      /* Size of input buffer for 32-bit-value */

/* WEP key strings must be exactly twice the number of bytes
 * in the key, since each character converts to 4 bits.
 */
#define AUXWLAM_WEP64_FIELD_SIZE (2 * AEECM_64BIT_WEP_KEY_SIZE)
#define AUXWLAM_WEP128_FIELD_SIZE (2 * AEECM_128BIT_WEP_KEY_SIZE)

const WLANEditDlgDataType WLANEditDlgTbl[] = {

  /* SSID Option Entry */
  { IDD_ENTER_SSID,
      IDC_SK_SSID_EDIT, IDL_SSID_EDIT_OK,
      IDC_TEXT_SSID_EDIT, AEEWIFI_OPT_SSID,
      AEECM_MAX_SSID_STR_SIZE, AEE_TM_LETTERS, TRUE, FALSE,
      IDD_WLAN_SETTINGS },

  /* Channel Option Entry */
  { IDD_ENTER_CHANNEL,
      IDC_SK_CHANNEL_EDIT, IDL_CHANNEL_EDIT_OK,
      IDC_TEXT_CHANNEL_EDIT, AEEWIFI_OPT_CHANNEL,
      AUX_CHANNEL_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_SETTINGS },

  /* Listen Interval Option Entry */
  { IDD_WLAN_LISTEN_INTERVAL_EDIT,
      IDC_SK_WLAN_LISTEN_EDIT, IDL_WLAN_LISTEN_EDIT_OK,
      IDC_TEXT_WLAN_LISTEN_EDIT, AEEWIFI_OPT_LISTEN_INTERVAL,
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_GENERAL_CONFIG },

  /* RTS Threshold Option Entry */
  { IDD_WLAN_RTS_EDIT,
      IDC_SK_WLAN_RTS_EDIT, IDL_WLAN_RTS_EDIT_OK,
      IDC_TEXT_WLAN_RTS_EDIT, AEEWIFI_OPT_RTSTHRESHOLD,
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_GENERAL_CONFIG },

  /* Scan Timing Option Entry */
  { IDD_WLAN_SCAN_TIME_EDIT,
      IDC_SK_WLAN_SCAN_TIME_EDIT, IDL_WLAN_SCAN_TIME_EDIT_OK,
      IDC_TEXT_WLAN_SCAN_TIME_EDIT, AEEWIFI_OPT_SCANTIMING,
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_GENERAL_CONFIG },

  /* Framentation Threshold Option Entry */
  { IDD_WLAN_FRAG_EDIT,
      IDC_SK_WLAN_FRAG_EDIT, IDL_WLAN_FRAG_EDIT_OK,
      IDC_TEXT_WLAN_FRAG_EDIT, AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD,
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_GENERAL_CONFIG },

  /*  WLAN Max Tx Power  */
  { IDD_WLAN_MAX_TX_POWER_EDIT,
      IDC_SK_WLAN_MAX_TX_POWER_EDIT, IDL_WLAN_MAX_TX_POWER_EDIT_OK,
      IDC_TEXT_WLAN_MAX_TX_POWER_EDIT, AEEWIFI_OPT_POWERLEVEL,
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, TRUE, FALSE,
      IDD_WLAN_GENERAL_CONFIG },

  /* 64-bit WEP Key ID */
  { IDD_WLAN_SEC_WEP64_KEY_ID,
      IDC_SK_WLAN_SEC_WEP64_KEY_ID, IDL_WLAN_SEC_WEP64_KEY_ID_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP64_KEY_ID, AEEWIFI_OPT_WEP_64BIT_KEY_ID,
#else
      IDC_TEXT_WLAN_SEC_WEP64_KEY_ID, AEEWIFI_OPT_WEP_KEY_ID,
#endif
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 64-bit WEP Key 1 */
  { IDD_WLAN_SEC_WEP64_KEY1,
      IDC_SK_WLAN_SEC_WEP64_KEY1, IDL_WLAN_SEC_WEP64_KEY1_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP64_KEY1, AEEWIFI_OPT_WEP_64BIT_KEY_0,
#else
      IDC_TEXT_WLAN_SEC_WEP64_KEY1, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP64_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 64-bit WEP Key 2 */
  { IDD_WLAN_SEC_WEP64_KEY2,
      IDC_SK_WLAN_SEC_WEP64_KEY2, IDL_WLAN_SEC_WEP64_KEY2_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP64_KEY2, AEEWIFI_OPT_WEP_64BIT_KEY_1,
#else
      IDC_TEXT_WLAN_SEC_WEP64_KEY2, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP64_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 64-bit WEP Key 3 */
  { IDD_WLAN_SEC_WEP64_KEY3,
      IDC_SK_WLAN_SEC_WEP64_KEY3, IDL_WLAN_SEC_WEP64_KEY3_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP64_KEY3, AEEWIFI_OPT_WEP_64BIT_KEY_2,
#else
      IDC_TEXT_WLAN_SEC_WEP64_KEY3, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP64_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 64-bit WEP Key 4 */
  { IDD_WLAN_SEC_WEP64_KEY4,
      IDC_SK_WLAN_SEC_WEP64_KEY4, IDL_WLAN_SEC_WEP64_KEY4_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP64_KEY4, AEEWIFI_OPT_WEP_64BIT_KEY_3,
#else
      IDC_TEXT_WLAN_SEC_WEP64_KEY4, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP64_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },


  /* 128-bit WEP Key ID */
  { IDD_WLAN_SEC_WEP128_KEY_ID,
      IDC_SK_WLAN_SEC_WEP128_KEY_ID, IDL_WLAN_SEC_WEP128_KEY_ID_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP128_KEY_ID, AEEWIFI_OPT_WEP_128BIT_KEY_ID,
#else
      IDC_TEXT_WLAN_SEC_WEP128_KEY_ID, AEEWIFI_OPT_WEP_KEY_ID,
#endif
      AUX_WORD_BUFFER_SIZE, AEE_TM_NUMBERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 128-bit WEP Key 1 */
  { IDD_WLAN_SEC_WEP128_KEY1,
      IDC_SK_WLAN_SEC_WEP128_KEY1, IDL_WLAN_SEC_WEP128_KEY1_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP128_KEY1, AEEWIFI_OPT_WEP_128BIT_KEY_0,
#else
      IDC_TEXT_WLAN_SEC_WEP128_KEY1, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP128_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 128-bit WEP Key 2 */
  { IDD_WLAN_SEC_WEP128_KEY2,
      IDC_SK_WLAN_SEC_WEP128_KEY2, IDL_WLAN_SEC_WEP128_KEY2_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP128_KEY2, AEEWIFI_OPT_WEP_128BIT_KEY_1,
#else
      IDC_TEXT_WLAN_SEC_WEP128_KEY2, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP128_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 128-bit WEP Key 3 */
  { IDD_WLAN_SEC_WEP128_KEY3,
      IDC_SK_WLAN_SEC_WEP128_KEY3, IDL_WLAN_SEC_WEP128_KEY3_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP128_KEY3, AEEWIFI_OPT_WEP_128BIT_KEY_2,
#else
      IDC_TEXT_WLAN_SEC_WEP128_KEY3, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP128_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* 128-bit WEP Key 4 */
  { IDD_WLAN_SEC_WEP128_KEY4,
      IDC_SK_WLAN_SEC_WEP128_KEY4, IDL_WLAN_SEC_WEP128_KEY4_OK,
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      IDC_TEXT_WLAN_SEC_WEP128_KEY4, AEEWIFI_OPT_WEP_128BIT_KEY_3,
#else
      IDC_TEXT_WLAN_SEC_WEP128_KEY4, AEEWIFI_OPT_WEP_KEY,
#endif
      AUXWLAM_WEP128_FIELD_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* WPA PSK Password */
  { IDD_WLAN_SEC_WPA_PSK_PASSWORD,
      IDC_SK_WLAN_SEC_WPA_PSK_PASSWORD, IDL_SK_WLAN_SEC_WPA_PSK_PASSWORD_OK,
      IDC_TEXT_WLAN_SEC_WPA_PSK_PASSWORD, AEEWIFI_OPT_8021X_PASSWORD,
      AEECM_8021X_PASSWD_STR_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* WPA TLS Certificate User Id */
  { IDD_WLAN_SEC_WPA_TLS_CERT_USERID,
      IDC_SK_WLAN_SEC_WPA_TLS_CERT_USERID, IDL_SK_WLAN_SEC_WPA_TLS_CERT_USERID_OK,
      IDC_TEXT_WLAN_SEC_WPA_TLS_CERT_USERID, AEEWIFI_OPT_8021X_ID,
      AEECM_8021X_USER_ID_STR_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* WPA MD5 User Id */
  { IDD_WLAN_SEC_WPA_MD5_USERID,
      IDC_SK_WLAN_SEC_WPA_MD5_USERID, IDL_SK_WLAN_SEC_WPA_MD5_USERID_OK,
      IDC_TEXT_WLAN_SEC_WPA_MD5_USERID, AEEWIFI_OPT_8021X_ID,
      AEECM_8021X_USER_ID_STR_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS },

  /* WPA MD5 Password */
  { IDD_WLAN_SEC_WPA_MD5_PASSWORD,
      IDC_SK_WLAN_SEC_WPA_MD5_PASSWORD, IDL_SK_WLAN_SEC_WPA_MD5_PASSWORD_OK,
      IDC_TEXT_WLAN_SEC_WPA_MD5_PASSWORD, AEEWIFI_OPT_8021X_PASSWORD,
      AEECM_8021X_PASSWD_STR_SIZE, AEE_TM_LETTERS, FALSE, TRUE,
      IDD_WLAN_SETTINGS }
};
static const int32 WLANSecOptTbl [] = 
{
  AEEWIFI_OPT_WEP_KEY,
  AEEWIFI_OPT_WEP_KEY_ID,
  AEEWIFI_OPT_WEP_64BIT_KEY_ID,
  AEEWIFI_OPT_WEP_64BIT_KEY_0,
  AEEWIFI_OPT_WEP_64BIT_KEY_1,
  AEEWIFI_OPT_WEP_64BIT_KEY_2,
  AEEWIFI_OPT_WEP_64BIT_KEY_3,
  AEEWIFI_OPT_WEP_128BIT_KEY_ID,
  AEEWIFI_OPT_WEP_128BIT_KEY_0,
  AEEWIFI_OPT_WEP_128BIT_KEY_1,
  AEEWIFI_OPT_WEP_128BIT_KEY_2,
  AEEWIFI_OPT_WEP_128BIT_KEY_3,
  AEEWIFI_OPT_8021X_PASSWORD,
  AEEWIFI_OPT_8021X_ID
};

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION InitWLANOptEditDlg

DESCRIPTION
  Initialize a WLAN Option Edit Dialog with current option value and
  appropriate ITextCtl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitWLANOptEditDlg(AUXApp *pMe, IDialog* pIActiveDialog)
{
  ITextCtl *pTextEdit;           /* Text control pointer */
  IMenuCtl *pISoftKeyMenu;       /* Softkey menu control pointer */
  AEERect   MenuRect;
  AEERect   TextRect;
  boolean   bReturnStatus;
  const WLANEditDlgDataType *pDlgData = NULL;  /* Data for this dialog */
  uint16    nDlgId;
  int       nWIFIRetVal;
  WIFIOpt   tmp_Opt;

  if (pMe == NULL || pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE; /* error */
  }

  /* Get the WLAN Edit Dialog table entry */
  nDlgId = IDIALOG_GetID(pIActiveDialog);
  pDlgData = AUXGetWLANEditDlgTbl(nDlgId);

  if (pDlgData == NULL)
  {
    /* If we couldn't find an entry for this dialog, we can't continue. */
    AUX_ERR("No Dialog entry in table for dialog %d.", nDlgId, 0, 0);
    return FALSE;
  }

  /* Get pointers to the controls on the dialog */
  pTextEdit = (ITextCtl*)IDIALOG_GetControl(pIActiveDialog,
                                            (int16)pDlgData->nTextId);
  pISoftKeyMenu = (IMenuCtl*)IDIALOG_GetControl(pIActiveDialog,
                                                (int16)pDlgData->nSKId);
  if((pTextEdit == NULL) || (pISoftKeyMenu == NULL)) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Update the softkey menu */
  bReturnStatus = IMENUCTL_DeleteAll(pISoftKeyMenu);
  if (bReturnStatus == FALSE) {
    AUX_ERR ("Could not delete menu items", 0, 0, 0);
  }

  bReturnStatus = IMENUCTL_AddItem(pISoftKeyMenu, gpAUXResourceFile, IDS_OK,
                                   pDlgData->nSKOKId, NULL, 0);
  if (bReturnStatus == FALSE) {
    AUX_ERR ("Could not add menu item", 0, 0, 0);
  }

  ITEXTCTL_SetSoftKeyMenu(pTextEdit, NULL);

  AUXSetDefaultSoftkeyLook(pMe->a.m_pIShell, pISoftKeyMenu);
  bReturnStatus = IMENUCTL_Redraw(pISoftKeyMenu);
  if (bReturnStatus == FALSE) {
    AUX_ERR ("Could not redraw menu item", 0, 0, 0);
  }
  /* Set Text control size */
  IMENUCTL_GetRect(pISoftKeyMenu, &MenuRect);
  ITEXTCTL_GetRect(pTextEdit, &TextRect);
  TextRect.dy = MenuRect.y - 1;

  ITEXTCTL_SetRect(pTextEdit, &TextRect);

  /* Set textctl properties */
  ITEXTCTL_SetProperties(pTextEdit, TP_FRAME);
  ITEXTCTL_SetMaxSize(pTextEdit, pDlgData->nTextFieldSz);
  (void)ITEXTCTL_SetInputMode(pTextEdit, pDlgData->TextInputMode);

  /* Get Option text from IWIFI */
  (void)ITEXTCTL_SetText(pTextEdit, NULL, 0);
  if (pMe->m_WLAN.m_bWIFIOptsLoaded == FALSE)
  {
    nWIFIRetVal = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
    if (SUCCESS != nWIFIRetVal)
    {
      /* Failed to load option */
      AUX_ERR("Failed to load options, error %d.", nWIFIRetVal, 0, 0);
      return FALSE;
    }
  }

  /* If it's one of the WEP keys, we need to construct the title */
  switch (pDlgData->nWIFIOptId)
  {
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
#else
    case AEEWIFI_OPT_WEP_KEY:
#endif
      {
        AECHAR *pszwResStr;
        char   *pszResStr;
        uint32  nDisplayKeyId;
        int     nErrCode;
        boolean bSuccess = FALSE;

        /* Use a do..while(0) to simplify cleanup */
        do
        {

          pszwResStr = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
          pszResStr = (char *)MALLOC(MAX_STR_SIZE);
          if ((! pszwResStr) || (! pszResStr))
          {
            AUX_ERR("No memory for option %d buffer, 0x%x 0x%x",
                     pDlgData->nWIFIOptId, pszwResStr, pszResStr);
            break;
          }

#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
          /* If we have the updated WIFI WEP parameters, use those.
           * Otherwise, revert to using the dialog id.
           */
          if ((pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_0) ||
              (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_1) ||
              (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_2) ||
              (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_3))
#else
          if ((nDlgId == IDD_WLAN_SEC_WEP64_KEY_ID) ||
              (nDlgId == IDD_WLAN_SEC_WEP64_KEY1) ||
              (nDlgId == IDD_WLAN_SEC_WEP64_KEY2) ||
              (nDlgId == IDD_WLAN_SEC_WEP64_KEY3) ||
              (nDlgId == IDD_WLAN_SEC_WEP64_KEY4))
#endif
          {
            nErrCode = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDS_WLAN_SEC_WEP64_KEY, pszwResStr,
                                            (MAX_STR_SIZE * sizeof(AECHAR)));
            if (nErrCode == 0)
            {
              AUX_ERR("Failed to load string, %d", nErrCode, 0, 0);
              break;
            }

#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
            if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_0)
            {
              nDisplayKeyId = 1;
            }
            else if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_1)
            {
              nDisplayKeyId = 2;
            }
            else if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_2)
            {
              nDisplayKeyId = 3;
            }
            else
            {
              nDisplayKeyId = 4;
            }
#else
            nDisplayKeyId = 1;
#endif
          }
          else
          {
            nErrCode = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDS_WLAN_SEC_WEP128_KEY, pszwResStr,
                                            (MAX_STR_SIZE * sizeof(AECHAR)));
            if (nErrCode == 0)
            {
              AUX_ERR("Failed to load string, %d", nErrCode, 0, 0);
              break;
            }

#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
            if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_128BIT_KEY_0)
            {
              nDisplayKeyId = 1;
            }
            else if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_128BIT_KEY_1)
            {
              nDisplayKeyId = 2;
            }
            else if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_128BIT_KEY_2)
            {
              nDisplayKeyId = 3;
            }
            else
            {
              nDisplayKeyId = 4;
            }
#else
            nDisplayKeyId = 1;
#endif
          }

          (void)WSTRTOSTR(pszwResStr, pszResStr, MAX_STR_SIZE);
          (void)SNPRINTF(pszResStr, MAX_STR_SIZE, "%s %d", pszResStr,
                         nDisplayKeyId);
          (void)STRTOWSTR(pszResStr, pszwResStr, MAX_STR_SIZE * sizeof(AECHAR));

          bReturnStatus = ITEXTCTL_SetTitle(pTextEdit, NULL, 0, pszwResStr);
          if (bReturnStatus != TRUE)
          {
            AUX_MSG("Failed to set title for option %d", pDlgData->nWIFIOptId,
                     0, 0);
            break;
          }

          bSuccess = TRUE;
        } while (0);

        FREEIF(pszwResStr);
        FREEIF(pszResStr);
        if (bSuccess == FALSE)
        {
          return FALSE;
        }
      }
      break;

    default:
      break;
  }

  /* Get the option */
  nWIFIRetVal = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                 (int32)pDlgData->nWIFIOptId, &tmp_Opt );
  if (SUCCESS == nWIFIRetVal)
  {
    if (pDlgData->TextInputMode == AEE_TM_NUMBERS)
    {
      char   *pszOptString;
      AECHAR *pszAEOptString;
      uint32  nOptVal = (uint32)tmp_Opt.pVal;

      /* If the user is editing one of the WEP key ids, we need to
       * convert the zero-based value stored in IWIFI to the one-based
       * value presented to the user.
       */
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      if ((pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_ID) ||
          (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_128BIT_KEY_ID))
#else
      if (pDlgData->nWIFIOptId == AEEWIFI_OPT_WEP_KEY_ID)
#endif
      {
        nOptVal++;
      }
      else if (pDlgData->nWIFIOptId == AEEWIFI_OPT_CHANNEL)
      {
        /* Convert to the local "any" value */
        if (nOptVal == AEEWIFI_CHANNEL_ANY)
        {
          nOptVal = AUX_WLAN_CHANNEL_ANY;
        }
      }

      pszOptString = (char *)MALLOC(pDlgData->nTextFieldSz + 1);
      if (! pszOptString)
      {
        AUX_ERR("No memory for option %d buffer.", pDlgData->nWIFIOptId,
                 0, 0);
        return FALSE;
      }

      pszAEOptString = (AECHAR *)MALLOC((pDlgData->nTextFieldSz + 1 ) *
                                        sizeof(AECHAR));
      if (! pszAEOptString)
      {
        AUX_ERR("No memory for option %d AECHAR buffer.",
                 pDlgData->nWIFIOptId, 0, 0);
        FREE(pszOptString);
        return FALSE;
      }

      (void)SNPRINTF(pszOptString, pDlgData->nTextFieldSz+1, "%d", nOptVal);

      (void)STRTOWSTR(pszOptString, pszAEOptString,
                      ((pDlgData->nTextFieldSz + 1) * sizeof(AECHAR)));
      /* Copy the contents. */
      (void)ITEXTCTL_SetText(pTextEdit, pszAEOptString, (int)pDlgData->nTextFieldSz);
      FREE(pszOptString);
      FREE(pszAEOptString);
    }
    else if (pDlgData->TextInputMode == AEE_TM_LETTERS)
    {
      /* Copy the contents. */
      (void)ITEXTCTL_SetText(pTextEdit, tmp_Opt.pVal, (int)pDlgData->nTextFieldSz);
    }
    else
    {
      AUX_ERR("Unsupported text input type, %d.", pDlgData->TextInputMode,
               0, 0);
      return FALSE;
    }
  }
  else
  {
    /* Failed to get option */
    AUX_ERR("Failed to get option %d, error %d.", pDlgData->TextInputMode,
             nWIFIRetVal, 0);
    return FALSE;
  }

  ITEXTCTL_SetCursorPos (pTextEdit, TC_CURSOREND);
  return TRUE;

} /* End InitWLANOptEditDlg  */

/*===========================================================================
FUNCTION InitEditIpDlg

DESCRIPTION
  Initializes the edit dialog for IP-style inputs (e.g. IP addr, subnet, etc

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitEditIpDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IStatic *pIStaticCtl = NULL;
  AECHAR editType[MAX_STR_SIZE];
  ITextCtl *pITextCtl = NULL;
  AECHAR *IP_Value;
  IMenuCtl* pISoftKeyMenu; /* softkey menu */

  if ( pMe == NULL || pIActiveDialog == NULL ) { /* error checking */
     AUX_ERR ("Null pointer", 0,0,0);
     return FALSE;
  }

  switch(pMe->m_WLAN.m_IpTypeSelected)
  {
    case IPADDRESS:
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_EDIT_IP,
                                 editType, sizeof(AECHAR)* MAX_STR_SIZE);
      IP_Value = pMe->m_WLAN.m_IpAddr;
      break;
    case SUBNETMASK:
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_EDIT_SUBNET,
                                 editType, sizeof(AECHAR)* MAX_STR_SIZE);
      IP_Value = pMe->m_WLAN.m_SubnetMask;
      break;
    case GATEWAY:
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_EDIT_GATEWAY,
                                 editType, sizeof(AECHAR)* MAX_STR_SIZE);
      IP_Value = pMe->m_WLAN.m_Gateway;
      break;
    case DNS:
      (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_EDIT_DNS,
                                 editType, sizeof(AECHAR)* MAX_STR_SIZE);
      IP_Value = pMe->m_WLAN.m_Dns;
      break;
    default:
      AUX_ERR("Unknown IP type",0,0,0);
      return FALSE;
  }

  pIStaticCtl = (IStatic*) IDIALOG_GetControl(pIActiveDialog, IDC_STATIC_EDIT_IP_TITLE);
  if ( pIStaticCtl == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  (void)ISTATIC_SetText(pIStaticCtl, NULL, editType, AEE_FONT_NORMAL, AEE_FONT_NORMAL);

  // Add frame around the text control.
  pITextCtl = (ITextCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_TEXT_EDIT_IP);
  if ( pITextCtl == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  ITEXTCTL_SetProperties(pITextCtl, TP_FRAME);
  (void)ITEXTCTL_SetInputMode(pITextCtl, AEE_TM_NUMBERS);
  (void)ITEXTCTL_SetText(pITextCtl, IP_Value, -1);

  pISoftKeyMenu = (IMenuCtl *) IDIALOG_GetControl(pIActiveDialog, IDC_SOFTKEY_EDIT_IP);
  (void) IMENUCTL_DeleteAll(pISoftKeyMenu);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, gpAUXResourceFile, IDS_OK, IDL_SK_EDIT_IP_OK, NULL, 0);
  (void) IMENUCTL_AddItem(pISoftKeyMenu, gpAUXResourceFile, IDS_CANCEL, IDL_SK_EDIT_IP_CANCEL, NULL, 0);

  (void)ITEXTCTL_Redraw(pITextCtl);

  return TRUE;
}

/*===========================================================================
FUNCTION InitWLAN_SelectDlg

DESCRIPTION
  Initializes the Select WLAN Network dialog.  This dialog contains
  two controls, a static that says Searching... and a list.  Initially
  we only show the static dialog.  Once an event User comes in, we remove
  the static and show the list of available networks.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitWLAN_SelectDlg(AUXApp *pMe, uint16 wDlgID,
                                  IDialog* pIActiveDialog)
{
  IMenuCtl *pIMenuCtl = NULL;

  if(pMe == NULL || pIActiveDialog == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  /* Get the List control  */
  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog,
                          IDC_LIST_AVAIL_NETWORKS);
  if(pIMenuCtl == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }
  // These updates take a long time.
  // Show the searching dialog
  pMe->m_WLAN.m_pDetailedBSSList = (AEECMWLANBSSInfoList*)
                                        MALLOC(sizeof(AEECMWLANBSSInfoList));

  /* We are showing the Searching string.  Start a command to get
   * the WLAN Networks.  When complete, it will call AUXUtil_GetWLANCb,
   * this causes a EVT_USER to be sent to the dialog so that we will
   * update the list of available networks.
   */
  pMe->m_WLAN.m_nCallback.pfnCancel = NULL;
  pMe->m_WLAN.m_nCallback.pfnNotify = (PFNNOTIFY) AUXUtil_GetWLANCb;
  pMe->m_WLAN.m_nCallback.pNotifyData = (void*) pMe;
  pMe->m_WLAN.m_nNWSelectionMode  = APPCOMFUNC_NW_SEL_WLAN;
  
  if(AUXCheckAEEReturnStatus(ICM_GetNetworks(pMe->m_WLAN.m_pICM,
                  AEECM_NETWORK_LIST_BSS,
                  &pMe->m_WLAN.m_phError,
                  pMe->m_WLAN.m_pDetailedBSSList,
                  sizeof(AEECMWLANBSSInfoList),
                  &pMe->m_WLAN.m_nCallback)) == FALSE)
  {
    pMe->m_WLAN.m_nNWSelectionMode = APPCOMFUNC_NW_SEL_INACTIVE;
    FREEIF(pMe->m_WLAN.m_pDetailedBSSList);
    AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
    return TRUE;
  }
  pMe->m_WLAN.m_bWLANSearching = TRUE;

 /* Mark the list control not active. */
  IMENUCTL_SetActive(pIMenuCtl, FALSE);

  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pIMenuCtl);
  IDIALOG_Redraw (pIActiveDialog);
  return TRUE;
}

/*===========================================================================
FUNCTION InitConfigIpDlg

DESCRIPTION
  Initializes the menu listing IPs that can be edited

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitConfigIpDlg(AUXApp *pMe, uint16 wDlgID, IDialog* pIActiveDialog)
{
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR szBuf[MAX_STR_SIZE+1];
  AECHAR gap[MAX_STR_SIZE+1];

  if(pMe == NULL || pIActiveDialog == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if (SUCCESS == IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts))
  {
    WIFIOpt tmp_Opt;
    /* Get the IP Address value from the object. */
    if (SUCCESS == IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_STATIC_IP,
                                   &tmp_Opt ))
    {
      /* Copy the contents. */
      (void)WSTRNCOPYN(pMe->m_WLAN.m_IpAddr, IP_BUFFER_SIZE+1, tmp_Opt.pVal, -1);
    }
    /* Get the DNS Address value from the object. */
    if (SUCCESS == IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_DNS_ADDRESS,
                                   &tmp_Opt ))
    {
      /* Copy the contents. */
      (void)WSTRNCOPYN(pMe->m_WLAN.m_Dns, IP_BUFFER_SIZE+1, tmp_Opt.pVal, -1);
    }
    /* Get the subnet mask value from the object. */
    if (SUCCESS == IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_SUBNET_MASK,
                                   &tmp_Opt ))
    {
      /* Copy the contents. */
      (void)WSTRNCOPYN(pMe->m_WLAN.m_SubnetMask, IP_BUFFER_SIZE+1, tmp_Opt.pVal, -1);
    }
    /* Get the default gateway value from the object. */
    if (SUCCESS == IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_DEFAULT_GATEWAY,
                                   &tmp_Opt ))
    {
      /* Copy the contents. */
      (void)WSTRNCOPYN(pMe->m_WLAN.m_Gateway, IP_BUFFER_SIZE+1, tmp_Opt.pVal, -1);
    }
  }

  pIMenuCtl = (IMenuCtl*) IDIALOG_GetControl(pIActiveDialog, IDC_MENU_CONFIGIP);

  if(pIMenuCtl == NULL)
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_GAP,
                             gap, sizeof(AECHAR)* MAX_STR_SIZE);

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_IPADDR,
                             szBuf, sizeof(AECHAR)* MAX_STR_SIZE);

  (void) WSTRLCAT(szBuf, gap, ARR_SIZE(szBuf));
  (void) WSTRLCAT(szBuf, pMe->m_WLAN.m_IpAddr, ARR_SIZE(szBuf));

  IMENUCTL_SetItemText(pIMenuCtl, IDC_MENU_CONFIGIP_IP, gpAUXResourceFile, 0, szBuf);

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_MASK,
                             szBuf, sizeof(AECHAR)* MAX_STR_SIZE);

  (void) WSTRLCAT(szBuf, gap, ARR_SIZE(szBuf));
  (void) WSTRLCAT(szBuf, pMe->m_WLAN.m_SubnetMask, ARR_SIZE(szBuf));

  IMENUCTL_SetItemText(pIMenuCtl, IDC_MENU_CONFIGIP_SUBNET, gpAUXResourceFile, 0, szBuf);

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_GATEWAY,
                             szBuf, sizeof(AECHAR)* MAX_STR_SIZE);

  (void) WSTRLCAT(szBuf, gap, ARR_SIZE(szBuf));
  (void) WSTRLCAT(szBuf, pMe->m_WLAN.m_Gateway, ARR_SIZE(szBuf));

  IMENUCTL_SetItemText(pIMenuCtl, IDC_MENU_CONFIGIP_GATEWAY, gpAUXResourceFile, 0, szBuf);

  (void)ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile, IDS_DNS,
                             szBuf, sizeof(AECHAR)* MAX_STR_SIZE);

  (void) WSTRLCAT(szBuf, gap, ARR_SIZE(szBuf));
  (void) WSTRLCAT(szBuf, pMe->m_WLAN.m_Dns, ARR_SIZE(szBuf));

  IMENUCTL_SetItemText(pIMenuCtl, IDC_MENU_CONFIGIP_DNS, gpAUXResourceFile, 0, szBuf);

  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pIMenuCtl);
  IDIALOG_Redraw (pIActiveDialog);
  return TRUE;
}

/*===========================================================================
FUNCTION AUXRefreshWLANSettingsMenu

DESCRIPTION
  refresh WLAN Settings menu display after menu items change
  due to actions of other menus

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXRefreshWLANSettingsMenu(void *pUser, void *pICurrMenu)
{
  AUXApp * pMe = (AUXApp*)pUser;
  CtlAddItem pai;
  IMenuCtl * pICurrentMenu = (IMenuCtl*)pICurrMenu;
  boolean bReturnFlag;

  if ((NULL == pMe) || ( NULL == pICurrentMenu)) {
    AUX_ERR ("Null pointer %x %x", pMe, pICurrentMenu, 0);
    return FALSE;
  }

  /* update the data for the menu */
  (void) MEMSET(&pai, 0, sizeof(pai));
  pai.pszResText = gpAUXResourceFile;

  if ((uint64)AEECM_NETWORK_SEL_MODE_PREF_MANUAL == GetWLANNwSelMode(pMe)) {
    pai.dwData = AEECM_NETWORK_SEL_MODE_PREF_MANUAL;
    pai.wText = IDS_WLAN_MANUAL_SCAN;
  }
  else {
    pai.dwData = AEECM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    pai.wText = IDS_WLAN_SCAN_STORED_PROFILES;
  }

  bReturnFlag = IMENUCTL_SetItem(pICurrentMenu, IDL_WLAN_GENERIC_SCAN,
                                 (MSIF_DATA | MSIF_TEXT), &pai);
  if (bReturnFlag == FALSE) {
    AUX_ERR ("Set item failed", 0,0,0);
    return FALSE;
  }

  if (((uint64)AEECM_NETWORK_SEL_MODE_PREF_MANUAL == GetWLANNwSelMode(pMe)) &&
        ((uint64)AEECM_WLAN_BSS_TYPE_PREF_ADHOC == GetWLANMode(pMe) ||
         (uint64)AEECM_WLAN_BSS_TYPE_PREF_ANY == GetWLANMode(pMe)))
  {
    (void)IMENUCTL_AddItem(pICurrentMenu,
                           gpAUXResourceFile,
                           IDS_WLAN_START_NEW_ADHOC_NETWORK,
                           IDL_WLAN_START_ADHOC_NETWORK,
                           NULL,
                           AEECM_WLAN_BSS_TYPE_ADHOC );
  }
  else
  {
    (void)IMENUCTL_DeleteItem(pICurrentMenu, IDL_WLAN_START_ADHOC_NETWORK);
  }

  bReturnFlag = IMENUCTL_Redraw(pICurrentMenu);
  if (bReturnFlag == FALSE) {
    AUX_ERR ("Redraw failed", 0,0,0);
    return FALSE;
  }

  return TRUE;

} /* End AUXRefreshWLANSettingsMenu */


/*===========================================================================
FUNCTION AUXDynamicMenuDlgEventHandler

DESCRIPTION
  Handle generic dynamic menu dlg events where menu items can change
  due to actions of other menus

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXDynamicMenuDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*)pUser;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;
  int nReturnStatus;
  boolean bReturnValue;
  AEECMSSInfo ssInfo;

  if (pMe == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("DynamicMenuDlg: EVT_DIALOG_START", 0, 0, 0);
    wCtrlID = AUXGetDlgMenuCtlID(pMe, (IDialog*)dw);
    if(wCtrlID == 0) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }
    pICurrentMenu = (IMenuCtl*)IDIALOG_GetControl((IDialog*)dw, (int16) wCtrlID);
    if(pICurrentMenu) {
      /* remove certain items based on featurization */
      AUXStripFeatureMenuItem(pMe, pICurrentMenu, wCtrlID);
      AUXSetDefaultMenuLook(pMe->a.m_pIShell, pICurrentMenu);
      pMe->m_WLAN.m_pWLANSettingsDialog = (IDialog*)dw;
      (void)IDIALOG_AddRef(pMe->m_WLAN.m_pWLANSettingsDialog);
      bReturnValue = AUXRefreshWLANSettingsMenu(pMe, pICurrentMenu);
      return TRUE;
    }
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ---> error */

  case EVT_KEY:
    AUX_MSG ("MenuDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      AUXEndCurrentDlg(pMe);
      return TRUE;

    /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* Digit keys should be ignored */

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("DynamicMenuDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);

    if (w == IDL_WLAN_GENERIC_SCAN)
    {
      if ( AEECM_NETWORK_SEL_MODE_PREF_MANUAL == dw ) {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDD_WLAN_SELECT, NULL);
      }
      else {
                
        nReturnStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
        if (AEE_SUCCESS != nReturnStatus)
        {
          AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
          return TRUE;
        }
        (void)WSTRLCPY(pMe->m_WLAN.m_currentSSID,
                       ssInfo.wlan_bss_info.ssid,
                       AEECM_MAX_SSID_STR_SIZE); 
        
        nReturnStatus = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                                  AEECM_SYS_MODE_WLAN,
                                                  TRUE);
                                                 
        if (AEE_SUCCESS != nReturnStatus) {
          AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
          AUX_ERR("Failed to change WLAN subs, %d", nReturnStatus, 0, 0);
        }
        else {
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                              IDD_WLAN_AUTOSCAN_SEARCHING, NULL);
        }
      }
      return TRUE;
    }
    else if (w == IDL_WLAN_START_ADHOC_NETWORK)
    {
      pMe->m_WLAN.m_bWLANAdhocAssociationRequest = TRUE;
      nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDD_ENTER_SSID, NULL);
      if (nReturnStatus != SUCCESS) {
        AUX_ERR ("Could not create dialog", 0, 0, 0);
      }
    }
    else {
      bReturnValue = AUXCreateMenuSelectedItemDlg(pMe);
      if (bReturnValue == FALSE) {
        AUX_ERR ("Could not create dialog", 0, 0, 0);
      }
    }
    return TRUE;

  case EVT_USER:
    AUX_MSG ("MenuDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("MenuDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)IDIALOG_Release(pMe->m_WLAN.m_pWLANSettingsDialog);
    pMe->m_WLAN.m_pWLANSettingsDialog = NULL;
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    break;
  }

  return FALSE;
} /* End AUXDynamicMenuDlgEventHandler */

/*===========================================================================
FUNCTION AUXWLANNetSelModeDlgEventHandler

DESCRIPTION
  Handle events for the WLAN network select mode dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean AUXWLANNetSelModeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus = FALSE;
  AUXApp * pMe = (AUXApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("WLANNetSelModeDlg: EVT_DIALOG_START", 0, 0, 0);
    return AUXInitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("WLANNetSelModeDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("WLANNetSelModeDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    wCtrlID = AUXGetDlgMenuCtlID(pMe, pIDialog);
    if(wCtrlID == 0) {
      AUX_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }
    pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
    if(pICurrentMenu == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    if (( w == IDL_WLAN_NW_SEL_MANUAL) || ( w == IDL_WLAN_NW_SEL_AUTO )) {
      if (AUXUpdateSetting(pMe, w, pICurrentMenu)) {
         /* Bring up appropriate dialog for network scan */
        if (w == IDL_WLAN_NW_SEL_MANUAL ) {
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDD_WLAN_PERFORM_MANUAL_SCAN_NOW, NULL);
          if (nReturnStatus != SUCCESS)
          {
            AUX_ERR("Fail to create dialog %d: %d", IDD_WLAN_PERFORM_MANUAL_SCAN_NOW, nReturnStatus, 0);
          }
        }
        else {
          nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                              IDD_WLAN_AUTOSCAN_SEARCHING, NULL);
          if (nReturnStatus != SUCCESS)
          {
            AUX_ERR("Fail to create dialog %d: %d", IDD_WLAN_AUTOSCAN_SEARCHING, nReturnStatus, 0);
          }
        }
        pMe->m_WLAN.m_bNetSelModeUpdated = TRUE;
        (void)AUXCheckAEEReturnStatus(nReturnStatus);
      }
    }
    else {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = IDS_CHANGE_FAILED;
      AUXDisplayMessageDlg(pMe, wStringID, 3);
    }
    return TRUE;

  case EVT_USER:
    AUX_MSG ("WLANNetSelModeDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("WLANNetSelModeDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }

    if (pMe->m_bAppIsRunning == TRUE)
    {
      AUXPopDlg(pMe, IDD_WLAN_SETTINGS);

      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wCtrlID = AUXGetDlgMenuCtlID(pMe, pIDialog);
      if(wCtrlID == 0) {
        AUX_ERR ("Null pointer ", 0,0,0);
        return FALSE;
      }
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
      if(pICurrentMenu == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      (void)AUXRefreshWLANSettingsMenu(pMe, pICurrentMenu );
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End AUXWLANNetSelModeDlgEventHandler */

/*===========================================================================
FUNCTION AUXWLANModeDlgEventHandler

DESCRIPTION
  Handle events for the WLAN mode dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANModeDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  int nReturnStatus = FALSE;
  AUXApp * pMe = (AUXApp*) pUser;
  IDialog *pIDialog;
  IMenuCtl* pICurrentMenu;
  uint16 wCtrlID;

  if ( pMe == NULL )
  {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND)
  {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("WLANModeDlg: EVT_DIALOG_START", 0, 0, 0);
    return AUXInitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("WLANModeDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG("WLANModeDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

    pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
    if(pIDialog == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    wCtrlID = AUXGetDlgMenuCtlID(pMe, pIDialog);
    if(wCtrlID == 0) {
      AUX_ERR ("Null pointer ", 0,0,0);
      return FALSE;
    }
    pICurrentMenu= (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
    if(pICurrentMenu == NULL) {
      AUX_ERR ("Null pointer", 0,0,0);
      return FALSE;
    }

    if (AUXUpdateSetting(pMe, w, pICurrentMenu)) {
      /* Let the update image be shown for 2 seconds before going back to
      the previous screen */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                       AUX_SELECTIONVERIFICATIONTIME,
                       (PFNNOTIFY) AUXEndCurrentDlg,
                       (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUX_SELECTIONVERIFICATIONTIME: %d", nReturnStatus, 0, 0);
      }
    }
    else {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = IDS_CHANGE_FAILED;
      AUXDisplayMessageDlg(pMe, wStringID, 3);
    }
    return TRUE;

  case EVT_USER:
    AUX_MSG ("WLANModeDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("WLANModeDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    
    if (pMe->m_bAppIsRunning == TRUE)
    {
      AUXPopDlg(pMe, IDD_WLAN_SETTINGS);

      pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIDialog == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wCtrlID = AUXGetDlgMenuCtlID(pMe, pIDialog);
      if(wCtrlID == 0) {
        AUX_ERR ("Null pointer ", 0,0,0);
        return FALSE;
      }
      pICurrentMenu = (IMenuCtl*) IDIALOG_GetControl(pIDialog, (int16)wCtrlID);
      if(pICurrentMenu == NULL) {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      (void)AUXRefreshWLANSettingsMenu(pMe, pICurrentMenu );
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End WLANModeDlgEventHandler */

/*===========================================================================
FUNCTION AUXWLANOptEditDlgHandler

DESCRIPTION
  Handle WLAN option editing dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANOptEditDlgHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp   *pMe = (AUXApp *)pUser;
  IDialog    *pIActiveDialog;
  ITextCtl   *pOptEdit;
  AECHAR     *pszwBuf = NULL;
  int         nReturnStatus;
  const WLANEditDlgDataType *pDlgData = NULL;  /* Data for this dialog */
  uint16      nDlgId;
  int         nErrCode;
  AEECMWLANPref  sICMWLANData;
  AEECMPhError   error;
  AEECMPhInfo    *pPhInfo = NULL;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if (evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_KEY_PRESS ||
      evt == EVT_COMMAND) {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  /* Get a pointer to the active dialog. */
  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  /* Get the WLAN Edit Dialog table entry */
  nDlgId = IDIALOG_GetID(pIActiveDialog);
  pDlgData = AUXGetWLANEditDlgTbl(nDlgId);

  if (pDlgData == NULL)
  {
    /* If we couldn't find an entry for this dialog, we can't continue. */
    AUX_ERR("No Dialog entry in table for dialog %d.", nDlgId, 0, 0);
    return FALSE;
  }

  switch (evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("WLANOptEditDlg %d: EVT_DIALOG_START", pDlgData->nDlgId, 0, 0);
    return InitWLANOptEditDlg(pMe, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("WLANOptEditDlg %d: EVT_KEY", pDlgData->nDlgId, 0, 0);
    if (w != AVK_SELECT) {
      switch(w) {
      case AVK_CLR:
        if (pDlgData->bIsWLANSec == TRUE)
        {
          nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Failed to close dialog, %d", nErrCode, 0, 0);
          }
        }
        else
        {
          AUXPopDlg (pMe, pDlgData->nReturnDlgId);
        }
        return TRUE;

      case AVK_DOWN:
      case AVK_UP:
        if (w == AVK_DOWN) {
          (void)IDIALOG_SetFocus(pIActiveDialog, (int16)pDlgData->nSKId);
        }
        else {
          (void)IDIALOG_SetFocus(pIActiveDialog, (int16)pDlgData->nTextId);
        }
        return TRUE;

      /* digit keys */
      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
      case AVK_STAR:
      case AVK_POUND:
      /* navigation keys */
      case AVK_LEFT:
      case AVK_RIGHT:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* event handled */


      default:
        return FALSE;
      }

    }

  /* fall through for AVK_SELECT */

  case EVT_COMMAND:
  {

    WIFIOpt tmp_Opt;
    boolean bErrDlg = FALSE;      /* Whether an error dialog was created */

    AUX_MSG ("WLANOptEditDlg %d: EVT_COMMAND, w=%d, dw=%d", pDlgData->nDlgId, w, dw);
    if (((evt == EVT_KEY) && (w == AVK_SELECT)) ||
         ((evt == EVT_COMMAND) && (w == pDlgData->nSKOKId)))
    {

      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if (pIActiveDialog == NULL) {
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      pOptEdit = (ITextCtl*)IDIALOG_GetControl(pIActiveDialog,
                                               (int16)pDlgData->nTextId);
      if (pOptEdit == NULL) {
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      /* Allocate a buffer and get the text from the text input box. */
      pszwBuf = (AECHAR *)MALLOC((pDlgData->nTextFieldSz + 1) * sizeof(AECHAR));
      if (! pszwBuf)
      {
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        AUX_ERR("No mem for text buffer.", 0, 0, 0);
        return FALSE;
      }

      (void)ITEXTCTL_GetText(pOptEdit, pszwBuf, pDlgData->nTextFieldSz + 1);
      
      if ((pDlgData->nDlgId == IDD_ENTER_SSID) &&
          (pMe->m_WLAN.m_bWLANAdhocAssociationRequest == TRUE))
      {
        do {

          pPhInfo = MALLOC(sizeof(AEECMPhInfo));
          if (pPhInfo == NULL)
          {
            AUX_ERR("Malloc PhInfo failed", 0, 0, 0);
            break;
          }  

          nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, pPhInfo, sizeof(AEECMPhInfo)); 
          if (nReturnStatus != AEE_SUCCESS)
          {
            AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
            AUX_ERR("Failed getting phone info from ICM because return status is %d", nReturnStatus, 0, 0);
            break;
          }  
          (void)MEMCPY(&sICMWLANData, &(pPhInfo->m_wlan_pref), sizeof(sICMWLANData));

          (void)WSTRLCPY(sICMWLANData.ssid, pszwBuf, AEECM_MAX_SSID_STR_SIZE);
                   
          (void)ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                             AEECM_MODE_PREF_CURRENT_PLUS_WLAN,
                                             AEECM_PREF_TERM_PERMANENT, 0,
                                             AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                             AEECM_BAND_PREF_NO_CHANGE,
                                             AEECM_ROAM_PREF_NO_CHANGE,
                                             AEECM_HYBR_PREF_NO_CHANGE,
                                             AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                             AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                             NULL, &sICMWLANData, &error, NULL);
        }while(0);
        FREEIF(pPhInfo);

        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDD_WLAN_START_NEW_ADHOC_NETWORK, NULL);        
      }
      else
      {
        /* Set the option value */
        tmp_Opt.nId = pDlgData->nWIFIOptId;

        /* Assumption: All "number input" options are 32-bit quantities.
         * This implies that the "ptr" in the option should actually be
         * the 32-bit value.
         *
         * If this assumption changes, assignment of the pVal in the WIFIOpt
         * will need to be based on some other criterion.
         */
        do
        {
          if (pDlgData->TextInputMode == AEE_TM_NUMBERS)
          {
            uint32 val;
            val = (uint32)AUXWATOI(pszwBuf);

            if ((nDlgId == IDD_WLAN_SEC_WEP64_KEY_ID) ||
                (nDlgId == IDD_WLAN_SEC_WEP128_KEY_ID))
            {
              /* WEP Key IDs are presented to the user as 1-based values
               * (i.e. keys 1-4), but are stored internally as zero-based
               * values (i.e. keys 0-3).  So, when the user inputs a key id,
               * we need to subtract one to convert.
               */
              if ((val >= 1) && (val <= 4))
              {
                val--;
              }
              else
              {
                AUX_ERR("Invalid range for WEP key id, %d", val, 0, 0);
                AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                bErrDlg = TRUE;
                break;
              }
            }
            else if (nDlgId == IDD_ENTER_CHANNEL)
            {
              /* Convert to the IWIFI value, if necessary */
              if (val == AUX_WLAN_CHANNEL_ANY)
              {
                val = AEEWIFI_CHANNEL_ANY;
              }
            }

            tmp_Opt.pVal = (void *)val;
          } //AEE_TM_NUMBERS
          else if (pDlgData->TextInputMode == AEE_TM_LETTERS)
          {
            /* The following inputs must be hexadecimal */
            if ((nDlgId == IDD_WLAN_SEC_WEP64_KEY1) ||
                (nDlgId == IDD_WLAN_SEC_WEP64_KEY2) ||
                (nDlgId == IDD_WLAN_SEC_WEP64_KEY3) ||
                (nDlgId == IDD_WLAN_SEC_WEP64_KEY4) ||
                (nDlgId == IDD_WLAN_SEC_WEP128_KEY1) ||
                (nDlgId == IDD_WLAN_SEC_WEP128_KEY2) ||
                (nDlgId == IDD_WLAN_SEC_WEP128_KEY3) ||
                (nDlgId == IDD_WLAN_SEC_WEP128_KEY4))
            {
              if (AUXIsHexWString(pszwBuf) == FALSE)
              {
                AUX_ERR("Invalid WEP key string", 0, 0, 0);
                AUXDisplayMessageDlg(pMe, IDS_ERROR_HEX_STRING, 3);
                bErrDlg = TRUE;
                break;
              }
              else if ((uint16)WSTRLEN(pszwBuf) != pDlgData->nTextFieldSz)
              {
                AUX_ERR("WEP key string size is wrong %d", WSTRLEN(pszwBuf), 0, 0);
                AUXDisplayMessageDlg(pMe, IDS_WEP_KEY_TOO_SHORT, 3);
                bErrDlg = TRUE;
                break;
              }
            }
            tmp_Opt.pVal = pszwBuf;
          } //AEE_TM_LETTERS
          else
          {
            AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
            bErrDlg = TRUE;
            AUX_ERR("Unsupported text input mode, %d.", pDlgData->TextInputMode,
                     0, 0);
            break;
          }

          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt);
          if (SUCCESS != nErrCode)
          {
            AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
            bErrDlg = TRUE;
            AUX_ERR("Could not set WLAN Opt %d, error code %d",
                     pDlgData->nWIFIOptId, nErrCode, 0);
            break;
          }
          else
          {
            /* If we updated one of the authentication key-related values,
             * set the flag so that we know to update the authentication
             * screen.
             */
            if (pDlgData->bIsWLANSec == TRUE)
            {
                pMe->m_WLAN.m_bAuthKeysUpdated = TRUE;
            }
            if (pDlgData->bWIFICommit)
            {
              nErrCode = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
              if (SUCCESS != nErrCode)
              {
                AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                bErrDlg = TRUE;
                AUX_ERR("Could not commit WLAN Opt %d, error code %d. ",
                         pDlgData->nWIFIOptId, nErrCode, 0);
                break;
              }
              else
              {
                nErrCode = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                                     AEECM_SYS_MODE_WLAN,
                                                     TRUE);
                if (AEE_SUCCESS != nErrCode)
                {
                  AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                  bErrDlg = TRUE;
                  AUX_ERR("Failed to change WLAN subs, %d", nErrCode, 0, 0);
                  break;
                }
                
              }
            }
          }
        } while (0); /*lint -save -e506 "constant value boolean" */
        
        /* If there's no error dialog and we're editing one of the WEP
         * params, end the edit dialog.
         */
        if (bErrDlg == FALSE)
        {
          if (pDlgData->bIsWLANSec == TRUE)
          {
            nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
            if (nErrCode != SUCCESS)
            {
              AUX_ERR("Failed to close dialog, %d", nErrCode, 0, 0);
            }
          }
          else
          {
            AUXPopDlg (pMe, pDlgData->nReturnDlgId);
          }
        }
      }
    }
    FREEIF(pszwBuf);

    return TRUE;
  }

  case EVT_DIALOG_END:
    AUX_MSG ("WLAN Dlg %d: EVT_DIALOG_END", pDlgData->nDlgId, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell, AUXAPPTIMEOUT,
                                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                                    (uint32 *)pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End AUXWLANOptEditDlgHandler */

/*===========================================================================
FUNCTION InitWLANSecurityDlg

DESCRIPTION
  Initializes the WLAN Security dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean InitWLANSecurityDlg(AUXApp *pMe, IDialog *pDialog)
{
  IMenuCtl   *pSecMenu;
  uint16      nDlgId;
  int        nErrCode;
  WIFIOpt    TmpOpt;

  if (pMe == NULL || pDialog == NULL)
  {
    AUX_ERR("NULL pointer, %d, %d", pMe, pDialog, 0);
    return FALSE;
  }

  nErrCode = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
  if (nErrCode != SUCCESS)
  {
    AUXDisplayMessageDlg(pMe, IDS_READ_FAILED, 3);
    AUX_ERR("Failed loading WIFI options, %d", nErrCode, 0, 0);
    return FALSE;
  }
  nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                              AEEWIFI_OPT_AUTHENTICATION_TYPE, &TmpOpt);
  if (nErrCode != SUCCESS) {
    AUX_ERR("Unable to load WIFI auth type: %d", nErrCode, 0, 0);
    return FALSE;
  }
  pMe->m_WLAN.m_nAuthType = (AEEWIFIAuthenticationOption)(uint32)TmpOpt.pVal;
  pMe->m_WLAN.m_nprevAuthType = pMe->m_WLAN.m_nAuthType;

  nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                              AEEWIFI_OPT_ENCRYPTION_TYPE, &TmpOpt);
  if (nErrCode != SUCCESS) {
    AUX_ERR("Unable to load WIFI encrypt type: %d", nErrCode, 0, 0);
    return FALSE;
  }
  pMe->m_WLAN.m_nEncrypType = (AEEWIFIEncryptionOption)(uint32)TmpOpt.pVal;
  pMe->m_WLAN.m_nprevEncrypType = pMe->m_WLAN.m_nEncrypType;

  FREEIF(pMe->m_WLAN.m_pWLANAuthInfo);
  pMe->m_WLAN.m_pWLANAuthInfo = MALLOC(sizeof(AUXAppWLANAuthInfo));
  if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_WEP) && 
      (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_WEP_64bit))
  {
    /* Get the WEP 64 Key ID and save in pMe
       GEt the WEP64 Keys 1-4 and save in pMe.
    */
    
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_64BIT_KEY_ID, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 64BIT_KEY_ID: %d", nErrCode, 0, 0);
      return FALSE;
    }
    pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.default_key = (uint32)TmpOpt.pVal;
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_64BIT_KEY_0, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 64BIT_KEY_0: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key1, TmpOpt.pVal, AUXAPP_64BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_64BIT_KEY_1, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 64BIT_KEY_1: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key2, TmpOpt.pVal, AUXAPP_64BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_64BIT_KEY_2, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 64BIT_KEY_2: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key3, TmpOpt.pVal, AUXAPP_64BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_64BIT_KEY_3, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 64BIT_KEY_3: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key4, TmpOpt.pVal, AUXAPP_64BIT_WEP_KEY_STR_SIZE);
  } 
  else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_WEP) && 
           (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_WEP_128bit))
  {
    /* Get the WEP 128 Key ID and save in pMe
       GEt the WEP128 Keys 1-4 and save in pMe.
    */

    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_128BIT_KEY_ID, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 128BIT_KEY_ID: %d", nErrCode, 0, 0);
      return FALSE;
    }
    pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.default_key = (uint32)TmpOpt.pVal;
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_128BIT_KEY_0, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 128BIT_KEY_0: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key1, TmpOpt.pVal, AUXAPP_128BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_128BIT_KEY_1, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 128BIT_KEY_1: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key2, TmpOpt.pVal, AUXAPP_128BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_128BIT_KEY_2, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 128BIT_KEY_2: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key3, TmpOpt.pVal, AUXAPP_128BIT_WEP_KEY_STR_SIZE);
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_WEP_128BIT_KEY_3, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 128BIT_KEY_3: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key4, TmpOpt.pVal, AUXAPP_128BIT_WEP_KEY_STR_SIZE);
  } 
  else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_PASSWORD) && 
           (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
  { 
    /* Get the AEEWIFI_OPT_8021X_PASSWORD and save in pMe
    */
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_PASSWORD, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 8021X_PASSWORD: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->passwd_info.pass, TmpOpt.pVal, AEECM_8021X_PASSWD_STR_SIZE);
  } 
  else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_MD5) && 
           (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
  {
    /* Get the AEEWIFI_OPT_8021X_PASSWORD and  AEEWIFI_OPT_8021X_ID
       and save in pMe
    */
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_ID, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 8021X_ID: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->md5_info.id, TmpOpt.pVal, AEECM_8021X_USER_ID_STR_SIZE);   
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_PASSWORD, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 8021X_PASSWORD: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->md5_info.pass, TmpOpt.pVal, AEECM_8021X_PASSWD_STR_SIZE);   
  } 
  else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_TLS) && 
           (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
  {
    /* Get the AEEWIFI_OPT_8021X_ID and save in pMe
    */
    nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts, AEEWIFI_OPT_8021X_ID, &TmpOpt);
    if (nErrCode != SUCCESS) {
      AUX_ERR("Unable to load WIFI 8021X_ID: %d", nErrCode, 0, 0);
      return FALSE;
    }
    (void)WSTRLCPY(pMe->m_WLAN.m_pWLANAuthInfo->tls_cert_info.id, TmpOpt.pVal, AEECM_8021X_USER_ID_STR_SIZE);   
  } 

  pSecMenu = (IMenuCtl*)IDIALOG_GetControl(pDialog, IDC_MENU_WLAN_SECURITY);
  if(! pSecMenu) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE; /* no menu control ---> error */
  }

  nDlgId = IDIALOG_GetID(pDialog);
  if (AUXInitRadioButtonDlg(pMe, nDlgId, pDialog) != TRUE)
  {
    AUX_ERR("Failed initializing radio buttons, dlg %d.", nDlgId, 0, 0);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION _RemoveAllSecOpts()

DESCRIPTION
  Remove all security options before commiting new security options.  If the
  option is not found, then it will ignore.  If the option is found but not able
  to be removed, it will return the error code.

DEPENDENCIES

SIDE EFFECTS
  None

RETURN VALUE

  SUCCESS      if remove was successful
  EBADPARM     if any of the parameters is NULL
  EFAILED      if remove was not successful
 
===========================================================================*/
static int AUXWLANMenu_RemoveAllSecOpts(IWIFIOpts *pIWIFIOpts)
{

  int nErrCode;
  int i;
  WIFIOpt    TmpOpt;
 
  if (pIWIFIOpts == NULL)
  {
     AUX_ERR( "AUXWLANMenu_RemoveAllSecOpts: NULL parameters - pIWIFIOpts 0x%x", 
       (uint32)(pIWIFIOpts), 0, 0 );
     return EBADPARM;
  }

  /*remove all stored security options*/
  
  for (i=0; i < ARR_SIZE(WLANSecOptTbl); i++)
  {
    nErrCode = IWIFIOpts_GetOpt(pIWIFIOpts, WLANSecOptTbl[i], &TmpOpt);
    if (nErrCode == SUCCESS)
    {
      nErrCode = IWIFIOpts_RemoveOpt(pIWIFIOpts, WLANSecOptTbl[i]);
      if (nErrCode != SUCCESS)
        return nErrCode;
    }
  }
  
  return SUCCESS;
}

/*===========================================================================
FUNCTION AUXWLANSecDlgEventHandler

DESCRIPTION
  Handles events for the WLAN Security dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANSecDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp  *pMe = (AUXApp*) pUser;
  int        nReturnStatus;
  AEECMPhError error;
  int        nErrCode;
  AEEWIFIAuthenticationOption nAuthType;
  AEEWIFIEncryptionOption     nEncrypType;
  WIFIOpt    TmpOpt;
  IMenuCtl   *pSecMenu;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {

  case EVT_DIALOG_START:
    AUX_MSG ("WLANSecDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitWLANSecurityDlg(pMe, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("WLANSecDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* Digit keys should be ignored */

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    {
      uint16    wDlgToLaunch = 0;
      IDialog   *pDialog;
      boolean   bErrDlg = FALSE;      /* Whether an error dialog was created */
      AEECMWLANAuthType nAEECMWLANAuthType;
      AEECMWLANEncType nAEECMWLANEncType;

      AUX_MSG ("WLANSecDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* if SoftKey == DONE do association and end dialog,
       * if SoftKey == CANCEL, no association, just end dialog */
      if ( w == IDL_SK_WLAN_SEC_EDIT_DONE )
      {
        nErrCode = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
        if (SUCCESS != nErrCode)
        {
          AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
          AUX_ERR("Could not commit WIFI Options, %d", nErrCode, 0, 0);
          bErrDlg = TRUE;
          break;
        }
        else
        {
          if (pMe->m_WLAN.m_bWLANManualScanAssociationRequest == TRUE)
          {
            pMe->m_WLAN.m_bWLANManualScanAssociationRequest = FALSE;
            if (NULL != pMe->m_WLAN.m_psICMWLANData)
            {
              /* Get the authentication type from IWIFI */
              nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                          AEEWIFI_OPT_AUTHENTICATION_TYPE, &TmpOpt);
              if (SUCCESS != nErrCode)
              {
                AUX_ERR("Failed to get WIFI auth type, %d", nErrCode, 0, 0);
                AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                return TRUE;
              }
              else
              {
                nErrCode = Convert_IWIFIAuth_to_ICMAuth((AEEWIFIAuthenticationOption)(uint32)TmpOpt.pVal, 
                                                     &nAEECMWLANAuthType);
                if (SUCCESS != nErrCode)
                {
                  AUX_ERR("Failed to convert WIFI auth type", 0, 0, 0);
                  AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                  return TRUE;
                }
                else
                {
                  pMe->m_WLAN.m_psICMWLANData->auth = nAEECMWLANAuthType;
                }
              }

              /* Get the encryption type from IWIFI */
              nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                          AEEWIFI_OPT_ENCRYPTION_TYPE, &TmpOpt);
              if (SUCCESS != nErrCode)
              {
                AUX_ERR("Failed to get WIFI encryption type, %d", nErrCode, 0, 0);
                AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                return TRUE;
              }
              else
              {
                nErrCode = Convert_IWIFIEncry_to_ICMEncry((AEEWIFIEncryptionOption)(uint32)TmpOpt.pVal, 
                                                       &nAEECMWLANEncType);
                if (SUCCESS != nErrCode)
                {
                  AUX_ERR("Failed to convert WIFI encryption type", 0, 0, 0);
                  AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                  return TRUE;
                }
                else
                {
                  pMe->m_WLAN.m_psICMWLANData->encrypt = nAEECMWLANEncType;
                }
              }

              /* Now set up the authentication info */
              nErrCode = AUXApp_FillWLANAuthInfo(pMe, pMe->m_WLAN.m_psICMWLANData);
              if (SUCCESS != nErrCode)
              {
                return FALSE;
              }
              (void)ICM_SetSystemPreference_WLAN(pMe->m_WLAN.m_pICM,
                                                 AEECM_MODE_PREF_CURRENT_PLUS_WLAN,
                                                 AEECM_PREF_TERM_PERMANENT, 0,
                                                 AEECM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                                 pMe->m_WLAN.m_WLANBandPref,
                                                 AEECM_ROAM_PREF_NO_CHANGE,
                                                 AEECM_HYBR_PREF_NO_CHANGE,
                                                 AEECM_SRV_DOMAIN_PREF_NO_CHANGE,
                                                 AEECM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                                 NULL, pMe->m_WLAN.m_psICMWLANData, &error, NULL);
            }   /*pMe->m_WLAN.m_psICMWLANData */
            else
            {
              AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
              AUX_ERR("NULL ICM Data pointer", 0, 0, 0);
              bErrDlg = TRUE;
              break;
            }
          } /* pMe->m_bWLANManualScanAssociationRequest */
          else
          {
            nErrCode = ICM_SetSubscriptionStatus(pMe->m_WLAN.m_pICM,
                                                 AEECM_SYS_MODE_WLAN,
                                                 TRUE);
                                               
            if (AEE_SUCCESS != nErrCode)
            {
              AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
              AUX_ERR("Failed to change WLAN subs, %d", nErrCode, 0, 0);
              bErrDlg = TRUE;
              break;
            }  
          }
        }
        if (bErrDlg == FALSE)
        {
          nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not end dialog, %d", nErrCode, 0, 0);
          }
        }
        FREEIF(pMe->m_WLAN.m_psICMWLANData);
        FREEIF(pMe->m_WLAN.m_pWLANAuthInfo);
        return TRUE;
      }
      else if ( w == IDL_SK_WLAN_SEC_EDIT_CANCEL )
      {
        nErrCode = AUXWLANMenu_RemoveAllSecOpts(pMe->m_WLAN.m_pIWIFIOpts);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Could not remove all security opts: %d", nErrCode, 0, 0);
          return TRUE;
        }
        /* Set the authentication value */
        TmpOpt.nId = AEEWIFI_OPT_AUTHENTICATION_TYPE;
        TmpOpt.pVal = (void *)pMe->m_WLAN.m_nAuthType;
        nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Could not reset WLAN auth type %d: %d", TmpOpt.nId,
                   nErrCode, 0);
          return TRUE;
        }

        /* Set the encryption value */
        TmpOpt.nId = AEEWIFI_OPT_ENCRYPTION_TYPE;
        TmpOpt.pVal = (void *)pMe->m_WLAN.m_nEncrypType;
        nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Could not reset WLAN encrypt type %d: %d", TmpOpt.nId,
                   nErrCode, 0);
          return TRUE;
        }
       
        nErrCode = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
        if (SUCCESS != nErrCode)
        {
          AUX_ERR("Could not commit WIFI Options, %d", nErrCode, 0, 0);
        }

        /*restore back up options from pMe*/
        if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_WEP) && 
            (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_WEP_64bit))
        {  
          TmpOpt.nId = AEEWIFI_OPT_WEP_64BIT_KEY_ID;
          TmpOpt.pVal = (void*)pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.default_key;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 64BIT_KEY_ID %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
          
          TmpOpt.nId = AEEWIFI_OPT_WEP_64BIT_KEY_0;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key1;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 64BIT_KEY_0 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
          
          TmpOpt.nId = AEEWIFI_OPT_WEP_64BIT_KEY_1;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key2;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 64BIT_KEY_1 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
          
          TmpOpt.nId = AEEWIFI_OPT_WEP_64BIT_KEY_2;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key3;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 64BIT_KEY_2 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
          
          TmpOpt.nId = AEEWIFI_OPT_WEP_64BIT_KEY_3;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep64_info.key4;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 64BIT_KEY_3 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
        }
        else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_WEP) &&
                 (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_WEP_128bit))
        {
          TmpOpt.nId = AEEWIFI_OPT_WEP_128BIT_KEY_ID;
          TmpOpt.pVal = (void*)pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.default_key;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 128BIT_KEY_ID %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }

          TmpOpt.nId = AEEWIFI_OPT_WEP_128BIT_KEY_0;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key1;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 128BIT_KEY_0 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }

          TmpOpt.nId = AEEWIFI_OPT_WEP_128BIT_KEY_1;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key2;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 128BIT_KEY_1 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }

          TmpOpt.nId = AEEWIFI_OPT_WEP_128BIT_KEY_2;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key3;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 128BIT_KEY_2 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }

          TmpOpt.nId = AEEWIFI_OPT_WEP_128BIT_KEY_3;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->wep128_info.key4;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 128BIT_KEY_3 %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
        }
        else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_PASSWORD) && 
                 (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
        {
          TmpOpt.nId = AEEWIFI_OPT_8021X_PASSWORD;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->passwd_info.pass;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 8021X_PASSWORD %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
        }
        else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_MD5) && 
                 (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
        {
          TmpOpt.nId = AEEWIFI_OPT_8021X_ID;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->md5_info.id;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 8021X_ID %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }

          TmpOpt.nId = AEEWIFI_OPT_8021X_PASSWORD;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->md5_info.pass;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 8021X_PASSWORD %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
        }
        else if ((pMe->m_WLAN.m_nAuthType == AEEWIFI_AUTHENTICATION_8021X_TLS) && 
                 (pMe->m_WLAN.m_nEncrypType == AEEWIFI_ENCRYPTION_TKIP))
        {
          TmpOpt.nId = AEEWIFI_OPT_8021X_ID;
          TmpOpt.pVal = pMe->m_WLAN.m_pWLANAuthInfo->tls_cert_info.id;
          nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
          if (nErrCode != SUCCESS)
          {
            AUX_ERR("Could not reset WLAN 8021X_ID %d: %d", TmpOpt.nId,
                     nErrCode, 0);
            return TRUE;
          }
        }

        nErrCode = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
        if (SUCCESS != nErrCode)
        {
          AUX_ERR("Could not commit WIFI Options, %d", nErrCode, 0, 0);
        }

        /* Even if we couldn't revert the WIFI opt changes, end the dialog */
        nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Failed to end dialog, %d", nErrCode, 0, 0);
        }
        FREEIF(pMe->m_WLAN.m_psICMWLANData);
        FREEIF(pMe->m_WLAN.m_pWLANAuthInfo);
        return TRUE;
      }

      /* if we reach this point, we are dealing with radio button events, not softkey events.  */
      switch (w)
      {
        case IDL_SECURITY_NONE:
          nAuthType = AEEWIFI_AUTHENTICATION_NONE;
          nEncrypType = AEEWIFI_ENCRYPTION_NONE;
          wDlgToLaunch = 0;     /* No security params to edit */
          break;

        case IDL_SECURITY_WEP64:
          nAuthType = AEEWIFI_AUTHENTICATION_WEP;
          nEncrypType = AEEWIFI_ENCRYPTION_WEP_64bit;
          wDlgToLaunch = IDD_WLAN_SEC_WEP64;
          break;

        case IDL_SECURITY_WEP128:
          nAuthType = AEEWIFI_AUTHENTICATION_WEP;
          nEncrypType = AEEWIFI_ENCRYPTION_WEP_128bit;
          wDlgToLaunch = IDD_WLAN_SEC_WEP128;
          break;

        case IDL_SECURITY_WPA_PASSWORD:
          nAuthType = AEEWIFI_AUTHENTICATION_8021X_PASSWORD;
          nEncrypType = AEEWIFI_ENCRYPTION_TKIP;
          wDlgToLaunch = 0;
          break;

        case IDL_SECURITY_WPA_MD5:
          nAuthType = AEEWIFI_AUTHENTICATION_8021X_MD5;
          nEncrypType = AEEWIFI_ENCRYPTION_TKIP;
          wDlgToLaunch = 0;
          break;

        case IDL_SECURITY_WPA_TLS_CERT:
          nAuthType = AEEWIFI_AUTHENTICATION_8021X_TLS;
          nEncrypType = AEEWIFI_ENCRYPTION_TKIP;
          wDlgToLaunch = 0;
          break;

        case IDL_SECURITY_WPA_EAP:
          //Just set to any EAP auth types
          //The cast is needed because AEEWIFI_AUTHENTICATION_PAP is the new auth schemes 
          //defined in AEEWIFIExt.h.  This will avoid the rvct compiler warning.
          nAuthType = (AEEWIFIAuthenticationOption)AEEWIFI_AUTHENTICATION_PAP;
          nEncrypType = AEEWIFI_ENCRYPTION_TKIP;
          wDlgToLaunch = 0;
          break;

        default:
          AUX_ERR("Unsupported WLAN security type: %d", w, 0, 0);
          AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
          return TRUE;
      }
      
      /* Remove all previous opt values.  If the current option is selected again, 
         then don't remove
      */
      if ((pMe->m_WLAN.m_nprevAuthType != nAuthType) || (pMe->m_WLAN.m_nprevEncrypType != nEncrypType))
      {
        nErrCode = AUXWLANMenu_RemoveAllSecOpts(pMe->m_WLAN.m_pIWIFIOpts);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Could not remove all security opts: %d", nErrCode, 0, 0);
          return TRUE;
        }
        pMe->m_WLAN.m_nprevAuthType = nAuthType;
        pMe->m_WLAN.m_nprevEncrypType = nEncrypType;
      }

      /* Set the authentication value */
      TmpOpt.nId = AEEWIFI_OPT_AUTHENTICATION_TYPE;
      TmpOpt.pVal = (void *)nAuthType;
      nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Could not set WLAN auth type %d: %d", TmpOpt.nId,
                 nErrCode, 0);
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        return TRUE;
      }

      /* Set the encryption value */
      TmpOpt.nId = AEEWIFI_OPT_ENCRYPTION_TYPE;
      TmpOpt.pVal = (void *)nEncrypType;
      nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Could not set WLAN encrypt type %d: %d", TmpOpt.nId,
                 nErrCode, 0);
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        return TRUE;
      }

      nErrCode = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (SUCCESS != nErrCode)
      {
        AUX_ERR("Could not commit WIFI Options, %d", nErrCode, 0, 0);
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        return TRUE;
      }

      pDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      
      if (! pDialog)
      {
        AUX_MSG("Failed to get active dialog", 0, 0, 0);
      }
      else
      {
        /* Re-initialize the dialog to update the radio button selection */
        pSecMenu = (IMenuCtl*)IDIALOG_GetControl(pDialog, IDC_MENU_WLAN_SECURITY);
        if(! pSecMenu) {
          AUX_ERR ("Null pointer", 0,0,0);
          return FALSE; /* no menu control ---> error */
        }
        if (AUXInitRadioButtonDlg(pMe, IDIALOG_GetID(pDialog), pDialog) != TRUE)
        {
           AUX_ERR("Failed initializing radio buttons, dlg %d.", IDIALOG_GetID(pDialog), 0, 0);
           return FALSE;
        }
      }

      if (wDlgToLaunch != 0)
      {
        /* If there's another dialog for setting parameters for this
         * particular security type, launch that dialog.
         */
        nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                       wDlgToLaunch, NULL);
        if (nErrCode != SUCCESS) {
          AUX_ERR ("Could not create dialog %d: %d", wDlgToLaunch, nErrCode, 0);
          return TRUE;
        }
      }
    }
    return TRUE;

  case EVT_USER:
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("WLANSecDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    FREEIF(pMe->m_WLAN.m_pWLANAuthInfo);
    if (pMe->m_bAppIsRunning)
    {
      AUXPopDlg(pMe, IDD_WLAN_SETTINGS);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
      return FALSE;
  }
  return FALSE; 
} /* End WLANSecDlgEventHandler */

/*===========================================================================
FUNCTION InitWLANSecEditDlg

DESCRIPTION
  Creates the WLAN security settings dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean InitWLANSecEditDlg(AUXApp *pMe, IDialog *pDialog)
{

  int16     nSecEditMenu;
  int16     nSKMenu;
  IMenuCtl *pSecEditMenu;
  IMenuCtl *pSecSKMenu;
  uint16    nDlgId;
  boolean   bRetVal = FALSE;
  AECHAR   *pszwResStr;
  char     *pszResStr;
  int       nErrCode;
  WIFIOpt   TmpOpt;
  const AUXWLANMenu_SecFieldEditStruct *pSecResData = NULL;
  boolean   bIsTextCorrect = FALSE;
  int       i;                 /* for loop control */
  uint32    nEncryptOpt;
  uint32    nAuthOpt;
  uint16    nTitleResID;
  const AUXWLANMenu_SecEditStruct *pDlgData = NULL;

  if (pMe == NULL || pDialog == NULL)
  {
    AUX_ERR("NULL object pointer, 0x%x 0x%x", pMe, pDialog, 0);
    return bRetVal;
  }

  nDlgId = IDIALOG_GetID(pDialog);

  for (i = 0; i < (int)ARR_SIZE(WLANSecEditData); i++)
  {
    if (nDlgId == WLANSecEditData[i].nDlgId)
    {
      pDlgData = &WLANSecEditData[i];
    }
  }

  if (! pDlgData)
  {
    AUX_ERR("Dlg data not found for dialog %d", nDlgId, 0, 0);
    return bRetVal;
  }
  else if (! pDlgData->pSecFieldData)
  {
    AUX_ERR("NULL field edit data for dialog %d", nDlgId, 0, 0);
    return bRetVal;
  }

  nSKMenu = (int16)pDlgData->nSKId;
  nSecEditMenu = (int16)pDlgData->nEditMenuId;
  nEncryptOpt = pDlgData->nEncrypType;
  nAuthOpt = pDlgData->nAuthType;
  nTitleResID = pDlgData->nTitleResID;
  pSecResData = pDlgData->pSecFieldData;

  pSecEditMenu = (IMenuCtl *)IDIALOG_GetControl(pDialog, nSecEditMenu);
  pSecSKMenu = (IMenuCtl *)IDIALOG_GetControl(pDialog, nSKMenu);
  if ((! pSecEditMenu) || (! pSecSKMenu))
  {
    AUX_ERR("Failed to get menu 0x%x 0x%x", pSecEditMenu, pSecSKMenu, 0);
    return bRetVal;
  }

  if (pMe->m_WLAN.m_bWIFIOptsLoaded == FALSE)
  {
    nErrCode = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
    if (nErrCode != SUCCESS)
    {
      AUX_ERR("Failed loading WIFI options, %d", nErrCode, 0, 0);
      return bRetVal;
    }

    /* Set the encryption value */
    TmpOpt.nId = AEEWIFI_OPT_ENCRYPTION_TYPE;
    TmpOpt.pVal = (void *)nEncryptOpt;
    nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
    if (nErrCode != SUCCESS)
    {
      AUX_ERR("Could not set option %d: %d", TmpOpt.nId, nErrCode, 0);
      return bRetVal;
    }

    /* Set the authentication value */
    TmpOpt.nId = AEEWIFI_OPT_AUTHENTICATION_TYPE;
    TmpOpt.pVal = (void *)nAuthOpt;
    nErrCode = IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &TmpOpt);
    if (nErrCode != SUCCESS)
    {
      AUX_ERR("Could not set option %d: %d", TmpOpt.nId, nErrCode, 0);
      return bRetVal;
    }

  }

  pszwResStr = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
  pszResStr = (char *)MALLOC(MAX_STR_SIZE);
  if ((! pszwResStr) || (! pszResStr))
  {
    AUX_ERR("Could not malloc strings, %d %d", pszwResStr, pszResStr, 0);
    FREEIF(pszwResStr);
    FREEIF(pszResStr);
    return bRetVal;
  }

  for (i = 0; pSecResData[i].nStrResId != AUXAPP_SEC_FIELD_EDIT_END; i++)
  {

    bIsTextCorrect = FALSE;

    /* Use a do..while(0) loop to simplify cleanup in error cases */
    do
    {


      nErrCode = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      pSecResData[i].nStrResId, pszwResStr,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
      if (nErrCode == 0)
      {
        AUX_ERR("Failed loading string %d: %d", pSecResData[i].nStrResId,
                 nErrCode, 0);
        break;
      }

      nErrCode = IWIFIOpts_GetOpt(pMe->m_WLAN.m_pIWIFIOpts,
                                  pSecResData[i].nWIFIOptId, &TmpOpt);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Unable to load WIFI option %d, err %d",
                 pSecResData[i].nWIFIOptId, nErrCode, 0);
        break;
      }

      /* The WEP key ids stored in IWIFI are zero-based, but we display
       * them to the user as one-based.  Consequently, we need to
       * increment by one.
       */
#if defined(AEEWIFI_OPT_WEP_64BIT_KEY_ID) /* 'til we get it in a BREW build */
      if ((pSecResData[i].nWIFIOptId == AEEWIFI_OPT_WEP_64BIT_KEY_ID) ||
          (pSecResData[i].nWIFIOptId == AEEWIFI_OPT_WEP_128BIT_KEY_ID))
#else
      if (pSecResData[i].nWIFIOptId == AEEWIFI_OPT_WEP_KEY_ID)
#endif
      {
         TmpOpt.pVal = (void *)((uint32)TmpOpt.pVal + 1);
      }

      (void)WSTRTOSTR(pszwResStr, pszResStr, MAX_STR_SIZE);
      if (pSecResData[i].bIsNumeric)
      {
        (void)SNPRINTF(pszResStr, MAX_STR_SIZE, "%s: %d", pszResStr,
                       (uint32)TmpOpt.pVal);
        (void)STRTOWSTR(pszResStr, pszwResStr, MAX_STR_SIZE * sizeof(AECHAR));
      }
      else
      {
        (void)STRLCAT(pszResStr, ": ", MAX_STR_SIZE);
        (void)STRTOWSTR(pszResStr, pszwResStr, MAX_STR_SIZE * sizeof(AECHAR));
        (void)WSTRLCAT(pszwResStr, (AECHAR *)TmpOpt.pVal, MAX_STR_SIZE);
      }

      IMENUCTL_SetItemText(pSecEditMenu, pSecResData[i].nMenuItemId,
                           NULL, 0, pszwResStr);


      bIsTextCorrect = TRUE;

    } while (0);


    if (bIsTextCorrect == FALSE)
    {
      break;
    }
  }

  if (bIsTextCorrect == TRUE)
  {

    nErrCode = (int)IMENUCTL_SetTitle(pSecEditMenu, gpAUXResourceFile,
                                      nTitleResID, NULL);
    if ((boolean)nErrCode == FALSE)
    {
      AUX_ERR("Failed to set title: %d", nTitleResID, 0, 0);
    }
    else
    {
      pMe->m_WLAN.m_bWIFIOptsLoaded = TRUE;
      bRetVal = TRUE;
    }
  }

  if (bRetVal == TRUE)
  {
    AUXSetDefaultSoftkeyLook(pMe->a.m_pIShell, pSecSKMenu);
    (void)IMENUCTL_Redraw(pSecEditMenu);
    (void)IMENUCTL_Redraw(pSecSKMenu);
    (void)IDIALOG_SetFocus(pDialog, nSecEditMenu);
  }

  FREE(pszwResStr);
  FREE(pszResStr);

  return bRetVal;
}

/*===========================================================================
FUNCTION AUXWLANSecEditDlgHandler

DESCRIPTION
  Handle WLAN authentication settings dialog events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANSecEditDlgHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp   *pMe = (AUXApp *)pUser;
  IDialog    *pIActiveDialog;
  int         nReturnStatus;
  uint16      nDlgId;
  int16       nSecEditMenu;
  IMenuCtl   *pEditMenu;
  int         nErrCode;
  const AUXWLANMenu_SecEditStruct *pDlgData = NULL;
  int         i;

  if (pMe == NULL) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if (evt == EVT_DIALOG_START || evt == EVT_KEY || evt == EVT_KEY_PRESS ||
      evt == EVT_COMMAND) {
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  /* Get a pointer to the active dialog. */
  pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
  if (pIActiveDialog == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  nDlgId = IDIALOG_GetID(pIActiveDialog);

  for (i = 0; i < (int)ARR_SIZE(WLANSecEditData); i++)
  {
    if (nDlgId == WLANSecEditData[i].nDlgId)
    {
      pDlgData = &WLANSecEditData[i];
    }
  }

  if (! pDlgData)
  {
    AUX_ERR("Dlg data not found for dialog %d", nDlgId, 0, 0);
    return FALSE;
  }
  else if (! pDlgData->pSecFieldData)
  {
    AUX_ERR("NULL field edit data for dialog %d", nDlgId, 0, 0);
    return FALSE;
  }

  nSecEditMenu = (int16)pDlgData->nEditMenuId;

  pEditMenu = (IMenuCtl *)IDIALOG_GetControl(pIActiveDialog, nSecEditMenu);
  if (! pEditMenu)
  {
    AUX_ERR("Failed to get menu", 0, 0, 0);
    return FALSE;
  }

  if (pMe->m_WLAN.m_bAuthKeysUpdated == TRUE)
  {
    (void)InitWLANSecEditDlg(pMe, pIActiveDialog);
  }
  pMe->m_WLAN.m_bAuthKeysUpdated = FALSE;

  switch (evt) {
  case EVT_DIALOG_START:
  {
    AUX_MSG("WLAN Dlg %d: EVT_DIALOG_START", nDlgId, 0, 0);
    return InitWLANSecEditDlg(pMe, (IDialog*)dw);
  }

  case EVT_KEY:
    AUX_MSG ("WLAN Dlg %d: EVT_KEY", nDlgId, 0, 0);
    if (w != AVK_SELECT) {
      switch(w) {
      case AVK_CLR:
        nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
        if (nErrCode != SUCCESS)
        {
          AUX_ERR("Failed to end dialog, %d", nErrCode, 0, 0);
        }
        return TRUE;

      /* digit keys */
      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
      case AVK_STAR:
      case AVK_POUND:
      /* navigation keys */
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_UP:
      case AVK_DOWN:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* event handled */

      default:
        break;
      }
      return FALSE;
    }
    return TRUE;

  case EVT_COMMAND:
  {
    /* If one of the security menu items was selected, we need to launch
     * that dialog.  Else, if it's from the soft key menu, we need to
     * either save the options or cancel the action.  This code assumes
     * that the menu item values for the soft key menu and the security menu
     * are disjoint.  The resource editor pretty much enforces that, so
     * it should be a safe assumption.
     */
    uint16 wDlgToLaunch;

    AUX_MSG ("WLANSecEditDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    if (w == pDlgData->nSKCancel)
    {
      /* Reload the options to discard the changes. */
      nErrCode = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Failed loading WIFI options, %d", nErrCode, 0, 0);
      }

      /* Even if we couldn't revert the WIFI opt changes, end the dialog */
      nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Failed to end dialog, %d", nErrCode, 0, 0);
      }
      return TRUE;
    }
    else if (w == pDlgData->nSKSave)
    {
      nErrCode = ISHELL_EndDialog(pMe->a.m_pIShell);
      if (nErrCode != SUCCESS)
      {
        AUX_ERR("Could not end dialog, %d", nErrCode, 0, 0);
      }
      return TRUE;
    }

    switch (w)  /* "w" is the resource ID of the key that was selected. */
    {
      case IDL_WLAN_SEC_WEP64_KEY_ID:
        wDlgToLaunch = IDD_WLAN_SEC_WEP64_KEY_ID;
        break;
      case IDL_WLAN_SEC_WEP64_KEY1:
        wDlgToLaunch = IDD_WLAN_SEC_WEP64_KEY1;
        break;
      case IDL_WLAN_SEC_WEP64_KEY2:
        wDlgToLaunch = IDD_WLAN_SEC_WEP64_KEY2;
        break;
      case IDL_WLAN_SEC_WEP64_KEY3:
        wDlgToLaunch = IDD_WLAN_SEC_WEP64_KEY3;
        break;
      case IDL_WLAN_SEC_WEP64_KEY4:
        wDlgToLaunch = IDD_WLAN_SEC_WEP64_KEY4;
        break;
      case IDL_WLAN_SEC_WEP128_KEY_ID:
        wDlgToLaunch = IDD_WLAN_SEC_WEP128_KEY_ID;
        break;
      case IDL_WLAN_SEC_WEP128_KEY1:
        wDlgToLaunch = IDD_WLAN_SEC_WEP128_KEY1;
        break;
      case IDL_WLAN_SEC_WEP128_KEY2:
        wDlgToLaunch = IDD_WLAN_SEC_WEP128_KEY2;
        break;
      case IDL_WLAN_SEC_WEP128_KEY3:
        wDlgToLaunch = IDD_WLAN_SEC_WEP128_KEY3;
        break;
      case IDL_WLAN_SEC_WEP128_KEY4:
        wDlgToLaunch = IDD_WLAN_SEC_WEP128_KEY4;
        break;
      case IDL_WLAN_SEC_WPA_PSK_PASSWORD:
        wDlgToLaunch = IDD_WLAN_SEC_WPA_PSK_PASSWORD;
        break;
      case IDL_WLAN_SEC_WPA_TLS_CERT_USERID:
        wDlgToLaunch = IDD_WLAN_SEC_WPA_TLS_CERT_USERID;
        break;
      case IDL_WLAN_SEC_WPA_MD5_USERID:
        wDlgToLaunch = IDD_WLAN_SEC_WPA_MD5_USERID;
        break;
      case IDL_WLAN_SEC_WPA_MD5_PASSWORD:
        wDlgToLaunch = IDD_WLAN_SEC_WPA_MD5_PASSWORD;
        break;

      default:
        AUX_ERR("Unknown input, %d", w, 0, 0);
        return FALSE;
    }

    /* If we get here, the user selected one of the WEP key edit options,
     * so we need to launch the edit dialog.
     */
    nErrCode = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                   wDlgToLaunch, NULL);
    if (nErrCode != SUCCESS)
    {
      AUX_ERR("Unable to launch dialog %d: err %d", wDlgToLaunch,
               nErrCode, 0);
    }

    return TRUE;
  }

  case EVT_DIALOG_END:
    AUX_MSG ("WLAN Dlg %d: EVT_DIALOG_END", nDlgId, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell, AUXAPPTIMEOUT,
                                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                                    (uint32 *)pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    pMe->m_WLAN.m_bWIFIOptsLoaded = FALSE;
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End AUXWLANSecEditDlgHandler */

/*===========================================================================
FUNCTION AUXLanDlgEventHandler

DESCRIPTION
  handle LAN radio button dlg events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXLanDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch (evt) {
  case EVT_DIALOG_START:
    AUX_MSG ("LanDlg: EVT_DIALOG_START", 0, 0, 0);
    return AUXInitRadioButtonDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("LanDlg: EVT_KEY", 0, 0, 0);
    switch(w) {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

    /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
      return FALSE;

    default:
      return FALSE;
    }

  case EVT_COMMAND:
    AUX_MSG ("LanDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* user makes (new) selection */
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

    if (AUXUpdateSetting(pMe, w, (IMenuCtl*)dw))
    {
      if (w == IDC_IP)
      {
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                        gpAUXResourceFile, IDD_WLAN_STATIC_IP_CONFIG,
                                        NULL);
        return (AUXCheckAEEReturnStatus(nReturnStatus));
      }
      else
      {
        /* Let the update image be shown for 2 seconds before going back to
        the previous screen */
        nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                         AUX_SELECTIONVERIFICATIONTIME,
                         (PFNNOTIFY) AUXEndCurrentDlg,
                         (uint32*) pMe);
        return (AUXCheckAEEReturnStatus(nReturnStatus));

      }
    }
    else
    {
      IDialog* pIActiveDialog;
      uint16 wDlgID;
      uint16 wStringID = IDS_CHANGE_FAILED;

      /* Display appropriate error message */
      pIActiveDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
      if(pIActiveDialog == NULL)
      {
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }

      wDlgID = IDIALOG_GetID(pIActiveDialog);
      if (wDlgID == 0) { /*error checking */
        AUX_ERR ("Null pointer", 0,0,0);
        return FALSE;
      }
      wStringID = IDS_CHANGE_FAILED;
      AUXDisplayMessageDlg(pMe, wStringID, 3);
    }
    return TRUE;

  case EVT_USER:
    AUX_MSG ("LanDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("LanDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default: /* Other events that have not been handled by the handler */
    return FALSE;
  }
} /* End AUXLanDlgEventHandler */

/*===========================================================================
FUNCTION AUXConfigIpDlgEventHandler

DESCRIPTION
  handle events in the configure static IP dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXConfigIpDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  if (pMe->m_WLAN.m_bIPsUpdated)
  {
    /* If the IP address info has been updated in IWIFI, but not in pMe,
     * go load them again from IWIFI.
     */
    (void) InitConfigIpDlg(pMe, IDD_WLAN_STATIC_IP_CONFIG,
                         (IDialog*)ISHELL_GetActiveDialog(pMe->a.m_pIShell));
  }
  pMe->m_WLAN.m_bIPsUpdated = FALSE;

  switch(evt)
  {
    case EVT_DIALOG_START:
      AUX_MSG ("ConfigIpDlg: EVT_DIALOG_START", 0, 0, 0);
      return InitConfigIpDlg(pMe, w, (IDialog*)dw);

    case EVT_KEY:
      AUX_MSG ("ConfigIpDlg: EVT_KEY", 0, 0, 0);

    switch(w)
    {
    case AVK_CLR:
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      return AUXCheckAEEReturnStatus(nReturnStatus);

     /* digit keys */
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
    case AVK_STAR:
    case AVK_POUND:
    /* navigation keys */
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
    case AVK_SELECT:
    /* send key */
    case AVK_SEND:
    case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
    case AVK_MENU:
    case AVK_MUTE:
      return TRUE; /* ignore all other keys */

    case AVK_END:
    case AVK_INFO:  /* power down */
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
      return FALSE;


    default:
      break;
    }
    break;

  case EVT_COMMAND:
    AUX_MSG ("ConfigIpDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
    switch (w)
    {
      case IDC_MENU_CONFIGIP_IP:
        pMe->m_WLAN.m_IpTypeSelected = IPADDRESS;
        break;
      case IDC_MENU_CONFIGIP_SUBNET:
        pMe->m_WLAN.m_IpTypeSelected = SUBNETMASK;
        break;
      case IDC_MENU_CONFIGIP_GATEWAY:
        pMe->m_WLAN.m_IpTypeSelected = GATEWAY;
        break;
      case IDC_MENU_CONFIGIP_DNS:
        pMe->m_WLAN.m_IpTypeSelected = DNS;
        break;
      default:
        break;
    }
    nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                                        gpAUXResourceFile, IDD_EDIT_IP, NULL);
    return (AUXCheckAEEReturnStatus(nReturnStatus));

  case EVT_USER:
    AUX_MSG ("ConfigIpDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("ConfigIpDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default:
    return FALSE;
  }
  return FALSE;
}


/* The following macro determines the appropriate size in bytes for a
 * WSPRINTF call in constructing the WLAN statistics string.  The resulting
 * string should not exceed the length of the buffer
 * (AUX_WLAN_STATUS_TEXT_SIZE AECHARs).  It also should not exceed
 * MAX_STR_SIZE AECHARs for any one statistic.
 *
 * The macro argument is the current length (in AECHARs) of the entire
 * statistics string.
 */
#define AUX_WLAN_STAT_BUF_SIZE(nCurLen) \
  (int)(AUX_MIN_VAL( \
           (int)((AUX_WLAN_STATUS_NUM_CHARS - (uint32)(nCurLen)) * sizeof(AECHAR)), \
           (int)(MAX_STR_SIZE * sizeof(AECHAR))))

/*===========================================================================
FUNCTION AUXWLANMenu_GetWLANStatus

DESCRIPTION
  Constructs the status info to be displayed on the WLAN Status dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
AECHAR *AUXWLANMenu_GetWLANStatus(AUXApp *pMe)
{
  AECHAR *pszwStatInfo = NULL;
  AECHAR *pszwTmp = NULL;       /* Used to construct the display string. */
  AECHAR *pszwResStr = NULL;    /* String to load resources */
  AECHAR *pszwSSSFmt = NULL;    /* "Three-string" format string */
  AECHAR *pszwSSDFmt = NULL;    /* "Two strings and a number" format string */
  AECHAR *pszwSep = NULL;       /* Separator used in to build display string */
  int     nCurLen = 0;          /* Length of our stat info string */
  int     nResStatus;           /* Return value for loading status info */
  boolean bStringReady = FALSE; /* Was final string constructed? */
  AEECMSSInfo ssInfo;

  if (pMe == NULL)
  {
    AUX_ERR ("NULL object ptr", 0, 0, 0);
    return (AECHAR *)NULL;
  }

  /* Using do..while(0) for easier cleanup */
  do
  {

    pszwSep = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
    pszwTmp = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
    pszwStatInfo = (AECHAR *)MALLOC(AUX_WLAN_STATUS_NUM_CHARS *
                                    sizeof(AECHAR));
    if ((! pszwSep) || (! pszwTmp) || (! pszwStatInfo))
    {
      AUX_ERR("Could not malloc strings, %d %d %d", pszwSep, pszwTmp,
               pszwStatInfo);
      break;
    }

    pszwResStr = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
    pszwSSSFmt = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
    pszwSSDFmt = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
    if ((! pszwResStr) || (! pszwSSSFmt) || (! pszwSSDFmt))
    {
      AUX_ERR("Could not malloc strings, %d %d %d", pszwResStr, pszwSSSFmt,
               pszwSSDFmt);
      break;
    }

    /* Get the separator string */
    nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      IDS_GAP, pszwSep,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
    if (nResStatus == 0)
    {
      AUX_ERR("Failed loading string %d", IDS_GAP, 0, 0);
      break;
    }

    (void)STRTOWSTR("%s%s%s\n", pszwSSSFmt, MAX_STR_SIZE * sizeof(AECHAR));
    (void)STRTOWSTR("%s%s%d\n", pszwSSDFmt, MAX_STR_SIZE * sizeof(AECHAR));

    //get serving system info
    nResStatus = ICM_GetSSInfo(pMe->m_WLAN.m_pICM, &ssInfo, sizeof(AEECMSSInfo));
    if (nResStatus != AEE_SUCCESS)
    {
      AUX_ERR("Failed getting serving system info from ICM", 0, 0, 0);
      break;
    }

    /* Create the string for the currently-associated AP */
    nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      IDS_SSID, pszwResStr,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
    if (nResStatus == 0)
    {
      AUX_ERR("Failed loading string %d", IDS_SSID, 0, 0);
      break;
    }
    else
    {
      WSPRINTF(pszwStatInfo + nCurLen, AUX_WLAN_STAT_BUF_SIZE(nCurLen),
               pszwSSSFmt, pszwResStr, pszwSep,
               ssInfo.wlan_bss_info.ssid);
      nCurLen = WSTRLEN(pszwStatInfo);
      
    }

    /* Create the current Tx rate string */
    nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      IDS_WLAN_TX_RATE, pszwResStr,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
    if (nResStatus == 0)
    {
      AUX_ERR("Failed loading string %d", IDS_WLAN_TX_RATE, 0, 0);
      break;
    }
   
    WSPRINTF(pszwStatInfo + nCurLen, AUX_WLAN_STAT_BUF_SIZE(nCurLen),
             pszwSSDFmt, pszwResStr, pszwSep,
             ssInfo.wlan_stats.current_xmit_rate);
    nCurLen = WSTRLEN(pszwStatInfo);
    
    /* Append the total Tx bytes string */
    nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      IDS_WLAN_TOTAL_TX_BYTES, pszwResStr,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
    if (nResStatus == 0)
    {
      AUX_ERR("Failed loading string %d", IDS_WLAN_TOTAL_TX_BYTES, 0, 0);
      break;
    }
   
    WSPRINTF(pszwStatInfo + nCurLen, AUX_WLAN_STAT_BUF_SIZE(nCurLen),
             pszwSSDFmt, pszwResStr, pszwSep,
             ssInfo.wlan_stats.total_tx_bytes);
    nCurLen = WSTRLEN(pszwStatInfo);

    /* Append the total Rx bytes string */
    nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                      IDS_WLAN_TOTAL_RX_BYTES, pszwResStr,
                                      (MAX_STR_SIZE * sizeof(AECHAR)));
    if (nResStatus == 0)
    {
      AUX_ERR("Failed loading string %d", IDS_WLAN_TOTAL_RX_BYTES, 0, 0);
      break;
    }
    
    WSPRINTF(pszwStatInfo + nCurLen, AUX_WLAN_STAT_BUF_SIZE(nCurLen),
             pszwSSDFmt, pszwResStr, pszwSep,
             ssInfo.wlan_stats.total_rx_bytes);
    nCurLen = WSTRLEN(pszwStatInfo);

    bStringReady = TRUE;
  } while(0);

  FREEIF(pszwTmp);
  FREEIF(pszwResStr);
  FREEIF(pszwSSSFmt);
  FREEIF(pszwSSDFmt);
  FREEIF(pszwSep);
  if (bStringReady)
  {
    return pszwStatInfo;
  }
  else
  {
    FREEIF(pszwStatInfo);
    return NULL;
  }

}

/*===========================================================================
FUNCTION AUXApp_WLANStatusUpdateCB

DESCRIPTION
  Updates the WLAN Status screen

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/* Prototype for function we're about to use */
static boolean InitWLAN_StatusDlg(AUXApp *pMe, uint16 w, IDialog* pIDialog);

static void AUXApp_WLANStatusUpdateCB(void *pUser)
{

  AUXApp *pMe = (AUXApp *)pUser;
  IDialog  *pStatusDlg;

  if (pMe == NULL)
  {
    AUX_ERR("NULL object pointer", 0, 0, 0);
    return;
  }

  pStatusDlg = pMe->m_WLAN.m_pWLANStatusDlg;
  if (! pStatusDlg)
  {
    AUX_ERR("Status dialog is NULL", 0, 0, 0);
    return;
  }

  /* Now update the status screen. */
  (void)InitWLAN_StatusDlg(pMe, 0, pStatusDlg);

}

/*===========================================================================
FUNCTION InitWLAN_StatusDlg

DESCRIPTION
  Initializes and displays the status info on the WLAN Status dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean InitWLAN_StatusDlg(AUXApp *pMe, uint16 w, IDialog* pIDialog)
{
  IStatic  *pIStatic = NULL;
  IMenuCtl *pIMenuCtl = NULL;
  AECHAR   *pszwText = NULL;
  AECHAR   *pszwTitle = NULL;
  int       nResStatus;

  if ((pMe == NULL) || (pIDialog == NULL))
  {
    AUX_ERR("NULL ptr", 0, 0, 0);
    return FALSE;
  }
  pszwText = AUXWLANMenu_GetWLANStatus(pMe);
  pIStatic= (IStatic *) IDIALOG_GetControl (pIDialog, IDC_STATIC_WLAN_STATUS);
  pIMenuCtl = (IMenuCtl *) IDIALOG_GetControl(pIDialog, IDC_SOFTKEY_WLAN_STATUS);
  if ((pszwText == NULL) || (pIStatic == NULL) || (pIMenuCtl == NULL))
  {
    AUX_ERR("NULL Pointer, %d %d %d", pszwText, pIStatic, pIMenuCtl);
    FREEIF(pszwText);
    return FALSE;
  }

  /* Get the Title string for the status dialog */
  pszwTitle = (AECHAR *)MALLOC(MAX_STR_SIZE * sizeof(AECHAR));
  if (! pszwTitle)
  {
    AUX_ERR("Failed to alloc title buffer", 0, 0, 0);
    FREE(pszwText);
    return FALSE;
  }

  nResStatus = ISHELL_LoadResString(pMe->a.m_pIShell, gpAUXResourceFile,
                                    IDS_WLAN_STATUS, pszwTitle,
                                    (MAX_STR_SIZE * sizeof(AECHAR)));
  if (nResStatus == 0)
  {
    AUX_ERR("Failed loading string %d", IDS_WLAN_STATUS, 0, 0);
    FREE(pszwText);
    FREE(pszwTitle);
    return FALSE;
  }

  /* Set up the menu, so that the static will also be sized correctly. */
  AUXSetDefaultMenuLook(pMe->a.m_pIShell, pIMenuCtl);

  (void)ISTATIC_SetText(pIStatic, pszwTitle, pszwText, AEE_FONT_BOLD, AEE_FONT_NORMAL);
  (void) AUXSetStaticTextCtlSize(pIStatic, (IControl *)pIMenuCtl);
  ISTATIC_SetActive(pIStatic, FALSE);
  ISTATIC_SetProperties(pIStatic, ST_CENTERTITLE);

  (void) IMENUCTL_DeleteAll(pIMenuCtl);
  (void)IMENUCTL_AddItem(pIMenuCtl, gpAUXResourceFile, IDS_OK, IDL_SK_WLAN_STATUS_OK, NULL, 0);
  IMENUCTL_SetActive(pIMenuCtl, TRUE);

  pMe->m_WLAN.m_pWLANStatusDlg = pIDialog;
  (void)IDIALOG_AddRef(pMe->m_WLAN.m_pWLANStatusDlg);

  /* Set a timer to update the stats */
  (void)ISHELL_SetTimer(pMe->a.m_pIShell, AUX_WLAN_STATUS_UPDATE_TIME,
                        AUXApp_WLANStatusUpdateCB, (void *)pMe);

  FREE(pszwText);
  FREE(pszwTitle);
  return TRUE;
}

/*===========================================================================
FUNCTION AUXApp_WLANSelectEventHandler

DESCRIPTION
  handle events in the Network Select dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXApp_WLANSelectEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;
  AEECMWLANBSSINFO * pBssListItem = (AEECMWLANBSSINFO*)dw;

  int nWIFIErr;
  WIFIOpt tmp_Opt;
  AEECMPhInfo *pPhInfo;

  if ( pMe == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

  if ( pBssListItem == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      AUX_MSG ("WLANSelectEventHandler: EVT_DIALOG_START", 0, 0, 0);
      return InitWLAN_SelectDlg(pMe, w, (IDialog*)dw);

    case EVT_KEY:
      AUX_MSG ("EVT_KEY", 0, 0, 0);

      switch(w)
      {
        /* These keys cancel the request. */
        case AVK_CLR:
        case AVK_END:
        {
          /* disable the timer that will bring the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          pMe->m_WLAN.m_bWLANSearching = FALSE;
          AUXWLANUtil_CancelGetWLAN(pMe);
          return TRUE;
        }

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_INFO:  /* power down */
          /* disable the timer that will bring the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
          return FALSE;

        default:
          break;
      }
      return FALSE;

    case EVT_COMMAND:
    {
      pPhInfo = MALLOC(sizeof(AEECMPhInfo));
      if (pPhInfo == NULL)
      {
        AUX_ERR("Malloc PhInfo failed", 0, 0, 0);
        return FALSE;
      }  
      
      nReturnStatus = ICM_GetPhoneInfo(pMe->m_WLAN.m_pICM, pPhInfo, sizeof(AEECMPhInfo)); 
      if (nReturnStatus != AEE_SUCCESS)
      {
        AUX_ERR("Failed getting phone info from ICM", 0, 0, 0);
        FREEIF(pPhInfo);
        return TRUE;
      }  
      pMe->m_WLAN.m_WLANBandPref = pPhInfo->band_pref;
      FREEIF(pPhInfo);

      AUX_MSG ("WLANSelectEventHandler: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* disable the timer that shows users their newly selected option */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
      /* The w should be the item ID (array offset), and the dw should be
       * the pointer to the actual object. */
      if ((pMe->m_WLAN.m_pDetailedBSSList == NULL) ||
          (w >= pMe->m_WLAN.m_pDetailedBSSList->length))
      {
        // really bad!
        AUX_ERR("Out of bounds menu selection %d %x!", w, pMe->m_WLAN.m_pDetailedBSSList, 0);
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        return TRUE;
      }

      FREEIF(pMe->m_WLAN.m_psICMWLANData);
      pMe->m_WLAN.m_psICMWLANData = (AEECMWLANPref*)MALLOC(sizeof(AEECMWLANPref));

      if (pMe->m_WLAN.m_psICMWLANData == NULL) {
        AUX_ERR("Cannot malloc", 0, 0, 0);
        AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
        return TRUE;
      }

      MSG_HIGH("User selected entry %d", w, 0, 0);

      pMe->m_WLAN.m_psICMWLANData->network_sel_mode_pref = AEECM_NETWORK_SEL_MODE_PREF_MANUAL;
      pMe->m_WLAN.m_psICMWLANData->scan_mode = AEECM_WLAN_SCAN_PREF_NO_CHANGE;
      pMe->m_WLAN.m_psICMWLANData->bss_type = pBssListItem->bss_type;
      pMe->m_WLAN.m_psICMWLANData->bss_type_pref = AEECM_WLAN_BSS_TYPE_PREF_NO_CHANGE;
      pMe->m_WLAN.m_psICMWLANData->bss_id = pBssListItem->bss_id;
      pMe->m_WLAN.m_psICMWLANData->band = AUXWLANBandClassToPref(pBssListItem->band);
      pMe->m_WLAN.m_psICMWLANData->mob_ip_profile_id = 0;

      /* Fill in the IWIFI object with the current information. */
      {
        int i;
        boolean field_error = FALSE;
        const int fields[] = { AEEWIFI_OPT_SSID, AEEWIFI_OPT_CHANNEL,
                               AEEWIFI_OPT_BANDPREF, AEEWIFI_OPT_TECH };

        /* First, get all the options */
        nWIFIErr = IWIFI_LoadOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
        if (nWIFIErr != SUCCESS)
        {
          AUX_ERR("Failed loading WIFI options, %d", nWIFIErr, 0, 0);
          AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
          return TRUE;
        }
        else
        {

          for (i = 0; i < (int)ARR_SIZE(fields); i++)
          {
            tmp_Opt.nId = fields[i];
            field_error = FALSE;
            switch (fields[i])
            {
              case AEEWIFI_OPT_SSID:
                tmp_Opt.pVal = pBssListItem->ssid;
                (void)WSTRNCOPYN(pMe->m_WLAN.m_psICMWLANData->ssid, sizeof(pMe->m_WLAN.m_psICMWLANData->ssid),
                              pBssListItem->ssid, -1);
                break;
              case AEEWIFI_OPT_CHANNEL:
                tmp_Opt.pVal =
                   (void *)ConvertICMWLANChanToIWIFI(
                                   pBssListItem->chan,
                                   pBssListItem->band);
                if ((uint32)tmp_Opt.pVal == AUXAPP_BAD_WLAN_CHANNEL)
                {
                  AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
                  return TRUE;
                }
                pMe->m_WLAN.m_psICMWLANData->chan = pBssListItem->chan;
                break;
              case AEEWIFI_OPT_BANDPREF:
                {
                  /* Just set to "any", since the band pref from ICM is
                   * still invalid.
                   */
                  tmp_Opt.pVal = (void *)(uint32)AEEWIFI_BAND_ANY; //lint !e64 "Type mismatch"; casting ptr as uint32

                  /* At the moment, the band pref from ICM is bogus, so
                   * we just have to just add all WLAN band preferences.
                   */
                  pMe->m_WLAN.m_WLANBandPref |= AEECM_BAND_PREF_WLAN_ANY;
                }
                break;
              case AEEWIFI_OPT_TECH:
                tmp_Opt.pVal = (void *)(uint32)AEEWIFI_TECH_80211_ANY; 
                AEECM_WLAN_TECH_TO_TECH_PREF(pBssListItem->tech,pMe->m_WLAN.m_psICMWLANData->tech_pref);
                break;
              default:
                field_error = TRUE;
                break;
            }
            if (field_error) continue;

            if (SUCCESS != IWIFIOpts_AddOpt(pMe->m_WLAN.m_pIWIFIOpts, &tmp_Opt))
            {
              /* Copy the contents. */
              AUX_ERR("Could not set option %d", tmp_Opt.nId, 0, 0);
              AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
              return TRUE;
            }
          } /* for... WIFI fields */

          nWIFIErr = IWIFI_CommitOptions(pMe->m_WLAN.m_pIWIFI, 0, pMe->m_WLAN.m_pIWIFIOpts);
          if (SUCCESS != nWIFIErr)
          {
            AUX_ERR("Could not commit Options, %d", nWIFIErr, 0, 0);
            AUXDisplayMessageDlg(pMe, IDS_CHANGE_FAILED, 3);
            return TRUE;
          }
          /* No need to send subscription change for manual selection */
        }
        pMe->m_WLAN.m_bWLANManualScanAssociationRequest =  TRUE;
        nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                                            IDD_WLAN_SECURITY, NULL);
        if (nReturnStatus != SUCCESS) {
          AUX_ERR("CreateDlg Fail", 0, 0, 0);
          return FALSE;
        }
      }
    }
    return TRUE;

    case EVT_USER:
      AUX_MSG ("EVT_USER", 0, 0, 0);
      return FALSE;

    case EVT_DIALOG_END:
      AUX_MSG ("EVT_DIALOG_END", 0, 0, 0);
      pMe->m_WLAN.m_bWLANSearching = FALSE;
      AUXWLANUtil_CancelGetWLAN(pMe);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      AUXAPPTIMEOUT,
                      (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
      }
      pMe->m_WLAN.m_bWLANManualScanAssociationRequest =  FALSE;
      FREEIF(pMe->m_WLAN.m_psICMWLANData);
      if (pMe->m_bAppIsRunning)
      {
        AUXPopDlg(pMe, IDD_WLAN_SETTINGS);
      }
      return TRUE;

    default:
      return FALSE;
  }
}

/*==========================================================================
FUNCTION AUXWLANNoAssociationDlgEventHandler

DESCRIPTION
  handle events in the WLAN No Association Dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANNoAssociationDlgEventHandler(void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;


  if ( pMe == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* set the WLAN searching timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPP_WLAN_AUTOSCAN_NO_ASSOCIATION_TIME,
                    (PFNNOTIFY) AUXEndCurrentDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPP_WLAN_AUTOSCAN_NO_ASSOCIATION_TIME: %d", nReturnStatus, 0, 0);
    }
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      AUX_MSG ("WLANNoAssociationDlg: EVT_DIALOG_START", 0, 0, 0);
      return TRUE;

    case EVT_KEY:
      AUX_MSG ("WLANNoAssociationDlg: EVT_KEY", 0, 0, 0);

      switch(w)
      {
        /* These keys cancel the request. */
        case AVK_CLR:
        case AVK_END:
        {
          /* disable the timer that will bring the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          return TRUE;
        }

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_INFO:  /* power down */
          /* disable the timer that will bring the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
          return FALSE;

        default:
          break;
      }
      return FALSE;

    case EVT_COMMAND:
      return TRUE;

    case EVT_USER:
      return FALSE;

    case EVT_DIALOG_END:
      AUX_MSG ("WLANNoAssociationDlg: EVT_DIALOG_END", 0, 0, 0);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      AUXAPPTIMEOUT,
                      (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                      (uint32*) pMe);
      if (pMe->m_bAppIsRunning)
      {
        AUXPopDlg (pMe, IDD_WLAN_SETTINGS);
      }
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
      }
      return TRUE;

    default:
      return FALSE;
  }
}

/*===========================================================================
FUNCTION WLANAutoScanConnectFailed

DESCRIPTION
  Timer call back for WLAN auto scan connect failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WLANAutoScanConnectFailed(AUXApp *pMe )
{

  if (pMe == NULL) {
    AUX_ERR ("Null pointer", 0,0,0);
    return;
  }
  (void)ISHELL_CreateDialog(pMe->a.m_pIShell, gpAUXResourceFile,
                            IDD_WLAN_AUTOSCAN_NO_ASSOCIATION, NULL);
}


/*==========================================================================
FUNCTION AUXWLANAutoScanDlgEventHandler

DESCRIPTION
  Handle events in the WLAN Autoscan dialog, displays Searching message, sets a
  timer for Auto Scan association, pops dialog back to WLAN settings on end

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXWLANAutoScanDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{

  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;


  if ( pMe == NULL ) {
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  switch(evt)
  {
    case EVT_DIALOG_START:
      AUX_MSG ("WLANAutoScanDlg: EVT_DIALOG_START", 0, 0, 0);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
      /* set the WLAN searching timer */
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      AUXAPP_NO_WLAN_AP_FOUND_TIME,
                      (PFNNOTIFY) WLANAutoScanConnectFailed,
                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUXAPP_NO_WLAN_AP_FOUND_TIME: %d", nReturnStatus, 0, 0);
      }
      return TRUE;
    case EVT_KEY:
      AUX_MSG ("WLANAutoScanDlg: EVT_KEY", 0, 0, 0);

      switch(w)
      {
        /* These keys cancel the request. */
        case AVK_CLR:
        case AVK_END:
        {
          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          return TRUE;
        }

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        case AVK_SELECT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_INFO:  /* power down */
          return FALSE;

        default:
          break;
      }
      return FALSE;

    case EVT_COMMAND:
      return TRUE;

    case EVT_USER:
      return FALSE;

    case EVT_DIALOG_END:
      AUX_MSG ("WLANAutoScanDlg: EVT_DIALOG_END", 0, 0, 0);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) WLANAutoScanConnectFailed, pMe);
      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                                      AUXAPPTIMEOUT,
                                      (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
      }
      if (pMe->m_bAppIsRunning)
      {
        AUXPopDlg(pMe, IDD_WLAN_SETTINGS);
      }
      return TRUE;

    default:
      return FALSE;
  }
}

/*===========================================================================
FUNCTION AUXEditIpDlgEventHandler

DESCRIPTION
  handle events in the edit IP set of dialogs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXEditIpDlgEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  AUXApp * pMe = (AUXApp*) pUser;
  int nReturnStatus;


  if ( pMe == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch(evt)
  {
  case EVT_DIALOG_START:
    AUX_MSG ("EditIpDlg: EVT_DIALOG_START", 0, 0, 0);
    return InitEditIpDlg(pMe, w, (IDialog*)dw);

  case EVT_KEY:
    AUX_MSG ("EditIpDlg: EVT_KEY", 0, 0, 0);

    switch(w)
    {
      case AVK_CLR:
        /* disable the timer that will bring the user to the previous screen */
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

        nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
        if (nReturnStatus != SUCCESS)
        {
          AUX_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
        }
        return TRUE;

      /* digit keys */
      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
      case AVK_POUND:
      /* navigation keys */
      case AVK_UP:
      case AVK_DOWN:
      case AVK_LEFT:
      case AVK_RIGHT:
      case AVK_SELECT:
      /* send key */
      case AVK_SEND:
      case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
      case AVK_MENU:
      case AVK_MUTE:
        return TRUE; /* ignore all other keys */

      case AVK_STAR:
      {
  #define MAX_IP_SIZE 15
        AECHAR buffer[MAX_IP_SIZE+1];
        AECHAR dest_buffer[MAX_IP_SIZE+1];
        int    total_length;
        int    position;
        IDialog  *p_dialog;
        ITextCtl *ptext;

        /* Since input type is "numbers", we use AVK_STAR to indicate the
         * "." in the IP address. When the user presses "*", put a "."
         * on the screen.
         */
        p_dialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        ptext = (ITextCtl *) IDIALOG_GetControl (p_dialog, IDC_TEXT_EDIT_IP);

        (void) ITEXTCTL_GetText(ptext, buffer, MAX_IP_SIZE);
        position = ITEXTCTL_GetCursorPos(ptext);
        if ((position >= 0) && (position < MAX_IP_SIZE))
        {
          total_length = WSTRLEN(buffer);
          if (total_length < MAX_IP_SIZE)
          {
            /* Copy from user input buffer to dest_buffer up to the current
             * cursor position, reserving a space for the ".".  That's why
             * we don't use (MAX_IP_SIZE + 1).
             */
            (void) WSTRNCOPYN(dest_buffer, MAX_IP_SIZE, buffer, position);
            dest_buffer[position] = (AECHAR) '.';

            /* Copy all remaining characters in the user input buffer to
             * dest_buffer.  The number of characters currently in dest_buffer
             * is (position +1)
             */
            (void) WSTRNCOPY(dest_buffer+(position+1),
                            (MAX_IP_SIZE+1)-(position+1), buffer+position);

            /* Set the text; add 1 to the length, since we just added a '.' */
            (void) ITEXTCTL_SetText(ptext, dest_buffer, total_length+1);
            ITEXTCTL_SetCursorPos(ptext, position+1);
          }
        }
        return TRUE;
      }

      case AVK_END:
      case AVK_INFO:  /* power down */
        /* disable the timer that will bring the user to the previous screen */
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
        return FALSE;

      default:
        break;
    }
    break;

  case EVT_COMMAND:
    AUX_MSG ("EditIpDlg: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
    /* disable the timer that shows users their newly selected option */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
    switch (w)
    {
      case IDL_SK_EDIT_IP_OK:
        if (SaveIP(pMe) == TRUE)
        {
          pMe->m_WLAN.m_bIPsUpdated = TRUE;
        }
        break;
      case IDL_SK_EDIT_IP_CANCEL:
        break;
      default:
        break;
    }
    nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
    return TRUE;

  case EVT_USER:
    AUX_MSG ("ConfigIpDlg: EVT_USER", 0, 0, 0);
    return FALSE;

  case EVT_DIALOG_END:
    AUX_MSG ("ConfigIpDlg: EVT_DIALOG_END", 0, 0, 0);
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
    return TRUE;

  default:
    return FALSE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION AUXApp_WLANStatusEventHandler

DESCRIPTION
  handle events in the WLAN status dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean AUXApp_WLANStatusEventHandler (void *pUser, AEEEvent evt, uint16 w, uint32 dw)
{
  int nReturnStatus;
  AUXApp * pMe = (AUXApp*) pUser;

  if ( pMe == NULL ) { /* error checking */
    AUX_ERR ("Null pointer", 0,0,0);
    return FALSE;
  }

#ifdef FEATURE_BREW_3_0
  // do not want to handle autorepeat key events
  if((evt == EVT_KEY) && ((dw & KB_AUTOREPEAT) != 0))
  {
    return FALSE;
  }
#endif

  /* Reset Timer */
  if(evt == EVT_DIALOG_START || evt == EVT_KEY_PRESS || evt == EVT_COMMAND) {
    /* disable the timer that brings screen back to idle */
    (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);
    /* reset the idle screen timer */
    nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                    AUXAPPTIMEOUT,
                    (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                    (uint32*) pMe);
    if (nReturnStatus != SUCCESS)
    {
      AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
    }
  }

  switch(evt)
  {
    case EVT_DIALOG_START:
      AUX_MSG ("WLANStatusEventHandler: EVT_DIALOG_START", 0, 0, 0);
      return InitWLAN_StatusDlg(pMe, w, (IDialog*)dw);

    case EVT_COMMAND:
      AUX_MSG ("WLANStatusEventHandler: EVT_COMMAND, w=%d, dw=%d", w, dw, 0);
      /* disable the timer that will bring the user to the previous screen */
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

      nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
      }
      return TRUE;

    case EVT_KEY_PRESS:
    case EVT_KEY_RELEASE:
      return TRUE;

    case EVT_KEY:
      AUX_MSG ("WLANStatusEventHandler: EVT_KEY", 0, 0, 0);

      switch(w)
      {
        /* These keys cancel the request. */
        case AVK_CLR:
        case AVK_END:
        {

          /* disable the timer that brings the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);

          if (pMe->m_WLAN.m_pWLANStatusDlg)
          {
            (void)IDIALOG_Release(pMe->m_WLAN.m_pWLANStatusDlg);
            pMe->m_WLAN.m_pWLANStatusDlg = NULL;
            (void)ISHELL_CancelTimer(pMe->a.m_pIShell, AUXApp_WLANStatusUpdateCB,
                               (void *)pMe);
          }

          nReturnStatus = ISHELL_EndDialog(pMe->a.m_pIShell);
          if (nReturnStatus != SUCCESS)
          {
            AUX_ERR("Fail to end dialog: %d", nReturnStatus, 0, 0);
          }
          return TRUE;
        }

        /* digit keys */
        case AVK_0:
        case AVK_1:
        case AVK_2:
        case AVK_3:
        case AVK_4:
        case AVK_5:
        case AVK_6:
        case AVK_7:
        case AVK_8:
        case AVK_9:
        case AVK_STAR:
        case AVK_POUND:
        /* navigation keys */
        case AVK_UP:
        case AVK_DOWN:
        case AVK_LEFT:
        case AVK_RIGHT:
        /* send key */
        case AVK_SEND:
        case AVK_MESSAGE:  /* message, menu and mute not sure..!!!! */
        case AVK_MENU:
        case AVK_MUTE:
          return TRUE; /* ignore all other keys */

        case AVK_INFO:  /* power down */
          /* disable the timer that will bring the user to the previous screen */
          (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXEndCurrentDlg, pMe);
          return FALSE;

        default:
          break;
      }
      return FALSE;

    case EVT_USER:
      AUX_MSG ("WLANStatusEventHandler: EVT_USER", 0, 0, 0);
      return FALSE;

    case EVT_DIALOG_END:
      AUX_MSG ("WLANStatusEventHandler: EVT_DIALOG_END", 0, 0, 0);
      (void)ISHELL_CancelTimer(pMe->a.m_pIShell, (PFNNOTIFY) AUXDisplayIdleMenuDlg, pMe);

      if (pMe->m_WLAN.m_pWLANStatusDlg)
      {
        (void)IDIALOG_Release(pMe->m_WLAN.m_pWLANStatusDlg);
        pMe->m_WLAN.m_pWLANStatusDlg = NULL;
        (void)ISHELL_CancelTimer(pMe->a.m_pIShell, AUXApp_WLANStatusUpdateCB,
                           (void *)pMe);
      }

      nReturnStatus = ISHELL_SetTimer(pMe->a.m_pIShell,
                      AUXAPPTIMEOUT,
                      (PFNNOTIFY) AUXDisplayIdleMenuDlg,
                      (uint32*) pMe);
      if (nReturnStatus != SUCCESS)
      {
        AUX_ERR("Fail to set AUXAPPTIMEOUT: %d", nReturnStatus, 0, 0);
      }
      return TRUE;

    default:
      return FALSE;
  }
}

/*===========================================================================
FUNCTION AUXGetWLANEditDlgTbl

DESCRIPTION
  The function is used to return WLANEditDlgTbl based on wDlgID
DEPENDENCIES
  
SIDE EFFECTS
  None
===========================================================================*/
const WLANEditDlgDataType* AUXGetWLANEditDlgTbl(uint16 wDlgID)
{

  int i;
  int nCount;

  if ( wDlgID == 0 ) {
    return WLANEditDlgTbl;
  }

  nCount = sizeof(WLANEditDlgTbl)/sizeof(WLANEditDlgTbl[0]);
  for (i = 0; i< nCount; i++) {
    if (WLANEditDlgTbl[i].nDlgId == wDlgID) {
      return &WLANEditDlgTbl[i];
    }
  }
  AUX_ERR( "AUXGetWLANEditDlgTbl(): No matching edit ctl", 0, 0, 0);
  return NULL; /* error */

} /* End AUXGetWLANEditDlgTbl */






